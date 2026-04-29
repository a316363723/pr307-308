#ifndef DMX_PROFILE_H
#define DMX_PROFILE_H
#include "dmx_profile_configuration.h"
#pragma pack (1)
/*****************************************************************
*dmx_profile_fx_rgb_lite_I_8bit_arg
*********************************************************************/
typedef struct
{
    uint8_t     cct;
    uint8_t     gm;
    uint8_t     frq;
} fx_rgb_lite_I_8bit_cct_arg_t;

/*-Effect HSI-*/
typedef struct
{
    uint8_t     hue;
    uint8_t     sat;
    uint8_t     frq;
} fx_rgb_lite_I_8bit_hsi_arg_t;

/*-Effect GEL-*/
typedef struct
{
    uint8_t     cct;
    uint8_t     brand;
    uint8_t     type;
    uint8_t     color;
    uint8_t     frq;
} fx_rgb_lite_I_8bit_gel_arg_t;

/*0x01：Paparazzi*/
typedef struct
{
    uint8_t         cct;
    uint8_t         gm;
    uint8_t         frq;
} fx_rgb_lite_I_8bit_paparazzi_arg_t;

/*0x02：Lightning*/
typedef struct
{
    uint8_t                 cct;
    uint8_t                 gm;
    uint8_t                 frq;
} fx_rgb_lite_I_8bit_lightning_arg_t;

/*0x03：TV*/
typedef struct
{
    uint8_t     cct_rang;
    uint8_t     frq;
} fx_rgb_lite_I_8bit_tv_arg_t;

/*0x05：Fire*/
typedef struct
{
    uint8_t     cct_rang;
    uint8_t     frq;
} fx_rgb_lite_I_8bit_fire_arg_t;

/*0x08：Fault_Bulb*/
typedef struct
{
    uint8_t                 mode;
    union
    {
        fx_rgb_lite_I_8bit_cct_arg_t     cct;
        fx_rgb_lite_I_8bit_hsi_arg_t     hsi;
        fx_rgb_lite_I_8bit_gel_arg_t     gel;
    } arg;

} fx_rgb_lite_I_8bit_fault_bulb_arg_t;

/*0x09：Pulsing*/
typedef struct
{
    uint8_t                 mode;
    union
    {
        fx_rgb_lite_I_8bit_cct_arg_t     cct;
        fx_rgb_lite_I_8bit_hsi_arg_t     hsi;
        fx_rgb_lite_I_8bit_gel_arg_t     gel;

    } arg;

} fx_rgb_lite_I_8bit_pulsing_arg_t;

/*0x0B：Cop_Car*/
typedef struct
{
    uint8_t     color;
    uint8_t     frq;
} fx_rgb_lite_I_8bit_cop_car_arg_t;

/*0x0D：Party_Lights*/
typedef struct
{
    uint8_t     sat;
    uint8_t     frq;
} fx_rgb_lite_I_8bit_party_lights_arg_t;

/*0x0E：Fireworks*/
typedef struct
{
    uint8_t     color;
    uint8_t     frq;
} fx_rgb_lite_I_8bit_fireworks_arg_t;

typedef union
{
    fx_rgb_lite_I_8bit_paparazzi_arg_t     paparazzi;
    fx_rgb_lite_I_8bit_lightning_arg_t     lightning;
    fx_rgb_lite_I_8bit_tv_arg_t            tv;
    fx_rgb_lite_I_8bit_fire_arg_t          fire;
    fx_rgb_lite_I_8bit_fault_bulb_arg_t    fault_bulb;
    fx_rgb_lite_I_8bit_pulsing_arg_t       pulsing;
    fx_rgb_lite_I_8bit_cop_car_arg_t       copcar;
    fx_rgb_lite_I_8bit_party_lights_arg_t  party_lights;
    fx_rgb_lite_I_8bit_fireworks_arg_t     fireworks;
} fx_rgb_lite_I_8bit_arg_t;



/*****************************************************************
*dmx_profile_fx_rgb_lite_III_16bit_arg
*********************************************************************/
typedef struct
{
    uint8_t     cct;
    uint8_t     gm;
} fx_rgb_lite_III_16bit_cct_arg_t;      /*size: 5*/

/*-Effect HSI-*/
typedef struct
{
    uint8_t     hue;
    uint8_t     sat;
    uint8_t     cct;
} fx_rgb_lite_III_16bit_hsi_arg_t;

typedef struct
{
    uint8_t      cct_lower;
    uint8_t      cct_upper;
    uint8_t      gm;
    uint8_t      time_lower_limit;
    uint8_t      time_upper_limit;
} fx_rgb_lite_III_16bit_cct_setting_arg_t;   /*size: 3*/

typedef struct
{
    uint8_t      hsi_lower;
    uint8_t      hsi_upper;
    uint8_t      sat;
    uint8_t      cct;
    uint8_t      time_lower_limit;
    uint8_t      time_upper_limit;
} fx_rgb_lite_III_16bit_hsi_setting_arg_t;   /*size: 3*/

/*Paparazzi*/
typedef struct
{
    uint8_t         cct;
    uint8_t         gm;
    uint8_t         time_lower_limit;
    uint8_t         time_upper_limit;
} fx_rgb_lite_II_16bit_paparazzi_2_arg_t;     /*size: 6*/

/*Lightning3*/
typedef struct
{
    uint8_t                 mode;
    union
    {
        struct
        {
            uint8_t      cct;
            uint8_t      gm;
            uint8_t      time_lower_limit;
            uint8_t      time_upper_limit;
        } cct_setting;
        struct
        {
            uint8_t      hue;
            uint8_t      sat;
            uint8_t      cct;
            uint8_t      time_lower_limit;
            uint8_t      time_upper_limit;
        } hsi_setting;

    } arg;

} fx_rgb_lite_III_16bit_lightning_3_arg_t;

/*TV3*/
typedef struct
{
    uint8_t                 mode;
    union
    {
        struct
        {
            uint8_t      cct_lower_limit;
            uint8_t      cct_upper_limit;
            uint8_t      gm;
            uint8_t      time_lower_limit;
            uint8_t      time_upper_limit;

        } cct_setting;
        struct
        {
            uint8_t      hsi_lower_limit;
            uint8_t      hsi_upper_limit;
            uint8_t      sat;
            uint8_t      cct;
            uint8_t      time_lower_limit;
            uint8_t      time_upper_limit;

        } hsi_setting;

    } arg;

} fx_rgb_lite_III_16bit_tv_3_arg_t;

typedef struct
{
    uint8_t                 mode;
    union
    {
        struct
        {
            uint8_t      cct_lower_limit;
            uint8_t      cct_upper_limit;
            uint8_t      gm;
            uint8_t      frq;

        } cct_setting;
        struct
        {
            uint8_t      hsi_lower_limit;
            uint8_t      hsi_upper_limit;
            uint8_t      sat;
            uint8_t      cct;
            uint8_t      frq;
			
        } hsi_setting;

    } arg;

} fx_rgb_lite_III_16bit_fire_3_arg_t;

/*Fault_Bulb*/
typedef struct
{
    uint8_t                 mode;
    union
    {
        struct
        {
            uint8_t      cct;
            uint8_t      gm;
            uint8_t      time_lower_limit;
            uint8_t      time_upper_limit;
        } cct_setting;

        struct
        {
            uint8_t      sat;
            uint8_t      cct;
            uint8_t      hue;
            uint8_t      time_lower_limit;
            uint8_t      time_upper_limit;

        } hsi_setting;

    } arg;
} fx_rgb_lite_III_16bit_fault_bulb_3_arg_t;

/*Pulsing3*/
typedef struct
{
    uint8_t                 mode;
    union
    {
        struct
        {
            uint8_t      cct;
            uint8_t      gm;
            uint8_t      pluses;
        } cct;

        struct
        {
            uint8_t      sat;
            uint8_t      cct;
            uint8_t      hue;
            uint8_t      pluses;

        } hsi;

    } arg;

} fx_rgb_lite_III_16bit_pulsing_3_arg_t;

/*Cop_Car*/
typedef struct
{
    uint8_t     color;
    uint8_t     frq;

} fx_rgb_lite_III_16bit_cop_car_3_arg_t;

/*Color_Chase*/
typedef struct
{
    uint8_t     sat;
    uint8_t     frq;

} fx_rgb_lite_III_16bit_color_chase_arg_t;

/*Party_Lights*/
typedef struct
{
    uint8_t     sat;
    uint8_t frg;

} fx_rgb_lite_II_16bit_party_lights_2_arg_t;

/*Fireworks*/
typedef struct
{
    uint8_t     color;
    uint8_t     time_lower_limit;
    uint8_t     time_upper_limit;
} fx_rgb_lite_II_16bit_fireworks_2_arg_t;

typedef union
{
    fx_rgb_lite_II_16bit_paparazzi_2_arg_t       paparazzi2;
    fx_rgb_lite_III_16bit_lightning_3_arg_t      lightning3;
    fx_rgb_lite_III_16bit_tv_3_arg_t             tv3;
    fx_rgb_lite_III_16bit_fire_3_arg_t           fire3;
    fx_rgb_lite_III_16bit_fault_bulb_3_arg_t     fault_bulb3;
    fx_rgb_lite_III_16bit_pulsing_3_arg_t        pulsing3;
    fx_rgb_lite_III_16bit_cop_car_3_arg_t        copcar3;
    fx_rgb_lite_II_16bit_party_lights_2_arg_t    Party_Lights2;
    fx_rgb_lite_II_16bit_fireworks_2_arg_t       fireworks2;

} fx_rgb_lite_III_16bit_arg_t;



/*****************************************************************
*dmx_profile_fx_rgb_8bit_arg
*********************************************************************/
typedef struct
{
    uint8_t    cct;
    uint8_t     gm;
} fx_rgb_8bit_cct_arg_t;

/*-Effect HSI-*/
typedef struct
{
    uint8_t    hue;
    uint8_t     sat;
    uint8_t    cct;
} fx_rgb_8bit_hsi_arg_t;

/*-Effect GEL-*/
typedef struct
{
    uint8_t     cct;
    uint8_t     brand;
    uint8_t     type;
    uint8_t     color;
} fx_rgb_8bit_gel_arg_t;

/*Effect XY_Coordinate*/
typedef struct
{
    uint16_t    x;
    uint16_t    y;
} fx_rgb_8bit_xy_arg_t;

/*-Effect Light Source-*/
typedef struct
{
    uint8_t     type;
} fx_rgb_8bit_source_arg_t;

/*-Effect Light Source-*/
typedef struct
{
    uint8_t     r;
    uint8_t     g;
    uint8_t     b;
} fx_rgb_8bit_rgb_arg_t;

/*0x00：Club_Lights*/
typedef struct
{
    uint8_t     color;
    uint8_t     spd;
} fx_rgb_8bit_club_lights_arg_t;

/*0x01：Paparazzi*/
typedef struct
{
    uint8_t         cct;
    uint8_t         gm;
    uint8_t         frq;
} fx_rgb_8bit_paparazzi_arg_t;

/*0x02：Lightning*/
typedef struct
{
    uint8_t                 cct;
    uint8_t                 gm;
    uint8_t                 frq;
    uint8_t                 spd;

} fx_rgb_8bit_lightning_arg_t;

/*0x03：TV*/
typedef struct
{
    uint8_t    cct_rang;
    uint8_t    spd;
} fx_rgb_8bit_tv_arg_t;

/*0x04：Candle*/
typedef struct
{
    uint8_t     cct_rang;
    uint8_t     spd;
} fx_rgb_8bit_candle_arg_t;

/*0x05：Fire*/
typedef struct
{
    uint8_t     cct_rang;
    uint8_t     spd;
} fx_rgb_8bit_fire_arg_t;

/*0x06：Strobe*/
typedef struct
{
    uint8_t        mode;
    union
    {
        struct
        {
            uint8_t     cct;
            uint8_t     gm;
            uint8_t     spd;
        } cct;
        struct
        {
            uint8_t     hue;
            uint8_t     sat;
            uint8_t     cct;
            uint8_t     spd;
        } hsi;
        struct
        {
            uint8_t     cct;
            uint8_t     brand;
            uint8_t     type;
            uint8_t     color;
            uint8_t     spd;
        } gel;
        struct
        {
            uint8_t    x;
            uint8_t    y;
            uint8_t     spd;
        } xy;
        struct
        {
            uint8_t     type;
            uint8_t     spd;
        } source;
        struct
        {
            uint8_t     r;
            uint8_t     g;
            uint8_t     b;
            uint8_t     spd;
        } rgb;

    } arg;
} fx_rgb_8bit_strobe_arg_t;

/*0x07：Explosion*/
typedef struct
{
    uint8_t        mode;
    union
    {
        struct
        {
            uint8_t     cct;
            uint8_t     gm;
            uint8_t     decay;
        } cct;
        struct
        {
            uint8_t     hue;
            uint8_t     sat;
            uint8_t     cct;
            uint8_t     decay;
        } hsi;
        struct
        {
            uint8_t     cct;
            uint8_t     brand;
            uint8_t     type;
            uint8_t     color;
            uint8_t     decay;
        } gel;
        struct
        {
            uint8_t    x;
            uint8_t    y;
            uint8_t    decay;
        } xy;
        struct
        {
            uint8_t     type;
            uint8_t     decay;
        } source;
        struct
        {
            uint8_t     r;
            uint8_t     g;
            uint8_t     b;
            uint8_t     decay;
        } rgb;

    } arg;
} fx_rgb_8bit_explosion_arg_t;

/*0x08：Fault_Bulb*/
typedef struct
{
    uint8_t        mode;
    union
    {
        struct
        {
            uint8_t     cct;
            uint8_t     gm;
            uint8_t     frq;
            uint8_t     spd;
        } cct;
        struct
        {
            uint8_t     hue;
            uint8_t     sat;
            uint8_t     cct;
            uint8_t     frq;
            uint8_t     spd;
        } hsi;
        struct
        {
            uint8_t     cct;
            uint8_t     brand;
            uint8_t     type;
            uint8_t     color;
            uint8_t     frq;
            uint8_t     spd;
        } gel;
        struct
        {
            uint8_t    x;
            uint8_t    y;
            uint8_t     frq;
            uint8_t     spd;
        } xy;
        struct
        {
            uint8_t     type;
            uint8_t     frq;
            uint8_t     spd;
        } source;
        struct
        {
            uint8_t     r;
            uint8_t     g;
            uint8_t     b;
            uint8_t     frq;
            uint8_t     spd;

        } rgb;

    } arg;

} fx_rgb_8bit_fault_bulb_arg_t;

/*0x09：Pulsing*/
typedef struct
{
    uint8_t        mode;
    union
    {
        struct
        {
            uint8_t     cct;
            uint8_t     gm;
            uint8_t     frq;
            uint8_t     spd;
        } cct;
        struct
        {
            uint8_t     hue;
            uint8_t     sat;
            uint8_t     cct;
            uint8_t     frq;
            uint8_t     spd;
        } hsi;
        struct
        {
            uint8_t     cct;
            uint8_t     brand;
            uint8_t     type;
            uint8_t     color;
            uint8_t     frq;
            uint8_t     spd;
        } gel;
        struct
        {
            uint8_t    x;
            uint8_t    y;
            uint8_t     frq;
            uint8_t    spd;
        } xy;
        struct
        {
            uint8_t     type;
            uint8_t     frq;
            uint8_t     spd;
        } source;
        struct
        {
            uint8_t     r;
            uint8_t     g;
            uint8_t     b;
            uint8_t     frq;
            uint8_t     spd;
        } rgb;

    } arg;

} fx_rgb_8bit_pulsing_arg_t;

/*0x0A：Welding*/
typedef struct
{

    uint8_t        mode;
    union
    {
        struct
        {
            uint8_t     cct;
            uint8_t     gm;
            uint8_t     frq;
            uint8_t     min_brightness;
        } cct;
        struct
        {
            uint8_t     hue;
            uint8_t     sat;
            uint8_t     cct;
            uint8_t     frq;
            uint8_t     min_brightness;
        } hsi;
        struct
        {
            uint8_t     cct;
            uint8_t     brand;
            uint8_t     type;
            uint8_t     color;
            uint8_t     frq;
            uint8_t     min_brightness;
        } gel;
        struct
        {
            uint8_t    x;
            uint8_t    y;
            uint8_t     frq;
            uint8_t     min_brightness;
        } xy;
        struct
        {
            uint8_t     type;
            uint8_t     frq;
            uint8_t     min_brightness;
        } source;
        struct
        {
            uint8_t     r;
            uint8_t     g;
            uint8_t     b;
            uint8_t     frq;
            uint8_t     min_brightness;
        } rgb;
    } arg;

} fx_rgb_8bit_welding_arg_t;

/*0x0B：Cop_Car*/
typedef struct
{
    uint8_t     color;
    uint8_t     frq;
} fx_rgb_8bit_cop_car_arg_t;

/*0x0C：Color_Chase*/
typedef struct
{
    uint8_t     sat;
    uint8_t     spd;
} fx_rgb_8bit_color_chase_arg_t;

/*0x0D：Party_Lights*/
typedef struct
{
    uint8_t     sat;
    uint8_t     spd;
} fx_rgb_8bit_party_lights_arg_t;

/*0x0E：Fireworks*/
typedef struct
{
    uint8_t     color;
    uint8_t     frq;
} fx_rgb_8bit_fireworks_arg_t;

typedef struct
{
    uint8_t     light_board_a;
    union
    {
        fx_rgb_8bit_cct_arg_t     cct;
        fx_rgb_8bit_hsi_arg_t     hsi;
    } arg_a;

    uint8_t     light_board_b;
    union
    {
        fx_rgb_8bit_cct_arg_t     cct;
        fx_rgb_8bit_hsi_arg_t     hsi;
    } arg_b;

    uint8_t     light_board_c;
    union
    {
        fx_rgb_8bit_cct_arg_t     cct;
        fx_rgb_8bit_hsi_arg_t     hsi;
    } arg_c;

    uint8_t     light_board_d;
    union
    {
        fx_rgb_8bit_cct_arg_t     cct;
        fx_rgb_8bit_hsi_arg_t     hsi;
    } arg_d;

    uint8_t     frq;
    uint8_t     direction;

} fx_rgb_8bit_color_fade_arg_t;


typedef struct
{
    uint8_t     light_board_a;
    union
    {
        fx_rgb_8bit_cct_arg_t     cct;
        fx_rgb_8bit_hsi_arg_t     hsi;
    } arg_a;

    uint8_t     light_board_b;
    union
    {
        fx_rgb_8bit_cct_arg_t     cct;
        fx_rgb_8bit_hsi_arg_t     hsi;
    } arg_b;

    uint8_t     light_board_c;
    union
    {
        fx_rgb_8bit_cct_arg_t     cct;
        fx_rgb_8bit_hsi_arg_t     hsi;
    } arg_c;

    uint8_t     light_board_d;
    union
    {
        fx_rgb_8bit_cct_arg_t     cct;
        fx_rgb_8bit_hsi_arg_t     hsi;
    } arg_d;

    uint8_t     frq;

} fx_rgb_8bit_color_cycle_arg_t;

typedef struct
{

    uint8_t     light_board_a;
    union
    {
        fx_rgb_8bit_cct_arg_t     cct;
        fx_rgb_8bit_hsi_arg_t     hsi;
    } arg_a;

    uint8_t     light_board_b;
    union
    {
        fx_rgb_8bit_cct_arg_t     cct;
        fx_rgb_8bit_hsi_arg_t     hsi;
    } arg_b;

    uint8_t     light_board_c;
    union
    {
        fx_rgb_8bit_cct_arg_t     cct;
        fx_rgb_8bit_hsi_arg_t     hsi;
    } arg_c;

    uint8_t     light_board_d;
    union
    {
        fx_rgb_8bit_cct_arg_t     cct;
        fx_rgb_8bit_hsi_arg_t     hsi;
    } arg_d;

    uint8_t     frq;
    uint8_t     min_brightness;

} fx_rgb_8bit_color_gradient_arg_t;

typedef struct
{
    uint8_t     light_board_a;
    union
    {
        fx_rgb_8bit_cct_arg_t     cct;
        fx_rgb_8bit_hsi_arg_t     hsi;
    } arg_a;

    uint8_t     light_board_b;
    union
    {
        fx_rgb_8bit_cct_arg_t     cct;
        fx_rgb_8bit_hsi_arg_t     hsi;
    } arg_b;
	
    uint8_t     frq;
    uint8_t     direction;

} fx_rgb_8bit_one_color_chase_arg_t;

typedef union
{
    fx_rgb_8bit_club_lights_arg_t     club_lights;
    fx_rgb_8bit_paparazzi_arg_t     paparazzi;
    fx_rgb_8bit_lightning_arg_t     lightning;
    fx_rgb_8bit_tv_arg_t            tv;
    fx_rgb_8bit_candle_arg_t      candle;
    fx_rgb_8bit_fire_arg_t          fire;
    fx_rgb_8bit_strobe_arg_t        strobe;
    fx_rgb_8bit_explosion_arg_t   explosion;
    fx_rgb_8bit_fault_bulb_arg_t    fault_bulb;
    fx_rgb_8bit_pulsing_arg_t       pulsing;
    fx_rgb_8bit_welding_arg_t     welding;
    fx_rgb_8bit_cop_car_arg_t       copcar;
    fx_rgb_8bit_color_chase_arg_t   color_chase;
    fx_rgb_8bit_party_lights_arg_t    party_lights;
    fx_rgb_8bit_fireworks_arg_t     fireworks;
    fx_rgb_8bit_color_fade_arg_t     color_fade;
    fx_rgb_8bit_color_cycle_arg_t   color_cycle;
    fx_rgb_8bit_color_gradient_arg_t   color_gradient;
    fx_rgb_8bit_one_color_chase_arg_t  one_color_chase;

} fx_rgb_8bit_arg_t;



/*****************************************************************
*dmx_profile_fx_rgb_16bit_arg
*********************************************************************/
typedef struct
{

    uint16_t     cct;
    uint16_t     gm;
} fx_rgb_16bit_cct_arg_t;

/*-Effect HSI-*/
typedef struct
{

    uint16_t    hue;
    uint16_t    sat;
    uint16_t    cct;
} fx_rgb_16bit_hsi_arg_t;

/*-Effect GEL-*/
typedef struct
{

    uint8_t     cct;
    uint8_t     brand;
    uint8_t     type;
    uint8_t     color;
} fx_rgb_16bit_gel_arg_t;

/*Effect XY_Coordinate*/
typedef struct
{

    uint16_t    x;
    uint16_t    y;
} fx_rgb_16bit_xy_arg_t;

/*-Effect Light Source-*/
typedef struct
{

    uint8_t     type;

} fx_rgb_16bit_source_arg_t;

/*-Effect Light Source-*/
typedef struct
{

    uint16_t     r;
    uint16_t     g;
    uint16_t     b;
} fx_rgb_16bit_rgb_arg_t;


/*0x00：Club_Lights*/
typedef struct
{
    uint8_t     color;
    uint8_t     spd;
} fx_rgb_16bit_club_lights_arg_t;

/*：Paparazzi*/
typedef struct
{
    uint16_t    cct;
    uint16_t     gm;
    uint8_t    frq;
} fx_rgb_16bit_paparazzi_arg_t;

/*：Lightning*/
typedef struct
{
	
    uint16_t                cct;
    uint16_t                gm;
    uint8_t                 frq;
    uint8_t                 spd;
} fx_rgb_16bit_lightning_arg_t;

/*0x03：TV*/
typedef struct
{

    uint8_t    cct_rang;
    uint8_t    spd;
} fx_rgb_16bit_tv_arg_t;

/*0x04：Candle*/
typedef struct
{

    uint8_t    cct_range;
    uint8_t     spd;
} fx_rgb_16bit_candle_arg_t;           /*size: 5*/

/*0x05：Fire*/
typedef struct
{

    uint8_t    cct_rang;
    uint8_t     spd;
} fx_rgb_16bit_fire_arg_t;

/*0x06：Strobe*/
typedef struct
{
    uint8_t        mode;
    union
    {
        struct
        {
            uint16_t     cct;
            uint16_t     gm;
            uint8_t     spd;
        } cct;
        struct
        {
            uint16_t     hue;
            uint16_t     sat;
            uint16_t     cct;
            uint8_t     spd;
        } hsi;
        struct
        {
            uint8_t     cct;
            uint8_t     brand;
            uint8_t     type;
            uint8_t     color;
            uint8_t     spd;
        } gel;
        struct
        {
            uint16_t    x;
            uint16_t    y;
            uint8_t    spd;
        } xy;
        struct
        {
            uint8_t     type;
            uint8_t     spd;
        } source;
        struct
        {
            uint16_t     r;
            uint16_t     g;
            uint16_t     b;
            uint8_t     spd;
        } rgb;

    } arg;
} fx_rgb_16bit_strobe_arg_t;                    /*size: 10*/

/*0x07：Explosion*/
typedef struct
{
    uint8_t        mode;
    union
    {
        struct
        {
            uint16_t     cct;
            uint16_t     gm;
            uint8_t     decay;
        } cct;
        struct
        {
            uint16_t     hue;
            uint16_t     sat;
            uint16_t     cct;
            uint8_t     decay;
        } hsi;
        struct
        {
            uint8_t     cct;
            uint8_t     brand;
            uint8_t     type;
            uint8_t     color;
            uint8_t     decay;
        } gel;
        struct
        {
            uint16_t    x;
            uint16_t    y;
            uint8_t    decay;
        } xy;
        struct
        {
            uint8_t     type;
            uint8_t     decay;
        } source;
        struct
        {
            uint16_t     r;
            uint16_t     g;
            uint16_t     b;
            uint8_t     decay;
        } rgb;

    } arg;
} fx_rgb_16bit_explosion_arg_t;                  /*size: 11*/

/*0x08：Fault_Bulb*/
typedef struct
{
    uint8_t        mode;
    union
    {
        struct
        {
            uint16_t     cct;
            uint16_t     gm;
            uint8_t     frq;
            uint8_t     spd;
        } cct;
        struct
        {
            uint16_t     hue;
            uint16_t     sat;
            uint16_t     cct;
            uint8_t     frq;
            uint8_t     spd;
        } hsi;
        struct
        {
            uint8_t     cct;
            uint8_t     brand;
            uint8_t     type;
            uint8_t     color;
            uint8_t     frq;
            uint8_t     spd;
        } gel;
        struct
        {
            uint16_t    x;
            uint16_t    y;
            uint8_t     frq;
            uint8_t     spd;
        } xy;
        struct
        {
            uint8_t     type;
            uint8_t     frq;
            uint8_t     spd;
        } source;
        struct
        {
            uint16_t     r;
            uint16_t     g;
            uint16_t     b;
            uint8_t     frq;
            uint8_t     spd;

        } rgb;

    } arg;

} fx_rgb_16bit_fault_bulb_arg_t;               /*size: 11*/

/*0x09：Pulsing*/
typedef struct
{
    uint8_t        mode;           /*0x00:CCT；0x01:HSI；0x02:GEL；0x03:XY；0x04:Source*/
    union
    {
        struct
        {
            uint16_t     cct;
            uint16_t     gm;
            uint8_t     frq;
            uint8_t     spd;
        } cct;
        struct
        {
            uint16_t     hue;
            uint16_t     sat;
            uint16_t     cct;
            uint8_t     frq;
            uint8_t     spd;
        } hsi;
        struct
        {
            uint8_t     cct;
            uint8_t     brand;
            uint8_t     type;
            uint8_t     color;
            uint8_t     frq;
            uint8_t     spd;
        } gel;
        struct
        {
            uint16_t    x;
            uint16_t    y;
            uint8_t     frq;
            uint8_t    spd;
        } xy;
        struct
        {
            uint8_t     type;
            uint8_t     frq;
            uint8_t     spd;
        } source;
        struct
        {
            uint16_t     r;
            uint16_t     g;
            uint16_t     b;
            uint8_t     frq;
            uint8_t     spd;
        } rgb;

    } arg;

} fx_rgb_16bit_pulsing_arg_t;                   /*size: 11*/

/*0x0A：Welding*/
typedef struct
{
    uint8_t        mode;
    union
    {
        struct
        {
            uint16_t     cct;
            uint16_t     gm;
            uint8_t     frq;
            uint8_t     min_brightness;
        } cct;
        struct
        {
            uint16_t     hue;
            uint16_t     sat;
            uint16_t     cct;
            uint8_t     frq;
            uint8_t     min_brightness;
        } hsi;
        struct
        {
            uint8_t     cct;
            uint8_t     brand;
            uint8_t     type;
            uint8_t     color;
            uint8_t     frq;
            uint8_t     min_brightness;
        } gel;
        struct
        {
            uint16_t    x;
            uint16_t    y;
            uint8_t     frq;
            uint8_t     min_brightness;
        } xy;
        struct
        {
            uint8_t     type;
            uint8_t     frq;
            uint8_t     min_brightness;
        } source;
        struct
        {
            uint16_t     r;
            uint16_t     g;
            uint16_t     b;
            uint8_t     frq;
            uint8_t     min_brightness;
        } rgb;
    } arg;

} fx_rgb_16bit_welding_arg_t;                   /*size: 12*/

/*0x0B：Cop_Car*/
typedef struct
{

    uint8_t     color;
    uint8_t     frq;
} fx_rgb_16bit_cop_car_arg_t;       /*size: 4*/

///*0x0C：Color_Chase*/
typedef struct
{
    uint8_t     sat;            /*饱和度：<0-100>*/
    uint8_t     spd;
} fx_rgb_16bit_color_chase_arg_t;   /*size: 4*/

/*0x0D：Party_Lights*/
typedef struct
{

    uint8_t     sat;            /*饱和度：<0-100>*/
    uint8_t     spd;
} fx_rgb_16bit_party_lights_arg_t;  /*size: 4*/

/*0x0E：Fireworks*/
typedef struct
{

    uint8_t     color;           /*光类型：0->CCT，1->HUE，2->CCT+HUE*/
    uint8_t     frq;
} fx_rgb_16bit_fireworks_arg_t;

typedef struct
{

    uint8_t     light_board_a;
    union
    {
        fx_rgb_16bit_cct_arg_t     cct;
        fx_rgb_16bit_hsi_arg_t     hsi;
    } arg_a;

    uint8_t     light_board_b;
    union
    {
        fx_rgb_16bit_cct_arg_t     cct;
        fx_rgb_16bit_hsi_arg_t     hsi;
    } arg_b;

    uint8_t     light_board_c;
    union
    {
        fx_rgb_16bit_cct_arg_t     cct;
        fx_rgb_16bit_hsi_arg_t     hsi;
    } arg_c;

    uint8_t     light_board_d;
    union
    {
        fx_rgb_16bit_cct_arg_t     cct;
        fx_rgb_16bit_hsi_arg_t     hsi;
    } arg_d;

    uint8_t     frq;
    uint8_t     direction;

} fx_rgb_16bit_color_fade_arg_t;

typedef struct
{

    uint8_t     light_board_a;
    union
    {
        fx_rgb_16bit_cct_arg_t     cct;
        fx_rgb_16bit_hsi_arg_t     hsi;
    } arg_a;

    uint8_t     light_board_b;
    union
    {
        fx_rgb_16bit_cct_arg_t     cct;
        fx_rgb_16bit_hsi_arg_t     hsi;
    } arg_b;

    uint8_t     light_board_c;
    union
    {
        fx_rgb_16bit_cct_arg_t     cct;
        fx_rgb_16bit_hsi_arg_t     hsi;
    } arg_c;

    uint8_t     light_board_d;
    union
    {
        fx_rgb_16bit_cct_arg_t     cct;
        fx_rgb_16bit_hsi_arg_t     hsi;
    } arg_d;

    uint8_t     frq;

} fx_rgb_16bit_color_cycle_arg_t;

typedef struct
{

    uint8_t     light_board_a;
    union
    {
        fx_rgb_16bit_cct_arg_t     cct;
        fx_rgb_16bit_hsi_arg_t     hsi;
    } arg_a;

    uint8_t     light_board_b;
    union
    {
        fx_rgb_16bit_cct_arg_t     cct;
        fx_rgb_16bit_hsi_arg_t     hsi;
    } arg_b;

    uint8_t     light_board_c;
    union
    {
        fx_rgb_16bit_cct_arg_t     cct;
        fx_rgb_16bit_hsi_arg_t     hsi;
    } arg_c;

    uint8_t     light_board_d;
    union
    {
        fx_rgb_16bit_cct_arg_t     cct;
        fx_rgb_16bit_hsi_arg_t     hsi;
    } arg_d;

    uint8_t     frq;
    uint8_t     min_brightness;

} fx_rgb_16bit_color_gradient_arg_t;

typedef struct
{

    uint8_t     light_board_a;
    union
    {
        fx_rgb_16bit_cct_arg_t     cct;
        fx_rgb_16bit_hsi_arg_t     hsi;
    } arg_a;

    uint8_t     light_board_b;
    union
    {
        fx_rgb_16bit_cct_arg_t     cct;
        fx_rgb_16bit_hsi_arg_t     hsi;
    } arg_b;

    uint8_t     frq;
    uint8_t     direction;

} fx_rgb_16bit_one_color_chase_arg_t;

typedef union
{
    fx_rgb_16bit_club_lights_arg_t     club_lights;
    fx_rgb_16bit_paparazzi_arg_t     paparazzi;
    fx_rgb_16bit_lightning_arg_t     lightning;
    fx_rgb_16bit_tv_arg_t            tv;
    fx_rgb_16bit_candle_arg_t      candle;
    fx_rgb_16bit_fire_arg_t          fire;
    fx_rgb_16bit_strobe_arg_t        strobe;
    fx_rgb_16bit_explosion_arg_t   explosion;
    fx_rgb_16bit_fault_bulb_arg_t    fault_bulb;
    fx_rgb_16bit_pulsing_arg_t       pulsing;
    fx_rgb_16bit_welding_arg_t     welding;
    fx_rgb_16bit_cop_car_arg_t       copcar;
    fx_rgb_16bit_color_chase_arg_t   color_chase;
    fx_rgb_16bit_party_lights_arg_t    party_lights;
    fx_rgb_16bit_fireworks_arg_t     fireworks;
    fx_rgb_16bit_color_fade_arg_t     color_fade;
    fx_rgb_16bit_color_cycle_arg_t   color_cycle;
    fx_rgb_16bit_color_gradient_arg_t   color_gradient;
    fx_rgb_16bit_one_color_chase_arg_t  one_color_chase;

} fx_rgb_16bit_arg_t;


/*****************************************************************
*dmx_profile_fx_pixel_8bit
*********************************************************************/
typedef struct
{

    uint8_t    cct;
    uint8_t     gm;
} fx_pixel_8bit_cct_arg_t;

/*-Effect HSI-*/
typedef struct
{

    uint8_t    hue;
    uint8_t     sat;
    uint8_t    cct;
} fx_pixel_8bit_hsi_arg_t;

/*0x00：Club_Lights*/
typedef struct
{
    uint8_t     color_choose;
    uint8_t     move;
    uint8_t     spd;
    uint8_t     color_1_move;
    union
    {
        fx_pixel_8bit_cct_arg_t     cct;
        fx_pixel_8bit_hsi_arg_t     hsi;
    } arg1;

    uint8_t     color_2_move;
    union
    {
        fx_pixel_8bit_cct_arg_t     cct;
        fx_pixel_8bit_hsi_arg_t     hsi;
    } arg2;


    uint8_t     color_3_move;
    union
    {
        fx_pixel_8bit_cct_arg_t     cct;
        fx_pixel_8bit_hsi_arg_t     hsi;
    } arg3;

    uint8_t     color_4_move;
    union
    {
        fx_pixel_8bit_cct_arg_t     cct;
        fx_pixel_8bit_hsi_arg_t     hsi;
    } arg4;


} fx_pixel_coloe_fade_8bit_arg_t;

typedef struct
{
    uint8_t     color_choose;
    uint8_t     spd;
    uint8_t     transition;
    uint8_t     move;

    uint8_t     color_1_move;
    union
    {
        fx_pixel_8bit_cct_arg_t     cct;
        fx_pixel_8bit_hsi_arg_t     hsi;
    } arg1;

    uint8_t     color_2_move;
    union
    {
        fx_pixel_8bit_cct_arg_t     cct;
        fx_pixel_8bit_hsi_arg_t     hsi;
    } arg2;


    uint8_t     color_3_move;
    union
    {
        fx_pixel_8bit_cct_arg_t     cct;
        fx_pixel_8bit_hsi_arg_t     hsi;
    } arg3;

    uint8_t     color_4_move;
    union
    {
        fx_pixel_8bit_cct_arg_t     cct;
        fx_pixel_8bit_hsi_arg_t     hsi;
    } arg4;


} fx_pixel_coloe_cycle_8bit_arg_t;


typedef struct
{
    uint8_t     pixel_size;
    uint8_t     spd;
    uint8_t     departure;
    union
    {
        uint8_t     move_1_way;
        uint8_t     move_2_way;
    } move_choose;
    union
    {
        struct
        {
            uint8_t     pixel_1_mode;
            union
            {
                fx_pixel_8bit_cct_arg_t     cct;
                fx_pixel_8bit_hsi_arg_t     hsi;
            } arg1;

            uint8_t     background_color_int;
            uint8_t     background_color_mode;

            union
            {
                fx_pixel_8bit_cct_arg_t     cct;
                fx_pixel_8bit_hsi_arg_t     hsi;
            } arg2;
        } way1;
        struct
        {
            uint8_t     pixel_1_mode;
            union
            {
                fx_pixel_8bit_cct_arg_t     cct;
                fx_pixel_8bit_hsi_arg_t     hsi;
            } arg1;
            uint8_t     pixel_2_mode;
            union
            {
                fx_pixel_8bit_cct_arg_t     cct;
                fx_pixel_8bit_hsi_arg_t     hsi;
            } arg2;
            uint8_t     background_color_int;
            uint8_t     background_color_mode;
            union
            {
                fx_pixel_8bit_cct_arg_t     cct;
                fx_pixel_8bit_hsi_arg_t     hsi;
            } arg3;
        } way2;
    } way_mode;

} fx_pixel_one_pixel_chase_8bit_arg_t;

typedef struct
{
    uint8_t     pixel_size;
    uint8_t     spd;
    uint8_t     departure;
    union
    {
        uint8_t     move_1_way;
        uint8_t     move_2_way;
    } move_choose;
    union
    {
        struct
        {
            uint8_t     pixel_1_mode;
            union
            {
                fx_pixel_8bit_cct_arg_t     cct;
                fx_pixel_8bit_hsi_arg_t     hsi;
            } arg1;

            uint8_t     pixel_2_mode;
            union
            {
                fx_pixel_8bit_cct_arg_t     cct;
                fx_pixel_8bit_hsi_arg_t     hsi;
            } arg2;

            uint8_t     background_color_int;
            uint8_t     background_color_mode;
            union
            {
                fx_pixel_8bit_cct_arg_t     cct;
                fx_pixel_8bit_hsi_arg_t     hsi;
            } arg3;
        } way1;
        struct
        {

            uint8_t     pixel_1_mode;
            union
            {
                fx_pixel_8bit_cct_arg_t     cct;
                fx_pixel_8bit_hsi_arg_t     hsi;
            } arg1;

            uint8_t     pixel_2_mode;
            union
            {
                fx_pixel_8bit_cct_arg_t     cct;
                fx_pixel_8bit_hsi_arg_t     hsi;
            } arg2;

            uint8_t     pixel_3_mode;
            union
            {
                fx_pixel_8bit_cct_arg_t     cct;
                fx_pixel_8bit_hsi_arg_t     hsi;
            } arg3;

            uint8_t     pixel_4_mode;
            union
            {
                fx_pixel_8bit_cct_arg_t     cct;
                fx_pixel_8bit_hsi_arg_t     hsi;
            } arg4;


            uint8_t     background_color_int;
            uint8_t     background_color_mode;
            union
            {
                fx_pixel_8bit_cct_arg_t     cct;
                fx_pixel_8bit_hsi_arg_t     hsi;
            } arg5;
        } way2;
    } way_mode;

} fx_pixel_two_pixel_chase_8bit_arg_t;

typedef struct
{
    uint8_t     pixel_size;
    uint8_t     spd;
    uint8_t     departure;
    union
    {
        uint8_t     move_1_way;
        uint8_t     move_2_way;
    } move_choose;
    union
    {
        struct
        {
            uint8_t     pixel_1_mode;
            union
            {
                fx_pixel_8bit_cct_arg_t     cct;
                fx_pixel_8bit_hsi_arg_t     hsi;
            } arg1;

            uint8_t     pixel_2_mode;
            union
            {
                fx_pixel_8bit_cct_arg_t     cct;
                fx_pixel_8bit_hsi_arg_t     hsi;
            } arg2;

            uint8_t     pixel_3_mode;
            union
            {
                fx_pixel_8bit_cct_arg_t     cct;
                fx_pixel_8bit_hsi_arg_t     hsi;
            } arg3;

            uint8_t     background_color_int;
            uint8_t     background_color_mode;
            union
            {
                fx_pixel_8bit_cct_arg_t     cct;
                fx_pixel_8bit_hsi_arg_t     hsi;
            } arg4;
        } way1;
        struct
        {

            uint8_t     pixel_1_mode;
            union
            {
                fx_pixel_8bit_cct_arg_t     cct;
                fx_pixel_8bit_hsi_arg_t     hsi;
            } arg1;

            uint8_t     pixel_2_mode;
            union
            {
                fx_pixel_8bit_cct_arg_t     cct;
                fx_pixel_8bit_hsi_arg_t     hsi;
            } arg2;

            uint8_t     pixel_3_mode;
            union
            {
                fx_pixel_8bit_cct_arg_t     cct;
                fx_pixel_8bit_hsi_arg_t     hsi;
            } arg3;

            uint8_t     pixel_4_mode;
            union
            {
                fx_pixel_8bit_cct_arg_t     cct;
                fx_pixel_8bit_hsi_arg_t     hsi;
            } arg4;


            uint8_t     pixel_5_mode;
            union
            {
                fx_pixel_8bit_cct_arg_t     cct;
                fx_pixel_8bit_hsi_arg_t     hsi;
            } arg5;

            uint8_t     pixel_6_mode;
            union
            {
                fx_pixel_8bit_cct_arg_t     cct;
                fx_pixel_8bit_hsi_arg_t     hsi;
            } arg6;


            uint8_t     background_color_int;
            uint8_t     background_color_mode;
            union
            {
                fx_pixel_8bit_cct_arg_t     cct;
                fx_pixel_8bit_hsi_arg_t     hsi;
            } arg7;
        } way2;
    } way_mode;

} fx_pixel_three_pixel_chase_8bit_arg_t;

typedef struct
{
    uint8_t     move;
    uint8_t     spd;

} fx_rainbow_8bit_arg_t;

typedef struct
{
    uint8_t     upper_limit_int;
    uint8_t     frq;
    uint8_t     move;
    uint8_t     fire_move;
    union
    {
        fx_pixel_8bit_cct_arg_t     cct;
        fx_pixel_8bit_hsi_arg_t     hsi;
    } arg1;

    uint8_t     background_color_int;
    uint8_t     background_color_mode;
    union
    {
        fx_pixel_8bit_cct_arg_t     cct;
        fx_pixel_8bit_hsi_arg_t     hsi;
    } arg2;

} fx_pixel_file_8bit_arg_t;

typedef union
{

    fx_pixel_coloe_fade_8bit_arg_t             coloe_fade;
    fx_pixel_coloe_cycle_8bit_arg_t            coloe_cycle;
    fx_pixel_one_pixel_chase_8bit_arg_t        one_pixel_chase;
    fx_pixel_two_pixel_chase_8bit_arg_t        two_pixel_chase;
    fx_pixel_three_pixel_chase_8bit_arg_t      three_pixel_chase;
    fx_rainbow_8bit_arg_t                      rainbow;
    fx_pixel_file_8bit_arg_t                   pixel_file;

} fx_pixel_8bit_arg_t;




typedef struct
{
    uint8_t intensity;
    uint8_t strobe;
} profile_0101_cct_w_8bit_t;

typedef struct
{
    uint16_t intensity;
    uint8_t strobe;
} profile_0102_cct_w_16bit_t;

typedef struct
{
    uint8_t intensity;
    uint8_t cct;
    uint8_t strobe;
} profile_0103_cct_lite_8bit_t;

typedef struct
{
    uint16_t intensity;
    uint16_t cct;
    uint8_t strobe;
} profile_0104_cct_lite_16bit_t;

typedef struct
{
    uint8_t intensity;
    uint8_t cct;
    uint8_t gm;
    uint8_t strobe;
} profile_0105_cct_gm_8bit_t;

typedef struct
{
    uint16_t intensity;
    uint16_t cct;
    uint16_t  gm;
    uint8_t strobe;
} profile_0106_cct_gm_16bit_t;

typedef struct
{
    uint8_t intensity;
    uint8_t red;
    uint8_t green;
    uint8_t blue;
    uint8_t strobe;
} profile_0201_rgb_8bit_t;

typedef struct
{
    uint16_t intensity;
    uint16_t red;
    uint16_t green;
    uint16_t blue;
    uint8_t strobe;
} profile_0202_rgb_16bit_t;

typedef struct
{
    uint8_t intensity;
    uint8_t red;
    uint8_t green;
    uint8_t blue;
    uint8_t white;
    uint8_t strobe;
} profile_0203_rgbw_8bit_t;

typedef struct
{
    uint16_t intensity;
    uint16_t red;
    uint16_t green;
    uint16_t blue;
    uint16_t white;
    uint8_t strobe;
} profile_0204_rgbw_16bit_t;

typedef struct
{
//    uint8_t intensity;
	uint8_t warm_white;
	uint8_t cold_white;
    uint8_t red;
    uint8_t green;
    uint8_t blue;
    uint8_t strobe;
} profile_0205_rgbww_8bit_t;


typedef struct
{
//    uint16_t intensity;
	uint16_t warm_white;
	uint16_t cold_white;
    uint16_t red;
    uint16_t green;
    uint16_t blue;
    uint8_t strobe;
} profile_0206_rgbww_16bit_t;

typedef struct
{
    uint8_t intensity;
    uint8_t red;
    uint8_t green;
    uint8_t blue;
    uint8_t amber;
    uint8_t cyan;
    uint8_t lime;
    uint8_t strobe;
} profile_0207_rgbacl_8bit_t;


typedef struct
{
    uint16_t intensity;
    uint16_t red;
    uint16_t green;
    uint16_t blue;
    uint16_t amber;
    uint16_t cyan;
    uint16_t lime;
    uint8_t strobe;
} profile_0208_rgbacl_16bit_t;


typedef struct
{
    uint8_t intensity;
    uint8_t hue;
    uint8_t  sat;
    uint8_t  strobe;
} profile_0301_hsi_8bit_t;

typedef struct
{
    uint16_t intensity;
    uint16_t hue;
    uint16_t sat;
    uint8_t strobe;
} profile_0302_hsi_16bit_t;

typedef struct
{
    uint8_t intensity;
    uint8_t hue;
    uint8_t sat;
    uint8_t cct;
    uint8_t strobe;
} profile_0303_advhsi_8bit_t;

typedef struct
{
    uint16_t intensity;
    uint16_t hue;
    uint16_t sat;
    uint16_t cct;
    uint8_t strobe;
} profile_0304_advhsi_16bit_t;

typedef struct
{
    uint8_t intensity;
    uint8_t x;
    uint8_t y;
    uint8_t strobe;
} profile_0401_xy_8bit_t;

typedef struct
{
    uint16_t intensity;
    uint16_t x;
    uint16_t y;
    uint8_t strobe;
} profile_0402_xy_16bit_t;

typedef struct
{
    uint8_t intensity;
    uint8_t cct;
    uint8_t brand;
    uint8_t type;
    uint8_t color;
    uint8_t strobe;
} profile_0501_gel_8bit_t;

typedef struct
{
    uint16_t intensity;
    uint8_t cct;
    uint8_t brand;
    uint8_t type;
    uint8_t color;
    uint8_t strobe;
} profile_0502_gel_16bit_t;

typedef struct
{
    uint8_t intensity;
    uint8_t type;
    uint8_t x;
    uint8_t y;
    uint8_t strobe;
} profile_0601_source_8bit_t;

typedef struct
{
    uint16_t intensity;
    uint8_t type;
    uint8_t x;
    uint8_t y;
    uint8_t strobe;
} profile_0602_source_16bit_t;

typedef struct
{
    uint8_t intensity;
    uint8_t state;
    uint8_t type;
    uint8_t frq;

} profile_0701_fx_w_8bit_t;

typedef struct
{
    uint16_t intensity;
    uint8_t state;
    uint8_t type;
    uint8_t frq;

} profile_0702_fx_w_16bit_t;

typedef struct
{
    uint8_t intensity;
//    uint8_t     cct;
    uint8_t     state;
    uint8_t     type;
    uint8_t     frq;
} profile_0703_fx_lite_8bit_t;


typedef struct
{
    uint16_t intensity;
//    uint16_t     cct;
    uint8_t     state;
    uint8_t     type;
    uint8_t     frq;
} profile_0704_fx_lite_16bit_t;

typedef struct
{
    uint8_t intensity;
    uint8_t                     state;
    uint8_t                     type;
    fx_rgb_lite_I_8bit_arg_t    arg;

} profile_0705_fx_rgb_lite_I_8bit_t;


typedef struct
{
    uint8_t intensity;
    uint8_t                         state;
    uint8_t                         type;
    fx_rgb_lite_III_16bit_arg_t     arg;

} profile_0706_fx_rgb_lite_III_8bit_t;


typedef struct
{
    uint8_t intensity;
    uint8_t                 state;
    uint8_t                 type;
    fx_rgb_8bit_arg_t       arg;

} profile_0707_fx_rgb_8bit_arg_t;


typedef struct
{
    uint16_t intensity;
    uint8_t                 state;
    uint8_t                 type;
    fx_rgb_16bit_arg_t       arg;

} profile_0708_fx_rgb_16bit_arg_t;

typedef struct
{
    uint8_t intensity;
    uint8_t                 state;
    uint8_t                 type;
    fx_pixel_8bit_arg_t      arg;

} profile_0709_pixel_8bit_arg_t;

typedef struct
{
    uint8_t intensity;
    uint8_t                 cct;
    uint8_t                 gm;
    uint8_t                 ratio;
    uint8_t                 r;
    uint8_t                 g;
    uint8_t                 b;
    uint8_t                 strobe;

} profile_0801_cct_and_rgb_8bit_arg_t;


typedef struct
{
    uint16_t intensity;
    uint16_t                 cct;
    uint16_t                 gm;
    uint16_t                 ratio;
    uint16_t                 r;
    uint16_t                 g;
    uint16_t                 b;
    uint8_t                  strobe;

} profile_0802_cct_and_rgb_16bit_arg_t;

typedef struct
{
    uint8_t intensity;
    uint8_t                 cct;
    uint8_t                 gm;
    uint8_t                 ratio;
    uint8_t                 r;
    uint8_t                 g;
    uint8_t                 b;
    uint8_t                 w;
    uint8_t                 strobe;

} profile_0803_cct_and_rgbw_8bit_arg_t;


typedef struct
{
    uint16_t intensity;
    uint16_t                 cct;
    uint16_t                 gm;
    uint16_t                 ratio;
    uint16_t                 r;
    uint16_t                 g;
    uint16_t                 b;
    uint16_t                 w;
    uint8_t                  strobe;

} profile_0804_cct_and_rgbw_16bit_arg_t;

typedef struct
{
    uint8_t                 intensity;
    uint8_t                 cct;
    uint8_t                 gm;
    uint8_t                 ratio;
    uint8_t                 r;
    uint8_t                 g;
    uint8_t                 b;
    uint8_t                 cw;
    uint8_t                 ww ;
    uint8_t                 strobe;

} profile_0805_cct_and_rgbww_8bit_arg_t;

typedef struct
{
    uint16_t intensity;
    uint16_t                 cct;
    uint16_t                 gm;
    uint16_t                 ratio;
    uint16_t                 r;
    uint16_t                 g;
    uint16_t                 b;
    uint16_t                 cw;
    uint16_t                 ww ;
    uint8_t                  strobe;

} profile_0806_cct_and_rgbww_16bit_arg_t;

typedef struct
{
    uint8_t intensity;
    uint8_t                  cct;
    uint8_t                  gm;
    uint8_t                  ratio;
    uint8_t                  r;
    uint8_t                  g;
    uint8_t                  b;
    uint8_t                  amber;
    uint8_t                  cyan ;
    uint8_t                  strobe;

} profile_0807_cct_and_rgbacl_8bit_arg_t;


typedef struct
{
    uint16_t intensity;
    uint16_t                  cct;
    uint16_t                  gm;
    uint16_t                  ratio;
    uint16_t                  r;
    uint16_t                  g;
    uint16_t                  b;
    uint16_t                  amber;
    uint16_t                  cyan ;
    uint8_t                   strobe;

} profile_0808_cct_and_rgbacl_16bit_arg_t;

typedef struct
{
    uint8_t intensity;
    uint8_t                  cct;
    uint8_t                  gm;
    uint8_t                  ratio;
    uint8_t                  hue;
    uint8_t                  sat;
    uint8_t                  strobe;

} profile_0901_cct_and_hsi_8bit_arg_t;


typedef struct
{
    uint16_t intensity;
    uint16_t                 cct;
    uint16_t                 gm;
    uint16_t                 ratio;
    uint16_t                 hue;
    uint16_t                 sat;
    uint8_t                  strobe;

} profile_0902_cct_and_hsi_16bit_arg_t;

typedef struct
{
    struct
    {
        uint8_t                 lightness;
        uint8_t                 cct;
        uint8_t                 gm;
        uint8_t                 ratio;
        uint8_t                 r;
        uint8_t                 g;
        uint8_t                 b;

    } engine[PROFILE_LIGHT_ENGINE_NUM];

} profile_2001_le_cct_and_rgb_8bit_arg_t;

typedef struct
{
    struct
    {
        uint16_t                  lightness;
        uint16_t                  cct;
        uint16_t                  gm;
        uint16_t                  ratio;
        uint16_t                  r;
        uint16_t                  g;
        uint16_t                  b;

    } engine[PROFILE_LIGHT_ENGINE_NUM];

} profile_2002_le_cct_and_rgb_16bit_arg_t;



typedef struct
{
    struct
    {
        uint8_t                  lightness;
        uint8_t                  cct;
        uint8_t                  gm;
        uint8_t                  ratio;
        uint8_t                  r;
        uint8_t                  g;
        uint8_t                  b;
        uint8_t                  w;

    } engine[PROFILE_LIGHT_ENGINE_NUM];

} profile_2003_le_cct_and_rgbw_8bit_arg_t;

typedef struct
{
    struct
    {
        uint16_t                  lightness;
        uint16_t                  cct;
        uint16_t                  gm;
        uint16_t                  ratio;
        uint16_t                  r;
        uint16_t                  g;
        uint16_t                  b;
        uint16_t                  w;

    } engine[PROFILE_LIGHT_ENGINE_NUM];

} profile_2004_le_cct_and_rgbw_16bit_arg_t;

typedef struct
{
    struct
    {
        uint8_t                  lightness;
        uint8_t                  cct;
        uint8_t                  gm;
        uint8_t                  ratio;
        uint8_t                  r;
        uint8_t                  g;
        uint8_t                  b;
        uint8_t                 cw;
        uint8_t                 ww;

    } engine[PROFILE_LIGHT_ENGINE_NUM];

} profile_2005_le_cct_and_rgbww_8bit_arg_t;

typedef struct
{
    struct
    {
        uint16_t                  lightness;
        uint16_t                  cct;
        uint16_t                  gm;
        uint16_t                  ratio;
        uint16_t                  r;
        uint16_t                  g;
        uint16_t                  b;
        uint16_t                  cw;
        uint16_t                 ww;

    } engine[PROFILE_LIGHT_ENGINE_NUM];

} profile_2006_le_cct_and_rgbww_16bit_arg_t;

typedef struct
{
    struct
    {
        uint8_t                  lightness;
        uint8_t                  cct;
        uint8_t                  gm;
        uint8_t                  ratio;
        uint8_t                  r;
        uint8_t                  g;
        uint8_t                  b;
        uint8_t                  amber;
        uint8_t                  cyan;
        uint8_t                  lime;

    } engine[PROFILE_LIGHT_ENGINE_NUM];

} profile_2007_le_cct_and_rgbacl_8bit_arg_t;

typedef struct
{
    struct
    {
        uint16_t                  lightness;
        uint16_t                  cct;
        uint16_t                  gm;
        uint16_t                  ratio;
        uint16_t                  r;
        uint16_t                  g;
        uint16_t                  b;
        uint16_t                  amber;
        uint16_t                  cyan;
        uint16_t                  lime;

    } engine[PROFILE_LIGHT_ENGINE_NUM];

} profile_2008_le_cct_and_rgbacl_16bit_arg_t;

typedef struct
{
    struct
    {
        uint8_t                  lightness;
        uint8_t                  hue;
        uint8_t                  sat;

    } engine[PROFILE_LIGHT_ENGINE_NUM];

} profile_2101_le_hsi_8bit_arg_t;

typedef struct
{
    struct
    {
        uint16_t                  lightness;
        uint16_t                  hue;
        uint16_t                  sat;

    } engine[PROFILE_LIGHT_ENGINE_NUM];

} profile_2102_le_hsi_16bit_arg_t;

typedef struct
{
    struct
    {
        uint8_t                   lightness;
        uint8_t                   hue;
        uint8_t                   sat;
        uint8_t                   cct;

    } engine[PROFILE_LIGHT_ENGINE_NUM];

} profile_2103_le_advhsi_8bit_arg_t;

typedef struct
{
    struct
    {
        uint16_t                   lightness;
        uint16_t                   hue;
        uint16_t                   sat;
        uint16_t                   cct;

    } engine[PROFILE_LIGHT_ENGINE_NUM];

} profile_2104_le_advhsi_16bit_arg_t;


typedef struct
{
    struct
    {
        uint8_t                   lightness;
        uint8_t                   x;
        uint8_t                   y;


    } engine[PROFILE_LIGHT_ENGINE_NUM];

} profile_2201_le_xy_8bit_arg_t;


typedef struct
{
    struct
    {
        uint16_t                   lightness;
        uint16_t                   x;
        uint16_t                   y;


    } engine[PROFILE_LIGHT_ENGINE_NUM];

} profile_2202_le_xy_16bit_arg_t;


typedef struct
{
    uint8_t                  intensity;
    uint8_t                  light_mode;
    uint8_t                  state;
    uint8_t                  effect_type;
    uint8_t                  frq;
} profile_3001_uitimate_w_8bit_arg_t;

typedef struct
{
    uint16_t                 intensity;
    uint8_t                  light_mode;
    uint8_t                  state;
    uint8_t                  effect_type;
    uint8_t                  frq;
} profile_3002_uitimate_w_16bit_arg_t;

typedef struct
{
    uint8_t                  intensity;
    uint8_t                  cct;
    uint8_t                  light_mode;
    uint8_t                  state;
    uint8_t                  effect_type;
    uint8_t                  frq;
} profile_3003_uitimate_lite_8bit_arg_t;


typedef struct
{
    uint16_t                 intensity;
    uint16_t                 cct;
    uint8_t                  light_mode;
    uint8_t                  state;
    uint8_t                  effect_type;
    uint8_t                  frq;
    uint8_t                  strobe;
} profile_3004_uitimate_lite_16bit_arg_t;

typedef struct
{
    uint8_t                  intensity;
    uint8_t                  cct;
	uint8_t                  gm;
    uint8_t                  light_mode;
    uint8_t                  state;
    uint8_t                  effect_type;
    uint8_t                  frq;
} profile_3007_uitimate_lite_8bit_arg_t;

typedef struct
{
    uint16_t                 intensity;
    uint16_t                 cct;
	uint16_t                 gm;
    uint8_t                 light_mode;
    uint8_t                  state;
    uint8_t                  effect_type;
    uint8_t                  frq;
} profile_3008_uitimate_lite_16bit_arg_t;

#pragma pack ()

#endif

