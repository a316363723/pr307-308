/*********************************************************************************
  *Copyright(C), 2020, Aputure, All rights reserver.
  *FileName:  	security_user_implement.c
  *Author:    	
  *Version:   	v1.1
  *Date:      	2020/03/26
  *Description: 软加密模块
  *History:  
			-v1.0: Steven,初始版本
			-v1.1: 2020/03/26,Steven,添加Security_Flash_Lock、Security_Flash_Unlock、
					Security_Enable_IRQ、Security_Disable_IRQ接口，加大函数加密buffer，
					优化函数加密过程
**********************************************************************************/
//#include "stm32f10x.h"
#include "security.h"
#include "security_encrypt.h"
#include "hc32_ddl.h"
#include "bsp_flash.h"
#define FUNCTION_ENCRYPT_ADDRESS	(uint32_t)0x18000
#define HC32FLASH_SECTOR_SIZE 0x2000  //8k
//FLASH起始地址
#define STM32_FLASH_BASE 0x0000 	//STM32 FLASH的起始地址

extern int main(void);

__align(4) static const uint8_t function_cache[16][16] = {                              
	{0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff},
	{0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff},
	{0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff},
	{0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff},
	{0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff},
	{0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff},
	{0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff},
	{0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff},
	{0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff},
	{0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff},
	{0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff},
	{0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff},
	{0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff},
	{0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff},
	{0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff},
	{0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff},
};


//加密文件擦除函数
//用户自定义
static void Security_Flash_Erase(uint32_t addr)
{
	uint8_t i = 0;
	uint8_t sectorPos = 0;         // 扇区位置
	
	/* 计算当前扇区位置 */
    sectorPos = addr / HC32FLASH_SECTOR_SIZE;
    Flash_Unlock();
    while(Set != EFM_GetFlagStatus(EFM_FLAG_RDY0) && Set != EFM_GetFlagStatus(EFM_FLAG_RDY1));
    for(i = 0; i < 1; i++)
    {
		/* Sectordisables write protection */
		(void)EFM_SectorCmd_Single(sectorPos + i, Enable);
        EFM_SectorErase(addr + (HC32FLASH_SECTOR_SIZE * i));
		(void)EFM_SectorCmd_Single(sectorPos + i, Disable);
    }
    Flash_Lock();  
}
////flash写函数
////用户自定义
//static void Security_Flash_Write_HalfWord(uint32_t Address, uint16_t Data)
//{
//	fmc_halfword_program(Address , Data);
//}

static void Security_function_cache_Write(uint32_t dst, uint8_t *src)
{
	
	uint32_t i = 0;
	uint8_t sectorPos = 0;         // 扇区位置
	
	if(NULL == src)
		return ;
	
    Flash_Unlock();
    uint32_t* p = (uint32_t*)src;
	
	/* 计算当前扇区位置 */
    sectorPos = dst / HC32FLASH_SECTOR_SIZE;
	/* Sector disables write protection */
	for(i=0;i<(16 / HC32FLASH_SECTOR_SIZE + 1);i++)
		(void)EFM_SectorCmd_Single(sectorPos + i , Enable);
	
    for(i = 0; i < 16; i += 4)
    {
        __disable_irq();
       EFM_SingleProgram(dst + i , *p);
        __enable_irq();
        p++;
    } 
    
	for(i=0;i<(16 / HC32FLASH_SECTOR_SIZE + 1);i++)
		(void)EFM_SectorCmd_Single(sectorPos , Disable);	
    Flash_Lock();
	
	
}

void Start_Run(void)
{
	uint8_t ddd;
	ddd += 1;
}

//加密解密
void Security_Init(void)
{
	int8_t index = 0;
	void (*run)();
	
	AES_Key_Create((uint32_t)main);  //AES128秘钥拓展
	
	if (*((uint64_t*)function_cache[15]) == 0xffffffffffffffff) //没有加密成功,进行加密过程
	{
		for (index = 0; index < 15; index++)
		{
			if (*((uint64_t*)function_cache[index]) == 0xffffffffffffffff)  //该地址没有写过
			{
				/*数据加密*/
				uint32_t cache_addr = 0;
				cache_addr = (uint32_t)function_cache[index];
				Security_function_cache_Write(cache_addr, Function_Encrypt((uint32_t)Start_Run)); //加密并存储加密数据
				
				/*解密运行，验证加密成功*/
				run = (void (*)())Function_Decrypt(function_cache[index]); 
				(*run)(); //没跑飞说明加密成功
				
				/*加密成功后擦除相关加密数据*/
				{
					uint8_t temp[16] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 1, 2, 3, 4, 5, 6};
					Security_function_cache_Write((uint32_t)function_cache[15], temp);
					Security_Flash_Erase(FUNCTION_ENCRYPT_ADDRESS);
				}
				return;
			}
		}
	}
	//检查加密区，如没擦除则进行擦除
	if (*((uint64_t*)FUNCTION_ENCRYPT_ADDRESS) != 0xffffffffffffffff)
	{
		Security_Flash_Erase(FUNCTION_ENCRYPT_ADDRESS);
	}
	
	for (index = 14; index >= 0; index--)
	{
		if (*((uint64_t*)function_cache[index]) != 0xffffffffffffffff)  //加密成功
		{
			break;
		}
	}
	if (index < 0)
	{
		index = 0;
	}
	
	run = (void (*)())Function_Decrypt(function_cache[index]);
	(*run)();
}
