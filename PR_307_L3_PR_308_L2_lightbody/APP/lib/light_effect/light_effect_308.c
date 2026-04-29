/*********************************************************************************
  *Copyright(C), 2018, Aputure, All rights reserver.
  *FileName:  	light_effect.c
  *Author:    	
  *Version:   	v1.7
  *Date:      	2021/05/19
  *Description: 光效模块
  *History:  
			-v1.1: 2019/08/20,Steven,初始版本
			-v1.2: 2019/08/27,Steven,添加source接口,pulsing、lightning添加speed
			-v1.3: 2019/10/26,Steven,添加source接口,faulty添加speed,修改缓变函数，
			修改Party和ColorChase光效，修改参数后立即生效而不是等待下一周期
			修改lightning，循环模式切换和频率修改立即生效
			-v1.4: 2019/11/25,Steven,EffectTypeClubLights、EffectTypePaparazzi、EffectTypeTV
			EffectTypeCandle、EffectTypeFire、EffectTypeExplosion、EffectTypeWelding、
			EffectTypeColorChase、EffectTypePartyLights  frq添加随机档
			-v1.5: 2019/11/29,Steven,hsi接口增加mode，实现快速慢速选择
			-v1.6: 2019/12/02,Steven,别的状态进入off光效时关灯，别的状态进入lightning和Explosion触发为空时关灯
			-v1.7: 2021/05/19,Steven,定位光效循环次数由2次改为1次，增加停止寻灯功能
**********************************************************************************/
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include "light_effect.h"
#include "light_effect_interface.h"

typedef enum {

	CopCar_Color_R = 0,
	CopCar_Color_B,
	CopCar_Color_RB,
	CopCar_Color_BW,
	CopCar_Color_RBW,
}CopCar_Type;

typedef __packed struct{

	uint8_t flash_num;
	float work_tick;
	Effect_PulseWightRGBWW *pwm;
	uint8_t area;
}CopCar_Item;

typedef __packed struct{

	uint8_t area_num;  //
	CopCar_Item item[10];
}CopCar_TypeDef;

static Effect_PulseWightRGBWW rgbww_pwm_arr[2][PWM_ARR_NUM] = {0};
static Effect_PulseWightRGBWW* p_rgbww_pwm = &rgbww_pwm_arr[0][0];

static Light_Effect effect_data[2] = {
	{EffectTypeNULL, 0},
	{EffectTypeNULL, 0}
};
static Light_Effect* p_effect_data = &effect_data[0];

uint8_t deal_data_update = 0;
Effect_TriggerType trigger_type = Trigger_None;  //光效触发标志
static Effect_TriggerType trigger_type_last = Trigger_None;  //光效触发标志
static bool light_effect_enable = false;

#if (ClubLights_USE == 1)
static const uint16_t club_interval_max[11] = {1200, 1200, 700, 500, 350, 250, 150, 130, 100, 90, 80}; /*ClubLights间隔时间*/
static const uint16_t club_interval_min[11] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}; /*ClubLights间隔时间*/
static const uint16_t club_wt[11] = {3800, 3800, 2250, 1800, 1500, 1200, 950, 750, 650, 550, 450}; /*ClubLights工作时间*/
static const uint16_t club_color_arr[8] = {3, 6, 9, 12, 15, 18, 24, 36}; /*ClubLights颜色种类*/
static const uint8_t sort_array[36] = {
	0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 
	21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35
}; /*用于随机排列*/
#endif /*ClubLights_USE*/


#if (Paparazzi_USE == 1)
static const uint16_t paparazzi_interval_max[11] = {11400, 11400, 8210, 5020, 1790, 1610, 1430, 1250, 1070, 890, 890}; /*Paparazzi间隔时间*/
static const uint16_t paparazzi_interval_min[11] = {1380, 1380, 920, 590, 200, 180, 150, 140, 120, 90, 90}; /*Paparazzi间隔时间*/
#endif /*Paparazzi_USE*/

#if (Lightning_USE == 1)
/*Lightning间隔时间（灭-亮）*/
static const uint16_t lightning_interval[11][11] = {
	
/*Speed: 0    1      2      3      4      5      6      7      8      9      10 */  /*Frq*/
	{29310, 29160, 29010, 28860, 28710, 28560, 28410, 28260, 28110, 27960, 27810},  /*0*/
	{25030, 24880, 24730, 24580, 24430, 24280, 24130, 23980, 23830, 23680, 23530},	/*1*/
	{20740, 20590, 20440, 20290, 20140, 19990, 19840, 19690, 19540, 19390, 19240},	/*2*/
	{16450, 16300, 16150, 16000, 15850, 15700, 15550, 15400, 15250, 15100, 14950},	/*3*/
	{12170, 12020, 11870, 11720, 11570, 11420, 11270, 11120, 10970, 10820, 10670},	/*4*/
	{7880,  7730,  7580,  7430,  7280,  7130,  6980,  6830,  6680,  6530,  6380},	/*5*/
	{7023,  6873,  6723,  6573,  6423,  6273,  6123,  5973,  5823,  5673,  5523},	/*6*/
	{6168,  6018,  5868,  5718,  5568,  5418,  5268,  5118,  4968,  4818,  4668},	/*7*/
	{5300,  5150,  5000,  4850,  4700,  4550,  4400,  4250,  4100,  3950,  3800},	/*8*/
	{4440,  4290,  4140,  3990,  3840,  3690,  3540,  3390,  3240,  3090,  2940},	/*9*/
	{3900,  3750,  3600,  3450,  3300,  3150,  3000,  2850,  2700,  2550,  2400}	/*10*/
};
#endif /*Lightning_USE*/

#if (TV_USE == 1)
//工作停留时间
static const uint16_t tv_wt_max[11] = {12800, 12800, 10800, 8400, 6900, 4800, 4300, 3700, 3300, 2700, 2200};    
static const uint16_t tv_wt_min[11] = {8200,  8200,  6700,  5500, 4200, 3100, 2800, 2500, 2300, 2000, 1500}; 
#endif /*(TV_USE == 1)*/



#if (Candle_USE == 1)
/*candle变化速率*/
int16_t candle_rate_max[11] = { 10, 20,  40,  60,  80,  100,  120,  140,  160,  180,  200};
int16_t candle_rate_min[11] = {-10,-20, -40, -60, -80, -100, -120, -140, -160, -180, -200};
/*candle变化时间*/
uint16_t candle_ct_max[11] = {600, 500, 400, 400, 400, 350, 300, 250, 200, 150, 100};
uint16_t candle_ct_min[11] = {220, 200, 180, 160, 140, 120, 100, 80,  60,  40,  20};
#endif /*(Candle_USE == 1)*/

#if (Fire_USE == 1)
/*Fire变化速率*/
int16_t fire_rate_max[11] = { 10, 20,  40,  60,  80,  100,  120,  140,  160,  180,  200};
int16_t fire_rate_min[11] = {-10,-20, -40, -60, -80, -100, -120, -140, -160, -180, -200};
/*Fire变化时间*/
uint16_t fire_ct_max[11] = {600, 500, 400, 400, 400, 350, 300, 250, 200, 150, 100};
uint16_t fire_ct_min[11] = {220, 200, 180, 160, 140, 120, 100, 80,  60,  40,  20};
#endif /*(Fire_USE == 1)*/

#if (Strobe_USE == 1)
static const uint16_t strobe_interval_max[11] = {500, 270, 152, 106, 76, 60, 46, 32, 30, 16, 10};
#endif /*(Strobe_USE == 1)*/

#if (Explosion_USE == 1)
/*Explosion工作时间*/
static const uint16_t explosion_wt[11] = {3982, 3628, 3282, 2934, 2593, 2178, 1783, 1510, 1190, 810, 488};
#endif /*(Explosion_USE == 1)*/

#if (FaultBulb_USE == 1)
/*Fault_Bulb工作时间*/
static const uint16_t fault_work_max[12] = {7500, 7500, 6833, 6166, 5500, 4833, 4166, 3500, 2833, 2166, 1500, 7500};
static const uint16_t fault_work_min[12] = {150, 150, 150, 150, 150, 150, 150, 150, 150, 150, 150, 150};
static const uint16_t fault_rt_max[12] = {500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 7500};
static const uint16_t fault_rt_min[12] = {50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50};
static const uint16_t fault_pr[12] = {990, 990, 990, 990, 990, 990, 990, 990, 990, 990, 990, 990};
static const uint16_t fault_r[12] = {700, 700, 700, 700, 700, 700, 700, 700, 700, 700, 700, 700};
#if (LIGHTNING_SPEED_USE == 1)
static const uint16_t fault_interval_max[11] = {250, 240, 230, 220, 210, 200, 190, 180, 170, 160, 150};
#endif
#endif /*(FaultBulb_USE == 1)*/

#if (Pulsing_USE == 1)
/*Pluse间隔时间*/
// static const uint16_t pluse_interval_max[11] = {8104, 6850, 5252, 3662, 2060, 436, 450, 450, 450, 450, 450};
/*Pluse工作时间*/
// static const uint16_t pluse_wt[11] = {3900, 3030, 2492, 1952, 1422, 902, 750, 628, 486, 360, 253};

/*Pluse间隔时间*/
static const uint16_t pluse_interval[11][11] = {
/*Speed:0    1      2      3      4      5      6     7     8     9    10 *//*Frq*/
	{8452,  8987,  9524, 10060, 10580, 11120,  137,  135,  120,  100,  75}, /*0*/
	{8987,  6856,  4715, 2584,  452,   156,    135,  127,  120,  100,  75},	/*1*/
	{9520,  7384,  5255, 3123,  992,   156,    135,  127,  120,  100,  75},	/*2*/
	{10050, 7923,  5798, 3662,  1530,  156,    135,  127,  120,  100,  75},	/*3*/
	{10580, 8452,  6322, 4190,  2060,  156,    135,  127,  120,  100,  75},	/*4*/
	{11120, 8990,  6858, 4727,  2597,  450,    316,  197,  120,  100,  75},	/*5*/
	{11250, 9125,  6992, 4862,  2731,  585,    450,  330,  195,  100,  75},	/*6*/
	{11380, 9245,  7112, 4983,  2853,  707,    572,  450,  317,  182,  75},	/*7*/
	{11520, 9393,  7253, 5131,  2991,  855,    720,  600,  465,  330,  196},/*8*/
	{11640, 9515,  7380, 5250,  3118,  972,    841,  720,  582,  450,  315},/*9*/
	{11770, 9650,  7507, 5378,  3245,  1110,   976,  850,  720,  585,  450}	/*10*/
};
/*Pluse工作时间*/
static const uint16_t pluse_worktime[11][11] = {
/*Speed:0    1      2      3      4      5      6     7     8     9    10 */ /*Frq*/
	{3551,  3029,  2490, 1947,  1421,  885,   1064,  945,  825,  700,  586}, /*0*/
	{3028,  3028,  3028, 3028,  3028,  1180,  1065,  955,  825,  700,  586}, /*1*/
	{2490,  2490,  2490, 2490,  2490,  1180,  1065,  955,  825,  700,  586}, /*2*/
	{1950,  1950,  1950, 1950,  1950,  1180,  1065,  955,  825,  700,  586}, /*3*/
	{1422,  1422,  1422, 1422,  1422,  1180,  1065,  955,  825,  700,  586}, /*4*/
	{885,   885,  	885,  885,  885,    885,   885,  885,  885,  700,  586}, /*5*/
	{750,   750,  	750,  750,  750,    750,   750,  750,  750,  700,  586}, /*6*/
	{629,   629,  	629,  629,  629,    629,   629,  629,  629,  629,  586}, /*7*/
	{481,   481,  	481,  481,  481,    481,   481,  481,  481,  481,  481}, /*8*/
	{364,   364,  	364,  364,  364,    364,   364,  364,  364,  364,  364}, /*9*/
	{230,   230,  	230,  230,  230,    230,   230,  230,  230,  230,  230}	 /*10*/
};
#endif /*(Pulsing_USE == 1)*/

#if (Welding_USE == 1)
/*Welding间隔时间*/
uint16_t welding_interval_max[11] = {1000, 700, 650, 550, 450, 450, 450, 450, 300, 300, 100};
uint16_t welding_interval_min[11] = {40, 40, 40, 40, 40, 100, 100, 100, 100, 100, 100};
/*Welding工作时间*/
static const uint16_t welding_wt = 60;
uint16_t welding_min = 100;
static const uint16_t welding_rt_max[11] = {100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100};
static const uint16_t welding_rt_min[11] = {40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40};
uint16_t welding_r[11] = {70, 90, 200, 225, 280, 380, 450, 500, 600, 500, 500};
#endif /*(Welding_USE == 1)*/

#if (CopCar_USE == 1)
/*CopCar非连闪间隔时间、例如不同颜色间的间隔*/
static const uint16_t CopCar_CInterval[6] = {120, 240, 456, 672, 890, 810};
/*CopCar工作时间*/
static const uint16_t CopCar_WT[6] = {180, 70, 70, 70, 70, 90};
/*CopCar闪烁次数*/
static const uint8_t CopCar_FlashTimers[6] = {1, 2, 3, 4, 5, 5};
#endif /*(CopCar_USE == 1)*/

#if (ColorChase_USE == 1)
/*ColorChase一圈所用时间*/
static const uint16_t color_chase_ct[11] = {65535, 54500, 48500, 42500, 36500, 30100, 24400, 18260, 12500, 6800, 1000};
#endif /*(ColorChase_USE == 1)*/

#if (PartyLights_USE == 1)
/*PartyLights一圈所用时间*/
uint16_t party_lights_wt[11] = {20000, 16800, 9600, 7050, 6000, 5175, 4650, 4200, 4050, 3830, 3700};
uint16_t party_lights_ct[11] = {20000, 16800, 9600, 7050, 6000, 5175, 4650, 4200, 4050, 3830, 3700};
#endif /*(PartyLights_USE == 1)*/

#if (Fireworks_USE == 1)
/*Fireware间隔时间*/
static const uint16_t firework_interval_max[12] = {10890, 9893, 8808, 7648, 6940, 5853, 4896, 3926, 3041, 2145, 2000, 9893};
static const uint16_t firework_interval_min[12] = {8917, 7917, 6945, 5950, 4882, 3878, 2856, 1940, 1098, 144, 100, 100};
/*Fireware连闪次数*/
static const uint8_t fireware_flash_min[12] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
static const uint8_t fireware_flash_max[12] = {0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 3, 3};
#endif /*(Fireworks_USE == 1)*/

static uint8_t iam_here_status = 0;

uint8_t Get_EffectTypeIAmHere_Status(void)
{
	return iam_here_status;
}

void Clear_EffectTypeIAmHere_Status(void)
{
	iam_here_status = 0;
}


#if (ClubLights_USE == 1)
//数组顺序随机排列
static void Randon_Sort(const uint8_t *src, uint8_t *dst, uint8_t length)
{
	uint8_t random = 0;
	uint8_t i = 0;
	uint8_t temp = 0;
	
	memcpy(dst, src, length);
	
	for(; i < length; i++)
	{
		random = Effect_Get_RandomRange_Int16(0, length - 1);
		
		temp = dst[i];
		dst[i] = dst[random];
		dst[random] = temp;
	}
}
#endif

uint8_t Effect_Fade_PWM(uint16_t *current, uint16_t object)
{
	if(object == *current)
	{
		return 1;
	}
	if(abs(object - *current) < EFFECT_FADE_TIME)
	{
		if(object < *current)
		{
			*current = *current - 1;
		}
		else if(object > *current)
		{
			*current = *current + 1;
		}
	}
	else
	{
		*current = (*current + object) * 0.5f;
	}

	return 0;
}

// 1:缓变结束 
uint8_t Effect_Fade_Change(Effect_PulseWightRGBWW *current, Effect_PulseWightRGBWW object)
{
	uint8_t res = 1;
	
	res *= Effect_Fade_PWM(&current->r, object.r);
	res *= Effect_Fade_PWM(&current->g, object.g);
	res *= Effect_Fade_PWM(&current->b, object.b);
	res *= Effect_Fade_PWM(&current->ww, object.ww);
	res *= Effect_Fade_PWM(&current->cw, object.cw);

	return res;
}

static uint8_t Effect_Fade_Change_1(Effect_PulseWightRGBWW *current, Effect_PulseWightRGBWW object, uint16_t *run_time, uint16_t whole_time)
{
	uint8_t res = 0;
	
	static float rate_r = 0.0f;
	static float start_pwm_r = 0.0f;
	static float rate_g = 0.0f;
	static float start_pwm_g = 0.0f;
	static float rate_b = 0.0f;
	static float start_pwm_b = 0.0f;
	static float rate_ww = 0.0f;
	static float start_pwm_ww = 0.0f;
	static float rate_cw = 0.0f;
	static float start_pwm_cw = 0.0f;
	
	if (*run_time == 0)
	{
		rate_r = (float)(object.r - current->r) / whole_time;
		start_pwm_r = current->r;
		
		rate_g = (float)(object.g - current->g) / whole_time;
		start_pwm_g = current->g;
		
		rate_b = (float)(object.b - current->b) / whole_time;
		start_pwm_b = current->b;
		
		rate_ww = (float)(object.ww - current->ww) / whole_time;
		start_pwm_ww = current->ww;
		
		rate_cw = (float)(object.cw - current->cw) / whole_time;
		start_pwm_cw = current->cw;
	}
	
	if (object.r != current->r)
	{
		res = 1;
		current->r = start_pwm_r + rate_r * (*run_time);
	}
	if (object.g != current->g)
	{
		res = 1;
		current->g = start_pwm_g + rate_g * (*run_time);
	}
	if (object.b != current->b)
	{
		res = 1;
		current->b = start_pwm_b + rate_b * (*run_time);
	}
	if (object.ww != current->ww)
	{
		res = 1;
		current->ww = start_pwm_ww + rate_ww * (*run_time);
	}
	if (object.cw != current->cw)
	{
		res = 1;
		current->cw = start_pwm_cw + rate_cw * (*run_time);
	}
	
	if (*run_time < whole_time)
	{
		 (*run_time)++;
	}
	else
	{
		current->r = object.r;
		current->g = object.g;
		current->b = object.b;
		current->ww = object.ww;
		current->cw = object.cw;
	}
	
	return res;
}

/**********************************************************
函数: Effect_Pwm_LinearChange
函数描述: 线性变化pwm：DutyStart->DutyEnd，时间TimeStart->TimeEnd
参数：
	pwm_start: 开始占空比
	time_start: 开始时间
	pwm_end: 结束占空比
	time_end: 结束时间
	whole_time：全部时间，工作时间加上间隔时间
	run_time : time运行时间
	pwm_current:当前pwm
	data_update：数据更新标志
返回值： pwm值，1表示全部时间完成
作者:
日期:2019/04/13
***********************************************************/
static int8_t Effect_Pwm_LinearChange(volatile Effect_PulseWightRGBWW *pwm_start,  volatile Effect_PulseWightRGBWW *pwm_end,
										volatile float time_start, volatile float time_end, volatile float whole_time, 
											volatile Effect_PulseWightRGBWW *pwm_current, float *run_time, uint8_t *data_update)
{
	int8_t ret = 0;
	float time1;
	float time2;

	static Effect_PulseWightRGBWW pwm_object = {0};
	static float whole_time_last = 0;
	static uint16_t fade_run_time = 0;
	
	if(*data_update == 0)
	{
		if(*run_time <= whole_time)	//周期时间，包括工作时间和间隔时间
		{
			if(*run_time <= time_end)	 //工作时间，pwm按线性变化
			{
				time1 = time_end - time_start;
				time2 = *run_time - time_start;
				
				pwm_current->r = pwm_start->r + (int32_t)(pwm_end->r - pwm_start->r)/time1 * time2;
				pwm_current->g = pwm_start->g + (int32_t)(pwm_end->g - pwm_start->g)/time1 * time2;
				pwm_current->b = pwm_start->b + (int32_t)(pwm_end->b - pwm_start->b)/time1 * time2;
				pwm_current->ww = pwm_start->ww + (int32_t)(pwm_end->ww - pwm_start->ww)/time1 * time2;
				pwm_current->cw = pwm_start->cw + (int32_t)(pwm_end->cw - pwm_start->cw)/time1 * time2;
				
			}
			else  //进入间隔时间，灭灯
			{
				pwm_current->r = 0;
				pwm_current->g = 0;
				pwm_current->b = 0;
				pwm_current->ww = 0;
				pwm_current->cw = 0;
			}
		}
		else
		{
			ret = 1;
		}
		*run_time += 1.0f;
	}
	else if(*data_update == 1) //更新当前时间、计算当前数据对应的pwm值
	{

		*run_time = *run_time/whole_time_last * whole_time;
		*run_time = (*run_time > whole_time) ? whole_time : *run_time;

		time1 = time_end - time_start;
		time2 = *run_time - time_start;
		
		if (time2 > time1)
		{
			time2 = time1;
		}
		
//		pwm_current->r = pwm_start->r + (int32_t)(pwm_end->r - pwm_start->r)/time1 * time2;
//		pwm_current->g = pwm_start->g + (int32_t)(pwm_end->g - pwm_start->g)/time1 * time2;
//		pwm_current->b = pwm_start->b + (int32_t)(pwm_end->b - pwm_start->b)/time1 * time2;
//		pwm_current->ww = pwm_start->ww + (int32_t)(pwm_end->ww - pwm_start->ww)/time1 * time2;
//		pwm_current->cw = pwm_start->cw + (int32_t)(pwm_end->cw - pwm_start->cw)/time1 * time2;
		
		pwm_object.r = pwm_start->r + (int32_t)(pwm_end->r - pwm_start->r)/time1 * time2;
		pwm_object.g = pwm_start->g + (int32_t)(pwm_end->g - pwm_start->g)/time1 * time2;
		pwm_object.b = pwm_start->b + (int32_t)(pwm_end->b - pwm_start->b)/time1 * time2;
		pwm_object.ww = pwm_start->ww + (int32_t)(pwm_end->ww - pwm_start->ww)/time1 * time2;
		pwm_object.cw = pwm_start->cw + (int32_t)(pwm_end->cw - pwm_start->cw)/time1 * time2;
		*data_update = 2;
		fade_run_time = 0;
		
	}//进入缓变处理
	else
	{
		if (Effect_Fade_Change_1((Effect_PulseWightRGBWW *)pwm_current, pwm_object, &fade_run_time, EFFECT_FADE_TIME) == 0)
		//if(Effect_Fade_Change((Effect_PulseWightRGBWW *)pwm_current, pwm_object) == 1)
		{
			*data_update = 0;
		}
	}
	whole_time_last = whole_time;
	
	return ret;
}

#if ((PartyLights_USE == 1) || (ColorChase_USE == 1))
//用于ColorChase和PartyLights
static uint8_t Effect_Pwm_LinearChange_1(volatile uint16_t index_start, volatile uint16_t index_end, volatile float time_start,
												volatile float time_end, volatile float whole_time,  volatile Effect_PulseWightRGBWW *pwm_src, 
													volatile Effect_PulseWightRGBWW *pwm_current, float *time, uint8_t *data_update)
{
	int8_t ret = 0;
	float index = 0;
	uint32_t index_temp = 0;
	static Effect_PulseWightRGBWW rgbww_pwm_object = {0};
	static uint16_t whole_time_last = 0;
	static uint16_t fade_run_time = 0;

	if(*data_update == 0)
	{
		if(*time <= whole_time)	//周期时间，包括工作时间和间隔时间
		{
			if(*time <= time_end)	 //工作时间，亮度按线性变化
			{
				index = (float)(index_start + (index_end - index_start)/(float)(time_end - time_start)*(*time - time_start));
			}
			else  //进入间隔时间，灭灯
			{
				index = 0;
			}
			index_temp = index;
			index_temp = (index > index_temp)? (index_temp + 1) : index_temp;
			pwm_src[index_temp].r;
			if(index > index_temp)
			{
				pwm_current->r = pwm_src[index_temp].r + (int16_t)(pwm_src[index_temp + 1].r - pwm_src[index_temp].r)*(index - index_temp);
				pwm_current->g = pwm_src[index_temp].g + (int16_t)(pwm_src[index_temp + 1].g - pwm_src[index_temp].g)*(index - index_temp);
				pwm_current->b = pwm_src[index_temp].b + (int16_t)(pwm_src[index_temp + 1].b - pwm_src[index_temp].b)*(index - index_temp);
				pwm_current->ww = pwm_src[index_temp].ww + (int16_t)(pwm_src[index_temp + 1].ww - pwm_src[index_temp].ww)*(index - index_temp);
				pwm_current->cw = pwm_src[index_temp].cw + (int16_t)(pwm_src[index_temp + 1].cw - pwm_src[index_temp].cw)*(index - index_temp);
			}
			else
			{
				pwm_current->r = pwm_src[index_temp].r;
				pwm_current->g = pwm_src[index_temp].g;
				pwm_current->b = pwm_src[index_temp].b;
				pwm_current->ww = pwm_src[index_temp].ww;
				pwm_current->cw = pwm_src[index_temp].cw;
			}
		}
		else
		{
			ret = 1;
		}
		*time += 1;
	}
	else if(*data_update == 1)
	{
		*time = *time/(float)whole_time_last * whole_time;
		*time = (*time > whole_time) ? whole_time : *time;
		
		if(*time <= time_end)	 //工作时间，亮度按线性变化
		{
			index = (int16_t)(index_start + (int32_t)(index_end - index_start)/(float)(time_end - time_start)*(*time - time_start));
		}
		else  //进入间隔时间，灭灯
		{
			index = 0;
		}
		index_temp = index;
		
		
//		pwm_current->r = pwm_src[index_temp].r;
//		pwm_current->g = pwm_src[index_temp].g;
//		pwm_current->b = pwm_src[index_temp].b;
//		pwm_current->ww = pwm_src[index_temp].ww;
//		pwm_current->cw = pwm_src[index_temp].cw;
		
		rgbww_pwm_object.r = pwm_src[index_temp].r;
		rgbww_pwm_object.g = pwm_src[index_temp].g;
		rgbww_pwm_object.b = pwm_src[index_temp].b;
		rgbww_pwm_object.ww = pwm_src[index_temp].ww;
		rgbww_pwm_object.cw = pwm_src[index_temp].cw;
		
		fade_run_time = 0;
		*data_update  = 2;
	}
	else
	{
		//if(Effect_Fade_Change((Effect_PulseWightRGBWW *)pwm_current, rgbww_pwm_object) == 1)
		if (Effect_Fade_Change_1((Effect_PulseWightRGBWW *)pwm_current, rgbww_pwm_object, &fade_run_time, EFFECT_FADE_TIME) == 0)
		{
			*data_update = 0;
		}
	}
	whole_time_last = whole_time;

	return ret;
}
#endif /*((PartyLights_USE == 1) || (Fireworks_USE == 1))*/

#if ((Candle_USE == 1) || (Fire_USE == 1))
static uint8_t Fire_Pwm_Change(volatile Effect_PulseWightRGBWW pwm_min, volatile Effect_PulseWightRGBWW pwm_max, volatile float work_time,
									volatile float rate, volatile Effect_PulseWightRGBWW *current_pwm, volatile float *time, uint8_t *data_update)
{
	uint8_t ret = 0;
	static uint16_t fade_run_time = 0;
	static Effect_PulseWightRGBWW pwm_last = {0};
	float rate_r = ((int32_t)(pwm_max.r - pwm_min.r))/10000.0f;
	float rate_g = ((int32_t)(pwm_max.g - pwm_min.g))/10000.0f;
	float rate_b = ((int32_t)(pwm_max.b - pwm_min.b))/(float)10000.0f;
	float rate_ww =((int32_t)(pwm_max.ww - pwm_min.ww))/(float)10000.0f;
	float rate_cw =((int32_t)(pwm_max.cw - pwm_min.cw))/(float)10000.0f;
	
	if(*data_update == 0)
	{
		if(rate < 0)
		{
			if(pwm_min.r < pwm_max.r)
			{
				if((current_pwm->r + rate * rate_r) > pwm_min.r)
				{
					current_pwm->r = pwm_last.r + rate_r * rate * (*time);
				}
				else
				{
					current_pwm->r = pwm_min.r;
				}
			}
			else
			{
				if((current_pwm->r + rate * rate_r) < pwm_min.r)
				{
					current_pwm->r = pwm_last.r + rate_r * rate * (*time);
				}
				else
				{
					current_pwm->r = pwm_min.r;
				}
			}

			if(pwm_min.g < pwm_max.g)
			{
				if((current_pwm->g + rate * rate_g) > pwm_min.g)
				{
					current_pwm->g = pwm_last.g + rate_g * rate * (*time);
				}
				else
				{
					current_pwm->g = pwm_min.g;
				}
			}
			else
			{
				if((current_pwm->g + rate * rate_g) < pwm_min.g)
				{
					current_pwm->g = pwm_last.g + rate_g * rate * (*time);
				}
				else
				{
					current_pwm->g = pwm_min.g;
				}
			}

			if(pwm_min.b < pwm_max.b)
			{
				if((current_pwm->b + rate * rate_b) > pwm_min.b)
				{
					current_pwm->b = pwm_last.b + rate_b * rate * (*time);
				}
				else
				{
					current_pwm->b = pwm_min.b;
				}
			}
			else
			{
				if((current_pwm->b + rate * rate_b) < pwm_min.b)
				{
					current_pwm->b = pwm_last.b + rate_b * rate * (*time);
				}
				else
				{
					current_pwm->b = pwm_min.b;
				}
			}

			if(pwm_min.ww < pwm_max.ww)
			{
				if((current_pwm->ww + rate * rate_ww) > pwm_min.ww)
				{
					current_pwm->ww = pwm_last.ww + rate_ww * rate * (*time);
				}
				else
				{
					current_pwm->ww = pwm_min.ww;
				}
			}
			else
			{
				if((current_pwm->ww + rate * rate_ww) < pwm_min.ww)
				{
					current_pwm->ww = pwm_last.ww + rate_ww * rate * (*time);
				}
				else
				{
					current_pwm->ww = pwm_min.ww;
				}
			}

			if(pwm_min.cw < pwm_max.cw)
			{
				if((current_pwm->cw + rate * rate_cw) > pwm_min.cw)
				{
					current_pwm->cw = pwm_last.cw + rate_cw * rate * (*time);
				}
				else
				{
					current_pwm->cw = pwm_min.cw;
				}
			}
			else
			{
				if((current_pwm->cw + rate * rate_cw) < pwm_min.cw)
				{
					current_pwm->cw = pwm_last.cw + rate_cw * rate * (*time);
				}
				else
				{
					current_pwm->cw = pwm_min.cw;
				}
			}
		}
		else if(rate > 0)
		{
			if(pwm_min.r < pwm_max.r)
			{
				if((current_pwm->r + rate * rate_r) < pwm_max.r)
				{
					current_pwm->r = pwm_last.r + rate_r * rate * (*time);
				}
				else
				{
					current_pwm->r = pwm_max.r;
				}
			}
			else
			{
				if((current_pwm->r + rate * rate_r) > pwm_max.r)
				{
					current_pwm->r = pwm_last.r + rate_r * rate * (*time);
				}
				else
				{
					current_pwm->r = pwm_max.r;
				}
			}

			if(pwm_min.g < pwm_max.g)
			{
				if((current_pwm->g + rate * rate_g) < pwm_max.g)
				{
					current_pwm->g = pwm_last.g + rate_g * rate * (*time);
				}
				else
				{
					current_pwm->g = pwm_max.g;
				}
			}
			else
			{
				if((current_pwm->g + rate * rate_g) > pwm_max.g)
				{
					current_pwm->g = pwm_last.g + rate_g * rate * (*time);
				}
				else
				{
					current_pwm->g = pwm_max.g;
				}
			}

			if(pwm_min.b < pwm_max.b)
			{
				if((current_pwm->b + rate * rate_b) < pwm_max.b)
				{
					current_pwm->b = pwm_last.b + rate_b * rate * (*time);
				}
				else
				{
					current_pwm->b = pwm_max.b;
				}
			}
			else
			{
				if((current_pwm->b + rate * rate_b) > pwm_max.b)
				{
					current_pwm->b = pwm_last.b + rate_b * rate * (*time);
				}
				else
				{
					current_pwm->b = pwm_max.b;
				}
			}

			if(pwm_min.ww < pwm_max.ww)
			{
				if((current_pwm->ww + rate * rate_ww) < pwm_max.ww)
				{
					current_pwm->ww = pwm_last.ww + rate_ww * rate * (*time);
				}
				else
				{
					current_pwm->ww = pwm_max.ww;
				}
			}
			else
			{
				if((current_pwm->ww + rate * rate_ww) > pwm_max.ww)
				{
					current_pwm->ww = pwm_last.ww + rate_ww * rate * (*time);
				}
				else
				{
					current_pwm->ww = pwm_max.ww;
				}
			}

			if(pwm_min.cw < pwm_max.cw)
			{
				if((current_pwm->cw + rate * rate_cw) < pwm_max.cw)
				{
					current_pwm->cw = pwm_last.cw + rate_cw * rate * (*time);
				}
				else
				{
					current_pwm->cw = pwm_max.cw;
				}
			}
			else
			{
				if((current_pwm->cw + rate * rate_cw) > pwm_max.cw)
				{
					current_pwm->cw = pwm_last.cw + rate_cw * rate * (*time);
				}
				else
				{
					current_pwm->cw = pwm_max.cw;
				}
			}
		}
		else	//无外力时自动恢复
		{

		}

		if(*time > work_time)
		{
			pwm_last.r = current_pwm->r;
			pwm_last.g = current_pwm->g;
			pwm_last.b = current_pwm->b;
			pwm_last.ww = current_pwm->ww;
			pwm_last.cw = current_pwm->cw;
			ret = 1;
		}
		(*time)++;
		fade_run_time = 0;
		
//		if(pwm_min.r < pwm_max.r)
//		{
//			if(current_pwm->r > pwm_max.r || current_pwm->r < pwm_min.r)
//			{
//				__NOP();
//			}
//		}
//		else
//		{
//			if(current_pwm->r < pwm_max.r || current_pwm->r > pwm_min.r)
//			{
//				__NOP();
//			}
//		}
//		if(pwm_min.g < pwm_max.g)
//		{
//			if(current_pwm->g > pwm_max.g || current_pwm->g < pwm_min.g)
//			{
//				__NOP();
//			}
//		}
//		else
//		{
//			if(current_pwm->g < pwm_max.g || current_pwm->g > pwm_min.g)
//			{
//				__NOP();
//			}
//		}
//		if(pwm_min.b < pwm_max.b)
//		{
//			if(current_pwm->b > pwm_max.b || current_pwm->b < pwm_min.b)
//			{
//				__NOP();
//			}
//		}
//		else
//		{
//			if(current_pwm->b < pwm_max.b || current_pwm->b > pwm_min.b)
//			{
//				__NOP();
//			}
//		}
//		if(pwm_min.ww < pwm_max.ww)
//		{
//			if(current_pwm->ww > pwm_max.ww || current_pwm->ww < pwm_min.ww)
//			{
//				__NOP();
//			}
//		}
//		else
//		{
//			if(current_pwm->ww < pwm_max.ww || current_pwm->ww > pwm_min.ww)
//			{
//				__NOP();
//			}
//		}
//		if(pwm_min.cw < pwm_max.cw)
//		{
//			if(current_pwm->cw > pwm_max.cw || current_pwm->cw < pwm_min.cw)
//			{
//				__NOP();
//			}
//		}
//		else
//		{
//			if(current_pwm->cw < pwm_max.cw || current_pwm->cw > pwm_min.cw)
//			{
//				__NOP();
//			}
//		}
	}
	else
	{
		if (Effect_Fade_Change_1((Effect_PulseWightRGBWW *)current_pwm, pwm_min, &fade_run_time, EFFECT_FADE_TIME) == 0)
		{
			pwm_last.r = current_pwm->r;
			pwm_last.g = current_pwm->g;
			pwm_last.b = current_pwm->b;
			pwm_last.ww = current_pwm->ww;
			pwm_last.cw = current_pwm->cw;
			*data_update = 0;
			ret = 1;
		}
	}
	
	return ret;
}

static Effect_Type effect_type_last = EffectTypeNULL;

void Clear_Last_EffectType(void)
{
	effect_type_last = EffectTypeNULL;
}
#endif /*(Candle_USE == 1) || (EffectType_Fire == 1)*/ 

//static Effect_Type effect_type_last = EffectTypeNULL;修改了
//光效数据初始化
void Effect_Data_Init(const Light_Effect* effect)		//光效数据初始化
{
	Effect_Type type = effect->effect_type;
	static Effect_Type type_last = EffectTypeNULL;
	
	uint16_t lightness = 0;
	static uint16_t lightness_last = 0;
	
	uint16_t cct = 0;
	static uint16_t cct_last = 0;
	
	uint8_t  gm = 0;
	static uint8_t gm_last = 0;
	
	Effect_Mode mode = EffectModeNULL;
	static Effect_Mode mode_last = EffectModeNULL;
	
	uint16_t hue = 0;
	static uint16_t hue_last = 0;
	
	uint8_t	sat = 0;
	static uint8_t sat_last = 0;
	
	uint8_t club_color = 0;
	static uint8_t club_color_last = 0;
	
	uint8_t gel_brand = 0;
	static uint8_t gel_brand_last = 0;
	uint8_t gel_type = 0;
	static uint8_t gel_type_last = 0;
	uint16_t gel_color = 0;
	static uint16_t gel_color_last = 0;
	
	uint16_t coord_x = 0;
	static uint16_t coord_x_last = 0;
	uint16_t coord_y = 0;
	static uint16_t coord_y_last = 0;
	
	uint8_t firework_color = 0;
	static uint8_t firework_color_last = 0;
	
	static uint8_t rgbww_pwm_arr_index = 0;
	static uint8_t effect_data_index = 0;
	
	CopCar_Type cop_car_type = CopCar_Color_R;
	static CopCar_Type cop_car_type_last;
	
	uint8_t source_type = 0;
	static uint8_t source_type_last;
	
	uint16_t index = 0;
	
	uint8_t data_updated = 0;
	
	//消除未使用警告
	(void)cct_last;
	(void)gm_last;
	(void)mode_last;
	(void)hue_last;
	(void)type_last;
	(void)lightness_last;
	(void)sat_last;
	(void)club_color_last;
	(void)gel_brand_last;
	(void)gel_type_last;
	(void)gel_color_last;
	(void)coord_x_last;
	(void)coord_y_last;
	(void)firework_color_last;
	(void)index;
	(void)cop_car_type_last;
	(void)source_type_last;
	
	switch(type)
	{
		#if (ClubLights_USE == 1)
		case EffectTypeClubLights:
			lightness = effect->effect_arg.club_lights_arg.lightness;
			sat = 99;
			club_color = (effect->effect_arg.club_lights_arg.color <= 7) ? effect->effect_arg.club_lights_arg.color : 7;
			club_color = club_color_arr[club_color];
		
			if(effect_type_last != type || lightness_last != lightness || club_color_last != club_color)  //亮度、光效类型、颜色种类变化时重新初始化参数
			{
				rgbww_pwm_arr_index = rgbww_pwm_arr_index%2 + 1;
				for(index = 360/club_color; index <= 360; index += 360/club_color)
				{
					Effect_HSI2PulseWight(index, sat, lightness, &rgbww_pwm_arr[rgbww_pwm_arr_index - 1][index/(360/club_color) - 1], 1);
				}
			}
			break;
		#endif /*ClubLights_USE*/
		
		#if (Paparazzi_USE == 1)
		case EffectTypePaparazzi:
			lightness = effect->effect_arg.paparazzi_arg.lightness;
			cct = effect->effect_arg.paparazzi_arg.cct;
			gm = effect->effect_arg.paparazzi_arg.gm;
			if(effect_type_last != type || lightness_last != lightness || cct_last != cct || gm_last != gm)  //亮度、cct、光效类型、gm变化时重新初始化参数
			{	
				data_updated = 1;
				rgbww_pwm_arr_index = rgbww_pwm_arr_index%2 + 1;
				Effect_CCT2PulseWight(cct, gm, lightness, &rgbww_pwm_arr[rgbww_pwm_arr_index - 1][0]);
			}
			break;
		#endif /*Paparazzi_USE*/
		
		#if (Lightning_USE == 1)
		case EffectTypeLightning:
			lightness = effect->effect_arg.lightning_arg.lightness;
			cct = effect->effect_arg.lightning_arg.cct;
			gm = effect->effect_arg.lightning_arg.gm;
			if(effect_type_last != type || lightness_last != lightness || cct_last != cct || gm_last != gm)  //光效类型、亮度、cct、gm变化时重新初始化参数
			{
				rgbww_pwm_arr_index = rgbww_pwm_arr_index%2 + 1;
				Effect_CCT2PulseWight(cct, gm, lightness, &rgbww_pwm_arr[rgbww_pwm_arr_index - 1][index]);
			}
			trigger_type = effect->effect_arg.lightning_arg.trigger;  //触发类型
			break;
		#endif /*Lightning_USE*/
		
		#if (TV_USE == 1)
		case EffectTypeTV:
			lightness = effect->effect_arg.tv_arg.lightness;
			cct = effect->effect_arg.tv_arg.cct;
			if (effect_type_last != type || lightness_last != lightness || cct_last != cct)  //光效类型、亮度、cct范围变化时重新初始化参数
			{
				data_updated = 1;
				rgbww_pwm_arr_index = rgbww_pwm_arr_index%2 + 1;
				#if (PRODUCT_FUNCTION != 0) //非单色温
					if (cct == 0)
					{
						for(index = TV_CCT_RANGE1_LOW; index <= TV_CCT_RANGE1_UP; index += 100) 		//TV_CCT_RANGE1_LOW ~ TV_CCT_RANGE1_UP
						{
							Effect_CCT2PulseWight(index, 10, lightness, &rgbww_pwm_arr[rgbww_pwm_arr_index - 1][(index - TV_CCT_RANGE1_LOW)/100]);
						}
					}
					else if (cct == 1)
					{
						for (index = TV_CCT_RANGE2_LOW; index <= TV_CCT_RANGE2_UP; index += 100) 		//TV_CCT_RANGE2_LOW ~ TV_CCT_RANGE2_UP
						{
							Effect_CCT2PulseWight(index, 10, lightness, &rgbww_pwm_arr[rgbww_pwm_arr_index - 1][(index - TV_CCT_RANGE2_LOW)/100]);
						}
					}
					else
					{
						for (index = TV_CCT_RANGE3_LOW; index <= TV_CCT_RANGE3_UP; index += 100) 		//TV_CCT_RANGE3_LOW ~ TV_CCT_RANGE3_UP
						{
							Effect_CCT2PulseWight(index, 10, lightness, &rgbww_pwm_arr[rgbww_pwm_arr_index - 1][(index - TV_CCT_RANGE3_LOW)/100]);
						}
					}
				#else //单色温模式
					Effect_CCT2PulseWight(index, 10, lightness, &rgbww_pwm_arr[rgbww_pwm_arr_index - 1][0]);
				#endif /*PRODUCT_FUNCTION != 0*/
			}
			break;
		#endif /*TV_USE*/
		
		#if (Candle_USE == 1)
		case EffectTypeCandle:
			lightness = effect->effect_arg.candle_arg.lightness;
			cct = effect->effect_arg.candle_arg.cct;
			if(effect_type_last != type || lightness_last != lightness || cct_last != cct)  //光效类型、亮度和cct变化时重新初始化参数
			{
				rgbww_pwm_arr_index = rgbww_pwm_arr_index%2 + 1;
				data_updated = 1;
				if(cct == 0)
				{
					for(index = CANDLE_CCT_RANGE1_LOW; index <= CANDLE_CCT_RANGE1_UP; index += 100) 		//CANDLE_CCT_RANGE1_LOW ~ CANDLE_CCT_RANGE1_UP
					{
						Effect_CCT2PulseWight(index, 10, lightness, &rgbww_pwm_arr[rgbww_pwm_arr_index - 1][(index - CANDLE_CCT_RANGE1_LOW)/100]);
					}
				}
				else if(cct == 1)
				{
					for(index = CANDLE_CCT_RANGE2_LOW; index <= CANDLE_CCT_RANGE2_UP; index += 100) 		//CANDLE_CCT_RANGE2_LOW ~ CANDLE_CCT_RANGE2_UP
					{
						Effect_CCT2PulseWight(index, 10, lightness, &rgbww_pwm_arr[rgbww_pwm_arr_index - 1][(index - CANDLE_CCT_RANGE2_LOW)/100]);
					}
				}
				else
				{
					for(index = CANDLE_CCT_RANGE3_LOW; index <= CANDLE_CCT_RANGE3_UP; index += 100) 		//CANDLE_CCT_RANGE3_LOW ~ CANDLE_CCT_RANGE3_UP
					{
						Effect_CCT2PulseWight(index, 10, lightness, &rgbww_pwm_arr[rgbww_pwm_arr_index - 1][(index - CANDLE_CCT_RANGE3_LOW)/100]);
					}
				}
			}
			break;
		#endif /*Candle_USE*/
		
		#if (Fire_USE == 1)
		case EffectTypeFire:
			lightness = effect->effect_arg.candle_arg.lightness;
			cct = effect->effect_arg.candle_arg.cct;
			if(effect_type_last != type || lightness_last != lightness || cct_last != cct)  //光效类型、亮度和cct变化时重新初始化参数
			{
				rgbww_pwm_arr_index = rgbww_pwm_arr_index%2 + 1;
				data_updated = 1;
				if(cct == 0)
				{
					for(index = FIRE_CCT_RANGE1_LOW; index <= FIRE_CCT_RANGE1_UP; index += 100) 		//FIRE_CCT_RANGE1_LOW ~ FIRE_CCT_RANGE1_UP
					{
						Effect_CCT2PulseWight(index, 10, lightness, &rgbww_pwm_arr[rgbww_pwm_arr_index - 1][(index - FIRE_CCT_RANGE1_LOW)/100]);
					}
				}
				else if(cct == 1)
				{
					for(index = FIRE_CCT_RANGE2_LOW; index <= FIRE_CCT_RANGE2_UP; index += 100) 		//FIRE_CCT_RANGE2_LOW ~ FIRE_CCT_RANGE2_UP
					{
						Effect_CCT2PulseWight(index, 10, lightness, &rgbww_pwm_arr[rgbww_pwm_arr_index - 1][(index - FIRE_CCT_RANGE2_LOW)/100]);
					}
				}
				else
				{
					for(index = FIRE_CCT_RANGE3_LOW; index <= FIRE_CCT_RANGE3_UP; index += 100) 		//FIRE_CCT_RANGE3_LOW ~ FIRE_CCT_RANGE3_UP
					{
						Effect_CCT2PulseWight(index, 10, lightness, &rgbww_pwm_arr[rgbww_pwm_arr_index - 1][(index - FIRE_CCT_RANGE3_LOW)/100]);
					}
				}
			}
			break;
		#endif /*Fire_USE*/
			
		#if (Strobe_USE == 1 || Explosion_USE == 1 || FaultBulb_USE == 1 || Pulsing_USE == 1 || Welding_USE == 1)
		case EffectTypeStrobe:
		case EffectTypeExplosion:
		case EffectTypeFaultBulb:
		case EffectTypePulsing:
		case EffectTypeWelding:
			mode = effect->effect_arg.strobe_arg.mode;
//			Effect_Frq = Effect_Body->Effect_Arg.Strobe_Arg.FRQ;
			
				
			switch(mode)
			{
				case EffectModeCCT:
					lightness = effect->effect_arg.strobe_arg.arg.cct_arg.lightness;
					cct = effect->effect_arg.strobe_arg.arg.cct_arg.cct;
					gm = effect->effect_arg.strobe_arg.arg.cct_arg.gm;
				
					if(effect_type_last != type || mode_last != mode || lightness_last != lightness || cct_last != cct || gm_last != gm)  //重新初始化参数
					{
						data_updated = 1;
						rgbww_pwm_arr_index = rgbww_pwm_arr_index%2 + 1;
						Effect_CCT2PulseWight(cct, gm, lightness, &rgbww_pwm_arr[rgbww_pwm_arr_index - 1][0]);
					}
					break;
				#if (PRODUCT_FUNCTION == 2 || PRODUCT_FUNCTION == 3) //简单彩色模式和全功能彩色模式有hsi功能
				case EffectModeHSI:
					lightness = effect->effect_arg.strobe_arg.arg.hsi_arg.lightness;
					hue = effect->effect_arg.strobe_arg.arg.hsi_arg.hue;
					sat = effect->effect_arg.strobe_arg.arg.hsi_arg.sat;	
					if(effect_type_last != type || mode_last != mode || lightness_last != lightness || hue_last != hue || sat_last != sat)  //重新初始化参数
					{
						data_updated = 1;
						rgbww_pwm_arr_index = rgbww_pwm_arr_index%2 + 1;
						Effect_HSI2PulseWight(hue, sat, lightness, &rgbww_pwm_arr[rgbww_pwm_arr_index - 1][0], 1);
					}
					break;
				#endif /*RODUCT_FUNCTION == 2 || PRODUCT_FUNCTION == 3*/
				
				#if (PRODUCT_FUNCTION == 3) //全功能彩色模式有gel功能和xy色坐标功能
				case EffectModeGEL:
					lightness = effect->effect_arg.strobe_arg.arg.gel_arg.lightness;
                    cct = effect->effect_arg.strobe_arg.arg.gel_arg.cct;
                    gel_brand = effect->effect_arg.strobe_arg.arg.gel_arg.brand;
                    gel_type = effect->effect_arg.strobe_arg.arg.gel_arg.type;
                    gel_color = effect->effect_arg.strobe_arg.arg.gel_arg.color;
                    if(effect_type_last != type || mode_last != mode || lightness_last != lightness || cct_last != cct || \
						gel_brand_last != gel_brand || gel_type_last != gel_type || gel_color_last != gel_color)  //重新初始化参数
                    {
                        data_updated = 1;
                        rgbww_pwm_arr_index = rgbww_pwm_arr_index%2 + 1;
                        Effect_Gel2PulseWight(gel_brand, gel_type, cct, gel_color, lightness, &rgbww_pwm_arr[rgbww_pwm_arr_index - 1][0]);
                    }
					break;
				case EffectModeCoord:
					lightness =  effect->effect_arg.strobe_arg.arg.coord_arg.lightness;
					coord_x = effect->effect_arg.strobe_arg.arg.coord_arg.x;
					coord_y = effect->effect_arg.strobe_arg.arg.coord_arg.y;
					if(effect_type_last != type || mode_last != mode || lightness_last != lightness || coord_x_last != coord_x || coord_y_last != coord_y)  //重新初始化参数
					{
						data_updated = 1;
                        rgbww_pwm_arr_index = rgbww_pwm_arr_index%2 + 1;
						Effect_Coord2PulseWight(coord_x, coord_y, lightness, &rgbww_pwm_arr[rgbww_pwm_arr_index - 1][0]);
					}
					break;
				case EffectModeSource:
					lightness =  effect->effect_arg.strobe_arg.arg.source_arg.lightness;
					source_type = effect->effect_arg.strobe_arg.arg.source_arg.type;
					if(effect_type_last != type || mode_last != mode || lightness_last != lightness || source_type_last != source_type)  //重新初始化参数
					{
						data_updated = 1;
                        rgbww_pwm_arr_index = rgbww_pwm_arr_index%2 + 1;
						Effect_Source2PulseWight(source_type, lightness, &rgbww_pwm_arr[rgbww_pwm_arr_index - 1][0]);
					}
					break;
				#endif /*PRODUCT_FUNCTION == 3*/
				default:
					break;
			}
			
			if(type == EffectTypeExplosion)
			{
				trigger_type = effect->effect_arg.explosion_arg.trigger;  //触发类型
			}
			if (type == EffectTypeStrobe)
			{
				data_updated = 0;
			}
			break;
		#endif /*Strobe_USE == 1 || Explosion_USE == 1 || FaultBulb_USE == 1 || Pulsing_USE == 1 || Welding_USE == 1*/
		
		#if (CopCar_USE == 1)
		case EffectTypeCopCar:
			lightness = effect->effect_arg.cop_car_arg.lightness;
			cop_car_type = (CopCar_Type)effect->effect_arg.cop_car_arg.color;
			if(effect_type_last != type || cop_car_type_last != cop_car_type || lightness_last != lightness)
			{
				rgbww_pwm_arr_index = rgbww_pwm_arr_index%2 + 1;
				if(cop_car_type == CopCar_Color_R)
				{
					Effect_HSI2PulseWight(1, 100, lightness, &rgbww_pwm_arr[rgbww_pwm_arr_index - 1][0], 1); //R
					Effect_HSI2PulseWight(1, 100, lightness, &rgbww_pwm_arr[rgbww_pwm_arr_index - 1][1], 1); //R
				}
				else if(cop_car_type == CopCar_Color_B)
				{
					Effect_HSI2PulseWight(240, 100, lightness, &rgbww_pwm_arr[rgbww_pwm_arr_index - 1][0], 1); //B
					Effect_HSI2PulseWight(240, 100, lightness, &rgbww_pwm_arr[rgbww_pwm_arr_index - 1][1], 1); //B
				}
				else if(cop_car_type == CopCar_Color_RB)
				{
					Effect_HSI2PulseWight(240, 100, lightness, &rgbww_pwm_arr[rgbww_pwm_arr_index - 1][0], 1); //B
					Effect_HSI2PulseWight(1, 100, lightness, &rgbww_pwm_arr[rgbww_pwm_arr_index - 1][1], 1); //R
					Effect_HSI2PulseWight(300, 100, lightness, &rgbww_pwm_arr[rgbww_pwm_arr_index - 1][2], 1); //B+R
				}
				else if(cop_car_type == CopCar_Color_BW)
				{
					Effect_HSI2PulseWight(240, 100, lightness, &rgbww_pwm_arr[rgbww_pwm_arr_index - 1][0], 1); //B
					Effect_HSI2PulseWight(240, 0, lightness, &rgbww_pwm_arr[rgbww_pwm_arr_index - 1][1], 1);  //W
					Effect_HSI2PulseWight(240, 50, lightness, &rgbww_pwm_arr[rgbww_pwm_arr_index - 1][2], 1); //B+W
				}
				else //R+B+W
				{
					Effect_HSI2PulseWight(240, 100, lightness, &rgbww_pwm_arr[rgbww_pwm_arr_index - 1][0], 1); //B
					Effect_HSI2PulseWight(240, 0, lightness, &rgbww_pwm_arr[rgbww_pwm_arr_index - 1][1], 1);  //W
					Effect_HSI2PulseWight(1, 100, lightness, &rgbww_pwm_arr[rgbww_pwm_arr_index - 1][2], 1);  //R
					Effect_HSI2PulseWight(300, 50, lightness, &rgbww_pwm_arr[rgbww_pwm_arr_index - 1][3], 1); // B+W=R
				}	
			}
			break;
		#endif /*CopCar_USE*/
		
		#if (ColorChase_USE == 1 || PartyLights_USE == 1)
		case EffectTypeColorChase:
		case EffectTypePartyLights:
			lightness = effect->effect_arg.color_chase_arg.lightness;
			sat = effect->effect_arg.color_chase_arg.sat;
			if(sat != 0)
			{
				if(effect_type_last != type || lightness_last != lightness || sat_last != sat)  //重新初始化参数
				{
					float pwm_per_angle_r = 0;
					float pwm_per_angle_g = 0;
					float pwm_per_angle_b = 0;
					float pwm_per_angle_ww = 0;
					float pwm_per_angle_cw = 0;
					
					data_updated = 1;
					rgbww_pwm_arr_index = rgbww_pwm_arr_index%2 + 1;
//					for(index = 0; index <= 360; index++)
//					{
//						Effect_HSI2PulseWight(index, sat, lightness, &rgbww_pwm_arr[rgbww_pwm_arr_index - 1][index], 1);
//					}
					Effect_HSI2PulseWight(0, sat, lightness, &rgbww_pwm_arr[rgbww_pwm_arr_index - 1][0], 1);
					Effect_HSI2PulseWight(120, sat, lightness, &rgbww_pwm_arr[rgbww_pwm_arr_index - 1][120], 1);
					Effect_HSI2PulseWight(240, sat, lightness, &rgbww_pwm_arr[rgbww_pwm_arr_index - 1][240], 1);
					
					pwm_per_angle_r = (rgbww_pwm_arr[rgbww_pwm_arr_index - 1][120].r - rgbww_pwm_arr[rgbww_pwm_arr_index - 1][0].r) / 120.0f;
					pwm_per_angle_g = (rgbww_pwm_arr[rgbww_pwm_arr_index - 1][120].g - rgbww_pwm_arr[rgbww_pwm_arr_index - 1][0].g) / 120.0f;
					pwm_per_angle_b = (rgbww_pwm_arr[rgbww_pwm_arr_index - 1][120].b - rgbww_pwm_arr[rgbww_pwm_arr_index - 1][0].b) / 120.0f;
					pwm_per_angle_ww = (rgbww_pwm_arr[rgbww_pwm_arr_index - 1][120].ww - rgbww_pwm_arr[rgbww_pwm_arr_index - 1][0].ww) / 120.0f;
					pwm_per_angle_cw = (rgbww_pwm_arr[rgbww_pwm_arr_index - 1][120].cw - rgbww_pwm_arr[rgbww_pwm_arr_index - 1][0].cw) / 120.0f;
					for(index = 1; index < 120; index++)
					{
						rgbww_pwm_arr[rgbww_pwm_arr_index - 1][index].r = rgbww_pwm_arr[rgbww_pwm_arr_index - 1][0].r + pwm_per_angle_r * index;
						rgbww_pwm_arr[rgbww_pwm_arr_index - 1][index].g = rgbww_pwm_arr[rgbww_pwm_arr_index - 1][0].g + pwm_per_angle_g * index;
						rgbww_pwm_arr[rgbww_pwm_arr_index - 1][index].b = rgbww_pwm_arr[rgbww_pwm_arr_index - 1][0].b + pwm_per_angle_b * index;
						rgbww_pwm_arr[rgbww_pwm_arr_index - 1][index].ww = rgbww_pwm_arr[rgbww_pwm_arr_index - 1][0].ww + pwm_per_angle_ww * index;
						rgbww_pwm_arr[rgbww_pwm_arr_index - 1][index].cw = rgbww_pwm_arr[rgbww_pwm_arr_index - 1][0].cw + pwm_per_angle_cw * index;
					}
					
					pwm_per_angle_r = (rgbww_pwm_arr[rgbww_pwm_arr_index - 1][240].r - rgbww_pwm_arr[rgbww_pwm_arr_index - 1][120].r) / 120.0f;
					pwm_per_angle_g = (rgbww_pwm_arr[rgbww_pwm_arr_index - 1][240].g - rgbww_pwm_arr[rgbww_pwm_arr_index - 1][120].g) / 120.0f;
					pwm_per_angle_b = (rgbww_pwm_arr[rgbww_pwm_arr_index - 1][240].b - rgbww_pwm_arr[rgbww_pwm_arr_index - 1][120].b) / 120.0f;
					pwm_per_angle_ww = (rgbww_pwm_arr[rgbww_pwm_arr_index - 1][240].ww - rgbww_pwm_arr[rgbww_pwm_arr_index - 1][120].ww) / 120.0f;
					pwm_per_angle_cw = (rgbww_pwm_arr[rgbww_pwm_arr_index - 1][240].cw - rgbww_pwm_arr[rgbww_pwm_arr_index - 1][120].cw) / 120.0f;
					for(index = 121; index < 240; index++)
					{
						rgbww_pwm_arr[rgbww_pwm_arr_index - 1][index].r = rgbww_pwm_arr[rgbww_pwm_arr_index - 1][120].r + pwm_per_angle_r * (index - 120);
						rgbww_pwm_arr[rgbww_pwm_arr_index - 1][index].g = rgbww_pwm_arr[rgbww_pwm_arr_index - 1][120].g + pwm_per_angle_g * (index - 120);
						rgbww_pwm_arr[rgbww_pwm_arr_index - 1][index].b = rgbww_pwm_arr[rgbww_pwm_arr_index - 1][120].b + pwm_per_angle_b * (index - 120);
						rgbww_pwm_arr[rgbww_pwm_arr_index - 1][index].ww = rgbww_pwm_arr[rgbww_pwm_arr_index - 1][120].ww + pwm_per_angle_ww * (index - 120);
						rgbww_pwm_arr[rgbww_pwm_arr_index - 1][index].cw = rgbww_pwm_arr[rgbww_pwm_arr_index - 1][120].cw + pwm_per_angle_cw * (index - 120);
					}
					
					pwm_per_angle_r = (rgbww_pwm_arr[rgbww_pwm_arr_index - 1][0].r - rgbww_pwm_arr[rgbww_pwm_arr_index - 1][240].r) / 120.0f;
					pwm_per_angle_g = (rgbww_pwm_arr[rgbww_pwm_arr_index - 1][0].g - rgbww_pwm_arr[rgbww_pwm_arr_index - 1][240].g) / 120.0f;
					pwm_per_angle_b = (rgbww_pwm_arr[rgbww_pwm_arr_index - 1][0].b - rgbww_pwm_arr[rgbww_pwm_arr_index - 1][240].b) / 120.0f;
					pwm_per_angle_ww = (rgbww_pwm_arr[rgbww_pwm_arr_index - 1][0].ww - rgbww_pwm_arr[rgbww_pwm_arr_index - 1][240].ww) / 120.0f;
					pwm_per_angle_cw = (rgbww_pwm_arr[rgbww_pwm_arr_index - 1][0].cw - rgbww_pwm_arr[rgbww_pwm_arr_index - 1][240].cw) / 120.0f;
					for(index = 241; index <= 360; index++)
					{
						rgbww_pwm_arr[rgbww_pwm_arr_index - 1][index].r = rgbww_pwm_arr[rgbww_pwm_arr_index - 1][240].r + pwm_per_angle_r * (index - 240);
						rgbww_pwm_arr[rgbww_pwm_arr_index - 1][index].g = rgbww_pwm_arr[rgbww_pwm_arr_index - 1][240].g + pwm_per_angle_g * (index - 240);
						rgbww_pwm_arr[rgbww_pwm_arr_index - 1][index].b = rgbww_pwm_arr[rgbww_pwm_arr_index - 1][240].b + pwm_per_angle_b * (index - 240);
						rgbww_pwm_arr[rgbww_pwm_arr_index - 1][index].ww = rgbww_pwm_arr[rgbww_pwm_arr_index - 1][240].ww + pwm_per_angle_ww * (index - 240);
						rgbww_pwm_arr[rgbww_pwm_arr_index - 1][index].cw = rgbww_pwm_arr[rgbww_pwm_arr_index - 1][240].cw + pwm_per_angle_cw * (index - 240);
					}
					
					//Effect_HSI2PulseWight(index, sat, lightness, &rgbww_pwm_arr[rgbww_pwm_arr_index - 1][index], 1);
					
				}
			}
			else
			{
				if(effect_type_last != type || lightness_last != lightness || sat_last != sat)  //重新初始化参数
				{
					data_updated = 1;
					rgbww_pwm_arr_index = rgbww_pwm_arr_index%2 + 1;
					for(index = EFFECT_CCT_MIN; index <= EFFECT_CCT_MAX; index += 100)
					{
						Effect_CCT2PulseWight(index, 10, lightness, &rgbww_pwm_arr[rgbww_pwm_arr_index - 1][(index - EFFECT_CCT_MIN)/100]);
					}
				}
			}
			break;
		#endif /*ColorChase_USE || PartyLights_USE*/
		
		#if (Fireworks_USE == 1)
		case EffectTypeFireworks:
			lightness = effect->effect_arg.fireworks_arg.lightness;
			firework_color = effect->effect_arg.fireworks_arg.lightness;	
			
			if(effect_type_last != type || lightness_last != lightness || firework_color_last != firework_color)  //重新初始化参数
			{
				data_updated = 1;
				rgbww_pwm_arr_index = rgbww_pwm_arr_index%2 + 1;
				#if (PRODUCT_FUNCTION == 2 || PRODUCT_FUNCTION == 3) //双色温简单彩色和全功能彩色
				for(index = 0; index <= 360; index++)
				{
					Effect_HSI2PulseWight(index, 100, lightness, &rgbww_pwm_arr[rgbww_pwm_arr_index - 1][index], 1);
				}
				if(effect_type_last != type || lightness_last != lightness)
				{
					for(; index <= (361 + (EFFECT_CCT_MAX - EFFECT_CCT_MIN)/100); index++) 
					{
						Effect_CCT2PulseWight((index - 361)*100 + EFFECT_CCT_MIN, 10, lightness, &rgbww_pwm_arr[rgbww_pwm_arr_index - 1][index]);
					}
				}
				#elif (PRODUCT_FUNCTION == 1)  //双色温
				if(effect_type_last != type || lightness_last != lightness)
				{
					for(; index <= ((EFFECT_CCT_MAX - EFFECT_CCT_MIN)/100); index++) 
					{
						Effect_CCT2PulseWight(index*100 + EFFECT_CCT_MIN, 10, lightness, &rgbww_pwm_arr[rgbww_pwm_arr_index - 1][index]);
					}
				}	
				#else
				if(effect_type_last != type || lightness_last != lightness)
				{
					Effect_CCT2PulseWight(EFFECT_CCT_MIN, 10, lightness, &rgbww_pwm_arr[rgbww_pwm_arr_index - 1][0]);
				}		
				#endif
			}
			break;
		#endif /*Fireworks_USE*/
		
		#if (IamHere_USE == 1)
		case EffectTypeIAmHere:
			if (effect->effect_arg.i_am_here.run)
			{
				data_updated = 1;
				rgbww_pwm_arr_index = rgbww_pwm_arr_index%2 + 1;
				Effect_CCT2PulseWight(4500, 10, 500, &rgbww_pwm_arr[rgbww_pwm_arr_index - 1][0]);
				trigger_type = Trigger_Once;  //触发类型
			}
			else
			{
				iam_here_status = 1;
			}
			break;
		#endif /*IamHere_USE*/
		
		default:
			break;
	}
	
	effect_type_last = type;
	lightness_last = lightness;
	club_color_last = club_color;
	cct_last = cct;
	gm_last = gm;
	mode_last = mode;
	hue_last = hue;
	sat_last = sat;
	gel_brand_last = gel_brand;
	gel_type_last = gel_type;
	gel_color_last = gel_color;	
	coord_x_last = coord_x;
	coord_y_last = coord_y;
	firework_color_last = firework_color;
	cop_car_type_last = cop_car_type;
	source_type_last = source_type;

	__disable_irq();
	p_rgbww_pwm = &rgbww_pwm_arr[rgbww_pwm_arr_index - 1][0];  //更新pwm数据
	
	effect_data_index = effect_data_index%2 +1;
	memcpy(&effect_data[effect_data_index - 1], effect, sizeof(Light_Effect));
	p_effect_data = &effect_data[effect_data_index - 1];	//更新光效结构体
	
	if(data_updated == 1)
	{
		deal_data_update = 1;
	}
	__enable_irq();
}

void Effect_Deal(uint8_t tick, uint8_t start)		// 处理光效
{
	Effect_Type type = p_effect_data->effect_type;
	static Effect_Type type_last = EffectTypeNULL;
	
	uint8_t effect_frq = 0;
	static uint8_t effect_frq_last = 0;
	
	uint8_t effect_speed = 0;
	static uint8_t effect_speed_last = 0;
	
	#if (TV_USE == 1)
	uint16_t effect_int = 0;
	static uint16_t effect_int_last = 0;
	#endif
	
	#if (ClubLights_USE == 1)
	uint8_t club_color = 0;
	static uint8_t club_color_last = 0;
	static uint8_t club_random_sort[36] = {0};
	static uint8_t random_sort_index = 0;
	static uint8_t club_change_type = 0;
	#endif

	static uint8_t run_stage = 0;
	
	static float work_tick, whole_tick;
	static float run_time = 0;

	static uint16_t interval_min = 0;
	static uint16_t interval_max = 0;

	static uint8_t flash_num = 0;
    static uint8_t flash_cnt = 0;
	
	static uint32_t work_interval;
	
	#if (TV_USE == 1)
	static uint16_t tv_random = 0;
	#endif
	
	static uint16_t random = 0;
	uint16_t random_1 = 0;
	uint16_t random_2 = 0;
	
	uint16_t cct = 0;
	static uint16_t cct_last = 0;
	
	static uint16_t index_begin, index_end, index;
	
	uint8_t firework_type = 0;
	
	#if (CopCar_USE == 1)
	CopCar_Type cop_car_type;
	static CopCar_TypeDef cop_car_arg = {0};
	static uint8_t cop_car_index = 0;
	static uint8_t area_num = 0;
	#endif
	#if (Fire_USE == 1)
	static float fire_rate = 0;
	#endif
	
	#if ((ColorChase_USE == 1) || (PartyLights_USE == 1))
	uint8_t sat = 0;
	static uint8_t sat_last = 0;
	#endif

	static Effect_PulseWightRGBWW* init_pwm = NULL;
	static Effect_PulseWightRGBWW  current_pwm = {0};
	Effect_PulseWightRGBWW  off_pwm = {0};  //默认pwm值，默认为最低亮度，即灭
	static Effect_PulseWightRGBWW temp_pwm = {0};
	static Effect_PulseWightRGBWW max_pwm = {0};
	static Effect_PulseWightRGBWW min_pwm = {0};


	uint8_t res = 0;
	
	/*消除未使用警告*/
	(void)firework_type;
 	(void)cct;
 	(void)cct_last;
 	(void)index_begin;
 	(void)index_end;
 	(void)index;
	(void)max_pwm;
	(void)min_pwm;
	(void)effect_speed_last;

	if(start == 1)
	{
		type_last = EffectTypeNULL;
	}
	
	switch(type)
	{
		#if (ClubLights_USE == 1)
		case EffectTypeClubLights:
			effect_frq = (p_effect_data->effect_arg.club_lights_arg.frq <= 11) ? (p_effect_data->effect_arg.club_lights_arg.frq) : 11;
			club_color = (p_effect_data->effect_arg.club_lights_arg.color <= 7) ? p_effect_data->effect_arg.club_lights_arg.color : 7;
			club_color = club_color_arr[club_color];
			if(type_last != type || effect_frq_last != effect_frq)
			{
				if (effect_frq == 11)
				{
					random = Effect_Get_RandomRange_Int16(1, 10);
				}
				else
				{
					random = effect_frq;
				}
				whole_tick = club_wt[random]/(float)tick; //每循环总工作时钟节拍数
				work_tick = (club_wt[random] - Effect_Get_RandomRange_Int16(club_interval_min[random], club_interval_max[random]))/(float)tick; //每循环工作时钟节拍数;
				deal_data_update = 1;
				if(type_last != type)
				{
					Randon_Sort(sort_array ,club_random_sort, club_color);
					random_sort_index = 0;
					init_pwm = p_rgbww_pwm + club_random_sort[random_sort_index];
					
					if(Effect_Get_RandomRange_Int16(0, 9) > 5)
					{
						club_change_type = 0;
					}
					else
					{
						club_change_type = 1;
					}
					run_stage = 0;
					run_time = 0;
					deal_data_update = 0;
				}
				type_last = type;
				effect_frq_last = effect_frq;
			}
			else
			{
				if(run_stage == 0)  //阶段一，亮，两种方式
				{
					if(club_change_type == 0)  //瞬亮
					{
						res = Effect_Pwm_LinearChange(init_pwm, init_pwm, 0, work_tick/2, work_tick/2, &current_pwm, &run_time, &deal_data_update);  	
					}
					else  //渐亮
					{
						res = Effect_Pwm_LinearChange(&off_pwm, init_pwm, 0, work_tick/2, work_tick/2, &current_pwm, &run_time, &deal_data_update);
					}
					if(res) 	//该工作阶段结束，进入第二阶段
					{
						club_change_type = Effect_Get_RandomRange_Int16(0, 1);
						run_stage = 1;
					}
				}
				else  //阶段二，灭，两种方式
				{
					if(club_change_type == 0)  //瞬灭
					{
						res = Effect_Pwm_LinearChange(init_pwm, init_pwm, work_tick/2, work_tick, whole_tick, &current_pwm, &run_time, &deal_data_update);  
					}
					else
					{
						res = Effect_Pwm_LinearChange(init_pwm, &off_pwm, work_tick/2, work_tick, whole_tick, &current_pwm, &run_time, &deal_data_update); 
					}

					if(res) 	//该工作周期结束，重新设置工作时间
					{
						if (effect_frq == 11)
						{
							random = Effect_Get_RandomRange_Int16(1, 10);
						}
						else
						{
							random = effect_frq;
						}
						whole_tick = club_wt[random]/(float)tick; //每循环总工作时钟节拍
						work_tick = (club_wt[random] - Effect_Get_RandomRange_Int16(club_interval_min[random], club_interval_max[random]))/(float)tick; //每循环工作时钟节拍数;
						
						if(++random_sort_index >= club_color || club_color_last != club_color)
						{
							Randon_Sort(sort_array ,club_random_sort, club_color);
							random_sort_index = 0;
							club_color_last = club_color;
						}
						init_pwm = p_rgbww_pwm + club_random_sort[random_sort_index];
						if(Effect_Get_RandomRange_Int16(0, 9) > 5)
						{
							club_change_type = 0;
						}
						else
						{
							club_change_type = 1;
						}
						run_stage = 0;
						run_time = 0;
					}
				}
			}
			Effect_Set_PulseWight(current_pwm);		//设置定时器PWM占空比
			break;
		#endif /*ClubLights_USE*/
		#if (Paparazzi_USE == 1)
		case EffectTypePaparazzi:
			effect_frq = (p_effect_data->effect_arg.paparazzi_arg.frq <= 11) ? (p_effect_data->effect_arg.paparazzi_arg.frq) : 11;
			work_tick = 135/(float)tick; //每循环工作时钟节拍数;
			init_pwm = p_rgbww_pwm;
			if(type_last != type || effect_frq_last != effect_frq)
			{
				if (effect_frq == 11)
				{
					random = Effect_Get_RandomRange_Int16(1, 10);
				}
				else
				{
					random = effect_frq;
				}
				interval_max = paparazzi_interval_max[random] - 35;
				interval_min = paparazzi_interval_min[random] - 35;
				
				deal_data_update = 1;
				whole_tick = work_tick + Effect_Get_RandomRange_Int16(interval_min, interval_max)/(float)tick;
				if(type_last != type)
				{
					run_time = 0;
					deal_data_update = 0;
				}
				type_last = type;
				effect_frq_last = effect_frq;
			}
			else
			{
				res = Effect_Pwm_LinearChange(init_pwm, &off_pwm, 0, work_tick, whole_tick, &current_pwm, &run_time, &deal_data_update); //逐渐变灭
				if(res) 	//该工作周期结束，重新设置工作时间
				{

					if (effect_frq == 11)
					{
						random = Effect_Get_RandomRange_Int16(1, 10);
					}
					else
					{
						random = effect_frq;
					}
					interval_max = paparazzi_interval_max[random] - 35;
					interval_min = paparazzi_interval_min[random] - 35;
				
					whole_tick = work_tick + Effect_Get_RandomRange_Int16(interval_min, interval_max)/(float)tick;
					run_time = 0;
				}
			}
			Effect_Set_PulseWight(current_pwm);		//设置定时器PWM占空比
			break;
		#endif /*Paparazzi_USE*/
			
		#if (Lightning_USE == 1)
		case EffectTypeLightning:
			effect_frq = (p_effect_data->effect_arg.lightning_arg.frq <= 11) ? (p_effect_data->effect_arg.lightning_arg.frq) : 11;
			#if (LIGHTNING_SPEED_USE == 1)
			effect_speed = (p_effect_data->effect_arg.lightning_arg.speed <= 11) ? (p_effect_data->effect_arg.lightning_arg.speed) : 11;
			#endif
		
			if(type_last != type || effect_frq_last != effect_frq || trigger_type == Trigger_Once 
				|| (trigger_type_last != trigger_type && trigger_type == Trigger_Continue)
				#if (LIGHTNING_SPEED_USE == 1)
				|| effect_speed_last != effect_speed 
				#endif
				)
			{
				deal_data_update = 1;
				
				if (effect_frq == 11)//随机
				{
					random_1 = Effect_Get_RandomRange_Int16(1, 10);
				}
				else
				{
					random_1 = effect_frq;
				}
				#if (LIGHTNING_SPEED_USE == 1)
				
				if (effect_speed == 11)//随机
				{
					random_2 = Effect_Get_RandomRange_Int16(1, 10);
				}
				else
				{
					random_2 = effect_speed;
				}
				#else
				random_2 = random_1;
				#endif
				interval_max = lightning_interval[random_1][random_2];
				
				if(type_last != type || trigger_type == Trigger_Once 
					|| (trigger_type_last != trigger_type && trigger_type == Trigger_Continue))
				{
					#if (LIGHTNING_SPEED_USE == 1) //没有speed时使用frq计算频闪次数
					if(effect_speed == 11)  //随机
					{
						flash_num = Effect_Get_RandomRange_Int16(2, 11);
					}
					else
					{
						flash_num = effect_speed + 1;
					}
					#else
					if(effect_frq == 11)  //随机
					{
						flash_num = Effect_Get_RandomRange_Int16(2, 11);
					}
					else
					{
						flash_num = effect_frq + 1;
					}
					#endif
					work_interval = 196;
					work_tick = 480/(float)tick; //每循环工作时钟节拍数;
					whole_tick = work_tick + work_interval/(float)tick;
					init_pwm = p_rgbww_pwm;
					
					flash_cnt = 0;
					run_stage = 0;
					run_time = 0;
					
					if (type_last != type && trigger_type == Trigger_None)
					{
						run_stage = 2;
						current_pwm.r = 0;
						current_pwm.g = 0;
						current_pwm.b = 0;
						current_pwm.ww = 0;
						current_pwm.cw = 0;
					}
					else if(trigger_type != Trigger_Continue)
					{
						trigger_type = Trigger_None;
					}
					deal_data_update = 0;
				}
				type_last = type;
				effect_frq_last = effect_frq;
				effect_speed_last = effect_speed;
				trigger_type_last = trigger_type;
			}
			else
			{
				if(run_stage == 0)  //第一阶段，亮闪阶段
				{
					res = Effect_Pwm_LinearChange(init_pwm, &off_pwm, 0, work_tick, whole_tick, &current_pwm, &run_time, &deal_data_update); //逐渐变灭
					if(res) 	//该工作周期结束，重新设置工作时间
					{
						//跳到第二阶段
						temp_pwm.r = init_pwm->r/2;
						temp_pwm.g = init_pwm->g/2;
						temp_pwm.b = init_pwm->b/2;
						temp_pwm.ww = init_pwm->ww/2;
						temp_pwm.cw = init_pwm->cw/2;
						flash_cnt++;
						if(flash_cnt == flash_num) //最后一次频闪，进入间隔时间
						{
							work_interval = interval_max;
						}
						else
						{
							work_interval = 120;
						}
						work_tick = 30/(float)tick;
						whole_tick = work_tick + work_interval/(float)tick;
						
						run_stage = 1;
						run_time = 0;
					}
				}
				else if(run_stage == 1)     //第二阶段，频闪阶段
				{
					if (deal_data_update == 1 && flash_cnt == flash_num)
					{
						whole_tick = work_tick + interval_max/(float)tick;
					}
					res = Effect_Pwm_LinearChange(&temp_pwm, &off_pwm, 0, work_tick, whole_tick, &current_pwm, &run_time, &deal_data_update);
					if(res) 	//该工作周期结束，重新设置工作时间
					{
						flash_cnt++;
						if(flash_cnt == flash_num)
						{
							whole_tick = work_tick + interval_max/(float)tick;
						}
						else if(flash_cnt > flash_num)
						{
							flash_cnt = 0;
							run_stage = 2;
						}
						run_time = 0;
					}
				}
				else
				{
					if(trigger_type == Trigger_Continue)
					{
						//该工作周期结束，重新设置间隔时间和闪烁次数
						#if (LIGHTNING_SPEED_USE == 1) //没有speed时使用frq计算频闪次数
						if(effect_speed == 11)  //随机
						{
							flash_num = Effect_Get_RandomRange_Int16(2, 11);
						}
						else
						{
							flash_num = effect_speed + 1;
						}
						#else
						if(effect_frq == 11)  //随机
						{
							flash_num = Effect_Get_RandomRange_Int16(2, 11);
						}
						else
						{
							flash_num = effect_frq + 1;
						}
						#endif
						
						if (effect_frq == 11)//随机
						{
							random_1 = Effect_Get_RandomRange_Int16(1, 10);
						}
						else
						{
							random_1 = effect_frq;
						}
						#if (LIGHTNING_SPEED_USE == 1)
						
						if (effect_speed == 11)//随机
						{
							random_2 = Effect_Get_RandomRange_Int16(1, 10);
						}
						else
						{
							random_2 = effect_speed;
						}
						#else
						random_2 = random_1;
						#endif
						interval_max = lightning_interval[random_1][random_2];
						
						work_interval = 196;
						work_tick = 480/(float)tick; //每循环工作时钟节拍数;
						whole_tick = work_tick + work_interval/(float)tick;
						init_pwm = p_rgbww_pwm;
						run_stage = 0;
					}
					else
					{
					}
				}
			}
			Effect_Set_PulseWight(current_pwm);		//设置定时器PWM占空比
			break;
		#endif
		
		#if (TV_USE == 1)
		case EffectTypeTV:
			effect_frq = (p_effect_data->effect_arg.tv_arg.frq <= 11) ? (p_effect_data->effect_arg.tv_arg.frq) : 11;
			effect_int = p_effect_data->effect_arg.tv_arg.lightness;
			cct = p_effect_data->effect_arg.tv_arg.cct;
			if(type_last != type || effect_frq_last != effect_frq || effect_int_last != effect_int || cct_last != cct)
			{
//				float rate_temp = 0.0f;
				if (effect_frq == 11)
				{
					random = Effect_Get_RandomRange_Int16(1, 10);
				}
				else
				{
					random = effect_frq;
				}
				work_tick = Effect_Get_RandomRange_Int16(tv_wt_min[random], tv_wt_max[random])/(float)tick; 			  //设置停留时间
				whole_tick = work_tick;
				if(type_last != type || effect_int != effect_int_last || cct_last != cct)
				{
					#if (PRODUCT_FUNCTION != 0) //非单色温模式
						if(type_last != type || cct_last != cct)
						{
							if(cct == 0)
							{
								tv_random = Effect_Get_RandomRange_Int16(0, (TV_CCT_RANGE1_UP - TV_CCT_RANGE1_LOW)/100);
							}
							else if(cct == 1)
							{
								tv_random = Effect_Get_RandomRange_Int16(0, (TV_CCT_RANGE2_UP - TV_CCT_RANGE2_LOW)/100);
							}
							else
							{
								tv_random = Effect_Get_RandomRange_Int16(0, (TV_CCT_RANGE3_UP - TV_CCT_RANGE3_LOW)/100);
							}
						}
					#else //单色温模式
						tv_random = 0; //单色温模式下没有随机色温
					#endif /*PRODUCT_FUNCTION != 0*/
					
					init_pwm = &p_rgbww_pwm[tv_random];	//设置随机色温

					temp_pwm.r = init_pwm->r;
					temp_pwm.g = init_pwm->g;
					temp_pwm.b = init_pwm->b;
					temp_pwm.ww = init_pwm->ww;
					temp_pwm.cw = init_pwm->cw;

					if(type_last != type)
					{
						run_time = 0;
						deal_data_update = 0;
					}
					else
					{
						deal_data_update = 1;
					}
				}
				else
				{
					deal_data_update = 1;
				}
				type_last = type;
				effect_frq_last = effect_frq;
				effect_int_last = effect_int;
				cct_last = cct;
			}
			else
			{
				res = Effect_Pwm_LinearChange(&temp_pwm, &temp_pwm, 0, work_tick, whole_tick, &current_pwm, &run_time, &deal_data_update); //保持亮度一定时间
				if(res) //该工作周期结束，重新设置工作时间和随机色温
				{
					float rate_temp = 0.0f;
					
					#if (PRODUCT_FUNCTION != 0) //非单色温模式
						if (p_effect_data->effect_arg.tv_arg.cct == 0)
						{
							tv_random = Effect_Get_RandomRange_Int16(0, (TV_CCT_RANGE1_UP - TV_CCT_RANGE1_LOW)/100);
						}
						else if (p_effect_data->effect_arg.tv_arg.cct == 1)
						{
							tv_random = Effect_Get_RandomRange_Int16(0, (TV_CCT_RANGE2_UP - TV_CCT_RANGE2_LOW)/100);
						}
						else
						{
							tv_random = Effect_Get_RandomRange_Int16(0, (TV_CCT_RANGE3_UP - TV_CCT_RANGE3_LOW)/100);
						}
					#else //单色温模式
						tv_random = 0; //单色温模式下没有随机色温
					#endif /*PRODUCT_FUNCTION != 0*/
					
					init_pwm = &p_rgbww_pwm[tv_random];	//设置随机色温

					if (effect_frq == 11)
					{
						random = Effect_Get_RandomRange_Int16(1, 10);
					}
					else
					{
						random = effect_frq;
					}
					work_tick = Effect_Get_RandomRange_Int16(tv_wt_min[random], tv_wt_max[random])/(float)tick; 			  //设置停留时间
					whole_tick = work_tick;
					
					random = Effect_Get_RandomRange_Int16(0, 2);
					if(random == 0)  //随机亮度
					{
						rate_temp = 0.6f;
					}
					else if(random == 1)  //随机亮度
					{
						rate_temp = 0.8f;
					}
					else
					{
						rate_temp = 1.0f;
					}
					temp_pwm.r = init_pwm->r * rate_temp;
					temp_pwm.g = init_pwm->g * rate_temp;
					temp_pwm.b = init_pwm->b * rate_temp;
					temp_pwm.ww = init_pwm->ww * rate_temp;
					temp_pwm.cw = init_pwm->cw * rate_temp;
					
					run_time = 0;
				}
			}
			Effect_Set_PulseWight(current_pwm);		//设置定时器PWM占空比
			break;
		#endif /*TV_USE*/
			
		#if (Candle_USE == 1)
		case EffectTypeCandle:
			effect_frq = (p_effect_data->effect_arg.candle_arg.frq <= 11) ? (p_effect_data->effect_arg.candle_arg.frq) : 11;
            if(type_last != type)
            {
				uint16_t cct_index_max;
				
				if (p_effect_data->effect_arg.candle_arg.cct == 0)
				{
					cct_index_max = (CANDLE_CCT_RANGE1_UP - CANDLE_CCT_RANGE1_LOW)/100;
				}
				else if (p_effect_data->effect_arg.candle_arg.cct == 1)
				{
					cct_index_max = (CANDLE_CCT_RANGE2_UP - CANDLE_CCT_RANGE2_LOW)/100;
				}
				else
				{
					cct_index_max = (CANDLE_CCT_RANGE3_UP - CANDLE_CCT_RANGE3_LOW)/100;
				}
				
				min_pwm.r = p_rgbww_pwm[0].r/(float)10*7;
				min_pwm.g = p_rgbww_pwm[0].g/(float)10*7;
				min_pwm.b = p_rgbww_pwm[0].b/(float)10*7;
				min_pwm.ww = p_rgbww_pwm[0].ww/(float)10*7;
				min_pwm.cw = p_rgbww_pwm[0].cw/(float)10*7;
				
				max_pwm.r = p_rgbww_pwm[cct_index_max].r;
				max_pwm.g = p_rgbww_pwm[cct_index_max].g;
				max_pwm.b = p_rgbww_pwm[cct_index_max].b;
				max_pwm.ww = p_rgbww_pwm[cct_index_max].ww;
				max_pwm.cw = p_rgbww_pwm[cct_index_max].cw;
				
				current_pwm.r = p_rgbww_pwm[cct_index_max].r;
				current_pwm.g = p_rgbww_pwm[cct_index_max].g;
				current_pwm.b = p_rgbww_pwm[cct_index_max].b;
				current_pwm.ww = p_rgbww_pwm[cct_index_max].ww;
				current_pwm.cw = p_rgbww_pwm[cct_index_max].cw;

				if (effect_frq == 11)
				{
					random = Effect_Get_RandomRange_Int16(1, 10);
				}
				else
				{
					random = effect_frq;
				}
                work_tick = Effect_Get_RandomRange_Int16(candle_ct_min[random], candle_ct_max[random])/(float)tick;
                fire_rate = Effect_Get_RandomRange_Int16(candle_rate_min[random], candle_rate_max[random]);
                run_time = 0;
				type_last = type;
            }
            else
            {
				if (deal_data_update == 1)
				{
					uint16_t cct_index_max;
				
					if (p_effect_data->effect_arg.candle_arg.cct == 0)
					{
						cct_index_max = (CANDLE_CCT_RANGE1_UP - CANDLE_CCT_RANGE1_LOW)/100;
					}
					else if (p_effect_data->effect_arg.candle_arg.cct == 1)
					{
						cct_index_max = (CANDLE_CCT_RANGE2_UP - CANDLE_CCT_RANGE2_LOW)/100;
					}
					else
					{
						cct_index_max = (CANDLE_CCT_RANGE3_UP - CANDLE_CCT_RANGE3_LOW)/100;
					}
					min_pwm.r = p_rgbww_pwm[0].r/(float)10*7;
					min_pwm.g = p_rgbww_pwm[0].g/(float)10*7;
					min_pwm.b = p_rgbww_pwm[0].b/(float)10*7;
					min_pwm.ww = p_rgbww_pwm[0].ww/(float)10*7;
					min_pwm.cw = p_rgbww_pwm[0].cw/(float)10*7;
					
					max_pwm.r = p_rgbww_pwm[cct_index_max].r;
					max_pwm.g = p_rgbww_pwm[cct_index_max].g;
					max_pwm.b = p_rgbww_pwm[cct_index_max].b;
					max_pwm.ww = p_rgbww_pwm[cct_index_max].ww;
					max_pwm.cw = p_rgbww_pwm[cct_index_max].cw;
				}
				
				res = Fire_Pwm_Change(min_pwm, max_pwm, work_tick, fire_rate, &current_pwm, &run_time, &deal_data_update);
				if(res)
                {
					uint16_t cct_index_max;
				
					if (p_effect_data->effect_arg.candle_arg.cct == 0)
					{
						cct_index_max = (CANDLE_CCT_RANGE1_UP - CANDLE_CCT_RANGE1_LOW)/100;
					}
					else if (p_effect_data->effect_arg.candle_arg.cct == 1)
					{
						cct_index_max = (CANDLE_CCT_RANGE2_UP - CANDLE_CCT_RANGE2_LOW)/100;
					}
					else
					{
						cct_index_max = (CANDLE_CCT_RANGE3_UP - CANDLE_CCT_RANGE3_LOW)/100;
					}
					
                	min_pwm.r = p_rgbww_pwm[0].r/(float)10*7;
					min_pwm.g = p_rgbww_pwm[0].g/(float)10*7;
					min_pwm.b = p_rgbww_pwm[0].b/(float)10*7;
					min_pwm.ww = p_rgbww_pwm[0].ww/(float)10*7;
					min_pwm.cw = p_rgbww_pwm[0].cw/(float)10*7;
					
					max_pwm.r = p_rgbww_pwm[cct_index_max].r;
					max_pwm.g = p_rgbww_pwm[cct_index_max].g;
					max_pwm.b = p_rgbww_pwm[cct_index_max].b;
					max_pwm.ww = p_rgbww_pwm[cct_index_max].ww;
					max_pwm.cw = p_rgbww_pwm[cct_index_max].cw;

					if (effect_frq == 11)
					{
						random = Effect_Get_RandomRange_Int16(1, 10);
					}
					else
					{
						random = effect_frq;
					}
				
// 					if(Effect_Get_RandomRange_Int16(0, 9) < 2)
// 					{
// 						Fire_Rate = 0;
// 					}
// 					else
                    {
//                      Fire_Rate = Effect_Get_RandomRange_Int16(Fire_Rmin[Effect_Frq],Fire_Rmax[Effect_Frq]);
						fire_rate = Effect_Get_RandomRange_Int16(candle_rate_min[random], candle_rate_max[random]);
                    }
					work_tick = Effect_Get_RandomRange_Int16(candle_ct_min[random], candle_ct_max[random])/(float)tick;
                    run_time = 0;
                }
            }
            
            Effect_Set_PulseWight(current_pwm);		//设置定时器PWM占空比
			break;
		#endif /*Candle_USE*/
		
		#if (Fire_USE == 1)
		case EffectTypeFire:
			effect_frq = (p_effect_data->effect_arg.fire_arg.frq <= 11) ? (p_effect_data->effect_arg.fire_arg.frq) : 11;
            if(type_last != type)
            {
				uint16_t cct_index_max;
				
				if (p_effect_data->effect_arg.fire_arg.cct == 0)
				{
					cct_index_max = (FIRE_CCT_RANGE1_UP - FIRE_CCT_RANGE1_LOW)/100;
				}
				else if (p_effect_data->effect_arg.fire_arg.cct == 1)
				{
					cct_index_max = (FIRE_CCT_RANGE2_UP - FIRE_CCT_RANGE2_LOW)/100;
				}
				else
				{
					cct_index_max = (FIRE_CCT_RANGE3_UP - FIRE_CCT_RANGE3_LOW)/100;
				}
				
				min_pwm.r = p_rgbww_pwm[0].r/(float)10*7;
				min_pwm.g = p_rgbww_pwm[0].g/(float)10*7;
				min_pwm.b = p_rgbww_pwm[0].b/(float)10*7;
				min_pwm.ww = p_rgbww_pwm[0].ww/(float)10*7;
				min_pwm.cw = p_rgbww_pwm[0].cw/(float)10*7;
				
				max_pwm.r = p_rgbww_pwm[cct_index_max].r;
				max_pwm.g = p_rgbww_pwm[cct_index_max].g;
				max_pwm.b = p_rgbww_pwm[cct_index_max].b;
				max_pwm.ww = p_rgbww_pwm[cct_index_max].ww;
				max_pwm.cw = p_rgbww_pwm[cct_index_max].cw;
				
				current_pwm.r = p_rgbww_pwm[cct_index_max].r;
				current_pwm.g = p_rgbww_pwm[cct_index_max].g;
				current_pwm.b = p_rgbww_pwm[cct_index_max].b;
				current_pwm.ww = p_rgbww_pwm[cct_index_max].ww;
				current_pwm.cw = p_rgbww_pwm[cct_index_max].cw;

				if (effect_frq == 11)
				{
					random = Effect_Get_RandomRange_Int16(1, 10);
				}
				else
				{
					random = effect_frq;
				}
                work_tick = Effect_Get_RandomRange_Int16(fire_ct_min[random], fire_ct_max[random])/(float)tick;
                fire_rate = Effect_Get_RandomRange_Int16(fire_rate_min[random], fire_rate_max[random]);
                run_time = 0;
				type_last = type;
            }
            else
            {
				if (deal_data_update == 1)
				{
					uint16_t cct_index_max;
				
					if (p_effect_data->effect_arg.fire_arg.cct == 0)
					{
						cct_index_max = (FIRE_CCT_RANGE1_UP - FIRE_CCT_RANGE1_LOW)/100;
					}
					else if (p_effect_data->effect_arg.fire_arg.cct == 1)
					{
						cct_index_max = (FIRE_CCT_RANGE2_UP - FIRE_CCT_RANGE2_LOW)/100;
					}
					else
					{
						cct_index_max = (FIRE_CCT_RANGE3_UP - FIRE_CCT_RANGE3_LOW)/100;
					}
					
					min_pwm.r = p_rgbww_pwm[0].r/(float)10*7;
					min_pwm.g = p_rgbww_pwm[0].g/(float)10*7;
					min_pwm.b = p_rgbww_pwm[0].b/(float)10*7;
					min_pwm.ww = p_rgbww_pwm[0].ww/(float)10*7;
					min_pwm.cw = p_rgbww_pwm[0].cw/(float)10*7;
					
					max_pwm.r = p_rgbww_pwm[cct_index_max].r;
					max_pwm.g = p_rgbww_pwm[cct_index_max].g;
					max_pwm.b = p_rgbww_pwm[cct_index_max].b;
					max_pwm.ww = p_rgbww_pwm[cct_index_max].ww;
					max_pwm.cw = p_rgbww_pwm[cct_index_max].cw;
				}
				
				res = Fire_Pwm_Change(min_pwm, max_pwm, work_tick, fire_rate, &current_pwm, &run_time, &deal_data_update);
				if(res)
                {
					uint16_t cct_index_max;
				
					if (p_effect_data->effect_arg.fire_arg.cct == 0)
					{
						cct_index_max = (FIRE_CCT_RANGE1_UP - FIRE_CCT_RANGE1_LOW)/100;
					}
					else if (p_effect_data->effect_arg.fire_arg.cct == 1)
					{
						cct_index_max = (FIRE_CCT_RANGE2_UP - FIRE_CCT_RANGE2_LOW)/100;
					}
					else
					{
						cct_index_max = (FIRE_CCT_RANGE3_UP - FIRE_CCT_RANGE3_LOW)/100;
					}
					
                	min_pwm.r = p_rgbww_pwm[0].r/(float)10*7;
					min_pwm.g = p_rgbww_pwm[0].g/(float)10*7;
					min_pwm.b = p_rgbww_pwm[0].b/(float)10*7;
					min_pwm.ww = p_rgbww_pwm[0].ww/(float)10*7;
					min_pwm.cw = p_rgbww_pwm[0].cw/(float)10*7;
					
					max_pwm.r = p_rgbww_pwm[cct_index_max].r;
					max_pwm.g = p_rgbww_pwm[cct_index_max].g;
					max_pwm.b = p_rgbww_pwm[cct_index_max].b;
					max_pwm.ww = p_rgbww_pwm[cct_index_max].ww;
					max_pwm.cw = p_rgbww_pwm[cct_index_max].cw;
					if (effect_frq == 11)
					{
						random = Effect_Get_RandomRange_Int16(1, 10);
					}
					else
					{
						random = effect_frq;
					}
				
// 					if(Effect_Get_RandomRange_Int16(0, 9) < 2)
// 					{
// 						Fire_Rate = 0;
// 					}
// 					else
                    {
//                      Fire_Rate = Effect_Get_RandomRange_Int16(Fire_Rmin[Effect_Frq],Fire_Rmax[Effect_Frq]);
						fire_rate = Effect_Get_RandomRange_Int16(fire_rate_min[random], fire_rate_max[random]);
                    }
					work_tick = Effect_Get_RandomRange_Int16(fire_ct_min[random], fire_ct_max[random])/(float)tick;
                    run_time = 0;
                }
            }
            
            Effect_Set_PulseWight(current_pwm);		//设置定时器PWM占空比
			break;
		#endif /*Fire_USE*/
		
		#if (Strobe_USE == 1)
		case EffectTypeStrobe:
			effect_frq = (p_effect_data->effect_arg.strobe_arg.frq <= 11) ? (p_effect_data->effect_arg.strobe_arg.frq) : 11;
			work_tick = 30/(float)tick;     //亮的时间
			init_pwm = p_rgbww_pwm;
			
			if(type_last != type || effect_frq_last != effect_frq)
			{	
				deal_data_update = 1;
				if(effect_frq == 11)  //随机
				{
					random = Effect_Get_RandomRange_Int16(1, 10);
					interval_max = strobe_interval_max[random];
				}
				else
				{
					interval_max = strobe_interval_max[effect_frq];
				}
				whole_tick = work_tick + interval_max/(float)tick;

				if(type_last != type)
				{
					run_time = 0;
					deal_data_update = 0;
				}
				type_last = type;
				effect_frq_last = effect_frq;
			}
			else
			{
				res = Effect_Pwm_LinearChange(init_pwm, init_pwm, 0, work_tick, whole_tick, &current_pwm, &run_time, &deal_data_update); //保持亮度一定时间
				if(res) //该工作周期结束，重新设置工作时间
				{
					if(effect_frq == 11)  //随机
					{
						random = Effect_Get_RandomRange_Int16(1, 10);
						interval_max = strobe_interval_max[random];
					}
					else
					{
						interval_max = strobe_interval_max[effect_frq];
					}
					whole_tick = work_tick + interval_max/(float)tick;
					run_time = 0;
				}
			}
			Effect_Set_PulseWight(current_pwm);		//设置定时器PWM占空比
			break;
		#endif /*Strobe_USE*/
		
		#if (Explosion_USE == 1)
		case EffectTypeExplosion:
			effect_frq = (p_effect_data->effect_arg.explosion_arg.frq <= 11) ? (p_effect_data->effect_arg.explosion_arg.frq) : 11;
			init_pwm = p_rgbww_pwm;
			
			if(type_last != type || effect_frq_last != effect_frq || trigger_type == Trigger_Once)
			{
				deal_data_update = 1;

				if (effect_frq == 11)
				{
					random = Effect_Get_RandomRange_Int16(1, 10);
				}
				else
				{
					random = effect_frq;
				}
				work_tick = explosion_wt[random]/(float)tick;
				whole_tick = work_tick;
				
				if (type_last != type && trigger_type == Trigger_None)
				{
					run_stage = 1;
					current_pwm.r = 0;
					current_pwm.g = 0;
					current_pwm.b = 0;
					current_pwm.ww = 0;
					current_pwm.cw = 0;
				}
				else if(type_last != type || trigger_type == Trigger_Once)
				{
					run_stage = 0;
					run_time = 0;
					trigger_type = Trigger_None;
					deal_data_update = 0;
				}
				type_last = type;
				effect_frq_last = effect_frq;
			}
			else
			{
				if(run_stage == 0)
				{
					res = Effect_Pwm_LinearChange(init_pwm, &off_pwm, 0, work_tick, whole_tick, &current_pwm, &run_time, &deal_data_update); //逐渐变灭
					if(res) 	//该工作周期结束，重新设置工作时间
					{
						run_stage = 1;
						trigger_type = Trigger_None;
					}
				}
				else
				{
				}
			}

			Effect_Set_PulseWight(current_pwm);		//设置定时器PWM占空比
			break;
		#endif /*Explosion_USE*/
		
		#if (FaultBulb_USE == 1)
		case EffectTypeFaultBulb:
			effect_frq = (p_effect_data->effect_arg.faultBulb_arg.frq <= 11) ? (p_effect_data->effect_arg.faultBulb_arg.frq) : 11;
			#if (FAULTY_SPEED_USE == 1)
			effect_speed = (p_effect_data->effect_arg.faultBulb_arg.speed <= 11) ? (p_effect_data->effect_arg.faultBulb_arg.speed) : 11;
			#endif
			init_pwm = p_rgbww_pwm;
			
			if(type_last != type || effect_frq_last != effect_frq
				#if (LIGHTNING_SPEED_USE == 1)
				|| effect_speed_last != effect_speed 
				#endif
			)
			{	
				deal_data_update = 1;
				
				if (effect_frq == 11)//随机
				{
					random_1 = Effect_Get_RandomRange_Int16(1, 10);
				}
				else
				{
					random_1 = effect_frq;
				}	
				
				#if (LIGHTNING_SPEED_USE == 1)
				
				if (effect_speed == 11)//随机
				{
					random_2 = Effect_Get_RandomRange_Int16(1, 10);
				}
				else
				{
					random_2 = effect_speed;
				}
				
				work_tick = Effect_Get_RandomRange_Int16(fault_work_min[random_2], fault_work_max[random_2])/(float)tick;

				#else
				work_tick = Effect_Get_RandomRange_Int16(fault_work_min[random_1], fault_work_max[random_1])/(float)tick;
				#endif
				
				whole_tick = work_tick;

				if(type_last != type)
				{
					run_stage = 0;
					run_time = 0;
					deal_data_update = 0;
				}
				type_last = type;
				effect_frq_last = effect_frq;
				effect_speed_last = effect_speed;
			}
			else
			{
				if(run_stage == 0)
				{
					res = Effect_Pwm_LinearChange(init_pwm, init_pwm, 0, work_tick, whole_tick, &current_pwm, &run_time, &deal_data_update);
					if(res) 	//该工作周期结束，重新设置工作时间
					{
						flash_cnt = 0;
						
						#if (LIGHTNING_SPEED_USE == 1)
						if(Effect_Get_RandomRange_Int16(1, 1000) <= fault_pr[effect_speed])  //小于fault_pr[effect_speed]则单闪
						#else
						if(Effect_Get_RandomRange_Int16(1, 1000) <= fault_pr[effect_frq])  //小于fault_pr[effect_frq]则单闪
						#endif
						{
							flash_num = 1;
							#if (LIGHTNING_SPEED_USE == 1)
							if(Effect_Get_RandomRange_Int16(1, 1000) < fault_r[effect_speed])
							#else
							if(Effect_Get_RandomRange_Int16(1, 1000) < fault_r[effect_frq])
							#endif
							{
								#if (LIGHTNING_SPEED_USE == 1)
								if(effect_speed == 11)  //随机
								#else
								if(effect_frq == 11)  //随机
								#endif
								{
									random = Effect_Get_RandomRange_Int16(1, 10);
									work_tick = Effect_Get_RandomRange_Int16(fault_rt_min[random], fault_rt_max[random])/(float)tick;
								}
								else
								{
									#if (LIGHTNING_SPEED_USE == 1)
									work_tick = Effect_Get_RandomRange_Int16(fault_rt_min[effect_speed], fault_rt_max[effect_speed])/(float)tick;
									#else
									work_tick = Effect_Get_RandomRange_Int16(fault_rt_min[effect_frq], fault_rt_max[effect_frq])/(float)tick;
									#endif
								}
							}
							else
							{
								#if (LIGHTNING_SPEED_USE == 1)
								if(effect_speed == 11)  //随机
								#else
								if(effect_frq == 11)  //随机
								#endif
								{
									random = Effect_Get_RandomRange_Int16(1, 11);
									work_tick = Effect_Get_RandomRange_Int16(fault_work_min[random], fault_work_max[random])/(float)tick;
								}
								else
								{
									#if (LIGHTNING_SPEED_USE == 1)
									work_tick = Effect_Get_RandomRange_Int16(fault_work_min[effect_speed], fault_work_max[effect_speed])/(float)tick;
									#else
									work_tick = Effect_Get_RandomRange_Int16(fault_work_min[effect_frq], fault_work_max[effect_frq])/(float)tick;
									#endif
								}
							}
							#if (LIGHTNING_SPEED_USE == 1)
							if (effect_speed == 11)
							{
								random = Effect_Get_RandomRange_Int16(1, 11);
							}
							else
							{
								random = effect_speed;
							}
							interval_max = fault_interval_max[random];
							#else
							interval_max = 150;
							#endif
							whole_tick = work_tick + interval_max/(float)tick;
						}
						else	//概率密集闪，随机2-32次, 2.54-4s,每次100ms
						{
							flash_num = Effect_Get_RandomRange_Int16(2, 32);
							interval_max = 50;
							work_tick = 50/(float)tick;
							whole_tick = work_tick + interval_max/(float)tick;
						}
						run_stage = 1;
						run_time = 0;
					}
				}
				else
				{
					res = Effect_Pwm_LinearChange(init_pwm, init_pwm, 0, work_tick, whole_tick, &current_pwm, &run_time, &deal_data_update);
					if(res) 	//该工作周期结束，重新设置工作时间
					{
						flash_cnt++;
						if(flash_cnt >= flash_num)
						{
							if(Effect_Get_RandomRange_Int16(1, 1000) < fault_r[effect_frq])
							{
								#if (LIGHTNING_SPEED_USE == 1)
								if(effect_speed == 11)  //随机
								#else
								if(effect_frq == 11)  //随机
								#endif
								{
									random = Effect_Get_RandomRange_Int16(1, 10);
									work_tick = Effect_Get_RandomRange_Int16(fault_rt_min[random], fault_rt_max[random])/(float)tick;
								}
								else
								{
									#if (LIGHTNING_SPEED_USE == 1)
									work_tick = Effect_Get_RandomRange_Int16(fault_rt_min[effect_speed], fault_rt_max[effect_speed])/(float)tick;
									#else
									work_tick = Effect_Get_RandomRange_Int16(fault_rt_min[effect_frq], fault_rt_max[effect_frq])/(float)tick;
									#endif
								}
							}
							else
							{
								#if (LIGHTNING_SPEED_USE == 1)
								if(effect_speed == 11)  //随机
								#else
								if(effect_frq == 11)  //随机
								#endif
								{
									random = Effect_Get_RandomRange_Int16(1, 10);
									work_tick = Effect_Get_RandomRange_Int16(fault_work_min[random], fault_work_max[random])/(float)tick;
								}
								else
								{
									#if (LIGHTNING_SPEED_USE == 1)
									work_tick = Effect_Get_RandomRange_Int16(fault_work_min[effect_speed], fault_work_max[effect_speed])/(float)tick;
									#else
									work_tick = Effect_Get_RandomRange_Int16(fault_work_min[effect_frq], fault_work_max[effect_frq])/(float)tick;
									#endif
								}
							}
							whole_tick = work_tick + interval_max/(float)tick;
							flash_cnt = 0;
							run_stage = 0;
						}
						run_time = 0;
					}
				}
			}
			Effect_Set_PulseWight(current_pwm);		//设置定时器PWM占空比
			break;
		#endif /*FaultBulb_USE*/
		
		#if (Pulsing_USE == 1)
		case EffectTypePulsing:
			effect_frq = (p_effect_data->effect_arg.pulsing_arg.frq <= 11) ? (p_effect_data->effect_arg.pulsing_arg.frq) : 11;
			#if (PULSING_SPEED_USE == 1)
			effect_speed = (p_effect_data->effect_arg.pulsing_arg.speed <= 11) ? (p_effect_data->effect_arg.pulsing_arg.speed) : 11;
			#endif
			init_pwm = p_rgbww_pwm;
	
			if(type_last != type || effect_frq_last != effect_frq
				#if (PULSING_SPEED_USE == 1)
				|| effect_speed != effect_speed_last
				#endif
			
			)
			{
				deal_data_update = 1;

				if (effect_frq == 11)  //随机
				{
					random_1 = Effect_Get_RandomRange_Int16(1, 10);
				}
				else
				{
					random_1 = effect_frq;
				}
				
				#if (PULSING_SPEED_USE == 1)
				if (effect_speed == 11)  //随机
				{
					random_2 = Effect_Get_RandomRange_Int16(1, 10);
				}
				else
				{
					random_2 = effect_speed;
				}
				#else
				random_2 = random_1;
				#endif
				interval_max = pluse_interval[random_1][random_2];
				
				if (effect_frq == 11)  //随机
				{
					random_1 = Effect_Get_RandomRange_Int16(1, 10);
				}
				else
				{
					random_1 = effect_frq;
				}
				
				#if (PULSING_SPEED_USE == 1)
				if (effect_speed == 11)  //随机
				{
					random_2 = Effect_Get_RandomRange_Int16(1, 10);
				}
				else
				{
					random_2 = effect_speed;
				}
				#else
				random_2 = random_1;
				#endif
				work_tick = pluse_worktime[random_1][random_2]/(float)tick;
				
				whole_tick = work_tick + interval_max/(float)tick;

				if(type_last != type)
				{
					run_stage = 0;
					run_time = 0;
					deal_data_update = 0;
				}
				type_last = type;
				effect_frq_last = effect_frq;
				effect_speed_last = effect_speed;
			}
			else
			{
				if(run_stage == 0) //第一阶段，逐渐增加亮度
				{
					res = Effect_Pwm_LinearChange(&off_pwm, init_pwm, 0, work_tick/2, work_tick/2, &current_pwm, &run_time, &deal_data_update); //逐渐亮
					if(res) 	//该工作周期结束，重新设置工作时间
					{
						run_stage = 1;
					}
				}
				else  //第二阶段，逐渐减小亮度
				{
					res = Effect_Pwm_LinearChange(init_pwm, &off_pwm, work_tick/2, work_tick, whole_tick, &current_pwm, &run_time, &deal_data_update); //逐渐灭
					if(res) 	//该工作周期结束，重新设置工作时间
					{
						if (effect_frq == 11)  //随机
						{
							random_1 = Effect_Get_RandomRange_Int16(1, 10);
						}
						else
						{
							random_1 = effect_frq;
						}
						
						#if (PULSING_SPEED_USE == 1)
						if (effect_speed == 11)  //随机
						{
							random_2 = Effect_Get_RandomRange_Int16(1, 10);
						}
						else
						{
							random_2 = effect_speed;
						}
						#else
						random_2 = random_1;
						#endif
						interval_max = pluse_interval[random_1][random_2];
						
						if (effect_frq == 11)  //随机
						{
							random_1 = Effect_Get_RandomRange_Int16(1, 10);
						}
						else
						{
							random_1 = effect_frq;
						}
						
						#if (PULSING_SPEED_USE == 1)
						if (effect_speed == 11)  //随机
						{
							random_2 = Effect_Get_RandomRange_Int16(1, 10);
						}
						else
						{
							random_2 = effect_speed;
						}
						#else
						random_2 = random_1;
						#endif
						work_tick = pluse_worktime[random_1][random_2]/(float)tick;
						
						whole_tick = work_tick + interval_max/(float)tick;

						run_stage = 0;
						run_time = 0;
					}
				}
			}
			Effect_Set_PulseWight(current_pwm);		//设置定时器PWM占空比
			break;
		#endif /*Pulsing_USE*/
		
		#if (Welding_USE == 1)
		case EffectTypeWelding:
			effect_frq = (p_effect_data->effect_arg.welding_arg.frq <= 11) ? (p_effect_data->effect_arg.welding_arg.frq) : 11;
			welding_min = p_effect_data->effect_arg.welding_arg.min;
			if(type_last != type || effect_frq_last != effect_frq)
			{
				deal_data_update = 1;
				
				work_tick = welding_wt/(float)tick;//Effect_Get_RandomRange_Int16(Welding_WTMin[Effect_Frq], Welding_WTMax[Effect_Frq]);	//第一阶段，计算随机高亮工作时间
				whole_tick = work_tick;
				if(type_last != type)
				{
					/*设置第一阶段pwm值和工作时间*/
					max_pwm.r = p_rgbww_pwm->r;
					max_pwm.g = p_rgbww_pwm->g;
					max_pwm.b = p_rgbww_pwm->b;
					max_pwm.ww = p_rgbww_pwm->ww;
					max_pwm.cw = p_rgbww_pwm->cw;
					run_stage = 0;
					run_time = 0;
					deal_data_update = 0;
				}
				type_last = type;
				effect_frq_last = effect_frq;
			}
			else
			{
				if(run_stage == 0)  //第一阶段，高亮阶段，保持高亮状态
				{
					res = Effect_Pwm_LinearChange(&max_pwm, &max_pwm, 0, work_tick, whole_tick, &current_pwm, &run_time, &deal_data_update);
					if(res) 	//该工作周期结束，设置第二阶段pwm值和工作时间，进入第二阶段
					{
						min_pwm.r = p_rgbww_pwm->r * 0.8f * 0.001f * welding_min;
						min_pwm.g = p_rgbww_pwm->g * 0.8f * 0.001f * welding_min;
						min_pwm.b = p_rgbww_pwm->b * 0.8f * 0.001f * welding_min;
						min_pwm.ww = p_rgbww_pwm->ww * 0.8f * 0.001f * welding_min;
						min_pwm.cw = p_rgbww_pwm->cw * 0.8f * 0.001f * welding_min;

						if (effect_frq == 11)
						{
							random = Effect_Get_RandomRange_Int16(1, 10);
						}
						else
						{
							random = effect_frq;
						}
						if(Effect_Get_RandomRange_Int16(1, 1000) < welding_r[random])
						{
							work_tick = Effect_Get_RandomRange_Int16(welding_rt_min[random], welding_rt_max[random])/(float)tick;
						}
						else
						{
							work_tick = Effect_Get_RandomRange_Int16(welding_interval_min[random], welding_interval_max[random])/(float)tick;
						}
						whole_tick = work_tick;
						run_time = 0;
						run_stage = 1;
					}
				}
				else   //第二阶段，低亮阶段
				{
					res = Effect_Pwm_LinearChange(&min_pwm, &min_pwm, 0, work_tick, whole_tick, &current_pwm, &run_time, &deal_data_update);
					if(res) 	//该工作周期结束，设置第一阶段pwm值和工作时间，进入第一阶段
					{
						max_pwm.r = p_rgbww_pwm->r;
						max_pwm.g = p_rgbww_pwm->g;
						max_pwm.b = p_rgbww_pwm->b;
						max_pwm.ww = p_rgbww_pwm->ww;
						max_pwm.cw = p_rgbww_pwm->cw;
						work_tick = welding_wt/(float)tick;//Effect_Get_RandomRange_Int16(Welding_WTMin[Effect_Frq], Welding_WTMax[Effect_Frq]);	//第一阶段，计算随机高亮工作时间
						whole_tick = work_tick;
						run_time = 0;
						run_stage = 0;
					}
				}
			}
			Effect_Set_PulseWight(current_pwm);		//设置定时器PWM占空比
			break;
		#endif /*Welding_USE*/
		
		#if (CopCar_USE == 1)
		case EffectTypeCopCar:
			cop_car_type = (p_effect_data->effect_arg.cop_car_arg.color < 5)? (CopCar_Type)p_effect_data->effect_arg.cop_car_arg.color : CopCar_Color_RBW;
			effect_frq = (p_effect_data->effect_arg.cop_car_arg.frq <= 10) ? (p_effect_data->effect_arg.cop_car_arg.frq) : 10;
			effect_frq = (effect_frq == 0)? 1 : effect_frq;
			
			if (type_last != type)
			{
				if (cop_car_type == CopCar_Color_RBW)
				{
					cop_car_arg.area_num = 3;
				}
				else if (cop_car_type == CopCar_Color_BW || cop_car_type == CopCar_Color_RB)
				{
					cop_car_arg.area_num = 2;
				}
				else
				{
					cop_car_arg.area_num = 1;
				}

				if (effect_frq < 7)
				{
					cop_car_index = effect_frq - 1;
				}
				else
				{
					cop_car_index = 0;
				}
				for(index = 0; index < cop_car_arg.area_num; index++)
				{
					cop_car_arg.item[index].flash_num = CopCar_FlashTimers[cop_car_index];
					cop_car_arg.item[index].work_tick = CopCar_WT[cop_car_index]/(float)tick;
					if(effect_frq == 6)
					{
						cop_car_arg.item[index].pwm = p_rgbww_pwm + cop_car_arg.area_num;
					}
					else
					{
						cop_car_arg.item[index].pwm = p_rgbww_pwm + index;
					}
				}
				area_num = 0;
				flash_cnt = 0;
				run_time = 0;
				type_last = type;
			}
			else
			{			
				if (area_num < cop_car_arg.area_num)
				{
					if (flash_cnt == (cop_car_arg.item[area_num].flash_num - 1))
					{
						whole_tick = cop_car_arg.item[area_num].work_tick + CopCar_CInterval[cop_car_index]/(float)tick;
						res = Effect_Pwm_LinearChange(cop_car_arg.item[area_num].pwm, cop_car_arg.item[area_num].pwm, 0, cop_car_arg.item[area_num].work_tick,
															whole_tick, &current_pwm, &run_time, &deal_data_update);
						if(res) 	//该工作周期结束，重新设置工作时间
						{
							run_time = 0; 
							area_num++;
							flash_cnt = 0;
						}
					}
					else if(flash_cnt < (cop_car_arg.item[area_num].flash_num - 1))
					{
						whole_tick = cop_car_arg.item[area_num].work_tick + CopCar_WT[cop_car_index]/(float)tick;
						res = Effect_Pwm_LinearChange(cop_car_arg.item[area_num].pwm, cop_car_arg.item[area_num].pwm, 0, cop_car_arg.item[area_num].work_tick,
															whole_tick, &current_pwm, &run_time, &deal_data_update);
						if(res) 	//该工作周期结束，重新设置工作时间
						{
							run_time = 0; 
							flash_cnt++;
						}
					}
					else
					{
						run_time = 0; 
						area_num = 0;
						flash_cnt = 0;
					}
				}
				else
				{
					/****************重新初始化数据******************/
					if (cop_car_type == CopCar_Color_RBW)
					{
						cop_car_arg.area_num = 3;
					}
					else if (cop_car_type == CopCar_Color_BW || cop_car_type == CopCar_Color_RB)
					{
						cop_car_arg.area_num = 2;
					}
					else
					{
						cop_car_arg.area_num = 1;
					}
					
					if(effect_frq < 7)
					{
						cop_car_index = effect_frq - 1;
					}
					else
					{
						cop_car_index++;
						if(effect_frq == 7)
						{
							if(cop_car_index >= 2)
							{
								cop_car_index = 0;
							}
						}
						else if(effect_frq == 8)
						{
							if(cop_car_index >= 3)
							{
								cop_car_index = 0;
							}
						}
						else if(effect_frq == 9)
						{
							if(cop_car_index >= 4)
							{
								cop_car_index = 0;
							}
						}
						else
						{
							if(cop_car_index >= 5)
							{
								cop_car_index = 0;
							}
						}
					}
					for(index = 0; index < cop_car_arg.area_num; index++)
					{
						cop_car_arg.item[index].flash_num = CopCar_FlashTimers[cop_car_index];
						cop_car_arg.item[index].work_tick = CopCar_WT[cop_car_index]/(float)tick;
						if(effect_frq == 6)
						{
							cop_car_arg.item[index].pwm = p_rgbww_pwm + cop_car_arg.area_num;
						}
						else
						{
							cop_car_arg.item[index].pwm = p_rgbww_pwm + index;
						}
					}
					area_num = 0;
					flash_cnt = 0;
					run_time = 0;
				}
			}
			Effect_Set_PulseWight(current_pwm);		//设置定时器PWM占空比
			break;
		#endif /*CopCar_USE*/
		
		#if (ColorChase_USE == 1)
		case EffectTypeColorChase:
			effect_frq = (p_effect_data->effect_arg.color_chase_arg.frq <= 11) ? (p_effect_data->effect_arg.color_chase_arg.frq) : 11;
			sat = p_effect_data->effect_arg.color_chase_arg.sat;
			init_pwm = p_rgbww_pwm;
			if(type_last != type || effect_frq_last != effect_frq || sat_last != sat)
			{
				init_pwm = p_rgbww_pwm;
				deal_data_update = 1;
				if(sat == 0)  //饱和度为0则只变化CCT, CCT_MIN - CCT_MAX
				{
					index_begin = 0;
					index_end = (EFFECT_CCT_MAX - EFFECT_CCT_MIN)/100;
				}
				else  //hue从1-360变化
				{
					index_begin = 1;
					index_end = 360;
				}
				if (effect_frq == 11)
				{
					random = Effect_Get_RandomRange_Int16(1, 10);
				}
				else
				{
					random = effect_frq;
				}
				work_tick = color_chase_ct[random]/(float)tick;;
				whole_tick = work_tick;

				if(type_last != type)// || sat_last != sat)
				{
					run_time = 0;
					deal_data_update = 0;
				}
				type_last = type;
				effect_frq_last = effect_frq;
				sat_last = sat;
			}
			else
			{
				res = Effect_Pwm_LinearChange_1(index_begin, index_end, 0, work_tick, whole_tick, init_pwm, &current_pwm, &run_time, &deal_data_update); //逐渐增加CCT或hue
				if(res)  
				{
					if(sat == 0)  //饱和度为0则只变化CCT,EFFECT_CCT_MIN-EFFECT_CCT_MAX EFFECT_CCT_MAX-EFFECT_CCT_MIN,循环变化
					{
						if(index_begin == 0)
						{
							index_begin = (EFFECT_CCT_MAX - EFFECT_CCT_MIN)/100;
							index_end = 0;
						}
						else
						{
							index_begin = 0;
							index_end = (EFFECT_CCT_MAX - EFFECT_CCT_MIN)/100;
						}
					}
					else  //hue从1-360变化
					{
						index_begin = 1;
						index_end = 360;
					}
					init_pwm = p_rgbww_pwm;
					if (effect_frq == 11)
					{
						random = Effect_Get_RandomRange_Int16(1, 10);
					}
					else
					{
						random = effect_frq;
					}
					work_tick = color_chase_ct[random]/(float)tick;;
					whole_tick = work_tick;
					run_time = 0;
				}
			}
			Effect_Set_PulseWight(current_pwm);		//设置定时器PWM占空比
			break;
		#endif /*ColorChase_USE*/
		
		#if (PartyLights_USE == 1)
		case EffectTypePartyLights:
			effect_frq = (p_effect_data->effect_arg.party_lights_arg.frq <= 11) ? (p_effect_data->effect_arg.party_lights_arg.frq) : 11;
			sat = p_effect_data->effect_arg.party_lights_arg.sat;
			init_pwm = p_rgbww_pwm;
			if(type_last != type || effect_frq_last != effect_frq || sat_last != sat)
			{
				init_pwm = p_rgbww_pwm;
				deal_data_update = 1;
				if(p_effect_data->effect_arg.party_lights_arg.sat == 0)  //饱和度为0则只变化CCT, CCT_MIN - CCT_MAX
				{
					index_begin = 0;
					index_end = (EFFECT_CCT_MAX - EFFECT_CCT_MIN)/100;
				}
				else  //hue从1-360变化
				{
					index_begin = 1;
					index_end = 360;
				}
				if (effect_frq == 11)
				{
					random = Effect_Get_RandomRange_Int16(1, 10);
				}
				else
				{
					random = effect_frq;
				}
				work_tick = party_lights_ct[random]/(float)tick;;
				whole_tick = work_tick;
				
				if(type_last != type)// || sat_last != sat)
				{
					run_time = 0;
					deal_data_update = 0;
				}
				type_last = type;
				effect_frq_last = effect_frq;
				sat_last = sat;
			}
			else
			{
				res = Effect_Pwm_LinearChange_1(index_begin, index_end, 0, work_tick, whole_tick, init_pwm, &current_pwm, &run_time, &deal_data_update); //逐渐增加CCT或hue
				if(res)  
				{
					if(p_effect_data->effect_arg.party_lights_arg.sat == 0)  //饱和度为0则只变化CCT,2800k-10000k 10000k-2800k,循环变化
					{
						if(index_begin == 0)
						{
							index_begin = (EFFECT_CCT_MAX - EFFECT_CCT_MIN)/100;
							index_end = 0;
						}
						else
						{
							index_begin = 0;
							index_end = (EFFECT_CCT_MAX - EFFECT_CCT_MIN)/100;
						}
						if (effect_frq == 11)
						{
							random = Effect_Get_RandomRange_Int16(1, 10);
						}
						else
						{
							random = effect_frq;
						}
						work_tick = party_lights_ct[random]/(float)tick;
						whole_tick = work_tick;
					}
					else  
					{
						if(index_begin == 1)
						{
							index_begin = 360;
							index_end = 1;
						}
						else
						{
							index_begin = 1;
							index_end = 360;
						}
						if (effect_frq == 11)
						{
							random = Effect_Get_RandomRange_Int16(1, 10);
						}
						else
						{
							random = effect_frq;
						}
						work_tick = party_lights_wt[random]/(float)tick;
						whole_tick = work_tick;
					}
					init_pwm = p_rgbww_pwm;
					run_time = 0;
				}
			}
			Effect_Set_PulseWight(current_pwm);		//设置定时器PWM占空比
			break;
		#endif /*PartyLights_USE*/
		
		#if (Fireworks_USE == 1)
		case EffectTypeFireworks:
			effect_frq = (p_effect_data->effect_arg.fireworks_arg.frq <= 11) ? (p_effect_data->effect_arg.fireworks_arg.frq) : 11;
			firework_type = p_effect_data->effect_arg.fireworks_arg.type;
			work_tick = 1700/(float)tick;
			if(type_last != type || effect_frq_last != effect_frq)
			{
				deal_data_update = 1;
				
				whole_tick = work_tick + Effect_Get_RandomRange_Int16(firework_interval_min[effect_frq], firework_interval_max[effect_frq])/(float)tick;

				if(type_last != type)
				{
					#if (PRODUCT_FUNCTION == 2 || PRODUCT_FUNCTION == 3) //双色温简单彩色和全功能彩色
					if(firework_type == 0)  //CT
					{
						random = Effect_Get_RandomRange_Int16(361, 361 + (EFFECT_CCT_MAX - EFFECT_CCT_MIN)/100);
					}
					else if(firework_type == 1)  //HUE
					{
						random = Effect_Get_RandomRange_Int16(1, 360);
					}
					else  //CT+HUE
					{
						 random = Effect_Get_RandomRange_Int16(1, 361 + (EFFECT_CCT_MAX - EFFECT_CCT_MIN)/100);
					}
					#elif (PRODUCT_FUNCTION == 1)  //双色温
					if (firework_type == 0)
					{
						if (EFFECT_CCT_MAX >= 3200 && EFFECT_CCT_MIN <= 3200)
						{
							random = (3200 - EFFECT_CCT_MIN)/100;
						}
						else
						{
							random = 0;
						}
					}
					else if (firework_type == 1)
					{
						if (EFFECT_CCT_MAX >= 5600 && EFFECT_CCT_MIN <= 5600)
						{
							random = (5600 - EFFECT_CCT_MIN)/100;
						}
						else
						{
							random = (EFFECT_CCT_MAX - EFFECT_CCT_MIN)/100;
						}
					}
					else
					{
						random = Effect_Get_RandomRange_Int16(0, (EFFECT_CCT_MAX - EFFECT_CCT_MIN)/100);
					}
					#else
					random = 0;	
					#endif
					run_stage = 0;
					run_time = 0;
					deal_data_update = 0;
				}
				init_pwm = p_rgbww_pwm + random;  //随机
				type_last = type;
				effect_frq_last = effect_frq;
			}
			else
			{
				if(run_stage == 0)  //无连闪
				{
					res = Effect_Pwm_LinearChange(init_pwm, &off_pwm, 0, work_tick, whole_tick, &current_pwm, &run_time, &deal_data_update); //逐渐灭
					if(res) //该工作周期结束，重新设置工作时间
					{
						#if (PRODUCT_FUNCTION == 2 || PRODUCT_FUNCTION == 3) //双色温简单彩色和全功能彩色
						if(firework_type == 0)  //CT
						{
							random = Effect_Get_RandomRange_Int16(361, 361 + (EFFECT_CCT_MAX - EFFECT_CCT_MIN)/100);
						}
						else if(firework_type == 1)  //HUE
						{
							random = Effect_Get_RandomRange_Int16(1, 360);
						}
						else  //CT+HUE
						{
							 random = Effect_Get_RandomRange_Int16(1, 361 + (EFFECT_CCT_MAX - EFFECT_CCT_MIN)/100);
						}
						#elif (PRODUCT_FUNCTION == 1)  //双色温
						if (firework_type == 0)
						{
							if (EFFECT_CCT_MAX >= 3200 && EFFECT_CCT_MIN <= 3200)
							{
								random = (3200 - EFFECT_CCT_MIN)/100;
							}
							else
							{
								random = 0;
							}
						}
						else if (firework_type == 1)
						{
							if (EFFECT_CCT_MAX >= 5600 && EFFECT_CCT_MIN <= 5600)
							{
								random = (5600 - EFFECT_CCT_MIN)/100;
							}
							else
							{
								random = (EFFECT_CCT_MAX - EFFECT_CCT_MIN)/100;
							}
						}
						else
						{
							random = Effect_Get_RandomRange_Int16(0, (EFFECT_CCT_MAX - EFFECT_CCT_MIN)/100);
						}
						//random = Effect_Get_RandomRange_Int16(0, (EFFECT_CCT_MAX - EFFECT_CCT_MIN)/100);
						#else //单色温
						random = 0;	
						#endif
						init_pwm = p_rgbww_pwm + random;  //随机
						
						flash_num = Effect_Get_RandomRange_Int16(fireware_flash_min[effect_frq], fireware_flash_max[effect_frq]);
						if(flash_num != 0)  //连闪次数不为0，则进入连闪阶段
						{
							run_stage = 1;
							flash_cnt = 0;
							whole_tick = Effect_Get_RandomRange_Int16(850, 1530)/(float)tick;//(1300, 1900);  //10%-70%
						}
						else
						{
							whole_tick = work_tick + Effect_Get_RandomRange_Int16(firework_interval_min[effect_frq], firework_interval_max[effect_frq])/(float)tick;
							run_stage = 0;
						}
						run_time = 0;
					}	
				}
				else  //连闪阶段
				{
					res = Effect_Pwm_LinearChange(init_pwm, &off_pwm, 0, work_tick, whole_tick, &current_pwm, &run_time, &deal_data_update); //逐渐灭
					if(res) //该工作周期结束，重新设置工作时间
					{
						#if (PRODUCT_FUNCTION == 2 || PRODUCT_FUNCTION == 3) //双色温简单彩色和全功能彩色
						if(firework_type == 0)  //CT
						{
							random = Effect_Get_RandomRange_Int16(361, 361 + (EFFECT_CCT_MAX - EFFECT_CCT_MIN)/100);
						}
						else if(firework_type == 1)  //HUE
						{
							random = Effect_Get_RandomRange_Int16(1, 360);
						}
						else  //CT+HUE
						{
							 random = Effect_Get_RandomRange_Int16(1, 361 + (EFFECT_CCT_MAX - EFFECT_CCT_MIN)/100);
						}
						#elif (PRODUCT_FUNCTION == 1)  //双色温
						if (firework_type == 0)
						{
							if (EFFECT_CCT_MAX >= 3200 && EFFECT_CCT_MIN <= 3200)
							{
								random = (3200 - EFFECT_CCT_MIN)/100;
							}
							else
							{
								random = 0;
							}
						}
						else if (firework_type == 1)
						{
							if (EFFECT_CCT_MAX >= 5600 && EFFECT_CCT_MIN <= 5600)
							{
								random = (5600 - EFFECT_CCT_MIN)/100;
							}
							else
							{
								random = (EFFECT_CCT_MAX - EFFECT_CCT_MIN)/100;
							}
						}
						else
						{
							random = Effect_Get_RandomRange_Int16(0, (EFFECT_CCT_MAX - EFFECT_CCT_MIN)/100);
						}
						//random = Effect_Get_RandomRange_Int16(0, (EFFECT_CCT_MAX - EFFECT_CCT_MIN)/100);
						#else //单色温
						random = 0;	
						#endif
						init_pwm = p_rgbww_pwm + random;  //随机
						flash_cnt++;
						if(flash_cnt >= flash_num)  //结束连闪
						{
							whole_tick = work_tick + Effect_Get_RandomRange_Int16(firework_interval_min[effect_frq], firework_interval_max[effect_frq])/(float)tick;
							run_stage = 0;
						}
						else  //继续连闪
						{
							whole_tick = Effect_Get_RandomRange_Int16(850, 1530)/(float)tick;//(1300, 1900);  //10%-70%
						}
						run_time = 0;
					}
				}
			}
			Effect_Set_PulseWight(current_pwm);		//设置定时器PWM占空比
			break;
		#endif /*PartyLights_USE*/
		case EffectTypeEffectOff:
			if (type_last != type)
			{
				type_last = type;
				current_pwm.r = 0;
				current_pwm.g = 0;
				current_pwm.b = 0;
				current_pwm.ww = 0;
				current_pwm.cw = 0;
				Effect_Set_PulseWight(current_pwm);		//设置定时器PWM占空比
			}
			break;
		#if (IamHere_USE == 1)
		case EffectTypeIAmHere:
			init_pwm = p_rgbww_pwm;
			if (type_last != type || trigger_type == Trigger_Once)
			{
				iam_here_status = 0;
				work_tick = 150/(float)tick;
				whole_tick = 300/(float)tick;
				run_time = 0;
				run_stage = 0;
				flash_num = 0;
				flash_cnt = 0;
				deal_data_update = 0;
				type_last = type;
				trigger_type = Trigger_None;
			}
			else
			{
				if (run_stage == 0) //三次短闪，持续150ms，间隔150ms
				{
					res = Effect_Pwm_LinearChange(init_pwm, init_pwm, 0, work_tick, whole_tick, &current_pwm, &run_time, &deal_data_update); //逐渐变灭
					if(res) 	//该工作周期结束，重新设置工作时间
					{
						if (++flash_cnt < 3)
						{
							work_tick = 150/(float)tick;
							whole_tick = 300/(float)tick;
						}
						else
						{
							work_tick = 150/(float)tick;
							whole_tick = 600/(float)tick;
							flash_cnt = 0;
							run_stage = 1;
						}
						run_time = 0;
					}
				}
				else if (run_stage == 1) //两次短闪，持续150ms，间隔450ms
				{
					res = Effect_Pwm_LinearChange(init_pwm, init_pwm, 0, work_tick, whole_tick, &current_pwm, &run_time, &deal_data_update); //逐渐变灭
					if(res) 	//该工作周期结束，重新设置工作时间
					{
						if (++flash_cnt < 2)
						{
							work_tick = 150/(float)tick;
							whole_tick = 600/(float)tick;
						}
						else
						{
							work_tick = 150/(float)tick;
							whole_tick = 300/(float)tick;
							flash_cnt = 0;
							run_stage = 2;
						}
						run_time = 0;
					}
				}
				else if (run_stage == 2) //1次短闪，持续150ms，间隔150ms
				{
					res = Effect_Pwm_LinearChange(init_pwm, init_pwm, 0, work_tick, whole_tick, &current_pwm, &run_time, &deal_data_update); //逐渐变灭
					if (res) 	//该工作周期结束，重新设置工作时间
					{
						work_tick = 450/(float)tick;
						whole_tick = 900/(float)tick;
						flash_cnt = 0;
						run_stage = 3;
						run_time = 0;
					}
				}
				else if (run_stage == 3) //1次长闪，持续450ms，间隔450ms
				{
					res = Effect_Pwm_LinearChange(init_pwm, init_pwm, 0, work_tick, whole_tick, &current_pwm, &run_time, &deal_data_update); //逐渐变灭
					if (res) 	//该工作周期结束，重新设置工作时间
					{
						work_tick = 150/(float)tick;
						whole_tick = 150/(float)tick;
						flash_cnt = 0;
						run_time = 0;
						run_stage = 4;
					}
				}
				else if (run_stage == 4) //1次短闪，持续150ms，间隔1050ms
				{
					res = Effect_Pwm_LinearChange(init_pwm, init_pwm, 0, work_tick, whole_tick, &current_pwm, &run_time, &deal_data_update); //逐渐变灭
					if (res) 	//该工作周期结束，重新设置工作时间
					{
//						if (++flash_num < 2)
//						{
//							work_tick = 150/(float)tick;
//							whole_tick = 300/(float)tick;
//							run_stage = 0;
//							flash_cnt = 0;
//							run_time = 0;
//						}
//						else
						{
							run_stage = 5;
						}
					}
				}
				else
				{
					iam_here_status = 1;
				}
			}
			Effect_Set_PulseWight(current_pwm);		//设置定时器PWM占空比
			break;
		#endif /*IamHere_USE*/
		
		default:
			break;
	}
}

void Light_Effect_Enable(bool en)
{
	light_effect_enable = en;
}

bool Light_Effect_State(void)
{
	return light_effect_enable;
}

/**********************************************************
函数: Effect_Per1msCallback
函数描述: 光效处理回调函数，间隔时间为1ms的中断或任务调用
参数：	无
返回值： 无
作者:
日期:
***********************************************************/
void Effect_Per1msCallback(void)
{
	static uint8_t first_flag = 1;
	if(Light_Effect_State() == true)
	{
		if(first_flag == 1)
		{
			Effect_Deal(1, 1);
			first_flag = 0;
		}
		else
		{
			Effect_Deal(1, 0);
		}
	}	
	else
	{
		first_flag = 1;
	}
}


void Light_Effect_Init(void)
{
	Light_Effect_Enable(false);
// 	TIM4_IRQ_Callback_Add(Effect_Per1msCallback);  //將光效处理函数添加到timer4 1ms中断中
}




