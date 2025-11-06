message(STATUS "\nProcessing ${CMAKE_CURRENT_LIST_FILE}...")

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

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${supported_compiler_flags}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${supported_compiler_flags_cxx}")

