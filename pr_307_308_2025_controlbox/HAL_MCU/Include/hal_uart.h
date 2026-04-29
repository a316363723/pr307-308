#ifndef HAL_UART_H
#define HAL_UART_H

#include <stdint.h>


enum hal_uart
{
	HAL_UART_DMX,
	HAL_UART_CRMX,
	HAL_UART_BLE,
	HAL_UART_RS485,
	HAL_UART_ADAPTER,
	HAL_UART_DEBUG,
	HAL_UART_NUM,
};

enum hal_uart_irq
{
	HAL_UART_IRQ_ERR,
	HAL_UART_IRQ_TX_DMA_FT,
	HAL_UART_IRQ_RX_TIMEOUT,
    HAL_UART_IRQ_RX_TIMER,
};

enum hal_uart_flag
{
	HAL_USART_FLAG_PE = 0,   
	HAL_USART_FLAG_FE,   
	HAL_USART_FLAG_ORE,
    HAL_USART_FLAG_BE,   
    HAL_USART_FLAG_RXNE, 
    HAL_USART_FLAG_TC,   
    HAL_USART_FLAG_TXE,  
    HAL_USART_FLAG_RTOF, 
    HAL_USART_FLAG_LBD,  
    HAL_USART_FLAG_WKUP ,
    HAL_USART_FLAG_MPB,  
};

enum hal_uart_cmd{
    HAL_USART_RX,        
    HAL_USART_TX,       
    HAL_USART_RTO,       
    HAL_USART_INT_RX,   
    HAL_USART_INT_TXE, 
    HAL_USART_INT_TC, 
    HAL_USART_INT_RTO,  
};

void hal_uart_init(enum hal_uart unit ,uint32_t u32Baudrate);
void hal_uart_transmit_dma_start(enum hal_uart unit, const uint8_t *p_data, uint32_t size);
void hal_uart_receive_dma_start(enum hal_uart unit, uint8_t *data, uint32_t size);
uint32_t hal_uart_get_transmit_dma_cnt(enum hal_uart unit);
uint32_t hal_uart_get_receive_dma_cnt(enum hal_uart unit);
void hal_uart_transmit_dma_stop(enum hal_uart unit);
void hal_uart_receive_dma_channel_stop(enum hal_uart unit);
void hal_uart_receive_dma_channel_start(enum hal_uart unit);
void hal_dmx_write_ctrl_pin(enum hal_uart unit, uint8_t state);
void hal_dmx_write_usart_tx_pin(enum hal_uart unit, uint8_t state);
void hal_ble_uart_set_tx_pin(enum hal_uart unit);
void hal_uart_irq_register(enum hal_uart unit, enum hal_uart_irq irq_type, void(*irq_cb)(void));
uint8_t hal_uart_get_status(enum hal_uart unit, enum hal_uart_flag flag);
void hal_uart_funcmd(enum hal_uart unit, enum hal_uart_cmd fun, uint8_t status);
void hal_dmx_usart_enable(enum hal_uart unit);
void hal_uart_send_data(enum hal_uart unit ,uint8_t *data, uint32_t size);
void hal_uart_generate_break(enum hal_uart unit, uint32_t us);
void hal_uart_set_stopbit(enum hal_uart unit, uint32_t stopbit);
void hal_ble_uart_set_enale(enum hal_uart unit);

#endif  /* HAL_UART_H */

