#ifndef __UI_CONFIG_H
#define __UI_CONFIG_H

//#define UI_PLATFORM_MCU
#ifndef UI_PLATFORM_MCU
    #define UI_PLATFORM_PC 
#endif   

//#define UI_DEBUG

//是否开启打印
#ifdef  UI_DEBUG
#define UI_PRINTF                printf
#else
#define UI_PRINTF(...)                
#endif

#define UI_MAJOR_VERSION				1
#define UI_MINOR_VERSION				2

//机器名字
#define UI_MACHINE_NAME                "PR-307-L3"

//使能假数据
#define UI_DUMMY_DATA_ENABLE           0

//是否使能测试页面
#define UI_TEST                         0

//是否打开触摸光效
#define UI_TOUCHBAR_FX_ENABLE           1

//光模式界面通用控件圆角
#define LIGHT_MODE_COMMON_RADIUS        5

//光效界面通用控件圆角
#define LIGHT_EFFECT_COMMON_RADIUS       5

//亮度(千分级调光), 出厂默认50.0%
#define UI_INTENSITY_MIN            0u
#define UI_INTENSITY_MAX            1000u
#define UI_INTENSITY_VAL            500u
#define UI_INTENSITY_STEP           1u
#define UI_MIN_INETENSITY_MAX       750u

//CCT, 常规光模式CCT值为5600
#if PROJECT_TYPE==307
#define UI_CCT_MIN                  2000u
#define UI_CCT_DEF_VAL              5600u
#define UI_CCT_MAX                  10000u
#define UI_CCT_STEP                 50u

#define UI_GM_MAX                 100
#define UI_GM_MIN                  -100
#elif PROJECT_TYPE==308
#define UI_CCT_MIN                  2700u
#define UI_CCT_DEF_VAL              5600u
#define UI_CCT_MAX                  6500u
#define UI_CCT_STEP                 100u

#define UI_GM_MAX                 50
#define UI_GM_MIN                  -50
#endif
//HUE，色相
#define UI_HUE_MIN                  1
#define UI_HUE_MAX                  3600
#define UI_HUE_DEF_VAL              1
#define UI_HUE_STEP                 1

//RGB
#define UI_RGB_MIN                  0u
#define UI_RGB_DEF_VAL              999
#define UI_RGB_MAX                  1000u
#define UI_RGB_STEP                 1u

//DMX Smooth 缓变时间
#define UI_DMX_NO_FADE_TIME                  0
#define UI_DMX_SMOOTH_FADE_TIME              350
#define UI_DMX_SUPER_SMOOTH_FADE_TIME        1000

//输出模式
#define UI_LEAD_OUT_MODE            1

//主从模式
#define UI_LEAD_FOLLOE_MODE         1

//高速摄影模式
#define UI_HIGH_SPEED_MODE          1

//DMX模式
#define UI_DMX_MODE                 0

//电动支架
#define UI_ELECT_ACCESSORY          1

//屏保页面
#define UI_SCREEN_SAVER             0

//电池页面
#define UI_BAT_MODE                 0

//DMX通道号
#define DMX_CHANNEL_DEF             0x01u

//Source页面, Source调节范围
#define SOURCE_ADJ_RANGE            50

//
#define FREQUENCY_SELECT_VALUE      1100

//编码智能提速等级
#define ENCODER_OFFSET_LEVEL0       1u
#define ENCODER_OFFSET_LEVEL1       2u
#define ENCODER_OFFSET_LEVEL2       3u
#define ENCODER_OFFSET_LEVEL3       4u

//编码器智能提速步距
#define ENCODER_OFFSET_LEVEL0_STEP  1
#define ENCODER_OFFSET_LEVEL1_STEP  5
#define ENCODER_OFFSET_LEVEL2_STEP  13
#define ENCODER_OFFSET_LEVEL3_STEP  26

//被聚焦后被编辑的颜色
#define COMMON_EDITED_COLOR             lv_color_make(0, 160, 233)

#define RED_THEME_COLOR                 lv_color_make(196, 56, 43)

#define CCT_RED_THEME_COLOR             lv_color_make(106, 0, 10)

#define GEL_CONT_BORDER_COLOR_1         lv_color_make(108, 206, 255)
  
#define BLUE_THEME_COLOR                lv_color_make(0,0,255)


//进入屏幕保护的时间
#define UI_ENTER_SCREEN_PROCTECTOR_TIME     (12ul * (60ul * 1000ul))

#endif
