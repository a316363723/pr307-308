/****************
 *   INCLUDE
 ****************/
#include "hc32_ddl.h"

#include "lcd_smc.h"

/*********************
 *      DEFINES
 *********************/
#define LCD_CS                          (EXMC_SMC_CHIP_3)

#define LCD_CS_PORT                     (GPIO_PORT_G)   /* PG12 - EXMC_CE3 */
#define LCD_CS_PIN                      (GPIO_PIN_12)

#define LCD_WE_PORT                     (GPIO_PORT_C)   /* PC00 - EXMC_WE */
#define LCD_WE_PIN                      (GPIO_PIN_00)

#define LCD_OE_PORT                     (GPIO_PORT_D)   /* PF11 - EXMC_OE */
#define LCD_OE_PIN                      (GPIO_PIN_04)

//#define LCD_OE_PORT                     (GPIO_PORT_F)   /* PF11 - EXMC_OE */
//#define LCD_OE_PIN                      (GPIO_PIN_11)

#define LCD_RS_PORT                     (GPIO_PORT_G)   /* PG02 - EXMC_ADD12 for LCD_RS */
#define LCD_RS_PIN                      (GPIO_PIN_02)

#define LCD_DATA0_PORT                  (GPIO_PORT_D)   /* PD14 - EXMC_DATA0 */
#define LCD_DATA0_PIN                   (GPIO_PIN_14)
#define LCD_DATA1_PORT                  (GPIO_PORT_D)   /* PD15 - EXMC_DATA1 */
#define LCD_DATA1_PIN                   (GPIO_PIN_15)
#define LCD_DATA2_PORT                  (GPIO_PORT_D)   /* PD00 - EXMC_DATA2 */
#define LCD_DATA2_PIN                   (GPIO_PIN_00)
#define LCD_DATA3_PORT                  (GPIO_PORT_D)   /* PD01 - EXMC_DATA3 */
#define LCD_DATA3_PIN                   (GPIO_PIN_01)
#define LCD_DATA4_PORT                  (GPIO_PORT_E)   /* PE07 - EXMC_DATA4 */
#define LCD_DATA4_PIN                   (GPIO_PIN_07)
#define LCD_DATA5_PORT                  (GPIO_PORT_E)   /* PE08 - EXMC_DATA5 */
#define LCD_DATA5_PIN                   (GPIO_PIN_08)
#define LCD_DATA6_PORT                  (GPIO_PORT_E)   /* PE09 - EXMC_DATA6 */
#define LCD_DATA6_PIN                   (GPIO_PIN_09)
#define LCD_DATA7_PORT                  (GPIO_PORT_E)   /* PE10 - EXMC_DATA7 */
#define LCD_DATA7_PIN                   (GPIO_PIN_10)
#define LCD_DATA8_PORT                  (GPIO_PORT_E)   /* PE11 - EXMC_DATA8 */
#define LCD_DATA8_PIN                   (GPIO_PIN_11)
#define LCD_DATA9_PORT                  (GPIO_PORT_E)   /* PE12 - EXMC_DATA9 */
#define LCD_DATA9_PIN                   (GPIO_PIN_12)
#define LCD_DATA10_PORT                 (GPIO_PORT_E)   /* PE13 - EXMC_DATA10 */
#define LCD_DATA10_PIN                  (GPIO_PIN_13)
#define LCD_DATA11_PORT                 (GPIO_PORT_E)   /* PE14 - EXMC_DATA11 */
#define LCD_DATA11_PIN                  (GPIO_PIN_14)
#define LCD_DATA12_PORT                 (GPIO_PORT_E)   /* PE15 - EXMC_DATA12 */
#define LCD_DATA12_PIN                  (GPIO_PIN_15)
#define LCD_DATA13_PORT                 (GPIO_PORT_D)   /* PD08 - EXMC_DATA13 */
#define LCD_DATA13_PIN                  (GPIO_PIN_08)
#define LCD_DATA14_PORT                 (GPIO_PORT_D)   /* PD09 - EXMC_DATA14 */
#define LCD_DATA14_PIN                  (GPIO_PIN_09)
#define LCD_DATA15_PORT                 (GPIO_PORT_D)   /* PD10 - EXMC_DATA15 */
#define LCD_DATA15_PIN                  (GPIO_PIN_10)


/* Use EXMC CS3, A12 as the RS signal */
//#define LCD_BASE                        (0x60000000UL | ((1UL << 13U) - 2UL))

/********************* 
 *     TYPEDEF
**********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void smc_port_init(void);
/**********************
 *  STATIC VARIABLES
 **********************/
static bool b_initialed = false;

/*********************
 *  GLOBAL VATIABLES
 *********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * @brief SMC 外设初始化
 * 
 * @return true 
 * @return false 
 */
bool bsp_smc_init(void)
{
    en_result_t enRet;
    stc_exmc_smc_init_t stcSmcInit;

    if (b_initialed)    
        return true;

    smc_port_init();

    /* Enable SMC module clk */
    PWC_Fcg3PeriphClockCmd(PWC_FCG3_SMC, Enable);

    /* Enable SMC. */
    EXMC_SMC_Cmd(Enable);

    EXMC_SMC_ExitLowPower();
    while (EXMC_SMC_READY != EXMC_SMC_GetStatus())
    {}

    (void)EXMC_SMC_StructInit(&stcSmcInit);

    /*0x6000_0000 ~ 0x60FFFFFF, 16Mb area */
    stcSmcInit.stcChipCfg.u32AddressMatch = 0x60UL;
    stcSmcInit.stcChipCfg.u32AddressMask  = 0xFFUL;
    stcSmcInit.stcChipCfg.u32SmcMemWidth  = EXMC_SMC_MEMORY_WIDTH_16BIT;
    stcSmcInit.stcChipCfg.u32BAA  = EXMC_SMC_BAA_PORT_DISABLE;
    stcSmcInit.stcChipCfg.u32ADV  = EXMC_SMC_ADV_PORT_DISABLE;
    stcSmcInit.stcChipCfg.u32BLS  = EXMC_SMC_BLS_SYNC_CS;
    stcSmcInit.stcChipCfg.u32ReadBurstLen  = EXMC_SMC_MEM_READ_BURST_4;
    stcSmcInit.stcChipCfg.u32WriteBurstLen  = EXMC_SMC_MEM_WRITE_BURST_4;
    stcSmcInit.stcChipCfg.u32ReadMode  = EXMC_SMC_MEM_READ_SYNC;
    stcSmcInit.stcChipCfg.u32WriteMode  = EXMC_SMC_MEM_WRITE_SYNC;

    stcSmcInit.stcTimingCfg.u32RC = 6UL;
    stcSmcInit.stcTimingCfg.u32WC = 4UL;
    stcSmcInit.stcTimingCfg.u32CEOE = 1UL;
    stcSmcInit.stcTimingCfg.u32WP = 2UL;
    stcSmcInit.stcTimingCfg.u32PC = 4UL;
    stcSmcInit.stcTimingCfg.u32TR = 1UL;

    (void)EXMC_SMC_Init(LCD_CS, &stcSmcInit);

    /* Set command: updateregs */
    EXMC_SMC_SetCommand(LCD_CS, EXMC_SMC_CMD_UPDATEREGS, 0UL, 0UL);

    /* Check timing status */
    do {
        enRet = EXMC_SMC_CheckTimingStatus(LCD_CS, &stcSmcInit.stcTimingCfg);
    } while (Ok != enRet);

    /* Check chip status */
    do {
        enRet = EXMC_SMC_CheckChipStatus(LCD_CS, &stcSmcInit.stcChipCfg);
    } while (Ok != enRet);

    if (enRet == Ok)
        b_initialed = true;

    return enRet == Ok ? true : false;
}

///**
// * @brief 读取屏幕ID
// * 
// * @return uint32_t 
// */
//uint32_t lcd_read_id(void)
//{
//    uint32_t id;
//    uint8_t read_cnt = 2;

//    do {
//    
//        lcd_write_reg(0x00);
//        lcd_write_data(0x01);
//		DDL_DelayMS(50);
//		lcd_write_reg(0x00);
//        id = lcd_read_data(); //假读
//        id = lcd_read_data();
//        id <<= 8;
//        DDL_DelayMS(2);
//        lcd_write_reg(0xDB);
//        id |= lcd_read_data(); //假读
//        id |= lcd_read_data();    
//        id <<= 8;
//        DDL_DelayMS(2);
//        lcd_write_reg(0xDC);
//        id |= lcd_read_data(); //假读
//        id |= lcd_read_data();       
//		DDL_DelayMS(2);
//    }while (read_cnt-- > 0);

//    return id;
//}

///**
// * @brief SMC读数据
// * 
// * @return uint16_t 
// */
//uint16_t lcd_read_data(void)
//{
//	uint16_t  data;
//	data = *(volatile uint16_t*)&LCD->RAM;
//	return  data;
//}

///**
// * @brief SMC写寄存器
// * 
// * @param reg 
// */
//void lcd_write_reg(uint16_t reg)
//{
//    LCD->REG = reg;
//}

///**
// * @brief SMC写数据
// * 
// * @param data 
// */
//void lcd_write_data(uint16_t data)
//{
//    LCD->RAM = data;
//}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * @brief SMC 硬件端口初始化
 * 
 */
static void smc_port_init(void)
{
    stc_gpio_init_t stcGpioInit;

    (void)GPIO_StructInit(&stcGpioInit);
    stcGpioInit.u16PinDrv = PIN_DRV_HIGH;

    /* LCD_CS */
    (void)GPIO_Init(LCD_CS_PORT, LCD_CS_PIN, &stcGpioInit);

    /* LCD_WE */
    (void)GPIO_Init(LCD_WE_PORT, LCD_WE_PIN, &stcGpioInit);

    /* LCD_OE */
    (void)GPIO_Init(LCD_OE_PORT, LCD_OE_PIN, &stcGpioInit);

    /* SMC_DATA[0:15] */
    (void)GPIO_Init(LCD_DATA0_PORT,  LCD_DATA0_PIN, &stcGpioInit);
    (void)GPIO_Init(LCD_DATA1_PORT,  LCD_DATA1_PIN, &stcGpioInit);
    (void)GPIO_Init(LCD_DATA2_PORT,  LCD_DATA2_PIN, &stcGpioInit);
    (void)GPIO_Init(LCD_DATA3_PORT,  LCD_DATA3_PIN, &stcGpioInit);
    (void)GPIO_Init(LCD_DATA4_PORT,  LCD_DATA4_PIN, &stcGpioInit);
    (void)GPIO_Init(LCD_DATA5_PORT,  LCD_DATA5_PIN, &stcGpioInit);
    (void)GPIO_Init(LCD_DATA6_PORT,  LCD_DATA6_PIN, &stcGpioInit);
    (void)GPIO_Init(LCD_DATA7_PORT,  LCD_DATA7_PIN, &stcGpioInit);
    (void)GPIO_Init(LCD_DATA8_PORT,  LCD_DATA8_PIN, &stcGpioInit);
    (void)GPIO_Init(LCD_DATA9_PORT,  LCD_DATA9_PIN, &stcGpioInit);
    (void)GPIO_Init(LCD_DATA10_PORT, LCD_DATA10_PIN, &stcGpioInit);
    (void)GPIO_Init(LCD_DATA11_PORT, LCD_DATA11_PIN, &stcGpioInit);
    (void)GPIO_Init(LCD_DATA12_PORT, LCD_DATA12_PIN, &stcGpioInit);
    (void)GPIO_Init(LCD_DATA13_PORT, LCD_DATA13_PIN, &stcGpioInit);
    (void)GPIO_Init(LCD_DATA14_PORT, LCD_DATA14_PIN, &stcGpioInit);
    (void)GPIO_Init(LCD_DATA15_PORT, LCD_DATA15_PIN, &stcGpioInit);

    (void)GPIO_Init(LCD_RS_PORT, LCD_RS_PIN, &stcGpioInit);

    /* LCD_DATA[0:15] */
    GPIO_SetFunc(LCD_DATA0_PORT,  LCD_DATA0_PIN, GPIO_FUNC_12_EXMC, PIN_SUBFUNC_DISABLE);
    GPIO_SetFunc(LCD_DATA1_PORT,  LCD_DATA1_PIN, GPIO_FUNC_12_EXMC, PIN_SUBFUNC_DISABLE);
    GPIO_SetFunc(LCD_DATA2_PORT,  LCD_DATA2_PIN, GPIO_FUNC_12_EXMC, PIN_SUBFUNC_DISABLE);
    GPIO_SetFunc(LCD_DATA3_PORT,  LCD_DATA3_PIN, GPIO_FUNC_12_EXMC, PIN_SUBFUNC_DISABLE);
    GPIO_SetFunc(LCD_DATA4_PORT,  LCD_DATA4_PIN, GPIO_FUNC_12_EXMC, PIN_SUBFUNC_DISABLE);
    GPIO_SetFunc(LCD_DATA5_PORT,  LCD_DATA5_PIN, GPIO_FUNC_12_EXMC, PIN_SUBFUNC_DISABLE);
    GPIO_SetFunc(LCD_DATA6_PORT,  LCD_DATA6_PIN, GPIO_FUNC_12_EXMC, PIN_SUBFUNC_DISABLE);
    GPIO_SetFunc(LCD_DATA7_PORT,  LCD_DATA7_PIN, GPIO_FUNC_12_EXMC, PIN_SUBFUNC_DISABLE);
    GPIO_SetFunc(LCD_DATA8_PORT,  LCD_DATA8_PIN, GPIO_FUNC_12_EXMC, PIN_SUBFUNC_DISABLE);
    GPIO_SetFunc(LCD_DATA9_PORT,  LCD_DATA9_PIN, GPIO_FUNC_12_EXMC, PIN_SUBFUNC_DISABLE);
    GPIO_SetFunc(LCD_DATA10_PORT, LCD_DATA10_PIN, GPIO_FUNC_12_EXMC, PIN_SUBFUNC_DISABLE);
    GPIO_SetFunc(LCD_DATA11_PORT, LCD_DATA11_PIN, GPIO_FUNC_12_EXMC, PIN_SUBFUNC_DISABLE);
    GPIO_SetFunc(LCD_DATA12_PORT, LCD_DATA12_PIN, GPIO_FUNC_12_EXMC, PIN_SUBFUNC_DISABLE);
    GPIO_SetFunc(LCD_DATA13_PORT, LCD_DATA13_PIN, GPIO_FUNC_12_EXMC, PIN_SUBFUNC_DISABLE);
    GPIO_SetFunc(LCD_DATA14_PORT, LCD_DATA14_PIN, GPIO_FUNC_12_EXMC, PIN_SUBFUNC_DISABLE);
    GPIO_SetFunc(LCD_DATA15_PORT, LCD_DATA15_PIN, GPIO_FUNC_12_EXMC, PIN_SUBFUNC_DISABLE);

    GPIO_SetFunc(LCD_CS_PORT, LCD_CS_PIN, GPIO_FUNC_12_EXMC, PIN_SUBFUNC_DISABLE);
    GPIO_SetFunc(LCD_RS_PORT, LCD_RS_PIN, GPIO_FUNC_12_EXMC, PIN_SUBFUNC_DISABLE);
    GPIO_SetFunc(LCD_WE_PORT, LCD_WE_PIN, GPIO_FUNC_12_EXMC, PIN_SUBFUNC_DISABLE);
    GPIO_SetFunc(LCD_OE_PORT, LCD_OE_PIN, GPIO_FUNC_12_EXMC, PIN_SUBFUNC_DISABLE);
}
