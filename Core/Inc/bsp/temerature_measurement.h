/** @file temerature_measurement.h
 *  @brief
 *
 *  @author Vasiliy Turchenko
 *  @bug
 *  @date 20-Jan-2020
 */

#ifndef TEMERATURE_MEASUREMENT_H
#define TEMERATURE_MEASUREMENT_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stddef.h>
#include <limits.h>
#include <stdbool.h>

#ifdef STM32F103xB
#include "stm32f1xx.h"
#else
#error MCU NOT DEFINED
#endif

typedef struct averaged_data {
	/* averaged data for channel 0 */
	uint16_t	average_val0;

	/* averaged data for channel 1 */
	uint16_t	average_val1;
} averaged_data_t;

/* thermocouple calibration data */
/* temperature = ADC_DATA * (float)A + (float)B */
typedef	struct __attribute__((packed)) tc_cal_data {
	float	A;
	float	B;
	_Bool	valid;
} tc_cal_data_t;


averaged_data_t get_average_adc_meas(void);
averaged_data_t get_temperature(void);
ErrorStatus start_ADC(void);

ErrorStatus calibrate_TC1(void);
ErrorStatus calibrate_TC2(void);

void print_current_temperature(void);

#ifdef __cplusplus
}
#endif

#endif // TEMERATURE_MEASUREMENT_H
