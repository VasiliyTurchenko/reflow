/** @file i2c_eeprom.h
 *  @brief I@C EEPROM routines header file
 *
 *  @author Vasiliy Turchenko
 *  @bug
 *  @date 14-Jan-2020
 */

#ifndef I2C_EEPROM_H
#define I2C_EEPROM_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stddef.h>
#include <limits.h>

#include "main.h"
#include "i2c.h"


#ifdef STM32F303xC
    #include "stm32f3xx_hal.h"
#elif STM32F103xB
    #include "stm32f1xx_hal.h"
#endif


#ifdef AT24C04
#define	EEPROM_SIZE		512U
#define EEPROM_PAGE_SIZE	16U
#else
#error "EEPROM type must be defined!"
#endif

/* EEPROM descriptor */
typedef struct eeprom_handle {
    I2C_HandleTypeDef	*hi2c;
    size_t			eeprom_size; /* full eeprom size in bytes */
    size_t			page_size;
    size_t			write_timeout_ms;
    uint8_t			eeprom_address;
} eeprom_handle_t;

/* STM32 Smart EEPROM chip */
extern const eeprom_handle_t at24c04;
extern const char *read_err;

/* */
ErrorStatus Clear_I2C_EEPROM(const eeprom_handle_t *eeprom);

ErrorStatus Read_Byte_I2C_EEPROM(uint8_t * addr_to, \
                 size_t eeaddr, \
                 const eeprom_handle_t * eeprom);

ErrorStatus Write_Byte_I2C_EEPROM(const uint8_t * addr_from, \
                  size_t eeaddr, \
                  const eeprom_handle_t * eeprom);

ErrorStatus Read_Array_I2C_EEPROM(uint8_t *addr_to,
                   size_t num_bytes,
                   size_t eeaddr,
                   const eeprom_handle_t *eeprom);

ErrorStatus Write_Array_I2C_EEPROM(const uint8_t *addr_from,
                   size_t num_bytes,
                   size_t eeaddr,
                   const eeprom_handle_t *eeprom);

ErrorStatus Read_Page_I2C_EEPROM(uint8_t * addr_to,
                  size_t pagenum, \
                  const eeprom_handle_t * eeprom);

ErrorStatus Write_Page_I2C_EEPROM(uint8_t * addr_from, \
                   size_t pagenum, \
                   const eeprom_handle_t * eeprom);

ErrorStatus Quick_I2C_EEPROM_Test(const eeprom_handle_t * eeprom);

void Test_I2C_EEPROM(const eeprom_handle_t * eeprom);

__attribute ((noreturn)) void I2C_EEPROM_Write_Error_Handler(void);

__attribute ((noreturn)) void I2C_EEPROM_Read_Error_Handler(void);

uint32_t crc32_over_eeprom(void);

#ifdef __cplusplus
}
#endif

#endif // I2C_EEPROM_H
