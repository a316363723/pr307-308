/*********************************************************************************
  *Copyright(C), 2018, Aputure, All rights reserver.
  *FileName:  	light_effect.h
  *Author:    	
  *Version:   	v1.3
  *Date:      	2021/05/19
  *Description: 光效模块
  *History:  
			-v1.1: 2019/08/20,Steven,初始版本
			-v1.2: 2019/08/27,Steven,添加source接口,pulsing、lightning添加speed
			-v1.3: 定位光效增加运行状态控制run
**********************************************************************************/

#ifndef __LIGHT_EFFECT_H
#define __LIGHT_EFFECT_H

#include <stdint.h>
#include <stdbool.h>

/*---光效类型---*/
typedef enum{
	
	EffectTypeClubLights = 0,
	EffectTypePaparazzi,
	EffectTypeLightning,
	EffectTypeTV,
	EffectTypeCandle,
	EffectTypeFire,
	EffectTypeStrobe,
	EffectTypeExplosion,
	EffectTypeFaultBulb,
	EffectTypePulsing,
	EffectTypeWelding,
	EffectTypeCopCar,
	EffectTypeColorChase,
	EffectTypePartyLights,
	EffectTypeFireworks,
	EffectTypeEffectOff,
	EffectTypeIAmHere,
//	EffectTypeColorFade,
//	EffectTypeColorCycle,
//	EffectTypeColorGradient,
//	EffectTypeOneColorChase,
	EffectTypeDMXStrobe,
	EffectTypePaparazziII,
	EffectTypeLightningII,
	EffectTypeTVII,
	EffectTypeCandleII,
	EffectTypeFireII,
	EffectTypeStrobeII,
	EffectTypeExplosionII,
	EffectTypeFaultBulbII,
	EffectTypePulsingII,
	EffectTypeWeldingII,
	EffectTypeCopCarII,
	EffectTypePartyLightsII,
	EffectTypeFireworksII,
	EffectTypeLightningIII,
	EffectTypeTVIII,
	EffectTypeFireIII,
	EffectTypeFaultBulbIII,
	EffectTypePulsingIII,
	EffectTypeCopCarIII,
	EffectTypeNULL,
}Effect_Type;

/*---光效里的子类型---*/
typedef enum{
	
	EffectModeCCT = 0,
	EffectModeHSI,
	EffectModeGEL,
	EffectModeCoord,
	EffectModeSource,
	EffectModeBlack,
	EffectModeRGB,
	EffectModeRGBWW,
	EffectModeColorMixing,
	EffectModeNULL,
}Effect_Mode;

/*---光效触发类型---*/
typedef enum{
	
	Trigger_None = 0,
	Trigger_Once,
	Trigger_Continue
}Effect_TriggerType;

#pragma pack (1)
typedef struct{
	
	uint16_t  lightness;  		/*亮度<0-1000>*/
	uint16_t  hue;  			/*hue<0-360>，0-360,360-0*/
	uint8_t   sat;  			/*饱和度<0-100>*/
	uint16_t  cct;  			/*中心色温*/
}Effect_HSI; 				

typedef struct{
	
	uint16_t  lightness;  		/*亮度<0-1000>*/
	uint16_t  cct;  			/*CCT，单位k*/
	uint8_t   gm;   			/*<0-20>: 0->-1.0,1->-0.9,2->-0.8,   10->0,20->+1.0每隔0.1递增1，以此类推*/
}Effect_CCT; 				

typedef struct{
	
	uint16_t  lightness;  		/*亮度<0-1000>*/
	uint16_t  cct;  			/*CCT，单位k*/
	uint8_t   brand;   			/*brand ，品牌，0x00：LEE; 0x01：Resco*/
	uint8_t   type; 			/*type*/
	uint16_t  color; 			/*color*/
}Effect_Gel; 				

typedef struct{
	
	uint16_t  lightness;  		/*亮度<0-1000>*/
	uint16_t  x;  				/*x色坐标<0-10000>，除以10000表示实际色坐标*/
	uint16_t  y;  				/*y色坐标<0-10000>，除以10000表示实际色坐标*/
}Effect_Coordinate; 	

typedef struct{
	
	uint16_t  lightness;  		/*亮度<0-1000>*/
	uint8_t   type;  			/*source类型*/
	uint16_t  x;  				/*x色坐标<0-10000>，除以10000表示实际色坐标*/
	uint16_t  y;  				/*y色坐标<0-10000>，除以10000表示实际色坐标*/
}Effect_Source; 

typedef struct{
	
	uint16_t  lightness;		/*总亮度<0-1000>*/
	uint16_t  r;  				/*r亮度<0-1000>*/
	uint16_t  g;  				/*g亮度<0-1000>*/
	uint16_t  b;				/*b亮度<0-1000>*/
}Effect_RGB; 	

typedef struct{
	
	uint16_t  lightness;		/*总亮度<0-1000>*/
	uint16_t  r;  				/*r亮度<0-1000>*/
	uint16_t  g;  				/*g亮度<0-1000>*/
	uint16_t  b;				/*b亮度<0-1000>*/
	uint16_t  ww;  				/*ww亮度<0-1000>*/
	uint16_t  cw;				/*cw亮度<0-1000>*/
}Effect_RGBWW;

typedef struct{
	Effect_Mode mode;
	
	union{
		Effect_HSI	hsi_arg;
		Effect_CCT	cct_arg;
		Effect_Gel	gel_arg;
		Effect_Coordinate	coord_arg;
		Effect_Source	source_arg;
		Effect_RGB  rgb_arg;
	}arg;
}Effect_ColorMixing_Arg;

typedef struct{
	
	Effect_ColorMixing_Arg color1_arg;	/*颜色1参数*/
	Effect_ColorMixing_Arg color2_arg;	/*颜色2参数*/
	uint16_t ratio;		/*颜色参与比例<0-10000>， ratio/10000表示Color1参与比例，Color2参与比例为(10000-ratio)/10000%*/
}Effect_ColorMixing;

/*Mode_Arg定义*/
typedef union{
	
	Effect_HSI	hsi_arg;
	Effect_CCT	cct_arg;
	Effect_Gel	gel_arg;
	Effect_Coordinate	coord_arg;
	Effect_Source	source_arg;
	Effect_RGB		rgb_arg;
	Effect_RGBWW	rgbww_arg;
	Effect_ColorMixing mixing_arg;
	//uint8_t   default_buffer[12];   //定义默认ModeArg长度，12
}Effect_Mode_Arg;

/*0x00：ClubLights*/
typedef struct{
	
	uint16_t  lightness;  		/*亮度<0-1000>*/
	uint8_t   frq;  			/*<1-11>,11为Random*/
	uint8_t   color;  			/*<0-3>,0:3; 1:6; 2:9; 3:12*/
}ClubLights;
/*0x01：Paparazzi*/
typedef struct{
	
	uint16_t  lightness;  		/*亮度<0-1000>*/
	uint8_t   frq;  			/*<1-11>,11为Random*/
	uint16_t  cct;  			/*CCT，单位k*/
	uint8_t   gm;  				/*<0-20>: 0->-1.0,1->-0.9,2->-0.8,   10->0,20->+1.0每隔0.1递增1，以此类推*/
	Effect_Mode	mode;			/*0x00:HIS；0x01:CCT；0x02:GEL*/
	Effect_Mode_Arg  arg;  		/*Mode_Arg，具体模式参数*/
	uint16_t max_interval;
	uint16_t min_interval;
}Paparazzi;

/*0x02：Lightning*/
typedef struct{
	
	uint16_t  lightness;  		/*亮度<0-1000>*/
	uint8_t   frq;  			/*<1-11>,11为Random*/
	uint16_t  cct;  			/*CCT，单位k*/
	uint8_t   gm;  				/*<0-20>: 0->-1.0,1->-0.9,2->-0.8,   10->0,20->+1.0每隔0.1递增1，以此类推*/
	Effect_TriggerType trigger; /*<0-2>,0-无触发/1-单次触发/2-多次触发*/
	uint8_t   speed;			/*<1-11>,11为Random*/
	Effect_Mode	mode;			/*0x00:HIS；0x01:CCT；0x02:GEL*/
	Effect_Mode_Arg  arg;  		/*Mode_Arg，具体模式参数*/
	uint16_t max_interval;
	uint16_t min_interval;
}Lightning;

/*0x03：TV*/
typedef struct{
	
	uint16_t  lightness;  		/*亮度<0-1000>*/
	uint8_t   frq;  			/*<1-11>,11为Random*/
	uint16_t  cct;  			/*CCT*/
	Effect_Mode	range1_mode;
	Effect_Mode_Arg  range1_arg;
	Effect_Mode	range2_mode;
	Effect_Mode_Arg  range2_arg;
	uint16_t  max_interval;
	uint16_t  min_interval;
}TV;

/*0x04：Candle*/
typedef struct{
	
	uint16_t  lightness;  		/*亮度<0-1000>*/
	uint8_t   frq;  			/*<1-11>,11为Random*/
	uint16_t  cct;  			/*CCT，单位k*/
}Candle;

/*0x05：Fire*/
typedef struct{
	
	uint16_t  lightness;  		/*亮度<0-1000>*/
	uint8_t   frq;  			/*<1-11>,11为Random*/
	uint16_t  cct;  			/*CCT，单位k*/
	Effect_Mode	range1_mode;
	Effect_Mode_Arg  range1_arg;
	Effect_Mode	range2_mode;
	Effect_Mode_Arg  range2_arg;
	uint16_t times;             /*次数/min*/
}Fire;

/*0x06：Strobe*/
typedef struct{
	
	Effect_Mode	mode;			/*0x00:HIS；0x01:CCT；0x02:GEL*/
	uint8_t   frq;  			/*<1-11>,11为Random*/
	Effect_Mode_Arg  arg;  		/*Mode_Arg，具体模式参数*/
}Strobe;

/*0x07：Explosion*/
typedef struct{
	
	Effect_Mode	mode;			/*0x00:HIS；0x01:CCT；0x02:GEL*/
	uint8_t   frq;  			/*<1-11>,11为Random*/
	Effect_Mode_Arg arg;  		/*Mode_Arg，具体模式参数*/
	Effect_TriggerType trigger; /*<0-2>,0-无触发/1-单次触发/2-多次触发*/
}Explosion;

/*0x08：FaultBulb*/
typedef struct{
	
	Effect_Mode	mode;			/*0x00:HIS；0x01:CCT；0x02:GEL*/
	uint8_t   frq;  			/*<1-11>,11为Random*/
	Effect_Mode_Arg  arg;  		/*Mode_Arg，具体模式参数*/
	uint8_t   speed;			/*<1-11>,11为Random*/
	uint16_t min_interval;
	uint16_t max_interval;
}FaultBulb;

/*0x09：Pulsing*/
typedef struct{
	
	Effect_Mode	mode;			/*0x00:HIS；0x01:CCT；0x02:GEL*/
	uint8_t   frq;  			/*<1-11>,11为Random*/
	Effect_Mode_Arg arg;  		/*Mode_Arg，具体模式参数*/
	uint8_t   speed;			/*<1-11>,11为Random*/
	uint8_t	flash_times;
}Pulsing;

/*0x0A：Welding*/
typedef struct{
	
	Effect_Mode	mode;			/*0x00:HIS；0x01:CCT；0x02:GEL*/
	uint8_t   frq;  			/*<1-11>,11为Random*/
	Effect_Mode_Arg arg;  		/*Mode_Arg，具体模式参数*/
	uint16_t min;  				/**/
}Welding;

/*0x0B：CopCar*/
typedef struct{
	
	uint16_t  lightness;  		/*亮度<0-1000>*/
	uint8_t   frq;  			/*<1-11>,11为Random*/
	uint8_t   color;  			/*0x00：R；0x01：B；0x02：R+B；0x03：B+W*/
}CopCar;

/*0x0C：ColorChase*/
typedef struct{
	
	uint16_t  lightness;  		/*亮度<0-1000>*/
	uint8_t   frq;  			/*<1-11>,11为Random*/
	uint8_t   sat;  			/*饱和度<0-100>*/
}ColorChase;

/*0x0D：PartyLights*/
typedef struct{
	
	uint16_t  lightness;  		/*亮度<0-1000>*/
	uint8_t   frq;  			/*<1-11>,11为Random*/
	uint8_t   sat;  			/*饱和度<0-100>*/
	uint16_t  whole_time;		/*一圈的时间*/
}PartyLights;

/*0x0E：Fireworks*/
typedef struct{
	
	uint16_t  lightness;  		/*亮度<0-1000>*/
	uint8_t   frq;  			/*<1-11>,11为Random*/
	uint8_t   type;  			/*Type*/
	uint16_t  max_interval;
	uint16_t  min_interval;
}Fireworks;

/*0x10：I Am Here*/
typedef struct {

    uint16_t  run;              /*0x00:运行  0x01：停止并退出*/
} IAmHere;

typedef struct{
	uint8_t   speed;            /*<1-10>,*/
    uint8_t   direction;        /*方向，0：向左，1：向右*/
	struct{
		Effect_Mode  mode;			/*0x00:CCT；0x01:HSI；0x02:GEL*/
		Effect_Mode_Arg arg;  		/*Mode_Arg，具体模式参数*/
	}color_arg[4];
	uint8_t  channel; 			/*点控通道，1表示通道1，n表示通道n*/
}ColorFade;

/*0x12：ColorCycle*/
typedef struct{
	
	uint8_t   speed;              /*<1-10>,*/
    //uint8_t   direction;            /*方向，0：向左，1：向右*/
	struct{
		Effect_Mode  mode;			/*0x00:CCT；0x01:HSI；0x02:GEL*/
		Effect_Mode_Arg arg;  		/*Mode_Arg，具体模式参数*/
	}color_arg[4];
	uint8_t  channel; 			/*点控通道，1表示通道1，n表示通道n*/
}ColorCycle;

/*0x13：ColorGradient*/
typedef struct{
	
	uint16_t  lightness_min;          /*亮度<0-1000>*/
    uint8_t   speed;              /*<1-10>,*/
    //uint8_t   Dir;            /*方向，0：向左，1：向右*/
    struct{
		Effect_Mode  mode;			/*0x00:CCT；0x01:HSI；0x02:GEL*/
		Effect_Mode_Arg arg;  		/*Mode_Arg，具体模式参数*/
	}color_arg[4];
	uint8_t  channel; 			/*点控通道，1表示通道1，n表示通道n*/
}ColorGradient;

/*0x14：OneColorChase*/
typedef struct{

	uint8_t   speed;              /*<1-10>,*/
    uint8_t   direction;              /*方向，0：向左，1：向右*/
   struct{
		Effect_Mode  mode;			/*0x00:CCT；0x01:HSI；0x02:GEL*/
		Effect_Mode_Arg arg;  		/*Mode_Arg，具体模式参数*/
	}color_arg[2];
	uint8_t  channel; 			/*点控通道，1表示通道1，n表示通道n*/
}OneColorChase;

/*0x15：DMXStrobe*/
typedef struct{

	float   frq;              	/*浮点数，闪烁频率，单位Hz*/
	Effect_Mode	mode;			/*0x00:HIS；0x01:CCT；0x02:GEL*/
	Effect_Mode_Arg arg;  		/*Mode_Arg，具体模式参数*/
}DMXStrobe;

/*0x01：Paparazzi II*/
typedef struct{
	
	Effect_Mode	mode;			/*0x00:HIS；0x01:CCT；0x02:GEL*/
	Effect_Mode_Arg  arg;  		/*Mode_Arg，具体模式参数*/
	uint16_t max_interval;      /*ms*/
	uint16_t min_interval;      /*ms*/
}Paparazzi_II;

/*Effect_Arg定义*/
typedef union{
	
	ClubLights		club_lights_arg;
	Paparazzi		paparazzi_arg;
	Paparazzi		paparazzi2_arg;
	Lightning		lightning_arg;
	Lightning		lightning2_arg;
	Lightning		lightning3_arg;
	TV				tv_arg;
	TV				tv2_arg;
	TV				tv3_arg;
	Candle			candle_arg;
	Candle			candle2_arg;
	Fire			fire_arg;
	Fire			fire2_arg;
	Fire			fire3_arg;
	Strobe			strobe_arg;
	Strobe			strobe2_arg;
	Explosion		explosion_arg;
	Explosion		explosion2_arg;
	FaultBulb		faultBulb_arg;
	FaultBulb		faultBulb2_arg;
	FaultBulb		faultBulb3_arg;
	Pulsing			pulsing_arg;
	Pulsing			pulsing2_arg;
	Pulsing			pulsing3_arg;
	Welding			welding_arg;
	Welding			welding2_arg;
	CopCar			cop_car_arg;
    CopCar			cop_car2_arg;
	CopCar			cop_car3_arg;
	ColorChase		color_chase_arg;
	PartyLights		party_lights_arg;
	PartyLights		party_lights2_arg;
	Fireworks		fireworks_arg;
	Fireworks		fireworks2_arg;
	IAmHere			i_am_here;
	ColorFade		color_fade_arg;
	ColorCycle		color_cycle_arg;
	ColorGradient	color_gradient_arg;
	OneColorChase	one_color_chase_arg;
	DMXStrobe		dmx_strobe_arg;
}Effect_Arg;

typedef struct{
	
	Effect_Type   	effect_type;  	/*光效类型*/
	Effect_Arg		effect_arg;		/*光效参数*/
}Light_Effect; 				
#pragma pack ()

typedef struct{
	
	uint16_t r;
	uint16_t g;
	uint16_t b;
	uint16_t ww;
	uint16_t cw;
}Effect_PulseWightRGBWW;

typedef struct{
	
	Effect_Type type;
	uint8_t state;
	uint8_t restart;
	Effect_PulseWightRGBWW min_pwm;
}Effect_Typedef;

void Effect_Data_Init(const Light_Effect* effect, uint8_t restart);		//光效数据初始化
void Effect_Per1msCallback(void);

void Light_Effect_Enable(bool en);
bool Light_Effect_State(void);
uint8_t Get_EffectTypeIAmHere_Status(void);
void Clear_EffectTypeIAmHere_Status(void);

void Clear_Last_EffectType(void);
#endif /*__LIGHT_EFFECT_H*/
