/*********************************************************************************
  *Copyright(C), 2023, Aputure, All rights reserver.
  *FileName:  	color_mixing_dev.c
  *Author:    	
  *Version:   	vv4.0.0
  *Date:      	2023/08/25
  *Description: 合光
  *History:  
            -v3.0.0: 2022/04/16,Steven,初始版本
            -v3.0.1: 2022/06/07,Steven,将输出模式、调光曲线、功率限制加到光模式接口中
            -v3.0.2: 2023/03/30,Matthew
                     1.新增wchar32、enum int类型库（编译器设置枚举类型为int类型时使用）
                     2.修复传入亮度递增时输出亮度可能回退的bug
            -v4.0.0: 2023/08/28,Matthew
                     1.加入功率比和照度比曲线多项式系数用于功率计算，功率与照度的曲线需要用excel拟合
                     2.加入照度lux转pwm输出接口，可用于输出结果lux转pwm及输出照度线性缓变过程中lux转pwm
                     3.删除min_lux、rgb_min_lux参数，由新的min_lightness控制合光最小亮度
**********************************************************************************/
#ifndef _MIXING_DEV_H
#define _MIXING_DEV_H
#include "color_mixing.h"


void color_mixing_init(uint8_t mode);
void color_cct_calc(float lightness, uint16_t cct, float duv, 
                        uint8_t ilumination, uint8_t curve, float power_limit, struct mixing_pwm *pwm, struct mixing_lux *lux);
void color_hsi_calc(float lightness, float hue, float sat, uint16_t cct, float duv, uint8_t curve, float power_limit, struct mixing_pwm *pwm, struct mixing_lux *lux);
void color_coordinate_calc(float lightness, float coord_x, float coord_y, uint8_t curve, float power_limit, struct mixing_pwm *pwm, struct mixing_lux *lux);
void color_gel_calc(float lightness, uint8_t brand, uint8_t type, uint16_t cct, uint16_t color, 
                            uint8_t curve, float power_limit, struct mixing_pwm *pwm, struct mixing_lux *lux);
void color_rgb_calc(float lightness, float r, float g, float b, uint8_t curve, float power_limit, struct mixing_pwm *pwm, struct mixing_lux *lux);
void color_source_calc(float lightness, uint8_t type, float coord_x, float coord_y, uint8_t mode, 
                            uint8_t curve, float power_limit, struct mixing_pwm *pwm, struct mixing_lux *lux);
void color_mixing_calc(struct color_mixing *mixing, uint8_t curve, float power_limit, struct mixing_pwm *pwm, struct mixing_lux *lux);
void color_rgbww_calc(float r, float g, float b, float ww, float cw, uint8_t curve, float power_limit, struct mixing_pwm *pwm, struct mixing_lux *lux);

uint16_t *color_get_zerolux_maxpwm(void);
void color_set_constant_max_lux(float lux);
void color_set_min_lux(float lux);
void color_lux_to_pwm(const struct mixing_lux *lux, struct mixing_pwm *pwm);
float gm_to_duv(uint8_t gm);
void set_max_power(float power_radio);
void fan_mode_set_min_lightness(void);
void analog_mode_lux_less_zero_process(struct mixing_lux *lux);
#endif /*_MIXING_DEV_H*/
