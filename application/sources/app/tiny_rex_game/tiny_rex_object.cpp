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
/* Axis */
#define AXIS_X_TINY_REX_OBJECT (5)
#define AXIS_Y_TINY_REX_OBJECT (HEIGHT - g_bitmap_table[tiny_rex_object.action_image].height - 1)

#define TREX_JUMP_SPEED (5)
#define TREX_JUMP_TOP_Y (0)
#define TREX_GROUND_Y (AXIS_Y_TINY_REX_OBJECT)
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
static void tiny_rex_update(void);
/* state */
static void tiny_rex_init(void);

//==================================================================================================
//	Source Code
//==================================================================================================
void tiny_rex_object_handle(ak_msg_t* msg)
{
    switch (msg->sig)
    {
    case EVENT_TINY_REX_SETUP:
    case EVENT_TINY_REX_PLAY:
    {
        tiny_rex_object.action_image = BITMAP_T_REX_RUN_1;
        tiny_rex_object.x = AXIS_X_TINY_REX_OBJECT;
        tiny_rex_object.y = AXIS_Y_TINY_REX_OBJECT;
        tiny_rex_object.visible = WHITE;
        tiny_rex_object.speed = TREX_JUMP_SPEED;
        tiny_rex_object.state = EM_TINY_REX_STATE_RUN;
    }
    break;

    case EVENT_BUTTON_UP_PRESS:
    {
        if (tiny_rex_object.state == EM_TINY_REX_STATE_RUN)
        {
            tiny_rex_object.state = EM_TINY_REX_STATE_JUMP;
        }
    }
    break;

    case EVENT_BUTTON_DOWN_PRESS:
    {
        if (tiny_rex_object.state == EM_TINY_REX_STATE_JUMP)
        {
            tiny_rex_object.state = EM_TINY_REX_STATE_FALL;
        }
    }
    break;

    case EVENT_BUTTON_MODE_PRESS:
    {
        if (tiny_rex_object.state == EM_TINY_REX_STATE_RUN)
        {
            tiny_rex_object.state = EM_TINY_REX_STATE_BEND_OVER;
            tiny_rex_object.action_image = BITMAP_T_REX_DUCKING_1;
            tiny_rex_object.y = AXIS_Y_TINY_REX_OBJECT;
        }
    }
    break;

    case EVENT_BUTTON_MODE_RELEASE:
    {
        if (tiny_rex_object.state == EM_TINY_REX_STATE_BEND_OVER)
        {
            tiny_rex_object.state = EM_TINY_REX_STATE_RUN;
            tiny_rex_object.action_image = BITMAP_T_REX_RUN_1;
            tiny_rex_object.y = AXIS_Y_TINY_REX_OBJECT;
        }
    }
    break;

    case EVENT_TINY_REX_UPDATE:
    {
        tiny_rex_update();
    }
    break;

    default:
        break;
    }
}
void draw_tiny_rex_object(void)
{
    // Draw bit-map of Tiny-Rex
    view_render.drawBitmap(
        tiny_rex_object.x,
        tiny_rex_object.y,
        g_bitmap_table[tiny_rex_object.action_image].bitmap,
        g_bitmap_table[tiny_rex_object.action_image].width,
        g_bitmap_table[tiny_rex_object.action_image].height,
        tiny_rex_object.visible);
}
static void tiny_rex_init(void)
{
    tiny_rex_object.action_image = BITMAP_T_REX_RUN_1;
    tiny_rex_object.x = AXIS_X_TINY_REX_OBJECT;
    tiny_rex_object.y = AXIS_Y_TINY_REX_OBJECT;
    tiny_rex_object.visible = WHITE;
    tiny_rex_object.speed = TREX_JUMP_SPEED;
    tiny_rex_object.state = EM_TINY_REX_STATE_RUN;
}
static void tiny_rex_update(void)
{
    switch (tiny_rex_object.state)
    {
    case EM_TINY_REX_STATE_RUN:
    {
        tiny_rex_object.action_image =
            (tiny_rex_object.action_image == BITMAP_T_REX_RUN_1)
                ? BITMAP_T_REX_RUN_2
                : BITMAP_T_REX_RUN_1;
    }
    break;

    case EM_TINY_REX_STATE_JUMP:
    {
        tiny_rex_object.action_image = BITMAP_T_REX_STAND;
        tiny_rex_object.y -= TREX_JUMP_SPEED;

        if (tiny_rex_object.y <= TREX_JUMP_TOP_Y)
        {
            tiny_rex_object.y = TREX_JUMP_TOP_Y;
            tiny_rex_object.state = EM_TINY_REX_STATE_FALL;
        }
    }
    break;

    case EM_TINY_REX_STATE_FALL:
    {
        tiny_rex_object.action_image = BITMAP_T_REX_STAND;
        tiny_rex_object.y += TREX_JUMP_SPEED;

        if (tiny_rex_object.y >= TREX_GROUND_Y)
        {
            tiny_rex_object.y = TREX_GROUND_Y;
            tiny_rex_object.state = EM_TINY_REX_STATE_RUN;
        }
    }
    break;

    case EM_TINY_REX_STATE_BEND_OVER:
    {
        tiny_rex_object.action_image =
            (tiny_rex_object.action_image == BITMAP_T_REX_DUCKING_1)
                ? BITMAP_T_REX_DUCKING_2
                : BITMAP_T_REX_DUCKING_1;
    }
    break;

    default:
        break;
    }
}
/* ************************************* End of File ******************************************** */
