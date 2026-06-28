#include "scr_tiny_rex_game.h"

static void view_scr_tity_rex_game();

view_dynamic_t dyn_view_tiny_rex_game = {
	{
		.item_type = ITEM_TYPE_DYNAMIC,
	},
	view_scr_tity_rex_game
};

view_screen_t scr_tiny_rex_game = {
	&dyn_view_tiny_rex_game,
	ITEM_NULL,
	ITEM_NULL,

	.focus_item = 0,
};

void view_scr_tity_rex_game() {
	view_render.clear();
	view_render.drawBitmap(	64, \
		32, \
		bitmap_tiny_rex, \
		32, \
		32, \
		WHITE);
}

void scr_tiny_rex_game_handle(ak_msg_t *msg) {
	switch (msg->sig) 
	{
	case AC_DISPLAY_BUTON_UP_PRESSED:
	case AC_DISPLAY_BUTON_DOWN_PRESSED:
	case AC_DISPLAY_BUTON_MODE_PRESSED: {
		APP_DBG_SIG("AC_DISPLAY_BUTON_%s_PRESSED\n", msg->sig == AC_DISPLAY_BUTON_MODE_PRESSED ? "MODE" :\
													 msg->sig == AC_DISPLAY_BUTON_DOWN_PRESSED ? "DOWN" : "UP");
		SCREEN_TRAN(scr_qrcode_handle, &scr_qrcode);
	} break;
	default:
		break;
	}
}