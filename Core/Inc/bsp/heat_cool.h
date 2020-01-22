/** @file
 *  @brief
 *
 *  @author Vasiliy Turchenko
 *  @bug
 *  @date
 */

#ifndef HEAT_COOL_H
#define HEAT_COOL_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stddef.h>
#include <limits.h>
#include <stdbool.h>

#define	FORCE_STEPS	10

/*
 * temperature rise rates vs %% of power
 * with step 10%
 *
 *  or
 *
 * temperature fall rates vs %% of door opening
 * with step 10%
*/
typedef struct __attribute__((packed)) heat_cool_cal_data {

	uint8_t		rate[FORCE_STEPS]; /* 1 unit = 0.1 deg C */
	_Bool		valid;

} heat_cool_cal_data_t;

#ifdef __cplusplus
}
#endif


#endif // HEAT_COOL_H
