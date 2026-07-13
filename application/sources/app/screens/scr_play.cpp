//==================================================================================================
//
//	File Name	:	led.c
//	CPU Type	:
//	IDE			:
//	Customer
//	Version		:	Ver.0.01
//	Coding		:	V.Vu
//	History		:	28/03/2026
//	Outline		:
//
//==================================================================================================
//==================================================================================================
//	#pragma section
//==================================================================================================

//==================================================================================================
//	Local Compile Option
//==================================================================================================

//==================================================================================================
//	Header File
//==================================================================================================
#include "scr_play.h"
//==================================================================================================
//	Local define
//==================================================================================================
#define GROUND_WIDTH (128)
#define GROUND_HEIGHT (5)
#define GROUND_Y (59)

#define TREX_X (5)
#define TREX_GROUND_Y (39)
#define TREX_WIDTH (23)
#define TREX_HEIGHT (25)

#define GROUND_SCROLL_SPEED (4)
#define TREX_JUMP_SPEED (5)
#define TREX_JUMP_TOP_Y (0)
//==================================================================================================
//	Local define I/O
//==================================================================================================

//==================================================================================================
//	Local Struct Template
//==================================================================================================
typedef enum
{
    TREX_STATE_RUN = 0,
    TREX_STATE_JUMP_UP,
    TREX_STATE_FALL_DOWN

} trex_state_t;
//==================================================================================================
//	Global RAM
//==================================================================================================
static void view_scr_play();
view_dynamic_t dyn_view_scr_play = {
    {
        .item_type = ITEM_TYPE_DYNAMIC,
    },
    view_scr_play};

view_screen_t scr_play = {
    &dyn_view_scr_play,
    ITEM_NULL,
    ITEM_NULL,

    .focus_item = 0,
};
//==================================================================================================
//	Local RAM
//==================================================================================================
static int16_t ground_offset_x;
static int16_t trex_y;
static trex_state_t trex_state;
//==================================================================================================
//	Local ROM
//==================================================================================================

//==================================================================================================
//	Local Function Prototype
//==================================================================================================
static void update_ground();
static void update_animation();
//==================================================================================================
//	Source Code
//==================================================================================================
////////////////////////////////////////////////////////////////////////////////////////////////////
//	Name     : view_scr_play
//	Function : Render all visual objects of Play screen
//	Argument : None
//	Return   : None
//	Created  : 13/07/2026 V.Vu
//	Changed  : -
//	Remarks  : This function is called by Screen Manager whenever the Play screen has event
//             Render order:
//             1. Draw scrolling ground.
//             2. Draw duplicated ground bitmap for seamless scrolling.
//             3. Draw T-Rex sprite.
////////////////////////////////////////////////////////////////////////////////////////////////////
static void view_scr_play()
{
    // Draw bit-map of line
    view_render.drawBitmap(
        ground_offset_x,
        GROUND_Y,
        epd_bitmap_line,
        GROUND_WIDTH,
        GROUND_HEIGHT,
        WHITE);

    view_render.drawBitmap(
        ground_offset_x + GROUND_WIDTH,
        GROUND_Y,
        epd_bitmap_line,
        GROUND_WIDTH,
        GROUND_HEIGHT,
        WHITE);

    // Draw bit-map of T-Rex
    view_render.drawBitmap(
        TREX_X,
        trex_y,
        image_trex_running,
        TREX_WIDTH,
        TREX_HEIGHT,
        WHITE);
}
////////////////////////////////////////////////////////////////////////////////////////////////////
//	Name     : scr_play_handle_signal
//	Function : Hanlde signal receive from Display task
//	Argument : ak_msg_t* msg: Message information
//	Return   : None
//	Created  : 13/07/2026 V.Vu
//	Changed  : -
//	Remarks  : -
////////////////////////////////////////////////////////////////////////////////////////////////////
void scr_play_handle_signal(ak_msg_t* msg)
{
    switch (msg->sig)
    {
    case SCREEN_ENTRY:
    {
        APP_DBG_SIG("SCREEN_PLAY_ENTRY\n");

        timer_set(
            AC_TASK_DISPLAY_ID,
            AC_DISPLAY_PLAYING_UPDATE,
            AC_DISPLAY_MENU_PLAYING_UPDATE_INTERVAL,
            TIMER_PERIODIC);

        ground_offset_x = 0;
        trex_y = TREX_GROUND_Y;
        trex_state = TREX_STATE_RUN;
    }
    break;

    case AC_DISPLAY_BUTON_UP_PRESSED:
    case AC_DISPLAY_BUTON_MODE_PRESSED:
    {
        BUZZER_PlaySound(BUZZER_SOUND_CLICK);

        if (trex_state == TREX_STATE_RUN)
        {
            trex_state = TREX_STATE_JUMP_UP;
        }
    }
    break;

    case AC_DISPLAY_BUTON_DOWN_PRESSED:
    {
        BUZZER_PlaySound(BUZZER_SOUND_CLICK);
        if (trex_state == TREX_STATE_JUMP_UP)
        {
            trex_state = TREX_STATE_FALL_DOWN;
        }
    }
    break;

    case AC_DISPLAY_PLAYING_UPDATE:
    {
        update_ground();
        update_animation();
    }
    break;

    default:
        break;
    }
}
////////////////////////////////////////////////////////////////////////////////////////////////////
//	Name     : update_ground
//	Function : update ground line
//	Argument : None
//	Return   : None
//	Created  : 13/07/2026 V.Vu
//	Changed  : -
//	Remarks  : -
////////////////////////////////////////////////////////////////////////////////////////////////////
static void update_ground()
{
    ground_offset_x -= GROUND_SCROLL_SPEED;

    if (ground_offset_x <= -GROUND_WIDTH)
    {
        ground_offset_x = 0;
    }
}
////////////////////////////////////////////////////////////////////////////////////////////////////
//	Name     : update_animation
//	Function : Udpate animation for T-Rex and other objects
//	Argument : None
//	Return   : None
//	Created  : 13/07/2026 V.Vu
//	Changed  : -
//	Remarks  : -
////////////////////////////////////////////////////////////////////////////////////////////////////
static void update_animation()
{
    switch (trex_state)
    {

    case TREX_STATE_RUN:
    {
        image_trex_running =
            toggle_frame(
                image_trex_running,
                epd_bitmap_t_rex__1_1,
                epd_bitmap_t_rex__1_2);
    }
    break;

    case TREX_STATE_JUMP_UP:
    {
        trex_y -= TREX_JUMP_SPEED;

        if (trex_y <= TREX_JUMP_TOP_Y)
        {
            trex_y = TREX_JUMP_TOP_Y;
            trex_state = TREX_STATE_FALL_DOWN;
        }
    }
    break;

    case TREX_STATE_FALL_DOWN:
    {
        trex_y += TREX_JUMP_SPEED;

        if (trex_y >= TREX_GROUND_Y)
        {
            trex_y = TREX_GROUND_Y;
            trex_state = TREX_STATE_RUN;
        }
    }
    break;

    default:
        break;
    }
}