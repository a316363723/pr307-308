/**** A P P L I C A T I O N   N O T E   ************************************
*
* Title			: DMX512 reception, RDM responder library
* Version		: v1.1
* Last updated	: 28.08.2010
* Target		: Transceiver Rev.3.01 [ATmega8515]
* Clock			: 8MHz, 16MHz
*
* written by hendrik hoelscher, www.hoelscher-hi.de
* edit by Markus Bechtold Markus@r-bechtold.de
***************************************************************************
 This program is free software; you can redistribute it and/or 
 modify it under the terms of the GNU General Public License 
 as published by the Free Software Foundation; either version2 of 
 the License, or (at your option) any later version. 

 This program is distributed in the hope that it will be useful, 
 but WITHOUT ANY WARRANTY; without even the implied warranty of 
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU 
 General Public License for more details. 

 If you have no copy of the GNU General Public License, write to the 
 Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA. 

 For other license models, please contact the author.

;***************************************************************************/
 
/*****************************************************************************************
									Includes
*****************************************************************************************/
#include "app_rdm.h"
#include "security.h"
#include "string.h"
#include "CRC32.h"
#include "app_data_center.h"
#include "os_event.h"
#include "hal_uart.h"
#include "app_dmx.h"
//#include "dev_wire_dmx.h"
/*****************************************************************************************
							  Variables definitions
*****************************************************************************************/
enum {IDLE, BRK, SB_DMX, STARTADR_DMX, SB_RDM, SSC_RDM, LEN_RDM, CHECKH_RDM, CHECKL_RDM};			//Rx states

#define UID_CS (0xFF *6 +UID_0 +UID_1 +UID_2 +UID_3 +UID_4 +UID_5)
uint8_t DevID[] = {UID_0, UID_1, UID_2, UID_3, UID_4, UID_5};
uint8_t g_rdm_tx_buf[RDM_DATA_NUM] = {0};
//uint8_t g_rdm_rx_buf[RDM_DATA_NUM] = {0};
RDM_Logic_TypeDef g_rdm_ctr = {
    .mute_flag = 0,
    //.recv_flag = 0,
};
uint16or8	 RxCount_16;						//start address and check sum counting

//设备描述
Device_Info_TypeDef s_device_info_msg = {
    .Rdm_Protocol = 0x0100,
    .device_model_id = 0x0000,
    .product_category = 0x01FF,
    .soft_ver_id = 0x01000001,
    .dmx512_footprint = 0x0001,
    .dmx512_personality = 0x0113,
    .dmx512_start_addr = 0x0001,
    .sub_device_count = 0x0000,
    .sensor_count = 0x0000,
};

const PID_Para_TypeDef s_suppord_pid[SUPPORT_PID_NUM] = {
    {STATUS_MESSAGES>>8, STATUS_MESSAGES&0xFF},
    {DEVICE_INFO>>8, DEVICE_INFO&0xFF},
    {MANUFACT_LABEL>>8, MANUFACT_LABEL&0xFF},
    {DEVICE_LABEL>>8, DEVICE_LABEL&0xFF}, 
    {DMX_PERSONALITY>>8, DMX_PERSONALITY&0XFF},
    {DMX_PERSONALITY_DESCRIPTION>>8, DMX_PERSONALITY_DESCRIPTION&0XFF},
    {DMX_START_ADDRESS>>8, DMX_START_ADDRESS&0xFF},
    //    {SLOT_INFO>>8, SLOT_INFO&0xFF},
    {LAMP_HOURS>>8, LAMP_HOURS&0xFF},
    {IDENTIFY>>8, IDENTIFY&0xFF},
};
const uint8_t s_slot_info[] = {
	//Slot 0
	0x00,0x00,0x00,
	(SD_COLOR_ADD_RED>>8), (SD_COLOR_ADD_RED &0xFF),
	//Slot 1
	0x00,0x01,0x00,
	(SD_COLOR_ADD_GREEN>>8), (SD_COLOR_ADD_GREEN &0xFF),
	//Slot 2
	0x00,0x02,0x00,
	(SD_COLOR_ADD_BLUE>>8), (SD_COLOR_ADD_BLUE &0xFF)
};
const uint8_t ProductDetail[] = {0x04, 0x00}; //LED

const uint8_t StatusMsg[] = {
0, 0,														//sub device ID
STATUS_NONE, 0,												//device state
STS_READY,													//error ID
(SD_UNDEFINED>>8), (SD_UNDEFINED &0xFF),  					//Parameter 1
0, 0														//Parameter 2
};

//---------------------------使用者需要根据实际项目更改
const char s_manufacturer_lable[32] = "Aputure";
char s_device_lable[32] = "fledible light";
const char s_software_ver[32] = "V1.0";

//下面这个参数是选择DMX模型的，AP-107 AP-179有，其他的灯只有一个模型，所以只需要保留一条  //TODO
const Dmx_Profile_Msg_TypeDef s_dmx_profile[DMX_ALL_PROFILE_NUM] = {
	{2, "1:Lighting 2Channels"},
	{5, "2:Effects 5Channels"},
	{6, "3:Lighting&Effects 6Channels"},
};
//----------------------------end
//--------------------回调函数，需要使用者实现
//寻灯
void (*g_pRdmIdentifyCallback)(uint8_t flag) = NULL;
//DMX起始地址
void (*g_pRdmSetStartAddrCallback)(uint16_t addr) = NULL;
uint16_t (*g_pRdmGetStartAddrCallback)(void)= NULL;
//DMX模型
void (*g_pRdmSetProfileCallback)(uint16_t profile)= NULL;
uint16_t (*g_pRdmGetProfileCallback)(void)= NULL;
//获取灯体工作总时间
uint32_t (*g_pRdmGetLampHousCallback)(void)= NULL;
//--------------------end

/*****************************************************************************************
							  Functions definitions
*****************************************************************************************/
static uint8_t isLower(RDM_Packet_TypeDef* p_rdm);
static uint8_t isHigher(RDM_Packet_TypeDef* p_rdm);
void Package_Disc_Unique_Respond_Msg(RDM_Packet_TypeDef* p_rdm);
static uint8_t Get_Respond_Flag(RDM_Packet_TypeDef* p_rdm);
static uint8_t Swip_UUID(RDM_Packet_TypeDef* p_rdm);
static void Package_Normal_Respond_Msg(RDM_Packet_TypeDef* p_rdm);
static void Send_Normal_Ack_Msg(RDM_Packet_TypeDef* p_rdm);


/*****************************************************************************************
* Function Name : Rdm_IdentifyCtr
* Description   : RDM寻灯
* Arguments     : 1 - 使能、0 - 关闭
* Return Value	: none
******************************************************************************************/
static void rdm_identify_ctr(uint8_t flag)
{

	if (1 == flag) 
	{
		os_ev_publish_event(MAIN_EV_LIGHT, EV_DATA_LIGHT_TRIGGRT, 0);  //开始寻灯
	}
	else		
	{   
		os_ev_publish_event(MAIN_EV_LIGHT, EV_DATA_LIGHT_TRIGGRT, 1);  //停止寻灯		
	}
}

/*****************************************************************************************
* Function Name : Rdm_SetDmxStartAddr
* Description   : RDM模块设置DMX起始地址值
* Arguments     : none
* Return Value	: none
******************************************************************************************/
static void rdm_dmx_addr_set(uint16_t addr)
{
	uint16_t dmx_addr= addr;
	
	if(addr <= 512 && addr > 0) 
	{
		data_center_write_config_data(SYS_CONFIG_DMX_ADDR, &dmx_addr, 1);
		//通知刷屏
	
	}
}

/*****************************************************************************************
* Function Name : Rdm_GetDmxStartAddr
* Description   : RDM模块获取DMX起始地址值
* Arguments     : none
* Return Value	: none
******************************************************************************************/
static uint16_t rdm_dmx_addr_get(void)
{
	uint16_t dmx_addr= 0;
	
	data_center_read_config_data(SYS_CONFIG_DMX_ADDR, &dmx_addr);
	return dmx_addr;
}

/*****************************************************************************************
* Function Name : RdmSetProfile
* Description   : RDM模块设置DMX模型值，同时要更新DMX模型菜单显示状态值
* Arguments     : none
* Return Value	: none
******************************************************************************************/
static void rdm_profile_set(uint16_t profile)
{
	uint8_t dmx_profile_index = profile;
	
	data_center_write_config_data(SYS_CONFIG_DMX_PROFILE, &dmx_profile_index, 1);
	//通知刷屏
}

/*****************************************************************************************
* Function Name : RdmGetProfile
* Description   : RDM模块获取DMX模型值
* Arguments     : none
* Return Value	: none
******************************************************************************************/
static uint16_t rdm_profile_get(void)
{
	uint8_t dmx_profile_index = 0;
	
	data_center_read_config_data(SYS_CONFIG_DMX_PROFILE, &dmx_profile_index);
	return dmx_profile_index;
}


static void Rdm_CallbackInit(void)
{
	g_pRdmIdentifyCallback = rdm_identify_ctr;
	g_pRdmSetStartAddrCallback = rdm_dmx_addr_set;
	g_pRdmGetStartAddrCallback = rdm_dmx_addr_get;
	g_pRdmSetProfileCallback = rdm_profile_set;
	g_pRdmGetProfileCallback = rdm_profile_get;
}
/*****************************************************************************************
* Function Name : init_RDM
* Description   : RDM Reception Initialisation
* Arguments     : NONE
* Return Value	: NONE
******************************************************************************************/
void init_RDM(void)
{	
	uint8_t buf[12] = {0};
    uint32_t chip_id = 0;
    memcpy(buf, (uint8_t*)CHIP_ID_ADDRESS, 12);   
    chip_id = CRC32_Calculate(0x00000000, buf, 12);

    DevID[2] = (chip_id>>24) & 0xff;
    DevID[3] = (chip_id>>16) & 0xff;
    DevID[4] = (chip_id>>8) & 0xff;
    DevID[5] = chip_id & 0xff;
	Rdm_CallbackInit();
}
///*****************************************************************************************
//* Function Name : Set_RdmData
//* Description   : 串口接收到数据后调用此函数
//* Arguments     : 数据首地址、数据长度
//* Return Value	: NONE
//******************************************************************************************/
//void Set_RdmData(uint8_t* p_data, uint16_t len)
//{
//	memcpy(g_rdm_rx_buf, p_data, len);
//}
/*****************************************************************************************
* Function Name : Dispose_Recv_Rdm_Data
* Description   : 处理接收到的RDM数据；
* Arguments     : NONE
* Return Value	: 目标数据
******************************************************************************************/
void dispose_recv_rdm_data(uint8_t* rec_data,uint16_t rec_size)
{
	uint16_t cmd_byte = 0;
    RDM_Packet_TypeDef *rdm = (RDM_Packet_TypeDef*)rec_data;
    RDM_Packet_TypeDef *rdm_tx = (RDM_Packet_TypeDef*)g_rdm_tx_buf;

	//UID匹配
	if (0 == Get_Respond_Flag(rdm)) 										//is packet for us?
	{
		memcpy(g_rdm_tx_buf, rec_data, sizeof(g_rdm_tx_buf));
		cmd_byte = Swap_Short(rdm->PID);
		switch(cmd_byte)
		{
			case DISC_UNIQUE_BRANCH:
			{
				if((isLower(rdm) == 0) && (isHigher(rdm) == 0))	//is UID in disc branch?
				{
					Package_Disc_Unique_Respond_Msg(rdm_tx);							//answer discovery msg
				}
			}
			break;	
			case DISC_MUTE:									//we shall shut up
			{
				g_rdm_ctr.mute_flag = 1;
				if(0 != Swip_UUID(rdm_tx))
					break;
				rdm_tx->PortID = RESPONSE_TYPE_ACK;
				rdm_tx->MsgCount = 0;
				rdm_tx->SubDev = 0X0000;
				rdm_tx->SubDev = Swap_Short(rdm_tx->SubDev);
				rdm_tx->PDLen = 2;
				rdm_tx->Data[0]= 0;
				rdm_tx->Data[1]= 0;
				Package_Normal_Respond_Msg(rdm_tx);
				Send_Normal_Ack_Msg(rdm_tx);
			}
			break;
			case DISC_UN_MUTE:								//we shall respond again
			{
				g_rdm_ctr.mute_flag = 0;
				//判断是否是广播，如果不是交换源目标UID
				if(0 != Swip_UUID(rdm_tx))
					break;
				rdm_tx->PortID = RESPONSE_TYPE_ACK;
				rdm_tx->MsgCount = 0;
				rdm_tx->SubDev = 0X0000;
				rdm_tx->SubDev = Swap_Short(rdm_tx->SubDev);
				rdm_tx->PDLen = 2;
				rdm_tx->Data[0]= 0;
				rdm_tx->Data[1]= 0;
				Package_Normal_Respond_Msg(rdm_tx);
				Send_Normal_Ack_Msg(rdm_tx);
			}
			break;
			case IDENTIFY:	
			{       
				if(0 == g_rdm_ctr.mute_flag)
					break;
				if(0 != Swip_UUID(rdm_tx))
					break;
				rdm_tx->PortID = RESPONSE_TYPE_ACK;
				rdm_tx->MsgCount = 0;
				
				if (rdm->Cmd == SET_CMD)
				{
					if(NULL != g_pRdmIdentifyCallback){
						g_pRdmIdentifyCallback(rdm->Data[0]);
					}
					rdm_tx->PDLen= 0;
				}
				else
				{				  
					rdm_tx->Data[0]= 0;
					rdm_tx->PDLen= 1;
				}
				Package_Normal_Respond_Msg(rdm_tx);
				Send_Normal_Ack_Msg(rdm_tx);
			}
			break;
			case DMX_START_ADDRESS:							//start address...
			{
				if(0 == g_rdm_ctr.mute_flag)
					break;
				if(0 != Swip_UUID(rdm_tx))
					break;
				rdm_tx->PortID = RESPONSE_TYPE_ACK;
				rdm_tx->MsgCount = 0;
				if (rdm->Cmd == SET_CMD)
				{
					rdm_tx->PDLen = 0;
					uint16or8 AdrBuf;
					AdrBuf.u8h= rdm->Data[0];
					AdrBuf.u8l= rdm->Data[1];
					if(NULL != g_pRdmSetStartAddrCallback){
						g_pRdmSetStartAddrCallback(AdrBuf.u16);
					}
				}
				else
				{
					rdm_tx->PDLen = 2;
					if(NULL != g_pRdmGetStartAddrCallback){
					uint16_t start_addr = g_pRdmGetStartAddrCallback();
					rdm_tx->Data[0]= start_addr>>8;
					rdm_tx->Data[1]= start_addr&0x00ff;
					}
				}
				Package_Normal_Respond_Msg(rdm_tx);
				Send_Normal_Ack_Msg(rdm_tx);
			}
			break;
			case DEVICE_INFO:
			{
				Device_Info_TypeDef* p_data = (Device_Info_TypeDef*)(&rdm_tx->Data[0]);
				uint16_t dmx_profile_sel = 0;
				uint16_t dmx_start_addr = 1;
				if(NULL != g_pRdmGetProfileCallback){
					dmx_profile_sel = g_pRdmGetProfileCallback();
				}
				if(NULL != g_pRdmGetStartAddrCallback){
					dmx_start_addr = g_pRdmGetStartAddrCallback();
				}
				if(0 == g_rdm_ctr.mute_flag)
					break;
				if(0 != Swip_UUID(rdm_tx))
					break;
				rdm_tx->PortID = RESPONSE_TYPE_ACK;
				rdm_tx->MsgCount = 0;
				rdm_tx->PDLen = 0x13;
				
				
				p_data->Rdm_Protocol = Swap_Short(s_device_info_msg.Rdm_Protocol);
				p_data->device_model_id = Swap_Short(s_device_info_msg.device_model_id);
				p_data->product_category = Swap_Short(s_device_info_msg.product_category);
				p_data->soft_ver_id = Swap_Int(s_device_info_msg.soft_ver_id);
				
				s_device_info_msg.dmx512_footprint = s_dmx_profile[dmx_profile_sel].slots;
				p_data->dmx512_footprint = Swap_Short(s_device_info_msg.dmx512_footprint);
				
				s_device_info_msg.dmx512_personality = (dmx_profile_sel + 1) * 256 + 9;
				p_data->dmx512_personality = Swap_Short(s_device_info_msg.dmx512_personality);
				
				s_device_info_msg.dmx512_start_addr = dmx_start_addr;
				p_data->dmx512_start_addr = Swap_Short(s_device_info_msg.dmx512_start_addr);
				p_data->sub_device_count = Swap_Short(s_device_info_msg.sub_device_count);
				p_data->sensor_count = Swap_Short(s_device_info_msg.sensor_count);
				
				Package_Normal_Respond_Msg(rdm_tx);
				Send_Normal_Ack_Msg(rdm_tx);
			}
			break;
			case DEVICE_LABEL:
			{
				if(0 == g_rdm_ctr.mute_flag)
					break;
				if(0 != Swip_UUID(rdm_tx))
					break;
				rdm_tx->PortID = RESPONSE_TYPE_ACK;
				rdm_tx->MsgCount = 0;
				
				if (rdm->Cmd == SET_CMD)
				{
					if (rdm->PDLen > 32) 
						rdm->PDLen= 32;	
					memcpy(s_device_lable, &(rdm->Data), rdm->PDLen);
					rdm_tx->PDLen = 0;
					s_device_lable[rdm->PDLen] = 0;//控制器下发的字符序列没有结束标志
				}
				else
				{
					rdm_tx->PDLen= strlen(s_device_lable);
					memcpy(rdm_tx->Data, s_device_lable, rdm_tx->PDLen);
				}
				Package_Normal_Respond_Msg(rdm_tx);
				Send_Normal_Ack_Msg(rdm_tx);
			}
			break;
			case MANUFACT_LABEL:
			{
				if(0 == g_rdm_ctr.mute_flag)
					break;
				if(0 != Swip_UUID(rdm_tx))
					break;

				rdm_tx->PortID = RESPONSE_TYPE_ACK;
				rdm_tx->MsgCount = 0;
				rdm_tx->PDLen= strlen(s_manufacturer_lable);
				memcpy(rdm_tx->Data, s_manufacturer_lable, rdm_tx->PDLen);
				Package_Normal_Respond_Msg(rdm_tx);
				Send_Normal_Ack_Msg(rdm_tx);
			}
			break;
			case PRODUCT_DETAIL_ID_LIST:
			{
				
			}
			break;
			case SLOT_INFO:
			{
				if(0 == g_rdm_ctr.mute_flag)
					break;
				if(0 != Swip_UUID(rdm_tx))
					break;
				rdm_tx->PortID = RESPONSE_TYPE_ACK;
				rdm_tx->MsgCount = 0;
				rdm_tx->PDLen = sizeof(s_slot_info);
				memcpy(&(rdm_tx->Data), s_slot_info, sizeof(s_slot_info));
				Package_Normal_Respond_Msg(rdm_tx);
				Send_Normal_Ack_Msg(rdm_tx);
			}
			break;  
			case STATUS_MESSAGES:
			{
				
			}
			break;
			case SUPPORTED_PARAMETERS:
			{
				if(0 == g_rdm_ctr.mute_flag)
					break;
				if(0 != Swip_UUID(rdm_tx))
					break;
				rdm_tx->PortID = RESPONSE_TYPE_ACK;
				rdm_tx->MsgCount = 0;
				rdm_tx->PDLen = sizeof(s_suppord_pid);
				memcpy(&(rdm_tx->Data),s_suppord_pid, sizeof(s_suppord_pid));
				Package_Normal_Respond_Msg(rdm_tx);
				Send_Normal_Ack_Msg(rdm_tx);
			}
			break;
			case DMX_PERSONALITY:
			{
				if(0 == g_rdm_ctr.mute_flag)
					break;
				if(0 != Swip_UUID(rdm_tx))
					break;
				rdm_tx->PortID = RESPONSE_TYPE_ACK;
				rdm_tx->MsgCount = 0;
				
				if (rdm->Cmd == SET_CMD)
				{
					rdm_tx->PDLen = 0;
					if(rdm_tx->Data[0] < (DMX_ALL_PROFILE_NUM +1)  && rdm_tx->Data[0] > 0){
						if(NULL != g_pRdmSetProfileCallback){
							g_pRdmSetProfileCallback(rdm_tx->Data[0] - 1);
						}
					}
				}
				else
				{
					rdm_tx->PDLen = 2;
					if(NULL != g_pRdmGetProfileCallback){
						rdm_tx->Data[0] = g_pRdmGetProfileCallback() + 1;
					}
					else{
						rdm_tx->Data[0] = 1;
					}
					rdm_tx->Data[1] = DMX_ALL_PROFILE_NUM;
				}
				Package_Normal_Respond_Msg(rdm_tx);
				Send_Normal_Ack_Msg(rdm_tx);
				
			}
			break;
			case DMX_PERSONALITY_DESCRIPTION:
			{
				uint16_t profile_request = 0;
				if(0 == g_rdm_ctr.mute_flag)
					break;
				if(0 != Swip_UUID(rdm_tx))
					break;
				rdm_tx->PortID = RESPONSE_TYPE_ACK;
				rdm_tx->MsgCount = 0;
				profile_request = rdm_tx->Data[0] > 0 ? rdm_tx->Data[0] - 1 : rdm_tx->Data[0];
				rdm_tx->PDLen = 3 + strlen((char*)s_dmx_profile[profile_request].text);

				rdm_tx->Data[1] = s_dmx_profile[profile_request].slots>>8; 
				rdm_tx->Data[2] = s_dmx_profile[profile_request].slots&0xFF;
				memcpy(&(rdm_tx->Data[3]), s_dmx_profile[profile_request].text, strlen((char*)s_dmx_profile[profile_request].text));
				Package_Normal_Respond_Msg(rdm_tx);
				Send_Normal_Ack_Msg(rdm_tx);
			}
			break;
			case DEVICE_HOURS:
			{

			}
			break;
			case LAMP_HOURS:
			{

			}
			break;
			case SOFTWARE_VERSION_LABEL:
			{
				if(0 == g_rdm_ctr.mute_flag)
					break;
				if(0 != Swip_UUID(rdm_tx))
					break;
				rdm_tx->PortID = RESPONSE_TYPE_ACK;
				rdm_tx->MsgCount = 0;
				rdm_tx->PDLen= strlen(s_software_ver);
				memcpy(rdm_tx->Data, s_software_ver, rdm_tx->PDLen);
				Package_Normal_Respond_Msg(rdm_tx);
				Send_Normal_Ack_Msg(rdm_tx);
			}
			break;
			case DEVICE_MODEL_DESCRIPTION:
			{
				if(0 == g_rdm_ctr.mute_flag)
					break;
				if(0 != Swip_UUID(rdm_tx))
					break;
				rdm_tx->PortID = RESPONSE_TYPE_ACK;
				rdm_tx->MsgCount = 0;
				rdm_tx->PDLen= strlen(s_device_lable);
				memcpy(rdm_tx->Data, s_device_lable, rdm_tx->PDLen);
				Package_Normal_Respond_Msg(rdm_tx);
				Send_Normal_Ack_Msg(rdm_tx);
			}
			break;
			default:break;	
		}
	}
}
/*****************************************************************************************
* Function Name : Swap_Short
* Description   : 双字节数据大端小端模式转换；
* Arguments     : 源数据
* Return Value	: 目标数据
******************************************************************************************/
uint16_t Swap_Short(uint16_t val)
{
	uint16_t res  = (val<<8);
	res |= (val>>8);
	return (res);
}
/*****************************************************************************************
* Function Name : Swap_Int
* Description   : 4字节数据大端小端模式转换；
* Arguments     : 源数据
* Return Value	: 目标数据
******************************************************************************************/
uint32_t Swap_Int(uint32_t val)
{
	val = ((val << 8) & 0xFF00FF00) | ((val >> 8) & 0x00FF00FF);
    return (val >> 16) | (val << 16);
}
/*****************************************************************************************
* Function Name : isLower
* Description   : 接收到数据的目标下限UUID与本机的UUID比较，如果本机UUID小于下限UUID，则
说明不在范围内，返回1；
* Arguments     : 源数据
* Return Value	: 0 - 在范围内/1 - 不在范围内
******************************************************************************************/
static uint8_t isLower(RDM_Packet_TypeDef* p_rdm)
{
    uint8_t* p_data = (uint8_t*)(&p_rdm->Data[0]);
    
	for (uint8_t i = 0; i < 6; i++)
	{
		if(p_data[i] < DevID[i]) 
            return 0;
		else if(p_data[i] > DevID[i]) 
            return 1;
	}
	return 0;
}
/*****************************************************************************************
* Function Name : isHigher
* Description   : 接收到数据的目标上限UUID与本机的UUID比较，如果本机UUID大于下限UUID，则
说明不在范围内，返回1；
* Arguments     : 源数据
* Return Value	: 0 - 在范围内/1 - 不在范围内
******************************************************************************************/
static uint8_t isHigher(RDM_Packet_TypeDef* p_rdm)
{
	uint8_t* p_data = (uint8_t*)(&p_rdm->Data[6]);
	
	for (uint8_t i = 0; i < 6; i++)
	{
		if(p_data[i] > DevID[i]) 
            return 0;
		else if(p_data[i] < DevID[i]) 
            return 1;
	}
	return 0;
}
#include "perf_counter.h"
/*****************************************************************************************
* Function Name : Package_Disc_Unique_Respond_Msg
* Description   : 设备应答控制器查找设备的信息，如果允许应答，则同时发送出去
* Arguments     : 源数据
* Return Value	: NONE
******************************************************************************************/
void Package_Disc_Unique_Respond_Msg(RDM_Packet_TypeDef* p_rdm)
{
	int32_t sl = osKernelLock();
    int32_t su = osKernelUnlock();
	
	
    uint8_t* p_data = (uint8_t*)p_rdm;
    
	uint8_t i = 0;
    uint8_t j = 0;
    uint8_t current_byte = 0;
    uint16_t checksum = 0;//0xFF *6; //TODO: Verify, that 0 is used
    //当前设备没有标记时，才允许应答此条信息
	if(0 == g_rdm_ctr.mute_flag)
	{	
        for (i=0; i<7; i++)
            p_data[j++] = 0xFE;
		p_data[j++] = 0xAA;
        
        for (i=0; i<6; i++) 
        {
            current_byte = DevID[i];
            p_data[j++] = current_byte|0xAA;
            checksum += current_byte|0xAA;
            p_data[j++] = current_byte|0x55;
            checksum += current_byte|0x55;
        }
        p_data[j++] = checksum>>8|0xAA;
        p_data[j++] = checksum>>8|0x55;
        p_data[j++] = (checksum & 0xff)|0xAA;
        p_data[j++] = (checksum & 0xff)|0x55;
        
		
		hal_dmx_write_ctrl_pin(HAL_UART_DMX, 1);
		delay_us(10);
		hal_uart_send_data(HAL_UART_DMX ,(uint8_t*)p_data, j);
		
		//临界区代码 时序重要不能被打断
		osKernelRestoreLock(su);
		delay_us(200);
		hal_dmx_write_ctrl_pin(HAL_UART_DMX, 0);
		osKernelRestoreLock(sl);
//        DMX_RS485_EN_Out = DMX_EN_TX;
//        Uart2Send(p_data, j);
//		usart_dma_tx(p_data, j);
	}
}
/*****************************************************************************************
* Function Name : Get_Respond_Flag
* Description   : 检测收到的UUID是否与本机匹配，或是广播UUID（FF FF FF FF FF FF）
* Arguments     : NONE
* Return Value	: 0 - 匹配/1 - 不匹配
******************************************************************************************/
static uint8_t Get_Respond_Flag(RDM_Packet_TypeDef* p_rdm)
{
    uint8_t res= 0;
    uint8_t resB= 0;
    
	for (uint8_t i=0; i<6; i++)
	{
		if (p_rdm->DestID[i] != DevID[i]) 
            res= 1;
		if (p_rdm->DestID[i] != 0xFF)	 
            resB= 1;
	}
	if (resB == 0) 
        res= resB;
	return (res);
}
/*****************************************************************************************
* Function Name : Swip_UUID
* Description   : 数据发送前，将目标UUID与源UUID互换；
* Arguments     : 发送数据地址
* Return Value	: 1 - 广播数据，不用应答/0 - 定向数据，需要应答
******************************************************************************************/
static uint8_t Swip_UUID(RDM_Packet_TypeDef* p_rdm)
{
    uint8_t ret_val = 0;
    //广播命令，则不需要应答
    if(p_rdm->DestID[0] != 0xFF)
    {
        for (uint8_t i=0; i<6; i++)
		{
			p_rdm->DestID[i]= p_rdm->SrcID[i];	
			p_rdm->SrcID[i]= DevID[i];
		}
        return ret_val;
    }
    ret_val = 1;
    return ret_val;
}
/*****************************************************************************************
* Function Name : Package_Normal_Respond_Msg
* Description   : 打包普通应答包信息
* Arguments     : 源数据
* Return Value	: NONE
******************************************************************************************/
static void Package_Normal_Respond_Msg(RDM_Packet_TypeDef* p_rdm)
{
	uint16or8 cs;										//build check sum
	uint8_t* p_data = (uint8_t*)p_rdm;
    uint8_t i;

    p_rdm->Length =   p_rdm->PDLen + RDM_MIN_BYTE_NUM;
    p_rdm->Cmd =	  p_rdm->Cmd +1;							//it is a response... 

    cs.u16= 0;
    for (i=0; i<p_rdm->Length; i++)
        cs.u16 += p_data[i];
    p_data[p_rdm->Length]= cs.u8h;
    p_data[p_rdm->Length + 1] = cs.u8l;										
}
/*****************************************************************************************
* Function Name : Send_Normal_Ack_Msg
* Description   : 发送普通的应答信息,将串口发送脚初始化为普通GPIO，发送完break和MAB后，再恢复
成串口功能；
* Arguments     : 源数据
* Return Value	: NONE
******************************************************************************************/
static void Send_Normal_Ack_Msg(RDM_Packet_TypeDef* p_rdm)
{
	wired_dmx_transmit_with_break((uint8_t*)p_rdm, p_rdm->Length + 2);
}
