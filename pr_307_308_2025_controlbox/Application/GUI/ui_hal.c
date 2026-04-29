/**
 * @file ui_hal.c
 * @author Darwin (darwin.xiao@aputure.com)
 * @brief 
 * @version 0.1
 * @date 2022-09-09
 * 
 * @copyright Copyright (c) 2022 爱图仕
 * 
 * @par 修改日志:
 * <table>
 * <tr><th> Date          <th>Version <th>Author        <th>Description
 * <tr><td>2022-09-09     <td>1.0     <td>Darwin.Xiao   <td>初始创建
 * </table>
 */

#include "ui_hal.h"
#include "os_event.h"
#include "app_ble.h"
#include "project_config.h"
#include "page_manager.h"
#include "dev_crmx.h"
#include "iot.h"
#include "version.h"

uint32_t ui_calc_checksum(uint8_t* addr, uint16_t size)
{
    uint32_t checksum = 0;
    for (uint16_t i = 0; i < size; i++)
    {
        checksum = checksum + *addr++;
    }    
    return checksum;
}
#include "app_usb.h"

void ui_ble_start_reset(void)
{
    os_ev_publish_event(MAIN_EV_BLE, OS_EVENT_BLE_RESET, 0);
}

void ui_ble_power_changed(uint8_t status)
{
    os_ev_publish_event(MAIN_EV_BLE, OS_EVENT_BLE_POWER, status);
}

void ui_ble_motor_read_angle(uint8_t mode)
{
    os_ev_publish_event(MAIN_EV_DATA, EV_DATA_READ_MOTOR_ANGLE, mode);
}

void ui_error_restore_effects(uint8_t mode)
{
	if(mode == 0)
		os_ev_publish_event(MAIN_EV_DATA, EV_DATA_LIGHT_CHANGE, 0);
	else
		os_ev_publish_event(MAIN_EV_LIGHT, EV_DATA_LIGHT_SWITCH, 1);
}

void ui_usb_start_batch_upgrade(void)
{
    os_ev_publish_event(MAIN_EV_USB, OS_EVENT_USB_BATCH_UPDATE_START, 0);
}

int ui_ble_get_reset_status(void)
{
    return get_ble_reset_status();
}

void ui_battery_start_activating(void)
{

}

void ui_crmx_power_changed(uint8_t status)
{
    os_ev_publish_event(MAIN_EV_DMX, EV_CRMX_POWER, status);
}

void ui_crmx_start_unlink(void)
{
    os_ev_publish_event(MAIN_EV_DMX, EV_CRMX_UNLINK, 0);
}

void ui_usb_start_upgrade(void)
{
    os_ev_publish_event(MAIN_EV_USB, OS_EVENT_USB_UPDATE_START, 0);
}

void ui_factory_reset(void)
{

}

void ui_machine_power_changed(uint8_t status)
{

}

void ui_batt_start_activating(void)
{

}

int ui_upgrade_get_status(void)
{
    struct sys_info_usb usb_status_offer;
    
    data_center_read_sys_info(SYS_INFO_USB, &usb_status_offer); 
    
    return usb_status_offer.update_firmware_state;
}

void ui_upgrade_clear_status(void)
{
    struct sys_info_usb usb_status_offer;
    
	data_center_read_sys_info(SYS_INFO_USB, &usb_status_offer); 
    usb_status_offer.percent = 0;
	usb_status_offer.update_firmware_type = 0;
	usb_status_offer.update_firmware_state = 0;
	data_center_write_sys_info(SYS_INFO_USB, &usb_status_offer); 
	
}

int ui_upgrade_get_percent(void)
{
    struct sys_info_usb usb_status_offer;
    
    data_center_read_sys_info(SYS_INFO_USB, &usb_status_offer); 
    
    return usb_status_offer.percent;
}

bool ui_upgrade_is_enable(void)
{
    struct sys_info_usb usb_status_offer;
    
    data_center_read_sys_info(SYS_INFO_USB, &usb_status_offer); 
    
    return usb_status_offer.state;
}

uint8_t ui_upgrade_get_type(void)
{
    struct sys_info_usb usb_status_offer;
    
    data_center_read_sys_info(SYS_INFO_USB, &usb_status_offer); 
    
    return usb_status_offer.update_firmware_type;
}

uint8_t ui_get_lamp_hw_version(void)
{
    struct sys_info_lamp lamp_info;
	uint8_t ver;
	
    data_center_read_sys_info(SYS_INFO_LAMP, &lamp_info);
	ver = 10 * (lamp_info.hard_ver[0] / 16) + (lamp_info.hard_ver[0] % 16);
	
    return ver;
}

uint8_t ui_get_lamp_soft_version(void)
{
    struct sys_info_lamp lamp_info;
    data_center_read_sys_info(SYS_INFO_LAMP, &lamp_info);
    return 10 * (lamp_info.soft_ver[0] / 16) + (lamp_info.soft_ver[0] % 16);
}

uint8_t ui_get_motorized_yoke_soft_version(void)
{
    struct sys_info_lamp lamp_info;
    data_center_read_sys_info(SYS_INFO_LAMP, &lamp_info);
//	return 10 * (0x01 / 16) + (0x01 % 16);
    return 10 * (lamp_info.motorized_yoke_soft_ver[0] / 16) + (lamp_info.motorized_yoke_soft_ver[0] % 16);
}

uint8_t ui_get_motorized_frenel_soft_version(void)
{
    struct sys_info_lamp lamp_info;
    data_center_read_sys_info(SYS_INFO_LAMP, &lamp_info);
	return 10 * (lamp_info.motorized_fresnel_soft_ver[0] / 16) + (lamp_info.motorized_fresnel_soft_ver[0] % 16);
}

uint8_t ui_get_motorized_yoke_hw_version(void)
{
    struct sys_info_lamp lamp_info;
    data_center_read_sys_info(SYS_INFO_LAMP, &lamp_info);
    return 10 * (lamp_info.motorized_yoke_hw_ver[0] / 16) + (lamp_info.motorized_yoke_hw_ver[0] % 16);
}

uint8_t ui_get_motorized_frenel_hw_version(void)
{
    struct sys_info_lamp lamp_info;
    data_center_read_sys_info(SYS_INFO_LAMP, &lamp_info);
	return 10 * (lamp_info.motorized_fresnel_hw_ver[0] / 16) + (lamp_info.motorized_fresnel_hw_ver[0] % 16);
}

uint8_t ui_get_ctrlbox_hw_version(void)
{
	uint8_t ver;
	ver = 10 * (HARDWARE_VERSION / 16) + (HARDWARE_VERSION % 16);
	return ver;
}

int8_t ui_get_lamp_cob_temp(void)
{
    struct sys_info_lamp lamp_info;
    data_center_read_sys_info(SYS_INFO_LAMP, &lamp_info);
    return lamp_info.cob_temp;
    
}

int8_t ui_get_box_mcu_temp(void)
{
    struct sys_info_ctrl_box ctrlbox_info;
    data_center_read_sys_info(SYS_INFO_CTRL, &ctrlbox_info);
    return ctrlbox_info.mcu_temp;
    
}

int8_t ui_get_box_ambient_temp(void)
{
    struct sys_info_ctrl_box ctrlbox_info;
    data_center_read_sys_info(SYS_INFO_CTRL, &ctrlbox_info);
    return ctrlbox_info.ambient_temp;
    
}


uint8_t ui_get_ctrlbox_soft_version(void)
{ 
	return 10 * (SOFTWARE_VERSION / 16) + (SOFTWARE_VERSION % 16);
}

int16_t ui_get_orienta_level(void)
{
    struct sys_info_motor ctrlbox_info;
    data_center_read_sys_info(SYS_INFO_MOTOR, &ctrlbox_info);    
    return ctrlbox_info.pan_angle;
    
}

int16_t ui_get_orienta_pitch(void)
{
    struct sys_info_motor ctrlbox_info;
	struct sys_info_lamp lemp_info;
	int16_t angle;
	
    data_center_read_sys_info(SYS_INFO_LAMP, &lemp_info);  
	if(lemp_info.exist == 1)
	{
		data_center_read_sys_info(SYS_INFO_MOTOR, &ctrlbox_info);  
		angle = ctrlbox_info.tilt_angle;
	}		
	else
		angle = 0;
	
    return angle;
}
int16_t ui_get_orienta_roll(void)
{
    struct sys_info_motor ctrlbox_info;
	struct sys_info_lamp lemp_info;
	int16_t angle;
	
    data_center_read_sys_info(SYS_INFO_LAMP, &lemp_info);  
	if(lemp_info.exist == 1)
	{
		data_center_read_sys_info(SYS_INFO_MOTOR, &ctrlbox_info);  
		angle = ctrlbox_info.roll_angle;
	}		
	else
		angle = 0;
	
    return angle;
}

uint32_t ui_get_sys_time(void)
{
//    IOT_Parameter_TypeDef g_iot_time;
	uint32_t time;
	
    time = data_center_read_run_time(); 
	time = time / 60;
	
    return time;
}


uint16_t ui_get_led_time(void)
{
    struct sys_info_lamp lemp_info;
    data_center_read_sys_info(SYS_INFO_LAMP, &lemp_info);  
	if(lemp_info.exist == 1)
		return lemp_info.led_run_time;
	else
		return 0;
}



uint64_t  ui_get_error_type(void)
{  
	sys_error_type error_data;
    data_center_read_sys_info(SYS_INFO_ERROR_TYPE, &error_data.error_data);
    return error_data.error_data;
}


void ui_set_motor_reset(void)
{
    os_ev_publish_event(MAIN_EV_DATA, EV_DATA_RESET_MOTOR, 0);
}

void ui_set_frsenel_reset(void)
{
    os_ev_publish_event(MAIN_EV_DATA, EV_DATA_RESET_FRESENEL, 0);
}

void ui_set_light_reset(void)
{
    os_ev_publish_event(MAIN_EV_DATA, EV_DATA_FACTORY_RESET, 0);
}

void ui_get_version(void)
{
    os_ev_publish_event(MAIN_EV_DATA, EV_DATA_READ_VER, 0);
}

#if 1
__weak int data_center_set_light_mode(enum light_mode mode)
{
    return 0;
}
#endif

void ui_update_light_mode(void)
{
    uint8_t pid = screen_get_act_pid();
    enum light_mode mode = ui_pid_to_light_mode(pid);

    if (mode != LIGHT_MODE_FACTORY_PWM)
    {
        data_center_set_light_mode(mode);
    }
}

enum light_mode ui_pid_to_light_mode(uint8_t pid)
{
    enum light_mode mode = LIGHT_MODE_FACTORY_PWM;
    switch (pid)
    {
        case PAGE_LIGHT_MODE_CCT:              mode = LIGHT_MODE_CCT;break;
        case PAGE_LIGHT_MODE_HSI:              mode = LIGHT_MODE_HSI;break;
        case PAGE_LIGHT_MODE_GEL:              mode = LIGHT_MODE_GEL;break;
        case PAGE_LIGHT_MODE_XY:               mode = LIGHT_MODE_XY; break;
        case PAGE_LIGHT_MODE_RGB:              mode = LIGHT_MODE_RGB;break;
        case PAGE_LIGHT_MODE_SOURCE:           mode = LIGHT_MODE_SOURCE;break;
        case PAGE_PULSING:                     mode = LIGHT_MODE_FX_PULSING_II; break;
        case PAGE_STROBE:                      mode = LIGHT_MODE_FX_STROBE_II;  break;
        case PAGE_EXPLOSIOIN:                  mode = LIGHT_MODE_FX_EXPLOSION_II; break;
        case PAGE_FAULTYBULB:                  mode = LIGHT_MODE_FX_FAULT_BULB_II; break;
        case PAGE_WELDING:                     mode = LIGHT_MODE_FX_WELDING_II; break;
        case PAGE_COPCAR:                      mode = LIGHT_MODE_FX_COP_CAR_II; break;
        case PAGE_CANDLE:                      mode = LIGHT_MODE_FX_CANDLE; break;
        case PAGE_CLUBLIGHTS:                  mode = LIGHT_MODE_FX_CLUBLIGHTS; break;
        case PAGE_COLORCHASE:                  mode = LIGHT_MODE_FX_COLOR_CHASE; break;
        case PAGE_TV:                          mode = LIGHT_MODE_FX_TV_II; break;
        case PAGE_PAPARAZZI:                   mode = LIGHT_MODE_FX_PAPARAZZI; break;
        case PAGE_LIGHTNING:                   mode = LIGHT_MODE_FX_LIGHTNING_II; break;
        case PAGE_FIREWORKS:                   mode = LIGHT_MODE_FX_FIREWORKS; break;
        case PAGE_FIRE:                        mode = LIGHT_MODE_FX_FIRE_II; break;
        case PAGE_PARTYLIGHT:                  mode = LIGHT_MODE_FX_PARTY_LIGHTS; break;
        default:
        break;
    }

    return mode;
}

uint8_t ui_light_mode_to_pid(enum light_mode mode)
{
    uint8_t page_id = PAGE_NONE;
    switch (mode)
    {
        case LIGHT_MODE_CCT:                page_id = PAGE_LIGHT_MODE_CCT;break;
        case LIGHT_MODE_HSI:                page_id = PAGE_LIGHT_MODE_HSI;break;
        case LIGHT_MODE_GEL:                page_id = PAGE_LIGHT_MODE_GEL;break;
        case LIGHT_MODE_XY:                 page_id = PAGE_LIGHT_MODE_XY; break;
        case LIGHT_MODE_RGB:                page_id = PAGE_LIGHT_MODE_RGB;break;
        case LIGHT_MODE_SOURCE:             page_id = PAGE_LIGHT_MODE_SOURCE;break;
        case LIGHT_MODE_FX_PULSING_II:      page_id = PAGE_PULSING; break;
        case LIGHT_MODE_FX_STROBE_II:       page_id = PAGE_STROBE;  break;
        case LIGHT_MODE_FX_EXPLOSION_II:    page_id = PAGE_EXPLOSIOIN; break;
        case LIGHT_MODE_FX_FAULT_BULB_II:   page_id = PAGE_FAULTYBULB; break;
        case LIGHT_MODE_FX_WELDING_II:      page_id = PAGE_WELDING; break;
        case LIGHT_MODE_FX_COP_CAR_II:      page_id = PAGE_COPCAR; break;
        case LIGHT_MODE_FX_CANDLE:          page_id = PAGE_CANDLE; break;
        case LIGHT_MODE_FX_CLUBLIGHTS:      page_id = PAGE_CLUBLIGHTS; break;
        case LIGHT_MODE_FX_COLOR_CHASE:     page_id = PAGE_COLORCHASE; break;
        case LIGHT_MODE_FX_TV_II:           page_id = PAGE_TV; break;
        case LIGHT_MODE_FX_PAPARAZZI:       page_id = PAGE_PAPARAZZI; break;
        case LIGHT_MODE_FX_LIGHTNING_II:    page_id = PAGE_LIGHTNING; break;
        case LIGHT_MODE_FX_FIREWORKS:       page_id = PAGE_FIREWORKS; break;
        case LIGHT_MODE_FX_FIRE_II:         page_id = PAGE_FIRE; break;
        case LIGHT_MODE_FX_PARTY_LIGHTS:    page_id = PAGE_PARTYLIGHT; break;
        default:
        break;
    }

    return page_id;
}



#if 0
bool ui_dmx_temination_disp(void)
{

}

bool ui_dmx_signal_is_exist(void)
{
    return true;
}

const char* ex_custom_fx_get_name(uint8_t type, uint8_t index)
{
    return NULL;
}

bool ex_custom_fx_is_empty(uint8_t type, uint8_t index)
{
    return false;
}

#endif
