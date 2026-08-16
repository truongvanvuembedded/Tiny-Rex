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
/* Y Axis position */
#define AXIS_Y_TINY_REX_OBJECT_TOP (0)
#define AXIS_Y_TINY_REX_OBJECT_UPDATE (HEIGHT - g_bitmap_table[tiny_rex_object.action_image].height - 1)
/* Axis */
#define AXIS_X_TINY_REX_OBJECT_INIT (5)
#define AXIS_Y_TINY_REX_OBJECT_INIT (AXIS_Y_TINY_REX_OBJECT_UPDATE)
//==================================================================================================
//	Local define I/O
//==================================================================================================

//==================================================================================================
//	Local Struct Template
//==================================================================================================
/* Level */
typedef enum
{
    TREX_LEVEL_L0_START = 0,
    TREX_LEVEL_L1_EASY,
    TREX_LEVEL_L2_NORMAL,
    TREX_LEVEL_L3_HARD,
    TREX_LEVEL_L4_EXTREME,
    TREX_LEVEL_MAX,
} TREX_LEVEL;

/* Speed */
typedef struct
{
    uint8_t u1_JumpSpeed;
    uint8_t u1_FallSpeed;
    uint8_t u1_FastFallSpeed;
} ST_TREX_SPEED;
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
/* Speed table */
static const ST_TREX_SPEED g_st_TrexSpeed[TREX_LEVEL_MAX] =
{
    /* L0 - Start */
    {
        .u1_JumpSpeed    = 6,
        .u1_FallSpeed    = 3,
        .u1_FastFallSpeed = 6
    },

    /* L1 - Easy */
    {
        .u1_JumpSpeed    = 6,
        .u1_FallSpeed    = 3,
        .u1_FastFallSpeed = 6
    },

    /* L2 - Normal */
    {
        .u1_JumpSpeed    = 7,
        .u1_FallSpeed    = 4,
        .u1_FastFallSpeed = 7
    },

    /* L3 - Hard */
    {
        .u1_JumpSpeed    = 8,
        .u1_FallSpeed    = 5,
        .u1_FastFallSpeed = 8
    },

    /* L4 - Extreme */
    {
        .u1_JumpSpeed    = 9,
        .u1_FallSpeed    = 6,
        .u1_FastFallSpeed = 9
    }
};
//==================================================================================================
//	Local Function Prototype
//==================================================================================================
static void tiny_rex_update(void);
//==================================================================================================
//	Source Code
//==================================================================================================
void tiny_rex_object_handle(ak_msg_t* msg)
{
    switch (msg->sig)
    {
    case TINY_REX_PLAY_EVENT:
    {
        /* State running */
        tiny_rex_object.state = EM_TINY_REX_STATE_RUNNING;
        /* Action image */
        tiny_rex_object.visible = WHITE;
        tiny_rex_object.action_image = BITMAP_T_REX_RUN_1;
        /* Initial position */
        tiny_rex_object.x = AXIS_X_TINY_REX_OBJECT_INIT;
        tiny_rex_object.y = AXIS_Y_TINY_REX_OBJECT_INIT;
        /* Difficult level */
        tiny_rex_object.level = TREX_LEVEL_L0_START;
    }
    break;

    case TINY_REX_JUMP_EVENT:
    {
        if (tiny_rex_object.state == EM_TINY_REX_STATE_RUNNING)
        {
            /* Change jump speed */
            tiny_rex_object.speed = g_st_TrexSpeed[tiny_rex_object.level].u1_JumpSpeed;
            /* Change action image and state */
            tiny_rex_object.state = EM_TINY_REX_STATE_JUMPING;
            tiny_rex_object.action_image = BITMAP_T_REX_STAND;
        }
    }
    break;

    case TINY_REX_FALL_EVENT:
    {
        if (tiny_rex_object.state == EM_TINY_REX_STATE_JUMPING || tiny_rex_object.state == EM_TINY_REX_STATE_FALLING)
        {
            /* Change fall speed */
            tiny_rex_object.speed = g_st_TrexSpeed[tiny_rex_object.level].u1_FastFallSpeed;
            /* Change action image and state */
            tiny_rex_object.state = EM_TINY_REX_STATE_FALLING;
            tiny_rex_object.action_image = BITMAP_T_REX_STAND;
        }
    }
    break;

    case TINY_REX_DUCK_EVENT:
    {
        if (tiny_rex_object.state == EM_TINY_REX_STATE_RUNNING)
        {
            /* Changee action image and state */
            tiny_rex_object.state = EM_TINY_REX_STATE_DUCKING;
            tiny_rex_object.action_image = BITMAP_T_REX_DUCKING_1;
            /* Update position for new image */
            tiny_rex_object.y = AXIS_Y_TINY_REX_OBJECT_UPDATE;
        }
    }
    break;

    case TINY_REX_DUCK_RELEASE_EVENT:
    {
        if (tiny_rex_object.state == EM_TINY_REX_STATE_DUCKING)
        {
            /* Changee action image and state */
            tiny_rex_object.state = EM_TINY_REX_STATE_RUNNING;
            tiny_rex_object.action_image = BITMAP_T_REX_RUN_1;
            /* Update position for new image */
            tiny_rex_object.y = AXIS_Y_TINY_REX_OBJECT_UPDATE;
        }
    }
    break;

    case TINY_REX_INC_SPEED_EVENT:
    {
        tiny_rex_object.level++;
        if (tiny_rex_object.level >= TREX_LEVEL_MAX)
        {
            tiny_rex_object.level = TREX_LEVEL_L4_EXTREME;
        }
    }
    break;

    case TINY_REX_MOVE_EVENT:
    {
        tiny_rex_update();
    }
    break;

    default:
        break;
    }
}
void draw_tiny_rex_object(void)
{
    if(tiny_rex_object.visible == BLACK)
        return;
    /* Clear image before write for avoid back-ground over write to object */
    view_render.fillRoundRect(
        tiny_rex_object.x,
        tiny_rex_object.y,
        (g_bitmap_table[tiny_rex_object.action_image].width) + ((tiny_rex_object.state == EM_TINY_REX_STATE_DUCKING)?2:0),
        g_bitmap_table[tiny_rex_object.action_image].height,
        0,
        BLACK);
    // Draw bit-map of Tiny-Rex
    view_render.drawBitmap(
        tiny_rex_object.x,
        tiny_rex_object.y,
        g_bitmap_table[tiny_rex_object.action_image].bitmap,
        g_bitmap_table[tiny_rex_object.action_image].width,
        g_bitmap_table[tiny_rex_object.action_image].height,
        WHITE);
}
static void tiny_rex_update(void)
{
    switch (tiny_rex_object.state)
    {
    case EM_TINY_REX_STATE_RUNNING:
    {
        tiny_rex_object.action_image =
            (tiny_rex_object.action_image == BITMAP_T_REX_RUN_1)
                ? BITMAP_T_REX_RUN_2
                : BITMAP_T_REX_RUN_1;
    }
    break;

    case EM_TINY_REX_STATE_JUMPING:
    {
        tiny_rex_object.y -= tiny_rex_object.speed;

        if (tiny_rex_object.y <= AXIS_Y_TINY_REX_OBJECT_TOP)
        {
            tiny_rex_object.y = AXIS_Y_TINY_REX_OBJECT_TOP;
            tiny_rex_object.speed = g_st_TrexSpeed[tiny_rex_object.level].u1_FallSpeed;
            tiny_rex_object.state = EM_TINY_REX_STATE_FALLING;
        }
    }
    break;

    case EM_TINY_REX_STATE_FALLING:
    {
        tiny_rex_object.y += tiny_rex_object.speed;

        if (tiny_rex_object.y >= AXIS_Y_TINY_REX_OBJECT_UPDATE)
        {
            tiny_rex_object.y = AXIS_Y_TINY_REX_OBJECT_UPDATE;
            tiny_rex_object.state = EM_TINY_REX_STATE_RUNNING;
            tiny_rex_object.action_image = BITMAP_T_REX_STAND;
        }
    }
    break;

    case EM_TINY_REX_STATE_DUCKING:
    {
        tiny_rex_object.action_image =
            (tiny_rex_object.action_image == BITMAP_T_REX_DUCKING_1)
                ? BITMAP_T_REX_DUCKING_2
                : BITMAP_T_REX_DUCKING_1;
    }
    break;

    default:
        break;
    }
}
/* ************************************* End of File ******************************************** */
