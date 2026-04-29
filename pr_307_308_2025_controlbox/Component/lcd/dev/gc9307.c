/*********************
 *      INCLUDE
 **********************/
#include "port/lcd_bsp.h"

#include "lcd.h"
#include "lcd_conf.h"

/*********************
 *      DEFINES
 *********************/
#define gc9307_read_data() lcd_read_data()
#define gc9307_write_cmd(cmd) lcd_write_reg(cmd)
#define gc9307_write_data(data) lcd_write_data(data)
#define gc9307_write_data_array(p_data, size)                 \
    do                                                        \
    {                                                         \
        for (uint32_t cnt = 0; cnt < size; cnt++, (p_data)++) \
            gc9307_write_data(*(p_data));                     \
    } while (0)

#define GC9307_ID (0x9307)
#define GC9307_HOR_RES (320)
#define GC9307_VER_RES (240)

#define COLUMN_ADDRESS_SET_CMD 0x2A
#define ROW_ADDRESS_SET_CMD 0x2B
#define MEMORY_WRITE_CMD 0x2C

/**********************
 *  STATIC PROTOTYPES
 **********************/
static bool gc9307_init(struct lcd_dev_t *p_dev);
static void gc9307_draw_point(int16_t x, int16_t y, uint16_t color);
static uint16_t gc9307_read_point(int16_t x, int16_t y);
static void gc9307_io_ctrl(uint8_t cmd, void *param);
static void gc9307_fill_rect(int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color);
static void gc9307_fill_rect_array(int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t *color);
static void gc9307_set_mem_frame(int16_t start_x, int16_t end_x, int16_t start_y, int16_t end_y);
static void gc9307_display_off(void);
static void gc9307_display_on(void);
/**********************
 *  STATIC VARIABLES
 **********************/
struct lcd_dev_ops_t gc9307_dev_ops = {
    .init = gc9307_init,
    .io_ctrl = gc9307_io_ctrl,
	.lcd_on = gc9307_display_on,
	.lcd_off = gc9307_display_off,
    .draw_point = gc9307_draw_point,
    .read_point = gc9307_read_point,
    .fill_rect = gc9307_fill_rect,
    .fill_rect_array = gc9307_fill_rect_array,
};

/**********************
 *  GLOBAL VATIABLES
 **********************/
struct lcd_dev_t gc9307_dev = {
    .id = GC9307_ID,
    .drv_type = FSMC_DRV_TYPE,
    .dir = DIR_HOR_ORG_LEFT_TOP,
    .width = GC9307_HOR_RES,
    .height = GC9307_VER_RES,
    .ops = &gc9307_dev_ops,
    .next = NULL};

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * @brief lcd初始化
 *
 */
static bool gc9307_init(struct lcd_dev_t *p_dev)
{
    uint8_t data = 0;

    gc9307_write_cmd(0xfe);
    gc9307_write_cmd(0xef);

    gc9307_write_cmd(0x36); // set the model of scanning
    switch (p_dev->dir)
    {
    case DIR_VER_ORG_LEFT_DOWN:
        data = DIR_VER_ORG_LEFT_DOWN << 5 | 0x08;
        p_dev->height = GC9307_HOR_RES;
        p_dev->width = GC9307_VER_RES;
        break;
    case DIR_VER_ORG_LEFT_TOP:
        data = DIR_VER_ORG_LEFT_TOP << 5 | 0x08;
        p_dev->height = GC9307_HOR_RES;
        p_dev->width = GC9307_VER_RES;
        break;
    case DIR_VER_ORG_RIGHT_DOWN:
        data = DIR_VER_ORG_RIGHT_DOWN << 5 | 0x08;
        p_dev->height = GC9307_HOR_RES;
        p_dev->width = GC9307_VER_RES;
        break;
    case DIR_VER_ORG_RIGHT_TOP:
        data = DIR_VER_ORG_RIGHT_TOP << 5 | 0x08;
        p_dev->height = GC9307_HOR_RES;
        p_dev->width = GC9307_VER_RES;
        break;
    case DIR_HOR_ORG_LEFT_TOP:
        data = DIR_HOR_ORG_LEFT_TOP << 5 | 0x08;
        p_dev->height = GC9307_VER_RES;
        p_dev->width = GC9307_HOR_RES;
        break;
    case DIR_HOR_ORG_LEFT_DOWN:
        data = DIR_HOR_ORG_LEFT_DOWN << 5 | 0x08;
        p_dev->height = GC9307_VER_RES;
        p_dev->width = GC9307_HOR_RES;
        break;
    case DIR_HOR_ORG_RIGHT_TOP:
        data = DIR_HOR_ORG_RIGHT_TOP << 5 | 0x08;
        p_dev->height = GC9307_VER_RES;
        p_dev->width = GC9307_HOR_RES;
        break;
    case DIR_HOR_ORG_RIGHT_DOWN:
        data = DIR_HOR_ORG_RIGHT_DOWN << 5 | 0x08;
        p_dev->height = GC9307_VER_RES;
        p_dev->width = GC9307_HOR_RES;
        break;
    default:
        break;
    }

    gc9307_write_data(data); // 0x48 竖屏参数  0xE8 横屏参数
    gc9307_write_cmd(0x3a);
    gc9307_write_data(0x05);

    gc9307_write_cmd(0x86);
    gc9307_write_data(0x98);
    gc9307_write_cmd(0x88);
    gc9307_write_data(0x02);

    gc9307_write_cmd(0x89);
    gc9307_write_data(0x03);
    gc9307_write_cmd(0x8b);
    gc9307_write_data(0x80);
    gc9307_write_cmd(0x8d);
    gc9307_write_data(0x3B);
    gc9307_write_cmd(0x8e);
    gc9307_write_data(0x0f);

    gc9307_write_cmd(0xe8);
    gc9307_write_data(0x12);
    gc9307_write_data(0x00);

    gc9307_write_cmd(0xc3);
    gc9307_write_data(0x08);
    gc9307_write_cmd(0xc4);
    gc9307_write_data(0x15);
    gc9307_write_cmd(0xc9);
    gc9307_write_data(0x3c);

    gc9307_write_cmd(0xff);
    gc9307_write_data(0x62);

    gc9307_write_cmd(0x99);
    gc9307_write_data(0x3e);
    gc9307_write_cmd(0x9d);
    gc9307_write_data(0x4b);

    gc9307_write_cmd(0x98);
    gc9307_write_data(0x3e);

    gc9307_write_cmd(0x9C);
    gc9307_write_data(0x4B);
    gc9307_write_cmd(0xF0);
    gc9307_write_data(0x13);
    gc9307_write_data(0x15);
    gc9307_write_data(0x0a);
    gc9307_write_data(0x07);
    gc9307_write_data(0x05);
    gc9307_write_data(0x34);

    gc9307_write_cmd(0xF2);
    gc9307_write_data(0x12);
    gc9307_write_data(0x14);
    gc9307_write_data(0x0a);
    gc9307_write_data(0x07);
    gc9307_write_data(0x04);
    gc9307_write_data(0x35);

    gc9307_write_cmd(0xF1);
    gc9307_write_data(0x4a);
    gc9307_write_data(0x8f);
    gc9307_write_data(0x8f);
    gc9307_write_data(0x2b);
    gc9307_write_data(0x31);
    gc9307_write_data(0x7f);

    gc9307_write_cmd(0xF3);
    gc9307_write_data(0x4a);
    gc9307_write_data(0x8f);
    gc9307_write_data(0x8f);
    gc9307_write_data(0x2b);
    gc9307_write_data(0x31);
    gc9307_write_data(0x7F);

    gc9307_write_cmd(0xE1);
    gc9307_write_data(0x10);
    gc9307_write_data(0x10);

    gc9307_write_cmd(0x11);
//    lcd_delay_ms(120);
    gc9307_write_cmd(0x29);
    gc9307_write_cmd(0x2c);

    return true;
}

/**
 * @brief 打点函数
 *
 * @param x1
 * @param y1
 * @param color
 */
static void gc9307_draw_point(int16_t x, int16_t y, uint16_t color)
{
    gc9307_set_mem_frame(x, x, y, y);
    gc9307_write_cmd(MEMORY_WRITE_CMD);
    gc9307_write_data(color);
}

/**
* @brief 读点函数,不可以使用, 因为没有读点命令
 *
 * @param x
 * @param y
 * @return uint16_t
 */
static uint16_t gc9307_read_point(int16_t x, int16_t y)
{
	gc9307_set_mem_frame(x, x, y, y);
	gc9307_write_cmd(0x2E);
    return gc9307_read_data();
}

/**
* @brief 关闭显示
 *
 * @param x
 * @param y
 * @return uint16_t
 */
static void gc9307_display_off(void)
{
	gc9307_write_cmd(0x28);
}

/**
* @brief 打开显示
 *
 * @param x
 * @param y
 * @return uint16_t
 */
static void gc9307_display_on(void)
{
	gc9307_write_cmd(0x29);
}

/**
 * @brief 控制命令
 *
 * @param cmd
 * @param param
 */
static void gc9307_io_ctrl(uint8_t cmd, void *param)
{
    switch (cmd)
    {
    case CMD_READ_ID:
    {
        uint8_t read_cnt = 2;
        uint32_t *id = param;
        do
        {
            gc9307_write_cmd(0x00);
            gc9307_write_data(0x01);
            lcd_delay_ms(50);
            gc9307_write_cmd(0x00);
            *id = gc9307_read_data(); //假读
            *id = gc9307_read_data();
            *id <<= 8;
            lcd_delay_ms(2);
            gc9307_write_cmd(0xDB);
            *id |= gc9307_read_data(); //假读
            *id |= gc9307_read_data();
            *id <<= 8;
            lcd_delay_ms(2);
            gc9307_write_cmd(0xDC);
            *id |= gc9307_read_data(); //假读
            *id |= gc9307_read_data();
            lcd_delay_ms(2);
        } while (read_cnt-- > 0);
    }
    break;
    }
}

/**
 * @brief 填充矩形
 *
 * @param p_rect
 * @param color
 */
static void gc9307_fill_rect(int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color)
{
    if (x2 < x1 || y2 < y1)
        return;

    gc9307_set_mem_frame(x1, x2, y1, y2);
    gc9307_write_cmd(MEMORY_WRITE_CMD);
    for (int16_t i = y1; i <= y2; i++)
        for (int16_t j = x1; j <= x2; j++)
        {
            gc9307_write_data(color);
        }
}

/**
 * @brief 使用颜色填充矩形
 *
 * @param p_rect
 * @param color
 */
static void gc9307_fill_rect_array(int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t *color)
{
    if (x2 < x1 || y2 < y1)
        return;

    gc9307_set_mem_frame(x1, x2, y1, y2);
    gc9307_write_cmd(MEMORY_WRITE_CMD);
    for (int16_t i = y1; i <= y2; i++)
        for (int16_t j = x1; j <= x2; j++)
        {
            gc9307_write_data(*color);
            color++;
        }
}

/**
 * @brief 设置写入的屏幕内存范围, 闭区间, [start_x, end_x] [start_y, end_y]
 *
 * @param start_x
 * @param end_x
 * @param start_y
 * @param end_y
 */
static void gc9307_set_mem_frame(int16_t start_x, int16_t end_x, int16_t start_y, int16_t end_y)
{
    uint16_t tmp[4];
    uint16_t *p_data;
		
    //设置列地址开始地址,结束地址
    p_data = &tmp[0];
    tmp[0] = start_x >> 8;
    tmp[1] = start_x & 0x00FF;
    tmp[2] = end_x >> 8;
    tmp[3] = end_x & 0x00FF;
    gc9307_write_cmd(COLUMN_ADDRESS_SET_CMD);
    gc9307_write_data_array(p_data, 4);

    //设置行开始地址，结束地址
    p_data = &tmp[0];
    tmp[0] = start_y >> 8;
    tmp[1] = start_y & 0x00FF;
    tmp[2] = end_y >> 8;
    tmp[3] = end_y& 0x00FF;
    gc9307_write_cmd(ROW_ADDRESS_SET_CMD);
    gc9307_write_data_array(p_data, 4);
}
