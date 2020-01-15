/** @file startup.c
 *  @brief app startup routines
 *
 *  @author Vasiliy Turchenko
 *  @bug
 *  @date 14-Jan-2020
 */

#include "FreeRTOS.h"
#include "task.h"
#include "cmsis_os.h"

#include <string.h>
#include <stdlib.h>

#include "rtc.h"

//#include "rtc_magics.h"

//#include "config_files.h"

#include "logging.h"
#include "my_comm.h"
//#include "i2c_eeprom.h"

#include "ascii_helpers.h"

#include "startup.h"
#include "buildinfo.h"
#include "i2c_eeprom.h"
#include "crc32_helpers.h"

#ifdef USE_FRAMEBUFFER

#include "framebuffer.h"
#include "ssd1306.h"

extern struct_FrameBuffer fb1;
extern struct_Context *context1;
extern unsigned char FrameBuffer1[(128 / CHAR_BIT) * (64)];

#endif

volatile uint32_t RCC_CSR_copy;     /* copy of the RCC_CSR */
volatile uint32_t Saved_Magic_Bits; /* watchdog */

/* where to put diagnostic data */
volatile bool Transmit_non_RTOS;

#ifdef USE_FRAMEBUFFER
/**
 * @brief fb_fill_unfill
 */
static void fb_fill_unfill(void)
{
	uint32_t zzz;
	for (zzz = 0; zzz < sizeof(FrameBuffer1); zzz++)
		FrameBuffer1[zzz] = 0xFF;
	HAL_Delay(500);

	for (zzz = 0; zzz < sizeof(FrameBuffer1); zzz++)
		FrameBuffer1[zzz] = 0x00;
	HAL_Delay(500);
}
#endif

static void eeprom_crc32_check(void);

static const char *id = "Reflow controller ";
/**
 * @brief AppStartUp
 * @return
 */
ErrorStatus AppStartUp(void)
{
	ErrorStatus retVal = ERROR;

	InitComm();
	xfunc_out = myxfunc_out_no_RTOS; /* diagnostic print */
	/* set up periodic UART transmissions */
	Transmit_non_RTOS = true;

	log_set_mask_on(MSG_LEVEL_ALL);

	log_xputs(MSG_LEVEL_INFO, "\n\nStarting up...\n");
	log_xputs(MSG_LEVEL_INFO, id);
	log_xputs(MSG_LEVEL_INFO, buildNum_s);
	log_xputs(MSG_LEVEL_INFO, buildDateTime);

#ifdef USE_FRAMEBUFFER
	/* initialize display */
	InitSSD1303(&hspi2);
	FB_Init();
	fb_fill_unfill();
#endif

	/* put logo to the display */
	gotoXY(0, 0);
	zprint(" ************** \0", NORM);
	zprint(" *            * \0", NORM);
	zprint(" *   Reflow   * \0", NORM);
	zprint(" * controller * \0", NORM);
	zprint(" *            * \0", NORM);
	zprint(" ************** \0", NORM);
	zprint(buildNum_s, NORM);
	zprint("\n", NORM);

	/* check EEPROM CRC */

	//	Test_I2C_EEPROM(&at24c04);
	eeprom_crc32_check();

	/* check keys */


	/* check mains sync irq */

	retVal = SUCCESS;

fExit:
	return retVal;
}

static const char *read_err = "I2C EEPROM read error!";

/**
 * @brief crc32_over_eeprom
 * @return
 */
static uint32_t crc32_over_eeprom()
{
	uint32_t res_crc = 0U;
	uint8_t buf[16] = { 0U };
	size_t steps = at24c04.eeprom_size / 16U;

	for (size_t i = 0U; i < steps - 1U; i++) {
		for (size_t j = 0U; j < 16U; j++) {
			size_t eepr_addr = (i * 16U) + j;
			if (Read_Byte_I2C_EEPROM(&buf[j], eepr_addr,
						 &at24c04) != SUCCESS) {
				log_xputs(MSG_LEVEL_FATAL, read_err);
				HAL_Delay(500U);
				NVIC_SystemReset();
			}
			/* 16 bytes are read */
			res_crc = CRC32_helper(buf, 16U, res_crc);
		}
	}
	size_t last_chunk = 16U - sizeof(uint32_t);
	for (size_t j = 0U; j < last_chunk; j++) {
		size_t eepr_addr = ((steps - 2U) * 16U) + j;
		if (Read_Byte_I2C_EEPROM(&buf[j], eepr_addr, &at24c04) !=
		    SUCCESS) {
			log_xputs(MSG_LEVEL_FATAL, read_err);
			HAL_Delay(500U);
			NVIC_SystemReset();
		}
	}
	/* 12 bytes are read */
	res_crc = CRC32_helper(buf, last_chunk, res_crc);
	return res_crc;
}

/**
 * @brief eeprom_crc32_check
 */
static void eeprom_crc32_check(void)
{
	uint32_t read_crc = 0U;
	uint8_t *tmp = (uint8_t *)&read_crc;

	for (size_t i = 0U; i < sizeof(uint32_t); i++) {
		uint32_t addr_from = at24c04.eeprom_size - sizeof(uint32_t) + i;
		if (Read_Byte_I2C_EEPROM(&tmp[i], addr_from, &at24c04) !=
		    SUCCESS) {
			log_xputs(MSG_LEVEL_FATAL, read_err);
			HAL_Delay(500U);
			NVIC_SystemReset();
		}
	}

	if (read_crc != crc32_over_eeprom()) {
		log_xputs(MSG_LEVEL_FATAL, "I2C EEPROM CRC32 error!");

		uint32_t new_crc = 0U;
		/* CRC32 is at address EEPROM_SIZE - 4 */
		/* clear EEPROM */
		if (Clear_I2C_EEPROM(&at24c04) != SUCCESS) {
			log_xputs(MSG_LEVEL_FATAL, "I2C EEPROM clear error!");
			HAL_Delay(500U);
			NVIC_SystemReset();
		}
		/* calculate new CRC32 and save to EEPROM */
		new_crc = crc32_over_eeprom();

		/* save new CRC32 to the EEPROM */
		uint8_t *tmp = (uint8_t *)&new_crc;
		for (size_t i = 0U; i < sizeof(uint32_t); i++) {
			uint32_t addr_to =
				at24c04.eeprom_size - sizeof(uint32_t) + i;

			if (Write_Byte_I2C_EEPROM(&tmp[i], addr_to, &at24c04) !=
			    SUCCESS) {
				log_xputs(MSG_LEVEL_FATAL,
					  "I2C EEPROM write error!");
				HAL_Delay(500U);
				NVIC_SystemReset();
			}
		}
		log_xputs(MSG_LEVEL_SERIOUS, "New CRC32 saved. Rebooting..");
		HAL_Delay(500U);
		NVIC_SystemReset();
	} else {
		log_xputs(MSG_LEVEL_INFO, "EEPROM CRC32 is OK.");
	}
}
