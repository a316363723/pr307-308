#ifndef LOCAL_DATA_H
#define LOCAL_DATA_H
#include <stdint.h>
#include "dmx_profile_analysis.h"

#define PIXEL_FX_COLORS_MAX     (10)
#define PARTITION_MAX_NUM       (32)   //支持最大分区数
#define PARTITION_DEFAULT_NUM   (4)

#pragma pack(1)

enum fx_mode
{
    FX_MODE_CCT,
    FX_MODE_HSI,
    FX_MODE_GEL,
    FX_MODE_XY,
    FX_MODE_SOURCE,
    FX_MODE_RGB,
};

enum sidus_fx_type
{
    SIDUS_FX_TYPE_PFX,
    SIDUS_FX_TYPE_MFX,
    SIDUS_FX_TYPE_CFX,
    SIDUS_FX_TYPE_CFX_PREVIEW,
};

struct cct_param {
    uint16_t cct;
    int16_t  duv;
};

struct hsi_param {
    uint16_t cct;
    uint16_t hue;
    uint16_t sat;
};

struct gel_param {
    uint16_t cct;
    uint8_t  brand;             //品牌: ROSCO, LEE
    uint8_t  type[2];           //系列获取: type[ 品牌 ]
    uint16_t color[2][5];       //色纸索引获取: color[ 品牌 ][ 系列 ]
};

struct rgb_param {
    uint16_t r;
    uint16_t g;
    uint16_t b;
};

struct rgbww_param{
    uint16_t    red_ratio;      /*红灯PWM<0-65535>*/
    uint16_t    green_ratio;    /*绿灯PWM<0-65535>*/
    uint16_t    blue_ratio;     /*蓝灯PWM<0-65535>*/
    uint16_t    ww_ratio;       /*暖白PWM<0-65535>*/
    uint16_t    cw_ratio;       /*冷白PWM<0-65535>*/
    uint32_t    fade;           /*Fade，缓变时间，单位ms*/
};                 

struct xy_param {
    uint16_t x;
    uint16_t y;
};

struct source_param {
    uint8_t type;
    uint16_t x;
    uint16_t y;
} ;

struct db_cct
{
    uint16_t lightness;
    struct  cct_param cct;
};

struct db_hsi
{
    uint16_t lightness;
    struct hsi_param  hsi;
};

struct db_gel
{
    uint16_t lightness;
    struct gel_param gel;
};

struct db_rgb
{
    uint16_t lightness;
    struct rgb_param rgb;
};

struct db_rgbww
{
	uint16_t lightness;
	struct rgbww_param rgbww;
};

struct db_xy
{
    uint16_t lightness;
    struct xy_param xy;
};

struct db_source
{
    float lightness;
    struct source_param source;
};

struct db_factory_pwm
{
    uint16_t pwm[7];
};

/******************************************************************************
LIGHT_MODE_FX
******************************************************************************/

struct db_fx_mode_arg
{
    struct cct_param    cct;
    struct hsi_param    hsi;
    struct gel_param    gel;
    struct xy_param     xy;
    struct source_param source;
    struct rgb_param    rgb;
};

struct db_fx_club_lights
{
    uint16_t lightness;
    uint8_t  spd;
    uint8_t  color;
    uint8_t  state;
};

struct db_fx_paparazzi
{
    uint16_t lightness;
    uint8_t  frq;
    uint16_t cct;
    int8_t  gm;
    uint8_t  state;
};

struct db_fx_lightning
{
    uint16_t lightness;
    uint8_t  frq;
    uint16_t cct;
    int8_t  gm;
    uint8_t  trigger;
    uint8_t  speed;
    uint8_t  state;
};

struct db_fx_tv
{
    uint16_t lightness;
    uint8_t  frq;
    uint16_t cct;
    uint8_t  state;
};

struct db_fx_candle
{
    uint16_t lightness;
    uint16_t cct;
    uint8_t  spd;    
    uint8_t  state;
};

struct db_fx_fire
{
    uint16_t lightness;
    uint8_t  frq;
    uint16_t cct;
    uint8_t  state;
};

struct db_fx_strobe
{
    uint16_t lightness;
    uint8_t  frq;
    uint8_t  state;
    enum fx_mode mode;
    struct db_fx_mode_arg mode_arg;
};

struct db_fx_explosion
{
    uint16_t lightness;
    uint8_t  frq;
    uint8_t  trigger;
    uint8_t  state;
    enum fx_mode mode;
    struct db_fx_mode_arg mode_arg;
};

struct db_fx_fault_bulb
{
    uint16_t lightness;
    uint8_t  frq;
    uint8_t  speed;
    uint8_t  state;
    enum fx_mode mode;
    struct db_fx_mode_arg mode_arg;
};

struct db_fx_pulsing
{
    uint16_t lightness;
    uint8_t  frq;
    uint8_t  speed;
    uint8_t  state;
    enum fx_mode mode;
    struct db_fx_mode_arg mode_arg;
};

struct db_fx_welding
{
    uint16_t lightness;
    uint8_t  frq;
    uint8_t  min;
    uint8_t  trigger;
    uint8_t  state;
    enum fx_mode mode;
    struct db_fx_mode_arg mode_arg;
};

struct db_fx_cop_car
{
    uint16_t lightness;
    uint8_t  frq;
    uint8_t  color;
    uint8_t  state;
};

struct db_fx_color_chase
{
    uint16_t lightness;
    uint8_t  spd;
    uint8_t  sat;
    uint8_t  state;
};

struct db_fx_party_lights
{
    uint16_t lightness;
    uint8_t  spd;
    uint8_t  sat;
    uint8_t  state;
};

struct db_fx_fireworks
{
    uint16_t lightness;
    uint8_t  frq;
    uint8_t  type;
    uint8_t  state;
};

/******************************************************************************
LIGHT_MODE_SIDUS_FX2
******************************************************************************/

struct db_fx_mode_limit_arg {
    struct {
        uint16_t max_cct;
        uint16_t min_cct;
        float    duv;        
    } cct;

    struct {
        uint16_t max_hue;
        uint16_t min_hue;
        uint16_t cct;
        uint8_t  sat;        
    } hsi;
};

struct db_fx_paparazzi_2 {
    uint16_t lightness;
    uint16_t gap_time;
    uint16_t min_gap_time;
    uint8_t  state;
    enum fx_mode mode;
    struct db_fx_mode_arg mode_arg;    
};

struct db_fx_lightning_2 {
    uint16_t lightness;    
    uint8_t  frq;
    uint8_t  spd;
    uint8_t  state;
    uint8_t  trigger;
    enum fx_mode mode;
    struct db_fx_mode_arg mode_arg;
};

struct db_fx_tv_2 {
    uint16_t lightness;
    uint8_t spd;
    uint8_t state;
    enum fx_mode mode;
    struct db_fx_mode_limit_arg mode_arg;    
};

struct db_fx_fire_2 {
    uint16_t lightness;
    uint8_t  spd;
    uint8_t  state;
    enum fx_mode mode;
    struct db_fx_mode_limit_arg mode_arg;    
};

struct db_fx_strobe_2 {
    uint16_t lightness;
    uint8_t  spd;
    uint8_t  state;
    enum fx_mode mode;
    struct db_fx_mode_arg mode_arg;    
};

struct db_fx_explosion_2 {
    uint16_t lightness;
    uint8_t  decay;
    uint8_t  state;
    enum fx_mode mode;
    struct db_fx_mode_arg mode_arg;    
};

struct db_fx_fault_bulb_2 {
    uint16_t lightness;
    uint8_t spd;
    uint8_t frq;
    uint8_t state;
    enum fx_mode mode;
    struct db_fx_mode_arg mode_arg;    
};

struct db_fx_pulsing_2 {
    uint16_t lightness;
    uint8_t  spd;
    uint8_t  frq;
    uint8_t  state;
    enum fx_mode mode;
    struct db_fx_mode_arg mode_arg;    
};

struct db_fx_welding_2 {
    uint16_t lightness;
    uint16_t min_lightness;
    uint8_t  frq;
    uint8_t  state;
    enum fx_mode mode;
    struct db_fx_mode_arg mode_arg;    
};

struct db_fx_cop_car_2 {
    uint16_t lightness;
    uint8_t  frq;
    uint8_t  color;    
    uint8_t  state;
};

struct db_fx_party_lights_2 {
    uint16_t lightness;
    uint16_t sat;
    uint8_t  spd;
    uint8_t  state;
};

struct db_fx_fireworks_2 {
    uint16_t lightness;
    uint16_t gap_time;
    uint16_t min_gap_time;
    uint8_t  mode;          
    uint8_t  state;
};

struct db_fx_lightning_3 {
    uint16_t lightness;
    uint16_t gap_time;
    uint16_t min_gap_time;        
    uint8_t  state;
    enum fx_mode mode;
    struct db_fx_mode_arg mode_arg;
};

struct db_fx_tv_3 {
    uint16_t lightness;
    uint16_t gap_time;
    uint16_t min_gap_time;
    uint8_t  state;
    enum fx_mode mode;
    struct db_fx_mode_limit_arg mode_arg;    
};

struct db_fx_fire_3 {
    uint16_t lightness;
    uint8_t  frq;
    uint8_t  state;
    enum fx_mode mode;
    struct db_fx_mode_limit_arg  mode_arg;
};

struct db_fx_fault_bulb_3 {
    uint16_t lightness;
    uint16_t  gap_time;
    uint16_t  min_gap_time;
    uint8_t   state;
    enum fx_mode mode;
    struct db_fx_mode_arg mode_arg;
};

struct db_fx_pulsing_3 {
    uint16_t lightness;
    uint8_t frq;
    uint8_t state;
    enum fx_mode mode;
    struct db_fx_mode_arg mode_arg;
};

struct db_fx_cop_car_3 {
    uint16_t lightness;
    uint8_t  frq;
    uint8_t  color;
    uint8_t  state;
};

/******************************************************************************
LIGHT_MODE_SIDUS_FX
******************************************************************************/

enum sidus_fx_base
{
    SIDUS_FX_BASE_CCT,
    SIDUS_FX_BASE_HSI,
};

union sidus_fx_base_range_arg
{
    struct
    {
        uint16_t int_mimi;
        uint16_t int_max;
        uint8_t int_seq;
        uint16_t cct_mini;
        uint16_t cct_max;
        uint8_t cct_seq;
        uint8_t gm_mini;
        uint8_t gm_max;
        uint8_t gm_seq;
    } cct_range;
    struct
    {
        uint16_t int_mimi;
        uint16_t int_max;
        uint8_t int_seq;
        uint16_t hue_mini;
        uint16_t hue_max;
        uint8_t hue_seq;
        uint8_t sat_max;
        uint8_t sat_mini;
        uint8_t sat_seq;
    } hsi_range;
};

union sidus_fx_base_normal_arg
{
    struct
    {
        uint16_t lightness;
        uint16_t cct;
        uint8_t gm;
    } cct;
    struct
    {
        uint16_t lightness;
        uint16_t hue;
        uint8_t sat;
    } hsi;
};

struct mfx_flash_arg
{
    uint8_t loop_times;
    uint8_t loop_mode;
    uint32_t cycle_time_mini;
    uint32_t cycle_time_max;
    uint8_t cycle_time_seq;
    uint32_t free_time_mini;
    uint32_t free_time_max;
    uint8_t free_time_seq;
    uint32_t unit_time_mini;
    uint32_t unit_time_max;
    uint8_t unit_time_seq;
    uint8_t frq_mini;
    uint8_t frq_max;
    uint8_t frq_seq;
};

struct mfx_continue_arg
{
    uint8_t loop_times;
    uint8_t loop_mode;
    uint32_t cycle_time_mini;
    uint32_t cycle_time_max;
    uint8_t cycle_time_seq;
    uint32_t fade_in_time_mini;
    uint32_t fade_in_time_max;
    uint8_t fade_in_time_seq;
    uint8_t fade_in_curve;
    uint32_t fade_out_time_mini;
    uint32_t fade_out_time_max;
    uint8_t fade_out_time_seq;
    uint8_t fade_out_curve;
    uint8_t flicker_frq;
};

struct mfx_paragragh_arg
{
    uint8_t loop_times;
    uint8_t loop_mode;
    uint32_t cycle_time_mini;
    uint32_t cycle_time_max;
    uint8_t cycle_time_seq;
    uint32_t free_time_mini;
    uint32_t free_time_max;
    uint8_t free_time_seq;
    uint32_t unit_time_mini;
    uint32_t unit_time_max;
    uint8_t unit_time_seq;
    uint32_t fade_in_time_mini;
    uint32_t fade_in_time_max;
    uint8_t fade_in_time_seq;
    uint8_t fade_in_curve;
    uint32_t fade_out_time_mini;
    uint32_t fade_out_time_max;
    uint8_t fade_out_time_seq;
    uint8_t fade_out_curve;
    uint8_t flicker_frq;
    uint8_t olp_mini;
    uint8_t olp_max;
    uint8_t olp_seq;
    uint8_t olr_mini;
    uint8_t olr_max;
    uint8_t olr_seq;
    uint8_t overlap_seq;
};

struct db_manual_fx
{
    uint8_t ctrl;
    enum sidus_fx_base base;
    union sidus_fx_base_range_arg base_arg;
    struct
    {
        enum
        {
            SIDUS_MFX_FLASH,
            SIDUS_MFX_CONTINUE,
            SIDUS_MFX_PARAGRAPH,  
        }   mode;
        union
        {
            struct mfx_flash_arg        flash;
            struct mfx_continue_arg     continues;
            struct mfx_paragragh_arg    paragraph;
        } mode_arg;
    } fx_arg;
};

struct pfx_flash_arg
{
    enum sidus_fx_base base;
    union sidus_fx_base_normal_arg base_arg;
    struct
    {
        uint16_t times;
        uint8_t frq;
    } fx_arg;
};

struct pfx_continue_arg
{
    enum sidus_fx_base base;
    union sidus_fx_base_normal_arg base_arg;
    struct
    {
        uint32_t continue_time;
        uint8_t fade_incurve;
        uint32_t fade_in_time;
        uint8_t fade_out_curve;
        uint32_t fade_out_time;
    } fx_arg;
};

struct pfx_chase_arg
{
    enum sidus_fx_base base;
    union sidus_fx_base_range_arg base_arg;
    struct
    {
        uint8_t loop;
        uint32_t time;
    } fx_arg;
};

struct db_program_fx
{
    enum
    {
        SIDUS_PFX_FLASH,
        SIDUS_PFX_Continue,
        SIDUS_PFX_Chase,
    }  mode;
    union
    {
        struct pfx_flash_arg    flash;
        struct pfx_continue_arg continues;
        struct pfx_chase_arg    chase;
    } mode_arg;
};

struct db_custom_fx
{
    uint16_t lightness;
    uint8_t  ctrl;
    uint8_t  loop;
    uint8_t  sequence;
    uint8_t  chaos;
    uint8_t  speed;
    uint8_t  bank;
    uint8_t  type;
    
};

struct db_custom_preview_fx
{
    struct cfx_frame_arg
    {
        enum sidus_fx_base base;
        union sidus_fx_base_normal_arg base_arg;
    } frame_1, frame_2;
};

struct db_local_custom_fx
{
    uint8_t ctrl;
    uint8_t type;
    uint8_t bank[3];
    uint16_t lightness[3][10];
};

/******************************************************************************
LIGHT_MODE_PIXEL_FX
******************************************************************************/
enum pixel_groud_unit
{
    SINGLE_GROUND,
    DOUBLE_DOUBLE,
};

enum pixel_run_status
{
    PLAY_STATUS,
    PAUSE_STATUS,
    STOP_STATUS,
};

union pixel_color_base_arg
{
    struct 
    {
        uint16_t cct;
        uint16_t gm;        
    }cct;
    struct
    {
        uint16_t hue;
        uint16_t cct;   //中心白点色温  默认6500k
        uint16_t sat;
    }hsi;
};

struct color_block_arg
{
    uint16_t lightness;
    uint8_t color_mode;
    union pixel_color_base_arg color_sel;
};

struct pixel_fire_color_base_arg
{
    uint16_t lightness_min;
    uint16_t lightness_max;
    uint8_t color_mode;/*0 - cct,1 - hsi, 2 - black*/
    union pixel_color_base_arg color_sel;
};

struct db_color_fade
{
    uint8_t colors;     //颜色数量
    uint8_t dir;        //0--左 1--右
    float speed;        //速度  单位：cm/s
    struct color_block_arg color_arg[PIXEL_FX_COLORS_MAX];
    enum pixel_run_status status;
};

struct db_color_cycle
{
    uint8_t colors;     //颜色数量
    uint8_t dir;        //0--左 1--右
    uint16_t period;    //颜色切换周期  单位：ms
    uint8_t color_trans_mode;  //颜色过渡模式   0--瞬变  1--缓变
    struct color_block_arg color_arg[PIXEL_FX_COLORS_MAX];
    enum pixel_run_status status;
};

struct db_rainbow
{
    uint16_t lightness;
    uint8_t dir;    //0--左 1--右
    float speed;    //速度  单位：cm/s
    enum pixel_run_status status;
};

struct db_one_color_chase
{
    enum pixel_groud_unit group;  /*0 - 单组，1 - 双组*/
    uint8_t dir;    /*单组：0 - 左，1 - 右，2 - 来回；双组：0 - 交错，1 - 反弹*/
    uint8_t length; /*总长度 三挡 S/M/L -> */
    float speed;      //速度  单位：cm/s
    struct color_block_arg color_arg[3];
    enum pixel_run_status status;
};

struct db_two_color_chase
{
    enum pixel_groud_unit group;  /*0 - 单组，1 - 双组*/
    uint8_t dir;    /*单组：0 - 左，1 - 右，2 - 来回；双组：0 - 交错，1 - 反弹*/
    uint8_t length; /*总长度 三挡 S/M/L -> */
    float speed;      //速度  单位：cm/s
    struct color_block_arg color_arg[5];
    enum pixel_run_status status;
};

struct db_three_color_chase
{
    enum pixel_groud_unit group;  /*0 - 单组，1 - 双组*/
    uint8_t dir;    /*单组：0 - 左，1 - 右，2 - 来回；双组：0 - 交错，1 - 反弹*/
    uint8_t length; /*总长度 三挡 S/M/L -> */
    float speed;      //速度  单位：cm/s
    struct color_block_arg color_arg[7];
    enum pixel_run_status status;
};

struct db_pixel_fire
{
    float frq;/*火焰频率：0.1hz - 10hz*/
    uint8_t dir;/*火焰方向：0 - 横向，1 - 纵向*/
    struct pixel_fire_color_base_arg color_arg;     //火焰颜色参数
    struct color_block_arg color_background_arg;   //背景颜色参数
    enum pixel_run_status status;   
};

/******************************************************************************
LIGHT_MODE_PARTITION_FX
******************************************************************************/
union partition_color_arg
{
	struct
    {
        uint16_t  lightness;  		/*亮度<0-1000>*/
        uint16_t  cct;  			/*CCT，单位k*/
        uint8_t   gm;   			/*<0-20>: 0->-1.0,1->-0.9,2->-0.8,   10->0,20->+1.0每隔0.1递增1，以此类推*/
    }cct;
	struct
    {
        uint16_t  lightness;  		/*亮度<0-1000>*/
        uint16_t  hue;  			/*hue<0-360>，0-360,360-0*/
        uint16_t  sat;  			/*饱和度<0-1000>*/
        uint16_t  cct;  			/*中心色温*/
    }hsi; 
};

struct part_fx_arg
{    
    struct
    {
        uint8_t trigger;    /*0->像素统一,1->像素独立*/
        float frq_max;      /*最大频率0.1-20Hz*/ 
        float frq_min;      /*最小频率0.1-20Hz*/   
        uint32_t interval_max;    /*100 - 60000ms，最大间隔时间*/
        uint32_t interval_min;    /*100 - 60000ms，最小间隔时间*/
        uint32_t lasting_max;    /*100 - 60000ms,最大持续时间*/
        uint32_t lasting_min;    /*100 - 60000ms,最小持续时间*/
    }fx_arg;
    uint8_t int_min;        /*<0-100>,精度1%*/ 
};

union partition_fx_arg
{
	struct part_fx_arg   Strobe;
    struct part_fx_arg   Flicker;
    struct part_fx_arg   Pulsing;
};

struct db_partition_data
{
    uint64_t            target :36;     /* 有效位域，1值生效 */
    uint8_t             state;          /* 叠加光效状态，0x00->开启，0x01->不开启 */
    uint8_t             light_mode[PARTITION_MAX_NUM];  /* 分块的颜色模式  CCT或HSI */
    union partition_color_arg  color_arg[PARTITION_MAX_NUM];
    uint8_t             partition_num;   /* 分区数 */
    uint8_t fx_mode;         /* 分区光效模式 */
    union partition_fx_arg  fx_mode_arg;
};

struct db_partition_fx
{
    uint8_t fx_mode;         /* 分区光效模式 */
    union partition_fx_arg  fx_arg;
};

struct db_partition_cfg
{
    uint8_t xy_mode;
    uint8_t pixel_x1;
    uint8_t pixel_y1;
    uint8_t pixel_x2;
    uint8_t pixel_y2;
};

struct db_dmx
{
    uint8_t     strobe_or_fx_state;
    uint32_t    fade;
    dmx_data_t  dmx_data;
};


enum sys_trigger_type
{
    SYS_TRIGGER_FIND_LAMP,
    
};



/* 电源状态信息 */
struct sys_info_power {
    uint8_t state;          //电源开关状态
    uint8_t max_power_flag; //最大功率
    uint8_t charge_state;   //充电状态
    uint8_t batt_time;      //电池使用时间
    uint8_t batt_level;     //电池电量（格数）
    uint8_t batt_voltage;   //电池电压
	uint8_t indoor_powe;    //室内供电状态
	uint16_t ratio;
};

/* USB状态信息 */
struct sys_info_usb {
    uint8_t state;                  //U盘状态   0--拔出  1--插入  3--格式错误 
    uint8_t update_firmware_state;  //升级固件状态
    uint8_t update_firmware_type;    //固件类型
    uint8_t percent;                  //升级百分比
}; 

/* 以太网状态信息 */
struct sys_info_eth {
    uint8_t pluged;         //以太网是否插入
    uint8_t mac_address[6]; // MAC地址
};

/* 灯体信息 */
struct sys_info_lamp {
	uint32_t	product_num;
    uint8_t  exist;             //灯体是否存在
    uint8_t  hard_ver[3];       //硬件版本
    uint8_t  soft_ver[3];       //软件版本
	uint8_t  motorized_yoke_soft_ver[3];       //软件版本
	uint8_t  motorized_yoke_hw_ver[3];       //硬件件版本
	uint8_t  motorized_fresnel_soft_ver[3];       //软件版本
	uint8_t  motorized_fresnel_hw_ver[3];       //软件版本
    uint16_t fan_speed;         //风扇速度
    int16_t  ambient_temp;      //环境温度
    uint8_t  cal_power_state;   //校准功率状态
    int16_t   cob_temp;          //光源温度
	int16_t   fre_temp;          //菲涅尔温度温度
	int16_t   fre2_temp;          //菲涅尔温度温度
	uint16_t  led_run_time;       //软件版本
};

/* 蓝牙 */ 
struct sys_info_ble {
    uint8_t reset_state;    //复位状态
    uint8_t power_state;    //电源状态
};

/* CRMX模块 */
struct sys_info_crmx {
    uint8_t pair_state;     //配对状态
    uint8_t paired;         //是否配过配对
	uint8_t crmx_link_streng;
};

/* DMX信息 */
struct sys_info_dmx {
    uint8_t wired_linked;         //有线DMX连接状态.
    uint8_t wireless_linked;      //无线DMX连接状态.
    uint8_t eth_linked;           //以太网DMX连接状态.
};

/* 控制盒信息 */
struct sys_info_ctrl_box {

    uint8_t mode;         //当前控制方式    0---手动控制   1---蓝牙控制
    int8_t  ambient_temp;      //环境温度
    int8_t  mcu_temp;       //mcu环境温度
    uint16_t fan_speed;         //风扇速度
    uint8_t  hw_ver[3];            //硬件版本信息
    uint8_t soft_ver[3];           //软件版本信息
    uint16_t  pan_angle;
    uint16_t  till_angle;
    uint16_t  scroll_angle;
	uint16_t  power;
	uint8_t      lcd_lock;
};


struct cfx_name_str
{

    char name[10][10];
};
//cfx name  信息 
struct  sys_info_cfx_name
{
    uint8_t state;   //0---读   1---写
    uint8_t cfx_type;
    uint8_t cfx_bank;
    uint8_t read_cfx_name[10];
    struct cfx_name_str cfx_name[3];   
};

struct  sys_info_accessories
{
    uint8_t fresnel_state;   
    uint8_t yoke_state;                 //0---没有装载电动支架       1---装载电动支架
    uint8_t optical_20_access_state;    //0---没有装载20°光学附件    1---转载20°   
    uint8_t optical_30_access_state;    //0---没有装载30°光学附件    1---转载30°
    uint8_t optical_50_access_state;    //0---没有装载50°光学附件    1---转载50°
	uint8_t adapter_ring;     //
};

struct sys_info_motor{
    float     pan_angle;       /*0~360°   角度*/
    float     tilt_angle;      /*0~360°   角度*/
    float     fresnel_angle;    /*0~360°   角度*/
	float     roll_angle;    /*0~360°   角度*/
    uint8_t      reset_state;   /*0~1      复位状态*/
    uint16_t     reset_angle;   /*0~360°   复位后的角度*/
}; 

struct sys_info_motor_state{
    uint8_t      crc_state;     /*0-1      堵转校准状态*/
    uint16_t     crc_value;     /*0~1024   堵转负载反馈值*/
    uint8_t      pan_stall_state;   /*0~1      水平旋转电机堵转状态*/
    uint8_t      tilt_stall_state;   /*0~1      俯仰旋转电机堵转状态*/
    uint8_t      fresnel_stall_state;   /*0~1      电动菲涅尔旋转电机堵转状态*/
	uint32_t        fresnel_loop_count;          
};   

struct sys_info_motor_test{
    uint16_t     motor_test_state;     /*0-1      堵转校准状态*/
    uint16_t     motor_test_count;     /*0~1024   堵转负载反馈值*/
};

struct sys_info_uuid{
    uint8_t     uuid[20]; 
}; 

//update file name  信息 
struct  sys_info_update_name
{
	char file_number;
    char name[10][80];  
	char usb_name[10][80]; 
};

enum sys_iot_error_type
{
    EV_IOT_ERROR_FAN1 = 0,
    EV_IOT_ERROR_FAN2,
    EV_IOT_ERROR_FAN3,
    EV_IOT_ERROR_FAN4,
    EV_IOT_ERROR_FAN5,
    EV_IOT_ERROR_FAN6,
    EV_IOT_ERROR_TEMP_SENSOR1,
    EV_IOT_ERROR_TEMP_SENSOR2,
    EV_IOT_ERROR_TEMP_SENSOR3,
    EV_IOT_ERROR_TEMP_SENSOR4,
    EV_IOT_ERROR_TEMP_SENSOR5,
    EV_IOT_ERROR_TEMP_SENSOR6,
    EV_IOT_ERROR_BAT1_VOLT,
    EV_IOT_ERROR_BAT2_VOLT,
    EV_IOT_ERROR_BAT3_VOLT,
    EV_IOT_ERROR_BAT4_VOLT,
    EV_IOT_ERROR_BAT5_VOLT,
    EV_IOT_ERROR_BAT6_VOLT,
    EV_IOT_ERROR_CTR_INPUT_VOLT,
    EV_IOT_ERROR_DRV_INPUT_VOLT,
    EV_IOT_ERROR_CTR_CURR,
    EV_IOT_ERROR_DRV_CURR,
    EV_IOT_ERROR_CTR_3D3V,
    EV_IOT_ERROR_CTR_5V,
    EV_IOT_ERROR_CTR_9V,
    EV_IOT_ERROR_CTR_12V,
    EV_IOT_ERROR_CTR_15V,
    EV_IOT_ERROR_DRV_3D3V,
    EV_IOT_ERROR_DRV_5V,
    EV_IOT_ERROR_DRV_9V,
    EV_IOT_ERROR_DRV_12V,
    EV_IOT_ERROR_DRV_15V,
    EV_IOT_ERROR_RS485,
    EV_IOT_ERROR_STORE,
    EV_IOT_ERROR_PD,
    EV_IOT_ERROR_WIRELESS_PD,
    EV_IOT_ERROR_HIGH_TEMP,
    EV_IOT_ERROR_USB_UPDATE,
    EV_IOT_ERROR_USB_MOUNT,
    EV_IOT_ERROR_BLE_UPDATE,
    EV_IOT_MAX_ERROR_TYPE,
};


typedef __packed struct 
{
	uint32_t led_mount_eh_001				   : 1;
	uint32_t led_temp_eh_002				   : 1;
	uint32_t mcu_temp_eh_003 				   : 1;
	uint32_t battery_voltage_eh_004			   : 1;
	uint32_t hard_drive_eh_005				   : 1;
	uint32_t ctrl_communications_eh_006 	   : 1;
	uint32_t ble_communications_eh_007 		   : 1;
	uint32_t sq_net_eh_008					   : 1;
	uint32_t lumenridio_eh_009 				   : 1;
	uint32_t fan_eh_010 					   : 1;
	uint32_t gyroscope_eh_011 				   : 1;
	uint32_t cool_eh_012 					   : 1;
	uint32_t electric_support_eh_013 		   : 1;
	uint32_t electric_accessories_eh_014 	   : 1;
	uint32_t flash_eh_015 					   : 1;
	uint32_t ambient_temp_eh_016 			   : 1;
	uint32_t lamp_head_not_match_eh_017   	   : 1;
	uint32_t motorized_f14_Fresnel_018   	   : 1;
	uint32_t motorized_yoke_panning_019   	   : 1;
	uint32_t motorized_yoke_tilting_020   	   : 1;
	uint32_t motorized_f14_ambient_temp_eh_021 : 1;
	uint32_t motorized_f14_motor_temp_eh_022   : 1;
	uint32_t motorized_f14_ic_temp_eh_023      : 1;
	uint32_t reserve_eh_024   	               : 1;
	uint32_t reserve_eh_025   	               : 1;
	uint32_t reserve_eh_026   	               : 1;
	uint32_t reserve_eh_027   	               : 1;
	uint32_t reserve_eh_028   	               : 1;
	uint32_t reserve_eh_029   	               : 1;
	uint32_t reserve_eh_030   	               : 1;
	uint32_t reserve_eh_031   	               : 1;
	uint32_t reserve_eh_032   	               : 1;
}sys_error_eh_type;


typedef __packed struct 
{
	uint32_t led_temp_ec_001				: 1;
	uint32_t mcu_temp_ec_002 				: 1;
	uint32_t adapter_output_ec_003 			: 1;
	uint32_t battery_voltage_ec_004			: 1;
	uint32_t hard_drive_ec_005				: 1;
	uint32_t ctrl_communications_ec_006 	: 1;
	uint32_t ble_communications_ec_007 		: 1;
	uint32_t sq_net_ec_008 					: 1;
	uint32_t lumenridio_ec_009 				: 1;
	uint32_t fan_ec_010 					: 1;
	uint32_t flash_ec_011 					: 1;
	uint32_t abnormal_power_ec_012 			: 1;
	uint32_t dc_unbalanced_power_ec_013 	: 1;
	uint32_t dc_interface_voltage_ec_014 	: 1;
	uint32_t reserve_ec_015 				: 1;
	uint32_t reserve_ec_016 				: 1;
	uint32_t reserve_ec_017   	            : 1;
	uint32_t reserve_ec_018   	            : 1;
	uint32_t reserve_ec_019   	            : 1;
	uint32_t reserve_ec_020   	            : 1;
	uint32_t reserve_ec_021   	            : 1;
	uint32_t reserve_ec_022   	            : 1;
	uint32_t reserve_ec_023   	            : 1;
	uint32_t reserve_ec_024   	            : 1;
	uint32_t reserve_ec_025   	            : 1;
	uint32_t reserve_ec_026   	            : 1;
	uint32_t reserve_ec_027   	            : 1;
	uint32_t reserve_ec_028   	            : 1;
	uint32_t reserve_ec_029   	            : 1;
	uint32_t reserve_ec_030   	            : 1;
	uint32_t reserve_ec_031   	            : 1;
	uint32_t reserve_ec_032   	            : 1;
}sys_error_ec_type;


typedef __packed union
{
	uint64_t error_data;
	__packed struct 
	{
		sys_error_eh_type error_eh;
		sys_error_ec_type error_ec;
	};
}sys_error_type;


#pragma pack()

#endif

