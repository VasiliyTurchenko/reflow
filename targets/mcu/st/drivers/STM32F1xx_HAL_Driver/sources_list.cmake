message(STATUS "Processing ${CMAKE_CURRENT_LIST_FILE}...")
SET(GROUP_INC_SRC
#	${CMAKE_CURRENT_LIST_DIR}/Inc/stm32_assert_template.h
	${CMAKE_CURRENT_LIST_DIR}/Inc/stm32f1xx_hal.h
	${CMAKE_CURRENT_LIST_DIR}/Inc/stm32f1xx_hal_adc.h
	${CMAKE_CURRENT_LIST_DIR}/Inc/stm32f1xx_hal_adc_ex.h
	${CMAKE_CURRENT_LIST_DIR}/Inc/stm32f1xx_hal_can.h
	${CMAKE_CURRENT_LIST_DIR}/Inc/stm32f1xx_hal_cec.h
#	${CMAKE_CURRENT_LIST_DIR}/Inc/stm32f1xx_hal_conf_template.h
	${CMAKE_CURRENT_LIST_DIR}/Inc/stm32f1xx_hal_cortex.h
	${CMAKE_CURRENT_LIST_DIR}/Inc/stm32f1xx_hal_crc.h
	${CMAKE_CURRENT_LIST_DIR}/Inc/stm32f1xx_hal_dac.h
	${CMAKE_CURRENT_LIST_DIR}/Inc/stm32f1xx_hal_dac_ex.h
	${CMAKE_CURRENT_LIST_DIR}/Inc/stm32f1xx_hal_def.h
	${CMAKE_CURRENT_LIST_DIR}/Inc/stm32f1xx_hal_dma.h
	${CMAKE_CURRENT_LIST_DIR}/Inc/stm32f1xx_hal_dma_ex.h
	${CMAKE_CURRENT_LIST_DIR}/Inc/stm32f1xx_hal_eth.h
	${CMAKE_CURRENT_LIST_DIR}/Inc/stm32f1xx_hal_exti.h
	${CMAKE_CURRENT_LIST_DIR}/Inc/stm32f1xx_hal_flash.h
	${CMAKE_CURRENT_LIST_DIR}/Inc/stm32f1xx_hal_flash_ex.h
	${CMAKE_CURRENT_LIST_DIR}/Inc/stm32f1xx_hal_gpio.h
	${CMAKE_CURRENT_LIST_DIR}/Inc/stm32f1xx_hal_gpio_ex.h
	${CMAKE_CURRENT_LIST_DIR}/Inc/stm32f1xx_hal_hcd.h
	${CMAKE_CURRENT_LIST_DIR}/Inc/stm32f1xx_hal_i2c.h
	${CMAKE_CURRENT_LIST_DIR}/Inc/stm32f1xx_hal_i2s.h
	${CMAKE_CURRENT_LIST_DIR}/Inc/stm32f1xx_hal_irda.h
	${CMAKE_CURRENT_LIST_DIR}/Inc/stm32f1xx_hal_iwdg.h
	${CMAKE_CURRENT_LIST_DIR}/Inc/stm32f1xx_hal_mmc.h
	${CMAKE_CURRENT_LIST_DIR}/Inc/stm32f1xx_hal_nand.h
	${CMAKE_CURRENT_LIST_DIR}/Inc/stm32f1xx_hal_nor.h
	${CMAKE_CURRENT_LIST_DIR}/Inc/stm32f1xx_hal_pccard.h
	${CMAKE_CURRENT_LIST_DIR}/Inc/stm32f1xx_hal_pcd.h
	${CMAKE_CURRENT_LIST_DIR}/Inc/stm32f1xx_hal_pcd_ex.h
	${CMAKE_CURRENT_LIST_DIR}/Inc/stm32f1xx_hal_pwr.h
	${CMAKE_CURRENT_LIST_DIR}/Inc/stm32f1xx_hal_rcc.h
	${CMAKE_CURRENT_LIST_DIR}/Inc/stm32f1xx_hal_rcc_ex.h
	${CMAKE_CURRENT_LIST_DIR}/Inc/stm32f1xx_hal_rtc.h
	${CMAKE_CURRENT_LIST_DIR}/Inc/stm32f1xx_hal_rtc_ex.h
	${CMAKE_CURRENT_LIST_DIR}/Inc/stm32f1xx_hal_sd.h
	${CMAKE_CURRENT_LIST_DIR}/Inc/stm32f1xx_hal_smartcard.h
	${CMAKE_CURRENT_LIST_DIR}/Inc/stm32f1xx_hal_spi.h
	${CMAKE_CURRENT_LIST_DIR}/Inc/stm32f1xx_hal_sram.h
	${CMAKE_CURRENT_LIST_DIR}/Inc/stm32f1xx_hal_tim.h
	${CMAKE_CURRENT_LIST_DIR}/Inc/stm32f1xx_hal_tim_ex.h
	${CMAKE_CURRENT_LIST_DIR}/Inc/stm32f1xx_hal_uart.h
	${CMAKE_CURRENT_LIST_DIR}/Inc/stm32f1xx_hal_usart.h
	${CMAKE_CURRENT_LIST_DIR}/Inc/stm32f1xx_hal_wwdg.h
	${CMAKE_CURRENT_LIST_DIR}/Inc/stm32f1xx_ll_adc.h
	${CMAKE_CURRENT_LIST_DIR}/Inc/stm32f1xx_ll_bus.h
	${CMAKE_CURRENT_LIST_DIR}/Inc/stm32f1xx_ll_cortex.h
	${CMAKE_CURRENT_LIST_DIR}/Inc/stm32f1xx_ll_crc.h
	${CMAKE_CURRENT_LIST_DIR}/Inc/stm32f1xx_ll_dac.h
	${CMAKE_CURRENT_LIST_DIR}/Inc/stm32f1xx_ll_dma.h
	${CMAKE_CURRENT_LIST_DIR}/Inc/stm32f1xx_ll_exti.h
	${CMAKE_CURRENT_LIST_DIR}/Inc/stm32f1xx_ll_fsmc.h
	${CMAKE_CURRENT_LIST_DIR}/Inc/stm32f1xx_ll_gpio.h
	${CMAKE_CURRENT_LIST_DIR}/Inc/stm32f1xx_ll_i2c.h
	${CMAKE_CURRENT_LIST_DIR}/Inc/stm32f1xx_ll_iwdg.h
	${CMAKE_CURRENT_LIST_DIR}/Inc/stm32f1xx_ll_pwr.h
	${CMAKE_CURRENT_LIST_DIR}/Inc/stm32f1xx_ll_rcc.h
	${CMAKE_CURRENT_LIST_DIR}/Inc/stm32f1xx_ll_rtc.h
	${CMAKE_CURRENT_LIST_DIR}/Inc/stm32f1xx_ll_sdmmc.h
	${CMAKE_CURRENT_LIST_DIR}/Inc/stm32f1xx_ll_spi.h
	${CMAKE_CURRENT_LIST_DIR}/Inc/stm32f1xx_ll_system.h
	${CMAKE_CURRENT_LIST_DIR}/Inc/stm32f1xx_ll_tim.h
	${CMAKE_CURRENT_LIST_DIR}/Inc/stm32f1xx_ll_usart.h
	${CMAKE_CURRENT_LIST_DIR}/Inc/stm32f1xx_ll_usb.h
	${CMAKE_CURRENT_LIST_DIR}/Inc/stm32f1xx_ll_utils.h
	${CMAKE_CURRENT_LIST_DIR}/Inc/stm32f1xx_ll_wwdg.h
) # of GROUP_INC_SRC

#SET(GROUP_INC_LEGACY_SRC
#	${CMAKE_CURRENT_LIST_DIR}/Inc/Legacy/stm32_hal_legacy.h
#	${CMAKE_CURRENT_LIST_DIR}/Inc/Legacy/stm32f1xx_hal_can_ex_legacy.h
#	${CMAKE_CURRENT_LIST_DIR}/Inc/Legacy/stm32f1xx_hal_can_legacy.h
#) # of GROUP_INC_LEGACY_SRC

SET(GROUP_SRC_SRC
	${CMAKE_CURRENT_LIST_DIR}/Src/stm32f1xx_hal.c
	${CMAKE_CURRENT_LIST_DIR}/Src/stm32f1xx_hal_adc.c
	${CMAKE_CURRENT_LIST_DIR}/Src/stm32f1xx_hal_adc_ex.c
	${CMAKE_CURRENT_LIST_DIR}/Src/stm32f1xx_hal_can.c
	${CMAKE_CURRENT_LIST_DIR}/Src/stm32f1xx_hal_cec.c
	${CMAKE_CURRENT_LIST_DIR}/Src/stm32f1xx_hal_cortex.c
	${CMAKE_CURRENT_LIST_DIR}/Src/stm32f1xx_hal_crc.c
	${CMAKE_CURRENT_LIST_DIR}/Src/stm32f1xx_hal_dac.c
	${CMAKE_CURRENT_LIST_DIR}/Src/stm32f1xx_hal_dac_ex.c
	${CMAKE_CURRENT_LIST_DIR}/Src/stm32f1xx_hal_dma.c
	${CMAKE_CURRENT_LIST_DIR}/Src/stm32f1xx_hal_eth.c
	${CMAKE_CURRENT_LIST_DIR}/Src/stm32f1xx_hal_exti.c
	${CMAKE_CURRENT_LIST_DIR}/Src/stm32f1xx_hal_flash.c
	${CMAKE_CURRENT_LIST_DIR}/Src/stm32f1xx_hal_flash_ex.c
	${CMAKE_CURRENT_LIST_DIR}/Src/stm32f1xx_hal_gpio.c
	${CMAKE_CURRENT_LIST_DIR}/Src/stm32f1xx_hal_gpio_ex.c
	${CMAKE_CURRENT_LIST_DIR}/Src/stm32f1xx_hal_hcd.c
	${CMAKE_CURRENT_LIST_DIR}/Src/stm32f1xx_hal_i2c.c
	${CMAKE_CURRENT_LIST_DIR}/Src/stm32f1xx_hal_i2s.c
	${CMAKE_CURRENT_LIST_DIR}/Src/stm32f1xx_hal_irda.c
	${CMAKE_CURRENT_LIST_DIR}/Src/stm32f1xx_hal_iwdg.c
	${CMAKE_CURRENT_LIST_DIR}/Src/stm32f1xx_hal_mmc.c
#	${CMAKE_CURRENT_LIST_DIR}/Src/stm32f1xx_hal_msp_template.c
	${CMAKE_CURRENT_LIST_DIR}/Src/stm32f1xx_hal_nand.c
	${CMAKE_CURRENT_LIST_DIR}/Src/stm32f1xx_hal_nor.c
	${CMAKE_CURRENT_LIST_DIR}/Src/stm32f1xx_hal_pccard.c
	${CMAKE_CURRENT_LIST_DIR}/Src/stm32f1xx_hal_pcd.c
	${CMAKE_CURRENT_LIST_DIR}/Src/stm32f1xx_hal_pcd_ex.c
	${CMAKE_CURRENT_LIST_DIR}/Src/stm32f1xx_hal_pwr.c
	${CMAKE_CURRENT_LIST_DIR}/Src/stm32f1xx_hal_rcc.c
	${CMAKE_CURRENT_LIST_DIR}/Src/stm32f1xx_hal_rcc_ex.c
	${CMAKE_CURRENT_LIST_DIR}/Src/stm32f1xx_hal_rtc.c
	${CMAKE_CURRENT_LIST_DIR}/Src/stm32f1xx_hal_rtc_ex.c
	${CMAKE_CURRENT_LIST_DIR}/Src/stm32f1xx_hal_sd.c
	${CMAKE_CURRENT_LIST_DIR}/Src/stm32f1xx_hal_smartcard.c
	${CMAKE_CURRENT_LIST_DIR}/Src/stm32f1xx_hal_spi.c
	${CMAKE_CURRENT_LIST_DIR}/Src/stm32f1xx_hal_sram.c
	${CMAKE_CURRENT_LIST_DIR}/Src/stm32f1xx_hal_tim.c
	${CMAKE_CURRENT_LIST_DIR}/Src/stm32f1xx_hal_tim_ex.c
#	${CMAKE_CURRENT_LIST_DIR}/Src/stm32f1xx_hal_timebase_rtc_alarm_template.c
#	${CMAKE_CURRENT_LIST_DIR}/Src/stm32f1xx_hal_timebase_tim_template.c
	${CMAKE_CURRENT_LIST_DIR}/Src/stm32f1xx_hal_uart.c
	${CMAKE_CURRENT_LIST_DIR}/Src/stm32f1xx_hal_usart.c
	${CMAKE_CURRENT_LIST_DIR}/Src/stm32f1xx_hal_wwdg.c
	${CMAKE_CURRENT_LIST_DIR}/Src/stm32f1xx_ll_adc.c
	${CMAKE_CURRENT_LIST_DIR}/Src/stm32f1xx_ll_crc.c
	${CMAKE_CURRENT_LIST_DIR}/Src/stm32f1xx_ll_dac.c
	${CMAKE_CURRENT_LIST_DIR}/Src/stm32f1xx_ll_dma.c
	${CMAKE_CURRENT_LIST_DIR}/Src/stm32f1xx_ll_exti.c
	${CMAKE_CURRENT_LIST_DIR}/Src/stm32f1xx_ll_fsmc.c
	${CMAKE_CURRENT_LIST_DIR}/Src/stm32f1xx_ll_gpio.c
	${CMAKE_CURRENT_LIST_DIR}/Src/stm32f1xx_ll_i2c.c
	${CMAKE_CURRENT_LIST_DIR}/Src/stm32f1xx_ll_pwr.c
	${CMAKE_CURRENT_LIST_DIR}/Src/stm32f1xx_ll_rcc.c
	${CMAKE_CURRENT_LIST_DIR}/Src/stm32f1xx_ll_rtc.c
	${CMAKE_CURRENT_LIST_DIR}/Src/stm32f1xx_ll_sdmmc.c
	${CMAKE_CURRENT_LIST_DIR}/Src/stm32f1xx_ll_spi.c
	${CMAKE_CURRENT_LIST_DIR}/Src/stm32f1xx_ll_tim.c
	${CMAKE_CURRENT_LIST_DIR}/Src/stm32f1xx_ll_usart.c
	${CMAKE_CURRENT_LIST_DIR}/Src/stm32f1xx_ll_usb.c
	${CMAKE_CURRENT_LIST_DIR}/Src/stm32f1xx_ll_utils.c
) # of GROUP_SRC_SRC


SET(INCLUDE_DIRS_MCU_DRIVER_LIST
	${CMAKE_CURRENT_LIST_DIR}/Inc
#	${CMAKE_CURRENT_LIST_DIR}/Inc/Legacy
	${CMAKE_CURRENT_LIST_DIR}/Src
) # of INCLUDE_DIRS_MCU_DRIVER_LIST

SET(MCU_DRIVER_SOURCES_LIST
	${GROUP_INC_SRC}
#	${GROUP_INC_LEGACY_SRC}
	${GROUP_SRC_SRC}
) # of MCU_DRIVER_SOURCES_LIST
