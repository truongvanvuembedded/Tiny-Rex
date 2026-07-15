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
    // Draw T-Rex
    draw_tiny_rex_object();
    // Draw line
    draw_line_object();
    // Draw tree
    draw_tree_object();
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
        task_post_pure_msg(TINY_REX_OBJECT_ID, EVENT_TINY_REX_OBJECT_SETUP);
        task_post_pure_msg(TINY_REX_OBJECT_ID, EVENT_TINY_REX_OBJECT_START);
        task_post_pure_msg(LINE_OBJECT_ID, EVENT_LINE_OBJECT_SETUP);
        task_post_pure_msg(LINE_OBJECT_ID, EVENT_LINE_OBJECT_START);
        task_post_pure_msg(TREE_OBJECT_ID, EVENT_TREE_OBJECT_SETUP);
        task_post_pure_msg(TREE_OBJECT_ID, EVENT_TREE_OBJECT_START);
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
        task_post_pure_msg(TINY_REX_OBJECT_ID, EVENT_TINY_REX_OBJECT_UP);
    }
    break;

    case AC_DISPLAY_BUTON_MODE_PRESSED:
    {
        BUZZER_PlaySound(BUZZER_SOUND_CLICK);
        task_post_pure_msg(TINY_REX_OBJECT_ID, EVENT_TINY_REX_OBJECT_DUCK);
    }
    break;
    case AC_DISPLAY_BUTON_MODE_RELEASE:
    {
        task_post_pure_msg(TINY_REX_OBJECT_ID, EVENT_TINY_REX_OBJECT_DUCK_RELEAASE);
    }
    break;

    case AC_DISPLAY_BUTON_DOWN_PRESSED:
    {
        BUZZER_PlaySound(BUZZER_SOUND_CLICK);
        task_post_pure_msg(TINY_REX_OBJECT_ID, EVENT_TINY_REX_OBJECT_DOWN);
    }
    break;

    case AC_DISPLAY_PLAYING_UPDATE:
    {
        task_post_pure_msg(TINY_REX_OBJECT_ID, EVENT_TINY_REX_OBJECT_UPDATE);
        task_post_pure_msg(LINE_OBJECT_ID, EVENT_LINE_OBJECT_UPDATE);
        task_post_pure_msg(TREE_OBJECT_ID, EVENT_TREE_OBJECT_UPDATE);
    }
    break;

    default:
        break;
    }
}