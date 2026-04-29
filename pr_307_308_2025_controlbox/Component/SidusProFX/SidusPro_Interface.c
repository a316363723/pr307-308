/*********************************************************************************
  *Copyright(C), 2021, Aputure, All rights reserver.
  *FileName:    SidusProFX_Interface.c
  *Author:
  *Version:     v1.3
  *Date:        2021/04/17
  *Description: 光效模块接口
  *History:
            -v1.0: 2020/06/18,Matthew,初始版本
            -v1.1: 2020/08/06,Matthew,修复MFX光效部分Bug
            -v1.2: 2020/12/17,Matthew,新增pwm数据队列操作接口,修复部分MFX光效bug
            -v1.3: 2021/04/17,Matthew,用户实现接口新增及修改,CFX文件名功能新增,MFX光效优化
**********************************************************************************/
//#include "3color_mixing_implement.h"
//#include "color_mixing_dev.h"
#include "SidusPro_Interface.h"
#include <stdlib.h>
#include "hc32_ddl.h"
#include "SidusProFile.h"
#include <string.h>
/*User Includes Begin*/
#include "dev_w25qxx.h"
#include "cmsis_os.h"
#include "dev_ble.h"
//#include "dev_flash.h"
#include "perf_counter.h"
#include "page_manager.h"
#include "app_ble_protocol_parse.h"
#include "user.h"
#include "base_type.h"
/*User Includes End*/

/*
W25Q64 光效文件存储地址
分配30个大小为72K的空间用于存储光效pwm文件
*/
const uint32_t SidusPro_CFX_BankAddr[3][10] =
{
    {0X7EE000, 0X7DC000, 0X7CA000, 0X7B8000, 0X7A6000, 0X794000, 0X782000, 0X770000, 0X75E000, 0X74C000},
    {0X73A000, 0X728000, 0X716000, 0X704000, 0X6F2000, 0X6E0000, 0X6CE000, 0X6BC000, 0X6AA000, 0X698000},
    {0X686000, 0X674000, 0X662000, 0X650000, 0X63E000, 0X62C000, 0X61A000, 0X608000, 0X5F6000, 0X5E4000},
};

/** 
 * @brief   随机函数
 * @param   min
 * @param   max
 * @retval  
 */
uint32_t SidusPro_Get_RandomRange(uint32_t min, uint32_t max)
{
    //ToDo
	static uint16_t i = 0xf;
    uint32_t per;
    per = get_system_ticks();
    srand(per + i);
//    srand(HAL_GetTick());
    i++;
    return rand() % (max - min + 1) + min;

}
SidusPro_Pwm_Type pwm_value1;
/** 
 * @brief   pwm设置函数
 * @param   pwm_struct
 * @retval  None
 */
void SidusPro_Set_Pwm(SidusPro_Pwm_Type* pwm_struct)
{
    //Todo
    pwm_struct->pwm[0] = pwm_struct->pwm[0] <= 25 ? 0 : pwm_struct->pwm[0];
    pwm_struct->pwm[1] = pwm_struct->pwm[1] <= 110 ? 0 : pwm_struct->pwm[1];
    pwm_struct->pwm[2] = pwm_struct->pwm[2] <= 130 ? 0 : pwm_struct->pwm[2];
    pwm_struct->pwm[3] = pwm_struct->pwm[3] <= 9 ? 0 : pwm_struct->pwm[3];
    pwm_struct->pwm[4] = pwm_struct->pwm[4] <= 8 ? 0 : pwm_struct->pwm[4];
}
/** 
 * @brief   CCT转pwm
 * @param   cct
 * @param   gm
 * @param   lightness
 * @param   rgbww_pwm_struct
 * @retval  None
 */
void SidusPro_CCT2PWM(uint16_t cct, uint8_t gm, uint16_t lightness, SidusPro_Pwm_Type* rgbww_pwm_struct)
{
    //Todo
//	Color_CCT_To_Pwm( cct, gm, lightness, 
//				(Mix_Pwm *)rgbww_pwm_struct, ILL_MAX, CURVE_LINEAR);
//	float cct_duv;
//	struct mixing_pwm RGBWW_Arrary;
////    Color_CCT_To_Pwm(cct, gm, lightness < 10 ? 0 : 100 + 0.9f * lightness, (Mix_Pwm*)pulse_rgbww, ILL_MAX, CURVE_LINEAR);
//	cct_duv = ((float)gm - 20.0 * 0.5f) * 0.002f;
//	color_cct_to_pwm((float)lightness/1000.0f, cct, cct_duv, ILL_MAX, CURVE_LINEAR, 1.0f, &RGBWW_Arrary);
	
//	rgbww_pwm_struct->pwm[0]= RGBWW_Arrary.pwm[0];
//	rgbww_pwm_struct->pwm[1]= RGBWW_Arrary.pwm[1];
//	rgbww_pwm_struct->pwm[2]= RGBWW_Arrary.pwm[2];
//	rgbww_pwm_struct->pwm[3] =RGBWW_Arrary.pwm[3];
//	rgbww_pwm_struct->pwm[4] =RGBWW_Arrary.pwm[4];
//	if(lightness == 1)
//		vTaskDelay(500);
}
/** 
 * @brief   HSI转pwm
 * @param   hue
 * @param   sat
 * @param   lightness
 * @param   rgbww_pwm_struct
 * @retval  None
 */
void SidusPro_HSI2PWM(uint16_t hue, uint8_t sat, uint16_t lightness, SidusPro_Pwm_Type* rgbww_pwm_struct)
{
    //Todo
//	Color_HSI_To_Pwm( hue, sat, lightness,
//                       (Mix_Pwm *)rgbww_pwm_struct, ILL_MAX, CURVE_LINEAR);
//	struct mixing_pwm RGBWW_Arrary;
//	color_hsi_to_pwm((float)lightness/1000.0f, hue, sat, 6500, CURVE_LINEAR, 1.0f, &RGBWW_Arrary);
	
//	rgbww_pwm_struct->pwm[0]= RGBWW_Arrary.pwm[0];
//	rgbww_pwm_struct->pwm[1]= RGBWW_Arrary.pwm[1];
//	rgbww_pwm_struct->pwm[2]= RGBWW_Arrary.pwm[2];
//	rgbww_pwm_struct->pwm[3] =RGBWW_Arrary.pwm[3];
//	rgbww_pwm_struct->pwm[4] =RGBWW_Arrary.pwm[4];
}

/*******************************************************分割线**************************************************************
                        以下接口为队列操作接口，可使用FIFO.h中接口，RTOS中可换成系统队列
***************************************************************************************************************************/

//static fifo_t sfx_pwm_fifo = NULL;
//QueueHandle_t sfx_os_pwm_fifo;
//QueueHandle_t sfx_os_gatt_fifo;
/** 
 * @brief   队列创建函数
 * @param   queue   PWM输出队列或者文件传输接收队列
 * @param   count   队列深度
 * @param   size    单个数据大小
 * @retval  true :成功  false:失败
 */
bool SidusPro_Queue_Create(SidusPro_Queue_Type queue, uint32_t count, uint32_t size)
{
    bool ret = false;
//    if (SidusPro_Queue_PWM == queue)
//    {
//        sfx_os_pwm_fifo = xQueueCreate(count, size);
//        return sfx_os_pwm_fifo == NULL ? false : true;
//    }
//    else if (SidusPro_Queue_File_Transfer == queue)
//    {
//        sfx_os_gatt_fifo = xQueueCreate(count, size);
//        return sfx_os_gatt_fifo == NULL ? false : true;
//    }
    return ret;
}

/** 
 * @brief   队列数据添加函数
 * @param   queue   PWM输出队列或者文件传输接收队列
 * @param   item    item    数据地址
 * @retval  true :成功  false:失败
 */
bool SidusPro_Queue_Put(SidusPro_Queue_Type queue, const void *item)
{
    BaseType_t xReturn;
//    BaseType_t  pxHigherPriorityTaskWoken = pdFALSE;
//    
//    if (SidusPro_Queue_PWM == queue)
//    {
//        if(__get_IPSR() != 0U)
//        {
//            xReturn = xQueueSendFromISR(sfx_os_pwm_fifo, item, &pxHigherPriorityTaskWoken);
//            portEND_SWITCHING_ISR(pxHigherPriorityTaskWoken);
//        }
//        else
//        {
//            xReturn = xQueueSend(sfx_os_pwm_fifo, item, 10);
//        }
//    }
//    else if (SidusPro_Queue_File_Transfer == queue)
//    {
//        if(__get_IPSR() != 0U)
//        {
//            xReturn = xQueueSendFromISR(sfx_os_gatt_fifo, item, &pxHigherPriorityTaskWoken);
//            portEND_SWITCHING_ISR(pxHigherPriorityTaskWoken);
//        }
//        else
//        {
//            xReturn = xQueueSend(sfx_os_gatt_fifo, item, 10);
//        }
//    }
    return pdTRUE == xReturn ? true : false;
}

/** 
 * @brief   队列数据获取函数
 * @param   queue   PWM输出队列或者文件传输接收队列
 * @param   item    
 * @retval  true :成功  false:失败
 */
bool SidusPro_Queue_Get(SidusPro_Queue_Type queue, void *item)
{
    BaseType_t xReturn;
//    BaseType_t  pxHigherPriorityTaskWoken = pdFALSE;
//    
//    if (SidusPro_Queue_PWM == queue)
//    {
//        if(__get_IPSR() != 0U)
//        {
//            xReturn = xQueueReceiveFromISR(sfx_os_pwm_fifo, item, &pxHigherPriorityTaskWoken);
//            portEND_SWITCHING_ISR(pxHigherPriorityTaskWoken);
//        }
//        else
//        {
//            xReturn = xQueueReceive(sfx_os_pwm_fifo, item, 10);
//        }
//    }
//    else if (SidusPro_Queue_File_Transfer == queue)
//    {
//        if(__get_IPSR() != 0U)
//        {
//            xReturn = xQueueReceiveFromISR(sfx_os_gatt_fifo, item, &pxHigherPriorityTaskWoken);
//            portEND_SWITCHING_ISR(pxHigherPriorityTaskWoken);
//        }
//        else
//        {
//            xReturn = xQueueReceive(sfx_os_gatt_fifo, item, 10);
//        }
//    }
//    
    return pdTRUE == xReturn ? true : false;    
}

/** 
 * @brief   队列重置函数
 * @param   queue   PWM输出队列或者文件传输接收队列
 * @retval  true :成功  false:失败
 */
bool SidusPro_Queue_Reset(SidusPro_Queue_Type queue)
{
	return true;
//    if (SidusPro_Queue_PWM == queue)
//    {
//        return xQueueReset(sfx_os_pwm_fifo);
//    }
//    else if (SidusPro_Queue_File_Transfer == queue)
//    {
//        return xQueueReset(sfx_os_gatt_fifo);
//    }
}

/** 
 * @brief   查询队列容量是否已满
 * @param   queue   PWM输出队列或者文件传输接收队列
 * @retval  true :是  false:否
 */
bool SidusPro_Queue_Is_Full(SidusPro_Queue_Type queue)
{
	return true;
//    if (SidusPro_Queue_PWM == queue)
//    {
//        return uxQueueSpacesAvailable(sfx_os_pwm_fifo) > 0 ? false : true;
//    }
//    else if (SidusPro_Queue_File_Transfer == queue)
//    {
//        return uxQueueSpacesAvailable(sfx_os_gatt_fifo) > 0 ? false : true;
//    }
}

/** 
 * @brief   CFX Bank位置扇区擦除函数，擦除大小为SIDUSPRO_CFX_BANK_SECTOR_SIZE
 * @param   addr    Flash地址，非扇区序号
 * @retval  None
 */
void SidusPro_CFX_Bank_SectorErase(uint32_t addr)
{
//    W25QXX_Erase_Sector(addr/4096);
}

/** 
 * @brief   CFX Bank位置写函数，即为SidusPro_CFX_BankAddr所在Flash写函数
 * @param   p_buffer    数据读出地址
 * @param   addr        读取位置
 * @param   size        读取大小
 * @retval  None
 */
void SidusPro_CFX_Bank_Write(uint8_t *p_buffer, uint32_t addr, uint16_t size)
{
//    W25QXX_Write_NoCheck(p_buffer, addr, size);
}

/** 
 * @brief   CFX Bank位置读函数，即为SidusPro_CFX_BankAddr所在Flash读函数
 * @param   p_buffer    数据读出地址
 * @param   addr        读取位置
 * @param   size        读取大小
 * @retval  None
 */
void SidusPro_CFX_Bank_Read(uint8_t *p_buffer, uint32_t addr, uint16_t size)
{
//    W25QXX_Read(p_buffer, addr, size);
}


/** 
 * @brief   片上缓存区擦除函数,擦除地址为SIDUSPRO_CFX_CACHE_ADDR
            当缓存区定义在Ram时可不用实现擦除，可忽略size参数按照最大大小
			128+7200*sizeof(SidusPro_Pwm_Type)+16擦除
 * @param   size    擦除大小
 * @retval  None
 */
void SidusPro_CFX_CacheArea_Erase(uint32_t size)
{
//    erase_page(36, 130);
}
/**
 * @brief   缓存区SIDUSPRO_CFX_CACHE_ADDR写操作函数
 * @param   Addr    写入地址
 * @param   len     写入长度
 * @param   pData   写入数据
 * @retval  None
 */
void SidusPro_CFX_CacheArea_Write(uint8_t *p_buffer, uint32_t addr, uint32_t size)
{
//    uint16_t *_tdata = (uint16_t *)p_buffer;
    
//    FLASH_HalfwordProgram(addr, size, _tdata);
//    memcpy((uint8_t*)addr, p_buffer, size);
}

/** 
 * @brief   CFX文件传输应答状态信息
 * @param   status  状态信息，对应GATT文件传输Ack或者485通讯协议FileTransfer应答
 * @retval  None
 */
void SidusPro_FileTransfer_Send_Status(uint8_t status)
{
    //ToDo  应答文件传输过程中的状态(蓝牙GATT的ack或者485协议FileTransfer的ack)
//    uint8_t txBuffer[13] = {"AT+GATT=5:0\r\n"};
//    txBuffer[10] = status;
    ble_tx_packet tx_packet = {0};
    uint8_t txBuffer[3] = {"5:0"};
//    status = status + '0';
    txBuffer[2] = status;
    ble_package_write_cmd(tx_packet.data, &tx_packet.length, NULL, "GATT", strlen("GATT"), txBuffer, 3);
    
//	BB1735_Cmd_RW("0001", BB1735_DATA, TXBuffer, 10, 0);
//    while(1 == BB1735_Cmd_RW("0001", BB1735_DATA, txBuffer, 10, 0));
//    ble_package_write_cmd(tx_packet.data, &tx_packet.length, "0001", "DATA", strlen("DATA"), txBuffer, 13);
    ble_usart_tx_data(tx_packet.data, tx_packet.length);
    osDelay(2);  //暂时设置
//    ble_usart_tx_data(txBuffer, 13);
//	BB1735_CMD_Write(txBuffer, 13);
}

/** 
 * @brief   CFX Init回调函数，调用SidusProFX_Arg_Init函数Init CFX时产生此回调
 * @param   result      Init结果 true：成功 false：失败
 * @param   type        光效类型
 * @param   bank        光效Bank
 * @retval  None
 */
void SidusPro_CFX_Init_Callback(bool result, uint8_t type, uint8_t bank)
{
    uint8_t Tx_Buffer[10];
    uint8_t ui_id;
    uint8_t sui_id;
    BT_Body_TypeDef Body = {0};			/*数据内容*/
	Body.CFX_Ctrl_Body.Effect_Type = type;
	Body.CFX_Ctrl_Body.Bank= bank;
    ble_tx_packet tx_packet = {0};
    ble_rx_packet rx_packet = {0};
//    
    if(result == true)
    {
        ui_id = screen_get_act_pid();
        sui_id = screen_get_act_spid();
        if(ui_id == PAGE_CUSTOM_FX && sui_id == SUB_ID_CUSTOM_FX_LOADING)
        {
//            _tFrameFlag.CFX_Writaccom = SET;
        }
        else
        {
            ble_protocol_data_pack_up(Tx_Buffer, &Body, BT_CommandType_CFX_Ctrl, 0);
            ble_package_write_cmd(tx_packet.data, &tx_packet.length, "0001", "DATA", strlen("DATA"), (uint8_t *)&Tx_Buffer, 10);    
            ble_usart_tx_data(tx_packet.data, 10);
            while(true == ble_usart_rx_data(&rx_packet, 50))
            {
                if(strncmp((const char*)&rx_packet.data[0], "\rOK\r", 4) == 0)
                {
                    break;
                }
            }
        }
    }
}
/** 
 * @brief   CFX重命名回调函数，调用SidusProFile_Set_Name函数时产生此回调
 * @param   result      重命名结果 true：成功 false：失败
 * @param   type        光效类型
 * @param   bank        光效Bank
 * @param   new_name    新名称
 * @retval  None
 */
void SidusPro_CFX_Rename_Callback(bool result, uint8_t type, uint8_t bank, const char* new_name)
{
//    BT_Body_TypeDef ReadCommandBody = {0};
//    uint8_t Tx_Buffer[10] = {0};
//    if (result)
//    {
//        ReadCommandBody.CFX_Name_Body.Character_0 = SidusProFile_ASCII2SidusCode(new_name[0]);
//        ReadCommandBody.CFX_Name_Body.Character_1 = SidusProFile_ASCII2SidusCode(new_name[1]);
//        ReadCommandBody.CFX_Name_Body.Character_2 = SidusProFile_ASCII2SidusCode(new_name[2]);
//        ReadCommandBody.CFX_Name_Body.Character_3 = SidusProFile_ASCII2SidusCode(new_name[3]);
//        ReadCommandBody.CFX_Name_Body.Character_4 = SidusProFile_ASCII2SidusCode(new_name[4]);
//        ReadCommandBody.CFX_Name_Body.Character_5 = SidusProFile_ASCII2SidusCode(new_name[5]);
//        ReadCommandBody.CFX_Name_Body.Character_6 = SidusProFile_ASCII2SidusCode(new_name[6]);
//        ReadCommandBody.CFX_Name_Body.Character_7 = SidusProFile_ASCII2SidusCode(new_name[7]);
//        ReadCommandBody.CFX_Name_Body.Character_8 = SidusProFile_ASCII2SidusCode(new_name[8]);
//        BT_CommandData_Pack(Tx_Buffer, (uint8_t *)&ReadCommandBody, BT_CommandType_CFX_Name, 0);
//        xQueueSend(Blu_Data_Queue, Tx_Buffer, 10);
//        //Ble_CommonData_Fifo_Add(BB1735_DATA, 10, Tx_Buffer);
//    }
}

