/** @file regulator.c
 *  @brief temperature regulator
 *
 *  @author Vasiliy Turchenko
 *  @bug
 *  @date 08-Feb-2020
 */

#include "regulator.h"

typedef struct oven_profile {
    int32_t pid_p;          ///< proportional coeff
    int32_t pid_i;          ///< integral coeff
    int32_t pid_d;          ///< derivative coeff
    int32_t sustained_temp; ///< sustained temperature at 100% power
                            //	int32_t tau_s;		///< time constant
    int32_t max_throttle;   ///< max throttle value
    int32_t min_throttle;   ///< minimal throttle value

} oven_profile_t;

static oven_profile_t profile = {
    .pid_p          = 300,
    .pid_i          = 30,
    .pid_d          = 0,
    .sustained_temp = 400,
    //	.tau_s = 300,
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
        retVal = (int16_t)(((int32_t)target * 100) / (int32_t)profile.sustained_temp);
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
static inline int32_t CLAMP(int32_t a, int32_t low, int32_t high)
{
    if (a < low) {
        a = low;
    } else if (a > high) {
        a = high;
    } else {
        //
    }
    return a;
}

int32_t unclamped_result = 0;

/**
 * @brief pid
 * @param target
 * @param temp
 * @return
 */
int8_t pid(uint16_t target, uint16_t temp)
{
    static int32_t integral   = 0;
    static int32_t last_error = 0;
    int8_t         retVal     = 0;

    int32_t error = (int32_t)target - (int32_t)temp;
    if (target == 0U) {
        integral   = 0;
        last_error = error;
    } else {
        int32_t p_term = profile.pid_p * error;
        int32_t i_term = integral * profile.pid_i;
        int32_t d_term = (last_error - error) * profile.pid_d;

        int16_t new_integral = (int16_t)(integral + error);

        /* Clamp integral to a reasonable value */
        new_integral = (int16_t)CLAMP(new_integral, -4 * 100, 4 * 100);

        last_error = error;

        int32_t result = approx_throttle((int16_t)target) + p_term + i_term + d_term;

        /* Avoid integral buildup */
        if (((result > profile.max_throttle) && (new_integral < integral)) ||
            ((result < 0) && (new_integral > integral)) ||
            ((result < profile.max_throttle) && (result >= 0))) {
            integral = new_integral;
        }

        /* Clamp the output value */
        if (result < 0) {
            result = result / 10;
        }
        unclamped_result = result;
        retVal           = (int8_t)(CLAMP(result, profile.min_throttle, profile.max_throttle));
    }
    return retVal;
}
