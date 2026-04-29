#include "app_ble_protocol_parse.h"
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include "dev_ble.h"
#include "iot.h"
#include "project_config.h"
#include "local_data.h"
#include "app_data_center.h"
#include "app_power.h"
#include "app_ble_port.h"
#include "user.h"
#include "app_ota.h"
#include "SidusProFile.h"
#include "app_light.h"
#include "dev_fan.h"
#include "base_type.h"
#include "page.h"
#include "version.h"
#include "os_event.h"
#include "ui_data.h"
#include "page_manager.h"
#include "ui_hal.h"

uint16_t* data_convert(uint16_t data);
static uint8_t read_cfx_name_ack = 0;
static uint8_t write_cfx_name_ack = 0;
static uint8_t read_cfx_bank_rw_ack = 0;
static uint8_t ble_write_flag = 0;
extern UI_State_Data_t g_tUIStateData;
void  ble_int_change(uint8_t mode ,uint16_t lightness);

uint8_t get_read_cfx_name_state(void)
{
    return read_cfx_name_ack;
}

void set_read_cfx_name_state(uint8_t state)
{
    read_cfx_name_ack = state;
}

uint8_t get_read_cfx_bank_rw_state(void)
{
    return read_cfx_bank_rw_ack;
}

void set_read_cfx_bank_rw_state(uint8_t state)
{
    read_cfx_bank_rw_ack = state;
}

uint8_t get_write_cfx_name_state(void)
{
    return write_cfx_name_ack;
}

void set_write_cfx_name_state(uint8_t state)
{
    write_cfx_name_ack = state;
}

/*---光效里的子类型---*/
typedef enum{
	
	EffectModeCCT = 0,
	EffectModeHSI,
	EffectModeGEL,
	EffectModeCoord,
	EffectModeSource,
	EffectModeBlack,
	EffectModeRGB,
	EffectModeColorMixing,
	EffectModeNULL,
}Effect_Mode;



//uint8_t SidusProFile_ASCII2SidusCode(char ascii_char)
//{
//    uint8_t  sidus_code = 0;
//    if('_' == ascii_char)
//    {
//        sidus_code = 0x3f;
//    }
//    else if('0' <= ascii_char && ascii_char <= '9')
//    {
//        sidus_code = 1 + ascii_char - '0';
//    }
//    else if('A' <= ascii_char && ascii_char <= 'Z')
//    {
//        sidus_code = 11 + ascii_char - 'A';
//    }
//    else if('a' <= ascii_char && ascii_char <= 'z')
//    {
//        sidus_code = 37 + ascii_char - 'a';
//    }
//    else
//    {
//        sidus_code = 0;//其余解析为空格
//    }
//    return sidus_code;
//}

uint8_t read_ble_effectmodeback( uint8_t effect_type)
{
	uint8_t eid = 0;
	switch(effect_type)
	{
		case PARAM_SETTING_CCT:
			eid =  BT_EffectMode_CCT;
			break;
		case PARAM_SETTING_HSI:
			eid =  BT_EffectMode_HSI;
			break;
		case PARAM_SETTING_GEL:
			eid =  BT_EffectMode_GEL;
		break;	
		case PARAM_SETTING_SOURCE:
			eid =  BT_EffectMode_SOURCE;
		break;
		case PARAM_SETTING_XY:
			eid =  BT_EffectMode_XY;
		break;
	
		default:
			eid  = BT_EffectMode_NULL;
			break;
	}
	return eid ;
}

uint8_t get_ble_effectmodeback( uint8_t effect_type)
{
	uint8_t eid = 0;
	switch(effect_type)
	{
		case BT_EffectMode_CCT:
			eid =  PARAM_SETTING_CCT;
			break;
		case BT_EffectMode_HSI:
			eid =  PARAM_SETTING_HSI;
			break;
		case BT_EffectMode_GEL:	
			eid =  PARAM_SETTING_GEL;
			break;
		case BT_EffectMode_XY:
			eid =  PARAM_SETTING_XY;
		default :
			eid  = PARAM_SETTING_MAX;
			break;
	}
	return eid ;
}

static uint8_t checksum_calculate(const uint8_t* p_data, uint8_t length)
{
    uint8_t checksum = 0;
    while (length--)
    {
        checksum += *p_data++;
    }
    return checksum;
}

bool ble_protocol_packet_is_correct(const uint8_t* p_data, uint8_t length)
{
    if(BT_PACKET_SIZE == length && p_data[0] == checksum_calculate(&p_data[1], length - 1))
    {
        return true;
    }
    else
    {
        return false;
    }
}

void ble_protocol_data_pack_up(uint8_t *p_data, BT_Body_TypeDef *cmd_body, BT_CommandType_TypeDef cmd_type, uint8_t cmd_operate)
{
    if (p_data == NULL || cmd_body == NULL)
    {
        return;
    }
	
    BT_Packet_TypeDef *p_packet = (BT_Packet_TypeDef *)p_data;
    p_packet->Header.Opera_Type = cmd_operate;
    p_packet->Header.Command_Type = cmd_type < BT_CommandType_NULL ? cmd_type : BT_CommandType_NULL;
    memcpy(&p_packet->Body, cmd_body, BT_PACKET_SIZE - 2);
    p_data[0] = checksum_calculate(&p_data[1], BT_PACKET_SIZE - 1);
}





/* ********************************************蓝牙读解析************************************************** */
/* 读自定义光效 */
//static BT_CommandType_TypeDef read_local_siduspro(BT_Body_TypeDef *read_body, light_mode_data_t light_body, enum light_mode mode)
//{
//	BT_CommandType_TypeDef cmd_type = BT_CommandType_NULL;
//	
//	switch(mode)
//	{
//		case LIGHT_MODE_SIDUS_MFX:
//			cmd_type = BT_CommandType_MFX;
//			break;
//		case LIGHT_MODE_SIDUS_PFX:
//			cmd_type = BT_CommandType_CCT;
//			break;
//		case LIGHT_MODE_SIDUS_CFX:
//			cmd_type = BT_CommandType_CFX_Ctrl;
//			break;
//		case LIGHT_MODE_SIDUS_CFX_PREVIEW:
//			cmd_type = BT_CommandType_CFX_Preview;
//			break;
//		case LIGHT_MODE_LOCAL_CFX:
//			//cmd_type = BT_CommandType_MFX;
//			break;
//		
//		default: break;
//	}
//	
//	return cmd_type;
//}

	
	

/* 读分区光效 */
static void read_parse_partition_effect(BT_Body_TypeDef *read_body, enum light_mode mode)
{
    if(mode == LIGHT_MODE_PARTITION_DATA)
    {
        struct sys_info_power power;
        bt_partition_para_arg_t *bt_partition_para;
        bt_partition_para = &read_body->partition_para;
        struct db_partition_data partition_data;
        data_center_read_light_data(mode, &partition_data); 

        data_center_read_sys_info(SYS_INFO_POWER, &power);
        bt_partition_para->ack.cfg.target = 255;
        bt_partition_para->ack.cfg.x = partition_data.partition_num;
        bt_partition_para->ack.cfg.y = 1;
        bt_partition_para->ack.cfg.state = partition_data.state;
        bt_partition_para->ack.cfg.sleep = power.state;
    } 
}

/* 读点控光效 */
static void read_parse_pixel_effect(BT_Body_TypeDef *read_body, enum light_mode mode)
{
	bt_pixel_fx_arg_t *bt_pixel_body;
	bt_pixel_body = &read_body->pixel_fx;
	uint8_t tx_data[10] = {0};
	uint8_t Color_Number = 0;
	uint8_t i = 0;
	
	switch(mode)
	{
		case LIGHT_MODE_PIXEL_FX_COLOR_FADE:
        {
            struct db_color_fade color_fade;
            data_center_read_light_data(mode, &color_fade); 
			bt_pixel_body->color_cut.base.effect_type = BT_PixelFX_Color_Cut;
			bt_pixel_body->color_cut.base.color_number = color_fade.colors;
			Color_Number = color_fade.colors;
			bt_pixel_body->color_cut.base.dir = color_fade.dir;
			bt_pixel_body->color_cut.base.spd = color_fade.speed;
			bt_pixel_body->color_cut.base.state = color_fade.status;
			bt_pixel_body->color_cut.pixel.cct.package_type = 0;		
			ble_protocol_data_pack_up(tx_data, read_body, BT_CommandType_Pixel_Fx, 0);
            ble_send_data_queue(BLE_MESH_DATA, tx_data, 10);  //第1包数据

			for(i=0; i < Color_Number; i++)
			{
				bt_pixel_body->color_cut.pixel.cct.serial = i;
				bt_pixel_body->color_cut.pixel.cct.brightness = color_fade.color_arg[0].lightness;
				bt_pixel_body->color_cut.pixel.cct.package_type = 1;
				if(color_fade.color_arg[i].color_mode == 1)
				{
					bt_pixel_body->color_cut.pixel.cct.light_mode = 1;  //HSI模式
					bt_pixel_body->color_cut.pixel.hsi.sat = color_fade.color_arg[i].color_sel.hsi.sat / 10;
					bt_pixel_body->color_cut.pixel.hsi.cct = color_fade.color_arg[i].color_sel.hsi.cct / 50;   //2000/50=40
					bt_pixel_body->color_cut.pixel.hsi.hue = color_fade.color_arg[i].color_sel.hsi.hue / 10;
				}
				else if(color_fade.color_arg[i].color_mode == 0)
				{
					bt_pixel_body->color_cut.pixel.cct.light_mode = 0;  //CCT模式
					bt_pixel_body->color_cut.pixel.cct.cct = color_fade.color_arg[i].color_sel.cct.cct / 50;
					bt_pixel_body->color_cut.pixel.cct.duv = (color_fade.color_arg[i].color_sel.cct.gm + 100) / 10;
				}
				else
					bt_pixel_body->color_cut.pixel.cct.light_mode = 3;  //Black	
				
				if(i != (Color_Number-1))
				{
					ble_protocol_data_pack_up(tx_data, read_body, BT_CommandType_Pixel_Fx, 0);
					ble_send_data_queue(BLE_MESH_DATA, tx_data, 10);   
				}
			}
			break;
        }
		case LIGHT_MODE_PIXEL_FX_COLOR_CYCLE:
        {
            struct db_color_cycle color_cycle;
            data_center_read_light_data(mode, &color_cycle); 
            bt_pixel_body->color_replace.base.effect_type = BT_PixelFX_Color_Replace;
            bt_pixel_body->color_replace.base.color_number = color_cycle.colors;
            Color_Number = color_cycle.colors;
            bt_pixel_body->color_replace.base.dir = color_cycle.dir;
            bt_pixel_body->color_replace.base.spd = color_cycle.period / 100;
            bt_pixel_body->color_replace.base.color_change_way = color_cycle.color_trans_mode;
            bt_pixel_body->color_replace.base.state = color_cycle.status;
            bt_pixel_body->color_replace.pixel.cct.package_type = 0;		
            ble_protocol_data_pack_up(tx_data, read_body, BT_CommandType_Pixel_Fx, 0);
            ble_send_data_queue(BLE_MESH_DATA, tx_data, 10);   //第1包数据

            for(i=0; i < Color_Number; i++)
            {
                bt_pixel_body->color_replace.pixel.cct.serial = i;
                bt_pixel_body->color_replace.pixel.cct.brightness = color_cycle.color_arg[0].lightness;
                bt_pixel_body->color_replace.pixel.cct.package_type = 1;
            
                if(color_cycle.color_arg[i].color_mode == 1)
                {
                    bt_pixel_body->color_replace.pixel.cct.light_mode = 1;  //HSI模式
                    bt_pixel_body->color_replace.pixel.hsi.sat = color_cycle.color_arg[i].color_sel.hsi.sat / 10;
                    bt_pixel_body->color_replace.pixel.hsi.cct = color_cycle.color_arg[i].color_sel.hsi.cct / 50;   //2000/50=40
                    bt_pixel_body->color_replace.pixel.hsi.hue = color_cycle.color_arg[i].color_sel.hsi.hue / 10;
                }
                else if(color_cycle.color_arg[i].color_mode == 0)
                {
                    bt_pixel_body->color_replace.pixel.cct.light_mode = 0;  //CCT模式
                    bt_pixel_body->color_replace.pixel.cct.cct = color_cycle.color_arg[i].color_sel.cct.cct / 50;
                    bt_pixel_body->color_replace.pixel.cct.duv = (color_cycle.color_arg[i].color_sel.cct.gm + 100) / 10;
                }
                else
                    bt_pixel_body->color_replace.pixel.cct.light_mode = 3;  //Black	
                
                if(i != (Color_Number-1))
                {
                    ble_protocol_data_pack_up(tx_data, read_body, BT_CommandType_Pixel_Fx, 0);
                    ble_send_data_queue(BLE_MESH_DATA, tx_data, 10);   //第1包数据
                }
            }	
			break;
        }
        case LIGHT_MODE_PIXEL_FX_RAINBOW:
		{
            struct db_rainbow rainbow;
            data_center_read_light_data(mode, &rainbow);
            bt_pixel_body->rainbow.effect_type = BT_PixelFX_Rainbow;
            bt_pixel_body->rainbow.brightness = rainbow.lightness;
            bt_pixel_body->rainbow.dir = rainbow.dir;
            bt_pixel_body->rainbow.spd = rainbow.speed;
            bt_pixel_body->rainbow.state = rainbow.status;
			break;
        }
		case LIGHT_MODE_PIXEL_FX_ONE_COLOR_CHASE:
		{
            struct db_one_color_chase one_color_chas;
            data_center_read_light_data(mode, &one_color_chas);
            bt_pixel_body->color_move_I.base.effect_type = BT_PixelFX_Color_Move_I;
            bt_pixel_body->color_move_I.base.package_type = 0;
            bt_pixel_body->color_move_I.base.group = one_color_chas.group;
            Color_Number = one_color_chas.group + 2;
            bt_pixel_body->color_move_I.base.dir = one_color_chas.dir;
            if(one_color_chas.length == 2)
                bt_pixel_body->color_move_I.base.pixel_lenght = 0;
            else if(one_color_chas.length == 4)
                bt_pixel_body->color_move_I.base.pixel_lenght = 1;
            else
                bt_pixel_body->color_move_I.base.pixel_lenght = 2;
            bt_pixel_body->color_move_I.base.spd = one_color_chas.speed * 100;
            bt_pixel_body->color_move_I.base.state = one_color_chas.status;		
            ble_protocol_data_pack_up(tx_data, read_body, BT_CommandType_Pixel_Fx, 0);
            ble_send_data_queue(BLE_MESH_DATA, tx_data, 10);   //第1包数据
            
            for(i=0; i < Color_Number; i++)
            {
                bt_pixel_body->color_move_I.base.package_type = 1;
                bt_pixel_body->color_move_I.pixel.cct.serial = i;  //0底色
                bt_pixel_body->color_move_I.pixel.cct.brightness = one_color_chas.color_arg[i].lightness;
                bt_pixel_body->color_move_I.pixel.cct.light_mode = one_color_chas.color_arg[i].color_mode;
                if(one_color_chas.color_arg[i].color_mode == 0)
                {
                    bt_pixel_body->color_move_I.pixel.cct.cct = one_color_chas.color_arg[i].color_sel.cct.cct / 50;
                    bt_pixel_body->color_move_I.pixel.cct.duv = (one_color_chas.color_arg[i].color_sel.cct.gm + 100) / 10;
                }
                else if(one_color_chas.color_arg[i].color_mode == 1)
                {
                    bt_pixel_body->color_move_I.pixel.hsi.hue = one_color_chas.color_arg[i].color_sel.hsi.hue / 10;
                    bt_pixel_body->color_move_I.pixel.hsi.sat = one_color_chas.color_arg[i].color_sel.hsi.sat / 10;
                    bt_pixel_body->color_move_I.pixel.hsi.cct = one_color_chas.color_arg[i].color_sel.hsi.cct / 50;
                }
                
                if(i != (Color_Number-1))
                {
                    ble_protocol_data_pack_up(tx_data, read_body, BT_CommandType_Pixel_Fx, 0);
                    ble_send_data_queue(BLE_MESH_DATA, tx_data, 10);   //第1包数据  
                }
            }
			break;
        }
		case LIGHT_MODE_PIXEL_FX_TWO_COLOR_CHASE:
		{
            struct db_two_color_chase two_color_chas;
            data_center_read_light_data(mode, &two_color_chas);
            bt_pixel_body->color_move_II.base.effect_type = BT_PixelFX_Color_Move_II;
            bt_pixel_body->color_move_II.base.package_type = 0;
            bt_pixel_body->color_move_II.base.group = two_color_chas.group;  
            if(two_color_chas.group == 0)
                Color_Number = 3;
            else
                Color_Number = 5;  //0底色
            bt_pixel_body->color_move_II.base.dir = two_color_chas.dir;
            if(two_color_chas.length == 2)
                bt_pixel_body->color_move_II.base.pixel_lenght = 0;
            else if(two_color_chas.length == 3)
                bt_pixel_body->color_move_II.base.pixel_lenght = 1;
            else
                bt_pixel_body->color_move_II.base.pixel_lenght = 2;
            bt_pixel_body->color_move_II.base.spd = two_color_chas.speed * 100;
            bt_pixel_body->color_move_II.base.state = two_color_chas.status;		
            ble_protocol_data_pack_up(tx_data, read_body, BT_CommandType_Pixel_Fx, 0);
            ble_send_data_queue(BLE_MESH_DATA, tx_data, 10);   //第1包数据
            
            for(i=0; i < Color_Number; i++)
            {
                bt_pixel_body->color_move_II.base.package_type = 1;
                bt_pixel_body->color_move_II.pixel.cct.serial = i;
                bt_pixel_body->color_move_II.pixel.cct.brightness = two_color_chas.color_arg[i].lightness;
                bt_pixel_body->color_move_II.pixel.cct.light_mode = two_color_chas.color_arg[i].color_mode;
                if(two_color_chas.color_arg[i].color_mode == 0)
                {
                    bt_pixel_body->color_move_II.pixel.cct.cct = two_color_chas.color_arg[i].color_sel.cct.cct / 50;
                    bt_pixel_body->color_move_II.pixel.cct.duv = (two_color_chas.color_arg[i].color_sel.cct.gm + 100) / 10;
                }
                else if(two_color_chas.color_arg[i].color_mode == 1)
                {
                    bt_pixel_body->color_move_II.pixel.hsi.hue = two_color_chas.color_arg[i].color_sel.hsi.hue / 10;
                    bt_pixel_body->color_move_II.pixel.hsi.sat = two_color_chas.color_arg[i].color_sel.hsi.sat / 10;
                    bt_pixel_body->color_move_II.pixel.hsi.cct = two_color_chas.color_arg[i].color_sel.hsi.cct / 50;
                }
                
                if(i != (Color_Number-1))
                {
                    ble_protocol_data_pack_up(tx_data, read_body, BT_CommandType_Pixel_Fx, 0);
                    ble_send_data_queue(BLE_MESH_DATA, tx_data, 10);   //第1包数据  
                }
            }
			break;
        }
		case LIGHT_MODE_PIXEL_FX_THREE_COLOR_CHASE:
		{
            struct db_three_color_chase three_color_ch;
            data_center_read_light_data(mode, &three_color_ch);
            bt_pixel_body->color_move_III.base.effect_type = BT_PixelFX_Color_Move_III;
            bt_pixel_body->color_move_III.base.package_type = 0;
            bt_pixel_body->color_move_III.base.group = three_color_ch.group;
            if(three_color_ch.group == 0)
                Color_Number = 4;
            else
                Color_Number = 7;  //0底色
            bt_pixel_body->color_move_III.base.dir = three_color_ch.dir;
            if(three_color_ch.length == 2)
                bt_pixel_body->color_move_III.base.pixel_lenght = 0;
            else if(three_color_ch.length == 3)
                bt_pixel_body->color_move_III.base.pixel_lenght = 1;
            else
                bt_pixel_body->color_move_III.base.pixel_lenght = 2;				
            bt_pixel_body->color_move_III.base.spd = three_color_ch.speed * 100;
            bt_pixel_body->color_move_III.base.state = three_color_ch.status;		
            ble_protocol_data_pack_up(tx_data, read_body, BT_CommandType_Pixel_Fx, 0);
            ble_send_data_queue(BLE_MESH_DATA, tx_data, 10);   //第1包数据
                
            for(i=0; i < Color_Number; i++)
            {
                bt_pixel_body->color_move_III.base.package_type = 1;
                bt_pixel_body->color_move_III.pixel.cct.serial = i;
                bt_pixel_body->color_move_III.pixel.cct.brightness = three_color_ch.color_arg[i].lightness;
                bt_pixel_body->color_move_III.pixel.cct.light_mode = three_color_ch.color_arg[i].color_mode;
                if(three_color_ch.color_arg[i].color_mode == 0)
                {
                    bt_pixel_body->color_move_III.pixel.cct.cct = three_color_ch.color_arg[i].color_sel.cct.cct / 50;
                    bt_pixel_body->color_move_III.pixel.cct.duv = (three_color_ch.color_arg[i].color_sel.cct.gm + 100) / 10;
                }
                else if(three_color_ch.color_arg[i].color_mode == 1)
                {
                    bt_pixel_body->color_move_III.pixel.hsi.hue = three_color_ch.color_arg[i].color_sel.hsi.hue / 10;
                    bt_pixel_body->color_move_III.pixel.hsi.sat = three_color_ch.color_arg[i].color_sel.hsi.sat / 10;
                    bt_pixel_body->color_move_III.pixel.hsi.cct = three_color_ch.color_arg[i].color_sel.hsi.cct / 50;
                }
                
                if(i != (Color_Number-1))
                {
                    ble_protocol_data_pack_up(tx_data, read_body, BT_CommandType_Pixel_Fx, 0);
                    ble_send_data_queue(BLE_MESH_DATA, tx_data, 10);
                }								
            }
			break;
        }
		
		case LIGHT_MODE_PIXEL_FX_FIRE:
		{
            struct db_pixel_fire pixel_fire;
            data_center_read_light_data(mode, &pixel_fire);
            bt_pixel_body->pixel_fire.pixel.cct.effect_type = BT_PixelFX_Pixel_Fire;
            bt_pixel_body->pixel_fire.pixel.cct.package_type = 0;
            bt_pixel_body->pixel_fire.base.frq = pixel_fire.frq * 10;
            bt_pixel_body->pixel_fire.base.dir = pixel_fire.dir;
            bt_pixel_body->pixel_fire.base.state = pixel_fire.status;
            ble_protocol_data_pack_up(tx_data, read_body, BT_CommandType_Pixel_Fx, 0);
            ble_send_data_queue(BLE_MESH_DATA, tx_data, 10); //第1包数据
    
            bt_pixel_body->pixel_fire.pixel.cct.package_type = 1;
            bt_pixel_body->pixel_fire.pixel.cct.max_brightness = pixel_fire.color_arg.lightness_max;
            bt_pixel_body->pixel_fire.pixel.cct.min_brightness = pixel_fire.color_arg.lightness_min;
            bt_pixel_body->pixel_fire.pixel.cct.light_mode = pixel_fire.color_arg.color_mode;
            if(pixel_fire.color_arg.color_mode == 0)  //cct
            {
                bt_pixel_body->pixel_fire.pixel.cct.cct = pixel_fire.color_arg.color_sel.cct.cct / 50;
                bt_pixel_body->pixel_fire.pixel.cct.duv = (pixel_fire.color_arg.color_sel.cct.gm + 100) / 10;
            }
            else if(pixel_fire.color_arg.color_mode == 1)  //hsi
            {
                bt_pixel_body->pixel_fire.pixel.hsi.hue = pixel_fire.color_arg.color_sel.hsi.hue / 10;
                bt_pixel_body->pixel_fire.pixel.hsi.sat = pixel_fire.color_arg.color_sel.hsi.sat / 10;
                bt_pixel_body->pixel_fire.pixel.hsi.cct = pixel_fire.color_arg.color_sel.hsi.cct / 50;
            }
            ble_protocol_data_pack_up(tx_data, read_body, BT_CommandType_Pixel_Fx, 0);
            ble_send_data_queue(BLE_MESH_DATA, tx_data, 10);   //第1包数据
            
            bt_pixel_body->pixel_fire.pixel.cct.package_type = 2;
            bt_pixel_body->pixel_fire.base_color.cct.brightness = pixel_fire.color_background_arg.lightness;
            bt_pixel_body->pixel_fire.base_color.cct.light_mode = pixel_fire.color_background_arg.color_mode;
            if(pixel_fire.color_background_arg.color_mode == 0)
            {
                bt_pixel_body->pixel_fire.base_color.cct.cct = pixel_fire.color_background_arg.color_sel.cct.cct / 50;
                bt_pixel_body->pixel_fire.base_color.cct.duv = (pixel_fire.color_background_arg.color_sel.cct.gm + 100) / 10;
            }
            else if(pixel_fire.color_background_arg.color_mode == 1)
            {
                bt_pixel_body->pixel_fire.base_color.hsi.hue = pixel_fire.color_background_arg.color_sel.hsi.hue / 10;
                bt_pixel_body->pixel_fire.base_color.hsi.sat = pixel_fire.color_background_arg.color_sel.hsi.sat / 10;
                bt_pixel_body->pixel_fire.base_color.hsi.cct = pixel_fire.color_background_arg.color_sel.hsi.cct / 50;
            }
            break;
        }
		default: break;
	}
	
}

static BT_CommandType_TypeDef read_parse_sidus_pro(BT_Body_TypeDef *read_body, enum light_mode mode)
{
    uint8_t tx_data[10] = {0};
    BT_CommandType_TypeDef cmd_type = BT_CommandType_NULL;
    
    switch(mode)
    {
        case LIGHT_MODE_SIDUS_MFX:
        {
            struct db_manual_fx mfx;
            data_center_read_light_data(mode, &mfx); 
            cmd_type = BT_CommandType_MFX;
            
            if(mfx.base == 0)
            {
                read_body->MFX_Body.Packet_0.BaseCCT.Base = mfx.base;
                read_body->MFX_Body.Packet_0.BaseCCT.Int_Mini = mfx.base_arg.cct_range.int_mimi / 10;
                read_body->MFX_Body.Packet_0.BaseCCT.Int_Max = mfx.base_arg.cct_range.int_max / 10;
                read_body->MFX_Body.Packet_0.BaseCCT.Int_Seq = mfx.base_arg.cct_range.int_seq;
                read_body->MFX_Body.Packet_0.BaseCCT.CCT_Mini = mfx.base_arg.cct_range.cct_mini / 10;
                read_body->MFX_Body.Packet_0.BaseCCT.CCT_Max = mfx.base_arg.cct_range.cct_max / 10;
                read_body->MFX_Body.Packet_0.BaseCCT.CCT_Seq = mfx.base_arg.cct_range.cct_seq;
                read_body->MFX_Body.Packet_0.BaseCCT.GM_Mini = mfx.base_arg.cct_range.gm_mini;
                read_body->MFX_Body.Packet_0.BaseCCT.GM_Max = mfx.base_arg.cct_range.gm_max;
                read_body->MFX_Body.Packet_0.BaseCCT.GM_Seq = mfx.base_arg.cct_range.gm_seq;
            }
            else
            {
                read_body->MFX_Body.Packet_0.BaseCCT.Base = mfx.base;
                read_body->MFX_Body.Packet_0.BaseHSI.Int_Mini = mfx.base_arg.hsi_range.int_mimi / 10;
                read_body->MFX_Body.Packet_0.BaseHSI.Int_Max = mfx.base_arg.hsi_range.int_max / 10;
                read_body->MFX_Body.Packet_0.BaseHSI.Int_Seq = mfx.base_arg.hsi_range.int_seq;
                read_body->MFX_Body.Packet_0.BaseHSI.Hue_Mini = mfx.base_arg.hsi_range.hue_mini;
                read_body->MFX_Body.Packet_0.BaseHSI.Hue_Max = mfx.base_arg.hsi_range.hue_max;
                read_body->MFX_Body.Packet_0.BaseHSI.Hue_Seq = mfx.base_arg.hsi_range.hue_seq;
                read_body->MFX_Body.Packet_0.BaseHSI.Sat_Mini = mfx.base_arg.hsi_range.sat_mini;
                read_body->MFX_Body.Packet_0.BaseHSI.Sat_Max = mfx.base_arg.hsi_range.sat_max;
                read_body->MFX_Body.Packet_0.BaseHSI.Sat_Seq = mfx.base_arg.hsi_range.sat_seq;
            }
            read_body->MFX_Body.Packet_0.BaseCCT.Packet_Num = 0;
            ble_protocol_data_pack_up(tx_data, read_body, cmd_type, 0);
            ble_send_data_queue(BLE_MESH_DATA, tx_data, 10);//第1包数据
            
            
            switch(mfx.fx_arg.mode)
            {
                case SIDUS_MFX_FLASH:
                    read_body->MFX_Body.Packet_1.Flash.Effect_Mode = mfx.fx_arg.mode;
                    read_body->MFX_Body.Packet_1.Flash.FreeTime_Seq = mfx.fx_arg.mode_arg.flash.free_time_seq;
                    read_body->MFX_Body.Packet_1.Flash.FreeTime_Max = mfx.fx_arg.mode_arg.flash.free_time_max / 100;
                    read_body->MFX_Body.Packet_1.Flash.FreeTime_Mini = mfx.fx_arg.mode_arg.flash.free_time_mini / 100;
                    read_body->MFX_Body.Packet_1.Flash.CycleTime_Seq = mfx.fx_arg.mode_arg.flash.cycle_time_seq;
                    read_body->MFX_Body.Packet_1.Flash.CycleTime_Max = mfx.fx_arg.mode_arg.flash.cycle_time_max / 100;
                    read_body->MFX_Body.Packet_1.Flash.CycleTime_Mini = mfx.fx_arg.mode_arg.flash.cycle_time_mini / 100;
                    read_body->MFX_Body.Packet_1.Flash.LoopTimes = mfx.fx_arg.mode_arg.flash.loop_times;
                    read_body->MFX_Body.Packet_1.Flash.LoopMode = mfx.fx_arg.mode_arg.flash.loop_mode;
                    break;

                case SIDUS_MFX_CONTINUE:
                    read_body->MFX_Body.Packet_1.Continue.Effect_Mode = mfx.fx_arg.mode;
                    read_body->MFX_Body.Packet_1.Continue.FadeIn_Curve = mfx.fx_arg.mode_arg.continues.fade_in_curve;
                    read_body->MFX_Body.Packet_1.Continue.FadeInTime_Max = mfx.fx_arg.mode_arg.continues.fade_in_time_max / 100;
                    read_body->MFX_Body.Packet_1.Continue.FadeInTime_Mini = mfx.fx_arg.mode_arg.continues.fade_in_time_mini / 100;
                    read_body->MFX_Body.Packet_1.Continue.FadeInTime_Seq = mfx.fx_arg.mode_arg.continues.fade_in_time_seq;
                    read_body->MFX_Body.Packet_1.Continue.CycleTime_Seq = mfx.fx_arg.mode_arg.continues.cycle_time_seq;
                    read_body->MFX_Body.Packet_1.Continue.CycleTime_Max = mfx.fx_arg.mode_arg.continues.cycle_time_max / 100;
                    read_body->MFX_Body.Packet_1.Continue.CycleTime_Mini = mfx.fx_arg.mode_arg.continues.cycle_time_mini / 100;
                    read_body->MFX_Body.Packet_1.Continue.LoopTimes = mfx.fx_arg.mode_arg.continues.loop_times;
                    read_body->MFX_Body.Packet_1.Continue.LoopMode = mfx.fx_arg.mode_arg.continues.loop_mode;
                    break;

                case SIDUS_MFX_PARAGRAPH:
                    read_body->MFX_Body.Packet_1.Paragraph.Effect_Mode = mfx.fx_arg.mode;
                    read_body->MFX_Body.Packet_1.Paragraph.UnitTime_Seq = mfx.fx_arg.mode_arg.paragraph.unit_time_max;
                    read_body->MFX_Body.Packet_1.Paragraph.FreeTime_Seq = mfx.fx_arg.mode_arg.paragraph.free_time_seq;
                    read_body->MFX_Body.Packet_1.Paragraph.Overlap_Seq = mfx.fx_arg.mode_arg.paragraph.overlap_seq;
                    read_body->MFX_Body.Packet_1.Paragraph.OLR_Seq = mfx.fx_arg.mode_arg.paragraph.olp_seq;
                    read_body->MFX_Body.Packet_1.Paragraph.OLR_Max = mfx.fx_arg.mode_arg.paragraph.olp_max;
                    read_body->MFX_Body.Packet_1.Paragraph.OLR_Mini = mfx.fx_arg.mode_arg.paragraph.olp_mini;
                    read_body->MFX_Body.Packet_1.Paragraph.CycleTime_Seq = mfx.fx_arg.mode_arg.paragraph.cycle_time_seq;
                    read_body->MFX_Body.Packet_1.Paragraph.CycleTime_Max = mfx.fx_arg.mode_arg.paragraph.cycle_time_max / 100;
                    read_body->MFX_Body.Packet_1.Paragraph.CycleTime_Mini = mfx.fx_arg.mode_arg.paragraph.cycle_time_mini / 100;
                    read_body->MFX_Body.Packet_1.Paragraph.LoopTimes = mfx.fx_arg.mode_arg.paragraph.loop_times;
                    read_body->MFX_Body.Packet_1.Paragraph.LoopMode = mfx.fx_arg.mode_arg.paragraph.loop_mode;
                    break;

                default:
                    break;
            }
            read_body->MFX_Body.Packet_0.BaseCCT.Packet_Num = 1;
            ble_protocol_data_pack_up(tx_data, read_body, cmd_type, 0);
            ble_send_data_queue(BLE_MESH_DATA, tx_data, 10);   //第2包数据
            
            
            switch(mfx.fx_arg.mode)
            {
                case SIDUS_MFX_FLASH:
                    read_body->MFX_Body.Packet_2.Flash.Effect_Mode = mfx.fx_arg.mode;
                    read_body->MFX_Body.Packet_2.Flash.Frq_Seq = mfx.fx_arg.mode_arg.flash.frq_seq;
                    read_body->MFX_Body.Packet_2.Flash.Frq_Max = mfx.fx_arg.mode_arg.flash.frq_max;
                    read_body->MFX_Body.Packet_2.Flash.Frq_Mini = mfx.fx_arg.mode_arg.flash.frq_mini;
                    read_body->MFX_Body.Packet_2.Flash.UnitTime_Seq = mfx.fx_arg.mode_arg.flash.unit_time_seq;
                    read_body->MFX_Body.Packet_2.Flash.UnitTime_Max = mfx.fx_arg.mode_arg.flash.unit_time_max / 100;
                    read_body->MFX_Body.Packet_2.Flash.UnitTime_Mini = mfx.fx_arg.mode_arg.flash.unit_time_mini / 100;
                    break;

                case SIDUS_MFX_CONTINUE:
                    read_body->MFX_Body.Packet_2.Continue.Effect_Mode = mfx.fx_arg.mode;
                    read_body->MFX_Body.Packet_2.Continue.Flicker_Frq = mfx.fx_arg.mode_arg.continues.flicker_frq;
                    read_body->MFX_Body.Packet_2.Continue.FadeOut_Curve = mfx.fx_arg.mode_arg.continues.fade_out_curve;
                    read_body->MFX_Body.Packet_2.Continue.FadeOutTime_Seq = mfx.fx_arg.mode_arg.continues.fade_out_time_seq;
                    read_body->MFX_Body.Packet_2.Continue.FadeOutTime_Max = mfx.fx_arg.mode_arg.continues.fade_out_time_max / 100;
                    read_body->MFX_Body.Packet_2.Continue.FadeOutTime_Mini = mfx.fx_arg.mode_arg.continues.fade_out_time_mini / 100;
                    break;

                case SIDUS_MFX_PARAGRAPH:
                    read_body->MFX_Body.Packet_2.Paragraph.Effect_Mode = mfx.fx_arg.mode;
                    read_body->MFX_Body.Packet_2.Paragraph.OLP_Seq = mfx.fx_arg.mode_arg.paragraph.olp_seq;
                    read_body->MFX_Body.Packet_2.Paragraph.OLP_Max = mfx.fx_arg.mode_arg.paragraph.olp_max;
                    read_body->MFX_Body.Packet_2.Paragraph.OLP_Mini = mfx.fx_arg.mode_arg.paragraph.olp_mini;
                    read_body->MFX_Body.Packet_2.Paragraph.UnitTime_Max = mfx.fx_arg.mode_arg.paragraph.unit_time_max / 100;
                    read_body->MFX_Body.Packet_2.Paragraph.UnitTime_Mini = mfx.fx_arg.mode_arg.paragraph.unit_time_mini / 100;
                    read_body->MFX_Body.Packet_2.Paragraph.FreeTime_Max = mfx.fx_arg.mode_arg.paragraph.free_time_max / 100;
                    read_body->MFX_Body.Packet_2.Paragraph.FreeTime_Mini = mfx.fx_arg.mode_arg.paragraph.free_time_mini / 100;
                    break;

                default:
                    break;
            }
            read_body->MFX_Body.Packet_0.BaseCCT.Packet_Num = 2;
            ble_protocol_data_pack_up(tx_data, read_body, cmd_type, 0);
            ble_send_data_queue(BLE_MESH_DATA, tx_data, 10);  //第3包数据
            

            switch(mfx.fx_arg.mode)
            {
                case SIDUS_MFX_FLASH:
                    break;

                case SIDUS_MFX_CONTINUE:
                    break;

                case SIDUS_MFX_PARAGRAPH:
                    read_body->MFX_Body.Packet_3.Paragraph.Effect_Mode = mfx.fx_arg.mode;
                    read_body->MFX_Body.Packet_3.Paragraph.Flicker_Frq = mfx.fx_arg.mode_arg.paragraph.flicker_frq;
                    read_body->MFX_Body.Packet_3.Paragraph.FadeOut_Curve = mfx.fx_arg.mode_arg.paragraph.fade_out_curve;
                    read_body->MFX_Body.Packet_3.Paragraph.FadeOutTime_Seq = mfx.fx_arg.mode_arg.paragraph.fade_out_time_seq;
                    read_body->MFX_Body.Packet_3.Paragraph.FadeOutTime_Max = mfx.fx_arg.mode_arg.paragraph.fade_out_time_max / 100;
                    read_body->MFX_Body.Packet_3.Paragraph.FadeOutTime_Mini = mfx.fx_arg.mode_arg.paragraph.fade_out_time_mini / 100;
                    read_body->MFX_Body.Packet_3.Paragraph.FadeIn_Curve = mfx.fx_arg.mode_arg.paragraph.fade_in_curve;
                    read_body->MFX_Body.Packet_3.Paragraph.FadeInTime_Seq = mfx.fx_arg.mode_arg.paragraph.cycle_time_seq;
                    read_body->MFX_Body.Packet_3.Paragraph.FadeInTime_Max = mfx.fx_arg.mode_arg.paragraph.fade_in_time_max / 100;
                    read_body->MFX_Body.Packet_3.Paragraph.FadeInTime_Mini = mfx.fx_arg.mode_arg.paragraph.fade_in_time_mini / 100;
                    break;

                default:
                    break;
            }
            read_body->MFX_Body.Packet_0.BaseCCT.Packet_Num = 3;
            break;
        }    
        case LIGHT_MODE_SIDUS_PFX:
        {
            struct db_cct cct_arg;
            struct sys_info_power power;
                
            cmd_type = BT_CommandType_CCT;
            data_center_read_sys_info(SYS_INFO_POWER, &power);
            data_center_read_light_data(mode, &cct_arg);
            read_body->CCT_Body.Int = cct_arg.lightness;
            read_body->CCT_Body.CCT = cct_arg.cct.cct / 10;
            read_body->CCT_Body.GM  = 10;
            read_body->CCT_Body.Sleep_Mode = power.state;
            break;
        } 
        case LIGHT_MODE_SIDUS_CFX:
        {
            struct db_custom_fx cfx;
            
            cmd_type = BT_CommandType_CFX_Ctrl;
            data_center_read_light_data(mode, &cfx);
            read_body->CFX_Ctrl_Body.Ctrl = cfx.ctrl;
            read_body->CFX_Ctrl_Body.Effect_Type = cfx.type;
            read_body->CFX_Ctrl_Body.Bank = cfx.bank;
            read_body->CFX_Ctrl_Body.Chaos = cfx.chaos;
            read_body->CFX_Ctrl_Body.Loop = cfx.loop;
            read_body->CFX_Ctrl_Body.Sequence = cfx.sequence;
            read_body->CFX_Ctrl_Body.Int = cfx.lightness;
            read_body->CFX_Ctrl_Body.Speed = cfx.speed;
            break;
        } 
        case LIGHT_MODE_SIDUS_CFX_PREVIEW:
        {
            cmd_type = BT_CommandType_CFX_Preview;
            break;
        } 
        case LIGHT_MODE_LOCAL_CFX:
        {
            //struct db_local_custom_fx local_cfx_arg;
            break;
        }
        
        
        default:break;
    }
    
    return cmd_type;
    
}

/* 读二代光效 */
static void read_parse_effect_II(BT_Body_TypeDef *read_body, enum light_mode mode)
{
	bt_light_effect_II_arg_t *bt_effect2_body = NULL;
	bt_effect2_body = &read_body->light_effect_II;
	uint8_t tx_data[10] = {0};
	uint8_t origin = 0;
	uint8_t type = 0;
	
	switch(mode)
	{
		case LIGHT_MODE_FX_PAPARAZZI_II:
        {
            struct db_fx_paparazzi_2 paparazzi_2;
            data_center_read_light_data(mode, &paparazzi_2); 
			bt_effect2_body->paparazzi_II.base.effect_type = BT_FX_II_Paparazzi_2;
            bt_effect2_body->paparazzi_II.base.state = paparazzi_2.state;
			bt_effect2_body->paparazzi_II.base.package_type = 0;
			bt_effect2_body->paparazzi_II.base.brightness = paparazzi_2.lightness;
			bt_effect2_body->paparazzi_II.base.gap_time = paparazzi_2.gap_time;
			bt_effect2_body->paparazzi_II.base.min_gap_time = paparazzi_2.min_gap_time;
			ble_protocol_data_pack_up(tx_data, read_body, BT_CommandType_Light_Effect_II, 0);
            ble_send_data_queue(BLE_MESH_DATA, tx_data, 10);

			bt_effect2_body->paparazzi_II.base.package_type = 1;
			bt_effect2_body->paparazzi_II.pixel.cct.light_mode = paparazzi_2.mode;
		
			switch(bt_effect2_body->paparazzi_II.pixel.cct.light_mode)
			{
				case BT_FX_II_Mode_CCT:
					bt_effect2_body->paparazzi_II.pixel.cct.cct = paparazzi_2.mode_arg.cct.cct / 50;
					bt_effect2_body->paparazzi_II.pixel.cct.duv = (paparazzi_2.mode_arg.cct.duv + 100) / 10;
					break;
				case BT_FX_II_Mode_HSI:				
					bt_effect2_body->paparazzi_II.pixel.hsi.sat = paparazzi_2.mode_arg.hsi.sat ;
					bt_effect2_body->paparazzi_II.pixel.hsi.hue = paparazzi_2.mode_arg.hsi.hue ;
					bt_effect2_body->paparazzi_II.pixel.hsi.cct = paparazzi_2.mode_arg.hsi.cct / 50;		
					break;
				case BT_FX_II_Mode_GEL:
					bt_effect2_body->paparazzi_II.pixel.gel.origin = paparazzi_2.mode_arg.gel.brand== 0 ? 1 : 0;
					origin = paparazzi_2.mode_arg.gel.brand;
					bt_effect2_body->paparazzi_II.pixel.gel.type = paparazzi_2.mode_arg.gel.type[origin];
					type = paparazzi_2.mode_arg.gel.type[origin];
					bt_effect2_body->paparazzi_II.pixel.gel.color = paparazzi_2.mode_arg.gel.color[origin][type];
					bt_effect2_body->paparazzi_II.pixel.gel.cct = paparazzi_2.mode_arg.gel.cct / 50;
					break;
				case BT_FX_II_Mode_XY:
					bt_effect2_body->paparazzi_II.pixel.xy.x = paparazzi_2.mode_arg.xy.x;
					bt_effect2_body->paparazzi_II.pixel.xy.y = paparazzi_2.mode_arg.xy.y;
					break;
				case BT_FX_II_Mode_SOUYRCE:
					bt_effect2_body->paparazzi_II.pixel.source.type = paparazzi_2.mode_arg.source.type;
					bt_effect2_body->paparazzi_II.pixel.source.x = paparazzi_2.mode_arg.source.x;
					bt_effect2_body->paparazzi_II.pixel.source.y = paparazzi_2.mode_arg.source.y;
					break;
				case BT_FX_II_Mode_RGB:
					bt_effect2_body->paparazzi_II.pixel.rgb.r = paparazzi_2.mode_arg.rgb.r;
					bt_effect2_body->paparazzi_II.pixel.rgb.g = paparazzi_2.mode_arg.rgb.g;
					bt_effect2_body->paparazzi_II.pixel.rgb.b = paparazzi_2.mode_arg.rgb.b;
					break;
				
				default: break;                    
			}  
			break; 
        }
		case LIGHT_MODE_FX_LIGHTNING_II: // 	 
		{
            struct db_fx_lightning_2 lightning_2;
            data_center_read_light_data(mode, &lightning_2); 
			bt_effect2_body->lightning_II.cct.effect_type = BT_FX_II_Lightning_2;
            bt_effect2_body->lightning_II.cct.state = lightning_2.state;
			bt_effect2_body->lightning_II.cct.brightness = lightning_2.lightness;
			bt_effect2_body->lightning_II.cct.spd = lightning_2.spd;
			bt_effect2_body->lightning_II.cct.light_mode = lightning_2.mode;
		
			switch(bt_effect2_body->lightning_II.cct.light_mode)
			{
				case BT_FX_II_Mode_CCT:
					bt_effect2_body->lightning_II.cct.cct = lightning_2.mode_arg.cct.cct / 50;
					bt_effect2_body->lightning_II.cct.duv = (lightning_2.mode_arg.cct.duv + 100) / 10;
					break;
				case BT_FX_II_Mode_HSI:				
					bt_effect2_body->lightning_II.hsi.sat = lightning_2.mode_arg.hsi.sat;
                    bt_effect2_body->lightning_II.hsi.hue = lightning_2.mode_arg.hsi.hue ;
					bt_effect2_body->lightning_II.hsi.cct = lightning_2.mode_arg.hsi.cct / 50;		
					break;
				default: break;                    
			}  
			break; 
        }
	
		case LIGHT_MODE_FX_TV_II:  // 
		{
            struct db_fx_tv_2 tv_2;
            data_center_read_light_data(mode, &tv_2); 
			bt_effect2_body->tv_II.cct.effect_type = BT_FX_II_TV_2;
            bt_effect2_body->tv_II.cct.state = tv_2.state;
			bt_effect2_body->tv_II.cct.brightness = tv_2.lightness;
			bt_effect2_body->tv_II.cct.spd = tv_2.spd;
			bt_effect2_body->tv_II.cct.light_mode = tv_2.mode;
		
			switch(bt_effect2_body->tv_II.cct.light_mode)
			{
				case BT_FX_II_Mode_CCT:
					bt_effect2_body->tv_II.cct.max_cct = tv_2.mode_arg.cct.max_cct / 50;
                    bt_effect2_body->tv_II.cct.min_cct = tv_2.mode_arg.cct.min_cct / 50;
					bt_effect2_body->tv_II.cct.duv = (tv_2.mode_arg.cct.duv + 100) / 10;
					break;
				case BT_FX_II_Mode_HSI:				
					bt_effect2_body->tv_II.hsi.sat = tv_2.mode_arg.hsi.sat ;
					bt_effect2_body->tv_II.hsi.max_hue = tv_2.mode_arg.hsi.max_hue ;
                    bt_effect2_body->tv_II.hsi.min_hue = tv_2.mode_arg.hsi.min_hue ;
					bt_effect2_body->tv_II.hsi.cct = tv_2.mode_arg.hsi.cct / 50;		
					break;
				default: break;                    
			}  
			break;
        }
		 
		case LIGHT_MODE_FX_FIRE_II: // 
        {
            struct db_fx_fire_2 fire_2;
            data_center_read_light_data(mode, &fire_2); 
			bt_effect2_body->fire_II.cct.effect_type = BT_FX_II_Fire_2;
            bt_effect2_body->fire_II.cct.state = fire_2.state;
			bt_effect2_body->fire_II.cct.brightness = fire_2.lightness;
			bt_effect2_body->fire_II.cct.spd = fire_2.spd;
			bt_effect2_body->fire_II.cct.light_mode = fire_2.mode;
		
			switch(bt_effect2_body->fire_II.cct.light_mode)
			{
				case BT_FX_II_Mode_CCT:
					bt_effect2_body->fire_II.cct.max_cct = fire_2.mode_arg.cct.max_cct / 50;
                    bt_effect2_body->fire_II.cct.min_cct = fire_2.mode_arg.cct.min_cct / 50;
					bt_effect2_body->fire_II.cct.duv = (fire_2.mode_arg.cct.duv + 100) / 10;
					break;
				case BT_FX_II_Mode_HSI:				
					bt_effect2_body->fire_II.hsi.sat = fire_2.mode_arg.hsi.sat ;
					bt_effect2_body->fire_II.hsi.max_hue = fire_2.mode_arg.hsi.max_hue ;
                    bt_effect2_body->fire_II.hsi.min_hue = fire_2.mode_arg.hsi.min_hue ;
					bt_effect2_body->fire_II.hsi.cct = fire_2.mode_arg.hsi.cct / 50;		
					break;
				default: break;                    
			} 
			break; 
        }
		
		case LIGHT_MODE_FX_STROBE_II:  // 闪光灯 2
		{
            struct db_fx_strobe_2 strobe_2;
            data_center_read_light_data(mode, &strobe_2); 
			bt_effect2_body->strobe_II.cct.effect_type = BT_FX_II_Strobe_2;
            bt_effect2_body->strobe_II.cct.state = strobe_2.state;
			bt_effect2_body->strobe_II.cct.brightness = strobe_2.lightness;
			bt_effect2_body->strobe_II.cct.spd = strobe_2.spd;
			bt_effect2_body->strobe_II.cct.light_mode = read_ble_effectmodeback(strobe_2.mode);
		
			switch(bt_effect2_body->strobe_II.cct.light_mode)
			{
				case BT_FX_II_Mode_CCT:
					bt_effect2_body->strobe_II.cct.cct = strobe_2.mode_arg.cct.cct / 50;
					bt_effect2_body->strobe_II.cct.duv = (strobe_2.mode_arg.cct.duv + 100) / 10;
					break;
				case BT_FX_II_Mode_HSI:				
					bt_effect2_body->strobe_II.hsi.sat = strobe_2.mode_arg.hsi.sat ;
					bt_effect2_body->strobe_II.hsi.hue = strobe_2.mode_arg.hsi.hue ;
					bt_effect2_body->strobe_II.hsi.cct = strobe_2.mode_arg.hsi.cct / 50;		
					break;
				case BT_FX_II_Mode_GEL:
					bt_effect2_body->strobe_II.gel.origin = strobe_2.mode_arg.gel.brand== 0 ? 1 : 0;
					origin = strobe_2.mode_arg.gel.brand;
					bt_effect2_body->strobe_II.gel.type = strobe_2.mode_arg.gel.type[origin];
					type = strobe_2.mode_arg.gel.type[origin];
					bt_effect2_body->strobe_II.gel.color = strobe_2.mode_arg.gel.color[origin][type];
					bt_effect2_body->strobe_II.gel.cct = strobe_2.mode_arg.gel.cct / 50;
					break;
				case BT_FX_II_Mode_XY:
					bt_effect2_body->strobe_II.xy.x = strobe_2.mode_arg.xy.x;
					bt_effect2_body->strobe_II.xy.y = strobe_2.mode_arg.xy.y;
					break;
				case BT_FX_II_Mode_SOUYRCE:
					bt_effect2_body->strobe_II.source.type = strobe_2.mode_arg.source.type;
					bt_effect2_body->strobe_II.source.x = strobe_2.mode_arg.source.x;
					bt_effect2_body->strobe_II.source.y = strobe_2.mode_arg.source.y;
					break;
				case BT_FX_II_Mode_RGB:
					bt_effect2_body->strobe_II.rgb.r = strobe_2.mode_arg.rgb.r;
					bt_effect2_body->strobe_II.rgb.g = strobe_2.mode_arg.rgb.g;
					bt_effect2_body->strobe_II.rgb.b = strobe_2.mode_arg.rgb.b;
					break;
				
				default: break;                    
			} 
			break; 	
        }
	
		case LIGHT_MODE_FX_EXPLOSION_II:  // 
		{
            struct db_fx_explosion_2 explosion_2;
            data_center_read_light_data(mode, &explosion_2); 
			bt_effect2_body->explosion_II.cct.effect_type = BT_FX_II_Explosion_2;
            bt_effect2_body->explosion_II.cct.state = explosion_2.state;
			bt_effect2_body->explosion_II.cct.brightness = explosion_2.lightness;
			bt_effect2_body->explosion_II.cct.decay = explosion_2.decay;
			bt_effect2_body->explosion_II.cct.light_mode = read_ble_effectmodeback(explosion_2.mode);
		
			switch(bt_effect2_body->explosion_II.cct.light_mode)
			{
				case BT_FX_II_Mode_CCT:
					bt_effect2_body->explosion_II.cct.cct = explosion_2.mode_arg.cct.cct / 50;
					bt_effect2_body->explosion_II.cct.duv = (explosion_2.mode_arg.cct.duv + 100) / 10;
					break;
				case BT_FX_II_Mode_HSI:				
					bt_effect2_body->explosion_II.hsi.sat = explosion_2.mode_arg.hsi.sat ;
					bt_effect2_body->explosion_II.hsi.hue = explosion_2.mode_arg.hsi.hue ;
					bt_effect2_body->explosion_II.hsi.cct = explosion_2.mode_arg.hsi.cct / 50;		
					break;
				case BT_FX_II_Mode_GEL:
					bt_effect2_body->explosion_II.gel.origin = explosion_2.mode_arg.gel.brand== 0 ? 1 : 0;
					origin = explosion_2.mode_arg.gel.brand;
					bt_effect2_body->explosion_II.gel.type = explosion_2.mode_arg.gel.type[origin];
					type = explosion_2.mode_arg.gel.type[origin];
					bt_effect2_body->explosion_II.gel.color = explosion_2.mode_arg.gel.color[origin][type];
					bt_effect2_body->explosion_II.gel.cct = explosion_2.mode_arg.gel.cct / 50;
					break;
				case BT_FX_II_Mode_XY:
					bt_effect2_body->explosion_II.xy.x = explosion_2.mode_arg.xy.x;
					bt_effect2_body->explosion_II.xy.y = explosion_2.mode_arg.xy.y;
					break;
				case BT_FX_II_Mode_SOUYRCE:
					bt_effect2_body->explosion_II.source.type = explosion_2.mode_arg.source.type;
					bt_effect2_body->explosion_II.source.x = explosion_2.mode_arg.source.x;
					bt_effect2_body->explosion_II.source.y = explosion_2.mode_arg.source.y;
					break;
				case BT_FX_II_Mode_RGB:
					bt_effect2_body->explosion_II.rgb.r = explosion_2.mode_arg.rgb.r;
					bt_effect2_body->explosion_II.rgb.g = explosion_2.mode_arg.rgb.g;
					bt_effect2_body->explosion_II.rgb.b = explosion_2.mode_arg.rgb.b;
					break;
				
				default: break;                    
			}  
			break;	
        }
	
		case LIGHT_MODE_FX_FAULT_BULB_II: // 
		{
            struct db_fx_fault_bulb_2 fault_bulb_2;
            data_center_read_light_data(mode, &fault_bulb_2); 
			bt_effect2_body->fault_bulb_II.cct.effect_type = BT_FX_II_Fault_Bulb_2;
            bt_effect2_body->fault_bulb_II.cct.state = fault_bulb_2.state;
			bt_effect2_body->fault_bulb_II.cct.brightness = fault_bulb_2.lightness;
			bt_effect2_body->fault_bulb_II.cct.spd = fault_bulb_2.spd;
			bt_effect2_body->fault_bulb_II.cct.frq = fault_bulb_2.frq;
			bt_effect2_body->fault_bulb_II.cct.light_mode = read_ble_effectmodeback(fault_bulb_2.mode);
		
			switch(bt_effect2_body->fault_bulb_II.cct.light_mode)
			{
				case BT_FX_II_Mode_CCT:
					bt_effect2_body->fault_bulb_II.cct.cct = fault_bulb_2.mode_arg.cct.cct / 50;
					bt_effect2_body->fault_bulb_II.cct.duv = (fault_bulb_2.mode_arg.cct.duv + 100) / 10;
					break;
				case BT_FX_II_Mode_HSI:				
					bt_effect2_body->fault_bulb_II.hsi.sat = fault_bulb_2.mode_arg.hsi.sat ;
					bt_effect2_body->fault_bulb_II.hsi.hue = fault_bulb_2.mode_arg.hsi.hue ;
					bt_effect2_body->fault_bulb_II.hsi.cct = fault_bulb_2.mode_arg.hsi.cct / 50;		
					break;
				case BT_FX_II_Mode_GEL:
					bt_effect2_body->fault_bulb_II.gel.origin = fault_bulb_2.mode_arg.gel.brand== 0 ? 1 : 0;
					origin = fault_bulb_2.mode_arg.gel.brand;
					bt_effect2_body->fault_bulb_II.gel.type = fault_bulb_2.mode_arg.gel.type[origin];
					type = fault_bulb_2.mode_arg.gel.type[origin];
					bt_effect2_body->fault_bulb_II.gel.color = fault_bulb_2.mode_arg.gel.color[origin][type];
					bt_effect2_body->fault_bulb_II.gel.cct = fault_bulb_2.mode_arg.gel.cct / 50;
					break;
				case BT_FX_II_Mode_XY:
					bt_effect2_body->fault_bulb_II.xy.x = fault_bulb_2.mode_arg.xy.x;
					bt_effect2_body->fault_bulb_II.xy.y = fault_bulb_2.mode_arg.xy.y;
					break;
				case BT_FX_II_Mode_SOUYRCE:
					bt_effect2_body->fault_bulb_II.source.type = fault_bulb_2.mode_arg.source.type;
					bt_effect2_body->fault_bulb_II.source.x = fault_bulb_2.mode_arg.source.x;
					bt_effect2_body->fault_bulb_II.source.y = fault_bulb_2.mode_arg.source.y;
					break;
				case BT_FX_II_Mode_RGB:
					bt_effect2_body->fault_bulb_II.rgb.r = fault_bulb_2.mode_arg.rgb.r;
					bt_effect2_body->fault_bulb_II.rgb.g = fault_bulb_2.mode_arg.rgb.g;
					bt_effect2_body->fault_bulb_II.rgb.b = fault_bulb_2.mode_arg.rgb.b;
					break;
				
				default: break;                    
			}  
			break;
        }
		case LIGHT_MODE_FX_PULSING_II:  // 脉搏2
		{
            struct db_fx_pulsing_2 pulsing_2;
            data_center_read_light_data(mode, &pulsing_2); 
			bt_effect2_body->pulsing_II.cct.effect_type = BT_FX_II_Pulsing_2;
            bt_effect2_body->pulsing_II.cct.state = pulsing_2.state;
			bt_effect2_body->pulsing_II.cct.brightness = pulsing_2.lightness;
			bt_effect2_body->pulsing_II.cct.spd = pulsing_2.spd;
			bt_effect2_body->pulsing_II.cct.frq = pulsing_2.frq;
			bt_effect2_body->pulsing_II.cct.light_mode = read_ble_effectmodeback(pulsing_2.mode);
		
			switch(bt_effect2_body->pulsing_II.cct.light_mode)
			{
				case BT_FX_II_Mode_CCT:
					bt_effect2_body->pulsing_II.cct.cct = pulsing_2.mode_arg.cct.cct / 50;
					bt_effect2_body->pulsing_II.cct.duv = (pulsing_2.mode_arg.cct.duv + 100) / 10;
					break;
				case BT_FX_II_Mode_HSI:				
					bt_effect2_body->pulsing_II.hsi.sat = pulsing_2.mode_arg.hsi.sat ;
					bt_effect2_body->pulsing_II.hsi.hue = pulsing_2.mode_arg.hsi.hue ;
					bt_effect2_body->pulsing_II.hsi.cct = pulsing_2.mode_arg.hsi.cct / 50;		
					break;
				case BT_FX_II_Mode_GEL:
					bt_effect2_body->pulsing_II.gel.origin = pulsing_2.mode_arg.gel.brand== 0 ? 1 : 0;
					origin = pulsing_2.mode_arg.gel.brand;
					bt_effect2_body->pulsing_II.gel.type = pulsing_2.mode_arg.gel.type[origin];
					type = pulsing_2.mode_arg.gel.type[origin];
					bt_effect2_body->pulsing_II.gel.color = pulsing_2.mode_arg.gel.color[origin][type];
					bt_effect2_body->pulsing_II.gel.cct = pulsing_2.mode_arg.gel.cct / 50;
					break;
				case BT_FX_II_Mode_XY:
					bt_effect2_body->pulsing_II.xy.x = pulsing_2.mode_arg.xy.x;
					bt_effect2_body->pulsing_II.xy.y = pulsing_2.mode_arg.xy.y;
					break;
				case BT_FX_II_Mode_SOUYRCE:
					bt_effect2_body->pulsing_II.source.type = pulsing_2.mode_arg.source.type;
					bt_effect2_body->pulsing_II.source.x = pulsing_2.mode_arg.source.x;
					bt_effect2_body->pulsing_II.source.y = pulsing_2.mode_arg.source.y;
					break;
				case BT_FX_II_Mode_RGB:
					bt_effect2_body->pulsing_II.rgb.r = pulsing_2.mode_arg.rgb.r;
					bt_effect2_body->pulsing_II.rgb.g = pulsing_2.mode_arg.rgb.g;
					bt_effect2_body->pulsing_II.rgb.b = pulsing_2.mode_arg.rgb.b;
					break;
				
				default: break;                    
			}  
			break;
        }
		case LIGHT_MODE_FX_WELDING_II:  // 
		{
            struct db_fx_welding_2 welding_2;
            data_center_read_light_data(mode, &welding_2); 
			bt_effect2_body->welding_II.base.effect_type = BT_FX_II_Welding_2;
            bt_effect2_body->welding_II.base.state = welding_2.state;
			bt_effect2_body->welding_II.base.package_type = 0;
			bt_effect2_body->welding_II.base.brightness = welding_2.lightness;
			bt_effect2_body->welding_II.base.min_brightness = welding_2.min_lightness;
			bt_effect2_body->welding_II.base.frq = welding_2.frq;
			ble_protocol_data_pack_up(tx_data, read_body, BT_CommandType_Light_Effect_II, 0);
            ble_send_data_queue(BLE_MESH_DATA, tx_data, 10);

			bt_effect2_body->welding_II.base.package_type = 1;
			bt_effect2_body->welding_II.pixel.cct.light_mode = read_ble_effectmodeback(welding_2.mode);
		
			switch(bt_effect2_body->welding_II.pixel.cct.light_mode)
			{
				case BT_FX_II_Mode_CCT:
					bt_effect2_body->welding_II.pixel.cct.cct = welding_2.mode_arg.cct.cct / 50;
					bt_effect2_body->welding_II.pixel.cct.duv = (welding_2.mode_arg.cct.duv + 100) / 10;
					break;
				case BT_FX_II_Mode_HSI:				
					bt_effect2_body->welding_II.pixel.hsi.sat = welding_2.mode_arg.hsi.sat ;
					bt_effect2_body->welding_II.pixel.hsi.hue = welding_2.mode_arg.hsi.hue ;
					bt_effect2_body->welding_II.pixel.hsi.cct = welding_2.mode_arg.hsi.cct / 50;		
					break;
				case BT_FX_II_Mode_GEL:
					bt_effect2_body->welding_II.pixel.gel.origin = welding_2.mode_arg.gel.brand== 0 ? 1 : 0;
					origin = welding_2.mode_arg.gel.brand;
					bt_effect2_body->welding_II.pixel.gel.type = welding_2.mode_arg.gel.type[origin];
					type = welding_2.mode_arg.gel.type[origin];
					bt_effect2_body->welding_II.pixel.gel.color = welding_2.mode_arg.gel.color[origin][type];
					bt_effect2_body->welding_II.pixel.gel.cct = welding_2.mode_arg.gel.cct / 50;
					break;
				case BT_FX_II_Mode_XY:
					bt_effect2_body->welding_II.pixel.xy.x = welding_2.mode_arg.xy.x;
					bt_effect2_body->welding_II.pixel.xy.y = welding_2.mode_arg.xy.y;
					break;
				case BT_FX_II_Mode_SOUYRCE:
					bt_effect2_body->welding_II.pixel.source.type = welding_2.mode_arg.source.type;
					bt_effect2_body->welding_II.pixel.source.x = welding_2.mode_arg.source.x;
					bt_effect2_body->welding_II.pixel.source.y = welding_2.mode_arg.source.y;
					break;
				case BT_FX_II_Mode_RGB:
					bt_effect2_body->welding_II.pixel.rgb.r = welding_2.mode_arg.rgb.r;
					bt_effect2_body->welding_II.pixel.rgb.g = welding_2.mode_arg.rgb.g;
					bt_effect2_body->welding_II.pixel.rgb.b = welding_2.mode_arg.rgb.b;
					break;
				
				default: break;                    
			}  
			break;
        }
		case LIGHT_MODE_FX_COP_CAR_II:  // 
		{
            struct db_fx_cop_car_2 cop_car_2;
            data_center_read_light_data(mode, &cop_car_2); 
			bt_effect2_body->cop_car_II.effect_type = BT_FX_II_Cop_Car_2;
			bt_effect2_body->cop_car_II.state = cop_car_2.state;
			bt_effect2_body->cop_car_II.brightness = cop_car_2.lightness; 
			bt_effect2_body->cop_car_II.frq = cop_car_2.frq;
			bt_effect2_body->cop_car_II.color = cop_car_2.color;
			break;
        }
		case LIGHT_MODE_FX_PARTY_LIGHTS_II:
        {
            struct db_fx_party_lights_2 party_lights_2;
            data_center_read_light_data(mode, &party_lights_2); 
			bt_effect2_body->party_lights_II.effect_type = BT_FX_II_Party_Lights_2;
			bt_effect2_body->party_lights_II.state = party_lights_2.state;
			bt_effect2_body->party_lights_II.brightness = party_lights_2.lightness; 
			bt_effect2_body->party_lights_II.sat = party_lights_2.sat;
			bt_effect2_body->party_lights_II.spd = party_lights_2.spd;
			break;
        }
		case LIGHT_MODE_FX_FIREWORKS_II:
        {
            struct db_fx_fireworks_2 fireworks_2;
            data_center_read_light_data(mode, &fireworks_2); 
			bt_effect2_body->fireworks_II.effect_type = BT_FX_II_Fireworks_2;
            bt_effect2_body->fireworks_II.state = fireworks_2.state;
            bt_effect2_body->fireworks_II.brightness = fireworks_2.lightness;
            bt_effect2_body->fireworks_II.mode = fireworks_2.mode;
            bt_effect2_body->fireworks_II.min_gap_time =  fireworks_2.min_gap_time;
            bt_effect2_body->fireworks_II.gap_time = fireworks_2.gap_time;  
			break;
        }
		case LIGHT_MODE_FX_LIGHTNING_III:
        {
            struct db_fx_lightning_3 lightning_3;
            data_center_read_light_data(mode, &lightning_3); 
			bt_effect2_body->lightning_III.base.effect_type = BT_FX_II_Lightning_3;
			bt_effect2_body->lightning_III.base.state = lightning_3.state;
			bt_effect2_body->lightning_III.base.package_type = 0;
			bt_effect2_body->lightning_III.base.brightness = lightning_3.lightness;
			bt_effect2_body->lightning_III.base.gap_time = lightning_3.gap_time;
			bt_effect2_body->lightning_III.base.min_gap_time = lightning_3.min_gap_time;
			ble_protocol_data_pack_up(tx_data, read_body, BT_CommandType_Light_Effect_II, 0);
            ble_send_data_queue(BLE_MESH_DATA, tx_data, 10); 

			bt_effect2_body->lightning_III.base.package_type = 1;
			bt_effect2_body->lightning_III.pixel.cct.light_mode = (bt_fx_2_mode_enum)lightning_3.mode;
			
			switch(bt_effect2_body->lightning_III.pixel.cct.light_mode)
			{
				case BT_FX_II_Mode_CCT:
					bt_effect2_body->lightning_III.pixel.cct.cct = lightning_3.mode_arg.cct.cct / 50;
					bt_effect2_body->lightning_III.pixel.cct.duv = (lightning_3.mode_arg.cct.duv + 100) / 10;				
					break;
				case BT_FX_II_Mode_HSI:			
					bt_effect2_body->lightning_III.pixel.hsi.sat = lightning_3.mode_arg.hsi.sat ;
					bt_effect2_body->lightning_III.pixel.hsi.hue = lightning_3.mode_arg.hsi.hue ;
					bt_effect2_body->lightning_III.pixel.hsi.cct = lightning_3.mode_arg.hsi.cct / 50;				
					break;
				case BT_FX_II_Mode_GEL:			
					bt_effect2_body->lightning_III.pixel.gel.origin = lightning_3.mode_arg.gel.brand;
					origin = lightning_3.mode_arg.gel.brand;
					bt_effect2_body->lightning_III.pixel.gel.type = lightning_3.mode_arg.gel.type[origin];
					type = lightning_3.mode_arg.gel.type[origin];
					bt_effect2_body->lightning_III.pixel.gel.color = lightning_3.mode_arg.gel.color[origin][type];
					bt_effect2_body->lightning_III.pixel.gel.cct =  lightning_3.mode_arg.gel.cct / 50;				
					break;
				case BT_FX_II_Mode_XY:				
					bt_effect2_body->lightning_III.pixel.xy.x = lightning_3.mode_arg.xy.x;
					bt_effect2_body->lightning_III.pixel.xy.y = lightning_3.mode_arg.xy.y;				
					break;
				case BT_FX_II_Mode_SOUYRCE:				
					bt_effect2_body->lightning_III.pixel.source.type = lightning_3.mode_arg.source.type;
					bt_effect2_body->lightning_III.pixel.source.x = lightning_3.mode_arg.source.x;
					bt_effect2_body->lightning_III.pixel.source.y = lightning_3.mode_arg.source.y;				
					break;
				case BT_FX_II_Mode_RGB:				
					bt_effect2_body->lightning_III.pixel.rgb.r = lightning_3.mode_arg.rgb.r;
					bt_effect2_body->lightning_III.pixel.rgb.g = lightning_3.mode_arg.rgb.g;
					bt_effect2_body->lightning_III.pixel.rgb.b = lightning_3.mode_arg.rgb.b;				
					break;
				
				default: break;
			}  
			break;
        }
		case LIGHT_MODE_FX_TV_III:
        {
            struct db_fx_tv_3 tv_3;
            data_center_read_light_data(mode, &tv_3); 
			bt_effect2_body->tv_III.base.effect_type = BT_FX_II_TV_3;
            bt_effect2_body->tv_III.base.state = tv_3.state;
			bt_effect2_body->tv_III.base.package_type = 0;
			bt_effect2_body->tv_III.base.brightness = tv_3.lightness;
			bt_effect2_body->tv_III.base.min_gap_time = tv_3.min_gap_time;
			bt_effect2_body->tv_III.base.gap_time = tv_3.gap_time;
			ble_protocol_data_pack_up(tx_data, read_body, BT_CommandType_Light_Effect_II, 0);
            ble_send_data_queue(BLE_MESH_DATA, tx_data, 10);
	 
			bt_effect2_body->tv_III.base.package_type = 1;
			bt_effect2_body->tv_III.pixel.cct.light_mode = (bt_fx_2_mode_enum)tv_3.mode;
			switch(bt_effect2_body->tv_III.pixel.cct.light_mode)
			{
				case BT_FX_II_Mode_CCT:
					bt_effect2_body->tv_III.pixel.cct.min_cct = tv_3.mode_arg.cct.min_cct / 10;
					bt_effect2_body->tv_III.pixel.cct.duv = (tv_3.mode_arg.cct.duv + 100) / 10;
					bt_effect2_body->tv_III.pixel.cct.max_cct = tv_3.mode_arg.cct.max_cct / 10;
					break;
				case BT_FX_II_Mode_HSI:
					bt_effect2_body->tv_III.pixel.hsi.max_hue = tv_3.mode_arg.hsi.max_hue ;
					bt_effect2_body->tv_III.pixel.hsi.min_hue = tv_3.mode_arg.hsi.min_hue ;
					bt_effect2_body->tv_III.pixel.hsi.cct = tv_3.mode_arg.hsi.cct / 50;
					bt_effect2_body->tv_III.pixel.hsi.sat = tv_3.mode_arg.hsi.sat ;
					break;
				
				default: break;
			}                  
			break;
        }
		case LIGHT_MODE_FX_FIRE_III:
        {
            struct db_fx_fire_3 fire_3;
            data_center_read_light_data(mode, &fire_3); 
			bt_effect2_body->fire_III.base.effect_type = BT_FX_II_Fire_3;
            bt_effect2_body->fire_III.base.state = fire_3.state;
			bt_effect2_body->fire_III.base.package_type = 0;
			bt_effect2_body->fire_III.base.brightness = fire_3.lightness;
			bt_effect2_body->fire_III.base.frq = fire_3.frq;
			ble_protocol_data_pack_up(tx_data, read_body, BT_CommandType_Light_Effect_II, 0);
            ble_send_data_queue(BLE_MESH_DATA, tx_data, 10); 
            
			bt_effect2_body->fire_III.base.package_type = 1;
			bt_effect2_body->fire_III.pixel.cct.light_mode = (bt_fx_2_mode_enum)fire_3.mode;
			switch(bt_effect2_body->fire_III.pixel.cct.light_mode)
			{
				case BT_FX_II_Mode_CCT:
					bt_effect2_body->fire_III.pixel.cct.min_cct = fire_3.mode_arg.cct.min_cct / 50;
					bt_effect2_body->fire_III.pixel.cct.duv = (fire_3.mode_arg.cct.duv + 100) / 10;
					bt_effect2_body->fire_III.pixel.cct.max_cct = fire_3.mode_arg.cct.max_cct / 50;
					break;
				case BT_FX_II_Mode_HSI:
					bt_effect2_body->fire_III.pixel.hsi.min_hue = fire_3.mode_arg.hsi.min_hue ;
					bt_effect2_body->fire_III.pixel.hsi.max_hue = fire_3.mode_arg.hsi.max_hue ;
					bt_effect2_body->fire_III.pixel.hsi.cct = fire_3.mode_arg.hsi.cct / 50;
					bt_effect2_body->fire_III.pixel.hsi.sat = fire_3.mode_arg.hsi.sat ;
					break;
				
				default: break;
			}                 
			break;
        }
		case LIGHT_MODE_FX_FAULTY_BULB_III:
        {
            struct db_fx_fault_bulb_3 fault_bulb_3;
            data_center_read_light_data(mode, &fault_bulb_3); 
			bt_effect2_body->fault_bulb_III.base.effect_type = BT_FX_II_Faulty_Bulb_3;
            bt_effect2_body->fault_bulb_III.base.state = fault_bulb_3.state;
			bt_effect2_body->fault_bulb_III.base.package_type = 0;
			bt_effect2_body->fault_bulb_III.base.brightness = fault_bulb_3.lightness;
			bt_effect2_body->fault_bulb_III.base.gap_time = fault_bulb_3.gap_time;
			bt_effect2_body->fault_bulb_III.base.min_gap_time = fault_bulb_3.min_gap_time;
			ble_protocol_data_pack_up(tx_data, read_body, BT_CommandType_Light_Effect_II, 0);
            ble_send_data_queue(BLE_MESH_DATA, tx_data, 10);  

			bt_effect2_body->fault_bulb_III.base.package_type = 1;
			bt_effect2_body->fault_bulb_III.pixel.cct.light_mode = (bt_fx_2_mode_enum)fault_bulb_3.mode;
			
			switch(bt_effect2_body->fault_bulb_III.pixel.cct.light_mode)
			{
				case BT_FX_II_Mode_CCT:
					bt_effect2_body->fault_bulb_III.pixel.cct.cct = fault_bulb_3.mode_arg.cct.cct / 50;
					bt_effect2_body->fault_bulb_III.pixel.cct.duv = (fault_bulb_3.mode_arg.cct.duv + 100) / 10;
					break;
				case BT_FX_II_Mode_HSI:
					bt_effect2_body->fault_bulb_III.pixel.hsi.sat = fault_bulb_3.mode_arg.hsi.sat ;
					bt_effect2_body->fault_bulb_III.pixel.hsi.hue = fault_bulb_3.mode_arg.hsi.hue ;
					bt_effect2_body->fault_bulb_III.pixel.hsi.cct = fault_bulb_3.mode_arg.hsi.cct / 50;
					break;
				case BT_FX_II_Mode_GEL:
					bt_effect2_body->fault_bulb_III.pixel.gel.origin = fault_bulb_3.mode_arg.gel.brand;
					origin = fault_bulb_3.mode_arg.gel.brand;
					bt_effect2_body->fault_bulb_III.pixel.gel.type = fault_bulb_3.mode_arg.gel.type[origin];
					type = fault_bulb_3.mode_arg.gel.type[origin];
					bt_effect2_body->fault_bulb_III.pixel.gel.color = fault_bulb_3.mode_arg.gel.color[origin][type];
					bt_effect2_body->fault_bulb_III.pixel.gel.cct = fault_bulb_3.mode_arg.gel.cct / 50;
					break;
				case BT_FX_II_Mode_XY:
					bt_effect2_body->fault_bulb_III.pixel.xy.x = fault_bulb_3.mode_arg.xy.x;
					bt_effect2_body->fault_bulb_III.pixel.xy.y = fault_bulb_3.mode_arg.xy.y;
					break;
				case BT_FX_II_Mode_SOUYRCE:
					bt_effect2_body->fault_bulb_III.pixel.source.type = fault_bulb_3.mode_arg.source.type;
					bt_effect2_body->fault_bulb_III.pixel.source.x = fault_bulb_3.mode_arg.source.x;
					bt_effect2_body->fault_bulb_III.pixel.source.y = fault_bulb_3.mode_arg.source.y;
					break;
				case BT_FX_II_Mode_RGB:
					bt_effect2_body->fault_bulb_III.pixel.rgb.r = fault_bulb_3.mode_arg.rgb.r;
					bt_effect2_body->fault_bulb_III.pixel.rgb.g = fault_bulb_3.mode_arg.rgb.g;
					bt_effect2_body->fault_bulb_III.pixel.rgb.b = fault_bulb_3.mode_arg.rgb.b;
					break;
				
				default: break;
			}                  
			break;
        }
		case LIGHT_MODE_FX_PULSING_III:
        {
            struct db_fx_pulsing_3 pulsing_3;
            data_center_read_light_data(mode, &pulsing_3); 
			bt_effect2_body->pulsing_III.cct.effect_type = BT_FX_II_Pulsing_3;
            bt_effect2_body->pulsing_III.cct.state = pulsing_3.state;
            bt_effect2_body->pulsing_III.cct.brightness = pulsing_3.lightness;
            bt_effect2_body->pulsing_III.cct.frq = pulsing_3.frq;
            bt_effect2_body->pulsing_III.cct.light_mode = (bt_fx_2_mode_enum)pulsing_3.mode;
			
            switch(bt_effect2_body->pulsing_III.cct.light_mode)
            {
                case BT_FX_II_Mode_CCT:
                    bt_effect2_body->pulsing_III.cct.cct = pulsing_3.mode_arg.cct.cct / 50;
                    bt_effect2_body->pulsing_III.cct.duv = (pulsing_3.mode_arg.cct.duv + 100) / 10;
					break;
                case BT_FX_II_Mode_HSI:
                    bt_effect2_body->pulsing_III.hsi.hue = pulsing_3.mode_arg.hsi.hue ;
                    bt_effect2_body->pulsing_III.hsi.sat = pulsing_3.mode_arg.hsi.sat ;
                    bt_effect2_body->pulsing_III.hsi.cct = pulsing_3.mode_arg.hsi.cct / 50;
					break;
                case BT_FX_II_Mode_GEL:
                    bt_effect2_body->pulsing_III.gel.origin = pulsing_3.mode_arg.gel.brand;
					origin = pulsing_3.mode_arg.gel.brand;
                    bt_effect2_body->pulsing_III.gel.type = pulsing_3.mode_arg.gel.type[origin];
					type = pulsing_3.mode_arg.gel.type[origin];
                    bt_effect2_body->pulsing_III.gel.color = pulsing_3.mode_arg.gel.color[origin][type];
                    bt_effect2_body->pulsing_III.gel.cct = pulsing_3.mode_arg.gel.cct / 50;
					break;
                case BT_FX_II_Mode_XY:
                    bt_effect2_body->pulsing_III.xy.x = pulsing_3.mode_arg.xy.x;
                    bt_effect2_body->pulsing_III.xy.y = pulsing_3.mode_arg.xy.y;
					break;
                case BT_FX_II_Mode_SOUYRCE:
                    bt_effect2_body->pulsing_III.source.type = pulsing_3.mode_arg.source.type;
                    bt_effect2_body->pulsing_III.source.x = pulsing_3.mode_arg.source.x;
                    bt_effect2_body->pulsing_III.source.y = pulsing_3.mode_arg.source.y;
					break;
                case BT_FX_II_Mode_RGB:
                    bt_effect2_body->pulsing_III.rgb.r = pulsing_3.mode_arg.rgb.r;
                    bt_effect2_body->pulsing_III.rgb.g = pulsing_3.mode_arg.rgb.g;
                    bt_effect2_body->pulsing_III.rgb.b = pulsing_3.mode_arg.rgb.b;
					break;
				
				default: break;
            }           
			break;
        }
		case LIGHT_MODE_FX_COP_CAR_III:
        {
            struct db_fx_cop_car_3 cop_car_3;
            data_center_read_light_data(mode, &cop_car_3); 
			bt_effect2_body->cop_car_III.effect_type = BT_FX_II_Cop_Car_3;
            bt_effect2_body->cop_car_III.state = cop_car_3.state;
            bt_effect2_body->cop_car_III.brightness = cop_car_3.lightness;
            bt_effect2_body->cop_car_III.color = cop_car_3.color;
            bt_effect2_body->cop_car_III.frq = cop_car_3.frq;
			break;
        }
		default: break;
	}
	
}

/* 读系统光效 */
static void read_parse_sys_effect(BT_Body_TypeDef *read_body, enum light_mode mode)
{
	BT_Effect_Mode_Body_TypeDef *bt_effect_body = NULL;
	bt_effect_body = &read_body->Light_Effect_Body;
	uint8_t Origin = 0;
	uint8_t Type = 0;
	
	switch(mode)
	{
		case LIGHT_MODE_FX_CLUBLIGHTS:
        {
            struct db_fx_club_lights club_lights;
            data_center_read_light_data(mode, &club_lights); 
			bt_effect_body->Club_Lights_Arg.Effect_Type = BT_EffectType_ClubLights;   
            bt_effect_body->Club_Lights_Arg.Frq = club_lights.spd;
            bt_effect_body->Club_Lights_Arg.Int = club_lights.lightness;
            bt_effect_body->Club_Lights_Arg.Color = club_lights.color;
			break;
        }
		case LIGHT_MODE_FX_PAPARAZZI:
        {
            struct db_fx_paparazzi paparazzi;
            data_center_read_light_data(mode, &paparazzi);
			bt_effect_body->Paparazzi_Arg.Effect_Type = BT_EffectType_Paparazzi;			   
            bt_effect_body->Paparazzi_Arg.Frq = paparazzi.frq;
            bt_effect_body->Paparazzi_Arg.CCT = paparazzi.cct / 10;			    // 蓝牙值为320-650
            bt_effect_body->Paparazzi_Arg.Int = paparazzi.lightness;			// 蓝牙值为0-1000
            bt_effect_body->Paparazzi_Arg.GM = (paparazzi.gm + 100) / 10;
			break;
        }
		case LIGHT_MODE_FX_LIGHTNING:
        {
            struct db_fx_lightning lightning;
            data_center_read_light_data(mode, &lightning);
			bt_effect_body->Lightning_Arg.Effect_Type = BT_EffectType_Lightning;
            bt_effect_body->Lightning_Arg.Int = lightning.lightness;
            bt_effect_body->Lightning_Arg.Frq = lightning.frq;
            bt_effect_body->Lightning_Arg.CCT = lightning.cct / 10;
			bt_effect_body->Lightning_Arg.GM = (lightning.gm + 100) / 10;
			bt_effect_body->Lightning_Arg.Speed = lightning.speed;
            bt_effect_body->Lightning_Arg.Trigger = lightning.trigger;
			break;
        }
		case LIGHT_MODE_FX_TV:
        {
            struct db_fx_tv tv;
            data_center_read_light_data(mode, &tv);
			bt_effect_body->TV_Arg.Effect_Type = BT_EffectType_TV;
            bt_effect_body->TV_Arg.CCT = tv.cct;                            // CCT只表示挡位，012对应123档
            bt_effect_body->TV_Arg.Frq = tv.frq;
            bt_effect_body->TV_Arg.Int = tv.lightness;
			break;
        }
		case LIGHT_MODE_FX_CANDLE:
        {
            struct db_fx_candle candle;
            data_center_read_light_data(mode, &candle);
			bt_effect_body->Candle_Arg.Effect_Type = BT_EffectType_Candle;
			bt_effect_body->Candle_Arg.Frq = candle.spd;
			bt_effect_body->Candle_Arg.Int = candle.lightness; 
			bt_effect_body->Candle_Arg.CCT = candle.cct;                    // CCT只表示挡位，012对应123档
			break;
        }
		case LIGHT_MODE_FX_FIRE:
        {
            struct db_fx_fire fire;
            data_center_read_light_data(mode, &fire);
			bt_effect_body->Fire_Arg.Effect_Type = BT_EffectType_Fire;
            bt_effect_body->Fire_Arg.Frq = fire.frq;
			bt_effect_body->Fire_Arg.Int = fire.lightness;  
			bt_effect_body->Fire_Arg.CCT = fire.cct;                        // CCT只表示挡位，012对应123档
			break;
        }
		case LIGHT_MODE_FX_STROBE:
        {
            struct db_fx_strobe strobe;
            data_center_read_light_data(mode, &strobe);
            bt_effect_body->Strobe_Arg.Mode.Effect_CCT_Mode.Effect_Type = BT_EffectType_Strobe;
            switch(read_ble_effectmodeback(strobe.mode))
			{
				case FX_MODE_CCT:
					bt_effect_body->Strobe_Arg.Mode.Effect_CCT_Mode.Effect_Mode = EffectModeCCT;
					bt_effect_body->Strobe_Arg.Mode.Effect_CCT_Mode.Int = strobe.lightness; 
					bt_effect_body->Strobe_Arg.Mode.Effect_CCT_Mode.CCT = strobe.mode_arg.cct.cct / 10;;
					bt_effect_body->Strobe_Arg.Mode.Effect_CCT_Mode.GM = (strobe.mode_arg.cct.duv + 100) / 10;
					bt_effect_body->Strobe_Arg.Mode.Effect_CCT_Mode.Frq = strobe.frq;
					break;
				case FX_MODE_HSI:
					bt_effect_body->Strobe_Arg.Mode.Effect_HSI_Mode.Effect_Mode = EffectModeHSI;
					bt_effect_body->Strobe_Arg.Mode.Effect_HSI_Mode.Int = strobe.lightness; 
					bt_effect_body->Strobe_Arg.Mode.Effect_HSI_Mode.Hue = strobe.mode_arg.hsi.hue ;
					bt_effect_body->Strobe_Arg.Mode.Effect_HSI_Mode.Sat = strobe.mode_arg.hsi.sat ;
					bt_effect_body->Strobe_Arg.Mode.Effect_HSI_Mode.Frq = strobe.frq;
					break;
				case FX_MODE_GEL:
					bt_effect_body->Strobe_Arg.Mode.Effect_Gel_Mode.Effect_Mode = EffectModeGEL;
					bt_effect_body->Strobe_Arg.Mode.Effect_Gel_Mode.Int = strobe.lightness; 
					bt_effect_body->Strobe_Arg.Mode.Effect_Gel_Mode.CCT = strobe.mode_arg.gel.cct / 10;;
					bt_effect_body->Strobe_Arg.Mode.Effect_Gel_Mode.Origin = strobe.mode_arg.gel.brand== 0 ? 1 : 0;
					Origin = strobe.mode_arg.gel.brand;
					bt_effect_body->Strobe_Arg.Mode.Effect_Gel_Mode.Type = strobe.mode_arg.gel.type[Origin];
					Type = strobe.mode_arg.gel.type[Origin];
					bt_effect_body->Strobe_Arg.Mode.Effect_Gel_Mode.Color = strobe.mode_arg.gel.color[Origin][Type];
					bt_effect_body->Strobe_Arg.Mode.Effect_Gel_Mode.Frq = strobe.frq;
					break;
				case FX_MODE_XY:
					bt_effect_body->Strobe_Arg.Mode.Effect_XY_Mode.Effect_Mode = EffectModeCoord;
					bt_effect_body->Strobe_Arg.Mode.Effect_XY_Mode.Int = strobe.lightness; 
					bt_effect_body->Strobe_Arg.Mode.Effect_XY_Mode.Coordinate_x = strobe.mode_arg.xy.x;
					bt_effect_body->Strobe_Arg.Mode.Effect_XY_Mode.Coordinate_y = strobe.mode_arg.xy.y;
					bt_effect_body->Strobe_Arg.Mode.Effect_XY_Mode.Frq = strobe.frq;
					break;
				default: break;
			}
			break;
        }
		case LIGHT_MODE_FX_EXPLOSION:
        {
            struct db_fx_explosion explosion;
            data_center_read_light_data(mode, &explosion);
			bt_effect_body->Explosion_Arg.Mode.Effect_CCT_Mode.Effect_Type = BT_EffectType_Explosion;
            switch(read_ble_effectmodeback(explosion.mode))
			{
				case FX_MODE_CCT:
					bt_effect_body->Explosion_Arg.Mode.Effect_CCT_Mode.Effect_Mode = EffectModeCCT;
					bt_effect_body->Explosion_Arg.Mode.Effect_CCT_Mode.Int = explosion.lightness; 
					bt_effect_body->Explosion_Arg.Mode.Effect_CCT_Mode.CCT = explosion.mode_arg.cct.cct / 10;
					bt_effect_body->Explosion_Arg.Mode.Effect_CCT_Mode.GM = (explosion.mode_arg.cct.duv + 100) / 10;
					bt_effect_body->Explosion_Arg.Mode.Effect_CCT_Mode.Frq = explosion.frq;
					bt_effect_body->Explosion_Arg.Mode.Effect_CCT_Mode.Trigger = explosion.trigger;
					break;
				case FX_MODE_HSI:
					bt_effect_body->Explosion_Arg.Mode.Effect_HSI_Mode.Effect_Mode = EffectModeHSI;
					bt_effect_body->Explosion_Arg.Mode.Effect_HSI_Mode.Int = explosion.lightness; 
					bt_effect_body->Explosion_Arg.Mode.Effect_HSI_Mode.Hue = explosion.mode_arg.hsi.hue ;
					bt_effect_body->Explosion_Arg.Mode.Effect_HSI_Mode.Sat = explosion.mode_arg.hsi.sat ;
					bt_effect_body->Explosion_Arg.Mode.Effect_HSI_Mode.Frq = explosion.frq;
					bt_effect_body->Explosion_Arg.Mode.Effect_HSI_Mode.Trigger = explosion.trigger;
					break;
				case FX_MODE_GEL:
					bt_effect_body->Explosion_Arg.Mode.Effect_Gel_Mode.Effect_Mode = EffectModeGEL;
					bt_effect_body->Explosion_Arg.Mode.Effect_Gel_Mode.Int = explosion.lightness;
					bt_effect_body->Explosion_Arg.Mode.Effect_Gel_Mode.CCT = explosion.mode_arg.gel.cct / 10;
					bt_effect_body->Explosion_Arg.Mode.Effect_Gel_Mode.Origin = explosion.mode_arg.gel.brand== 0 ? 1 : 0;
					Origin = explosion.mode_arg.gel.brand;
					bt_effect_body->Explosion_Arg.Mode.Effect_Gel_Mode.Type = explosion.mode_arg.gel.type[Origin];
					Type = explosion.mode_arg.gel.type[Origin];
					bt_effect_body->Explosion_Arg.Mode.Effect_Gel_Mode.Color = explosion.mode_arg.gel.color[Origin][Type];
					bt_effect_body->Explosion_Arg.Mode.Effect_Gel_Mode.Frq = explosion.frq;
					bt_effect_body->Explosion_Arg.Mode.Effect_Gel_Mode.Trigger = explosion.trigger;
					break;
				case FX_MODE_XY:
					bt_effect_body->Explosion_Arg.Mode.Effect_XY_Mode.Effect_Mode = EffectModeCoord;
					bt_effect_body->Explosion_Arg.Mode.Effect_XY_Mode.Int = explosion.lightness;
					bt_effect_body->Explosion_Arg.Mode.Effect_XY_Mode.Coordinate_x = explosion.mode_arg.xy.x;
					bt_effect_body->Explosion_Arg.Mode.Effect_XY_Mode.Coordinate_y = explosion.mode_arg.xy.y;
					bt_effect_body->Explosion_Arg.Mode.Effect_XY_Mode.Frq = explosion.frq;
					bt_effect_body->Explosion_Arg.Mode.Effect_XY_Mode.Trigger = explosion.trigger;
					break;
				default: break;
			}
			break;
        }
		case LIGHT_MODE_FX_FAULT_BULB:
        {
            struct db_fx_fault_bulb fault_bulb;
            data_center_read_light_data(mode, &fault_bulb);
			bt_effect_body->Fault_Bulb_Arg.Mode.Effect_CCT_Mode.Effect_Type = BT_EffectType_FaultBulb;
            switch(read_ble_effectmodeback(fault_bulb.mode))
			{
				case FX_MODE_CCT:
					bt_effect_body->Fault_Bulb_Arg.Mode.Effect_CCT_Mode.Effect_Mode = EffectModeCCT;
					bt_effect_body->Fault_Bulb_Arg.Mode.Effect_CCT_Mode.Int = fault_bulb.lightness;
					bt_effect_body->Fault_Bulb_Arg.Mode.Effect_CCT_Mode.CCT = fault_bulb.mode_arg.cct.cct / 10;;
					bt_effect_body->Fault_Bulb_Arg.Mode.Effect_CCT_Mode.GM = (fault_bulb.mode_arg.cct.duv + 100) / 10;
					bt_effect_body->Fault_Bulb_Arg.Mode.Effect_CCT_Mode.Frq = fault_bulb.frq;
					bt_effect_body->Fault_Bulb_Arg.Mode.Effect_CCT_Mode.Speed = fault_bulb.speed;
					break;
				case FX_MODE_HSI:
					bt_effect_body->Fault_Bulb_Arg.Mode.Effect_HSI_Mode.Effect_Mode = EffectModeHSI;
					bt_effect_body->Fault_Bulb_Arg.Mode.Effect_HSI_Mode.Int = fault_bulb.lightness;
					bt_effect_body->Fault_Bulb_Arg.Mode.Effect_HSI_Mode.Hue = fault_bulb.mode_arg.hsi.hue;
					bt_effect_body->Fault_Bulb_Arg.Mode.Effect_HSI_Mode.Sat = fault_bulb.mode_arg.hsi.sat;
					bt_effect_body->Fault_Bulb_Arg.Mode.Effect_HSI_Mode.Frq = fault_bulb.frq;
					bt_effect_body->Fault_Bulb_Arg.Mode.Effect_HSI_Mode.Speed = fault_bulb.speed;
					break;
				case FX_MODE_GEL:
					bt_effect_body->Fault_Bulb_Arg.Mode.Effect_Gel_Mode.Effect_Mode = EffectModeGEL;
					bt_effect_body->Fault_Bulb_Arg.Mode.Effect_Gel_Mode.Int = fault_bulb.lightness;
					bt_effect_body->Fault_Bulb_Arg.Mode.Effect_Gel_Mode.CCT = fault_bulb.mode_arg.gel.cct / 10;;
					bt_effect_body->Fault_Bulb_Arg.Mode.Effect_Gel_Mode.Origin = fault_bulb.mode_arg.gel.brand== 0 ? 1 : 0;
					Origin = fault_bulb.mode_arg.gel.brand;
					bt_effect_body->Fault_Bulb_Arg.Mode.Effect_Gel_Mode.Type = fault_bulb.mode_arg.gel.type[Origin];
					Type = fault_bulb.mode_arg.gel.type[Origin];
					bt_effect_body->Fault_Bulb_Arg.Mode.Effect_Gel_Mode.Color = fault_bulb.mode_arg.gel.color[Origin][Type];
					bt_effect_body->Fault_Bulb_Arg.Mode.Effect_Gel_Mode.Frq = fault_bulb.frq;
					bt_effect_body->Fault_Bulb_Arg.Mode.Effect_Gel_Mode.Speed = fault_bulb.speed;
					break;
				case FX_MODE_XY:
					bt_effect_body->Fault_Bulb_Arg.Mode.Effect_XY_Mode.Effect_Mode = EffectModeCoord;
					bt_effect_body->Fault_Bulb_Arg.Mode.Effect_XY_Mode.Int = fault_bulb.lightness;
					bt_effect_body->Fault_Bulb_Arg.Mode.Effect_XY_Mode.Coordinate_x = fault_bulb.mode_arg.xy.x;
					bt_effect_body->Fault_Bulb_Arg.Mode.Effect_XY_Mode.Coordinate_y = fault_bulb.mode_arg.xy.y;
					bt_effect_body->Fault_Bulb_Arg.Mode.Effect_XY_Mode.Frq = fault_bulb.frq;
					bt_effect_body->Fault_Bulb_Arg.Mode.Effect_XY_Mode.Speed = fault_bulb.speed;
					break;
				default: break;
			}
			break;
        }
		case LIGHT_MODE_FX_PULSING:
        {
            struct db_fx_pulsing pulsing;
            data_center_read_light_data(mode, &pulsing);
			bt_effect_body->Pulsing_Arg.Mode.Effect_CCT_Mode.Effect_Type = (BT_EffectType_Pulsing);
            switch(read_ble_effectmodeback(pulsing.mode))
			{
				case FX_MODE_CCT:
					bt_effect_body->Pulsing_Arg.Mode.Effect_CCT_Mode.Effect_Mode = EffectModeCCT;
					bt_effect_body->Pulsing_Arg.Mode.Effect_CCT_Mode.Int = pulsing.lightness; 
					bt_effect_body->Pulsing_Arg.Mode.Effect_CCT_Mode.CCT = pulsing.mode_arg.cct.cct / 10;;
					bt_effect_body->Pulsing_Arg.Mode.Effect_CCT_Mode.GM = (pulsing.mode_arg.cct.duv + 100) / 10;
					bt_effect_body->Pulsing_Arg.Mode.Effect_CCT_Mode.Frq = pulsing.frq;
					bt_effect_body->Pulsing_Arg.Mode.Effect_CCT_Mode.Speed = pulsing.speed;
					break;
				case FX_MODE_HSI:
					bt_effect_body->Pulsing_Arg.Mode.Effect_HSI_Mode.Effect_Mode = EffectModeHSI;
					bt_effect_body->Pulsing_Arg.Mode.Effect_HSI_Mode.Int = pulsing.lightness; 
					bt_effect_body->Pulsing_Arg.Mode.Effect_HSI_Mode.Hue = pulsing.mode_arg.hsi.hue ;
					bt_effect_body->Pulsing_Arg.Mode.Effect_HSI_Mode.Sat = pulsing.mode_arg.hsi.sat ;
					bt_effect_body->Pulsing_Arg.Mode.Effect_HSI_Mode.Frq = pulsing.frq;
					bt_effect_body->Pulsing_Arg.Mode.Effect_HSI_Mode.Speed = pulsing.speed;
					break;
				case FX_MODE_GEL:
					bt_effect_body->Pulsing_Arg.Mode.Effect_Gel_Mode.Effect_Mode = EffectModeGEL;
					bt_effect_body->Pulsing_Arg.Mode.Effect_Gel_Mode.Int = pulsing.lightness;
					bt_effect_body->Pulsing_Arg.Mode.Effect_Gel_Mode.CCT = pulsing.mode_arg.gel.cct / 10;;
					bt_effect_body->Pulsing_Arg.Mode.Effect_Gel_Mode.Origin = pulsing.mode_arg.gel.brand== 0 ? 1 : 0;
					Origin = pulsing.mode_arg.gel.brand;
					bt_effect_body->Pulsing_Arg.Mode.Effect_Gel_Mode.Type = pulsing.mode_arg.gel.type[Origin];
					Type = pulsing.mode_arg.gel.type[Origin];
					bt_effect_body->Pulsing_Arg.Mode.Effect_Gel_Mode.Color = pulsing.mode_arg.gel.color[Origin][Type];
					bt_effect_body->Pulsing_Arg.Mode.Effect_Gel_Mode.Frq = pulsing.frq;
					bt_effect_body->Pulsing_Arg.Mode.Effect_Gel_Mode.Speed = pulsing.speed;
					break;
				case FX_MODE_XY:
					bt_effect_body->Pulsing_Arg.Mode.Effect_XY_Mode.Effect_Mode = EffectModeCoord;
					bt_effect_body->Pulsing_Arg.Mode.Effect_XY_Mode.Int = pulsing.lightness;
					bt_effect_body->Pulsing_Arg.Mode.Effect_XY_Mode.Coordinate_x = pulsing.mode_arg.xy.x;
					bt_effect_body->Pulsing_Arg.Mode.Effect_XY_Mode.Coordinate_y = pulsing.mode_arg.xy.y;
					bt_effect_body->Pulsing_Arg.Mode.Effect_XY_Mode.Frq = pulsing.frq;
					bt_effect_body->Pulsing_Arg.Mode.Effect_XY_Mode.Speed = pulsing.speed;
					break;
				default: break;
			}
			break;
        }
		case LIGHT_MODE_FX_WELDING:
        {
            struct db_fx_welding welding;
            data_center_read_light_data(mode, &welding);
			bt_effect_body->Welding_Arg.Mode.Effect_CCT_Mode.Effect_Type = (BT_EffectType_Welding);
            switch(read_ble_effectmodeback(welding.mode))
			{
				case FX_MODE_CCT:
					bt_effect_body->Welding_Arg.Mode.Effect_CCT_Mode.Effect_Mode = EffectModeCCT;
					bt_effect_body->Welding_Arg.Mode.Effect_CCT_Mode.Int = welding.lightness;  
					bt_effect_body->Welding_Arg.Mode.Effect_CCT_Mode.CCT = welding.mode_arg.cct.cct / 10;;
					bt_effect_body->Welding_Arg.Mode.Effect_CCT_Mode.GM = (welding.mode_arg.cct.duv + 100) / 10;
					bt_effect_body->Welding_Arg.Mode.Effect_CCT_Mode.Frq = welding.frq; 
					bt_effect_body->Welding_Arg.Mode.Effect_CCT_Mode.Min = welding.min;
					break;
				case FX_MODE_HSI:
					bt_effect_body->Welding_Arg.Mode.Effect_HSI_Mode.Effect_Mode = EffectModeHSI;
					bt_effect_body->Welding_Arg.Mode.Effect_HSI_Mode.Int = welding.lightness;  
					bt_effect_body->Welding_Arg.Mode.Effect_HSI_Mode.Hue = welding.mode_arg.hsi.hue ;
					bt_effect_body->Welding_Arg.Mode.Effect_HSI_Mode.Sat = welding.mode_arg.hsi.sat ;
					bt_effect_body->Welding_Arg.Mode.Effect_HSI_Mode.Frq = welding.frq; 
					bt_effect_body->Welding_Arg.Mode.Effect_HSI_Mode.Min = welding.min;
					break;
				case FX_MODE_GEL:
					bt_effect_body->Welding_Arg.Mode.Effect_Gel_Mode.Effect_Mode = EffectModeGEL;
					bt_effect_body->Welding_Arg.Mode.Effect_Gel_Mode.Int = welding.lightness; 
					bt_effect_body->Welding_Arg.Mode.Effect_Gel_Mode.CCT = welding.mode_arg.gel.cct / 10;;
					bt_effect_body->Welding_Arg.Mode.Effect_Gel_Mode.Origin = welding.mode_arg.gel.brand== 0 ? 1 : 0;
					Origin = welding.mode_arg.gel.brand;
					bt_effect_body->Welding_Arg.Mode.Effect_Gel_Mode.Type = welding.mode_arg.gel.type[Origin];
					Type = welding.mode_arg.gel.type[Origin];
					bt_effect_body->Welding_Arg.Mode.Effect_Gel_Mode.Color = welding.mode_arg.gel.color[Origin][Type];
					bt_effect_body->Welding_Arg.Mode.Effect_Gel_Mode.Frq = welding.frq; 
					bt_effect_body->Welding_Arg.Mode.Effect_Gel_Mode.Min = welding.min;
					break;
				case FX_MODE_XY:
					bt_effect_body->Welding_Arg.Mode.Effect_XY_Mode.Effect_Mode = EffectModeCoord;
					bt_effect_body->Welding_Arg.Mode.Effect_XY_Mode.Int = welding.lightness; 
					bt_effect_body->Welding_Arg.Mode.Effect_XY_Mode.Coordinate_x = welding.mode_arg.xy.x;
					bt_effect_body->Welding_Arg.Mode.Effect_XY_Mode.Coordinate_y = welding.mode_arg.xy.y;
					bt_effect_body->Welding_Arg.Mode.Effect_XY_Mode.Frq = welding.frq; 
					bt_effect_body->Welding_Arg.Mode.Effect_XY_Mode.Min = welding.min;
					break;
				default: break;
			}
			break;
        }
		case LIGHT_MODE_FX_COP_CAR:
        {
            struct db_fx_cop_car cop_car;
            data_center_read_light_data(mode, &cop_car);
			bt_effect_body->Cop_Car_Arg.Effect_Type = BT_EffectType_CopCar;
            bt_effect_body->Cop_Car_Arg.Color = cop_car.color;
            bt_effect_body->Cop_Car_Arg.Frq = cop_car.frq;
            bt_effect_body->Cop_Car_Arg.Int = cop_car.lightness;
			break;
        }
		case LIGHT_MODE_FX_COLOR_CHASE:
        {
            struct db_fx_color_chase color_chase;
            data_center_read_light_data(mode, &color_chase);
			bt_effect_body->Color_Chase_Arg.Effect_Type = BT_EffectType_ColorChase;
            bt_effect_body->Color_Chase_Arg.Sat = color_chase.sat ;
            bt_effect_body->Color_Chase_Arg.Frq = color_chase.spd;
            bt_effect_body->Color_Chase_Arg.Int = color_chase.lightness;
			break;
        }
		case LIGHT_MODE_FX_PARTY_LIGHTS:
        {
            struct db_fx_party_lights party_lights;
            data_center_read_light_data(mode, &party_lights);
		    bt_effect_body->Party_Lights_Arg.Effect_Type = BT_EffectType_PartyLights;
            bt_effect_body->Party_Lights_Arg.Sat = party_lights.sat ;
            bt_effect_body->Party_Lights_Arg.Frq = party_lights.spd;
            bt_effect_body->Party_Lights_Arg.Int = party_lights.lightness;
			break;
        }
		case LIGHT_MODE_FX_FIREWORKS:
        {
            struct db_fx_fireworks fireworks;
            data_center_read_light_data(mode, &fireworks);
			bt_effect_body->Fireworks_Arg.Effect_Type = BT_EffectType_Fireworks;
            bt_effect_body->Fireworks_Arg.Type = fireworks.type;
            bt_effect_body->Fireworks_Arg.Frq = fireworks.frq;
            bt_effect_body->Fireworks_Arg.Int = fireworks.lightness;
			break;
        }
		default: break;
	}
	
}

/* 读常规光模式 */
static BT_CommandType_TypeDef read_parse_light_mode(BT_Body_TypeDef *read_body, enum light_mode mode)
{
	BT_CommandType_TypeDef cmd_type = BT_CommandType_NULL;

	switch(mode)
	{
		case LIGHT_MODE_CCT:
        {
            struct db_cct cct_arg;
            data_center_read_light_data(mode, &cct_arg); 
			read_body->CCT_Body.Int = cct_arg.lightness;
			read_body->CCT_Body.CCT = cct_arg.cct.cct / 10;
			read_body->CCT_Body.GM  = (cct_arg.cct.duv + 100) / 10;
			cmd_type = BT_CommandType_CCT;
			break;
        }
		case LIGHT_MODE_HSI:
        {
            struct db_hsi hsi_arg;
            data_center_read_light_data(mode, &hsi_arg); 
			read_body->HSI_Body.Int = hsi_arg.lightness;;
			read_body->HSI_Body.Hue = hsi_arg.hsi.hue / 10;
			read_body->HSI_Body.Sat = hsi_arg.hsi.sat / 10;
			cmd_type = BT_CommandType_HSI;
			break;
        }
		case LIGHT_MODE_GEL:
        {
            struct db_gel gel_arg;
            data_center_read_light_data(mode, &gel_arg); 
			read_body->Gel_Body.Int = gel_arg.lightness;
			read_body->Gel_Body.CCT = gel_arg.gel.cct / 10;
			read_body->Gel_Body.Origin = gel_arg.gel.brand== 0 ? 1 : 0;
			read_body->Gel_Body.Type = gel_arg.gel.type[gel_arg.gel.brand];
			read_body->Gel_Body.Color = gel_arg.gel.color[gel_arg.gel.brand][read_body->Gel_Body.Type];
			cmd_type = BT_CommandType_GEL;
			break;
        }
		case LIGHT_MODE_RGB:
        {
            struct db_rgb rgb_arg;
            data_center_read_light_data(mode, &rgb_arg);
			read_body->RGBWW_Body.Int = rgb_arg.lightness;
			read_body->RGBWW_Body.R_Int = rgb_arg.rgb.r;
			read_body->RGBWW_Body.G_Int = rgb_arg.rgb.g;
			read_body->RGBWW_Body.B_Int = rgb_arg.rgb.b;
			cmd_type = BT_CommandType_RGBWW;
			break;
        }
		case LIGHT_MODE_XY:
        {
            struct db_xy xy_arg;
            data_center_read_light_data(mode, &xy_arg);
			read_body->XY_Coordinate_Body.Int = xy_arg.lightness;
			read_body->XY_Coordinate_Body.Coordinate_X = xy_arg.xy.x;
			read_body->XY_Coordinate_Body.Coordinate_Y = xy_arg.xy.y;
			cmd_type = BT_CommandType_XY_Coordinate;
			break;
        }
		default: break;
	}
	
	return  cmd_type;
}

/* 读解析 */
static BT_CommandType_TypeDef read_light_mode(BT_Body_TypeDef *read_body, enum light_mode mode)
{
	BT_CommandType_TypeDef cmd_type = BT_CommandType_NULL;

	
	if(mode <= LIGHT_MODE_SOURCE)
	{
		cmd_type = read_parse_light_mode(read_body, mode);
	}
	else if(mode <= LIGHT_MODE_FX_FIREWORKS)
	{
		cmd_type = BT_CommandType_Light_Effect;
		read_parse_sys_effect(read_body, mode);
	}
	else if(mode <= LIGHT_MODE_FX_COP_CAR_III)
	{
		cmd_type = BT_CommandType_Light_Effect_II;
		read_parse_effect_II(read_body, mode);
	}
    else if(mode <= LIGHT_MODE_LOCAL_CFX) 
	{
		cmd_type = read_parse_sidus_pro(read_body, mode);
	}
	else if(mode <= LIGHT_MODE_PIXEL_FX_FIRE)
	{
		cmd_type = BT_CommandType_Pixel_Fx;
		read_parse_pixel_effect(read_body, mode);
	}
    else if(mode == LIGHT_MODE_PARTITION_DATA)
	{
		cmd_type = BT_CommandType_Partition_Para;
		read_parse_partition_effect(read_body, mode);
	}
	else
	{
		
	}
	
	return  cmd_type;
}

/* *******************************************蓝牙写解析************************************************** */
/* 写分区光效 */
static void write_parse_partition_effect(BT_Body_TypeDef *write_body, uint8_t type)
{
    static uint8_t partition_num = 0;

    switch(type)
    {
        case BT_CommandType_Partition_Para:
        {
            struct db_partition_data partition_data;
            bt_partition_para_arg_t *bt_partition_para = NULL;
            bt_partition_para = &write_body->partition_para;
            
            partition_data.partition_num = partition_num;
            for(uint8_t i = 0; i < partition_data.partition_num; i++)
            {
                if(bt_partition_para->write.cct.target >> (36-partition_data.partition_num+i) & 0x01)
                {
                    partition_data.light_mode[i] = bt_partition_para->write.cct.light_mode;
                    partition_data.target = bt_partition_para->write.cct.target;
                    partition_data.state = bt_partition_para->write.cct.state;
                    
                    if(partition_data.light_mode[i] == 0) // cct
                    {
                        partition_data.color_arg[i].cct.lightness = bt_partition_para->write.cct.brightness;
                        partition_data.color_arg[i].cct.cct = bt_partition_para->write.cct.cct;
                        partition_data.color_arg[i].cct.gm = bt_partition_para->write.cct.duv;
                    }
                    else // hsi
                    {
                        partition_data.color_arg[i].cct.lightness = bt_partition_para->write.hsi.brightness;
                        partition_data.color_arg[i].hsi.hue = bt_partition_para->write.hsi.hue;
                        partition_data.color_arg[i].hsi.sat = bt_partition_para->write.hsi.sat;
                    }
                }
            }
            data_center_write_light_data(LIGHT_MODE_PARTITION_DATA, &partition_data);
            break;
        }
        
        case BT_CommandType_Partition_Fx:
        {
            struct db_partition_data partition_data;
            bt_partition_fx_arg_t *bt_partition_fx = NULL;
            bt_partition_fx = &write_body->partition_fx;
             
            partition_data.fx_mode_arg.Flicker.fx_arg.lasting_min = bt_partition_fx->flicker.lasting_min;
            partition_data.fx_mode_arg.Flicker.fx_arg.lasting_max = bt_partition_fx->flicker.lasting_max;
            partition_data.fx_mode_arg.Flicker.fx_arg.interval_min = bt_partition_fx->flicker.interval_min;
            partition_data.fx_mode_arg.Flicker.fx_arg.interval_max = bt_partition_fx->flicker.interval_max;
            partition_data.fx_mode_arg.Flicker.fx_arg.frq_min = bt_partition_fx->flicker.frq_min;
            partition_data.fx_mode_arg.Flicker.fx_arg.frq_max = bt_partition_fx->flicker.frq_max;
            partition_data.fx_mode_arg.Flicker.fx_arg.trigger = bt_partition_fx->flicker.trigger;
            if(bt_partition_fx->flicker.mode != 0)
            {
                partition_data.fx_mode = bt_partition_fx->flicker.int_min;
            }
            data_center_write_light_data(LIGHT_MODE_PARTITION_FX, &partition_data);
            break;
        }
        
        case BT_CommandType_Partition_Cfg:
        {
            struct db_partition_cfg partition_cfg;
            bt_partition_cfg_art_t *bt_partition_cfg = NULL;
            bt_partition_cfg = &write_body->partition_cfg;
            
            partition_cfg.xy_mode = bt_partition_cfg->write.xy_mode;
            if(bt_partition_cfg->write.xy_mode == 0)  // 默认分区
            {
                partition_num = PARTITION_DEFAULT_NUM;
            }
            else
            {
                partition_num = PARTITION_MAX_NUM;
            }
            data_center_write_light_data(LIGHT_MODE_PARTITION_CFG, &partition_cfg);
            break;
        }
        default: break;
    } 
}
/* 写点控光效 */
static void write_parse_pixel_effect(BT_Body_TypeDef *write_body)
{
    uint8_t i = 0;
    bt_pixel_fx_arg_t *bt_pixel_fx = NULL;
    bt_pixel_fx = &write_body->pixel_fx;
    
    switch(bt_pixel_fx->color_cut.base.effect_type)
    {
        case BT_PixelFX_Color_Cut:
        {
            struct db_color_fade color_fade;

            if(bt_pixel_fx->color_cut.pixel.cct.package_type == 1)
            {
                i = bt_pixel_fx->color_cut.pixel.cct.serial;

                color_fade.color_arg[i].lightness = bt_pixel_fx->color_cut.pixel.cct.brightness;
                color_fade.color_arg[i].color_mode = bt_pixel_fx->color_cut.pixel.cct.light_mode;
                if(color_fade.color_arg[i].color_mode == 0) //cct
                {
                    color_fade.color_arg[i].color_sel.cct.cct = bt_pixel_fx->color_cut.pixel.cct.cct * 50;
                    color_fade.color_arg[i].color_sel.cct.gm = bt_pixel_fx->color_cut.pixel.cct.duv * 10 - 100;
                }
                else if(color_fade.color_arg[i].color_mode == 1)  //hsi
                {
                    color_fade.color_arg[i].color_sel.hsi.sat = bt_pixel_fx->color_cut.pixel.hsi.sat * 10;
                    color_fade.color_arg[i].color_sel.hsi.cct = bt_pixel_fx->color_cut.pixel.hsi.cct * 50;
                    color_fade.color_arg[i].color_sel.hsi.hue = bt_pixel_fx->color_cut.pixel.hsi.hue * 10;
                }
            }
            else
            {
                color_fade.colors = bt_pixel_fx->color_cut.base.color_number;
                color_fade.dir = bt_pixel_fx->color_cut.base.dir;
                color_fade.speed = bt_pixel_fx->color_cut.base.spd; 
            }
            color_fade.status = (enum pixel_run_status)bt_pixel_fx->color_cut.base.state;
            if(bt_pixel_fx->color_cut.base.state != 3)  //连续发数据包
                data_center_write_light_data(LIGHT_MODE_PIXEL_FX_COLOR_FADE, &color_fade);
            break;
        }
        
        case BT_PixelFX_Color_Replace:
        {
            struct db_color_cycle color_cycle;
            
            if(bt_pixel_fx->color_replace.pixel.cct.package_type == 1)
            {
              i = bt_pixel_fx->color_replace.pixel.cct.serial;
                
                color_cycle.color_arg[i].lightness = bt_pixel_fx->color_replace.pixel.cct.brightness;
                color_cycle.color_arg[i].color_mode = bt_pixel_fx->color_replace.pixel.cct.light_mode;
                if(color_cycle.color_arg[i].color_mode == 0) //cct
                {
                    color_cycle.color_arg[i].color_sel.cct.cct = bt_pixel_fx->color_replace.pixel.cct.cct * 50;
                    color_cycle.color_arg[i].color_sel.cct.gm = bt_pixel_fx->color_replace.pixel.cct.duv * 10 - 100;
                }
                else if(color_cycle.color_arg[i].color_mode == 1)  //hsi
                {
                    color_cycle.color_arg[i].color_sel.hsi.sat = bt_pixel_fx->color_replace.pixel.hsi.sat * 10;
                    color_cycle.color_arg[i].color_sel.hsi.cct = bt_pixel_fx->color_replace.pixel.hsi.cct * 50;
                    color_cycle.color_arg[i].color_sel.hsi.hue = bt_pixel_fx->color_replace.pixel.hsi.hue * 10;
                }			
            }
            else
            {
                color_cycle.colors = bt_pixel_fx->color_replace.base.color_number;
                color_cycle.dir = bt_pixel_fx->color_replace.base.dir;
                color_cycle.period = bt_pixel_fx->color_replace.base.spd*100;  //ble(1-100) 单位：ms
                color_cycle.color_trans_mode = bt_pixel_fx->color_replace.base.color_change_way;
            }
            color_cycle.status = (enum pixel_run_status)bt_pixel_fx->color_replace.base.state;
            if(bt_pixel_fx->color_replace.base.state != 3)
                data_center_write_light_data(LIGHT_MODE_PIXEL_FX_COLOR_CYCLE, &color_cycle);
            break;
        }
        case BT_PixelFX_Rainbow:
        {
            struct db_rainbow rainbow;

            rainbow.lightness = bt_pixel_fx->rainbow.brightness;
            rainbow.dir = bt_pixel_fx->rainbow.dir;
            rainbow.speed = bt_pixel_fx->rainbow.spd;
            rainbow.status = (enum pixel_run_status)bt_pixel_fx->rainbow.state;
            data_center_write_light_data(LIGHT_MODE_PIXEL_FX_RAINBOW, &rainbow);
            break;
        }

        case BT_PixelFX_Color_Move_I:
        {
            struct db_one_color_chase one_color_chas;
            if(bt_pixel_fx->color_move_I.pixel.cct.package_type == 1)
            {
                i = bt_pixel_fx->color_move_I.pixel.cct.serial;  //0底色
                
                one_color_chas.color_arg[i].lightness = bt_pixel_fx->color_move_I.pixel.cct.brightness;
                one_color_chas.color_arg[i].color_mode = bt_pixel_fx->color_move_I.pixel.cct.light_mode;
                if(one_color_chas.color_arg[i].color_mode == 0)
                {
                    one_color_chas.color_arg[i].color_sel.cct.cct = bt_pixel_fx->color_move_I.pixel.cct.cct * 50;
                    one_color_chas.color_arg[i].color_sel.cct.gm = bt_pixel_fx->color_move_I.pixel.cct.duv * 10 - 100;
                }
                else if(one_color_chas.color_arg[i].color_mode == 1)
                {
                    one_color_chas.color_arg[i].color_sel.hsi.hue = bt_pixel_fx->color_move_I.pixel.hsi.hue * 10;
                    one_color_chas.color_arg[i].color_sel.hsi.sat = bt_pixel_fx->color_move_I.pixel.hsi.sat * 10;
                    one_color_chas.color_arg[i].color_sel.hsi.cct = bt_pixel_fx->color_move_I.pixel.hsi.cct * 50;
                }
            }
            else
            {
                one_color_chas.group = (enum pixel_groud_unit)bt_pixel_fx->color_move_I.base.group;
                one_color_chas.dir = bt_pixel_fx->color_move_I.base.dir;
                if(bt_pixel_fx->color_move_I.base.pixel_lenght == 0)
                    one_color_chas.length = 2; 
                else if(bt_pixel_fx->color_move_I.base.pixel_lenght == 1)
                    one_color_chas.length = 4; 
                else
                    one_color_chas.length = 9; 
                one_color_chas.speed = (float)bt_pixel_fx->color_move_I.base.spd / 100.0f;
            }
            one_color_chas.status = (enum pixel_run_status)bt_pixel_fx->color_move_I.base.state;
            if(bt_pixel_fx->color_move_I.base.state != 3)
                data_center_write_light_data(LIGHT_MODE_PIXEL_FX_ONE_COLOR_CHASE, &one_color_chas);
            break;
        }
        case BT_PixelFX_Color_Move_II:
        {
            struct db_two_color_chase two_color_chas;
            if(bt_pixel_fx->color_move_II.pixel.cct.package_type == 1)
            {
                i = bt_pixel_fx->color_move_II.pixel.cct.serial;  //0底色
                
                two_color_chas.color_arg[i].lightness = bt_pixel_fx->color_move_II.pixel.cct.brightness;
                two_color_chas.color_arg[i].color_mode = bt_pixel_fx->color_move_II.pixel.cct.light_mode;
                if(two_color_chas.color_arg[i].color_mode == 0)
                {
                    two_color_chas.color_arg[i].color_sel.cct.cct = bt_pixel_fx->color_move_II.pixel.cct.cct * 50;
                    two_color_chas.color_arg[i].color_sel.cct.gm = bt_pixel_fx->color_move_II.pixel.cct.duv * 10 - 100;
                }
                else if(two_color_chas.color_arg[i].color_mode == 1)
                {
                    two_color_chas.color_arg[i].color_sel.hsi.hue = bt_pixel_fx->color_move_II.pixel.hsi.hue * 10;
                    two_color_chas.color_arg[i].color_sel.hsi.sat = bt_pixel_fx->color_move_II.pixel.hsi.sat * 10;
                    two_color_chas.color_arg[i].color_sel.hsi.cct = bt_pixel_fx->color_move_II.pixel.hsi.cct * 50;
                }
                else
                {
                    two_color_chas.color_arg[i].color_sel.cct.cct = 1900;  //特殊处理，某个灯珠设为黑色再开始，不会进入校验
                }
            }
            else
            {
                two_color_chas.group = (enum pixel_groud_unit)bt_pixel_fx->color_move_II.base.group;
                two_color_chas.dir = bt_pixel_fx->color_move_II.base.dir;
                if(bt_pixel_fx->color_move_II.base.pixel_lenght == 0)
                    two_color_chas.length = 2; 
                else if(bt_pixel_fx->color_move_II.base.pixel_lenght == 1)
                    two_color_chas.length = 3; 
                else
                    two_color_chas.length = 6; 
                two_color_chas.speed = (float)bt_pixel_fx->color_move_II.base.spd / 100.0f;
            }
            two_color_chas.status = (enum pixel_run_status)bt_pixel_fx->color_move_II.base.state;
            if(bt_pixel_fx->color_move_II.base.state != 3)
                data_center_write_light_data(LIGHT_MODE_PIXEL_FX_TWO_COLOR_CHASE, &two_color_chas);
            break;
        }
        case BT_PixelFX_Color_Move_III:
        {
            struct db_three_color_chase three_color_ch;
            if(bt_pixel_fx->color_move_III.pixel.cct.package_type == 1)
            {
                i = bt_pixel_fx->color_move_III.pixel.cct.serial;  //0底色
                
                three_color_ch.color_arg[i].lightness = bt_pixel_fx->color_move_III.pixel.cct.brightness;
                three_color_ch.color_arg[i].color_mode = bt_pixel_fx->color_move_III.pixel.cct.light_mode;
                if(three_color_ch.color_arg[i].color_mode == 0)
                {
                    three_color_ch.color_arg[i].color_sel.cct.cct = bt_pixel_fx->color_move_III.pixel.cct.cct * 50;
                    three_color_ch.color_arg[i].color_sel.cct.gm = bt_pixel_fx->color_move_III.pixel.cct.duv * 10 - 100;
                }
                else if(three_color_ch.color_arg[i].color_mode == 1)
                {
                    three_color_ch.color_arg[i].color_sel.hsi.hue = bt_pixel_fx->color_move_III.pixel.hsi.hue * 10;
                    three_color_ch.color_arg[i].color_sel.hsi.sat = bt_pixel_fx->color_move_III.pixel.hsi.sat * 10;
                    three_color_ch.color_arg[i].color_sel.hsi.cct = bt_pixel_fx->color_move_III.pixel.hsi.cct * 50;
                }
                else
                {
                    three_color_ch.color_arg[i].color_sel.cct.cct = 1900;
                }
            }
            else
            {
                three_color_ch.group = (enum pixel_groud_unit)bt_pixel_fx->color_move_III.base.group;
                three_color_ch.dir = bt_pixel_fx->color_move_III.base.dir;
                if(bt_pixel_fx->color_move_III.base.pixel_lenght == 0)
                    three_color_ch.length = 2; 
                else if(bt_pixel_fx->color_move_III.base.pixel_lenght == 1)
                    three_color_ch.length = 3; 
                else
                    three_color_ch.length = 6; 
                three_color_ch.speed = (float)bt_pixel_fx->color_move_III.base.spd / 100.0f;
            }
            three_color_ch.status = (enum pixel_run_status)bt_pixel_fx->color_move_III.base.state;
            if(bt_pixel_fx->color_move_III.base.state != 3)
                data_center_write_light_data(LIGHT_MODE_PIXEL_FX_THREE_COLOR_CHASE, &three_color_ch);
            break;
        }
        case BT_PixelFX_Pixel_Fire:
        {
            struct db_pixel_fire pixel_fire;
            if(bt_pixel_fx->pixel_fire.pixel.cct.package_type == 1)
            {
                pixel_fire.color_arg.lightness_max = bt_pixel_fx->pixel_fire.pixel.cct.max_brightness;
                pixel_fire.color_arg.lightness_min = bt_pixel_fx->pixel_fire.pixel.cct.min_brightness;
                pixel_fire.color_arg.color_mode = bt_pixel_fx->pixel_fire.pixel.cct.light_mode;
                if(pixel_fire.color_arg.color_mode == 0)  //cct
                {
                    pixel_fire.color_arg.color_sel.cct.cct = bt_pixel_fx->pixel_fire.pixel.cct.cct * 50;
                    pixel_fire.color_arg.color_sel.cct.gm = bt_pixel_fx->pixel_fire.pixel.cct.duv * 10 - 100;
                }
                else if(pixel_fire.color_arg.color_mode == 1)  //hsi
                {
                    pixel_fire.color_arg.color_sel.hsi.hue = bt_pixel_fx->pixel_fire.pixel.hsi.hue * 10;
                    pixel_fire.color_arg.color_sel.hsi.sat = bt_pixel_fx->pixel_fire.pixel.hsi.sat * 10;
                    pixel_fire.color_arg.color_sel.hsi.cct = bt_pixel_fx->pixel_fire.pixel.hsi.cct * 50;
                }
                    
            }
            else if(bt_pixel_fx->pixel_fire.pixel.cct.package_type == 2)
            {
                pixel_fire.color_background_arg.lightness = bt_pixel_fx->pixel_fire.base_color.cct.brightness;
                pixel_fire.color_background_arg.color_mode = bt_pixel_fx->pixel_fire.base_color.cct.light_mode;
                if(pixel_fire.color_background_arg.color_mode == 0)
                {
                     pixel_fire.color_background_arg.color_sel.cct.cct = bt_pixel_fx->pixel_fire.base_color.cct.cct * 50;
                     pixel_fire.color_background_arg.color_sel.cct.gm = bt_pixel_fx->pixel_fire.base_color.cct.duv * 10 - 100;
                }
                else if(pixel_fire.color_background_arg.color_mode == 1)
                {
                    pixel_fire.color_background_arg.color_sel.hsi.hue = bt_pixel_fx->pixel_fire.base_color.hsi.hue * 10;
                    pixel_fire.color_background_arg.color_sel.hsi.sat = bt_pixel_fx->pixel_fire.base_color.hsi.sat * 10;
                    pixel_fire.color_background_arg.color_sel.hsi.cct = bt_pixel_fx->pixel_fire.base_color.hsi.cct * 50;
                }
            }
            else
            {
                pixel_fire.frq = (float)bt_pixel_fx->pixel_fire.base.frq / 10.0f;
                pixel_fire.dir = bt_pixel_fx->pixel_fire.base.dir;
            }
            pixel_fire.status = (enum pixel_run_status)bt_pixel_fx->pixel_fire.base.state;
            if(bt_pixel_fx->pixel_fire.base.state != 3)
                data_center_write_light_data(LIGHT_MODE_PIXEL_FX_FIRE, &pixel_fire);
            break;
        }
/*        case BT_PixelFX_Color_Move_M:
        {
            struct db_one_color_chase color_fade;
            
            break;
        }*/
        
        
        
        default: break;
    }  
}

//char SidusProFile_SidusCode2ASCII(uint8_t sidus_char)
//{
//    char ascii_char = 0;
//    if(sidus_char < 1)
//    {
//        ascii_char = ' ';
//    }
//    else if(sidus_char < 11)
//    {
//        ascii_char = '0' + sidus_char - 1;
//    }
//    else if(sidus_char < 37)
//    {
//        ascii_char = 'A' + sidus_char - 11;
//    }
//    else if(sidus_char < 63)
//    {
//        ascii_char = 'a' + sidus_char - 37;
//    }
//    else if(sidus_char < 64)
//    {
//        ascii_char = '_';
//    }
//    else
//    {
//        ascii_char = '\0';
//    }
//    return ascii_char;
//}
//static CFX_Info_Type CFX_Info[3];
//bool SidusProFile_Set_Info_Name(uint8_t type, uint8_t bank, const char* name)
//{
//    uint8_t ret = false;
//    if(type < 3 && bank < 10 && '\0' != name[0])
//    {
//        memcpy(CFX_Info[type].Name[bank], name,9);
//        ret = true;
//    }
//	else
//	{
//		memcpy(CFX_Info[type].Name[bank],  "NO CFX" ,9);
//	}
//    return ret;
//}

static void write_parse_sidus_pro(BT_Body_TypeDef *write_body, uint8_t type)
{
    switch(type)
    {
        case BT_CommandType_PFX_Flash:
        {
            struct db_program_fx pfx;
            BT_PFX_Flash_Body_TypeDef *bt_pfx_flash_body = NULL;
            
            bt_pfx_flash_body = &write_body->PFX_Flash_Body;
            if(bt_pfx_flash_body->Flash_CCT.Base_Type == 0)
            {
                pfx.mode_arg.flash.base = bt_pfx_flash_body->Flash_CCT.Base_Type;
                pfx.mode_arg.flash.fx_arg.times = bt_pfx_flash_body->Flash_CCT.Times;
                pfx.mode_arg.flash.fx_arg.frq = bt_pfx_flash_body->Flash_CCT.Frq;
                pfx.mode_arg.flash.base_arg.cct.lightness = bt_pfx_flash_body->Flash_CCT.Int;
                pfx.mode_arg.flash.base_arg.cct.cct = bt_pfx_flash_body->Flash_CCT.CCT * 10;
                pfx.mode_arg.flash.base_arg.cct.gm = (bt_pfx_flash_body->Flash_CCT.GM- 10) * 10;
            }
            else
            {
                pfx.mode_arg.flash.base = bt_pfx_flash_body->Flash_HSI.Base_Type;
                pfx.mode_arg.flash.fx_arg.times = bt_pfx_flash_body->Flash_HSI.Times;
                pfx.mode_arg.flash.fx_arg.frq = bt_pfx_flash_body->Flash_HSI.Frq;
                pfx.mode_arg.flash.base_arg.hsi.lightness = bt_pfx_flash_body->Flash_HSI.Int;
                pfx.mode_arg.flash.base_arg.hsi.hue = bt_pfx_flash_body->Flash_HSI.Hue;
                pfx.mode_arg.flash.base_arg.hsi.sat = bt_pfx_flash_body->Flash_HSI.Sat;
            }
            pfx.mode = SIDUS_PFX_FLASH;
            
            data_center_write_light_data(LIGHT_MODE_SIDUS_PFX, &pfx);
        }
        break;          
        case BT_CommandType_PFX_Chase:
        {
            struct db_program_fx pfx;
            BT_PFX_Chase_Body_TypeDef *bt_pfx_chase_body = NULL;
            
            bt_pfx_chase_body = &write_body->PFX_Chase_Body;
            if(bt_pfx_chase_body->Chase_CCT.Base_Type == 0)
            {
                pfx.mode_arg.chase.base = bt_pfx_chase_body->Chase_CCT.Base_Type;
                pfx.mode_arg.chase.base_arg.cct_range.cct_max = bt_pfx_chase_body->Chase_CCT.CCT_Max * 100;
                pfx.mode_arg.chase.base_arg.cct_range.cct_mini = bt_pfx_chase_body->Chase_CCT.CCT_Mini * 100;
                pfx.mode_arg.chase.base_arg.cct_range.cct_seq  = bt_pfx_chase_body->Chase_CCT.CCT_Seq;
                pfx.mode_arg.chase.base_arg.cct_range.gm_max = (bt_pfx_chase_body->Chase_CCT.GM_Max- 10) * 10;
                pfx.mode_arg.chase.base_arg.cct_range.gm_mini = (bt_pfx_chase_body->Chase_CCT.GM_Mini- 10) * 10;
                pfx.mode_arg.chase.base_arg.cct_range.gm_seq = bt_pfx_chase_body->Chase_CCT.GM_Seq;
                pfx.mode_arg.chase.base_arg.cct_range.int_max = bt_pfx_chase_body->Chase_CCT.Int_Max * 10;
                pfx.mode_arg.chase.base_arg.cct_range.int_mimi = bt_pfx_chase_body->Chase_CCT.Int_Mini * 10;
                pfx.mode_arg.chase.base_arg.cct_range.int_seq = bt_pfx_chase_body->Chase_CCT.Int_Seq;
                pfx.mode_arg.chase.fx_arg.loop = bt_pfx_chase_body->Chase_CCT.Loop;
                pfx.mode_arg.chase.fx_arg.time = bt_pfx_chase_body->Chase_CCT.Time * 100;                
            }
            else
            {
                pfx.mode_arg.chase.base = bt_pfx_chase_body->Chase_HSI.Base_Type;
                pfx.mode_arg.chase.base_arg.hsi_range.int_max = bt_pfx_chase_body->Chase_HSI.Int_Max * 20; 
                pfx.mode_arg.chase.base_arg.hsi_range.int_mimi = bt_pfx_chase_body->Chase_HSI.Int_Mini * 20;
                pfx.mode_arg.chase.base_arg.hsi_range.int_seq = bt_pfx_chase_body->Chase_HSI.Int_Seq;
                pfx.mode_arg.chase.base_arg.hsi_range.hue_max = bt_pfx_chase_body->Chase_HSI.Hue_Max;  //是0-100吗？？
                pfx.mode_arg.chase.base_arg.hsi_range.hue_mini = bt_pfx_chase_body->Chase_HSI.Hue_Mini;
                pfx.mode_arg.chase.base_arg.hsi_range.hue_seq = bt_pfx_chase_body->Chase_HSI.Hue_Seq;
                pfx.mode_arg.chase.base_arg.hsi_range.sat_max = bt_pfx_chase_body->Chase_HSI.Sat_Max * 2;
                pfx.mode_arg.chase.base_arg.hsi_range.sat_mini = bt_pfx_chase_body->Chase_HSI.Sat_Mini * 2;
                pfx.mode_arg.chase.base_arg.hsi_range.sat_seq = bt_pfx_chase_body->Chase_HSI.Sat_Seq;
                pfx.mode_arg.chase.fx_arg.loop = bt_pfx_chase_body->Chase_HSI.Loop;
                pfx.mode_arg.chase.fx_arg.time = bt_pfx_chase_body->Chase_HSI.Time * 100;                 
            }
            pfx.mode = SIDUS_PFX_Chase;
            data_center_write_light_data(LIGHT_MODE_SIDUS_PFX, &pfx);             
        }
        break;
        case BT_CommandType_PFX_Continue:
        {
            struct db_program_fx pfx;
            BT_PFX_Continue_Body_TypeDef *bt_pfx_continue_body = NULL;
            
            bt_pfx_continue_body = &write_body->PFX_Continue_Body;
            if(bt_pfx_continue_body->Continue_CCT.Base_Type == 0)
            {
                pfx.mode_arg.continues.base = bt_pfx_continue_body->Continue_CCT.Base_Type;
                pfx.mode_arg.continues.base_arg.cct.cct = bt_pfx_continue_body->Continue_CCT.CCT * 10;
                pfx.mode_arg.continues.base_arg.cct.lightness = bt_pfx_continue_body->Continue_CCT.Int;
                pfx.mode_arg.continues.base_arg.cct.gm = (bt_pfx_continue_body->Continue_CCT.GM- 10) * 10;  
                pfx.mode_arg.continues.fx_arg.continue_time = bt_pfx_continue_body->Continue_CCT.ContinueTime * 100;
                pfx.mode_arg.continues.fx_arg.fade_incurve = bt_pfx_continue_body->Continue_CCT.FadeInCurve;
                pfx.mode_arg.continues.fx_arg.fade_in_time = bt_pfx_continue_body->Continue_CCT.FadeInTime * 100;
                pfx.mode_arg.continues.fx_arg.fade_out_curve = bt_pfx_continue_body->Continue_CCT.FadeOutCurve;
                pfx.mode_arg.continues.fx_arg.fade_out_time = bt_pfx_continue_body->Continue_CCT.FadeOutTime * 100;                            
            }
            else
            {
                pfx.mode_arg.continues.base = bt_pfx_continue_body->Continue_HSI.Base_Type;  
                pfx.mode_arg.continues.base_arg.hsi.lightness = bt_pfx_continue_body->Continue_HSI.Int;
                pfx.mode_arg.continues.base_arg.hsi.hue = bt_pfx_continue_body->Continue_HSI.Hue;
                pfx.mode_arg.continues.base_arg.hsi.sat = bt_pfx_continue_body->Continue_HSI.Sat;
                pfx.mode_arg.continues.fx_arg.continue_time = bt_pfx_continue_body->Continue_HSI.ContinueTime * 100;
                pfx.mode_arg.continues.fx_arg.fade_incurve = bt_pfx_continue_body->Continue_HSI.FadeInCurve;
                pfx.mode_arg.continues.fx_arg.fade_in_time = bt_pfx_continue_body->Continue_HSI.FadeInTime * 100;
                pfx.mode_arg.continues.fx_arg.fade_out_curve = bt_pfx_continue_body->Continue_HSI.FadeOutCurve;
                pfx.mode_arg.continues.fx_arg.fade_out_time = bt_pfx_continue_body->Continue_HSI.FadeOutTime * 100;                          
            }
            pfx.mode = SIDUS_PFX_Continue;
            data_center_write_light_data(LIGHT_MODE_SIDUS_PFX, &pfx);                   
        }
        break;
        case BT_CommandType_CFX_Ctrl:
        {
            struct db_custom_fx cfx;
            BT_CFX_Ctrl_Body_TypeDef *bt_cfx_ctrl_body = NULL;
            
            bt_cfx_ctrl_body = &write_body->CFX_Ctrl_Body;
            cfx.bank = bt_cfx_ctrl_body->Bank;
            cfx.chaos = bt_cfx_ctrl_body->Chaos;
            cfx.lightness = bt_cfx_ctrl_body->Int;
            cfx.ctrl = bt_cfx_ctrl_body->Ctrl;
            cfx.loop = bt_cfx_ctrl_body->Loop;
            cfx.sequence = bt_cfx_ctrl_body->Sequence;
            cfx.speed = bt_cfx_ctrl_body->Speed;
            cfx.type = bt_cfx_ctrl_body->Effect_Type;
            data_center_write_light_data(LIGHT_MODE_SIDUS_CFX, &cfx);    
        }
        break;
        case BT_CommandType_CFX_Preview:
        {
            struct db_custom_preview_fx cfx_pre;
            BT_CFX_Preview_Body_TypeDef *bt_cfx_ctrl_body = NULL;
            
            bt_cfx_ctrl_body = &write_body->CFX_Preview_Body;
            
            cfx_pre.frame_1.base = bt_cfx_ctrl_body->Frame_1.BaseCCT.Base;
            switch(cfx_pre.frame_1.base)
            {
                case 0:
                    cfx_pre.frame_1.base_arg.cct.lightness = bt_cfx_ctrl_body->Frame_1.BaseCCT.Int;
                    cfx_pre.frame_1.base_arg.cct.cct = bt_cfx_ctrl_body->Frame_1.BaseCCT.CCT * 10;
                    cfx_pre.frame_1.base_arg.cct.gm = (bt_cfx_ctrl_body->Frame_1.BaseCCT.GM- 10) * 10;
                break;
                case 1:
                    cfx_pre.frame_1.base_arg.hsi.lightness = bt_cfx_ctrl_body->Frame_1.BaseHSI.Int;
                    cfx_pre.frame_1.base_arg.hsi.hue = bt_cfx_ctrl_body->Frame_1.BaseHSI.Hue;
                    cfx_pre.frame_1.base_arg.hsi.sat = bt_cfx_ctrl_body->Frame_1.BaseHSI.Sat;
                break;
                default:
                    break;           
            }
            cfx_pre.frame_2.base = bt_cfx_ctrl_body->Frame_2.BaseCCT.Base;
            switch(cfx_pre.frame_2.base)
            {
                case 0:
                    cfx_pre.frame_2.base_arg.cct.lightness = bt_cfx_ctrl_body->Frame_2.BaseCCT.Int;
                    cfx_pre.frame_2.base_arg.cct.cct = bt_cfx_ctrl_body->Frame_2.BaseCCT.CCT * 10;
                    cfx_pre.frame_2.base_arg.cct.gm = (bt_cfx_ctrl_body->Frame_2.BaseCCT.GM- 10) * 10;
                break;
                case 1:
                    cfx_pre.frame_2.base_arg.hsi.lightness = bt_cfx_ctrl_body->Frame_2.BaseHSI.Int;
                    cfx_pre.frame_2.base_arg.hsi.hue = bt_cfx_ctrl_body->Frame_2.BaseHSI.Hue;
                    cfx_pre.frame_2.base_arg.hsi.sat = bt_cfx_ctrl_body->Frame_2.BaseHSI.Sat;
                break;
                default:
                    break;           
            }  
            data_center_write_light_data(LIGHT_MODE_SIDUS_CFX_PREVIEW, &cfx_pre); 
        }
        break;
        case BT_CommandType_MFX:
        {
            struct db_manual_fx mfx;
            BT_MFX_Body_TypeDef *bt_mfx_ctrl_body = NULL;
            
            
            data_center_read_light_data(LIGHT_MODE_SIDUS_MFX , &mfx);
            bt_mfx_ctrl_body = &write_body->MFX_Body;  
            
            mfx.ctrl = bt_mfx_ctrl_body->Packet_0.BaseCCT.Ctrl;
            switch(bt_mfx_ctrl_body->Packet_0.BaseCCT.Packet_Num)
            {
                case 0:
                {
                    mfx.base = bt_mfx_ctrl_body->Packet_0.BaseCCT.Base;
                    if(mfx.base == 0)
                    {
                        mfx.base_arg.cct_range.int_mimi = bt_mfx_ctrl_body->Packet_0.BaseCCT.Int_Mini * 10;
                        mfx.base_arg.cct_range.int_max = bt_mfx_ctrl_body->Packet_0.BaseCCT.Int_Max * 10;
                        mfx.base_arg.cct_range.int_seq = bt_mfx_ctrl_body->Packet_0.BaseCCT.Int_Seq;
                        mfx.base_arg.cct_range.cct_mini = bt_mfx_ctrl_body->Packet_0.BaseCCT.CCT_Mini * 10;
                        mfx.base_arg.cct_range.cct_max = bt_mfx_ctrl_body->Packet_0.BaseCCT.CCT_Max * 10;
                        mfx.base_arg.cct_range.cct_seq = bt_mfx_ctrl_body->Packet_0.BaseCCT.CCT_Seq;
                        mfx.base_arg.cct_range.gm_mini = (bt_mfx_ctrl_body->Packet_0.BaseCCT.GM_Mini- 10) * 10;
                        mfx.base_arg.cct_range.gm_max = (bt_mfx_ctrl_body->Packet_0.BaseCCT.GM_Max- 10) * 10;
                        mfx.base_arg.cct_range.gm_seq = bt_mfx_ctrl_body->Packet_0.BaseCCT.GM_Seq;    
                    }
                    else if(mfx.base == 1)
                    {
                        mfx.base_arg.hsi_range.int_mimi = bt_mfx_ctrl_body->Packet_0.BaseHSI.Int_Mini * 10;
                        mfx.base_arg.hsi_range.int_max = bt_mfx_ctrl_body->Packet_0.BaseHSI.Int_Max * 10;
                        mfx.base_arg.hsi_range.int_seq = bt_mfx_ctrl_body->Packet_0.BaseHSI.Int_Seq;
                        mfx.base_arg.hsi_range.hue_mini = bt_mfx_ctrl_body->Packet_0.BaseHSI.Hue_Mini;
                        mfx.base_arg.hsi_range.hue_max = bt_mfx_ctrl_body->Packet_0.BaseHSI.Hue_Max;
                        mfx.base_arg.hsi_range.hue_seq = bt_mfx_ctrl_body->Packet_0.BaseHSI.Hue_Seq;
                        mfx.base_arg.hsi_range.sat_mini = bt_mfx_ctrl_body->Packet_0.BaseHSI.Sat_Mini;
                        mfx.base_arg.hsi_range.sat_max = bt_mfx_ctrl_body->Packet_0.BaseHSI.Sat_Max;
                        mfx.base_arg.hsi_range.sat_seq = bt_mfx_ctrl_body->Packet_0.BaseHSI.Sat_Seq;   
                    }
                }
                break;
                case 1:
                {
                    mfx.fx_arg.mode = bt_mfx_ctrl_body->Packet_1.Flash.Effect_Mode;
                    if(mfx.fx_arg.mode == 0)
                    {
                        mfx.fx_arg.mode_arg.flash.free_time_seq = bt_mfx_ctrl_body->Packet_1.Flash.FreeTime_Seq;
                        mfx.fx_arg.mode_arg.flash.free_time_max = bt_mfx_ctrl_body->Packet_1.Flash.FreeTime_Max * 100;
                        mfx.fx_arg.mode_arg.flash.free_time_mini = bt_mfx_ctrl_body->Packet_1.Flash.FreeTime_Mini * 100;
                        mfx.fx_arg.mode_arg.flash.cycle_time_seq = bt_mfx_ctrl_body->Packet_1.Flash.CycleTime_Seq;
                        mfx.fx_arg.mode_arg.flash.cycle_time_max = bt_mfx_ctrl_body->Packet_1.Flash.CycleTime_Max * 100;
                        mfx.fx_arg.mode_arg.flash.cycle_time_mini = bt_mfx_ctrl_body->Packet_1.Flash.CycleTime_Mini * 100;
                        mfx.fx_arg.mode_arg.flash.loop_times = bt_mfx_ctrl_body->Packet_1.Flash.LoopTimes;
                        mfx.fx_arg.mode_arg.flash.loop_mode = bt_mfx_ctrl_body->Packet_1.Flash.LoopMode;                        
                    }
                    else if(mfx.fx_arg.mode == 1)
                    {
                        mfx.fx_arg.mode_arg.continues.fade_in_curve = bt_mfx_ctrl_body->Packet_1.Continue.FadeIn_Curve;
                        mfx.fx_arg.mode_arg.continues.fade_in_time_max = bt_mfx_ctrl_body->Packet_1.Continue.FadeInTime_Max * 100;
                        mfx.fx_arg.mode_arg.continues.fade_in_time_mini = bt_mfx_ctrl_body->Packet_1.Continue.FadeInTime_Mini * 100;
                        mfx.fx_arg.mode_arg.continues.fade_in_time_seq = bt_mfx_ctrl_body->Packet_1.Continue.FadeInTime_Seq;
                        mfx.fx_arg.mode_arg.continues.cycle_time_seq = bt_mfx_ctrl_body->Packet_1.Continue.CycleTime_Seq;
                        mfx.fx_arg.mode_arg.continues.cycle_time_max = bt_mfx_ctrl_body->Packet_1.Continue.CycleTime_Max * 100;
                        mfx.fx_arg.mode_arg.continues.cycle_time_mini = bt_mfx_ctrl_body->Packet_1.Continue.CycleTime_Mini * 100;
                        mfx.fx_arg.mode_arg.continues.loop_times = bt_mfx_ctrl_body->Packet_1.Continue.LoopTimes;
                        mfx.fx_arg.mode_arg.continues.loop_mode = bt_mfx_ctrl_body->Packet_1.Continue.LoopMode;
                    
                    }
                    else if(mfx.fx_arg.mode == 2)
                    {
                        mfx.fx_arg.mode_arg.paragraph.unit_time_seq = bt_mfx_ctrl_body->Packet_1.Paragraph.UnitTime_Seq;
                        mfx.fx_arg.mode_arg.paragraph.free_time_seq = bt_mfx_ctrl_body->Packet_1.Paragraph.FreeTime_Seq;
                        mfx.fx_arg.mode_arg.paragraph.overlap_seq = bt_mfx_ctrl_body->Packet_1.Paragraph.Overlap_Seq;
                        mfx.fx_arg.mode_arg.paragraph.olr_seq = bt_mfx_ctrl_body->Packet_1.Paragraph.OLR_Seq;
                        mfx.fx_arg.mode_arg.paragraph.olr_max = bt_mfx_ctrl_body->Packet_1.Paragraph.OLR_Max;
                        mfx.fx_arg.mode_arg.paragraph.olr_mini = bt_mfx_ctrl_body->Packet_1.Paragraph.OLR_Mini;
                        mfx.fx_arg.mode_arg.paragraph.cycle_time_seq = bt_mfx_ctrl_body->Packet_1.Paragraph.CycleTime_Seq;
                        mfx.fx_arg.mode_arg.paragraph.cycle_time_max = bt_mfx_ctrl_body->Packet_1.Paragraph.CycleTime_Max * 100;
                        mfx.fx_arg.mode_arg.paragraph.cycle_time_mini = bt_mfx_ctrl_body->Packet_1.Paragraph.CycleTime_Mini * 100;
                        mfx.fx_arg.mode_arg.paragraph.loop_times = bt_mfx_ctrl_body->Packet_1.Paragraph.LoopTimes;
                        mfx.fx_arg.mode_arg.paragraph.loop_mode = bt_mfx_ctrl_body->Packet_1.Paragraph.LoopMode;                    
                    }
                
                
                
                }
                break;
                case 2:
                    mfx.fx_arg.mode = bt_mfx_ctrl_body->Packet_2.Flash.Effect_Mode;
                    switch(mfx.fx_arg.mode)
                    {
                        case 0:
                            mfx.fx_arg.mode_arg.flash.frq_seq = bt_mfx_ctrl_body->Packet_2.Flash.Frq_Seq;
                            mfx.fx_arg.mode_arg.flash.frq_max = bt_mfx_ctrl_body->Packet_2.Flash.Frq_Max;
                            mfx.fx_arg.mode_arg.flash.frq_mini = bt_mfx_ctrl_body->Packet_2.Flash.Frq_Mini;
                            mfx.fx_arg.mode_arg.flash.unit_time_seq = bt_mfx_ctrl_body->Packet_2.Flash.UnitTime_Seq;
                            mfx.fx_arg.mode_arg.flash.unit_time_max = bt_mfx_ctrl_body->Packet_2.Flash.UnitTime_Max * 100;
                            mfx.fx_arg.mode_arg.flash.unit_time_mini = bt_mfx_ctrl_body->Packet_2.Flash.UnitTime_Mini * 100;
                            break;
                        case 1:
                            mfx.fx_arg.mode_arg.continues.flicker_frq = bt_mfx_ctrl_body->Packet_2.Continue.Flicker_Frq;
                            mfx.fx_arg.mode_arg.continues.fade_out_curve = bt_mfx_ctrl_body->Packet_2.Continue.FadeOut_Curve;
                            mfx.fx_arg.mode_arg.continues.fade_out_time_seq = bt_mfx_ctrl_body->Packet_2.Continue.FadeOutTime_Seq;
                            mfx.fx_arg.mode_arg.continues.fade_out_time_max = bt_mfx_ctrl_body->Packet_2.Continue.FadeOutTime_Max * 100;
                            mfx.fx_arg.mode_arg.continues.fade_out_time_mini = bt_mfx_ctrl_body->Packet_2.Continue.FadeOutTime_Mini * 100;
                            break;
                        case 2:
                            mfx.fx_arg.mode_arg.paragraph.olp_seq = bt_mfx_ctrl_body->Packet_2.Paragraph.OLP_Seq;
                            mfx.fx_arg.mode_arg.paragraph.olp_max = bt_mfx_ctrl_body->Packet_2.Paragraph.OLP_Max;
                            mfx.fx_arg.mode_arg.paragraph.olp_mini = bt_mfx_ctrl_body->Packet_2.Paragraph.OLP_Mini;
                            mfx.fx_arg.mode_arg.paragraph.unit_time_max = bt_mfx_ctrl_body->Packet_2.Paragraph.UnitTime_Max * 100;
                            mfx.fx_arg.mode_arg.paragraph.unit_time_mini = bt_mfx_ctrl_body->Packet_2.Paragraph.UnitTime_Mini * 100;
                            mfx.fx_arg.mode_arg.paragraph.free_time_max = bt_mfx_ctrl_body->Packet_2.Paragraph.FreeTime_Max * 100;
                            mfx.fx_arg.mode_arg.paragraph.free_time_mini = bt_mfx_ctrl_body->Packet_2.Paragraph.FreeTime_Mini * 100;
                            break;
                        default:
                            break;  
                    }                        
                break;
                case 3:
                    mfx.fx_arg.mode = bt_mfx_ctrl_body->Packet_3.Paragraph.Effect_Mode;
                    switch(mfx.fx_arg.mode)
                    {
                        case 0:
                            break;
                        case 1:
                            break;
                        case 2:
                            mfx.fx_arg.mode_arg.paragraph.flicker_frq = bt_mfx_ctrl_body->Packet_3.Paragraph.Flicker_Frq;
                            mfx.fx_arg.mode_arg.paragraph.fade_out_curve = bt_mfx_ctrl_body->Packet_3.Paragraph.FadeOut_Curve;
                            mfx.fx_arg.mode_arg.paragraph.fade_out_time_seq = bt_mfx_ctrl_body->Packet_3.Paragraph.FadeOutTime_Seq;
                            mfx.fx_arg.mode_arg.paragraph.fade_out_time_max = bt_mfx_ctrl_body->Packet_3.Paragraph.FadeOutTime_Max * 100;
                            mfx.fx_arg.mode_arg.paragraph.fade_out_time_mini = bt_mfx_ctrl_body->Packet_3.Paragraph.FadeOutTime_Mini * 100;
                            mfx.fx_arg.mode_arg.paragraph.fade_in_curve = bt_mfx_ctrl_body->Packet_3.Paragraph.FadeIn_Curve;
                            mfx.fx_arg.mode_arg.paragraph.fade_in_time_seq = bt_mfx_ctrl_body->Packet_3.Paragraph.FadeInTime_Seq;
                            mfx.fx_arg.mode_arg.paragraph.fade_in_time_max = bt_mfx_ctrl_body->Packet_3.Paragraph.FadeInTime_Max * 100;
                            mfx.fx_arg.mode_arg.paragraph.fade_in_time_mini = bt_mfx_ctrl_body->Packet_3.Paragraph.FadeInTime_Mini * 100;
                            break;
                        default:
                            break;
                    }
                    break;                    
                break;
                default:
                    break;
            }
            
            data_center_write_light_data(LIGHT_MODE_SIDUS_MFX , &mfx);
        }
        break;
        case BT_CommandType_CFX_Name:
        {
//            char *cfx_name1;
            struct  sys_info_cfx_name cfx_name;
			uint8_t tx_data[10] = {0};
            BT_CFX_Name_Body_TypeDef *bt_cfx_name_body = NULL; 
            bt_cfx_name_body = &write_body->CFX_Name_Body;              
            data_center_read_sys_info(SYS_INFO_CFX_NAME, &cfx_name);
            
//            user_data_str.Local_SidusFX_Data.SFX_Arg.CFX.Effect_Type = bt_cfx_name_body->Effect_Type;
//            user_data_str.Local_SidusFX_Data.SFX_Arg.CFX.Bank = bt_cfx_name_body->Bank;
            cfx_name.cfx_type = bt_cfx_name_body->Effect_Type;
            cfx_name.cfx_bank = bt_cfx_name_body->Bank;
            
//            cfx_name1 = &cfx_name.cfx_name[bt_cfx_name_body->Effect_Type].name[bt_cfx_name_body->Bank][0];
            cfx_name.read_cfx_name[0] = SidusProFile_SidusCode2ASCII(bt_cfx_name_body->Character_0);
            cfx_name.read_cfx_name[1] = SidusProFile_SidusCode2ASCII(bt_cfx_name_body->Character_1);
            cfx_name.read_cfx_name[2] = SidusProFile_SidusCode2ASCII(bt_cfx_name_body->Character_2);
            cfx_name.read_cfx_name[3] = SidusProFile_SidusCode2ASCII(bt_cfx_name_body->Character_3);
            cfx_name.read_cfx_name[4] = SidusProFile_SidusCode2ASCII(bt_cfx_name_body->Character_4);
            cfx_name.read_cfx_name[5] = SidusProFile_SidusCode2ASCII(bt_cfx_name_body->Character_5);
            cfx_name.read_cfx_name[6] = SidusProFile_SidusCode2ASCII(bt_cfx_name_body->Character_6);
            cfx_name.read_cfx_name[7] = SidusProFile_SidusCode2ASCII(bt_cfx_name_body->Character_7);
            cfx_name.read_cfx_name[8] = SidusProFile_SidusCode2ASCII(bt_cfx_name_body->Character_8);
            
            data_center_write_sys_info(SYS_INFO_CFX_NAME, &cfx_name);
            set_write_cfx_name_state(1);
			ble_protocol_data_pack_up(tx_data, (BT_Body_TypeDef*)&bt_cfx_name_body, BT_CommandType_CFX_Name, 0);
			ble_send_data_queue(BLE_MESH_DATA, tx_data, 10);
            //user_turn_to_page(PAGE_SIDUS_PRO_FX,0,false);
            
        }
        break;
        
        default:break;
    }
}

/* 写二代光效 */
static void write_parse_effect_II(BT_Body_TypeDef *write_body)
{
	bt_light_effect_II_arg_t *bt_effect2_body = NULL;
	bt_effect2_body = &write_body->light_effect_II;
	uint8_t origin = 0;
	uint8_t gtype = 0;
	switch(bt_effect2_body->paparazzi_II.base.effect_type)
	{
		case BT_FX_II_Paparazzi_2:
        {
            struct db_fx_paparazzi_2 paparazzi_2;
			paparazzi_2.state = bt_effect2_body->paparazzi_II.base.state;
            if(bt_effect2_body->paparazzi_II.base.package_type == 0)
            {
				paparazzi_2.lightness = bt_effect2_body->paparazzi_II.base.brightness;
                paparazzi_2.gap_time = bt_effect2_body->paparazzi_II.base.gap_time * 100;           // ms
                paparazzi_2.min_gap_time = bt_effect2_body->paparazzi_II.base.min_gap_time * 100;	// ms
            }
            else if(bt_effect2_body->paparazzi_II.base.package_type == 1)
            {
				paparazzi_2.mode = (enum fx_mode)bt_effect2_body->paparazzi_II.pixel.cct.light_mode;
				
                switch(bt_effect2_body->paparazzi_II.pixel.cct.light_mode)
                {
                    case BT_FX_II_Mode_CCT:
                        paparazzi_2.mode_arg.cct.cct = bt_effect2_body->paparazzi_II.pixel.cct.cct * 50;
                        paparazzi_2.mode_arg.cct.duv = bt_effect2_body->paparazzi_II.pixel.cct.duv  - 100;
						break;
                    case BT_FX_II_Mode_HSI:
                        paparazzi_2.mode_arg.hsi.sat = bt_effect2_body->paparazzi_II.pixel.hsi.sat ;
                        paparazzi_2.mode_arg.hsi.hue = bt_effect2_body->paparazzi_II.pixel.hsi.hue ;
                        paparazzi_2.mode_arg.hsi.cct = bt_effect2_body->paparazzi_II.pixel.hsi.cct * 50;
						break;
                    case BT_FX_II_Mode_GEL:
                        paparazzi_2.mode_arg.gel.brand = bt_effect2_body->paparazzi_II.pixel.gel.origin== 0 ? 1 : 0;
						origin = bt_effect2_body->paparazzi_II.pixel.gel.origin== 0 ? 1 : 0;
                        paparazzi_2.mode_arg.gel.type[origin] = bt_effect2_body->paparazzi_II.pixel.gel.type;
						gtype = bt_effect2_body->paparazzi_II.pixel.gel.type;
                        paparazzi_2.mode_arg.gel.color[origin][gtype] = bt_effect2_body->paparazzi_II.pixel.gel.color;
                        paparazzi_2.mode_arg.gel.cct = bt_effect2_body->paparazzi_II.pixel.gel.cct * 50;
						break;
                    case BT_FX_II_Mode_XY:
                        paparazzi_2.mode_arg.xy.x = bt_effect2_body->paparazzi_II.pixel.xy.x;
                        paparazzi_2.mode_arg.xy.y = bt_effect2_body->paparazzi_II.pixel.xy.y;
						break;
                    case BT_FX_II_Mode_SOUYRCE:
                        paparazzi_2.mode_arg.source.type = bt_effect2_body->paparazzi_II.pixel.source.type;
                        paparazzi_2.mode_arg.source.x = bt_effect2_body->paparazzi_II.pixel.source.x;
                        paparazzi_2.mode_arg.source.y = bt_effect2_body->paparazzi_II.pixel.source.y;
						break;
                    case BT_FX_II_Mode_RGB:
                        paparazzi_2.mode_arg.rgb.r = bt_effect2_body->paparazzi_II.pixel.rgb.r;
                        paparazzi_2.mode_arg.rgb.g = bt_effect2_body->paparazzi_II.pixel.rgb.g;
                        paparazzi_2.mode_arg.rgb.b = bt_effect2_body->paparazzi_II.pixel.rgb.b;
						break;

                    default: break;
                }
            }
			if(bt_effect2_body->paparazzi_II.base.state != 3)
                data_center_write_light_data(LIGHT_MODE_FX_PAPARAZZI_II, &paparazzi_2);
			break;
        }
        case BT_FX_II_Lightning_2: // 
        {
            struct db_fx_lightning_2 lightning_2;
			data_center_read_light_data(LIGHT_MODE_FX_LIGHTNING_II, &lightning_2); 
            lightning_2.state = bt_effect2_body->lightning_II.cct.state;
            lightning_2.lightness = bt_effect2_body->lightning_II.cct.brightness;
            lightning_2.frq = bt_effect2_body->lightning_II.cct.frq;
            lightning_2.spd = bt_effect2_body->lightning_II.cct.spd;
            lightning_2.mode = (enum fx_mode)bt_effect2_body->lightning_II.cct.light_mode;
			if(lightning_2.state == 2)
			{
				lightning_2.state = 1;
				lightning_2.trigger = 0;
			}	
			else if(lightning_2.state == 1)
			{
				lightning_2.trigger = 1;
				lightning_2.state = 0;
				ui_set_Lightning_2_state(1);
			}
			else
			{
				lightning_2.trigger = 0;
			
			}
			
            switch(bt_effect2_body->lightning_II.cct.light_mode)
            {
                case BT_FX_II_Mode_CCT:
                    lightning_2.mode_arg.cct.cct = bt_effect2_body->lightning_II.cct.cct * 50;
                    lightning_2.mode_arg.cct.duv = bt_effect2_body->lightning_II.cct.duv  - 100;
                    break;
                case BT_FX_II_Mode_HSI:
                    lightning_2.mode_arg.hsi.sat = bt_effect2_body->lightning_II.hsi.sat ;
                    lightning_2.mode_arg.hsi.hue = bt_effect2_body->lightning_II.hsi.hue ;
                    lightning_2.mode_arg.hsi.cct = bt_effect2_body->lightning_II.hsi.cct * 50;
                    break;
                
                default: break;
            }
            data_center_write_light_data(LIGHT_MODE_FX_LIGHTNING_II, &lightning_2);
			break;  
        }
		case BT_FX_II_TV_2:  // 
		{
            struct db_fx_tv_2 tv_2;
			data_center_read_light_data(LIGHT_MODE_FX_TV_II, &tv_2); 
			tv_2.state = bt_effect2_body->tv_II.cct.state;
            tv_2.lightness = bt_effect2_body->tv_II.cct.brightness;
            tv_2.spd = bt_effect2_body->tv_II.cct.spd;
            tv_2.mode = (enum fx_mode)bt_effect2_body->tv_II.cct.light_mode;
				
            switch(bt_effect2_body->tv_II.cct.light_mode)
            {
                case BT_FX_II_Mode_CCT:
                    tv_2.mode_arg.cct.max_cct = bt_effect2_body->tv_II.cct.max_cct * 50;
                    tv_2.mode_arg.cct.min_cct = bt_effect2_body->tv_II.cct.min_cct * 50;
                    tv_2.mode_arg.cct.duv = bt_effect2_body->tv_II.cct.duv  - 100;
                    break;
                case BT_FX_II_Mode_HSI:
                    tv_2.mode_arg.hsi.sat = bt_effect2_body->tv_II.hsi.sat * 10;
                    tv_2.mode_arg.hsi.max_hue = bt_effect2_body->tv_II.hsi.max_hue ;
                    tv_2.mode_arg.hsi.min_hue = bt_effect2_body->tv_II.hsi.min_hue ;
                    tv_2.mode_arg.hsi.cct = bt_effect2_body->tv_II.hsi.cct * 50;
                    break;
                
                default: break;
            }
            data_center_write_light_data(LIGHT_MODE_FX_TV_II, &tv_2);
			break;
        }
		case BT_FX_II_Fire_2: // 
		{
            struct db_fx_fire_2 fire_2;
			data_center_read_light_data(LIGHT_MODE_FX_FIRE_II, &fire_2); 
			fire_2.state = bt_effect2_body->fire_II.cct.state;
            fire_2.lightness = bt_effect2_body->fire_II.cct.brightness;
            fire_2.spd = bt_effect2_body->fire_II.cct.spd;
            fire_2.mode = (enum fx_mode)bt_effect2_body->fire_II.cct.light_mode;
				
            switch(bt_effect2_body->fire_II.cct.light_mode)
            {
                case BT_FX_II_Mode_CCT:
                    fire_2.mode_arg.cct.max_cct = bt_effect2_body->fire_II.cct.max_cct * 50;
                    fire_2.mode_arg.cct.min_cct = bt_effect2_body->fire_II.cct.min_cct * 50;
                    fire_2.mode_arg.cct.duv = bt_effect2_body->fire_II.cct.duv  - 100;
                    break;
                case BT_FX_II_Mode_HSI:
                    fire_2.mode_arg.hsi.sat = bt_effect2_body->fire_II.hsi.sat ;
                    fire_2.mode_arg.hsi.max_hue = bt_effect2_body->fire_II.hsi.max_hue ;
                    fire_2.mode_arg.hsi.min_hue = bt_effect2_body->fire_II.hsi.min_hue ;
                    fire_2.mode_arg.hsi.cct = bt_effect2_body->fire_II.hsi.cct * 50;
                    break;
                
                default: break;
            }
            data_center_write_light_data(LIGHT_MODE_FX_FIRE_II, &fire_2);
			break; 
        } 
		case BT_FX_II_Strobe_2:  // 闪光灯 2
		{
            struct db_fx_strobe_2 strobe_2;
			data_center_read_light_data(LIGHT_MODE_FX_STROBE_II, &strobe_2); 
			strobe_2.state = bt_effect2_body->strobe_II.cct.state;
            strobe_2.lightness = bt_effect2_body->strobe_II.cct.brightness;
            strobe_2.spd = bt_effect2_body->strobe_II.cct.spd;
            strobe_2.mode = (enum fx_mode)bt_effect2_body->strobe_II.cct.light_mode;
				
            switch(bt_effect2_body->strobe_II.cct.light_mode)
            {
                case BT_FX_II_Mode_CCT:
                    strobe_2.mode_arg.cct.cct = bt_effect2_body->strobe_II.cct.cct * 50;
                    strobe_2.mode_arg.cct.duv = bt_effect2_body->strobe_II.cct.duv  - 100;
                    break;
                case BT_FX_II_Mode_HSI:
                    strobe_2.mode_arg.hsi.sat = bt_effect2_body->strobe_II.hsi.sat ;
                    strobe_2.mode_arg.hsi.hue = bt_effect2_body->strobe_II.hsi.hue ;
                    strobe_2.mode_arg.hsi.cct = bt_effect2_body->strobe_II.hsi.cct * 50;
                case BT_FX_II_Mode_GEL:
                    strobe_2.mode_arg.gel.brand = bt_effect2_body->strobe_II.gel.origin== 0 ? 1 : 0;
                    origin = bt_effect2_body->strobe_II.gel.origin== 0 ? 1 : 0;
                    strobe_2.mode_arg.gel.type[origin] = bt_effect2_body->strobe_II.gel.type;
                    gtype = bt_effect2_body->strobe_II.gel.type;
                    strobe_2.mode_arg.gel.color[origin][gtype] = bt_effect2_body->strobe_II.gel.color;
                    strobe_2.mode_arg.gel.cct = bt_effect2_body->strobe_II.gel.cct * 50;
                    break;
                case BT_FX_II_Mode_XY:
                    strobe_2.mode_arg.xy.x = bt_effect2_body->strobe_II.xy.x;
                    strobe_2.mode_arg.xy.y = bt_effect2_body->strobe_II.xy.y;
                    break;
                case BT_FX_II_Mode_SOUYRCE:
                    strobe_2.mode_arg.source.type = bt_effect2_body->strobe_II.source.type;
                    strobe_2.mode_arg.source.x = bt_effect2_body->strobe_II.source.x;
                    strobe_2.mode_arg.source.y = bt_effect2_body->strobe_II.source.y;
                    break;
                case BT_FX_II_Mode_RGB:
                    strobe_2.mode_arg.rgb.r = bt_effect2_body->strobe_II.rgb.r;
                    strobe_2.mode_arg.rgb.g = bt_effect2_body->strobe_II.rgb.g;
                    strobe_2.mode_arg.rgb.b = bt_effect2_body->strobe_II.rgb.b;
                    break;
                
                default: break;
            }
            data_center_write_light_data(LIGHT_MODE_FX_STROBE_II, &strobe_2);
			break; 
        }
		case BT_FX_II_Explosion_2:  // 
		{
            struct db_fx_explosion_2 explosion_2;
			data_center_read_light_data(LIGHT_MODE_FX_EXPLOSION_II, &explosion_2); 
            explosion_2.state = bt_effect2_body->explosion_II.cct.state;
            explosion_2.lightness = bt_effect2_body->explosion_II.cct.brightness;
            explosion_2.decay = bt_effect2_body->explosion_II.cct.decay;
            explosion_2.mode = (enum fx_mode)bt_effect2_body->explosion_II.cct.light_mode;
			ui_set_explosion_2_anim(1);
            switch(bt_effect2_body->explosion_II.cct.light_mode)
            {
                case BT_FX_II_Mode_CCT:
                    explosion_2.mode_arg.cct.cct = bt_effect2_body->explosion_II.cct.cct * 50;
                    explosion_2.mode_arg.cct.duv = bt_effect2_body->explosion_II.cct.duv  - 100;
                    break;
                case BT_FX_II_Mode_HSI:
                    explosion_2.mode_arg.hsi.sat = bt_effect2_body->explosion_II.hsi.sat ;
                    explosion_2.mode_arg.hsi.hue = bt_effect2_body->explosion_II.hsi.hue ;
                    explosion_2.mode_arg.hsi.cct = bt_effect2_body->explosion_II.hsi.cct * 50;
                case BT_FX_II_Mode_GEL:
                    explosion_2.mode_arg.gel.brand = bt_effect2_body->explosion_II.gel.origin== 0 ? 1 : 0;
                    origin = bt_effect2_body->explosion_II.gel.origin== 0 ? 1 : 0;
                    explosion_2.mode_arg.gel.type[origin] = bt_effect2_body->explosion_II.gel.type;
                    gtype = bt_effect2_body->explosion_II.gel.type;
                    explosion_2.mode_arg.gel.color[origin][gtype] = bt_effect2_body->explosion_II.gel.color;
                    explosion_2.mode_arg.gel.cct = bt_effect2_body->explosion_II.gel.cct * 50;
                    break;
                case BT_FX_II_Mode_XY:
                    explosion_2.mode_arg.xy.x = bt_effect2_body->explosion_II.xy.x;
                    explosion_2.mode_arg.xy.y = bt_effect2_body->explosion_II.xy.y;
                    break;
                case BT_FX_II_Mode_SOUYRCE:
                    explosion_2.mode_arg.source.type = bt_effect2_body->explosion_II.source.type;
                    explosion_2.mode_arg.source.x = bt_effect2_body->explosion_II.source.x;
                    explosion_2.mode_arg.source.y = bt_effect2_body->explosion_II.source.y;
                    break;
                case BT_FX_II_Mode_RGB:
                    explosion_2.mode_arg.rgb.r = bt_effect2_body->explosion_II.rgb.r;
                    explosion_2.mode_arg.rgb.g = bt_effect2_body->explosion_II.rgb.g;
                    explosion_2.mode_arg.rgb.b = bt_effect2_body->explosion_II.rgb.b;
                    break;
                
                default: break;
            }
            data_center_write_light_data(LIGHT_MODE_FX_EXPLOSION_II, &explosion_2);
			break;
        }
		case BT_FX_II_Fault_Bulb_2: // 
		{
            struct db_fx_fault_bulb_2 fault_bulb_2;
			data_center_read_light_data(LIGHT_MODE_FX_FAULT_BULB_II, &fault_bulb_2); 
            fault_bulb_2.state = bt_effect2_body->fault_bulb_II.cct.state;
            fault_bulb_2.lightness = bt_effect2_body->fault_bulb_II.cct.brightness;
            fault_bulb_2.spd = bt_effect2_body->fault_bulb_II.cct.spd;
            fault_bulb_2.frq = bt_effect2_body->fault_bulb_II.cct.frq;
            fault_bulb_2.mode = (enum fx_mode)bt_effect2_body->fault_bulb_II.cct.light_mode;
				
            switch(read_ble_effectmodeback(bt_effect2_body->fault_bulb_II.cct.light_mode))
            {
                case BT_FX_II_Mode_CCT:
                    fault_bulb_2.mode_arg.cct.cct = bt_effect2_body->fault_bulb_II.cct.cct * 50;
                    fault_bulb_2.mode_arg.cct.duv = bt_effect2_body->fault_bulb_II.cct.duv  - 100;
                    break;
                case BT_FX_II_Mode_HSI:
                    fault_bulb_2.mode_arg.hsi.sat = bt_effect2_body->fault_bulb_II.hsi.sat ;
                    fault_bulb_2.mode_arg.hsi.hue = bt_effect2_body->fault_bulb_II.hsi.hue ;
                    fault_bulb_2.mode_arg.hsi.cct = bt_effect2_body->fault_bulb_II.hsi.cct * 50;
                case BT_FX_II_Mode_GEL:
                    fault_bulb_2.mode_arg.gel.brand = bt_effect2_body->fault_bulb_II.gel.origin== 0 ? 1 : 0;
                    origin = bt_effect2_body->fault_bulb_II.gel.origin== 0 ? 1 : 0;
                    fault_bulb_2.mode_arg.gel.type[origin] = bt_effect2_body->fault_bulb_II.gel.type;
                    gtype = bt_effect2_body->fault_bulb_II.gel.type;
                    fault_bulb_2.mode_arg.gel.color[origin][gtype] = bt_effect2_body->fault_bulb_II.gel.color;
                    fault_bulb_2.mode_arg.gel.cct = bt_effect2_body->fault_bulb_II.gel.cct * 50;
                    break;
                case BT_FX_II_Mode_XY:
                    fault_bulb_2.mode_arg.xy.x = bt_effect2_body->fault_bulb_II.xy.x;
                    fault_bulb_2.mode_arg.xy.y = bt_effect2_body->fault_bulb_II.xy.y;
                    break;
                case BT_FX_II_Mode_SOUYRCE:
                    fault_bulb_2.mode_arg.source.type = bt_effect2_body->fault_bulb_II.source.type;
                    fault_bulb_2.mode_arg.source.x = bt_effect2_body->fault_bulb_II.source.x;
                    fault_bulb_2.mode_arg.source.y = bt_effect2_body->fault_bulb_II.source.y;
                    break;
                case BT_FX_II_Mode_RGB:
                    fault_bulb_2.mode_arg.rgb.r = bt_effect2_body->fault_bulb_II.rgb.r;
                    fault_bulb_2.mode_arg.rgb.g = bt_effect2_body->fault_bulb_II.rgb.g;
                    fault_bulb_2.mode_arg.rgb.b = bt_effect2_body->fault_bulb_II.rgb.b;
                    break;
                
                default: break;
            }
            data_center_write_light_data(LIGHT_MODE_FX_FAULT_BULB_II, &fault_bulb_2);
			break;
        }
		case BT_FX_II_Pulsing_2:  // 脉搏2
		{
            struct db_fx_pulsing_2 pulsing_2;
			data_center_read_light_data(LIGHT_MODE_FX_PULSING_II, &pulsing_2); 
            pulsing_2.state = bt_effect2_body->pulsing_II.cct.state;
            pulsing_2.lightness = bt_effect2_body->pulsing_II.cct.brightness;
            pulsing_2.spd = bt_effect2_body->pulsing_II.cct.spd;
			pulsing_2.frq = bt_effect2_body->pulsing_II.cct.frq;
            pulsing_2.mode = (enum fx_mode)bt_effect2_body->pulsing_II.cct.light_mode;
				
            switch(bt_effect2_body->pulsing_II.cct.light_mode)
            {
                case BT_FX_II_Mode_CCT:
                    pulsing_2.mode_arg.cct.cct = bt_effect2_body->pulsing_II.cct.cct * 50;
                    pulsing_2.mode_arg.cct.duv = bt_effect2_body->pulsing_II.cct.duv  - 100;
                    break;
                case BT_FX_II_Mode_HSI:
                    pulsing_2.mode_arg.hsi.sat = bt_effect2_body->pulsing_II.hsi.sat ;
                    pulsing_2.mode_arg.hsi.hue = bt_effect2_body->pulsing_II.hsi.hue ;
                    pulsing_2.mode_arg.hsi.cct = bt_effect2_body->pulsing_II.hsi.cct * 50;
                case BT_FX_II_Mode_GEL:
                    pulsing_2.mode_arg.gel.brand = bt_effect2_body->pulsing_II.gel.origin== 0 ? 1 : 0;
                    origin = bt_effect2_body->pulsing_II.gel.origin== 0 ? 1 : 0;
                    pulsing_2.mode_arg.gel.type[origin] = bt_effect2_body->pulsing_II.gel.type;
                    gtype = bt_effect2_body->pulsing_II.gel.type;
                    pulsing_2.mode_arg.gel.color[origin][gtype] = bt_effect2_body->pulsing_II.gel.color;
                    pulsing_2.mode_arg.gel.cct = bt_effect2_body->pulsing_II.gel.cct * 50;
                    break;
                case BT_FX_II_Mode_XY:
                    pulsing_2.mode_arg.xy.x = bt_effect2_body->pulsing_II.xy.x;
                    pulsing_2.mode_arg.xy.y = bt_effect2_body->pulsing_II.xy.y;
                    break;
                case BT_FX_II_Mode_SOUYRCE:
                    pulsing_2.mode_arg.source.type = bt_effect2_body->pulsing_II.source.type;
                    pulsing_2.mode_arg.source.x = bt_effect2_body->pulsing_II.source.x;
                    pulsing_2.mode_arg.source.y = bt_effect2_body->pulsing_II.source.y;
                    break;
                case BT_FX_II_Mode_RGB:
                    pulsing_2.mode_arg.rgb.r = bt_effect2_body->pulsing_II.rgb.r;
                    pulsing_2.mode_arg.rgb.g = bt_effect2_body->pulsing_II.rgb.g;
                    pulsing_2.mode_arg.rgb.b = bt_effect2_body->pulsing_II.rgb.b;
                    break;
                
                default: break;
            }
            data_center_write_light_data(LIGHT_MODE_FX_PULSING_II, &pulsing_2);
			break;
        }
		case BT_FX_II_Welding_2:  // 
		{
            struct db_fx_welding_2 welding_2;
			data_center_read_light_data(LIGHT_MODE_FX_WELDING_II, &welding_2); 
            welding_2.state = bt_effect2_body->welding_II.base.state;
            if(bt_effect2_body->welding_II.base.package_type == 0)
            {
				welding_2.lightness = bt_effect2_body->welding_II.base.brightness;
                welding_2.min_lightness = bt_effect2_body->welding_II.base.min_brightness;
                welding_2.frq = bt_effect2_body->welding_II.base.frq;
            }
            else if(bt_effect2_body->welding_II.base.package_type == 1)
            {
				welding_2.mode = (enum fx_mode)bt_effect2_body->welding_II.pixel.cct.light_mode;
				
                switch(bt_effect2_body->welding_II.pixel.cct.light_mode)
                {
                    case BT_FX_II_Mode_CCT:
                        welding_2.mode_arg.cct.cct = bt_effect2_body->welding_II.pixel.cct.cct * 50;
                        welding_2.mode_arg.cct.duv = bt_effect2_body->welding_II.pixel.cct.duv - 100;
						break;
                    case BT_FX_II_Mode_HSI:
                        welding_2.mode_arg.hsi.sat = bt_effect2_body->welding_II.pixel.hsi.sat ;
                        welding_2.mode_arg.hsi.hue = bt_effect2_body->welding_II.pixel.hsi.hue ;
                        welding_2.mode_arg.hsi.cct = bt_effect2_body->welding_II.pixel.hsi.cct * 50;
						break;
                    case BT_FX_II_Mode_GEL:
                        welding_2.mode_arg.gel.brand = bt_effect2_body->welding_II.pixel.gel.origin== 0 ? 1 : 0;
						origin = bt_effect2_body->welding_II.pixel.gel.origin== 0 ? 1 : 0;
                        welding_2.mode_arg.gel.type[origin] = bt_effect2_body->welding_II.pixel.gel.type;
						gtype = bt_effect2_body->welding_II.pixel.gel.type;
                        welding_2.mode_arg.gel.color[origin][gtype] = bt_effect2_body->welding_II.pixel.gel.color;
                        welding_2.mode_arg.gel.cct = bt_effect2_body->welding_II.pixel.gel.cct * 50;
						break;
                    case BT_FX_II_Mode_XY:
                        welding_2.mode_arg.xy.x = bt_effect2_body->welding_II.pixel.xy.x;
                        welding_2.mode_arg.xy.y = bt_effect2_body->welding_II.pixel.xy.y;
						break;
                    case BT_FX_II_Mode_SOUYRCE:
                        welding_2.mode_arg.source.type = bt_effect2_body->welding_II.pixel.source.type;
                        welding_2.mode_arg.source.x = bt_effect2_body->welding_II.pixel.source.x;
                        welding_2.mode_arg.source.y = bt_effect2_body->welding_II.pixel.source.y;
						break;
                    case BT_FX_II_Mode_RGB:
                        welding_2.mode_arg.rgb.r = bt_effect2_body->welding_II.pixel.rgb.r;
                        welding_2.mode_arg.rgb.g = bt_effect2_body->welding_II.pixel.rgb.g;
                        welding_2.mode_arg.rgb.b = bt_effect2_body->welding_II.pixel.rgb.b;
						break;

                    default: break;
                }
            }
			if(bt_effect2_body->welding_II.base.state != 3)
                data_center_write_light_data(LIGHT_MODE_FX_WELDING_II, &welding_2);
			break;
        }
		case BT_FX_II_Cop_Car_2:  // 
		{
            struct db_fx_cop_car_2 cop_car_2;
            cop_car_2.state = bt_effect2_body->cop_car_II.state;
            cop_car_2.lightness = bt_effect2_body->cop_car_II.brightness; 
            cop_car_2.frq = bt_effect2_body->cop_car_II.frq;
            cop_car_2.color = bt_effect2_body->cop_car_II.color; 
            data_center_write_light_data(LIGHT_MODE_FX_COP_CAR_II, &cop_car_2);
			break;
        }
        case BT_FX_II_Party_Lights_2:
        {
            struct db_fx_party_lights_2 party_lights_2;
            party_lights_2.state = bt_effect2_body->party_lights_II.state;
            party_lights_2.lightness = bt_effect2_body->party_lights_II.brightness; 
            party_lights_2.sat = bt_effect2_body->party_lights_II.sat * 10;
            party_lights_2.spd = bt_effect2_body->party_lights_II.spd * 1000; 
            data_center_write_light_data(LIGHT_MODE_FX_PARTY_LIGHTS_II, &party_lights_2);
			break;
        }
        case BT_FX_II_Fireworks_2:
        {
            struct db_fx_fireworks_2 fireworks_2;
            fireworks_2.state = bt_effect2_body->fireworks_II.state;
            fireworks_2.mode = (enum fx_mode)bt_effect2_body->fireworks_II.mode;
            fireworks_2.lightness = bt_effect2_body->fireworks_II.brightness;
            fireworks_2.min_gap_time = bt_effect2_body->fireworks_II.min_gap_time * 100;  // ms
            fireworks_2.gap_time = bt_effect2_body->fireworks_II.gap_time * 100;		  // ms
            data_center_write_light_data(LIGHT_MODE_FX_FIREWORKS_II, &fireworks_2);
			break;
		}
        case BT_FX_II_Lightning_3:
        {
            struct db_fx_lightning_3 lightning_3;
			lightning_3.state = bt_effect2_body->lightning_III.base.state;
            if(bt_effect2_body->lightning_III.base.package_type == 0)
            {
				lightning_3.lightness = bt_effect2_body->lightning_III.base.brightness;
                lightning_3.gap_time = bt_effect2_body->lightning_III.base.gap_time * 100;
                lightning_3.min_gap_time = bt_effect2_body->lightning_III.base.min_gap_time * 100;
            }
            else if(bt_effect2_body->lightning_III.base.package_type == 1)
            {
                lightning_3.mode = (enum fx_mode)bt_effect2_body->lightning_III.pixel.cct.light_mode;
                
                switch(bt_effect2_body->lightning_III.pixel.cct.light_mode)
                {
                    case BT_FX_II_Mode_CCT:
                        lightning_3.mode_arg.cct.cct = bt_effect2_body->lightning_III.pixel.cct.cct * 50;
                        lightning_3.mode_arg.cct.duv = bt_effect2_body->lightning_III.pixel.cct.duv * 10 - 100;
						break;
                    case BT_FX_II_Mode_HSI:
                        lightning_3.mode_arg.hsi.sat = bt_effect2_body->lightning_III.pixel.hsi.sat * 10;
                        lightning_3.mode_arg.hsi.hue = bt_effect2_body->lightning_III.pixel.hsi.hue * 10;
                        lightning_3.mode_arg.hsi.cct = bt_effect2_body->lightning_III.pixel.hsi.cct * 50;
						break;
                    case BT_FX_II_Mode_GEL:
                        lightning_3.mode_arg.gel.brand = bt_effect2_body->lightning_III.pixel.gel.origin;
						origin = bt_effect2_body->lightning_III.pixel.gel.origin;
                        lightning_3.mode_arg.gel.type[origin] = bt_effect2_body->lightning_III.pixel.gel.type;
						gtype = bt_effect2_body->lightning_III.pixel.gel.type;
                        lightning_3.mode_arg.gel.color[origin][gtype] = bt_effect2_body->lightning_III.pixel.gel.color;
                        lightning_3.mode_arg.gel.cct = bt_effect2_body->lightning_III.pixel.gel.cct * 50;
						break;
                    case BT_FX_II_Mode_XY:
                        lightning_3.mode_arg.xy.x = bt_effect2_body->lightning_III.pixel.xy.x;
                        lightning_3.mode_arg.xy.y = bt_effect2_body->lightning_III.pixel.xy.y;
						break;
                    case BT_FX_II_Mode_SOUYRCE:
                        lightning_3.mode_arg.source.type = bt_effect2_body->lightning_III.pixel.source.type;
                        lightning_3.mode_arg.source.x = bt_effect2_body->lightning_III.pixel.source.x;
                        lightning_3.mode_arg.source.y = bt_effect2_body->lightning_III.pixel.source.y;
						break;
                    case BT_FX_II_Mode_RGB:
                        lightning_3.mode_arg.rgb.r = bt_effect2_body->lightning_III.pixel.rgb.r;
                        lightning_3.mode_arg.rgb.g = bt_effect2_body->lightning_III.pixel.rgb.g;
                        lightning_3.mode_arg.rgb.b = bt_effect2_body->lightning_III.pixel.rgb.b;
						break;

                    default: break;
                }
            }
			if(bt_effect2_body->lightning_III.base.state != 3)
                data_center_write_light_data(LIGHT_MODE_FX_LIGHTNING_III, &lightning_3);
			break;
        }
        case BT_FX_II_TV_3:
        {
            struct db_fx_tv_3 tv_3;
            tv_3.state = bt_effect2_body->tv_III.base.state;
            if(bt_effect2_body->tv_III.base.package_type == 0)
            {
				tv_3.lightness = bt_effect2_body->tv_III.base.brightness;
                tv_3.min_gap_time = bt_effect2_body->tv_III.base.min_gap_time * 100;
                tv_3.gap_time = bt_effect2_body->tv_III.base.gap_time * 100;
            }
            else if(bt_effect2_body->tv_III.base.package_type == 1)
            {
                tv_3.mode = (enum fx_mode)bt_effect2_body->tv_III.pixel.cct.light_mode;
                switch(bt_effect2_body->tv_III.pixel.cct.light_mode)
                {
                    case BT_FX_II_Mode_CCT:
                        tv_3.mode_arg.cct.max_cct = bt_effect2_body->tv_III.pixel.cct.max_cct * 50;
                        tv_3.mode_arg.cct.min_cct = bt_effect2_body->tv_III.pixel.cct.min_cct * 50;
                        tv_3.mode_arg.cct.duv = bt_effect2_body->tv_III.pixel.cct.duv * 10 - 100;
						break;
                    case BT_FX_II_Mode_HSI:
						tv_3.mode_arg.hsi.sat = bt_effect2_body->tv_III.pixel.hsi.sat * 10;
                        tv_3.mode_arg.hsi.max_hue = bt_effect2_body->tv_III.pixel.hsi.max_hue * 10;
						tv_3.mode_arg.hsi.min_hue = bt_effect2_body->tv_III.pixel.hsi.min_hue * 10;
                        tv_3.mode_arg.hsi.cct = bt_effect2_body->tv_III.pixel.hsi.cct * 50;
						break;

                    default: break;
                }
			}
			if(bt_effect2_body->tv_III.base.state != 3)
                data_center_write_light_data(LIGHT_MODE_FX_TV_III, &tv_3);
			break;
        }
        case BT_FX_II_Fire_3:
        {
            struct db_fx_fire_3 fire_3;
            fire_3.state = bt_effect2_body->fire_III.base.state;
            if(bt_effect2_body->fire_III.base.package_type == 0)
            {
				fire_3.lightness = bt_effect2_body->fire_III.base.brightness;
                fire_3.frq = bt_effect2_body->fire_III.base.frq;
            }
            else if(bt_effect2_body->fire_III.base.package_type == 1)
            {
                fire_3.mode = (enum fx_mode)bt_effect2_body->fire_III.pixel.cct.light_mode;

                switch(bt_effect2_body->fire_III.pixel.cct.light_mode)
                {
                    case BT_FX_II_Mode_CCT:
                        fire_3.mode_arg.cct.min_cct = bt_effect2_body->fire_III.pixel.cct.min_cct * 50;
                        fire_3.mode_arg.cct.max_cct = bt_effect2_body->fire_III.pixel.cct.max_cct * 50;
                        fire_3.mode_arg.cct.duv = bt_effect2_body->fire_III.pixel.cct.duv * 10 - 100;
						break;

                    case BT_FX_II_Mode_HSI:
                        fire_3.mode_arg.hsi.min_hue = bt_effect2_body->fire_III.pixel.hsi.min_hue * 10;
                        fire_3.mode_arg.hsi.max_hue = bt_effect2_body->fire_III.pixel.hsi.max_hue * 10;
						fire_3.mode_arg.hsi.sat = bt_effect2_body->fire_III.pixel.hsi.sat * 10;
                        fire_3.mode_arg.hsi.cct = bt_effect2_body->fire_III.pixel.hsi.cct * 50;
						break;

                    default: break;
                }
            }
			if(bt_effect2_body->fire_III.base.state != 3)
                data_center_write_light_data(LIGHT_MODE_FX_FIRE_III, &fire_3);
			break;
        }
        case BT_FX_II_Faulty_Bulb_3:
        {
            struct db_fx_fault_bulb_3 fault_bulb_3;
            fault_bulb_3.state = bt_effect2_body->fault_bulb_III.base.state;
            if(bt_effect2_body->fault_bulb_III.base.package_type == 0)
            {
				fault_bulb_3.lightness = bt_effect2_body->fault_bulb_III.base.brightness;
                fault_bulb_3.gap_time = bt_effect2_body->fault_bulb_III.base.gap_time * 100;
                fault_bulb_3.min_gap_time = bt_effect2_body->fault_bulb_III.base.min_gap_time * 100;
            }
            else if(bt_effect2_body->fault_bulb_III.base.package_type == 1)
            {
                fault_bulb_3.mode = (enum fx_mode)bt_effect2_body->fault_bulb_III.pixel.cct.light_mode;

                switch(bt_effect2_body->fault_bulb_III.pixel.cct.light_mode)
                {
                    case BT_FX_II_Mode_CCT:
                        fault_bulb_3.mode_arg.cct.cct = bt_effect2_body->fault_bulb_III.pixel.cct.cct * 50;
                        fault_bulb_3.mode_arg.cct.duv = bt_effect2_body->fault_bulb_III.pixel.cct.duv * 10 - 100;
						break;
                    case BT_FX_II_Mode_HSI:
                        fault_bulb_3.mode_arg.hsi.sat = bt_effect2_body->fault_bulb_III.pixel.hsi.sat * 10;
                        fault_bulb_3.mode_arg.hsi.hue = bt_effect2_body->fault_bulb_III.pixel.hsi.hue * 10;
                        fault_bulb_3.mode_arg.hsi.cct = bt_effect2_body->fault_bulb_III.pixel.hsi.cct * 50;
						break;
                    case BT_FX_II_Mode_GEL:
                        fault_bulb_3.mode_arg.gel.brand = bt_effect2_body->fault_bulb_III.pixel.gel.origin;
						origin = bt_effect2_body->fault_bulb_III.pixel.gel.origin;
                        fault_bulb_3.mode_arg.gel.type[origin] = bt_effect2_body->fault_bulb_III.pixel.gel.type;
						gtype = bt_effect2_body->fault_bulb_III.pixel.gel.type;
                        fault_bulb_3.mode_arg.gel.color[origin][gtype] = bt_effect2_body->fault_bulb_III.pixel.gel.color;
                        fault_bulb_3.mode_arg.gel.cct = bt_effect2_body->fault_bulb_III.pixel.gel.cct * 50;
						break;
                    case BT_FX_II_Mode_XY:
                        fault_bulb_3.mode_arg.xy.x = bt_effect2_body->fault_bulb_III.pixel.xy.x;
                        fault_bulb_3.mode_arg.xy.y = bt_effect2_body->fault_bulb_III.pixel.xy.y;
						break;
                    case BT_FX_II_Mode_SOUYRCE:
                        fault_bulb_3.mode_arg.source.type = bt_effect2_body->fault_bulb_III.pixel.source.type;
                        fault_bulb_3.mode_arg.source.x = bt_effect2_body->fault_bulb_III.pixel.source.x;
                        fault_bulb_3.mode_arg.source.y = bt_effect2_body->fault_bulb_III.pixel.source.y;
						break;
                    case BT_FX_II_Mode_RGB:
                        fault_bulb_3.mode_arg.rgb.r = bt_effect2_body->fault_bulb_III.pixel.rgb.r;
                        fault_bulb_3.mode_arg.rgb.g = bt_effect2_body->fault_bulb_III.pixel.rgb.g;
                        fault_bulb_3.mode_arg.rgb.b = bt_effect2_body->fault_bulb_III.pixel.rgb.b;
						break;

                    default: break;
                }
            }
			if(bt_effect2_body->fault_bulb_III.base.state != 3)
                data_center_write_light_data(LIGHT_MODE_FX_FAULTY_BULB_III, &fault_bulb_3);
			break;
        }
        case BT_FX_II_Pulsing_3:
        {
            struct db_fx_pulsing_3 pulsing_3;
            pulsing_3.state = bt_effect2_body->pulsing_III.cct.state;
            pulsing_3.lightness = bt_effect2_body->pulsing_III.cct.brightness;
            pulsing_3.frq = bt_effect2_body->pulsing_III.cct.frq;
            pulsing_3.mode = (enum fx_mode)bt_effect2_body->pulsing_III.cct.light_mode;
            
            switch(bt_effect2_body->pulsing_III.cct.light_mode)
            {
				case BT_FX_II_Mode_CCT:
					pulsing_3.mode_arg.cct.cct = bt_effect2_body->pulsing_III.cct.cct * 50;
					pulsing_3.mode_arg.cct.duv = bt_effect2_body->pulsing_III.cct.duv * 10 - 100;
					break;
				case BT_FX_II_Mode_HSI:
					pulsing_3.mode_arg.hsi.hue = bt_effect2_body->pulsing_III.hsi.hue * 10;
					pulsing_3.mode_arg.hsi.sat = bt_effect2_body->pulsing_III.hsi.sat * 10;
					pulsing_3.mode_arg.hsi.cct = bt_effect2_body->pulsing_III.hsi.cct * 50;
					break;
				case BT_FX_II_Mode_GEL:
					pulsing_3.mode_arg.gel.brand = bt_effect2_body->pulsing_III.gel.origin;
					origin = bt_effect2_body->pulsing_III.gel.origin;
					pulsing_3.mode_arg.gel.type[origin] = bt_effect2_body->pulsing_III.gel.type;
					gtype = bt_effect2_body->pulsing_III.gel.type;
					pulsing_3.mode_arg.gel.color[origin][gtype] = bt_effect2_body->pulsing_III.gel.color;
					pulsing_3.mode_arg.gel.cct = bt_effect2_body->pulsing_III.gel.cct * 50;
					break;
				case BT_FX_II_Mode_XY:
					pulsing_3.mode_arg.xy.x = bt_effect2_body->pulsing_III.xy.x;
					pulsing_3.mode_arg.xy.y = bt_effect2_body->pulsing_III.xy.y;
					break;
				case BT_FX_II_Mode_SOUYRCE:
					pulsing_3.mode_arg.source.type = bt_effect2_body->pulsing_III.source.type;
					pulsing_3.mode_arg.source.x = bt_effect2_body->pulsing_III.source.x;
					pulsing_3.mode_arg.source.y = bt_effect2_body->pulsing_III.source.y;
					break;
				case BT_FX_II_Mode_RGB:
					pulsing_3.mode_arg.rgb.r = bt_effect2_body->pulsing_III.rgb.r;
					pulsing_3.mode_arg.rgb.g = bt_effect2_body->pulsing_III.rgb.g;
					pulsing_3.mode_arg.rgb.b = bt_effect2_body->pulsing_III.rgb.b;
					break;
            }
            data_center_write_light_data(LIGHT_MODE_FX_PULSING_III, &pulsing_3);
			break;
        }
        case BT_FX_II_Cop_Car_3:
        {
            struct db_fx_cop_car_3 cop_car_3;
            cop_car_3.state = bt_effect2_body->cop_car_III.state;
            cop_car_3.lightness = bt_effect2_body->cop_car_III.brightness;
            cop_car_3.color = bt_effect2_body->cop_car_III.color;
            cop_car_3.frq = bt_effect2_body->cop_car_III.frq;
            data_center_write_light_data(LIGHT_MODE_FX_COP_CAR_III, &cop_car_3);
			break;
        }
        
		default: break;
	}
}

/* 写系统光效 */
static void write_parse_sys_effect(BT_Body_TypeDef *write_body)
{
	BT_Effect_Mode_Body_TypeDef *bt_effect_body = NULL;
	bt_effect_body = &write_body->Light_Effect_Body;
	uint8_t origin = 0;
	uint8_t gtype = 0;
	
	switch(bt_effect_body->Club_Lights_Arg.Effect_Type)
	{
		case BT_EffectType_ClubLights:
        {
            struct db_fx_club_lights club_lights;
			club_lights.spd = bt_effect_body->Club_Lights_Arg.Frq;
			club_lights.lightness = bt_effect_body->Club_Lights_Arg.Int;
			club_lights.color = bt_effect_body->Club_Lights_Arg.Color;
            club_lights.state = 1;
            data_center_write_light_data(LIGHT_MODE_FX_CLUBLIGHTS, &club_lights);
			break;
        }
		case BT_EffectType_Paparazzi:
        {
            struct db_fx_paparazzi paparazzi;
			paparazzi.frq = bt_effect_body->Paparazzi_Arg.Frq;
			paparazzi.lightness = bt_effect_body->Paparazzi_Arg.Int;        	// 蓝牙值为0-1000
			paparazzi.cct = bt_effect_body->Paparazzi_Arg.CCT * 10;				// 蓝牙值为320-650
			paparazzi.gm = bt_effect_body->Paparazzi_Arg.GM * 10 - 100;
            paparazzi.state = 1;
			#if PROJECT_TYPE==308
			g_tUIAllData.paparazzi_model.state = 1;
			#endif
            data_center_write_light_data(LIGHT_MODE_FX_PAPARAZZI, &paparazzi);
			break;
        }
		case BT_EffectType_Lightning:
        {
            struct db_fx_lightning lightning;
			lightning.frq = bt_effect_body->Lightning_Arg.Frq;
			lightning.lightness = bt_effect_body->Lightning_Arg.Int;
			lightning.cct = bt_effect_body->Lightning_Arg.CCT * 10;
			lightning.gm = bt_effect_body->Lightning_Arg.GM * 10 - 100;
			lightning.trigger = bt_effect_body->Lightning_Arg.Trigger;
			lightning.speed = bt_effect_body->Lightning_Arg.Speed;
			lightning.state = 1;
			#if PROJECT_TYPE==308
			g_tUIAllData.lightning_model.state = 1;
			#endif
			if(lightning.trigger == 1)
			{
				ui_set_Lightning_2_state(1);
				if(ui_get_master_mode())
					gui_data_sync_event_gen();
				page_set_invalid_type(PAGE_INVALID_TYPE_ALL);
			}
			#if PROJECT_TYPE == 308
			if(PAGE_LIGHTNING == screen_get_act_pid())
				data_center_write_light_data(LIGHT_MODE_FX_LIGHTNING, &lightning);
			else
				data_center_write_light_data_no_event(LIGHT_MODE_FX_LIGHTNING, &lightning);
			break;
			#elif PROJECT_TYPE == 307
				data_center_write_light_data(LIGHT_MODE_FX_LIGHTNING, &lightning);
			#endif
			break;
        }
		case BT_EffectType_TV:
        {
            struct db_fx_tv tv;
			tv.frq = bt_effect_body->TV_Arg.Frq;
			tv.lightness = bt_effect_body->TV_Arg.Int;
			tv.cct = bt_effect_body->TV_Arg.CCT;     				// CCT只表示挡位，012对应123档
            tv.state = 1;
			#if PROJECT_TYPE==308
			g_tUIAllData.tv_model.state = 1;
			#endif
            data_center_write_light_data(LIGHT_MODE_FX_TV, &tv);
			break;
        }
		case BT_EffectType_Candle:
        {
            struct db_fx_candle candle;
			candle.spd = bt_effect_body->Candle_Arg.Frq;
			candle.lightness = bt_effect_body->Candle_Arg.Int;
			candle.cct = bt_effect_body->Candle_Arg.CCT;			// CCT只表示挡位，012对应123档
            candle.state = 1;
            data_center_write_light_data(LIGHT_MODE_FX_CANDLE, &candle);
			break;
        }
		case BT_EffectType_Fire:
        {
            struct db_fx_fire fire;
			fire.frq = bt_effect_body->Fire_Arg.Frq;
			fire.lightness = bt_effect_body->Fire_Arg.Int;   
			fire.cct = bt_effect_body->Fire_Arg.CCT;        		// CCT只表示挡位，012对应123档
            fire.state = 1;
			#if PROJECT_TYPE==308
			g_tUIAllData.fire_model.state = 1;
			#endif
            data_center_write_light_data(LIGHT_MODE_FX_FIRE, &fire);
			break;
        }
		case BT_EffectType_Strobe:
        {
            struct db_fx_strobe strobe;
			data_center_read_light_data(LIGHT_MODE_FX_STROBE, &strobe); 
			strobe.mode = get_ble_effectmodeback((enum fx_mode)bt_effect_body->Strobe_Arg.Mode.Effect_CCT_Mode.Effect_Mode);
            strobe.state = 1;
			#if PROJECT_TYPE==308
			g_tUIAllData.strobe_model.state = 1;
			#endif
			switch (bt_effect_body->Strobe_Arg.Mode.Effect_CCT_Mode.Effect_Mode)
			{
				case EffectModeCCT:
					strobe.lightness = bt_effect_body->Strobe_Arg.Mode.Effect_CCT_Mode.Int; 
					strobe.mode_arg.cct.cct = bt_effect_body->Strobe_Arg.Mode.Effect_CCT_Mode.CCT * 10;
					strobe.mode_arg.cct.duv = bt_effect_body->Strobe_Arg.Mode.Effect_CCT_Mode.GM * 10 - 100;
					strobe.frq = bt_effect_body->Strobe_Arg.Mode.Effect_CCT_Mode.Frq;
					break;
				case EffectModeHSI:
					strobe.lightness = bt_effect_body->Strobe_Arg.Mode.Effect_HSI_Mode.Int; 
					strobe.mode_arg.hsi.hue = bt_effect_body->Strobe_Arg.Mode.Effect_HSI_Mode.Hue;
					strobe.mode_arg.hsi.sat = bt_effect_body->Strobe_Arg.Mode.Effect_HSI_Mode.Sat;
					strobe.frq = bt_effect_body->Strobe_Arg.Mode.Effect_HSI_Mode.Frq;
					break;
				case EffectModeGEL:
					strobe.lightness = bt_effect_body->Strobe_Arg.Mode.Effect_Gel_Mode.Int; 
					strobe.mode_arg.gel.cct = bt_effect_body->Strobe_Arg.Mode.Effect_Gel_Mode.CCT * 10;
					strobe.mode_arg.gel.brand = bt_effect_body->Strobe_Arg.Mode.Effect_Gel_Mode.Origin== 0 ? 1 : 0;
					origin = bt_effect_body->Strobe_Arg.Mode.Effect_Gel_Mode.Origin== 0 ? 1 : 0;
					strobe.mode_arg.gel.type[origin] = bt_effect_body->Strobe_Arg.Mode.Effect_Gel_Mode.Type;
					gtype = bt_effect_body->Strobe_Arg.Mode.Effect_Gel_Mode.Type;
					strobe.mode_arg.gel.color[origin][gtype] = bt_effect_body->Strobe_Arg.Mode.Effect_Gel_Mode.Color;
					strobe.frq = bt_effect_body->Strobe_Arg.Mode.Effect_Gel_Mode.Frq;
					break;
				case EffectModeCoord:
					strobe.lightness = bt_effect_body->Strobe_Arg.Mode.Effect_XY_Mode.Int; 
					strobe.mode_arg.xy.x = bt_effect_body->Strobe_Arg.Mode.Effect_XY_Mode.Coordinate_x;
					strobe.mode_arg.xy.y = bt_effect_body->Strobe_Arg.Mode.Effect_XY_Mode.Coordinate_y;
					strobe.frq = bt_effect_body->Strobe_Arg.Mode.Effect_XY_Mode.Frq;
					break;

				default: break;
			}
            data_center_write_light_data(LIGHT_MODE_FX_STROBE, &strobe);
			break;
        }
		case BT_EffectType_Explosion:
        {
            struct db_fx_explosion explosion;
			data_center_read_light_data(LIGHT_MODE_FX_EXPLOSION, &explosion); 
			explosion.mode = get_ble_effectmodeback((enum fx_mode)bt_effect_body->Explosion_Arg.Mode.Effect_CCT_Mode.Effect_Mode);
			explosion.state = 1;
			#if PROJECT_TYPE==308
			g_tUIAllData.explosion_model.state = 1;
			#endif
			switch (bt_effect_body->Explosion_Arg.Mode.Effect_CCT_Mode.Effect_Mode)
			{
				case EffectModeCCT:
					explosion.lightness = bt_effect_body->Explosion_Arg.Mode.Effect_CCT_Mode.Int; 
					explosion.mode_arg.cct.cct = bt_effect_body->Explosion_Arg.Mode.Effect_CCT_Mode.CCT * 10;
					explosion.mode_arg.cct.duv = bt_effect_body->Explosion_Arg.Mode.Effect_CCT_Mode.GM * 10 - 100;
					explosion.frq = bt_effect_body->Explosion_Arg.Mode.Effect_CCT_Mode.Frq;
					explosion.trigger = bt_effect_body->Explosion_Arg.Mode.Effect_CCT_Mode.Trigger;
					break;
				case EffectModeHSI:
					explosion.lightness = bt_effect_body->Explosion_Arg.Mode.Effect_HSI_Mode.Int; 
					explosion.mode_arg.hsi.hue = bt_effect_body->Explosion_Arg.Mode.Effect_HSI_Mode.Hue ;
					explosion.mode_arg.hsi.sat = bt_effect_body->Explosion_Arg.Mode.Effect_HSI_Mode.Sat ;
					explosion.frq = bt_effect_body->Explosion_Arg.Mode.Effect_HSI_Mode.Frq;
					explosion.trigger = bt_effect_body->Explosion_Arg.Mode.Effect_HSI_Mode.Trigger;
					break;
				case EffectModeGEL:
					explosion.lightness = bt_effect_body->Explosion_Arg.Mode.Effect_Gel_Mode.Int; 
					explosion.mode_arg.gel.cct = bt_effect_body->Explosion_Arg.Mode.Effect_Gel_Mode.CCT * 10;
					explosion.mode_arg.gel.brand = bt_effect_body->Explosion_Arg.Mode.Effect_Gel_Mode.Origin== 0 ? 1 : 0;
					origin = bt_effect_body->Explosion_Arg.Mode.Effect_Gel_Mode.Origin== 0 ? 1 : 0;
					explosion.mode_arg.gel.type[origin] = bt_effect_body->Explosion_Arg.Mode.Effect_Gel_Mode.Type;
					gtype = bt_effect_body->Explosion_Arg.Mode.Effect_Gel_Mode.Type;
					explosion.mode_arg.gel.color[origin][gtype] = bt_effect_body->Explosion_Arg.Mode.Effect_Gel_Mode.Color;
					explosion.frq = bt_effect_body->Explosion_Arg.Mode.Effect_Gel_Mode.Frq;
					explosion.trigger = bt_effect_body->Explosion_Arg.Mode.Effect_Gel_Mode.Trigger;
					break;
				case EffectModeCoord:
					explosion.lightness = bt_effect_body->Explosion_Arg.Mode.Effect_XY_Mode.Int; 
					explosion.mode_arg.xy.x = bt_effect_body->Explosion_Arg.Mode.Effect_XY_Mode.Coordinate_x;
					explosion.mode_arg.xy.y = bt_effect_body->Explosion_Arg.Mode.Effect_XY_Mode.Coordinate_y;
					explosion.frq = bt_effect_body->Explosion_Arg.Mode.Effect_XY_Mode.Frq;
					explosion.trigger = bt_effect_body->Explosion_Arg.Mode.Effect_XY_Mode.Trigger;
					break;
				default: break;
			}
			if(explosion.trigger == 1)
			{
				if(ui_get_master_mode())
					gui_data_sync_event_gen();
				ui_set_explosion_2_anim(1);
			}
			#if PROJECT_TYPE == 308
				if(PAGE_EXPLOSIOIN == screen_get_act_pid())
				data_center_write_light_data(LIGHT_MODE_FX_EXPLOSION, &explosion);
			else
				data_center_write_light_data_no_event(LIGHT_MODE_FX_EXPLOSION, &explosion);
			break;
			#elif PROJECT_TYPE == 307
				data_center_write_light_data(LIGHT_MODE_FX_EXPLOSION, &explosion);
			#endif
			break;
	
        }
		case BT_EffectType_FaultBulb:
        {
            struct db_fx_fault_bulb fault_bulb;
			data_center_read_light_data(LIGHT_MODE_FX_FAULT_BULB, &fault_bulb); 
			fault_bulb.mode = get_ble_effectmodeback((enum fx_mode)bt_effect_body->Fault_Bulb_Arg.Mode.Effect_CCT_Mode.Effect_Mode);
			fault_bulb.state = 1;
			#if PROJECT_TYPE==308
			g_tUIAllData.faultybulb_model.state = 1;
			#endif
			switch(bt_effect_body->Fault_Bulb_Arg.Mode.Effect_CCT_Mode.Effect_Mode)
			{
				case EffectModeCCT:
					fault_bulb.lightness = bt_effect_body->Fault_Bulb_Arg.Mode.Effect_CCT_Mode.Int;
					fault_bulb.mode_arg.cct.cct = bt_effect_body->Fault_Bulb_Arg.Mode.Effect_CCT_Mode.CCT * 10;
				    fault_bulb.mode_arg.cct.duv = bt_effect_body->Fault_Bulb_Arg.Mode.Effect_CCT_Mode.GM * 10 - 100;
					fault_bulb.frq = bt_effect_body->Fault_Bulb_Arg.Mode.Effect_CCT_Mode.Frq;
				    fault_bulb.speed = bt_effect_body->Fault_Bulb_Arg.Mode.Effect_CCT_Mode.Speed;
					break;
				case EffectModeHSI:
					fault_bulb.lightness = bt_effect_body->Fault_Bulb_Arg.Mode.Effect_HSI_Mode.Int;
					fault_bulb.mode_arg.hsi.sat = bt_effect_body->Fault_Bulb_Arg.Mode.Effect_HSI_Mode.Sat;
					fault_bulb.mode_arg.hsi.hue = bt_effect_body->Fault_Bulb_Arg.Mode.Effect_HSI_Mode.Hue ;
					fault_bulb.frq = bt_effect_body->Fault_Bulb_Arg.Mode.Effect_HSI_Mode.Frq;
				    fault_bulb.speed = bt_effect_body->Fault_Bulb_Arg.Mode.Effect_HSI_Mode.Speed;
					break;
				case EffectModeGEL:
					fault_bulb.lightness = bt_effect_body->Fault_Bulb_Arg.Mode.Effect_Gel_Mode.Int; 
					fault_bulb.mode_arg.gel.cct = bt_effect_body->Fault_Bulb_Arg.Mode.Effect_Gel_Mode.CCT * 10;
					fault_bulb.mode_arg.gel.brand = bt_effect_body->Fault_Bulb_Arg.Mode.Effect_Gel_Mode.Origin== 0 ? 1 : 0;
					origin = bt_effect_body->Fault_Bulb_Arg.Mode.Effect_Gel_Mode.Origin== 0 ? 1 : 0;
				    fault_bulb.mode_arg.gel.type[origin] = bt_effect_body->Fault_Bulb_Arg.Mode.Effect_Gel_Mode.Type;
					gtype = bt_effect_body->Fault_Bulb_Arg.Mode.Effect_Gel_Mode.Type;
					fault_bulb.mode_arg.gel.color[origin][gtype] = bt_effect_body->Fault_Bulb_Arg.Mode.Effect_Gel_Mode.Color;
				    fault_bulb.frq = bt_effect_body->Fault_Bulb_Arg.Mode.Effect_Gel_Mode.Frq;
				    fault_bulb.speed = bt_effect_body->Fault_Bulb_Arg.Mode.Effect_Gel_Mode.Speed;
				    break;
				case EffectModeCoord:
					fault_bulb.lightness = bt_effect_body->Fault_Bulb_Arg.Mode.Effect_XY_Mode.Int; 
					fault_bulb.mode_arg.xy.x = bt_effect_body->Fault_Bulb_Arg.Mode.Effect_XY_Mode.Coordinate_x;
				    fault_bulb.mode_arg.xy.y = bt_effect_body->Fault_Bulb_Arg.Mode.Effect_XY_Mode.Coordinate_y;
				    fault_bulb.frq = bt_effect_body->Fault_Bulb_Arg.Mode.Effect_XY_Mode.Frq;
				    fault_bulb.speed = bt_effect_body->Fault_Bulb_Arg.Mode.Effect_XY_Mode.Speed;
					break;
				
				default: break;
			}
            data_center_write_light_data(LIGHT_MODE_FX_FAULT_BULB, &fault_bulb);
			break;
        }
		case BT_EffectType_Pulsing:
        {
            struct db_fx_pulsing pulsing;
			data_center_read_light_data(LIGHT_MODE_FX_PULSING, &pulsing); 
			pulsing.mode = get_ble_effectmodeback((enum fx_mode)bt_effect_body->Pulsing_Arg.Mode.Effect_CCT_Mode.Effect_Mode);
			pulsing.state = 1;
			#if PROJECT_TYPE==308
			g_tUIAllData.pulsing_model.state = 1;
			#endif
			switch(bt_effect_body->Pulsing_Arg.Mode.Effect_CCT_Mode.Effect_Mode)
			{
				case EffectModeCCT:
					pulsing.lightness = bt_effect_body->Pulsing_Arg.Mode.Effect_CCT_Mode.Int;
					pulsing.mode_arg.cct.cct = bt_effect_body->Pulsing_Arg.Mode.Effect_CCT_Mode.CCT * 10;
					pulsing.mode_arg.cct.duv = bt_effect_body->Pulsing_Arg.Mode.Effect_CCT_Mode.GM * 10 - 100;
					pulsing.frq = bt_effect_body->Pulsing_Arg.Mode.Effect_CCT_Mode.Frq;
					pulsing.speed = bt_effect_body->Pulsing_Arg.Mode.Effect_CCT_Mode.Speed;
					break;
				case EffectModeHSI:
					pulsing.lightness = bt_effect_body->Pulsing_Arg.Mode.Effect_HSI_Mode.Int;
					pulsing.mode_arg.hsi.sat = bt_effect_body->Pulsing_Arg.Mode.Effect_HSI_Mode.Sat ;
					pulsing.mode_arg.hsi.hue = bt_effect_body->Pulsing_Arg.Mode.Effect_HSI_Mode.Hue ;
					pulsing.frq = bt_effect_body->Pulsing_Arg.Mode.Effect_HSI_Mode.Frq;
					pulsing.speed = bt_effect_body->Pulsing_Arg.Mode.Effect_HSI_Mode.Speed;
					break;
				case EffectModeGEL:
					pulsing.lightness = bt_effect_body->Pulsing_Arg.Mode.Effect_Gel_Mode.Int; 
					pulsing.mode_arg.gel.cct = bt_effect_body->Pulsing_Arg.Mode.Effect_Gel_Mode.CCT * 10;
					pulsing.mode_arg.gel.brand = bt_effect_body->Pulsing_Arg.Mode.Effect_Gel_Mode.Origin== 0 ? 1 : 0;
					origin = bt_effect_body->Pulsing_Arg.Mode.Effect_Gel_Mode.Origin== 0 ? 1 : 0;
					pulsing.mode_arg.gel.type[origin] = bt_effect_body->Pulsing_Arg.Mode.Effect_Gel_Mode.Type;
					gtype = bt_effect_body->Pulsing_Arg.Mode.Effect_Gel_Mode.Type;
					pulsing.mode_arg.gel.color[origin][gtype] = bt_effect_body->Pulsing_Arg.Mode.Effect_Gel_Mode.Color;
					pulsing.frq = bt_effect_body->Pulsing_Arg.Mode.Effect_Gel_Mode.Frq;
					pulsing.speed = bt_effect_body->Pulsing_Arg.Mode.Effect_Gel_Mode.Speed;
					break;
				case EffectModeCoord:
					pulsing.lightness = bt_effect_body->Pulsing_Arg.Mode.Effect_XY_Mode.Int; 
					pulsing.mode_arg.xy.x = bt_effect_body->Pulsing_Arg.Mode.Effect_XY_Mode.Coordinate_x;
					pulsing.mode_arg.xy.y = bt_effect_body->Pulsing_Arg.Mode.Effect_XY_Mode.Coordinate_y;
					pulsing.frq = bt_effect_body->Pulsing_Arg.Mode.Effect_XY_Mode.Frq;
					pulsing.speed = bt_effect_body->Pulsing_Arg.Mode.Effect_XY_Mode.Speed;
					break;
				
				default: break;
			}
            data_center_write_light_data(LIGHT_MODE_FX_PULSING, &pulsing);
			break;
        }
		case BT_EffectType_Welding:
        {
            struct db_fx_welding welding;
			data_center_read_light_data(LIGHT_MODE_FX_WELDING, &welding); 
			welding.mode = get_ble_effectmodeback((enum fx_mode)bt_effect_body->Welding_Arg.Mode.Effect_CCT_Mode.Effect_Mode);
			welding.state = 1;
			
			switch (bt_effect_body->Welding_Arg.Mode.Effect_CCT_Mode.Effect_Mode)
			{
				case EffectModeCCT:
					welding.lightness = bt_effect_body->Welding_Arg.Mode.Effect_CCT_Mode.Int;
					welding.mode_arg.cct.cct = bt_effect_body->Welding_Arg.Mode.Effect_CCT_Mode.CCT * 10;
					welding.mode_arg.cct.duv = bt_effect_body->Welding_Arg.Mode.Effect_CCT_Mode.GM * 10 - 100;
					welding.frq = bt_effect_body->Welding_Arg.Mode.Effect_CCT_Mode.Frq;
					welding.min = bt_effect_body->Welding_Arg.Mode.Effect_CCT_Mode.Min;
					break;
				case EffectModeHSI:
					welding.lightness = bt_effect_body->Welding_Arg.Mode.Effect_HSI_Mode.Int; 
					welding.mode_arg.hsi.hue = bt_effect_body->Welding_Arg.Mode.Effect_HSI_Mode.Hue ;
					welding.mode_arg.hsi.sat = bt_effect_body->Welding_Arg.Mode.Effect_HSI_Mode.Sat ;
					welding.frq = bt_effect_body->Welding_Arg.Mode.Effect_HSI_Mode.Frq;
					welding.min = bt_effect_body->Welding_Arg.Mode.Effect_HSI_Mode.Min;
					break;
				case EffectModeGEL:
					welding.lightness = bt_effect_body->Welding_Arg.Mode.Effect_Gel_Mode.Int;
					welding.mode_arg.gel.cct = bt_effect_body->Welding_Arg.Mode.Effect_Gel_Mode.CCT * 10;
					welding.mode_arg.gel.brand = bt_effect_body->Welding_Arg.Mode.Effect_Gel_Mode.Origin == 0 ? 1 : 0;
					origin = bt_effect_body->Welding_Arg.Mode.Effect_Gel_Mode.Origin == 0 ? 1 : 0;
					welding.mode_arg.gel.type[origin] = bt_effect_body->Welding_Arg.Mode.Effect_Gel_Mode.Type;
					gtype = bt_effect_body->Welding_Arg.Mode.Effect_Gel_Mode.Type;
					welding.mode_arg.gel.color[origin][gtype] = bt_effect_body->Welding_Arg.Mode.Effect_Gel_Mode.Color;
					welding.frq = bt_effect_body->Welding_Arg.Mode.Effect_Gel_Mode.Frq;
					welding.min = bt_effect_body->Welding_Arg.Mode.Effect_Gel_Mode.Min;
					break;
				case EffectModeCoord:
					welding.lightness = bt_effect_body->Welding_Arg.Mode.Effect_XY_Mode.Int; 
					welding.mode_arg.xy.x = bt_effect_body->Welding_Arg.Mode.Effect_XY_Mode.Coordinate_x;
					welding.mode_arg.xy.y = bt_effect_body->Welding_Arg.Mode.Effect_XY_Mode.Coordinate_y;
					welding.frq = bt_effect_body->Welding_Arg.Mode.Effect_XY_Mode.Frq;
					welding.min = bt_effect_body->Welding_Arg.Mode.Effect_XY_Mode.Min;
					break;

				default: break;
			}
            data_center_write_light_data(LIGHT_MODE_FX_WELDING, &welding);
			break;
        }
		case BT_EffectType_CopCar:
        {
            struct db_fx_cop_car cop_car;
			cop_car.frq = bt_effect_body->Cop_Car_Arg.Frq;
			cop_car.lightness = bt_effect_body->Cop_Car_Arg.Int;
			cop_car.color = bt_effect_body->Cop_Car_Arg.Color;
            cop_car.state = 1;
            data_center_write_light_data(LIGHT_MODE_FX_COP_CAR, &cop_car);
			break;
        }
		case BT_EffectType_ColorChase:
        {
            struct db_fx_color_chase color_chase;
			color_chase.spd = bt_effect_body->Color_Chase_Arg.Frq;
			color_chase.lightness = bt_effect_body->Color_Chase_Arg.Int;
			color_chase.sat = bt_effect_body->Color_Chase_Arg.Sat;
            color_chase.state =1;
            data_center_write_light_data(LIGHT_MODE_FX_COLOR_CHASE, &color_chase);
			break;
        }
		case BT_EffectType_PartyLights:
        {
            struct db_fx_party_lights party_lights;
			party_lights.spd = bt_effect_body->Party_Lights_Arg.Frq;
			party_lights.lightness = bt_effect_body->Party_Lights_Arg.Int;
			party_lights.sat = bt_effect_body->Party_Lights_Arg.Sat;
            party_lights.state = 1;
            data_center_write_light_data(LIGHT_MODE_FX_PARTY_LIGHTS, &party_lights);
			break;
        }
		case BT_EffectType_Fireworks:
        {
            struct db_fx_fireworks fireworks;
			fireworks.frq = bt_effect_body->Fireworks_Arg.Frq;
			fireworks.lightness = bt_effect_body->Fireworks_Arg.Int;
			fireworks.type = bt_effect_body->Fireworks_Arg.Type;
            fireworks.state = 1;
			#if PROJECT_TYPE==308
			g_tUIAllData.fireworks_model.state = 1;
			#endif
            data_center_write_light_data(LIGHT_MODE_FX_FIREWORKS, &fireworks);
			break;
        }
		case BT_EffectType_Effect_Off:  // 关灯
//			ble_publish_ctr_light_event(TURN_OFF_EVENT);
			if(date_center_get_light_mode() == LIGHT_MODE_FX_LIGHTNING)
			{
				struct db_fx_lightning lightning;
				data_center_read_light_data(LIGHT_MODE_FX_LIGHTNING, &lightning);
				lightning.trigger = 0;
				lightning.state = 0;
				data_center_write_light_data(date_center_get_light_mode(), &lightning);
			}
			else
			{
				#if (PROJECT_TYPE == 308)  
					g_tUIAllData.explosion_model.state = 0;
					g_tUIAllData.faultybulb_model.state = 0;
					g_tUIAllData.pulsing_model.state = 0; 
					g_tUIAllData.strobe_model.state = 0;
					g_tUIAllData.lightning_model.state = 0;
					g_tUIAllData.tv_model.state = 0;
					g_tUIAllData.fireworks_model.state = 0;
					g_tUIAllData.fire_model.state = 0;
				#else    
					g_tUIAllData.explosion2_model.state = 0;
					g_tUIAllData.faultybulb2_model.state = 0;
					g_tUIAllData.strobe2_model.state = 0;
					g_tUIAllData.lightning2_model.state = 0;
					g_tUIAllData.fireworks_model.state = 0;  //need change
					g_tUIAllData.fire2_model.state = 0;
					g_tUIAllData.strobe_model.state = 0;
					g_tUIAllData.explosion_model.state = 0;
					g_tUIAllData.faultybulb_model.state = 0;
				#endif
					g_tUIAllData.tv2_model.state = 0;
					g_tUIAllData.pulsing2_model.state = 0;
					g_tUIAllData.welding2_model.state = 0;    
					g_tUIAllData.copcar2_model.state = 0;
					g_tUIAllData.candle_model.state = 0;
					g_tUIAllData.clublights_model.state = 0;
					g_tUIAllData.colorchase_model.state = 0;
					g_tUIAllData.paparazzi_model.state = 0;
					g_tUIAllData.partylight_model.state = 0;
				data_center_all_sys_fx_stop();
			}
			break;
		case BT_EffectType_I_Am_Here:   // 寻灯
			ble_publish_ctr_light_event((bt_effect_body->I_Am_Here_Arg.Ctrl == 0 ? 1:0) );
			break;
		
		default: break;
	}
	
}

/* 写常规光模式 */
static void write_parse_light_mode(BT_Body_TypeDef *write_body, uint8_t type)
{
	uint8_t hs_mode;
	data_center_read_config_data(SYS_CONFIG_HIGH_SPEED_MODE, &hs_mode);
	switch(type)
	{
		case BT_CommandType_CCT:
        {
            struct db_cct cct_arg;
			if(hs_mode)
			{
				if(write_body->CCT_Body.Int == 0)cct_arg.lightness = 0;
				else if(write_body->CCT_Body.Int < 200)cct_arg.lightness = 200;
				else {cct_arg.lightness = write_body->CCT_Body.Int;}
			}
			else{
				cct_arg.lightness = write_body->CCT_Body.Int;
			}
			cct_arg.cct.cct = write_body->CCT_Body.CCT * 10;
			#if PROJECT_TYPE==307
			cct_arg.cct.duv = write_body->CCT_Body.GM * 10 - 100;
			#elif PROJECT_TYPE==308
			if(write_body->CCT_Body.GM < 5)
				write_body->CCT_Body.GM = 5;
			else if(write_body->CCT_Body.GM > 15)
				write_body->CCT_Body.GM = 15;
			cct_arg.cct.duv = write_body->CCT_Body.GM * 10 - 100;
			g_tUIAllData.cct_model.lightness = cct_arg.lightness;
			g_tUIAllData.cct_model.cct.cct = cct_arg.cct.cct;
			g_tUIAllData.cct_model.cct.duv = cct_arg.cct.duv;
			#endif
            data_center_write_light_data(LIGHT_MODE_CCT, &cct_arg);
			break;
        }
		case BT_CommandType_HSI:
        {
            struct db_hsi hsi_arg;
			if(hs_mode)
			{
				if(write_body->HSI_Body.Int == 0)hsi_arg.lightness = 0;
				else if(write_body->HSI_Body.Int < 200)hsi_arg.lightness = 200;
				else {hsi_arg.lightness = write_body->HSI_Body.Int;}
			}
			else{
				hsi_arg.lightness = write_body->HSI_Body.Int;
			}
			data_center_read_light_data(LIGHT_MODE_HSI, &hsi_arg); 
			hsi_arg.lightness = write_body->HSI_Body.Int;
			hsi_arg.hsi.hue = write_body->HSI_Body.Hue * 10;
			hsi_arg.hsi.sat = write_body->HSI_Body.Sat * 10;
            data_center_write_light_data(LIGHT_MODE_HSI, &hsi_arg);
			break;
        }
		case BT_CommandType_GEL:
        {
            struct db_gel gel_arg;
			
			memcpy(&gel_arg, &g_tUIAllData.gel_model, sizeof(gel_arg));
			if(hs_mode)
			{
				if(write_body->Gel_Body.Int == 0)gel_arg.lightness = 0;
				else if(write_body->Gel_Body.Int < 200)gel_arg.lightness = 200;
				else {gel_arg.lightness = write_body->Gel_Body.Int;}
			}
			else{
				gel_arg.lightness = write_body->Gel_Body.Int;
			}
			gel_arg.lightness = write_body->Gel_Body.Int;
			gel_arg.gel.cct = write_body->Gel_Body.CCT  * 10;
			gel_arg.gel.brand = write_body->Gel_Body.Origin == 0 ? 1 : 0;
			gel_arg.gel.type[gel_arg.gel.brand] = write_body->Gel_Body.Type;
			gel_arg.gel.color[gel_arg.gel.brand][write_body->Gel_Body.Type] = write_body->Gel_Body.Color;
			data_center_write_light_data(LIGHT_MODE_GEL, &gel_arg);
			break;
        }
		case BT_CommandType_RGBWW:
        {
            struct db_rgb rgb_arg;
			if(hs_mode)
			{
				if(write_body->RGBWW_Body.Int == 0)rgb_arg.lightness = 0;
				else if(write_body->RGBWW_Body.Int < 200)rgb_arg.lightness = 200;
				else {rgb_arg.lightness = write_body->RGBWW_Body.Int;}
			}
			else{
				rgb_arg.lightness = write_body->RGBWW_Body.Int;
			}
			rgb_arg.lightness = write_body->RGBWW_Body.Int;
			rgb_arg.rgb.r = write_body->RGBWW_Body.R_Int;
			rgb_arg.rgb.g = write_body->RGBWW_Body.G_Int;
			rgb_arg.rgb.b = write_body->RGBWW_Body.B_Int;
            data_center_write_light_data(LIGHT_MODE_RGB, &rgb_arg);
			break;
        }
		case BT_CommandType_XY_Coordinate:
        {
            struct db_xy xy_arg;
			if(hs_mode)
			{
				if(write_body->XY_Coordinate_Body.Int == 0)xy_arg.lightness = 0;
				else if(write_body->XY_Coordinate_Body.Int < 200)xy_arg.lightness = 200;
				else {xy_arg.lightness = write_body->XY_Coordinate_Body.Int;}
			}
			else{
				xy_arg.lightness = write_body->XY_Coordinate_Body.Int;
			}
			xy_arg.lightness = write_body->XY_Coordinate_Body.Int;
			xy_arg.xy.x = write_body->XY_Coordinate_Body.Coordinate_X;
			xy_arg.xy.y = write_body->XY_Coordinate_Body.Coordinate_Y;
            data_center_write_light_data(LIGHT_MODE_XY, &xy_arg);
			break;
        }
		default: break;
	}
}

/* 写解析 */
static void write_light_mode(BT_Body_TypeDef *write_body, uint8_t type)
{
	uint8_t hsmode;
	data_center_read_config_data(SYS_CONFIG_HIGH_SPEED_MODE, &hsmode);
	if(type <= BT_CommandType_XY_Coordinate)
	{
		write_parse_light_mode(write_body, type);
		user_switch_page(date_center_get_light_mode());
	}
	else if(type == BT_CommandType_Light_Effect)
	{
		if(hsmode)
		{
			hsmode = 0;
			ui_set_hs_mode_state(hsmode);
			data_center_write_config_data(SYS_CONFIG_HIGH_SPEED_MODE, &hsmode);
		}
		BT_Effect_Mode_Body_TypeDef *bt_effect_body = NULL;
		bt_effect_body = &write_body->Light_Effect_Body;
		write_parse_sys_effect(write_body);
		if(bt_effect_body->Club_Lights_Arg.Effect_Type == BT_EffectType_I_Am_Here) return;
		user_switch_page(date_center_get_light_mode());
	}
    else if(type <= BT_CommandType_CFX_Name)
	{
		if(hsmode)
		{
			hsmode = 0;
			ui_set_hs_mode_state(hsmode);
			data_center_write_config_data(SYS_CONFIG_HIGH_SPEED_MODE, &hsmode);
		}
        set_ble_cfx_file_state(0);
		write_parse_sidus_pro(write_body, type);
		user_switch_page(date_center_get_light_mode());
	}
	else if(type == BT_CommandType_Light_Effect_II)
	{
		if(hsmode)
		{
			hsmode = 0;
			ui_set_hs_mode_state(hsmode);
			data_center_write_config_data(SYS_CONFIG_HIGH_SPEED_MODE, &hsmode);
		}
		write_parse_effect_II(write_body);
		user_switch_page(date_center_get_light_mode());
	}	
	else if(type == BT_CommandType_Pixel_Fx)
	{
		if(hsmode)
		{
			hsmode = 0;
			ui_set_hs_mode_state(hsmode);
			data_center_write_config_data(SYS_CONFIG_HIGH_SPEED_MODE, &hsmode);
		}
		write_parse_pixel_effect(write_body);
		user_switch_page(date_center_get_light_mode());
	}	
	else if(type <= BT_CommandType_Partition_Cfg)
	{
		if(hsmode)
		{
			hsmode = 0;
			ui_set_hs_mode_state(hsmode);
			data_center_write_config_data(SYS_CONFIG_HIGH_SPEED_MODE, &hsmode);
		}
		write_parse_partition_effect(write_body, type);
		user_switch_page(date_center_get_light_mode());
	}	
}

static uint8_t s_iot_send_cnt = 0;
void app_ble_proto_parse(uint8_t *rx_data)
{
	IOT_Msg_TypeDef *p_iot_msg = (IOT_Msg_TypeDef *)rx_data;
    BT_Packet_TypeDef *rx_packet = (BT_Packet_TypeDef *)rx_data;
	BT_Body_TypeDef read_body = {0};
	BT_CommandType_TypeDef cmd_type = BT_CommandType_NULL;
	uint8_t tx_data[10] = {0};
	struct sys_info_dmx dmx_link_state;
	
	if(0 == rx_packet->Header.Opera_Type)
	{
		switch(rx_packet->Header.Command_Type)
		{
            case BT_CommandType_Version:
            {
                struct sys_info_lamp lamp;
                data_center_read_sys_info(SYS_INFO_LAMP, &lamp);
                read_body.Version_Body.MFX_Support = 1;
                read_body.Version_Body.PFX_Support = 1;
                read_body.Version_Body.CFX_Picker_Support = 1;
                read_body.Version_Body.CFX_Touchbar_Support = 1;
                read_body.Version_Body.CFX_Music_Support = 1;
                read_body.Version_Body.Upgrade_Type = 1;
                read_body.Version_Body.GATT_Ver = 1;
                read_body.Version_Body.Protocol_Ver = BLUETOOTH_PROTO_VER;
                read_body.Version_Body.Product_Function = CONFIG_PRODUCT_FUNCTION;
                read_body.Version_Body.Product_LedType = CONFIG_PRODUCT_LED_TYPE;
                read_body.Version_Body.Product_CCTRange_L = CONFIG_PRODUCT_CCT_MIN / 100;
                read_body.Version_Body.Product_CCTRange = CONFIG_PRODUCT_CCT_MAX / 100;
                read_body.Version_Body.Product_Machine = CONFIG_PRODUCT_MACHINE;
                read_body.Version_Body.Control_HwVer = 10 * (HARDWARE_VERSION / 16) + (HARDWARE_VERSION % 16);
                read_body.Version_Body.Control_SwVer = 10 * (SOFTWARE_VERSION / 16) + (SOFTWARE_VERSION % 16);
                if(1 == get_lamp_connet_state())
                {
                    read_body.Version_Body.Driver_HwVer = 10 * (lamp.hard_ver[0] / 16) + (lamp.hard_ver[0] % 16);
                    read_body.Version_Body.Driver_SwVer = 10 * (lamp.soft_ver[0] / 16) + (lamp.soft_ver[0] % 16);
                }
                else 
                {
                    read_body.Version_Body.Driver_HwVer = 0;
                    read_body.Version_Body.Driver_SwVer = 0;
                }
                ble_protocol_data_pack_up(tx_data, &read_body, BT_CommandType_Version, 0);
                ble_send_data_queue(BLE_MESH_DATA, tx_data, 10);
            }
            break;
            case BT_CommandType_Version2:
            {
                struct sys_info_power power;
                data_center_read_sys_info(SYS_INFO_POWER, &power);
                read_body.version2.pixel_num = 0;  
                read_body.version2.sys_fx2_support = 1;
                read_body.version2.sys_fx2_gpA_support = 1;
                read_body.version2.sys_fx2_gpB_support = 1;
                read_body.version2.sys_fx2_gpC_support = 1;
                read_body.version2.sys_fx2_gpD_support = 1;
                read_body.version2.sys_fx2_gpE_support = 1;
                read_body.version2.sys_fx2_gpF_support = 0;
                read_body.version2.sys_fx2_gpG_support = 0;
                read_body.version2.sys_fx2_gpH_support = 0;
                read_body.version2.pixel_fx_support = 0;
                read_body.version2.pixel_fx_gpA_support = 0;
                read_body.version2.pixel_fx_gpB_support = 0;
                read_body.version2.pixel_fx_gpC_support = 0;
                read_body.version2.pixel_fx_gpD_support = 0;
                read_body.version2.pixel_fx_gpE_support = 0;
                read_body.version2.pixel_fx_gpF_support = 0;
                read_body.version2.pixel_fx_gpG_support = 0;
                read_body.version2.pixel_x1 = 1;
                read_body.version2.pixel_y1 = 1;
                read_body.version2.pixel_x2 = 1;
                read_body.version2.pixel_y2 = 1;
                read_body.version2.add_state = 1;
                read_body.version2.sleep = power.state;
                read_body.version2.emotion_support = 1;
                ble_protocol_data_pack_up(tx_data, &read_body, BT_CommandType_Version2, 0);
                ble_send_data_queue(BLE_MESH_DATA, tx_data, 10);
            }
            break;
            case BT_CommandType_Light_Mode:
            {
                struct sys_info_power power;
                
                data_center_read_sys_info(SYS_INFO_POWER, &power);
                cmd_type = read_light_mode(&read_body, date_center_get_light_mode());
                read_body.HSI_Body.Sleep_Mode = power.state;
                ble_protocol_data_pack_up(tx_data, &read_body, cmd_type, 0);
                ble_send_data_queue(BLE_MESH_DATA, tx_data, 10);
            }
            break;
            case BT_CommandType_Get_Power:
            {
//                struct sys_info_power power;
//                
//                data_center_read_sys_info(SYS_INFO_POWER, &power);
                read_body.Get_Power_Body.Battery_time = 120;
                read_body.Get_Power_Body.Battery_Power = 100; 			// 4格电量
                read_body.Get_Power_Body.Extern_Voltage = 1;      
//                if(power.charge_state)
//                {
//                    read_body.Get_Power_Body.Extern_Voltage = 5000;
//                }
//                else
//                {
//                    read_body.Get_Power_Body.Extern_Voltage = 0;
//                }
                ble_protocol_data_pack_up(tx_data, &read_body, BT_CommandType_Get_Power, 0);
                ble_send_data_queue(BLE_MESH_DATA, tx_data, 10);
              
            }
            break;
            case BT_CommandType_Dimming_Curve:  // 读取没有发这包数据
            {
                uint8_t curve_type = 0;
                data_center_read_config_data(SYS_CONFIG_CURVE_TYPE, &curve_type);
                read_body.Dimming_Curve_Body.Curve = curve_type;
                ble_protocol_data_pack_up(tx_data, &read_body, BT_CommandType_Dimming_Curve, 0);
                ble_send_data_queue(BLE_MESH_DATA, tx_data, 10);
            }
            break;
            case  BT_CommandType_Sleep_Mode:
            {
                struct sys_info_power sys_power;
            
                data_center_read_sys_info(SYS_INFO_POWER, &sys_power);
                
                read_body.Sleep_Mode_Body.Mode = sys_power.state;
                
                ble_protocol_data_pack_up(tx_data, &read_body, BT_CommandType_Sleep_Mode, 0);
                ble_send_data_queue(BLE_MESH_DATA, tx_data, 10);
            }
            break;
            case BT_CommandType_Board_State:
                break;
            case BT_CommandType_IOT_Cmd:
                if(0xff != get_send_data_serial(p_iot_msg->IOT_Type))
                {
//                    data_center_read_iot_data(p_iot_msg->IOT_Type, (uint8_t*)tx_data);
//                    ble_send_data_queue(BLE_MESH_DATA, tx_data, 10);
                }
                else
                {
//                    osTimerStop(os_timer_iot);
//                    s_iot_send_cnt = 0;
//                    osTimerStart(os_timer_iot, 1000);
                }
                break; 
            case BT_CommandType_Fan_Speed:
            {
                uint8_t fan_mode = 0;
                struct sys_info_ctrl_box box_read_body;
                struct sys_info_lamp   lamp_read_body;
                data_center_read_config_data(SYS_CONFIG_FAN_MODE, &fan_mode);
                data_center_read_sys_info(SYS_INFO_CTRL, &box_read_body);
                data_center_read_sys_info(SYS_INFO_LAMP, &lamp_read_body);
                switch(fan_mode)
                {
                        case 0:
                                read_body.Fan_Speed_Body.Mode = 0x01;
                        break;
                        case 1:
                                read_body.Fan_Speed_Body.Mode = 0x04;
                        break;
                        case 2:
                                read_body.Fan_Speed_Body.Mode = 0x05;
                        break;
                        case 3:
                                read_body.Fan_Speed_Body.Mode = 0x07;
                        break;
                        default:break;
                }
                read_body.Fan_Speed_Body.Silent_Use = 1;
                read_body.Fan_Speed_Body.Medium_Use = 1;
                read_body.Fan_Speed_Body.Smart_Use = 1;
                read_body.Fan_Speed_Body.High_Use = 1;
                read_body.Fan_Speed_Body.High_Temp = 0;
				if(lamp_read_body.exist == 1)
				{
					read_body.Fan_Speed_Body.Current_Temp = lamp_read_body.cob_temp; 
					read_body.Fan_Speed_Body.Fixture_Speed = lamp_read_body.fan_speed;
				}
				else
				{
					read_body.Fan_Speed_Body.Current_Temp = box_read_body.ambient_temp; 
					read_body.Fan_Speed_Body.Fixture_Speed = dev_fan_revolving_speed_get();
				}
               
                ble_protocol_data_pack_up(tx_data, &read_body, BT_CommandType_Fan_Speed, 0);
                ble_send_data_queue(BLE_MESH_DATA, tx_data, 10);
            }
            break;
			case BT_CommandType_High_speed:
			{
				uint8_t hsmode;
				data_center_read_config_data(SYS_CONFIG_HIGH_SPEED_MODE, &hsmode); 
				read_body.high_speed_body.state = hsmode ;
				ble_protocol_data_pack_up(tx_data, &read_body, BT_CommandType_High_speed, 0);
                ble_send_data_queue(BLE_MESH_DATA, tx_data, 10);
				
			}
			break;
            case BT_CommandType_CFX_Bank_RW:
            {
                //TODO   485获取灯体的bank_info信息
                struct sys_info_cfx_name cfx_info;
                
                data_center_read_sys_info(SYS_INFO_CFX_NAME, &cfx_info);
                cfx_info.state = 0;
                cfx_info.cfx_type = rx_packet->Body.CFX_Bank_RW_Body.Effect_Type;
                data_center_write_sys_info(SYS_INFO_CFX_NAME, &cfx_info);
                set_read_cfx_bank_rw_state(1);
//                switch(read_body.CFX_Bank_RW_Body.Effect_Type)
//                {
//                    case 0:
//                        read_body.CFX_Bank_RW_Body.Effect_Type = 0;
//                        read_body.CFX_Bank_RW_Body.Bank_Info = SidusProFile_CFX_BankInfo_Get(0);
//                        break;

//                    case 1:
//                        read_body.CFX_Bank_RW_Body.Effect_Type = 1;
//                        read_body.CFX_Bank_RW_Body.Bank_Info = SidusProFile_CFX_BankInfo_Get(1);
//                        break;

//                    case 2:
//                        read_body.CFX_Bank_RW_Body.Effect_Type = 2;
//                        read_body.CFX_Bank_RW_Body.Bank_Info = SidusProFile_CFX_BankInfo_Get(2);
//                        break;

//                    default:
//                        break;
//                }
//                
//                ble_protocol_data_pack_up(tx_data, &read_body, BT_CommandType_CFX_Bank_RW, 0);
//                ble_send_data_queue(BLE_MESH_DATA, tx_data, 10); 
            }
            break;
            case BT_CommandType_CFX_Name:
            {
//                char *cfx_name1;
                struct  sys_info_cfx_name cfx_name;
				
                //TODO   485获取灯体的cfx_name信息
//                osSemaphoreAcquire (Get_fx_bank_name, 0);
//                osThreadFlagsSet(ThreadRS485HandHandle,  RS485_Rw_Bank_Name);
//                if(osOK == osSemaphoreAcquire (Get_fx_bank_name, 200))
                {
                    data_center_read_sys_info(SYS_INFO_CFX_NAME, &cfx_name);
                    
					cfx_name.cfx_type = rx_packet->Body.CFX_Name_Body.Effect_Type;
					cfx_name.cfx_bank =  rx_packet->Body.CFX_Name_Body.Bank;
                    data_center_write_sys_info(SYS_INFO_CFX_NAME, &cfx_name);
                    
                    set_read_cfx_name_state(1);
//                    cfx_name1 = &cfx_name.cfx_name[cfx_name.cfx_type].name[cfx_name.cfx_bank][0];
//                    read_body.CFX_Name_Body.Character_0 = SidusProFile_ASCII2SidusCode(cfx_name1[0]);
//                    read_body.CFX_Name_Body.Character_1 = SidusProFile_ASCII2SidusCode(cfx_name1[1]);
//                    read_body.CFX_Name_Body.Character_2 = SidusProFile_ASCII2SidusCode(cfx_name1[2]);
//                    read_body.CFX_Name_Body.Character_3 = SidusProFile_ASCII2SidusCode(cfx_name1[3]);
//                    read_body.CFX_Name_Body.Character_4 = SidusProFile_ASCII2SidusCode(cfx_name1[4]);
//                    read_body.CFX_Name_Body.Character_5 = SidusProFile_ASCII2SidusCode(cfx_name1[5]);
//                    read_body.CFX_Name_Body.Character_6 = SidusProFile_ASCII2SidusCode(cfx_name1[6]);
//                    read_body.CFX_Name_Body.Character_7 = SidusProFile_ASCII2SidusCode(cfx_name1[7]);
//                    read_body.CFX_Name_Body.Character_8 = SidusProFile_ASCII2SidusCode(cfx_name1[8]);

//                    ble_protocol_data_pack_up(tx_data, &read_body, BT_CommandType_CFX_Name, 0);
//                    ble_send_data_queue(BLE_MESH_DATA, tx_data, 10);                    

                }
                
//                ble_protocol_data_pack_up(tx_data, &read_body, BT_CommandType_CFX_Name, 0);
//                ble_send_data_queue(BLE_MESH_DATA, tx_data, 10);
            }
            break;
            case BT_CommandType_Partition_Para:
            {
                struct sys_info_power power;
                bt_partition_para_arg_t *bt_partition_para;
                bt_partition_para = &read_body.partition_para;
                
                struct db_partition_data partition_data;
                data_center_read_light_data(LIGHT_MODE_PARTITION_DATA, &partition_data); 
                
                if(partition_data.target == 255)
                {
                    data_center_read_sys_info(SYS_INFO_POWER, &power);
                    bt_partition_para->ack.cfg.target = partition_data.target;
                    bt_partition_para->ack.cfg.x = partition_data.partition_num;
                    bt_partition_para->ack.cfg.y = 1;
                    bt_partition_para->ack.cfg.state = partition_data.state;
                    bt_partition_para->ack.cfg.sleep = power.state;
                }
                else
                {
                    uint8_t target = 0;
                    bt_partition_para->read.target = partition_data.target;
                    target = partition_data.target;

                    switch(partition_data.light_mode[target])
                    {
                        case 0:
                            if(partition_data.light_mode[target+1] == 0) //CCT-CCT
                            {
                                bt_partition_para->ack.para.gp0.light_mode1 = 0;
                                bt_partition_para->ack.para.gp0.cct1 = partition_data.color_arg[target].cct.cct;
                                bt_partition_para->ack.para.gp0.duv1 = partition_data.color_arg[target].cct.gm;
                                bt_partition_para->ack.para.gp0.brightness1 = partition_data.color_arg[target].cct.lightness;
                                bt_partition_para->ack.para.gp0.light_mode2 = 0;
                                bt_partition_para->ack.para.gp0.cct2 = partition_data.color_arg[target+1].cct.cct;
                                bt_partition_para->ack.para.gp0.duv2 = partition_data.color_arg[target+1].cct.gm;
                                bt_partition_para->ack.para.gp0.brightness2 = partition_data.color_arg[target+1].cct.lightness;
                                bt_partition_para->ack.para.gp0.target = target;
                            }
                            else if(partition_data.light_mode[target+1] == 1) //CCT-HSI
                            {
                                bt_partition_para->ack.para.gp1.light_mode1 = 0;
                                bt_partition_para->ack.para.gp1.cct1 = partition_data.color_arg[target].cct.cct;
                                bt_partition_para->ack.para.gp1.duv1 = partition_data.color_arg[target].cct.gm;
                                bt_partition_para->ack.para.gp1.brightness1 = partition_data.color_arg[target].cct.lightness;
                                bt_partition_para->ack.para.gp1.light_mode2 = 1;
                                bt_partition_para->ack.para.gp1.hue2 = partition_data.color_arg[target+1].hsi.hue;
                                bt_partition_para->ack.para.gp1.sat2 = partition_data.color_arg[target+1].hsi.sat;
                                bt_partition_para->ack.para.gp1.brightness2 = partition_data.color_arg[target+1].hsi.lightness;
                                bt_partition_para->ack.para.gp1.target = target;
                            }
                            break;
                        case 1:
                            if(partition_data.light_mode[target+1] == 0) //HSI-CCT
                            {
                                bt_partition_para->ack.para.gp2.light_mode1 = 1;
                                bt_partition_para->ack.para.gp2.hue1 = partition_data.color_arg[target].hsi.hue;
                                bt_partition_para->ack.para.gp2.sat1 = partition_data.color_arg[target].hsi.sat;
                                bt_partition_para->ack.para.gp2.brightness1 = partition_data.color_arg[target].hsi.lightness;
                                bt_partition_para->ack.para.gp2.light_mode2 = 0;
                                bt_partition_para->ack.para.gp2.cct2 = partition_data.color_arg[target+1].cct.cct;
                                bt_partition_para->ack.para.gp2.duv2 = partition_data.color_arg[target+1].cct.gm;
                                bt_partition_para->ack.para.gp2.brightness2 = partition_data.color_arg[target+1].cct.lightness;
                                bt_partition_para->ack.para.gp2.target = target;
                            }
                            else if(partition_data.light_mode[target+1] == 1) //HSI-HSI
                            {
                                bt_partition_para->ack.para.gp3.light_mode1 = 1;
                                bt_partition_para->ack.para.gp3.hue1 = partition_data.color_arg[target].hsi.hue;
                                bt_partition_para->ack.para.gp3.sat1 = partition_data.color_arg[target].hsi.sat;
                                bt_partition_para->ack.para.gp3.brightness1 = partition_data.color_arg[target].hsi.lightness;
                                bt_partition_para->ack.para.gp3.light_mode2 = 1;
                                bt_partition_para->ack.para.gp3.hue2 = partition_data.color_arg[target+1].hsi.hue;
                                bt_partition_para->ack.para.gp3.sat2 = partition_data.color_arg[target+1].hsi.sat;
                                bt_partition_para->ack.para.gp3.brightness2 = partition_data.color_arg[target+1].hsi.lightness;
                                bt_partition_para->ack.para.gp3.target = target;
                            }
                            break;
                            
                        default: break;
                    } 
                }
            }
            break;
            case BT_CommandType_Partition_Fx:
            {
                bt_partition_fx_arg_t *bt_partition_fx;
                bt_partition_fx = &read_body.partition_fx;
                struct db_partition_data partition_data;
                data_center_read_light_data(LIGHT_MODE_PARTITION_FX, &partition_data); 
                
                bt_partition_fx->flicker.trigger = partition_data.fx_mode_arg.Flicker.fx_arg.trigger;
                bt_partition_fx->flicker.lasting_min = partition_data.fx_mode_arg.Flicker.fx_arg.lasting_min;
                bt_partition_fx->flicker.lasting_max = partition_data.fx_mode_arg.Flicker.fx_arg.lasting_max;
                bt_partition_fx->flicker.interval_min = partition_data.fx_mode_arg.Flicker.fx_arg.interval_min;
                bt_partition_fx->flicker.interval_max = partition_data.fx_mode_arg.Flicker.fx_arg.interval_max;
                bt_partition_fx->flicker.frq_max = partition_data.fx_mode_arg.Flicker.fx_arg.frq_max;
                bt_partition_fx->flicker.frq_min = partition_data.fx_mode_arg.Flicker.fx_arg.frq_min;
                bt_partition_fx->flicker.int_min = partition_data.fx_mode_arg.Flicker.int_min;
                bt_partition_fx->flicker.mode = partition_data.fx_mode;
            }
            break;
            case BT_CommandType_Partition_Cfg:
            {
                bt_partition_cfg_art_t *bt_partition_cfg;
                bt_partition_cfg = &read_body.partition_cfg;
                struct db_partition_cfg partition_cfg;
                data_center_read_light_data(LIGHT_MODE_PARTITION_CFG, &partition_cfg); 
                
                bt_partition_cfg->read.xy_mode = partition_cfg.xy_mode;
                bt_partition_cfg->read.pixel_x1 = 5;
                bt_partition_cfg->read.pixel_y1 = 1;
                bt_partition_cfg->read.pixel_x2 = 25;
                bt_partition_cfg->read.pixel_y2 = 1;
            }
            break;
            case BT_CommandType_MFX:
            {
                uint8_t i = 0;
                struct db_manual_fx mfx;
            
                data_center_read_light_data(LIGHT_MODE_SIDUS_MFX, &mfx);  
                for(; i < 4; ++i)
                {
                    read_body.MFX_Body.Packet_0.BaseCCT.Packet_Num = i;
                    switch (read_body.MFX_Body.Packet_0.BaseCCT.Packet_Num)
                    {
                        case 0:
                        { 
                            if(mfx.base == 0)
                            {
                                read_body.MFX_Body.Packet_0.BaseCCT.Base = mfx.base;
                                read_body.MFX_Body.Packet_0.BaseCCT.Int_Mini = mfx.base_arg.cct_range.int_mimi / 10;
                                read_body.MFX_Body.Packet_0.BaseCCT.Int_Max = mfx.base_arg.cct_range.int_max / 10;
                                read_body.MFX_Body.Packet_0.BaseCCT.Int_Seq = mfx.base_arg.cct_range.int_seq;
                                read_body.MFX_Body.Packet_0.BaseCCT.CCT_Mini = mfx.base_arg.cct_range.cct_mini / 10;
                                read_body.MFX_Body.Packet_0.BaseCCT.CCT_Max = mfx.base_arg.cct_range.cct_max / 10;
                                read_body.MFX_Body.Packet_0.BaseCCT.CCT_Seq = mfx.base_arg.cct_range.cct_seq;
                                read_body.MFX_Body.Packet_0.BaseCCT.GM_Mini = mfx.base_arg.cct_range.gm_mini/10 + 10;
                                read_body.MFX_Body.Packet_0.BaseCCT.GM_Max = mfx.base_arg.cct_range.gm_max/10 + 10;
                                read_body.MFX_Body.Packet_0.BaseCCT.GM_Seq = mfx.base_arg.cct_range.gm_seq;
                            }
                            else
                            {
                                read_body.MFX_Body.Packet_0.BaseCCT.Base = mfx.base;
                                read_body.MFX_Body.Packet_0.BaseHSI.Int_Mini = mfx.base_arg.hsi_range.int_mimi / 10;
                                read_body.MFX_Body.Packet_0.BaseHSI.Int_Max = mfx.base_arg.hsi_range.int_max / 10;
                                read_body.MFX_Body.Packet_0.BaseHSI.Int_Seq = mfx.base_arg.hsi_range.int_seq;
                                read_body.MFX_Body.Packet_0.BaseHSI.Hue_Mini = mfx.base_arg.hsi_range.hue_mini;
                                read_body.MFX_Body.Packet_0.BaseHSI.Hue_Max = mfx.base_arg.hsi_range.hue_max;
                                read_body.MFX_Body.Packet_0.BaseHSI.Hue_Seq = mfx.base_arg.hsi_range.hue_seq;
                                read_body.MFX_Body.Packet_0.BaseHSI.Sat_Mini = mfx.base_arg.hsi_range.sat_mini;
                                read_body.MFX_Body.Packet_0.BaseHSI.Sat_Max = mfx.base_arg.hsi_range.sat_max;
                                read_body.MFX_Body.Packet_0.BaseHSI.Sat_Seq = mfx.base_arg.hsi_range.sat_seq;
                            }                         
                        }
                        break;
                        case 1:
                        {
                            switch( mfx.fx_arg.mode)
                            {
                                case 0:
                                    read_body.MFX_Body.Packet_1.Flash.Effect_Mode = mfx.fx_arg.mode;
                                    read_body.MFX_Body.Packet_1.Flash.FreeTime_Seq = mfx.fx_arg.mode_arg.flash.free_time_seq;
                                    read_body.MFX_Body.Packet_1.Flash.FreeTime_Max = mfx.fx_arg.mode_arg.flash.free_time_max / 100;
                                    read_body.MFX_Body.Packet_1.Flash.FreeTime_Mini = mfx.fx_arg.mode_arg.flash.free_time_mini / 100;
                                    read_body.MFX_Body.Packet_1.Flash.CycleTime_Seq = mfx.fx_arg.mode_arg.flash.cycle_time_seq;
                                    read_body.MFX_Body.Packet_1.Flash.CycleTime_Max = mfx.fx_arg.mode_arg.flash.cycle_time_max / 100;
                                    read_body.MFX_Body.Packet_1.Flash.CycleTime_Mini = mfx.fx_arg.mode_arg.flash.cycle_time_mini / 100;
                                    read_body.MFX_Body.Packet_1.Flash.LoopTimes = mfx.fx_arg.mode_arg.flash.loop_times;
                                    read_body.MFX_Body.Packet_1.Flash.LoopMode = mfx.fx_arg.mode_arg.flash.loop_mode;
                                    break;
                                case 1:
                                    read_body.MFX_Body.Packet_1.Continue.Effect_Mode = mfx.fx_arg.mode;
                                    read_body.MFX_Body.Packet_1.Continue.FadeIn_Curve = mfx.fx_arg.mode_arg.continues.fade_in_curve;
                                    read_body.MFX_Body.Packet_1.Continue.FadeInTime_Max = mfx.fx_arg.mode_arg.continues.fade_in_time_max / 100;
                                    read_body.MFX_Body.Packet_1.Continue.FadeInTime_Mini = mfx.fx_arg.mode_arg.continues.fade_in_time_mini / 100;
                                    read_body.MFX_Body.Packet_1.Continue.FadeInTime_Seq = mfx.fx_arg.mode_arg.continues.fade_in_time_seq;
                                    read_body.MFX_Body.Packet_1.Continue.CycleTime_Seq = mfx.fx_arg.mode_arg.continues.cycle_time_seq;
                                    read_body.MFX_Body.Packet_1.Continue.CycleTime_Max = mfx.fx_arg.mode_arg.continues.cycle_time_max / 100;
                                    read_body.MFX_Body.Packet_1.Continue.CycleTime_Mini = mfx.fx_arg.mode_arg.continues.cycle_time_mini / 100;
                                    read_body.MFX_Body.Packet_1.Continue.LoopTimes =  mfx.fx_arg.mode_arg.continues.loop_times;
                                    read_body.MFX_Body.Packet_1.Continue.LoopMode = mfx.fx_arg.mode_arg.continues.loop_mode;
                                    break;
                                case 2:
                                    read_body.MFX_Body.Packet_1.Paragraph.Effect_Mode = mfx.fx_arg.mode;
                                    read_body.MFX_Body.Packet_1.Paragraph.UnitTime_Seq = mfx.fx_arg.mode_arg.paragraph.unit_time_seq;
                                    read_body.MFX_Body.Packet_1.Paragraph.FreeTime_Seq = mfx.fx_arg.mode_arg.paragraph.free_time_seq;
                                    read_body.MFX_Body.Packet_1.Paragraph.Overlap_Seq = mfx.fx_arg.mode_arg.paragraph.overlap_seq;
                                    read_body.MFX_Body.Packet_1.Paragraph.OLR_Seq = mfx.fx_arg.mode_arg.paragraph.olr_seq;
                                    read_body.MFX_Body.Packet_1.Paragraph.OLR_Max = mfx.fx_arg.mode_arg.paragraph.olr_max;
                                    read_body.MFX_Body.Packet_1.Paragraph.OLR_Mini = mfx.fx_arg.mode_arg.paragraph.olr_mini;
                                    read_body.MFX_Body.Packet_1.Paragraph.CycleTime_Seq = mfx.fx_arg.mode_arg.paragraph.cycle_time_seq;
                                    read_body.MFX_Body.Packet_1.Paragraph.CycleTime_Max = mfx.fx_arg.mode_arg.paragraph.cycle_time_max / 100;
                                    read_body.MFX_Body.Packet_1.Paragraph.CycleTime_Mini = mfx.fx_arg.mode_arg.paragraph.cycle_time_mini / 100;
                                    read_body.MFX_Body.Packet_1.Paragraph.LoopTimes = mfx.fx_arg.mode_arg.paragraph.loop_times;
                                    read_body.MFX_Body.Packet_1.Paragraph.LoopMode = mfx.fx_arg.mode_arg.paragraph.loop_mode;
                                    break;
                                default:
                                    break;
                            }            
                        }
                        break;
                        case 2:
                        {
                            switch(mfx.fx_arg.mode)
                            {
                                case 0:
                                    read_body.MFX_Body.Packet_2.Flash.Effect_Mode = mfx.fx_arg.mode;
                                    read_body.MFX_Body.Packet_2.Flash.Frq_Seq = mfx.fx_arg.mode_arg.flash.frq_seq;
                                    read_body.MFX_Body.Packet_2.Flash.Frq_Max = mfx.fx_arg.mode_arg.flash.frq_max;
                                    read_body.MFX_Body.Packet_2.Flash.Frq_Mini = mfx.fx_arg.mode_arg.flash.frq_mini;
                                    read_body.MFX_Body.Packet_2.Flash.UnitTime_Seq = mfx.fx_arg.mode_arg.flash.unit_time_seq;
                                    read_body.MFX_Body.Packet_2.Flash.UnitTime_Max = mfx.fx_arg.mode_arg.flash.unit_time_max / 100;
                                    read_body.MFX_Body.Packet_2.Flash.UnitTime_Mini = mfx.fx_arg.mode_arg.flash.unit_time_mini / 100;
                                    break;
                                case 1:
                                    read_body.MFX_Body.Packet_2.Continue.Effect_Mode = mfx.fx_arg.mode;
                                    read_body.MFX_Body.Packet_2.Continue.Flicker_Frq = mfx.fx_arg.mode_arg.continues.flicker_frq;
                                    read_body.MFX_Body.Packet_2.Continue.FadeOut_Curve = mfx.fx_arg.mode_arg.continues.fade_out_curve;
                                    read_body.MFX_Body.Packet_2.Continue.FadeOutTime_Seq = mfx.fx_arg.mode_arg.continues.fade_out_time_seq;
                                    read_body.MFX_Body.Packet_2.Continue.FadeOutTime_Max = mfx.fx_arg.mode_arg.continues.fade_out_time_max / 100;
                                    read_body.MFX_Body.Packet_2.Continue.FadeOutTime_Mini = mfx.fx_arg.mode_arg.continues.fade_out_time_mini / 100;
                                    break;
                                case 2:
                                    read_body.MFX_Body.Packet_2.Paragraph.Effect_Mode = mfx.fx_arg.mode;
                                    read_body.MFX_Body.Packet_2.Paragraph.OLP_Seq = mfx.fx_arg.mode_arg.paragraph.olp_seq;
                                    read_body.MFX_Body.Packet_2.Paragraph.OLP_Max = mfx.fx_arg.mode_arg.paragraph.olp_max;
                                    read_body.MFX_Body.Packet_2.Paragraph.OLP_Mini = mfx.fx_arg.mode_arg.paragraph.olp_mini;
                                    read_body.MFX_Body.Packet_2.Paragraph.UnitTime_Max = mfx.fx_arg.mode_arg.paragraph.unit_time_max / 100;
                                    read_body.MFX_Body.Packet_2.Paragraph.UnitTime_Mini = mfx.fx_arg.mode_arg.paragraph.unit_time_mini / 100;
                                    read_body.MFX_Body.Packet_2.Paragraph.FreeTime_Max = mfx.fx_arg.mode_arg.paragraph.free_time_max / 100;
                                    read_body.MFX_Body.Packet_2.Paragraph.FreeTime_Mini = mfx.fx_arg.mode_arg.paragraph.free_time_mini / 100;
                                    break;
                                default:
                                    break;
                            }
                        }
                        break;
                        case 3:
                        {
                            switch(mfx.fx_arg.mode)
                            {
                                case 0:
                                    break;
                                case 1:
                                    break;
                                case 2:
                                    read_body.MFX_Body.Packet_3.Paragraph.Effect_Mode = mfx.fx_arg.mode;
                                    read_body.MFX_Body.Packet_3.Paragraph.Flicker_Frq = mfx.fx_arg.mode_arg.paragraph.flicker_frq;
                                    read_body.MFX_Body.Packet_3.Paragraph.FadeOut_Curve = mfx.fx_arg.mode_arg.paragraph.fade_out_curve;
                                    read_body.MFX_Body.Packet_3.Paragraph.FadeOutTime_Seq = mfx.fx_arg.mode_arg.paragraph.fade_out_time_seq;
                                    read_body.MFX_Body.Packet_3.Paragraph.FadeOutTime_Max = mfx.fx_arg.mode_arg.paragraph.fade_out_time_max / 100;
                                    read_body.MFX_Body.Packet_3.Paragraph.FadeOutTime_Mini = mfx.fx_arg.mode_arg.paragraph.fade_out_time_mini  / 100;
                                    read_body.MFX_Body.Packet_3.Paragraph.FadeIn_Curve = mfx.fx_arg.mode_arg.paragraph.fade_in_curve;
                                    read_body.MFX_Body.Packet_3.Paragraph.FadeInTime_Seq = mfx.fx_arg.mode_arg.paragraph.fade_in_time_seq;
                                    read_body.MFX_Body.Packet_3.Paragraph.FadeInTime_Max = mfx.fx_arg.mode_arg.paragraph.fade_in_time_max / 100;
                                    read_body.MFX_Body.Packet_3.Paragraph.FadeInTime_Mini = mfx.fx_arg.mode_arg.paragraph.fade_in_time_mini / 100;
                                    break;
                                default:
                                    break;
                            }                
                        }
                    break;
                    default: break;
                    }                
                    ble_protocol_data_pack_up(tx_data, &read_body, BT_CommandType_MFX, 0);
                    ble_send_data_queue(BLE_MESH_DATA, tx_data, 10);                       
                }  
                if(i >= 4)
                    return;            
            }
            break;
            case BT_CommandType_CFX_Ctrl:
            {
                struct db_custom_fx cfx;
                
                data_center_read_light_data(LIGHT_MODE_SIDUS_CFX, &cfx);
                read_body.CFX_Ctrl_Body.Ctrl = cfx.ctrl;
                read_body.CFX_Ctrl_Body.Effect_Type = cfx.type ;
                read_body.CFX_Ctrl_Body.Bank = cfx.bank;
                read_body.CFX_Ctrl_Body.Chaos = cfx.chaos;
                read_body.CFX_Ctrl_Body.Loop = cfx.loop;
                read_body.CFX_Ctrl_Body.Sequence = cfx.sequence;
                read_body.CFX_Ctrl_Body.Int = cfx.lightness;
                read_body.CFX_Ctrl_Body.Speed = cfx.speed;                   
                ble_protocol_data_pack_up(tx_data, &read_body, BT_CommandType_CFX_Ctrl, 0);
                ble_send_data_queue(BLE_MESH_DATA, tx_data, 10);                               
            }
            break;
            case BT_CommandType_Emotion_Cfg:
            {
                struct sys_info_accessories access_info;
				
                data_center_read_sys_info(SYS_INFO_ACCESS, &access_info);
                
                read_body.emotion_cfg_body.emotion_cfg_packet1.packet_index = 0;
                if(access_info.fresnel_state == 1)
                {
                    read_body.emotion_cfg_body.emotion_cfg_packet1.bayonet_uuid = 1;
                }
                else if(access_info.optical_20_access_state == 1)
                {
                    read_body.emotion_cfg_body.emotion_cfg_packet1.bayonet_uuid = 2;
                }
                else if(access_info.optical_30_access_state == 1)
                {
                    read_body.emotion_cfg_body.emotion_cfg_packet1.bayonet_uuid = 3;
                }
                else if(access_info.optical_50_access_state == 1)
                {
                    read_body.emotion_cfg_body.emotion_cfg_packet1.bayonet_uuid = 4;
                }
                else
                {
                    read_body.emotion_cfg_body.emotion_cfg_packet1.bayonet_uuid = 0;
                }
                if(access_info.yoke_state == 1)
                {
                    read_body.emotion_cfg_body.emotion_cfg_packet1.rotation_lateral = 0x01;
                    read_body.emotion_cfg_body.emotion_cfg_packet1.rotation_pitch = 0x01;
                }
                else
                {
                    read_body.emotion_cfg_body.emotion_cfg_packet1.rotation_lateral = 0x00;
                    read_body.emotion_cfg_body.emotion_cfg_packet1.rotation_pitch = 0x00;
                }
                if(access_info.fresnel_state == 1)
                {
                    read_body.emotion_cfg_body.emotion_cfg_packet1.zoom = 1;
                    read_body.emotion_cfg_body.emotion_cfg_packet1.zoom_min = 18;
                    read_body.emotion_cfg_body.emotion_cfg_packet1.zoom_blur = 0;
                }
                else
                {
                    read_body.emotion_cfg_body.emotion_cfg_packet1.zoom = 0;
                    read_body.emotion_cfg_body.emotion_cfg_packet1.zoom_min = 0;
                    read_body.emotion_cfg_body.emotion_cfg_packet1.zoom_blur = 0;
                }
                
                ble_protocol_data_pack_up(tx_data, &read_body, BT_CommandType_Emotion_Cfg, 0);
                ble_send_data_queue(BLE_MESH_DATA, tx_data, 10); 
                
                if(access_info.fresnel_state == 1)
                {
                    read_body.emotion_cfg_body.emotion_cfg_packet2.zoom_max = 50;
                }
                else
                {
                    read_body.emotion_cfg_body.emotion_cfg_packet2.zoom_max = 0;
                }
                read_body.emotion_cfg_body.emotion_cfg_packet2.cut = 0;
                read_body.emotion_cfg_body.emotion_cfg_packet2.barndoor = 0;
                read_body.emotion_cfg_body.emotion_cfg_packet2.gobo_figure = 0;
                read_body.emotion_cfg_body.emotion_cfg_packet2.gobo_speed = 0;
                read_body.emotion_cfg_body.emotion_cfg_packet2.smoke = 0;
                read_body.emotion_cfg_body.emotion_cfg_packet2.lock_lateral = 1;
				read_body.emotion_cfg_body.emotion_cfg_packet2.lock_pitch = 1;
                read_body.emotion_cfg_body.emotion_cfg_packet2.packet_index = 1;
                
                ble_protocol_data_pack_up(tx_data, &read_body, BT_CommandType_Emotion_Cfg, 0);
                ble_send_data_queue(BLE_MESH_DATA, tx_data, 10); 
            }
            break;
            case BT_CommandType_Emotion_Live:
            {
                struct sys_info_motor motor_info;
				struct sys_info_accessories access_info;
				uint8_t lock[2];
				
				data_center_read_sys_info(SYS_INFO_ACCESS, &access_info);
                data_center_read_sys_info(SYS_INFO_MOTOR, &motor_info);
                
                read_body.emotion_live_body.emotion_live_packet1.target_field = 0;
                read_body.emotion_live_body.emotion_live_packet1.packet_index = 0;
                read_body.emotion_live_body.emotion_live_packet1.rotation_laterial = motor_info.pan_angle;
				if(access_info.fresnel_state == 1 || access_info.optical_20_access_state == 1 || 
				   access_info.optical_30_access_state == 1 || access_info.optical_50_access_state == 1)
				{
					int16_t  yoke_tilt_limit[2];
					int16_t  yoke_pan_limit[2];
					uint8_t tiltm;
					
					data_center_read_config_data(SYS_CONFIG_ANGLE_LIMIT, &tiltm);
					if(tiltm == 1)
					{
						data_center_read_config_data(SYS_CONFIG_YOKE_TILT_LIMIT, yoke_tilt_limit);
						data_center_read_config_data(SYS_CONFIG_YOKE_PAN_LIMIT, yoke_pan_limit);
						
						if(access_info.fresnel_state == 1 || access_info.optical_20_access_state == 1)
						{
							if(yoke_tilt_limit[1] >= 120)
								yoke_tilt_limit[1] = 120;
							if(yoke_tilt_limit[1] <= -120)
								yoke_tilt_limit[1] = -120;
							if(yoke_tilt_limit[0] <= -120)
								yoke_tilt_limit[0] = -120;
							if(yoke_tilt_limit[0] >= 120)
								yoke_tilt_limit[0] = 120;
						}
						read_body.emotion_live_body.emotion_live_packet1.rotation_pitch_max = ble_const_convert_symbol(yoke_tilt_limit[1]);
						read_body.emotion_live_body.emotion_live_packet1.rotation_pitch_min = ble_const_convert_symbol(yoke_tilt_limit[0]);
						read_body.emotion_live_body.emotion_live_packet1.rotation_laterial_max = yoke_pan_limit[1];
						read_body.emotion_live_body.emotion_live_packet1.rotation_laterial_min = yoke_pan_limit[0];
					}
					else
					{
						if(access_info.fresnel_state == 1 || access_info.optical_20_access_state == 1)
						{
							yoke_tilt_limit[1] = 120;
							yoke_tilt_limit[0] = -120;
						}
						else
						{
							yoke_tilt_limit[1] = ANGLE_LIMIT_DEFAULT_MAX;
							yoke_tilt_limit[0] = ANGLE_LIMIT_DEFAULT_MIN;
						}
						yoke_pan_limit[1] = ANGLE_LIMIT_MAX;
						yoke_pan_limit[0] = ANGLE_LIMIT_MIN;
						
						read_body.emotion_live_body.emotion_live_packet1.rotation_pitch_max = ble_const_convert_symbol(yoke_tilt_limit[1]);
						read_body.emotion_live_body.emotion_live_packet1.rotation_pitch_min = ble_const_convert_symbol(yoke_tilt_limit[0]);
						read_body.emotion_live_body.emotion_live_packet1.rotation_laterial_max = yoke_pan_limit[1];
						read_body.emotion_live_body.emotion_live_packet1.rotation_laterial_min = yoke_pan_limit[0];
					}
				}
				else
				{
					uint8_t tiltm[12];
					uint8_t ele_no_access_sel;
					
					data_center_read_config_data(SYS_CONFIG_NO_ANGLE_LIMIT, tiltm);
					data_center_read_config_data(SYS_CONFIG_NOT_ACCESS_SELECT, &ele_no_access_sel);
					if(ele_no_access_sel > 10)
						ele_no_access_sel = 10;
					if(tiltm[ele_no_access_sel] != 1)   //判断角度限制是否开启
					{
						read_body.emotion_live_body.emotion_live_packet1.rotation_pitch_max = ble_const_convert_symbol(ANGLE_LIMIT_DEFAULT_MAX);
						read_body.emotion_live_body.emotion_live_packet1.rotation_pitch_min = ble_const_convert_symbol(ANGLE_LIMIT_DEFAULT_MIN);
						read_body.emotion_live_body.emotion_live_packet1.rotation_laterial_max = ANGLE_LIMIT_MAX;
						read_body.emotion_live_body.emotion_live_packet1.rotation_laterial_min = ANGLE_LIMIT_MIN;
					}
					else
					{
						int16_t            ele_yoke_tilt1[12][2];  //电动支架非光学附件俯仰  最小最大值
						int16_t            ele_yoke_pan1[12][2];  //电动支架非光学附件水平
						
						data_center_read_config_data(SYS_CONFIG_YOKE_NO_TILT_LIMIT, &ele_yoke_tilt1);
						data_center_read_config_data(SYS_CONFIG_YOKE_NO_PAN_LIMIT, &ele_yoke_pan1);
						if(ele_no_access_sel <= 6)
						{
							if(ele_yoke_tilt1[ele_no_access_sel][0] < aputure_limit_angle_value[ele_no_access_sel].lower_limit_angle)
							{
								ele_yoke_tilt1[ele_no_access_sel][0] = aputure_limit_angle_value[ele_no_access_sel].lower_limit_angle;
							}
							if(ele_yoke_tilt1[ele_no_access_sel][0] > aputure_limit_angle_value[ele_no_access_sel].upper_limit_angle)
							{
								ele_yoke_tilt1[ele_no_access_sel][0] = aputure_limit_angle_value[ele_no_access_sel].upper_limit_angle;
							}
							if(ele_yoke_tilt1[ele_no_access_sel][1] > aputure_limit_angle_value[ele_no_access_sel].upper_limit_angle)
							{
								ele_yoke_tilt1[ele_no_access_sel][1] = aputure_limit_angle_value[ele_no_access_sel].upper_limit_angle;
							}
							if(ele_yoke_tilt1[ele_no_access_sel][1] < aputure_limit_angle_value[ele_no_access_sel].lower_limit_angle)
							{
								ele_yoke_tilt1[ele_no_access_sel][1] = aputure_limit_angle_value[ele_no_access_sel].lower_limit_angle;
							}
						}
						else
						{
							if(ele_yoke_tilt1[ele_no_access_sel][0] < ANGLE_LIMIT_DEFAULT_MIN)
							{
								ele_yoke_tilt1[ele_no_access_sel][0] = ANGLE_LIMIT_DEFAULT_MIN;
							}
							if(ele_yoke_tilt1[ele_no_access_sel][0] > ANGLE_LIMIT_DEFAULT_MAX)
							{
								ele_yoke_tilt1[ele_no_access_sel][0] = ANGLE_LIMIT_DEFAULT_MAX;
							}
							if(ele_yoke_tilt1[ele_no_access_sel][1] > ANGLE_LIMIT_DEFAULT_MAX)
							{
								ele_yoke_tilt1[ele_no_access_sel][1] = ANGLE_LIMIT_DEFAULT_MAX;
							}
							if(ele_yoke_tilt1[ele_no_access_sel][1] < ANGLE_LIMIT_DEFAULT_MIN)
							{
								ele_yoke_tilt1[ele_no_access_sel][1] = ANGLE_LIMIT_DEFAULT_MIN;
							}
						}
						read_body.emotion_live_body.emotion_live_packet1.rotation_pitch_max = ble_const_convert_symbol(ele_yoke_tilt1[ele_no_access_sel][1]);
						read_body.emotion_live_body.emotion_live_packet1.rotation_pitch_min = ble_const_convert_symbol(ele_yoke_tilt1[ele_no_access_sel][0]);
						read_body.emotion_live_body.emotion_live_packet1.rotation_laterial_max = ele_yoke_pan1[ele_no_access_sel][1];
						read_body.emotion_live_body.emotion_live_packet1.rotation_laterial_min = ele_yoke_pan1[ele_no_access_sel][0];
					}
				}
                read_body.emotion_live_body.emotion_live_packet1.rotation_pitch = ble_const_convert_symbol(motor_info.tilt_angle);
                ble_protocol_data_pack_up(tx_data, &read_body, BT_CommandType_Emotion_Live, 0);
                ble_send_data_queue(BLE_MESH_DATA, tx_data, 10);  
                
                if(motor_info.fresnel_angle < 18)
                    motor_info.fresnel_angle = 18;
                read_body.emotion_live_body.emotion_live_packet2.target_field = 2;
                read_body.emotion_live_body.emotion_live_packet2.packet_index = 1;
                read_body.emotion_live_body.emotion_live_packet2.zoom_angle = motor_info.fresnel_angle;
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

                read_body.emotion_live_body.emotion_live_packet4.target_field = 3;
                read_body.emotion_live_body.emotion_live_packet4.packet_index = 3;
                read_body.emotion_live_body.emotion_live_packet4.brandoor_left = 0;
                read_body.emotion_live_body.emotion_live_packet4.brandoor_right = 0;
                read_body.emotion_live_body.emotion_live_packet4.location_x = 0;
                read_body.emotion_live_body.emotion_live_packet4.location_y = 0;
                read_body.emotion_live_body.emotion_live_packet4.location_z = 0;
				if(access_info.fresnel_state == 1 || access_info.optical_20_access_state == 1 || 
				   access_info.optical_30_access_state == 1 || access_info.optical_50_access_state == 1)
				{
					lock[0] = ui_accessory_pan_lock_get(0);
					lock[1] = ui_accessory_tilt_lock_get(0);
				}
				else
				{
					lock[0] = ui_accessory_pan_lock_get(1);
					lock[1] = ui_accessory_tilt_lock_get(1);
				}
                read_body.emotion_live_body.emotion_live_packet4.lock_lateral = lock[0];
				read_body.emotion_live_body.emotion_live_packet4.lock_pitch = lock[1];
                ble_protocol_data_pack_up(tx_data, &read_body, BT_CommandType_Emotion_Live, 0);
                ble_send_data_queue(BLE_MESH_DATA, tx_data, 10);                               
            }
            break;
			
			case BT_CommandType_READ_SN:
			{
				struct sys_info_uuid  production_uuid;
				data_center_read_sys_info(SYS_INFO_UUID, &production_uuid);
				if(read_body.sn_get_body.package_type == 0)
				{
					read_body.sn_get_body.Character_0 = production_uuid.uuid[0] - '0';
					read_body.sn_get_body.Character_1 = production_uuid.uuid[1] - '0';
					read_body.sn_get_body.Character_2 = production_uuid.uuid[2] - '0';
					read_body.sn_get_body.Character_3 = production_uuid.uuid[3] - '0';
					read_body.sn_get_body.Character_4 = production_uuid.uuid[4] - '0';
					read_body.sn_get_body.Character_5 = production_uuid.uuid[5] - '0';
					read_body.sn_get_body.Character_6 = production_uuid.uuid[6] - '0';
				}
				else
				{
					read_body.sn_get_body.Character_0 = production_uuid.uuid[7] - '0';
					read_body.sn_get_body.Character_1 = production_uuid.uuid[8] - '0';
					read_body.sn_get_body.Character_2 = production_uuid.uuid[9] - '0';
					read_body.sn_get_body.Character_3 = production_uuid.uuid[10] - '0';
					read_body.sn_get_body.Character_4 = production_uuid.uuid[11] - '0';
				}
				break;
			}
			default: break;
		}
	}
	else  
	{
		data_center_read_sys_info(SYS_INFO_DMX, &dmx_link_state); 
		if(dmx_link_state.eth_linked == 1 || dmx_link_state.wired_linked == 1 || dmx_link_state.wireless_linked == 1)  //如果当前不在DMX界面
			return;
		if((PAGE_UPDATE == screen_get_act_pid() && screen_get_act_spid() >= SUB_ID_UPDATING) || 
		  (PAGE_WORK_MODE == screen_get_act_pid() && screen_get_act_spid() == SUB_ID_SYNC_UPDATAING))
			return;
        app_ble_write_state_set(1);
		switch(rx_packet->Header.Command_Type)
		{
			case BT_CommandType_Dimming_Curve:			//调光变化曲线数据包解析(0->2 1->0 2->1 3->3)
				if(0 == rx_packet->Body.Dimming_Curve_Body.Curve)
					set_light_curve(0);
				else if(1 == rx_packet->Body.Dimming_Curve_Body.Curve)
					set_light_curve(1);
				else if(2 == rx_packet->Body.Dimming_Curve_Body.Curve)
					set_light_curve(2);
				else
					set_light_curve(3);
				data_center_write_config_data(SYS_CONFIG_CURVE_TYPE, (uint8_t *)get_light_curve());
				if(ui_get_master_mode())
				{
					extern void gui_data_sync_event_gen(void);		
					gui_data_sync_event_gen();
				}
				break;
			case BT_CommandType_Sleep_Mode:				//开关数据包解析
			{
				uint8_t effects_mode;
				
				struct sys_info_power info;
				data_center_read_sys_info(SYS_INFO_POWER, &info);
				if(info.state == rx_packet->Body.Sleep_Mode_Body.Mode)return;
				if(rx_packet->Body.Sleep_Mode_Body.Mode)
				{
					ble_publish_power_onoff_event(1);
				}
				else
				{
					ble_publish_power_onoff_event(0);
				}
				if(ui_get_master_mode())
				{
					gui_power_event_gen();
					effects_mode = date_center_get_light_mode();
					if(effects_mode != LIGHT_MODE_DMX)
						gui_data_sync_event_gen();
				}
			}
			break;
            case BT_CommandType_Fan_Speed:
            {
                struct sys_info_ctrl_box sys_info;
                uint8_t fan_mode = 0;
                sys_info.fan_speed = rx_packet->Body.Fan_Speed_Body.Fixture_Speed;
                sys_info.mode = 1; // 蓝牙模式
                
                switch(rx_packet->Body.Fan_Speed_Body.Mode)
                {
					case 1: fan_mode = 0;break;
					case 4: fan_mode = 1;break;
					case 5: fan_mode = 2;break;
					case 7: fan_mode = 3;break;
					default:break;
                }
                data_center_write_config_data(SYS_CONFIG_FAN_MODE, &fan_mode);
				if(ui_get_master_mode())
				{
					extern void gui_data_sync_event_gen(void);		
					gui_data_sync_event_gen();
				}
                data_center_write_sys_info(SYS_INFO_CTRL, &sys_info);
                break;
            }
			case BT_CommandType_Light_Bright:	// 主亮度解析
			{
				uint8_t effects_mode = 0;
				uint8_t page_id = 7;
				
				effects_mode = date_center_get_light_mode();
				if(effects_mode == LIGHT_MODE_DMX)
				{
					data_center_read_config_data(SYS_CONFIG_EFFECTS_MODE, &page_id);
					effects_mode = ui_get_effectts_mode(page_id);
				}
				ble_int_change(effects_mode, rx_packet->Body.Light_Bright_Body.Int);
			}
			break;
			case BT_CommandType_Factory_Reset:	// 恢复出厂设置
				if(ui_get_master_mode())
				{
					extern void gui_factory_event_gen(void);		
					gui_factory_event_gen();
				}
                ui_restore_def_setting();
				ui_set_hs_mode_state(1);
				ui_set_motor_reset();
				user_turn_to_page(PAGE_FACTORY, 1, false);
			break;
			case BT_CommandType_APP_Ctrl:		// 蓝牙复位
				start_ble_reset();
				if(ui_get_master_mode())
				{
					extern void gui_ble_reset_event_gen(void);		
					gui_ble_reset_event_gen();
				}	
                user_turn_to_page(PAGE_CONTROL_SYSTEM, SUB_ID_BLE_RESETING, false);
			break;
			case BT_CommandType_High_speed:
			{
				uint8_t hsmode; 
				hsmode = rx_packet->Body.high_speed_body.state ;
				ui_set_hs_mode_state(hsmode);	
				data_center_write_config_data(SYS_CONFIG_HIGH_SPEED_MODE, &hsmode);
				if(ui_get_master_mode())
				{
					extern void gui_data_sync_event_gen(void);		
					gui_data_sync_event_gen();
				}
			}
			break;
            case BT_CommandType_Emotion_Live:
            {
                int16_t pan[12][2] = {0};
                int16_t tilt[12][2] = {0};
				int16_t pan1[2] = {0};
                int16_t tilt1[2] = {0};
				int16_t angle = 0;
				uint8_t angle_flag[2] = {0};
				uint8_t tiltm = 0;
                struct sys_info_motor        motor_info;
                struct sys_info_accessories  access_info;
                struct sys_info_motor_state  motor_state;
                uint8_t acces_sel = 0;
                
                data_center_read_sys_info(SYS_INFO_MOTOR_STATE, &motor_state);
                data_center_read_sys_info(SYS_INFO_MOTOR, &motor_info);
                data_center_read_sys_info(SYS_INFO_ACCESS, &access_info);
				
				ui_fresnel_get_angle_flag_set(1);
				switch(rx_packet->Body.emotion_live_body.emotion_live_packet1.packet_index)
				{
					case 0:
						if(motor_state.tilt_stall_state == 1 || motor_state.pan_stall_state == 1)
						{
							return;
						}
						angle = rx_packet->Body.emotion_live_body.emotion_live_packet1.rotation_laterial;
						if(motor_info.pan_angle != angle)
						{
							motor_info.pan_angle = angle;
							angle_flag[0] = 1;
						}
						angle = rx_packet->Body.emotion_live_body.emotion_live_packet1.rotation_pitch;
						angle = ble_symbol_convert_const(angle);
						if(motor_info.tilt_angle != angle)
						{
							motor_info.tilt_angle = angle;
							angle_flag[1] = 1;
						}
						if(access_info.yoke_state != 1)
							break;
						if(access_info.fresnel_state != 1 && access_info.optical_20_access_state != 1 && access_info.optical_30_access_state != 1 && access_info.optical_50_access_state != 1)
						{
							data_center_read_config_data(SYS_CONFIG_NOT_ACCESS_SELECT, &acces_sel);
							if(acces_sel > 10)
								acces_sel = 10;
							data_center_read_config_data(SYS_CONFIG_YOKE_NO_PAN_LIMIT, &pan);
							data_center_read_config_data(SYS_CONFIG_YOKE_NO_TILT_LIMIT, &tilt);
							if(angle_flag[0] == 1)
							{
								angle_flag[0] = 0;
								if(motor_info.pan_angle <= pan[acces_sel][0])
									motor_info.pan_angle = pan[acces_sel][0];
								if(motor_info.pan_angle >= pan[acces_sel][1])
									motor_info.pan_angle = pan[acces_sel][1];
								data_center_write_sys_info(SYS_INFO_MOTOR, &motor_info);
								data_center_write_config_data_no_event(SYS_CONFIG_YOKE_PAN, &motor_info.pan_angle);
								app_light_flags_set(LIGHT_RS485_YOKE_PAN_FLAG);
							}
						   if(angle_flag[1] == 1)
							{
								angle_flag[1] = 0;
								if(motor_info.tilt_angle <= tilt[acces_sel][0])
									motor_info.tilt_angle = tilt[acces_sel][0];
								if(motor_info.tilt_angle >= tilt[acces_sel][1])
									motor_info.tilt_angle = tilt[acces_sel][1];
								data_center_write_sys_info(SYS_INFO_MOTOR, &motor_info);
								data_center_write_config_data_no_event(SYS_CONFIG_YOKE_TILT, &motor_info.tilt_angle);
								app_light_flags_set(LIGHT_RS485_YOKE_TILT_FLAG);
							}
							if(PAGE_MENU == user_get_act_pid())
							{
								if(acces_sel >= 8)
									screen_load_page(PAGE_ELECT_ACCESSORY, SUB_ID_ELE_NO_YOKE, false);
								else
									screen_load_page(PAGE_ELECT_ACCESSORY, SUB_ID_ELE_APTURE_NO_YOKE, false);
							}
							else if((PAGE_ELECT_ACCESSORY != user_get_act_pid()) || (PAGE_ELECT_ACCESSORY == user_get_act_pid() && SUB_ID_ELE_NO_YOKE != screen_get_act_spid()))
							{
								if(acces_sel >= 8)
									user_enter_ele_access(PAGE_ELECT_ACCESSORY, SUB_ID_ELE_NO_YOKE);
								else
									screen_load_page(PAGE_ELECT_ACCESSORY, SUB_ID_ELE_APTURE_NO_YOKE, false);
							}
						}
						else
						{
							data_center_read_config_data(SYS_CONFIG_ANGLE_LIMIT, &tiltm);
							if(angle_flag[0] == 1)
							{
								angle_flag[0] = 0;
								data_center_read_config_data(SYS_CONFIG_YOKE_PAN_LIMIT, &pan1);
								if(access_info.fresnel_state == 1 || access_info.optical_20_access_state == 1)
								{
									if(tiltm == 1)
									{
										if(pan1[0] <= ANGLE_LIMIT_MIN)
											pan1[0] = ANGLE_LIMIT_MIN;
										if(pan1[1] >= ANGLE_LIMIT_MAX)
											pan1[1] = ANGLE_LIMIT_MAX;
									}
									else
									{
										pan1[0] = ANGLE_LIMIT_MIN;
										pan1[1] = ANGLE_LIMIT_MAX;
									}
								}
								if(motor_info.pan_angle <= pan1[0])
									motor_info.pan_angle = pan1[0];
								if(motor_info.pan_angle >= pan1[1])
									motor_info.pan_angle = pan1[1];
								data_center_write_sys_info(SYS_INFO_MOTOR, &motor_info);
								data_center_write_config_data_no_event(SYS_CONFIG_YOKE_PAN, &motor_info.pan_angle);
								app_light_flags_set(LIGHT_RS485_YOKE_PAN_FLAG);
							}
							if(angle_flag[1] == 1)
							{
								angle_flag[1] = 0;
								data_center_read_config_data(SYS_CONFIG_YOKE_TILT_LIMIT, &tilt1);
								if(access_info.fresnel_state == 1 || access_info.optical_20_access_state == 1)
								{
									if(tiltm == 1)
									{
										if(tilt1[0] <= -120)
											tilt1[0] = -120;
										if(tilt1[1] >= 120)
											tilt1[1] = 120;
									}
									else
									{
										tilt1[0] = -120;
										tilt1[1] = 120;
									}
								}
								if(motor_info.tilt_angle <= tilt1[0])
									motor_info.tilt_angle = tilt1[0];
								if(motor_info.tilt_angle >= tilt1[1])
									motor_info.tilt_angle = tilt1[1];
								data_center_write_sys_info(SYS_INFO_MOTOR, &motor_info);
								data_center_write_config_data_no_event(SYS_CONFIG_YOKE_TILT, &motor_info.tilt_angle);
								app_light_flags_set(LIGHT_RS485_YOKE_TILT_FLAG);
							}
							if(PAGE_MENU == user_get_act_pid())   
							{
								screen_load_page(PAGE_ELECT_ACCESSORY, SUB_ID_ELE_YOKE, true);
							}
							else if((PAGE_ELECT_ACCESSORY != user_get_act_pid()) || (PAGE_ELECT_ACCESSORY == user_get_act_pid() && SUB_ID_ELE_YOKE != screen_get_act_spid()))
							{
								user_enter_ele_access(PAGE_ELECT_ACCESSORY, SUB_ID_ELE_YOKE);
							}
						}
					break;
					case 1:
						if(motor_state.fresnel_stall_state)
						{
							break;
						}
						if(access_info.fresnel_state != 1)
						{
							break;
						}
						if(motor_info.fresnel_angle != rx_packet->Body.emotion_live_body.emotion_live_packet2.zoom_angle)
						{
							motor_info.fresnel_angle = rx_packet->Body.emotion_live_body.emotion_live_packet2.zoom_angle;
							data_center_write_config_data(SYS_CONFIG_FRESNEL_ANGLE, &motor_info.fresnel_angle);
							data_center_write_sys_info(SYS_INFO_MOTOR, &motor_info);
							app_light_flags_set(LIGHT_RS485_FRESNEL_ANGLE_FLAG);
						}
						if(PAGE_MENU == user_get_act_pid())   
						{
							screen_load_page(PAGE_ELECT_ACCESSORY, SUB_ID_ELE_FRESNEL, true);
						}
						else if((PAGE_ELECT_ACCESSORY != user_get_act_pid()) || (PAGE_ELECT_ACCESSORY == user_get_act_pid() && SUB_ID_ELE_FRESNEL != screen_get_act_spid()))
						{
							user_enter_ele_access(PAGE_ELECT_ACCESSORY, SUB_ID_ELE_FRESNEL);
						}
					break;
					case 2:
						
					break;
					case 3:
					{
						uint8_t lock[2] = {0};
						
						if(access_info.yoke_state != 1)
							break;
						if(access_info.fresnel_state == 1 || access_info.optical_20_access_state == 1 || access_info.optical_30_access_state == 1 || access_info.optical_50_access_state == 1)
						{
							lock[0] = rx_packet->Body.emotion_live_body.emotion_live_packet4.lock_pitch;
							lock[1] = rx_packet->Body.emotion_live_body.emotion_live_packet4.lock_lateral;
							if(lock[0] != ui_accessory_tilt_lock_get(0))
							{
								ui_accessory_tilt_lock_set(0, lock[0]);
								page_set_invalid_type(PAGE_INVALID_TYPE_REFRESH);
							}
							if(lock[1] != ui_accessory_pan_lock_get(0))
							{
								ui_accessory_pan_lock_set(0, lock[1]);
								page_set_invalid_type(PAGE_INVALID_TYPE_REFRESH);
							}
						}
						else
						{
							lock[0] = rx_packet->Body.emotion_live_body.emotion_live_packet4.lock_pitch;
							lock[1] = rx_packet->Body.emotion_live_body.emotion_live_packet4.lock_lateral;
							if(lock[0] != ui_accessory_tilt_lock_get(1))
							{
								ui_accessory_tilt_lock_set(1, lock[0]);
								page_set_invalid_type(PAGE_INVALID_TYPE_REFRESH);
							}
							if(lock[1] != ui_accessory_pan_lock_get(1))
							{
								ui_accessory_pan_lock_set(1, lock[1]);
								page_set_invalid_type(PAGE_INVALID_TYPE_REFRESH);
							}
						}
					}
					break;
					default:break;
				}
            }
            break;
			default:write_light_mode(&rx_packet->Body, rx_packet->Header.Command_Type);break;	
		}	
	}
}


/* 发送IOT数据的回调函数 */
void os_timer_iot_callback(void *argument)
{
    if(s_iot_send_cnt < IOT_FRAME_NUM)
    {
        ble_send_data_queue(BLE_MESH_DATA, g_iot_send_data.buf[s_iot_send_cnt], 10);
        s_iot_send_cnt++;
    }
    else
    {
        osTimerStop(os_timer_iot);
    }
}



void  ble_int_change(uint8_t mode ,uint16_t lightness)
{
	int res = 0;
	
	res = data_center_write_brightness(mode, lightness);
	
	(void)res;
}

void app_ble_write_state_set(uint8_t state)
{
    ble_write_flag = state;
}

uint8_t app_ble_write_state_get(void)
{
    return ble_write_flag;
}










































