#ifndef _LUMENRADIO_MANUAL_H_
#define _LUMENRADIO_MANUAL_H_
#include <stdint.h>
/********************************************************************************************
 *   LumenRadio命令
 ********************************************************************************************/
#define LUMENRADIO_CMD_READ_REG(regAddr)    (0x00 | ((regAddr) & 0x3F)) //读寄存器
#define LUMENRADIO_CMD_WRITE_REG(regAddr)   (0x40 | ((regAddr) & 0x3F)) //写寄存器
#define LUMENRADIO_CMD_READ_DMX             0x81 //读DMX数据
#define LUMENRADIO_CMD_READ_ASC             0x82 //读ASC数据
#define LUMENRADIO_CMD_READ_RDM             0x83 //读RDM数据
#define LUMENRADIO_CMD_READ_RXTX            0x84 //读RXTX数据
#define LUMENRADIO_CMD_WRITE_DMX            0x91 //写DMX数据
#define LUMENRADIO_CMD_WRITE_RDM            0x92 //写RDM数据
#define LUMENRADIO_CMD_WRITE_RXTX           0x93 //写RXTX数据
#define LUMENRADIO_CMD_NOP                  0xFF //空操作

/********************************************************************************************
 *   LumenRadio寄存器地址
 ********************************************************************************************/
#define LUMENRADIO_REG_CONFIG               0x00 //Config寄存器地址
#define LUMENRADIO_REG_STATUS               0x01 //Status寄存器地址
#define LUMENRADIO_REG_IRQ_MASK             0x02 //Irq Mask寄存器地址
#define LUMENRADIO_REG_IRQ_FLAGS            0x03 //Irq Flags寄存器地址
#define LUMENRADIO_REG_DMX_WINDOW           0x04 //DMX Window寄存器地址
#define LUMENRADIO_REG_ASC_FRAME            0x05 //ASC Frame寄存器地址
#define LUMENRADIO_REG_LINK_QUALITY         0x06 //Link Quality寄存器地址
#define LUMENRADIO_REG_DMX_SPEC             0x08 //DMX Spec寄存器地址
#define LUMENRADIO_REG_DMX_CONTROL          0x09 //DMX Control寄存器地址
#define LUMENRADIO_REG_EXTENDED_IRQ_MASK    0x0A //Extended Irq Mask寄存器地址
#define LUMENRADIO_REG_EXTENDED_IRQ_FLAGS   0x0B //Extended Irq Flags寄存器地址
#define LUMENRADIO_REG_VERSION              0x10 //Version寄存器地址
#define LUMENRADIO_REG_RF_POWER             0x11 //RF Power寄存器地址
#define LUMENRADIO_REG_BLOCKED_CHANNELS     0x12 //Blocked Channels寄存器地址
#define LUMENRADIO_REG_BINDING_UID          0x20 //Binding UID寄存器地址
#define LUMENRADIO_REG_LINKING_KEY          0x21 //Linking Key寄存器地址
#define LUMENRADIO_REG_BLE_STATUS           0x30 //Ble Status寄存器地址
#define LUMENRADIO_REG_BLE_PIN              0x31 //Ble PIN寄存器地址
#define LUMENRADIO_REG_BATTERY              0x32 //Battery寄存器地址
#define LUMENRADIO_REG_UNIVERSE_COLOR       0x33 //Universe Color寄存器地址
#define LUMENRADIO_REG_OEM_INFO             0x34 //OEM Info寄存器地址
#define LUMENRADIO_REG_RXTX_STATUS          0x35 //RXTX Status寄存器地址
#define LUMENRADIO_REG_DEVICE_NAME          0x36 //Device Name寄存器地址
#define LUMENRADIO_REG_UNIVERSE_NAME        0x37 //Universe Name寄存器地址

/********************************************************************************************
 *   SPI操作数据长度
 ********************************************************************************************/
#define LUMENRADIO_SPI_READ_DMX_MAX         128 //单次最多可读取DMX数据长度
#define LUMENRADIO_SPI_READ_ASC_MAX         128 //单次最多可读取ASC数据长度
#define LUMENRADIO_SPI_WRITE_DMX_MAX        128 //单次最多可写入DMX数据长度

/********************************************************************************************
 *   SPI片选建立时间
 ********************************************************************************************/
#define LUMENRADIO_SPI_NSS_SETUP_TIME       5 //单位：us

/********************************************************************************************
 *   LumenRadio射频通道信息
 ********************************************************************************************/
#define LUMENRADIO_RF_CHANNEL_START_NUMBER  2  //通道起始编号
#define LUMENRADIO_RF_CHANNEL_END_NUMBER    80 //通道结束编号
#define LUMENRADIO_RF_CHANNEL_ACTIVE_LEAST  16 //通道最少活动数量

/********************************************************************************************
 *   LumenRadio寄存器定义
 ********************************************************************************************/
#pragma pack(1)
typedef struct //Config寄存器
{
	uint8_t m_uartEn       : 1;
	uint8_t m_radioTxRxMode: 1;
	uint8_t m_reserved1    : 1;
	uint8_t m_spiRdm       : 1;
	uint8_t m_reserved2    : 3;
	uint8_t m_radioEnable  : 1;
}LumenRadioConfigReg_t;
/*********************************************************/
typedef struct //Status寄存器
{
	uint8_t m_linked    : 1;
	uint8_t m_rfLink    : 1;
	uint8_t m_identify  : 1;
	uint8_t m_dmx       : 1;
	uint8_t m_reserved  : 3;
	uint8_t m_updateMode: 1;
}LumenRadioStatusReg_t;
/*********************************************************/
typedef struct //Irq Mask寄存器
{
	uint8_t m_rxDmxIrqEn     : 1;
	uint8_t m_lostDmxIrqEn   : 1;
	uint8_t m_dmxChangedIrqEn: 1;
	uint8_t m_rfLinkIrqEn    : 1;
	uint8_t m_ascIrqEn       : 1;
	uint8_t m_identifyIrqEn  : 1;
	uint8_t m_extendedIrqEn  : 1;
}LumenRadioIrqMaskReg_t;
/*********************************************************/
typedef struct //Irq Flags寄存器
{
	uint8_t m_rxDmxIrq     : 1;
	uint8_t m_lostDmxIrq   : 1;
	uint8_t m_dmxChangedIrq: 1;
	uint8_t m_rfLinkIrq    : 1;
	uint8_t m_ascIrq       : 1;
	uint8_t m_identifyIrq  : 1;
	uint8_t m_extendedIrq  : 1;
	uint8_t m_spiDeviceBusy: 1;
}LumenRadioIrqFlagsReg_t;
/*********************************************************/
typedef struct //DMX Window寄存器
{
	uint16_t m_startAddress;
	uint16_t m_windowSize;
}LumenRadioDmxWindowReg_t;
/*********************************************************/
typedef struct //ASC Frame寄存器
{
	uint8_t  m_startCode;
	uint16_t m_ascFrameLength;
}LumenRadioAscFrameReg_t;
/*********************************************************/
typedef struct //Link Quality寄存器
{
	uint8_t m_pdr;
}LumenRadioLinkQualityReg_t;
/*********************************************************/
typedef struct //DMX Spec寄存器
{
	uint32_t m_refreshPeriod;
	uint16_t m_interslotTime;
	uint16_t m_nChannels;
}LumenRadioDmxSpecReg_t;
/*********************************************************/
typedef struct //DMX Control寄存器
{
	uint8_t m_enable: 1;
}LumenRadioDmxControlReg_t;
/*********************************************************/
typedef struct //Extended Irq Mask寄存器
{
	uint32_t m_rdmRequestEn     : 1;
	uint32_t m_rxTxDaEn         : 1;
	uint32_t m_rxTxCtsEn        : 1;
	uint32_t m_reserved         : 3;
	uint32_t m_univMetaChangedEn: 1;
}LumenRadioExtendedIrqMaskReg_t;
/*********************************************************/
typedef struct //Extended Irq Flags寄存器
{
	uint32_t m_rdmRequest     : 1;
	uint32_t m_rxTxDa         : 1;
	uint32_t m_rxTxCts        : 1;
	uint32_t m_reserved       : 3;
	uint32_t m_univMetaChanged: 1;
}LumenRadioExtendedIrqFlagsReg_t;
/*********************************************************/
typedef struct //Version寄存器
{
	uint32_t m_driverVersion;
	uint32_t m_hwVersion;
}LumenRadioVersionReg_t;
/*********************************************************/
typedef struct //RF Power寄存器
{
	uint8_t m_outputPower;
}LumenRadioRfPowerReg_t;
/*********************************************************/
typedef struct //Blocked Channels寄存器
{
	uint8_t m_flags[11];
}LumenRadioBlockedChannelsReg_t;
/*********************************************************/
typedef struct //Binding UID寄存器
{
	char m_uid[6];
}LumenRadioBindingUidReg_t;
/*********************************************************/
typedef struct //Linking Key寄存器
{
	uint8_t  m_universe;
	uint8_t  m_mode;
	uint64_t m_code;
}LumenRadioLinkingKeyReg_t;
/*********************************************************/
typedef struct //Ble Status寄存器
{
	uint8_t m_bleEnabled: 1;
	uint8_t m_blePin    : 1;
}LumenRadioBleStatusReg_t;
/*********************************************************/
typedef struct //Ble PIN寄存器
{
	char m_blePin[6];
}LumenRadioBlePinReg_t;
/*********************************************************/
typedef struct //Battery寄存器
{
	uint8_t m_batteryLevel;
}LumenRadioBatteryReg_t;
/*********************************************************/
typedef struct //Universe Color寄存器
{
	uint8_t m_bValue;
	uint8_t m_gValue;
	uint8_t m_rValue;
}LumenRadioUniverseColorReg_t;
/*********************************************************/
typedef struct //OEM Info寄存器
{
	uint16_t m_deviceModelId;
	uint16_t m_manufacturerId;
}LumenRadioOemInfoReg_t;
/*********************************************************/
typedef struct //RXTX Status寄存器
{
	uint8_t m_dataAvailable: 1;
	uint8_t m_clearToSend  : 1;
}LumenRadioRxTxStatusReg_t;
/*********************************************************/
typedef struct //Device Name寄存器
{
	char m_deviceName[32];
}LumenRadioDeviceNameReg_t;
/*********************************************************/
typedef struct //Universe Name寄存器
{
	char m_universeName[16];
}LumenRadioUniverseNameReg_t;
#pragma pack()

#endif
