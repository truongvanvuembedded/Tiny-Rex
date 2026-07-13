#include "scr_play.h"

#define NUMBER_ITEMS (4)

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

static void view_scr_play()
{
    // Line
    view_render.drawBitmap(
        1,
        59,
        epd_bitmap_line,
        128,
        5,
        WHITE);
    // T-Rex
    view_render.drawBitmap(
        0,
        39,
        epd_bitmap_t_rex_1,
        23,
        25,
        WHITE);
}

void scr_play_handle(ak_msg_t* msg)
{
    switch (msg->sig)
    {
    case SCREEN_ENTRY:
    {
        APP_DBG_SIG("SCREEN_PLAY_ENTRY\n");
    }
    break;

    case AC_DISPLAY_BUTON_DOWN_PRESSED:
    {
        BUZZER_PlaySound(BUZZER_SOUND_CLICK);
    }
    break;

    case AC_DISPLAY_BUTON_UP_PRESSED:
    {
        BUZZER_PlaySound(BUZZER_SOUND_CLICK);
    }
    break;

    case AC_DISPLAY_BUTON_MODE_PRESSED:
    {
        BUZZER_PlaySound(BUZZER_SOUND_CLICK);
    }
    break;

    default:
        break;
    }
}