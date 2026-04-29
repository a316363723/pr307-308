/*********************************************************************************
  *Copyright(C), 2021, Aputure, All rights reserver.
  *FileName:    user_bluetooth_proto.h
  *Author:         
  *Version:     v3.9.2
  *Date:        2021/08/13
  *Description: APP与MCU通信的蓝牙数据包格式
                v1.0：2018/11/07,Steven
                v1.1：2019/01/11,Steven,对应v1.3版本蓝牙通信协议
                v1.2：2019/02/22,Steven,对应v1.4版本蓝牙通信协议，将通信包长度修改为10byte
                v1.3：2019/03/09,Steven,对应v1.5版本蓝牙通信协议，将GET_VER中Product由16bit改为32bit，
                      将其余版本说明改为6bit，添加BT_Product_TypeDef结构体对产品类型进行说明
                v1.4：2019/03/25,Steven,对应v1.6版本蓝牙通信协议，添加电池电量剩余使用时间,添加获取当前界面信息命令
                      将产品类型数据结构嵌入到BT_Version_Body_TypeDef中，添加Main_Interface协议
                v1.5：2019/04/30,Steven,对应v1.7版本蓝牙通信协议，在光模式协议包最后添加开关机状态
                v1.6: 2019/05/08,Steven,对应v1.8版本蓝牙通信协议，细分Get_Power中的Battery_Power，细分GEL_CMD中的Gel Type，
                      细分EFFECT_CMD中的Strobe/Explosion/Fault_Bulb/Pulsing/Welding这5个光效中的Gel Type
                v1.7: 2019年5月9日，Jagger,对应v1.9版本蓝牙协议，增加了整体照度设置协议Light_Bright；
                v1.8: 2019/08/03,Steven,对应v2.3版本蓝牙通信协议
                V3.0: 2019/11/20,Matthew,对应V3.0版本协议
                V3.4: 2020/01/16,Matthew,对应V3.4版本协议
                V3.5: 2020/02/29,Jagger,对应V3.5版本协议，增加一个APP控制类型，目前此类型值为0，对应控制蓝牙重置
                V3.6: 2020/03/04,Matthew,对应V3.6版本协议，新增CustomEfect_Preview命令0x1E，用与预览光效，包含两个帧的参数
                V3.7  2020/05/06,Matthew,对应V3.7版本协议，新增BT_CommandType_MFX命令0x1F;原Program Flash的Time参数更改为Times;
                      新增BLUETOOTH_PROTO_VER协议版本号宏定义
                V3.8: 2021/01/25,Matthew,对应V3.8.3版本协议，BT_CommandType_Fan_Speed命令内容修改
                V3.9.1  2021/04/20,Matthew,对应V3.9.1版本协议，新增BT_CommandType_CFX_Name命令，用于CFX文件名操作
                      定位光效I_Am_Here新增Ctrl控制位，用于主动终止定位光效，Version_Body中新增Upgrade_Type区分OTA支持类型
                V3.9.2  2021/08/13,Matthew,对应V3.9.2版本协议，版本信息中新增GATT协议版本区分
                V3.9.3  2022/3/23,Jagger
                        1.优化新增的像素光效（点控光效，11种必选和19种可选灯带光效）；
                        2.优化新增的2代常规光效，共19种；
                        3.优化新增的分区控制协议；
                V3.9.3  2022/4/19,Jagger
                        1.新增了获取二代系统光效和点控光效状态的协议；
                V3.9.4  2022/4/29,Jagger
                        1、点控光效协议变更
                           --颜色切换、颜色替换、单色移动、双色移动、三色移动、点控火焰、多色移动光效的状态更改 停止/运行 -> 停止/暂停/运行；
                           --颜色切换、单色移动、双色移动、三色移动和多色移动的速度范围已经确定，采用绝对速度 1cm/s - 80cm/s;
                           --颜色切换光效的方向参数定义具体化；
                           --点控火焰光效的速度字段改为频率，增加火焰方向字段，有横向火焰和纵向火焰；
                           --删除颜色渐变，后面的光效序号依次向前平移；
                        2、删除TV III和Fire III光效的亮度下限；
                        3、点控光效的颜色切换和颜色替换光效的像素数量由2-4改为2-10；
                        4、单色移动、双色移动、三色移动和多色移动光效，去掉像素单元的的光柱长度字段，在外层结构体中增加像素长度字段，长度范围S/M/L，具体长度根据具体产品而定；
                        5、彩虹光效的速度由 0.1s - 30秒改为1cm/s - 640cm/s；
                V3.9.5  2022/5/13,Jagger
                        1、将烛光2，火焰2（协议大改），TV3（协议大改），火焰3（协议大改），脉搏3（协议大改）五个光效的亮度下限取消；
                        2、将点控火焰底色的光柱长度去掉；
                        3、点控光效颜色轮转增加一个方向；
                        4、点控光效的灯带光效中所有底色参数增加一个选项，即511对应的是没有底色；
                 V4.0.0  2022/5/26,Jagger
                        1、二代系统光效的参数结构体命名不全；
                        2、二代系统光效的PaparazziII 、LightningIII、TVIII、FireworksII、Faulty BulbII的间隔时间参数由一个间隔时间改为上限间隔时间和下限间隔时间；
                        3、二代系统光效的TVIII、FireIII的光模式改为CCT_RANGE和HSI_RANGE;
                        4、分区光效协议增加默认值；
                        5、蓝牙协议的RGB增加总亮度；
                        6、GET_VER2协议分区像素参数变更；
                        7、制作协议目录；
                        8、将协议版本号升级为4.0；
                 V4.0.1  2022/6/22,Jagger
                        1、去掉二代系统光效中的Candle_II光效；
                        2、get_version2协议中增加一套XY值；
                        3、二代光效和点控光效中的多数据包光效的状态增加一个continue状态；
                 V4.0.2  2022/06/23,Jagger
                        1、在get_version2协议中增加点控光效支持最大像素数量，在不同产品部分点控光效支持最大像素数量会有不同的情况，但同一产品这些光效支持的最大像素数量一样；
**********************************************************************************/

#ifndef __BT_PROTO_H
#define __BT_PROTO_H
#include "stdint.h"

#define BLUETOOTH_PROTO_VER     (40)

#ifdef     __GNUC__
#define     PACKED     __attribute__((packed))

#elif defined ( __CC_ARM )
#define PACKED __packed

#else
#error "We do not know how your compiler packs structures"
#endif

#define LITTLE_ENDIAN       (0)       /*小端模式*/
#define BIG_ENDIAN          (1)       /*大端模式*/


#ifndef BYTE_ORDER
#define BYTE_ORDER          LITTLE_ENDIAN
#endif

#define BT_PACKET_SIZE      (10)

#define DEFINE_STRUCT       PACKED struct
#define TYPEDEFINE_STRUCT   typedef PACKED struct
#define DEFINE_BODY(s)      s ## _Body_TypeDef

#define TYPEDEFINE_ENUM     typedef enum

#define DEFINE_EFFECT_ARG(s)    s ## _Arg_TypeDef

/*---命令类型---*/
typedef enum{     
    BT_CommandType_Version = 0,
    BT_CommandType_HSI,
    BT_CommandType_CCT,
    BT_CommandType_GEL,
    BT_CommandType_RGBWW,
    BT_CommandType_XY_Coordinate,
    BT_CommandType_LED_Frequency,
    BT_CommandType_Light_Effect,
    BT_CommandType_Dimming_Curve,
    BT_CommandType_Fan_Speed,
    BT_CommandType_Get_Power,
    BT_CommandType_Board_State,
    BT_CommandType_Sleep_Mode,
    BT_CommandType_Main_Interface,
    BT_CommandType_Light_Mode,
    BT_CommandType_Light_Bright,
    BT_CommandType_Bridge_Effects,  //0x10
    BT_CommandType_Bridge_Cmd,
    BT_CommandType_Bridge_BankStatus,
    BT_CommandType_PFX_Flash,
    BT_CommandType_PFX_Chase,
    BT_CommandType_PFX_Continue,
    BT_CommandType_Bridge_SleepMode,
    BT_CommandType_Bridge_LightBright,
    BT_CommandType_Bridge_FixtureSet,
    BT_CommandType_IOT_Cmd,
    BT_CommandType_CFX_Bank_RW,
    BT_CommandType_Factory_Reset,
    BT_CommandType_CFX_Ctrl,
    BT_CommandType_APP_Ctrl,
    BT_CommandType_CFX_Preview,
    BT_CommandType_MFX,
    BT_CommandType_CFX_Name,        //0x20
	BT_CommandType_Pixel_Fx,
	BT_CommandType_Light_Effect_II,
	BT_CommandType_Partition_Para,
    BT_CommandType_Partition_Fx,
    BT_CommandType_Version2,
    BT_CommandType_Partition_Cfg,    
	BT_CommandType_Emotion_Cfg = 0X2B,
    BT_CommandType_Emotion_Live = 0X2C,
	BT_CommandType_High_speed = 0x35,
	BT_CommandType_READ_SN = 0x3A,
    BT_CommandType_NULL = 0x7F,     //Max=0x7F
}BT_CommandType_TypeDef;

/*---光效类型---*/
typedef enum{     
     BT_EffectType_ClubLights = 0,
     BT_EffectType_Paparazzi,
     BT_EffectType_Lightning,
     BT_EffectType_TV,
     BT_EffectType_Candle,
     BT_EffectType_Fire,
     BT_EffectType_Strobe,
     BT_EffectType_Explosion,
     BT_EffectType_FaultBulb,
     BT_EffectType_Pulsing,
     BT_EffectType_Welding,
     BT_EffectType_CopCar,
     BT_EffectType_ColorChase,
     BT_EffectType_PartyLights,
     BT_EffectType_Fireworks,
     BT_EffectType_Effect_Off,
     BT_EffectType_I_Am_Here,
     BT_EffectType_NULL,
}BT_EffectType_TypeDef;

/*---光效子类型---*/
typedef enum{
     BT_EffectMode_CCT = 0,
     BT_EffectMode_HSI,
     BT_EffectMode_GEL,
     BT_EffectMode_XY,
	 BT_EffectMode_SOURCE,
     BT_EffectMode_NULL,
}BT_Effect_Mode_TypeDef;

typedef __packed struct{     
     uint8_t Command_Type       :7;       /*0-127*/ /*max: 127*/
     uint8_t Opera_Type         :1;       /*0-1*/ /*master: read:0  write:1; 
                                                    slave:  ack:0;   nack:1; */
}BT_Header_TypeDef; /*size: 1*/

/*----------Version：0x00----------*/
typedef __packed struct{    
    #if (BYTE_ORDER == LITTLE_ENDIAN)
    uint64_t GATT_Ver           :1;     /*0:GATT协议1.x版本  1:GATT协议2.0及以上版本*/
    uint64_t Upgrade_Type       :1;     /*升级类型 0：OTA 1：GATT*/
    uint64_t Driver_SwVer       :6;     /*11:代表版本v1.1*/
    uint64_t Driver_HwVer       :6;     /*11:代表版本v1.1*/
    uint64_t Control_SwVer      :6;     /*11:代表版本v1.1*/
    uint64_t Control_HwVer      :6;     /*11:代表版本v1.1*/
    uint64_t CFX_Music_Support  :1;     /*支持CFX_Music*/
    uint64_t CFX_Touchbar_Support:1;    /*支持CFX_Touchbar*/
    uint64_t CFX_Picker_Support :1;     /*支持CFX_Picker*/
    uint64_t PFX_Support        :1;     /*支持PFX*/
    uint64_t MFX_Support        :1;     /*支持MFX*/
    uint64_t Product_Machine    :4;     /*0：无/SFL  1：变焦/VFL  2：变焦+航向+俯仰/TA*/
    uint64_t Product_CCTRange   :7;     /*色温上限*/
    uint64_t Product_CCTRange_L :7;     /*色温下限*/
    uint64_t Product_LedType    :5;     /*0：D; 1：T; 2：WW；3:WWG; 4：RGB; 5：WRGB; 6：WWRGB; 7:WWRGBCMYP*/
    uint64_t Product_Function   :4;     /*0:单色温；1：双色温；2：简单彩色；3：全功能彩色*/
    uint64_t Protocol_Ver       :6;     /*11:代表版本v1.1*/     
    #else
    #endif
}BT_Version_Body_TypeDef;/*size: 8*/

/*----------HSI：0x01------------*/
typedef __packed struct{    
    #if (BYTE_ORDER == LITTLE_ENDIAN)
    uint64_t Sleep_Mode :1;         /*0x00:睡眠   0x01:唤醒*/
    uint64_t Reserve    :37;        /*预留*/
    uint64_t Sat        :7;         /*0-100*/     /*max: 127*/
    uint64_t Hue        :9;         /*0-360*/     /*max: 511*/
    uint64_t Int        :10;        /*0-1000*/  /*max: 1023*/
    #else
    #endif
}BT_HSI_Body_TypeDef;/*size: 8*/

/*----------CCT：0x02-----------*/
typedef __packed struct{     
    #if (BYTE_ORDER == LITTLE_ENDIAN)
    uint64_t Sleep_Mode :1;         /*0x00:睡眠   0x01:唤醒*/
    uint64_t Reserve    :36;        /*预留*/
    uint64_t GM         :7;         /*<0-20>: 0->-1.0,1->-0.9,2->-0.8,   20->+1.0每隔0.1递增1，以此类推*/  /*max: 31*/
    uint64_t CCT        :10;        /*CCT乘以10，<2800-10000> 280->2800K,290->2900K, 300->3000K,依次类推*/     /*max: 1023*/
    uint64_t Int        :10;        /*0-1000*/  /*max: 1023*/
    #else 
    #endif
}BT_CCT_Body_TypeDef;/*size: 8*/

/*----------GEL：0x03-----------*/
typedef __packed struct{
    #if (BYTE_ORDER == LITTLE_ENDIAN)
    uint64_t Sleep_Mode :1;         /*0x00:睡眠   0x01:唤醒*/
    uint64_t Reserve    :28;        /*预留*/
    uint64_t Color      :10;        /*0 - 15*/
    uint64_t Type       :4;         /*0-1023*/
    uint64_t Origin     :1;         /*品牌(0->LEE)(1->Resco)*/
    uint64_t CCT        :10;        /*CCT乘以10，<2800-10000> 280->2800K,290->2900K, 300->3000K,依次类推*/     /*max: 1023*/
    uint64_t Int        :10;        /*0-1000*/
    #else
    #endif
}BT_Gel_Body_TypeDef;/*size: 8*/

/*----------RGBW：0x04-----------*/
typedef __packed struct{
    #if (BYTE_ORDER == LITTLE_ENDIAN)
    uint64_t Sleep_Mode :1;         /*0x00:睡眠   0x01:唤醒*/
    uint64_t Reserve    :3;         /*预留*/
    uint64_t Int        :10;        /*Red   0-1000*/        /*max: 1023*/
    uint64_t CW_Int     :10;        /*Cool white 0-1000*/   /*max: 1023*/
    uint64_t WW_Int     :10;        /*Warm white 0-1000*/   /*max: 1023*/
    uint64_t B_Int      :10;        /*Blue  0-1000*/        /*max: 1023*/
    uint64_t G_Int      :10;        /*Green 0-1000*/        /*max: 1023*/
    uint64_t R_Int      :10;        /*Red   0-1000*/        /*max: 1023*/
    #else
    #endif
}BT_RGBWW_Body_TypeDef;

/*------XY_Coordinate：0x05-----*/
typedef __packed struct{
    #if (BYTE_ORDER == LITTLE_ENDIAN)
    uint64_t Sleep_Mode     :1;     /*0x00:睡眠   0x01:唤醒*/
    uint64_t Reserve        :25;    /*预留*/
    uint64_t Coordinate_Y   :14;    /*max: 16384*/
    uint64_t Coordinate_X   :14;    /*max: 16384*/
    uint64_t Int            :10;    /*Warm white 0-1000*/       /*max: 1023*/
    #else
    #endif
}BT_XY_Coordinate_Body_TypeDef;

/*-----LED_Frequency：0x06------*/
typedef __packed struct{
    #if (BYTE_ORDER == LITTLE_ENDIAN)
    uint64_t Reserve        :56;    /*预留*/
    uint64_t Frequency      :8;     /*max: 255*/
    #else
    #endif
}BT_LED_Frequency_Body_TypeDef;

/*------Dimming_Curve：0x08------*/
typedef __packed struct{
    #if (BYTE_ORDER == LITTLE_ENDIAN)
    uint64_t Reserve        :56;    /*预留*/
    uint64_t Curve          :8;     /*<0-3> 0x00:线性,0x01:指数,0x02:对数,0x03:S线*/
    #else
    #endif
}BT_Dimming_Curve_Body_TypeDef;

/*--------Fan_Speed：0x09-------*/
typedef __packed struct{
    #if (BYTE_ORDER == LITTLE_ENDIAN)
    uint64_t Reserve        :20;    /*预留*/
    uint64_t Silent_Use     :1;     /*支持静音模式*/
    uint64_t Low_Use        :1;     /*支持低速模式*/
    uint64_t Medium_Use     :1;     /*支持中速模式*/
    uint64_t High_Use       :1;     /*支持高速模式*/
    uint64_t Off_Use        :1;     /*支持关风扇模式*/
    uint64_t Max_Use        :1;     /*支持全速模式*/
    uint64_t Smart_Use      :1;     /*支持自动模式*/
    uint64_t Manual_Use     :1;     /*支持手动模式*/
    uint64_t High_Temp      :4;     /*0x00=正常；0x01=高温*/
    uint64_t Current_Temp   :8;     /*0x00-7F；0x80-FF <0~127；-128~-1> 0x80（-128）=不支持溫度讀取*/
    uint64_t Fixture_Speed  :16;    /*<0-4096> 自定义风扇转速*/
    uint64_t Mode           :8;     /*<0-7> 0x00:Manual 0x01:Smart 0x02:MAX 0x03:OFF 0x04:High 0x05:Medium 0x06:Low 0x07:Silent*/
    #else
    #endif
}BT_Fan_Speed_Body_TypeDef;

/*--------Get_Power：0x0A-------*/
typedef __packed struct{
    #if (BYTE_ORDER == LITTLE_ENDIAN)
    uint64_t Reserve        :16;    /*预留*/
    uint64_t Battery_time   :9;     /*电池剩余使用时间，单位分钟*/
    uint64_t Battery_Power  :7;     /*电池剩余电量，单位%*/
    uint64_t Battery_Voltage:16;    /*电池电压，单位为mv*/
    uint64_t Extern_Voltage :16;    /*外部电源电压，单位为mv*/
    #else
    #endif
}BT_Get_Power_Body_TypeDef;

/*------Board_State：0x0B-------*/
typedef __packed struct{
    #if (BYTE_ORDER == LITTLE_ENDIAN)
    uint64_t Reserve        :32;    /*预留*/
    uint64_t Error_Info     :16;    /*错误信息，Bit1:state…….  State 0：None error  State 1：Error */
    uint64_t Driver_Temp    :8;     /*,驱动板温度*/
    uint64_t Control_Temp   :8;     /*,控制板板温度*/
    #else
    #endif
}BT_Board_State_Body_TypeDef;

/*--------Sleep_Mode：0x0C-------*/
typedef __packed struct{
    #if (BYTE_ORDER == LITTLE_ENDIAN)
    uint64_t Reserve        :56;    /*预留*/
    uint64_t Mode           :8;     /*0x00:睡眠 0x01:唤醒*/
    #else
    #endif
}BT_Sleep_Mode_Body_TypeDef;

/*--------Main_Interface_CMD: 0X0D-------*/
typedef __packed struct{
    #if (BYTE_ORDER == LITTLE_ENDIAN)
    uint64_t Sleep_Mode     :1;     /*0x00:睡眠   0x01:唤醒*/
    uint64_t Reserve        :53;    /*预留*/
    uint64_t Int            :10;    /*主界面亮度*/
    #else
    #endif
}BT_Main_Interface_Body_TypeDef;

/*--------Current_Menu: 0X0E-------*/
typedef __packed struct{
    #if (BYTE_ORDER == LITTLE_ENDIAN)
    uint64_t Reserve        :64;    /*预留*/
    #else
    #endif
}BT_Current_Menu_Body_TypeDef;

/*--------Light_Bright: 0X0F-------*/
typedef __packed struct{
    #if (BYTE_ORDER == LITTLE_ENDIAN)
    uint64_t Reserve        :54;    /*预留*/
    uint64_t Int            :10;    /*主界面亮度*/
    #else
    #endif
}BT_Light_Bright_Body_TypeDef;

/*-------- 0x10-0x12为Bridge专用协议 --------*/

/*--------Program Effect Flash: 0X13-------*/
typedef __packed struct{     
    #if (BYTE_ORDER == LITTLE_ENDIAN)     
    uint64_t Reserve        :17;    /*预留*/
    uint64_t Times          :10;    /*1-1000:1-1000次     大于1000=always*/
    uint64_t Frq            :5;     /*0.1-10Hz 0=0.1Hz …… 9=1Hz …… 19=10Hz*/
    uint64_t GM             :7;     /*<0-20>: -1.0->0,-0.9->1,-0.8->2,   +1.0->20每隔0.1递增1，以此类推*/  /*max: 31*/
    uint64_t CCT            :10;    /*CCT乘以10，<2800-10000> 280->2800K,290->2900K, 300->3000K,依次类推*/     /*max: 1023*/
    uint64_t Int            :10;    /*0-1000*/
    uint64_t Base_Type      :2;     /*0x00:CCT   0x01:HSI*/
    uint64_t Light_Type     :3;     /*0x00:normal  0x01:tube  0x01:pixel mapping*/
    #else
    #endif
}BT_PFX_Flash_CCT_TypeDef;

typedef __packed struct{     
    #if (BYTE_ORDER == LITTLE_ENDIAN)     
    uint64_t Reserve        :18;    /*预留*/
    uint64_t Times          :10;    /*1-1000:1-1000次     大于1000=always*/
    uint64_t Frq            :5;     /*0.1-10Hz 0=0.1Hz …… 9=1Hz …… 19=10Hz*/
    uint64_t Sat            :7;     /*0-100*/  /*max: 127*/
    uint64_t Hue            :9;     /*0-360*/     /*max: 511*/
    uint64_t Int            :10;    /*0-1000*/
    uint64_t Base_Type      :2;     /*0x00:CCT   0x01:HSI*/
    uint64_t Light_Type     :3;     /*0x00:normal  0x01:tube  0x01:pixel mapping*/
    #else
    #endif
}BT_PFX_Flash_HSI_TypeDef;

typedef __packed union{     
    #if (BYTE_ORDER == LITTLE_ENDIAN)     
    BT_PFX_Flash_CCT_TypeDef Flash_CCT;
    BT_PFX_Flash_HSI_TypeDef Flash_HSI;
    #else
    #endif
}BT_PFX_Flash_Body_TypeDef;

/*--------Program Effect Chase: 0X14-------*/
typedef __packed struct{     
    #if (BYTE_ORDER == LITTLE_ENDIAN)     
    uint64_t Reserve        :2;     /*预留*/
    uint64_t Loop           :1;         
    uint64_t GM_Seq         :2;     /*0:正向    1:反向    2:往复    3:随机*/
    uint64_t Int_Seq        :2;     /*0:正向    1:反向    2:往复    3:随机*/
    uint64_t CCT_Seq        :2;     /*0:正向    1:反向    2:往复    3:随机*/
    uint64_t Time           :10;    /*0.1-100s， 0x09=1s   1023=always*/
    uint64_t GM_Max         :6;     /*<0-20>: -1.0->0,-0.9->1,-0.8->2,   +1.0->20每隔0.1递增1，以此类推*/  
    uint64_t GM_Mini        :6;     /*<0-20>: -1.0->0,-0.9->1,-0.8->2,   +1.0->20每隔0.1递增1，以此类推*/  
    uint64_t CCT_Max        :7;     /*CCT乘以100，<2800-10000> 28->2800K,29->2900K, 30->3000K,依次类推*/     /*max: 127*/
    uint64_t CCT_Mini       :7;     /*CCT乘以100，<2800-10000> 28->2800K,29->2900K, 30->3000K,依次类推*/     /*max: 127*/
    uint64_t Int_Max        :7;     /*0-100*/
    uint64_t Int_Mini       :7;     /*0-100*/
    uint64_t Base_Type      :2;     /*0x00:CCT   0x01:HSI*/
    uint64_t Light_Type     :3;     /*0x00:normal  0x01:tube  0x01:pixel mapping*/
    #else
    #endif
}BT_PFX_Chase_CCT_TypeDef;

typedef __packed struct{     
    #if (BYTE_ORDER == LITTLE_ENDIAN)     
    uint64_t Loop           :1;         
    uint64_t Sat_Seq        :2;     /*0:正向    1:反向    2:往复    3:随机*/
    uint64_t Int_Seq        :2;     /*0:正向    1:反向    2:往复    3:随机*/
    uint64_t Hue_Seq        :2;     /*0:正向    1:反向    2:往复    3:随机*/
    uint64_t Time           :10;    /*0.1-100s， 0x09=1s   1023=always*/
    uint64_t Sat_Max        :6;     /*0-50 *2处理*/
    uint64_t Sat_Mini       :6;     /*0-50 *2处理*/
    uint64_t Hue_Max        :9;     /*0-360*/     /*max: 511*/
    uint64_t Hue_Mini       :9;     /*0-360*/     /*max: 511*/
    uint64_t Int_Max        :6;     /*0-50 *2处理*/
    uint64_t Int_Mini       :6;     /*0-50 *2处理*/
    uint64_t Base_Type      :2;     /*0x00:CCT   0x01:HSI*/
    uint64_t Light_Type     :3;     /*0x00:normal  0x01:tube  0x01:pixel mapping*/
    #else
    #endif
}BT_PFX_Chase_HSI_TypeDef;

typedef __packed union{     
    #if (BYTE_ORDER == LITTLE_ENDIAN)     
    BT_PFX_Chase_CCT_TypeDef Chase_CCT;
    BT_PFX_Chase_HSI_TypeDef Chase_HSI;
    #else
    #endif
}BT_PFX_Chase_Body_TypeDef;

/*--------Program Effect Continue: 0X15-------*/
typedef __packed struct{     
    #if (BYTE_ORDER == LITTLE_ENDIAN)     
    uint64_t Reserve        :1;     /*预留*/    
    uint64_t FadeOutCurve   :3;     /*0:线性    1:指数    2:对数    3:s型*/ 
    uint64_t FadeOutTime    :8;     /*0.1S为单位    */    
    uint64_t ContinueTime   :10;    /*0.1-100s， 0x09=1s   1023=always*/
    uint64_t FadeInCurve    :3;     /*0:线性    1:指数    2:对数    3:s型*/ 
    uint64_t FadeInTime     :8;     /*0.1S为单位    */ 
    uint64_t GM             :6;     /*<0-20>: -1.0->0,-0.9->1,-0.8->2,   +1.0->20每隔0.1递增1，以此类推*/  
    uint64_t CCT            :10;    /*CCT乘以10，<2800-10000> 280->2800K,290->2900K, 300->3000K,依次类推*/     /*max: 1023*/
    uint64_t Int            :10;    /*0-1000*/
    uint64_t Base_Type      :2;     /*0x00:CCT   0x01:HSI*/
    uint64_t Light_Type     :3;     /*0x00:normal  0x01:tube  0x01:pixel mapping*/
    #else
    #endif
}BT_PFX_Continue_CCT_TypeDef;

typedef __packed struct{     
    #if (BYTE_ORDER == LITTLE_ENDIAN)     
    uint64_t Reserve        :1;     /*预留*/
    uint64_t FadeOutCurve   :3;     /*0:线性    1:指数    2:对数    3:s型*/ 
    uint64_t FadeOutTime    :8;     /*0.1S为单位    */    
    uint64_t ContinueTime   :10;    /*0.1-100s， 0x09=1s   1023=always*/
    uint64_t FadeInCurve    :3;     /*0:线性    1:指数    2:对数    3:s型*/ 
    uint64_t FadeInTime     :8;     /*0.1S为单位    */     
    uint64_t Sat            :7;     /*0-100*/  /*max: 127*/
    uint64_t Hue            :9;     /*0-360*/     /*max: 511*/
    uint64_t Int            :10;    /*0-1000*/
    uint64_t Base_Type      :2;     /*0x00:CCT   0x01:HSI*/
    uint64_t Light_Type     :3;     /*0x00:normal  0x01:tube  0x01:pixel mapping*/
    #else
    #endif
}BT_PFX_Continue_HSI_TypeDef;

typedef __packed union{     
    #if (BYTE_ORDER == LITTLE_ENDIAN)     
    BT_PFX_Continue_CCT_TypeDef Continue_CCT;
    BT_PFX_Continue_HSI_TypeDef Continue_HSI;
    #else
    #endif
}BT_PFX_Continue_Body_TypeDef;

/*--------CustomEffect_Bank_Ctrl: 0X1A-------*/
typedef __packed struct{     
    #if (BYTE_ORDER == LITTLE_ENDIAN)     
    uint64_t Reserve        :47;    /*预留*/
    uint64_t Current_Cache  :4;     /*当前缓存区光效，0-9：bank1-bank10，10-15：NULL(无缓存)*/
    uint64_t Bank_Info      :10;    /*外部flash当前自定义光效类型bank存储信息*/
    uint64_t Effect_Type    :3;     /*自定义光效类型*/
    #else
    #endif
}BT_CFX_Bank_RW_Body_TypeDef;

/*--------CustomEffect_Ctrl: 0X1C-------*/
typedef __packed struct{     
    #if (BYTE_ORDER == LITTLE_ENDIAN)     
    uint64_t Reserve        :31;    /*预留*/
    uint64_t Ctrl           :2;     /*0:stop  1:init  2:running 3:pause*/
    uint64_t Loop           :1;     /*0:一次 1:循环*/
    uint64_t Sequence       :2;     /*0:正序 1:反序 2:往复*/
    uint64_t Chaos          :7;     /*0%-100%*/
    uint64_t Speed          :4;     /*0=0.5X, 1=1X 2=2X, 3=4X 4=8X, 5=16X*/
    uint64_t Int            :10;    /*预留*/     
    uint64_t Bank           :4;     /*外部flash当前自定义光效类型bank存储信息*/
    uint64_t Effect_Type    :3;     /*自定义光效类型*/
    #else
    #endif
}BT_CFX_Ctrl_Body_TypeDef;

/*--------APP_Ctrl:0X1D-------*/
typedef __packed struct{
    #if (BYTE_ORDER == LITTLE_ENDIAN)     
    uint64_t Reserve       :56;     /*预留*/
    uint64_t Ctrl_Type     :8;      /*0-蓝牙重置*/
    #else
    #endif
}BT_APP_Ctrl_Body_TypeDef;

/*--------CFX_Preview:0X1E-------*/
typedef __packed struct{
    #if (BYTE_ORDER == LITTLE_ENDIAN)     
     uint32_t Reserve       :3;     /*预留*/
     uint32_t GM            :7;     /*<0-20>: 0->-1.0,1->-0.9,2->-0.8,   20->+1.0每隔0.1递增1，以此类推*/
     uint32_t CCT           :10;    /*CCT乘以10，<2800-10000> 280->2800K,290->2900K, 300->3000K,依次类推*/     /*max: 1023*/
     uint32_t Int           :10;    /*0-1000*/  /*max: 1023*/
     uint32_t Base          :2;     /*0:Base CCT    1:Base HSI*/
    #else
    #endif
}BT_CFX_Preview_BaseCCT;

typedef __packed struct{
    #if (BYTE_ORDER == LITTLE_ENDIAN)     
     uint32_t Reserve       :3;     /*预留*/
     uint32_t Sat           :7;     /*0-100*/   /*max: 127*/
     uint32_t Hue           :10;    /*0-360*/   /*max: 1023*/
     uint32_t Int           :10;    /*0-1000*/  /*max: 1023*/
     uint32_t Base          :2;     /*0:Base CCT    1:Base HSI*/
    #else
    #endif
}BT_CFX_Preview_BaseHSI;

typedef __packed union{
    #if (BYTE_ORDER == LITTLE_ENDIAN)
    BT_CFX_Preview_BaseCCT BaseCCT;
    BT_CFX_Preview_BaseHSI BaseHSI;
    #else
    #endif
}BT_CFX_Preview_Frame;

typedef __packed struct{
    #if (BYTE_ORDER == LITTLE_ENDIAN)
     BT_CFX_Preview_Frame Frame_2;  //32 Bit
     BT_CFX_Preview_Frame Frame_1;  //32 Bit
     #else
    #endif
}BT_CFX_Preview_Body_TypeDef;

/*--------MFX: 0X1F-------*/
typedef __packed struct{
    #if (BYTE_ORDER == LITTLE_ENDIAN)     
    uint64_t Reserve        :4;     /*预留*/
    uint64_t GM_Seq         :2;     /*0:正向    1:反向    2:往复    3:随机*/
    uint64_t GM_Max         :7;     /*<0-20>: 0->-1.0,1->-0.9,2->-0.8,   20->+1.0每隔0.1递增1，以此类推*/
    uint64_t GM_Mini        :7;     /*<0-20>: 0->-1.0,1->-0.9,2->-0.8,   20->+1.0每隔0.1递增1，以此类推*/
    uint64_t CCT_Seq        :2;     /*0:正向    1:反向    2:往复    3:随机*/
    uint64_t CCT_Max        :10;    /*0-360*/
    uint64_t CCT_Mini       :10;    /*0-360*/
    uint64_t Int_Seq        :2;     /*0:正向    1:反向    2:往复    3:随机*/
    uint64_t Int_Max        :7;     /*0-100*/  
    uint64_t Int_Mini       :7;     /*0-100*/  
    uint64_t Base           :2;     /*0:Base CCT    1:Base HSI*/
    uint64_t Ctrl           :2;     /*0:Stop  1:Restart  2:Pause  3:Continue*/
    uint64_t Packet_Num     :2;     /*0-3:Packet_0~Packet_3*/
    #else
    #endif
}BT_MFX_Packet_0_BaseCCT;
    
typedef __packed struct{
    #if (BYTE_ORDER == LITTLE_ENDIAN)     
    uint64_t Reserve        :4;      /*预留*/
    uint64_t Sat_Seq        :2;      /*0:正向    1:反向    2:往复    3:随机*/
    uint64_t Sat_Max        :7;      /*0-100*/     /*max: 127*/
    uint64_t Sat_Mini       :7;      /*0-100*/     /*max: 127*/
    uint64_t Hue_Seq        :2;      /*0:正向    1:反向    2:往复    3:随机*/
    uint64_t Hue_Max        :10;     /*0-360*/     /*max: 1023*/
    uint64_t Hue_Mini       :10;     /*0-360*/     /*max: 1023*/
    uint64_t Int_Seq        :2;      /*0:正向    1:反向    2:往复    3:随机*/
    uint64_t Int_Max        :7;      /*0-100*/  
    uint64_t Int_Mini       :7;      /*0-100*/  
    uint64_t Base           :2;      /*0:Base CCT    1:Base HSI*/
    uint64_t Ctrl           :2;      /*0:Stop  1:Restart  2:Pause  3:Continue*/
    uint64_t Packet_Num     :2;      /*0-3:Packet_0~Packet_3*/
    #else
    #endif
}BT_MFX_Packet_0_BaseHSI;

typedef __packed struct{
    #if (BYTE_ORDER == LITTLE_ENDIAN)     
    uint64_t Reserve        :5;      /*预留*/ 
    uint64_t FreeTime_Seq   :2;      /*0:正向    1:反向    2:往复    3:随机*/
    uint64_t FreeTime_Max   :10;     /*0-1000    0.0s~100.0s*/
    uint64_t FreeTime_Mini  :10;     /*0-1000    0.0s~100.0s*/    
    uint64_t CycleTime_Seq  :2;      /*0:正向    1:反向    2:往复    3:随机*/
    uint64_t CycleTime_Max  :10;     /*0-1000    0.0s~100.0s*/
    uint64_t CycleTime_Mini :10;     /*0-1000    0.0s~100.0s*/
    uint64_t LoopTimes      :7;      /*1-100*/
    uint64_t LoopMode       :2;      /*0:一次    1:循环   2:随机*/
    uint64_t Effect_Mode    :2;      /*0:Flash  1:Continue  2:Paragraph*/
    uint64_t Ctrl           :2;      /*0:Stop  1:Restart  2:Pause  3:Continue*/
    uint64_t Packet_Num     :2;      /*0-3:Packet_0~Packet_3*/
    #else
    #endif
}BT_MFX_Packet_1_Flash;
    
typedef __packed struct{
    #if (BYTE_ORDER == LITTLE_ENDIAN)     
    uint64_t Reserve        :3;      /*预留*/ 
    uint64_t FadeIn_Curve   :2;      /*0:线性    1:指数    2:对数    3:S型*/
    uint64_t FadeInTime_Seq :2;      /*0:正向    1:反向    2:往复    3:随机*/
    uint64_t FadeInTime_Max :10;     /*0-1000    0.0s~100.0s*/
    uint64_t FadeInTime_Mini:10;     /*0-1000    0.0s~100.0s*/    
    uint64_t CycleTime_Seq  :2;      /*0:正向    1:反向    2:往复    3:随机*/
    uint64_t CycleTime_Max  :10;     /*0-1000    0.0s~100.0s*/
    uint64_t CycleTime_Mini :10;     /*0-1000    0.0s~100.0s*/
    uint64_t LoopTimes      :7;      /*1-100*/
    uint64_t LoopMode       :2;      /*0:一次    1:循环   2:随机*/
    uint64_t Effect_Mode    :2;      /*0:Flash  1:Continue  2:Paragraph*/
    uint64_t Ctrl           :2;      /*0:Stop  1:Restart  2:Pause  3:Continue*/
    uint64_t Packet_Num     :2;      /*0-3:Packet_0~Packet_3*/
    #else
    #endif
}BT_MFX_Packet_1_Continue;    

typedef __packed struct{
    #if (BYTE_ORDER == LITTLE_ENDIAN)     
    uint64_t Reserve        :5;     /*预留*/ 
    uint64_t UnitTime_Seq   :2;     /*0:正向    1:反向    2:往复    3:随机*/
    uint64_t FreeTime_Seq   :2;     /*0:正向    1:反向    2:往复    3:随机*/
    uint64_t Overlap_Seq    :2;     /*0:正向    1:反向    2:往复    3:随机*/
    uint64_t OLR_Seq        :2;     /*0:正向    1:反向    2:往复    3:随机*/
    uint64_t OLR_Max        :7;     /*重合比例0-100*/
    uint64_t OLR_Mini       :7;     /*重合比例0-100*/   
    uint64_t CycleTime_Seq  :2;     /*0:正向    1:反向    2:往复    3:随机*/
    uint64_t CycleTime_Max  :10;    /*0-1000    0.0s~100.0s*/
    uint64_t CycleTime_Mini :10;    /*0-1000    0.0s~100.0s*/
    uint64_t LoopTimes      :7;     /*1-100*/
    uint64_t LoopMode       :2;     /*0:一次    1:循环   2:随机*/
    uint64_t Effect_Mode    :2;     /*0:Flash  1:Continue  2:Paragraph*/
    uint64_t Ctrl           :2;     /*0:Stop  1:Restart  2:Pause  3:Continue*/
    uint64_t Packet_Num     :2;     /*0-3:Packet_0~Packet_3*/
    #else
    #endif
}BT_MFX_Packet_1_Paragraph;

typedef __packed struct{
    #if (BYTE_ORDER == LITTLE_ENDIAN)     
     uint64_t Reserve       :18;    /*预留*/ 
    uint64_t Frq_Seq        :2;     /*1-129     <1-9:0.1-0.9Hz> <10-129:1-120Hz>*/
    uint64_t Frq_Max        :8;     /*0-1000    0.0s~100.0s*/
    uint64_t Frq_Mini       :8;     /*0-1000    0.0s~100.0s*/    
    uint64_t UnitTime_Seq   :2;     /*0:正向    1:反向    2:往复    3:随机*/
    uint64_t UnitTime_Max   :10;    /*0-1000    0.0s~100.0s*/
    uint64_t UnitTime_Mini  :10;    /*0-1000    0.0s~100.0s*/
    uint64_t Effect_Mode    :2;     /*0:Flash  1:Continue  2:Paragraph*/
    uint64_t Ctrl           :2;     /*0:Stop  1:Restart  2:Pause  3:Continue*/
    uint64_t Packet_Num     :2;     /*0-3:Packet_0~Packet_3*/
    #else
    #endif
}BT_MFX_Packet_2_Flash;
    
typedef __packed struct{
    #if (BYTE_ORDER == LITTLE_ENDIAN)     
    uint64_t Reserve        :26;    /*预留*/ 
    uint64_t Flicker_Frq    :8;     /*1-129     <1-9:0.1-0.9Hz> <10-129:1-120Hz>*/
    uint64_t FadeOut_Curve  :2;     /*0:线性    1:指数    2:对数    3:S型*/   
    uint64_t FadeOutTime_Seq:2;     /*0:正向    1:反向    2:往复    3:随机*/
    uint64_t FadeOutTime_Max:10;    /*0-1000    0.0s~100.0s*/
    uint64_t FadeOutTime_Mini:10;   /*0-1000    0.0s~100.0s*/
    uint64_t Effect_Mode    :2;     /*0:Flash  1:Continue  2:Paragraph*/
    uint64_t Ctrl           :2;     /*0:Stop  1:Restart  2:Pause  3:Continue*/
    uint64_t Packet_Num     :2;     /*0-3:Packet_0~Packet_3*/
    #else
    #endif
}BT_MFX_Packet_2_Continue;    

typedef __packed struct{
    #if (BYTE_ORDER == LITTLE_ENDIAN)     
    uint64_t Reserve        :2;     /*预留*/ 
    uint64_t OLP_Seq        :2;     /*0:正向    1:反向    2:往复    3:随机*/
    uint64_t OLP_Max        :7;     /*重合几率0-100*/
    uint64_t OLP_Mini       :7;     /*重合几率0-100*/
    uint64_t UnitTime_Max   :10;    /*0-1000    0.0s~100.0s*/
    uint64_t UnitTime_Mini  :10;    /*0-1000    0.0s~100.0s*/  
    uint64_t FreeTime_Max   :10;    /*0-1000    0.0s~100.0s*/
    uint64_t FreeTime_Mini  :10;    /*0-1000    0.0s~100.0s*/   
    uint64_t Effect_Mode    :2;     /*0:Flash  1:Continue  2:Paragraph*/
    uint64_t Ctrl           :2;     /*0:Stop  1:Restart  2:Pause  3:Continue*/
    uint64_t Packet_Num     :2;     /*0-3:Packet_0~Packet_3*/
    #else
    #endif
}BT_MFX_Packet_2_Paragraph; 
    
typedef __packed struct{
    #if (BYTE_ORDER == LITTLE_ENDIAN)     
    uint64_t Reserve        :2;     /*预留*/ 
    uint64_t Flicker_Frq    :8;     /*1-129     <1-9:0.1-0.9Hz> <10-129:1-120Hz>*/
    uint64_t FadeOut_Curve  :2;     /*0:线性    1:指数    2:对数    3:S型*/   
    uint64_t FadeOutTime_Seq:2;     /*0:正向    1:反向    2:往复    3:随机*/
    uint64_t FadeOutTime_Max:10;    /*0-1000    0.0s~100.0s*/
    uint64_t FadeOutTime_Mini:10;   /*0-1000    0.0s~100.0s*/    
    uint64_t FadeIn_Curve   :2;     /*0:线性    1:指数    2:对数    3:S型*/
    uint64_t FadeInTime_Seq :2;     /*0:正向    1:反向    2:往复    3:随机*/
    uint64_t FadeInTime_Max :10;    /*0-1000    0.0s~100.0s*/
    uint64_t FadeInTime_Mini:10;    /*0-1000    0.0s~100.0s*/    
    uint64_t Effect_Mode    :2;     /*0:Flash  1:Continue  2:Paragraph*/
    uint64_t Ctrl           :2;     /*0:Stop  1:Restart  2:Pause  3:Continue*/
    uint64_t Packet_Num     :2;     /*0-3:Packet_0~Packet_3*/
    #else
    #endif
}BT_MFX_Packet_3_Paragraph; 
    
typedef __packed union{
    #if (BYTE_ORDER == LITTLE_ENDIAN)     
    BT_MFX_Packet_0_BaseCCT     BaseCCT;
    BT_MFX_Packet_0_BaseHSI     BaseHSI;
    #else
    #endif
}BT_MFX_Packet_0;

typedef __packed union{
    #if (BYTE_ORDER == LITTLE_ENDIAN)     
    BT_MFX_Packet_1_Flash       Flash;
    BT_MFX_Packet_1_Continue    Continue;
    BT_MFX_Packet_1_Paragraph   Paragraph;
    #else
    #endif
}BT_MFX_Packet_1;

typedef __packed union{
    #if (BYTE_ORDER == LITTLE_ENDIAN)     
    BT_MFX_Packet_2_Flash       Flash;
    BT_MFX_Packet_2_Continue    Continue;
    BT_MFX_Packet_2_Paragraph   Paragraph;
    #else
    #endif
}BT_MFX_Packet_2;

typedef __packed union{
    #if (BYTE_ORDER == LITTLE_ENDIAN)     
    BT_MFX_Packet_3_Paragraph   Paragraph;
    #else
    #endif
}BT_MFX_Packet_3;
    
typedef __packed union{
    #if (BYTE_ORDER == LITTLE_ENDIAN)
    BT_MFX_Packet_0 Packet_0;
    BT_MFX_Packet_1 Packet_1;
    BT_MFX_Packet_2 Packet_2;
    BT_MFX_Packet_3 Packet_3;
     #else
    #endif
}BT_MFX_Body_TypeDef;

typedef __packed struct{
    #if (BYTE_ORDER == LITTLE_ENDIAN)
    uint64_t Reserve        :2;     /*预留*/
    uint64_t Character_8    :6;     /*字符8     sidus code编码*/
    uint64_t Character_7    :6;     /*字符7     sidus code编码*/
    uint64_t Character_6    :6;     /*字符6     sidus code编码*/
    uint64_t Character_5    :6;     /*字符5     sidus code编码*/
    uint64_t Character_4    :6;     /*字符4     sidus code编码*/
    uint64_t Character_3    :6;     /*字符3     sidus code编码*/
    uint64_t Character_2    :6;     /*字符2     sidus code编码*/
    uint64_t Character_1    :6;     /*字符1     sidus code编码*/
    uint64_t Character_0    :6;     /*字符0     sidus code编码*/  
    uint64_t Code_Type      :2;     /*编码方式 0: sidus code*/  
    uint64_t Bank           :4;     /*Bank位置    0-9:Bank1-10*/
    uint64_t Effect_Type    :2;     /*自定义光效类型 0-Picker         1-Touchbar   2-Music*/
    #else
    #endif
}BT_CFX_Name_Body_TypeDef;



/**************************************************************************************/
/******************************Light Effect********************************************/
/**************************************************************************************/

/************************************光效的类型***************************************/
/*0x00：Club_Lights*/
typedef __packed struct{    
    #if (BYTE_ORDER == LITTLE_ENDIAN)
    uint64_t Sleep_Mode :1;     /*0x00:睡眠   0x01:唤醒*/
    uint64_t Reserve    :37;    /*预留*/
    uint64_t Color      :4;     /*<0-3> 0:3; 1:6; 2:9; 3:12*/
    uint64_t Frq        :4;     /*0-15*/   /*max: 15*/
    uint64_t Int        :10;    /*0-1000*/
    uint64_t Effect_Type:8;     /*0-127*/
    #else
    #endif
}BT_Club_Lightse_TypeDef;

/*0x01：Paparazzi*/
typedef __packed struct{    
    #if (BYTE_ORDER == LITTLE_ENDIAN)
    uint64_t Sleep_Mode :1;     /*0x00:睡眠   0x01:唤醒*/
    uint64_t Reserve    :24;    /*预留*/
    uint64_t GM         :7;     /*<0-20>: -1.0->0,-0.9->1,-0.8->2,   +1.0->20每隔0.1递增1，以此类推*/  /*max: 31*/
    uint64_t CCT        :10;    /*CCT乘以10，<2800-10000> 280->2800K,290->2900K, 300->3000K,依次类推*/     /*max: 1023*/
    uint64_t Frq        :4;     /*0-15*/   /*max: 15*/
    uint64_t Int        :10;    /*0-1000*/
    uint64_t Effect_Type:8;     /*0-127*/
    #else
    #endif
}BT_Paparazzi_TypeDef;

/*0x02：Lightning*/
typedef __packed struct{     
    #if (BYTE_ORDER == LITTLE_ENDIAN)
    uint64_t Sleep_Mode :1;     /*0x00:睡眠   0x01:唤醒*/
    uint64_t Reserve    :18;    /*预留*/
    uint64_t Speed      :4;     /*<1-11>,11为Random*/
    uint64_t Trigger    :2;     /*<0-2>,0-无触发/1-单次触发/2-多次触发 */
    uint64_t GM         :7;     /*<0-20>: -1.0->0,-0.9->1,-0.8->2,   +1.0->20每隔0.1递增1，以此类推*/  /*max: 31*/
    uint64_t CCT        :10;    /*CCT乘以10，<2800-10000> 280->2800K,290->2900K, 300->3000K,依次类推*/     /*max: 1023*/
    uint64_t Frq        :4;     /*0-15*/   /*max: 15*/
    uint64_t Int        :10;    /*0-1000*/
    uint64_t Effect_Type:8;     /*0-127*/
    #else
    #endif
}BT_Lightning_TypeDef;

/*0x03：TV*/
typedef __packed struct{    
    #if (BYTE_ORDER == LITTLE_ENDIAN)
    uint64_t Sleep_Mode :1;     /*0x00:睡眠   0x01:唤醒*/
    uint64_t Reserve    :31;    /*预留*/
    uint64_t CCT        :10;    /*<0-1000>,CCT变化范围，0表示范围1*/     /*max: 1023*/
    uint64_t Frq        :4;     /*0-10*/
    uint64_t Int        :10;    /*0-1000*/
    uint64_t Effect_Type:8;     /*0-1000*/
    #else
    #endif
}BT_TV_TypeDef;

/*0x04：Candle*/
typedef __packed struct{    
    #if (BYTE_ORDER == LITTLE_ENDIAN)
    uint64_t Sleep_Mode :1;     /*0x00:睡眠   0x01:唤醒*/
    uint64_t Reserve    :31;    /*预留*/
    uint64_t CCT        :10;    /*<0-1000>,CCT变化范围，0表示范围1*/     /*max: 1023*/
    uint64_t Frq        :4;     /*0-10*/
    uint64_t Int        :10;    /*0-1000*/
    uint64_t Effect_Type:8;     /*0-1000*/
    #else
    #endif
}BT_Candle_TypeDef;

/*0x05：Fire*/
typedef __packed struct{     
    #if (BYTE_ORDER == LITTLE_ENDIAN)
    uint64_t Sleep_Mode :1;     /*0x00:睡眠   0x01:唤醒*/
    uint64_t Reserve    :31;    /*预留*/
    uint64_t CCT        :10;    /*<0-1000>,CCT变化范围，0表示范围1*/     /*max: 1023*/
    uint64_t Frq        :4;     /*0-10*/
    uint64_t Int        :10;    /*0-1000*/
    uint64_t Effect_Type:8;     /*0-1000*/
    #else
    #endif
}BT_Fire_TypeDef;

/*0x06：Strobe*/
typedef __packed struct{   
    #if (BYTE_ORDER == LITTLE_ENDIAN)
    uint64_t Sleep_Mode :1;     /*0x00:睡眠   0x01:唤醒*/
    uint64_t Reserve    :18;    /*预留*/
    uint64_t Trigger    :2;     /*<0-2>,0-无触发/1-单次触发/2-多次触发 */
    uint64_t GM         :7;     /*<0-20>: -1.0->0,-0.9->1,-0.8->2,   +1.0->20每隔0.1递增1，以此类推*/  /*max: 31*/
    uint64_t CCT        :10;    /*CCT乘以10，<2800-10000> 280->2800K,290->2900K, 300->3000K,依次类推*/     /*max: 1023*/
    uint64_t Int        :10;    /*0-1000*/  /*max: 1023*//*忽略*/
    uint64_t Frq        :4;     /*<0-15>*/
    uint64_t Effect_Mode:4;     /*0-10, 0x00:CCT   0x01:HSI  0x02: Gel*/
    uint64_t Effect_Type:8;     /*0-127*/
    #else
    #endif
}BT_Strobe_CCT_TypeDef;
typedef __packed struct{   
    #if (BYTE_ORDER == LITTLE_ENDIAN)
    uint64_t Sleep_Mode :1;     /*0x00:睡眠   0x01:唤醒*/
    uint64_t Reserve    :19;    /*预留*/
    uint64_t Trigger    :2;     /*<0-2>,0-无触发/1-单次触发/2-多次触发 */
    uint64_t Sat        :7;     /*0-100*/     /*max: 127*/
    uint64_t Hue        :9;     /*0-360*/     /*max: 511*/
    uint64_t Int        :10;    /*0-1000*/  /*max: 1023*/
    uint64_t Frq        :4;     /*<0-15>*/
    uint64_t Effect_Mode:4;     /*0-10, 0x00:CCT   0x01:HSI  0x02: Gel*/
    uint64_t Effect_Type:8;     /*0-127*/
    #else
    #endif
}BT_Strobe_HSI_TypeDef;

typedef __packed struct{    
    #if (BYTE_ORDER == LITTLE_ENDIAN)
    uint64_t Sleep_Mode :1;     /*0x00:睡眠   0x01:唤醒*/
    uint64_t Reserve    :10;    /*预留*/
    uint64_t Trigger    :2;     /*<0-2>,0-无触发/1-单次触发/2-多次触发 */
    uint64_t Color      :10;    /*0 - 15*/
    uint64_t Type       :4;     /*0-1023*/
    uint64_t Origin     :1;     /*品牌(0->LEE)(1->Resco)*/
    uint64_t CCT        :10;    /*CCT乘以10，<2800-10000> 280->2800K,290->2900K, 300->3000K,依次类推*/     /*max: 1023*/
    uint64_t Int        :10;    /*0-1000*//*忽略*/
    uint64_t Frq        :4;     /*<0-15>*/
    uint64_t Effect_Mode:4;     /*0-10*/
    uint64_t Effect_Type:8;     /*0-1000*/
    #else
    #endif
}BT_Strobe_GEL_TypeDef;

typedef __packed struct{    
    #if (BYTE_ORDER == LITTLE_ENDIAN)
    uint64_t Sleep_Mode :1;     /*0x00:睡眠   0x01:唤醒*/
    uint64_t Reserve    :7;     /*预留*/
    uint64_t Trigger    :2;     /*<0-2>,0-无触发/1-单次触发/2-多次触发 */
    uint64_t Coordinate_y:14;   /*0-10000*/     /*max: 16383*/
    uint64_t Coordinate_x:14;   /*0-10000*/     /*max: 16383*/
    uint64_t Int        :10;    /*0-1000*/  /*max: 1023*/
    uint64_t Frq        :4;     /*<0-15>*/
    uint64_t Effect_Mode:4;     /*0-10, 0x00:CCT   0x01:HSI  0x02: Gel*/
    uint64_t Effect_Type:8;     /*0-127*/
    #else
    #endif
}BT_Strobe_XY_TypeDef;

typedef __packed union{
    BT_Strobe_CCT_TypeDef   Effect_CCT_Mode;
    BT_Strobe_HSI_TypeDef   Effect_HSI_Mode;
    BT_Strobe_GEL_TypeDef   Effect_Gel_Mode;
    BT_Strobe_XY_TypeDef    Effect_XY_Mode;
}BT_Strobe_MODE_TypeDef;

typedef __packed struct{     
     BT_Strobe_MODE_TypeDef Mode;
}BT_Strobe_TypeDef;

/*0x07：Explosion*/
typedef __packed struct{     
    #if (BYTE_ORDER == LITTLE_ENDIAN)
    uint64_t Sleep_Mode :1;     /*0x00:睡眠   0x01:唤醒*/
    uint64_t Reserve    :18;    /*预留*/
    uint64_t Trigger    :2;     /*<0-2>,0-无触发/1-单次触发/2-多次触发 */
    uint64_t GM         :7;     /*<0-20>: -1.0->0,-0.9->1,-0.8->2,   +1.0->20每隔0.1递增1，以此类推*/  /*max: 31*/
    uint64_t CCT        :10;    /*CCT乘以10，<2800-10000> 280->2800K,290->2900K, 300->3000K,依次类推*/     /*max: 1023*/
    uint64_t Int        :10;    /*0-1000*/  /*max: 1023*//*忽略*/
    uint64_t Frq        :4;     /*<0-15>*/
    uint64_t Effect_Mode:4;     /*0-10, 0x00:CCT   0x01:HSI  0x02: Gel*/
    uint64_t Effect_Type:8;     /*0-127*/
    #else
    #endif
}BT_Explosion_CCT_TypeDef;
typedef __packed struct{    
    #if (BYTE_ORDER == LITTLE_ENDIAN)
    uint64_t Sleep_Mode :1;     /*0x00:睡眠   0x01:唤醒*/
    uint64_t Reserve    :19;    /*预留*/
    uint64_t Trigger    :2;     /*<0-2>,0-无触发/1-单次触发/2-多次触发 */
    uint64_t Sat        :7;     /*0-100*/     /*max: 127*/
    uint64_t Hue        :9;     /*0-360*/     /*max: 511*/
    uint64_t Int        :10;    /*0-1000*/  /*max: 1023*/
    uint64_t Frq        :4;     /*<0-15>*/
    uint64_t Effect_Mode:4;     /*0-10, 0x00:CCT   0x01:HSI  0x02: Gel*/
    uint64_t Effect_Type:8;     /*0-127*/
    #else
    #endif
}BT_Explosion_HSI_TypeDef;

typedef __packed struct{
    #if (BYTE_ORDER == LITTLE_ENDIAN)
    uint64_t Sleep_Mode :1;     /*0x00:睡眠   0x01:唤醒*/
    uint64_t Reserve    :10;    /*预留*/
    uint64_t Trigger    :2;     /*<0-2>,0-无触发/1-单次触发/2-多次触发 */
    uint64_t Color      :10;    /*0 - 15*/
    uint64_t Type       :4;     /*0-1023*/
    uint64_t Origin     :1;     /*品牌(0->LEE)(1->Resco)*/
    uint64_t CCT        :10;    /*CCT乘以10，<2800-10000> 280->2800K,290->2900K, 300->3000K,依次类推*/     /*max: 1023*/
    uint64_t Int        :10;    /*0-1000*//*忽略*/
    uint64_t Frq        :4;     /*<0-15>*/
    uint64_t Effect_Mode:4;     /*0-10*/
    uint64_t Effect_Type:8;     /*0-1000*/
    #else
    #endif
}BT_Explosion_GEL_TypeDef;

typedef __packed struct{   
    #if (BYTE_ORDER == LITTLE_ENDIAN)
    uint64_t Sleep_Mode :1;     /*0x00:睡眠   0x01:唤醒*/
    uint64_t Reserve    :7;     /*预留*/
    uint64_t Trigger    :2;     /*<0-2>,0-无触发/1-单次触发/2-多次触发 */
    uint64_t Coordinate_y:14;   /*0-10000*/     /*max: 16383*/
    uint64_t Coordinate_x:14;   /*0-10000*/     /*max: 16383*/
    uint64_t Int        :10;    /*0-1000*/  /*max: 1023*/
    uint64_t Frq        :4;     /*<0-15>*/
    uint64_t Effect_Mode:4;     /*0-10, 0x00:CCT   0x01:HSI  0x02: Gel*/
    uint64_t Effect_Type:8;     /*0-127*/
    #else
    #endif
}BT_Explosion_XY_TypeDef;

typedef __packed union{
    BT_Explosion_CCT_TypeDef    Effect_CCT_Mode;
    BT_Explosion_HSI_TypeDef    Effect_HSI_Mode;
    BT_Explosion_GEL_TypeDef    Effect_Gel_Mode;
    BT_Explosion_XY_TypeDef     Effect_XY_Mode;
}BT_Explosion_MODE_TypeDef;

typedef __packed struct{    
     BT_Explosion_MODE_TypeDef Mode;
}BT_Explosion_TypeDef;

/*0x08：Fault_Bulb*/
typedef __packed struct{    
    #if (BYTE_ORDER == LITTLE_ENDIAN)
    uint64_t Sleep_Mode :1;     /*0x00:睡眠   0x01:唤醒*/
    uint64_t Reserve    :14;    /*预留*/
    uint64_t Speed      :4;     /*<1-11>,11为Random*/
    uint64_t Trigger    :2;     /*<0-2>,0-无触发/1-单次触发/2-多次触发 */
    uint64_t GM         :7;     /*<0-20>: -1.0->0,-0.9->1,-0.8->2,   +1.0->20每隔0.1递增1，以此类推*/  /*max: 31*/
    uint64_t CCT        :10;    /*CCT乘以10，<2800-10000> 280->2800K,290->2900K, 300->3000K,依次类推*/     /*max: 1023*/
    uint64_t Int        :10;    /*0-1000*/  /*max: 1023*//*忽略*/
    uint64_t Frq        :4;     /*<0-15>*/
    uint64_t Effect_Mode:4;     /*0-10, 0x00:CCT   0x01:HSI  0x02: Gel*/
    uint64_t Effect_Type:8;     /*0-127*/
    #else
    #endif
}BT_FaultBulb_CCT_TypeDef;

typedef __packed struct{    
    #if (BYTE_ORDER == LITTLE_ENDIAN)
    uint64_t Sleep_Mode :1;     /*0x00:睡眠   0x01:唤醒*/
    uint64_t Reserve    :15;    /*预留*/
    uint64_t Speed      :4;     /*<1-11>,11为Random*/
    uint64_t Trigger    :2;     /*<0-2>,0-无触发/1-单次触发/2-多次触发 */
    uint64_t Sat        :7;     /*0-100*/     /*max: 127*/
    uint64_t Hue        :9;     /*0-360*/     /*max: 511*/
    uint64_t Int        :10;    /*0-1000*/  /*max: 1023*/
    uint64_t Frq        :4;     /*<0-15>*/
    uint64_t Effect_Mode:4;     /*0-10, 0x00:CCT   0x01:HSI  0x02: Gel*/
    uint64_t Effect_Type:8;     /*0-127*/
    #else
    #endif
}BT_FaultBulb_HSI_TypeDef;

typedef __packed struct{    
    #if (BYTE_ORDER == LITTLE_ENDIAN)
    uint64_t Sleep_Mode :1;     /*0x00:睡眠   0x01:唤醒*/
    uint64_t Reserve    :6;     /*预留*/
    uint64_t Speed      :4;     /*<1-11>,11为Random*/
    uint64_t Trigger    :2;     /*<0-2>,0-无触发/1-单次触发/2-多次触发 */
    uint64_t Color      :10;    /*0 - 15*/
    uint64_t Type       :4;     /*0-1023*/
    uint64_t Origin     :1;     /*品牌(0->LEE)(1->Resco)*/
    uint64_t CCT        :10;    /*CCT乘以10，<2800-10000> 280->2800K,290->2900K, 300->3000K,依次类推*/     /*max: 1023*/
    uint64_t Int        :10;    /*0-1000*//*忽略*/
    uint64_t Frq        :4;     /*<0-15>*/
    uint64_t Effect_Mode:4;     /*0-10*/
    uint64_t Effect_Type:8;     /*0-1000*/
    #else
    #endif
}BT_FaultBulb_GEL_TypeDef;

typedef __packed struct{    
    #if (BYTE_ORDER == LITTLE_ENDIAN)
    uint64_t Sleep_Mode :1;     /*0x00:睡眠   0x01:唤醒*/
    uint64_t Reserve    :3;     /*预留*/
    uint64_t Speed      :4;     /*<1-11>,11为Random*/
    uint64_t Trigger    :2;     /*<0-2>,0-无触发/1-单次触发/2-多次触发 */
    uint64_t Coordinate_y:14;   /*0-10000*/     /*max: 16383*/
    uint64_t Coordinate_x:14;   /*0-10000*/     /*max: 16383*/
    uint64_t Int        :10;    /*0-1000*/  /*max: 1023*/
    uint64_t Frq        :4;     /*<0-15>*/
    uint64_t Effect_Mode:4;     /*0-10, 0x00:CCT   0x01:HSI  0x02: Gel*/
    uint64_t Effect_Type:8;     /*0-127*/
    #else
    #endif
}BT_FaultBulb_XY_TypeDef;

typedef __packed union{
    BT_FaultBulb_CCT_TypeDef    Effect_CCT_Mode;
    BT_FaultBulb_HSI_TypeDef    Effect_HSI_Mode;
    BT_FaultBulb_GEL_TypeDef    Effect_Gel_Mode;
    BT_FaultBulb_XY_TypeDef     Effect_XY_Mode;
}BT_FaultBulb_MODE_TypeDef;

typedef __packed struct{    
     BT_FaultBulb_MODE_TypeDef Mode;
}BT_FaultBulb_TypeDef;

/*0x09：Pulsing*/
typedef __packed struct{     
    #if (BYTE_ORDER == LITTLE_ENDIAN)
    uint64_t Sleep_Mode :1;     /*0x00:睡眠   0x01:唤醒*/
    uint64_t Reserve    :14;    /*预留*/
    uint64_t Speed      :4;     /*<1-11>,11为Random*/
    uint64_t Trigger    :2;     /*<0-2>,0-无触发/1-单次触发/2-多次触发 */
    uint64_t GM         :7;     /*<0-20>: -1.0->0,-0.9->1,-0.8->2,   +1.0->20每隔0.1递增1，以此类推*/  /*max: 31*/
    uint64_t CCT        :10;    /*CCT乘以10，<2800-10000> 280->2800K,290->2900K, 300->3000K,依次类推*/     /*max: 1023*/
    uint64_t Int        :10;    /*0-1000*/  /*max: 1023*//*忽略*/
    uint64_t Frq        :4;     /*<0-15>*/
    uint64_t Effect_Mode:4;     /*0-10, 0x00:CCT   0x01:HSI  0x02: Gel*/
    uint64_t Effect_Type:8;     /*0-127*/
    #else
    #endif
}BT_Pulsing_CCT_TypeDef;
typedef __packed struct{    
    #if (BYTE_ORDER == LITTLE_ENDIAN)
    uint64_t Sleep_Mode :1;     /*0x00:睡眠   0x01:唤醒*/
    uint64_t Reserve    :15;    /*预留*/
    uint64_t Speed      :4;     /*<1-11>,11为Random*/
    uint64_t Trigger    :2;     /*<0-2>,0-无触发/1-单次触发/2-多次触发 */
    uint64_t Sat        :7;     /*0-100*/     /*max: 127*/
    uint64_t Hue        :9;     /*0-360*/     /*max: 511*/
    uint64_t Int        :10;    /*0-1000*/  /*max: 1023*/
    uint64_t Frq        :4;     /*<0-15>*/
    uint64_t Effect_Mode:4;     /*0-10, 0x00:CCT   0x01:HSI  0x02: Gel*/
    uint64_t Effect_Type:8;     /*0-127*/
    #else
    #endif
}BT_Pulsing_HSI_TypeDef;

typedef __packed struct{    
    #if (BYTE_ORDER == LITTLE_ENDIAN)
    uint64_t Sleep_Mode :1;     /*0x00:睡眠   0x01:唤醒*/
    uint64_t Reserve    :6;     /*预留*/
    uint64_t Speed      :4;     /*<1-11>,11为Random*/
    uint64_t Trigger    :2;     /*<0-2>,0-无触发/1-单次触发/2-多次触发 */
    uint64_t Color      :10;    /*0 - 15*/
    uint64_t Type       :4;     /*0-1023*/
    uint64_t Origin     :1;     /*品牌(0->LEE)(1->Resco)*/
    uint64_t CCT        :10;    /*CCT乘以10，<2800-10000> 280->2800K,290->2900K, 300->3000K,依次类推*/     /*max: 1023*/
    uint64_t Int        :10;    /*0-1000*//*忽略*/
    uint64_t Frq        :4;     /*<0-15>*/
    uint64_t Effect_Mode:4;     /*0-10*/
    uint64_t Effect_Type:8;     /*0-1000*/
    #else
    #endif
}BT_Pulsing_GEL_TypeDef;

typedef __packed struct{   
    #if (BYTE_ORDER == LITTLE_ENDIAN)
    uint64_t Sleep_Mode :1;     /*0x00:睡眠   0x01:唤醒*/
    uint64_t Reserve    :3;     /*预留*/
    uint64_t Speed      :4;     /*<1-11>,11为Random*/
    uint64_t Trigger    :2;     /*<0-2>,0-无触发/1-单次触发/2-多次触发 */
    uint64_t Coordinate_y:14;   /*0-10000*/     /*max: 16383*/
    uint64_t Coordinate_x:14;   /*0-10000*/     /*max: 16383*/
    uint64_t Int        :10;    /*0-1000*/  /*max: 1023*/
    uint64_t Frq        :4;     /*<0-15>*/
    uint64_t Effect_Mode:4;     /*0-10, 0x00:CCT   0x01:HSI  0x02: Gel*/
    uint64_t Effect_Type:8;     /*0-127*/
    #else
    #endif
}BT_Pulsing_XY_TypeDef;
typedef __packed union{
    BT_Pulsing_CCT_TypeDef  Effect_CCT_Mode;
    BT_Pulsing_HSI_TypeDef  Effect_HSI_Mode;
    BT_Pulsing_GEL_TypeDef  Effect_Gel_Mode;
    BT_Pulsing_XY_TypeDef   Effect_XY_Mode;
}BT_Pulsing_MODE_TypeDef;

typedef __packed struct{    
    BT_Pulsing_MODE_TypeDef Mode;
}BT_Pulsing_TypeDef;

/*0x0A：Welding*/
typedef __packed struct{     
    #if (BYTE_ORDER == LITTLE_ENDIAN)
    uint64_t Sleep_Mode :1;     /*0x00:睡眠   0x01:唤醒*/
    uint64_t Reserve    :11;    /*预留*/
    uint64_t Min        :7;     /*<0-100>,100表示100%*/
    uint64_t Trigger    :2;     /*<0-2>,0-无触发/1-单次触发/2-多次触发 */
    uint64_t GM         :7;     /*<0-20>: -1.0->0,-0.9->1,-0.8->2,   +1.0->20每隔0.1递增1，以此类推*/  /*max: 31*/
    uint64_t CCT        :10;    /*CCT乘以10，<2800-10000> 280->2800K,290->2900K, 300->3000K,依次类推*/     /*max: 1023*/
    uint64_t Int        :10;    /*0-1000*/  /*max: 1023*//*忽略*/
    uint64_t Frq        :4;     /*<0-15>*/
    uint64_t Effect_Mode:4;     /*0-10, 0x00:CCT   0x01:HSI  0x02: Gel*/
    uint64_t Effect_Type:8;     /*0-127*/
    #else
    #endif
}BT_Welding_CCT_TypeDef;

typedef __packed struct{   
    #if (BYTE_ORDER == LITTLE_ENDIAN)
    uint64_t Sleep_Mode :1;     /*0x00:睡眠   0x01:唤醒*/
    uint64_t Reserve    :12;    /*预留*/
    uint64_t Min        :7;     /*<0-100>,100表示100%*/
    uint64_t Trigger    :2;     /*<0-2>,0-无触发/1-单次触发/2-多次触发 */
    uint64_t Sat        :7;     /*0-100*/     /*max: 127*/
    uint64_t Hue        :9;     /*0-360*/     /*max: 511*/
    uint64_t Int        :10;    /*0-1000*/  /*max: 1023*/
    uint64_t Frq        :4;     /*<0-15>*/
    uint64_t Effect_Mode:4;     /*0-10, 0x00:CCT   0x01:HSI  0x02: Gel*/
    uint64_t Effect_Type:8;     /*0-127*/
    #else
    #endif
}BT_Welding_HSI_TypeDef;

typedef __packed struct{    
    #if (BYTE_ORDER == LITTLE_ENDIAN)
    uint64_t Sleep_Mode :1;     /*0x00:睡眠   0x01:唤醒*/
    uint64_t Reserve    :3;     /*预留*/
    uint64_t Min        :7;     /*<0-100>,100表示100%*/
    uint64_t Trigger    :2;     /*<0-2>,0-无触发/1-单次触发/2-多次触发 */
    uint64_t Color      :10;    /*0 - 15*/
    uint64_t Type       :4;     /*0-1023*/
    uint64_t Origin     :1;     /*品牌(0->LEE)(1->Resco)*/
    uint64_t CCT        :10;    /*CCT乘以10，<2800-10000> 280->2800K,290->2900K, 300->3000K,依次类推*/     /*max: 1023*/
    uint64_t Int        :10;    /*0-1000*//*忽略*/
    uint64_t Frq        :4;     /*<0-15>*/
    uint64_t Effect_Mode:4;     /*0-10*/
    uint64_t Effect_Type:8;     /*0-1000*/
    #else
    #endif
}BT_Welding_GEL_TypeDef;

typedef __packed struct{     
    #if (BYTE_ORDER == LITTLE_ENDIAN)
    uint64_t Sleep_Mode :1;     /*0x00:睡眠   0x01:唤醒*/
    //uint64_t Reserve    :3;     /*预留*/
    uint64_t Min        :7;     /*<0-100>,100表示100%*/
    uint64_t Trigger    :2;     /*<0-2>,0-无触发/1-单次触发/2-多次触发 */
    uint64_t Coordinate_y:14;   /*0-10000*/     /*max: 16383*/
    uint64_t Coordinate_x:14;   /*0-10000*/     /*max: 16383*/
    uint64_t Int        :10;    /*0-1000*/  /*max: 1023*/
    uint64_t Frq        :4;     /*<0-15>*/
    uint64_t Effect_Mode:4;     /*0-10, 0x00:CCT   0x01:HSI  0x02: Gel*/
    uint64_t Effect_Type:8;     /*0-127*/
    #else
    #endif
}BT_Welding_XY_TypeDef;

typedef __packed union{
    BT_Welding_CCT_TypeDef  Effect_CCT_Mode;
    BT_Welding_HSI_TypeDef  Effect_HSI_Mode;
    BT_Welding_GEL_TypeDef  Effect_Gel_Mode;
    BT_Welding_XY_TypeDef   Effect_XY_Mode;
}BT_Welding_MODE_TypeDef;

typedef __packed struct{
    BT_Welding_MODE_TypeDef Mode;
}BT_Welding_TypeDef;

/*0x0B：Cop_Car*/
typedef __packed struct{    
    #if (BYTE_ORDER == LITTLE_ENDIAN)
    uint64_t Sleep_Mode :1;     /*0x00:睡眠   0x01:唤醒*/
    uint64_t Reserve    :37;    /*预留*/
    uint64_t Color      :4;     /*0x00：R；0x01：B；0x02：R+B；0x03：B+W;  0x04：B+W+R*//*max: 1023*/
    uint64_t Frq        :4;     /*0-10*/
    uint64_t Int        :10;    /*0-1000*/
    uint64_t Effect_Type:8;     /*0-1000*/
    #else
    #endif
}BT_Cop_Car_TypeDef;

/*0x0C：Color_Chase*/
typedef __packed struct{    
    #if (BYTE_ORDER == LITTLE_ENDIAN)
    uint64_t Sleep_Mode :1;     /*0x00:睡眠   0x01:唤醒*/
    uint64_t Reserve    :34;    /*预留*/
    uint64_t Sat        :7;     /*0-100*/     /*max: 127*/
    uint64_t Frq        :4;     /*0-10*/
    uint64_t Int        :10;    /*0-1000*/
    uint64_t Effect_Type:8;     /*0-1000*/
    #else
    #endif
}BT_Color_Chase_TypeDef;

/*0x0D：Party_Lights*/
typedef __packed struct{    
    #if (BYTE_ORDER == LITTLE_ENDIAN)
    uint64_t Sleep_Mode :1;     /*0x00:睡眠   0x01:唤醒*/
    uint64_t Reserve    :34;    /*预留*/
    uint64_t Sat        :7;     /*0-100*/     /*max: 127*/
    uint64_t Frq        :4;     /*0-10*/
    uint64_t Int        :10;    /*0-1000*/
    uint64_t Effect_Type:8;     /*0-1000*/
    #else
    #endif
}BT_Party_Lights_TypeDef;

/*0x0E：Fireworks*/
typedef __packed struct{    
    #if (BYTE_ORDER == LITTLE_ENDIAN)
    uint64_t Sleep_Mode :1;     /*0x00:睡眠   0x01:唤醒*/
    uint64_t Reserve    :33;    /*预留*/
    uint64_t Type       :8;     /*0-100*/     /*max: 127*/
    uint64_t Frq        :4;     /*0-10*/
    uint64_t Int        :10;    /*0-1000*/
    uint64_t Effect_Type:8;     /*0-1000*/
    #else
    #endif
}BT_Fireworks_TypeDef;

/*0x0F：Effect_Off*/
typedef __packed struct{    
    #if (BYTE_ORDER == LITTLE_ENDIAN)
    uint64_t Sleep_Mode :1;     /*0x00:睡眠   0x01:唤醒*/
    uint64_t Reserve    :55;    /*预留*/
    uint64_t Effect_Type:8;     /*0-1000*/
    #else
    #endif
}BT_Effect_Off_TypeDef;

/*0x10：I_AM_HERE*/
typedef __packed struct{  
    #if (BYTE_ORDER == LITTLE_ENDIAN)
    uint64_t Sleep_Mode :1;     /*0x00:睡眠   0x01:唤醒*/
    uint64_t Reserve    :53;    /*预留*/
    uint64_t Ctrl       :2;     /*控制状态 0x00：启动    0x01：停止并退出*/
    uint64_t Effect_Type:8;     /*0-1000*/
    #else
    #endif
}I_Am_Here_TypeDef;

/*--------EFFECT_CMD：0x07-------*/
typedef __packed union{   
    BT_Club_Lightse_TypeDef Club_Lights_Arg;
    BT_Paparazzi_TypeDef    Paparazzi_Arg;
    BT_Lightning_TypeDef    Lightning_Arg;
    BT_TV_TypeDef           TV_Arg;
    BT_Candle_TypeDef       Candle_Arg;
    BT_Fire_TypeDef         Fire_Arg;
    BT_Strobe_TypeDef       Strobe_Arg;
    BT_Explosion_TypeDef    Explosion_Arg;
    BT_FaultBulb_TypeDef    Fault_Bulb_Arg;
    BT_Pulsing_TypeDef      Pulsing_Arg;
    BT_Welding_TypeDef      Welding_Arg;
    BT_Cop_Car_TypeDef      Cop_Car_Arg;
    BT_Color_Chase_TypeDef  Color_Chase_Arg;
    BT_Party_Lights_TypeDef Party_Lights_Arg;
    BT_Fireworks_TypeDef    Fireworks_Arg;
    BT_Effect_Off_TypeDef   Effect_Off_Arg;
    I_Am_Here_TypeDef       I_Am_Here_Arg;
}BT_Effect_Mode_Body_TypeDef; //Bluetooth_Effect_TypeDef

/**************************************************************************************/
/******************************pixel Effect********************************************/
/**************************************************************************************/

/************************************光效的类型***************************************/
/*---像素光效类型---*/
typedef enum{
    BT_PixelFX_Color_Cut = 0,
    BT_PixelFX_Color_Replace,
    BT_PixelFX_Color_Move_I,
    BT_PixelFX_Color_Move_II,
    BT_PixelFX_Color_Move_III,
    BT_PixelFX_Pixel_Fire,
    BT_PixelFX_Color_Move_M,
    BT_PixelFX_Rainbow,
    BT_PixelFX_On_Off_Effect,
    BT_PixelFX_Belt_Effect,
    BT_PixelFX_Music,
    BT_PixelFX_Null,
}bt_pixel_fx_type_enum;

/*---像素光效里的子类型---*/
typedef enum{
    BT_PixelFX_Mode_CCT = 0,
    BT_PixelFX_Mode_HSI,
    BT_PixelFX_Mode_Black,
    BT_PixelFX_Mode_Null,
}bt_pixel_fx_mode_enum;

/*---像素光效触发类型---*/
typedef enum{
    BT_PixelFX_State_None = 0,
    BT_PixelFX_State_Once,
    BT_PixelFX_State_Continue,
}bt_pixel_fx_state_enum;

/*0x00：color_cut*/
typedef __packed struct{    
    #if (BYTE_ORDER == LITTLE_ENDIAN)
    uint64_t reserve        :34;    /*预留*/
    uint64_t spd            :10;    /*<1-640> -> <1cm/s - 640cm/s> 精度1cm/s*/
    uint64_t dir            :4;     /*0-1 0-左，1-右*/
    uint64_t color_number   :4;     /*2-10 -> 2个像素 - 10个像素*/
    uint64_t package_type   :2;     /*<0-1>,0-分包1,1-分包2，此处为1*/
    uint64_t state          :2;     /*0-停止，1-暂停，2-运行，3-continue*/
    uint64_t effect_type    :8;     /*0-127*/
    #else
    #endif
}bt_color_cut_type1_arg_t;

typedef __packed struct{    
    #if (BYTE_ORDER == LITTLE_ENDIAN)
    uint64_t reserve        :19;    /*预留*/
    uint64_t duv            :8;     /*<0-200> 精度0.01,0->-1.0,200->1.0*/
    uint64_t cct            :9;     /*<30-400>-><1500-20000>,精度50K*/
    uint64_t light_mode     :2;     /*<0-2>,0-cct,1-hsi，2-black*/
    uint64_t brightness     :10;    /*<0-1000>,精度0.1%*/
    uint64_t serial         :4;     /*<0-9>-><像素1-像素10>*/   
    uint64_t package_type   :2;     /*<0-1>,0-分包1,1-分包2，此处为1*/    
    uint64_t state          :2;     /*0-停止，1-暂停，2-运行，3-continue*/
    uint64_t effect_type    :8;     /*0-127*/
    #else
    #endif
}bt_color_cut_type2_cct_arg_t;
typedef __packed struct{    
    #if (BYTE_ORDER == LITTLE_ENDIAN)
    uint64_t reserve        :11;    /*预留*/
    uint64_t cct            :9;     /*<30-400>-><1500-20000>,精度50K*/
    uint64_t sat            :7;     /*<0-100>*/
    uint64_t hue            :9;     /*<1-360>*/
    uint64_t light_mode     :2;     /*<0-2>,0-cct,1-hsi，2-black*/
    uint64_t brightness     :10;    /*<0-1000>,精度0.1%*/
    uint64_t serial         :4;     /*<0-9>-><像素1-像素10>*/   
    uint64_t package_type   :2;     /*<0-1>,0-分包1,1-分包2，此处为1*/    
    uint64_t state          :2;     /*0-停止，1-暂停，2-运行，3-continue*/
    uint64_t effect_type    :8;     /*0-127*/
    #else
    #endif
}bt_color_cut_type2_hsi_arg_t;
typedef __packed struct{    
    #if (BYTE_ORDER == LITTLE_ENDIAN)
    uint64_t reserve        :36;    /*预留*/
    uint64_t light_mode     :2;     /*<0-2>,0-cct,1-hsi，2-black*/
    uint64_t brightness     :10;    /*<0-1000>,精度0.1%*/
    uint64_t serial         :4;     /*<0-9>-><像素1-像素10>*/   
    uint64_t package_type   :2;     /*<0-1>,0-分包1,1-分包2，此处为1*/    
    uint64_t state          :2;     /*0-停止，1-暂停，2-运行，3-continue*/
    uint64_t effect_type    :8;     /*0-127*/
    #else
    #endif
}bt_color_cut_type2_black_arg_t;

typedef __packed union{
    bt_color_cut_type2_cct_arg_t    cct;
    bt_color_cut_type2_hsi_arg_t    hsi;
    bt_color_cut_type2_black_arg_t  black;
}bt_color_cut_type2_arg_t;

typedef __packed union{
    bt_color_cut_type1_arg_t    base;
    bt_color_cut_type2_arg_t    pixel;
}bt_color_cut_arg_t;

/*0x01：color_replace*/
typedef __packed struct{    
    #if (BYTE_ORDER == LITTLE_ENDIAN)
    uint64_t reserve        :32;    /*预留*/
    uint64_t color_change_way :2;   /*<0-1> 0-瞬变，1-缓变*/
    uint64_t spd            :10;    /*<1-100> 精度0.1秒*/
    uint64_t dir            :4;     /*<0-1> -> <左、右>*/
    uint64_t color_number   :4;     /*2-10 -> 2个像素 - 10个像素*/
    uint64_t package_type   :2;     /*<0-1>,0-分包1,1-分包2，此处为1*/
    uint64_t state          :2;     /*0-停止，1-暂停，2-运行，3-continue*/
    uint64_t effect_type    :8;     /*0-127*/
    #else
    #endif
}bt_color_replace_type1_arg_t;

typedef __packed struct{    
    #if (BYTE_ORDER == LITTLE_ENDIAN)
    uint64_t reserve        :19;    /*预留*/
    uint64_t duv            :8;     /*<0-200> 精度0.01,0->-1.0,200->1.0*/
    uint64_t cct            :9;     /*<30-400>-><1500-20000>,精度50K*/
    uint64_t light_mode     :2;     /*<0-2>,0-cct,1-hsi，2-black*/
    uint64_t brightness     :10;    /*<0-1000>,精度0.1%*/
    uint64_t serial         :4;     /*<0-9>-><像素1-像素10>*/   
    uint64_t package_type   :2;     /*<0-1>,0-分包1,1-分包2，此处为1*/    
    uint64_t state          :2;     /*0-停止，1-暂停，2-运行，3-continue*/
    uint64_t effect_type    :8;     /*0-127*/
    #else
    #endif
}bt_color_replace_type2_cct_arg_t;
typedef __packed struct{    
    #if (BYTE_ORDER == LITTLE_ENDIAN)
    uint64_t reserve        :11;    /*预留*/
    uint64_t cct            :9;     /*<30-400>-><1500-20000>,精度50K*/
    uint64_t sat            :7;     /*<0-100>*/
    uint64_t hue            :9;     /*<1-360>*/
    uint64_t light_mode     :2;     /*<0-2>,0-cct,1-hsi，2-black*/
    uint64_t brightness     :10;    /*<0-1000>,精度0.1%*/
    uint64_t serial         :4;     /*<0-9>-><像素1-像素10>*/   
    uint64_t package_type   :2;     /*<0-1>,0-分包1,1-分包2，此处为1*/    
    uint64_t state          :2;     /*0-停止，1-暂停，2-运行，3-continue*/
    uint64_t effect_type    :8;     /*0-127*/
    #else
    #endif
}bt_color_replace_type2_hsi_arg_t;
typedef __packed struct{    
    #if (BYTE_ORDER == LITTLE_ENDIAN)
    uint64_t reserve        :36;    /*预留*/
    uint64_t light_mode     :2;     /*<0-2>,0-cct,1-hsi，2-black*/
    uint64_t brightness     :10;    /*<0-1000>,精度0.1%*/
    uint64_t serial         :4;     /*<0-9>-><像素1-像素10>*/   
    uint64_t package_type   :2;     /*<0-1>,0-分包1,1-分包2，此处为1*/    
    uint64_t state          :2;     /*0-停止，1-暂停，2-运行，3-continue*/
    uint64_t effect_type    :8;     /*0-127*/
    #else
    #endif
}bt_color_replace_type2_black_arg_t;

typedef __packed union{
    bt_color_replace_type2_cct_arg_t    cct;
    bt_color_replace_type2_hsi_arg_t    hsi;
    bt_color_replace_type2_black_arg_t  black;
}bt_color_replace_type2_arg_t;

typedef __packed union{
    bt_color_replace_type1_arg_t    base;
    bt_color_replace_type2_arg_t    pixel;
}bt_color_replace_arg_t;

/*0x02：color_move_I*/
typedef __packed struct{    
    #if (BYTE_ORDER == LITTLE_ENDIAN)
    uint64_t reserve        :35;    /*预留*/
    uint64_t spd            :10;    /*<1-640> -> <1cm/s - 640cm/s> 精度1cm/s*/
    uint64_t pixel_lenght   :3;     /*像素长度<0-2> -> <S/M/L>*/
    uint64_t dir            :2;     /*单组：0-左，1-右，2-来回；双组：0-交错，1-反弹*/
    uint64_t group          :2;     /*0-单组，1-双组*/
    uint64_t package_type   :2;     /*<0-1>,0-分包1,1-分包2，此处为1*/
    uint64_t state          :2;     /*0-停止，1-暂停，2-运行，3-continue*/
    uint64_t effect_type    :8;     /*0-127*/
    #else
    #endif
}bt_color_move_I_type1_arg_t;

typedef __packed struct{    
    #if (BYTE_ORDER == LITTLE_ENDIAN)
    uint64_t reserve        :19;    /*预留*/
    uint64_t duv            :8;     /*<0-200> 精度0.01,0->-1.0,200->1.0*/
    uint64_t cct            :9;     /*<30-400>-><1500-20000>,精度50K*/
    uint64_t light_mode     :2;     /*<0-2>,0-cct,1-hsi，2-black*/
    uint64_t brightness     :10;    /*<0-1000>,精度0.1%*/
    uint64_t serial         :4;     /*<0-2>-><底色，像素1，像素2>*/   
    uint64_t package_type   :2;     /*<0-1>,0-分包1,1-分包2，此处为1*/    
    uint64_t state          :2;     /*0-停止，1-暂停，2-运行，3-continue*/
    uint64_t effect_type    :8;     /*0-127*/
    #else
    #endif
}bt_color_move_I_type2_cct_arg_t;
typedef __packed struct{    
    #if (BYTE_ORDER == LITTLE_ENDIAN)
    uint64_t reserve        :11;    /*预留*/
    uint64_t cct            :9;     /*<30-400>-><1500-20000>,精度50K*/
    uint64_t sat            :7;     /*<0-100>*/
    uint64_t hue            :9;     /*<1-360>*/
    uint64_t light_mode     :2;     /*<0-2>,0-cct,1-hsi，2-black*/
    uint64_t brightness     :10;    /*<0-1000>,精度0.1%*/
    uint64_t serial         :4;     /*<0-2>-><底色，像素1，像素2>*/  
    uint64_t package_type   :2;     /*<0-1>,0-分包1,1-分包2，此处为1*/    
    uint64_t state          :2;     /*0-停止，1-暂停，2-运行，3-continue*/
    uint64_t effect_type    :8;     /*0-127*/
    #else
    #endif
}bt_color_move_I_type2_hsi_arg_t;
typedef __packed struct{    
    #if (BYTE_ORDER == LITTLE_ENDIAN)
    uint64_t reserve        :36;    /*预留*/
    uint64_t light_mode     :2;     /*<0-2>,0-cct,1-hsi，2-black*/
    uint64_t brightness     :10;    /*<0-1000>,精度0.1%*/
    uint64_t serial         :4;     /*<0-2>-><底色，像素1，像素2>*/   
    uint64_t package_type   :2;     /*<0-1>,0-分包1,1-分包2，此处为1*/    
    uint64_t state          :2;     /*0-停止，1-暂停，2-运行，3-continue*/
    uint64_t effect_type    :8;     /*0-127*/
    #else
    #endif
}bt_color_move_I_type2_black_arg_t;

typedef __packed union{
    bt_color_move_I_type2_cct_arg_t    cct;
    bt_color_move_I_type2_hsi_arg_t    hsi;
    bt_color_move_I_type2_black_arg_t  black;
}bt_color_move_I_type2_arg_t;

typedef __packed union{
    bt_color_move_I_type1_arg_t    base;
    bt_color_move_I_type2_arg_t    pixel;
}bt_color_move_I_arg_t;

/*0x03：color_move_II*/
typedef __packed struct{    
    #if (BYTE_ORDER == LITTLE_ENDIAN)
    uint64_t reserve        :35;    /*预留*/
    uint64_t spd            :10;    /*<1-640> -> <1cm/s - 640cm/s> 精度1cm/s*/
    uint64_t pixel_lenght   :3;     /*像素长度<0-2> -> <S/M/L>*/
    uint64_t dir            :2;     /*单组：0-左，1-右，2-来回；双组：0-交错，1-反弹*/
    uint64_t group          :2;     /*0-单组，1-双组*/
    uint64_t package_type   :2;     /*<0-1>,0-分包1,1-分包2，此处为1*/
    uint64_t state          :2;     /*0-停止，1-暂停，2-运行，3-continue*/
    uint64_t effect_type    :8;     /*0-127*/
    #else
    #endif
}bt_color_move_II_type1_arg_t;

typedef __packed struct{    
    #if (BYTE_ORDER == LITTLE_ENDIAN)
    uint64_t reserve        :19;    /*预留*/
    uint64_t duv            :8;     /*<0-200> 精度0.01,0->-1.0,200->1.0*/
    uint64_t cct            :9;     /*<30-400>-><1500-20000>,精度50K*/
    uint64_t light_mode     :2;     /*<0-2>,0-cct,1-hsi，2-black*/
    uint64_t brightness     :10;    /*<0-1000>,精度0.1%*/
    uint64_t serial         :4;     /*<0-4>-><底色，像素1...像素4>*/   
    uint64_t package_type   :2;     /*<0-1>,0-分包1,1-分包2，此处为1*/    
    uint64_t state          :2;     /*0-停止，1-暂停，2-运行，3-continue*/
    uint64_t effect_type    :8;     /*0-127*/
    #else
    #endif
}bt_color_move_II_type2_cct_arg_t;
typedef __packed struct{    
    #if (BYTE_ORDER == LITTLE_ENDIAN)
    uint64_t reserve        :11;    /*预留*/
    uint64_t cct            :9;     /*<30-400>-><1500-20000>,精度50K*/
    uint64_t sat            :7;     /*<0-100>*/
    uint64_t hue            :9;     /*<1-360>*/
    uint64_t light_mode     :2;     /*<0-2>,0-cct,1-hsi，2-black*/
    uint64_t brightness     :10;    /*<0-1000>,精度0.1%*/
    uint64_t serial         :4;     /*<0-4>-><底色，像素1...像素4>*/
    uint64_t package_type   :2;     /*<0-1>,0-分包1,1-分包2，此处为1*/    
    uint64_t state          :2;     /*0-停止，1-暂停，2-运行，3-continue*/
    uint64_t effect_type    :8;     /*0-127*/
    #else
    #endif
}bt_color_move_II_type2_hsi_arg_t;
typedef __packed struct{    
    #if (BYTE_ORDER == LITTLE_ENDIAN)
    uint64_t reserve        :36;    /*预留*/
    uint64_t light_mode     :2;     /*<0-2>,0-cct,1-hsi，2-black*/
    uint64_t brightness     :10;    /*<0-1000>,精度0.1%*/
    uint64_t serial         :4;     /*<0-4>-><底色，像素1...像素4>*/  
    uint64_t package_type   :2;     /*<0-1>,0-分包1,1-分包2，此处为1*/    
    uint64_t state          :2;     /*0-停止，1-暂停，2-运行，3-continue*/
    uint64_t effect_type    :8;     /*0-127*/
    #else
    #endif
}bt_color_move_II_type2_black_arg_t;

typedef __packed union{
    bt_color_move_II_type2_cct_arg_t    cct;
    bt_color_move_II_type2_hsi_arg_t    hsi;
    bt_color_move_II_type2_black_arg_t  black;
}bt_color_move_II_type2_arg_t;

typedef __packed union{
    bt_color_move_II_type1_arg_t    base;
    bt_color_move_II_type2_arg_t    pixel;
}bt_color_move_II_arg_t;

/*0x04：color_move_III*/
typedef __packed struct{    
    #if (BYTE_ORDER == LITTLE_ENDIAN)
    uint64_t reserve        :35;    /*预留*/
    uint64_t spd            :10;    /*<1-640> -> <1cm/s - 640cm/s> 精度1cm/s*/
    uint64_t pixel_lenght   :3;     /*像素长度<0-2> -> <S/M/L>*/
    uint64_t dir            :2;     /*单组：0-左，1-右，2-来回；双组：0-交错，1-反弹*/
    uint64_t group          :2;     /*0-单组，1-双组*/
    uint64_t package_type   :2;     /*<0-1>,0-分包1,1-分包2，此处为1*/
    uint64_t state          :2;     /*0-停止，1-暂停，2-运行，3-continue*/
    uint64_t effect_type    :8;     /*0-127*/
    #else
    #endif
}bt_color_move_III_type1_arg_t;

typedef __packed struct{    
    #if (BYTE_ORDER == LITTLE_ENDIAN)
    uint64_t reserve        :19;    /*预留*/
    uint64_t duv            :8;     /*<0-200> 精度0.01,0->-1.0,200->1.0*/
    uint64_t cct            :9;     /*<30-400>-><1500-20000>,精度50K*/
    uint64_t light_mode     :2;     /*<0-2>,0-cct,1-hsi，2-black*/
    uint64_t brightness     :10;    /*<0-1000>,精度0.1%*/
    uint64_t serial         :4;     /*<0-6>-><底色，像素1...像素6>*/   
    uint64_t package_type   :2;     /*<0-1>,0-分包1,1-分包2，此处为1*/    
    uint64_t state          :2;     /*0-停止，1-暂停，2-运行，3-continue*/
    uint64_t effect_type    :8;     /*0-127*/
    #else
    #endif
}bt_color_move_III_type2_cct_arg_t;
typedef __packed struct{    
    #if (BYTE_ORDER == LITTLE_ENDIAN)
    uint64_t reserve        :11;    /*预留*/
    uint64_t cct            :9;     /*<30-400>-><1500-20000>,精度50K*/
    uint64_t sat            :7;     /*<0-100>*/
    uint64_t hue            :9;     /*<1-360>*/
    uint64_t light_mode     :2;     /*<0-2>,0-cct,1-hsi，2-black*/
    uint64_t brightness     :10;    /*<0-1000>,精度0.1%*/
    uint64_t serial         :4;     /*<0-6>-><底色，像素1...像素6>*/  
    uint64_t package_type   :2;     /*<0-1>,0-分包1,1-分包2，此处为1*/    
    uint64_t state          :2;     /*0-停止，1-暂停，2-运行，3-continue*/
    uint64_t effect_type    :8;     /*0-127*/
    #else
    #endif
}bt_color_move_III_type2_hsi_arg_t;
typedef __packed struct{    
    #if (BYTE_ORDER == LITTLE_ENDIAN)
    uint64_t reserve        :36;    /*预留*/
    uint64_t light_mode     :2;     /*<0-2>,0-cct,1-hsi，2-black*/
    uint64_t brightness     :10;    /*<0-1000>,精度0.1%*/
    uint64_t serial         :4;     /*<0-6>-><底色，像素1...像素6>*/  
    uint64_t package_type   :2;     /*<0-1>,0-分包1,1-分包2，此处为1*/    
    uint64_t state          :2;     /*0-停止，1-暂停，2-运行，3-continue*/
    uint64_t effect_type    :8;     /*0-127*/
    #else
    #endif
}bt_color_move_III_type2_black_arg_t;

typedef __packed union{
    bt_color_move_III_type2_cct_arg_t    cct;
    bt_color_move_III_type2_hsi_arg_t    hsi;
    bt_color_move_III_type2_black_arg_t  black;
}bt_color_move_III_type2_arg_t;

typedef __packed union{
    bt_color_move_III_type1_arg_t    base;
    bt_color_move_III_type2_arg_t    pixel;
}bt_color_move_III_arg_t;

/*0x05：pixel_fire*/
typedef __packed struct{    
    #if (BYTE_ORDER == LITTLE_ENDIAN)
    uint64_t reserve        :40;    /*预留*/
    uint64_t dir            :2;     /*<0-1> 0-横向，1-纵向*/
    uint64_t frq            :10;    /*<1-100> -> <0.1hz - 10hz>精度0.1hz*/
    uint64_t package_type   :2;     /*<0-2>,0-分包1,1-分包2,2-分包3*/
    uint64_t state          :2;     /*0-停止，1-暂停，2-运行，3-continue*/
    uint64_t effect_type    :8;     /*0-127*/
    #else
    #endif
}bt_pixel_fire_type1_arg_t;

typedef __packed struct{    
    #if (BYTE_ORDER == LITTLE_ENDIAN)
    uint64_t reserve        :13;    /*预留*/
    uint64_t duv            :8;     /*<0-200> 精度0.01,0->-1.0,200->1.0*/
    uint64_t cct            :9;     /*<30-400>-><1500-20000>,精度50K*/
    uint64_t light_mode     :2;     /*<0-2>,0-cct,1-hsi，2-black*/
    uint64_t min_brightness :10;    /*<0-1000>最低亮度,精度0.1%*/ 
    uint64_t max_brightness :10;    /*<0-1000>最高亮度,精度0.1%*/ 
    uint64_t package_type   :2;     /*<0-1>,0-分包1,1-分包2,2-分包3*/    
    uint64_t state          :2;     /*0-停止，1-暂停，2-运行，3-continue*/
    uint64_t effect_type    :8;     /*0-127*/
    #else
    #endif
}bt_pixel_fire_type2_cct_arg_t;
typedef __packed struct{    
    #if (BYTE_ORDER == LITTLE_ENDIAN)
    uint64_t reserve        :5;    /*预留*/
    uint64_t cct            :9;     /*<30-400>-><1500-20000>,精度50K*/
    uint64_t sat            :7;     /*<0-100>*/
    uint64_t hue            :9;     /*<1-360>*/
    uint64_t light_mode     :2;     /*<0-2>,0-cct,1-hsi，2-black*/
    uint64_t min_brightness :10;    /*<0-1000>最低亮度,精度0.1%*/ 
    uint64_t max_brightness :10;    /*<0-1000>最高亮度,精度0.1%*/ 
    uint64_t package_type   :2;     /*<0-1>,0-分包1,1-分包2，,2-分包3*/    
    uint64_t state          :2;     /*0-停止，1-暂停，2-运行，3-continue*/
    uint64_t effect_type    :8;     /*0-127*/
    #else
    #endif
}bt_pixel_fire_type2_hsi_arg_t;
typedef __packed struct{    
    #if (BYTE_ORDER == LITTLE_ENDIAN)
    uint64_t reserve        :30;    /*预留*/
    uint64_t light_mode     :2;     /*<0-2>,0-cct,1-hsi，2-black*/
    uint64_t min_brightness :10;    /*<0-1000>最低亮度,精度0.1%*/ 
    uint64_t max_brightness :10;    /*<0-1000>最高亮度,精度0.1%*/ 
    uint64_t package_type   :2;     /*<0-1>,0-分包1,1-分包2,2-分包3*/    
    uint64_t state          :2;     /*0-停止，1-暂停，2-运行，3-continue*/
    uint64_t effect_type    :8;     /*0-127*/
    #else
    #endif
}bt_pixel_fire_type2_black_arg_t;

typedef __packed union{
    bt_pixel_fire_type2_cct_arg_t    cct;
    bt_pixel_fire_type2_hsi_arg_t    hsi;
    bt_pixel_fire_type2_black_arg_t  black;
}bt_pixel_fire_type2_arg_t;

typedef __packed struct{    
    #if (BYTE_ORDER == LITTLE_ENDIAN)
    uint64_t reserve        :23;    /*预留*/
    uint64_t duv            :8;     /*<0-200> 精度0.01,0->-1.0,200->1.0*/
    uint64_t cct            :9;     /*<30-400>-><1500-20000>,精度50K*/
    uint64_t light_mode     :2;     /*<0-2>,0-cct,1-hsi，2-black*/
    uint64_t brightness     :10;    /*<0-1000>,精度0.1%*/
    uint64_t package_type   :2;     /*<0-1>,0-分包1,1-分包2,2-分包3*/    
    uint64_t state          :2;     /*0-停止，1-暂停，2-运行，3-continue*/
    uint64_t effect_type    :8;     /*0-127*/
    #else
    #endif
}bt_pixel_fire_type3_cct_arg_t;
typedef __packed struct{    
    #if (BYTE_ORDER == LITTLE_ENDIAN)
    uint64_t reserve        :15;    /*预留*/
    uint64_t cct            :9;     /*<30-400>-><1500-20000>,精度50K*/
    uint64_t sat            :7;     /*<0-100>*/
    uint64_t hue            :9;     /*<1-360>*/
    uint64_t light_mode     :2;     /*<0-2>,0-cct,1-hsi，2-black*/
    uint64_t brightness     :10;    /*<0-1000>,精度0.1%*/
    uint64_t package_type   :2;     /*<0-1>,0-分包1,1-分包2,2-分包3*/    
    uint64_t state          :2;     /*0-停止，1-暂停，2-运行，3-continue*/
    uint64_t effect_type    :8;     /*0-127*/
    #else
    #endif
}bt_pixel_fire_type3_hsi_arg_t;
typedef __packed struct{    
    #if (BYTE_ORDER == LITTLE_ENDIAN)
    uint64_t reserve        :40;    /*预留*/
    uint64_t light_mode     :2;     /*<0-2>,0-cct,1-hsi，2-black*/
    uint64_t brightness     :10;    /*<0-1000>,精度0.1%*/
    uint64_t package_type   :2;     /*<0-1>,0-分包1,1-分包2,2-分包3*/    
    uint64_t state          :2;     /*0-停止，1-暂停，2-运行，3-continue*/
    uint64_t effect_type    :8;     /*0-127*/
    #else
    #endif
}bt_pixel_fire_type3_black_arg_t;

typedef __packed union{
    bt_pixel_fire_type3_cct_arg_t    cct;
    bt_pixel_fire_type3_hsi_arg_t    hsi;
    bt_pixel_fire_type3_black_arg_t  black;
}bt_pixel_fire_type3_arg_t;

typedef __packed union{
    bt_pixel_fire_type1_arg_t    base;
    bt_pixel_fire_type2_arg_t    pixel;
    bt_pixel_fire_type3_arg_t    base_color;
}bt_pixel_fire_arg_t;

/*0x06：color_move_m*/
typedef __packed struct{    
    #if (BYTE_ORDER == LITTLE_ENDIAN)
    uint64_t reserve        :32;    /*预留*/
    uint64_t spd            :10;    /*<1-640> -> <1cm/s - 640cm/s> 精度1cm/s*/
    uint64_t color_number   :3;     /*<1-6>颜色移动数量*/
    uint64_t pixel_lenght   :3;     /*像素长度<0-2> -> <S/M/L>*/
    uint64_t dir            :2;     /*单组：0-左，1-右，2-来回；双组：0-交错，1-反弹*/
    uint64_t group          :2;     /*0-单组，1-双组*/
    uint64_t package_type   :2;     /*<0-1>,0-分包1,1-分包2*/
    uint64_t state          :2;     /*0-停止，1-暂停，2-运行，3-continue*/
    uint64_t effect_type    :8;     /*0-127*/
    #else
    #endif
}bt_color_move_m_type1_arg_t;

typedef __packed struct{    
    #if (BYTE_ORDER == LITTLE_ENDIAN)
    uint64_t reserve        :19;    /*预留*/
    uint64_t duv            :8;     /*<0-200> 精度0.01,0->-1.0,200->1.0*/
    uint64_t cct            :9;     /*<30-400>-><1500-20000>,精度50K*/
    uint64_t light_mode     :2;     /*<0-2>,0-cct,1-hsi，2-black*/
    uint64_t brightness     :10;    /*<0-1000>,精度0.1%*/
    uint64_t serial         :4;     /*<0-12>-><底色，像素1...像素12>*/   
    uint64_t package_type   :2;     /*<0-1>,0-分包1,1-分包2*/    
    uint64_t state          :2;     /*0-停止，1-暂停，2-运行，3-continue*/
    uint64_t effect_type    :8;     /*0-127*/
    #else
    #endif
}bt_color_move_m_type2_cct_arg_t;
typedef __packed struct{    
    #if (BYTE_ORDER == LITTLE_ENDIAN)
    uint64_t reserve        :11;    /*预留*/
    uint64_t cct            :9;     /*<30-400>-><1500-20000>,精度50K*/
    uint64_t sat            :7;     /*<0-100>*/
    uint64_t hue            :9;     /*<1-360>*/
    uint64_t light_mode     :2;     /*<0-2>,0-cct,1-hsi，2-black*/
    uint64_t brightness     :10;    /*<0-1000>,精度0.1%*/
    uint64_t serial         :4;     /*<0-12>-><底色，像素1...像素12>*/    
    uint64_t package_type   :2;     /*<0-1>,0-分包1,1-分包2*/    
    uint64_t state          :2;     /*0-停止，1-暂停，2-运行，3-continue*/
    uint64_t effect_type    :8;     /*0-127*/
    #else
    #endif
}bt_color_move_m_type2_hsi_arg_t;
typedef __packed struct{    
    #if (BYTE_ORDER == LITTLE_ENDIAN)
    uint64_t reserve        :36;    /*预留*/
    uint64_t light_mode     :2;     /*<0-2>,0-cct,1-hsi，2-black*/
    uint64_t brightness     :10;    /*<0-1000>,精度0.1%*/
    uint64_t serial         :4;     /*<0-12>-><底色，像素1...像素12>*/    
    uint64_t package_type   :2;     /*<0-1>,0-分包1,1-分包2，此处为1*/    
    uint64_t state          :2;     /*0-停止，1-暂停，2-运行，3-continue*/
    uint64_t effect_type    :8;     /*0-127*/
    #else
    #endif
}bt_color_move_m_type2_black_arg_t;

typedef __packed union{
    bt_color_move_m_type2_cct_arg_t    cct;
    bt_color_move_m_type2_hsi_arg_t    hsi;
    bt_color_move_m_type2_black_arg_t  black;
}bt_color_move_m_type2_arg_t;

typedef __packed union{
    bt_color_move_m_type1_arg_t    base;
    bt_color_move_m_type2_arg_t    pixel;
}bt_color_move_m_arg_t;

/*0x07：rainbow*/
typedef __packed struct{    
    #if (BYTE_ORDER == LITTLE_ENDIAN)
    uint64_t reserve        :31;    /*预留*/
    uint64_t spd            :10;    /*<1-640> -> <1cm/s - 640cm/s> 精度1cm/s*/
    uint64_t dir            :3;     /*<0-4>-><自左向右,自右向左,汇聚,分离,左右循环>*/
    uint64_t brightness     :10;    /*<0-1000>,精度0.1%*/ 
    uint64_t state          :2;     /*0-停止，1-暂停，2-运行*/
    uint64_t effect_type    :8;     /*0-127*/
    #else
    #endif
}bt_rainbow_arg_t;

/*0x08：on_off*/
typedef __packed struct{    
    #if (BYTE_ORDER == LITTLE_ENDIAN)
    uint64_t reserve        :48;    /*预留*/
    uint64_t color          :3;     /*0-白色*/
    uint64_t dir            :3;     /*<0-1>-><头到尾，开机；尾到头，关机>*/
    uint64_t state          :2;     /*0-停止，1-运行*/
    uint64_t effect_type    :8;     /*0-127*/
    #else
    #endif
}bt_on_off_arg_t;

/*0x09：belt_fx*/
/*---像素灯带光效类型---*/
typedef enum{
    BT_BeltFX_Fade = 0,
    BT_BeltFX_Festival,
    BT_BeltFX_Marquee_I,
    BT_BeltFX_Marquee_II,
    BT_BeltFX_Star,
    BT_BeltFX_Aurora,
    BT_BeltFX_Race,
    BT_BeltFX_Train,
    BT_BeltFX_Snake,
    BT_BeltFX_Forest,
    BT_BeltFX_Fire,
    BT_BeltFX_Stream,
    BT_BeltFX_Game,
    BT_BeltFX_Timing,
    BT_BeltFX_Party,
    BT_BeltFX_Fireworks,
    BT_BeltFX_Pinball,
    BT_BeltFX_Clock,
    BT_BeltFX_Fuse,
    BT_BeltFX_Null,
}bt_belt_fx_type_enum;

/*0x00：belt_color_fade*/
typedef __packed struct{    
    #if (BYTE_ORDER == LITTLE_ENDIAN)
    uint64_t reserve        :35;    /*预留*/
    uint64_t spd            :4;     /*<1-10> 精度0.1秒*/
    uint64_t brightness     :10;    /*<0-100>,精度1%*/ 
    uint64_t state          :2;     /*0-停止，1-运行*/
    uint64_t belt_type      :5;     /*灯带光效类型*/
    uint64_t effect_type    :8;     /*0-127*/
    #else
    #endif
}bt_belt_color_fade_arg_t;

/*0x01：belt_festival*/
typedef __packed struct{    
    #if (BYTE_ORDER == LITTLE_ENDIAN)
    uint64_t reserve        :7;     /*预留*/
    uint64_t base_color     :9;     /*底色<1-360°>,511对应无色(灯不亮)*/
    uint64_t move_color     :9;     /*光柱颜色<1-360°>*/
    uint64_t frq            :4;     /*<0-10>-><1-11,11是随机>*/
    uint64_t dir            :3;     /*<0-4>-><头到尾，尾到头，头尾到中间，中间到两头，随机>*/
    uint64_t sat            :7;     /*<0-100>*/
    uint64_t brightness     :10;    /*<0-100>,精度1%*/ 
    uint64_t state          :2;     /*0-停止，1-运行*/
    uint64_t belt_type      :5;     /*灯带光效类型*/
    uint64_t effect_type    :8;     /*0-127*/
    #else
    #endif
}bt_belt_festival_arg_t;

/*0x02：belt_marquee_I*/
typedef __packed struct{    
    #if (BYTE_ORDER == LITTLE_ENDIAN)
    uint64_t reserve        :3;     /*预留*/
    uint64_t base_color     :9;     /*底色<1-360°>,511对应无色(灯不亮)*/
    uint64_t move_color     :9;     /*光柱颜色<1-360°>*/
    uint64_t length         :4;     /*<1-10>*/
    uint64_t spd            :4;     /*<1-10> 精度0.1秒*/
    uint64_t dir            :3;     /*<0-4>-><头到尾，尾到头，头尾到中间，中间到两头，随机>*/
    uint64_t sat            :7;     /*<0-100>*/
    uint64_t brightness     :10;    /*<0-100>,精度1%*/ 
    uint64_t state          :2;     /*0-停止，1-运行*/
    uint64_t belt_type      :5;     /*灯带光效类型*/
    uint64_t effect_type    :8;     /*0-127*/
    #else
    #endif
}bt_belt_marquee_I_arg_t;

/*0x03：belt_marquee_II*/
typedef __packed struct{    
    #if (BYTE_ORDER == LITTLE_ENDIAN)
    uint64_t reserve        :19;     /*预留*/
    uint64_t base_color     :9;     /*底色<1-360°>,511对应无色(灯不亮)*/
    uint64_t length         :4;     /*<1-10>*/
    uint64_t spd            :4;     /*<1-10> 精度0.1秒*/
    uint64_t dir            :3;     /*<0-4>-><头到尾，尾到头，头尾到中间，中间到两头，随机>*/
    uint64_t brightness     :10;    /*<0-100>,精度1%*/ 
    uint64_t state          :2;     /*0-停止，1-运行*/
    uint64_t belt_type      :5;     /*灯带光效类型*/
    uint64_t effect_type    :8;     /*0-127*/
    #else
    #endif
}bt_belt_marquee_II_arg_t;

/*0x04：belt_star*/
typedef __packed struct{    
    #if (BYTE_ORDER == LITTLE_ENDIAN)
    uint64_t reserve        :19;    /*预留*/
    uint64_t color          :9;     /*底色<1-360°>*/
    uint64_t frq            :4;     /*<0-10>-><1-11,11是随机>*/
    uint64_t sat            :7;     /*<0-100>*/
    uint64_t brightness     :10;    /*<0-100>,精度1%*/ 
    uint64_t state          :2;     /*0-停止，1-运行*/
    uint64_t belt_type      :5;     /*灯带光效类型*/
    uint64_t effect_type    :8;     /*0-127*/
    #else
    #endif
}bt_belt_star_arg_t;

/*0x05：belt_aurora*/
typedef __packed struct{    
    #if (BYTE_ORDER == LITTLE_ENDIAN)
    uint64_t reserve        :26;    /*预留*/
    uint64_t color          :9;     /*底色<1-360°>,511对应无色(灯不亮)*/
    uint64_t frq            :4;     /*<0-10>-><1-11,11是随机>*/
    uint64_t brightness     :10;    /*<0-100>,精度1%*/ 
    uint64_t state          :2;     /*0-停止，1-运行*/
    uint64_t belt_type      :5;     /*灯带光效类型*/
    uint64_t effect_type    :8;     /*0-127*/
    #else
    #endif
}bt_belt_aurora_arg_t;

/*0x06：belt_race*/
typedef __packed struct{    
    #if (BYTE_ORDER == LITTLE_ENDIAN)
    uint64_t reserve        :22;    /*预留*/
    uint64_t color          :9;     /*底色<1-360°>,511对应无色(灯不亮)*/
    uint64_t length         :4;     /*<1-10>*/
    uint64_t spd            :4;     /*<1-10> 精度0.1秒*/
    uint64_t brightness     :10;    /*<0-100>,精度1%*/ 
    uint64_t state          :2;     /*0-停止，1-运行*/
    uint64_t belt_type      :5;     /*灯带光效类型*/
    uint64_t effect_type    :8;     /*0-127*/
    #else
    #endif
}bt_belt_race_arg_t;

/*0x07：belt_train*/
typedef __packed struct{    
    #if (BYTE_ORDER == LITTLE_ENDIAN)
    uint64_t reserve        :3;     /*预留*/
    uint64_t base_color     :9;     /*底色<1-360°>,511对应无色(灯不亮)*/
    uint64_t move_color     :9;     /*光柱颜色<1-360°>*/
    uint64_t length         :4;     /*<1-10>*/
    uint64_t spd            :4;     /*<1-10> 精度0.1秒*/
    uint64_t dir            :3;     /*<0-4>-><头到尾，尾到头，头尾到中间，中间到两头，随机>*/
    uint64_t sat            :7;     /*<0-100>*/
    uint64_t brightness     :10;    /*<0-100>,精度1%*/ 
    uint64_t state          :2;     /*0-停止，1-运行*/
    uint64_t belt_type      :5;     /*灯带光效类型*/
    uint64_t effect_type    :8;     /*0-127*/
    #else
    #endif
}bt_belt_train_arg_t;

/*0x08：belt_snake*/
typedef __packed struct{    
    #if (BYTE_ORDER == LITTLE_ENDIAN)
    uint64_t reserve        :22;    /*预留*/
    uint64_t color          :9;     /*底色<1-360°>,511对应无色(灯不亮)*/
    uint64_t length         :4;     /*<1-10>*/
    uint64_t spd            :4;     /*<1-10> 精度0.1秒*/
    uint64_t brightness     :10;    /*<0-100>,精度1%*/ 
    uint64_t state          :2;     /*0-停止，1-运行*/
    uint64_t belt_type      :5;     /*灯带光效类型*/
    uint64_t effect_type    :8;     /*0-127*/
    #else
    #endif
}bt_belt_snake_arg_t;

/*0x09：belt_forest*/
typedef __packed struct{    
    #if (BYTE_ORDER == LITTLE_ENDIAN)
    uint64_t reserve        :32;    /*预留*/
    uint64_t spd            :4;     /*<1-10> 精度0.1秒*/
    uint64_t dir            :3;     /*<0-4>-><头到尾，尾到头，头尾到中间，中间到两头，随机>*/
    uint64_t brightness     :10;    /*<0-100>,精度1%*/ 
    uint64_t state          :2;     /*0-停止，1-运行*/
    uint64_t belt_type      :5;     /*灯带光效类型*/
    uint64_t effect_type    :8;     /*0-127*/
    #else
    #endif
}bt_belt_forest_arg_t;

/*0x0A：belt_fire*/
typedef __packed struct{    
    #if (BYTE_ORDER == LITTLE_ENDIAN)
    uint64_t reserve        :32;    /*预留*/
    uint64_t spd            :4;     /*<1-10> 精度0.1秒*/
    uint64_t dir            :3;     /*<0-4>-><头到尾，尾到头，头尾到中间，中间到两头，随机>*/
    uint64_t brightness     :10;    /*<0-100>,精度1%*/ 
    uint64_t state          :2;     /*0-停止，1-运行*/
    uint64_t belt_type      :5;     /*灯带光效类型*/
    uint64_t effect_type    :8;     /*0-127*/
    #else
    #endif
}bt_belt_fire_arg_t;

/*0x0B：belt_stream*/
typedef __packed struct{    
    #if (BYTE_ORDER == LITTLE_ENDIAN)
    uint64_t reserve        :32;    /*预留*/
    uint64_t spd            :4;     /*<1-10> 精度0.1秒*/
    uint64_t dir            :3;     /*<0-4>-><头到尾，尾到头，头尾到中间，中间到两头，随机>*/
    uint64_t brightness     :10;    /*<0-100>,精度1%*/ 
    uint64_t state          :2;     /*0-停止，1-运行*/
    uint64_t belt_type      :5;     /*灯带光效类型*/
    uint64_t effect_type    :8;     /*0-127*/
    #else
    #endif
}bt_belt_stream_arg_t;

/*0x0C：belt_game*/
typedef __packed struct{    
    #if (BYTE_ORDER == LITTLE_ENDIAN)
    uint64_t reserve        :28;    /*预留*/
    uint64_t length         :4;     /*<1-10>*/
    uint64_t spd            :4;     /*<1-10> 精度0.1秒*/
    uint64_t dir            :3;     /*<0-4>-><头到尾，尾到头，头尾到中间，中间到两头，随机>*/
    uint64_t brightness     :10;    /*<0-100>,精度1%*/ 
    uint64_t state          :2;     /*0-停止，1-运行*/
    uint64_t belt_type      :5;     /*灯带光效类型*/
    uint64_t effect_type    :8;     /*0-127*/
    #else
    #endif
}bt_belt_game_arg_t;

/*0x0D：belt_timing*/
typedef __packed struct{    
    #if (BYTE_ORDER == LITTLE_ENDIAN)
    uint64_t reserve        :19;    /*预留*/
    uint64_t color          :9;     /*底色<1-360°>,511对应无色(灯不亮)*/
    uint64_t length         :4;     /*<1-10>*/
    uint64_t spd            :4;     /*<1-10> 精度0.1秒*/
    uint64_t dir            :3;     /*<0-4>-><头到尾，尾到头，头尾到中间，中间到两头，随机>*/
    uint64_t brightness     :10;    /*<0-100>,精度1%*/ 
    uint64_t state          :2;     /*0-停止，1-运行*/
    uint64_t belt_type      :5;     /*灯带光效类型*/
    uint64_t effect_type    :8;     /*0-127*/
    #else
    #endif
}bt_belt_timing_arg_t;

/*0x0E：belt_party*/
typedef __packed struct{    
    #if (BYTE_ORDER == LITTLE_ENDIAN)
    uint64_t reserve        :39;    /*预留*/
    uint64_t brightness     :10;    /*<0-100>,精度1%*/ 
    uint64_t state          :2;     /*0-停止，1-运行*/
    uint64_t belt_type      :5;     /*灯带光效类型*/
    uint64_t effect_type    :8;     /*0-127*/
    #else
    #endif
}bt_belt_party_arg_t;

/*0x0F：belt_fireworks*/
typedef __packed struct{    
    #if (BYTE_ORDER == LITTLE_ENDIAN)
    uint64_t reserve        :31;    /*预留*/
    uint64_t length         :4;     /*<1-10>*/
    uint64_t spd            :4;     /*<1-10> 精度0.1秒*/
    uint64_t brightness     :10;    /*<0-100>,精度1%*/ 
    uint64_t state          :2;     /*0-停止，1-运行*/
    uint64_t belt_type      :5;     /*灯带光效类型*/
    uint64_t effect_type    :8;     /*0-127*/
    #else
    #endif
}bt_belt_fireworks_arg_t;

/*0x10：belt_pinball*/
typedef __packed struct{    
    #if (BYTE_ORDER == LITTLE_ENDIAN)
    uint64_t reserve        :26;    /*预留*/
    uint64_t color          :9;     /*底色<1-360°>,511对应无色(灯不亮)*/
    uint64_t spd            :4;     /*<1-10> 精度0.1秒*/
    uint64_t brightness     :10;    /*<0-100>,精度1%*/ 
    uint64_t state          :2;     /*0-停止，1-运行*/
    uint64_t belt_type      :5;     /*灯带光效类型*/
    uint64_t effect_type    :8;     /*0-127*/
    #else
    #endif
}bt_belt_pinball_arg_t;

/*0x11：belt_clock*/
typedef __packed struct{    
    #if (BYTE_ORDER == LITTLE_ENDIAN)
    uint64_t reserve        :6;     /*预留*/
    uint64_t base_color     :9;     /*底色<1-360°>,511对应无色(灯不亮)*/
    uint64_t move_color     :9;     /*光柱颜色<1-360°>*/
    uint64_t length         :4;     /*<1-10>*/
    uint64_t spd            :4;     /*<1-10> 精度0.1秒*/
    uint64_t sat            :7;     /*<0-100>*/
    uint64_t brightness     :10;    /*<0-100>,精度1%*/ 
    uint64_t state          :2;     /*0-停止，1-运行*/
    uint64_t belt_type      :5;     /*灯带光效类型*/
    uint64_t effect_type    :8;     /*0-127*/
    #else
    #endif
}bt_belt_clock_arg_t;

/*0x12：belt_fuse*/
typedef __packed struct{    
    #if (BYTE_ORDER == LITTLE_ENDIAN)
    uint64_t reserve        :17;     /*预留*/
    uint64_t base_color     :9;     /*底色<1-360°>,511对应无色(灯不亮)*/
    uint64_t move_color     :9;     /*光柱颜色<1-360°>*/
    uint64_t spd            :4;     /*<1-10> 精度0.1秒*/
    uint64_t brightness     :10;    /*<0-100>,精度1%*/ 
    uint64_t state          :2;     /*0-停止，1-运行*/
    uint64_t belt_type      :5;     /*灯带光效类型*/
    uint64_t effect_type    :8;     /*0-127*/
    #else
    #endif
}bt_belt_fuse_arg_t;

typedef __packed union{ 
    bt_belt_color_fade_arg_t    color_fade;
    bt_belt_festival_arg_t      festival;
    bt_belt_marquee_I_arg_t     marquee_I;
    bt_belt_marquee_II_arg_t    marquee_II;
    bt_belt_star_arg_t          star;
    bt_belt_aurora_arg_t        aurora;
    bt_belt_race_arg_t          race;
    bt_belt_train_arg_t         train;
    bt_belt_snake_arg_t         snake;
    bt_belt_forest_arg_t        forest;
    bt_belt_fire_arg_t          fire;
    bt_belt_stream_arg_t        stream;
    bt_belt_game_arg_t          game;
    bt_belt_timing_arg_t        timing;
    bt_belt_party_arg_t         party;
    bt_belt_fireworks_arg_t     fireworks;
    bt_belt_pinball_arg_t       pinball;
    bt_belt_clock_arg_t         clock;
    bt_belt_fuse_arg_t          fuse;
}bt_belt_fx_arg_t;

/*0x0A：music*/
typedef __packed struct{    
    #if (BYTE_ORDER == LITTLE_ENDIAN)
    uint64_t reserve        :54;    /*预留*/
    uint64_t state          :2;     /*0-停止，1-运行*/
    uint64_t effect_type    :8;     /*0-127*/
    #else
    #endif
}bt_music_arg_t;

/*--------EFFECT_CMD：0x21-------*/
typedef __packed union{  
	bt_color_cut_arg_t      color_cut;
    bt_color_replace_arg_t  color_replace;
    bt_color_move_I_arg_t   color_move_I;
    bt_color_move_II_arg_t  color_move_II;
    bt_color_move_III_arg_t color_move_III;
    bt_pixel_fire_arg_t     pixel_fire;
    bt_color_move_m_arg_t   color_move_m;
    bt_rainbow_arg_t        rainbow;
    bt_on_off_arg_t         on_off;
    bt_belt_fx_arg_t        belt_fx;
    bt_music_arg_t          music;
}bt_pixel_fx_arg_t;

/**************************************************************************************/
/*********************************二代光效*********************************************/
/**************************************************************************************/

/************************************光效的类型***************************************/
/*---二代光效类型---*/
typedef enum{
    BT_FX_II_Paparazzi_2 = 0,
    BT_FX_II_Lightning_2,
    BT_FX_II_TV_2,
    BT_FX_II_Fire_2,
    BT_FX_II_Strobe_2,
    BT_FX_II_Explosion_2,
    BT_FX_II_Fault_Bulb_2,
    BT_FX_II_Pulsing_2,
    BT_FX_II_Welding_2,
    BT_FX_II_Cop_Car_2,
    BT_FX_II_Party_Lights_2,
    BT_FX_II_Fireworks_2,
    BT_FX_II_Lightning_3,
    BT_FX_II_TV_3,
    BT_FX_II_Fire_3,
    BT_FX_II_Faulty_Bulb_3,
    BT_FX_II_Pulsing_3,
    BT_FX_II_Cop_Car_3,
    BT_FX_II_Null,
}bt_fx_2_type_enum;

/*---二代光效里的子类型---*/
typedef enum{
    BT_FX_II_Mode_CCT = 0,
    BT_FX_II_Mode_HSI,
    BT_FX_II_Mode_GEL,
    BT_FX_II_Mode_XY,
    BT_FX_II_Mode_SOUYRCE,
    BT_FX_II_Mode_RGB,
    BT_FX_II_Mode_Null,
}bt_fx_2_mode_enum;

typedef enum{
    BT_FX_II_Mode_CCT_Range,
    BT_FX_II_Mode_HSI_Range,
    BT_FX_II_Mode_Range_Null,
}bt_fx_2_range_mode_enum;

/*---二代光效触发类型---*/
typedef enum{
    BT_FX_II_Trigger_None = 0,
    BT_FX_II_Trigger_Once,
    BT_FX_II_Trigger_Continue
}bt_fx_2_trigger_enum;

/*0x00：paparazzi_II*/
typedef __packed struct{    
    #if (BYTE_ORDER == LITTLE_ENDIAN)
    uint64_t reserve        :25;    /*预留*/
    uint64_t min_gap_time   :9;     /*最小间隔时间：<1-300> -> <0.1-30秒>，调节精度0.1秒*/
    uint64_t gap_time       :9;    /*最小间隔时间：<1-300> -> <0.1-30秒>，调节精度0.1秒*/ 
    uint64_t brightness     :10;    /*<0-1000>,精度0.1%*/  
    uint64_t package_type   :1;     /*0->base,1-pixel*/
    uint64_t state          :2;     /*0-停止，1-运行，3-continue*/
    uint64_t effect_type    :8;     /*0-127*/
    #else
    #endif
}bt_paparazzi_II_type1_arg_t;

typedef __packed struct{    
    #if (BYTE_ORDER == LITTLE_ENDIAN)
    uint64_t reserve        :33;    /*预留*/
    uint64_t duv            :8;     /*<0-200> 精度0.01,0->-1.0,200->1.0*/
    uint64_t cct            :9;     /*<30-400>-><1500-20000>,精度50K*/
    uint64_t light_mode     :3;     /*<0-5>-><cct,hsi,gel,xy,source,rgb>*/
    uint64_t package_type   :1;     /*0->base,1-pixel*/
    uint64_t state          :2;     /*0-停止，1-运行，3-continue*/
    uint64_t effect_type    :8;     /*0-127*/
    #else
    #endif
}bt_paparazzi_II_type2_cct_arg_t;
typedef __packed struct{    
    #if (BYTE_ORDER == LITTLE_ENDIAN)
    uint64_t reserve        :25;    /*预留*/
    uint64_t cct            :9;     /*<30-400>-><1500-20000>,精度50K*/
    uint64_t sat            :7;     /*<0-100>*/
    uint64_t hue            :9;     /*<1-360>*/
    uint64_t light_mode     :3;     /*<0-5>-><cct,hsi,gel,xy,source,rgb>*/
    uint64_t package_type   :1;     /*0->base,1-pixel*/
    uint64_t state          :2;     /*0-停止，1-运行，3-continue*/
    uint64_t effect_type    :8;     /*0-127*/
    #else
    #endif
}bt_paparazzi_II_type2_hsi_arg_t;
typedef __packed struct{    
    #if (BYTE_ORDER == LITTLE_ENDIAN)
    uint64_t reserve        :26;     /*预留*/
    uint64_t color          :10;    /*色纸颜色<0-1023>*/
    uint64_t type           :4;     /*色纸类型<0-15>*/
    uint64_t origin         :1;     /*0-LEE,1-ROSCO*/
    uint64_t cct            :9;     /*<30-400>-><1500-20000>,精度50K*/
    uint64_t light_mode     :3;     /*<0-5>-><cct,hsi,gel,xy,source,rgb>*/
    uint64_t package_type   :1;     /*0->base,1-pixel*/
    uint64_t state          :2;     /*0-停止，1-运行，3-continue*/
    uint64_t effect_type    :8;     /*0-127*/
    #else
    #endif
}bt_paparazzi_II_type2_gel_arg_t;
typedef __packed struct{    
    #if (BYTE_ORDER == LITTLE_ENDIAN)
    uint64_t reserve        :22;     /*预留*/
    uint64_t y              :14;    /*<0-10000>,精度0.0001*/
    uint64_t x              :14;    /*<0-10000>,精度0.0001*/
    uint64_t light_mode     :3;     /*<0-5>-><cct,hsi,gel,xy,source,rgb>*/
    uint64_t package_type   :1;     /*0->base,1-pixel*/ 
    uint64_t state          :2;     /*0-停止，1-运行，3-continue*/
    uint64_t effect_type    :8;     /*0-127*/
    #else
    #endif
}bt_paparazzi_II_type2_xy_arg_t;
typedef __packed struct{    
    #if (BYTE_ORDER == LITTLE_ENDIAN)
    uint64_t reserve        :26;     /*预留*/
    uint64_t y              :8;     /*<0-100>,精度0.0001,调节范围 -0.005 - +0.005*/
    uint64_t x              :8;     /*<0-100>,精度0.0001,调节范围 -0.005 - +0.005*/
    uint64_t type           :8;     /*<0-256>，光源类型*/
    uint64_t light_mode     :3;     /*<0-5>-><cct,hsi,gel,xy,source,rgb>*/
    uint64_t package_type   :1;     /*0->base,1-pixel*/ 
    uint64_t state          :2;     /*0-停止，1-运行，3-continue*/
    uint64_t effect_type    :8;     /*0-127*/
    #else
    #endif
}bt_paparazzi_II_type2_source_arg_t;
typedef __packed struct{    
    #if (BYTE_ORDER == LITTLE_ENDIAN)
    uint64_t reserve        :20;     /*预留*/
    uint64_t b              :10;    /*<0-100>,精度0.0001,调节范围 -0.005 - +0.005*/
    uint64_t g              :10;    /*<0-100>,精度0.0001,调节范围 -0.005 - +0.005*/
    uint64_t r              :10;    /*<0-256>，光源类型*/
    uint64_t light_mode     :3;     /*<0-5>-><cct,hsi,gel,xy,source,rgb>*/
    uint64_t package_type   :1;     /*0->base,1-pixel*/ 
    uint64_t state          :2;     /*0-停止，1-运行，3-continue*/
    uint64_t effect_type    :8;     /*0-127*/
    #else
    #endif
}bt_paparazzi_II_type2_rgb_arg_t;

typedef __packed union{  
    bt_paparazzi_II_type2_cct_arg_t       cct;
    bt_paparazzi_II_type2_hsi_arg_t       hsi;
    bt_paparazzi_II_type2_gel_arg_t       gel;
    bt_paparazzi_II_type2_xy_arg_t        xy;
    bt_paparazzi_II_type2_source_arg_t    source;
    bt_paparazzi_II_type2_rgb_arg_t       rgb;
}bt_paparazzi_II_type2_arg_t;

typedef __packed union{
    bt_paparazzi_II_type1_arg_t    base;
    bt_paparazzi_II_type2_arg_t    pixel;
}bt_paparazzi_II_arg_t;

/*0x01：lightning_II*/
typedef __packed struct{    
    #if (BYTE_ORDER == LITTLE_ENDIAN)
    uint64_t reserve        :16;    /*预留*/
    uint64_t duv            :8;     /*<0-200> 精度0.01,0->-1.0,200->1.0*/
    uint64_t cct            :9;     /*<30-400>-><1500-20000>,精度50K*/
    uint64_t light_mode     :3;     /*<0-1>-><cct,hsi>*/
    uint64_t spd            :4;     /*<0-10>-><1-11,11为随机>*/
    uint64_t frq            :4;     /*<0-10>-><1-11,11为随机>*/
    uint64_t brightness     :10;    /*<0-1000>,精度0.1%*/  
    uint64_t state          :2;     /*0-停止，1-单次，2-循环*/
    uint64_t effect_type    :8;     /*0-127*/
    #else
    #endif
}bt_lightning_II_cct_arg_t;
typedef __packed struct{    
    #if (BYTE_ORDER == LITTLE_ENDIAN)
    uint64_t reserve        :8;    /*预留*/
    uint64_t cct            :9;     /*<30-400>-><1500-20000>,精度50K*/
    uint64_t sat            :7;     /*<0-100>*/
    uint64_t hue            :9;     /*<1-360>*/
    uint64_t light_mode     :3;     /*<0-1>-><cct,hsi>*/
    uint64_t spd            :4;     /*<0-10>-><1-11,11为随机>*/
    uint64_t frq            :4;     /*<0-10>-><1-11,11为随机>*/
    uint64_t brightness     :10;    /*<0-1000>,精度0.1%*/  
    uint64_t state          :2;     /*0-停止，1-运行*/
    uint64_t effect_type    :8;     /*0-127*/
    #else
    #endif
}bt_lightning_II_hsi_arg_t;

typedef __packed union{  
    bt_lightning_II_cct_arg_t       cct;
    bt_lightning_II_hsi_arg_t       hsi;
}bt_lightning_II_arg_t;

/*0x02：tv_II*/
typedef __packed struct{    
    #if (BYTE_ORDER == LITTLE_ENDIAN)
    uint64_t reserve        :11;    /*预留*/
    uint64_t duv            :8;     /*<0-200> 精度0.01,0->-1.0,200->1.0*/
    uint64_t min_cct        :9;     /*<30-400>-><1500-20000>,精度50K*/
    uint64_t max_cct        :9;     /*<30-400>-><1500-20000>,精度50K*/
    uint64_t light_mode     :3;     /*<0-1>-><cct,hsi>*/
    uint64_t spd            :4;     /*<0-10>-><1-11,11为随机>*/
    uint64_t brightness     :10;    /*<0-1000>,精度0.1%*/  
    uint64_t state          :2;     /*0-停止，1-运行*/
    uint64_t effect_type    :8;     /*0-127*/
    #else
    #endif
}bt_tv_II_cct_arg_t;
typedef __packed struct{    
    #if (BYTE_ORDER == LITTLE_ENDIAN)
    uint64_t reserve        :3;    /*预留*/
    uint64_t cct            :9;     /*<30-400>-><1500-20000>,精度50K*/
    uint64_t sat            :7;     /*<0-100>*/
    uint64_t min_hue        :9;     /*<1-360>*/
    uint64_t max_hue        :9;     /*<1-360>*/
    uint64_t light_mode     :3;     /*<0-1>-><cct,hsi>*/
    uint64_t spd            :4;     /*<0-10>-><1-11,11为随机>*/
    uint64_t brightness     :10;    /*<0-1000>,精度0.1%*/  
    uint64_t state          :2;     /*0-停止，1-运行*/
    uint64_t effect_type    :8;     /*0-127*/
    #else
    #endif
}bt_tv_II_hsi_arg_t;

typedef __packed union{  
    bt_tv_II_cct_arg_t       cct;
    bt_tv_II_hsi_arg_t       hsi;
}bt_tv_II_arg_t;

/*0x03：fire_II*/
typedef __packed struct{    
    #if (BYTE_ORDER == LITTLE_ENDIAN)
    uint64_t reserve        :11;    /*预留*/
    uint64_t duv            :8;     /*<0-200> 精度0.01,0->-1.0,200->1.0*/
    uint64_t min_cct        :9;     /*<30-400>-><1500-20000>,精度50K*/
    uint64_t max_cct        :9;     /*<30-400>-><1500-20000>,精度50K*/
    uint64_t light_mode     :3;     /*<0-5>-><cct,hsi>*/
    uint64_t spd            :4;     /*<0-10>-><1-11,11为随机>*/
    uint64_t brightness     :10;    /*<0-1000>,精度0.1%*/ 
    uint64_t state          :2;     /*0-停止，1-运行*/
    uint64_t effect_type    :8;     /*0-127*/
    #else
    #endif
}bt_fire_II_cct_arg_t;
typedef __packed struct{    
    #if (BYTE_ORDER == LITTLE_ENDIAN)
    uint64_t reserve        :3;    /*预留*/
    uint64_t cct            :9;     /*<30-400>-><1500-20000>,精度50K*/
    uint64_t sat            :7;     /*<0-100>*/
    uint64_t min_hue        :9;     /*<1-360>*/
    uint64_t max_hue        :9;     /*<1-360>*/
    uint64_t light_mode     :3;     /*<0-5>-><cct,hsi>*/
    uint64_t spd            :4;     /*<0-10>-><1-11,11为随机>*/
    uint64_t brightness     :10;    /*<0-1000>,精度0.1%*/ 
    uint64_t state          :2;     /*0-停止，1-运行*/
    uint64_t effect_type    :8;     /*0-127*/
    #else
    #endif
}bt_fire_II_hsi_arg_t;

typedef __packed union{
    bt_fire_II_cct_arg_t    cct;
    bt_fire_II_hsi_arg_t    hsi;
}bt_fire_II_arg_t;

/*0x04：strobe_II*/
typedef __packed struct{    
    #if (BYTE_ORDER == LITTLE_ENDIAN)
    uint64_t reserve        :20;    /*预留*/
    uint64_t duv            :8;     /*<0-200> 精度0.01,0->-1.0,200->1.0*/
    uint64_t cct            :9;     /*<30-400>-><1500-20000>,精度50K*/
    uint64_t light_mode     :3;     /*<0-5>-><cct,hsi,gel,xy,source,rgb>*/
    uint64_t spd            :4;     /*<0-10>-><1-11,11为随机>*/
    uint64_t brightness     :10;    /*<0-1000>,精度0.1%*/  
    uint64_t state          :2;     /*0-停止，1-运行*/
    uint64_t effect_type    :8;     /*0-127*/
    #else
    #endif
}bt_strobe_II_cct_arg_t;
typedef __packed struct{    
    #if (BYTE_ORDER == LITTLE_ENDIAN)
    uint64_t reserve        :12;    /*预留*/
    uint64_t cct            :9;     /*<30-400>-><1500-20000>,精度50K*/
    uint64_t sat            :7;     /*<0-100>*/
    uint64_t hue            :9;     /*<1-360>*/
    uint64_t light_mode     :3;     /*<0-5>-><cct,hsi,gel,xy,source,rgb>*/
    uint64_t spd            :4;     /*<0-10>-><1-11,11为随机>*/
    uint64_t brightness     :10;    /*<0-1000>,精度0.1%*/  
    uint64_t state          :2;     /*0-停止，1-运行*/
    uint64_t effect_type    :8;     /*0-127*/
    #else
    #endif
}bt_strobe_II_hsi_arg_t;
typedef __packed struct{    
    #if (BYTE_ORDER == LITTLE_ENDIAN)
    uint64_t reserve        :13;     /*预留*/
    uint64_t color          :10;    /*色纸颜色<0-1023>*/
    uint64_t type           :4;     /*色纸类型<0-15>*/
    uint64_t origin         :1;     /*0-LEE,1-ROSCO*/
    uint64_t cct            :9;     /*<30-400>-><1500-20000>,精度50K*/
    uint64_t light_mode     :3;     /*<0-5>-><cct,hsi,gel,xy,source,rgb>*/
    uint64_t spd            :4;     /*<0-10>-><1-11,11为随机>*/
    uint64_t brightness     :10;    /*<0-1000>,精度0.1%*/  
    uint64_t state          :2;     /*0-停止，1-运行*/
    uint64_t effect_type    :8;     /*0-127*/
    #else
    #endif
}bt_strobe_II_gel_arg_t;
typedef __packed struct{    
    #if (BYTE_ORDER == LITTLE_ENDIAN)
    uint64_t reserve        :9;     /*预留*/
    uint64_t y              :14;    /*<0-10000>,精度0.0001*/
    uint64_t x              :14;    /*<0-10000>,精度0.0001*/
    uint64_t light_mode     :3;     /*<0-5>-><cct,hsi,gel,xy,source,rgb>*/
    uint64_t spd            :4;     /*<0-10>-><1-11,11为随机>*/
    uint64_t brightness     :10;    /*<0-1000>,精度0.1%*/  
    uint64_t state          :2;     /*0-停止，1-运行*/
    uint64_t effect_type    :8;     /*0-127*/
    #else
    #endif
}bt_strobe_II_xy_arg_t;
typedef __packed struct{    
    #if (BYTE_ORDER == LITTLE_ENDIAN)
    uint64_t reserve        :13;    /*预留*/
    uint64_t y              :8;     /*<0-100>,精度0.0001,调节范围 -0.005 - +0.005*/
    uint64_t x              :8;     /*<0-100>,精度0.0001,调节范围 -0.005 - +0.005*/
    uint64_t type           :8;     /*<0-256>，光源类型*/
    uint64_t light_mode     :3;     /*<0-5>-><cct,hsi,gel,xy,source,rgb>*/
    uint64_t spd            :4;     /*<0-10>-><1-11,11为随机>*/
    uint64_t brightness     :10;    /*<0-1000>,精度0.1%*/  
    uint64_t state          :2;     /*0-停止，1-运行*/
    uint64_t effect_type    :8;     /*0-127*/
    #else
    #endif
}bt_strobe_II_source_arg_t;
typedef __packed struct{    
    #if (BYTE_ORDER == LITTLE_ENDIAN)
    uint64_t reserve        :7;     /*预留*/
    uint64_t b              :10;    /*<0-100>,精度0.0001,调节范围 -0.005 - +0.005*/
    uint64_t g              :10;    /*<0-100>,精度0.0001,调节范围 -0.005 - +0.005*/
    uint64_t r              :10;    /*<0-256>，光源类型*/
    uint64_t light_mode     :3;     /*<0-5>-><cct,hsi,gel,xy,source,rgb>*/
    uint64_t spd            :4;     /*<0-10>-><1-11,11为随机>*/
    uint64_t brightness     :10;    /*<0-1000>,精度0.1%*/  
    uint64_t state          :2;     /*0-停止，1-运行*/
    uint64_t effect_type    :8;     /*0-127*/
    #else
    #endif
}bt_strobe_II_rgb_arg_t;

typedef __packed union{  
    bt_strobe_II_cct_arg_t       cct;
    bt_strobe_II_hsi_arg_t       hsi;
    bt_strobe_II_gel_arg_t       gel;
    bt_strobe_II_xy_arg_t        xy;
    bt_strobe_II_source_arg_t    source;
    bt_strobe_II_rgb_arg_t       rgb;
}bt_strobe_II_arg_t;

/*0x05：explosion_II*/
typedef __packed struct{    
    #if (BYTE_ORDER == LITTLE_ENDIAN)
    uint64_t reserve        :20;    /*预留*/
    uint64_t duv            :8;     /*<0-200> 精度0.01,0->-1.0,200->1.0*/
    uint64_t cct            :9;     /*<30-400>-><1500-20000>,精度50K*/
    uint64_t light_mode     :3;     /*<0-5>-><cct,hsi,gel,xy,source,rgb>*/
    uint64_t decay          :4;     /*<0-9>-><1-10>*/
    uint64_t brightness     :10;    /*<0-1000>,精度0.1%*/  
    uint64_t state          :2;     /*0-停止，1-单次*/
    uint64_t effect_type    :8;     /*0-127*/
    #else
    #endif
}bt_explosion_II_cct_arg_t;
typedef __packed struct{    
    #if (BYTE_ORDER == LITTLE_ENDIAN)
    uint64_t reserve        :12;    /*预留*/
    uint64_t cct            :9;     /*<30-400>-><1500-20000>,精度50K*/
    uint64_t sat            :7;     /*<0-100>*/
    uint64_t hue            :9;     /*<1-360>*/
    uint64_t light_mode     :3;     /*<0-5>-><cct,hsi,gel,xy,source,rgb>*/
    uint64_t decay          :4;     /*<0-9>-><1-10>*/
    uint64_t brightness     :10;    /*<0-1000>,精度0.1%*/  
    uint64_t state          :2;     /*0-停止，1-单次*/
    uint64_t effect_type    :8;     /*0-127*/
    #else
    #endif
}bt_explosion_II_hsi_arg_t;
typedef __packed struct{    
    #if (BYTE_ORDER == LITTLE_ENDIAN)
    uint64_t reserve        :13;     /*预留*/
    uint64_t color          :10;    /*色纸颜色<0-1023>*/
    uint64_t type           :4;     /*色纸类型<0-15>*/
    uint64_t origin         :1;     /*0-LEE,1-ROSCO*/
    uint64_t cct            :9;     /*<30-400>-><1500-20000>,精度50K*/
    uint64_t light_mode     :3;     /*<0-5>-><cct,hsi,gel,xy,source,rgb>*/
    uint64_t decay          :4;     /*<0-9>-><1-10>*/
    uint64_t brightness     :10;    /*<0-1000>,精度0.1%*/  
    uint64_t state          :2;     /*0-停止，1-单次*/
    uint64_t effect_type    :8;     /*0-127*/
    #else
    #endif
}bt_explosion_II_gel_arg_t;
typedef __packed struct{    
    #if (BYTE_ORDER == LITTLE_ENDIAN)
    uint64_t reserve        :9;     /*预留*/
    uint64_t y              :14;    /*<0-10000>,精度0.0001*/
    uint64_t x              :14;    /*<0-10000>,精度0.0001*/
    uint64_t light_mode     :3;     /*<0-5>-><cct,hsi,gel,xy,source,rgb>*/
    uint64_t decay          :4;     /*<0-9>-><1-10>*/
    uint64_t brightness     :10;    /*<0-1000>,精度0.1%*/  
    uint64_t state          :2;     /*0-停止，1-单次*/
    uint64_t effect_type    :8;     /*0-127*/
    #else
    #endif
}bt_explosion_II_xy_arg_t;
typedef __packed struct{    
    #if (BYTE_ORDER == LITTLE_ENDIAN)
    uint64_t reserve        :13;    /*预留*/
    uint64_t y              :8;     /*<0-100>,精度0.0001,调节范围 -0.005 - +0.005*/
    uint64_t x              :8;     /*<0-100>,精度0.0001,调节范围 -0.005 - +0.005*/
    uint64_t type           :8;     /*<0-256>，光源类型*/
    uint64_t light_mode     :3;     /*<0-5>-><cct,hsi,gel,xy,source,rgb>*/
    uint64_t decay          :4;     /*<0-9>-><1-10>*/
    uint64_t brightness     :10;    /*<0-1000>,精度0.1%*/  
    uint64_t state          :2;     /*0-停止，1-单次*/
    uint64_t effect_type    :8;     /*0-127*/
    #else
    #endif
}bt_explosion_II_source_arg_t;
typedef __packed struct{    
    #if (BYTE_ORDER == LITTLE_ENDIAN)
    uint64_t reserve        :7;     /*预留*/
    uint64_t b              :10;    /*<0-100>,精度0.0001,调节范围 -0.005 - +0.005*/
    uint64_t g              :10;    /*<0-100>,精度0.0001,调节范围 -0.005 - +0.005*/
    uint64_t r              :10;    /*<0-256>，光源类型*/
    uint64_t light_mode     :3;     /*<0-5>-><cct,hsi,gel,xy,source,rgb>*/
    uint64_t decay          :4;     /*<0-9>-><1-10>*/
    uint64_t brightness     :10;    /*<0-1000>,精度0.1%*/  
    uint64_t state          :2;     /*0-停止，1-单次*/
    uint64_t effect_type    :8;     /*0-127*/
    #else
    #endif
}bt_explosion_II_rgb_arg_t;

typedef __packed union{  
    bt_explosion_II_cct_arg_t       cct;
    bt_explosion_II_hsi_arg_t       hsi;
    bt_explosion_II_gel_arg_t       gel;
    bt_explosion_II_xy_arg_t        xy;
    bt_explosion_II_source_arg_t    source;
    bt_explosion_II_rgb_arg_t       rgb;
}bt_explosion_II_arg_t;

/*0x06：fault_bulb_II*/
typedef __packed struct{    
    #if (BYTE_ORDER == LITTLE_ENDIAN)
    uint64_t reserve        :16;    /*预留*/
    uint64_t duv            :8;     /*<0-200> 精度0.01,0->-1.0,200->1.0*/
    uint64_t cct            :9;     /*<30-400>-><1500-20000>,精度50K*/
    uint64_t light_mode     :3;     /*<0-5>-><cct,hsi,gel,xy,source,rgb>*/
    uint64_t frq            :4;     /*<0-9>-><1-10>*/
    uint64_t spd            :4;     /*<0-9>-><1-10>*/
    uint64_t brightness     :10;    /*<0-1000>,精度0.1%*/  
    uint64_t state          :2;     /*0-停止，1-运行*/
    uint64_t effect_type    :8;     /*0-127*/
    #else
    #endif
}bt_fault_bulb_II_cct_arg_t;
typedef __packed struct{    
    #if (BYTE_ORDER == LITTLE_ENDIAN)
    uint64_t reserve        :8;    /*预留*/
    uint64_t cct            :9;     /*<30-400>-><1500-20000>,精度50K*/
    uint64_t sat            :7;     /*<0-100>*/
    uint64_t hue            :9;     /*<1-360>*/
    uint64_t light_mode     :3;     /*<0-5>-><cct,hsi,gel,xy,source,rgb>*/
    uint64_t frq            :4;     /*<0-9>-><1-10>*/
    uint64_t spd            :4;     /*<0-9>-><1-10>*/
    uint64_t brightness     :10;    /*<0-1000>,精度0.1%*/  
    uint64_t state          :2;     /*0-停止，1-运行*/
    uint64_t effect_type    :8;     /*0-127*/
    #else
    #endif
}bt_fault_bulb_II_hsi_arg_t;
typedef __packed struct{    
    #if (BYTE_ORDER == LITTLE_ENDIAN)
    uint64_t reserve        :9;     /*预留*/
    uint64_t color          :10;    /*色纸颜色<0-1023>*/
    uint64_t type           :4;     /*色纸类型<0-15>*/
    uint64_t origin         :1;     /*0-LEE,1-ROSCO*/
    uint64_t cct            :9;     /*<30-400>-><1500-20000>,精度50K*/
    uint64_t light_mode     :3;     /*<0-5>-><cct,hsi,gel,xy,source,rgb>*/
    uint64_t frq            :4;     /*<0-9>-><1-10>*/
    uint64_t spd            :4;     /*<0-9>-><1-10>*/
    uint64_t brightness     :10;    /*<0-1000>,精度0.1%*/  
    uint64_t state          :2;     /*0-停止，1-运行*/
    uint64_t effect_type    :8;     /*0-127*/
    #else
    #endif
}bt_fault_bulb_II_gel_arg_t;
typedef __packed struct{    
    #if (BYTE_ORDER == LITTLE_ENDIAN)
    uint64_t reserve        :5;     /*预留*/
    uint64_t y              :14;    /*<0-10000>,精度0.0001*/
    uint64_t x              :14;    /*<0-10000>,精度0.0001*/
    uint64_t light_mode     :3;     /*<0-5>-><cct,hsi,gel,xy,source,rgb>*/
    uint64_t frq            :4;     /*<0-9>-><1-10>*/
    uint64_t spd            :4;     /*<0-9>-><1-10>*/
    uint64_t brightness     :10;    /*<0-1000>,精度0.1%*/  
    uint64_t state          :2;     /*0-停止，1-运行*/
    uint64_t effect_type    :8;     /*0-127*/
    #else
    #endif
}bt_fault_bulb_II_xy_arg_t;
typedef __packed struct{    
    #if (BYTE_ORDER == LITTLE_ENDIAN)
    uint64_t reserve        :9;    /*预留*/
    uint64_t y              :8;     /*<0-100>,精度0.0001,调节范围 -0.005 - +0.005*/
    uint64_t x              :8;     /*<0-100>,精度0.0001,调节范围 -0.005 - +0.005*/
    uint64_t type           :8;     /*<0-256>，光源类型*/
    uint64_t light_mode     :3;     /*<0-5>-><cct,hsi,gel,xy,source,rgb>*/
    uint64_t frq            :4;     /*<0-9>-><1-10>*/
    uint64_t spd            :4;     /*<0-9>-><1-10>*/
    uint64_t brightness     :10;    /*<0-1000>,精度0.1%*/  
    uint64_t state          :2;     /*0-停止，1-运行*/
    uint64_t effect_type    :8;     /*0-127*/
    #else
    #endif
}bt_fault_bulb_II_source_arg_t;
typedef __packed struct{    
    #if (BYTE_ORDER == LITTLE_ENDIAN)
    uint64_t reserve        :3;     /*预留*/
    uint64_t b              :10;    /*<0-100>,精度0.0001,调节范围 -0.005 - +0.005*/
    uint64_t g              :10;    /*<0-100>,精度0.0001,调节范围 -0.005 - +0.005*/
    uint64_t r              :10;    /*<0-256>，光源类型*/
    uint64_t light_mode     :3;     /*<0-5>-><cct,hsi,gel,xy,source,rgb>*/
    uint64_t frq            :4;     /*<0-9>-><1-10>*/
    uint64_t spd            :4;     /*<0-9>-><1-10>*/
    uint64_t brightness     :10;    /*<0-1000>,精度0.1%*/  
    uint64_t state          :2;     /*0-停止，1-运行*/
    uint64_t effect_type    :8;     /*0-127*/
    #else
    #endif
}bt_fault_bulb_II_rgb_arg_t;

typedef __packed union{  
    bt_fault_bulb_II_cct_arg_t       cct;
    bt_fault_bulb_II_hsi_arg_t       hsi;
    bt_fault_bulb_II_gel_arg_t       gel;
    bt_fault_bulb_II_xy_arg_t        xy;
    bt_fault_bulb_II_source_arg_t    source;
    bt_fault_bulb_II_rgb_arg_t       rgb;
}bt_fault_bulb_II_arg_t;


/*0x07：pulsing_II*/
typedef __packed struct{    
    #if (BYTE_ORDER == LITTLE_ENDIAN)
    uint64_t reserve        :16;    /*预留*/
    uint64_t duv            :8;     /*<0-200> 精度0.01,0->-1.0,200->1.0*/
    uint64_t cct            :9;     /*<30-400>-><1500-20000>,精度50K*/
    uint64_t light_mode     :3;     /*<0-5>-><cct,hsi,gel,xy,source,rgb>*/
    uint64_t frq            :4;     /*<0-9>-><1-10>*/
    uint64_t spd            :4;     /*<0-9>-><1-10>*/
    uint64_t brightness     :10;    /*<0-1000>,精度0.1%*/  
    uint64_t state          :2;     /*0-停止，1-运行*/
    uint64_t effect_type    :8;     /*0-127*/
    #else
    #endif
}bt_pulsing_II_cct_arg_t;
typedef __packed struct{    
    #if (BYTE_ORDER == LITTLE_ENDIAN)
    uint64_t reserve        :8;    /*预留*/
    uint64_t cct            :9;     /*<30-400>-><1500-20000>,精度50K*/
    uint64_t sat            :7;     /*<0-100>*/
    uint64_t hue            :9;     /*<1-360>*/
    uint64_t light_mode     :3;     /*<0-5>-><cct,hsi,gel,xy,source,rgb>*/
    uint64_t frq            :4;     /*<0-9>-><1-10>*/
    uint64_t spd            :4;     /*<0-9>-><1-10>*/
    uint64_t brightness     :10;    /*<0-1000>,精度0.1%*/  
    uint64_t state          :2;     /*0-停止，1-运行*/
    uint64_t effect_type    :8;     /*0-127*/
    #else
    #endif
}bt_pulsing_II_hsi_arg_t;
typedef __packed struct{    
    #if (BYTE_ORDER == LITTLE_ENDIAN)
    uint64_t reserve        :9;     /*预留*/
    uint64_t color          :10;    /*色纸颜色<0-1023>*/
    uint64_t type           :4;     /*色纸类型<0-15>*/
    uint64_t origin         :1;     /*0-LEE,1-ROSCO*/
    uint64_t cct            :9;     /*<30-400>-><1500-20000>,精度50K*/
    uint64_t light_mode     :3;     /*<0-5>-><cct,hsi,gel,xy,source,rgb>*/
    uint64_t frq            :4;     /*<0-9>-><1-10>*/
    uint64_t spd            :4;     /*<0-9>-><1-10>*/
    uint64_t brightness     :10;    /*<0-1000>,精度0.1%*/  
    uint64_t state          :2;     /*0-停止，1-运行*/
    uint64_t effect_type    :8;     /*0-127*/
    #else
    #endif
}bt_pulsing_II_gel_arg_t;
typedef __packed struct{    
    #if (BYTE_ORDER == LITTLE_ENDIAN)
    uint64_t reserve        :5;     /*预留*/
    uint64_t y              :14;    /*<0-10000>,精度0.0001*/
    uint64_t x              :14;    /*<0-10000>,精度0.0001*/
    uint64_t light_mode     :3;     /*<0-5>-><cct,hsi,gel,xy,source,rgb>*/
    uint64_t frq            :4;     /*<0-9>-><1-10>*/
    uint64_t spd            :4;     /*<0-9>-><1-10>*/
    uint64_t brightness     :10;    /*<0-1000>,精度0.1%*/  
    uint64_t state          :2;     /*0-停止，1-运行*/
    uint64_t effect_type    :8;     /*0-127*/
    #else
    #endif
}bt_pulsing_II_xy_arg_t;
typedef __packed struct{    
    #if (BYTE_ORDER == LITTLE_ENDIAN)
    uint64_t reserve        :9;    /*预留*/
    uint64_t y              :8;     /*<0-100>,精度0.0001,调节范围 -0.005 - +0.005*/
    uint64_t x              :8;     /*<0-100>,精度0.0001,调节范围 -0.005 - +0.005*/
    uint64_t type           :8;     /*<0-256>，光源类型*/
    uint64_t light_mode     :3;     /*<0-5>-><cct,hsi,gel,xy,source,rgb>*/
    uint64_t frq            :4;     /*<0-9>-><1-10>*/
    uint64_t spd            :4;     /*<0-9>-><1-10>*/
    uint64_t brightness     :10;    /*<0-1000>,精度0.1%*/  
    uint64_t state          :2;     /*0-停止，1-运行*/
    uint64_t effect_type    :8;     /*0-127*/
    #else
    #endif
}bt_pulsing_II_source_arg_t;
typedef __packed struct{    
    #if (BYTE_ORDER == LITTLE_ENDIAN)
    uint64_t reserve        :3;     /*预留*/
    uint64_t b              :10;    /*<0-100>,精度0.0001,调节范围 -0.005 - +0.005*/
    uint64_t g              :10;    /*<0-100>,精度0.0001,调节范围 -0.005 - +0.005*/
    uint64_t r              :10;    /*<0-256>，光源类型*/
    uint64_t light_mode     :3;     /*<0-5>-><cct,hsi,gel,xy,source,rgb>*/
    uint64_t frq            :4;     /*<0-9>-><1-10>*/
    uint64_t spd            :4;     /*<0-9>-><1-10>*/
    uint64_t brightness     :10;    /*<0-1000>,精度0.1%*/  
    uint64_t state          :2;     /*0-停止，1-运行*/
    uint64_t effect_type    :8;     /*0-127*/
    #else
    #endif
}bt_pulsing_II_rgb_arg_t;

typedef __packed union{  
    bt_pulsing_II_cct_arg_t       cct;
    bt_pulsing_II_hsi_arg_t       hsi;
    bt_pulsing_II_gel_arg_t       gel;
    bt_pulsing_II_xy_arg_t        xy;
    bt_pulsing_II_source_arg_t    source;
    bt_pulsing_II_rgb_arg_t       rgb;
}bt_pulsing_II_arg_t;


/*0x08：welding_II*/
typedef __packed struct{    
    #if (BYTE_ORDER == LITTLE_ENDIAN)
    uint64_t reserve        :29;    /*预留*/
    uint64_t frq            :4;     /*<0-9>-><1-10>*/
    uint64_t min_brightness :10;    /*<0-750>,精度0.1%*/  
    uint64_t brightness     :10;    /*<0-1000>,精度0.1%*/  
    uint64_t package_type   :1;     /*0->base,1-pixel*/
    uint64_t state          :2;     /*0-停止，1-运行，3-continue*/
    uint64_t effect_type    :8;     /*0-127*/
    #else
    #endif
}bt_welding_II_type1_arg_t;

typedef __packed struct{    
    #if (BYTE_ORDER == LITTLE_ENDIAN)
    uint64_t reserve        :33;    /*预留*/
    uint64_t duv            :8;     /*<0-200> 精度0.01,0->-1.0,200->1.0*/
    uint64_t cct            :9;     /*<30-400>-><1500-20000>,精度50K*/
    uint64_t light_mode     :3;     /*<0-5>-><cct,hsi,gel,xy,source,rgb>*/ 
    uint64_t package_type   :1;     /*0->base,1-pixel*/
    uint64_t state          :2;     /*0-停止，1-运行，3-continue*/
    uint64_t effect_type    :8;     /*0-127*/
    #else
    #endif
}bt_welding_II_type2_cct_arg_t;
typedef __packed struct{    
    #if (BYTE_ORDER == LITTLE_ENDIAN)
    uint64_t reserve        :25;    /*预留*/
    uint64_t cct            :9;     /*<30-400>-><1500-20000>,精度50K*/
    uint64_t sat            :7;     /*<0-100>*/
    uint64_t hue            :9;     /*<1-360>*/
    uint64_t light_mode     :3;     /*<0-5>-><cct,hsi,gel,xy,source,rgb>*/
    uint64_t package_type   :1;     /*0->base,1-pixel*/
    uint64_t state          :2;     /*0-停止，1-运行，3-continue*/
    uint64_t effect_type    :8;     /*0-127*/
    #else
    #endif
}bt_welding_II_type2_hsi_arg_t;
typedef __packed struct{    
    #if (BYTE_ORDER == LITTLE_ENDIAN)
    uint64_t reserve        :26;     /*预留*/
    uint64_t color          :10;    /*色纸颜色<0-1023>*/
    uint64_t type           :4;     /*色纸类型<0-15>*/
    uint64_t origin         :1;     /*0-LEE,1-ROSCO*/
    uint64_t cct            :9;     /*<30-400>-><1500-20000>,精度50K*/
    uint64_t light_mode     :3;     /*<0-5>-><cct,hsi,gel,xy,source,rgb>*/
    uint64_t package_type   :1;     /*0->base,1-pixel*/
    uint64_t state          :2;     /*0-停止，1-运行，3-continue*/
    uint64_t effect_type    :8;     /*0-127*/
    #else
    #endif
}bt_welding_II_type2_gel_arg_t;
typedef __packed struct{    
    #if (BYTE_ORDER == LITTLE_ENDIAN)
    uint64_t reserve        :22;     /*预留*/
    uint64_t y              :14;    /*<0-10000>,精度0.0001*/
    uint64_t x              :14;    /*<0-10000>,精度0.0001*/
    uint64_t light_mode     :3;     /*<0-5>-><cct,hsi,gel,xy,source,rgb>*/
    uint64_t package_type   :1;     /*0->base,1-pixel*/
    uint64_t state          :2;     /*0-停止，1-运行，3-continue*/
    uint64_t effect_type    :8;     /*0-127*/
    #else
    #endif
}bt_welding_II_type2_xy_arg_t;
typedef __packed struct{    
    #if (BYTE_ORDER == LITTLE_ENDIAN)
    uint64_t reserve        :26;    /*预留*/
    uint64_t y              :8;     /*<0-100>,精度0.0001,调节范围 -0.005 - +0.005*/
    uint64_t x              :8;     /*<0-100>,精度0.0001,调节范围 -0.005 - +0.005*/
    uint64_t type           :8;     /*<0-256>，光源类型*/
    uint64_t light_mode     :3;     /*<0-5>-><cct,hsi,gel,xy,source,rgb>*/
    uint64_t package_type   :1;     /*0->base,1-pixel*/
    uint64_t state          :2;     /*0-停止，1-运行，3-continue*/
    uint64_t effect_type    :8;     /*0-127*/
    #else
    #endif
}bt_welding_II_type2_source_arg_t;
typedef __packed struct{    
    #if (BYTE_ORDER == LITTLE_ENDIAN)
    uint64_t reserve        :20;     /*预留*/
    uint64_t b              :10;    /*<0-100>,精度0.0001,调节范围 -0.005 - +0.005*/
    uint64_t g              :10;    /*<0-100>,精度0.0001,调节范围 -0.005 - +0.005*/
    uint64_t r              :10;    /*<0-256>，光源类型*/
    uint64_t light_mode     :3;     /*<0-5>-><cct,hsi,gel,xy,source,rgb>*/
    uint64_t package_type   :1;     /*0->base,1-pixel*/
    uint64_t state          :2;     /*0-停止，1-运行，3-continue*/
    uint64_t effect_type    :8;     /*0-127*/
    #else
    #endif
}bt_welding_II_type2_rgb_arg_t;

typedef __packed union{
    bt_welding_II_type2_cct_arg_t    cct;
    bt_welding_II_type2_hsi_arg_t    hsi;
    bt_welding_II_type2_gel_arg_t    gel;
    bt_welding_II_type2_xy_arg_t     xy;
    bt_welding_II_type2_source_arg_t source;
    bt_welding_II_type2_rgb_arg_t    rgb;
}bt_welding_II_type2_arg_t;

typedef __packed union{
    bt_welding_II_type1_arg_t    base;
    bt_welding_II_type2_arg_t    pixel;
}bt_welding_II_arg_t;

/*0x09：cop_car_II*/
typedef __packed struct{    
    #if (BYTE_ORDER == LITTLE_ENDIAN)
    uint64_t reserve        :37;    /*预留*/
    uint64_t color          :3;     /*<0-4>-><R,B,R+B,B+W,R+B+W>*/
    uint64_t frq            :4;     /*<0-9>-><1-10>*/
    uint64_t brightness     :10;    /*<0-1000>,精度0.1%*/ 
    uint64_t state          :2;     /*0-停止，1-运行*/
    uint64_t effect_type    :8;     /*0-127*/
    #else
    #endif
}bt_cop_car_II_arg_t;

/*0x0A：party_lights_II*/
typedef __packed struct{    
    #if (BYTE_ORDER == LITTLE_ENDIAN)
    uint64_t reserve        :28;    /*预留*/
    uint64_t spd            :6;     /*<1-60>单位秒*/
    uint64_t sat            :10;    /*<0-1000>精度0.1%*/
    uint64_t brightness     :10;    /*<0-1000>,精度0.1%*/ 
    uint64_t state          :2;     /*0-停止，1-运行*/
    uint64_t effect_type    :8;     /*0-127*/
    #else
    #endif
}bt_party_lights_II_arg_t;

/*0x0B：fireworks_II*/
typedef __packed struct{    
    #if (BYTE_ORDER == LITTLE_ENDIAN)
    uint64_t reserve        :24;    /*预留*/
    uint64_t mode           :2;     /*<0-2>-><cct,hue,cct+hue>*/
    uint64_t min_gap_time   :9;     /*<1-300>精度0.1秒*/
    uint64_t gap_time       :9;     /*<1-300>精度0.1秒*/
    uint64_t brightness     :10;    /*<0-1000>,精度0.1%*/ 
    uint64_t state          :2;     /*0-停止，1-运行*/
    uint64_t effect_type    :8;     /*0-127*/
    #else
    #endif
}bt_fireworks_II_arg_t;

/*0x0C：lightning_III*/
typedef __packed struct{    
    #if (BYTE_ORDER == LITTLE_ENDIAN)
    uint64_t reserve        :25;    /*预留*/
    uint64_t min_gap_time   :9;     /*最小间隔时间：<1-300> -> <0.1-30秒>，调节精度0.1秒*/
    uint64_t gap_time       :9;    /*最小间隔时间：<1-300> -> <0.1-30秒>，调节精度0.1秒*/ 
    uint64_t brightness     :10;    /*<0-1000>,精度0.1%*/  
    uint64_t package_type   :1;     /*0->base,1-pixel*/
    uint64_t state          :2;     /*0-停止，1-单次，2-循环，3-continue*/
    uint64_t effect_type    :8;     /*0-127*/
    #else
    #endif
}bt_lightning_III_type1_arg_t;

typedef __packed struct{    
    #if (BYTE_ORDER == LITTLE_ENDIAN)
    uint64_t reserve        :33;    /*预留*/
    uint64_t duv            :8;     /*<0-200> 精度0.01,0->-1.0,200->1.0*/
    uint64_t cct            :9;     /*<30-400>-><1500-20000>,精度50K*/
    uint64_t light_mode     :3;     /*<0-5>-><cct,hsi,gel,xy,source,rgb>*/
    uint64_t package_type   :1;     /*0->base,1-pixel*/
    uint64_t state          :2;     /*0-停止，1-单次，2-循环，3-continue*/
    uint64_t effect_type    :8;     /*0-127*/
    #else
    #endif
}bt_lightning_III_type2_cct_arg_t;
typedef __packed struct{    
    #if (BYTE_ORDER == LITTLE_ENDIAN)
    uint64_t reserve        :25;    /*预留*/
    uint64_t cct            :9;     /*<30-400>-><1500-20000>,精度50K*/
    uint64_t sat            :7;     /*<0-100>*/
    uint64_t hue            :9;     /*<1-360>*/
    uint64_t light_mode     :3;     /*<0-5>-><cct,hsi,gel,xy,source,rgb>*/
    uint64_t package_type   :1;     /*0->base,1-pixel*/
    uint64_t state          :2;     /*0-停止，1-单次，2-循环，3-continue*/
    uint64_t effect_type    :8;     /*0-127*/
    #else
    #endif
}bt_lightning_III_type2_hsi_arg_t;
typedef __packed struct{    
    #if (BYTE_ORDER == LITTLE_ENDIAN)
    uint64_t reserve        :26;     /*预留*/
    uint64_t color          :10;    /*色纸颜色<0-1023>*/
    uint64_t type           :4;     /*色纸类型<0-15>*/
    uint64_t origin         :1;     /*0-LEE,1-ROSCO*/
    uint64_t cct            :9;     /*<30-400>-><1500-20000>,精度50K*/
    uint64_t light_mode     :3;     /*<0-5>-><cct,hsi,gel,xy,source,rgb>*/
    uint64_t package_type   :1;     /*0->base,1-pixel*/
    uint64_t state          :2;     /*0-停止，1-单次，2-循环，3-continue*/
    uint64_t effect_type    :8;     /*0-127*/
    #else
    #endif
}bt_lightning_III_type2_gel_arg_t;
typedef __packed struct{    
    #if (BYTE_ORDER == LITTLE_ENDIAN)
    uint64_t reserve        :22;     /*预留*/
    uint64_t y              :14;    /*<0-10000>,精度0.0001*/
    uint64_t x              :14;    /*<0-10000>,精度0.0001*/
    uint64_t light_mode     :3;     /*<0-5>-><cct,hsi,gel,xy,source,rgb>*/
    uint64_t package_type   :1;     /*0->base,1-pixel*/  
    uint64_t state          :2;     /*0-停止，1-单次，2-循环，3-continue*/
    uint64_t effect_type    :8;     /*0-127*/
    #else
    #endif
}bt_lightning_III_type2_xy_arg_t;
typedef __packed struct{    
    #if (BYTE_ORDER == LITTLE_ENDIAN)
    uint64_t reserve        :26;     /*预留*/
    uint64_t y              :8;     /*<0-100>,精度0.0001,调节范围 -0.005 - +0.005*/
    uint64_t x              :8;     /*<0-100>,精度0.0001,调节范围 -0.005 - +0.005*/
    uint64_t type           :8;     /*<0-256>，光源类型*/
    uint64_t light_mode     :3;     /*<0-5>-><cct,hsi,gel,xy,source,rgb>*/
    uint64_t package_type   :1;     /*0->base,1-pixel*/
    uint64_t state          :2;     /*0-停止，1-单次，2-循环，3-continue*/
    uint64_t effect_type    :8;     /*0-127*/
    #else
    #endif
}bt_lightning_III_type2_source_arg_t;
typedef __packed struct{    
    #if (BYTE_ORDER == LITTLE_ENDIAN)
    uint64_t reserve        :20;     /*预留*/
    uint64_t b              :10;    /*<0-100>,精度0.0001,调节范围 -0.005 - +0.005*/
    uint64_t g              :10;    /*<0-100>,精度0.0001,调节范围 -0.005 - +0.005*/
    uint64_t r              :10;    /*<0-256>，光源类型*/
    uint64_t light_mode     :3;     /*<0-5>-><cct,hsi,gel,xy,source,rgb>*/
    uint64_t package_type   :1;     /*0->base,1-pixel*/
    uint64_t state          :2;     /*0-停止，1-单次，2-循环，3-continue*/
    uint64_t effect_type    :8;     /*0-127*/
    #else
    #endif
}bt_lightning_III_type2_rgb_arg_t;

typedef __packed union{  
    bt_lightning_III_type2_cct_arg_t       cct;
    bt_lightning_III_type2_hsi_arg_t       hsi;
    bt_lightning_III_type2_gel_arg_t       gel;
    bt_lightning_III_type2_xy_arg_t        xy;
    bt_lightning_III_type2_source_arg_t    source;
    bt_lightning_III_type2_rgb_arg_t       rgb;
}bt_lightning_III_type2_arg_t;

typedef __packed union{
    bt_lightning_III_type1_arg_t    base;
    bt_lightning_III_type2_arg_t    pixel;
}bt_lightning_III_arg_t;

/*0x0D：tv_III*/
typedef __packed struct{    
    #if (BYTE_ORDER == LITTLE_ENDIAN)
    uint64_t reserve        :25;    /*预留*/
    uint64_t min_gap_time   :9;     /*最小间隔时间：<1-300> -> <0.1-30秒>，调节精度0.1秒*/
    uint64_t gap_time       :9;    /*最小间隔时间：<1-300> -> <0.1-30秒>，调节精度0.1秒*/ 
    uint64_t brightness     :10;    /*<0-1000>,精度0.1%*/  
    uint64_t package_type   :1;     /*0->base,1-pixel*/
    uint64_t state          :2;     /*0-停止，1-运行，3-continue*/
    uint64_t effect_type    :8;     /*0-127*/
    #else
    #endif
}bt_tv_III_type1_arg_t;

typedef __packed struct{    
    #if (BYTE_ORDER == LITTLE_ENDIAN)
    uint64_t reserve        :24;    /*预留*/
    uint64_t duv            :8;     /*<0-200> 精度0.01,0->-1.0,200->1.0*/
    uint64_t min_cct        :9;     /*<30-400>-><1500-20000>,精度50K*/
    uint64_t max_cct        :9;     /*<30-400>-><1500-20000>,精度50K*/
    uint64_t light_mode     :3;     /*<0-5>-><cct,hsi,gel,xy,source,rgb>*/ 
    uint64_t package_type   :1;     /*0->base,1-pixel*/ 
    uint64_t state          :2;     /*0-停止，1-运行，3-continue*/
    uint64_t effect_type    :8;     /*0-127*/
    #else
    #endif
}bt_tv_III_type2_cct_arg_t;
typedef __packed struct{    
    #if (BYTE_ORDER == LITTLE_ENDIAN)
    uint64_t reserve        :16;    /*预留*/
    uint64_t cct            :9;     /*<30-400>-><1500-20000>,精度50K*/
    uint64_t sat            :7;     /*<0-100>*/
    uint64_t min_hue        :9;     /*<1-360>*/
    uint64_t max_hue        :9;     /*<1-360>*/
    uint64_t light_mode     :3;     /*<0-5>-><cct,hsi,gel,xy,source,rgb>*/
    uint64_t package_type   :1;     /*0->base,1-pixel*/ 
    uint64_t state          :2;     /*0-停止，1-运行，3-continue*/
    uint64_t effect_type    :8;     /*0-127*/
    #else
    #endif
}bt_tv_III_type2_hsi_arg_t;

typedef __packed union{
    bt_tv_III_type2_cct_arg_t    cct;
    bt_tv_III_type2_hsi_arg_t    hsi;
}bt_tv_III_type2_arg_t;

typedef __packed union{
    bt_tv_III_type1_arg_t    base;
    bt_tv_III_type2_arg_t    pixel;
}bt_tv_III_arg_t;

/*0x0E：fire_III*/
typedef __packed struct{    
    #if (BYTE_ORDER == LITTLE_ENDIAN)
    uint64_t reserve        :35;    /*预留*/
    uint64_t frq            :8;     /*频率：<20-200> -> <20-200次/分钟>*/
    uint64_t brightness     :10;    /*<0-1000>,精度0.1%*/  
    uint64_t package_type   :1;     /*0->base,1-pixel*/
    uint64_t state          :2;     /*0-停止，1-运行，3-continue*/
    uint64_t effect_type    :8;     /*0-127*/
    #else
    #endif
}bt_fire_III_type1_arg_t;

typedef __packed struct{    
    #if (BYTE_ORDER == LITTLE_ENDIAN)
    uint64_t reserve        :24;    /*预留*/
    uint64_t duv            :8;     /*<0-200> 精度0.01,0->-1.0,200->1.0*/
    uint64_t min_cct        :9;     /*<30-400>-><1500-20000>,精度50K*/
    uint64_t max_cct        :9;     /*<30-400>-><1500-20000>,精度50K*/
    uint64_t light_mode     :3;     /*<0-5>-><cct,hsi,gel,xy,source,rgb>*/ 
    uint64_t package_type   :1;     /*0->base,1-pixel*/ 
    uint64_t state          :2;     /*0-停止，1-运行，3-continue*/
    uint64_t effect_type    :8;     /*0-127*/
    #else
    #endif
}bt_fire_III_type2_cct_arg_t;
typedef __packed struct{    
    #if (BYTE_ORDER == LITTLE_ENDIAN)
    uint64_t reserve        :16;    /*预留*/
    uint64_t cct            :9;     /*<30-400>-><1500-20000>,精度50K*/
    uint64_t sat            :7;     /*<0-100>*/
    uint64_t min_hue        :9;     /*<1-360>*/
    uint64_t max_hue        :9;     /*<1-360>*/
    uint64_t light_mode     :3;     /*<0-5>-><cct,hsi,gel,xy,source,rgb>*/
    uint64_t package_type   :1;     /*0->base,1-pixel*/ 
    uint64_t state          :2;     /*0-停止，1-运行，3-continue*/
    uint64_t effect_type    :8;     /*0-127*/
    #else
    #endif
}bt_fire_III_type2_hsi_arg_t;

typedef __packed union{
    bt_fire_III_type2_cct_arg_t    cct;
    bt_fire_III_type2_hsi_arg_t    hsi;
}bt_fire_III_type2_arg_t;

typedef __packed union{
    bt_fire_III_type1_arg_t    base;
    bt_fire_III_type2_arg_t    pixel;
}bt_fire_III_arg_t;
/*0x0F：fault_bulb_III*/
typedef __packed struct{    
    #if (BYTE_ORDER == LITTLE_ENDIAN)
    uint64_t reserve        :25;    /*预留*/
    uint64_t min_gap_time   :9;     /*最小间隔时间：<1-300> -> <0.1-30秒>，调节精度0.1秒*/
    uint64_t gap_time       :9;    /*最小间隔时间：<1-300> -> <0.1-30秒>，调节精度0.1秒*/ 
    uint64_t brightness     :10;    /*<0-1000>,精度0.1%*/  
    uint64_t package_type   :1;     /*0->base,1-pixel*/
    uint64_t state          :2;     /*0-停止，1-运行，3-continue*/
    uint64_t effect_type    :8;     /*0-127*/
    #else
    #endif
}bt_fault_bulb_III_type1_arg_t;

typedef __packed struct{    
    #if (BYTE_ORDER == LITTLE_ENDIAN)
    uint64_t reserve        :33;    /*预留*/
    uint64_t duv            :8;     /*<0-200> 精度0.01,0->-1.0,200->1.0*/
    uint64_t cct            :9;     /*<30-400>-><1500-20000>,精度50K*/
    uint64_t light_mode     :3;     /*<0-5>-><cct,hsi,gel,xy,source,rgb>*/
    uint64_t package_type   :1;     /*0->base,1-pixel*/ 
    uint64_t state          :2;     /*0-停止，1-运行，3-continue*/
    uint64_t effect_type    :8;     /*0-127*/
    #else
    #endif
}bt_fault_bulb_III_type2_cct_arg_t;
typedef __packed struct{    
    #if (BYTE_ORDER == LITTLE_ENDIAN)
    uint64_t reserve        :25;    /*预留*/
    uint64_t cct            :9;     /*<30-400>-><1500-20000>,精度50K*/
    uint64_t sat            :7;     /*<0-100>*/
    uint64_t hue            :9;     /*<1-360>*/
    uint64_t light_mode     :3;     /*<0-5>-><cct,hsi,gel,xy,source,rgb>*/
    uint64_t package_type   :1;     /*0->base,1-pixel*/
    uint64_t state          :2;     /*0-停止，1-运行，3-continue*/
    uint64_t effect_type    :8;     /*0-127*/
    #else
    #endif
}bt_fault_bulb_III_type2_hsi_arg_t;
typedef __packed struct{    
    #if (BYTE_ORDER == LITTLE_ENDIAN)
    uint64_t reserve        :26;     /*预留*/
    uint64_t color          :10;    /*色纸颜色<0-1023>*/
    uint64_t type           :4;     /*色纸类型<0-15>*/
    uint64_t origin         :1;     /*0-LEE,1-ROSCO*/
    uint64_t cct            :9;     /*<30-400>-><1500-20000>,精度50K*/
    uint64_t light_mode     :3;     /*<0-5>-><cct,hsi,gel,xy,source,rgb>*/
    uint64_t package_type   :1;     /*0->base,1-pixel*/
    uint64_t state          :2;     /*0-停止，1-运行，3-continue*/
    uint64_t effect_type    :8;     /*0-127*/
    #else
    #endif
}bt_fault_bulb_III_type2_gel_arg_t;
typedef __packed struct{    
    #if (BYTE_ORDER == LITTLE_ENDIAN)
    uint64_t reserve        :22;     /*预留*/
    uint64_t y              :14;    /*<0-10000>,精度0.0001*/
    uint64_t x              :14;    /*<0-10000>,精度0.0001*/
    uint64_t light_mode     :3;     /*<0-5>-><cct,hsi,gel,xy,source,rgb>*/
    uint64_t package_type   :1;     /*0->base,1-pixel*/
    uint64_t state          :2;     /*0-停止，1-运行，3-continue*/
    uint64_t effect_type    :8;     /*0-127*/
    #else
    #endif
}bt_fault_bulb_III_type2_xy_arg_t;
typedef __packed struct{    
    #if (BYTE_ORDER == LITTLE_ENDIAN)
    uint64_t reserve        :26;    /*预留*/
    uint64_t y              :8;     /*<0-100>,精度0.0001,调节范围 -0.005 - +0.005*/
    uint64_t x              :8;     /*<0-100>,精度0.0001,调节范围 -0.005 - +0.005*/
    uint64_t type           :8;     /*<0-256>，光源类型*/
    uint64_t light_mode     :3;     /*<0-5>-><cct,hsi,gel,xy,source,rgb>*/
    uint64_t package_type   :1;     /*0->base,1-pixel*/ 
    uint64_t state          :2;     /*0-停止，1-运行，3-continue*/
    uint64_t effect_type    :8;     /*0-127*/
    #else
    #endif
}bt_fault_bulb_III_type2_source_arg_t;
typedef __packed struct{    
    #if (BYTE_ORDER == LITTLE_ENDIAN)
    uint64_t reserve        :20;     /*预留*/
    uint64_t b              :10;    /*<0-100>,精度0.0001,调节范围 -0.005 - +0.005*/
    uint64_t g              :10;    /*<0-100>,精度0.0001,调节范围 -0.005 - +0.005*/
    uint64_t r              :10;    /*<0-256>，光源类型*/
    uint64_t light_mode     :3;     /*<0-5>-><cct,hsi,gel,xy,source,rgb>*/
    uint64_t package_type   :1;     /*0->base,1-pixel*/
    uint64_t state          :2;     /*0-停止，1-运行，3-continue*/
    uint64_t effect_type    :8;     /*0-127*/
    #else
    #endif
}bt_fault_bulb_III_type2_rgb_arg_t;

typedef __packed union{  
    bt_fault_bulb_III_type2_cct_arg_t       cct;
    bt_fault_bulb_III_type2_hsi_arg_t       hsi;
    bt_fault_bulb_III_type2_gel_arg_t       gel;
    bt_fault_bulb_III_type2_xy_arg_t        xy;
    bt_fault_bulb_III_type2_source_arg_t    source;
    bt_fault_bulb_III_type2_rgb_arg_t       rgb;
}bt_fault_bulb_III_type2_arg_t;

typedef __packed union{
    bt_fault_bulb_III_type1_arg_t    base;
    bt_fault_bulb_III_type2_arg_t    pixel;
}bt_fault_bulb_III_arg_t;


/*0x10：pulsing_III*/
typedef __packed struct{    
    #if (BYTE_ORDER == LITTLE_ENDIAN)
    uint64_t reserve        :16;    /*预留*/
    uint64_t duv            :8;     /*<0-200> 精度0.01,0->-1.0,200->1.0*/
    uint64_t cct            :9;     /*<30-400>-><1500-20000>,精度50K*/
    uint64_t light_mode     :3;     /*<0-5>-><cct,hsi,gel,xy,source,rgb>*/ 
    uint64_t frq            :8;     /*<20-200>次/分钟*/ 
    uint64_t brightness     :10;    /*<0-1000>,精度0.1%*/ 
    uint64_t state          :2;     /*0-停止，1-运行*/
    uint64_t effect_type    :8;     /*0-127*/
    #else
    #endif
}bt_pulsing_III_cct_arg_t;
typedef __packed struct{    
    #if (BYTE_ORDER == LITTLE_ENDIAN)
    uint64_t reserve        :8;    /*预留*/
    uint64_t cct            :9;     /*<30-400>-><1500-20000>,精度50K*/
    uint64_t sat            :7;     /*<0-100>*/
    uint64_t hue            :9;     /*<1-360>*/
    uint64_t light_mode     :3;     /*<0-5>-><cct,hsi,gel,xy,source,rgb>*/
    uint64_t frq            :8;     /*<20-200>次/分钟*/ 
    uint64_t brightness     :10;        /*<0-1000>,精度0.1%*/
    uint64_t state          :2;     /*0-停止，1-运行*/
    uint64_t effect_type    :8;     /*0-127*/
    #else
    #endif
}bt_pulsing_III_hsi_arg_t;
typedef __packed struct{    
    #if (BYTE_ORDER == LITTLE_ENDIAN)
    uint64_t reserve        :9;     /*预留*/
    uint64_t color          :10;    /*色纸颜色<0-1023>*/
    uint64_t type           :4;     /*色纸类型<0-15>*/
    uint64_t origin         :1;     /*0-LEE,1-ROSCO*/
    uint64_t cct            :9;     /*<30-400>-><1500-20000>,精度50K*/
    uint64_t light_mode     :3;     /*<0-5>-><cct,hsi,gel,xy,source,rgb>*/
    uint64_t frq            :8;     /*<20-200>次/分钟*/ 
    uint64_t brightness     :10;        /*<0-1000>,精度0.1%*/
    uint64_t state          :2;     /*0-停止，1-运行*/
    uint64_t effect_type    :8;     /*0-127*/
    #else
    #endif
}bt_pulsing_III_gel_arg_t;
typedef __packed struct{    
    #if (BYTE_ORDER == LITTLE_ENDIAN)
    uint64_t reserve        :5;     /*预留*/
    uint64_t y              :14;    /*<0-10000>,精度0.0001*/
    uint64_t x              :14;    /*<0-10000>,精度0.0001*/
    uint64_t light_mode     :3;     /*<0-5>-><cct,hsi,gel,xy,source,rgb>*/
    uint64_t frq            :8;     /*<20-200>次/分钟*/ 
    uint64_t brightness     :10;        /*<0-1000>,精度0.1%*/
    uint64_t state          :2;     /*0-停止，1-运行*/
    uint64_t effect_type    :8;     /*0-127*/
    #else
    #endif
}bt_pulsing_III_xy_arg_t;
typedef __packed struct{    
    #if (BYTE_ORDER == LITTLE_ENDIAN)
    uint64_t reserve        :9;    /*预留*/
    uint64_t y              :8;     /*<0-100>,精度0.0001,调节范围 -0.005 - +0.005*/
    uint64_t x              :8;     /*<0-100>,精度0.0001,调节范围 -0.005 - +0.005*/
    uint64_t type           :8;     /*<0-256>，光源类型*/
    uint64_t light_mode     :3;     /*<0-5>-><cct,hsi,gel,xy,source,rgb>*/
    uint64_t frq            :8;     /*<20-200>次/分钟*/ 
    uint64_t brightness     :10;        /*<0-1000>,精度0.1%*/
    uint64_t state          :2;     /*0-停止，1-运行*/
    uint64_t effect_type    :8;     /*0-127*/
    #else
    #endif
}bt_pulsing_III_source_arg_t;
typedef __packed struct{    
    #if (BYTE_ORDER == LITTLE_ENDIAN)
    uint64_t reserve        :3;     /*预留*/
    uint64_t b              :10;    /*<0-100>,精度0.0001,调节范围 -0.005 - +0.005*/
    uint64_t g              :10;    /*<0-100>,精度0.0001,调节范围 -0.005 - +0.005*/
    uint64_t r              :10;    /*<0-256>，光源类型*/
    uint64_t light_mode     :3;     /*<0-5>-><cct,hsi,gel,xy,source,rgb>*/
    uint64_t frq            :8;     /*<20-200>次/分钟*/ 
    uint64_t brightness     :10;        /*<0-1000>,精度0.1%*/
    uint64_t state          :2;     /*0-停止，1-运行*/
    uint64_t effect_type    :8;     /*0-127*/
    #else
    #endif
}bt_pulsing_III_rgb_arg_t;

typedef __packed union{
    bt_pulsing_III_cct_arg_t    cct;
    bt_pulsing_III_hsi_arg_t    hsi;
    bt_pulsing_III_gel_arg_t    gel;
    bt_pulsing_III_xy_arg_t     xy;
    bt_pulsing_III_source_arg_t source;
    bt_pulsing_III_rgb_arg_t    rgb;
}bt_pulsing_III_arg_t;

/*0x11：cop_car_III*/
typedef __packed struct{    
    #if (BYTE_ORDER == LITTLE_ENDIAN)
    uint64_t reserve        :37;    /*预留*/
    uint64_t color          :3;     /*<0-4>-><R,B,R+B,B+W,R+B+W>*/
    uint64_t frq            :4;     /*<1-6>-><闪1次...闪5次，循环闪>*/
    uint64_t brightness     :10;    /*<0-1000>,精度0.1%*/ 
    uint64_t state          :2;     /*0-停止，1-运行*/
    uint64_t effect_type    :8;     /*0-127*/
    #else
    #endif
}bt_cop_car_III_arg_t;

/*--------EFFECT_CMD：0x22-------*/
typedef __packed union{  
    bt_paparazzi_II_arg_t   paparazzi_II;
    bt_lightning_II_arg_t   lightning_II;
    bt_tv_II_arg_t          tv_II;
    bt_fire_II_arg_t        fire_II;
    bt_strobe_II_arg_t      strobe_II;
    bt_explosion_II_arg_t   explosion_II;
    bt_fault_bulb_II_arg_t  fault_bulb_II;
    bt_pulsing_II_arg_t     pulsing_II;
    bt_welding_II_arg_t     welding_II;
    bt_cop_car_II_arg_t     cop_car_II;
    bt_party_lights_II_arg_t party_lights_II;
    bt_fireworks_II_arg_t   fireworks_II;
    bt_lightning_III_arg_t  lightning_III;
    bt_tv_III_arg_t         tv_III;
    bt_fire_III_arg_t       fire_III;
    bt_fault_bulb_III_arg_t fault_bulb_III;
    bt_pulsing_III_arg_t    pulsing_III;
    bt_cop_car_III_arg_t    cop_car_III;
}bt_light_effect_II_arg_t;
/**************************************************************************************/
/******************************分区光效************************************************/
/**************************************************************************************/

/************************************光效的类型***************************************/
/*分区光效*/
/*---Partition_Color子类型---*/
typedef enum{
    BT_Partition_Mode_CCT = 0,
    BT_Partition_Mode_HSI,
    BT_Partition_Mode_Null,
}bt_partition_color_mode_enum;
/*write*/
typedef __packed struct{
	uint64_t duv    			:8;     /*<0-200> 精度0.01,0->-1.0,200->1.0 255 -> 不改变数据*/
	uint64_t cct    			:8;     /*<15-200>-><1500-20000>,精度100K 255 -> 不改变数据*/
	uint64_t brightness    	    :10;    /*<0-1000>,精度0.1% 1023 -> 不改变数据*/ 
    uint64_t light_mode    		:1;     /*0cct-1hsi*/
	uint64_t state    		    :1;     /*叠加光效状态,0-开启，1-关闭*/
	uint64_t target   	        :36;    /*设置目标像素，用位域表示，共36位，1值生效；*/
}bt_partition_para_write_cct_arg_t;
typedef __packed struct{
    uint64_t sat    			:7;     /*<0-100> 127 -> 不改变数据*/
	uint64_t hue    			:9;     /*<1-360> 511 -> 不改变数据*/
	uint64_t brightness    	    :10;    /*<0-1000>,精度0.1% 1023 -> 不改变数据*/ 
    uint64_t light_mode    		:1;     /*0cct-1hsi*/
	uint64_t state    		    :1;     /*叠加光效状态,0-开启，1-关闭*/
	uint64_t target   	        :36;    /*设置目标像素，用位域表示，共36位，1值生效；*/
}bt_partition_para_write_hsi_arg_t;

typedef __packed union{
	bt_partition_para_write_cct_arg_t cct;
	bt_partition_para_write_hsi_arg_t hsi;
}bt_partition_para_write_arg_t;
/*read*/
typedef __packed struct{
    uint64_t reserve    		:56;    /*预留*/
	uint64_t target		        :8;     /*<0-35>:读取像素信息，APP只发偶数(0,2,4..)，灯体一次回两个像素参数；255:读取分区配置*/
}bt_partition_para_read_arg_t;
/*ack para*/
typedef __packed struct{
    uint64_t reserve    			:2;    /*预留*/
	uint64_t duv2    		        :8;    /*<0-200> 精度0.01,0->-1.0,200->1.0*/
	uint64_t cct2    		        :8;    /*<15-200>-><1500-20000>,精度100K*/
	uint64_t brightness2    	    :10;   /*<0-1000>,精度0.1%*/ 
	uint64_t light_mode2			:1;	   /*0:CCT 1:HSI */
	uint64_t duv1    		        :8;    /*<0-200> 精度0.01,0->-1.0,200->1.0*/
	uint64_t cct1    		        :8;    /*<15-200>-><1500-20000>,精度100K*/
	uint64_t brightness1    	    :10;   /*<0-1000>,精度0.1%*/ 
	uint64_t light_mode1			:1;	   /*0:CCT 1:HSI */
	uint64_t target			        :8;    /*<0-35>：这个区间的偶数，设备回复APP以这个偶数为起始点的两个分区的参数；(与APP下发值保持一致)*/
}bt_partition_para_ack_gp_0_arg_t;
typedef __packed struct{
    uint64_t reserve    			:2;    /*预留*/
	uint64_t sat2    			    :7;    /*<0-100>*/
	uint64_t hue2    			    :9;    /*<1-360>*/
	uint64_t brightness2    	    :10;   /*<0-1000>,精度0.1%*/ 
	uint64_t light_mode2			:1;	   /*0:CCT 1:HSI */
	uint64_t duv1    		        :8;    /*<0-200> 精度0.01,0->-1.0,200->1.0*/
	uint64_t cct1    		        :8;    /*<15-200>-><1500-20000>,精度100K*/
	uint64_t brightness1    	    :10;   /*<0-1000>,精度0.1%*/ 
	uint64_t light_mode1			:1;	   /*0:CCT 1:HSI */
	uint64_t target			        :8;    /*<0-35>：这个区间的偶数，设备回复APP以这个偶数为起始点的两个分区的参数；(与APP下发值保持一致)*/
}bt_partition_para_ack_gp_1_arg_t;
typedef __packed struct{
    uint64_t reserve    			:2;    /*预留*/
	uint64_t duv2    		        :8;    /*<0-200> 精度0.01,0->-1.0,200->1.0*/
	uint64_t cct2    		        :8;    /*<15-200>-><1500-20000>,精度100K*/
	uint64_t brightness2    	    :10;   /*<0-1000>,精度0.1%*/ 
	uint64_t light_mode2			:1;	   /*0:CCT 1:HSI */
	uint64_t sat1    			    :7;    /*<0-100>*/
	uint64_t hue1    			    :9;    /*<1-360>*/
	uint64_t brightness1    	    :10;   /*<0-1000>,精度0.1%*/ 
	uint64_t light_mode1			:1;	   /*0:CCT 1:HSI */
	uint64_t target			        :8;    /*<0-35>：这个区间的偶数，设备回复APP以这个偶数为起始点的两个分区的参数；(与APP下发值保持一致)*/
}bt_partition_para_ack_gp_2_arg_t;
typedef __packed struct{
    uint64_t reserve    			:2;    /*预留*/
	uint64_t sat2    			    :7;    /*<0-100>*/
	uint64_t hue2   			    :9;    /*<1-360>*/
	uint64_t brightness2    	    :10;   /*<0-1000>,精度0.1%*/ 
	uint64_t light_mode2			:1;	   /*0:CCT 1:HSI */
	uint64_t sat1    			    :7;    /*<0-100>*/
	uint64_t hue1    			    :9;    /*<1-360>*/
	uint64_t brightness1    	    :10;   /*<0-1000>,精度0.1%*/ 
	uint64_t light_mode1			:1;	   /*0:CCT 1:HSI */
	uint64_t target			        :8;    /*<0-35>：这个区间的偶数，设备回复APP以这个偶数为起始点的两个分区的参数；(与APP下发值保持一致)*/
}bt_partition_para_ack_gp_3_arg_t;

typedef __packed union{
    bt_partition_para_ack_gp_0_arg_t    gp0;    /*CCT+CCT*/
    bt_partition_para_ack_gp_1_arg_t    gp1;    /*CCT+HSI*/
    bt_partition_para_ack_gp_2_arg_t    gp2;    /*HSI+CCT*/
    bt_partition_para_ack_gp_3_arg_t    gp3;    /*HSI+HSI*/
}bt_partition_para_ack_para_arg_t;
/*ack config*/
typedef __packed struct{
    uint64_t reserve    			:42;    /*预留*/
    uint64_t sleep                  :1;     /*0->关机,1->开机*/
    uint64_t state                  :1;     /*叠加光效状态,0x00->开启,0x01->不开启*/
    uint64_t y                      :6;     /*<1-36>,Y方向像素个数*/
    uint64_t x                      :6;     /*<1-36>,X方向像素个数*/
    uint64_t target                 :8;     /*255:读取分区配置(与APP下发值保持一致)*/
}bt_partition_para_ack_cfg_arg_t;

typedef __packed union{
    bt_partition_para_ack_para_arg_t    para;
    bt_partition_para_ack_cfg_arg_t     cfg;
}bt_partition_para_ack_arg_t;

typedef __packed union{
    bt_partition_para_write_arg_t   write;
    bt_partition_para_read_arg_t    read;
    bt_partition_para_ack_arg_t     ack;
}bt_partition_para_arg_t;

/*分区光效*/
typedef __packed struct{
	uint64_t Reserve    		:23;   /*预留*/  
    uint64_t trigger    		:1;    /*0->像素统一,1->像素独立*/
	uint64_t frq_max    		:5;    /*0 ->像素统一 1->像素独立*/
	uint64_t frq_min    		:5;    /*0～29，0.1-1Hz(0.1步进)1-20Hz(1步进)*/
	uint64_t interval_max   	:7;    /*0～29，0.1-1Hz(0.1步进)1-20Hz(1步进)*/
	uint64_t interval_min   	:7;    /*0～69，0-1s(0.1步进)，1-60s(1步进)*/
	uint64_t lasting_max    	:7;    /*0～69，0-1s(0.1步进)，1-60s(1步进)*/
	uint64_t lasting_min    	:7;    /*0～69，0-1s(0.1步进)，1-60s(1步进)*/
	uint64_t mode			    :2;	   /*0:Strobe 1:Flicker 2:Pulsing */
}bt_partition_fx_strobe_arg_t;
typedef __packed struct{
	uint64_t Reserve    		:16;    /*预留*/
	uint64_t int_min    		:7;    /*<0-100>,精度1%*/
    uint64_t trigger    		:1;    /*0->像素统一,1->像素独立*/
	uint64_t frq_max    		:5;    /*0 ->像素统一 1->像素独立*/
	uint64_t frq_min    		:5;    /*0～29，0.1-1Hz(0.1步进)1-20Hz(1步进)*/
	uint64_t interval_max   	:7;    /*0～29，0.1-1Hz(0.1步进)1-20Hz(1步进)*/
	uint64_t interval_min   	:7;    /*0～69，0-1s(0.1步进)，1-60s(1步进)*/
	uint64_t lasting_max    	:7;    /*0～69，0-1s(0.1步进)，1-60s(1步进)*/
	uint64_t lasting_min    	:7;    /*0～69，0-1s(0.1步进)，1-60s(1步进)*/
	uint64_t mode			    :2;	   /*0:Strobe 1:Flicker 2:Pulsing */
}bt_partition_fx_flicker_arg_t;
typedef __packed struct{
	uint64_t Reserve    		:16;    /*预留*/
	uint64_t int_min    		:7;    /*<0-100>,精度1%*/
    uint64_t trigger    		:1;    /*0->像素统一,1->像素独立*/
	uint64_t frq_max    		:5;    /*0 ->像素统一 1->像素独立*/
	uint64_t frq_min    		:5;    /*0～29，0.1-1Hz(0.1步进)1-20Hz(1步进)*/
	uint64_t interval_max   	:7;    /*0～29，0.1-1Hz(0.1步进)1-20Hz(1步进)*/
	uint64_t interval_min   	:7;    /*0～69，0-1s(0.1步进)，1-60s(1步进)*/
	uint64_t lasting_max    	:7;    /*0～69，0-1s(0.1步进)，1-60s(1步进)*/
	uint64_t lasting_min    	:7;    /*0～69，0-1s(0.1步进)，1-60s(1步进)*/
	uint64_t mode			    :2;	   /*0:Strobe 1:Flicker 2:Pulsing */
}bt_partition_fx_pulsing_arg_t;

typedef __packed union{
	bt_partition_fx_strobe_arg_t    strobe;
    bt_partition_fx_flicker_arg_t   flicker;
    bt_partition_fx_pulsing_arg_t   pulsing;
}bt_partition_fx_arg_t;

/*版本信息2*/
typedef __packed struct{
    uint64_t reserve                :20;/*预留*/
    uint64_t emotion_support        :1; /*用途：是否支持电动调节*/
    uint64_t pixel_num              :4; /*用途：针对一部分点控光效在不同产品上的像素数量不同的情况。同一产品这类光效的像素数量一样。*/
    uint64_t sleep                  :1; /*设备状态0-关机，1-开机*/
    uint64_t add_state              :1; /*分区光效叠加状态0->开启 1->不开启*/
    uint64_t pixel_y2               :6; /*X方向像素数量*/
    uint64_t pixel_x2               :6; /*Y方向像素数量*/
    uint64_t pixel_y1               :4; /*X方向像素数量*/
    uint64_t pixel_x1               :4; /*Y方向像素数量*/
    uint64_t pixel_fx_gpG_support   :1; /*点控光效G组支持状态0-不支持，1-支持<rainbow>*/
    uint64_t pixel_fx_gpF_support   :1; /*点控光效F组支持状态0-不支持，1-支持<music>*/
    uint64_t pixel_fx_gpE_support   :1; /*点控光效E组支持状态0-不支持，1-支持<belt_fx,灯带光效19种>*/
    uint64_t pixel_fx_gpD_support   :1; /*点控光效D组支持状态0-不支持，1-支持<on_off>*/
    uint64_t pixel_fx_gpC_support   :1; /*点控光效C组支持状态0-不支持，1-支持<color_move_m>*/
    uint64_t pixel_fx_gpB_support   :1; /*点控光效B组支持状态0-不支持，1-支持<color_move_I/color_move_II/color_move_III>*/
    uint64_t pixel_fx_gpA_support   :1; /*点控光效A组支持状态0-不支持，1-支持<color_cut/color_replace/pixel_fire/color_fade>*/
    uint64_t pixel_fx_support       :1; /*点控光效支持状态0-不支持，1-支持*/
    uint64_t sys_fx2_gpH_support    :1; /*系统光效2代H组支持状态0-不支持，1-支持(只支持点控产品)；<cop_car_III/party_lights_II>*/
    uint64_t sys_fx2_gpG_support    :1; /*系统光效2代G组支持状态0-不支持，1-支持(只支持点控产品)；<fire_III>*/
    uint64_t sys_fx2_gpF_support    :1; /*系统光效2代F组支持状态0-不支持，1-支持(只支持点控产品)；<paparazzi_II/lightning_III/tv_III/faulty_bulb_III/pulsing_III/fireworks_II>*/
    uint64_t sys_fx2_gpE_support    :1; /*系统光效2代E组支持状态0-不支持，1-支持(只支持非点控产品)；<welding_II>*/
    uint64_t sys_fx2_gpD_support    :1; /*系统光效2代D组支持状态0-不支持，1-支持(只支持非点控产品)；<cop_car_II>*/
    uint64_t sys_fx2_gpC_support    :1; /*系统光效2代C组支持状态0-不支持，1-支持(只支持非点控产品)；<fire_II>*/
    uint64_t sys_fx2_gpB_support    :1; /*系统光效2代B组支持状态0-不支持，1-支持(只支持非点控产品)；<strobe_II/explosion_II>*/
    uint64_t sys_fx2_gpA_support    :1; /*系统光效2代A组支持状态0-不支持，1-支持(只支持非点控产品)；<lightning_II/tv_II/fault_bulb_II/pulsing_II>*/
    uint64_t sys_fx2_support        :1; /*系统光效2代支持状态0-不支持，1-支持*/
}bt_get_ver2_arg_t;
/*分区配置*/
typedef __packed struct{
    uint64_t reserve                :60;/*预留*/
    uint64_t xy_mode                :4; /*0 - 默认分区数模式，1 - 最大分区数模式*/
}bt_partition_cfg_write_art_t;
typedef __packed struct{
    uint64_t reserve                :40;/*预留*/
    uint64_t pixel_y2               :6; /*Y方向最大分区数,<1-36> Y方向像素个数*/
    uint64_t pixel_x2               :6; /*X方向最大分区数,<1-36> X方向像素个数*/
    uint64_t pixel_y1               :4; /*默认情况下Y方向分区数,<1-10> Y方向像素个数*/
    uint64_t pixel_x1               :4; /*默认情况下X方向分区数,<1-10> X方向像素个数*/
    uint64_t xy_mode                :4; /*0 - 默认分区数模式，1 - 最大分区数模式*/
}bt_partition_cfg_read_art_t;
typedef __packed union{
    bt_partition_cfg_write_art_t write;
    bt_partition_cfg_read_art_t  read;
}bt_partition_cfg_art_t;

/*--------EMOTION_CFG_CMD：0x2B-------*/
typedef __packed struct{
    uint64_t zoom_min               :8; /*变焦最小角度*/
    uint64_t cut_angle              :1; /*角度支持*/
    uint64_t zoom_blur              :1; /*模糊支持*/
    uint64_t zoom                   :1; /*变焦支持*/
    uint64_t rotation_pitch         :1; /*俯仰旋转支持*/
    uint64_t rotation_lateral       :1; /*水平旋转支持*/
    uint64_t extend2_uuid           :12; /*扩展2UUID*/
    uint64_t extend1_uuid           :12; /*扩展1UUID*/
    uint64_t bayonet_uuid           :12; /*卡扣UUID*/
    uint64_t built_uuid             :12; /*内置UUID*/
    uint64_t packet_index           :3;  /*包序*/
}bt_emotion_cfg_packet1_art_t;

typedef __packed struct{
    uint64_t reserve              :44;/*预留*/
    uint64_t lock_pitch           :1; /*俯仰锁定*/
    uint64_t lock_lateral         :1; /*水平锁定*/
    uint64_t smoke                :1; /*烟雾支持*/
    uint64_t gobo_speed           :1; /*速度支持*/
    uint64_t gobo_figure          :1; /*图案支持*/
    uint64_t barndoor             :2; /*挡板相位数量支持*/
    uint64_t cut                  :2; /*切割相位数量支持*/
    uint64_t zoom_max             :8; /*变焦最大角度*/
    uint64_t packet_index         :3;  /*包序*/
}bt_emotion_cfg_packet2_art_t;

typedef __packed struct{
    uint64_t reserve              :61; /*预留*/
    uint64_t packet_index         :3;  /*包序*/
}bt_emotion_cfg_packet3_art_t;

typedef __packed struct{
    uint64_t reserve              :61; /*预留*/
    uint64_t packet_index         :3;  /*包序*/
}bt_emotion_cfg_packet4_art_t;

typedef __packed struct{
    uint64_t reserve              :61; /*预留*/
    uint64_t packet_index         :3;  /*包序*/
}bt_emotion_cfg_packet5_art_t;

typedef __packed struct{
    uint64_t reserve              :61; /*预留*/
    uint64_t packet_index         :3;  /*包序*/
}bt_emotion_cfg_packet6_art_t;

typedef __packed struct{
    uint64_t reserve              :61; /*预留*/
    uint64_t packet_index         :3;  /*包序*/
}bt_emotion_cfg_packet7_art_t;

typedef __packed struct{
    uint64_t reserve              :61; /*预留*/
    uint64_t packet_index         :3;  /*包序*/
}bt_emotion_cfg_packet8_art_t;

typedef __packed union{
    bt_emotion_cfg_packet8_art_t emotion_cfg_packet8; 
    bt_emotion_cfg_packet7_art_t emotion_cfg_packet7;  
    bt_emotion_cfg_packet6_art_t emotion_cfg_packet6;  
    bt_emotion_cfg_packet5_art_t emotion_cfg_packet5;  
    bt_emotion_cfg_packet4_art_t emotion_cfg_packet4;  
    bt_emotion_cfg_packet3_art_t emotion_cfg_packet3;  
    bt_emotion_cfg_packet2_art_t emotion_cfg_packet2;  
    bt_emotion_cfg_packet1_art_t emotion_cfg_packet1;  
}bt_emotion_cfg_arg_t;
/*--------EMOTION_LIVE_CMD：0x2C-------*/
typedef __packed struct{
//    uint64_t reserve                  :3; /*预留*/
    uint64_t rotation_pitch_max       :9;  /*俯仰最大角度*/
    uint64_t rotation_pitch_min       :9;  /*俯仰最小角度*/
    uint64_t rotation_pitch           :9;  /*俯仰角度*/
    uint64_t rotation_laterial_max    :10; /*水平最打角度*/
    uint64_t rotation_laterial_min    :10; /*水平最小角度*/
    uint64_t rotation_laterial        :10; /*水平角度*/
    uint64_t packet_index             :3;     /*包序*/
    uint64_t target_field             :4;     /*包段*/
}bt_emotion_live_packet1_art_t;       /*size:64*/

typedef __packed struct{
    uint64_t smoke_switch             :1; /*开关*/
    uint64_t smoke_time               :6; /*间隔等级*/
    uint64_t smoke_fog                :6; /*雾量等级*/
    uint64_t smoke_wind               :6; /*风速等级*/
    uint64_t cut_right                :6; /*右切割*/
    uint64_t cut_left                 :6; /*左切割*/
    uint64_t cut_down                 :6; /*下切割等级*/
    uint64_t cut_up                   :6; /*上切割等级*/
    uint64_t zoom_blur                :6; /*模糊等级*/
    uint64_t zoom_angle               :8; /*变焦最大角度*/
    uint64_t packet_index             :3;     /*包序*/
    uint64_t target_field             :4;     /*包段*/
}bt_emotion_live_packet2_art_t;      /*size:64*/

typedef __packed struct{
    uint64_t reserve            :1;       /*变焦最大角度*/
    uint64_t brandoor_down      :6;       /*下挡板等级*/
    uint64_t brandoor_up        :6;       /*上挡板等级*/
    uint64_t cut_right_angle    :7;       /*切割右角度*/
    uint64_t cut_left_angle     :7;       /*切割左角度*/
    uint64_t cut_down_angle     :7;       /*切割下角度*/
    uint64_t cut_up_angle       :7;       /*切割上角度*/
    uint64_t gobo_figure        :8;       /*图案*/
    uint64_t gobo_speed         :7;       /*图案速度*/
    uint64_t gobo_switch        :1;       /*图案开关*/
    uint64_t packet_index       :3;       /*包序*/
    uint64_t target_field       :4;       /*包段*/
}bt_emotion_live_packet3_art_t;      /*size:64*/

typedef __packed struct{
    uint64_t reserve           :13;    /*预留*/
	uint64_t lock_pitch        :1;     /*垂直锁定*/
	uint64_t lock_lateral      :1;     /*水平锁定*/
    uint64_t location_z        :10;    /*z范围*/
    uint64_t location_y        :10;    /*y范围*/
    uint64_t location_x        :10;    /*x范围*/
    uint64_t brandoor_right    :6;     /*右挡板等级*/
    uint64_t brandoor_left     :6;     /*左挡板等级*/
    uint64_t packet_index      :3;     /*包序*/
    uint64_t target_field      :4;     /*包段*/
}bt_emotion_live_packet4_art_t;   /*size:64*/


typedef __packed struct{
    uint64_t reserve        :7;    /*预留*/
    uint64_t Character_6    :8;     /*字符6     ASCII 编码*/
    uint64_t Character_5    :8;     /*字符5     ASCII 编码*/
    uint64_t Character_4    :8;     /*字符4     ASCII 编码*/
    uint64_t Character_3    :8;     /*字符3     ASCII 编码*/
    uint64_t Character_2    :8;     /*字符2     ASCII 编码*/
    uint64_t Character_1    :8;     /*字符1     ASCII 编码*/
    uint64_t Character_0    :8;     /*字符0     ASCII 编码*/ 
	uint64_t package_type   :1;     
}bt_get_sn_packet_t;   /*size:64*/


typedef __packed union{
    bt_emotion_live_packet4_art_t emotion_live_packet4;
    bt_emotion_live_packet3_art_t emotion_live_packet3;
    bt_emotion_live_packet2_art_t emotion_live_packet2;
    bt_emotion_live_packet1_art_t emotion_live_packet1; 
}bt_emotion_live_arg_t;   /*size:64*/

typedef __packed struct{
    uint64_t reserve            :63;       /*变焦最大角度*/
    uint64_t state      :1;       /*下挡板等级*/
}bt_high_speed_arg_t;      /*size:64*/

/*协议体*/
typedef __packed union{    
    BT_Version_Body_TypeDef         Version_Body;
    BT_HSI_Body_TypeDef             HSI_Body;
    BT_CCT_Body_TypeDef             CCT_Body;
    BT_Gel_Body_TypeDef             Gel_Body;
    BT_RGBWW_Body_TypeDef           RGBWW_Body;
    BT_XY_Coordinate_Body_TypeDef   XY_Coordinate_Body;
    BT_LED_Frequency_Body_TypeDef   LED_Frequency_Body;
    BT_Effect_Mode_Body_TypeDef     Light_Effect_Body;
    BT_Dimming_Curve_Body_TypeDef   Dimming_Curve_Body;
    BT_Fan_Speed_Body_TypeDef       Fan_Speed_Body;
    BT_Get_Power_Body_TypeDef       Get_Power_Body;
    BT_Board_State_Body_TypeDef     Board_State_Body;
    BT_Sleep_Mode_Body_TypeDef      Sleep_Mode_Body;
    BT_Main_Interface_Body_TypeDef  Main_Interface_Body;
    BT_Current_Menu_Body_TypeDef    Current_Menu_Body;
    BT_Light_Bright_Body_TypeDef    Light_Bright_Body;
    BT_PFX_Flash_Body_TypeDef       PFX_Flash_Body;
    BT_PFX_Chase_Body_TypeDef       PFX_Chase_Body;
    BT_PFX_Continue_Body_TypeDef    PFX_Continue_Body;
    BT_CFX_Bank_RW_Body_TypeDef     CFX_Bank_RW_Body;
    BT_CFX_Ctrl_Body_TypeDef        CFX_Ctrl_Body;
    BT_APP_Ctrl_Body_TypeDef        APP_Ctrl_Body;
    BT_CFX_Preview_Body_TypeDef     CFX_Preview_Body;
    BT_MFX_Body_TypeDef             MFX_Body;
    BT_CFX_Name_Body_TypeDef        CFX_Name_Body;
	bt_pixel_fx_arg_t				pixel_fx;
    bt_light_effect_II_arg_t        light_effect_II;
    bt_partition_para_arg_t         partition_para;
    bt_partition_fx_arg_t           partition_fx;
    bt_get_ver2_arg_t               version2;
    bt_partition_cfg_art_t          partition_cfg;
    bt_emotion_cfg_arg_t            emotion_cfg_body;  
    bt_emotion_live_arg_t           emotion_live_body;    
	bt_high_speed_arg_t       		high_speed_body;
	bt_get_sn_packet_t              sn_get_body;
}BT_Body_TypeDef;

/*--------通信包数据结构------*/
typedef __packed struct{
    uint8_t             CheckSum;   /*校验和*/
    BT_Body_TypeDef     Body;       /*数据内容*/
    BT_Header_TypeDef   Header;     /*数据头*/
}BT_Packet_TypeDef; /*size: 10*/

#endif /*__BT_PROTO_H*/

