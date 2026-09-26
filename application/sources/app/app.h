/**
 ******************************************************************************
 * @author: GaoKong
 * @date:   13/08/2016
 ******************************************************************************
 **/

#ifndef __APP_H__
#define __APP_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include "ak.h"

#include "app_eeprom.h"
#include "app_data.h"

/*****************************************************************************/
/*  LIFE task define
 */
/*****************************************************************************/
/* define timer */
#define TINY_REX_LIFE_TASK_TIMER_LED_LIFE_INTERVAL (1000)

    /* define signal */
    enum
    {
        TINY_REX_LIFE_SYSTEM_CHECK = AK_USER_DEFINE_SIG,
    };

/*****************************************************************************/
/*  DISPLAY task define
 */
/*****************************************************************************/
/* define timer */
#define TINY_REX_DISPLAY_INITIAL_INTERVAL (100)
#define TINY_REX_DISPLAY_STARTUP_INTERVAL (2000)
#define TINY_REX_DISPLAY_MINIMUM_SCREEN_RENDER_INTERVAL_MS (50) /* 50ms => Max 20 FPS */
#define TINY_REX_DISPLAY_IDLE_INTERVAL (15000)
    /* Tiny game intervals */
#define TINY_REX_DISPLAY_MENU_UPDATE_INTERVAL (100)
#define TINY_REX_DISPLAY_PLAYING_UPDATE_INTERVAL (50)
#define TINY_REX_DISPLAY_SETTING_ANIMATION_UPDATE_INTERVAL (200)

    /* define signal */
    enum
    {
        TINY_REX_DISPLAY_RENDER_SCREEN = AK_SYS_DEFINE_SIG,
        TINY_REX_DISPLAY_INITIAL = AK_USER_DEFINE_SIG,
        TINY_REX_DISPLAY_BUTTON_MODE_PRESSED,
        TINY_REX_DISPLAY_BUTTON_MODE_RELEASE,
        TINY_REX_DISPLAY_BUTTON_MODE_LONG_PRESS,
        TINY_REX_DISPLAY_BUTTON_UP_PRESSED,
        TINY_REX_DISPLAY_BUTTON_DOWN_PRESSED,
        TINY_REX_DISPLAY_SHOW_IDLE,
        /* Tiny game signals */
        TINY_REX_DISPLAY_SHOW_MENU,
        TINY_REX_DISPLAY_MENU_UPDATE,
        TINY_REX_DISPLAY_PLAYING_UPDATE,
        TINY_REX_DISPLAY_PLAYING_GAME_OVER,
        EVENT_DISPLAY_SETTING_NAME,
    };

    /*****************************************************************************/
    /*  Tiny-Rex object signal define
     */
    /*****************************************************************************/
    /* Define signal */
    enum
    {
        TINY_REX_PLAY_EVENT = TINY_REX_DEFINE_SIG,
        TINY_REX_MOVE_EVENT,
        TINY_REX_JUMP_EVENT,
        TINY_REX_FALL_EVENT,
        TINY_REX_DUCK_EVENT,
        TINY_REX_DUCK_RELEASE_EVENT,
        TINY_REX_INC_SPEED_EVENT,
    };

    /*****************************************************************************/
    /*  Obstacle objects signal define
     */
    /*****************************************************************************/
    /* Define signal */
    enum
    {
        OBSTACLE_PLAY_EVENT = TINY_REX_DEFINE_SIG,
        OBSTACLE_MOVE_EVENT,
        OBSTACLE_CHECK_COLLISSION_EVENT,
        OBSTACLE_INC_SPEED_EVENT
    };

    /*****************************************************************************/
    /*  Horizon object signal define
     */
    /*****************************************************************************/
    /* Define signal */
    enum
    {
        HORIZON_OBJECT_PLAY_EVENT = TINY_REX_DEFINE_SIG,
        HORIZON_OBJECT_UPDATE_EVENT,
        HORIZON_INC_SPEED_EVENT,
    };

    /*****************************************************************************/
    /*  Score object signal define
     */
    /*****************************************************************************/
    /* Define signal */
    enum
    {
        SCORE_SETUP = TINY_REX_DEFINE_SIG,
        SCORE_UPDATE,
        SCORE_GAME_OVER,
    };

/*****************************************************************************/
/*  app function declare
 */
/*****************************************************************************/
#define APP_MAGIC_NUMBER 0xAABBCCDD
#define APP_VER    \
    {              \
        0, 0, 0, 3 \
    }

    typedef struct
    {
        uint32_t magic_number;
        uint8_t version[4];
    } app_info_t;

    extern const app_info_t app_info;

    extern void* app_get_boot_share_data();
    extern int main_app();

#ifdef __cplusplus
}
#endif

#endif //__APP_H__
