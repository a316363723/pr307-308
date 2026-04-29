#ifndef APP_LIGHT_H
#define APP_LIGHT_H
#include "cmsis_os.h"
#include "project_def.h"

#include "rs485_protocol.h"
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

enum{
	LIGHT_RS485_OUTPUT_MODE_FLAG = (1<<0),       
	LIGHT_RS485_CURVE_TYPE_FLAG = (1<<1),      
	LIGHT_RS485_FAN_MODE_FLAGE = (1<<2),       
	LIGHT_RS485_FRQ_FLAG = (1<<3),          
    LIGHT_RS485_YOKE_PAN_FLAG = (1<<4),       
    LIGHT_RS485_YOKE_TILT_FLAG = (1<<5),      
    LIGHT_RS485_FRESNEL_ANGLE_FLAG = (1<<6),  	
	LIGHT_RS485_HIGHT_MODE_FLAG = (1<<7),  
};

typedef struct dmx_loss_state_type{
    uint8_t dmx_fade_time;
    uint16_t dmx_fade_ms;
    uint8_t dmx_fade_s;
    uint16_t dmx_hold_ms;
    uint8_t dmx_hold_s;
    float dmx_fade_slope;
    uint8_t dmx_loss_mode;
    uint8_t dmx_loss_flag;
}dmx_loss_state;

extern osThreadId_t app_dac_fade_task_id;
extern const osThreadAttr_t g_light_thread_attr;
extern osThreadId_t app_light_task_id;
extern const osThreadAttr_t g_dac_fade_thread_attr;
extern void app_light_thread_entrance(void *argument);
extern void app_dac_fade_thread(void *argument);
extern void light_rs485_link_set(bool link);
extern void analog_dim_atuo_boot_restart(void);
uint8_t get_lamp_connet_state(void);
uint8_t *get_light_data_pack(void);
void app_light_flags_set(uint32_t flags);
uint32_t app_light_flags_get(uint32_t flags);
#endif

