/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define LED0_Pin GPIO_PIN_13
#define LED0_GPIO_Port GPIOC
#define KEY_UP_Pin GPIO_PIN_2
#define KEY_UP_GPIO_Port GPIOA
#define KEY_DOWN_Pin GPIO_PIN_3
#define KEY_DOWN_GPIO_Port GPIOA
#define KEY_ENTER_Pin GPIO_PIN_4
#define KEY_ENTER_GPIO_Port GPIOA
#define KEY_ESC_Pin GPIO_PIN_5
#define KEY_ESC_GPIO_Port GPIOA
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

extern volatile uint32_t SPI2_TxCplt_flag;
extern volatile uint32_t RX_ready_flag;


/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
