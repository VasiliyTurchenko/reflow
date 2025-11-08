/** @file logging.c
 *  @brief central logging system
 *
 *  @author Vasiliy Turchenko
 *  @bug
 *  @date 05-Oct-2019
 *  @note MISRA passed
 */

#include "logging.h"
#include "platform_time_util.h"

#ifdef LOG_WITH_TIME
#include "hex_gen.h"
#endif

static uint32_t current_mask = 0U;
const char *	delim		 = " : ";
extern _Bool	nice_mode;
_Bool	nice_mode	 = false;

void log_nice_mode_on(void)
{
	nice_mode = true;
}

void log_nice_mode_off(void)
{
	nice_mode = false;
}

/**
 * @brief filterIsPassed
 * @param lvl
 * @return
 */
_Bool filterIsPassed(MSG_LEVEL lvl)
{
	uint32_t reqLevel = (uint32_t)lvl & (0x7FFFFFFFU);
	return ((current_mask & reqLevel) != 0U);
}

/**
 * @brief log_set_mask_on
 * @param lvl
 */
void log_set_mask_on(MSG_LEVEL lvl)
{
	uint32_t reqLevel = (uint32_t)lvl & (0x7FFFFFFFU);
	current_mask |= reqLevel;
}

/**
 * @brief log_set_mask_off
 * @param lvl
 */
void log_set_mask_off(MSG_LEVEL lvl)
{
	uint32_t reqLevel = (uint32_t)lvl & (0x7FFFFFFFU);
	current_mask &= ~reqLevel;
}

#ifndef NO_RTOS
/**
 * @brief log_current_task_name
 */
void log_current_task_name(void)
{
	if (xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED) {
		xputs(pcTaskGetName(xTaskGetCurrentTaskHandle()));
		xputs(delim);
	}
}
#endif

/**
 * @brief log_timestamp
 */
#ifdef LOG_WITH_TIME
void log_timestamp(void)
{
	char	 buf[] = "[xxxxxxx.xxx] "; /* added for dec point */
	uint32_t ts	   = time_now_ms();
	uint32_to_asciiz(ts, &buf[1]);
	buf[11]	 = buf[10];
	buf[10]	 = buf[9];
	buf[9]	 = buf[8];
	buf[8]	 = '.';
	size_t i = 1U;
	while (buf[i] == '0') {
		buf[i] = ' ';
		i++;
	}
	xputs(buf);
}
#endif

void sel_color(MSG_LEVEL lvl)
{
	if (nice_mode) {
		switch (lvl) {
		case (MSG_LEVEL_FATAL): {
			CRT_textColor(cBOLDRED);
			break;
		}
		case (MSG_LEVEL_SERIOUS): {
			CRT_textColor(cBOLDYELLOW);
			break;
		}
		case (MSG_LEVEL_PROC_ERR): {
			CRT_textColor(cBOLDMAGENTA);
			break;
		}
		case (MSG_LEVEL_INFO): {
			CRT_textColor(cBOLDBLUE);
			break;
		}

		case (MSG_LEVEL_TASK_INIT):
		case (MSG_LEVEL_EXT_INF): {
			CRT_textColor(cBOLDGREEN);
			break;
		}
		case (MSG_LEVEL_ALL):
		default:
			/* stub */
			break;
		}
	}
}

#ifdef LOG_WITH_LEVEL
void log_level(MSG_LEVEL lev)
{
	const char * s;
	switch (lev) {
	case MSG_LEVEL_FATAL: {
		s = "{FAT} ";
		break;
	}
	case MSG_LEVEL_SERIOUS: {
		s = "{SER} ";
		break;
	}
	case MSG_LEVEL_PROC_ERR: {
		s = "{P_E} ";
		break;
	}

	case MSG_LEVEL_INFO : {
		s = "{INF} ";
		break;
	}

	case MSG_LEVEL_EXT_INF : {
		s = "{EXT} ";
		break;
	}
	case MSG_LEVEL_TASK_INIT :{
		s = "{INI} ";
		break;
	}
	default:{
		s = "{???} ";
		break;
	}
	}
	xputs(s);
}
#endif

/**
 * @brief log_xputs
 * @param lvl
 * @param str
 */
void log_xputs(MSG_LEVEL lvl, const char *str)
{
	if (filterIsPassed(lvl)) {
		/* grab the mutex */
		TAKE_MUTEX(logging_MutexHandle);
		/* change color */
		sel_color(lvl);

#ifdef LOG_WITH_TIME
		log_timestamp();
#endif

#ifdef LOG_WITH_LEVEL
		log_level(lvl);
#endif

#ifndef NO_RTOS
		log_current_task_name();
#endif
		xputs(str);
		xputc('\n');
		CRT_resetToDefaults();
		/* release the mutex */
		GIVE_MUTEX(logging_MutexHandle);
	}
}
