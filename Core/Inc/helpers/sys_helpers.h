/** @file sys_helpers.h
 *  @brief
 *
 *  @author Vasiliy Turchenko
 *  @bug
 *  @date 22-Jan-2020
 */

#ifndef SYS_HELPERS_H
#define SYS_HELPERS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stddef.h>
#include <limits.h>

#ifdef STM32F103xB
#include "stm32f1xx.h"
#else
#error MCU NOT DEFINED
#endif


void sys_helpers_delay_ms(uint32_t ms);
void sys_helpers_delay_ms_and_reboot(uint32_t ms);

#ifdef __cplusplus
}
#endif

#endif // SYS_HELPERS_H
