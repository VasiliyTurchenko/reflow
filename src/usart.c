/** @file usart.c
 *  @brief
 *
 *  @author turchenkov@gmail.com
 *  @bug
 *  @date 2025-11-08
 */
/*! file usart.c
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

#include "usart.h"
#include "error_handler.h"

UART_HandleTypeDef       huart1;
extern DMA_HandleTypeDef hdma_usart1_tx;
DMA_HandleTypeDef        hdma_usart1_tx;

static void (*UART1_cb_)(void) = NULL;

/**
 * @brief HAL_UART_TxCpltCallback
 * @param huart
 */
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart == &huart1) {
        if (NULL != UART1_cb_) {
            UART1_cb_();
        }
    }
}

/* */
void UART1_Init(void (*cb)(void), uint32_t br)
{
    UART1_cb_                = cb;
    huart1.Instance          = USART1;
    huart1.Init.BaudRate     = br;
    huart1.Init.WordLength   = UART_WORDLENGTH_8B;
    huart1.Init.StopBits     = UART_STOPBITS_1;
    huart1.Init.Parity       = UART_PARITY_NONE;
    huart1.Init.Mode         = UART_MODE_TX_RX;
    huart1.Init.HwFlowCtl    = UART_HWCONTROL_NONE;
    huart1.Init.OverSampling = UART_OVERSAMPLING_16;
    if (HAL_UART_Init(&huart1) != HAL_OK) {
        Error_Handler();
    }
}

/* */
void HAL_UART_MspInit(UART_HandleTypeDef *uartHandle)
{
    GPIO_InitTypeDef GPIO_InitStruct = { 0 };
    if (uartHandle->Instance == USART1) {
        __HAL_RCC_USART1_CLK_ENABLE();
        __HAL_RCC_GPIOA_CLK_ENABLE();
        /*
     * USART1 GPIO Configuration
    *  PA9     ------> USART1_TX
    *  PA10    ------> USART1_RX
    */
        GPIO_InitStruct.Pin   = GPIO_PIN_9;
        GPIO_InitStruct.Mode  = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

        GPIO_InitStruct.Pin  = GPIO_PIN_10;
        GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

        /* USART1 DMA Init */
        /* USART1_TX Init */
        hdma_usart1_tx.Instance                 = DMA1_Channel4;
        hdma_usart1_tx.Init.Direction           = DMA_MEMORY_TO_PERIPH;
        hdma_usart1_tx.Init.PeriphInc           = DMA_PINC_DISABLE;
        hdma_usart1_tx.Init.MemInc              = DMA_MINC_ENABLE;
        hdma_usart1_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
        hdma_usart1_tx.Init.MemDataAlignment    = DMA_MDATAALIGN_BYTE;
        hdma_usart1_tx.Init.Mode                = DMA_NORMAL;
        hdma_usart1_tx.Init.Priority            = DMA_PRIORITY_HIGH;
        if (HAL_DMA_Init(&hdma_usart1_tx) != HAL_OK) {
            Error_Handler();
        }

        __HAL_LINKDMA(uartHandle, hdmatx, hdma_usart1_tx);

        /* USART1 interrupt Init */
        HAL_NVIC_SetPriority(USART1_IRQn, 6, 0);
        HAL_NVIC_EnableIRQ(USART1_IRQn);
    }
}

/* */
void HAL_UART_MspDeInit(UART_HandleTypeDef *uartHandle)
{
    if (uartHandle->Instance == USART1) {
        /* Peripheral clock disable */
        __HAL_RCC_USART1_CLK_DISABLE();

        /*
     * USART1 GPIO Configuration
     * PA9     ------> USART1_TX
     * PA10    ------> USART1_RX
    */
        HAL_GPIO_DeInit(GPIOA, GPIO_PIN_9 | GPIO_PIN_10);

        /* USART1 DMA DeInit */
        HAL_DMA_DeInit(uartHandle->hdmatx);

        /* USART1 interrupt Deinit */
        HAL_NVIC_DisableIRQ(USART1_IRQn);
    }
}

/* */
void UART1_DeInit(void)
{
    HAL_UART_MspDeInit(&huart1);
}
