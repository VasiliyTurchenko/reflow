/** @file door_pwm.h
 *  @brief Oven Door PWM functions
 *
 *  @author Vasiliy Turchenko
 *  @bug
 *  @date 21-Jan-2020
 */

#ifndef DOOR_PWM_H
#define DOOR_PWM_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stddef.h>
#include <limits.h>
#include <stdbool.h>

typedef struct __attribute__((packed)) door_pwm_cal_data {

	uint32_t	open_PWM;
	uint32_t	closed_PWM;
	_Bool	valid;

}	door_pwm_cal_data_t;

#ifdef __cplusplus
}
#endif

#endif // DOOR_PWM_H
