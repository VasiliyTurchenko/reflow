SET(GROUP_SRC_SRC
	${CMAKE_CURRENT_LIST_DIR}/FreeRTOSConfig.h
	${CMAKE_CURRENT_LIST_DIR}/adc.c
	${CMAKE_CURRENT_LIST_DIR}/adc.h
	${CMAKE_CURRENT_LIST_DIR}/crc.c
	${CMAKE_CURRENT_LIST_DIR}/crc.h
	${CMAKE_CURRENT_LIST_DIR}/debug.h
	${CMAKE_CURRENT_LIST_DIR}/dma.c
	${CMAKE_CURRENT_LIST_DIR}/dma.h
	${CMAKE_CURRENT_LIST_DIR}/freertos.c
	${CMAKE_CURRENT_LIST_DIR}/globaldef.h
	${CMAKE_CURRENT_LIST_DIR}/gpio.c
	${CMAKE_CURRENT_LIST_DIR}/gpio.h
	${CMAKE_CURRENT_LIST_DIR}/i2c.c
	${CMAKE_CURRENT_LIST_DIR}/i2c.h
	${CMAKE_CURRENT_LIST_DIR}/iwdg.c
	${CMAKE_CURRENT_LIST_DIR}/iwdg.h
	${CMAKE_CURRENT_LIST_DIR}/main.c
	${CMAKE_CURRENT_LIST_DIR}/main.h
	${CMAKE_CURRENT_LIST_DIR}/rtc.c
	${CMAKE_CURRENT_LIST_DIR}/rtc.h
	${CMAKE_CURRENT_LIST_DIR}/spi.c
	${CMAKE_CURRENT_LIST_DIR}/spi.h
	${CMAKE_CURRENT_LIST_DIR}/tim.c
	${CMAKE_CURRENT_LIST_DIR}/tim.h
	${CMAKE_CURRENT_LIST_DIR}/usart.c
	${CMAKE_CURRENT_LIST_DIR}/usart.h
	${CMAKE_CURRENT_LIST_DIR}/wwdg.c
	${CMAKE_CURRENT_LIST_DIR}/wwdg.h
) # of GROUP_SRC_SRC

SET(GROUP_APP_SRC
	${CMAKE_CURRENT_LIST_DIR}/app/comm_task.c
	${CMAKE_CURRENT_LIST_DIR}/app/comm_task.h
	${CMAKE_CURRENT_LIST_DIR}/app/control_task.c
	${CMAKE_CURRENT_LIST_DIR}/app/control_task.h
	${CMAKE_CURRENT_LIST_DIR}/app/kbd_task.c
	${CMAKE_CURRENT_LIST_DIR}/app/kbd_task.h
	${CMAKE_CURRENT_LIST_DIR}/app/regulator.h
	${CMAKE_CURRENT_LIST_DIR}/app/startup.c
	${CMAKE_CURRENT_LIST_DIR}/app/startup.h
	${CMAKE_CURRENT_LIST_DIR}/app/task_tokens.h
	${CMAKE_CURRENT_LIST_DIR}/app/temperatur_task.c
	${CMAKE_CURRENT_LIST_DIR}/app/temperatur_task.h
	${CMAKE_CURRENT_LIST_DIR}/app/ui_task.c
	${CMAKE_CURRENT_LIST_DIR}/app/ui_task.h
) # of GROUP_APP_SRC

SET(GROUP_BSP_SRC
	${CMAKE_CURRENT_LIST_DIR}/bsp/door_pwm.h
	${CMAKE_CURRENT_LIST_DIR}/bsp/door_servo.c
	${CMAKE_CURRENT_LIST_DIR}/bsp/door_servo.h
	${CMAKE_CURRENT_LIST_DIR}/bsp/exti.c
	${CMAKE_CURRENT_LIST_DIR}/bsp/exti.h
	${CMAKE_CURRENT_LIST_DIR}/bsp/heat_cool.h
	${CMAKE_CURRENT_LIST_DIR}/bsp/parameters_storage.c
	${CMAKE_CURRENT_LIST_DIR}/bsp/parameters_storage.h
	${CMAKE_CURRENT_LIST_DIR}/bsp/temerature_measurement.c
	${CMAKE_CURRENT_LIST_DIR}/bsp/temerature_measurement.h
) # of GROUP_BSP_SRC

SET(GROUP_DISPLAY_SRC
	${CMAKE_CURRENT_LIST_DIR}/display/display_pins.h
	${CMAKE_CURRENT_LIST_DIR}/display/framebuffer.c
	${CMAKE_CURRENT_LIST_DIR}/display/framebuffer.h
	${CMAKE_CURRENT_LIST_DIR}/display/ssd1306.c
	${CMAKE_CURRENT_LIST_DIR}/display/ssd1306.h
	${CMAKE_CURRENT_LIST_DIR}/display/tvv_macros.h
) # of GROUP_DISPLAY_SRC

SET(GROUP_DISPLAY_FONT8X8_MASTER_SRC
	${CMAKE_CURRENT_LIST_DIR}/display/font8x8-master/font8x8.h
	${CMAKE_CURRENT_LIST_DIR}/display/font8x8-master/font8x8_basic.h
	${CMAKE_CURRENT_LIST_DIR}/display/font8x8-master/font8x8_block.h
	${CMAKE_CURRENT_LIST_DIR}/display/font8x8-master/font8x8_box.h
	${CMAKE_CURRENT_LIST_DIR}/display/font8x8-master/font8x8_control.h
	${CMAKE_CURRENT_LIST_DIR}/display/font8x8-master/font8x8_ext_latin.h
	${CMAKE_CURRENT_LIST_DIR}/display/font8x8-master/font8x8_greek.h
	${CMAKE_CURRENT_LIST_DIR}/display/font8x8-master/font8x8_hiragana.h
	${CMAKE_CURRENT_LIST_DIR}/display/font8x8-master/font8x8_latin.h
	${CMAKE_CURRENT_LIST_DIR}/display/font8x8-master/font8x8_misc.h
	${CMAKE_CURRENT_LIST_DIR}/display/font8x8-master/font8x8_sga.h
) # of GROUP_DISPLAY_FONT8X8_MASTER_SRC

SET(GROUP_HELPERS_SRC
	${CMAKE_CURRENT_LIST_DIR}/helpers/CAssert.h
	${CMAKE_CURRENT_LIST_DIR}/helpers/ascii_helpers.c
	${CMAKE_CURRENT_LIST_DIR}/helpers/ascii_helpers.h
	${CMAKE_CURRENT_LIST_DIR}/helpers/crc32_helpers.c
	${CMAKE_CURRENT_LIST_DIR}/helpers/crc32_helpers.h
	${CMAKE_CURRENT_LIST_DIR}/helpers/ftoa.c
	${CMAKE_CURRENT_LIST_DIR}/helpers/ftoa.h
	${CMAKE_CURRENT_LIST_DIR}/helpers/hex_gen.c
	${CMAKE_CURRENT_LIST_DIR}/helpers/hex_gen.h
	${CMAKE_CURRENT_LIST_DIR}/helpers/i2c_eeprom.c
	${CMAKE_CURRENT_LIST_DIR}/helpers/i2c_eeprom.h
	${CMAKE_CURRENT_LIST_DIR}/helpers/logging.c
	${CMAKE_CURRENT_LIST_DIR}/helpers/logging.h
	${CMAKE_CURRENT_LIST_DIR}/helpers/messages.c
	${CMAKE_CURRENT_LIST_DIR}/helpers/messages.h
	${CMAKE_CURRENT_LIST_DIR}/helpers/mutex_helpers.h
	${CMAKE_CURRENT_LIST_DIR}/helpers/myCRT.c
	${CMAKE_CURRENT_LIST_DIR}/helpers/myCRT.h
	${CMAKE_CURRENT_LIST_DIR}/helpers/my_comm.c
	${CMAKE_CURRENT_LIST_DIR}/helpers/my_comm.h
	${CMAKE_CURRENT_LIST_DIR}/helpers/num_helpers.c
	${CMAKE_CURRENT_LIST_DIR}/helpers/num_helpers.h
	${CMAKE_CURRENT_LIST_DIR}/helpers/rtc_helpers.c
	${CMAKE_CURRENT_LIST_DIR}/helpers/rtc_helpers.h
	${CMAKE_CURRENT_LIST_DIR}/helpers/sys_cfg.h
	${CMAKE_CURRENT_LIST_DIR}/helpers/sys_helpers.c
	${CMAKE_CURRENT_LIST_DIR}/helpers/sys_helpers.h
	${CMAKE_CURRENT_LIST_DIR}/helpers/test_ascii_helpers.h
	${CMAKE_CURRENT_LIST_DIR}/helpers/unused.h
	${CMAKE_CURRENT_LIST_DIR}/helpers/watchdog.c
	${CMAKE_CURRENT_LIST_DIR}/helpers/watchdog.h
	${CMAKE_CURRENT_LIST_DIR}/helpers/xprintf.c
	${CMAKE_CURRENT_LIST_DIR}/helpers/xprintf.h
) # of GROUP_HELPERS_SRC

SET(GROUP_MENU_SRC
	${CMAKE_CURRENT_LIST_DIR}/menu/menu.c
	${CMAKE_CURRENT_LIST_DIR}/menu/menu.h
) # of GROUP_MENU_SRC

SET(GROUP_PROFILES_SRC
	${CMAKE_CURRENT_LIST_DIR}/profiles/profiles.c
	${CMAKE_CURRENT_LIST_DIR}/profiles/profiles.h
	${CMAKE_CURRENT_LIST_DIR}/profiles/regulator.c
) # of GROUP_PROFILES_SRC


SET(INCLUDE_DIRS_SRC_LIST
	${CMAKE_CURRENT_LIST_DIR}
	${CMAKE_CURRENT_LIST_DIR}/app
	${CMAKE_CURRENT_LIST_DIR}/bsp
	${CMAKE_CURRENT_LIST_DIR}/display
	${CMAKE_CURRENT_LIST_DIR}/display/font8x8-master
	${CMAKE_CURRENT_LIST_DIR}/helpers
	${CMAKE_CURRENT_LIST_DIR}/menu
	${CMAKE_CURRENT_LIST_DIR}/profiles
) # of INCLUDE_DIRS_SRC_LIST

SET(SRC_SOURCES_LIST
	${GROUP_SRC_SRC}
	${GROUP_APP_SRC}
	${GROUP_BSP_SRC}
	${GROUP_DISPLAY_SRC}
	${GROUP_DISPLAY_FONT8X8_MASTER_SRC}
	${GROUP_HELPERS_SRC}
	${GROUP_MENU_SRC}
	${GROUP_PROFILES_SRC}
) # of SRC_SOURCES_LIST