//==================================================================================================
//
//	File Name	:	scr_play.c
//	CPU Type	:
//	IDE			:
//	Customer
//	Version		:	Ver.0.01
//	Coding		:	V.Vu
//	History		:	28/03/2026
//	Outline		:
//
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
#include "scr_play.h"
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

//==================================================================================================
//	Global RAM
//==================================================================================================
static void view_scr_play();
view_dynamic_t dyn_view_scr_play = {
    {
        .item_type = ITEM_TYPE_DYNAMIC,
    },
    view_scr_play};

view_screen_t scr_play = {
    &dyn_view_scr_play,
    ITEM_NULL,
    ITEM_NULL,

    .focus_item = 0,
};
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
game_object_t over_check_object;
score_t score_object;
//==================================================================================================
//	Local ROM
//==================================================================================================

//==================================================================================================
//	Local Function Prototype
//==================================================================================================
static void reset(void);
static bool tiny_rex_collision_check(void);
static bool object_collision(const game_object_t *obj1,
                             const bitmap_info_t *bmp1,
                             const game_object_t *obj2,
                             const bitmap_info_t *bmp2);
static void draw_over_icon(void);
/* Score */
static void draw_score(void);
static void update_score(void);
//==================================================================================================
//	Source Code
//==================================================================================================
////////////////////////////////////////////////////////////////////////////////////////////////////
//	Name     : view_scr_play
//	Function : Render all visual objects of Play screen
//	Argument : None
//	Return   : None
//	Created  : 13/07/2026 V.Vu
//	Changed  : -
//	Remarks  : This function is called by Screen Manager whenever the Play screen has event
//             Render order:
//             1. Draw scrolling ground.
//             2. Draw duplicated ground bitmap for seamless scrolling.
//             3. Draw T-Rex sprite.
////////////////////////////////////////////////////////////////////////////////////////////////////
static void view_scr_play()
{
    /* Draw object */
    draw_line_object();
    draw_bird_object();
    draw_tree_object();
    draw_tiny_rex_object();
    draw_score();
    draw_over_icon();
}
////////////////////////////////////////////////////////////////////////////////////////////////////
//	Name     : scr_play_handle_signal
//	Function : Hanlde signal receive from Display task
//	Argument : ak_msg_t* msg: Message information
//	Return   : None
//	Created  : 13/07/2026 V.Vu
//	Changed  : -
//	Remarks  : -
////////////////////////////////////////////////////////////////////////////////////////////////////
void scr_play_handle_signal(ak_msg_t* msg)
{
    switch (msg->sig)
    {
    case SCREEN_ENTRY:
    {
        APP_DBG_SIG("SCREEN_PLAY_ENTRY\n");
        reset();
        task_post_pure_msg(TINY_REX_OBJECT_ID, EVENT_TINY_REX_SETUP);
        task_post_pure_msg(TINY_REX_OBJECT_ID, EVENT_TINY_REX_PLAY);
        task_post_pure_msg(BIRD_OBJECT_ID, EVENT_BIRD_OBJECT_SETUP);
        task_post_pure_msg(BIRD_OBJECT_ID, EVENT_BIRD_OBJECT_PLAY);
        task_post_pure_msg(TREE_OBJECT_ID, EVENT_TREE_OBJECT_SETUP);
        task_post_pure_msg(TREE_OBJECT_ID, EVENT_TREE_OBJECT_PLAY);
        task_post_pure_msg(LINE_OBJECT_ID, EVENT_LINE_OBJECT_SETUP);
        task_post_pure_msg(LINE_OBJECT_ID, EVENT_LINE_OBJECT_PLAY);
        timer_set(
            AC_TASK_DISPLAY_ID,
            AC_DISPLAY_PLAYING_UPDATE,
            AC_DISPLAY_PLAYING_UPDATE_INTERVAL,
            TIMER_PERIODIC);
    }
    break;

    case AC_DISPLAY_BUTON_UP_PRESSED:
    {
        BUZZER_PlaySound(BUZZER_SOUND_CLICK);
        task_post_pure_msg(TINY_REX_OBJECT_ID, EVENT_BUTTON_UP_PRESS);
    }
    break;

    case AC_DISPLAY_BUTON_MODE_PRESSED:
    {
        BUZZER_PlaySound(BUZZER_SOUND_CLICK);
        task_post_pure_msg(TINY_REX_OBJECT_ID, EVENT_BUTTON_MODE_PRESS);
        if (over_check_object.state == EM_GAME_STATE_OVER)
        {
            SCREEN_TRAN(scr_menu_handle, &scr_menu);
        }
    }
    break;
    case AC_DISPLAY_BUTON_MODE_RELEASE:
    {
        task_post_pure_msg(TINY_REX_OBJECT_ID, EVENT_BUTTON_MODE_RELEASE);
    }
    break;

    case AC_DISPLAY_BUTON_DOWN_PRESSED:
    {
        BUZZER_PlaySound(BUZZER_SOUND_CLICK);
        task_post_pure_msg(TINY_REX_OBJECT_ID, EVENT_BUTTON_DOWN_PRESS);
    }
    break;

    case AC_DISPLAY_PLAYING_UPDATE:
    {
        if(tiny_rex_collision_check() == false)
        {
            update_score();
            task_post_pure_msg(TINY_REX_OBJECT_ID, EVENT_TINY_REX_UPDATE);
            task_post_pure_msg(BIRD_OBJECT_ID, EVENT_BIRD_OBJECT_UPDATE);
            task_post_pure_msg(TREE_OBJECT_ID, EVENT_TREE_OBJECT_UPDATE);
            task_post_pure_msg(LINE_OBJECT_ID, EVENT_LINE_OBJECT_UPDATE);
        }
    }
    break;

    default:
        break;
    }
}
////////////////////////////////////////////////////////////////////////////////////////////////////
//	Name     : object_collision
//	Function : Check collision between two object
//	Argument : const game_object_t *obj1: Information of object 1
//             const bitmap_info_t *bmp1: Bitmap of object 1
//             const game_object_t *obj2: Information of object 2
//             const bitmap_info_t *bmp2: Bitmap of object 2
//	Return   : None
//	Created  : 13/07/2026 V.Vu
//	Changed  : -
//	Remarks  : -
////////////////////////////////////////////////////////////////////////////////////////////////////
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
////////////////////////////////////////////////////////////////////////////////////////////////////
//	Name     : tiny_rex_collision_check
//	Function : Check collistion between tiny rex with other objects
//	Argument : None
//	Return   : None
//	Created  : 13/07/2026 V.Vu
//	Changed  : -
//	Remarks  : -
////////////////////////////////////////////////////////////////////////////////////////////////////
static bool tiny_rex_collision_check(void)
{

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
        over_check_object.state = EM_GAME_STATE_OVER;
        over_check_object.visible = WHITE;
        over_check_object.action_image = BITMAP_GAME_OVER_ICON;
        over_check_object.x = (WIDTH-g_bitmap_table[over_check_object.action_image].width) / 2;
        over_check_object.y = (HEIGHT-g_bitmap_table[over_check_object.action_image].height) / 2;;
        timer_remove_attr(AC_TASK_DISPLAY_ID, AC_DISPLAY_PLAYING_UPDATE);
        BUZZER_PlaySound(BUZZER_SOUND_GOODBYE);
        /* Save new score */
        udpate_high_score(score_object.current_score);
		score_object.high_score = get_highest_score();
    }
    return (rex_tree_collistion || rex_bird_collistion)?true:false;
}
////////////////////////////////////////////////////////////////////////////////////////////////////
//	Name     : draw_over_icon
//	Function : Draw over icon when collistion detected
//	Argument : None
//	Return   : None
//	Created  : 13/07/2026 V.Vu
//	Changed  : -
//	Remarks  : -
////////////////////////////////////////////////////////////////////////////////////////////////////
static void draw_over_icon(void)
{
    if(over_check_object.visible == BLACK)
        return;
    // Draw rectangle
    view_render.drawRoundRect(
        over_check_object.x-1,
        over_check_object.y-1,
        g_bitmap_table[over_check_object.action_image].width+2,
        g_bitmap_table[over_check_object.action_image].height+2,
        2,
        WHITE);
    // Draw bit-map of over check icon
    view_render.drawBitmap(
        over_check_object.x,
        over_check_object.y,
        g_bitmap_table[over_check_object.action_image].bitmap,
        g_bitmap_table[over_check_object.action_image].width,
        g_bitmap_table[over_check_object.action_image].height,
        WHITE);
}
////////////////////////////////////////////////////////////////////////////////////////////////////
//	Name     : draw_score
//	Function : Draw current and highest score
//	Argument : None
//	Return   : None
//	Created  : 13/07/2026 V.Vu
//	Changed  : -
//	Remarks  : -
////////////////////////////////////////////////////////////////////////////////////////////////////
static void draw_score(void)
{
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
    if(score_blink_on || over_check_object.state == EM_GAME_STATE_OVER)
    {
        snprintf(str, sizeof(str), "%04lu", score_object.current_score);
        view_render.setCursor(102, 6);
        view_render.print(str);
    }
}
////////////////////////////////////////////////////////////////////////////////////////////////////
//	Name     : update_score
//	Function : Update score and play sound if high score exceeded
//	Argument : None
//	Return   : None
//	Created  : 13/07/2026 V.Vu
//	Changed  : -
//	Remarks  : -
////////////////////////////////////////////////////////////////////////////////////////////////////
static void update_score(void)
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
        BUZZER_PlaySound(BUZZER_SOUND_HIGHSCORE);
    }
}
////////////////////////////////////////////////////////////////////////////////////////////////////
//	Name     : reset
//	Function : Reset overcheck object and score to default value
//	Argument : None
//	Return   : None
//	Created  : 13/07/2026 V.Vu
//	Changed  : -
//	Remarks  : -
////////////////////////////////////////////////////////////////////////////////////////////////////
static void reset(void)
{
    /* Reset over check object */
    over_check_object.state = EM_GAME_STATE_PLAYING;
    over_check_object.visible = BLACK;
    /* Reset score */
    score_object.skip_count = 0;
    score_object.current_score = 0;
    score_object.high_score = 0;
    score_object.threshold = 100;
    score_object.animation_timer = 0;
}