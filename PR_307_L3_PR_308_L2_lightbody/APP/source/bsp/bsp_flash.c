#include "bsp_flash.h"
#include "hc32_ddl.h"


#define FLASH_RW(addr)				*((uint32_t *)(addr))
#define FLASH_RHW(addr)				*((uint16_t *)(addr))

void Flash_Unlock(void)
{
    /* Unlock all EFM registers */
    EFM_Unlock();
    /* Unlock EFM register: FWMC */
    EFM_FWMC_Unlock();
}

void Flash_Lock(void)
{
    /* Lock EFM register: FWMC */
     EFM_FWMC_Lock();
    /* Lock all EFM registers */
    EFM_Lock();
}

void Flash_Init(void)
{
	stc_efm_cfg_t stcEfmCfg;
	en_int_status_t flag1;
    en_int_status_t flag2;
	
	Flash_Unlock();
   /* EFM default config. */
    (void)EFM_StructInit(&stcEfmCfg);
    /*
     * Clock <= 40MHZ             EFM_WAIT_CYCLE_0
     * 40MHZ < Clock <= 80MHZ     EFM_WAIT_CYCLE_1
     * 80MHZ < Clock <= 120MHZ    EFM_WAIT_CYCLE_2
     * 120MHZ < Clock <= 160MHZ   EFM_WAIT_CYCLE_3
     * 160MHZ < Clock <= 200MHZ   EFM_WAIT_CYCLE_4
     * 200MHZ < Clock <= 240MHZ   EFM_WAIT_CYCLE_5
     */
    stcEfmCfg.u32WaitCycle = EFM_WAIT_CYCLE_5;
    /* EFM config */
    (void)EFM_Init(&stcEfmCfg);
	
	/* Wait flash0, flash1 ready. */
    do{
        flag1 = EFM_GetFlagStatus(EFM_FLAG_RDY0);
        flag2 = EFM_GetFlagStatus(EFM_FLAG_RDY1);
    }while((Set != flag1) || (Set != flag2));
	Flash_Lock();


}

//Addr:擦除地址   SectorCount：擦除扇区数量  每个扇区8K
void Flash_Erase(uint32_t Addr,uint8_t SectorCount)
{
	uint8_t i = 0;
	uint8_t sectorPos = 0;         // 扇区位置
	
	/* 计算当前扇区位置 */
    sectorPos = Addr / HC32FLASH_SECTOR_SIZE;
    Flash_Unlock();
    while(Set != EFM_GetFlagStatus(EFM_FLAG_RDY0) && Set != EFM_GetFlagStatus(EFM_FLAG_RDY1));
    for(i = 0; i < SectorCount; i++)
    {
//		__disable_irq();
		/* Sectordisables write protection */
		(void)EFM_SectorCmd_Single(sectorPos + i, Enable);
        EFM_SectorErase(Addr + (HC32FLASH_SECTOR_SIZE * i));
		(void)EFM_SectorCmd_Single(sectorPos + i, Disable);
//		__enable_irq();
    }
    Flash_Lock();    
}

uint8_t Flash_Write(uint32_t Addr, uint32_t Len, uint8_t* Data)
{
    uint32_t i = 0;
	uint8_t sectorPos = 0;         // 扇区位置
	
	if(NULL == Data)
		return 1;
	
    Flash_Unlock();
	while(Set != EFM_GetFlagStatus(EFM_FLAG_RDY0) && Set != EFM_GetFlagStatus(EFM_FLAG_RDY1));
    uint32_t* p = (uint32_t*)Data;
	
	//dui  qi 
	if(Addr%4!=0)
	{
		Addr+=Addr%4;
	}
	/* 计算当前扇区位置 */
    sectorPos = Addr / HC32FLASH_SECTOR_SIZE;
	/* Sector disables write protection */
	for(i=0;i<=(Len / HC32FLASH_SECTOR_SIZE + 1);i++)
		(void)EFM_SectorCmd_Single(sectorPos + i , Enable);
	
    for(i = 0; i < Len; i += 4)
    {
//        __disable_irq();
		EFM_SingleProgram(Addr + i , *p);
//        __enable_irq();
        p++;
    } 
    
	for(i=0;i<=(Len / HC32FLASH_SECTOR_SIZE + 1);i++)
		(void)EFM_SectorCmd_Single(sectorPos , Disable);	
    Flash_Lock();
	
	return 0;
	
}

