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
/* Y axis when tiny rex ducking */
#define AXIS_Y_TINY_REX_OBJECT_DUCKING (49)
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
static void update_position(void);
static void update_animation(void);
/* state */
static void tiny_rex_init(void);
static void tiny_rex_run(void);
static void tiny_rex_update(void);
static void tiny_rex_jump(void);
static void tiny_rex_fall(void);
static void tiny_rex_duck(void);
static void tiny_rex_duck_release(void);
static void tiny_rex_over(void);

//==================================================================================================
//	Source Code
//==================================================================================================
void tiny_rex_object_handle(ak_msg_t* msg)
{
    switch (msg->sig)
    {
    case EVENT_TINY_REX_OBJECT_SETUP:
        tiny_rex_init();
        break;

    case EVENT_TINY_REX_OBJECT_RUN:
        tiny_rex_run();
        break;

    case EVENT_TINY_REX_OBJECT_UPDATE:
        tiny_rex_update();
        break;

    case EVENT_TINY_REX_OBJECT_UP:
        tiny_rex_jump();
        break;

    case EVENT_TINY_REX_OBJECT_DOWN:
        tiny_rex_fall();
        break;

    case EVENT_TINY_REX_OBJECT_DUCK:
        tiny_rex_duck();
        break;

    case EVENT_TINY_REX_OBJECT_DUCK_RELEAASE:
        tiny_rex_duck_release();
        break;

    case EVENT_TINY_REX_OBJECT_GAME_OVER:
        tiny_rex_over();
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
        g_bitmap_table[tiny_rex_object.bitmap_index].bitmap,
        g_bitmap_table[tiny_rex_object.bitmap_index].width,
        g_bitmap_table[tiny_rex_object.bitmap_index].height,
        tiny_rex_object.visible);
}
static void tiny_rex_init(void)
{

    tiny_rex_object.x = AXIS_X_TINY_REX_OBJECT;
    tiny_rex_object.y = AXIS_Y_TINY_REX_OBJECT;
    tiny_rex_object.visible = BLACK;
    tiny_rex_object.state = EM_TINY_REX_STATE_IDLE;
    tiny_rex_object.bitmap_index = BITMAP_T_REX_STAND;
}
static void tiny_rex_run(void)
{
    tiny_rex_init();
    tiny_rex_object.visible = WHITE;
    tiny_rex_object.state = EM_TINY_REX_STATE_RUNNING;
    tiny_rex_object.bitmap_index = BITMAP_T_REX_RUN_1;
}
static void tiny_rex_jump(void)
{

    if (tiny_rex_object.state == EM_TINY_REX_STATE_RUNNING)
    {
        tiny_rex_object.state = EM_TINY_REX_STATE_JUMPING;
    }
}
static void tiny_rex_fall(void)
{
    if (tiny_rex_object.state == EM_TINY_REX_STATE_JUMPING)
    {
        tiny_rex_object.state = EM_TINY_REX_STATE_FALLING;
    }
}
static void tiny_rex_duck(void)
{
    if (tiny_rex_object.state == EM_TINY_REX_STATE_RUNNING)
    {
        tiny_rex_object.state = EM_TINY_REX_STATE_DUCKING;
        tiny_rex_object.y = AXIS_Y_TINY_REX_OBJECT_DUCKING;
        tiny_rex_object.bitmap_index = BITMAP_T_REX_DUCKING_1;
    }
}
static void tiny_rex_duck_release(void)
{
    if (tiny_rex_object.state == EM_TINY_REX_STATE_DUCKING)
    {
        tiny_rex_object.state = EM_TINY_REX_STATE_RUNNING;
        tiny_rex_object.y = AXIS_Y_TINY_REX_OBJECT;
        tiny_rex_object.bitmap_index = BITMAP_T_REX_RUN_1;
    }
}
static void tiny_rex_over(void)
{
    tiny_rex_init();
}
static void tiny_rex_update(void)
{
    update_position();
    update_animation();
}
static void update_position(void)
{
    switch (tiny_rex_object.state)
    {
    case EM_TINY_REX_STATE_JUMPING:
    {
        tiny_rex_object.y -= TREX_JUMP_SPEED;

        if (tiny_rex_object.y <= TREX_JUMP_TOP_Y)
        {
            tiny_rex_object.y = TREX_JUMP_TOP_Y;
            tiny_rex_object.state = EM_TINY_REX_STATE_FALLING;
        }
    }
    break;

    case EM_TINY_REX_STATE_FALLING:
    {
        tiny_rex_object.y += TREX_JUMP_SPEED;

        if (tiny_rex_object.y >= TREX_GROUND_Y)
        {
            tiny_rex_object.y = TREX_GROUND_Y;
            tiny_rex_object.state = EM_TINY_REX_STATE_RUNNING;
        }
    }
    break;

    default:
        break;
    }
}
static void update_animation(void)
{
    switch (tiny_rex_object.state)
    {
    case EM_TINY_REX_STATE_RUNNING:
    {
        tiny_rex_object.bitmap_index =
            (tiny_rex_object.bitmap_index == BITMAP_T_REX_RUN_1)
                ? BITMAP_T_REX_RUN_2
                : BITMAP_T_REX_RUN_1;
    }
    break;

    case EM_TINY_REX_STATE_JUMPING:
    case EM_TINY_REX_STATE_FALLING:
    {
        tiny_rex_object.bitmap_index = BITMAP_T_REX_STAND;
    }
    break;

    case EM_TINY_REX_STATE_DUCKING:
    {
        tiny_rex_object.bitmap_index =
            (tiny_rex_object.bitmap_index == BITMAP_T_REX_DUCKING_1)
                ? BITMAP_T_REX_DUCKING_2
                : BITMAP_T_REX_DUCKING_1;
    }
    break;

    default:
        break;
    }
}
/* ************************************* End of File ******************************************** */
