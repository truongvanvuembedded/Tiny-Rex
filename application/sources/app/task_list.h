#ifndef __TASK_LIST_H__
#define __TASK_LIST_H__

#include "ak.h"
#include "task.h"

extern const task_t app_task_table[];
extern const task_polling_t app_task_polling_table[];

/*****************************************************************************/
/*  DECLARE: Internal Task ID
 *  Note: Task id MUST be increasing order.
 */
/*****************************************************************************/
enum {
	/* SYSTEM TASKS */
	TASK_TIMER_TICK_ID,

	/* APP TASKS */
	TINY_REX_TASK_LIFE_ID,
	TINY_REX_TASK_DISPLAY_ID,

	/* TINY_REX GAME ID */
	TINY_REX_OBJECT_ID,
	OBSTACLE_OBJECT_ID,
	HORIZON_OBJECT_ID,
	SCORE_ID,
	/* EOT task ID */
	AK_TASK_EOT_ID,
};

/*****************************************************************************/
/*  DECLARE: Internal Polling Task ID
 *  Note: Task id MUST be increasing order.
 */
/*****************************************************************************/
enum {
	/* EOT polling task ID */
	AK_TASK_POLLING_EOT_ID,
};

/*****************************************************************************/
/*  DECLARE: Task entry point
 */
/*****************************************************************************/
/* APP TASKS */
extern void task_life(ak_msg_t*);
extern void task_display(ak_msg_t*);

/* GAME HANDLE */
extern void tiny_rex_object_handle(ak_msg_t* msg);
extern void horizon_object_handle(ak_msg_t* msg);
extern void obstacle_objects_handle(ak_msg_t* msg);
extern void score_handle(ak_msg_t* msg);

#endif //__TASK_LIST_H__
