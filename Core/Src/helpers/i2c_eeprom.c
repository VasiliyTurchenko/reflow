/** @file i2c_eeprom.c
 *  @brief I2C EEPROM routines
 *
 *  @author Vasiliy Turchenko
 *  @bug
 *  @date 14-Jan-2020
 */

#include <stdbool.h>
#include <stdlib.h>
#include "logging.h"
#include "i2c_eeprom.h"

/* STM32 Smart eeprom */
const eeprom_handle_t at24c04 = {
	.hi2c = &hi2c1,
	.eeprom_size = (uint32_t)EEPROM_SIZE,
	.page_size = (uint32_t)EEPROM_PAGE_SIZE,
	.eeprom_address = 0xA0U,
	.write_timeout_ms = 6U,
};

/**
 * @brief soft_reset
 * @param eeprom
 */
static void soft_reset(const eeprom_handle_t *eeprom)
{
}

/**
 * @brief Read_Byte_I2C_EEPROM
 * @param addr_to where to read byte
 * @param eeaddr byte address in the eeprom
 * @param eeprom pointer to the eeprom handler
 * @return
 */
ErrorStatus Read_Byte_I2C_EEPROM(uint8_t *addr_to, size_t eeaddr,
				 const eeprom_handle_t *eeprom)
{
	ErrorStatus retVal = ERROR;
	if ((eeprom == NULL) || (addr_to == NULL)) {
		goto fExit;
	}
	if (eeaddr >= eeprom->eeprom_size) {
		goto fExit;
	}
	soft_reset(eeprom);
	uint8_t eeaddress = (uint8_t)eeaddr;
	uint16_t dev_addr = (uint16_t)eeprom->eeprom_address;

	if (eeaddr > 0xFFU) {
		dev_addr+=2U;
	}

	if (HAL_I2C_Master_Transmit(eeprom->hi2c, dev_addr, &eeaddress, 1U,
				    5U) != HAL_OK) {
		goto fExit;
	}

	if (HAL_I2C_Master_Receive(eeprom->hi2c, dev_addr, addr_to, 1U, 5U) ==
	    HAL_OK) {
		retVal = SUCCESS;
	}

fExit:
	return retVal;
}

ErrorStatus Write_Byte_I2C_EEPROM(const uint8_t *addr_from, size_t eeaddr,
				  const eeprom_handle_t *eeprom)
{
	ErrorStatus retVal = ERROR;
	if ((eeprom == NULL) || (addr_from == NULL)) {
		goto fExit;
	}
	if (eeaddr >= eeprom->eeprom_size) {
		goto fExit;
	}
	soft_reset(eeprom);
	uint8_t eeaddress = (uint8_t)eeaddr;
	uint16_t dev_addr = (uint16_t)eeprom->eeprom_address;

	if (eeaddr > 0xFFU) {
		dev_addr+=2U;
	}
	uint8_t	buf[2];
	buf[0] = eeaddress;
	buf[1] = *addr_from;

	if (HAL_I2C_IsDeviceReady(eeprom->hi2c, dev_addr, 3U, eeprom->write_timeout_ms) != HAL_OK) {
		goto fExit;
	}

	if (HAL_I2C_Master_Transmit(eeprom->hi2c, dev_addr, (uint8_t*)&buf, 2U,
				    eeprom->write_timeout_ms) == HAL_OK) {
		retVal = SUCCESS;
		HAL_Delay(eeprom->write_timeout_ms);
	}

fExit:
	return retVal;
}

/**
 * @brief Clear_I2C_EEPROM
 * @param eeprom
 * @return
 */
ErrorStatus Clear_I2C_EEPROM(const eeprom_handle_t *eeprom)
{
	ErrorStatus retVal = ERROR;
	if (eeprom == NULL) {
		goto fExit;
	}
	const uint8_t	pad = 0xAAU;
	for (size_t i = 0U; i < eeprom->eeprom_size; i++) {
		if (Write_Byte_I2C_EEPROM(&pad, i, eeprom) != SUCCESS) {
			goto fExit;
		}
	}
	retVal = SUCCESS;
fExit:
	return retVal;
}


/**
 * @brief Test_I2C_EEPROM
 * @param eeprom
 */
__attribute((noreturn)) void Test_I2C_EEPROM(const eeprom_handle_t *eeprom)
{
	while (true) {

		srand(HAL_GetTick());

		static uint8_t		buffer[512];
		for (size_t i = 0U; i < 512U; i++) {
			buffer[i] = (uint8_t)(rand() % 512);
		}

		for (size_t i = 0U; i < 512U; i++) {

			if (Write_Byte_I2C_EEPROM(&buffer[i], i, eeprom) != SUCCESS) {
				log_xprintf(
					MSG_LEVEL_SERIOUS,
					"I2C EEPROM write error at address %d",
					i);
			} else {
				log_xprintf(
					MSG_LEVEL_INFO,
					"I2C EEPROM write %d to address %d OK",
					buffer[i], i);
			}
		HAL_Delay(300U);
		}

		static uint8_t		read_buffer[512];
		for (size_t i = 0U; i < 512U; i++) {

			if (Read_Byte_I2C_EEPROM(&read_buffer[i], i, eeprom) !=
			    SUCCESS) {
				log_xprintf(
					MSG_LEVEL_SERIOUS,
					"I2C EEPROM read error at address %d",
					i);
			} else {
				log_xprintf(
					MSG_LEVEL_INFO,
					"I2C EEPROM read at address %d = %d", i,
					read_buffer[i]);
			}

			if (buffer[i] != read_buffer[i]) {
				log_xprintf(
					MSG_LEVEL_SERIOUS,
					"buffer[i] != read_buffer[i] at address %d",
					i);
			}

		HAL_Delay(300U);
		}
	}

}

__attribute((noreturn)) void I2C_EEPROM_Write_Error_Handler(void);

__attribute((noreturn)) void I2C_EEPROM_Read_Error_Handler(void);

