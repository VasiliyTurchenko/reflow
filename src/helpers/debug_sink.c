/** @file debug_sink.c
 *  @brief
 *
 *  @author turchenkov@gmail.com
 *  @bug
 *  @date 2019-01-01
 */
/*! file debug_sink.c
 * Copyright (c) 2019-01-01 turchenkov@gmail.com
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

#include <stdint.h>
#include <stddef.h>

#include "usart.h"
#include "unused.h"
#include "error_handler.h"

#include "debug_sink.h"
#include "my_comm.h"

_Bool         Transmit_non_RTOS = false; ///< used in non-rtos isr
static void  *tc_callbak_param  = NULL;
static tc_fun tc_callback       = NULL;


static int32_t usart_init(const sink_init_parameters_t *init_params, tc_fun callback,
                          void *callback_param); ///< open real sink (ser. port, etc.)

static int32_t usart_deinit(int32_t h); ///< close the sink

static size_t usart_send(uint8_t *pbytes, size_t nb); ///< actual transmit function

/* */
static ostream_sink_functions_t deb_ostream = { .deinit_sink = usart_deinit,
                                                .init_sink   = usart_init,
                                                .send_bytes  = usart_send };

/* */

/* 2 * 1K buffers */
static uint8_t    debug_sink_buffer[2048U];
static uni_vect_t dsb = { .capacity = 2048U, .data_len = 0U, .pdata = debug_sink_buffer };

/**
 * @brief debug_sink_init
 */
void debug_sink_init(void)
{
    static const sink_init_parameters_t p = {.baud_rate = DEBUG_UART_BAUDRATE};
    if (ERROR == InitComm(dsb, &deb_ostream, &p)) {
        Error_Handler();
    }
}

/**
 * @brief deb_usart_tc_cb
 */
static void deb_usart_tc_cb(void)
{
    if (NULL != tc_callback) {
        tc_callback(tc_callbak_param);
    }
}

/**
 * @brief usart_init
 * @param init_params
 * @param callback
 * @param callback_param
 * @return
 */
static int32_t usart_init(const sink_init_parameters_t *init_params, tc_fun callback, void *callback_param)
{
    ARG_UNUSED init_params;
    assert_param(init_params != NULL);
    DEBUG_USART_Init(deb_usart_tc_cb, init_params->baud_rate);
    tc_callbak_param = callback_param;
    tc_callback      = callback;
    return (int32_t)&DEBUG_huart;
}

/**
 * @brief usart_deinit
 * @param h
 * @return
 */
static int32_t usart_deinit(int32_t h)
{
    UART_HandleTypeDef *huart = (UART_HandleTypeDef *)h;

    if (huart == &DEBUG_huart) {
        DEBUG_USART_DeInit();
        tc_callbak_param = NULL;
        tc_callback      = NULL;
        return 0;
    }
    return -1;
}

/**
 * @brief usart_send
 * @param pbytes
 * @param nb
 * @return
 */
static size_t usart_send(uint8_t *pbytes, size_t nb)
{
    HAL_StatusTypeDef XmitStatus;
    XmitStatus = HAL_UART_Transmit_DMA(&DEBUG_huart, pbytes, (uint16_t)nb);
    if (XmitStatus != HAL_OK) {
        return 0U;
    }
    return nb;
}

/**
 * @brief debug_sink_switch_to_RTOS
 */
void debug_sink_switch_to_RTOS(void)
{
    my_comm_switch_to_RTOS_func();
}

/**
 * @brief debug_sink_update_mutexes
 */
void debug_sink_update_mutexes(void)
{
    my_comm_update_mutexes();
}
