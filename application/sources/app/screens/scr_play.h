#ifndef __SCR_PLAY_H__
#define __SCR_PLAY_H__

#include "screens.h"
/* Game state */
enum{
	EM_GAME_STATE_IDLE = 0,
	EM_GAME_STATE_PLAYING,
	EM_GAME_STATE_OVER,
};
/* Score */
typedef struct{
	uint32_t skip_count;
	uint32_t threshold;
	uint32_t current_score;
	uint32_t high_score;
	uint8_t animation_timer;
}score_t;
#endif //__SCR_PLAY_H__
