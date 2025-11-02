/** @file profiles.h
 *  @brief thermal profiles definitions adn functions
 *
 *  @author Vasiliy Turchenko
 *  @bug
 *  @date 21-Jan-2020
 */

#ifndef PROFILES_H
#define PROFILES_H

#ifdef __cplusplus
extern "C" {
#endif

#include <limits.h>
#include <stddef.h>
#include <stdint.h>

/* a point of the profile */
struct profile_point;

typedef struct __attribute__((packed)) profile_point {
  const struct profile_point *prev; /* prev point in the profile */
  const struct profile_point *next; /* next point in the profile */
  uint16_t target_temperature;      /* we have to reach this value in Celsius */
  uint16_t min_duration;            /* minimum step duration time , seconds */
  uint16_t max_duration;            /* maximum step duration time , seconds */
  char name[9];                    /* phase name incl. trailing \0 */
} profile_point_t;

#define PROFILE_POINT_SIZE sizeof(profile_point_t)

/* a profile */
typedef struct __attribute__((packed)) reflow_profile {
  char name[17];                /* a profile name, incl. trailing \0 */
  const profile_point_t *first; /* first point of the profile */
} reflow_profile_t;

#define REFLOW_PROFILE_SIZE sizeof(reflow_profile_t)

/* exported links */
extern const reflow_profile_t *const prof1;
extern const reflow_profile_t *const prof2;


/**
 * @brief profiles_get_first_step
 * @param running_profile
 * @return
 */
static inline
const profile_point_t * profiles_get_first_step(const reflow_profile_t *running_profile)
{
	const profile_point_t * retVal = NULL;
	if (running_profile != NULL) {
		retVal = running_profile->first;
	}
	return retVal;
}

/**
 * @brief profiles_get_next_step
 * @param running_profile_point
 * @return
 */
static inline
const profile_point_t * profiles_get_next_step(const profile_point_t *running_profile_point)
{
	const profile_point_t * retVal = NULL;
	if (running_profile_point != NULL) {
		retVal = running_profile_point->next;
	}
	return retVal;
}

/**
 * @brief profiles_get_num_steps
 * @param running_profile
 * @return
 */
static inline
size_t profiles_get_num_steps(const reflow_profile_t *running_profile)
{
	size_t retVal = 0U;
	if (running_profile != NULL) {
		const profile_point_t *curr_step = running_profile->first;
		do {
			retVal++;
			curr_step = curr_step->next;
		} while (curr_step != NULL);
	}
	return retVal;
}

uint16_t profiles_get_prof_length(const reflow_profile_t * running_profile);

uint16_t profiles_get_temp_for_step_time(const profile_point_t *running_profile_point, uint16_t step_time);

void profiles_set_room_temp(uint16_t rt);

//void test_profiles_get_temp_for_time(void);

#ifdef __cplusplus
}
#endif

#endif
