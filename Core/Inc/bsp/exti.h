/** @file exti.h
 *  @brief External interrupt(s) routines
 *
 *  @author Vasiliy Turchenko
 *  @bug
 *  @date 19-Jan-2020
 */

#ifndef EXTI_H
#define EXTI_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stddef.h>
#include <limits.h>

extern uint64_t	counter_1us;
extern uint32_t	half_periods_cnt;

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin);
uint32_t get_mains_half_period(void);

void enable_exti_notifications(void);
void disable_exti_notifications(void);


#ifdef __cplusplus
}
#endif

#endif
