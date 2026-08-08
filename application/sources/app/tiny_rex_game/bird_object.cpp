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
#include "bird_object.h"
//==================================================================================================
//	Local define
//==================================================================================================
/* Axis */
#define AXIS_X_BIRD_OBJECT_INIT (64)
/* Menu screen */
#define AXIS_X_BIRD_OBJECT_IN_MENU_SCREEN (50)
#define AXIS_Y_BIRD_OBJECT_IN_MENU_SCREEN (HEIGHT - g_bitmap_table[bird_object.action_image].height)
/* Spawn delay in game ticks */
#define BIRD_SPAWN_DELAY_MIN_TICK (1000 / 50)
#define BIRD_SPAWN_DELAY_MAX_TICK (3000 / 50)
/* Spawn axis in game ticks */
#define AXIS_Y_BIRD_OBJECT_MIN (5)
#define AXIS_Y_BIRD_OBJECT_MAX (HEIGHT - 20)
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
game_object_t bird_object;
static uint32_t hidden_timer;
//==================================================================================================
//	Local ROM
//==================================================================================================

//==================================================================================================
//	Local Function Prototype
//==================================================================================================
static void bird_update(void);
//==================================================================================================
//	Source Code
//==================================================================================================
void bird_object_handle(ak_msg_t* msg)
{
    switch (msg->sig)
    {
    case EVENT_BIRD_OBJECT_SETUP:
    {
        bird_object.x = AXIS_X_BIRD_OBJECT_IN_MENU_SCREEN;
        bird_object.y = AXIS_Y_BIRD_OBJECT_IN_MENU_SCREEN;
        bird_object.visible = WHITE;
        bird_object.state = EM_BIRD_STATE_STAND_FLY;
        bird_object.action_image = BITMAP_BIRD_1;
        bird_object.speed = 4;
    }
    break;

    case EVENT_BIRD_OBJECT_PLAY:
    {
        bird_object.visible = BLACK;
        bird_object.state = EM_BIRD_STATE_HIDDEN_FLY;
        hidden_timer = random(BIRD_SPAWN_DELAY_MIN_TICK, BIRD_SPAWN_DELAY_MAX_TICK);
    }
    break;

    case EVENT_BIRD_OBJECT_UPDATE:
        bird_update();
        break;

    default:
        break;
    }
}
void draw_bird_object(void)
{
    if(bird_object.visible == BLACK)
        return;
    // Draw bit-map of bird
    view_render.drawBitmap(
        bird_object.x,
        bird_object.y,
        g_bitmap_table[bird_object.action_image].bitmap,
        g_bitmap_table[bird_object.action_image].width,
        g_bitmap_table[bird_object.action_image].height,
        bird_object.visible);
}
static void bird_update(void)
{
    if (bird_object.state == EM_BIRD_STATE_HIDDEN_FLY)
    {
        if (hidden_timer > 0)
        {
            hidden_timer--;
            return;
        }
        /* Only display bird when tree is not moving */
        if(tree_object.state == EM_TREE_STATE_MOVE)
        {
            return;
        }
        bird_object.visible = WHITE;
        bird_object.state = EM_BIRD_STATE_FLY;
        bird_object.x = WIDTH;
        bird_object.y = random(AXIS_Y_BIRD_OBJECT_MIN, AXIS_Y_BIRD_OBJECT_MAX);
        return;
    }

    if (bird_object.state == EM_BIRD_STATE_FLY)
    {
        /* Update position */
        bird_object.x -= bird_object.speed;
        if (bird_object.x <= 0-g_bitmap_table[BITMAP_BIRD_1].width)
        {
            bird_object.visible = BLACK;
            bird_object.state = EM_BIRD_STATE_HIDDEN_FLY;
            hidden_timer = random(BIRD_SPAWN_DELAY_MIN_TICK, BIRD_SPAWN_DELAY_MAX_TICK);
        }
    }
    /* Update animation */
    bird_object.action_image =
        (bird_object.action_image == BITMAP_BIRD_1)
            ? BITMAP_BIRD_2
            : BITMAP_BIRD_1;
}