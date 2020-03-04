/** @file profiles.c
 *  @brief thermal profiles functions
 *
 *  @author Vasiliy Turchenko
 *  @bug
 *  @date 21-Jan-2020
 */

#include <math.h>

#include "profiles.h"
#include "CAssert.h"

#include "logging.h"

/* **************** first profile **********************************/
/* https:/ /www.compuphase.com/electronics/reflowsolderprofiles.htm */
/* Lead (Sn63 Pb37) */
static const profile_point_t Sn63Pb37_pre_heat;
static const profile_point_t Sn63Pb37_soak;
static const profile_point_t Sn63Pb37_ramp_up;
static const profile_point_t Sn63Pb37_reflow;
static const profile_point_t Sn63Pb37_cool_down;

static const profile_point_t Sn63Pb37_pre_heat = {
	.prev = NULL,
	.next = &Sn63Pb37_soak,
	.target_temperature = 150U,
	.min_duration = 60U,
	.max_duration = 90U,
	.name = { "Pre-heat" },
};

static const profile_point_t Sn63Pb37_soak = {
	.prev = &Sn63Pb37_pre_heat,
	.next = &Sn63Pb37_ramp_up,
	.target_temperature = 165U,
	.min_duration = 120U,
	.max_duration = 130U,
	.name = { "Soak" },
};

static const profile_point_t Sn63Pb37_ramp_up = {
	.prev = &Sn63Pb37_soak,
	.next = &Sn63Pb37_reflow,
	.target_temperature = 230U,
	.min_duration = 45U,
	.max_duration = 55U,
	.name = { "Ramp-Up" },
};

static const profile_point_t Sn63Pb37_reflow = {
	.prev = &Sn63Pb37_ramp_up,
	.next = &Sn63Pb37_cool_down,
	.target_temperature = 230U,
	.min_duration = 20U,
	.max_duration = 21U,
	.name = { "Reflow" },
};

static const profile_point_t Sn63Pb37_cool_down = {
	.prev = &Sn63Pb37_reflow,
	.next = NULL,
	.target_temperature = 30U,
	.min_duration = 60U,
	.max_duration = 100U,
	.name = { "CoolDown" },

};

static const reflow_profile_t Sn63Pb37 = {
	.name = { "Sn63Pb37        " },
	.first = &Sn63Pb37_pre_heat,
};

/***************** second profile **********************************/
/* https:/ /www.compuphase.com/electronics/reflowsolderprofiles.htm */
/* Lead-free (SAC305) */
static const profile_point_t SAC305_pre_heat;
static const profile_point_t SAC305_soak;
static const profile_point_t SAC305_ramp_up;
static const profile_point_t SAC305_reflow;
static const profile_point_t SAC305_cool_down;

static const profile_point_t SAC305_pre_heat = {
	.prev = NULL,
	.next = &SAC305_soak,
	.target_temperature = 150U,
	.min_duration = 60U,
	.max_duration = 90U,
	.name = { "Pre-heat" },
};

static const profile_point_t SAC305_soak = {
	.prev = &SAC305_pre_heat,
	.next = &SAC305_ramp_up,
	.target_temperature = 180U,
	.min_duration = 120U,
	.max_duration = 130U,
	.name = { "Soak" },
};

static const profile_point_t SAC305_ramp_up = {
	.prev = &SAC305_pre_heat,
	.next = &SAC305_reflow,
	.target_temperature = 245U,
	.min_duration = 45U,
	.max_duration = 45U,
	.name = { "Ramp-Up" },
};

static const profile_point_t SAC305_reflow = {
	.prev = &SAC305_ramp_up,
	.next = &SAC305_cool_down,
	.target_temperature = 245U,
	.min_duration = 15U,
	.max_duration = 16U,
	.name = { "Reflow" },
};

static const profile_point_t SAC305_cool_down = {
	.prev = &SAC305_reflow,
	.next = NULL,
	.target_temperature = 30U,
	.min_duration = 60U,
	.max_duration = 100U,
	.name = { "CoolDown" },
};

static const reflow_profile_t SAC305 = {
	.name = { "SAC305          " },
	.first = &SAC305_pre_heat,
};

const reflow_profile_t *const prof1 = &Sn63Pb37;
const reflow_profile_t *const prof2 = &SAC305;

/**
 * @brief profiles_get_prof_length returns the total length of the profile in seconds
 * @param running_profile
 * @return
 */
uint16_t profiles_get_prof_length(const reflow_profile_t *running_profile)
{
	uint16_t retVal = 0U;

	if (running_profile == NULL) {
		goto fExit;
	}
	/* find the current step */
	const profile_point_t *curr_step = running_profile->first;

	do {
		retVal += curr_step->min_duration;
		curr_step = curr_step->next;

	} while (curr_step != NULL);

fExit:
	return retVal;
}

/* room temperature must be initialized to the actual value before reflow starts */
static uint16_t room_temp = 0U;

void profiles_set_room_temp(uint16_t rt)
{
	room_temp = rt;
}

/**
 * @brief profiles_get_temp_for_step_time returns target temperature for the step time
 * @note  temperature is calculated with respect to the fastest permissible heating rate!
 * @param running_profile_point
 * @param step_time
 * @return
 */
uint16_t profiles_get_temp_for_step_time(const profile_point_t *running_profile_point, uint16_t step_time)
{
	int32_t retVal = 0;
	if ((running_profile_point != NULL) && (step_time < running_profile_point->min_duration)) {
		int32_t step_start_temp = (int32_t)room_temp; ////< room temp for the first step
		if (running_profile_point->prev != NULL) {
			step_start_temp = (int32_t)running_profile_point->prev->target_temperature;
		}
		int32_t step_end_temp = (int32_t)running_profile_point->target_temperature;

		float rate = (float)step_end_temp - (float)step_start_temp;
		rate = rate / (float)running_profile_point->min_duration;

		float fretVal = (float)step_time * rate;
		fretVal += (float)step_start_temp;

		retVal = (int32_t)fretVal;
		retVal = (retVal < 0) ? 0 : retVal;		///< clamp
		retVal = (retVal > 500) ? 0 : retVal;		///< clamp again
	}
	return (uint16_t)retVal;
}


#if(0)

/**
 * @brief get_current_step
 * @param running_profile
 * @param time elapsed process time
 * @return
 */
static inline const profile_point_t *
get_current_step(const reflow_profile_t *running_profile, uint16_t time)
{
	const profile_point_t *curr_step = running_profile->first;
	uint16_t steps_end_time =
		curr_step->min_duration; ///< accumulated steps time

	while ((time > steps_end_time) && (curr_step != NULL)) {
		curr_step = curr_step->next;
		steps_end_time += curr_step->min_duration;
	}
	return curr_step;
}

/**
 * @brief get_current_step_end_time
 * @param running_profile
 * @param time elapsed process time
 * @return
 */
static inline uint16_t
get_current_step_end_time(const reflow_profile_t *running_profile,
			  uint16_t time)
{
	const profile_point_t *curr_step = running_profile->first;
	uint16_t steps_end_time =
		curr_step->min_duration; ///< accumulated steps time

	while ((time > steps_end_time) && (curr_step != NULL)) {
		curr_step = curr_step->next;
		steps_end_time += curr_step->min_duration;
	}
	return steps_end_time;
}

/**
 * @brief profiles_get_remaining_step_time returns remaining time for the step calculated from process time
 * @param[in] running_profile
 * @param[in] time elapsed process time
 * @return
 */
uint16_t
profiles_get_remaining_step_time(const reflow_profile_t *running_profile,
				 uint16_t time)
{
	uint16_t retVal = 0U;

	if (running_profile == NULL) {
		goto fExit;
	}
	retVal = get_current_step_end_time(running_profile, time) - time;

fExit:
	return retVal;
}

/**
 * @brief profiles_get_step_name
 * @param running_profile
 * @param time elapsed process time
 * @return
 */
const char * profiles_get_step_name(const reflow_profile_t *running_profile,
				 uint16_t time)
{
	const char * retVal = "Bad step";
	if (running_profile == NULL) {
		goto fExit;
	}
	return get_current_step(running_profile, time)->name;
fExit:
	return retVal;
}


/**
 * @brief profiles_get_temp_for_time
 * @param running_profile
 * @param time time from start of the overall process!
 * @return target temperature for the profile at the ime
 */
uint8_t profiles_get_temp_for_time(const reflow_profile_t *running_profile,
				   uint16_t time)
{
	uint8_t retVal = 0U;

	if (running_profile == NULL) {
		goto fExit;
	}
	/* find the current step */
	const profile_point_t *curr_step = running_profile->first;

	/* time at the end of current step */
	uint16_t elapsed_sec = 0;
	uint16_t elapsed_at_step = 0U;
	/* assume initial room temp for step 0 */
	uint16_t start_step_temp = 20U;

	do {
		if ((time >= elapsed_sec) &&
		    (time < (elapsed_sec + curr_step->min_duration))) {
			/* this is the step */
			elapsed_at_step = time - elapsed_sec;
			break;
		} else {
			elapsed_sec += curr_step->min_duration;
			start_step_temp = curr_step->target_temperature;
			curr_step = curr_step->next;
		}
	} while (curr_step != NULL);

	/* elapsed_at_step is the time from the begin of the step */
	/* curr_step is the pointer at the current step */
	float rate = (float)(curr_step->target_temperature - start_step_temp);
	rate /= (float)curr_step->min_duration;

	retVal = (uint8_t)(roundf((float)elapsed_at_step * rate) +
			   start_step_temp);
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
		uint8_t t;
		t = profiles_get_temp_for_time(prof1, i);
		log_xprintf(MSG_LEVEL_INFO, "%d, %d", i, t);
		vTaskDelay(pdMS_TO_TICKS(100));
	}

	log_xputs(MSG_LEVEL_INFO, "\n profile 2 test started\n\n");
	for (uint16_t i = 0U; i < 400U; i++) {
		uint8_t t;
		t = profiles_get_temp_for_time(prof2, i);
		log_xprintf(MSG_LEVEL_INFO, "%d, %d", i, t);
		vTaskDelay(pdMS_TO_TICKS(100));
	}
}

#endif
