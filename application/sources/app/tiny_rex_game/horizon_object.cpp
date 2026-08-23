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
#include "horizon_object.h"
//==================================================================================================
//	Local define
//==================================================================================================
/* Axis */
#define AXIS_X_LINE_OBJECT (0)
#define AXIS_Y_LINE_OBJECT (HEIGHT - g_bitmap_table[line_object.action_image].height)
/* Axis cloud */
#define AXIS_X_CLOUD_OBJECT_0 (12)
#define AXIS_X_CLOUD_OBJEC_GRAB_WITH_0 (30)

#define CLOUD_OBJECT_COUNT 2
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
game_object_t cloud_object[CLOUD_OBJECT_COUNT];
//==================================================================================================
//	Local ROM
//==================================================================================================

//==================================================================================================
//	Local Function Prototype
//==================================================================================================

//==================================================================================================
//	Source Code
//==================================================================================================
void horizon_object_handle(ak_msg_t* msg)
{
    switch (msg->sig)
    {
    case EVENT_HORIZON_OBJECT_PLAY:
    {
        /* line object */
        line_object.speed = obstacle_objects_speed;
        line_object.visible = WHITE;
        line_object.action_image = BITMAP_LINE;
        line_object.x = AXIS_X_LINE_OBJECT;
        line_object.y = AXIS_Y_LINE_OBJECT;

        /* Cloud */
        for (uint8_t i = 0; i < CLOUD_OBJECT_COUNT; i++)
        {
            cloud_object[i].speed = 1;
            cloud_object[i].visible = WHITE;
            cloud_object[i].action_image = BITMAP_GAME_CLOUD_ICON;
            cloud_object[i].x = (i == 0) ? 12 : 77;
            cloud_object[i].y = (i == 0) ? 4 : 9;
        }
    }
    break;

    case EVENT_HORIZON_OBJECT_UPDATE:
    {
        line_object.x -= line_object.speed;
        if (line_object.x <= -g_bitmap_table[BITMAP_LINE].width)
        {
            line_object.x = 0;
        }
        /* Cloud */
        for (uint8_t i = 0; i < CLOUD_OBJECT_COUNT; i++)
        {
            cloud_object[i].x -= cloud_object[i].speed;

            if (cloud_object[i].x <= -g_bitmap_table[BITMAP_GAME_CLOUD_ICON].width)
            {
                cloud_object[i].x = WIDTH;
            }
        }
    }
    break;

    case EVENT_HORIZON_INC_SPEED:
    {
        line_object.speed = obstacle_objects_speed;
    }
    break;

    default:
        break;
    }
}
void draw_horizon_objects(void)
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
        line_object.x + WIDTH,
        line_object.y,
        g_bitmap_table[BITMAP_LINE].bitmap,
        g_bitmap_table[BITMAP_LINE].width,
        g_bitmap_table[BITMAP_LINE].height,
        line_object.visible);
    /* Cloud */
    for (uint8_t i = 0; i < CLOUD_OBJECT_COUNT; i++)
    {
        view_render.drawBitmap(
            cloud_object[i].x,
            cloud_object[i].y,
            g_bitmap_table[BITMAP_GAME_CLOUD_ICON].bitmap,
            g_bitmap_table[BITMAP_GAME_CLOUD_ICON].width,
            g_bitmap_table[BITMAP_GAME_CLOUD_ICON].height,
            cloud_object[i].visible);
    }
}