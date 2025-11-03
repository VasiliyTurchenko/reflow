# ------- TARGET STARTS HERE -------
set(MCU stm32f103c8)

set(TARGET_NAME reflow_${MCU})
set(EXEC_NAME ${TARGET_NAME})
add_executable(${EXEC_NAME})

# set(DEBUG_SINK_SRC "debug_sink.c")

target_compile_definitions(
    ${EXEC_NAME}
    PRIVATE USE_FULL_LL_DRIVER
            USE_HAL_DRIVER
            STM32F103CXX
            STM32F103C8T6
            STM32F103xB
            USE_FRAMEBUFFER
            USE_FULL_ASSERT
            NO_LAN
            TARGET_NAME=${TARGET_NAME})

include(${CMAKE_CURRENT_LIST_DIR}/../../../../src/sources_list.cmake)
target_include_directories(${EXEC_NAME} PRIVATE ${INCLUDE_DIRS_SRC_LIST})

include(${CMAKE_CURRENT_LIST_DIR}/../../../../third_party/sources_list.cmake)
target_include_directories(${EXEC_NAME} PRIVATE ${INCLUDE_DIRS_THIRD_PARTY_LIST})

include(${CMAKE_CURRENT_LIST_DIR}/../../../../target/mcu/st/drivers/CMSIS/sources_list.cmake)
target_include_directories(${EXEC_NAME} PRIVATE ${INCLUDE_DIRS_CMSIS_LIST})

include(
    ${CMAKE_CURRENT_LIST_DIR}/../../../../target/mcu/st/drivers/STM32F1xx_HAL_Driver/sources_list.cmake
)
target_include_directories(${EXEC_NAME} PRIVATE ${INCLUDE_DIRS_STM32F1XX_HAL_DRIVER_LIST})

include(sources_list.cmake)
target_include_directories(${EXEC_NAME} PRIVATE ${INCLUDE_DIRS_STM32F103_LIST})

set(COMPILE_FLAGS -mcpu=cortex-m3 -mthumb -mfloat-abi=soft -mno-thumb-interwork -v -fmessage-length=0 -fdata-sections -ffunction-sections --specs=nano.specs)

# The path to the asm startup file(s)
set(STARTUP_CODE_DIR ${CMAKE_CURRENT_LIST_DIR}/startup/)
message("STARTUP_CODE_DIR = " ${STARTUP_CODE_DIR})

# The path to the linker script(s)
set(LINKER_SCRIPTS_DIR ${CMAKE_CURRENT_LIST_DIR}/ld/)
message("LINKER_SCRIPTS_DIR = " ${LINKER_SCRIPTS_DIR})

set(STARTUP_CODE_SOURCE ${STARTUP_CODE_DIR}/startup_stm32f103xb.s)
set(LDSCRIPT ${LINKER_SCRIPTS_DIR}/STM32F103C8Tx_FLASH.ld)
# set(LDSCRIPT ${LINKER_SCRIPTS_DIR}/STM32F103XX_FLASH.ld)

# ##################################################################################################
# ##################################################################################################
# Let's gather all together
# ##################################################################################################
# ##################################################################################################

set(${EXEC_NAME}_LIST_OF_SOURCES
    ${SRC_SOURCES_LIST} ${THIRD_PARTY_SOURCES_LIST} ${CMSIS_SOURCES_LIST}
    ${STM32F1XX_HAL_DRIVER_SOURCES_LIST} ${STM32F103_SOURCES_LIST})

target_sources(${EXEC_NAME} PRIVATE ${${EXEC_NAME}_LIST_OF_SOURCES} ${STARTUP_CODE_SOURCE})

target_compile_options(${EXEC_NAME} PRIVATE ${COMPILE_FLAGS})

target_link_options(
    ${EXEC_NAME}
    BEFORE
    PRIVATE
    "-Wl,-Map=${EXEC_NAME}.map"
    "-Wl,-T${LDSCRIPT}"
    "-Wl,--gc-sections"
    "-Wl,--verbose"
    "-Wl,-V"
    "-Wl,--cref"
    ${COMPILE_FLAGS})

target_link_libraries(
    ${EXEC_NAME} c # c runtime
    m # math
    nosys # for non-os
)

# we do not need bin and hex
stm32_add_hex_bin_targets(${EXEC_NAME})
stm32_print_size_of_targets(${EXEC_NAME})
