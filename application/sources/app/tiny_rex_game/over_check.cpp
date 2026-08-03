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
#include "over_check.h"
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
game_object_t over_check_object;
//==================================================================================================
//	Local ROM
//==================================================================================================

//==================================================================================================
//	Local Function Prototype
//==================================================================================================
static bool object_collision(const game_object_t *obj1,
                             const bitmap_info_t *bmp1,
                             const game_object_t *obj2,
                             const bitmap_info_t *bmp2);

//==================================================================================================
//	Source Code
//==================================================================================================
void over_check_handle(ak_msg_t* msg)
{
    switch (msg->sig)
    {
    case EVENT_OVER_CHECK_SETUP:
    {
        over_check_object.visible = BLACK;
        over_check_object.state = EM_OVER_CHECK_STATE_IDLE;
    }
    break;

    case EVENT_OVER_CHECK_COLLISTION_DETECT:
    {
        over_check_object.state = EM_OVER_CHECK_STATE_RUN;
    }
    break;

    case EVENT_OVER_CHECK_UPDATE:
    {
        if (over_check_object.state == EM_OVER_CHECK_STATE_IDLE)
        {
            return;
        }

        /* Rex <-> Bird */
        uint8_t rex_bird_collistion = object_collision(&tiny_rex_object,
                            &g_bitmap_table[tiny_rex_object.action_image],
                            &bird_object,
                            &g_bitmap_table[bird_object.action_image]);
        /* Rex <-> Tree */
        uint8_t rex_tree_collistion = object_collision(&tiny_rex_object,
                            &g_bitmap_table[tiny_rex_object.action_image],
                            &tree_object,
                            &g_bitmap_table[tree_object.action_image]);
        if (rex_tree_collistion || rex_bird_collistion)
        {
            /* Game Over */
            over_check_object.state = EM_OVER_CHECK_STATE_FAULT;
            over_check_object.visible = WHITE;
            over_check_object.x = 46;
            over_check_object.y = 16;
            over_check_object.action_image = BITMAP_GAME_OVER_ICON;
            task_post_pure_msg(AC_TASK_DISPLAY_ID, EVENT_TINY_REX_GAME_OVER);
        }
    }
    break;

    default:
        break;
    }
}
void draw_over_icon(void)
{
    // Draw bit-map of over check icon
    view_render.drawBitmap(
        over_check_object.x,
        over_check_object.y,
        g_bitmap_table[over_check_object.action_image].bitmap,
        g_bitmap_table[over_check_object.action_image].width,
        g_bitmap_table[over_check_object.action_image].height,
        over_check_object.visible);
}
static bool object_collision(const game_object_t *obj1,
                             const bitmap_info_t *bmp1,
                             const game_object_t *obj2,
                             const bitmap_info_t *bmp2)
{
    if ((!obj1->visible) || (!obj2->visible))
    {
        return false;
    }

    return !( (obj1->x + bmp1->width  <= obj2->x) ||
              (obj2->x + bmp2->width  <= obj1->x) ||
              (obj1->y + bmp1->height <= obj2->y) ||
              (obj2->y + bmp2->height <= obj1->y) );
}