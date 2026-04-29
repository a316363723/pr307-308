#ifndef HAL_TIMER_H
#define HAL_TIMER_H
#include "stdint.h"


enum hal_timera_enum
{
    
	HAL_PWM_SELF_ADAPTION_R = 1,
    HAL_PWM_SELF_ADAPTION_G,
    HAL_PWM_SELF_ADAPTION_B,
    HAL_PWM_SELF_ADAPTION_WW1,
    HAL_PWM_SELF_ADAPTION_CW1,
    HAL_PWM_SELF_ADAPTION_WW2,
	HAL_PWM_SELF_ADAPTION_CW2,
    HAL_PWM_FAN,
	HAL_TIMERA_NUM,
};


/*PWM??*/
void hal_timer_pwm_init(enum hal_timera_enum timer_type);
void hal_timer_duty_cycle_set(enum hal_timera_enum timer_type, uint32_t value);
uint32_t hal_timer_duty_cycle_get(enum hal_timera_enum timer_type);

/*??????*/
void hal_timer_capture_init(void (*irq_caputure_cb)(void));
uint32_t hal_fan_capture_val_get(void);

/*???*/
void hal_timer_encoder_cnt_init(void);
int32_t hal_get_encoder_cut(void);
void hal_encoder_tmr_clear(void);

/*1ms ???*/
void hal_tmr_1ms_init(void (*irq_cb)(void));


#endif
