#ifndef DEV_POWER_H
#define DEV_POWER_H
#include <stdint.h>
#include <stdbool.h>

typedef struct
{
    int (*gpio_init)(void);
    void (*gpio_write)(uint8_t ctrl, bool state);
    bool (*get_power_key)(void);
    void (*power_led_set)(bool state);
    int (*gpio_read)(uint8_t ctrl);
    uint8_t (*power_manage_init)(void);
    uint8_t (*power_boost_up)(bool state);
    void (*analog_dim_ctrl)(bool state);
    void (*pwm_dim_ctrl)(bool state);
    
} dev_power_gpio_t;

extern dev_power_gpio_t dev_power;
uint8_t get_v_sys_pg(void);
void dev_surge_en(bool state);
uint8_t dev_power_manage_init(void);
void dev_analog_green_ctrl(bool state);//testing
uint8_t dev_read_ac_check(void);
void ac_power_falling_count_set(uint8_t count);
uint8_t ac_power_falling_count_get(void);

#endif
