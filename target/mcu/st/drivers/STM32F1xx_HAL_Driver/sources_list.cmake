SET(GROUP_INC_SRC
	${CMAKE_CURRENT_LIST_DIR}/Inc/stm32f1xx_hal.h
	${CMAKE_CURRENT_LIST_DIR}/Inc/stm32f1xx_hal_adc.h
	${CMAKE_CURRENT_LIST_DIR}/Inc/stm32f1xx_hal_adc_ex.h
	${CMAKE_CURRENT_LIST_DIR}/Inc/stm32f1xx_hal_cortex.h
	${CMAKE_CURRENT_LIST_DIR}/Inc/stm32f1xx_hal_crc.h
	${CMAKE_CURRENT_LIST_DIR}/Inc/stm32f1xx_hal_def.h
	${CMAKE_CURRENT_LIST_DIR}/Inc/stm32f1xx_hal_dma.h
	${CMAKE_CURRENT_LIST_DIR}/Inc/stm32f1xx_hal_dma_ex.h
	${CMAKE_CURRENT_LIST_DIR}/Inc/stm32f1xx_hal_flash.h
	${CMAKE_CURRENT_LIST_DIR}/Inc/stm32f1xx_hal_flash_ex.h
	${CMAKE_CURRENT_LIST_DIR}/Inc/stm32f1xx_hal_gpio.h
	${CMAKE_CURRENT_LIST_DIR}/Inc/stm32f1xx_hal_gpio_ex.h
	${CMAKE_CURRENT_LIST_DIR}/Inc/stm32f1xx_hal_i2c.h
	${CMAKE_CURRENT_LIST_DIR}/Inc/stm32f1xx_hal_iwdg.h
	${CMAKE_CURRENT_LIST_DIR}/Inc/stm32f1xx_hal_pwr.h
	${CMAKE_CURRENT_LIST_DIR}/Inc/stm32f1xx_hal_rcc.h
	${CMAKE_CURRENT_LIST_DIR}/Inc/stm32f1xx_hal_rcc_ex.h
	${CMAKE_CURRENT_LIST_DIR}/Inc/stm32f1xx_hal_rtc.h
	${CMAKE_CURRENT_LIST_DIR}/Inc/stm32f1xx_hal_rtc_ex.h
	${CMAKE_CURRENT_LIST_DIR}/Inc/stm32f1xx_hal_spi.h
	${CMAKE_CURRENT_LIST_DIR}/Inc/stm32f1xx_hal_tim.h
	${CMAKE_CURRENT_LIST_DIR}/Inc/stm32f1xx_hal_tim_ex.h
	${CMAKE_CURRENT_LIST_DIR}/Inc/stm32f1xx_hal_uart.h
	${CMAKE_CURRENT_LIST_DIR}/Inc/stm32f1xx_hal_wwdg.h
) # of GROUP_INC_SRC

SET(GROUP_INC_LEGACY_SRC
	${CMAKE_CURRENT_LIST_DIR}/Inc/Legacy/stm32_hal_legacy.h
) # of GROUP_INC_LEGACY_SRC

SET(GROUP_SRC_SRC
	${CMAKE_CURRENT_LIST_DIR}/Src/stm32f1xx_hal.c
	${CMAKE_CURRENT_LIST_DIR}/Src/stm32f1xx_hal_adc.c
	${CMAKE_CURRENT_LIST_DIR}/Src/stm32f1xx_hal_adc_ex.c
	${CMAKE_CURRENT_LIST_DIR}/Src/stm32f1xx_hal_cortex.c
	${CMAKE_CURRENT_LIST_DIR}/Src/stm32f1xx_hal_crc.c
	${CMAKE_CURRENT_LIST_DIR}/Src/stm32f1xx_hal_dma.c
	${CMAKE_CURRENT_LIST_DIR}/Src/stm32f1xx_hal_flash.c
	${CMAKE_CURRENT_LIST_DIR}/Src/stm32f1xx_hal_flash_ex.c
	${CMAKE_CURRENT_LIST_DIR}/Src/stm32f1xx_hal_gpio.c
	${CMAKE_CURRENT_LIST_DIR}/Src/stm32f1xx_hal_gpio_ex.c
	${CMAKE_CURRENT_LIST_DIR}/Src/stm32f1xx_hal_i2c.c
	${CMAKE_CURRENT_LIST_DIR}/Src/stm32f1xx_hal_iwdg.c
	${CMAKE_CURRENT_LIST_DIR}/Src/stm32f1xx_hal_pwr.c
	${CMAKE_CURRENT_LIST_DIR}/Src/stm32f1xx_hal_rcc.c
	${CMAKE_CURRENT_LIST_DIR}/Src/stm32f1xx_hal_rcc_ex.c
	${CMAKE_CURRENT_LIST_DIR}/Src/stm32f1xx_hal_rtc.c
	${CMAKE_CURRENT_LIST_DIR}/Src/stm32f1xx_hal_rtc_ex.c
	${CMAKE_CURRENT_LIST_DIR}/Src/stm32f1xx_hal_spi.c
	${CMAKE_CURRENT_LIST_DIR}/Src/stm32f1xx_hal_spi_ex.c
	${CMAKE_CURRENT_LIST_DIR}/Src/stm32f1xx_hal_tim.c
	${CMAKE_CURRENT_LIST_DIR}/Src/stm32f1xx_hal_tim_ex.c
	${CMAKE_CURRENT_LIST_DIR}/Src/stm32f1xx_hal_uart.c
	${CMAKE_CURRENT_LIST_DIR}/Src/stm32f1xx_hal_wwdg.c
) # of GROUP_SRC_SRC


SET(INCLUDE_DIRS_STM32F1XX_HAL_DRIVER_LIST
	${CMAKE_CURRENT_LIST_DIR}/Inc
	${CMAKE_CURRENT_LIST_DIR}/Inc/Legacy
	${CMAKE_CURRENT_LIST_DIR}/Src
) # of INCLUDE_DIRS_STM32F1XX_HAL_DRIVER_LIST

SET(STM32F1XX_HAL_DRIVER_SOURCES_LIST
	${GROUP_INC_SRC}
	${GROUP_INC_LEGACY_SRC}
	${GROUP_SRC_SRC}
) # of STM32F1XX_HAL_DRIVER_SOURCES_LIST