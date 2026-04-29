#ifndef DEV_RS485_H
#define DEV_RS485_H
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>


typedef struct
{
    uint8_t length;
    uint8_t data[140];
} rs485_pack_type;

int rs485_usart_tx_data(uint8_t* p_data, uint32_t size, uint32_t timeout);
int rs485_usart_rx_data(uint8_t* p_data, uint32_t size, uint32_t timeout);
void rs485_init_config(void);


#endif
