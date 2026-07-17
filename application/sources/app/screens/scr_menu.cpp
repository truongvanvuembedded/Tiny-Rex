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

static void view_scr_menu()
{
    // Configure text properties
    view_render.setTextSize(1);
    view_render.setTextColor(WHITE);
    // Title
    view_render.setCursor(39, 8);
    view_render.print("T-Rex Game");
    // Mode
    view_render.setCursor(22, 23);
    view_render.print(items_name[current_location]);
    /* Draw object */
    draw_tiny_rex_object();
    draw_tree_object();
    draw_bird_object();
}

void scr_menu_handle(ak_msg_t* msg)
{
    switch (msg->sig)
    {
    case SCREEN_ENTRY:
    {
        APP_DBG_SIG("SCREEN_MENU_ENTRY\n");
        // Init Variable
        current_location = 0;
        task_post_pure_msg(TINY_REX_OBJECT_ID, EVENT_TINY_REX_OBJECT_SETUP);
        task_post_pure_msg(TINY_REX_OBJECT_ID, EVENT_TINY_REX_OBJECT_RUN);
        task_post_pure_msg(TREE_OBJECT_ID, EVENT_TREE_OBJECT_SETUP);
        task_post_pure_msg(TREE_OBJECT_ID, EVENT_TREE_OBJECT_STAND);
        task_post_pure_msg(BIRD_OBJECT_ID, EVENT_BIRD_OBJECT_SETUP);
        task_post_pure_msg(BIRD_OBJECT_ID, EVENT_BIRD_OBJECT_STAND_FLY);
        timer_set(
            AC_TASK_DISPLAY_ID,
            AC_DISPLAY_MENU_ANIMATION_UPDATE,
            AC_DISPLAY_MENU_ANIMATION_UPDATE_INTERVAL,
            TIMER_PERIODIC);
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
            // BUZZER_PlaySound(BUZZER_SOUND_CLICK);
            SCREEN_TRAN(scr_play_handle_signal, &scr_play);
        }
        timer_remove_attr(
            AC_TASK_DISPLAY_ID,
            AC_DISPLAY_MENU_ANIMATION_UPDATE);
    }
    break;

    case AC_DISPLAY_MENU_ANIMATION_UPDATE:
    {
        task_post_pure_msg(TINY_REX_OBJECT_ID, EVENT_TINY_REX_OBJECT_UPDATE);
        task_post_pure_msg(BIRD_OBJECT_ID, EVENT_TINY_REX_OBJECT_UPDATE);
    }
    break;

    default:
        break;
    }
}