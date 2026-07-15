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
#include "line_object.h"
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
game_object_t line_object;
//==================================================================================================
//	Local ROM
//==================================================================================================

//==================================================================================================
//	Local Function Prototype
//==================================================================================================
static void line_init(void);
static void line_start(void);
static void line_update(void);
static void line_over(void);
//==================================================================================================
//	Source Code
//==================================================================================================
void line_object_handle(ak_msg_t* msg)
{
    switch (msg->sig)
    {
    case EVENT_LINE_OBJECT_SETUP:
        line_init();
        break;

    case EVENT_LINE_OBJECT_START:
        line_start();
        break;

    case EVENT_LINE_OBJECT_UPDATE:
        line_update();
        break;

    case EVENT_LINE_OBJECT_GAME_OVER:
        line_over();
        break;

    default:
        break;
    }
}
void draw_line_object(void)
{
    // Draw bit-map of line
    view_render.drawBitmap(
        line_object.x,
        line_object.y,
        g_bitmap_table[BITMAP_LINE].bitmap,
        g_bitmap_table[BITMAP_LINE].width,
        g_bitmap_table[BITMAP_LINE].height,
        line_object.visible);

    view_render.drawBitmap(
        line_object.x + GROUND_WIDTH,
        line_object.y,
        g_bitmap_table[BITMAP_LINE].bitmap,
        g_bitmap_table[BITMAP_LINE].width,
        g_bitmap_table[BITMAP_LINE].height,
        line_object.visible);
}
static void line_init(void)
{
    line_object.x = AXIS_X_LINE_OBJECT;
    line_object.y = AXIS_Y_LINE_OBJECT;
    line_object.visible = WHITE;
    line_object.state = EM_LINE_STATE_IDLE;
    line_object.bitmap_index = BITMAP_LINE;
    line_object.speed = 4;
}
static void line_start(void)
{
    line_init();
    line_object.state = EM_LINE_STATE_RUNNING;
}
static void line_update(void)
{
    if (line_object.state != EM_LINE_STATE_RUNNING)
    {
        return;
    }
    line_object.x -= line_object.speed;

    if (line_object.x <= -GROUND_WIDTH)
    {
        line_object.x = 0;
    }
}
static void line_over(void)
{
    line_init();
}