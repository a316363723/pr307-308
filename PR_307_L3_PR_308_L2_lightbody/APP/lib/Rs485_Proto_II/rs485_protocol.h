/*********************************************************************************
  *Copyright(C), 2022, Aputure, All rights reserver.
  *FileName:     rs485_protocol.h
  *Author:
  *Version:       v1.0
  *Date:          2022/02/21
  *Description: 定义主机与从机通信的数据包格式，第二版485通讯协议
  *History:
        -v1.0: 初始版本
        -v1.0.1:2022/05/26
        1、优化了二代系统光效协议；
        2、优化了点控光效协议；
        3、优化了分区光效协议；

**********************************************************************************/
#ifndef __RS485_PROTOCOL_H
#define __RS485_PROTOCOL_H

#include <stdint.h>

#ifdef    __GNUC__
    #define    PACKED    __attribute__((packed))

#elif defined ( __CC_ARM )
    #define PACKED __packed

#else
    #error "We do not know how your compiler packs structures"
#endif

#define DEFINE_STRUCT               PACKED struct
#define TYPEDEFINE_STRUCT           typedef PACKED struct
#define DEFINE_BODY(s)              s ## _Body_TypeDef

#define TYPEDEFINE_ENUM             typedef enum

#define DEFINE_EFFECT_ARG(s)        s ## _Arg_TypeDef


/***************************************************************************************************
--Protocol:

-通信数据包由2部分组成：
-    1、起始包(数据帧头，校验值，头信息长度，总数据长度(2Byte)，协议版本(3Byte)，消息序号(2Byte)，应答使能，目前长度9byte，可在后面随意扩展)：11 byte
-    2、命令包(长度任意可变，可携包含多包数据)：X byte
-    3、具体协议请参考第二代485协议文件   

***************************************************************************************************/
#define RS485_START_CODE            ((uint8_t)0X55)
#define RS485_PROTO_VER_MAJOR       ((uint8_t)0X01)
#define RS485_PROTO_VER_MINOR       ((uint8_t)0X00)
#define RS485_PROTO_VER_REVISION    ((uint8_t)0X00)

#define RS485_MESSAGE_MAX_SIZE      ((uint16_t)1024)
#define RS485_HEADER_SIZE           ((uint8_t)11)
#define RS485_HEADER_NO_CHECK_LEN   (2)
#define RS485_CMD_BODY_MAX_LENGTH   (RS485_MESSAGE_MAX_SIZE - RS485_HEADER_SIZE)    //数据体的最大长度

#define RS485_COMMAND_READ          ((uint8_t)0x00)    //读
#define RS485_COMMAND_WRITE         ((uint8_t)0x01)    //写

#define RS485_ACK_ENABLE            ((uint8_t)0X01)
#define RS485_ACK_DISABLE           ((uint8_t)0X00)

#pragma pack (1)
/*数据帧格式：1个起始包 +  1个或多个命令包*/
/*------------------------------------- 起始包 ----------------------------------------------*/
typedef struct{
    uint8_t     start;              /*帧起始标志：0X55*/
    uint8_t     check_sum;          /*帧校验和：消息数据中除Start和CheckSum两个字节外所有字节的累加和，不包含start和check_sum*/
    uint8_t     header_size;        /*头信息包长度：11，可以在后面扩展*/
    uint16_t    msg_size;           /*message总长度*/
    uint8_t     proto_ver_major;    /*协议主版本*/
    uint8_t     proto_ver_minor;    /*协议次版本号*/
    uint8_t     proto_ver_revision; /*协议修订版本号*/
    uint16_t    serial_num;         /*帧序：<0-65535>,循环使用，主机每次下发的命令包序都不同*/
    uint8_t     ack_en;             /*从机应答使能：0-从机禁止应答，1-从机使能应答*/
}rs485_proto_header_t;              /*size: 11*/  

/*------------------------------------- 命令包 ----------------------------------------------*/

/*---命令类型---*/
typedef enum{
    RS485_Cmd_Version = 0,
    RS485_Cmd_HSI,
    RS485_Cmd_CCT,
    RS485_Cmd_GEL,
    RS485_Cmd_RGB,
    RS485_Cmd_XY_Coordinate,
    RS485_Cmd_Dim_Frq,
    RS485_Cmd_Sys_FX,
    RS485_Cmd_Dimming_Curve,
    RS485_Cmd_Fan,
    RS485_Cmd_Power_Suppy,
    RS485_Cmd_Battery_State,
    RS485_Cmd_Switch,
    RS485_Cmd_FileTransfer,
    RS485_Cmd_Temperature_Msg,
    RS485_Cmd_Sys_FX_II,
    RS485_Cmd_Self_Adaption = 0x10,
    RS485_Cmd_Factory_RGBWW,
    RS485_Cmd_Source,
    RS485_Cmd_IlluminationMode,
    RS485_Cmd_Color_Mixing,
    RS485_Cmd_Err_Msg,
    RS485_Cmd_PFX_Ctrl,
    RS485_Cmd_CFX_Bank_RW,
    RS485_Cmd_CFX_Ctrl,
    RS485_Cmd_CFX_Preview,
    RS485_Cmd_MFX_Ctrl,
    RS485_Cmd_RGBWW,
    RS485_Cmd_CFX_Name,
    RS485_Cmd_Curr_Light_Mode,
    RS485_Cmd_PixelEffect,
    RS485_Cmd_DMX_Strobe,
    RS485_Cmd_Partition_Color = 0x20,
    RS485_Cmd_Partition_Effect,
    RS485_Cmd_High_speed,     
    RS485_Cmd_Analog_Dim,
    RS485_Cmd_Pump,
    RS485_Cmd_Self_Adjust,
    RS485_Cmd_Motor,
    RS485_Cmd_Accessories, 
	RS485_Cmd_Version2,
	RS485_Cmd_Factory_RGBWW_Crc,   	//0x29
	RS485_Cmd_Clear_Run_Time,   	//0x2A
    RS485_Cmd_Null,
}rs485_cmd_enum; 

/*----------Header----------*/
typedef struct{
    uint8_t             rw;         /*操作类型：0X00-读，0X01-写*/
    rs485_cmd_enum      cmd_type;   /*命令类型*/
    uint16_t            arg_size;   /*单个命令包(cmd_arg)长度：0-512*/
}rs485_cmd_header_t;              /*size: 4*/

/*----------命令包----------*/
/*------------------------------------*/
/*----------Version：0x00----------*/
/*------------------------------------*/
typedef struct{
    uint32_t  product_num;      /*Product，产品类型，0x006B表示107*/
    uint8_t   hard_ver_major;   /*硬件主版本号 版本号定义：主.次.修订 如0x01 0x00 0x00 表示1.0.0 两位版本号可舍弃修订版本号*/
    uint8_t   hard_ver_minor;   /*硬件次版本号*/
    uint8_t   hard_ver_revision;/*硬件修订版本号*/
    uint8_t   soft_ver_major;   /*软件主版本号 版本号定义：主.次.修订 如0x01 0x00 0x00 表示1.0.0 两位版本号可舍弃修订版本号*/
    uint8_t   soft_ver_minor;   /*软件次版本号*/
    uint8_t   soft_ver_revision;/*软件修订版本号*/
}rs485_version_t;               /*size: 10*/

/*------------------------------------*/
/*----------HSI：0x01------------*/
/*------------------------------------*/
typedef struct{
    float       intensity;      /*亮度：<0-1.0>，1.0代表100%*/
    uint16_t    hue;            /*色调：<0-36000>，36000代表360°*/
    uint16_t    sat;            /*饱和度：<0-10000>，10000代表100%*/
    uint32_t    fade;           /*缓变时间：单位ms <0-120000>*/
    uint16_t    cct;            /*中心白点色温：<0-65535>K*/
	float       duv;            /*浮点数 <-0.04~0.04>或<-0.02~0.02>,产品不同，范围有所不同，默认0.00*/
    uint16_t    pixel_x;        /*点控通道X坐标：<0-65535>*/
    uint16_t    pixel_y;        /*点控通道Y坐标：<0-65535>*/
}rs485_hsi_t;              /*size: 18*/

/*------------------------------------*/
/*----------CCT：0x02-----------*/
/*------------------------------------*/
typedef struct{
    float       intensity;      /*亮度：<0-1.0>，1.0代表100%*/
    uint16_t    cct;            /*色温：<0-65535>K*/
    float       duv;            /*浮点数 <-0.04~0.04>或<-0.02~0.02>,产品不同，范围有所不同，默认0.00*/
    uint32_t    fade;           /*缓变时间：单位ms <0-120000>*/
    uint16_t    pixel_x;        /*点控通道X坐标：<0-65535>*/
    uint16_t    pixel_y;        /*点控通道Y坐标：<0-65535>*/
}rs485_cct_t;              /*size: 18*/

/*------------------------------------*/
/*----------GEL：0x03-----------*/
/*------------------------------------*/
typedef struct{
    float       intensity;      /*亮度：<0-1.0>，1.0代表100%*/
    uint16_t    cct;            /*色温：<0-65535>K*/
    uint8_t     origin;         /*色纸品牌：0->LEE 1->Resco*/
    uint8_t     series;         /*色纸系列：<0-255>*/
    uint16_t    color;          /*色纸颜色：<0-65535>*/
    uint32_t    fade;           /*缓变时间：单位ms <0-120000>*/
    uint16_t    pixel_x;        /*点控通道X坐标：<0-65535>*/
    uint16_t    pixel_y;        /*点控通道Y坐标：<0-65535>*/
}rs485_gel_t;              /*size: 18*/

/*------------------------------------*/
/*----------RGB：0x04---------*/
/*------------------------------------*/
typedef struct{
    float       intensity;      /*亮度：<0-1.0>，1.0代表100%*/
    float    red_ratio;      /*红灯比例：<0-10000>*/
    float    green_ratio;    /*绿灯比例：<0-10000>*/
    float    blue_ratio;     /*蓝灯比例：<0-10000>*/
    float    fade;           /*缓变时间：单位ms <0-120000>*/
    uint16_t    pixel_x;        /*点控通道X坐标：<0-65535>*/
    uint16_t    pixel_y;        /*点控通道Y坐标：<0-65535>*/
}rs485_rgb_t;                   /*size: 18*/

/*------------------------------------*/
/*------XY_Coordinate：0x05-----*/
/*------------------------------------*/
typedef struct{
    float       intensity;      /*亮度：<0-1.0>，1.0代表100%*/
    float    x;              /*X色坐标：<0-10000>，除以10000表示实际色坐标*/
    float    y;              /*Y色坐标：<0-10000>，除以10000表示实际色坐标*/
    uint32_t    fade;           /*缓变时间：单位ms <0-120000>*/
    uint16_t    pixel_x;        /*点控通道X坐标：<0-65535>*/
    uint16_t    pixel_y;        /*点控通道Y坐标：<0-65535>*/
}rs485_xy_coord_t;              /*size:16*/

/*------------------------------------*/
/*-----Dim_Frq：0x06------*/
/*------------------------------------*/
typedef struct{
    int16_t  adjust_val;        /*调节频率值：<-32767 - +32767>HZ*/
}rs485_dimming_frq_t;           /*size: 2*/

/*------------------------------------*/
/*---------Light_Effect：0x07---------*/
/*------------------------------------*/

/*---一代光效类型---*/
typedef enum{
    RS485_FX_ClubLights = 0,
    RS485_FX_Paparazzi,
    RS485_FX_Lightning,
    RS485_FX_TV,
    RS485_FX_Candle,
    RS485_FX_Fire,
    RS485_FX_Strobe,
    RS485_FX_Explosion,
    RS485_FX_FaultBulb,
    RS485_FX_Pulsing,
    RS485_FX_Welding,
    RS485_FX_CopCar,
    RS485_FX_ColorChase,
    RS485_FX_PartyLights,
    RS485_FX_Fireworks,
    RS485_FX_Effect_Off,
    RS485_FX_I_Am_Here,
    RS485_FX_Null,
}rs485_fx_type_enum;

/*---一代光效里的子类型---*/
typedef enum{
    RS485_FX_Mode_CCT = 0,
    RS485_FX_Mode_HSI,
    RS485_FX_Mode_GEL,
    RS485_FX_Mode_XY,
    RS485_FX_Mode_Source,
	RS485_FX_Mode_RGB,
    RS485_FX_Mode_Null,
}rs485_fx_mode_enum;

/*---一代光效触发类型---*/
typedef enum{
    RS485_FX_Trigger_None = 0,
    RS485_FX_Trigger_Once,
    RS485_FX_Trigger_Continue
}rs485_fx_trigger_enum;

/*-Effect CCT-*/
typedef struct{
    uint16_t    intensity;      /*亮度：<0-1000>*/
    uint16_t    cct;            /*色温：<2000-10000>K*/
    uint8_t     gm;             /*偏色：<0-20>-><-1.0 - 1.0>，精度0.1*/
}rs485_fx_mode_cct_arg_t;       /*size: 5*/

/*-Effect HSI-*/
typedef struct{
    uint16_t    intensity;      /*亮度：<0-1000>*/
    uint16_t    hue;            /*色调：<0-360>，0-360,360-0*/
    uint8_t     sat;            /*饱和度<0-100>*/
}rs485_fx_mode_hsi_arg_t;       /*size: 5*/

/*-Effect GEL-*/
typedef struct{
    uint16_t    intensity;      /*亮度：<0-1000>*/
    uint16_t    cct;            /*色温：<2000-10000>K*/
    uint8_t     origin;         /*色纸品牌：0x00->LEE; 0x01->Resco*/
    uint8_t     series;         /*色纸系列：<0-255>*/
    uint16_t    color;          /*色纸颜色：<0-65535>*/
}rs485_fx_mode_gel_arg_t;       /*size: 8*/

/*Effect XY_Coordinate*/
typedef struct{
    uint16_t    intensity;      /*亮度：<0-1000>*/
    uint16_t    x;              /*X色坐标:<0-10000>，除以10000表示实际色坐标*/
    uint16_t    y;              /*Y色坐标:<0-10000>，除以10000表示实际色坐标*/
}rs485_fx_mode_xy_arg_t;        /*size: 6*/

/*-Effect Light Source-*/
typedef struct{
    uint16_t    intensity;      /*亮度：<0-1000>*/
    uint8_t     type;           /*光源类型：<0-255>*/
}rs485_fx_mode_source_arg_t;    /*size: 3*/

/*-Effect Light Source-*/
typedef struct{
	uint16_t 	intensity;/*亮度：<0-1000>*/
    uint16_t    red_ratio;     
    uint16_t    green_ratio;      
	uint16_t	blue_ratio;
}rs485_fx_mode_rgb_arg_t;    /*size: 3*/


/*Mode_Arg定义*/
typedef union{
    rs485_fx_mode_cct_arg_t     cct;
    rs485_fx_mode_hsi_arg_t     hsi;
    rs485_fx_mode_gel_arg_t     gel;
    rs485_fx_mode_xy_arg_t      xy;
    rs485_fx_mode_source_arg_t  source;
	rs485_fx_mode_rgb_arg_t     rgb;
}rs485_fx_mode_arg_t;

/*0x00：Club_Lights*/
typedef struct{
    uint16_t    intensity;      /*亮度：<0-1000>*/
    uint8_t     frq;            /*频率：<1-11>,11为Random*/
    uint8_t     color;          /*颜色数量：<0-7>,0:3; 1:6; 2:9; 3:12 4:15; 5:18; 6:24; 7:36*/
}rs485_fx_club_lights_arg_t;    /*size: 4*/

/*0x01：Paparazzi*/
typedef struct{
    uint16_t    intensity;      /*亮度：<0-1000>*/
    uint8_t     frq;            /*频率：<1-11>,11为Random*/
    uint16_t    cct;            /*色温：<2000-10000>K*/
    uint8_t     gm;             /*偏色：<0-20>-><-1.0 - 1.0>，精度0.1*/
}rs485_fx_paparazzi_arg_t;      /*size: 6*/

/*0x02：Lightning*/
typedef struct{
    uint16_t                intensity;  /*亮度：<0-1000>*/
    uint8_t                 frq;        /*频率：<1-11>,11为Random*/
    uint16_t                cct;        /*色温：<2000-10000>K*/
    uint8_t                 gm;         /*偏色：<0-20>-><-1.0 - 1.0>，精度0.1*/
    rs485_fx_trigger_enum   trigger;    /*<0-2>,0-无触发/1-单次触发/2-多次触发*/
    uint8_t                 spd;        /*速度：<1-11>,11为Random*/
}rs485_fx_lightning_arg_t;              /*size: 6*/

/*0x03：TV*/
typedef struct{
    uint16_t    intensity;      /*亮度：<0-1000>*/
    uint8_t     frq;            /*频率：<1-11>,11为Random*/
    uint16_t    cct;            /*色温：<2000-10000>K*/
}rs485_fx_tv_arg_t;             /*size: 5*/

/*0x04：Candle*/
typedef struct{
    uint16_t    intensity;      /*亮度：<0-1000>*/
    uint8_t     frq;            /*频率：<1-11>,11为Random*/
    uint16_t    cct;            /*色温：<2000-10000>K*/
}rs485_fx_candle_arg_t;            /*size: 5*/

/*0x05：Fire*/
typedef struct{
    uint16_t    intensity;      /*亮度：<0-1000>*/
    uint8_t     frq;            /*频率：<1-11>,11为Random*/
    uint16_t    cct;            /*色温：<2000-10000>K*/
}rs485_fx_fire_arg_t;           /*size: 5*/

/*0x06：Strobe*/
typedef struct{
    uint8_t                 frq;            /*频率：<1-11>,11为Random*/
    rs485_fx_mode_enum      mode;           /*0x00:CCT；0x01:HSI；0x02:GEL；0x03:XY；0x04:Source*/
    rs485_fx_mode_arg_t     mode_arg;       /*Mode_Arg，具体模式参数*/
}rs485_fx_strobe_arg_t;                     /*size: 10*/

/*0x07：Explosion*/
typedef struct{
    uint8_t                 frq;            /*频率：<1-11>,11为Random*/
    rs485_fx_trigger_enum   trigger;        /*触发类型：<0-2>,0-无触发/1-单次触发/2-多次触发*/
    rs485_fx_mode_enum      mode;           /*0x00:CCT；0x01:HSI；0x02:GEL；0x03:XY；0x04:Source*/
    rs485_fx_mode_arg_t     mode_arg;       /*Mode_Arg，具体模式参数*/
}rs485_fx_explosion_arg_t;                     /*size: 11*/

/*0x08：Fault_Bulb*/
typedef struct{
    uint8_t                 frq;            /*频率：<1-11>,11为Random*/
    uint8_t                 spd;            /*速度：<1-11>,11为Random*/
    rs485_fx_mode_enum      mode;           /*0x00:CCT；0x01:HSI；0x02:GEL；0x03:XY；0x04:Source*/
    rs485_fx_mode_arg_t     mode_arg;       /*Mode_Arg，具体模式参数*/
}rs485_fx_fault_bulb_arg_t;                /*size: 11*/

/*0x09：Pulsing*/
typedef struct{
    uint8_t                 frq;            /*频率：<1-11>,11为Random*/
    uint8_t                 spd;            /*速度：<1-11>,11为Random*/
    rs485_fx_mode_enum      mode;           /*0x00:CCT；0x01:HSI；0x02:GEL；0x03:XY；0x04:Source*/
    rs485_fx_mode_arg_t     mode_arg;       /*Mode_Arg，具体模式参数*/
}rs485_fx_pulsing_arg_t;                    /*size: 11*/

/*0x0A：Welding*/
typedef struct{
    uint8_t                 frq;            /*频率：<1-11>,11为Random*/
    uint16_t                min_int;        /*最低亮度：<0-1000>*/
    rs485_fx_mode_enum      mode;           /*0x00:CCT；0x01:HSI；0x02:GEL；0x03:XY；0x04:Source*/
    rs485_fx_mode_arg_t     mode_arg;       /*Mode_Arg，具体模式参数*/
}rs485_fx_welding_arg_t;                    /*size: 12*/

/*0x0B：Cop_Car*/
typedef struct{
    uint16_t    intensity;      /*亮度：<0-1000>*/
    uint8_t     frq;            /*频率：<1-11>,11为Random*/
    uint8_t     color;          /*颜色选择：0x00：R；0x01：B；0x02：R+B；0x03：B+W*/
}rs485_fx_cop_car_arg_t;        /*size: 4*/

/*0x0C：Color_Chase*/
typedef struct{
    uint16_t    intensity;      /*亮度：<0-1000>*/
    uint8_t     frq;            /*频率：<1-11>,11为Random*/
    uint8_t     sat;            /*饱和度：<0-100>*/
}rs485_fx_color_chase_arg_t;    /*size: 4*/

/*0x0D：Party_Lights*/
typedef struct{
    uint16_t    intensity;      /*亮度：<0-1000>*/
    uint8_t     frq;            /*频率：<1-11>,11为Random*/
    uint8_t     sat;            /*饱和度：<0-100>*/
}rs485_fx_party_lights_arg_t;   /*size: 4*/

/*0x0E：Fireworks*/
typedef struct{
    uint16_t    intensity;      /*亮度：<0-1000>*/
    uint8_t     frq;            /*频率：<1-11>,11为Random*/
    uint8_t     type;           /*光类型：0->CCT，1->HUE，2->CCT+HUE*/
}rs485_fx_fireworks_arg_t;

/*0x10：I Am Here*/
typedef struct{
    uint8_t     state;          /*定位光效状态：0x00->运行,0x01->停止并退出*/
}rs485_fx_i_am_here_arg_t;

/*Effect_Arg定义*/
typedef union{
    rs485_fx_club_lights_arg_t     club_lights;
    rs485_fx_paparazzi_arg_t       paparazzi;
    rs485_fx_lightning_arg_t       lightning;
    rs485_fx_tv_arg_t              tv;
    rs485_fx_candle_arg_t          candle;
    rs485_fx_fire_arg_t            fire;
    rs485_fx_strobe_arg_t          strobe;
    rs485_fx_explosion_arg_t       explosion;
    rs485_fx_fault_bulb_arg_t      fault_bulb;
    rs485_fx_pulsing_arg_t         pulsing;
    rs485_fx_welding_arg_t         welding;
    rs485_fx_cop_car_arg_t         cop_car;
    rs485_fx_color_chase_arg_t     color_chase;
    rs485_fx_party_lights_arg_t    party_lights;
    rs485_fx_fireworks_arg_t       fireworks;
    rs485_fx_i_am_here_arg_t       i_am_here;
}rs485_fx_arg_t;

typedef struct{
    rs485_fx_type_enum  type;               /*光效类型*/
    rs485_fx_arg_t      arg;                /*光效参数*/
}rs485_sys_fx_t;                            /*size: */

/*------------------------------------*/
/*------Dimming_Curve：0x08------*/
/*------------------------------------*/
typedef struct{
    uint8_t     curve;          /*变化曲线 <0-3>：0x00:线性,  0x01:指数,  0x02:对数,  0x03:S线*/
}rs485_dimming_curve_t;    /*size: 1*/

/*------------------------------------*/
/*--------Fan：0x09-------*/
/*------------------------------------*/
typedef struct{
    uint8_t     mode;           /*0x00: Fixture; 0x01: Auto; 0x02: High Temp; 0x03: MAX; 0x04: OFF*/
    uint16_t    speed;          /*自定义风扇转速，<0-4096>*/
    uint8_t     fan_index;      /*0/255->所有风扇，1->风扇1*/
    uint8_t     err_msg;        /*风扇故障，读指令时有效*/
}rs485_fan_t;              /*size: 5*/

/*------------------------------------*/
/*--------Power_Suppy：0x0A-------*/
/*------------------------------------*/
typedef struct{
    uint8_t     power_type;     /*供电类型 0->适配器，1->DC直流，2->电池*/
    uint32_t    input_voltage;  /*输入电压，单位为mv*/
    uint32_t    input_current;  /*输入电流，单位为ma*/
}rs485_power_t;      /*size: 9*/

/*------------------------------------*/
/*------Battery_msg：0x0B-------*/
/*------------------------------------*/
typedef struct{
    uint8_t     bat_index;          /*当前电池序号 0/255->所有电池，1->电池1*/
    uint16_t    bat_voltage;        /*电池电压 单位MV*/
    uint16_t    bat_current;        /*电池电流 单位MA*/
    uint8_t     bat_type;           /*电池类型，根据电压区分 0->6-8.4V,1->12-16.8V,2->24-33.6V*/
    uint16_t    bat_time_remain;    /*电池剩余时间 单位分钟*/
    uint8_t     bat_power_remain;   /*电池剩余电量百分比 %*/
}rs485_battery_t;            /*size: 9*/

/*------------------------------------*/
/*--------Switch：0x0C-------*/
/*------------------------------------*/
typedef struct{
    uint8_t     state;          /*<0-1>, 0x00: 关机;   0x01: 开机 */
}rs485_switch_t;       /*size: 1*/

/*------------------------------------*/
/*----------FileTransfer：0x0D---------*/
/*------------------------------------*/
typedef enum{
    RS485_File_TX_Step_None = 0,
    RS485_File_TX_Step_Start,
    RS485_File_TX_Step_Size,
    RS485_File_TX_Step_Data,
    RS485_File_TX_Step_CRC,
}rs485_file_tx_step_enum;

typedef enum{
    RS485_File_TX_CFX = 0,
    RS485_File_TX_Firmware,
    RS485_File_TX_LED_Data,
    RS485_File_TX_Null,
}rs485_file_tx_file_type_enum;

typedef struct{
    uint8_t     effect_type;
    uint8_t     bank;
}rs485_file_tx_cfx_start_arg_t;

typedef struct{
    uint8_t     type;
}rs485_file_tx_firmware_start_arg_t;

typedef union{
    rs485_file_tx_cfx_start_arg_t       cfx_start;
    rs485_file_tx_firmware_start_arg_t  firmware_start;
}rs485_file_tx_step_start_arg_t;

typedef struct{
    uint32_t file_size;
}rs485_file_tx_step_size_arg_t;

typedef struct{
    uint16_t    block_size;         /*数据块实际大小，从1开始*/   
    uint32_t    block_cnt;          /*数据块计数，从1开始*/   
    uint8_t     block_buf[128];     /*数据块Buffer*/
}rs485_file_tx_step_data_arg_t;

typedef struct{
    uint32_t crc32;
}rs485_file_tx_step_crc_arg_t;

typedef union{
    rs485_file_tx_step_start_arg_t  start_arg;
    rs485_file_tx_step_size_arg_t   size_arg;
    rs485_file_tx_step_data_arg_t   data_arg;
    rs485_file_tx_step_crc_arg_t    crc_arg;
}rs485_file_tx_step_body_t;

typedef struct{
    rs485_file_tx_file_type_enum    file_type;
    rs485_file_tx_step_enum         step;
    rs485_file_tx_step_body_t       step_body;
}rs485_file_transfer_t;

/*------------------------------------*/
/*----------Temperature_Msg：0x0E-------*/
/*------------------------------------*/
typedef struct{
    uint8_t     type;                   /*温度类型 0/255->所有温度，1->COB温度，2->环境温度，3->PCB温度*，4->运行时间*/
    int16_t    value;                  /*温度值 or 运行时间*/
    uint8_t     err_state;              /*温度错误信息 ->温度正常，1->高温降功率，2->高温关机，3->低温*/
}rs485_temperature_msg_t;          /*size: 4*/


/*------------------------------------*/
/*--------Fan：0x09-------*/
/*------------------------------------*/
typedef struct{
    uint8_t     mode;           /*0x00: Fixture; 0x01: Auto; 0x02: High Temp; 0x03: MAX; 0x04: OFF*/
    uint16_t    speed;          /*自定义风扇转速，<0-4096>*/
    uint8_t     pump_index;      /*0/255->所有风扇，1->风扇1*/
    uint8_t     err_msg;        /*风扇故障，读指令时有效*/
}rs485_pump_t;  


typedef struct{
    uint8_t    led_color_type;          
    uint8_t    adjust_dir;        
    uint16_t   adjust_val;      
    uint8_t    save_ctr;        
}rs485_self_adjust_t;  




/*------------------------------------*/
/*---------Light_Effect_II：0x0F---------*/
/*------------------------------------*/

/*---二代光效类型---*/
typedef enum{
    RS485_FX_II_Paparazzi_2 = 0,
    RS485_FX_II_Lightning_2,
    RS485_FX_II_TV_2,
    RS485_FX_II_Candle_2,
    RS485_FX_II_Fire_2,
    RS485_FX_II_Strobe_2,
    RS485_FX_II_Explosion_2,
    RS485_FX_II_Fault_Bulb_2,
    RS485_FX_II_Pulsing_2,
    RS485_FX_II_Welding_2,
    RS485_FX_II_Cop_Car_2,
    RS485_FX_II_Party_Lights_2,
    RS485_FX_II_Fireworks_2,
    RS485_FX_II_Lightning_3,
    RS485_FX_II_TV_3,
    RS485_FX_II_Fire_3,
    RS485_FX_II_Faulty_Bulb_3,
    RS485_FX_II_Pulsing_3,
    RS485_FX_II_Cop_Car_3,
    RS485_FX_II_Null,
}rs485_fx_2_type_enum;

/*---二代光效里的子类型---*/
typedef enum{
    RS485_FX_II_Mode_CCT = 0,
    RS485_FX_II_Mode_HSI,
    RS485_FX_II_Mode_GEL,
    RS485_FX_II_Mode_XY,
    RS485_FX_II_Mode_SOUYRCE,
    RS485_FX_II_Mode_RGB,
    RS485_FX_II_Mode_CCT_Range,
    RS485_FX_II_Mode_HSI_Range,
    RS485_FX_II_Mode_Null,
}rs485_fx_2_mode_enum;

/*---二代光效触发类型---*/
typedef enum{
    RS485_FX_II_Trigger_None = 0,
    RS485_FX_II_Trigger_Once,
    RS485_FX_II_Trigger_Continue
}rs485_fx_2_trigger_enum;

/*-Effect II CCT-*/
typedef struct{
    uint16_t    cct;            /*CCT，单位k*/
    float       duv;            /*浮点数 <-0.04~0.04>或<-0.02~0.02>,产品不同，范围有所不同，默认0.00*/
}rs485_fx_2_mode_cct_t;         /*size: 6*/

/*-Effect II HSI-*/
typedef struct{
    uint16_t    hue;            /*色调：<0-360>，0-360,360-0*/
    uint8_t     sat;            /*饱和度：<0-100>*/
    uint16_t    cct;            /*中心白点色温：<0-65535>，单位K*/
}rs485_fx_2_mode_hsi_t;         /*size: 5*/

/*-Effect II GEL-*/
typedef struct{
    uint16_t    cct;            /*色温：<0-65535>，单位K*/
    uint8_t     origin;         /*色纸品牌，0x00：LEE; 0x01：Resco*/
    uint8_t     series;         /*色纸类型：<0-255>*/
    uint16_t    color;          /*色纸颜色：<0-65535>*/
}rs485_fx_2_mode_gel_t;         /*size: 6*/

/*Effect II XY_Coordinate*/
typedef struct{
    float    x;       /*x色坐标<0-10000>，除以10000表示实际色坐标*/
    float    y;       /*y色坐标<0-10000>，除以10000表示实际色坐标*/
}rs485_fx_2_mode_xy_t;   /*size: 4*/

/*-Effect II Light Source-*/
typedef struct{
    uint8_t     type;           /*光源类型：<0-255>*/
    float    x_coord;        /*x色坐标<0-10000>，除以10000表示实际色坐标*/
    float    y_coord;        /*y色坐标<0-10000>，除以10000表示实际色坐标*/
}rs485_fx_2_mode_source_t;      /*size: 5*/

/*-Effect II LightRGB-*/
typedef struct{
    float    red_ratio;      /*红光比例：<0-1000>*/
    float    green_ratio;    /*绿光比例：<0-1000>*/
    float    blue_ratio;     /*蓝光比例：<0-1000>*/
}rs485_fx_2_mode_rgb_t;         /*size: 6*/

/*-Effect II CCT_Range*/
typedef struct{
    uint16_t    cct_upper;      /*色温上限：<0-65535>，单位k*/
    uint16_t    cct_lower;      /*色温下限：<0-65535>，单位k*/
    float       duv;            /*偏色： <-0.04~0.04>或<-0.02~0.02>,产品不同，范围有所不同，默认0.00*/
}rs485_fx_2_mode_cct_range_arg_t;        /*size: 8*/

/*-Effect II HSI_Range*/
typedef struct{
    uint16_t    hue_upper;      /*色调上限：<0-360>，0-360,360-0*/
    uint16_t    hue_lower;      /*色调下限：<0-360>，0-360,360-0*/
    uint8_t     sat;            /*饱和度：<0-100>*/
    uint16_t    cct;            /*中心白点色温：<0-65535>，单位K*/
}rs485_fx_2_mode_hsi_range_arg_t;        /*size: 7*/

/*Mode_Arg定义*/
typedef union{
    rs485_fx_2_mode_cct_t           cct;
    rs485_fx_2_mode_hsi_t           hsi;
    rs485_fx_2_mode_gel_t           gel;
    rs485_fx_2_mode_xy_t            xy;
    rs485_fx_2_mode_source_t        source;
    rs485_fx_2_mode_rgb_t           rgb;
    rs485_fx_2_mode_cct_range_arg_t cct_range;
    rs485_fx_2_mode_hsi_range_arg_t hsi_range;
}rs485_fx_2_mode_arg_t;

/*0x00：Paparazzi_II*/
typedef struct{
    rs485_fx_2_trigger_enum     state;      /*0-停止 ,2-循环*/
    uint16_t                    intensity;  /*亮度<0-1000>*/
    uint16_t                    gap_time;   /*<1-300>,<0.1-30秒>精度0.1秒*/
    uint16_t                    min_gap_time;/*<1-300>,<0.1-30秒>精度0.1秒*/
    rs485_fx_2_mode_enum        mode;       /*0x00:CCT;0x01:HSI;0x02:GEL;0x03:XY;0x04:Source;0X05:RGB*/
    rs485_fx_2_mode_arg_t       mode_arg;   /*Mode_Arg，具体模式参数*/
}rs485_fx_2_paparazzi_2_arg_t;              /*size: 12*/

/*0x01：Lightning_II*/
typedef struct{
    rs485_fx_2_trigger_enum     state;      /*<0-2>,0-停止 1-单次 2-循环*/
    uint16_t                    intensity;  /*亮度<0-1000>*/
    uint8_t                     frq;        /*<1-11>11为Random*/
    uint8_t                     spd;        /*<1-11>11为Random*/
    rs485_fx_2_mode_enum        mode;       /*0x00:CCT;0x01:HSI*/
    rs485_fx_2_mode_arg_t       mode_arg;   /*Mode_Arg，具体模式参数*/
}rs485_fx_2_lightning_2_arg_t;              /*size: 12*/

/*0x02：TV_II*/
typedef struct{
    uint16_t                    intensity;  /*亮度<0-1000>*/
    uint8_t                     spd;        /*<1-11>11为Random*/
    rs485_fx_2_trigger_enum     state;      /*0-停止,2-循环*/
    rs485_fx_2_mode_enum        mode;       /*0x06:CCT_Range;0x07:HSI_Range*/
    rs485_fx_2_mode_arg_t       mode_arg;  /*Mode_Arg，具体模式参数*/
}rs485_fx_2_tv_2_arg_t;                     /*size: 13*/

/*0x03：Candle_II*/
typedef struct{
    rs485_fx_2_trigger_enum     state;      /*0-停止,2-循环*/
    uint16_t                    intensity;  /*亮度<0-1000>*/
    uint8_t                     cct_range;  /*0->Warmer 1400K-1700K,1->Neutral 1700k-2000k,2->Cooler 2000K-2300K*/
    uint8_t                     spd;        /*<1-10>*/
}rs485_fx_2_candle_2_arg_t;                 /*size: 7*/

/*0x04：Fire_II*/
typedef struct{
    rs485_fx_2_trigger_enum     state;      /*0-停止,2-循环*/
    uint16_t                    intensity;  /*亮度<0-1000>*/
    uint8_t                     spd;        /*<1-11>11为Random*/
    rs485_fx_2_mode_enum        mode;       /*0x06:CCT_Range;0x07:HSI_Range*/
    rs485_fx_2_mode_arg_t       mode_arg;  /*Mode_Arg，具体模式参数*/
}rs485_fx_2_fire_2_arg_t;                   /*size: 15*/

/*0x05：Strobe_II*/
typedef struct{
    rs485_fx_2_trigger_enum     state;      /*0-停止,2-循环*/
    uint16_t                    intensity;  /*亮度<0-1000>*/
    uint8_t                     spd;        /*<1-11>11为Random*/
    rs485_fx_2_mode_enum        mode;       /*0x00:CCT;0x01:HSI;0x02:GEL;0x03:XY;0x04:Source;0X05:RGB*/
    rs485_fx_2_mode_arg_t       mode_arg;   /*Mode_Arg，具体模式参数*/
}rs485_fx_2_strobe_2_arg_t;                 /*size: 11*/

/*0x06：Explosion_II*/
typedef struct{
    rs485_fx_2_trigger_enum     state;      /*0-停止 1-单次*/
    uint16_t                    intensity;  /*亮度<0-1000>*/
    uint8_t                     decay;      /*<1-10>*/
    rs485_fx_2_mode_enum        mode;       /*0x00:CCT;0x01:HSI;0x02:GEL;0x03:XY;0x04:Source;0X05:RGB*/
    rs485_fx_2_mode_arg_t       mode_arg;   /*Mode_Arg，具体模式参数*/
}rs485_fx_2_explosion_2_arg_t;              /*size: 11*/

/*0x07：Fault_Bulb_II*/
typedef struct{
    rs485_fx_2_trigger_enum     state;      /*0-停止,2-循环*/
    uint16_t                    intensity;  /*亮度<0-1000>*/
    uint8_t                     spd;        /*<1-11>11为Random*/
    uint8_t                     frq;        /*<1-11>11为Random*/
    rs485_fx_2_mode_enum        mode;       /*0x00:CCT;0x01:HSI;0x02:GEL;0x03:XY;0x04:Source;0X05:RGB*/
    rs485_fx_2_mode_arg_t       mode_arg;   /*Mode_Arg，具体模式参数*/
}rs485_fx_2_fault_bulb_2_arg_t;             /*size: 12*/

/*0x08：Pulsing_II*/
typedef struct{
    rs485_fx_2_trigger_enum     state;      /*0-停止,2-循环*/
    uint16_t                    intensity;  /*亮度<0-1000>*/
    uint8_t                     spd;        /*<1-10>*/
    uint8_t                     frq;        /*<1-10>*/
    rs485_fx_2_mode_enum        mode;       /*0x00:CCT;0x01:HSI;0x02:GEL;0x03:XY;0x04:Source;0X05:RGB*/
    rs485_fx_2_mode_arg_t       mode_arg;   /*Mode_Arg，具体模式参数*/
}rs485_fx_2_pulsing_2_arg_t;                /*size: 12*/

/*0x09：Welding_II*/
typedef struct{
    rs485_fx_2_trigger_enum     state;      /*0-停止,2-循环*/
    uint16_t                    intensity;  /*亮度<0-1000>*/
    uint16_t                    min_intensity;    /*亮度<0-750>*/
    uint8_t                     frq;        /*<1-10>*/
    rs485_fx_2_mode_enum        mode;       /*0x00:CCT;0x01:HSI;0x02:GEL;0x03:XY;0x04:Source;0X05:RGB*/
    rs485_fx_2_mode_arg_t       mode_arg;   /*Mode_Arg，具体模式参数*/
}rs485_fx_2_welding_2_arg_t;                /*size: 13*/

/*0x0A：Cop_Car_II*/
typedef struct{
    rs485_fx_2_trigger_enum     state;          /*0-停止,2-循环*/
    uint16_t                    intensity;      /*亮度<0-1000>*/
    uint8_t                     frq;            /*<1-10>*/
    uint8_t                     color;          /*0->R,1->Bk,2->R+B,3->B+W,4->R+B+W*/
}rs485_fx_2_cop_car_2_arg_t;                    /*size: 5*/

/*0x0B：Party_Lights_II*/
typedef struct{
    rs485_fx_2_trigger_enum     state;      /*0-停止,1-单次,2-循环*/
    uint16_t                    intensity;  /*亮度<0-1000>*/
    uint16_t                    sat;        /*<0 - 1000> -> <0% - 100%>,调节精度0.1%*/
    uint16_t                    spd;        /*<1-60>-><1-60秒>精度1秒*/
}rs485_fx_2_party_lights_2_arg_t;           /*size: 12*/

/*0x0C：Fireworks_II*/
typedef struct{
    rs485_fx_2_trigger_enum     state;      /*0-停止,2-循环*/
    uint16_t                    intensity;  /*亮度<0-1000>*/
    uint16_t                    gap_time;   /*<1-300> -> <0.1 - 30秒>，精度0.1秒*/
    uint16_t                    min_gap_time;/*<1-300> -> <0.1 - 30秒>，精度0.1秒*/
    uint8_t                     mode;       /*0x00:CCT;0x01:HUE;0x02:CCT+HUE*/
}rs485_fx_2_fireworks_2_arg_t;              /*size: 11*/

/*0x0D：Lightning_III*/
typedef struct{
    rs485_fx_2_trigger_enum     state;      /*0-停止,1-单次,2-循环*/
    uint16_t                    intensity;  /*亮度<0-1000>*/
    uint16_t                    gap_time;   /*<1-300>-><0.1-30秒>精度0.1秒*/
    uint16_t                    min_gap_time;   /*<1-300>-><0.1-30秒>精度0.1秒*/
    rs485_fx_2_mode_enum        mode;       /*0x00:CCT;0x01:HSI;0x02:GEL;0x03:XY;0x04:Source;0X05:RGB*/
    rs485_fx_2_mode_arg_t       mode_arg;   /*Mode_Arg，具体模式参数*/
}rs485_fx_2_lightning_3_arg_t;              /*size: 12*/

/*0x0E：TV_III*/
typedef struct{
    rs485_fx_2_trigger_enum     state;      /*0-停止,1-单次,2-循环*/
    uint16_t                    intensity;  /*亮度<0-1000>*/
    uint16_t                    gap_time;   /*<1-300>-><0.1-30秒>精度0.1秒*/
    uint16_t                    min_gap_time;   /*<1-300>-><0.1-30秒>精度0.1秒*/
    rs485_fx_2_mode_enum        mode;       /*0x06:CCT_Range;0x07:HSI_Range*/
    rs485_fx_2_mode_arg_t       mode_arg;   /*Mode_Arg，具体模式参数*/
}rs485_fx_2_tv_3_arg_t;                     /*size: 14*/

/*0x0F：Fire_III*/
typedef struct{
    rs485_fx_2_trigger_enum     state;      /*0-停止,1-单次,2-循环*/
    uint16_t                    intensity;  /*亮度<0-1000>*/
    uint8_t                     frq;        /*<20-200>*/
    rs485_fx_2_mode_enum        mode;       /*0x06:CCT_Range;0x07:HSI_Range*/
    rs485_fx_2_mode_arg_t       mode_arg;   /*Mode_Arg，具体模式参数*/
}rs485_fx_2_fire_3_arg_t;                   /*size: 13*/

/*0x10：Faulty_Bulb_III*/
typedef struct{
    rs485_fx_2_trigger_enum     state;      /*0-停止,1-单次,2-循环*/
    uint16_t                    intensity;  /*亮度<0-750>*/
    uint16_t                    gap_time;   /*<1-300>-><0.1-30秒>精度0.1秒*/
    uint16_t                    min_gap_time;   /*<1-300>-><0.1-30秒>精度0.1秒*/
    rs485_fx_2_mode_enum        mode;       /*0x00:CCT;0x01:HSI;0x02:GEL;0x03:XY;0x04:Source;0X05:RGB*/
    rs485_fx_2_mode_arg_t       mode_arg;   /*Mode_Arg，具体模式参数*/
}rs485_fx_2_faulty_bulb_3_arg_t;            /*size: 13*/

/*0x11：Pulsing_III*/
typedef struct{
    rs485_fx_2_trigger_enum     state;      /*0-停止,1-单次,2-循环*/
    uint16_t                    intensity;  /*亮度<0-1000>*/
    uint8_t                     frq;        /*<20-200>*/
    rs485_fx_2_mode_enum        mode;       /*0x00:CCT;0x01:HSI;0x02:GEL;0x03:XY;0x04:Source;0X05:RGB*/
    rs485_fx_2_mode_arg_t       mode_arg;   /*Mode_Arg，具体模式参数*/
}rs485_fx_2_pulsing_3_arg_t;                /*size: 13*/

/*0x12：cop_car_3*/
typedef struct{
    rs485_fx_2_trigger_enum     state;      /*0-停止,1-单次,2-循环*/
    uint16_t                    intensity;  /*亮度<0-750>*/
    uint8_t                     frq;        /*1->闪一次，5->闪5次，6->循环闪*/
    uint8_t                     mode;       /*0->R,1->B,2->R+B,3->B+W,4->R+B+W*/
}rs485_fx_2_fireworks_3_arg_t;              /*size: 12*/

/*EffectII_Arg定义*/
typedef union{
    rs485_fx_2_paparazzi_2_arg_t    paparazzi_2;
    rs485_fx_2_lightning_2_arg_t    lightning_2;
    rs485_fx_2_tv_2_arg_t           tv_2;
    rs485_fx_2_candle_2_arg_t       candle_2;
    rs485_fx_2_fire_2_arg_t         fire_2;
    rs485_fx_2_strobe_2_arg_t       strobe_2;
    rs485_fx_2_explosion_2_arg_t    explosion_2;
    rs485_fx_2_fault_bulb_2_arg_t   fault_bulb_2;
    rs485_fx_2_pulsing_2_arg_t      pulsing_2;
    rs485_fx_2_welding_2_arg_t      welding_2;
    rs485_fx_2_cop_car_2_arg_t      cop_car_2;
    rs485_fx_2_party_lights_2_arg_t party_lights_2;
    rs485_fx_2_fireworks_2_arg_t    fireworks_2;
    rs485_fx_2_lightning_3_arg_t    lightning_3;
    rs485_fx_2_tv_3_arg_t           tv_3;
    rs485_fx_2_fire_3_arg_t         fire_3;
    rs485_fx_2_faulty_bulb_3_arg_t  faulty_bulb_3;
    rs485_fx_2_pulsing_3_arg_t      pulsing_3;
    rs485_fx_2_fireworks_3_arg_t    fireworks_3;
}rs485_fx_2_arg_t;

typedef struct{
    rs485_fx_2_type_enum    type;    /*光效类型*/
    rs485_fx_2_arg_t        arg;     /*光效参数*/
}rs485_sys_fx_2_t;                 

/*------------------------------------*/
/*----------Self_Adaption：0x10-------*/
/*------------------------------------*/
//typedef struct{
//    uint8_t     led_color_type; /*灯颜色类型 0->所有灯，1->红灯，2->绿灯，3->蓝灯，4->暖白灯，5->冷白灯*/
//    uint8_t     adjust_upper;   /*调节上限百分比%*/
//    uint8_t     adjust_lower;   /*调节下限百分比%*/
//    uint8_t     adjust_dir;     /*调节方向 0->不调节，1->正向，2->反向*/
//    uint16_t    adjust_val;     /*调节值 0-65535*/
//}rs485_self_adaption_t;    /*size: 6*/

typedef struct{
    uint8_t     led_color_type; /*灯颜色类型 0->所有灯，1->红灯，2->绿灯，3->蓝灯，4->暖白灯，5->冷白灯*/
    uint16_t    led_voltage;     /*恒流芯片电压值 0-65535*/
	uint16_t    led_pwm;    /*灯体PWM值 0-65535*/
}rs485_self_adaption_t;    /*size: 6*/
/*------------------------------------*/
/*----------Factory_PWM：0x11-------*/
/*------------------------------------*/
typedef struct{
    uint16_t    red;            /*红灯pwm<0-65535>*/
    uint16_t    green;          /*绿灯pwm<0-65535>*/
    uint16_t    blue;           /*蓝灯pwm<0-65535>*/
    uint16_t    ww;             /*暖白pwm<0-65535>*/
    uint16_t    cw;             /*冷白pwm<0-65535>*/
    uint16_t    ww2;             /*冷白pwm<0-65535>*/
    uint16_t    cw2;             /*冷白pwm<0-65535>*/
    
}rs485_factory_rgbww_t;    /*size: 10*/

/*------------------------------------*/
/*----------Source：0x12-------*/
/*------------------------------------*/
typedef struct{
    float       intensity;      /*亮度：<0-1.0>，1.0代表100%*/
    uint8_t     type;           /*光源类型：<0-255>*/
    float    x_coord;        /*x色坐标<0-10000>，除以10000表示实际色坐标*/
    float    y_coord;        /*y色坐标<0-10000>，除以10000表示实际色坐标*/
    uint32_t    fade;           /*Fade，缓变时间，单位ms*/
    uint16_t    pixel_x;        /*点控通道横坐标<0-65535>*/
    uint16_t    pixel_y;        /*点控通道纵坐标<0-65535>*/
}rs485_source_t;           /*size: 15*/

/*------------------------------------*/
/*----------Illumination_Mode：0x13-------*/
/*------------------------------------*/
typedef struct{
    uint8_t     mode;               /*照度模式,0x00：最大功率输出 0x01：恒照度输出*/
    uint16_t    ratio;              /*<0-100>%，功率比例，功率比例，默认10000，10000代表100%*/
    uint8_t     save_ctr;           /*<0-1>%，功率核准，是否保存功率，1保存到灯体，0不保存*/
    uint16_t    power_calib_ratio;  /*<0-10000>,灯体功率校准比例*/
}rs485_illumination_mode_t;     /*size: 6*/

/*------------------------------------*/
/*----------DMX Color_Mixing：0x14-------*/
/*------------------------------------*/
/*---ColorMixing里的子类型---*/
typedef enum{
    RS485_ColorMixingMode_CCT = 0,
    RS485_ColorMixingMode_HSI,
    RS485_ColorMixingMode_GEL,
    RS485_ColorMixingMode_XY,
    RS485_ColorMixingMode_SOUYRCE,
    RS485_ColorMixingMode_RGB,
    RS485_ColorMixingMode_Null,
}rs485_color_mixing_mode_enum;

/*-ColorMixing CCT-*/
typedef struct{
    float       intensity;      /*亮度：<0-1.0>，1.0代表100%*/
    uint16_t    cct;            /*CCT，<0-65535>，单位k*/
    float       duv;            /*浮点数 <-0.04~0.04>或<-0.02~0.02>,产品不同，范围有所不同，默认0.00*/
}rs485_color_mixing_cct_t;      /*size: 10*/

/*-ColorMixing HSI-*/
typedef struct{
    float       intensity;      /*亮度：<0-1.0>，1.0代表100%*/
    uint16_t    hue;            /*hue<0-36000>，0-36000,36000-0*/
    uint16_t    sat;            /*饱和度：<0-10000>，10000代表100%*/
    uint16_t    cct;            /*中心白点色温：<0-65535>K*/
}rs485_color_mixing_hsi_t;      /*size: 8*/

/*-ColorMixing GEL-*/
typedef struct{
    float       intensity;      /*亮度：<0-1.0>，1.0代表100%*/
    uint16_t    cct;            /*CCT，<0-65535>，单位k*/
    uint8_t     origin;         /*色纸品牌：0->LEE 1->Resco*/
    uint8_t     series;         /*色纸系列：<0-255>*/
    uint16_t    color;          /*色纸颜色：<0-65535>*/
}rs485_color_mixing_gel_t;      /*size: 10*/

/*ColorMixing XY_Coordinate*/
typedef struct{
    float       intensity;      /*亮度：<0-1.0>，1.0代表100%*/
    float    x;              /*x色坐标<0-10000>，除以10000表示实际色坐标*/
    float    y;              /*y色坐标<0-10000>，除以10000表示实际色坐标*/
}rs485_color_mixing_xy_t;       /*size: 8*/

/*-ColorMixing Light Source-*/
typedef struct{
    float       intensity;          /*亮度：<0-1.0>，1.0代表100%*/
    uint8_t     type;               /*Type*/
    float       x_coord;            /*x色坐标<0-10000>，除以10000表示实际色坐标*/
    float       y_coord;            /*y色坐标<0-10000>，除以10000表示实际色坐标*/
}rs485_color_mixing_source_t;       /*size: 9*/

/*-ColorMixing RGB-*/
typedef struct{
    float    intensity;          /*亮度：<0-1.0>，1.0代表100%*/
    float    red_ratio;          /*红灯亮度<0-1000>*/
    float    green_ratio;        /*绿灯亮度<0-1000>*/
    float    blue_ratio;         /*蓝灯亮度<0-1000>*/
}rs485_color_mixing_rgb_t;          /*size: 10*/

typedef struct{
    rs485_color_mixing_mode_enum mode;           /*光模式类型  0->CCT,1->HSI,2->GEL,3->XY,4->Source,5->RGB*/
    union{
        rs485_color_mixing_cct_t        cct;
        rs485_color_mixing_hsi_t        hsi;
        rs485_color_mixing_gel_t        gel;
        rs485_color_mixing_xy_t         xy;
        rs485_color_mixing_source_t     source;
        rs485_color_mixing_rgb_t        rgb;
    }mode_arg;
}rs485_color_mixing_color_arg_t;                /*size: 10*/   

typedef struct{
    rs485_color_mixing_color_arg_t  color_1_arg;
    rs485_color_mixing_color_arg_t  color_2_arg;
    float                        color_1_ratio;  /*颜色参与比例<0-10000>， ratio/10000表示Color1参与比例，Color2参与比例为(10000-ratio)/10000%*/
    uint32_t                        fade;           /*Fade，缓变时间，单位ms*/
    uint16_t                        pixel_x;        /*点控通道横坐标<0-65535>*/
    uint16_t                        pixel_y;        /*点控通道纵坐标<0-65535>*/
}rs485_color_mixing_t;             /*size: 32*/

/*------------------------------------*/
/*------Err_Msg：0x15-------*/
/*------------------------------------*/
typedef struct{
    uint32_t     err_state1;     /*错误信息
                                     Bit0:state
                                     Bit1:state
                                     …….
                                   State：0：None error
                                   State：1：Error
                                */
}rs485_err_msg_t;          /*size:1*/

/*------------------------------------*/
/*------PFX_Ctrl：0x16-------*/
/*------------------------------------*/
typedef struct{
    uint16_t    intensity;      /*<2000-10000>K*/ 
    uint16_t    cct;            /*5600对应5600K*/
    uint32_t    gm;             /*<0-20>: -1.0->0,-0.9->1,-0.8->2,   +1.0->20每隔0.1递增1，以此类推*/  /*max: 31*/
    uint8_t     frq;            /*0.1-10Hz 0=0.1Hz …… 9=1Hz …… 19=10Hz*/
    uint16_t    times;          /*1-1000:1-1000次     大于1000=always*/
}rs485_pfx_arg_flash_base_cct_t;       /*size:11*/

typedef struct{
    uint16_t    intensity;      /*0-1000*/ 
    uint16_t    hue;            /*0-360*/
    uint8_t     sat;            /*0-100*/
    uint8_t     frq;            /*0.1-10Hz 0=0.1Hz …… 9=1Hz …… 19=10Hz*/
    uint16_t    times;          /*1-1000:1-1000次     大于1000=always*/
}rs485_pfx_arg_flash_base_hsi_t;       /*size:8*/

typedef struct{
    uint8_t     base;           /*0:CCT 1:HSI*/ 
    union{
        rs485_pfx_arg_flash_base_cct_t cct;
        rs485_pfx_arg_flash_base_hsi_t hsi;
    }base_arg;
}rs485_pfx_arg_flash_t;            /*size:12*/

typedef struct{
    uint16_t    min_intensity;  /*0-1000*/
    uint16_t    max_intensity;  /*0-1000*/
    uint16_t    min_cct;        /*5600对应5600K*/
    uint16_t    max_cct;        /*5600对应5600K*/
    uint32_t    min_gm;         /*<0-20>: -1.0->0,-0.9->1,-0.8->2,   +1.0->20每隔0.1递增1，以此类推*/
    uint32_t    max_gm;         /*<0-20>: -1.0->0,-0.9->1,-0.8->2,   +1.0->20每隔0.1递增1，以此类推*/
    uint32_t    times;          /*<0-100000>  单位ms,最大100S,大于100S=Always*/
    uint8_t     cct_seq;        /*0:正向    1:反向    2:往复    3:随机*/
    uint8_t     int_seq;        /*0:正向    1:反向    2:往复    3:随机*/
    uint8_t     gm_seq;         /*0:正向    1:反向    2:往复    3:随机*/
    uint8_t     loop;           /*0:一次   1:循环*/
}rs485_pfx_arg_chase_base_cct_t;       /*size:24*/

typedef struct{
    uint16_t    min_intensity;  /*0-1000*/
    uint16_t    max_intensity;  /*0-1000*/
    uint16_t    min_hue;        /*0-360*/
    uint16_t    max_hue;        /*0-360*/
    uint8_t     min_sat;        /*0-100*/
    uint8_t     max_sat;        /*0-100*/
    uint32_t    times;          /*<0-100000>  单位ms,最大100S,大于100S=Always*/
    uint8_t     hue_seq;        /*0:正向    1:反向    2:往复    3:随机*/
    uint8_t     int_seq;        /*0:正向    1:反向    2:往复    3:随机*/
    uint8_t     sat_Seq;        /*0:正向    1:反向    2:往复    3:随机*/
    uint8_t     loop;           /*0:一次   1:循环*/
}rs485_pfx_arg_chase_base_hsi_t;       /*size:18*/

typedef struct{
    uint8_t     base;           /*0x00:CCT   0x01:HSI*/
    union{
        rs485_pfx_arg_chase_base_cct_t cct;
        rs485_pfx_arg_chase_base_hsi_t hsi;
    }base_arg;
}rs485_pfx_arg_chase_t;            /*size:25*/

typedef struct{
    uint16_t    intensity;      /*0-1000*/
    uint16_t    cct;            /*5600对应5600K*/
    uint32_t    gm;             /*<0-20>: -1.0->0,-0.9->1,-0.8->2,   +1.0->20每隔0.1递增1，以此类推*/
    uint32_t    fade_in_time;   /*0-20000  单位ms,最大20S*/
    uint8_t     fade_in_curve;  /*0:线性    1:指数    2:对数    3:s型*/
    uint32_t    continue_time;  /*<0-100000>  单位ms,最大100S,大于100S=Always*/
    uint32_t    fade_out_time;  /*0-20000  单位ms,最大20S*/
    uint8_t     fade_out_curve; /*0:线性    1:指数    2:对数    3:s型*/
}rs485_pfx_arg_continues_base_cct_t;    /*size:22*/    

typedef struct{
    uint16_t    intensity;      /*0-1000*/
    uint16_t    hue;            /*0-360*/
    uint8_t     sat;            /*0-100*/
    uint32_t    fade_in_time;   /*0-20000  单位ms,最大20S*/
    uint8_t     fade_in_curve;  /*0:线性    1:指数    2:对数    3:s型*/
    uint32_t    continue_time;  /*<0-100000>  单位ms,最大100S,大于100S=Always*/
    uint32_t    fade_out_time;  /*0-20000  单位ms,最大20S*/
    uint8_t     fade_out_curve; /*0:线性    1:指数    2:对数    3:s型*/
}rs485_pfx_arg_continues_base_hsi_t;    /*size:19*/ 

typedef struct{
    uint8_t base;               /*0x00:CCT   0x01:HSI*/
    union{
        rs485_pfx_arg_continues_base_cct_t cct;
        rs485_pfx_arg_continues_base_hsi_t hsi;
    }base_arg;
}rs485_pfx_arg_continues_t;         /*size:23*/ 

typedef struct{
    uint8_t             pfx_type;   /*光效类型, 0x00:PFX_Flash  0x01:PFX_ColorChase  0x02:PFX_Continue*/
    uint8_t             light_type; /*灯具类型, 0x00:normal  0x01:tube  0x01:pixel mapping*/
    union{
        rs485_pfx_arg_flash_t       flash;
        rs485_pfx_arg_chase_t       chase;
        rs485_pfx_arg_continues_t   continues;
    }pfx_arg;        /*光效参数*/
}rs485_pfx_ctrl_t;         /*size:25*/

/*------------------------------------*/
/*----------CFX_Bank_RW：0x17-----------*/
/*------------------------------------*/
typedef struct{
    uint16_t    picker;         /*bit0-9: Bank1-Bank10*/
    uint16_t    touch_bar;      /*bit0-9: Bank1-Bank10*/
    uint16_t    music;          /*bit0-9: Bank1-Bank10*/
}rs485_cfx_bank_rw_t;      /*size:6*/

/*------------------------------------*/
/*----------CFX_Ctrl：0x18-----------*/
/*------------------------------------*/
typedef struct{
    uint8_t     effect_type;    /*自定义光效类型*/
    uint8_t     bank;           /*<0-9>,Bank1-Bank10*/
    uint16_t    intensity;      /*0-1000*/
    uint8_t     spd;            /*0=0.5X, 1=1X 2=2X, 3=4X 4=8X, 5=16X*/
    uint8_t     chaos;          /*0%-100%*/
    uint8_t     sequence;       /*0:正序 1:反序 2:往复*/
    uint8_t     loop;           /*0:一次 1:循环*/
    uint8_t     state;          /*0:stop  1:init  2:running  3:Pause*/
}rs485_cfx_ctrl_t;         /*size:9*/

/*------------------------------------*/
/*----------CFX_Preview：0x19-----------*/
/*------------------------------------*/
typedef struct{
    uint16_t    intensity;      /*0-1000*/
    uint16_t    cct;            /*5600对应5600K*/
    uint32_t    gm;             /*<0-20>: -1.0->0,-0.9->1,-0.8->2,   +1.0->20每隔0.1递增1，以此类推*/
}rs485_cfx_preview_frame_base_cct_t;         /*size:8*/

typedef struct{
    uint16_t    intensity;      /*0-1000*/
    uint16_t    hue;            /*0-360*/
    uint8_t     sat;            /*0-100*/
}rs485_cfx_preview_frame_base_hsi_t;         /*size:5*/

typedef struct{
    uint8_t base;               /*0x00:CCT   0x01:HSI*/
    union{
        rs485_cfx_preview_frame_base_cct_t cct;
        rs485_cfx_preview_frame_base_hsi_t hsi;
    }base_arg;
}rs485_cfx_preview_frame_t;        /*size:9*/

typedef struct{
    rs485_cfx_preview_frame_t frame_1;
    rs485_cfx_preview_frame_t frame_2;
}rs485_cfx_preview_t;      /*size:18*/

/*------------------------------------*/
/*----------MFX_Ctrl：0x1A-----------*/
/*------------------------------------*/
typedef struct{
    uint16_t    min_intensity;  /*0-1000*/
    uint16_t    max_intensity;  /*0-1000*/
    uint8_t     int_seq;        /*0:正向    1:反向    2:往复    3:随机*/
    uint16_t    min_cct;        /*<2000-10000>*/
    uint16_t    max_cct;        /*<2000-10000>*/
    uint8_t     cct_seq;        /*0:正向    1:反向    2:往复    3:随机*/
    uint32_t    min_gm;         /*<0-20>: 0->-1.0,1->-0.9,2->-0.8,   20->+1.0每隔0.1递增1，以此类推*/
    uint32_t    max_gm;         /*<0-20>: 0->-1.0,1->-0.9,2->-0.8,   20->+1.0每隔0.1递增1，以此类推*/
    uint8_t     gm_seq;         /*0:正向    1:反向    2:往复    3:随机*/
}rs485_mfx_base_cct_t;           /*size:19*/

typedef struct{
    uint16_t    min_intensity;  /*0-1000*/
    uint16_t    max_intensity;  /*0-1000*/
    uint8_t     int_seq;        /*0:正向    1:反向    2:往复    3:随机*/
    uint16_t    min_hue;        /*色调上限：<0-360>*/
    uint16_t    max_hue;        /*色调下限：<0-360>*/
    uint8_t     hue_seq;        /*0:正向    1:反向    2:往复    3:随机*/
    uint8_t     min_sat;        /*<0-20>: -1.0->0,-0.9->1,-0.8->2,   +1.0->20每隔0.1递增1，以此类推*/
    uint8_t     max_sat;        /*<0-20>: -1.0->0,-0.9->1,-0.8->2,   +1.0->20每隔0.1递增1，以此类推*/
    uint8_t     sat_seq;        /*0:正向    1:反向    2:往复    3:随机*/
}rs485_mfx_base_hsi_t;           /*size:13*/

typedef struct{
    uint8_t base;          /*0:Base CCT    1:Base HSI*/
    union {
        rs485_mfx_base_cct_t    cct;
        rs485_mfx_base_hsi_t    hsi;
    }base_arg;
}rs485_mfx_base_arg_t;              /*size:14*/

typedef struct{
    uint8_t     loop_mode;      /*0:一次    1:循环   2:随机*/
    uint8_t     loop_times;     /*1-100*/
    uint32_t    min_cycle_time; /*<0-100000>  单位ms*/
    uint32_t    max_cycle_time; /*<0-100000>  单位ms*/
    uint8_t     cycle_time_seq; /*0:正向    1:反向    2:往复    3:随机*/
    uint32_t    min_free_time;  /*<0-100000>  单位ms*/
    uint32_t    max_free_time;  /*<0-100000>  单位ms*/
    uint8_t     free_time_seq;  /*0:正向    1:反向    2:往复    3:随机*/
    uint32_t    min_unit_uime;  /*<0-100000>  单位ms*/
    uint32_t    max_unit_uime;  /*<0-100000>  单位ms*/
    uint8_t     unit_time_seq;  /*0:正向    1:反向    2:往复    3:随机*/
    uint8_t     min_frq;        /*1-129  <0.1-0.9Hz:1-9> <1-120Hz:10-129>*/
    uint8_t     max_frq;        /*1-129  <0.1-0.9Hz:1-9> <1-120Hz:10-129>*/
    uint8_t     frq_Seq;        /*1-129     <1-9:0.1-0.9Hz> <10-129:1-120Hz>*/
}rs485_mfx_mode_flash_t;        /*size:32*/

typedef struct{
    uint8_t     loop_mode;          /*0:一次    1:循环   2:随机*/
    uint8_t     loop_times;         /*1-100*/
    uint32_t    min_cycle_time;     /*<0-100000>  单位ms*/
    uint32_t    max_cycle_time;     /*<0-100000>  单位ms*/
    uint8_t     cycle_time_seq;     /*0:正向    1:反向    2:往复    3:随机*/
    uint32_t    min_fade_in_time;   /*<0-100000>  单位ms*/
    uint32_t    max_fade_in_time;   /*<0-100000>  单位ms*/
    uint8_t     fade_in_time_seq;   /*0:正向    1:反向    2:往复    3:随机*/
    uint8_t     fade_in_curve;      /*0:线性    1:指数    2:对数    3:S型*/
    uint32_t    min_fade_out_time;  /*<0-100000>  单位ms*/
    uint32_t    max_fade_out_time;  /*<0-100000>  单位ms*/
    uint8_t     fade_out_time_seq;  /*0:正向    1:反向    2:往复    3:随机*/
    uint8_t     fade_out_curve;     /*0:线性    1:指数    2:对数    3:S型*/
    uint8_t     flash_frq;          /*1-129     <1-9:0.1-0.9Hz> <10-129:1-120Hz>*/
}rs485_mfx_mode_continues_t;         /*size:32*/

typedef struct{
    uint8_t     loop_mode;          /*0:一次    1:循环   2:随机*/
    uint8_t     loop_times;         /*1-100*/
    uint32_t    min_cycle_time;     /*<0-100000>  单位ms*/
    uint32_t    max_cycle_time;     /*<0-100000>  单位ms*/
    uint8_t     cycle_time_seq;     /*0:正向    1:反向    2:往复    3:随机*/
    uint8_t     min_olr;            /*<0-100> */
    uint8_t     max_olr;            /*<0-100> */
    uint8_t     olr_seq;            /*0:正向    1:反向    2:往复    3:随机*/
    uint8_t     over_lap_seq;       /*0:正向    1:反向    2:往复    3:随机*/
    uint8_t     free_time_seq;      /*0:正向    1:反向    2:往复    3:随机*/
    uint8_t     unit_time_seq;      /*0:正向    1:反向    2:往复    3:随机*/
    uint32_t    min_free_time;      /*<0-100000>  单位ms*/
    uint32_t    max_free_time;      /*<0-100000>  单位ms*/
    uint32_t    min_unit_uime;      /*<0-100000>  单位ms*/
    uint32_t    max_unit_uime;      /*<0-100000>  单位ms*/
    uint8_t     min_olp;            /*0-100*/
    uint8_t     max_olp;            /*0-100*/
    uint8_t     olp_seq;            /*0:正向    1:反向    2:往复    3:随机*/
    uint32_t    min_fade_in_time;   /*<0-100000>  单位ms*/
    uint32_t    max_fade_in_time;   /*<0-100000>  单位ms*/
    uint8_t     fade_in_time_seq;   /*0:正向    1:反向    2:往复    3:随机*/
    uint8_t     fade_in_curve;      /*0:线性    1:指数    2:对数    3:S型*/
    uint32_t    min_fade_out_time;  /*<0-100000>  单位ms*/
    uint32_t    max_fade_out_time;  /*<0-100000>  单位ms*/
    uint8_t     fade_out_time_seq;  /*0:正向    1:反向    2:往复    3:随机*/
    uint8_t     fade_out_curve;     /*0:线性    1:指数    2:对数    3:S型*/
    uint8_t     flash_frq;          /*1-129     <1-9:0.1-0.9Hz> <10-129:1-120Hz>*/
}rs485_mfx_mode_paragraph_t;        /*size:57*/


typedef struct{
    uint8_t effect_mode;            /*0:Flash  1:Continue  2:Paragraph*/
    union {
        rs485_mfx_mode_flash_t      flash;
        rs485_mfx_mode_continues_t  continues;
        rs485_mfx_mode_paragraph_t  paragraph;
    }mode;
}rs485_mfx_mode_t;                  /*size:58*/       

typedef struct {
    uint8_t state;               /*0:Stop  1:Restart  2:Pause  3:Continue*/
    rs485_mfx_base_arg_t    base_arg;
    rs485_mfx_mode_t        mode_arg;
}rs485_mfx_ctrl_t;             /*size:73*/

/*------------------------------------*/
/*----------RGBWW：0x1B---------*/
/*------------------------------------*/
typedef struct{
    float       intensity;      /*浮点数<0-1.0>,1.0代表100%*/
    float    	red_ratio;      /*红灯PWM<0-65535>*/
    float    	green_ratio;    /*绿灯PWM<0-65535>*/
    float    	blue_ratio;     /*蓝灯PWM<0-65535>*/
    float    	ww_ratio;       /*暖白PWM<0-65535>*/
    float    	cw_ratio;       /*冷白PWM<0-65535>*/
    uint32_t    fade;           /*Fade，缓变时间，单位ms*/
    uint16_t    pixel_x;        /*点控通道横坐标*/
    uint16_t    pixel_y;        /*点控通道纵坐标*/
}rs485_rgbww_t;                 /*size:22*/  

/*------------------------------------*/
/*----------CFX_Name：0x1C---------*/
/*------------------------------------*/
typedef struct{
    uint8_t     effect_type;    /*光效类型  0-Picker  1-Touchbar  2-Music*/
    uint8_t     bank;           /*光效Bank  0-9:Bank1-10*/
    uint16_t    code_type;      /*编码方式 0：Ascii码*/
    char        name[10];       /*文件名，Ascii码*/
}rs485_cfx_name_t;         /*size:14*/ 

/*------------------------------------*/
/*----------Curr_Light_Mode：0x1D---------*/
/*------------------------------------*/
typedef struct{
    uint8_t     light_mode;         /*光模式类型  0->CCT,1->HSI,2->GEL,3->XY,4->Source,5->RGB,6->第一代光效,7->第二代光效,8->CCT&HSI,9->CCT&RGB*/
    uint8_t     fx_mode;            /*第一代或第二代光效类型*/
    uint8_t     belt_fx_mode;       /*点控光效的灯带类型*/
}rs485_curr_light_mode_t;      /*size:3*/  

/*------------------------------------*/
/*---------pixel_Effect：0x1E---------*/
/*------------------------------------*/

/*---像素光效类型---*/
typedef enum{
    RS485_PixelFX_Color_Cut = 0,
    RS485_PixelFX_Color_Rotate,
    RS485_PixelFX_One_Color_Move,
    RS485_PixelFX_Two_Color_Move,
    RS485_PixelFX_Three_Color_Move,
    RS485_PixelFX_Pixel_Fire,
    RS485_PixelFX_Many_Color_Move,
    RS485_PixelFX_Rainbow,
    RS485_PixelFX_On_Off_Effect,
    RS485_PixelFX_Belt_Effect,
    RS485_PixelFX_Music,
    RS485_PixelFX_Null,
}rs485_pixel_fx_type_enum;

/*---像素光效里的子类型---*/
typedef enum{
    RS485_PixelFX_Mode_CCT = 0,
    RS485_PixelFX_Mode_HSI,
    RS485_PixelFX_Mode_Black,
    RS485_PixelFX_Mode_Null,
}rs485_pixel_fx_mode_enum;

/*---像素光效触发类型---*/
typedef enum{
    RS485_PixelFX_State_None = 0,
    RS485_PixelFX_State_Once,
    RS485_PixelFX_State_Continue,
}rs485_pixel_fx_state_enum;

/*-Effect Pixel CCT-*/
typedef struct{
    uint16_t    cct;            /*CCT，单位k*/
    float       duv;            /*浮点数 <-0.04~0.04>或<-0.02~0.02>,产品不同，范围有所不同，默认0.00*/
}rs485_pixel_fx_mode_cct_arg_t;      /*size: 6*/

/*-Effect Pixel HSI-*/
typedef struct{
    uint16_t    hue;            /*hue<0-360>，0-360,360-0*/
    uint8_t     sat;            /*饱和度<0-100>*/
    uint16_t    cct;            /*<2000-10000>，中心白点色温，单位K*/
}rs485_pixel_fx_mode_hsi_arg_t;      /*size: 5*/

/*Mode_Arg_Pixel定义*/
typedef union{
    rs485_pixel_fx_mode_cct_arg_t   cct;
    rs485_pixel_fx_mode_hsi_arg_t   hsi;
}rs485_pixel_fx_mode_arg_t;        /*size: 6*/

/*0x00：Color_Cut*/
typedef struct{
    rs485_pixel_fx_state_enum   state;              /*0-停止 2-循环*/
    uint8_t                     pixel_num;          /*此光效的像素个数：2-10个*/
    uint8_t                     dir;                /*方向：0-左，1-右*/
    uint16_t                    spd;                /*速度：<1-640>-><1-640cm/s>精度1*/
    uint8_t                     pixel_index;        /*像素序号：<0-9> 0->光柱颜色1, ..., 9->光柱颜色10*/
    uint16_t                    intensity;          /*亮度<0-1000>*/
    rs485_pixel_fx_mode_enum    mode;               /*0x00:CCT；0x01:HSI；0x02:Black*/
    rs485_pixel_fx_mode_arg_t   mode_arg;           /*Mode_Arg，具体模式参数*/
}rs485_pixel_fx_color_cut_arg_t;                    /*size: 16*/

/*0x01：Color_rotate*/
typedef struct{
    rs485_pixel_fx_state_enum   state;              /*0-停止 2-循环*/
    uint8_t                     pixel_num;          /*此光效的像素个数：2-10个*/
    uint8_t                     dir;                /*方向：0-左，1-右*/
    uint16_t                    spd;                /*速度：<1-100>-><0.1-10s>精度0.1s*/
    uint8_t                     rotate_way;         /*切换方式：0-瞬变，1-渐变*/
    uint8_t                     pixel_index;        /*像素序号：<0-9> 0->光柱颜色1, ..., 9->光柱颜色10*/
    uint16_t                    intensity;          /*亮度<0-1000>*/
    rs485_pixel_fx_mode_enum    mode;               /*0x00:CCT；0x01:HSI；0x02:Black*/
    rs485_pixel_fx_mode_arg_t   mode_arg;           /*Mode_Arg，具体模式参数*/
}rs485_pixel_fx_color_rotate_arg_t;                   /*size: 13*/               /*size: 14*/

/*0x02：One_Color_Move*/
typedef struct{
    rs485_pixel_fx_state_enum   state;              /*0-停止 2-循环*/
    uint8_t                     dir_group;          /*方向组：0->单组，1->双组*/
    uint8_t                     dir;                /*方向：方向组为单组时（0->左，1->右，2->来回）；为双组时（0->交错，1->反弹）*/
    uint8_t                     pixel_length;       /*像素长度：0->S（短）,1->M（中）,2->L（长）*/
    uint16_t                    spd;                /*速度：<1-640>-><1-640cm/s>精度1*/
    uint8_t                     pixel_index;        /*像素序号：<0-2> 0->底色, ..., 2->光柱颜色2*/
    uint16_t                    intensity;          /*亮度<0-1000>*/
    rs485_pixel_fx_mode_enum    mode;               /*0x00:CCT；0x01:HSI；0x02:Black*/
    rs485_pixel_fx_mode_arg_t   mode_arg;           /*Mode_Arg，具体模式参数*/
}rs485_pixel_fx_one_color_move_arg_t;               /*size: 17*/

/*0x03：Two_Color_Move*/
typedef struct{
    rs485_pixel_fx_state_enum   state;              /*0-停止 2-循环*/
    uint8_t                     dir_group;          /*方向组：0->单组，1->双组*/
    uint8_t                     dir;                /*方向：方向组为单组时（0->左，1->右，2->来回）；为双组时（0->交错，1->反弹）*/
    uint8_t                     pixel_length;       /*像素长度：0->S（短）,1->M（中）,2->L（长）*/
    uint16_t                    spd;                /*速度：<1-640>-><1-640cm/s>精度1*/
    uint8_t                     pixel_index;        /*像素序号：<0-4> 0->底色, ..., 4->光柱颜色4*/
    uint16_t                    intensity;          /*亮度<0-1000>*/
    rs485_pixel_fx_mode_enum    mode;               /*0x00:CCT；0x01:HSI；0x02:Black*/
    rs485_pixel_fx_mode_arg_t   mode_arg;           /*Mode_Arg，具体模式参数*/
}rs485_pixel_fx_two_color_move_arg_t;               /*size: 17*/

/*0x04：Three_Color_Move*/
typedef struct{
    rs485_pixel_fx_state_enum   state;              /*0-停止 2-循环*/
    uint8_t                     dir_group;          /*方向组：0->单组，1->双组*/
    uint8_t                     dir;                /*方向：方向组为单组时（0->左，1->右，2->来回）；为双组时（0->交错，1->反弹）*/
    uint8_t                     pixel_length;       /*像素长度：0->S（短）,1->M（中）,2->L（长）*/
    uint16_t                    spd;                /*速度：<1-640>-><1-640cm/s>精度1*/
    uint8_t                     pixel_index;        /*像素序号：<0-6> 0->底色, ..., 6->光柱颜色6*/
    uint16_t                    intensity;          /*亮度<0-1000>*/
    rs485_pixel_fx_mode_enum    mode;               /*0x00:CCT；0x01:HSI；0x02:Black*/
    rs485_pixel_fx_mode_arg_t   mode_arg;           /*Mode_Arg，具体模式参数*/
}rs485_pixel_fx_three_color_move_arg_t;             /*size: 17*/

/*0x05：Pixel_Fire*/
typedef struct{
    rs485_pixel_fx_state_enum   state;      /*0-停止 2-循环*/
    uint8_t                     frq;        /*频率：<1-10>-><1-10hz>精度1*/
    uint8_t                     dir;        /*方向：0-横向，1 - 竖向*/
    uint16_t                    intensity;  /*亮度<0-1000>*/
    uint16_t                    min_intensity;/*亮度<0-1000>*/
    uint8_t                     pixel_index;/*像素序号：<0-1> 0->底色, ..., 1->火焰颜色*/
    rs485_pixel_fx_mode_enum    mode;       /*0x00:CCT；0x01:HSI；0x02:Black*/
    rs485_pixel_fx_mode_arg_t   mode_arg;   /*Mode_Arg，具体模式参数*/
}rs485_pixel_fx_pixel_fire_arg_t;           /*size: 16*/

/*0x06：Many_Color_Move*/
typedef struct{
    rs485_pixel_fx_state_enum   state;              /*0-停止 2-循环*/
    uint8_t                     dir_group;          /*方向组：0->单组，1->双组*/
    uint8_t                     dir;                /*方向：方向组为单组时（0->左，1->右，2->来回）；为双组时（0->交错，1->反弹）*/
    uint8_t                     pixel_length;       /*像素长度：0->S（短）,1->M（中）,2->L（长）*/
    uint8_t                     pixel_group;        /*像素组数量：<1-6> -> <1-6组>*/
    uint16_t                    spd;                /*速度：<1-640>-><1-640cm/s>精度1*/
    uint8_t                     pixel_index;        /*像素序号：<0-6> 0->底色, ..., 6->光柱颜色6*/
    uint16_t                    intensity;          /*亮度<0-1000>*/
    rs485_pixel_fx_mode_enum    mode;               /*0x00:CCT；0x01:HSI；0x02:Black*/
    rs485_pixel_fx_mode_arg_t   mode_arg;           /*Mode_Arg，具体模式参数*/
}rs485_pixel_fx_many_color_move_arg_t;             /*size: 17*/

/*0x07：rainbow*/
typedef struct{
    rs485_pixel_fx_state_enum   state;      /*0-停止 1-启动*/
    uint16_t                    intensity;  /*亮度<0-1000>*/
    uint8_t                     dir;        /*0->头到尾,1->尾到头,2->头尾到中间,3->中间到两头,4->随机*/
    uint8_t                     spd;        /*速度：<1-640>-><1-640cm/s>精度1*/
}rs485_pixel_fx_rainbow_arg_t;                       /*size: 5*/

/*0x08：On_Off_Effect*/
typedef struct{
    rs485_pixel_fx_state_enum   state;      /*0-停止 2-循环*/
    uint8_t                     dir;        /*0:开机光效，从头到尾,1:关机光效，从尾到头*/
    uint8_t                     color;      /*0*/
}rs485_pixel_fx_on_off_arg_t;               /*size: 3*/

/*0x09：Belt_Effect*/
/*---像素灯带光效类型---*/
typedef enum{
    RS485_BeltFX_Fade = 0,
    RS485_BeltFX_Holoday,
    RS485_BeltFX_One_Color_Marquee,
    RS485_BeltFX_Full_Color_Marquee,
    RS485_BeltFX_Starry_Sky,
    RS485_BeltFX_Aurora,
    RS485_BeltFX_Racing,
    RS485_BeltFX_Train,
    RS485_BeltFX_Snake,
    RS485_BeltFX_Forest,
    RS485_BeltFX_Bonfire,
    RS485_BeltFX_Brook,
    RS485_BeltFX_Game,
    RS485_BeltFX_Timewait,
    RS485_BeltFX_Party,
    RS485_BeltFX_Belt_Fireworks,
    RS485_BeltFX_Marbles,
    RS485_BeltFX_Pendulum,
    RS485_BeltFX_Flash_Point,
    RS485_BeltFX_Null,
}rs485_belt_fx_type_enum;

/*0x00：Fade*/
typedef struct{
    rs485_pixel_fx_state_enum   state;              /*0-停止 2-循环*/
    uint16_t                    intensity;          /*亮度<0-1000>*/
    uint8_t                     spd;                /*<0-9> -> <0.1-1秒>*/
}rs485_belt_fx_fade_arg_t;                          /*size: 5*/

/*0x01：Holiday*/
typedef struct{
    rs485_pixel_fx_state_enum   state;              /*0-停止 2-循环*/
    uint16_t                    intensity;          /*亮度<0-1000>*/
    uint8_t                     sat;                /*<0 - 100>*/
    uint8_t                     dir;                /*0->头到尾,1->尾到头,2->头尾到中间,3->中间到两头,4->随机*/
    uint8_t                     frq;                /*<0-10> -> <1-11>,11是随机*/
    uint16_t                    pixel_color;        /*光柱颜色：<0 - 359> -> <1°-360°>*/
    uint16_t                    base_color;         /*基础颜色：<0 - 359> -> <1°-360°> 511->无底色*/
}rs485_belt_fx_holiday_arg_t;                       /*size: 10*/

/*0x02：One_Color_Marquee*/
typedef struct{
    rs485_pixel_fx_state_enum   state;              /*0-停止 2-循环*/
    uint16_t                    intensity;          /*亮度<0-1000>*/
    uint8_t                     sat;                /*<0 - 100>*/
    uint8_t                     dir;                /*0->头到尾,1->尾到头,2->头尾到中间,3->中间到两头,4->随机*/
    uint8_t                     spd;                /*<0-9> -> <0.1-1秒>*/
    uint8_t                     pixel_length;       /*光柱长度：<0-9> -> <1-10>*/
    uint16_t                    pixel_color;        /*光柱颜色：<0 - 359> -> <1°-360°>*/
    uint16_t                    base_color;         /*基础颜色：<0 - 359> -> <1°-360°> 511->无底色*/
}rs485_belt_fx_one_color_marquee_arg_t;             /*size: 11*/

/*0x03：Full_Color_Marquee*/
typedef struct{
    rs485_pixel_fx_state_enum   state;              /*0-停止 2-循环*/
    uint16_t                    intensity;          /*亮度<0-1000>*/
    uint8_t                     dir;                /*0->头到尾,1->尾到头,2->头尾到中间,3->中间到两头,4->随机*/
    uint8_t                     spd;                /*<0-9> -> <0.1-1秒>*/
    uint8_t                     pixel_length;       /*光柱长度：<0-9> -> <1-10>*/
    uint16_t                    base_color;         /*基础颜色：<0 - 359> -> <1°-360°> 511->无底色*/
}rs485_belt_fx_full_color_marquee_arg_t;            /*size: 8*/

/*0x04：Starry_Sky*/
typedef struct{
    rs485_pixel_fx_state_enum   state;              /*0-停止 2-循环*/
    uint16_t                    intensity;          /*亮度<0-1000>*/
    uint8_t                     sat;                /*<0 - 100>*/
    uint8_t                     frq;                /*<0-10> -> <1-11>,11是随机*/
    uint16_t                    pixel_color;        /*光柱颜色：<0 - 359> -> <1°-360°>*/
}rs485_belt_fx_starry_sky_arg_t;                    /*size: 7*/

/*0x05：Aurora*/
typedef struct{
    rs485_pixel_fx_state_enum   state;              /*0-停止 2-循环*/
    uint16_t                    intensity;          /*亮度<0-1000>*/
    uint8_t                     frq;                /*<0-10> -> <1-11>,11是随机*/
    uint16_t                    base_color;         /*基础颜色：<0 - 359> -> <1°-360°> 511->无底色*/
}rs485_belt_fx_aurora_arg_t;                /*size: 6*/

/*0x06：Racing*/
typedef struct{
    rs485_pixel_fx_state_enum   state;              /*0-停止 2-循环*/
    uint16_t                    intensity;          /*亮度<0-1000>*/
    uint8_t                     spd;                /*<0-9> -> <0.1-1秒>*/
    uint8_t                     pixel_length;       /*光柱长度：<0-9> -> <1-10>*/
    uint16_t                    base_color;         /*基础颜色：<0 - 359> -> <1°-360°> 511->无底色*/
}rs485_belt_fx_racing_arg_t;                        /*size: 7*/

/*0x07：Train*/
typedef struct{
    rs485_pixel_fx_state_enum   state;              /*0-停止 2-循环*/
    uint16_t                    intensity;          /*亮度<0-1000>*/
    uint8_t                     sat;                /*<0 - 100>*/
    uint8_t                     dir;                /*0->头到尾,1->尾到头,2->头尾到中间,3->中间到两头,4->随机*/
    uint8_t                     spd;                /*<0-9> -> <0.1-1秒>*/
    uint8_t                     pixel_length;       /*光柱长度：<0-9> -> <1-10>*/
    uint16_t                    pixel_color;        /*光柱颜色：<0 - 359> -> <1°-360°>*/
    uint16_t                    base_color;         /*基础颜色：<0 - 359> -> <1°-360°> 511->无底色*/
}rs485_belt_fx_train_arg_t;                         /*size: 11*/

/*0x08：Snake*/
typedef struct{
    rs485_pixel_fx_state_enum   state;              /*0-停止 2-循环*/
    uint16_t                    intensity;          /*亮度<0-1000>*/
    uint8_t                     spd;                /*<0-9> -> <0.1-1秒>*/
    uint8_t                     pixel_length;       /*光柱长度：<0-9> -> <1-10>*/
    uint16_t                    base_color;         /*基础颜色：<0 - 359> -> <1°-360°> 511->无底色*/
}rs485_belt_fx_snake_arg_t;                         /*size: 7*/

/*0x09：Forest*/
typedef struct{
    rs485_pixel_fx_state_enum   state;              /*0-停止 2-循环*/
    uint16_t                    intensity;          /*亮度<0-1000>*/
    uint8_t                     dir;        /*0->头到尾,1->尾到头,2->头尾到中间,3->中间到两头,4->随机*/
    uint8_t                     spd;        /*<0-9> -> <0.1-1秒>*/
}rs485_belt_fx_forest_arg_t;                /*size: 5*/

/*0x0A：Bonfire*/
typedef struct{
    rs485_pixel_fx_state_enum   state;              /*0-停止 2-循环*/
    uint16_t                    intensity;          /*亮度<0-1000>*/
    uint8_t                     dir;        /*0->头到尾,1->尾到头,2->头尾到中间,3->中间到两头,4->随机*/
    uint8_t                     spd;        /*<0-9> -> <0.1-1秒>*/
}rs485_belt_fx_bonfire_arg_t;               /*size: 5*/

/*0x0B：Brook*/
typedef struct{
    rs485_pixel_fx_state_enum   state;              /*0-停止 2-循环*/
    uint16_t                    intensity;          /*亮度<0-1000>*/
    uint8_t                     dir;        /*0->头到尾,1->尾到头,2->头尾到中间,3->中间到两头,4->随机*/
    uint8_t                     spd;        /*<0-9> -> <0.1-1秒>*/
}rs485_belt_fx_brook_arg_t;                 /*size: 5*/

/*0x0C：Game*/
typedef struct{
    rs485_pixel_fx_state_enum   state;              /*0-停止 2-循环*/
    uint16_t                    intensity;          /*亮度<0-1000>*/
    uint8_t                     dir;        /*0->头到尾,1->尾到头,2->头尾到中间,3->中间到两头,4->随机*/
    uint8_t                     spd;        /*<0-9> -> <0.1-1秒>*/
    uint8_t                     pixel_length;       /*光柱长度：<0-9> -> <1-10>*/
}rs485_belt_fx_game_arg_t;                          /*size: 6*/

/*0x0D：Timewait*/
typedef struct{
    rs485_pixel_fx_state_enum   state;              /*0-停止 2-循环*/
    uint16_t                    intensity;          /*亮度<0-1000>*/
    uint8_t                     dir;        /*0->头到尾,1->尾到头,2->头尾到中间,3->中间到两头,4->随机*/
    uint8_t                     spd;        /*<0-9> -> <0.1-1秒>*/
    uint8_t                     pixel_length;       /*光柱长度：<0-9> -> <1-10>*/
    uint16_t                    base_color;         /*基础颜色：<0 - 359> -> <1°-360°> 511->无底色*/
}rs485_belt_fx_timewait_arg_t;                      /*size: 8*/

/*0x0E：Party*/
typedef struct{
    rs485_pixel_fx_state_enum   state;              /*0-停止 2-循环*/
    uint16_t                    intensity;          /*亮度<0-1000>*/
}rs485_belt_fx_party_arg_t;                 /*size: 3*/

/*0x0F：Belt_Fireworks*/
typedef struct{
    rs485_pixel_fx_state_enum   state;              /*0-停止 2-循环*/
    uint16_t                    intensity;          /*亮度<0-1000>*/
    uint8_t                     spd;                /*<0-9> -> <0.1-1秒>*/
    uint8_t                     pixel_length;       /*光柱长度：<0-9> -> <1-10>*/
}rs485_belt_fx_fireworks_arg_t;                     /*size: 5*/

/*0x10：Marbles*/
typedef struct{
    rs485_pixel_fx_state_enum   state;              /*0-停止 2-循环*/
    uint16_t                    intensity;          /*亮度<0-1000>*/
    uint8_t                     spd;                /*<0-9> -> <0.1-1秒>*/
    uint16_t                    base_color;         /*基础颜色：<0 - 359> -> <1°-360°> 511->无底色*/
}rs485_belt_fx_marbles_arg_t;               /*size: 6*/

/*0x11：Pendulum*/
typedef struct{
    rs485_pixel_fx_state_enum   state;              /*0-停止 2-循环*/
    uint16_t                    intensity;          /*亮度<0-1000>*/
    uint8_t                     sat;                /*<0 - 100>*/
    uint8_t                     spd;                /*<0-9> -> <0.1-1秒>*/
    uint8_t                     pixel_length;       /*光柱长度：<0-9> -> <1-10>*/
    uint16_t                    pixel_color;        /*光柱颜色：<0 - 359> -> <1°-360°>*/
    uint16_t                    base_color;         /*基础颜色：<0 - 359> -> <1°-360°> 511->无底色*/
}rs485_belt_fx_pendulum_arg_t;                      /*size: 10*/

/*0x12：Flash_Point*/
typedef struct{
    rs485_pixel_fx_state_enum   state;              /*0-停止 2-循环*/
    uint16_t                    intensity;          /*亮度<0-1000>*/
    uint8_t                     spd;                /*<0-9> -> <0.1-1秒>*/
    uint16_t                    pixel_color;        /*光柱颜色：<0 - 359> -> <1°-360°>*/
    uint16_t                    base_color;         /*基础颜色：<0 - 359> -> <1°-360°> 511->无底色*/
}rs485_belt_fx_flash_point_arg_t;                   /*size: 8*/

/*Belt_Effect_Arg定义*/
typedef union{
    rs485_belt_fx_fade_arg_t                fade;
    rs485_belt_fx_holiday_arg_t             holiday;
    rs485_belt_fx_one_color_marquee_arg_t   one_color_marquee;
    rs485_belt_fx_full_color_marquee_arg_t  full_color_marquee_move;
    rs485_belt_fx_starry_sky_arg_t          starry_sky;
    rs485_belt_fx_aurora_arg_t              aurora;
    rs485_belt_fx_racing_arg_t              racing;
    rs485_belt_fx_train_arg_t               train;
    rs485_belt_fx_snake_arg_t               snake;
    rs485_belt_fx_forest_arg_t              forest;
    rs485_belt_fx_bonfire_arg_t             bonfire;
    rs485_belt_fx_brook_arg_t               brook;
    rs485_belt_fx_game_arg_t                game;
    rs485_belt_fx_timewait_arg_t            timewait;
    rs485_belt_fx_party_arg_t               party;
    rs485_belt_fx_fireworks_arg_t           fireworks;
    rs485_belt_fx_marbles_arg_t             marbles;
    rs485_belt_fx_pendulum_arg_t            pendulum;
    rs485_belt_fx_flash_point_arg_t         flash_point;
}rs485_belt_fx_arg_t;

typedef struct{
    rs485_belt_fx_type_enum         type;   /*光效类型*/
    rs485_belt_fx_arg_t             arg;    /*光效参数*/
}rs485_belt_fx_body_t;   

/*0x0A：Music*/
typedef struct{
    rs485_pixel_fx_state_enum   state;      /*0-停止 1-启动*/
}rs485_pixel_fx_music_arg_t;                         /*size: 1*/



/*Pixel_Effect_Arg定义*/
typedef union{
    rs485_pixel_fx_color_cut_arg_t          color_cut;
    rs485_pixel_fx_color_rotate_arg_t       color_stay;
    rs485_pixel_fx_one_color_move_arg_t     one_color_move;
    rs485_pixel_fx_two_color_move_arg_t     two_color_move;
    rs485_pixel_fx_three_color_move_arg_t   three_color_move;
    rs485_pixel_fx_pixel_fire_arg_t         pixel_fire;
    rs485_pixel_fx_many_color_move_arg_t    many_color_move;
    rs485_pixel_fx_rainbow_arg_t            rainbow;
    rs485_pixel_fx_on_off_arg_t             on_off_fx;
    rs485_belt_fx_body_t                    belt_fx;
    rs485_pixel_fx_music_arg_t              music;
}rs485_pixel_fx_arg_t;

typedef struct{
    rs485_pixel_fx_type_enum    type;   /*光效类型*/
    rs485_pixel_fx_arg_t        arg;    /*光效参数*/
}rs485_pixel_fx_t;   

/*------------------------------------*/
/*---------DMX_Strobe：0x1F---------*/
/*------------------------------------*/
/*---DMX_Strobe光效里的子类型---*/
typedef enum{
    RS485_DMX_Strobe_Mode_CCT = 0,
    RS485_DMX_Strobe_Mode_HSI,
    RS485_DMX_Strobe_Mode_GEL,
    RS485_DMX_Strobe_Mode_XY,
    RS485_DMX_Strobe_Mode_SOUYRCE,
    RS485_DMX_Strobe_Mode_RGB,
    RS485_DMX_Strobe_Mode_RGBWW,
    RS485_DMX_Strobe_Mode_Color_Mixing,
    RS485_DMX_Strobe_Mode_Null,
}rs485_dmx_strobe_mode_enum;

/*-DMX_Strobe CCT-*/
typedef struct{
    float       intensity;      /*亮度：<0-1.0>，1.0代表100%*/
    uint16_t    cct;            /*CCT，单位k*/
    float       duv;            /*浮点数 <-0.04~0.04>或<-0.02~0.02>,产品不同，范围有所不同，默认0.00*/
}rs485_strobe_cct_t;            /*size: 10*/

/*-DMX_Strobe HSI-*/
typedef struct{
    float       intensity;      /*亮度：<0-1.0>，1.0代表100%*/
    uint16_t    hue;            /*hue<0-36000>，0-36000,36000-0*/
    uint16_t    sat;            /*饱和度：<0-10000>，10000代表100%*/
    uint16_t    cct;            /*中心白点色温：<2000-10000>*/
}rs485_strobe_hsi_t;            /*size: 10*/

/*-DMX_Strobe GEL-*/
typedef struct{
    float       intensity;      /*亮度：<0-1.0>，1.0代表100%*/
    uint16_t    cct;            /*CCT，单位k*/
    uint8_t     origin;         /*色纸品牌：0->LEE 1->Resco*/
    uint8_t     series;         /*色纸系列：<0-255>*/
    uint16_t    color;          /*色纸颜色：<0-65535>*/
}rs485_strobe_gel_t;            /*size: 10*/

/*DMX_Strobe XY_Coordinate*/
typedef struct{
    float       intensity;      /*亮度：<0-1.0>，1.0代表100%*/
    float    x;              /*x色坐标<0-10000>，除以10000表示实际色坐标*/
    float    y;              /*y色坐标<0-10000>，除以10000表示实际色坐标*/
}rs485_strobe_xy_t;             /*size: 8*/

/*-DMX_Strobe Light Source-*/
typedef struct{
    float       intensity;      /*亮度：<0-1.0>，1.0代表100%*/
    uint8_t     type;           /*Type*/
    float    x_coord;        /*x色坐标<0-10000>，除以10000表示实际色坐标*/
    float    y_coord;        /*y色坐标<0-10000>，除以10000表示实际色坐标*/
}rs485_strobe_source_t;         /*size: 9*/

/*-DMX_Strobe Light RGB-*/
typedef struct{
    float       intensity;      /*亮度：<0-1.0>，1.0代表100%*/
    float    red_ratio;      /*红光占比*/
    float    green_ratio;    /*绿光占比*/
    float    blue_ratio;     /*蓝光占比*/
}rs485_strobe_rgb_t;            /*size: 10*/

/*-DMX_Strobe Light RGBWW-*/
typedef struct{
    float       intensity;      /*亮度：<0-1.0>，1.0代表100%*/
    float    red_ratio;      /*红光占比*/
    float    green_ratio;    /*绿光占比*/
    float    blue_ratio;     /*蓝光占比*/
    float    ww_ratio;       /*绿光占比*/
    float    cw_ratio;       /*蓝光占比*/
}rs485_strobe_rgbww_t;          /*size: 14*/

/*Color_Mixing_Arg定义*/
typedef union{
    rs485_strobe_cct_t              cct;
    rs485_strobe_hsi_t              hsi;
    rs485_strobe_gel_t              gel;
    rs485_strobe_xy_t               xy;
    rs485_strobe_source_t           source;
    rs485_strobe_rgb_t              rgb;
}rs485_dmx_strobe_mixing_arg_t;      

typedef struct{
    rs485_dmx_strobe_mode_enum      color_1_mode;   /*0->CCT,1->HSI,2->GEL,3->XY,4->Source,5->RGB*/
    rs485_dmx_strobe_mixing_arg_t   color_1_arg;
    rs485_dmx_strobe_mode_enum      color_2_mode;   /*0->CCT,1->HSI,2->GEL,3->XY,4->Source,5->RGB*/
    rs485_dmx_strobe_mixing_arg_t   color_2_arg;
    float                        color_1_ratio;  /*<0-10000>ratio/10000表示Color1参与比例；Color2参与比例为(10000-ratio)/10000*/
}rs485_strobe_color_mixing_t;       

/*DMX_Strobe_Arg定义*/
typedef union{
    rs485_strobe_cct_t              cct;
    rs485_strobe_hsi_t              hsi;
    rs485_strobe_gel_t              gel;
    rs485_strobe_xy_t               xy;
    rs485_strobe_source_t           source;
    rs485_strobe_rgb_t              rgb;
    rs485_strobe_rgbww_t            rgbww;
    rs485_strobe_color_mixing_t     color_mixing;
}rs485_dmx_strobe_arg_t;        

typedef struct{
    float                       frq;
    rs485_dmx_strobe_mode_enum  mode;       /*0->CCT,1->HSI,2->GEL,3->XY,4->Source,5->RGB,6->RGBWW,7->Color_Mixing*/
    rs485_dmx_strobe_arg_t      mode_arg;
}rs485_dmx_strobe_t;               /*size: 25*/

/*------------------------------------*/
/*---------Partition_Color：0x20---------*/
/*------------------------------------*/
/*---Partition_Color子类型---*/
typedef enum{
    RS485_Partition_Color_Mode_CCT = 0,
    RS485_Partition_Color_Mode_HSI,
    RS485_Partition_Color_Mode_Null,
}rs485_partition_color_mode_enum;

/*-Partition_Color CCT-*/
typedef struct{
    float       intensity;      /*亮度：<0-1.0>，1.0代表100%*/
    uint16_t    cct;            /*CCT，单位k*/
    float       duv;            /*浮点数 <-0.04~0.04>或<-0.02~0.02>,产品不同，范围有所不同，默认0.00*/
}rs485_partition_color_cct_arg_t;   /*size: 10*/

/*-Partition_Color HSI-*/
typedef struct{
    float       intensity;      /*亮度：<0-1.0>，1.0代表100%*/
    uint16_t    hue;            /*hue<0-36000>，0-36000,36000-0*/
    uint16_t    sat;            /*饱和度<0-10000> -> <0-100%>调节精度0.01%*/
    uint16_t    cct;            /*中心白点色温：<2000-10000>*/
}rs485_partition_color_hsi_arg_t;   /*size: 10*/

/*Partition_Color_Arg定义*/
typedef union{
    rs485_partition_color_cct_arg_t     cct;
    rs485_partition_color_hsi_arg_t     hsi;
}rs485_partition_color_mode_arg_t;       /*size: 8*/

typedef struct{
    uint8_t                             pixel_index;    /*<0-36> -> <像素1 - 像素36>*/
    uint8_t                             fx_add;         /*是否开启叠加光效FX:0x00->开启,0x01->不开启*/
    rs485_partition_color_mode_enum     mode;           /*0->CCT,1->HSI*/
    rs485_partition_color_mode_arg_t    mode_arg;
}rs485_partition_color_t;                      /*size: 15*/    

/*------------------------------------*/
/*---------Partition_Effect：0x21---------*/
/*------------------------------------*/
/*---Partition_Effect光效里的子类型---*/
typedef enum {
    RS485_Partition_FX_Mode_Strobe = 0,
    RS485_Partition_FX_Mode_Flicker,
    RS485_Partition_FX_Mode_Pulsing,
    RS485_Partition_FX_Mode_Null,
}rs485_partition_fx_mode_enum;

/*-Partition Effect Strobe-*/
typedef struct{
    uint8_t     lasting_min;    /*0～69，0-1s(0.1步进)，1-60s(1步进)*/
    uint8_t     lasting_max;    /*0～69，0-1s(0.1步进)，1-60s(1步进)*/
    uint8_t     interval_min;   /*0～69，0-1s(0.1步进)，1-60s(1步进)*/
    uint8_t     interval_max;   /*0～69，0-1s(0.1步进)，1-60s(1步进)*/
    uint8_t     frq_min;        /*0～29，0.1-1Hz(0.1步进)1-20Hz(1步进)*/
    uint8_t     frq_max;        /*0～29，0.1-1Hz(0.1步进)1-20Hz(1步进)*/
    uint8_t     trigger;        /*0->像素统一,1->像素独立*/
}rs485_partition_fx_strobe_arg_t;/*size: 7*/

/*-Partition Effect Flicker-*/
typedef struct{
    uint8_t     lasting_min;    /*0～69，0-1s(0.1步进)，1-60s(1步进)*/
    uint8_t     lasting_max;    /*0～69，0-1s(0.1步进)，1-60s(1步进)*/
    uint8_t     interval_min;   /*0～69，0-1s(0.1步进)，1-60s(1步进)*/
    uint8_t     interval_max;   /*0～69，0-1s(0.1步进)，1-60s(1步进)*/
    uint8_t     frq_min;        /*0～29，0.1-1Hz(0.1步进)1-20Hz(1步进)*/
    uint8_t     frq_max;        /*0～29，0.1-1Hz(0.1步进)1-20Hz(1步进)*/
    uint8_t     trigger;        /*0->像素统一,1->像素独立*/
    uint8_t     min_intensity;  /*<0~100>*/
}rs485_partition_fx_flicker_arg_t;/*size: 8*/

/*-Partition Effect Pulsing-*/
typedef struct{
    uint8_t     lasting_min;    /*0～69，0-1s(0.1步进)，1-60s(1步进)*/
    uint8_t     lasting_max;    /*0～69，0-1s(0.1步进)，1-60s(1步进)*/
    uint8_t     interval_min;   /*0～69，0-1s(0.1步进)，1-60s(1步进)*/
    uint8_t     interval_max;   /*0～69，0-1s(0.1步进)，1-60s(1步进)*/
    uint8_t     frq_min;        /*0～29，0.1-1Hz(0.1步进)1-20Hz(1步进)*/
    uint8_t     frq_max;        /*0～29，0.1-1Hz(0.1步进)1-20Hz(1步进)*/
    uint8_t     trigger;        /*0->像素统一,1->像素独立*/
    uint8_t     max_intensity;  /*<0~100>*/
}rs485_partition_fx_pulsing_arg_t;/*size: 8*/

/*Partition_Effect_Arg定义*/
typedef union{
    rs485_partition_fx_strobe_arg_t     strobe;
    rs485_partition_fx_flicker_arg_t    flicker;
    rs485_partition_fx_pulsing_arg_t    pulsing;
}rs485_partition_fx_arg_t;      /*size: 8*/

typedef struct{
    rs485_partition_fx_mode_enum    mode;       /*0->Strobe,1->Flicker,2->Pulsing*/
    rs485_partition_fx_arg_t        mode_arg;
}rs485_partition_fx_t;                 /*size: 9*/


typedef struct{
    uint8_t     mode;          /*<0-1>, 0x00: PWM;   0x01: 模拟调光 */
}rs485_high_speed_mode_t;       /*size: 1*/

typedef struct{
	uint8_t     type;
    uint8_t     led_color_type; /*灯颜色类型 0->所有灯，1->红灯，2->绿灯，3->蓝灯，4->暖白灯，5->冷白灯*/
    uint16_t    adjust_val;     /*调节值 0-65535*/
}rs485_analog_dim_t;    /*size: 6*/

/*------------------------------------*/
/*---------Motor_arg：0x26---------*/
/*------------------------------------*/
/*---Motor_arg里的子类型---*/
typedef enum {
    RS485_Motor_Mode_Move = 0,
    RS485_Motor_Mode_Crc,
    RS485_Motor_Mode_Reset,
    RS485_Motor_Mode_State,
    RS485_Motor_Mode_Null,
}rs485_motor_mode_enum;

/*---Motor_type里的子类型---*/
typedef enum {
    RS485_Pitch_Motor = 0,
    RS485_Rotale_Motor,
    RS485_Fresnel_Motor,
	RS485_Gyroscope_Roll,
	RS485_Gyroscope_Pitch,
    RS485_Motor_Null = 99,
}rs485_motor_type_enum;

/*-motor_move_arg-*/
typedef struct{
    uint8_t      move_state;         /*0-1  1--到达目标位置*/
    float     	 motor_angle;        /*0~xxx°          角度*/
    uint32_t     motor_star_speed;   /*xxx           起始速度*/
    uint32_t     motor_a1;           /*xxx           起始速度和v1的加速度*/
    uint32_t     motor_v1;           /*xxx           第一阶段速度*/
    uint32_t     motor_acce;         /*xxx           V1和目标速度之间的加速度*/
    uint32_t     motor_targe_speed;  /*xxx         目标速度*/
    uint32_t     motor_dece;         /*xxx           V1和目标速度之间的减速度*/
    uint32_t     motor_d1;           /*xxx           第一阶段速度到stop的减速度*/
    uint32_t     motor_stop_speed;   /*xxx           停止速度*/
}rs485_motor_move_arg_t;/*size: 15*/

/*-motor_crc_arg-*/
typedef struct{
    uint8_t      crc_state;          /*0-1  堵转校准状态*/
    float        crc_value;          /*0--1024  堵转反馈值*/
    int32_t      sttall_min_value;   /*0--1024  堵转触发最小值*/
    int32_t      sttall_max_value;   /*0--1024  堵转触发最大值*/
    uint32_t     reserva2;
    uint32_t     reserva3;
    uint32_t     reserva4;
    uint32_t     reserva5;
    uint32_t     reserva6;
    uint32_t     reserva7;
}rs485_motor_crc_arg_t;/*size: 15*/

/*-motor_reset_arg-*/
typedef struct{
    uint8_t     reset_state;    /*0~1     复位状态*/
    float       reset_angle;    /*0~xxx°  复位后的角度*/
    uint32_t    reserva;
    uint32_t    reserva1;
    uint32_t    reserva2;
    uint32_t    reserva3;
    uint32_t    reserva4;
    uint32_t    reserva5;
    uint32_t    reserva6;
    uint32_t    reserva7;
}rs485_motor_reset_arg_t;/*size: 15*/

/*-motor_state_arg-*/
typedef struct{
    uint8_t     stall_state;    /*0~1     复位状态*/
    float       reserva;        
    uint32_t    reserva1;
    uint32_t    reserva2;
    uint32_t    reserva3;
    uint32_t    reserva4;
    uint32_t    reserva5;
    uint32_t    reserva6;
    uint32_t    reserva7;
    uint32_t    reserva8;
}rs485_motor_state_arg_t; /*size: 15*/

typedef union{
    rs485_motor_move_arg_t     motor_move_arg;
    rs485_motor_crc_arg_t      motor_crc_arg;
    rs485_motor_reset_arg_t    motor_reset_arg;
    rs485_motor_state_arg_t    motor_state_arg;
}rs485_motor_data_arg_t;      /*size: 15*/

typedef struct{
    rs485_motor_move_arg_t     motor_move_arg;
    rs485_motor_crc_arg_t      motor_crc_arg;
    rs485_motor_reset_arg_t    motor_reset_arg;
    rs485_motor_state_arg_t    motor_state_arg;
}local_motor_data_arg_t;      /*size: 15*/

/*Partition_Effect_Arg定义*/
typedef struct{
    rs485_motor_mode_enum      motor_mode;
    rs485_motor_data_arg_t     motor_arg;
}rs485_motor_arg_t;      /*size: 4*/

/*Partition_Effect_Arg定义*/
typedef struct{
    rs485_motor_mode_enum      motor_mode;
    local_motor_data_arg_t     motor_arg;
}local_rs485_motor_arg_t;      /*size: 4*/

typedef struct{
    rs485_motor_type_enum       type;
    rs485_motor_arg_t           mode_arg;
}rs485_motor_t;                 /*size: 5*/

typedef struct{
    rs485_motor_type_enum       type;
    local_rs485_motor_arg_t           mode_arg;
}local_rs485_motor_t;                 /*size: 5*/


/*------------------------------------*/
/*---------Accessories_arg：0x27---------*/
/*------------------------------------*/
/*---Accessories_arg里的子类型---*/
typedef enum {
    RS485_Access_NOT,             //空
    RS485_Access_Bracket,         //电动支架
    RS485_Access_Fresnel,         //电动菲涅尔
    RS485_Access_20_Reflector,    //20°反光罩
    RS485_Access_30_Reflector,    //30°反光罩
    RS485_Access_50_Reflector,    //50°反光罩
	RS485_Access_Adapter_Bowl,	  //转接碗
//	RS485_Access_Protect_Cover,	  //保护罩 
    RS485_Access_Num = 99,
}rs485_access_type_enum;     /*size: 1*/

typedef struct{
    uint8_t state;           //0--不存在  1--存在
}rs485_access_state_t;       /*size: 1*/

typedef struct{
    rs485_access_type_enum       type;
    rs485_access_state_t         access_state;
}rs485_access_t;                 /*size: 2*/

typedef enum {
    RS485_Version2_Bracket,         //电动支架
    RS485_Version2_Fresnel,         //电动菲涅尔
    RS485_Version2_Num = 0x10000000,
}rs485_version2_type_enum;     /*size: 1*/
typedef struct{
    rs485_version2_type_enum  product_type;      /*Product，产品类型，0x006B表示107*/
    uint8_t   hard_ver_major;   /*硬件主版本号 版本号定义：主.次.修订 如0x01 0x00 0x00 表示1.0.0 两位版本号可舍弃修订版本号*/
    uint8_t   hard_ver_minor;   /*硬件次版本号*/
    uint8_t   hard_ver_revision;/*硬件修订版本号*/
    uint8_t   soft_ver_major;   /*软件主版本号 版本号定义：主.次.修订 如0x01 0x00 0x00 表示1.0.0 两位版本号可舍弃修订版本号*/
    uint8_t   soft_ver_minor;   /*软件次版本号*/
    uint8_t   soft_ver_revision;/*软件修订版本号*/
}rs485_version2_t;               /*size: 10*/

/*------------------------------------*/
/*----------Factory_PWM_Crc：0x28-------*/
/*------------------------------------*/
typedef struct{
    uint16_t    red;            /*红灯pwm<0-65535>*/
    uint16_t    green;          /*绿灯pwm<0-65535>*/
    uint16_t    blue;           /*蓝灯pwm<0-65535>*/
    uint16_t    ww;             /*暖白pwm<0-65535>*/
    uint16_t    cw;             /*冷白pwm<0-65535>*/
    uint16_t    ww2;             /*冷白pwm<0-65535>*/
    uint16_t    cw2;             /*冷白pwm<0-65535>*/
    uint8_t     state[7];           /* 保存标记位0---不保存   1---保存结束 */
}rs485_factory_rgbww_crc_t;    /*size: 21*/

/*----------Clear_Run_Time：0x30----------*/
/*------------------------------------*/
typedef struct{
    uint8_t   clear_run_time_flag;   /*开启清除运行时间标记*/
}rs485_clear_run_time_t;   
/*------------------------------------- Command Argument----------------------------------------------*/
typedef union{
    rs485_version_t             version;
    rs485_hsi_t                 hsi;
    rs485_cct_t                 cct;
    rs485_gel_t                 gel;
    rs485_rgb_t                 rgb;
    rs485_xy_coord_t            xy_coord;
    rs485_dimming_frq_t         dimming_frq;
    rs485_sys_fx_t              sys_fx;
    rs485_dimming_curve_t       dimming_curve;
    rs485_fan_t                 fan;
    rs485_power_t               power_suppy;
    rs485_battery_t             battery_state;
    rs485_switch_t              power_switch;
    rs485_file_transfer_t       file_transfer;    
    rs485_temperature_msg_t     temperature_msg;
    rs485_sys_fx_2_t            sys_fx_2;
    rs485_self_adaption_t       self_adaption;
    rs485_factory_rgbww_t       factory_rgbww;
    rs485_source_t              source;
    rs485_illumination_mode_t   illumination_mode;
    rs485_color_mixing_t        color_mixing;
    rs485_err_msg_t             err_msg;
    rs485_pfx_ctrl_t            pfx_ctrl;
    rs485_cfx_bank_rw_t         cfx_bank_rw;
    rs485_cfx_ctrl_t            cfx_ctrl;
    rs485_cfx_preview_t         cfx_preview;
    rs485_mfx_ctrl_t            mfx_ctrl;
    rs485_rgbww_t               rgbww;
    rs485_cfx_name_t            cfx_name;
    rs485_curr_light_mode_t     curr_light_mode;
    rs485_pixel_fx_t            pixel_fx;
    rs485_dmx_strobe_t          dmx_strobe;
    rs485_partition_color_t     partition_color;
    rs485_partition_fx_t        partition_fx;
    rs485_high_speed_mode_t     high_speed_mode;
    rs485_analog_dim_t          analog_dim_t;
    rs485_pump_t                pump_t;
    rs485_self_adjust_t         self_adjust_t;
    rs485_motor_t               motor_t;
    rs485_access_t              access_t;
	rs485_version2_t            version2_t; 
	rs485_factory_rgbww_crc_t   factory_rgbww_crc_t;
	rs485_clear_run_time_t      clear_run_time_t;
}rs485_cmd_arg_t;              /*size: */

/*----------命令包----------*/
typedef struct{
    rs485_cmd_header_t              header;     /* 命令包包头包头*/
    rs485_cmd_arg_t                 cmd_arg;    /* 命令包数据部分*/
}rs485_cmd_body_t;                              /*size: */
/*------------------------------------- End ----------------------------------------------*/
typedef enum{
    RS485_Ack_Ok = 0,
    //主要是升级类
    RS485_Ack_Err_Over_Size,
    RS485_Ack_Err_Erase,
    RS485_Ack_Err_File_Head,
    RS485_Ack_Err_Sequence,
    RS485_Ack_Err_Write,          //（写错误，不支持写）
    RS485_Ack_Err_Verify,
    RS485_Ack_Err_Timeout,
    RS485_Ack_Err_Step,
    //主要是常规控制类
    RS485_Ack_Err_Len,            //（帧长度错误）
    RS485_Ack_Err_Read,           //（读错误，不支持读）
    RS485_Ack_Err_Command,        //（命令类型不支持）
    RS485_Ack_Err_Busy,           //(忙错误，发送太频繁)
}rs485_ack_enum;

/*----------应答参数----------*/
typedef struct{
    rs485_ack_enum          ack;      /*应答类型*/
}rs485_ack_arg_t;              /*size: 1*/

/*----------应答命令包，主机读从机且读操作正确，则不调用应答包----------*/
typedef struct{
    rs485_cmd_header_t              header;     /*应答包的命令类型与原包一样*/
    rs485_ack_arg_t                 ack_arg;    /*通信命令包数据部分*/
}rs485_ack_body_t;                      /*size: 5*/


#pragma pack ()

#endif  /*__USER_PROTO_H*/


