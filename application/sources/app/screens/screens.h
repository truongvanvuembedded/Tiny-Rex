#ifndef __SCREENS_H__
#define __SCREENS_H__

#include "fsm.h"
#include "port.h"
#include "message.h"
#include "timer.h"

#include "sys_ctrl.h"
#include "sys_dbg.h"

#include "app.h"
#include "app_dbg.h"
#include "task_list.h"
#include "task_display.h"
#include "view_render.h"

#include "buzzer.h"

#include <math.h>
#include <vector>

#include "screens_bitmap.h"
/* game */
#include "tiny_rex_object.h"

#ifdef REAL_RAM
	#define EXTERN
#else
	#define EXTERN extern
#endif

// scr_menu.cpp
extern view_screen_t scr_menu;
extern void scr_menu_handle(ak_msg_t* msg);

// scr_startup.cpp
extern view_screen_t scr_startup;
extern void scr_startup_handle(ak_msg_t* msg);

// scr_play.cpp
extern view_screen_t scr_play;
extern void scr_play_handle_signal(ak_msg_t* msg);

// Variable used in screens
EXTERN const unsigned char* image_trex_running;
EXTERN const unsigned char* image_trex_ducking;
EXTERN const unsigned char* image_bird_flying;

// Function used in screens
extern const unsigned char* toggle_frame(
    const unsigned char* current,
    const unsigned char* frame1,
    const unsigned char* frame2);

#endif //__SCREENS_H__
