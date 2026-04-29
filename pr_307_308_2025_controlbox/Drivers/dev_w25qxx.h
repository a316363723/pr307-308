#ifndef DEV_W25QXX_H
#define DEV_W25QXX_H

#include <stdint.h>

/********************************************************************************************
*                                   引脚接口
********************************************************************************************/
//SCK
#define W25QXX_SCK_PORT  GPIO_PORT_B
#define W25QXX_SCK_PIN   GPIO_PIN_14
#define W25QXX_SCK_AF    GPIO_FUNC_18_QSPI
//IO0
#define W25QXX_IO0_PORT  GPIO_PORT_B
#define W25QXX_IO0_PIN   GPIO_PIN_13
#define W25QXX_IO0_AF    GPIO_FUNC_18_QSPI
//IO1
#define W25QXX_IO1_PORT  GPIO_PORT_B
#define W25QXX_IO1_PIN   GPIO_PIN_12
#define W25QXX_IO1_AF    GPIO_FUNC_18_QSPI
//IO2
#define W25QXX_IO2_PORT  GPIO_PORT_B
#define W25QXX_IO2_PIN   GPIO_PIN_10
#define W25QXX_IO2_AF    GPIO_FUNC_18_QSPI
//IO3
#define W25QXX_IO3_PORT  GPIO_PORT_D
#define W25QXX_IO3_PIN   GPIO_PIN_11 
#define W25QXX_IO3_AF    GPIO_FUNC_18_QSPI
//NSS
#define W25QXX_NSS_PORT  GPIO_PORT_C
#define W25QXX_NSS_PIN   GPIO_PIN_07
#define W25QXX_NSS_AF    GPIO_FUNC_18_QSPI
/********************************************************************************************
 *  芯片ID
 ********************************************************************************************/
#define W25Q80_CHIP_ID              0xEF13
#define W25Q16_CHIP_ID              0xEF14
#define W25Q32_CHIP_ID              0xEF15
#define W25Q64_CHIP_ID              0xEF16
#define W25Q128_CHIP_ID             0xEF17
#define W25Q256_CHIP_ID             0xEF18   //GD32Q256  0xC818
#define GD25Q256_CHIP_ID            0xC818   //GD32Q256  0xC818
/********************************************************************************************
 *  芯片信息
 ********************************************************************************************/
#define W25QXX_SECTOR_PAGE_NUM      16U                                         //Sector的Page数量
#define W25QXX_PAGE_SIZE            256U                                        //Page大小
#define W25QXX_SECTOR_SIZE          (W25QXX_PAGE_SIZE * W25QXX_SECTOR_PAGE_NUM) //Sector大小
#define W25QXX_32KBLOCK_SIZE        (32U * 1024U)                               //32K Block大小
#define W25QXX_64KBLOCK_SIZE        (64U * 1024U)                               //64K Block大小
#define W25Q80_CHIP_SIZE            (80U  / 8U * 1024U * 1024U)                 //W25Q80芯片容量
#define W25Q16_CHIP_SIZE            (16U  / 8U * 1024U * 1024U)                 //W25Q16芯片容量
#define W25Q32_CHIP_SIZE            (32U  / 8U * 1024U * 1024U)                 //W25Q32芯片容量
#define W25Q64_CHIP_SIZE            (64U  / 8U * 1024U * 1024U)                 //W25Q64芯片容量
#define W25Q128_CHIP_SIZE           (128U / 8U * 1024U * 1024U)                 //W25Q128芯片容量
#define W25Q256_CHIP_SIZE           (256U / 8U * 1024U * 1024U)                 //W25Q256芯片容量

/********************************************************************************************
 *  芯片操作用时
 ********************************************************************************************/
#define W25QXX_PROGRAM_PAGE_TIME    3U             //页编程最大用时，单位ms
#define W25QXX_ERASE_SECTOR_TIME    400U           //擦除一个Sector最大用时，单位ms
#define W25QXX_ERASE_32KBLOCK_TIME  1600U          //擦除一个32K Block最大用时，单位ms（以W25Q256为准）
#define W25QXX_ERASE_64KBLOCK_TIME  2000U          //擦除一个64K Block最大用时，单位ms（以W25Q256为准）
#define W25QXX_ERASE_CHIP_TIME      (400U * 1000U) //擦除芯片最大用时，单位ms（以W25Q256为准）

/********************************************************************************************
 *  常用指令集
 ********************************************************************************************/
#define W25QXX_CMD_MANUFACT_ID      0x90 //读制造商ID
#define W25QXX_CMD_READ_SR1         0x05 //读状态寄存器1
#define W25QXX_CMD_WRITE_SR1        0x01 //写状态寄存器1
#define W25QXX_CMD_READ_SR2         0x35 //读状态寄存器2
#define W25QXX_CMD_WRITE_SR2        0x31 //写状态寄存器2
#define W25QXX_CMD_READ_DATA        0x03 //标准单线读
#define W25QXX_CMD_FAST_READ_DATA   0x0B //快速单线读
#define W25QXX_CMD_FAST_READ_DUAL   0x3B //快速双线读
#define W25QXX_CMD_FAST_READ_QUAD   0x6B //快速四线读
#define W25QXX_CMD_PAGE_PROGRAM     0x02 //页编程
#define W25QXX_CMD_SECTOR_ERASE     0x20 //Sector擦除
#define W25QXX_CMD_32KBLOCK_ERASE   0x52 //32K Block擦除
#define W25QXX_CMD_64KBLOCK_ERASE   0xD8 //64K Block擦除
#define W25QXX_CMD_CHIP_ERASE       0x60 //整片擦除
#define W25QXX_CMD_ENABLE_WRITE     0x06 //写使能
#define W25QXX_CMD_DISABLE_WRITE    0x04 //写失能
#define W25QXX_CMD_ENTER_4BYTE      0xB7 //进入4Byte模式（仅W25Q256支持）
#define W25QXX_CMD_EXIT_4BYTE       0xE9 //退出4Byte模式（仅W25Q256支持）

/********************************************************************************************
 *  状态寄存器1位定义
 ********************************************************************************************/
#define W25QXX_SR1_BIT_BUSY         0x01 //忙标记位(1,忙;0,空闲)
#define W25QXX_SR1_BIT_WEL          0x02 //写使能锁定位
#define W25QXX_SR1_BIT_BP0          0x04 //Block保护位0
#define W25QXX_SR1_BIT_BP1          0x08 //Block保护位1
#define W25QXX_SR1_BIT_BP2          0x10 //Block保护位2
#define W25QXX_SR1_BIT_TB           0x20 //Block保护位
#define W25QXX_SR1_BIT_SEC          0x40 //Sector/Block保护位
#define W25QXX_SR1_BIT_SRP0         0x80 //状态寄存器保护位0，配合WP使用
/********************************************************************************************
 *  等待就绪时间，单位ms
 ********************************************************************************************/
#define W25QXX_WAIT_READY_TIME  100U
#define QSPI_ROM_MAP_BASE_ADDR  (0x98000000u)
#define QSPI_ROM_MAP_WIN_SIZE   (64u * 1024u *1024u)
/********************************************************************************************
*                                  函数声明
********************************************************************************************/
#define LFS_FLASH_OFFS (0x1400000)   //外部flash偏移地址

#define RUN_TIME_FLASH_OFFS1 (0x13F1000)   //存储运行时间flash偏移地址1
#define RUN_TIME_FLASH_OFFS2 (0x13F2000)   //存储运行时间flash偏移地址2
#define RUN_TIME_FLASH_OFFS3 (0x13F3000)   //存储运行时间flash偏移地址3

void dev_w25qxx_init(void);
uint32_t dev_w25qxx_read_data(uint32_t startAddr, uint8_t *readData, uint32_t dataNum);
uint32_t dev_w25qxx_write_data(uint32_t startAddr, const uint8_t *writeData, uint32_t dataNum);
uint32_t dev_w25qxx_erase_sectors(uint32_t startSector, uint32_t sectorNum);
uint32_t dev_w25qxx_erase_64sectors(uint32_t startaddr);
void dev_w25qxx_lfs_erase_sector(uint32_t sector);
uint16_t dev_w25qxx_read_id(void);
void dev_w25qxx_erase_chip(void);
uint32_t dev_w25qxx_get_mapaddr(uint32_t extAddr, const void **mapPointer);

#endif
