#ifndef DEV_BLE_H
#define DEV_BLE_H

#include <stdint.h>
#include <stdbool.h>
#include "cmsis_os.h"


#if PROJECT_TYPE == 307
#define BLE_PRODUCT_UUID   "000K5"
#elif PROJECT_TYPE==308
#define BLE_PRODUCT_UUID   "000J5"
#endif


#define BLE_UUID_LEN      (32)
#define BLE_MAC_LEN       (12)

#define	BLE_RX_MAX_LEN	  192
#define	BLE_TX_MAX_LEN	  64

typedef enum 
{
    BLE_MESH_DATA,
    BLE_GATT_DATA,
}ble_send_data_type;

typedef struct 
{
	ble_send_data_type type;
    uint16_t length;
    uint8_t  data[BLE_TX_MAX_LEN];
}app_ble_tx_packet;

typedef struct 
{
    uint16_t length;
    uint8_t  data[BLE_TX_MAX_LEN];
}ble_tx_packet;

typedef struct 
{
    uint16_t length;
    uint8_t  data[BLE_RX_MAX_LEN];
}ble_rx_packet;


typedef struct
{
	void (*usart_tx_data)(const void *p_data, uint32_t size);
	bool (*usart_rx_data)(ble_rx_packet *rx_packet, uint32_t time_out);
	void (*usart_user_data)(ble_rx_packet *rx_packet);
}dev_ble_t;


bool ble_power_on_reset(dev_ble_t *p_dev);
bool ble_get_device_uuid(dev_ble_t *p_dev, const char *node_addr, uint8_t *uuid);
bool ble_set_device_uuid(dev_ble_t *p_dev, const uint8_t *uuid, uint8_t len);
bool ble_get_device_mac(dev_ble_t *p_dev, const char *node_addr, uint8_t* mac);
bool ble_send_string(dev_ble_t* p_dev, const char *node_addr, const uint8_t *p_str, uint8_t len);
bool ble_send_gatt_data(dev_ble_t* p_dev, const uint8_t* p_data, uint8_t len);
bool ble_node_reset(dev_ble_t* p_dev);
bool ble_get_version(dev_ble_t* p_dev);
void ble_deal_receive_data(dev_ble_t* p_dev);
void ble_power_on(void);
void ble_power_off(void);
void ble_product_uuid_generate(const char* product_uuid, uint8_t* mac, uint8_t* generate_uuid);
void ble_package_write_cmd(uint8_t *p_pack, uint16_t *pack_len, const char* node_addr, const char *cmd, uint8_t cmd_len, const uint8_t* Data, uint8_t DataLen);

void ble_send_data_queue(ble_send_data_type type, uint8_t *p_data, uint8_t length);
bool ble_usart_rx_data(ble_rx_packet *rx_packet, uint32_t time_out);
void ble_usart_tx_data(const void *p_data, uint32_t size);
void start_ble_reset(void);


extern osMessageQueueId_t os_queue_usart_ble_rx;
extern osMessageQueueId_t os_queue_usart_ble_tx;
extern osMessageQueueId_t os_queue_ble_cfx;
extern osSemaphoreId_t os_semaphore_ble_reset;
extern osMutexId_t os_mutex_tx_data;
extern osTimerId_t os_timer_iot;

#endif /* DEV_BLE_H */


