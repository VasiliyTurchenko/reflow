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

typedef struct heater_runtime_info {
	/* 0..50 step 2% */
	uint8_t working_heater_setpoint;
	/* 0..10 step 2% */
	uint8_t extra_top_setpoint;
} heater_runtime_info_t;

/* which task must be notified by this task */
extern osThreadId ui_taskHandle;

/* settings for the next control cycle */
control_task_set_t control_task_setpoints;

/* global number of current control cycle */
/* 1 control cycle is 25 periods of the mains frequency */
static uint32_t control_cycle_number = 0U;

/* current half-period of the current control cycle */
static uint32_t half_period_number = 0U;

static heater_runtime_info_t top_heater_runtime = { 0U, 0U };
static heater_runtime_info_t bottom_heater_runtime = { 0U, 0U };

extern osThreadId control_taskHandle;

static _Bool Bresenham_control(heater_runtime_info_t *heater);

/**
 * @brief control_task_init
 */
void control_task_init(void)
{
	register_magic(CONTROL_TASK_MAGIC);
	//	i_am_alive(CONTROL_TASK_MAGIC);
	log_xputs(MSG_LEVEL_INFO, " started.");
}

static inline void top_heater_off(void)
{
	HAL_GPIO_WritePin(TOP_HEATER_GPIO_Port, TOP_HEATER_Pin, GPIO_PIN_RESET);
}

static inline void top_heater_on(void)
{
	HAL_GPIO_WritePin(TOP_HEATER_GPIO_Port, TOP_HEATER_Pin, GPIO_PIN_SET);
}

static inline void bottom_heater_off(void)
{
	HAL_GPIO_WritePin(BOTTOM_HEATER_GPIO_Port, BOTTOM_HEATER_Pin,
			  GPIO_PIN_RESET);
}

static inline void bottom_heater_on(void)
{
	HAL_GPIO_WritePin(BOTTOM_HEATER_GPIO_Port, BOTTOM_HEATER_Pin,
			  GPIO_PIN_SET);
}

/**
 * @brief reset_controls
 */
void reset_controls(void)
{
	control_cycle_number = 0U;
	half_period_number = 0U;
	half_period_number = 0U;
	top_heater_runtime.extra_top_setpoint = 0U;
	top_heater_runtime.working_heater_setpoint = 0U;

	bottom_heater_runtime.extra_top_setpoint = 0U;
	bottom_heater_runtime.working_heater_setpoint = 0U;

	top_heater_off();
	bottom_heater_off();
}

#if (HEATER_CTRL_MODE == HEATER_CTRL_MODE_SERIAL)
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
	if ((control_task_setpoints.top_heater_setpoint > 100U) |
	    (control_task_setpoints.bottom_heater_setpoint > 100U)) {
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
	working_top_heater_setpoint =
		control_task_setpoints.top_heater_setpoint / 2U;
	working_bottom_heater_setpoint =
		control_task_setpoints.bottom_heater_setpoint / 2U;

	/* enable exti notifications */

	enable_exti_notifications();

	do {
		/* wait for exti notification */
		uint32_t ulNotifiedValue = 0U;
		BaseType_t notify_result;

		notify_result =
			xTaskNotifyWait(pdFALSE, ULONG_MAX, &ulNotifiedValue,
					pdMS_TO_TICKS(MS_TO_WAIT_EXTI));
		if (notify_result != pdPASS) {
			/* EXTI did not arrived */
			/* ERROR! */
			top_heater_off();
			bottom_heater_off();
			control_task_result = CONTROL_TASK_RES_ERR_NO_EXTI;
			break;
		}

		if (ulNotifiedValue != EXTI_ARRIVED) {
			/* unknown notification addived */
			top_heater_off();
			bottom_heater_off();
			control_task_result = CONTROL_TASK_RES_ERR_BAD_NOTIF;
			//		HAL_GPIO_TogglePin(BOOST_HEATER_GPIO_Port, BOOST_HEATER_Pin);

			break;
		}

		/* exti occured */
		if (working_top_heater_setpoint > half_period_number) {
			top_heater_on();
		} else {
			top_heater_off();
		}
		if (working_bottom_heater_setpoint > half_period_number) {
			bottom_heater_on();
		} else {
			bottom_heater_off();
		}
		/* wait 6 ms ON to ensure TRIAC goes on for this half-period */
		vTaskDelay(pdMS_TO_TICKS(6U));

		top_heater_off();
		bottom_heater_off();
		half_period_number++;
	} while (half_period_number < 50U);

fExit_w_notify:

	/* disable exti notifications */
	disable_exti_notifications();
	xTaskNotifyStateClear(control_taskHandle);

	/* notify temperature task about control cycle end */
	if (xTaskNotify(ui_taskHandle, control_task_result,
			eSetValueWithOverwrite) != pdPASS) {
		// error
	}

fExit:
	//i_am_alive(CONTROL_TASK_MAGIC);
	return;
}

#elif (HEATER_CTRL_MODE == HEATER_CTRL_MODE_PARALLEL)

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
	if ((control_task_setpoints.top_heater_setpoint > SETPOINT_UPPER_LIM) |
	    (control_task_setpoints.bottom_heater_setpoint >
	     SETPOINT_UPPER_LIM)) {
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
	top_heater_runtime.working_heater_setpoint =
		control_task_setpoints.top_heater_setpoint / 2U;

	if (top_heater_runtime.working_heater_setpoint > 50U) {
		top_heater_runtime.extra_top_setpoint =
			top_heater_runtime.working_heater_setpoint - 50U;
		top_heater_runtime.working_heater_setpoint = 50U;
	}

	bottom_heater_runtime.working_heater_setpoint =
		control_task_setpoints.bottom_heater_setpoint / 2U;

	if (bottom_heater_runtime.working_heater_setpoint > 50U) {
		bottom_heater_runtime.extra_top_setpoint =
			bottom_heater_runtime.working_heater_setpoint - 50U;
		bottom_heater_runtime.working_heater_setpoint = 50U;
	}

	/* enable exti notifications */

	enable_exti_notifications();

	do {
		/* wait for exti notification */
		uint32_t ulNotifiedValue = 0U;
		BaseType_t notify_result;

		notify_result =
			xTaskNotifyWait(pdFALSE, ULONG_MAX, &ulNotifiedValue,
					pdMS_TO_TICKS(MS_TO_WAIT_EXTI));
		if (notify_result != pdPASS) {
			/* EXTI did not arrived */
			/* ERROR! */
			top_heater_off();
			bottom_heater_off();
			control_task_result = CONTROL_TASK_RES_ERR_NO_EXTI;
			break;
		}

		if (ulNotifiedValue != EXTI_ARRIVED) {
			/* unknown notification addived */
			top_heater_off();
			bottom_heater_off();
			control_task_result = CONTROL_TASK_RES_ERR_BAD_NOTIF;
			//		HAL_GPIO_TogglePin(BOOST_HEATER_GPIO_Port, BOOST_HEATER_Pin);

			break;
		}
		/*
	100% of power = odd exti for top, even exti for bottom
*/
		/* exti occured */

		if ((half_period_number & 0x01U) == 0U) {
			/* odd half-period, do top*/
			if (Bresenham_control(&top_heater_runtime)) {
				/* on */
				top_heater_on();
			} else {
				top_heater_off();
			}
			/* add extra bottom power */
			if (bottom_heater_runtime.extra_top_setpoint > 0U) {
				bottom_heater_on();
				bottom_heater_runtime.extra_top_setpoint--;
			}

		} else {
			/* do bottom */
			if (Bresenham_control(&bottom_heater_runtime)) {
				/* on */
				bottom_heater_on();
			} else {
				bottom_heater_off();
			}
			/* add extra top power */
			if (half_period_number > 25U) {
				if (top_heater_runtime.extra_top_setpoint >
				    0U) {
					top_heater_on();
					top_heater_runtime
						.extra_top_setpoint--;
				}
			}
		}

		/* wait 6 ms ON to ensure TRIAC goes on for this half-period */
		vTaskDelay(pdMS_TO_TICKS(6U));

		top_heater_off();
		bottom_heater_off();
		half_period_number++;
	} while (half_period_number < 50U);

fExit_w_notify:

	/* disable exti notifications */
	disable_exti_notifications();
	xTaskNotifyStateClear(control_taskHandle);

	/* notify temperature task about control cycle end */
	if (xTaskNotify(ui_taskHandle, control_task_result,
			eSetValueWithOverwrite) != pdPASS) {
		// error
	}

fExit:
	//i_am_alive(CONTROL_TASK_MAGIC);
	return;
}

static _Bool Bresenham_control(heater_runtime_info_t *heater)
{
	_Bool retVal = false;
/*
 *	draw line from 0,0 to 24, heater->working_heater_setpoint
 *
*/


	return retVal;
}

#else

void control_task_run(void)
{
}

#endif
