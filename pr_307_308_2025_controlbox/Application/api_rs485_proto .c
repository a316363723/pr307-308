#include "api_rs485_proto.h"
#include "app_data_center.h"
#include "app_ble_port.h"
#include "app_ble_protocol_parse.h"
#include "dev_rs485.h"
#include <stdint.h>
#include <stdarg.h>
#include <string.h>
#include "hal_timer.h"
#include "ui_data.h"
#include "page.h"
#include "hal_timer.h"
#include "hal_iic.h"
#include "SidusProFX.h"
#include "SidusPro_Interface.h"
#include "SidusProFile.h"
#include "crc32.h"
#include "dev_pwm_self.h"
#pragma pack ()

typedef struct
{
    char     name[10][10];
} rs485_cfx_info_type;


dev_lamp_t dev_lamp ;
#define DEV_RS485_VA_ARG_END    (NULL)
static uint8_t read_buff[200] = {0};
static struct sys_info_accessories s_access_info;

rs485_cfx_info_type rs485_cfx_info[3];


static const uint16_t rs485_cmd_arg_size[RS485_Cmd_Null] =
{
    sizeof(rs485_version_t),
    sizeof(rs485_hsi_t),
    sizeof(rs485_cct_t),
    sizeof(rs485_gel_t),
    sizeof(rs485_rgb_t),
    sizeof(rs485_xy_coord_t),
    sizeof(rs485_dimming_frq_t),
    sizeof(rs485_sys_fx_t),
    sizeof(rs485_dimming_curve_t),
    sizeof(rs485_fan_t),
    sizeof(rs485_power_t),
    sizeof(rs485_battery_t),
    sizeof(rs485_switch_t),
    sizeof(rs485_file_transfer_t),
    sizeof(rs485_temperature_msg_t),
    sizeof(rs485_sys_fx_2_t),
    sizeof(rs485_self_adaption_t),
    sizeof(rs485_factory_rgbww_t),
    sizeof(rs485_source_t),
    sizeof(rs485_illumination_mode_t),
    sizeof(rs485_color_mixing_t),
    sizeof(rs485_err_msg_t),
    sizeof(rs485_pfx_ctrl_t),
    sizeof(rs485_cfx_bank_rw_t),
    sizeof(rs485_cfx_ctrl_t),
    sizeof(rs485_cfx_preview_t),
    sizeof(rs485_mfx_ctrl_t),
    sizeof(rs485_rgbww_t),
    sizeof(rs485_cfx_name_t),
    sizeof(rs485_curr_light_mode_t),
    sizeof(rs485_pixel_fx_t),
    sizeof(rs485_dmx_strobe_t),
    sizeof(rs485_partition_color_t),
    sizeof(rs485_partition_fx_t),
    sizeof(rs485_high_speed_mode_t),
    sizeof(rs485_analog_dim_t),
    sizeof(rs485_pump_t),    
    sizeof(rs485_self_adjust_t),  
    sizeof(rs485_motor_t),  
    sizeof(rs485_access_t),  
	sizeof(rs485_version2_t),  		
	sizeof(rs485_factory_rgbww_crc_t),  
	sizeof(rs485_clear_run_time_t), 
};



static uint8_t check_sum_calc(const uint8_t* buff, uint16_t size)
{
    uint8_t check_sum = 0;

    while(size--)
    {
        check_sum += *buff++;
    }

    return check_sum;
}

static int receive_message_check(const rs485_proto_header_t* p_send_header, const rs485_proto_header_t* p_receive_header)
{
    if(p_receive_header->start != RS485_START_CODE)
        return -1;

    if(p_receive_header->msg_size > RS485_MESSAGE_MAX_SIZE)
        return -2;

    if(p_receive_header->serial_num != p_send_header->serial_num)
        return -3;

    if(p_receive_header->check_sum != check_sum_calc((uint8_t*)&p_receive_header->header_size, p_receive_header->msg_size - 2))
        return -4;

    return 0;
}

static void init_message_header(rs485_proto_header_t* p_header, bool ack_en, uint16_t serial_num, uint16_t msg_size)
{
    p_header->start = RS485_START_CODE;
    p_header->header_size = sizeof(rs485_proto_header_t);
    p_header->proto_ver_major = RS485_PROTO_VER_MAJOR;
    p_header->proto_ver_minor = RS485_PROTO_VER_MINOR;
    p_header->proto_ver_revision = RS485_PROTO_VER_REVISION;
    p_header->serial_num = serial_num;
    p_header->ack_en = ack_en;
    p_header->msg_size = msg_size;
    p_header->check_sum = check_sum_calc((uint8_t*)&p_header->header_size, p_header->msg_size - 2);
}

static uint16_t write_cmd_pack_up_and_copy(uint8_t* p_pack_up_buff, const rs485_cmd_arg_t* p_cmd_arg, rs485_cmd_enum cmd_type)
{
    uint16_t cmd_body_size = sizeof(rs485_cmd_header_t);
    rs485_cmd_header_t header = {1, cmd_type, rs485_cmd_arg_size[cmd_type]};

    if(cmd_type >= RS485_Cmd_Null)
        return 0;

    cmd_body_size += rs485_cmd_arg_size[cmd_type];
    memcpy(p_pack_up_buff, &header, sizeof(rs485_cmd_header_t));
    memcpy((void*)(p_pack_up_buff + sizeof(rs485_cmd_header_t)), p_cmd_arg, header.arg_size);
    return cmd_body_size;
}

static uint16_t read_cmd_pack_up_and_copy(uint8_t* p_pack_up_buff, const rs485_cmd_arg_t* p_cmd_arg, rs485_cmd_enum cmd_type)
{
    uint16_t cmd_body_size = sizeof(rs485_cmd_header_t);
    rs485_cmd_header_t header = {0, cmd_type, 0};
    uint16_t cmd_arg_size = 0;

    if(cmd_type >= RS485_Cmd_Null)
        return 0;

    switch (cmd_type)
    {
        case RS485_Cmd_Sys_FX:
            cmd_arg_size = 1;
            break;

        case RS485_Cmd_FileTransfer:
            switch (p_cmd_arg->file_transfer.step)
            {
                case RS485_File_TX_Step_Start:
                    if(RS485_File_TX_CFX == p_cmd_arg->file_transfer.file_type)
                    {
                        cmd_arg_size = 2;
                    }
                    else if(RS485_File_TX_Firmware == p_cmd_arg->file_transfer.file_type)
                    {
                        cmd_arg_size = 2;
                    }

                    break;

                case RS485_File_TX_Step_Size:
                    break;

                case RS485_File_TX_Step_Data:
                    cmd_arg_size = 2;
                    break;

                case RS485_File_TX_Step_CRC:
                    break;

                default:
                    break;
            }

            break;

        case RS485_Cmd_Sys_FX_II:
            cmd_arg_size = 1;
            break;
        case RS485_Cmd_Fan:
        case RS485_Cmd_Battery_State:
        case RS485_Cmd_Temperature_Msg:
        case RS485_Cmd_Self_Adaption:
        case RS485_Cmd_CFX_Name:
        case RS485_Cmd_High_speed:
        case RS485_Cmd_Analog_Dim:
        case RS485_Cmd_Pump:
        case RS485_Cmd_Motor:
        case RS485_Cmd_Accessories:
		case RS485_Cmd_Version2:
		case RS485_Cmd_Factory_RGBWW_Crc:   	//0x29
	    case RS485_Cmd_Clear_Run_Time:   	//0x30
            cmd_arg_size = rs485_cmd_arg_size[cmd_type];
            break;

        default:
            break;
    }

    header.arg_size = cmd_arg_size;
    cmd_body_size += cmd_arg_size;
    memcpy(p_pack_up_buff, &header, sizeof(rs485_cmd_header_t));

    if(0 != cmd_arg_size)
    {
        memcpy((void*)(p_pack_up_buff + sizeof(rs485_cmd_header_t)), p_cmd_arg, header.arg_size);
    }

    return cmd_body_size;
}

int dev_rs485_cmd_write(struct dev_rs485* p_dev, int retry_times, uint32_t retry_ticks, bool ack_en, void* p_ack, ...)
{
    int res = 0;
    int read_size = 0;
    int res_read = -1;
    va_list list;
    uint32_t ticks_cnt = 0;
    rs485_cmd_body_t* p_body = NULL;
    rs485_proto_header_t* p_send_head = (rs485_proto_header_t*)p_dev->write_buff;
    rs485_proto_header_t* p_receive_head = (rs485_proto_header_t*)p_dev->read_buff;
    rs485_ack_body_t* p_ack_body = NULL;
    uint8_t* p_pack_up_buff = (uint8_t*)((uint32_t)p_dev->write_buff + sizeof(rs485_proto_header_t));

    va_start(list, p_ack);

    while((bool)(p_body = va_arg(list, rs485_cmd_body_t*)) && DEV_RS485_VA_ARG_END != p_body)
    {
        p_pack_up_buff += write_cmd_pack_up_and_copy(p_pack_up_buff, &p_body->cmd_arg, p_body->header.cmd_type);
    }

    init_message_header(p_send_head, ack_en, p_dev->message_serial_num++, (uint32_t)p_pack_up_buff - (uint32_t)p_dev->write_buff);

    do
    {
        ticks_cnt = retry_ticks;
        p_dev->write_data(p_dev->write_buff, p_send_head->msg_size, 100);

        while(ticks_cnt--)
        {
            read_size = p_dev->read_data(p_dev->read_buff, 0xFFFF, 1);

            if(read_size > 0 && 0 == receive_message_check(p_send_head, p_receive_head))
            {
                res_read = 0;
                break;
            }
        }
    }
    while(ack_en && --retry_times > 0 && 0 != res_read);

    if(0 != res_read)
    {
        res = -1;
        goto out;
    }

    p_ack_body = (rs485_ack_body_t*)((uint32_t)p_dev->read_buff + p_receive_head->header_size);
    va_start(list, p_ack);

    while( (bool)(p_body = va_arg(list, rs485_cmd_body_t*)) && DEV_RS485_VA_ARG_END != p_body )
    {
        if(p_body->header.cmd_type != p_ack_body->header.cmd_type)
        {
            res = -2;
            break;
        }
		if(p_ack_body->ack_arg.ack != RS485_Ack_Ok)
        {
            res = p_ack_body->ack_arg.ack;
            break;

        }
        p_ack_body++;
    }

out:

    if(ack_en && NULL != p_ack)
    {
        memcpy(p_ack, (void*)((uint32_t)p_dev->read_buff + sizeof(rs485_proto_header_t)), read_size - sizeof(rs485_proto_header_t));
    }

    va_end(list);
    return res;
}

int dev_rs485_cmd_read(struct dev_rs485* p_dev, int retry_times, uint32_t retry_ticks, bool ack_en, void* p_read_body, ...)
{
    int res = 0;
    int read_size = 0;
    int res_read = -1;
    va_list list;
    uint32_t ticks_cnt = 0;
    rs485_cmd_body_t* p_body = NULL;
    rs485_proto_header_t* p_send_head = (rs485_proto_header_t*)p_dev->write_buff;
    rs485_proto_header_t* p_receive_head = (rs485_proto_header_t*)p_dev->read_buff;
    rs485_cmd_body_t* p_rcv_body = NULL;
    uint8_t* p_pack_up_buff = (uint8_t*)((uint32_t)p_dev->write_buff + sizeof(rs485_proto_header_t));

    va_start(list, p_read_body);

    while((bool)(p_body = va_arg(list, rs485_cmd_body_t*)) && DEV_RS485_VA_ARG_END != p_body)
    {
        p_pack_up_buff += read_cmd_pack_up_and_copy(p_pack_up_buff, &p_body->cmd_arg, p_body->header.cmd_type);
    }

    init_message_header(p_send_head, ack_en, p_dev->message_serial_num++, (uint32_t)p_pack_up_buff - (uint32_t)p_dev->write_buff);

    do
    {
        ticks_cnt = retry_ticks;
        p_dev->write_data(p_dev->write_buff, p_send_head->msg_size, 100);

        while(ticks_cnt--)
        {
            read_size = p_dev->read_data(p_dev->read_buff, 0xFFFF, 1);

            if(read_size > 0 && 0 == receive_message_check(p_send_head, p_receive_head))
            {
                res_read = 0;
                break;
            }
        }
    }
    while(ack_en && --retry_times > 0 && 0 != res_read);

    if(0 != res_read)
    {
        res = -1;
        goto out;
    }

    p_rcv_body = (rs485_cmd_body_t*)((uint32_t)p_dev->read_buff + p_receive_head->header_size);
    va_start(list, p_read_body);

    while((bool)(p_body = va_arg(list, rs485_cmd_body_t*)) && DEV_RS485_VA_ARG_END != p_body)
    {
        if(p_body->header.cmd_type != p_rcv_body->header.cmd_type)
        {
            res = -2;
            break;
        }
        p_rcv_body = (rs485_cmd_body_t*)((uint32_t)p_rcv_body + sizeof(rs485_cmd_header_t) + p_rcv_body->header.arg_size);
    }

    if(ack_en && NULL != p_read_body)
    {
        memcpy(p_read_body, (void*)((uint32_t)p_dev->read_buff + sizeof(rs485_proto_header_t)), read_size - sizeof(rs485_proto_header_t));
    }

out:
    va_end(list);
    return res;
}


bool rs485_get_cfx_name(dev_lamp_t* dev_lamp, bool ack_need, uint32_t retry_tick, uint32_t retry_times, uint8_t type ,uint8_t bank, char* name)
{
    bool ack = false;
    rs485_cmd_body_t cmd_body;
    rs485_cmd_body_t read_buff;
    cmd_body.header.cmd_type = RS485_Cmd_CFX_Name;
    cmd_body.cmd_arg.cfx_name.bank = bank;
    cmd_body.cmd_arg.cfx_name.code_type = 0;
    cmd_body.cmd_arg.cfx_name.effect_type = type;
    
    if(0 == dev_rs485_cmd_read(&dev_lamp->rs485, retry_tick, retry_times, true, (rs485_cmd_body_t *)&read_buff, &cmd_body, DEV_RS485_VA_ARG_END))
    {
        memcpy(name, read_buff.cmd_arg.cfx_name.name, 10);
        ack = 1;
    }
	else
	{
		memcpy(name, "NO FX", 10);
	}
    return ack;
}

bool rs485_set_power_mode(dev_lamp_t* dev_lamp, bool ack_need, uint32_t retry_times, uint32_t retry_tick, uint8_t power_mode, uint16_t Ratio_value, uint8_t save_ctr)
{
    bool ack = false;
    rs485_cmd_body_t cmd_body;
    cmd_body.header.cmd_type = RS485_Cmd_IlluminationMode;
    cmd_body.cmd_arg.illumination_mode.mode = power_mode;
    cmd_body.cmd_arg.illumination_mode.ratio = Ratio_value;
    cmd_body.cmd_arg.illumination_mode.save_ctr = save_ctr;
    ack = 0 == dev_rs485_cmd_write(&dev_lamp->rs485, retry_times, retry_tick, true, NULL, &cmd_body, DEV_RS485_VA_ARG_END) ? true : false;
    return ack;
}

bool rs485_set_highspeed_mode(dev_lamp_t* dev_lamp, bool ack_need, uint32_t retry_times, uint32_t retry_tick, uint8_t highspeed_mode)
{
    bool ack = false;
    rs485_cmd_body_t cmd_body;
    cmd_body.header.cmd_type = RS485_Cmd_High_speed;
    cmd_body.cmd_arg.high_speed_mode.mode = highspeed_mode;
    ack = 0 == dev_rs485_cmd_write(&dev_lamp->rs485, retry_times, retry_tick, true, NULL, &cmd_body, DEV_RS485_VA_ARG_END) ? true : false;
    return ack;
}

bool rs485_get_motor_reset(dev_lamp_t* dev_lamp, bool ack_need, uint32_t retry_times, uint32_t retry_tick, rs485_motor_type_enum motor_type)
{
    bool ack = false;
    rs485_cmd_body_t cmd_body;
	rs485_cmd_body_t read_buff;
    cmd_body.header.cmd_type = RS485_Cmd_Motor;
    cmd_body.cmd_arg.motor_t.type = motor_type;
	if( cmd_body.cmd_arg.motor_t.type  > 2)
	{
		__nop();
	}
    cmd_body.cmd_arg.motor_t.mode_arg.motor_mode = RS485_Motor_Mode_Reset;
    if(0 == dev_rs485_cmd_read(&dev_lamp->rs485, retry_times, retry_tick, true, (rs485_cmd_body_t *)&read_buff, &cmd_body, DEV_RS485_VA_ARG_END))
	{
		if(read_buff.cmd_arg.motor_t.mode_arg.motor_arg.motor_reset_arg.reset_state == 1)
			ack = true;
	}
	
	return ack;
}

bool rs485_set_motor_reset(dev_lamp_t* dev_lamp, bool ack_need, uint32_t retry_times, uint32_t retry_tick, rs485_motor_type_enum motor_type)
{
    bool ack = false;
    rs485_cmd_body_t cmd_body;
    cmd_body.header.cmd_type = RS485_Cmd_Motor;
    cmd_body.cmd_arg.motor_t.type = motor_type;
    cmd_body.cmd_arg.motor_t.mode_arg.motor_mode = RS485_Motor_Mode_Reset;
    cmd_body.cmd_arg.motor_t.mode_arg.motor_arg.motor_reset_arg.reset_state = 1;
	data_center_read_sys_info(SYS_INFO_ACCESS, &s_access_info);
    if(motor_type == RS485_Fresnel_Motor)
        cmd_body.cmd_arg.motor_t.mode_arg.motor_arg.motor_reset_arg.reset_angle = 50;
    else if(motor_type == RS485_Pitch_Motor)
	{
		if(s_access_info.fresnel_state == 1)
			cmd_body.cmd_arg.motor_t.mode_arg.motor_arg.motor_reset_arg.reset_angle = 45;
		else
			cmd_body.cmd_arg.motor_t.mode_arg.motor_arg.motor_reset_arg.reset_angle = 90;
	}
    else if(motor_type == RS485_Rotale_Motor)
        cmd_body.cmd_arg.motor_t.mode_arg.motor_arg.motor_reset_arg.reset_angle = 0;
    ack = 0 == dev_rs485_cmd_write(&dev_lamp->rs485, retry_times, retry_tick, true, NULL, &cmd_body, DEV_RS485_VA_ARG_END) ? true : false;
    return ack;
}

bool rs485_set_motor_stal_value(dev_lamp_t* dev_lamp, bool ack_need, uint32_t retry_times, uint32_t retry_tick)
{
    bool ack = false;
    rs485_cmd_body_t cmd_body;
    rs485_cmd_body_t cmd_body1;
    rs485_cmd_body_t cmd_body2;
    
    cmd_body.header.cmd_type = RS485_Cmd_Motor;
    cmd_body.cmd_arg.motor_t.type = RS485_Fresnel_Motor;
    cmd_body.cmd_arg.motor_t.mode_arg.motor_mode = RS485_Motor_Mode_Crc;
    cmd_body.cmd_arg.motor_t.mode_arg.motor_arg.motor_crc_arg.crc_state = 0;
    cmd_body.cmd_arg.motor_t.mode_arg.motor_arg.motor_crc_arg.sttall_min_value = 10;
    cmd_body.cmd_arg.motor_t.mode_arg.motor_arg.motor_crc_arg.sttall_max_value = 90;
    cmd_body1.header.cmd_type = RS485_Cmd_Motor;
    cmd_body1.cmd_arg.motor_t.type = RS485_Pitch_Motor;
    cmd_body1.cmd_arg.motor_t.mode_arg.motor_mode = RS485_Motor_Mode_Crc;
    cmd_body1.cmd_arg.motor_t.mode_arg.motor_arg.motor_crc_arg.crc_state = 0;
    cmd_body1.cmd_arg.motor_t.mode_arg.motor_arg.motor_crc_arg.sttall_min_value = 10;
    cmd_body1.cmd_arg.motor_t.mode_arg.motor_arg.motor_crc_arg.sttall_max_value = 90;
    cmd_body2.header.cmd_type = RS485_Cmd_Motor;
    cmd_body2.cmd_arg.motor_t.type = RS485_Rotale_Motor;
    cmd_body2.cmd_arg.motor_t.mode_arg.motor_mode = RS485_Motor_Mode_Crc;
    cmd_body2.cmd_arg.motor_t.mode_arg.motor_arg.motor_crc_arg.crc_state = 0;
    cmd_body2.cmd_arg.motor_t.mode_arg.motor_arg.motor_crc_arg.sttall_min_value = 10;
    cmd_body2.cmd_arg.motor_t.mode_arg.motor_arg.motor_crc_arg.sttall_max_value = 90;
    ack = 0 == dev_rs485_cmd_write(&dev_lamp->rs485, retry_times, retry_tick, true, NULL, &cmd_body, &cmd_body1, &cmd_body2, DEV_RS485_VA_ARG_END) ? true : false;
    return ack;
}

bool rs485_get_motor_stall(dev_lamp_t* dev_lamp, bool ack_need, uint32_t retry_times, uint32_t retry_tick, uint8_t *stall_state)
{
    bool ack = false;
    rs485_cmd_body_t cmd_body;
    rs485_cmd_body_t cmd_body1;
    rs485_cmd_body_t cmd_body2;
    memset(read_buff, 0, sizeof(read_buff));
    rs485_cmd_body_t* p_read_body = (rs485_cmd_body_t*)read_buff;
    struct sys_info_motor_state  *motor_state = (struct sys_info_motor_state  *)stall_state;
    uint16_t body_size = 0;
    
    cmd_body.header.cmd_type = RS485_Cmd_Motor;
    cmd_body.cmd_arg.motor_t.type = RS485_Fresnel_Motor;
    cmd_body.cmd_arg.motor_t.mode_arg.motor_mode = RS485_Motor_Mode_State;
    cmd_body1.header.cmd_type = RS485_Cmd_Motor;
    cmd_body1.cmd_arg.motor_t.type = RS485_Pitch_Motor;
    cmd_body1.cmd_arg.motor_t.mode_arg.motor_mode = RS485_Motor_Mode_State;
    cmd_body2.header.cmd_type = RS485_Cmd_Motor;
    cmd_body2.cmd_arg.motor_t.type = RS485_Rotale_Motor;
    cmd_body2.cmd_arg.motor_t.mode_arg.motor_mode = RS485_Motor_Mode_State;
	cmd_body2.cmd_arg.motor_t.mode_arg.motor_arg.motor_state_arg.stall_state = 0;
	if( cmd_body.cmd_arg.motor_t.type > 2|| cmd_body1.cmd_arg.motor_t.type>2 || cmd_body2.cmd_arg.motor_t.type> 2)
	{
		__nop();
	}
    if(0 == dev_rs485_cmd_read(&dev_lamp->rs485, retry_times, retry_tick, true, &read_buff, &cmd_body, &cmd_body1, &cmd_body2, DEV_RS485_VA_ARG_END))
    {
        motor_state->fresnel_stall_state = p_read_body->cmd_arg.motor_t.mode_arg.motor_arg.motor_state_arg.stall_state;	
		motor_state->fresnel_loop_count = p_read_body->cmd_arg.motor_t.mode_arg.motor_arg.motor_state_arg.loop_count;
        body_size += sizeof(rs485_cmd_header_t) + p_read_body->header.arg_size;
        p_read_body = (rs485_cmd_body_t*)((uint32_t)read_buff + body_size);
        motor_state->tilt_stall_state = p_read_body->cmd_arg.motor_t.mode_arg.motor_arg.motor_state_arg.stall_state;
        body_size += sizeof(rs485_cmd_header_t) + p_read_body->header.arg_size;
        p_read_body = (rs485_cmd_body_t*)((uint32_t)read_buff + body_size);
        motor_state->pan_stall_state = p_read_body->cmd_arg.motor_t.mode_arg.motor_arg.motor_state_arg.stall_state;
	
        ack = true;
    }
    return ack;
}

bool rs485_set_motor_angle(dev_lamp_t* dev_lamp, bool ack_need, uint32_t retry_times, uint32_t retry_tick, rs485_motor_type_enum motor_type, float angle)
{
    bool ack = false;
    rs485_cmd_body_t cmd_body;
    cmd_body.header.cmd_type = RS485_Cmd_Motor;
    cmd_body.cmd_arg.motor_t.type = motor_type;
    cmd_body.cmd_arg.motor_t.mode_arg.motor_mode = RS485_Motor_Mode_Move;
    cmd_body.cmd_arg.motor_t.mode_arg.motor_arg.motor_move_arg.motor_angle = angle;
	data_center_read_sys_info(SYS_INFO_ACCESS, &s_access_info);
	if(s_access_info.fresnel_state == 1)
		cmd_body.cmd_arg.motor_t.mode_arg.motor_arg.motor_move_arg.move_state = 5;
	else if(s_access_info.optical_20_access_state == 1)
		cmd_body.cmd_arg.motor_t.mode_arg.motor_arg.motor_move_arg.move_state = 1;
	else if(s_access_info.optical_30_access_state == 1)
		cmd_body.cmd_arg.motor_t.mode_arg.motor_arg.motor_move_arg.move_state = 2;
	else if(s_access_info.optical_50_access_state == 1)
		cmd_body.cmd_arg.motor_t.mode_arg.motor_arg.motor_move_arg.move_state = 3;
	else if(s_access_info.adapter_ring == 1)
		cmd_body.cmd_arg.motor_t.mode_arg.motor_arg.motor_move_arg.move_state = 4;
	else
		cmd_body.cmd_arg.motor_t.mode_arg.motor_arg.motor_move_arg.move_state = 0;
	
	cmd_body.cmd_arg.motor_t.mode_arg.motor_arg.motor_move_arg.motor_star_speed = 0;
    cmd_body.cmd_arg.motor_t.mode_arg.motor_arg.motor_move_arg.motor_a1 = 0;
    cmd_body.cmd_arg.motor_t.mode_arg.motor_arg.motor_move_arg.motor_v1 = 0;
    cmd_body.cmd_arg.motor_t.mode_arg.motor_arg.motor_move_arg.motor_acce = 0;
    cmd_body.cmd_arg.motor_t.mode_arg.motor_arg.motor_move_arg.motor_targe_speed = 0;
    cmd_body.cmd_arg.motor_t.mode_arg.motor_arg.motor_move_arg.motor_dece = 0;
    cmd_body.cmd_arg.motor_t.mode_arg.motor_arg.motor_move_arg.motor_d1 = 0;
    cmd_body.cmd_arg.motor_t.mode_arg.motor_arg.motor_move_arg.motor_stop_speed = 0;
    ack = 0 == dev_rs485_cmd_write(&dev_lamp->rs485, retry_times, retry_tick, true, NULL, &cmd_body, DEV_RS485_VA_ARG_END) ? true : false;
    return ack;
}

bool rs485_set_motor_demo(dev_lamp_t* dev_lamp, bool ack_need)
{
    bool ack = false;
    rs485_cmd_body_t cmd_body;
	rs485_cmd_body_t cmd_body2;
	rs485_cmd_body_t cmd_body3;
	uint8_t state[3];
	int16_t limit[3][2];
	uint8_t state1;
	
	data_center_read_config_data(SYS_CONFIG_DEMO_STATE, &state1);
	data_center_read_config_data(SYS_CONFIG_PAN_DEMO_STATE, &state[0]);
	data_center_read_config_data(SYS_CONFIG_PAN_DEMO_LIMIT, &limit[0]);
	data_center_read_config_data(SYS_CONFIG_TILI_DEMO_STATE, &state[1]);
	data_center_read_config_data(SYS_CONFIG_TILI_DEMO_LIMIT, &limit[1]);
	data_center_read_config_data(SYS_CONFIG_ZOOM_DEMO_STATE, &state[2]);
	data_center_read_config_data(SYS_CONFIG_ZOOM_DEMO_LIMIT, &limit[2]);
	
    cmd_body.header.cmd_type = RS485_Cmd_Motor;
    cmd_body.cmd_arg.motor_t.type = RS485_Rotale_Motor;
    cmd_body.cmd_arg.motor_t.mode_arg.motor_mode = RS485_Motor_Mode_Crc;
	cmd_body.cmd_arg.motor_t.mode_arg.motor_arg.motor_crc_arg.crc_value = state[0];
	if(state1 == 1)
	{
		cmd_body.cmd_arg.motor_t.mode_arg.motor_arg.motor_crc_arg.crc_state = 2;
		cmd_body2.cmd_arg.motor_t.mode_arg.motor_arg.motor_crc_arg.crc_state = 2;
		cmd_body3.cmd_arg.motor_t.mode_arg.motor_arg.motor_crc_arg.crc_state = 2;
	}
	else
	{
		cmd_body.cmd_arg.motor_t.mode_arg.motor_arg.motor_crc_arg.crc_state = 3;
		cmd_body2.cmd_arg.motor_t.mode_arg.motor_arg.motor_crc_arg.crc_state = 3;
		cmd_body3.cmd_arg.motor_t.mode_arg.motor_arg.motor_crc_arg.crc_state = 3;
	}
	cmd_body.cmd_arg.motor_t.mode_arg.motor_arg.motor_crc_arg.sttall_min_value = limit[0][0];
	cmd_body.cmd_arg.motor_t.mode_arg.motor_arg.motor_crc_arg.sttall_max_value = limit[0][1];
	cmd_body2.header.cmd_type = RS485_Cmd_Motor;
    cmd_body2.cmd_arg.motor_t.type = RS485_Pitch_Motor;
    cmd_body2.cmd_arg.motor_t.mode_arg.motor_mode = RS485_Motor_Mode_Crc;
	cmd_body2.cmd_arg.motor_t.mode_arg.motor_arg.motor_crc_arg.crc_value = state[1];
	cmd_body2.cmd_arg.motor_t.mode_arg.motor_arg.motor_crc_arg.sttall_min_value = limit[1][0];
	cmd_body2.cmd_arg.motor_t.mode_arg.motor_arg.motor_crc_arg.sttall_max_value = limit[1][1];
	cmd_body3.header.cmd_type = RS485_Cmd_Motor;
    cmd_body3.cmd_arg.motor_t.type = RS485_Fresnel_Motor;
    cmd_body3.cmd_arg.motor_t.mode_arg.motor_mode = RS485_Motor_Mode_Crc;
	cmd_body3.cmd_arg.motor_t.mode_arg.motor_arg.motor_crc_arg.crc_value = state[2];
	cmd_body3.cmd_arg.motor_t.mode_arg.motor_arg.motor_crc_arg.sttall_min_value = limit[2][0];
	cmd_body3.cmd_arg.motor_t.mode_arg.motor_arg.motor_crc_arg.sttall_max_value = limit[2][1];
    ack = 0 == dev_rs485_cmd_write(&dev_lamp->rs485, 5, 100, true, NULL, &cmd_body, &cmd_body2, &cmd_body3, DEV_RS485_VA_ARG_END) ? true : false;
    return ack;
}

bool rs485_get_motor_angle(dev_lamp_t* dev_lamp, bool ack_need, uint32_t retry_times, uint32_t retry_tick, rs485_motor_type_enum motor_type, float *angle)
{
    bool ack = false;
    rs485_cmd_body_t cmd_body;
    memset(read_buff, 0, sizeof(read_buff));
    rs485_cmd_body_t* p_read_body = (rs485_cmd_body_t*)read_buff;
//    uint16_t body_size = 0;
    
    cmd_body.header.cmd_type = RS485_Cmd_Motor;
    cmd_body.cmd_arg.motor_t.type = motor_type;
    cmd_body.cmd_arg.motor_t.mode_arg.motor_mode = RS485_Motor_Mode_Move;
    cmd_body.cmd_arg.motor_t.mode_arg.motor_arg.motor_move_arg.motor_angle = 0;
    cmd_body.cmd_arg.motor_t.mode_arg.motor_arg.motor_move_arg.move_state = 0;
	dev_rs485_cmd_read(&dev_lamp->rs485, retry_times, retry_tick, true, &read_buff, &cmd_body, DEV_RS485_VA_ARG_END);
	dev_rs485_cmd_read(&dev_lamp->rs485, retry_times, retry_tick, true, &read_buff, &cmd_body, DEV_RS485_VA_ARG_END);
    if(0 == dev_rs485_cmd_read(&dev_lamp->rs485, retry_times, retry_tick, true, &read_buff, &cmd_body, DEV_RS485_VA_ARG_END))
    {
        *angle = p_read_body->cmd_arg.motor_t.mode_arg.motor_arg.motor_move_arg.motor_angle;
        ack = true;
    }
    else
    {
        ack = false;
    }
    return ack;
}

bool rs485_set_fan_mode(dev_lamp_t* dev_lamp, bool ack_need, uint32_t retry_times, uint32_t retry_tick, uint8_t mode ,uint16_t speed )
{
    bool ack = false;
    rs485_cmd_body_t cmd_body;
    cmd_body.header.cmd_type = RS485_Cmd_Fan;
    cmd_body.cmd_arg.fan.mode = mode;
    cmd_body.cmd_arg.fan.speed = speed;
    ack = 0 == dev_rs485_cmd_write(&dev_lamp->rs485, retry_times, retry_tick, true, NULL, &cmd_body, DEV_RS485_VA_ARG_END) ? true : false;
    return ack;
}


bool rs485_set_led_frq(dev_lamp_t* dev_lamp, bool ack_need, uint32_t retry_times, uint32_t retry_tick, int16_t frq_value)
{
    bool ack = false;
    rs485_cmd_body_t cmd_body;
    cmd_body.header.cmd_type = RS485_Cmd_Dim_Frq;
    cmd_body.cmd_arg.dimming_frq.adjust_val = frq_value;
    ack = 0 == dev_rs485_cmd_write(&dev_lamp->rs485, 5, 100, true, NULL, &cmd_body, DEV_RS485_VA_ARG_END) ? true : false;
    return ack;
}

bool rs485_set_dimming_curve(dev_lamp_t* dev_lamp, bool ack_need, uint32_t retry_times, uint32_t retry_tick, uint8_t curve)
{
    bool ack = false;
    rs485_cmd_body_t cmd_body;
    cmd_body.header.cmd_type = RS485_Cmd_Dimming_Curve;
    cmd_body.cmd_arg.dimming_curve.curve = curve;
    ack = 0 == dev_rs485_cmd_write(&dev_lamp->rs485, 5, 100, true, NULL, &cmd_body, DEV_RS485_VA_ARG_END) ? true : false;
    return ack;
}

bool rs485_set_sleep_mode(dev_lamp_t* dev_lamp, bool ack_need, uint32_t retry_times, uint32_t retry_tick, uint8_t sleep_mode)
{
    bool ack = false;
    rs485_cmd_body_t cmd_body;
    cmd_body.header.cmd_type = RS485_Cmd_Switch;
    cmd_body.cmd_arg.power_switch.state = sleep_mode;
    ack = 0 == dev_rs485_cmd_write(&dev_lamp->rs485, 5, 100, true, NULL, &cmd_body, DEV_RS485_VA_ARG_END) ? true : false;
    return ack;
}

bool rs485_set_Industrial_fresnel_angle(dev_lamp_t* dev_lamp, bool ack_need, uint32_t retry_times, uint32_t retry_tick)
{
    bool ack = false;
    rs485_cmd_body_t cmd_body;
    cmd_body.header.cmd_type = RS485_Cmd_Motor;
    cmd_body.cmd_arg.motor_t.type = RS485_Fresnel_Motor;
    cmd_body.cmd_arg.motor_t.mode_arg.motor_mode = RS485_Motor_Mode_Move;
    cmd_body.cmd_arg.motor_t.mode_arg.motor_arg.motor_move_arg.motor_angle = 9999;
    ack = 0 == dev_rs485_cmd_write(&dev_lamp->rs485, retry_times, retry_tick, true, NULL, &cmd_body, DEV_RS485_VA_ARG_END) ? true : false;
    return ack;
}

bool rs485_find_Lamp_mode(dev_lamp_t* dev_lamp, bool ack_need, uint32_t retry_times, uint32_t retry_tick, uint16_t Lamp_state)
{
    bool ack = false;
    rs485_cmd_body_t cmd_body;
    cmd_body.header.cmd_type = RS485_Cmd_Sys_FX;
    cmd_body.cmd_arg.sys_fx.type = RS485_FX_I_Am_Here;
    cmd_body.cmd_arg.sys_fx.arg.i_am_here.state = Lamp_state;
    ack = 0 == dev_rs485_cmd_write(&dev_lamp->rs485, 5, 100, true, NULL, &cmd_body, DEV_RS485_VA_ARG_END) ? true : false;
    return ack;
}

bool rs485_set_cfx_name(dev_lamp_t* dev_lamp, bool ack_need, uint32_t retry_tick, uint32_t retry_times, uint8_t type ,uint8_t bank, char* name )
{
    bool ack = false;
    rs485_cmd_body_t cmd_body;
    cmd_body.header.cmd_type = RS485_Cmd_CFX_Name;
    cmd_body.cmd_arg.cfx_name.bank = bank;
    cmd_body.cmd_arg.cfx_name.code_type = 0;
    cmd_body.cmd_arg.cfx_name.effect_type = type;
    memcpy(cmd_body.cmd_arg.cfx_name.name, name,10);
    ack = 0 == dev_rs485_cmd_write(&dev_lamp->rs485, retry_tick, retry_times, true, NULL, &cmd_body, DEV_RS485_VA_ARG_END) ? true : false;
    return ack;
}


bool rs485_get_cfx_bank_rw(dev_lamp_t* dev_lamp, bool ack_need, uint32_t retry_tick, uint32_t retry_times, uint8_t type)
{
    rs485_cmd_body_t cmd_body;
    memset(read_buff, 0, sizeof(read_buff));
    rs485_cmd_body_t* p_read_body = (rs485_cmd_body_t*)read_buff;
    cmd_body.header.cmd_type = RS485_Cmd_CFX_Bank_RW;
    if(0 == dev_rs485_cmd_read(&dev_lamp->rs485, retry_tick, retry_times, true, read_buff, &cmd_body, DEV_RS485_VA_ARG_END))
    {
        if(type == 0)
            send_ble_cfx_bank_rw_pack(type, p_read_body->cmd_arg.cfx_bank_rw.picker);
        else if(type == 1)
            send_ble_cfx_bank_rw_pack(type, p_read_body->cmd_arg.cfx_bank_rw.touch_bar);
        else if(type == 2)
            send_ble_cfx_bank_rw_pack(type, p_read_body->cmd_arg.cfx_bank_rw.music);
        return 1;
    }
    return 0;
}

bool rs485_get_mount_access(dev_lamp_t* dev_lamp, bool ack_need, uint32_t retry_tick, uint32_t retry_times, struct sys_info_accessories *arg)
{
    rs485_cmd_body_t cmd_body;
    memset(read_buff, 0, sizeof(read_buff));
    rs485_cmd_body_t* p_read_body = (rs485_cmd_body_t*)read_buff;
    
    cmd_body.header.cmd_type = RS485_Cmd_Accessories;
    cmd_body.cmd_arg.access_t.type = RS485_Access_adapter_ring;
    if(0 == dev_rs485_cmd_read(&dev_lamp->rs485, retry_tick, retry_times, true, read_buff, &cmd_body, DEV_RS485_VA_ARG_END))
    {
        if(p_read_body->cmd_arg.access_t.access_state.state == 1)
            arg->adapter_ring = 1;
        else 
            arg->adapter_ring = 0;
        return 1;
    }
    return 0;
}

bool rs485_get_access(dev_lamp_t* dev_lamp, bool ack_need, uint32_t retry_tick, uint32_t retry_times, struct sys_info_accessories *arg)
{
    rs485_cmd_body_t cmd_body;
    rs485_cmd_body_t cmd_body1;
    rs485_cmd_body_t cmd_body2;
    rs485_cmd_body_t cmd_body3;
    rs485_cmd_body_t cmd_body4;
	rs485_cmd_body_t cmd_body5;
    uint16_t body_size = 0;
    memset(read_buff, 0, sizeof(read_buff));
    rs485_cmd_body_t* p_read_body = (rs485_cmd_body_t*)read_buff;
    
    cmd_body.header.cmd_type = RS485_Cmd_Accessories;
    cmd_body.cmd_arg.access_t.type = RS485_Access_Bracket;
    cmd_body1.header.cmd_type = RS485_Cmd_Accessories;
    cmd_body1.cmd_arg.access_t.type = RS485_Access_Fresnel;
    cmd_body2.header.cmd_type = RS485_Cmd_Accessories;
    cmd_body2.cmd_arg.access_t.type = RS485_Access_20_Reflector;
    cmd_body3.header.cmd_type = RS485_Cmd_Accessories;
    cmd_body3.cmd_arg.access_t.type = RS485_Access_30_Reflector;
    cmd_body4.header.cmd_type = RS485_Cmd_Accessories;
    cmd_body4.cmd_arg.access_t.type = RS485_Access_50_Reflector;
	cmd_body5.header.cmd_type = RS485_Cmd_Accessories;
    cmd_body5.cmd_arg.access_t.type = RS485_Access_adapter_ring;
    if(0 == dev_rs485_cmd_read(&dev_lamp->rs485, retry_tick, retry_times, true, read_buff, &cmd_body, &cmd_body1, &cmd_body2, &cmd_body3, &cmd_body4, &cmd_body5, DEV_RS485_VA_ARG_END))
    {
        if(p_read_body->cmd_arg.access_t.access_state.state == 1)
            arg->yoke_state = 1;
        else
            arg->yoke_state = 0;
        body_size += sizeof(rs485_cmd_header_t) + p_read_body->header.arg_size;
        p_read_body = (rs485_cmd_body_t*)((uint32_t)read_buff + body_size);
        if(p_read_body->cmd_arg.access_t.access_state.state == 1)
            arg->fresnel_state = 1;
        else 
            arg->fresnel_state = 0;
        body_size += sizeof(rs485_cmd_header_t) + p_read_body->header.arg_size;
        p_read_body = (rs485_cmd_body_t*)((uint32_t)read_buff + body_size);
        if(p_read_body->cmd_arg.access_t.access_state.state == 1)
            arg->optical_20_access_state = 1;
        else 
            arg->optical_20_access_state = 0;
        body_size += sizeof(rs485_cmd_header_t) + p_read_body->header.arg_size;
        p_read_body = (rs485_cmd_body_t*)((uint32_t)read_buff + body_size);
        if(p_read_body->cmd_arg.access_t.access_state.state == 1)
            arg->optical_30_access_state = 1;
        else 
            arg->optical_30_access_state = 0;
        body_size += sizeof(rs485_cmd_header_t) + p_read_body->header.arg_size;
        p_read_body = (rs485_cmd_body_t*)((uint32_t)read_buff + body_size);
        if(p_read_body->cmd_arg.access_t.access_state.state == 1)
            arg->optical_50_access_state = 1;
        else 
            arg->optical_50_access_state = 0;
//		body_size += sizeof(rs485_cmd_header_t) + p_read_body->header.arg_size;
//        p_read_body = (rs485_cmd_body_t*)((uint32_t)read_buff + body_size);
//        if(p_read_body->cmd_arg.access_t.access_state.state == 1)
//            arg->protective_cover_state = 1;
//        else 
//            arg->protective_cover_state = 0;
		body_size += sizeof(rs485_cmd_header_t) + p_read_body->header.arg_size;
        p_read_body = (rs485_cmd_body_t*)((uint32_t)read_buff + body_size);
		if(p_read_body->cmd_arg.access_t.access_state.state == 1)
            arg->adapter_ring = 1;
        else 
            arg->adapter_ring = 0;
		return 1;
    }
//	cmd_body6.header.cmd_type = RS485_Cmd_Accessories;
//    cmd_body6.cmd_arg.access_t.type = 7;
//	if(0 == dev_rs485_cmd_read(&dev_lamp->rs485, retry_tick, retry_times, true, read_buff, &cmd_body, &cmd_body6, DEV_RS485_VA_ARG_END))
//    {
//        if(p_read_body->cmd_arg.access_t.access_state.state == 1)
//            arg->adapter_ring = 1;
//        else 
//            arg->adapter_ring = 0;
//        return 1;
//    }
    return 0;
}



bool rs485_light_control(dev_lamp_t* dev_lamp, bool ack_need, uint32_t retry_times, uint32_t retry_tick, void* p_cmd_body)
{
    bool ack = false;
    ack = 0 == dev_rs485_cmd_write(&dev_lamp->rs485, 5, retry_tick, true, NULL, p_cmd_body, DEV_RS485_VA_ARG_END) ? true : false;
    return ack;
}



void rs485_power_on_trigger_cfx(void)
{
    rs485_cmd_body_t cmd_body_1 = {0};
	
	if(ui_get_trigger_cfx_state())
	{
		ui_set_trigger_cfx_state(0);
		ui_set_cfx_read_bank_state(0);
		
		struct sys_info_cfx_name cfx_name_info;
		
		for(uint8_t i = 0; i < 10; i++)
		{
			rs485_get_cfx_name(&dev_lamp, true, 3, 1000, 0, i, (char *)&cfx_name_info.cfx_name[0].name[i]);
			rs485_get_cfx_name(&dev_lamp, true, 3, 1000, 2, i, (char *)&cfx_name_info.cfx_name[2].name[i]);
		}
		data_center_write_sys_info(SYS_INFO_CFX_NAME, &cfx_name_info);
		data_center_read_light_data(LIGHT_MODE_LOCAL_CFX, &g_tUIAllData.local_fx);
		data_center_read_light_data(LIGHT_MODE_SIDUS_CFX, &g_tUIAllData.cfx_ctrl);
		cmd_body_1.header.cmd_type = RS485_Cmd_CFX_Ctrl;
		cmd_body_1.cmd_arg.cfx_ctrl.effect_type = g_tUIAllData.cfx_ctrl.type;
		cmd_body_1.cmd_arg.cfx_ctrl.bank = g_tUIAllData.cfx_ctrl.bank;
		cmd_body_1.cmd_arg.cfx_ctrl.intensity = g_tUIAllData.cfx_ctrl.lightness;
		cmd_body_1.cmd_arg.cfx_ctrl.spd = 1;
		cmd_body_1.cmd_arg.cfx_ctrl.chaos = 0;
		cmd_body_1.cmd_arg.cfx_ctrl.sequence = 0;
		cmd_body_1.cmd_arg.cfx_ctrl.loop = 1;
		cmd_body_1.cmd_arg.cfx_ctrl.state = 1;
		
		g_tUIAllData.local_fx.type = g_tUIAllData.cfx_ctrl.type;
		g_tUIAllData.local_fx.bank[g_tUIAllData.cfx_ctrl.type] = g_tUIAllData.cfx_ctrl.bank;
		
		dev_lamp.Connected = rs485_light_control(&dev_lamp, true, 5, 2000, &cmd_body_1);
		if(dev_lamp.Connected == 1)
			ui_set_cfx_read_bank_state(1);
	}
}

uint8_t get_lamp_connet_state(void)
{
    return dev_lamp.Connected;
}




bool rs485_custom_control(dev_lamp_t* dev_lamp, bool ack_need, uint32_t retry_times, uint32_t retry_tick, void* p_cmd_body, uint8_t * ack )
{
    int res = false;
    rs485_ack_body_t ack_body = {0};
    res =  dev_rs485_cmd_write(&dev_lamp->rs485, 1, 10000, true, &ack_body, p_cmd_body, DEV_RS485_VA_ARG_END);

    if(res == 0 || res == -2)
    {
        *ack  = ack_body.ack_arg.ack;
        return 1;
    }
    else
    {
        return 0;
    }

}


bool rs485_fx_bank_get( dev_lamp_t* dev_lamp, rs485_cfx_bank_rw_t  * cfx_bank_data  )
{

    bool ack = false;
    
    memset(read_buff, 0, sizeof(read_buff));
    rs485_cmd_body_t* p_read_body = (rs485_cmd_body_t*)read_buff;
    rs485_cmd_body_t cmd_body_cfx = {.header.cmd_type = RS485_Cmd_CFX_Bank_RW, };

    if(0 == dev_rs485_cmd_read(&dev_lamp->rs485, 5, 100, true, p_read_body, &cmd_body_cfx, DEV_RS485_VA_ARG_END))
    {

        cfx_bank_data->music = p_read_body->cmd_arg.cfx_bank_rw.music;
        cfx_bank_data->picker = p_read_body->cmd_arg.cfx_bank_rw.picker;
        cfx_bank_data->touch_bar = p_read_body->cmd_arg.cfx_bank_rw.touch_bar;
        ack = true;

    }

    return ack;


}

//void SidusProFile_CFX_BankInfo_Set(uint8_t Type, uint16_t Bank_Info)
//{
//  // if(Type < 3 && Bank < 10)
//  // {
//		CFX_Info[Type].Bank_Info = Bank_Info;
//	//}
//}
bool SidusProFile_Set_Info_Name(uint8_t type, uint8_t bank, const char* name)
{
    uint8_t ret = false;

    if(type < 3 && bank < 10 && '\0' != name[0])
    {
        memcpy(rs485_cfx_info[type].name[bank], name, 9);
        ret = true;
    }
    else
    {
        memcpy(rs485_cfx_info[type].name[bank],  "NO CFX", 9);
    }

    return ret;
}

bool rs485_fx_name_get(dev_lamp_t* dev_lamp)
{
    int i, j;
    bool ack = false;
    
    memset(read_buff, 0, sizeof(read_buff));
    rs485_cmd_body_t* p_read_body = (rs485_cmd_body_t*)read_buff;

    for(  j = 0 ; j < 3 ; j++ )
    {
        for(i = 0 ; i < 10 ; i++)
        {

            rs485_cmd_body_t  cmd_body_cfx1 =  {.header.cmd_type = RS485_Cmd_CFX_Name, .cmd_arg.cfx_name.effect_type = j, .cmd_arg.cfx_name.bank = i};

            if(0 == dev_rs485_cmd_read(&dev_lamp->rs485, 5, 100, true, p_read_body, &cmd_body_cfx1, DEV_RS485_VA_ARG_END))
            {
                SidusProFile_Set_Info_Name(j, i, p_read_body->cmd_arg.cfx_name.name);
                ack = true;
            }
            else
            {
                ack = false;
                return ack;
            }

        }
    }

    return ack;

}


bool rs485_fx_name_single_get(dev_lamp_t* dev_lamp, uint8_t type, uint8_t  bank )
{

    bool ack = false;
    //uint32_t retry_cnt = 0;
    memset(read_buff, 0, sizeof(read_buff));
    rs485_cmd_body_t* p_read_body = (rs485_cmd_body_t*)read_buff;

    rs485_cmd_body_t  cmd_body_cfx1 =  {.header.cmd_type = RS485_Cmd_CFX_Name, .cmd_arg.cfx_name.effect_type = type, .cmd_arg.cfx_name.bank = bank};

    if(0 == dev_rs485_cmd_read(&dev_lamp->rs485, 5, 100, true, p_read_body, &cmd_body_cfx1, DEV_RS485_VA_ARG_END))
    {
        SidusProFile_Set_Info_Name(type, bank, p_read_body->cmd_arg.cfx_name.name);
        ack = true;
    }
    else
    {
        ack = false;
        return ack;
    }

    return ack;

}


bool rs485_fx_name_set(dev_lamp_t* dev_lamp, bool ack_need, uint32_t retry_times, uint32_t retry_tick, void* p_cmd_body)
{
    bool ack = false;

    ack = 0 == dev_rs485_cmd_write(&dev_lamp->rs485, 5, 100, true, NULL, p_cmd_body, DEV_RS485_VA_ARG_END) ? true : false;
    return ack;

}


bool rs485_board_msg_read(dev_lamp_t* dev_lamp, bool ack_need, uint32_t retry_times, uint32_t retry_tick, struct sys_info_lamp* body)
{
    bool ack = false;
	uint32_t body_size = 0;
	
    memset(read_buff, 0, sizeof(read_buff));
    rs485_cmd_body_t* p_read_body = (rs485_cmd_body_t*)read_buff;
    rs485_cmd_body_t cmd_body_fan = {.header.cmd_type = RS485_Cmd_Fan, };
    rs485_cmd_body_t cmd_body_temp = {.header.cmd_type = RS485_Cmd_Temperature_Msg, .cmd_arg.temperature_msg.type = 1,   };
	rs485_cmd_body_t cmd_body_temp2 = {.header.cmd_type = RS485_Cmd_Temperature_Msg, .cmd_arg.temperature_msg.type = 4,   };
	rs485_cmd_body_t cmd_body_temp3 = {.header.cmd_type = RS485_Cmd_Temperature_Msg, .cmd_arg.temperature_msg.type = 5,   }; //电动菲涅尔温度
	rs485_cmd_body_t cmd_body_temp4 = {.header.cmd_type = RS485_Cmd_Temperature_Msg, .cmd_arg.temperature_msg.type = 6,   }; //电动菲涅尔2温度
	
    if(0 == dev_rs485_cmd_read(&dev_lamp->rs485, 5, 100, true, p_read_body, &cmd_body_fan, &cmd_body_temp, &cmd_body_temp2, &cmd_body_temp3, DEV_RS485_VA_ARG_END))
    {
        body->fan_speed = (uint16_t)p_read_body->cmd_arg.fan.speed;
		body_size += sizeof(rs485_cmd_header_t) + p_read_body->header.arg_size;
        p_read_body = (rs485_cmd_body_t*)((uint32_t)read_buff + body_size);
        body->cob_temp = (uint16_t) p_read_body->cmd_arg.temperature_msg.value;
		body_size += sizeof(rs485_cmd_header_t) + p_read_body->header.arg_size;
        p_read_body = (rs485_cmd_body_t*)((uint32_t)read_buff + body_size);
		body->led_run_time = (uint16_t) p_read_body->cmd_arg.temperature_msg.value;
		body_size += sizeof(rs485_cmd_header_t) + p_read_body->header.arg_size;
		p_read_body = (rs485_cmd_body_t*)((uint32_t)read_buff + body_size);
		body->fre_temp = (uint16_t) p_read_body->cmd_arg.temperature_msg.value;
		body_size += sizeof(rs485_cmd_header_t) + p_read_body->header.arg_size;
		p_read_body = (rs485_cmd_body_t*)((uint32_t)read_buff + body_size);
		body->fre2_temp = (uint16_t) p_read_body->cmd_arg.temperature_msg.value;
        ack = true;
    }

    return ack;
}

bool rs485_read_err_msg(dev_lamp_t* dev_lamp, uint32_t* err_msg)
{
    bool ack = false;
    memset(read_buff, 0, sizeof(read_buff));
    rs485_cmd_body_t* p_read_body = (rs485_cmd_body_t*)read_buff;
    rs485_cmd_body_t cmd_body_msg = {.header.cmd_type = RS485_Cmd_Err_Msg, };

    if(0 == dev_rs485_cmd_read(&dev_lamp->rs485, 5, 100, true, p_read_body, &cmd_body_msg, DEV_RS485_VA_ARG_END))
    {
        *err_msg = *(uint32_t*)&p_read_body->cmd_arg.err_msg.err_state1;
        ack = true;
    }
    return ack;
}

uint32_t rs485_get_device_version2(dev_lamp_t* dev_lamp, bool ack_need, uint32_t retry_times, uint32_t retry_tick, struct sys_info_lamp* body)
{
    bool ack = false;
	memset(read_buff, 0, sizeof(read_buff));
	uint32_t body_size = 0;
	rs485_cmd_body_t* p_read_body = (rs485_cmd_body_t*)read_buff;
    rs485_cmd_body_t cmd_body = {.header.cmd_type = RS485_Cmd_Version2, .cmd_arg.version2_t.product_num = 0};  //电动支架
	rs485_cmd_body_t cmd_body2 = {.header.cmd_type = RS485_Cmd_Version2, .cmd_arg.version2_t.product_num = 1};//电动菲涅尔
	 
    if(0 == dev_rs485_cmd_read(&dev_lamp->rs485, 5, 100, true, p_read_body, &cmd_body, &cmd_body2, DEV_RS485_VA_ARG_END))
    {
        body->motorized_yoke_soft_ver[0] = p_read_body->cmd_arg.version2_t.soft_ver_major * 0x10 + p_read_body->cmd_arg.version2_t.soft_ver_minor;
		body->motorized_yoke_hw_ver[0] = p_read_body->cmd_arg.version2_t.hard_ver_major * 0x10 + p_read_body->cmd_arg.version2_t.hard_ver_minor;
		body_size += sizeof(rs485_cmd_header_t) + p_read_body->header.arg_size;
		p_read_body = (rs485_cmd_body_t*)((uint32_t)read_buff + body_size);
		body->motorized_fresnel_soft_ver[0] = p_read_body->cmd_arg.version2_t.soft_ver_major * 0x10 + p_read_body->cmd_arg.version2_t.soft_ver_minor;
		body->motorized_fresnel_hw_ver[0] = p_read_body->cmd_arg.version2_t.hard_ver_major * 0x10 + p_read_body->cmd_arg.version2_t.hard_ver_minor;
    }
    
    return ack;
}

uint32_t rs485_get_device_version(dev_lamp_t* dev_lamp, bool ack_need, uint32_t retry_times, uint32_t retry_tick, struct sys_info_lamp* body)
{
    bool ack = false;
    rs485_cmd_body_t cmd_body = {.header.cmd_type = RS485_Cmd_Version, };
    rs485_cmd_body_t read_body;

    if(0 == dev_rs485_cmd_read(&dev_lamp->rs485, 5, 100, true, (rs485_cmd_body_t *)&read_body, &cmd_body, DEV_RS485_VA_ARG_END))
    {
        body->soft_ver[0] = read_body.cmd_arg.version.soft_ver_major * 0x10 + read_body.cmd_arg.version.soft_ver_minor;
        body->hard_ver[0] = read_body.cmd_arg.version.hard_ver_major * 0x10 + read_body.cmd_arg.version.hard_ver_minor;
        body->product_num =  read_body.cmd_arg.version.product_num ;
		ack = true;
    }
    
    return ack;
}


bool rs485_heartbeat(dev_lamp_t* dev_lamp, uint32_t retry_times, uint32_t retry_tick)
{
    // static uint16_t sequence = 0;
    // bool ack = false;

    return true;
}


bool rs485_self_adaption(dev_lamp_t* dev_lamp, bool ack_need, uint32_t retry_times, uint32_t retry_tick, void* self_body)
{


    bool ack = false;

    rs485_cmd_body_t cmd_body = {.header.cmd_type = RS485_Cmd_Self_Adaption, };

    if(0 == dev_rs485_cmd_read(&dev_lamp->rs485, 5, 100, true, self_body, &cmd_body, DEV_RS485_VA_ARG_END))
    {
        ack = true;
    }

    return ack;
}

bool rs485_send_upgrade_data(dev_lamp_t* dev_lamp, void* file_tx, uint8_t* ack, uint32_t retry_ticks)
{
    bool res = false;
	
	rs485_cmd_body_t body = {.header.cmd_type = RS485_Cmd_FileTransfer};
    memset(&body.cmd_arg.file_transfer, 0, sizeof(body.cmd_arg.file_transfer));
	memcpy(&body.cmd_arg.file_transfer, file_tx, sizeof(rs485_file_transfer_t));
    res = 0 == dev_rs485_cmd_write(&dev_lamp->rs485, 2, retry_ticks, true, NULL, &body, DEV_RS485_VA_ARG_END) ? false : true;

    return res;
}   


uint8_t api_send_siduspro_fx_file(dev_lamp_t* dev_lamp, uint8_t *arg, rs485_file_tx_step_enum code, uint16_t time)
{
    rs485_cmd_body_t body = {.header.cmd_type = RS485_Cmd_FileTransfer};
    uint8_t res = 0;
    
    switch (code)
    {
        case RS485_File_TX_Step_Start:
            body.cmd_arg.file_transfer.step = RS485_File_TX_Step_Start;
            body.cmd_arg.file_transfer.file_type = RS485_File_TX_CFX;
            body.cmd_arg.file_transfer.step_body.start_arg.cfx_start.effect_type = arg[4];
            body.cmd_arg.file_transfer.step_body.start_arg.cfx_start.bank = arg[5];
        break;
        case RS485_File_TX_Step_Size:
            body.cmd_arg.file_transfer.step = RS485_File_TX_Step_Size;
            body.cmd_arg.file_transfer.file_type = RS485_File_TX_CFX;
            body.cmd_arg.file_transfer.step_body.data_arg.block_cnt = 1;
            body.cmd_arg.file_transfer.step_body.size_arg.file_size = *((uint32_t*)arg);
        break;
        case RS485_File_TX_Step_Data:
            body.cmd_arg.file_transfer.step = RS485_File_TX_Step_Data;
            body.cmd_arg.file_transfer.file_type = RS485_File_TX_CFX;
            body.cmd_arg.file_transfer.step_body.data_arg.block_size = arg[2] - 4;
            body.cmd_arg.file_transfer.step_body.data_arg.block_cnt = (arg[6] << 24) | (arg[5] << 16) | (arg[4] << 8) | arg[3];
            memcpy(&body.cmd_arg.file_transfer.step_body.data_arg.block_buf, &arg[7], 128);
        break;
        case RS485_File_TX_Step_CRC:
            body.cmd_arg.file_transfer.step = RS485_File_TX_Step_CRC;
            body.cmd_arg.file_transfer.file_type = RS485_File_TX_CFX;
            body.cmd_arg.file_transfer.step_body.crc_arg.crc32 = *((uint32_t*)arg);
        break;
        default:break;
    }
    res = 0 == dev_rs485_cmd_write(&dev_lamp->rs485, 5, time, true, NULL, &body, DEV_RS485_VA_ARG_END) ? true : false;
    
    return res;
}

void api_cfx_file_handle(dev_lamp_t* dev_lamp, uint8_t *buffer)
{
    static uint32_t RcvBinCrc = 0;
    static uint32_t Rcv_SeqNum_Count = 1;
    uint32_t BinSize = 0;
    uint32_t length = 0;
    static uint32_t CRC_RCV = 0;
    static uint8_t updata_res = 0;
    
    updata_res = 0;
    switch(buffer[0])
    {
        case '1':       //1:xx xx xx xx????
            if(buffer[4] > CFX_Type_Music || buffer[5] > CFX_Bank_10)
            {
                set_ble_cfx_file_state(0);
                SidusPro_FileTransfer_Send_Status(CFX_FILE_RX_STATUS_ILLEGAL);
            }
            else
            {
                RcvBinCrc = 0;   //CRC?????
                Rcv_SeqNum_Count = 1;    //??????1
                updata_res = api_send_siduspro_fx_file(dev_lamp, buffer, RS485_File_TX_Step_Start, 2000);
                if(updata_res == 1)
                {
                    SidusPro_FileTransfer_Send_Status(CFX_FILE_RX_STATUS_OK);
                }
                else
                {
                    set_ble_cfx_file_state(0);
                    SidusPro_FileTransfer_Send_Status(CFX_FILE_RX_STATUS_ILLEGAL);
                }
            }
            break;
        case '2':       //1:xxxxxxxx  bin????
            BinSize = (buffer[5] << 24) | (buffer[4] << 16) | (buffer[3] << 8) | buffer[2];
            if(BinSize > MAX_RCV_BIN_SIZE)
            {
                set_ble_cfx_file_state(0);
                SidusPro_FileTransfer_Send_Status(CFX_FILE_RX_STATUS_INVALID_BINSIZE);
            }
            else
            {
                updata_res = api_send_siduspro_fx_file(dev_lamp, (uint8_t *)&BinSize, RS485_File_TX_Step_Size, 5000);
                if(updata_res == 1)
                {
                    SidusPro_FileTransfer_Send_Status(CFX_FILE_RX_STATUS_OK);
                }
                else
                {
                    set_ble_cfx_file_state(0);
                    SidusPro_FileTransfer_Send_Status(CFX_FILE_RX_STATUS_ILLEGAL);
                }
            }
            break;
        case '3':       //3:xx xxxxxxxx GATT???
            length = buffer[2] - 4;
            api_send_siduspro_fx_file(dev_lamp, buffer, RS485_File_TX_Step_Data, 2000);
            Rcv_SeqNum_Count++;
            RcvBinCrc = CRC32_Calculate(RcvBinCrc, &buffer[7], length);//??crc
            break;
        case '4':       //+4:xxxxxxxx??CRC??
            CRC_RCV = (buffer[5] << 24) | (buffer[4] << 16) | (buffer[3] << 8) | buffer[2];
            if(RcvBinCrc == CRC_RCV)
            {
                updata_res = api_send_siduspro_fx_file(dev_lamp, (uint8_t *)&RcvBinCrc, RS485_File_TX_Step_CRC, 2000);
                if(updata_res == 1)
                {
                    SidusPro_FileTransfer_Send_Status(CFX_FILE_RX_STATUS_OK);
                }
                else
                {
                    set_ble_cfx_file_state(0);
                    SidusPro_FileTransfer_Send_Status(CFX_FILE_RX_STATUS_ILLEGAL);
                }
            }
            else
            {
                set_ble_cfx_file_state(0);
                SidusPro_FileTransfer_Send_Status(CFX_FILE_RX_STATUS_VERIFY_FAIL);
            }
            set_ble_cfx_file_state(0);
            break;
        default:break;
    }
}

int light_update_open(const char *file_name)
{
    uint8_t ack = 0;
    bool res = false;
    if(NULL == file_name) return res;
    rs485_file_transfer_t file_tx ;
    memset(&file_tx, 0, sizeof(rs485_file_transfer_t));
    file_tx.file_type = RS485_File_TX_Firmware;
	file_tx.step = RS485_File_TX_Step_Start;
    if(strncmp(file_name, "lamp_firmware.bin", strlen("lamp_firmware.bin")) == 0)
        file_tx.step_body.start_arg.firmware_start.type =  1;
    else if(strncmp(file_name, "ele_frnsel_firmware.bin", strlen("ele_frnsel_firmware.bin")) == 0)
        file_tx.step_body.start_arg.firmware_start.type =  2;
    else if(strncmp(file_name, "ele_bracket_firmware.bin", strlen("ele_bracket_firmware.bin")) == 0)
        file_tx.step_body.start_arg.firmware_start.type =  3;
    res = rs485_send_upgrade_data(&dev_lamp, &file_tx, &ack, 10000);
    return res;
}

int light_update_erase(uint32_t addr, uint32_t size)
{
    bool res = false;
    uint8_t ack = 0;
    rs485_file_transfer_t file_tx ;
    memset(&file_tx, 0, sizeof(rs485_file_transfer_t));
    file_tx.file_type = RS485_File_TX_Firmware;
    file_tx.step = RS485_File_TX_Step_Size;
    file_tx.step_body.size_arg.file_size = size;
    res = rs485_send_upgrade_data(&dev_lamp, &file_tx, &ack, 20000);
    //dev_lamp.update_crc = 0;
    return res;
}

int light_update_write(uint32_t addr, const uint8_t *data, uint32_t size)
{
    bool res = false;
    uint8_t ack = 0;
    rs485_file_transfer_t file_tx ;
    memset(&file_tx, 0, sizeof(rs485_file_transfer_t));
    file_tx.file_type = RS485_File_TX_Firmware;
    file_tx.step = RS485_File_TX_Step_Data;
    file_tx.step_body.data_arg.block_size = size;
    memcpy(&file_tx.step_body.data_arg.block_buf, data, sizeof(file_tx.step_body.data_arg.block_buf));
    file_tx.step_body.data_arg.block_cnt =  addr;
    dev_lamp.update_crc += CRC32_Calculate(dev_lamp.update_crc, file_tx.step_body.data_arg.block_buf, size);
    res = rs485_send_upgrade_data(&dev_lamp, &file_tx, &ack,2000);
    return res;
}

int light_update_close(void *arg)
{
    bool res = false;
    uint8_t ack = 0;
    rs485_file_transfer_t file_tx ;
    memset(&file_tx, 0, sizeof(rs485_file_transfer_t));
    file_tx.file_type = RS485_File_TX_Firmware;
    file_tx.step = RS485_File_TX_Step_CRC;
    file_tx.step_body.crc_arg.crc32 = (uint32_t)arg;
    res = rs485_send_upgrade_data(&dev_lamp, &file_tx, &ack,1000);
    return res;
}

bool rs485_send_cfx_data(dev_lamp_t* dev_lamp, void* CfxBody, uint8_t* ack,uint32_t retry_ticks)
{
    bool res = false;
	rs485_cmd_body_t body = {.header.cmd_type = RS485_Cmd_FileTransfer};
	memcpy(&body.cmd_arg.file_transfer, CfxBody, sizeof(rs485_file_transfer_t));
    res = 0 == dev_rs485_cmd_write(&dev_lamp->rs485, 1, retry_ticks, true, NULL, &body, DEV_RS485_VA_ARG_END) ? true : false;
    return res;
}   

uint16_t aaaaaaaaa[7];
uint8_t rs485_self_adaption_read(dev_lamp_t* dev_lamp)
{
	static uint16_t s_error_num = 0;
	memset(read_buff, 0, sizeof(read_buff));
	static struct sys_info_lamp   s_info_read_body = {0};
	data_center_read_sys_info(SYS_INFO_LAMP, &s_info_read_body);
	rs485_cmd_body_t* p_read_body = (rs485_cmd_body_t*)read_buff;
	uint16_t body_size = 0;
	rs485_cmd_body_t cmd_body_1 = {.header.cmd_type = RS485_Cmd_Self_Adaption, .cmd_arg.self_adaption.led_color_type = HAL_PWM_SELF_ADAPTION_R,};
	rs485_cmd_body_t cmd_body_2 = {.header.cmd_type = RS485_Cmd_Self_Adaption,.cmd_arg.self_adaption.led_color_type = HAL_PWM_SELF_ADAPTION_G,};
	rs485_cmd_body_t cmd_body_3 = {.header.cmd_type = RS485_Cmd_Self_Adaption, .cmd_arg.self_adaption.led_color_type = HAL_PWM_SELF_ADAPTION_B,};
	rs485_cmd_body_t cmd_body_4 = {.header.cmd_type = RS485_Cmd_Self_Adaption,.cmd_arg.self_adaption.led_color_type = HAL_PWM_SELF_ADAPTION_WW1,};
	rs485_cmd_body_t cmd_body_5 = {.header.cmd_type = RS485_Cmd_Self_Adaption, .cmd_arg.self_adaption.led_color_type = HAL_PWM_SELF_ADAPTION_CW1,};
	rs485_cmd_body_t cmd_body_6 = {.header.cmd_type = RS485_Cmd_Self_Adaption,.cmd_arg.self_adaption.led_color_type = HAL_PWM_SELF_ADAPTION_WW2,};
	rs485_cmd_body_t cmd_body_7 = {.header.cmd_type = RS485_Cmd_Self_Adaption,.cmd_arg.self_adaption.led_color_type = HAL_PWM_SELF_ADAPTION_CW2,};

	if(0 == dev_rs485_cmd_read(&dev_lamp->rs485, 5, 100, true, read_buff, &cmd_body_1, &cmd_body_2, \
                             &cmd_body_3, &cmd_body_4,&cmd_body_5, &cmd_body_6,&cmd_body_7, DEV_RS485_VA_ARG_END))
    {
		for(uint8_t i = HAL_PWM_SELF_ADAPTION_R ; i< (HAL_PWM_SELF_ADAPTION_CW2 + 1) ; i++)
		{
			if(s_info_read_body.hard_ver[0] == 0x10)
				dev_adapt_pwm_regulate(&pwm_adapt_20k,i-1, p_read_body->cmd_arg.self_adaption.led_voltage,p_read_body->cmd_arg.self_adaption.led_pwm );
			else
				dev_adapt_pwm_regulate(&pwm_adapt_80k,i-1, p_read_body->cmd_arg.self_adaption.led_voltage,p_read_body->cmd_arg.self_adaption.led_pwm );
			aaaaaaaaa[i-1] = p_read_body->cmd_arg.self_adaption.led_voltage;
			if(p_read_body->header.arg_size != sizeof(rs485_self_adaption_t))
			{
				s_error_num ++ ;
			}
			else{
				s_error_num = 0;
			}
			if(s_error_num > 20)
			{
				return 2;
		    }
			body_size += sizeof(rs485_cmd_header_t) + p_read_body->header.arg_size;
			p_read_body = (rs485_cmd_body_t*)((uint32_t)read_buff + body_size);
		}
		if(s_info_read_body.hard_ver[0] == 0x10)
			dev_adapt_fade_restart(&pwm_adapt_20k);
		else
			dev_adapt_fade_restart(&pwm_adapt_80k);
		return 1;
    }
	else
    {
		return 0;

    }

}