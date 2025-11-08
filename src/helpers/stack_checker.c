/** @file stack_checker.c
 *  @brief
 *
 *  @author turchenkov@gmail.com
 *  @bug
 *  @date 2025-11-08
 */
/*! file stack_checker.c
 * Copyright (c) 2025-11-08 turchenkov@gmail.com
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
#include "stack_checker.h"
#include "uni_vector.h"
#include "logging.h"

/**
 * @brief stack_checker_check
 */
UBaseType_t stack_checker_check(UBaseType_t old_s)
{
    UBaseType_t retVal = 0U;
    if (xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED) {
        TaskHandle_t my = xTaskGetCurrentTaskHandle();
        UBaseType_t  wm = uxTaskGetStackHighWaterMark(my);

        if (wm < old_s) {
            retVal = wm;
#ifndef STACK_CHECK_SILENT
            log_xprintf(MSG_LEVEL_EXT_INF, "new min. stack watermark reached = %d", wm);
#endif
        }
    }
    return retVal;
}

/*
****************************** E. O. F. ***************************** */
