/** @file startup.h
 *  @brief app startup routines header file
 *
 *  @author Vasiliy Turchenko
 *  @bug
 *  @date 14-Jan-2020
 */

#ifndef STARTUP_H
#define STARTUP_H

#ifdef __cplusplus
 extern "C" {
#endif

#include <stdint.h>
#include <stddef.h>
#include <limits.h>
#include <stdbool.h>

#ifdef STM32F103xB
        #include "stm32f1xx_hal.h"
#endif

extern volatile	uint32_t	RCC_CSR_copy;		/* copy of the RCC_CSR */
extern volatile uint32_t	Saved_Magic_Bits;	/* watchdog */

extern volatile bool Transmit_non_RTOS;


ErrorStatus AppStartUp(void);

#ifdef __cplusplus
 }
#endif

#endif
