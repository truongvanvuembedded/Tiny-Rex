//==================================================================================================
//	File Name	: app_eeprom.cpp
//	CPU Type	:
//	Builder		:
//	Coding		: V.Vu
//	History		: 08/09/2026 New
//	Outline		: Application layer for save and read data in eeprom.
/*
 * EEPROM layout for Tiny-Rex game persistent data.
 *
 * Each record is stored as:
 *   [magic_number][payload struct][8-bit additive checksum]
 *
 * magic_number identifies an initialized record and prevents reading erased or
 * unrelated EEPROM bytes as valid app data. The checksum is the byte sum over
 * magic_number + payload; it is lightweight corruption detection, not security.
 *
 * Current layout uses fixed addresses only:
 *   EEPROM_SCORE_START_ADDR   -> tiny_game_score_eeprom_t
 *
 * There is no wear-leveling in this module. Callers should avoid repeated
 * writes when the value has not changed.
 */

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
#include "./app_eeprom.h"
#include "eeprom.h"
//==================================================================================================
//	Local define
//==================================================================================================
#define TINY_REX_SCORE_CHECKSUM_SIZE	  (sizeof(uint32_t) + sizeof(eeprom_ranking_t))
//==================================================================================================
//	Local define I/O
//==================================================================================================

//==================================================================================================
//	Local Struct Template
//==================================================================================================
typedef struct {
    uint32_t magic_number;
    eeprom_ranking_t data;
    uint8_t check_sum;
} tiny_game_score_eeprom_t;

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
//    Name    : trex_game_eeprom_checksum
//    Function: Get checksum value from data
//    Argument: uint8_t *data: Input data need get checksum value
//              uint32_t size: Size of data
//    Return  : Checksum value
//    Change  : 
//    Note    : 
////////////////////////////////////////////////////////////////////////////////////////////////////
static uint8_t trex_game_eeprom_checksum(uint8_t *data, uint32_t size) {
    uint8_t check_sum = 0;

    for (uint32_t i = 0; i < size; i++) {
        check_sum += data[i];
    }

    return check_sum;
}
////////////////////////////////////////////////////////////////////////////////////////////////////
//    Name    : trex_game_eeprom_update_checksum
//    Function: Udpate magic number before get checksum value
//    Argument: uint32_t *magic_number: Pointer to magic number
//              uint8_t *check_sum: Pointer to checksum number for get checksum value
//              uint32_t check_sum_size: Size need checksum
//    Return  : None
//    Change  : 
//    Note    : 
////////////////////////////////////////////////////////////////////////////////////////////////////
static void trex_game_eeprom_update_checksum(uint32_t *magic_number, uint8_t *check_sum, uint32_t check_sum_size) {
    *magic_number = TINY_REX_EEPROM_MAGIC_NUMBER;
    *check_sum	  = trex_game_eeprom_checksum((uint8_t *)magic_number, check_sum_size);
}
////////////////////////////////////////////////////////////////////////////////////////////////////
//    Name    : trex_game_eeprom_is_valid
//    Function: Check data readed from eeprom valid or not
//    Argument: uint32_t *magic_number: Pointer to magic number
//              uint8_t check_sum: Checksum value need check
//              uint32_t check_sum_size: Size need checksum
//    Return  : Checksum value
//    Change  : 
//    Note    : 
////////////////////////////////////////////////////////////////////////////////////////////////////
static bool trex_game_eeprom_is_valid(uint32_t *magic_number, uint8_t check_sum, uint32_t check_sum_size) {
    return (*magic_number == TINY_REX_EEPROM_MAGIC_NUMBER) && (check_sum == trex_game_eeprom_checksum((uint8_t *)magic_number, check_sum_size));
}
////////////////////////////////////////////////////////////////////////////////////////////////////
//    Name    : trex_game_score_set_default
//    Function: Clear ranking value if data read not match with checksum or format rule.
//    Argument: ranking_t *data: Pointer to ranking value need reset
//    Return  : Checksum value
//    Change  : 
//    Note    : 
////////////////////////////////////////////////////////////////////////////////////////////////////
static void trex_game_score_set_default(eeprom_ranking_t *data) {
    for(uint8_t au1_ForC = 0; au1_ForC<RANKING_MAX; au1_ForC++){
        memset(data->ranking[au1_ForC].name, 0, SETTING_MAX_NAME);
        data->ranking[au1_ForC].score = 0;
    }
}
////////////////////////////////////////////////////////////////////////////////////////////////////
//    Name    : trex_game_score_read
//    Function: Read score value from eeprom memory
//    Argument: ranking_t *data: Pointer to buffer need write when read success
//    Return  : Checksum value
//    Change  : 
//    Note    :
////////////////////////////////////////////////////////////////////////////////////////////////////
bool tinyRex_game_score_read(eeprom_ranking_t* data){
    tiny_game_score_eeprom_t eeprom_data;

    uint8_t ret =eeprom_read(EEPROM_SCORE_START_ADDR, (uint8_t *)&eeprom_data, sizeof(eeprom_data));

    if (ret == EEPROM_DRIVER_OK && trex_game_eeprom_is_valid(&eeprom_data.magic_number, eeprom_data.check_sum, TINY_REX_SCORE_CHECKSUM_SIZE)) {
        *data = eeprom_data.data;
        return true;
    }

  trex_game_score_set_default(data);
  return false; 
}
////////////////////////////////////////////////////////////////////////////////////////////////////
//    Name    : trex_game_score_write
//    Function: Write score value to eeprom memory
//    Argument: tiny_game_score_eeprom_t *data: Pointer to buffer need write to memory
//    Return  : Checksum value
//    Change  : 
//    Note    : 
////////////////////////////////////////////////////////////////////////////////////////////////////
bool tinyRex_game_score_write(eeprom_ranking_t* data){
    tiny_game_score_eeprom_t eeprom_data;

    eeprom_data.data = *data;
    trex_game_eeprom_update_checksum(&eeprom_data.magic_number, &eeprom_data.check_sum, TINY_REX_SCORE_CHECKSUM_SIZE);

    return eeprom_write(EEPROM_SCORE_START_ADDR, (uint8_t *)&eeprom_data, sizeof(eeprom_data)) == EEPROM_DRIVER_OK;
}
/* ************************************* End of File ******************************************** */