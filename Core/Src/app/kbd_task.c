/** @file kbd_task.c
 *  @brief keyboard task
 *
 *  @author turchenkov@gmail.com
 *  @bug
 *  @date 15-Jan-2020
 */

#include <limits.h>
#include <stdbool.h>

#include "kbd_task.h"
#include "watchdog.h"
#include "task_tokens.h"
#include "logging.h"

#ifdef STM32F103xB
#include "stm32f1xx.h"
#else
#error MCU NOT DEFINED
#endif

#define KBD_WIN_SIZE 20U

static key_code_t detect_key_event(uint8_t up_cnt, uint8_t *key_state);

static const uint16_t key_up_mask = (uint16_t)(0x01U << 2U);
static const uint16_t key_dn_mask = (uint16_t)(0x01U << 3U);
static const uint16_t key_enter_mask = (uint16_t)(0x01U << 4U);
static const uint16_t key_esc_mask = (uint16_t)(0x01U << 5U);

static uint16_t keys_mask = 0U;

static uint16_t acq_array[KBD_WIN_SIZE] = { 0 };
static uint8_t acq_pos = 0U;

static uint8_t key_up_state = 0U;
static uint8_t key_dn_state = 0U;
static uint8_t key_enter_state = 0U;
static uint8_t key_esc_state = 0U;

static key_code_t last_key_event = NO_KEY;

#if(false)
/**
 * @brief get_up_key
 * @return
 */
uint8_t get_up_key(void)
{
	return key_up_state;
}

/**
 * @brief get_dn_key
 * @return
 */
uint8_t get_dn_key(void)
{
	return key_dn_state;
}

/**
 * @brief get_enter_key
 * @return
 */
uint8_t get_enter_key(void)
{
	return key_enter_state;
}

/**
 * @brief get_esc_key
 * @return
 */
uint8_t get_esc_key(void)
{
	return key_esc_state;
}
#endif

/**
 * @brief clear_key_buffer
 */
void clear_key_buffer(void)
{
	last_key_event = NO_KEY;
}

/**
 * @brief get_key
 * @return
 */
key_code_t get_key(void)
{
	key_code_t retVal = last_key_event;
	clear_key_buffer();
	return retVal;
}

/**
 * @brief wait_for_key
 * @param key_req
 * @return
 */
_Bool wait_for_key(key_code_t key_req)
{
	clear_key_buffer();
	while (true) {
		if (get_key() == key_req) {
			break;
		}
		vTaskDelay(pdMS_TO_TICKS(10U));
	}
	return true;
}

/**
 * @brief kbd_task_init
 */
void kbd_task_init(void)
{
	register_magic(KBD_TASK_MAGIC);
	//	i_am_alive(KBD_TASK_MAGIC);
	log_xputs(MSG_LEVEL_INFO, " started.");
	keys_mask = key_up_mask | key_dn_mask | key_enter_mask | key_esc_mask;
}

/**
 * @brief kbd_task_run
 */
void kbd_task_run(void)
{
	static uint32_t notified_val = 0U;
	/* wait for periodic event */
	/* every 2 ms */
	if (xTaskNotifyWait(ULONG_MAX, ULONG_MAX, &notified_val,
			    portMAX_DELAY) == pdTRUE) {
		if (notified_val != 1U) {
			// error!!!
		}
	}
	/* it's time to read GPIO */
	acq_array[acq_pos] = (uint16_t)GPIOA->IDR & keys_mask;
	acq_pos++;
	if (acq_pos == KBD_WIN_SIZE) {
		acq_pos = 0U;

		uint8_t key_up_cnt = 0U;
		uint8_t key_dn_cnt = 0U;
		uint8_t key_enter_cnt = 0U;
		uint8_t key_esc_cnt = 0U;

		for (size_t i = 0U; i < KBD_WIN_SIZE; i++) {
			/* count ones */
			if ((acq_array[i] & key_up_mask) != 0U) {
				key_up_cnt++;
			}

			if ((acq_array[i] & key_dn_mask) != 0U) {
				key_dn_cnt++;
			}

			if ((acq_array[i] & key_enter_mask) != 0U) {
				key_enter_cnt++;
			}

			if ((acq_array[i] & key_esc_mask) != 0U) {
				key_esc_cnt++;
			}
		}

		/* events prioritization */
		last_key_event = detect_key_event(key_up_cnt, &key_up_state);
		key_code_t dn = detect_key_event(key_dn_cnt, &key_dn_state);
		if (dn != NO_KEY) {
			last_key_event = dn;
		}

		key_code_t ent = detect_key_event(key_enter_cnt, &key_enter_state);
		if (ent!= NO_KEY) {
			last_key_event = ent;
		}

		key_code_t esc = detect_key_event(key_esc_cnt, &key_esc_state);
		if (esc != NO_KEY) {
			last_key_event = esc;
		}
	}
	//	i_am_alive(KBD_TASK_MAGIC);
}

/**
 * @brief detect_key_event
 * @param up_cnt
 * @param key_state
 * @return
 */
static key_code_t detect_key_event(uint8_t up_cnt, uint8_t *key_state)
{
	key_code_t retVal = NO_KEY;
	if (up_cnt == KBD_WIN_SIZE) {
		if (*key_state == 1U) {
			if (key_state == &key_up_state) {
				retVal = UP_KEY_RELEASED;
			} else if (key_state == &key_dn_state) {
				retVal = DN_KEY_RELEASED;
			} else if (key_state == &key_enter_state) {
				retVal = ENTER_KEY_RELEASED;
			} else if (key_state == &key_esc_state) {
				retVal = ESC_KEY_RELEASED;
			}
		}
		*key_state = 0U;

	} else if (up_cnt == 0U) {
		if (*key_state == 0U) {
			if (key_state == &key_up_state) {
				retVal = UP_KEY_PRESSED;
			} else if (key_state == &key_dn_state) {
				retVal = DN_KEY_PRESSED;
			} else if (key_state == &key_enter_state) {
				retVal = ENTER_KEY_PRESSED;
			} else if (key_state == &key_esc_state) {
				retVal = ESC_KEY_PRESSED;
			}
		}
		*key_state = 1U;
	}
	return retVal;
}
