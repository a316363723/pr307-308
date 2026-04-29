/****************************************************************************************
**  Filename :  111.c
**  Abstract :  。
**  By       :  何建国
**  Date     :  
**  Changelog:1.First Create
*****************************************************************************************/

/*****************************************************************************************
									Includes
*****************************************************************************************/
#include "Rs485_Proto_Slave_Analysis.h"
#include "Rs485_Proto_Slave_Core.h"
#include "data_acc.h"
#include "string.h"
#include "define.h"
#include "SidusProFile.h"
#include "stdio.h"
#include "app_led.h"
#include "bsp_electronic.h"
#include "sm4_enc_user.h"
#include "string.h"
#include "bsp_usart.h"
#include "bsp_power.h"
rs485_motor_t Pitch_Roll[3] = {
	[0].type = RS485_Pitch_Motor,
	[1].type = RS485_Rotale_Motor,
	[2].type = RS485_Fresnel_Motor,
};

rs485_motor_t Pitch_Roll_Angel[5] = {
	[0].type = RS485_Pitch_Motor,
	[1].type = RS485_Rotale_Motor,
	[2].type = RS485_Fresnel_Motor,
	[3].type = RS485_Gyroscope_Roll,
	[4].type = RS485_Gyroscope_Pitch,
};

float pitch_last = 0;
float roll_last = 0;
float frensel_last = 0;

extern local_rs485_motor_t Pitch_Roll_Slave[3];

#define NICKER_BROAD
/*****************************************************************************************
							  Variables definitions
*****************************************************************************************/
Rs485_Special_Ack_TypeDef g_rs485_ack_wait = {
    RS485_Cmd_Version,
    0,
    RS485_Ack_Ok,
    0,
    NULL,
};
#ifdef ENCODE
sm4_data_t *encode_data1;
sm4_data_t *de_encode_data1;
#endif
/*****************************************************************************************
							  Functions definitions
*****************************************************************************************/
static uint8_t Rs485_Slave_AnalysisAndPackage(uint8_t* p_data, uint8_t* p_ack_data, uint16_t* p_recv_offset, uint16_t* p_ack_offset, rs485_proto_header_t* p_msg);
void Rs485_Slave_Ack_Package1(rs485_cmd_header_t* p_header, rs485_ack_arg_t* p_ack_body, uint8_t* p_ack_data, uint16_t* p_ack_offset);
static uint16_t sidus_fx_power_cut(uint16_t lightness, float illumination_rate);
/*****************************************************************************************
* Function Name: Rs485_Slave_Data_Check
* Description  : RS485从机接收数据校验
* Arguments    : 接收数据首地址
* Return Value : 0 - OK
                 1 - frame check error
                 2 - 
                 3 - 
                 4 - 
******************************************************************************************/
uint8_t Rs485_Slave_Data_Check(uint8_t* p_recv_data, uint8_t* p_ack_data)
{
    uint16_t    ack_data_offset = 0;
    uint8_t     package_state = 0;
    uint8_t     head_check_state = 0;
    Rs485_Header_Packet_TypeDef header_msg = {0};
    
    rs485_proto_header_t msg_struct = {0};
    Rs485_Recv_Check_TypeDef check_struct = {0};
    /*数据帧合法性校验*/
    head_check_state = Rs485_Recv_Data_Check(p_recv_data, &check_struct);
    if(0 != head_check_state){
        /****note start*****/
        
        /****note end*****/
        return 1;
    }
    memcpy(&msg_struct, p_recv_data, check_struct.min_msg_length);
    
    /*应答偏移*/
    ack_data_offset += sizeof(rs485_proto_header_t);
    /*解析接收数据和打包应答数据*/
    while(check_struct.offset_val < check_struct.frame_length){
        
        package_state = Rs485_Slave_AnalysisAndPackage(p_recv_data, p_ack_data, (uint16_t*)&check_struct.offset_val, &ack_data_offset, &msg_struct);
        if(0 != package_state){
            /****note start*****/
            
            /****note end*****/
            break;
        }
    }
    /*header打包*/
    header_msg.ack_en = RS485_ACK_ENABLE;
    header_msg.frame_length = ack_data_offset;
    header_msg.frame_sel = msg_struct.serial_num;
    Rs485_Send_Header_Package(p_ack_data, &header_msg);
    return 0;
}
/*****************************************************************************************
* Function Name: Rs485_Slave_AnalysisAndPackage
* Description  : RS485从机解析数据和打包应答数据
* Arguments    : 接收数据指针 - 接收数据存放数组的首地址
                 应答数据指针 - 打包的应答数据存放数组的首地址
                 接收数据偏移 - 指示当前数据相对于首地址的位置
                 应答数据偏移 - 指示应答数据的偏移位置
                 信息包数据地址 - 信息包数据地址
* Return Value : 0 - OK
                 1 - 需要解析的数据长度超过规定长度
                 2 -
******************************************************************************************/
static uint8_t set_sys_fx2_light_para(fx_mode_para_t* p_obj, const rs485_fx_2_mode_arg_t* p_source);
static uint8_t data_pin[512] = {0};
static uint8_t Rs485_Slave_AnalysisAndPackage(uint8_t* p_data, uint8_t* p_ack_data, uint16_t* p_recv_offset, uint16_t* p_ack_offset, rs485_proto_header_t* p_msg)
{
    rs485_cmd_header_t  data_header = {0};
    rs485_ack_arg_t     ack_body = {RS485_Ack_Ok};
    uint8_t             ret_val = 0;
    rs485_cmd_arg_t*    p_decode = NULL;
    memcpy(&data_header, p_data + *p_recv_offset, sizeof(rs485_cmd_header_t));
    /*包数据长度大于剩余空间*/
    if(*p_recv_offset + sizeof(rs485_cmd_header_t) + data_header.arg_size > RS485_MESSAGE_MAX_SIZE){
        return 1;
    }
    /*获取数据*/
    p_decode = (rs485_cmd_arg_t*)(p_data + *p_recv_offset + sizeof(rs485_cmd_header_t));
    /*完成本单元的接收偏移值更新*/ 
    *p_recv_offset += sizeof(rs485_cmd_header_t) + data_header.arg_size; 
//		printf("data_header.cmd_type [0x%x]\r\n", data_header.cmd_type);delay_ms(10);
    switch(data_header.rw){
        case RS485_COMMAND_READ:{
            switch(data_header.cmd_type){
                case RS485_Cmd_Version:{
                    data_header.arg_size = sizeof(rs485_version_t);
                    memcpy(p_ack_data + *p_ack_offset, &data_header, sizeof(rs485_cmd_header_t));
                    *p_ack_offset += sizeof(rs485_cmd_header_t);
                    #if 1
                    ((rs485_version_t*)(p_ack_data + *p_ack_offset))->product_num = PRODUCT_TYPE_CODE;
                    ((rs485_version_t*)(p_ack_data + *p_ack_offset))->hard_ver_major = 0X00;
                    ((rs485_version_t*)(p_ack_data + *p_ack_offset))->hard_ver_minor = LAMP_HARDWARE_VER;
                    ((rs485_version_t*)(p_ack_data + *p_ack_offset))->hard_ver_revision = 0X00;
                    ((rs485_version_t*)(p_ack_data + *p_ack_offset))->soft_ver_major = 0X00;
                    ((rs485_version_t*)(p_ack_data + *p_ack_offset))->soft_ver_minor = LAMP_SOFTWART_VER;//*/led_base_data.ver;
                    ((rs485_version_t*)(p_ack_data + *p_ack_offset))->soft_ver_revision = 0X00;
                    #endif
                    *p_ack_offset += sizeof(rs485_version_t);
                }
                break;
                case RS485_Cmd_HSI:{
                    ack_body.ack = RS485_Ack_Err_Command;
                    Rs485_Slave_Ack_Package1(&data_header, &ack_body, p_ack_data, p_ack_offset);
                }
                break;
                case RS485_Cmd_CCT:{
					ack_body.ack = RS485_Ack_Err_Command;
                    Rs485_Slave_Ack_Package1(&data_header, &ack_body, p_ack_data, p_ack_offset);
                }
                break;
                case RS485_Cmd_GEL:{
					ack_body.ack = RS485_Ack_Err_Command;
                    Rs485_Slave_Ack_Package1(&data_header, &ack_body, p_ack_data, p_ack_offset);
                }
                break;
                case RS485_Cmd_RGB:{
                    ack_body.ack = RS485_Ack_Err_Command;
                    Rs485_Slave_Ack_Package1(&data_header, &ack_body, p_ack_data, p_ack_offset);
                }
                break;
                case RS485_Cmd_XY_Coordinate:{
                    ack_body.ack = RS485_Ack_Err_Command;
                    Rs485_Slave_Ack_Package1(&data_header, &ack_body, p_ack_data, p_ack_offset);
                }
                break;
                case RS485_Cmd_Dim_Frq:{
                    
                }
                break;
                case RS485_Cmd_Sys_FX:{
                    switch(p_decode->sys_fx.type){
                        case RS485_FX_ClubLights:{
                            ack_body.ack = RS485_Ack_Err_Command;
                            Rs485_Slave_Ack_Package1(&data_header, &ack_body, p_ack_data, p_ack_offset);
                        }
                        break;
                        case RS485_FX_Paparazzi:{
                            ack_body.ack = RS485_Ack_Err_Command;
                            Rs485_Slave_Ack_Package1(&data_header, &ack_body, p_ack_data, p_ack_offset);
                        }
                        break;
                        case RS485_FX_Lightning:{
                            ack_body.ack = RS485_Ack_Err_Command;
                            Rs485_Slave_Ack_Package1(&data_header, &ack_body, p_ack_data, p_ack_offset);
                        }
                        break;
                        case RS485_FX_TV:{
                            ack_body.ack = RS485_Ack_Err_Command;
                            Rs485_Slave_Ack_Package1(&data_header, &ack_body, p_ack_data, p_ack_offset);
                        }
                        break;
                        case RS485_FX_Candle:{
                            ack_body.ack = RS485_Ack_Err_Command;
                            Rs485_Slave_Ack_Package1(&data_header, &ack_body, p_ack_data, p_ack_offset);
                        }
                        break;
                        case RS485_FX_Fire:{
                            ack_body.ack = RS485_Ack_Err_Command;
                            Rs485_Slave_Ack_Package1(&data_header, &ack_body, p_ack_data, p_ack_offset);
                        }
                        break;
                        case RS485_FX_Strobe:{
                            ack_body.ack = RS485_Ack_Err_Command;
                            Rs485_Slave_Ack_Package1(&data_header, &ack_body, p_ack_data, p_ack_offset);
                        }
                        break;
                        case RS485_FX_Explosion:{
                            ack_body.ack = RS485_Ack_Err_Command;
                            Rs485_Slave_Ack_Package1(&data_header, &ack_body, p_ack_data, p_ack_offset);
                        }
                        break;
                        case RS485_FX_FaultBulb:{
                            ack_body.ack = RS485_Ack_Err_Command;
                            Rs485_Slave_Ack_Package1(&data_header, &ack_body, p_ack_data, p_ack_offset);
                        }
                        break;
                        case RS485_FX_Pulsing:{
                            ack_body.ack = RS485_Ack_Err_Command;
                            Rs485_Slave_Ack_Package1(&data_header, &ack_body, p_ack_data, p_ack_offset);
                        }
                        break;
                        case RS485_FX_Welding:{
                            ack_body.ack = RS485_Ack_Err_Command;
                            Rs485_Slave_Ack_Package1(&data_header, &ack_body, p_ack_data, p_ack_offset);
                        }
                        break;
                        case RS485_FX_CopCar:{
                            ack_body.ack = RS485_Ack_Err_Command;
                            Rs485_Slave_Ack_Package1(&data_header, &ack_body, p_ack_data, p_ack_offset);
                        }
                        break;
                        case RS485_FX_ColorChase:{
                            ack_body.ack = RS485_Ack_Err_Command;
                            Rs485_Slave_Ack_Package1(&data_header, &ack_body, p_ack_data, p_ack_offset);
                        }
                        break;
                        case RS485_FX_PartyLights:{
                            ack_body.ack = RS485_Ack_Err_Command;
                            Rs485_Slave_Ack_Package1(&data_header, &ack_body, p_ack_data, p_ack_offset);
                        }
                        break;
                        case RS485_FX_Fireworks:{
                            ack_body.ack = RS485_Ack_Err_Command;
                            Rs485_Slave_Ack_Package1(&data_header, &ack_body, p_ack_data, p_ack_offset);
                        }
                        break;
                        case RS485_FX_Effect_Off:{
                            ack_body.ack = RS485_Ack_Err_Command;
                            Rs485_Slave_Ack_Package1(&data_header, &ack_body, p_ack_data, p_ack_offset);
                        }
                        break;
                        case RS485_FX_I_Am_Here:{
                            ack_body.ack = RS485_Ack_Err_Command;
                            Rs485_Slave_Ack_Package1(&data_header, &ack_body, p_ack_data, p_ack_offset);
                        }
                        break;
                        default:{
							ack_body.ack = RS485_Ack_Err_Command;
                            Rs485_Slave_Ack_Package1(&data_header, &ack_body, p_ack_data, p_ack_offset);
						}break;
                    }
                }
                break;
                case RS485_Cmd_Dimming_Curve:{
                    
                }
                break;
                case RS485_Cmd_Fan:{
                    data_header.arg_size = sizeof(rs485_fan_t);
                    memcpy(p_ack_data + *p_ack_offset, &data_header, sizeof(rs485_cmd_header_t));
                    *p_ack_offset += sizeof(rs485_cmd_header_t);
                    #if 1   //使用时再恢复，消除警告
                    ((rs485_fan_t*)(p_ack_data + *p_ack_offset))->mode = g_rs485_data.fan.mode;
                    ((rs485_fan_t*)(p_ack_data + *p_ack_offset))->speed = Get_Fan_Curr_Spd(0);
                    ((rs485_fan_t*)(p_ack_data + *p_ack_offset))->fan_index = p_decode->fan.fan_index;
                    ((rs485_fan_t*)(p_ack_data + *p_ack_offset))->err_msg = 0;
                    #endif
                    *p_ack_offset += sizeof(rs485_fan_t);
                }
                break;
                case RS485_Cmd_Power_Suppy:{
                    
                }
                break;
                case RS485_Cmd_Battery_State:{
                    
                }
                break;
                case RS485_Cmd_Switch:{ 
                    
                }
                break;
                case RS485_Cmd_FileTransfer:{
                    ack_body.ack = RS485_Ack_Err_Command;
                    Rs485_Slave_Ack_Package1(&data_header, &ack_body, p_ack_data, p_ack_offset);
                }
                break;
                case RS485_Cmd_Temperature_Msg:{
                    data_header.arg_size = sizeof(rs485_temperature_msg_t);
                    memcpy(p_ack_data + *p_ack_offset, &data_header, sizeof(rs485_cmd_header_t));
                    *p_ack_offset += sizeof(rs485_cmd_header_t);
                    #if 1   //使用时再恢复，消除警告
                    ((rs485_temperature_msg_t*)(p_ack_data + *p_ack_offset))->type = p_decode->temperature_msg.type;
                    switch(p_decode->temperature_msg.type){
                        case 1:{//COB
                            ((rs485_temperature_msg_t*)(p_ack_data + *p_ack_offset))->value = g_adc_para.cob_temper;
                            ((rs485_temperature_msg_t*)(p_ack_data + *p_ack_offset))->err_state = 0;
                        }
                        break;
                        case 2:{//环境
                            ((rs485_temperature_msg_t*)(p_ack_data + *p_ack_offset))->value = g_adc_para.envir_temper;
                            ((rs485_temperature_msg_t*)(p_ack_data + *p_ack_offset))->err_state = 0;
                        }
                        break;
                        case 3:{//PCB
                            ((rs485_temperature_msg_t*)(p_ack_data + *p_ack_offset))->value = g_adc_para.borad_temper;
                            ((rs485_temperature_msg_t*)(p_ack_data + *p_ack_offset))->err_state = 0;
                        }
                        break;
                        case 4:{//运行时间
                            ((rs485_temperature_msg_t*)(p_ack_data + *p_ack_offset))->value = (uint16_t)(g_runtime / 60);
                            ((rs485_temperature_msg_t*)(p_ack_data + *p_ack_offset))->err_state = 0;
                        }
                        break;
						case 5:{//菲涅尔温度
								if( Electronic_Buf.Bayonet_Status == 1 && Electronic_Buf.Bayont_Send_State == Electronic_Idle )
								{
									Task_AccessBaylet(Electronic_Buf.W_Bayont_Buf.Electronic_Data,0,RS485_Cmd_Temperature_Msg,p_decode->temperature_msg.type);
									#ifdef ENCODE
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
										DDL_DelayMS(5);
										#endif
									}
							  }
                            ((rs485_temperature_msg_t*)(p_ack_data + *p_ack_offset))->value = (uint16_t)Electronic_Buf.Fresnel_Temperature1;
                            ((rs485_temperature_msg_t*)(p_ack_data + *p_ack_offset))->err_state = 0;
                        }
                        break;
						case 6:{//菲涅尔温度
								if( Electronic_Buf.Bayonet_Status == 1 && Electronic_Buf.Bayont_Send_State == Electronic_Idle )
								{
									Task_AccessBaylet(Electronic_Buf.W_Bayont_Buf.Electronic_Data,0,RS485_Cmd_Temperature_Msg,p_decode->temperature_msg.type);
									#ifdef ENCODE
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
										DDL_DelayMS(5);
										#endif
									}
							  }
                            ((rs485_temperature_msg_t*)(p_ack_data + *p_ack_offset))->value = (uint16_t)Electronic_Buf.Fresnel_Temperature2;
                            ((rs485_temperature_msg_t*)(p_ack_data + *p_ack_offset))->err_state = 0;
                        }
                        break;
                        default:break;
                    }
                    #endif
                    *p_ack_offset += sizeof(rs485_temperature_msg_t);
                }
                break;
                case RS485_Cmd_Sys_FX_II:{
                    switch(p_decode->sys_fx_2.type){
                        case RS485_FX_II_Paparazzi_2:{
                            ack_body.ack = RS485_Ack_Err_Command;
                            Rs485_Slave_Ack_Package1(&data_header, &ack_body, p_ack_data, p_ack_offset);
                        }
                        break;
                        case RS485_FX_II_Lightning_2:{
                            ack_body.ack = RS485_Ack_Err_Command;
                            Rs485_Slave_Ack_Package1(&data_header, &ack_body, p_ack_data, p_ack_offset);
                        }
                        break;
                        case RS485_FX_II_TV_2:{
                            ack_body.ack = RS485_Ack_Err_Command;
                            Rs485_Slave_Ack_Package1(&data_header, &ack_body, p_ack_data, p_ack_offset);
                        }
                        break;
                        case RS485_FX_II_Fire_2:{
                            ack_body.ack = RS485_Ack_Err_Command;
                            Rs485_Slave_Ack_Package1(&data_header, &ack_body, p_ack_data, p_ack_offset);
                        }
                        break;
                        case RS485_FX_II_Strobe_2:{
                            ack_body.ack = RS485_Ack_Err_Command;
                            Rs485_Slave_Ack_Package1(&data_header, &ack_body, p_ack_data, p_ack_offset);
                        }
                        break;
                        case RS485_FX_II_Explosion_2:{
                            ack_body.ack = RS485_Ack_Err_Command;
                            Rs485_Slave_Ack_Package1(&data_header, &ack_body, p_ack_data, p_ack_offset);
                        }
                        break;
                        case RS485_FX_II_Fault_Bulb_2:{
                            ack_body.ack = RS485_Ack_Err_Command;
                            Rs485_Slave_Ack_Package1(&data_header, &ack_body, p_ack_data, p_ack_offset);
                        }
                        break;
                        case RS485_FX_II_Pulsing_2:{
                            ack_body.ack = RS485_Ack_Err_Command;
                            Rs485_Slave_Ack_Package1(&data_header, &ack_body, p_ack_data, p_ack_offset);
                        }
                        break;
                        case RS485_FX_II_Welding_2:{
                            ack_body.ack = RS485_Ack_Err_Command;
                            Rs485_Slave_Ack_Package1(&data_header, &ack_body, p_ack_data, p_ack_offset);
                        }
                        break;
                        case RS485_FX_II_Cop_Car_2:{
                            ack_body.ack = RS485_Ack_Err_Command;
                            Rs485_Slave_Ack_Package1(&data_header, &ack_body, p_ack_data, p_ack_offset);
                        }
                        break;
                        case RS485_FX_II_Party_Lights_2:{
                            ack_body.ack = RS485_Ack_Err_Command;
                            Rs485_Slave_Ack_Package1(&data_header, &ack_body, p_ack_data, p_ack_offset);
                        }
                        break;
                        case RS485_FX_II_Fireworks_2:{
                            ack_body.ack = RS485_Ack_Err_Command;
                            Rs485_Slave_Ack_Package1(&data_header, &ack_body, p_ack_data, p_ack_offset);
                        }
                        break;
                        case RS485_FX_II_Lightning_3:{
                            ack_body.ack = RS485_Ack_Err_Command;
                            Rs485_Slave_Ack_Package1(&data_header, &ack_body, p_ack_data, p_ack_offset);
                        }
                        break;
                        case RS485_FX_II_TV_3:{
                            ack_body.ack = RS485_Ack_Err_Command;
                            Rs485_Slave_Ack_Package1(&data_header, &ack_body, p_ack_data, p_ack_offset);
                        }
                        break;
                        case RS485_FX_II_Fire_3:{
                            ack_body.ack = RS485_Ack_Err_Command;
                            Rs485_Slave_Ack_Package1(&data_header, &ack_body, p_ack_data, p_ack_offset);
                        }
                        break;
                        case RS485_FX_II_Faulty_Bulb_3:{
                            ack_body.ack = RS485_Ack_Err_Command;
                            Rs485_Slave_Ack_Package1(&data_header, &ack_body, p_ack_data, p_ack_offset);
                        }
                        break;
                        case RS485_FX_II_Pulsing_3:{
                            ack_body.ack = RS485_Ack_Err_Command;
                            Rs485_Slave_Ack_Package1(&data_header, &ack_body, p_ack_data, p_ack_offset);
                        }
                        break;
                        case RS485_FX_II_Cop_Car_3:{
                            ack_body.ack = RS485_Ack_Err_Command;
                            Rs485_Slave_Ack_Package1(&data_header, &ack_body, p_ack_data, p_ack_offset);
                        }
                        break;
                        default:{
							ack_body.ack = RS485_Ack_Err_Command;
                            Rs485_Slave_Ack_Package1(&data_header, &ack_body, p_ack_data, p_ack_offset);
						}break;
                    }
                }
                break;
                case RS485_Cmd_Self_Adaption:{
					uint8_t led_color_type = 0, count = 1;
					static uint8_t s_self_adapt_version_not_match_cnt = 0;
					if(0 == p_decode->self_adaption.led_color_type)
					{
						led_color_type = 1;
						count = LED_CHANNEL_NUM;
					}
					else
					{
						led_color_type = p_decode->self_adaption.led_color_type;
					}
					if(data_header.arg_size != (sizeof(rs485_self_adaption_t)*count))
					{
						if(s_self_adapt_version_not_match_cnt++ > 10)
						{
							s_self_adapt_version_not_match_cnt = 11;
							g_self_adapt_version_not_match = 1;
							
						}
					}
					else
					{
						s_self_adapt_version_not_match_cnt = 0;
						g_self_adapt_version_not_match = 0;
					}
					for(uint8_t i = 0; i < count; i++)
					{
						data_header.arg_size = sizeof(rs485_self_adaption_t);

						memcpy(p_ack_data + *p_ack_offset, &data_header, sizeof(rs485_cmd_header_t));
						*p_ack_offset += sizeof(rs485_cmd_header_t);
						#if 1   //使用时再恢复，消除警告
						((rs485_self_adaption_t*)(p_ack_data + *p_ack_offset))->led_color_type = p_decode->self_adaption.led_color_type;
                         uint8_t index = led_color_type + i;
						switch(led_color_type + i){
							case 1:
							{
								#ifdef PR_307_L3
								((rs485_self_adaption_t*)(p_ack_data + *p_ack_offset))->led_pwm = g_rs485_data.self_adapt_pwm.pwm[index - 1];//g_led_pwm_arg.led_pwm[index - 1];
								((rs485_self_adaption_t*)(p_ack_data + *p_ack_offset))->led_voltage = *g_adc_adaptive_para.p_volt[index - 1];
								#endif
								
								#ifdef PR_308_L2
								((rs485_self_adaption_t*)(p_ack_data + *p_ack_offset))->led_pwm = g_rs485_data.self_adapt_pwm.pwm[index - 1];//g_led_pwm_arg.led_pwm[index - 1];
								((rs485_self_adaption_t*)(p_ack_data + *p_ack_offset))->led_voltage = *g_adc_adaptive_para.p_volt[index - 1];   
								#endif								
							}
							break;
							
							case 2:
							{
								#ifdef PR_307_L3
								((rs485_self_adaption_t*)(p_ack_data + *p_ack_offset))->led_pwm = g_rs485_data.self_adapt_pwm.pwm[index - 1];//g_led_pwm_arg.led_pwm[index - 1];
								((rs485_self_adaption_t*)(p_ack_data + *p_ack_offset))->led_voltage = *g_adc_adaptive_para.p_volt[index - 1];
								#endif
								
								#ifdef PR_308_L2
								((rs485_self_adaption_t*)(p_ack_data + *p_ack_offset))->led_pwm = g_rs485_data.self_adapt_pwm.pwm[index - 1];//g_led_pwm_arg.led_pwm[index - 1];
								((rs485_self_adaption_t*)(p_ack_data + *p_ack_offset))->led_voltage = *g_adc_adaptive_para.p_volt[index - 1]; 
								#endif
							}
							break;
							case 3:
							{
								#ifdef PR_307_L3
								((rs485_self_adaption_t*)(p_ack_data + *p_ack_offset))->led_pwm = g_rs485_data.self_adapt_pwm.pwm[index - 1];//g_led_pwm_arg.led_pwm[index - 1];
								((rs485_self_adaption_t*)(p_ack_data + *p_ack_offset))->led_voltage = *g_adc_adaptive_para.p_volt[index - 1];
								#endif
								
								#ifdef PR_308_L2
								((rs485_self_adaption_t*)(p_ack_data + *p_ack_offset))->led_pwm = g_rs485_data.self_adapt_pwm.pwm[5];//g_led_pwm_arg.led_pwm[5];
								((rs485_self_adaption_t*)(p_ack_data + *p_ack_offset))->led_voltage = *g_adc_adaptive_para.p_volt[5];
								#endif
							}	
							break;
							case 4:
							{
								#ifdef PR_307_L3
								((rs485_self_adaption_t*)(p_ack_data + *p_ack_offset))->led_pwm = g_rs485_data.self_adapt_pwm.pwm[index - 1];//g_led_pwm_arg.led_pwm[index - 1];
								((rs485_self_adaption_t*)(p_ack_data + *p_ack_offset))->led_voltage = *g_adc_adaptive_para.p_volt[index - 1];
								#endif
								#ifdef PR_308_L2
								((rs485_self_adaption_t*)(p_ack_data + *p_ack_offset))->led_pwm = g_rs485_data.self_adapt_pwm.pwm[index - 1];//g_led_pwm_arg.led_pwm[index - 1];
								((rs485_self_adaption_t*)(p_ack_data + *p_ack_offset))->led_voltage = *g_adc_adaptive_para.p_volt[index - 1];    
								#endif
							}
							break;
							case 5:
							{
								#ifdef PR_307_L3
								((rs485_self_adaption_t*)(p_ack_data + *p_ack_offset))->led_pwm = g_rs485_data.self_adapt_pwm.pwm[index - 1];//g_led_pwm_arg.led_pwm[index - 1];
								((rs485_self_adaption_t*)(p_ack_data + *p_ack_offset))->led_voltage = *g_adc_adaptive_para.p_volt[index - 1];
								#endif
								#ifdef PR_308_L2
								((rs485_self_adaption_t*)(p_ack_data + *p_ack_offset))->led_pwm = g_rs485_data.self_adapt_pwm.pwm[index - 1];//g_led_pwm_arg.led_pwm[index - 1];
								((rs485_self_adaption_t*)(p_ack_data + *p_ack_offset))->led_voltage = *g_adc_adaptive_para.p_volt[index - 1];  
								#endif
							}
							break;
							case 6:
							{
								#ifdef PR_307_L3
								((rs485_self_adaption_t*)(p_ack_data + *p_ack_offset))->led_pwm = g_rs485_data.self_adapt_pwm.pwm[index - 1];//g_led_pwm_arg.led_pwm[index - 1];
								((rs485_self_adaption_t*)(p_ack_data + *p_ack_offset))->led_voltage = *g_adc_adaptive_para.p_volt[index - 1];
								#endif
								
								#ifdef PR_308_L2
								((rs485_self_adaption_t*)(p_ack_data + *p_ack_offset))->led_pwm = g_rs485_data.self_adapt_pwm.pwm[2];//g_led_pwm_arg.led_pwm[2];
								((rs485_self_adaption_t*)(p_ack_data + *p_ack_offset))->led_voltage = *g_adc_adaptive_para.p_volt[2];
								#endif                       
							}
							break;
							case 7:
							{
								#ifdef PR_307_L3
								((rs485_self_adaption_t*)(p_ack_data + *p_ack_offset))->led_pwm = g_rs485_data.self_adapt_pwm.pwm[index - 1];//g_led_pwm_arg.led_pwm[index - 1];
								((rs485_self_adaption_t*)(p_ack_data + *p_ack_offset))->led_voltage = *g_adc_adaptive_para.p_volt[index - 1];
								#endif
								#ifdef PR_308_L2
								((rs485_self_adaption_t*)(p_ack_data + *p_ack_offset))->led_pwm = g_rs485_data.self_adapt_pwm.pwm[index - 1];//g_led_pwm_arg.led_pwm[index - 1];
								((rs485_self_adaption_t*)(p_ack_data + *p_ack_offset))->led_voltage = *g_adc_adaptive_para.p_volt[index - 1];
								#endif
							}
							break;
							default:break;
						}
						#endif
						*p_ack_offset += sizeof(rs485_self_adaption_t);
					}
                }
                break;
                case RS485_Cmd_Factory_RGBWW:{
                    ack_body.ack = RS485_Ack_Err_Command;
                    Rs485_Slave_Ack_Package1(&data_header, &ack_body, p_ack_data, p_ack_offset);
                }
                break;
                case RS485_Cmd_Source:{
                    ack_body.ack = RS485_Ack_Err_Command;
                    Rs485_Slave_Ack_Package1(&data_header, &ack_body, p_ack_data, p_ack_offset);
                }
                break;
                case RS485_Cmd_IlluminationMode:{ 
                    
                }
                break;
                case RS485_Cmd_Color_Mixing:{
                    ack_body.ack = RS485_Ack_Err_Command;
                    Rs485_Slave_Ack_Package1(&data_header, &ack_body, p_ack_data, p_ack_offset);
                }
                break;
                case RS485_Cmd_Err_Msg:{
                    data_header.arg_size = sizeof(rs485_err_msg_t);
                    memcpy(p_ack_data + *p_ack_offset, &data_header, sizeof(rs485_cmd_header_t));
                    *p_ack_offset += sizeof(rs485_cmd_header_t);
					
                    ((rs485_err_msg_t*)(p_ack_data + *p_ack_offset))->err_state1 = g_error_state.err_msg;
					
					*p_ack_offset += sizeof(rs485_err_msg_t);
                }
                break;
                case RS485_Cmd_PFX_Ctrl:{
                    ack_body.ack = RS485_Ack_Err_Command;
                    Rs485_Slave_Ack_Package1(&data_header, &ack_body, p_ack_data, p_ack_offset);
                }
                break;
                case RS485_Cmd_CFX_Bank_RW:{
                    data_header.arg_size = sizeof(rs485_cfx_bank_rw_t);
                    memcpy(p_ack_data + *p_ack_offset, &data_header, sizeof(rs485_cmd_header_t));
                    *p_ack_offset += sizeof(rs485_cmd_header_t);
                    #if 1
                    ((rs485_cfx_bank_rw_t*)(p_ack_data + *p_ack_offset))->picker = SidusProFile_CFX_BankInfo_Get(0);
                    ((rs485_cfx_bank_rw_t*)(p_ack_data + *p_ack_offset))->touch_bar = SidusProFile_CFX_BankInfo_Get(1);
                    ((rs485_cfx_bank_rw_t*)(p_ack_data + *p_ack_offset))->music = SidusProFile_CFX_BankInfo_Get(2);
                    #endif
                    *p_ack_offset += sizeof(rs485_cfx_bank_rw_t);
                }
                break;
                case RS485_Cmd_CFX_Ctrl:{
                    ack_body.ack = RS485_Ack_Err_Command;
                    Rs485_Slave_Ack_Package1(&data_header, &ack_body, p_ack_data, p_ack_offset);
                }
                break;
                case RS485_Cmd_CFX_Preview:{
                    ack_body.ack = RS485_Ack_Err_Command;
                    Rs485_Slave_Ack_Package1(&data_header, &ack_body, p_ack_data, p_ack_offset);
                }
                break;
                case RS485_Cmd_MFX_Ctrl:{
                    ack_body.ack = RS485_Ack_Err_Command;
                    Rs485_Slave_Ack_Package1(&data_header, &ack_body, p_ack_data, p_ack_offset);
                }
                break;
                case RS485_Cmd_RGBWW:{
                    ack_body.ack = RS485_Ack_Err_Command;
                    Rs485_Slave_Ack_Package1(&data_header, &ack_body, p_ack_data, p_ack_offset);
                }
                break;
                case RS485_Cmd_CFX_Name:{
                    data_header.arg_size = sizeof(rs485_cfx_name_t);
                    memcpy(p_ack_data + *p_ack_offset, &data_header, sizeof(rs485_cmd_header_t));
                    *p_ack_offset += sizeof(rs485_cmd_header_t);
                    #if 1
                    ((rs485_cfx_name_t*)(p_ack_data + *p_ack_offset))->effect_type = p_decode->cfx_name.effect_type;
                    ((rs485_cfx_name_t*)(p_ack_data + *p_ack_offset))->bank = p_decode->cfx_name.bank;
                    ((rs485_cfx_name_t*)(p_ack_data + *p_ack_offset))->code_type = 0;
                    ((rs485_cfx_name_t*)(p_ack_data + *p_ack_offset))->name[0] = 0;
                    if( false == SidusProFile_Get_Name(p_decode->cfx_name.effect_type, 
                                          p_decode->cfx_name.bank, 
                                          (char*)((rs485_cfx_name_t*)(p_ack_data + *p_ack_offset))->name))
										  {
											strcpy((char*)((rs485_cfx_name_t*)(p_ack_data + *p_ack_offset))->name,"NO FX");
										  }
                    #endif
                    *p_ack_offset += sizeof(rs485_cfx_name_t);
                }
                break;
                case RS485_Cmd_Curr_Light_Mode:{
                    
                }
                break;
                case RS485_Cmd_PixelEffect:{
                    switch(p_decode->pixel_fx.type){
                        case RS485_PixelFX_Color_Cut:{
                            ack_body.ack = RS485_Ack_Err_Command;
                            Rs485_Slave_Ack_Package1(&data_header, &ack_body, p_ack_data, p_ack_offset);
                        }
                        break;
                        case RS485_PixelFX_Color_Rotate:{
                            ack_body.ack = RS485_Ack_Err_Command;
                            Rs485_Slave_Ack_Package1(&data_header, &ack_body, p_ack_data, p_ack_offset);
                        }
                        break;
                        case RS485_PixelFX_One_Color_Move:{
                            ack_body.ack = RS485_Ack_Err_Command;
                            Rs485_Slave_Ack_Package1(&data_header, &ack_body, p_ack_data, p_ack_offset);
                        }
                        break;
                        case RS485_PixelFX_Two_Color_Move:{
                            ack_body.ack = RS485_Ack_Err_Command;
                            Rs485_Slave_Ack_Package1(&data_header, &ack_body, p_ack_data, p_ack_offset);
                        }
                        break;
                        case RS485_PixelFX_Three_Color_Move:{
                            ack_body.ack = RS485_Ack_Err_Command;
                            Rs485_Slave_Ack_Package1(&data_header, &ack_body, p_ack_data, p_ack_offset);
                        }
                        break;
                        case RS485_PixelFX_Pixel_Fire:{
                            ack_body.ack = RS485_Ack_Err_Command;
                            Rs485_Slave_Ack_Package1(&data_header, &ack_body, p_ack_data, p_ack_offset);
                        }
                        break;
                        case RS485_PixelFX_Many_Color_Move:{
                            ack_body.ack = RS485_Ack_Err_Command;
                            Rs485_Slave_Ack_Package1(&data_header, &ack_body, p_ack_data, p_ack_offset);
                        }
                        break;
                        case RS485_PixelFX_On_Off_Effect:{
                            ack_body.ack = RS485_Ack_Err_Command;
                            Rs485_Slave_Ack_Package1(&data_header, &ack_body, p_ack_data, p_ack_offset);
                        }
                        break;
                        case RS485_PixelFX_Belt_Effect:{
                            switch(p_decode->pixel_fx.arg.belt_fx.type){
                                case RS485_BeltFX_Fade:{
                                    ack_body.ack = RS485_Ack_Err_Command;
                                    Rs485_Slave_Ack_Package1(&data_header, &ack_body, p_ack_data, p_ack_offset);
                                }
                                break;
                                case RS485_BeltFX_Holoday:{
                                    ack_body.ack = RS485_Ack_Err_Command;
                                    Rs485_Slave_Ack_Package1(&data_header, &ack_body, p_ack_data, p_ack_offset);
                                }
                                break;
                                case RS485_BeltFX_One_Color_Marquee:{
                                    ack_body.ack = RS485_Ack_Err_Command;
                                    Rs485_Slave_Ack_Package1(&data_header, &ack_body, p_ack_data, p_ack_offset);
                                }
                                break;
                                case RS485_BeltFX_Full_Color_Marquee:{
                                    ack_body.ack = RS485_Ack_Err_Command;
                                    Rs485_Slave_Ack_Package1(&data_header, &ack_body, p_ack_data, p_ack_offset);
                                }
                                break;
                                case RS485_BeltFX_Starry_Sky:{
                                    ack_body.ack = RS485_Ack_Err_Command;
                                    Rs485_Slave_Ack_Package1(&data_header, &ack_body, p_ack_data, p_ack_offset);
                                }
                                break;
                                case RS485_BeltFX_Aurora:{
                                    ack_body.ack = RS485_Ack_Err_Command;
                                    Rs485_Slave_Ack_Package1(&data_header, &ack_body, p_ack_data, p_ack_offset);
                                }
                                break;
                                case RS485_BeltFX_Racing:{
                                    ack_body.ack = RS485_Ack_Err_Command;
                                    Rs485_Slave_Ack_Package1(&data_header, &ack_body, p_ack_data, p_ack_offset);
                                }
                                break;
                                case RS485_BeltFX_Train:{
                                    ack_body.ack = RS485_Ack_Err_Command;
                                    Rs485_Slave_Ack_Package1(&data_header, &ack_body, p_ack_data, p_ack_offset);
                                }
                                break;
                                case RS485_BeltFX_Snake:{
                                    ack_body.ack = RS485_Ack_Err_Command;
                                    Rs485_Slave_Ack_Package1(&data_header, &ack_body, p_ack_data, p_ack_offset);
                                }
                                break;
                                case RS485_BeltFX_Forest:{
                                    ack_body.ack = RS485_Ack_Err_Command;
                                    Rs485_Slave_Ack_Package1(&data_header, &ack_body, p_ack_data, p_ack_offset);
                                }
                                break;
                                case RS485_BeltFX_Bonfire:{
                                    ack_body.ack = RS485_Ack_Err_Command;
                                    Rs485_Slave_Ack_Package1(&data_header, &ack_body, p_ack_data, p_ack_offset);
                                }
                                break;
                                case RS485_BeltFX_Brook:{
                                    ack_body.ack = RS485_Ack_Err_Command;
                                    Rs485_Slave_Ack_Package1(&data_header, &ack_body, p_ack_data, p_ack_offset);
                                }
                                break;
                                case RS485_BeltFX_Game:{
                                    ack_body.ack = RS485_Ack_Err_Command;
                                    Rs485_Slave_Ack_Package1(&data_header, &ack_body, p_ack_data, p_ack_offset);
                                }
                                break;
                                case RS485_BeltFX_Timewait:{
                                    ack_body.ack = RS485_Ack_Err_Command;
                                    Rs485_Slave_Ack_Package1(&data_header, &ack_body, p_ack_data, p_ack_offset);
                                }
                                break;
                                case RS485_BeltFX_Party:{
                                    ack_body.ack = RS485_Ack_Err_Command;
                                    Rs485_Slave_Ack_Package1(&data_header, &ack_body, p_ack_data, p_ack_offset);
                                }
                                break;
                                case RS485_BeltFX_Belt_Fireworks:{
                                    ack_body.ack = RS485_Ack_Err_Command;
                                    Rs485_Slave_Ack_Package1(&data_header, &ack_body, p_ack_data, p_ack_offset);
                                }
                                break;
                                case RS485_BeltFX_Marbles:{
                                    ack_body.ack = RS485_Ack_Err_Command;
                                    Rs485_Slave_Ack_Package1(&data_header, &ack_body, p_ack_data, p_ack_offset);
                                }
                                break;
                                case RS485_BeltFX_Pendulum:{
                                    ack_body.ack = RS485_Ack_Err_Command;
                                    Rs485_Slave_Ack_Package1(&data_header, &ack_body, p_ack_data, p_ack_offset);
                                }
                                break;
                                case RS485_BeltFX_Flash_Point:{
                                    ack_body.ack = RS485_Ack_Err_Command;
                                    Rs485_Slave_Ack_Package1(&data_header, &ack_body, p_ack_data, p_ack_offset);
                                }
                                break;
                                default:{
									ack_body.ack = RS485_Ack_Err_Command;
                                    Rs485_Slave_Ack_Package1(&data_header, &ack_body, p_ack_data, p_ack_offset);
								}break;
                            }
                        }
                        break;
                        case RS485_PixelFX_Music:{
                            ack_body.ack = RS485_Ack_Err_Command;
                            Rs485_Slave_Ack_Package1(&data_header, &ack_body, p_ack_data, p_ack_offset);
                        }
                        break;
                        case RS485_PixelFX_Rainbow:{
                            ack_body.ack = RS485_Ack_Err_Command;
                            Rs485_Slave_Ack_Package1(&data_header, &ack_body, p_ack_data, p_ack_offset);
                        }
                        break;
                        default:{
                            ack_body.ack = RS485_Ack_Err_Command;
                            Rs485_Slave_Ack_Package1(&data_header, &ack_body, p_ack_data, p_ack_offset);
                        }break;
                    }
                }
                break;
                case RS485_Cmd_DMX_Strobe:{
                    ack_body.ack = RS485_Ack_Err_Command;
                    Rs485_Slave_Ack_Package1(&data_header, &ack_body, p_ack_data, p_ack_offset);
                }
                break;
                case RS485_Cmd_Partition_Color:{
                    ack_body.ack = RS485_Ack_Err_Command;
                    Rs485_Slave_Ack_Package1(&data_header, &ack_body, p_ack_data, p_ack_offset);
                }
                break;
                case RS485_Cmd_Partition_Effect:{
                    ack_body.ack = RS485_Ack_Err_Command;
                    Rs485_Slave_Ack_Package1(&data_header, &ack_body, p_ack_data, p_ack_offset);
                }
				break;
				case RS485_Cmd_High_speed:{
					ack_body.ack = RS485_Ack_Err_Command; 
					Rs485_Slave_Ack_Package1(&data_header, &ack_body, p_ack_data, p_ack_offset);
				}
				break;
				case RS485_Cmd_Analog_Dim:{	
					update_power_rate();
					if(g_rs485_data.high_speed.mode == 1)
					{
						float analog_intensity = 0.0f;
						if(g_rs485_data.light_mode_para.intensity > 0.0f)
						{
							analog_intensity = 0.001f + ((g_rs485_data.light_mode_para.intensity - 0.2f)/(0.8f))*0.999f;
							if(analog_intensity >= 1.0f)
							{
								analog_intensity = 1.0f;
							}
						}
						switch(g_rs485_data.light_mode_state.command_1){
							case RS485_Cmd_CCT:{
							color_cct_calc(analog_intensity, g_rs485_data.light_mode_para.cct, 
							g_rs485_data.light_mode_para.duv, (enum ilumination_mode)g_rs485_data.illumination_mode.mode, (enum mixing_curve)g_rs485_data.dimming_curve.curve, 
							g_power_factor.sum_power_rate, &g_str_cob_analog, &g_str_cob_analog_lux);
							}
							break;
#ifdef PR_307_L3
							case RS485_Cmd_HSI:{
							color_hsi_calc(analog_intensity, (float)(g_rs485_data.light_mode_para.hue / 100.0f), (float)(g_rs485_data.light_mode_para.sat / 100.0f), //Phoebe:数据类型
							g_rs485_data.light_mode_para.cct, 0.0f,(enum mixing_curve)g_rs485_data.dimming_curve.curve, g_power_factor.sum_power_rate, &g_str_cob_analog, &g_str_cob_analog_lux);
							}
							break;
							case RS485_Cmd_GEL:{ 
							color_gel_calc(analog_intensity, g_rs485_data.light_mode_para.origin, 
							g_rs485_data.light_mode_para.series, g_rs485_data.light_mode_para.cct, g_rs485_data.light_mode_para.color, \
							(enum mixing_curve)g_rs485_data.dimming_curve.curve, g_power_factor.sum_power_rate,  &g_str_cob_analog, &g_str_cob_analog_lux);
							}
							break;
							case RS485_Cmd_RGB:{
							color_rgb_calc(analog_intensity, (float)g_rs485_data.light_mode_para.red_ratio / 1000, (float)g_rs485_data.light_mode_para.green_ratio / 1000,
							(float)g_rs485_data.light_mode_para.blue_ratio / 1000,(enum mixing_curve)g_rs485_data.dimming_curve.curve, g_power_factor.sum_power_rate, &g_str_cob_analog, &g_str_cob_analog_lux);
							}
							break;
							
							case RS485_Cmd_XY_Coordinate:{
							color_coordinate_calc(analog_intensity, (float)g_rs485_data.light_mode_para.x / 10000.f,
							(float)g_rs485_data.light_mode_para.y / 10000.f, (enum mixing_curve)g_rs485_data.dimming_curve.curve, g_power_factor.sum_power_rate, &g_str_cob_analog, &g_str_cob_analog_lux);
							}
							break;
							
							case RS485_Cmd_Source:{
							color_source_calc(analog_intensity, g_rs485_data.light_mode_para.type, (float)g_rs485_data.light_mode_para.x / 10000.f, (float)g_rs485_data.light_mode_para.y / 10000.f, g_rs485_data.illumination_mode.mode, 
							(enum mixing_curve)g_rs485_data.dimming_curve.curve, g_power_factor.sum_power_rate, &g_str_cob_analog, &g_str_cob_analog_lux);
							}
							break;
#endif
							default:break;
						}
					}
					else
					{
						g_str_cob_analog.pwm[0] = 0;
						g_str_cob_analog.pwm[1] = 0;
						g_str_cob_analog.pwm[2] = 0;
						g_str_cob_analog.pwm[3] = 0;
						g_str_cob_analog.pwm[4] = 0;
					}
					
					data_header.arg_size = sizeof(rs485_analog_dim_t);
					memcpy(p_ack_data + *p_ack_offset, &data_header, sizeof(rs485_cmd_header_t));
					*p_ack_offset += sizeof(rs485_cmd_header_t);
					((rs485_analog_dim_t*)(p_ack_data + *p_ack_offset))->type = p_decode->analog_dim_t.type;
					((rs485_analog_dim_t*)(p_ack_data + *p_ack_offset))->led_color_type = p_decode->analog_dim_t.led_color_type;
					
#ifdef PR_307_L3 
					if( p_decode->analog_dim_t.type == 1)
					{
						((rs485_analog_dim_t*)(p_ack_data + *p_ack_offset))->adjust_val = g_str_cob_analog.pwm[p_decode->analog_dim_t.led_color_type - 1];//adc_value;//
					}
					else
					{
						((rs485_analog_dim_t*)(p_ack_data + *p_ack_offset))->adjust_val = g_str_cob_analog_sta[p_decode->analog_dim_t.led_color_type - 1];//adc_value;//
					}
#endif
					
#ifdef PR_308_L2			
					uint16_t anlog_pwm[5] = {0};
					if( p_decode->analog_dim_t.type == 1)
					{
						anlog_pwm[1] = g_str_cob_analog.pwm[1];
						anlog_pwm[3] = g_str_cob_analog.pwm[0];
						anlog_pwm[4] = g_str_cob_analog.pwm[2];
						((rs485_analog_dim_t*)(p_ack_data + *p_ack_offset))->adjust_val = anlog_pwm[p_decode->analog_dim_t.led_color_type - 1];
					}
					else
					{
						anlog_pwm[1] = g_str_cob_analog_sta[1];
						anlog_pwm[3] = g_str_cob_analog_sta[0];
						anlog_pwm[4] = g_str_cob_analog_sta[2];
						((rs485_analog_dim_t*)(p_ack_data + *p_ack_offset))->adjust_val = anlog_pwm[p_decode->analog_dim_t.led_color_type - 1];
					}							
#endif
					 *p_ack_offset += sizeof(rs485_analog_dim_t);
				}
				break;
				case RS485_Cmd_Pump:{
                    data_header.arg_size = sizeof(rs485_pump_t);
                    memcpy(p_ack_data + *p_ack_offset, &data_header, sizeof(rs485_cmd_header_t));
                    *p_ack_offset += sizeof(rs485_cmd_header_t);
                    #if 1   //使用时再恢复，消除警告
                    ((rs485_pump_t*)(p_ack_data + *p_ack_offset))->mode = g_rs485_data.pump_t.mode;
                    ((rs485_pump_t*)(p_ack_data + *p_ack_offset))->speed = Get_Fan_Curr_Spd(1);
                    ((rs485_pump_t*)(p_ack_data + *p_ack_offset))->pump_index = p_decode->pump_t.pump_index;
                    ((rs485_pump_t*)(p_ack_data + *p_ack_offset))->err_msg = 0;
                    #endif
                    *p_ack_offset += sizeof(rs485_pump_t);
                }
                break;
				case RS485_Cmd_Self_Adjust:{
					uint16_t self_adjust_buff[7] = {3200, 1000, 3200, 3200, 3200, 3200, 3200};
					#if 0
                    data_header.arg_size = sizeof(rs485_self_adjust_t);
                    memcpy(p_ack_data + *p_ack_offset, &data_header, sizeof(rs485_cmd_header_t));
                    *p_ack_offset += sizeof(rs485_cmd_header_t);
                    #if 1   //使用时再恢复，消除警告
                    ((rs485_self_adjust_t*)(p_ack_data + *p_ack_offset))->led_color_type = p_decode->self_adjust_t.led_color_type;
                    ((rs485_self_adjust_t*)(p_ack_data + *p_ack_offset))->adjust_dir = p_decode->self_adjust_t.adjust_dir;
					switch(p_decode->self_adjust_t.led_color_type){
						case 0:
							break;
						case 1:
						case 2:
						case 3:
						case 4:
						case 5:
						case 6:
						case 7:
							if(p_decode->self_adjust_t.adjust_dir == 1)
							{
								((rs485_self_adjust_t*)(p_ack_data + *p_ack_offset))->save_ctr = led_adc_pwm_volt_state(p_decode->self_adjust_t.led_color_type);
								((rs485_self_adjust_t*)(p_ack_data + *p_ack_offset))->adjust_val = 0;//adc_value;//
							}
							if(p_decode->self_adjust_t.adjust_dir == 0)
							{
								((rs485_self_adjust_t*)(p_ack_data + *p_ack_offset))->save_ctr = 0;
								((rs485_self_adjust_t*)(p_ack_data + *p_ack_offset))->adjust_val = self_adjust_buff[p_decode->self_adjust_t.led_color_type];//adc_value;//
							}
							
							break;
						default:
							break;
							
					 }
					
                    #endif
                    *p_ack_offset += sizeof(rs485_self_adjust_t);
					
					#else
					 data_header.arg_size = sizeof(rs485_self_adjust_t);
					 memcpy(p_ack_data + *p_ack_offset, &data_header, sizeof(rs485_cmd_header_t));
					*p_ack_offset += sizeof(rs485_cmd_header_t);
					 ((rs485_self_adjust_t*)(p_ack_data + *p_ack_offset))->led_color_type = p_decode->self_adjust_t.led_color_type;
					
					 switch(p_decode->self_adjust_t.led_color_type){
						case 0:
							break;
						case 1:
						case 2:
						case 3:
						case 4:
						case 5:
						case 6:
						case 7:
							if(p_decode->self_adjust_t.adjust_dir == 1)
							{
								((rs485_self_adjust_t*)(p_ack_data + *p_ack_offset))->save_ctr = led_adc_pwm_volt_state(p_decode->self_adjust_t.led_color_type);
								((rs485_self_adjust_t*)(p_ack_data + *p_ack_offset))->adjust_val = 0;//adc_value;//
							}
							if(p_decode->self_adjust_t.adjust_dir == 0)
							{
								((rs485_self_adjust_t*)(p_ack_data + *p_ack_offset))->save_ctr = 0;
								((rs485_self_adjust_t*)(p_ack_data + *p_ack_offset))->adjust_val = self_adjust_buff[p_decode->self_adjust_t.led_color_type];//adc_value;//
							}
							break;
						default:
							break;
							
					 }
					 *p_ack_offset += sizeof(rs485_self_adjust_t);
					#endif
                }
                break;
					case RS485_Cmd_Motor:{
					data_header.arg_size = sizeof(rs485_motor_t);
					memcpy(p_ack_data + *p_ack_offset, &data_header, sizeof(rs485_cmd_header_t));
					*p_ack_offset += sizeof(rs485_cmd_header_t);
					((rs485_motor_t *)(p_ack_data + *p_ack_offset))->type = p_decode->motor_t.type;
			
					((rs485_motor_t *)(p_ack_data + *p_ack_offset))->mode_arg.motor_mode = p_decode->motor_t.mode_arg.motor_mode;

					Pitch_Roll[p_decode->motor_t.type].mode_arg.motor_mode = p_decode->motor_t.mode_arg.motor_mode;
					
					if( p_decode->motor_t.mode_arg.motor_mode == RS485_Motor_Mode_Move )//read angel the priority higher than another commands
					{
						if( p_decode->motor_t.type == RS485_Pitch_Motor || p_decode->motor_t.type == RS485_Rotale_Motor )
						 Electronic_Buf.Bracket_Send_State = Electronic_Idle;
						if( p_decode->motor_t.type == RS485_Fresnel_Motor )
						 Electronic_Buf.Bayont_Send_State = Electronic_Idle;
					}
					//this code need to ENCODE the information
					if( p_decode->motor_t.type == RS485_Fresnel_Motor )
					{
						Electronic_Package_Fuction(Electronic_Buf.W_Bayont_Buf.Electronic_Data,0,Pitch_Roll[p_decode->motor_t.type],1);
						if( p_decode->motor_t.mode_arg.motor_mode == RS485_Motor_Mode_Move )
						{
							#ifdef ENCODE
							encode_data1 = user_plaintext_encryption(Electronic_Buf.W_Bayont_Buf.Electronic_Data,((rs485_proto_header_t *)(Electronic_Buf.W_Bayont_Buf.Electronic_Data))->msg_size);
							if( encode_data1 != NULL && encode_data1->data_len <= 500 )
							{
								memcpy(data_pin,Electronic_Buf.W_Bayont_Buf.Electronic_Data,11);
								memcpy(data_pin+11,encode_data1->data,encode_data1->data_len);
								push_electronic_data(&Bayont_Queue,data_pin,encode_data1->data_len+11);
							}
							#else
							hal_rs485_3_usart_send(Electronic_Buf.W_Bayont_Buf.Electronic_Data,((rs485_proto_header_t *)(Electronic_Buf.W_Bayont_Buf.Electronic_Data))->msg_size);
							#endif
							Pitch_Roll_Angel[p_decode->motor_t.type].mode_arg.motor_arg.motor_move_arg.motor_angle =  frensel_last;
						}
					}
					//this code need to ENCODE the information
					if(p_decode->motor_t.type == RS485_Pitch_Motor || p_decode->motor_t.type == RS485_Rotale_Motor)
					{
						Electronic_Package_Fuction(Electronic_Buf.W_Bracket_Buf.Electronic_Data,0,Pitch_Roll[p_decode->motor_t.type],1);
						if(p_decode->motor_t.type > 3)
						{
						__nop();
						}
						if( p_decode->motor_t.mode_arg.motor_mode == RS485_Motor_Mode_Move )
						{	
							#ifdef ENCODE
							encode_data1 = user_plaintext_encryption(Electronic_Buf.W_Bracket_Buf.Electronic_Data,((rs485_proto_header_t *)(Electronic_Buf.W_Bracket_Buf.Electronic_Data))->msg_size);
							if( encode_data1 != NULL && encode_data1->data_len <= 500 )
							{						
								memcpy(data_pin,Electronic_Buf.W_Bracket_Buf.Electronic_Data,11);
								memcpy(data_pin+11,encode_data1->data,encode_data1->data_len);
								push_electronic_data(&Bracket_Queue,data_pin,encode_data1->data_len+11);
							}
							#else
							hal_rs485_2_usart_send(Electronic_Buf.W_Bracket_Buf.Electronic_Data,((rs485_proto_header_t *)(Electronic_Buf.W_Bracket_Buf.Electronic_Data))->msg_size);
							#endif
							if( p_decode->motor_t.type == RS485_Pitch_Motor )
							 {
									 Pitch_Roll_Angel[p_decode->motor_t.type].mode_arg.motor_arg.motor_move_arg.motor_angle = pitch_last;
							 }
							 if( p_decode->motor_t.type == RS485_Rotale_Motor )
							 {
									Pitch_Roll_Angel[p_decode->motor_t.type].mode_arg.motor_arg.motor_move_arg.motor_angle =  roll_last;
							 }
						}				
					}
					//this code need to decode the information
					if(g_gyroscope.gyroscope_type != USE_NULL)
					{
						if(p_decode->motor_t.type == RS485_Gyroscope_Pitch && p_decode->motor_t.mode_arg.motor_mode == RS485_Motor_Mode_Move )
						{
							Pitch_Roll_Angel[p_decode->motor_t.type].mode_arg.motor_arg.motor_move_arg.motor_angle = g_gyroscope.pitch_angle;
						}
						if(p_decode->motor_t.type == RS485_Gyroscope_Roll && p_decode->motor_t.mode_arg.motor_mode == RS485_Motor_Mode_Move )
						{
							Pitch_Roll_Angel[p_decode->motor_t.type].mode_arg.motor_arg.motor_move_arg.motor_angle = g_gyroscope.roll_angle;
						}
					}						
					
					switch(p_decode->motor_t.mode_arg.motor_mode)
					{			
						case RS485_Motor_Mode_Move:{
							((rs485_motor_t *)(p_ack_data + *p_ack_offset))->mode_arg.motor_arg.motor_move_arg.motor_angle = Pitch_Roll_Angel[p_decode->motor_t.type].mode_arg.motor_arg.motor_move_arg.motor_angle;
							((rs485_motor_t *)(p_ack_data + *p_ack_offset))->mode_arg.motor_arg.motor_move_arg.move_state = Pitch_Roll_Angel[p_decode->motor_t.type].mode_arg.motor_arg.motor_move_arg.move_state;
							((rs485_motor_t *)(p_ack_data + *p_ack_offset))->mode_arg.motor_arg.motor_move_arg.motor_a1 = Pitch_Roll_Angel[p_decode->motor_t.type].mode_arg.motor_arg.motor_move_arg.motor_a1;			
							((rs485_motor_t *)(p_ack_data + *p_ack_offset))->mode_arg.motor_arg.motor_move_arg.motor_acce = Pitch_Roll_Angel[p_decode->motor_t.type].mode_arg.motor_arg.motor_move_arg.motor_acce;
							((rs485_motor_t *)(p_ack_data + *p_ack_offset))->mode_arg.motor_arg.motor_move_arg.motor_d1 = Pitch_Roll_Angel[p_decode->motor_t.type].mode_arg.motor_arg.motor_move_arg.motor_d1;
							((rs485_motor_t *)(p_ack_data + *p_ack_offset))->mode_arg.motor_arg.motor_move_arg.motor_dece = Pitch_Roll_Angel[p_decode->motor_t.type].mode_arg.motor_arg.motor_move_arg.motor_dece;
							((rs485_motor_t *)(p_ack_data + *p_ack_offset))->mode_arg.motor_arg.motor_move_arg.motor_star_speed = Pitch_Roll_Angel[p_decode->motor_t.type].mode_arg.motor_arg.motor_move_arg.motor_star_speed;
							((rs485_motor_t *)(p_ack_data + *p_ack_offset))->mode_arg.motor_arg.motor_move_arg.motor_stop_speed = Pitch_Roll_Angel[p_decode->motor_t.type].mode_arg.motor_arg.motor_move_arg.motor_stop_speed;
							((rs485_motor_t *)(p_ack_data + *p_ack_offset))->mode_arg.motor_arg.motor_move_arg.motor_targe_speed = Pitch_Roll_Angel[p_decode->motor_t.type].mode_arg.motor_arg.motor_move_arg.motor_targe_speed;
							((rs485_motor_t *)(p_ack_data + *p_ack_offset))->mode_arg.motor_arg.motor_move_arg.motor_v1 = Pitch_Roll_Angel[p_decode->motor_t.type].mode_arg.motor_arg.motor_move_arg.motor_v1;					
						}
						break;
						case RS485_Motor_Mode_Crc:{
							((rs485_motor_t *)(p_ack_data + *p_ack_offset))->mode_arg.motor_arg.motor_crc_arg.crc_state = Pitch_Roll_Slave[p_decode->motor_t.type].mode_arg.motor_arg.motor_crc_arg.crc_state;
							((rs485_motor_t *)(p_ack_data + *p_ack_offset))->mode_arg.motor_arg.motor_crc_arg.crc_value = Pitch_Roll_Slave[p_decode->motor_t.type].mode_arg.motor_arg.motor_crc_arg.crc_value;
							((rs485_motor_t *)(p_ack_data + *p_ack_offset))->mode_arg.motor_arg.motor_crc_arg.sttall_max_value = Pitch_Roll_Slave[p_decode->motor_t.type].mode_arg.motor_arg.motor_crc_arg.sttall_max_value;
							((rs485_motor_t *)(p_ack_data + *p_ack_offset))->mode_arg.motor_arg.motor_crc_arg.sttall_min_value = Pitch_Roll_Slave[p_decode->motor_t.type].mode_arg.motor_arg.motor_crc_arg.sttall_min_value;
						}
						break;
						case RS485_Motor_Mode_Reset:{
							((rs485_motor_t *)(p_ack_data + *p_ack_offset))->mode_arg.motor_arg.motor_reset_arg.reset_angle = Pitch_Roll_Slave[p_decode->motor_t.type].mode_arg.motor_arg.motor_reset_arg.reset_angle;
							((rs485_motor_t *)(p_ack_data + *p_ack_offset))->mode_arg.motor_arg.motor_reset_arg.reset_state = Pitch_Roll_Slave[p_decode->motor_t.type].mode_arg.motor_arg.motor_reset_arg.reset_state;
						}
						break;
						case RS485_Motor_Mode_State:{
//							((rs485_motor_t *)(p_ack_data + *p_ack_offset))->mode_arg.motor_arg.motor_state_arg.stall_state = Pitch_Roll_Slave[p_decode->motor_t.type].mode_arg.motor_arg.motor_state_arg.stall_state;
//							((rs485_motor_t *)(p_ack_data + *p_ack_offset))->mode_arg.motor_arg.motor_state_arg.reserva1 = Pitch_Roll_Slave[p_decode->motor_t.type].mode_arg.motor_arg.motor_state_arg.reserva1;
							((rs485_motor_t *)(p_ack_data + *p_ack_offset))->mode_arg.motor_arg.motor_state_arg = Pitch_Roll_Slave[p_decode->motor_t.type].mode_arg.motor_arg.motor_state_arg;
						}
						break;
						default:break;
					}
					*p_ack_offset += sizeof(rs485_motor_t);
				}
				break;
				case RS485_Cmd_Accessories:{
						data_header.arg_size = sizeof(rs485_access_t);
						memcpy(p_ack_data + *p_ack_offset, &data_header, sizeof(rs485_cmd_header_t));
						*p_ack_offset += sizeof(rs485_cmd_header_t);
						((rs485_access_t*)(p_ack_data + *p_ack_offset))->type =  p_decode->access_t.type;
						if(p_decode->access_t.type > 0x09)
						{
							__nop();
						}
						if( p_decode->access_t.type == RS485_Access_Bracket )
						{
							if( Electronic_Buf.Bayonet_Status == 1)
							{
								Task_AccessBaylet(Electronic_Buf.W_Bayont_Buf.Electronic_Data,0,RS485_Cmd_Accessories,0);
								#ifdef ENCODE
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
									Electronic_Buf.Bayonet_Times_connect++;
									
								}
								if( Electronic_Buf.Bayonet_Times_connect == 5 )
								{
									 Electronic_Buf.Bayonet_Times_connect = 0;
									 Electronic_Buf.Electronic_Connect &= 0x01;
									 Electronic_Buf.Bayont_Send_State = Electronic_Idle;
									 Electronic_Buf.Bayonet_Communicate_Error = 1;
//									g_error_state.electric_accessories_eh_014 = 1;
								}
						  }
							if(Electronic_Buf.Bracket_Status == 1)
							{
								Task_AccessBaylet(Electronic_Buf.W_Bracket_Buf.Electronic_Data,0,RS485_Cmd_Accessories,0);
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
									Electronic_Buf.Bracket_Times_connect++;
								}
								if( Electronic_Buf.Bracket_Times_connect == 10 )//10S
								{
									 Electronic_Buf.Bracket_Times_connect = 0;
									 Electronic_Buf.Electronic_Connect &= 0xfffe;
									 Electronic_Buf.Bracket_Send_State = Electronic_Idle;
									 g_error_state.electric_support_eh_013 = 1;
								}
							}
						}
						switch(p_decode->access_t.type)
						{
							case RS485_Access_Bracket:((rs485_access_t*)(p_ack_data + *p_ack_offset))->access_state.state = Electronic_Buf.Electronic_Connect&1?1:0;break;
							case RS485_Access_Fresnel:((rs485_access_t*)(p_ack_data + *p_ack_offset))->access_state.state = (Electronic_Buf.Electronic_Connect>>1)&1?1:0;break;
							case RS485_Access_20_Reflector:((rs485_access_t*)(p_ack_data + *p_ack_offset))->access_state.state = (Electronic_Buf.Electronic_Connect>>2)&1?1:0;break;
							case RS485_Access_30_Reflector:((rs485_access_t*)(p_ack_data + *p_ack_offset))->access_state.state = (Electronic_Buf.Electronic_Connect>>3)&1?1:0;break;
							case RS485_Access_50_Reflector:((rs485_access_t*)(p_ack_data + *p_ack_offset))->access_state.state = (Electronic_Buf.Electronic_Connect>>4)&1?1:0;break;
							case RS485_Access_Adapter_Bowl:((rs485_access_t*)(p_ack_data + *p_ack_offset))->access_state.state = (Electronic_Buf.Electronic_Connect>>5)&1?1:0;break;
//							case RS485_Access_Protect_Cover:((rs485_access_t*)(p_ack_data + *p_ack_offset))->access_state.state = (Electronic_Buf.Electronic_Connect>>6)&1?1:0;break;
						}
						*p_ack_offset += sizeof(rs485_access_t);
				}
				break;
				case RS485_Cmd_Version2:{
					data_header.arg_size = sizeof(rs485_version2_t);
					memcpy(p_ack_data + *p_ack_offset, &data_header, sizeof(rs485_cmd_header_t));
					*p_ack_offset += sizeof(rs485_cmd_header_t);
					if(p_decode->version2_t.product_type == RS485_Version2_Bracket)
					{
						if( Electronic_Buf.Bayonet_Status == 1 )
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
						if( Electronic_Buf.Bracket_Status == 1 )
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
					switch(p_decode->version2_t.product_type)
					{
						case RS485_Version2_Bracket:
						((rs485_version2_t*)(p_ack_data + *p_ack_offset))->product_type = RS485_Version2_Bracket;
						((rs485_version2_t*)(p_ack_data + *p_ack_offset))->hard_ver_major = Electronic_Buf.Bracket_Version.hard_ver_major;
						((rs485_version2_t*)(p_ack_data + *p_ack_offset))->hard_ver_minor = Electronic_Buf.Bracket_Version.hard_ver_minor;
						((rs485_version2_t*)(p_ack_data + *p_ack_offset))->hard_ver_revision = Electronic_Buf.Bracket_Version.hard_ver_revision;
						((rs485_version2_t*)(p_ack_data + *p_ack_offset))->soft_ver_major = Electronic_Buf.Bracket_Version.soft_ver_major;
						((rs485_version2_t*)(p_ack_data + *p_ack_offset))->soft_ver_minor = Electronic_Buf.Bracket_Version.soft_ver_minor;//*/led_base_data.ver;
						((rs485_version2_t*)(p_ack_data + *p_ack_offset))->soft_ver_revision = Electronic_Buf.Bracket_Version.soft_ver_revision;
						break;
						case RS485_Version2_Fresnel:
						((rs485_version2_t*)(p_ack_data + *p_ack_offset))->product_type = RS485_Version2_Fresnel;
						((rs485_version2_t*)(p_ack_data + *p_ack_offset))->hard_ver_major = Electronic_Buf.Fresnel_Version.hard_ver_major;
						((rs485_version2_t*)(p_ack_data + *p_ack_offset))->hard_ver_minor = Electronic_Buf.Fresnel_Version.hard_ver_minor;
						((rs485_version2_t*)(p_ack_data + *p_ack_offset))->hard_ver_revision = Electronic_Buf.Fresnel_Version.hard_ver_revision;
						((rs485_version2_t*)(p_ack_data + *p_ack_offset))->soft_ver_major = Electronic_Buf.Fresnel_Version.soft_ver_major;
						((rs485_version2_t*)(p_ack_data + *p_ack_offset))->soft_ver_minor = Electronic_Buf.Fresnel_Version.soft_ver_minor;//*/led_base_data.ver;
						((rs485_version2_t*)(p_ack_data + *p_ack_offset))->soft_ver_revision = Electronic_Buf.Fresnel_Version.soft_ver_revision;
						break;
						default:
							break;
					}
					*p_ack_offset += sizeof(rs485_version2_t);
                }
                break;
				case RS485_Cmd_Factory_RGBWW_Crc:{
					ack_body.ack = RS485_Ack_Err_Command;
					Rs485_Slave_Ack_Package1(&data_header, &ack_body, p_ack_data, p_ack_offset);
                }
                break;
				case RS485_Cmd_Clear_Run_Time:{
					ack_body.ack = RS485_Ack_Err_Command; 
					Rs485_Slave_Ack_Package1(&data_header, &ack_body, p_ack_data, p_ack_offset);
				}
				break;
        default:{
					ack_body.ack = RS485_Ack_Err_Command;
          Rs485_Slave_Ack_Package1(&data_header, &ack_body, p_ack_data, p_ack_offset);
				}break;
            }
        }
        break;
        case RS485_COMMAND_WRITE:{
            switch(data_header.cmd_type){
                case RS485_Cmd_Version:{
                    ack_body.ack = RS485_Ack_Err_Command;
                    Rs485_Slave_Ack_Package1(&data_header, &ack_body, p_ack_data, p_ack_offset);
                }
                break;
                case RS485_Cmd_HSI:{
                    #if 1
                    g_rs485_data.light_mode_para.intensity = p_decode->hsi.intensity;
                    g_rs485_data.light_mode_para.hue = p_decode->hsi.hue;
                    g_rs485_data.light_mode_para.sat = p_decode->hsi.sat;
                    g_rs485_data.light_mode_para.fade = p_decode->hsi.fade;
                    g_rs485_data.light_mode_para.cct = p_decode->hsi.cct;
					g_rs485_data.light_mode_para.duv = p_decode->hsi.duv;
                    g_rs485_data.light_mode_para.pixel_x = p_decode->hsi.pixel_x;
                    g_rs485_data.light_mode_para.pixel_y = p_decode->hsi.pixel_y;
                    #endif
                    g_rs485_data.new_command.light_mode = 1;
                    g_rs485_data.light_mode_state.command_1 = data_header.cmd_type;
                    ack_body.ack = RS485_Ack_Ok;
                    Rs485_Slave_Ack_Package1(&data_header, &ack_body, p_ack_data, p_ack_offset);
                }
                break;
                case RS485_Cmd_CCT:{
                    #if 1
                    g_rs485_data.light_mode_para.intensity = p_decode->cct.intensity;
                    g_rs485_data.light_mode_para.cct = p_decode->cct.cct;
                    g_rs485_data.light_mode_para.duv = p_decode->cct.duv;
                    g_rs485_data.light_mode_para.fade = p_decode->cct.fade;
                    g_rs485_data.light_mode_para.pixel_x = p_decode->cct.pixel_x;
                    g_rs485_data.light_mode_para.pixel_y = p_decode->cct.pixel_y;
                    #endif
                    g_rs485_data.new_command.light_mode = 1;
                    g_rs485_data.light_mode_state.command_1 = data_header.cmd_type;
                    ack_body.ack = RS485_Ack_Ok;
                    Rs485_Slave_Ack_Package1(&data_header, &ack_body, p_ack_data, p_ack_offset);
                }
                break;
                case RS485_Cmd_GEL:{
                    #if 1
                    g_rs485_data.light_mode_para.intensity = p_decode->gel.intensity;
                    g_rs485_data.light_mode_para.cct = p_decode->gel.cct;
                    g_rs485_data.light_mode_para.origin = p_decode->gel.origin;
                    g_rs485_data.light_mode_para.series = p_decode->gel.series;
                    g_rs485_data.light_mode_para.color = p_decode->gel.color;
                    g_rs485_data.light_mode_para.fade = p_decode->gel.fade;
                    g_rs485_data.light_mode_para.pixel_x = p_decode->gel.pixel_x;
                    g_rs485_data.light_mode_para.pixel_y = p_decode->gel.pixel_y;
                    #endif
                    g_rs485_data.new_command.light_mode = 1;
                    g_rs485_data.light_mode_state.command_1 = data_header.cmd_type;
                    ack_body.ack = RS485_Ack_Ok;
                    Rs485_Slave_Ack_Package1(&data_header, &ack_body, p_ack_data, p_ack_offset);
                }
                break;
                case RS485_Cmd_RGB:{
                    #if 1
                    g_rs485_data.light_mode_para.intensity = p_decode->rgb.intensity;
                    g_rs485_data.light_mode_para.red_ratio = p_decode->rgb.red_ratio;
                    g_rs485_data.light_mode_para.green_ratio = p_decode->rgb.green_ratio;
                    g_rs485_data.light_mode_para.blue_ratio = p_decode->rgb.blue_ratio;
                    g_rs485_data.light_mode_para.fade = p_decode->rgb.fade;
                    g_rs485_data.light_mode_para.pixel_x = p_decode->rgb.pixel_x;
                    g_rs485_data.light_mode_para.pixel_y = p_decode->rgb.pixel_y;
                    #endif
                    g_rs485_data.new_command.light_mode = 1;
                    g_rs485_data.light_mode_state.command_1 = data_header.cmd_type;
                    ack_body.ack = RS485_Ack_Ok;
                    Rs485_Slave_Ack_Package1(&data_header, &ack_body, p_ack_data, p_ack_offset);
                }
                break;
                case RS485_Cmd_XY_Coordinate:{
                    #if 1
                    g_rs485_data.light_mode_para.intensity = p_decode->xy_coord.intensity;
                    g_rs485_data.light_mode_para.x = p_decode->xy_coord.x;
                    g_rs485_data.light_mode_para.y = p_decode->xy_coord.y;
                    g_rs485_data.light_mode_para.fade = p_decode->xy_coord.fade;
                    g_rs485_data.light_mode_para.pixel_x = p_decode->xy_coord.pixel_x;
                    g_rs485_data.light_mode_para.pixel_y = p_decode->xy_coord.pixel_y;
                    #endif
                    g_rs485_data.new_command.light_mode = 1;
                    g_rs485_data.light_mode_state.command_1 = data_header.cmd_type;
                    ack_body.ack = RS485_Ack_Ok;
                    Rs485_Slave_Ack_Package1(&data_header, &ack_body, p_ack_data, p_ack_offset);
                }
                break;
                case RS485_Cmd_Dim_Frq:{
                    #if 1
                    g_rs485_data.dimming_frq.adjust_val = p_decode->dimming_frq.adjust_val;
                    #endif
                    g_rs485_data.new_command.frq = 1;
                    ack_body.ack = RS485_Ack_Ok;
                    Rs485_Slave_Ack_Package1(&data_header, &ack_body, p_ack_data, p_ack_offset);
                }
                break;
                case RS485_Cmd_Sys_FX:{
                    g_rs485_data.fx_restart = 0;/*光效正常运行*/
                    g_rs485_data.new_command.light_mode = 1;
                    if(RS485_FX_I_Am_Here != p_decode->sys_fx.type){
                        g_rs485_data.light_mode_state.command_1 = data_header.cmd_type;
                    }
                    switch(p_decode->sys_fx.type){
                        case RS485_FX_ClubLights:{
                            #if 1
                            g_rs485_data.fx_mode_para.intensity = p_decode->sys_fx.arg.club_lights.intensity;
                            g_rs485_data.fx_mode_para.frq = p_decode->sys_fx.arg.club_lights.frq;
                            g_rs485_data.fx_mode_para.color_num = p_decode->sys_fx.arg.club_lights.color;
                            #endif
                            g_rs485_data.light_mode_state.command_2 = p_decode->sys_fx.type;
                            ack_body.ack = RS485_Ack_Ok;
                            Rs485_Slave_Ack_Package1(&data_header, &ack_body, p_ack_data, p_ack_offset);
                        }
                        break;
                        case RS485_FX_Paparazzi:{
                            #if 1
                            g_rs485_data.fx_mode_para.intensity = p_decode->sys_fx.arg.paparazzi.intensity;
                            g_rs485_data.fx_mode_para.frq = p_decode->sys_fx.arg.paparazzi.frq;
                            g_rs485_data.fx_mode_para.cct = p_decode->sys_fx.arg.paparazzi.cct;
                            g_rs485_data.fx_mode_para.gm = p_decode->sys_fx.arg.paparazzi.gm;
                            #endif
                            g_rs485_data.light_mode_state.command_2 = p_decode->sys_fx.type;
                            ack_body.ack = RS485_Ack_Ok;
                            Rs485_Slave_Ack_Package1(&data_header, &ack_body, p_ack_data, p_ack_offset);
                        }
                        break;
                        case RS485_FX_Lightning:{
                            #if 1
                            g_rs485_data.fx_mode_para.intensity = p_decode->sys_fx.arg.lightning.intensity;
                            g_rs485_data.fx_mode_para.frq = p_decode->sys_fx.arg.lightning.frq;
                            g_rs485_data.fx_mode_para.cct = p_decode->sys_fx.arg.lightning.cct;
                            g_rs485_data.fx_mode_para.gm = p_decode->sys_fx.arg.lightning.gm;
                            g_rs485_data.fx_mode_para.trigger = p_decode->sys_fx.arg.lightning.trigger;
                            g_rs485_data.fx_mode_para.spd = p_decode->sys_fx.arg.lightning.spd;
                            #endif
                            g_rs485_data.light_mode_state.command_2 = p_decode->sys_fx.type;
                            ack_body.ack = RS485_Ack_Ok;
                            Rs485_Slave_Ack_Package1(&data_header, &ack_body, p_ack_data, p_ack_offset);
                        }
                        break;
                        case RS485_FX_TV:{
                            #if 1
                            g_rs485_data.fx_mode_para.intensity = p_decode->sys_fx.arg.tv.intensity;
                            g_rs485_data.fx_mode_para.frq = p_decode->sys_fx.arg.tv.frq;
                            g_rs485_data.fx_mode_para.cct = p_decode->sys_fx.arg.tv.cct;
                            #endif
                            g_rs485_data.light_mode_state.command_2 = p_decode->sys_fx.type;
                            ack_body.ack = RS485_Ack_Ok;
                            Rs485_Slave_Ack_Package1(&data_header, &ack_body, p_ack_data, p_ack_offset);
                        }
                        break;
                        case RS485_FX_Candle:{
                            #if 1
                            g_rs485_data.fx_mode_para.intensity = p_decode->sys_fx.arg.candle.intensity;
                            g_rs485_data.fx_mode_para.frq = p_decode->sys_fx.arg.candle.frq;
                            g_rs485_data.fx_mode_para.cct = p_decode->sys_fx.arg.candle.cct;
                            #endif
                            g_rs485_data.light_mode_state.command_2 = p_decode->sys_fx.type;
                            ack_body.ack = RS485_Ack_Ok;
                            Rs485_Slave_Ack_Package1(&data_header, &ack_body, p_ack_data, p_ack_offset);
                        }
                        break;
                        case RS485_FX_Fire:{
                            #if 1
                            g_rs485_data.fx_mode_para.intensity = p_decode->sys_fx.arg.fire.intensity;
                            g_rs485_data.fx_mode_para.frq = p_decode->sys_fx.arg.fire.frq;
                            g_rs485_data.fx_mode_para.cct = p_decode->sys_fx.arg.fire.cct;
                            #endif
                            g_rs485_data.light_mode_state.command_2 = p_decode->sys_fx.type;
                            ack_body.ack = RS485_Ack_Ok;
                            Rs485_Slave_Ack_Package1(&data_header, &ack_body, p_ack_data, p_ack_offset);
                        }
                        break;
                        case RS485_FX_Strobe:{
                            ack_body.ack = RS485_Ack_Ok;
                            #if 1
                            g_rs485_data.fx_mode_para.frq = p_decode->sys_fx.arg.strobe.frq;
                            g_rs485_data.fx_mode_para.mode = p_decode->sys_fx.arg.strobe.mode;
                            switch(p_decode->sys_fx.arg.strobe.mode){
                                case RS485_FX_Mode_CCT:{
                                    g_rs485_data.fx_mode_para.intensity = p_decode->sys_fx.arg.strobe.mode_arg.cct.intensity;
                                    g_rs485_data.fx_mode_para.cct = p_decode->sys_fx.arg.strobe.mode_arg.cct.cct;
                                    g_rs485_data.fx_mode_para.gm = p_decode->sys_fx.arg.strobe.mode_arg.cct.gm;
                                }
                                break;
                                case RS485_FX_Mode_HSI:{
                                    g_rs485_data.fx_mode_para.intensity = p_decode->sys_fx.arg.strobe.mode_arg.hsi.intensity;
                                    g_rs485_data.fx_mode_para.hue = p_decode->sys_fx.arg.strobe.mode_arg.hsi.hue;
                                    g_rs485_data.fx_mode_para.sat = p_decode->sys_fx.arg.strobe.mode_arg.hsi.sat;
                                }
                                break;
                                case RS485_FX_Mode_GEL:{
                                    g_rs485_data.fx_mode_para.intensity = p_decode->sys_fx.arg.strobe.mode_arg.gel.intensity;
                                    g_rs485_data.fx_mode_para.cct = p_decode->sys_fx.arg.strobe.mode_arg.gel.cct;
                                    g_rs485_data.fx_mode_para.origin = p_decode->sys_fx.arg.strobe.mode_arg.gel.origin;
                                    g_rs485_data.fx_mode_para.series = p_decode->sys_fx.arg.strobe.mode_arg.gel.series;
                                    g_rs485_data.fx_mode_para.color = p_decode->sys_fx.arg.strobe.mode_arg.gel.color;
                                }
                                break;
                                case RS485_FX_Mode_XY:{
                                    g_rs485_data.fx_mode_para.intensity = p_decode->sys_fx.arg.strobe.mode_arg.xy.intensity;
                                    g_rs485_data.fx_mode_para.x = p_decode->sys_fx.arg.strobe.mode_arg.xy.x;
                                    g_rs485_data.fx_mode_para.y = p_decode->sys_fx.arg.strobe.mode_arg.xy.y;
                                }
                                break;
                                case RS485_FX_Mode_Source:{
                                    g_rs485_data.fx_mode_para.intensity = p_decode->sys_fx.arg.strobe.mode_arg.source.intensity;
                                    g_rs485_data.fx_mode_para.type = p_decode->sys_fx.arg.strobe.mode_arg.source.type;
                                }
                                break;
								case RS485_FX_Mode_RGB:{
                                    g_rs485_data.fx_mode_para.intensity = p_decode->sys_fx.arg.strobe.mode_arg.rgb.intensity;
                                    g_rs485_data.fx_mode_para.red_ratio = p_decode->sys_fx.arg.strobe.mode_arg.rgb.red_ratio;
									g_rs485_data.fx_mode_para.green_ratio = p_decode->sys_fx.arg.strobe.mode_arg.rgb.green_ratio;
									g_rs485_data.fx_mode_para.blue_ratio = p_decode->sys_fx.arg.strobe.mode_arg.rgb.blue_ratio;
                                }
                                break;
                                default:{
                                    ack_body.ack = RS485_Ack_Err_Command;
                                }
                                break;
                            }
                            #endif
                            g_rs485_data.light_mode_state.command_2 = p_decode->sys_fx.type;
                            Rs485_Slave_Ack_Package1(&data_header, &ack_body, p_ack_data, p_ack_offset);
                        }
                        break;
                        case RS485_FX_Explosion:{
                            ack_body.ack = RS485_Ack_Ok;
                            #if 1
                            g_rs485_data.fx_mode_para.frq = p_decode->sys_fx.arg.explosion.frq;
                            g_rs485_data.fx_mode_para.trigger = p_decode->sys_fx.arg.explosion.trigger;
                            g_rs485_data.fx_mode_para.mode = p_decode->sys_fx.arg.explosion.mode;
                            switch(p_decode->sys_fx.arg.explosion.mode){
                                case RS485_FX_Mode_CCT:{
                                    g_rs485_data.fx_mode_para.intensity = p_decode->sys_fx.arg.explosion.mode_arg.cct.intensity;
                                    g_rs485_data.fx_mode_para.cct = p_decode->sys_fx.arg.explosion.mode_arg.cct.cct;
                                    g_rs485_data.fx_mode_para.gm = p_decode->sys_fx.arg.explosion.mode_arg.cct.gm;
                                }
                                break;
                                case RS485_FX_Mode_HSI:{
                                    g_rs485_data.fx_mode_para.intensity = p_decode->sys_fx.arg.explosion.mode_arg.hsi.intensity;
                                    g_rs485_data.fx_mode_para.hue = p_decode->sys_fx.arg.explosion.mode_arg.hsi.hue;
                                    g_rs485_data.fx_mode_para.sat = p_decode->sys_fx.arg.explosion.mode_arg.hsi.sat;
                                }
                                break;
                                case RS485_FX_Mode_GEL:{
                                    g_rs485_data.fx_mode_para.intensity = p_decode->sys_fx.arg.explosion.mode_arg.gel.intensity;
                                    g_rs485_data.fx_mode_para.cct = p_decode->sys_fx.arg.explosion.mode_arg.gel.cct;
                                    g_rs485_data.fx_mode_para.origin = p_decode->sys_fx.arg.explosion.mode_arg.gel.origin;
                                    g_rs485_data.fx_mode_para.series = p_decode->sys_fx.arg.explosion.mode_arg.gel.series;
                                    g_rs485_data.fx_mode_para.color = p_decode->sys_fx.arg.explosion.mode_arg.gel.color;
                                }
                                break;
                                case RS485_FX_Mode_XY:{
                                    g_rs485_data.fx_mode_para.intensity = p_decode->sys_fx.arg.explosion.mode_arg.xy.intensity;
                                    g_rs485_data.fx_mode_para.x = p_decode->sys_fx.arg.explosion.mode_arg.xy.x;
                                    g_rs485_data.fx_mode_para.y = p_decode->sys_fx.arg.explosion.mode_arg.xy.y;
                                }
                                break;
                                case RS485_FX_Mode_Source:{
                                    g_rs485_data.fx_mode_para.intensity = p_decode->sys_fx.arg.explosion.mode_arg.source.intensity;
                                    g_rs485_data.fx_mode_para.type = p_decode->sys_fx.arg.explosion.mode_arg.source.type;
                                }
                                break;
								case RS485_FX_Mode_RGB:{
                                    g_rs485_data.fx_mode_para.intensity = p_decode->sys_fx.arg.explosion.mode_arg.rgb.intensity;
                                    g_rs485_data.fx_mode_para.red_ratio = p_decode->sys_fx.arg.explosion.mode_arg.rgb.red_ratio;
									g_rs485_data.fx_mode_para.green_ratio = p_decode->sys_fx.arg.explosion.mode_arg.rgb.green_ratio;
									g_rs485_data.fx_mode_para.blue_ratio = p_decode->sys_fx.arg.explosion.mode_arg.rgb.blue_ratio;
                                }
								break;
                                default:{
                                    ack_body.ack = RS485_Ack_Err_Command;
                                }
                                break;
                            }
                            #endif
                            g_rs485_data.light_mode_state.command_2 = p_decode->sys_fx.type;
                            Rs485_Slave_Ack_Package1(&data_header, &ack_body, p_ack_data, p_ack_offset);
                        }
                        break;
                        case RS485_FX_FaultBulb:{
                            ack_body.ack = RS485_Ack_Ok;
                            #if 1
                            g_rs485_data.fx_mode_para.frq = p_decode->sys_fx.arg.fault_bulb.frq;
                            g_rs485_data.fx_mode_para.spd = p_decode->sys_fx.arg.fault_bulb.spd;
                            g_rs485_data.fx_mode_para.mode = p_decode->sys_fx.arg.fault_bulb.mode;
                            switch(p_decode->sys_fx.arg.fault_bulb.mode){
                                case RS485_FX_Mode_CCT:{
                                    g_rs485_data.fx_mode_para.intensity = p_decode->sys_fx.arg.fault_bulb.mode_arg.cct.intensity;
                                    g_rs485_data.fx_mode_para.cct = p_decode->sys_fx.arg.fault_bulb.mode_arg.cct.cct;
                                    g_rs485_data.fx_mode_para.gm = p_decode->sys_fx.arg.fault_bulb.mode_arg.cct.gm;
                                }
                                break;
                                case RS485_FX_Mode_HSI:{
                                    g_rs485_data.fx_mode_para.intensity = p_decode->sys_fx.arg.fault_bulb.mode_arg.hsi.intensity;
                                    g_rs485_data.fx_mode_para.hue = p_decode->sys_fx.arg.fault_bulb.mode_arg.hsi.hue;
                                    g_rs485_data.fx_mode_para.sat = p_decode->sys_fx.arg.fault_bulb.mode_arg.hsi.sat;
                                }
                                break;
                                case RS485_FX_Mode_GEL:{
                                    g_rs485_data.fx_mode_para.intensity = p_decode->sys_fx.arg.fault_bulb.mode_arg.gel.intensity;
                                    g_rs485_data.fx_mode_para.cct = p_decode->sys_fx.arg.fault_bulb.mode_arg.gel.cct;
                                    g_rs485_data.fx_mode_para.origin = p_decode->sys_fx.arg.fault_bulb.mode_arg.gel.origin;
                                    g_rs485_data.fx_mode_para.series = p_decode->sys_fx.arg.fault_bulb.mode_arg.gel.series;
                                    g_rs485_data.fx_mode_para.color = p_decode->sys_fx.arg.fault_bulb.mode_arg.gel.color;
                                }
                                break;
                                case RS485_FX_Mode_XY:{
                                    g_rs485_data.fx_mode_para.intensity = p_decode->sys_fx.arg.fault_bulb.mode_arg.xy.intensity;
                                    g_rs485_data.fx_mode_para.x = p_decode->sys_fx.arg.fault_bulb.mode_arg.xy.x;
                                    g_rs485_data.fx_mode_para.y = p_decode->sys_fx.arg.fault_bulb.mode_arg.xy.y;
                                }
                                break;
                                case RS485_FX_Mode_Source:{
                                    g_rs485_data.fx_mode_para.intensity = p_decode->sys_fx.arg.fault_bulb.mode_arg.source.intensity;
                                    g_rs485_data.fx_mode_para.type = p_decode->sys_fx.arg.fault_bulb.mode_arg.source.type;
                                }
                                break;
								case RS485_FX_Mode_RGB:{
                                    g_rs485_data.fx_mode_para.intensity = p_decode->sys_fx.arg.fault_bulb.mode_arg.rgb.intensity;
                                    g_rs485_data.fx_mode_para.red_ratio = p_decode->sys_fx.arg.fault_bulb.mode_arg.rgb.red_ratio;
									g_rs485_data.fx_mode_para.green_ratio = p_decode->sys_fx.arg.fault_bulb.mode_arg.rgb.green_ratio;
									g_rs485_data.fx_mode_para.blue_ratio = p_decode->sys_fx.arg.fault_bulb.mode_arg.rgb.blue_ratio;
                                }
								break;
                                default:{
                                    ack_body.ack = RS485_Ack_Err_Command;
                                }
                                break;
                            }
                            #endif
                            g_rs485_data.light_mode_state.command_2 = p_decode->sys_fx.type;
                            Rs485_Slave_Ack_Package1(&data_header, &ack_body, p_ack_data, p_ack_offset);
                        }
                        break;
                        case RS485_FX_Pulsing:{
                            ack_body.ack = RS485_Ack_Ok;
                            #if 1
                            g_rs485_data.fx_mode_para.frq = p_decode->sys_fx.arg.pulsing.frq;
                            g_rs485_data.fx_mode_para.spd = p_decode->sys_fx.arg.pulsing.spd;
                            g_rs485_data.fx_mode_para.mode = p_decode->sys_fx.arg.pulsing.mode;
                            switch(p_decode->sys_fx.arg.pulsing.mode){
                                case RS485_FX_Mode_CCT:{
                                    g_rs485_data.fx_mode_para.intensity = p_decode->sys_fx.arg.pulsing.mode_arg.cct.intensity;
                                    g_rs485_data.fx_mode_para.cct = p_decode->sys_fx.arg.pulsing.mode_arg.cct.cct;
                                    g_rs485_data.fx_mode_para.gm = p_decode->sys_fx.arg.pulsing.mode_arg.cct.gm;
                                }
                                break;
                                case RS485_FX_Mode_HSI:{
                                    g_rs485_data.fx_mode_para.intensity = p_decode->sys_fx.arg.pulsing.mode_arg.hsi.intensity;
                                    g_rs485_data.fx_mode_para.hue = p_decode->sys_fx.arg.pulsing.mode_arg.hsi.hue;
                                    g_rs485_data.fx_mode_para.sat = p_decode->sys_fx.arg.pulsing.mode_arg.hsi.sat;
                                }
                                break;
                                case RS485_FX_Mode_GEL:{
                                    g_rs485_data.fx_mode_para.intensity = p_decode->sys_fx.arg.pulsing.mode_arg.gel.intensity;
                                    g_rs485_data.fx_mode_para.cct = p_decode->sys_fx.arg.pulsing.mode_arg.gel.cct;
                                    g_rs485_data.fx_mode_para.origin = p_decode->sys_fx.arg.pulsing.mode_arg.gel.origin;
                                    g_rs485_data.fx_mode_para.series = p_decode->sys_fx.arg.pulsing.mode_arg.gel.series;
                                    g_rs485_data.fx_mode_para.color = p_decode->sys_fx.arg.pulsing.mode_arg.gel.color;
                                }
                                break;
                                case RS485_FX_Mode_XY:{
                                    g_rs485_data.fx_mode_para.intensity = p_decode->sys_fx.arg.pulsing.mode_arg.xy.intensity;
                                    g_rs485_data.fx_mode_para.x = p_decode->sys_fx.arg.pulsing.mode_arg.xy.x;
                                    g_rs485_data.fx_mode_para.y = p_decode->sys_fx.arg.pulsing.mode_arg.xy.y;
                                }
                                break;
                                case RS485_FX_Mode_Source:{
                                    g_rs485_data.fx_mode_para.intensity = p_decode->sys_fx.arg.pulsing.mode_arg.source.intensity;
                                    g_rs485_data.fx_mode_para.type = p_decode->sys_fx.arg.pulsing.mode_arg.source.type;
                                }
                                break;
								case RS485_FX_Mode_RGB:{
                                    g_rs485_data.fx_mode_para.intensity = p_decode->sys_fx.arg.pulsing.mode_arg.rgb.intensity;
                                    g_rs485_data.fx_mode_para.red_ratio = p_decode->sys_fx.arg.pulsing.mode_arg.rgb.red_ratio;
									g_rs485_data.fx_mode_para.green_ratio = p_decode->sys_fx.arg.pulsing.mode_arg.rgb.green_ratio;
									g_rs485_data.fx_mode_para.blue_ratio = p_decode->sys_fx.arg.pulsing.mode_arg.rgb.blue_ratio;
                                }
								break;
                                default:{
                                    ack_body.ack = RS485_Ack_Err_Command;
                                }
                                break;
                            }
                            #endif
                            g_rs485_data.light_mode_state.command_2 = p_decode->sys_fx.type;
                            Rs485_Slave_Ack_Package1(&data_header, &ack_body, p_ack_data, p_ack_offset);
                        }
                        break;
                        case RS485_FX_Welding:{
                            ack_body.ack = RS485_Ack_Ok;
                            #if 1
                            g_rs485_data.fx_mode_para.frq = p_decode->sys_fx.arg.welding.frq;
                            g_rs485_data.fx_mode_para.min_int = p_decode->sys_fx.arg.welding.min_int;
                            g_rs485_data.fx_mode_para.mode = p_decode->sys_fx.arg.welding.mode;
                            switch(p_decode->sys_fx.arg.welding.mode){
                                case RS485_FX_Mode_CCT:{
                                    g_rs485_data.fx_mode_para.intensity = p_decode->sys_fx.arg.welding.mode_arg.cct.intensity;
                                    g_rs485_data.fx_mode_para.cct = p_decode->sys_fx.arg.welding.mode_arg.cct.cct;
                                    g_rs485_data.fx_mode_para.gm = p_decode->sys_fx.arg.welding.mode_arg.cct.gm;
                                }
                                break;
                                case RS485_FX_Mode_HSI:{
                                    g_rs485_data.fx_mode_para.intensity = p_decode->sys_fx.arg.welding.mode_arg.hsi.intensity;
                                    g_rs485_data.fx_mode_para.hue = p_decode->sys_fx.arg.welding.mode_arg.hsi.hue;
                                    g_rs485_data.fx_mode_para.sat = p_decode->sys_fx.arg.welding.mode_arg.hsi.sat;
                                }
                                break;
                                case RS485_FX_Mode_GEL:{
                                    g_rs485_data.fx_mode_para.intensity = p_decode->sys_fx.arg.welding.mode_arg.gel.intensity;
                                    g_rs485_data.fx_mode_para.cct = p_decode->sys_fx.arg.welding.mode_arg.gel.cct;
                                    g_rs485_data.fx_mode_para.origin = p_decode->sys_fx.arg.welding.mode_arg.gel.origin;
                                    g_rs485_data.fx_mode_para.series = p_decode->sys_fx.arg.welding.mode_arg.gel.series;
                                    g_rs485_data.fx_mode_para.color = p_decode->sys_fx.arg.welding.mode_arg.gel.color;
                                }
                                break;
                                case RS485_FX_Mode_XY:{
                                    g_rs485_data.fx_mode_para.intensity = p_decode->sys_fx.arg.welding.mode_arg.xy.intensity;
                                    g_rs485_data.fx_mode_para.x = p_decode->sys_fx.arg.welding.mode_arg.xy.x;
                                    g_rs485_data.fx_mode_para.y = p_decode->sys_fx.arg.welding.mode_arg.xy.y;
                                }
                                break;
                                case RS485_FX_Mode_Source:{
                                    g_rs485_data.fx_mode_para.intensity = p_decode->sys_fx.arg.welding.mode_arg.source.intensity;
                                    g_rs485_data.fx_mode_para.type = p_decode->sys_fx.arg.welding.mode_arg.source.type;
                                }
                                break;
								case RS485_FX_Mode_RGB:{
                                    g_rs485_data.fx_mode_para.intensity = p_decode->sys_fx.arg.welding.mode_arg.rgb.intensity;
                                    g_rs485_data.fx_mode_para.red_ratio = p_decode->sys_fx.arg.welding.mode_arg.rgb.red_ratio;
									g_rs485_data.fx_mode_para.green_ratio = p_decode->sys_fx.arg.welding.mode_arg.rgb.green_ratio;
									g_rs485_data.fx_mode_para.blue_ratio = p_decode->sys_fx.arg.welding.mode_arg.rgb.blue_ratio;
                                }
								break;
                                default:{
                                    ack_body.ack = RS485_Ack_Err_Command;
                                }
                                break;
                            }
                            #endif
                            g_rs485_data.light_mode_state.command_2 = p_decode->sys_fx.type;
                            Rs485_Slave_Ack_Package1(&data_header, &ack_body, p_ack_data, p_ack_offset);
                        }
                        break;
                        case RS485_FX_CopCar:{
                            #if 1
                            g_rs485_data.fx_mode_para.intensity = p_decode->sys_fx.arg.cop_car.intensity;
                            g_rs485_data.fx_mode_para.frq = p_decode->sys_fx.arg.cop_car.frq;
                            g_rs485_data.fx_mode_para.color_num = p_decode->sys_fx.arg.cop_car.color;
                            #endif
                            g_rs485_data.light_mode_state.command_2 = p_decode->sys_fx.type;
                            ack_body.ack = RS485_Ack_Ok;
                            Rs485_Slave_Ack_Package1(&data_header, &ack_body, p_ack_data, p_ack_offset);
                        }
                        break;
                        case RS485_FX_ColorChase:{
                            #if 1
                            g_rs485_data.fx_mode_para.intensity = p_decode->sys_fx.arg.color_chase.intensity;
                            g_rs485_data.fx_mode_para.frq = p_decode->sys_fx.arg.color_chase.frq;
                            g_rs485_data.fx_mode_para.sat = p_decode->sys_fx.arg.color_chase.sat;
                            #endif
                            g_rs485_data.light_mode_state.command_2 = p_decode->sys_fx.type;
                            ack_body.ack = RS485_Ack_Ok;
                            Rs485_Slave_Ack_Package1(&data_header, &ack_body, p_ack_data, p_ack_offset);
                        }
                        break;
                        case RS485_FX_PartyLights:{
                            #if 1
                            g_rs485_data.fx_mode_para.intensity = p_decode->sys_fx.arg.party_lights.intensity;
                            g_rs485_data.fx_mode_para.frq = p_decode->sys_fx.arg.party_lights.frq;
                            g_rs485_data.fx_mode_para.sat = p_decode->sys_fx.arg.party_lights.sat;
                            #endif
                            g_rs485_data.light_mode_state.command_2 = p_decode->sys_fx.type;
                            ack_body.ack = RS485_Ack_Ok;
                            Rs485_Slave_Ack_Package1(&data_header, &ack_body, p_ack_data, p_ack_offset);
                        }
                        break;
                        case RS485_FX_Fireworks:{
                            #if 1
                            g_rs485_data.fx_mode_para.intensity = p_decode->sys_fx.arg.fireworks.intensity;
                            g_rs485_data.fx_mode_para.frq = p_decode->sys_fx.arg.fireworks.frq;
                            g_rs485_data.fx_mode_para.type = p_decode->sys_fx.arg.fireworks.type;
                            #endif
                            g_rs485_data.light_mode_state.command_2 = p_decode->sys_fx.type;
                            ack_body.ack = RS485_Ack_Ok;
                            Rs485_Slave_Ack_Package1(&data_header, &ack_body, p_ack_data, p_ack_offset);
                        }
                        break;
                        case RS485_FX_Effect_Off:{
                            g_rs485_data.light_mode_state.command_2 = p_decode->sys_fx.type;
                            ack_body.ack = RS485_Ack_Ok;
                            Rs485_Slave_Ack_Package1(&data_header, &ack_body, p_ack_data, p_ack_offset);
                        }
                        break;
                        case RS485_FX_I_Am_Here:{
                            #if 1
                            g_rs485_data.i_am_here.state = p_decode->sys_fx.arg.i_am_here.state;
                            #endif
                            g_rs485_data.new_command.light_mode = 0;/*确保寻灯光效能继续运行*/
                            g_rs485_data.new_command.here_fx = 1;
                            ack_body.ack = RS485_Ack_Ok;
                            Rs485_Slave_Ack_Package1(&data_header, &ack_body, p_ack_data, p_ack_offset);
                        }
                        break;
                        case RS485_FX_Null:{
                            g_rs485_data.light_mode_state.command_2 = p_decode->sys_fx.type;
                            ack_body.ack = RS485_Ack_Ok;
                            Rs485_Slave_Ack_Package1(&data_header, &ack_body, p_ack_data, p_ack_offset);
                        }
                        break;
                        default:{
							ack_body.ack = RS485_Ack_Err_Command;
                            Rs485_Slave_Ack_Package1(&data_header, &ack_body, p_ack_data, p_ack_offset);
						}
                        break;
                    }
                }
                break;
                case RS485_Cmd_Dimming_Curve:{
                    #if 1
                    g_rs485_data.dimming_curve.curve = p_decode->dimming_curve.curve;
                    #endif
                    g_rs485_data.new_command.dim = 1;
                    ack_body.ack = RS485_Ack_Ok;
                    Rs485_Slave_Ack_Package1(&data_header, &ack_body, p_ack_data, p_ack_offset);
                }
                break;
                case RS485_Cmd_Fan:{
                    #if 1
                    g_rs485_data.fan.mode       = p_decode->fan.mode;
                    g_rs485_data.fan.speed      = p_decode->fan.speed;
                    g_rs485_data.fan.fan_index  = p_decode->fan.fan_index;
                    g_rs485_data.fan.err_msg    = p_decode->fan.err_msg;
                    #endif
                    g_rs485_data.new_command.fan = 1;
                    ack_body.ack = RS485_Ack_Ok;
                    Rs485_Slave_Ack_Package1(&data_header, &ack_body, p_ack_data, p_ack_offset);
                }
                break;
                case RS485_Cmd_Power_Suppy:{
                    #if 0
                    p_decode->Power_Suppy.Power_Type;
                    p_decode->Power_Suppy.Input_Voltage;
                    p_decode->Power_Suppy.Input_Current;
                    #endif
                    ack_body.ack = RS485_Ack_Ok;
                    Rs485_Slave_Ack_Package1(&data_header, &ack_body, p_ack_data, p_ack_offset);
                }
                break;
                case RS485_Cmd_Battery_State:{
                    #if 0
                    p_decode->Battery_State.Battery_Sel;
                    p_decode->Battery_State.Battery_Voltage;
                    p_decode->Battery_State.Battery_Current;
                    p_decode->Battery_State.Battery_Type;
                    p_decode->Battery_State.Battery_Time_Remain;
                    p_decode->Battery_State.Battery_Power_Remain;
                    #endif
                    ack_body.ack = RS485_Ack_Ok;
                    Rs485_Slave_Ack_Package1(&data_header, &ack_body, p_ack_data, p_ack_offset);
                }
                break;
                case RS485_Cmd_Switch:{			
						g_rs485_data.power_switch.state = p_decode->power_switch.state;
						if(get_fan_state()){
							g_rs485_data.new_command.on_off = 1;
							if(Electronic_Buf.Bayonet_Status == 1 || Electronic_Buf.Bracket_Status == 1)
							{
								Electronic_Package_Fuction(Electronic_Buf.W_Bayont_Buf.Electronic_Data,1,Pitch_Roll[RS485_Fresnel_Motor],0);
								#ifdef ENCODE
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
								Electronic_Package_Fuction(Electronic_Buf.W_Bracket_Buf.Electronic_Data,1,Pitch_Roll[RS485_Rotale_Motor],0);
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
								DDL_DelayMS(10);
							}
							#if OLD_VER
								ack_body.ack = 1;
							#else
								ack_body.ack = RS485_Ack_Ok;
							#endif
							Rs485_Slave_Ack_Package1(&data_header, &ack_body, p_ack_data, p_ack_offset);
						}
						else{
							ack_body.ack = RS485_Ack_Err_Busy;
							Rs485_Slave_Ack_Package1(&data_header, &ack_body, p_ack_data, p_ack_offset);
						}
					
                }
                break;
                case RS485_Cmd_FileTransfer:{
                    #if 1
					static uint8_t firm_type = 0;
					uint16_t sent_times = 0;
					uint16_t time = 0;
                    g_rs485_ack_wait.command_type = RS485_Cmd_FileTransfer;
                    g_rs485_ack_wait.delay_time = 0;
                    g_rs485_ack_wait.ack_type = RS485_Ack_Ok;
                    g_rs485_ack_wait.ack_slot = *p_ack_offset + sizeof(rs485_cmd_header_t);/*后面需要重新计算校验和时使用*/
					if( RS485_File_TX_Firmware == p_decode->file_transfer.file_type )
					{
						if(	p_decode->file_transfer.step == RS485_File_TX_Step_Start )
							firm_type = p_decode->file_transfer.step_body.start_arg.firmware_start.type;
						if( firm_type == 3 )
						{
							init_queue(&Bracket_Queue);
							Electronic_Buf.Bracket_Loading_State = 1;
							if(Electronic_Buf.Bracket_Status == 1)
							{
								#ifdef ENCODE
								static uint16_t last_serial3 = 0;
Bracket_sent_again:
								encode_data1 = user_plaintext_encryption(p_data,((rs485_proto_header_t *)(p_data))->msg_size);
								if( encode_data1 != NULL && encode_data1->data_len <= 500 )
								{
									last_serial3 = ((rs485_proto_header_t *)(p_data))->serial_num;
									
									memcpy(data_pin,p_data,11);
									memcpy(data_pin+11,encode_data1->data,encode_data1->data_len);
									hal_rs485_2_usart_send(data_pin,encode_data1->data_len+11);
								}
								#else
								hal_rs485_2_usart_send(p_data,((rs485_proto_header_t*)(p_data))->msg_size);
								#endif
								if(	p_decode->file_transfer.step == RS485_File_TX_Step_Size )
								{
									time = 5000;
									while( Electronic_Buf.RW_Bracket_State != Electronic_Recive && time-- )
									{
										DDL_DelayMS(1);
									}
								}
								else
								{
									time = 500;
									while( Electronic_Buf.RW_Bracket_State != Electronic_Recive && time-- )
									{
										DDL_DelayMS(1);
									}
								}
								if( Electronic_Buf.RW_Bracket_State == Electronic_Recive)
								{
									#ifdef ENCODE
									de_encode_data1 = user_chiphertext_decryption(Electronic_Buf.R_Bracket_Buf.Electronic_Data,Electronic_Buf.R_Bracket_Buf.Electronic_Size);
									if(de_encode_data1 != NULL)
									{
										memcpy(&Electronic_Buf.R_Bracket_Buf.Electronic_Data[11],de_encode_data1,de_encode_data1->data_len);
									}
									if( last_serial3 == ((rs485_proto_header_t*)(Electronic_Buf.R_Bracket_Buf.Electronic_Data))->serial_num )
									{
									#else
									if( ((rs485_proto_header_t*)(Electronic_Buf.R_Bracket_Buf.Electronic_Data))->serial_num == ((rs485_proto_header_t*)(p_data))->serial_num )
									{
									#endif
										if( Electronic_Buf.R_Bracket_Buf.Electronic_Data[15] == RS485_Ack_Ok )
											ack_body.ack = RS485_Ack_Ok;
										else
										{
											ack_body.ack = RS485_Ack_Err_Command;
											Electronic_Buf.Bracket_Loading_State = 0;
										}
									}
									else{
										if(sent_times < 2)
										{
											sent_times++;
											Electronic_Buf.RW_Bracket_State = Electronic_Idle;
											goto Bracket_sent_again;
										}
										else
										{
											ack_body.ack = RS485_Ack_Err_Sequence;
											Electronic_Buf.Bracket_Loading_State = 0;
										}
									}
									Electronic_Buf.RW_Bracket_State = Electronic_Idle;
								}
								else
								{
									ack_body.ack = RS485_Ack_Err_Timeout;
									Electronic_Buf.Bracket_Loading_State = 0;
								}
							}
							else
							{
								ack_body.ack = RS485_Ack_Err_Timeout;
								Electronic_Buf.Bracket_Loading_State = 0;
							}
							if((p_decode->file_transfer.step == RS485_File_TX_Step_CRC && ack_body.ack == RS485_Ack_Ok))// ||	ack_body.ack != RS485_Ack_Ok)
							{
								Electronic_Buf.Bracket_Loading_State = 0;
								if(ack_body.ack == RS485_Ack_Ok)
								{
									init_queue(&Bracket_Queue);
								}
							}
//							else
//							{
//								Electronic_Buf.Bracket_Loading_State = 1;
//							}
						}
						else if( firm_type == 2 )
						{
							init_queue(&Bayont_Queue);
							Electronic_Buf.Bayonet_Loading_State = 1;
							if(Electronic_Buf.Bayonet_Status == 1)
							{
								#ifdef ENCODE
								static uint16_t last_serial2 = 0;
Bayont_sent_again:
								encode_data1 = user_plaintext_encryption(p_data,((rs485_proto_header_t *)(p_data))->msg_size);
								if( encode_data1 != NULL && encode_data1->data_len <= 500 )
								{
									last_serial2 = ((rs485_proto_header_t *)(p_data))->serial_num;
			
									memcpy(data_pin,p_data,11);
									memcpy(data_pin+11,encode_data1->data,encode_data1->data_len);
									hal_rs485_3_usart_send(data_pin,encode_data1->data_len+11);
								}
								#else
								hal_rs485_3_usart_send(p_data,((rs485_proto_header_t*)(p_data))->msg_size);
								#endif
								if(	p_decode->file_transfer.step == RS485_File_TX_Step_Size )
								{
									time = 5000;
									while( Electronic_Buf.RW_Bayont_State != Electronic_Recive && time-- )
									{
										DDL_DelayMS(1);
									}
								}
								else
								{
									time = 500;
									while( Electronic_Buf.RW_Bayont_State != Electronic_Recive && time-- )
									{
										DDL_DelayMS(1);
									}
								}
								if( Electronic_Buf.RW_Bayont_State == Electronic_Recive)
								{
									#ifdef ENCODE
									de_encode_data1 = user_chiphertext_decryption(Electronic_Buf.R_Bayont_Buf.Electronic_Data,Electronic_Buf.R_Bayont_Buf.Electronic_Size);
									if(de_encode_data1 != NULL)
									{
										memcpy(&Electronic_Buf.R_Bayont_Buf.Electronic_Data[11],de_encode_data1,de_encode_data1->data_len);
									}
									if( last_serial2 == ((rs485_proto_header_t*)(Electronic_Buf.R_Bayont_Buf.Electronic_Data))->serial_num )
									{
									#else
									if( ((rs485_proto_header_t*)(Electronic_Buf.R_Bayont_Buf.Electronic_Data))->serial_num == ((rs485_proto_header_t*)(p_data))->serial_num )
									{
									#endif
										if( Electronic_Buf.R_Bayont_Buf.Electronic_Data[15] == RS485_Ack_Ok )
											ack_body.ack = RS485_Ack_Ok;
										else
										{
											ack_body.ack = RS485_Ack_Err_Command;
											Electronic_Buf.Bayonet_Loading_State = 0;
										}
									}
									else{
										if(sent_times < 2)
										{
											sent_times++;
											Electronic_Buf.RW_Bayont_State = Electronic_Idle;
											goto Bayont_sent_again;
										}
										else
										{
											ack_body.ack = RS485_Ack_Err_Sequence;
											Electronic_Buf.Bayonet_Loading_State = 0;
										}
									}
									Electronic_Buf.RW_Bayont_State = Electronic_Idle;
								}
								else
								{
									ack_body.ack = RS485_Ack_Err_Timeout;
									Electronic_Buf.Bayonet_Loading_State = 0;
								}
							}
							else
							{
								ack_body.ack = RS485_Ack_Err_Timeout;
								Electronic_Buf.Bayonet_Loading_State = 0;
							}
							if((p_decode->file_transfer.step == RS485_File_TX_Step_CRC && ack_body.ack == RS485_Ack_Ok))// ||	ack_body.ack != RS485_Ack_Ok)
							{
								Electronic_Buf.Bayonet_Loading_State = 0;
								if(ack_body.ack == RS485_Ack_Ok)
								{
									init_queue(&Bayont_Queue);
								}
							}
//							else
//							{
//								Electronic_Buf.Bayonet_Loading_State = 1;
//							}
						}
						else if( firm_type == 1 )
						{
							Upgrade_Process_Ctr(&p_decode->file_transfer, &g_rs485_ack_wait);
							ack_body.ack = g_rs485_ack_wait.ack_type;
							g_rs485_ack_wait.wait_ack = Get_Upgrade_Start_State;
						}
					}
                    else if(RS485_File_TX_CFX == p_decode->file_transfer.file_type){
                        uint8_t temp_buf[140] = {0};
                        switch(p_decode->file_transfer.step){
                            case RS485_File_TX_Step_Start:{
                                g_rs485_ack_wait.delay_time = 10000;
                                temp_buf[0] = p_decode->file_transfer.step;
                                temp_buf[1] = p_decode->file_transfer.file_type;
                                temp_buf[2] = p_decode->file_transfer.step_body.start_arg.cfx_start.effect_type;
                                temp_buf[3] = p_decode->file_transfer.step_body.start_arg.cfx_start.bank;
															
                                SidusProFile_FileTransfer_Receive(temp_buf, 4);
                               
                                ack_body.ack = g_rs485_ack_wait.ack_type;

                                g_rs485_ack_wait.wait_ack = get_cfx_file_state;
                                #if(1 == GLOBAL_PRINT_ENABLE)
//                                printf("cfr1.\n");
                                #endif
                            }
                            break;
                            case RS485_File_TX_Step_Size:{
                                g_rs485_ack_wait.delay_time = 10000;
                                temp_buf[0] = p_decode->file_transfer.step;
                                ((rs485_file_tx_step_size_arg_t*)(&temp_buf[1]))->file_size = p_decode->file_transfer.step_body.size_arg.file_size;
                                SidusProFile_FileTransfer_Receive(temp_buf, 5);
                                
                                ack_body.ack = g_rs485_ack_wait.ack_type;
                                g_rs485_ack_wait.wait_ack = get_cfx_file_state;
                                #if(1 == GLOBAL_PRINT_ENABLE)
//                                printf("cfr2.\n");
                                #endif
                            }
                            break;
                            case RS485_File_TX_Step_Data:{
                                g_rs485_ack_wait.delay_time = 2000;
                                temp_buf[0] = p_decode->file_transfer.step;
                                temp_buf[1] = p_decode->file_transfer.step_body.data_arg.block_size;
                                *((uint32_t*)(&temp_buf[2])) = p_decode->file_transfer.step_body.data_arg.block_cnt;
                                memcpy(&temp_buf[6], p_decode->file_transfer.step_body.data_arg.block_buf, 128);
                                SidusProFile_FileTransfer_Receive(temp_buf, 134);
                                
                                ack_body.ack = g_rs485_ack_wait.ack_type;
                                g_rs485_ack_wait.wait_ack = get_cfx_file_state;
                                #if(1 == GLOBAL_PRINT_ENABLE)
//                                printf("cfr3.\n");
                                #endif
                            }
                            break;
                            case RS485_File_TX_Step_CRC:{
                                g_rs485_ack_wait.delay_time = 2000;
                                temp_buf[0] = p_decode->file_transfer.step;
                                *((uint32_t*)(&temp_buf[1])) = p_decode->file_transfer.step_body.crc_arg.crc32;
                                SidusProFile_FileTransfer_Receive(temp_buf, 5);
                                
                                ack_body.ack = g_rs485_ack_wait.ack_type;
                                g_rs485_ack_wait.wait_ack = get_cfx_file_state;
                            }
                            break;
                            default:break;
                        }
                    }
                    else{
                        ack_body.ack = RS485_Ack_Err_Command;/*目前没有调试自定义光效的文件升级，只需到此处则回复命令错误*/
                    }
                    #endif
                    Rs485_Slave_Ack_Package1(&data_header, &ack_body, p_ack_data, p_ack_offset);
                }
                break;
                case RS485_Cmd_Temperature_Msg:{
                    ack_body.ack = RS485_Ack_Ok;
                    Rs485_Slave_Ack_Package1(&data_header, &ack_body, p_ack_data, p_ack_offset);
                }
                break;
                case RS485_Cmd_Sys_FX_II:{
                    g_rs485_data.fx_restart = 0;/*光效正常运行*/
                    g_rs485_data.new_command.light_mode = 1;
                    g_rs485_data.light_mode_state.command_1 = data_header.cmd_type;
					switch(p_decode->sys_fx_2.type){
                        case RS485_FX_II_Paparazzi_2:{
                            ack_body.ack = RS485_Ack_Ok;
                            #if 1
                            g_rs485_data.fx_mode_para.state = p_decode->sys_fx_2.arg.paparazzi_2.state;
                            g_rs485_data.fx_mode_para.intensity = p_decode->sys_fx_2.arg.paparazzi_2.intensity;
                            g_rs485_data.fx_mode_para.gap_time = p_decode->sys_fx_2.arg.paparazzi_2.gap_time;
                            g_rs485_data.fx_mode_para.min_gap_time = p_decode->sys_fx_2.arg.paparazzi_2.min_gap_time;
                            g_rs485_data.fx_mode_para.mode = p_decode->sys_fx_2.arg.paparazzi_2.mode;
                            if(1 == set_sys_fx2_light_para(&g_rs485_data.fx_mode_para, 
                                                           &p_decode->sys_fx_2.arg.paparazzi_2.mode_arg)){
                                ack_body.ack = RS485_Ack_Err_Command;
                            }
                            #endif
                            g_rs485_data.light_mode_state.command_2 = p_decode->sys_fx_2.type;
                            Rs485_Slave_Ack_Package1(&data_header, &ack_body, p_ack_data, p_ack_offset);
                        }
                        break;
                        case RS485_FX_II_Lightning_2:{
                            ack_body.ack = RS485_Ack_Ok;
                            #if 1
                            g_rs485_data.fx_mode_para.trigger = (uint8_t)p_decode->sys_fx_2.arg.lightning_2.state;
                            g_rs485_data.fx_mode_para.intensity = p_decode->sys_fx_2.arg.lightning_2.intensity;
                            g_rs485_data.fx_mode_para.frq = p_decode->sys_fx_2.arg.lightning_2.frq;
                            g_rs485_data.fx_mode_para.spd = p_decode->sys_fx_2.arg.lightning_2.spd;
                            g_rs485_data.fx_mode_para.mode = p_decode->sys_fx_2.arg.lightning_2.mode;
                            if(1 == set_sys_fx2_light_para(&g_rs485_data.fx_mode_para, 
                                                           &p_decode->sys_fx_2.arg.lightning_2.mode_arg)){
                                ack_body.ack = RS485_Ack_Err_Command;
                            }
                            #endif
                            g_rs485_data.light_mode_state.command_2 = p_decode->sys_fx_2.type;
                            Rs485_Slave_Ack_Package1(&data_header, &ack_body, p_ack_data, p_ack_offset);
                        }
                        break;
                        case RS485_FX_II_TV_2:{
                            ack_body.ack = RS485_Ack_Ok;
                            #if 1
                            g_rs485_data.fx_mode_para.state = p_decode->sys_fx_2.arg.tv_2.state;
                            g_rs485_data.fx_mode_para.intensity = p_decode->sys_fx_2.arg.tv_2.intensity;
                            g_rs485_data.fx_mode_para.spd = p_decode->sys_fx_2.arg.tv_2.spd;
                            g_rs485_data.fx_mode_para.mode = p_decode->sys_fx_2.arg.tv_2.mode;
                            if(1 == set_sys_fx2_light_para(&g_rs485_data.fx_mode_para, 
                                                           &p_decode->sys_fx_2.arg.tv_2.mode_arg)){
                                ack_body.ack = RS485_Ack_Err_Command;
                            }
                            #endif
                            g_rs485_data.light_mode_state.command_2 = p_decode->sys_fx_2.type;
                            Rs485_Slave_Ack_Package1(&data_header, &ack_body, p_ack_data, p_ack_offset);
                        }
                        break;
                        case RS485_FX_II_Fire_2:{
                            ack_body.ack = RS485_Ack_Ok;
                            #if 1
                            g_rs485_data.fx_mode_para.state = p_decode->sys_fx_2.arg.fire_2.state;
                            g_rs485_data.fx_mode_para.intensity = p_decode->sys_fx_2.arg.fire_2.intensity;
                            g_rs485_data.fx_mode_para.spd = p_decode->sys_fx_2.arg.fire_2.spd;
                            g_rs485_data.fx_mode_para.mode = p_decode->sys_fx_2.arg.fire_2.mode;
                            if(1 == set_sys_fx2_light_para(&g_rs485_data.fx_mode_para, 
                                                           &p_decode->sys_fx_2.arg.fire_2.mode_arg)){
                                ack_body.ack = RS485_Ack_Err_Command;
                            }
                            #endif
                            g_rs485_data.light_mode_state.command_2 = p_decode->sys_fx_2.type;
                            Rs485_Slave_Ack_Package1(&data_header, &ack_body, p_ack_data, p_ack_offset);
                        }
                        break;
                        case RS485_FX_II_Strobe_2:{
                            ack_body.ack = RS485_Ack_Ok;
                            #if 1
                            g_rs485_data.fx_mode_para.state = p_decode->sys_fx_2.arg.strobe_2.state;
                            g_rs485_data.fx_mode_para.intensity = p_decode->sys_fx_2.arg.strobe_2.intensity;
                            g_rs485_data.fx_mode_para.spd = p_decode->sys_fx_2.arg.strobe_2.spd;
                            g_rs485_data.fx_mode_para.mode = p_decode->sys_fx_2.arg.strobe_2.mode;
                            if(1 == set_sys_fx2_light_para(&g_rs485_data.fx_mode_para, 
                                                           &p_decode->sys_fx_2.arg.strobe_2.mode_arg)){
                                ack_body.ack = RS485_Ack_Err_Command;
                            }
                            #endif
                            g_rs485_data.light_mode_state.command_2 = p_decode->sys_fx_2.type;
                            Rs485_Slave_Ack_Package1(&data_header, &ack_body, p_ack_data, p_ack_offset);
                        }
                        break;
                        case RS485_FX_II_Explosion_2:{
                            ack_body.ack = RS485_Ack_Ok;
                            #if 1
                            g_rs485_data.fx_mode_para.trigger = (uint8_t)p_decode->sys_fx_2.arg.explosion_2.state;
                            g_rs485_data.fx_mode_para.intensity = p_decode->sys_fx_2.arg.explosion_2.intensity;
                            g_rs485_data.fx_mode_para.decay = p_decode->sys_fx_2.arg.explosion_2.decay;
                            g_rs485_data.fx_mode_para.mode = p_decode->sys_fx_2.arg.explosion_2.mode;
                            if(1 == set_sys_fx2_light_para(&g_rs485_data.fx_mode_para, 
                                                           &p_decode->sys_fx_2.arg.explosion_2.mode_arg)){
                                ack_body.ack = RS485_Ack_Err_Command;
                            }
                            #endif
                            g_rs485_data.light_mode_state.command_2 = p_decode->sys_fx_2.type;
                            Rs485_Slave_Ack_Package1(&data_header, &ack_body, p_ack_data, p_ack_offset);
                        }
                        break;
                        case RS485_FX_II_Fault_Bulb_2:{
                            ack_body.ack = RS485_Ack_Ok;
                            #if 1
                            g_rs485_data.fx_mode_para.state = p_decode->sys_fx_2.arg.fault_bulb_2.state;
                            g_rs485_data.fx_mode_para.intensity = p_decode->sys_fx_2.arg.fault_bulb_2.intensity;
                            g_rs485_data.fx_mode_para.spd = p_decode->sys_fx_2.arg.fault_bulb_2.spd;
                            g_rs485_data.fx_mode_para.frq = p_decode->sys_fx_2.arg.fault_bulb_2.frq;
                            g_rs485_data.fx_mode_para.mode = p_decode->sys_fx_2.arg.fault_bulb_2.mode;
                            if(1 == set_sys_fx2_light_para(&g_rs485_data.fx_mode_para, 
                                                           &p_decode->sys_fx_2.arg.fault_bulb_2.mode_arg)){
                                ack_body.ack = RS485_Ack_Err_Command;
                            }
                            #endif
                            g_rs485_data.light_mode_state.command_2 = p_decode->sys_fx_2.type;
                            Rs485_Slave_Ack_Package1(&data_header, &ack_body, p_ack_data, p_ack_offset);
                        }
                        break;
                        case RS485_FX_II_Pulsing_2:{
                            ack_body.ack = RS485_Ack_Ok;
                            #if 1
                            g_rs485_data.fx_mode_para.state = p_decode->sys_fx_2.arg.pulsing_2.state;
                            g_rs485_data.fx_mode_para.intensity = p_decode->sys_fx_2.arg.pulsing_2.intensity;
                            g_rs485_data.fx_mode_para.spd = p_decode->sys_fx_2.arg.pulsing_2.spd;
                            g_rs485_data.fx_mode_para.frq = p_decode->sys_fx_2.arg.pulsing_2.frq;
                            g_rs485_data.fx_mode_para.mode = p_decode->sys_fx_2.arg.pulsing_2.mode;
                            if(1 == set_sys_fx2_light_para(&g_rs485_data.fx_mode_para, 
                                                           &p_decode->sys_fx_2.arg.pulsing_2.mode_arg)){
                                ack_body.ack = RS485_Ack_Err_Command;
                            }
                            #endif
                            g_rs485_data.light_mode_state.command_2 = p_decode->sys_fx_2.type;
                            Rs485_Slave_Ack_Package1(&data_header, &ack_body, p_ack_data, p_ack_offset);
                        }
                        break;
                        case RS485_FX_II_Welding_2:{
                            ack_body.ack = RS485_Ack_Ok;
                            #if 1
                            g_rs485_data.fx_mode_para.state = p_decode->sys_fx_2.arg.welding_2.state;
                            g_rs485_data.fx_mode_para.intensity = p_decode->sys_fx_2.arg.welding_2.intensity;
                            g_rs485_data.fx_mode_para.min_int = p_decode->sys_fx_2.arg.welding_2.min_intensity;
                            g_rs485_data.fx_mode_para.frq = p_decode->sys_fx_2.arg.welding_2.frq;
                            g_rs485_data.fx_mode_para.mode = p_decode->sys_fx_2.arg.welding_2.mode;
                            if(1 == set_sys_fx2_light_para(&g_rs485_data.fx_mode_para, 
                                                           &p_decode->sys_fx_2.arg.welding_2.mode_arg)){
                                ack_body.ack = RS485_Ack_Err_Command;
                            }
                            #endif
                            g_rs485_data.light_mode_state.command_2 = p_decode->sys_fx_2.type;
                            Rs485_Slave_Ack_Package1(&data_header, &ack_body, p_ack_data, p_ack_offset);
                        }
                        break;
                        case RS485_FX_II_Cop_Car_2:{
                            ack_body.ack = RS485_Ack_Ok;
                            #if 1
                            g_rs485_data.fx_mode_para.state = p_decode->sys_fx_2.arg.cop_car_2.state;
                            g_rs485_data.fx_mode_para.intensity = p_decode->sys_fx_2.arg.cop_car_2.intensity;
                            g_rs485_data.fx_mode_para.frq = p_decode->sys_fx_2.arg.cop_car_2.frq;
                            g_rs485_data.fx_mode_para.color = p_decode->sys_fx_2.arg.cop_car_2.color;
                            #endif
                            g_rs485_data.light_mode_state.command_2 = p_decode->sys_fx_2.type;
                            Rs485_Slave_Ack_Package1(&data_header, &ack_body, p_ack_data, p_ack_offset);
                        }
                        break;
                        case RS485_FX_II_Party_Lights_2:{
                            ack_body.ack = RS485_Ack_Ok;
                            #if 1
                            g_rs485_data.fx_mode_para.state = p_decode->sys_fx_2.arg.party_lights_2.state;
                            g_rs485_data.fx_mode_para.intensity = p_decode->sys_fx_2.arg.party_lights_2.intensity;
                            g_rs485_data.fx_mode_para.sat = p_decode->sys_fx_2.arg.party_lights_2.sat;
                            g_rs485_data.fx_mode_para.spd = p_decode->sys_fx_2.arg.party_lights_2.spd;
                            #endif
                            g_rs485_data.light_mode_state.command_2 = p_decode->sys_fx_2.type;
                            Rs485_Slave_Ack_Package1(&data_header, &ack_body, p_ack_data, p_ack_offset);
                        }
                        break;
                        case RS485_FX_II_Fireworks_2:{
                            ack_body.ack = RS485_Ack_Ok;
                            #if 1
                            g_rs485_data.fx_mode_para.state = p_decode->sys_fx_2.arg.fireworks_2.state;
                            g_rs485_data.fx_mode_para.intensity = p_decode->sys_fx_2.arg.fireworks_2.intensity;
                            g_rs485_data.fx_mode_para.gap_time = p_decode->sys_fx_2.arg.fireworks_2.gap_time;
                            g_rs485_data.fx_mode_para.min_gap_time = p_decode->sys_fx_2.arg.fireworks_2.min_gap_time;
                            g_rs485_data.fx_mode_para.mode = p_decode->sys_fx_2.arg.fireworks_2.mode;
                            #endif
                            g_rs485_data.light_mode_state.command_2 = p_decode->sys_fx_2.type;
                            Rs485_Slave_Ack_Package1(&data_header, &ack_body, p_ack_data, p_ack_offset);
                        }
                        break;
                        case RS485_FX_II_Lightning_3:{
                            ack_body.ack = RS485_Ack_Ok;
                            #if 1
                            g_rs485_data.fx_mode_para.state = p_decode->sys_fx_2.arg.lightning_3.state;
                            g_rs485_data.fx_mode_para.intensity = p_decode->sys_fx_2.arg.lightning_3.intensity;
                            g_rs485_data.fx_mode_para.gap_time = p_decode->sys_fx_2.arg.lightning_3.gap_time;
                            g_rs485_data.fx_mode_para.min_gap_time = p_decode->sys_fx_2.arg.lightning_3.min_gap_time;
                            g_rs485_data.fx_mode_para.mode = p_decode->sys_fx_2.arg.lightning_3.mode;
                            if(1 == set_sys_fx2_light_para(&g_rs485_data.fx_mode_para, 
                                                           &p_decode->sys_fx_2.arg.lightning_3.mode_arg)){
                                ack_body.ack = RS485_Ack_Err_Command;
                            }
                            #endif
                            g_rs485_data.light_mode_state.command_2 = p_decode->sys_fx_2.type;
                            Rs485_Slave_Ack_Package1(&data_header, &ack_body, p_ack_data, p_ack_offset);
                        }
                        break;
                        case RS485_FX_II_TV_3:{
                            ack_body.ack = RS485_Ack_Ok;
                            #if 1
                            g_rs485_data.fx_mode_para.state = p_decode->sys_fx_2.arg.tv_3.state;
                            g_rs485_data.fx_mode_para.intensity = p_decode->sys_fx_2.arg.tv_3.intensity;
                            g_rs485_data.fx_mode_para.gap_time = p_decode->sys_fx_2.arg.tv_3.gap_time;
                            g_rs485_data.fx_mode_para.min_gap_time = p_decode->sys_fx_2.arg.tv_3.min_gap_time;
                            g_rs485_data.fx_mode_para.mode = p_decode->sys_fx_2.arg.tv_3.mode;
                            if(1 == set_sys_fx2_light_para(&g_rs485_data.fx_mode_para, 
                                                           &p_decode->sys_fx_2.arg.tv_3.mode_arg)){
                                ack_body.ack = RS485_Ack_Err_Command;
                            }                               
                            #endif
                            g_rs485_data.light_mode_state.command_2 = p_decode->sys_fx_2.type;
                            Rs485_Slave_Ack_Package1(&data_header, &ack_body, p_ack_data, p_ack_offset);
                        }
                        break;
                        case RS485_FX_II_Fire_3:{
                            ack_body.ack = RS485_Ack_Ok;
                            #if 1
                            g_rs485_data.fx_mode_para.state = p_decode->sys_fx_2.arg.fire_3.state;
                            g_rs485_data.fx_mode_para.intensity = p_decode->sys_fx_2.arg.fire_3.intensity;
                            g_rs485_data.fx_mode_para.frq = p_decode->sys_fx_2.arg.fire_3.frq;
                            g_rs485_data.fx_mode_para.mode = p_decode->sys_fx_2.arg.fire_3.mode;
                            if(1 == set_sys_fx2_light_para(&g_rs485_data.fx_mode_para, 
                                                           &p_decode->sys_fx_2.arg.fire_3.mode_arg)){
                                ack_body.ack = RS485_Ack_Err_Command;
                            }
                            #endif
                            g_rs485_data.light_mode_state.command_2 = p_decode->sys_fx_2.type;
                            Rs485_Slave_Ack_Package1(&data_header, &ack_body, p_ack_data, p_ack_offset);
                        }
                        break;
                        case RS485_FX_II_Faulty_Bulb_3:{
                            ack_body.ack = RS485_Ack_Ok;
                            #if 1
                            g_rs485_data.fx_mode_para.state = p_decode->sys_fx_2.arg.faulty_bulb_3.state;
                            g_rs485_data.fx_mode_para.intensity = p_decode->sys_fx_2.arg.faulty_bulb_3.intensity;
                            g_rs485_data.fx_mode_para.gap_time = p_decode->sys_fx_2.arg.faulty_bulb_3.gap_time;
                            g_rs485_data.fx_mode_para.min_gap_time = p_decode->sys_fx_2.arg.faulty_bulb_3.min_gap_time;
                            g_rs485_data.fx_mode_para.mode = p_decode->sys_fx_2.arg.faulty_bulb_3.mode;
                            if(1 == set_sys_fx2_light_para(&g_rs485_data.fx_mode_para, 
                                                           &p_decode->sys_fx_2.arg.faulty_bulb_3.mode_arg)){
                                ack_body.ack = RS485_Ack_Err_Command;
                            }
                            #endif
                            g_rs485_data.light_mode_state.command_2 = p_decode->sys_fx_2.type;
                            Rs485_Slave_Ack_Package1(&data_header, &ack_body, p_ack_data, p_ack_offset);
                        }
                        break;
                        case RS485_FX_II_Pulsing_3:{
                            ack_body.ack = RS485_Ack_Ok;
                            #if 1
                            g_rs485_data.fx_mode_para.state = p_decode->sys_fx_2.arg.pulsing_3.state;
                            g_rs485_data.fx_mode_para.intensity = p_decode->sys_fx_2.arg.pulsing_3.intensity;
                            g_rs485_data.fx_mode_para.frq = p_decode->sys_fx_2.arg.pulsing_3.frq;
                            g_rs485_data.fx_mode_para.mode = p_decode->sys_fx_2.arg.pulsing_3.mode;
                            if(1 == set_sys_fx2_light_para(&g_rs485_data.fx_mode_para, 
                                                           &p_decode->sys_fx_2.arg.pulsing_3.mode_arg)){
                                ack_body.ack = RS485_Ack_Err_Command;
                            }
                            #endif
                            g_rs485_data.light_mode_state.command_2 = p_decode->sys_fx_2.type;
                            Rs485_Slave_Ack_Package1(&data_header, &ack_body, p_ack_data, p_ack_offset);
                        }
                        break;
                        case RS485_FX_II_Cop_Car_3:{
                            ack_body.ack = RS485_Ack_Ok;
                            #if 1
                            g_rs485_data.fx_mode_para.state = p_decode->sys_fx_2.arg.fireworks_3.state;
                            g_rs485_data.fx_mode_para.intensity = p_decode->sys_fx_2.arg.fireworks_3.intensity;
                            g_rs485_data.fx_mode_para.frq = p_decode->sys_fx_2.arg.fireworks_3.frq;
                            g_rs485_data.fx_mode_para.mode = p_decode->sys_fx_2.arg.fireworks_3.mode;
                            #endif
                            g_rs485_data.light_mode_state.command_2 = p_decode->sys_fx_2.type;
                            Rs485_Slave_Ack_Package1(&data_header, &ack_body, p_ack_data, p_ack_offset);
                        }
                        break;
                        default:{
							ack_body.ack = RS485_Ack_Err_Command;
                            Rs485_Slave_Ack_Package1(&data_header, &ack_body, p_ack_data, p_ack_offset);
						}break;
                    }
                }
                break;
                case RS485_Cmd_Self_Adaption:{
                    ack_body.ack = RS485_Ack_Err_Command;
                    Rs485_Slave_Ack_Package1(&data_header, &ack_body, p_ack_data, p_ack_offset);
                }
                break;
                case RS485_Cmd_Factory_RGBWW:{ 
                    #if 1
                    g_rs485_data.light_mode_para.red_ratio = p_decode->factory_rgbww.red;
                    g_rs485_data.light_mode_para.green_ratio = p_decode->factory_rgbww.green;
                    g_rs485_data.light_mode_para.blue_ratio =p_decode->factory_rgbww.blue;
                    g_rs485_data.light_mode_para.ww_ratio = p_decode->factory_rgbww.ww;
                    g_rs485_data.light_mode_para.cw_ratio = p_decode->factory_rgbww.cw;
					g_rs485_data.light_mode_para.ww2_ratio = p_decode->factory_rgbww.ww2;
                    g_rs485_data.light_mode_para.cw2_ratio = p_decode->factory_rgbww.cw2;
                    #endif
                    g_rs485_data.new_command.light_mode = 1;
                    g_rs485_data.light_mode_state.command_1 = data_header.cmd_type;
                    ack_body.ack = RS485_Ack_Ok;
                    Rs485_Slave_Ack_Package1(&data_header, &ack_body, p_ack_data, p_ack_offset);
                }
                break;
                case RS485_Cmd_Source:{
                    #if 1
                    g_rs485_data.light_mode_para.intensity = p_decode->source.intensity;
                    g_rs485_data.light_mode_para.type = p_decode->source.type;
                    g_rs485_data.light_mode_para.x = p_decode->source.x_coord;
                    g_rs485_data.light_mode_para.y = p_decode->source.y_coord;
                    g_rs485_data.light_mode_para.fade = p_decode->source.fade;
                    g_rs485_data.light_mode_para.pixel_x = p_decode->source.pixel_x;
                    g_rs485_data.light_mode_para.pixel_y = p_decode->source.pixel_y;
                    #endif
                    g_rs485_data.new_command.light_mode = 1;
                    g_rs485_data.light_mode_state.command_1 = data_header.cmd_type;
                    ack_body.ack = RS485_Ack_Ok;
                    Rs485_Slave_Ack_Package1(&data_header, &ack_body, p_ack_data, p_ack_offset);
                }
                break;
                case RS485_Cmd_IlluminationMode:{ 
                    #if 1
                    g_rs485_data.illumination_mode.mode = p_decode->illumination_mode.mode;
                    g_rs485_data.illumination_mode.ratio = p_decode->illumination_mode.ratio;
                    g_rs485_data.illumination_mode.save_ctr = p_decode->illumination_mode.save_ctr;
                    g_rs485_data.illumination_mode.power_calib_ratio = p_decode->illumination_mode.power_calib_ratio;
                    #endif
                    g_rs485_data.new_command.Illumination_Mode = 1;
                    ack_body.ack = RS485_Ack_Ok;
                    Rs485_Slave_Ack_Package1(&data_header, &ack_body, p_ack_data, p_ack_offset);
                }
                break;
                case RS485_Cmd_Color_Mixing:{ 
                    g_rs485_data.new_command.light_mode = 1;
                    g_rs485_data.light_mode_state.command_1 = data_header.cmd_type;
                    ack_body.ack = RS485_Ack_Ok;
					//TITO:
                    #if 1
//                    g_color_mixing_struct.Color1_Arg.Color_Mode = (Mix_ColorMixingMode)p_decode->color_mixing.color_1_arg.mode;
					switch(p_decode->color_mixing.color_1_arg.mode){
                        case RS485_ColorMixingMode_CCT:{
                            g_color_mixing_struct.color1_arg.mode = Mix_ColorMixingMode_CCT;
                            g_color_mixing_struct.color1_arg.arg.cct.lightness = p_decode->color_mixing.color_1_arg.mode_arg.cct.intensity;
                            g_color_mixing_struct.color1_arg.arg.cct.cct = p_decode->color_mixing.color_1_arg.mode_arg.cct.cct;
                            g_color_mixing_struct.color1_arg.arg.cct.duv = p_decode->color_mixing.color_1_arg.mode_arg.cct.duv;//Phoebe:根据灯控修改,走的浮点
                        }
                        break;
                        case RS485_ColorMixingMode_HSI:{
                            g_color_mixing_struct.color1_arg.mode = Mix_ColorMixingMode_HSI;
                            g_color_mixing_struct.color1_arg.arg.hsi.lightness  = p_decode->color_mixing.color_1_arg.mode_arg.hsi.intensity;
                            g_color_mixing_struct.color1_arg.arg.hsi.hue = (float)p_decode->color_mixing.color_1_arg.mode_arg.hsi.hue / 100.0f;//Phoebe
                            g_color_mixing_struct.color1_arg.arg.hsi.sat = (float)p_decode->color_mixing.color_1_arg.mode_arg.hsi.sat / 100.0f;
                            g_color_mixing_struct.color1_arg.arg.hsi.cct = p_decode->color_mixing.color_1_arg.mode_arg.hsi.cct;
                        }
                        break;
                        case RS485_ColorMixingMode_GEL:{
                            g_color_mixing_struct.color1_arg.mode = Mix_ColorMixingMode_GEL;
                            g_color_mixing_struct.color1_arg.arg.gel.lightness = p_decode->color_mixing.color_1_arg.mode_arg.gel.intensity;
                            g_color_mixing_struct.color1_arg.arg.gel.cct = p_decode->color_mixing.color_1_arg.mode_arg.gel.cct;
                            g_color_mixing_struct.color1_arg.arg.gel.brand = p_decode->color_mixing.color_1_arg.mode_arg.gel.origin;
                            g_color_mixing_struct.color1_arg.arg.gel.type = p_decode->color_mixing.color_1_arg.mode_arg.gel.series;
                            g_color_mixing_struct.color1_arg.arg.gel.color = p_decode->color_mixing.color_1_arg.mode_arg.gel.color;
							
                        }
                        break;
                        case RS485_ColorMixingMode_XY:{
                            g_color_mixing_struct.color1_arg.mode = Mix_ColorMixingMode_XY;
                            g_color_mixing_struct.color1_arg.arg.xy.lightness = p_decode->color_mixing.color_1_arg.mode_arg.xy.intensity;
                            g_color_mixing_struct.color1_arg.arg.xy.x = (float)p_decode->color_mixing.color_1_arg.mode_arg.xy.x / 10000.0f;//Phoebe
                            g_color_mixing_struct.color1_arg.arg.xy.y = (float)p_decode->color_mixing.color_1_arg.mode_arg.xy.y / 10000.0f;
                        }
                        break;
                        case RS485_ColorMixingMode_SOUYRCE:{
                            g_color_mixing_struct.color1_arg.mode = Mix_ColorMixingMode_SOUYRCE;
                            g_color_mixing_struct.color1_arg.arg.source.lightness = p_decode->color_mixing.color_1_arg.mode_arg.source.intensity;
                            g_color_mixing_struct.color1_arg.arg.source.type = p_decode->color_mixing.color_1_arg.mode_arg.source.type;
                            g_color_mixing_struct.color1_arg.arg.source.x = (float)p_decode->color_mixing.color_1_arg.mode_arg.source.x_coord / 10000.0f;
                            g_color_mixing_struct.color1_arg.arg.source.y = (float)p_decode->color_mixing.color_1_arg.mode_arg.source.y_coord / 10000.0f;
                        }
                        break;
                        case RS485_ColorMixingMode_RGB:{
                            g_color_mixing_struct.color1_arg.mode = Mix_ColorMixingMode_RGB;
                            g_color_mixing_struct.color1_arg.arg.rgb.lightness = p_decode->color_mixing.color_1_arg.mode_arg.rgb.intensity;
                            g_color_mixing_struct.color1_arg.arg.rgb.r = (float)p_decode->color_mixing.color_1_arg.mode_arg.rgb.red_ratio / 1000.0f;
                            g_color_mixing_struct.color1_arg.arg.rgb.g = (float)p_decode->color_mixing.color_1_arg.mode_arg.rgb.green_ratio / 1000.0f;
                            g_color_mixing_struct.color1_arg.arg.rgb.b = (float)p_decode->color_mixing.color_1_arg.mode_arg.rgb.blue_ratio / 1000.0f;
                        }
                        break;
                        default:{
                            ack_body.ack = RS485_Ack_Err_Command;
                        }
                        break;
                    }
//                    g_color_mixing_struct.color2_arg.mode = (Mix_ColorMixingMode)p_decode->color_mixing.color_2_arg.mode;
                    switch(p_decode->color_mixing.color_2_arg.mode){
                        case RS485_ColorMixingMode_CCT:{
                            g_color_mixing_struct.color2_arg.mode = Mix_ColorMixingMode_CCT;
                            g_color_mixing_struct.color2_arg.arg.cct.lightness = p_decode->color_mixing.color_2_arg.mode_arg.cct.intensity;
                            g_color_mixing_struct.color2_arg.arg.cct.cct = p_decode->color_mixing.color_2_arg.mode_arg.cct.cct;
                            g_color_mixing_struct.color2_arg.arg.cct.duv = p_decode->color_mixing.color_2_arg.mode_arg.cct.duv;//灯控按协议走浮点型
                        }
                        break;
                        case RS485_ColorMixingMode_HSI:{
                            g_color_mixing_struct.color2_arg.mode = Mix_ColorMixingMode_HSI;
                            g_color_mixing_struct.color2_arg.arg.hsi.lightness  = p_decode->color_mixing.color_2_arg.mode_arg.hsi.intensity;
                            g_color_mixing_struct.color2_arg.arg.hsi.hue = (float)p_decode->color_mixing.color_2_arg.mode_arg.hsi.hue / 100.0f;
                            g_color_mixing_struct.color2_arg.arg.hsi.sat = (float)p_decode->color_mixing.color_2_arg.mode_arg.hsi.sat / 100.0f;
                            g_color_mixing_struct.color2_arg.arg.hsi.cct = p_decode->color_mixing.color_2_arg.mode_arg.hsi.cct;
                        }
                        break;
                        case RS485_ColorMixingMode_GEL:{
                            g_color_mixing_struct.color2_arg.mode = Mix_ColorMixingMode_GEL;
                            g_color_mixing_struct.color2_arg.arg.gel.lightness = p_decode->color_mixing.color_2_arg.mode_arg.gel.intensity;
                            g_color_mixing_struct.color2_arg.arg.gel.cct = p_decode->color_mixing.color_2_arg.mode_arg.gel.cct;
                            g_color_mixing_struct.color2_arg.arg.gel.brand = p_decode->color_mixing.color_2_arg.mode_arg.gel.origin;
                            g_color_mixing_struct.color2_arg.arg.gel.type = p_decode->color_mixing.color_2_arg.mode_arg.gel.series;
                            g_color_mixing_struct.color2_arg.arg.gel.color = p_decode->color_mixing.color_2_arg.mode_arg.gel.color;
                        }
                        break;
                        case RS485_ColorMixingMode_XY:{
                            g_color_mixing_struct.color2_arg.mode = Mix_ColorMixingMode_XY;
                            g_color_mixing_struct.color2_arg.arg.xy.lightness = p_decode->color_mixing.color_2_arg.mode_arg.xy.intensity;
                            g_color_mixing_struct.color2_arg.arg.xy.x = (float)p_decode->color_mixing.color_2_arg.mode_arg.xy.x / 10000.0f;//Phoebe
                            g_color_mixing_struct.color2_arg.arg.xy.y = (float)p_decode->color_mixing.color_2_arg.mode_arg.xy.y / 10000.0f;
                        }
                        break;
                        case RS485_ColorMixingMode_SOUYRCE:{
                            g_color_mixing_struct.color2_arg.mode = Mix_ColorMixingMode_SOUYRCE;
                            g_color_mixing_struct.color2_arg.arg.source.lightness = p_decode->color_mixing.color_2_arg.mode_arg.source.intensity;
                            g_color_mixing_struct.color2_arg.arg.source.type = p_decode->color_mixing.color_2_arg.mode_arg.source.type;
                            g_color_mixing_struct.color2_arg.arg.source.x = (float)p_decode->color_mixing.color_2_arg.mode_arg.source.x_coord / 10000.0f;
                            g_color_mixing_struct.color2_arg.arg.source.y = (float)p_decode->color_mixing.color_2_arg.mode_arg.source.y_coord /10000.0f;
                        }
                        break;
                        case RS485_ColorMixingMode_RGB:{
                            g_color_mixing_struct.color2_arg.mode = Mix_ColorMixingMode_RGB;
                            g_color_mixing_struct.color2_arg.arg.rgb.lightness = p_decode->color_mixing.color_2_arg.mode_arg.rgb.intensity;
                            g_color_mixing_struct.color2_arg.arg.rgb.r = (float)p_decode->color_mixing.color_2_arg.mode_arg.rgb.red_ratio / 1000.0f;
                            g_color_mixing_struct.color2_arg.arg.rgb.g = (float)p_decode->color_mixing.color_2_arg.mode_arg.rgb.green_ratio / 1000.0f;//Phoebe
                            g_color_mixing_struct.color2_arg.arg.rgb.b = (float)p_decode->color_mixing.color_2_arg.mode_arg.rgb.blue_ratio / 1000.0f;
                        }
                        break;
                        default:{
                            ack_body.ack = RS485_Ack_Err_Command;
                        }
                        break;
                    }
                    g_color_mixing_struct.ratio = (float)p_decode->color_mixing.color_1_ratio / 10000.0f;
                    g_rs485_data.light_mode_para.fade = p_decode->color_mixing.fade;
                    g_rs485_data.light_mode_para.pixel_x = p_decode->color_mixing.pixel_x;
                    g_rs485_data.light_mode_para.pixel_y = p_decode->color_mixing.pixel_y;
                    #endif
                    Rs485_Slave_Ack_Package1(&data_header, &ack_body, p_ack_data, p_ack_offset);
                }
                break;
                case RS485_Cmd_Err_Msg:{ 
                    ack_body.ack = RS485_Ack_Err_Command;
                    Rs485_Slave_Ack_Package1(&data_header, &ack_body, p_ack_data, p_ack_offset);
                }
                break;
                case RS485_Cmd_PFX_Ctrl:{
                    g_sidus_fx_str.SFX_Type = SidusPro_PFX;
                    p_decode->pfx_ctrl.light_type = 0;/*目前这个参数没有使用*/
                    
                    ack_body.ack = RS485_Ack_Ok;
                    g_rs485_data.new_command.light_mode = 1;
                    g_rs485_data.light_mode_state.command_1 = data_header.cmd_type;
                    switch(p_decode->pfx_ctrl.pfx_type){
                        case 0:{//PFX_Flash
                            g_sidus_fx_str.SFX_Arg.PFX.Mode = PFX_Mode_Flash;
                            switch(p_decode->pfx_ctrl.pfx_arg.flash.base){
                                case 0:{//CCT
                                    g_sidus_fx_str.SFX_Arg.PFX.ModeArg.Flash.BaseCCT.Base = p_decode->pfx_ctrl.pfx_arg.flash.base;
                                    g_sidus_fx_str.SFX_Arg.PFX.ModeArg.Flash.BaseCCT.Int = 
                                    sidus_fx_power_cut(p_decode->pfx_ctrl.pfx_arg.flash.base_arg.cct.intensity, g_power_factor.illumination);
                                    g_sidus_fx_str.SFX_Arg.PFX.ModeArg.Flash.BaseCCT.CCT = p_decode->pfx_ctrl.pfx_arg.flash.base_arg.cct.cct;
                                    g_sidus_fx_str.SFX_Arg.PFX.ModeArg.Flash.BaseCCT.GM = p_decode->pfx_ctrl.pfx_arg.flash.base_arg.cct.gm;
                                    g_sidus_fx_str.SFX_Arg.PFX.ModeArg.Flash.BaseCCT.Frq = p_decode->pfx_ctrl.pfx_arg.flash.base_arg.cct.frq;
                                    g_sidus_fx_str.SFX_Arg.PFX.ModeArg.Flash.BaseCCT.Times = p_decode->pfx_ctrl.pfx_arg.flash.base_arg.cct.times;
                                    
                                }
                                break;
                                case 1:{//HSI
                                    g_sidus_fx_str.SFX_Arg.PFX.ModeArg.Flash.BaseHSI.Base = p_decode->pfx_ctrl.pfx_arg.flash.base;
                                    g_sidus_fx_str.SFX_Arg.PFX.ModeArg.Flash.BaseHSI.Int = 
                                    sidus_fx_power_cut(p_decode->pfx_ctrl.pfx_arg.flash.base_arg.hsi.intensity, g_power_factor.illumination);
                                    g_sidus_fx_str.SFX_Arg.PFX.ModeArg.Flash.BaseHSI.Hue = p_decode->pfx_ctrl.pfx_arg.flash.base_arg.hsi.hue;
                                    g_sidus_fx_str.SFX_Arg.PFX.ModeArg.Flash.BaseHSI.Sat = p_decode->pfx_ctrl.pfx_arg.flash.base_arg.hsi.sat;
                                    g_sidus_fx_str.SFX_Arg.PFX.ModeArg.Flash.BaseHSI.Frq = p_decode->pfx_ctrl.pfx_arg.flash.base_arg.hsi.frq;
                                    g_sidus_fx_str.SFX_Arg.PFX.ModeArg.Flash.BaseHSI.Times = p_decode->pfx_ctrl.pfx_arg.flash.base_arg.hsi.times;
                                    
                                }
                                break;
                                default:{
                                    ack_body.ack = RS485_Ack_Err_Command;
                                }break;
                            }
                        }
                        break;
                        
                        case 1:{//PFX_Continue
                            g_sidus_fx_str.SFX_Arg.PFX.Mode = PFX_Mode_Continue;
                            switch(p_decode->pfx_ctrl.pfx_arg.continues.base){
                                case 0:{//CCT
                                    g_sidus_fx_str.SFX_Arg.PFX.ModeArg.Continue.BaseCCT.Base = p_decode->pfx_ctrl.pfx_arg.continues.base;
                                    g_sidus_fx_str.SFX_Arg.PFX.ModeArg.Continue.BaseCCT.Int = 
                                    sidus_fx_power_cut(p_decode->pfx_ctrl.pfx_arg.continues.base_arg.cct.intensity, g_power_factor.illumination);
                                    g_sidus_fx_str.SFX_Arg.PFX.ModeArg.Continue.BaseCCT.CCT = p_decode->pfx_ctrl.pfx_arg.continues.base_arg.cct.cct;
                                    g_sidus_fx_str.SFX_Arg.PFX.ModeArg.Continue.BaseCCT.GM = p_decode->pfx_ctrl.pfx_arg.continues.base_arg.cct.gm;
                                    g_sidus_fx_str.SFX_Arg.PFX.ModeArg.Continue.BaseCCT.FadeInTime = p_decode->pfx_ctrl.pfx_arg.continues.base_arg.cct.fade_in_time;
                                    g_sidus_fx_str.SFX_Arg.PFX.ModeArg.Continue.BaseCCT.FadeInCurve = p_decode->pfx_ctrl.pfx_arg.continues.base_arg.cct.fade_in_curve;
                                    g_sidus_fx_str.SFX_Arg.PFX.ModeArg.Continue.BaseCCT.ContinueTime = p_decode->pfx_ctrl.pfx_arg.continues.base_arg.cct.continue_time;
                                    g_sidus_fx_str.SFX_Arg.PFX.ModeArg.Continue.BaseCCT.FadeOutTime = p_decode->pfx_ctrl.pfx_arg.continues.base_arg.cct.fade_out_time;
                                    g_sidus_fx_str.SFX_Arg.PFX.ModeArg.Continue.BaseCCT.FadeOutCurve = p_decode->pfx_ctrl.pfx_arg.continues.base_arg.cct.fade_out_curve;
                                    
                                }
                                break;
                                case 1:{//HSI
                                    g_sidus_fx_str.SFX_Arg.PFX.ModeArg.Continue.BaseHSI.Base = p_decode->pfx_ctrl.pfx_arg.continues.base;
                                    g_sidus_fx_str.SFX_Arg.PFX.ModeArg.Continue.BaseHSI.Int = 
                                    sidus_fx_power_cut(p_decode->pfx_ctrl.pfx_arg.continues.base_arg.hsi.intensity, g_power_factor.illumination);
                                    g_sidus_fx_str.SFX_Arg.PFX.ModeArg.Continue.BaseHSI.Hue = p_decode->pfx_ctrl.pfx_arg.continues.base_arg.hsi.hue;
                                    g_sidus_fx_str.SFX_Arg.PFX.ModeArg.Continue.BaseHSI.Sat = p_decode->pfx_ctrl.pfx_arg.continues.base_arg.hsi.sat;
                                    g_sidus_fx_str.SFX_Arg.PFX.ModeArg.Continue.BaseHSI.FadeInTime = p_decode->pfx_ctrl.pfx_arg.continues.base_arg.hsi.fade_in_time;
                                    g_sidus_fx_str.SFX_Arg.PFX.ModeArg.Continue.BaseHSI.FadeInCurve = p_decode->pfx_ctrl.pfx_arg.continues.base_arg.hsi.fade_in_curve;
                                    g_sidus_fx_str.SFX_Arg.PFX.ModeArg.Continue.BaseHSI.ContinueTime = p_decode->pfx_ctrl.pfx_arg.continues.base_arg.hsi.continue_time;
                                    g_sidus_fx_str.SFX_Arg.PFX.ModeArg.Continue.BaseHSI.FadeOutTime = p_decode->pfx_ctrl.pfx_arg.continues.base_arg.hsi.fade_out_time;
                                    g_sidus_fx_str.SFX_Arg.PFX.ModeArg.Continue.BaseHSI.FadeOutCurve = p_decode->pfx_ctrl.pfx_arg.continues.base_arg.hsi.fade_out_curve;
                                    
                                }
                                break;
                                default:{
                                    ack_body.ack = RS485_Ack_Err_Command;
                                }break;
                            }
                        }
                        break;
                        case 2:{//PFX_ColorChase
                            g_sidus_fx_str.SFX_Arg.PFX.Mode = PFX_Mode_Chase;
                            switch(p_decode->pfx_ctrl.pfx_arg.chase.base){
                                case 0:{//CCT
                                    g_sidus_fx_str.SFX_Arg.PFX.ModeArg.Chase.BaseCCT.Base = p_decode->pfx_ctrl.pfx_arg.chase.base;
                                    g_sidus_fx_str.SFX_Arg.PFX.ModeArg.Chase.BaseCCT.Int_Mini = 
                                    sidus_fx_power_cut(p_decode->pfx_ctrl.pfx_arg.chase.base_arg.cct.min_intensity, g_power_factor.illumination);
                                    g_sidus_fx_str.SFX_Arg.PFX.ModeArg.Chase.BaseCCT.Int_Max = 
                                    sidus_fx_power_cut(p_decode->pfx_ctrl.pfx_arg.chase.base_arg.cct.max_intensity, g_power_factor.illumination);
                                    g_sidus_fx_str.SFX_Arg.PFX.ModeArg.Chase.BaseCCT.CCT_Mini = p_decode->pfx_ctrl.pfx_arg.chase.base_arg.cct.min_cct;
                                    g_sidus_fx_str.SFX_Arg.PFX.ModeArg.Chase.BaseCCT.CCT_Max = p_decode->pfx_ctrl.pfx_arg.chase.base_arg.cct.max_cct;
                                    g_sidus_fx_str.SFX_Arg.PFX.ModeArg.Chase.BaseCCT.GM_Mini = p_decode->pfx_ctrl.pfx_arg.chase.base_arg.cct.min_gm;
                                    g_sidus_fx_str.SFX_Arg.PFX.ModeArg.Chase.BaseCCT.GM_Max = p_decode->pfx_ctrl.pfx_arg.chase.base_arg.cct.max_gm;
                                    g_sidus_fx_str.SFX_Arg.PFX.ModeArg.Chase.BaseCCT.Time = p_decode->pfx_ctrl.pfx_arg.chase.base_arg.cct.times;
                                    g_sidus_fx_str.SFX_Arg.PFX.ModeArg.Chase.BaseCCT.CCT_Seq = p_decode->pfx_ctrl.pfx_arg.chase.base_arg.cct.cct_seq;
                                    g_sidus_fx_str.SFX_Arg.PFX.ModeArg.Chase.BaseCCT.Int_Seq = p_decode->pfx_ctrl.pfx_arg.chase.base_arg.cct.int_seq;
                                    g_sidus_fx_str.SFX_Arg.PFX.ModeArg.Chase.BaseCCT.GM_Seq = p_decode->pfx_ctrl.pfx_arg.chase.base_arg.cct.gm_seq;
                                    g_sidus_fx_str.SFX_Arg.PFX.ModeArg.Chase.BaseCCT.loop = p_decode->pfx_ctrl.pfx_arg.chase.base_arg.cct.loop;
                                    
                                }
                                break;
                                case 1:{//HSI
                                    g_sidus_fx_str.SFX_Arg.PFX.ModeArg.Chase.BaseHSI.Base = p_decode->pfx_ctrl.pfx_arg.chase.base;
                                    g_sidus_fx_str.SFX_Arg.PFX.ModeArg.Chase.BaseHSI.Int_Mini = 
                                    sidus_fx_power_cut(p_decode->pfx_ctrl.pfx_arg.chase.base_arg.hsi.min_intensity, g_power_factor.illumination);
                                    g_sidus_fx_str.SFX_Arg.PFX.ModeArg.Chase.BaseHSI.Int_Max = 
                                    sidus_fx_power_cut(p_decode->pfx_ctrl.pfx_arg.chase.base_arg.hsi.max_intensity, g_power_factor.illumination);
                                    g_sidus_fx_str.SFX_Arg.PFX.ModeArg.Chase.BaseHSI.Hue_Mini = p_decode->pfx_ctrl.pfx_arg.chase.base_arg.hsi.min_hue;
                                    g_sidus_fx_str.SFX_Arg.PFX.ModeArg.Chase.BaseHSI.Hue_Max = p_decode->pfx_ctrl.pfx_arg.chase.base_arg.hsi.max_hue;
                                    g_sidus_fx_str.SFX_Arg.PFX.ModeArg.Chase.BaseHSI.Sat_Mini = p_decode->pfx_ctrl.pfx_arg.chase.base_arg.hsi.min_sat;
                                    g_sidus_fx_str.SFX_Arg.PFX.ModeArg.Chase.BaseHSI.Sat_Max = p_decode->pfx_ctrl.pfx_arg.chase.base_arg.hsi.max_sat;
                                    g_sidus_fx_str.SFX_Arg.PFX.ModeArg.Chase.BaseHSI.Time = p_decode->pfx_ctrl.pfx_arg.chase.base_arg.hsi.times;
                                    g_sidus_fx_str.SFX_Arg.PFX.ModeArg.Chase.BaseHSI.Hue_Seq = p_decode->pfx_ctrl.pfx_arg.chase.base_arg.hsi.hue_seq;
                                    g_sidus_fx_str.SFX_Arg.PFX.ModeArg.Chase.BaseHSI.Int_Seq = p_decode->pfx_ctrl.pfx_arg.chase.base_arg.hsi.int_seq;
                                    g_sidus_fx_str.SFX_Arg.PFX.ModeArg.Chase.BaseHSI.Sat_Seq = p_decode->pfx_ctrl.pfx_arg.chase.base_arg.hsi.sat_Seq;
                                    g_sidus_fx_str.SFX_Arg.PFX.ModeArg.Chase.BaseHSI.loop = p_decode->pfx_ctrl.pfx_arg.chase.base_arg.hsi.loop;
                                    
                                }
                                break;
                                default:{
                                    ack_body.ack = RS485_Ack_Err_Command;
                                }break;
                            }
                        }
                        break;
                        default:{
                            ack_body.ack = RS485_Ack_Err_Command;
                        }break;
                    }
                    Rs485_Slave_Ack_Package1(&data_header, &ack_body, p_ack_data, p_ack_offset);
                }
                break;
                case RS485_Cmd_CFX_Bank_RW:{
                    #if 0
                    p_decode->cfx_bank_rw.picker;
                    p_decode->cfx_bank_rw.touch_bar;
                    p_decode->cfx_bank_rw.music;
                    #endif
//                    g_rs485_data.new_command.X = X;/*应用层需要新定义一个变量来做此功能的信号量*/
                    g_rs485_data.light_mode_state.command_1 = data_header.cmd_type;
                    ack_body.ack = RS485_Ack_Ok;
                    Rs485_Slave_Ack_Package1(&data_header, &ack_body, p_ack_data, p_ack_offset);
                }
                break;
                case RS485_Cmd_CFX_Ctrl:{
                    #if 1
                    g_sidus_fx_str.SFX_Type = SidusPro_CFX;
                    g_sidus_fx_str.SFX_Arg.CFX.Effect_Type = p_decode->cfx_ctrl.effect_type;
                    g_sidus_fx_str.SFX_Arg.CFX.Bank = p_decode->cfx_ctrl.bank;
                    g_sidus_fx_str.SFX_Arg.CFX.Int = 
                    sidus_fx_power_cut(p_decode->cfx_ctrl.intensity, g_power_factor.illumination);
                    g_sidus_fx_str.SFX_Arg.CFX.Speed = p_decode->cfx_ctrl.spd;
                    g_sidus_fx_str.SFX_Arg.CFX.Chaos = p_decode->cfx_ctrl.chaos;
                    g_sidus_fx_str.SFX_Arg.CFX.Sequence = p_decode->cfx_ctrl.sequence;
                    g_sidus_fx_str.SFX_Arg.CFX.Loop = p_decode->cfx_ctrl.loop;
                    g_sidus_fx_str.SFX_Arg.CFX.Ctrl = p_decode->cfx_ctrl.state;
                    #endif
                    g_rs485_data.new_command.light_mode = 1;
                    g_rs485_data.light_mode_state.command_1 = data_header.cmd_type;
                    ack_body.ack = RS485_Ack_Ok;
                    if(1 == g_sidus_fx_str.SFX_Arg.CFX.Ctrl){
                        g_rs485_ack_wait.command_type = RS485_Cmd_CFX_Ctrl;
                        g_rs485_ack_wait.delay_time = 40000;
                        g_rs485_ack_wait.ack_type = RS485_Ack_Ok;
                        g_rs485_ack_wait.ack_slot = *p_ack_offset + sizeof(rs485_cmd_header_t);/*后面需要重新计算校验和时使用*/
                        
                        g_rs485_ack_wait.wait_ack = get_cfx_init_state;
                    }
                    Rs485_Slave_Ack_Package1(&data_header, &ack_body, p_ack_data, p_ack_offset);
                }
                break;
                case RS485_Cmd_CFX_Preview:{
                    #if 1
                    g_sidus_fx_str.SFX_Type = SidusPro_CFX_Preview;
                    g_rs485_data.new_command.light_mode = 1;
                    g_rs485_data.light_mode_state.command_1 = data_header.cmd_type;
                    ack_body.ack = RS485_Ack_Ok;
                    switch(p_decode->cfx_preview.frame_1.base){
                        case 0:{//CCT
                            g_sidus_fx_str.SFX_Arg.CFX_Preview.Frame_1.BaseCCT.Base = 0;
                            g_sidus_fx_str.SFX_Arg.CFX_Preview.Frame_1.BaseCCT.Int = 
                            sidus_fx_power_cut(p_decode->cfx_preview.frame_1.base_arg.cct.intensity, g_power_factor.illumination);
                            g_sidus_fx_str.SFX_Arg.CFX_Preview.Frame_1.BaseCCT.CCT = p_decode->cfx_preview.frame_1.base_arg.cct.cct;
                            g_sidus_fx_str.SFX_Arg.CFX_Preview.Frame_1.BaseCCT.GM = p_decode->cfx_preview.frame_1.base_arg.cct.gm;
                        }
                        break;
                        case 1:{//HSI
                            g_sidus_fx_str.SFX_Arg.CFX_Preview.Frame_1.BaseHSI.Base = 1;
                            g_sidus_fx_str.SFX_Arg.CFX_Preview.Frame_1.BaseHSI.Int = 
                            sidus_fx_power_cut(p_decode->cfx_preview.frame_1.base_arg.hsi.intensity, g_power_factor.illumination);
                            g_sidus_fx_str.SFX_Arg.CFX_Preview.Frame_1.BaseHSI.Hue = p_decode->cfx_preview.frame_1.base_arg.hsi.hue;
                            g_sidus_fx_str.SFX_Arg.CFX_Preview.Frame_1.BaseHSI.Sat = p_decode->cfx_preview.frame_1.base_arg.hsi.sat;
                        }
                        break;
                        default:{
                            ack_body.ack = RS485_Ack_Err_Command;
                        }break;
                    }
                    switch(p_decode->cfx_preview.frame_2.base){
                        case 0:{//CCT
                            g_sidus_fx_str.SFX_Arg.CFX_Preview.Frame_2.BaseCCT.Base = 0;
                            g_sidus_fx_str.SFX_Arg.CFX_Preview.Frame_2.BaseCCT.Int = 
                            sidus_fx_power_cut(p_decode->cfx_preview.frame_2.base_arg.cct.intensity, g_power_factor.illumination);
                            g_sidus_fx_str.SFX_Arg.CFX_Preview.Frame_2.BaseCCT.CCT = p_decode->cfx_preview.frame_2.base_arg.cct.cct;
                            g_sidus_fx_str.SFX_Arg.CFX_Preview.Frame_2.BaseCCT.GM = p_decode->cfx_preview.frame_2.base_arg.cct.gm;
                        }
                        break;
                        case 1:{//HSI
                            g_sidus_fx_str.SFX_Arg.CFX_Preview.Frame_2.BaseHSI.Base = 1;
                            g_sidus_fx_str.SFX_Arg.CFX_Preview.Frame_2.BaseHSI.Int = 
                            sidus_fx_power_cut(p_decode->cfx_preview.frame_2.base_arg.hsi.intensity, g_power_factor.illumination);
                            g_sidus_fx_str.SFX_Arg.CFX_Preview.Frame_2.BaseHSI.Hue = p_decode->cfx_preview.frame_2.base_arg.hsi.hue;
                            g_sidus_fx_str.SFX_Arg.CFX_Preview.Frame_2.BaseHSI.Sat = p_decode->cfx_preview.frame_2.base_arg.hsi.sat;
                        }
                        break;
                        default:{
                            ack_body.ack = RS485_Ack_Err_Command;
                        }break;
                    }
                    #endif
                    
                    Rs485_Slave_Ack_Package1(&data_header, &ack_body, p_ack_data, p_ack_offset);
                }
                break;
                case RS485_Cmd_MFX_Ctrl:{
                    #if 1
                    g_sidus_fx_str.SFX_Type = SidusPro_MFX;
                    g_sidus_fx_str.SFX_Arg.MFX.Ctrl = (MFX_Ctrl_Type)p_decode->mfx_ctrl.state;
                    
                    g_rs485_data.new_command.light_mode = 1;
                    g_rs485_data.light_mode_state.command_1 = data_header.cmd_type;
                    ack_body.ack = RS485_Ack_Ok;
                    switch(p_decode->mfx_ctrl.base_arg.base){
                        case 0:{//CCT
                            g_sidus_fx_str.SFX_Arg.MFX.BaseArg.BaseCCT.Base = 0;
                            g_sidus_fx_str.SFX_Arg.MFX.BaseArg.BaseCCT.Int_Mini = 
                            sidus_fx_power_cut(p_decode->mfx_ctrl.base_arg.base_arg.cct.min_intensity, g_power_factor.illumination);
                            g_sidus_fx_str.SFX_Arg.MFX.BaseArg.BaseCCT.Int_Max = 
                            sidus_fx_power_cut(p_decode->mfx_ctrl.base_arg.base_arg.cct.max_intensity, g_power_factor.illumination);
                            g_sidus_fx_str.SFX_Arg.MFX.BaseArg.BaseCCT.Int_Seq = p_decode->mfx_ctrl.base_arg.base_arg.cct.int_seq;
                            g_sidus_fx_str.SFX_Arg.MFX.BaseArg.BaseCCT.CCT_Mini = p_decode->mfx_ctrl.base_arg.base_arg.cct.min_cct;
                            g_sidus_fx_str.SFX_Arg.MFX.BaseArg.BaseCCT.CCT_Max = p_decode->mfx_ctrl.base_arg.base_arg.cct.max_cct;
                            g_sidus_fx_str.SFX_Arg.MFX.BaseArg.BaseCCT.CCT_Seq = p_decode->mfx_ctrl.base_arg.base_arg.cct.cct_seq;
                            g_sidus_fx_str.SFX_Arg.MFX.BaseArg.BaseCCT.GM_Mini = p_decode->mfx_ctrl.base_arg.base_arg.cct.min_gm;
                            g_sidus_fx_str.SFX_Arg.MFX.BaseArg.BaseCCT.GM_Max = p_decode->mfx_ctrl.base_arg.base_arg.cct.max_gm;
                            g_sidus_fx_str.SFX_Arg.MFX.BaseArg.BaseCCT.GM_Seq = p_decode->mfx_ctrl.base_arg.base_arg.cct.gm_seq;
                        }
                        break;
                        case 1:{//HSI
                           g_sidus_fx_str.SFX_Arg.MFX.BaseArg.BaseHSI.Base = 1;
                           g_sidus_fx_str.SFX_Arg.MFX.BaseArg.BaseHSI.Int_Mini =  
                           sidus_fx_power_cut(p_decode->mfx_ctrl.base_arg.base_arg.hsi.min_intensity, g_power_factor.illumination);
                           g_sidus_fx_str.SFX_Arg.MFX.BaseArg.BaseHSI.Int_Max =  
                           sidus_fx_power_cut(p_decode->mfx_ctrl.base_arg.base_arg.hsi.max_intensity, g_power_factor.illumination);
                           g_sidus_fx_str.SFX_Arg.MFX.BaseArg.BaseHSI.Int_Seq = p_decode->mfx_ctrl.base_arg.base_arg.hsi.int_seq; 
                           g_sidus_fx_str.SFX_Arg.MFX.BaseArg.BaseHSI.Hue_Mini = p_decode->mfx_ctrl.base_arg.base_arg.hsi.min_hue; 
                           g_sidus_fx_str.SFX_Arg.MFX.BaseArg.BaseHSI.Hue_Max = p_decode->mfx_ctrl.base_arg.base_arg.hsi.max_hue; 
                           g_sidus_fx_str.SFX_Arg.MFX.BaseArg.BaseHSI.Hue_Seq = p_decode->mfx_ctrl.base_arg.base_arg.hsi.hue_seq; 
                           g_sidus_fx_str.SFX_Arg.MFX.BaseArg.BaseHSI.Sat_Mini = p_decode->mfx_ctrl.base_arg.base_arg.hsi.min_sat; 
                           g_sidus_fx_str.SFX_Arg.MFX.BaseArg.BaseHSI.Sat_Max = p_decode->mfx_ctrl.base_arg.base_arg.hsi.max_sat; 
                           g_sidus_fx_str.SFX_Arg.MFX.BaseArg.BaseHSI.Sat_Seq = p_decode->mfx_ctrl.base_arg.base_arg.hsi.sat_seq; 
                        }
                        break;
                        default:break;
                    }
                    switch(p_decode->mfx_ctrl.mode_arg.effect_mode){
                        case 0:{//Flash
                            g_sidus_fx_str.SFX_Arg.MFX.Mode = MFX_MODE_FLASH;
                            g_sidus_fx_str.SFX_Arg.MFX.ModeArg.Flash.LoopMode = p_decode->mfx_ctrl.mode_arg.mode.flash.loop_mode;
                            g_sidus_fx_str.SFX_Arg.MFX.ModeArg.Flash.LoopTimes = p_decode->mfx_ctrl.mode_arg.mode.flash.loop_times;
                            g_sidus_fx_str.SFX_Arg.MFX.ModeArg.Flash.CycleTime_Mini = p_decode->mfx_ctrl.mode_arg.mode.flash.min_cycle_time;
                            g_sidus_fx_str.SFX_Arg.MFX.ModeArg.Flash.CycleTime_Max = p_decode->mfx_ctrl.mode_arg.mode.flash.max_cycle_time;
                            g_sidus_fx_str.SFX_Arg.MFX.ModeArg.Flash.CycleTime_Seq = p_decode->mfx_ctrl.mode_arg.mode.flash.cycle_time_seq;
                            g_sidus_fx_str.SFX_Arg.MFX.ModeArg.Flash.FreeTime_Mini = p_decode->mfx_ctrl.mode_arg.mode.flash.min_free_time;
                            g_sidus_fx_str.SFX_Arg.MFX.ModeArg.Flash.FreeTime_Max = p_decode->mfx_ctrl.mode_arg.mode.flash.max_free_time;
                            g_sidus_fx_str.SFX_Arg.MFX.ModeArg.Flash.FreeTime_Seq = p_decode->mfx_ctrl.mode_arg.mode.flash.free_time_seq;
                            g_sidus_fx_str.SFX_Arg.MFX.ModeArg.Flash.UnitTime_Mini = p_decode->mfx_ctrl.mode_arg.mode.flash.min_unit_uime;
                            g_sidus_fx_str.SFX_Arg.MFX.ModeArg.Flash.UnitTime_Max = p_decode->mfx_ctrl.mode_arg.mode.flash.max_unit_uime;
                            g_sidus_fx_str.SFX_Arg.MFX.ModeArg.Flash.UnitTime_Seq = p_decode->mfx_ctrl.mode_arg.mode.flash.unit_time_seq;
                            g_sidus_fx_str.SFX_Arg.MFX.ModeArg.Flash.Frq_Mini = p_decode->mfx_ctrl.mode_arg.mode.flash.min_frq;
                            g_sidus_fx_str.SFX_Arg.MFX.ModeArg.Flash.Frq_Max = p_decode->mfx_ctrl.mode_arg.mode.flash.max_frq;
                            g_sidus_fx_str.SFX_Arg.MFX.ModeArg.Flash.Frq_Seq = p_decode->mfx_ctrl.mode_arg.mode.flash.frq_Seq;
                        }
                        break;
                        case 1:{//Continue
                            g_sidus_fx_str.SFX_Arg.MFX.Mode = MFX_MODE_CONTINUE;
                            g_sidus_fx_str.SFX_Arg.MFX.ModeArg.Continue.LoopMode = p_decode->mfx_ctrl.mode_arg.mode.continues.loop_mode;
                            g_sidus_fx_str.SFX_Arg.MFX.ModeArg.Continue.LoopTimes = p_decode->mfx_ctrl.mode_arg.mode.continues.loop_times;
                            g_sidus_fx_str.SFX_Arg.MFX.ModeArg.Continue.CycleTime_Mini = p_decode->mfx_ctrl.mode_arg.mode.continues.min_cycle_time;
                            g_sidus_fx_str.SFX_Arg.MFX.ModeArg.Continue.CycleTime_Max = p_decode->mfx_ctrl.mode_arg.mode.continues.max_cycle_time;
                            g_sidus_fx_str.SFX_Arg.MFX.ModeArg.Continue.CycleTime_Seq = p_decode->mfx_ctrl.mode_arg.mode.continues.cycle_time_seq;
                            g_sidus_fx_str.SFX_Arg.MFX.ModeArg.Continue.FadeInTime_Mini = p_decode->mfx_ctrl.mode_arg.mode.continues.min_fade_in_time;
                            g_sidus_fx_str.SFX_Arg.MFX.ModeArg.Continue.FadeInTime_Max = p_decode->mfx_ctrl.mode_arg.mode.continues.max_fade_in_time;
                            g_sidus_fx_str.SFX_Arg.MFX.ModeArg.Continue.FadeInTime_Seq = p_decode->mfx_ctrl.mode_arg.mode.continues.fade_in_time_seq;
                            g_sidus_fx_str.SFX_Arg.MFX.ModeArg.Continue.FadeIn_Curve = p_decode->mfx_ctrl.mode_arg.mode.continues.fade_in_curve;
                            g_sidus_fx_str.SFX_Arg.MFX.ModeArg.Continue.FadeOutTime_Mini = p_decode->mfx_ctrl.mode_arg.mode.continues.min_fade_out_time;
                            g_sidus_fx_str.SFX_Arg.MFX.ModeArg.Continue.FadeOutTime_Max = p_decode->mfx_ctrl.mode_arg.mode.continues.max_fade_out_time;
                            g_sidus_fx_str.SFX_Arg.MFX.ModeArg.Continue.FadeOutTime_Seq = p_decode->mfx_ctrl.mode_arg.mode.continues.fade_out_time_seq;
                            g_sidus_fx_str.SFX_Arg.MFX.ModeArg.Continue.FadeOut_Curve = p_decode->mfx_ctrl.mode_arg.mode.continues.fade_out_curve;
                            g_sidus_fx_str.SFX_Arg.MFX.ModeArg.Continue.Flicker_Frq = p_decode->mfx_ctrl.mode_arg.mode.continues.flash_frq;
                        }
                        break;
                        case 2:{//Paragraph
                            g_sidus_fx_str.SFX_Arg.MFX.Mode = MFX_MODE_PARAGRAPH;
                            g_sidus_fx_str.SFX_Arg.MFX.ModeArg.Paragraph.LoopMode = p_decode->mfx_ctrl.mode_arg.mode.paragraph.loop_mode;
                            g_sidus_fx_str.SFX_Arg.MFX.ModeArg.Paragraph.LoopTimes = p_decode->mfx_ctrl.mode_arg.mode.paragraph.loop_times;
                            g_sidus_fx_str.SFX_Arg.MFX.ModeArg.Paragraph.CycleTime_Mini = p_decode->mfx_ctrl.mode_arg.mode.paragraph.min_cycle_time;
                            g_sidus_fx_str.SFX_Arg.MFX.ModeArg.Paragraph.CycleTime_Max = p_decode->mfx_ctrl.mode_arg.mode.paragraph.max_cycle_time;
                            g_sidus_fx_str.SFX_Arg.MFX.ModeArg.Paragraph.CycleTime_Seq = p_decode->mfx_ctrl.mode_arg.mode.paragraph.cycle_time_seq;
                            g_sidus_fx_str.SFX_Arg.MFX.ModeArg.Paragraph.OLR_Mini = p_decode->mfx_ctrl.mode_arg.mode.paragraph.min_olr;
                            g_sidus_fx_str.SFX_Arg.MFX.ModeArg.Paragraph.OLR_Max = p_decode->mfx_ctrl.mode_arg.mode.paragraph.max_olr;
                            g_sidus_fx_str.SFX_Arg.MFX.ModeArg.Paragraph.OLR_Seq = p_decode->mfx_ctrl.mode_arg.mode.paragraph.olr_seq;
                            g_sidus_fx_str.SFX_Arg.MFX.ModeArg.Paragraph.Overlap_Seq = p_decode->mfx_ctrl.mode_arg.mode.paragraph.over_lap_seq;
                            g_sidus_fx_str.SFX_Arg.MFX.ModeArg.Paragraph.FreeTime_Seq = p_decode->mfx_ctrl.mode_arg.mode.paragraph.free_time_seq;
                            g_sidus_fx_str.SFX_Arg.MFX.ModeArg.Paragraph.UnitTime_Seq = p_decode->mfx_ctrl.mode_arg.mode.paragraph.unit_time_seq;
                            g_sidus_fx_str.SFX_Arg.MFX.ModeArg.Paragraph.FreeTime_Mini = p_decode->mfx_ctrl.mode_arg.mode.paragraph.min_free_time;
                            g_sidus_fx_str.SFX_Arg.MFX.ModeArg.Paragraph.FreeTime_Max = p_decode->mfx_ctrl.mode_arg.mode.paragraph.max_free_time;
                            g_sidus_fx_str.SFX_Arg.MFX.ModeArg.Paragraph.UnitTime_Mini = p_decode->mfx_ctrl.mode_arg.mode.paragraph.min_unit_uime;
                            g_sidus_fx_str.SFX_Arg.MFX.ModeArg.Paragraph.UnitTime_Max = p_decode->mfx_ctrl.mode_arg.mode.paragraph.max_unit_uime;
                            g_sidus_fx_str.SFX_Arg.MFX.ModeArg.Paragraph.OLP_Mini = p_decode->mfx_ctrl.mode_arg.mode.paragraph.min_olp;
                            g_sidus_fx_str.SFX_Arg.MFX.ModeArg.Paragraph.OLP_Max = p_decode->mfx_ctrl.mode_arg.mode.paragraph.max_olp;
                            g_sidus_fx_str.SFX_Arg.MFX.ModeArg.Paragraph.OLP_Seq = p_decode->mfx_ctrl.mode_arg.mode.paragraph.olp_seq;
                            g_sidus_fx_str.SFX_Arg.MFX.ModeArg.Paragraph.FadeInTime_Mini = p_decode->mfx_ctrl.mode_arg.mode.paragraph.min_fade_in_time;
                            g_sidus_fx_str.SFX_Arg.MFX.ModeArg.Paragraph.FadeInTime_Max = p_decode->mfx_ctrl.mode_arg.mode.paragraph.max_fade_in_time;
                            g_sidus_fx_str.SFX_Arg.MFX.ModeArg.Paragraph.FadeInTime_Seq = p_decode->mfx_ctrl.mode_arg.mode.paragraph.fade_in_time_seq;
                            g_sidus_fx_str.SFX_Arg.MFX.ModeArg.Paragraph.FadeIn_Curve = p_decode->mfx_ctrl.mode_arg.mode.paragraph.fade_in_curve;
                            g_sidus_fx_str.SFX_Arg.MFX.ModeArg.Paragraph.FadeOutTime_Mini = p_decode->mfx_ctrl.mode_arg.mode.paragraph.min_fade_out_time;
                            g_sidus_fx_str.SFX_Arg.MFX.ModeArg.Paragraph.FadeOutTime_Max = p_decode->mfx_ctrl.mode_arg.mode.paragraph.max_fade_out_time;
                            g_sidus_fx_str.SFX_Arg.MFX.ModeArg.Paragraph.FadeOutTime_Seq = p_decode->mfx_ctrl.mode_arg.mode.paragraph.fade_out_time_seq;
                            g_sidus_fx_str.SFX_Arg.MFX.ModeArg.Paragraph.FadeOut_Curve = p_decode->mfx_ctrl.mode_arg.mode.paragraph.fade_out_curve;
                            g_sidus_fx_str.SFX_Arg.MFX.ModeArg.Paragraph.Flicker_Frq = p_decode->mfx_ctrl.mode_arg.mode.paragraph.flash_frq;
                        }
                        break;
                        default:break;
                    }
                    #endif
                    
                    Rs485_Slave_Ack_Package1(&data_header, &ack_body, p_ack_data, p_ack_offset);
                }
                break;
                case RS485_Cmd_RGBWW:{
                    #if 1
                    g_rs485_data.light_mode_para.intensity = p_decode->rgbww.intensity;
                    g_rs485_data.light_mode_para.red_ratio = p_decode->rgbww.red_ratio;
                    g_rs485_data.light_mode_para.green_ratio = p_decode->rgbww.green_ratio;
                    g_rs485_data.light_mode_para.blue_ratio = p_decode->rgbww.blue_ratio;
                    g_rs485_data.light_mode_para.ww_ratio = p_decode->rgbww.ww_ratio;
                    g_rs485_data.light_mode_para.cw_ratio = p_decode->rgbww.cw_ratio;
                    g_rs485_data.light_mode_para.fade = p_decode->rgbww.fade;
                    g_rs485_data.light_mode_para.pixel_x = p_decode->rgbww.pixel_x;
                    g_rs485_data.light_mode_para.pixel_y = p_decode->rgbww.pixel_y;
                    #endif
                    g_rs485_data.new_command.light_mode = 1;
                    g_rs485_data.light_mode_state.command_1 = data_header.cmd_type;
                    ack_body.ack = RS485_Ack_Ok;
                    Rs485_Slave_Ack_Package1(&data_header, &ack_body, p_ack_data, p_ack_offset);
                }
                break;
                case RS485_Cmd_CFX_Name:{
                    if(true == Set_Cfx_Name(p_decode)){
                        ack_body.ack = RS485_Ack_Ok;
                    }
                    else{
                        ack_body.ack = RS485_Ack_Err_Verify;
                    }
                    Rs485_Slave_Ack_Package1(&data_header, &ack_body, p_ack_data, p_ack_offset);
                }
                break;
                case RS485_Cmd_Curr_Light_Mode:{ 
                    ack_body.ack = RS485_Ack_Err_Command;
                    Rs485_Slave_Ack_Package1(&data_header, &ack_body, p_ack_data, p_ack_offset);
                }
                break;
                case RS485_Cmd_PixelEffect:{
                    g_rs485_data.fx_restart = 0;/*光效正常运行*/
                    g_rs485_data.new_command.light_mode = 1;
                    g_rs485_data.light_mode_state.command_1 = data_header.cmd_type;
                    switch(p_decode->pixel_fx.type){
                        case RS485_PixelFX_Color_Cut:{
                            #if 1
                            g_rs485_data.pixel_fx_para.pixel_num = p_decode->pixel_fx.arg.color_cut.pixel_num;
                            g_rs485_data.pixel_fx_para.pixel_index = p_decode->pixel_fx.arg.color_cut.pixel_index;
                            g_rs485_data.pixel_fx_para.intensity = p_decode->pixel_fx.arg.color_cut.intensity;
//                            g_rs485_data.pixel_fx_para.light_cross_len = p_decode->pixel_fx.arg.color_cut.light_cross_len;
                            g_rs485_data.pixel_fx_para.dir = p_decode->pixel_fx.arg.color_cut.dir;
                            g_rs485_data.pixel_fx_para.spd = p_decode->pixel_fx.arg.color_cut.spd;
                            g_rs485_data.pixel_fx_para.state = p_decode->pixel_fx.arg.color_cut.state;
                            g_rs485_data.pixel_fx_para.mode = p_decode->pixel_fx.arg.color_cut.mode;
                            switch(p_decode->pixel_fx.arg.color_cut.mode){
                                case 0:{//cct
                                    g_rs485_data.pixel_fx_para.cct = p_decode->pixel_fx.arg.color_cut.mode_arg.cct.cct;
                                    g_rs485_data.pixel_fx_para.duv = p_decode->pixel_fx.arg.color_cut.mode_arg.cct.duv;
                                }
                                break;
                                case 1:{//hsi
                                    g_rs485_data.pixel_fx_para.hue = p_decode->pixel_fx.arg.color_cut.mode_arg.hsi.hue;
                                    g_rs485_data.pixel_fx_para.sat = p_decode->pixel_fx.arg.color_cut.mode_arg.hsi.sat;
                                    g_rs485_data.pixel_fx_para.cct = p_decode->pixel_fx.arg.color_cut.mode_arg.hsi.cct;
                                }
                                break;
                                default:break;
                            }
                            #endif
                            g_rs485_data.light_mode_state.command_2 = p_decode->pixel_fx.type;
                            ack_body.ack = RS485_Ack_Ok;
                            Rs485_Slave_Ack_Package1(&data_header, &ack_body, p_ack_data, p_ack_offset);
                        }
                        break;
                        case RS485_PixelFX_Color_Rotate:{
                            #if 1
                            g_rs485_data.pixel_fx_para.pixel_num = p_decode->pixel_fx.arg.color_stay.pixel_num;
                            g_rs485_data.pixel_fx_para.pixel_index = p_decode->pixel_fx.arg.color_stay.pixel_index;
                            g_rs485_data.pixel_fx_para.intensity = p_decode->pixel_fx.arg.color_stay.intensity;
//                            g_rs485_data.pixel_fx_para.color_num = p_decode->pixel_fx.arg.color_stay.color_num;
                            g_rs485_data.pixel_fx_para.state = p_decode->pixel_fx.arg.color_stay.state;
                            g_rs485_data.pixel_fx_para.mode = p_decode->pixel_fx.arg.color_stay.mode;
                            switch(p_decode->pixel_fx.arg.color_stay.mode){
                                case 0:{//cct
                                    g_rs485_data.pixel_fx_para.cct = p_decode->pixel_fx.arg.color_stay.mode_arg.cct.cct;
                                    g_rs485_data.pixel_fx_para.duv = p_decode->pixel_fx.arg.color_stay.mode_arg.cct.duv;
                                }
                                break;
                                case 1:{//hsi
                                    g_rs485_data.pixel_fx_para.hue = p_decode->pixel_fx.arg.color_stay.mode_arg.hsi.hue;
                                    g_rs485_data.pixel_fx_para.sat = p_decode->pixel_fx.arg.color_stay.mode_arg.hsi.sat;
                                    g_rs485_data.pixel_fx_para.cct = p_decode->pixel_fx.arg.color_stay.mode_arg.hsi.cct;
                                }
                                break;
                                default:break;
                            }
                            #endif
                            g_rs485_data.light_mode_state.command_2 = p_decode->pixel_fx.type;
                            ack_body.ack = RS485_Ack_Ok;
                            Rs485_Slave_Ack_Package1(&data_header, &ack_body, p_ack_data, p_ack_offset);
                        }
                        break;
                        case RS485_PixelFX_One_Color_Move:{
                            #if 1
                            
                            #endif
                            g_rs485_data.light_mode_state.command_2 = p_decode->pixel_fx.type;
                            ack_body.ack = RS485_Ack_Ok;
                            Rs485_Slave_Ack_Package1(&data_header, &ack_body, p_ack_data, p_ack_offset);
                        }
                        break;
                        case RS485_PixelFX_Two_Color_Move:{
                            #if 1
                            
                            #endif
                            g_rs485_data.light_mode_state.command_2 = p_decode->pixel_fx.type;
                            ack_body.ack = RS485_Ack_Ok;
                            Rs485_Slave_Ack_Package1(&data_header, &ack_body, p_ack_data, p_ack_offset);
                        }
                        break;
                        case RS485_PixelFX_Three_Color_Move:{
                            #if 1
                            
                            #endif
                            g_rs485_data.light_mode_state.command_2 = p_decode->pixel_fx.type;
                            ack_body.ack = RS485_Ack_Ok;
                            Rs485_Slave_Ack_Package1(&data_header, &ack_body, p_ack_data, p_ack_offset);
                        }
                        break;
                        case RS485_PixelFX_Pixel_Fire:{
                            #if 1
                            
                            #endif
                            g_rs485_data.light_mode_state.command_2 = p_decode->pixel_fx.type;
                            ack_body.ack = RS485_Ack_Ok;
                            Rs485_Slave_Ack_Package1(&data_header, &ack_body, p_ack_data, p_ack_offset);
                        }
                        break;
                        case RS485_PixelFX_Many_Color_Move:{
                            #if 1
                            
                            #endif
                            g_rs485_data.light_mode_state.command_2 = p_decode->pixel_fx.type;
                            ack_body.ack = RS485_Ack_Ok;
                            Rs485_Slave_Ack_Package1(&data_header, &ack_body, p_ack_data, p_ack_offset);
                        }
                        break;
                        case RS485_PixelFX_On_Off_Effect:{
                            #if 1
                            
                            #endif
                            g_rs485_data.light_mode_state.command_2 = p_decode->pixel_fx.type;
                            ack_body.ack = RS485_Ack_Ok;
                            Rs485_Slave_Ack_Package1(&data_header, &ack_body, p_ack_data, p_ack_offset);
                        }
                        break;
                        case RS485_PixelFX_Belt_Effect:{
                            g_rs485_data.light_mode_state.command_2 = p_decode->pixel_fx.type;
                            switch(p_decode->pixel_fx.arg.belt_fx.type){
                                case RS485_BeltFX_Fade:{
                                    
                                    g_rs485_data.light_mode_state.command_3 = p_decode->pixel_fx.arg.belt_fx.type;
                                }
                                break;
                                case RS485_BeltFX_Holoday:{
                                    
                                    g_rs485_data.light_mode_state.command_3 = p_decode->pixel_fx.arg.belt_fx.type;
                                }
                                break;
                                case RS485_BeltFX_One_Color_Marquee:{
                                    
                                    g_rs485_data.light_mode_state.command_3 = p_decode->pixel_fx.arg.belt_fx.type;
                                }
                                break;
                                case RS485_BeltFX_Full_Color_Marquee:{
                                    
                                    g_rs485_data.light_mode_state.command_3 = p_decode->pixel_fx.arg.belt_fx.type;
                                }
                                break;
                                case RS485_BeltFX_Starry_Sky:{
                                    
                                    g_rs485_data.light_mode_state.command_3 = p_decode->pixel_fx.arg.belt_fx.type;
                                }
                                break;
                                case RS485_BeltFX_Aurora:{
                                    
                                    g_rs485_data.light_mode_state.command_3 = p_decode->pixel_fx.arg.belt_fx.type;
                                }
                                break;
                                case RS485_BeltFX_Racing:{
                                    
                                    g_rs485_data.light_mode_state.command_3 = p_decode->pixel_fx.arg.belt_fx.type;
                                }
                                break;
                                case RS485_BeltFX_Train:{
                                    
                                    g_rs485_data.light_mode_state.command_3 = p_decode->pixel_fx.arg.belt_fx.type;
                                }
                                break;
                                case RS485_BeltFX_Snake:{
                                    
                                    g_rs485_data.light_mode_state.command_3 = p_decode->pixel_fx.arg.belt_fx.type;
                                }
                                break;
                                case RS485_BeltFX_Forest:{
                                    
                                    g_rs485_data.light_mode_state.command_3 = p_decode->pixel_fx.arg.belt_fx.type;
                                }
                                break;
                                case RS485_BeltFX_Bonfire:{
                                    
                                    g_rs485_data.light_mode_state.command_3 = p_decode->pixel_fx.arg.belt_fx.type;
                                }
                                break;
                                case RS485_BeltFX_Brook:{
                                    
                                    g_rs485_data.light_mode_state.command_3 = p_decode->pixel_fx.arg.belt_fx.type;
                                }
                                break;
                                case RS485_BeltFX_Game:{
                                    
                                    g_rs485_data.light_mode_state.command_3 = p_decode->pixel_fx.arg.belt_fx.type;
                                }
                                break;
                                case RS485_BeltFX_Timewait:{
                                    
                                    g_rs485_data.light_mode_state.command_3 = p_decode->pixel_fx.arg.belt_fx.type;
                                }
                                break;
                                case RS485_BeltFX_Party:{
                                    
                                    g_rs485_data.light_mode_state.command_3 = p_decode->pixel_fx.arg.belt_fx.type;
                                }
                                break;
                                case RS485_BeltFX_Belt_Fireworks:{
                                    
                                    g_rs485_data.light_mode_state.command_3 = p_decode->pixel_fx.arg.belt_fx.type;
                                }
                                break;
                                case RS485_BeltFX_Marbles:{
                                    
                                    g_rs485_data.light_mode_state.command_3 = p_decode->pixel_fx.arg.belt_fx.type;
                                }
                                break;
                                case RS485_BeltFX_Pendulum:{
                                    
                                    g_rs485_data.light_mode_state.command_3 = p_decode->pixel_fx.arg.belt_fx.type;
                                }
                                break;
                                case RS485_BeltFX_Flash_Point:{
                                    
                                    g_rs485_data.light_mode_state.command_3 = p_decode->pixel_fx.arg.belt_fx.type;
                                }
                                break;
                                default:{
									ack_body.ack = RS485_Ack_Err_Command;
                                    Rs485_Slave_Ack_Package1(&data_header, &ack_body, p_ack_data, p_ack_offset);
								}break;
                            }
                        }
                        break;
                        case RS485_PixelFX_Music:{
                            
                            g_rs485_data.light_mode_state.command_2 = p_decode->pixel_fx.type;
                        }
                        break;
                        case RS485_PixelFX_Rainbow:{
                            
                            g_rs485_data.light_mode_state.command_2 = p_decode->pixel_fx.type;
                        }
                        break;
                        default:{
							ack_body.ack = RS485_Ack_Err_Command;
                            Rs485_Slave_Ack_Package1(&data_header, &ack_body, p_ack_data, p_ack_offset);
						}break;
                    }
                }
                break;
                case RS485_Cmd_DMX_Strobe:{
                    g_rs485_data.fx_restart = 0;/*光效正常运行*/
                    ack_body.ack = RS485_Ack_Ok;
                    g_effect_struct.effect_arg.dmx_strobe_arg.frq = p_decode->dmx_strobe.frq;
                    switch(p_decode->dmx_strobe.mode){
                        case RS485_DMX_Strobe_Mode_CCT:{
                            g_effect_struct.effect_arg.dmx_strobe_arg.mode = EffectModeCCT;
                            g_effect_struct.effect_arg.dmx_strobe_arg.arg.cct_arg.lightness = (uint16_t)(p_decode->dmx_strobe.mode_arg.cct.intensity * 1000.0f);
                            g_effect_struct.effect_arg.dmx_strobe_arg.arg.cct_arg.cct = p_decode->dmx_strobe.mode_arg.cct.cct;
							#ifdef PR_307_L3
                            g_effect_struct.effect_arg.dmx_strobe_arg.arg.cct_arg.gm = (uint8_t)((p_decode->dmx_strobe.mode_arg.cct.duv)*100);//Phoebe：光效模块的gm是整形的,这样写灯控不能走浮点型给灯体
							#endif
							#ifdef PR_308_L2
                            g_effect_struct.effect_arg.dmx_strobe_arg.arg.cct_arg.gm = (uint8_t)((p_decode->dmx_strobe.mode_arg.cct.duv)*100);//Phoebe：光效模块的gm是整形的,这样写灯控不能走浮点型给灯体
							#endif
                        }
                        break;
#ifdef PR_307_L3
                        case RS485_DMX_Strobe_Mode_HSI:{
                            g_effect_struct.effect_arg.dmx_strobe_arg.mode = EffectModeHSI;
                            g_effect_struct.effect_arg.dmx_strobe_arg.arg.hsi_arg.lightness = (uint16_t)(p_decode->dmx_strobe.mode_arg.hsi.intensity * 1000.0f);
                            g_effect_struct.effect_arg.dmx_strobe_arg.arg.hsi_arg.hue = p_decode->dmx_strobe.mode_arg.hsi.hue / 100;
                            g_effect_struct.effect_arg.dmx_strobe_arg.arg.hsi_arg.sat = (uint8_t)(p_decode->dmx_strobe.mode_arg.hsi.sat / 100);
                            g_effect_struct.effect_arg.dmx_strobe_arg.arg.hsi_arg.cct = p_decode->dmx_strobe.mode_arg.hsi.cct;
                        }
                        break;
                        case RS485_DMX_Strobe_Mode_GEL:{
                            g_effect_struct.effect_arg.dmx_strobe_arg.mode = EffectModeGEL;
                            g_effect_struct.effect_arg.dmx_strobe_arg.arg.gel_arg.lightness = (uint16_t)(p_decode->dmx_strobe.mode_arg.gel.intensity * 1000.0f);
                            g_effect_struct.effect_arg.dmx_strobe_arg.arg.gel_arg.cct = p_decode->dmx_strobe.mode_arg.gel.cct;
                            g_effect_struct.effect_arg.dmx_strobe_arg.arg.gel_arg.brand = p_decode->dmx_strobe.mode_arg.gel.origin;
                            g_effect_struct.effect_arg.dmx_strobe_arg.arg.gel_arg.type = p_decode->dmx_strobe.mode_arg.gel.series;
                            g_effect_struct.effect_arg.dmx_strobe_arg.arg.gel_arg.color = p_decode->dmx_strobe.mode_arg.gel.color;
                        }
                        break;
                        case RS485_DMX_Strobe_Mode_XY:{
                            g_effect_struct.effect_arg.dmx_strobe_arg.mode = EffectModeCoord;
                            g_effect_struct.effect_arg.dmx_strobe_arg.arg.coord_arg.lightness = (uint16_t)(p_decode->dmx_strobe.mode_arg.xy.intensity * 1000.0f);
                            g_effect_struct.effect_arg.dmx_strobe_arg.arg.coord_arg.x = p_decode->dmx_strobe.mode_arg.xy.x;
                            g_effect_struct.effect_arg.dmx_strobe_arg.arg.coord_arg.y = p_decode->dmx_strobe.mode_arg.xy.y;
                        }
                        break;
                        case RS485_DMX_Strobe_Mode_SOUYRCE:{
                            g_effect_struct.effect_arg.dmx_strobe_arg.mode = EffectModeSource;
                            g_effect_struct.effect_arg.dmx_strobe_arg.arg.source_arg.lightness = (uint16_t)(p_decode->dmx_strobe.mode_arg.source.intensity * 1000.0f);
                            g_effect_struct.effect_arg.dmx_strobe_arg.arg.source_arg.type = p_decode->dmx_strobe.mode_arg.source.type;
                            g_effect_struct.effect_arg.dmx_strobe_arg.arg.source_arg.x = p_decode->dmx_strobe.mode_arg.source.x_coord;
                            g_effect_struct.effect_arg.dmx_strobe_arg.arg.source_arg.y = p_decode->dmx_strobe.mode_arg.source.y_coord;
                        }
                        break;
                        case RS485_DMX_Strobe_Mode_RGB:{
                            g_effect_struct.effect_arg.dmx_strobe_arg.mode = EffectModeRGB;
                            g_effect_struct.effect_arg.dmx_strobe_arg.arg.rgb_arg.lightness = (uint16_t)(p_decode->dmx_strobe.mode_arg.rgb.intensity * 1000.0f);
                            g_effect_struct.effect_arg.dmx_strobe_arg.arg.rgb_arg.r = p_decode->dmx_strobe.mode_arg.rgb.red_ratio;
                            g_effect_struct.effect_arg.dmx_strobe_arg.arg.rgb_arg.g = p_decode->dmx_strobe.mode_arg.rgb.green_ratio;
                            g_effect_struct.effect_arg.dmx_strobe_arg.arg.rgb_arg.b = p_decode->dmx_strobe.mode_arg.rgb.blue_ratio;
                        }
                        break;
                        case RS485_DMX_Strobe_Mode_RGBWW:{
							g_effect_struct.effect_arg.dmx_strobe_arg.mode = EffectModeRGBWW;
							g_effect_struct.effect_arg.dmx_strobe_arg.arg.rgbww_arg.lightness = (uint16_t)(p_decode->dmx_strobe.mode_arg.rgbww.intensity * 1000.0f);
                            g_effect_struct.effect_arg.dmx_strobe_arg.arg.rgbww_arg.r = p_decode->dmx_strobe.mode_arg.rgbww.red_ratio;
                            g_effect_struct.effect_arg.dmx_strobe_arg.arg.rgbww_arg.g = p_decode->dmx_strobe.mode_arg.rgbww.green_ratio;
                            g_effect_struct.effect_arg.dmx_strobe_arg.arg.rgbww_arg.b = p_decode->dmx_strobe.mode_arg.rgbww.blue_ratio;
							g_effect_struct.effect_arg.dmx_strobe_arg.arg.rgbww_arg.ww = p_decode->dmx_strobe.mode_arg.rgbww.ww_ratio;
                            g_effect_struct.effect_arg.dmx_strobe_arg.arg.rgbww_arg.cw = p_decode->dmx_strobe.mode_arg.rgbww.cw_ratio;
                        }
                        break;
                        case RS485_DMX_Strobe_Mode_Color_Mixing:{
                            g_effect_struct.effect_arg.dmx_strobe_arg.mode = EffectModeColorMixing;
							g_effect_struct.effect_arg.dmx_strobe_arg.arg.mixing_arg.ratio = p_decode->dmx_strobe.mode_arg.color_mixing.color_1_ratio;
                            switch(p_decode->dmx_strobe.mode_arg.color_mixing.color_1_mode){
                                case RS485_DMX_Strobe_Mode_CCT:{
                                    g_effect_struct.effect_arg.dmx_strobe_arg.arg.mixing_arg.color1_arg.mode = EffectModeCCT;
                                    g_effect_struct.effect_arg.dmx_strobe_arg.arg.mixing_arg.color1_arg.arg.cct_arg.lightness = (uint16_t)(p_decode->dmx_strobe.mode_arg.color_mixing.color_1_arg.cct.intensity * 1000.0f);
                                    g_effect_struct.effect_arg.dmx_strobe_arg.arg.mixing_arg.color1_arg.arg.cct_arg.cct = p_decode->dmx_strobe.mode_arg.color_mixing.color_1_arg.cct.cct;
                                    g_effect_struct.effect_arg.dmx_strobe_arg.arg.mixing_arg.color1_arg.arg.cct_arg.gm = (uint8_t)((p_decode->dmx_strobe.mode_arg.color_mixing.color_1_arg.cct.duv)*100);//(int8_t)p_decode->dmx_strobe.mode_arg.color_mixing.color_1_arg.cct.duv;
                                }
                                break;
                                case RS485_DMX_Strobe_Mode_HSI:{
                                    g_effect_struct.effect_arg.dmx_strobe_arg.arg.mixing_arg.color1_arg.mode = EffectModeHSI;
                                    g_effect_struct.effect_arg.dmx_strobe_arg.arg.mixing_arg.color1_arg.arg.hsi_arg.lightness = (uint16_t)(p_decode->dmx_strobe.mode_arg.color_mixing.color_1_arg.hsi.intensity * 1000.0f);
                                    g_effect_struct.effect_arg.dmx_strobe_arg.arg.mixing_arg.color1_arg.arg.hsi_arg.hue = p_decode->dmx_strobe.mode_arg.color_mixing.color_1_arg.hsi.hue / 100;
                                    g_effect_struct.effect_arg.dmx_strobe_arg.arg.mixing_arg.color1_arg.arg.hsi_arg.sat = (uint8_t)(p_decode->dmx_strobe.mode_arg.color_mixing.color_1_arg.hsi.sat / 100);
                                    g_effect_struct.effect_arg.dmx_strobe_arg.arg.mixing_arg.color1_arg.arg.hsi_arg.cct = p_decode->dmx_strobe.mode_arg.color_mixing.color_1_arg.hsi.cct;
                                }
                                break;
                                case RS485_DMX_Strobe_Mode_GEL:{
                                    g_effect_struct.effect_arg.dmx_strobe_arg.arg.mixing_arg.color1_arg.mode = EffectModeGEL;
                                    g_effect_struct.effect_arg.dmx_strobe_arg.arg.mixing_arg.color1_arg.arg.gel_arg.lightness = (uint16_t)(p_decode->dmx_strobe.mode_arg.color_mixing.color_1_arg.gel.intensity * 1000.0f);
                                    g_effect_struct.effect_arg.dmx_strobe_arg.arg.mixing_arg.color1_arg.arg.gel_arg.cct = p_decode->dmx_strobe.mode_arg.color_mixing.color_1_arg.gel.cct;
                                    g_effect_struct.effect_arg.dmx_strobe_arg.arg.mixing_arg.color1_arg.arg.gel_arg.brand = p_decode->dmx_strobe.mode_arg.color_mixing.color_1_arg.gel.origin;
                                    g_effect_struct.effect_arg.dmx_strobe_arg.arg.mixing_arg.color1_arg.arg.gel_arg.type = p_decode->dmx_strobe.mode_arg.color_mixing.color_1_arg.gel.series;
                                    g_effect_struct.effect_arg.dmx_strobe_arg.arg.mixing_arg.color1_arg.arg.gel_arg.color = p_decode->dmx_strobe.mode_arg.color_mixing.color_1_arg.gel.color;
                                }
                                break;
                                case RS485_DMX_Strobe_Mode_XY:{
                                    g_effect_struct.effect_arg.dmx_strobe_arg.arg.mixing_arg.color1_arg.mode = EffectModeCoord;
                                    g_effect_struct.effect_arg.dmx_strobe_arg.arg.mixing_arg.color1_arg.arg.coord_arg.lightness = (uint16_t)(p_decode->dmx_strobe.mode_arg.color_mixing.color_1_arg.xy.intensity * 1000.0f);
                                    g_effect_struct.effect_arg.dmx_strobe_arg.arg.mixing_arg.color1_arg.arg.coord_arg.x = p_decode->dmx_strobe.mode_arg.color_mixing.color_1_arg.xy.x;
                                    g_effect_struct.effect_arg.dmx_strobe_arg.arg.mixing_arg.color1_arg.arg.coord_arg.y = p_decode->dmx_strobe.mode_arg.color_mixing.color_1_arg.xy.y;
                                }
                                break;
                                case RS485_DMX_Strobe_Mode_SOUYRCE:{
                                    g_effect_struct.effect_arg.dmx_strobe_arg.arg.mixing_arg.color1_arg.mode = EffectModeSource;
                                    g_effect_struct.effect_arg.dmx_strobe_arg.arg.mixing_arg.color1_arg.arg.source_arg.lightness = (uint16_t)(p_decode->dmx_strobe.mode_arg.color_mixing.color_1_arg.source.intensity * 1000.0f);
                                    g_effect_struct.effect_arg.dmx_strobe_arg.arg.mixing_arg.color1_arg.arg.source_arg.type = p_decode->dmx_strobe.mode_arg.color_mixing.color_1_arg.source.type;
                                    g_effect_struct.effect_arg.dmx_strobe_arg.arg.mixing_arg.color1_arg.arg.source_arg.x = p_decode->dmx_strobe.mode_arg.color_mixing.color_1_arg.source.x_coord;
                                    g_effect_struct.effect_arg.dmx_strobe_arg.arg.mixing_arg.color1_arg.arg.source_arg.y = p_decode->dmx_strobe.mode_arg.color_mixing.color_1_arg.source.y_coord;
                                }
                                break;
                                case RS485_DMX_Strobe_Mode_RGB:{
                                    g_effect_struct.effect_arg.dmx_strobe_arg.arg.mixing_arg.color1_arg.mode = EffectModeRGB;
                                    g_effect_struct.effect_arg.dmx_strobe_arg.arg.mixing_arg.color1_arg.arg.rgb_arg.lightness = (uint16_t)(p_decode->dmx_strobe.mode_arg.color_mixing.color_1_arg.rgb.intensity * 1000.0f);
                                    g_effect_struct.effect_arg.dmx_strobe_arg.arg.mixing_arg.color1_arg.arg.rgb_arg.r = p_decode->dmx_strobe.mode_arg.color_mixing.color_1_arg.rgb.red_ratio;
                                    g_effect_struct.effect_arg.dmx_strobe_arg.arg.mixing_arg.color1_arg.arg.rgb_arg.g = p_decode->dmx_strobe.mode_arg.color_mixing.color_1_arg.rgb.green_ratio;
                                    g_effect_struct.effect_arg.dmx_strobe_arg.arg.mixing_arg.color1_arg.arg.rgb_arg.b = p_decode->dmx_strobe.mode_arg.color_mixing.color_1_arg.rgb.blue_ratio;
                                }
                                break;
                                default:{
                                    ack_body.ack = RS485_Ack_Err_Command;
                                }
                                break;
                            }
                            switch(p_decode->dmx_strobe.mode_arg.color_mixing.color_2_mode){
                                case RS485_DMX_Strobe_Mode_CCT:{
                                    g_effect_struct.effect_arg.dmx_strobe_arg.arg.mixing_arg.color2_arg.mode = EffectModeCCT;
                                    g_effect_struct.effect_arg.dmx_strobe_arg.arg.mixing_arg.color2_arg.arg.cct_arg.lightness = (uint16_t)(p_decode->dmx_strobe.mode_arg.color_mixing.color_2_arg.cct.intensity * 1000.0f);
                                    g_effect_struct.effect_arg.dmx_strobe_arg.arg.mixing_arg.color2_arg.arg.cct_arg.cct = p_decode->dmx_strobe.mode_arg.color_mixing.color_2_arg.cct.cct;
                                    g_effect_struct.effect_arg.dmx_strobe_arg.arg.mixing_arg.color2_arg.arg.cct_arg.gm = (uint8_t)((p_decode->dmx_strobe.mode_arg.color_mixing.color_2_arg.cct.duv)*100);//p_decode->dmx_strobe.mode_arg.color_mixing.color_2_arg.cct.duv;
                                }
                                break;
                                case RS485_DMX_Strobe_Mode_HSI:{
                                    g_effect_struct.effect_arg.dmx_strobe_arg.arg.mixing_arg.color2_arg.mode = EffectModeHSI;
                                    g_effect_struct.effect_arg.dmx_strobe_arg.arg.mixing_arg.color2_arg.arg.hsi_arg.lightness = (uint16_t)(p_decode->dmx_strobe.mode_arg.color_mixing.color_2_arg.hsi.intensity * 1000.0f);
                                    g_effect_struct.effect_arg.dmx_strobe_arg.arg.mixing_arg.color2_arg.arg.hsi_arg.hue = p_decode->dmx_strobe.mode_arg.color_mixing.color_2_arg.hsi.hue / 100;
                                    g_effect_struct.effect_arg.dmx_strobe_arg.arg.mixing_arg.color2_arg.arg.hsi_arg.sat = (uint8_t)(p_decode->dmx_strobe.mode_arg.color_mixing.color_2_arg.hsi.sat / 100);
                                    g_effect_struct.effect_arg.dmx_strobe_arg.arg.mixing_arg.color2_arg.arg.hsi_arg.cct = p_decode->dmx_strobe.mode_arg.color_mixing.color_2_arg.hsi.cct;
                                }
                                break;
                                case RS485_DMX_Strobe_Mode_GEL:{
                                    g_effect_struct.effect_arg.dmx_strobe_arg.arg.mixing_arg.color2_arg.mode = EffectModeGEL;
                                    g_effect_struct.effect_arg.dmx_strobe_arg.arg.mixing_arg.color2_arg.arg.gel_arg.lightness = (uint16_t)(p_decode->dmx_strobe.mode_arg.color_mixing.color_2_arg.gel.intensity * 1000.0f);
                                    g_effect_struct.effect_arg.dmx_strobe_arg.arg.mixing_arg.color2_arg.arg.gel_arg.cct = p_decode->dmx_strobe.mode_arg.color_mixing.color_2_arg.gel.cct;
                                    g_effect_struct.effect_arg.dmx_strobe_arg.arg.mixing_arg.color2_arg.arg.gel_arg.brand = p_decode->dmx_strobe.mode_arg.color_mixing.color_2_arg.gel.origin;
                                    g_effect_struct.effect_arg.dmx_strobe_arg.arg.mixing_arg.color2_arg.arg.gel_arg.type = p_decode->dmx_strobe.mode_arg.color_mixing.color_2_arg.gel.series;
                                    g_effect_struct.effect_arg.dmx_strobe_arg.arg.mixing_arg.color2_arg.arg.gel_arg.color = p_decode->dmx_strobe.mode_arg.color_mixing.color_2_arg.gel.color;
                                }
                                break;
                                case RS485_DMX_Strobe_Mode_XY:{
                                    g_effect_struct.effect_arg.dmx_strobe_arg.arg.mixing_arg.color2_arg.mode = EffectModeCoord;
                                    g_effect_struct.effect_arg.dmx_strobe_arg.arg.mixing_arg.color2_arg.arg.coord_arg.lightness = (uint16_t)(p_decode->dmx_strobe.mode_arg.color_mixing.color_2_arg.xy.intensity * 1000.0f);
                                    g_effect_struct.effect_arg.dmx_strobe_arg.arg.mixing_arg.color2_arg.arg.coord_arg.x = p_decode->dmx_strobe.mode_arg.color_mixing.color_2_arg.xy.x;
                                    g_effect_struct.effect_arg.dmx_strobe_arg.arg.mixing_arg.color2_arg.arg.coord_arg.y = p_decode->dmx_strobe.mode_arg.color_mixing.color_2_arg.xy.y;
                                }
                                break;
                                case RS485_DMX_Strobe_Mode_SOUYRCE:{
                                    g_effect_struct.effect_arg.dmx_strobe_arg.arg.mixing_arg.color2_arg.mode = EffectModeSource;
                                    g_effect_struct.effect_arg.dmx_strobe_arg.arg.mixing_arg.color2_arg.arg.source_arg.lightness = (uint16_t)(p_decode->dmx_strobe.mode_arg.color_mixing.color_2_arg.source.intensity * 1000.0f);
                                    g_effect_struct.effect_arg.dmx_strobe_arg.arg.mixing_arg.color2_arg.arg.source_arg.type = p_decode->dmx_strobe.mode_arg.color_mixing.color_2_arg.source.type;
                                    g_effect_struct.effect_arg.dmx_strobe_arg.arg.mixing_arg.color2_arg.arg.source_arg.x = p_decode->dmx_strobe.mode_arg.color_mixing.color_2_arg.source.x_coord;
                                    g_effect_struct.effect_arg.dmx_strobe_arg.arg.mixing_arg.color2_arg.arg.source_arg.y = p_decode->dmx_strobe.mode_arg.color_mixing.color_2_arg.source.y_coord;
                                }
                                break;
                                case RS485_DMX_Strobe_Mode_RGB:{
                                    g_effect_struct.effect_arg.dmx_strobe_arg.arg.mixing_arg.color2_arg.mode = EffectModeRGB;
                                    g_effect_struct.effect_arg.dmx_strobe_arg.arg.mixing_arg.color2_arg.arg.rgb_arg.lightness = (uint16_t)(p_decode->dmx_strobe.mode_arg.color_mixing.color_2_arg.rgb.intensity * 1000.0f);
                                    g_effect_struct.effect_arg.dmx_strobe_arg.arg.mixing_arg.color2_arg.arg.rgb_arg.r = p_decode->dmx_strobe.mode_arg.color_mixing.color_2_arg.rgb.red_ratio;
                                    g_effect_struct.effect_arg.dmx_strobe_arg.arg.mixing_arg.color2_arg.arg.rgb_arg.g = p_decode->dmx_strobe.mode_arg.color_mixing.color_2_arg.rgb.green_ratio;
                                    g_effect_struct.effect_arg.dmx_strobe_arg.arg.mixing_arg.color2_arg.arg.rgb_arg.b = p_decode->dmx_strobe.mode_arg.color_mixing.color_2_arg.rgb.blue_ratio;
                                }
                                break;
                                default:{
                                    ack_body.ack = RS485_Ack_Err_Command;
                                }
                                break;
                            }
                        }
                        break;
#endif
                        default:{
                            ack_body.ack = RS485_Ack_Err_Command;
                        }
                        break;
                    }
                    g_rs485_data.new_command.light_mode = 1;
                    g_rs485_data.light_mode_state.command_1 = data_header.cmd_type;
                    Rs485_Slave_Ack_Package1(&data_header, &ack_body, p_ack_data, p_ack_offset);
                }
                break;
                case RS485_Cmd_Partition_Color:{
                    g_rs485_data.new_command.light_mode = 1;
                    g_rs485_data.light_mode_state.command_1 = data_header.cmd_type;
                }
                break;
                case RS485_Cmd_Partition_Effect:{
                    g_rs485_data.new_command.light_mode = 1;
                    g_rs485_data.light_mode_state.command_1 = data_header.cmd_type;
                }
                break;  
				case RS485_Cmd_Analog_Dim:{
					ack_body.ack = RS485_Ack_Err_Command; 
					Rs485_Slave_Ack_Package1(&data_header, &ack_body, p_ack_data, p_ack_offset);
				}
				break;
				case RS485_Cmd_High_speed:{
					if((g_rs485_data.power_switch.state == 0) || (g_rs485_data.high_speed.mode != p_decode->high_speed_mode.mode))//Phoebe:防止断联的情况下，出现闪灯情况，只有处于模拟调光切pwm才进行缓起和合光初始化
					{
						g_rs485_data.new_command.adjust_hight_mode = 1;
						g_rs485_data.high_speed.mode = p_decode->high_speed_mode.mode;
						
					}
					ack_body.ack = RS485_Ack_Ok; 
					Rs485_Slave_Ack_Package1(&data_header, &ack_body, p_ack_data, p_ack_offset);
				}
				break;
				case RS485_Cmd_Pump:{
                    #if 1
                    g_rs485_data.pump_t.mode       = p_decode->pump_t.mode;
                    g_rs485_data.pump_t.speed      = p_decode->pump_t.speed;
                    g_rs485_data.pump_t.pump_index  = p_decode->pump_t.pump_index;
                    g_rs485_data.pump_t.err_msg    = p_decode->pump_t.err_msg;
                    #endif
                    g_rs485_data.new_command.pump_t = 1;
                    ack_body.ack = RS485_Ack_Ok;
                    Rs485_Slave_Ack_Package1(&data_header, &ack_body, p_ack_data, p_ack_offset);
                }
                break;
				case RS485_Cmd_Self_Adjust:{
                    #if 1
                    g_rs485_data.self_adjust_t.led_color_type       = p_decode->self_adjust_t.led_color_type;
                    g_rs485_data.self_adjust_t.adjust_dir      = p_decode->self_adjust_t.adjust_dir;
                    g_rs485_data.self_adjust_t.adjust_val  = p_decode->self_adjust_t.adjust_val;
                    g_rs485_data.self_adjust_t.save_ctr    = p_decode->self_adjust_t.save_ctr;
                    #endif
                    g_rs485_data.new_command.self_adjust = 1;
                    ack_body.ack = RS485_Ack_Ok;
                    Rs485_Slave_Ack_Package1(&data_header, &ack_body, p_ack_data, p_ack_offset);
                }
                break;
				case RS485_Cmd_Motor:{
					g_rs485_data.motor_t.type = p_decode->motor_t.type;
                    g_rs485_data.motor_t.mode_arg.motor_mode = p_decode->motor_t.mode_arg.motor_mode;
					switch(p_decode->motor_t.mode_arg.motor_mode)
					{
						case RS485_Motor_Mode_Move:{
							g_rs485_data.motor_t.mode_arg.motor_arg.motor_move_arg = p_decode->motor_t.mode_arg.motor_arg.motor_move_arg;
							if( p_decode->motor_t.type == RS485_Pitch_Motor || p_decode->motor_t.type == RS485_Rotale_Motor  )
							{
								if( p_decode->motor_t.type == RS485_Pitch_Motor )
								{
									pitch_last = p_decode->motor_t.mode_arg.motor_arg.motor_move_arg.motor_angle;
								}
									
								if( p_decode->motor_t.type == RS485_Rotale_Motor )
								{
									roll_last = p_decode->motor_t.mode_arg.motor_arg.motor_move_arg.motor_angle;
								}
									
								Electronic_Buf.Bracket_Send_State = Electronic_Idle;
								Electronic_Buf.Bracket_Times = 0;
								Electronic_Buf.Bracket_Angel_Time = 0;
							}
							if( p_decode->motor_t.type == RS485_Fresnel_Motor )
							{
								frensel_last = p_decode->motor_t.mode_arg.motor_arg.motor_move_arg.motor_angle;
								Electronic_Buf.Bayont_Send_State = Electronic_Idle;
								Electronic_Buf.Bayonet_Times = 0;
								Electronic_Buf.Frensel_Angel_Time = 0;
							}
						}
						break;
						case RS485_Motor_Mode_Crc:{
							g_rs485_data.motor_t.mode_arg.motor_arg.motor_crc_arg = p_decode->motor_t.mode_arg.motor_arg.motor_crc_arg;
							if( p_decode->motor_t.type == RS485_Pitch_Motor || p_decode->motor_t.type == RS485_Rotale_Motor  )
							{
								Electronic_Buf.Bracket_Send_State = Electronic_Idle;
							}
							if( p_decode->motor_t.type == RS485_Fresnel_Motor )
							{
								Electronic_Buf.Bayont_Send_State = Electronic_Idle;
							}
						}
						break;
						case RS485_Motor_Mode_Reset:{
							g_rs485_data.motor_t.mode_arg.motor_arg.motor_reset_arg = p_decode->motor_t.mode_arg.motor_arg.motor_reset_arg;
							if( p_decode->motor_t.type == RS485_Pitch_Motor || p_decode->motor_t.type == RS485_Rotale_Motor  )
							{
								Electronic_Buf.Bracket_Send_State = Electronic_Idle;
							}
							Pitch_Roll_Slave[p_decode->motor_t.type].mode_arg.motor_arg.motor_reset_arg.reset_angle = 0;
							Pitch_Roll_Slave[p_decode->motor_t.type].mode_arg.motor_arg.motor_reset_arg.reset_state = 0;
							pitch_last = 90;
							roll_last = 0;
							
						}
						break;
						case RS485_Motor_Mode_State:{
							g_rs485_data.motor_t.mode_arg.motor_arg.motor_state_arg = p_decode->motor_t.mode_arg.motor_arg.motor_state_arg;
						}
						break;
						default:break;
					}
					ack_body.ack = RS485_Ack_Ok;
					memcpy((void*)&Pitch_Roll[p_decode->motor_t.type],(void*)&g_rs485_data.motor_t,sizeof(rs485_motor_t));
					if((p_decode->motor_t.type == RS485_Pitch_Motor || p_decode->motor_t.type == RS485_Rotale_Motor) )
					{
							Electronic_Package_Fuction(Electronic_Buf.W_Bracket_Buf.Electronic_Data,1,Pitch_Roll[p_decode->motor_t.type],1);
							#ifdef ENCODE
							encode_data1 = user_plaintext_encryption(Electronic_Buf.W_Bracket_Buf.Electronic_Data,((rs485_proto_header_t *)(Electronic_Buf.W_Bracket_Buf.Electronic_Data))->msg_size);
							if( encode_data1 != NULL && encode_data1->data_len <= 500 )
							{						
								memcpy(data_pin,Electronic_Buf.W_Bracket_Buf.Electronic_Data,11);
								memcpy(data_pin+11,encode_data1->data,encode_data1->data_len);
								push_electronic_data(&Bracket_Queue,data_pin,encode_data1->data_len+11);
							}
							#else
							if( p_decode->motor_t.mode_arg.motor_mode == RS485_Motor_Mode_Reset )
							{
								DDL_DelayMS(5);
								hal_rs485_2_usart_send(Electronic_Buf.W_Bracket_Buf.Electronic_Data,((rs485_proto_header_t *)(Electronic_Buf.W_Bracket_Buf.Electronic_Data))->msg_size);
								DDL_DelayMS(10);
							}
							#endif

					}
					if(p_decode->motor_t.type == RS485_Fresnel_Motor )
					{
						Electronic_Package_Fuction(Electronic_Buf.W_Bayont_Buf.Electronic_Data,1,Pitch_Roll[p_decode->motor_t.type],1);
						encode_data1 = user_plaintext_encryption(Electronic_Buf.W_Bayont_Buf.Electronic_Data,((rs485_proto_header_t *)(Electronic_Buf.W_Bayont_Buf.Electronic_Data))->msg_size);
						if( encode_data1 != NULL && encode_data1->data_len <= 500 )
						{						
							memcpy(data_pin,Electronic_Buf.W_Bayont_Buf.Electronic_Data,11);
							memcpy(data_pin+11,encode_data1->data,encode_data1->data_len);
							push_electronic_data(&Bayont_Queue,data_pin,encode_data1->data_len+11);
						}
					}
					Rs485_Slave_Ack_Package1(&data_header, &ack_body, p_ack_data, p_ack_offset);
                }
				break;
				case RS485_Cmd_Version2:{
                    ack_body.ack = RS485_Ack_Err_Command;
                    Rs485_Slave_Ack_Package1(&data_header, &ack_body, p_ack_data, p_ack_offset);
                }
				case RS485_Cmd_Factory_RGBWW_Crc:{ 
                    #if 1
                    g_rs485_data.factory_rgbww_crc_t.red = p_decode->factory_rgbww_crc_t.red;
                    g_rs485_data.factory_rgbww_crc_t.green = p_decode->factory_rgbww_crc_t.green;
                    g_rs485_data.factory_rgbww_crc_t.blue =p_decode->factory_rgbww_crc_t.blue;
                    g_rs485_data.factory_rgbww_crc_t.ww = p_decode->factory_rgbww_crc_t.ww;
                    g_rs485_data.factory_rgbww_crc_t.cw = p_decode->factory_rgbww_crc_t.cw;
					g_rs485_data.factory_rgbww_crc_t.ww2 = p_decode->factory_rgbww_crc_t.ww2;
                    g_rs485_data.factory_rgbww_crc_t.cw2 = p_decode->factory_rgbww_crc_t.cw2;
					for(uint8_t i = 0; i < 7; i++)
					{
						g_rs485_data.factory_rgbww_crc_t.state[i] = p_decode->factory_rgbww_crc_t.state[i];
					}
                    #endif
                    g_rs485_data.new_command.light_mode = 1;
                    g_rs485_data.light_mode_state.command_1 = data_header.cmd_type;
                    ack_body.ack = RS485_Ack_Ok;
                    Rs485_Slave_Ack_Package1(&data_header, &ack_body, p_ack_data, p_ack_offset);
                }
                break;
				case RS485_Cmd_Clear_Run_Time:{
                    ack_body.ack = RS485_Ack_Ok;
					if(p_decode->clear_run_time_t.clear_run_time_flag == 1)//清除运行时间
					{
						g_clear_run_time = 1;
					}
                    Rs485_Slave_Ack_Package1(&data_header, &ack_body, p_ack_data, p_ack_offset);
                }
				break;
                default:{
					ack_body.ack = RS485_Ack_Err_Command;
          Rs485_Slave_Ack_Package1(&data_header, &ack_body, p_ack_data, p_ack_offset);
				}break;
            }
        }
        break;
        default:{
            ack_body.ack = RS485_Ack_Err_Command;
            Rs485_Slave_Ack_Package1(&data_header, &ack_body, p_ack_data, p_ack_offset);
        }
        break;
    }
    return ret_val;
}
/*****************************************************************************************
* Function Name: Rs485_Slave_Ack_Package1
* Description  : RS485从机应答数据打包
* Arguments    : 包头地址 - 
                 应答数据地址 - 
                 应答数据指针 - 打包的应答数据存放数组的首地址
                 应答数据偏移 - 指示应答数据的偏移位置
* Return Value : none
******************************************************************************************/
void Rs485_Slave_Ack_Package1(rs485_cmd_header_t* p_header, rs485_ack_arg_t* p_ack_body, uint8_t* p_ack_data, uint16_t* p_ack_offset)
{
    ((rs485_ack_body_t*)(p_ack_data + *p_ack_offset))->header.rw = p_header->rw;
    ((rs485_ack_body_t*)(p_ack_data + *p_ack_offset))->header.cmd_type = p_header->cmd_type;
    ((rs485_ack_body_t*)(p_ack_data + *p_ack_offset))->header.arg_size = sizeof(rs485_ack_arg_t);
    ((rs485_ack_body_t*)(p_ack_data + *p_ack_offset))->ack_arg.ack = p_ack_body->ack;
    *p_ack_offset += sizeof(rs485_ack_body_t);
}
/*****************************************************************************************
* Function Name : sidus_fx_power_cut
* Description   : sidus光效输出功率限制
* Arguments     : 亮度（0-1000），照度比例（0.0-1.0）
* Return Value	: 限制后的亮度值
******************************************************************************************/
static uint16_t sidus_fx_power_cut(uint16_t lightness, float illumination_rate)
{
    if(0 == lightness)
        return 0;
#ifdef PR_307_L3
    if(lightness < 100)
        return 100;
#endif
#ifdef PR_308_L2
    if(lightness < 150)
        return 150;
#endif
    /*风扇静音模式，此比例在0.1左右*/
    if(illumination_rate < 0.4f)
        return lightness;
    if(lightness > 800)
        return 800;
    return lightness;
}
/*****************************************************************************************
* Function Name : set_sys_fx2_light_para
* Description   : 设置二代系统光效的光模式参数
* Arguments     : p_obj：获取数据端
                  p_source：提供数据端
* Return Value	: 0 - 正常， 1 - 异常
******************************************************************************************/
static uint8_t set_sys_fx2_light_para(fx_mode_para_t* p_obj, const rs485_fx_2_mode_arg_t* p_source)
{
    uint8_t ret_val = 0;
    switch(p_obj->mode){
        case RS485_FX_II_Mode_CCT:{
            p_obj->cct = p_source->cct.cct;
            p_obj->duv = p_source->cct.duv;
        }
        break;
        case RS485_FX_II_Mode_HSI:{
            p_obj->hue = p_source->hsi.hue;
            p_obj->sat = p_source->hsi.sat;
            p_obj->cct = p_source->hsi.cct;
        }
        break;
        case RS485_FX_II_Mode_GEL:{
            p_obj->cct    = p_source->gel.cct;
            p_obj->origin = p_source->gel.origin;
            p_obj->series = p_source->gel.series;
            p_obj->color  = p_source->gel.color;
        }
        break;
        case RS485_FX_II_Mode_XY:{
            p_obj->x = p_source->xy.x;
            p_obj->y = p_source->xy.y;
        }
        break;
        case RS485_FX_II_Mode_SOUYRCE:{
            p_obj->type = p_source->source.type;
            p_obj->x    = p_source->source.x_coord;
            p_obj->y    = p_source->source.y_coord;
        }
        break;
        case RS485_FX_II_Mode_RGB:{
            p_obj->red_ratio   = p_source->rgb.red_ratio;
            p_obj->green_ratio = p_source->rgb.green_ratio;
            p_obj->blue_ratio  = p_source->rgb.blue_ratio;
        }
        break;
        case RS485_FX_II_Mode_CCT_Range:{
            p_obj->cct_upper = p_source->cct_range.cct_upper;
            p_obj->cct_lower = p_source->cct_range.cct_lower;
            p_obj->duv       = p_source->cct_range.duv;
        }
        break;
        case RS485_FX_II_Mode_HSI_Range:{
            p_obj->hue_upper = p_source->hsi_range.hue_upper;
            p_obj->hue_lower = p_source->hsi_range.hue_lower;
            p_obj->sat       = p_source->hsi_range.sat;
            p_obj->cct       = p_source->hsi_range.cct;
        }
        break;
        default:{
            ret_val = 1;
        }
        break;
    }
    return ret_val;
}

/***********************************END OF FILE*******************************************/
