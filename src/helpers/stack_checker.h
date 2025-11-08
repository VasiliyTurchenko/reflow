/** @file stack_checker.h
 *  @brief
 *
 *  @author turchenkov@gmail.com
 *  @bug
 *  @date 2021-Sep-16
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

#ifndef STACK_CHECKER_H
#define STACK_CHECKER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stddef.h>

#ifdef USE_FreeRTOS
#include "cmsis_os.h"

typedef struct {
    TaskHandle_t h;
    UBaseType_t  s;
} task_stack_info_t;

UBaseType_t stack_checker_check(UBaseType_t old_s);
#else
typedef struct {
    uint32_t h;
    size_t   s;
} task_stack_info_t;
#define stack_checker_check() (void)(0)
#endif

#ifdef __cplusplus
}
#endif

#endif // STACK_CHECKER_H
