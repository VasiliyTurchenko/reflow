/** @file mutex_helpers.h
 *  @brief mutex macros
 *
 *  @author turchenkov@gmail.com
 *  @bug
 *  @date 08-Jul-2021
 */

#ifndef MUTEX_HELPERS_H
#define MUTEX_HELPERS_H

/* An odd helper */
#define OBJECT_EXPORTED(PREFIX, NAME) typedef int NAME##_##PREFIX##_EXPORTED

/*A macro to ease mutex export */
#define EXPORT_MUTEX_HANDLE(NAME)                                                                  \
    extern SemaphoreHandle_t NAME##_MutexHandle;                                                   \
    OBJECT_EXPORTED(M, NAME);

/*A macro to ease mutex export */
#define EXPORT_TASK_HANDLE(NAME)                                                                   \
    extern TaskHandle_t NAME##_taskHandle;                                                         \
    OBJECT_EXPORTED(H, NAME);

/* a macro to ease defining task's memory structures */
/* do not forget export TaskHandle_t's to the freertos_exported.h ! */
#define TASK_MEM_DEF(NAME, STACKSIZE_STACKTYPE, PRIO)                                              \
    TaskHandle_t NAME##_taskHandle ATTR_CCMRAM = NULL;                                             \
    static StackType_t             NAME##_task_stack[STACKSIZE_STACKTYPE] ATTR_CCMRAM;             \
    static StaticTask_t NAME##_task_TCB ATTR_CCMRAM;                                               \
    static const osThreadAttr_t         NAME##_task_attributes = {                                 \
                .name       = #NAME "_task",                                                       \
                .attr_bits  = osThreadDetached,                                                    \
                .cb_mem     = (void *)&NAME##_task_TCB,                                            \
                .cb_size    = sizeof(StaticTask_t),                                                \
                .stack_mem  = (void *)NAME##_task_stack, /*stack_size in STACKSIZE_STACKTYPE!*/    \
                .stack_size = (STACKSIZE_STACKTYPE),                                               \
                .priority   = (UBaseType_t)PRIO,                                                   \
    };                                                                                             \
    typedef NAME##_H_EXPORTED NAME##_H_EXPORT_CHECKED;

/* a macro to ease defining mutex's memory structures */
#define MUTEX_MEM_DEF(NAME)                                                                        \
    extern SemaphoreHandle_t NAME##_MutexHandle;                                                   \
    SemaphoreHandle_t NAME##_MutexHandle ATTR_CCMRAM;                                              \
    static StaticSemaphore_t NAME##_MutexControlBlock ATTR_CCMRAM;                                 \
    static osMutexAttr_t NAME##_MutexAttr ATTR_CCMRAM = {                                          \
        .name      = #NAME "_mutex",                                                               \
        .attr_bits = 0U,                                                                           \
        .cb_mem    = (void *)&NAME##_MutexControlBlock,                                            \
        .cb_size   = sizeof(StaticSemaphore_t),                                                    \
    };                                                                                             \
    typedef NAME##_M_EXPORTED NAME##_M_EXPORT_CHECKED;

#define TAKE_MUTEX(X)                                                                              \
    do {                                                                                           \
        /*lint -save -e9076 */                                                                     \
        if (xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED) {                               \
            (void)xSemaphoreTake((X), portMAX_DELAY);                                              \
            /*lint -restore */                                                                     \
        }                                                                                          \
    } while (false)

#define GIVE_MUTEX(X)                                                                              \
    do {                                                                                           \
        /*lint -save -e9076 */                                                                     \
        if (xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED) {                               \
            (void)xSemaphoreGive(X);                                                               \
        }                                                                                          \
        /*lint -restore */                                                                         \
    } while (false)

#define ENTER_CRITICAL_SECTION_                                                                    \
    do {                                                                                           \
        if (xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED) {                               \
            taskENTER_CRITICAL();                                                                  \
        }                                                                                          \
    } while (false)

#define EXIT_CRITICAL_SECTION_                                                                     \
    do {                                                                                           \
        if (xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED) {                               \
            taskEXIT_CRITICAL();                                                                   \
        }                                                                                          \
    } while (false)

#endif // MUTEX_HELPERS_H
