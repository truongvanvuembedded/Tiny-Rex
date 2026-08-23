#ifndef __OBSTACLE_OBJECT_H_
#define __OBSTACLE_OBJECT_H_

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

/* Variables */
extern uint8_t obstacle_objects_speed;
/* Function */
extern void draw_obstacle_objects(void);
#endif //__OBSTACLE_OBJECT_H_
