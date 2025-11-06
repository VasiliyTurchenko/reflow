# TOP LEVEL CMAKEFILE #######

cmake_minimum_required(VERSION 3.20)
set(CMAKE_VERBOSE_MAKEFILE)
set(CMAKE_C_STANDARD 17)
# set(CMAKE_CXX_STANDARD 20)

project(REFLOW LANGUAGES C CXX ASM)

message(STATUS "\nProcessing ${CMAKE_CURRENT_LIST_FILE}...")

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/modules" ${CMAKE_MODULE_PATH})
set(CMAKE_COLOR_DIAGNOSTICS ON)

option(ENABLE_CUSTOM_COMPILER_FLAGS "Enables custom compiler flags" ON)
option(ENABLE_SANITIZERS "Enables AddressSanitizer and UndefinedBehaviorSanitizer." OFF)

include(CheckCCompilerFlag)
include(StrictCompileFlags)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -v")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  -v")

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

include(BuildInfo)

message(STATUS "CMAKE_SYSTEM_NAME = ${CMAKE_SYSTEM_NAME}")
message(STATUS "CMAKE_HOST_SYSTEM_NAME = ${CMAKE_HOST_SYSTEM_NAME}")
message(STATUS "CMAKE_TOOLCHAIN_FILE = " ${CMAKE_TOOLCHAIN_FILE})
message(STATUS "CMAKE_CURRENT_SOURCE_DIR = " ${CMAKE_CURRENT_SOURCE_DIR})
message(STATUS "CMAKE_SYSROOT =  ${CMAKE_SYSROOT}")

if(NOT CMAKE_SYSTEM_NAME STREQUAL CMAKE_HOST_SYSTEM_NAME)
    message(STATUS "Cross-compiling...")

    include(MCUSelector)

    # PC-Lint function INCLUDE_LINT variable is set/reset by the command line option for cmake
    if(${INCLUDE_LINT})
        if(EXISTS ${TOOLCHAIN_PREFIX}/pc-lint2.cmake)
            include(${TOOLCHAIN_PREFIX}/pc-lint2.cmake)
        endif()
    endif()

    # comment it out, if not needed
    add_definitions(-DMCU_TARGET)

    # Start including targets here:
    set(mcu "stm32f103c8t6")
    set(board "st_smart_v2")
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src)
    unset(MCU)
    unset(BOARD)

    # another target here set(MCU xxxx) set(BOARD yyyyy)
    # add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src) unset(MCU) unset(BOARD)

else()
    message("Non-MCU target detected")

    # add host target cmake file - for tests on the host
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/target/desktop)

endif()
