#ifndef __APP_EEPROM_H__
#define __APP_EEPROM_H__

#include <stdint.h>
#include "app.h"

/**
  *****************************************************************************
  * EEPROM define address.
  *
  *****************************************************************************
  */
#define EEPROM_START_ADDR           (0X0000)
#define EEPROM_END_ADDR             (0X1000)

#define EEPROM_SCORE_START_ADDR     (0X0010)
#define EEPROM_SCORE_PLAY_ADDR      (0X00FA)

#define TINY_REX_EEPROM_MAGIC_NUMBER   ((uint32_t)0x54524558) /*TREX*/

/******************************************************************************/
/* Tiny-Rex game */
/******************************************************************************/
#define RANKING_MAX (5)
#define SETTING_MAX_NAME        (6)

/* Ranking structure */
typedef struct
{
    uint32_t score;
    char name[SETTING_MAX_NAME+1];
} ranking_t;

typedef struct
{
	ranking_t ranking[RANKING_MAX];
}eeprom_ranking_t;

#ifdef __cplusplus
extern "C" {
#endif

bool tinyRex_game_score_read(eeprom_ranking_t* data);
bool tinyRex_game_score_write(eeprom_ranking_t* data);

#ifdef __cplusplus
}
#endif

#endif //__APP_EEPROM_H__
