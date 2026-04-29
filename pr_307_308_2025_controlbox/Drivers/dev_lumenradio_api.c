#include "dev_lumenradio_api.h"
/********************************************************************************************
 *   通用功能函数定义
 ********************************************************************************************/
/**
 *@brief  SPI中断处理
 *@param  device：LumenRadio设备
 *@return 是否处理成功
 */
bool lumenradio_spi_irq_handler(LumenRadioDevice_t *device)
{
	return lumenradio_pend_spi_irq(device);
}

/**
 *@brief  串口输出使/失能
 *@param  device：LumenRadio设备
 *@param  enable：串口输出使/失能控制
 *@return 是否控制成功
 */
bool lumenradio_uart_output_enable(LumenRadioDevice_t *device, bool enable)
{
	uint8_t uartEn;
	LumenRadioConfigReg_t configReg;
//读取Config寄存器
	if(lumenradio_read_register(device, LUMENRADIO_REG_CONFIG, &configReg) == false)
	{
		return false;
	}
//串口输出使/失能控制
	uartEn = (enable == false) ? 0 : 1;
	if(configReg.m_uartEn != uartEn)
	{
		configReg.m_uartEn = uartEn;
		if(lumenradio_write_register(device, LUMENRADIO_REG_CONFIG, &configReg) == false)
		{
			return false;
		}
	}
	return true;
}

/**
 *@brief  中断使/失能
 *@param  device：LumenRadio设备
 *@param  irqs  ：中断，取值为LUMENRADIO_IRQ_XXX组合
 *@param  enable：中断使/失能控制
 *@return 是否控制成功
 */
bool lumenradio_irq_enable(LumenRadioDevice_t *device, uint32_t irqs, bool enable)
{
	uint8_t irqEnable = (enable == false) ? 0 : 1;
//通用中断使/失能控制
	if(irqs & (LUMENRADIO_IRQ_RX_DMX | LUMENRADIO_IRQ_LOST_DMX | LUMENRADIO_IRQ_DMX_CHANGED
	| LUMENRADIO_IRQ_RF_LINK | LUMENRADIO_IRQ_ASC | LUMENRADIO_IRQ_IDENTIFY | LUMENRADIO_IRQ_EXTENDED))
	{
		LumenRadioIrqMaskReg_t irqMaskReg;
		if(lumenradio_read_register(device, LUMENRADIO_REG_IRQ_MASK, &irqMaskReg) == false)
		{
			return false;
		}
		if(irqs & LUMENRADIO_IRQ_RX_DMX)      irqMaskReg.m_rxDmxIrqEn      = irqEnable;
		if(irqs & LUMENRADIO_IRQ_LOST_DMX)    irqMaskReg.m_lostDmxIrqEn    = irqEnable;
		if(irqs & LUMENRADIO_IRQ_DMX_CHANGED) irqMaskReg.m_dmxChangedIrqEn = irqEnable;
		if(irqs & LUMENRADIO_IRQ_RF_LINK)     irqMaskReg.m_rfLinkIrqEn     = irqEnable;
		if(irqs & LUMENRADIO_IRQ_ASC)         irqMaskReg.m_ascIrqEn        = irqEnable;
		if(irqs & LUMENRADIO_IRQ_IDENTIFY)    irqMaskReg.m_identifyIrqEn   = irqEnable;
		if(irqs & LUMENRADIO_IRQ_EXTENDED)    irqMaskReg.m_extendedIrqEn   = irqEnable;
		if(lumenradio_write_register(device, LUMENRADIO_REG_IRQ_MASK, &irqMaskReg) == false)
		{
			return false;
		}
	}
//扩展中断使/失能控制
	if(irqs & (LUMENRADIO_IRQ_RDM_REQUEST | LUMENRADIO_IRQ_RXTX_DA | LUMENRADIO_IRQ_RXTX_CTS
	| LUMENRADIO_IRQ_UNIV_META_CHANGED))
	{
		LumenRadioExtendedIrqMaskReg_t exIrqMaskReg;
		if(lumenradio_read_register(device, LUMENRADIO_REG_EXTENDED_IRQ_MASK, &exIrqMaskReg) == false)
		{
			return false;
		}
		if(irqs & LUMENRADIO_IRQ_RDM_REQUEST)       exIrqMaskReg.m_rdmRequestEn      = irqEnable;
		if(irqs & LUMENRADIO_IRQ_RXTX_DA)           exIrqMaskReg.m_rxTxDaEn          = irqEnable;
		if(irqs & LUMENRADIO_IRQ_RXTX_CTS)          exIrqMaskReg.m_rxTxCtsEn         = irqEnable;
		if(irqs & LUMENRADIO_IRQ_UNIV_META_CHANGED) exIrqMaskReg.m_univMetaChangedEn = irqEnable;
		if(lumenradio_write_register(device, LUMENRADIO_REG_EXTENDED_IRQ_MASK, &exIrqMaskReg) == false)
		{
			return false;
		}
	}
	return true;
}

/**
 *@biref  获取标志状态
 *@param  device  ：LumenRadio设备
 *@param  flags   ：状态标志，取值为LUMENRADIO_FLAG_XXX组合
 *@param  relation：标志关系
 *@param  status  ：标志状态
 *@return 是否获取成功
 */
bool lumenradio_get_status(LumenRadioDevice_t *device, uint32_t flags, LumenRadioFlagsRelation_t relation, bool *status)
{
	uint32_t flagsMask;
	uint32_t flagsFilter;
	bool getResult = true;
	bool flagsValid = false;
//参数检查
	if(status == NULL)
	{
		return false;
	}
//获取通用标志状态
	flagsFilter = flags & (LUMENRADIO_FLAG_LINKED | LUMENRADIO_FLAG_RF_LINK | LUMENRADIO_FLAG_IDENTIFY
	| LUMENRADIO_FLAG_DMX | LUMENRADIO_FLAG_UPDATE_MODE);
	if(flagsFilter != 0x00000000)
	{
		flagsValid = true;
		LumenRadioStatusReg_t statusReg;
		if(lumenradio_read_register(device, LUMENRADIO_REG_STATUS, &statusReg))
		{
			flagsMask = 0x00000000;
			if(statusReg.m_linked)     flagsMask |= LUMENRADIO_FLAG_LINKED;
			if(statusReg.m_rfLink)     flagsMask |= LUMENRADIO_FLAG_RF_LINK;
			if(statusReg.m_identify)   flagsMask |= LUMENRADIO_FLAG_IDENTIFY;
			if(statusReg.m_dmx)        flagsMask |= LUMENRADIO_FLAG_DMX;
			if(statusReg.m_updateMode) flagsMask |= LUMENRADIO_FLAG_UPDATE_MODE;
			if((relation == LUMENRADIO_FLAGS_RELA_OR) && ((flagsMask & flagsFilter) != 0x00000000))
			{
				*status = true;
				return true;
			}
			if((relation == LUMENRADIO_FLAGS_RELA_AND) && ((flagsMask & flagsFilter) != flagsFilter))
			{
				*status = false;
				return true;
			}
		}
		else if(relation == LUMENRADIO_FLAGS_RELA_OR)
		{
			getResult = false;
		}
		else
		{
			return false;
		}
	}
//获取蓝牙PIN标志状态
	flagsFilter = flags & LUMENRADIO_FLAG_BLE_PIN;
	if(flagsFilter != 0x00000000)
	{
		flagsValid = true;
		LumenRadioBleStatusReg_t bleStatusReg;
		if(lumenradio_read_register(device, LUMENRADIO_REG_BLE_STATUS, &bleStatusReg))
		{
			flagsMask = 0x00000000;
			if(bleStatusReg.m_blePin) flagsMask |= LUMENRADIO_FLAG_BLE_PIN;
			if((relation == LUMENRADIO_FLAGS_RELA_OR) && ((flagsMask & flagsFilter) != 0x00000000))
			{
				*status = true;
				return true;
			}
			if((relation == LUMENRADIO_FLAGS_RELA_AND) && ((flagsMask & flagsFilter) != flagsFilter))
			{
				*status = false;
				return true;
			}
		}
		else if(relation == LUMENRADIO_FLAGS_RELA_OR)
		{
			getResult = false;
		}
		else
		{
			return false;
		}
	}
//获取蓝牙通信标志状态
	flagsFilter = flags & (LUMENRADIO_FLAG_DATA_AVAILABLE | LUMENRADIO_FLAG_CLEAR_TO_SEND);
	if(flagsFilter != 0x00000000)
	{
		flagsValid = true;
		LumenRadioRxTxStatusReg_t rxTxStatusReg;
		if(lumenradio_read_register(device, LUMENRADIO_REG_RXTX_STATUS, &rxTxStatusReg))
		{
			flagsMask = 0x00000000;
			if(rxTxStatusReg.m_dataAvailable) flagsMask |= LUMENRADIO_FLAG_DATA_AVAILABLE;
			if(rxTxStatusReg.m_clearToSend)   flagsMask |= LUMENRADIO_FLAG_CLEAR_TO_SEND;
			if((relation == LUMENRADIO_FLAGS_RELA_OR) && ((flagsMask & flagsFilter) != 0x00000000))
			{
				*status = true;
				return true;
			}
			if((relation == LUMENRADIO_FLAGS_RELA_AND) && ((flagsMask & flagsFilter) != flagsFilter))
			{
				*status = false;
				return true;
			}
		}
		else if(relation == LUMENRADIO_FLAGS_RELA_OR)
		{
			getResult = false;
		}
		else
		{
			return false;
		}
	}
//获取通用中断标志状态
	flagsFilter = flags & (LUMENRADIO_FLAG_RX_DMX_IRQ | LUMENRADIO_FLAG_LOST_DMX_IRQ | LUMENRADIO_FLAG_DMX_CHANGED_IRQ
	| LUMENRADIO_FLAG_RF_LINK_IRQ | LUMENRADIO_FLAG_ASC_IRQ | LUMENRADIO_FLAG_IDENTIFY_IRQ | LUMENRADIO_FLAG_EXTENDED_IRQ);
	if(flagsFilter != 0x00000000)
	{
		flagsValid = true;
		LumenRadioIrqFlagsReg_t irqFlagsReg;
		if(lumenradio_nop(device, &irqFlagsReg))
		{
			flagsMask = 0x00000000;
			if(irqFlagsReg.m_rxDmxIrq)      flagsMask |= LUMENRADIO_FLAG_RX_DMX_IRQ;
			if(irqFlagsReg.m_lostDmxIrq)    flagsMask |= LUMENRADIO_FLAG_LOST_DMX_IRQ;
			if(irqFlagsReg.m_dmxChangedIrq) flagsMask |= LUMENRADIO_FLAG_DMX_CHANGED_IRQ;
			if(irqFlagsReg.m_rfLinkIrq)     flagsMask |= LUMENRADIO_FLAG_RF_LINK_IRQ;
			if(irqFlagsReg.m_ascIrq)        flagsMask |= LUMENRADIO_FLAG_ASC_IRQ;
			if(irqFlagsReg.m_identifyIrq)   flagsMask |= LUMENRADIO_FLAG_IDENTIFY_IRQ;
			if(irqFlagsReg.m_extendedIrq)   flagsMask |= LUMENRADIO_FLAG_EXTENDED_IRQ;
			if((relation == LUMENRADIO_FLAGS_RELA_OR) && ((flagsMask & flagsFilter) != 0x00000000))
			{
				*status = true;
				return true;
			}
			if((relation == LUMENRADIO_FLAGS_RELA_AND) && ((flagsMask & flagsFilter) != flagsFilter))
			{
				*status = false;
				return true;
			}
		}
		else if(relation == LUMENRADIO_FLAGS_RELA_OR)
		{
			getResult = false;
		}
		else
		{
			return false;
		}
	}
//获取扩展中断标志状态
	flagsFilter = flags & (LUMENRADIO_FLAG_RDM_REQUEST_IRQ | LUMENRADIO_FLAG_RXTX_DA_IRQ | LUMENRADIO_FLAG_RXTX_CTS_IRQ
	| LUMENRADIO_FLAG_UNIV_META_CHANGED_IRQ);
	if(flagsFilter != 0x00000000)
	{
		flagsValid = true;
		LumenRadioExtendedIrqFlagsReg_t exIrqFlagsReg;
		if(lumenradio_read_register(device, LUMENRADIO_REG_EXTENDED_IRQ_FLAGS, &exIrqFlagsReg))
		{
			flagsMask = 0x00000000;
			if(exIrqFlagsReg.m_rdmRequest)      flagsMask |= LUMENRADIO_FLAG_RDM_REQUEST_IRQ;
			if(exIrqFlagsReg.m_rxTxDa)          flagsMask |= LUMENRADIO_FLAG_RXTX_DA_IRQ;
			if(exIrqFlagsReg.m_rxTxCts)         flagsMask |= LUMENRADIO_FLAG_RXTX_CTS_IRQ;
			if(exIrqFlagsReg.m_univMetaChanged) flagsMask |= LUMENRADIO_FLAG_UNIV_META_CHANGED_IRQ;
			if((relation == LUMENRADIO_FLAGS_RELA_OR) && ((flagsMask & flagsFilter) != 0x00000000))
			{
				*status = true;
				return true;
			}
			if((relation == LUMENRADIO_FLAGS_RELA_AND) && ((flagsMask & flagsFilter) != flagsFilter))
			{
				*status = false;
				return true;
			}
		}
		else if(relation == LUMENRADIO_FLAGS_RELA_OR)
		{
			getResult = false;
		}
		else
		{
			return false;
		}
	}
//获取结果处理
	if((flagsValid == false) || (getResult == false)) return false;
	*status = (relation == LUMENRADIO_FLAGS_RELA_OR) ? false : true;
	return true;
}

/**
 *@brief  获取版本信息
 *@param  device   ：LumenRadio设备
 *@param  hwVersion：硬件版本号
 *@param  swVersion：软件版本号
 *@return 是否获取成功
 */
bool lumenradio_get_version(LumenRadioDevice_t *device, uint32_t *hwVersion, uint32_t *swVersion)
{
	LumenRadioVersionReg_t versionReg;
//参数检查
	if((hwVersion == NULL) && (swVersion == NULL))
	{
		return false;
	}
//获取版本信息
	if(lumenradio_read_register(device, LUMENRADIO_REG_VERSION, &versionReg) == false)
	{
		return false;
	}
	if(hwVersion != NULL) *hwVersion = versionReg.m_hwVersion;
	if(swVersion != NULL) *swVersion = versionReg.m_driverVersion;
	return true;
}

/********************************************************************************************
 *   无线功能函数定义
 ********************************************************************************************/
/**
 *@brief  无线操作使/失能
 *@param  device：LumenRadio设备
 *@param  enable：无线操作使/失能控制
 *@return 是否控制成功
 */
bool lumenradio_wireless_operation_enable(LumenRadioDevice_t *device, bool enable)
{
	uint8_t radioEnable;
	LumenRadioConfigReg_t configReg;
//读取Config寄存器
	if(lumenradio_read_register(device, LUMENRADIO_REG_CONFIG, &configReg) == false)
	{
		return false;
	}
//无线操作使/失能控制
	radioEnable = (enable == false) ? 0 : 1;
	if(configReg.m_radioEnable != radioEnable)
	{
		configReg.m_radioEnable = radioEnable;
		if(lumenradio_write_register(device, LUMENRADIO_REG_CONFIG, &configReg) == false)
		{
			return false;
		}
	}
	return true;
}

/**
 *@brief  射频通道使/失能
 *@param  device    ：LumenRadio设备
 *@param  blockedTab：射频通道阻塞列表
 *@param  blockedNum：射频通道阻塞数量
 *@return 是否控制成功
 */
bool lumenradio_rf_channels_enable(LumenRadioDevice_t *device, const LumenRadioRfBlocked_t *blockedTab, uint8_t blockedNum)
{
	bool cfgAvailable;
	uint8_t activeNum;
	uint8_t rfChannel;
	LumenRadioRfBlocked_t rfBlocked;
	LumenRadioBlockedChannelsReg_t blockedChannelsReg;
//参数检查
	if(blockedNum == 0) return true;
	if(blockedTab == NULL) return false;
//读取Blocked Channels寄存器
	if(lumenradio_read_register(device, LUMENRADIO_REG_BLOCKED_CHANNELS, &blockedChannelsReg) == false)
	{
		return false;
	}
//处理Blocked Channels寄存器配置值
	cfgAvailable = false;
	activeNum = UINT8_MAX;
	for(; blockedNum > 0; ++blockedTab, --blockedNum)
	{
		rfBlocked = *blockedTab;
		if(rfBlocked.m_startChannel > rfBlocked.m_endChannel)
		{
			rfChannel = rfBlocked.m_startChannel;
			rfBlocked.m_startChannel = rfBlocked.m_endChannel;
			rfBlocked.m_endChannel = rfChannel;
		}
		if((rfBlocked.m_startChannel <= LUMENRADIO_RF_CHANNEL_END_NUMBER)
		&& (rfBlocked.m_endChannel >= LUMENRADIO_RF_CHANNEL_START_NUMBER))
		{
			cfgAvailable = true;
			if(rfBlocked.m_channelEnable == false)
			{
				activeNum = 0;
			}
			if(rfBlocked.m_startChannel < LUMENRADIO_RF_CHANNEL_START_NUMBER)
			{
				rfBlocked.m_startChannel = LUMENRADIO_RF_CHANNEL_START_NUMBER;
			}
			if(rfBlocked.m_endChannel > LUMENRADIO_RF_CHANNEL_END_NUMBER)
			{
				rfBlocked.m_endChannel = LUMENRADIO_RF_CHANNEL_END_NUMBER;
			}
			for(rfChannel = rfBlocked.m_startChannel; rfChannel <= rfBlocked.m_endChannel; ++rfChannel)
			{
				if(rfBlocked.m_channelEnable == false)
				{
					blockedChannelsReg.m_flags[rfChannel / 8] |= 0x01 << (rfChannel % 8);
				}
				else
				{
					blockedChannelsReg.m_flags[rfChannel / 8] &= ~(0x01 << (rfChannel % 8));
				}
			}
		}
	}
	if(cfgAvailable == false)
	{
		return false;
	}
//可用射频通道数量检测
	if(activeNum == 0)
	{
		for(rfChannel = LUMENRADIO_RF_CHANNEL_START_NUMBER; rfChannel <= LUMENRADIO_RF_CHANNEL_END_NUMBER; ++rfChannel)
		{
			if((blockedChannelsReg.m_flags[rfChannel / 8] & (0x01 << (rfChannel % 8))) == 0x00)
			{
				++activeNum;
			}
		}
		if(activeNum < LUMENRADIO_RF_CHANNEL_ACTIVE_LEAST)
		{
			return false;
		}
	}
//使/失能射频通道控制
	if(lumenradio_write_register(device, LUMENRADIO_REG_BLOCKED_CHANNELS, &blockedChannelsReg) == false)
	{
		return false;
	}
	return true;
}

/**
 *@brief  设置LumenRadio工作模式
 *@param  device  ：LumenRadio设备
 *@param  workMode：设备工作模式
 *@return 是否设置成功
 */
bool lumenradio_set_work_mode(LumenRadioDevice_t *device, LumenRadioWorkMode_t workMode)
{
	LumenRadioConfigReg_t configReg;
//读取Config寄存器
	if(lumenradio_read_register(device, LUMENRADIO_REG_CONFIG, &configReg) == false)
	{
		return false;
	}
//设置设备工作模式
	if(configReg.m_radioTxRxMode != workMode)
	{
		configReg.m_radioTxRxMode = workMode;
		if(lumenradio_write_register(device, LUMENRADIO_REG_CONFIG, &configReg) == false)
		{
			return false;
		}
	}
	return true;
}

/**
 *@brief  设置连接秘钥
 *@param  device  ：LumenRadio设备
 *@param  linkCode：连接码
 *@param  tranMode：发送器模式
 *@param  universe：Universe编号
 *@return 是否设置成功
 */
bool lumenradio_set_linking_key(LumenRadioDevice_t *device, uint64_t linkCode, LumenRadioTransmitterMode_t tranMode, uint8_t universe)
{
	LumenRadioLinkingKeyReg_t linkingKeyReg;
	linkingKeyReg.m_code     = linkCode;
	linkingKeyReg.m_mode     = tranMode;
	linkingKeyReg.m_universe = universe;
	if(lumenradio_write_register(device, LUMENRADIO_REG_LINKING_KEY, &linkingKeyReg) == false)
	{
		return false;
	}
	return true;
}

/**
 *@brief  设置射频输出功率
 *@param  device ：LumenRadio设备
 *@param  rfPower：射频输出功率
 *@return 是否设置成功
 */
bool lumenradio_set_rf_output_power(LumenRadioDevice_t *device, LumenRadioRfPower_t rfPower)
{
	LumenRadioRfPowerReg_t rfPowerReg;
	rfPowerReg.m_outputPower = rfPower;
	if(lumenradio_write_register(device, LUMENRADIO_REG_RF_POWER, &rfPowerReg) == false)
	{
		return false;
	}
	return true;
}

/**
 *@brief  启动无线连接
 *@param  device：LumenRadio设备
 *@return 是否启动成功
 */
bool lumenradio_start_linking(LumenRadioDevice_t *device)
{
	LumenRadioStatusReg_t statusReg = {.m_rfLink = 1};
	if(lumenradio_write_register(device, LUMENRADIO_REG_STATUS, &statusReg) == false)
	{
		return false;
	}
	return true;
}

/**
 *@brief  断开无线连接
 *@param  device：LumenRadio设备
 *@return 是否断开成功
 */
bool lumenradio_unlink(LumenRadioDevice_t *device)
{
	LumenRadioStatusReg_t statusReg = {.m_linked = 1};
	if(lumenradio_write_register(device, LUMENRADIO_REG_STATUS, &statusReg) == false)
	{
		return false;
	}
	return true;
}

/**
 *@brief  获取连接质量
 *@param  device ：LumenRadio设备
 *@param  quality：连接质量
 *@return 是否获取成功
 */
bool lumenradio_get_link_quality(LumenRadioDevice_t *device, uint8_t *quality)
{
	LumenRadioLinkQualityReg_t linkQualityReg;
//参数检查
	if(quality == NULL)
	{
		return false;
	}
//获取连接质量
	if(lumenradio_read_register(device, LUMENRADIO_REG_LINK_QUALITY, &linkQualityReg) == false)
	{
		*quality = 0;
		return false;
	}
	*quality = 100 * linkQualityReg.m_pdr / UINT8_MAX;
	return true;
}

/********************************************************************************************
 *   DMX功能函数定义
 ********************************************************************************************/
/**
 *@brief  内部生成器使/失能
 *@param  device：LumenRadio设备
 *@param  enable：内部生成器使/失能控制
 *@return 是否控制成功
 */
bool lumenradio_internal_generation_enable(LumenRadioDevice_t *device, bool enable)
{
	LumenRadioDmxControlReg_t dmxControlReg = {.m_enable = 0};
	dmxControlReg.m_enable = (enable == false) ? 0 : 1;
	if(lumenradio_write_register(device, LUMENRADIO_REG_DMX_CONTROL, &dmxControlReg) == false)
	{
		return false;
	}
	return true;
}

/**
 *@brief  设置DMX窗口
 *@param  device    ：LumenRadio设备
 *@param  startAddr ：窗口起始地址
 *@param  windowSize：窗口大小
 *@return 是否设置成功
 */
bool lumenradio_set_dmx_window(LumenRadioDevice_t *device, uint16_t startAddr, uint16_t windowSize)
{
	LumenRadioDmxWindowReg_t dmxWindowReg;
//参数检查
	if((startAddr < 1) || (startAddr > 512) || (windowSize > (512 - startAddr + 1)))
	{
		return false;
	}
//设置DMX窗口
	dmxWindowReg.m_startAddress = startAddr - 1;
	dmxWindowReg.m_windowSize   = windowSize;
	if(lumenradio_write_register(device, LUMENRADIO_REG_DMX_WINDOW, &dmxWindowReg) == false)
	{
		return false;
	}
	return true;
}

/**
 *@brief  设置DMX参数
 *@param  device     ：LumenRadio设备
 *@param  slotSpace  ：通道间隔，单位us
 *@param  frameLength：DMX帧时长，单位us
 *@return 是否设置成功
 */
bool lumenradio_set_dmx_param(LumenRadioDevice_t *device, uint16_t slotSpace, uint32_t frameLength)
{
	LumenRadioDmxSpecReg_t dmxSpecReg;
//获取DMX参数
	if(lumenradio_read_register(device, LUMENRADIO_REG_DMX_SPEC, &dmxSpecReg) == false)
	{
		return false;
	}
//设置DMX参数
	if((dmxSpecReg.m_interslotTime != slotSpace) || (dmxSpecReg.m_refreshPeriod != frameLength))
	{
		dmxSpecReg.m_interslotTime = slotSpace;
		dmxSpecReg.m_refreshPeriod = frameLength;
		if(lumenradio_write_register(device, LUMENRADIO_REG_DMX_SPEC, &dmxSpecReg) == false)
		{
			return false;
		}
	}
	return true;
}

/**
 *@brief  读取DMX帧
 *@param  device ：LumenRadio设备
 *@param  dmxData：DMX数据
 *@param  dataNum：数据数量
 *@return 是否读取成功
 */
bool lumenradio_read_dmx_frame(LumenRadioDevice_t *device, uint8_t *dmxData, uint16_t *dataNum)
{
	uint8_t readNum;
	uint16_t readCount;
	LumenRadioDmxWindowReg_t dmxWindowReg;
//参数检查
	if((dmxData == NULL) || (dataNum == NULL))
	{
		return false;
	}
//获取窗口配置，并复位读计数器
	if(lumenradio_read_register(device, LUMENRADIO_REG_DMX_WINDOW, &dmxWindowReg) == false)
	{
		*dataNum = 0;
		return false;
	}
//读取DMX数据
	readCount = 0;
	while(dmxWindowReg.m_windowSize > 0)
	{
		readNum = LUMENRADIO_SPI_READ_DMX_MAX;
		readNum = (readNum > dmxWindowReg.m_windowSize) ? dmxWindowReg.m_windowSize : readNum;
		if(lumenradio_read_dmx_data(device, dmxData, readNum) == false)
		{
			break;
		}
		readCount += readNum;
		dmxData += readNum;
		dmxWindowReg.m_windowSize -= readNum;
	}
	*dataNum = readCount;
	return ((dmxWindowReg.m_windowSize > 0) ? false : true);
}

/**
 *@brief  写入DMX帧
 *@param  device ：LumenRadio设备
 *@param  dmxData：DMX数据
 *@param  dataNum：数据数量
 *@return 是否写入成功
 */
bool lumenradio_write_dmx_frame(LumenRadioDevice_t *device, const uint8_t *dmxData, uint16_t dataNum)
{
	uint8_t writeNum;
	LumenRadioDmxSpecReg_t dmxSpecReg;
//参数检查
	if(((dmxData == NULL) && (dataNum > 0)) || (dataNum > 512))
	{
		return false;
	}
//获取DMX参数，并复位写计数器
	if(lumenradio_read_register(device, LUMENRADIO_REG_DMX_SPEC, &dmxSpecReg) == false)
	{
		return false;
	}
//设置DMX参数
	if(dmxSpecReg.m_nChannels != dataNum)
	{
		dmxSpecReg.m_nChannels = dataNum;
		if(lumenradio_write_register(device, LUMENRADIO_REG_DMX_SPEC, &dmxSpecReg) == false)
		{
			return false;
		}
	}
//写入DMX数据
	while(dataNum > 0)
	{
		writeNum = LUMENRADIO_SPI_WRITE_DMX_MAX;
		writeNum = (writeNum > dataNum) ? dataNum : writeNum;
		if(lumenradio_write_dmx_data(device, dmxData, writeNum) == false)
		{
			break;
		}
		dmxData += writeNum;
		dataNum -= writeNum;
	}
	return ((dataNum > 0) ? false : true);
}

/********************************************************************************************
 *   RDM功能函数定义
 ********************************************************************************************/
/**
 *@brief  SPI RDM使/失能
 *@param  device：LumenRadio设备
 *@param  enable：SPI RDM使/失能控制
 *@return 是否控制成功
 */
bool lumenradio_spi_rdm_enable(LumenRadioDevice_t *device, bool enable)
{
	uint8_t spiRdm;
	LumenRadioConfigReg_t configReg;
//读取Config寄存器
	if(lumenradio_read_register(device, LUMENRADIO_REG_CONFIG, &configReg) == false)
	{
		return false;
	}
//SPI RDM使/失能控制
	spiRdm = (enable == false) ? 0 : 1;
	if(configReg.m_spiRdm != spiRdm)
	{
		configReg.m_spiRdm = spiRdm;
		if(lumenradio_write_register(device, LUMENRADIO_REG_CONFIG, &configReg) == false)
		{
			return false;
		}
	}
	return true;
}

/**
 *@brief  RDM认证
 *@param  device：LumenRadio设备
 *@param  active：RDM认证控制
 *@return 是否控制成功
 */
bool lumenradio_rdm_identify_active(LumenRadioDevice_t *device, bool active)
{
	LumenRadioStatusReg_t statusReg = {.m_identify = 0};
	statusReg.m_identify = (active == false) ? 0 : 1;
	if(lumenradio_write_register(device, LUMENRADIO_REG_STATUS, &statusReg) == false)
	{
		return false;
	}
	return true;
}

/**
 *@brief  设置绑定UID
 *@param  device    ：LumenRadio设备
 *@param  bindingUid：绑定UID
 *@return 是否设置成功
 */
bool lumenradio_set_binding_uid(LumenRadioDevice_t *device, const char *bindingUid)
{
	uint32_t bindingUidLenSet, bindingUidLenMax;
	LumenRadioBindingUidReg_t bindingUidReg = {.m_uid = {'\0'}};
//参数检查
	if(bindingUid == NULL)
	{
		return false;
	}
//数据处理
	bindingUidLenSet = strlen(bindingUid);
	bindingUidLenMax = sizeof(bindingUidReg.m_uid);
	bindingUidLenSet = (bindingUidLenSet > bindingUidLenMax) ? bindingUidLenMax : bindingUidLenSet;
//设置绑定UID
	memcpy(bindingUidReg.m_uid, bindingUid, bindingUidLenSet);
	if(lumenradio_write_register(device, LUMENRADIO_REG_BINDING_UID, &bindingUidReg) == false)
	{
		return false;
	}
	return true;
}

/**
 *@brief  设置OEM信息
 *@param  device        ：LumenRadio设备
 *@param  manufacturerId：厂商ID
 *@param  deviceModelId ：设备类ID
 *@return 是否设置成功
 */
bool lumenradio_set_oem_info(LumenRadioDevice_t *device, uint16_t manufacturerId, uint16_t deviceModelId)
{
	LumenRadioOemInfoReg_t oemInfoReg;
	oemInfoReg.m_manufacturerId = manufacturerId;
	oemInfoReg.m_deviceModelId  = deviceModelId;
	if(lumenradio_write_register(device, LUMENRADIO_REG_OEM_INFO, &oemInfoReg) == false)
	{
		return false;
	}
	return true;
}

/********************************************************************************************
 *   ASC功能函数定义
 ********************************************************************************************/
/**
 *@brief  读取ASC帧
 *@param  device   ：LumenRadio设备
 *@param  startCode：起始码
 *@param  ascData  ：ASC数据
 *@param  dataNum  ：数据数量
 *@return 是否读取成功
 */
bool lumenradio_read_asc_frame(LumenRadioDevice_t *device, uint8_t *startCode, uint8_t *ascData, uint16_t *dataNum)
{
	uint8_t readNum;
	uint16_t readCount;
	LumenRadioAscFrameReg_t ascFrameReg;
//参数检查
	if((startCode == NULL) || (ascData == NULL) || (dataNum == NULL))
	{
		return false;
	}
//获取ASC帧信息，并复位读计数器
	if(lumenradio_read_register(device, LUMENRADIO_REG_ASC_FRAME, &ascFrameReg) == false)
	{
		*dataNum = 0;
		return false;
	}
	if(ascFrameReg.m_ascFrameLength > 512)
	{
		*dataNum = 0;
		return false;
	}
//读取ASC数据
	readCount = 0;
	while(ascFrameReg.m_ascFrameLength > 0)
	{
		readNum = LUMENRADIO_SPI_READ_ASC_MAX;
		readNum = (readNum > ascFrameReg.m_ascFrameLength) ? ascFrameReg.m_ascFrameLength : readNum;
		if(lumenradio_read_asc_data(device, ascData, readNum) == false)
		{
			break;
		}
		readCount += readNum;
		ascData += readNum;
		ascFrameReg.m_ascFrameLength -= readNum;
	}
	*dataNum = readCount;
	*startCode = ascFrameReg.m_startCode;
	return ((ascFrameReg.m_ascFrameLength > 0) ? false : true);
}

/********************************************************************************************
 *   BLE功能函数定义
 ********************************************************************************************/
/**
 *@brief  蓝牙使/失能
 *@param  device：LumenRadio设备
 *@param  enable：蓝牙使/失能控制
 *@return 是否控制成功
 */
bool lumenradio_ble_enable(LumenRadioDevice_t *device, bool enable)
{
	LumenRadioBleStatusReg_t bleStatusReg = {.m_bleEnabled = 0};
	bleStatusReg.m_bleEnabled = (enable == false) ? 0 : 1;
	if(lumenradio_write_register(device, LUMENRADIO_REG_BLE_STATUS, &bleStatusReg) == false)
	{
		return false;
	}
	return true;
}

/**
 *@brief  设置蓝牙PIN码
 *@param  device：LumenRadio设备
 *@param  blePin：蓝牙PIN码，'0'~'9'的组合序列
 *@return 是否设置成功
 */
bool lumenradio_set_ble_pin(LumenRadioDevice_t *device, const char *blePin)
{
	uint32_t blePinLenSet, blePinLenMax;
	LumenRadioBlePinReg_t blePinReg = {.m_blePin = {'\0'}};
//参数检查
	if(blePin == NULL)
	{
		return false;
	}
//数据处理
	blePinLenSet = strlen(blePin);
	blePinLenMax = sizeof(blePinReg.m_blePin);
	blePinLenSet = (blePinLenSet > blePinLenMax) ? blePinLenMax : blePinLenSet;
//设置蓝牙PIN码
	memcpy(blePinReg.m_blePin, blePin, blePinLenSet);
	if(lumenradio_write_register(device, LUMENRADIO_REG_BLE_PIN, &blePinReg) == false)
	{
		return false;
	}
	return true;
}

/**
 *@brief  设置电池电量
 *@param  device  ：LumenRadio设备
 *@param  batLevel：电池电量，255表示无电池
 *@return 是否设置成功
 */
bool lumenradio_set_battery_level(LumenRadioDevice_t *device, uint8_t batLevel)
{
	LumenRadioBatteryReg_t batteryReg;
	batteryReg.m_batteryLevel = (batLevel == 255) ? 255 : ((batLevel > 100) ? 100 : batLevel);
	if(lumenradio_write_register(device, LUMENRADIO_REG_BATTERY, &batteryReg) == false)
	{
		return false;
	}
	return true;
}

/********************************************************************************************
 *   Universe功能函数定义
 ********************************************************************************************/
/**
 *@brief  设置Universe颜色
 *@param  device：LumenRadio设备
 *@param  rValue：红色值
 *@param  gValue：绿色值
 *@param  bValue：蓝色值
 *@return 是否设置成功
 */
bool lumenradio_set_universe_color(LumenRadioDevice_t *device, uint8_t rValue, uint8_t gValue, uint8_t bValue)
{
	LumenRadioUniverseColorReg_t universeColorReg;
	universeColorReg.m_rValue = rValue;
	universeColorReg.m_gValue = gValue;
	universeColorReg.m_bValue = bValue;
	if(lumenradio_write_register(device, LUMENRADIO_REG_UNIVERSE_COLOR, &universeColorReg) == false)
	{
		return false;
	}
	return true;
}

/**
 *@brief  设置设备名称
 *@param  device ：LumenRadio设备
 *@param  devName：设备名称
 *@return 是否设置成功
 */
bool lumenradio_set_device_name(LumenRadioDevice_t *device, const char *devName)
{
	uint32_t devNameLenSet, devNameLenMax;
	LumenRadioDeviceNameReg_t deviceNameReg = {.m_deviceName = {'\0'}};
//参数检查
	if(devName == NULL)
	{
		return false;
	}
//数据处理
	devNameLenSet = strlen(devName);
	devNameLenMax = sizeof(deviceNameReg.m_deviceName) - 1;
	devNameLenSet = (devNameLenSet > devNameLenMax) ? devNameLenMax : devNameLenSet;
//设置设备名称
	memcpy(deviceNameReg.m_deviceName, devName, devNameLenSet);
	if(lumenradio_write_register(device, LUMENRADIO_REG_DEVICE_NAME, &deviceNameReg) == false)
	{
		return false;
	}
	return true;
}

/**
 *@brief  获取Universe名称
 *@param  device ：LumenRadio设备
 *@param  uniName：Universe名称
 *@return 是否获取成功
 */
bool lumenradio_get_universe_name(LumenRadioDevice_t *device, char *uniName)
{
	LumenRadioUniverseNameReg_t universeNameReg;
//参数检查
	if(uniName == NULL)
	{
		return false;
	}
//获取Universe名称
	if(lumenradio_read_register(device, LUMENRADIO_REG_UNIVERSE_NAME, &universeNameReg) == false)
	{
		return false;
	}
	universeNameReg.m_universeName[sizeof(universeNameReg.m_universeName) - 1] = '\0';
	memcpy(uniName, universeNameReg.m_universeName, strlen(universeNameReg.m_universeName) + 1);
	return true;
}
