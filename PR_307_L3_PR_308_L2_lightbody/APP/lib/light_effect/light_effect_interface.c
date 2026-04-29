/*********************************************************************************
  *Copyright(C), 2018, Aputure, All rights reserver.
  *FileName:  	light_effect_interface.c
  *Author:    	
  *Version:   	v1.3
  *Date:      	2019/11/29
  *Description: 光效模块接口
  *History:  
			-v1.1: 2019/08/20,Steven,初始版本
			-v1.2: 2019/08/27,Steven,添加source接口,pulsing、lightning添加speed
			-v1.3: 2019/11/29,Steven,hsi接口增加mode，实现快速慢速选择；
**********************************************************************************/

#include "light_effect_interface.h"
#include "bsp_timer.h"
#include "light_effect.h"
#include "define.h"
#include "color_mixing.h"

//#ifdef PR_308_L2
//static Light_Effect effect_struct;
//#endif

//#ifdef PR_307_L3
extern Effect_Typedef light_effect;
//#endif
/*设置定时器占空比*/
void Effect_Set_PulseWight(Effect_PulseWightRGBWW pulse_rgbww)//未调用
{
#ifdef PR_308_L2
	//TODO
	uint16_t led_data[5] = {0};

	led_data[0] = pulse_rgbww.g;
	led_data[1] = pulse_rgbww.r;
	led_data[2] = pulse_rgbww.cw;
	led_data[3] = pulse_rgbww.ww;
	led_data[4] = pulse_rgbww.b;
	Set_Drive_Cob_Pwm_Val(led_data);
#endif
}

/*用于分屏或点控, 设置像素设置定时器占空比，pixel为0xff时全控*/
void Effect_SetPixel_PulseWight(Effect_PulseWightRGBWW pulse_rgbww, uint16_t pixel)
{
	uint16_t led_data[5] = {0};
#ifdef PR_307_L3
	
	led_data[0] = pulse_rgbww.r/* * g_power_factor.frq_change*/;
	led_data[1] = pulse_rgbww.g/* * g_power_factor.frq_change*/;
	led_data[2] = pulse_rgbww.b/* * g_power_factor.frq_change*/;
	led_data[3] = pulse_rgbww.ww/* * g_power_factor.frq_change*/;
	led_data[4] = pulse_rgbww.cw/* * g_power_factor.frq_change*/;
    
    led_data[0] = led_data[0] <=  light_effect.min_pwm.r ? 0 : led_data[0];
	led_data[1] = led_data[1] <=  light_effect.min_pwm.g ? 0 : led_data[1];
	led_data[2] = led_data[2] <=  light_effect.min_pwm.b ? 0 : led_data[2];
	led_data[3] = led_data[3] <= light_effect.min_pwm.ww ? 0 : led_data[3];
	led_data[4] = led_data[4] <= light_effect.min_pwm.cw ? 0 : led_data[4];

	if (pixel == 0xff)
	{
		pixel = 10;
	}
//	RGBWW_SetPwm(SEL_COLOR_ALL, rgbww_pwm_struct, 1);
	set_self_adapt_pwm(led_data);
    Set_Drive_Cob_Pwm_Val(led_data);
#endif

#ifdef PR_308_L2

	led_data[0] = pulse_rgbww.g;
	led_data[1] = pulse_rgbww.r;
	led_data[2] = pulse_rgbww.cw;
	led_data[3] = pulse_rgbww.ww;
	led_data[4] = pulse_rgbww.b;
	set_self_adapt_pwm(led_data);
	Set_Drive_Cob_Pwm_Val(led_data);
#endif
}
/*用于分屏或点控,非直接控制时，调用使pwm生效，目前用于179*/
void Effect_ActivePixel_PulseWight(void)
{
}


/*cct实现函数*/
void Effect_CCT2PulseWight(uint16_t cct, uint8_t gm, uint16_t lightness, Effect_PulseWightRGBWW *pulse_rgbww)
{

	//TODO
	struct mixing_pwm RGBWW_Arrary;
	struct mixing_lux RGBWW_Lux_Arrary;
#ifdef PR_308_L2	
	color_cct_calc((float)lightness / 1000.0f, cct, gm_to_duv(gm), (enum ilumination_mode)g_rs485_data.illumination_mode.mode, (enum mixing_curve)g_rs485_data.dimming_curve.curve, g_power_factor.sum_power_rate,\
        &RGBWW_Arrary ,&RGBWW_Lux_Arrary);
	if( lightness == 0 )
	{
		RGBWW_Arrary.pwm[0] = 0;
		RGBWW_Arrary.pwm[1] = 0;
		RGBWW_Arrary.pwm[2] = 0;
		RGBWW_Arrary.pwm[3] = 0;
		RGBWW_Arrary.pwm[4] = 0;
	}
#endif
#ifdef PR_307_L3
	color_cct_calc((float)lightness / 1000.0f, cct, gm_to_duv(gm), (enum ilumination_mode)g_rs485_data.illumination_mode.mode, (enum mixing_curve)g_rs485_data.dimming_curve.curve, g_power_factor.sum_power_rate,\
        &RGBWW_Arrary,&RGBWW_Lux_Arrary);
#endif
	pulse_rgbww->r = RGBWW_Arrary.pwm[0];
	pulse_rgbww->g = RGBWW_Arrary.pwm[1];
	pulse_rgbww->b = RGBWW_Arrary.pwm[2];
	pulse_rgbww->ww =RGBWW_Arrary.pwm[3];
	pulse_rgbww->cw =RGBWW_Arrary.pwm[4];
}

void Effect_HSI2PulseWight(uint16_t hue, uint8_t sat, uint16_t lightness, Effect_PulseWightRGBWW *pulse_rgbww, uint16_t cct)
{
	#ifdef PR_307_L3
	//TODO
	//color_hsi_calc(hue, sat, lightness, (Mix_Pwm *)pulse_rgbww, cct, (MIX_CurveTypeDef)g_rs485_data.dimming_curve.curve);
	struct mixing_pwm RGBWW_Arrary;	
	struct mixing_lux RGBWW_Lux_Arrary;
	color_hsi_calc((float)lightness / 1000.0f, hue, sat, cct, 0.0f,(enum mixing_curve)g_rs485_data.dimming_curve.curve, g_power_factor.sum_power_rate, &RGBWW_Arrary, &RGBWW_Lux_Arrary);
	
	pulse_rgbww->r = RGBWW_Arrary.pwm[0];
	pulse_rgbww->g = RGBWW_Arrary.pwm[1];
	pulse_rgbww->b = RGBWW_Arrary.pwm[2];
	pulse_rgbww->ww =RGBWW_Arrary.pwm[3];
	pulse_rgbww->cw =RGBWW_Arrary.pwm[4];
	#endif
}

//#ifdef PR_308_L2
///*hsi实现函数，单色温和双色温模式不需实现*/
//void Effect_HSI2PulseWight(uint16_t hue, uint8_t sat, uint16_t lightness, Effect_PulseWightRGBWW *pulse_rgbww, uint8_t mode)
//{
//	//TODO


//}
//#endif
/*gel接口，全功能彩色需实现*/
void Effect_Gel2PulseWight(uint8_t brand, uint8_t type, uint16_t cct, uint16_t color, uint16_t lightness, Effect_PulseWightRGBWW *pulse_rgbww)
{
	//TODO
#ifdef PR_307_L3
	struct mixing_pwm RGBWW_Arrary;	
	struct mixing_lux RGBWW_Lux_Arrary;
	color_gel_calc((float)lightness / 1000.0f, brand, type, cct, color, \
                            (enum mixing_curve)g_rs485_data.dimming_curve.curve, g_power_factor.sum_power_rate, &RGBWW_Arrary, &RGBWW_Lux_Arrary);
	
	pulse_rgbww->r = RGBWW_Arrary.pwm[0];
	pulse_rgbww->g = RGBWW_Arrary.pwm[1];
	pulse_rgbww->b = RGBWW_Arrary.pwm[2];
	pulse_rgbww->ww =RGBWW_Arrary.pwm[3];
	pulse_rgbww->cw =RGBWW_Arrary.pwm[4];
#endif
}

/*任意色坐标接口，全功能彩色需实现*/
void Effect_Coord2PulseWight(uint16_t coord_x, uint16_t coord_y, uint16_t lightness, Effect_PulseWightRGBWW *pulse_rgbww)
{
	//TODO
#ifdef PR_307_L3
	struct mixing_pwm RGBWW_Arrary;	
	struct mixing_lux RGBWW_Lux_Arrary;
	color_coordinate_calc((float)lightness / 1000.0f, coord_x*0.0001, coord_y*0.0001, (enum mixing_curve)g_rs485_data.dimming_curve.curve, g_power_factor.sum_power_rate, &RGBWW_Arrary, &RGBWW_Lux_Arrary);
	pulse_rgbww->r = RGBWW_Arrary.pwm[0];
	pulse_rgbww->g = RGBWW_Arrary.pwm[1];
	pulse_rgbww->b = RGBWW_Arrary.pwm[2];
	pulse_rgbww->ww =RGBWW_Arrary.pwm[3];
	pulse_rgbww->cw =RGBWW_Arrary.pwm[4];
#endif
}

/*source光源接口，全功能彩色需实现*/
void Effect_Source2PulseWight(uint8_t type, uint16_t coord_x, uint16_t coord_y, uint16_t lightness, uint8_t mode, Effect_PulseWightRGBWW *pulse_rgbww)
{
#ifdef PR_307_L3
	//TODO
	//color_source_calc(type, coord_x*0.0001, coord_y*0.0001, lightness, (Mix_Pwm *)pulse_rgbww, (MIX_CurveTypeDef)g_rs485_data.dimming_curve.curve, mode);
   struct mixing_pwm RGBWW_Arrary;	
	struct mixing_lux RGBWW_Lux_Arrary;
   color_source_calc((float)lightness / 1000.0f, type, coord_x*0.0001, coord_y*0.0001, mode, 
                           (enum mixing_curve)g_rs485_data.dimming_curve.curve, g_power_factor.sum_power_rate, &RGBWW_Arrary, &RGBWW_Lux_Arrary);

  	pulse_rgbww->r = RGBWW_Arrary.pwm[0];
	pulse_rgbww->g = RGBWW_Arrary.pwm[1];
	pulse_rgbww->b = RGBWW_Arrary.pwm[2];
	pulse_rgbww->ww =RGBWW_Arrary.pwm[3];
	pulse_rgbww->cw =RGBWW_Arrary.pwm[4];
#endif
}

//#ifdef PR_308_L2
///*source光源接口，全功能彩色需实现*/
//void Effect_Source2PulseWight(uint8_t type, uint16_t lightness, Effect_PulseWightRGBWW *pulse_rgbww)
//{
//	//TODO



//}
//#endif

/*RGB接口，全功能彩色需实现*/
void Effect_RGB2PulseWight(uint16_t lightness, uint16_t r, uint16_t g, uint16_t b, Effect_PulseWightRGBWW *pulse_rgbww)
{
#ifdef PR_307_L3
	//TODO
//	uint16_t rgbww_struct[5] = {r, g, b, 0, 0};
//	color_rgb_calc(lightness, &rgbww_struct, pulse_rgbww, (MIX_CurveTypeDef)g_rs485_data.dimming_curve.curve);
	
	struct mixing_pwm RGBWW_Arrary;	
	struct mixing_lux RGBWW_Lux_Arrary;
	color_rgb_calc((float)lightness / 1000.0f, (float)r / 1000.0f, (float)g / 1000.0f, (float)b / 1000.0f,(enum mixing_curve)g_rs485_data.dimming_curve.curve, g_power_factor.sum_power_rate, &RGBWW_Arrary, &RGBWW_Lux_Arrary);
	pulse_rgbww->r = RGBWW_Arrary.pwm[0];
	pulse_rgbww->g = RGBWW_Arrary.pwm[1];
	pulse_rgbww->b = RGBWW_Arrary.pwm[2];
	pulse_rgbww->ww =RGBWW_Arrary.pwm[3];
	pulse_rgbww->cw =RGBWW_Arrary.pwm[4];
#endif
}

/*RGBWW接口，全功能彩色需实现*/
void Effect_RGBWW2PulseWight(uint16_t lightness, uint16_t r, uint16_t g, uint16_t b, uint16_t ww, uint16_t cw,Effect_PulseWightRGBWW *pulse_rgbww)
{
#ifdef PR_307_L3
	//TODO
//	uint16_t rgbww_struct[5] = {r, g, b, 0, 0};
//	color_rgb_calc(lightness, &rgbww_struct, pulse_rgbww, (MIX_CurveTypeDef)g_rs485_data.dimming_curve.curve);
	
	struct mixing_pwm RGBWW_Arrary;	
	struct mixing_lux RGBWW_Lux_Arrary;
	color_rgbww_calc((float)r / 1000.0f, (float)g / 1000.0f, (float)b / 1000.0f, (float)ww / 1000.0f, (float)cw / 1000.0f, (enum mixing_curve)g_rs485_data.dimming_curve.curve, g_power_factor.sum_power_rate, &RGBWW_Arrary, &RGBWW_Lux_Arrary);
	pulse_rgbww->r = RGBWW_Arrary.pwm[0];
	pulse_rgbww->g = RGBWW_Arrary.pwm[1];
	pulse_rgbww->b = RGBWW_Arrary.pwm[2];
	pulse_rgbww->ww =RGBWW_Arrary.pwm[3];
	pulse_rgbww->cw =RGBWW_Arrary.pwm[4];
#endif
}

/*ColorMixing接口，全功能彩色需实现*/
void Effect_ColorMixing2PulseWight(void *color, Effect_PulseWightRGBWW *pulse_rgbww)//Phoebe:修改数据
{
#ifdef PR_307_L3
	//TODO
	Effect_ColorMixing* effect_color = (Effect_ColorMixing*)color;
	struct color_mixing color_mix;
	
	color_mix.ratio = (float)effect_color->ratio / 10000.0f;
	switch (effect_color->color1_arg.mode)
	{
		case EffectModeCCT:
			color_mix.color1_arg.mode = Mix_ColorMixingMode_CCT;
			color_mix.color1_arg.arg.cct.lightness = (float)effect_color->color1_arg.arg.cct_arg.lightness / 1000.0f;
			color_mix.color1_arg.arg.cct.cct = effect_color->color1_arg.arg.cct_arg.cct;
			color_mix.color1_arg.arg.cct.duv = gm_to_duv(effect_color->color1_arg.arg.cct_arg.gm);
			break;
		case EffectModeHSI:
			color_mix.color1_arg.mode = Mix_ColorMixingMode_HSI;
			color_mix.color1_arg.arg.hsi.lightness = (float)effect_color->color1_arg.arg.hsi_arg.lightness / 1000.0f;
			color_mix.color1_arg.arg.hsi.hue = (float)effect_color->color1_arg.arg.hsi_arg.hue;
			color_mix.color1_arg.arg.hsi.sat = (float)effect_color->color1_arg.arg.hsi_arg.sat;
			color_mix.color1_arg.arg.hsi.cct = effect_color->color1_arg.arg.hsi_arg.cct;
			break;
		case EffectModeRGB:
			color_mix.color1_arg.mode = Mix_ColorMixingMode_RGB;
			color_mix.color1_arg.arg.rgb.lightness = (float)effect_color->color1_arg.arg.rgb_arg.lightness / 1000.0f;
			color_mix.color1_arg.arg.rgb.r = (float)effect_color->color1_arg.arg.rgb_arg.r / 1000.0f;
			color_mix.color1_arg.arg.rgb.g = (float)effect_color->color1_arg.arg.rgb_arg.g / 1000.0f;
			color_mix.color1_arg.arg.rgb.b = (float)effect_color->color1_arg.arg.rgb_arg.b / 1000.0f;
			break;
		default:
			color_mix.color1_arg.mode = Mix_ColorMixingMode_NULL;
			break;
	}
	switch (effect_color->color2_arg.mode)
	{
		case EffectModeCCT:
			color_mix.color2_arg.mode = Mix_ColorMixingMode_CCT;
			color_mix.color2_arg.arg.cct.lightness = (float)effect_color->color2_arg.arg.cct_arg.lightness / 1000.0f;
			color_mix.color2_arg.arg.cct.cct = effect_color->color2_arg.arg.cct_arg.cct;
			color_mix.color2_arg.arg.cct.duv = gm_to_duv(effect_color->color2_arg.arg.cct_arg.gm);
			break;
		case EffectModeHSI:
			color_mix.color2_arg.mode = Mix_ColorMixingMode_HSI;
			color_mix.color2_arg.arg.hsi.lightness = (float)effect_color->color2_arg.arg.hsi_arg.lightness / 1000.0f;
			color_mix.color2_arg.arg.hsi.hue = (float)effect_color->color2_arg.arg.hsi_arg.hue;
			color_mix.color2_arg.arg.hsi.sat = (float)effect_color->color2_arg.arg.hsi_arg.sat;
			color_mix.color2_arg.arg.hsi.cct = effect_color->color2_arg.arg.hsi_arg.cct;
			break;
		case EffectModeRGB:
			color_mix.color2_arg.mode = Mix_ColorMixingMode_RGB;
			color_mix.color2_arg.arg.rgb.lightness = (float)effect_color->color2_arg.arg.rgb_arg.lightness / 1000.0f;
			color_mix.color2_arg.arg.rgb.r = (float)effect_color->color2_arg.arg.rgb_arg.r / 1000.0f;
			color_mix.color2_arg.arg.rgb.g = (float)effect_color->color2_arg.arg.rgb_arg.g / 1000.0f;
			color_mix.color2_arg.arg.rgb.b = (float)effect_color->color2_arg.arg.rgb_arg.b / 1000.0f;
			break;
		default:
			color_mix.color2_arg.mode = Mix_ColorMixingMode_NULL;
			break;
	}
//	Color_ColorMixing_To_Pwm(color_mix, (Mix_Pwm *)pulse_rgbww, ILL_MAX, (MIX_CurveTypeDef)g_rs485_data.dimming_curve.curve);  
//	//(enum mixing_curve)g_rs485_data.dimming_curve.curve, g_power_factor.sum_power_rate, &RGBWW_Arrary
	struct mixing_pwm RGBWW_Arrary;	
	struct mixing_lux RGBWW_Lux_Arrary;
	color_mixing_calc(&color_mix, (enum mixing_curve)g_rs485_data.dimming_curve.curve, g_power_factor.sum_power_rate, &RGBWW_Arrary, &RGBWW_Lux_Arrary);
	pulse_rgbww->r = RGBWW_Arrary.pwm[0];
	pulse_rgbww->g = RGBWW_Arrary.pwm[1];
	pulse_rgbww->b = RGBWW_Arrary.pwm[2];
	pulse_rgbww->ww =RGBWW_Arrary.pwm[3];
	pulse_rgbww->cw =RGBWW_Arrary.pwm[4];
#endif
}


//uint16_t RNG_Get_RandomNum(void)
//{	
//	static 	uint16_t random;
//	//random = random << 8 | ADC2_Value_Get();
//	random = random << 8 | SysTick_GetTick();
//	srand(random);
//	random = rand();
//	
//	return random;
//}

///*获取min-max区间的随机数*/
//int16_t RNG_Get_RandomRange(int16_t min, int16_t max)
//{ 
//  // return RNG_Get_RandomNum()%(max-min+1) + min;
//	return RNG_Get_RandomNum()%(max-min+1) + min;
//}

///*获取随机数接口*/
//int16_t Effect_Get_RandomRange(int16_t min,int16_t max)
//{
//	//TODO
//	return RNG_Get_RandomRange(min, max);
//}

uint16_t RNG_Get_RandomNum(void)
{	
	static 	uint16_t random;
	//random = random << 8 | ADC2_Value_Get();
	random = random << 8 | SysTick_GetTick();
	srand(random);
	random = rand();
	
	return random;
}

/*获取min-max区间的随机数*/
uint32_t RNG_Get_RandomRange(int16_t min, int16_t max)
{ 
  // return RNG_Get_RandomNum()%(max-min+1) + min;
	return RNG_Get_RandomNum()%(max-min+1) + min;
}

/*获取随机数接口*/
int16_t Effect_Get_RandomRange_Int16(int16_t min,int16_t max)
{
	//TODO
	return RNG_Get_RandomRange(min, max);
}
uint32_t Effect_Get_RandomRange_Uint32(uint32_t min, uint32_t max)
{
	//TODO
	return RNG_Get_RandomRange(min, max);
}

void Light_Effect_Init(void)
{
#ifdef PR_307_L3  //处理红光拖尾
	uint8_t i = 0;
	uint16_t j = 0;
	uint16_t pwm[5] = {0};	
	for(i = 0; i < 5; i++)
	{
		for(j = 0; j < led_base_data.data[i]->length; j++)
		{
			if(led_base_data.data[i]->data[j].Illuminance != 0)
			{
				pwm[i] = led_base_data.data[i]->data[j].pwm;
				break;
			}
		}
	}
	//	light_effect.min_pwm.r = pwm[0];
	light_effect.min_pwm.g = pwm[1];
	light_effect.min_pwm.b = pwm[2];
	light_effect.min_pwm.ww = pwm[3];
	light_effect.min_pwm.cw = pwm[4];
#endif
	Light_Effect_Enable(false);
}
