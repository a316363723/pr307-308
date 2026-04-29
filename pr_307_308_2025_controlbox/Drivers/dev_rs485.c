#include "dev_rs485.h"
#include <stdarg.h>
#include <string.h>
#include "cmsis_os2.h"
#include "hal_gpio.h"
#include "hal_uart.h"
#include "hc32_ddl.h"
#include "perf_counter.h"
#include "sys_delay.h"

osMessageQueueId_t os_queue_usart_rs485_rx;
osSemaphoreId_t    os_semaphore_rs485_tx;

rs485_pack_type rx_pack = {0};

static void rs485_rx_complete_int_cb(void)
{

	hal_uart_receive_dma_channel_stop(HAL_UART_RS485);
    rx_pack.length = sizeof(rx_pack.data) - hal_uart_get_receive_dma_cnt(HAL_UART_RS485);
//    memcpy(rx_pack.data, rx_pack.data, rx_pack.length);
	osMessageQueuePut(os_queue_usart_rs485_rx, &rx_pack, NULL, 0);
	
	hal_uart_receive_dma_start(HAL_UART_RS485, rx_pack.data, sizeof(rx_pack.data));
	hal_uart_receive_dma_channel_start(HAL_UART_RS485);

}
void rs485_init_config(void)
{
    rs485_pack_type rx_pack;
    os_queue_usart_rs485_rx = osMessageQueueNew(10, sizeof(rs485_pack_type), NULL);
    os_semaphore_rs485_tx =   osSemaphoreNew(1, 0, NULL);

	hal_uart_init(HAL_UART_RS485,250000);
    hal_uart_receive_dma_start(HAL_UART_RS485, rx_pack.data, sizeof(rx_pack.data));
	hal_uart_irq_register(HAL_UART_RS485, HAL_UART_IRQ_RX_TIMEOUT, rs485_rx_complete_int_cb);
    
}

static void rs485_tx_complete_int_cb(void)
{
     osSemaphoreRelease(os_semaphore_rs485_tx);
}

int rs485_usart_tx_data(uint8_t* p_data, uint32_t size, uint32_t timeout)
{
    int32_t sl;
    hal_dmx_write_ctrl_pin(HAL_UART_RS485, 1);
    osSemaphoreAcquire(os_semaphore_rs485_tx, 0);
    hal_uart_transmit_dma_start(HAL_UART_RS485, p_data, size);
    hal_uart_irq_register(HAL_UART_RS485, HAL_UART_IRQ_TX_DMA_FT, rs485_tx_complete_int_cb);
    osSemaphoreAcquire(os_semaphore_rs485_tx, (size / 25) + 2);
	sl = osKernelLock();
	sys_delay_us(100);
    hal_dmx_write_ctrl_pin(HAL_UART_RS485, 0);
	osKernelRestoreLock(sl);
    return size;
}

int rs485_usart_rx_data(uint8_t* p_data, uint32_t size, uint32_t timeout)
{
    if(osOK == osMessageQueueGet(os_queue_usart_rs485_rx, &rx_pack, NULL, timeout))
    {
        memcpy(p_data, rx_pack.data, rx_pack.length);
        return rx_pack.length;
    }

    return 0;
}
