/**
 ******************************************************************************
 * @author: GaoKong
 * @date:   13/08/2016
 ******************************************************************************
 **/
#include <malloc.h>
#include <stdlib.h>
#include <string.h>

#include <forward_list>
#include <functional>
#include <iterator>
#include <vector>
#include <deque>
#include <queue>
#include <array>
#include <map>

/* kernel include */
#include "ak.h"
#include "message.h"
#include "timer.h"
#include "fsm.h"

/* driver include */
#include "led.h"
#include "button.h"
#include "flash.h"

/* app include */
#include "app.h"
#include "app_dbg.h"
#include "app_bsp.h"
#include "app_flash.h"
#include "app_non_clear_ram.h"

#include "task_list.h"
#include "task_life.h"
#include "task_display.h"

/* sys include */
#include "sys_boot.h"
#include "sys_irq.h"
#include "sys_io.h"
#include "sys_ctrl.h"
#include "sys_dbg.h"

/* arduino include */
#include "SPI.h"
#include "WString.h"
#include "HardwareSerial.h"

/* common include */
#include "screen_manager.h"

#include "buzzer.h"

using namespace std;

#if defined(RELEASE)
const char* app_run_mode = "RELEASE";
#else
static const char* app_run_mode = "DEBUG";
#endif

const app_info_t app_info{
    APP_MAGIC_NUMBER,
    APP_VER,
};

static boot_app_share_data_t boot_app_share_data;

static void app_power_on_reset();
static void app_start_timer();
static void app_init_state_machine();
static void app_task_init();

/*****************************************************************************/
/* app main function.
 */
/*****************************************************************************/
int main_app()
{
    APP_PRINT("App run mode: %s, App version: %d.%d.%d.%d\n", app_run_mode, app_info.version[0], app_info.version[1], app_info.version[2], app_info.version[3]);

    sys_soft_reboot_counter++;

    /******************************************************************************
     * init active kernel
     *******************************************************************************/
    ENTRY_CRITICAL();
    task_init();
    task_create((task_t*)app_task_table);
    task_polling_create((task_polling_t*)app_task_polling_table);
    EXIT_CRITICAL();

    /******************************************************************************
     * init applications
     *******************************************************************************/
    /*********************
     * hardware configure *
     **********************/
    /* init watch dog timer */
    sys_ctrl_independent_watchdog_init(); /* 32s */
    sys_ctrl_soft_watchdog_init(200);     /* 20s */

    SPI.begin();

    /* adc peripheral configure */
    io_cfg_adc1(); /* configure adc for thermistor and CT sensor */

    /* adc configure for ct sensor */
    adc_bat_io_cfg();

    /* flash io init */
    flash_io_ctrl_init();

    /*********************
     * software configure *
     **********************/
    /* initial boot object */
    sys_boot_init();

    /* life led init */
    led_init(&led_life, led_life_init, led_life_on, led_life_off);

    /* button init */
    button_init(&btn_mode, 10, BUTTON_MODE_ID, io_button_mode_init, io_button_mode_read, btn_mode_callback);
    button_init(&btn_up, 10, BUTTON_UP_ID, io_button_up_init, io_button_up_read, btn_up_callback);
    button_init(&btn_down, 10, BUTTON_DOWN_ID, io_button_down_init, io_button_down_read, btn_down_callback);

    button_enable(&btn_mode);
    button_enable(&btn_up);
    button_enable(&btn_down);

    /* siren init */
    BUZZER_Init();
    BUZZER_PlaySound(BUZZER_SOUND_STARTUP);

    /* get boot share data */
    flash_read(APP_FLASH_INTTERNAL_SHARE_DATA_SECTOR_1, reinterpret_cast<uint8_t*>(&boot_app_share_data), sizeof(boot_app_share_data_t));
    if (boot_app_share_data.is_power_on_reset == SYS_POWER_ON_RESET)
    {
        app_power_on_reset();
    }

    /* increase start time */
    fatal_log_t app_fatal_log;
    flash_read(APP_FLASH_AK_DBG_FATAL_LOG_SECTOR, reinterpret_cast<uint8_t*>(&app_fatal_log), sizeof(fatal_log_t));
    app_fatal_log.restart_times++;
    flash_erase_sector(APP_FLASH_AK_DBG_FATAL_LOG_SECTOR);
    flash_write(APP_FLASH_AK_DBG_FATAL_LOG_SECTOR, reinterpret_cast<uint8_t*>(&app_fatal_log), sizeof(fatal_log_t));

    EXIT_CRITICAL();

    /* start timer for application */
    app_init_state_machine();
    app_start_timer();

    /******************************************************************************
     * app task initial
     *******************************************************************************/
    app_task_init();

    /******************************************************************************
     * run applications
     *******************************************************************************/
    sys_ctrl_shell_sw_to_nonblock();

    return task_run();
}

/*****************************************************************************/
/* app initial function.
 */
/*****************************************************************************/

/* start software timer for application
 * used for app tasks
 */
void app_start_timer()
{
    /* start timer to toggle life led */
    timer_set(TINY_REX_TASK_LIFE_ID, TINY_REX_LIFE_SYSTEM_CHECK, TINY_REX_LIFE_TASK_TIMER_LED_LIFE_INTERVAL, TIMER_PERIODIC);
    timer_set(TINY_REX_TASK_DISPLAY_ID, TINY_REX_DISPLAY_INITIAL, TINY_REX_DISPLAY_INITIAL_INTERVAL, TIMER_ONE_SHOT);
}

/* init state machine for tasks
 * used for app tasks
 */
void app_init_state_machine()
{
}

/* send first message to trigger start tasks
 * used for app tasks
 */
void app_task_init()
{
    SCREEN_CTOR(&scr_mng_app, scr_startup_handle, &scr_startup);
}

/*****************************************************************************/
/* app common function
 */
/*****************************************************************************/

/* hardware timer interrupt 10ms
 * used for led, button polling
 */
void sys_irq_timer_10ms()
{
    button_timer_polling(&btn_mode);
    button_timer_polling(&btn_up);
    button_timer_polling(&btn_down);
}

/* console UART RX interrupt: must read the data register every time it
 * fires to clear USART_IT_RXNE, even though no command console consumes
 * the byte anymore.
 */
void sys_irq_shell()
{
    (void)sys_ctrl_shell_get_char();
}

/* init non-clear RAM objects
 */
void app_power_on_reset()
{
    sys_soft_reboot_counter = 0;
}

void* app_get_boot_share_data()
{
    return static_cast<void*>(&boot_app_share_data);
}
