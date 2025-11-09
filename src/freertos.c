/** @file freertos.c
 *  @brief
 *
 *  @author turchenkov@gmail.com
 *  @bug
 *  @date 2025-11-08
 */
/*! file freertos.c
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

#include "FreeRTOS.h"

#include "semphr.h"
#include "queue.h"
#include "task.h"

#include "spi.h"

#include "hal_ll.h"

#include "freertos_exported.h"
#include "platform_time_util.h"

#include "stack_checker.h"

#include "comm_task.h"
#include "kbd_task.h"
#include "ui_task.h"
#include "control_task.h"
#include "logging.h"
#include "framebuffer.h"

#define ATTR_CCMRAM

typedef struct {
    task_stack_info_t tinf; // used by the stack checker
    // add more...
} task_context_t;

/// Attributes structure for thread.
typedef struct {
    const char *name;       ///< name of the thread
    uint32_t    attr_bits;  ///< attribute bits
    void       *cb_mem;     ///< memory for control block
    uint32_t    cb_size;    ///< size of provided memory for control block
    void       *stack_mem;  ///< memory for stack
    uint32_t    stack_size; ///< size of stack
    UBaseType_t priority;   ///< initial thread priority (default: osPriorityNormal)
    uint32_t    tz_module;  ///< TrustZone module identifier
    uint32_t    reserved;   ///< reserved (must be 0)
} osThreadAttr_t;

/// Priority values.
typedef enum {
    osPriorityNone        = 0,         ///< No priority (not initialized).
    osPriorityIdle        = 1,         ///< Reserved for Idle thread.
    osPriorityLow         = 2,         ///< Priority: low
    osPriorityBelowNormal = 3,         ///< Priority: below normal
    osPriorityNormal      = 4,         ///< Priority: normal
    osPriorityAboveNormal = 5,         ///< Priority: above normal
    osPriorityHigh        = 6,         ///< Priority: high
    osPriorityHigh1       = 7,         ///< Priority: high
    osPriorityRealtime    = 13,        ///< Priority: realtime
    osPriorityRealtime1   = 14,        ///< Priority: realtime + 1
    osPriorityISR         = 15,        ///< Reserved for ISR deferred thread.
    osPriorityError       = -1,        ///< System cannot determine priority or illegal priority.
    osPriorityReserved    = 0x7FFFFFFF ///< Prevents enum down-size compiler optimization.
} osPriority_t;

#define osThreadDetached 0x00000000U ///< Thread created in detached mode (default)

/// Attributes structure for mutex.
typedef struct {
    const char *name;      ///< name of the mutex
    uint32_t    attr_bits; ///< attribute bits
    void       *cb_mem;    ///< memory for control block
    uint32_t    cb_size;   ///< size of provided memory for control block
} osMutexAttr_t;

static bool InitCompleted = false;
extern bool Transmit_non_RTOS;

static const char *task_started_msg = "started...\n";

/**
 * @brief msg_task_started
 */
static inline void msg_task_started(void)
{
    log_xputs(MSG_LEVEL_TASK_INIT, task_started_msg);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

static SemaphoreHandle_t osStaticMutexNew(const osMutexAttr_t *attr)
{
    SemaphoreHandle_t hMutex;
#if (configQUEUE_REGISTRY_SIZE > 0)
    const char *name;
#endif
    hMutex = NULL;

    if (attr != NULL) {
        if ((attr->cb_mem != NULL) && (attr->cb_size >= sizeof(StaticSemaphore_t))) {
        } else {
            return hMutex;
        }

#if (configSUPPORT_STATIC_ALLOCATION == 1)
        hMutex = (SemaphoreHandle_t)xSemaphoreCreateMutexStatic(attr->cb_mem);
#endif
    }

#if (configQUEUE_REGISTRY_SIZE > 0)
    if (hMutex != NULL) {
        if (attr != NULL) {
            name = attr->name;
        } else {
            name = NULL;
        }
        vQueueAddToRegistry(hMutex, name);
    }
#endif
    return hMutex;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

MUTEX_MEM_DEF(xfunc_out)

MUTEX_MEM_DEF(logging)

MUTEX_MEM_DEF(spi2)

#define DEFAULT_TASK_STACK_SIZE (128U)
TASK_MEM_DEF(default, DEFAULT_TASK_STACK_SIZE, osPriorityLow)

#define KBD_TASK_STACK_SIZE (128U)
TASK_MEM_DEF(kbd, KBD_TASK_STACK_SIZE, osPriorityNormal)

#define CONTROL_TASK_STACK_SIZE (256U)
TASK_MEM_DEF(control, CONTROL_TASK_STACK_SIZE, osPriorityRealtime)

#define COMM_TASK_STACK_SIZE (200U)
TASK_MEM_DEF(comm, COMM_TASK_STACK_SIZE, osPriorityNormal)

#define TEMPERATUR_TASK_STACK_SIZE (200U)
TASK_MEM_DEF(temperatur, TEMPERATUR_TASK_STACK_SIZE, osPriorityHigh)

#define UI_TASK_STACK_SIZE (200U)
TASK_MEM_DEF(ui, UI_TASK_STACK_SIZE, osPriorityNormal)

void StartDefaultTask(void *argument);
void Start_kbd_task(void *argument);
void Start_control_task(void *argument);
void Start_comm_task(void *argument);
void Start_temperatur_task(void *argument);
void Start_ui_task(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* GetIdleTaskMemory prototype (linked to static allocation support) */
void vApplicationGetIdleTaskMemory(StaticTask_t **ppxIdleTaskTCBBuffer,
                                   StackType_t  **ppxIdleTaskStackBuffer,
                                   uint32_t      *pulIdleTaskStackSize);

/* Hook prototypes */
void vApplicationIdleHook(void);
void vApplicationTickHook(void);

/* USER CODE BEGIN 2 */
__weak void vApplicationIdleHook(void)
{
    /* vApplicationIdleHook() will only be called if configUSE_IDLE_HOOK is set
   to 1 in FreeRTOSConfig.h. It will be called on each iteration of the idle
   task. It is essential that code added to this hook function never attempts
   to block in any way (for example, call xQueueReceive() with a block time
   specified, or call vTaskDelay()). If the application makes use of the
   vTaskDelete() API function (as this demo application does) then it is also
   important that vApplicationIdleHook() is permitted to return to its calling
   function, because it is the responsibility of the idle task to clean up
   memory allocated by the kernel to any task that has since been deleted. */
}
/* USER CODE END 2 */

/* USER CODE BEGIN 3 */
__weak void vApplicationTickHook(void)
{
    /* This function will be called by each tick interrupt if
   configUSE_TICK_HOOK is set to 1 in FreeRTOSConfig.h. User code can be
   added here, but the tick hook is called from an interrupt context, so
   code must not attempt to block, and only the interrupt safe FreeRTOS API
   functions can be used (those that end in FromISR()). */
}
/* USER CODE END 3 */

/* USER CODE BEGIN GET_IDLE_TASK_MEMORY */
static StaticTask_t xIdleTaskTCBBuffer;
static StackType_t  xIdleStack[configMINIMAL_STACK_SIZE];

void vApplicationGetIdleTaskMemory(StaticTask_t **ppxIdleTaskTCBBuffer,
                                   StackType_t  **ppxIdleTaskStackBuffer,
                                   uint32_t      *pulIdleTaskStackSize)
{
    *ppxIdleTaskTCBBuffer   = &xIdleTaskTCBBuffer;
    *ppxIdleTaskStackBuffer = &xIdleStack[0];
    *pulIdleTaskStackSize   = configMINIMAL_STACK_SIZE;
    /* place for user code */
}
/* USER CODE END GET_IDLE_TASK_MEMORY */

static TaskHandle_t osStaticTaskNew(TaskFunction_t tf, osThreadAttr_t attr, void *const params)
{
    return xTaskCreateStatic(tf, attr.name, attr.stack_size, params, attr.priority,
                             (StackType_t *)attr.stack_mem, (StaticTask_t *)attr.cb_mem);
}

#define ERROR_HANDLER_IF_NULL(val)                                                                 \
    {                                                                                              \
        if (NULL == val)                                                                           \
            Error_Handler();                                                                       \
    }
static void printTaskAttr(osThreadAttr_t attr)
{
    log_xprintf(MSG_LEVEL_INFO, "%s: prio: %d, stack: %08LX, stack size: %d 32bit words", attr.name,
                attr.priority, attr.stack_mem, attr.stack_size);
}

/**
 * @brief printAllTasksInfo
 */
void printAllTasksInfo(void) {
    printTaskAttr(default_task_attributes);
    printTaskAttr(kbd_task_attributes);
    printTaskAttr(control_task_attributes);
    printTaskAttr(comm_task_attributes);
    printTaskAttr(temperatur_task_attributes);
    printTaskAttr(ui_task_attributes);
}

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void)
{
    /* Create the thread(s) */
    default_taskHandle = osStaticTaskNew(StartDefaultTask, default_task_attributes, NULL);
    ERROR_HANDLER_IF_NULL(default_taskHandle);

    kbd_taskHandle = osStaticTaskNew(Start_kbd_task, kbd_task_attributes, NULL);
    ERROR_HANDLER_IF_NULL(kbd_taskHandle);

    control_taskHandle = osStaticTaskNew(Start_control_task, control_task_attributes, NULL);
    ERROR_HANDLER_IF_NULL(control_taskHandle);

    comm_taskHandle = osStaticTaskNew(Start_comm_task, comm_task_attributes, NULL);
    ERROR_HANDLER_IF_NULL(comm_taskHandle);

    temperatur_taskHandle =
            osStaticTaskNew(Start_temperatur_task, temperatur_task_attributes, NULL);
    ERROR_HANDLER_IF_NULL(temperatur_taskHandle);

    ui_taskHandle = osStaticTaskNew(Start_ui_task, ui_task_attributes, NULL);
    ERROR_HANDLER_IF_NULL(ui_taskHandle);

    /* Create the mutex(es) */
    xfunc_out_MutexHandle = osStaticMutexNew(&xfunc_out_MutexAttr);
    ERROR_HANDLER_IF_NULL(xfunc_out_MutexHandle);

    logging_MutexHandle = osStaticMutexNew(&logging_MutexAttr);
    ERROR_HANDLER_IF_NULL(logging_MutexHandle);
}

/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
void __attribute__((noreturn)) StartDefaultTask(void *argument)
{
    UNUSED(argument);
    msg_task_started();
    for (;;) {
        if (context1->pDevFB->fUpdateScreen != NULL) {
            context1->pDevFB->fUpdateScreen(&hspi2, context1->pDevFB);
        }
        sys_helpers_delay_ms(50U);
    }
}

/**
* @brief Function implementing the kbd_task thread.
* @param argument: Not used
* @retval None
*/
void __attribute__((noreturn)) Start_kbd_task(void *argument)
{
    kbd_task_init();
    (void)argument;
    for (;;) {
        kbd_task_run();
    }
}

/**
* @brief Function implementing the display_task thread.
* @param argument: Not used
* @retval None
*/
void __attribute__((noreturn)) Start_control_task(void *argument)
{
    control_task_init();
    (void)argument;
    for (;;) {
        control_task_run();
    }
}

/**
* @brief Function implementing the comm_task thread.
* @param argument: Not used
* @retval None
*/
void __attribute__((noreturn)) Start_comm_task(void *argument)
{
    comm_task_init();
    (void)argument;

    for (;;) {
        comm_task_run();
    }
}

/**
* @brief Function implementing the temperatur_task thread.
* @param argument: Not used
* @retval None
*/
void __attribute__((noreturn)) Start_temperatur_task(void *argument)
{
    UNUSED(argument);
    for (;;) {
        sys_helpers_delay_ms(UINT32_MAX);
    }
}

/**
* @brief Function implementing the ui_task thread.
* @param argument: Not used
* @retval None
*/
void __attribute__((noreturn)) Start_ui_task(void *argument)
{
    ui_task_init();
    UNUSED(argument);

    for (;;) {
        ui_task_run();
    }
}
