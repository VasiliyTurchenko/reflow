/** @file error_handler.h
 *  @brief
 *
 *  @author turchenkov@gmail.com
 *  @bug
 *  @date 2019-01-01
 */
/*! file error_handler.h
 * Copyright (c) 2019-01-01 turchenkov@gmail.com
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

#ifndef ERROR_HANDLER_H
#define ERROR_HANDLER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#include "compiler_defs.h"

void ATTR_NORETURN Error_Handler(void);

#ifdef USE_FULL_ASSERT
void assert_failed(const char *file, uint32_t line);
#else
#define assert_failed
#endif

#ifdef __cplusplus
}
#endif

#endif // ERROR_HANDLER_H
