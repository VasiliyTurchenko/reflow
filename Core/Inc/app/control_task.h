/** @file control_task.h
 *  @brief triacs main control task
 *
 *  @author Vasiliy Turchenko
 *  @bug
 *  @date 19-Jan-2020
 */

#ifndef CONTROL_TASK_H
#define CONTROL_TASK_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stddef.h>
#include <limits.h>

/* maximum time between two EXTIs*/
#define	MS_TO_WAIT_EXTI		(13U)

/* setpoint upper limit, %% */

//#define SETPOINT_UPPER_LIM_TOP	42U /* 740W*/

#define SETPOINT_UPPER_LIM_TOP	(50U) /* 740W*/
#define SETPOINT_UPPER_LIM_BTM	(32U) /* 671W*/

/* for manual temperature control */
#define MANUAL_LOW_TEMP		(20U)
#define MANUAL_HIGH_TEMP	(250U)

/* task notifications */
#define	HEATER_START		((uint32_t)0x5AU)
#define	EXTI_ARRIVED		((uint32_t)0xA5U)

/* control task results */
#define	CONTROL_TASK_RES_OK			((uint32_t)1U)
#define	CONTROL_TASK_RES_ERR			((uint32_t)2U)
#define	CONTROL_TASK_RES_ERR_BAD_SETTINGS	((uint32_t)3U)
#define	CONTROL_TASK_RES_ERR_NO_EXTI		((uint32_t)4U)
#define	CONTROL_TASK_RES_ERR_BAD_CMD		((uint32_t)5U)
#define	CONTROL_TASK_RES_ERR_BAD_NOTIF		((uint32_t)6U)


/*
 * settings for the half-period
*/
typedef	struct control_task_set {
	/* */
	uint32_t	mains_half_period;	///<  averaged half-period duration

	uint8_t		top_heater_setpoint;	///<  setpoint for top heater

	uint8_t		bottom_heater_setpoint;	///<  setpoint bottom top heater
} control_task_set_t;

extern control_task_set_t	control_task_setpoints;

void control_task_init(void);
void control_task_run(void);


#ifdef __cplusplus
}
#endif

#endif
