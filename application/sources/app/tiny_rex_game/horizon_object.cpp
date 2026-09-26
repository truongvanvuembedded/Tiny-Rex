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
#include "tsm.h"
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
/* TSM action */
static void horizon_on_play(ak_msg_t* msg);
static void horizon_on_update(ak_msg_t* msg);
static void horizon_on_inc_speed(ak_msg_t* msg);
//==================================================================================================
//	State Table (TSM)
//	Horizon has only one state, so the table is a signal -> action map.
//==================================================================================================
typedef enum
{
    EM_HORIZON_STATE_ACTIVE = 0,
} EM_HORIZON_STATE;

static tsm_t horizon_tsm_active[] =
    {
        {HORIZON_OBJECT_PLAY_EVENT, TSM_NULL_STATE, horizon_on_play},
        {HORIZON_OBJECT_UPDATE_EVENT, TSM_NULL_STATE, horizon_on_update},
        {HORIZON_INC_SPEED_EVENT, TSM_NULL_STATE, horizon_on_inc_speed},
        {TSM_NULL_MSG, TSM_NULL_STATE, TSM_NULL_ROUTINE},
};

/* Index of this table MUST be the same as EM_HORIZON_STATE */
static tsm_t* horizon_tsm_table[] =
    {
        horizon_tsm_active,
};

static tsm_tbl_t horizon_tsm =
    {
        EM_HORIZON_STATE_ACTIVE,
        TSM_NULL_ON_STATE,
        horizon_tsm_table,
};
//==================================================================================================
//	Source Code
//==================================================================================================
void horizon_object_handle(ak_msg_t* msg)
{
    tsm_dispatch(&horizon_tsm, msg);
}

static void horizon_on_play(ak_msg_t* msg)
{
    (void)msg;
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

static void horizon_on_update(ak_msg_t* msg)
{
    (void)msg;
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

static void horizon_on_inc_speed(ak_msg_t* msg)
{
    (void)msg;
    line_object.speed = obstacle_objects_speed;
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