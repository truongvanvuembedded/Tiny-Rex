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
#include "tsm.h"
//==================================================================================================
//	Local define
//==================================================================================================
/* Y Axis position */
#define AXIS_Y_TINY_REX_OBJECT_UPDATE (HEIGHT - g_bitmap_table[tiny_rex_object.action_image].height - 1)
/* Axis */
#define AXIS_X_TINY_REX_OBJECT_INIT (5)
#define AXIS_Y_TINY_REX_OBJECT_INIT (AXIS_Y_TINY_REX_OBJECT_UPDATE)
/* Change the running/ducking image every 3 updates (3 x 50ms = 150ms), same as the idle screen */
#define TINY_REX_OBJECT_ANIMATION_STEP (3)
/* Steps of the jump height table for each update. Fast fall (Down button) goes through the table 2 times faster */
#define TINY_REX_JUMP_STEP_NORMAL (1)
#define TINY_REX_JUMP_STEP_FAST (2)
//==================================================================================================
//	Local define I/O
//==================================================================================================

//==================================================================================================
//	Local Struct Template
//==================================================================================================
/* Level */
typedef enum
{
    TREX_LEVEL_L1_EASY = 0, /* The game starts here */
    TREX_LEVEL_L2_NORMAL,
    TREX_LEVEL_L3_HARD,
    TREX_LEVEL_L4_EXTREME,
    TREX_LEVEL_MAX,
} TREX_LEVEL;

/* Jump */
typedef struct
{
    const uint8_t* height; /* Height of T-Rex above the ground in each update of a jump */
    uint8_t length;        /* Number of entries of the table */
} ST_TREX_JUMP;
//==================================================================================================
//	Local RAM
//==================================================================================================
static uint8_t animation_step;     /* Updates since the image was changed */
static const uint8_t* jump_height; /* Jump height table of the jump in progress */
static uint8_t jump_length;        /* Number of entries of jump_height */
static uint8_t jump_index;         /* Next entry of jump_height */
static uint8_t jump_step;          /* TINY_REX_JUMP_STEP_NORMAL or TINY_REX_JUMP_STEP_FAST */

//==================================================================================================
//	Global RAM
//==================================================================================================
game_object_t tiny_rex_object;
//==================================================================================================
//	Local ROM
//==================================================================================================
/* Jump height tables, one per level. Every table goes up to the same top and comes back down
 * (symmetric). The higher the level the faster the obstacles come, so the jump is shorter.
 * Entry n is the height of T-Rex above the ground n updates (50ms) after the jump starts. */
static const uint8_t TINY_REX_JUMP_HEIGHT_L1[] = {7, 14, 19, 24, 28, 31, 34, 35, 36, 36, 35, 34, 31, 28, 24, 19, 14, 7};
static const uint8_t TINY_REX_JUMP_HEIGHT_L2[] = {10, 18, 24, 29, 33, 35, 36, 35, 33, 29, 24, 18, 10};
static const uint8_t TINY_REX_JUMP_HEIGHT_L3[] = {10, 19, 26, 31, 34, 36, 36, 34, 31, 26, 19, 10};
static const uint8_t TINY_REX_JUMP_HEIGHT_L4[] = {11, 20, 27, 32, 35, 36, 35, 32, 27, 20, 11};

#define TINY_REX_JUMP_TABLE(table)              \
    {                                           \
        table, sizeof(table) / sizeof(table[0]) \
    }
/* Index of this table MUST be the same as TREX_LEVEL */
static const ST_TREX_JUMP g_st_TrexJump[TREX_LEVEL_MAX] =
    {
        TINY_REX_JUMP_TABLE(TINY_REX_JUMP_HEIGHT_L1),
        TINY_REX_JUMP_TABLE(TINY_REX_JUMP_HEIGHT_L2),
        TINY_REX_JUMP_TABLE(TINY_REX_JUMP_HEIGHT_L3),
        TINY_REX_JUMP_TABLE(TINY_REX_JUMP_HEIGHT_L4),
};
//==================================================================================================
//	Local Function Prototype
//==================================================================================================
/* TSM state callback */
static void tiny_rex_on_state(tsm_state_t state);
static bool tiny_rex_animation_is_time(void);
/* TSM action */
static void tiny_rex_on_play(ak_msg_t* msg);
static void tiny_rex_on_jump(ak_msg_t* msg);
static void tiny_rex_on_fall(ak_msg_t* msg);
static void tiny_rex_on_duck(ak_msg_t* msg);
static void tiny_rex_on_duck_release(ak_msg_t* msg);
static void tiny_rex_on_inc_speed(ak_msg_t* msg);
static void tiny_rex_on_move_running(ak_msg_t* msg);
static void tiny_rex_on_move_jumping(ak_msg_t* msg);
static void tiny_rex_on_move_falling(ak_msg_t* msg);
static void tiny_rex_on_move_ducking(ak_msg_t* msg);
//==================================================================================================
//	State Table (TSM)
//	Each row: {signal, next state, action}. A signal which is not in the table of the current state
//	is ignored. The row TSM_NULL_MSG ends the table.
//==================================================================================================
static tsm_t tiny_rex_tsm_running[] =
    {
        {TINY_REX_PLAY_EVENT, EM_TINY_REX_STATE_RUNNING, tiny_rex_on_play},
        {TINY_REX_JUMP_EVENT, EM_TINY_REX_STATE_JUMPING, tiny_rex_on_jump},
        {TINY_REX_DUCK_EVENT, EM_TINY_REX_STATE_DUCKING, tiny_rex_on_duck},
        {TINY_REX_INC_SPEED_EVENT, TSM_NULL_STATE, tiny_rex_on_inc_speed},
        {TINY_REX_MOVE_EVENT, TSM_NULL_STATE, tiny_rex_on_move_running},
        {TSM_NULL_MSG, TSM_NULL_STATE, TSM_NULL_ROUTINE},
};

static tsm_t tiny_rex_tsm_jumping[] =
    {
        {TINY_REX_PLAY_EVENT, EM_TINY_REX_STATE_RUNNING, tiny_rex_on_play},
        {TINY_REX_FALL_EVENT, EM_TINY_REX_STATE_FALLING, tiny_rex_on_fall},
        {TINY_REX_INC_SPEED_EVENT, TSM_NULL_STATE, tiny_rex_on_inc_speed},
        {TINY_REX_MOVE_EVENT, TSM_NULL_STATE, tiny_rex_on_move_jumping},
        {TSM_NULL_MSG, TSM_NULL_STATE, TSM_NULL_ROUTINE},
};

static tsm_t tiny_rex_tsm_falling[] =
    {
        {TINY_REX_PLAY_EVENT, EM_TINY_REX_STATE_RUNNING, tiny_rex_on_play},
        {TINY_REX_FALL_EVENT, EM_TINY_REX_STATE_FALLING, tiny_rex_on_fall},
        {TINY_REX_INC_SPEED_EVENT, TSM_NULL_STATE, tiny_rex_on_inc_speed},
        {TINY_REX_MOVE_EVENT, TSM_NULL_STATE, tiny_rex_on_move_falling},
        {TSM_NULL_MSG, TSM_NULL_STATE, TSM_NULL_ROUTINE},
};

static tsm_t tiny_rex_tsm_ducking[] =
    {
        {TINY_REX_PLAY_EVENT, EM_TINY_REX_STATE_RUNNING, tiny_rex_on_play},
        {TINY_REX_DUCK_RELEASE_EVENT, EM_TINY_REX_STATE_RUNNING, tiny_rex_on_duck_release},
        {TINY_REX_INC_SPEED_EVENT, TSM_NULL_STATE, tiny_rex_on_inc_speed},
        {TINY_REX_MOVE_EVENT, TSM_NULL_STATE, tiny_rex_on_move_ducking},
        {TSM_NULL_MSG, TSM_NULL_STATE, TSM_NULL_ROUTINE},
};

/* Index of this table MUST be the same as EM_TINY_REX_STATE */
static tsm_t* tiny_rex_tsm_table[] =
    {
        tiny_rex_tsm_running,
        tiny_rex_tsm_jumping,
        tiny_rex_tsm_falling,
        tiny_rex_tsm_ducking,
};

/* Start in RUNNING state, tiny_rex_object.state follows it via tiny_rex_on_state() */
static tsm_tbl_t tiny_rex_tsm =
    {
        EM_TINY_REX_STATE_RUNNING,
        tiny_rex_on_state,
        tiny_rex_tsm_table,
};
//==================================================================================================
//	Source Code
//==================================================================================================
void tiny_rex_object_handle(ak_msg_t* msg)
{
    tsm_dispatch(&tiny_rex_tsm, msg);
}

static void tiny_rex_on_state(tsm_state_t state)
{
    tiny_rex_object.state = state;
}

/* True every TINY_REX_OBJECT_ANIMATION_STEP updates, then it is time to change the image */
static bool tiny_rex_animation_is_time(void)
{
    animation_step++;
    if (animation_step < TINY_REX_OBJECT_ANIMATION_STEP)
    {
        return false;
    }
    animation_step = 0;
    return true;
}

static void tiny_rex_on_play(ak_msg_t* msg)
{
    (void)msg;
    /* Action image */
    tiny_rex_object.visible = WHITE;
    tiny_rex_object.action_image = BITMAP_T_REX_RUN_1;
    animation_step = 0;
    /* Initial position */
    tiny_rex_object.x = AXIS_X_TINY_REX_OBJECT_INIT;
    tiny_rex_object.y = AXIS_Y_TINY_REX_OBJECT_INIT;
    /* Difficult level */
    tiny_rex_object.level = TREX_LEVEL_L1_EASY;
}

static void tiny_rex_on_jump(ak_msg_t* msg)
{
    (void)msg;
    /* Use the jump height table of the current level for the whole jump */
    jump_height = g_st_TrexJump[tiny_rex_object.level].height;
    jump_length = g_st_TrexJump[tiny_rex_object.level].length;
    jump_index = 0;
    jump_step = TINY_REX_JUMP_STEP_NORMAL;
    /* Change action image */
    tiny_rex_object.action_image = BITMAP_T_REX_STAND;
}

/* Down button in the air: fall 2 times faster.
 * On the way up (or at the top) continue on the way down from the same height, so the T-Rex never
 * goes higher after the button is pressed. */
static void tiny_rex_on_fall(ak_msg_t* msg)
{
    (void)msg;
    if (jump_index <= (jump_length / 2))
    {
        jump_index = jump_length - jump_index + 1;
    }
    jump_step = TINY_REX_JUMP_STEP_FAST;
}

static void tiny_rex_on_duck(ak_msg_t* msg)
{
    (void)msg;
    /* Change action image */
    tiny_rex_object.action_image = BITMAP_T_REX_DUCKING_1;
    animation_step = 0;
    /* Update position for new image */
    tiny_rex_object.y = AXIS_Y_TINY_REX_OBJECT_UPDATE;
}

static void tiny_rex_on_duck_release(ak_msg_t* msg)
{
    (void)msg;
    /* Change action image */
    tiny_rex_object.action_image = BITMAP_T_REX_RUN_1;
    animation_step = 0;
    /* Update position for new image */
    tiny_rex_object.y = AXIS_Y_TINY_REX_OBJECT_UPDATE;
}

static void tiny_rex_on_inc_speed(ak_msg_t* msg)
{
    (void)msg;
    tiny_rex_object.level++;
    if (tiny_rex_object.level >= TREX_LEVEL_MAX)
    {
        tiny_rex_object.level = TREX_LEVEL_L4_EXTREME;
    }
}

static void tiny_rex_on_move_running(ak_msg_t* msg)
{
    (void)msg;
    if (tiny_rex_animation_is_time())
    {
        tiny_rex_object.action_image =
            (tiny_rex_object.action_image == BITMAP_T_REX_RUN_1)
                ? BITMAP_T_REX_RUN_2
                : BITMAP_T_REX_RUN_1;
    }
}

static void tiny_rex_on_move_jumping(ak_msg_t* msg)
{
    (void)msg;
    tiny_rex_object.y = AXIS_Y_TINY_REX_OBJECT_UPDATE - jump_height[jump_index];
    jump_index++;
    /* First half of the table is the way up, the second half is the way down */
    if (jump_index >= (jump_length / 2))
    {
        TSM_TRAN(&tiny_rex_tsm, EM_TINY_REX_STATE_FALLING);
    }
}

static void tiny_rex_on_move_falling(ak_msg_t* msg)
{
    (void)msg;
    if (jump_index >= jump_length)
    {
        /* Landed */
        tiny_rex_object.y = AXIS_Y_TINY_REX_OBJECT_UPDATE;
        tiny_rex_object.action_image = BITMAP_T_REX_STAND;
        animation_step = 0;
        TSM_TRAN(&tiny_rex_tsm, EM_TINY_REX_STATE_RUNNING);
        return;
    }
    tiny_rex_object.y = AXIS_Y_TINY_REX_OBJECT_UPDATE - jump_height[jump_index];
    jump_index += jump_step;
}

static void tiny_rex_on_move_ducking(ak_msg_t* msg)
{
    (void)msg;
    if (tiny_rex_animation_is_time())
    {
        tiny_rex_object.action_image =
            (tiny_rex_object.action_image == BITMAP_T_REX_DUCKING_1)
                ? BITMAP_T_REX_DUCKING_2
                : BITMAP_T_REX_DUCKING_1;
    }
}
void draw_tiny_rex_object(void)
{
    if (tiny_rex_object.visible == BLACK)
        return;
    if (tiny_rex_tsm.state == EM_TINY_REX_STATE_RUNNING || tiny_rex_tsm.state == EM_TINY_REX_STATE_DUCKING)
    {
        /* Clear image before write for avoid back-ground over write to object */
        view_render.fillRoundRect(
            tiny_rex_object.x,
            tiny_rex_object.y,
            (g_bitmap_table[tiny_rex_object.action_image].width) + ((tiny_rex_object.state == EM_TINY_REX_STATE_DUCKING) ? 2 : 0),
            g_bitmap_table[tiny_rex_object.action_image].height,
            0,
            BLACK);
    }
    // Draw bit-map of Tiny-Rex
    view_render.drawBitmap(
        tiny_rex_object.x,
        tiny_rex_object.y,
        g_bitmap_table[tiny_rex_object.action_image].bitmap,
        g_bitmap_table[tiny_rex_object.action_image].width,
        g_bitmap_table[tiny_rex_object.action_image].height,
        WHITE);
}
/* ************************************* End of File ******************************************** */
