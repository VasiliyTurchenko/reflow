/** @file regulator.h
 *  @brief temperature regulator
 *
 *  @author Vasiliy Turchenko
 *  @bug
 *  @date 08-Feb-2020
 */

#ifndef REGULATOR_H
#define REGULATOR_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stddef.h>
#include <limits.h>

#include "control_task.h"

uint8_t get_heater_throttle(uint16_t t_actual, uint16_t time);
int8_t pid(uint16_t target, uint16_t temp);

#ifdef __cplusplus
}
#endif

#endif // REGULATOR_H
