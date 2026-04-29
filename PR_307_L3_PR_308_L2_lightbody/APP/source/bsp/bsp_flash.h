#ifndef BSP_FLASH_H
#define BSP_FLASH_H

#include "stdint.h"


#define HC32FLASH_SECTOR_SIZE 0x2000  //8k

void Flash_Unlock(void);
void Flash_Lock(void);
void Flash_Init(void);
void Flash_Erase(uint32_t Addr,uint8_t SectorCount);
uint8_t Flash_Write(uint32_t Addr, uint32_t Len, uint8_t* Data);

#endif



