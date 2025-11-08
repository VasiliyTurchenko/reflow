# ##################################################################################################
#
# Application
#
# ##################################################################################################
message(STATUS "\nProcessing ${CMAKE_CURRENT_LIST_FILE}...")

# hardware-independent sources
include(sources_list.cmake)

# hardware-independent 3rd party sources
include(../third_party/sources_list.cmake)

message(STATUS "mcu: ${mcu}")
message(STATUS "board: ${board}")

get_mcu_cmakelists("${mcu}")

message(STATUS "CMSIS_FOLDER: ${CMSIS_FOLDER}")
message(STATUS "HAL_LL_FOLDER: ${HAL_LL_FOLDER}")
message(STATUS "MCU_COMPILE_FLAGS: ${MCU_COMPILE_FLAGS}")
message(STATUS "MCU_COMPILE_DEFS: ${MCU_COMPILE_DEFS}")

add_subdirectory(${HAL_LL_FOLDER} ${CMAKE_BINARY_DIR}/libhal_build)

include(${CMSIS_FOLDER}/sources_list.cmake)

# The path to the asm startup file(s)
set(STARTUP_CODE_DIR ${HAL_LL_FOLDER}/../startup/)
message("STARTUP_CODE_DIR = " ${STARTUP_CODE_DIR})

# The path to the linker script(s)
set(LINKER_SCRIPTS_DIR ${HAL_LL_FOLDER}/../ldfiles/)
message("LINKER_SCRIPTS_DIR = " ${LINKER_SCRIPTS_DIR})

# board - dependent lists
include(${HAL_LL_FOLDER}/../../${mcu}/sources_list.cmake)

# ------- TARGET STARTS HERE -------

set(TARGET_NAME reflow_${mcu})
set(EXEC_NAME ${TARGET_NAME})
add_executable(${EXEC_NAME})

# ---- MCU-agnostic definitions ----
set(COMMON_DEFS
    USE_FRAMEBUFFER
    USE_FreeRTOS
    WITH_RTOS
    USE_FULL_ASSERT
    NO_LAN
    TARGET_NAME=${TARGET_NAME}
    DEBUG_USART=1)

target_compile_definitions(${EXEC_NAME} PRIVATE ${COMMON_DEFS} ${MCU_COMPILE_DEFS})

target_include_directories(${EXEC_NAME} PRIVATE ${INCLUDE_DIRS_COMPILER_LIST})
target_include_directories(${EXEC_NAME} PRIVATE ${INCLUDE_DIRS_SRC_LIST})
target_include_directories(${EXEC_NAME} PRIVATE ${INCLUDE_DIRS_THIRD_PARTY_LIST})
target_include_directories(${EXEC_NAME} PRIVATE ${INCLUDE_DIRS_CMSIS_LIST})

target_include_directories(${EXEC_NAME} PRIVATE ${INCLUDE_DIRS_STM32F103_LIST})

set(COMPILE_FLAGS ${MCU_COMPILE_FLAGS} -v -fmessage-length=0 -fdata-sections -ffunction-sections)
# --specs=nosys.specs)

set(STARTUP_CODE_SOURCE ${STARTUP_CODE_DIR}/startup_stm32f103xb.s)
set(LDSCRIPT ${LINKER_SCRIPTS_DIR}/STM32F103C8Tx_FLASH.ld)
# set(LDSCRIPT ${LINKER_SCRIPTS_DIR}/STM32F103XX_FLASH.ld)

# ##################################################################################################
# ##################################################################################################
# Let's gather all together
# ##################################################################################################
# ##################################################################################################

set(${EXEC_NAME}_LIST_OF_SOURCES
    ${SRC_SOURCES_LIST} ${THIRD_PARTY_SOURCES_LIST} ${CMSIS_SOURCES_LIST} ${STM32F103_SOURCES_LIST}
    ${COMPILER_SOURCES_LIST})

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
    ${EXEC_NAME}
    c # c runtime
    m # math
    nosys # for non-os
    hal)

# we do not need bin and hex
stm32_add_hex_bin_targets(${EXEC_NAME})
stm32_print_size_of_targets(${EXEC_NAME})
