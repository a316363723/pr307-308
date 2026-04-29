#ifndef APP_DMX_H
#define APP_DMX_H
#include "hal_uart.h"
#include "cmsis_os.h"
#include <stdint.h>
#include <stdbool.h>

extern const osThreadAttr_t g_dmx_thread_attr;
extern osThreadId_t app_dmx_thread_task_id;

void app_dmx_thread_entrance(void *argument);

void dmx_uart_config(void);
void app_dmx_set_profile_size(uint16_t size);
void app_dmx_lumenradio_name_flag_set(uint8_t flag);
uint8_t app_dmx_lumenradio_name_flag_get(void);
bool app_dmx_get_connection_state(enum hal_uart unit);
void wired_dmx_generate_transmit_event(uint8_t* p_data, uint16_t size);
void app_dmx_eth_write_data(uint8_t *p_data, uint32_t size);
void wired_dmx_transmit_with_break(uint8_t* data, uint16_t size);
void wired_dmx_transmit_data(uint8_t* data, uint16_t size);
void app_dmx_eth_disconnet_handle(void);
uint8_t app_dmx_get_status(enum hal_uart mode);
void app_dmx_eth_set_transmit(void);
void app_dmx_last_data_clear(void);
void wired_dmx_transmit_with_mab(uint8_t* data, uint16_t size);

#endif


