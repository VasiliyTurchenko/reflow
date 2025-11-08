/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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

/* Includes ------------------------------------------------------------------*/
#include "hal_ll.h"
#include "main.h"
#include "cmsis_os.h"
#include "adc.h"
#include "crc.h"
#include "dma.h"
#include "i2c.h"
#include "iwdg.h"
#include "rtc.h"
#include "spi.h"
#include "tim.h"

#include "wwdg.h"
#include "gpio.h"
#include "debug_sink.h"
#include "startup.h"
#include "logging.h"

#include "exti.h"

#ifdef USE_FRAMEBUFFER

#include "framebuffer.h"
#include "ssd1306.h"

#endif

volatile uint32_t SPI2_TxCplt_flag;
volatile uint32_t RX_ready_flag;

void SystemClock_Config(void);
void MX_FREERTOS_Init(void);

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
    (void)HAL_Init();
    SystemClock_Config();
    RCC_CSR_copy = RCC->CSR;
    SET_BIT(RCC->CSR, RCC_CSR_RMVF); /* clear the reboot flags */
    MX_GPIO_Init();
    MX_DMA_Init();
    MX_ADC1_Init();
    MX_CRC_Init();
    MX_I2C1_Init();
    //  MX_IWDG_Init();
    MX_SPI2_Init();
    MX_TIM2_Init();
    MX_TIM3_Init();

    //  MX_WWDG_Init();
    MX_RTC_Init();

    (void)HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);
    (void)HAL_TIM_Base_Start_IT(&htim3);

    if (AppStartUp() != SUCCESS) {
        NVIC_SystemReset();
    }

    HAL_Delay(200U);

    debug_sink_switch_to_RTOS();

    MX_FREERTOS_Init();

    debug_sink_update_mutexes();

    RESULT_UNUSED osKernelStart();

    /* We should never get here as control is now taken by the scheduler */
    /* Infinite loop */
    while (true) {
    }
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
    RCC_OscInitTypeDef       RCC_OscInitStruct = { 0 };
    RCC_ClkInitTypeDef       RCC_ClkInitStruct = { 0 };
    RCC_PeriphCLKInitTypeDef PeriphClkInit     = { 0 };

    /** Initializes the CPU, AHB and APB busses clocks
  */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSI | RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState       = RCC_HSE_ON;
    RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
    RCC_OscInitStruct.HSIState       = RCC_HSI_ON;
    RCC_OscInitStruct.LSIState       = RCC_LSI_ON;
    RCC_OscInitStruct.PLL.PLLState   = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource  = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLMUL     = RCC_PLL_MUL9;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
        Error_Handler();
    }
    /** Initializes the CPU, AHB and APB busses clocks
  */
    RCC_ClkInitStruct.ClockType =
            RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource   = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider  = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK) {
        Error_Handler();
    }
    PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_RTC | RCC_PERIPHCLK_ADC;
    PeriphClkInit.RTCClockSelection    = RCC_RTCCLKSOURCE_LSI;
    PeriphClkInit.AdcClockSelection    = RCC_ADCPCLK2_DIV6;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK) {
        Error_Handler();
    }
    /** Enables the Clock Security System
  */
    HAL_RCC_EnableCSS();
}
#if (0)
void SystemClock_Config(void)
{
    RCC_OscInitTypeDef       RCC_OscInitStruct = { 0 };
    RCC_ClkInitTypeDef       RCC_ClkInitStruct = { 0 };
    RCC_PeriphCLKInitTypeDef PeriphClkInit     = { 0 };

    /** Initializes the CPU, AHB and APB busses clocks
  */
    RCC_OscInitStruct.OscillatorType =
            RCC_OSCILLATORTYPE_LSI | RCC_OSCILLATORTYPE_HSE | RCC_OSCILLATORTYPE_LSE;
    RCC_OscInitStruct.HSEState       = RCC_HSE_ON;
    RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
    RCC_OscInitStruct.LSEState       = RCC_LSE_ON;
    RCC_OscInitStruct.HSIState       = RCC_HSI_ON;
    RCC_OscInitStruct.LSIState       = RCC_LSI_ON;
    RCC_OscInitStruct.PLL.PLLState   = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource  = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLMUL     = RCC_PLL_MUL9;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
        Error_Handler();
    }
    /** Initializes the CPU, AHB and APB busses clocks
  */
    RCC_ClkInitStruct.ClockType =
            RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource   = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider  = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK) {
        Error_Handler();
    }
    PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_RTC | RCC_PERIPHCLK_ADC;
    PeriphClkInit.RTCClockSelection    = RCC_RTCCLKSOURCE_LSE;
    PeriphClkInit.AdcClockSelection    = RCC_ADCPCLK2_DIV6;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK) {
        Error_Handler();
    }
    /** Enables the Clock Security System
  */
    HAL_RCC_EnableCSS();
}
#endif

/* USER CODE BEGIN 4 */

/**
 * @brief HAL_SPI_RxCpltCallback
 * @param hspi
 */
void HAL_SPI_RxCpltCallback(SPI_HandleTypeDef *hspi)
{
    if (hspi == &hspi2) {
        RX_ready_flag = 1U;
    }
}

/**
 * @brief HAL_SPI_TxCpltCallback
 * @param hspi
 */
void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi)
{
    if (hspi == &hspi2) {
        SPI2_TxCplt_flag = 1U;
#ifdef USE_FRAMEBUFFER
        extern struct_Context *context1;
        (void)CompleteUpdateSSD1306_callback(hspi, context1->pDevFB);
#endif
    }
}

/**
 * @brief HAL_SPI_ErrorCallback
 * @param hspi
 */
void HAL_SPI_ErrorCallback(SPI_HandleTypeDef *hspi)
{
    volatile uint32_t ec1;

    if (hspi == &hspi2) {
        ec1 = (*hspi).ErrorCode;
    }
}

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM1 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    /* USER CODE BEGIN Callback 0 */

    /* USER CODE END Callback 0 */
    if (htim->Instance == TIM1) {
        HAL_IncTick();
    }
    /* USER CODE BEGIN Callback 1 */
    if (htim->Instance == TIM1) {
        /* increment us counter */
        /* add 1000 us */
        counter_1us += 1000U;

#ifdef USE_FRAMEBUFFER
        if ((HAL_GetTick() % 64U) == 0U) {
            if (context1->pDevFB->fUpdateScreen != NULL) {
                context1->pDevFB->fUpdateScreen(&hspi2, context1->pDevFB);
            }
        }
#endif
    }

#if defined(DEBUG_USART)
    static uint32_t decimator = 0U;
    if (Transmit_non_RTOS) {
        if (htim->Instance == TIM1) {
            if (0U == decimator) {
                extern ErrorStatus Transmit(void);
                RESULT_UNUSED      Transmit();
                decimator = 11U;
            }
            decimator--;
        }
    }

#endif

#if (0)
    if (htim->Instance == TIM3) {
        HAL_GPIO_TogglePin(BOOST_HEATER_GPIO_Port, BOOST_HEATER_Pin);
        if ((HAL_GetTick() & 0x01U) == 0U) {
            BaseType_t xHigherPriorityTaskWoken;
            if (xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED) {
                extern osThreadId kbd_taskHandle;
                if (xTaskNotifyFromISR(kbd_taskHandle, 1U, eSetValueWithOverwrite,
                                       &xHigherPriorityTaskWoken) != pdPASS) {
                    // error
                }
                portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
            }
        }
    }
#endif
}
