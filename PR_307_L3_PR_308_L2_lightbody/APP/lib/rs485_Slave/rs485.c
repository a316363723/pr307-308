/****************************************************************************************
**  Filename :  het_led.c
**  Abstract :  LED模块化程序，支持设置LED常亮，常灭，闪烁周期设置，闪烁次数设置等功能。
**  By       :  王锐彬
**  Date     :  2018-01-01
**  Changelog:1.First Create
*****************************************************************************************/

/*****************************************************************************************
									Includes
*****************************************************************************************/
#include "define.h"
#include "rs485.h" 
#include "delay.h"
#include "bsp_rs485.h"
#include "data_acc.h"
#include "Rs485_Proto_Slave_Analysis.h"
/*****************************************************************************************
							  Variables definitions
*****************************************************************************************/

/*****************************************************************************************
							  Functions definitions
*****************************************************************************************/
static void Recv_Rs485_Callback(uint8_t *pBuff,uint16_t len);
static void Send_Rs485_Over_Callback(void);
/*****************************************************************************************
* Function Name: Rs485_Ctr_GpioInit
* Description  : RS485除串口引脚外的其他引脚初始化
* Arguments    : NONE
* Return Value : NONE
******************************************************************************************/
void Rs485_Ctr_GpioInit(void)
{

}
/*****************************************************************************************
* Function Name : Rs485_Msg_Init
* Description   : DMX模块底层元素初始化，包含硬件初始化、驱动层结构体初始化；
* Arguments     : void
* Return Value	: void
******************************************************************************************/
void Rs485_Msg_Init(void)
{
    g_pRs485RxCallback = Recv_Rs485_Callback;
	g_pRs485TxCallback = Send_Rs485_Over_Callback;
    g_pRs485ProtoTxCallback = Send_Rs485_Data;
}
/*****************************************************************************************
* Function Name: Send_Rs485_Data
* Description  : 
* Arguments    : NONE
* Return Value : NONE
******************************************************************************************/
void Send_Rs485_Data(uint16_t data_len, uint8_t *data)     
{
	rx485_send(data, data_len);	
}
/*****************************************************************************************
* Function Name: Recv_Rs485_Callback
* Description  : RS485串口接收数据回调函数
* Arguments    : NONE
* Return Value : NONE
******************************************************************************************/
static void Recv_Rs485_Callback(uint8_t *pBuff,uint16_t len)
{	
    if(0 != len){
        Rs485_Slave_Recv_Port(pBuff);
    }
}
/*****************************************************************************************
* Function Name: Send_Rs485_Over_Callback
* Description  : RS485串口发送数据回调函数
* Arguments    : NONE
* Return Value : NONE
******************************************************************************************/
static void Send_Rs485_Over_Callback(void)
{
//    delay_us(100);
//	RS485_EN_Out((FlagStatus)RS485_EN_RX);
}
/***********************************END OF FILE*******************************************/
