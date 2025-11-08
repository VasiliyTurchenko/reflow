/** @file logging.h
 *  @brief central logging system
 *  @author Vasiliy Turchenko
 *  @bug
 *  @date 05-Oct-2019
 */

#ifndef LOGGING_H
#define LOGGING_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>

#include "xprintf.h"
#include "myCRT.h"
#include "unused.h"

#define LOG_WITH_TIME
#define LOG_WITH_LEVEL

/* there should be single include which holds all the mutex and thread defs */
#include "platform_threading_defs.h"

/* log messages level */

/* fatal failures, only reboot can help */
#define MSG_LEVEL_FATAL_ 1

/* serious failures, no reboot needed */
#define MSG_LEVEL_SERIOUS_ 2

/* normal process errors, no reboot needed */
#define MSG_LEVEL_PROC_ERR_ 4

/* info messages */
#define MSG_LEVEL_INFO_ 8

/* extended info messages */
#define MSG_LEVEL_EXT_INF_ 16

/* task initialisation messages */
#define MSG_LEVEL_TASK_INIT_ 32

typedef enum {
    MSG_LEVEL_FATAL     = MSG_LEVEL_FATAL_,
    MSG_LEVEL_SERIOUS   = MSG_LEVEL_SERIOUS_,
    MSG_LEVEL_PROC_ERR  = MSG_LEVEL_PROC_ERR_,
    MSG_LEVEL_INFO      = MSG_LEVEL_INFO_,
    MSG_LEVEL_EXT_INF   = MSG_LEVEL_EXT_INF_,
    MSG_LEVEL_TASK_INIT = MSG_LEVEL_TASK_INIT_,
    MSG_LEVEL_ALL       = 0x7FFFFFFF,
} MSG_LEVEL;

extern const char *delim;

void log_nice_mode_on(void);
void log_nice_mode_off(void);

void log_set_mask_on(MSG_LEVEL lvl);
void log_set_mask_off(MSG_LEVEL lvl);
// void log_xputc(MSG_LEVEL lvl, char c);
void log_xputs(MSG_LEVEL lvl, const char *str);
#ifndef NO_RTOS
void log_current_task_name(void);
#else
static inline void log_current_task_name(void) {}
#endif

#ifdef LOG_WITH_TIME
void log_timestamp(void);
#endif

#ifdef LOG_WITH_LEVEL
void log_level(MSG_LEVEL lev);
#endif

void sel_color(MSG_LEVEL lvl);

_Bool filterIsPassed(MSG_LEVEL lvl);

#define log_xprintf(MSG_LVL, ...)                                                                  \
    do {                                                                                           \
        if (filterIsPassed((MSG_LVL))) {                                                           \
            /* grab the mutex */                                                                   \
            TAKE_MUTEX(logging_MutexHandle);                                                       \
            sel_color(MSG_LVL);                                                                    \
            log_timestamp();                                                                       \
            log_level(MSG_LVL);                                                                    \
            log_current_task_name();                                                               \
            xprintf(__VA_ARGS__);                                                                  \
            xputc('\n');                                                                           \
            CRT_resetToDefaults();                                                                 \
            GIVE_MUTEX(logging_MutexHandle);                                                       \
        }                                                                                          \
    } while (false)

#define bare_xprintf(FMT, ...)                                                                   \
    do {                                                                                           \
        /* grab the mutex */                                                                       \
        TAKE_MUTEX(logging_MutexHandle);                                                           \
        xprintf(FMT, __VA_ARGS__);                                                                      \
        GIVE_MUTEX(logging_MutexHandle);                                                           \
    }                                                                                              \
    while (false)

#define log_xdump(MSG_LVL, ...)                                                                    \
    do {                                                                                           \
        if (filterIsPassed((MSG_LVL))) {                                                           \
            /* grab the mutex */                                                                   \
            TAKE_MUTEX(logging_MutexHandle);                                                       \
            sel_color(MSG_LVL);                                                                    \
            log_timestamp();                                                                       \
            log_level(MSG_LVL);                                                                    \
            log_current_task_name();                                                               \
            put_dump(__VA_ARGS__);                                                                 \
            xputc('\n');                                                                           \
            CRT_resetToDefaults();                                                                 \
            GIVE_MUTEX(logging_MutexHandle);                                                       \
        }                                                                                          \
    } while (false)

#define log_err_fatal()                                                                            \
    {                                                                                              \
        log_xprintf(MSG_LEVEL_FATAL, "error at line %d in file %s", __LINE__, __FILE__);           \
    }

#define log_err_fatal_w_string(S)                                                                            \
{                                                                                              \
            log_xprintf(MSG_LEVEL_FATAL, "error at line %d in file %s: %s", __LINE__, __FILE__, S);           \
}


/* this  macro  converts compile time defined constant int to string */
#define LTS(L) #L

#ifdef __cplusplus
}
#endif

#endif // LOGGING_H
