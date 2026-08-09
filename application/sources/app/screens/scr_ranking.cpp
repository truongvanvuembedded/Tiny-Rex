//==================================================================================================
//	File Name	: scr_ranking.cpp
//	CPU Type	:
//	Builder		:
//	Coding		: V.Vu
//	History		: 08/09/2026 New
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
#include "scr_ranking.h"
#include "scr_setting.h"
//==================================================================================================
//	Local define
//==================================================================================================
#define RANKING_MAX (5)
//==================================================================================================
//	Local define I/O
//==================================================================================================

//==================================================================================================
//	Struct Template
//==================================================================================================
static void view_scr_ranking();
view_dynamic_t dyn_view_ranking = {
    {
        .item_type = ITEM_TYPE_DYNAMIC,
    },
    view_scr_ranking
};

view_screen_t scr_ranking = {
    &dyn_view_ranking,
    ITEM_NULL,
    ITEM_NULL,

    .focus_item = 0,
};
/* Ranking structure */
typedef struct
{
    char name[SETTING_MAX_NAME+1];
    uint32_t score;
} ranking_t;
//==================================================================================================
//	Local RAM
//==================================================================================================

//==================================================================================================
//	Local ROM
//==================================================================================================
/* Default ranking table */
static ranking_t g_ranking[RANKING_MAX];
//==================================================================================================
//	Local Function Prototype
//==================================================================================================

//==================================================================================================
//	Source Code
//==================================================================================================
////////////////////////////////////////////////////////////////////////////////////////////////////
//    Name    : view_scr_ranking
//    Function: Render ranking in screen
//    Argument: None
//    Return  : None
//    Created : V.Vu
//    Change  : 
//    Note    : 
////////////////////////////////////////////////////////////////////////////////////////////////////
void view_scr_ranking()
{
    view_render.setTextSize(1);
    view_render.setTextColor(WHITE);

    /* Ranking title */
    view_render.setCursor(45, 5);
    view_render.print("RANKING");

    /* Draw ranking list */
    for (uint8_t i = 0; i < RANKING_MAX; i++)
    {
        uint8_t y = 18 + (i * 9);

        view_render.setCursor(10, y);
        view_render.print(i + 1);

        view_render.setCursor(25, y);
        view_render.print(g_ranking[i].name);

        view_render.setCursor(80, y);
        view_render.print(g_ranking[i].score);
    }
}
////////////////////////////////////////////////////////////////////////////////////////////////////
//    Name    : scr_ranking_handle
//    Function: Handle message send to ranking screen
//    Argument: None
//    Return  : None
//    Created : V.Vu
//    Change  : 
//    Note    : 
////////////////////////////////////////////////////////////////////////////////////////////////////
void scr_ranking_handle(ak_msg_t *msg) {
    switch (msg->sig) {
    case AC_DISPLAY_INITIAL: {
    } break;

    case AC_DISPLAY_BUTON_MODE_PRESSED:
    case AC_DISPLAY_BUTON_UP_PRESSED:
    case AC_DISPLAY_BUTON_DOWN_PRESSED:
    {
        SCREEN_TRAN(scr_menu_handle, &scr_menu);
        BUZZER_PlaySound(BUZZER_SOUND_CLICK);
    } break;

    default:
        break;
    }
}
////////////////////////////////////////////////////////////////////////////////////////////////////
//    Name    : udpate_high_score
//    Function: Udpate highest score table
//    Argument: uint32_t score: New score
//    Return  : None
//    Created : V.Vu
//    Change  : 
//    Note    : 
////////////////////////////////////////////////////////////////////////////////////////////////////
void udpate_high_score(uint32_t score)
{
    /* Draw ranking list */
    for (int8_t i = 0; i<RANKING_MAX ; i++)
    {
        if(score >= g_ranking[i].score)
        {
            for (int8_t y = RANKING_MAX-1; y>i ; y--)
            {
                memcpy(g_ranking[y].name, g_ranking[y-1].name, SETTING_MAX_NAME);
                g_ranking[y].score = g_ranking[y-1].score;
            }
            memcpy(g_ranking[i].name, user_name, SETTING_MAX_NAME);
            g_ranking[i].score = score;
            return;
        }
    }
}
////////////////////////////////////////////////////////////////////////////////////////////////////
//    Name    : get_highest_score
//    Function: Get highest scrore
//    Argument: None
//    Return  : Highest score
//    Created : V.Vu
//    Change  : 
//    Note    : 
////////////////////////////////////////////////////////////////////////////////////////////////////
uint32_t get_highest_score(void){
    return g_ranking[0].score;
}
/* ************************************* End of File ******************************************** */





