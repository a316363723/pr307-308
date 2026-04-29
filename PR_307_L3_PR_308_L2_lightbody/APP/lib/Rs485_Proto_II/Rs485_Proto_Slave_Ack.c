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
#include "Rs485_Proto_Slave_Core.h"
#include "define.h"
#include "stdio.h"
/*****************************************************************************************
							  Variables definitions
*****************************************************************************************/
static uint16_t s_rs485_ack_enable = 0;
static uint16_t s_rs4845_ack_length = 0;
/*****************************************************************************************
							  Functions definitions
*****************************************************************************************/

/*****************************************************************************************
* Function Name: Set_Rs485_Ack_Msg
* Description  : 从机接收并处理完接收的数据后，打包应答数据然后调用此函数设置相关应答信息
* Arguments    : NONE
* Return Value : NONE
******************************************************************************************/
void Set_Rs485_Ack_Msg(uint16_t data_len, uint8_t *data)  
{
    s_rs485_ack_enable = 1;
    s_rs4845_ack_length = data_len;
}
/*****************************************************************************************
* Function Name: RS485_Ack_Ctr
* Description  : 485应答逻辑(1ms时间片)
* Arguments    : NONE
* Return Value : NONE
******************************************************************************************/
void RS485_Ack_Ctr(void)
{
    rs485_ack_enum ack_state = RS485_Ack_Ok;
    if(g_rs485_ack_wait.delay_time > 0){
        g_rs485_ack_wait.delay_time--;
        ack_state = g_rs485_ack_wait.wait_ack();

        if(RS485_Ack_Err_Busy != ack_state){
            g_rs485_ack_wait.ack_type = ack_state;
            g_Rs485_Send_Buf[g_rs485_ack_wait.ack_slot] = g_rs485_ack_wait.ack_type;
            /*校验和重新计算,能这种操作的前提是默认应答状态是ACK_OK，并且ACK_OK对应的值为0*/
            ((rs485_proto_header_t*)g_Rs485_Send_Buf)->check_sum += g_rs485_ack_wait.ack_type;
            g_rs485_ack_wait.delay_time = 0;
            #if(1 == GLOBAL_PRINT_ENABLE)
//            printf("aks.\n");
            #endif
        }
        else{
            if(0 == g_rs485_ack_wait.delay_time){
                g_Rs485_Send_Buf[g_rs485_ack_wait.ack_slot] = RS485_Ack_Err_Timeout;
                /*校验和重新计算,能这种操作的前提是默认应答状态是ACK_OK，并且ACK_OK对应的值为0*/
                ((rs485_proto_header_t*)g_Rs485_Send_Buf)->check_sum += RS485_Ack_Err_Timeout;
            }
        }
        return;
    }
    else{
        if(1 == s_rs485_ack_enable){
            s_rs485_ack_enable = 0;
            /*这个回调函数对应串口数据发送函数*/
            if(NULL != g_pRs485ProtoTxCallback){
                g_pRs485ProtoTxCallback(s_rs4845_ack_length, g_Rs485_Send_Buf);
                #if(1 == GLOBAL_PRINT_ENABLE)
//                printf("ak.\n");
                #endif
            }
        }
    }
}
/***********************************END OF FILE*******************************************/
