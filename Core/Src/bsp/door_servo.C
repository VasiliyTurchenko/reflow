/** @file door_servo.c
 *  @brief oven door servo
 *
 *  @author Vasiliy Turchenko
 *  @bug
 *  @date 28-Jan-2020
 */

#include "tim.h"
#include "kbd_task.h"
#include "parameters_storage.h"
#include "door_servo.h"
#include "framebuffer.h"
#include "logging.h"
#include "sys_helpers.h"
#include "hex_gen.h"

static door_pwm_cal_data_t *door_pwm_cal = NULL;

/* 10 pwm values for 10 door positions */
/* index 0 - door is closed */
#define N_POS (10)
static uint16_t door_position[N_POS] = { 0 };

static const uint32_t open_step_ms = 5U; ///< step in ms between CCR change
static const uint32_t close_step_ms = 2U; ///< step in ms between CCR change
static _Bool inc_open = false; ///< true if open requires inc CCR

/**
 * @brief door_servo_init_module
 */
void door_servo_init_module(void)
{
	door_pwm_cal = (door_pwm_cal_data_t *)get_cfg(CFG_TYPE_DOOR_PWM);
	if (!door_pwm_cal->valid) {
		door_pwm_cal->open_PWM = 0U;
		door_pwm_cal->closed_PWM = 0U;
	} else {
		int32_t op_pwm = (int32_t)((door_pwm_cal->open_PWM) & 0xFFFFU);
		int32_t cl_pwm =
			(int32_t)((door_pwm_cal->closed_PWM) & 0xFFFFU);
		int32_t step = 0;
		/* positive step means we need increment CCR to open */
		step = (op_pwm - cl_pwm) / (N_POS - 1);
		if (step > 0) {
			inc_open = true;
		}
		for (int32_t i = 0U; i < (N_POS - 1); i++) {
			door_position[i] = (uint16_t)(cl_pwm + step * i);
		}
		door_position[N_POS - 1] = (uint16_t)(op_pwm);
	}
}

/**
 * @brief pwm_manual
 * @param pwm pointer to store set pwm value
 * @return true if enter , false if esc
 */
_Bool pwm_manual(uint32_t *pwm)
{
	uint32_t pwm_min = MIN_PWM_DURATION;
	uint32_t pwm_max = MAX_PWM_DURATION;
	uint32_t pwm_curr = MID_PWM_DURATION;

	_Bool retVal = false;

	while (true) {
		key_code_t key_code;
		key_code = get_key();
		if (key_code == UP_KEY_RELEASED) {
			if (pwm_curr < pwm_max) {
				pwm_curr += 5U;
			}
		}
		if (key_code == DN_KEY_RELEASED) {
			if (pwm_curr > pwm_min) {
				pwm_curr -= 5U;
			}
		}

		if (key_code == ESC_KEY_RELEASED) {
			break;
		}

		if (key_code == ENTER_KEY_RELEASED) {
			retVal = true;
			*pwm = pwm_curr;
			break;
		}

		htim2.Instance->CCR1 = pwm_curr;

		char *buf[6];
		uint16_to_asciiz((uint16_t)pwm_curr, (char *)buf);
		gotoXY(20, 54);
		zprint((char *)buf, NORM);

		sys_helpers_delay_ms(10U);
	}
	return retVal;
}

/**
 * @brief door_servo_calibrate calibrates the door servo
 */
ErrorStatus door_servo_calibrate(void)
{
	ErrorStatus retVal = ERROR;

	if (door_pwm_cal == NULL) {
		goto fExit;
	}

	fast_clear_screen();
	gotoXY(0U, 8U);
	zprint("Ready to start\n", NORM);
	zprint("door servo\n", NORM);
	zprint(" calibration?\n", NORM);
	zprint("Press ENTER\n", NORM);
	zprint("or ESC\n", NORM);

	door_pwm_cal->valid = false;

	clear_key_buffer();
	key_code_t key;
	while (true) {
		key = get_key();
		if ((key == ESC_KEY_RELEASED) || (key == ENTER_KEY_RELEASED)) {
			break;
		}
		vTaskDelay(pdMS_TO_TICKS(10U));
	}

	if (key == ESC_KEY_RELEASED) {
		goto fExit;
	}

	/* next step */

	uint32_t open_pwm = MID_PWM_DURATION;
	uint32_t close_pwm = MID_PWM_DURATION;

	fast_clear_screen();
	gotoXY(0U, 8U);
	const char *use_up_dn = "Use Up and Down\n";
	zprint(use_up_dn, NORM);
	zprint("to open the door", NORM);
	zprint("Press ENTER when", NORM);
	zprint("door is opened", NORM);

	if (!pwm_manual(&open_pwm)) {
		goto fExit;
	}

	fast_clear_screen();
	gotoXY(0U, 8U);
	zprint(use_up_dn, NORM);
	zprint("to close the door", NORM);
	zprint("Press ENTER when", NORM);
	zprint("door is closed", NORM);

	if (!pwm_manual(&close_pwm)) {
		goto fExit;
	}

	/* save calibrated pwm values*/

	door_pwm_cal->valid = true;
	door_pwm_cal->open_PWM = open_pwm;
	door_pwm_cal->closed_PWM = close_pwm;

	fast_clear_screen();
	gotoXY(0U, 8U);
	zprint("Door pwm is \ncalibrated\n", NORM);

	if (save_cfg_pool() == SUCCESS) {
		sys_helpers_delay_ms_and_reboot(1000U);
	}

	retVal = SUCCESS;

fExit:
	return retVal;
}

/**
 * @brief pwm_change
 * @param ch
 */
static inline void pwm_change(int32_t ch)
{
	uint32_t dly = 0U;
	if ((ch >= 0) && (inc_open)) {
		/* opening */
		dly = open_step_ms;
	} else if ((ch >= 0) && (!inc_open)) {
		/* closing */
		dly = close_step_ms;
	}else if ((ch < 0) && (inc_open)) {
		/* closing */
		dly = close_step_ms;
	} else {
		/* opening */
		dly = open_step_ms;
	}
	int32_t start_pwm = (int32_t)htim2.Instance->CCR1;
	int32_t stop_pwm = start_pwm + ch;
	int32_t inc = (stop_pwm >= start_pwm) ? (int32_t)1 : -1;

	while (start_pwm != stop_pwm) {
		start_pwm += inc;
		htim2.Instance->CCR1 = (uint16_t)start_pwm;
		sys_helpers_delay_ms(dly);
	}
}

/**
 * @brief door_servo_set_position
* @param pos position index : 0 closed, N_POS open
 * @return ERROR or SUCCESS
 */
ErrorStatus door_servo_set_position(uint8_t pos)
{
	ErrorStatus retVal = ERROR;
	if ((door_pwm_cal == NULL) || (!door_pwm_cal->valid)) {
		goto fExit;
	}
	int32_t curr_pwm = (int32_t)htim2.Instance->CCR1;
	int32_t target_pwm = 0;

	pos = (pos < (uint8_t)N_POS) ? pos : (uint8_t)(N_POS - 1);
	/* it is safe to convert u16 to i32 */
	target_pwm = (int32_t)door_position[pos];

	int32_t change = target_pwm - curr_pwm;
	pwm_change(change);
	retVal = SUCCESS;
fExit:
	return retVal;
}

/**
 * @brief door_servo_open_door
 */
void door_servo_open_door(void)
{
	door_servo_set_position((uint8_t)(N_POS - 1));
}

/**
 * @brief door_servo_close_door
 */
void door_servo_close_door(void)
{
	door_servo_set_position(0U);
}
