/** @file menu.c
 *  @brief menu
 *
 *  @author Vasiliy Turchenko
 *  @bug
 *  @date 20-Jan-2020
 */

#include "FreeRTOS.h"
#include "task.h"
#include "cmsis_os.h"

#include "menu.h"

#include "logging.h"

#include "kbd_task.h"

#include "profiles.h"

#include "parameters_storage.h"

#ifdef USE_FRAMEBUFFER

#include "framebuffer.h"

extern struct_FrameBuffer fb1;
extern struct_Context    *context1;
extern unsigned char      FrameBuffer1[(128 / CHAR_BIT) * (64)];
#endif

/* flag to redraw menu level */
static _Bool force_redraw = false;

static out_device_t menu_drawer = { NULL, NULL, NULL };

/**
 * @brief up_action
 * @param item
 * @return
 */
static const menu_item_t *up_action(const menu_item_t *item)
{
    if (item->prev->item_type == MENU_ITEM_HEADER) {
        return item;
    } else {
        return item->prev;
    }
}

/**
 * @brief dn_action
 * @param item
 * @return
 */
static const menu_item_t *dn_action(const menu_item_t *item)
{
    return item->next;
}

/**
 * @brief go_to_parent
 * @param item
 * @return
 */
static const menu_item_t *go_to_parent(const menu_item_t *item)
{
    return item->parent;
}

/**
 * @brief go_to_child
 * @param item
 * @return
 */
static const menu_item_t *go_to_child(const menu_item_t *item)
{
    return item->child;
}

/**
 * @brief manual_heater_ctrl
 * @return
 */
static const menu_item_t *manual_heater_ctrl(const menu_item_t *item)
{
    (void)item;
    menu_drawer.clrscr();
    extern void manual_heater_control(void);
    manual_heater_control();
    force_redraw = true;
    return item;
}

/**
 * @brief test_profiles wrapper for test function
 * @param item
 * @return
 */
static const menu_item_t *test_profiles(const menu_item_t *item)
{
    //	test_profiles_get_temp_for_time();
    force_redraw = true;
    return item;
}

/**
 * @brief test_save_cal_data
 * @param item
 * @return
 */
static const menu_item_t *test_save_cal_data(const menu_item_t *item)
{
    tc_cal_data_t     *cal1;
    static const char *fail = "test_save_cal_data failed";

    cal1 = (tc_cal_data_t *)get_cfg(CFG_TYPE_TC_CAL1);
    if (cal1 == NULL) {
        log_xputs(MSG_LEVEL_FATAL, fail);
    } else {
        cal1->A     = (float)HAL_GetTick();
        cal1->valid = true;

        if (save_cfg_pool() != SUCCESS) {
            log_xputs(MSG_LEVEL_FATAL, fail);
        }
    }
    return item;
}

/**
 * @brief calibrate_TC1_wrapper
 * @param item
 * @return
 */
static const menu_item_t *calibrate_TC1_wrapper(const menu_item_t *item)
{
    calibrate_TC1();
    force_redraw = true;
    return item;
}

/**
 * @brief calibrate_TC2_wrapper
 * @param item
 * @return
 */
static const menu_item_t *calibrate_TC2_wrapper(const menu_item_t *item)
{
    calibrate_TC2();
    force_redraw = true;
    return item;
}

/**
 * @brief temperature_info_wrapper
 * @param item
 * @return
 */
static const menu_item_t *temperature_info_wrapper(const menu_item_t *item)
{
    print_current_temperature();
    force_redraw = true;
    return item;
}

/**
 * @brief pwm_maunal_ctr_wrapper manual door control wrapper
 * @param item
 * @return
 */
static const menu_item_t *pwm_maunal_ctr_wrapper(const menu_item_t *item)
{
    force_redraw = true;
    return item;
}

/**
 * @brief start_reflow_prof1 reflow with profile 1 wrapper
 * @param item
 * @return
 */
static const menu_item_t *start_reflow_prof1(const menu_item_t *item)
{
    menu_drawer.clrscr();
    extern void start_reflow(uint8_t np);
    start_reflow(0);
    force_redraw = true;
    return item->parent;
}

/**
 * @brief start_reflow_prof1 reflow with profile 2 wrapper
 * @param item
 * @return
 */
static const menu_item_t *start_reflow_prof2(const menu_item_t *item)
{
    menu_drawer.clrscr();
    extern void start_reflow(uint8_t np);
    start_reflow(1);
    force_redraw = true;
    return item->parent;
}

/**
 * @brief start_door_cal door pwm calibration wrapper
 * @param item
 * @return
 */
static const menu_item_t *start_door_cal(const menu_item_t *item)
{
    menu_drawer.clrscr();
    extern ErrorStatus door_servo_calibrate(void);

    door_servo_calibrate();
    force_redraw = true;
    return item->parent;
}

/**
 * @brief close_door
 * @param item
 * @return
 */
static const menu_item_t *close_door(const menu_item_t *item)
{
    menu_drawer.clrscr();
    extern void door_servo_close_door(void);
    door_servo_close_door();
    force_redraw = true;
    return item->parent;
}

/**
 * @brief open_door
 * @param item
 * @return
 */
static const menu_item_t *open_door(const menu_item_t *item)
{
    menu_drawer.clrscr();
    extern void door_servo_open_door(void);
    door_servo_open_door();
    force_redraw = true;
    return item->parent;
}

/* root menu */
//menu_item_t L0_h;
extern const menu_item_t L0_i0;
extern const menu_item_t L0_i1;
extern const menu_item_t L0_i2;
extern const menu_item_t L0_i3;
extern const menu_item_t L0_i4;
#if (0)
extern const menu_item_t L0_i5;
#endif
extern const menu_item_t L0_i6;

/* iformation menu */

/* calibrate menu */
//menu_item_t calibr_back;
extern const menu_item_t calibr_tc1;
extern const menu_item_t calibr_tc2;
extern const menu_item_t calibr_actuator;
extern const menu_item_t calibr_heater;
extern const menu_item_t manual_heater;

/* reflow */
extern const menu_item_t reflow_back;

extern menu_item_t reflow1;

extern menu_item_t       reflow2;
extern const menu_item_t reflow_test_prof;

/* ROOT MENU HEADER */
static const menu_item_t L0_h = {
    .key_up_action    = NULL,
    .key_dn_action    = NULL,
    .key_esc_action   = NULL,
    .key_enter_action = NULL,

    .menu_string = "    MAIN MENU   ",

    .parent = NULL,   /* parent node */
    .child  = NULL,   /* child node */
    .next   = &L0_i0, /* next item at this level */
    .prev   = &L0_h,  /* prev item at this level */

    .item_type = MENU_ITEM_HEADER,
};

/* INFO MENU ITEM */
const menu_item_t L0_i0 = {
    .key_up_action    = up_action,
    .key_dn_action    = dn_action,
    .key_esc_action   = NULL,
    .key_enter_action = temperature_info_wrapper,

    .menu_string = "INFORMATION     ",

    .parent = NULL,   /* parent node */
    .child  = NULL,   /* child node */
    .next   = &L0_i1, /* next item at this level */
    .prev   = &L0_h,  /* prev item at this level */

    .item_type = MENU_ITEM_ACTIVE,
};

/* CALIBRATE MENU ITEM */
const menu_item_t L0_i1 = {
    .key_up_action    = up_action,
    .key_dn_action    = dn_action,
    .key_esc_action   = NULL,
    .key_enter_action = go_to_child,

    .menu_string = "CALIBRATE       ",

    .parent = NULL,        /* parent node */
    .child  = &calibr_tc1, /* child node */
    .next   = &L0_i2,      /* next item at this level */
    .prev   = &L0_i0,      /* prev item at this level */

    .item_type = MENU_ITEM_ACTIVE,
};

/* OPEN DOOR MENU ITEM */
const menu_item_t L0_i2 = {
    .key_up_action    = up_action,
    .key_dn_action    = dn_action,
    .key_esc_action   = NULL,
    .key_enter_action = open_door,

    .menu_string = "OPEN DOOR       ",

    .parent = NULL,   /* parent node */
    .child  = NULL,   /* child node */
    .next   = &L0_i3, /* next item at this level */
    .prev   = &L0_i1, /* prev item at this level */

    .item_type = MENU_ITEM_ACTIVE,
};

/* CLOSE DOOR MENU ITEM */
const menu_item_t L0_i3 = {
    .key_up_action    = up_action,
    .key_dn_action    = dn_action,
    .key_esc_action   = NULL,
    .key_enter_action = close_door,

    .menu_string = "CLOSE DOOR      ",

    .parent = NULL,   /* parent node */
    .child  = NULL,   /* child node */
    .next   = &L0_i4, /* next item at this level */
    .prev   = &L0_i2, /* prev item at this level */

    .item_type = MENU_ITEM_ACTIVE,
};

/* REFLOW WITH PROFILE 1 MENU ITEM */
const menu_item_t L0_i4 = {
    .key_up_action    = up_action,
    .key_dn_action    = dn_action,
    .key_esc_action   = NULL,
    .key_enter_action = go_to_child,

    .menu_string = "REFLOW          ",

    .parent = NULL,         /* parent node */
    .child  = &reflow_back, /* child node */
    .next   = &L0_i6,       /* next item at this level */
    .prev   = &L0_i3,       /* prev item at this level */

    .item_type = MENU_ITEM_ACTIVE,
};

#if (0)
/* REFLOW WITH PROFILE 2 MENU ITEM */
menu_item_t L0_i5 = {
    .key_up_action    = up_action,
    .key_dn_action    = dn_action,
    .key_esc_action   = NULL,
    .key_enter_action = go_to_child,

    .menu_string = "REFLOW BY PROF 2",

    .parent = NULL,            /* parent node */
    .child  = &reflow2_cancel, /* child node */
    .next   = &L0_i6,          /* next item at this level */
    .prev   = &L0_i4,          /* prev item at this level */
};
#endif

/* RESET ALL MENU ITEM */
const menu_item_t L0_i6 = {
    .key_up_action    = up_action,
    .key_dn_action    = dn_action,
    .key_esc_action   = NULL,
    .key_enter_action = NULL,

    .menu_string = "RESET SETTINGS  ",

    .parent = NULL,   /* parent node */
    .child  = NULL,   /* child node */
    .next   = &L0_i6, /* next item at this level */
    .prev   = &L0_i4, /* prev item at this level */

    .item_type = MENU_ITEM_ACTIVE,
};

/* GO BACK - level up */
static const menu_item_t calibr_back = {
    .key_up_action    = up_action,
    .key_dn_action    = dn_action,
    .key_esc_action   = NULL,
    .key_enter_action = go_to_parent,

    .menu_string = "UP ONE LEVEL    ",

    .parent = &L0_i1,       /* parent node */
    .child  = NULL,         /* child node */
    .next   = &calibr_tc1,  /* next item at this level */
    .prev   = &calibr_back, /* prev item at this level */

    .item_type = MENU_ITEM_ACTIVE,
};

/* termocouple 1 */
const menu_item_t calibr_tc1 = {
    .key_up_action    = up_action,
    .key_dn_action    = dn_action,
    .key_esc_action   = NULL,
    .key_enter_action = calibrate_TC1_wrapper,

    .menu_string = "THERMOCOUPLE 1  ",

    .parent = NULL,         /* parent node */
    .child  = NULL,         /* child node */
    .next   = &calibr_tc2,  /* next item at this level */
    .prev   = &calibr_back, /* prev item at this level */

    .item_type = MENU_ITEM_ACTIVE,
};

/* termocouple 2 */
const menu_item_t calibr_tc2 = {
    .key_up_action    = up_action,
    .key_dn_action    = dn_action,
    .key_esc_action   = NULL,
    .key_enter_action = calibrate_TC2_wrapper,

    .menu_string = "THERMOCOUPLE 2  ",

    .parent = NULL,             /* parent node */
    .child  = NULL,             /* child node */
    .next   = &calibr_actuator, /* next item at this level */
    .prev   = &calibr_tc1,      /* prev item at this level */

    .item_type = MENU_ITEM_ACTIVE,
};

/* door actuator */
const menu_item_t calibr_actuator = {
    .key_up_action    = up_action,
    .key_dn_action    = dn_action,
    .key_esc_action   = NULL,
    .key_enter_action = start_door_cal,

    .menu_string = "DOOR ACTUATOR   ",

    .parent = NULL,           /* parent node */
    .child  = NULL,           /* child node */
    .next   = &calibr_heater, /* next item at this level */
    .prev   = &calibr_tc2,    /* prev item at this level */

    .item_type = MENU_ITEM_ACTIVE,
};

/* heater  */
const menu_item_t calibr_heater = {
    .key_up_action    = up_action,
    .key_dn_action    = dn_action,
    .key_esc_action   = NULL,
    .key_enter_action = NULL,

    .menu_string = "HEATER(S)       ",

    .parent = NULL,             /* parent node */
    .child  = NULL,             /* child node */
    .next   = &manual_heater,   /* next item at this level */
    .prev   = &calibr_actuator, /* prev item at this level */

    .item_type = MENU_ITEM_ACTIVE,
};

/* manual heater control */
const menu_item_t manual_heater = {
    .key_up_action    = up_action,
    .key_dn_action    = dn_action,
    .key_esc_action   = NULL,
    .key_enter_action = manual_heater_ctrl,

    .menu_string = "MANUAL HEATR CTR",

    .parent = NULL,           /* parent node */
    .child  = NULL,           /* child node */
    .next   = &manual_heater, /* next item at this level */
    .prev   = &calibr_heater, /* prev item at this level */

    .item_type = MENU_ITEM_ACTIVE,
};

/* REFLOW SECTION */
const menu_item_t reflow_back = {
    .key_up_action    = up_action,
    .key_dn_action    = dn_action,
    .key_esc_action   = NULL,
    .key_enter_action = go_to_parent,

    .menu_string = "UP ONE LEVEL    ",

    .parent = &L0_i4,       /* parent node */
    .child  = NULL,         /* child node */
    .next   = &reflow1,     /* next item at this level */
    .prev   = &reflow_back, /* prev item at this level */

    .item_type = MENU_ITEM_ACTIVE,
};

/* reflow profile 1*/
menu_item_t reflow1 = {
    .key_up_action    = up_action,
    .key_dn_action    = dn_action,
    .key_esc_action   = NULL,
    .key_enter_action = start_reflow_prof1,

    //	.menu_string = "START REFLOW PR1",
    .menu_string = NULL,

    .parent = &L0_i4,       /* parent node */
    .child  = NULL,         /* child node */
    .next   = &reflow2,     /* next item at this level */
    .prev   = &reflow_back, /* prev item at this level */

    .item_type = MENU_ITEM_ACTIVE,
};

/* reflow profile 2*/
menu_item_t reflow2 = {
    .key_up_action    = up_action,
    .key_dn_action    = dn_action,
    .key_esc_action   = NULL,
    .key_enter_action = start_reflow_prof2,

    //	.menu_string = "START REFLOW PR2",
    .menu_string = NULL,

    .parent = &L0_i4,            /* parent node */
    .child  = NULL,              /* child node */
    .next   = &reflow_test_prof, /* next item at this level */
    .prev   = &reflow1,          /* prev item at this level */

    .item_type = MENU_ITEM_ACTIVE,
};

/* reflow reflow_test_profiles */
const menu_item_t reflow_test_prof = {
    .key_up_action    = up_action,
    .key_dn_action    = dn_action,
    .key_esc_action   = NULL,
    .key_enter_action = test_profiles,

    .menu_string = "TEST PROFILES   ",

    .parent = NULL,              /* parent node */
    .child  = NULL,              /* child node */
    .next   = &reflow_test_prof, /* next item at this level */
    .prev   = &reflow2,          /* prev item at this level */

    .item_type = MENU_ITEM_ACTIVE,

};

/* current selected item */
const menu_item_t *current_item;

/**
 * @brief init_menu_system
 * @param drawer
 * @return
 */
ErrorStatus init_menu_system(out_device_t drawer)
{
    ErrorStatus retVal = ERROR;

    if ((drawer.clrscr != NULL) && (drawer.xfunc_print != NULL) && (drawer.set_position != NULL)) {
        menu_drawer.clrscr       = drawer.clrscr;
        menu_drawer.xfunc_print  = drawer.xfunc_print;
        menu_drawer.set_position = drawer.set_position;
        retVal                   = SUCCESS;
    }

    /* set-up dynamic names in the menu(s) */
    reflow1.menu_string = (const char *)&prof1->name;
    reflow2.menu_string = (const char *)&prof2->name;

    return retVal;
}

/**
 * @brief draw_menu_level
 * @param item
 */
void draw_menu_level(const menu_item_t *item)
{

    if (item == NULL) {
       return;
    }

    const menu_item_t *prev = item->prev;
    const menu_item_t *next = prev;
    const menu_item_t *draw_item = prev;
    size_t i_start = 0U;

    static const menu_item_t *last_current = NULL;

    if ((last_current == item) && (!force_redraw)) {
        /* nothing to do */
        goto fExit;
    }

    size_t number_of_items;
    /* go to the first item */


    while (true) {
        if (prev == prev->prev) {
            break;
        }
        prev = prev->prev;
    }

    /* count items */

    number_of_items         = 1U;
    while (true) {
        if (next == next->next) {
            break;
        }
        number_of_items++;
        next = next->next;
    }
    log_xprintf(MSG_LEVEL_INFO, "Found %d menu items", number_of_items);

    /* prev points at the first item */
    menu_drawer.clrscr();
    menu_drawer.set_position(0U, 0U);




    if (draw_item->item_type == MENU_ITEM_HEADER) {
        menu_drawer.xfunc_print(draw_item->menu_string, INVERT);
        i_start++;
        draw_item = draw_item->next;
    }

    for (size_t i = i_start; i < number_of_items; i++) {
        uint8_t mode;
        if (draw_item == item) {
            mode         = INVERT;
            last_current = draw_item;
        } else {
            mode = NORM;
        }
        menu_drawer.xfunc_print(draw_item->menu_string, mode);
        //		menu_drawer.xfunc_print("\n", mode);
        draw_item = draw_item->next;
    }
    force_redraw = false;
fExit:
    return;
}

/**
 * @brief process_item
 */
void process_current_item(void)
{
    const menu_item_t *target_item = NULL;
    /* read key */

    key_code_t key_code = get_key();
    if (key_code != NO_KEY) {
        log_xprintf(MSG_LEVEL_INFO, "Key code = %d", (int)key_code);
    }

    switch (key_code) {
    case UP_KEY_RELEASED: {
        if (current_item->key_up_action != NULL) {
            target_item = current_item->key_up_action(current_item);
        }
        break;
    }

    case DN_KEY_RELEASED: {
        if (current_item->key_dn_action != NULL) {
            target_item = current_item->key_dn_action(current_item);
        }
        break;
    }

    case ESC_KEY_RELEASED: {
        if (current_item->key_esc_action != NULL) {
            target_item = current_item->key_esc_action(current_item);
        }
        break;
    }

    case ENTER_KEY_RELEASED: {
        if (current_item->key_enter_action != NULL) {
            target_item = current_item->key_enter_action(current_item);
        }
        break;
    }

    case DN_KEY_PRESSED:
    case ENTER_KEY_PRESSED:
    case ESC_KEY_PRESSED:
    case KEY1_PRESSED:
    case KEY2_PRESSED:
    case UP_KEY_PRESSED:
    case KEY1_RELEASED:
    case KEY2_RELEASED:
    case NO_KEY:
    default: {
        break;
    }
    }
    current_item = (target_item != NULL) ? target_item : current_item;
    vTaskDelay(pdMS_TO_TICKS(40U));
}

/**
 * @brief test_draw_menu_level
 */
void test_draw_menu_level(void)
{
    draw_menu_level(&L0_i1);
    HAL_Delay(1000U);
    draw_menu_level(&L0_i2);
    HAL_Delay(1000U);
    draw_menu_level(&L0_i3);
    HAL_Delay(1000U);
    draw_menu_level(&L0_i4);
    HAL_Delay(1000U);
    draw_menu_level(&L0_i6);
    HAL_Delay(1000U);
}
