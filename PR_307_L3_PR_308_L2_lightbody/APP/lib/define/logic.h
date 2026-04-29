/****************************************************************************************
**  Filename :  111.h
**  Abstract :  。
**  By       :  何建国
**  Date     :  
**  Changelog:1.First Create
*****************************************************************************************/
#ifndef _LOGIC_H
#define _LOGIC_H
/*****************************************************************************************
									Includes
*****************************************************************************************/
#include "rs485_protocol.h"
#include "hc32_ddl.h"
#include "color_mixing_dev.h"
/*****************************************************************************************
								Macro definitions
*****************************************************************************************/
#define LED_CHANNEL_NUM				7
/*****************************************************************************************
								Typedef definitions
*****************************************************************************************/
#pragma pack (1)
//-----------------------接收485信号进行逻辑处理
/*参数改变信号*/
typedef struct{
    uint8_t fan;
    uint8_t dim;
    uint8_t frq;
    uint8_t on_off;
    uint8_t upgrade;
    uint8_t Illumination_Mode;
    uint8_t light_mode;
    uint8_t here_fx;
	uint8_t adjust_hight_mode;
	uint8_t pump_t;
	uint8_t self_adjust;
}model_change_msg_arg_t;
/*光模式状态参数，光模式间有互斥*/
typedef struct{
    uint8_t command_1;//指令
    uint8_t command_2;//光效类型
    uint8_t command_3;//灯带光效
}light_mode_state_arg_t;
/*CCT HSI RGB SOURCE XY GEL六种常规光模式的参数汇总*/
typedef struct{
    float       intensity;      /*亮度：<0-1.0>，1.0代表100%*/
    uint16_t    cct;            /*色温：<0-65535>K*/
    float       duv;            /*浮点数 <-0.04~0.04>或<-0.02~0.02>,产品不同，范围有所不同，默认0.00*/
    uint16_t    hue;            /*色调：<0-36000>，36000代表360°*/
    uint16_t    sat;            /*饱和度：<0-10000>，10000代表100%*/
    uint8_t     origin;         /*色纸品牌：0->LEE 1->Resco*/
    uint8_t     series;         /*色纸系列：<0-255>*/
    uint16_t    color;          /*色纸颜色：<0-65535>*/
    float       red_ratio;      /*红灯比例：<0-10000>*/
    float	    green_ratio;    /*绿灯比例：<0-10000>*/
    float	    blue_ratio;     /*蓝灯比例：<0-10000>*/
    float	    ww_ratio;       /*暖白灯比例：<0-10000>*/
    float	    cw_ratio;       /*冷白灯比例：<0-10000>*/
	float	    ww2_ratio;       /*暖白灯比例：<0-10000>*/
    float	    cw2_ratio;       /*冷白灯比例：<0-10000>*/
    float	    x;              /*X色坐标：<0-10000>，除以10000表示实际色坐标*/
    float	    y;              /*Y色坐标：<0-10000>，除以10000表示实际色坐标*/
    uint8_t     type;           /*光源类型：<0-255>*/
    uint32_t    fade;
    uint16_t    pixel_x;
    uint16_t    pixel_y;
}light_mode_para_t;
/*常规光效参数*/
typedef struct{
    uint16_t                intensity;      /*亮度：<0-1000>*/
    uint16_t                cct;            /*色温：<2000-10000>K*/
    uint8_t                 gm;             /*偏色：<0-20>-><-1.0 - 1.0>，精度0.1*/
    uint16_t                hue;            /*色调：<0-360>，0-360,360-0*/
    uint8_t                 sat;            /*饱和度<0-100>*/
    uint8_t                 origin;         /*色纸品牌：0x00->LEE; 0x01->Resco*/
    uint8_t                 series;         /*色纸系列：<0-255>*/
    uint16_t                color;          /*色纸颜色：<0-65535>*/
    float	                x;              /*X色坐标:<0-10000>，除以10000表示实际色坐标*/
    float	                y;              /*Y色坐标:<0-10000>，除以10000表示实际色坐标*/
    uint8_t                 type;           /*光源类型：<0-255>*/
    uint8_t                 frq;            /*频率：<1-11>,11为Random*/
    uint8_t                 color_num;      /*颜色数量：<0-7>,0:3; 1:6; 2:9; 3:12 4:15; 5:18; 6:24; 7:36*/
    uint8_t   trigger;        /*<0-2>,0-无触发/1-单次触发/2-多次触发*/
    uint8_t                 spd;            /*速度：<1-11>,11为Random*/
    uint8_t                 mode;           /*0x00:CCT；0x01:HSI；0x02:GEL；0x03:XY；0x04:Source*/
    uint16_t                min_int;        /*最低亮度：<0-1000>*/
    /*下面是二代光效专用*/
    uint16_t                gap_time;       /*<1-300>,<0.1-30秒>精度0.1秒*/
    uint16_t                min_gap_time;   /*<1-300>,<0.1-30秒>精度0.1秒*/
    rs485_fx_2_trigger_enum state;          /*0-停止 ,2-循环*/
    float                   duv;            /*浮点数 <-0.04~0.04>或<-0.02~0.02>,产品不同，范围有所不同，默认0.00*/
    float	                red_ratio;      /*红光比例：<0-1000>*/
    float	                green_ratio;    /*绿光比例：<0-1000>*/
    float	                blue_ratio;     /*蓝光比例：<0-1000>*/
    uint16_t                cct_upper;      /*色温上限：<0-65535>，单位k*/
    uint16_t                cct_lower;      /*色温下限：<0-65535>，单位k*/
    uint16_t                hue_upper;      /*色调上限：<0-360>，0-360,360-0*/
    uint16_t                hue_lower;      /*色调下限：<0-360>，0-360,360-0*/
    uint8_t                 cct_range;      /*0->Warmer 1400K-1700K,1->Neutral 1700k-2000k,2->Cooler 2000K-2300K*/
    uint8_t                 decay;          /*<1-10>*/
    uint16_t                max_int;        /*亮度<0-1000>*/
    uint8_t                 pulsing_num;    /*<20-200>*/
}fx_mode_para_t;


/*像素光效参数*/
typedef struct{
    uint16_t                cct;                /*CCT，单位k*/
    float                   duv;                /*浮点数 <-0.04~0.04>或<-0.02~0.02>,产品不同，范围有所不同，默认0.00*/
    uint16_t                hue;                /*hue<0-360>，0-360,360-0*/
    uint8_t                 sat;                /*饱和度<0-100>*/
    uint8_t                 pixel_num;          /*此光效的像素个数：4个*/
    uint8_t                 pixel_index;        /*像素序号：<0-3> 0->光柱颜色1, ..., 3->光柱颜色4*/
    uint16_t                intensity;          /*亮度<0-1000>*/
    uint8_t                 light_cross_len;    /*光柱长度：<0-143>-><1-144>*/
    uint8_t                 dir;                /*方向：<0-2>-><1-3>*/
    uint16_t                spd;                /*速度：<0-299>-><0.1-30>精度0.1*/
    uint8_t                 state;              /*0-停止 2-循环*/
    uint8_t                 mode;               /*0x00:CCT；0x01:HSI；0x02:Black*/
    uint8_t                 color_num;          /*颜色数量：<0-143>-><1-144>*/
    uint8_t                 dir_group;          /*方向组：0->单组，1->双组*/
    uint16_t                min_int;            /*亮度<0-1000>*/
    uint8_t                 color;              /*0*/
    uint8_t                 frq;                /*<0-10> -> <1-11>,11是随机*/
    uint16_t                light_cross_color;  /*光柱颜色：<0 - 359> -> <1°-360°>*/
    uint16_t                base_color;         /*基础颜色：<0 - 359> -> <1°-360°>*/
}pixel_fx_para_t;

/*高速摄影模式选择*/
typedef struct{
	uint8_t					mode;
}high_speed_t;
/*自适应pwm*/
typedef struct{
	uint16_t					pwm[7];
}self_adapt_pwm_t;

typedef struct{
    model_change_msg_arg_t      new_command;//用于全局指令的触发，只能赋1or0
    light_mode_state_arg_t      light_mode_state;
    
    light_mode_para_t           light_mode_para;
    fx_mode_para_t              fx_mode_para;
    pixel_fx_para_t             pixel_fx_para;
    uint8_t                     fx_restart;         /*光效重新启动标志*/
    
    rs485_fan_t                 fan;
    rs485_dimming_curve_t       dimming_curve;
    rs485_dimming_frq_t         dimming_frq;
    rs485_illumination_mode_t   illumination_mode;
    rs485_switch_t              power_switch;
    rs485_fx_i_am_here_arg_t    i_am_here;
	high_speed_t				high_speed;
	rs485_pump_t	            pump_t;
	rs485_self_adjust_t         self_adjust_t;
	rs485_motor_t				motor_t;
	rs485_factory_rgbww_crc_t   factory_rgbww_crc_t;
	self_adapt_pwm_t   			self_adapt_pwm;
}rs485_data_ctr_t;


//auto adapt logit
typedef struct
{
	uint16_t trig_times;
	uint8_t  obj_level;
//	uint8_t  stable_flag;
}Adapt_H_Para_TypeDef;
typedef struct
{
	uint16_t trig_times;
	uint8_t  obj_level;
	uint8_t  stable_state;
}Adapt_L_Para_TypeDef;
typedef struct
{
	Adapt_H_Para_TypeDef high;
	Adapt_L_Para_TypeDef low;
}Adapt_Unit_TypeDef;



#pragma pack ()

#endif
/***********************************END OF FILE*******************************************/
