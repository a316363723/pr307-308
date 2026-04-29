#include "dev_lumenradio_drive.h"
/********************************************************************************************
 *   设备有效判断宏
 ********************************************************************************************/
#define LUMENRADIO_DEVICE_AVAILABLE(device)  (((device)                != NULL) && \
                                              ((device)->m_delayus     != NULL) && \
                                              ((device)->m_writeNssPin != NULL) && \
                                              ((device)->m_readIrqPin  != NULL) && \
                                              ((device)->m_spiTxRxByte != NULL))

/********************************************************************************************
 *   函数声明
 ********************************************************************************************/
static LumenRadioRegInfo_t lumenradio_get_register_info(uint8_t regAddr);
static bool lumenradio_execute_command(LumenRadioDevice_t *device, uint8_t command, const uint8_t *txData, uint8_t *rxData, uint16_t dataNum, bool bigEnd);

/********************************************************************************************
 *   变量定义
 ********************************************************************************************/
static const LumenRadioRegInfo_t LumenRadioRegInfo[] = {
	{LUMENRADIO_REG_CONFIG,             sizeof(LumenRadioConfigReg_t),           true},
	{LUMENRADIO_REG_STATUS,             sizeof(LumenRadioStatusReg_t),           true},
	{LUMENRADIO_REG_IRQ_MASK,           sizeof(LumenRadioIrqMaskReg_t),          true},
	{LUMENRADIO_REG_IRQ_FLAGS,          sizeof(LumenRadioIrqFlagsReg_t),         true},
	{LUMENRADIO_REG_DMX_WINDOW,         sizeof(LumenRadioDmxWindowReg_t),        true},
	{LUMENRADIO_REG_ASC_FRAME,          sizeof(LumenRadioAscFrameReg_t),         true},
	{LUMENRADIO_REG_LINK_QUALITY,       sizeof(LumenRadioLinkQualityReg_t),      true},
	{LUMENRADIO_REG_DMX_SPEC,           sizeof(LumenRadioDmxSpecReg_t),          true},
	{LUMENRADIO_REG_DMX_CONTROL,        sizeof(LumenRadioDmxControlReg_t),       true},
	{LUMENRADIO_REG_EXTENDED_IRQ_MASK,  sizeof(LumenRadioExtendedIrqMaskReg_t),  true},
	{LUMENRADIO_REG_EXTENDED_IRQ_FLAGS, sizeof(LumenRadioExtendedIrqFlagsReg_t), true},
	{LUMENRADIO_REG_VERSION,            sizeof(LumenRadioVersionReg_t),          true},
	{LUMENRADIO_REG_RF_POWER,           sizeof(LumenRadioRfPowerReg_t),          true},
	{LUMENRADIO_REG_BLOCKED_CHANNELS,   sizeof(LumenRadioBlockedChannelsReg_t),  false},
	{LUMENRADIO_REG_BINDING_UID,        sizeof(LumenRadioBindingUidReg_t),       false},
	{LUMENRADIO_REG_LINKING_KEY,        sizeof(LumenRadioLinkingKeyReg_t),       true},
	{LUMENRADIO_REG_BLE_STATUS,         sizeof(LumenRadioBleStatusReg_t),        true},
	{LUMENRADIO_REG_BLE_PIN,            sizeof(LumenRadioBlePinReg_t),           false},
	{LUMENRADIO_REG_BATTERY,            sizeof(LumenRadioBatteryReg_t),          true},
	{LUMENRADIO_REG_UNIVERSE_COLOR,     sizeof(LumenRadioUniverseColorReg_t),    true},
	{LUMENRADIO_REG_OEM_INFO,           sizeof(LumenRadioOemInfoReg_t),          true},
	{LUMENRADIO_REG_RXTX_STATUS,        sizeof(LumenRadioRxTxStatusReg_t),       true},
	{LUMENRADIO_REG_DEVICE_NAME,        sizeof(LumenRadioDeviceNameReg_t),       false},
	{LUMENRADIO_REG_UNIVERSE_NAME,      sizeof(LumenRadioUniverseNameReg_t),     false},
};

/********************************************************************************************
 *   函数定义
 ********************************************************************************************/
/**
 *@brief  挂起SPI中断
 *@param  device：LumenRadio设备
 *@return 是否挂起成功
 */
bool lumenradio_pend_spi_irq(LumenRadioDevice_t *device)
{
//设备检查
	lumenradio_assert(LUMENRADIO_DEVICE_AVAILABLE(device));
//挂起SPI中断
	if(device->m_spiIrqState == LUMENRADIO_SPI_IRQ_STATE_WAIT)
	{
		device->m_spiIrqState = LUMENRADIO_SPI_IRQ_STATE_PEND;
		return true;
	}
	return false;
}

/**
 *@brief  读寄存器
 *@param  device  ：LumenRadio设备
 *@param  regAddr ：寄存器地址
 *@param  regValue：保存寄存器值
 *@return 是否成功
 */
bool lumenradio_read_register(LumenRadioDevice_t *device, uint8_t regAddr, void *regValue)
{
	LumenRadioRegInfo_t regInfo;
//获取寄存器信息
	regInfo = lumenradio_get_register_info(regAddr);
	if(regInfo.m_regSize == 0) return false;
//执行读寄存器命令
	return lumenradio_execute_command(device, LUMENRADIO_CMD_READ_REG(regAddr), NULL, regValue, regInfo.m_regSize, regInfo.m_bigEnd);
}

/**
 *@brief  写寄存器
 *@param  device  ：LumenRadio设备
 *@param  regAddr ：寄存器地址
 *@param  regValue：写入寄存器值
 *@return 是否成功
 */
bool lumenradio_write_register(LumenRadioDevice_t *device, uint8_t regAddr, const void *regValue)
{
	LumenRadioRegInfo_t regInfo;
//参数检查
	if(regValue == NULL) return false;
//获取寄存器信息
	regInfo = lumenradio_get_register_info(regAddr);
	if(regInfo.m_regSize == 0) return false;
//执行写寄存器命令
	return lumenradio_execute_command(device, LUMENRADIO_CMD_WRITE_REG(regAddr), regValue, NULL, regInfo.m_regSize, regInfo.m_bigEnd);
}

/**
 *@brief  读DMX数据
 *@param  device ：LumenRadio设备
 *@param  dmxData：保存DMX数据
 *@param  dataNum：数据数量
 *@return 是否成功
 */
bool lumenradio_read_dmx_data(LumenRadioDevice_t *device, uint8_t *dmxData, uint8_t dataNum)
{
//参数检查
	if(dataNum == 0) return true;
	if(dataNum > LUMENRADIO_SPI_READ_DMX_MAX) return false;
//执行读DMX数据命令
	return lumenradio_execute_command(device, LUMENRADIO_CMD_READ_DMX, NULL, dmxData, dataNum, false);
}

/**
 *@brief  读ASC数据
 *@param  device ：LumenRadio设备
 *@param  ascData：保存ASC数据
 *@param  dataNum：数据数量
 *@return 是否成功
 */
bool lumenradio_read_asc_data(LumenRadioDevice_t *device, uint8_t *ascData, uint8_t dataNum)
{
//参数检查
	if(dataNum == 0) return true;
	if(dataNum > LUMENRADIO_SPI_READ_ASC_MAX) return false;
//执行读ASC数据命令
	return lumenradio_execute_command(device, LUMENRADIO_CMD_READ_ASC, NULL, ascData, dataNum, false);
}

/**
 *@brief  写DMX数据
 *@param  device ：LumenRadio设备
 *@param  dmxData：写入DMX数据
 *@param  dataNum：数据数量
 *@return 是否成功
 */
bool lumenradio_write_dmx_data(LumenRadioDevice_t *device, const uint8_t *dmxData, uint8_t dataNum)
{
//参数检查
	if(dataNum == 0) return true;
	if((dmxData == NULL) || (dataNum > LUMENRADIO_SPI_WRITE_DMX_MAX)) return false;
//执行写DMX数据命令
	return lumenradio_execute_command(device, LUMENRADIO_CMD_WRITE_DMX, dmxData, NULL, dataNum, false);
}

/**
 *@brief  空操作，可用于读Irq Flags寄存器
 *@param  device     ：LumenRadio设备
 *@param  irqFlagsReg：保存Irq Flags寄存器值
 *@return 是否成功
 */
bool lumenradio_nop(LumenRadioDevice_t *device, LumenRadioIrqFlagsReg_t *irqFlagsReg)
{
	return lumenradio_execute_command(device, LUMENRADIO_CMD_NOP, NULL, (uint8_t *)irqFlagsReg, 0, false);
}

/********************************************************************************************
 *   工具函数定义
 ********************************************************************************************/
/**
 *@brief  获取寄存器信息
 *@param  regAddr：寄存器地址
 *@return 寄存器信息
 */
static LumenRadioRegInfo_t lumenradio_get_register_info(uint8_t regAddr)
{
	uint8_t i;
	LumenRadioRegInfo_t regInfo = {.m_regSize = 0};
	for(i = 0; i < (sizeof(LumenRadioRegInfo) / sizeof(LumenRadioRegInfo[0])); ++i)
	{
		if(LumenRadioRegInfo[i].m_regAddr == regAddr)
		{
			regInfo = LumenRadioRegInfo[i];
			break;
		}
	}
	return regInfo;
}

/**
 *@brief  执行命令
 *@param  device ：LumenRadio设备
 *@param  command：要执行的命令
 *@param  txData ：要发送的数据
 *@param  rxData ：保存接收的数据
 *@param  dataNum：荷载数据数量
 *@param  bigEnd ：荷载端模式
 *@return 是否执行成功
 */
static bool lumenradio_execute_command(LumenRadioDevice_t *device, uint8_t command, const uint8_t *txData, uint8_t *rxData, uint16_t dataNum, bool bigEnd)
{
	uint32_t waitTick;
	int8_t txInc, rxInc;
	uint8_t txByte, rxByte;
	bool exeResult = true;
//设备检查
	lumenradio_assert(LUMENRADIO_DEVICE_AVAILABLE(device));
//执行通信第一阶段（写入命令）
	device->m_writeNssPin(device->m_deviceId, LUMENRADIO_PIN_STATE_LOW);
	device->m_delayus(device->m_deviceId, LUMENRADIO_SPI_NSS_SETUP_TIME);
	rxByte = device->m_spiTxRxByte(device->m_deviceId, command);
	if(command == LUMENRADIO_CMD_NOP)
	{
		device->m_writeNssPin(device->m_deviceId, LUMENRADIO_PIN_STATE_HIGH);
		if(rxData != NULL) *rxData = rxByte;
		return true;
	}
	if(((LumenRadioIrqFlagsReg_t *)(&rxByte))->m_spiDeviceBusy != 0)
	{
		device->m_writeNssPin(device->m_deviceId, LUMENRADIO_PIN_STATE_HIGH);
		return false;
	}
	device->m_spiIrqState = LUMENRADIO_SPI_IRQ_STATE_WAIT;
	device->m_writeNssPin(device->m_deviceId, LUMENRADIO_PIN_STATE_HIGH);
//等待LumenRadio应答
	waitTick = LUMENRADIO_SPI_WAIT_IRQ_PEND_TIME;
	while(device->m_spiIrqState != LUMENRADIO_SPI_IRQ_STATE_PEND)
	{
		if(waitTick == 0) {exeResult = false; break;}
		device->m_delayus(device->m_deviceId, 1);
		--waitTick;
	}
	device->m_spiIrqState = LUMENRADIO_SPI_IRQ_STATE_IDLE;
	if(exeResult == false) return false;
//执行通信第二阶段（处理命令）
	device->m_writeNssPin(device->m_deviceId, LUMENRADIO_PIN_STATE_LOW);
	device->m_delayus(device->m_deviceId, LUMENRADIO_SPI_NSS_SETUP_TIME);
	rxByte = device->m_spiTxRxByte(device->m_deviceId, 0xFF);
	if(((LumenRadioIrqFlagsReg_t *)(&rxByte))->m_spiDeviceBusy != 0)
	{
		exeResult = false;
	}
	else if(dataNum > 0)
	{
		txByte = 0xFF;
		txInc  = (txData == NULL) ? 0 : (bigEnd ? -1 : 1);
		rxInc  = (rxData == NULL) ? 0 : (bigEnd ? -1 : 1);
		txData = (txData == NULL) ? (&txByte) : (bigEnd ? (txData + dataNum - 1) : txData);
		rxData = (rxData == NULL) ? (&rxByte) : (bigEnd ? (rxData + dataNum - 1) : rxData);
		while(dataNum > 0)
		{
			*rxData = device->m_spiTxRxByte(device->m_deviceId, *txData);
			txData += txInc;
			rxData += rxInc;
			--dataNum;
		}
	}
	device->m_writeNssPin(device->m_deviceId, LUMENRADIO_PIN_STATE_HIGH);
//等待LumenRadio拉高IRQ
	waitTick = LUMENRADIO_SPI_WAIT_IRQ_PLUP_TIME;
	while(device->m_readIrqPin(device->m_deviceId) == LUMENRADIO_PIN_STATE_LOW)
	{
		if(waitTick == 0) break;
		device->m_delayus(device->m_deviceId, 1);
		--waitTick;
	}
	return exeResult;
}
