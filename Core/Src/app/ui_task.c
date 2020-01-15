/** @file ui_task.c
 *  @brief user interface task
 *
 *  @author turchenkov@gmail.com
 *  @bug
 *  @date 15-Jan-2020
 */


#include "ui_task.h"
#include "kbd_task.h"
#include "watchdog.h"
#include "task_tokens.h"
#include "logging.h"
#include "framebuffer.h"

/**
 * @brief ui_task_init
 */
void ui_task_init(void)
{
	register_magic(UI_TASK_MAGIC);
//	i_am_alive(ui_TASK_MAGIC);
	log_xputs(MSG_LEVEL_INFO, " started.");
}

/**
 * @brief ui_task_run
 */
void ui_task_run(void)
{
	uint8_t	up_key = get_up_key();
	uint8_t dn_key = get_dn_key();
	uint8_t esc_key = get_esc_key();
	uint8_t enter_key = get_enter_key();

	if ((up_key | dn_key | esc_key | enter_key) != 0U) {
		gotoXY(0U, 0U);
		for (size_t i = 0U; i < 8U; i++) {
			zprint("                \0", NORM);
		}
		gotoXY(0U, 0U);
	}



//	i_am_alive(ui_TASK_MAGIC);
	vTaskDelay(pdMS_TO_TICKS(20U));

}
