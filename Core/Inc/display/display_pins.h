/** @file
 *  @brief
 *
 *  @author
 *  @bug
 *  @date
 */

#ifndef DISPLAY_PINS__H
#define DISPLAY_PINS__H

#include "main.h"

#define		CSL		do { SPI_CS_GPIO_Port->BSRR = (uint32_t)SPI_CS_Pin << 16U; \
	                        } while (false)

#define		CSH		do { SPI_CS_GPIO_Port->BSRR = SPI_CS_Pin; \
	                        } while (false)

#define		RESL		do { DISP_RESET_GPIO_Port->BSRR = (uint32_t)DISP_RESET_Pin << 16U; \
	                        } while (false)

#define		RESH		do { DISP_RESET_GPIO_Port->BSRR = DISP_RESET_Pin; \
	                        } while (false)

#define		DCC		do { D_C_GPIO_Port->BSRR = (uint32_t)D_C_Pin << 16U; \
	                        } while (false)

#define		DCD		do { D_C_GPIO_Port->BSRR = D_C_Pin; \
	                        } while (false)

#endif

