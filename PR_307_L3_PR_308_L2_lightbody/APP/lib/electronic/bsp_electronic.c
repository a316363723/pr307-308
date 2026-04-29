#include "bsp_electronic.h"
#include "bsp_usart.h"
#include "Rs485_Proto_Slave_Analysis.h"
#include "Rs485_Proto_Slave_Core.h"
#include "data_acc.h"
#include "string.h"
#include "stdio.h"
#include "define.h"
#include "upgrade_list.h"
#include "sm4_enc_user.h"
#include "stdlib.h"
#ifdef ENCODE
 sm4_data_t *encode_data;
 sm4_data_t *encode_data_sent_pitch_angel;
 sm4_data_t *de_encode_data;
#endif
Electronic_RW_Buf Electronic_Buf;
Electronic_Queue Bracket_Queue;
Electronic_Queue Bayont_Queue;
static uint8_t data_pin[512] = {0};
extern float pitch_last;
extern float roll_last;
extern float frensel_last;
extern rs485_motor_t Pitch_Roll_Angel[5];

void Electronic_Package_Fuction(uint8_t *Electronic_Data,uint8_t RW_Cmd,rs485_motor_t motor,uint8_t onoff_cmd)
{
	static uint16_t pev_offset = 0;
	static uint8_t R_State = 0;
	static uint16_t Electronic_Serial = 0;
	static uint16_t Electronic_Serial_Frensel = 0;
	if( R_State == 0 )
	{
		((rs485_proto_header_t *)(Electronic_Data))->start = RS485_START_CODE;
		((rs485_proto_header_t *)(Electronic_Data))->header_size = sizeof(rs485_proto_header_t);
		((rs485_proto_header_t *)(Electronic_Data))->proto_ver_major = RS485_PROTO_VER_MAJOR;
		((rs485_proto_header_t *)(Electronic_Data))->proto_ver_minor = RS485_PROTO_VER_MINOR;
		((rs485_proto_header_t *)(Electronic_Data))->proto_ver_revision = RS485_PROTO_VER_REVISION;
		if( motor.type == RS485_Pitch_Motor || motor.type == RS485_Rotale_Motor )
			((rs485_proto_header_t *)(Electronic_Data))->serial_num = Electronic_Serial++;
		else if( motor.type == RS485_Fresnel_Motor )
			((rs485_proto_header_t *)(Electronic_Data))->serial_num = Electronic_Serial_Frensel++;
		((rs485_proto_header_t *)(Electronic_Data))->ack_en = RW_Cmd;
		((rs485_proto_header_t *)(Electronic_Data))->msg_size = sizeof(rs485_proto_header_t);
		pev_offset += sizeof(rs485_proto_header_t);
		R_State = 1;
	}
	switch(	RW_Cmd )
	{
		case RS485_COMMAND_READ:{	
			((rs485_cmd_header_t *)(Electronic_Data + pev_offset))->rw = RW_Cmd;
			((rs485_cmd_header_t *)(Electronic_Data + pev_offset))->cmd_type = RS485_Cmd_Motor;
			((rs485_cmd_header_t *)(Electronic_Data + pev_offset))->arg_size = sizeof(rs485_motor_t);
			pev_offset += sizeof(rs485_cmd_header_t);
			((rs485_motor_t *)(Electronic_Data + pev_offset))->type = motor.type;
				if(motor.type> 3)
						{
						__nop();
						}
			((rs485_proto_header_t *)(Electronic_Data))->msg_size += sizeof(rs485_cmd_header_t);	
			((rs485_proto_header_t *)(Electronic_Data))->msg_size += sizeof(rs485_motor_t);	
			
			switch(motor.mode_arg.motor_mode)
			{			
				case RS485_Motor_Mode_Move:{
					((rs485_motor_t *)(Electronic_Data+pev_offset))->mode_arg.motor_mode = RS485_Motor_Mode_Move;
					((rs485_motor_t *)(Electronic_Data+pev_offset))->mode_arg.motor_arg.motor_move_arg.motor_angle = motor.mode_arg.motor_arg.motor_move_arg.motor_angle;
					((rs485_motor_t *)(Electronic_Data+pev_offset))->mode_arg.motor_arg.motor_move_arg.move_state = motor.mode_arg.motor_arg.motor_move_arg.move_state;
					((rs485_motor_t *)(Electronic_Data+pev_offset))->mode_arg.motor_arg.motor_move_arg.motor_a1 = motor.mode_arg.motor_arg.motor_move_arg.motor_a1;
					((rs485_motor_t *)(Electronic_Data+pev_offset))->mode_arg.motor_arg.motor_move_arg.motor_acce = motor.mode_arg.motor_arg.motor_move_arg.motor_acce;
					((rs485_motor_t *)(Electronic_Data+pev_offset))->mode_arg.motor_arg.motor_move_arg.motor_d1 = motor.mode_arg.motor_arg.motor_move_arg.motor_d1;
					((rs485_motor_t *)(Electronic_Data+pev_offset))->mode_arg.motor_arg.motor_move_arg.motor_dece = motor.mode_arg.motor_arg.motor_move_arg.motor_dece;
					((rs485_motor_t *)(Electronic_Data+pev_offset))->mode_arg.motor_arg.motor_move_arg.motor_star_speed = motor.mode_arg.motor_arg.motor_move_arg.motor_star_speed;
					((rs485_motor_t *)(Electronic_Data+pev_offset))->mode_arg.motor_arg.motor_move_arg.motor_stop_speed = motor.mode_arg.motor_arg.motor_move_arg.motor_stop_speed;
					((rs485_motor_t *)(Electronic_Data+pev_offset))->mode_arg.motor_arg.motor_move_arg.motor_targe_speed = motor.mode_arg.motor_arg.motor_move_arg.motor_targe_speed;
					((rs485_motor_t *)(Electronic_Data+pev_offset))->mode_arg.motor_arg.motor_move_arg.motor_v1 = motor.mode_arg.motor_arg.motor_move_arg.motor_v1;
				}
				break;
				case RS485_Motor_Mode_Crc:{
					((rs485_motor_t *)(Electronic_Data+pev_offset))->mode_arg.motor_mode = RS485_Motor_Mode_Crc;
					((rs485_motor_t *)(Electronic_Data+pev_offset))->mode_arg.motor_arg.motor_crc_arg.crc_state = motor.mode_arg.motor_arg.motor_crc_arg.crc_state;
					((rs485_motor_t *)(Electronic_Data+pev_offset))->mode_arg.motor_arg.motor_crc_arg.crc_value = motor.mode_arg.motor_arg.motor_crc_arg.crc_value;
					((rs485_motor_t *)(Electronic_Data+pev_offset))->mode_arg.motor_arg.motor_crc_arg.sttall_max_value = motor.mode_arg.motor_arg.motor_crc_arg.sttall_max_value;
					((rs485_motor_t *)(Electronic_Data+pev_offset))->mode_arg.motor_arg.motor_crc_arg.sttall_min_value = motor.mode_arg.motor_arg.motor_crc_arg.sttall_min_value;
				}
				break;
				case RS485_Motor_Mode_Reset:{
					((rs485_motor_t *)(Electronic_Data+pev_offset))->mode_arg.motor_mode = RS485_Motor_Mode_Reset;
					((rs485_motor_t *)(Electronic_Data+pev_offset))->mode_arg.motor_arg.motor_reset_arg.reset_angle = motor.mode_arg.motor_arg.motor_reset_arg.reset_angle;
					((rs485_motor_t *)(Electronic_Data+pev_offset))->mode_arg.motor_arg.motor_reset_arg.reset_state = motor.mode_arg.motor_arg.motor_reset_arg.reset_state;

				}
				break;
				case RS485_Motor_Mode_State:{
					((rs485_motor_t *)(Electronic_Data+pev_offset))->mode_arg.motor_mode = RS485_Motor_Mode_State;
					((rs485_motor_t *)(Electronic_Data+pev_offset))->mode_arg.motor_arg.motor_state_arg.stall_state = motor.mode_arg.motor_arg.motor_state_arg.stall_state;
				}
				break;
				default:break;
			}
			pev_offset += sizeof(rs485_motor_t);
			if( motor.mode_arg.motor_mode != RS485_Motor_Mode_Move && motor.type == RS485_Fresnel_Motor )
			{
			  ((rs485_proto_header_t *)(Electronic_Data))->check_sum = Rs485_CheckSum(((rs485_proto_header_t*)Electronic_Data)->msg_size - RS485_HEADER_NO_CHECK_LEN, Electronic_Data + RS485_HEADER_NO_CHECK_LEN);
				
				#ifdef ENCODE
				//sm4_data_t *encode_data;
				encode_data = user_plaintext_encryption(Electronic_Buf.W_Bayont_Buf.Electronic_Data,((rs485_proto_header_t *)(Electronic_Buf.W_Bayont_Buf.Electronic_Data))->msg_size);
				if( encode_data != NULL && encode_data->data_len <= 500 )
				{
//					uint8_t data_pin[512] = {0};
					memcpy(data_pin,Electronic_Buf.W_Bayont_Buf.Electronic_Data,11);
					memcpy(data_pin+11,encode_data->data,encode_data->data_len);
					push_electronic_data(&Bayont_Queue,data_pin,encode_data->data_len+11);
//					hal_rs485_3_usart_send(data_pin,encode_data->data_len+11);
				}
				#else
				hal_rs485_3_usart_send(Electronic_Data,((rs485_proto_header_t *)(Electronic_Data))->msg_size);
				#endif
				R_State = 0;
				pev_offset = 0;
			}
			if( motor.mode_arg.motor_mode == RS485_Motor_Mode_Move && motor.type == RS485_Fresnel_Motor )
			{
			  ((rs485_proto_header_t *)(Electronic_Data))->check_sum = Rs485_CheckSum(((rs485_proto_header_t*)Electronic_Data)->msg_size - RS485_HEADER_NO_CHECK_LEN, Electronic_Data + RS485_HEADER_NO_CHECK_LEN);
//				hal_rs485_3_usart_send(Electronic_Data,((rs485_proto_header_t *)(Electronic_Data))->msg_size);
				R_State = 0;
				pev_offset = 0;
			}
			if( motor.mode_arg.motor_mode == RS485_Motor_Mode_Move && motor.type == RS485_Pitch_Motor )
			{
			   ((rs485_proto_header_t *)(Electronic_Data))->check_sum = Rs485_CheckSum(((rs485_proto_header_t*)Electronic_Data)->msg_size - RS485_HEADER_NO_CHECK_LEN, Electronic_Data + RS485_HEADER_NO_CHECK_LEN);
//				hal_rs485_2_usart_send(Electronic_Data,((rs485_proto_header_t *)(Electronic_Data))->msg_size);
				R_State = 0;
				pev_offset = 0;
			}
			if( motor.mode_arg.motor_mode == RS485_Motor_Mode_Move && motor.type == RS485_Rotale_Motor )
			{
			   ((rs485_proto_header_t *)(Electronic_Data))->check_sum = Rs485_CheckSum(((rs485_proto_header_t*)Electronic_Data)->msg_size - RS485_HEADER_NO_CHECK_LEN, Electronic_Data + RS485_HEADER_NO_CHECK_LEN);
//				hal_rs485_2_usart_send(Electronic_Data,((rs485_proto_header_t *)(Electronic_Data))->msg_size);
				R_State = 0;
				pev_offset = 0;
			}
			if( motor.mode_arg.motor_mode != RS485_Motor_Mode_Move && motor.type == RS485_Rotale_Motor )
			{
			  ((rs485_proto_header_t *)(Electronic_Data))->check_sum = Rs485_CheckSum(((rs485_proto_header_t*)Electronic_Data)->msg_size - RS485_HEADER_NO_CHECK_LEN, Electronic_Data + RS485_HEADER_NO_CHECK_LEN);
				
				#ifdef ENCODE
				//sm4_data_t *encode_data;
				encode_data = user_plaintext_encryption(Electronic_Buf.W_Bracket_Buf.Electronic_Data,((rs485_proto_header_t *)(Electronic_Buf.W_Bracket_Buf.Electronic_Data))->msg_size);
				if( encode_data != NULL && encode_data->data_len <= 500 )
				{
//					uint8_t data_pin[512] = {0};
					memcpy(data_pin,Electronic_Buf.W_Bracket_Buf.Electronic_Data,11);
					memcpy(data_pin+11,encode_data->data,encode_data->data_len);
					push_electronic_data(&Bracket_Queue,data_pin,encode_data->data_len+11);
//					hal_rs485_2_usart_send(data_pin,encode_data->data_len+11);
				}
				#else
				hal_rs485_2_usart_send(Electronic_Data,((rs485_proto_header_t *)(Electronic_Data))->msg_size);
				#endif
				R_State = 0;
				pev_offset = 0;
			}
		}break;
		case RS485_COMMAND_WRITE:{
			if(onoff_cmd == 0)
			{
				((rs485_cmd_header_t *)(Electronic_Data + pev_offset))->rw = RW_Cmd;
				((rs485_cmd_header_t *)(Electronic_Data + pev_offset))->cmd_type = RS485_Cmd_Switch;
				((rs485_cmd_header_t *)(Electronic_Data + pev_offset))->arg_size = sizeof(rs485_switch_t);
				pev_offset += sizeof(rs485_cmd_header_t);
				((rs485_switch_t *)(Electronic_Data + pev_offset))->state = g_rs485_data.power_switch.state;
				
				((rs485_proto_header_t *)(Electronic_Data))->msg_size += sizeof(rs485_cmd_header_t);	
				((rs485_proto_header_t *)(Electronic_Data))->msg_size += sizeof(rs485_switch_t);
				pev_offset += sizeof(rs485_motor_t);				
			    ((rs485_proto_header_t *)(Electronic_Data))->check_sum = Rs485_CheckSum(((rs485_proto_header_t*)Electronic_Data)->msg_size - RS485_HEADER_NO_CHECK_LEN, Electronic_Data + RS485_HEADER_NO_CHECK_LEN);
				R_State = 0;
				pev_offset = 0;
				if( motor.type == RS485_Pitch_Motor || motor.type == RS485_Rotale_Motor )
					Electronic_Buf.Bracket_Send_State = Electronic_Recive;
				else if( motor.type == RS485_Fresnel_Motor )
					Electronic_Buf.Bayont_Send_State = Electronic_Recive;
			}
			else
			{
				((rs485_cmd_header_t *)(Electronic_Data + pev_offset))->rw = RW_Cmd;
				((rs485_cmd_header_t *)(Electronic_Data + pev_offset))->cmd_type = RS485_Cmd_Motor;
				((rs485_cmd_header_t *)(Electronic_Data + pev_offset))->arg_size = sizeof(rs485_motor_t);
				pev_offset += sizeof(rs485_cmd_header_t);
				((rs485_motor_t *)(Electronic_Data + pev_offset))->type = motor.type;
				
				((rs485_proto_header_t *)(Electronic_Data))->msg_size += sizeof(rs485_cmd_header_t);	
				((rs485_proto_header_t *)(Electronic_Data))->msg_size += sizeof(rs485_motor_t);	
				
				switch(motor.mode_arg.motor_mode)
				{			
					case RS485_Motor_Mode_Move:{
						((rs485_motor_t *)(Electronic_Data+pev_offset))->mode_arg.motor_mode = RS485_Motor_Mode_Move;
						((rs485_motor_t *)(Electronic_Data+pev_offset))->mode_arg.motor_arg.motor_move_arg.motor_angle = motor.mode_arg.motor_arg.motor_move_arg.motor_angle;
						((rs485_motor_t *)(Electronic_Data+pev_offset))->mode_arg.motor_arg.motor_move_arg.move_state = motor.mode_arg.motor_arg.motor_move_arg.move_state;
						((rs485_motor_t *)(Electronic_Data+pev_offset))->mode_arg.motor_arg.motor_move_arg.motor_a1 = motor.mode_arg.motor_arg.motor_move_arg.motor_a1;
						((rs485_motor_t *)(Electronic_Data+pev_offset))->mode_arg.motor_arg.motor_move_arg.motor_acce = motor.mode_arg.motor_arg.motor_move_arg.motor_acce;
						((rs485_motor_t *)(Electronic_Data+pev_offset))->mode_arg.motor_arg.motor_move_arg.motor_d1 = motor.mode_arg.motor_arg.motor_move_arg.motor_d1;
						((rs485_motor_t *)(Electronic_Data+pev_offset))->mode_arg.motor_arg.motor_move_arg.motor_dece = motor.mode_arg.motor_arg.motor_move_arg.motor_dece;
						((rs485_motor_t *)(Electronic_Data+pev_offset))->mode_arg.motor_arg.motor_move_arg.motor_star_speed = motor.mode_arg.motor_arg.motor_move_arg.motor_star_speed;
						((rs485_motor_t *)(Electronic_Data+pev_offset))->mode_arg.motor_arg.motor_move_arg.motor_stop_speed = motor.mode_arg.motor_arg.motor_move_arg.motor_stop_speed;
						((rs485_motor_t *)(Electronic_Data+pev_offset))->mode_arg.motor_arg.motor_move_arg.motor_targe_speed = motor.mode_arg.motor_arg.motor_move_arg.motor_targe_speed;
						((rs485_motor_t *)(Electronic_Data+pev_offset))->mode_arg.motor_arg.motor_move_arg.motor_v1 = motor.mode_arg.motor_arg.motor_move_arg.motor_v1;
					}
					break;
					case RS485_Motor_Mode_Crc:{
						((rs485_motor_t *)(Electronic_Data+pev_offset))->mode_arg.motor_mode = RS485_Motor_Mode_Crc;
						((rs485_motor_t *)(Electronic_Data+pev_offset))->mode_arg.motor_arg.motor_crc_arg.crc_state = motor.mode_arg.motor_arg.motor_crc_arg.crc_state;
						((rs485_motor_t *)(Electronic_Data+pev_offset))->mode_arg.motor_arg.motor_crc_arg.crc_value = motor.mode_arg.motor_arg.motor_crc_arg.crc_value;
						((rs485_motor_t *)(Electronic_Data+pev_offset))->mode_arg.motor_arg.motor_crc_arg.sttall_max_value = motor.mode_arg.motor_arg.motor_crc_arg.sttall_max_value;
						((rs485_motor_t *)(Electronic_Data+pev_offset))->mode_arg.motor_arg.motor_crc_arg.sttall_min_value = motor.mode_arg.motor_arg.motor_crc_arg.sttall_min_value;
					}
					break;
					case RS485_Motor_Mode_Reset:{
						((rs485_motor_t *)(Electronic_Data+pev_offset))->mode_arg.motor_mode = RS485_Motor_Mode_Reset;
						((rs485_motor_t *)(Electronic_Data+pev_offset))->mode_arg.motor_arg.motor_reset_arg.reset_angle = motor.mode_arg.motor_arg.motor_reset_arg.reset_angle;
						((rs485_motor_t *)(Electronic_Data+pev_offset))->mode_arg.motor_arg.motor_reset_arg.reset_state = motor.mode_arg.motor_arg.motor_reset_arg.reset_state;
					}
					break;
					case RS485_Motor_Mode_State:{
						((rs485_motor_t *)(Electronic_Data+pev_offset))->mode_arg.motor_mode = RS485_Motor_Mode_State;
						((rs485_motor_t *)(Electronic_Data+pev_offset))->mode_arg.motor_arg.motor_state_arg.stall_state = motor.mode_arg.motor_arg.motor_state_arg.stall_state;
					}
					break;
					default:break;
				}
				pev_offset += sizeof(rs485_motor_t);
				if( motor.type == RS485_Fresnel_Motor )//the Frensnel motor is the type that dont like bracket
				{
				   ((rs485_proto_header_t *)(Electronic_Data))->check_sum = Rs485_CheckSum(((rs485_proto_header_t*)Electronic_Data)->msg_size - RS485_HEADER_NO_CHECK_LEN, Electronic_Data + RS485_HEADER_NO_CHECK_LEN);
					R_State = 0;
					pev_offset = 0;
					Electronic_Buf.Bayont_Send_State = Electronic_Recive;
				}
				if( motor.mode_arg.motor_mode != RS485_Motor_Mode_Move && (motor.type == RS485_Rotale_Motor || motor.type == RS485_Pitch_Motor))
				{
				   ((rs485_proto_header_t *)(Electronic_Data))->check_sum = Rs485_CheckSum(((rs485_proto_header_t*)Electronic_Data)->msg_size - RS485_HEADER_NO_CHECK_LEN, Electronic_Data + RS485_HEADER_NO_CHECK_LEN);
					R_State = 0;
					pev_offset = 0;
					Electronic_Buf.Bracket_Send_State = Electronic_Recive;
				}
				#if NULL
				if( motor.mode_arg.motor_mode == RS485_Motor_Mode_Reset && motor.type == RS485_Pitch_Motor )
				{
					((rs485_proto_header_t *)(Electronic_Data))->check_sum = Rs485_CheckSum(((rs485_proto_header_t*)Electronic_Data)->msg_size - RS485_HEADER_NO_CHECK_LEN, Electronic_Data + RS485_HEADER_NO_CHECK_LEN);
					R_State = 0;
					pev_offset = 0;
					Electronic_Buf.Bracket_Send_State = Electronic_Recive;
				}
				#endif
				if( motor.mode_arg.motor_mode == RS485_Motor_Mode_Move || motor.mode_arg.motor_mode == RS485_Motor_Mode_Reset )
				{
				   ((rs485_proto_header_t *)(Electronic_Data))->check_sum = Rs485_CheckSum(((rs485_proto_header_t*)Electronic_Data)->msg_size - RS485_HEADER_NO_CHECK_LEN, Electronic_Data + RS485_HEADER_NO_CHECK_LEN);
					R_State = 0;
					pev_offset = 0;
					if(  motor.type == RS485_Rotale_Motor )
						Electronic_Buf.Bracket_Send_State = Electronic_Send;
					if( motor.type == RS485_Pitch_Motor )
						Electronic_Buf.Bracket_Send_State = Electronic_Send;
					if(  motor.type == RS485_Fresnel_Motor )
						Electronic_Buf.Bayont_Send_State = Electronic_Send;
				}	
			}
		}break;
		default:break;
	}
}


uint8_t Electronic_Anlaysic_Fuction(uint8_t *Electronic_Data,uint16_t *p_recv_offset,local_rs485_motor_t *motor)
{
	rs485_cmd_header_t  data_header = {0};
    rs485_cmd_arg_t*    p_decode = NULL;
    memcpy(&data_header, Electronic_Data + *p_recv_offset, sizeof(rs485_cmd_header_t));
    /*包数据长度大于剩余空间*/
    if(*p_recv_offset + sizeof(rs485_cmd_header_t) + data_header.arg_size > RS485_MESSAGE_MAX_SIZE){
		 p_decode = (rs485_cmd_arg_t*)(Electronic_Data + *p_recv_offset + sizeof(rs485_cmd_header_t));
		*p_recv_offset += sizeof(rs485_cmd_header_t) + data_header.arg_size; 
        return 0;
    }
    /*获取数据*/
    p_decode = (rs485_cmd_arg_t*)(Electronic_Data + *p_recv_offset + sizeof(rs485_cmd_header_t));
		*p_recv_offset += sizeof(rs485_cmd_header_t) + data_header.arg_size; 
	switch(data_header.rw){
        case RS485_COMMAND_READ:{
				switch(data_header.cmd_type){
				case RS485_Cmd_Motor:{
					if(p_decode->motor_t.type > 2 )
					{
						  return 1;
					}
					motor[p_decode->motor_t.type].type = p_decode->motor_t.type;
                    motor[p_decode->motor_t.type].mode_arg.motor_mode = p_decode->motor_t.mode_arg.motor_mode;
					
					switch(p_decode->motor_t.mode_arg.motor_mode)
					{
						case RS485_Motor_Mode_Move:{
							Pitch_Roll_Angel[p_decode->motor_t.type].mode_arg.motor_arg.motor_move_arg = p_decode->motor_t.mode_arg.motor_arg.motor_move_arg;
							if( p_decode->motor_t.type == RS485_Pitch_Motor )
							 {
										pitch_last = Pitch_Roll_Angel[p_decode->motor_t.type].mode_arg.motor_arg.motor_move_arg.motor_angle;
							 }
							 if( p_decode->motor_t.type == RS485_Rotale_Motor )
							 {
									 roll_last = Pitch_Roll_Angel[p_decode->motor_t.type].mode_arg.motor_arg.motor_move_arg.motor_angle;
							 }
							 if( p_decode->motor_t.type == RS485_Fresnel_Motor )
							 {
									 frensel_last = Pitch_Roll_Angel[p_decode->motor_t.type].mode_arg.motor_arg.motor_move_arg.motor_angle;
							 }
						}
						break;
						case RS485_Motor_Mode_Crc:{
							motor[p_decode->motor_t.type].mode_arg.motor_arg.motor_crc_arg = p_decode->motor_t.mode_arg.motor_arg.motor_crc_arg;
						}
						break;
						case RS485_Motor_Mode_Reset:{
							motor[p_decode->motor_t.type].mode_arg.motor_arg.motor_reset_arg = p_decode->motor_t.mode_arg.motor_arg.motor_reset_arg;
						}
						break;
						case RS485_Motor_Mode_State:{
							motor[p_decode->motor_t.type].mode_arg.motor_arg.motor_state_arg = p_decode->motor_t.mode_arg.motor_arg.motor_state_arg;
						}
						break;
						default:break;
					}
                }
				break;
				case RS485_Cmd_Accessories:{
				uint8_t state_bit = 1;
				 switch(p_decode->access_t.type)
				 {					 
					 case RS485_Access_Bracket:{
						 Electronic_Buf.Bracket_Times_connect = 0;
						 g_error_state.electric_support_eh_013 = 0;
						if( p_decode->access_t.access_state.state == 1 )
							Electronic_Buf.Electronic_Connect |= state_bit;
						else
							Electronic_Buf.Electronic_Connect &= ~state_bit;
					 }break;
					 case RS485_Access_Fresnel:{
						 Electronic_Buf.Bayonet_Times_connect = 0;
						 g_error_state.electric_accessories_eh_014 = 0;
						 Electronic_Buf.Bayonet_Communicate_Error = 0;
						 Electronic_Buf.Bayonet_Error_Reset_Cnt= 0;
						if( p_decode->access_t.access_state.state == 1 )
							Electronic_Buf.Electronic_Connect |= (state_bit<<1);
						else
							Electronic_Buf.Electronic_Connect &= (~state_bit<<1);
					 }break;
					  case RS485_Access_20_Reflector:{
							Electronic_Buf.Bayonet_Times_connect = 0;
							g_error_state.electric_accessories_eh_014 = 0;
							Electronic_Buf.Bayonet_Communicate_Error = 0;
							Electronic_Buf.Bayonet_Error_Reset_Cnt= 0;
						if( p_decode->access_t.access_state.state == 1 )
							Electronic_Buf.Electronic_Connect |= (state_bit<<2);
						else
							Electronic_Buf.Electronic_Connect &= (~state_bit<<2);
					 }break;
						case RS485_Access_30_Reflector:{
							Electronic_Buf.Bayonet_Times_connect = 0;
							g_error_state.electric_accessories_eh_014 = 0;
							Electronic_Buf.Bayonet_Communicate_Error = 0;
							Electronic_Buf.Bayonet_Error_Reset_Cnt= 0;
						if( p_decode->access_t.access_state.state == 1 )
							Electronic_Buf.Electronic_Connect |= (state_bit<<3);
						else
							Electronic_Buf.Electronic_Connect &= (~state_bit<<3);
					 }break;
						case RS485_Access_50_Reflector:{
							Electronic_Buf.Bayonet_Times_connect = 0;
							g_error_state.electric_accessories_eh_014 = 0;
							Electronic_Buf.Bayonet_Communicate_Error = 0;
							Electronic_Buf.Bayonet_Error_Reset_Cnt= 0;
						if( p_decode->access_t.access_state.state == 1 )
							Electronic_Buf.Electronic_Connect |= (state_bit<<4);
						else
							Electronic_Buf.Electronic_Connect &= (~state_bit<<4);
					 }break;
//						case RS485_Access_Protect_Cover:{
//							Electronic_Buf.Bayonet_Times_connect = 0;
//						if( p_decode->access_t.access_state.state == 1 )
//							Electronic_Buf.Electronic_Connect |= (state_bit<<5);
//						else
//							Electronic_Buf.Electronic_Connect &= (~state_bit<<5);
//					 }break;
						case RS485_Access_Adapter_Bowl:{
							Electronic_Buf.Bayonet_Times_connect = 0;
							g_error_state.electric_accessories_eh_014 = 0;
							Electronic_Buf.Bayonet_Communicate_Error = 0;
							Electronic_Buf.Bayonet_Error_Reset_Cnt= 0;
						if( p_decode->access_t.access_state.state == 1 )
							Electronic_Buf.Electronic_Connect |= (state_bit<<5);
						else
							Electronic_Buf.Electronic_Connect &= (~state_bit<<5);
					 }break;
					 default:break;
				 }
				}
				break;
				case RS485_Cmd_Temperature_Msg:
				{
					switch(p_decode->temperature_msg.type)
					{
						case 5:
						{
							Electronic_Buf.Fresnel_Temperature1 = p_decode->temperature_msg.value;
						}
						break;
						case 6:
						{
							Electronic_Buf.Fresnel_Temperature2 = p_decode->temperature_msg.value;
						}
						break;
						default:
						break;
					}
				}
				break;
				case RS485_Cmd_Version2:
				{
					switch(p_decode->version2_t.product_type)
					{
						case RS485_Version2_Bracket:
						{
							Electronic_Buf.Bracket_Times_Read_version = 0;
							Electronic_Buf.Bracket_Version.hard_ver_major = p_decode->version2_t.hard_ver_major;
							Electronic_Buf.Bracket_Version.hard_ver_minor = p_decode->version2_t.hard_ver_minor;
							Electronic_Buf.Bracket_Version.hard_ver_revision = p_decode->version2_t.hard_ver_revision;
							Electronic_Buf.Bracket_Version.soft_ver_major = p_decode->version2_t.soft_ver_major;
							Electronic_Buf.Bracket_Version.soft_ver_minor = p_decode->version2_t.soft_ver_minor;
							Electronic_Buf.Bracket_Version.soft_ver_revision = p_decode->version2_t.soft_ver_revision;
						}
						break;
						case RS485_Version2_Fresnel:
						{
							Electronic_Buf.Fresnel_Times_Read_version = 0;
							Electronic_Buf.Fresnel_Version.hard_ver_major = p_decode->version2_t.hard_ver_major;
							Electronic_Buf.Fresnel_Version.hard_ver_minor = p_decode->version2_t.hard_ver_minor;
							Electronic_Buf.Fresnel_Version.hard_ver_revision = p_decode->version2_t.hard_ver_revision;
							Electronic_Buf.Fresnel_Version.soft_ver_major = p_decode->version2_t.soft_ver_major;
							Electronic_Buf.Fresnel_Version.soft_ver_minor = p_decode->version2_t.soft_ver_minor;
							Electronic_Buf.Fresnel_Version.soft_ver_revision = p_decode->version2_t.soft_ver_revision;
						}
						break;
						default:
						break;
					}
				}
				break;
			}
		}
		break;
		case RS485_COMMAND_WRITE:{
        switch(data_header.cmd_type){
				case RS485_Cmd_Motor:{
					if(p_decode->motor_t.type > 2 )
					{
						__NOP();
					}
					switch(p_decode->motor_t.mode_arg.motor_mode)
					{
						
						case RS485_Motor_Mode_Move:{
							if( p_decode->motor_t.type == RS485_Pitch_Motor || p_decode->motor_t.type == RS485_Rotale_Motor)
							{
								Electronic_Buf.Bracket_Send_State = Electronic_Idle;
								if ( ((rs485_proto_header_t *)(Electronic_Buf.W_Bracket_Buf_Sent_Rotale_Angel.Electronic_Data))->serial_num != ((rs485_proto_header_t *)(Electronic_Data))->serial_num
										&& ((rs485_proto_header_t *)(Electronic_Buf.W_Bracket_Buf_Sent_Pitch_Angel.Electronic_Data))->serial_num != ((rs485_proto_header_t *)(Electronic_Data))->serial_num)
									Electronic_Buf.Bracket_Send_State = Electronic_Send;
								else
								{
									Electronic_Buf.Bracket_Times = 0;
									Electronic_Buf.Bracket_Send_State = Electronic_Idle;
								}
							}
							if( p_decode->motor_t.type == RS485_Fresnel_Motor )
							{
								Electronic_Buf.Bayont_Send_State = Electronic_Idle;
								if ( ((rs485_proto_header_t *)(Electronic_Buf.W_Bayont_Buf.Electronic_Data))->serial_num != ((rs485_proto_header_t *)(Electronic_Data))->serial_num )
									Electronic_Buf.Bayont_Send_State = Electronic_Send;
								else
								{
									Electronic_Buf.Bayonet_Times = 0;
									Electronic_Buf.Bayont_Send_State = Electronic_Idle;
								}
							}
						}
						break;
						case RS485_Motor_Mode_Crc:{
							motor[p_decode->motor_t.type].mode_arg.motor_arg.motor_crc_arg = p_decode->motor_t.mode_arg.motor_arg.motor_crc_arg;
						}
						break;
						case RS485_Motor_Mode_Reset:{
							motor[p_decode->motor_t.type].mode_arg.motor_arg.motor_reset_arg = p_decode->motor_t.mode_arg.motor_arg.motor_reset_arg;
							if( p_decode->motor_t.type == RS485_Pitch_Motor || p_decode->motor_t.type == RS485_Rotale_Motor)
									Electronic_Buf.Bracket_Send_State = Electronic_Idle;
						}
						break;
						case RS485_Motor_Mode_State:{
							motor[p_decode->motor_t.type].mode_arg.motor_arg.motor_state_arg = p_decode->motor_t.mode_arg.motor_arg.motor_state_arg;
						}
						break;
						default:break;
					}
                }
				break;
				case RS485_Cmd_Accessories:{
					
				}
				break;
				case RS485_Cmd_Version2:{
					
				}
				break;
				case RS485_Cmd_Temperature_Msg:{
					
				}
				break;
			}
		}
		break;
	}
	return 0;
}


local_rs485_motor_t Pitch_Roll_Slave[3] = {
	[0].type = RS485_Pitch_Motor,
	[1].type = RS485_Rotale_Motor,
	[2].type = RS485_Fresnel_Motor,
};



uint8_t Electronic_Send_Buf[RS485_MESSAGE_MAX_SIZE] = {0};
uint8_t Task_Send_Electronic_Cmd(uint8_t *Electronic_Data,uint16_t Electronic_Size,local_rs485_motor_t *motor)
{
	if( Electronic_Size != 0 )
	{
		uint16_t    ack_data_offset = 0;
		uint8_t     head_check_state = 0;
		Rs485_Recv_Check_TypeDef check_struct = {0};
		/*数据帧合法性校验*/
		head_check_state = Rs485_Recv_Data_Check(Electronic_Data, &check_struct);
		if(0 != head_check_state){
			return 1;
		}	
		/*应答偏移*/
		ack_data_offset += sizeof(rs485_proto_header_t);
		while(check_struct.offset_val < check_struct.frame_length){
			if(	1 == Electronic_Anlaysic_Fuction(Electronic_Data,(uint16_t*)&check_struct.offset_val,motor) )
				break;
		}
	}
	return 0;
}

void Task_AccessBaylet(uint8_t *AccessBaylet,uint8_t RW_Cmd,uint8_t Cmd_type,uint8_t Temperature_Type)
{
	static uint16_t AccessBaylet_Serial = 0;
	((rs485_proto_header_t *)(AccessBaylet))->start = RS485_START_CODE;
	((rs485_proto_header_t *)(AccessBaylet))->header_size = sizeof(rs485_proto_header_t);
	((rs485_proto_header_t *)(AccessBaylet))->proto_ver_major = RS485_PROTO_VER_MAJOR;
	((rs485_proto_header_t *)(AccessBaylet))->proto_ver_minor = RS485_PROTO_VER_MINOR;
	((rs485_proto_header_t *)(AccessBaylet))->proto_ver_revision = RS485_PROTO_VER_REVISION;
	((rs485_proto_header_t *)(AccessBaylet))->serial_num = AccessBaylet_Serial++;
	
	
	((rs485_cmd_header_t *)(AccessBaylet+sizeof(rs485_proto_header_t)))->rw = RW_Cmd;
	switch(	RW_Cmd )
	{
		case RS485_COMMAND_READ:{
			switch(Cmd_type)
			{
				case RS485_Cmd_Accessories:
				{
					((rs485_proto_header_t *)(AccessBaylet))->ack_en = 0x00;
					((rs485_proto_header_t *)(AccessBaylet))->msg_size = sizeof(rs485_proto_header_t)+sizeof(rs485_cmd_header_t)+1;
					((rs485_cmd_header_t *)(AccessBaylet+sizeof(rs485_proto_header_t)))->cmd_type = (rs485_cmd_enum)Cmd_type;
					((rs485_cmd_header_t *)(AccessBaylet+sizeof(rs485_proto_header_t)))->arg_size = 2;//之前的数值是1，是否是错误的？
				}
				break;
				case RS485_Cmd_Version2:
				{
					((rs485_proto_header_t *)(AccessBaylet))->ack_en = 0x00;
					((rs485_proto_header_t *)(AccessBaylet))->msg_size = sizeof(rs485_proto_header_t)+sizeof(rs485_cmd_header_t)+1;
					((rs485_cmd_header_t *)(AccessBaylet+sizeof(rs485_proto_header_t)))->cmd_type = (rs485_cmd_enum)Cmd_type;
					((rs485_cmd_header_t *)(AccessBaylet+sizeof(rs485_proto_header_t)))->arg_size = 10;
				}
				break;
				case RS485_Cmd_Temperature_Msg:
				{
					((rs485_proto_header_t *)(AccessBaylet))->ack_en = 0x00;
					((rs485_proto_header_t *)(AccessBaylet))->msg_size = sizeof(rs485_proto_header_t)+sizeof(rs485_cmd_header_t)+sizeof(rs485_cmd_arg_t)+1;
					((rs485_cmd_header_t *)(AccessBaylet+sizeof(rs485_proto_header_t)))->cmd_type = (rs485_cmd_enum)Cmd_type;
					((rs485_cmd_header_t *)(AccessBaylet+sizeof(rs485_proto_header_t)))->arg_size = 4;
					((rs485_cmd_arg_t*)(AccessBaylet+sizeof(rs485_proto_header_t)+sizeof(rs485_cmd_header_t)))->temperature_msg.type = Temperature_Type;
				}
				break;
			}
		
		}break;
	}
	((rs485_proto_header_t *)(AccessBaylet))->check_sum = Rs485_CheckSum(((rs485_proto_header_t*)AccessBaylet)->msg_size - RS485_HEADER_NO_CHECK_LEN, AccessBaylet + RS485_HEADER_NO_CHECK_LEN);
}


void Electronic_Stall_Crc_Reset_Package(void)
{
	sent_heartbeat();
}

//this code need to ENCODE the information 
void Electronic_angel_Package(void)
{

}
//uint8_t data_pin[512] = {0};
void Electronic_Para_Package(void)
{
	static uint8_t Bracket_Recieve_Count = 0;
	static uint8_t Bayont_Recieve_Count = 0;
	#ifdef ENCODE
	if( Electronic_Buf.RW_Bracket_State == Electronic_Recive )
	{
		de_encode_data = user_chiphertext_decryption(Electronic_Buf.R_Bracket_Buf.Electronic_Data,Electronic_Buf.R_Bracket_Buf.Electronic_Size);
		if(de_encode_data != NULL && de_encode_data->data_len <= 500 )
		{
//			uint8_t data_pin[512] = {0};
			memcpy(data_pin,Electronic_Buf.R_Bracket_Buf.Electronic_Data,11);
			memcpy(data_pin+11,de_encode_data->data,de_encode_data->data_len);
			
			Task_Send_Electronic_Cmd(data_pin,((rs485_proto_header_t *)(Electronic_Buf.R_Bracket_Buf.Electronic_Data))->msg_size,Pitch_Roll_Slave);
		}
		Electronic_Buf.RW_Bracket_State = Electronic_Idle;
	}	
	if( Electronic_Buf.RW_Bayont_State == Electronic_Recive )
	{
		de_encode_data = user_chiphertext_decryption(Electronic_Buf.R_Bayont_Buf.Electronic_Data,Electronic_Buf.R_Bayont_Buf.Electronic_Size);
		if(de_encode_data != NULL && de_encode_data->data_len <= 500 )
		{
			
			memcpy(data_pin,Electronic_Buf.R_Bayont_Buf.Electronic_Data,11);
			memcpy(data_pin+11,de_encode_data->data,de_encode_data->data_len);
		
			Task_Send_Electronic_Cmd(data_pin,((rs485_proto_header_t *)(Electronic_Buf.R_Bayont_Buf.Electronic_Data))->msg_size,Pitch_Roll_Slave);
		}
		Electronic_Buf.RW_Bayont_State = Electronic_Idle;
	}
	#else
	if( Electronic_Buf.RW_Bracket_State == Electronic_Recive )
	{
		Task_Send_Electronic_Cmd(Electronic_Buf.R_Bracket_Buf.Electronic_Data,Electronic_Buf.R_Bracket_Buf.Electronic_Size,Pitch_Roll_Slave);
		Electronic_Buf.RW_Bracket_State = Electronic_Idle;
	}	
	if( Electronic_Buf.RW_Bayont_State == Electronic_Recive)
	{
		Task_Send_Electronic_Cmd(Electronic_Buf.R_Bayont_Buf.Electronic_Data,Electronic_Buf.R_Bayont_Buf.Electronic_Size,Pitch_Roll_Slave);
		Electronic_Buf.RW_Bayont_State = Electronic_Idle;
	}
	#endif
	if(Electronic_Buf.Bayont_Send_State == Electronic_Recive)
	{
		Bayont_Recieve_Count++;
		if(Bayont_Recieve_Count > 15)
		{
			Bayont_Recieve_Count = 0;
			Electronic_Buf.Bayont_Send_State = Electronic_Idle;
		}
	}
	else
	{
		Bayont_Recieve_Count = 0;
	}
	if(Electronic_Buf.Bracket_Send_State == Electronic_Recive)
	{
		Bracket_Recieve_Count++;
		if(Bracket_Recieve_Count > 15)
		{
			Bracket_Recieve_Count = 0;
			Electronic_Buf.Bracket_Send_State = Electronic_Idle;
		}
		
	}
	else
	{
		Bracket_Recieve_Count = 0;
	}
	
}
void init_queue(Electronic_Queue *Queue)
{
	Queue->font = 0;
	Queue->rear = 0;
}
int queue_is_full(Electronic_Queue *Queue)
{
	return ((Queue->rear + 1)%MAXQSIZE == Queue->font);
}
int queue_is_empty(Electronic_Queue *Queue)
{
	return (Queue-> font == Queue->rear);
}

int push_electronic_data(Electronic_Queue *Queue,uint8_t *pData, uint16_t Cnt)
{
	if(queue_is_full(Queue))
	{
		return 0;
	}

	memcpy((Queue->data[Queue->rear].data),pData,Cnt);
	Queue->data[Queue->rear].data_length = Cnt;
	Queue->rear = (Queue->rear +1)%MAXQSIZE;
	return 1;
}

int get_electronic_data(Electronic_Queue *Queue,uint16_t type)
{
	if(queue_is_empty(Queue))
	{
		return 0;
	}
	if(type == 0)
	{
		hal_rs485_2_usart_send(Queue->data[Queue->font].data,Queue->data[Queue->font].data_length);
		Queue->font = (Queue->font +1)%MAXQSIZE;
	}
	if(type == 1)
	{
		hal_rs485_3_usart_send(Queue->data[Queue->font].data,Queue->data[Queue->font].data_length);
		Queue->font = (Queue->font +1)%MAXQSIZE;
	}
	return 1;
}
void Sent_Electronic_Data(void)
{
	if(Electronic_Buf.Bracket_Loading_State != 1)
	{
		get_electronic_data(&Bracket_Queue,0);
	}
	if(Electronic_Buf.Bayonet_Loading_State != 1)
	{
		get_electronic_data(&Bayont_Queue,1);
	}
}

void sent_heartbeat(void)
{
	sm4_data_t *encode_data1;
	if( Electronic_Buf.Bayonet_Status == 1 && (queue_is_empty(&Bayont_Queue)))
	{
		Task_AccessBaylet(Electronic_Buf.W_Bayont_Buf.Electronic_Data,0,RS485_Cmd_Version2,0);
		#ifdef ENCODE
		//sm4_data_t *encode_data1;;
		encode_data1 = user_plaintext_encryption(Electronic_Buf.W_Bayont_Buf.Electronic_Data,((rs485_proto_header_t *)(Electronic_Buf.W_Bayont_Buf.Electronic_Data))->msg_size);
		if( encode_data1 != NULL && encode_data1->data_len <= 500 )
		#endif
		{
			#ifdef ENCODE
			memcpy(data_pin,Electronic_Buf.W_Bayont_Buf.Electronic_Data,11);
			memcpy(data_pin+11,encode_data1->data,encode_data1->data_len);
			push_electronic_data(&Bayont_Queue,data_pin,encode_data1->data_len+11);
			#else
			hal_rs485_3_usart_send(Electronic_Buf.W_Bayont_Buf.Electronic_Data,((rs485_proto_header_t *)(Electronic_Buf.W_Bayont_Buf.Electronic_Data))->msg_size);
			#endif
		}
	}
	if( Electronic_Buf.Bracket_Status == 1 && (queue_is_empty(&Bracket_Queue)))
	{
		Task_AccessBaylet(Electronic_Buf.W_Bracket_Buf.Electronic_Data,0,RS485_Cmd_Version2,0);
		#ifdef ENCODE
		encode_data1 = user_plaintext_encryption(Electronic_Buf.W_Bracket_Buf.Electronic_Data,((rs485_proto_header_t *)(Electronic_Buf.W_Bracket_Buf.Electronic_Data))->msg_size);
		if( encode_data1 != NULL && encode_data1->data_len <= 500 )
		#endif
		{
			#ifdef ENCODE
			memcpy(data_pin,Electronic_Buf.W_Bracket_Buf.Electronic_Data,11);
			memcpy(data_pin+11,encode_data1->data,encode_data1->data_len);
			push_electronic_data(&Bracket_Queue,data_pin,encode_data1->data_len+11);
			#else
			hal_rs485_2_usart_send(Electronic_Buf.W_Bracket_Buf.Electronic_Data,((rs485_proto_header_t *)(Electronic_Buf.W_Bracket_Buf.Electronic_Data))->msg_size);
			#endif
		}
	}
}

extern rs485_motor_t Pitch_Roll[3];
void less_than_48V_electronic_off(void)
{
	sm4_data_t *encode_data1;
	Electronic_Package_Fuction(Electronic_Buf.W_Bayont_Buf.Electronic_Data,1,Pitch_Roll[RS485_Fresnel_Motor],0);
	encode_data1 = user_plaintext_encryption(Electronic_Buf.W_Bayont_Buf.Electronic_Data,((rs485_proto_header_t *)(Electronic_Buf.W_Bayont_Buf.Electronic_Data))->msg_size);
	if( encode_data1 != NULL && encode_data1->data_len <= 500 )
	{
		memcpy(data_pin,Electronic_Buf.W_Bayont_Buf.Electronic_Data,11);
		memcpy(data_pin+11,encode_data1->data,encode_data1->data_len);
		push_electronic_data(&Bayont_Queue,data_pin,encode_data1->data_len+11);
	}
	hal_rs485_2_usart_send(data_pin,encode_data1->data_len+11);
	Electronic_Package_Fuction(Electronic_Buf.W_Bracket_Buf.Electronic_Data,1,Pitch_Roll[RS485_Rotale_Motor],0);
	encode_data1 = user_plaintext_encryption(Electronic_Buf.W_Bracket_Buf.Electronic_Data,((rs485_proto_header_t *)(Electronic_Buf.W_Bracket_Buf.Electronic_Data))->msg_size);
	if( encode_data1 != NULL && encode_data1->data_len <= 500 )
	{
		memcpy(data_pin,Electronic_Buf.W_Bracket_Buf.Electronic_Data,11);
		memcpy(data_pin+11,encode_data1->data,encode_data1->data_len);
		push_electronic_data(&Bracket_Queue,data_pin,encode_data1->data_len+11);
	}
	hal_rs485_3_usart_send(data_pin,encode_data1->data_len+11);
}
