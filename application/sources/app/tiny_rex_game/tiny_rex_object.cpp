//==================================================================================================
//	File Name	:
//	CPU Type	:
//	Builder		:
//	Coding		:
//	History		:
//	Outline		:
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
#include "tiny_rex_object.h"
//==================================================================================================
//	Local define
//==================================================================================================
#define TREX_JUMP_SPEED (5)
#define TREX_JUMP_TOP_Y (0)
#define TREX_GROUND_Y (39)
//==================================================================================================
//	Local define I/O
//==================================================================================================

//==================================================================================================
//	Local Struct Template
//==================================================================================================

//==================================================================================================
//	Local RAM
//==================================================================================================

//==================================================================================================
//	Global RAM
//==================================================================================================
game_object_t tiny_rex_object;
//==================================================================================================
//	Local ROM
//==================================================================================================

//==================================================================================================
//	Local Function Prototype
//==================================================================================================
static void update_trex_y(void);
static void update_trex_bitmap(void);
//==================================================================================================
//	Source Code
//==================================================================================================
void tiny_rex_object_handle(ak_msg_t* msg)
{
    switch (msg->sig)
    {
    case EVENT_TINY_REX_OBJECT_SETUP:
    {
        APP_DBG_SIG("EVENT_TINY_REX_OBJECT_SETUP\n");
        tiny_rex_object.x = AXIS_X_TINY_REX_OBJECT;
        tiny_rex_object.y = AXIS_Y_TINY_REX_OBJECT;
        tiny_rex_object.visible = WHITE;
        tiny_rex_object.state = EM_TINY_REX_STATE_IDLE;
        tiny_rex_object.bitmap_index = BITMAP_T_REX_1;
    }
    break;
    case EVENT_TINY_REX_OBJECT_START:
    {
        APP_DBG_SIG("EVENT_TINY_REX_OBJECT_START\n");
        tiny_rex_object.x = AXIS_X_TINY_REX_OBJECT;
        tiny_rex_object.y = AXIS_Y_TINY_REX_OBJECT;
        tiny_rex_object.visible = WHITE;
        tiny_rex_object.state = EM_TINY_REX_STATE_RUNNING;
        tiny_rex_object.bitmap_index = BITMAP_T_REX_RUN_1;
    }
    break;
    case EVENT_TINY_REX_OBJECT_UPDATE:
    {
        APP_DBG_SIG("EVENT_TINY_REX_OBJECT_UPDATE\n");
        tiny_rex_object.visible = WHITE;
        /* Update */
        update_trex_y();
        update_trex_bitmap();
    }
    break;
    case EVENT_TINY_REX_OBJECT_DANCE:
    {
        APP_DBG_SIG("EVENT_TINY_REX_OBJECT_DANCE\n");
        tiny_rex_object.visible = WHITE;
        tiny_rex_object.state = EM_TINY_REX_STATE_DANCING;
    }
    break;
    case EVENT_TINY_REX_OBJECT_UP:
    {
        APP_DBG_SIG("EVENT_TINY_REX_OBJECT_UP\n");
        tiny_rex_object.visible = WHITE;
        tiny_rex_object.state = EM_TINY_REX_STATE_JUMPING;
    }
    break;
    case EVENT_TINY_REX_OBJECT_DOWN:
    {
        APP_DBG_SIG("EVENT_TINY_REX_OBJECT_DOWN\n");
        tiny_rex_object.visible = WHITE;
        if (tiny_rex_object.state == EM_TINY_REX_STATE_JUMPING)
        {
            tiny_rex_object.state = EM_TINY_REX_STATE_FALLING;
        }
    }
    break;
    case EVENT_TINY_REX_OBJECT_DUCK:
    {
        APP_DBG_SIG("EVENT_TINY_REX_OBJECT_DUCK\n");
        tiny_rex_object.visible = WHITE;
        if (tiny_rex_object.state == EM_TINY_REX_STATE_RUNNING)
        {
            tiny_rex_object.state = EM_TINY_REX_STATE_DUCKING;
        }
    }
    break;

    default:
        break;
    }
}
static void update_trex_y(void)
{
    if (tiny_rex_object.state == EM_TINY_REX_STATE_JUMPING)
    {
        tiny_rex_object.y -= TREX_JUMP_SPEED;

        if (tiny_rex_object.y <= TREX_JUMP_TOP_Y)
        {
            tiny_rex_object.y = TREX_JUMP_TOP_Y;
            tiny_rex_object.state = EM_TINY_REX_STATE_FALLING;
        }
    }
    else if (tiny_rex_object.state == EM_TINY_REX_STATE_FALLING)
    {
        tiny_rex_object.y += TREX_JUMP_SPEED;

        if (tiny_rex_object.y >= TREX_GROUND_Y)
        {
            tiny_rex_object.y = TREX_GROUND_Y;
            tiny_rex_object.state = EM_TINY_REX_STATE_RUNNING;
        }
    }
}
static void update_trex_bitmap(void)
{
    if (tiny_rex_object.state == EM_TINY_REX_STATE_RUNNING || tiny_rex_object.state == EM_TINY_REX_STATE_DANCING)
    {
        tiny_rex_object.bitmap_index = (tiny_rex_object.bitmap_index == BITMAP_T_REX_RUN_1) ? BITMAP_T_REX_RUN_2 : BITMAP_T_REX_RUN_1;
    }
}
/* ************************************* End of File ******************************************** */
