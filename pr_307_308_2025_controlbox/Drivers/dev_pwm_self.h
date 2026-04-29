#ifndef DEV_PWM_SELF_H
#define DEV_PWM_SELF_H
#include "stdint.h"

typedef struct
{
    uint16_t  r;
    uint16_t  g;
    uint16_t  b;	
    uint16_t  ww1;
    uint16_t  cw1;
    uint16_t  ww2;
    uint16_t  cw2;
}adapt_pwm_dim_t;

typedef struct  
{
    adapt_pwm_dim_t pwm_start;	
    adapt_pwm_dim_t pwm_current;
    adapt_pwm_dim_t pwm_end;
	adapt_pwm_dim_t pwm_base;
	adapt_pwm_dim_t pwm_limit;
    uint32_t run_time;
    uint8_t  fade_start;
	uint32_t fade_time;
}adapt_change_t;

typedef __packed struct{ 
	uint16_t    led_pwm;  
	uint16_t 	p_volt;
}adapt_Coarse_t;

typedef struct
{
	uint16_t led_base;
    uint16_t timer_period;
	uint16_t led_max_volt;
	uint16_t led_min_volt;
	uint16_t max_step;
	uint16_t min_step;
	uint8_t  led_num;
}adapt_config_t;

typedef struct
{	
	uint16_t length;
    adapt_config_t config;
	adapt_change_t adapt_fade;	
	const adapt_Coarse_t (*data)[10];
	void (*adapt_pwm_ctrl)(uint8_t timer_type, uint32_t value);
}dev_pwm_adapt_t;

extern dev_pwm_adapt_t pwm_adapt_80k;
extern dev_pwm_adapt_t pwm_adapt_20k;
/**
 * @brief  
 *         the SystemCoreClock variable.
 * @param  None
 * @retval None
 */
extern void dev_adapt_pwm_1ms_back(dev_pwm_adapt_t* dev);

extern void dev_adapt_fade_restart(dev_pwm_adapt_t *dev);
extern void dev_adapt_pwm_limit_set(dev_pwm_adapt_t* dev, float whole_time);
extern void dev_adapt_pwm_regulate(dev_pwm_adapt_t* dev,uint8_t type, uint16_t volt,uint16_t pwm );
#endif