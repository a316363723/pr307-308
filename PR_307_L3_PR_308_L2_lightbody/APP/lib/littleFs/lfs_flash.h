#ifndef _LFT_FLASH_H_
#define _LFT_FLASH_H_

#include "lfs.h"
#include "hc32_ddl.h"
#include "bsp_w25qxx_q.h"

#define LFS_FLASH_NAME "board_spiflash"

extern uint8_t  lfs_mount_fail;

int sys_lfs_mount(void);
void Read_Data_From_W25qxx(uint8_t* data, uint16_t size, const char *path);
void Write_Data_To_W25qxx(uint8_t *data, uint16_t size, const char *path);
void Read_Iot_Data_From_W25qxx(uint8_t* data, uint16_t size);
void Write_Iot_Data_To_W25qxx(uint8_t* data, uint16_t size);

#endif /* _LFT_FLASH_H_ */
