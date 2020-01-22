/** @file profiles.c
 *  @brief thermal profiles functions
 *
 *  @author Vasiliy Turchenko
 *  @bug
 *  @date 21-Jan-2020
 */

#include <math.h>

#include "profiles.h"

#include "logging.h"

/* **************** first profile **********************************/
/* https://www.compuphase.com/electronics/reflowsolderprofiles.htm */
/* Lead (Sn63 Pb37) */
static const profile_point_t Sn63Pb37_pre_heat;
static const profile_point_t Sn63Pb37_soak;
static const profile_point_t Sn63Pb37_ramp_up;
static const profile_point_t Sn63Pb37_reflow;
static const profile_point_t Sn63Pb37_cool_down;

static const profile_point_t Sn63Pb37_pre_heat = {
	.next			= &Sn63Pb37_soak,
	.target_temperature	= 150U,
	.duration		= 60U,
	.name			= {"Pre-heat\0"},
	.max_rate		= 25	/* < 2.5 C/ sec */
};

static const profile_point_t Sn63Pb37_soak = {
	.next			= &Sn63Pb37_ramp_up,
	.target_temperature	= 165U,
	.duration		= 120U,
	.name			= {"Soak\0"},
	.max_rate		= 2	/* < 0.2 C/ sec */
};

static const profile_point_t Sn63Pb37_ramp_up = {
	.next			= &Sn63Pb37_reflow,
	.target_temperature	= 230U,
	.duration		= 45U,
	.name			= {"Ramp-Up\0"},
	.max_rate		= 18	/* < 1.8 C/ sec */
};

static const profile_point_t Sn63Pb37_reflow = {
	.next			= &Sn63Pb37_cool_down,
	.target_temperature	= 230U,
	.duration		= 20U,
	.name			= {"Reflow\0"},
	.max_rate		= 1	/* < 0.1 C/ sec */
};

static const profile_point_t Sn63Pb37_cool_down =  {
	.next			= NULL,
	.target_temperature	= 30U,
	.duration		= 60U,
	.name			= {"Cool Down\0"},
	.max_rate		= -40	/* < -4.0 C/ sec */

};

static const reflow_profile_t Sn63Pb37 = {
	.name			= {"Sn63Pb37        \0"},
	.first			= &Sn63Pb37_pre_heat,
};

/***************** second profile **********************************/
/* https://www.compuphase.com/electronics/reflowsolderprofiles.htm */
/* Lead-free (SAC305) */
static const profile_point_t SAC305_pre_heat;
static const profile_point_t SAC305_soak;
static const profile_point_t SAC305_ramp_up;
static const profile_point_t SAC305_reflow;
static const profile_point_t SAC305_cool_down;

static const profile_point_t SAC305_pre_heat = {
	.next			= &SAC305_soak,
	.target_temperature	= 150U,
	.duration		= 60U,
	.name			= {"Pre-heat\0"},
	.max_rate		= 25	/* < 2.5 C/ sec */
};

static const profile_point_t SAC305_soak = {
	.next			= &SAC305_ramp_up,
	.target_temperature	= 180U,
	.duration		= 120U,
	.name			= {"Soak\0"},
	.max_rate		= 3	/* < 0.3 C/ sec */
};

static const profile_point_t SAC305_ramp_up = {
	.next			= &SAC305_reflow,
	.target_temperature	= 245U,
	.duration		= 45U,
	.name			= {"Ramp-Up\0"},
	.max_rate		= 15	/* < 1.5 C/ sec */
};

static const profile_point_t SAC305_reflow = {
	.next			= &SAC305_cool_down,
	.target_temperature	= 245U,
	.duration		= 15U,
	.name			= {"Reflow\0"},
	.max_rate		= 1	/* < 0.1 C/ sec */
};

static const profile_point_t SAC305_cool_down =  {
	.next			= NULL,
	.target_temperature	= 30U,
	.duration		= 60U,
	.name			= {"Cool Down\0"},
	.max_rate		= -40	/* < -4.0 C/ sec */
};

static const reflow_profile_t SAC305 = {
	.name			= {"SAC305          \0"},
	.first			= &SAC305_pre_heat,
};

const reflow_profile_t	* const prof1 = &Sn63Pb37;
const reflow_profile_t	*const prof2 = &SAC305;


/**
 * @brief profiles_get_temp_for_time
 * @param running_profile
 * @param time time from start of the overall process!
 * @return target temperature for the profile at the ime
 */
uint8_t profiles_get_temp_for_time(const reflow_profile_t * running_profile, uint16_t time)
{
	uint8_t retVal = 0U;

	if (running_profile == NULL) {
		goto fExit;
	}
	/* find the current step */
	const profile_point_t	* curr_step = running_profile->first;

	/* time at the end of current step */
	uint16_t	elapsed_sec = 0;
	uint16_t	elapsed_at_step = 0U;
	/* assume initial room temp for step 0 */
	uint16_t	start_step_temp = 20U;

	do {
		if ((time >= elapsed_sec) && \
		    (time < (elapsed_sec + curr_step->duration))) {
		    /* this is the step */
			elapsed_at_step = time - elapsed_sec;
			break;
		} else {
			elapsed_sec += curr_step->duration;
			start_step_temp = curr_step->target_temperature;
			curr_step = curr_step->next;
		}
	} while (curr_step != NULL);

	/* elapsed_at_step is the time from the begin of the step */
	/* curr_step is the pointer at the current step */
	float rate = (float)(curr_step->target_temperature - start_step_temp);
	rate /= (float)curr_step->duration;

	retVal =(uint8_t)(roundf((float)elapsed_at_step * rate) + start_step_temp) ;
fExit:
	return retVal;
}

/**
 * @brief test_profiles_get_temp_for_time
 */
void test_profiles_get_temp_for_time(void)
{
	log_xputs(MSG_LEVEL_INFO, "\n profile 1 test started\n\n");
	for (uint16_t i = 0U; i < 400U; i++) {
		uint8_t	t;
		t = profiles_get_temp_for_time(prof1, i);
		log_xprintf(MSG_LEVEL_INFO, "%d, %d", i, t);
		vTaskDelay(pdMS_TO_TICKS(100));
	}

	log_xputs(MSG_LEVEL_INFO, "\n profile 2 test started\n\n");
	for (uint16_t i = 0U; i < 400U; i++) {
		uint8_t	t;
		t = profiles_get_temp_for_time(prof2, i);
		log_xprintf(MSG_LEVEL_INFO, "%d, %d", i, t);
		vTaskDelay(pdMS_TO_TICKS(100));
	}

}
