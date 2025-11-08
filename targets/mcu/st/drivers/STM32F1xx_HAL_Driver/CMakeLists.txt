# ##################################################################################################
#
# HAL/LL library
#
# ##################################################################################################
message(STATUS "\nProcessing ${CMAKE_CURRENT_LIST_FILE}...")

# hardware-independent sources
include(sources_list.cmake)

message(STATUS "mcu: ${mcu}")
message(STATUS "board: ${board}")

get_mcu_cmakelists("${mcu}")

message(STATUS "MCU_COMPILE_FLAGS: ${MCU_COMPILE_FLAGS}")
message(STATUS "MCU_COMPILE_DEFS: ${MCU_COMPILE_DEFS}")
message(STATUS "CMSIS_FOLDER: ${CMSIS_FOLDER}")

# board - dependent lists
include(${HAL_LL_FOLDER}/../../${mcu}/sources_list.cmake)

include(${CMSIS_FOLDER}/sources_list.cmake)

set(COMPILE_FLAGS ${MCU_COMPILE_FLAGS} -v -fmessage-length=0 -fdata-sections -ffunction-sections)

set(COMMON_DEFS USE_FULL_ASSERT)

add_library(hal STATIC)

message("INCLUDE_DIRS_CMSIS_LIST = ${INCLUDE_DIRS_CMSIS_LIST}")

target_sources(hal PRIVATE ${MCU_DRIVER_SOURCES_LIST})
target_include_directories(hal PRIVATE ${INCLUDE_DIRS_CMSIS_LIST})
target_include_directories(hal PUBLIC ${INCLUDE_DIRS_MCU_DRIVER_LIST})
target_include_directories(hal INTERFACE ${INCLUDE_DIRS_MCU_DRIVER_LIST})
target_include_directories(hal PRIVATE ${INCLUDE_DIRS_STM32F103_LIST})
target_compile_options(hal PRIVATE ${COMPILE_FLAGS})
target_compile_definitions(hal PRIVATE USE_FULL_ASSERT ${MCU_COMPILE_DEFS})
