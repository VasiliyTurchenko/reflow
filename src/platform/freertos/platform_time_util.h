/** @file platform_time_util.f
 *  @brief platform-dependent time functions
 *
 *  @author turchenkov@gmail.com
 *  @bug
 *  @date 30-Sep-2021
 */
/*! file platform_time_util.c
 * Copyright (c) 30-Sep-2021 turchenkov@gmail.com
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software and
 * associated documentation files (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge, publish, distribute,
 * sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
 * PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR
 * OTHER LIABILITY,  WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR
 * IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#ifndef PLATFORM_TIME_UTIL_H
#define PLATFORM_TIME_UTIL_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stddef.h>
#include "compiler_defs.h"

/* delay without watchdog reloading */
void sys_helpers_delay_ms(uint32_t ms);

/* reload MCU giving time to get logs printed out  */
void ATTR_NORETURN sys_helpers_delay_ms_and_reboot(uint32_t ms);

/* long delays with watchdog reload */
void sys_helpers_delay_ms_wd(uint32_t ms, uint32_t wdt_ms, void (*rld)(void));

/* returns native system milliseconds ticks */
uint32_t time_now_ms(void);

/* 64-bit version of time_now_ms */
uint64_t time_now_ms64(void);

/**
 * @brief update_timeout
 * @param ts
 */
static inline void reset_timeout(uint32_t *ts)
{
    uint32_t now = time_now_ms();
    if (now >= *ts) {
        *ts = now - *ts;
    } else {
        *ts = *ts + 1U + (UINT32_MAX - *ts);
    }
}

#ifdef __cplusplus
}
#endif

#endif /* PLATFORM_TIME_UTIL_H */
