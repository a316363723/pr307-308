#include "hal_qspi.h"
#include "hc32_ddl.h"

//检查时钟分频
#define IS_QSPI_CLK_DIVIDER(x)  (((x) >= 2U) && ((x) <= 64U))
//检查地址宽度
#define IS_QSPI_ADDR_WIDTH(x)   (((x) == QSPI_ADDR_WIDTH_1BYTE) || \
                                 ((x) == QSPI_ADDR_WIDTH_2BYTE) || \
																 ((x) == QSPI_ADDR_WIDTH_3BYTE) || \
																 ((x) == QSPI_ADDR_WIDTH_4BYTE))

/*
功能：QSPI初始化
参数：无
返回值：无
*/
void hal_qspi_init(void)
{
	stc_qspi_init_t qspiinitstruct;
//使能设备时钟        
	PWC_Fcg1PeriphClockCmd(PWC_FCG1_QSPI, Enable);
//复位设备
	QSPI_DeInit();
//配置QSPI
	QSPI_StructInit(&qspiinitstruct);
	qspiinitstruct.u32ClkDiv = 3;
	qspiinitstruct.u32ReadMode      = QSPI_READ_FAST_READ_DUAL_OUTPUT;
    qspiinitstruct.u8RomAccessInstr = 0x3BU;
    qspiinitstruct.u32DummyCycles   = 8;
	QSPI_Init(&qspiinitstruct);
}

/*
功能：QSPI通信模式下传输数据
参数：@u8Val：传输的数据
返回值：无
*/
void hal_qspi_communication_mode_write_data(uint8_t u8Val)
{
	QSPI_WriteDirectCommValue(u8Val);
}

/*
功能：QSPI通信模式下读取数据
参数：无
返回值：rx_data
*/
uint8_t hal_qspi_communication_mode_read_data(void)
{
	uint8_t rx_data;
	
	rx_data = QSPI_ReadDirectCommValue();
	
	return rx_data;
}

/*
功能：QSPI传输数据
参数：@u8Val：传输的数据
返回值：无
*/
void hal_qspi_write_data(uint32_t Instr, uint32_t address, const uint8_t pu8Src[], uint32_t srcSize)
{
	QSPI_WriteData(Instr, address, pu8Src, srcSize);
}

/*
功能：QSPI读取数据
参数：无
返回值：rx_data
*/
void hal_qspi_read_data(uint32_t address, uint8_t pu8Dest[], uint32_t dest_Size)
{
	if(pu8Dest == 0)
	{
		return;
	}
	QSPI_ReadData(address, pu8Dest, dest_Size);
}

/*
功能：使能直接通信模式
参数：无
返回值：无
*/
void hal_qspi_direct_mode_enable(void)
{
	QSPI_EnterDirectCommMode();
}

/*
功能：失能直接通信模式
参数：无
返回值：无
*/
void hal_qspi_direct_mode_disable(void)
{
	QSPI_ExitDirectCommMode();
}

/*
功能：设置分频
参数：@clkdiv ：分频大小
返回值：无
*/
void hal_qspi_set_clk_div(uint32_t clkdiv)
{
	DDL_ASSERT(IS_QSPI_CLK_DIVIDER(clkdiv));
	MODIFY_REG32(M4_QSPI->CR, QSPI_CR_DIV, (clkdiv - 1U) << QSPI_CR_DIV_POS);
}

/*
功能：设置地址宽度
参数：@addr_width ：地址宽度
返回值：无
*/
void hal_qspi_set_addrwidth(uint32_t addr_width)
{
	DDL_ASSERT(IS_QSPI_ADDR_WIDTH(addr_width));
	MODIFY_REG32(M4_QSPI->FCR, QSPI_FCR_AWSL, addr_width);
}

/*
功能：设置分频
参数：@clkdiv ：分频大小
返回值：无
*/
uint8_t hal_qspi_get_addrwidth(void)
{
	uint32_t addrWidth = READ_REG32_BIT(M4_QSPI->FCR, QSPI_FCR_AWSL);
	switch(addrWidth)
	{
		case QSPI_ADDR_WIDTH_1BYTE: return 1;
		case QSPI_ADDR_WIDTH_2BYTE: return 2;
		case QSPI_ADDR_WIDTH_3BYTE: return 3;
		case QSPI_ADDR_WIDTH_4BYTE: return 4;
		default                   : return 4;
	}
}

void hal_qspi_set_read_mode(uint32_t u32ReadMode, uint8_t u8ReadInstr, uint32_t u32DummyCycles)
{
	QSPI_SetReadMode(u32ReadMode, u8ReadInstr, u32DummyCycles);
}

void hal_qspi_selects_block(uint32_t u32Block)
{
	QSPI_SelectBlock(u32Block);
}
