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
#include "tree_object.h"
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
game_object_t tree_object;
//==================================================================================================
//	Local ROM
//==================================================================================================

//==================================================================================================
//	Local Function Prototype
//==================================================================================================
static void tree_init(void);
static void tree_start(void);
static void tree_update(void);
static void tree_stand(void);
static void tree_over(void);
//==================================================================================================
//	Source Code
//==================================================================================================
void tree_object_handle(ak_msg_t* msg)
{
    switch (msg->sig)
    {
    case EVENT_TREE_OBJECT_SETUP:
        tree_init();
        break;

    case EVENT_TREE_OBJECT_START:
        tree_start();
        break;

    case EVENT_TREE_OBJECT_UPDATE:
        tree_update();
        break;

    case EVENT_TREE_OBJECT_STAND:
        tree_stand();
        break;

    case EVENT_TREE_OBJECT_GAME_OVER:
        tree_over();
        break;

    default:
        break;
    }
}
void draw_tree_object(void)
{
    // Draw bit-map of tree
    view_render.drawBitmap(
        tree_object.x,
        tree_object.y,
        g_bitmap_table[BITMAP_TREE_1].bitmap,
        g_bitmap_table[BITMAP_TREE_1].width,
        g_bitmap_table[BITMAP_TREE_1].height,
        tree_object.visible);
}
static void tree_init(void)
{
    tree_object.x = AXIS_X_TREE_OBJECT;
    tree_object.y = AXIS_Y_TREE_OBJECT;
    tree_object.visible = WHITE;
    tree_object.state = EM_TREE_STATE_IDLE;
    tree_object.bitmap_index = BITMAP_TREE_1;
    tree_object.speed = 4;
}
static void tree_start(void)
{
    tree_init();
    tree_object.state = EM_TREE_STATE_RUNNING;
}
static void tree_update(void)
{
    if (tree_object.state != EM_TREE_STATE_RUNNING)
    {
        return;
    }
    tree_object.x -= tree_object.speed;
    if (tree_object.x <= -TREE_WIDTH_IN_RUN)
    {
        tree_object.x = AXIS_X_TREE_OBJECT;
    }
}
static void tree_stand(void)
{
    tree_object.x = AXIS_X_TREE_OBJECT_IN_MENU_SCREEN;
    tree_object.y = AXIS_Y_TREE_OBJECT_IN_MENU_SCREEN;
}
static void tree_over(void)
{
    tree_init();
}