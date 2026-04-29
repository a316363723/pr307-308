/*********************************************************************************
  *Copyright(C), 2018, Aputure, All rights reserver.
  *FileName:    source_light.h
  *Author:      
  *Version:     v1.0
  *Date:        
  *Description: source模式对应的xy色坐标
  *History:  
        -v1.0: 20190826,Steven,初始版本
**********************************************************************************/
#ifndef __SOURCE_H
#define __SOURCE_H

#include <stdint.h>

struct source_table{

    uint16_t coord_x;
    uint16_t coord_y;
};

enum source_type{
    
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
};

extern const struct source_table source_tab[SOURCE_MAX];

#endif /*__SOURCE_H*/
