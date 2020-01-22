/** @file exti.c
 *  @brief External interrupt(s) routines
 *
 *  @author Vasiliy Turchenko
 *  @bug
 *  @date 19-Jan-2020
 */

#include "stdbool.h"

#include "cmsis_os.h"
#include "exti.h"
#include "control_task.h"
#include "main.h"

#ifdef STM32F103xB
#include "stm32f1xx.h"
#else
#error MCU NOT DEFINED
#endif

extern TIM_HandleTypeDef htim1;

/* 1 us ticks counter */
uint64_t	counter_1us = 0U;

/* half-periods counter */
uint32_t	half_periods_cnt = 0U;

static uint64_t	last_exti_pin_timestamp =  0U;
static uint32_t duration = 0U;


/* number of half-periods to measure mains frequency period */
#define	NUM_HALF_PERIODS	50U

static uint32_t	half_periods[NUM_HALF_PERIODS] = {0U};
static size_t	half_periods_index = 0U;

static _Bool	need_to_notify = false;

/**
 * @brief enable_exti_notifications
 */
void enable_exti_notifications(void)
{
	need_to_notify = true;
}

/**
 * @brief disable_exti_notifications
 */
void disable_exti_notifications(void)
{
	need_to_notify = false;
}


/**
 * @brief HAL_GPIO_EXTI_Callback
 * @param GPIO_Pin
 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	static uint64_t current_1us_ts = 0U;

	if (GPIO_Pin == GPIO_PIN_6) {
		current_1us_ts  = counter_1us + htim1.Instance->CNT;
		duration = (uint32_t)(current_1us_ts - last_exti_pin_timestamp);
		last_exti_pin_timestamp = current_1us_ts;

		half_periods_cnt++;
		half_periods[half_periods_index] = duration;
		half_periods_index++;
		if (half_periods_index == NUM_HALF_PERIODS) {
			half_periods_index = 0U;
		}

//		HAL_GPIO_TogglePin(BOOST_HEATER_GPIO_Port, BOOST_HEATER_Pin);

	/* notify heater control task */
		if (need_to_notify) {
		BaseType_t xHigherPriorityTaskWoken;
		extern osThreadId control_taskHandle;
		if (xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED) {
			if (xTaskNotifyFromISR(control_taskHandle, EXTI_ARRIVED,
					       eSetValueWithOverwrite,
					       &xHigherPriorityTaskWoken) !=
			    pdPASS) {
				// error
				volatile uint8_t a = 0;
				(void)a;
			}
			portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
		}
		}
	}

}

/**
 * @brief get_mains_half_period
 * @return mains_half_period in microseconds
 */
uint32_t get_mains_half_period(void)
{
	uint32_t retVal = 0U;
	for (size_t i = 0U; i < NUM_HALF_PERIODS; i++) {
		retVal += half_periods[i];
	}
	return (retVal / NUM_HALF_PERIODS);
}
