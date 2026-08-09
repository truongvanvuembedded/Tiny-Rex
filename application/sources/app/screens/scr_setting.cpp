#include "scr_setting.h"
#include "scr_ranking.h"
/*==================================================================================================
 * Local Define
 *================================================================================================*/
#define SETTING_ITEM_NAME       (0)
#define SETTING_ITEM_LEN        (1)
#define SETTING_ITEM_MUTE       (2)
#define SETTING_ITEM_EXIT       (3)
#define SETTING_ITEM_MAX        (4)

#define ALPHABET_SIZE           (26)
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
static int8_t setting_item;
/* Name setting */
static char current_user_name[SETTING_MAX_NAME+1];
static uint8_t len_name;
static uint8_t curr_name_idx;
static int8_t alphabet_idx[SETTING_MAX_NAME];
static bool change_name;
/* Sound */
static bool mute_enable = true;

static const setting_item_info_t setting_item_table[SETTING_ITEM_MAX] =
{
    [SETTING_ITEM_NAME] = {"NAME"},
    [SETTING_ITEM_LEN]  = {"LEN"},
    [SETTING_ITEM_MUTE] = {"MUTE"},
    [SETTING_ITEM_EXIT] = {"EXIT"},
};
static const char g_alphabet[ALPHABET_SIZE] =
{
    'A', 'B', 'C', 'D', 'E', 'F', 'G',
    'H', 'I', 'J', 'K', 'L', 'M', 'N',
    'O', 'P', 'Q', 'R', 'S', 'T', 'U',
    'V', 'W', 'X', 'Y', 'Z'
};
/*==================================================================================================
 * Local Function
 *================================================================================================*/
static void view_scr_setting(void);
static void udpate_user_name(void);
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
////////////////////////////////////////////////////////////////////////////////////////////////////
//    Name    : scr_setting_handle
//    Function: Handle message for screen setting
//    Argument: ak_msg_t *msg: Message information
//    Return  : None
//    Created : V.Vu
//    Change  : 
//    Note    : 
////////////////////////////////////////////////////////////////////////////////////////////////////
void scr_setting_handle(ak_msg_t *msg)
{
    switch (msg->sig)
    {
        case SCREEN_ENTRY:
        {
            change_name = false;
        }
        break;

        case AC_DISPLAY_BUTON_DOWN_PRESSED:
        {
            APP_DBG_SIG("AC_DISPLAY_BUTON_DOWN_PRESSED\n");
            if(change_name){
                /* Udpate charater */
                alphabet_idx[curr_name_idx]++;
                if(alphabet_idx[curr_name_idx] >= ALPHABET_SIZE){
                    alphabet_idx[curr_name_idx] = 0;
                }
            }else{
                /* Move pointer to next item */
                setting_item++;
                if (setting_item >= SETTING_ITEM_MAX)
                {
                    setting_item = 0;
                }
            }
            BUZZER_PlaySound(BUZZER_SOUND_CLICK);
        }
        break;

        case AC_DISPLAY_BUTON_UP_PRESSED:
        {
            APP_DBG_SIG("AC_DISPLAY_BUTON_UP_PRESSED\n");
            if(change_name){
                /* Udpate charater */
                alphabet_idx[curr_name_idx]--;
                if(alphabet_idx[curr_name_idx] < 0){
                    alphabet_idx[curr_name_idx] = ALPHABET_SIZE-1;

                }
            }else{
                /* Move pointer to next item */
                setting_item--;
                if (setting_item < 0)
                {
                    setting_item = SETTING_ITEM_MAX - 1;
                }
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
            else if (setting_item == SETTING_ITEM_NAME)
            {
                if(change_name){
                    curr_name_idx++;
                    if(curr_name_idx >= len_name){
                        curr_name_idx = 0;
                    }
                }
            }
            else if (setting_item == SETTING_ITEM_LEN)
            {
                len_name++;
                if(len_name > SETTING_MAX_NAME){
                    len_name = 0;
                }
            }
            else if (setting_item == SETTING_ITEM_MUTE)
            {
                mute_enable = !mute_enable;
                BUZZER_Silent(mute_enable);
            }
            BUZZER_PlaySound(BUZZER_SOUND_CLICK);
        }
        break;

        case AC_DISPLAY_BUTON_MODE_LONG_PRESS:
        {
            if (setting_item == SETTING_ITEM_NAME)
            {
                change_name = !change_name;
            }
        }
        break;

        default:
        break;
    }
}
////////////////////////////////////////////////////////////////////////////////////////////////////
//    Name    : view_scr_setting
//    Function: Draw object in setting screen
//    Argument: None
//    Return  : None
//    Created : V.Vu
//    Change  : 
//    Note    : 
////////////////////////////////////////////////////////////////////////////////////////////////////
static void view_scr_setting(void)
{
    view_render.setTextSize(1);
    view_render.setTextColor(WHITE);

    /* Title */
    view_render.setCursor(45, 5);
    view_render.print("SETTING");
    /* Update user name */
    udpate_user_name();
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
            view_render.print(current_user_name);
            //view_render.drawLine()
        }
        else if (i == SETTING_ITEM_LEN)
        {
            view_render.print(" : ");
            view_render.print(len_name);
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
////////////////////////////////////////////////////////////////////////////////////////////////////
//    Name    : udpate_user_name
//    Function: Update current user name when user enter or delete
//    Argument: None
//    Return  : None
//    Created : V.Vu
//    Change  : 
//    Note    : 
////////////////////////////////////////////////////////////////////////////////////////////////////
static void udpate_user_name(void)
{
    uint8_t au1_ForC;
    for(au1_ForC = 0; au1_ForC < SETTING_MAX_NAME; au1_ForC++)
    {
        if(au1_ForC < len_name){
            current_user_name[au1_ForC] = g_alphabet[alphabet_idx[au1_ForC]];
        }else{
            current_user_name[au1_ForC] = ' ';
            alphabet_idx[au1_ForC] = 0;
        }
    }
    current_user_name[SETTING_MAX_NAME] = '\0';
}
////////////////////////////////////////////////////////////////////////////////////////////////////
//    Name    : get_current_user_name
//    Function: Get current user name
//    Argument: None
//    Return  : None
//    Created : V.Vu
//    Change  : 
//    Note    : Only get user name correct when name updated before game play.
////////////////////////////////////////////////////////////////////////////////////////////////////
bool get_current_user_name(char* user_name, uint8_t len)
{
	if(!user_name || len < SETTING_MAX_NAME || !len_name){
		return false;
	}
	memcpy(user_name, &current_user_name[0], SETTING_MAX_NAME);
	return true;
}