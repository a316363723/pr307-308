#include "dev_crmx.h"
#include "hal_spi.h"
#include "hal_gpio.h"
#include "cmsis_os2.h"
#include "perf_counter.h"
#include "hal_uart.h"

#define CRMX_IRQ_PORT   HAL_GPIOC
#define CRMX_IRQ_PIN    HAL_PIN_3  

#define CRMX_EN_PORT   HAL_GPIOA
#define CRMX_EN_PIN    HAL_PIN_8  

#define CRMX_EN_ON()     do{hal_gpio_write_pin(CRMX_EN_PORT, CRMX_EN_PIN, 1);}while(0)
#define CRMX_EN_OFF()     do{hal_gpio_write_pin(CRMX_EN_PORT, CRMX_EN_PIN, 0);}while(0)

#define	CRMX_IRQ_READ()				hal_gpio_read_pin(CRMX_IRQ_PORT, CRMX_IRQ_PIN)
    
/* Lumenradio的SPI通信命令 */
#define TIMO_READ_REG_COMMAND(address)  (address)
#define TIMO_WRITE_REG_COMMAND(address) (0x40 | address)
#define TIMO_READ_DMX_COMMAND           0x81
#define TIMO_READ_ASC_COMMAND           0x82
#define TIMO_READ_RDM_COMMAND           0x83
#define TIMO_WRITE_DMX_COMMAND          0x91
#define TIMO_WRITE_RDM_COMMAND          0x92
#define TIMO_FW_BLOCK_CMD_1_COMMAND     0x8E
#define TIMO_FW_BLOCK_CMD_2_COMMAND     0x8F
#define TIMO_NOP_COMMAND                0xFF

#define TIMO_CONFIG_REG                 0x00
#define TIMO_STATUS_REG                 0x01
#define TIMO_IRQ_MASK_REG               0x02
#define TIMO_IRQ_FLAGS_REG              0x03
#define TIMO_DMX_WINDOW_REG             0x04
#define TIMO_ASC_FRAME_REG              0x05
#define TIMO_LINK_QUALITY_REG           0x06
#define TIMO_ANTENNA_REG                0x07
#define TIMO_DMX_SPEC_REG               0x08
#define TIMO_DMX_CONTROL_REG            0x09
#define TIMO_EXT_IRQ_MASK_REG           0x0A
#define TIMO_EXT_IRQ_FLAGS_REG          0x0B
#define TIMO_VERSION_REG                0x10
#define TIMO_RF_POWER_REG               0x11
#define TIMO_BLOCKED_CHANNELS_REG       0x12
#define TIMO_BINDING_UID_REG            0x20
#define TIMO_BLE_STATUS_REG             0x30
#define TIMO_BLE_PIN_REG                0x31
#define TIMO_BATTERY_REG                0x32
#define TIMO_UNIVERSE_COLOR_REG         0x33
#define TIMO_OEM_INFO_REG               0x34

#define TIMO_STATUS_LINKED_NOt          (1<<0)

#define TIMO_CONFIG_UART_EN             (1<<0)
#define TIMO_CONFIG_RADIO_TX_RX_MODE    (1<<1)
#define TIMO_CONFIG_SPI_RDM_EN          (1<<3)
#define TIMO_CONFIG_RADIO_EN            (1<<7)

#define TIMO_IRQ_EXTENDED_FLAG          (1<<6)
#define TIMO_IRQ_RF_LINK_FLAG           (1<<3)
#define TIMO_IRQ_DMX_CHANGED_FLAG       (1<<2)

#define TIMO_SPI_DEVICE_BUSY_IRQ_MASK 	(1 << 7)
#define TIMO_EXTIRQ_SPI_RDM_FLAG           (1<<0)
#define TIMO_EXTIRQ_SPI_RADIO_DISC_FLAG    (1<<3)

static osSemaphoreId_t semaphore_id;

static void irq_pin_cb(void);
LumenRadioDevice_t g_lumenradio_device;

void dev_crmx_init(void)
{
    semaphore_id = osSemaphoreNew (1, 0, NULL);
    hal_spi_init(HAL_SPI_CRMX);
    hal_gpio_init(CRMX_IRQ_PORT,CRMX_IRQ_PIN, HAL_PIN_MODE_IPU);
    //exint_func_assign();
    hal_gpio_irq_register(CRMX_IRQ_PORT, CRMX_IRQ_PIN, 28/*Int028_IRQn*/,5,HAL_EXIRQ_TRIGGER_FALLING, irq_pin_cb);
    
    hal_gpio_init(CRMX_EN_PORT,CRMX_EN_PIN, HAL_PIN_MODE_OUT_PP);
}

void dev_crmx_power_ctrl(uint8_t state)
{
    if(state == 1)
    {
        CRMX_EN_ON();
		hal_ble_uart_set_enale(HAL_UART_CRMX);
		set_spi_cs_pin(HAL_SPI_CRMX, 1);
		hal_spi_pin_enable(HAL_SPI_CRMX);
    }
    else
    {
		
        CRMX_EN_OFF(); 
		hal_ble_uart_set_tx_pin(HAL_UART_CRMX);
		set_spi_cs_pin(HAL_SPI_CRMX, 0);
		hal_spi_pin_reset(HAL_SPI_CRMX);
    }
}

uint8_t dev_crmx_irq_read(void)
{
	uint8_t status = 0;
	
	status = CRMX_IRQ_READ();
	
	return status;
}

void dev_crmx_cs_ctrl(uint8_t state)
{
     set_spi_cs_pin(HAL_SPI_CRMX, state);
}

uint8_t dev_crmx_send_read_data(uint8_t data)
{
	uint8_t read_data = 0;
	
    read_data = hal_spi_send_recv_byte(HAL_SPI_CRMX, data);
	
	return read_data;
}

static void irq_pin_cb(void)
{
	lumenradio_spi_irq_handler(&g_lumenradio_device);
    osSemaphoreRelease(semaphore_id);
}

//crmx发送数据  返回接收数据
uint8_t dev_crmx_data_send(uint8_t cmd, uint8_t tx_data)
{
    uint8_t irq_flags = 0;
    uint8_t rec_data = 0;
    uint16_t cnt = 0;
    
//crmx_send_restart:    
    osSemaphoreAcquire(semaphore_id, 0);
    set_spi_cs_pin(HAL_SPI_CRMX, 0);
    hal_spi_send_recv_byte(HAL_SPI_CRMX, cmd);
    set_spi_cs_pin(HAL_SPI_CRMX, 1);
    osSemaphoreAcquire(semaphore_id, 5);
    set_spi_cs_pin(HAL_SPI_CRMX, 0);
    irq_flags = hal_spi_send_recv_byte(HAL_SPI_CRMX, TIMO_NOP_COMMAND);
    if((irq_flags & TIMO_SPI_DEVICE_BUSY_IRQ_MASK) && (++cnt < 10))
    {
        set_spi_cs_pin(HAL_SPI_CRMX, 1);
//        osDelay(100);
//        goto crmx_send_restart;
    }
    cnt = 0;
    rec_data = hal_spi_send_recv_byte(HAL_SPI_CRMX, tx_data);
    set_spi_cs_pin(HAL_SPI_CRMX, 1);
    while(CRMX_IRQ_READ() == 0)
    {
		osDelay(1);
		cnt++;
		if(cnt >= 5)
			break;        
    }     
    return rec_data;  
}

//断开连接  1--断开成功  0-- 失败
uint8_t dev_crmx_disconnect(void)
{
    uint8_t rec_data = 0;
    
    dev_crmx_data_send(TIMO_WRITE_REG_COMMAND(TIMO_STATUS_REG),0x01);
    osDelay(10);
    rec_data = dev_crmx_data_send(TIMO_READ_REG_COMMAND(TIMO_STATUS_REG),0xff);
    if((rec_data & 0x01) == 0x01)  
        return 0;
    else
        return 1;
}

//检测是否连接  1--已连接
uint8_t dev_crmx_connect_detect(void)
{
    uint8_t rec_data = 0;
    rec_data = dev_crmx_data_send(TIMO_READ_REG_COMMAND(TIMO_STATUS_REG),0xff);
    if((rec_data & 0x03) == 0x03)
        return 1;
    else
        return 0;
}


