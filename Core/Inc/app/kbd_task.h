/** @file kbd_task.h
 *  @brief
 *
 *  @author Vasiliy Turchenko
 *  @bug
 *  @date
 */

#ifndef KBD_TASK_H
#define KBD_TASK_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

void kbd_task_init(void);
void kbd_task_run(void);

uint8_t	get_up_key(void);
uint8_t get_dn_key(void);
uint8_t get_enter_key(void);
uint8_t get_esc_key(void);

#ifdef __cplusplus
}
#endif

#endif // KBD_TASK_H
