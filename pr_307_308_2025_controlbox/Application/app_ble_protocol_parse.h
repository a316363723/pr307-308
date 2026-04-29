#ifndef APP_BLE_PROTOCAL_PARSE
#define APP_BLE_PROTOCAL_PARSE

#include <stdbool.h>
#include <stdint.h>
#include "user_bluetooth_proto.h"


#define BT_PACKET_SIZE      (10)


bool ble_protocol_packet_is_correct(const uint8_t* p_data, uint8_t length);
void app_ble_proto_parse(uint8_t *rx_data);
void os_timer_iot_callback(void *argument);
void ble_protocol_data_pack_up(uint8_t *p_data, BT_Body_TypeDef *cmd_body, BT_CommandType_TypeDef cmd_type, uint8_t cmd_operate);
uint8_t get_read_cfx_name_state(void);
uint8_t get_read_cfx_bank_rw_state(void);
void set_read_cfx_name_state(uint8_t state);
void set_read_cfx_bank_rw_state(uint8_t state);
uint8_t get_write_cfx_name_state(void);
void set_write_cfx_name_state(uint8_t state);
void app_ble_write_state_set(uint8_t state);
uint8_t app_ble_write_state_get(void);

#endif  /* APP_BLE_PROTOCAL_PARSE */

