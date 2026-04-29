/**
 * @file hal_gpio.c
 * @author Matthew (matthew.li@aputure.com)
 * @brief 
 * @version 0.1
 * @date 2022-09-09
 * 
 * @copyright Copyright (c) 2022 爱图仕
 * 
 * @par 修改日志:
 * <table>
 * <tr><th> Date         <th>Version  <th>Author    <th>Description
 * <tr><td>2022-09-09     <td>1.0     <td>Matthew   <td>初始创建
 * <tr><td>2022-09-09     <td>1.0     <td>Darwin    <td>填充函数内容
 * </table>
 */

#include "hal_gpio.h"
#include "hc32_ddl.h"
#include "os_event.h"

static uint32_t get_exirq_number(uint32_t pin);
static void gpio0_exint_irq_cb(void);
static void gpio1_exint_irq_cb(void);
static void gpio2_exint_irq_cb(void);
static void gpio3_exint_irq_cb(void);
static void gpio4_exint_irq_cb(void);
static void gpio5_exint_irq_cb(void);
static void gpio6_exint_irq_cb(void);
static void gpio7_exint_irq_cb(void);
static void gpio8_exint_irq_cb(void);
static void gpio9_exint_irq_cb(void);
static void gpio10_exint_irq_cb(void);
static void gpio11_exint_irq_cb(void);
static void gpio12_exint_irq_cb(void);
static void gpio13_exint_irq_cb(void);
static void gpio14_exint_irq_cb(void);
static void gpio15_exint_irq_cb(void);

struct pin_desc {
    uint32_t pin;
    uint32_t ex_irq_num;
};

struct gpio_exint
{
    void (*gpio_func_cb[16])(void);
    void (*gpio_irq_cb[16])(void);
};

struct gpio_exint gpio_exint_func = 
{
    .gpio_irq_cb[0] = gpio0_exint_irq_cb,
    .gpio_irq_cb[1] = gpio1_exint_irq_cb,
    .gpio_irq_cb[2] = gpio2_exint_irq_cb,
    .gpio_irq_cb[3] = gpio3_exint_irq_cb,
    .gpio_irq_cb[4] = gpio4_exint_irq_cb,
    .gpio_irq_cb[5] = gpio5_exint_irq_cb,
    .gpio_irq_cb[6] = gpio6_exint_irq_cb,
    .gpio_irq_cb[7] = gpio7_exint_irq_cb,
    .gpio_irq_cb[8] = gpio8_exint_irq_cb,
    .gpio_irq_cb[9] = gpio9_exint_irq_cb,
    .gpio_irq_cb[10] = gpio10_exint_irq_cb,
    .gpio_irq_cb[11] = gpio11_exint_irq_cb,    
    .gpio_irq_cb[12] = gpio12_exint_irq_cb,
    .gpio_irq_cb[13] = gpio13_exint_irq_cb,
    .gpio_irq_cb[14] = gpio14_exint_irq_cb,
    .gpio_irq_cb[15] = gpio15_exint_irq_cb,

}; 

const struct pin_desc pins[] = {
    {HAL_PIN_0,  INT_PORT_EIRQ0},
    {HAL_PIN_1,  INT_PORT_EIRQ1},
    {HAL_PIN_2,  INT_PORT_EIRQ2},
    {HAL_PIN_3,  INT_PORT_EIRQ3},
    {HAL_PIN_4,  INT_PORT_EIRQ4},
    {HAL_PIN_5,  INT_PORT_EIRQ5},
    {HAL_PIN_6,  INT_PORT_EIRQ6},
    {HAL_PIN_7,  INT_PORT_EIRQ7},
    {HAL_PIN_8,  INT_PORT_EIRQ8},
    {HAL_PIN_9,  INT_PORT_EIRQ9},
    {HAL_PIN_10, INT_PORT_EIRQ10},
    {HAL_PIN_11, INT_PORT_EIRQ11},
    {HAL_PIN_12, INT_PORT_EIRQ12},
    {HAL_PIN_13, INT_PORT_EIRQ13},
    {HAL_PIN_14, INT_PORT_EIRQ14},
    {HAL_PIN_15, INT_PORT_EIRQ15},
};

void hal_gpio_init(enum hal_gpio_port port,uint32_t pin, enum hal_pin_mode mode)
{
    stc_gpio_init_t gpio_struct;

    GPIO_StructInit(&gpio_struct);
    switch (mode)
    {
        case HAL_PIN_MODE_IN_FLOATING:
            gpio_struct.u16PinDir = PIN_DIR_IN;      
            break;
        case HAL_PIN_MODE_IPU:
            gpio_struct.u16PinDir = PIN_DIR_IN;
            gpio_struct.u16PullUp = PIN_PU_ON;  
            gpio_struct.u16ExInt = PIN_EXINT_ON;    //开启中断    
            break;
        case HAL_PIN_MODE_IPD:
            gpio_struct.u16PinDir = PIN_DIR_IN;  
            break;
        case HAL_PIN_MODE_OUT_OD:
            gpio_struct.u16PinDir = PIN_DIR_OUT;
            gpio_struct.u16PullUp = PIN_PU_OFF; 
            gpio_struct.u16PinOType = PIN_OTYPE_NMOS;
            break;        
        case HAL_PIN_MODE_OUT_PP:
            gpio_struct.u16PinDir = PIN_DIR_OUT;
            gpio_struct.u16PinOType = PIN_OTYPE_CMOS;
            break;
        default:
            break;
    }
        
    GPIO_Init((uint8_t)port , pin, &gpio_struct);
}

void hal_gpio_write_pin(enum hal_gpio_port port, uint32_t pin, uint8_t state)
{
    if (state)
    {
        GPIO_SetPins((uint8_t)port, pin);
    }
    else
    {
        GPIO_ResetPins((uint8_t)port, pin);
    }    
}

uint32_t hal_gpio_read_pin(enum hal_gpio_port port, uint32_t pin)
{
    if (GPIO_ReadInputPins((uint8_t)port, pin) == Pin_Reset)
        return 0;
    return 1;
}
  
void hal_gpio_irq_register(enum hal_gpio_port port, 
                    uint32_t pin, 
                    uint32_t irqn,
                    uint32_t priority,
                    enum hal_exirq_trigger trigger_type,  
                    void(*irq_cb)(void))
{
    stc_exint_init_t stcExintInit;
    stc_irq_signin_config_t stcIrqSignConfig;
    uint32_t exint_channel = pin;
    uint32_t trigger_level = trigger_type;
    uint8_t gpio_index = 0;

    /* Exint config */
    (void)EXINT_StructInit(&stcExintInit);
    stcExintInit.u32ExIntCh = exint_channel;
    stcExintInit.u32ExIntLvl= trigger_level;
    (void)EXINT_Init(&stcExintInit);

    /* IRQ sign-in */
    stcIrqSignConfig.enIntSrc = (en_int_src_t)get_exirq_number(pin);
    stcIrqSignConfig.enIRQn   = (IRQn_Type)irqn;
    
    
    gpio_index = get_first_bit_pos(pin);
    gpio_exint_func.gpio_func_cb[gpio_index] = irq_cb;
    
    stcIrqSignConfig.pfnCallback = gpio_exint_func.gpio_irq_cb[gpio_index];
    (void)INTC_IrqSignIn(&stcIrqSignConfig);

    NVIC_ClearPendingIRQ((IRQn_Type)irqn);
    NVIC_SetPriority((IRQn_Type)irqn, priority);    
    NVIC_EnableIRQ((IRQn_Type)irqn);
}


static void gpio0_exint_irq_cb(void)
{    
	if (Set == EXINT_GetExIntSrc(EXINT_CH00))
	{
        gpio_exint_func.gpio_func_cb[0](); 
		EXINT_ClrExIntSrc(EXINT_CH00);
	}
}

static void gpio1_exint_irq_cb(void)
{    
	if (Set == EXINT_GetExIntSrc(EXINT_CH01))
	{
        gpio_exint_func.gpio_func_cb[1](); 
		EXINT_ClrExIntSrc(EXINT_CH01);
	}
}

static void gpio2_exint_irq_cb(void)
{    
	if (Set == EXINT_GetExIntSrc(EXINT_CH02))
	{
        gpio_exint_func.gpio_func_cb[2](); 
		EXINT_ClrExIntSrc(EXINT_CH02);
	}
}

static void gpio3_exint_irq_cb(void)
{    
	if (Set == EXINT_GetExIntSrc(EXINT_CH03))
	{
        gpio_exint_func.gpio_func_cb[3](); 
		EXINT_ClrExIntSrc(EXINT_CH03);
	}
}

static void gpio4_exint_irq_cb(void)
{    
	if (Set == EXINT_GetExIntSrc(EXINT_CH04))
	{
        gpio_exint_func.gpio_func_cb[4](); 
		EXINT_ClrExIntSrc(EXINT_CH04);
	}
}

static void gpio5_exint_irq_cb(void)
{    
	if (Set == EXINT_GetExIntSrc(EXINT_CH05))
	{
        gpio_exint_func.gpio_func_cb[5](); 
		EXINT_ClrExIntSrc(EXINT_CH05);
	}
}

static void gpio6_exint_irq_cb(void)
{    
	if (Set == EXINT_GetExIntSrc(EXINT_CH06))
	{
        gpio_exint_func.gpio_func_cb[6](); 
		EXINT_ClrExIntSrc(EXINT_CH06);
	}
}

static void gpio7_exint_irq_cb(void)
{    
	if (Set == EXINT_GetExIntSrc(EXINT_CH07))
	{
        gpio_exint_func.gpio_func_cb[7](); 
		EXINT_ClrExIntSrc(EXINT_CH07);
	}
}

static void gpio8_exint_irq_cb(void)
{    
	if (Set == EXINT_GetExIntSrc(EXINT_CH08))
	{
        gpio_exint_func.gpio_func_cb[8](); 
		EXINT_ClrExIntSrc(EXINT_CH08);
	}
}

static void gpio9_exint_irq_cb(void)
{    
	if (Set == EXINT_GetExIntSrc(EXINT_CH09))
	{
        gpio_exint_func.gpio_func_cb[9](); 
		EXINT_ClrExIntSrc(EXINT_CH09);
	}
}

static void gpio10_exint_irq_cb(void)
{    
	if (Set == EXINT_GetExIntSrc(EXINT_CH10))
	{
        gpio_exint_func.gpio_func_cb[10](); 
		EXINT_ClrExIntSrc(EXINT_CH10);
	}
}
static void gpio11_exint_irq_cb(void)
{    
	if (Set == EXINT_GetExIntSrc(EXINT_CH11))
	{
        gpio_exint_func.gpio_func_cb[11](); 
		EXINT_ClrExIntSrc(EXINT_CH11);
	}
}
static void gpio12_exint_irq_cb(void)
{    
	if (Set == EXINT_GetExIntSrc(EXINT_CH12))
	{
        gpio_exint_func.gpio_func_cb[12](); 
		EXINT_ClrExIntSrc(EXINT_CH12);
	}
}
static void gpio13_exint_irq_cb(void)
{    
	if (Set == EXINT_GetExIntSrc(EXINT_CH13))
	{
        gpio_exint_func.gpio_func_cb[13](); 
		EXINT_ClrExIntSrc(EXINT_CH13);
	}
}
static void gpio14_exint_irq_cb(void)
{    
	if (Set == EXINT_GetExIntSrc(EXINT_CH14))
	{
        gpio_exint_func.gpio_func_cb[14](); 
		EXINT_ClrExIntSrc(EXINT_CH14);
	}
}

static void gpio15_exint_irq_cb(void)
{    
	if (Set == EXINT_GetExIntSrc(EXINT_CH15))
	{
        gpio_exint_func.gpio_func_cb[15](); 
		EXINT_ClrExIntSrc(EXINT_CH15);
	}
}

static uint32_t get_exirq_number(uint32_t pin)
{
    uint8_t index = 0;
    bool found = false;

    for (index = 0; index < (sizeof(pins) / sizeof(pins[0])); index++)
    {
        if (pins[index].pin == pin)
        {
            found = true;
            break;
        }
    }

    DDL_ASSERT(found);    
    return pins[index].ex_irq_num;
}
