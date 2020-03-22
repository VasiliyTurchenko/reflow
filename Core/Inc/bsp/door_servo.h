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


#ifdef STM32F303xC
#include "stm32f3xx.h"
#elif STM32F103xB
#include "stm32f1xx.h"
#else
#error MCU NOT DEFINED
#endif
/* macro for pwm data calculation */


typedef struct __attribute__((packed)) door_pwm_cal_data {

	uint32_t	open_PWM;
	uint32_t	closed_PWM;
	_Bool	valid;

}	door_pwm_cal_data_t;

void door_servo_init_module(void);
_Bool pwm_manual(uint32_t * pwm);
ErrorStatus door_servo_calibrate(void);

void door_servo_close_door(void);
void door_servo_open_door(void);
ErrorStatus door_servo_set_position(uint8_t pos);

#ifdef __cplusplus
}
#endif

#endif // DOOR_SERVO_H
