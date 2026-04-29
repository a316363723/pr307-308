#ifndef SIDUSPROFX_H
#define SIDUSPROFX_H
#include <stdint.h>
#include <stdbool.h>

typedef enum 
{
    MFX_MODE_FLASH,
    MFX_MODE_CONTINUE,
    MFX_MODE_PARAGRAPH,
    MFX_MODE_NULL,
}MFX_Mode_Type;

typedef enum 
{
    MFX_CTRL_STOP,
    MFX_CTRL_RESTART,
    MFX_CTRL_PAUSE,
    MFX_CTRL_CONTINUE,
}MFX_Ctrl_Type;

typedef __packed struct
{
    uint8_t Base;           //0:Base CCT    1:Base HSI
    uint16_t Int_Mini;      /*0-1000*/ 
    uint16_t Int_Max;       /*0-1000*/
    uint8_t Int_Seq;        /*0:正向    1:反向    2:往复    3:随机*/  
    uint16_t CCT_Mini; 
    uint16_t CCT_Max;       
    uint8_t CCT_Seq;        /*0:正向    1:反向    2:往复    3:随机*/
    uint8_t GM_Mini;        /*<0-20>: -1.0->0,-0.9->1,-0.8->2,   +1.0->20每隔0.1递增1，以此类推*/
    uint8_t GM_Max;         /*<0-20>: -1.0->0,-0.9->1,-0.8->2,   +1.0->20每隔0.1递增1，以此类推*/
    uint8_t GM_Seq;         /*0:正向    1:反向    2:往复    3:随机*/    
}MFX_Base_CCT;

typedef __packed struct
{
    uint8_t Base;           //0:Base CCT    1:Base HSI  
    uint16_t Int_Mini;      /*0-1000*/ 
    uint16_t Int_Max;       /*0-1000*/
    uint8_t Int_Seq;        /*0:正向    1:反向    2:往复    3:随机*/ 
    uint16_t Hue_Mini;
    uint16_t Hue_Max;       
    uint8_t Hue_Seq;        /*0:正向    1:反向    2:往复    3:随机*/
    uint8_t Sat_Max;        /*<0-20>: -1.0->0,-0.9->1,-0.8->2,   +1.0->20每隔0.1递增1，以此类推*/
    uint8_t Sat_Mini;       /*<0-20>: -1.0->0,-0.9->1,-0.8->2,   +1.0->20每隔0.1递增1，以此类推*/    
    uint8_t Sat_Seq;        /*0:正向    1:反向    2:往复    3:随机*/
}MFX_Base_HSI;

typedef  __packed union
{
    MFX_Base_CCT BaseCCT;
    MFX_Base_HSI BaseHSI;
}MFX_Base_Type;

typedef __packed struct 
{
    uint8_t LoopTimes;
    uint8_t LoopMode;               /**/
    uint32_t CycleTime_Mini;        /*0-100000    单位ms*/
    uint32_t CycleTime_Max;         /*0-100000    单位ms*/
    uint8_t CycleTime_Seq;          /*0:正向    1:反向    2:往复    3:随机*/
    uint32_t FreeTime_Mini;         /*0-100000    单位ms*/
    uint32_t FreeTime_Max;          /*0-100000    单位ms*/
    uint8_t FreeTime_Seq;           /*0:正向    1:反向    2:往复    3:随机*/
    uint32_t UnitTime_Mini;         /*0-100000    单位ms*/
    uint32_t UnitTime_Max;          /*0-100000    单位ms*/
    uint8_t UnitTime_Seq;           /*0:正向    1:反向    2:往复    3:随机*/
    uint8_t Frq_Mini;               /*1-129  <0.1-0.9Hz:1-9> <1-120Hz:10-129>*/
    uint8_t Frq_Max;                /*1-129  <0.1-0.9Hz:1-9> <1-120Hz:10-129>*/
    uint8_t Frq_Seq;                /*0:正向    1:反向    2:往复    3:随机*/
}MFX_Flash_ModeArg;

typedef __packed struct 
{ 
    uint8_t LoopTimes;
    uint8_t LoopMode;
    uint32_t CycleTime_Mini;
    uint32_t CycleTime_Max;
    uint8_t CycleTime_Seq;
    uint32_t FadeInTime_Mini;
    uint32_t FadeInTime_Max;
    uint8_t FadeInTime_Seq;
    uint8_t FadeIn_Curve;
    uint32_t FadeOutTime_Mini;
    uint32_t FadeOutTime_Max;
    uint8_t FadeOutTime_Seq;
    uint8_t FadeOut_Curve;
    uint8_t Flicker_Frq;
}MFX_Continue_ModeArg;

typedef __packed struct 
{ 
    uint8_t LoopTimes;
    uint8_t LoopMode;
    uint32_t CycleTime_Mini;
    uint32_t CycleTime_Max;
    uint8_t CycleTime_Seq; 
    uint32_t FreeTime_Mini;
    uint32_t FreeTime_Max;
    uint8_t FreeTime_Seq;  
    uint32_t UnitTime_Mini;
    uint32_t UnitTime_Max;
    uint8_t UnitTime_Seq;     
    uint32_t FadeInTime_Mini;
    uint32_t FadeInTime_Max;
    uint8_t FadeInTime_Seq;
    uint8_t FadeIn_Curve;
    uint32_t FadeOutTime_Mini;
    uint32_t FadeOutTime_Max;
    uint8_t FadeOutTime_Seq;
    uint8_t FadeOut_Curve;
    uint8_t Flicker_Frq;
    uint8_t OLP_Mini;
    uint8_t OLP_Max;
    uint8_t OLP_Seq;
    uint8_t OLR_Mini;
    uint8_t OLR_Max;
    uint8_t OLR_Seq;   
    uint8_t Overlap_Seq;
}MFX_Paragraph_ModeArg;

typedef __packed union
{
    MFX_Flash_ModeArg       Flash;
    MFX_Continue_ModeArg    Continue;
    MFX_Paragraph_ModeArg   Paragraph;
}MFX_ModeArg_Type;



typedef __packed struct
{
    MFX_Ctrl_Type       Ctrl;
    MFX_Mode_Type       Mode;
    MFX_Base_Type       BaseArg;
    MFX_ModeArg_Type    ModeArg;
}MFX_Arg_Type;

typedef enum
{
    PFX_Mode_Flash,
    PFX_Mode_Continue,
    PFX_Mode_Chase,
    PFX_Mode_NULL,
}PFX_Mode_Type;



typedef __packed struct
{
    uint8_t Base;
    uint16_t Times;
    uint8_t Frq;
    uint16_t Int;
    uint16_t CCT;
    uint8_t GM;
}Flash_Base_CCT;

typedef __packed struct
{
    uint8_t Base;
    uint16_t Times;
    uint8_t Frq;
    uint16_t Int;
    uint16_t Hue;
    uint8_t Sat;
}Flash_Base_HSI;

typedef __packed union 
{
    Flash_Base_CCT BaseCCT;
    Flash_Base_HSI BaseHSI;
}PFX_Flash_Arg;

typedef __packed struct
{
    uint8_t Base;               /*0x00:CCT   0x01:HSI*/
    uint16_t Int;               /*0-1000*/ 
    uint16_t CCT; 
    uint8_t GM;                 /*<0-20>: -1.0->0,-0.9->1,-0.8->2,   +1.0->20每隔0.1递增1，以此类推*/      
    uint32_t ContinueTime;      /*单位ms,大于100 * 1000ms 为always*/
    uint8_t FadeInCurve;        /*0:线性    1:指数    2:对数    3:s型*/ 
    uint32_t FadeInTime;        /*单位ms   */ 
    uint8_t FadeOutCurve;       /*0:线性    1:指数    2:对数    3:s型*/ 
    uint32_t FadeOutTime;   /*单位ms*/      
}Continue_Base_CCT;

typedef __packed struct{

    uint8_t Base;              /*0x00:CCT   0x01:HSI*/
    uint16_t Int;               /*0-1000*/ 
    uint16_t Hue; 
    uint8_t Sat;
    uint32_t ContinueTime;      /*单位ms,大于100 * 1000ms 为always*/
    uint8_t FadeInCurve;        /*0:线性    1:指数    2:对数    3:s型*/ 
    uint32_t FadeInTime;        /*单位ms   */ 
    uint8_t FadeOutCurve;       /*0:线性    1:指数    2:对数    3:s型*/ 
    uint32_t FadeOutTime;       /*单位ms   */ 
}Continue_Base_HSI;

typedef  __packed union 
{
    Continue_Base_CCT BaseCCT;
    Continue_Base_HSI BaseHSI;
}PFX_Continue_Arg;

typedef __packed struct
{   
    uint8_t Base;           /*0x00:CCT   0x01:HSI*/
    uint8_t loop;           /*0:一次   1:循环*/
    uint32_t Time;          /*<0-100000>  单位ms*/
    uint8_t GM_Seq;         /*0:正向    1:反向    2:往复    3:随机*/
    uint8_t CCT_Seq;        /*0:正向    1:反向    2:往复    3:随机*/
    uint8_t Int_Seq;        /*0:正向    1:反向    2:往复    3:随机*/    
    uint8_t GM_Max;         /*<0-20>: -1.0->0,-0.9->1,-0.8->2,   +1.0->20每隔0.1递增1，以此类推*/
    uint8_t GM_Mini;        /*<0-20>: -1.0->0,-0.9->1,-0.8->2,   +1.0->20每隔0.1递增1，以此类推*/
    uint16_t CCT_Max;
    uint16_t CCT_Mini;      
    uint16_t Int_Max;       /*0-1000*/
    uint16_t Int_Mini;      /*0-1000*/    
}Chase_Base_CCT;

typedef __packed struct
{
    uint8_t Base;           /*0x00:CCT   0x01:HSI*/
    uint8_t loop;           /*0:一次   1:循环*/
    uint32_t Time;          /*<0-100000>  单位ms*/
    uint8_t Sat_Seq;        /*0:正向    1:反向    2:往复    3:随机*/
    uint8_t Hue_Seq;        /*0:正向    1:反向    2:往复    3:随机*/
    uint8_t Int_Seq;        /*0:正向    1:反向    2:往复    3:随机*/
    uint8_t Sat_Max;        /*0-100*/  
    uint8_t Sat_Mini;       /*0-100*/  
    uint16_t Hue_Max;       /*0-360*/
    uint16_t Hue_Mini;      /*0-360*/
    uint16_t Int_Max;       /*0-1000*/
    uint16_t Int_Mini;      /*0-1000*/
}Chase_Base_HSI;

typedef  __packed union 
{
    Chase_Base_CCT BaseCCT;
    Chase_Base_HSI BaseHSI;
}PFX_ChaseArg;

typedef  __packed union 
{
    PFX_Continue_Arg    Continue;
    PFX_Flash_Arg       Flash;
    PFX_ChaseArg        Chase;
}PFX_ModeArg_Type;

typedef __packed struct 
{
    PFX_Mode_Type       Mode;
    PFX_ModeArg_Type    ModeArg;
}PFX_Arg_Type;


typedef enum
{
    CFX_CTRL_STOP,
    CFX_CTRL_INIT,
    CFX_CTRL_RUNNING,
    CFX_CTRL_PAUSE,
}CFX_Ctrl_Type;

typedef enum
{
    CFX_Type_Picker = 0,
    CFX_Type_TouchBar,
    CFX_Type_Music,
    CFX_Type_Num,
} CFX_Type_Type;

typedef enum
{
    CFX_Bank_1 = 0,
    CFX_Bank_2,
    CFX_Bank_3,
    CFX_Bank_4,
    CFX_Bank_5,
    CFX_Bank_6,
    CFX_Bank_7,
    CFX_Bank_8,
    CFX_Bank_9,
    CFX_Bank_10,
    CFX_Bank_Num,
} CFX_Bank_Type;


typedef __packed struct
{   
    uint8_t  Ctrl;          /*0:stop  1:init  2:running*/
    uint8_t  Loop;          /*0:一次 1:循环*/
    uint8_t  Sequence;      /*0:正序 1:反序 2:往复*/
    uint8_t  Chaos;         /*0%-100%*/
    uint8_t  Speed;         /*0=0.5X, 1=1X 2=2X, 3=4X 4=8X, 5=16X*/
    uint16_t  Int;          /*0-1000*/
    uint8_t  Bank;          /*外部flash当前自定义光效类型bank存储信息*/
    uint8_t  Effect_Type;   /*自定义光效类型*/
}CFX_Arg_Type;

typedef __packed struct
{
    uint8_t Base;           /*0:Base CCT    1:Base HSI*/
    uint16_t Int;           /*0-1000*/  /*max: 1023*/
    uint16_t CCT;           /*CCT乘以10，<2800-10000> 280->2800K,290->2900K, 300->3000K,依次类推*/  /*max: 1023*/
    uint8_t GM;             /*<0-20>: 0->-1.0,1->-0.9,2->-0.8,   20->+1.0每隔0.1递增1，以此类推*/  
}
CFX_Preview_BaseCCT;

typedef __packed struct
{  
    uint8_t Base;           /*0:Base CCT    1:Base HSI*/
    uint16_t Int;           /*0-1000*/  /*max: 1023*/
    uint16_t Hue;           /*0-360*/   /*max: 1023*/
    uint8_t Sat;            /*0-100*/   /*max: 127*/
}CFX_Preview_BaseHSI;

typedef __packed union
{
    CFX_Preview_BaseCCT BaseCCT;
    CFX_Preview_BaseHSI BaseHSI;
}CFX_Preview_Frame;

typedef __packed struct
{
    CFX_Preview_Frame Frame_2;
    CFX_Preview_Frame Frame_1;
}CFX_Preview_Type;

typedef enum 
{
    SidusPro_PFX,
    SidusPro_CFX,
    SidusPro_MFX,
    SidusPro_CFX_Preview,
    SidusPro_FX_Num,
}SidusProFX_Type;

typedef __packed union
{
    MFX_Arg_Type        MFX;
    PFX_Arg_Type        PFX; 
    CFX_Arg_Type        CFX;
    CFX_Preview_Type    CFX_Preview;
}SidusProFX_Arg_Type;

typedef __packed struct 
{
    SidusProFX_Type     SFX_Type;
    SidusProFX_Arg_Type SFX_Arg;
}SidusProFX_ArgBody_Type;


typedef __packed struct
{
    uint8_t Base;
    uint16_t Int;
    uint16_t CCT;
    uint8_t GM;
}SFX_SourceCCT_Type;

typedef __packed struct
{
    uint8_t Base;
    uint16_t Int;
    uint16_t Hue;
    uint8_t Sat;
}SFX_SourceHSI_Type;

typedef __packed union 
{
    SFX_SourceCCT_Type CCT;
    SFX_SourceHSI_Type HSI;
}SFX_Source_Type;

typedef __packed struct
{
    uint8_t Tag[16];        //文件标识符                               ---16Byte
    uint8_t TxMode;         //传输模式 0:SidusProFX                   ---1Byte
    uint16_t FxType;        //子模式 0:Picker 1:TouchBar 2:Music     ---2Byte
    uint16_t Supported;     //支持光效类型                              ---2Byte
    uint16_t UintTime;      //单位时间/采样率                            ---2Byte
    uint8_t Bars;           //小节格式                                ---1Byte
    uint32_t FrameNum;      //帧数                                  ---4Byte
    char     Name[10];      //光效名                                 ---10Byte
    uint8_t Reserve[85];    //头信息大小                               ---85Byte
    uint8_t FxBank;         //Bank区域                              ---1Byte
    uint32_t PwmDataCrc;    //pwm数据crc校验                          ---4Byte
} CFX_PwmFileHead_Type; //本地存储pwm数据头信息   ---128Byte


uint32_t SidusProFX_System_Init(void);//系统上电调用
void SidusProFX_Task(void);//光效任务循环调用
void SidusProFX_1Ms_Callback(void);//1ms中断中调用
void SidusProFX_Arg_Init(SidusProFX_ArgBody_Type* arg);//传参初始化光效参数并调用SidusProFX_Enable函数使能光效
void SidusProFX_Enable(bool en);//
bool SidusProFX_State(void);

#endif

