/**
  ******************************************************************************
  * File Name          : ssd1306.c
  * Description        : This file provides code for all the functions
  *                      related to ssd1306 OLED controller
  ******************************************************************************
  * v.0.0.1
  * 17/11/2017
  * v.0.0.2
  * 09/12/2017
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/

#include "display_pins.h"
#include "ssd1306.h"

#ifdef USE_FRAMEBUFFER

#define SET_LOWER_NIBBLE_CA (uint8_t)0x00
#define SET_HIGHER_NIBBLE_CA (uint8_t)0x10
#define COLUMN_ADDRESS (uint8_t)0x00

#define SET_MEM_ADDR_MODE_CMD (uint8_t)0x20
#define ADDR_MODE_HOR (uint8_t)0x00
#define ADDR_MODE_VERT (uint8_t)0x01
#define ADDR_MODE_PAGES (uint8_t)0x10

#define SET_COL_ADDR_CMD (uint8_t)0x21

#define HOR_SCROLL_ON (uint8_t)0x2F
#define HOR_SCROLL_OFF (uint8_t)0x2E

#define HOR_SCROLL_SETUP (uint8_t)0x26

#define START_LINE_ADDRESS_CMD (uint8_t)0x40
#define DISPLAY_START_LINE (uint8_t)0

#define SET_CONTRAST_CR (uint8_t)0x81
#define CONTRAST (uint8_t)0xFF

#define SET_BRIGHTNESS_CB (uint8_t)0x82
#define COL_BANK_BIRGHTNESS (uint8_t)0xFF

#define SET_REMAP_0_0 (uint8_t)0xA0
#define SET_REMAP_132_0 (uint8_t)0xA1

#define SET_DISPLAY_RAM (uint8_t)0xA4
#define SET_DISPLAY_ALL_ON (uint8_t)0xA5

#define SET_DISPLAY_NORM (uint8_t)0xA6
#define SET_DISPLAY_INV (uint8_t)0xA7

#define SET_MUX_RATIO (uint8_t)0xA8
#define MUX_RATIO (uint8_t)63

#define SET_DCDC (uint8_t)0xAD
#define DCDC_ON (uint8_t)0x8B
#define DCDC_OFF (uint8_t)0x8A

#define SET_DISP_OFF (uint8_t)0xAE
#define SET_DISP_ON (uint8_t)0xAF

#define SET_PAGE_ADDR_CMD (uint8_t)0xB0

#define COM_REMAP (uint8_t)(0x01 << 3)
#define COM_NO_REMAP (uint8_t)0x00
#define SET_COM_OUT_DIR_CMD (uint8_t)0xC0

#define SET_DISP_OFFSET (uint8_t)0xD3

#define SET_DISP_CLK_DIV (uint8_t)0xD5

#define SET_AREA_COLOR_MODE (uint8_t)0xD8
#define COL_MODE_LP(CM, LP)	(uint8_t)((CM)<<5) | ((CM)<<4) | ((LP)<<2) | (LP))
#define CM_MONO (uint8_t)0x00
#define CM_COLOR (uint8_t)0x01
#define NORM_PWR (uint8_t)0x00
#define LOW_PWR (uint8_t)0x01

#define SET_PRE_CHARGE_PERIOD (uint8_t)0xD9

#define SET_COM_PIN_CFG (uint8_t)0xDA
#define COM_PIN_CFG_SEQ (uint8_t)0x02
#define COM_PIN_CFG_ALT (uint8_t)0x12

#define SET_VCOM_LVL (uint8_t)0xDB
#define LOW_VCOM (uint8_t)0x00
#define HIGH_VCOM (uint8_t)0x35

#define NOP_CMD (uint8_t)0xE3

/*---------------------the function updates ssd1306-controlled screen */
uint8_t UpdateSSD1306(SPI_HandleTypeDef *OLED_hspi, ptr_fb fb)
{
	extern volatile uint32_t SPI2_TxCplt_flag;
	if (fb->Locked)
		return 1U;
	fb->Locked = true;
	SPI2_TxCplt_flag = 0U;
	DCD;
	CSL;
	HAL_SPI_Transmit_DMA(OLED_hspi, fb->pFB, (128 / CHAR_BIT) * (64));
	return 0U;
}
/*----------------End of the function updates ssd1306-controlled screen */

/* --------------- command out to ssd1306 ----*/
void WriteCmdSSD1306(SPI_HandleTypeDef *OLED_hspi, uint8_t cmd)
{
	DCC;
	CSL;
	HAL_SPI_Transmit(OLED_hspi, &cmd, 0x01, 0x500);
	CSH;
	DCD;
} // end of WriteCmdSSD1306

/*	CompleteUpdateSSD1306_callback called when transmit process is done
*/
uint8_t CompleteUpdateSSD1306_callback(SPI_HandleTypeDef *OLED_hspi, ptr_fb fb)
{
	WriteCmdSSD1306(OLED_hspi, NOP_CMD);
	CSH;
	fb->Locked = false;
	return 0U;
}
/*	end of CompleteUpdateSSD1306_callback called when transmit process is done */

/*----
*	Init SSD1306
*	06-12-2017
*	23:01
*/
uint8_t InitSSD1306(SPI_HandleTypeDef *OLED_hspi)
{
	/* Init LCD */
	RESL;
	HAL_Delay(1);
	RESH;

	WriteCmdSSD1306(OLED_hspi, 0xAE); //display off
	WriteCmdSSD1306(OLED_hspi, 0x20); //Set Memory Addressing Mode
	//	WriteCmdSSD1306(OLED_hspi, 0x10); //00,Horizontal Addressing Mode;01,Vertical Addressing Mode;10,Page Addressing Mode (RESET);11,Invalid
	WriteCmdSSD1306(OLED_hspi, 0x00);
	WriteCmdSSD1306(
		OLED_hspi,
		0xB0); //Set Page Start Address for Page Addressing Mode,0-7
	WriteCmdSSD1306(OLED_hspi, 0xC8); //Set COM Output Scan Direction
	WriteCmdSSD1306(OLED_hspi, 0x00); //---set low column address
	WriteCmdSSD1306(OLED_hspi, 0x10); //---set high column address
	WriteCmdSSD1306(OLED_hspi, 0x40); //--set start line address
	WriteCmdSSD1306(OLED_hspi, 0x81); //--set contrast control register
	WriteCmdSSD1306(OLED_hspi, 0xFF);
	WriteCmdSSD1306(OLED_hspi, 0xA1); //--set segment re-map 0 to 127
	WriteCmdSSD1306(OLED_hspi, 0xA6); //--set normal display
	WriteCmdSSD1306(OLED_hspi, 0xA8); //--set multiplex ratio(1 to 64)
	WriteCmdSSD1306(OLED_hspi, 0x3F); //
	WriteCmdSSD1306(
		OLED_hspi,
		0xA4); //0xa4,Output follows RAM content;0xa5,Output ignores RAM content
	WriteCmdSSD1306(OLED_hspi, 0xD3); //-set display offset
	WriteCmdSSD1306(OLED_hspi, 0x00); //-not offset
	WriteCmdSSD1306(
		OLED_hspi,
		0xD5); //--set display clock divide ratio/oscillator frequency
	WriteCmdSSD1306(OLED_hspi, 0xF0); //--set divide ratio
	WriteCmdSSD1306(OLED_hspi, 0xD9); //--set pre-charge period
	WriteCmdSSD1306(OLED_hspi, 0x22); //
	WriteCmdSSD1306(OLED_hspi,
			0xDA); //--set com pins hardware configuration
	WriteCmdSSD1306(OLED_hspi, 0x12);
	WriteCmdSSD1306(OLED_hspi, 0xDB); //--set vcomh
	WriteCmdSSD1306(OLED_hspi, 0x20); //0x20,0.77xVcc
	WriteCmdSSD1306(OLED_hspi, 0x8D); //--set DC-DC enable
	WriteCmdSSD1306(OLED_hspi, 0x14); //
	WriteCmdSSD1306(OLED_hspi, 0xAF); //--turn on SSD1306 panel
	/* Return OK */
	return 1;
}

uint8_t InitSSD1303(SPI_HandleTypeDef *OLED_hspi)
{
#define WriteCmdSSD1303 WriteCmdSSD1306
	/* Init LCD */
	RESL;
	HAL_Delay(1);
	RESH;

	WriteCmdSSD1303(OLED_hspi, SET_DISP_OFF); //display off
	WriteCmdSSD1303(OLED_hspi, SET_VCOM_LVL);
	WriteCmdSSD1303(OLED_hspi, HIGH_VCOM); //0x20,0.77xVcc
	WriteCmdSSD1303(OLED_hspi, SET_PRE_CHARGE_PERIOD);
	WriteCmdSSD1303(OLED_hspi, 0x22); //--set pre-charge period
	WriteCmdSSD1303(OLED_hspi,
			SET_REMAP_0_0); //--set segment re-map 132 -> 0

	WriteCmdSSD1303(OLED_hspi,
			SET_COM_OUT_DIR_CMD |
				COM_REMAP); //Set COM Output Scan Direction

	WriteCmdSSD1303(OLED_hspi, SET_COM_PIN_CFG);
	WriteCmdSSD1303(
		OLED_hspi,
		COM_PIN_CFG_ALT); //--set com pins hardware configuration
	WriteCmdSSD1303(OLED_hspi, SET_MUX_RATIO);
	WriteCmdSSD1303(OLED_hspi, MUX_RATIO); //--set multiplex ratio(1 to 64)
	WriteCmdSSD1303(OLED_hspi, SET_DISP_CLK_DIV);
	WriteCmdSSD1303(
		OLED_hspi,
		0x71 /*F0*/); //--set display clock divide ratio/oscillator frequency
	WriteCmdSSD1303(OLED_hspi, SET_CONTRAST_CR);
	WriteCmdSSD1303(OLED_hspi,
			CONTRAST >> 1); //--set contrast control register
	WriteCmdSSD1303(OLED_hspi, SET_DISP_OFFSET);
	WriteCmdSSD1303(OLED_hspi, 0x00);	     //-set display offset
	WriteCmdSSD1303(OLED_hspi, SET_DISPLAY_NORM); //--set normal display
	WriteCmdSSD1303(OLED_hspi, SET_DCDC);
	WriteCmdSSD1303(OLED_hspi, DCDC_ON); //--set DC-DC enable
	//	WriteCmdSSD1303(OLED_hspi, HOR_SCROLL_SETUP); WriteCmdSSD1303(OLED_hspi, 0x01); WriteCmdSSD1303(OLED_hspi, 0x00);
	//							WriteCmdSSD1303(OLED_hspi, 0x00);  WriteCmdSSD1303(OLED_hspi, 0x00);
	//	WriteCmdSSD1303(OLED_hspi, HOR_SCROLL_ON);
	WriteCmdSSD1303(OLED_hspi, HOR_SCROLL_OFF);
	WriteCmdSSD1303(OLED_hspi, SET_MEM_ADDR_MODE_CMD);
	WriteCmdSSD1303(OLED_hspi, ADDR_MODE_HOR);

	WriteCmdSSD1303(OLED_hspi, SET_COL_ADDR_CMD);
	WriteCmdSSD1303(OLED_hspi, 0);
	WriteCmdSSD1303(OLED_hspi, 127);

	WriteCmdSSD1303(OLED_hspi,
			SET_LOWER_NIBBLE_CA |
				(2 & 0x0F)); //---set low column address
	WriteCmdSSD1303(OLED_hspi, SET_HIGHER_NIBBLE_CA |
					   (0)); //---set high column address
	WriteCmdSSD1303(OLED_hspi, 2);
	WriteCmdSSD1303(OLED_hspi, 0x10);
	WriteCmdSSD1303(
		OLED_hspi,
		SET_DISPLAY_RAM); //0xa4,Output follows RAM content;0xa5,Output ignores RAM content
	WriteCmdSSD1303(OLED_hspi,
			START_LINE_ADDRESS_CMD |
				DISPLAY_START_LINE); //--set start line address
	WriteCmdSSD1303(OLED_hspi, SET_AREA_COLOR_MODE); WriteCmdSSD1303(OLED_hspi, COL_MODE_LP(CM_MONO, NORM_PWR);
	WriteCmdSSD1303(OLED_hspi, SET_BRIGHTNESS_CB); WriteCmdSSD1303(OLED_hspi, COL_BANK_BIRGHTNESS); //--set contrast control register
//	WriteCmdSSD1303(OLED_hspi, SET_PAGE_ADDR_CMD | 0x00); //Set Page Start Address for Page Addressing Mode,0-7

	WriteCmdSSD1303(OLED_hspi, SET_DISP_ON); //--turn on SSD1306 panel
	/* Return OK */
	return 1;
}
#endif
/************************ (C) COPYRIGHT turchenkov@gmail.com *****END OF FILE****/
