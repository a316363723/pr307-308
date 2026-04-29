/*********************************************************************************
  *Copyright(C), 2021, Aputure, All rights reserver.
  *FileName:      user_proto.h
  *Author:
  *Version:       v1.9
  *Date:          2021/04/22
  *Description: 定义控制盒与驱动板通信的数据包格式
  *History:
        -v1.0: 初始版本
        -v1.1:
              1、添加通信包格式错误响应数据包：CommandType添加0x0E：PacketErr
        -v1.2：2019/08/03,Steven,对应v2.11版本485通信协议
              1、将0x11：Auto_Test更改为Factory_RGBWW，用于测试基础数据
              2、修改光效参数、添加off和定位光效
              3、添加Source：0x12协议
              4、Get_Power添加参数：电池剩余电量(%)、电池剩余使用时间
        -v1.3：2019/08/15,Steven,升级协议修改，升级应答包长度变短
        -v1.4：2019/08/27,Steven,对应v2.13版本485通信协议
              1、0x12：source模式添加x、y色坐标参数，支持色坐标微调
              2、HSI、CCT、XY_Coordinate、Gel、WWRGB、Source模式添加Fade控制标识位，支持缓变和瞬变两种亮度变化模式
        -v1.5：2020/12/07,Steven,对应v2.16版本485通信协议
              1、添加输出模式
              2、增加DMX颜色混合模式
        -v1.6：2020/12/15,Steven,对应v2.18版本485通信协议
              1、增加控制板状态指令，0x15:Control_State，控制板状态，温度等信息
              2、在0x0B：Board_State，驱动板状中添加风扇转速信息
        -v1.7：2021/01/13,Matthew,对应v2.19版本485通信协议
              1、新增PFX_Ctrl(Program Effect)命令
              2、新增CFX_Bank_RW命令，读取Custom Effect的Bank信息或者删除对应Bank光效
              3、新增CFX_Ctrl命令，用于控制Custom Effect
              4、新增CFX_Preview命令，用于预览Custom Effect
              5、新增MFX_Ctrl命令，用于预览Manual Effect
              6、新增File_Transfer命令，用于传输大文件，该版本新增CFX_File文件类型
        -v1.8：2021/04/12,Steven,对应v3.1版本485通信协议
        -v1.9：2021/04/22,Matthew,对应v3.11版本485通信协议
              1、CFX_Ctrl新增Init Result，用于判断Init结果
              2、新增CFX_Name：0x1D用于读写CFX文件名

**********************************************************************************/
#ifndef __RS485_PROTO_H
#define __RS485_PROTO_H

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

-通信数据包由3部分组成：
-    1、Header(通信数据包包头)：5 byte
-    2、CommandBody(通信数据包数据部分)：26 byte
-    3、CheckSum(所有数据校验和)：1 byte

-    Header|CommandBody|CheckSum

-    Header(5 byte):Start(1 byte)|Length(1 byte)|ProtocolVer(1 byte)|RW(1 byte)|CommandType(1 byte)
-    CommandBody(26 byte)
-    CheckSum(1 byte)
***************************************************************************************************/
#define HEADER_SIZE             (5)        //HEADER长度

#define PACKET_SIZE             (32)    //数据包长度
#define CommandBody_Max         (26)    //数据包CommandBody长度
#define ModeArg_Max             (12)    //数据包ModeArg长度

#define UPGRADE_PACKET_SIZE     (140)    //升级数据包长度
#define UPGRADE_CommandBody_Max (134)    //数据包CommandBody长度
#define UPGRADE_ACK_SIZE        (HEADER_SIZE+2)    //升级数据应答包长度

#define FileTransfer_PACKET_SIZE        (140)    //文件传输数据包长度
#define FileTransfer_CommandBody_Max    (134)    //数据包CommandBody长度
#define FileTransfer_ACK_SIZE           (HEADER_SIZE+2)    //文件传输应答包长度

#define PACKET_START            (0x55)  //包头默认0x55
#define PACKET_LENGTH           (0x20)    //数据包长度
#define PROTOCOL_VER            (0x10)    //协议版本v2.3

//#define COMMAND_READ            (0x00)    //读
//#define COMMAND_WRITE           (0x01)    //写

#define PACKET_SIZE_MAX         UPGRADE_PACKET_SIZE  //通信包最大长度

#if 0
/*---命令类型---*/
typedef enum {

    CommandType_Version = 0,
    CommandType_HSI,
    CommandType_CCT,
    CommandType_GEL,
    CommandType_RGBWW,
    CommandType_XY_Coordinate,
    CommandType_LED_Frequency,
    CommandType_Light_Effect,
    CommandType_Dimming_Curve,
    CommandType_Fan_Speed,
    CommandType_Get_Power,
    CommandType_Board_State,
    CommandType_Sleep_Mode,
    CommandType_Upgrade,
    CommandType_PacketErr,
    CommandType_Main_Interface,
    CommandType_Beat,
    CommandType_Factory_RGBWW,
    CommandType_Source,
    CommandType_Illumination_Mode,
    CommandType_Color_Mixing,
    CommandType_Control_State,
    CommandType_PFX_Ctrl,
    CommandType_CFX_Bank_RW,
    CommandType_CFX_Ctrl,
    CommandType_CFX_Preview,
    CommandType_MFX_Ctrl,
    CommandType_File_Transfer,
    CommandType_RGBWW_1,
	CommandType_CFX_Name,
    CommandType_Factory_CCT = 0xFE,
    CommandType_NULL,
} CommandType_TypeDef;

/*---光效类型---*/
typedef enum {

    EffectType_ClubLights = 0,
    EffectType_Paparazzi,
    EffectType_Lightning,
    EffectType_TV,
    EffectType_Candle,
    EffectType_Fire,
    EffectType_Strobe,
    EffectType_Explosion,
    EffectType_FaultBulb,
    EffectType_Pulsing,
    EffectType_Welding,
    EffectType_CopCar,
    EffectType_ColorChase,
    EffectType_PartyLights,
    EffectType_Fireworks,
    EffectType_Effect_Off,
    EffectType_I_Am_Here,
    EffectType_ColorFade,
    EffectType_ColorCycle,
    EffectType_ColorGradient,
    EffectType_OneColorChase,
    EffectType_NULL,
} EffectType_TypeDef;

/*---光效里的子类型---*/
typedef enum {

    EffectMode_CCT = 0,
    EffectMode_HSI,
    EffectMode_GEL,
    EffectMode_XY,
    EffectMode_SOUYRCE,
	AUTO_DEFINE_WWRGB, //自定义的
    EffectMode_NULL,
} EffectMode_TypeDef;

/*---光效触发类型---*/
typedef enum {

    EffectTrigger_None = 0,
    EffectTrigger_Once,
    EffectTrigger_Continue
} EffectTrigger_TypeDef;

/*---通信错误类型---*/
typedef enum {

    ERR_LENGTH = 0,
    ERR_CHECKSUM,
    ERR_WRITE,
    ERR_READ,
    ERR_COMMAND,
    ERR_FAST,
    ERR_NULL
} ErrorNum_TypeDef;

/*-------------------------------------- Header --------------------------------------------------*/
typedef __packed struct {

    uint8_t   Start;            /*表示通信包起始数据，默认值为0x55*/
    uint8_t   Length;           /*整个数据包长度（包含包头和校验字节），固定长度32byte*/
    uint8_t   ProtocolVer;      /*通信协议版本，例如0x23表示V2.3*/
    uint8_t   RW;               /*表示读或写,0x00：表示读; 0x01：表示写*/
    CommandType_TypeDef   CommandType;        /*命令类型*/
} Header_TypeDef;                 /*size: 5*/

/*------------------------------------- CommandBody ----------------------------------------------*/
/*----------Version：0x00----------*/
typedef __packed struct {

    uint32_t  Product;          /*Product，产品类型，0x006B表示107*/
    uint8_t   HwVer;            /*HwVer，硬件版本，默认0x10表示版本V1.0*/
    uint8_t   SwVer;            /*软件版本，默认0x10表示版本V1.0*/
} Version_Body_TypeDef;             /*size: 6*/

/*----------HSI：0x01------------*/
typedef __packed struct {

    uint16_t  INT;              /*亮度<0-1000>*/
    uint16_t  HUE;              /*hue<0-360>，0-360,360-0*/
    uint8_t   SAT;              /*饱和度<0-100>*/
    uint8_t   Fade;             /*Fade，Fade控制模式<0-1>，0表示缓变，1表示使用瞬变*/
    uint16_t  CCT;              /*中心色温*/
    uint16_t  Channel;          /*点控通道，0表示全控，n表示通道n，默认0*/
} HSI_Body_TypeDef;                 /*size: 6*/

/*----------CCT：0x02-----------*/
typedef __packed struct {

    uint16_t  INT;              /*亮度<0-1000>*/
    uint16_t  CCT;              /*CCT，单位k*/
    uint8_t   GM;               /*<0-20>: 0->-1.0,1->-0.9,2->-0.8,   10->0,20->+1.0每隔0.1递增1，以此类推*/
    uint8_t   Fade;             /*Fade，Fade控制模式<0-1>，0表示缓变，1表示使用瞬变*/
    uint16_t  Channel;          /*点控通道，0表示全控，n表示通道n，默认0*/
} CCT_Body_TypeDef;                 /*size: 6*/

/*----------GEL：0x03-----------*/
typedef __packed struct {

    uint16_t  INT;              /*亮度<0-1000>*/
    uint16_t  CCT;              /*CCT，单位k*/
    uint8_t   Gel_Origin;       /*Gel_Origin ，品牌，0x00：LEE; 0x01：Resco*/
    uint8_t   Gel_Type;         /*Gel_Type*/
    uint16_t  Gel_Color;        /*Gel_Color*/
    uint8_t   Fade;             /*Fade，Fade控制模式<0-1>，0表示缓变，1表示使用瞬变*/
    uint16_t  Channel;          /*点控通道，0表示全控，n表示通道n，默认0*/
} Gel_Body_TypeDef;                 /*size: 9*/

/*----------RGBWW：0x04---------*/
typedef __packed struct {

    uint16_t  Red_INT;          /*红灯亮度<0-1000>*/
    uint16_t  Green_INT;        /*绿灯亮度<0-1000>*/
    uint16_t  Blue_INT;         /*蓝灯亮度<0-1000>*/
    uint16_t  WW_INT;           /*暖白亮度<0-1000>*/
    uint16_t  CW_INT;           /*冷白亮度<0-1000>*/
    uint8_t   Fade;             /*Fade，Fade控制模式<0-1>，0表示缓变，1表示使用瞬变*/
    uint16_t  INT;              /*总亮度<0-1000>*/
    uint16_t  Channel;          /*点控通道，0表示全控，n表示通道n，默认0*/
} RGBWW_Body_TypeDef;               /*size: 11*/

/*------XY_Coordinate：0x05-----*/
typedef __packed struct {

    uint16_t  INT;              /*亮度<0-1000>*/
    uint16_t  X_Coordinate;     /*x色坐标<0-10000>，除以10000表示实际色坐标*/
    uint16_t  Y_Coordinate;     /*y色坐标<0-10000>，除以10000表示实际色坐标*/
    uint8_t   Fade;             /*Fade，Fade控制模式<0-1>，0表示缓变，1表示使用瞬变*/
    uint16_t  Channel;          /*点控通道，0表示全控，n表示通道n，默认0*/
} XY_Coordinate_Body_TypeDef;     /*size: 7*/

/*----------Light Source：0x12-------*/
typedef __packed struct {

    uint16_t  INT;              /*亮度<0-1000>*/
    uint8_t   Type;             /*Type*/
    uint16_t  X_Coordinate;     /*x色坐标<0-10000>，除以10000表示实际色坐标*/
    uint16_t  Y_Coordinate;     /*y色坐标<0-10000>，除以10000表示实际色坐标*/
    uint8_t   Fade;             /*Fade，Fade控制模式<0-1>，0表示缓变，1表示使用瞬变*/
    uint16_t  Channel;          /*点控通道，0表示全控，n表示通道n，默认0*/
} Source_Body_TypeDef;            /*size: 4*/

/*----------Illumination_Mode：0x13-------*/
typedef __packed struct {

    uint8_t   Mode;             /*照度模式,0x00：最大功率输出 0x01：恒照度输出*/
    uint16_t  Ratio;            /*<0-100>%，功率比例，功率比例，默认10000，10000代表100%*/
    uint8_t   Save;             /*<0-1>%，功率核准，是否保存功率，1保存到灯体，0不保存*/
} IlluminationMode_Body_TypeDef;    /*size: 3*/

/*----------DMX Color_Mixing：0x14-------*/

/*---ColorMixing里的子类型---*/
typedef enum {

    ColorMixingMode_CCT = 0,
    ColorMixingMode_HSI,
    ColorMixingMode_GEL,
    ColorMixingMode_XY,
    ColorMixingMode_SOUYRCE,
    ColorMixingMode_RGB,
    ColorMixingMode_NULL,
} ColorMixingMode_TypeDef;

/*-ColorMixing CCT-*/
typedef __packed struct {

    uint16_t  INT;              /*亮度<0-1000>*/
    uint16_t  CCT;              /*CCT，单位k*/
    uint8_t   GM;               /*GM分20档<0-20>: 0->-1.0,1->-0.9,2->-0.8,   10->0,20->+1.0每隔0.1递增1，以此类推
                                  GM分200档：<-100 - +100>: -100->-0.04, -1->-0.0004, 0-> 0, 1->0.0004,100->0.04*/
} ColorMixing_CCT_TypeDef;             /*size: 5*/

/*-ColorMixing HSI-*/
typedef __packed struct {

    uint16_t  INT;              /*亮度<0-1000>*/
    uint16_t  HUE;              /*hue<0-360>，0-360,360-0*/
    uint8_t   SAT;              /*饱和度<0-100>*/
} ColorMixing_HSI_TypeDef;             /*size: 5*/

/*-ColorMixing GEL-*/
typedef __packed struct {

    uint16_t  INT;              /*亮度<0-1000>*/
    uint16_t  CCT;              /*CCT，单位k*/
    uint8_t   Gel_Origin;       /*Gel_Origin ，品牌，0x00：LEE; 0x01：Resco*/
    uint8_t   Gel_Type;         /*Gel_Type*/
    uint16_t  Gel_Color;        /*Gel_Color*/
} ColorMixing_Gel_TypeDef;             /*size: 8*/

/*ColorMixing XY_Coordinate*/
typedef __packed struct {

    uint16_t  INT;              /*亮度<0-1000>*/
    uint16_t  X_Coordinate;     /*x色坐标<0-10000>，除以10000表示实际色坐标*/
    uint16_t  Y_Coordinate;     /*y色坐标<0-10000>，除以10000表示实际色坐标*/
} ColorMixing_XY_Coordinate_TypeDef;     /*size: 6*/

/*-ColorMixing Light Source-*/
typedef __packed struct {

    uint16_t  INT;              /*亮度<0-1000>*/
    uint8_t   Type;             /*Type*/
    uint16_t  X_Coordinate;     /*x色坐标<0-10000>，除以10000表示实际色坐标*/
    uint16_t  Y_Coordinate;     /*y色坐标<0-10000>，除以10000表示实际色坐标*/
} ColorMixing_Source_TypeDef;    /*size: */

/*-ColorMixing RGB-*/
typedef __packed struct {

    uint16_t  INT;              /*亮度<0-1000>*/
    uint16_t  Red_INT;          /*红灯亮度<0-1000>*/
    uint16_t  Green_INT;        /*绿灯亮度<0-1000>*/
    uint16_t  Blue_INT;         /*蓝灯亮度<0-1000>*/
} ColorMixing_RGB_TypeDef;            /*size: */
typedef __packed struct {

    ColorMixingMode_TypeDef Color_Mode;
    __packed union {
        ColorMixing_CCT_TypeDef ColorMixing_CCT;
        ColorMixing_HSI_TypeDef ColorMixing_HSI;
        ColorMixing_Gel_TypeDef ColorMixing_Gel;
        ColorMixing_XY_Coordinate_TypeDef ColorMixing_XY_Coordinate;
        ColorMixing_Source_TypeDef ColorMixing_Source;
        ColorMixing_RGB_TypeDef ColorMixing_RGB;
        uint8_t   DefaultModeArg_Buffer[9];   //定义默认ModeArg长度，CommandBody_Max
    } Arg;

} Color_Arg_TypeDef;
typedef __packed struct {

    Color_Arg_TypeDef   Color1_Arg;         /*照度模式,0x00：最大功率输出 0x01：恒照度输出*/
    Color_Arg_TypeDef   Color2_Arg;
    uint16_t  Ratio;                /*颜色参与比例<0-10000>， ratio/10000表示Color1参与比例，Color2参与比例为(10000-ratio)/10000%*/
    uint8_t   Fade;                 /*控制模式<0-1>，0表示缓变，1表示使用瞬变*/
    uint16_t  Channel;              /*点控通道，0表示全控，n表示通道n，默认0*/
} ColorMixing_Body_TypeDef;    /*size: */

/*-----LED_Frequency：0x06------*/
typedef __packed struct {

    uint16_t  Frequency;          /*频率<1-10>*/
} LED_Frequency_Body_TypeDef;     /*size: 2*/

/*------------------------------------*/
/*---------Light_Effect：0x07---------*/
/*------------------------------------*/
/*-Effect CCT-*/
typedef __packed struct {

    uint16_t  INT;              /*亮度<0-1000>*/
    uint16_t  CCT;              /*CCT，单位k*/
    uint8_t   GM;               /*<0-20>: 0->-1.0,1->-0.9,2->-0.8,   10->0,20->+1.0每隔0.1递增1，以此类推*/
} Effect_CCT_TypeDef;             /*size: 5*/

/*-Effect HSI-*/
typedef __packed struct {

    uint16_t  INT;              /*亮度<0-1000>*/
    uint16_t  HUE;              /*hue<0-360>，0-360,360-0*/
    uint8_t   SAT;              /*饱和度<0-100>*/
} Effect_HSI_TypeDef;             /*size: 5*/

/*-Effect GEL-*/
typedef __packed struct {

    uint16_t  INT;              /*亮度<0-1000>*/
    uint16_t  CCT;              /*CCT，单位k*/
    uint8_t   Gel_Origin;       /*Gel_Origin ，品牌，0x00：LEE; 0x01：Resco*/
    uint8_t   Gel_Type;         /*Gel_Type*/
    uint16_t  Gel_Color;        /*Gel_Color*/
} Effect_Gel_TypeDef;             /*size: 8*/

/*Effect XY_Coordinate*/
typedef __packed struct {

    uint16_t  INT;              /*亮度<0-1000>*/
    uint16_t  X_Coordinate;     /*x色坐标<0-10000>，除以10000表示实际色坐标*/
    uint16_t  Y_Coordinate;     /*y色坐标<0-10000>，除以10000表示实际色坐标*/
} Effect_XY_Coordinate_TypeDef;     /*size: 6*/

/*-Effect Light Source-*/
typedef __packed struct {

    uint16_t  INT;              /*亮度<0-1000>*/
    uint8_t   Type;             /*Type*/
} Effect_Source_TypeDef;            /*size: 3*/
/*Mode_Arg定义*/
typedef __packed union {

    Effect_CCT_TypeDef              CCT_Mode_Arg;
    Effect_HSI_TypeDef              HSI_Mode_Arg;
    Effect_Gel_TypeDef              Gel_Mode_Arg;
    Effect_XY_Coordinate_TypeDef    XY_Mode_Arg;
    Effect_Source_TypeDef           Source_Mode_Arg;
    uint8_t   DefaultModeArg_Buffer[ModeArg_Max];   //定义默认ModeArg长度，CommandBody_Max
} Mode_Arg_TypeDef;

/*0x00：Club_Lights*/
typedef __packed struct {

    uint16_t  INT;              /*亮度<0-1000>*/
    uint8_t   FRQ;              /*<1-11>,11为Random*/
    uint8_t   Color;            /*<0-3>,0:3; 1:6; 2:9; 3:12*/
} Club_Lights_Arg_TypeDef;

/*0x01：Paparazzi*/
typedef __packed struct {

    uint16_t  INT;              /*亮度<0-1000>*/
    uint8_t   FRQ;              /*<1-11>,11为Random*/
    uint16_t  CCT;              /*CCT，单位k*/
    uint8_t   GM;               /*<0-20>: 0->-1.0,1->-0.9,2->-0.8,   10->0,20->+1.0每隔0.1递增1，以此类推*/
} Paparazzi_Arg_TypeDef;

/*0x02：Lightning*/
typedef __packed struct {

    uint16_t  INT;              /*亮度<0-1000>*/
    uint8_t   FRQ;              /*<1-11>,11为Random*/
    uint16_t  CCT;              /*CCT，单位k*/
    uint8_t   GM;               /*<0-20>: 0->-1.0,1->-0.9,2->-0.8,   10->0,20->+1.0每隔0.1递增1，以此类推*/
    EffectTrigger_TypeDef      Trigger;          /*<0-2>,0-无触发/1-单次触发/2-多次触发*/
    uint8_t   SPD;              /*<1-11>,11为Random*/
} Lightning_Arg_TypeDef;

/*0x03：TV*/
typedef __packed struct {

    uint16_t  INT;              /*亮度<0-1000>*/
    uint8_t   FRQ;              /*<1-11>,11为Random*/
    uint16_t  CCT;              /*CCT，单位k*/
} TV_Arg_TypeDef;

/*0x04：Candle*/
typedef __packed struct {

    uint16_t  INT;              /*亮度<0-1000>*/
    uint8_t   FRQ;              /*<1-11>,11为Random*/
    uint16_t  CCT;              /*CCT，单位k*/
} Candle_Arg_TypeDef;

/*0x05：Fire*/
typedef __packed struct {

    uint16_t  INT;              /*亮度<0-1000>*/
    uint8_t   FRQ;              /*<1-11>,11为Random*/
    uint16_t  CCT;              /*CCT，单位k*/
} Fire_Arg_TypeDef;

/*0x06：Strobe*/
typedef __packed struct {

    EffectMode_TypeDef    EffectMode;   /*0x00:HIS；0x01:CCT；0x02:GEL*/
    uint8_t   FRQ;                      /*<1-11>,11为Random*/
    Mode_Arg_TypeDef    Mode_Arg;       /*Mode_Arg，具体模式参数*/
} Strobe_Arg_TypeDef;

/*0x07：Explosion*/
typedef __packed struct {

    EffectMode_TypeDef    EffectMode;   /*0x00:HIS；0x01:CCT；0x02:GEL*/
    uint8_t   FRQ;                      /*<1-11>,11为Random*/
    Mode_Arg_TypeDef    Mode_Arg;       /*Mode_Arg，具体模式参数*/
    EffectTrigger_TypeDef    Trigger;   /*<0-2>,0-无触发/1-单次触发/2-多次触发*/
} Explosion_Arg_TypeDef;

/*0x08：Fault_Bulb*/
typedef __packed struct {

    EffectMode_TypeDef    EffectMode;   /*0x00:HIS；0x01:CCT；0x02:GEL*/
    uint8_t   FRQ;                      /*<1-11>,11为Random*/
    Mode_Arg_TypeDef    Mode_Arg;       /*Mode_Arg，具体模式参数*/
    uint8_t   SPD;                      /*<1-11>,11为Random*/
} Fault_Bulb_Arg_TypeDef;

/*0x09：Pulsing*/
typedef __packed struct {

    EffectMode_TypeDef    EffectMode;   /*0x00:HIS；0x01:CCT；0x02:GEL*/
    uint8_t   FRQ;                      /*<1-11>,11为Random*/
    Mode_Arg_TypeDef Mode_Arg;          /*Mode_Arg，具体模式参数*/
    uint8_t   SPD;                      /*<1-11>,11为Random*/
} Pulsing_Arg_TypeDef;

/*0x0A：Welding*/
typedef __packed struct {

    EffectMode_TypeDef    EffectMode;   /*0x00:HIS；0x01:CCT；0x02:GEL*/
    uint8_t   FRQ;                      /*<1-11>,11为Random*/
    Mode_Arg_TypeDef Mode_Arg;          /*Mode_Arg，具体模式参数*/
    uint8_t   Min;
} Welding_Arg_TypeDef;

/*0x0B：Cop_Car*/
typedef __packed struct {

    uint16_t  INT;              /*亮度<0-1000>*/
    uint8_t   FRQ;              /*<1-11>,11为Random*/
    uint8_t   Color;            /*0x00：R；0x01：B；0x02：R+B；0x03：B+W*/
} Cop_Car_Arg_TypeDef;

/*0x0C：Color_Chase*/
typedef __packed struct {

    uint16_t  INT;              /*亮度<0-1000>*/
    uint8_t   FRQ;              /*<1-11>,11为Random*/
    uint8_t   SAT;              /*饱和度<0-100>*/
} Color_Chase_Arg_TypeDef;

/*0x0D：Party_Lights*/
typedef __packed struct {

    uint16_t  INT;              /*亮度<0-1000>*/
    uint8_t   FRQ;              /*<1-11>,11为Random*/
    uint8_t   SAT;              /*饱和度<0-100>*/
} Party_Lights_Arg_TypeDef;

/*0x0E：Fireworks*/
typedef __packed struct {

    uint16_t  INT;              /*亮度<0-1000>*/
    uint8_t   FRQ;              /*<1-11>,11为Random*/
    uint8_t   Type;             /*Type*/
} Fireworks_Arg_TypeDef;

/*0x10：I Am Here*/
typedef __packed struct {

    uint16_t  Run;              /*0x00:运行  0x01：停止并退出*/
} I_Am_Here_Arg_TypeDef;

/*0x11：ColorFade */
typedef __packed struct{
        
    uint16_t  INT;              /*亮度<0-1000>*/
    uint8_t   SPD;              /*<1-10>,*/
    uint8_t   Dir;              /*方向，0：向左，1：向右*/
    uint8_t   Color_CH[10];     /*最多支持10个像素*/
}ColorFade_Arg_TypeDef;

/*0x12：ColorCycle*/
typedef __packed struct{
        
    uint16_t  INT;              /*亮度<0-1000>*/
    uint8_t   SPD;              /*<1-10>,*/
    //uint8_t   Dir;            /*方向，0：向左，1：向右*/
    uint8_t  Color_CH[10];      /*最多支持10个像素*/
}ColorCycle_Arg_TypeDef;

/*0x13：ColorGradient*/
typedef __packed struct{
        
    uint16_t  INT;              /*亮度<0-1000>*/
    uint16_t  INT_Min;          /*亮度<0-1000>*/
    uint8_t   SPD;              /*<1-10>,*/
    //uint8_t   Dir;            /*方向，0：向左，1：向右*/
    uint8_t  Color_CH[10];      /*最多支持10个像素*/
}ColorGradient_Arg_TypeDef;

/*0x14：OneColorChase*/
typedef __packed struct{
        
    uint16_t  INT;              /*亮度<0-1000>*/
    uint8_t   SPD;              /*<1-10>,*/
    uint8_t   Dir;              /*方向，0：向左，1：向右*/
    uint8_t  Color_CH[10];      /*最多支持10个像素*/
}OneColorChase_Arg_TypeDef;

/*Effect_Arg定义*/
typedef __packed union {

    Club_Lights_Arg_TypeDef     Club_Lights_Arg;
    Paparazzi_Arg_TypeDef       Paparazzi_Arg;
    Lightning_Arg_TypeDef       Lightning_Arg;
    TV_Arg_TypeDef              TV_Arg;
    Candle_Arg_TypeDef          Candle_Arg;
    Fire_Arg_TypeDef            Fire_Arg;
    Strobe_Arg_TypeDef          Strobe_Arg;
    Explosion_Arg_TypeDef       Explosion_Arg;
    Fault_Bulb_Arg_TypeDef      Fault_Bulb_Arg;
    Pulsing_Arg_TypeDef         Pulsing_Arg;
    Welding_Arg_TypeDef         Welding_Arg;
    Cop_Car_Arg_TypeDef         Cop_Car_Arg;
    Color_Chase_Arg_TypeDef     Color_Chase_Arg;
    Party_Lights_Arg_TypeDef    Party_Lights_Arg;
    Fireworks_Arg_TypeDef       Fireworks_Arg;
    I_Am_Here_Arg_TypeDef       I_Am_Here_Arg;
    ColorFade_Arg_TypeDef       ColorFade_Arg;
    ColorCycle_Arg_TypeDef      ColorCycle_Arg;
    ColorGradient_Arg_TypeDef   ColorGradient_Arg;
    OneColorChase_Arg_TypeDef   OneColorChase_Arg;
} Effect_Arg_TypeDef;

typedef __packed struct {

    EffectType_TypeDef  Effect_Type;        /*光效类型*/
    Effect_Arg_TypeDef  Effect_Arg;         /*光效参数*/
} Light_Effect_Body_TypeDef;                 /*size: */


/*------Dimming_Curve：0x08------*/
typedef __packed struct {

    uint8_t   Curve;             /*变化曲线 <0-3>：0x00:线性,  0x01:指数,  0x02:对数,  0x03:S线*/
} Dimming_Curve_Body_TypeDef;     /*size: 1*/

/*--------Fan_Speed：0x09-------*/
typedef __packed struct {

    uint8_t   Mode;             /*0x00: Fixture; 0x01: Auto; 0x02: High Temp; 0x03: MAX; 0x04: OFF*/
    uint16_t  Fixture_Speed;    /*自定义风扇转速，<0-4096>*/
} Fan_Speed_Body_TypeDef;         /*size: 3*/

/*--------Get_Power：0x0A-------*/
typedef __packed struct {

    uint16_t  Extern_Voltage;       /*外部电源电压，单位为mv*/
    uint16_t  Battery_Voltage;      /*电池电压，单位为mv*/
    uint16_t  Battery_Power;        /*电池剩余电量，单位%*/
    uint16_t  Battery_Time;         /*电池剩余使用时间，单位分钟*/
} Get_Power_Body_TypeDef;         /*size: 4*/

/*------Board_State：0x0B-------*/
typedef __packed struct {

    uint16_t  Temperature;          /*驱动板温度*/
    uint16_t  Error_Info;           /*    错误信息
                                        Bit0:state
                                        Bit1:state
                                        …….
                                    State：0：None error
                                    State：1：Error
                                */
    uint16_t  Fan_Speed;        /*风扇转速，转/分钟*/
} Board_State_Body_TypeDef;         /*size: 6*/

/*--------Sleep_Mode：0x0C-------*/
typedef __packed struct {

    uint8_t  Mode;                 /*<0-1>, 0x00: 睡眠;   0x01: 唤醒 */
} Sleep_Mode_Body_TypeDef;         /*size: 1*/


/*----------PacketErr：0x0E-------*/
typedef __packed struct {

    ErrorNum_TypeDef  ErrorNum;   /*错误码*/
} PacketErr_Body_TypeDef;

/*----------PacketErr：0x0F-------*/
typedef __packed struct {

    uint16_t  INT;                  /*亮度<0-1000>*/
    uint8_t   Fan_Mode;             /*0x00: Fixture; 0x01: Auto; 0x02: High Temp; 0x03: MAX; 0x04: OFF*/
    uint16_t  Fan_Fixture_Speed;    /*自定义风扇转速，<0-4096>*/
} PacketAp129MainMode_Body_TypeDef;

/*----------Beat：0x10-------*/
typedef __packed struct {

    uint16_t  Sequence;                  /*帧序<0-65535>*/
	uint8_t	  ww_adaptive_dir;
	uint8_t   ww_adaptive_val;
	uint8_t   ww_adaptive_max_duty;
	uint8_t   ww_adaptive_min_duty;
	uint8_t	  cw_adaptive_dir;
	uint8_t   cw_adaptive_val;
	uint8_t   cw_adaptive_max_duty;
	uint8_t   cw_adaptive_min_duty;
} PacketBeat_Body_TypeDef;

/*----------Factory_RGBWW：0x11-------*/
typedef __packed struct {

    uint16_t  Red_PWM;          /*红灯pwm<0-65535>*/
    uint16_t  Green_PWM;        /*绿灯pwm<0-65535>*/
    uint16_t  Blue_PWM;         /*蓝灯pwm<0-65535>*/
    uint16_t  WW_PWM;           /*暖白pwm<0-65535>*/
    uint16_t  CW_PWM;           /*冷白pwm<0-65535>*/
} Factory_RGBWW_Body_TypeDef;

/*------Board_State：0x15-------*/
typedef __packed struct {

    uint16_t  Temperature;      /*控制板温度*/
    uint16_t  Error_Info;       /*    错误信息
                                        Bit0:state
                                        Bit1:state
                                        …….
                                    State：0：None error
                                    State：1：Error
                                */
} Control_State_Body_TypeDef;         /*size: 4*/

/*------PFX_Ctrl：0x16-------*/
typedef __packed struct {
    uint8_t Base;       /*0x00:CCT   0x01:HSI*/
    uint16_t Int;       /*0-1000*/ 
    uint16_t CCT;       /*5600对应5600K*/
    uint8_t GM;         /*<0-20>: -1.0->0,-0.9->1,-0.8->2,   +1.0->20每隔0.1递增1，以此类推*/  /*max: 31*/
    uint8_t Frq;        /*0.1-10Hz 0=0.1Hz …… 9=1Hz …… 19=10Hz*/
    uint16_t Times;     /*1-1000:1-1000次     大于1000=always*/
} PFX_Flash_Base_CCT_Type;

typedef __packed struct {
    uint8_t Base;       /*0:CCT 1:HSI*/ 
    uint16_t Int;       /*0-1000*/ 
    uint16_t Hue;       /*0-360*/
    uint8_t Sat;        /*0-100*/
    uint8_t Frq;        /*0.1-10Hz 0=0.1Hz …… 9=1Hz …… 19=10Hz*/
    uint16_t Times;     /*1-1000:1-1000次     大于1000=always*/
} PFX_Flash_Base_HSI_Type;

typedef __packed union {
    PFX_Flash_Base_CCT_Type BaseCCT;
    PFX_Flash_Base_HSI_Type BaseHSI;
} PFX_Flash_Arg_Type;

typedef __packed struct {
    uint8_t Base;           /*0x00:CCT   0x01:HSI*/
    uint16_t Int_Max;       /*0-1000*/
    uint16_t Int_Mini;      /*0-1000*/
    uint16_t CCT_Max;       /*5600对应5600K*/
    uint16_t CCT_Mini;      /*5600对应5600K*/
    uint8_t GM_Max;         /*<0-20>: -1.0->0,-0.9->1,-0.8->2,   +1.0->20每隔0.1递增1，以此类推*/
    uint8_t GM_Mini;        /*<0-20>: -1.0->0,-0.9->1,-0.8->2,   +1.0->20每隔0.1递增1，以此类推*/
    uint32_t Time;          /*<0-100000>  单位ms,最大100S,大于100S=Always*/
    uint8_t CCT_Seq;        /*0:正向    1:反向    2:往复    3:随机*/
    uint8_t Int_Seq;        /*0:正向    1:反向    2:往复    3:随机*/
    uint8_t GM_Seq;         /*0:正向    1:反向    2:往复    3:随机*/
    uint8_t loop;           /*0:一次   1:循环*/
} PFX_Chase_Base_CCT_Type;

typedef __packed struct {
    uint8_t Base;           /*0x00:CCT   0x01:HSI*/
    uint16_t Int_Max;       /*0-1000*/
    uint16_t Int_Mini;      /*0-1000*/
    uint16_t Hue_Max;       /*0-360*/
    uint16_t Hue_Mini;      /*0-360*/
    uint8_t Sat_Max;        /*0-100*/
    uint8_t Sat_Mini;       /*0-100*/
    uint32_t Time;          /*<0-100000>  单位ms,最大100S,大于100S=Always*/
    uint8_t Hue_Seq;        /*0:正向    1:反向    2:往复    3:随机*/
    uint8_t Int_Seq;        /*0:正向    1:反向    2:往复    3:随机*/
    uint8_t Sat_Seq;        /*0:正向    1:反向    2:往复    3:随机*/
    uint8_t loop;           /*0:一次   1:循环*/
} PFX_Chase_Base_HSI_Type;

typedef __packed union {
    PFX_Chase_Base_CCT_Type BaseCCT;
    PFX_Chase_Base_HSI_Type BaseHSI;
} PFX_Chase_Arg_Type;

typedef __packed struct {
    uint8_t Base;           /*0x00:CCT   0x01:HSI*/
    uint16_t Int;           /*0-1000*/
    uint16_t CCT;           /*5600对应5600K*/
    uint8_t GM;             /*<0-20>: -1.0->0,-0.9->1,-0.8->2,   +1.0->20每隔0.1递增1，以此类推*/
    uint32_t FadeInTime;    /*0-20000  单位ms,最大20S*/
    uint8_t FadeInCurve;    /*0:线性    1:指数    2:对数    3:s型*/
    uint32_t ContinueTime;  /*<0-100000>  单位ms,最大100S,大于100S=Always*/
    uint32_t FadeOutTime;   /*0-20000  单位ms,最大20S*/
    uint8_t FadeOutCurve;   /*0:线性    1:指数    2:对数    3:s型*/
} PFX_Continue_Base_CCT_Type;

typedef __packed struct {
    uint8_t Base;           /*0x00:CCT   0x01:HSI*/
    uint16_t Int;           /*0-1000*/
    uint16_t Hue;           /*0-360*/
    uint8_t Sat;            /*0-100*/
    uint32_t FadeInTime;    /*0-20000  单位ms,最大20S*/
    uint8_t FadeInCurve;    /*0:线性    1:指数    2:对数    3:s型*/
    uint32_t ContinueTime;  //*<0-100000>  单位ms,最大100S,大于100S=Always*/
    uint32_t FadeOutTime;   /*0-20000  单位ms,最大20S*/
    uint8_t FadeOutCurve;   /*0:线性    1:指数    2:对数    3:s型*/
} PFX_Continue_Base_HSI_Type;

typedef __packed union {
    PFX_Continue_Base_CCT_Type BaseCCT;
    PFX_Continue_Base_HSI_Type BaseHSI;
} PFX_Continue_Arg_Type;

typedef __packed union {
    PFX_Flash_Arg_Type Flash;
    PFX_Chase_Arg_Type Chase;
    PFX_Continue_Arg_Type Continue;
} PFX_Arg_Body_Type;

typedef __packed struct {

    uint8_t             PFX_Type;       /*光效类型, 0x00:PFX_Flash  0x01:PFX_ColorChase  0x02:PFX_Continue*/
    uint8_t             Light_Type;     /*灯具类型, 0x00:normal  0x01:tube  0x01:pixel mapping*/
    PFX_Arg_Body_Type  PFX_Arg;        /*光效参数*/
} PFX_Ctrl_Body_TypeDef;

/*----------CFX_Bank_RW：0x17-----------*/
typedef __packed struct {

    uint16_t Picker;            /*bit0-9: Bank1-Bank10*/
    uint16_t TouchBar;          /*bit0-9: Bank1-Bank10*/
    uint16_t Music;             /*bit0-9: Bank1-Bank10*/
} CFX_Bank_RW_Body_TypeDef;

/*----------CFX_Ctrl：0x18-----------*/
typedef __packed struct {

    uint8_t  Effect_Type;   /*自定义光效类型*/
    uint8_t  Bank;          /*<0-9>,Bank1-Bank10*/
    uint16_t Int;           /*0-1000*/
    uint8_t  Speed;         /*0=0.5X, 1=1X 2=2X, 3=4X 4=8X, 5=16X*/
    uint8_t  Chaos;         /*0%-100%*/
    uint8_t  Sequence;      /*0:正序 1:反序 2:往复*/
    uint8_t  Loop;          /*0:一次 1:循环*/
    uint8_t  Ctrl;          /*0:stop  1:init  2:running  3:Pause*/
    uint8_t  Init_Res;      /*Init结果，0成功，1失败，仅Init时候用*/
} CFX_Ctrl_Body_TypeDef;

/*----------CFX_Preview：0x19-----------*/
typedef __packed struct {
    uint8_t Base;       /*0x00:CCT   0x01:HSI*/
    uint16_t Int;       /*0-1000*/
    uint16_t CCT;       /*5600对应5600K*/
    uint8_t GM;         /*<0-20>: -1.0->0,-0.9->1,-0.8->2,   +1.0->20每隔0.1递增1，以此类推*/
} Preview_Base_CCT_Type;

typedef __packed struct {
    uint8_t Base;       /*0x00:CCT   0x01:HSI*/
    uint16_t Int;       /*0-1000*/
    uint16_t Hue;       /*0-360*/
    uint8_t Sat;        /*0-100*/
} Preview_Base_HSI_Type;

typedef __packed union {

    Preview_Base_CCT_Type Base_CCT;
    Preview_Base_HSI_Type Base_HSI;
} Preview_Frame_Arg_Type;

typedef __packed struct {

    Preview_Frame_Arg_Type Frame_1;
    Preview_Frame_Arg_Type Frame_2;
} CFX_Preview_Body_TypeDef;

/*----------MFX_Ctrl：0x1A-----------*/
typedef __packed struct {

    uint8_t Packet_Num;         /*0-3:Packet_0~Packet_3*/
    uint8_t Ctrl;               /*0:Stop  1:Restart  2:Pause  3:Continue*/
    uint8_t Base;               /*0:Base CCT    1:Base HSI*/
    uint16_t Int_Mini;          /*0-1000*/
    uint16_t Int_Max;           /*0-1000*/
    uint8_t Int_Seq;            /*0:正向    1:反向    2:往复    3:随机*/
    uint16_t CCT_Mini;
    uint16_t CCT_Max;
    uint8_t CCT_Seq;            /*0:正向    1:反向    2:往复    3:随机*/
    uint8_t GM_Mini;            /*<0-20>: 0->-1.0,1->-0.9,2->-0.8,   20->+1.0每隔0.1递增1，以此类推*/
    uint8_t GM_Max;             /*<0-20>: 0->-1.0,1->-0.9,2->-0.8,   20->+1.0每隔0.1递增1，以此类推*/
    uint8_t GM_Seq;             /*0:正向    1:反向    2:往复    3:随机*/
} MFX_Packet_0_BaseCCT;

typedef __packed struct {

    uint8_t Packet_Num;         /*0-3:Packet_0~Packet_3*/
    uint8_t Ctrl;               /*0:Stop  1:Restart  2:Pause  3:Continue*/
    uint8_t Base;               /*0:Base CCT    1:Base HSI*/
    uint16_t Int_Mini;          /*0-1000*/
    uint16_t Int_Max;           /*0-1000*/
    uint8_t Int_Seq;            /*0:正向    1:反向    2:往复    3:随机*/
    uint16_t Hue_Mini;
    uint16_t Hue_Max;
    uint8_t Hue_Seq;            /*0:正向    1:反向    2:往复    3:随机*/
    uint8_t Sat_Mini;           /*<0-20>: -1.0->0,-0.9->1,-0.8->2,   +1.0->20每隔0.1递增1，以此类推*/
    uint8_t Sat_Max;            /*<0-20>: -1.0->0,-0.9->1,-0.8->2,   +1.0->20每隔0.1递增1，以此类推*/
    uint8_t Sat_Seq;            /*0:正向    1:反向    2:往复    3:随机*/
} MFX_Packet_0_BaseHSI;

typedef __packed struct {

    uint8_t Packet_Num;         /*0-3:Packet_0~Packet_3*/
    uint8_t Ctrl;               /*0:Stop  1:Restart  2:Pause  3:Continue*/
    uint8_t Effect_Mode;        /*0:Flash  1:Continue  2:Paragraph*/
    uint8_t LoopMode;           /*0:一次    1:循环   2:随机*/
    uint8_t LoopTimes;          /*1-100*/
    uint32_t CycleTime_Mini;    /*<0-100000>  单位ms*/
    uint32_t CycleTime_Max;     /*<0-100000>  单位ms*/
    uint8_t CycleTime_Seq;      /*0:正向    1:反向    2:往复    3:随机*/
    uint32_t FreeTime_Mini;     /*<0-100000>  单位ms*/
    uint32_t FreeTime_Max;      /*<0-100000>  单位ms*/
    uint8_t FreeTime_Seq;       /*0:正向    1:反向    2:往复    3:随机*/
} MFX_Packet_1_Flash;

typedef __packed struct {

    uint8_t Packet_Num;         /*0-3:Packet_0~Packet_3*/
    uint8_t Ctrl;               /*0:Stop  1:Restart  2:Pause  3:Continue*/
    uint8_t Effect_Mode;        /*0:Flash  1:Continue  2:Paragraph*/
    uint8_t LoopMode;           /*0:一次    1:循环   2:随机*/
    uint8_t LoopTimes;          /*1-100*/
    uint32_t CycleTime_Mini;    /*<0-100000>  单位ms*/
    uint32_t CycleTime_Max;     /*<0-100000>  单位ms*/
    uint8_t CycleTime_Seq;      /*0:正向    1:反向    2:往复    3:随机*/
    uint32_t FadeInTime_Mini;   /*<0-100000>  单位ms*/
    uint32_t FadeInTime_Max;    /*<0-100000>  单位ms*/
    uint8_t FadeInTime_Seq;     /*0:正向    1:反向    2:往复    3:随机*/
    uint8_t FadeIn_Curve;       /*0:线性    1:指数    2:对数    3:S型*/
} MFX_Packet_1_Continue;

typedef __packed struct {

    uint8_t Packet_Num;         /*0-3:Packet_0~Packet_3*/
    uint8_t Ctrl;               /*0:Stop  1:Restart  2:Pause  3:Continue*/
    uint8_t Effect_Mode;        /*0:Flash  1:Continue  2:Paragraph*/
    uint8_t LoopMode;           /*0:一次    1:循环   2:随机*/
    uint8_t LoopTimes;          /*1-100*/
    uint32_t CycleTime_Mini;    /*<0-100000>  单位ms*/
    uint32_t CycleTime_Max;     /*<0-100000>  单位ms*/
    uint8_t CycleTime_Seq;      /*0:正向    1:反向    2:往复    3:随机*/
    uint8_t OLR_Mini;           /*<0-100> */
    uint8_t OLR_Max;            /*<0-100> */
    uint8_t OLR_Seq;            /*0:正向    1:反向    2:往复    3:随机*/
    uint8_t Overlap_Seq;        /*0:正向    1:反向    2:往复    3:随机*/
    uint8_t FreeTime_Seq;       /*0:正向    1:反向    2:往复    3:随机*/
    uint8_t UnitTime_Seq;       /*0:正向    1:反向    2:往复    3:随机*/
} MFX_Packet_1_Paragraph;

typedef __packed struct {

    uint8_t Packet_Num;         /*0-3:Packet_0~Packet_3*/
    uint8_t Ctrl;               /*0:Stop  1:Restart  2:Pause  3:Continue*/
    uint8_t Effect_Mode;        /*0:Flash  1:Continue  2:Paragraph*/
    uint32_t UnitTime_Mini;     /*<0-100000>  单位ms*/
    uint32_t UnitTime_Max;      /*<0-100000>  单位ms*/
    uint8_t UnitTime_Seq;       /*0:正向    1:反向    2:往复    3:随机*/
    uint8_t Frq_Mini;           /*1-129  <0.1-0.9Hz:1-9> <1-120Hz:10-129>*/
    uint8_t Frq_Max;            /*1-129  <0.1-0.9Hz:1-9> <1-120Hz:10-129>*/
    uint8_t Frq_Seq;            /*1-129     <1-9:0.1-0.9Hz> <10-129:1-120Hz>*/
} MFX_Packet_2_Flash;

typedef __packed struct {

    uint8_t Packet_Num;         /*0-3:Packet_0~Packet_3*/
    uint8_t Ctrl;               /*0:Stop  1:Restart  2:Pause  3:Continue*/
    uint8_t Effect_Mode;        /*0:Flash  1:Continue  2:Paragraph*/
    uint32_t FadeOutTime_Mini;  /*<0-100000>  单位ms*/
    uint32_t FadeOutTime_Max;   /*<0-100000>  单位ms*/
    uint8_t FadeOutTime_Seq;    /*0:正向    1:反向    2:往复    3:随机*/
    uint8_t FadeOut_Curve;      /*0:线性    1:指数    2:对数    3:S型*/
    uint8_t Flicker_Frq;        /*1-129     <1-9:0.1-0.9Hz> <10-129:1-120Hz>*/
} MFX_Packet_2_Continue;

typedef __packed struct {

    uint8_t Packet_Num;         /*0-3:Packet_0~Packet_3*/
    uint8_t Ctrl;               /*0:Stop  1:Restart  2:Pause  3:Continue*/
    uint8_t Effect_Mode;        /*0:Flash  1:Continue  2:Paragraph*/
    uint32_t FreeTime_Mini;     /*<0-100000>  单位ms*/
    uint32_t FreeTime_Max;      /*<0-100000>  单位ms*/
    uint32_t UnitTime_Mini;     /*<0-100000>  单位ms*/
    uint32_t UnitTime_Max;      /*<0-100000>  单位ms*/
    uint8_t OLP_Mini;           /*0-100*/
    uint8_t OLP_Max;            /*0-100*/
    uint8_t OLP_Seq;            /*0:正向    1:反向    2:往复    3:随机*/
} MFX_Packet_2_Paragraph;

typedef __packed struct {

    uint8_t Packet_Num;         /*0-3:Packet_0~Packet_3*/
    uint8_t Ctrl;               /*0:Stop  1:Restart  2:Pause  3:Continue*/
    uint8_t Effect_Mode;        /*0:Flash  1:Continue  2:Paragraph*/
    uint32_t FadeInTime_Mini;   /*<0-100000>  单位ms*/
    uint32_t FadeInTime_Max;    /*<0-100000>  单位ms*/
    uint8_t FadeInTime_Seq;     /*0:正向    1:反向    2:往复    3:随机*/
    uint8_t FadeIn_Curve;       /*0:线性    1:指数    2:对数    3:S型*/
    uint32_t FadeOutTime_Mini;  /*<0-100000>  单位ms*/
    uint32_t FadeOutTime_Max;   /*<0-100000>  单位ms*/
    uint8_t FadeOutTime_Seq;    /*0:正向    1:反向    2:往复    3:随机*/
    uint8_t FadeOut_Curve;      /*0:线性    1:指数    2:对数    3:S型*/
    uint8_t Flicker_Frq;        /*1-129     <1-9:0.1-0.9Hz> <10-129:1-120Hz>*/
} MFX_Packet_3_Paragraph;

typedef __packed union {

    MFX_Packet_0_BaseCCT BaseCCT;
    MFX_Packet_0_BaseHSI BaseHSI;
} MFX_Packet_0;

typedef __packed union {

    MFX_Packet_1_Flash       Flash;
    MFX_Packet_1_Continue    Continue;
    MFX_Packet_1_Paragraph   Paragraph;
} MFX_Packet_1;

typedef __packed union {

    MFX_Packet_2_Flash       Flash;
    MFX_Packet_2_Continue    Continue;
    MFX_Packet_2_Paragraph   Paragraph;
} MFX_Packet_2;

typedef __packed union {

    MFX_Packet_3_Paragraph   Paragraph;
} MFX_Packet_3;

typedef __packed union {
    MFX_Packet_0 Packet_0;
    MFX_Packet_1 Packet_1;
    MFX_Packet_2 Packet_2;
    MFX_Packet_3 Packet_3;
} MFX_Ctrl_Body_TypeDef;

/*----------RGBWW_1：0x1C---------*/
typedef __packed struct {

    uint16_t  Red_INT;          /*红灯亮度<0-1000>*/
    uint16_t  Green_INT;        /*绿灯亮度<0-1000>*/
    uint16_t  Blue_INT;         /*蓝灯亮度<0-1000>*/
    uint16_t  WW_INT;           /*暖白亮度<0-1000>*/
    uint16_t  CW_INT;           /*冷白亮度<0-1000>*/
    uint16_t  Fade_Time;        /*缓变时间（ms），0表示缓变时间为0，表示瞬变。缓变时间默认350*/
    uint16_t  Channel;          /*点控通道，0表示全控，n表示通道n，默认0*/
} RGBWW_1_Body_TypeDef;    

/*----------CFX_Name：0x1D---------*/
typedef __packed struct {

    uint8_t   Effect_Type;      /*光效类型  0-Picker  1-Touchbar  2-Music*/
    uint8_t   Bank;             /*光效Bank  0-9:Bank1-10*/
    uint16_t  Code_Type;        /*编码方式 0：Ascii码*/
    char      Name[10];         /*文件名，Ascii码*/
} CFX_Name_Body_TypeDef;  

/*----------FactoryCCT：0xFE-----------*/
typedef __packed struct {

    uint16_t  INT;              /*亮度<0-1000>*/
    uint16_t  CCT;              /*CCT，单位k*/
    uint8_t   GM;               /*<0-20>: 0->-1.0,1->-0.9,2->-0.8,   10->0,20->+1.0每隔0.1递增1，以此类推*/
    uint16_t  W_Ratio;          /*最大白光配比的比例,除以1000*/
} Factory_CCT_Body_TypeDef;                 /*size: 5*/

/*----------ProdTest：0xFF 总长26字节-------*/
/*----------目前只用于AP129-------*/
typedef __packed struct {

    uint16_t  InputVolt;                /*输入电压 单位Mv*/
    uint8_t   Temperture;               //温度
    uint16_t  Fan1State;                //风扇1状态
    uint16_t  Fan2State;                //风扇2状态
    uint16_t  Fan1Speed;                //风扇1转速
    uint16_t  Fan2Speed;                //风扇2转速
    uint8_t   Reserve[15];              //预留
} PacketProdTestAck_Body_TypeDef;

typedef __packed union {

    Version_Body_TypeDef                Version_Body;
    HSI_Body_TypeDef                    HSI_Body;
    CCT_Body_TypeDef                    CCT_Body;
    Gel_Body_TypeDef                    Gel_Body;
    RGBWW_Body_TypeDef                  RGBWW_Body;
    XY_Coordinate_Body_TypeDef          XY_Coordinate_Body;
    LED_Frequency_Body_TypeDef          LED_Frequency_Body;
    Light_Effect_Body_TypeDef           Light_Effect_Body;
    Dimming_Curve_Body_TypeDef          Dimming_Curve_Body;
    Fan_Speed_Body_TypeDef              Fan_Speed_Body;
    Get_Power_Body_TypeDef              Get_Power_Body;
    Board_State_Body_TypeDef            Board_State_Body;
    Sleep_Mode_Body_TypeDef             Sleep_Mode_Body;
    PacketErr_Body_TypeDef              PacketErr_Body;
    PacketAp129MainMode_Body_TypeDef    PacketAp129MainMode_Body;
    PacketBeat_Body_TypeDef             PacketBeat_Body;
    Factory_RGBWW_Body_TypeDef          Factory_RGBWW_Body;
    Source_Body_TypeDef                 Source_Body;
    Factory_CCT_Body_TypeDef            Factory_CCT_Body;
    IlluminationMode_Body_TypeDef       IlluminationMode_Body;
    ColorMixing_Body_TypeDef            ColorMixing_Body;
    Control_State_Body_TypeDef          Control_State_Body;
    PFX_Ctrl_Body_TypeDef               PFX_Ctrl_Body;
    CFX_Bank_RW_Body_TypeDef            CFX_Bank_RW_Body;
    CFX_Ctrl_Body_TypeDef               CFX_Ctrl_Body;
    CFX_Preview_Body_TypeDef            CFX_Preview_Body;
    MFX_Ctrl_Body_TypeDef               MFX_Ctrl_Body;
    RGBWW_1_Body_TypeDef                RGBWW_1_Body;
    CFX_Name_Body_TypeDef               CFX_Name_Body;
    uint8_t   DefaultBody_Buffer[CommandBody_Max];   //定义默认CommandBody长度，CommandBody_Max
} CommandBody_TypeDef;

/*------------------------------------- Packet ----------------------------------------------*/
/*通用数据包*/
typedef __packed struct {

    Header_TypeDef          Header;            /* 通信数据包包头*/
    CommandBody_TypeDef     CommandBody;    /*通信数据包数据部分*/
    uint8_t                 CheckSum;        /*数据校验和*/
} Packet_TypeDef;

/****************************************升级数据包******************************************/
typedef enum {

    CODE_NONE = 0,
    CODE_CMD,
    CODE_LEN,
    CODE_DATA,
    CODE_VERIFY,
    CODE_STATUS
} Upgrade_OperCode;

typedef enum {

    CMD_NONE = 0,
    CMD_START,
    CMD_ABORT
} Upgrad_Cmd_Type;

typedef enum {

    STATUS_OK = 0,
    STATUS_UNSUPPORTED_COMMAND,
    STATUS_ILLEGAL_STATE,
    STATUS_VERIFICATION_FAILED,
    STATUS_INVALID_IMAGE,
    STATUS_INVALID_IMAGE_SIZE,
    STATUS_MORE_DATA,
    STATUS_INVALID_APPID,
    STATUS_INVALID_VERSION,
    STATUS_CONTINUE
} Upgrad_STATUS_Type;

typedef __packed struct {

    uint8_t len;
    uint32_t seq_number;
    uint8_t data[128];
} Upgrad_Data_Proto;

typedef __packed union {

    Upgrad_Cmd_Type     cmd;
    uint32_t            length;
    Upgrad_Data_Proto   data;
    uint32_t            crc32;
    //Upgrad_STATUS_Type status;
} Upgrad_Body_Arg;

/*----------Upgrade：0x0D-------*/
typedef __packed struct {

    Upgrade_OperCode    code;
    Upgrad_Body_Arg     arg;
} Upgrade_Body_TypeDef;

typedef __packed struct {

    Header_TypeDef          Header;            /*通信数据包包头*/
    Upgrade_Body_TypeDef    CommandBody;    /*通信数据包数据部分*/
    uint8_t                 CheckSum;        /*数据校验和*/
} Upgrade_Packet_TypeDef;

/*升级数据应答包*/
typedef __packed struct {

    Header_TypeDef          Header;         /*通信数据包包头*/
    Upgrad_STATUS_Type      status;
    uint8_t                 CheckSum;       /*数据校验和*/
} Upgrade_Ack_TypeDef;

/****************************************File Transfre******************************************/
typedef enum {

    FILE_TX_CODE_NONE = 0,
    FILE_TX_CODE_START,
    FILE_TX_CODE_LEN,
    FILE_TX_CODE_DATA,
    FILE_TX_CODE_VERIFY,
    FILE_TX_CODE_STATUS
} FileTransferCode;

typedef enum {

    FILE_TX_STATUS_OK = 0,
    FILE_TX_STATUS_UNSUPPORTED_COMMAND,
    FILE_TX_STATUS_ILLEGAL_STATE,
    FILE_TX_STATUS_VERIFICATION_FAILED,
    FILE_TX_STATUS_INVALID_IMAGE,
    FILE_TX_STATUS_INVALID_IMAGE_SIZE,
    FILE_TX_STATUS_MORE_DATA,
    FILE_TX_STATUS_INVALID_APPID,
    FILE_TX_STATUS_INVALID_VERSION,
    FILE_TX_STATUS_CONTINUE
} FileTransferStatus;

typedef enum {
    FileType_CFX = 0,
    FileType_Null,
} FileTransfer_FileType;

typedef __packed struct {

    uint8_t EffectType;
    uint8_t Bank;
} CFXStartBody_Pack;

typedef __packed union {

    CFXStartBody_Pack CFXStartBody;
} StartBody_Pack;

typedef __packed struct {
    FileTransfer_FileType FileType;
    StartBody_Pack StartBody;
} FileTransferStartBody_Pack;

typedef __packed struct {

    uint8_t     Len;
    uint32_t    Sequence;
    uint8_t     Buffer[128];
} FileTransferDataBody_Pack;

typedef __packed union {

    FileTransferStartBody_Pack  Start;
    uint32_t                    FileLen;
    FileTransferDataBody_Pack   Data;
    uint32_t                    CRC32;
} FileTransfer_Pack;

/*----------FileTransfer：0x1B-------*/
typedef __packed struct {

    FileTransferCode    Code;
    FileTransfer_Pack   Arg;
} FileTransfer_Body_TypeDef;

/*文件传输数据包，主机发送*/
typedef __packed struct {

    Header_TypeDef              Header;            /*通信数据包包头*/
    FileTransfer_Body_TypeDef   CommandBody;    /*通信数据包数据部分*/
    uint8_t                     CheckSum;        /*数据校验和*/
} FileTransfer_Packet_TypeDef;

/*文件传输应答包，从机发送*/
typedef __packed struct {

    Header_TypeDef          Header;         /*通信数据包包头*/
    FileTransferStatus      Status;
    uint8_t                 CheckSum;       /*数据校验和*/
} FileTransfer_Ack_TypeDef;


/****************************************目前只用于AP129************************************************/
/*产测试数据包*/
typedef __packed struct {
    Header_TypeDef            Header;            /*通信数据包包头*/
    uint8_t                 CheckSum;        /*数据校验和*/
} PacketProdTest_TypeDef;
/*产测试数据应答包*/
typedef __packed struct {
    Header_TypeDef            Header;         /*通信数据包包头*/
    PacketProdTestAck_Body_TypeDef ProdTestAck_Body; //
    uint8_t                 CheckSum;       /*数据校验和*/
} PacketProdTestAck_TypeDef;
#endif

#endif  /*__USER_PROTO_H*/

