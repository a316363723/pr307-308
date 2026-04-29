#ifndef LCD_H
#define LCD_H

/*********************
 *      INCLUDE
 **********************/
#include <stdint.h>
#include <stdbool.h>
/*********************
 *      DEFINE
 **********************/

#define USE_ASSERT

#ifndef NULL
#define NULL ((void *)0)
#endif

#ifdef USE_ASSERT
#define assert_param(expr) \
    do                     \
    {                      \
        if (expr)          \
            while (1)      \
                ;          \
    } while (0);
#else
#define assert_param(expr) ((void)0)
#endif

#define RGB565(r, g, b) (((((uint16_t)r) >> 3) << 11) | ((((uint16_t)g) >> 2) << 6) | ((((uint16_t)b) >> 3) << 0))
//#define RGB888(r, g, b)     (((uint32_t)r << 16) | ((uint32_t)g << 8) | (b))

#define CMD_READ_ID 0x10
#define CMD_DISP_ON 0x11
#define CMD_DISP_OFF 0x12

/*********************
 *      TYPEDEF
 **********************/

typedef enum
{
    SPI_DRV_TYPE = 0x00,
    FSMC_DRV_TYPE = 0x01,
} lcd_drv_type_t;

/* 横屏, 用户面向LCD屏幕, 排线在右侧 */
/* 竖屏, 用户面向LCD屏幕, 排线在下方 */
typedef enum
{
    DIR_VER_ORG_RIGHT_TOP = ((0 << 2) | (0 << 1) | (0 << 0)),  /* 竖屏, 从左到右， 从上到下*/
    DIR_VER_ORG_RIGHT_DOWN = ((1 << 2) | (0 << 1) | (0 << 0)), /* 竖屏, 从左到右， 从下到上*/
    DIR_VER_ORG_LEFT_TOP = ((0 << 2) | (1 << 1) | (0 << 0)),   /* 竖屏, 从右到左， 从上到下*/
    DIR_VER_ORG_LEFT_DOWN = ((1 << 2) | (1 << 1) | (0 << 0)),  /* 竖屏, 从右到左， 从下到上*/

    DIR_HOR_ORG_LEFT_TOP = ((0 << 2) | (0 << 1) | (1 << 0)),   /* 横屏, 从左到右， 从上到下*/
    DIR_HOR_ORG_RIGHT_TOP = ((1 << 2) | (0 << 1) | (1 << 0)),  /* 横屏, 从右到左， 从下到上*/
    DIR_HOR_ORG_LEFT_DOWN = ((0 << 2) | (1 << 1) | (1 << 0)),  /* 横屏, 从右到左， 从上到下*/
    DIR_HOR_ORG_RIGHT_DOWN = ((1 << 2) | (1 << 1) | (1 << 0)), /* 横屏, 从右到左， 从下到上*/
} lcd_dir_t;

struct lcd_dev_t;

struct lcd_dev_ops_t
{
    bool (*init)(struct lcd_dev_t *p_dev);
    void (*deinit)(void);
    void (*io_ctrl)(uint8_t cmd, void *param);
	void (*lcd_on)(void);
	void (*lcd_off)(void);
    void (*draw_point)(int16_t x, int16_t y, uint16_t color);
    uint16_t (*read_point)(int16_t x, int16_t y);
    void (*fill_rect)(int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color);
    void (*fill_rect_array)(int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t *color);
};

struct lcd_dev_t
{
    uint32_t id;
    lcd_dir_t dir;
    lcd_drv_type_t drv_type;
    uint16_t width;
    uint16_t height;
    const struct lcd_dev_ops_t *const ops;
    struct lcd_dev_t *next;
};

/*********************
 *  GLOBAL PROTOTYPES
 **********************/

/**
 * @brief LCD 初始化
 * 
 * @return true 
 * @return false 
 */
bool lcd_init(void);

/**
 * @brief 描点
 * 
 * @param x  X坐标
 * @param y  Y坐标
 * @param color 颜色
 */
void lcd_draw_point(int16_t x, int16_t y, uint16_t color);

/**
 * @brief 填充矩形
 * 
 * @param x1 X起始坐标
 * @param y1 Y起始坐标
 * @param x2 X结束坐标
 * @param y2 Y结束坐标
 * @param color 颜色
 */
void lcd_fill_rect(int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color);

/**
 * @brief 填充矩形
 * 
 * @param x1 X起始坐标
 * @param y1 Y起始坐标
 * @param x2 X结束坐标
 * @param y2 Y结束坐标
 * @param color 颜色数组
 */
void lcd_fill_rect_array(int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t *color);

/**
 * @brief 读点
 * 
 * @param x X坐标
 * @param y Y坐标
 * @return uint16_t 颜色
 */
uint16_t lcd_read_point(int16_t x, int16_t y);

/**
 * @brief 绘制水平直线
 * 
 * @param y Y坐标
 * @param start_x X开始坐标
 * @param end_x   X结束坐标
 * @param color   颜色
 */
void lcd_draw_horizon_line(int16_t y, int16_t start_x, int16_t end_x, uint16_t color);

/**
 * @brief 绘制垂直直线
 * 
 * @param x X坐标
 * @param start_y Y开始坐标
 * @param end_y   Y结束坐标
 * @param color   颜色
 */
void lcd_draw_vetical_line(int16_t x, int16_t start_y, int16_t end_y, uint16_t color);

/**
* @brief 打开LCD显示
 * 
 * @param none
 * @param none
 * @param none
 */
void lcd_display_on(void);

/**
* @brief 关闭LCD显示
 * 
 * @param none
 * @param none
 * @param none
 */
void lcd_display_off(void);

#endif // !LCD_H


void lcd_init_reset(void);
