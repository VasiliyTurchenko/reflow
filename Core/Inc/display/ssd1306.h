/**
  ******************************************************************************
  * File Name          : ssd1306.h
  * Description        : This file contains all the functions prototypes for
  *                      the ssd1306 OLED controller
  ******************************************************************************
	*	v.0.0.1
	* 18-11-2017
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef SSD1306_H
#define SSD1306_H

#include "globaldef.h"

#ifdef		USE_FRAMEBUFFER

#include <stdint.h>
#include "framebuffer.h"
#include "spi.h"


uint8_t		UpdateSSD1306(SPI_HandleTypeDef *OLED_hspi, ptr_fb fb);
uint8_t		CompleteUpdateSSD1306_callback(SPI_HandleTypeDef *OLED_hspi, ptr_fb fb);
uint8_t		InitSSD1306(SPI_HandleTypeDef *OLED_hspi);
uint8_t		InitSSD1303(SPI_HandleTypeDef *OLED_hspi);

#endif /* USE_FRAMEBUFFER */
#endif /* __ssd1306_H */
/************************ (C) COPYRIGHT turchenkov@gmail.com *****END OF FILE****/
