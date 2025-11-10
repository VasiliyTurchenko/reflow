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
#include "main.h"

// #ifdef STM32F103xB
// #include "stm32f1xx.h"
// #else
// #error MCU NOT DEFINED
// #endif

#define KBD_WIN_SIZE 10U

#define N_ROWS 3
#define N_COLS 2

struct KeyMatrixRuntime;

// Physical keyboard description
typedef struct KeyMatrix {
    const uint8_t n_rows; ///< Number of rows, up to 16
    const uint8_t n_cols; ///< Number of columns, up to 16

    void (*const drop_next_row)(struct KeyMatrixRuntime *);
    void (*const rise_rows)(struct KeyMatrixRuntime *);
    uint16_t (*const read_col)(struct KeyMatrixRuntime *);

    GPIO_TypeDef *const row_ports[N_ROWS];
    uint16_t const      row_pins[N_ROWS];

    GPIO_TypeDef *const col_ports[N_COLS];
    uint16_t const      col_pins[N_COLS];

} KeyMatrix_t;

typedef struct KeyMatrixRuntime {
    const KeyMatrix_t *km;
    uint16_t           current_row; ///< The row which set to 0
    uint16_t           clear_mask;
} KeyMatrixRuntime_t;

/**
 * @brief rise_row_lines
 * @param km
 */
static void rise_row_lines(KeyMatrixRuntime_t *kmr)
{
    const KeyMatrix_t *km = kmr->km;
    for (uint8_t i = 0U; i < km->n_rows; i++) {
        HAL_GPIO_WritePin(km->row_ports[i], km->row_pins[i], GPIO_PIN_SET);
    }
    kmr->current_row = UINT16_MAX;
}

/**
 * @brief drop_next_row_line
 * @param km
 * @param row
 */
static void drop_next_row_line(KeyMatrixRuntime_t *kmr)
{
    const KeyMatrix_t *km = kmr->km;
    uint16_t           cr = kmr->current_row + 1U;
    km->rise_rows(kmr);
    cr = cr % km->n_rows;
    HAL_GPIO_WritePin(km->row_ports[cr], km->row_pins[cr], GPIO_PIN_RESET);
    kmr->current_row = cr;
}

/**
 * @brief read_column
 * @param km
 * @return
 */
static uint16_t read_column(KeyMatrixRuntime_t *kmr)
{
    const KeyMatrix_t *km     = kmr->km;
    uint16_t           retVal = 0U;
    if (kmr->current_row < km->n_rows) {
        for (uint8_t i = 0U; i < km->n_cols; i++) {
            uint8_t pt = (GPIO_PIN_RESET == HAL_GPIO_ReadPin(km->col_ports[i], km->col_pins[i])) ?
                                 1U :
                                 0U;
            pt         = (uint8_t)(pt << i);
            retVal     = retVal | (uint16_t)pt;
        }
    }
    return (uint16_t)(retVal << (kmr->current_row * km->n_cols));
}

static const KeyMatrix_t key_matrix = { .n_rows        = N_ROWS,
                                        .n_cols        = N_COLS,
                                        .drop_next_row = drop_next_row_line,
                                        .rise_rows     = rise_row_lines,
                                        .read_col      = read_column,
                                        .row_ports     = { ROW0_GPIO_Port, ROW1_GPIO_Port,
                                                           ROW2_GPIO_Port },
                                        .row_pins      = { ROW0_PIN, ROW1_PIN, ROW2_PIN },
                                        .col_ports     = { COL0_GPIO_Port, COL1_GPIO_Port },
                                        .col_pins      = { COL0_PIN, COL1_PIN } };

static KeyMatrixRuntime_t key_matrix_runtime = { .km = &key_matrix, .current_row = UINT16_MAX };

static key_code_t detect_key_event(uint8_t up_cnt, uint8_t *key_state);

static const uint16_t key_up_mask    = 1U;
static const uint16_t key_dn_mask    = 2U;
static const uint16_t key_enter_mask = 8U;
static const uint16_t key_esc_mask   = 32U;
static const uint16_t key_1_mask     = 16U;
static const uint16_t key_2_mask     = 4U;

static uint16_t keys_mask = 0U;

static uint16_t acq_array[KBD_WIN_SIZE] = { 0 };
static uint8_t  acq_pos                 = 0U;

static uint8_t key_up_state    = 0U;
static uint8_t key_dn_state    = 0U;
static uint8_t key_enter_state = 0U;
static uint8_t key_esc_state   = 0U;
// static uint8_t key_1_state     = 0U;
// static uint8_t key_2_state     = 0U;

static key_code_t last_key_event = NO_KEY;

#if (false)
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
 * @brief wait_for_enter_or_esc
 * @return true if enter, false if esc
 */
_Bool wait_for_enter_or_esc(void)
{
    clear_key_buffer();
    key_code_t key;
    while (true) {
        key = get_key();
        if ((key == ESC_KEY_RELEASED) || (key == ENTER_KEY_RELEASED)) {
            break;
        }
        vTaskDelay(pdMS_TO_TICKS(10U));
    }
    return (key == ENTER_KEY_RELEASED) ? true : false;
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

    uint16_t clr_mask = 0U;
    for (uint8_t i = 0U; i < key_matrix_runtime.km->n_cols; i++) {
        clr_mask = (uint16_t)(clr_mask | (1U << i));
    }
    key_matrix_runtime.clear_mask = clr_mask;
    key_matrix_runtime.km->drop_next_row(&key_matrix_runtime);

    keys_mask = key_up_mask;
    keys_mask |= key_dn_mask;
    keys_mask |= key_enter_mask;
    keys_mask |= key_esc_mask;
    keys_mask |= key_1_mask;
    keys_mask |= key_2_mask;
}

/**
 * @brief kbd_task_run
 */
void kbd_task_run(void)
{
    static uint32_t notified_val = 0U;
    static uint16_t keys_line    = 0U;

    /* wait for periodic event */
    /* every 2 ms */
    if (xTaskNotifyWait(ULONG_MAX, ULONG_MAX, &notified_val, portMAX_DELAY) == pdTRUE) {
        if (notified_val != 1U) {
            // error!!!
        }
    }
    // the required row is alredy set to 0
    const uint16_t keys_column = key_matrix_runtime.km->read_col(&key_matrix_runtime);
    const uint16_t clrm =
            (uint16_t) ~(key_matrix_runtime.clear_mask
                         << (key_matrix_runtime.current_row * key_matrix_runtime.km->n_cols));

    keys_line = keys_line & clrm;
    keys_line = keys_line | keys_column;

    // prepare the next row
    key_matrix_runtime.km->drop_next_row(&key_matrix_runtime);
    if (key_matrix_runtime.current_row == 0U) {
        acq_array[acq_pos] = (uint16_t)keys_line;
        acq_pos++;
        if (acq_pos == KBD_WIN_SIZE) {
            acq_pos = 0U;

            uint8_t key_up_cnt    = 0U;
            uint8_t key_dn_cnt    = 0U;
            uint8_t key_enter_cnt = 0U;
            uint8_t key_esc_cnt   = 0U;
            // uint8_t key_1_cnt     = 0U;
            // uint8_t key_2_cnt     = 0U;

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

            //            log_xprintf(MSG_LEVEL_EXT_INF, "%d, %d, %d, %d", key_up_cnt, key_dn_cnt, key_enter_cnt, key_esc_cnt);
            //            log_xprintf(MSG_LEVEL_EXT_INF, "                   %d, %d, %d, %d", key_up_state, key_dn_state, key_enter_state, key_esc_state);

            /* events prioritization */
            key_code_t backup_evt = last_key_event;

            last_key_event = detect_key_event(key_up_cnt, &key_up_state);

            key_code_t dn = detect_key_event(key_dn_cnt, &key_dn_state);
            if (dn != NO_KEY) {
                last_key_event = dn;
            }

            key_code_t ent = detect_key_event(key_enter_cnt, &key_enter_state);
            if (ent != NO_KEY) {
                last_key_event = ent;
            }

            key_code_t esc = detect_key_event(key_esc_cnt, &key_esc_state);
            if (esc != NO_KEY) {
                last_key_event = esc;
            }

            if (last_key_event == NO_KEY) {
                last_key_event = backup_evt;
            }
        }
        keys_line = 0U;
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
        if (*key_state == 0U) {
            if (key_state == &key_up_state) {
                retVal = UP_KEY_PRESSED;
            } else if (key_state == &key_dn_state) {
                retVal = DN_KEY_PRESSED;
            } else if (key_state == &key_enter_state) {
                retVal = ENTER_KEY_PRESSED;
            } else if (key_state == &key_esc_state) {
                retVal = ESC_KEY_PRESSED;
            } else {
            }
        }
        *key_state = 1U;

    } else if (up_cnt == 0U) {
        if (*key_state == 1U) {
            if (key_state == &key_up_state) {
                retVal = UP_KEY_RELEASED;
            } else if (key_state == &key_dn_state) {
                retVal = DN_KEY_RELEASED;
            } else if (key_state == &key_enter_state) {
                retVal = ENTER_KEY_RELEASED;
            } else if (key_state == &key_esc_state) {
                retVal = ESC_KEY_RELEASED;
            } else {
            }
        }
        *key_state = 0U;
    }
    return retVal;
}
