#ifndef __TREE_OBJECT_H_
#define __TREE_OBJECT_H_

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
#define AXIS_X_TREE_OBJECT (128)
#define AXIS_Y_TREE_OBJECT (36)
/* Menu screen */
#define AXIS_X_TREE_OBJECT_IN_MENU_SCREEN (104)
#define AXIS_Y_TREE_OBJECT_IN_MENU_SCREEN (36)
/* */
#define TREE_WIDTH_IN_RUN (128+15)

/* State */
typedef enum
{
    EM_TREE_STATE_IDLE = 0,
    EM_TREE_STATE_STAND,
    EM_TREE_STATE_RUNNING,
} EM_TREE_STATE;
/* Object */
extern game_object_t tree_object;
/* Function */
extern void draw_tree_object(void);
#endif //__TREE_OBJECT_H_
