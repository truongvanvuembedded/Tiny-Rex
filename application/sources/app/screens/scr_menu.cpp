// Define varaible
#define REAL_RAM
#include "screens.h"
#undef REAL_RAM
#include "scr_menu.h"

#define NUMBER_ITEMS (4)

static void view_scr_menu();

view_dynamic_t dyn_view_scr_menu = {
    {
        .item_type = ITEM_TYPE_DYNAMIC,
    },
    view_scr_menu};

view_screen_t scr_menu = {
    &dyn_view_scr_menu,
    ITEM_NULL,
    ITEM_NULL,

    .focus_item = 0,
};

static uint8_t current_location = 0;
static uint8_t is_ducking = 0;
static const char* const items_name[NUMBER_ITEMS] = {
    "<<   Play   >>",
    "<<  Setting >>",
    "<<   Rank   >>",
    "<<   Exit   >>",
};

const unsigned char* toggle_frame(
    const unsigned char* current,
    const unsigned char* frame1,
    const unsigned char* frame2)
{
    return (current == frame1) ? frame2 : frame1;
}

static void menu_animation_update()
{
    // Ducking toggle
    is_ducking = !is_ducking;
    // Toggle frames
    image_trex_running = toggle_frame(image_trex_running,
                 epd_bitmap_t_rex__1_1,
                 epd_bitmap_t_rex__1_2);

    image_trex_ducking = toggle_frame(image_trex_ducking,
                 epd_bitmap_t_rex_1,
                 epd_bitmap_t_rex_2);

    image_bird_flying = toggle_frame(image_bird_flying,
                 epd_bitmap_bird_1,
                 epd_bitmap_bird_2);
}

static void view_scr_menu()
{
    view_render.clear();

    view_render.drawBitmap(
        0,
        39,
        image_trex_running,
        23,
        25,
        WHITE);

    view_render.drawBitmap(
        30,
        (is_ducking) ? 49 : 39,
        image_trex_ducking,
        (is_ducking) ? 30 : 23,
        (is_ducking) ? 15 : 25,
        WHITE);

    view_render.drawBitmap(
        108,
        37,
        epd_bitmap_tree_1,
        15,
        27,
        WHITE);

    view_render.drawBitmap(
        69,
        44,
        image_bird_flying,
        29,
        21,
        WHITE);

    // Configure text properties
    view_render.setTextSize(1);
    view_render.setTextColor(WHITE);
    // Title
    view_render.setCursor(39, 8);
    view_render.print("T-Rex Game");
    // Mode
    view_render.setCursor(22, 23);
    view_render.print(items_name[current_location]);
    //
    view_render.drawBitmap(
        39,
        17,
        image_ButtonLeftSmall_bits,
        3,
        5,
        WHITE);
    view_render.drawBitmap(
        86,
        17,
        image_ButtonRightSmall_bits,
        3,
        5,
        WHITE);
}

void scr_menu_handle(ak_msg_t* msg)
{
    switch (msg->sig)
    {
    case SCREEN_ENTRY:
    {
        APP_DBG_SIG("SCREEN_MENU_ENTRY\n");

        timer_set(
            AC_TASK_DISPLAY_ID,
            AC_DISPLAY_MENU_ANIMATION_UPDATE,
            AC_DISPLAY_MENU_ANIMATION_UPDATE_INTERVAL,
            TIMER_PERIODIC);
        // Init Variable
        image_trex_running = epd_bitmap_t_rex__1_1;
        image_trex_ducking = epd_bitmap_t_rex_1;
        image_bird_flying = epd_bitmap_bird_1;
        current_location = 0;
        is_ducking = 0;
    }
    break;

    case AC_DISPLAY_MENU_ANIMATION_UPDATE:
    {
        menu_animation_update();
    }
    break;

    case AC_DISPLAY_BUTON_DOWN_PRESSED:
    {
        current_location++;
        if (current_location >= NUMBER_ITEMS)
        {
            current_location = 0;
        }
        BUZZER_PlaySound(BUZZER_SOUND_CLICK);
    }
    break;

    case AC_DISPLAY_BUTON_UP_PRESSED:
    {
        if (current_location == 0)
        {
            current_location = NUMBER_ITEMS - 1;
        }
        else
        {
            current_location--;
        }
        BUZZER_PlaySound(BUZZER_SOUND_CLICK);
    }
    break;

    case AC_DISPLAY_BUTON_MODE_PRESSED:
    {
        if (current_location == 0)
        {
            BUZZER_PlaySound(BUZZER_SOUND_CLICK);
            SCREEN_TRAN(scr_play_handle_signal, &scr_play);
            timer_remove_attr(
                AC_TASK_DISPLAY_ID,
                AC_DISPLAY_MENU_ANIMATION_UPDATE);
        }
    }
    break;

    default:
        break;
    }
}