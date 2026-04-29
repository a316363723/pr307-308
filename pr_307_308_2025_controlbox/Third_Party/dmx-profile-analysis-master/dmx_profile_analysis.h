#ifndef DMX_PROFILE_ANALYSIS_H
#define DMX_PROFILE_ANALYSIS_H
#include "dmx_profile.h"
#include "dmx_profile_configuration.h"

#pragma pack (1)

enum dmx_type
{
    DMX_TYPE_CCT,
    DMX_TYPE_HSI,
    DMX_TYPE_GEL,
    DMX_TYPE_XY,
    DMX_TYPE_SOUCE,
    DMX_TYPE_RGB,
	DMX_TYPE_RGBWW,
    DMX_TYPE_MIXING,
    DMX_TYPE_PIXEL_CTRL,
    DMX_TYPE_FX,
    DMX_TYPE_FX_II,
    DMX_TYPE_FX_PIXEL
};

enum dmx_light_mode
{
    DMX_LIGHT_MODE_CCT,
    DMX_LIGHT_MODE_HSI,
    DMX_LIGHT_MODE_GEL,
    DMX_LIGHT_MODE_XY,
    DMX_LIGHT_MODE_SOUCE,
    DMX_LIGHT_MODE_RGB,
    DMX_LIGHT_MODE_MIXING,
    DMX_LIGHT_MODE_BLACK,
};

enum dmx_fx_mode
{

    DMX_FX_MODE_CCT = 0,
    DMX_FX_MODE_HSI,
    DMX_FX_MODE_GEL,
    DMX_FX_MODE_XY,
    DMX_FX_MODE_SOUYRCE,
    DMX_FX_MODE_RGB,
    DMX_FX_MODE_CCT_Range,
    DMX_FX_MODE_HSI_Range,
    DMX_FX_II_MODE_Null,
};

enum dmx_color_mixing_mode
{
    DMX_ColorMixingMode_CCT = 0,
    DMX_ColorMixingMode_HSI,
    DMX_ColorMixingMode_GEL,
    DMX_ColorMixingMode_XY,
    DMX_ColorMixingMode_SOUYRCE,
    DMX_ColorMixingMode_RGB,
    DMX_ColorMixingMode_Null,
};

enum dmx_fx_state
{
    DMX_FX_STATE_LOOP,
    DMX_FX_STATE_TRIGGER,
    DMX_FX_STATE_STOP,
    DMX_FX_STATE_NULL,
};

enum dmx_fx_2_state
{
    DMX_FX_II_STATE_LOOP,
    DMX_FX_II_STATE_TRIGGER,
    DMX_FX_II_STATE_STOP,
    DMX_FX_II_STATE_NULL,
};


typedef enum
{
    DMX_FX_Trigger_None = 0,
    DMX_FX_Trigger_Once,
    DMX_FX_Trigger_Continue
} DMX_fx_trigger_enum;


enum dmx_fx_type
{
    DMX_FX_CLUB_LIGHTS,
    DMX_FX_PAPARAZZI,
    DMX_FX_LIGHTNING,
    DMX_FX_TV,
    DMX_FX_CANDLE,
    DMX_FX_FIRE,
    DMX_FX_STROBE,
    DMX_FX_EXPLOSION,
    DMX_FX_FAULT_BULB,
    DMX_FX_PULSING,
    DMX_FX_WELDING,
    DMX_FX_COP_CAR,
    DMX_FX_COLOR_CHASE,
    DMX_FX_PARTY_LIGHTS,
    DMX_FX_FIREWORKS,
    DMX_FX_COLOR_FADE,
    DMX_FX_COLOR_CYCLE,
    DMX_FX_COLOR_GRADIENT,
    DMX_FX_ONE_COLOR_CHASE,
};



enum dmx_fx_2_type
{
    DMX_FX_II_PAPARAZZI,
    DMX_FX_III_LIGHTNING,
    DMX_FX_III_TV,
    DMX_FX_III_FIRE,
    DMX_FX_III_FAULT_BULB,
    DMX_FX_III_PULSING,
    DMX_FX_III_COP_CAR,
    DMX_FX_II_PARTY_LIGHTS,
    DMX_FX_II_FIREWORKS,
};


enum dmx_pixel_fx_type
{
    DMX_FX_COLOE_FADE,
    DMX_FX_COLOE_CYCLE,
    DMX_FX_ONE_PIXEL_CHASE,
    DMX_FX_TWO_PIXEL_CHASE,
    DMX_FX_THREE_PIXEL_CHASE,
    DMX_FX_RAINBOW,
    DMX_FX_PIXEL_FILE,

};

enum dmx_fan_mode_type
{
    DMX_FAN_MODE_SMART,
    DMX_FAN_MODE_SILENT,
    DMX_FAN_MODE_MEDIUM,
    DMX_FAN_MODE_HIGH,
    DMX_FAN_MODE_MANUAL,
};

enum refresh_event_type
{
    DMX_ZOOM_REFRESH = 1<<0,
    DMX_PAN_TILT_REFRESH= 1<<1,
    DMX_FAN_REFRESH= 1<<2,
    DMX_CURVE_REFRESH= 1<<3,
    DMX_FRQ_REFRESH= 1<<4,
	DMX_PAN_REFRESH= 1<<5,
	DMX_ZOOM_REFRESH_FINE = 1<<6,
	DMX_PAN_TILT_REFRESH_FINE= 1<<7,
    DMX_PAN_REFRESH_FINE= 1<<8,
};



typedef struct
{
    float x;
    float y;
} dmx_xy_arg_t;

typedef struct
{
    float    lightness;
} dmx_obj_t;


typedef struct
{

    uint16_t cct;
    float    duv;

} dmx_cct_arg_t;

typedef struct
{

    float  hue;
    float  sat;
    uint16_t cct;
} dmx_hsi_arg_t;

typedef struct
{

    uint16_t cct;
    uint8_t  brand;
    uint8_t  type;
    uint16_t color;
} dmx_gel_arg_t;


typedef struct
{

    float   red;
    float   green;
    float   blue;
    float   ww;
    float   cw;
    float   amber;
    float   cyan;
    float   lime;

} dmx_rgb_arg_t;


typedef struct
{
    uint8_t    type;
    uint16_t      x;
    uint16_t      y;
} dmx_souce_arg_t;

typedef struct
{
    uint16_t c_up_limit;
    uint16_t c_down_limit;

} cct_param_setting_t;

typedef struct
{
    uint16_t h_up_limit;
    uint16_t h_down_limit;

} hsi_param_setting_t;


struct dmx_mixing_color
{

    union
    {
        dmx_cct_arg_t         cct;
        dmx_hsi_arg_t         hsi;
        dmx_gel_arg_t         gel;
        dmx_rgb_arg_t         rgb;
    } arg;
    enum dmx_color_mixing_mode mode;
};

typedef struct
{

    struct dmx_mixing_color color1_arg;
    struct dmx_mixing_color color2_arg;
    float  ratio;
} dmx_mixing_arg_t;


typedef struct
{
    struct
    {
        float          lightness;
        enum dmx_light_mode mode;
        union
        {
            dmx_cct_arg_t         cct;
            dmx_hsi_arg_t         hsi;
            dmx_gel_arg_t         gel;
            dmx_rgb_arg_t         rgb;
            dmx_xy_arg_t          xy;
            dmx_mixing_arg_t      mixing;
        } arg;
    } pixel[PROFILE_LIGHT_ENGINE_NUM];
} dmx_pixel_ctrl_arg_t;



typedef struct
{
    struct
    {
        uint16_t  lightness;
        enum dmx_color_mixing_mode mode;
        float  ratio;
        union
        {
            dmx_cct_arg_t         cct;
            dmx_hsi_arg_t         hsi;
            dmx_gel_arg_t         gel;
            dmx_rgb_arg_t         rgb;
            dmx_xy_arg_t          xy;
        } arg;
    }
    engine[PROFILE_LIGHT_ENGINE_NUM];
    uint8_t                  fan_mode;
    uint8_t                  fan_rpm;

} dmx_Score_board_mixing_arg_t;

struct dmx_fx_clue_lights
{
    uint8_t color;
    uint8_t   frq;
} ;

struct dmx_fx_paparazzi
{
    uint16_t  cct;
    int8_t     duv;
    uint8_t   frq;

} ;

struct dmx_fx_lightning
{
    uint16_t  cct;
    int8_t     duv;
    uint8_t   frq;
    uint8_t   spd;
};

struct dmx_fx_tv
{
    uint8_t   cct_range;
    uint8_t   frq;
};

struct dmx_fx_candle
{

    uint8_t   cct_rang;
    uint8_t   frq;

};

struct dmx_fx_fire
{
    uint8_t   cct_range;
    uint8_t   frq;

};

struct dmx_fx_strobe
{
    enum dmx_fx_mode mode;
    union
    {
        struct
        {
            uint16_t cct;
            int8_t duv;
        } cct;
        struct
        {
            uint16_t hue;
            uint8_t sat;
            uint16_t cct ;
        } hsi;
        struct
        {
            uint16_t cct;
            uint8_t brand;
            uint8_t type;
            uint8_t color;
        } gel;
        struct
        {
            float x;
            float y;
        } xy;
        struct
        {
            uint8_t type;
        } socue;
        struct
        {
            uint16_t r_ratio;
            uint16_t g_ratio;
            uint16_t b_ratio;
        } rgb;
    } mode_arg;
    uint8_t   spd;
};

struct dmx_fx_explosion
{
    enum dmx_fx_mode mode;
    uint8_t   decay;
    union
    {
        struct
        {
            uint16_t cct;
            int8_t duv;
        } cct;
        struct
        {
            uint16_t hue;
            uint8_t sat;
            uint16_t cct;
        } hsi;
        struct
        {
            uint16_t cct;
            uint8_t brand;
            uint8_t type;
            uint8_t color;
        } gel;
        struct
        {
            float x;
            float y;
        } xy;
        struct
        {
            uint8_t type;
        } socue;
        struct
        {
            uint16_t r_ratio;
            uint16_t g_ratio;
            uint16_t b_ratio;
        } rgb;
    } mode_arg;

};

struct dmx_fx_fault_bulb
{
    enum dmx_fx_mode mode;
    uint8_t   frq;
    uint8_t   spd;
    union
    {
        struct
        {
            uint16_t cct;
            int8_t duv;
        } cct;
        struct
        {
            uint16_t hue;
            uint8_t sat;
            uint16_t cct;
        } hsi;
        struct
        {
            uint16_t cct;
            uint8_t brand;
            uint8_t type;
            uint8_t color;
        } gel;
        struct
        {
            float x;
            float y;
        } xy;
        struct
        {
            uint8_t type;
        } socue;
        struct
        {
            uint16_t r_ratio;
            uint16_t g_ratio;
            uint16_t b_ratio;
        } rgb;
    } mode_arg;
};
struct dmx_fx_pulsing
{
    enum dmx_fx_mode mode;
    uint8_t   spd;
    uint8_t   frq;
    union
    {
        struct
        {
            uint16_t cct;
            int8_t duv;
        } cct;
        struct
        {
            uint16_t hue;
            uint8_t sat;
            uint16_t cct;
        } hsi;
        struct
        {
            uint16_t cct;
            uint8_t brand;
            uint8_t type;
            uint8_t color;
        } gel;
        struct
        {
            float x;
            float y;
        } xy;
        struct
        {
            uint8_t type;
        } socue;
        struct
        {
            uint16_t r_ratio;
            uint16_t g_ratio;
            uint16_t b_ratio;
        } rgb;
    } mode_arg;
};
struct dmx_fx_welding
{
    enum dmx_fx_mode mode;
    uint8_t   frq;
    uint16_t  min_lightness;
    union
    {
        struct
        {
            uint16_t cct;
            int8_t duv;
        } cct;
        struct
        {
            uint16_t hue;
            uint8_t sat;
            uint16_t cct;
        } hsi;
        struct
        {
            uint16_t cct;
            uint8_t brand;
            uint8_t type;
            uint8_t color;
        } gel;
        struct
        {
            float x;
            float y;
        } xy;
        struct
        {
            uint8_t type;
        } socue;
        struct
        {
            uint16_t r_ratio;
            uint16_t g_ratio;
            uint16_t b_ratio;
        } rgb;
    } mode_arg;

};

struct dmx_fx_copcar
{
    uint8_t  color;
    uint8_t   frq;
};

struct dmx_fx_color_chase
{
    uint8_t   sat;
    uint8_t   spd;
};
struct dmx_fx_party_lights
{
    uint8_t  sat;
    uint8_t   frq;
};

struct dmx_fx_firework
{
    uint8_t  type;
    uint8_t   frq;
};

struct dmx_fx_color_fade
{

    uint8_t     light_board_a;
    union
    {
        dmx_cct_arg_t     cct;
        dmx_hsi_arg_t     hsi;
    } arg_a;

    uint8_t     light_board_b;
    union
    {
        dmx_cct_arg_t     cct;
        dmx_hsi_arg_t     hsi;
    } arg_b;

    uint8_t     light_board_c;
    union
    {
        dmx_cct_arg_t     cct;
        dmx_hsi_arg_t     hsi;
    } arg_c;

    uint8_t     light_board_d;
    union
    {
        dmx_cct_arg_t     cct;
        dmx_hsi_arg_t     hsi;
    } arg_d;

    uint8_t     frq;
    uint8_t     direction;

};


struct dmx_fx_color_cycle
{

    uint8_t     light_board_a;
    union
    {
        dmx_cct_arg_t     cct;
        dmx_hsi_arg_t     hsi;
    } arg_a;

    uint8_t     light_board_b;
    union
    {
        dmx_cct_arg_t     cct;
        dmx_hsi_arg_t     hsi;
    } arg_b;

    uint8_t     light_board_c;
    union
    {
        dmx_cct_arg_t     cct;
        dmx_hsi_arg_t     hsi;
    } arg_c;

    uint8_t     light_board_d;
    union
    {
        dmx_cct_arg_t     cct;
        dmx_hsi_arg_t     hsi;
    } arg_d;

    uint8_t     frq;


};
struct dmx_fx_color_gradient
{

    uint8_t     light_board_a;
    union
    {
        dmx_cct_arg_t     cct;
        dmx_hsi_arg_t     hsi;
    } arg_a;

    uint8_t     light_board_b;
    union
    {
        dmx_cct_arg_t     cct;
        dmx_hsi_arg_t     hsi;
    } arg_b;

    uint8_t     light_board_c;
    union
    {
        dmx_cct_arg_t     cct;
        dmx_hsi_arg_t     hsi;
    } arg_c;

    uint8_t     light_board_d;
    union
    {
        dmx_cct_arg_t     cct;
        dmx_hsi_arg_t     hsi;
    } arg_d;

    uint8_t     frq;
    uint8_t     min_brightness;

};

struct dmx_one_color_chase
{

    uint8_t     light_board_a;
    union
    {
        dmx_cct_arg_t     cct;
        dmx_hsi_arg_t     hsi;
    } arg_a;

    uint8_t     light_board_b;
    union
    {
        dmx_cct_arg_t     cct;
        dmx_hsi_arg_t     hsi;
    } arg_b;

    uint8_t     frq;
    uint8_t     direction;

};


struct dmx_fx_2_mode
{

    union
    {
        dmx_cct_arg_t               cct;
        dmx_hsi_arg_t               hsi;
        dmx_gel_arg_t               gel;
        dmx_xy_arg_t                xy;
        dmx_souce_arg_t             source;
        dmx_rgb_arg_t               rgb;
        cct_param_setting_t         cct_range;
        hsi_param_setting_t         hsi_range;
    } arg;
};

typedef struct
{
    int8_t                       duv;
    uint16_t                    cct;
    uint16_t                    time_lower_limit;
    uint16_t                    time_upper_limit;

} dmx_fx_paparazzi2_t;             /*size: 12*/


typedef struct
{
    uint8_t                     frq;
    uint8_t                     spd;
    enum dmx_fx_mode            mode;
    union
    {
        dmx_cct_arg_t               cct;
        dmx_hsi_arg_t               hsi;
    } arg;

} dmx_fx_lightning2_t;

/*0x02??TV_II*/
typedef struct
{

    uint8_t                     spd;
    enum dmx_fx_mode            mode;
    union
    {
        dmx_cct_arg_t               cct;
        dmx_hsi_arg_t               hsi;
    } arg;
} dmx_fx_tv2_t;


/*0x03??Candle_II*/
typedef struct
{

    uint8_t                     cct_range;
    uint8_t                     spd;
} dmx_fx_candle2_t;


/*0x04??Fire_II*/
typedef struct
{

    uint8_t                      spd;
    enum dmx_fx_mode            mode;
    union
    {
        cct_param_setting_t         cct_range;
        hsi_param_setting_t         hsi_range;
    } arg;
} dmx_fx_fire2_t;

/*0x05??Strobe_II*/
typedef struct
{

    uint8_t                     spd;
    enum dmx_fx_mode            mode;
    union
    {
        dmx_cct_arg_t               cct;
        dmx_hsi_arg_t               hsi;
        dmx_gel_arg_t               gel;
        dmx_xy_arg_t                xy;
        dmx_souce_arg_t             source;
        dmx_rgb_arg_t               rgb;
    } arg;
} dmx_fx_strobe2_t;

/*0x06??Explosion_II*/
typedef struct
{

    uint8_t                     decay;
    enum dmx_fx_mode            mode;
    union
    {
        dmx_cct_arg_t               cct;
        dmx_hsi_arg_t               hsi;
        dmx_gel_arg_t               gel;
        dmx_xy_arg_t                xy;
        dmx_souce_arg_t             source;
        dmx_rgb_arg_t               rgb;
    } arg;
} dmx_fx_explosion2_t;

/*0x07??Fault_Bulb_II*/
typedef struct
{

    uint8_t                     spd;
    uint8_t                     frq;
    enum dmx_fx_mode            mode;
    union
    {
        dmx_cct_arg_t               cct;
        dmx_hsi_arg_t               hsi;
        dmx_gel_arg_t               gel;
        dmx_xy_arg_t                xy;
        dmx_souce_arg_t             source;
        dmx_rgb_arg_t               rgb;
    } arg;
} dmx_fx_fault_bulb2_t;            /*size: 12*/

/*0x08??Pulsing_II*/
typedef struct
{

    uint8_t                     spd;
    uint8_t                     frq;
    enum dmx_fx_mode            mode;
    union
    {
        dmx_cct_arg_t               cct;
        dmx_hsi_arg_t               hsi;
        dmx_gel_arg_t               gel;
        dmx_xy_arg_t                xy;
        dmx_souce_arg_t             source;
        dmx_rgb_arg_t               rgb;
    } arg;
} dmx_fx_pulsing2_t;               /*size: 12*/

/*0x09??Welding_II*/
typedef struct
{

    uint16_t                    min_intensity;
    uint8_t                     frq;
    enum dmx_fx_mode            mode;
    union
    {
        dmx_cct_arg_t               cct;
        dmx_hsi_arg_t               hsi;
        dmx_gel_arg_t               gel;
        dmx_xy_arg_t                xy;
        dmx_souce_arg_t             source;
        dmx_rgb_arg_t               rgb;
    } arg;
} dmx_fx_welding2_t;               /*size: 13*/


typedef struct
{

    uint8_t                     frq;
    uint8_t                     color;
} dmx_fx_cop_car3_t;

/*0x0B??Party_Lights_II*/
typedef struct
{

    uint8_t                    sat;
    uint8_t                    spd;
} dmx_fx_party_lights2_t;

/*0x0C??Fireworks_II*/
typedef struct
{

    uint8_t                     color;
    uint16_t                    lower_limit;
    uint16_t                    upper_limit;
} dmx_fx_fireworks2_t;

typedef struct
{

    uint16_t                     time_lower_limit;
    uint16_t                     time_upper_limit;
    enum dmx_fx_mode             mode;
    union
    {
        dmx_cct_arg_t               cct;
        dmx_hsi_arg_t               hsi;
        dmx_gel_arg_t               gel;
        dmx_xy_arg_t                xy;
        dmx_souce_arg_t             source;
        dmx_rgb_arg_t               rgb;

    } arg;
} dmx_fx2_lightning3_t;

/*0x0E??TV_III*/
typedef struct
{
    uint16_t                     time_lower_limit;
    uint16_t                     time_upper_limit;
    enum dmx_fx_mode             mode;
    union
    {
        struct
        {
            cct_param_setting_t   cct_range;

            int8_t      duv;
        } cct;
        struct
        {
            hsi_param_setting_t   hsi_range;
            uint16_t     cct;
            uint8_t      sat;

        } hsi;
    } arg;
} dmx_fx_tv3_t;

/*0x0F??Fire_III*/
typedef struct
{

    uint8_t                      frq;
    enum dmx_fx_mode             mode;
    union
    {
        struct
        {
            cct_param_setting_t   cct_range;
            int8_t      duv;
        } cct;
        struct
        {
            hsi_param_setting_t   hsi_range;
            uint16_t     cct;
            uint8_t      sat;

        } hsi;
    } arg;
} dmx_fx_fire3_t;

/*0x10??Faulty_Bulb_III*/
typedef struct
{
    uint16_t                     time_lower_limit;
    uint16_t                     time_upper_limit;
    enum dmx_fx_mode              mode;
    union
    {
        dmx_cct_arg_t               cct;
        dmx_hsi_arg_t               hsi;
        dmx_gel_arg_t               gel;
        dmx_xy_arg_t                xy;
        dmx_souce_arg_t             source;
        dmx_rgb_arg_t               rgb;
    } arg;
} dmx_fx_faulty_bulb3_t;

/*0x11??Pulsing_III*/
typedef struct
{
    enum dmx_fx_mode               mode;
    uint8_t  pluse;
    union
    {
        dmx_cct_arg_t               cct;
        dmx_hsi_arg_t               hsi;
        dmx_gel_arg_t               gel;
        dmx_xy_arg_t                xy;
        dmx_souce_arg_t             source;
        dmx_rgb_arg_t               rgb;
    } arg;

} dmx_fx_pulsing3_t;

/*0x12??cop_car_3*/
typedef struct
{

    uint8_t                     colors;
    uint8_t                     frq;

} dmx_fx_fireworks3_t;


/*0x00??Club_Lights*/
typedef struct
{
    uint8_t     color_choose;
    uint16_t     move;
    uint8_t     spd;
    uint8_t     color_1_move;
    union
    {
        dmx_cct_arg_t     cct;
        dmx_hsi_arg_t     hsi;
    } arg1;

    uint8_t     color_2_move;
    union
    {
        dmx_cct_arg_t     cct;
        dmx_hsi_arg_t     hsi;
    } arg2;


    uint8_t     color_3_move;
    union
    {
        dmx_cct_arg_t     cct;
        dmx_hsi_arg_t     hsi;
    } arg3;

    uint8_t     color_4_move;
    union
    {
        dmx_cct_arg_t     cct;
        dmx_hsi_arg_t     hsi;
    } arg4;


} dmx_fx_pixel_coloe_fade_t;

typedef struct
{
    uint8_t     color_choose;
    uint16_t    spd;
    uint8_t     transition;
    uint8_t     move;

    uint8_t     color_1_move;
    union
    {
        dmx_cct_arg_t     cct;
        dmx_hsi_arg_t     hsi;
    } arg1;

    uint8_t     color_2_move;
    union
    {
        dmx_cct_arg_t     cct;
        dmx_hsi_arg_t     hsi;
    } arg2;


    uint8_t     color_3_move;
    union
    {
        dmx_cct_arg_t     cct;
        dmx_hsi_arg_t     hsi;
    } arg3;

    uint8_t     color_4_move;
    union
    {
        dmx_cct_arg_t     cct;
        dmx_hsi_arg_t     hsi;
    } arg4;


} dmx_fx_pixel_coloe_cycle_t;


typedef struct
{
    uint8_t     pixel_size;
    uint16_t     spd;
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
                dmx_cct_arg_t     cct;
                dmx_hsi_arg_t     hsi;
            } arg1;

            float      background_color_int;
            uint8_t     background_color_mode;

            union
            {
                dmx_cct_arg_t     cct;
                dmx_hsi_arg_t     hsi;
            } arg2;
        } way1;
        struct
        {
            uint8_t     pixel_1_mode;
            union
            {
                dmx_cct_arg_t     cct;
                dmx_hsi_arg_t     hsi;
            } arg1;
            uint8_t     pixel_2_mode;
            union
            {
                dmx_cct_arg_t     cct;
                dmx_hsi_arg_t     hsi;
            } arg2;
            float     background_color_int;
            uint8_t     background_color_mode;
            union
            {
                dmx_cct_arg_t     cct;
                dmx_hsi_arg_t     hsi;
            } arg3;
        } way2;
    } way_mode;

} dmx_fx_pixel_one_pixel_chase_t;

typedef struct
{
    uint8_t     pixel_size;
    uint16_t     spd;
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
                dmx_cct_arg_t     cct;
                dmx_hsi_arg_t     hsi;
            } arg1;

            uint8_t     pixel_2_mode;
            union
            {
                dmx_cct_arg_t     cct;
                dmx_hsi_arg_t     hsi;
            } arg2;

            float     background_color_int;
            uint8_t     background_color_mode;
            union
            {
                dmx_cct_arg_t     cct;
                dmx_hsi_arg_t     hsi;
            } arg3;
        } way1;
        struct
        {

            uint8_t     pixel_1_mode;
            union
            {
                dmx_cct_arg_t     cct;
                dmx_hsi_arg_t     hsi;
            } arg1;

            uint8_t     pixel_2_mode;
            union
            {
                dmx_cct_arg_t     cct;
                dmx_hsi_arg_t     hsi;
            } arg2;

            uint8_t     pixel_3_mode;
            union
            {
                dmx_cct_arg_t     cct;
                dmx_hsi_arg_t     hsi;
            } arg3;

            uint8_t     pixel_4_mode;
            union
            {
                dmx_cct_arg_t     cct;
                dmx_hsi_arg_t     hsi;
            } arg4;


            float     background_color_int;
            uint8_t     background_color_mode;
            union
            {
                dmx_cct_arg_t     cct;
                dmx_hsi_arg_t     hsi;
            } arg5;
        } way2;
    } way_mode;

} dmx_fx_pixel_two_pixel_chase_t;

typedef struct
{
    uint8_t     pixel_size;
    uint16_t     spd;
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
                dmx_cct_arg_t     cct;
                dmx_hsi_arg_t     hsi;
            } arg1;

            uint8_t     pixel_2_mode;
            union
            {
                dmx_cct_arg_t     cct;
                dmx_hsi_arg_t     hsi;
            } arg2;

            uint8_t     pixel_3_mode;
            union
            {
                dmx_cct_arg_t     cct;
                dmx_hsi_arg_t     hsi;
            } arg3;

            float       background_color_int;
            uint8_t     background_color_mode;
            union
            {
                dmx_cct_arg_t     cct;
                dmx_hsi_arg_t     hsi;
            } arg4;
        } way1;
        struct
        {

            uint8_t     pixel_1_mode;
            union
            {
                dmx_cct_arg_t     cct;
                dmx_hsi_arg_t     hsi;
            } arg1;

            uint8_t     pixel_2_mode;
            union
            {
                dmx_cct_arg_t     cct;
                dmx_hsi_arg_t     hsi;
            } arg2;

            uint8_t     pixel_3_mode;
            union
            {
                dmx_cct_arg_t     cct;
                dmx_hsi_arg_t     hsi;
            } arg3;

            uint8_t     pixel_4_mode;
            union
            {
                dmx_cct_arg_t     cct;
                dmx_hsi_arg_t     hsi;
            } arg4;


            uint8_t     pixel_5_mode;
            union
            {
                dmx_cct_arg_t     cct;
                dmx_hsi_arg_t     hsi;
            } arg5;

            uint8_t     pixel_6_mode;
            union
            {
                dmx_cct_arg_t     cct;
                dmx_hsi_arg_t     hsi;
            } arg6;


            float       background_color_int;
            uint8_t     background_color_mode;
            union
            {
                dmx_cct_arg_t     cct;
                dmx_hsi_arg_t     hsi;
            } arg7;
        } way2;
    } way_mode;

} dmx_fx_pixel_three_pixel_chase_t;



typedef struct
{

    uint8_t     move;
    uint8_t     spd;

} dmx_fx_rainbow_t;


typedef struct
{

    float      lightness_lower;
    float      lightness_upper;
    float      bg_lightness;
    uint8_t     frq;
    uint8_t     direction;
    struct
    {
        enum dmx_light_mode mode;
        union
        {
            dmx_cct_arg_t     cct;
            dmx_hsi_arg_t     hsi;
        } arg;
    } fire_arg, bg_arg;
} dmx_fx_pixel_file_t;


typedef struct
{
    uint16_t cct;
    enum dmx_fx_type type;
    union
    {
        struct dmx_fx_clue_lights   clue_lights;
        struct dmx_fx_paparazzi     paparazzi;
        struct dmx_fx_lightning     lightning;
        struct dmx_fx_tv            tv;
        struct dmx_fx_candle        candle;
        struct dmx_fx_fire          fire;
        struct dmx_fx_fault_bulb    fault_bulb;
        struct dmx_fx_pulsing       pulsing;
        struct dmx_fx_copcar        copcar;
        struct dmx_fx_color_chase   color_chase;
        struct dmx_fx_party_lights  party_lights;
        struct dmx_fx_firework      fireworks;
        struct dmx_fx_strobe        strobe;
        struct dmx_fx_explosion     explosion;
        struct dmx_fx_welding       welding;
        struct dmx_fx_color_fade    color_fade;
        struct dmx_fx_color_cycle   color_cycle;
        struct dmx_fx_color_gradient  color_gradient;
        struct dmx_one_color_chase     one_color_chase;

    } arg;
} dmx_fx_arg_t;

typedef struct
{
    enum dmx_fx_2_type type;
    union
    {
        dmx_fx_paparazzi2_t            paparazzi2;
        dmx_fx_lightning2_t            lightning2;
        dmx_fx_tv2_t                   tv2;
        dmx_fx_candle2_t               candle2;
        dmx_fx_fire2_t                 fire2;
        dmx_fx_strobe2_t               strobe2;
        dmx_fx_explosion2_t            explosion2;
        dmx_fx_fault_bulb2_t           fault_bulb2;
        dmx_fx_pulsing2_t              pulsing2;
        dmx_fx_welding2_t              welding2;
        dmx_fx_cop_car3_t              cop_car3;
        dmx_fx_party_lights2_t         party_lights2;
        dmx_fx_fireworks2_t            fireworks2;
        dmx_fx2_lightning3_t           lightning3;
        dmx_fx_tv3_t                   tv3;
        dmx_fx_fire3_t                 fire3;
        dmx_fx_faulty_bulb3_t          faulty_bulb3;
        dmx_fx_pulsing3_t              pulsing3;
        dmx_fx_fireworks3_t            fireworks3;

    } arg;
} dmx_fx_2_arg_t;


typedef struct
{
    enum dmx_pixel_fx_type type;
    dmx_fx_pixel_coloe_fade_t             coloe_fade;
    dmx_fx_pixel_coloe_cycle_t            coloe_cycle;
    dmx_fx_pixel_one_pixel_chase_t        one_pixel_chase;
    dmx_fx_pixel_two_pixel_chase_t        two_pixel_chase;
    dmx_fx_pixel_three_pixel_chase_t      three_pixel_chase;
    dmx_fx_rainbow_t                      rainbow;
    dmx_fx_pixel_file_t                   pixel_file;

} dmx_fx_pixel_arg_t;

typedef union
{
    dmx_cct_arg_t       cct;
    dmx_hsi_arg_t       hsi;
    dmx_gel_arg_t       gel;
    dmx_xy_arg_t        xy;
    dmx_souce_arg_t     souce;
    dmx_rgb_arg_t       rgb;
    dmx_mixing_arg_t    mixing;
    dmx_pixel_ctrl_arg_t pixel_ctrl;
    dmx_fx_arg_t        fx;
    dmx_fx_2_arg_t      fx2;
    dmx_fx_pixel_arg_t  pixel_fx;
} dmx_mode_arg_t;


typedef struct
{
    enum dmx_type           type;
    float                   lightness;
    dmx_mode_arg_t          arg;
} dmx_data_t;

//typedef struct
//{
//    uint8_t fan_mode;
//    uint8_t fan_rpm;

//} dmx_fan_t;

struct dmx_config_data
{
    float zoom;
    float pan;
    float tilt;
    uint8_t fan_mode;
    uint8_t dim_curve;
    uint16_t dim_freq;  //µ÷¹âÆµÂÊ£¨HZ£©

};

#pragma pack ()

typedef enum
{
    #if DMX_PROFILE_0101_EN
    DMX_PROFILE_ID_0101,
    #endif
    #if DMX_PROFILE_0102_EN
    DMX_PROFILE_ID_0102,
    #endif
    #if DMX_PROFILE_0103_EN
    DMX_PROFILE_ID_0103,
    #endif
    #if DMX_PROFILE_0104_EN
    DMX_PROFILE_ID_0104,
    #endif
    #if DMX_PROFILE_0105_EN
    DMX_PROFILE_ID_0105,
    #endif
    #if DMX_PROFILE_0106_EN
    DMX_PROFILE_ID_0106,
    #endif
    #if DMX_PROFILE_0201_EN
    DMX_PROFILE_ID_0201,
    #endif
    #if DMX_PROFILE_0202_EN
    DMX_PROFILE_ID_0202,
    #endif
    #if DMX_PROFILE_0203_EN
    DMX_PROFILE_ID_0203,
    #endif
    #if DMX_PROFILE_0204_EN
    DMX_PROFILE_ID_0204,
    #endif
    #if DMX_PROFILE_0205_EN
    DMX_PROFILE_ID_0205,
    #endif
    #if DMX_PROFILE_0206_EN
    DMX_PROFILE_ID_0206,
    #endif
    #if DMX_PROFILE_0207_EN
    DMX_PROFILE_ID_0207,
    #endif
    #if DMX_PROFILE_0208_EN
    DMX_PROFILE_ID_0208,
    #endif
    #if DMX_PROFILE_0301_EN
    DMX_PROFILE_ID_0301,
    #endif
    #if DMX_PROFILE_0302_EN
    DMX_PROFILE_ID_0302,
    #endif
    #if DMX_PROFILE_0303_EN
    DMX_PROFILE_ID_0303,
    #endif
    #if DMX_PROFILE_0304_EN
    DMX_PROFILE_ID_0304,
    #endif
    #if DMX_PROFILE_0401_EN
    DMX_PROFILE_ID_0401,
    #endif
    #if DMX_PROFILE_0402_EN
    DMX_PROFILE_ID_0402,//20
    #endif
    #if DMX_PROFILE_0501_EN
    DMX_PROFILE_ID_0501,
    #endif
    #if DMX_PROFILE_0502_EN
    DMX_PROFILE_ID_0502,
    #endif
    #if DMX_PROFILE_0601_EN
    DMX_PROFILE_ID_0601,
    #endif
    #if DMX_PROFILE_0602_EN
    DMX_PROFILE_ID_0602,
    #endif
    #if DMX_PROFILE_0701_EN
    DMX_PROFILE_ID_0701,
    #endif
    #if DMX_PROFILE_0702_EN
    DMX_PROFILE_ID_0702,
    #endif
    #if DMX_PROFILE_0703_EN
    DMX_PROFILE_ID_0703,
    #endif
    #if DMX_PROFILE_0704_EN
    DMX_PROFILE_ID_0704,
    #endif
    #if DMX_PROFILE_0705_EN
    DMX_PROFILE_ID_0705,
    #endif
    #if DMX_PROFILE_0706_EN
    DMX_PROFILE_ID_0706,//30
    #endif
    #if DMX_PROFILE_0707_EN
    DMX_PROFILE_ID_0707,
    #endif
    #if DMX_PROFILE_0708_EN
    DMX_PROFILE_ID_0708,
    #endif
    #if DMX_PROFILE_0709_EN
    DMX_PROFILE_ID_0709,
    #endif
    #if DMX_PROFILE_0801_EN
    DMX_PROFILE_ID_0801,
    #endif
    #if DMX_PROFILE_0802_EN
    DMX_PROFILE_ID_0802,
    #endif
    #if DMX_PROFILE_0803_EN
    DMX_PROFILE_ID_0803,
    #endif
    #if DMX_PROFILE_0804_EN
    DMX_PROFILE_ID_0804,
    #endif
    #if DMX_PROFILE_0805_EN
    DMX_PROFILE_ID_0805,
    #endif
    #if DMX_PROFILE_0806_EN
    DMX_PROFILE_ID_0806,
    #endif
    #if DMX_PROFILE_0807_EN
    DMX_PROFILE_ID_0807, //40
    #endif
    #if DMX_PROFILE_0808_EN
    DMX_PROFILE_ID_0808,
    #endif
    #if DMX_PROFILE_0901_EN
    DMX_PROFILE_ID_0901,
    #endif
    #if DMX_PROFILE_0902_EN
    DMX_PROFILE_ID_0902,
    #endif
    #if DMX_PROFILE_2001_EN
    DMX_PROFILE_ID_2001,
    #endif
    #if DMX_PROFILE_2002_EN
    DMX_PROFILE_ID_2002,
    #endif
    #if DMX_PROFILE_2003_EN
    DMX_PROFILE_ID_2003,
    #endif
    #if DMX_PROFILE_2004_EN
    DMX_PROFILE_ID_2004,
    #endif
    #if DMX_PROFILE_2005_EN
    DMX_PROFILE_ID_2005,
    #endif
    #if DMX_PROFILE_2006_EN
    DMX_PROFILE_ID_2006,
    #endif
    #if DMX_PROFILE_2007_EN
    DMX_PROFILE_ID_2007, //50
    #endif
    #if DMX_PROFILE_2008_EN
    DMX_PROFILE_ID_2008,
    #endif
    #if DMX_PROFILE_2101_EN
    DMX_PROFILE_ID_2101,
    #endif
    #if DMX_PROFILE_2102_EN
    DMX_PROFILE_ID_2102,
    #endif
    #if DMX_PROFILE_2103_EN
    DMX_PROFILE_ID_2103,
    #endif
    #if DMX_PROFILE_2104_EN
    DMX_PROFILE_ID_2104,
    #endif
    #if DMX_PROFILE_2201_EN
    DMX_PROFILE_ID_2201,
    #endif
    #if DMX_PROFILE_2202_EN
    DMX_PROFILE_ID_2202,
    #endif
    #if DMX_PROFILE_3001_EN
    DMX_PROFILE_ID_3001,
    #endif
    #if DMX_PROFILE_3002_EN
    DMX_PROFILE_ID_3002,
    #endif
    #if DMX_PROFILE_3003_EN
    DMX_PROFILE_ID_3003, //60
    #endif
    #if DMX_PROFILE_3004_EN
    DMX_PROFILE_ID_3004,
    #endif
    #if DMX_PROFILE_3005_EN
    DMX_PROFILE_ID_3005,
    #endif
    #if DMX_PROFILE_3006_EN
    DMX_PROFILE_ID_3006,
    #endif
	#if DMX_PROFILE_3007_EN
    DMX_PROFILE_ID_3007,
    #endif
	#if DMX_PROFILE_3008_EN
    DMX_PROFILE_ID_3008,
    #endif
    DMX_PROFILE_ID_NUM,
} dmx_profile_id;


void dmx_yoke_fpesnel_angle_range_set(uint16_t max, uint16_t min);
void dmx_yoke_pan_range_set(uint16_t max, uint16_t min);
void dmx_yoke_tilt_range_set(uint16_t max, uint16_t min);
uint8_t profile_user_data_execte(dmx_profile_id id, const uint8_t* profile_data, dmx_data_t* dmx_data);
uint16_t dmx_sys_config_refresh_event_get(struct dmx_config_data** config_data);
void dmx_sys_config_event_clear(uint16_t val);
void dmx_electric_control_switch_set(uint8_t state);
void dmx_funtion_control_switch_set(uint8_t state);
uint8_t dmx_get_fx_extern_len(uint8_t type, uint8_t mode);

#endif

