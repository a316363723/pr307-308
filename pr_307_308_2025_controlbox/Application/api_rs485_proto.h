#ifndef API_RS485_PROTO_H
#define API_RS485_PROTO_H
#include "cmsis_os.h"
#include "project_def.h"
#include "rs485_protocol.h"
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "local_data.h"

typedef struct dev_rs485
{   
    int (*write_data)(uint8_t* p_data, uint32_t size, uint32_t timeout);
    int (*read_data)(uint8_t* p_data, uint32_t size, uint32_t timeout);
    uint8_t write_buff[RS485_MESSAGE_MAX_SIZE];
    uint8_t read_buff[RS485_MESSAGE_MAX_SIZE];
    uint16_t message_serial_num;
} dev_rs485_t;

typedef struct
{	
    bool            Connected;  
    bool            power_on;
	uint16_t        time;
    uint32_t        update_crc;
	uint8_t         hs_state;
    dev_rs485_t     rs485;
}dev_lamp_t;


extern dev_lamp_t dev_lamp ;
extern bool rs485_get_cfx_bank_rw(dev_lamp_t* dev_lamp, bool ack_need, uint32_t retry_tick, uint32_t retry_times, uint8_t type);
extern bool rs485_set_motor_angle(dev_lamp_t* dev_lamp, bool ack_need, uint32_t retry_times, uint32_t retry_tick, rs485_motor_type_enum motor_type, float angle);
extern bool rs485_get_mount_access(dev_lamp_t* dev_lamp, bool ack_need, uint32_t retry_tick, uint32_t retry_times, struct sys_info_accessories *arg);
extern void rs485_power_on_trigger_cfx(void);
extern bool rs485_set_motor_reset(dev_lamp_t* dev_lamp, bool ack_need, uint32_t retry_times, uint32_t retry_tick, rs485_motor_type_enum motor_type);
extern int light_update_close(void *arg);
extern bool rs485_send_cfx_data(dev_lamp_t* dev_lamp, void* CfxBody, uint8_t* ack,uint32_t retry_ticks);
extern int light_update_write(uint32_t addr, const uint8_t *data, uint32_t size);
extern int light_update_erase(uint32_t addr, uint32_t size);
extern int light_update_open(const char *file_name);
extern void api_cfx_file_handle(dev_lamp_t* dev_lamp, uint8_t *buffer);
extern  uint8_t api_send_siduspro_fx_file(dev_lamp_t* dev_lamp, uint8_t *arg, rs485_file_tx_step_enum code, uint16_t time);
extern bool rs485_send_upgrade_data(dev_lamp_t* dev_lamp, void* file_tx, uint8_t* ack, uint32_t retry_ticks);
extern bool rs485_get_cfx_name(dev_lamp_t* dev_lamp, bool ack_need, uint32_t retry_tick, uint32_t retry_times, uint8_t type ,uint8_t bank, char* name);
extern uint32_t rs485_get_device_version(dev_lamp_t* dev_lamp, bool ack_need, uint32_t retry_times, uint32_t retry_tick, struct sys_info_lamp* body);
extern bool rs485_get_motor_stall(dev_lamp_t* dev_lamp, bool ack_need, uint32_t retry_times, uint32_t retry_tick, uint8_t *stall_state);
extern bool rs485_read_err_msg(dev_lamp_t* dev_lamp, uint32_t* err_msg);
extern bool rs485_get_motor_angle(dev_lamp_t* dev_lamp, bool ack_need, uint32_t retry_times, uint32_t retry_tick, rs485_motor_type_enum motor_type, float *angle);
extern uint32_t rs485_get_device_version2(dev_lamp_t* dev_lamp, bool ack_need, uint32_t retry_times, uint32_t retry_tick, struct sys_info_lamp* body);
extern bool rs485_get_access(dev_lamp_t* dev_lamp, bool ack_need, uint32_t retry_tick, uint32_t retry_times, struct sys_info_accessories *arg);
extern bool rs485_find_Lamp_mode(dev_lamp_t* dev_lamp, bool ack_need, uint32_t retry_times, uint32_t retry_tick, uint16_t Lamp_state);
extern bool rs485_set_sleep_mode(dev_lamp_t* dev_lamp, bool ack_need, uint32_t retry_times, uint32_t retry_tick, uint8_t sleep_mode);
extern bool rs485_set_dimming_curve(dev_lamp_t* dev_lamp, bool ack_need, uint32_t retry_times, uint32_t retry_tick, uint8_t curve);
extern bool rs485_set_led_frq(dev_lamp_t* dev_lamp, bool ack_need, uint32_t retry_times, uint32_t retry_tick, int16_t frq_value);
extern bool rs485_set_fan_mode(dev_lamp_t* dev_lamp, bool ack_need, uint32_t retry_times, uint32_t retry_tick, uint8_t mode ,uint16_t speed );
extern bool rs485_set_cfx_name(dev_lamp_t* dev_lamp, bool ack_need, uint32_t retry_tick, uint32_t retry_times, uint8_t type ,uint8_t bank, char* name );
extern bool rs485_set_power_mode(dev_lamp_t* dev_lamp, bool ack_need, uint32_t retry_times, uint32_t retry_tick, uint8_t power_mode, uint16_t Ratio_value, uint8_t save_ctr);
extern bool rs485_board_msg_read(dev_lamp_t* dev_lamp, bool ack_need, uint32_t retry_times, uint32_t retry_tick, struct sys_info_lamp* body);
extern int dev_rs485_cmd_read(struct dev_rs485* p_dev, int retry_times, uint32_t retry_ticks, bool ack_en, void* p_read_body, ...);
extern int dev_rs485_cmd_write(struct dev_rs485* p_dev, int retry_times, uint32_t retry_ticks, bool ack_en, void* p_ack, ...);
extern bool rs485_light_control(dev_lamp_t* dev_lamp, bool ack_need, uint32_t retry_times, uint32_t retry_tick, void* p_cmd_body);
extern bool rs485_set_highspeed_mode(dev_lamp_t* dev_lamp, bool ack_need, uint32_t retry_times, uint32_t retry_tick, uint8_t highspeed_mode);
extern uint8_t rs485_self_adaption_read(dev_lamp_t* dev_lamp);
bool rs485_set_motor_demo(dev_lamp_t* dev_lamp, bool ack_need);
bool rs485_get_motor_reset(dev_lamp_t* dev_lamp, bool ack_need, uint32_t retry_times, uint32_t retry_tick, rs485_motor_type_enum motor_type);
bool rs485_set_Industrial_fresnel_angle(dev_lamp_t* dev_lamp, bool ack_need, uint32_t retry_times, uint32_t retry_tick);
#endif

