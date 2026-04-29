#ifndef LCD_CONF_H
#define LCD_CONF_H

/* 默认驱动类型 FSMC_DRV_TYPE 和 SPI_DRV_TYPE */
#define DEFAULT_DRV_TYPE FSMC_DRV_TYPE

/* 默认屏幕背景色 */
#define DEFAULT_BACKGROUND_COLOR RGB565(0, 0, 0)

/* 默认开机背光亮度等级0-255 */
#define DEFAULT_BKLIGHT 1

/* LCD屏幕宽度 */
#define LCD_HOR_WIDTH		(320)	

/* LCD屏幕高度 */
#define LCD_VER_HEIGHT		(240)

/* 是否使用GC9307 */
#define USE_GC9307 1

/* 是否使用ST8552 */
#define USE_ST8552 0

#if USE_GC9307
extern struct lcd_dev_t gc9307_dev;
#endif

#if USE_ST8552
extern struct lcd_dev_t st8552_dev;
#endif

#define LCD_DELAY(ms) DDL_DelayMS(ms)

#endif
