#ifndef LCD_BSP_H
#define LCD_BSP_H

#include <stdint.h>
#include <stdbool.h>

/**
 * @brief LCD 屏幕初始化、电源控制、背光引脚
 * 
 */
void bsp_lcd_init(void);

/**
 * @brief LCD SPI接口外设初始化
 * 
 */
void bsp_lcd_spi_init(void);

/**
 * @brief LCD FSMC接口外设初始化
 * 
 */
void bsp_lcd_exmc_init(void);

/**
 * @brief LCD 电源控制
 * 
 * @param flag 
 */
void bsp_lcd_power_ctrl(uint8_t flag);

/**
 * @brief LCD 背光控制
 * 
 * @param light 
 */
void bsp_lcd_set_bklight(uint8_t light);

/**
 * @brief LCD 复位
 * 
 */
void bsp_lcd_reset(void);

/**
 * @brief LCD 写寄存器
 * 
 * @param reg 
 */
void lcd_write_reg(uint16_t reg);

/**
 * @brief LCD 写数据
 * 
 * @param data 
 */
void lcd_write_data(uint16_t data);

/**
 * @brief LCD读取寄存器
 * 
 * @return uint16_t 
 */
uint16_t lcd_read_data(void);

/**
 * @brief LCD延时函数.
 * 
 * @param ms 
 */
void lcd_delay_ms(uint16_t ms);

#endif // !LCD_BSP_H
