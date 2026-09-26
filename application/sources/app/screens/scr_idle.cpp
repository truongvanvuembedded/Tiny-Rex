//==================================================================================================
//	File Name	: scr_idle.cpp
//	CPU Type	: STM32L151CBT6A
//	Builder		: gcc-arm-none-eabi
//	Coding		:
//	History		: First version
//	Outline		: Idle screen (attract mode). The T-Rex runs and jumps over trees by itself.
//	              Press any button to go to the menu screen.
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
#include "scr_idle.h"
#include "scr_ranking.h"
#include "tsm.h"
//==================================================================================================
//	Local define
//==================================================================================================
/* Everything moves one step each TINY_REX_DISPLAY_IDLE_UPDATE_INTERVAL (50ms) */
#define IDLE_SPEED (4) /* Pixel per step of ground and tree */
/* Text */
#define IDLE_FONT_WIDTH (X_SIZE_FONT + 1)
#define IDLE_HI_TEXT_AXIS_Y (2)
#define IDLE_HI_TEXT_MAX (12)
#define IDLE_PROMPT_TEXT "PRESS BUTTON"
#define IDLE_PROMPT_AXIS_Y (30)
#define IDLE_PROMPT_BLINK_STEP (10) /* 10 step x 50ms = 500ms on, 500ms off */
/* Cloud */
#define IDLE_CLOUD_COUNT (2)
#define IDLE_CLOUD_MOVE_STEP (2) /* Cloud move 1 pixel every 2 step */
/* T-Rex */
#define IDLE_REX_AXIS_X (10)
#define IDLE_REX_RUN_FRAME_STEP (3) /* Change running image every 3 step */
/* T-Rex jump over a tree when the distance from T-Rex to the tree is not over this value */
#define IDLE_JUMP_TRIGGER_DISTANCE (17)
/* Tree */
#define IDLE_TREE_FIRST (BITMAP_TREE_1)
#define IDLE_TREE_LAST (BITMAP_TREE_4)
//==================================================================================================
//	Local define I/O
//==================================================================================================

//==================================================================================================
//	Struct Template
//==================================================================================================
typedef struct
{
    int16_t ground_x;
    int16_t cloud_x[IDLE_CLOUD_COUNT];
    int16_t tree_x;
    uint8_t tree_image;
    uint8_t tree_gap_index;
    uint8_t rex_image;
    int16_t rex_jump_height;
    uint8_t rex_jump_index;
    uint8_t rex_run_step;
    uint8_t cloud_step;
    uint8_t prompt_step;
    char hi_text[IDLE_HI_TEXT_MAX];
} idle_object_t;

static void view_scr_idle();
view_dynamic_t dyn_view_scr_idle = {
    {
        .item_type = ITEM_TYPE_DYNAMIC,
    },
    view_scr_idle};

view_screen_t scr_idle = {
    &dyn_view_scr_idle,
    ITEM_NULL,
    ITEM_NULL,

    .focus_item = 0,
};
//==================================================================================================
//	Local RAM
//==================================================================================================
static idle_object_t idle_object;
//==================================================================================================
//	Local ROM
//==================================================================================================
static const uint8_t IDLE_CLOUD_AXIS_Y[IDLE_CLOUD_COUNT] = {12, 16};
static const int16_t IDLE_CLOUD_AXIS_X_INIT[IDLE_CLOUD_COUNT] = {12, 77};
/* Extra distance before the next tree comes. Keep it a multiple of IDLE_SPEED */
static const uint8_t IDLE_TREE_GAP[] = {0, 24, 8, 40};
/* Height of T-Rex above the ground in each step of a jump */
static const uint8_t IDLE_JUMP_HEIGHT[] = {8, 15, 21, 26, 30, 33, 35, 36, 36, 35, 33, 30, 26, 21, 15, 8};
//==================================================================================================
//	Local Function Prototype
//==================================================================================================
/* TSM action */
static void idle_on_entry(ak_msg_t* msg);
static void idle_on_update_running(ak_msg_t* msg);
static void idle_on_update_jumping(ak_msg_t* msg);
static void idle_on_button(ak_msg_t* msg);
static void idle_reset(void);
static void idle_move_scenery(void);
//==================================================================================================
//	State Table (TSM)
//	The T-Rex is either RUNNING or JUMPING. It starts to jump when a tree is close and it is
//	RUNNING again when the jump is done. Each row: {signal, next state, action}.
//	A signal which is not in the table of the current state is ignored.
//==================================================================================================
typedef enum
{
    EM_IDLE_STATE_RUNNING = 0,
    EM_IDLE_STATE_JUMPING,
} EM_IDLE_STATE;

static tsm_t idle_tsm_running[] = {
    {SCREEN_ENTRY, EM_IDLE_STATE_RUNNING, idle_on_entry},
    {TINY_REX_DISPLAY_IDLE_UPDATE, TSM_NULL_STATE, idle_on_update_running},
    {TINY_REX_DISPLAY_BUTTON_MODE_PRESSED, TSM_NULL_STATE, idle_on_button},
    {TINY_REX_DISPLAY_BUTTON_UP_PRESSED, TSM_NULL_STATE, idle_on_button},
    {TINY_REX_DISPLAY_BUTTON_DOWN_PRESSED, TSM_NULL_STATE, idle_on_button},
    {TSM_NULL_MSG, TSM_NULL_STATE, TSM_NULL_ROUTINE},
};

static tsm_t idle_tsm_jumping[] = {
    {SCREEN_ENTRY, EM_IDLE_STATE_RUNNING, idle_on_entry},
    {TINY_REX_DISPLAY_IDLE_UPDATE, TSM_NULL_STATE, idle_on_update_jumping},
    {TINY_REX_DISPLAY_BUTTON_MODE_PRESSED, TSM_NULL_STATE, idle_on_button},
    {TINY_REX_DISPLAY_BUTTON_UP_PRESSED, TSM_NULL_STATE, idle_on_button},
    {TINY_REX_DISPLAY_BUTTON_DOWN_PRESSED, TSM_NULL_STATE, idle_on_button},
    {TSM_NULL_MSG, TSM_NULL_STATE, TSM_NULL_ROUTINE},
};

/* Index of this table MUST be the same as EM_IDLE_STATE */
static tsm_t* idle_tsm_table[] = {
    idle_tsm_running,
    idle_tsm_jumping,
};

static tsm_tbl_t idle_tsm = {
    EM_IDLE_STATE_RUNNING,
    TSM_NULL_ON_STATE,
    idle_tsm_table,
};
//==================================================================================================
//	Source Code
//==================================================================================================
////////////////////////////////////////////////////////////////////////////////////////////////////
//	Name     : scr_idle_handle
//	Function : Handle message send to idle screen
//	Argument : ak_msg_t* msg: Message information
//	Return   : None
//	Created  : -
//	Changed  : -
//	Remarks  : Any button pressed goes to the menu screen.
////////////////////////////////////////////////////////////////////////////////////////////////////
void scr_idle_handle(ak_msg_t* msg)
{
    tsm_dispatch(&idle_tsm, msg);
}
////////////////////////////////////////////////////////////////////////////////////////////////////
//	Name     : idle_on_entry
//	Function : Screen is opened, start the animation
//	Argument : ak_msg_t* msg: Message information
//	Return   : None
//	Created  : -
//	Changed  : -
//	Remarks  : The table already moved the state to RUNNING.
////////////////////////////////////////////////////////////////////////////////////////////////////
static void idle_on_entry(ak_msg_t* msg)
{
    (void)msg;
    APP_DBG_SIG("SCREEN_IDLE_ENTRY\n");
    idle_reset();
    timer_set(TINY_REX_TASK_DISPLAY_ID, TINY_REX_DISPLAY_IDLE_UPDATE, TINY_REX_DISPLAY_IDLE_UPDATE_INTERVAL, TIMER_PERIODIC);
}
////////////////////////////////////////////////////////////////////////////////////////////////////
//	Name     : idle_on_button
//	Function : Stop animation and go to menu screen
//	Argument : ak_msg_t* msg: Message information
//	Return   : None
//	Created  : -
//	Changed  : -
//	Remarks  : -
////////////////////////////////////////////////////////////////////////////////////////////////////
static void idle_on_button(ak_msg_t* msg)
{
    (void)msg;
    timer_remove_attr(TINY_REX_TASK_DISPLAY_ID, TINY_REX_DISPLAY_IDLE_UPDATE);
    SCREEN_TRAN(scr_menu_handle, &scr_menu);
    BUZZER_PlaySound(BUZZER_SOUND_CLICK);
}
////////////////////////////////////////////////////////////////////////////////////////////////////
//	Name     : idle_reset
//	Function : Put all objects of the idle screen to the start position
//	Argument : None
//	Return   : None
//	Created  : -
//	Changed  : -
//	Remarks  : The high score is read here (EEPROM), not in every step of the animation.
////////////////////////////////////////////////////////////////////////////////////////////////////
static void idle_reset(void)
{
    idle_object.ground_x = 0;
    for (uint8_t i = 0; i < IDLE_CLOUD_COUNT; i++)
    {
        idle_object.cloud_x[i] = IDLE_CLOUD_AXIS_X_INIT[i];
    }
    idle_object.tree_image = IDLE_TREE_FIRST;
    idle_object.tree_gap_index = 0;
    idle_object.tree_x = WIDTH;
    idle_object.rex_image = BITMAP_T_REX_RUN_1;
    idle_object.rex_jump_height = 0;
    idle_object.rex_jump_index = 0;
    idle_object.rex_run_step = 0;
    idle_object.cloud_step = 0;
    idle_object.prompt_step = 0;

    /* The game keeps score in 0..9999 (see score.cpp), so 4 digits are enough */
    uint32_t high_score = get_highest_score();
    if (high_score > 9999)
    {
        high_score = 9999;
    }
    snprintf(idle_object.hi_text, sizeof(idle_object.hi_text), "HI %04lu", (unsigned long)high_score);
}
////////////////////////////////////////////////////////////////////////////////////////////////////
//	Name     : idle_move_scenery
//	Function : Move ground, cloud and tree one step and update the prompt blink
//	Argument : None
//	Return   : None
//	Created  : -
//	Changed  : -
//	Remarks  : Called in every step, before the T-Rex decides what to do.
////////////////////////////////////////////////////////////////////////////////////////////////////
static void idle_move_scenery(void)
{
    /* Ground */
    idle_object.ground_x -= IDLE_SPEED;
    if (idle_object.ground_x <= -g_bitmap_table[BITMAP_LINE].width)
    {
        idle_object.ground_x += g_bitmap_table[BITMAP_LINE].width;
    }
    /* Cloud */
    idle_object.cloud_step++;
    if (idle_object.cloud_step >= IDLE_CLOUD_MOVE_STEP)
    {
        idle_object.cloud_step = 0;
        for (uint8_t i = 0; i < IDLE_CLOUD_COUNT; i++)
        {
            idle_object.cloud_x[i]--;
            if (idle_object.cloud_x[i] <= -g_bitmap_table[BITMAP_GAME_CLOUD_ICON].width)
            {
                idle_object.cloud_x[i] = WIDTH;
            }
        }
    }
    /* Tree, when it goes out of the screen the next kind of tree comes */
    idle_object.tree_x -= IDLE_SPEED;
    if (idle_object.tree_x <= -g_bitmap_table[idle_object.tree_image].width)
    {
        idle_object.tree_image = (idle_object.tree_image >= IDLE_TREE_LAST) ? IDLE_TREE_FIRST : (idle_object.tree_image + 1);
        idle_object.tree_x = WIDTH + IDLE_TREE_GAP[idle_object.tree_gap_index];
        idle_object.tree_gap_index = (idle_object.tree_gap_index + 1) % (sizeof(IDLE_TREE_GAP) / sizeof(IDLE_TREE_GAP[0]));
    }
    /* Prompt blink */
    idle_object.prompt_step++;
    if (idle_object.prompt_step >= (IDLE_PROMPT_BLINK_STEP * 2))
    {
        idle_object.prompt_step = 0;
    }
}
////////////////////////////////////////////////////////////////////////////////////////////////////
//	Name     : idle_on_update_running
//	Function : T-Rex is running, jump when a tree is close
//	Argument : ak_msg_t* msg: Message information
//	Return   : None
//	Created  : -
//	Changed  : -
//	Remarks  : -
////////////////////////////////////////////////////////////////////////////////////////////////////
static void idle_on_update_running(ak_msg_t* msg)
{
    (void)msg;
    idle_move_scenery();
    idle_object.rex_jump_height = 0;
    /* Running animation */
    idle_object.rex_run_step++;
    if (idle_object.rex_run_step >= IDLE_REX_RUN_FRAME_STEP)
    {
        idle_object.rex_run_step = 0;
        idle_object.rex_image = (idle_object.rex_image == BITMAP_T_REX_RUN_1) ? BITMAP_T_REX_RUN_2 : BITMAP_T_REX_RUN_1;
    }
    /* Jump when a tree is close */
    int16_t distance = idle_object.tree_x - (IDLE_REX_AXIS_X + g_bitmap_table[BITMAP_T_REX_RUN_1].width);
    if ((distance > 0) && (distance <= IDLE_JUMP_TRIGGER_DISTANCE))
    {
        idle_object.rex_image = BITMAP_T_REX_STAND;
        idle_object.rex_jump_height = IDLE_JUMP_HEIGHT[0];
        idle_object.rex_jump_index = 1;
        TSM_TRAN(&idle_tsm, EM_IDLE_STATE_JUMPING);
    }
}
////////////////////////////////////////////////////////////////////////////////////////////////////
//	Name     : idle_on_update_jumping
//	Function : T-Rex is in the air, follow the jump height table
//	Argument : ak_msg_t* msg: Message information
//	Return   : None
//	Created  : -
//	Changed  : -
//	Remarks  : -
////////////////////////////////////////////////////////////////////////////////////////////////////
static void idle_on_update_jumping(ak_msg_t* msg)
{
    (void)msg;
    idle_move_scenery();
    idle_object.rex_jump_height = IDLE_JUMP_HEIGHT[idle_object.rex_jump_index];
    idle_object.rex_jump_index++;
    if (idle_object.rex_jump_index >= (sizeof(IDLE_JUMP_HEIGHT) / sizeof(IDLE_JUMP_HEIGHT[0])))
    {
        /* Land, the next step T-Rex is running again */
        idle_object.rex_image = BITMAP_T_REX_RUN_1;
        idle_object.rex_run_step = 0;
        TSM_TRAN(&idle_tsm, EM_IDLE_STATE_RUNNING);
    }
}
////////////////////////////////////////////////////////////////////////////////////////////////////
//	Name     : view_scr_idle
//	Function : Draw objects in idle screen
//	Argument : None
//	Return   : None
//	Created  : -
//	Changed  : -
//	Remarks  : -
////////////////////////////////////////////////////////////////////////////////////////////////////
static void view_scr_idle()
{
    view_render.setTextSize(1);
    view_render.setTextColor(WHITE);
    /* Cloud */
    for (uint8_t i = 0; i < IDLE_CLOUD_COUNT; i++)
    {
        view_render.drawBitmap(
            idle_object.cloud_x[i],
            IDLE_CLOUD_AXIS_Y[i],
            g_bitmap_table[BITMAP_GAME_CLOUD_ICON].bitmap,
            g_bitmap_table[BITMAP_GAME_CLOUD_ICON].width,
            g_bitmap_table[BITMAP_GAME_CLOUD_ICON].height,
            WHITE);
    }
    /* High score */
    view_render.setCursor((WIDTH - strlen(idle_object.hi_text) * IDLE_FONT_WIDTH) / 2, IDLE_HI_TEXT_AXIS_Y);
    view_render.print(idle_object.hi_text);
    /* Prompt, blink */
    if (idle_object.prompt_step < IDLE_PROMPT_BLINK_STEP)
    {
        view_render.setCursor((WIDTH - (sizeof(IDLE_PROMPT_TEXT) - 1) * IDLE_FONT_WIDTH) / 2, IDLE_PROMPT_AXIS_Y);
        view_render.print(IDLE_PROMPT_TEXT);
    }
    /* Ground, the second one makes the scroll seamless */
    view_render.drawBitmap(
        idle_object.ground_x,
        HEIGHT - g_bitmap_table[BITMAP_LINE].height,
        g_bitmap_table[BITMAP_LINE].bitmap,
        g_bitmap_table[BITMAP_LINE].width,
        g_bitmap_table[BITMAP_LINE].height,
        WHITE);
    view_render.drawBitmap(
        idle_object.ground_x + WIDTH,
        HEIGHT - g_bitmap_table[BITMAP_LINE].height,
        g_bitmap_table[BITMAP_LINE].bitmap,
        g_bitmap_table[BITMAP_LINE].width,
        g_bitmap_table[BITMAP_LINE].height,
        WHITE);
    /* Tree, clear its area first, otherwise the ground line shows through the tree */
    view_render.fillRoundRect(
        idle_object.tree_x,
        HEIGHT - g_bitmap_table[idle_object.tree_image].height,
        g_bitmap_table[idle_object.tree_image].width,
        g_bitmap_table[idle_object.tree_image].height,
        0,
        BLACK);
    view_render.drawBitmap(
        idle_object.tree_x,
        HEIGHT - g_bitmap_table[idle_object.tree_image].height,
        g_bitmap_table[idle_object.tree_image].bitmap,
        g_bitmap_table[idle_object.tree_image].width,
        g_bitmap_table[idle_object.tree_image].height,
        WHITE);
    if(idle_tsm.state == EM_IDLE_STATE_RUNNING)
    {
        /* T-Rex, clear its area first, otherwise the ground line shows through the belly */
        view_render.fillRoundRect(
            IDLE_REX_AXIS_X,
            HEIGHT - g_bitmap_table[idle_object.rex_image].height - 1 - idle_object.rex_jump_height,
            g_bitmap_table[idle_object.rex_image].width,
            g_bitmap_table[idle_object.rex_image].height,
            0,
            BLACK);
    }
    view_render.drawBitmap(
        IDLE_REX_AXIS_X,
        HEIGHT - g_bitmap_table[idle_object.rex_image].height - 1 - idle_object.rex_jump_height,
        g_bitmap_table[idle_object.rex_image].bitmap,
        g_bitmap_table[idle_object.rex_image].width,
        g_bitmap_table[idle_object.rex_image].height,
        WHITE);
}
/* ************************************* End of File ******************************************** */
