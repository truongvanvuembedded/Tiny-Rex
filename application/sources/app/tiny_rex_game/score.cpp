//==================================================================================================
//	File Name	: Score.cpp
//	CPU Type	: STM32
//	Builder		: STM32CUBEIDE
//	Coding		: V.Vu
//	History		: 25/08/2026 First version
//	Outline		: Score and difficullt level mamanger
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
#include "score.h"
#include "tsm.h"
#include "scr_setting.h"
#include "scr_ranking.h"
//==================================================================================================
//	Local define
//==================================================================================================

//==================================================================================================
//	Local define I/O
//==================================================================================================

//==================================================================================================
//	Local Struct Template
//==================================================================================================
/* Score */
typedef struct
{
    uint32_t skip_count;
    uint32_t threshold;
    uint32_t current_score;
    uint32_t high_score;
    uint8_t animation_timer;
} score_t;
//==================================================================================================
//	Local RAM
//==================================================================================================
game_object_t over_icon;
score_t score_object;
//==================================================================================================
//	Global RAM
//==================================================================================================
extern uint8_t Game_State;
//==================================================================================================
//	Local ROM
//==================================================================================================

//==================================================================================================
//	Local Function Prototype
//==================================================================================================
/* TSM action */
static void score_on_setup(ak_msg_t* msg);
static void score_on_update(ak_msg_t* msg);
static void score_on_game_over(ak_msg_t* msg);
//==================================================================================================
//	State Table (TSM)
//	Score has only one state, so the table is a signal -> action map.
//==================================================================================================
typedef enum
{
    EM_SCORE_STATE_ACTIVE = 0,
} EM_SCORE_STATE;

static tsm_t score_tsm_active[] =
    {
        {SCORE_SETUP, TSM_NULL_STATE, score_on_setup},
        {SCORE_UPDATE, TSM_NULL_STATE, score_on_update},
        {SCORE_GAME_OVER, TSM_NULL_STATE, score_on_game_over},
        {TSM_NULL_MSG, TSM_NULL_STATE, TSM_NULL_ROUTINE},
};

/* Index of this table MUST be the same as EM_SCORE_STATE */
static tsm_t* score_tsm_table[] =
    {
        score_tsm_active,
};

static tsm_tbl_t score_tsm =
    {
        EM_SCORE_STATE_ACTIVE,
        TSM_NULL_ON_STATE,
        score_tsm_table,
};
//==================================================================================================
//	Source Code
//==================================================================================================
void score_handle(ak_msg_t* msg)
{
    tsm_dispatch(&score_tsm, msg);
}

static void score_on_setup(ak_msg_t* msg)
{
    (void)msg;
    over_icon.visible = BLACK;
    /* Reset score */
    score_object.skip_count = 0;
    score_object.current_score = 0;
    score_object.high_score = get_highest_score();
    ;
    score_object.threshold = 100;
    score_object.animation_timer = 0;
}

static void score_on_update(ak_msg_t* msg)
{
    (void)msg;
    /* Update score */
    score_object.skip_count++;
    if (score_object.skip_count >= 2)
    {
        score_object.current_score++;
        if (score_object.current_score > 9999)
        {
            score_object.current_score = 9999;
        }
        score_object.skip_count = 0;
    }
    /* Update new high score */
    if (score_object.current_score > score_object.threshold)
    {
        score_object.threshold += 100;
        score_object.animation_timer = 25;
        BUZZER_PlaySound_Priority(BUZZER_SOUND_HIGHSCORE);
        task_post_pure_msg(TINY_REX_OBJECT_ID, TINY_REX_INC_SPEED_EVENT);
        task_post_pure_msg(OBSTACLE_OBJECT_ID, OBSTACLE_INC_SPEED_EVENT);
        task_post_pure_msg(HORIZON_OBJECT_ID, HORIZON_INC_SPEED_EVENT);
    }
}

static void score_on_game_over(ak_msg_t* msg)
{
    (void)msg;
    over_icon.visible = WHITE;
    over_icon.action_image = BITMAP_GAME_OVER_ICON;
    over_icon.x = (WIDTH - g_bitmap_table[over_icon.action_image].width) / 2;
    over_icon.y = (HEIGHT - g_bitmap_table[over_icon.action_image].height) / 2;
    ;
    timer_remove_attr(TINY_REX_TASK_DISPLAY_ID, TINY_REX_DISPLAY_PLAYING_UPDATE);
    /* Save new score */
    ranking_t new_data;
    if (get_current_user_name(new_data.name, SETTING_MAX_NAME))
    {
        new_data.score = score_object.current_score;
        udpate_high_score(&new_data);
        score_object.high_score = get_highest_score();
    }
}

void draw_score(void)
{
    /* Score */
    char str[5];
    uint8_t score_blink_on = true;

    view_render.setTextSize(1);
    view_render.setTextColor(WHITE);

    /* HI */
    view_render.setCursor(61, 6);
    view_render.print("HI");

    /* High Score */
    snprintf(str, sizeof(str), "%04lu", score_object.high_score);
    view_render.setCursor(75, 6);
    view_render.print(str);

    /* Current Score */
    if (score_object.animation_timer > 0)
    {
        score_blink_on = ((score_object.animation_timer / 5) & 0x01);
        score_object.animation_timer--;
    }
    if (score_blink_on || Game_State == EM_GAME_STATE_OVER)
    {
        snprintf(str, sizeof(str), "%04lu", score_object.current_score);
        view_render.setCursor(102, 6);
        view_render.print(str);
    }
}
void draw_over_icon(void)
{
    /* Over icon */
    if (over_icon.visible == BLACK)
        return;
    // Draw rectangle
    view_render.drawRoundRect(
        over_icon.x - 1,
        over_icon.y - 1,
        g_bitmap_table[over_icon.action_image].width + 2,
        g_bitmap_table[over_icon.action_image].height + 2,
        2,
        WHITE);
    // Draw bit-map of over check icon
    view_render.drawBitmap(
        over_icon.x,
        over_icon.y,
        g_bitmap_table[over_icon.action_image].bitmap,
        g_bitmap_table[over_icon.action_image].width,
        g_bitmap_table[over_icon.action_image].height,
        WHITE);
}