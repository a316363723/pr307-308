/*********************************************************************************
  *Copyright(C), 2018, Aputure, All rights reserver.
  *FileName:  	light_effect_interface.h
  *Author:    	
  *Version:   	v1.2
  *Date:      	2019/08/27
  *Description: 光效模块接口
  *History:  
			-v1.1: 2019/08/20,Steven,初始版本
			-v1.2: 2019/08/27,Steven,添加source接口,pulsing、lightning添加speed
**********************************************************************************/
#ifndef __EFFECT_INF_H
#define __EFFECT_INF_H

#include "light_effect.h"

#ifdef  PR_307_L3 
#define PRODUCT_FUNCTION		3	/*0:单色温；1：双色温；2：简单彩色；3：全功能彩色*/

/*tv光效cct变化区间*/
#define TV_CCT_RANGE1_LOW		2800
#define TV_CCT_RANGE1_UP		4700
#define TV_CCT_RANGE2_LOW		4700
#define TV_CCT_RANGE2_UP		6500
#define TV_CCT_RANGE3_LOW		6500
#define TV_CCT_RANGE3_UP		10000

/*candle光效cct变化区间*/
#define CANDLE_CCT_RANGE1_LOW	1700
#define CANDLE_CCT_RANGE1_UP	1800
#define CANDLE_CCT_RANGE2_LOW	1900
#define CANDLE_CCT_RANGE2_UP	2000
#define CANDLE_CCT_RANGE3_LOW	2100
#define CANDLE_CCT_RANGE3_UP	2200

/*fire光效cct变化区间*/
#define FIRE_CCT_RANGE1_LOW		1900
#define FIRE_CCT_RANGE1_UP		2000
#define FIRE_CCT_RANGE2_LOW		2100
#define FIRE_CCT_RANGE2_UP		2200
#define FIRE_CCT_RANGE3_LOW		2500
#define FIRE_CCT_RANGE3_UP		2600

#define EFFECT_CCT_MAX			10000  	/*10000K*/
#define EFFECT_CCT_MIN			2000	//2800	/*3200K*/
#endif 

#ifdef  PR_308_L2
#define PRODUCT_FUNCTION		1	/*0:单色温；1：双色温；2：简单彩色；3：全功能彩色*/

/*tv光效cct变化区间*/
#define TV_CCT_RANGE1_LOW		2700
#define TV_CCT_RANGE1_UP		3900
#define TV_CCT_RANGE2_LOW		4000
#define TV_CCT_RANGE2_UP		5200
#define TV_CCT_RANGE3_LOW		5300
#define TV_CCT_RANGE3_UP		6500

/*candle光效cct变化区间*/
#define CANDLE_CCT_RANGE1_LOW	2700
#define CANDLE_CCT_RANGE1_UP	2800
#define CANDLE_CCT_RANGE2_LOW	2800
#define CANDLE_CCT_RANGE2_UP	2900
#define CANDLE_CCT_RANGE3_LOW	2900
#define CANDLE_CCT_RANGE3_UP	3000

/*fire光效cct变化区间*/
#define FIRE_CCT_RANGE1_LOW		2700
#define FIRE_CCT_RANGE1_UP		3000
#define FIRE_CCT_RANGE2_LOW		4000
#define FIRE_CCT_RANGE2_UP		5200
#define FIRE_CCT_RANGE3_LOW		5300
#define FIRE_CCT_RANGE3_UP		6500

#define EFFECT_CCT_MAX			6500  	/*10000K*/
#define EFFECT_CCT_MIN			2700	//2800	/*3200K*/
#endif

#define ClubLights_USE			1
#define Paparazzi_USE			1
#define Lightning_USE			1
#define TV_USE					1
#define Candle_USE				1
#define Fire_USE				1
#define Strobe_USE				1
#define Explosion_USE			1
#define FaultBulb_USE			1
#define Pulsing_USE				1
#define Welding_USE				1
#define CopCar_USE				1
#define ColorChase_USE			1
#define PartyLights_USE			1
#define Fireworks_USE			1
#define IamHere_USE				1

#define COPCAR_PIXEL_NUM        1  /*警灯光效像素数量，当不为1时分区闪*/

#if (PRODUCT_FUNCTION == 0)
	#if ClubLights_USE == 1
		#error "ClubLights_USE must be defined as 0 !"
	#endif
	#if Candle_USE == 1
		#error "Candle_USE must be defined as 0 !"
	#endif
	#if Fire_USE == 1
		#error "Fire_USE must be defined as 0 !"
	#endif
	#if CopCar_USE == 1
		#error "CopCar_USE must be defined as 0 !"
	#endif
	#if ColorChase_USE == 1
		#error "ColorChase_USE must be defined as 0 !"
	#endif
	#if PartyLights_USE == 1
		#error "PartyLights_USE must be defined as 0 !"
	#endif
#endif

#
#define EFFECT_FADE_TIME 		100 	//100ms


#if (PRODUCT_FUNCTION == 0) 	//单色温
	#define PWM_ARR_NUM			1
#elif (PRODUCT_FUNCTION == 1) 	//双色温
	#define PWM_ARR_NUM			((EFFECT_CCT_MAX - EFFECT_CCT_MIN)/100 + 1)
#else //PRODUCT_FUNCTION == 2 || PRODUCT_FUNCTION == 3,简单彩色和全功能彩色
	#define PWM_ARR_NUM			(361 + (EFFECT_CCT_MAX - EFFECT_CCT_MIN)/100 + 1)
#endif

#define EFFECT_PIXEL_NUM 4	/*只用于分板控制*/

//#ifdef PR_307_L3
void Effect_Set_PulseWight(Effect_PulseWightRGBWW pulse_rgbww);
void Effect_CCT2PulseWight(uint16_t cct, uint8_t gm, uint16_t lightness, Effect_PulseWightRGBWW *pulse_rgbww);
void Effect_HSI2PulseWight(uint16_t hue, uint8_t sat, uint16_t lightness, Effect_PulseWightRGBWW *pulse_rgbww, uint16_t cct);
void Effect_Gel2PulseWight(uint8_t brand, uint8_t type, uint16_t cct, uint16_t color, 
								uint16_t lightness, Effect_PulseWightRGBWW *pulse_rgbww);
void Effect_Coord2PulseWight(uint16_t coord_x, uint16_t coord_y, uint16_t lightness, Effect_PulseWightRGBWW *pulse_rgbww);
int16_t Effect_Get_RandomRange_Int16(int16_t min,int16_t max);
uint32_t Effect_Get_RandomRange_Uint32(uint32_t min, uint32_t max);
void Effect_Source2PulseWight(uint8_t type, uint16_t coord_x, uint16_t coord_y, uint16_t lightness, uint8_t mode, Effect_PulseWightRGBWW *pulse_rgbww);
void Effect_RGB2PulseWight(uint16_t lightness, uint16_t r, uint16_t g, uint16_t b, Effect_PulseWightRGBWW *pulse_rgbww);
void Effect_RGBWW2PulseWight(uint16_t lightness, uint16_t r, uint16_t g, uint16_t b, uint16_t ww, uint16_t cw,Effect_PulseWightRGBWW *pulse_rgbww);
void Effect_ColorMixing2PulseWight(void *color, Effect_PulseWightRGBWW *pulse_rgbww);

void Effect_SetPixel_PulseWight(Effect_PulseWightRGBWW pulse_rgbww, uint16_t pixel);
void Effect_ActivePixel_PulseWight(void);

void Light_Effect_Init(void);
extern void Set_Drive_Cob_Pwm_Val(uint16_t* p_pwm);
extern void set_self_adapt_pwm(uint16_t* p_pwm);
#endif /*__EFFECT_INF_H*/
