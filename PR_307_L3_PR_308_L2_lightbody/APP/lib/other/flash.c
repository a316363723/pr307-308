/****************************************************************************************
**  Filename :  flash.c
**  Abstract :  这个文件中的函数主要为提供应用接口函数，调用底层的驱动程序，经过逻辑处理后供
                应用层使用。
**  By       :  HeJianGuo <hejianguo5515@126.com>
**  Date     :  2018-08-11
**  Changelog:1.First Create
*****************************************************************************************/

/*****************************************************************************************
									Includes
*****************************************************************************************/
#include "flash.h"
#include "define.h"
#include "bsp_flash.h"
#include "bsp_power.h"
/*****************************************************************************************
							  Variables definitions
*****************************************************************************************/
uint32_t PageError = 0;
uint32_t s_flash_sector_info[] = {
//	CTL_SECTOR_NUMBER_0,
//	CTL_SECTOR_NUMBER_1,
//	CTL_SECTOR_NUMBER_2,
//	CTL_SECTOR_NUMBER_3,
//	CTL_SECTOR_NUMBER_4,
//	CTL_SECTOR_NUMBER_5,
//	CTL_SECTOR_NUMBER_6,
//	CTL_SECTOR_NUMBER_7,
//	CTL_SECTOR_NUMBER_8,
//	CTL_SECTOR_NUMBER_9,
//	CTL_SECTOR_NUMBER_10,
//	CTL_SECTOR_NUMBER_11,
//	CTL_SECTOR_NUMBER_12,
//	CTL_SECTOR_NUMBER_13,
//	CTL_SECTOR_NUMBER_14,
//	CTL_SECTOR_NUMBER_15,
//	CTL_SECTOR_NUMBER_16,
//	CTL_SECTOR_NUMBER_17,
//	CTL_SECTOR_NUMBER_18,
//	CTL_SECTOR_NUMBER_19,
//	CTL_SECTOR_NUMBER_20,
//	CTL_SECTOR_NUMBER_21,
//	CTL_SECTOR_NUMBER_22,
//	CTL_SECTOR_NUMBER_23,
};
/*****************************************************************************************
							  Functions definitions
*****************************************************************************************/


/*****************************************************************************************
* Function Name: Erase_Flash_Area
* Description  : 单片机升级flash区域擦除函数。
* Arguments    : 起始擦除块、被擦除块的数量
* Return Value : 擦除状态 SUCCESS - 正常/ERROR - 错误
******************************************************************************************/
ErrStatus Erase_Flash_Area(uint32_t erase_start_addr, uint16_t erase_sector_num)
{
	Flash_Erase(erase_start_addr,erase_sector_num);
    return SUCCESS;
}
/*****************************************************************************************
* Function Name: write_data_to_flash
* Description  : 单片机升级flash区域写函数。
* Arguments    : 起始地址、数组首地址、数据长度
* Return Value : 写状态 SUCCESS - 正常/ERROR - 错误
******************************************************************************************/
ErrStatus write_data_to_flash(uint32_t *start_addr, uint8_t *data_buf, uint16_t write_byte_len)
{
	Flash_Write(*start_addr, write_byte_len, data_buf);
	return SUCCESS;
}

/*****************************************************************************************
* Function Name: write_data_to_flash
* Description  : 单片机升级flash区域写函数。
* Arguments    : 起始地址、数组首地址、数据长度
* Return Value : 写状态 SUCCESS - 正常/ERROR - 错误
******************************************************************************************/
ErrStatus write_cfx_data_to_flash(uint32_t *start_addr, uint8_t *data_buf, uint16_t write_byte_len)
{
	Flash_Write(*start_addr, write_byte_len, data_buf);
	return SUCCESS;
}
/*****************************************************************************************
* Function Name: Move_Data_Flash_To_flash
* Description  : 将一块flash中存储的数据移动到另一区域。
* Arguments    : 目标起始、源起始地址、移动数据总长度（字节）
* Return Value : 写状态 SUCCESS - 正常/ERROR - 错误
******************************************************************************************/
ErrStatus Move_Data_Flash_To_flash(uint32_t obj_start_addr, uint32_t source_start_addr, uint32_t write_byte_len)
{
	if(obj_start_addr == source_start_addr)
		return ERROR;
	Flash_Write(obj_start_addr, write_byte_len, (uint8_t *)source_start_addr);
	return SUCCESS;
}
/*****************************************************************************************
* Function Name : write_flag_to_flash
* Description   : 在FLASH中写入升级是能标志值。
* Arguments     : 写入的地址、写入的数据
* Return Value	: 状态 SUCCESS/ERROR
******************************************************************************************/
ErrStatus write_flag_to_flash(uint32_t write_addr,uint32_t w_data)
{
    return SUCCESS;
}
/*****************************************************************************************
* Function Name : clr_flag_form_flash
* Description   : 清除某一个flash位置的标志，即向此地址写0；
* Arguments     : 被清除标志的地址
* Return Value	: 状态 SUCCESS/ERROR
******************************************************************************************/
ErrStatus clr_flag_form_flash(uint32_t clr_addr)
{
    return SUCCESS;
}
/***********************************END OF FILE*******************************************/

#include "bsp_debug.h"
uint8_t read_buff[10];
uint8_t wite_buff[4];
void reset_test_flash()
{
	wite_buff[0] = 0;
	wite_buff[1] = 0;
	wite_buff[2] = 0;
	wite_buff[3] = 0;
	 if(Fan_Power_Check())
	 {
			Flash_Erase(0xf000, 1);
			Flash_Write(0xf000, sizeof(wite_buff), wite_buff); 
	 }
}

//uint32_t flag    __attribute__((at(0x0f000)));
//void flash_test()
//{
//	reset_test_flash();
//	uint32_t count = flag + 1;
//	wite_buff[0] = count >> 0;
//	wite_buff[1] = count >> 8;
//	wite_buff[2] = count >> 16;
//	wite_buff[3] = count >> 24;
//	DBG("reset count [%d]\r\n", count);
//	memcpy(read_buff,0x0f000,sizeof(read_buff));
//	Flash_Erase(0xf000, 1);
//	Flash_Write(0xf000, sizeof(wite_buff), wite_buff);
//	memcpy(read_buff,0xf000,sizeof(read_buff));
//}
