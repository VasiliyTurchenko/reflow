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


static door_pwm_cal_data_t * door_pwm_cal = NULL;

/**
 * @brief door_servo_init_module
 */
void door_servo_init_module(void)
{
	door_pwm_cal = get_cfg(CFG_TYPE_DOOR_PWM);
	if (!door_pwm_cal->valid) {
		door_pwm_cal->open_PWM = 0U;
		door_pwm_cal->closed_PWM = 0U;
	}
}

/**
 * @brief pwm_manual
 */
void pwm_manual(void)
{
	uint32_t	pwm_min = MIN_PWM_DURATION;
	uint32_t	pwm_max = MAX_PWM_DURATION;
	uint32_t	pwm_curr = pwm_min;

	fast_clear_screen();
	_Bool up = true;
	while (true) {
		key_code_t key_code;
		key_code = get_key();
		if (key_code == UP_KEY_RELEASED) {
			if(pwm_curr < pwm_max) {
				pwm_curr+=10U;
			}
		}
		if (key_code == DN_KEY_RELEASED) {
			if(pwm_curr > pwm_min) {
				pwm_curr-=10U;
			}
		}

		if(up) {
			pwm_curr++;
			if (pwm_curr > pwm_max) {
				pwm_curr = pwm_max;
				up = false;
			}
		} else {
			pwm_curr--;
			if (pwm_curr < pwm_min) {
				pwm_curr = pwm_min;
				up = true;
			}
		}

		if (key_code == ESC_KEY_RELEASED) {
			break;
		}
		htim2.Instance->CCR1 = pwm_curr;

		char * buf[6];
		uint16_to_asciiz((uint16_t)pwm_curr, (char*)buf);
		gotoXY(20,20);
		zprint((char*)buf, NORM);


		sys_helpers_delay_ms(10U);
	}

}

/**
 * @brief door_servo_calibrate calibrates the door servo
 */
ErrorStatus door_servo_calibrate(void)
{
	ErrorStatus retVal = ERROR;
	fast_clear_screen();
	gotoXY(0U, 8U);
	zprint("Ready to start\n", NORM);
	zprint("door servo\n", NORM);
	zprint(" calibration?\n", NORM);
	zprint("Press ENTER\n", NORM);
	zprint("or ESC\n", NORM);

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

	uint32_t open_pwm = 0U;
	uint32_t close_pwm = 0U;

	fast_clear_screen();
	gotoXY(0U, 8U);
	char * use_up_dn = "Use Up and Down\n";
	zprint(use_up_dn, NORM);
	zprint("to open the door\n", NORM);
	zprint("Press ENTER when\n", NORM);
	zprint("door is opened\n", NORM);

	open_pwm = 0;

	wait_for_key(ENTER_KEY_RELEASED);

	fast_clear_screen();
	gotoXY(0U, 8U);
	zprint(use_up_dn, NORM);
	zprint("to close the door\n", NORM);
	zprint("Press ENTER when\n", NORM);
	zprint("door is closed\n", NORM);

	close_pwm = 1;



fExit:
	return retVal;
}
