#ifndef APP_POWER_H
#define APP_POWER_H
#include "cmsis_os.h"
#include "project_def.h"
#include <stdint.h>
#include <stdbool.h>

void power_init(void);
uint8_t power_timing_manage(void);
void app_power_thread_entrance(void *argument);
void app_power_iic_send_all_data(bool state);
void hal_self_adapt_all_pwm_set(uint16_t value);
uint8_t power_state_get(void);
void app_power_state_set(uint8_t state);
void api_stdio_mode(void);

uint16_t get_total_power(void);
extern const osThreadAttr_t g_power_thread_attr;
extern osThreadId_t app_power_thread_task_id;
void app_power_iic_send_all_data_no_wait(bool state);
void sys_till_angle_set(uint16_t till_angle);
void sys_scroll_angle_set(uint16_t scroll_angle);
void sys_pan_angle_set(uint16_t pan_angle);
void sys_ctrl_mode_set(uint8_t mode);
void sys_dmx_power_on_set(uint8_t mode);
//void app_powe_key_off_device(void);
void sys_box_lcd_lock(uint8_t value);
uint8_t app_power_get_voltage_value(void);
void power_led_flicker(void);
#endif /* APP_POWER_H */

