/** @file main.h
 *  @brief
 *
 *  @author turchenkov@gmail.com
 *  @bug
 *  @date 2025-11-08
 */
/*! file main.h
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

#ifndef MAIN_H
#define MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stddef.h>

#include "hal_ll.h"

void Error_Handler(void);

#define RESULT_UNUSED	UNUSED

#define LED0_Pin GPIO_PIN_13
#define LED0_GPIO_Port GPIOC
// #define KEY_UP_Pin GPIO_PIN_2
// #define KEY_UP_GPIO_Port GPIOA
// #define KEY_DOWN_Pin GPIO_PIN_3
// #define KEY_DOWN_GPIO_Port GPIOA
// #define KEY_ENTER_Pin GPIO_PIN_4
// #define KEY_ENTER_GPIO_Port GPIOA
// #define KEY_ESC_Pin GPIO_PIN_5
// #define KEY_ESC_GPIO_Port GPIOA

#define COL0_PIN    GPIO_PIN_0
#define COL1_PIN    GPIO_PIN_1
#define COL0_GPIO_Port    GPIOA
#define COL1_GPIO_Port    GPIOA

#define ROW0_PIN    GPIO_PIN_2
#define ROW1_PIN    GPIO_PIN_3
#define ROW2_PIN    GPIO_PIN_4

#define ROW0_GPIO_Port    GPIOA
#define ROW1_GPIO_Port    GPIOA
#define ROW2_GPIO_Port    GPIOA

#define MAINS_Pin GPIO_PIN_6
#define MAINS_GPIO_Port GPIOA
#define MAINS_EXTI_IRQn EXTI9_5_IRQn
#define D_C_Pin GPIO_PIN_1
#define D_C_GPIO_Port GPIOB
#define SPI_CS_Pin GPIO_PIN_12
#define SPI_CS_GPIO_Port GPIOB
#define DISP_RESET_Pin GPIO_PIN_14
#define DISP_RESET_GPIO_Port GPIOB
#define BOTTOM_HEATER_Pin GPIO_PIN_4
#define BOTTOM_HEATER_GPIO_Port GPIOB
#define TOP_HEATER_Pin GPIO_PIN_5
#define TOP_HEATER_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

#define DEBUG_UART	huart1
#define AT24C04

/* define as 1 if only one thermocouple is installed */
/* define as 0 if there are two of them */
#define	SINGLE_TC	1

#define MIN_PWM_DURATION (3540U)
#define MAX_PWM_DURATION (3900U)

#define MID_PWM_DURATION ( MIN_PWM_DURATION + ((MAX_PWM_DURATION - MIN_PWM_DURATION) / 2U) )

extern volatile uint32_t SPI2_TxCplt_flag;
extern volatile uint32_t RX_ready_flag;


/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* MAIN_H */
