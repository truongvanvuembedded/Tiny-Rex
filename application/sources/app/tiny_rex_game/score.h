#ifndef __SCORE_OBJECT_H_
#define __SCORE_OBJECT_H_

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

/* Function */
extern void draw_score(void);
extern void draw_over_icon(void);
#endif //__SCORE_OBJECT_H_
