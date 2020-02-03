/** @file ui_task.c
 *  @brief user interface task
 *
 *  @author turchenkov@gmail.com
 *  @bug
 *  @date 15-Jan-2020
 */

#include <stdbool.h>

#include "FreeRTOS.h"
#include "task.h"
#include "cmsis_os.h"

#include "stm32f1xx_hal.h"

#include "ui_task.h"
#include "kbd_task.h"
#include "watchdog.h"
#include "task_tokens.h"
#include "logging.h"
#include "framebuffer.h"

#include "control_task.h"
#include "exti.h"

#include "hex_gen.h"

#include "menu.h"
#include "temerature_measurement.h"
#include "sys_helpers.h"
#include "main.h"

#include "tim.h"

extern osThreadId ui_taskHandle;
extern osThreadId control_taskHandle;

/**
 * @brief ui_task_init
 */
void ui_task_init(void)
{
	register_magic(UI_TASK_MAGIC);
	//	i_am_alive(ui_TASK_MAGIC);
	log_xputs(MSG_LEVEL_INFO, " started.");
}

/**
 * @brief print_main_set
 */
static void print_top_set(void)
{
	gotoXY(20, 20);
	zprint("TOP:\0", NORM);

	uint8_t buf[4] = { 0 };
	uint8_to_asciiz((uint8_t)control_task_setpoints.top_heater_setpoint,
			buf);
	zprint(buf, NORM);
}
/**
 * @brief print_boost_set
 */
static void print_bottom_set(void)
{
	gotoXY(20, 28);
	zprint("BOTTOM:\0", NORM);
	uint8_t buf[4] = { 0 };
	uint8_to_asciiz((uint8_t)control_task_setpoints.bottom_heater_setpoint,
			buf);
	zprint(buf, NORM);
}

/**
 * @brief manual_heater_control
 */
void manual_heater_control(void)
{
	control_task_setpoints.top_heater_setpoint = 100U;
	control_task_setpoints.bottom_heater_setpoint =
		50U - control_task_setpoints.top_heater_setpoint;

	_Bool start_trigger = false;
	_Bool lock = false;

	while (true) {
		key_code_t key_code;
/*
		clear_key_buffer();
		sys_helpers_delay_ms(60U);
*/
		key_code = get_key();

		if (key_code != NO_KEY) {
			log_xprintf(MSG_LEVEL_INFO, "Key code = %d",
				    (int)key_code);
		}

		control_task_setpoints.mains_half_period =
			get_mains_half_period();
		xTaskNotifyStateClear(ui_taskHandle);

		if (key_code == UP_KEY_RELEASED) {
//HAL_GPIO_TogglePin(BOOST_HEATER_GPIO_Port, BOOST_HEATER_Pin);
			if (control_task_setpoints.top_heater_setpoint <
			    100U) {
				control_task_setpoints.top_heater_setpoint++;
			}


			control_task_setpoints.bottom_heater_setpoint = control_task_setpoints.top_heater_setpoint;
/*
				100U - control_task_setpoints.top_heater_setpoint;
*/
			print_top_set();
			print_bottom_set();
		} else if (key_code == DN_KEY_RELEASED) {
			if (control_task_setpoints.top_heater_setpoint > 0U) {
				control_task_setpoints.top_heater_setpoint--;
			}
			control_task_setpoints.bottom_heater_setpoint = control_task_setpoints.top_heater_setpoint;
/*
				100U - control_task_setpoints.top_heater_setpoint;
*/
			print_top_set();
			print_bottom_set();
		} else if (key_code == ESC_KEY_RELEASED) {
			control_task_setpoints.top_heater_setpoint = 0U;
			control_task_setpoints.bottom_heater_setpoint = 0U;
			start_trigger = false;
			lock = false;
			gotoXY(20, 40);
			zprint("    \0", NORM);
			print_top_set();
			print_bottom_set();
		} else if (((key_code == ENTER_KEY_RELEASED) ||
			    start_trigger) &&
			   (!lock)) {
			print_top_set();
			print_bottom_set();

			start_trigger = true;

			if (xTaskNotify(control_taskHandle, HEATER_START,
					eSetValueWithOverwrite) != pdPASS) {
				// errror
			}

			/* wait for respond */

			uint32_t notif_val = 0U;
			if (xTaskNotifyWait(ULONG_MAX, ULONG_MAX, &notif_val,
					    pdMS_TO_TICKS(600U)) != pdTRUE) {
				/* error */
				/* stop */
				start_trigger = false;
				lock = true;
			}

			if (notif_val != CONTROL_TASK_RES_OK) {
				log_xprintf(MSG_LEVEL_SERIOUS,
					    "Control task notif. = %d",
					    notif_val);
				start_trigger = false;
				lock = true;
			}
//HAL_GPIO_WritePin(BOOST_HEATER_GPIO_Port, BOOST_HEATER_Pin, GPIO_PIN_RESET);
			gotoXY(20, 40);
			if (lock) {
				zprint("LOCK\0", NORM);
			}

			/* temp graph */
			uint32_t elapsed = 0U;
			if (HAL_GetTick() - elapsed >= 1000U) {
				elapsed = HAL_GetTick();
				averaged_data_t tmp = get_temperature();
//				log_xprintf(MSG_LEVEL_EXT_INF, "Temperature [0, 1] = [ %d , %d]", tmp.average_val0, tmp.average_val1);
				xprintf("%d, %d\n", elapsed / 1000U, tmp.average_val0);
			}
		}
	}
}

/**
 * @brief ui_task_run
 */
void ui_task_run(void)
{
	current_item = &L0_i0;
	draw_menu_level(&L0_i0);

	/* test */
	static uint32_t cnt = 0U;

	while (true) {
		process_current_item();
		draw_menu_level(current_item);

		/* test */
		cnt++;
		if ((cnt % 20U) == 0U) {
		averaged_data_t tmp = get_temperature();

		log_xprintf(MSG_LEVEL_EXT_INF, "Temperature [0, 1] = [ %d , %d]", tmp.average_val0, tmp.average_val1);
		}
	}

#if (false)

#endif
}

//	i_am_alive(ui_TASK_MAGIC);

