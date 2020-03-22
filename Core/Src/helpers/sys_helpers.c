/** @file sys_helpers.c
 *  @brief
 *
 *  @author Vasiliy Turchenko
 *  @bug
 *  @date 22-Jan-2020
 */

#include "cmsis_os.h"
#include "sys_helpers.h"

/**
 * @brief sys_helpers_delay_ms
 * @param ms
 */
void sys_helpers_delay_ms(uint32_t ms)
{
	if (ms > 0U) {
		if (xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED) {
			vTaskDelay(pdMS_TO_TICKS(ms));
		} else {
			HAL_Delay(ms);
		}
	}
}

/**
 * @brief sys_helpers_delay_ms_and_reboot
 * @param ms
 */
void __attribute__((noreturn)) sys_helpers_delay_ms_and_reboot(uint32_t ms)
{
	sys_helpers_delay_ms(ms);
	NVIC_SystemReset();
	while(1) {}
}
