#ifndef _LUMENRADIO_DRIVE_H_
#define _LUMENRADIO_DRIVE_H_
#include <stddef.h>
#include <stdbool.h>
#include "dev_lumenradio_manual.h"
/********************************************************************************************
 *   断言功能定义
 ********************************************************************************************/
#define LUMENRADIO_ASSERT_EN               0 //断言功能开关
#if LUMENRADIO_ASSERT_EN
  #define lumenradio_assert(x)             do                           \
                                           {                            \
																						 if((x) == false) while(1); \
																					 }while(0)
#else
  #define lumenradio_assert(x)             ((void)0)
#endif

/********************************************************************************************
 *   SPI操作时长定义
 ********************************************************************************************/
#define LUMENRADIO_SPI_WAIT_IRQ_PEND_TIME  1 //等待IRQ挂起时长，单位：us
#define LUMENRADIO_SPI_WAIT_IRQ_PLUP_TIME  1  //等待IRQ拉高时长，单位：us

/********************************************************************************************
 *   SPI中断状态
 ********************************************************************************************/
typedef enum
{
	LUMENRADIO_SPI_IRQ_STATE_IDLE = 0, //空闲状态
	LUMENRADIO_SPI_IRQ_STATE_WAIT,     //等待状态
	LUMENRADIO_SPI_IRQ_STATE_PEND,     //挂起状态
}LumenRadioSpiIrqState_t;

/********************************************************************************************
 *   LumenRadio引脚状态
 ********************************************************************************************/
typedef enum
{
	LUMENRADIO_PIN_STATE_LOW = 0, //低电平
	LUMENRADIO_PIN_STATE_HIGH,    //高电平
}LumenRadioPinState_t;

/********************************************************************************************
 *   LumenRadio寄存器信息
 ********************************************************************************************/
typedef struct
{
	uint8_t m_regAddr; //寄存器地址
	uint8_t m_regSize; //寄存器长度
	bool    m_bigEnd;  //操作端模式
}LumenRadioRegInfo_t;

/********************************************************************************************
 *   LumenRadio设备
 ********************************************************************************************/
typedef struct
{
	uint8_t                          m_deviceId;                                    //设备ID
	volatile LumenRadioSpiIrqState_t m_spiIrqState;                                 //SPI中断状态
	void                           (*m_delayus)    (uint8_t, uint8_t); //微秒级延时函数
	void                           (*m_writeNssPin)(uint8_t, LumenRadioPinState_t); //写NSS引脚函数
	LumenRadioPinState_t           (*m_readIrqPin) (uint8_t                      ); //读IRQ引脚函数
	uint8_t                        (*m_spiTxRxByte)(uint8_t, uint8_t); //SPI收发字节函数
}LumenRadioDevice_t;

/********************************************************************************************
 *   函数声明
 ********************************************************************************************/
bool lumenradio_pend_spi_irq(LumenRadioDevice_t *device);
bool lumenradio_read_register(LumenRadioDevice_t *device, uint8_t regAddr, void *regValue);
bool lumenradio_write_register(LumenRadioDevice_t *device, uint8_t regAddr, const void *regValue);
bool lumenradio_read_dmx_data(LumenRadioDevice_t *device, uint8_t *dmxData, uint8_t dataNum);
bool lumenradio_read_asc_data(LumenRadioDevice_t *device, uint8_t *ascData, uint8_t dataNum);
bool lumenradio_write_dmx_data(LumenRadioDevice_t *device, const uint8_t *dmxData, uint8_t dataNum);
bool lumenradio_nop(LumenRadioDevice_t *device, LumenRadioIrqFlagsReg_t *irqFlagsReg);

#endif
