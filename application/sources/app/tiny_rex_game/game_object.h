#ifndef __OBJECT_H_
#define __OBJECT_H_

#include <stdio.h>

typedef struct {
    bool visible;
    uint8_t state;
    uint8_t speed;
    int8_t x, y;
    uint8_t action_image;
    uint8_t bitmap_index;
} game_object_t;

#endif //__OBJECT_H_