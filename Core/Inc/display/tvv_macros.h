/**
  ******************************************************************************
  * @file    tvv_macros.h
  * @author  Vasiliy Turchenko
  * @version V0.0.1
  * @date    2-Dec-2017
  * @brief   Header file
  ******************************************************************************
  * @attention   
  *
  ******************************************************************************
  */
  /* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __tvv_macros_H
#define __tvv_macros_H

/*
#define		CSL		GPIOB->BSRR = (uint32_t)OLED_CS_Pin << 16U
#define		CSH		GPIOB->BSRR = OLED_CS_Pin
#define		RESL		GPIOB->BSRR = (uint32_t)OLED_RESET_Pin << 16U
#define		RESH		GPIOB->BSRR = OLED_RESET_Pin
#define		DCC		GPIOB->BSRR = (uint32_t)OLED_DC_Pin << 16U
#define		DCD		GPIOB->BSRR = OLED_DC_Pin
#define		T2EVL		GPIOB->BSRR = (uint32_t)TIM2_EVENT_Pin << 16U
#define		T2EVH		GPIOB->BSRR = TIM2_EVENT_Pin
*/
/*
#define		LED0L		GPIOC->BSRR = (uint32_t)LED_0_Pin << 16U
#define		LED0H		GPIOC->BSRR = LED_0_Pin
*/
#define		HEARTBEAT_L	GPIOB->BSRR = (uint32_t)HEARTBEAT_Pin << 16U
#define		HEARTBEAT_H	GPIOB->BSRR = HEARTBEAT_Pin

#endif

