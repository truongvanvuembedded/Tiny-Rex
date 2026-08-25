//==================================================================================================
//
//	File Name	:	scr_play.c
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
#include "scr_ranking.h"
#include "scr_setting.h"
#include "score.h"
//==================================================================================================
//	Local define
//==================================================================================================

//==================================================================================================
//	Local define I/O
//==================================================================================================

//==================================================================================================
//	Local Struct Template
//==================================================================================================

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
uint8_t Game_State;
//==================================================================================================
//	Local RAM
//==================================================================================================

//==================================================================================================
//	Local ROM
//==================================================================================================

//==================================================================================================
//	Local Function Prototype
//==================================================================================================

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
    /* Draw object */
    draw_score();
    draw_horizon_objects();
    draw_obstacle_objects();
    draw_tiny_rex_object();
    draw_over_icon();
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
        Game_State = EM_GAME_STATE_PLAYING;
        task_post_pure_msg(TINY_REX_OBJECT_ID, TINY_REX_PLAY_EVENT);
        task_post_pure_msg(OBSTACLE_OBJECT_ID, OBSTACLE_PLAY_EVENT);
        task_post_pure_msg(HORIZON_OBJECT_ID, HORIZON_OBJECT_PLAY_EVENT);
        task_post_pure_msg(SCORE_ID, SCORE_SETUP);
        timer_set(
            AC_TASK_DISPLAY_ID,
            AC_DISPLAY_PLAYING_UPDATE,
            AC_DISPLAY_PLAYING_UPDATE_INTERVAL,
            TIMER_PERIODIC);
    }
    break;

    case AC_DISPLAY_BUTON_UP_PRESSED:
    {
        BUZZER_PlaySound(BUZZER_SOUND_CLICK);
        task_post_pure_msg(TINY_REX_OBJECT_ID, TINY_REX_JUMP_EVENT);
    }
    break;

    case AC_DISPLAY_BUTON_MODE_PRESSED:
    {
        BUZZER_PlaySound(BUZZER_SOUND_CLICK);
        if (Game_State == EM_GAME_STATE_OVER)
        {
            SCREEN_TRAN(scr_menu_handle, &scr_menu);
        }else{
            task_post_pure_msg(TINY_REX_OBJECT_ID, TINY_REX_DUCK_EVENT);
        }
    }
    break;
    case AC_DISPLAY_BUTON_MODE_RELEASE:
    {
        task_post_pure_msg(TINY_REX_OBJECT_ID, TINY_REX_DUCK_RELEASE_EVENT);
    }
    break;

    case AC_DISPLAY_BUTON_DOWN_PRESSED:
    {
        BUZZER_PlaySound(BUZZER_SOUND_CLICK);
        task_post_pure_msg(TINY_REX_OBJECT_ID, TINY_REX_FALL_EVENT);
    }
    break;

    case AC_DISPLAY_PLAYING_UPDATE:
    {
        task_post_pure_msg(OBSTACLE_OBJECT_ID, OBSTACLE_CHECK_COLLISSION_EVENT);
        task_post_pure_msg(TINY_REX_OBJECT_ID, TINY_REX_MOVE_EVENT);
        task_post_pure_msg(OBSTACLE_OBJECT_ID, OBSTACLE_MOVE_EVENT);
        task_post_pure_msg(HORIZON_OBJECT_ID, HORIZON_OBJECT_UPDATE_EVENT);
        task_post_pure_msg(SCORE_ID, SCORE_UPDATE);
    }
    break;

    case AC_DISPLAY_PLAYING_GAME_OVER:
    {
        /* Game Over */
        Game_State = EM_GAME_STATE_OVER;
        task_post_pure_msg(SCORE_ID, SCORE_GAME_OVER);
        BUZZER_PlaySound(BUZZER_SOUND_GOODBYE);
    }
    break;

    default:
        break;
    }
}