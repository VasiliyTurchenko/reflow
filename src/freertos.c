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
#include "spi.h"
#include "task.h"
#include "hal_ll.h"
#include "cmsis_os.h"
#include "freertos_exported.h"

#include "comm_task.h"
#include "kbd_task.h"
#include "ui_task.h"
#include "control_task.h"
#include "logging.h"
#include "framebuffer.h"

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

osMutexId          xfunc_out_MutexHandle;
osStaticMutexDef_t xfunc_out_Mutex_ControlBlock;

osMutexId          logging_MutexHandle;
osStaticMutexDef_t logging_Mutex_ControlBlock;

osThreadId          defaultTaskHandle;
uint32_t            defaultTaskBuffer[128];
osStaticThreadDef_t defaultTaskControlBlock;

osThreadId          kbd_taskHandle;
uint32_t            kbd_task_Buffer[128];
osStaticThreadDef_t kbd_task_ControlBlock;

osThreadId          control_taskHandle;
uint32_t            control_task_Buffer[256];
osStaticThreadDef_t control_task_ControlBlock;

osThreadId          comm_taskHandle;
uint32_t            comm_task_Buffer[200];
osStaticThreadDef_t comm_task_ControlBlock;

osThreadId          temperatur_taskHandle;
uint32_t            temperatur_task_Buffer[200];
osStaticThreadDef_t temperatur_task_ControlBlock;

osThreadId          ui_taskHandle;
uint32_t            ui_task_Buffer[200];
osStaticThreadDef_t ui_task_ControlBlock;

void StartDefaultTask(void const *argument);
void Start_kbd_task(void const *argument);
void Start_control_task(void const *argument);
void Start_comm_task(void const *argument);
void Start_temperatur_task(void const *argument);
void Start_ui_task(void const *argument);

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

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void)
{
    osMutexStaticDef(xfunc_out_Mutex, &xfunc_out_Mutex_ControlBlock);
    xfunc_out_MutexHandle = osMutexCreate(osMutex(xfunc_out_Mutex));

    osMutexStaticDef(logging_Mutex, &logging_Mutex_ControlBlock);
    logging_MutexHandle = osMutexCreate(osMutex(logging_Mutex));

    /* Create the thread(s) */
    /* definition and creation of defaultTask */
    osThreadStaticDef(defaultTask, StartDefaultTask, osPriorityNormal, 0, 128, defaultTaskBuffer,
                      &defaultTaskControlBlock);
    defaultTaskHandle = osThreadCreate(osThread(defaultTask), NULL);

    /* definition and creation of kbd_task */
    osThreadStaticDef(kbd_task, Start_kbd_task, osPriorityNormal, 0, 128, kbd_task_Buffer,
                      &kbd_task_ControlBlock);
    kbd_taskHandle = osThreadCreate(osThread(kbd_task), NULL);

    /* definition and creation of display_task */
    osThreadStaticDef(control_task, Start_control_task, osPriorityRealtime, 0, 256,
                      control_task_Buffer, &control_task_ControlBlock);
    control_taskHandle = osThreadCreate(osThread(control_task), NULL);

    /* definition and creation of comm_task */
    osThreadStaticDef(comm_task, Start_comm_task, osPriorityAboveNormal, 0, 200, comm_task_Buffer,
                      &comm_task_ControlBlock);
    comm_taskHandle = osThreadCreate(osThread(comm_task), NULL);

    /* definition and creation of temperatur_task */
    osThreadStaticDef(temperatur_task, Start_temperatur_task, osPriorityRealtime, 0, 200,
                      temperatur_task_Buffer, &temperatur_task_ControlBlock);
    temperatur_taskHandle = osThreadCreate(osThread(temperatur_task), NULL);

    /* definition and creation of ui_task */
    osThreadStaticDef(ui_task, Start_ui_task, osPriorityNormal, 0, 200, ui_task_Buffer,
                      &ui_task_ControlBlock);
    ui_taskHandle = osThreadCreate(osThread(ui_task), NULL);
}

/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
void StartDefaultTask(void const *argument)
{
    UNUSED(argument);
    msg_task_started();
    for (;;) {
        if (context1->pDevFB->fUpdateScreen != NULL) {
            context1->pDevFB->fUpdateScreen(&hspi2, context1->pDevFB);
        }
        osDelay(50U);
    }
}

/**
* @brief Function implementing the kbd_task thread.
* @param argument: Not used
* @retval None
*/
void __attribute__((noreturn)) Start_kbd_task(void const *argument)
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
void __attribute__((noreturn)) Start_control_task(void const *argument)
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
void __attribute__((noreturn)) Start_comm_task(void const *argument)
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
void Start_temperatur_task(void const *argument)
{
    UNUSED(argument);
    for (;;) {
        osDelay(UINT32_MAX);
    }
}

/**
* @brief Function implementing the ui_task thread.
* @param argument: Not used
* @retval None
*/
void __attribute__((noreturn)) Start_ui_task(void const *argument)
{
    ui_task_init();
    UNUSED(argument);

    for (;;) {
        ui_task_run();
    }
}
