#include "app_usb.h"
#include "usb_bsp.h"
#include "ff.h"
#include "string.h"
#include "aes.h"
#include "crc32.h"
#include "hal_flash.h"
#include "cmsis_armcc.h"
#include "cmsis_os2.h"
#include "os_event.h"
#include "update_module.h"
#include "app_data_center.h"
#include "project_config.h"
#include "base_type.h"
#include "app_ota.h"
#include "ui_data.h"
#include "ui_common.h"
#include "user.h"
#include "app_light.h"
#include "dev_w25qxx.h"
#include "usb_host_driver.h"
#include "usb_host_user.h"
#include "lfm_core.h"
#include "app_gui.h"
#include "version.h"
#include "app_ble.h"

#pragma pack(1)
typedef struct
{
    char     product_name[16]; //产品名称
    uint32_t software_version; //产品软件版本
    uint32_t hardware_version; //产品硬件版本
    uint32_t firmware_size;    //产品固件大小
    uint32_t firmware_crc;     //产品固件校验值
    uint16_t firmware_type;    //产品固件类型
    uint32_t uuid;             //产品批量升级UUID
    uint16_t erase_time;       //产品批量升级升级区擦除等待缓存时间
    uint16_t interval_time;    //产品批量升级单包等待时间
}upgrade_head_info_type;
#pragma pack()

#define LFM_BATCH_UPGRADE_LAMP_LENGHT (128u)
#define LFM_BATCH_UPGRADE_BOX_LENGHT  (384u)

static void app_usb_update_comfirm(uint32_t event_type, uint32_t event_value);
static void app_usb_core_task(void);
static void app_update_mcu_system_reset(void);
static bool app_usb_update_file_find(uint8_t firmware_type, char* find_name);
static void app_usb_batchupgrade_file_seek(void);
static int  app_usb_upgrade_control_boxs(const char* filename);
static int  app_usb_upgrade_lamp(const char* filename);
static int  app_usb_upgrade_bmp(const char* filename);
static int  app_usb_upgrade_ele_frnsel(const char* filename);
static int  app_usb_upgrade_ele_bracket(const char* filename);
static int  app_usb_upgrade_file_send(const char* filename);
#if USE_BOOT_UPDATING == 1
static int app_usb_upgrade_boot(const char* filename);
#endif

FATFS fatfs;
FIL fp_upgrade;
DIR MyDir = {0};
FILINFO MyFileInfo = {0};
struct usb_status_str usb_status;
static char update_file_name[100] = {0};
static uint8_t read_buffer1[512] = {0};
AES_ctx                 usb_ctx;
struct sys_info_usb usb_status_offer;
char USBHPath[4];
static uint8_t  s_frist_usb_flag = 0;
static uint8_t  s_frist_fat_flag = 0;
static uint8_t  s_reset_boot_flag = 0;
static uint8_t  s_work_mode = 0;
static uint8_t  s_led_on_flag = 0;
static uint8_t  s_batch_upgrade_flag = 0;
extern usb_core_instance usb_app_instance;
extern USBH_HOST         usb_app_host;
static uint8_t  s_read_buffer[128] = {0};
osThreadId_t app_usb_task_id;

const osThreadAttr_t g_usb_cb_thread_attr = { 
    .name = "app_usb_cb", 
    .priority = osPriorityAboveNormal1, 
#if PROJECT_TYPE==307
	.stack_size = 1536 * 4
#elif PROJECT_TYPE==308
    .stack_size = 1792 * 4
#endif
};


uint32_t usb_task_static_size;

void app_usb_entry(void *argument)
{
    os_ev_subscribe_event(MAIN_EV_USB, OS_EVENT_USB_UPDATE_START | OS_EVENT_USB_BATCH_UPDATE_START, app_usb_update_comfirm);
    
	for(;;)
	{
        os_ev_pull_event(MAIN_EV_USB);
        app_usb_core_task();
		app_usb_batchupgrade_file_seek();
		usb_task_static_size = osThreadGetStackSpace(app_usb_task_id);
        osDelay(2);
	}
  
}

uint8_t app_usb_fatfile_state_get(void)
{
    return s_frist_fat_flag;
}

uint8_t app_usb_batch_upgrade_state_get(void)
{
    return s_batch_upgrade_flag;
}

void app_usb_update_handle(void)
{
    int res = -1;
    static uint8_t drv_flag[4] = {0};
	struct sys_info_accessories usb_access_info;
	
    if(s_frist_fat_flag == 1)  //挂载成功
    {
        switch(usb_status.update_step)
        {
            case UPDATE_IDLE:
                 memset(drv_flag, 0, sizeof(drv_flag));
            break;
#if USE_BOOT_UPDATING == 1
            case UPDATE_BOOT_FILE_SEARCH:
				{
                memset(update_file_name, 0, sizeof(update_file_name));
                res = app_usb_update_file_find(FIRMWARE_TYPE_BOOT, update_file_name);  
                if(res == true)
                {
                    usb_status_offer.update_firmware_type = FIRMWARE_TYPE_BOOT;
                    usb_status.update_step = UPDATE_BOOT_FILE_ACQUIRE;
                }
                else
                {
                    drv_flag[1] = 0;
                    usb_status.update_step =  UPDATE_ELE_FRNSEL_FILE_SEARCH;
                }
            }
			break;
			case UPDATE_BOOT_FILE_ACQUIRE:
				user_enter_updating();
				ui_set_upgrade_timeout_clear();
                usb_status_offer.update_firmware_state = FIRMWARE_UPD_STATUS_UPDING; 
                data_center_write_sys_info(SYS_INFO_USB, &usb_status_offer);
				drv_flag[1] = 1;
				s_led_on_flag = 1;
				os_ev_publish_event(MAIN_EV_LIGHT, EV_DATA_LIGHT_SWITCH, 0);
				ui_updata_start_flag_set(1);
				osDelay(500);
				ui_set_upgrade_timeout_clear();
                res = app_usb_upgrade_ele_frnsel(update_file_name);
                if(res == 0)
                {
                    usb_status_offer.update_firmware_state = FIRMWARE_UPD_STATUS_OK;
					data_center_read_sys_info(SYS_INFO_ACCESS, &usb_access_info);
					usb_access_info.fresnel_state = 0;
					data_center_write_sys_info(SYS_INFO_ACCESS, &usb_access_info);
                }
                else
                {
					if(res <= -2 && s_led_on_flag == 1)
					{
						os_ev_publish_event(MAIN_EV_LIGHT, EV_DATA_LIGHT_SWITCH, 1);
						s_led_on_flag = 0;
					}
                    usb_status_offer.update_firmware_state = FIRMWARE_UPD_STATUS_FAILED;
                }
                data_center_write_sys_info(SYS_INFO_USB, &usb_status_offer);   
                osDelay(2000);
                usb_status.update_step =  UPDATE_ELE_BRACKET_FILE_SEARCH;
			break;
#endif
            case UPDATE_ELE_FRNSEL_FILE_SEARCH:
            {
                memset(update_file_name, 0, sizeof(update_file_name));
                res = app_usb_update_file_find(FIRMWARE_TYPE_ELE_FRNSEL, update_file_name);  
                if(res == true && get_lamp_connet_state() == 1)
                {
                    usb_status_offer.update_firmware_type = FIRMWARE_TYPE_ELE_FRNSEL;
                    usb_status.update_step = UPDATE_ELE_FRNSEL_FILE_ACQUIRE;
                }
                else
                {
                    drv_flag[1] = 0;
                    usb_status.update_step =  UPDATE_ELE_BRACKET_FILE_SEARCH;
                }
            }
            break;
            case UPDATE_ELE_FRNSEL_FILE_ACQUIRE:
				user_enter_updating();
				ui_set_upgrade_timeout_clear();
                usb_status_offer.update_firmware_state = FIRMWARE_UPD_STATUS_UPDING; 
                data_center_write_sys_info(SYS_INFO_USB, &usb_status_offer);
				drv_flag[1] = 1;
				s_led_on_flag = 1;
				os_ev_publish_event(MAIN_EV_LIGHT, EV_DATA_LIGHT_SWITCH, 0);
				ui_updata_start_flag_set(1);
				osDelay(500);
				ui_set_upgrade_timeout_clear();
                res = app_usb_upgrade_ele_frnsel(update_file_name);
                if(res == 0)
                {
                    usb_status_offer.update_firmware_state = FIRMWARE_UPD_STATUS_OK;
					data_center_read_sys_info(SYS_INFO_ACCESS, &usb_access_info);
					usb_access_info.fresnel_state = 0;
					data_center_write_sys_info(SYS_INFO_ACCESS, &usb_access_info);
                }
                else
                {
					if(res <= -2 && s_led_on_flag == 1)
					{
						os_ev_publish_event(MAIN_EV_LIGHT, EV_DATA_LIGHT_SWITCH, 1);
						s_led_on_flag = 0;
					}
                    usb_status_offer.update_firmware_state = FIRMWARE_UPD_STATUS_FAILED;
                }
                data_center_write_sys_info(SYS_INFO_USB, &usb_status_offer);   
                osDelay(2000);
                usb_status.update_step =  UPDATE_ELE_BRACKET_FILE_SEARCH;
            break;
            case UPDATE_ELE_BRACKET_FILE_SEARCH:
            {
				ui_set_upgrade_timeout_clear();
                memset(update_file_name, 0, sizeof(update_file_name));
                res = app_usb_update_file_find(FIRMWARE_TYPE_ELE_BRACKET, update_file_name);  //先升级灯体
                if(res == true && get_lamp_connet_state() == 1)
                {
                    usb_status_offer.update_firmware_type = FIRMWARE_TYPE_ELE_BRACKET;
                    usb_status.update_step = UPDATE_ELE_BRACKET_FILE_ACQUIRE;
                }
                else
                {
                    drv_flag[2] = 0;
                    usb_status.update_step =  UPDATE_LAMP_FILE_SEARCH;
                }
            }
            break;
            case UPDATE_ELE_BRACKET_FILE_ACQUIRE:
				user_enter_updating();
				ui_set_upgrade_timeout_clear();
                usb_status_offer.update_firmware_state = FIRMWARE_UPD_STATUS_UPDING; 
                data_center_write_sys_info(SYS_INFO_USB, &usb_status_offer);
				drv_flag[2] = 1;
				ui_updata_start_flag_set(1);
				if(drv_flag[1] == 0)
				{
					s_led_on_flag = 1;
					os_ev_publish_event(MAIN_EV_LIGHT, EV_DATA_LIGHT_SWITCH, 0);
					osDelay(500);
				}
                res = app_usb_upgrade_ele_bracket(update_file_name);
                if(res == 0)
                {
                    usb_status_offer.update_firmware_state = FIRMWARE_UPD_STATUS_OK;
					data_center_read_sys_info(SYS_INFO_ACCESS, &usb_access_info);
					usb_access_info.yoke_state = 0;
					data_center_write_sys_info(SYS_INFO_ACCESS, &usb_access_info);
                }
                else
                {
					if(res <= -2 && s_led_on_flag == 1)
					{
						os_ev_publish_event(MAIN_EV_LIGHT, EV_DATA_LIGHT_SWITCH, 1);
						osDelay(500);
						s_led_on_flag = 0;
					}
                    usb_status_offer.update_firmware_state = FIRMWARE_UPD_STATUS_FAILED;
                }
                data_center_write_sys_info(SYS_INFO_USB, &usb_status_offer);   
                osDelay(2000);
                usb_status.update_step =  UPDATE_LAMP_FILE_SEARCH;
            break;
            case UPDATE_LAMP_FILE_SEARCH:
            {
				ui_set_upgrade_timeout_clear();
                memset(update_file_name, 0, sizeof(update_file_name));
                res = app_usb_update_file_find(FIRMWARE_TYPE_LAMP, update_file_name);  //先升级灯体
                if(res == true)
                {
                    usb_status_offer.update_firmware_type = FIRMWARE_TYPE_LAMP;
                    usb_status.update_step = UPDATE_LAMP_FILE_ACQUIRE;
                }
                else
                {
                    drv_flag[0] = 0;
                    usb_status.update_step =  UPDATE_BMP_FILE_SEARCH;
                }
            }
            break;
            case UPDATE_LAMP_FILE_ACQUIRE:
				user_enter_updating();
				ui_set_upgrade_timeout_clear();
                usb_status_offer.update_firmware_state = FIRMWARE_UPD_STATUS_UPDING; 
                data_center_write_sys_info(SYS_INFO_USB, &usb_status_offer);
				drv_flag[0] = 1;
				ui_updata_start_flag_set(1);
				if(drv_flag[1] == 0 && drv_flag[2] == 0)
				{
					os_ev_publish_event(MAIN_EV_LIGHT, EV_DATA_LIGHT_SWITCH, 0);
					osDelay(500);
					s_led_on_flag = 1;
				}
                res = app_usb_upgrade_lamp(update_file_name);
                if(res == 0)
                {
                    usb_status_offer.update_firmware_state = FIRMWARE_UPD_STATUS_OK;
					light_rs485_link_set(0);
                }
                else
                {
					if(res <= -2 && s_led_on_flag == 1)
					{
						os_ev_publish_event(MAIN_EV_LIGHT, EV_DATA_LIGHT_SWITCH, 1);
						osDelay(500);
						s_led_on_flag = 0;
					}
                    usb_status_offer.update_firmware_state = FIRMWARE_UPD_STATUS_FAILED;
                }
                data_center_write_sys_info(SYS_INFO_USB, &usb_status_offer);   
                osDelay(2000);
                usb_status.update_step =  UPDATE_BMP_FILE_SEARCH;
            break;
            case UPDATE_BMP_FILE_SEARCH:
            {
				ui_set_upgrade_timeout_clear();
                memset(update_file_name, 0, sizeof(update_file_name));
                res = app_usb_update_file_find(FIRMWARE_TYPE_BMP, update_file_name);  
                if(res == true)
                {
                    usb_status_offer.update_firmware_type = FIRMWARE_TYPE_BMP;
                    usb_status.update_step = UPDATE_BMP_FILE_ACQUIRE;
                }
                else
                {
                    drv_flag[3] = 0;
                    usb_status.update_step =  UPDATE_CTR_FILE_SEARCH;
                }
            }
            break;
            case UPDATE_BMP_FILE_ACQUIRE:
				user_enter_updating();
				ui_set_upgrade_timeout_clear();
                usb_status_offer.update_firmware_state = FIRMWARE_UPD_STATUS_UPDING; 
                data_center_write_sys_info(SYS_INFO_USB, &usb_status_offer);
				drv_flag[3] = 1;
				ui_updata_start_flag_set(1);
				if(drv_flag[1] == 0 && drv_flag[2] == 0 && drv_flag[0] == 0)
				{
					os_ev_publish_event(MAIN_EV_LIGHT, EV_DATA_LIGHT_SWITCH, 0);
					osDelay(500);
					s_led_on_flag = 1;
				}
                res = app_usb_upgrade_bmp(update_file_name);
                if(res == 0)
                {
                    usb_status_offer.update_firmware_state = FIRMWARE_UPD_STATUS_OK;
                }
                else
                {
					if(res <= -2 && s_led_on_flag == 1)
					{
						os_ev_publish_event(MAIN_EV_LIGHT, EV_DATA_LIGHT_SWITCH, 1);
						osDelay(500);
						s_led_on_flag = 0;
					}
					drv_flag[3] = 0;
                    usb_status_offer.update_firmware_state = FIRMWARE_UPD_STATUS_FAILED;
					app_bmp_update_data_erase();
                }
                osDelay(500);
                usb_status.update_step =  UPDATE_CTR_FILE_SEARCH;
            break;
            case UPDATE_CTR_FILE_SEARCH:
				ui_set_upgrade_timeout_clear();
                memset(update_file_name, 0, sizeof(update_file_name));
                res = app_usb_update_file_find(FIRMWARE_TYPE_CTRL_BOX , update_file_name);  //寻找灯控升级文件
                if(res == true)
                {
                    usb_status_offer.update_firmware_type = FIRMWARE_TYPE_CTRL_BOX;                                       
                    usb_status.update_step = UPDATE_CTR_FILE_ACQUIRE;
                }
                else
                {
					if(drv_flag[3] == 1)  //图片升级需要跳转
                    {
						s_reset_boot_flag = 1;
                    }
                    else if(drv_flag[0] == 1 || drv_flag[1] == 1 || drv_flag[2] == 1)
                    {
                        drv_flag[0] = 0;
                        drv_flag[1] = 0;
                        drv_flag[2] = 0;
                        usb_status.update_step =  UPDATE_IDLE;  
                        screen_load_prev_sub_page(0, 0);
						if(s_led_on_flag == 1)
						{
							s_led_on_flag = 0;
							os_ev_publish_event(MAIN_EV_LIGHT, EV_DATA_LIGHT_SWITCH, 1);
							osDelay(100);
						}
                    }
                    else
                    {
						if(s_led_on_flag == 1)
						{
							s_led_on_flag = 0;
							os_ev_publish_event(MAIN_EV_LIGHT, EV_DATA_LIGHT_SWITCH, 1);
							osDelay(500);
						}
                        usb_status.update_step =  UPDATE_ELE_FRNSEL_FILE_SEARCH;  
                        usb_status_offer.update_firmware_state = FIRMWARE_UPD_STATUS_FAILED_NO_FW;  //没有固件 
						data_center_write_sys_info(SYS_INFO_USB, &usb_status_offer);
                    }
                }
            break;
            case UPDATE_CTR_FILE_ACQUIRE:
				user_enter_updating();
				ui_set_upgrade_timeout_clear();
                usb_status_offer.update_firmware_state = FIRMWARE_UPD_STATUS_UPDING;   
                data_center_write_sys_info(SYS_INFO_USB, &usb_status_offer);
				ui_updata_start_flag_set(1);
				if(drv_flag[1] == 0 && drv_flag[2] == 0 && drv_flag[0] == 0 && drv_flag[3] == 0)
				{
					s_led_on_flag = 1;
					os_ev_publish_event(MAIN_EV_LIGHT, EV_DATA_LIGHT_SWITCH, 0);
					osDelay(500);
				}
                res = app_usb_upgrade_control_boxs(update_file_name);
                if(res == 0)
                {
                    uint8_t lang = 0;
                
                    data_center_read_config_data(SYS_CONFIG_LANG, &lang);
                    usb_status_offer.update_firmware_state = FIRMWARE_UPD_STATUS_OK;
                    if(lang == LANGU_EN)
                        LagStatus = STATUS_EN_LANGUAGE;
                    else if(lang == LANGU_CHINESE)
                        LagStatus = STATUS_CN_LANGUAGE;
					data_center_write_sys_info(SYS_INFO_USB, &usb_status_offer);
					s_reset_boot_flag = 1;
                    osDelay(1000);
                }
                else
                {
					if(s_led_on_flag == 1)
					{
						s_led_on_flag = 0;
						os_ev_publish_event(MAIN_EV_LIGHT, EV_DATA_LIGHT_SWITCH, 1);
						osDelay(500);
					}
					app_bmp_update_data_erase();
                    usb_status_offer.update_firmware_state = FIRMWARE_UPD_STATUS_FAILED;
                    usb_status.update_step =  UPDATE_IDLE; 
					data_center_write_sys_info(SYS_INFO_USB, &usb_status_offer);
                }
            break;   
            default:break;
        }
    }
}

void app_usb_batch_upgrade_handle(void)
{
	uint8_t file_serial_num = 0;
	struct sys_info_update_name update_name_str;
	int res = 0;
	
	ui_set_upgrade_timeout_clear();
	data_center_read_sys_info(SYS_INFO_WORK_UPDATE, &update_name_str);	
	memset(update_file_name, 0, sizeof(update_file_name));
	file_serial_num = ui_get_batch_update_serial_num();
	memcpy(update_file_name, update_name_str.usb_name[file_serial_num], strlen(update_name_str.usb_name[file_serial_num]));
	
	os_ev_publish_event(MAIN_EV_LIGHT, EV_DATA_LIGHT_SWITCH, 0);
	osDelay(100);
	ui_set_upgrade_timeout_clear();
	usb_status_offer.update_firmware_state = FIRMWARE_UPD_STATUS_UPDING;   
	data_center_write_sys_info(SYS_INFO_USB, &usb_status_offer);
	res = app_usb_upgrade_file_send(update_file_name);
	os_ev_publish_event(MAIN_EV_LIGHT, EV_DATA_LIGHT_SWITCH, 1);
	if(res == 0)
	{
		usb_status_offer.update_firmware_state = FIRMWARE_UPD_STATUS_OK;
		data_center_write_sys_info(SYS_INFO_USB, &usb_status_offer);
	}
	else
	{
		usb_status_offer.update_firmware_state = FIRMWARE_UPD_STATUS_FAILED;
		data_center_write_sys_info(SYS_INFO_USB, &usb_status_offer);
	}
	s_batch_upgrade_flag = 0;
}

static uint8_t IsBmpFirmwareCorrect(sfirmwarebin* p_arg)
{
	#if PROJECT_TYPE==307
	if(0 != strncmp(p_arg->productname, "PR_307_L3_BMP", 14))
	{
		return 0;
	}
	#endif
	#if PROJECT_TYPE==308
	if(0 != strncmp(p_arg->productname, "PR_308_L2_BMP", 14))
	{
		return 0;
	}
	#endif
	if(p_arg->hardwareversion != HARDWARE_VERSION)    
	{
		return 0;
	}
	return 1;
}

static uint8_t Isupdatefile(upgrade_head_info_type* p_arg)
{
	if(p_arg->erase_time == 0)    
	{
		return 0;
	}
	return 1;
}

static void app_update_mcu_system_reset(void)
{
    LaunchStatus = STATUS_NEED_OTA;  //TODO
    system_reset();
}

static void app_usb_core_task(void)
{
    static uint8_t usb_state_change = 0;
	
    if(usb_app_host.host_state == HOST_IDLE && usb_state_change == 0)
    {
        usb_state_change = 1;
		s_batch_upgrade_flag = 0;
        usb_status_offer.state = U_DISK_PULL_OUT;
        data_center_write_sys_info(SYS_INFO_USB, &usb_status_offer);
		if(s_led_on_flag == 1)  //升级失败时要把灯体打开
		{
			s_led_on_flag = 0;
			os_ev_publish_event(MAIN_EV_LIGHT, EV_DATA_LIGHT_SWITCH, 1);
			osDelay(500);
		}
    }
    else if(usb_app_host.host_state == HOST_CLASS_PROCESS && usb_state_change == 1)
    {
        usb_state_change = 0;
        usb_status_offer.state = U_DISK_INSERT;
        data_center_write_sys_info(SYS_INFO_USB, &usb_status_offer);
    }
    usb_host_mainprocess(&usb_app_instance, &usb_app_host);
	if(s_reset_boot_flag == 1)
	{
		s_reset_boot_flag = 0;
		osThreadSuspend(app_ble_task_id);
		osThreadSuspend(app_gui_task_id);
		data_center_local_handle();
		app_update_mcu_system_reset();
	}
		
	switch(usb_app_host.host_state)
	{
		case HOST_IDLE:
			if(s_frist_usb_flag == 1)
			{
				s_frist_usb_flag = 0;
			}
			if(s_frist_fat_flag == 1)
			{
				usb_status.update_step = UPDATE_IDLE;
				f_mount(NULL, (TCHAR const*)USBHPath, 0);
				s_frist_fat_flag = 0;
				usb_status_offer.state = U_DISK_PULL_OUT;
				data_center_write_sys_info(SYS_INFO_USB, &usb_status_offer);
			}
		break;
		case HOST_CLASS_PROCESS:
			if(s_frist_usb_flag == 0)
			{
				if(s_frist_fat_flag == 0)
				{
					if(FR_OK == f_mount(&fatfs, (TCHAR const*)USBHPath, 0))
					{
						s_frist_fat_flag = 1;
						s_frist_usb_flag = 1;
						usb_status_offer.state = U_DISK_INSERT;
						data_center_write_sys_info(SYS_INFO_USB, &usb_status_offer);
					}
				}
			}
		break;
		default:break;
	}
}

static void app_usb_batchupgrade_file_seek(void)
{
	uint8_t res = 0;
	static uint8_t ui_chage_flag = 0;
	
	if(ui_get_work_update_number_flag() == 1)
    {
		switch(usb_app_host.host_state)
		{
			case HOST_IDLE:
				screen_load_sub_page(SUB_ID_SYNC_NOT_FOUND_UPD_DEV, 0);
				ui_set_work_update_number_flag(0);
			break;
			case HOST_CLASS_PROCESS:
				if(s_frist_fat_flag == 1)
				{
					ui_chage_flag = 0;
					res = app_usb_batch_upgrade_file_find();
					if(res == 1)
					{
						ui_set_work_update_number_flag(0);
						screen_load_sub_page(SUB_ID_SYNC_NO_FIRMWARE, 0);
					}
					else if(res == 0 || res == 2)
					{
						ui_set_work_update_number_flag(0);
						screen_load_sub_page(SUB_ID_SYNC_UPDATA_SELECT, 0);
					}
				}
			break;
			default:break;
		}
	}
}

/*******************************************************************************************
函数名称: uint8_t app_usb_batch_upgrade_file_find(void)
函数描述: 查找支持批量升级的固件是否存在
函数参数：NONE
返回值：state：查找状态   0---成功  1---失败
*******************************************************************************************/
uint8_t app_usb_batch_upgrade_file_find(void)
{
	uint8_t state = 0;
	static FRESULT res;
	struct sys_info_update_name  update_name;
    uint32_t bytesread = 0;
	char hver[] = {"_H1.0"};
	char sver[] = {"_S1.0"};
	
	memset(&update_name, 0, sizeof(update_name));
	memset(s_read_buffer, 0, sizeof(s_read_buffer));
	res = f_opendir(&MyDir, (TCHAR const*)USBHPath);		/* Open the target directory */
	if(res != FR_OK)
	{
		f_closedir(&MyDir);
		state = 1;
		return state;
	}
	do
    {
		memset(MyFileInfo.fname, 0, sizeof(MyFileInfo.fname));
        res = f_readdir(&MyDir, &MyFileInfo);   /* Search for next item */
        if(res == FR_OK && MyFileInfo.fname[0])
        {
            for(uint8_t i = 1; i < 100; i++)
			{
				if((0 == memcmp(&MyFileInfo.fname[i], "_HardwareV", 10))    && 
				   (0 == memcmp(&MyFileInfo.fname[i+13], "_SoftwareV", 10)) &&
				   (0 == memcmp(&MyFileInfo.fname[i+13+13], ".bin", 4)))
				{
					if(FR_OK == f_open(&fp_upgrade, MyFileInfo.fname, FA_READ)) 
					{
						f_read(&fp_upgrade, s_read_buffer, 128, &bytesread);
						
						if(Isupdatefile((upgrade_head_info_type*)s_read_buffer) != 0)
						{
							hver[2] = MyFileInfo.fname[i+10];
							hver[4] = MyFileInfo.fname[i+12];
							memcpy(update_name.usb_name[update_name.file_number], MyFileInfo.fname, strlen(MyFileInfo.fname));
							memset(update_file_name, 0, sizeof(update_file_name));
							memcpy(update_file_name, MyFileInfo.fname, i);
							strcat(update_file_name, hver);//23
							sver[2] = MyFileInfo.fname[i+10+13];
							sver[4] = MyFileInfo.fname[i+12+13];
							strcat(update_file_name, sver);//23
							memcpy(update_name.name[update_name.file_number], update_file_name, strlen(update_file_name));
							update_name.file_number++;
						}
					}
					f_close(&fp_upgrade);
					break;
				}
			}
			if(update_name.file_number > 9)
			{
				f_closedir(&MyDir);
				data_center_write_sys_info(SYS_INFO_WORK_UPDATE, &update_name);	
				state = 0;
				return state;
			}
        }
		else
		{
			if(update_name.file_number >= 1)
				state = 0;
			else
				state = 1;
			data_center_write_sys_info(SYS_INFO_WORK_UPDATE, &update_name);	
			f_closedir(&MyDir);
			return state;
		}
    }
	while(res == FR_OK && MyFileInfo.fname[0]);
	data_center_write_sys_info(SYS_INFO_WORK_UPDATE, &update_name);	
	state = 0;
	f_closedir(&MyDir);
	return state;
	
}

/*******************************************************************************************
函数名称: FlagStatus Upgrade_file_Get(FirmwareType_Typedef FirmwareType, char* FileName)
函数描述: 查找固件升级文件是否存在
函数参数：
          FirmwareType：查找文件的类型
          FileName：指向存放文件名的指针
返回值：
        find：RESET----没有升级文件    SET---有升级文件
*******************************************************************************************/
/**
 * @brief 寻找固件名字
 * 
 */ 
static bool app_usb_update_file_find(uint8_t firmware_type, char* find_name)
{
    static FRESULT res;
    char   f_find_rule[50] = {0};
    char ver[] = {"1.0"};
    uint8_t find = 0;
    uint8_t HardwareVersion = 0x10;
	struct sys_info_lamp   lamp_info = {0};
	data_center_read_sys_info(SYS_INFO_LAMP, &lamp_info);
    memset(f_find_rule, 0, sizeof(f_find_rule));
	
    if(firmware_type == FIRMWARE_TYPE_CTRL_BOX)
    {
		strcpy(f_find_rule, CONFIG_USB_UPDATE_CTR_FILE_NAME);
		ver[0] = '0' + HardwareVersion / 0x10;
		ver[2] = '0' + HardwareVersion % 0x10;
    }
    else if(firmware_type == FIRMWARE_TYPE_LAMP)
    {
		strcpy(f_find_rule, CONFIG_USB_UPDATE_LAMP_FILE_NAME);
		ver[0] =  '0' + lamp_info.hard_ver[0] / 0x10;
		ver[2] =  '0' + lamp_info.hard_ver[0] % 0x10;
    }
    else if(firmware_type == FIRMWARE_TYPE_ELE_FRNSEL)
    {
		strcpy(f_find_rule, CONFIG_USB_UPDATE_ELE_FRNSEL_FILE_NAME);
		ver[0] = '?';
		ver[2] = '?';
    }
    else if(firmware_type == FIRMWARE_TYPE_ELE_BRACKET)
    {
		strcpy(f_find_rule, CONFIG_USB_UPDATE_ELE_BRACKET_FILE_NAME);
		ver[0] = '?';
		ver[2] = '?';
    }
    else if(firmware_type == FIRMWARE_TYPE_BMP)
    {
		strcpy(f_find_rule, CONFIG_USB_UPDATE_BMP_FILE_NAME);
		ver[0] = '?';
		ver[2] = '?';
    }
#if USE_BOOT_UPDATING == 1
	else if(firmware_type == FIRMWARE_TYPE_BOOT)
	{
		strcpy(f_find_rule, CONFIG_USB_UPDATE_BOOT_FILE_NAME);
		ver[0] = '?';
		ver[2] = '?';
	}
#endif
    strcat(f_find_rule, "_HardwareV");//11+10=21
    strcat(f_find_rule, ver);//23
    strcat(f_find_rule, "_SoftwareV?.?.bin");//23+17=40 
    
    res = f_findfirst(&MyDir, &MyFileInfo, (TCHAR const*)USBHPath, (TCHAR const*)f_find_rule);  
    if(res == FR_OK && MyFileInfo.fname[0])
    {
        find = 1;
        strcpy(find_name, MyFileInfo.fname);
    }

    while(res == FR_OK && MyFileInfo.fname[0])
    {
        res = f_findnext(&MyDir, &MyFileInfo);   /* Search for next item   */
        if(res == FR_OK && MyFileInfo.fname[0])
        {
           // HardwareVersion = strlen
            if(MyFileInfo.fname[strlen(f_find_rule) - 7] > find_name[strlen(f_find_rule) - 7]//33
                || MyFileInfo.fname[strlen(f_find_rule) - 5] > find_name[strlen(f_find_rule) - 5])
            {
                strcpy(find_name, MyFileInfo.fname);
            }
        }
    }
    f_closedir(&MyDir);
    return find; 
}

#if USE_BOOT_UPDATING == 1
/**
 * @brief  灯控boot区固件升级
 * @return 0--升级成功  !0--失败
 */ 
static int app_usb_upgrade_boot(const char* filename)
{
    int res = 0;
	int write_res = 0;
    uint8_t read_buffer[128] = {0};
    uint32_t bytesread = 0;
    uint32_t addr = 0;
	static uint32_t usb_crc_local = 0;
	sfirmwarebin* info_arg = NULL;
	
    usb_status_offer.percent = 0;;
    data_center_write_sys_info(SYS_INFO_USB, &usb_status_offer); 
    write_res = update_open(USB_UPDATA_BOOT_DEVICE, "firmware1");
    if(0 != write_res)
        return -1;
    ui_set_upgrade_timeout_clear();
    if(FR_OK != f_open(&fp_upgrade, filename, FA_READ)) 
    {
        res = -2;
        goto close_dev_and_file;
    }
    ui_set_upgrade_timeout_clear();
   
    if(FR_OK == f_read(&fp_upgrade, read_buffer, 128, &bytesread) && bytesread == 128)
    {
		ui_set_upgrade_timeout_clear();
		info_arg = (sfirmwarebin*)&read_buffer;
        if(!FirmwareCorrect((sfirmwarebin*)&read_buffer))
        {
            goto close_dev_and_file;
        }
        else
        {
			usb_crc_local = CRC32_Calculate(usb_crc_local, read_buffer, bytesread);
        }
    }
	while(f_tell(&fp_upgrade) < f_size(&fp_upgrade)) 
    {
		ui_set_upgrade_timeout_clear();
        if(FR_OK != f_read(&fp_upgrade, read_buffer1, 512, &bytesread))
        {
            res = -3;
            goto close_dev_and_file;
        }
        //拔出U盘
        if(host_driver_ifdevconnected(&usb_app_instance) == 0)
        {
            res = -4;
            usb_status_offer.state = 0;
            goto close_dev_and_file;
        }
		usb_crc_local = CRC32_Calculate(usb_crc_local, read_buffer1, bytesread);
        osDelay(1);
    }
	if(usb_crc_local != info_arg->checkcrc32)
	{
		res = -4;
		goto close_dev_and_file;
	}
	f_close(&fp_upgrade);
	osDelay(100);
	if(FR_OK != f_open(&fp_upgrade, filename, FA_READ)) 
    {
        res = -2;
        goto close_dev_and_file;
    }
	addr = 0;
	write_res = update_erase(USB_UPDATA_BOOT_DEVICE, 0, BOOT_AREA_SIZE);
    ui_set_upgrade_timeout_clear();
	ui_updata_start_flag_set(0);
    while(f_tell(&fp_upgrade) < f_size(&fp_upgrade)) 
    {
		ui_set_upgrade_timeout_clear();
        if(FR_OK != f_read(&fp_upgrade, read_buffer1, 512, &bytesread))
        {
            res = -3;
            goto close_dev_and_file;
        }
        //拔出U盘
        if(host_driver_ifdevconnected(&usb_app_instance) == 0)
        {
            res = -4;
            usb_status_offer.state = 0;
            goto close_dev_and_file;
        }
        write_res = update_write(USB_UPDATA_BOOT_DEVICE, addr, read_buffer1, bytesread);
        if(0 != write_res)
            goto close_dev_and_file;
        usb_status_offer.percent = 100 * f_tell(&fp_upgrade) / f_size(&fp_upgrade);
        addr += bytesread;
        data_center_write_sys_info(SYS_INFO_USB, &usb_status_offer); 
        osDelay(1);
    }   
    f_close(&fp_upgrade);
	ui_set_upgrade_timeout_clear();
    return update_close(USB_UPDATA_BOOT_DEVICE,NULL); 
close_dev_and_file:
    f_close(&fp_upgrade);
	ui_set_upgrade_timeout_clear();
    return res;
}
#endif
/**
 * @brief  灯控固件升级
 * @return 0--升级成功  !0--失败
 */ 
static int app_usb_upgrade_control_boxs(const char* filename)
{
    int res = 0;
	int write_res = 0;
    uint8_t read_buffer[128] = {0};
    uint32_t bytesread = 0;
    uint32_t addr = 0;
	
    usb_status_offer.percent = 0;;
    data_center_write_sys_info(SYS_INFO_USB, &usb_status_offer); 
    write_res = update_open(USB_UPDATA_BOX_DEVICE, "firmware");
    if(0 != write_res)
        return -1;
    ui_set_upgrade_timeout_clear();
    if(FR_OK != f_open(&fp_upgrade, filename, FA_READ)) 
    {
        res = -2;
        goto close_dev_and_file;
    }
    ui_set_upgrade_timeout_clear();
    /* 头信息不用解密 */
    if(FR_OK == f_read(&fp_upgrade, read_buffer, 128, &bytesread) && bytesread == 128)
    {
		ui_set_upgrade_timeout_clear();
        if(!FirmwareCorrect((sfirmwarebin*)&read_buffer))
        {
            goto close_dev_and_file;
        }
        else
        {
			write_res = update_erase(USB_UPDATA_BOX_DEVICE, 0, f_size(&fp_upgrade));
			if(0 != write_res)
				goto close_dev_and_file;
			addr = 0;
            AES_init_ctx_iv(&usb_ctx, aes_key, aes_iv);	
            write_res = update_write(USB_UPDATA_BOX_DEVICE, addr, read_buffer, bytesread);
            if(0 != write_res)
                goto close_dev_and_file;
            addr += bytesread;
        }
    }
    ui_set_upgrade_timeout_clear();
	ui_updata_start_flag_set(0);
    while(f_tell(&fp_upgrade) < f_size(&fp_upgrade)) 
    {
		ui_set_upgrade_timeout_clear();
        if(FR_OK != f_read(&fp_upgrade, read_buffer1, 512, &bytesread))
        {
            res = -3;
            goto close_dev_and_file;
        }
        //拔出U盘
        if(host_driver_ifdevconnected(&usb_app_instance) == 0)
        {
            res = -4;
            usb_status_offer.state = 0;
            goto close_dev_and_file;
        }
        AES_CBC_decrypt_buffer(&usb_ctx, read_buffer1, bytesread);//解密
        write_res = update_write(USB_UPDATA_BOX_DEVICE, addr, read_buffer1, bytesread);
        if(0 != write_res)
            goto close_dev_and_file;
        usb_status_offer.percent = 100 * f_tell(&fp_upgrade) / f_size(&fp_upgrade);
        addr += bytesread;
        data_center_write_sys_info(SYS_INFO_USB, &usb_status_offer); 
        osDelay(1);
    }   
    f_close(&fp_upgrade);
	ui_set_upgrade_timeout_clear();
    return update_close(USB_UPDATA_BOX_DEVICE,NULL); 
close_dev_and_file:
    f_close(&fp_upgrade);
	ui_set_upgrade_timeout_clear();
    return res;
}

/**
 * @brief  灯体固件升级
 * @return 0--升级成功  -1--失败
 */ 
static int app_usb_upgrade_lamp(const char* filename)
{
    int res = -1;
	int write_res = 0;
    uint32_t bytesread = 0;
    uint32_t block_cnt = 0;
    uint8_t  block_buf[128];
    uint32_t crc32 = 0;
	
    usb_status_offer.percent = 0;
    data_center_write_sys_info(SYS_INFO_USB, &usb_status_offer); 
    write_res = update_open(USB_UPDATA_LAMP_DEVICE, "lamp_firmware.bin");//485发送start指令
    if(0 != write_res)
		goto close_lamp_dev_and_file;
	ui_set_upgrade_timeout_clear();
    if(FR_OK != f_open(&fp_upgrade, filename, FA_READ))
    {
        res = -2;
        goto close_lamp_dev_and_file;
    }
    ui_set_upgrade_timeout_clear();
    write_res = update_erase(USB_UPDATA_LAMP_DEVICE, 0, f_size(&fp_upgrade));  //485发送升级文件长度信息
    if(0 != write_res)
        goto close_lamp_dev_and_file;
	ui_updata_start_flag_set(0);
    while(f_tell(&fp_upgrade) < f_size(&fp_upgrade))
    {
		ui_set_upgrade_timeout_clear();
        if(FR_OK != f_read(&fp_upgrade, block_buf, 128, &bytesread))
        {
            res = -3;
            goto close_lamp_dev_and_file;
        }
        //拔出U盘
        if(host_driver_ifdevconnected(&usb_app_instance) == 0)
        {
            res = -4;
            usb_status_offer.state = 0;
            goto close_lamp_dev_and_file;
        }
		
        block_cnt = (f_tell(&fp_upgrade) / 128) + ((f_tell(&fp_upgrade) % 128) ? 1 : 0);
        write_res = update_write(USB_UPDATA_LAMP_DEVICE, block_cnt, (const uint8_t*)block_buf, bytesread);
		crc32 = CRC32_Calculate(crc32, (uint8_t*)block_buf, bytesread);
        if(0 != write_res)
            goto close_lamp_dev_and_file;
        usb_status_offer.percent = 100 * f_tell(&fp_upgrade) / f_size(&fp_upgrade);
        data_center_write_sys_info(SYS_INFO_USB, &usb_status_offer); 
        osDelay(1);
    }  
    f_close(&fp_upgrade);
	ui_set_upgrade_timeout_clear();
    return update_close(USB_UPDATA_LAMP_DEVICE,(uint32_t*)crc32);  
close_lamp_dev_and_file:
    f_close(&fp_upgrade);
	ui_set_upgrade_timeout_clear();
    return res;
}

/**
 * @brief  图片固件升级
 * @return 0--升级成功  !0--失败
 */ 
static int app_usb_upgrade_bmp(const char* filename)
{  
    uint16_t erase_number = BMP_UPDATE_START_ADDRESS/1024/64;
    int res = -1;
    uint32_t bytesread = 0;
	int write_res = 0;
    
	usb_status_offer.percent = 0;;
    data_center_write_sys_info(SYS_INFO_USB, &usb_status_offer); 
    uint32_t ProgAddr = BMP_UPDATE_START_ADDRESS;
    
    if(FR_OK == f_open(&fp_upgrade, filename, FA_READ))
    {   
		ui_set_upgrade_timeout_clear();
        for(uint8_t i = erase_number; i < 32 + erase_number; i++)
        {
            dev_w25qxx_erase_64sectors(i);
			ui_set_upgrade_timeout_clear();
			osDelay(1);
        }
        /* 头信息不用解密 */
        if(FR_OK == f_read(&fp_upgrade, read_buffer1, 128, &bytesread) && bytesread == 128)
        {
			ui_set_upgrade_timeout_clear();
            if(!IsBmpFirmwareCorrect((sfirmwarebin*)&read_buffer1))
            {
                goto out;
            }
            write_res = dev_w25qxx_write_data(ProgAddr, (uint8_t *)read_buffer1, bytesread);
            ProgAddr += bytesread;
        }
		else
			res = -4;
        /* 升级图片数据 */
		ui_updata_start_flag_set(0);
        while(f_tell(&fp_upgrade) < f_size(&fp_upgrade))
        {
			ui_set_upgrade_timeout_clear();
            if(FR_OK != f_read(&fp_upgrade, read_buffer1, 512, &bytesread))
            {
				res = -4;
                goto out;
                
            }
            //拔出U盘
            if(host_driver_ifdevconnected(&usb_app_instance) == 0)
			{
                goto out;
            }
            write_res = dev_w25qxx_write_data(ProgAddr, (uint8_t *)read_buffer1, bytesread);
            if(0 == write_res)
            {
                goto out;
            }
            ProgAddr += bytesread;
            usb_status_offer.percent = 100 * f_tell(&fp_upgrade) / f_size(&fp_upgrade);
            data_center_write_sys_info(SYS_INFO_USB, &usb_status_offer); 
            osDelay(1);
        }
        res = 0;
    }
	else
		res = -4;
	f_close(&fp_upgrade);
	ui_set_upgrade_timeout_clear();
    return 0; 
out:
	ui_set_upgrade_timeout_clear();
    f_close(&fp_upgrade);
    return res;
}

/**
 * @brief  电动菲涅尔固件升级
 * @return 0--升级成功  -1--失败
 */ 
static int app_usb_upgrade_ele_frnsel(const char* filename)
{
    int res = -1;
	int write_res = 0;
    uint32_t bytesread = 0;
    uint32_t block_cnt = 0;
    uint8_t  block_buf[128];
	uint8_t lfm_star_flag = 0;
	
    usb_status_offer.percent = 0;
    data_center_write_sys_info(SYS_INFO_USB, &usb_status_offer); 
    write_res = update_open(USB_UPDATA_LAMP_DEVICE, "ele_frnsel_firmware.bin");//485发送start指令
    if(0 != write_res)
        return -1;
    ui_set_upgrade_timeout_clear();
    if(FR_OK != f_open(&fp_upgrade, filename, FA_READ))
    {
        res = -2;
        goto close_lamp_dev_and_fransel_file;
    }
    ui_set_upgrade_timeout_clear();
    write_res = update_erase(USB_UPDATA_LAMP_DEVICE, 0, f_size(&fp_upgrade));  //485发送升级文件长度信息
    if(0 != write_res)
        goto close_lamp_dev_and_fransel_file;
	ui_updata_start_flag_set(0);
	ui_set_upgrade_timeout_clear();
    while(f_tell(&fp_upgrade) < f_size(&fp_upgrade))
    {
		ui_set_upgrade_timeout_clear();
        if(FR_OK != f_read(&fp_upgrade, block_buf, 128, &bytesread))
        {
            res = -3;
            goto close_lamp_dev_and_fransel_file;
        }
		
        //拔出U盘
        if(host_driver_ifdevconnected(&usb_app_instance) == 0)
        {
            res = -4;
            usb_status_offer.state = 0;
            goto close_lamp_dev_and_fransel_file;
        }
        
        block_cnt = (f_tell(&fp_upgrade) / 128) + ((f_tell(&fp_upgrade) % 128) ? 1 : 0);
        write_res = update_write(USB_UPDATA_LAMP_DEVICE, block_cnt, (const uint8_t*)block_buf, bytesread);
        if(0 != write_res)
            goto close_lamp_dev_and_fransel_file;
        usb_status_offer.percent = 100 * f_tell(&fp_upgrade) / f_size(&fp_upgrade);
        data_center_write_sys_info(SYS_INFO_USB, &usb_status_offer); 
        osDelay(1);
    }  
    f_close(&fp_upgrade);
    return update_close(USB_UPDATA_LAMP_DEVICE,NULL);  
close_lamp_dev_and_fransel_file:
    f_close(&fp_upgrade);
    return res;
}

/**
 * @brief  电动支架固件升级
 * @return 0--升级成功  -1--失败
 */ 
static int app_usb_upgrade_ele_bracket(const char* filename)
{
    int res = -1;
	int write_res = 0;
    uint32_t bytesread = 0;
    uint32_t block_cnt = 0;
    uint8_t  block_buf[128];
	
    usb_status_offer.percent = 0;
    data_center_write_sys_info(SYS_INFO_USB, &usb_status_offer); 
    write_res = update_open(USB_UPDATA_LAMP_DEVICE, "ele_bracket_firmware.bin");//485发送start指令
    if(0 != write_res)
        return -1;
    ui_set_upgrade_timeout_clear();
    if(FR_OK != f_open(&fp_upgrade, filename, FA_READ))
    {
        res = -2;
        goto close_lamp_dev_and_bracket_file;
    }
    ui_set_upgrade_timeout_clear();
    write_res = update_erase(USB_UPDATA_LAMP_DEVICE, 0, f_size(&fp_upgrade));  //485发送升级文件长度信息
    if(0 != write_res)
        goto close_lamp_dev_and_bracket_file;
	ui_updata_start_flag_set(0);
	ui_set_upgrade_timeout_clear();
    while(f_tell(&fp_upgrade) < f_size(&fp_upgrade))
    {
		ui_set_upgrade_timeout_clear();
        if(FR_OK != f_read(&fp_upgrade, block_buf, 128, &bytesread))
        {
            res = -3;
            goto close_lamp_dev_and_bracket_file;
        }
        //拔出U盘
        if(host_driver_ifdevconnected(&usb_app_instance) == 0)
        {
            res = -4;
            usb_status_offer.state = 0;
            goto close_lamp_dev_and_bracket_file;
        }
        
        block_cnt = (f_tell(&fp_upgrade) / 128) + ((f_tell(&fp_upgrade) % 128) ? 1 : 0);
        write_res = update_write(USB_UPDATA_LAMP_DEVICE, block_cnt, (const uint8_t*)block_buf, bytesread);
        if(0 != write_res)
            goto close_lamp_dev_and_bracket_file;
        usb_status_offer.percent = 100 * f_tell(&fp_upgrade) / f_size(&fp_upgrade);
        data_center_write_sys_info(SYS_INFO_USB, &usb_status_offer); 
        osDelay(1);
    }  
    f_close(&fp_upgrade);
    return update_close(USB_UPDATA_LAMP_DEVICE,NULL);  
close_lamp_dev_and_bracket_file:
    f_close(&fp_upgrade);
    return res;
}

/**
 * @brief  批量升级发送文件数据
 * @param  filename：文件名
 * @return 0--升级成功  -1--失败
 */ 
static int app_usb_upgrade_file_send(const char* filename)
{
    int res = -1;
    uint32_t bytesread = 0;
	uint32_t send_addr = 0;
	struct lfm_upgrade_header lfm_header;
	uint8_t work_mode = 0;
	uint16_t read_size = 0;
	upgrade_head_info_type *head_arg = NULL;
	static uint16_t erase_time = 0;
	static uint16_t interval_time = 0;
	static uint32_t uuid = 0;
	static uint8_t type = 0;
	uint8_t timeout = 0;
	
    usb_status_offer.percent = 0;
    data_center_write_sys_info(SYS_INFO_USB, &usb_status_offer); 
	data_center_read_config_data(SYS_CONFIG_WORK_MODE, &work_mode);
	
	memset(read_buffer1, 0, sizeof(read_buffer1));
	ui_set_upgrade_timeout_clear();
	if(work_mode != 1)
	{
		return res; 
	}
	
    if(FR_OK == f_open(&fp_upgrade, filename, FA_READ))
    {   
		ui_set_upgrade_timeout_clear();
        /* 头信息不用解密 */
        if(FR_OK == f_read(&fp_upgrade, read_buffer1, 128, &bytesread) && bytesread == 128)
        {
			ui_set_upgrade_timeout_clear();
			head_arg = (upgrade_head_info_type *)read_buffer1;
			erase_time = head_arg->erase_time;
			interval_time = head_arg->interval_time;
			uuid = head_arg->uuid;
			type = (uuid&0x000000ff);
			read_size = LFM_BATCH_UPGRADE_LAMP_LENGHT;
			if(type == 0x01 || type == 0x03)
				read_size = LFM_BATCH_UPGRADE_BOX_LENGHT;
			switch(type)
			{
				case 0x01:type = FIRMWARE_TYPE_CTRL_BOX;break;
				case 0x02:type = FIRMWARE_TYPE_LAMP;break;
				case 0x03:type = FIRMWARE_TYPE_BMP;break;
				//do
				break;
			}
			//特殊处理--例如电动附件
			if(uuid == LFM_UPGRADE_ELE_BRACKET_UUID)
			{
				read_size = LFM_BATCH_UPGRADE_LAMP_LENGHT;
				type = FIRMWARE_TYPE_ELE_BRACKET;
			}
			else if(uuid == LFM_UPGRADE_ELE_FRNSEL_UUID)
			{
				read_size = LFM_BATCH_UPGRADE_LAMP_LENGHT;
				type = FIRMWARE_TYPE_ELE_FRNSEL;
			}
			
			lfm_header.type = type;
			lfm_header.error = 0;
			if(type == FIRMWARE_TYPE_LAMP || 
			   type == FIRMWARE_TYPE_ELE_BRACKET || 
			   type == FIRMWARE_TYPE_ELE_FRNSEL)   
			{
				send_addr = (f_tell(&fp_upgrade) / LFM_BATCH_UPGRADE_LAMP_LENGHT) + ((f_tell(&fp_upgrade) % LFM_BATCH_UPGRADE_LAMP_LENGHT) ? 1 : 0);;
			}
			else
			{
				send_addr += 0;
			}
			lfm_header.prog_addr = send_addr;
			send_addr += bytesread;
			lfm_header.step = LFM_UPGRADE_STEP_START;
			lfm_header.data_len  = bytesread;
			
			ui_set_upgrade_timeout_clear();
			lfm_transmit_upgrade_packet(&lfm_header, read_buffer1, uuid);
			lfm_upgrade_transfer_wait_finished(erase_time);
			ui_set_upgrade_timeout_clear();
        }
		else
		{
			goto out;
		}
        /* 发送数据 */
        while(f_tell(&fp_upgrade) < f_size(&fp_upgrade))
        {
			data_center_read_config_data(SYS_CONFIG_WORK_MODE, &work_mode);
			ui_set_upgrade_timeout_clear();
            if(FR_OK != f_read(&fp_upgrade, read_buffer1, read_size, &bytesread))
            {
                goto out;
            }
            //拔出U盘
            if(host_driver_ifdevconnected(&usb_app_instance) == 0)
			{
                goto out;
            }
			if(work_mode != 1)
			{
				return res;
			}
			lfm_header.type = type;
			lfm_header.error = 0;
			if(type == FIRMWARE_TYPE_LAMP || 
			   type == FIRMWARE_TYPE_ELE_BRACKET || 
			   type == FIRMWARE_TYPE_ELE_FRNSEL)   
			{
				send_addr = (f_tell(&fp_upgrade) / LFM_BATCH_UPGRADE_LAMP_LENGHT) + ((f_tell(&fp_upgrade) % LFM_BATCH_UPGRADE_LAMP_LENGHT) ? 1 : 0);
				lfm_header.prog_addr = send_addr;
			}
			else
			{
				lfm_header.prog_addr = send_addr;
			}
			lfm_header.step = LFM_UPGRADE_STEP_DATA;
			lfm_header.data_len  =  bytesread;		
			lfm_transmit_upgrade_packet(&lfm_header, read_buffer1, uuid);	
			send_addr += bytesread;
            usb_status_offer.percent = 100 * f_tell(&fp_upgrade) / f_size(&fp_upgrade);
            data_center_write_sys_info(SYS_INFO_USB, &usb_status_offer); 
			lfm_upgrade_transfer_wait_finished(interval_time);
        }
    }
	else
	{
		goto out;
	}
	ui_set_upgrade_timeout_clear();
	res = 0;
	lfm_header.type = type;
	lfm_header.error = 0;
	lfm_header.prog_addr = 0;
	lfm_header.step = LFM_UPGRADE_STEP_END;
	lfm_header.data_len  =  0;		
	lfm_transmit_upgrade_packet(&lfm_header, read_buffer1, uuid);
	lfm_upgrade_transfer_wait_finished(interval_time);
	f_close(&fp_upgrade);
	
    return res; 
out:
	ui_set_upgrade_timeout_clear();
	lfm_header.type = type;
	lfm_header.error = 1;
	lfm_header.prog_addr = 100;
	lfm_header.step = LFM_UPGRADE_STEP_END;
	lfm_header.data_len  =  0;		
	lfm_transmit_upgrade_packet(&lfm_header, read_buffer1, uuid);	
	
    f_close(&fp_upgrade);
    return res;
}


//开始升级事件触发回调函数
static void app_usb_update_comfirm(uint32_t event_type, uint32_t event_value)
{
    if(event_type == MAIN_EV_USB && event_value == OS_EVENT_USB_UPDATE_START)
    {
#if USE_BOOT_UPDATING == 1
        usb_status.update_step = UPDATE_BOOT_FILE_SEARCH;
#else
		usb_status.update_step = UPDATE_ELE_FRNSEL_FILE_SEARCH;  
#endif
		usb_status_offer.state = 1;
		usb_status_offer.percent = 0;
		usb_status_offer.update_firmware_state = 0;
		usb_status_offer.update_firmware_type = 0;
        data_center_write_sys_info(SYS_INFO_USB, &usb_status_offer);
		data_center_read_config_data(SYS_CONFIG_WORK_MODE, &s_work_mode);
    }
	if(event_type == MAIN_EV_USB && event_value == OS_EVENT_USB_BATCH_UPDATE_START)
    {
		s_batch_upgrade_flag = 1;
		usb_status_offer.state = 1;
		usb_status_offer.percent = 0;
		usb_status_offer.update_firmware_state = 0;
		usb_status_offer.update_firmware_type = 0;
        data_center_write_sys_info(SYS_INFO_USB, &usb_status_offer);
		data_center_read_config_data(SYS_CONFIG_WORK_MODE, &s_work_mode);
    }
}

