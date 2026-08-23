#ifndef __SCREENS_BITMAP_H__
#define __SCREENS_BITMAP_H__

#include "view_render.h"

typedef struct
{
    const unsigned char *bitmap;
    uint16_t width;
    uint16_t height;
} bitmap_info_t;

typedef enum
{
    BITMAP_T_REX_STAND = 0,
    BITMAP_T_REX_DUCKING_1,
    BITMAP_T_REX_DUCKING_2,
    BITMAP_T_REX_RUN_1,
    BITMAP_T_REX_RUN_2,
    BITMAP_BIRD_1,
    BITMAP_BIRD_2,
    BITMAP_TREE_1,
    BITMAP_TREE_2,
    BITMAP_TREE_3,
    BITMAP_TREE_4,
    BITMAP_LINE,
    BITMAP_GAME_OVER_ICON,
    BITMAP_GAME_CLOUD_ICON,
    BITMAP_GAME_SETTING_ICON,
    BITMAP_GAME_RANKING_ICON,
    BITMAP_GAME_EXIT_ICON,
    BITMAP_MAX
} bitmap_id_t;

extern const bitmap_info_t g_bitmap_table[BITMAP_MAX];

#endif //__SCREENS_BITMAP_H__
