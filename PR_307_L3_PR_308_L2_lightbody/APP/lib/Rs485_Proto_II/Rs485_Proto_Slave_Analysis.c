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
#include "stdio.h"
/*****************************************************************************************
							  Variables definitions
*****************************************************************************************/
uint8_t     g_Rs485_Send_Buf[RS485_MESSAGE_MAX_SIZE] = {0};
/*****************************************************************************************
							  Functions definitions
*****************************************************************************************/
void (*g_pRs485ProtoTxCallback)(uint16_t Length, uint8_t *pBuff);
//------------------从机模式
//---------------------------------------------从机解析数据
/*****************************************************************************************
* Function Name: Rs485_Slave_Recv_Port
* Description  : RS485从机接收数据接口（从机串口收到数据后，调用此函数）
* Arguments    : p_data：主机收到的应答数据起始地址
* Return Value : none
******************************************************************************************/
void Rs485_Slave_Recv_Port(uint8_t* p_data)
{
    uint8_t check_state = 0;
    check_state = Rs485_Slave_Data_Check(p_data, g_Rs485_Send_Buf);
    if(0 != check_state){
        /****note start*****/
        
        /****note end*****/
    }
    else{
        Set_Rs485_Ack_Msg(((rs485_proto_header_t*)g_Rs485_Send_Buf)->msg_size, g_Rs485_Send_Buf);
    }
}

//---------------------------------------------共有功能模块化
/*****************************************************************************************
* Function Name: Rs485_Recv_Data_Check
* Description  : RS485主机/从机对接收的数据进行校验
* Arguments    : 接收数据首地址,校验参数
* Return Value : 0 - OK
                 1 - start error
                 2 - message package length error
                 3 - check error
******************************************************************************************/
uint8_t Rs485_Recv_Data_Check(uint8_t* p_data, Rs485_Recv_Check_TypeDef* p_check)
{
    uint8_t     msg_len = 0;
    /*包头判断*/
    if(RS485_START_CODE != ((rs485_proto_header_t*)p_data)->start){
        return 1;
    }
    /*信息包长度判断*/
    msg_len = ((rs485_proto_header_t*)p_data)->header_size;
    p_check->frame_length = ((rs485_proto_header_t*)p_data)->msg_size;
    if((msg_len + 4) > p_check->frame_length || msg_len < 9 || p_check->frame_length > RS485_MESSAGE_MAX_SIZE || p_check->frame_length < 13){
        return 2;
    }
    /*比较主机、从机信息包的最短值，并取最短值*/
    p_check->min_msg_length = msg_len > sizeof(rs485_proto_header_t) ? sizeof(rs485_proto_header_t) : msg_len;
    /*校验和判断*/
    if(((rs485_proto_header_t*)p_data)->check_sum != Rs485_CheckSum(((rs485_proto_header_t*)p_data)->msg_size - RS485_HEADER_NO_CHECK_LEN, p_data + RS485_HEADER_NO_CHECK_LEN)){
        return 3;
    }
    p_check->offset_val += msg_len;
    return 0;
}
/*****************************************************************************************
* Function Name: Rs485_Send_Header_Package
* Description  : RS485主机/从机对发送数据的起始包和信息包打包处理
* Arguments    : 接收数据首地址,打包所需的信息
* Return Value : NONE
******************************************************************************************/
void Rs485_Send_Header_Package(uint8_t* p_data, Rs485_Header_Packet_TypeDef* p_msg)
{
    ((rs485_proto_header_t*)p_data)->header_size = sizeof(rs485_proto_header_t);
    ((rs485_proto_header_t*)p_data)->msg_size = p_msg->frame_length;
    ((rs485_proto_header_t*)p_data)->proto_ver_major = RS485_PROTO_VER_MAJOR;
    ((rs485_proto_header_t*)p_data)->proto_ver_minor = RS485_PROTO_VER_MINOR;
    ((rs485_proto_header_t*)p_data)->proto_ver_revision = RS485_PROTO_VER_REVISION;
    ((rs485_proto_header_t*)p_data)->serial_num = p_msg->frame_sel;
    ((rs485_proto_header_t*)p_data)->ack_en = p_msg->ack_en;
    
    ((rs485_proto_header_t*)p_data)->start = RS485_START_CODE;
    ((rs485_proto_header_t*)p_data)->check_sum = Rs485_CheckSum(p_msg->frame_length - RS485_HEADER_NO_CHECK_LEN, p_data + RS485_HEADER_NO_CHECK_LEN);
}
/*****************************************************************************************
* Function Name: Rs485_CheckSum
* Description  : 计算以buf为起始地址的len个字节的累加教研和，结果存入字节变量sum中().
* Arguments    : NONE
* Return Value : NONE
******************************************************************************************/
uint8_t Rs485_CheckSum(uint16_t len, uint8_t buf[len])
{
    uint32_t sum = 0;
    uint16_t i = 0;
    for(i = 0; i < len; i++)
    {
        sum += buf[i];
    }
    return sum;
}

/***********************************END OF FILE*******************************************/
