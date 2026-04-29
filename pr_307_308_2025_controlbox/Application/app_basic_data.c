#include "app_basic_data.h"
#include "stdint.h"
#include "hc32_ddl.h"
#include "stdio.h"
#include "UI_Data.h"
#include "page_manager.h"
#include "app_power.h"
#include "page.h"
#include "crc32.h"
#include "dev_rs485.h"
#include "hal_uart.h"
#include "perf_counter.h"
#include "update_module.h"

#define PACKET_START			(0x55)  //°üÍ·Ä¬ÈÏ0x55
#define PACKET_LENGTH			(0x20)	//Êý¾Ý°ü³¤¶È
#define PROTOCOL_VER			(0x23)	//Ð­Òé°æ±¾v2.3
#define COMMAND_READ			(0x00)	//¶Á
#define COMMAND_WRITE			(0x01)	//Ð´
#define CommandBody_Max     	(26)	//Êý¾Ý°üCommandBody³¤¶È

/*-------------------------------------- Header --------------------------------------------------*/

static osSemaphoreId_t base_data_tx_semaphore_id;
static const char* dev_name = {USB_UPDATA_LAMP_DEVICE};
static const char* firmware_name = {"lamp_firmware.bin"};
static bool base_updata_state;


typedef enum {
	
	STATUS_OK = 0,
	STATUS_UNSUPPORTED_COMMAND,
	STATUS_ILLEGAL_STATE,
	STATUS_VERIFICATION_FAILED,
	STATUS_INVALID_IMAGE,
	STATUS_INVALID_IMAGE_SIZE,
	STATUS_MORE_DATA,
	STATUS_INVALID_APPID,
	STATUS_INVALID_VERSION,
	STATUS_CONTINUE
}Upgrad_STATUS_Type;

/*---ÃüÁîÀàÐÍ---*/
typedef enum {

    CommandType_Version = 0,
    CommandType_HSI,
    CommandType_CCT,
    CommandType_GEL,
    CommandType_RGBWW,
    CommandType_XY_Coordinate,
    CommandType_LED_Frequency,
    CommandType_Light_Effect,
    CommandType_Dimming_Curve,
    CommandType_Fan_Speed,
    CommandType_Get_Power,
    CommandType_Board_State,
    CommandType_Sleep_Mode,
    CommandType_Upgrade,
    CommandType_PacketErr,
    CommandType_Main_Interface,
    CommandType_Beat,
    CommandType_Factory_RGBWW,
    CommandType_Source,
    CommandType_Illumination_Mode,
    CommandType_Color_Mixing,
    CommandType_Control_State,
    CommandType_PFX_Ctrl,
    CommandType_CFX_Bank_RW,
    CommandType_CFX_Ctrl,
    CommandType_CFX_Preview,
    CommandType_MFX_Ctrl,
    CommandType_File_Transfer,
    CommandType_RGBWW_1,
    CommandType_CFX_Name,
	CommandType_DMX_Strobe,
	CommandType_USER_CUSTOM = 0xFD,
    CommandType_Factory_CCT = 0xFE,
    CommandType_NULL,
} CommandType_TypeDef;

typedef __packed struct {

    uint8_t   Start;            /*±íÊ¾Í¨ÐÅ°üÆðÊ¼Êý¾Ý£¬Ä¬ÈÏÖµÎª0x55*/
    uint8_t   Length;           /*Õû¸öÊý¾Ý°ü³¤¶È£¨°üº¬°üÍ·ºÍÐ£Ñé×Ö½Ú£©£¬¹Ì¶¨³¤¶È32byte*/
    uint8_t   ProtocolVer;      /*Í¨ÐÅÐ­Òé°æ±¾£¬ÀýÈç0x23±íÊ¾V2.3*/
    uint8_t   RW;               /*±íÊ¾¶Á»òÐ´,0x00£º±íÊ¾¶Á; 0x01£º±íÊ¾Ð´*/
    CommandType_TypeDef   CommandType;        /*ÃüÁîÀàÐÍ*/
} Header_TypeDef;                 /*size: 5*/
/*----------CCT£º0x02-----------*/
typedef __packed struct {

    uint16_t  INT;              /*ÁÁ¶È<0-1000>*/
    uint16_t  CCT;              /*CCT£¬µ¥Î»k*/
    uint8_t   GM;               /*<0-20>: 0->-1.0,1->-0.9,2->-0.8,   10->0,20->+1.0Ã¿¸ô0.1µÝÔö1£¬ÒÔ´ËÀàÍÆ*/
    uint8_t   Fade;             /*Fade£¬Fade¿ØÖÆÄ£Ê½<0-1>£¬0±íÊ¾»º±ä£¬1±íÊ¾Ê¹ÓÃË²±ä*/
	uint16_t  Channel;			/*µã¿ØÍ¨µÀ£¬0±íÊ¾È«¿Ø£¬n±íÊ¾Í¨µÀn£¬Ä¬ÈÏ0*/
} CCT_Body_TypeDef;                 /*size: 6*/

/*-Effect HSI-*/
typedef __packed struct {

    uint16_t  INT;              /*äº®åº¦<0-1000>*/
    uint16_t  HUE;              /*hue<0-360>ï¼Œ0-360,360-0*/
    uint8_t   SAT;              /*é¥±å’Œåº¦<0-100>*/
} Effect_HSI_TypeDef;             /*size: 5*/

typedef __packed struct {

    uint16_t  Red_PWM;          /*ºìµÆpwm<0-65535>*/
    uint16_t  Green_PWM;        /*ÂÌµÆpwm<0-65535>*/
    uint16_t  Blue_PWM;         /*À¶µÆpwm<0-65535>*/
    uint16_t  WW_PWM;           /*Å¯°×pwm<0-65535>*/
    uint16_t  CW_PWM;           /*Àä°×pwm<0-65535>*/
} Factory_RGBWW_Body_TypeDef;

typedef __packed struct {
    uint8_t  type;          
} Factory_User_Body_TypeDef;

typedef __packed union {

	CCT_Body_TypeDef                    CCT_Body;
	Effect_HSI_TypeDef					HSI_Body;
    Factory_RGBWW_Body_TypeDef          Factory_RGBWW_Body;
   Factory_User_Body_TypeDef            User_Body; 
    uint8_t   DefaultBody_Buffer[CommandBody_Max];   //¶¨ÒåÄ¬ÈÏCommandBody³¤¶È£¬CommandBody_Max
} CommandBody_TypeDef;


/****************************************Éý¼¶Êý¾Ý°ü******************************************/
typedef enum {

    CODE_NONE = 0,
    CODE_CMD,
    CODE_LEN,
    CODE_DATA,
    CODE_VERIFY,
    CODE_STATUS
} Upgrade_OperCode;

typedef enum {

    CMD_NONE = 0,
    CMD_START,
    CMD_ABORT
} Upgrad_Cmd_Type;

typedef __packed struct {

    uint8_t len;
    uint32_t seq_number;
    uint8_t data[128];
} Upgrad_Data_Proto;

typedef __packed union {

    Upgrad_Cmd_Type     cmd;
    uint32_t            length;
    Upgrad_Data_Proto   data;
    uint32_t            crc32;
    Upgrad_STATUS_Type status;
} Upgrad_Body_Arg;

typedef __packed struct {

    Upgrade_OperCode    code;
    Upgrad_Body_Arg     arg;
} Upgrade_Body_TypeDef;

typedef __packed struct {

    Header_TypeDef          Header;            /*Í¨ÐÅÊý¾Ý°ü°üÍ·*/
    Upgrade_Body_TypeDef    CommandBody;    /*Í¨ÐÅÊý¾Ý°üÊý¾Ý²¿·Ö*/
    uint8_t                 CheckSum;        /*Êý¾ÝÐ£ÑéºÍ*/
} Upgrade_Packet_TypeDef;

typedef __packed struct {

    Header_TypeDef          Header;            /* Í¨ÐÅÊý¾Ý°ü°üÍ·*/
    CommandBody_TypeDef     CommandBody;    /*Í¨ÐÅÊý¾Ý°üÊý¾Ý²¿·Ö*/
    uint8_t                 CheckSum;        /*Êý¾ÝÐ£ÑéºÍ*/
} Packet_TypeDef;

/*Éý¼¶Êý¾ÝÓ¦´ð°ü*/
typedef __packed struct{
	
	Header_TypeDef			Header;     	/*Í¨ÐÅÊý¾Ý°ü°üÍ·*/
	Upgrad_STATUS_Type		status;      	
	uint8_t 				CheckSum;   	/*Êý¾ÝÐ£ÑéºÍ*/
}Upgrade_Ack_TypeDef;

Upgrade_Ack_TypeDef ack_packet = {
		
		.Header = {
			.Start = PACKET_START,
			.Length = 7,
			.ProtocolVer = PROTOCOL_VER,
			.RW = COMMAND_WRITE,
			.CommandType = CommandType_Upgrade
		},	
}; 

bool get_base_updata_state(void) 
{
	return base_updata_state;
}	

void set_base_updata_state(bool state) 
{
	base_updata_state = state;

}	

rs485_pack_type  base_data_pack = {0};

static uint8_t rs485_proto_check_sum_calc(const uint8_t* p_data, uint8_t num)
{
    uint8_t check_sum = 0;

    while(num--)
    {
        check_sum += *p_data++;
    }
	__NOP();
    return check_sum;
	
}

static void base_data_rx_complete_irq_callback(void)
{  
    hal_uart_receive_dma_channel_stop(HAL_UART_DMX); 
    base_data_pack.length = sizeof(base_data_pack.data) - hal_uart_get_receive_dma_cnt(HAL_UART_DMX);
    hal_uart_receive_dma_start(HAL_UART_DMX, base_data_pack.data, sizeof(base_data_pack.data));
    hal_uart_receive_dma_channel_start(HAL_UART_DMX); 
}

static void base_data_dmx_tx_complete_irq_cb(void)
{
    osSemaphoreRelease(base_data_tx_semaphore_id);
}

void  base_data_upgrade_ack_pack(uint8_t status)
{
    int32_t sl;
   
    osSemaphoreAcquire(base_data_tx_semaphore_id, 0);
	ack_packet.status = (Upgrad_STATUS_Type)status;
	ack_packet.CheckSum = rs485_proto_check_sum_calc((uint8_t *)&ack_packet, ack_packet.Header.Length - 1);  //Ð£ÑéÖµ 
	hal_dmx_write_ctrl_pin(HAL_UART_DMX, 1);
    hal_uart_transmit_dma_start(HAL_UART_DMX, (uint8_t*)&ack_packet, ack_packet.Header.Length);
    hal_uart_irq_register(HAL_UART_RS485, HAL_UART_IRQ_TX_DMA_FT, base_data_dmx_tx_complete_irq_cb);
    osSemaphoreAcquire(base_data_tx_semaphore_id, (ack_packet.Header.Length / 10) + 2);
//    memset(&base_data_pack, 0 , sizeof(base_data_pack));
    sl = osKernelLock();
	delay_us(200);
	osKernelRestoreLock(sl);
    hal_dmx_write_ctrl_pin(HAL_UART_DMX, 0);
	
}

void  base_data_general_ack_pack(void)
{
    int32_t sl;
    hal_dmx_write_ctrl_pin(HAL_UART_DMX, 1);
    osSemaphoreAcquire(base_data_tx_semaphore_id, 0);
    hal_uart_transmit_dma_start(HAL_UART_DMX, (uint8_t*)&base_data_pack.data, base_data_pack.length);
    hal_uart_irq_register(HAL_UART_RS485, HAL_UART_IRQ_TX_DMA_FT, base_data_dmx_tx_complete_irq_cb);
    osSemaphoreAcquire(base_data_tx_semaphore_id, (base_data_pack.length/ 10) + 2);
    memset(&base_data_pack, 0 , sizeof(base_data_pack));
    sl = osKernelLock();
	delay_us(200);
	osKernelRestoreLock(sl);
    hal_dmx_write_ctrl_pin(HAL_UART_DMX, 0);
	
}

void app_update_dubug_handle(void)
{
    #if _base_data_ 
	
	uint8_t  res = STATUS_OK;
    Upgrade_Packet_TypeDef * p_base_data = (Upgrade_Packet_TypeDef*)&base_data_pack.data;
    if(base_data_pack.length == 140 && p_base_data->CheckSum == rs485_proto_check_sum_calc(base_data_pack.data, base_data_pack.length - 1))
    {
		  set_base_updata_state(1);
          if(p_base_data->Header.CommandType == CommandType_Upgrade)
        {
            switch(p_base_data->CommandBody.code)
            {
                case 1:
					if(0 != update_open(dev_name, firmware_name))
					{
						set_base_updata_state(0);
						res = STATUS_UNSUPPORTED_COMMAND;
					}
					base_data_upgrade_ack_pack(res);
                    break;
                case 2:
					 if(0 != update_erase(dev_name, 0, p_base_data->CommandBody.arg.length))
					{
						set_base_updata_state(0);
						res = STATUS_ILLEGAL_STATE;
					}
					base_data_upgrade_ack_pack(res);
                    break;
                case 3:
					 if(0 != update_write(dev_name, p_base_data->CommandBody.arg.data.seq_number,p_base_data->CommandBody.arg.data.data,p_base_data->CommandBody.arg.data.len))
					{
						res = STATUS_MORE_DATA;
						set_base_updata_state(0);
						base_data_upgrade_ack_pack(res);
					}
                    break;
                case 4:
					 if(0 != update_close(dev_name,(uint32_t* )p_base_data->CommandBody.arg.crc32))
					{
						res = STATUS_VERIFICATION_FAILED;
					}
					base_data_upgrade_ack_pack(res);
					set_base_updata_state(0);
                    break;
                default:
                    
                    break;
            }
            
        }
		 memset(&base_data_pack, 0 , sizeof(base_data_pack));
    }

    #endif
    
}


uint8_t debug_base_data_collect_cb(rs485_cmd_body_t *p_body )
{
    #if _base_data_ 
    uint8_t res = 0;
    Packet_TypeDef* p_rs485 = (Packet_TypeDef*)&base_data_pack.data;

    if(base_data_pack.length == 32  && p_rs485->CheckSum == rs485_proto_check_sum_calc(base_data_pack.data, base_data_pack.length - 1))
    {  
		set_base_updata_state(0);
        if(p_rs485->Header.CommandType == CommandType_Factory_RGBWW || p_rs485->Header.CommandType == CommandType_RGBWW)
        {
            p_body->header.cmd_type = RS485_Cmd_Factory_RGBWW;
			#if PROJECT_TYPE==307
			p_body->cmd_arg.factory_rgbww.red =  p_rs485->CommandBody.Factory_RGBWW_Body.Red_PWM;
            p_body->cmd_arg.factory_rgbww.green =  p_rs485->CommandBody.Factory_RGBWW_Body.Green_PWM;
            p_body->cmd_arg.factory_rgbww.blue =  p_rs485->CommandBody.Factory_RGBWW_Body.Blue_PWM;
			p_body->cmd_arg.factory_rgbww.ww =  p_rs485->CommandBody.Factory_RGBWW_Body.WW_PWM;
            p_body->cmd_arg.factory_rgbww.cw =  p_rs485->CommandBody.Factory_RGBWW_Body.CW_PWM;
            p_body->cmd_arg.factory_rgbww.ww2 = p_rs485->CommandBody.Factory_RGBWW_Body.WW_PWM;
            p_body->cmd_arg.factory_rgbww.cw2 = p_rs485->CommandBody.Factory_RGBWW_Body.CW_PWM;
			#endif
			#if PROJECT_TYPE==308
//			p_body->cmd_arg.factory_rgbww.ww =  p_rs485->CommandBody.Factory_RGBWW_Body.Red_PWM;
//            p_body->cmd_arg.factory_rgbww.green =  p_rs485->CommandBody.Factory_RGBWW_Body.Green_PWM;
//            p_body->cmd_arg.factory_rgbww.cw =  p_rs485->CommandBody.Factory_RGBWW_Body.Blue_PWM;
//			p_body->cmd_arg.factory_rgbww.red =  0;
//            p_body->cmd_arg.factory_rgbww.blue =  0;
			p_body->cmd_arg.factory_rgbww.red =  p_rs485->CommandBody.Factory_RGBWW_Body.Red_PWM;
            p_body->cmd_arg.factory_rgbww.green =  p_rs485->CommandBody.Factory_RGBWW_Body.Green_PWM;
            p_body->cmd_arg.factory_rgbww.blue =  p_rs485->CommandBody.Factory_RGBWW_Body.Blue_PWM;
            p_body->cmd_arg.factory_rgbww.ww =  p_rs485->CommandBody.Factory_RGBWW_Body.WW_PWM;
            p_body->cmd_arg.factory_rgbww.cw =  p_rs485->CommandBody.Factory_RGBWW_Body.CW_PWM;
			p_body->cmd_arg.factory_rgbww.ww2 = 12000;
            p_body->cmd_arg.factory_rgbww.cw2 = 12000;
			#endif
            res =  1;
        }
        else if(p_rs485->Header.CommandType == CommandType_CCT || p_rs485->Header.CommandType == CommandType_Factory_CCT)
        {
            p_body->header.cmd_type = RS485_Cmd_CCT;
            p_body->cmd_arg.cct.intensity = p_rs485->CommandBody.CCT_Body.INT / 1000.0f;
            p_body->cmd_arg.cct.fade = 0;
            p_body->cmd_arg.cct.cct = p_rs485->CommandBody.CCT_Body.CCT;
			#if PROJECT_TYPE==307
			p_body->cmd_arg.cct.duv =  (p_rs485->CommandBody.CCT_Body.GM - 10) * 0.002;
			#elif PROJECT_TYPE==308
			if(p_rs485->CommandBody.CCT_Body.GM <= 10)
				p_body->cmd_arg.cct.duv =  (p_rs485->CommandBody.CCT_Body.GM - 10) * 0.0005;
			else 
				p_body->cmd_arg.cct.duv =  (p_rs485->CommandBody.CCT_Body.GM - 10) * 0.00065;
			#endif
            res =  1;
        }
        else if(p_rs485->Header.CommandType == CommandType_USER_CUSTOM)
		{
			p_body->header.cmd_type = RS485_Cmd_user_custom;
			p_body->cmd_arg.user_custom_t.type = p_rs485->CommandBody.User_Body.type;
			res =  1;
		}
        else 
		{
			res =  0;
			base_data_general_ack_pack();
		}
    }
    app_update_dubug_handle();
    return res;
    
    #endif
}


void base_data_uart_config(void)
{
    hal_dmx_write_ctrl_pin(HAL_UART_DMX, 0);
	hal_uart_set_stopbit(HAL_UART_DMX, USART_STOPBIT_1BIT);
    hal_uart_init(HAL_UART_DMX,115200);
    hal_uart_receive_dma_start(HAL_UART_DMX, base_data_pack.data, sizeof(base_data_pack.data));
    hal_uart_irq_register(HAL_UART_DMX, HAL_UART_IRQ_RX_TIMEOUT, base_data_rx_complete_irq_callback);	
    hal_uart_irq_register(HAL_UART_DMX, HAL_UART_IRQ_TX_DMA_FT, base_data_dmx_tx_complete_irq_cb);
}


void base_data_collect_init(void)
{
    #if _base_data_ 
    base_data_uart_config();
    base_data_tx_semaphore_id = osSemaphoreNew (1, 0, NULL);
    #endif
}


