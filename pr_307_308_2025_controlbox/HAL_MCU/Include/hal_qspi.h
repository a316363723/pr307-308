#ifndef HAL_QSPI_H
#define HAL_QSPI_H

#include <stdint.h>

#define QSPI_ROM_WINDOW_SIZE  (64U * 1024U * 1024U)

/********************************************************************************************
*                                  º¯ÊýÉùÃ÷
********************************************************************************************/
void hal_qspi_init(void);
void hal_qspi_write_data(uint32_t Instr, uint32_t address, const uint8_t pu8Src[], uint32_t SrcSize);
void hal_qspi_communication_mode_write_data(uint8_t u8Val);
void hal_qspi_direct_mode_enable(void);
void hal_qspi_direct_mode_disable(void);
void hal_qspi_set_clk_div(uint32_t clkdiv);
void hal_qspi_set_addrwidth(uint32_t addr_width);
void hal_qspi_set_read_mode(uint32_t u32ReadMode, uint8_t u8ReadInstr, uint32_t u32DummyCycles);
void hal_qspi_selects_block(uint32_t u32Block);
uint8_t hal_qspi_get_addrwidth(void);
uint8_t hal_qspi_communication_mode_read_data(void);
void hal_qspi_read_data(uint32_t address, uint8_t pu8Dest[], uint32_t dest_Size);

#endif
