/** @file temperatur_task.c
 *  @brief
 *
 *  @author Vasiliy Turchenko
 *  @bug
 *  @date 20-Jan-2020
 */

#include "temperatur_task.h"
#include "watchdog.h"
#include "task_tokens.h"
#include "logging.h"

#ifdef STM32F103xB
#include "stm32f1xx.h"
#else
#error MCU NOT DEFINED
#endif



