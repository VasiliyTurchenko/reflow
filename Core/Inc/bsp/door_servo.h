/** @file door_servo.c
 *  @brief oven door servo
 *
 *  @author Vasiliy Turchenko
 *  @bug
 *  @date 28-Jan-2020
 */

#ifndef DOOR_SERVO_H
#define DOOR_SERVO_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stddef.h>
#include <limits.h>
#include <stdbool.h>

/* macro for pwm data calculation */


typedef struct __attribute__((packed)) door_pwm_cal_data {

	uint32_t	open_PWM;
	uint32_t	closed_PWM;
	_Bool	valid;

}	door_pwm_cal_data_t;

void door_servo_init_module(void);
void pwm_manual(void);
ErrorStatus door_servo_calibrate(void);

#ifdef __cplusplus
}
#endif

#endif // DOOR_SERVO_H
