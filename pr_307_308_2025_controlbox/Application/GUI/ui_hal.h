/**
 * @file ui_hal.h
 * @author Darwin (darwin.xiao@aputure.com)
 * @brief 
 * @version 0.1
 * @date 2022-09-09
 * 
 * @copyright Copyright (c) 2022 爱图仕
 * 
 * @par 修改日志:
 * <table>
 * <tr><th> Date          <th>Version <th>Author  <th>Description
 * <tr><td>2022-09-09     <td>1.0     <td>Darwin.Xiao   <td>初始创建
 * </table>
 */

#ifndef UI_HAL_H
#define UI_HAL_H

#include <stdint.h>
#include <stdbool.h>
#include "app_data_center.h"

void ui_ble_start_reset(void);
void ui_ble_power_changed(uint8_t status);
int  ui_ble_get_reset_status(void);
void ui_crmx_power_changed(uint8_t status);
void ui_crmx_start_unlink(void);
void ui_battery_start_activating(void);
int  ui_upgrade_get_status(void);
int  ui_upgrade_get_percent(void);
bool ui_upgrade_is_enable(void);
int8_t ui_get_box_ambient_temp(void);
int8_t ui_get_box_mcu_temp(void);
int8_t ui_get_lamp_cob_temp(void);
int16_t ui_get_orienta_level(void);
int16_t ui_get_orienta_pitch(void);
int16_t ui_get_orienta_roll(void);
uint8_t ui_upgrade_get_type(void);
void ui_usb_start_upgrade(void);
uint32_t ui_calc_checksum(uint8_t* addr, uint16_t size);
void ui_upgrade_clear_status(void);

uint8_t ui_get_lamp_hw_version(void);
uint8_t ui_get_lamp_soft_version(void);
uint8_t ui_get_ctrlbox_hw_version(void);
uint8_t ui_get_ctrlbox_soft_version(void);
uint8_t ui_get_motorized_yoke_soft_version(void);
uint8_t ui_get_motorized_frenel_soft_version(void);
uint16_t ui_get_led_time(void);
uint32_t ui_get_sys_time(void);
void ui_update_light_mode(void);
enum light_mode ui_pid_to_light_mode(uint8_t pid);
uint8_t ui_light_mode_to_pid(enum light_mode mode);
void ui_ble_motor_read_angle(uint8_t mode);
void ui_set_motor_reset(void);
uint64_t  ui_get_error_type(void);
void ui_error_restore_effects(uint8_t mode);
void ui_set_light_reset(void);
void ui_set_frsenel_reset(void);
void ui_get_version(void);
void ui_usb_start_batch_upgrade(void);
uint8_t ui_get_motorized_yoke_hw_version(void);

uint8_t ui_get_motorized_frenel_hw_version(void);

#endif // !UI_HAL_H
