/** @file menu.h
 *  @brief menu
 *
 *  @author Vasiliy Turchenko
 *  @bug
 *  @date 20-Jan-2020
 */

#ifndef MENU_H
#define MENU_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stddef.h>
#include <limits.h>

#ifdef STM32F103xB
#include "stm32f1xx.h"
#else
#error MCU NOT DEFINED
#endif

/**
	type of the menu item
*/
typedef enum {
	MENU_ITEM_HEADER,	///< will be printed inverted. Don't scroll.
	MENU_ITEM_INACTIVE,	///< will be printed as normal, but skipped
				///< during navigation
	MENU_ITEM_ACTIVE	///< normal item
} menu_item_type_t;

struct menu_item;

/* function pointer */
typedef const struct menu_item * (*action_fn)(const struct menu_item *);

/**
 *	@brief single menu item structure
*/
typedef	struct menu_item {
	action_fn key_up_action;
	action_fn key_dn_action;
	action_fn key_esc_action;
	action_fn key_enter_action;

	char *	menu_string;

	const struct menu_item * const parent; /* parent node */
	const struct menu_item * const child;  /* child node */
	const struct menu_item * const next;   /* next item at this level */
	const struct menu_item * const prev;   /* prev item at this level */

	menu_item_type_t item_type;

} menu_item_t;

/**
 *	object used to draw menu
 */
typedef struct out_device {
	/* pointer to the text output function */
	uint32_t (*xfunc_print)(char *, uint8_t);

	/* pointer to the cursor positioning function */
	uint8_t (*set_position)(uint32_t x, uint32_t y);

	/* pointer to the clear screeen function */
	uint8_t (*clrscr)(void);
} out_device_t;

/* 1st menu item at root level */
extern const menu_item_t L0_i0;

/* current item highlighted */
extern const menu_item_t * current_item;


ErrorStatus init_menu_system(out_device_t drawer);
void draw_menu_level(const menu_item_t * item);
void process_current_item(void);

void	test_draw_menu_level(void);

#ifdef __cplusplus
}
#endif

#endif
