/** @file usart.h
 *  @brief
 *
 *  @author turchenkov@gmail.com
 *  @bug
 *  @date 2025-11-08
 */
/*! file usart.h
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

#ifndef USART_H
#define USART_H

#ifdef __cplusplus
/**
 * @brief UART1_Init
 */
extern "C" {
#endif

#include "hal_ll.h"

extern UART_HandleTypeDef huart1;

void UART1_Init(void (*cb)(void));
void UART1_DeInit(void);

void HAL_UART_MspDeInit(UART_HandleTypeDef *uartHandle);
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart);

#if defined(DEBUG_USART)
#if (DEBUG_USART == 1)
#define DEBUG_USART_Init    UART1_Init
#define DEBUG_USART_DeInit  UART1_DeInit
#define DEBUG_huart         huart1
#endif
#endif

#ifdef __cplusplus
}
#endif

#endif // USART_H
