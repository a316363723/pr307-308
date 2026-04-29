#ifndef _LUMENRADIO_API_H_
#define _LUMENRADIO_API_H_
#include <string.h>
#include "dev_lumenradio_drive.h"
/********************************************************************************************
 *   LumenRadio中断掩码
 ********************************************************************************************/
#define lumenradio_bit_mask(x)                 (0x00000001UL << (x))   //位掩码函数
#define LUMENRADIO_IRQ_RX_DMX                  lumenradio_bit_mask(0)  //Rx DMX中断
#define LUMENRADIO_IRQ_LOST_DMX                lumenradio_bit_mask(1)  //Lost DMX中断
#define LUMENRADIO_IRQ_DMX_CHANGED             lumenradio_bit_mask(2)  //DMX Changed中断
#define LUMENRADIO_IRQ_RF_LINK                 lumenradio_bit_mask(3)  //Rf Link中断
#define LUMENRADIO_IRQ_ASC                     lumenradio_bit_mask(4)  //ASC中断
#define LUMENRADIO_IRQ_IDENTIFY                lumenradio_bit_mask(5)  //Identify中断
#define LUMENRADIO_IRQ_EXTENDED                lumenradio_bit_mask(6)  //Extended中断
#define LUMENRADIO_IRQ_RDM_REQUEST             lumenradio_bit_mask(7)  //RDM Request中断
#define LUMENRADIO_IRQ_RXTX_DA                 lumenradio_bit_mask(8)  //Rx Tx Da中断
#define LUMENRADIO_IRQ_RXTX_CTS                lumenradio_bit_mask(9)  //Rx Tx Cts中断
#define LUMENRADIO_IRQ_UNIV_META_CHANGED       lumenradio_bit_mask(10) //Univ Meta Changed中断

/********************************************************************************************
 *   LumenRadio标志掩码
 ********************************************************************************************/
#define LUMENRADIO_FLAG_LINKED                 lumenradio_bit_mask(0)  //Linked标志
#define LUMENRADIO_FLAG_RF_LINK                lumenradio_bit_mask(1)  //Rf Link标志
#define LUMENRADIO_FLAG_IDENTIFY               lumenradio_bit_mask(2)  //Identify标志
#define LUMENRADIO_FLAG_DMX                    lumenradio_bit_mask(3)  //DMX标志
#define LUMENRADIO_FLAG_UPDATE_MODE            lumenradio_bit_mask(4)  //Update Mode标志
#define LUMENRADIO_FLAG_BLE_PIN                lumenradio_bit_mask(5)  //Ble Pin标志
#define LUMENRADIO_FLAG_DATA_AVAILABLE         lumenradio_bit_mask(6)  //Data Available标志
#define LUMENRADIO_FLAG_CLEAR_TO_SEND          lumenradio_bit_mask(7)  //Clear To Send标志
#define LUMENRADIO_FLAG_RX_DMX_IRQ             lumenradio_bit_mask(8)  //Rx DMX中断标志
#define LUMENRADIO_FLAG_LOST_DMX_IRQ           lumenradio_bit_mask(9)  //Lost DMX中断标志
#define LUMENRADIO_FLAG_DMX_CHANGED_IRQ        lumenradio_bit_mask(10) //DMX Changed中断标志
#define LUMENRADIO_FLAG_RF_LINK_IRQ            lumenradio_bit_mask(11) //Rf Link中断标志
#define LUMENRADIO_FLAG_ASC_IRQ                lumenradio_bit_mask(12) //ASC中断标志
#define LUMENRADIO_FLAG_IDENTIFY_IRQ           lumenradio_bit_mask(13) //Identify中断标志
#define LUMENRADIO_FLAG_EXTENDED_IRQ           lumenradio_bit_mask(14) //Extended中断标志
#define LUMENRADIO_FLAG_RDM_REQUEST_IRQ        lumenradio_bit_mask(15) //RDM Request中断标志
#define LUMENRADIO_FLAG_RXTX_DA_IRQ            lumenradio_bit_mask(16) //Rx Tx Da中断标志
#define LUMENRADIO_FLAG_RXTX_CTS_IRQ           lumenradio_bit_mask(17) //Rx Tx Cts中断标志
#define LUMENRADIO_FLAG_UNIV_META_CHANGED_IRQ  lumenradio_bit_mask(18) //Univ Meta Changed中断标志

/********************************************************************************************
 *   LumenRadio工作模式配置值
 ********************************************************************************************/
typedef enum
{
	LUMENRADIO_WORK_MODE_RECEIVER    = 0, //接收器
	LUMENRADIO_WORK_MODE_TRANSMITTER = 1, //发送器
}LumenRadioWorkMode_t;

/********************************************************************************************
 *   LumenRadio发送器模式配置值
 ********************************************************************************************/
typedef enum
{
	LUMENRADIO_TRAN_MODE_CLASSIC = 0, //经典CRMX
	LUMENRADIO_TRAN_MODE_CRMX2   = 1, //CRMX2
}LumenRadioTransmitterMode_t;

/********************************************************************************************
 *   LumenRadio射频输出功率配置值
 ********************************************************************************************/
typedef enum
{
	LUMENRADIO_RF_POWER_100MW = 2, //100mW
	LUMENRADIO_RF_POWER_40MW  = 3, //40mW
	LUMENRADIO_RF_POWER_13MW  = 4, //13mW
	LUMENRADIO_RF_POWER_3MW   = 5, //3mW
}LumenRadioRfPower_t;

/********************************************************************************************
 *   LumenRadio标志关系
 ********************************************************************************************/
typedef enum
{
	LUMENRADIO_FLAGS_RELA_OR = 0, //或关系
	LUMENRADIO_FLAGS_RELA_AND,    //与关系
}LumenRadioFlagsRelation_t;

/********************************************************************************************
 *   LumenRadio射频通道阻塞
 ********************************************************************************************/
typedef struct
{
	bool    m_channelEnable; //通道使/失能
	uint8_t m_startChannel;  //起始通道编号
	uint8_t m_endChannel;    //结束通道编号
}LumenRadioRfBlocked_t;

/********************************************************************************************
 *   函数声明
 ********************************************************************************************/
bool lumenradio_spi_irq_handler(LumenRadioDevice_t *device);
bool lumenradio_uart_output_enable(LumenRadioDevice_t *device, bool enable);
bool lumenradio_irq_enable(LumenRadioDevice_t *device, uint32_t irqs, bool enable);
bool lumenradio_get_status(LumenRadioDevice_t *device, uint32_t flags, LumenRadioFlagsRelation_t relation, bool *status);
bool lumenradio_get_version(LumenRadioDevice_t *device, uint32_t *hwVersion, uint32_t *swVersion);
/********************************************************************************************/
bool lumenradio_wireless_operation_enable(LumenRadioDevice_t *device, bool enable);
bool lumenradio_rf_channels_enable(LumenRadioDevice_t *device, const LumenRadioRfBlocked_t *blockedTab, uint8_t blockedNum);
bool lumenradio_set_work_mode(LumenRadioDevice_t *device, LumenRadioWorkMode_t workMode);
bool lumenradio_set_linking_key(LumenRadioDevice_t *device, uint64_t linkCode, LumenRadioTransmitterMode_t tranMode, uint8_t universe);
bool lumenradio_set_rf_output_power(LumenRadioDevice_t *device, LumenRadioRfPower_t rfPower);
bool lumenradio_start_linking(LumenRadioDevice_t *device);
bool lumenradio_unlink(LumenRadioDevice_t *device);
bool lumenradio_get_link_quality(LumenRadioDevice_t *device, uint8_t *quality);
/********************************************************************************************/
bool lumenradio_internal_generation_enable(LumenRadioDevice_t *device, bool enable);
bool lumenradio_set_dmx_window(LumenRadioDevice_t *device, uint16_t startAddr, uint16_t windowSize);
bool lumenradio_set_dmx_param(LumenRadioDevice_t *device, uint16_t slotSpace, uint32_t frameLength);
bool lumenradio_read_dmx_frame(LumenRadioDevice_t *device, uint8_t *dmxData, uint16_t *dataNum);
bool lumenradio_write_dmx_frame(LumenRadioDevice_t *device, const uint8_t *dmxData, uint16_t dataNum);
/********************************************************************************************/
bool lumenradio_spi_rdm_enable(LumenRadioDevice_t *device, bool enable);
bool lumenradio_rdm_identify_active(LumenRadioDevice_t *device, bool active);
bool lumenradio_set_binding_uid(LumenRadioDevice_t *device, const char *bindingUid);
bool lumenradio_set_oem_info(LumenRadioDevice_t *device, uint16_t manufacturerId, uint16_t deviceModelId);
/********************************************************************************************/
bool lumenradio_read_asc_frame(LumenRadioDevice_t *device, uint8_t *startCode, uint8_t *ascData, uint16_t *dataNum);
/********************************************************************************************/
bool lumenradio_ble_enable(LumenRadioDevice_t *device, bool enable);
bool lumenradio_set_ble_pin(LumenRadioDevice_t *device, const char *blePin);
bool lumenradio_set_battery_level(LumenRadioDevice_t *device, uint8_t batLevel);
/********************************************************************************************/
bool lumenradio_set_universe_color(LumenRadioDevice_t *device, uint8_t rValue, uint8_t gValue, uint8_t bValue);
bool lumenradio_set_device_name(LumenRadioDevice_t *device, const char *devName);
bool lumenradio_get_universe_name(LumenRadioDevice_t *device, char *uniName);

#endif
