/** @file regulator.c
 *  @brief temperature regulator
 *
 *  @author Vasiliy Turchenko
 *  @bug
 *  @date 08-Feb-2020
 */

#include "regulator.h"

static const uint8_t s10[305] = {

	/*0...59s*/
	100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100,
	100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100,
	100, 100,
	/*30*/
	100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 80, 80, 80, 80, 80,
	50, 50, 50, 50, 50, 20, 20, 20, 20, 20, 00, 00, 00, 00, 00,

	/*60...119s*/
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0,
	/*90*/
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0,

	/*140...179s*/
	40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40,
	40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40,
	/*150*/
	40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 100, 100, 100, 100, 100, 100,
	100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100,

	/* 180...239s*/
	100, 100, 100, 100, 100, 00, 00, 00, 00, 00, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40,
	/*210 */
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 00, 00, 00, 00,
	00, 00, 00, 00, 00, 00, 00,

	/*240...305s*/
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

/**
 * @brief get_heater_throttle
 * @param t_actual
 * @param t_set
 * @param time
 * @return
 */
uint8_t get_heater_throttle(uint16_t t_actual, uint16_t time)
{
	uint8_t retVal = 0U;
	static uint16_t last_time = 0U;
	if (t_actual < 120U) {
		retVal = 100U;
		last_time = time;
		goto fExit;
	}
	uint16_t elapsed = time - last_time;

	if ((elapsed > 30U) && (elapsed < 45U)) {
		retVal = 40U;
		goto fExit;
	}

	if (elapsed < 105U) {
		retVal = 0U;
		goto fExit;
	}

	if (elapsed < 110U) {
		retVal = 100U;
		goto fExit;
	}
	if (elapsed < 180U) {
		retVal = 100U;
		goto fExit;
	}
	if (elapsed < 190U) {
		retVal = 20U;
		goto fExit;
	}

fExit:
	return retVal;
}

typedef struct oven_profile {
	int32_t pid_p;		///< proportional coeff
	int32_t pid_i;		///< integral coeff
	int32_t pid_d;		///< derivative coeff
	int32_t sustained_temp; ///< sustained temperature at 100% power
	int32_t tau_s;		///< time constant
	int32_t max_throttle;   ///< max throttle value
	int32_t min_throttle;	///< minimal throttle value

} oven_profile_t;

static oven_profile_t profile = {
	.pid_p = 300,
	.pid_i = 30,
	.pid_d = 0,
	.sustained_temp = 400,
	.tau_s = 300,
	.max_throttle = 100,
	.min_throttle = -100,
};

/**
 * @brief approx_throttle
 * @param target
 * @return
 */
static int16_t approx_throttle(int16_t target)
{
	int16_t retVal = 100;
	if (target < profile.sustained_temp) {
		retVal = (target * 100) / profile.sustained_temp;
	}
	return retVal;
}

/**
 * @brief CLAMP
 * @param a
 * @param low
 * @param high
 * @return
 */
static /*inline*/ int32_t CLAMP(int32_t a, int32_t low, int32_t high)
{
	if (a < low) {
		a = low;
	} else if (a > high) {
		a = high;
	}
	return a;
}

int32_t unclamped_result = 0U;

/**
 * @brief pid
 * @param target
 * @param temp
 * @return
 */
int8_t pid(uint16_t target, uint16_t temp)
{
	static int32_t integral = 0;
	static int32_t last_error = 0;

	int32_t error = (int32_t)target - (int32_t)temp;
	if (target == 0) {
		integral = 0;
		last_error = error;
		return 0;
	} else {
		int32_t p_term = profile.pid_p * error;
		int32_t i_term = integral * profile.pid_i;
		int32_t d_term = (last_error - error) * profile.pid_d;

		int16_t new_integral = integral + error;

		/* Clamp integral to a reasonable value */
		new_integral = CLAMP(new_integral, -4 * 100, 4 * 100);

		last_error = error;

		int32_t result = approx_throttle((int16_t)target) + p_term +
				 i_term + d_term;

		/* Avoid integral buildup */
		if (((result > profile.max_throttle) &&
		     (new_integral < integral)) ||
		    ((result < 0) && (new_integral > integral)) ||
		    ((result < profile.max_throttle) && (result >= 0))) {
			integral = new_integral;
		}

		/* Clamp the output value */
		if (result < 0) {
			result = result / 10;
		}
		unclamped_result = result;
		return (int8_t)(CLAMP(result, profile.min_throttle, profile.max_throttle));
	}
}
