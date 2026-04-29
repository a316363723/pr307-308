#ifndef DEV_FAN_H
#define DEV_FAN_H
#include <stdint.h>
#include <stdbool.h>

typedef struct
{
    float proportion;
	float integral;
	float differential;
    int   max_output;
}fan_pid_config_t;

typedef struct
{
    int set_speed;
    int cur_output;
	int i_error;
	int i_incpid;
    int last_error;
    int prev_error;
}fan_pid_state_t;

typedef struct
{
    fan_pid_config_t pid_config;
    fan_pid_state_t  pid_state;
    uint16_t (*real_fan_speed_read)(void);
    void (*fan_pwm_ctrl)(uint16_t);
}dev_fan_t;


extern dev_fan_t fan;

uint16_t dev_fan_error_get(void);
void dev_fan_init(void);
void dev_fan_set_speed(dev_fan_t* dev, uint16_t speed);
void dev_fan_polling_fun(dev_fan_t* dev, uint32_t ticks);
uint16_t dev_fan_revolving_speed_get(void);
uint16_t dev_fan_error_set( uint8_t flag );
uint16_t dev_fan_irq_count_get(void);
uint16_t dev_fan_irq_count_set( uint8_t flag );

#endif
