#include "app_ble.h"
#include "os_event.h"
#include "hal_uart.h"
#include "dev_ble.h"
#include <string.h>
#include "app_ble_protocol_parse.h"
#include "user_gatt_proto.h"
#include "api_gatt_proto.h"
#include "app_ble_port.h"
#include "app_ota.h"
#include "app_data_center.h"
#include "page_manager.h"
#include "user.h"
#include "base_type.h"
#include "app_gui.h"
#include "app_power.h"
#include "app_dmx.h"
#include "app_err_detect.h"
#include "ui_hal.h"
#include "app_usb.h"

static void app_ble_get_version(void);
static void ble_pull_event_cb(uint32_t event_type, uint32_t event_value);
static void ble_deal_pull_event(void);
static void app_get_ble_power_info(void);
static void app_ble_power_on_init(void);
static void app_ble_deal_send_data(uint8_t ticks);
static void app_ble_usart_user_data(ble_rx_packet *rx_packet);
static void app_ble_clear_error_conut(void);

const osThreadAttr_t g_ble_thread_attr = { 
    .name = "app ble", 
    .priority = osPriorityAboveNormal1, 
    .stack_size = 640 * 4
};

osThreadId_t app_ble_task_id;
uint32_t g_ble_task_static_size;
static dev_ble_t s_dev_ble;
static uint16_t s_get_version_count = 0;
static uint8_t s_get_version_index = 0;

void app_ble_thread_entrance(void *argument)
{
	app_ble_power_on_init();
	os_ev_subscribe_event(MAIN_EV_BLE, OS_EVENT_BLE_POWER | OS_EVENT_BLE_RESET , ble_pull_event_cb);
	
	if(1 == app_dmx_lumenradio_name_flag_get())
		app_dmx_lumenradio_name_flag_set(2);
	for(;;)
	{
		os_ev_pull_event(MAIN_EV_BLE);
		ble_deal_pull_event();
		ble_deal_receive_data(&s_dev_ble);
		app_ble_deal_send_data(20);
        if(1 == get_read_cfx_name_state())
        {
			app_ble_clear_error_conut();
            set_read_cfx_name_state(0);	
            os_ev_publish_event(MAIN_EV_DATA, EV_DATA_READ_CFX_NAME, 1);
        }
        if(1 == get_read_cfx_bank_rw_state())
        {
			app_ble_clear_error_conut();
            set_read_cfx_bank_rw_state(0);
            os_ev_publish_event(MAIN_EV_DATA, EV_DATA_CFX_BANK_RW, 1);
        }
        if(1 == get_write_cfx_name_state())
        {
			app_ble_clear_error_conut();
            set_write_cfx_name_state(0);
            os_ev_publish_event(MAIN_EV_DATA, EV_DATA_WRITE_CFX_NAME, 1);
        }
        if(1 == app_ble_write_state_get())
        {
			app_ble_clear_error_conut();
            app_ble_write_state_set(0);
            exit_screen_protector();
        }
//		app_ble_get_version();
		g_ble_task_static_size = osThreadGetStackSpace(app_ble_task_id);
		osDelay(5);
	}
}

static void app_ble_clear_error_conut(void)
{
	s_get_version_count = 0;
	s_get_version_index = 0;
}

static void app_ble_get_version(void)
{
	static uint8_t ble_trriger_error_count = 0;
	uint8_t ble_status = 0;
	
	data_center_read_config_data(SYS_CONFIG_BLE_CTRL, &ble_status);
	if(ble_status == 0)
	{
		ble_trriger_error_count = 0;
		s_get_version_count = 0;
		s_get_version_index = 0;
		return;
	}
	if((PAGE_UPDATE == screen_get_act_pid() && screen_get_act_spid() >= SUB_ID_UPDATING) || 
	   (PAGE_WORK_MODE == screen_get_act_pid() && screen_get_act_spid() == SUB_ID_SYNC_UPDATAING))
	{
		ble_trriger_error_count = 0;
		s_get_version_count = 0;
		s_get_version_index = 0;
		return;
	}
	switch(s_get_version_index)
	{
		case 0:
			s_get_version_count++;
			if(s_get_version_count > 1000)
			{
				s_get_version_count = 0;
				s_get_version_index = 1;
			}
		break;
		case 1:
		{
			bool res = 0;
			res = ble_get_version(&s_dev_ble);
			if(res == true)
			{
				ble_trriger_error_count = 0;
				error_functionArray[ble_communications_ec_007](0);
			}
			else
			{
				ble_trriger_error_count++;
			}
			if(ble_trriger_error_count > 7)
			{
				ble_trriger_error_count = 0;
				error_functionArray[ble_communications_ec_007](1);
			}
			if(ble_trriger_error_count > 3)
			{
				set_ble_power(1);
				set_ble_power_status(0);
			}
			if(ble_trriger_error_count > 4)
			{
				set_ble_power(1);
				set_ble_power_status(1);
			}
			s_get_version_index = 0;
		}
		break;
		default:break;
	}
}

/* 蓝牙下拉事件回调函数 */
static void ble_pull_event_cb(uint32_t event_type, uint32_t event_value)
{
	if(event_type == MAIN_EV_BLE)
	{
		if(event_value == OS_EVENT_BLE_RESET)
		{
			start_ble_reset();
		}
		else if(event_value == OS_EVENT_BLE_POWER)
		{
			set_ble_power(1);
            set_ble_power_status(os_ev_get_event_data(event_type, event_value));
		}
	}
}


/* 蓝牙处理下拉事件 */
void ble_deal_pull_event(void)
{
	uint8_t mac[BLE_MAC_LEN] = {0};
	static uint16_t s_ble_reset_time;
	if(get_ble_power())
	{
		app_ble_clear_error_conut();
		set_ble_power(0);
		
		if(get_ble_power_status())
		{
			ble_power_on();
		}
		else
		{
			ble_power_off();
		}
	}
	if(get_ble_reset())
	{
		app_ble_clear_error_conut();
		set_ble_reset_status(BLE_STATE_RESETINGT);
		if(true == ble_node_reset(&s_dev_ble))
		{
			s_ble_reset_time = 0;
			set_ble_reset(0);
			osSemaphoreRelease(os_semaphore_ble_reset);
			set_ble_reset_status(BLE_RESET_STATE_SUCCESS);
		}
		else
		{
			if (s_ble_reset_time++ > 600)
			{
				s_ble_reset_time = 0;
				set_ble_reset(0);
				set_ble_reset_status(BLE_RESET_STATE_FAIL);
			}
		}
		if(true == ble_get_device_mac(&s_dev_ble, NULL, mac))
		{
			for (uint8_t ui = 6; ui < 12; ui++)//MAC ºó6Î»
			{
				if(mac[ui] >= 97 &&  mac[ui] <= 122 )//×ª´óÐ´
				{
					mac[ui] = mac[ui] - 0x20 ;
				}
				else
				{ 
					mac[ui] = mac[ui] - 0 ; 
				}
			}
			data_center_write_config_data(SYS_CONFIG_BLE_SN, &mac[6]);
		}
	}
}


static void app_get_ble_power_info(void)
{
    uint8_t ble_power = 0;
    
    data_center_read_config_data(SYS_CONFIG_BLE_CTRL, &ble_power);
    set_ble_power_status(ble_power);
    set_ble_power(1);
}

static void app_ble_power_on_init(void)
{
	bool res;
	uint8_t mac[BLE_MAC_LEN] = {0};
    uint8_t uuid_read[BLE_UUID_LEN] = {0};
    uint8_t uuid_generate[BLE_UUID_LEN] = {0};
	os_queue_usart_ble_rx = osMessageQueueNew(10, sizeof(ble_rx_packet), NULL);
	os_queue_usart_ble_tx = osMessageQueueNew(25, sizeof(app_ble_tx_packet), NULL);
    os_queue_ble_cfx = osMessageQueueNew(7, 156, NULL);
	os_semaphore_ble_reset = osSemaphoreNew(1, 0, NULL);    // 蓝牙复位
	os_mutex_tx_data = osMutexNew(NULL);  					// 串口发送数据
	os_timer_iot = osTimerNew(os_timer_iot_callback, osTimerPeriodic, NULL, NULL);
	
	s_dev_ble.usart_rx_data = ble_usart_rx_data;
	s_dev_ble.usart_tx_data = ble_usart_tx_data;
	s_dev_ble.usart_user_data = app_ble_usart_user_data;
	
	if(false == ble_power_on_reset(&s_dev_ble))
		return;
//	res = ble_get_version(&s_dev_ble);
//	if(res == true)
//	{
//		error_functionArray[ble_communications_ec_007](0);
//	}
//	else
//	{
//		res = ble_get_version(&s_dev_ble);
//		if(res == false)
//		{
//			res = ble_get_version(&s_dev_ble);
//			if(res == false)
//			{
//				error_functionArray[ble_communications_ec_007](1);
//			}
//		}
//	}
	if(true == ble_get_device_mac(&s_dev_ble, NULL, mac))
	{

		if(true == ble_get_device_uuid(&s_dev_ble, NULL, uuid_read))
		{
			ble_product_uuid_generate(BLE_PRODUCT_UUID, mac, uuid_generate);
			if(0 != memcmp(uuid_generate, uuid_read, BLE_UUID_LEN))
			{
				if(true == ble_set_device_uuid(&s_dev_ble, uuid_generate, BLE_UUID_LEN))
				{
					ble_node_reset(&s_dev_ble);
				}
			}
		}

		for (uint8_t ui = 6; ui < 12; ui++)//MAC ºó6Î»
		{
			if(mac[ui] >= 97 &&  mac[ui] <= 122 )//×ª´óÐ´
			{
				mac[ui] = mac[ui] - 0x20 ;
			}
			else
			{ 
				mac[ui] = mac[ui] - 0 ; 
			}
		}
        data_center_write_config_data(SYS_CONFIG_BLE_SN, &mac[6]);

	}
	
    app_get_ble_power_info();
	
}


static void app_ble_usart_user_data(ble_rx_packet *rx_packet)
{
	uint8_t pack_gatt[10];
	gatt_packet_t *p_gatt;
	uint8_t gatt_ack = 0;
	uint8_t cfx_data[156] = {0};
	uint8_t ui_id,ui;
	uint8_t lenght = 0;
	
	if(0 == strncmp((const char*)&rx_packet->data[0], "+DATA@0001:", strlen("+DATA@0001:")))
	{
		app_ble_clear_error_conut();
		if(true == ble_protocol_packet_is_correct(&rx_packet->data[11], BT_PACKET_SIZE))
		{
			app_ble_proto_parse(&rx_packet->data[11]);
		}
	}
	if(0 == strncmp((const char *)&rx_packet->data[0], "+GATT", strlen("+GATT")))
	{
		app_ble_clear_error_conut();
		p_gatt = (gatt_packet_t *)&rx_packet->data[6];
		
		if(osTimerIsRunning(os_timer_iot))
        {
            osTimerStop(os_timer_iot);     // 停止发送IOT数据
        }
		
		if(0 == api_gatt_packet_check(p_gatt))
		{
			if(p_gatt->head.cmd == GATT_CMD_OTA)
			{
				gatt_ack = app_ble_gatt_ota(&p_gatt->body.ota_body);
				lenght = 2;
				if(p_gatt->body.ota_body.step <= 0x04)
				{
					p_gatt->body.ota_body.step_body.ack = gatt_ack;
				}
				if(p_gatt->body.ota_body.step == 0x05)
				{
					if(p_gatt->body.ota_body.step_body.gatt_ver_t.type == 0)
					{
						lenght += 4;
						p_gatt->body.ota_body.step_body.gatt_ver_t.type = 0;
						p_gatt->body.ota_body.step_body.gatt_ver_t.drv_hw_ver = ui_get_lamp_hw_version();
						p_gatt->body.ota_body.step_body.gatt_ver_t.drv_sw_ver = ui_get_lamp_soft_version();
						p_gatt->body.ota_body.step_body.gatt_ver_t.ctr_hw_ver = ui_get_ctrlbox_hw_version();
						p_gatt->body.ota_body.step_body.gatt_ver_t.ctr_sw_ver = ui_get_ctrlbox_soft_version();
						
					}
					else if(p_gatt->body.ota_body.step_body.gatt_ver_t.type == 1)
					{
						struct sys_info_accessories access_info;
						
						data_center_read_sys_info(SYS_INFO_ACCESS, &access_info);
						p_gatt->body.ota_body.step_body.gatt_ver2_t.type = 1;
						if(access_info.yoke_state == 1 && access_info.fresnel_state == 0)
						{
							lenght += 7;
							memcpy(p_gatt->body.ota_body.step_body.gatt_ver2_t.ver2[0].uuid, YOKE_PRODUCT_UUID, 5);
							p_gatt->body.ota_body.step_body.gatt_ver2_t.ver2[0].ctr_hw_ver = ui_get_motorized_yoke_hw_version();
							p_gatt->body.ota_body.step_body.gatt_ver2_t.ver2[0].ctr_sw_ver = ui_get_motorized_yoke_soft_version();
						}
						else if(access_info.yoke_state == 1 && access_info.fresnel_state == 1)
						{
							for(uint8_t i = 0; i < BLE_MOTORIZED_UPGRADE_MAX; i++)
							{
								lenght += 7;
								i==0?memcpy(p_gatt->body.ota_body.step_body.gatt_ver2_t.ver2[i].uuid, FRESNEL_PRODUCT_UUID, 5):memcpy(p_gatt->body.ota_body.step_body.gatt_ver2_t.ver2[i].uuid, YOKE_PRODUCT_UUID, 5);
								p_gatt->body.ota_body.step_body.gatt_ver2_t.ver2[i].ctr_hw_ver = (i==0?ui_get_motorized_frenel_hw_version():ui_get_motorized_yoke_hw_version());
								p_gatt->body.ota_body.step_body.gatt_ver2_t.ver2[i].ctr_sw_ver = (i==0?ui_get_motorized_frenel_soft_version():ui_get_motorized_yoke_soft_version());
							}
						}
						else if(access_info.yoke_state == 0 && access_info.fresnel_state == 1)
						{
							lenght += 7;
							memcpy(p_gatt->body.ota_body.step_body.gatt_ver2_t.ver2[0].uuid, FRESNEL_PRODUCT_UUID, 5);
							p_gatt->body.ota_body.step_body.gatt_ver2_t.ver2[0].ctr_hw_ver = ui_get_motorized_frenel_hw_version();
							p_gatt->body.ota_body.step_body.gatt_ver2_t.ver2[0].ctr_sw_ver = ui_get_motorized_frenel_soft_version();
						}
					}
				}
				api_gatt_data_pack_up(pack_gatt, (gatt_cmd_enum)p_gatt->head.cmd, (const uint8_t*)&p_gatt->body.ota_body, lenght, p_gatt->head.sequence, p_gatt->head.rw);
				ble_send_gatt_data(&s_dev_ble, pack_gatt, ((gatt_packet_t*)pack_gatt)->head.length);	// 直接发送ack
				if(p_gatt->body.ota_body.step == 4 && gatt_ack == 0 && app_ota_get_upgrade_type())
				{
					osDelay(3000);
					app_ota_mcu_system_reset();
				}
			}
		}
		else
		{
            ui_id = screen_get_act_pid();
            if(ui_id != PAGE_SIDUS_PRO_FX && ui_id != PAGE_UPDATE)
            {
                user_switch_page(PAGE_SIDUS_PRO_FX);
            }
            set_ble_cfx_file_state(1);
            memset(cfx_data, 0, 156);
            memcpy(cfx_data, &rx_packet->data[6], rx_packet->length);

            if(osOK == osMessageQueuePut(os_queue_ble_cfx, cfx_data, 0, 0))
            {
				
            }
            else
            {
                __nop();
            }
		}
	}
}

static void app_ble_deal_send_data(uint8_t ticks)
{
	static uint32_t s_send_ticks = 0;
	app_ble_tx_packet tx_packet;

	if(s_send_ticks)
    {
        s_send_ticks--;
    }
    else
    {
        if(osOK == osMessageQueueGet(os_queue_usart_ble_tx, &tx_packet, NULL, 0))
        {
			app_ble_clear_error_conut();
            s_send_ticks = 100 / ticks;	//100ms 倒计时
            switch(tx_packet.type)
            {
                case BLE_MESH_DATA:
                    ble_send_string(&s_dev_ble, "0001", tx_packet.data, tx_packet.length);
                    break;
                case BLE_GATT_DATA:
                    ble_send_gatt_data(&s_dev_ble, tx_packet.data, tx_packet.length);
                    break;
                default:break;
            }
        }
    }
}




