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
    BITMAP_BIRD_1 = 0,
    BITMAP_BIRD_2,
    BITMAP_T_REX_1,
    BITMAP_T_REX_2,
    BITMAP_T_REX_RUN_1,
    BITMAP_T_REX_RUN_2,
    BITMAP_TREE_1,
    BITMAP_LINE,
    BITMAP_MAX
} bitmap_id_t;

extern const bitmap_info_t g_bitmap_table[BITMAP_MAX];

#endif //__SCREENS_BITMAP_H__
