#ifndef BSP_POWER_H
#define BSP_POWER_H
#include <stdint.h>
uint8_t get_fan_state(void);
void Power_IO_Init(void);
uint8_t Fan_Power_Check(void);
uint8_t Water_Power_Check(void);
uint8_t Ctrl_48V_Check(void);
uint8_t Set_Water_Enable(uint8_t state);
uint8_t Set_Fan_Enable(uint8_t state);
uint8_t Get_Water_Enable(void);
uint8_t Get_Fan_Enable(void);
uint8_t Set_Adaptive_Or_PWM_Port(uint8_t state);
void star_fan_en(void);
uint8_t get_fan_state(void);
uint8_t Fan_Power_Check(void);
#endif
