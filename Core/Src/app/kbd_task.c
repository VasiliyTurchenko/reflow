/** @file kbd_task.c
 *  @brief keyboard task
 *
 *  @author turchenkov@gmail.com
 *  @bug
 *  @date 15-Jan-2020
 */

#include <limits.h>

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


/**
 * @brief get_up_key
 * @return
 */
uint8_t	get_up_key(void)
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

		uint8_t	key_up_cnt = 0U;
		uint8_t	key_dn_cnt = 0U;
		uint8_t	key_enter_cnt = 0U;
		uint8_t	key_esc_cnt = 0U;

		for (size_t i = 0U; i < KBD_WIN_SIZE; i++) {
			/* count ones */
			if ( (acq_array[i] & key_up_mask) != 0U) {
				key_up_cnt++;
			}

			if ( (acq_array[i] & key_dn_mask) != 0U) {
				key_dn_cnt++;
			}

			if ( (acq_array[i] & key_enter_mask) != 0U) {
				key_enter_cnt++;
			}

			if ( (acq_array[i] & key_esc_mask) != 0U) {
				key_esc_cnt++;
			}
		}

		if (key_up_cnt == KBD_WIN_SIZE) {
			key_up_state = 1U;
		} else if (key_up_cnt == 0U) {
			key_up_state = 0U;
		}

		if (key_dn_cnt == KBD_WIN_SIZE) {
			key_dn_state = 1U;
		} else if (key_dn_cnt == 0U) {
			key_dn_state = 0U;
		}

		if (key_enter_cnt == KBD_WIN_SIZE) {
			key_enter_state = 1U;
		} else if (key_enter_cnt == 0U) {
			key_enter_state = 0U;
		}

		if (key_esc_cnt == KBD_WIN_SIZE) {
			key_esc_state = 1U;
		} else if (key_esc_cnt == 0U) {
			key_esc_state = 0U;
		}

	}
	//	i_am_alive(KBD_TASK_MAGIC);
}
