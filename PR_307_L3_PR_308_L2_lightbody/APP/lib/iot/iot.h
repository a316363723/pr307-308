/****************************************************************************************
**  Filename :  iot.h
**  Abstract :  APP与MCU通信的蓝牙IOT数据包格式。
**  By       :  何建国
**  Date     :  
**  Changelog:1.v1.0：2019/11/28,Jagger
                v1.1:2019/12/20,Jagger
                     数据转换已经完成验证；
                v1.2:2019/12/24,Jagger
                     解决了边界数据错误的问题；
                     v1.3:2020/5/5,Jagger
                     1、同步蓝牙通讯协议中的版本信息协议；（关于非常规光效的标记）
                     2、解决了IOT边界数据处理不善的问题；
*****************************************************************************************/
#ifndef _IOT_H
#define _IOT_H
/*****************************************************************************************
									Includes
*****************************************************************************************/
#include "stdint.h"
/*****************************************************************************************
								Macro definitions
*****************************************************************************************/

/*****************************************************************************************
								Typedef definitions
*****************************************************************************************/
#define IOT_COMMAND_TYPE            0X19
#define IOT_PACKET_SIZE      10     //次值必须与BT_PACKET_SIZE一致

typedef enum
{
    DEVICE_CLOSE = 0,
    DEVICE_OPEN,
    MAX_STATE_SERIAL,
}IOT_Device_State;
typedef enum
{
    ADAPTER_SUPPLY_TIME,
    ONE_BAT_SUPPLY_TIME,
    TWO_BAT_SUPPLY_TIME,
    THREE_BAT_SUPPLY_TIME,
    FOUR_BAT_SUPPLY_TIME,
    FIVE_BAT_SUPPLY_TIME,
    SIX_BAT_SUPPLY_TIME,
    MAX_RUM_TIME_TYPE,
}IOT_Run_Time_TypeDef;

typedef enum
{
    DMX_MODE_TIME = 0,
    CCT_MODE_TIME,
    GEL_MODE_TIME,
    HSI_MODE_TIME,
    XY_MODE_TIME,
    RGB_MODE_TIME,
    SFX_MODE_TIME,
    SIDUSPRO_FX_MODE_TIME,
    MAX_MODE_TIME_TYPE,
}IOT_Mode_Time_TypeDef;

typedef enum
{
    INT_TYPE1 = 0,  //INT = <0>
    INT_TYPE2,      //INT = <1-5>
    INT_TYPE3,      //INT = <6-10>
    INT_TYPE4,      //INT = <11-20>
    INT_TYPE5,      //INT = <21-30>
    INT_TYPE6,      //INT = <31-40>
    INT_TYPE7,      //INT = <41-50>
    INT_TYPE8,      //INT = <51-60>
    INT_TYPE9,      //INT = <61-70>
    INT_TYPE10,     //INT = <71-80>
    INT_TYPE11,     //INT = <81-90>
    INT_TYPE12,     //INT = <91-95>
    INT_TYPE13,     //INT = <96-99>
    INT_TYPE14,     //INT = <100>
    MAX_INT_TYPE,
}IOT_Int_Time_TypeDef;
//单次使用持续时间分布
typedef enum
{
    DURATION_TIME_TYPE1 = 0,    //duration <0.5-5> min
    DURATION_TIME_TYPE2,        //duration <5-10> min
    DURATION_TIME_TYPE3,        //duration <11-20> min
    DURATION_TIME_TYPE4,        //duration <21-30> min
    DURATION_TIME_TYPE5,        //duration <31-40> min
    DURATION_TIME_TYPE6,        //duration <41-50> min
    DURATION_TIME_TYPE7,        //duration <51-60> min
    DURATION_TIME_TYPE8,        //duration <61-70> min
    DURATION_TIME_TYPE9,        //duration <71-80> min
    DURATION_TIME_TYPE10,       //duration <81-90> min
    DURATION_TIME_TYPE11,       //duration <91-100> min
    DURATION_TIME_TYPE12,       //duration <101-110> min      
    DURATION_TIME_TYPE13,       //duration <111-120> min
    DURATION_TIME_TYPE14,       //duration <121-130> min
    DURATION_TIME_TYPE15,       //duration <131-140> min
    DURATION_TIME_TYPE16,       //duration <141-150> min
    DURATION_TIME_TYPE17,       //duration <151-160> min       
    DURATION_TIME_TYPE18,       //duration <161-170> min
    DURATION_TIME_TYPE19,       //duration <171-180> min
    DURATION_TIME_TYPE20,       //duration <181-190> min
    DURATION_TIME_TYPE21,       //duration <191-200> min
    DURATION_TIME_TYPE22,       //duration <201-210> min
    DURATION_TIME_TYPE23,       //duration <211-220> min
    DURATION_TIME_TYPE24,       //duration <221-230> min
    DURATION_TIME_TYPE25,       //duration <231-240> min
    DURATION_TIME_TYPE26,       //duration <241-300> min
    DURATION_TIME_TYPE27,       //duration <301-360> min
    DURATION_TIME_TYPE28,       //duration <361-420> min
    DURATION_TIME_TYPE29,       //duration <421-480> min
    DURATION_TIME_TYPE30,       //duration <481-540> min
    DURATION_TIME_TYPE31,       //duration <541-600> min
    DURATION_TIME_TYPE32,       //duration <>600> min
    MAX_DURATION_TIME_TYPE,
}IOT_Duration_Time_TypeDef;

typedef enum
{
    CTR_BOX_WAY = 0,
    CTR_BLE_WAY,
    MAX_CTR_WAY,
}IOT_Control_Way_TypeDef;

typedef enum
{
    NO_FRESNEL = 0,     //no
    FRESNEL_TYPE1,      //Fresnel <5>°
    FRESNEL_TYPE2,      //Fresnel <6-10>°
    FRESNEL_TYPE3,      //Fresnel <11-15>°
    FRESNEL_TYPE4,      //Fresnel <16-20>°
    FRESNEL_TYPE5,      //Fresnel <21-25>°
    FRESNEL_TYPE6,      //Fresnel <26-30>°
    FRESNEL_TYPE7,      //Fresnel <31-35>°
    FRESNEL_TYPE8,      //Fresnel <36-40>°
    FRESNEL_TYPE9,      //Fresnel <41-45>°
    FRESNEL_TYPE10,     //Fresnel <46-50>°
    FRESNEL_TYPE11,     //Fresnel <51-54>°
    FRESNEL_TYPE12,     //Fresnel <55>°
    MAX_FRESNEL_TYPE,
}IOT_Fresnel_Time_TypeDef;

typedef enum
{
    NO_PITCH_ANGLE = 0,     //NO
    PITCH_ANGLE_TYPE1,      //Pitch_Angle <-60>°
    PITCH_ANGLE_TYPE2,      //Pitch_Angle <-59 - -50>°
    PITCH_ANGLE_TYPE3,      //Pitch_Angle <-49 - -40>°
    PITCH_ANGLE_TYPE4,      //Pitch_Angle <-39 - -30>°
    PITCH_ANGLE_TYPE5,      //Pitch_Angle <-29 - -20>°
    PITCH_ANGLE_TYPE6,      //Pitch_Angle <-19 - -10>°
    PITCH_ANGLE_TYPE7,      //Pitch_Angle <-9 - -1>°
    PITCH_ANGLE_TYPE8,      //Pitch_Angle <0>°
    PITCH_ANGLE_TYPE9,      //Pitch_Angle <1 - 10>°
    PITCH_ANGLE_TYPE10,     //Pitch_Angle <11 - 20>°
    PITCH_ANGLE_TYPE11,     //Pitch_Angle <21 - 30>°
    PITCH_ANGLE_TYPE12,     //Pitch_Angle <31 - 40>°     
    PITCH_ANGLE_TYPE13,     //Pitch_Angle <41 - 50>°
    PITCH_ANGLE_TYPE14,     //Pitch_Angle <51 - 60>°
    PITCH_ANGLE_TYPE15,     //Pitch_Angle <61 - 70>°    
    PITCH_ANGLE_TYPE16,     //Pitch_Angle <71 - 80>°
    PITCH_ANGLE_TYPE17,     //Pitch_Angle <81 - 89>°
    PITCH_ANGLE_TYPE18,     //Pitch_Angle <90>°
    MAX_PITCH_ANGLE_TYPE,
}IOT_Pitch_Angle_Time_TypeDef;

typedef enum
{
    MANUAL_EFFECTS_TYPE = 0,
    TOUCHBAR_EFFECTS_TYPE, 
    MUSIC_EFFECTS_TYPE,
    PICKER_EFFECTS_TYPE,
    MAGIC_PROGRAM_TYPE,
    MAX_SIDUS_TYPE,
}IOT_SidusProFX_Time_TypeDef;

typedef enum
{
    ERROR_FAN1 = 0,
    ERROR_FAN2,
    ERROR_FAN3,
    ERROR_FAN4,
    ERROR_FAN5,
    ERROR_FAN6,
    ERROR_TEMP_SENSOR1,
    ERROR_TEMP_SENSOR2,
    ERROR_TEMP_SENSOR3,
    ERROR_TEMP_SENSOR4,
    ERROR_TEMP_SENSOR5,
    ERROR_TEMP_SENSOR6,
    ERROR_BAT1_VOLT,
    ERROR_BAT2_VOLT,
    ERROR_BAT3_VOLT,
    ERROR_BAT4_VOLT,
    ERROR_BAT5_VOLT,
    ERROR_BAT6_VOLT,
    ERROR_CTR_INPUT_VOLT,
    ERROR_DRV_INPUT_VOLT,
    ERROR_CTR_CURR,
    ERROR_DRV_CURR,
    ERROR_CTR_3D3V,
    ERROR_CTR_5V,
    ERROR_CTR_9V,
    ERROR_CTR_12V,
    ERROR_CTR_15V,
    ERROR_DRV_3D3V,
    ERROR_DRV_5V,
    ERROR_DRV_9V,
    ERROR_DRV_12V,
    ERROR_DRV_15V,
    ERROR_RS485,
    ERROR_STORE,
    ERROR_PD,
    ERROR_WIRELESS_PD,
    ERROR_HIGH_TEMP,
    ERROR_USB_UPDATE,
    ERROR_USB_MOUNT,
    ERROR_BLE_UPDATE,
    
    MAX_ERROR_TYPE,
}IOT_Error_Msg_TypeDef;

//参数序号状态，应用层改变值
typedef __packed struct
{
    IOT_Device_State device_state_serial;           //设备的开关机状态
    IOT_Run_Time_TypeDef run_time_serial;           //设备的供电状态
    IOT_Mode_Time_TypeDef mode_time_serial;         //设备的模式状态
    uint8_t int_val;                                //当前设备的亮度
    uint16_t fresnel_val;                            //当前设备的自动调焦角度
    int16_t pitch_angle_val;                         //当前设备的自动俯仰角度
    IOT_SidusProFX_Time_TypeDef sidus_pro_fx_serial;//当前设备的非常规光效状态
}IOT_Parameter_Serial_TypeDef;
//参数列表
typedef __packed struct
{
    uint32_t device_state_time[MAX_STATE_SERIAL];
    uint32_t run_time[MAX_RUM_TIME_TYPE];
    uint32_t mode_time[MAX_MODE_TIME_TYPE];
    uint32_t int_time[MAX_INT_TYPE];
    uint32_t duration_time[MAX_DURATION_TIME_TYPE];
    uint32_t control_way[MAX_CTR_WAY];
    uint32_t fresnel_time[MAX_FRESNEL_TYPE];
    uint32_t pitch_angle_time[MAX_PITCH_ANGLE_TYPE];
    uint32_t sidus_pro_fx[MAX_SIDUS_TYPE];
    uint16_t error_msg[MAX_ERROR_TYPE];
    uint32_t  store_serial;//存储序号，用于循环存储操作
    uint32_t  check_sum;
}IOT_Parameter_TypeDef;
//-----------------------------------IOT数据转换
typedef __packed struct{
	uint8_t  Command_Type 	:7;  	/*0-127*/ /*max: 127*/
	uint8_t  Opera_Type		:1;  	/*0-1*/   /*master: read:0  write:1; 
												slave:  ack:0;   nack:1; */
}IOT_Header_TypeDef; /*size: 1*/

//IOT数据总信息
typedef __packed struct{
    uint8_t 				    CheckSum;		/*校验和*/
    uint64_t Reserve            :49;
	uint64_t Frame_Num  		:8;  		/*IOT数据帧总数量，包含此数据包*/	
    uint64_t IOT_Type		    :7;  		/*IOT类型*/
    IOT_Header_TypeDef		    Header;	    /*数据头*/
}IOT_Msg_TypeDef;
//版本信息
typedef __packed struct{
    uint8_t 				    CheckSum;		/*校验和*/
    uint64_t Reserve            :27;
    uint64_t Driver_SwVer		:6;  		/*11:代表版本v1.1*/
	uint64_t Driver_HwVer		:6;  		/*11:代表版本v1.1*/
	uint64_t Control_SwVer		:6;  		/*11:代表版本v1.1*/
	uint64_t Control_HwVer		:6;  		/*11:代表版本v1.1*/
	uint64_t Protocol_Ver		:6;  		/*11:代表版本v1.1*/	
    uint64_t IOT_Type		    :7;  		/*IOT类型*/
    IOT_Header_TypeDef		    Header;			/*数据头*/
}IOT_Ver1_TypeDef;

typedef __packed struct{
    uint8_t 				    CheckSum;	/*校验和*/
    uint64_t Reserve            :25;
    uint64_t CFX_MusicFX_support    :1;
    uint64_t CFX_TouchbarFX_support :1;
    uint64_t CFX_PickerFX_support :1;
    uint64_t ProgramFX_support  :1;
    uint64_t ManualFX_support   :1;
	uint64_t Product_Machine	:4;  		/*0：无/SFL  1：变焦/VFL  2：变焦+航向+俯仰/TA*/
	uint64_t Product_CCTRange	:7;  		/*色温上限*/
	uint64_t Product_CCTRange_L	:7;  		/*色温下限*/
	uint64_t Product_LedType	:5;  		/*0：D; 1：T; 2：WW；3:WWG; 4：RGB; 5：WRGB; 6：WWRGB; 7:WWRGBCMYP*/
	uint64_t Product_Function	:4;  		/*0:单色温；1：双色温；2：简单彩色；3：全功能彩色*/
    uint64_t IOT_Type		    :7;  		/*IOT类型*/
    IOT_Header_TypeDef		    Header;		/*数据头*/
}IOT_Ver2_TypeDef;

//运行时间
typedef __packed struct{
    uint8_t 				    CheckSum;	/*校验和*/
    uint64_t Reserve            :3;        /*预留*/
    uint64_t Use_DC_Duty		:10;  		/*使用适配器时间占比*/
    uint64_t Off_Tim_Duty		:10;  		/*关机时间占比*/
	uint64_t Work_Tim_Duty		:10;  		/*开机时间占比*/
	uint64_t Rum_Tim		    :24;  		/*总上电时间*/
	uint64_t IOT_Type		    :7;  		/*IOT类型*/	
    IOT_Header_TypeDef		    Header;		/*数据头*/
}IOT_Run_Tim1_TypeDef;

typedef __packed struct{
    uint8_t 				    CheckSum;	/*校验和*/
    uint64_t Reserve            :7;        /*预留*/
    uint64_t BAT4_Duty          :10;        /*4个电池使用时间占比*/
    uint64_t BAT3_Duty          :10;        /*3个电池使用时间占比*/
    uint64_t BAT2_Duty          :10;        /*2个电池使用时间占比*/
    uint64_t BAT1_Duty          :10;        /*1个电池使用时间占比*/
    uint64_t Use_BAT_Duty		:10;  		/*使用电池时间占比*/
	uint64_t IOT_Type		    :7;  		/*IOT类型*/	
    IOT_Header_TypeDef		    Header;		/*数据头*/
}IOT_Run_Tim2_TypeDef;

typedef __packed struct{
    uint8_t 				    CheckSum;	/*校验和*/
    uint64_t Reserve            :37;         /*预留*/
    uint64_t BAT6_Duty          :10;        /*6个电池使用时间占比*/
    uint64_t BAT5_Duty          :10;        /*5个电池使用时间占比*/
	uint64_t IOT_Type		    :7;  		/*IOT类型*/	
    IOT_Header_TypeDef		    Header;		/*数据头*/
}IOT_Run_Tim3_TypeDef;
//各种模式使用时间
typedef __packed struct{
    uint8_t 				    CheckSum;	/*校验和*/
    uint64_t Reserve            :3;         /*预留*/
    uint64_t GEL_Duty		    :10;  		/*GEL时间占比*/
	uint64_t CCT_Duty		    :10;  		/*CCT时间占比*/
	uint64_t DMX_Duty		    :10;  		/*DMX时间占比*/
	uint64_t Work_Tim		    :24;  		/*总开机时间*/
	uint64_t IOT_Type		    :7;  		/*IOT类型*/	
    IOT_Header_TypeDef		    Header;		/*数据头*/
}IOT_Mode_Tim1_TypeDef;

typedef __packed struct{
    uint8_t 				    CheckSum;	/*校验和*/
    uint64_t Reserve            :7;         /*预留*/
    uint64_t SidusPro_FX_Duty   :10;  		/*SidusPro_FX时间占比*/
	uint64_t SFX_Duty		    :10;  		/*SFX时间占比*/
	uint64_t RGB_Duty		    :10;  		/*RGB时间占比*/
	uint64_t XY_Duty		    :10;  		/*XY时间占比*/
    uint64_t HSI_Duty		    :10;  		/*HSI时间占比*/
	uint64_t IOT_Type		    :7;  		/*IOT类型*/	
    IOT_Header_TypeDef		    Header;		/*数据头*/
}IOT_Mode_Tim2_TypeDef;
//亮度分布
typedef __packed struct{
    uint8_t 				    CheckSum;	/*校验和*/
    uint64_t Reserve            :7;         /*预留*/
    uint64_t INT5_Duty          :10;  		/*亮度<21-30>*/
	uint64_t INT4_Duty		    :10;  		/*亮度<11-20>*/
	uint64_t INT3_Duty		    :10;  		/*亮度<6-10>*/
	uint64_t INT2_Duty		    :10;  		/*亮度<1-5>*/
    uint64_t INT1_Duty		    :10;  		/*亮度<0>*/
	uint64_t IOT_Type		    :7;  		/*IOT类型*/	
    IOT_Header_TypeDef		    Header;		/*数据头*/
}IOT_INT_Tim1_TypeDef;
typedef __packed struct{
    uint8_t 				    CheckSum;	/*校验和*/
    uint64_t Reserve            :7;         /*预留*/
    uint64_t INT10_Duty         :10;  		/*亮度<71-80>*/
	uint64_t INT9_Duty		    :10;  		/*亮度<61-70>*/
	uint64_t INT8_Duty		    :10;  		/*亮度<51-60>*/
	uint64_t INT7_Duty		    :10;  		/*亮度<41-50>*/
    uint64_t INT6_Duty		    :10;  		/*亮度<31-40>*/
	uint64_t IOT_Type		    :7;  		/*IOT类型*/	
    IOT_Header_TypeDef		    Header;		/*数据头*/
}IOT_INT_Tim2_TypeDef;
typedef __packed struct{
    uint8_t 				    CheckSum;	/*校验和*/
    uint64_t Reserve            :17;         /*预留*/
	uint64_t INT14_Duty		    :10;  		/*亮度<100>*/
	uint64_t INT13_Duty		    :10;  		/*亮度<96-99>*/
	uint64_t INT12_Duty		    :10;  		/*亮度<91-95>*/
    uint64_t INT11_Duty		    :10;  		/*亮度<81-90>*/
	uint64_t IOT_Type		    :7;  		/*IOT类型*/	
    IOT_Header_TypeDef		    Header;		/*数据头*/
}IOT_INT_Tim3_TypeDef;
//单次工作时间分布
typedef __packed struct{
    uint8_t 				        CheckSum;	/*校验和*/
    uint64_t Reserve                :3;         /*预留*/
    uint64_t Duration_Time3_Duty    :10;  		/*持续工作时间<11-20>分钟*/
	uint64_t Duration_Time2_Duty    :10;  		/*持续工作时间<6-10>分钟*/
	uint64_t Duration_Time1_Duty    :10;  		/*持续工作时间<0.5-5>分钟*/
	uint64_t Work_Times		        :24;  		/*总开机次数*/
	uint64_t IOT_Type		        :7;  		/*IOT类型*/	
    IOT_Header_TypeDef		        Header;		/*数据头*/
}IOT_Duration_Tim1_TypeDef;
typedef __packed struct{
    uint8_t 				        CheckSum;	/*校验和*/
    uint64_t Reserve                :7;         /*预留*/
    uint64_t Duration_Time8_Duty    :10;  		/*持续工作时间<61-70>分钟*/
	uint64_t Duration_Time7_Duty    :10;  		/*持续工作时间<51-60>分钟*/
	uint64_t Duration_Time6_Duty    :10;  		/*持续工作时间<41-50>分钟*/
    uint64_t Duration_Time5_Duty    :10;  		/*持续工作时间<31-40>分钟*/
    uint64_t Duration_Time4_Duty    :10;  		/*持续工作时间<21-30>分钟*/
	uint64_t IOT_Type		        :7;  		/*IOT类型*/	
    IOT_Header_TypeDef		        Header;		/*数据头*/
}IOT_Duration_Tim2_TypeDef;
typedef __packed struct{
    uint8_t 				        CheckSum;	/*校验和*/
    uint64_t Reserve                :7;         /*预留*/
    uint64_t Duration_Time13_Duty   :10;  		/*持续工作时间<111-120>分钟*/
	uint64_t Duration_Time12_Duty   :10;  		/*持续工作时间<101-110>分钟*/
	uint64_t Duration_Time11_Duty   :10;  		/*持续工作时间<91-100>分钟*/
    uint64_t Duration_Time10_Duty   :10;  		/*持续工作时间<81-90>分钟*/
    uint64_t Duration_Time9_Duty    :10;  		/*持续工作时间<71-80>分钟*/
	uint64_t IOT_Type		        :7;  		/*IOT类型*/	
    IOT_Header_TypeDef		        Header;		/*数据头*/
}IOT_Duration_Tim3_TypeDef;
typedef __packed struct{
    uint8_t 				        CheckSum;	/*校验和*/
    uint64_t Reserve                :7;         /*预留*/
    uint64_t Duration_Time18_Duty   :10;  		/*持续工作时间<161-170>分钟*/
	uint64_t Duration_Time17_Duty   :10;  		/*持续工作时间<151-160>分钟*/
	uint64_t Duration_Time16_Duty   :10;  		/*持续工作时间<141-150>分钟*/
    uint64_t Duration_Time15_Duty   :10;  		/*持续工作时间<131-140>分钟*/
    uint64_t Duration_Time14_Duty   :10;  		/*持续工作时间<121-130>分钟*/
	uint64_t IOT_Type		        :7;  		/*IOT类型*/	
    IOT_Header_TypeDef		        Header;		/*数据头*/
}IOT_Duration_Tim4_TypeDef;
typedef __packed struct{
    uint8_t 				        CheckSum;	/*校验和*/
    uint64_t Reserve                :7;         /*预留*/
    uint64_t Duration_Time23_Duty   :10;  		/*持续工作时间<211-220>分钟*/
	uint64_t Duration_Time22_Duty   :10;  		/*持续工作时间<201-210>分钟*/
	uint64_t Duration_Time21_Duty   :10;  		/*持续工作时间<191-200>分钟*/
    uint64_t Duration_Time20_Duty   :10;  		/*持续工作时间<181-190>分钟*/
    uint64_t Duration_Time19_Duty   :10;  		/*持续工作时间<171-180>分钟*/
	uint64_t IOT_Type		        :7;  		/*IOT类型*/	
    IOT_Header_TypeDef		        Header;		/*数据头*/
}IOT_Duration_Tim5_TypeDef;
typedef __packed struct{
    uint8_t 				        CheckSum;	/*校验和*/
    uint64_t Reserve                :7;         /*预留*/
    uint64_t Duration_Time28_Duty   :10;  		/*持续工作时间<361-420>分钟*/
	uint64_t Duration_Time27_Duty   :10;  		/*持续工作时间<301-360>分钟*/
	uint64_t Duration_Time26_Duty   :10;  		/*持续工作时间<241-300>分钟*/
    uint64_t Duration_Time25_Duty   :10;  		/*持续工作时间<231-240>分钟*/
    uint64_t Duration_Time24_Duty   :10;  		/*持续工作时间<221-230>分钟*/
	uint64_t IOT_Type		        :7;  		/*IOT类型*/	
    IOT_Header_TypeDef		        Header;		/*数据头*/
}IOT_Duration_Tim6_TypeDef;
typedef __packed struct{
    uint8_t 				        CheckSum;	/*校验和*/
    uint64_t Reserve                :17;        /*预留*/
	uint64_t Duration_Time32_Duty   :10;  		/*持续工作时间<>600>分钟*/
	uint64_t Duration_Time31_Duty   :10;  		/*持续工作时间<541-600>分钟*/
    uint64_t Duration_Time30_Duty   :10;  		/*持续工作时间<481-540>分钟*/
    uint64_t Duration_Time29_Duty   :10;  		/*持续工作时间<421-480>分钟*/
	uint64_t IOT_Type		        :7;  		/*IOT类型*/	
    IOT_Header_TypeDef		        Header;		/*数据头*/
}IOT_Duration_Tim7_TypeDef;
//控制方式分布
typedef __packed struct{
    uint8_t 				        CheckSum;	/*校验和*/
    uint64_t Reserve                :13;        /*预留*/
	uint64_t Ble_Ctr_Duty           :10;  		/*蓝牙控制次数占比*/
    uint64_t Control_Box_Duty       :10;  		/*控制盒控制次数占比*/
    uint64_t Control_Times          :24;  		/*总控制次数*/
	uint64_t IOT_Type		        :7;  		/*IOT类型*/	
    IOT_Header_TypeDef		        Header;		/*数据头*/
}IOT_Control_Times_TypeDef;
//电动调焦角度分布
typedef __packed struct{
    uint8_t 				        CheckSum;	/*校验和*/
    uint64_t Reserve                :3;         /*预留*/
    uint64_t Fresnel2_Duty          :10;  		/*6-10°自动调焦占比*/
	uint64_t Fresnel1_Duty          :10;  		/*5°自动调焦占比*/
	uint64_t No_Fresnel_Duty        :10;  		/*没有自动调焦占比*/
	uint64_t Work_Time		        :24;  		/*工作时间*/
	uint64_t IOT_Type		        :7;  		/*IOT类型*/	
    IOT_Header_TypeDef		        Header;		/*数据头*/
}IOT_Fresnel_Tim1_TypeDef;
typedef __packed struct{
    uint8_t 				        CheckSum;	/*校验和*/
    uint64_t Reserve                :7;        /*预留*/
	uint64_t Fresnel7_Duty          :10;  		/*31-35°自动调焦占比*/
	uint64_t Fresnel6_Duty          :10;  		/*26-30°自动调焦占比*/
	uint64_t Fresnel5_Duty          :10;  		/*21-25°自动调焦占比*/
    uint64_t Fresnel4_Duty          :10;  		/*16-20°自动调焦占比*/
    uint64_t Fresnel3_Duty          :10;  		/*11-15°自动调焦占比*/
	uint64_t IOT_Type		        :7;  		/*IOT类型*/	
    IOT_Header_TypeDef		        Header;		/*数据头*/
}IOT_Fresnel_Tim2_TypeDef;
typedef __packed struct{
    uint8_t 				        CheckSum;	/*校验和*/
    uint64_t Reserve                :7;         /*预留*/
	uint64_t Fresnel12_Duty         :10;  		/*55°自动调焦占比*/
	uint64_t Fresnel11_Duty         :10;  		/*51-55°自动调焦占比*/
	uint64_t Fresnel10_Duty         :10;  		/*46-50°自动调焦占比*/
    uint64_t Fresnel9_Duty          :10;  		/*41-45°自动调焦占比*/
    uint64_t Fresnel8_Duty          :10;  		/*36-40°自动调焦占比*/
	uint64_t IOT_Type		        :7;  		/*IOT类型*/	
    IOT_Header_TypeDef		        Header;		/*数据头*/
}IOT_Fresnel_Tim3_TypeDef;
//电动俯仰角度分布
typedef __packed struct{
    uint8_t 				        CheckSum;	/*校验和*/
    uint64_t Reserve                :3;         /*预留*/
    uint64_t Pitch_Angle2_Duty      :10;  		/*-59 - -50°自动俯仰角占比*/
	uint64_t Pitch_Angle1_Duty      :10;  		/*-60°自动俯仰角占比*/
	uint64_t No_Pitch_Angle_Duty    :10;  		/*没有自动俯仰角占比*/
	uint64_t Work_Time		        :24;  		/*工作时间*/
	uint64_t IOT_Type		        :7;  		/*IOT类型*/	
    IOT_Header_TypeDef		        Header;		/*数据头*/
}IOT_Pitch_Angle_Tim1_TypeDef;
typedef __packed struct{
    uint8_t 				        CheckSum;	/*校验和*/
    uint64_t Reserve                :7;         /*预留*/
	uint64_t Pitch_Angle7_Duty      :10;  		/*-9 - -1°自动俯仰角占比*/
	uint64_t Pitch_Angle6_Duty      :10;  		/*-19 - -10°自动俯仰角占比*/
	uint64_t Pitch_Angle5_Duty      :10;  		/*-29 - -20°自动俯仰角占比*/
    uint64_t Pitch_Angle4_Duty      :10;  		/*-39 - -30°自动俯仰角占比*/
    uint64_t Pitch_Angle3_Duty      :10;  		/*-49 - -40°自动俯仰角占比*/
	uint64_t IOT_Type		        :7;  		/*IOT类型*/	
    IOT_Header_TypeDef		        Header;		/*数据头*/
}IOT_Pitch_Angle_Tim2_TypeDef;
typedef __packed struct{
    uint8_t 				        CheckSum;	/*校验和*/
    uint64_t Reserve                :7;         /*预留*/
	uint64_t Pitch_Angle12_Duty     :10;  		/*31 - 40°自动俯仰角占比*/
	uint64_t Pitch_Angle11_Duty     :10;  		/*21 - 30°自动俯仰角占比*/
	uint64_t Pitch_Angle10_Duty     :10;  		/*11 - 20°自动俯仰角占比*/
    uint64_t Pitch_Angle9_Duty      :10;  		/*1 - 10°自动俯仰角占比*/
    uint64_t Pitch_Angle8_Duty      :10;  		/*0°自动俯仰角占比*/
	uint64_t IOT_Type		        :7;  		/*IOT类型*/	
    IOT_Header_TypeDef		        Header;		/*数据头*/
}IOT_Pitch_Angle_Tim3_TypeDef;
typedef __packed struct{
    uint8_t 				        CheckSum;	/*校验和*/
    uint64_t Reserve                :7;         /*预留*/
	uint64_t Pitch_Angle17_Duty     :10;  		/*81 - 89°自动俯仰角占比*/
	uint64_t Pitch_Angle16_Duty     :10;  		/*71 - 80°自动俯仰角占比*/
	uint64_t Pitch_Angle15_Duty     :10;  		/*61 - 70°自动俯仰角占比*/
    uint64_t Pitch_Angle14_Duty     :10;  		/*51 - 60°自动俯仰角占比*/
    uint64_t Pitch_Angle13_Duty     :10;  		/*41 - 50°自动俯仰角占比*/
	uint64_t IOT_Type		        :7;  		/*IOT类型*/	
    IOT_Header_TypeDef		        Header;		/*数据头*/
}IOT_Pitch_Angle_Tim4_TypeDef;
typedef __packed struct{
    uint8_t 				        CheckSum;	/*校验和*/
    uint64_t Reserve                :47;         /*预留*/
	uint64_t Pitch_Angle18_Duty     :10;  		/*81 - 89°自动俯仰角占比*/
	uint64_t IOT_Type		        :7;  		/*IOT类型*/	
    IOT_Header_TypeDef		        Header;		/*数据头*/
}IOT_Pitch_Angle_Tim5_TypeDef;
//SidusPro_FX时间分布
typedef __packed struct{
    uint8_t 				        CheckSum;	/*校验和*/
    uint64_t Reserve                :7;         /*预留*/
	uint64_t Magic_Program_Duty     :10;  		/*占比*/
	uint64_t PickerEffects_Duty     :10;  		/*占比*/
	uint64_t MusicEffects_Duty      :10;  		/*占比*/
    uint64_t Touchbar_Effects_Duty  :10;  		/*占比*/
    uint64_t Manual_Effects_Duty    :10;  		/*占比*/
	uint64_t IOT_Type		        :7;  		/*IOT类型*/	
    IOT_Header_TypeDef		        Header;		/*数据头*/
}IOT_SidusPro_FX_Tim_TypeDef;
//ERROR MSG
typedef __packed struct{
    uint8_t 				        CheckSum;	/*校验和*/
    uint64_t Reserve                :3;         /*预留*/
    uint64_t Fan6_Error             :9;  		/*风扇6故障*/
	uint64_t Fan5_Error             :9;  		/*风扇5故障*/
	uint64_t Fan4_Error             :9;  		/*风扇4故障*/
	uint64_t Fan3_Error             :9;  		/*风扇3故障*/
    uint64_t Fan2_Error             :9;  		/*风扇2故障*/
    uint64_t Fan1_Error             :9;  		/*风扇1故障*/
	uint64_t IOT_Type		        :7;  		/*IOT类型*/	
    IOT_Header_TypeDef		        Header;		/*数据头*/
}IOT_Error_Msg1_TypeDef;
typedef __packed struct{
    uint8_t 				        CheckSum;	/*校验和*/
    uint64_t Reserve                :3;         /*预留*/
    uint64_t Temp_Sensor6_Error     :9;  		/*温度传感器6故障*/
	uint64_t Temp_Sensor5_Error     :9;  		/*温度传感器5故障*/
	uint64_t Temp_Sensor4_Error     :9;  		/*温度传感器4故障*/
	uint64_t Temp_Sensor3_Error     :9;  		/*温度传感器3故障*/
    uint64_t Temp_Sensor2_Error     :9;  		/*温度传感器2故障*/
    uint64_t Temp_Sensor1_Error     :9;  		/*温度传感器1故障*/
	uint64_t IOT_Type		        :7;  		/*IOT类型*/	
    IOT_Header_TypeDef		        Header;		/*数据头*/
}IOT_Error_Msg2_TypeDef;
typedef __packed struct{
    uint8_t 				        CheckSum;	/*校验和*/
    uint64_t Reserve                :3;         /*预留*/
    uint64_t Bat6_Volt_Error        :9;  		/*电池6电压故障*/
	uint64_t Bat5_Volt_Error        :9;  		/*电池5电压故障*/
	uint64_t Bat4_Volt_Error        :9;  		/*电池4电压故障*/
	uint64_t Bat3_Volt_Error        :9;  		/*电池3电压故障*/
    uint64_t Bat2_Volt_Error        :9;  		/*电池2电压故障*/
    uint64_t Bat1_Volt_Error        :9;  		/*电池1电压故障*/
	uint64_t IOT_Type		        :7;  		/*IOT类型*/	
    IOT_Header_TypeDef		        Header;		/*数据头*/
}IOT_Error_Msg3_TypeDef;
typedef __packed struct{
    uint8_t 				        CheckSum;	/*校验和*/
    uint64_t Reserve                :3;         /*预留*/
    uint64_t Ctr_Volt_5V_Error      :9;  		/*控制盒5V电压故障*/
    uint64_t Ctr_Volt_3d3V_Error    :9;  		/*控制盒3.3V电压故障 23*/
	uint64_t Drv_Input_Curr_Error   :9;  		/*灯体电流故障*/
	uint64_t Ctr_Input_Curr_Error   :9;  		/*控制盒电流故障*/
    uint64_t Drv_Input_Volt_Error   :9;  		/*灯体输入电压故障*/
    uint64_t Ctr_Input_Volt_Error   :9;  		/*控制盒输入电压故障*/
	uint64_t IOT_Type		        :7;  		/*IOT类型*/	
    IOT_Header_TypeDef		        Header;		/*数据头*/
}IOT_Error_Msg4_TypeDef;
typedef __packed struct{
    uint8_t 				        CheckSum;	/*校验和*/
    uint64_t Reserve                :3;  		/*预留*/
    uint64_t Drv_Volt_9V_Error      :9;  		/*灯体9V电压故障*/
    uint64_t Drv_Volt_5V_Error      :9;  		/*灯体5V电压故障*/
    uint64_t Drv_Volt_3d3V_Error    :9;  		/*灯体3.3V电压故障28*/
	uint64_t Ctr_Volt_15V_Error     :9;  		/*控制盒15V电压故障*/
	uint64_t Ctr_Volt_12V_Error     :9;  		/*控制盒12V电压故障*/
	uint64_t Ctr_Volt_9V_Error      :9;  		/*控制盒9V电压故障*/
	uint64_t IOT_Type		        :7;  		/*IOT类型*/	
    IOT_Header_TypeDef		        Header;		/*数据头*/
}IOT_Error_Msg5_TypeDef;
typedef __packed struct{
    uint8_t 				        CheckSum;	/*校验和*/
    uint64_t Reserve                :3;  		/*预留*/
    uint64_t PD_Wireless_Error      :9;  		/*无线快充故障*/
	uint64_t PD_Error               :9;  		/*快充故障*/
    uint64_t Stroe_Error            :9;  		/*存储故障*/
    uint64_t RS485_Error            :9;  		/*RS485通讯故障33*/
	uint64_t Drv_Volt_15V_Error     :9;  		/*灯体15V电压故障*/
	uint64_t Drv_Volt_12V_Error     :9;  		/*灯体12V电压故障*/
	uint64_t IOT_Type		        :7;  		/*IOT类型*/	
    IOT_Header_TypeDef		        Header;		/*数据头*/
}IOT_Error_Msg6_TypeDef;
typedef __packed struct{
    uint8_t 				        CheckSum;	/*校验和*/
    uint64_t Reserve                :21;  		/*预留*/
    uint64_t Ble_Update_Error       :9;  		/*蓝牙升级故障*/
    uint64_t USB_Mount_Error        :9;  		/*USB挂载故障*/
    uint64_t USB_Update_Error       :9;  		/*USB升级故障*/
	uint64_t High_Temp_Error        :9;  		/*高温关机故障37*/
	uint64_t IOT_Type		        :7;  		/*IOT类型*/	
    IOT_Header_TypeDef		        Header;		/*数据头*/
}IOT_Error_Msg7_TypeDef;
//每种IOT数据对应的帧数
typedef enum{
    IOT_MSG_NUM = 1,
    IOT_VER_NUM = 2,
    IOT_RUN_TIM_NUM  = 3,
    IOT_MODE_TIM_NUM = 2,
    IOT_INT_TIM_NUM = 3,
    IOT_DURATION_TIM_NUM = 7,
    IOT_CONTROL_TIMES_NUM = 1,
    IOT_FRESNEL_TIM_NUM = 3,
    IOT_PITCH_ANGLE_TIM_NUM = 5,
    IOT_SIDUSPRO_FX_TIM_NUM = 1,
    IOT_ERROR_NUM = 7,
}IOT_Frame_Num_TypeDef;
//IOT数据总帧数
#define IOT_FRAME_NUM (IOT_MSG_NUM + IOT_VER_NUM + IOT_RUN_TIM_NUM + IOT_MODE_TIM_NUM + IOT_INT_TIM_NUM + \
                       IOT_DURATION_TIM_NUM + IOT_CONTROL_TIMES_NUM + IOT_FRESNEL_TIM_NUM + \
                       IOT_PITCH_ANGLE_TIM_NUM + IOT_SIDUSPRO_FX_TIM_NUM + IOT_ERROR_NUM)
#define IOT_FRAME_TYPE_NUM  11 //IOT帧类型数量
#pragma anon_unions
typedef __packed union{
    uint8_t buf[IOT_FRAME_NUM][IOT_PACKET_SIZE];
    __packed struct{
        IOT_Msg_TypeDef Msg;
        IOT_Ver1_TypeDef Ver1;
        IOT_Ver2_TypeDef Ver2;
        IOT_Run_Tim1_TypeDef Run_Tim1;
        IOT_Run_Tim2_TypeDef Run_Tim2;
        IOT_Run_Tim3_TypeDef Run_Tim3;
        IOT_Mode_Tim1_TypeDef Mode_Tim1;
        IOT_Mode_Tim2_TypeDef Mode_Tim2;
        IOT_INT_Tim1_TypeDef INT_Tim1;
        IOT_INT_Tim2_TypeDef INT_Tim2;
        IOT_INT_Tim3_TypeDef INT_Tim3;
        IOT_Duration_Tim1_TypeDef Duration_Tim1;
        IOT_Duration_Tim2_TypeDef Duration_Tim2;
        IOT_Duration_Tim3_TypeDef Duration_Tim3;
        IOT_Duration_Tim4_TypeDef Duration_Tim4;
        IOT_Duration_Tim5_TypeDef Duration_Tim5;
        IOT_Duration_Tim6_TypeDef Duration_Tim6;
        IOT_Duration_Tim7_TypeDef Duration_Tim7;
        IOT_Control_Times_TypeDef Control_Times;
        IOT_Fresnel_Tim1_TypeDef Fresnel_Tim1;
        IOT_Fresnel_Tim2_TypeDef Fresnel_Tim2;
        IOT_Fresnel_Tim3_TypeDef Fresnel_Tim3;
        IOT_Pitch_Angle_Tim1_TypeDef Pitch_Angle_Tim1;
        IOT_Pitch_Angle_Tim2_TypeDef Pitch_Angle_Tim2;
        IOT_Pitch_Angle_Tim3_TypeDef Pitch_Angle_Tim3;
        IOT_Pitch_Angle_Tim4_TypeDef Pitch_Angle_Tim4;
        IOT_Pitch_Angle_Tim5_TypeDef Pitch_Angle_Tim5;
        IOT_SidusPro_FX_Tim_TypeDef SidusPro_FX_Tim;
        IOT_Error_Msg1_TypeDef Error_Msg1;
        IOT_Error_Msg2_TypeDef Error_Msg2;
        IOT_Error_Msg3_TypeDef Error_Msg3;
        IOT_Error_Msg4_TypeDef Error_Msg4;
        IOT_Error_Msg5_TypeDef Error_Msg5;
        IOT_Error_Msg6_TypeDef Error_Msg6;
        IOT_Error_Msg7_TypeDef Error_Msg7;
    };
}IOT_Send_Data_TypeDef;
//-----------------------------
typedef __packed struct
{
    uint8_t Driver_SwVer;  		/*11:代表版本v1.1*/
	uint8_t Driver_HwVer;  		/*11:代表版本v1.1*/
	uint8_t Control_SwVer;      /*11:代表版本v1.1*/
	uint8_t Control_HwVer;      /*11:代表版本v1.1*/
    uint8_t CFX_MusicFX_support;			/*预留*/
    uint8_t CFX_TouchbarFX_support;
    uint8_t CFX_PickerFX_support;
    uint8_t ProgramFX_support;
    uint8_t ManualFX_support;
	uint8_t Product_Machine;    /*0：无/SFL  1：变焦/VFL  2：变焦+航向+俯仰/TA*/
	uint8_t Product_CCTRange;   /*色温上限*/
	uint8_t Product_CCTRange_L; /*色温下限*/
	uint8_t Product_LedType;    /*0：D; 1：T; 2：WW；3:WWG; 4：RGB; 5：WRGB; 6：WWRGB; 7:WWRGBCMYP*/
	uint8_t Product_Function;   /*0:单色温；1：双色温；2：简单彩色；3：全功能彩色*/
	uint8_t Protocol_Ver;       /*11:代表版本v1.1*/	
}Device_Ver_TypeDef;
/*****************************************************************************************
							Global variables and functions
*****************************************************************************************/
extern IOT_Send_Data_TypeDef g_iot_send_data;
extern Device_Ver_TypeDef App_Data_Struct;
//----------------------------------------------------------------------------------------
//-----------------------------------IOT数据记录
extern void dispose_iot_parameter_data(IOT_Parameter_Serial_TypeDef* p_parameter_serial);
extern void set_iot_error_data(IOT_Error_Msg_TypeDef error_msg);
extern void set_ctr_way_msg(IOT_Control_Way_TypeDef control_way_serial);
//-----------------------------------IOT数据存储
extern void get_iot_data(uint8_t* p_iot_data);
extern void recover_iot_data(uint8_t* p_iot_data);
extern void recover_run_time(void);
//-----------------------------------IOT数据转换和发送
extern void set_device_version_msg(Device_Ver_TypeDef* p_ver);
extern uint8_t get_send_data_serial(uint8_t iot_type);
#endif
/***********************************END OF FILE*******************************************/
