/****************************************************************************************
**  Filename :  het_led.c
**  Abstract :  LED模块化程序，支持设置LED常亮，常灭，闪烁周期设置，闪烁次数设置等功能。
**  By       :  王锐彬
**  Date     :  2018-01-01
**  Changelog:1.First Create
*****************************************************************************************/
typedef enum 
{
	RS485_FREE_STATE = 0,
	RS485_LEN,
	RS485_DATA,
	RS485_CRC,
}RS485_Process_TypeDef;

/*****************************************************************************************
									Includes
*****************************************************************************************/
#include "define.h"
#include "base_data_rs485.h" 
#include "delay.h"
#include "data_acc.h"
#include "stdio.h"
//#include "app_led.h"
/*****************************************************************************************
							  Variables definitions
*****************************************************************************************/
uint8_t Rs485TxBuf[32] = {0};
uint8_t Rs485RxBuf[32] = {0};
//static uint8_t s_base_data_ack_en = 0;
/*****************************************************************************************
							  Functions definitions
*****************************************************************************************/
//static void Recv_Rs485_Callback(uint8_t *pBuff,uint16_t len);
/*****************************************************************************************
* Function Name : Rs485_Msg_Init
* Description   : DMX模块底层元素初始化，包含硬件初始化、驱动层结构体初始化；
* Arguments     : void
* Return Value	: void
******************************************************************************************/
void Base_Data_Rs485_Msg_Init(void)
{
	//TITO:
//    g_pRsBaseDataRxCallback = Recv_Rs485_Callback;
}
/*****************************************************************************************
* Function Name: Send_Rs485_Data
* Description  : 
* Arguments    : NONE
* Return Value : NONE
******************************************************************************************/
void Base_Data_Send_Rs485_Data(void)     
{
	//TITO:
//	if(0 == s_base_data_ack_en)
//		return;
//	s_base_data_ack_en = 0;
//	for(uint8_t i = 0; i < 32; i++){
////		printf("%d", Rs485TxBuf[i]);
//		while(RESET == usart_flag_get(USART0, USART_FLAG_TC));
//		usart_data_transmit(USART0, Rs485TxBuf[i]);
//	}
}
/*****************************************************************************************
* Function Name: Recv_Rs485_Callback
* Description  : RS485串口接收数据回调函数
* Arguments    : NONE
* Return Value : NONE
******************************************************************************************/
/*----------HSI：0x01------------*/
typedef __packed struct {

    uint16_t  INT;              /*亮度<0-1000>*/
    uint16_t  HUE;              /*hue<0-360>，0-360,360-0*/
    uint8_t   SAT;              /*饱和度<0-100>*/
    uint8_t   Fade;             /*Fade，Fade控制模式<0-1>，0表示缓变，1表示使用瞬变*/
    uint16_t  CCT;              /*中心色温*/
    uint16_t  Channel;          /*点控通道，0表示全控，n表示通道n，默认0*/
} HSI_Body_TypeDef;                 /*size: 6*/
/*----------CCT：0x02-----------*/
typedef __packed struct {

    uint16_t  INT;              /*亮度<0-1000>*/
    uint16_t  CCT;              /*CCT，单位k*/
    uint8_t   GM;               /*<0-20>: 0->-1.0,1->-0.9,2->-0.8,   10->0,20->+1.0每隔0.1递增1，以此类推*/
    uint8_t   Fade;             /*Fade，Fade控制模式<0-1>，0表示缓变，1表示使用瞬变*/
    uint16_t  Channel;          /*点控通道，0表示全控，n表示通道n，默认0*/
} CCT_Body_TypeDef;                 /*size: 6*/
/*----------GEL：0x03-----------*/
typedef __packed struct {

    uint16_t  INT;              /*亮度<0-1000>*/
    uint16_t  CCT;              /*CCT，单位k*/
    uint8_t   Gel_Origin;       /*Gel_Origin ，品牌，0x00：LEE; 0x01：Resco*/
    uint8_t   Gel_Type;         /*Gel_Type*/
    uint16_t  Gel_Color;        /*Gel_Color*/
    uint8_t   Fade;             /*Fade，Fade控制模式<0-1>，0表示缓变，1表示使用瞬变*/
    uint16_t  Channel;          /*点控通道，0表示全控，n表示通道n，默认0*/
} Gel_Body_TypeDef;                 /*size: 9*/
/*----------RGBWW：0x04---------*/
typedef __packed struct {

    uint16_t  Red_INT;          /*红灯亮度<0-1000>*/
    uint16_t  Green_INT;        /*绿灯亮度<0-1000>*/
    uint16_t  Blue_INT;         /*蓝灯亮度<0-1000>*/
    uint16_t  WW_INT;           /*暖白亮度<0-1000>*/
    uint16_t  CW_INT;           /*冷白亮度<0-1000>*/
    uint8_t   Fade;             /*Fade，Fade控制模式<0-1>，0表示缓变，1表示使用瞬变*/
    uint16_t  INT;              /*总亮度<0-1000>*/
    uint16_t  Channel;          /*点控通道，0表示全控，n表示通道n，默认0*/
} RGBWW_Body_TypeDef;               /*size: 11*/

/*----------Factory_RGBWW：0x11-------*/
typedef __packed struct {

    uint16_t  Red_PWM;          /*红灯pwm<0-65535>*/
    uint16_t  Green_PWM;        /*绿灯pwm<0-65535>*/
    uint16_t  Blue_PWM;         /*蓝灯pwm<0-65535>*/
    uint16_t  WW_PWM;           /*暖白pwm<0-65535>*/
    uint16_t  CW_PWM;           /*冷白pwm<0-65535>*/
} Factory_RGBWW_Body_TypeDef;
//static void Recv_Rs485_Callback(uint8_t *pBuff,uint16_t len)
//{
//    static RS485_Process_TypeDef frame_status = RS485_FREE_STATE;
//    static uint8_t data_sel = 0;
//    static uint8_t data_len = 0;					 			
//    switch(frame_status)
//    {
//        case RS485_FREE_STATE:
//            if(*pBuff != 0x55)
//                break;
//            data_sel = 0;
//            frame_status = RS485_LEN;
//            Rs485RxBuf[data_sel++] = *pBuff;
//        break;
//        case RS485_LEN:
//            Rs485RxBuf[data_sel++] = *pBuff;
//            data_len = *pBuff;
//            frame_status = RS485_DATA;
//            if(data_len != 32)
//            {
//                data_sel = 0;
//                frame_status = RS485_FREE_STATE;
//            }
//            break;
//        case RS485_DATA:
//            Rs485RxBuf[data_sel++] = *pBuff;
//            if(data_sel >= (data_len - 1))
//            {
//                frame_status = RS485_CRC;
//            }
//            break;
//        case RS485_CRC:	
//            #if 1
//            Rs485RxBuf[data_sel] = *pBuff;
//			if(Rs485RxBuf[data_sel] == ByteSum(data_sel, Rs485RxBuf))
//			{
//				memcpy(Rs485TxBuf, Rs485RxBuf, 32);
//				if(0x55 == Rs485TxBuf[0] && 0x01 == Rs485TxBuf[3]){
//					if(0X04 == Rs485TxBuf[4]){/*RGB*/
//						//s_base_data_ack_en = 1;
//                        g_rs485_data.new_command.light_mode = 1;
//                        g_rs485_data.light_mode_state.command_1 = RS485_Cmd_RGB;
//                        
////                        g_rs485_data.light_mode_para.intensity = ((RGBWW_Body_TypeDef*)&Rs485TxBuf[5])->INT;
////                        g_rs485_data.light_mode_para.intensity = g_rs485_data.light_mode_para.intensity / 1000.f;
//                        g_rs485_data.light_mode_para.intensity = 0.5f;
//                        g_rs485_data.light_mode_para.red_ratio = ((RGBWW_Body_TypeDef*)&Rs485TxBuf[5])->Red_INT;
//                        g_rs485_data.light_mode_para.green_ratio = ((RGBWW_Body_TypeDef*)&Rs485TxBuf[5])->Green_INT;
//                        g_rs485_data.light_mode_para.blue_ratio = ((RGBWW_Body_TypeDef*)&Rs485TxBuf[5])->Blue_INT;
//                        g_rs485_data.light_mode_para.fade = 0;
//					}
//					else if(0X11 == Rs485TxBuf[4]){/*FACTORY RGBWW*/
//						//s_base_data_ack_en = 1;
//                        g_rs485_data.new_command.light_mode = 1;
//                        g_rs485_data.light_mode_state.command_1 = RS485_Cmd_Factory_RGBWW;
//                        
//						g_rs485_data.light_mode_para.red_ratio = ((Factory_RGBWW_Body_TypeDef*)&Rs485TxBuf[5])->Red_PWM;
//						g_rs485_data.light_mode_para.green_ratio = ((Factory_RGBWW_Body_TypeDef*)&Rs485TxBuf[5])->Green_PWM;
//						g_rs485_data.light_mode_para.blue_ratio = ((Factory_RGBWW_Body_TypeDef*)&Rs485TxBuf[5])->Blue_PWM;
//						g_rs485_data.light_mode_para.ww_ratio = ((Factory_RGBWW_Body_TypeDef*)&Rs485TxBuf[5])->WW_PWM;
//						g_rs485_data.light_mode_para.cw_ratio = ((Factory_RGBWW_Body_TypeDef*)&Rs485TxBuf[5])->CW_PWM;
//					}
//                    else if(0X02 == Rs485TxBuf[4]){/*cct*/
//                        //s_base_data_ack_en = 1;
//                        g_rs485_data.new_command.light_mode = 1;
//                        g_rs485_data.light_mode_state.command_1 = RS485_Cmd_CCT;
//                        
//                        
//                        g_rs485_data.light_mode_para.intensity = ((CCT_Body_TypeDef*)&Rs485TxBuf[5])->INT;
//                        g_rs485_data.light_mode_para.intensity = g_rs485_data.light_mode_para.intensity / 1000.f;
//                        g_rs485_data.light_mode_para.cct = ((CCT_Body_TypeDef*)&Rs485TxBuf[5])->CCT;
//                        g_rs485_data.light_mode_para.duv = ((CCT_Body_TypeDef*)&Rs485TxBuf[5])->GM;
//                        g_rs485_data.light_mode_para.fade = 0;
//                    }
//                    else if(0X01 == Rs485TxBuf[4]){/*hsi*/
//                        //s_base_data_ack_en = 1;
//                        g_rs485_data.new_command.light_mode = 1;
//                        g_rs485_data.light_mode_state.command_1 = RS485_Cmd_HSI;
//                        
//                        g_rs485_data.light_mode_para.intensity = ((HSI_Body_TypeDef*)&Rs485TxBuf[5])->INT;
//                        g_rs485_data.light_mode_para.intensity = g_rs485_data.light_mode_para.intensity / 1000.f;
//                        g_rs485_data.light_mode_para.hue = ((HSI_Body_TypeDef*)&Rs485TxBuf[5])->HUE;
//                        g_rs485_data.light_mode_para.sat = ((HSI_Body_TypeDef*)&Rs485TxBuf[5])->SAT;
//                        g_rs485_data.light_mode_para.fade = 0;
//                    }
//                    else if(0X03 == Rs485TxBuf[4]){/*gel*/
//                        //s_base_data_ack_en = 1;
//                        g_rs485_data.new_command.light_mode = 1;
//                        g_rs485_data.light_mode_state.command_1 = RS485_Cmd_GEL;
//                        #if 0
//                        ((Gel_Body_TypeDef*)&Rs485TxBuf[5])->INT;
//                        ((Gel_Body_TypeDef*)&Rs485TxBuf[5])->CCT;
//                        ((Gel_Body_TypeDef*)&Rs485TxBuf[5])->Gel_Origin;
//                        ((Gel_Body_TypeDef*)&Rs485TxBuf[5])->Gel_Type;
//                        ((Gel_Body_TypeDef*)&Rs485TxBuf[5])->Gel_Color;
//                        g_rs485_data.light_mode_para.fade = 0;
//                        #endif
//                    }
//					//s_base_data_ack_en = 1;
//				}
//			}
//			else
//			{

//			} 
//            data_sel = 0;
//            frame_status = RS485_FREE_STATE;
//            #endif
//            break;
//        default:
//            break;
//    }
//}
/***********************************END OF FILE*******************************************/
