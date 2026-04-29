#include "dev_power.h"
#include "hc32_ddl.h"
#include "stdbool.h"
#include "hal_timer.h"
#include "hal_gpio.h"
#include "perf_counter.h"
#include "hal_iic.h"
#include "cmsis_os.h"

#define SYS_PG_Port                 	HAL_GPIOD
#define SYS_PG_PIN                  	HAL_PIN_7

#define LAN_EN_Port                     HAL_GPIOB
#define LAN_EN_PIN                      HAL_PIN_7
#define LU_AND_BLE_EN_Port              HAL_GPIOB
#define LU_AND_BLE_EN_PIN               HAL_PIN_6
#define SYS_EN_Port                     HAL_GPIOB
#define SYS_EN_PIN                      HAL_PIN_5
#define SURGE_EN_Port                   HAL_GPIOB
#define SURGE_EN_PIN                    HAL_PIN_2
#define SWITCH_WW1_Port                 HAL_GPIOG
#define SWITCH_WW1_PIN                  HAL_PIN_10
#define SWITCH_WW2_Port                 HAL_GPIOD
#define SWITCH_WW2_PIN                  HAL_PIN_2
#define SWITCH_CW1_Port                 HAL_GPIOG
#define SWITCH_CW1_PIN                  HAL_PIN_3
#define SWITCH_CW2_Port                 HAL_GPIOF
#define SWITCH_CW2_PIN                  HAL_PIN_1
#define SWITCH_R_Port                   HAL_GPIOF
#define SWITCH_R_PIN                    HAL_PIN_2
#define SWITCH_B_Port                   HAL_GPIOH
#define SWITCH_B_PIN                    HAL_PIN_11
#define SWITCH_G_Port                   HAL_GPIOG
#define SWITCH_G_PIN                    HAL_PIN_6
#define PWM_G_Port                      HAL_GPIOG
#define PWM_G_PIN                       HAL_PIN_5
#define PWM_B_Port                      HAL_GPIOH
#define PWM_B_PIN                       HAL_PIN_10
#define PWM_R_Port                      HAL_GPIOI
#define PWM_R_PIN                       HAL_PIN_11
#define PWM_CW1_Port                    HAL_GPIOG
#define PWM_CW1_PIN                     HAL_PIN_4
#define PWM_CW2_Port                    HAL_GPIOF
#define PWM_CW2_PIN                     HAL_PIN_0
#define PWM_WW1_Port                    HAL_GPIOG
#define PWM_WW1_PIN                     HAL_PIN_9
#define PWM_WW2_Port                    HAL_GPIOD
#define PWM_WW2_PIN                     HAL_PIN_3
#define I2C_BASE_EN_Port                HAL_GPIOC
#define I2C_BASE_EN_PIN                 HAL_PIN_9
#define POWER_KEY_Port                  HAL_GPIOI
#define POWER_KEY_PIN                   HAL_PIN_10
#define POWER_LED1_Port                 HAL_GPIOE
#define POWER_LED1_PIN                  HAL_PIN_0
#define POWER_LED2_Port                 HAL_GPIOI
#define POWER_LED2_PIN                  HAL_PIN_9


#define ANALOG_SWITCH1_Port             HAL_GPIOH
#define ANALOG_SWITCH1_PIN              HAL_PIN_12
#define ANALOG_G_Port             		HAL_GPIOI
#define ANALOG_G_PIN             		HAL_PIN_7
#define POWER_AC_CHECK_Port             HAL_GPIOI
#define POWER_AC_CHECK_PIN              HAL_PIN_6

#define  POWER_GPIO_WRITE_MUN    23
#define  POWER_GPIO_READ_MUN    3

uint8_t dev_power_manage_init(void);
int power_gpio_init(void);
void power_gpio_write(uint8_t ctrl, bool state);
int power_gpio_read(uint8_t ctrl);
bool dev_power_key_get(void);
void  dev_power_led_set(bool state);
uint8_t dev_power_boost_up(bool state);
void dev_pwm_dimming_ctrl(bool state);
void dev_analog_dimming_ctrl(bool state);

typedef enum  
{
    LU_AND_BLE_SWTICH,
    SYS_EN_SWTICH,
    SURGE_EN_SWTICH,
    CW1_SWITCH,
    CW2_SWITCH,
    WW1_SWITCH,
    WW2_SWITCH,
    R_SWITCH,
    G_SWITCH,
    B_SWITCH,
    PWM_R_SWITCH,
    PWM_G_SWITCH,
    PWM_B_SWITCH,
    PWM_WW1_SWITCH,
    PWM_WW2_SWITCH,
    PWM_CW1_SWITCH,
    PWM_CW2_SWITCH,
    I2C_BASE_SWITCH,
    ANALOG_SWITCH1,  
    POWER_LED1,
    POWER_LED2,
    LAN_EN,
	ANALOG_G_SWITCH,
	ANALOG_AC_CHECK_SWITCH,
}power_gpio_write_type;


typedef enum  
{
    POWER_KEY,
    SYS_PG_SWITCH,
    POWER_AC_CHECK,
}power_gpio_read_type;


dev_power_gpio_t dev_power = {
    .gpio_init = power_gpio_init,
    .gpio_write = power_gpio_write,
    .get_power_key = dev_power_key_get,
    .power_led_set = dev_power_led_set,
    .gpio_read = power_gpio_read,
    .power_manage_init = dev_power_manage_init,
    .power_boost_up = dev_power_boost_up,
    .analog_dim_ctrl = dev_analog_dimming_ctrl,
    .pwm_dim_ctrl = dev_pwm_dimming_ctrl,
};


const uint32_t power_write_gpio[POWER_GPIO_WRITE_MUN][2] = 
{
    {LU_AND_BLE_EN_Port, LU_AND_BLE_EN_PIN},
    {SYS_EN_Port, SYS_EN_PIN},
    {SURGE_EN_Port, SURGE_EN_PIN},
    {SWITCH_CW1_Port, SWITCH_CW1_PIN},
    {SWITCH_CW2_Port, SWITCH_CW2_PIN},
    {SWITCH_WW1_Port, SWITCH_WW1_PIN},
    {SWITCH_WW2_Port, SWITCH_WW2_PIN},
    {SWITCH_R_Port, SWITCH_R_PIN},
    {SWITCH_G_Port, SWITCH_G_PIN},
    {SWITCH_B_Port, SWITCH_B_PIN},
    {PWM_R_Port, PWM_R_PIN},
    {PWM_G_Port, PWM_G_PIN},
    {PWM_B_Port, PWM_B_PIN},
    {PWM_WW1_Port, PWM_WW1_PIN},
    {PWM_WW2_Port, PWM_WW2_PIN},
    {PWM_CW1_Port, PWM_CW1_PIN},
    {PWM_CW2_Port, PWM_CW2_PIN},
    {I2C_BASE_EN_Port,I2C_BASE_EN_PIN},
    {ANALOG_SWITCH1_Port,ANALOG_SWITCH1_PIN},
    {POWER_LED1_Port, POWER_LED1_PIN},
    {POWER_LED2_Port, POWER_LED2_PIN},
    {LAN_EN_Port, LAN_EN_PIN},
	{ANALOG_G_Port, ANALOG_G_PIN},
};

const uint32_t power_read_gpio[POWER_GPIO_READ_MUN][2] = 
{
    {POWER_KEY_Port, POWER_KEY_PIN},
    {SYS_PG_Port, SYS_PG_PIN},
    {POWER_AC_CHECK_Port, POWER_AC_CHECK_PIN},
};

static uint8_t ac_power_falling_count = 0;

void ac_power_falling_count_set(uint8_t count)
{
	ac_power_falling_count = count;
}

uint8_t ac_power_falling_count_get(void)
{
	return ac_power_falling_count;
}

static void ac_power_change_cb(void)
{
	ac_power_falling_count++;
}

int power_gpio_init(void)
{
    for(uint8_t i = 0 ; i < POWER_GPIO_WRITE_MUN  ; i++ )
    {
        hal_gpio_init(power_write_gpio[i][0], power_write_gpio[i][1], HAL_PIN_MODE_OUT_PP);
    }
    hal_gpio_init(SYS_PG_Port, SYS_PG_PIN, HAL_PIN_MODE_IN_FLOATING);
    hal_gpio_init(POWER_KEY_Port, POWER_KEY_PIN , PIN_DIR_IN);
    hal_gpio_init(POWER_AC_CHECK_Port, POWER_AC_CHECK_PIN, HAL_PIN_MODE_IPU);
	
	hal_gpio_init(SURGE_EN_Port,SURGE_EN_PIN, HAL_PIN_MODE_IPU);
    hal_gpio_irq_register(SURGE_EN_Port, SURGE_EN_PIN, 29/*Int028_IRQn*/,5,HAL_EXIRQ_TRIGGER_FALLING, ac_power_change_cb);
}


int power_gpio_read(uint8_t ctrl)
{
    uint8_t res;
    res = hal_gpio_read_pin(power_read_gpio[ctrl][0], power_read_gpio[ctrl][1]);
    return res;
}

void power_gpio_write(uint8_t ctrl, bool state)
{
   hal_gpio_write_pin(power_write_gpio[ctrl][0], power_write_gpio[ctrl][1],state);

}

bool dev_power_key_get(void)
{
    
    dev_power.gpio_read(POWER_KEY);
}

void  dev_power_led_set(bool state)
{
    
    dev_power.gpio_write(POWER_LED1,state);
    dev_power.gpio_write(POWER_LED2,state);
}


void dev_analog_green_ctrl(bool state)//testing
{
    dev_power.gpio_write(ANALOG_G_SWITCH, state);  
}

void dev_pwm_dimming_ctrl(bool state)
{
	if(state == 1)
	{
		dev_power.gpio_write(CW1_SWITCH, true);
		delay_ms(20);
		dev_power.gpio_write(CW2_SWITCH, true);
		delay_ms(20);
		dev_power.gpio_write(WW1_SWITCH, true);
		delay_ms(20);
		dev_power.gpio_write(WW2_SWITCH, true);
		delay_ms(20);
		dev_power.gpio_write(R_SWITCH, true);
		delay_ms(20);
		dev_power.gpio_write(G_SWITCH, true);
		delay_ms(20);
		dev_power.gpio_write(B_SWITCH, true);
		delay_ms(20);
		dev_power.gpio_write(ANALOG_SWITCH1, true);
	}
	else
	{
		dev_power.gpio_write(CW1_SWITCH,false);
		dev_power.gpio_write(CW2_SWITCH, false);
		dev_power.gpio_write(WW1_SWITCH, false);
		dev_power.gpio_write(WW2_SWITCH, false);
		dev_power.gpio_write(R_SWITCH, false);
		dev_power.gpio_write(G_SWITCH, false);
		dev_power.gpio_write(B_SWITCH, false);
	}
}

uint8_t dev_read_ac_check(void)
{
	uint8_t res = 0;
	
	res = hal_gpio_read_pin(POWER_AC_CHECK_Port, POWER_AC_CHECK_PIN);
	
	return res;
}


void dev_analog_dimming_ctrl(bool state)
{
	if(state == 1)
	{
		
		dev_power.gpio_write(ANALOG_SWITCH1, true);
		dev_analog_green_ctrl(true);
	}
	else
	{
		dev_power.gpio_write(ANALOG_SWITCH1, false);
		dev_analog_green_ctrl(false);
		
	}

}


uint8_t dev_power_manage_init(void)
{
	dev_power.gpio_init(); 
	dev_power.analog_dim_ctrl(0);
	dev_power.pwm_dim_ctrl(0);
	dev_analog_green_ctrl(false);
	dev_power.gpio_write(LU_AND_BLE_SWTICH, true);
	dev_power.gpio_write(SYS_EN_SWTICH, true);
	dev_power.gpio_write(SURGE_EN_SWTICH, true);
	dev_power.gpio_write(I2C_BASE_SWITCH, true);
	dev_power.gpio_write(LAN_EN, true);
    return 1;

}


uint8_t dev_power_boost_up(bool state)
{
    static uint16_t pg_time = 0;
	if(state == 0)
	{
		  
		dev_power.gpio_write(PWM_R_SWITCH, false);
		dev_power.gpio_write(PWM_G_SWITCH, false);
		dev_power.gpio_write(PWM_B_SWITCH, false);
		dev_power.gpio_write(PWM_WW1_SWITCH, false);
		dev_power.gpio_write(PWM_CW1_SWITCH, false);
		dev_power.gpio_write(PWM_WW2_SWITCH, false);
		dev_power.gpio_write(PWM_CW2_SWITCH, false);
	
	}
	else
	{
		do{
			pg_time++;
			delay_ms(50);
			if(pg_time > 300)
			{

				pg_time = 0;
				return 0;
			}
		}while((dev_power.gpio_read(SYS_PG_SWITCH) == 0));
		pg_time = 0;
		delay_ms(20);
		dev_power.gpio_write(PWM_R_SWITCH, true);
		delay_ms(15);
		dev_power.gpio_write(PWM_G_SWITCH, true);
		delay_ms(15);
		dev_power.gpio_write(PWM_B_SWITCH, true);
		delay_ms(15);
		dev_power.gpio_write(PWM_WW1_SWITCH, true);
		delay_ms(15);
		dev_power.gpio_write(PWM_CW1_SWITCH, true);
		delay_ms(15);
		dev_power.gpio_write(PWM_WW2_SWITCH, true);
		delay_ms(15);
		dev_power.gpio_write(PWM_CW2_SWITCH, true);
		return 1;
	}
}



