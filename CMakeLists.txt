# TOP LEVEL CMAKEFILE #######

cmake_minimum_required(VERSION 3.20)
set(CMAKE_VERBOSE_MAKEFILE)
set(CMAKE_C_STANDARD 17)
# set(CMAKE_CXX_STANDARD 20)

project(REFLOW LANGUAGES C CXX ASM)

message(STATUS "\nProcessing ${CMAKE_CURRENT_LIST_FILE}...")
message(STATUS "CMAKE_SYSROOT =  ${CMAKE_SYSROOT}")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -v")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -v")

include(CheckCCompilerFlag)
option(ENABLE_CUSTOM_COMPILER_FLAGS "Enables custom compiler flags" ON)
if(ENABLE_CUSTOM_COMPILER_FLAGS)
    if(("${CMAKE_C_COMPILER_ID}" STREQUAL "Clang") OR ("${CMAKE_C_COMPILER_ID}" STREQUAL "GNU"))
        list(
            APPEND
            custom_compiler_flags_c
            # -std=c89
            -pedantic
            -Wall
            -Wextra
            # -Werror
            -Wstrict-prototypes
            -Wwrite-strings
            -Wshadow
            -Winit-self
            -Wcast-align
            -Wformat=2
            -Wmissing-prototypes
            -Wstrict-overflow=2
            -Wcast-qual
            -Wundef
            -Wswitch-default
            -Wconversion
            -Wc++-compat
            # -fstack-protector-strong
            -Wcomma
            -Wdouble-promotion
            -Wparentheses
            -Wformat-overflow
            -Wunused-macros
            -Wmissing-variable-declarations
            -Wused-but-marked-unused
            -Wswitch-enum)
        list(
            APPEND
            custom_compiler_flags_cxx
            # -std=c89
            -pedantic
            -Wall
            -Wextra
            # -Werror
            -Wwrite-strings
            -Wshadow
            -Winit-self
            -Wcast-align
            -Wformat=2
            -Wstrict-overflow=2
            -Wcast-qual
            -Wundef
            -Wswitch-default
            -Wconversion
            -fstack-protector-strong
            -Wcomma
            -Wdouble-promotion
            -Wparentheses
            -Wformat-overflow
            -Wunused-macros
            -Wmissing-variable-declarations
            -Wused-but-marked-unused
            -Wswitch-enum)

        # elseif("${CMAKE_C_COMPILER_ID}" STREQUAL "MSVC") # Disable warning c4001 - nonstandard
        # extension 'single line comment' was used # Define _CRT_SECURE_NO_WARNINGS to disable
        # deprecation warnings for "insecure" C library functions list(APPEND custom_compiler_flags
        # /GS /Za /sdl /W4 /wd4001 /D_CRT_SECURE_NO_WARNINGS )
    endif()
endif()

option(ENABLE_SANITIZERS "Enables AddressSanitizer and UndefinedBehaviorSanitizer." OFF)
if(ENABLE_SANITIZERS)
    list(
        APPEND
        custom_compiler_flags_c
        -fno-omit-frame-pointer
        -fsanitize=address
        -fsanitize=undefined
        -fsanitize=float-cast-overflow
        -fsanitize-address-use-after-scope
        -fsanitize=integer
        -01
        -fno-sanitize-recover)
endif()

# apply custom compiler flags
foreach(compiler_flag ${custom_compiler_flags_c})
    # remove problematic characters
    string(REGEX REPLACE "[^a-zA-Z0-9]" "" current_variable ${compiler_flag})

    check_c_compiler_flag(${compiler_flag} "FLAG_SUPPORTED_${current_variable}")
    if(FLAG_SUPPORTED_${current_variable})
        list(APPEND supported_compiler_flags)
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${compiler_flag}")
    endif()
endforeach()

# apply custom compiler flags
foreach(compiler_flag ${custom_compiler_flags_cxx})
    # remove problematic characters
    string(REGEX REPLACE "[^a-zA-Z0-9]" "" current_variable ${compiler_flag})
    # TODO FIX ME
    check_c_compiler_flag(${compiler_flag} "FLAG_SUPPORTED_${current_variable}")
    if(FLAG_SUPPORTED_${current_variable})
        list(APPEND supported_compiler_flags_cxx)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${compiler_flag}")
    endif()
endforeach()

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${supported_compiler_flags} -v")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${supported_compiler_flags_cxx} -v")

set(UNITTEST_TARGET_PREFIX unit_test_)

# add project-wide directory with useful macros, project-wide definitions, etc.
set(COMPILER_INTERNALS_INC
    ${CMAKE_CURRENT_SOURCE_DIR}/compiler
    CACHE INTERNAL "")
message(STATUS "COMPILER_INTERNALS_INC =  ${COMPILER_INTERNALS_INC}")

set(BUILDINFO_PATH
    ${CMAKE_CURRENT_SOURCE_DIR}
    CACHE INTERNAL "")
message(STATUS "BUILDINFO_PATH = ${BUILDINFO_PATH}")

add_custom_command(
    OUTPUT ${BUILDINFO_PATH}/buildinfo.h
    COMMAND rm -f ${BUILDINFO_PATH}/buildinfo.h && git rev-parse --short=8 HEAD | xargs python3
            scripts/gen_info.py > buildinfo.h
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    VERBATIM)

add_custom_target(gen_buildinfo ALL DEPENDS ${BUILDINFO_PATH}/buildinfo.h)

include_directories(${BUILDINFO_PATH})

if(NOT CMAKE_SYSTEM_NAME STREQUAL CMAKE_HOST_SYSTEM_NAME)
    # PC-Lint function INCLUDE_LINT variable is set/reset by the command line option for cmake
    if(${INCLUDE_LINT})
        if(EXISTS ${TOOLCHAIN_PREFIX}/pc-lint2.cmake)
            include(${TOOLCHAIN_PREFIX}/pc-lint2.cmake)
        endif()
    endif()

    # comment it out, if not needed
    add_definitions(-DMCU_TARGET)

    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/target/mcu)

    message(STATUS "CMAKE_TOOLCHAIN_FILE = " ${CMAKE_TOOLCHAIN_FILE})
    message(STATUS "CMAKE_CURRENT_SOURCE_DIR = " ${CMAKE_CURRENT_SOURCE_DIR})

    # PC-Lint target
    if(COMMAND add_pc_lint)
        # establish list of compile defifntiond
        set(comp_defs_list)
        list(APPEND comp_defs_list ${D_MCU})
        list(APPEND comp_defs_list ${D_MCU_EXACT})
        list(APPEND comp_defs_list ${D_HSE_VAL})

        # fantastic loop
        foreach(s IN LISTS ${EXEC_NAME}_LIST_OF_SOURCES)

            set(t_name)
            string(REPLACE "/" "_" t_name ${s})

            add_pc_lint(
                # compile defines
                "${comp_defs_list}"
                # target
                ${t_name} ${EXEC_NAME} ${s})
        endforeach()
    endif(COMMAND add_pc_lint)

else()

    message("Non-MCU target detected")

    # add host target cmake file - for tests on the host
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/target/desktop)

endif()
