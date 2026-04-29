/*********************************************************************************
  *Copyright(C), 2020, Aputure, All rights reserver.
  *FileName:    dim_curve.h
  *Author:      
  *Version:     v1.0
  *Date:        
  *Description: 调光曲线，不同档位照度与最大照度的比例
  *History:  
        -v1.0: Steven,初始版本
**********************************************************************************/
#ifndef __DIM_CURVE_H
#define __DIM_CURVE_H
#include <stdint.h>

enum mixing_curve{
    
    CURVE_LINEAR = 0,   //线性
    CURVE_EXP,          //指数
    CURVE_LOG,          //对数
    CURVE_S             //S曲线
};

float get_curve_lux(float ratio, float lux_max, float lux_min, uint8_t curve);
#endif /*__DIM_CURVE_H*/
