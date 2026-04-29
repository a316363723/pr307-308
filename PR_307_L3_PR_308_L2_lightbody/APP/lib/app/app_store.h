#ifndef APP_STORE_H
#define APP_STORE_H

#include "stdint.h"

void store_data_recover_init(void);
void store_data_task(void);
void set_power_calibration_state(uint8_t flag);
void set_single_power_calibration_state(uint8_t channel,uint8_t flag);
#endif
