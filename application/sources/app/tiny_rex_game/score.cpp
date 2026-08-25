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
typedef struct{
    uint32_t skip_count;
    uint32_t threshold;
    uint32_t current_score;
    uint32_t high_score;
    uint8_t animation_timer;
}score_t;
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

//==================================================================================================
//	Source Code
//==================================================================================================
void score_handle(ak_msg_t* msg)
{
    switch (msg->sig)
    {
        case SCORE_SETUP:
        {
            over_icon.visible = BLACK;
            /* Reset score */
            score_object.skip_count = 0;
            score_object.current_score = 0;
            score_object.high_score = get_highest_score();;
            score_object.threshold = 100;
            score_object.animation_timer = 0;
        }
        break;
        case SCORE_UPDATE:
        {
            /* Update score */
            score_object.skip_count++;
            if(score_object.skip_count >= 2)
            {
                score_object.current_score++;
                if(score_object.current_score > 9999)
                {
                    score_object.current_score = 9999;
                }
                score_object.skip_count = 0;
            }
            /* Update new high score */
            if(score_object.current_score > score_object.threshold)
            {
                score_object.threshold += 100;
                score_object.animation_timer = 25;
                BUZZER_PlaySound_Priority(BUZZER_SOUND_HIGHSCORE);
                task_post_pure_msg(TINY_REX_OBJECT_ID, TINY_REX_INC_SPEED_EVENT);
                task_post_pure_msg(OBSTACLE_OBJECT_ID, OBSTACLE_INC_SPEED_EVENT);
                task_post_pure_msg(HORIZON_OBJECT_ID, HORIZON_INC_SPEED_EVENT);
            }
        }
        break;
        case SCORE_GAME_OVER:
        {
            over_icon.visible = WHITE;
            over_icon.action_image = BITMAP_GAME_OVER_ICON;
            over_icon.x = (WIDTH-g_bitmap_table[over_icon.action_image].width) / 2;
            over_icon.y = (HEIGHT-g_bitmap_table[over_icon.action_image].height) / 2;;
            timer_remove_attr(AC_TASK_DISPLAY_ID, AC_DISPLAY_PLAYING_UPDATE);
            /* Save new score */
            ranking_t new_data;
            if(get_current_user_name(new_data.name, SETTING_MAX_NAME)){
                new_data.score = score_object.current_score;
                udpate_high_score(&new_data);
                score_object.high_score = get_highest_score();
            }
        }
        break;
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
    if(score_blink_on || Game_State == EM_GAME_STATE_OVER)
    {
        snprintf(str, sizeof(str), "%04lu", score_object.current_score);
        view_render.setCursor(102, 6);
        view_render.print(str);
    }
}
void draw_over_icon(void)
{
    /* Over icon */
    if(over_icon.visible == BLACK)
        return;
    // Draw rectangle
    view_render.drawRoundRect(
        over_icon.x-1,
        over_icon.y-1,
        g_bitmap_table[over_icon.action_image].width+2,
        g_bitmap_table[over_icon.action_image].height+2,
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