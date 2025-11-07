message(STATUS "\nProcessing ${CMAKE_CURRENT_LIST_FILE}...")

set(MCU_prefix ${CMAKE_CURRENT_LIST_DIR}/../../targets/mcu)

function(GET_MCU_CMAKELISTS mcu_string)
    string(TOLOWER "${mcu_string}" mcu_string_lower)
    # message("mcu_string = ${mcu_string}") message("mcu_string_lower = ${mcu_string_lower}")

    set(MCUS_MAP
        "stm32f3\\;st/drivers/CMSIS\\;st/drivers/STM32F3xx_HAL_Driver\\;-mcpu=cortex-m3 -mthumb -mfloat-abi=soft -mno-thumb-interwork\\;USE_FULL_LL_DRIVER USE_HAL_DRIVER STM32F303CXX STM32F303C8T6 STM32F303xB"
        "stm32f4\\;st/drivers/CMSIS\\;st/drivers/STM32F4xx_HAL_Driver\\;-mcpu=cortex-m3 -mthumb -mfloat-abi=soft -mno-thumb-interwork\\;USE_FULL_LL_DRIVER USE_HAL_DRIVER STM32F303CXX STM32F303C8T6 STM32F303xB"
        "stm32f7\\;st/drivers/CMSIS\\;st/drivers/STM32F7xx_HAL_Driver\\;-mcpu=cortex-m3 -mthumb -mfloat-abi=soft -mno-thumb-interwork\\;USE_FULL_LL_DRIVER USE_HAL_DRIVER STM32F303CXX STM32F303C8T6 STM32F303xB"
        "stm32f1\\;st/drivers/CMSIS\\;st/drivers/STM32F1xx_HAL_Driver\\;-mcpu=cortex-m3 -mthumb -mfloat-abi=soft -mno-thumb-interwork --specs=nano.specs\\;USE_FULL_LL_DRIVER USE_HAL_DRIVER STM32F103CXX STM32F103C8T6 STM32F103xB"
    )

    set(found FALSE)
    set(matches "")

    set(map_key "")
    set(cmsis "")
    set(hal_ll "")
    set(compile_flags "")
    set(mcu_defs "")
    set(pos -1)

    foreach(item IN LISTS MCUS_MAP)
        # message("map item: ${item}")
        string(REPLACE "\\;" ";" map_item "${item}")
        # message("map item 2: ${map_item}")

        list(GET map_item 0 map_key)

        string(FIND "${mcu_string_lower}" "${map_key}" POS)
        if(NOT POS EQUAL -1)

            list(GET map_item 1 cmsis)
            list(GET map_item 2 hal_ll)
            list(GET map_item 3 compile_flags)
            list(GET map_item 4 mcu_defs)

            # message("map_key: ${map_key}") message("cmsis: ${cmsis}") message("hal_ll: ${hal_ll}")
            # message("compile_flags: ${compile_flags}")

            set(found TRUE)
            break()

        else()
            set(map_key "")
            set(cmsis "")
            set(hal_ll "")
            set(compile_flags "")
            set(mcu_defs "")
        endif()

    endforeach()

    if(NOT found)
        message(FATAL_ERROR "No MCU found!")
    endif()

    set(cmsis_nn ${MCU_prefix}/${cmsis})
    set(hal_ll_nn ${MCU_prefix}/${hal_ll})
    cmake_path(NATIVE_PATH cmsis_nn NORMALIZE CMSIS_FOLDER_N)
    cmake_path(NATIVE_PATH hal_ll_nn NORMALIZE HAL_LL_FOLDER_N)

    set(CMSIS_FOLDER
        ${CMSIS_FOLDER_N}
        PARENT_SCOPE)
    set(HAL_LL_FOLDER
        ${HAL_LL_FOLDER_N}
        PARENT_SCOPE)

    string(REGEX REPLACE " +" ";" compile_flags_list "${compile_flags}")
    set(MCU_COMPILE_FLAGS
        ${compile_flags_list}
        PARENT_SCOPE)

    string(REGEX REPLACE " +" ";" mcu_defs_list "${mcu_defs}")
    set(MCU_COMPILE_DEFS
        ${mcu_defs_list}
        PARENT_SCOPE)

endfunction()
