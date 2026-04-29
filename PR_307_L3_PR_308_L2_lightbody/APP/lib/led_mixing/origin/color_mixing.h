/*********************************************************************************
  *Copyright(C), 2023, Aputure, All rights reserver.
  *FileName:  	color_mixing_dev.c
  *Author:    	
  *Version:   	v3.0.2
  *Date:      	2023/03/30
  *Description: 合光
  *History:  
			-v3.0.0: 2022/04/16,Steven,初始版本
            -v3.0.1: 2022/06/07,Steven,将输出模式、调光曲线、功率限制加到光模式接口中
            -v3.0.2: 2023/03/30,Matthew
                     1.新增wchar32、enum int类型库（编译器设置枚举类型为int类型时使用）
                     2.修复传入亮度递增时输出亮度可能回退的bug
            -v3.0.3: 2023/06/27,Matthew，加入功率曲线多项式，解决照度与功率关系非线性时功率计算问题
**********************************************************************************/
#ifndef __3COLOR_MIXING_H
#define __3COLOR_MIXING_H
#include <stdint.h>
#include <stddef.h>
#include "base_data.h"
#include "dim_curve.h"

//struct mixing_dev;
//#pragma pack (1)

struct mix_coordinate{
    
    float coord_x;
    float coord_y;
};

enum ilumination_mode{
    
    ILL_CONSTANT = 0,   //恒照度模式
    ILL_MAX             //最大照度模式
};

struct source_arg{
    
    float lightness;
    uint8_t type;
    float coord_x;
    float coord_y;
};

struct mixing_pwm{
    
    uint16_t pwm[5];
};

struct lux_struct{
    
    float lux[5];
};

struct coord_lux{
    
    struct mix_coordinate coordinate;
    float lux;
};

struct color_convert_matrix{

    uint16_t cct;
    float matrix[9]; //按循序分别为M11，M12，M13；M21，M22，M23.。。。。
};

struct cct_arg{
    
    float lightness;    //照度，范围0-100%，1为100%
    uint16_t cct;       //色温，单位k
    float duv;          //Duv
};

struct hsi_arg{
    
    float lightness;    //照度，范围0-100%，1为100%
    float hue;          //色环，0-360
    float sat;          //饱和度，0-100
    uint16_t cct;       //中心色温，单位k
};

struct coordinate_arg{
    
    float lightness;    //照度，范围0-100%，1为100%
    float x;            //
    float y;            //
};

struct gel_arg{
    
    float lightness;    //照度，范围0-100%，1为100%
    uint8_t brand;
    uint8_t type;
    uint16_t cct;
    uint16_t color;
};

struct rgb_arg{
    
    float lightness;    //照度，范围0-100%，1为100%
    float r;            //R，范围0-100%，1为100%
    float g;            //G，范围0-100%，1为100%
    float b;            //B，范围0-100%，1为100%
};

/*---ColorMixing里的子类型---*/
enum color_mixing_mode{
    
    Mix_ColorMixingMode_CCT = 0,
    Mix_ColorMixingMode_HSI,
    Mix_ColorMixingMode_GEL,
    Mix_ColorMixingMode_XY,
    Mix_ColorMixingMode_SOUYRCE,
    Mix_ColorMixingMode_RGB,
    Mix_ColorMixingMode_NULL,
};

struct color_mixing_arg{
    
    uint8_t mode;   /* 参考 enum color_mixing_mode 中模式*/
    union{
        struct cct_arg cct;
        struct hsi_arg hsi;
        struct gel_arg gel;
        struct coordinate_arg xy;
        struct source_arg source;
        struct rgb_arg rgb;
    }arg;
};

struct color_mixing{
    
    struct color_mixing_arg color1_arg;  /*颜色1参数*/
    struct color_mixing_arg color2_arg;  /*颜色2参数*/
    float   ratio;                       /*颜色参与比例<0-100%>， 1表示100%，ratio表示Color2参与比例，Color1参与比例为(1-ratio)*/
};  /*size: */

//struct mixing_opt{
    
//  int8_t (*set_ilumination_mode)(struct mixing_dev *dev, uint8_t mode); //设置光输出模式
//  int8_t (*set_mixing_curve)(struct mixing_dev *dev, uint8_t curve); //设置调光曲线
//  int8_t (*cct_to_pwm)(struct mixing_dev *dev, float lightness, uint16_t cct, float duv, struct mixing_pwm *pwm); //CCT求PWM
//  int8_t (*hsi_to_pwm)(struct mixing_dev *dev, float lightness, float hue, float sat, uint16_t cct, struct mixing_pwm *pwm); //HSi求PWM
//  int8_t (*coordinate_to_pwm)(struct mixing_dev *dev, float lightness, float coord_x, float coord_y, struct mixing_pwm *pwm_struct, struct lux_struct *lux); //色坐标求PWM及对应照度
//  int8_t (*gel_to_pwm)(struct mixing_dev *dev, float lightness, uint8_t brand, uint8_t type, uint16_t cct, uint16_t color, struct mixing_pwm *pwm_struct); //gel求PWM
//  int8_t (*rgb_to_pwm)(struct mixing_dev *dev, float lightness, float r, float g, float b, struct mixing_pwm *pwm_struct); //rgb求PWM
//  int8_t (*rgbww_to_pwm)(struct mixing_dev *dev, float lightness, float r, float g, float b, float ww, float cw, struct mixing_pwm *pwm_struct); //rgb求PWM
//  int8_t (*source_to_pwm)(struct mixing_dev *dev, float lightness, uint8_t type, float coord_x, float coord_y, struct mixing_pwm *pwm_struct, uint8_t mode);
//  int8_t (*color_mixing_to_pwm)(struct mixing_dev *dev, struct color_mixing mixing, struct mixing_pwm *pwm_struct);
//};

struct mixing_led{
    
    char name[16];  //灯珠名称
    struct mix_coordinate average_coord; //平均色坐标
    float max_lux; //最大照度
    float min_lux; //最小照度
    uint16_t min_pwm;//亮度为0的最大pwm值
    uint16_t max_power; //单色最大功率
    float cur_power; //当前合光功率
    float power_curve_coef[4];//功率曲线多项式系数
    struct coord_lux led_arg; //参与合光的led的照度和色坐标
    uint16_t length; //基础数据长度
    const Base_Data* base_data; //基础数据
};

struct mixing_dev_cfg{
    
    uint8_t curve;          //调光曲线 详见enum mixing_curve
    uint8_t ilumination;    //光输出模式 详见enum ilumination_mode
    float power_limit;      //功率限制，0-1，1为不限制
};

struct mixing_dev{

    char dev_name[16];
    //struct mixing_opt opt;
    uint32_t min_lux;       /*合光最小照度值*/
    uint32_t rgb_min_lux;   /*RGB模式最小照度值*/
    uint16_t max_power;     /*最大功率*/
    float cur_power;        /*当前输出功率*/
    float power_limit;      //功率限制，0-1，1为不限制
    uint32_t max_lux;       /*恒照度模式最大照度*/
    uint8_t led_num;        /*灯珠数量*/
    struct mixing_led led_arg[5]; //参与合光的LED
    struct coord_lux object_arg;  //目标色坐标及照度
    uint8_t ilumination;        //光输出模式 详见enum ilumination_mode
    uint8_t curve;          //调光曲线 详见enum mixing_curve
};

struct mixing_led_cfg{
    
    char name[16];  //灯珠名称
    uint32_t max_power; //单色最大功率
    float power_curve_coef[4];//功率曲线多项式系数
    uint16_t length; //基础数据长度
    const Base_Data *base_data; //基础数据
};

struct mixing_cfg{
    
    char *dev_name;                 /*设备名称*/
    uint32_t min_lux;               /*合光最小照度值*/
    uint32_t rgb_min_lux;           /*RGB模式最小照度值*/
    uint16_t max_power;             /*最大功率*/
    //uint32_t max_lux;             /*恒照度模式最大照度*/
    uint8_t led_num;                /*灯珠数量*/
    //struct mixing_opt opt;
    struct mixing_led_cfg *led[5]; //基础数据
};

/**
  * @brief  合光设备初始化
  * @param  dev 合光设备
  * @param  cfg 配置参数，具体参数详见struct mixing_cfg结构体
  * @retval 0：成功 -1：失败
  */
int8_t mixing_dev_init(struct mixing_dev *dev, struct mixing_cfg *cfg); 

/**
  * @brief  led配置初始化
  * @param  cfg 配置参数，具体参数详见struct mixing_cfg结构体
  * @param  name LED名称
  * @param  max_power 单色最大功率
  * @param  length 基础数据长度
  * @param  base_data 基础数据
  * @retval none
  */
void led_cfg_init(struct mixing_led_cfg *cfg, char *name, uint32_t max_power, const float power_curve_coef[], uint16_t length, const Base_Data *base_data);

/**
  * @brief  CCT求PWM
  * @param  dev 合光设备
  * @param  lightness 照度等级，范围<0.0-1.0>，0.5为50%，1为100%
  * @param  cct 色温，范围<1600-10000>，单位k
  * @param  duv Duv，需将GM转为Duv
  * @param  ilumination_mode 光输出模式，详见enum ilumination_mode
  * @param  curve 调光曲线，详见enum mixing_curve
  * @param  power_limit 功率限制，范围<0.0-1.0>，1.0表示无限制，0.1表示限制为10%
  * @param  pwm 计算结果，PWM，用于设置定时器
  * @retval 0：成功 -1：失败
  */
int8_t cct_to_pwm(struct mixing_dev *dev, float lightness, uint16_t cct, float duv, 
                    uint8_t ilumination, uint8_t curve, float power_limit, struct mixing_pwm *pwm);

/**
  * @brief  HSI求PWM
  * @param  dev 合光设备
  * @param  lightness 照度等级，范围<0.0-1.0>，0.5为50%，1为100%
  * @param  hue 色环，范围<0.0-360.0>
  * @param  sat 饱和度，范围<0.0-100.0>
  * @param  cct 中心色温，范围<1600-10000>，单位k
  * @param  curve 调光曲线，详见enum mixing_curve
  * @param  power_limit 功率限制，范围<0.0-1.0>，1.0表示无限制，0.1表示限制为10%
  * @param  pwm 计算结果，PWM，用于设置定时器
  * @retval 0：成功 -1：失败
  */
int8_t hsi_to_pwm(struct mixing_dev *dev, float lightness, float hue, float sat, uint16_t cct, uint8_t curve, float power_limit, struct mixing_pwm *pwm);

/**
  * @brief  色坐标求PWM及对应照度
  * @param  dev 合光设备
  * @param  lightness 照度等级，范围<0.0-1.0>，0.5为50%，1为100%
  * @param  coord_x 色坐标x，范围<0.0-1.0>
  * @param  coord_y 色坐标y，范围<0.0-1.0>
  * @param  curve 调光曲线，详见enum mixing_curve
  * @param  power_limit 功率限制，范围<0.0-1.0>，1.0表示无限制，0.1表示限制为10%
  * @param  pwm_struct pwm计算结果，PWM，用于设置定时器
  * @param  lux lux计算结果
  * @retval 0：成功 -1：失败
  */
int8_t coordinate_to_pwm(struct mixing_dev *dev, float lightness, float coord_x, float coord_y, 
                            uint8_t curve, float power_limit, struct mixing_pwm *pwm_struct, struct lux_struct *lux);

/**
  * @brief  gel求PWM
  * @param  dev 合光设备
  * @param  brand 品牌，0x00：LEE; 0x01：Resco
  * @param  type 类型
  * @param  cct 色温，范围<1600-10000>，单位k
  * @param  color
  * @param  curve 调光曲线，详见enum mixing_curve
  * @param  power_limit 功率限制，范围<0.0-1.0>，1.0表示无限制，0.1表示限制为10%
  * @param  pwm 计算结果，PWM，用于设置定时器
  * @retval 0：成功 -1：失败
  */
int8_t gel_to_pwm(struct mixing_dev *dev, float lightness, uint8_t brand, uint8_t type, uint16_t cct, uint16_t color, 
                    uint8_t curve, float power_limit, struct mixing_pwm *pwm_struct);

/**
  * @brief  rgb求PWM
  * @param  dev 合光设备
  * @param  lightness 照度等级，范围<0.0-1.0>，0.5为50%，1为100%
  * @param  r r值，范围<0.0-1.0>
  * @param  g g值，范围<0.0-1.0>
  * @param  b b值，范围<0.0-1.0>
  * @param  curve 调光曲线，详见enum mixing_curve
  * @param  power_limit 功率限制，范围<0.0-1.0>，1.0表示无限制，0.1表示限制为10%
  * @param  pwm_struct 计算结果，PWM，用于设置定时器
  * @retval 0：成功 -1：失败
  */
int8_t rgb_to_pwm(struct mixing_dev *dev, float lightness, float r, float g, float b, uint8_t curve, float power_limit, struct mixing_pwm *pwm_struct);

/**
  * @brief  rgbww求PWM
  * @param  dev 合光设备
  * @param  r r值，范围<0.0-1.0>
  * @param  g g值，范围<0.0-1.0>
  * @param  b b值，范围<0.0-1.0>
  * @param  ww ww值，范围<0.0-1.0>
  * @param  cw cw值，范围<0.0-1.0>
  * @param  curve 调光曲线，详见enum mixing_curve
  * @param  power_limit 功率限制，范围<0.0-1.0>，1.0表示无限制，0.1表示限制为10%
  * @param  pwm_struct 计算结果，PWM，用于设置定时器
  * @retval 0：成功 -1：失败
  */
int8_t rgbww_to_pwm(struct mixing_dev *dev, float r, float g, float b, float ww, float cw, uint8_t curve, float power_limit, struct mixing_pwm *pwm_struct);

/**
  * @brief  source求PWM
  * @param  dev 合光设备
  * @param  lightness 照度等级，范围<0.0-1.0>，0.5为50%，1为100%
  * @param  type 类型,详见enum source_type
  * @param  coord_x source色坐标x，范围<0.0-1.0>
  * @param  coord_y source色坐标y，范围<0.0-1.0>
  * @param  mode 色坐标模式，范围<0-1>，0：根据type查找色坐标，1：使用传参coord_x、coord_y
  * @param  curve 调光曲线，详见enum mixing_curve
  * @param  power_limit 功率限制，范围<0.0-1.0>，1.0表示无限制，0.1表示限制为10%
  * @param  pwm_struct 计算结果，PWM，用于设置定时器
  * @retval 0：成功 -1：失败
  */
int8_t source_to_pwm(struct mixing_dev *dev, float lightness, uint8_t type, 
                        float coord_x, float coord_y, uint8_t mode, uint8_t curve, float power_limit, struct mixing_pwm *pwm_struct);

/**
  * @brief  颜色混合求PWM
  * @param  dev 合光设备
  * @param  lightness 照度等级，范围<0.0-1.0>，0.5为50%，1为100%
  * @param  mixing 颜色参数，详见struct color_mixing
  * @param  curve 调光曲线，详见enum mixing_curve
  * @param  power_limit 功率限制，范围<0.0-1.0>，1.0表示无限制，0.1表示限制为10%
  * @param  pwm_struct 计算结果，PWM，用于设置定时器
  * @retval 0：成功 -1：失败
  */
int8_t mixing_to_pwm(struct mixing_dev *dev, struct color_mixing *mixing, uint8_t curve, float power_limit, struct mixing_pwm *pwm_struct);

///**
//  * @brief  设置调光曲线
//  * @param  dev 合光设备
//  * @param  curve 调光曲线，详见enum mixing_curve
//  * @retval 0：成功 -1：失败
//  */
//int8_t set_mixing_curve(struct mixing_dev *dev, uint8_t curve);

///**
//  * @brief  设置光输出模式
//  * @param  dev 合光设备
//  * @param  mode 光输出模式，详见enum ilumination_mode
//  * @retval 0：成功 -1：失败
//  */
//int8_t set_ilumination_mode(struct mixing_dev *dev, uint8_t mode); //设置光输出模式

///**
//  * @brief  设置功率限制
//  * @param  dev 合光设备
//  * @param  limit 功率限制，范围<0.0-1.0>，1.0为不限制，0.5为50%
//  * @retval 0：成功 -1：失败
//  */
//int8_t set_power_limit(struct mixing_dev *dev, float limit); //设置功率限制

/**
  * @brief  设置最大照度
  * @param  dev 合光设备
  * @param  lux 照度
  * @retval 0：成功 -1：失败
  */
int8_t set_max_lux(struct mixing_dev *dev, float lux);

/**
  * @brief  设置非0最小照度
  * @param  dev 合光设备
  * @param  lux 照度
  * @retval 0：成功 -1：失败
  */
int8_t set_min_lux(struct mixing_dev *dev, float lux);

/**
  * @brief  获取亮度为0时的最大pwm值
  * @param  dev 合光设备
  * @retval PWM数组，数组长度为5
  */
uint16_t *get_zerolux_maxpwm(struct mixing_dev *dev);

//#pragma pack ()

#endif /*__3COLOR_MIXING_H*/
