/** @file platform_time_util.c
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

#include "cmsis_os.h"
#include "platform_time_util.h"
#include "platform_reboot.h"
#include "logging.h"

/**
 * @brief time_now_ms64
 * @return
 */
uint64_t time_now_ms64(void)
{
    uint32_t ms_now = time_now_ms();
    uint64_t retVal = (uint64_t)ms_now;

    static uint32_t old_ticks = 0x10101010U; /* it's a magic number */
    if (old_ticks == 0x10101010U) {
        old_ticks = ms_now; /* runs once */
    }

    if (old_ticks > ms_now) {
        /* 32bit value is wrapped around 0 - it happens every 49 days 17 hours */
        const uint64_t a = 0x100000000U;
        retVal			 = retVal + a; /* postpone wrappind to 100 day cycle */
    }
    old_ticks = ms_now;
    return retVal;
}

/**
 * @brief time_now_ms
 * @return
 */
uint32_t time_now_ms(void)
{
    if (xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED) {
        return xTaskGetTickCount();
    } else {
        return HAL_GetTick();
    }
}


/**
 * @brief sys_helpers_delay_ms
 * @param ms
 */
void sys_helpers_delay_ms(uint32_t ms)
{
    if (ms > 0U) {
        if (xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED) {
            vTaskDelay(pdMS_TO_TICKS(ms));
        } else {
            HAL_Delay(ms);
        }
    }
}

/**
 * @brief sys_helpers_delay_ms_and_reboot
 * @param ms
 */
void __NO_RETURN sys_helpers_delay_ms_and_reboot(uint32_t ms)
{
    log_xputs(MSG_LEVEL_FATAL, "going to reboot!");
    sys_helpers_delay_ms(ms);
    platform_reboot();
}

/**
 * @brief sys_helpers_delay_ms_wd
 * @param ms time to delay
 * @param wdt_ms wdt reload time, ms
 * @param rld pointer to reload function
 */
void sys_helpers_delay_ms_wd(uint32_t ms, uint32_t wdt_ms, void (*rld)(void))
{
    uint32_t left = ms;
    if ((ms > 0U) && (ms < wdt_ms)) {
        sys_helpers_delay_ms(ms);
        return;
    }
    do {
        uint32_t next_ms = (left < wdt_ms) ? left : wdt_ms;
        sys_helpers_delay_ms(next_ms);
        if (rld != NULL) {
            rld();
        }
        left -= next_ms;
    } while (left > 0U);
}
