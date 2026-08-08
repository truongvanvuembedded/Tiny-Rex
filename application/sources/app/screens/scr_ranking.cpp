#include "scr_ranking.h"

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

void view_scr_ranking() {

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
	} break;

	default:
		break;
	}
}
