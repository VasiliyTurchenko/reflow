set(GROUP_FREERTOS_SOURCE_SRC
	${CMAKE_CURRENT_LIST_DIR}/FreeRTOS/Source/croutine.c
	${CMAKE_CURRENT_LIST_DIR}/FreeRTOS/Source/event_groups.c
	${CMAKE_CURRENT_LIST_DIR}/FreeRTOS/Source/list.c
	${CMAKE_CURRENT_LIST_DIR}/FreeRTOS/Source/queue.c
	${CMAKE_CURRENT_LIST_DIR}/FreeRTOS/Source/tasks.c
	${CMAKE_CURRENT_LIST_DIR}/FreeRTOS/Source/timers.c) # of GROUP_FREERTOS_SOURCE_SRC

set(GROUP_FREERTOS_SOURCE_CMSIS_RTOS_SRC
	${CMAKE_CURRENT_LIST_DIR}/FreeRTOS/Source/CMSIS_RTOS/cmsis_os.c
	${CMAKE_CURRENT_LIST_DIR}/FreeRTOS/Source/CMSIS_RTOS/cmsis_os.h
)# of GROUP_FREERTOS_SOURCE_CMSIS_RTOS_SRC

set(GROUP_FREERTOS_SOURCE_INCLUDE_SRC
	${CMAKE_CURRENT_LIST_DIR}/FreeRTOS/Source/include/FreeRTOS.h
	${CMAKE_CURRENT_LIST_DIR}/FreeRTOS/Source/include/StackMacros.h
	${CMAKE_CURRENT_LIST_DIR}/FreeRTOS/Source/include/croutine.h
	${CMAKE_CURRENT_LIST_DIR}/FreeRTOS/Source/include/deprecated_definitions.h
	${CMAKE_CURRENT_LIST_DIR}/FreeRTOS/Source/include/event_groups.h
	${CMAKE_CURRENT_LIST_DIR}/FreeRTOS/Source/include/list.h
	${CMAKE_CURRENT_LIST_DIR}/FreeRTOS/Source/include/mpu_prototypes.h
	${CMAKE_CURRENT_LIST_DIR}/FreeRTOS/Source/include/mpu_wrappers.h
	${CMAKE_CURRENT_LIST_DIR}/FreeRTOS/Source/include/portable.h
	${CMAKE_CURRENT_LIST_DIR}/FreeRTOS/Source/include/projdefs.h
	${CMAKE_CURRENT_LIST_DIR}/FreeRTOS/Source/include/queue.h
	${CMAKE_CURRENT_LIST_DIR}/FreeRTOS/Source/include/semphr.h
	${CMAKE_CURRENT_LIST_DIR}/FreeRTOS/Source/include/task.h
	${CMAKE_CURRENT_LIST_DIR}/FreeRTOS/Source/include/timers.h) # of
                                                                # GROUP_FREERTOS_SOURCE_INCLUDE_SRC

set(GROUP_FREERTOS_SOURCE_PORTABLE_GCC_ARM_CM3_SRC
	${CMAKE_CURRENT_LIST_DIR}/FreeRTOS/Source/portable/GCC/ARM_CM3/port.c
	${CMAKE_CURRENT_LIST_DIR}/FreeRTOS/Source/portable/GCC/ARM_CM3/portmacro.h
)# of GROUP_FREERTOS_SOURCE_PORTABLE_GCC_ARM_CM3_SRC

set(INCLUDE_DIRS_THIRD_PARTY_LIST
	${CMAKE_CURRENT_LIST_DIR}/FreeRTOS/Source/CMSIS_RTOS
	${CMAKE_CURRENT_LIST_DIR}/FreeRTOS/Source/include
	${CMAKE_CURRENT_LIST_DIR}/FreeRTOS/Source/portable/GCC/ARM_CM3) # of
                                                                    # INCLUDE_DIRS_THIRD_PARTY_LIST

set(THIRD_PARTY_SOURCES_LIST
	${GROUP_FREERTOS_SOURCE_SRC} ${GROUP_FREERTOS_SOURCE_CMSIS_RTOS_SRC}
	${GROUP_FREERTOS_SOURCE_INCLUDE_SRC} ${GROUP_FREERTOS_SOURCE_PORTABLE_GCC_ARM_CM3_SRC}
)# of THIRD_PARTY_SOURCES_LIST
