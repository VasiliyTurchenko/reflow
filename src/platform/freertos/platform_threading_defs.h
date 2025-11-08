/** @file platform_threading_defs.h
 *  @brief portable threading related stuff for FreeRTOS
 *  @author Vasiliy Turchenko
 *  @date 23-Aug-2023
 */

#ifndef PLATFORM_THREADING_DEFS__H
#define PLATFORM_THREADING_DEFS__H

#ifdef __cplusplus
extern "C" {
#endif

#include "FreeRTOS.h"                   // ARM.FreeRTOS::RTOS:Core
#include "task.h"                       // ARM.FreeRTOS::RTOS:Core
#include "event_groups.h"               // ARM.FreeRTOS::RTOS:Event Groups
#include "semphr.h"                     // ARM.FreeRTOS::RTOS:Core

#include "cmsis_os.h"

#include "mutex_helpers.h"

extern osMutexId logging_MutexHandle;

#ifdef __cplusplus
}
#endif

#endif /* PLATFORM_THREADING_DEFS__H */
