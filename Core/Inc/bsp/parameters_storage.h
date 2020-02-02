/** @file parameters_storage.h
 *  @brief EEPROM parameters layout and store/read functions
 *
 *  @author Vasiliy Turchenko
 *  @bug
 *  @date 21-Jan-2020
 */

#ifndef PARAMETERS_STORAGE_H
#define PARAMETERS_STORAGE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stddef.h>
#include <limits.h>

#include "CAssert.h"

#include "i2c_eeprom.h"

#include "temerature_measurement.h"
#include "door_servo.h"
#include "heat_cool.h"

/* predefined config tokens */
typedef enum {
	CFG_TYPE_TC_CAL1,  /* termocouple 1 caibration data*/
	CFG_TYPE_TC_CAL2,  /* termocouple 1 caibration data*/

	CFG_TYPE_DOOR_PWM,

	CFG_TYPE_HEAT_CAL,
	CFG_TYPE_COOL_CAL,

	CFG_TYPE_NONE,
} CFG_TYPE;

typedef struct __attribute__((packed)) EEPROM_pool {

	tc_cal_data_t		tc1_cal_data;	/* Thermocouples calibration data */
	tc_cal_data_t		tc2_cal_data;

	door_pwm_cal_data_t	door_pwm_cal_data;

	/* oven heating / cooling characteristics */
	heat_cool_cal_data_t	heating_cal_data;
	heat_cool_cal_data_t	cooling_cal_data;

} EEPROM_pool_t;

/* static check size of the EEPROM_pool_t vs EEPROM_SIZE */
CASSERT( EEPROM_SIZE >= (sizeof(EEPROM_pool_t) + sizeof (uint32_t)) , parameters_storage_h )

ErrorStatus load_cfg_pool(void);
ErrorStatus save_cfg_pool(void);
void *get_cfg(CFG_TYPE requested_cfg);

#ifdef __cplusplus
}
#endif


#endif // PARAMETERS_STORAGE_H
