#ifndef __BIRD_OBJECT_H_
#define __BIRD_OBJECT_H_

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
#define AXIS_X_BIRD_OBJECT_INIT (64)
#define AXIS_X_BIRD_OBJECT (128)
#define AXIS_Y_BIRD_OBJECT (25)
/* Menu screen */
#define AXIS_X_BIRD_OBJECT_IN_MENU_SCREEN (50)
#define AXIS_Y_BIRD_OBJECT_IN_MENU_SCREEN (40)
/* */
#define BIRD_WIDTH_IN_RUN (128+15)

/* State */
typedef enum
{
    EM_BIRD_STATE_IDLE = 0,
    EM_BIRD_STATE_STAND_FLY,
    EM_BIRD_STATE_FLYING,
} EM_BIRD_STATE;
/* Object */
extern game_object_t bird_object;
/* Function */
extern void draw_bird_object(void);
#endif //__BIRD_OBJECT_H_
