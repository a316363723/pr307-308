#ifndef BSP_USART_H
#define BSP_USART_H
#include <stdint.h>
#include "hc32_ddl.h"


#define USART_MAX_RX_LEN    (1024)

typedef enum 
{
    HAL_USART_RS485,
	HAL_USART_DEBUG,
	HAL_USART_RS485_2,
	HAL_USART_RS485_3,
    HAL_USART_NUM,
}hal_usart_enum;

typedef struct transmit_para
{
	hal_usart_enum ch;
	M4_USART_TypeDef *USARTx;
	M4_DMA_TypeDef  *DMAx;
	uint8_t DMA_CH;
	uint32_t u32transInt;
	en_int_src_t enintsrc;
	IRQn_Type enirq;
	uint32_t priority;
	en_event_src_t event_src;
	uint8_t u8port;
	uint16_t u8pin;
	uint8_t u8func;
	void(*pfncallback)(void);
}transmit;

typedef struct receive_para
{
	hal_usart_enum ch;
	M4_USART_TypeDef *USARTx;
	M4_DMA_TypeDef  *DMAx;
	uint8_t DMA_CH;
	en_int_src_t enintsrc;
	IRQn_Type enirq;
	uint32_t priority;
	en_event_src_t event_src;
	void(*pfncallback)(void);
}receive;

typedef struct 
{
    uint32_t length;
    uint8_t  data[USART_MAX_RX_LEN];
}hal_usart_rx_pack_t;


void hal_usart_init(hal_usart_enum ch, uint32_t baudval);
void hal_usart_receive_dma_start(hal_usart_enum ch, void(*rx_complete_irq_callback)(hal_usart_rx_pack_t* pack));
void hal_rs485_write_ctrl_pin(uint8_t state);
void hal_usart_transmit_dma_start(hal_usart_enum ch);
void hal_usart_send(uint8_t *pData, uint16_t Cnt);
void rs485_ctrl_pin_init(void);
void Rs485_2_Init(void);

void hal_rs485_2_usart_send(uint8_t *pData, uint16_t Cnt);
void hal_rs485_3_usart_send(uint8_t *pData, uint16_t Cnt);
void hal_rs485_2_write_ctrl_pin(uint8_t state);
void hal_rs485_4_write_ctrl_pin(uint8_t state);


void Relay45_init(void);
void Relay23_init(void);
void Relay45_TX_485_Send(uint8_t *pData, uint16_t Cnt);
void Relay23_TX_485_Send(uint8_t *pData, uint16_t Cnt);
void Relay_Usart_Select_Init(void);
#endif
