/** @file control_task.c
 *  @brief triacs main control task
 *
 *  @author Vasiliy Turchenko
 *  @bug
 *  @date 19-Jan-2020
 */

#include "main.h"
#include "watchdog.h"
#include "task_tokens.h"
#include "logging.h"

#ifdef STM32F103xB
#include "stm32f1xx.h"
#else
#error MCU NOT DEFINED
#endif

#include "control_task.h"
#include "exti.h"

/* which task must be notified by this task */
extern osThreadId ui_taskHandle;

/* settings for the next control cycle */
control_task_set_t	control_task_setpoints;

/* global number of current control cycle */
/* 1 control cycle is 25 periods of the mains frequency */
static uint32_t control_cycle_number = 0U;

/* current half-period of the current control cycle */
static uint32_t half_period_number = 0U;

/* main heater setpoint 0 ... 99 %*/
static uint8_t working_main_heater_setpoint = 0U;

/* boost heater setpoint 0 ... 99 %*/
static uint8_t working_boost_heater_setpoint = 0U;

extern osThreadId control_taskHandle;

/**
 * @brief control_task_init
 */
void control_task_init(void)
{
	register_magic(CONTROL_TASK_MAGIC);
	//	i_am_alive(CONTROL_TASK_MAGIC);
	log_xputs(MSG_LEVEL_INFO, " started.");
}

static inline void main_heater_off(void)
{
	HAL_GPIO_WritePin(MAIN_HEATER_GPIO_Port, MAIN_HEATER_Pin,
			  GPIO_PIN_RESET);
}

static inline void main_heater_on(void)
{
	HAL_GPIO_WritePin(MAIN_HEATER_GPIO_Port, MAIN_HEATER_Pin, GPIO_PIN_SET);
}

static inline void boost_heater_off(void)
{
//	HAL_GPIO_WritePin(BOOST_HEATER_GPIO_Port, BOOST_HEATER_Pin,
			  //GPIO_PIN_RESET);
}

static inline void boost_heater_on(void)
{
//	HAL_GPIO_WritePin(BOOST_HEATER_GPIO_Port, BOOST_HEATER_Pin,
//			  GPIO_PIN_SET);
}

/**
 * @brief reset_controls
 */
void reset_controls(void)
{
	control_cycle_number = 0U;
	half_period_number = 0U;
	working_main_heater_setpoint = 0U;
	working_boost_heater_setpoint = 0U;
	main_heater_off();
	boost_heater_off();
}

/**
 * @brief control_task_run
 */
void control_task_run(void)
{
	uint32_t control_task_result = CONTROL_TASK_RES_OK;
	/* wait for notification to start */
	uint32_t notif_val = 0U;
	if (xTaskNotifyWait(ULONG_MAX, ULONG_MAX, &notif_val,
			    pdMS_TO_TICKS(1000U)) != pdTRUE) {
		/* nothing to do */
		goto fExit;
	}
	xTaskNotifyStateClear(NULL);

	if (notif_val != HEATER_START) {
		/* bad command received */
		i_am_alive(CONTROL_TASK_MAGIC);
		control_task_result = CONTROL_TASK_RES_ERR_BAD_CMD;
		/* nothing to do */
		goto fExit_w_notify;
	}

	/* check values */
	if ((control_task_setpoints.main_heater_setpoint > 100U) |
	    (control_task_setpoints.boost_heater_setpoint > 100U)) {
		// error!
		control_task_result = CONTROL_TASK_RES_ERR_BAD_SETTINGS;
		goto fExit_w_notify;
	}

	if ((control_task_setpoints.mains_half_period > 12000U) /* 41.6 Hz */ ||
	    (control_task_setpoints.mains_half_period < 7500U) /* 66.6 Hz*/) {
		// error!
		control_task_result = CONTROL_TASK_RES_ERR_BAD_SETTINGS;
		goto fExit_w_notify;
	}

	/* settings are ok */
	reset_controls();
	working_main_heater_setpoint = control_task_setpoints.main_heater_setpoint / 2U;
	working_boost_heater_setpoint = control_task_setpoints.boost_heater_setpoint / 2U;

	/* enable exti notifications */

	enable_exti_notifications();

	do {
	/* wait for exti notification */
		uint32_t	ulNotifiedValue = 0U;
		BaseType_t	notify_result;

		notify_result = xTaskNotifyWait(pdFALSE, ULONG_MAX, \
						&ulNotifiedValue, \
						pdMS_TO_TICKS(MS_TO_WAIT_EXTI));
		if (notify_result != pdPASS) {
		/* EXTI did not arrived */
		/* ERROR! */
			main_heater_off();
			boost_heater_off();
			control_task_result = CONTROL_TASK_RES_ERR_NO_EXTI;
			break;
		}

		if (ulNotifiedValue != EXTI_ARRIVED) {
		/* unknown notification addived */
			main_heater_off();
			boost_heater_off();
			control_task_result = CONTROL_TASK_RES_ERR_BAD_NOTIF;
//		HAL_GPIO_TogglePin(BOOST_HEATER_GPIO_Port, BOOST_HEATER_Pin);

			break;
		}

		/* exti occured */
		if (working_main_heater_setpoint > half_period_number) {
			main_heater_on();
		} else {
			main_heater_off();
		}
		if (working_boost_heater_setpoint > half_period_number) {
			boost_heater_on();
		} else {
			boost_heater_off();
		}
		/* wait 6 ms ON to ensure TRIAC goes on for this half-period */
		vTaskDelay(pdMS_TO_TICKS(6U));

		main_heater_off();
		boost_heater_off();
		half_period_number++;
	} while (half_period_number < 50U);

fExit_w_notify:

	/* disable exti notifications */
	disable_exti_notifications();
	xTaskNotifyStateClear(control_taskHandle);

	/* notify temperature task about control cycle end */
	if (xTaskNotify(ui_taskHandle, \
			control_task_result, \
			eSetValueWithOverwrite) != pdPASS ) {

			// error

			}

fExit:
	//i_am_alive(CONTROL_TASK_MAGIC);
	return;
}
