/****************************************************************************************
**  Filename :  111.h
**  Abstract :  。
**  By       :  何建国
**  Date     :  
**  Changelog:1.First Create
*****************************************************************************************/
#ifndef _RS485_PROTO_SLAVE_CORE_H
#define _RS485_PROTO_SLAVE_CORE_H
/*****************************************************************************************
									Includes
*****************************************************************************************/
#include "Rs485_Proto_Slave_Analysis.h"
#include "rs485_protocol.h"
#include "stdlib.h"
/*****************************************************************************************
								Macro definitions
*****************************************************************************************/

/*****************************************************************************************
								Typedef definitions
*****************************************************************************************/
typedef __packed struct{
	uint16_t                offset_val;             /*偏移量*/                   
	uint16_t                frame_length;           /*帧长度*/
	uint8_t                 min_msg_length;         /*最小信息包长度*/
}Rs485_Recv_Check_TypeDef;

typedef __packed struct{
    uint16_t                frame_length;           /*帧长度*/
    uint16_t                frame_sel;              /*帧序号，循环码*/
    uint8_t                 ack_en;                 /*应答使能*/
}Rs485_Header_Packet_TypeDef;
/*****************************************************************************************
							Global variables and functions
*****************************************************************************************/

//----------------------------------------------------------------------------------------
extern uint8_t Rs485_Slave_Data_Check(uint8_t* p_recv_data, uint8_t* p_ack_data);
extern uint8_t Rs485_Recv_Data_Check(uint8_t* p_data, Rs485_Recv_Check_TypeDef* p_check);
extern uint8_t Rs485_CheckSum(uint16_t len, uint8_t buf[len]);
extern void Rs485_Send_Header_Package(uint8_t* p_data, Rs485_Header_Packet_TypeDef* p_msg);
extern void Set_Rs485_Ack_Msg(uint16_t data_len, uint8_t *data);
#endif
/***********************************END OF FILE*******************************************/
