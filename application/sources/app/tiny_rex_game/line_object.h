#ifndef __LINE_OBJECT_H_
#define __LINE_OBJECT_H_

#include <stdio.h>

#include "fsm.h"
#include "port.h"
#include "message.h"
#include "timer.h"

#include "app.h"
#include "app_dbg.h"
#include "task_list.h"
#include "task_display.h"

#include "scr_play.h"
#include "game_object.h"

/* Axis */
#define AXIS_X_LINE_OBJECT (0)
#define AXIS_Y_LINE_OBJECT (59)
/* Size */
#define GROUND_WIDTH (128)
#define GROUND_HEIGHT (5)
/* State */
typedef enum
{
    EM_LINE_STATE_IDLE = 0,
    EM_LINE_STATE_RUNNING,
} EM_LINE_STATE;
/* Object */
extern game_object_t line_object;
/* Function */
extern void draw_line_object(void);
#endif //__LINE_OBJECT_H_
