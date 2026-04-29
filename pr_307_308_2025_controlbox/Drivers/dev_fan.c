#include "dev_fan.h"
#include "hc32_ddl.h"
#include "stdbool.h"
#include "hal_timer.h"
#include "hal_gpio.h"

#define FAN_EN_Port       GPIO_PORT_B
#define FAN_EN_Pin        GPIO_PIN_04
void dev_fan_pwm_set( uint16_t value);
uint16_t dev_fan_revolving_speed_get(void);
static uint8_t fan_err_ticks = 0;
static uint16_t  fan_speed_val;
static uint16_t  fan_irq_count_val;
//static uint16_t   fan_capture_ticks;

#if (PROJECT_TYPE == 307) 
dev_fan_t fan =
{
    .pid_config = {
        .proportion = 2.0,
        .integral = 2.0,
        .differential = 0.1,
        .max_output = 1700
    },
    .pid_state = {
        .set_speed = 600,
        .cur_output = 500,
        .i_error = 0,
        .i_incpid = 0,
        .last_error = 0,
        .prev_error = 0,
    },
    .fan_pwm_ctrl = dev_fan_pwm_set,
    .real_fan_speed_read = dev_fan_revolving_speed_get,
    
};
#elif (PROJECT_TYPE == 308)  
dev_fan_t fan =
{
    .pid_config = {
        .proportion = 2.0,
        .integral = 2.0,
        .differential = 0.1,
        .max_output = 3200
    },
    .pid_state = {
        .set_speed = 600,
        .cur_output = 500,
        .i_error = 0,
        .i_incpid = 0,
        .last_error = 0,
        .prev_error = 0,
    },
    .fan_pwm_ctrl = dev_fan_pwm_set,
    .real_fan_speed_read = dev_fan_revolving_speed_get,
    
};
#endif  

void bsp_fan_en_set(bool state)
{
    
	if( state== 1)
	{
		 GPIO_SetPins(FAN_EN_Port, FAN_EN_Pin);
	}
	else
	{
		GPIO_ResetPins(FAN_EN_Port,FAN_EN_Pin);
	}
	
}


void dev_fan_set_speed(dev_fan_t* dev, uint16_t speed)
{
    if(NULL != dev)
        dev->pid_state.set_speed = speed;
    if(speed == 0)
    {
        bsp_fan_en_set(false);
    }
    else
    {
        bsp_fan_en_set(true);
    }
}


void dev_fan_polling_fun(dev_fan_t* dev, uint32_t ticks)
{
    if(NULL == dev || NULL == dev->real_fan_speed_read || NULL == dev->fan_pwm_ctrl)
        return;

  
    dev->pid_state.i_error = dev->pid_state.set_speed - dev->real_fan_speed_read();

    dev->pid_state.i_incpid = dev->pid_config.proportion * dev->pid_state.i_error
                            - dev->pid_config.integral * dev->pid_state.last_error
                            + dev->pid_config.differential * dev->pid_state.prev_error;
    dev->pid_state.prev_error = dev->pid_state.last_error;
    dev->pid_state.last_error = dev->pid_state.i_error;
    
    dev->pid_state.cur_output += dev->pid_state.i_incpid;
    if(dev->pid_state.cur_output > dev->pid_config.max_output)
    {
        dev->pid_state.cur_output = dev->pid_config.max_output;
    }
    else if(dev->pid_state.cur_output < 500)
    {
        dev->pid_state.cur_output = 500;
    }

    dev->fan_pwm_ctrl(dev->pid_state.cur_output);
}



void dev_fan_pwm_set( uint16_t value)
{
    hal_timer_duty_cycle_set(HAL_PWM_FAN,  value);
}

void power_fan_capture_back(void)
{
    static int32_t cur = 0;
    uint32_t speed = 0;
	static uint8_t count = 0;
	
    cur = hal_fan_capture_val_get();
    speed = (234375.0f) / (float)cur  * 60.0f  / 3;//  240 /1024 *1000000 = 234375HZ
	fan_irq_count_val++;
    if(speed < 10000 && speed > 100)
    {
        fan_speed_val = speed; 
		fan_err_ticks = 1;
		count = 0;
    }
	else
    {
		if(count++ > 5)
		{
			count = 0;
			fan_err_ticks = 0;
		}
    }
}


uint16_t dev_fan_revolving_speed_get(void)
{
    return fan_speed_val ;

}


void dev_fan_init(void)
{
    hal_gpio_init(FAN_EN_Port, FAN_EN_Pin, HAL_PIN_MODE_OUT_PP);
    hal_timer_pwm_init(HAL_PWM_FAN);
    hal_timer_capture_init(power_fan_capture_back);
	bsp_fan_en_set(true);
	dev_fan_pwm_set(500);
}


uint16_t dev_fan_error_get(void)
{
	return fan_err_ticks;
}


uint16_t dev_fan_error_set( uint8_t flag )
{
	fan_err_ticks = flag;
}

uint16_t dev_fan_irq_count_get(void)
{
	return fan_irq_count_val;
}


uint16_t dev_fan_irq_count_set( uint8_t flag )
{
	fan_irq_count_val = flag;
}
