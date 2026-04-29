#include "hc32_ddl.h"
#include "lcd_smc.h"
#include "lcd_bsp.h"

#define LCD_RST_PIN_PORT                (GPIO_PORT_C) 
#define LCD_RST_PIN                     (GPIO_PIN_06)

#define LCD_BACKLIGHT_PIN_PORT          (GPIO_PORT_F) 
#define LCD_BACKLIGHT_PIN               (GPIO_PIN_09)

#define LCD_EN_PIN_PORT                (GPIO_PORT_D)
#define LCD_EN_PIN                     (GPIO_PIN_13)


//#define LCD_RST_PIN_PORT                (GPIO_PORT_G) 
//#define LCD_RST_PIN                     (GPIO_PIN_06)

//#define LCD_BACKLIGHT_PIN_PORT          (GPIO_PORT_I) 
//#define LCD_BACKLIGHT_PIN               (GPIO_PIN_00)

//#define LCD_EN_PIN_PORT                (GPIO_PORT_D)
//#define LCD_EN_PIN                     (GPIO_PIN_13)

/**
 * @brief LCD 屏幕初始化、电源控制、背光引脚
 * 
 */
void bsp_lcd_init(void)
{
    //#error "LCD屏幕初始化、电源控制、背光引脚"
    stc_gpio_init_t stcGpioInit;

    (void)GPIO_StructInit(&stcGpioInit);
    stcGpioInit.u16PinDrv = PIN_DRV_HIGH;
    stcGpioInit.u16PinOType = PIN_OTYPE_CMOS;
    stcGpioInit.u16PinDir = PIN_DIR_OUT;

    /* LCD_RST */
    (void)GPIO_Init(LCD_RST_PIN_PORT, LCD_RST_PIN, &stcGpioInit);
    (void)GPIO_Init(LCD_BACKLIGHT_PIN_PORT, LCD_BACKLIGHT_PIN, &stcGpioInit);
	(void)GPIO_Init(LCD_EN_PIN_PORT, LCD_EN_PIN, &stcGpioInit);

        
    GPIO_ResetPins(LCD_RST_PIN_PORT, LCD_RST_PIN);
    GPIO_ResetPins(LCD_EN_PIN_PORT, LCD_EN_PIN);
    GPIO_ResetPins(LCD_BACKLIGHT_PIN_PORT, LCD_BACKLIGHT_PIN);	
}

/**
 * @brief LCD SPI接口外设初始化
 * 
 */
void bsp_lcd_spi_init(void)
{
    //#error "LCD屏幕SPI接口初始化"
}

/**
 * @brief LCD FSMC接口外设初始化
 * 
 */
void bsp_lcd_exmc_init(void)
{
    //#error "LCD屏幕8080接口初始化"
	bsp_smc_init();
}

/**
 * @brief LCD 电源控制
 * 
 * @param flag 
 */
void bsp_lcd_power_ctrl(uint8_t flag)
{

    /* LCD_RST */
     if (0 == flag)
    {
        GPIO_ResetPins(LCD_EN_PIN_PORT, LCD_EN_PIN);
    }
    else
    {
        GPIO_SetPins(LCD_EN_PIN_PORT, LCD_EN_PIN);
    }
    //#error "电源控制, 1使能, 0失能"
}

/**
 * @brief LCD 背光控制
 * 
 * @param light 
 */
void bsp_lcd_set_bklight(uint8_t light)
{
    //#error "控制LCD背光亮度"
    if (0 == light)
    {
        GPIO_ResetPins(LCD_BACKLIGHT_PIN_PORT, LCD_BACKLIGHT_PIN);
    }
    else
    {
        GPIO_SetPins(LCD_BACKLIGHT_PIN_PORT, LCD_BACKLIGHT_PIN);
    }	
}

/**
 * @brief LCD 复位
 * 
 */
void bsp_lcd_reset(void)
{
    //#error "LCD屏幕复位"
	GPIO_ResetPins(LCD_RST_PIN_PORT, LCD_RST_PIN);
    DDL_DelayMS(10);
    GPIO_SetPins(LCD_RST_PIN_PORT, LCD_RST_PIN);
    DDL_DelayMS(10);
}

/**
 * @brief LCD 写寄存器
 * 
 * @param reg 
 */
inline void lcd_write_reg(uint16_t reg)
{
    LCD->REG = reg;
}

/**
 * @brief LCD 写数据
 * 
 * @param data 
 */
inline void lcd_write_data(uint16_t data)
{
    LCD->RAM = data;
}

/**
 * @brief LCD读取寄存器
 * 
 * @return uint16_t 
 */
uint16_t lcd_read_data(void)
{
	uint16_t  data;
	data = *(volatile uint16_t*)&LCD->RAM;
	return  data;
}

/**
 * @brief LCD延时函数.
 * 
 * @param ms 
 */
void lcd_delay_ms(uint16_t ms)
{
    DDL_DelayMS(ms);
}
