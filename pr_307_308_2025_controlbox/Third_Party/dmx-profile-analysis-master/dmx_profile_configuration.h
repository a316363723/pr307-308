#ifndef DMX_DMX_PROFILE_ID_FX_CONFIGRATION_H
#define DMX_DMX_PROFILE_ID_FX_CONFIGRATION_H
#include <stddef.h>
#include <stdint.h>

/*****************************************/


/*****************************************/
#define PRODUCT_MT_PRO      0
#define PRODUCT_APUTURE_S1C 1
#define PRODUCT_APUTURE_S2C 2
#define PRODUCT_APUTURE_S4C 3

#define DMX_FX_PIXEL_CFG    (PRODUCT_MT_PRO)

/*****************************************/
#define PROFILE_LIGHT_ENGINE_NUM   2

/*****************************************/

#if PROJECT_TYPE==307 
#define DMX_ULTIMATE_8BIT_RGB_PROFILE_CFG_CCT_RGB    (0x0801)
#define DMX_ULTIMATE_8BIT_RGB_PROFILE_CFG_WW_RGB     (0x0205)
#define DMX_ULTIMATE_8BIT_RGB_PROFILE_CFG_RGB        (0x0201)
#define DMX_ULTIMATE_8BIT_RGB_PROFILE_CFG_HSI        (0x0303)
#define DMX_ULTIMATE_8BIT_RGB_PROFILE_CFG_FX         (0x0707)

#define DMX_ULTIMATE_16BIT_RGB_PROFILE_CFG_CCT_RGB    (0x0802)
#define DMX_ULTIMATE_16BIT_RGB_PROFILE_CFG_RGB        (0x0202)
#define DMX_ULTIMATE_16BIT_RGB_PROFILE_CFG_WW_RGB     (0x0206)
#define DMX_ULTIMATE_16BIT_RGB_PROFILE_CFG_HSI        (0x0304)
#define DMX_ULTIMATE_16BIT_RGB_PROFILE_CFG_FX         (0x0708)

#define DMX_CCT_MIN         (2000UL)
#define DMX_CCT_MAX         (10000UL)
#define DMX_DUV_MIN         ((float)-0.02f)
#define DMX_DUV_MAX         ((float)0.02f)
#define DMX_FX_GM_MIN       ((int8_t)-100)
#define DMX_FX_GM_MAX       ((int8_t)100)
#endif
#if PROJECT_TYPE==308


#define DMX_ULTIMATE_8BIT_RGB_PROFILE_CFG_CCT_RGB    (0x0801)
#define DMX_ULTIMATE_8BIT_RGB_PROFILE_CFG_RGB        (0x0201)
#define DMX_ULTIMATE_8BIT_RGB_PROFILE_CFG_WW_RGB     (0x0205)
#define DMX_ULTIMATE_8BIT_RGB_PROFILE_CFG_HSI        (0x0301)
#define DMX_ULTIMATE_8BIT_RGB_PROFILE_CFG_FX         (0x0705)

#define DMX_ULTIMATE_16BIT_RGB_PROFILE_CFG_CCT_RGB    (0x0802)
#define DMX_ULTIMATE_16BIT_RGB_PROFILE_CFG_RGB        (0x0203)
#define DMX_ULTIMATE_16BIT_RGB_PROFILE_CFG_WW_RGB     (0x0206)
#define DMX_ULTIMATE_16BIT_RGB_PROFILE_CFG_HSI        (0x0302)
#define DMX_ULTIMATE_16BIT_RGB_PROFILE_CFG_FX         (0x0706)

#define DMX_CCT_MIN         (2700UL)
#define DMX_CCT_MAX         (6500UL)
#define DMX_DUV_MIN         ((float)-0.005f)
#define DMX_DUV_MAX         ((float)0.005f)
#define DMX_FX_GM_MIN       ((int8_t)-50)
#define DMX_FX_GM_MAX       ((int8_t)50)

#endif

/*****************************************/

#define DMX_PROFILE_0101_EN  1
#define DMX_PROFILE_0102_EN  1
#define DMX_PROFILE_0103_EN  1


#define DMX_PROFILE_0104_EN  1
#define DMX_PROFILE_0105_EN  1
#define DMX_PROFILE_0106_EN  1


#define DMX_PROFILE_0201_EN  1
#define DMX_PROFILE_0202_EN  1
#define DMX_PROFILE_0203_EN  1
#define DMX_PROFILE_0204_EN  1
#define DMX_PROFILE_0205_EN  1
#define DMX_PROFILE_0206_EN  1
#define DMX_PROFILE_0207_EN  1
#define DMX_PROFILE_0208_EN  1


#define DMX_PROFILE_0301_EN  1
#define DMX_PROFILE_0302_EN  1
#define DMX_PROFILE_0303_EN  1
#define DMX_PROFILE_0304_EN  1


#define DMX_PROFILE_0401_EN  1
#define DMX_PROFILE_0402_EN  1


#define DMX_PROFILE_0501_EN  1
#define DMX_PROFILE_0502_EN  1


#define DMX_PROFILE_0601_EN  1
#define DMX_PROFILE_0602_EN  1


#define DMX_PROFILE_0701_EN  1
#define DMX_PROFILE_0702_EN  1
#define DMX_PROFILE_0703_EN  1
#define DMX_PROFILE_0704_EN  1
#define DMX_PROFILE_0705_EN  1
#define DMX_PROFILE_0706_EN  1
#define DMX_PROFILE_0707_EN  1
#define DMX_PROFILE_0708_EN  1
#define DMX_PROFILE_0709_EN  1


#define DMX_PROFILE_0801_EN  1
#define DMX_PROFILE_0802_EN  1
#define DMX_PROFILE_0803_EN  1
#define DMX_PROFILE_0804_EN  1
#define DMX_PROFILE_0805_EN  1
#define DMX_PROFILE_0806_EN  1
#define DMX_PROFILE_0807_EN  1
#define DMX_PROFILE_0808_EN  1


#define DMX_PROFILE_0901_EN  1
#define DMX_PROFILE_0902_EN  1


#define DMX_PROFILE_2001_EN  1
#define DMX_PROFILE_2002_EN  1
#define DMX_PROFILE_2003_EN  1
#define DMX_PROFILE_2004_EN  1
#define DMX_PROFILE_2005_EN  1
#define DMX_PROFILE_2006_EN  1
#define DMX_PROFILE_2007_EN  1
#define DMX_PROFILE_2008_EN  1


#define DMX_PROFILE_2101_EN  1
#define DMX_PROFILE_2102_EN  1
#define DMX_PROFILE_2103_EN  1
#define DMX_PROFILE_2104_EN  1


#define DMX_PROFILE_2201_EN  1
#define DMX_PROFILE_2202_EN  1


#define DMX_PROFILE_3001_EN  1
#define DMX_PROFILE_3002_EN  1
#define DMX_PROFILE_3003_EN  1
#define DMX_PROFILE_3004_EN  1
#define DMX_PROFILE_3005_EN  1
#define DMX_PROFILE_3006_EN  1
#define DMX_PROFILE_3007_EN  1
#define DMX_PROFILE_3008_EN  1

#endif



