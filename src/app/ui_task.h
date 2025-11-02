/** @file
 *  @brief
 *
 *  @author Vasiliy Turchenko
 *  @bug
 *  @date
 */

#ifndef UI_TASK_H
#define UI_TASK_H

#ifdef __cplusplus
extern "C" {
#endif

void ui_task_init(void);
void ui_task_run(void);

void start_reflow(uint8_t np);
void manual_heater_control(void);

#ifdef __cplusplus
}
#endif

#endif
