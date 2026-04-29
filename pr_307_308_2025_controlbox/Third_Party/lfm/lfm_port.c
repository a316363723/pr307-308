#include "lfm_core.h"
#include "rs485_protocol.h"
#include "cmsis_os2.h"
#include "dev_w25qxx.h"
#include "CRC32.h"
#include "common.h"
#include "page_manager.h"
#include <stdbool.h>
#include "UI_Data.h"
#include "user.h"
#include "base_type.h"
#include "aes.h"
#include "project_config.h"
#include "app_ota.h"
#include <string.h>
#include "page.h"
#include "dev_rs485.h"
#include "ui_hal.h"
#include "update_module.h"
#include "app_data_center.h"
#include "app_light.h"
#include "dev_crmx.h"
#include "app_ble_port.h"
#include "app_power.h"
#include "os_event.h"
#include "update_module.h"

#define UPGRADE_START_ADDRESS       EX_FLASH_FIRMWARE_START_ADDR

//extern void rs485_usart_tx_data(uint8_t* data, uint16_t len, uint32_t block_time);
extern void dmx_send_data(uint8_t *buff, uint16_t len);
extern UI_Data_t g_tUIAllData;
extern osMessageQueueId_t dmx_rx_queue;
extern void exit_screen_protector(void);

bool lfm_send_upgrade_data_to_lamp_start(uint8_t file_type);
bool lfm_send_upgrade_data_to_lamp_size(uint32_t size);
bool lfm_send_upgrade_data_to_lamp_data(uint8_t* buff, uint16_t len, uint32_t block_cnt);
bool lfm_send_upgrade_data_to_lamp_crc32(uint32_t crc32);

uint8_t tx_frame_buffer[512];
uint8_t dmx_rx_buffer[512];

static void     init_message_header(rs485_proto_header_t* p_header, bool ack_en, uint16_t serial_num, uint16_t msg_size);
static int      rs485_dev_write(uint8_t* data, uint16_t len);
//static int      event_notify_cb(enum lfm_event, struct lfm_comm_setting*);
static int      event_notify_cb(enum lfm_event e, uint8_t* data);
static int      refresh_before_send(enum lfm_event, struct lfm_comm_setting*);
static int      check_firmware(uint8_t type, struct lfm_firmware_info*);
static int      upgrade_start_cb(struct lfm_core* core, struct lfm_upgrade_header* p_header, uint32_t* prog_size);
static int      upgrade_data_cb(struct lfm_core* core,  struct lfm_upgrade_header* p_header, uint32_t* prog_size);
static int      upgrade_end_cb(struct lfm_core* core,  struct lfm_upgrade_header* p_header);
static void     time_callback(const struct lfm_core* core, uint32_t ms);
static enum light_mode dev_get_effects_type_by_lfm(enum lfm_data_type type);
//static void     erase_flash(uint32_t address, uint32_t size);
//static void     write_flash(uint32_t address, uint8_t* data, uint16_t size);
//static bool     broadcast_valid_page(uint8_t page_id);
static enum lfm_data_type lfm_get_type_by_pid(uint8_t pid);
static void     init_cmd_header(rs485_cmd_header_t* p_cmd_header, uint8_t rw, rs485_cmd_enum cmd, uint16_t arg_size);
static rs485_cmd_arg_t* get_cmd_arg(uint8_t* buff);
static uint8_t check_sum_calc(const uint8_t* buff, uint16_t size);
static uint8_t  get_flag_cb(void);
static void     set_lfm_mode(uint8_t value);
//static int      crmx_dev_write(uint8_t* data, uint16_t len);
static AES_ctx  lfm_ctx;

static struct sys_info_usb lfm_status_offer;
static uint8_t s_lfm_send_data_flag;
static uint8_t s_lfm_upgrade_state = 0;
//主从升级UUID
static uint32_t s_batch_upgrade_uuid[FIRMWARE_TYPE_MAX] = {
	LFM_UPGRADE_BOX_UUID, LFM_UPGRADE_LAMP_UUID, LFM_UPGRADE_BMP_UUID, LFM_UPGRADE_ELE_BRACKET_UUID, LFM_UPGRADE_ELE_FRNSEL_UUID
};

void leader_follwer_mode_init(void)
{
    lfm_core_deinit();

    static struct lfm_device rs485_device;    
    rs485_device.type = LFM_DEVICE_RS485;		/* 485设备类型 */
    rs485_device.write = rs485_dev_write;		/* 485设备写函数 */
    lfm_register_device(&rs485_device);			/* 注册设备 */

//    static struct lfm_device crmx_device;    
//    crmx_device.type = LFM_DEVICE_WIRELESS;		/* CRMX设备类型 */
//    crmx_device.write = crmx_dev_write;			/* CRMX设备写函数 */
//    lfm_register_device(&crmx_device);			/* 注册CRMX设备 */

    static struct lfm_upgrade_ops u_ops;		
    u_ops.check_firmware = check_firmware;		/* 检验固件 */
    u_ops.start = upgrade_start_cb;				/* 升级开始回调函数 */
    u_ops.data = upgrade_data_cb;				/* 升级中数据处理回调函数 */
    u_ops.end = upgrade_end_cb;					/* 升级结束回调函数 */    
    u_ops.time_callback = time_callback;		/* 定时器回调函数 */
    lfm_upgrade_set_operations(&u_ops); 		/* 设置升级操作方法集合 */

    static struct lfm_core_ops c_ops;
    c_ops.event_notify_cb = event_notify_cb;   	/* 事件处理回调函数 */
    c_ops.get_lfm_mode = get_flag_cb;			/* 获取标志回调函数 */
    c_ops.set_lfm_mode = set_lfm_mode;			/* 设置标志回掉函数 */
    c_ops.refresh_before_send = refresh_before_send;	/* 发送前更新回调函数 */
    lfm_core_set_operations(&c_ops);    				/* 设置核心操作方法集 */
	lfm_core_init();									/* 核心初始化 */
}

void lfm_send_light_data_set_flag(uint8_t flag)
{
	s_lfm_send_data_flag = flag;
}

uint8_t lfm_send_light_data_get_flag(void)
{
	return s_lfm_send_data_flag;
}

static uint8_t get_flag_cb(void)
{
    return g_tUIAllData.sys_menu_model.work_mode;
}

static void set_lfm_mode(uint8_t value)
{
    g_tUIAllData.sys_menu_model.work_mode = value;
}


void* lfm_get_settiing_storage(uint16_t* size)
{
	uint8_t mode = date_center_get_light_mode();
	void* ptr = NULL;
	#if PROJECT_TYPE==307
	switch(mode)
	{	
		case LIGHT_MODE_FX_LIGHTNING_II:
			*size =sizeof(g_tUIAllData.lightning2_model.mode_arg);
			ptr = &g_tUIAllData.lightning2_model.mode_arg;      
		break;
		case LIGHT_MODE_FX_STROBE_II:
			*size =sizeof(g_tUIAllData.strobe2_model.mode_arg);
			ptr = &g_tUIAllData.strobe2_model.mode_arg; 
		break;
		case LIGHT_MODE_FX_EXPLOSION_II:
			*size =sizeof(g_tUIAllData.explosion2_model.mode_arg);
			ptr = &g_tUIAllData.explosion2_model.mode_arg; 
		break;
		case LIGHT_MODE_FX_FAULT_BULB_II:
			*size =sizeof(g_tUIAllData.faultybulb2_model.mode_arg);
			ptr = &g_tUIAllData.faultybulb2_model.mode_arg; 
		break;	
		case LIGHT_MODE_FX_PULSING_II:
			*size =sizeof(g_tUIAllData.pulsing2_model.mode_arg);
			ptr = &g_tUIAllData.pulsing2_model.mode_arg; 
		break;
		case LIGHT_MODE_FX_WELDING_II:
			*size =sizeof(g_tUIAllData.welding2_model.mode_arg);
			ptr = &g_tUIAllData.welding2_model.mode_arg; 
		break;
		case LIGHT_MODE_FX_FIRE_II:
			*size =sizeof(g_tUIAllData.fire2_model.mode_arg);
			ptr = &g_tUIAllData.fire2_model.mode_arg; 
			break;
		case LIGHT_MODE_FX_TV_II:
			*size =sizeof(g_tUIAllData.tv2_model.mode_arg);
			ptr = &g_tUIAllData.tv2_model.mode_arg; 
			break;
	}
  #endif
  return ptr;
}


static void time_callback(const struct lfm_core* core, uint32_t ms)
{
    switch (core->upgrade.state)
    {
        case LFM_UPGRADE_STATE_IDLE:    
            break;
        case LFM_UPGRADE_STATE_START:
        {
            g_ptUIStateData->upd_status.fw_type = core->upgrade.type;  
        } 
        break;
        case LFM_UPGRADE_STATE_ING:
        {
            g_ptUIStateData->upd_status.percent = core->upgrade.prog_percent;
			lfm_status_offer.percent = core->upgrade.prog_percent;
			data_center_write_sys_info(SYS_INFO_USB, &lfm_status_offer); 
        }
        break;
        case LFM_UPGRADE_STATE_TIMROUT:
        case LFM_UPGRADE_STATE_ERR:
        {
			lfm_status_offer.update_firmware_state = FIRMWARE_UPD_STATUS_FAILED;
			data_center_write_sys_info(SYS_INFO_USB, &lfm_status_offer);  
        }
        break;
        case LFM_UPGRADE_STATE_END:
        {
			lfm_status_offer.update_firmware_state = FIRMWARE_UPD_STATUS_OK;
			data_center_write_sys_info(SYS_INFO_USB, &lfm_status_offer);  
        }
        break;
    }
}



/**
 * @brief 数据同步动作发送回调函数
 * 
 * @param type 数据类型
 * @param size 该数据类型的数据大小
 * @return void* 返回数据指针
 */
void* lfm_get_data_by_type(enum lfm_data_type type, uint16_t* size)
{
    void* ptr = NULL;
	uint8_t mode = date_center_get_light_mode();
    switch (type)
    {
        case LFM_DATA_TYPE_HSI:
            *size = sizeof(g_tUIAllData.hsi_model);
            ptr = &g_tUIAllData.hsi_model;
            break;
        case LFM_DATA_TYPE_RGB:
            *size = sizeof(g_tUIAllData.rgb_model);
            ptr = &g_tUIAllData.rgb_model;
            break;        
        case LFM_DATA_TYPE_GEL:
            *size = sizeof(g_tUIAllData.gel_model);
            ptr = &g_tUIAllData.gel_model;
            break;        
        case LFM_DATA_TYPE_XY:
            *size = sizeof(g_tUIAllData.xy_model);
            ptr = &g_tUIAllData.xy_model;
            break;        
        case LFM_DATA_TYPE_SOURCE:
            *size = sizeof(g_tUIAllData.source_model);
            ptr = &g_tUIAllData.source_model;
            break;        
        case LFM_DATA_TYPE_CCT: 
            *size = sizeof(g_tUIAllData.cct_model);
            ptr = &g_tUIAllData.cct_model;
            break;       
#if (PROJECT_TYPE == 308)
        case LFM_DATA_TYPE_STROBE:
		*size = sizeof(g_tUIAllData.strobe_model);
            ptr = &g_tUIAllData.strobe_model;
            break;        
        case LFM_DATA_TYPE_EXPLOSION:
            *size = sizeof(g_tUIAllData.explosion_model);
            ptr = &g_tUIAllData.explosion_model;
            break;        
        case LFM_DATA_TYPE_FAULTYBULB:
            *size = sizeof(g_tUIAllData.faultybulb_model);
            ptr = &g_tUIAllData.faultybulb_model;
            break;     
#endif 
#if (PROJECT_TYPE == 307)
		case LFM_DATA_TYPE_PULSING_II:
            *size = sizeof(g_tUIAllData.pulsing2_model);
            ptr = &g_tUIAllData.pulsing2_model;
            break;
		case LFM_DATA_TYPE_STROBE_II:
			*size =sizeof(g_tUIAllData.strobe2_model);
			ptr = &g_tUIAllData.strobe2_model;   
			break;
		case LFM_DATA_TYPE_EXPLOSION_II:
			*size =sizeof(g_tUIAllData.explosion2_model);
			ptr = &g_tUIAllData.explosion2_model;      
			break;	
		case LFM_DATA_TYPE_FAULTYBULB_II:
			*size =sizeof(g_tUIAllData.faultybulb2_model);
			ptr = &g_tUIAllData.faultybulb2_model;      
			break;		
	   case LFM_DATA_TYPE_WELDING_II:
            *size = sizeof(g_tUIAllData.welding2_model);
            ptr = &g_tUIAllData.welding2_model;
            break;  
	   case LFM_DATA_TYPE_COPCAR_II:
            *size = sizeof(g_tUIAllData.copcar2_model);
            ptr = &g_tUIAllData.copcar2_model;
            break;  
#endif
       case LFM_DATA_TYPE_CANDLE:
            *size = sizeof(g_tUIAllData.candle_model);
            ptr = &g_tUIAllData.candle_model;
            break;   
        case LFM_DATA_TYPE_CLUBLIGHTS:
            *size = sizeof(g_tUIAllData.clublights_model);
            ptr = &g_tUIAllData.clublights_model;
            break;                        
        case LFM_DATA_TYPE_COLORCHASE:   
            *size = sizeof(g_tUIAllData.colorchase_model);
            ptr = &g_tUIAllData.colorchase_model;
            break;
#if (PROJECT_TYPE == 308)	
		case LFM_DATA_TYPE_PULSING:
            *size = sizeof(g_tUIAllData.pulsing_model);
            ptr = &g_tUIAllData.pulsing_model;
            break;   		
        case LFM_DATA_TYPE_TV:
            *size = sizeof(g_tUIAllData.tv_model);
            ptr = &g_tUIAllData.tv_model;
            break;        
#elif (PROJECT_TYPE == 307)	
		case LFM_DATA_TYPE_PULSING:
            *size = sizeof(g_tUIAllData.pulsing2_model);
            ptr = &g_tUIAllData.pulsing2_model;
            break;   
        case LFM_DATA_TYPE_TV:
            break;                                
        case LFM_DATA_TYPE_TV_II:
            *size = sizeof(g_tUIAllData.tv2_model);
            ptr = &g_tUIAllData.tv2_model;
            break;    
#endif		
        case LFM_DATA_TYPE_PAPARAZZI:
            *size = sizeof(g_tUIAllData.paparazzi_model);
            ptr = &g_tUIAllData.paparazzi_model;
            break;   
#if (PROJECT_TYPE == 307)	
		case LFM_DATA_TYPE_LIGHTNING_II:
            *size = sizeof(g_tUIAllData.lightning2_model);
            ptr = &g_tUIAllData.lightning2_model;
            break;  
		case LFM_DATA_TYPE_FIRE_II:
            *size = sizeof(g_tUIAllData.fire2_model);
            ptr = &g_tUIAllData.fire2_model;
            break;  
#endif	
#if (PROJECT_TYPE == 308)		
        case LFM_DATA_TYPE_LIGHTNING:
            *size = sizeof(g_tUIAllData.lightning_model);
            ptr = &g_tUIAllData.lightning_model;
            break;      
#endif   		
        case LFM_DATA_TYPE_FIREWORKS:
            *size = sizeof(g_tUIAllData.fireworks_model);
            ptr = &g_tUIAllData.fireworks_model;
            break;   
#if (PROJECT_TYPE == 308)			
        case LFM_DATA_TYPE_FIRE:
            *size = sizeof(g_tUIAllData.fire_model);
            ptr = &g_tUIAllData.fire_model;
            break;  
#endif   		
        case LFM_DATA_TYPE_PARTYLIGHT:
            *size = sizeof(g_tUIAllData.partylight_model);
            ptr = &g_tUIAllData.partylight_model;
            break;     
		case LFM_DATA_TYPE_CCT_LIMIT:  
		case LFM_DATA_TYPE_HSI_LIMIT:
		case LFM_DATA_TYPE_SOURCE_SET:
		case LFM_DATA_TYPE_GEL_SET:
		case LFM_DATA_TYPE_HSI_SET:
		case LFM_DATA_TYPE_CCT_SET:
		case LFM_DATA_TYPE_XY_SET:
		case LFM_DATA_TYPE_RGB_SET:
			ptr = lfm_get_settiing_storage(size);
			break;
        default:
            *size = 0;
            ptr = NULL;
    }

    return ptr;
}


/**
 * @brief 数据同步动作发送回调函数
 * 
 * @param type 数据类型
 * @param size 该数据类型的数据大小
 * @return void* 返回数据指针
 */
void lfm_write_data_by_type(uint8_t  type)
{
	uint8_t mode = date_center_get_light_mode();
    switch (type)
    {
        case LIGHT_MODE_HSI:
            data_center_write_light_data(type, &g_tUIAllData.hsi_model); 
            break;
        case LIGHT_MODE_RGB:
			data_center_write_light_data(type, &g_tUIAllData.rgb_model); 
            break;        
        case LIGHT_MODE_GEL:
			data_center_write_light_data(type, &g_tUIAllData.gel_model); 
            break;        
        case LIGHT_MODE_XY:
			data_center_write_light_data(type, &g_tUIAllData.xy_model); 
            break;        
        case LIGHT_MODE_SOURCE:
			data_center_write_light_data(type, &g_tUIAllData.source_model); 
            break;        
        case LIGHT_MODE_CCT:
			data_center_write_light_data(type, &g_tUIAllData.cct_model); 
            break;       
#if (PROJECT_TYPE == 308)
        case LIGHT_MODE_FX_STROBE:
			data_center_write_light_data(type, &g_tUIAllData.strobe_model);
            break;        
        case LIGHT_MODE_FX_EXPLOSION:
            data_center_write_light_data(type, &g_tUIAllData.explosion_model);
            break;        
        case LIGHT_MODE_FX_FAULT_BULB:
            data_center_write_light_data(type, &g_tUIAllData.faultybulb_model);
            break;     
#endif 
#if (PROJECT_TYPE == 307)
		case LIGHT_MODE_FX_PULSING_II:
            data_center_write_light_data(type, &g_tUIAllData.pulsing2_model);
            break;
		case LIGHT_MODE_FX_STROBE_II:
			data_center_write_light_data(type, &g_tUIAllData.strobe2_model);   
			break;
		case LIGHT_MODE_FX_EXPLOSION_II:
			data_center_write_light_data(type, &g_tUIAllData.explosion2_model);      
			break;	
		case LIGHT_MODE_FX_FAULT_BULB_II:
			
			data_center_write_light_data(type, &g_tUIAllData.faultybulb2_model);     
			break;		
	   case LIGHT_MODE_FX_WELDING_II:
			data_center_write_light_data(type, &g_tUIAllData.welding2_model);
  
            break;  
	   case LIGHT_MODE_FX_COP_CAR_II:
			data_center_write_light_data(type, &g_tUIAllData.copcar2_model);
        
            break;  
#endif
       case LIGHT_MODE_FX_CANDLE:
			data_center_write_light_data(type, &g_tUIAllData.candle_model);
            
            break;   
        case LIGHT_MODE_FX_CLUBLIGHTS:
			data_center_write_light_data(type, &g_tUIAllData.clublights_model);
           
            break;                        
        case LIGHT_MODE_FX_COLOR_CHASE:   
			data_center_write_light_data(type, &g_tUIAllData.colorchase_model);
         
            break;
#if (PROJECT_TYPE == 308)	
		case LIGHT_MODE_FX_PULSING:
			data_center_write_light_data(type, &g_tUIAllData.pulsing_model);
           
            break;   		
        case LIGHT_MODE_FX_TV:
			data_center_write_light_data(type, &g_tUIAllData.tv_model);
         
            break;        
#elif (PROJECT_TYPE == 307)	
		case LIGHT_MODE_FX_PULSING:
			data_center_write_light_data(type, &g_tUIAllData.pulsing2_model);
         
            break;   
        case LIGHT_MODE_FX_TV:
		
            break;                                
        case LIGHT_MODE_FX_TV_II:
           data_center_write_light_data(type, &g_tUIAllData.tv2_model);
          
            break;    
#endif		
        case LIGHT_MODE_FX_PAPARAZZI:
            data_center_write_light_data(type, &g_tUIAllData.paparazzi_model);
     
            break;   
#if (PROJECT_TYPE == 307)	
		case LIGHT_MODE_FX_LIGHTNING_II:
			data_center_write_light_data(type, &g_tUIAllData.lightning2_model);
            
            break;  
		case LIGHT_MODE_FX_FIRE_II:
            data_center_write_light_data(type, &g_tUIAllData.fire2_model);
        
            break;  
#endif	
#if (PROJECT_TYPE == 308)		
        case LIGHT_MODE_FX_LIGHTNING:
            data_center_write_light_data(type, &g_tUIAllData.lightning_model);
           
            break;      
#endif   		
        case LIGHT_MODE_FX_FIREWORKS:
			data_center_write_light_data(type, &g_tUIAllData.fireworks_model);
           
            break;   
#if (PROJECT_TYPE == 308)			
        case LIGHT_MODE_FX_FIRE:
            data_center_write_light_data(type, &g_tUIAllData.fire_model);
           
            break;  
#endif   		
        case LIGHT_MODE_FX_PARTY_LIGHTS:
			data_center_write_light_data(type, &g_tUIAllData.partylight_model);
         
            break;     
		case LFM_DATA_TYPE_CCT_LIMIT:  
		case LFM_DATA_TYPE_HSI_LIMIT:
		case LFM_DATA_TYPE_SOURCE_SET:
		case LFM_DATA_TYPE_GEL_SET:
		case LFM_DATA_TYPE_HSI_SET:
		case LFM_DATA_TYPE_CCT_SET:
		case LFM_DATA_TYPE_XY_SET:
		case LFM_DATA_TYPE_RGB_SET:
		break;
        default:break;
    }

  
}


static int upgrade_start_cb(struct lfm_core* core,  struct lfm_upgrade_header* p_header, uint32_t* prog_size)
{
	int res = 0;
    struct lfm_firmware_info* firmware_bin = &core->upgrade.firmware_bin;
	
	lfm_status_offer.update_firmware_state = FIRMWARE_UPD_STATUS_UPDING; 
	data_center_write_sys_info(SYS_INFO_USB, &lfm_status_offer);
	user_enter_updating(); 
    switch (p_header->type)
    {
        case FIRMWARE_TYPE_CTRL_BOX:
        {
			os_ev_publish_event(MAIN_EV_LIGHT, EV_DATA_LIGHT_SWITCH, 0);
			lfm_upgrade_transfer_wait_finished(100);
            AES_init_ctx_iv(&lfm_ctx, aes_key, aes_iv);	
			update_open(USB_UPDATA_BOX_DEVICE, "firmware");
			update_erase(USB_UPDATA_BOX_DEVICE, 0, MAX_FIRMWARE_FILE_SIZE);
			update_write(USB_UPDATA_BOX_DEVICE, 0, (uint8_t*)firmware_bin, sizeof(sfirmwarebin));
			*prog_size = 128;	
        }
        break;
        case FIRMWARE_TYPE_LAMP:
        {
			lfm_upgrade_transfer_wait_finished(500);
			res = update_open(USB_UPDATA_LAMP_DEVICE, "lamp_firmware.bin");//485发送start指令
			if(res != 0)
				break;
			res = update_erase(USB_UPDATA_LAMP_DEVICE, 0, firmware_bin->FirmwareSize);  //485发送升级文件长度信息
			if(res != 0)
				break;
			res = update_write(USB_UPDATA_LAMP_DEVICE, p_header->prog_addr, (const uint8_t*)p_header->data, p_header->data_len);
			if(res != 0)
				break;
			*prog_size = 2;	
        }
        break;  
		case FIRMWARE_TYPE_ELE_FRNSEL:
        {
			os_ev_publish_event(MAIN_EV_LIGHT, EV_DATA_LIGHT_SWITCH, 0);
			lfm_upgrade_transfer_wait_finished(300);
			res = update_open(USB_UPDATA_LAMP_DEVICE, "ele_frnsel_firmware.bin");//485发送start指令
			if(res != 0)
				break;
			res = update_erase(USB_UPDATA_LAMP_DEVICE, 0, firmware_bin->FirmwareSize);  //485发送升级文件长度信息
			if(res != 0)
				break;
			res = update_write(USB_UPDATA_LAMP_DEVICE, p_header->prog_addr, (const uint8_t*)p_header->data, p_header->data_len);
			if(res != 0)
				break;
//            bool ok = lfm_send_upgrade_data_to_lamp_start(2);
//            ok = lfm_send_upgrade_data_to_lamp_size(firmware_bin->FirmwareSize);
//			lfm_upgrade_transfer_wait_finished(5000);			
//            lfm_send_upgrade_data_to_lamp_data(p_header->data, p_header->data_len, 1);
			
			*prog_size = 2;	
        }
        break; 
		case FIRMWARE_TYPE_ELE_BRACKET:
        {
			os_ev_publish_event(MAIN_EV_LIGHT, EV_DATA_LIGHT_SWITCH, 0);
			lfm_upgrade_transfer_wait_finished(500);
			res = update_open(USB_UPDATA_LAMP_DEVICE, "ele_bracket_firmware.bin");//485发送start指令
			if(res != 0)
				break;
			res = update_erase(USB_UPDATA_LAMP_DEVICE, 0, firmware_bin->FirmwareSize);  //485发送升级文件长度信息
			if(res != 0)
				break;
			res = update_write(USB_UPDATA_LAMP_DEVICE, p_header->prog_addr, (const uint8_t*)p_header->data, p_header->data_len);
			if(res != 0)
				break;
//            bool ok = lfm_send_upgrade_data_to_lamp_start(3);
//            ok = lfm_send_upgrade_data_to_lamp_size(firmware_bin->FirmwareSize);	
//			lfm_upgrade_transfer_wait_finished(5000);
//            lfm_send_upgrade_data_to_lamp_data(p_header->data, p_header->data_len, 1);
			*prog_size = 2;	
        }
        break; 
		case FIRMWARE_TYPE_BMP:
        {
			os_ev_publish_event(MAIN_EV_LIGHT, EV_DATA_LIGHT_SWITCH, 0);
			lfm_upgrade_transfer_wait_finished(500);
			uint16_t erase_number = BMP_UPDATE_START_ADDRESS/1024/64;
			for(uint8_t i = erase_number; i < 32 + erase_number; i++)
			{
				dev_w25qxx_erase_64sectors(i);
				osDelay(1);
			}
			dev_w25qxx_write_data(BMP_UPDATE_START_ADDRESS+p_header->prog_addr, (uint8_t *)firmware_bin, sizeof(sfirmwarebin));
			*prog_size = 128;	
        }
        break;      
    }
	return res;
}

static int upgrade_data_cb(struct lfm_core* core,  struct lfm_upgrade_header* p_header, uint32_t* cur_prog_size)
{
	int res = 0;
	
    switch(p_header->type)
    {
        case FIRMWARE_TYPE_CTRL_BOX:
        {
            /* 解密 */
			AES_CBC_decrypt_buffer(&lfm_ctx, p_header->data, p_header->data_len);//解密
            
            /* 计算校验和 */
            core->upgrade.crc32 = CRC32_Calculate(core->upgrade.crc32, p_header->data, p_header->data_len);
            
            /* 写Flash */
			update_write(USB_UPDATA_BOX_DEVICE, core->upgrade.next_addr, p_header->data, p_header->data_len);
            *cur_prog_size = p_header->data_len;
        }
        break;
        case FIRMWARE_TYPE_LAMP:
        {
//            uint32_t block_cnt = core->upgrade.next_addr;                    
//            bool ok = lfm_send_upgrade_data_to_lamp_data(p_header->data, p_header->data_len, p_header->prog_addr);
			res = update_write(USB_UPDATA_LAMP_DEVICE, p_header->prog_addr, (const uint8_t*)p_header->data, p_header->data_len);
			*cur_prog_size = 1;
        }
        break;
		case FIRMWARE_TYPE_ELE_FRNSEL:
        {
//            uint32_t block_cnt = core->upgrade.next_addr;    
			res = update_write(USB_UPDATA_LAMP_DEVICE, p_header->prog_addr, (const uint8_t*)p_header->data, p_header->data_len);
//            bool ok = lfm_send_upgrade_data_to_lamp_data(p_header->data, p_header->data_len, block_cnt);	
			*cur_prog_size = 1;
        }
        break;
		case FIRMWARE_TYPE_ELE_BRACKET:
        {
//            uint32_t block_cnt = core->upgrade.next_addr;    
			res = update_write(USB_UPDATA_LAMP_DEVICE, p_header->prog_addr, (const uint8_t*)p_header->data, p_header->data_len);
//            bool ok = lfm_send_upgrade_data_to_lamp_data(p_header->data, p_header->data_len, block_cnt);	
			*cur_prog_size = 1;
        }
        break;
		case FIRMWARE_TYPE_BMP:
        {
			dev_w25qxx_write_data(BMP_UPDATE_START_ADDRESS+core->upgrade.next_addr, p_header->data, p_header->data_len);
			*cur_prog_size = p_header->data_len;
        }
        break;
		default:break;
    }
	
	return res;
}

static int upgrade_end_cb(struct lfm_core* core,  struct lfm_upgrade_header* p_header)
{
	int res = 0;
	uint32_t crc;
	struct sys_info_accessories lfm_access_info;
	
    switch (p_header->type)
    {
        case FIRMWARE_TYPE_CTRL_BOX:
        {
            osDelay(1000);
			LaunchStatus = STATUS_NEED_OTA;  //TODO
			uint8_t lang = 0;
                
			data_center_read_config_data(SYS_CONFIG_LANG, &lang);
			if(lang == LANGU_EN)
				LagStatus = STATUS_EN_LANGUAGE;
			else if(lang == LANGU_CHINESE)
				LagStatus = STATUS_CN_LANGUAGE;
			app_ota_mcu_system_reset();
        }
        break;
        case FIRMWARE_TYPE_LAMP:
        {
            //转发数据.
//            lfm_send_upgrade_data_to_lamp_crc32(0); 
			crc = p_header->prog_addr;
			res = update_close(USB_UPDATA_LAMP_DEVICE,(uint32_t*)crc);  
			osDelay(500);
			light_rs485_link_set(0);
        }
        break;
		case FIRMWARE_TYPE_ELE_FRNSEL:
        {
            //转发数据.
//            lfm_send_upgrade_data_to_lamp_crc32(0);   
			crc = p_header->prog_addr;
			res = update_close(USB_UPDATA_LAMP_DEVICE,(uint32_t*)crc); 
			osDelay(500);
			data_center_read_sys_info(SYS_INFO_ACCESS, &lfm_access_info);
			lfm_access_info.fresnel_state = 0;
			data_center_write_sys_info(SYS_INFO_ACCESS, &lfm_access_info);
			
        }
        break;
		case FIRMWARE_TYPE_ELE_BRACKET:
        {
            //转发数据.
			crc = p_header->prog_addr;
			res = update_close(USB_UPDATA_LAMP_DEVICE,(uint32_t*)crc); 
//            lfm_send_upgrade_data_to_lamp_crc32(0);  
			osDelay(500);
			data_center_read_sys_info(SYS_INFO_ACCESS, &lfm_access_info);
			lfm_access_info.yoke_state = 0;
			data_center_write_sys_info(SYS_INFO_ACCESS, &lfm_access_info);
        }
        break;
		case FIRMWARE_TYPE_BMP:
        {
				osDelay(500);
				LaunchStatus = STATUS_NEED_OTA;  //TODO
				uint8_t lang = 0;
                
				data_center_read_config_data(SYS_CONFIG_LANG, &lang);
				if(lang == LANGU_EN)
					LagStatus = STATUS_EN_LANGUAGE;
				else if(lang == LANGU_CHINESE)
					LagStatus = STATUS_CN_LANGUAGE;
				app_ota_mcu_system_reset();
        }
		break;
		default:break;
    }
	return res;
}

/**
 * @brief 灯体是否存在
 * 
 * @return true 存在
 * @return false 不存在
 */
//static bool lamp_is_exist(void)
//{
//    return true;
//}

/**
 * @brief 检验固件类型
 * 
 * @param info 
 * @return int 
 */
static int check_firmware(uint8_t type, struct lfm_firmware_info* info)
{
	switch (type)
    {
        case FIRMWARE_TYPE_CTRL_BOX:
        {
            #if PROJECT_TYPE == 307
			if (0 == strcmp(info->ProductName, "PR_307_L3"))
			{
				if (info->FirmwareSize > MAX_FIRMWARE_FILE_SIZE)
				{
					return LFM_ERR_FW_OVER_SIZE;
				}		
			}
			#elif PROJECT_TYPE == 308
			if (0 == strcmp(info->ProductName, "PR_308_L2"))
			{
				if (info->FirmwareSize > MAX_FIRMWARE_FILE_SIZE)
				{
					return LFM_ERR_FW_OVER_SIZE;
				}		
			}
			#endif
			if(info->HardwareVersion != 0x10)    
			{
				return LFM_ERR_FW_TYPE;
			}
        }
        break;
        case FIRMWARE_TYPE_LAMP:
		case FIRMWARE_TYPE_ELE_FRNSEL:
		case FIRMWARE_TYPE_ELE_BRACKET:
        {
                     
        }
        break;
		case FIRMWARE_TYPE_BMP:
        {            
			#if PROJECT_TYPE==307
			if(0 != strncmp(info->ProductName, "PR_307_L3_BMP", 14))
			{  
				return LFM_ERR_FW_TYPE;
			}
			#endif
			#if PROJECT_TYPE==308
			if(0 != strncmp(info->ProductName, "PR_308_L2_BMP", 14))
			{  
				return LFM_ERR_FW_TYPE;
			}
			#endif
			if(info->HardwareVersion != 0x10)    
			{
				return LFM_ERR_FW_TYPE;
			}
        }
		break;
		default:break;
    }
	
    return LFM_ERR_OK;
}

/**
 * @brief 事件回调函数
 * 
 * @param e         事件
 * @param setting   通用设置
 * @return int      返回值
 */
//static int event_notify_cb(enum lfm_event e, struct lfm_comm_setting* p_setting)
uint8_t data_des[100];
static int event_notify_cb(enum lfm_event e, uint8_t* data)
{    
	struct lfm_comm_setting* p_setting = (struct lfm_comm_setting*)data;
	struct sys_info_accessories access_info;
	struct sys_info_power lfm_power;
		
	exit_screen_protector();
	data_center_read_sys_info(SYS_INFO_POWER, &lfm_power);
    switch (e)
    {
        case LFM_EVENT_DATA_SYNC:
        { 
			uint16_t data_size = 0;
			uint32_t len;
			uint8_t rx_data[100] = {0};
			uint8_t effect_data[100];
			uint8_t effects_mode;
			uint8_t pid = 0;
			uint8_t spid = 0;
			
			if(lfm_power.state == 0)
			{
				break;
			}
			if(screen_get_act_pid() == PAGE_UPDATE && screen_get_act_spid() == SUB_ID_UPDATING)
			{
				break;
			}
			void* data_des_ptr = lfm_get_data_by_type((enum lfm_data_type)p_setting->data_type, &data_size);
			uint8_t mode = 0;
			uint16_t frq = 0;
			
			data_center_read_config_data(SYS_CONFIG_BLE_CTRL, &mode);
			if(mode != p_setting->ble_status)
			{
				data_center_write_config_data_no_event(SYS_CONFIG_BLE_CTRL, &p_setting->ble_status);
				if(p_setting->ble_status)
				{
					set_ble_power(1);
					set_ble_power_status(1);
				}
				else
				{
					set_ble_power(1);
					set_ble_power_status(0);
				}
			}
			data_center_read_config_data(SYS_CONFIG_CRMX_CTRL, &mode);
			if(mode != p_setting->crmx_status)
			{
				data_center_write_config_data_no_event(SYS_CONFIG_CRMX_CTRL, &p_setting->crmx_status);
				if(p_setting->crmx_status)
				{
					dev_crmx_power_ctrl(1);
				}
				else
				{
					dev_crmx_power_ctrl(0);
				}
			}
			data_center_read_config_data(SYS_CONFIG_CURVE_TYPE, &mode);
			if(mode != p_setting->curve)
			{
				data_center_write_config_data_no_event(SYS_CONFIG_CURVE_TYPE, &p_setting->curve);
				app_light_flags_set(LIGHT_RS485_CURVE_TYPE_FLAG);
			}
			data_center_write_config_data_no_event(SYS_CONFIG_LANG, &p_setting->language);
			data_center_read_config_data(SYS_CONFIG_FRQ, &frq);
			if(frq != p_setting->frequency)
			{
				data_center_write_config_data_no_event(SYS_CONFIG_FRQ, &p_setting->frequency);
				app_light_flags_set(LIGHT_RS485_FRQ_FLAG);
			}
			data_center_read_config_data(SYS_CONFIG_OUTPUT_MODE, &mode);
			if(mode != p_setting->output_mode)
			{
				data_center_write_config_data_no_event(SYS_CONFIG_OUTPUT_MODE, &p_setting->output_mode);
				app_light_flags_set(LIGHT_RS485_OUTPUT_MODE_FLAG);
			}
			data_center_read_config_data(SYS_CONFIG_HIGH_SPEED_MODE, &mode);
			if(mode != p_setting->hsmode)
			{
				ui_set_hs_mode_state(p_setting->hsmode);
				data_center_write_config_data_no_event(SYS_CONFIG_HIGH_SPEED_MODE, &p_setting->hsmode);
				app_light_flags_set(LIGHT_RS485_HIGHT_MODE_FLAG);
			}
			data_center_write_config_data_no_event(SYS_CONFIG_STUDIO_MODE, &p_setting->studio_mode);
			data_center_read_config_data(SYS_CONFIG_FAN_MODE, &mode);
			if(mode != p_setting->fan_mode)
			{
				data_center_write_config_data_no_event(SYS_CONFIG_FAN_MODE, &p_setting->fan_mode);
				app_light_flags_set(LIGHT_RS485_FAN_MODE_FLAGE);
			}
			data_center_read_config_data(SYS_CONFIG_ETH_START, &mode);
			if(mode != p_setting->eth_state)
			{
				data_center_write_config_data(SYS_CONFIG_ETH_START, &p_setting->eth_state);
			}
            g_tUIAllData.sys_menu_model.ble_on = p_setting->ble_status;
            g_tUIAllData.sys_menu_model.crmx_on = p_setting->crmx_status;
            g_tUIAllData.sys_menu_model.curve_type = p_setting->curve;
            g_tUIAllData.sys_menu_model.lang = p_setting->language;
			data_center_write_config_data_no_event(SYS_CONFIG_LANG, &g_tUIAllData.sys_menu_model.lang);
            g_tUIAllData.sys_menu_model.frequency = p_setting->frequency;
            g_tUIAllData.sys_menu_model.output_mode = p_setting->output_mode;
			data_center_read_config_data(SYS_CONFIG_DMX_LOSS_BEHAVIOR, &mode);
			if(mode != p_setting->dmx_loss_behavior)
			{
				g_tUIAllData.sys_menu_model.dmx_loss_behavior = p_setting->dmx_loss_behavior;
				data_center_write_config_data(SYS_CONFIG_DMX_LOSS_BEHAVIOR, &g_tUIAllData.sys_menu_model.dmx_loss_behavior);
			}
			data_center_read_config_data(SYS_CONFIG_DMX_PROFILE, &mode);
			if(mode != p_setting->dmx_profile_index)
			{
				g_tUIAllData.sys_menu_model.dmx_profile_index = p_setting->dmx_profile_index;
				data_center_write_config_data(SYS_CONFIG_DMX_PROFILE, &g_tUIAllData.sys_menu_model.dmx_profile_index);
			}
            g_tUIAllData.sys_menu_model.fan_mode = p_setting->fan_mode;    
			len = &p_setting->data[0]-&data[0];
			if(data_des_ptr != NULL)
			{
				for(uint8_t i = 0;i < data_size; i++)
				{
					effect_data[i] = *(uint8_t*)(data_des_ptr+i);
				}
				for(uint8_t i = 0;i < data_size; i++)
				{
					rx_data[i] = data[len+i];
				}
				effects_mode = dev_get_effects_type_by_lfm((enum lfm_data_type)p_setting->data_type);
				if(memcmp((char *)effect_data, (char *)rx_data, data_size) != 0)
				{
					if ((p_setting->data_type != LFM_DATA_TYPE_NONE) && (p_setting->data_size != 0))
					{				
						if (data_des_ptr != NULL  \
									&& (data_size != 0 && p_setting->data_size == data_size))
						{
							memcpy(data_des_ptr, p_setting->data, p_setting->data_size);	
								
						}						
						#if PROJECT_TYPE==307
//						page_set_invalid_type(PAGE_INVALID_TYPE_ALL);
						#endif
						lfm_write_data_by_type(effects_mode);
						#if PROJECT_TYPE==308
						if(effects_mode == LIGHT_MODE_FX_EXPLOSION)
						{
							g_tUIAllData.explosion_model.state = 0;
						}
						else if(effects_mode == LIGHT_MODE_FX_LIGHTNING)
						{
							if(g_tUIAllData.lightning_model.trigger == 1)
							{
								g_tUIAllData.lightning_model.trigger = 0;
							}
						}
						#endif
					}             
				}
				if(effects_mode != date_center_get_light_mode())
				{
					memcpy(data_des_ptr, p_setting->data, p_setting->data_size);	
					lfm_write_data_by_type(effects_mode);
				}
			}
			pid = screen_get_act_pid();
			spid = screen_get_act_spid();
			if(pid == PAGE_WORK_MODE && spid != SUB_ID_SYNC_FOLLWO_MODE)
			{
				user_turn_to_page(PAGE_WORK_MODE, SUB_ID_SYNC_FOLLWO_MODE, false);
			}
			else if(pid != PAGE_WORK_MODE)
			{
				user_turn_to_page(PAGE_WORK_MODE, SUB_ID_SYNC_FOLLWO_MODE, false);
			}
			else if(pid == PAGE_WORK_MODE && spid == SUB_ID_WORK_MODE_LIST)
			{
				user_turn_to_page(PAGE_MENU, 0, false);
			}
        }
        break;
		case LFM_EVENT_ELECTRIC_MOVE:
		{
			int16_t angle_value1;
			float angle;
			uint8_t tiltm;
			
			data_center_read_config_data(SYS_CONFIG_ANGLE_LIMIT, &tiltm);
			if(lfm_power.state == 0)
			{
				break;
			}
			data_center_read_sys_info(SYS_INFO_ACCESS, &access_info);
			if(access_info.fresnel_state == 1)
			{
				data_center_read_config_data(SYS_CONFIG_FRESNEL_ANGLE, &angle);
				angle_value1 = angle * 100;
				
				if(angle_value1 != p_setting->fresnel_angle)
				{
					angle = ((float)p_setting->fresnel_angle/100.0f);
					data_center_write_config_data_no_event(SYS_CONFIG_FRESNEL_ANGLE, &angle);
					app_light_flags_set(LIGHT_RS485_FRESNEL_ANGLE_FLAG);
				}
			}
			if(access_info.yoke_state == 1)
			{
				data_center_read_config_data(SYS_CONFIG_YOKE_PAN, &angle);
				angle_value1 = angle * 100;
				
				if(angle_value1 != p_setting->yoke_pan)
				{
					angle = ((float)p_setting->yoke_pan/100.0f);
					angle = ui_accessory_get_angle_limit(angle, 0);
					data_center_write_config_data_no_event(SYS_CONFIG_YOKE_PAN, &angle);
					app_light_flags_set(LIGHT_RS485_YOKE_PAN_FLAG);
				}
				data_center_read_config_data(SYS_CONFIG_YOKE_TILT, &angle);
				angle_value1 = angle * 100;
				
				if(angle_value1 != p_setting->yoke_tilt)
				{
					angle = ((float)p_setting->yoke_tilt/100.0f);
					angle = ui_accessory_get_angle_limit(angle, 1);
					data_center_write_config_data_no_event(SYS_CONFIG_YOKE_TILT, &angle);
					app_light_flags_set(LIGHT_RS485_YOKE_TILT_FLAG);
				}
			}
			
			if(p_setting->motor_reset == 1)
			{
				p_setting->motor_reset = 0;
				ui_set_motor_reset();
			}
			if(p_setting->motor_reset == 2)
			{
				p_setting->motor_reset = 0;
				ui_set_frsenel_reset();
			}
		}
		break;
        case LFM_EVENT_BLE_RESET:
        {
			if(lfm_power.state == 0)
			{
				break;
			}
			if(screen_get_act_pid() == PAGE_UPDATE && screen_get_act_spid() == SUB_ID_UPDATING)
			{
				break;
			}
            g_ptUIStateData->ble_status.status = BLE_RESET_START;
			ui_ble_start_reset();     
            user_turn_to_page(PAGE_CONTROL_SYSTEM, SUB_ID_BLE_RESETING, true);
        }
        break;
        case LFM_EVENT_PAGE_SYNC:
        {
			if(lfm_power.state == 0)
			{
				break;
			}
			if(screen_get_act_pid() == PAGE_UPDATE && screen_get_act_spid() == SUB_ID_UPDATING)
			{
				break;
			}
            user_turn_to_page(p_setting->pid, p_setting->spid, false);
        }
        break;
        case LFM_EVENT_CRMX_UNPAIR:
        {
            //do nothing...
            g_ptUIStateData->lumen_status.unpair_status = LUMENRADIO_UNPAIR_START;            
        }
        break;
        case LFM_EVENT_FACTORY_RESET:
        {
			uint8_t hs_mode = 0;
			if(lfm_power.state == 0)
			{
				break;
			}
			if(screen_get_act_pid() == PAGE_UPDATE && screen_get_act_spid() == SUB_ID_UPDATING)
			{
				break;
			}
			data_center_read_sys_info(SYS_INFO_POWER, &lfm_power);
			if(lfm_power.indoor_powe == 1)
			{
				#if PROJECT_TYPE==308
				gui_set_power_limit(1700);
				#endif
				#if PROJECT_TYPE==307
				gui_set_power_limit(2000);
				#endif
			}
			else if(lfm_power.indoor_powe == 2)
			{
				#if PROJECT_TYPE==308
				gui_set_power_limit(1200);
				#endif
				#if PROJECT_TYPE==307
				gui_set_power_limit(1200);
				#endif
			}
			ui_max_power_flag_set(0);
			data_center_read_config_data(SYS_CONFIG_HIGH_SPEED_MODE, &hs_mode);
			ui_restore_def_setting();
			if(hs_mode)
			{
				ui_set_hs_mode_state(0);
				data_center_write_config_data(SYS_CONFIG_HIGH_SPEED_MODE, &hs_mode);
			}
            user_turn_to_page(PAGE_FACTORY, 1, false);
			ui_set_motor_reset();
        }
        break;
		case LFM_EVENT_POWER:
		{
			uint8_t power;
			
			data_center_read_sys_info(SYS_INFO_POWER, &lfm_power);
			power = p_setting->power_state;
			if(lfm_power.state != power)
				sys_dmx_power_on_set(1);
		}
		break;
		default: break;
    }
	
	return 0;
}

/**
 * @brief 普通事件发送之前刷新
 * 
 * @param e 
 * @param p_setting 
 * @return int 
 */
static int refresh_before_send(enum lfm_event e, struct lfm_comm_setting* p_setting)
{
	uint8_t mode = 0;
	uint16_t addr = 0;
	float  angle_int = 0;
	float angle = 0;
	struct sys_info_power lfm_power;
		
	data_center_read_sys_info(SYS_INFO_POWER, &lfm_power);
	p_setting->power_state = lfm_power.state;
	data_center_read_config_data(SYS_CONFIG_BLE_CTRL, &mode);
    p_setting->ble_status = mode;
	data_center_read_config_data(SYS_CONFIG_CRMX_CTRL, &mode);
    p_setting->crmx_status = mode;
	data_center_read_config_data(SYS_CONFIG_CURVE_TYPE, &mode);
    p_setting->curve = mode;
	data_center_read_config_data(SYS_CONFIG_LANG, &mode);
    p_setting->language = mode;
	data_center_read_config_data(SYS_CONFIG_FRQ, &addr);
    p_setting->frequency = addr;
	data_center_read_config_data(SYS_CONFIG_OUTPUT_MODE, &mode);
    p_setting->output_mode = mode;
	data_center_read_config_data(SYS_CONFIG_STUDIO_MODE, &mode);
    p_setting->studio_mode = mode;
	data_center_read_config_data(SYS_CONFIG_FAN_MODE, &mode);
    p_setting->fan_mode = mode;
	data_center_read_config_data(SYS_CONFIG_DMX_LOSS_BEHAVIOR, &mode);
    p_setting->dmx_loss_behavior = mode;
	data_center_read_config_data(SYS_CONFIG_DMX_PROFILE, &mode);
    p_setting->dmx_profile_index = mode;
//	data_center_read_config_data(SYS_CONFIG_DMX_ADDR, &addr);
//    p_setting->dmx_address = addr;
    data_center_read_config_data(SYS_CONFIG_YOKE_PAN, &angle);
	angle_int = ui_accessory_get_angle_limit(angle, 0);
	angle_int = angle_int * 100;
	p_setting->yoke_pan = angle_int;
	data_center_read_config_data(SYS_CONFIG_YOKE_TILT, &angle);
	angle_int = ui_accessory_get_angle_limit(angle, 1);
	angle_int = angle_int * 100;
	p_setting->yoke_tilt = angle_int;
	data_center_read_config_data(SYS_CONFIG_FRESNEL_ANGLE, &angle);
	angle_int = angle_int * 100;
	p_setting->fresnel_angle = angle_int;
	data_center_read_config_data(SYS_CONFIG_HIGH_SPEED_MODE, &mode);
	p_setting->hsmode = mode;
	data_center_read_config_data(SYS_CONFIG_ETH_START, &mode);
	p_setting->motor_reset = 0;
	if(ui_motor_reset_status_get() == 1)
	{
		ui_motor_reset_status_set(0);
		p_setting->motor_reset = 1;
	}
	if(ui_motor_reset_status_get() == 2)
	{
		ui_motor_reset_status_set(0);
		p_setting->motor_reset = 2;
	}
	p_setting->eth_state = mode;
    p_setting->lighting_mode = 0;    
    p_setting->pid = screen_get_act_pid();
    p_setting->spid = screen_get_act_spid();
    p_setting->data_type = LFM_DATA_TYPE_NONE;

    if (e == LFM_EVENT_DATA_SYNC)
    {
        p_setting->data_type =  lfm_get_type_by_pid(p_setting->pid);
        void* user_data = lfm_get_data_by_type((enum lfm_data_type)p_setting->data_type, &p_setting->data_size);
        if (user_data != NULL)
        {
            memcpy(p_setting->data, user_data, p_setting->data_size);
        }                    
    }
	return 0;
}


uint8_t lfm_upgrade_send_state_get(void)
{
	return s_lfm_upgrade_state;
}

/**
 * @brief 485设备写
 * 
 * @param data 数据指针
 * @param len  数据长度
 * @return int 返回值
 */
static int rs485_dev_write(uint8_t* data, uint16_t len)
{
//	extern void wired_dmx_transmit_data(uint8_t* data, uint16_t len);
	extern void wired_dmx_transmit_with_break(uint8_t* data, uint16_t size);
    static uint32_t cnt = 0;
	uint8_t send_count = 5;
	
    if (data == NULL || len == 0)
        return -1;
    
	uint16_t msg_size = 0;
	uint8_t* p_data_buff = &tx_frame_buffer[sizeof(rs485_proto_header_t) + sizeof(rs485_cmd_header_t)];
	
	s_lfm_upgrade_state = 1;
	memcpy(p_data_buff, data, len);	
		
	msg_size = len + sizeof(rs485_proto_header_t) + sizeof(rs485_cmd_header_t);		
    init_cmd_header((rs485_cmd_header_t*)(&tx_frame_buffer[0] + sizeof(rs485_proto_header_t)), 1, RS485_Cmd_Version, len);		
    init_message_header((rs485_proto_header_t*)&tx_frame_buffer[0], false, cnt++, msg_size);	

//    wired_dmx_transmit_data(tx_frame_buffer, msg_size);
	do{
		wired_dmx_transmit_with_break(tx_frame_buffer, msg_size);
		osDelay(5);
	}while(send_count--);
	s_lfm_upgrade_state = 0;
    return 0;
}

/**
 * @brief CRMX 设备写
 * 
 * @param data 
 * @param len 
 * @return int 
 */
//static int crmx_dev_write(uint8_t* data, uint16_t len)
//{
//    extern void wired_dmx_transmit_with_break(uint8_t* data, uint16_t len);
//    static uint32_t cnt = 0;

//    int repeat_times = 5;
//	struct lfm_frame_header* p_header = (struct lfm_frame_header*)data;
//	
////	if(p_header->event == LFM_EVENT_UPGRADE)
////		repeat_times = 2;
//    if (data == NULL || len == 0)
//        return -1;
//    
//	uint16_t msg_size = 0;
//	uint8_t* p_data_buff = &tx_frame_buffer[sizeof(rs485_proto_header_t) + sizeof(rs485_cmd_header_t)];
//	
//	memcpy(p_data_buff, data, len);	
//		
//	msg_size = len + sizeof(rs485_proto_header_t) + sizeof(rs485_cmd_header_t);		
//    init_cmd_header((rs485_cmd_header_t*)(&tx_frame_buffer[0] + sizeof(rs485_proto_header_t)), 1, RS485_Cmd_Version, len);		
//    init_message_header((rs485_proto_header_t*)&tx_frame_buffer[0], false, cnt++, msg_size);	

//    do {
//        wired_dmx_transmit_with_break(tx_frame_buffer, msg_size);
//        osDelay(5);
//    } while (--repeat_times > 0);        
//    return 0;
//}


/**
 * @brief 初始化485消息头部
 * 
 * @param p_header 
 * @param ack_en 
 * @param serial_num 
 * @param msg_size 
 */
static void init_message_header(rs485_proto_header_t* p_header, bool ack_en, uint16_t serial_num, uint16_t msg_size)
{
	p_header->start = RS485_START_CODE;
	p_header->proto_ver_major = RS485_PROTO_VER_MAJOR;
	p_header->proto_ver_minor = RS485_PROTO_VER_MINOR;
	p_header->proto_ver_revision = RS485_PROTO_VER_REVISION;	
	p_header->header_size = sizeof(rs485_proto_header_t);
	p_header->serial_num = serial_num;
	p_header->msg_size = msg_size;
	p_header->check_sum = check_sum_calc((uint8_t*)&p_header->header_size, p_header->msg_size - 2);
}	


static void init_cmd_header(rs485_cmd_header_t* p_cmd_header, uint8_t rw, rs485_cmd_enum cmd, uint16_t arg_size)
{
	p_cmd_header->cmd_type = cmd;
	p_cmd_header->arg_size = arg_size;
	p_cmd_header->rw = rw;
}

//static void erase_flash(uint32_t address, uint32_t size)
//{
//    uint8_t number = (UPDATE_START_ADDRESS/1024/64);
//    //默认2M大小删除
//    for(uint8_t erase_number = number; erase_number < number+32; erase_number++)  //跳过图片+文本区
//    {
//        dev_w25qxx_erase_64sectors(erase_number);
//        //增加看门狗喂狗
//    }
////    w25qxx_erase(UPGRADE_START_ADDRESS, UPGRADE_START_ADDRESS + size);
//}

//static void write_flash(uint32_t address, uint8_t* data, uint16_t size)
//{
//	dev_w25qxx_write_data(address, data, size);
////    w25qxx_write(address, data, size);
//}

bool lfm_send_upgrade_data_to_lamp(rs485_cmd_arg_t* p_cmd_arg)
{
	uint16_t msg_size = 0;
    uint16_t arg_size = sizeof(rs485_file_transfer_t);
	uint8_t rx_data[20];
	
    if (p_cmd_arg == NULL)
        return false;

	msg_size = arg_size + sizeof(rs485_proto_header_t) + sizeof(rs485_cmd_header_t);		
    init_cmd_header((rs485_cmd_header_t*)(&tx_frame_buffer[0] + sizeof(rs485_proto_header_t)), 1, RS485_Cmd_FileTransfer, arg_size);		
    init_message_header((rs485_proto_header_t*)&tx_frame_buffer[0], false, 1, msg_size);	

    rs485_usart_tx_data(tx_frame_buffer, msg_size, 0);  
	msg_size = rs485_usart_rx_data(rx_data, 20, 50);
    return true;
}

bool lfm_send_upgrade_data_to_lamp_start(uint8_t file_type)
{   
    rs485_cmd_arg_t* p_cmd_arg = get_cmd_arg((uint8_t*)tx_frame_buffer);

    p_cmd_arg->file_transfer.step = RS485_File_TX_Step_Start;
    p_cmd_arg->file_transfer.file_type = RS485_File_TX_Firmware;
    p_cmd_arg->file_transfer.step_body.start_arg.firmware_start.type = file_type;

    return lfm_send_upgrade_data_to_lamp(p_cmd_arg);
}


bool lfm_send_upgrade_data_to_lamp_size(uint32_t size)
{
    rs485_cmd_arg_t* p_cmd_arg = get_cmd_arg((uint8_t*)tx_frame_buffer);

    p_cmd_arg->file_transfer.step = RS485_File_TX_Step_Size;
    p_cmd_arg->file_transfer.file_type = RS485_File_TX_Firmware;
    p_cmd_arg->file_transfer.step_body.size_arg.file_size = size;
    	
    return lfm_send_upgrade_data_to_lamp(p_cmd_arg);
}

bool lfm_send_upgrade_data_to_lamp_data(uint8_t* buff, uint16_t len, uint32_t block_cnt)
{
    rs485_cmd_arg_t* p_cmd_arg = get_cmd_arg((uint8_t*)tx_frame_buffer);

    p_cmd_arg->file_transfer.step = RS485_File_TX_Step_Data;
    p_cmd_arg->file_transfer.file_type = RS485_File_TX_Firmware;  
    p_cmd_arg->file_transfer.step_body.data_arg.block_cnt = block_cnt;
    p_cmd_arg->file_transfer.step_body.data_arg.block_size = len;

    memset(p_cmd_arg->file_transfer.step_body.data_arg.block_buf, 0, sizeof(p_cmd_arg->file_transfer.step_body.data_arg.block_buf));    
    memcpy(p_cmd_arg->file_transfer.step_body.data_arg.block_buf, buff, len);    

    return lfm_send_upgrade_data_to_lamp(p_cmd_arg);
}

bool lfm_send_upgrade_data_to_lamp_crc32(uint32_t crc32)
{
    rs485_cmd_arg_t* p_cmd_arg = get_cmd_arg((uint8_t*)tx_frame_buffer);

    p_cmd_arg->file_transfer.step = RS485_File_TX_Step_CRC;
    p_cmd_arg->file_transfer.file_type = RS485_File_TX_Firmware;
    p_cmd_arg->file_transfer.step_body.crc_arg.crc32 = crc32;
    	
    return lfm_send_upgrade_data_to_lamp(p_cmd_arg);
}


/**
 * @brief 广播数据的有效页面
 * 
 * @param page_id 
 * @return true 
 * @return false 
 */
//static bool broadcast_valid_page(uint8_t page_id)
//{
//    if (page_id == PAGE_LIGHT_MODE_HSI \
//            || page_id == PAGE_LIGHT_MODE_RGB\
//            || page_id == PAGE_LIGHT_MODE_GEL\
//            || page_id == PAGE_LIGHT_MODE_XY\
//            || page_id == PAGE_LIGHT_MODE_SOURCE\
//            || page_id == PAGE_LIGHT_MODE_CCT\
//            || page_id == PAGE_PULSING\
//            || page_id == PAGE_STROBE\
//            || page_id == PAGE_EXPLOSIOIN\
//            || page_id == PAGE_FAULTYBULB\
//            || page_id == PAGE_WELDING\
//            || page_id == PAGE_COPCAR\
//            || page_id == PAGE_CANDLE\
//            || page_id == PAGE_CLUBLIGHTS\
//            || page_id == PAGE_COLORCHASE\
//            || page_id == PAGE_TV\
//            || page_id == PAGE_PAPARAZZI\
//            || page_id == PAGE_LIGHTNING\
//            || page_id == PAGE_FIREWORKS\
//            || page_id == PAGE_FIRE\

//            || page_id == PAGE_PARTYLIGHT \
//			|| page_id == PAGE_CCT_LIMIT_SETTING\
//			|| page_id == PAGE_RGB_SETTING \
//			|| page_id == PAGE_XY_SETTING \
//			|| page_id == PAGE_GEL_SETTING\
//			|| page_id == PAGE_CCT_SETTING \
//			|| page_id == PAGE_SOURCE_SETTING \
//			|| page_id == PAGE_HSI_LIMIT_SETTING\
//		    || page_id == PAGE_CONTROL_SYSTEM\
//			|| page_id == PAGE_FAN_MODE\
//			|| page_id == PAGE_FREQUENCY_SELECTION\
//			|| page_id == PAGE_OUTPUT_MODE\
//			|| page_id == PAGE_DIMMING_CURVE\
//			|| page_id == PAGE_LANGUAGE\
//			|| page_id == PAGE_PRODUCT_INFO\
//			|| page_id == PAGE_DMX_MODE\
//			|| page_id == PAGE_ELECT_ACCESSORY\
//			|| page_id == PAGE_STUDIO_MODE)

//            return true;
//        
//    return false;
//}

/**
 * @brief 获取光模式
 * 
 * @param page_id 
 * @return true 
 * @return false 
 */
static enum light_mode dev_get_effects_type_by_lfm(enum lfm_data_type type)
{
	enum light_mode mode = LIGHT_MODE_CCT;
	switch (type)
    {
        case LFM_DATA_TYPE_HSI:                  mode = LIGHT_MODE_HSI;        break;
        case LFM_DATA_TYPE_RGB:                  mode = LIGHT_MODE_RGB;        break;
        case LFM_DATA_TYPE_GEL:                  mode = LIGHT_MODE_GEL;        break;
        case LFM_DATA_TYPE_XY:                   mode = LIGHT_MODE_XY;         break;
        case LFM_DATA_TYPE_SOURCE:               mode = LIGHT_MODE_SOURCE;     break;
        case LFM_DATA_TYPE_CCT:                  mode = LIGHT_MODE_CCT;        break;      
#if PROJECT_TYPE==308
		case LFM_DATA_TYPE_PULSING:              mode = LIGHT_MODE_FX_PULSING;    break;
        case LFM_DATA_TYPE_STROBE:               mode = LIGHT_MODE_FX_STROBE;    break;
        case LFM_DATA_TYPE_EXPLOSION:            mode = LIGHT_MODE_FX_EXPLOSION;    break;
        case LFM_DATA_TYPE_FAULTYBULB:           mode = LIGHT_MODE_FX_FAULT_BULB;    break;
        case LFM_DATA_TYPE_WELDING:              mode = LIGHT_MODE_FX_WELDING;    break;
		case LFM_DATA_TYPE_TV:                   mode = LIGHT_MODE_FX_TV;    break;
		case LFM_DATA_TYPE_FIRE:                 mode = LIGHT_MODE_FX_FIRE;    break;
		case LFM_DATA_TYPE_LIGHTNING:            mode = LIGHT_MODE_FX_LIGHTNING;    break;
#elif PROJECT_TYPE==307
		case LFM_DATA_TYPE_PULSING_II:           mode = LIGHT_MODE_FX_PULSING_II;    break;
        case LFM_DATA_TYPE_STROBE_II:            mode = LIGHT_MODE_FX_STROBE_II;    break;
        case LFM_DATA_TYPE_EXPLOSION_II:         mode = LIGHT_MODE_FX_EXPLOSION_II;    break;
        case LFM_DATA_TYPE_FAULTYBULB_II:        mode = LIGHT_MODE_FX_FAULT_BULB_II;    break;
        case LFM_DATA_TYPE_WELDING_II:           mode = LIGHT_MODE_FX_WELDING_II;    break;
        case LFM_DATA_TYPE_COPCAR_II:            mode = LIGHT_MODE_FX_COP_CAR_II;    break;
		case LFM_DATA_TYPE_TV_II:                mode = LIGHT_MODE_FX_TV_II;    break;
		case LFM_DATA_TYPE_FIRE_II:              mode = LIGHT_MODE_FX_FIRE_II;    break;
		case LFM_DATA_TYPE_LIGHTNING_II:         mode = LIGHT_MODE_FX_LIGHTNING_II;    break;
#endif
        case LFM_DATA_TYPE_CANDLE:               mode = LIGHT_MODE_FX_CANDLE;    break;
        case LFM_DATA_TYPE_CLUBLIGHTS:           mode = LIGHT_MODE_FX_CLUBLIGHTS;    break;
        case LFM_DATA_TYPE_COLORCHASE:           mode = LIGHT_MODE_FX_COLOR_CHASE;    break;
        case LFM_DATA_TYPE_PAPARAZZI:            mode = LIGHT_MODE_FX_PAPARAZZI;    break;
        case LFM_DATA_TYPE_FIREWORKS:            mode = LIGHT_MODE_FX_FIREWORKS;    break;
        case LFM_DATA_TYPE_PARTYLIGHT:           mode = LIGHT_MODE_FX_PARTY_LIGHTS;    break;
		case LFM_DATA_TYPE_CCT_LIMIT:	     
		case LFM_DATA_TYPE_HSI_LIMIT:	     
		case LFM_DATA_TYPE_RGB_SET:       	 
		case LFM_DATA_TYPE_XY_SET:           
		case LFM_DATA_TYPE_CCT_SET:          
		case LFM_DATA_TYPE_HSI_SET:          
		case LFM_DATA_TYPE_GEL_SET:          
		case LFM_DATA_TYPE_SOURCE_SET:	         mode = date_center_get_light_mode();	break;
        default:break;
    }

    return  mode;
}

/**
 * @brief 获取光模式
 * 
 * @param page_id 
 * @return true 
 * @return false 
 */
static enum lfm_data_type dev_get_lfm_type_by_lightmode(enum light_mode mode)
{
	enum lfm_data_type type = LFM_DATA_TYPE_CCT;
	
	switch (mode)
    {
        case LIGHT_MODE_HSI:                 type = LFM_DATA_TYPE_HSI;          break;
        case LIGHT_MODE_RGB:                 type = LFM_DATA_TYPE_RGB;          break;
        case LIGHT_MODE_GEL:                 type = LFM_DATA_TYPE_GEL;          break;
        case LIGHT_MODE_XY:                  type = LFM_DATA_TYPE_XY;           break;
        case LIGHT_MODE_SOURCE:              type = LFM_DATA_TYPE_SOURCE;       break;
        case LIGHT_MODE_CCT:                 type = LFM_DATA_TYPE_CCT;          break;      
#if PROJECT_TYPE==308
		case LIGHT_MODE_FX_PULSING:          type = LFM_DATA_TYPE_PULSING;     break;
        case LIGHT_MODE_FX_STROBE:           type = LFM_DATA_TYPE_STROBE;      break;
        case LIGHT_MODE_FX_EXPLOSION:        type = LFM_DATA_TYPE_EXPLOSION;   break;
        case LIGHT_MODE_FX_FAULT_BULB:       type = LFM_DATA_TYPE_FAULTYBULB;  break;
        case LIGHT_MODE_FX_WELDING:          type = LFM_DATA_TYPE_WELDING;     break;
		case LIGHT_MODE_FX_TV:               type = LFM_DATA_TYPE_TV;          break;
		case LIGHT_MODE_FX_FIRE:             type = LFM_DATA_TYPE_FIRE;        break;
		case LIGHT_MODE_FX_LIGHTNING:        type = LFM_DATA_TYPE_LIGHTNING;   break;
#elif PROJECT_TYPE==307
		case LIGHT_MODE_FX_PULSING_II:        type = LFM_DATA_TYPE_PULSING_II;     break;
		case LIGHT_MODE_FX_STROBE_II:         type = LFM_DATA_TYPE_STROBE_II;      break;
        case LIGHT_MODE_FX_EXPLOSION_II:      type = LFM_DATA_TYPE_EXPLOSION_II;   break;
        case LIGHT_MODE_FX_FAULT_BULB_II:     type = LFM_DATA_TYPE_FAULTYBULB_II;  break;
        case LIGHT_MODE_FX_WELDING_II:        type = LFM_DATA_TYPE_WELDING_II;     break;
        case LIGHT_MODE_FX_COP_CAR_II:        type = LFM_DATA_TYPE_COPCAR_II;     break;
		case LIGHT_MODE_FX_TV_II:             type = LFM_DATA_TYPE_TV_II;          break;
		case LIGHT_MODE_FX_FIRE_II:           type = LFM_DATA_TYPE_FIRE_II;        break;
		case LIGHT_MODE_FX_LIGHTNING_II:      type = LFM_DATA_TYPE_LIGHTNING_II;   break;
#endif
		case LIGHT_MODE_FX_CANDLE:            type = LFM_DATA_TYPE_CANDLE;       break;
		case LIGHT_MODE_FX_CLUBLIGHTS:        type = LFM_DATA_TYPE_CLUBLIGHTS;    break;
		case LIGHT_MODE_FX_COLOR_CHASE:       type = LFM_DATA_TYPE_COLORCHASE;     break;
		case LIGHT_MODE_FX_PAPARAZZI:         type = LFM_DATA_TYPE_PAPARAZZI;    break;
		case LIGHT_MODE_FX_FIREWORKS:         type = LFM_DATA_TYPE_FIREWORKS;    break;
		case LIGHT_MODE_FX_PARTY_LIGHTS:      type = LFM_DATA_TYPE_PARTYLIGHT;      break;
        default:break;
    }

    return  type;
}

enum lfm_data_type lfm_get_type_by_pid(uint8_t pid)
{
    enum lfm_data_type type = LFM_DATA_TYPE_NONE;
    switch (pid)
    {
        case PAGE_LIGHT_MODE_HSI:   type = LFM_DATA_TYPE_HSI;        break;
        case PAGE_LIGHT_MODE_RGB:   type = LFM_DATA_TYPE_RGB;        break;
        case PAGE_LIGHT_MODE_GEL:   type = LFM_DATA_TYPE_GEL;        break;
        case PAGE_LIGHT_MODE_XY:    type = LFM_DATA_TYPE_XY;         break;
        case PAGE_LIGHT_MODE_SOURCE:type = LFM_DATA_TYPE_SOURCE;     break;
        case PAGE_LIGHT_MODE_CCT:   type = LFM_DATA_TYPE_CCT;        break;      
#if PROJECT_TYPE==308
		case PAGE_PULSING:          type = LFM_DATA_TYPE_PULSING;    break;
        case PAGE_STROBE:           type = LFM_DATA_TYPE_STROBE;    break;
        case PAGE_EXPLOSIOIN:       type = LFM_DATA_TYPE_EXPLOSION;    break;
        case PAGE_FAULTYBULB:       type = LFM_DATA_TYPE_FAULTYBULB;    break;
        case PAGE_WELDING:          type = LFM_DATA_TYPE_WELDING;    break;
		case PAGE_TV:               type = LFM_DATA_TYPE_TV;    break;
		case PAGE_FIRE:             type = LFM_DATA_TYPE_FIRE;    break;
		case PAGE_LIGHTNING:        type = LFM_DATA_TYPE_LIGHTNING;    break;
#elif PROJECT_TYPE==307
		case PAGE_PULSING:          type = LFM_DATA_TYPE_PULSING_II;    break;
        case PAGE_STROBE:           type = LFM_DATA_TYPE_STROBE_II;    break;
        case PAGE_EXPLOSIOIN:       type = LFM_DATA_TYPE_EXPLOSION_II;    break;
        case PAGE_FAULTYBULB:       type = LFM_DATA_TYPE_FAULTYBULB_II;    break;
        case PAGE_WELDING:          type = LFM_DATA_TYPE_WELDING_II;    break;
        case PAGE_COPCAR:           type = LFM_DATA_TYPE_COPCAR_II;    break;
		case PAGE_TV:               type = LFM_DATA_TYPE_TV_II;    break;
		case PAGE_FIRE:             type = LFM_DATA_TYPE_FIRE_II;    break;
		case PAGE_LIGHTNING:        type = LFM_DATA_TYPE_LIGHTNING_II;    break;
#endif
        case PAGE_CANDLE:           type = LFM_DATA_TYPE_CANDLE;    break;
        case PAGE_CLUBLIGHTS:       type = LFM_DATA_TYPE_CLUBLIGHTS;    break;
        case PAGE_COLORCHASE:       type = LFM_DATA_TYPE_COLORCHASE;    break;
        case PAGE_PAPARAZZI:        type = LFM_DATA_TYPE_PAPARAZZI;    break;
        case PAGE_FIREWORKS:        type = LFM_DATA_TYPE_FIREWORKS;    break;
        case PAGE_PARTYLIGHT:       type = LFM_DATA_TYPE_PARTYLIGHT;    break;
		case PAGE_CCT_LIMIT_SETTING:	type = LFM_DATA_TYPE_CCT_LIMIT;	break;
		case PAGE_HSI_LIMIT_SETTING:	type = LFM_DATA_TYPE_HSI_LIMIT;	break;
		case PAGE_RGB_SETTING:       	type = LFM_DATA_TYPE_RGB_SET;	break;
		case PAGE_XY_SETTING:           type = LFM_DATA_TYPE_XY_SET;	break;
		case PAGE_CCT_SETTING:          type = LFM_DATA_TYPE_CCT_SET;	break;
		case PAGE_HSI_SETTING:          type = LFM_DATA_TYPE_HSI_SET;	break;
		case PAGE_GEL_SETTING:          type = LFM_DATA_TYPE_GEL_SET;	break;
		case PAGE_SOURCE_SETTING:	    type = LFM_DATA_TYPE_SOURCE_SET;	break;
        default:type = dev_get_lfm_type_by_lightmode(date_center_get_light_mode());break;
    }

    return  type;
}




static rs485_cmd_arg_t* get_cmd_arg(uint8_t* buff)
{
    return (rs485_cmd_arg_t*)&buff[sizeof(rs485_proto_header_t) + sizeof(rs485_cmd_header_t)];
}

/**
 * @brief 计算校验和
 * 
 * @param buff 
 * @param size 
 * @return uint8_t 
 */
static uint8_t check_sum_calc(const uint8_t* buff, uint16_t size)
{
    uint8_t check_sum = 0;
    while(size--)
    {
        check_sum += *buff++;
    }
    return check_sum;
}

uint32_t lfm_batch_upgrade_uuid_check(uint32_t uuid_arg)
{
	uint32_t uuid = 0xffffffff;
	
	for(uint8_t i = 0; i < FIRMWARE_TYPE_MAX; i++)
	{
		if(s_batch_upgrade_uuid[i] == uuid_arg)
		{
			uuid =  s_batch_upgrade_uuid[i];
			break;
		}
	}
	
	return uuid;
}

uint8_t lfm_batch_upgrade_type_check(uint32_t uuid_arg)
{
	uint8_t tupe = 0xff;
	
	for(uint8_t i = 0; i < FIRMWARE_TYPE_MAX; i++)
	{
		if(s_batch_upgrade_uuid[i] == uuid_arg)
		{
			tupe =  i;
			break;
		}
	}
	
	return tupe;
}

