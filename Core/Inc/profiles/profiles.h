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
  const struct profile_point *next; /* next point in the profile */
  uint16_t target_temperature;      /* we have to reach this value in Celsius */
  uint16_t duration;                /* in the duration time , seconds */
  char name[16];                    /* phase name incl. trailing \0 */

  int8_t max_rate; /* maximal rise/fall rate in 0.1 C per sec
		      25 means heating 2.5C/sec
		      -54 mean cooling 5.4C/sec
		   */
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

void test_profiles_get_temp_for_time(void);

#ifdef __cplusplus
}
#endif

#endif
