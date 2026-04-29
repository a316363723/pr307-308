#include "app_ble_port.h"
#include "os_event.h"
#include "app_ble_protocol_parse.h"
#include "dev_ble.h"
#include "SidusProFile.h"

static enum ble_reset_status s_ble_status = BLE_RESET_STATE_IDLE;
static ble_event_collect s_ble_event = {0};
static uint8_t s_ble_curve = 0;
static uint8_t s_ble_cfx_file_state = 0;


uint32_t get_ble_reset_status(void)
{
	return s_ble_status;
}

void set_ble_reset_status(enum ble_reset_status status)
{
	s_ble_status = status;
}

uint8_t get_ble_cfx_file_state(void)
{
	return s_ble_cfx_file_state;
}

void set_ble_cfx_file_state(uint8_t state)
{
	s_ble_cfx_file_state = state;
}


/* 设置蓝牙复位 */
void set_ble_reset(uint8_t flg)
{
	s_ble_event.ble_reset = flg;
}

/* 获取蓝牙复位 */
uint8_t get_ble_reset(void)
{
	return s_ble_event.ble_reset;
}

/* 设置蓝牙电源 */
void set_ble_power(uint8_t flg)
{
	s_ble_event.ble_power = flg;
}

/* 获取蓝牙电源 */
uint32_t get_ble_power(void)
{
	return s_ble_event.ble_power;
}

/* 获取蓝牙电源的状态 */
uint32_t get_ble_power_status(void)
{
	return s_ble_event.ble_power_status;
}

void set_ble_power_status(uint8_t stauts)
{
	s_ble_event.ble_power_status = stauts;
}

/* 设置调光曲线 */
void set_light_curve(uint8_t data)
{
	s_ble_curve = data;
}

/* 获取调光曲线 */
uint8_t* get_light_curve(void)
{
	return &s_ble_curve;
}

void send_ble_motion_live_pack1(uint16_t pan_angle, uint16_t tilt_angle)
{
    uint8_t tx_data[10] = {0};
    BT_Body_TypeDef read_body = {0};
    
    read_body.emotion_live_body.emotion_live_packet1.target_field = 0;
    read_body.emotion_live_body.emotion_live_packet1.packet_index = 0;
    read_body.emotion_live_body.emotion_live_packet1.rotation_laterial = pan_angle;
    read_body.emotion_live_body.emotion_live_packet1.rotation_laterial_max = 360;
    read_body.emotion_live_body.emotion_live_packet1.rotation_laterial_min = 0;
    read_body.emotion_live_body.emotion_live_packet1.rotation_pitch = tilt_angle;
    read_body.emotion_live_body.emotion_live_packet1.rotation_pitch_max = 270;
    read_body.emotion_live_body.emotion_live_packet1.rotation_pitch_min = 0;
    ble_protocol_data_pack_up(tx_data, &read_body, BT_CommandType_Emotion_Live, 0);
    ble_send_data_queue(BLE_MESH_DATA, tx_data, 10);   
}

void send_ble_motion_live_pack2(uint8_t angle)
{
    uint8_t tx_data[10] = {0};
    BT_Body_TypeDef read_body = {0};
    
    if(angle < 15)
        angle = 15;
    read_body.emotion_live_body.emotion_live_packet2.target_field = 0;
    read_body.emotion_live_body.emotion_live_packet2.packet_index = 1;
    read_body.emotion_live_body.emotion_live_packet2.zoom_angle = angle;
    read_body.emotion_live_body.emotion_live_packet2.zoom_blur = 0;
    read_body.emotion_live_body.emotion_live_packet2.cut_up = 0;
    read_body.emotion_live_body.emotion_live_packet2.cut_down = 0;
    read_body.emotion_live_body.emotion_live_packet2.cut_left = 0;
    read_body.emotion_live_body.emotion_live_packet2.cut_right = 0;
    read_body.emotion_live_body.emotion_live_packet2.smoke_wind = 0;
    read_body.emotion_live_body.emotion_live_packet2.smoke_fog = 0;
    read_body.emotion_live_body.emotion_live_packet2.smoke_time = 0;
    read_body.emotion_live_body.emotion_live_packet2.smoke_switch = 0;
    ble_protocol_data_pack_up(tx_data, &read_body, BT_CommandType_Emotion_Live, 0);
    ble_send_data_queue(BLE_MESH_DATA, tx_data, 10);   
}

void send_ble_cfx_bank_rw_pack(uint8_t effect_type, uint16_t bank_rw)
{
    uint8_t tx_data[10] = {0};
    BT_Body_TypeDef read_body = {0};
    
    if(effect_type == 0)
        read_body.CFX_Bank_RW_Body.Effect_Type = 0;
    else if(effect_type == 1)
        read_body.CFX_Bank_RW_Body.Effect_Type = 2;
    else if(effect_type == 2)
        read_body.CFX_Bank_RW_Body.Effect_Type = 1;
    read_body.CFX_Bank_RW_Body.Bank_Info = bank_rw;
    ble_protocol_data_pack_up(tx_data, &read_body, BT_CommandType_CFX_Bank_RW, 0);
    ble_send_data_queue(BLE_MESH_DATA, tx_data, 10);
}

void send_ble_cfx_ctrl_pack(uint8_t effect_type, uint16_t bank)
{
    uint8_t tx_data[10] = {0};
    BT_Body_TypeDef read_body = {0};
    
    read_body.CFX_Ctrl_Body.Effect_Type = effect_type;
    read_body.CFX_Ctrl_Body.Bank = bank;
    ble_protocol_data_pack_up(tx_data, &read_body, BT_CommandType_CFX_Ctrl, 0);
    ble_send_data_queue(BLE_MESH_DATA, tx_data, 10);
}

void send_ble_read_cfx_name_pack(uint8_t *name)
{
    uint8_t tx_data[10] = {0};
    BT_Body_TypeDef read_body = {0};
    
    read_body.CFX_Name_Body.Character_0 = SidusProFile_ASCII2SidusCode(name[0]);
    read_body.CFX_Name_Body.Character_1 = SidusProFile_ASCII2SidusCode(name[1]);
    read_body.CFX_Name_Body.Character_2 = SidusProFile_ASCII2SidusCode(name[2]);
    read_body.CFX_Name_Body.Character_3 = SidusProFile_ASCII2SidusCode(name[3]);
    read_body.CFX_Name_Body.Character_4 = SidusProFile_ASCII2SidusCode(name[4]);
    read_body.CFX_Name_Body.Character_5 = SidusProFile_ASCII2SidusCode(name[5]);
    read_body.CFX_Name_Body.Character_6 = SidusProFile_ASCII2SidusCode(name[6]);
    read_body.CFX_Name_Body.Character_7 = SidusProFile_ASCII2SidusCode(name[7]);
    read_body.CFX_Name_Body.Character_8 = SidusProFile_ASCII2SidusCode(name[8]);
    
    ble_protocol_data_pack_up(tx_data, &read_body, BT_CommandType_CFX_Name, 0);
    ble_send_data_queue(BLE_MESH_DATA, tx_data, 10);
}


/* 控灯事件： 0：表示关灯， 1：表示寻灯 */
void ble_publish_ctr_light_event(uint32_t data)
{
    os_ev_publish_event(MAIN_EV_LIGHT, EV_DATA_LIGHT_TRIGGRT, data); 
	os_ev_publish_event(MAIN_EV_BLE, OS_EVENT_BLE_CTR_LIGHT, data); 
}

void ble_publish_set_light_event(uint32_t data)
{
    os_ev_publish_event(MAIN_EV_LIGHT, EV_DATA_LIGHT_CHANGE, data); 
}

/* 开关机事件： 0：表示关机， 1：表示开机 */
void ble_publish_power_onoff_event(uint32_t data)
{
	os_ev_publish_event(MAIN_EV_BLE, OS_EVENT_BLE_POWER_ON_OFF, data); 
}

int16_t ble_const_convert_symbol(int16_t num)
{
	int16_t data = 0;
	
	if(num >= 0)
	{
		num = num & 0x00ff;
		data = num;
	}
	else
	{
		num = -num;
		data = num | 0x0100;
	}
		
	return data;
}

int16_t ble_symbol_convert_const(int16_t num)
{
	int16_t data = 0;
	
	if((num & 0x0100) == 0x0100)
	{
		num = num & 0x00ff;
		data = -num;
	}
	else
	{
		num = num & 0x00ff;
		data = num;
	}
		
	return data;
}

/* 恢复出厂设置 */
//void ble_publish_factory_reset_event(uint32_t data)
//{
//	os_ev_publish_event(MAIN_EV_BLE, OS_EVENT_BLE_FACTORY_RESET, data); 
//}




