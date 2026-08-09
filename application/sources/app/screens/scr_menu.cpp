//==================================================================================================
//	File Name	: scr_menu
//	CPU Type	: 
//	Builder		:
//	Coding		: V.Vu
//	History		: 08/09/2026 New
//	Outline		: Handle message and render objects in menu screen.
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
#include "screens.h"
#include "scr_menu.h"
//==================================================================================================
//	Local define
//==================================================================================================
/* Screens */
enum
{
    EM_SCREEN_PLAY,
    EM_SCREEN_SETTING,
    EM_SCREEN_RANKING,
    EM_SCREEN_EXIT,
    EM_SCREEN_NUM,
};
//==================================================================================================
//	Local define I/O
//==================================================================================================

//==================================================================================================
//	Struct Template
//==================================================================================================
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
//==================================================================================================
//	Local RAM
//==================================================================================================
static uint8_t current_location;
//==================================================================================================
//	Local ROM
//==================================================================================================
static const uint8_t MENU_BOX_X[] = {10, 38, 68, 99};
static const uint8_t MENU_BOX_W[] = {19, 20, 21, 20};
static const uint8_t ICON_BITMAP_IDEX[] = {BITMAP_T_REX_STAND, BITMAP_GAME_SETTING_ICON, BITMAP_GAME_RANKING_ICON, BITMAP_GAME_EXIT_ICON};
// Menu items name
static const char *menu_items_name[EM_SCREEN_NUM] = {
    "T-REX GAME", // item 1
    "SETTING",    // item 2
    "RANKING",    // item 3
    "EXIT",       // item 4
};
//==================================================================================================
//	Local Function Prototype
//==================================================================================================
static void draw_menu_icon(void);
//==================================================================================================
//	Source Code
//==================================================================================================
////////////////////////////////////////////////////////////////////////////////////////////////////
//	Name     : scr_menu_handle
//	Function : Handle message send to menu screen
//	Argument : ak_msg_t* msg: Message information
//	Return   : None
//	Created  : 13/07/2026 V.Vu
//	Changed  : -
//	Remarks  : -
////////////////////////////////////////////////////////////////////////////////////////////////////
void scr_menu_handle(ak_msg_t* msg)
{
    switch (msg->sig)
    {
    case SCREEN_ENTRY:
    {
        APP_DBG_SIG("SCREEN_MENU_ENTRY\n");
    }
    break;

    case AC_DISPLAY_BUTON_UP_PRESSED:
    {
        current_location++;
        if (current_location >= EM_SCREEN_NUM)
        {
            current_location = EM_SCREEN_PLAY;
        }
        BUZZER_PlaySound(BUZZER_SOUND_CLICK);
    }
    break;

    case AC_DISPLAY_BUTON_DOWN_PRESSED:
    {
        if (current_location == EM_SCREEN_PLAY)
        {
            current_location = EM_SCREEN_NUM - 1;
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
        if (current_location == EM_SCREEN_PLAY)
        {
            SCREEN_TRAN(scr_play_handle_signal, &scr_play);
        }
        else if (current_location == EM_SCREEN_SETTING)
        {
            SCREEN_TRAN(scr_setting_handle, &scr_setting);
        }
        else if (current_location == EM_SCREEN_RANKING)
        {
            SCREEN_TRAN(scr_ranking_handle, &scr_ranking);
        }
        else if (current_location == EM_SCREEN_EXIT)
        {
        }

        BUZZER_PlaySound(BUZZER_SOUND_CLICK);
    }
    break;

    default:
        break;
    }
}
////////////////////////////////////////////////////////////////////////////////////////////////////
//	Name     : view_scr_menu
//	Function : Draw objects in menu screen
//	Argument : None
//	Return   : None
//	Created  : 13/07/2026 V.Vu
//	Changed  : -
//	Remarks  : -
////////////////////////////////////////////////////////////////////////////////////////////////////
static void view_scr_menu()
{
    // Draw selected menu item name centered at Y=50
    view_render.setTextSize(1);
    view_render.setTextColor(WHITE);
    const char* name = menu_items_name[current_location];
    uint8_t name_len = strlen(name);
    uint8_t name_x = 64 - (name_len * 6) / 2;
    view_render.setCursor(name_x, 2);
    view_render.print(name);
    /* Draw line for seperate */
    view_render.drawLine(0,15,128,15,WHITE);
    /* Draw object */
    draw_menu_icon();
}
////////////////////////////////////////////////////////////////////////////////////////////////////
//	Name     : draw_menu_icon
//	Function : Draw menu icon
//	Argument : None
//	Return   : None
//	Created  : 13/07/2026 V.Vu
//	Changed  : -
//	Remarks  : -
////////////////////////////////////////////////////////////////////////////////////////////////////
static void draw_menu_icon(void)
{
  // Draw the 4 cards horizontally
  for (uint8_t i = 0; i < EM_SCREEN_NUM; i++)
  {
    uint8_t bx = MENU_BOX_X[i];
    uint8_t bx_w = MENU_BOX_W[i];
    
    if (i == current_location)
    {
      // Highlighted item: solid white background
      view_render.fillRoundRect(bx, 32, bx_w, 20, 0, WHITE);
    }
    else
    {
      // Unselected item: outline only
      view_render.drawRoundRect(bx, 32, bx_w, 20, 0, WHITE);
    }
      view_render.drawBitmap(
        bx+2,
        34,
        g_bitmap_table[ICON_BITMAP_IDEX[i]].bitmap,
        g_bitmap_table[ICON_BITMAP_IDEX[i]].width,
        g_bitmap_table[ICON_BITMAP_IDEX[i]].height,
        (i == current_location)?BLACK:WHITE);
  }
}
/* ************************************* End of File ******************************************** */