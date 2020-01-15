/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#include "comm_task.h"
#include "kbd_task.h"
#include "ui_task.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */

osMutexId xfunc_outMutexHandle;
osStaticMutexDef_t xfunc_outMutex_ControlBlock;


/* USER CODE END Variables */
osThreadId defaultTaskHandle;
uint32_t defaultTaskBuffer[ 128 ];
osStaticThreadDef_t defaultTaskControlBlock;
osThreadId kbd_taskHandle;
uint32_t kbd_task_Buffer[ 128 ];
osStaticThreadDef_t kbd_task_ControlBlock;
osThreadId display_taskHandle;
uint32_t display_task_Buffer[ 256 ];
osStaticThreadDef_t display_task_ControlBlock;
osThreadId comm_taskHandle;
uint32_t comm_task_Buffer[ 200 ];
osStaticThreadDef_t comm_task_ControlBlock;
osThreadId temperatur_taskHandle;
uint32_t temperatur_task_Buffer[ 200 ];
osStaticThreadDef_t temperatur_task_ControlBlock;
osThreadId ui_taskHandle;
uint32_t ui_task_Buffer[ 200 ];
osStaticThreadDef_t ui_task_ControlBlock;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void const * argument);
void Start_kbd_task(void const * argument);
void Start_display_task(void const * argument);
void Start_comm_task(void const * argument);
void Start_temperatur_task(void const * argument);
void Start_ui_task(void const * argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* GetIdleTaskMemory prototype (linked to static allocation support) */
void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize );

/* Hook prototypes */
void vApplicationIdleHook(void);
void vApplicationTickHook(void);

/* USER CODE BEGIN 2 */
__weak void vApplicationIdleHook( void )
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
__weak void vApplicationTickHook( void )
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
static StackType_t xIdleStack[configMINIMAL_STACK_SIZE];

void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize )
{
  *ppxIdleTaskTCBBuffer = &xIdleTaskTCBBuffer;
  *ppxIdleTaskStackBuffer = &xIdleStack[0];
  *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
  /* place for user code */
}
/* USER CODE END GET_IDLE_TASK_MEMORY */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */

	osMutexStaticDef(xfunc_outMutex, &xfunc_outMutex_ControlBlock);
	xfunc_outMutexHandle = osMutexCreate(osMutex(xfunc_outMutex));

  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* definition and creation of defaultTask */
  osThreadStaticDef(defaultTask, StartDefaultTask, osPriorityNormal, 0, 128, defaultTaskBuffer, &defaultTaskControlBlock);
  defaultTaskHandle = osThreadCreate(osThread(defaultTask), NULL);

  /* definition and creation of kbd_task */
  osThreadStaticDef(kbd_task, Start_kbd_task, osPriorityNormal, 0, 128, kbd_task_Buffer, &kbd_task_ControlBlock);
  kbd_taskHandle = osThreadCreate(osThread(kbd_task), NULL);

  /* definition and creation of display_task */
  osThreadStaticDef(display_task, Start_display_task, osPriorityNormal, 0, 256, display_task_Buffer, &display_task_ControlBlock);
  display_taskHandle = osThreadCreate(osThread(display_task), NULL);

  /* definition and creation of comm_task */
  osThreadStaticDef(comm_task, Start_comm_task, osPriorityAboveNormal, 0, 200, comm_task_Buffer, &comm_task_ControlBlock);
  comm_taskHandle = osThreadCreate(osThread(comm_task), NULL);

  /* definition and creation of temperatur_task */
  osThreadStaticDef(temperatur_task, Start_temperatur_task, osPriorityRealtime, 0, 200, temperatur_task_Buffer, &temperatur_task_ControlBlock);
  temperatur_taskHandle = osThreadCreate(osThread(temperatur_task), NULL);

  /* definition and creation of ui_task */
  osThreadStaticDef(ui_task, Start_ui_task, osPriorityNormal, 0, 200, ui_task_Buffer, &ui_task_ControlBlock);
  ui_taskHandle = osThreadCreate(osThread(ui_task), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void const * argument)
{

  /* USER CODE BEGIN StartDefaultTask */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END StartDefaultTask */
}

/**
* @brief Function implementing the kbd_task thread.
* @param argument: Not used
* @retval None
*/
void __attribute__((noreturn)) Start_kbd_task(void const * argument)
{
	kbd_task_init();
	(void)argument;
  for(;;)
  {
    kbd_task_run();
  }
}

/* USER CODE BEGIN Header_Start_display_task */
/**
* @brief Function implementing the display_task thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_Start_display_task */
void Start_display_task(void const * argument)
{
  /* USER CODE BEGIN Start_display_task */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END Start_display_task */
}


/**
* @brief Function implementing the comm_task thread.
* @param argument: Not used
* @retval None
*/
void __attribute__((noreturn)) Start_comm_task(void const * argument)
{
  comm_task_init();
  (void)argument;

  for(;;)
  {
    comm_task_run();
  }
}

/* USER CODE BEGIN Header_Start_temperatur_task */
/**
* @brief Function implementing the temperatur_task thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_Start_temperatur_task */
void Start_temperatur_task(void const * argument)
{
  /* USER CODE BEGIN Start_temperatur_task */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END Start_temperatur_task */
}

/**
* @brief Function implementing the ui_task thread.
* @param argument: Not used
* @retval None
*/
void __attribute__((noreturn)) Start_ui_task(void const * argument)
{
  ui_task_init();
  (void)argument;

  for(;;)
  {
	ui_task_run();
  }
}


/***************************** END OF FILE ************************************/
