/**
 * @file watchdog.h
 * @author Vasiliy Turchenko
 * @date 06-Feb-2018
 * @version 0.0.1
 *
 */
#ifndef _WATCHDOG_H
#define _WATCHDOG_H

#include <stdint.h>
/* watchdog test */

#define IWD_TEST                                                                                   \
    do {                                                                                           \
        if (HAL_GetTick() > 10000U) {                                                              \
            for (;;) {                                                                             \
                ;                                                                                  \
            }                                                                                      \
        }                                                                                          \
    } while (0U)

#define WDT_REG 8U /* BKP_REG number to store */

void register_magic(const uint32_t magic_pos);
void i_am_alive(uint32_t const magic);

#endif
