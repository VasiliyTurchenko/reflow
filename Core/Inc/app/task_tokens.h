/** @file task_tokens.h
 *  @brief Task tokens for independent watchdog
 *
 *  @author Vasiliy Turchenko
 *  @bug
 *  @date
 */

#ifndef TASK_TOKENS_H
#define TASK_TOKENS_H

#ifdef __cplusplus
extern "C" {
#endif

#define MAGIC_SEED	1U

#define	COMM_TASK_MAGIC		(uint32_t)(MAGIC_SEED)
#define KBD_TASK_MAGIC		(uint32_t)(MAGIC_SEED + 1U)
#define CONTROL_TASK_MAGIC	(uint32_t)(MAGIC_SEED + 2U)
#define TEMPERATUR_TASK_MAGIC	(uint32_t)(MAGIC_SEED + 3U)
#define UI_TASK_MAGIC		(uint32_t)(MAGIC_SEED + 4U)

#ifdef __cplusplus
}
#endif

#endif // TASK_TOKENS_H
