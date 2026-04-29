#include "hal_flash.h"
#include "hc32_ddl.h"
#define HC32FLASH_SECTOR_SIZE  0x2000  //8k


void hal_flash_erase(uint32_t addr, uint32_t size)
{
	uint16_t i;
    uint16_t sector_cnt = size / HC32FLASH_SECTOR_SIZE;
	en_result_t res;
	
    if((size % HC32FLASH_SECTOR_SIZE) != 0)
        ++sector_cnt;
    
	EFM_Unlock();
	
	while(Set != EFM_GetFlagStatus(EFM_FLAG_RDY0));
	while(Set != EFM_GetFlagStatus(EFM_FSR_RDY1));
	for(i=0; i<sector_cnt; i++)
	{
		(void)EFM_SectorCmd_Single((addr/0x2000)+i, Enable);
		(void)EFM_SectorErase(( addr / 0x2000 + i) * 0x2000);
	}
	EFM_Lock();    
}

void hal_flash_write(uint32_t addr, uint32_t size, uint8_t* data)
{
	uint16_t i = 0;
	uint32_t* p = (uint32_t*)data;
	uint16_t write_times = 0;
    
    uint16_t sector_cnt = size / HC32FLASH_SECTOR_SIZE;

    if((size % HC32FLASH_SECTOR_SIZE) != 0)
        ++sector_cnt;
	if(((addr + size) / HC32FLASH_SECTOR_SIZE) > (addr / HC32FLASH_SECTOR_SIZE))  //chen
		++sector_cnt;
	
	EFM_Unlock();
	EFM_SectorCmd_Sequential(addr, sector_cnt, Enable);
	
	write_times = size / 4 + ((size % 4) != 0 ? 1 : 0);
		
	for(i = 0; i < write_times; i++)
    {
		__disable_irq();
		EFM_SingleProgram(addr, *p);
		 __enable_irq();
		while(Set != EFM_GetFlagStatus(EFM_FLAG_RDY0));
		EFM_ClearFlag(EFM_FLAG_CLR_OPTEND0);
        addr += 4;
		p++;
	}
	EFM_SectorCmd_Sequential(addr, sector_cnt, Disable);
	EFM_Lock();
}

void system_reset (void)
{
    __set_PRIMASK(1);
    NVIC_SystemReset();
}

