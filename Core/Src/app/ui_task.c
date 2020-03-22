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
#include "profiles.h"

#include "regulator.h"
#include "door_servo.h"

extern osThreadId ui_taskHandle;
extern osThreadId control_taskHandle;

//extern int32_t unclamped_result;
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
static void print_top_set(uint16_t t)
{
	gotoXY(0, 32);
	zprint("\nTset:", NORM);

	uint8_t buf[6] = { 0 };
	uint16_to_asciiz(t, (char *)buf);
	zprint((char *)buf, NORM);
}

#if (0)
/**
 * @brief print_boost_set
 */
static void print_bottom_set(void)
{
	gotoXY(20, 28);
	zprint("BOTTOM:\0", NORM);
	uint8_t buf[4] = { 0 };
	uint8_to_asciiz((uint8_t)control_task_setpoints.bottom_heater_setpoint,
			(char *)buf);
	zprint((char *)buf, NORM);
}
#endif

/**
 * @brief u16_eq_tol
 * @param a
 * @param b
 * @param tol
 * @return
 */
static /*inline*/ _Bool u16_eq_tol(uint16_t a, uint16_t b, uint16_t tol)
{
	if (a >= b) {
		return (((int32_t)a - (int32_t)b) < (int32_t)tol);
	}
	return (((int32_t)b - (int32_t)a) < (int32_t)tol);
}

/**
 * @brief do_setpoints
 * @param p_t
 */
static inline void do_setpoints(uint32_t p_t)
{
	control_task_setpoints.top_heater_setpoint =
		(uint8_t)((p_t * (uint32_t)SETPOINT_UPPER_LIM_TOP) / 100U);
	control_task_setpoints.bottom_heater_setpoint =
		(uint8_t)((p_t * (uint32_t)SETPOINT_UPPER_LIM_BTM) / 100U);

	control_task_setpoints.mains_half_period = get_mains_half_period();
}

/**
 * @brief manual_heater_control
 */
void manual_heater_control(void)
{
	control_task_setpoints.top_heater_setpoint = SETPOINT_UPPER_LIM_TOP;
	control_task_setpoints.bottom_heater_setpoint = SETPOINT_UPPER_LIM_BTM;

	_Bool start_trigger = false;
	_Bool lock = false;
	int8_t throttle = 0;
	uint16_t temperature = 0U; ///< actual temperature
	_Bool opened = false;      //// < door state

	xputs("time, set point t, actual t, throttle, unclamped_result\n");

	while (true) {
		key_code_t key_code;
		key_code = get_key();

#if (0)
		if (key_code != NO_KEY) {
			log_xprintf(MSG_LEVEL_INFO, "Key code = %d",
				    (int)key_code);
		}
#endif
		control_task_setpoints.mains_half_period =
			get_mains_half_period();
		xTaskNotifyStateClear(ui_taskHandle);

		static uint16_t temp_setpoint = MANUAL_LOW_TEMP;

		if (key_code == UP_KEY_RELEASED) {
			if (temp_setpoint <= MANUAL_HIGH_TEMP) {
				temp_setpoint++;
			}

			print_top_set(temp_setpoint);

		} else if (key_code == DN_KEY_RELEASED) {
			if (temp_setpoint >= MANUAL_LOW_TEMP) {
				temp_setpoint--;
			}

			print_top_set(temp_setpoint);

		} else if (key_code == ESC_KEY_RELEASED) {
			temp_setpoint = MANUAL_LOW_TEMP;
			control_task_setpoints.top_heater_setpoint = 0U;
			control_task_setpoints.bottom_heater_setpoint = 0U;
			start_trigger = false;
			lock = false;
			break;

		} else if (((key_code == ENTER_KEY_RELEASED) ||
			    start_trigger) &&
			   (!lock)) {
			print_top_set(temp_setpoint);

			/* process time counter */
			static uint32_t proc_time = 0U;
			if (!start_trigger) {
				proc_time = 0U;
			}

			start_trigger = true;
			temperature = get_temperature().average_val0;
			throttle = pid(temp_setpoint, temperature);

			uint32_t p_t = 0U;
			if (throttle > 0) {
				p_t = (uint32_t)throttle;
			}

			do_setpoints(p_t);

			char str_time[8];
			uint16_to_asciiz((uint16_t)proc_time, str_time);
			gotoXY(0U, 8U);
			zprint("MANUAL\n", NORM);
			zprint("\nTime  = ", NORM);
			zprint(str_time, NORM);
			zprint("\nTset  = ", NORM);
			uint16_to_asciiz(temp_setpoint, str_time);
			zprint(str_time, NORM);
			zprint("\nTmeas = ", NORM);
			uint16_to_asciiz(temperature, str_time);
			zprint(str_time, NORM);

			if (xTaskNotify(control_taskHandle, HEATER_START,
					eSetValueWithOverwrite) != pdPASS) {
				// errror
			}

			/* move door, if needed */
			if (throttle < 0) {
				uint8_t d_pos = (uint8_t)(-throttle / 10);
				door_servo_set_position(d_pos);
				opened = true;
			} else {
				if (opened) {
					door_servo_close_door();
					opened = false;
				}
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
			gotoXY(20, 40);
			if (lock) {
				zprint("LOCK\0", NORM);
			}

			/* temp graph */
			static uint32_t elapsed = 0U;
			if ( (HAL_GetTick() - elapsed) >= 1000U ) {
				elapsed = HAL_GetTick();
				proc_time++;
				xprintf("%d, %d, %d, %d, %d\n", proc_time,
					temp_setpoint, temperature, throttle,
					unclamped_result);
			}
		} else {
		//
		}
	}
}

/**
 * @brief ui_task_run
 */
void __attribute__((noreturn)) ui_task_run(void)
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

			log_xprintf(MSG_LEVEL_EXT_INF,
				    "Temperature [0, 1] = [ %d , %d]",
				    tmp.average_val0, tmp.average_val1);
		}
	}
}

/**
 * @brief heaters_error_inf
 */
static inline void heaters_error_inf(void)
{
	control_task_setpoints.top_heater_setpoint = 0U;
	control_task_setpoints.bottom_heater_setpoint = 0U;
	fast_clear_screen();
	gotoXY(0U, 8U);
	zprint("Heater(s) error!", NORM);
	sys_helpers_delay_ms(1000U);
}

/**
 * @brief start_reflow starts reflow process with profile
 * @param np number of the profile
 */
void start_reflow(uint8_t np)
{
	/* init step */
	const reflow_profile_t *work_prof;
	if (np == 0U) {
		work_prof = prof1;
	} else if (np == 1U) {
		work_prof = prof2;
	} else {
		goto fExit;
	}

	fast_clear_screen();
	gotoXY(0U, 8U);
	zprint("Start reflow\n with profile\n", NORM);
	zprint(work_prof->name, INVERT);
	zprint("\nPress ENTER\n when ready\n or ESC\n", NORM);
	if (!wait_for_enter_or_esc()) {
		goto fExit;
	}
	/* reflow confirmed */
	log_xputs(MSG_LEVEL_INFO, "Reflow process started!");
	door_servo_close_door();

	control_task_setpoints.top_heater_setpoint =
		0U; //SETPOINT_UPPER_LIM_TOP;
	control_task_setpoints.bottom_heater_setpoint =
		0U; //SETPOINT_UPPER_LIM_BTM;
	control_task_setpoints.mains_half_period = get_mains_half_period();

	size_t nsteps = profiles_get_num_steps(work_prof);
	if (nsteps == 0U) {
		fast_clear_screen();
		gotoXY(0U, 8U);
		char *ep = "Empty profile!";
		zprint(ep, NORM);
		log_xputs(MSG_LEVEL_PROC_ERR, ep);
		sys_helpers_delay_ms(1000U);
		goto fExit;
	}
	const profile_point_t *curr_step = profiles_get_first_step(work_prof);
	if (curr_step == NULL) {
		log_xputs(MSG_LEVEL_PROC_ERR, "NULL step");
		goto fExit;
	}

	uint16_t estimated_time = profiles_get_prof_length(work_prof);
	log_xprintf(MSG_LEVEL_EXT_INF, "Estimated time = %d seconds.",
		    estimated_time);
	xputs("time, step_time, set point t, actual t, throttle, extra_time, unclamped_result\n");

	fast_clear_screen();

	uint16_t start_time = (uint16_t)(HAL_GetTick() / 1000U);
	_Bool opened = false;		//// < door state
	_Bool extra_time_added = false; //// < extra time was added

	/* if we did not reach the target temperature at the end of the step,
	 * we can add more time but not more than
	 * .max_duration - .min_duration
	*/
	uint16_t extra_time = 0U;

	int8_t throttle = 0; ///< main control value

	uint16_t prev_proc_time = 0U;     ///< not only for logging
	uint16_t total_process_time = 0U; ///< elapsed time for entrie process
	uint16_t elapsed_step_time =
		0U; ///< with respect to the fastest permissible heat rate
	uint16_t temperature = 0U;    ///< actual temperature
	uint16_t set_point_temp = 0U; ///< set point for the current step time

	uint16_t step_start_time = start_time;

#define START_TEMP (50U)
	profiles_set_room_temp(START_TEMP);

	/* some pre-heating */
	_Bool pre_heat = true; ///< PRE-PRE-HEAT

	while (true) {
		temperature = get_temperature().average_val0;

		/* process time */
		uint16_t now = (uint16_t)(HAL_GetTick() / 1000U);
		total_process_time = now - start_time;

		if (pre_heat && (temperature > START_TEMP)) {
			pre_heat = false;
			step_start_time = now;
		}

		if (!pre_heat) {
			elapsed_step_time = now - step_start_time;

			_Bool extra_time_expired = (extra_time == 0U);
			_Bool min_step_duration_expired =
				(elapsed_step_time >= curr_step->min_duration);
			_Bool temp_diff = !u16_eq_tol(
				temperature, curr_step->target_temperature, 2U);

			if ((extra_time_added && extra_time_expired) ||
			    (min_step_duration_expired && !temp_diff)) {
				/* we can go to the next step */
				elapsed_step_time = 0U;
				curr_step = curr_step->next;
				step_start_time = now;
				extra_time_added = false;
				extra_time = 0U;
				continue;
			}

			if (curr_step == NULL) {
				break; /* all done */
			}

			if (!extra_time_added && min_step_duration_expired &&
			    temp_diff) {
				/* we need more time */
				extra_time = curr_step->max_duration -
					     curr_step->min_duration;
				extra_time_added = true;
#if (0)
				xprintf("add extra time %d sec for step ",
					extra_time);
				xputs(curr_step->name);
				xputc('\n');
#endif
			}

			/* process temperature */

			if (!extra_time_added) {
				set_point_temp =
					profiles_get_temp_for_step_time(
						curr_step, elapsed_step_time);
			} else {
				set_point_temp = curr_step->target_temperature;
			}

			throttle = pid(set_point_temp, temperature);
		} else {
			throttle = 100; ///< PRE-PRE-HEAT
		}

		uint32_t p_t = 0U;
		if (throttle > 0) {
			p_t = (uint32_t)throttle;
		}

		do_setpoints(p_t);

		char str_time[8];
		uint16_to_asciiz(total_process_time, str_time);
		gotoXY(0U, 8U);
		zprint("                ", NORM);
		gotoXY(0U, 8U);
		zprint(curr_step->name, NORM);
		zprint("\nTime  = ", NORM);
		zprint(str_time, NORM);
		zprint("\nTset  = ", NORM);
		uint16_to_asciiz(set_point_temp, str_time);
		zprint(str_time, NORM);
		zprint("\nTmeas = ", NORM);
		uint16_to_asciiz(temperature, str_time);
		zprint(str_time, NORM);

		if (xTaskNotify(control_taskHandle, HEATER_START,
				eSetValueWithOverwrite) != pdPASS) {
			// errror
			heaters_error_inf();
			break;
		}

		/* move door, if needed */
		if (throttle < 0) {
			uint8_t d_pos = (uint8_t)(-throttle / 10);
			door_servo_set_position(d_pos);
			opened = true;
		} else {
			if (opened) {
				door_servo_close_door();
				opened = false;
			}
		}

		/* wait for respond */
		uint32_t notif_val = 0U;
		if (xTaskNotifyWait(ULONG_MAX, ULONG_MAX, &notif_val,
				    pdMS_TO_TICKS(600U)) != pdTRUE) {
			/* error */
			/* stop */
			heaters_error_inf();
			break;
		}

		if (notif_val != CONTROL_TASK_RES_OK) {
			log_xprintf(MSG_LEVEL_SERIOUS,
				    "Control task notif. = %d", notif_val);
			heaters_error_inf();
			break;
		}

		if (prev_proc_time < total_process_time) {
			if (extra_time > 0U) {
				extra_time--;
			}

			xprintf("%d, %d, %d, %d, %d, %d, %d\n",
				total_process_time, elapsed_step_time,
				set_point_temp, temperature, throttle,
				extra_time, unclamped_result);
			prev_proc_time = total_process_time;
		}
	}
fExit:
	return;
}
