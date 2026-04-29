#ifndef APP_BLE_PORT_H
#define APP_BLE_PORT_H

#include <stdint.h>

enum ctr_light_event
{
	TURN_OFF_EVENT = 0,
	FIND_LIGHT_EVENT,
};

enum ble_reset_status
{
	BLE_RESET_STATE_IDLE = 0,
	BLE_RESET_STATE_START,
	BLE_STATE_RESETINGT,
	BLE_RESET_STATE_SUCCESS,
	BLE_RESET_STATE_FAIL,
};


typedef struct
{
	uint8_t ble_power;
	uint8_t ble_reset;
	uint32_t ble_power_status;
}ble_event_collect;



uint32_t get_ble_reset_status(void);
void set_ble_power(uint8_t flg);
uint32_t get_ble_power(void);
void set_ble_reset_status(enum ble_reset_status status);
void set_ble_reset(uint8_t flg);
uint8_t get_ble_reset(void);
uint32_t get_ble_power_status(void);
void set_ble_power_status(uint8_t stauts);
void set_light_curve(uint8_t data);
uint8_t* get_light_curve(void);
void send_ble_cfx_bank_rw_pack(uint8_t effect_type, uint16_t bank_rw);
void send_ble_read_cfx_name_pack(uint8_t *name);
void send_ble_cfx_ctrl_pack(uint8_t effect_type, uint16_t bank);

void ble_publish_ctr_light_event(uint32_t data);
void ble_publish_power_onoff_event(uint32_t data);
void send_ble_motion_live_pack1(uint16_t pan_angle, uint16_t tilt_angle);
void send_ble_motion_live_pack2(uint8_t angle);
//void ble_publish_factory_reset_event(uint32_t data);
uint8_t get_ble_cfx_file_state(void);
void set_ble_cfx_file_state(uint8_t state);
int16_t ble_const_convert_symbol(int16_t num);
int16_t ble_symbol_convert_const(int16_t num);

#endif  /* APP_BLE_PORT_H */
