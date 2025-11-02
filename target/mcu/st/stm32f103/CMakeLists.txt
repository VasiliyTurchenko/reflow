# ------- TARGET STARTS HERE -------
set(MCU stm32f103c8)

set(TARGET_NAME reflow_${MCU})
set(EXEC_NAME ${TARGET_NAME})
add_executable(${EXEC_NAME})

set(DEBUG_SINK_SRC "debug_sink.c")

target_compile_definitions(
    ${EXEC_NAME} PRIVATE USE_FULL_LL_DRIVER USE_HAL_DRIVER STM32F103CXX STM32F103C8T6
                         USE_FULL_ASSERT TARGET_NAME=${TARGET_NAME})

include(${CMAKE_CURRENT_LIST_DIR}/../../..)
target_include_directories(${EXEC_NAME} PRIVATE ${})

set(COMPILE_FLAGS -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -mno-thumb-interwork
                  -v)

# The path to the asm startup file(s)
set(STARTUP_CODE_DIR ${CMAKE_CURRENT_LIST_DIR}/startup/)
message("STARTUP_CODE_DIR = " ${STARTUP_CODE_DIR})

# The path to the linker script(s)
set(LINKER_SCRIPTS_DIR ${CMAKE_CURRENT_LIST_DIR}/ld/)
message("LINKER_SCRIPTS_DIR = " ${LINKER_SCRIPTS_DIR})

set(STARTUP_CODE_SOURCE ${STARTUP_CODE_DIR}/startup_stm32f103xb.s)
set(LDSCRIPT ${LINKER_SCRIPTS_DIR}/STM32F103C8Tx_FLASH.ld)

set(GROUP_SRC ${CMAKE_CURRENT_LIST_DIR}/src/main.c ${CMAKE_CURRENT_LIST_DIR}/src/${DEBUG_SINK_SRC})

set(GROUP_SRC_COMP_DEBUG
    ${CMAKE_CURRENT_LIST_DIR}/../../../components/debug/logging.c
    ${CMAKE_CURRENT_LIST_DIR}/../../../components/debug/my_comm.c
    ${CMAKE_CURRENT_LIST_DIR}/../../../components/debug/myCRT.c
    ${CMAKE_CURRENT_LIST_DIR}/../../../components/debug/hex_gen.c
    ${CMAKE_CURRENT_LIST_DIR}/../../../components/debug/xprintf.c)

set(GROUP_SRC_DRIVERS_HAL_DRIVER
    ${CMAKE_CURRENT_LIST_DIR}/../../../stm/STM32MP1xx_HAL_Driver/Src/stm32mp1xx_hal.c)

# ##################################################################################################
# ##################################################################################################
# Let's gather all together
# ##################################################################################################
# ##################################################################################################

set(${EXEC_NAME}_LIST_OF_SOURCES ${GROUP_SRC} ${GROUP_CMSIS})

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
