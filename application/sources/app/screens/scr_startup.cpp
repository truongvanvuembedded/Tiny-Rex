//==================================================================================================
//	File Name	:
//	CPU Type	:
//	Builder		:
//	Coding		:
//	History		:
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
#include "scr_startup.h"
//==================================================================================================
//	Local define
//==================================================================================================
#define AK_LOGO_AXIS_X (23)
#define AK_LOGO_TEXT (AK_LOGO_AXIS_X + 4)
//==================================================================================================
//	Local define I/O
//==================================================================================================

//==================================================================================================
//	Local Struct Template
//==================================================================================================
static void view_scr_startup();
view_dynamic_t dyn_view_startup = {
    {
        .item_type = ITEM_TYPE_DYNAMIC,
    },
    view_scr_startup};

view_screen_t scr_startup = {
    &dyn_view_startup,
    ITEM_NULL,
    ITEM_NULL,

    .focus_item = 0,
};
//==================================================================================================
//	Local RAM
//==================================================================================================

//==================================================================================================
//	Local ROM
//==================================================================================================

//==================================================================================================
//	Local Function Prototype
//==================================================================================================

//==================================================================================================
//	Source Code
//==================================================================================================
////////////////////////////////////////////////////////////////////////////////////////////////////
//    Name    : view_scr_startup
//    Function: draw screen startup
//    Argument: None
//    Return  : None
//    Created : V.Vu
//    Change  :
//    Note    :
////////////////////////////////////////////////////////////////////////////////////////////////////
void view_scr_startup()
{
    /* ak logo */
    view_render.clear();
    view_render.setTextSize(1);
    view_render.setTextColor(WHITE);
    view_render.setCursor(AK_LOGO_AXIS_X, 3);
    view_render.print("   __    _  _ ");
    view_render.setCursor(AK_LOGO_AXIS_X, 10);
    view_render.print("  /__\\  ( )/ )");
    view_render.setCursor(AK_LOGO_AXIS_X, 20);
    view_render.print(" /(__)\\ (   (");
    view_render.setCursor(AK_LOGO_AXIS_X, 30);
    view_render.print("(__)(__)(_)\\_)");
    view_render.setCursor(AK_LOGO_TEXT, 42);
    view_render.print("Active Kernel");
}
////////////////////////////////////////////////////////////////////////////////////////////////////
//    Name    : scr_startup_handle
//    Function: Handle message send to startup screen
//    Argument: ak_msg_t *msg: Message information
//    Return  : None
//    Created : V.Vu
//    Change  :
//    Note    :
////////////////////////////////////////////////////////////////////////////////////////////////////
void scr_startup_handle(ak_msg_t* msg)
{
    switch (msg->sig)
    {
    case TINY_REX_DISPLAY_INITIAL:
    {
        APP_DBG_SIG("TINY_REX_DISPLAY_INITIAL\n");
        view_render.initialize();
        view_render_display_on();
        timer_set(TINY_REX_TASK_DISPLAY_ID, TINY_REX_DISPLAY_SHOW_IDLE, TINY_REX_DISPLAY_STARTUP_INTERVAL, TIMER_ONE_SHOT);
    }
    break;

    case TINY_REX_DISPLAY_BUTTON_MODE_PRESSED:
    {
        APP_DBG_SIG("TINY_REX_DISPLAY_BUTTON_MODE_PRESSED\n");
        timer_remove_attr(TINY_REX_TASK_DISPLAY_ID, TINY_REX_DISPLAY_SHOW_IDLE);
        SCREEN_TRAN(scr_menu_handle, &scr_menu);
    }
    break;

    case TINY_REX_DISPLAY_SHOW_IDLE:
    {
        APP_DBG_SIG("TINY_REX_DISPLAY_SHOW_IDLE\n");
        timer_remove_attr(TINY_REX_TASK_DISPLAY_ID, TINY_REX_DISPLAY_SHOW_IDLE);
        SCREEN_TRAN(scr_idle_handle, &scr_idle);
    }
    break;

    default:
        break;
    }
}
/* ************************************* End of File ******************************************** */