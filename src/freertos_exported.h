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

#include "cmsis_os.h"
#include "mutex_helpers.h"

EXPORT_MUTEX_HANDLE(xfunc_out)
EXPORT_MUTEX_HANDLE(logging)

extern osThreadId comm_taskHandle;

#ifdef __cplusplus
}
#endif

#endif // FREERTOS_EXPORTED_H
