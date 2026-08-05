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
/* Axis */
#define AXIS_Y_TREE_OBJECT (HEIGHT - g_bitmap_table[tree_object.action_image].height - 1)
/* Menu screen */
#define AXIS_X_TREE_OBJECT_IN_MENU_SCREEN (104)
#define AXIS_Y_TREE_OBJECT_IN_MENU_SCREEN AXIS_Y_TREE_OBJECT

/* Spawn delay in game ticks */
#define TREE_SPAWN_DELAY_MIN_TICK (1000 / 50)
#define TREE_SPAWN_DELAY_MAX_TICK (3000 / 50)
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
static uint32_t tree_hidden_timer;
//==================================================================================================
//	Local ROM
//==================================================================================================

//==================================================================================================
//	Local Function Prototype
//==================================================================================================
static void tree_update(void);
//==================================================================================================
//	Source Code
//==================================================================================================
void tree_object_handle(ak_msg_t* msg)
{
    switch (msg->sig)
    {
    case EVENT_TREE_OBJECT_SETUP:
    {
        tree_object.action_image = BITMAP_TREE_1;
        tree_object.x = AXIS_X_TREE_OBJECT_IN_MENU_SCREEN;
        tree_object.y = AXIS_Y_TREE_OBJECT_IN_MENU_SCREEN;
        tree_object.visible = WHITE;
        tree_object.state = EM_TREE_STATE_STAND;
        tree_object.speed = 4;
    }
    break;

    case EVENT_TREE_OBJECT_PLAY:
    {
        tree_object.visible = BLACK;
        tree_object.state = EM_TREE_STATE_HIDDEN;
        tree_hidden_timer = random(TREE_SPAWN_DELAY_MIN_TICK, TREE_SPAWN_DELAY_MAX_TICK);
    }
    break;

    case EVENT_TREE_OBJECT_UPDATE:
        tree_update();
    break;

    default:
        break;
    }
}
void draw_tree_object(void)
{
    if(tree_object.visible == BLACK)
        return;
    // Draw bit-map of tree
    view_render.drawBitmap(
        tree_object.x,
        tree_object.y,
        g_bitmap_table[tree_object.action_image].bitmap,
        g_bitmap_table[tree_object.action_image].width,
        g_bitmap_table[tree_object.action_image].height,
        tree_object.visible);
}
static void tree_update(void)
{
    if (tree_object.state == EM_TREE_STATE_HIDDEN)
    {
        if (tree_hidden_timer > 0)
        {
            tree_hidden_timer--;
            return;
        }
        /* Only display tree when bird is not moving */
        if(bird_object.state == EM_BIRD_STATE_FLY)
        {
            return;
        }
        tree_object.visible = WHITE;
        tree_object.state = EM_TREE_STATE_MOVE;
        tree_object.x = WIDTH;
        tree_object.y = AXIS_Y_TREE_OBJECT;
        /* Change action iamge */
        tree_object.action_image = random(BITMAP_TREE_1, BITMAP_TREE_4+1);
    }
    else if (tree_object.state == EM_TREE_STATE_MOVE)
    {
        /* Update position */
        tree_object.x -= tree_object.speed;
        tree_object.y = AXIS_Y_TREE_OBJECT;
        if (tree_object.x <= 0-g_bitmap_table[tree_object.action_image].width)
        {
            tree_object.visible = BLACK;
            tree_object.state = EM_TREE_STATE_HIDDEN;
            tree_hidden_timer = random(TREE_SPAWN_DELAY_MIN_TICK, TREE_SPAWN_DELAY_MAX_TICK);
        }
    }
}