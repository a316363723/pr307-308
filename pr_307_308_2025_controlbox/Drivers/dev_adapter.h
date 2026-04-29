#ifndef DEV_ADAPTER_H
#define DEV_ADAPTER_H
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "cmsis_os.h"


typedef enum
{
    ADAPTER_A_EN ,
    ADAPTER_B_EN,
    ADAPTER_A_AND_B_EN,
    ADAPTER_A_AND_B_DIS,
    
} adapter_choose_enum;

 struct adapter_user_info_t
{
    float   v_output;
    float   i_output;
    float   i_bat;
    float   temp;
    uint8_t   warn;
    uint8_t   type;
    float   i_input;
    float   v_input;
} ;


 struct adapter_output_set_t
 {
	uint8_t voltage_high;
	uint8_t voltage_low;
	uint8_t current_high;
	uint8_t current_low;
	 
 };


void adapter_a_and_b_ctrl(adapter_choose_enum value);
void adapter_init_config(void);
int adapter_switch_ctrl(uint8_t state);
int adapter_read_info(struct adapter_user_info_t *user_body);
int adapter_set_voltage_and_current(struct adapter_output_set_t *data);

extern osMessageQueueId_t os_queue_usart_adapter_rx;
extern osSemaphoreId_t    os_semaphore_adapter_tx;
#endif
