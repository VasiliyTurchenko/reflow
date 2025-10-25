cmake_minimum_required(VERSION 3.15)

project(reflow
	VERSION 1.0
	DESCRIPTION "Reflow oven controller"
	LANGUAGES C ASM)
enable_language(ASM)

if (CMAKE_SYSTEM_NAME MATCHES Generic)

## directories

# PC-Linf function
#include(${TOOLCHAIN_PREFIX}/pc-lint.cmake)


set(STARTUP_CODE_DIR ${CMAKE_CURRENT_SOURCE_DIR})
message("STARTUP_CODE_DIR = " ${STARTUP_CODE_DIR})

set(LINKER_SCRIPTS_DIR ${CMAKE_CURRENT_SOURCE_DIR})

set(TARGET_MCU "stm32f1")

IF(TARGET_MCU MATCHES "stm32f1")

        message	(STATUS "TARGET_MCU: " ${TARGET_MCU})
	set	(D_MCU "-DSTM32F103xB")
	set	(STARTUP_CODE_SOURCE ${STARTUP_CODE_DIR}/startup_stm32f103xb.s)
	SET	(LDSCRIPT	${LINKER_SCRIPTS_DIR}/STM32F103C8Tx_FLASH.ld)

	SET	(COMPILE_FLAGS -mcpu=cortex-m3 -mthumb -mfloat-abi=soft  -mno-thumb-interwork -v)
else()

	message(FATAL_ERROR "TARGET_MCU NOT DEFINED!")

endif()

message(STATUS "LDSCRIPT = " ${LDSCRIPT})

### generate map file
FUNCTION(ADD_MAPFILE TARGET)
    IF(EXECUTABLE_OUTPUT_PATH)
	  SET(MAPFILENAME ${EXECUTABLE_OUTPUT_PATH}/${TARGET}.map)
	ELSE()
	  SET(MAPFILENAME ${TARGET}.map)
	ENDIF()
	    message("MAPFILENAME = " ${MAPFILENAME})
ENDFUNCTION()

message(VERBOSE "TARGET NAME = " ${MASTER_BOARD})

message ("CMAKE_TOOLCHAIN_FILE = " ${CMAKE_TOOLCHAIN_FILE})
message("CMAKE_CURRENT_SOURCE_DIR = " ${CMAKE_CURRENT_SOURCE_DIR})

include(${CMAKE_CURRENT_SOURCE_DIR}/sources_list.cmake)


# MCU independent definitions
add_definitions(-DUSE_HAL_DRIVER)
add_definitions(-DDEBUG)

# for desktop&mcu code
add_definitions(-DMCU_TARGET)

add_definitions(-DNO_LAN)
add_definitions(-DUSE_FRAMEBUFFER)

if(CMAKE_BUILD_TYPE MATCHES "Debug")
	message( STATUS "Compiling with debug info")
	set(CMAKE_CXX_DEBUG_FLAGS "-gdwarf-2 -Og")
else()
	set(CMAKE_CXX_DEBUG_FLAGS "-O3")
endif()

##assembler flags
set(CMAKE_ASM_FLAGS "${CMAKE_ASM_FLAGS} -Wall -fdata-sections -ffunction-sections")

set(GCC_COVERAGE_COMPILE_FLAGS "${GCC_COVERAGE_COMPILE_FLAGS} -Wall -fmessage-length=0 -fdata-sections -ffunction-sections --specs=nano.specs")

set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} ${GCC_COVERAGE_COMPILE_FLAGS} ${CMAKE_CXX_DEBUG_FLAGS}")
set(CMAKE_C_FLAGS  "${CMAKE_C_FLAGS} ${GCC_COVERAGE_COMPILE_FLAGS} ${CMAKE_CXX_DEBUG_FLAGS}")

message(STATUS "100: CMAKE_CXX_FLAGS = " ${CMAKE_CXX_FLAGS})
message(STATUS "101: CMAKE_C_FLAGS = " ${CMAKE_C_FLAGS})


set (BOARD ${PROJECT_NAME}_${TARGET_MCU})

add_executable			(
	                        ${BOARD}
				${LIST_OF_SOURCES}
#				${MCU_SOURCES_GROUP}
                                ${STARTUP_CODE_SOURCE}
				)

target_compile_definitions	(
	                                ${BOARD} PRIVATE
					-DMASTERBOARD
					${D_MCU}
				)


target_compile_options		(
	                                ${BOARD} PRIVATE
					${COMPILE_FLAGS}
				)

target_link_options		(
	                                ${BOARD} BEFORE PRIVATE
					"-Wl,-Map=${BOARD}.map"
					"-Wl,-T${LDSCRIPT}"
					"-Wl,--gc-sections"
					"-Wl,--verbose"
					"-Wl,-V"
					"-Wl,--cref"
					${COMPILE_FLAGS}
				    )
target_link_libraries		(
	                                ${BOARD}
					c
					m
					nosys
				    )

STM32_ADD_HEX_BIN_TARGETS(${BOARD})
STM32_PRINT_SIZE_OF_TARGETS(${BOARD})
#STM32_ADD_DUMP_TARGET(${BOARD})

# добавляем таргет PC-Lint
if(COMMAND add_pc_lint)
# establish list of compile defifntiond
        set (comp_defs_list)
	list(APPEND comp_defs_list ${D_MCU})
	list(APPEND comp_defs_list "-DMASTERBOARD")

#f antastic loop
        foreach(s IN LISTS LIST_OF_SOURCES)

		set(t_name)
		string(REPLACE "/" "_" t_name ${s})

#		message("t_name = " ${t_name})

                add_pc_lint(
# compile defines
                                "${comp_defs_list}"
# target
                                ${t_name}
				${s}
			    )
	endforeach()
endif(COMMAND add_pc_lint)

message(STATUS "184: CMAKE_CXX_FLAGS = " ${CMAKE_CXX_FLAGS})
message(STATUS "185: CMAKE_C_FLAGS = " ${CMAKE_C_FLAGS})


#ADD_MAPFILE(${BOARD})
#message("MAPFILENAME = " ${MAPFILENAME})

else()
MESSAGE("Non-MCU target detected")
add_executable( i386 Core386/Src/main.c )


endif()

MESSAGE("*********************")
message("CMAKE_SYSTEM_NAME = " ${CMAKE_SYSTEM_NAME} "      CMAKE_HOST_SYSTEM_NAME = " ${CMAKE_HOST_SYSTEM_NAME})
message("CMAKE_SYSTEM = " ${CMAKE_SYSTEM} "      CMAKE_HOST_SYSTEM = " ${CMAKE_HOST_SYSTEM})
MESSAGE("CMAKE_C_COMPILER = " ${CMAKE_C_COMPILER})
MESSAGE("CMAKE_BUILD_TYPE = " ${CMAKE_BUILD_TYPE})
get_cmake_property(EL ENABLED_LANGUAGES)
message ("ENABLED_LANGUAGES = " ${EL})
get_cmake_property(LL LINKER_LANGUAGE)
message ("LINKER_LANGUAGE = " ${LL})
MESSAGE("*********************")

#foreach(e IN LISTS CMAKE_C_IMPLICIT_INCLUDE_DIRECTORIES)
#	message("CMAKE_C_IMPLICIT_INCLUDE_DIRECTORIES = " ${e})
#endforeach(e)

#foreach(e IN LISTS CMAKE_CXX_IMPLICIT_INCLUDE_DIRECTORIES)
#	message("CMAKE_CXX_IMPLICIT_INCLUDE_DIRECTORIES = " ${e})
#endforeach(e)

#
#function (get_compiler_preprocessor_defines target_comp_defs target)
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

set(target "${PROJECT_NAME}_preprocessor_defines")
        # create empty c file in temp dir
	message("CMAKE_CURRENT_BINARY_DIR = " ${CMAKE_CURRENT_BINARY_DIR})
	set(test_c_file "${CMAKE_CURRENT_BINARY_DIR}/${target}.c")
	message("test_c_file = " ${test_c_file})
	file(WRITE ${test_c_file} "\n")

	set(try_compile_result)
	set(output_text)
	set(copy_file "${CMAKE_CURRENT_BINARY_DIR}/${target}.object")
	set(copy_file_error "${CMAKE_CURRENT_BINARY_DIR}/${target}.stderr")

	#store current variable CMAKE_C_FLAGS
	set(stored_cmake_c_flags ${CMAKE_C_FLAGS})
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -dM -E")

	try_compile(
		        try_compile_result
			"${CMAKE_CURRENT_BINARY_DIR}/preprocessor_defs"
			${test_c_file}
			OUTPUT_VARIABLE output_text
			COPY_FILE ${copy_file}
			COPY_FILE_ERROR ${copy_file_error}
	)
        message("try_compile_result = " ${try_compile_result})
	message("output_text = " ${output_text})
	file(REMOVE ${test_c_file})

	set(CMAKE_C_FLAGS ${stored_cmake_c_flags})
