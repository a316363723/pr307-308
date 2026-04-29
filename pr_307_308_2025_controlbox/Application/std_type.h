/**
 * @file std_type.h
 * @author Darwin (darwin.xiao@aputure.com)
 * @brief 
 * @version 0.1
 * @date 2022-09-14
 * 
 * @copyright Copyright (c) 2022 爱图仕
 * 
 * @par 修改日志:
 * <table>
 * <tr><th> Date          <th>Version <th>Author        <th>Description
 * <tr><td>2022-09-14     <td>1.0     <td>Darwin.Xiao   <td>初始创建
 * </table>
 */
#ifndef STD_TYPE_H
#define STD_TYPE_H


/* 当前的通讯类型 */
enum {
    COM_TYPE_CRMX = 0,
    COM_TYPE_BLE,
    COM_TYPE_NETWORK,    
    COM_TYPE_MAX,
};

/* 风扇模式 */
enum {
    FAN_MODE_SMART,
    FAN_MODE_HIGH,
    FAN_MODE_MEDIUM,
    FAN_MODE_SILENT,
    FAN_MODE_MAX,
};

/* 色纸品牌 */
enum {
    ROSCO = 0,
    LEE = 1,
    GEL_BRAND_MAX,
};

/* LEE品牌系列 */
enum {
    LEE_COLOR_CORRECTION = 0,
    LEE_COLOR_FILTERS,
    LEE_COLOR_600S,
    LEE_COLOR_COS_FILTERS,
    LEE_COLOR_700S,
    LEE_COLOR_MAX,
};

/* ROSCO品牌类型系列 */
enum {
    ROSCO_COLOR_CORRECTION = 0,
    ROSCO_COLOR_CALCOLOR,
    ROSCO_STORARO_SEL,
    ROSCO_CINELUX,
    ROSCO_COLOR_MAX,
};

enum {
    LEE_SUMS = LEE_COLOR_MAX,
    ROSCO_SUMS = ROSCO_COLOR_MAX,
};

/* SOURCE类型 */
typedef enum{
	TUNGSTEN_BULB = 0,
	INCANDESCENT,
	HALOGEN,
	ANTIQUE_BULB,
	WARM_ANTIQUE_BULB,
	CHRISTMAS_LIGHTS,
	NIGHT_LIGHT,
	INFRARED_HEAT_LAMP,
	GROW_LIGHT,
	CFL_SOFT_WHITE,
	CFL_Bright_White,
	CFL_COOL_WhITE,
	CFL_DAYLIGHT,
	COOL_WHITE_1,
	COOL_WHITE_2,
	COOL_WHITE_3,
	WARM_WHITE,
	CFL_BLACKLIGHT,
	HMI,
	HIGH_PRESSURE_SODIUM,
	LOW_PRESSURE_SODIUM,
	MERCURY_VAPOR,
	METAL_HALIDE,
	CERAMIC,
	CARBON_ARC,
	XENON,
	CANDLE,
	GAS_FIRE,
	SUN_DIRECT,
	SUN_OVERCAST,
	SUN_BLUE_HOUR,
	MOBILE_PHONE,
	COMPUTER_MONITOR,
	ELECTROLUMONENCE,
	BLOW_TORCH,
	ROAD_FLARE,
	AMBER_CAUTION,
	GREEN_TRAFFIC_LIGHT,
	YELLOW_TRAFFIC_LIGHT,
	RED_TTRAFFIC_LIGHT,
	BLUE_GLOW_STICK,
	GREEN_GLOW_STICK,
	RED_GLOW_STICK,
	YELLOW_GLOW_STICK,
	PINK_GLOW_STICK,
	VIOLE_GLOW_STICK,
	SOURCE_MAX
}src_light_t;


/* 输出模式 */
enum {
    OUTPUT_MODE_CONST_OUTPUT = 0,
    OUTPUT_MODE_MAX_OUTPUT,
    OUTPUT_MODE_TYPE_MAX,
};

/* 语言 */
enum {
    LANGU_EN = 0,
    LANGU_CHINESE = 1,
    LANGU_MAX
};

/* DMX丢失行为 */
enum {
    DMX_LOSS_BEHAVIOR_HLS = 0,// black out
    DMX_LOSS_BEHAVIOR_BO  = 1, // fade to black
    DMX_LOSS_BEHAVIOR_FTB = 2,// hold last setting
    DMX_LOSS_BEHAVIOR_HFO = 3,// hold 2min fade out
    DMX_LOSS_BEHAVIOR_MAX
};

/* 屏幕保护设置 */
enum {
    SCREENSAVAR_SETTING_AFTER_30_MINS = 0,
    SCREENSAVAR_SETTING_AFTER_60_MINS,
    SCREENSAVAR_SETTING_AFTER_120_MINS,
    SCREENSAVAR_SETTING_NO,
    SCREENSAVAR_SETTING_MAX,
};

/* 自定义光效类型 */
enum {
    CUSTOM_FX_TYPE_PIKER = 0,
    CUSTOM_FX_TYPE_MUSIC,
    CUSTOM_FX_TYPE_TOUCHBAR,
    CUSTOM_FX_TYPE_MAX,
};

/* 终端电阻状态 */
enum {
    DMX_TEMINATION_OFF = 0,
    DMX_TEMINATION_ON = 1,
};

/* 演播厅模式 */
enum {
    STUDIO_MODE_OFF = 0,
    STUDIO_MODE_ON,
};

/* CRMX配对状态机 */
enum {
    LUMENRADIO_NO_PAIR = 0,
    LUMENRADIO_START_PAIR,
    LUMENRADIO_PAIRING,
    LUMENRADIO_PARIED_SUCCEED,
    LUMENRADIO_PARIED_FAILED,
};

/* CRMX取消配对状态机 */
enum {
    LUMENRADIO_UNPAIR_IDLE = 0,
    LUMENRADIO_UNPAIR_START,
};

/* 固件升级状态机 */
enum {
    FIRMWARE_UPD_STATUS_IDLE,
    FIRMWARE_UPD_STATUS_START,
    FIRMWARE_UPD_STATUS_UPDING,
    FIRMWARE_UPD_STATUS_OK,
    FIRMWARE_UPD_STATUS_FAILED,
    FIRMWARE_UPD_STATUS_FAILED_NO_FW,    
    FIRMWARE_UPD_STATUS_FAILED_FORMAT_ERR,
};


#endif // !STD_TYPE_H