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

/* key codes */
typedef enum {
	NO_KEY = 0x00,

	UP_KEY_PRESSED = 0x01,
	UP_KEY_RELEASED = 0x81,

	DN_KEY_PRESSED = 0x02,
	DN_KEY_RELEASED = 0x82,

	ESC_KEY_PRESSED = 0x03,
	ESC_KEY_RELEASED = 0x83,

	ENTER_KEY_PRESSED = 0x04,
	ENTER_KEY_RELEASED = 0x84,

} key_code_t;


void kbd_task_init(void);
void kbd_task_run(void);

void clear_key_buffer(void);
key_code_t get_key(void);

_Bool wait_for_key(key_code_t key_req);
_Bool wait_for_enter_or_esc(void);

#if(false)
uint8_t	get_up_key(void);
uint8_t get_dn_key(void);
uint8_t get_enter_key(void);
uint8_t get_esc_key(void);
#endif

#ifdef __cplusplus
}
#endif

#endif // KBD_TASK_H
