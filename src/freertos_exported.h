/** @file freertos_exported.h
 *  @brief
 *
 *  @author Vasily.Turchenko@huld.io
 *  @bug
 *  @date 2024-10-18
 */

#ifndef FREERTOS_EXPORTED_H
#define FREERTOS_EXPORTED_H

#ifdef __cplusplus
extern "C" {
#endif
#include <stdbool.h>

#include "FreeRTOS.h"
#include "semphr.h"

#include "mutex_helpers.h"

EXPORT_MUTEX_HANDLE(xfunc_out)
EXPORT_MUTEX_HANDLE(logging)
EXPORT_MUTEX_HANDLE(spi2)

EXPORT_TASK_HANDLE(comm)
EXPORT_TASK_HANDLE(default)
EXPORT_TASK_HANDLE(kbd)
EXPORT_TASK_HANDLE(control)
EXPORT_TASK_HANDLE(temperatur)
EXPORT_TASK_HANDLE(ui)


#ifdef __cplusplus
}
#endif

#endif // FREERTOS_EXPORTED_H
