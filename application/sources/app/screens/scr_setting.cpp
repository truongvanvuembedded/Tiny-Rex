#include "scr_setting.h"

/*==================================================================================================
 * Local Define
 *================================================================================================*/
#define SETTING_ITEM_NAME       (0)
#define SETTING_ITEM_MUTE       (1)
#define SETTING_ITEM_EXIT       (2)
#define SETTING_ITEM_MAX        (3)

/*==================================================================================================
 * Local Type
 *================================================================================================*/
typedef struct
{
    const char *name;
} setting_item_info_t;

/*==================================================================================================
 * Local Variable
 *================================================================================================*/
static int8_t setting_item = SETTING_ITEM_NAME;

static char user_name[6] = "VU";
static bool mute_enable = true;

static const setting_item_info_t setting_item_table[SETTING_ITEM_MAX] =
{
    [SETTING_ITEM_NAME] = {"NAME"},
    [SETTING_ITEM_MUTE] = {"MUTE"},
    [SETTING_ITEM_EXIT] = {"EXIT"},
};

/*==================================================================================================
 * Local Function
 *================================================================================================*/
static void view_scr_setting(void);

/*==================================================================================================
 * Global Variable
 *================================================================================================*/
view_dynamic_t dyn_view_setting =
{
    {
        .item_type = ITEM_TYPE_DYNAMIC,
    },
    view_scr_setting
};

view_screen_t scr_setting =
{
    &dyn_view_setting,
    ITEM_NULL,
    ITEM_NULL,

    .focus_item = 0,
};

/*==================================================================================================
 * Function
 *================================================================================================*/
void scr_setting_handle(ak_msg_t *msg)
{
    switch (msg->sig)
    {
        case AC_DISPLAY_INITIAL:
        {
        }
        break;

        case AC_DISPLAY_BUTON_DOWN_PRESSED:
        {
            APP_DBG_SIG("AC_DISPLAY_BUTON_DOWN_PRESSED\n");

            setting_item++;

            if (setting_item >= SETTING_ITEM_MAX)
            {
                setting_item = 0;
            }
            BUZZER_PlaySound(BUZZER_SOUND_CLICK);
        }
        break;

        case AC_DISPLAY_BUTON_UP_PRESSED:
        {
            APP_DBG_SIG("AC_DISPLAY_BUTON_UP_PRESSED\n");

            setting_item--;

            if (setting_item < 0)
            {
                setting_item = SETTING_ITEM_MAX - 1;
            }
            BUZZER_PlaySound(BUZZER_SOUND_CLICK);
        }
        break;

        case AC_DISPLAY_BUTON_MODE_PRESSED:
        {
            APP_DBG_SIG("AC_DISPLAY_BUTON_MODE_PRESSED\n");

            if (setting_item == SETTING_ITEM_EXIT)
            {
                SCREEN_TRAN(scr_menu_handle, &scr_menu);
            }
            else if (setting_item == SETTING_ITEM_MUTE)
            {
                mute_enable = !mute_enable;
                BUZZER_Silent(mute_enable);
            }
            BUZZER_PlaySound(BUZZER_SOUND_CLICK);
        }
        break;

        default:
        break;
    }
}

/*==================================================================================================
 * View
 *================================================================================================*/
static void view_scr_setting(void)
{
    view_render.setTextSize(1);
    view_render.setTextColor(WHITE);

    /* Title */
    view_render.setCursor(45, 5);
    view_render.print("SETTING");

    /* Setting item */
    for (uint8_t i = 0; i < SETTING_ITEM_MAX; i++)
    {
        uint8_t y = 20 + (i * 12);

        view_render.setCursor(10, y);

        if (setting_item == i)
        {
            view_render.print("> ");
        }
        else
        {
            view_render.print("  ");
        }

        view_render.print(setting_item_table[i].name);

        /* Value */
        if (i == SETTING_ITEM_NAME)
        {
            view_render.print(": ");
            view_render.print(user_name);
        }
        else if (i == SETTING_ITEM_MUTE)
        {
            view_render.print(": ");

            if (mute_enable)
            {
                view_render.print("ON");
            }
            else
            {
                view_render.print("OFF");
            }
        }
    }
}