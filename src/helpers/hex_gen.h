/**
  ******************************************************************************
  * @file    hex_gen.h
  * @author  Vasily Turchenko <turchenkov@gmail.com>
  * @version V1.0.0
  * @date    1-Nov-2018
  * @date    30-Jan-2018
  * @brief   Functions for bin to hex conversion - Header file of hex_gen.c
  ******************************************************************************/

#ifndef HEX_GEN_H
#define HEX_GEN_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

char mybtol(char b);									  // 09-12-2017
void uint32_to_asciiz(uint32_t num, char *result_string); // 09-12-2017
void uint16_to_asciiz(uint16_t num, char *result_string); // 09-12-2017
void uint8_to_asciiz(uint8_t num, char *result_string);	  // 09-12-2017
void int32_to_asciiz(int32_t num, char *result_string);	  // 01-04-2018
void uint32_to_asciiz_hex(uint32_t num, void *result_string);

#ifdef __cplusplus
}
#endif

#endif

/***************************** END OF FILE *************************/
