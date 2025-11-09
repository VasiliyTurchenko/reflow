/** @file my_comm.c
 *  @brief
 *
 *  @author turchenkov@gmail.com
 *  @bug
 * @date    30-12-2017
 * @modified 20-Jan-2019
 */
/*! file my_comm.c
 * Copyright (c) 2025-11-08 turchenkov@gmail.com
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software and
 * associated documentation files (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge, publish, distribute,
 * sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
 * PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR
 * OTHER LIABILITY,  WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR
 * IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include <string.h>

#include "my_comm.h"
#include "unused.h"
#include "platform_time_util.h"
#include "freertos_exported.h"
#include "error_handler.h"

typedef enum locks {
    STATE_UNLOCKED = 0,
    STATE_LOCKED,
} my_comm_lock_t;

/**
 * @brief out_stream_t
 */
typedef struct out_stream_tag {
    uni_vect_t init_vect; ///< initial value is kept here
    uint8_t   *pTxBuf1;   ///< first buffer
    uint8_t   *pTxBuf2;   ///< second buffer
    size_t     bufsize;
    uint8_t   *pActTxBuf;  ///< pointer to the buffer which is used for text output by myxfunc_out()
    uint8_t   *pXmitTxBuf; ///< pointer to the buffer which is being transmitted out
    size_t     TxTail;     ///< index of the first free byte in the active buffer
    volatile my_comm_lock_t
            ActBufState; ///< this variable is a mutex which locks access to the active buffer
    volatile my_comm_lock_t
                  XmitState; ///< this variable is a mutex which locks access to the transmit buffer
    volatile bool TransmitFuncRunning; ///< flag is set when transmission is on the run
    size_t        MaxTail;             ///< some statistic
    volatile ErrorStatus XmitError;

    ostream_sink_functions_t sink_fun;
    int32_t                  sink_handle;

#ifdef WITH_RTOS
    /* Tx buffers access mutex */
    /* defined in freertos.c */
    osMutexId  xfunc_out_MutexHandle;
    osThreadId comm_taskHandle;
#endif

    uint32_t    fullBufferTransmitTime_ms;
    uint32_t    LastTransmitStartTime_ms;

} out_stream_t;

/* currently only one stream exists */
static out_stream_t out_stream;
/* pointer to the active output stream */
static out_stream_t *p_act_outstream = NULL;

/* complete transmit callback */
static void o_stream_cb(void *arg);

/******************************** initialisation functions ********************/

/**
 * @brief reset_o_stream
 * @param o_s
 */
static void reset_o_stream(out_stream_t *o_stream)
{
    o_stream->TxTail      = 0U;
    o_stream->MaxTail     = 0U;
    o_stream->pActTxBuf   = o_stream->pTxBuf1;
    o_stream->pXmitTxBuf  = o_stream->pTxBuf2;
    o_stream->ActBufState = STATE_UNLOCKED;
    o_stream->XmitState   = STATE_UNLOCKED;
}

static inline _Bool check_fun_list(ostream_sink_functions_t *fl)
{
    return ((NULL != fl->deinit_sink) && (NULL != fl->init_sink) && (NULL != fl->send_bytes));
}

/**
 * @brief InitComm_impl
 * @param buffer pointer to the doublebuffer object
 * @param o_stream pointer to the out_stream_t object
 * @param fun_list pointer to the sink functions
 * @param sink_init_params pointer to the sink init parameters
 * @return ERROR or SUCCESS
 */
static ErrorStatus InitComm_impl(uni_vect_t buffer, out_stream_t *o_stream,
                                 ostream_sink_functions_t     *fun_list,
                                 const sink_init_parameters_t *sink_init_params)
{
    ErrorStatus result = ERROR;

    if (NULL == o_stream) {
        return result;
    }
    memset(o_stream, 0, sizeof(*o_stream));

    if ((NULL == fun_list) && (!check_fun_list(fun_list))) {
        return result;
    }

    if (!uni_vect_is_valid(&buffer) || (buffer.capacity < 32U)) {
        return result;
    }
    uni_vector_clear(&buffer); ///< already checked

    uint8_t *p0 = &buffer.pdata[0];
    uint8_t *p1 = &buffer.pdata[buffer.capacity / 2U];

    o_stream->sink_fun  = *fun_list;
    o_stream->init_vect = buffer;
    o_stream->pTxBuf1   = p0;
    o_stream->pTxBuf2   = p1;
    o_stream->bufsize   = buffer.capacity / 2U;

    o_stream->xfunc_out_MutexHandle = NULL;
    o_stream->comm_taskHandle       = NULL;

    // time calculation
    o_stream->fullBufferTransmitTime_ms = 1 + ((1000U * o_stream->bufsize * 10) / sink_init_params->baud_rate);

    reset_o_stream(o_stream);

    p_act_outstream = o_stream;

    o_stream->sink_handle =
            o_stream->sink_fun.init_sink(sink_init_params, o_stream_cb, (void *)o_stream);
    if (o_stream->sink_handle > 0) {
        result = SUCCESS;
    }
    extern _Bool Transmit_non_RTOS;
    Transmit_non_RTOS = true;
    extern void (*xfunc_out)(unsigned char);
    xfunc_out = myxfunc_out_no_RTOS;

    return result;
}

/**
 * @param buffer pointer to the doublebuffer object
 * @param fun_list pointer to the sink functions
 * @param sink_init_params pointer to the sink init parameters
 * @return ERROR or SUCCESS
 */
ErrorStatus InitComm(uni_vect_t buffer, ostream_sink_functions_t *fun_list,
                     const sink_init_parameters_t *sink_init_params)
{
    return InitComm_impl(buffer, &out_stream, fun_list, sink_init_params);
}
/* end of the function  InitComm */

#define ENTER_CRITICAL_SECTION_NO_RTOS()                                                           \
    do {                                                                                           \
        uint32_t sreg_temp = __get_PRIMASK();                                                      \
        __disable_irq();
#define LEAVE_CRITICAL_SECTION_NO_RTOS()                                                           \
    __set_PRIMASK(sreg_temp);                                                                      \
    }                                                                                              \
    while (0)

/**
 * @brief myxfunc_out_dummy does nothing
  * @note   helper for xprintf
  * @param  char
  * @retval none
  */
void myxfunc_out_dummy(unsigned char c)
{
    (void)c;
}

/**
 * @brief add2buf
 * @param c
 */
static inline void add2buf(uint8_t c)
{
    if (p_act_outstream->TxTail < p_act_outstream->bufsize) {
        p_act_outstream->pActTxBuf[p_act_outstream->TxTail] = c;
        p_act_outstream->TxTail++;
    }
    if (p_act_outstream->TxTail > p_act_outstream->MaxTail) {
        p_act_outstream->MaxTail++;
    }
}

/**
 * @brief myxfunc_out_no_RTOS puts the char to the xmit buffer
  * @note   helper for xprintf
  * @param  char
  * @retval none
  */
void myxfunc_out_no_RTOS(unsigned char c)
{
    // check last tx time
    if (time_now_ms() - p_act_outstream->LastTransmitStartTime_ms > p_act_outstream->fullBufferTransmitTime_ms) {
        Transmit();
    }

    ENTER_CRITICAL_SECTION_NO_RTOS();
    if (p_act_outstream->ActBufState == STATE_UNLOCKED) {
        p_act_outstream->ActBufState = STATE_LOCKED;
        add2buf(c);
        p_act_outstream->ActBufState = STATE_UNLOCKED;
    }
    LEAVE_CRITICAL_SECTION_NO_RTOS();

    //TODO add check and transmit
}

#ifdef WITH_RTOS
/**
  * @brief  myxfunc_out_RTOS puts the char to the xmit buffer
  * @note   helper for xprintf
  * @param  char
  * @retval none
  */
void myxfunc_out_RTOS(unsigned char c)
{
    while (osMutexWait(p_act_outstream->xfunc_out_MutexHandle, pdMS_TO_TICKS(1U)) != osOK) {
        /* stub */
    }
    add2buf(c);
    RESULT_UNUSED osMutexRelease(p_act_outstream->xfunc_out_MutexHandle);
}

/**
 * @brief my_comm_switch_to_RTOS_func_impl
 * @param o_s which stream should be switched
 */
static void my_comm_switch_to_RTOS_func_impl(out_stream_t *o_s)
{
    _Bool please_quit = false;
    do {
        Transmit();
        ENTER_CRITICAL_SECTION_NO_RTOS();
        if (o_s->XmitState == STATE_UNLOCKED) {
            if (o_s->TxTail == 0U) {
                please_quit = true;
                extern _Bool Transmit_non_RTOS; ///< used in non-rtos isr
                Transmit_non_RTOS = false;
                extern void (*xfunc_out)(unsigned char);
                xfunc_out = myxfunc_out_RTOS;
                reset_o_stream(o_s);
            }
        }
        LEAVE_CRITICAL_SECTION_NO_RTOS();
        if (!please_quit) {
            uint32_t t0 = time_now_ms();
            uint32_t t1 = t0 + o_s->fullBufferTransmitTime_ms / 2U;
            while (time_now_ms() < t1) {
                /*  */
            };
        }
    } while (!please_quit);
    o_s->LastTransmitStartTime_ms = 0U;
}

/**
 * @brief my_comm_update_mutexes
 */
void my_comm_update_mutexes(void)
{
    if (NULL == xfunc_out_MutexHandle) {
        Error_Handler();
    }
    p_act_outstream->xfunc_out_MutexHandle = xfunc_out_MutexHandle;

    if (NULL == comm_taskHandle) {
        Error_Handler();
    }
    p_act_outstream->comm_taskHandle = comm_taskHandle;
}

/**
 * @brief my_comm_switch_to_RTOS_func
 */
void my_comm_switch_to_RTOS_func(void)
{
    my_comm_switch_to_RTOS_func_impl(p_act_outstream);
}

#endif

/******************************** transmit functions ********************/

/**
 * @brief Transmit_impl
 * @param o_stream pointer to the out_stream_t object
 * @return ERROR or SUCCESS
 */
static ErrorStatus Transmit_impl(out_stream_t *const o_stream)
{
    ErrorStatus result = ERROR;
    do {
        if (o_stream->TransmitFuncRunning) {
            result = SUCCESS; /* this is not an error */
            break;
        }

        o_stream->TransmitFuncRunning = true;

        if (xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED) {
            /* Error - the function was called under RTOS! */
            break;
        }

        /* flag indicates "exit now" condition appeared inside critical section */
        _Bool  need_exit = false;
        size_t tmptail   = o_stream->TxTail;

        ENTER_CRITICAL_SECTION_NO_RTOS();

        if ((o_stream->XmitState != STATE_UNLOCKED) || (o_stream->ActBufState == STATE_LOCKED) ||
            (o_stream->TxTail == (size_t)0U)) {
            /* usart didn't finish the transmission yet OR outfunc is runnung OR  active buffer is empty */
            need_exit = true;
        } else {
            /* there is something to transmit */
            o_stream->XmitState   = STATE_LOCKED; // set lock
            o_stream->ActBufState = STATE_LOCKED; // lock access to the  active buffer

            if (o_stream->pActTxBuf == o_stream->pTxBuf1) {
                o_stream->pActTxBuf  = o_stream->pTxBuf2;
                o_stream->pXmitTxBuf = o_stream->pTxBuf1;
            } else if (o_stream->pActTxBuf == o_stream->pTxBuf2) {
                o_stream->pActTxBuf  = o_stream->pTxBuf1;
                o_stream->pXmitTxBuf = o_stream->pTxBuf2;
            } else {
                // wrong act buf pointer!!
                o_stream->XmitState = STATE_UNLOCKED;
                need_exit           = true;
            }
            o_stream->TxTail = 0U; // RESET index
        }
        LEAVE_CRITICAL_SECTION_NO_RTOS();

        o_stream->ActBufState = STATE_UNLOCKED;

        if (!need_exit) { /* here all the conditions are OK. let's send! */

            const size_t actual = o_stream->sink_fun.send_bytes(o_stream->pXmitTxBuf, tmptail);
            result              = (actual != tmptail) ? ERROR : SUCCESS;
            o_stream->LastTransmitStartTime_ms = time_now_ms();
        }
    } while (false);

    o_stream->TransmitFuncRunning = false;
    return result;
}

/**
 * @brief Transmit
 * @return ERROR or SUCCESS
 */
ErrorStatus Transmit(void)
{
    return Transmit_impl(p_act_outstream);
}

/* end of the function Transmit() */

#ifdef WITH_RTOS
/**
 * @brief Transmit_RTOS_impl
 * @param o_stream pointer to the out_stream_t object
 * @return ERROR or SUCCESS
 */
static ErrorStatus Transmit_RTOS_impl(out_stream_t *const o_stream)
{
    ErrorStatus result = ERROR;
    do {
        if (o_stream->TxTail == (size_t)0U) {
            result = SUCCESS;
            break; /* nothing to do */
        }

        /* temporary raise task priority */
        UBaseType_t task_prio;
        task_prio = uxTaskPriorityGet(o_stream->comm_taskHandle);
        /* set the new maximal priority */
        vTaskPrioritySet(o_stream->comm_taskHandle,
                         ((UBaseType_t)configMAX_PRIORITIES - (UBaseType_t)1U));

        /* take the mutex */
        if (osMutexWait(o_stream->xfunc_out_MutexHandle, 0) != osOK) {
            vTaskPrioritySet(o_stream->comm_taskHandle, task_prio);
            result = SUCCESS;
            break; /* try next time*/
        }

        /* flag indicates "exit now" condition appeared inside critical section */
        _Bool  need_exit = false;
        size_t tmptail   = o_stream->TxTail;

        if (o_stream->pActTxBuf == o_stream->pTxBuf1) {
            o_stream->pActTxBuf  = o_stream->pTxBuf2;
            o_stream->pXmitTxBuf = o_stream->pTxBuf1;
        } else if (o_stream->pActTxBuf == o_stream->pTxBuf2) {
            o_stream->pActTxBuf  = o_stream->pTxBuf1;
            o_stream->pXmitTxBuf = o_stream->pTxBuf2;
        } else {
            /* error */
            need_exit = true;
        }

        o_stream->TxTail = 0U;
        RESULT_UNUSED osMutexRelease(o_stream->xfunc_out_MutexHandle);
        vTaskPrioritySet(o_stream->comm_taskHandle, task_prio);

        if (need_exit) {
            break;
        }

        /* here all the conditions are OK. let's send! */
        const size_t actual = o_stream->sink_fun.send_bytes(o_stream->pXmitTxBuf, tmptail);
        result              = (actual != tmptail) ? ERROR : SUCCESS;

        /* we have to wait a notification ! */
        static uint32_t notified_val = 0U;
        if (xTaskNotifyWait(ULONG_MAX, ULONG_MAX, &notified_val, portMAX_DELAY) == pdTRUE) {
            if (notified_val != 1U) {
                result = ERROR;
            }
        }
    } while (false);

    return result;
}

/**
 * @brief Transmit_RTOS_impl
 * @return ERROR or SUCCESS
 */
ErrorStatus Transmit_RTOS(void)
{
    return Transmit_RTOS_impl(p_act_outstream);
}

/* end of the function Transmit_RTOS() */
#endif

/**
 * @brief o_stream_cb
 */
static void o_stream_cb(void *arg)
{
    out_stream_t *o_stream = (out_stream_t *)arg;

    o_stream->XmitState = STATE_UNLOCKED; /* used in the non-RTOS calls */
#ifdef WITH_RTOS
    /* usart1 IRQ priority must be lower than MAX_SYSCALL_...._PRIORITY */
    BaseType_t xHigherPriorityTaskWoken;

    if (xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED) {
        if (xTaskNotifyFromISR(o_stream->comm_taskHandle, 1U, eSetValueWithOverwrite,
                               &xHigherPriorityTaskWoken) != pdPASS) {
            // error
        }
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }
#endif
}

#ifdef WITH_RTOS

#endif

/* ############################### end of file ############################### */
