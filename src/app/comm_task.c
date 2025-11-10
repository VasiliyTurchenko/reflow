/** @file comm_task.c
 *  @brief serial communication task
 *
 *  @author turchenkov@gmail.com
 *  @bug
 *  @date 13-Jan-2020
 */


#include "comm_task.h"
#include "my_comm.h"
#include "watchdog.h"
#include "task_tokens.h"
#include "logging.h"

/**
 * @brief comm_task_init
 */
void comm_task_init(void)
{
    register_magic(COMM_TASK_MAGIC);
//	i_am_alive(COMM_TASK_MAGIC);
}

/**
 * @brief comm_task_run
 */
void comm_task_run(void)
{
    (void)Transmit_RTOS();
//	i_am_alive(COMM_TASK_MAGIC);
    vTaskDelay(pdMS_TO_TICKS(10U));
}
