#ifndef __OVER_OBJECT_H_
#define __OVER_OBJECT_H_

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

/* State */
typedef enum
{
    EM_OVER_CHECK_STATE_IDLE = 0,
    EM_OVER_CHECK_STATE_RUN,
    EM_OVER_CHECK_STATE_FAULT,
} EM_OVER_CHECK_STATE;
/* Object */
extern game_object_t over_check_object;
/* Function */
extern void draw_over_icon(void);
#endif //__OVER_OBJECT_H_
