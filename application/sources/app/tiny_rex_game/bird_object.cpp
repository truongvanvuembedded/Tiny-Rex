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
//==================================================================================================
//	Local ROM
//==================================================================================================

//==================================================================================================
//	Local Function Prototype
//==================================================================================================
static void bird_init(void);
static void bird_fly(void);
static void bird_update(void);
static void bird_stand_fly(void);
static void bird_over(void);
/* Helper function */
static void update_position(void);
static void update_animation(void);
//==================================================================================================
//	Source Code
//==================================================================================================
void bird_object_handle(ak_msg_t* msg)
{
    switch (msg->sig)
    {
    case EVENT_BIRD_OBJECT_SETUP:
        bird_init();
        break;

    case EVENT_BIRD_OBJECT_FLY:
        bird_fly();
        break;

    case EVENT_BIRD_OBJECT_UPDATE:
        bird_update();
        break;

    case EVENT_BIRD_OBJECT_STAND_FLY:
        bird_stand_fly();
        break;

    case EVENT_BIRD_OBJECT_GAME_OVER:
        bird_over();
        break;

    default:
        break;
    }
}
void draw_bird_object(void)
{
    // Draw bit-map of bird
    view_render.drawBitmap(
        bird_object.x,
        bird_object.y,
        g_bitmap_table[bird_object.bitmap_index].bitmap,
        g_bitmap_table[bird_object.bitmap_index].width,
        g_bitmap_table[bird_object.bitmap_index].height,
        bird_object.visible);
}
static void bird_init(void)
{
    bird_object.x = AXIS_X_BIRD_OBJECT;
    bird_object.y = AXIS_Y_BIRD_OBJECT;
    bird_object.visible = BLACK;
    bird_object.state = EM_BIRD_STATE_IDLE;
    bird_object.bitmap_index = BITMAP_TREE_1;
    bird_object.speed = 4;
}
static void bird_fly(void)
{
    bird_init();
    bird_object.visible = WHITE;
    bird_object.state = EM_BIRD_STATE_FLYING;
}
static void bird_update(void)
{
    update_position();
    update_animation();
}
static void update_position(void)
{
    if (bird_object.state != EM_BIRD_STATE_FLYING)
    {
        return;
    }
    bird_object.x -= bird_object.speed;
    if (bird_object.x <= -BIRD_WIDTH_IN_RUN)
    {
        bird_object.x = AXIS_X_BIRD_OBJECT;
    }
}
static void update_animation(void)
{
    bird_object.bitmap_index =
        (bird_object.bitmap_index == BITMAP_BIRD_1)
            ? BITMAP_BIRD_2
            : BITMAP_BIRD_1;
}
static void bird_stand_fly(void)
{
    bird_object.x = AXIS_X_BIRD_OBJECT_IN_MENU_SCREEN;
    bird_object.y = AXIS_Y_BIRD_OBJECT_IN_MENU_SCREEN;
}
static void bird_over(void)
{
    bird_init();
}