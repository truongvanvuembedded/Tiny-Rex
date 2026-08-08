#include "scr_ranking.h"
#include "scr_setting.h"

#define RANKING_MAX 5

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

typedef struct
{
    char name[SETTING_MAX_NAME+1];
    uint32_t score;
} ranking_t;

static ranking_t g_ranking[RANKING_MAX] =
{
    {" ", 0},
    {" ", 0},
    {" ", 0},
    {" ", 0},
    {" ", 0},
};

static void drawRanking(void);

void view_scr_ranking() {
    drawRanking();
}

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

static void drawRanking(void)
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
void udpate_high_score(uint32_t score)
{
    /* Draw ranking list */
    for (uint8_t i = 0; i<RANKING_MAX ; i++)
    {
        if(score >= g_ranking[i].score)
        {
            for (uint8_t y = i; y<RANKING_MAX-1 ; y++)
            {
                memcpy(g_ranking[y+1].name, g_ranking[y].name, SETTING_MAX_NAME);
                g_ranking[y+1].score = g_ranking[y].score;
            }
            memcpy(g_ranking[i].name, user_name, SETTING_MAX_NAME);
            g_ranking[i].score = score;
            return;
        }
    }
}
uint32_t get_highest_score(void){
    return g_ranking[0].score;
}