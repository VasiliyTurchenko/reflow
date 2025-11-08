/** @file error_handler.c
 *  @brief
 *
 *  @author turchenkov@gmail.com
 *  @bug
 *  @date 2019-01-01
 */
/*! file error_handler.c
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

#include "error_handler.h"
#include "logging.h"
#include "platform_time_util.h"

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void ATTR_NORETURN Error_Handler(void)
{
    //    __disable_irq();
    sys_helpers_delay_ms_and_reboot(200U);
    while (1) {
    }
}

#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(const char *file, uint32_t line)
{
    int   line_ = (int)(line & INT32_MAX);
    const char *file_ = (NULL == file) ? "unknown" : file;

    log_xprintf(MSG_LEVEL_FATAL, "assertion failed at %d in %s", line_, file_);
    sys_helpers_delay_ms_and_reboot(100U);
}
#endif /* USE_FULL_ASSERT */
