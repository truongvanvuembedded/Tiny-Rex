#ifndef __TINY_REX_OBJECT_H_
#define __TINY_REX_OBJECT_H_

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
#define AXIS_X_TINY_REX_OBJECT (5)
#define AXIS_Y_TINY_REX_OBJECT (39)
/* State */
typedef enum
{
    EM_TINY_REX_STATE_IDLE = 0,
    EM_TINY_REX_STATE_RUNNING,
    EM_TINY_REX_STATE_DANCING,
    EM_TINY_REX_STATE_JUMPING,
    EM_TINY_REX_STATE_FALLING,
    EM_TINY_REX_STATE_DUCKING,
} EM_TINY_REX_STATE;
/* Object */
extern game_object_t tiny_rex_object;

#endif //__TINY_REX_OBJECT_H_
