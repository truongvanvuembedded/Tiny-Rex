#ifndef __APP_DATA_H__
#define __APP_DATA_H__

#include <stdint.h>

#include "sys_boot.h"
#include "sys_dbg.h"
#include "app.h"
#include "buzzer.h"

typedef struct {
	uint8_t is_power_on_reset;
} boot_app_share_data_t;

#endif //__APP_DATA_H__
