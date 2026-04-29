#ifndef _BSP_W25QXX_Q_H_
#define _BSP_W25QXX_Q_H_

#include "hc32_ddl.h"

#if 1

/*
 * QSPI read mode and W25Q64 instruction.
 *
 *     QSPI_Read_Mode                   	  W25Q64_Instruction                 		W25Q64_Instruction_Dummy_Cycles
 *     QSPI_READ_STANDARD_READ          |     W25Q64_READ_DATA              |          (Ignore)
 *     QSPI_READ_FAST_READ              |     W25Q64_FAST_READ              |             8
 *     QSPI_READ_FAST_READ_DUAL_OUTPUT  |     W25Q64_FAST_READ_DUAL_OUTPUT  |             8
 *     QSPI_READ_FAST_READ_DUAL_IO      |     W25Q64_FAST_READ_DUAL_IO      |             4
 *     QSPI_READ_FAST_READ_QUAD_OUTPUT  |     W25Q64_FAST_READ_QUAD_OUTPUT  |             8
 *     QSPI_READ_FAST_READ_QUAD_IO      |     W25Q64_FAST_READ_QUAD_IO      |             6
 */
#define APP_QSPI_READ_MODE                QSPI_READ_FAST_READ_QUAD_OUTPUT//  (QSPI_READ_FAST_READ_DUAL_OUTPUT)
#define APP_W25Q64_READ_INSTR             W25Q64_FAST_READ_QUAD_OUTPUT//  (W25Q64_FAST_READ_DUAL_OUTPUT)
#define APP_W25Q64_READ_INSTR_DUMMY_CYCLES  (8U)

/* Pin definitions. */
#define QSPI_CS_PORT                (GPIO_PORT_C)
#define QSPI_CS_PIN                 (GPIO_PIN_07)
#define QSPI_SCK_PORT               (GPIO_PORT_B)
#define QSPI_SCK_PIN                (GPIO_PIN_14)
#define QSPI_IO0_PORT               (GPIO_PORT_D)
#define QSPI_IO0_PIN                (GPIO_PIN_08)
#define QSPI_IO1_PORT               (GPIO_PORT_D)
#define QSPI_IO1_PIN                (GPIO_PIN_09)
#define QSPI_IO2_PORT               (GPIO_PORT_D)
#define QSPI_IO2_PIN                (GPIO_PIN_10)
#define QSPI_IO3_PORT               (GPIO_PORT_D)
#define QSPI_IO3_PIN                (GPIO_PIN_11)
#define QSPI_PIN_FUNC               (GPIO_FUNC_18_QSPI)


#define APP_TEST_DATA_SIZE                  (1024UL)
/* External QSPI flash address definition. */
#define APP_TEST_ADDRESS                   (0x80000UL)

/**
 * @defgroup W25Q64_Standard_SPI_Instructions W25Q64 Standard SPI Instructions
 * @{
 */		
#define W25Q64_WRITE_ENABLE                             (0x06U)
#define W25Q64_VOLATILE_SR_WRITE_ENABLE                 (0x50U)
#define W25Q64_WRITE_DISABLE                            (0x04U)
#define W25Q64_RELEASE_POWER_DOWN_ID                    (0xABU)
#define W25Q64_MANUFACTURER_DEVICE_ID                   (0x90U)
#define W25Q64_JEDEC_ID                                 (0x9FU)
#define W25Q64_READ_UNIQUE_ID                           (0x4BU)
#define W25Q64_READ_DATA                                (0x03U)
#define W25Q64_FAST_READ                                (0x0BU)
#define W25Q64_PAGE_PROGRAM                             (0x02U)
#define W25Q64_SECTOR_ERASE                             (0x20U)
#define W25Q64_BLOCK_ERASE_32KB                         (0x52U)
#define W25Q64_BLOCK_ERASE_64KB                         (0xD8U)
#define W25Q64_CHIP_ERASE                               (0xC7U)
#define W25Q64_READ_STATUS_REGISTER_1                   (0x05U)
#define W25Q64_WRITE_STATUS_REGISTER_1                  (0x01U)
#define W25Q64_READ_STATUS_REGISTER_2                   (0x35U)
#define W25Q64_WRITE_STATUS_REGISTER_2                  (0x31U)
#define W25Q64_READ_STATUS_REGISTER_3                   (0x15U)
#define W25Q64_WRITE_STATUS_REGISTER_3                  (0x11U)
#define W25Q64_READ_SFDP_REGISTER                       (0x5AU)
#define W25Q64_ERASE_SECURITY_REGISTER                  (0x44U)
#define W25Q64_PROGRAM_SECURITY_REGISTER                (0x42U)
#define W25Q64_READ_SECURITY_REGISTER                   (0x48U)
#define W25Q64_GLOBAL_BLOCK_LOCK                        (0x7EU)
#define W25Q64_GLOBAL_BLOCK_UNLOCK                      (0x98U)
#define W25Q64_READ_BLOCK_LOCK                          (0x3DU)
#define W25Q64_INDIVIDUAL_BLOCK_LOCK                    (0x36U)
#define W25Q64_INDIVIDUAL_BLOCK_UNLOCK                  (0x39U)
#define W25Q64_ERASE_PROGRAM_SUSPEND                    (0x75U)
#define W25Q64_ERASE_PROGRAM_RESUME                     (0x7AU)
#define W25Q64_POWER_DOWN                               (0xB9U)
#define W25Q64_ENABLE_RESET                             (0x66U)
#define W25Q64_RESET_DEVICE                             (0x99U)

/**
 * @defgroup W25Q64_Dual_Quad_SPI_Instruction W25Q64 Dual Quad SPI Instruction
 * @{
 */
#define W25Q64_FAST_READ_DUAL_OUTPUT                    (0x3BU)
#define W25Q64_FAST_READ_DUAL_IO                        (0xBBU)
#define W25Q64_MFTR_DEVICE_ID_DUAL_IO                   (0x92U)
#define W25Q64_QUAD_INPUT_PAGE_PROGRAM                  (0x32U)
#define W25Q64_FAST_READ_QUAD_OUTPUT                    (0x6BU)
#define W25Q64_MFTR_DEVICE_ID_QUAD_IO                   (0x94U)
#define W25Q64_FAST_READ_QUAD_IO                        (0xEBU)
#define W25Q64_SET_BURST_WITH_WRAP                      (0x77U)

/**
 * @defgroup W25Q64_Size W25Q64 Size
 * @{
 */
#define W25Q64_PAGE_SIZE                                (256UL)
#define W25Q64_SECTOR_SIZE                              (1024UL * 4UL)
#define W25Q64_BLOCK_SIZE                               (1024UL * 64UL)
#define W25Q64_PAGE_PER_SECTOR                          (W25Q64_SECTOR_SIZE / W25Q64_PAGE_SIZE)

/**
 * @defgroup W25Q64_Status_Flag W25Q64 Status Flag
 * @{
 */
#define W25Q64_FLAG_BUSY                                (0x01U)
#define W25Q64_FLAG_SUSPEND                             (0x80U)

/**
 * @defgroup W25Q64_Miscellaneous_Macros W25Q64 Miscellaneous Macros
 * @{
 */
#define W25Q64_UNIQUE_ID_SIZE                           (8U)
#define W25Q64_DUMMY_BYTE_VALUE                         (0xFFU)	
		
		
#define GD25Q26_QSPI_ENABLE                            	(0x02U) //(0x02U)

#define W25X_Enable4ByteAddr    0xB7

void Qspi_Init(void);
void Qspi_test(void);
void Lfs_Write(void);
void Lfs_Read(void);
en_result_t W25Q64_EraseSector(uint32_t u32SectorAddress);
void W25Q64_ReadData(uint32_t u32Address, uint8_t pu8ReadBuf[], uint32_t u32NumByteToRead);
en_result_t W25Q64_WriteData(uint32_t u32Address, const uint8_t pu8WriteBuf[], uint32_t u32NumByteToWrite);
uint16_t W25Qxx_ReadID(void);

#endif

#endif  /* _BSP_W25QXX_Q_H_ */
