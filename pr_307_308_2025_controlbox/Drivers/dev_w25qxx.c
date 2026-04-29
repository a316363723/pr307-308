#include "dev_w25qxx.h"
#include "hc32_ddl.h"
#include "hal_qspi.h"
#include "stdbool.h"
#include "perf_counter.h"
#include "cmsis_os.h"

static uint32_t W25QxxChipSize = 0;

static void dev_w25qxx_quadfast_read(uint32_t startAddr, uint8_t *readData, uint32_t dataNum);
static uint8_t dev_w25qxx_read_status_reg1(void);
static uint8_t dev_w25qxx_read_status_reg2(void);
static void dev_w25qxx_write_status_reg1(uint8_t value);
static void dev_w25qxx_write_status_reg2(uint8_t value);
static bool dev_w25qxx_wait_busy(uint32_t timeout);
static void dev_w25qxx_write_cmd(uint8_t cmd, const uint8_t *param, uint8_t paramLen);
static void dev_w25qxx_program_page(uint32_t startAddr, const uint8_t *writeData, uint16_t dataNum);
static void dev_w25qxx_enable_write(void);
static void dev_w25qxx_disable_write(void);
static void dev_w25qxx_enable_4biyte(void);
static void dev_w25qxx_disable_4biyte(void);
static void dev_w25qxx_enable_4line(void);
void dev_w25qxx_erase_sector(uint32_t sector);
static void dev_w25qxx_erase_64sector(uint32_t addr);

typedef __packed union //状态寄存器2
{
//寄存器域
	__packed struct
	{
		uint8_t m_srp1 : 1; //状态寄存器保护位1，配合WP使用
		uint8_t m_qe   : 1; //四线操作使能
		uint8_t m_sus2 : 1;
		uint8_t m_lb1  : 1;
		uint8_t m_lb2  : 1;
		uint8_t m_lb3  : 1;
		uint8_t m_cmp  : 1;
		uint8_t m_sus1 : 1;
	}m_region;
//寄存器值
	uint8_t m_value;
}w25qxx_status_reg2_type;

/*
功能：W25QXX初始化
参数：无
返回值：无
*/
void dev_w25qxx_init(void)
{
	stc_gpio_init_t w25qxxPinInitStruct;
/*** 引脚初始化 ***/
	GPIO_StructInit(&w25qxxPinInitStruct);
	w25qxxPinInitStruct.u16PullUp = PIN_PU_ON;
	w25qxxPinInitStruct.u16PinDrv = PIN_DRV_HIGH;
//SCK
	GPIO_Init(W25QXX_SCK_PORT, W25QXX_SCK_PIN, &w25qxxPinInitStruct);
	GPIO_SetFunc(W25QXX_SCK_PORT, W25QXX_SCK_PIN, W25QXX_SCK_AF, PIN_SUBFUNC_DISABLE);
//IO0
	GPIO_Init(W25QXX_IO0_PORT, W25QXX_IO0_PIN, &w25qxxPinInitStruct);
	GPIO_SetFunc(W25QXX_IO0_PORT, W25QXX_IO0_PIN, W25QXX_IO0_AF, PIN_SUBFUNC_DISABLE);
//IO1
	GPIO_Init(W25QXX_IO1_PORT, W25QXX_IO1_PIN, &w25qxxPinInitStruct);
	GPIO_SetFunc(W25QXX_IO1_PORT, W25QXX_IO1_PIN, W25QXX_IO1_AF, PIN_SUBFUNC_DISABLE);
//IO2
	GPIO_Init(W25QXX_IO2_PORT, W25QXX_IO2_PIN, &w25qxxPinInitStruct);
	GPIO_SetFunc(W25QXX_IO2_PORT, W25QXX_IO2_PIN, W25QXX_IO2_AF, PIN_SUBFUNC_DISABLE);
//IO3
	GPIO_Init(W25QXX_IO3_PORT, W25QXX_IO3_PIN, &w25qxxPinInitStruct);
	GPIO_SetFunc(W25QXX_IO3_PORT, W25QXX_IO3_PIN, W25QXX_IO3_AF, PIN_SUBFUNC_DISABLE);
//NSS
	GPIO_Init(W25QXX_NSS_PORT, W25QXX_NSS_PIN, &w25qxxPinInitStruct);
	GPIO_SetFunc(W25QXX_NSS_PORT, W25QXX_NSS_PIN, W25QXX_NSS_AF, PIN_SUBFUNC_DISABLE);
	
	hal_qspi_init();
	
	switch(dev_w25qxx_read_id())
	{
		case W25Q80_CHIP_ID : W25QxxChipSize = W25Q80_CHIP_SIZE;  break;
		case W25Q16_CHIP_ID : W25QxxChipSize = W25Q16_CHIP_SIZE;  break;
		case W25Q32_CHIP_ID : W25QxxChipSize = W25Q32_CHIP_SIZE;  break;
		case W25Q64_CHIP_ID : W25QxxChipSize = W25Q64_CHIP_SIZE;  break;
		case W25Q128_CHIP_ID: W25QxxChipSize = W25Q128_CHIP_SIZE; break;
		case W25Q256_CHIP_ID: W25QxxChipSize = W25Q256_CHIP_SIZE; break;
        case GD25Q256_CHIP_ID: W25QxxChipSize = W25Q256_CHIP_SIZE;break;
		default             : W25QxxChipSize = 0U;                break;
	}
	//使能4线操作
	dev_w25qxx_enable_4line();
    //设置4Byte模式（当W25Qxx容量超过3Byte地址时，就需要设置为4Byte地址模式）
	if(W25QxxChipSize > (0x00000001U << (3U * 8U)))
	{
		dev_w25qxx_enable_4biyte();
	}
}

/*
功能：读取W25QXX芯片ID
参数：无
返回值：@chipID：芯片ID值
*/
uint16_t dev_w25qxx_read_id(void)
{
	uint16_t chipID = 0x0000;
//进入直接模式
	hal_qspi_direct_mode_enable();
//发送命令
	hal_qspi_communication_mode_write_data(W25QXX_CMD_MANUFACT_ID);
//发送无用数据
	hal_qspi_communication_mode_write_data(0x00);
	hal_qspi_communication_mode_write_data(0x00);
	hal_qspi_communication_mode_write_data(0x00);
//读取ID
	chipID |= hal_qspi_communication_mode_read_data() << 8U;
	chipID |= hal_qspi_communication_mode_read_data() << 0U;
//退出直接模式
	hal_qspi_direct_mode_disable();
	
	return chipID;
}

/*
功能：读取W25QXX的数据
参数：@startAddr：开始读取的地址
      @readData：保存读取到数据的地址
	  @dataNum：读取数据的多少
返回值：@dataNum：读取到多少数据值
*/
uint32_t dev_w25qxx_read_data(uint32_t startAddr, uint8_t *readData, uint32_t dataNum)
{
//参数检查
	if((startAddr >= W25QxxChipSize) || (readData == NULL) || (dataNum == 0))
	{
		return 0;
	}
//数据处理
	dataNum = (dataNum > (W25QxxChipSize - startAddr)) ? (W25QxxChipSize - startAddr) : dataNum;
//读取数据
	dev_w25qxx_quadfast_read(startAddr, readData, dataNum);
	return dataNum;
}

/*
功能：直接写W25Qxx。调用本函数前必须保证目标空间已被擦除
参数：@startAddr：开始写的地址
      @readData：需要写入数据的数据地址
	  @dataNum：读取数据的多少
返回值：@writeCount：写入多少数据值
*/
uint32_t dev_w25qxx_write_data(uint32_t startAddr, const uint8_t *writeData, uint32_t dataNum)
{
	uint16_t writeNum;
	uint32_t writeCount;
//参数检查
	if((startAddr >= W25QxxChipSize) || (writeData == NULL) || (dataNum == 0))
	{
		return 0;
	}
//数据处理
	writeCount = 0;
	dataNum = (dataNum > (W25QxxChipSize - startAddr)) ? (W25QxxChipSize - startAddr) : dataNum;
//等待就绪
	if(dev_w25qxx_wait_busy(W25QXX_WAIT_READY_TIME) == false)
	{
		return 0;
	}
	while(dataNum > 0)
	{
//写使能
		dev_w25qxx_enable_write();
//写入数据
		writeNum = W25QXX_PAGE_SIZE - (startAddr % W25QXX_PAGE_SIZE);
		writeNum = (writeNum > dataNum) ? dataNum : writeNum;
//		hal_qspi_write_data(W25QXX_CMD_PAGE_PROGRAM, startAddr, writeData, dataNum);
		dev_w25qxx_program_page(startAddr, writeData, writeNum);
//等待写完成
		if(dev_w25qxx_wait_busy(W25QXX_PROGRAM_PAGE_TIME) == false)
		{
			break;
		}
//数据处理
		writeCount += writeNum;
		startAddr += writeNum;
		writeData += writeNum;
		dataNum -= writeNum;
	}
	return writeCount;
}

/*
功能：擦除W25Qxx扇区
参数：@startSector：起始扇区
      @sectorNum：擦除删除的数量
返回值：@eraseCount：擦除多少删除
*/
uint32_t dev_w25qxx_erase_sectors(uint32_t startSector, uint32_t sectorNum)
{
        uint32_t eraseCount;
        uint32_t sectorNumMax;
//参数检查
        sectorNumMax = W25QxxChipSize / W25QXX_SECTOR_SIZE;
        if((startSector >= sectorNumMax) || (sectorNum == 0))
        {
                return 0;
        }
//数据处理
        eraseCount = 0;
        sectorNum = (sectorNum > (sectorNumMax - startSector)) ? (sectorNumMax - startSector) : sectorNum;
//等待就绪
        if(dev_w25qxx_wait_busy(W25QXX_WAIT_READY_TIME) == false)
        {
                return 0;
        }
        while(sectorNum > 0)
        {
//写使能
                dev_w25qxx_enable_write();
//请求擦除
                dev_w25qxx_erase_sector(startSector);
//等待擦除完成
                if(dev_w25qxx_wait_busy(W25QXX_ERASE_SECTOR_TIME) == false)
                {
                        break;
                }
//数据处理
                ++eraseCount;
                ++startSector;
                --sectorNum;
        }
        return eraseCount;
}

/*
功能：擦除W25Qxx64k
参数：@startSector：起始扇区
      @sectorNum：擦除删除的数量
返回值：@eraseCount：擦除多少删除
*/
uint32_t dev_w25qxx_erase_64sectors(uint32_t startaddr)
{
        dev_w25qxx_enable_write();
    
        if(dev_w25qxx_wait_busy(W25QXX_WAIT_READY_TIME) == false)
        {
                return 0;
        }
        
        dev_w25qxx_erase_64sector(startaddr);
//等待擦除完成
        if(dev_w25qxx_wait_busy(W25QXX_ERASE_SECTOR_TIME) == false)
        {
                return 0;
        }
        
        return 1;
}

/*
功能：擦除整个W25Qxx
参数：无
返回值：无
*/
void dev_w25qxx_erase_chip(void)
{
	dev_w25qxx_enable_write();
	
	dev_w25qxx_write_cmd(W25QXX_CMD_CHIP_ERASE, NULL, 0);
	
	dev_w25qxx_wait_busy(W25QXX_ERASE_CHIP_TIME);
}

/*
功能：擦除1个扇区
参数：@sector：扇区的序号
返回值：无
*/
void dev_w25qxx_erase_sector(uint32_t sector)
{
	uint32_t addr;
	uint8_t addrWidth;
	uint8_t addrArr[4];
//数据处理
	addr = sector * W25QXX_SECTOR_SIZE;
	addrArr[0] = (addr >> 24U) & 0xFF;
	addrArr[1] = (addr >> 16U) & 0xFF;
	addrArr[2] = (addr >>  8U) & 0xFF;
	addrArr[3] = (addr >>  0U) & 0xFF;
	addrWidth = hal_qspi_get_addrwidth();
//写擦除命令
	dev_w25qxx_write_cmd(W25QXX_CMD_SECTOR_ERASE, addrArr + 4 - addrWidth, addrWidth);
}

/*
功能：擦除1个扇区
参数：@sector：扇区的序号
返回值：无
*/
void dev_w25qxx_lfs_erase_sector(uint32_t sector)
{
//	uint32_t addr;
	uint8_t addrWidth;
	uint8_t addrArr[4];
//数据处理
//	addr = sector * W25QXX_SECTOR_SIZE;
	addrArr[0] = (sector >> 24U) & 0xFF;
	addrArr[1] = (sector >> 16U) & 0xFF;
	addrArr[2] = (sector >>  8U) & 0xFF;
	addrArr[3] = (sector >>  0U) & 0xFF;
	addrWidth = hal_qspi_get_addrwidth();
//写擦除命令
	dev_w25qxx_write_cmd(W25QXX_CMD_SECTOR_ERASE, addrArr + 4 - addrWidth, addrWidth);
}

/*
功能：擦除1个扇区
参数：@sector：扇区的序号
返回值：无
*/
static void dev_w25qxx_erase_64sector(uint32_t addr)
{
	uint8_t addrWidth;
	uint8_t addrArr[4];
//数据处理
	addr *= W25QXX_64KBLOCK_SIZE;
	addrArr[0] = (addr >> 24U) & 0xFF;
	addrArr[1] = (addr >> 16U) & 0xFF;
	addrArr[2] = (addr >>  8U) & 0xFF;
	addrArr[3] = (addr >>  0U) & 0xFF;
	addrWidth = hal_qspi_get_addrwidth();
//写擦除命令
	dev_w25qxx_write_cmd(W25QXX_CMD_64KBLOCK_ERASE, addrArr + 4 - addrWidth, addrWidth);
}

/*
功能：快速读数据
参数：@startAddr：读取的起始地址
	  @readData：保存数据的地址
	  @dataNum：读取多少数据
返回值：@readNum：读取到多少数据
*/
static void dev_w25qxx_quadfast_read(uint32_t startAddr, uint8_t *readData, uint32_t dataNum)
{
	uint32_t readNum;
	uint32_t addrOffset;
//参数检查
	if((readData == NULL) || (dataNum == 0))
	{
		return;
	}
//配置读模式
	hal_qspi_set_read_mode(QSPI_READ_FAST_READ_QUAD_OUTPUT, W25QXX_CMD_FAST_READ_QUAD, 8);
	while(dataNum > 0)
	{
//设置映射Block
		hal_qspi_selects_block(startAddr / QSPI_ROM_WINDOW_SIZE);
//读取数据
		addrOffset = startAddr % QSPI_ROM_WINDOW_SIZE;
		readNum = QSPI_ROM_WINDOW_SIZE - addrOffset;
		readNum = (readNum > dataNum) ? dataNum : readNum;
		hal_qspi_read_data(addrOffset, readData, readNum);
//数据处理
		startAddr += readNum;
		readData += readNum;
		dataNum -= readNum;
	}
}

/*
功能：页编程
参数：@startAddr：编程的起始地址
	  @readData：写入数据的地址
	  @dataNum：写入多少数据
返回值：@readNum：成功写入多少数据
*/
static void dev_w25qxx_program_page(uint32_t startAddr, const uint8_t *writeData, uint16_t dataNum)
{
	uint8_t i;
	uint8_t addrWidth;
	uint8_t addrArr[4];
//参数检查
	if((writeData == NULL) || (dataNum == 0))
	{
		return;
	}
//数据处理
	addrArr[0] = (startAddr >> 24U) & 0xFF;
	addrArr[1] = (startAddr >> 16U) & 0xFF;
	addrArr[2] = (startAddr >>  8U) & 0xFF;
	addrArr[3] = (startAddr >>  0U) & 0xFF;
	addrWidth = hal_qspi_get_addrwidth();
//进入直接模式
	hal_qspi_direct_mode_enable();
//发送命令
	hal_qspi_communication_mode_write_data(W25QXX_CMD_PAGE_PROGRAM);
//写地址
	for(i = 4 - addrWidth; i < 4; ++i)
	{
		hal_qspi_communication_mode_write_data(addrArr[i]);
	}
//写数据
	while(dataNum > 0)
	{
		hal_qspi_communication_mode_write_data(*writeData);
		++writeData;
		--dataNum;
	}
//退出直接模式
	hal_qspi_direct_mode_disable();
}

/**
 *@brief  定位存储映射空间
 *@param  extAddr：外部存储空间地址
          mapAddr：保存映射空间地址
 *@return 从定位空间起可读取数据数量
 */
static uint32_t dev_w25qxx_locate_mapaddr(uint32_t extAddr, uint32_t *mapAddr)
{
        uint32_t addrOffset;
//数据处理
        addrOffset = extAddr % QSPI_ROM_MAP_WIN_SIZE;
//设置映射Block
        QSPI_SelectBlock(extAddr / QSPI_ROM_MAP_WIN_SIZE);
//进入ROM模式
        QSPI_ExitDirectCommMode();
//计算ROM映射地址
        if(mapAddr != NULL)
        {
                *mapAddr = QSPI_ROM_MAP_BASE_ADDR + addrOffset;
        }
        return (QSPI_ROM_MAP_WIN_SIZE - addrOffset);
}

/**
 *@brief  获取映射指针
 *@param  extAddr   ：外部存储地址
          mapPointer：保存映射指针
 *@return 从映射处起可读取数据数量
 */
uint32_t dev_w25qxx_get_mapaddr(uint32_t extAddr, const void **mapPointer)
{
        uint32_t mapAddr, mapSize;
        mapSize = dev_w25qxx_locate_mapaddr(extAddr, &mapAddr);
        if(mapPointer != NULL) 
            *mapPointer = (const void *)mapAddr;
        return mapSize;
}

/*
功能：等待W25Qxx空闲
参数：@timeout：等待的事件
返回值：@bool：是否等待成功。false-成功，true-失败
*/
static bool dev_w25qxx_wait_busy(uint32_t timeout)
{
	while((dev_w25qxx_read_status_reg1() & W25QXX_SR1_BIT_BUSY) == W25QXX_SR1_BIT_BUSY)
	{
		if(timeout == 0)
		{
			return false;
		}
		osDelay(1);
		--timeout;
	}
	return true;
}

/*
功能：使能4线操作
参数：无
返回值：无
*/
static void dev_w25qxx_enable_4line(void)
{
	w25qxx_status_reg2_type statusReg;
	statusReg.m_value = dev_w25qxx_read_status_reg2();
	if(statusReg.m_region.m_qe == 0)
	{
//等待就绪
		if(dev_w25qxx_wait_busy(W25QXX_WAIT_READY_TIME) == false)
		{
			return;
		}
//写使能
		dev_w25qxx_enable_write();
//使能4线操作
		statusReg.m_region.m_qe = 1;
		dev_w25qxx_write_status_reg2(statusReg.m_value);
	}
}

/*
功能：W25Qxx读状态寄存器1
参数：无
返回值：@regValue：状态寄存器1的值
*/
static uint8_t dev_w25qxx_read_status_reg1(void)
{
	uint8_t regValue;
//进入直接模式
	hal_qspi_direct_mode_enable();
//发送命令
	hal_qspi_communication_mode_write_data(W25QXX_CMD_READ_SR1);
//读取状态寄存器
	regValue = hal_qspi_communication_mode_read_data();
//退出直接模式
	hal_qspi_direct_mode_disable();
	return regValue;
}

/*
功能：W25Qxx读状态寄存器2
参数：无
返回值：@regValue：状态寄存器2的值
*/
static uint8_t dev_w25qxx_read_status_reg2(void)
{
	uint8_t regValue;
//进入直接模式
	hal_qspi_direct_mode_enable();
//发送命令
	hal_qspi_communication_mode_write_data(W25QXX_CMD_READ_SR2);
//读取状态寄存器
	regValue = hal_qspi_communication_mode_read_data();
//退出直接模式
	hal_qspi_direct_mode_disable();
	return regValue;
}

/*
功能：W25Qxx写状态寄存器1
参数：@value：写入状态寄存器1的值
返回值：无
*/
static void dev_w25qxx_write_status_reg1(uint8_t value)
{
//进入直接模式
	hal_qspi_direct_mode_enable();
//发送命令
	hal_qspi_communication_mode_write_data(W25QXX_CMD_WRITE_SR1);
//写状态寄存器
	hal_qspi_communication_mode_write_data(value);
//退出直接模式
	hal_qspi_direct_mode_disable();
}

/*
功能：W25Qxx写状态寄存器2
参数：@value：写入状态寄存器2的值
返回值：无
*/
static void dev_w25qxx_write_status_reg2(uint8_t value)
{
//进入直接模式
	hal_qspi_direct_mode_enable();
//发送命令
	hal_qspi_communication_mode_write_data(W25QXX_CMD_WRITE_SR2);
//写状态寄存器
	hal_qspi_communication_mode_write_data(value);
//退出直接模式
	hal_qspi_direct_mode_disable();
}

/*
功能：W25Qxx写命令
参数：@cmd：写入的命令
	  @param：命令参数
      @paramLen：参数长度
返回值：无
*/
static void dev_w25qxx_write_cmd(uint8_t cmd, const uint8_t *param, uint8_t paramLen)
{
//进入直接模式
	hal_qspi_direct_mode_enable();
//发送命令
	hal_qspi_communication_mode_write_data(cmd);
//发送参数
	if(param != NULL)
	{
		while(paramLen > 0)
		{
			hal_qspi_communication_mode_write_data(*param);
			++param;
			--paramLen;
		}
	}
//退出直接模式
	hal_qspi_direct_mode_disable();
}

/*
功能：W25Qxx写使能
参数：无
返回值：无
*/
static void dev_w25qxx_enable_write(void)
{
	dev_w25qxx_write_cmd(W25QXX_CMD_ENABLE_WRITE, NULL, 0);
}

/*
功能：W25Qxx写失能
参数：无
返回值：无
*/
static void dev_w25qxx_disable_write(void)
{
	dev_w25qxx_write_cmd(W25QXX_CMD_DISABLE_WRITE, NULL, 0);
}

/*
功能：W25Qxx使能4byte模式
参数：无
返回值：无
*/
static void dev_w25qxx_enable_4biyte(void)
{
	hal_qspi_set_addrwidth(QSPI_ADDR_WIDTH_4BYTE);
	dev_w25qxx_write_cmd(W25QXX_CMD_ENTER_4BYTE, NULL, 0);
}

/*
功能：W25Qxx失能4byte模式
参数：无
返回值：无
*/
static void dev_w25qxx_disable_4biyte(void)
{
	hal_qspi_set_addrwidth(QSPI_ADDR_WIDTH_3BYTE);
	dev_w25qxx_write_cmd(W25QXX_CMD_EXIT_4BYTE, NULL, 0);
}
