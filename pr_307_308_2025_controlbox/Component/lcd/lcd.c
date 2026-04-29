/*********************
 *      INCLUDE
 **********************/
#include "port/lcd_bsp.h"
#include "app_power.h"
#include "lcd.h"
#include "lcd_conf.h"

/*********************
 *      DEFINES
 *********************/
#define list_for_each_entry(_tmp, _head) for (_tmp = _head; _tmp != NULL; _tmp = _tmp->next)

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void lcd_dev_register(struct lcd_dev_t *p_dev);
/**********************
 *  STATIC VARIABLES
 **********************/
static struct lcd_dev_t dev_head;
static struct lcd_dev_t *p_cur_dev = NULL;
/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * @brief 初始化LCD控制器
 * 
 * @return true 
 * @return false 
 */
bool lcd_init(void)
{
    struct lcd_dev_t *p_tmp = NULL;
    uint32_t lcd_id;

    /* 根据通讯类型初始化进行MCU外设初始化 */
    bsp_lcd_init();
    
#if FSMC_DRV_TYPE == DEFAULT_DRV_TYPE
    bsp_lcd_exmc_init();
#elif SPI_DRV_TYPE == DEFAULT_DRV_TYPE
    bsp_lcd_spi_init();
#else 
    assert_param(1);
#endif

#if USE_GC9307
    lcd_dev_register(&gc9307_dev);
#endif

#if USE_ST8552
    lcd_dev_register(&st8552_dev);
#endif

    /* LCD打开电源并复位 */
    bsp_lcd_power_ctrl(1);
    bsp_lcd_reset();
	
    /* 根据通讯类型查找LCD设备 */
    list_for_each_entry(p_tmp, &dev_head)
    {
        if (p_tmp != NULL && p_tmp->drv_type == DEFAULT_DRV_TYPE)
        {
            p_tmp->ops->io_ctrl(CMD_READ_ID, &lcd_id);
            if (lcd_id == p_tmp->id)
            {
                p_cur_dev = p_tmp;
                break;
            }
			else
			{
				while(1)
				{
					power_led_flicker();
				}
			}
        }
    }

    /* LCD屏幕寄存器初始化/初始化 */

    assert_param(p_cur_dev == NULL);
    p_cur_dev->ops->init(p_cur_dev);    
	p_cur_dev->ops->fill_rect(0, 0, p_cur_dev->width, p_cur_dev->height, DEFAULT_BACKGROUND_COLOR);
   
    return true;
}

void lcd_init_reset(void)
{
	assert_param(p_cur_dev == NULL);
    p_cur_dev->ops->init(p_cur_dev); 

}
/**
 * @brief 描点函数
 * 
 * @param x X坐标
 * @param y Y坐标
 * @param color 颜色
 */
void lcd_draw_point(int16_t x, int16_t y, uint16_t color)
{
    p_cur_dev->ops->draw_point(x, y, color);
}

/**
* @brief 打开LCD显示
 * 
 * @param none
 * @param none
 * @param none
 */
void lcd_display_on(void)
{
    p_cur_dev->ops->lcd_on();
}

/**
* @brief 关闭LCD显示
 * 
 * @param none
 * @param none
 * @param none
 */
void lcd_display_off(void)
{
    p_cur_dev->ops->lcd_off();
}

/**
 * @brief 填充矩形
 * 
 * @param x1 起始X坐标
 * @param y1 起始Y坐标
 * @param x2 结束X坐标
 * @param y2 结束Y坐标
 * @param color 颜色
 */
void lcd_fill_rect(int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color)
{
    p_cur_dev->ops->fill_rect(x1, y1, x2, y2, color);
}

/**
 * @brief 填充矩形
 * 
 * @param x1 起始X坐标
 * @param y1 起始Y坐标
 * @param x2 结束X坐标
 * @param y2 结束Y坐标
 * @param color 颜色数组
 */
void lcd_fill_rect_array(int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t *color)
{
    p_cur_dev->ops->fill_rect_array(x1, y1, x2, y2, color);
}

/**
 * @brief 读取坐标的点
 * 
 * @param x X坐标
 * @param y Y坐标
 * @return uint16_t 颜色
 */
uint16_t lcd_read_point(int16_t x, int16_t y)
{
    return p_cur_dev->ops->read_point(x, y);
}

/**
 * @brief 绘制水平直线
 * 
 * @param y Y坐标
 * @param start_x 起始X坐标 
 * @param end_x  结束X坐标
 * @param color  颜色
 */
void lcd_draw_horizon_line(int16_t y, int16_t start_x, int16_t end_x, uint16_t color)
{
    p_cur_dev->ops->fill_rect(start_x, y, end_x, y, color);
}

/**
 * @brief 绘制垂直直线
 * 
 * @param x X坐标
 * @param start_y 起始Y坐标 
 * @param end_y 结束Y坐标
 * @param color 颜色
 */
void lcd_draw_vetical_line(int16_t x, int16_t start_y, int16_t end_y, uint16_t color)
{
    p_cur_dev->ops->fill_rect(x, start_y, x, end_y, color);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * @brief 注册LCD设备
 *
 * @param p_dev
 */
static void lcd_dev_register(struct lcd_dev_t *p_dev)
{
    struct lcd_dev_t *p_tmp = &dev_head;

    while (p_tmp->next != NULL)
    {
        p_tmp = p_tmp->next;
    }

    p_tmp->next = p_dev;
    p_dev->next = NULL;
}
