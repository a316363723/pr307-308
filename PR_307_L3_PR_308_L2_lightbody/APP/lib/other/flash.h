/****************************************************************************************
**  Filename :  api.h
**  Abstract :  This file include API function statement.
**  By       :  HeJianGuo <hejianguo5515@126.com>
**  Date     :  2018-01-23
**  Changelog:1.First Create
*****************************************************************************************/
#ifndef __FLASH_H
#define __FLASH_H
/*****************************************************************************************
									Includes
*****************************************************************************************/
#include "hc32_ddl.h"
#include "define.h"
/*****************************************************************************************
								Macro definitions
*****************************************************************************************/
#define FLASH_START_ADDR           	((uint32_t)0x00000000) 
#define FLASH_PAGE_SIZE				(0X2000)

#define BOOT_START_SECTOR			(0) 
#define BOOT_USE_SECTORS			(11) //88K
#define BOOT_START_ADDR				((FLASH_START_ADDR) + (BOOT_START_SECTOR) * (FLASH_PAGE_SIZE))
#define BOOT_END_ADDR				((BOOT_START_ADDR) + (BOOT_USE_SECTORS) * (FLASH_PAGE_SIZE))

#define FLAG_START_SECTOR			(11)
#define FLAG_USE_SECTORS			(1) //8K
#define FLAG_START_ADDR				((FLASH_START_ADDR) + (FLAG_START_SECTOR) * (FLASH_PAGE_SIZE))
#define FLAG_END_ADDR				((FLAG_START_ADDR) + (FLAG_USE_SECTORS) * (FLASH_PAGE_SIZE))

#define AES_START_SECTOR			(12)
#define AES_USE_SECTORS				(2) //16K
#define AES_START_ADDR				((FLASH_START_ADDR) + (AES_START_SECTOR) * (FLASH_PAGE_SIZE))
#define AES_END_ADDR				((AES_START_ADDR) + (AES_USE_SECTORS) * (FLASH_PAGE_SIZE))

#define APP_START_SECTOR			(14)
#define APP_USE_SECTORS				(64) //512K
#define APP_START_ADDR				((FLASH_START_ADDR) + (APP_START_SECTOR) * (FLASH_PAGE_SIZE))
#define APP_END_ADDR				((APP_START_ADDR) + (APP_USE_SECTORS) * (FLASH_PAGE_SIZE))

#define UPGRADE_START_SECTOR		(94)
#define UPGRADE_USE_SECTORS			(64) //512K
#define UPGRADE_START_ADDR			((FLASH_START_ADDR) + (UPGRADE_START_SECTOR) * (FLASH_PAGE_SIZE))
#define UPGRADE_END_ADDR			((UPGRADE_START_ADDR) + (UPGRADE_USE_SECTORS) * (FLASH_PAGE_SIZE))
//偏移
#define BASE_DATA_OFFSET			(FLASH_PAGE_SIZE * 78) //app区后面，9C000
#define ANALOG_DATA_OFFSET			(FLASH_PAGE_SIZE * 86) //app区后面,AC000

#define FLASH_RW(addr)				*((uint32_t *)(addr))
#define FLASH_RHW(addr)				*((uint16_t *)(addr))


#define UPGRADE_ENABLE             	((uint32_t)0xAE86AE86)//升级使能
#define UPGRADE_ENABLE_ADDR			(FLAG_START_ADDR + 4)
#define UPGRADE_FILE_LEN_ADDR		(FLAG_START_ADDR + 8)
#define UPGRADE_FILE_CRC_ADDR		(FLAG_START_ADDR + 12)
/*****************************************************************************************
								Typedef definitions
*****************************************************************************************/
typedef __packed struct
{
	uint32_t start_w_addr;
	uint32_t up_len;	//升级长度，比如单独升级基础数据时的长度
	uint32_t sum_len;	//总长度，如单独升级基础数据时 的基础数据长度再加上前面APP程序和中间的填充区
	uint32_t up_enable;
	uint32_t up_file_crc;
}Upgrade_W_Flag_TypeDef;
/*****************************************************************************************
							Global variables and functions
*****************************************************************************************/
extern ErrStatus Erase_Flash_Area(uint32_t erase_start_sector, uint16_t erase_sector_num);
extern ErrStatus write_data_to_flash(uint32_t *start_addr, uint8_t *data_buf, uint16_t write_byte_len);
extern ErrStatus write_cfx_data_to_flash(uint32_t *start_addr, uint8_t *data_buf, uint16_t write_byte_len);
extern ErrStatus Move_Data_Flash_To_flash(uint32_t obj_start_addr, uint32_t source_start_addr, uint32_t write_byte_len);
extern ErrStatus write_flag_to_flash(uint32_t write_addr,uint32_t w_data);
extern ErrStatus clr_flag_form_flash(uint32_t clr_addr);
#endif
/***********************************END OF FILE*******************************************/

