message(STATUS "\nProcessing ${CMAKE_CURRENT_LIST_FILE}...")

add_custom_command(
    OUTPUT ${BUILDINFO_PATH}/buildinfo.h
    COMMAND rm -f ${BUILDINFO_PATH}/buildinfo.h && git rev-parse --short=8 HEAD | xargs python3
            scripts/gen_info.py > buildinfo.h
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    VERBATIM)

add_custom_target(gen_buildinfo ALL DEPENDS ${BUILDINFO_PATH}/buildinfo.h)

include_directories(${BUILDINFO_PATH})
