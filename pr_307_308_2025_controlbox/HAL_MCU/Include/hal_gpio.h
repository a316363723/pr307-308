/**
 * @file hal_gpio.h
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
 * </table>
 */
#ifndef HAL_GPIO_H
#define HAL_GPIO_H
#include <stdint.h>
#include <stdbool.h>

#define BIT(x)                       ((uint32_t)((uint32_t)0x01U<<(x)))
#define BITS(start, end)             ((0xFFFFFFFFUL << (start)) & (0xFFFFFFFFUL >> (31U - (uint32_t)(end)))) 
#define GET_BITS(regval, start, end) (((regval) & BITS((start),(end))) >> (start))

#define HAL_PIN_0       BIT(0)
#define HAL_PIN_1       BIT(1)
#define HAL_PIN_2       BIT(2)
#define HAL_PIN_3       BIT(3)
#define HAL_PIN_4       BIT(4)
#define HAL_PIN_5       BIT(5)
#define HAL_PIN_6       BIT(6)
#define HAL_PIN_7       BIT(7)
#define HAL_PIN_8       BIT(8)
#define HAL_PIN_9       BIT(9)
#define HAL_PIN_10      BIT(10)
#define HAL_PIN_11      BIT(11)
#define HAL_PIN_12      BIT(12)
#define HAL_PIN_13      BIT(13)
#define HAL_PIN_14      BIT(14)
#define HAL_PIN_15      BIT(15)
#define HAL_PIN_ALL     BITS(0,15)

enum hal_gpio_port
{
    HAL_GPIOA = 0x00,
    HAL_GPIOB = 0x01,
    HAL_GPIOC = 0x02,
    HAL_GPIOD = 0x03,
    HAL_GPIOE = 0x04,
    HAL_GPIOF = 0x05,
    HAL_GPIOG = 0x06,
    HAL_GPIOH = 0x07,
    HAL_GPIOI = 0x08,
    HAL_GPIOJ = 0x09,
    HAL_GPIOK = 0x0A,
    HAL_GPIOL = 0x0B,
    HAL_GPIOM = 0x0C,
    HAL_GPION = 0x0D,
    HAL_GPIOO = 0x0E,
    HAL_GPIOP = 0x0F,
    HAL_GPIOQ = 0x10,
    HAL_GPIOR = 0x11,
    HAL_GPIOS = 0x12,
    HAL_GPIOT = 0x13,
    HAL_GPIOU = 0x14,
    HAL_GPIOV = 0x15,
    HAL_GPIOW = 0x16,
    HAL_GPIOX = 0x17,
    HAL_GPIOY = 0x18,
    HAL_GPIOZ = 0x19,
};

enum hal_pin_mode
{
    HAL_PIN_MODE_IN_FLOATING = 0x00,
    HAL_PIN_MODE_IPU         = 0x01,
    HAL_PIN_MODE_IPD         = 0x02,
    HAL_PIN_MODE_OUT_OD      = 0x03,
    HAL_PIN_MODE_OUT_PP      = 0x04,
};

enum hal_exirq_trigger
{
    HAL_EXIRQ_TRIGGER_FALLING = 0x00,
    HAL_EXIRQ_TRIGGER_RISING  = 0x01,
    HAL_EXIRQ_TRIGGER_BOTH    = 0x02,
    HAL_EXIRQ_TRIGGER_LOW     = 0x03,
};

void hal_gpio_init(enum hal_gpio_port port,uint32_t pin, enum hal_pin_mode mode);

void hal_gpio_write_pin(enum hal_gpio_port port, uint32_t pin,uint8_t state);

uint32_t hal_gpio_read_pin(enum hal_gpio_port port, uint32_t pin);

void hal_gpio_irq_register(enum hal_gpio_port port, 
                    uint32_t pin, 
                    uint32_t irqn,
                    uint32_t priority,
                    enum hal_exirq_trigger trigger_type,  
                    void(*irq_cb)(void));
#endif

