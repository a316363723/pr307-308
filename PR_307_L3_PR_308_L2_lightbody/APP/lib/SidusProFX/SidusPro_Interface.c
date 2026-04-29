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

#include "SidusPro_Interface.h"
/*User Includes Begin*/
#include "color_mixing_dev.h"
#include "FIFO.h"
#include "bsp_timer.h"
#include "flash.h"
//#include "SidusProFile.h"
#include "color_mixing.h"
//#include "app_led.h"
#include "systick.h"
#include "bsp_w25qxx_q.h"
/*User Includes End*/
#include "stdio.h"

/** 
CFX光效文件外部存储地址，地址要求扇区对齐，每个Bank需满足至少128+7200*sizeof(SidusPro_Pwm_Type)+16大小
每个Bank大小应为SIDUSPRO_CFX_BANK_SECTOR_SIZE*SIDUSPRO_CFX_EACH_BANK_SECTOR_NUM
 */
const uint32_t SidusPro_CFX_BankAddr[3][10] =
{
    {0X7EE000, 0X7DC000, 0X7CA000, 0X7B8000, 0X7A6000, 0X794000, 0X782000, 0X770000, 0X75E000, 0X74C000},
    {0X73A000, 0X728000, 0X716000, 0X704000, 0X6F2000, 0X6E0000, 0X6CE000, 0X6BC000, 0X6AA000, 0X698000},
    {0X686000, 0X674000, 0X662000, 0X650000, 0X63E000, 0X62C000, 0X61A000, 0X608000, 0X5F6000, 0X5E4000},
};

/**
 * @brief   随机值获取函数
 * @param   min
 * @param   max
 * @retval  
 */
uint32_t SidusPro_Get_RandomRange(uint32_t min, uint32_t max)
{
    //ToDo
    srand(HAL_GetTick());
    return rand() % (max - min + 1) + min;
}
/** 
 * @brief   pwm设置函数
 * @param   pwm_struct
 * @retval  None
 */
uint16_t led_data_buf[2024];
uint16_t led_pwm_num = 0;

void SidusPro_Set_Pwm(SidusPro_Pwm_Type* pwm_struct)
{
    //Todo
	static uint16_t data_buf[SIDUSPRO_PWM_CH_NUM] = {0};
	for(uint8_t i = 0; i < SIDUSPRO_PWM_CH_NUM; i++){
		data_buf[i] = pwm_struct->pwm[i] * g_power_factor.sum_power_rate;
	}
	set_self_adapt_pwm(data_buf);
	Set_Drive_Cob_Pwm_Val(data_buf);
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
	struct mixing_lux RGBWW_Lux_Arrary;
#ifdef PR_308_L2
	color_cct_calc((float)lightness / 1000, cct, 0, (enum ilumination_mode)g_rs485_data.illumination_mode.mode, (enum mixing_curve)g_rs485_data.dimming_curve.curve, g_power_factor.sum_power_rate,
        (struct mixing_pwm *)rgbww_pwm_struct, &RGBWW_Lux_Arrary);
	uint16_t led_pwm[5] = {0};
	led_pwm[0] = rgbww_pwm_struct->pwm[0];
	led_pwm[1] = rgbww_pwm_struct->pwm[1];
	led_pwm[2] = rgbww_pwm_struct->pwm[2];
	
	rgbww_pwm_struct->pwm[0] = led_pwm[1];
	rgbww_pwm_struct->pwm[1] = led_pwm[0];
	rgbww_pwm_struct->pwm[4] = led_pwm[2];
	
	if( lightness == 0 )
	{
		rgbww_pwm_struct->pwm[0] = 0;
		rgbww_pwm_struct->pwm[1] = 0;
		rgbww_pwm_struct->pwm[2] = 0;
		rgbww_pwm_struct->pwm[3] = 0;
		rgbww_pwm_struct->pwm[4] = 0;
	}
#endif

#ifdef PR_307_L3
	color_cct_calc((float)lightness / 1000.0f, cct, gm_to_duv(gm), (enum ilumination_mode)g_rs485_data.illumination_mode.mode, (enum mixing_curve)g_rs485_data.dimming_curve.curve, g_power_factor.sum_power_rate,
        (struct mixing_pwm *)rgbww_pwm_struct, &RGBWW_Lux_Arrary);
#endif
//	printf("pwm_data[0]:%d\r\n",rgbww_pwm_struct[0]);
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
	struct mixing_lux RGBWW_Lux_Arrary;
	color_hsi_calc((float)lightness / 1000.0f, hue, sat, 6500, 0.0f, (enum mixing_curve)g_rs485_data.dimming_curve.curve, g_power_factor.sum_power_rate, (struct mixing_pwm *)rgbww_pwm_struct ,&RGBWW_Lux_Arrary);
}

static fifo_t sfx_os_pwm_fifo;
static fifo_t sfx_os_file_tx_fifo;
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
    if (SidusPro_Queue_PWM == queue)
    {
		sfx_os_pwm_fifo = fifo_create(count, size);
		ret = sfx_os_pwm_fifo == NULL ? false : true;
    }
    else if (SidusPro_Queue_File_Transfer == queue)
    {
		sfx_os_file_tx_fifo = fifo_create(count, size);
        ret = sfx_os_file_tx_fifo == NULL ? false : true;
    }
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
    bool ret = false;
    if (SidusPro_Queue_PWM == queue)
    {
//		return fifo_add(gatt_rx_fifo, item);
		ret = fifo_add(sfx_os_pwm_fifo, item);
    }
    else if (SidusPro_Queue_File_Transfer == queue)
    {
		ret = fifo_add(sfx_os_file_tx_fifo, item);
    }
    return ret;
}

/** 
 * @brief   队列数据获取函数
 * @param   queue   PWM输出队列或者文件传输接收队列
 * @param   item    
 * @retval  true :成功  false:失败
 */
bool SidusPro_Queue_Get(SidusPro_Queue_Type queue, void *item)
{
    bool ret = false;
    if (SidusPro_Queue_PWM == queue)
    {
//		return fifo_get(gatt_rx_fifo, item); 
		ret = fifo_get(sfx_os_pwm_fifo, item);
    }
    else if (SidusPro_Queue_File_Transfer == queue)
    {
		ret = fifo_get(sfx_os_file_tx_fifo, item);
    }
    return ret;
}

/** 
 * @brief   队列重置函数
 * @param   queue   PWM输出队列或者文件传输接收队列
 * @retval  true :成功  false:失败
 */
bool SidusPro_Queue_Reset(SidusPro_Queue_Type queue)
{
    bool ret = false;
    if (SidusPro_Queue_PWM == queue)
    {
		ret = fifo_reset(sfx_os_pwm_fifo);
    }
    else if (SidusPro_Queue_File_Transfer == queue)
    {
		ret = fifo_reset(sfx_os_file_tx_fifo);
    }
    return ret;
}

/** 
 * @brief   查询队列容量是否已满
 * @param   queue   PWM输出队列或者文件传输接收队列
 * @retval  true :是  false:否
 */
bool SidusPro_Queue_Is_Full(SidusPro_Queue_Type queue)
{
    bool ret = false;
    if (SidusPro_Queue_PWM == queue)
    {
		ret = fifo_is_full(sfx_os_pwm_fifo);
    }
    else if (SidusPro_Queue_File_Transfer == queue)
    {
		ret = fifo_is_full(sfx_os_file_tx_fifo);
    }
    return ret;
}

/** 
 * @brief   CFX Bank位置扇区擦除函数，擦除大小为SIDUSPRO_CFX_BANK_SECTOR_SIZE
 * @param   addr    Flash地址，非扇区序号
 * @retval  None
 */
void SidusPro_CFX_Bank_SectorErase(uint32_t addr)
{
//	W25QXX_Erase_Sector(addr / 4096);
	W25Q64_EraseSector(addr/4096);
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
//	W25QXX_Write_NoCheck(p_buffer, addr, size);
	W25Q64_WriteData(addr, p_buffer, size);
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
//	W25QXX_Read(p_buffer, addr, size);
	W25Q64_ReadData(addr, p_buffer, size);
}


/** 
 * @brief   片上缓存区擦除函数,擦除地址为SIDUSPRO_CFX_CACHE_ADDR
            当缓存区定义在Ram时可不用实现擦除，可忽略size参数按照最大大小
			128+7200*sizeof(SidusPro_Pwm_Type)+16擦除
 * @param   size    擦除大小
 * @retval  None
 */
#define HC32FLASH_SECTOR_SIZE 0x2000
void SidusPro_CFX_CacheArea_Erase(uint32_t size)
{
	Erase_Flash_Area(SIDUSPRO_CFX_CACHE_ADDR, 9);//擦除升级区,擦除最后一个快128K
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
    write_cfx_data_to_flash(&addr, p_buffer, size);
//	uint8_t SectorNum = 0;
//	if(size % 0x2000 != 0)
//		SectorNum = size/0x2000 + 1;
//	else
//		SectorNum = size/0x2000;
//	UpdataFlashProgram(addr, size, SectorNum, p_buffer);
}

/** 
 * @brief   CFX文件传输应答状态信息
 * @param   status  状态信息，对应GATT文件传输Ack或者485通讯协议FileTransfer应答
 * @retval  None
 */
static uint8_t s_msg_flag1 = 0;      /*标志产生标志*/
static uint8_t s_msg_state1 = 0;     /*状态信息*/
void SidusPro_FileTransfer_Send_Status(uint8_t status)
{
    s_msg_flag1 = 1;
    s_msg_state1 = status;
    #if(1 == GLOBAL_PRINT_ENABLE)
//    printf("file_state = %d\n", s_msg_state1);
    #endif
}

/** 
 * @brief   CFX Init回调函数，调用SidusProFX_Arg_Init函数Init CFX时产生此回调
 * @param   result      Init结果 true：成功 false：失败
 * @param   type        光效类型
 * @param   bank        光效Bank
 * @retval  None
 */
static uint8_t s_msg_flag2 = 0;
static uint8_t s_msg_state2 = 0;
void SidusPro_CFX_Init_Callback(bool result, uint8_t type, uint8_t bank)
{
    s_msg_flag2 = 1;
    s_msg_state2 = (uint8_t)result;
    #if(1 == GLOBAL_PRINT_ENABLE)
//    printf("cfx_init = %d\n", s_msg_state2);
    #endif
}
/** 
 * @brief   CFX重命名回调函数，调用SidusProFile_Set_Name函数时产生此回调
 * @param   result      重命名结果 true：成功 false：失败
 * @param   type        光效类型
 * @param   bank        光效Bank
 * @param   new_name    新名称
 * @retval  None
 */
static uint8_t s_msg_flag3 = 0;
static uint8_t s_msg_state3 = 0;
void SidusPro_CFX_Rename_Callback(bool result, uint8_t type, uint8_t bank, const char* new_name)
{
    s_msg_flag3 = 1;
    s_msg_state3 = (uint8_t)result;
}
/*****************************************************************************************
* Function Name: get_cfx_file_state
* Description  : 485协议获取sidus光效文件传输状态
* Arguments    : NONE
* Return Value : RS485_Ack_Err_Busy：继续等待状态
******************************************************************************************/
rs485_ack_enum get_cfx_file_state(void)
{
    if(1 == s_msg_flag1){
        s_msg_flag1 = 0;
        #if(1 == GLOBAL_PRINT_ENABLE)
//        printf("cs. ");
        #endif
        return((rs485_ack_enum)s_msg_state1);
    }
    else{
        return(RS485_Ack_Err_Busy);
    }
}
/*****************************************************************************************
* Function Name: get_cfx_init_state
* Description  : 485协议获取sidus cfx光效初始化状态
* Arguments    : NONE
* Return Value : RS485_Ack_Err_Busy：继续等待状态
******************************************************************************************/
rs485_ack_enum get_cfx_init_state(void)
{
    if(1 == s_msg_flag2){
        s_msg_flag2 = 0;
        #if(1 == GLOBAL_PRINT_ENABLE)
//        printf("css. ");
        #endif
        if(true == s_msg_state2){
            return RS485_Ack_Ok;
        }
        else{
            return RS485_Ack_Err_Over_Size;
        }
    }
    else{
        return(RS485_Ack_Err_Busy);
    }
}
/*****************************************************************************************
* Function Name: get_cfx_rename_state
* Description  : 485协议获取sidus cfx光效重命名状态
* Arguments    : NONE
* Return Value : RS485_Ack_Err_Busy：继续等待状态
******************************************************************************************/
rs485_ack_enum get_cfx_rename_state(void)
{
    if(1 == s_msg_flag3){
        s_msg_flag3 = 0;
        #if(1 == GLOBAL_PRINT_ENABLE)
//        printf("csss. ");
        #endif
        if(true == s_msg_state3){
            return RS485_Ack_Ok;
        }
        else{
            return RS485_Ack_Err_Over_Size;
        }
    }
    else{
        return(RS485_Ack_Err_Busy);
    }
}

