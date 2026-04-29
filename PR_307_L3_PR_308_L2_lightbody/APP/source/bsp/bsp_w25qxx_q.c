#include "bsp_w25qxx_q.h"
#include "lfs_flash.h"

#if 1

void W25QXX_Init(void);
/* QSPI initialization configuration. */
static void Qspi_InitConfig(void)
{
    stc_qspi_init_t stcInit;

    PWC_Fcg1PeriphClockCmd(PWC_FCG1_QSPI, Enable);

    (void)QSPI_StructInit(&stcInit);
    stcInit.u32ClkDiv        = 6UL; 
    stcInit.u32ReadMode      = APP_QSPI_READ_MODE;
    stcInit.u8RomAccessInstr = APP_W25Q64_READ_INSTR;
    stcInit.u32DummyCycles   = APP_W25Q64_READ_INSTR_DUMMY_CYCLES;
    (void)QSPI_Init(&stcInit);
}

/* QSPI pin configuration. */
static void Qspi_PinConfig(void)
{
    stc_gpio_init_t stcGpioInit;
    (void)GPIO_StructInit(&stcGpioInit);
    stcGpioInit.u16PullUp = PIN_PU_ON;
    stcGpioInit.u16PinDrv = PIN_DRV_HIGH;
    (void)GPIO_Init(QSPI_IO0_PORT, QSPI_IO0_PIN|QSPI_IO1_PIN|QSPI_IO2_PIN|QSPI_IO3_PIN, &stcGpioInit);
	(void)GPIO_Init(QSPI_CS_PORT, QSPI_CS_PIN|QSPI_SCK_PIN, &stcGpioInit);
	
    GPIO_SetFunc(QSPI_CS_PORT, QSPI_CS_PIN, QSPI_PIN_FUNC, PIN_SUBFUNC_DISABLE);
    GPIO_SetFunc(QSPI_SCK_PORT, QSPI_SCK_PIN, QSPI_PIN_FUNC, PIN_SUBFUNC_DISABLE);
    GPIO_SetFunc(QSPI_IO0_PORT, QSPI_IO0_PIN, QSPI_PIN_FUNC, PIN_SUBFUNC_DISABLE);
    GPIO_SetFunc(QSPI_IO1_PORT, QSPI_IO1_PIN, QSPI_PIN_FUNC, PIN_SUBFUNC_DISABLE);
    GPIO_SetFunc(QSPI_IO2_PORT, QSPI_IO2_PIN, QSPI_PIN_FUNC, PIN_SUBFUNC_DISABLE);
    GPIO_SetFunc(QSPI_IO3_PORT, QSPI_IO3_PIN, QSPI_PIN_FUNC, PIN_SUBFUNC_DISABLE);
}

/* QSPI初始化函数 */
void Qspi_Init(void)
{
    Qspi_InitConfig();
    Qspi_PinConfig();
	W25Qxx_ReadID();
	W25QXX_Init();
}

void W25Q64_WriteCommand(uint8_t u8Command, uint32_t au32Address)
{
    QSPI_EnterDirectCommMode();
    QSPI_WriteDirectCommValue(u8Command);

    QSPI_WriteDirectCommValue((au32Address & 0xFF0000) >> 16);
    QSPI_WriteDirectCommValue((au32Address & 0xFF00) >> 8);
    QSPI_WriteDirectCommValue(au32Address & 0xFF);
    QSPI_ExitDirectCommMode();
}

void W25Q64_WriteEnable(void)
{
	QSPI_EnterDirectCommMode();
    QSPI_WriteDirectCommValue(W25Q64_WRITE_ENABLE);
	QSPI_ExitDirectCommMode();
}

en_result_t W25Q64_CheckProcessDone(void)
{
    uint8_t u8Status;
    uint32_t u32TimeCnt;
    stc_clk_freq_t stcClkFreq;
    en_result_t enRet = ErrorTimeout;

    (void)CLK_GetClockFreq(&stcClkFreq);
    u32TimeCnt = stcClkFreq.sysclkFreq / 100U;

    QSPI_EnterDirectCommMode();
    QSPI_WriteDirectCommValue(W25Q64_READ_STATUS_REGISTER_1);
    while ((u32TimeCnt--) != 0UL)
    {
        u8Status = QSPI_ReadDirectCommValue();
        if ((u8Status & W25Q64_FLAG_BUSY) == 0U)
        {
            enRet = Ok;
            break;
        }
    }
    QSPI_ExitDirectCommMode();

    return enRet;
}

/**
 * @brief  W25Q64 read data.
 * @param  [in]  u32Address             Target address.
 * @param  [in]  pu8ReadBuf             Destination data buffer.
 * @param  [in]  u32NumByteToRead       Number of byte to be read.
 * @retval None
 */
void W25Q64_ReadData(uint32_t u32Address, uint8_t pu8ReadBuf[], uint32_t u32NumByteToRead)
{
    QSPI_SetReadMode(APP_QSPI_READ_MODE, APP_W25Q64_READ_INSTR, APP_W25Q64_READ_INSTR_DUMMY_CYCLES);
    (void)QSPI_ReadData(u32Address, pu8ReadBuf, u32NumByteToRead);
}

/* 擦除扇区，参数：扇区号 */
en_result_t W25Q64_EraseSector(uint32_t u32SectorAddress)
{
	u32SectorAddress *= 4096;
    W25Q64_WriteEnable();
    W25Q64_WriteCommand(W25Q64_SECTOR_ERASE, u32SectorAddress);
    return W25Q64_CheckProcessDone();
}

/**
 * @brief  W25Q64 erase sector.
 */
en_result_t W25Q64_EraseBlock(uint8_t u8EraseBlockInstr, uint32_t u32BlockAddress)
{
    W25Q64_WriteEnable();
    W25Q64_WriteCommand(u8EraseBlockInstr, u32BlockAddress);
    return W25Q64_CheckProcessDone();
}

/* 重构QSPI写函数，只用于3字节地址（此项目是8M外部Flash） */
static en_result_t QSPI_Write_Data(uint32_t u32Instr, uint32_t u32Address, const uint8_t pu8Src[], uint32_t u32SrcSize)
{
    uint32_t i;
    en_result_t enRet = ErrorInvalidParameter;

    if ((pu8Src != NULL) && (u32SrcSize > 0U))
    {
        /* Enter direct communication mode. */
        QSPI_EnterDirectCommMode();
        /* Send instruction. */
        WRITE_REG32(M4_QSPI->DCOM, u32Instr);
        /* Send ROM address. */
		WRITE_REG32(M4_QSPI->DCOM, ((u32Address & 0xFF0000UL) >> 16));
		WRITE_REG32(M4_QSPI->DCOM, ((u32Address & 0xFF00U) >> 8));
		WRITE_REG32(M4_QSPI->DCOM, (u32Address & 0xFF));
		
        /* Write data at last. */
        for (i=0U; i<u32SrcSize; i++)
        {
            WRITE_REG32(M4_QSPI->DCOM, pu8Src[i]);
        }
        /* Exit direct communication mode. */
        QSPI_ExitDirectCommMode();

        enRet = Ok;
    }

    return enRet;
}

//en_result_t W25Q64_WriteData(uint32_t u32Address, const uint8_t pu8WriteBuf[], uint32_t u32NumByteToWrite)
//{
//    uint32_t u32TempSize;
//    uint32_t u32AddrOfst = 0U;
//    en_result_t enRet = ErrorInvalidParameter;

//    if (pu8WriteBuf != NULL)
//    {
//        while (u32NumByteToWrite != 0UL)
//        {
//            if (u32NumByteToWrite >= W25Q64_PAGE_SIZE)
//            {
//                u32TempSize = W25Q64_PAGE_SIZE;
//            }
//            else
//            {
//                u32TempSize = u32NumByteToWrite;
//            }

//            W25Q64_WriteEnable();
//            (void)QSPI_Write_Data(W25Q64_PAGE_PROGRAM, u32Address, (uint8_t *)&pu8WriteBuf[u32AddrOfst], u32TempSize);
//            enRet = W25Q64_CheckProcessDone();
//            if (enRet != Ok)
//            {
//                break;
//            }
//            u32NumByteToWrite -= u32TempSize;
//            u32AddrOfst += u32TempSize;
//            u32Address += u32TempSize;
//        }
//    }

//    return enRet;
//	
//}

en_result_t W25Q64_WriteData(uint32_t u32Address, const uint8_t pu8WriteBuf[], uint32_t u32NumByteToWrite)
{
    uint32_t u32AddrOfst = 0U;
    en_result_t enRet = ErrorInvalidParameter;
	uint32_t remain_size = W25Q64_PAGE_SIZE - u32Address % W25Q64_PAGE_SIZE;

	if(remain_size >= u32NumByteToWrite)
		remain_size = u32NumByteToWrite;
	
    if (pu8WriteBuf != NULL)
    {
        while (u32NumByteToWrite != 0UL)
        {
            W25Q64_WriteEnable();
            (void)QSPI_Write_Data(W25Q64_PAGE_PROGRAM, u32Address, (uint8_t *)&pu8WriteBuf[u32AddrOfst], remain_size);
            enRet = W25Q64_CheckProcessDone();
            if (enRet != Ok)
            {
                break;
            }
			u32NumByteToWrite -= remain_size;
			u32AddrOfst += remain_size;
            u32Address += remain_size;
			
			if(u32NumByteToWrite >= W25Q64_PAGE_SIZE)
				remain_size = W25Q64_PAGE_SIZE;
			else
				remain_size = u32NumByteToWrite;
        }
    }

    return enRet;
}



//0XEF13,表示芯片型号为W25Q80
//0XEF14,表示芯片型号为W25Q16
//0XEF15,表示芯片型号为W25Q32
//0XEF16,表示芯片型号为W25Q64
//0XEF17,表示芯片型号为W25Q128
//0XEF18,表示芯片型号为W25Q256
uint16_t W25Qxx_ReadID(void)
{
    uint16_t device_id = 0x0000;
    uint32_t i;

    QSPI_EnterDirectCommMode();
    QSPI_WriteDirectCommValue(W25Q64_MANUFACTURER_DEVICE_ID);

    /* Four dummy bytes. */
    for (i=0UL; i<3UL; i++)  
    {
        QSPI_WriteDirectCommValue(0x00U);  
    }
	device_id |= QSPI_ReadDirectCommValue() << 8;
	device_id |= QSPI_ReadDirectCommValue();
    QSPI_ExitDirectCommMode();
	printf("%x",device_id);
	return device_id;
}


//uint8_t W25qxxAddrWidth = 0;
//初始化SPI FLASH的IO口
void W25QXX_Init(void)
{
	uint8_t temp;


/*32M Flash 才是使用到该指令*/
//		QSPI_EnterDirectCommMode();
////状态寄存器3：
////BIT7      6    5    4   3   2   1   0
////HOLD/RST  DRV1 DRV0 (R) (R) WPS ADP ADS		
//		QSPI_WriteDirectCommValue(W25Q64_READ_STATUS_REGISTER_2);

//		temp = QSPI_ReadDirectCommValue();
//		QSPI_ExitDirectCommMode();
//		if ((temp & 0x01) == 0U)
//		{
//			W25Q64_WriteEnable();
//			W25Q64_WriteCommand(W25X_Enable4ByteAddr, NULL);
//			W25Q64_CheckProcessDone();

//		}
		
		QSPI_EnterDirectCommMode();
		QSPI_WriteDirectCommValue(W25Q64_READ_STATUS_REGISTER_2);  //QE bit 置1  开启QSPI模式
		temp = QSPI_ReadDirectCommValue();
		QSPI_ExitDirectCommMode();
		if ((temp & GD25Q26_QSPI_ENABLE) == 0U)
		{
            W25Q64_WriteEnable(); 
			QSPI_EnterDirectCommMode();
			temp |= GD25Q26_QSPI_ENABLE;
			QSPI_WriteDirectCommValue(W25Q64_WRITE_STATUS_REGISTER_2);
			QSPI_WriteDirectCommValue(temp);
			W25Q64_CheckProcessDone();
			QSPI_ExitDirectCommMode();
		}	
//        W25Q64_WriteEnable(); //擦除整个Flash
//        W25Q64_WriteCommand(0xC7,NULL);
//        W25Q64_CheckProcessDone();    
        
        

//        W25qxxAddrWidth = (READ_REG32_BIT(M4_QSPI->FCR, QSPI_FCR_AWSL) >> QSPI_FCR_AWSL_POS) + 1U;
}


#if  1

uint8_t m_au8WriteData[APP_TEST_DATA_SIZE] = {0};
uint8_t m_au8ReadData[APP_TEST_DATA_SIZE] = {0};
static uint32_t u32Start = 0U;

static void AppClearData(void)
{
    uint32_t i;
    for (i=0UL; i<APP_TEST_DATA_SIZE; i++)
    {
        m_au8ReadData[i] = 0U;
    }
}

static void AppCheckData(void)
{
    uint32_t i;
    for (i=0UL; i<APP_TEST_DATA_SIZE; i++)
    {
        if(m_au8ReadData[i] != m_au8WriteData[i])
			printf("[%d]= %-4d", i, m_au8ReadData[i]);
    }
	printf("运行完一次:%d--%d\n", u32Start,m_au8ReadData[0]);
}

static void AppLoadData(void)
{
    uint32_t i;
    
    u32Start++;
	for (i=0UL; i<APP_TEST_DATA_SIZE; i++)
    {
        m_au8WriteData[i] = u32Start+6U;
    }
	printf("更新赋值:%d--%d\n", u32Start,m_au8WriteData[0]);
	
}
void Qspi_test(void)
{
	W25Q64_ReadData(APP_TEST_ADDRESS, m_au8ReadData, APP_TEST_DATA_SIZE);
	AppLoadData();
	W25Q64_EraseSector(128);
	W25Q64_WriteData(APP_TEST_ADDRESS, m_au8WriteData, APP_TEST_DATA_SIZE);
	W25Q64_ReadData(APP_TEST_ADDRESS, m_au8ReadData, APP_TEST_DATA_SIZE);
	AppClearData();
	W25Q64_EraseSector(128);
	W25Q64_ReadData(APP_TEST_ADDRESS, m_au8ReadData, APP_TEST_DATA_SIZE);
	AppClearData();
    W25Q64_WriteData(APP_TEST_ADDRESS, m_au8WriteData, APP_TEST_DATA_SIZE);
	W25Q64_ReadData(APP_TEST_ADDRESS, m_au8ReadData, APP_TEST_DATA_SIZE);
	AppCheckData();
}

#endif

#endif

