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
**********************************************************************************/
#ifndef _MIXING_DEV_H
#define _MIXING_DEV_H
#include "color_mixing.h"

void color_mixing_init(uint8_t mode);
void color_cct_to_pwm(float lightness, uint16_t cct, float duv, 
                        uint8_t ilumination, uint8_t curve, float power_limit, struct mixing_pwm *pwm);
void color_hsi_to_pwm(float lightness, float hue, float sat, uint16_t cct, uint8_t curve, float power_limit, struct mixing_pwm *pwm);
void color_coordinate_to_pwm(float lightness, float coord_x, float coord_y, uint8_t curve, float power_limit, struct mixing_pwm *pwm_struct);
void color_gel_to_pwm(float lightness, uint8_t brand, uint8_t type, uint16_t cct, uint16_t color, 
                            uint8_t curve, float power_limit, struct mixing_pwm *pwm_struct);
void color_rgb_to_pwm(float lightness, float r, float g, float b, uint8_t curve, float power_limit, struct mixing_pwm *pwm_struct);
void color_rgbww_to_pwm(float r, float g, float b, float ww, float cw, uint8_t curve, float power_limit, struct mixing_pwm *pwm_struct);
void color_source_to_pwm(float lightness, uint8_t type, float coord_x, float coord_y, uint8_t mode, 
                            uint8_t curve, float power_limit, struct mixing_pwm *pwm_struct);
void color_mixing_to_pwm(struct color_mixing *mixing, uint8_t curve, float power_limit, struct mixing_pwm *pwm_struct);
uint16_t *color_get_zerolux_maxpwm(void);
void color_set_max_lux(float lux);
void color_set_min_lux(float lux);
float gm_to_duv(uint8_t gm);
void silence_mode_set_min_lux(uint8_t fan_mode);
#endif /*_MIXING_DEV_H*/

