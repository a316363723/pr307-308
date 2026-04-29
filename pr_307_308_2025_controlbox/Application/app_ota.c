#include "app_ota.h"
#include <string.h>
#include "update_module.h"
#include "aes.h"
#include "crc32.h"
#include "project_config.h"
#include "hal_gpio.h"
#include "hal_flash.h"
#include "cmsis_os2.h"
#include "app_data_center.h"
#include "base_type.h"
#include "page_manager.h"
#include "version.h"
#include "dev_w25qxx.h"
#include "app_light.h"
#include "os_event.h"
#include "app_wdt.h"
#include "ui_data.h"
#include "app_usb.h"
#include "app_gui.h"

uint32_t LaunchStatus    __attribute__((at(0x2005FF00)));
uint32_t LagStatus       __attribute__((at(0x2005FF08)));

static AES_ctx                 ble_ctx;
//static struct sys_info_usb usb_status_offer;
sfirmwarebin* pFirmwareOta;

static const char* dev_name[4] = {USB_UPDATA_LAMP_DEVICE, USB_UPDATA_BOX_DEVICE, USB_UPDATA_LAMP_DEVICE, USB_UPDATA_LAMP_DEVICE};
static const char* firmware_name[4] = {"lamp_firmware.bin", "ctrl_firmware.bin", "ele_bracket_firmware.bin", "ele_frnsel_firmware.bin"};
struct sys_info_usb ota_status_offer;
static uint8_t s_upgrade_type = 0;

void app_bmp_update_data_erase(void)
{
	uint32_t Firmwar_infomation;
    uint32_t number;
	
	number = (BMP_UPDATE_START_ADDRESS/1024/64);
    dev_w25qxx_read_data(BMP_UPDATE_START_ADDRESS, (uint8_t *)&Firmwar_infomation, 4);  //ËØªÂèñÊòØÂê¶ÊúâÂõæÁâáÂçáÁ∫ßÊï∞ÊçÆ
    if(Firmwar_infomation != 0xffffffff)   //Ê∏ÖÈô§ÂçáÁ∫ßÊï∞ÊçÆ
    {
        for(uint32_t erase_number = number; erase_number < 32+number; erase_number++)
        {
            dev_w25qxx_erase_64sectors(erase_number);
            //Â¢ûÂä†ÁúãÈó®ÁãóÂñÇÁãó
			app_wdt_reset_set();
        }
    }
}

void app_ctr_update_data_erase(void)
{
	uint32_t Firmwar_infomation;
    uint32_t number;
	
	number = (UPDATE_START_ADDRESS/1024/64);
    dev_w25qxx_read_data(UPDATE_START_ADDRESS, (uint8_t *)&Firmwar_infomation, 4);  //ËØªÂèñÊòØÂê¶ÊúâÂçáÁ∫ßÊï∞ÊçÆ
    if(Firmwar_infomation != 0xffffffff)   //Ê∏ÖÈô§ÂçáÁ∫ßÊï∞ÊçÆ
    {
        for(uint32_t erase_number = number; erase_number < 32+number; erase_number++)
        {
            dev_w25qxx_erase_64sectors(erase_number);
            //Â¢ûÂä†ÁúãÈó®ÁãóÂñÇÁãó
			app_wdt_reset_set();
        }
    }
}

uint8_t app_ota_get_upgrade_type(void)
{
	return s_upgrade_type;
}	

int app_ble_gatt_ota(gatt_packet_ota_t* ota_body)
{
	enum enum_gatt_ack {
        OK = 0,
        ERR_OVER_SIZE,   
        ERR_ERASE,   
        ERR_FILE_HEAD,   
        ERR_SEQUENCE,   
        ERR_WRITE,   
        ERR_VERIFY,
        ERR_TIMEOUT, 
        ERR_STEP,
    }res = OK;
    static uint32_t pre_handle_seq = 0;
	static uint32_t crc_local = 0;
	static uint8_t light_num = 0;
	static uint32_t programe_addr = 0;
    static uint32_t seq_total = 0;
    static uint8_t led_on_flag = 0;
    switch(ota_body->step)
    {
        case 1:
            if(ota_body->step_body.device <= 3)  //3:µÁ∂Ø∑∆ƒ˘∂˚ s2:µÁ∂Ø÷ßº‹ 1:µ∆øÿ, 0:µ∆ÃÂ
            {
				led_on_flag = 1;
				os_ev_publish_event(MAIN_EV_LIGHT, EV_DATA_LIGHT_SWITCH, 0);
				osDelay(100);
				ota_status_offer.update_firmware_state = FIRMWARE_UPD_STATUS_START;
				screen_load_page_with_clear(PAGE_UPDATE, SUB_ID_UPDATING, false);
				ui_set_upgrade_timeout_clear();
                ota_status_offer.state = 0;
				data_center_write_sys_info(SYS_INFO_USB, &ota_status_offer);  
				light_num = ota_body->step_body.device;
                if(0 != update_open(dev_name[light_num], firmware_name[light_num]))
                {
                    res = ERR_STEP;
					ota_status_offer.update_firmware_state = FIRMWARE_UPD_STATUS_FAILED;
					data_center_write_sys_info(SYS_INFO_USB, &ota_status_offer);  
					os_ev_publish_event(MAIN_EV_LIGHT, EV_DATA_LIGHT_SWITCH, 1);
					osDelay(100);
					break;
                }
				if(ota_body->step_body.device == 0)
				{
					s_upgrade_type = 0;
					ota_status_offer.update_firmware_type = FIRMWARE_TYPE_LAMP;
				}
				else if(ota_body->step_body.device == 1)
				{
					s_upgrade_type = 1;
					ota_status_offer.update_firmware_type = FIRMWARE_TYPE_CTRL_BOX;
				}
				else if(ota_body->step_body.device == 2)
				{
					s_upgrade_type = 0;
					ota_status_offer.update_firmware_type = FIRMWARE_TYPE_ELE_BRACKET;
				}
				else if(ota_body->step_body.device == 3)
				{
					s_upgrade_type = 0;
					ota_status_offer.update_firmware_type = FIRMWARE_TYPE_ELE_FRNSEL;
				}
            }
            else
            {
				ota_status_offer.update_firmware_state = FIRMWARE_UPD_STATUS_FAILED;
                res = ERR_STEP;
				data_center_write_sys_info(SYS_INFO_USB, &ota_status_offer);
				os_ev_publish_event(MAIN_EV_LIGHT, EV_DATA_LIGHT_SWITCH, 1);
				osDelay(100);
				break;
            }
            AES_init_ctx_iv(&ble_ctx, aes_key, aes_iv);	
            pre_handle_seq = 0;
			crc_local = 0;
        break;
        case 2:
            ota_status_offer.update_firmware_state = FIRMWARE_UPD_STATUS_UPDING;
			ui_set_upgrade_timeout_clear();
            if(ota_body->step_body.length > UPDATE_AREA_SIZE)
            {
                res = ERR_OVER_SIZE;
                ota_status_offer.update_firmware_state = FIRMWARE_UPD_STATUS_FAILED;
				os_ev_publish_event(MAIN_EV_LIGHT, EV_DATA_LIGHT_SWITCH, 1);
            }
            else
            {
                if(0 != update_erase(dev_name[light_num], 0, ota_body->step_body.length))
                {
                    res = ERR_ERASE;
                    ota_status_offer.update_firmware_state = FIRMWARE_UPD_STATUS_FAILED;
					os_ev_publish_event(MAIN_EV_LIGHT, EV_DATA_LIGHT_SWITCH, 1);
                }
            }
            seq_total = ota_body->step_body.length / 128 + (ota_body->step_body.length % 128 ? 1 : 0);
            programe_addr = 0;
            ota_status_offer.percent = 0;
            data_center_write_sys_info(SYS_INFO_USB, &ota_status_offer);  
        break;
        case 3:
			ui_set_upgrade_timeout_clear();
            if(pre_handle_seq != ota_body->step_body.data_body.sequence)
            {
                if(pre_handle_seq + 1 != ota_body->step_body.data_body.sequence)
                {
                    res = ERR_SEQUENCE;
                    ota_status_offer.update_firmware_state = FIRMWARE_UPD_STATUS_FAILED;
                }
                else
                {
                    if(ota_body->step_body.data_body.sequence > 1)
                    {
						crc_local = CRC32_Calculate(crc_local, ota_body->step_body.data_body.data, ota_body->step_body.data_body.length);
                        if(light_num == 1)  // ÷ª”–µ∆øÿ≤≈Ω‚√‹
                        {
                            AES_CBC_decrypt_buffer(&ble_ctx, ota_body->step_body.data_body.data, ota_body->step_body.data_body.length);//Ω‚√‹
							if(0 != update_write(dev_name[light_num], programe_addr, ota_body->step_body.data_body.data, ota_body->step_body.data_body.length))
							{
								res = ERR_WRITE;
								ota_status_offer.update_firmware_state = FIRMWARE_UPD_STATUS_FAILED;
								os_ev_publish_event(MAIN_EV_LIGHT, EV_DATA_LIGHT_SWITCH, 1);
								data_center_write_sys_info(SYS_INFO_USB, &ota_status_offer); 
								break;
							}
                        }
						else if(light_num == 0 || light_num == 2 || light_num == 3)
						{
							if(0 != update_write(dev_name[light_num], ota_body->step_body.data_body.sequence, ota_body->step_body.data_body.data, ota_body->step_body.data_body.length))
							{
								res = ERR_WRITE;
								ota_status_offer.update_firmware_state = FIRMWARE_UPD_STATUS_FAILED;
								os_ev_publish_event(MAIN_EV_LIGHT, EV_DATA_LIGHT_SWITCH, 1);
								data_center_write_sys_info(SYS_INFO_USB, &ota_status_offer); 
								break;
							}
						}
                        ota_status_offer.percent = 100 * ota_body->step_body.data_body.sequence / seq_total;;
                    }
                    else
                    {
						if(light_num == 1)
						{
							pFirmwareOta = (sfirmwarebin*)&ota_body->step_body.data_body.data;
							if(FirmwareCorrect(pFirmwareOta) != 1)
							{
								res = ERR_WRITE;
								ota_status_offer.update_firmware_state = FIRMWARE_UPD_STATUS_FAILED;
								os_ev_publish_event(MAIN_EV_LIGHT, EV_DATA_LIGHT_SWITCH, 1);
								data_center_write_sys_info(SYS_INFO_USB, &ota_status_offer); 
								break;
							}
							pre_handle_seq = 0;
						}
						else if(light_num == 0 || light_num == 2 || light_num == 3)
						{
							pre_handle_seq = 1;
						}
						crc_local = CRC32_Calculate(crc_local, ota_body->step_body.data_body.data, ota_body->step_body.data_body.length);  // º∆À„Œ¥Ω‚√‹÷Æ«∞µƒcrc
						if(0 != update_write(dev_name[light_num], pre_handle_seq, ota_body->step_body.data_body.data, ota_body->step_body.data_body.length))
						{
							res = ERR_WRITE;
							ota_status_offer.update_firmware_state = FIRMWARE_UPD_STATUS_FAILED;
							os_ev_publish_event(MAIN_EV_LIGHT, EV_DATA_LIGHT_SWITCH, 1);
							data_center_write_sys_info(SYS_INFO_USB, &ota_status_offer); 
							break;
						}
                    }
                }
                data_center_write_sys_info(SYS_INFO_USB, &ota_status_offer); 
                pre_handle_seq = ota_body->step_body.data_body.sequence;
                programe_addr += ota_body->step_body.data_body.length;
                
            }
        break;
        case 4:
			ui_set_upgrade_timeout_clear();
            if(0 != update_close(dev_name[light_num],&ota_body->step_body.crc32) || crc_local != ota_body->step_body.crc32)
            {
                ota_status_offer.update_firmware_state = FIRMWARE_UPD_STATUS_FAILED;
				
                res = ERR_VERIFY;
				os_ev_publish_event(MAIN_EV_LIGHT, EV_DATA_LIGHT_SWITCH, 1);
            }
            else
            {
                uint8_t lang = 0;
                
                LaunchStatus = STATUS_NEED_OTA;
                data_center_read_config_data(SYS_CONFIG_LANG, &lang);
                if(lang == LANGU_EN)
                    LagStatus = STATUS_EN_LANGUAGE;
                else if(lang == LANGU_CHINESE)
                    LagStatus = STATUS_CN_LANGUAGE;
                ota_status_offer.update_firmware_state = FIRMWARE_UPD_STATUS_OK;
				if(light_num == 0 || light_num == 2 || light_num == 3)
					light_rs485_link_set(0);
            }
            data_center_write_sys_info(SYS_INFO_USB, &ota_status_offer); 
        break;
        default:break;
    }
	
    return res;
}

uint8_t FirmwareCorrect(sfirmwarebin* p_arg)
{
#if PROJECT_TYPE == 307
    if(0 != strcmp(p_arg->productname, "PR_307_L3"))
    {
        return false;
    }
#elif PROJECT_TYPE == 308
    if(0 != strcmp(p_arg->productname, "PR_308_L2"))
    {
        return 0;
    }
#endif
    if(p_arg->hardwareversion != HARDWARE_VERSION)//Base Data≤ªºÏ≤È”≤º˛∞Ê±æ
    {
        return 0;
    }
    
    if(p_arg->firmwaresize > UPDATE_AREA_SIZE)
    {
        return 0;
    }
    return 1;
}

//uint32_t get_hardware_version(void)
//{
////    uint32_t version = 0x10;

////    version += hal_gpio_read_pin(HAL_GPIOA, HAL_PIN_0) ? 0x01 : 0x00;
////    version += hal_gpio_read_pin(HAL_GPIOA, HAL_PIN_1) ? 0x02 : 0x00;
////    version += hal_gpio_read_pin(HAL_GPIOA, HAL_PIN_2) ? 0x04 : 0x00;
////	return version = 0x10;
//    return HARDWARE_VERSION;
//}

void app_ota_mcu_system_reset(void)
{
//	osThreadSuspend(app_usb_task_id);
	osThreadSuspend(app_gui_task_id);
	data_center_local_handle();
    //LaunchStatus = STATUS_NEED_OTA;
   system_reset();
}



