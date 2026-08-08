#include "scr_setting.h"

static void view_scr_setting();

view_dynamic_t dyn_view_setting = {
	{
		.item_type = ITEM_TYPE_DYNAMIC,
	},
	view_scr_setting
};

view_screen_t scr_setting = {
	&dyn_view_setting,
	ITEM_NULL,
	ITEM_NULL,

	.focus_item = 0,
};

void view_scr_setting() {

}

void scr_setting_handle(ak_msg_t *msg) {
	switch (msg->sig) {
	case AC_DISPLAY_INITIAL: {
	} break;

	case AC_DISPLAY_BUTON_MODE_PRESSED:
	{
		APP_DBG_SIG("AC_DISPLAY_BUTON_MODE_PRESSED\n");
	} break;
	case AC_DISPLAY_BUTON_UP_PRESSED:
	{
		APP_DBG_SIG("AC_DISPLAY_BUTON_UP_PRESSED\n");
	} break;
	case AC_DISPLAY_BUTON_DOWN_PRESSED:
	{
		APP_DBG_SIG("AC_DISPLAY_BUTON_DOWN_PRESSED\n");
		SCREEN_TRAN(scr_menu_handle, &scr_menu);
	} break;

	default:
		break;
	}
}
