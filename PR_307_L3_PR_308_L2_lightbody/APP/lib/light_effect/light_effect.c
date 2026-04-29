/*********************************************************************************
  *Copyright(C), 2018, Aputure, All rights reserver.
  *FileName:  	light_effect.c
  *Author:    	
  *Version:   	v1.7
  *Date:      	2021/06/10
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
			-v1.7: 2021/06/10,Steven,光效初步重构，待继续
**********************************************************************************/
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include "light_effect.h"
#include "light_effect_interface.h"

#define PIXEL_NUM_MAX 1
#define MEMORY_POOL_NUM (PIXEL_NUM_MAX + 1)
Effect_PulseWightRGBWW *memory_pool[MEMORY_POOL_NUM] = {NULL};

struct effect_buffer_t {
	uint8_t use_flag; //1:已使用
	uint8_t buffer[sizeof(Effect_PulseWightRGBWW) * PWM_ARR_NUM];
};

struct effect_buffer_t effect_buffer[2] = {0};

typedef enum {

	CopCar_Color_R = 0,
	CopCar_Color_B,
	CopCar_Color_RB,
	CopCar_Color_BW,
	CopCar_Color_RBW,
}CopCar_Type;

static bool light_effect_enable = false;


static const uint16_t club_interval_max[11] = {1200, 1200, 700, 500, 350, 250, 150, 130, 100, 90, 80}; /*ClubLights间隔时间*/
static const uint16_t club_interval_min[11] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}; /*ClubLights间隔时间*/
static const uint16_t club_wt[11] = {3800, 3800, 2250, 1800, 1500, 1200, 950, 750, 650, 550, 450}; /*ClubLights工作时间*/
static const uint16_t club_color_arr[8] = {3, 6, 9, 12, 15, 18, 24, 36}; /*ClubLights颜色种类*/
static const uint8_t sort_array[36] = {
	0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 
	21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35
}; /*用于随机排列*/

#if (ClubLights_USE == 1)
#endif /*ClubLights_USE*/

#if (Paparazzi_USE == 1)
static const uint16_t paparazzi_interval_max[12] = {11400, 11400, 8210, 5020, 1790, 1610, 1430, 1250, 1070, 890, 890, 11400}; /*Paparazzi间隔时间*/
static const uint16_t paparazzi_interval_min[12] = {1380, 1380, 920, 590, 200, 180, 150, 140, 120, 90, 90, 90}; /*Paparazzi间隔时间*/
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
static const uint16_t tv_wt_max[12] = {12800, 12800, 10800, 8400, 6900, 4800, 4300, 3700, 3300, 2700, 2200, 12800};    
static const uint16_t tv_wt_min[12] = {8200,  8200,  6700,  5500, 4200, 3100, 2800, 2500, 2300, 2000, 1500, 1500}; 
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
//static const uint16_t fault_work_min[12] = {150, 150, 150, 150, 150, 150, 150, 150, 150, 150, 150, 150};
static const uint16_t fault_work_min[12] = {300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300};
//static const uint16_t fault_rt_max[12] = {500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 7500};
//static const uint16_t fault_rt_min[12] = {50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50};
//static const uint16_t fault_pr[12] = {990, 990, 990, 990, 990, 990, 990, 990, 990, 990, 990, 990};
static const uint16_t fault_pr[12] = {990, 990, 980, 970, 960, 950, 940, 930, 920, 910, 900, 900};
//static const uint16_t fault_r[12] = {700, 700, 700, 700, 700, 700, 700, 700, 700, 700, 700, 700};
static const uint16_t fault_interval_max[11] = {250, 240, 230, 220, 210, 200, 190, 180, 170, 160, 150};
#endif /*(FaultBulb_USE == 1)*/

#if (Pulsing_USE == 1)
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
static const uint16_t welding_rt_max[11] = {100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100};
static const uint16_t welding_rt_min[11] = {40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40};
uint16_t welding_r[11] = {70, 90, 200, 225, 280, 380, 450, 500, 600, 500, 500};
#endif /*(Welding_USE == 1)*/

#if (CopCar_USE == 1)
/*CopCar非连闪间隔时间、例如不同颜色间的间隔*/
static const uint16_t CopCar_CInterval[6] = {120, 240, 456, 672, 890, 810};//{1000, 1000, 1000, 1000, 1000, 1000};//{120, 240, 456, 672, 890, 810};
static const uint16_t CopCar2_CInterval[6] = {120, 240, 456, 672, 890, 60};
/*CopCar工作时间*/
static const uint16_t CopCar_WT[6] = {180, 70, 70, 70, 70, 90};//{500, 500, 500, 500, 500, 500};//{180, 70, 70, 70, 70, 90};
//static const uint16_t CopCar_CInterval[6] = {120, 240, 456, 672, 890, 810};
///*CopCar工作时间*/
//static const uint16_t CopCar_WT[6] = {300, 300, 300, 300, 300, 300};
/*CopCar闪烁次数*/
static const uint8_t CopCar_FlashTimers[6] = {1, 2, 3, 4, 5, 5};

uint8_t copcarI_index[5] = {0, 1, 2, 3, 4};
uint8_t copcarII_index[6] = {0, 1, 2, 3, 4, 5};
uint8_t copcarIII_index[5] = {0, 1, 3, 4, 5};
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
static const uint8_t fireware_flash_min[12] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
static const uint8_t fireware_flash_max[12] = {1, 1, 1, 1, 1, 1, 1, 1, 2, 3, 4, 4};
#endif /*(Fireworks_USE == 1)*/

uint16_t color_fade_wt[11] = {3000, 3000, 2000, 1500, 1200, 800, 500, 300, 150, 100, 50};
uint16_t color_cycle_wt[11] = {6000, 6000, 5000, 4000, 3000, 2000, 1500, 1000, 500, 200, 100};
uint16_t color_gradient_wt[11] = {10000, 10000, 8000, 6000, 5000, 4000, 3000, 2000, 1500, 1000, 500};
uint16_t one_color_chase_wt[11] = {3000, 3000, 2000, 1500, 1200, 800, 500, 300, 150, 100, 50};

static uint8_t iam_here_status = 0;
Effect_Typedef light_effect;

uint8_t Get_EffectTypeIAmHere_Status(void)
{
	return iam_here_status;
}

void Clear_EffectTypeIAmHere_Status(void)
{
	iam_here_status = 0;
}

void *effect_malloc(void)
{
	int i;
	for (i = 0; i < 2; i++)
	{
		if (effect_buffer[i].use_flag == 0)
		{
			effect_buffer[i].use_flag = 1;
			return effect_buffer[i].buffer;
		}
	}
	return NULL;
}

void effect_free(void *address)
{
	int i;
	for (i = 0; i < 2; i++)
	{
		if (address == effect_buffer[i].buffer)
		{
			effect_buffer[i].use_flag = 0;
		}
	}
}

#if (ClubLights_USE == 1)
//数组顺序随机排列
static void Random_Sort(const uint8_t *src, uint8_t *dst, uint8_t length)
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
typedef struct{
	
	Effect_PulseWightRGBWW *current;
	Effect_PulseWightRGBWW *object;
	float rate[5]; //r/g/b/ww/cw
	float start_pwm[5]; //r/g/b/ww/cw
	uint16_t run_time;
	uint16_t whole_time;
}Fade_Struct;
// 1:缓变结束 
uint8_t Effect_Fade_Change(Fade_Struct *fade)
{
	uint8_t res = 0;
	
	if (fade->run_time == 0)
	{
		fade->rate[0] = (float)(fade->object->r - fade->current->r) / fade->whole_time;
		fade->start_pwm[0] = fade->current->r;
		fade->rate[1] = (float)(fade->object->g - fade->current->g) / fade->whole_time;
		fade->start_pwm[1] = fade->current->g;
		fade->rate[2] = (float)(fade->object->b - fade->current->b) / fade->whole_time;
		fade->start_pwm[2] = fade->current->b;
		fade->rate[3] = (float)(fade->object->ww - fade->current->ww) / fade->whole_time;
		fade->start_pwm[3] = fade->current->ww;
		fade->rate[4] = (float)(fade->object->cw - fade->current->cw) / fade->whole_time;
		fade->start_pwm[4] = fade->current->cw;
	}
	
	if (fade->object->r != fade->current->r)
	{
		res = 1;
		fade->current->r = fade->start_pwm[0] + fade->rate[0] * (fade->run_time);
	}
	if (fade->object->g != fade->current->g)
	{
		res = 1;
		fade->current->g = fade->start_pwm[1] + fade->rate[1] * (fade->run_time);
	}
	if (fade->object->b != fade->current->b)
	{
		res = 1;
		fade->current->b = fade->start_pwm[2] + fade->rate[2] * (fade->run_time);
	}
	if (fade->object->ww != fade->current->ww)
	{
		res = 1;
		fade->current->ww = fade->start_pwm[3] + fade->rate[3] * (fade->run_time);
	}
	if (fade->object->cw != fade->current->cw)
	{
		res = 1;
		fade->current->cw = fade->start_pwm[4] + fade->rate[4] * (fade->run_time);
	}
	
	if (fade->run_time < fade->whole_time)
	{
		 fade->run_time++;
	}
	else
	{
		fade->current->r = fade->object->r;
		fade->current->g = fade->object->g;
		fade->current->b = fade->object->b;
		fade->current->ww = fade->object->ww;
		fade->current->cw = fade->object->cw;
//		res = 1;
	}

	return res;
}

typedef struct{
	
	Effect_PulseWightRGBWW *pwm_start;
	float time_start;
	Effect_PulseWightRGBWW *pwm_end;
	float time_end;
	float time_end_last;
	float whole_time;
	float whole_time_last;
	Effect_PulseWightRGBWW *pwm_current;
	float *run_time;
	uint8_t* data_update;
	Effect_PulseWightRGBWW pwm_object;
	Fade_Struct fade;
}LinearChange_Struct;

static int8_t Effect_Pwm_LinearChange(LinearChange_Struct *liner)
{
	int8_t ret = 0;
	float time1;
	float time2;
	
	if(*liner->data_update == 0)
	{
		if(*liner->run_time <= liner->whole_time)	//周期时间，包括工作时间和间隔时间
		{
			if(*liner->run_time <= liner->time_end)	 //工作时间，pwm按线性变化
			{
				time1 = liner->time_end - liner->time_start;
				time2 = *liner->run_time - liner->time_start;
				liner->pwm_current->r = liner->pwm_start->r + (int32_t)(liner->pwm_end->r - liner->pwm_start->r)/time1 * time2;
				liner->pwm_current->g = liner->pwm_start->g + (int32_t)(liner->pwm_end->g - liner->pwm_start->g)/time1 * time2;
				liner->pwm_current->b = liner->pwm_start->b + (int32_t)(liner->pwm_end->b - liner->pwm_start->b)/time1 * time2;
				liner->pwm_current->ww = liner->pwm_start->ww + (int32_t)(liner->pwm_end->ww - liner->pwm_start->ww)/time1 * time2;
				liner->pwm_current->cw = liner->pwm_start->cw + (int32_t)(liner->pwm_end->cw - liner->pwm_start->cw)/time1 * time2;
			}
			else  //进入间隔时间，灭灯
			{
				memset(liner->pwm_current, 0, sizeof(Effect_PulseWightRGBWW));
			}
		}
		else
		{
			ret = 1;
		}
		*liner->run_time += 1.0f;
	}
	else if(*liner->data_update == 1) //更新当前时间、计算当前数据对应的pwm值
	{
		*liner->data_update = 0;
		if (*liner->run_time > liner->time_end_last)
		{
			*liner->run_time = liner->time_end + (*liner->run_time - liner->time_end_last)/(liner->whole_time_last - liner->time_end_last) * (liner->whole_time - liner->time_end);
			*liner->data_update = 0;
		}
		else
		{
			*liner->run_time = *liner->run_time/liner->time_end_last * liner->time_end;
		}
	}

	liner->whole_time_last = liner->whole_time;
	liner->time_end_last = liner->time_end;
	return ret;
}

#if ((PartyLights_USE == 1) || (ColorChase_USE == 1))

typedef struct{

	uint16_t index_start;
	uint16_t index_end;
	float time_start;
	float time_end;
	float whole_time;
	Effect_PulseWightRGBWW *pwm_src;
	Effect_PulseWightRGBWW *pwm_current;
	float *run_time;
	uint8_t *data_update;
	Effect_PulseWightRGBWW rgbww_pwm_object;
	uint16_t whole_time_last;
	uint16_t fade_run_time;
	Fade_Struct fade;
}Index_LinearChange;

static uint8_t Effect_Pwm_IndexChange(Index_LinearChange *index_change)
{
	int8_t ret = 0;
	float index = 0;
	uint32_t index_temp = 0;

	if(*index_change->data_update == 0)
	{
		if(*index_change->run_time <= index_change->whole_time)	//周期时间，包括工作时间和间隔时间
		{
			if(*index_change->run_time <= index_change->time_end)	 //工作时间，亮度按线性变化
			{
				index = (float)(index_change->index_start + (index_change->index_end - index_change->index_start)
					/(float)(index_change->time_end - index_change->time_start)*(*index_change->run_time - index_change->time_start));
			}
			else  //进入间隔时间，灭灯
			{
				index = 0;
			}
			index_temp = index;
			//index_temp = (index > index_temp)? (index_temp + 1) : index_temp;
			if(index > index_temp)
			{
				index_change->pwm_current->r = index_change->pwm_src[index_temp].r + (int16_t)(index_change->pwm_src[index_temp + 1].r - index_change->pwm_src[index_temp].r)*(index - index_temp);
				index_change->pwm_current->g = index_change->pwm_src[index_temp].g + (int16_t)(index_change->pwm_src[index_temp + 1].g - index_change->pwm_src[index_temp].g)*(index - index_temp);
				index_change->pwm_current->b = index_change->pwm_src[index_temp].b + (int16_t)(index_change->pwm_src[index_temp + 1].b - index_change->pwm_src[index_temp].b)*(index - index_temp);
				index_change->pwm_current->ww = index_change->pwm_src[index_temp].ww + (int16_t)(index_change->pwm_src[index_temp + 1].ww - index_change->pwm_src[index_temp].ww)*(index - index_temp);
				index_change->pwm_current->cw = index_change->pwm_src[index_temp].cw + (int16_t)(index_change->pwm_src[index_temp + 1].cw - index_change->pwm_src[index_temp].cw)*(index - index_temp);
			}
			else
			{
				index_change->pwm_current->r = index_change->pwm_src[index_temp].r;
				index_change->pwm_current->g = index_change->pwm_src[index_temp].g;
				index_change->pwm_current->b = index_change->pwm_src[index_temp].b;
				index_change->pwm_current->ww = index_change->pwm_src[index_temp].ww;
				index_change->pwm_current->cw = index_change->pwm_src[index_temp].cw;
			}
		}
		else
		{
			ret = 1;
		}
		*index_change->run_time += 1;
	}
	else if(*index_change->data_update == 1)
	{
		*index_change->run_time = *index_change->run_time/(float)index_change->whole_time_last * index_change->whole_time;
		*index_change->run_time = (*index_change->run_time > index_change->whole_time) ? index_change->whole_time : *index_change->run_time;
		
		if(*index_change->run_time <= index_change->time_end)	 //工作时间，亮度按线性变化
		{
			index = (int16_t)(index_change->index_start + (int32_t)(index_change->index_end - index_change->index_start)/(float)(index_change->time_end - index_change->time_start)*(*index_change->run_time - index_change->time_start));
		}
		else  //进入间隔时间，灭灯
		{
			index = 0;
		}
		index_temp = index;

		index_change->rgbww_pwm_object.r = index_change->pwm_src[index_temp].r;
		index_change->rgbww_pwm_object.g = index_change->pwm_src[index_temp].g;
		index_change->rgbww_pwm_object.b = index_change->pwm_src[index_temp].b;
		index_change->rgbww_pwm_object.ww = index_change->pwm_src[index_temp].ww;
		index_change->rgbww_pwm_object.cw = index_change->pwm_src[index_temp].cw;
		
		index_change->fade_run_time = 0;
		*index_change->data_update  = 2;
		
		index_change->fade.current = index_change->pwm_current;
		index_change->fade.object = &index_change->rgbww_pwm_object;
		index_change->fade.run_time = index_change->fade_run_time;
		index_change->fade.whole_time = EFFECT_FADE_TIME;
	}
	else
	{
		if (Effect_Fade_Change(&index_change->fade) == 0)
		{
			*index_change->data_update = 0;
		}
	}
	index_change->whole_time_last = index_change->whole_time;

	return ret;
}

#endif /*((PartyLights_USE == 1) || (Fireworks_USE == 1))*/

#if ((Candle_USE == 1) || (Fire_USE == 1))
typedef struct{
	
	uint16_t *pwm_min; //rbgwwcw
	uint16_t *pwm_max; //rbgwwcw
	float work_time;
	float rate;
	float *run_time;
	uint8_t* data_update;
	
	float time_end;
	float whole_time;
	float whole_time_last;
	uint16_t *pwm_current;
	uint16_t pwm_last[5];
	Fade_Struct fade;
}FirePwmChange_Struct;
static uint8_t Fire_Pwm_Change(FirePwmChange_Struct *fire)
{
	uint8_t i = 0;
	uint8_t ret = 0;
	float pwm_rate[5] = {0};
	
	for (i = 0; i < 5; i++)
	{
		pwm_rate[i] = ((int32_t)(fire->pwm_max[i] - fire->pwm_min[i]))/10000.0f;
	}
	
	if (*fire->data_update == 0)
	{
		if(fire->rate < 0)
		{
			for (i = 0; i < 5; i++)
			{
				if (fire->pwm_min[i] < fire->pwm_max[i])
				{
					if((fire->pwm_current[i] + fire->rate * pwm_rate[i]) > fire->pwm_min[i])
					{
						fire->pwm_current[i] = fire->pwm_last[i] + pwm_rate[i] * fire->rate * (*fire->run_time);
					}
					else
					{
						fire->pwm_current[i] = fire->pwm_min[i];
					}
				}
				else
				{
					if((fire->pwm_current[i] + fire->rate * pwm_rate[i]) < fire->pwm_min[i])
					{
						fire->pwm_current[i] = fire->pwm_last[i] + pwm_rate[i] * fire->rate * (*fire->run_time);
					}
					else
					{
						fire->pwm_current[i] = fire->pwm_min[i];
					}
				}
			}
		}
		else if(fire->rate > 0)
		{
			for (i = 0; i < 5; i++)
			{
				if(fire->pwm_min[i] < fire->pwm_max[i])
				{
					if((fire->pwm_current[i] + fire->rate * pwm_rate[i]) < fire->pwm_max[i])
					{
						fire->pwm_current[i] = fire->pwm_last[i] + pwm_rate[i] * fire->rate * (*fire->run_time);
					}
					else
					{
						fire->pwm_current[i] = fire->pwm_max[i];
					}
				}
				else
				{
					if((fire->pwm_current[i] + fire->rate * pwm_rate[i]) > fire->pwm_max[i])
					{
						fire->pwm_current[i] = fire->pwm_last[i] + pwm_rate[i] * fire->rate * (*fire->run_time);
					}
					else
					{
						fire->pwm_current[i] = fire->pwm_max[i];
					}
				}
			}
		}
		else	//无外力时自动恢复
		{
		}
		
		if(*fire->run_time > fire->work_time)
		{
			for (i = 0; i < 5; i++)
			{
				fire->pwm_last[i] = fire->pwm_current[i];
			}
			ret = 1;
		}
		(*fire->run_time)++;
		fire->fade.run_time = 0;
	}
	else
	{
		fire->fade.current = (Effect_PulseWightRGBWW *)fire->pwm_current;
		fire->fade.object = (Effect_PulseWightRGBWW *)fire->pwm_min;
		fire->fade.whole_time = EFFECT_FADE_TIME;
		if (Effect_Fade_Change(&fire->fade) == 0)
		{
			for (i = 0; i < 5; i++)
			{
				fire->pwm_last[i] = fire->pwm_current[i];
			}
			*fire->data_update = 0;
			ret = 1;
		}
		
	}//进入缓变处理

	return ret;
}

typedef struct {
	Effect_PulseWightRGBWW *pwm_start;
	Effect_PulseWightRGBWW *pwm_end;
	float time_start;
	float time_end;
	float whole_time;
}Effect_Element;

uint8_t effect_color_init(const Effect_Mode_Arg *arg, Effect_Mode mode, Effect_PulseWightRGBWW *rgbww_pwm)
{
	uint8_t ret = 1;
	switch(mode)
	{
		case EffectModeCCT:
			Effect_CCT2PulseWight(arg->cct_arg.cct, arg->cct_arg.gm, arg->cct_arg.lightness, rgbww_pwm);
			break;
		case EffectModeHSI:
			Effect_HSI2PulseWight(arg->hsi_arg.hue, arg->hsi_arg.sat, arg->hsi_arg.lightness, rgbww_pwm, arg->hsi_arg.cct);
			break;
		case EffectModeGEL:
			Effect_Gel2PulseWight(arg->gel_arg.brand, arg->gel_arg.type, arg->gel_arg.cct, 
									arg->gel_arg.color, arg->gel_arg.lightness, rgbww_pwm);
			break;
		case EffectModeCoord:
			Effect_Coord2PulseWight(arg->coord_arg.x, arg->coord_arg.y, arg->coord_arg.lightness, rgbww_pwm);
			break;
		case EffectModeSource:
			Effect_Source2PulseWight(arg->source_arg.type, 0, 0, arg->source_arg.lightness, 0, rgbww_pwm);
			break;
		case EffectModeRGB:
			Effect_RGB2PulseWight(arg->rgb_arg.lightness, arg->rgb_arg.r, arg->rgb_arg.g, arg->rgb_arg.b, rgbww_pwm);
			break;
		case EffectModeBlack:
			memset(rgbww_pwm, 0, sizeof(Effect_PulseWightRGBWW));
			break;
		default:
			ret = 0;
			break;
	}
	return ret;
}

void Effect_Init(const void* effect, void *effect_mode, void (*Effect_Init_Fun)(void *, const Light_Effect* , Effect_PulseWightRGBWW *, uint8_t ), uint8_t restart)
{
	uint8_t i;
	Effect_PulseWightRGBWW *buffer = NULL;
	uint32_t address = 0;
	//buffer = (Effect_PulseWightRGBWW *)malloc(sizeof(Effect_PulseWightRGBWW) * PWM_ARR_NUM);
	buffer = effect_malloc();
	if (buffer)
	{
		Effect_Init_Fun(effect_mode, (const Light_Effect*)effect, buffer, restart);
		memcpy(&address, effect_mode, sizeof(uint32_t));
		if (address == (uint32_t)buffer)
		{
			//内存池管理
			for (i = 0; memory_pool[i] != NULL && i < MEMORY_POOL_NUM; i++)
			{
				effect_free(memory_pool[i]);//free(memory_pool[i]);
				memory_pool[i] = NULL;
			}
			memory_pool[0] = buffer;
		}
		else
		{
			effect_free(buffer);//free(buffer);
		}
	}
}

#endif /*(Candle_USE == 1) || (EffectType_Fire == 1)*/

typedef struct{
	
	Effect_PulseWightRGBWW *rgbww_pwm_arr;
	uint16_t  lightness;  		/*亮度<0-1000>*/
	uint16_t  init_lightness;
	uint8_t   frq;  			/*<1-11>,11为Random*/
	uint8_t   proc_frq;  		/*<1-11>,11为Random*/
	uint8_t   color;  			/*<0-3>,0:3; 1:6; 2:9; 3:12*/
	uint8_t   init_color;
	uint8_t	sat;
	float whole_tick;
	float work_tick;
	uint8_t deal_data_update;
	uint8_t club_random_sort[36];
	uint8_t random_sort_index;
	Effect_PulseWightRGBWW init_pwm;
	Effect_PulseWightRGBWW pwm_current;
	uint8_t club_change_type;
	uint8_t run_stage;
	float run_time;
	LinearChange_Struct liner;
	
}Effect_ClubLights;
Effect_ClubLights club;

void ClubLights_Init(void *test, const Light_Effect* effect, Effect_PulseWightRGBWW *rgbww_pwm, uint8_t restart)
{
	uint8_t color;
	uint16_t index = 0;
	uint16_t  lightness;
	uint8_t	sat;
	Effect_ClubLights *club = (Effect_ClubLights *)test;
	
	if (club == NULL || effect == NULL || rgbww_pwm == NULL)
	{
		return;
	}
	
	lightness = effect->effect_arg.club_lights_arg.lightness;
	sat = 99;
	color = (effect->effect_arg.club_lights_arg.color <= 7) ? effect->effect_arg.club_lights_arg.color : 7;
	color = club_color_arr[color];

	if(restart == 1 || club->init_lightness != lightness || club->init_color != color)  //亮度、光效类型、颜色种类变化时重新初始化参数
	{
		for(index = 360/color; index <= 360; index += 360/color)
		{
			Effect_HSI2PulseWight(index, sat, lightness, &rgbww_pwm[index/(360/color) - 1], 6500);
		}
		__disable_irq();
		club->rgbww_pwm_arr = rgbww_pwm; ////更新pwm数据
		__enable_irq();
	}
	club->init_lightness = lightness;
	club->init_color = color;
	__disable_irq();
	//更新光效参数
	club->lightness = lightness;
	club->sat = 99;
	club->color = color; //颜色数量
	club->frq = (effect->effect_arg.club_lights_arg.frq <= 11) ? (effect->effect_arg.club_lights_arg.frq) : 11; //频率
	__enable_irq();
}

void Effect_ClubLights_Init(const void* effect, uint8_t restart)
{
	Effect_Init(effect, &club, ClubLights_Init, restart);
}
//
void ClubLights_Process(Effect_ClubLights *club, Effect_PulseWightRGBWW *current_pwm, uint8_t tick, uint8_t restart)
{
	uint16_t random = 0;
	uint8_t res = 0;
	uint8_t i = 0;
	Effect_PulseWightRGBWW off_pwm = {light_effect.min_pwm.r, light_effect.min_pwm.g,  
										light_effect.min_pwm.b, light_effect.min_pwm.ww, light_effect.min_pwm.cw};  //默认pwm值，默认为最低亮度，即灭
	
	LinearChange_Struct linear[2][2] = {
		{{.pwm_start = &club->init_pwm, .pwm_end = &club->init_pwm,}, {.pwm_start = &off_pwm, .pwm_end = &club->init_pwm,},},
		{{.pwm_start = &club->init_pwm, .pwm_end = &club->init_pwm,}, {.pwm_start = &club->init_pwm, .pwm_end = &off_pwm,},},
	};

	if (club == NULL || current_pwm == NULL)
	{
		return;
	}
	
	if(restart == 1 || club->proc_frq != club->frq)
	{
		if (club->frq == 11)
		{
			random = Effect_Get_RandomRange_Int16(1, 10);
		}
		else
		{
			random = club->frq;
		}
		club->whole_tick = club_wt[random]/(float)tick; //每循环总工作时钟节拍数
		club->work_tick = (club_wt[random] - Effect_Get_RandomRange_Int16(club_interval_min[random], club_interval_max[random]))/(float)tick; //每循环工作时钟节拍数;
		club->deal_data_update = 1;
		if(restart == 1)
		{
			Random_Sort(sort_array ,club->club_random_sort, club->color);
			club->random_sort_index = 0;
			memcpy(&club->init_pwm, club->rgbww_pwm_arr + club->club_random_sort[club->random_sort_index], sizeof(Effect_PulseWightRGBWW));
			
			if(Effect_Get_RandomRange_Int16(0, 9) > 5)
			{
				club->club_change_type = 0;
			}
			else
			{
				club->club_change_type = 1;
			}
			club->run_stage = 0;
			club->run_time = 0;
			club->deal_data_update = 0;
		}
		restart = 0;
		club->proc_frq = club->frq;
		
		
	}
	for(i = 0; i < 2; i++)
	{
		linear[0][i].time_start = 0;
		linear[0][i].time_end = club->work_tick/2;
		linear[0][i].whole_time = club->work_tick/2;
	}
	for(i = 0; i < 2; i++)
	{
		linear[1][i].time_start = club->work_tick/2;
		linear[1][i].time_end = club->work_tick;
		linear[1][i].whole_time = club->whole_tick;
	}
	club->liner.pwm_start = linear[club->run_stage][club->club_change_type].pwm_start;
	club->liner.pwm_end = linear[club->run_stage][club->club_change_type].pwm_end;
	club->liner.time_start = linear[club->run_stage][club->club_change_type].time_start;
	club->liner.time_end = linear[club->run_stage][club->club_change_type].time_end;
	club->liner.whole_time = linear[club->run_stage][club->club_change_type].whole_time;
	club->liner.pwm_current = &club->pwm_current;
	club->liner.run_time = &club->run_time;
	club->liner.data_update = &club->deal_data_update;
	res = Effect_Pwm_LinearChange(&club->liner);
	memcpy(current_pwm, &club->pwm_current, sizeof(Effect_PulseWightRGBWW));
	if(res) 	//该工作阶段结束，进入第二阶段
	{
		
		if (club->run_stage == 1)
		{
			if (club->frq == 11)
			{
				random = Effect_Get_RandomRange_Int16(1, 10);
			}
			else
			{
				random = club->frq;
			}
			club->whole_tick = club_wt[random]/(float)tick; //每循环总工作时钟节拍
			club->work_tick = (club_wt[random] - Effect_Get_RandomRange_Int16(club_interval_min[random], club_interval_max[random]))/(float)tick; //每循环工作时钟节拍数;
			
			if(++club->random_sort_index >= club->color || club->proc_frq != club->frq)
			{
				Random_Sort(sort_array, club->club_random_sort, club->color);
				club->random_sort_index = 0;
				club->proc_frq = club->frq;
			}
			memcpy(&club->init_pwm, club->rgbww_pwm_arr + club->club_random_sort[club->random_sort_index], sizeof(Effect_PulseWightRGBWW));
			if(Effect_Get_RandomRange_Int16(0, 9) > 5)
			{
				club->club_change_type = 0;
			}
			else
			{
				club->club_change_type = 1;
			}
			club->run_stage = 0;
			club->run_time = 0;
		}
		else
		{
			club->club_change_type = Effect_Get_RandomRange_Int16(0, 1);
			club->run_stage = 1;
		}
	}
}

void Effect_ClubLights_Process(uint8_t tick, uint8_t restart)
{
	static Effect_PulseWightRGBWW current_pwm = {0};
	
	if(restart)
	{
		memset(&current_pwm, 0, sizeof(Effect_PulseWightRGBWW));
	}
	
	ClubLights_Process(&club, &current_pwm, tick, restart);
	Effect_SetPixel_PulseWight(current_pwm, 0xff);
	Effect_ActivePixel_PulseWight();
}

typedef struct{
	
	Effect_PulseWightRGBWW *rgbww_pwm_arr;
	uint16_t interval_min;
	uint16_t interval_max;
	float whole_tick;
	float work_tick;
	uint8_t deal_data_update;
	Effect_PulseWightRGBWW* init_pwm;
	Effect_PulseWightRGBWW pwm_current;
	float run_time;
	LinearChange_Struct liner;
	Light_Effect init_effect;
	Light_Effect proc_effect;
}Effect_Paparazzi;
Effect_Paparazzi effect_paparazzi = {0};

void Paparazzi_Init(void *effect_mode, const Light_Effect* effect, Effect_PulseWightRGBWW *rgbww_pwm, uint8_t restart, uint8_t type)
{
	Effect_Paparazzi *paparazzi = effect_mode;
	const Paparazzi *paparazzi_arg = &effect->effect_arg.paparazzi_arg;
	uint8_t frq;
	
	if (type == 2)
	{
		paparazzi_arg = &effect->effect_arg.paparazzi2_arg;
	}
	if (paparazzi == NULL || effect == NULL || rgbww_pwm == NULL)
	{
		return;
	}

	if (restart == 1 || paparazzi_arg->mode != paparazzi->init_effect.effect_arg.paparazzi_arg.mode
		|| memcmp(&paparazzi->init_effect.effect_arg.paparazzi_arg.arg, &paparazzi_arg->arg, sizeof(paparazzi->init_effect.effect_arg.paparazzi_arg.arg)) != 0)//重新初始化参数
	{
		if (effect_color_init(&paparazzi_arg->arg, paparazzi_arg->mode, rgbww_pwm))
		{
			__disable_irq();
			paparazzi->deal_data_update = 1;
			paparazzi->rgbww_pwm_arr = rgbww_pwm; //更新pwm数据
			__enable_irq();
		}
	}
	memcpy(&paparazzi->init_effect.effect_arg.paparazzi_arg, paparazzi_arg, sizeof(paparazzi->init_effect.effect_arg.paparazzi_arg));
	__disable_irq();
	if (type == 1) //一代光效
	{
		frq = (paparazzi_arg->frq <= 11) ? (paparazzi_arg->frq) : 11;
		paparazzi->init_effect.effect_arg.paparazzi_arg.max_interval = paparazzi_interval_max[frq] - 35;
		paparazzi->init_effect.effect_arg.paparazzi_arg.min_interval = paparazzi_interval_min[frq] - 35;
	}
	else if (type == 2)
	{
		paparazzi->init_effect.effect_arg.paparazzi_arg.max_interval = paparazzi_arg->max_interval;
		paparazzi->init_effect.effect_arg.paparazzi_arg.min_interval = paparazzi_arg->min_interval;
	}
	__enable_irq();
}

void PaparazziI_Init(void *effect_mode, const Light_Effect* effect, Effect_PulseWightRGBWW *rgbww_pwm, uint8_t restart)
{
	Paparazzi_Init(effect_mode, effect, rgbww_pwm, restart, 1);
}

void Effect_Paparazzi_Init(const void* effect, uint8_t restart)
{
	Effect_Init(effect, &effect_paparazzi, PaparazziI_Init, restart);
}

void PaparazziII_Init(void *effect_mode, const Light_Effect* effect, Effect_PulseWightRGBWW *rgbww_pwm, uint8_t restart)
{
	Paparazzi_Init(effect_mode, effect, rgbww_pwm, restart, 2);
}
void Effect_PaparazziII_Init(const void* effect, uint8_t restart)
{
	Effect_Init(effect, &effect_paparazzi, PaparazziII_Init, restart);
}

void Paparazzi_ResetArg(Effect_Paparazzi *paparazzi, uint8_t tick, uint8_t restart)
{
	paparazzi->interval_max = paparazzi->init_effect.effect_arg.paparazzi_arg.max_interval;
	paparazzi->interval_min = paparazzi->init_effect.effect_arg.paparazzi_arg.min_interval;
	
	paparazzi->whole_tick = paparazzi->work_tick + Effect_Get_RandomRange_Int16(paparazzi->interval_min, paparazzi->interval_max)/(float)tick;
	if(restart == 1)
	{
		paparazzi->run_time = 0;
		paparazzi->deal_data_update = 0;
	}
}
void Paparazzi_Process(Effect_Paparazzi *paparazzi, Effect_PulseWightRGBWW *current_pwm, uint8_t tick, uint8_t restart)
{
	uint8_t res = 0;
	Effect_PulseWightRGBWW off_pwm = {light_effect.min_pwm.r, light_effect.min_pwm.g,  
										light_effect.min_pwm.b, light_effect.min_pwm.ww, light_effect.min_pwm.cw};  //默认pwm值，默认为最低亮度，即灭
	
	if (paparazzi == NULL || current_pwm == NULL)
	{
		return;
	}
	
	paparazzi->work_tick = 135/(float)tick; //每循环工作时钟节拍数;
	paparazzi->init_pwm = paparazzi->rgbww_pwm_arr;
	if(restart == 1 || paparazzi->proc_effect.effect_arg.paparazzi_arg.min_interval != paparazzi->init_effect.effect_arg.paparazzi_arg.min_interval
		|| paparazzi->proc_effect.effect_arg.paparazzi_arg.max_interval != paparazzi->init_effect.effect_arg.paparazzi_arg.max_interval)
	{
		paparazzi->deal_data_update = 1;
		Paparazzi_ResetArg(paparazzi, tick, restart);
		paparazzi->proc_effect.effect_arg.paparazzi_arg.min_interval = paparazzi->init_effect.effect_arg.paparazzi_arg.min_interval;
		paparazzi->proc_effect.effect_arg.paparazzi_arg.max_interval = paparazzi->init_effect.effect_arg.paparazzi_arg.max_interval;
	}
	else
	{
		paparazzi->liner.pwm_start = paparazzi->init_pwm;
		paparazzi->liner.pwm_end = &off_pwm;
		paparazzi->liner.time_start = 0;
		paparazzi->liner.time_end = paparazzi->work_tick;
		paparazzi->liner.whole_time = paparazzi->whole_tick;
		paparazzi->liner.pwm_current = &paparazzi->pwm_current;;
		paparazzi->liner.run_time = &paparazzi->run_time;
		paparazzi->liner.data_update = &paparazzi->deal_data_update;
		res = Effect_Pwm_LinearChange(&paparazzi->liner); //逐渐变灭
		memcpy(current_pwm, &paparazzi->pwm_current, sizeof(Effect_PulseWightRGBWW));
		if(res) 	//该工作周期结束，重新设置工作时间
		{
			Paparazzi_ResetArg(paparazzi, tick, 0);
			paparazzi->run_time = 0;
		}
	}
}

void Effect_Paparazzi_Process(uint8_t tick, uint8_t restart)
{
	static Effect_PulseWightRGBWW current_pwm = {0};
	
	if(restart)
	{
		memset(&current_pwm, 0, sizeof(Effect_PulseWightRGBWW));
	}
	
	Paparazzi_Process(&effect_paparazzi, &current_pwm, tick, restart);
	Effect_SetPixel_PulseWight(current_pwm, 0xff);
	Effect_ActivePixel_PulseWight();
}

typedef struct{
	
	Effect_PulseWightRGBWW *rgbww_pwm_arr;
	Effect_TriggerType trigger_type;
	Effect_TriggerType proc_trigger_type;
	uint16_t interval;
	uint32_t work_interval;
	uint8_t flash_num;
	uint8_t min_flash_num;
	uint8_t max_flash_num;
	float whole_tick;
	float work_tick;
	uint8_t deal_data_update;
	Effect_PulseWightRGBWW init_pwm;
	Effect_PulseWightRGBWW pwm_min;
	float run_time;
	uint8_t run_stage;
	LinearChange_Struct liner;
	Effect_PulseWightRGBWW pwm_current;
	Light_Effect init_effect;
	Light_Effect proc_effect;
}Effect_Lightning;
Effect_Lightning effect_lightning;
void Lightning_Init(void *effect_mode, const Light_Effect* effect, Effect_PulseWightRGBWW *rgbww_pwm, uint8_t restart, uint8_t type)
{
	Effect_Lightning *lightning = (Effect_Lightning *)effect_mode;
	const Lightning *lightning_arg = &effect->effect_arg.lightning_arg;
	
	if (lightning == NULL || effect == NULL || rgbww_pwm == NULL)
	{
		return;
	}
	if (type == 3)
	{
		lightning_arg = &effect->effect_arg.lightning3_arg;
	}
	if (restart == 1 || memcmp(&lightning->init_effect.effect_arg.lightning_arg.arg, &lightning_arg->arg,
								sizeof(lightning->init_effect.effect_arg.lightning_arg.arg)) != 0)//重新初始化参数
	{
		if (effect_color_init(&lightning_arg->arg, lightning_arg->mode, rgbww_pwm))
		{
			__disable_irq();
			lightning->rgbww_pwm_arr = rgbww_pwm; //更新pwm数据
			__enable_irq();
		}
	}
	memcpy(&lightning->init_effect.effect_arg.lightning_arg, lightning_arg, sizeof(lightning->init_effect.effect_arg.lightning_arg));
	__disable_irq();
	
//	if (type == 1)
//	{
//		uint8_t frq_index_min, frq_index_max;
//		
//		if (lightning_arg->frq >= 11)
//		{
//			frq_index_min = 1;
//			frq_index_max = 10;
//			lightning->min_flash_num = 2;
//			lightning->max_flash_num = 11;
//		}
//		else
//		{
//			frq_index_min = lightning_arg->frq;
//			frq_index_max = frq_index_min;
//			lightning->min_flash_num = frq_index_min + 1;
//			lightning->max_flash_num = lightning->min_flash_num;
//		}
//		lightning->init_effect.effect_arg.lightning_arg.min_interval = lightning_interval[frq_index_max][frq_index_max];
//		lightning->init_effect.effect_arg.lightning_arg.max_interval = lightning_interval[frq_index_min][frq_index_min];
//	}
//	else if(type == 2)
    if (type == 1 || type == 2)    
	{
		uint8_t speed_index_min, speed_index_max;
		uint8_t frq_index_min, frq_index_max;
		
		if (lightning_arg->frq >= 11)
		{
			frq_index_min = 1;
			frq_index_max = 10;
		}
		else
		{
			frq_index_min = lightning_arg->frq;
			frq_index_max = frq_index_min;
		}
		if (lightning_arg->speed >= 11)
		{
			speed_index_min = 1;
			speed_index_max = 10;
			lightning->min_flash_num = 2;
			lightning->max_flash_num = 11;
		}
		else
		{
			speed_index_min = lightning_arg->speed;
			speed_index_max = speed_index_min;
			lightning->min_flash_num = speed_index_min + 1;
			lightning->max_flash_num = lightning->min_flash_num;
		}
		lightning->init_effect.effect_arg.lightning_arg.min_interval = lightning_interval[frq_index_max][speed_index_max];
		lightning->init_effect.effect_arg.lightning_arg.max_interval = lightning_interval[frq_index_min][speed_index_min];
	}
	else if(type == 3)
	{
		lightning->min_flash_num = 2;
		lightning->max_flash_num = 6;
		lightning->init_effect.effect_arg.lightning_arg.min_interval = lightning_arg->min_interval;
		lightning->init_effect.effect_arg.lightning_arg.max_interval = lightning_arg->max_interval;
	}
	lightning->trigger_type = lightning_arg->trigger;  //触发类型
	__enable_irq();
}

void Lightning_InitI(void *effect_mode, const Light_Effect* effect, Effect_PulseWightRGBWW *rgbww_pwm, uint8_t restart)
{
	Lightning_Init(effect_mode, effect, rgbww_pwm, restart, 1);
}

void Effect_Lightning_Init(const void* effect, uint8_t restart)
{
	Effect_Init(effect, &effect_lightning, Lightning_InitI, restart);
}

void Lightning_InitII(void *effect_mode, const Light_Effect* effect, Effect_PulseWightRGBWW *rgbww_pwm, uint8_t restart)
{
	Lightning_Init(effect_mode, effect, rgbww_pwm, restart, 2);
}

void Effect_LightningII_Init(const void* effect, uint8_t restart)
{
	Effect_Init(effect, &effect_lightning, Lightning_InitII, restart);
}

void Lightning_InitIII(void *effect_mode, const Light_Effect* effect, Effect_PulseWightRGBWW *rgbww_pwm, uint8_t restart)
{
	Lightning_Init(effect_mode, effect, rgbww_pwm, restart, 3);
}

void Effect_LightningIII_Init(const void* effect, uint8_t restart)
{
	Effect_Init(effect, &effect_lightning, Lightning_InitIII, restart);
}

void Lightning_Process(Effect_Lightning *lightning, Effect_PulseWightRGBWW *current_pwm, uint8_t tick, uint8_t restart)
{
	static uint8_t res = 0;
	Effect_PulseWightRGBWW off_pwm = {light_effect.min_pwm.r, light_effect.min_pwm.g,  
										light_effect.min_pwm.b, light_effect.min_pwm.ww, light_effect.min_pwm.cw};  //默认pwm值，默认为最低亮度，即灭

	if (lightning == NULL || current_pwm == NULL)
	{
		return;
	}

	lightning->interval = Effect_Get_RandomRange_Int16(lightning->init_effect.effect_arg.lightning_arg.min_interval, lightning->init_effect.effect_arg.lightning_arg.max_interval);
	
	if (lightning->flash_num == 0 && (
		lightning->proc_effect.effect_arg.lightning_arg.min_interval != lightning->init_effect.effect_arg.lightning_arg.min_interval
		|| lightning->proc_effect.effect_arg.lightning_arg.max_interval != lightning->init_effect.effect_arg.lightning_arg.max_interval
		)
	)
	{
		lightning->work_interval = lightning->interval;
		lightning->work_tick = 30/(float)tick;
		lightning->whole_tick = lightning->work_tick + lightning->work_interval/(float)tick;
		lightning->proc_effect.effect_arg.lightning_arg.min_interval = lightning->init_effect.effect_arg.lightning_arg.min_interval;
		lightning->proc_effect.effect_arg.lightning_arg.max_interval = lightning->init_effect.effect_arg.lightning_arg.max_interval;
	}
	if (res || restart == 1 || lightning->trigger_type == Trigger_Once || (lightning->proc_trigger_type != lightning->trigger_type && lightning->trigger_type == Trigger_Continue))
	{
		if (restart == 1 || lightning->trigger_type == Trigger_Once || (lightning->proc_trigger_type != lightning->trigger_type && lightning->trigger_type == Trigger_Continue))
		{
			lightning->flash_num = 0;
		}
		if (lightning->flash_num == 0 && lightning->trigger_type != Trigger_None)
		{
			lightning->flash_num = Effect_Get_RandomRange_Int16(lightning->min_flash_num, lightning->max_flash_num);
			
			lightning->work_interval = 196;
			lightning->work_tick = 480/(float)tick; //每循环工作时钟节拍数;
			lightning->whole_tick = lightning->work_tick + lightning->work_interval/(float)tick;
			memcpy(&lightning->init_pwm, lightning->rgbww_pwm_arr, sizeof(Effect_PulseWightRGBWW));
			lightning->pwm_min.r = light_effect.min_pwm.r + (lightning->init_pwm.r - (int32_t)light_effect.min_pwm.r) * 0.5f;
			lightning->pwm_min.g = light_effect.min_pwm.g + (lightning->init_pwm.g - (int32_t)light_effect.min_pwm.g) * 0.5f;
			lightning->pwm_min.b = light_effect.min_pwm.b + (lightning->init_pwm.b - (int32_t)light_effect.min_pwm.b) * 0.5f;
			lightning->pwm_min.ww = light_effect.min_pwm.ww + (lightning->init_pwm.ww - (int32_t)light_effect.min_pwm.ww) * 0.5f;
			lightning->pwm_min.cw = light_effect.min_pwm.cw + (lightning->init_pwm.cw - (int32_t)light_effect.min_pwm.cw) * 0.5f;
			lightning->liner.pwm_start = &lightning->init_pwm;
			lightning->run_stage = 1;
			lightning->run_time = 0;
		}
		else if (lightning->flash_num > 0)
		{
			if (lightning->flash_num != 1)
			{
				lightning->work_interval = 120;
				lightning->work_tick = 30/(float)tick;
				lightning->whole_tick = lightning->work_tick + lightning->work_interval/(float)tick;
			}
			else
			{
				lightning->work_interval = lightning->interval;
				lightning->work_tick = 30/(float)tick;
				lightning->whole_tick = lightning->work_tick + lightning->work_interval/(float)tick;
			}
			lightning->flash_num--;
			lightning->run_time = 0;
			lightning->liner.pwm_start = &lightning->pwm_min;
		}	
		if (lightning->trigger_type == Trigger_Once)
		{
			lightning->trigger_type = Trigger_None;
		}
		lightning->proc_trigger_type = lightning->trigger_type;
	}
	if (lightning->run_stage != 1)
	{
		return;
	}
	lightning->liner.pwm_end = &off_pwm;
	lightning->liner.time_start = 0;
	lightning->liner.time_end = lightning->work_tick;
	lightning->liner.whole_time = lightning->whole_tick;
	lightning->liner.pwm_current = &lightning->pwm_current;
	lightning->liner.run_time = &lightning->run_time;
	lightning->liner.data_update = &lightning->deal_data_update;
	res = Effect_Pwm_LinearChange(&lightning->liner); //逐渐变灭
	memcpy(current_pwm, &lightning->pwm_current, sizeof(Effect_PulseWightRGBWW));
}

void Effect_Lightning_Process(uint8_t tick, uint8_t restart)
{
	static Effect_PulseWightRGBWW current_pwm = {0};
	
	if(restart)
	{
		memset(&current_pwm, 0, sizeof(Effect_PulseWightRGBWW));
	}
	
	Lightning_Process(&effect_lightning, &current_pwm, tick, restart);
	Effect_SetPixel_PulseWight(current_pwm, 0xff);
	Effect_ActivePixel_PulseWight();
}

typedef struct{
	
	Effect_PulseWightRGBWW *rgbww_pwm_arr;
	float whole_tick;
	float work_tick;
	uint8_t deal_data_update;
	Effect_PulseWightRGBWW init_pwm;
	Effect_PulseWightRGBWW pwm;
	float run_time;
	LinearChange_Struct liner;
	Effect_PulseWightRGBWW pwm_current;
	Light_Effect init_effect;
	uint16_t max_color_index;
	uint16_t init_max_work_time;
	uint16_t init_min_work_time;
	uint16_t proc_max_work_time;
	uint16_t proc_min_work_time;
	uint8_t data_init;
	uint16_t init_lightness;
	uint16_t proc_lightness;
}Effect_TV;
Effect_TV effect_tv;
void TV_Init(void *effect_mode, const Light_Effect* effect, Effect_PulseWightRGBWW *rgbww_pwm, uint8_t restart, uint8_t type)
{
	uint16_t cct = 0;
	uint16_t hue;
	uint16_t index = 0;
	uint8_t data_updated = 0;
	Effect_TV *tv = (Effect_TV *)effect_mode;
	float pwm_ratio[5];
	uint8_t   frq;  
	const TV *tv_arg = &effect->effect_arg.tv_arg;
	
	if (tv == NULL || effect == NULL || rgbww_pwm == NULL)
	{
		return;
	}
	if (type == 2)
	{
		tv_arg = &effect->effect_arg.tv2_arg;
	}
	else if (type == 3)
	{
		tv_arg = &effect->effect_arg.tv3_arg;
	}
	if (restart == 1 
		|| tv_arg->range1_mode != tv->init_effect.effect_arg.tv_arg.range1_mode
		|| tv_arg->range2_mode != tv->init_effect.effect_arg.tv_arg.range2_mode
		|| memcmp(&tv->init_effect.effect_arg.tv_arg.range1_arg, &tv_arg->range1_arg, sizeof(tv->init_effect.effect_arg.tv_arg.range1_arg)) != 0
		|| memcmp(&tv->init_effect.effect_arg.tv_arg.range2_arg, &tv_arg->range2_arg, sizeof(tv->init_effect.effect_arg.tv_arg.range2_arg)) != 0)//重新初始化参数
	{
		if (tv_arg->range1_mode == EffectModeCCT && tv_arg->range2_mode == EffectModeCCT)
		{
			if (tv_arg->range1_arg.cct_arg.cct > tv_arg->range2_arg.cct_arg.cct)
			{
				for (cct = tv_arg->range2_arg.cct_arg.cct, index = 0; cct <= tv_arg->range1_arg.cct_arg.cct; cct += 50, index++)
				{
					Effect_CCT2PulseWight(cct, tv_arg->range1_arg.cct_arg.gm, tv_arg->range1_arg.cct_arg.lightness, &rgbww_pwm[index]);
				}
			}
			else
			{
				for (cct = tv_arg->range1_arg.cct_arg.cct, index = 0; cct <= tv_arg->range2_arg.cct_arg.cct; cct += 50, index++)
				{
					Effect_CCT2PulseWight(cct, tv_arg->range1_arg.cct_arg.gm, tv_arg->range1_arg.cct_arg.lightness, &rgbww_pwm[index]);
				}
			}
			tv->max_color_index = index - 1;
		}
		else if (tv_arg->range1_mode == EffectModeHSI && tv_arg->range2_mode == EffectModeHSI)
		{
			if (tv_arg->range1_arg.hsi_arg.hue > tv_arg->range2_arg.hsi_arg.hue)
			{
				for (hue = tv_arg->range2_arg.hsi_arg.hue, index = 0; hue <= tv_arg->range1_arg.hsi_arg.hue; hue += 1, index++)
				{
					Effect_HSI2PulseWight(hue, tv_arg->range1_arg.hsi_arg.sat, tv_arg->range1_arg.hsi_arg.lightness, &rgbww_pwm[index], tv_arg->range1_arg.hsi_arg.cct);
				}
			}
			else
			{
				for (hue = tv_arg->range1_arg.hsi_arg.hue, index = 0; hue <= tv_arg->range2_arg.hsi_arg.hue; hue += 1, index++)
				{
					Effect_HSI2PulseWight(hue, tv_arg->range1_arg.hsi_arg.sat, tv_arg->range1_arg.hsi_arg.lightness, &rgbww_pwm[index], tv_arg->range1_arg.hsi_arg.cct);
				}
			}
			tv->max_color_index = index - 1;
		}
		else
		{
			effect_color_init(&tv_arg->range1_arg, tv_arg->range1_mode, &rgbww_pwm[0]);
			effect_color_init(&tv_arg->range2_arg, tv_arg->range2_mode, &rgbww_pwm[100]);
			tv->max_color_index = 100;
			pwm_ratio[0] = (rgbww_pwm[100].r - rgbww_pwm[0].r)/100.0f;
			pwm_ratio[1] = (rgbww_pwm[100].g - rgbww_pwm[0].g)/100.0f;
			pwm_ratio[2] = (rgbww_pwm[100].b - rgbww_pwm[0].b)/100.0f;
			pwm_ratio[3] = (rgbww_pwm[100].ww - rgbww_pwm[0].ww)/100.0f;
			pwm_ratio[4] = (rgbww_pwm[100].cw - rgbww_pwm[0].cw)/100.0f;
			for (index = 1; index < 100; index++)
			{
				rgbww_pwm[index].r = rgbww_pwm[0].r + pwm_ratio[0] * index;
				rgbww_pwm[index].g = rgbww_pwm[0].g + pwm_ratio[1] * index;
				rgbww_pwm[index].b = rgbww_pwm[0].b + pwm_ratio[2] * index;
				rgbww_pwm[index].ww = rgbww_pwm[0].ww + pwm_ratio[3] * index;
				rgbww_pwm[index].cw = rgbww_pwm[0].cw + pwm_ratio[4] * index;
			}
		}
		data_updated = 1;
		__disable_irq();
		tv->init_lightness =tv_arg->lightness;
		tv->rgbww_pwm_arr = rgbww_pwm; ////更新pwm数据
		__enable_irq();
	}
	
	memcpy(&tv->init_effect.effect_arg.tv_arg, tv_arg, sizeof(tv->init_effect.effect_arg.tv_arg));
	
	__disable_irq();
	if (data_updated == 1)
	{
		tv->data_init = 1;
		tv->deal_data_update = 1;
	}

	if (type == 1 || type == 2)
	{
		frq  = (tv_arg->frq <= 11) ? (tv_arg->frq) : 11;
		tv->init_min_work_time = tv_wt_min[frq];
		tv->init_max_work_time = tv_wt_max[frq];
	}
	else
	{
		tv->init_min_work_time = tv_arg->min_interval;
		tv->init_max_work_time = tv_arg->max_interval;
	}
	
	__enable_irq();
}

void TV_InitI(void *effect_mode, const Light_Effect* effect, Effect_PulseWightRGBWW *rgbww_pwm, uint8_t restart)
{
	TV_Init(effect_mode, effect, rgbww_pwm, restart, 1);
}

void Effect_TV_Init(const void* effect, uint8_t restart)
{
	Effect_Init(effect, &effect_tv, TV_InitI, restart);
}

void TV_InitII(void *effect_mode, const Light_Effect* effect, Effect_PulseWightRGBWW *rgbww_pwm, uint8_t restart)
{
	TV_Init(effect_mode, effect, rgbww_pwm, restart, 2);
}

void Effect_TVII_Init(const void* effect, uint8_t restart)
{
	Effect_Init(effect, &effect_tv, TV_InitII, restart);
}

void TV_InitIII(void *effect_mode, const Light_Effect* effect, Effect_PulseWightRGBWW *rgbww_pwm, uint8_t restart)
{
	TV_Init(effect_mode, effect, rgbww_pwm, restart, 3);
}

void Effect_TVIII_Init(const void* effect, uint8_t restart)
{
	Effect_Init(effect, &effect_tv, TV_InitIII, restart);
}
void TV_Process(Effect_TV *tv, Effect_PulseWightRGBWW *current_pwm, uint8_t tick, uint8_t restart)
{
	static uint8_t res = 0;
	uint16_t random;
	
	if (tv == NULL || current_pwm == NULL)
	{
		return;
	}
	if(res || restart == 1 || tv->proc_min_work_time != tv->init_min_work_time || tv->proc_max_work_time != tv->init_max_work_time)
	{
		tv->work_tick = Effect_Get_RandomRange_Uint32(tv->init_min_work_time, tv->init_max_work_time)/(float)tick; 			  //设置停留时间
		tv->whole_tick = tv->work_tick;
		tv->deal_data_update = 1;
		tv->proc_min_work_time = tv->init_min_work_time;
		tv->proc_max_work_time = tv->init_max_work_time;
	}
	if(tv->data_init)
	{
		tv->data_init = 0;
		tv->work_tick = tv->run_time + 1000;
		tv->whole_tick = tv->run_time + 1000;
	}
	if (res || restart == 1)// || tv->data_init)
	{
		float rate_temp[3] = {0.6f, 0.8f, 1.0f};
		
		random = Effect_Get_RandomRange_Int16(0, tv->max_color_index);
		memcpy(&tv->init_pwm, &tv->rgbww_pwm_arr[random], sizeof(Effect_PulseWightRGBWW));	//设置随机颜色
		random = Effect_Get_RandomRange_Int16(0, 2);
		tv->pwm.r = light_effect.min_pwm.r + (tv->init_pwm.r - (int32_t)light_effect.min_pwm.r) * rate_temp[random];
		tv->pwm.g = light_effect.min_pwm.g + (tv->init_pwm.g - (int32_t)light_effect.min_pwm.g) * rate_temp[random];
		tv->pwm.b = light_effect.min_pwm.b + (tv->init_pwm.b - (int32_t)light_effect.min_pwm.b) * rate_temp[random];
		tv->pwm.ww = light_effect.min_pwm.ww + (tv->init_pwm.ww - (int32_t)light_effect.min_pwm.ww) * rate_temp[random];
		tv->pwm.cw = light_effect.min_pwm.cw + (tv->init_pwm.cw - (int32_t)light_effect.min_pwm.cw) * rate_temp[random];
		tv->deal_data_update = 1;
		tv->data_init = 0;
	}
	
	if (res || restart == 1)
	{
		tv->run_time = 0;
		tv->deal_data_update = 0;
	}
	tv->liner.pwm_start = &tv->pwm;
	tv->liner.pwm_end = &tv->pwm;
	tv->liner.time_start = 0;
	tv->liner.time_end = tv->work_tick;
	tv->liner.whole_time = tv->whole_tick;
	tv->liner.pwm_current = &tv->pwm_current;
	tv->liner.run_time = &tv->run_time;
	tv->liner.data_update = &tv->deal_data_update;
	res = Effect_Pwm_LinearChange(&tv->liner); //保持亮度一定时间
	memcpy(current_pwm, &tv->pwm_current, sizeof(Effect_PulseWightRGBWW));
}

void Effect_TV_Process(uint8_t tick, uint8_t restart)
{
	static Effect_PulseWightRGBWW current_pwm = {0};
	
	if(restart)
	{
		memset(&current_pwm, 0, sizeof(Effect_PulseWightRGBWW));
	}
	
	TV_Process(&effect_tv, &current_pwm, tick, restart);
	Effect_SetPixel_PulseWight(current_pwm, 0xff);
	Effect_ActivePixel_PulseWight();
}

typedef struct{
	
	Effect_PulseWightRGBWW *rgbww_pwm_arr;
	float work_tick;
	uint8_t deal_data_update;
	uint8_t deal_data_init;
	Effect_PulseWightRGBWW pwm_current;
	Effect_PulseWightRGBWW pwm_min;
	Effect_PulseWightRGBWW pwm_max;
	float run_time;
	float rate;
	FirePwmChange_Struct fire_change;
	uint16_t  init_lightness;  	/*亮度<0-1000>*/
	uint16_t cct;
	uint16_t init_cct;
	uint8_t res;
	uint16_t min_work_time;
	uint16_t max_work_time;
	float min_rate;
	float max_rate;
	Light_Effect init_effect;
}Effect_Fire;
Effect_Fire effect_fire[PIXEL_NUM_MAX] = {0};

void Fire_Init(Effect_Fire *fire, const Light_Effect* effect, Effect_PulseWightRGBWW *rgbww_pwm, uint8_t restart, uint8_t type)
{
	uint8_t data_updated = 0;
	const Fire *fire_arg = &effect->effect_arg.fire_arg;
	//uint16_t cct_range[3][2] = {{FIRE_CCT_RANGE1_LOW, FIRE_CCT_RANGE1_UP}, {FIRE_CCT_RANGE2_LOW, FIRE_CCT_RANGE2_UP}, {FIRE_CCT_RANGE3_LOW, FIRE_CCT_RANGE3_UP}};
	
	if (fire == NULL || effect == NULL || rgbww_pwm == NULL)
	{
		return;
	}
	if (type == 2)
	{
		fire_arg = &effect->effect_arg.fire2_arg;
	}
	else if(type == 3)
	{
		fire_arg = &effect->effect_arg.fire3_arg;
	}
	fire_arg = type == 2 ? &effect->effect_arg.fire2_arg : &effect->effect_arg.fire3_arg;
	
	if (restart == 1 
		|| fire_arg->range1_mode != fire->init_effect.effect_arg.fire_arg.range1_mode
		|| fire_arg->range2_mode != fire->init_effect.effect_arg.fire_arg.range2_mode
		|| memcmp(&fire->init_effect.effect_arg.fire_arg.range1_arg, &fire_arg->range1_arg, sizeof(fire->init_effect.effect_arg.fire_arg.range1_arg)) != 0
		|| memcmp(&fire->init_effect.effect_arg.fire_arg.range2_arg, &fire_arg->range2_arg, sizeof(fire->init_effect.effect_arg.fire_arg.range2_arg)) != 0)//重新初始化参数
	{
		if (effect_color_init(&fire_arg->range1_arg, fire_arg->range1_mode, &rgbww_pwm[0])
			&& effect_color_init(&fire_arg->range2_arg, fire_arg->range2_mode, &rgbww_pwm[1]))
		{
			data_updated = 1;
			__disable_irq();
			fire->rgbww_pwm_arr = rgbww_pwm; ////更新pwm数据
			__enable_irq();
		}
	}
	memcpy(&fire->init_effect.effect_arg.fire_arg, fire_arg, sizeof(fire->init_effect.effect_arg.fire_arg));
	
	__disable_irq();
	if (data_updated == 1)
	{
		fire->deal_data_update = 1;
		fire->deal_data_init = 1;
	}
	if (type == 1 || type == 2)
	{
		uint8_t frg = fire_arg->frq;
		if (fire_arg->frq >= 11)
		{
			fire->min_work_time = fire_ct_min[10];
			fire->max_work_time = fire_ct_max[1];
			fire->min_rate = fire_rate_min[10];
			fire->max_rate = fire_rate_max[1];
		}
		else
		{
			fire->min_work_time = fire_ct_min[frg];
			fire->max_work_time = fire_ct_max[frg];
			fire->min_rate = fire_rate_min[frg];
			fire->max_rate = fire_rate_max[frg];
		}
	}
	else if(type == 3)
	{
		fire->min_work_time = fire_ct_max[1] - (fire_ct_max[1] - fire_ct_min[10]) * (fire_arg->times - 20)/180.0f;
		fire->max_work_time = fire->min_work_time;
		fire->max_rate = fire_rate_max[1] + (fire_rate_max[10] - fire_rate_max[1]) * (fire_arg->times - 20)/180.0f;
		fire->min_rate = -fire->max_rate;//fire_rate_max[1] - (fire_rate_max[1] - fire_rate_min[10]) * (fire_arg->times - 20)/180.0f;;
		//fire->max_rate = fire->min_rate;
	}
	__enable_irq();
}
void Fire_InitI(Effect_Fire *fire, const Light_Effect* effect, Effect_PulseWightRGBWW *rgbww_pwm, uint8_t restart)
{
	Fire_Init(fire, effect, rgbww_pwm, restart, 1);
}
void Effect_Fire_Init(const void* effect, uint8_t restart)
{
	uint8_t i;
	uint8_t j = 0;
	Effect_PulseWightRGBWW *buffer = NULL;
	
	//buffer = (Effect_PulseWightRGBWW *)malloc(sizeof(Effect_PulseWightRGBWW) * PWM_ARR_NUM);
	buffer = (Effect_PulseWightRGBWW *)effect_malloc();
	if (buffer)
	{
		for (j = 0; j < PIXEL_NUM_MAX; j++)
		{
			Fire_InitI(&effect_fire[j], (const Light_Effect*)effect, buffer, restart);
		}
		if (effect_fire[0].rgbww_pwm_arr == buffer)
		{
			//内存池管理
			for (i = 0; memory_pool[i] != NULL && i < MEMORY_POOL_NUM; i++)
			{
				effect_free(memory_pool[i]);//free(memory_pool[i]);
				memory_pool[i] = NULL;
			}
			memory_pool[0] = buffer;
		}
		else
		{
			effect_free(buffer);//free(buffer);
		}
	}
}

void Fire_InitII(Effect_Fire *fire, const Light_Effect* effect, Effect_PulseWightRGBWW *rgbww_pwm, uint8_t restart)
{
	Fire_Init(fire, effect, rgbww_pwm, restart, 2);
}
void Effect_FireII_Init(const void* effect, uint8_t restart)
{
	uint8_t i;
	uint8_t j = 0;
	Effect_PulseWightRGBWW *buffer = NULL;
	
	//buffer = (Effect_PulseWightRGBWW *)malloc(sizeof(Effect_PulseWightRGBWW) * PWM_ARR_NUM);
	buffer = (Effect_PulseWightRGBWW *)effect_malloc();
	if (buffer)
	{
		for (j = 0; j < PIXEL_NUM_MAX; j++)
		{
			Fire_InitII(&effect_fire[j], (const Light_Effect*)effect, buffer, restart);
		}
		if (effect_fire[0].rgbww_pwm_arr == buffer)
		{
			//内存池管理
			for (i = 0; memory_pool[i] != NULL && i < MEMORY_POOL_NUM; i++)
			{
				effect_free(memory_pool[i]);//free(memory_pool[i]);
				memory_pool[i] = NULL;
			}
			memory_pool[0] = buffer;
		}
		else
		{
			effect_free(buffer);//free(buffer);
		}
	}
}

void Fire_InitIII(Effect_Fire *fire, const Light_Effect* effect, Effect_PulseWightRGBWW *rgbww_pwm, uint8_t restart)
{
	Fire_Init(fire, effect, rgbww_pwm, restart, 3);
}
void Effect_FireIII_Init(const void* effect, uint8_t restart)
{
	uint8_t i;
	uint8_t j = 0;
	Effect_PulseWightRGBWW *buffer = NULL;
	
	//buffer = (Effect_PulseWightRGBWW *)malloc(sizeof(Effect_PulseWightRGBWW) * PWM_ARR_NUM);
	buffer = (Effect_PulseWightRGBWW *)effect_malloc();
	if (buffer)
	{
		for (j = 0; j < PIXEL_NUM_MAX; j++)
		{
			Fire_InitIII(&effect_fire[j], (const Light_Effect*)effect, buffer, restart);
		}
		if (effect_fire[0].rgbww_pwm_arr == buffer)
		{
			//内存池管理
			for (i = 0; memory_pool[i] != NULL && i < MEMORY_POOL_NUM; i++)
			{
				effect_free(memory_pool[i]);//free(memory_pool[i]);
				memory_pool[i] = NULL;
			}
			memory_pool[0] = buffer;
		}
		else
		{
			effect_free(buffer);//free(buffer);
		}
	}
}

void Fire_Process(Effect_Fire *fire, Effect_PulseWightRGBWW *current_pwm, uint8_t tick, uint8_t restart)
{
	if (fire == NULL || current_pwm == NULL)
	{
		return;
	}
	
	if (fire->res || restart == 1 || fire->deal_data_init == 1)
	{
		float rate = 0.7f;
		
		if (fire->deal_data_init == 1)
		{
			fire->deal_data_init = 0;
			rate = 0.9f;
		}
		fire->pwm_min.r = light_effect.min_pwm.r + (fire->rgbww_pwm_arr[0].r - (int32_t)light_effect.min_pwm.r) * rate;
		fire->pwm_min.g = light_effect.min_pwm.g + (fire->rgbww_pwm_arr[0].g - (int32_t)light_effect.min_pwm.g) * rate;
		fire->pwm_min.b = light_effect.min_pwm.b + (fire->rgbww_pwm_arr[0].b - (int32_t)light_effect.min_pwm.b) * rate;
		fire->pwm_min.ww = light_effect.min_pwm.ww + (fire->rgbww_pwm_arr[0].ww - (int32_t)light_effect.min_pwm.ww) * rate;
		fire->pwm_min.cw = light_effect.min_pwm.cw + (fire->rgbww_pwm_arr[0].cw - (int32_t)light_effect.min_pwm.cw) * rate;

		fire->pwm_max.r = fire->rgbww_pwm_arr[1].r;
		fire->pwm_max.g = fire->rgbww_pwm_arr[1].g;
		fire->pwm_max.b = fire->rgbww_pwm_arr[1].b;
		fire->pwm_max.ww = fire->rgbww_pwm_arr[1].ww;
		fire->pwm_max.cw = fire->rgbww_pwm_arr[1].cw;
		
		if (restart == 1)
		{
			fire->pwm_current.r = fire->rgbww_pwm_arr[1].r;
			fire->pwm_current.g = fire->rgbww_pwm_arr[1].g;
			fire->pwm_current.b = fire->rgbww_pwm_arr[1].b;
			fire->pwm_current.ww = fire->rgbww_pwm_arr[1].ww;
			fire->pwm_current.cw = fire->rgbww_pwm_arr[1].cw;
		}
		
		if (fire->res || restart == 1)
		{
			fire->work_tick = Effect_Get_RandomRange_Int16(fire->min_work_time, fire->max_work_time)/(float)tick;
			fire->rate = Effect_Get_RandomRange_Int16(fire->min_rate, fire->max_rate)/(float)tick;
			fire->run_time = 0;
		}
	}
	fire->fire_change.pwm_min = (uint16_t *)&fire->pwm_min;
	fire->fire_change.pwm_max = (uint16_t *)&fire->pwm_max;
	fire->fire_change.work_time = fire->work_tick;
	fire->fire_change.rate = fire->rate;
	fire->fire_change.pwm_current = (uint16_t *)&fire->pwm_current;
	fire->fire_change.run_time = &fire->run_time;
	fire->fire_change.data_update = &fire->deal_data_update;
	fire->res = Fire_Pwm_Change(&fire->fire_change);
	memcpy(current_pwm, &fire->pwm_current, sizeof(Effect_PulseWightRGBWW));
}

void Effect_Fire_Process(uint8_t tick, uint8_t restart)
{
	static Effect_PulseWightRGBWW current_pwm = {0};
	int i = 0;
	
	if(restart)
	{
		memset(&current_pwm, 0, sizeof(Effect_PulseWightRGBWW));
	}
	for (i = 0; i < PIXEL_NUM_MAX; i++)
	{
		Fire_Process(&effect_fire[i], &current_pwm, tick, restart);
		i = PIXEL_NUM_MAX == 1 ? 0xff : i;
		Effect_SetPixel_PulseWight(current_pwm, i);
	}
	Effect_ActivePixel_PulseWight();
}

typedef struct{
	
	Effect_PulseWightRGBWW *rgbww_pwm_arr;
	uint16_t  lightness;  		/*亮度<0-1000>*/
	uint16_t  init_lightness;  	/*亮度<0-1000>*/
	uint8_t   frq;  			/*<1-11>,11为Random*/
	float work_tick;
	uint8_t deal_data_update;
	uint8_t deal_data_init;
	Effect_PulseWightRGBWW pwm_current;
	Effect_PulseWightRGBWW pwm_min;
	Effect_PulseWightRGBWW pwm_max;
	float run_time;
	float rate;
	FirePwmChange_Struct fire_change;
	uint16_t cct;
	uint16_t init_cct;
	uint8_t res;
}Effect_Candle;
Effect_Candle effect_candle[PIXEL_NUM_MAX] = {0};
void Candle_Init(Effect_Candle *candle, const Light_Effect* effect, Effect_PulseWightRGBWW *rgbww_pwm, uint8_t restart)
{
	uint16_t  lightness;
	uint16_t cct = 0;
	uint16_t index = 0;
	uint8_t data_updated = 0;
	Effect_PulseWightRGBWW rgbww_pwm_temp = {0};
    const Candle *candle_arg = &effect->effect_arg.candle_arg;
    
	uint16_t cct_range[3][2] = {{CANDLE_CCT_RANGE1_LOW, CANDLE_CCT_RANGE1_UP}, {CANDLE_CCT_RANGE2_LOW, CANDLE_CCT_RANGE2_UP}, {CANDLE_CCT_RANGE3_LOW, CANDLE_CCT_RANGE3_UP}};
	
	if (candle == NULL || effect == NULL || rgbww_pwm == NULL)
	{
		return;
	}

	lightness = candle_arg->lightness;
	cct = candle_arg->cct < 2 ? candle_arg->cct : 2;
	if(restart == 1 || candle->init_lightness != lightness || candle->init_cct != cct)  //光效类型、亮度和cct变化时重新初始化参数
	{
		data_updated = 1;
		for(index = cct_range[cct][0]; index <= cct_range[cct][1]; index += 100) 		//FIRE_CCT_RANGE1_LOW ~ FIRE_CCT_RANGE1_UP
		{
			Effect_CCT2PulseWight(index, 0, lightness, &rgbww_pwm_temp);
			memcpy(&rgbww_pwm[(index - cct_range[cct][0])/100], &rgbww_pwm_temp, sizeof(rgbww_pwm_temp));
		}
		candle->init_lightness = lightness;
		candle->init_cct = cct;
		
		__disable_irq();
		candle->rgbww_pwm_arr = rgbww_pwm; ////更新pwm数据
		__enable_irq();
	}
	__disable_irq();
	if (data_updated == 1)
	{
		candle->deal_data_update = 1;
		candle->deal_data_init = 1;
	}
	candle->frq = (candle_arg->frq <= 11) ? (candle_arg->frq) : 11;
	candle->cct = cct;
	__enable_irq();
	
}

void Effect_Candle_Init(const void* effect, uint8_t restart)
{
	uint8_t i;
	uint8_t j = 0;
	Effect_PulseWightRGBWW *buffer = NULL;
	
    //buffer = (Effect_PulseWightRGBWW *)malloc(sizeof(Effect_PulseWightRGBWW) * PWM_ARR_NUM);
	buffer = (Effect_PulseWightRGBWW *)effect_malloc();
	if (buffer)
	{
		for (j = 0; j < PIXEL_NUM_MAX; j++)
		{
			Candle_Init(&effect_candle[j], (const Light_Effect*)effect, buffer, restart);
		}
		if (effect_candle[0].rgbww_pwm_arr == buffer)
		{
			//内存池管理
			for (i = 0; memory_pool[i] != NULL && i < MEMORY_POOL_NUM; i++)
			{
				effect_free(memory_pool[i]);//free(memory_pool[i]);
				memory_pool[i] = NULL;
			}
			memory_pool[0] = buffer;
		}
		else
		{
			effect_free(buffer);//free(buffer);
		}
	}
}

void Candle_Process(Effect_Candle *candle, Effect_PulseWightRGBWW *current_pwm, uint8_t tick, uint8_t restart)
{
	uint16_t random;
	
	if (candle == NULL || current_pwm == NULL)
	{
		return;
	}
	
	if (candle->res || restart == 1 || candle->deal_data_init == 1)
	{
		uint16_t cct_index_max;
		float rate = 0.7f;
		
		if (candle->cct == 0)
		{
			cct_index_max = (CANDLE_CCT_RANGE1_UP - CANDLE_CCT_RANGE1_LOW)/100;
		}
		else if (candle->cct == 1)
		{
			cct_index_max = (CANDLE_CCT_RANGE2_UP - CANDLE_CCT_RANGE2_LOW)/100;
		}
		else
		{
			cct_index_max = (CANDLE_CCT_RANGE3_UP - CANDLE_CCT_RANGE3_LOW)/100;
		}
		if (candle->deal_data_init == 1)
		{
			candle->deal_data_init = 0;
			rate = 0.9f;
		}
		candle->pwm_min.r = light_effect.min_pwm.r + (candle->rgbww_pwm_arr[0].r - (int32_t)light_effect.min_pwm.r) * rate;
		candle->pwm_min.g = light_effect.min_pwm.g + (candle->rgbww_pwm_arr[0].g - (int32_t)light_effect.min_pwm.g) * rate;
		candle->pwm_min.b = light_effect.min_pwm.b + (candle->rgbww_pwm_arr[0].b - (int32_t)light_effect.min_pwm.b) * rate;
		candle->pwm_min.ww = light_effect.min_pwm.ww + (candle->rgbww_pwm_arr[0].ww - (int32_t)light_effect.min_pwm.ww) * rate;
		candle->pwm_min.cw = light_effect.min_pwm.cw + (candle->rgbww_pwm_arr[0].cw - (int32_t)light_effect.min_pwm.cw) * rate;

		candle->pwm_max.r = candle->rgbww_pwm_arr[cct_index_max].r;
		candle->pwm_max.g = candle->rgbww_pwm_arr[cct_index_max].g;
		candle->pwm_max.b = candle->rgbww_pwm_arr[cct_index_max].b;
		candle->pwm_max.ww = candle->rgbww_pwm_arr[cct_index_max].ww;
		candle->pwm_max.cw = candle->rgbww_pwm_arr[cct_index_max].cw;
		
		if (restart == 1)
		{
			candle->pwm_current.r = candle->rgbww_pwm_arr[cct_index_max].r;
			candle->pwm_current.g = candle->rgbww_pwm_arr[cct_index_max].g;
			candle->pwm_current.b = candle->rgbww_pwm_arr[cct_index_max].b;
			candle->pwm_current.ww = candle->rgbww_pwm_arr[cct_index_max].ww;
			candle->pwm_current.cw = candle->rgbww_pwm_arr[cct_index_max].cw;
		}
		
		if (candle->res || restart == 1)
		{
			if (candle->frq == 11)
			{
				random = Effect_Get_RandomRange_Int16(1, 10);
			}
			else
			{
				random = candle->frq;
			}
			candle->work_tick = Effect_Get_RandomRange_Int16(candle_ct_min[random], candle_ct_max[random])/(float)tick;
			candle->rate = Effect_Get_RandomRange_Int16(candle_rate_min[random], candle_rate_max[random]);
			candle->run_time = 0;
		}
	}
	candle->fire_change.pwm_min = (uint16_t *)&candle->pwm_min;
	candle->fire_change.pwm_max = (uint16_t *)&candle->pwm_max;
	candle->fire_change.work_time = candle->work_tick;
	candle->fire_change.rate = candle->rate;
	candle->fire_change.pwm_current = (uint16_t *)&candle->pwm_current;
	candle->fire_change.run_time = &candle->run_time;
	candle->fire_change.data_update = &candle->deal_data_update;
	candle->res = Fire_Pwm_Change(&candle->fire_change);
	memcpy(current_pwm, &candle->pwm_current, sizeof(Effect_PulseWightRGBWW));
}

void Effect_Candle_Process(uint8_t tick, uint8_t restart)
{
	static Effect_PulseWightRGBWW current_pwm = {0};
	uint16_t i;
	
	if(restart)
	{
		memset(&current_pwm, 0, sizeof(Effect_PulseWightRGBWW));
	}
	for (i = 0; i < PIXEL_NUM_MAX; i++)
	{
		Candle_Process(&effect_candle[i], &current_pwm, tick, restart);
		i = PIXEL_NUM_MAX == 1 ? 0xff : i;
		Effect_SetPixel_PulseWight(current_pwm, i);
	}
	Effect_ActivePixel_PulseWight();
}

typedef struct{
	
	Effect_PulseWightRGBWW *rgbww_pwm_arr;
	uint8_t   frq;  			/*<1-11>,11为Random*/
	uint8_t   proc_frq;  		/*<1-11>,11为Random*/
	float whole_tick;
	float work_tick;
	uint8_t deal_data_update;
	Effect_PulseWightRGBWW* init_pwm;
	Effect_PulseWightRGBWW pwm_current;
	float run_time;
	LinearChange_Struct liner;
	Light_Effect init_effect;
}Effect_Strobe;

Effect_Strobe effect_strobe;
void Strobe_Init(void *effect_mode, const Light_Effect* effect, Effect_PulseWightRGBWW *rgbww_pwm, uint8_t restart, uint8_t type)
{
	Effect_Strobe *strobe = (Effect_Strobe *)effect_mode;
	const Strobe *strobe_arg;
	if (strobe == NULL || effect == NULL || rgbww_pwm == NULL)
	{
		return;
	}
	strobe_arg = type == 1 ? &effect->effect_arg.strobe_arg : &effect->effect_arg.strobe2_arg;
	if (restart == 1 || strobe_arg->mode != strobe->init_effect.effect_arg.strobe_arg.mode
		|| memcmp(&strobe->init_effect.effect_arg.strobe_arg.arg, &strobe_arg->arg, sizeof(strobe->init_effect.effect_arg.strobe_arg.arg)) != 0)//重新初始化参数
	{
		if (effect_color_init(&strobe_arg->arg, strobe_arg->mode, &rgbww_pwm[0]))
		{
			__disable_irq();
			strobe->rgbww_pwm_arr = rgbww_pwm; ////更新pwm数据
			__enable_irq();
		}
	}
	memcpy(&strobe->init_effect.effect_arg.strobe_arg, strobe_arg, sizeof(strobe->init_effect.effect_arg.strobe_arg));
	__disable_irq();
	strobe->deal_data_update = 0;
	strobe->frq = (strobe_arg->frq <= 11) ? (strobe_arg->frq) : 11;
	__enable_irq();
}
void StrobeI_Init(void *effect_mode, const Light_Effect* effect, Effect_PulseWightRGBWW *rgbww_pwm, uint8_t restart)
{
	Strobe_Init(effect_mode, effect, rgbww_pwm, restart, 1);
}
void Effect_Strobe_Init(const void* effect, uint8_t restart)
{
	Effect_Init(effect, &effect_strobe, StrobeI_Init, restart);
}
void StrobeII_Init(void *effect_mode, const Light_Effect* effect, Effect_PulseWightRGBWW *rgbww_pwm, uint8_t restart)
{
	Strobe_Init(effect_mode, effect, rgbww_pwm, restart, 2);
}
void Effect_StrobeII_Init(const void* effect, uint8_t restart)
{
	Effect_Init(effect, &effect_strobe, StrobeII_Init, restart);
}

void Strobe_Process(Effect_Strobe *strobe, Effect_PulseWightRGBWW *current_pwm, uint8_t tick, uint8_t restart)
{
	uint16_t interval = 0;
	uint16_t random = 0;
	uint8_t res = 0;
	
	if (strobe == NULL || current_pwm == NULL)
	{
		return;
	}

	strobe->work_tick = 30/(float)tick;     //亮的时间
	strobe->init_pwm = strobe->rgbww_pwm_arr;
	
	if(restart == 1 || strobe->proc_frq != strobe->frq)
	{	
		strobe->deal_data_update = 1;
		if(strobe->frq == 11)  //随机
		{
			random = Effect_Get_RandomRange_Int16(1, 10);
			interval = strobe_interval_max[random];
		}
		else
		{
			interval = strobe_interval_max[strobe->frq];
		}
		strobe->whole_tick = strobe->work_tick + interval/(float)tick;

		if(restart == 1)
		{
			strobe->run_time = 0;
			strobe->deal_data_update = 0;
		}
		restart = 0;
		strobe->proc_frq = strobe->frq;
	}
	strobe->liner.pwm_start = strobe->init_pwm;
	strobe->liner.pwm_end = strobe->init_pwm;
	strobe->liner.time_start = 0;
	strobe->liner.time_end = strobe->work_tick;
	strobe->liner.whole_time = strobe->whole_tick;
	strobe->liner.pwm_current = &strobe->pwm_current;
	strobe->liner.run_time = &strobe->run_time;
	strobe->liner.data_update = &strobe->deal_data_update;
	res = Effect_Pwm_LinearChange(&strobe->liner); //保持亮度一定时间
	if(res) //该工作周期结束，重新设置工作时间
	{
		if(strobe->frq == 11)  //随机
		{
			random = Effect_Get_RandomRange_Int16(1, 10);
			interval = strobe_interval_max[random];
		}
		else
		{
			interval = strobe_interval_max[strobe->frq];
		}
		strobe->whole_tick = strobe->work_tick + interval/(float)tick;
		strobe->run_time = 0;
	}
	memcpy(current_pwm, &strobe->pwm_current, sizeof(Effect_PulseWightRGBWW));
}

void Effect_Strobe_Process(uint8_t tick, uint8_t restart)
{
	static Effect_PulseWightRGBWW current_pwm = {0};
	
	if(restart)
	{
		memset(&current_pwm, 0, sizeof(Effect_PulseWightRGBWW));
	}
	
	Strobe_Process(&effect_strobe, &current_pwm, tick, restart);
	Effect_SetPixel_PulseWight(current_pwm, 0xff);
	Effect_ActivePixel_PulseWight();
}

typedef struct{
	
	Effect_PulseWightRGBWW *rgbww_pwm_arr;
	uint8_t   frq;  			/*<1-11>,11为Random*/
	uint8_t   proc_frq;  		/*<1-11>,11为Random*/
	float whole_tick;
	float work_tick;
	uint8_t deal_data_update;
	Effect_PulseWightRGBWW* init_pwm;
	Effect_PulseWightRGBWW pwm_current;
	float run_time;
	LinearChange_Struct liner;
	uint8_t run_stage;
	Effect_TriggerType trigger_type;
	Light_Effect init_effect;
}Effect_Explosion;

Effect_Explosion effect_explosion;
void Explosion_Init(void *effect_mode, const Light_Effect* effect, Effect_PulseWightRGBWW *rgbww_pwm, uint8_t restart, uint8_t type)
{
	uint8_t data_updated = 0;
	Effect_Explosion *explosion = (Effect_Explosion *)effect_mode;
	const Explosion *explosion_arg;
	if (explosion == NULL || effect == NULL || rgbww_pwm == NULL)
	{
		return;
	}
	explosion_arg = type == 1 ? &effect->effect_arg.explosion_arg : &effect->effect_arg.explosion2_arg;
	if (restart == 1 || explosion_arg->mode != explosion->init_effect.effect_arg.explosion_arg.mode
		|| memcmp(&explosion->init_effect.effect_arg.explosion_arg.arg, &explosion_arg->arg, sizeof(explosion->init_effect.effect_arg.explosion_arg.arg)) != 0)//重新初始化参数
	{
		if (effect_color_init(&explosion_arg->arg, explosion_arg->mode, &rgbww_pwm[0]))
		{
			data_updated = 1;
			__disable_irq();
			explosion->rgbww_pwm_arr = rgbww_pwm; ////更新pwm数据
			__enable_irq();
		}
	}
	memcpy(&explosion->init_effect.effect_arg.explosion_arg, explosion_arg, sizeof(explosion->init_effect.effect_arg.explosion_arg));
	__disable_irq();
	explosion->trigger_type = explosion_arg->trigger;
	if(data_updated == 1)
	{
		explosion->deal_data_update = 1;
	}
	explosion->frq = (explosion_arg->frq <= 11) ? (explosion_arg->frq) : 11;
	__enable_irq();
}

void Explosion_InitI(void *effect_mode, const Light_Effect* effect, Effect_PulseWightRGBWW *rgbww_pwm, uint8_t restart)
{
	Explosion_Init(effect_mode, effect, rgbww_pwm, restart, 1);
}
void Effect_Explosion_Init(const void* effect, uint8_t restart)
{
	Effect_Init(effect, &effect_explosion, Explosion_InitI, restart);
}

void Explosion_InitII(void *effect_mode, const Light_Effect* effect, Effect_PulseWightRGBWW *rgbww_pwm, uint8_t restart)
{
	Explosion_Init(effect_mode, effect, rgbww_pwm, restart, 2);
}
void Effect_ExplosionII_Init(const void* effect, uint8_t restart)
{
	Effect_Init(effect, &effect_explosion, Explosion_InitII, restart);
}

void Explosion_Process(Effect_Explosion *explosion, Effect_PulseWightRGBWW *current_pwm, uint8_t tick, uint8_t restart)
{
	uint16_t random = 0;
	uint8_t res = 0;
	Effect_PulseWightRGBWW off_pwm = {light_effect.min_pwm.r, light_effect.min_pwm.g,  
										light_effect.min_pwm.b, light_effect.min_pwm.ww, light_effect.min_pwm.cw};  //默认pwm值，默认为最低亮度，即灭
	
	if (explosion == NULL || current_pwm == NULL)
	{
		return;
	}
	explosion->init_pwm = explosion->rgbww_pwm_arr;
	
	if(restart == 1 || explosion->proc_frq != explosion->frq || explosion->trigger_type == Trigger_Once)
	{
		explosion->deal_data_update = 1;

		if (explosion->frq == 11)
		{
			random = Effect_Get_RandomRange_Int16(1, 10);
		}
		else
		{
			random =explosion->frq;
		}
		explosion->work_tick = explosion_wt[random]/(float)tick;
		explosion->whole_tick = explosion->work_tick;
		
		if (restart == 1 && explosion->trigger_type == Trigger_None)
		{
			explosion->run_stage = 1;
			memset(&explosion->pwm_current, 0, sizeof(Effect_PulseWightRGBWW));
			memset(current_pwm, 0, sizeof(Effect_PulseWightRGBWW));
		}
		else if(restart == 1 || explosion->trigger_type == Trigger_Once)
		{
			explosion->run_stage = 0;
			explosion->run_time = 0;
			explosion->trigger_type = Trigger_None;
			explosion->deal_data_update = 0;
		}
		restart = 0;
		explosion->proc_frq = explosion->frq;
	}
	else
	{
		if(explosion->run_stage == 0)
		{
			explosion->liner.pwm_start = explosion->init_pwm;
			explosion->liner.pwm_end = &off_pwm;
			explosion->liner.time_start = 0;
			explosion->liner.time_end = explosion->work_tick;
			explosion->liner.whole_time = explosion->whole_tick;
			explosion->liner.pwm_current = &explosion->pwm_current;
			explosion->liner.run_time = &explosion->run_time;
			explosion->liner.data_update = &explosion->deal_data_update;
			res = Effect_Pwm_LinearChange(&explosion->liner); //逐渐变灭
			if(res) 	//该工作周期结束，重新设置工作时间
			{
				explosion->run_stage = 1;
				explosion->trigger_type = Trigger_None;
			}
			memcpy(current_pwm, &explosion->pwm_current, sizeof(Effect_PulseWightRGBWW));
		}
		else
		{
		}
	}
}

void Effect_Explosion_Process(uint8_t tick, uint8_t restart)
{
	static Effect_PulseWightRGBWW current_pwm = {0};
	
	if(restart)
	{
		memset(&current_pwm, 0, sizeof(Effect_PulseWightRGBWW));
	}
	
	Explosion_Process(&effect_explosion, &current_pwm, tick, restart);
	Effect_SetPixel_PulseWight(current_pwm, 0xff);
	Effect_ActivePixel_PulseWight();
}

typedef struct{
	
	Effect_PulseWightRGBWW *rgbww_pwm_arr;
	uint8_t   frq;  			/*<1-11>,11为Random*/
	uint8_t   proc_frq;  		/*<1-11>,11为Random*/
	float whole_tick;
	float work_tick;
	uint8_t speed;
	uint8_t proc_speed;
	uint8_t deal_data_update;
	Effect_PulseWightRGBWW* init_pwm;
	Effect_PulseWightRGBWW pwm_current;
	float run_time;
	LinearChange_Struct liner;
	uint8_t run_stage;
	uint8_t flash_cnt;
	uint8_t flash_num;
	uint16_t interval;
	
	uint16_t init_min_work_time;
	uint16_t init_max_work_time;
	uint16_t proc_min_work_time;
	uint16_t proc_max_work_time;
	uint16_t init_min_interval_time;
	uint16_t init_max_interval_time;
	uint16_t proc_min_interval_time;
	uint16_t proc_max_interval_time;
	uint16_t fault_pr;
	Light_Effect init_effect;
}Effect_FaultBulb;

Effect_FaultBulb effect_faultbulb;
void FaultBulb_Init(void *effect_mode, const Light_Effect* effect, Effect_PulseWightRGBWW *rgbww_pwm, uint8_t restart, uint8_t type)
{
	uint8_t data_updated = 0;
	Effect_FaultBulb *faultbulb = (Effect_FaultBulb *)effect_mode;
	uint8_t frq;
	const FaultBulb *faultBulb_arg = &effect->effect_arg.faultBulb_arg;
	if (faultbulb == NULL || effect == NULL || rgbww_pwm == NULL)
	{
		return;
	}
	if (type == 2)
	{
		faultBulb_arg = &effect->effect_arg.faultBulb2_arg;
	}
	else if (type == 3)
	{
		faultBulb_arg = &effect->effect_arg.faultBulb3_arg;
	}
	if (restart == 1 || faultBulb_arg->mode != faultbulb->init_effect.effect_arg.faultBulb_arg.mode
		|| memcmp(&faultbulb->init_effect.effect_arg.faultBulb_arg.arg, &faultBulb_arg->arg, sizeof(faultbulb->init_effect.effect_arg.faultBulb_arg.arg)) != 0)//重新初始化参数
	{
		if (effect_color_init(&faultBulb_arg->arg, faultBulb_arg->mode, &rgbww_pwm[0]))
		{
			data_updated = 1;
			__disable_irq();
			faultbulb->rgbww_pwm_arr = rgbww_pwm; ////更新pwm数据
			__enable_irq();
		}
		
		data_updated = 1;
	}
	memcpy(&faultbulb->init_effect.effect_arg.faultBulb_arg, faultBulb_arg, sizeof(faultbulb->init_effect.effect_arg.faultBulb_arg));
	__disable_irq();
	if(data_updated == 1)
	{
		faultbulb->deal_data_update = 1;
	}
	
	if (type == 1 || type == 2)
	{
		if (faultBulb_arg->frq >= 11)
		{
			frq = 11;
			faultbulb->init_min_work_time = fault_work_min[10];
			faultbulb->init_max_work_time = fault_work_max[1];
		}
		else
		{
			frq = faultBulb_arg->frq;
			faultbulb->init_min_work_time = fault_work_min[frq];
			faultbulb->init_max_work_time = fault_work_max[frq];
		}
//		if (type == 1)
//		{
//			faultbulb->init_min_interval_time = 150;
//			faultbulb->init_max_interval_time = 150;
//		}
//		else
		{
			if (faultBulb_arg->speed >= 11)
			{
				faultbulb->init_min_interval_time = fault_interval_max[10];
				faultbulb->init_max_interval_time = fault_interval_max[1];
			}
			else
			{
				faultbulb->init_min_interval_time = fault_interval_max[faultBulb_arg->speed];
				faultbulb->init_max_interval_time = fault_interval_max[faultBulb_arg->speed];
			}
		}
		faultbulb->fault_pr = fault_pr[frq];
	}
	else
	{
		faultbulb->init_min_work_time = faultBulb_arg->min_interval;
		faultbulb->init_max_work_time = faultBulb_arg->max_interval;
		faultbulb->init_min_interval_time = 150;
		faultbulb->init_max_interval_time = 150;
		faultbulb->fault_pr = 950;
	}
	__enable_irq();
}

void FaultBulb_InitI(void *effect_mode, const Light_Effect* effect, Effect_PulseWightRGBWW *rgbww_pwm, uint8_t restart)
{
	FaultBulb_Init(effect_mode, effect, rgbww_pwm, restart, 1);
}

void Effect_FaultBulb_Init(const void* effect, uint8_t restart)
{
	Effect_Init(effect, &effect_faultbulb, FaultBulb_InitI, restart);
}

void FaultBulb_InitII(void *effect_mode, const Light_Effect* effect, Effect_PulseWightRGBWW *rgbww_pwm, uint8_t restart)
{
	FaultBulb_Init(effect_mode, effect, rgbww_pwm, restart, 2);
}

void Effect_FaultBulbII_Init(const void* effect, uint8_t restart)
{
	Effect_Init(effect, &effect_faultbulb, FaultBulb_InitII, restart);
}

void FaultBulb_InitIII(void *effect_mode, const Light_Effect* effect, Effect_PulseWightRGBWW *rgbww_pwm, uint8_t restart)
{
	FaultBulb_Init(effect_mode, effect, rgbww_pwm, restart, 3);
}

void Effect_FaultBulbIII_Init(const void* effect, uint8_t restart)
{
	Effect_Init(effect, &effect_faultbulb, FaultBulb_InitIII, restart);
}

void FaultBulb_Process(Effect_FaultBulb *fault, Effect_PulseWightRGBWW *current_pwm, uint8_t tick, uint8_t restart)
{
	static uint8_t res = 0;

	if (fault == NULL || current_pwm == NULL)
	{
		return;
	}
	
	fault->init_pwm = fault->rgbww_pwm_arr;
	if(res || restart == 1 || fault->proc_min_work_time != fault->init_min_work_time || fault->proc_max_work_time != fault->init_max_work_time
		|| fault->proc_min_interval_time != fault->init_min_interval_time || fault->proc_max_interval_time != fault->init_max_interval_time) 	//该工作周期结束，重新设置工作时间
	{
		fault->deal_data_update = 1;
		if (restart == 1 || res)
		{
			fault->run_time = 0;
			fault->deal_data_update = 0;
		}
		if (fault->flash_num == 0 || restart == 1 || fault->proc_min_work_time != fault->init_min_work_time || fault->proc_max_work_time != fault->init_max_work_time
			|| fault->proc_min_interval_time != fault->init_min_interval_time || fault->proc_max_interval_time != fault->init_max_interval_time)
		{
			fault->work_tick = Effect_Get_RandomRange_Int16(fault->init_min_work_time, fault->init_max_work_time)/(float)tick;
			fault->interval = Effect_Get_RandomRange_Int16(fault->init_min_interval_time, fault->init_max_interval_time);
			if(Effect_Get_RandomRange_Int16(1, 1000) > fault->fault_pr)  //小于fault_pr[effect_frq]则单闪
			{
				fault->flash_num = Effect_Get_RandomRange_Int16(2, 32);
			}
		}
		else if (fault->flash_num != 0)
		{
			fault->interval = 50;
			fault->work_tick = 50/(float)tick;
			fault->flash_num--;
		}
		
		fault->whole_tick = fault->work_tick + fault->interval/(float)tick;
		fault->proc_min_work_time = fault->init_min_work_time;
		fault->proc_max_work_time = fault->init_max_work_time;
		fault->proc_min_interval_time = fault->init_min_interval_time;
		fault->proc_max_interval_time = fault->init_max_interval_time;
	}
	fault->liner.pwm_start = fault->init_pwm;
	fault->liner.pwm_end = fault->init_pwm;
	fault->liner.time_start = 0;
	fault->liner.time_end = fault->work_tick;
	fault->liner.whole_time = fault->whole_tick;
	fault->liner.pwm_current = &fault->pwm_current;
	fault->liner.run_time = &fault->run_time;
	fault->liner.data_update = &fault->deal_data_update;
	res = Effect_Pwm_LinearChange(&fault->liner);
	memcpy(current_pwm, &fault->pwm_current, sizeof(Effect_PulseWightRGBWW));
}
void Effect_FaultBulb_Process(uint8_t tick, uint8_t restart)
{
	static Effect_PulseWightRGBWW current_pwm = {0};
	
	if(restart)
	{
		memset(&current_pwm, 0, sizeof(Effect_PulseWightRGBWW));
	}
	
	FaultBulb_Process(&effect_faultbulb, &current_pwm, tick, restart);
	Effect_SetPixel_PulseWight(current_pwm, 0xff);
	Effect_ActivePixel_PulseWight();
}
typedef struct{
	
	Effect_PulseWightRGBWW *rgbww_pwm_arr;
	Light_Effect* effect_arg;
	float whole_tick;
	float work_tick;
	uint8_t deal_data_update;
	Effect_PulseWightRGBWW* init_pwm;
	Effect_PulseWightRGBWW pwm_current;
	float run_time;
	LinearChange_Struct liner;
	uint8_t run_stage;
	uint16_t interval;
	uint16_t init_min_interval;
	uint16_t init_max_interval;
	float init_min_work_time;
	float init_max_work_time;
	uint16_t proc_min_interval;
	uint16_t proc_max_interval;
	float proc_min_work_time;
	float proc_max_work_time;
	Light_Effect init_effect;
}Effect_Pulsing;
Effect_Pulsing effect_pulsing;
void Pulsing_Init(void *effect_mode, const Light_Effect* effect, Effect_PulseWightRGBWW *rgbww_pwm, uint8_t restart, uint8_t type)
{
	uint8_t data_updated = 0;
	float whole_time;
	Effect_Pulsing *pulsing = (Effect_Pulsing *)effect_mode;
	uint8_t   frq;
	const Pulsing *pulsing_arg = &effect->effect_arg.pulsing_arg;
	
	if (pulsing == NULL || effect == NULL || rgbww_pwm == NULL)
	{
		return;
	}
	if (type == 2)
	{
		pulsing_arg = &effect->effect_arg.pulsing2_arg;
	}
	else if (type == 3)
	{
		pulsing_arg = &effect->effect_arg.pulsing3_arg;
	}
	if (restart == 1 || pulsing_arg->mode != pulsing->init_effect.effect_arg.pulsing_arg.mode
		|| memcmp(&pulsing->init_effect.effect_arg.pulsing_arg.arg, &pulsing_arg->arg, sizeof(pulsing->init_effect.effect_arg.pulsing_arg.arg)) != 0)//重新初始化参数
	{
		if (effect_color_init(&pulsing_arg->arg, pulsing_arg->mode, &rgbww_pwm[0]))
		{
			data_updated = 1;
			__disable_irq();
			pulsing->rgbww_pwm_arr = rgbww_pwm; ////更新pwm数据
			__enable_irq();
		}
	}
	memcpy(&pulsing->init_effect.effect_arg.pulsing_arg, pulsing_arg, sizeof(pulsing->init_effect.effect_arg.pulsing_arg));
	__disable_irq();
	
	if (type == 1 || type == 2)
	{
		uint8_t frq_index_min, frq_index_max;
		
		if (pulsing_arg->frq >= 11)
		{
			frq = 11;
			frq_index_min = 1;
			frq_index_max = 10;
		}
		else
		{
			frq = pulsing_arg->frq;
			frq_index_min = frq;
			frq_index_max = frq;
		}
//		if (type == 1)
//		{
//			pulsing->init_min_interval = pluse_interval[frq_index_max][frq_index_max];
//			pulsing->init_max_interval = pluse_interval[frq_index_min][frq_index_min];
//			pulsing->init_min_work_time = pluse_worktime[frq_index_max][frq_index_max];
//			pulsing->init_max_work_time = pluse_worktime[frq_index_min][frq_index_min];
//		}
//		else
		{
			uint8_t speed, speed_index_min, speed_index_max;
			if (pulsing_arg->speed >= 11)
			{
				speed = 11;
				speed_index_min = 1;
				speed_index_max = 10;
			}
			else
			{
				speed = pulsing_arg->speed;
				speed_index_min = speed;
				speed_index_max = speed;
			}
			pulsing->init_min_interval = pluse_interval[frq_index_max][speed_index_max];
			pulsing->init_max_interval = pluse_interval[frq_index_min][speed_index_min];
			pulsing->init_min_work_time = pluse_worktime[frq_index_max][speed_index_max];
			pulsing->init_max_work_time = pluse_worktime[frq_index_min][speed_index_min];
		}
	}
	else if(type == 3)
	{
		uint8_t flash_times = pulsing_arg->flash_times < 1 ? 1 : (pulsing_arg->flash_times > 200 ? 200 : pulsing_arg->flash_times);
		
		whole_time = 60000.0f/flash_times;
		pulsing->init_min_interval = whole_time/3;
		pulsing->init_max_interval = pulsing->init_min_interval;
		pulsing->init_min_work_time = pulsing->init_min_interval * 2;
		pulsing->init_max_work_time = pulsing->init_min_work_time;
	}
	if(data_updated == 1)
	{
		pulsing->deal_data_update = 1;
	}
	__enable_irq();
}

void Pulsing_InitI(void *effect_mode, const Light_Effect* effect, Effect_PulseWightRGBWW *rgbww_pwm, uint8_t restart)
{
	Pulsing_Init(effect_mode, effect, rgbww_pwm, restart, 1);
}

void Effect_Pulsing_Init(const void* effect, uint8_t restart)
{
	Effect_Init(effect, &effect_pulsing, Pulsing_InitI, restart);
}
void Pulsing_InitII(void *effect_mode, const Light_Effect* effect, Effect_PulseWightRGBWW *rgbww_pwm, uint8_t restart)
{
	Pulsing_Init(effect_mode, effect, rgbww_pwm, restart, 2);
}

void Effect_PulsingII_Init(const void* effect, uint8_t restart)
{
	Effect_Init(effect, &effect_pulsing, Pulsing_InitII, restart);
}
void Pulsing_InitIII(void *effect_mode, const Light_Effect* effect, Effect_PulseWightRGBWW *rgbww_pwm, uint8_t restart)
{
	Pulsing_Init(effect_mode, effect, rgbww_pwm, restart, 3);
}

void Effect_PulsingIII_Init(const void* effect, uint8_t restart)
{
	Effect_Init(effect, &effect_pulsing, Pulsing_InitIII, restart);
}
void Pulsing_Process(Effect_Pulsing *pulsing, Effect_PulseWightRGBWW *current_pwm, uint8_t tick, uint8_t restart)
{
	static uint8_t res = 0;
	Effect_PulseWightRGBWW off_pwm = {light_effect.min_pwm.r, light_effect.min_pwm.g, light_effect.min_pwm.b, light_effect.min_pwm.ww, light_effect.min_pwm.cw};  //默认pwm值，默认为最低亮度，即灭
	
	if (pulsing == NULL || current_pwm == NULL)
	{
		return;
	}
	pulsing->init_pwm = pulsing->rgbww_pwm_arr;
	if (pulsing->proc_min_interval != pulsing->init_min_interval
		|| pulsing->proc_max_interval != pulsing->init_max_interval
		|| pulsing->proc_min_work_time != pulsing->init_min_work_time
		|| pulsing->proc_max_work_time != pulsing->init_max_work_time
		|| (res == 1 && (pulsing->run_stage == 1))
		|| restart == 1)
	{
		pulsing->interval = Effect_Get_RandomRange_Int16(pulsing->init_min_interval, pulsing->init_max_interval);
		pulsing->work_tick = Effect_Get_RandomRange_Int16(pulsing->init_min_work_time, pulsing->init_max_work_time)/(float)tick;
		pulsing->whole_tick = pulsing->work_tick + pulsing->interval/(float)tick;
		
		if (res != 1)
		{
			pulsing->deal_data_update = 1;
		}
		if(restart == 1)
		{
			pulsing->run_stage = 0;
			pulsing->run_time = 0;
			pulsing->deal_data_update = 0;
		}
		pulsing->proc_min_interval = pulsing->init_min_interval;
		pulsing->proc_max_interval = pulsing->init_max_interval;
		pulsing->proc_min_work_time = pulsing->init_min_work_time;
		pulsing->proc_max_work_time = pulsing->init_max_work_time;
	}
	
	if (res == 1)
	{
		res = 0;
		if (pulsing->run_stage == 1)
		{
			pulsing->run_time = 0;
			pulsing->run_stage = 0;
		}
		else
		{
			pulsing->run_stage = 1;
		}
	}
	if(pulsing->run_stage == 0) //第一阶段，逐渐增加亮度
	{
		pulsing->liner.pwm_start = &off_pwm;
		pulsing->liner.pwm_end = pulsing->init_pwm;
		pulsing->liner.time_start = 0;
		pulsing->liner.time_end = pulsing->work_tick/2;
		pulsing->liner.whole_time = pulsing->work_tick/2;
	}
	else  //第二阶段，逐渐减小亮度
	{
		pulsing->liner.pwm_start = pulsing->init_pwm;
		pulsing->liner.pwm_end = &off_pwm;
		pulsing->liner.time_start = pulsing->work_tick/2;
		pulsing->liner.time_end = pulsing->work_tick;
		pulsing->liner.whole_time = pulsing->whole_tick;
	}
	pulsing->liner.pwm_current = &pulsing->pwm_current;
	pulsing->liner.run_time = &pulsing->run_time;
	pulsing->liner.data_update = &pulsing->deal_data_update;
	res = Effect_Pwm_LinearChange(&pulsing->liner);  //逐渐亮
	memcpy(current_pwm, &pulsing->pwm_current, sizeof(Effect_PulseWightRGBWW));
}
void Effect_Pulsing_Process(uint8_t tick, uint8_t restart)
{
	static Effect_PulseWightRGBWW current_pwm = {0};
	
	if(restart)
	{
		memset(&current_pwm, 0, sizeof(Effect_PulseWightRGBWW));
	}
	
	Pulsing_Process(&effect_pulsing, &current_pwm, tick, restart);
	Effect_SetPixel_PulseWight(current_pwm, 0xff);
	Effect_ActivePixel_PulseWight();
}

typedef struct{
	
	Effect_PulseWightRGBWW *rgbww_pwm_arr;
	uint8_t   frq;  			/*<1-11>,11为Random*/
	uint8_t   proc_frq;  		/*<1-11>,11为Random*/
	Light_Effect* effect_arg;
	float whole_tick;
	float work_tick;
	uint8_t deal_data_update;
	Effect_PulseWightRGBWW pwm_current;
	float run_time;
	LinearChange_Struct liner;
	uint8_t run_stage;
	uint16_t interval;
	Effect_PulseWightRGBWW pwm_max;
	Effect_PulseWightRGBWW pwm_min;
	uint16_t min;
	Light_Effect init_effect;
}Effect_Welding;
Effect_Welding effect_welding;
void Welding_Init(void *effect_mode, const Light_Effect* effect, Effect_PulseWightRGBWW *rgbww_pwm, uint8_t restart, uint8_t type)
{
	uint8_t data_updated = 0;
	Effect_Welding *welding = (Effect_Welding *)effect_mode;
	const Welding *welding_arg;
	if (welding == NULL || effect == NULL || rgbww_pwm == NULL)
	{
		return;
	}
	welding_arg = type == 1 ? &effect->effect_arg.welding_arg : &effect->effect_arg.welding2_arg;
	if (restart == 1 || welding_arg->mode != welding->init_effect.effect_arg.welding_arg.mode
		|| memcmp(&welding->init_effect.effect_arg.welding_arg.arg, &welding_arg->arg, sizeof(welding->init_effect.effect_arg.welding_arg.arg)) != 0)//重新初始化参数
	{
		if (effect_color_init(&welding_arg->arg, welding_arg->mode, &rgbww_pwm[0]))
		{
			data_updated = 1;
			__disable_irq();
			welding->rgbww_pwm_arr = rgbww_pwm; ////更新pwm数据
			__enable_irq();
		}
	}
	memcpy(&welding->init_effect.effect_arg.welding_arg, welding_arg, sizeof(welding->init_effect.effect_arg.welding_arg));
	__disable_irq();
	if(data_updated == 1)
	{
		welding->deal_data_update = 1;
	}
	welding->frq = (welding_arg->frq <= 11) ? (welding_arg->frq) : 11;
	welding->min = welding_arg->min;
	__enable_irq();
}

void WeldingI_Init(void *effect_mode, const Light_Effect* effect, Effect_PulseWightRGBWW *rgbww_pwm, uint8_t restart)
{
	Welding_Init(effect_mode, effect, rgbww_pwm, restart, 1);
}
void Effect_Welding_Init(const void* effect, uint8_t restart)
{
	Effect_Init(effect, &effect_welding, WeldingI_Init, restart);
}

void WeldingII_Init(void *effect_mode, const Light_Effect* effect, Effect_PulseWightRGBWW *rgbww_pwm, uint8_t restart)
{
	Welding_Init(effect_mode, effect, rgbww_pwm, restart, 2);
}
void Effect_WeldingII_Init(const void* effect, uint8_t restart)
{
	Effect_Init(effect, &effect_welding, WeldingII_Init, restart);
}
void Welding_Process(Effect_Welding *welding, Effect_PulseWightRGBWW *current_pwm, uint8_t tick, uint8_t restart)
{
	uint16_t random = 0;
	static uint8_t res = 0;
	
	if (welding == NULL || current_pwm == NULL)
	{
		return;
	}
	if(res || restart == 1 || welding->proc_frq != welding->frq)
	{
		if (welding->proc_frq != welding->frq)
		{
			welding->deal_data_update = 1;
			welding->proc_frq = welding->frq;
		}
		if(restart == 1)
		{
			welding->run_stage = 1;
			welding->deal_data_update = 0;
		}
		if(welding->run_stage == 0)
		{
			welding->pwm_min.r = light_effect.min_pwm.r + (welding->rgbww_pwm_arr->r - (int32_t)light_effect.min_pwm.r) * 0.8f * 0.001f * welding->min;
			welding->pwm_min.g = light_effect.min_pwm.g + (welding->rgbww_pwm_arr->g - (int32_t)light_effect.min_pwm.g) * 0.8f * 0.001f * welding->min;
			welding->pwm_min.b = light_effect.min_pwm.b + (welding->rgbww_pwm_arr->b - (int32_t)light_effect.min_pwm.b) * 0.8f * 0.001f * welding->min;
			welding->pwm_min.ww = light_effect.min_pwm.ww + (welding->rgbww_pwm_arr->ww - (int32_t)light_effect.min_pwm.ww) * 0.8f * 0.001f * welding->min;
			welding->pwm_min.cw = light_effect.min_pwm.cw + (welding->rgbww_pwm_arr->cw - (int32_t)light_effect.min_pwm.cw) * 0.8f * 0.001f * welding->min;

			if (welding->frq == 11)
			{
				random = Effect_Get_RandomRange_Int16(1, 10);
			}
			else
			{
				random = welding->frq;
			}
			if(Effect_Get_RandomRange_Int16(1, 1000) < welding_r[random])
			{
				welding->work_tick = Effect_Get_RandomRange_Int16(welding_rt_min[random], welding_rt_max[random])/(float)tick;
			}
			else
			{
				welding->work_tick = Effect_Get_RandomRange_Int16(welding_interval_min[random], welding_interval_max[random])/(float)tick;
			}
			
			welding->run_stage = 1;
		}
		else
		{
			welding->pwm_max.r = welding->rgbww_pwm_arr->r;
			welding->pwm_max.g = welding->rgbww_pwm_arr->g;
			welding->pwm_max.b = welding->rgbww_pwm_arr->b;
			welding->pwm_max.ww = welding->rgbww_pwm_arr->ww;
			welding->pwm_max.cw = welding->rgbww_pwm_arr->cw;
			welding->work_tick = welding_wt/(float)tick;//Effect_Get_RandomRange_Int16(Welding_WTMin[Effect_Frq], Welding_WTMax[Effect_Frq]);	//第一阶段，计算随机高亮工作时间
			welding->run_stage = 0;
		}
		welding->whole_tick = welding->work_tick;
		welding->run_time = 0;
		res = 0;
	}
	
	if(welding->run_stage == 0)  //第一阶段，高亮阶段，保持高亮状态
	{
		welding->liner.pwm_start = &welding->pwm_max;
		welding->liner.pwm_end = &welding->pwm_max;
	}
	else   //第二阶段，低亮阶段
	{
		welding->liner.pwm_start = &welding->pwm_min;
		welding->liner.pwm_end = &welding->pwm_min;
	}
	welding->liner.time_start = 0;
	welding->liner.time_end = welding->work_tick;
	welding->liner.whole_time = welding->whole_tick;
	welding->liner.pwm_current = &welding->pwm_current;
	welding->liner.run_time = &welding->run_time;
	welding->liner.data_update = &welding->deal_data_update;
	res = Effect_Pwm_LinearChange(&welding->liner);
	memcpy(current_pwm, &welding->pwm_current, sizeof(Effect_PulseWightRGBWW));
}

void Effect_Welding_Process(uint8_t tick, uint8_t restart)
{
	static Effect_PulseWightRGBWW current_pwm = {0};
	
	if(restart)
	{
		memset(&current_pwm, 0, sizeof(Effect_PulseWightRGBWW));
	}
	
	Welding_Process(&effect_welding, &current_pwm, tick, restart);
	Effect_SetPixel_PulseWight(current_pwm, 0xff);
	Effect_ActivePixel_PulseWight();
}

typedef struct{
	
	Effect_PulseWightRGBWW *rgbww_pwm_arr;
	uint16_t  lightness;  		/*亮度<0-1000>*/
	uint16_t  init_lightness;
	uint8_t   frq;  			/*<1-11>,11为Random*/
	float whole_tick;
	float work_tick;
	Effect_PulseWightRGBWW pwm[3];
	uint8_t deal_data_update[3];
	Effect_PulseWightRGBWW* init_pwm;
	Effect_PulseWightRGBWW pwm_current[3];
	float run_time[3];
	LinearChange_Struct liner[3];
	uint8_t flash_cnt;
	uint8_t flash_num;
	uint8_t cop_car_index;
	uint8_t cop_car_index_cnt;
	uint8_t area_index;
	CopCar_Type init_copcar_type;
	CopCar_Type copcar_type;
	uint8_t   area_num;
	uint8_t *copcar_index;
}Effect_CopCar;
Effect_CopCar effect_copcar;
void CopCar_Init(void *effect_mode, const Light_Effect* effect, Effect_PulseWightRGBWW *rgbww_pwm, uint8_t restart, uint8_t type)
{
	uint16_t  lightness;
	CopCar_Type copcar_type;
	Effect_CopCar *copcar = (Effect_CopCar *)effect_mode;
	const CopCar *cop_car_arg = &effect->effect_arg.cop_car_arg;
	if (copcar == NULL || effect == NULL || rgbww_pwm == NULL)
	{
		return;
	}
    if (type == 2)
    {
        cop_car_arg = &effect->effect_arg.cop_car2_arg;
    }
    else if (type == 3)
    {
        cop_car_arg = &effect->effect_arg.cop_car3_arg;
    }

	lightness = cop_car_arg->lightness;
	copcar_type = (cop_car_arg->color < 5)? (CopCar_Type)cop_car_arg->color : CopCar_Color_RBW;
	if(restart == 1 || copcar->init_copcar_type != copcar_type || copcar->init_lightness != lightness)
	{
		if(copcar_type == CopCar_Color_R)
		{
			Effect_HSI2PulseWight(1, 100, lightness, &rgbww_pwm[0], 6500); //R
			memcpy(&rgbww_pwm[1], &rgbww_pwm[0], sizeof(Effect_PulseWightRGBWW));
			memcpy(&rgbww_pwm[2], &rgbww_pwm[0], sizeof(Effect_PulseWightRGBWW));
		}
		else if(copcar_type == CopCar_Color_B)
		{
			Effect_HSI2PulseWight(240, 100, lightness, &rgbww_pwm[0], 6500); //B
			memcpy(&rgbww_pwm[1], &rgbww_pwm[0], sizeof(Effect_PulseWightRGBWW));
			memcpy(&rgbww_pwm[2], &rgbww_pwm[0], sizeof(Effect_PulseWightRGBWW));
		}
		else if(copcar_type == CopCar_Color_RB)
		{
			Effect_HSI2PulseWight(240, 100, lightness, &rgbww_pwm[0], 6500); //B
			Effect_HSI2PulseWight(1, 100, lightness, &rgbww_pwm[1], 6500); //R
			Effect_HSI2PulseWight(300, 100, lightness, &rgbww_pwm[2], 6500); //B+R
		}
		else if(copcar_type == CopCar_Color_BW)
		{
			Effect_HSI2PulseWight(240, 100, lightness, &rgbww_pwm[0], 6500); //B
			Effect_HSI2PulseWight(240, 0, lightness, &rgbww_pwm[1], 6500);  //W
			Effect_HSI2PulseWight(240, 50, lightness, &rgbww_pwm[2], 6500); //B+W
		}
		else //R+B+W
		{
			Effect_HSI2PulseWight(240, 100, lightness, &rgbww_pwm[0], 6500); //B
			Effect_HSI2PulseWight(240, 0, lightness, &rgbww_pwm[1], 6500);  //W
			Effect_HSI2PulseWight(1, 100, lightness, &rgbww_pwm[2], 6500);  //R
			Effect_HSI2PulseWight(300, 50, lightness, &rgbww_pwm[3], 6500); // B+W+R
		}	
		__disable_irq();
		copcar->rgbww_pwm_arr = rgbww_pwm;
		__enable_irq();
		copcar->init_copcar_type = copcar_type;
		copcar->init_lightness = lightness;
	}
	__disable_irq();
	copcar->copcar_type = copcar_type;
	if (type == 1 || type == 2)
	{
		copcar->frq = (cop_car_arg->frq <= 10) ? (cop_car_arg->frq) : 10;
        if (type == 1)
        {
           copcar->copcar_index = copcarI_index; 
        }
        else
        {
            copcar->copcar_index = copcarII_index;
        }
	}
	else if(type == 3)
	{
		copcar->frq = (cop_car_arg->frq <= 6) ? (cop_car_arg->frq) : 6;
		if (cop_car_arg->frq >= 6)
		{
			copcar->frq = 10;
		}
		else if (cop_car_arg->frq >= 3)
		{
			copcar->frq = cop_car_arg->frq >= 3 ? cop_car_arg->frq + 1 : cop_car_arg->frq;
		}
		copcar->copcar_index = copcarIII_index;
	}
	copcar->frq = (copcar->frq == 0)? 1 : copcar->frq;
	copcar->copcar_type = copcar_type;
	__enable_irq();
}

void CopCar_InitI(void *effect_mode, const Light_Effect* effect, Effect_PulseWightRGBWW *rgbww_pwm, uint8_t restart)
{
	CopCar_Init(effect_mode, effect, rgbww_pwm, restart, 1);
}

void Effect_CopCar_Init(const void* effect, uint8_t restart)
{
	Effect_Init(effect, &effect_copcar, CopCar_InitI, restart);
}

void CopCar_InitII(void *effect_mode, const Light_Effect* effect, Effect_PulseWightRGBWW *rgbww_pwm, uint8_t restart)
{
	CopCar_Init(effect_mode, effect, rgbww_pwm, restart, 2);
}
void Effect_CopCarII_Init(const void* effect, uint8_t restart)
{
	Effect_Init(effect, &effect_copcar, CopCar_InitII, restart);
}

void CopCar_InitIII(void *effect_mode, const Light_Effect* effect, Effect_PulseWightRGBWW *rgbww_pwm, uint8_t restart)
{
	CopCar_Init(effect_mode, effect, rgbww_pwm, restart, 3);
}

void Effect_CopCarIII_Init(const void* effect, uint8_t restart)
{
	Effect_Init(effect, &effect_copcar, CopCar_InitIII, restart);
}
uint8_t CopCar_Process(Effect_CopCar *copcar, Effect_PulseWightRGBWW *current_pwm, uint8_t tick, uint8_t restart)
{
	static uint8_t res = 1;
	uint16_t index = 0;
	uint8_t j;
	uint8_t ret = 0;  
	uint16_t interveal = 0;
	
	if (copcar == NULL || current_pwm == NULL)
	{
		return ret;
	}
	
	if(restart == 1)
	{
		copcar->flash_cnt = 0;
		copcar->area_index = 0;
		copcar->cop_car_index = 0;
		res = 1;
	}
	if (res)
	{
		copcar->area_num = copcar->copcar_type == CopCar_Color_RBW ? 3 : 2;
		if (copcar->frq < 7)
		{
			copcar->cop_car_index = copcar->frq - 1;
		}
		else
		{
			if (copcar->area_index == copcar->area_num - 1 && copcar->flash_cnt >= copcar->flash_num)
			{
				copcar->cop_car_index_cnt = ++copcar->cop_car_index_cnt % (copcar->frq - 5);
				copcar->cop_car_index = copcar->copcar_index[copcar->cop_car_index_cnt];
				copcar->flash_cnt = 0;
				copcar->area_index = 0;
				if ((copcar->frq == 10 && copcar->cop_car_index == 5))
				{
					copcar->area_index = copcar->area_num - 1;
				}
			}
		}
		copcar->work_tick = CopCar_WT[copcar->cop_car_index]/(float)tick;// + 200;
		copcar->flash_num = CopCar_FlashTimers[copcar->cop_car_index];
		
		interveal = copcar->work_tick;
		
		if (copcar->flash_cnt >= copcar->flash_num)
		{
			copcar->flash_cnt = 0;
			copcar->area_index = ++copcar->area_index % copcar->area_num;
		}
		if (++copcar->flash_cnt == copcar->flash_num) //
		{
            if (copcar->copcar_index != copcarII_index)
            {
                interveal = CopCar_CInterval[copcar->cop_car_index]/(float)tick;// + 500;
            }
			else
            {
                interveal = CopCar2_CInterval[copcar->cop_car_index]/(float)tick;// + 500;
            }
		}
		
		copcar->whole_tick = copcar->work_tick + interveal;
		
		memset(&copcar->pwm[0], 0, sizeof(Effect_PulseWightRGBWW) * 3);
		if ((copcar->frq  == 6 || (copcar->frq == 10 && copcar->cop_car_index == 5)) && copcar->copcar_index != copcarII_index)
		{
			for (index = 0; index < copcar->area_num; index++)
			{
				if (COPCAR_PIXEL_NUM > 1)
				{
					memcpy(&copcar->pwm[index], copcar->rgbww_pwm_arr + index, sizeof(Effect_PulseWightRGBWW));
				}
				else
				{
					memcpy(&copcar->pwm[index], copcar->rgbww_pwm_arr + copcar->area_num, sizeof(Effect_PulseWightRGBWW));
				}
			}
		}
		else
		{
			memcpy(&copcar->pwm[copcar->area_index], copcar->rgbww_pwm_arr + copcar->area_index, sizeof(Effect_PulseWightRGBWW));
		}
		memset(copcar->run_time, 0, sizeof(float) * 3);
	}
	if (COPCAR_PIXEL_NUM > 1)
	{
		for (index = 0; index < copcar->area_num; index++)
		{
			copcar->liner[index].pwm_start = &copcar->pwm[index];
			copcar->liner[index].pwm_end = &copcar->pwm[index];
			copcar->liner[index].time_start = 0;
			copcar->liner[index].time_end = copcar->work_tick;
			copcar->liner[index].whole_time = copcar->whole_tick;
			copcar->liner[index].pwm_current = &copcar->pwm_current[index];
			copcar->liner[index].run_time = &copcar->run_time[index];
			copcar->liner[index].data_update = &copcar->deal_data_update[index];
			res = Effect_Pwm_LinearChange(&copcar->liner[index]); 
			if (copcar->area_num == 3)
			{
                uint8_t index_max[3];
                if (COPCAR_PIXEL_NUM % 3 == 0)
                {
                    index_max[0] = (uint8_t)(COPCAR_PIXEL_NUM/3 - 1);
                    index_max[1] = (uint8_t)(COPCAR_PIXEL_NUM/3 * 2 - 1);
                }
                else if (COPCAR_PIXEL_NUM % 3 == 1)
                {
                    index_max[0] = (uint8_t)(COPCAR_PIXEL_NUM/3 - 1);
                    index_max[1] = COPCAR_PIXEL_NUM/3 * 2;
                }
                else
                {
                    index_max[0] = COPCAR_PIXEL_NUM/3;
                    index_max[1] = COPCAR_PIXEL_NUM/3 * 2;
                }
                index_max[2] = (uint8_t)(COPCAR_PIXEL_NUM - 1);
                
				if (index == 0)
				{
					for(j = 0; j <= index_max[0]; j++)
					{
						Effect_SetPixel_PulseWight(copcar->pwm_current[index], j);
					}
				}
				else if(index == 1)
				{
					for(j = index_max[0] + 1; j <= index_max[1]; j++)
					{
						Effect_SetPixel_PulseWight(copcar->pwm_current[index], j);
					}
				}
				else
				{
					for(j = index_max[1] + 1; j <= index_max[2]; j++)
					{
						Effect_SetPixel_PulseWight(copcar->pwm_current[index], j);
					}
				}
			}
			else
			{
                uint8_t index_max[2];
                if (COPCAR_PIXEL_NUM % 2 == 0)
                {
                    index_max[0] = (uint8_t)(COPCAR_PIXEL_NUM/3 - 1);
                    index_max[1] = (uint8_t)(COPCAR_PIXEL_NUM/3 * 2 - 1);
                }
                else if (COPCAR_PIXEL_NUM % 3 == 1)
                {
                    index_max[0] = (uint8_t)(COPCAR_PIXEL_NUM/3 - 1);
                    index_max[1] = COPCAR_PIXEL_NUM/3 * 2;
                }
                index_max[0] = (uint8_t)(COPCAR_PIXEL_NUM/2 - 1);
                index_max[1] = (uint8_t)(COPCAR_PIXEL_NUM - 1);
                
				if (index == 0)
				{
					for(j = 0; j <= index_max[0]; j++)
					{
						Effect_SetPixel_PulseWight(copcar->pwm_current[index], j);
					}
				}
				else
				{
					for(j = index_max[0] + 1; j <= index_max[1]; j++)
					{
						Effect_SetPixel_PulseWight(copcar->pwm_current[index], j);
					}
				}
			}
			ret = 1;
		}
	}
	else
	{
		copcar->liner[copcar->area_index].pwm_start = &copcar->pwm[copcar->area_index];
		copcar->liner[copcar->area_index].pwm_end = &copcar->pwm[copcar->area_index];
		copcar->liner[copcar->area_index].time_start = 0;
		copcar->liner[copcar->area_index].time_end = copcar->work_tick;
		copcar->liner[copcar->area_index].whole_time = copcar->whole_tick;
		copcar->liner[copcar->area_index].pwm_current = &copcar->pwm_current[copcar->area_index];
		copcar->liner[copcar->area_index].run_time = &copcar->run_time[copcar->area_index];
		copcar->liner[copcar->area_index].data_update = &copcar->deal_data_update[copcar->area_index];
		res = Effect_Pwm_LinearChange(&copcar->liner[copcar->area_index]); 
		Effect_SetPixel_PulseWight(copcar->pwm_current[copcar->area_index], 0xff);
		ret = 1;
	}
	
	return ret;
}

void Effect_CopCar_Process(uint8_t tick, uint8_t restart)
{
	static Effect_PulseWightRGBWW current_pwm = {0};
	
	if(restart)
	{
		memset(&current_pwm, 0, sizeof(Effect_PulseWightRGBWW));
	}
	
	if (CopCar_Process(&effect_copcar, &current_pwm, tick, restart))
	{
		Effect_ActivePixel_PulseWight();
	}
}

typedef struct{
	
	Effect_PulseWightRGBWW *rgbww_pwm_arr;
	uint16_t  lightness;  		/*亮度<0-1000>*/
	uint16_t init_lightness;
	uint8_t   frq;  			/*<1-11>,11为Random*/
	uint8_t   proc_frq;  		/*<1-11>,11为Random*/
	float whole_tick;
	float work_tick;
	uint8_t deal_data_update;
	Effect_PulseWightRGBWW* init_pwm;
	Effect_PulseWightRGBWW pwm_current;
	float run_time;
	uint8_t run_stage;
	uint8_t flash_cnt;
	uint8_t flash_num;
	uint16_t interval;
	uint8_t sat;
	uint8_t init_sat;
	uint8_t proc_sat;
	uint16_t index_begin;
	uint16_t index_end;
	Index_LinearChange index_change;
}Effect_ColorChase;
Effect_ColorChase effect_colorchase;
void ColorChase_Init(void *effect_mode, const Light_Effect* effect, Effect_PulseWightRGBWW *rgbww_pwm, uint8_t restart)
{
	uint16_t  lightness;
	uint8_t	sat = 0;
	uint8_t data_updated = 0;
	uint16_t index = 0;
	float pwm_per_angle_r = 0;
	float pwm_per_angle_g = 0;
	float pwm_per_angle_b = 0;
	float pwm_per_angle_ww = 0;
	float pwm_per_angle_cw = 0;
	int i = 0;
	Effect_ColorChase *colorchase = (Effect_ColorChase *)effect_mode;
	
	if (colorchase == NULL || effect == NULL || rgbww_pwm == NULL)
	{
		return;
	}
	
	lightness = effect->effect_arg.color_chase_arg.lightness;
	sat = effect->effect_arg.color_chase_arg.sat;
	if(sat != 0)
	{
		if(restart == 1 || colorchase->init_lightness != lightness || colorchase->init_sat != sat)  //重新初始化参数
		{
			data_updated = 1;
			for (i = 0; i <= 240; i += 120)
			{
				Effect_HSI2PulseWight(i, sat, lightness, &rgbww_pwm[i], 6500);
				rgbww_pwm[i].r = rgbww_pwm[i].r < light_effect.min_pwm.r ? light_effect.min_pwm.r : rgbww_pwm[i].r;
				rgbww_pwm[i].g = rgbww_pwm[i].g < light_effect.min_pwm.g ? light_effect.min_pwm.g : rgbww_pwm[i].g;
				rgbww_pwm[i].b = rgbww_pwm[i].b < light_effect.min_pwm.b ? light_effect.min_pwm.b : rgbww_pwm[i].b;
				rgbww_pwm[i].ww = rgbww_pwm[i].ww < light_effect.min_pwm.ww ? light_effect.min_pwm.ww : rgbww_pwm[i].ww;
				rgbww_pwm[i].cw = rgbww_pwm[i].cw < light_effect.min_pwm.cw ? light_effect.min_pwm.cw : rgbww_pwm[i].cw;
			}
			for (i = 0; i <= 240; i += 120)
			{
				pwm_per_angle_r = (rgbww_pwm[(i + 120)%360].r - rgbww_pwm[i].r) / 120.0f;
				pwm_per_angle_g = (rgbww_pwm[(i + 120)%360].g - rgbww_pwm[i].g) / 120.0f;
				pwm_per_angle_b = (rgbww_pwm[(i + 120)%360].b - rgbww_pwm[i].b) / 120.0f;
				pwm_per_angle_ww = (rgbww_pwm[(i + 120)%360].ww - rgbww_pwm[i].ww) / 120.0f;
				pwm_per_angle_cw = (rgbww_pwm[(i + 120)%360].cw - rgbww_pwm[i].cw) / 120.0f;
				for(index = i + 1; index <= i + 120; index++)
				{
					rgbww_pwm[index].r = rgbww_pwm[i].r + pwm_per_angle_r * (index - i);
					rgbww_pwm[index].g = rgbww_pwm[i].g + pwm_per_angle_g * (index - i);
					rgbww_pwm[index].b = rgbww_pwm[i].b + pwm_per_angle_b * (index - i);
					rgbww_pwm[index].ww = rgbww_pwm[i].ww + pwm_per_angle_ww * (index - i);
					rgbww_pwm[index].cw = rgbww_pwm[i].cw + pwm_per_angle_cw * (index - i);
				}
			}
			__disable_irq();
			colorchase->rgbww_pwm_arr = rgbww_pwm;
			__enable_irq();
		}
	}
	else
	{
		if(restart == 1 || colorchase->init_lightness != lightness || colorchase->init_sat != sat)  //重新初始化参数
		{
			data_updated = 1;
			for(index = EFFECT_CCT_MIN; index <= EFFECT_CCT_MAX; index += 100)
			{
				Effect_CCT2PulseWight(index, 0, lightness, &rgbww_pwm[(index - EFFECT_CCT_MIN)/100]);
			}
			__disable_irq();
			colorchase->rgbww_pwm_arr = rgbww_pwm;
			__enable_irq();
		}
	}
	
	colorchase->init_lightness = lightness;
	colorchase->init_sat = sat;
	
	__disable_irq();
	if(data_updated == 1)
	{
		colorchase->deal_data_update = 1;
	}
	colorchase->frq = (effect->effect_arg.color_chase_arg.frq <= 11) ? (effect->effect_arg.color_chase_arg.frq) : 11;
	colorchase->sat = effect->effect_arg.color_chase_arg.sat;
	__enable_irq();
	
}

void Effect_ColorChase_Init(const void* effect, uint8_t restart)
{
	Effect_Init(effect, &effect_colorchase, ColorChase_Init, restart);
}
void ColorChase_Process(Effect_ColorChase *colorchase, Effect_PulseWightRGBWW *current_pwm, uint8_t tick, uint8_t restart)
{
	static uint16_t random = 0;
	static uint8_t res = 0;
	uint16_t index_begin;
	uint16_t index_end;

	if (colorchase == NULL || current_pwm == NULL)
	{
		return;
	}
	colorchase->init_pwm = colorchase->rgbww_pwm_arr;
	
	if(colorchase->sat == 0)  //饱和度为0则只变化CCT,2800k-10000k 10000k-2800k,循环变化
	{
		index_begin = 0;
		index_end = (EFFECT_CCT_MAX - EFFECT_CCT_MIN)/100;
	}
	else  
	{
		index_begin = 1;
		index_end = 360;
	}
	
	if (colorchase->proc_sat != colorchase->sat && ((colorchase->proc_sat & colorchase->sat) == 0))
	{
		colorchase->deal_data_update = 1;
		colorchase->index_begin = index_begin;
		colorchase->index_end = index_end;
		colorchase->proc_sat = colorchase->sat;
	}
	
	if (colorchase->proc_frq != colorchase->frq || restart == 1 || res == 1)
	{
		if (res != 1)
		{
			colorchase->deal_data_update = 1;
		}
		if (colorchase->frq == 11)
		{
			random = Effect_Get_RandomRange_Int16(1, 10);
		}
		else
		{
			random = colorchase->frq;
		}
		colorchase->proc_frq = colorchase->frq;
	}
	
	if(restart == 1)
	{
		colorchase->index_begin = index_begin;
		colorchase->index_end = index_end;
		colorchase->run_time = 0;
		colorchase->deal_data_update = 0;
		res = 0;
	}
	if (res == 1)
	{
		res = 0;
		colorchase->run_time = 0;
	}
	colorchase->work_tick = color_chase_ct[random]/(float)tick;;
	colorchase->whole_tick = colorchase->work_tick;
	colorchase->index_change.index_start = colorchase->index_begin;
	colorchase->index_change.index_end = colorchase->index_end;
	colorchase->index_change.time_start = 0;
	colorchase->index_change.time_end = colorchase->work_tick;
	colorchase->index_change.whole_time = colorchase->whole_tick;
	colorchase->index_change.pwm_src = colorchase->init_pwm;
	colorchase->index_change.pwm_current = &colorchase->pwm_current;
	colorchase->index_change.run_time = &colorchase->run_time;
	colorchase->index_change.data_update = &colorchase->deal_data_update;
	res = Effect_Pwm_IndexChange(&colorchase->index_change);
	memcpy(current_pwm, &colorchase->pwm_current, sizeof(Effect_PulseWightRGBWW)); //逐渐增加CCT或hue
}

void Effect_ColorChase_Process(uint8_t tick, uint8_t restart)
{
	static Effect_PulseWightRGBWW current_pwm = {0};
	
	if(restart)
	{
		memset(&current_pwm, 0, sizeof(Effect_PulseWightRGBWW));
	}
	
	ColorChase_Process(&effect_colorchase, &current_pwm, tick, restart);
	Effect_SetPixel_PulseWight(current_pwm, 0xff);
	Effect_ActivePixel_PulseWight();
}

typedef struct{
	
	Effect_PulseWightRGBWW *rgbww_pwm_arr;
	uint16_t  lightness;  		/*亮度<0-1000>*/
	uint16_t init_lightness;
	uint8_t   frq;  			/*<1-11>,11为Random*/
	float whole_tick;
	float work_tick;
	uint8_t deal_data_update;
	Effect_PulseWightRGBWW* init_pwm;
	Effect_PulseWightRGBWW pwm_current;
	float run_time;
	uint8_t run_stage;
	uint16_t interval;
	uint8_t sat;
	uint8_t init_sat;
	uint8_t proc_sat;
	uint16_t index_begin;
	uint16_t index_end;
	uint16_t init_min_whole;
	uint16_t init_max_whole;
	uint16_t proc_min_whole;
	uint16_t proc_max_whole;
	Index_LinearChange index_change;
}Effect_PartyLights;
Effect_PartyLights effect_partylights;
void PartyLights_Init(void *effect_mode, const Light_Effect* effect, Effect_PulseWightRGBWW *rgbww_pwm, uint8_t restart, uint8_t type)
{
	uint16_t  lightness;
	uint8_t	sat = 0;
	uint8_t data_updated = 0;
	uint16_t index = 0;
	float pwm_per_angle_r = 0;
	float pwm_per_angle_g = 0;
	float pwm_per_angle_b = 0;
	float pwm_per_angle_ww = 0;
	float pwm_per_angle_cw = 0;
	int i = 0;
	uint16_t* temp = NULL;
	Effect_PartyLights *partylights = (Effect_PartyLights *)effect_mode;
	const PartyLights *party_lights_arg;
	
	if (partylights == NULL || effect == NULL || rgbww_pwm == NULL)
	{
		return;
	}
	party_lights_arg = type == 1 ? &effect->effect_arg.party_lights_arg : &effect->effect_arg.party_lights2_arg;
	lightness = party_lights_arg->lightness;
	sat = party_lights_arg->sat;
	if(sat != 0)
	{
		if(restart == 1 || partylights->init_lightness != lightness || partylights->init_sat != sat)  //重新初始化参数
		{

			data_updated = 1;
			for (i = 0; i <= 240; i += 120)
			{
				Effect_HSI2PulseWight(i, sat, lightness, &rgbww_pwm[i], 6500);
				rgbww_pwm[i].r = rgbww_pwm[i].r < light_effect.min_pwm.r ? light_effect.min_pwm.r : rgbww_pwm[i].r;
				rgbww_pwm[i].g = rgbww_pwm[i].g < light_effect.min_pwm.g ? light_effect.min_pwm.g : rgbww_pwm[i].g;
				rgbww_pwm[i].b = rgbww_pwm[i].b < light_effect.min_pwm.b ? light_effect.min_pwm.b : rgbww_pwm[i].b;
				rgbww_pwm[i].ww = rgbww_pwm[i].ww < light_effect.min_pwm.ww ? light_effect.min_pwm.ww : rgbww_pwm[i].ww;
				rgbww_pwm[i].cw = rgbww_pwm[i].cw < light_effect.min_pwm.cw ? light_effect.min_pwm.cw : rgbww_pwm[i].cw;
			}
			for (i = 0; i <= 240; i += 120)
			{
				pwm_per_angle_r = (rgbww_pwm[(i + 120)%360].r - rgbww_pwm[i].r) / 120.0f;
				pwm_per_angle_g = (rgbww_pwm[(i + 120)%360].g - rgbww_pwm[i].g) / 120.0f;
				pwm_per_angle_b = (rgbww_pwm[(i + 120)%360].b - rgbww_pwm[i].b) / 120.0f;
				pwm_per_angle_ww = (rgbww_pwm[(i + 120)%360].ww - rgbww_pwm[i].ww) / 120.0f;
				pwm_per_angle_cw = (rgbww_pwm[(i + 120)%360].cw - rgbww_pwm[i].cw) / 120.0f;
				for(index = i + 1; index <= i + 120; index++)
				{
					rgbww_pwm[index].r = rgbww_pwm[i].r + pwm_per_angle_r * (index - i);
					rgbww_pwm[index].g = rgbww_pwm[i].g + pwm_per_angle_g * (index - i);
					rgbww_pwm[index].b = rgbww_pwm[i].b + pwm_per_angle_b * (index - i);
					rgbww_pwm[index].ww = rgbww_pwm[i].ww + pwm_per_angle_ww * (index - i);
					rgbww_pwm[index].cw = rgbww_pwm[i].cw + pwm_per_angle_cw * (index - i);
				}
			}
			__disable_irq();
			partylights->rgbww_pwm_arr = rgbww_pwm;
			__enable_irq();
		}
	}
	else
	{
		if(restart == 1 || partylights->init_lightness != lightness || partylights->init_sat != sat)  //重新初始化参数
		{
			data_updated = 1;
			for(index = EFFECT_CCT_MIN; index <= EFFECT_CCT_MAX; index += 100)
			{
				Effect_CCT2PulseWight(index, 0, lightness, &rgbww_pwm[(index - EFFECT_CCT_MIN)/100]);
			}
			__disable_irq();
			partylights->rgbww_pwm_arr = rgbww_pwm;
			__enable_irq();
		}
	}
	
	partylights->init_lightness = lightness;
	partylights->init_sat = sat;
	
	__disable_irq();
	if(data_updated == 1)
	{
		partylights->deal_data_update = 1;
	}
	
	
	if (type == 1)
	{
		if(partylights->sat == 0)  //饱和度为0则只变化CCT,2800k-10000k 10000k-2800k,循环变化
		{
			temp = party_lights_ct;
		}
		else  
		{
			temp = party_lights_wt;
		}
		if (party_lights_arg->frq >= 11)
		{
			partylights->frq = 11;
			partylights->init_min_whole = temp[10];
			partylights->init_max_whole = temp[0];
		}
		else
		{
			partylights->frq = party_lights_arg->frq;
			partylights->init_min_whole = temp[partylights->frq];
			partylights->init_max_whole = temp[partylights->frq];
		}
	}
	else
	{
		partylights->init_min_whole = party_lights_arg->whole_time;
		partylights->init_max_whole = partylights->init_min_whole;
	}
	partylights->sat = party_lights_arg->sat;
	__enable_irq();
}

void PartyLights_InitI(void *effect_mode, const Light_Effect* effect, Effect_PulseWightRGBWW *rgbww_pwm, uint8_t restart)
{
	PartyLights_Init(effect_mode, effect, rgbww_pwm, restart, 1);
}

void Effect_PartyLights_Init(const void* effect, uint8_t restart)
{
	Effect_Init(effect, &effect_partylights, PartyLights_InitI, restart);
}

void PartyLights_InitII(void *effect_mode, const Light_Effect* effect, Effect_PulseWightRGBWW *rgbww_pwm, uint8_t restart)
{
	PartyLights_Init(effect_mode, effect, rgbww_pwm, restart, 2);
}

void Effect_PartyLightsII_Init(const void* effect, uint8_t restart)
{
	Effect_Init(effect, &effect_partylights, PartyLights_InitII, restart);
}
void PartyLights_Process(Effect_PartyLights *partylights, Effect_PulseWightRGBWW *current_pwm, uint8_t tick, uint8_t restart)
{
	static uint8_t res = 0;
	
	uint16_t index_begin;
	uint16_t index_end;
	
	if (partylights == NULL || current_pwm == NULL)
	{
		return;
	}
	partylights->init_pwm = partylights->rgbww_pwm_arr;
	
	if(partylights->sat == 0)  //饱和度为0则只变化CCT,2800k-10000k 10000k-2800k,循环变化
	{
		index_begin = 0;
		index_end = (EFFECT_CCT_MAX - EFFECT_CCT_MIN)/100;
	}
	else  
	{
		index_begin = 1;
		index_end = 360;
	}
	
	if (partylights->proc_sat != partylights->sat && ((partylights->proc_sat & partylights->sat) == 0))
	{
		partylights->deal_data_update = 1;
		partylights->index_begin = index_begin;
		partylights->index_end = index_end;
		partylights->proc_sat = partylights->sat;
	}
	
	if (partylights->proc_min_whole != partylights->init_min_whole || partylights->proc_max_whole != partylights->init_max_whole 
		|| restart == 1 || res == 1)
	{
		if (res != 1)
		{
			partylights->deal_data_update = 1;
		}
		partylights->proc_min_whole = partylights->init_min_whole;
		partylights->proc_max_whole = partylights->init_max_whole;
		partylights->work_tick =  Effect_Get_RandomRange_Uint32(partylights->init_min_whole, partylights->init_max_whole)/(float)tick;
	}
	if(restart == 1)
	{
		partylights->index_begin = index_begin;
		partylights->index_end = index_end;
		partylights->run_time = 0;
		partylights->deal_data_update = 0;
		res = 0;
	}
	if (res == 1)
	{
		res = 0;
		partylights->index_begin = partylights->index_begin == index_begin ? index_end:index_begin;
		partylights->index_end = partylights->index_begin == index_begin ? index_end:index_begin;
		partylights->run_time = 0;
	}
	partylights->whole_tick = partylights->work_tick;
	partylights->index_change.index_start = partylights->index_begin;
	partylights->index_change.index_end = partylights->index_end;
	partylights->index_change.time_start = 0;
	partylights->index_change.time_end = partylights->work_tick;
	partylights->index_change.whole_time = partylights->whole_tick;
	partylights->index_change.pwm_src = partylights->init_pwm;
	partylights->index_change.pwm_current = &partylights->pwm_current;
	partylights->index_change.run_time = &partylights->run_time;
	partylights->index_change.data_update = &partylights->deal_data_update;
	res = Effect_Pwm_IndexChange(&partylights->index_change);
	memcpy(current_pwm, &partylights->pwm_current, sizeof(Effect_PulseWightRGBWW)); //逐渐增加CCT或hue
}

void Effect_PartyLights_Process(uint8_t tick, uint8_t restart)
{
	static Effect_PulseWightRGBWW current_pwm = {0};
	
	if(restart)
	{
		memset(&current_pwm, 0, sizeof(Effect_PulseWightRGBWW));
	}
	PartyLights_Process(&effect_partylights, &current_pwm, tick, restart);
	Effect_SetPixel_PulseWight(current_pwm, 0xff);
	Effect_ActivePixel_PulseWight();
}

typedef struct{
	
	Effect_PulseWightRGBWW *rgbww_pwm_arr;
	uint16_t  lightness;  		/*亮度<0-1000>*/
	uint16_t  init_lightness;  		/*亮度<0-1000>*/
	uint8_t   frq;  			/*<1-11>,11为Random*/
	float whole_tick;
	float work_tick;
	uint8_t deal_data_update;
	Effect_PulseWightRGBWW init_pwm;
	Effect_PulseWightRGBWW pwm_current;
	float run_time;
	LinearChange_Struct liner;
	uint8_t flash_num;
	uint8_t min_flash_num;
	uint8_t max_flash_num;
	uint8_t firework_type;
	uint16_t init_min_interval;
	uint16_t init_max_interval;
	uint16_t proc_min_interval;
	uint16_t proc_max_interval;
}Effect_Fireworks;
Effect_Fireworks effect_fireworks;
void Fireworks_Init(void *effect_mode, const Light_Effect* effect, Effect_PulseWightRGBWW *rgbww_pwm, uint8_t restart, uint8_t type)
{
	uint16_t  lightness;
	uint8_t data_updated = 0;
	uint16_t index = 0;
	Effect_Fireworks *fireworks = (Effect_Fireworks *)effect_mode;
	const Fireworks *fireworks_arg;
	#if (PRODUCT_FUNCTION == 2 || PRODUCT_FUNCTION == 3)
	float pwm_per_angle_r = 0;
	float pwm_per_angle_g = 0;
	float pwm_per_angle_b = 0;
	float pwm_per_angle_ww = 0;
	float pwm_per_angle_cw = 0;
	uint16_t i;
	#endif
	
	if (fireworks == NULL || effect == NULL || rgbww_pwm == NULL)
	{
		return;
	}
	fireworks_arg = type == 2 ? &effect->effect_arg.fireworks2_arg : &effect->effect_arg.fireworks_arg;
	lightness = fireworks_arg->lightness;
	
	if(restart == 1 || fireworks->init_lightness != lightness)  //重新初始化参数
	{
		data_updated = 1;
		#if (PRODUCT_FUNCTION == 2 || PRODUCT_FUNCTION == 3) //双色温简单彩色和全功能彩色
//			for(index = 0; index <= 360; index++)
//			{
//				Effect_HSI2PulseWight(index, 100, lightness, &rgbww_pwm[index], 6500);
//			}
			for (i = 0; i <= 240; i += 120)
			{
				Effect_HSI2PulseWight(i, 100, lightness, &rgbww_pwm[i], 6500);
				rgbww_pwm[i].r = rgbww_pwm[i].r < light_effect.min_pwm.r ? light_effect.min_pwm.r : rgbww_pwm[i].r;
				rgbww_pwm[i].g = rgbww_pwm[i].g < light_effect.min_pwm.g ? light_effect.min_pwm.g : rgbww_pwm[i].g;
				rgbww_pwm[i].b = rgbww_pwm[i].b < light_effect.min_pwm.b ? light_effect.min_pwm.b : rgbww_pwm[i].b;
				rgbww_pwm[i].ww = rgbww_pwm[i].ww < light_effect.min_pwm.ww ? light_effect.min_pwm.ww : rgbww_pwm[i].ww;
				rgbww_pwm[i].cw = rgbww_pwm[i].cw < light_effect.min_pwm.cw ? light_effect.min_pwm.cw : rgbww_pwm[i].cw;
			}
			for (i = 0; i <= 240; i += 120)
			{
				pwm_per_angle_r = (rgbww_pwm[(i + 120)%360].r - rgbww_pwm[i].r) / 120.0f;
				pwm_per_angle_g = (rgbww_pwm[(i + 120)%360].g - rgbww_pwm[i].g) / 120.0f;
				pwm_per_angle_b = (rgbww_pwm[(i + 120)%360].b - rgbww_pwm[i].b) / 120.0f;
				pwm_per_angle_ww = (rgbww_pwm[(i + 120)%360].ww - rgbww_pwm[i].ww) / 120.0f;
				pwm_per_angle_cw = (rgbww_pwm[(i + 120)%360].cw - rgbww_pwm[i].cw) / 120.0f;
				for(index = i + 1; index <= i + 120; index++)
				{
					rgbww_pwm[index].r = rgbww_pwm[i].r + pwm_per_angle_r * (index - i);
					rgbww_pwm[index].g = rgbww_pwm[i].g + pwm_per_angle_g * (index - i);
					rgbww_pwm[index].b = rgbww_pwm[i].b + pwm_per_angle_b * (index - i);
					rgbww_pwm[index].ww = rgbww_pwm[i].ww + pwm_per_angle_ww * (index - i);
					rgbww_pwm[index].cw = rgbww_pwm[i].cw + pwm_per_angle_cw * (index - i);
				}
			}
			for(; index <= (361 + (EFFECT_CCT_MAX - EFFECT_CCT_MIN)/100); index++) 
			{
				Effect_CCT2PulseWight((index - 361)*100 + EFFECT_CCT_MIN, 10, lightness, &rgbww_pwm[index]);
			}
			__disable_irq();
			fireworks->rgbww_pwm_arr = rgbww_pwm;
			__enable_irq();
		#elif (PRODUCT_FUNCTION == 1)  //双色温
		for(; index <= ((EFFECT_CCT_MAX - EFFECT_CCT_MIN)/100); index++) 
		{
			Effect_CCT2PulseWight(index*100 + EFFECT_CCT_MIN, 0, lightness, &rgbww_pwm[index]);
		}
		__disable_irq();
		fireworks->rgbww_pwm_arr = rgbww_pwm;
		__enable_irq();	
		#else
		Effect_CCT2PulseWight(EFFECT_CCT_MIN, 0, lightness, &rgbww_pwm[0]);
		__disable_irq();
		fireworks->rgbww_pwm_arr = rgbww_pwm;
		__enable_irq();	
		#endif
		fireworks->init_lightness = lightness;
	}
	
	__disable_irq();
	if(data_updated == 1)
	{
		fireworks->deal_data_update = 1;
	}
	
	if (type == 1)
	{
		fireworks->frq = (fireworks_arg->frq <= 11) ? (fireworks_arg->frq) : 11;
		fireworks->init_min_interval = firework_interval_min[fireworks->frq];
		fireworks->init_max_interval = firework_interval_max[fireworks->frq];
		fireworks->min_flash_num = fireware_flash_min[fireworks->frq];
		fireworks->max_flash_num = fireware_flash_max[fireworks->frq];
	}
	else
	{
		fireworks->min_flash_num = 1;
		fireworks->max_flash_num = 4;
		fireworks->init_min_interval = fireworks_arg->min_interval;
		fireworks->init_max_interval = fireworks_arg->max_interval;
	}
	fireworks->firework_type =  fireworks_arg->type;
	__enable_irq();
}

void Fireworks_InitI(void *effect_mode, const Light_Effect* effect, Effect_PulseWightRGBWW *rgbww_pwm, uint8_t restart)
{
	Fireworks_Init(effect_mode, effect, rgbww_pwm, restart, 1);
}

void Effect_Fireworks_Init(const void* effect, uint8_t restart)
{
	Effect_Init(effect, &effect_fireworks, Fireworks_InitI, restart);
}

void Fireworks_InitII(void *effect_mode, const Light_Effect* effect, Effect_PulseWightRGBWW *rgbww_pwm, uint8_t restart)
{
	Fireworks_Init(effect_mode, effect, rgbww_pwm, restart, 2);
}

void Effect_FireworksII_Init(const void* effect, uint8_t restart)
{
	Effect_Init(effect, &effect_fireworks, Fireworks_InitII, restart);
}
void Fireworks_Process(Effect_Fireworks *fireworks, Effect_PulseWightRGBWW *current_pwm, uint8_t tick, uint8_t restart)
{
	uint16_t random = 0;
	static uint8_t res = 0;
	Effect_PulseWightRGBWW off_pwm = {light_effect.min_pwm.r, light_effect.min_pwm.g,  
										light_effect.min_pwm.b, light_effect.min_pwm.ww, light_effect.min_pwm.cw};  //默认pwm值，默认为最低亮度，即灭
	
	if (fireworks == NULL || current_pwm == NULL)
	{
		return;
	}
	fireworks->work_tick = 1700/(float)tick;
	if (fireworks->proc_min_interval != fireworks->init_min_interval || fireworks->proc_max_interval != fireworks->init_max_interval)
	{
		fireworks->deal_data_update = 1;
		fireworks->whole_tick = fireworks->work_tick + Effect_Get_RandomRange_Int16(fireworks->init_min_interval, fireworks->init_max_interval)/(float)tick;
		fireworks->proc_min_interval = fireworks->init_min_interval;
		fireworks->proc_max_interval = fireworks->init_max_interval;
	}
	if(res || restart == 1)
	{
		if (restart == 1)
		{
			fireworks->deal_data_update = 0;
			fireworks->flash_num = 0;
		}
		if (fireworks->flash_num == 0)
		{
			fireworks->flash_num = Effect_Get_RandomRange_Int16(fireworks->min_flash_num, fireworks->max_flash_num);
		}
		
		if (fireworks->flash_num == 1)
		{
			fireworks->whole_tick = fireworks->work_tick + Effect_Get_RandomRange_Int16(fireworks->init_min_interval, fireworks->init_max_interval)/(float)tick;
		}
		else
		{
			fireworks->whole_tick = Effect_Get_RandomRange_Int16(850, 1530)/(float)tick;
		}
		fireworks->flash_num--;
		
		#if (PRODUCT_FUNCTION == 2 || PRODUCT_FUNCTION == 3) //双色温简单彩色和全功能彩色
		if(fireworks->firework_type == 0)  //CT
		{
			random = Effect_Get_RandomRange_Int16(361, 361 + (EFFECT_CCT_MAX - EFFECT_CCT_MIN)/100);
		}
		else if(fireworks->firework_type == 1)  //HUE
		{
			random = Effect_Get_RandomRange_Int16(1, 360);
		}
		else  //CT+HUE
		{
			 random = Effect_Get_RandomRange_Int16(1, 361 + (EFFECT_CCT_MAX - EFFECT_CCT_MIN)/100);
		}
		#elif (PRODUCT_FUNCTION == 1)  //双色温
		if (fireworks->firework_type == 0)
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
		else if (fireworks->firework_type == 1)
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
		#else //单色温
		random = 0;	
		#endif
		memcpy(&fireworks->init_pwm, fireworks->rgbww_pwm_arr + random, sizeof(Effect_PulseWightRGBWW)); //随机
		fireworks->run_time = 0;
	}
	fireworks->liner.pwm_start = &fireworks->init_pwm;
	fireworks->liner.pwm_end = &off_pwm;
	fireworks->liner.time_start = 0;
	fireworks->liner.time_end = fireworks->work_tick;
	fireworks->liner.whole_time = fireworks->whole_tick;
	fireworks->liner.pwm_current = &fireworks->pwm_current;
	fireworks->liner.run_time = &fireworks->run_time;
	fireworks->liner.data_update = &fireworks->deal_data_update;
	res = Effect_Pwm_LinearChange(&fireworks->liner); //逐渐灭
	memcpy(current_pwm, &fireworks->pwm_current, sizeof(Effect_PulseWightRGBWW));
}

void Effect_Fireworks_Process(uint8_t tick, uint8_t restart)
{
	static Effect_PulseWightRGBWW current_pwm = {0};
	
	if(restart)
	{
		memset(&current_pwm, 0, sizeof(Effect_PulseWightRGBWW));
	}
	
	Fireworks_Process(&effect_fireworks, &current_pwm, tick, restart);
	Effect_SetPixel_PulseWight(current_pwm, 0xff);
	Effect_ActivePixel_PulseWight();
}

void Effect_Off_Process(uint8_t tick, uint8_t restart)
{
	Effect_PulseWightRGBWW current_pwm = {0};
	Effect_SetPixel_PulseWight(current_pwm, 0xff);
	Effect_ActivePixel_PulseWight();
}

typedef struct{
	
	Effect_PulseWightRGBWW *rgbww_pwm_arr;
	uint8_t deal_data_update;
	Effect_PulseWightRGBWW pwm_current;
	float run_time;
	LinearChange_Struct liner;
	Effect_TriggerType trigger_type;
}Effect_IAmHere;
Effect_IAmHere effect_iamhere;
void IAmHere_Init(void *effect_mode, const Light_Effect* effect, Effect_PulseWightRGBWW *rgbww_pwm, uint8_t restart)
{
	Effect_IAmHere *iamhere = (Effect_IAmHere *)effect_mode;
	if (iamhere == NULL || effect == NULL || rgbww_pwm == NULL)
	{
		return;
	}
	if (effect->effect_arg.i_am_here.run == 0)
	{
		Effect_CCT2PulseWight(5600, 0, 500, &rgbww_pwm[0]);
		__disable_irq();
		iamhere->deal_data_update = 1;
		iamhere->rgbww_pwm_arr = rgbww_pwm;
		iamhere->trigger_type = Trigger_Once;  //触发类型
		__enable_irq();
	}
	else
	{
		iam_here_status = 1;
	}
}

void Effect_IAmHere_Init(const void* effect, uint8_t restart)
{
	Effect_Init(effect, &effect_iamhere, IAmHere_Init, restart);
}

void IAmHere_Process(Effect_IAmHere *here, Effect_PulseWightRGBWW *current_pwm, uint8_t tick, uint8_t restart)
{
	uint8_t res = 0;
	static uint8_t index = 0;
	Effect_Element element[] = {
		{here->rgbww_pwm_arr, here->rgbww_pwm_arr, 0, 150/(float)tick, 300/(float)tick},
		{here->rgbww_pwm_arr, here->rgbww_pwm_arr, 0, 150/(float)tick, 300/(float)tick},
		{here->rgbww_pwm_arr, here->rgbww_pwm_arr, 0, 150/(float)tick, 300/(float)tick},
		{here->rgbww_pwm_arr, here->rgbww_pwm_arr, 0, 150/(float)tick, 600/(float)tick},
		{here->rgbww_pwm_arr, here->rgbww_pwm_arr, 0, 150/(float)tick, 600/(float)tick},
		{here->rgbww_pwm_arr, here->rgbww_pwm_arr, 0, 150/(float)tick, 300/(float)tick},
		{here->rgbww_pwm_arr, here->rgbww_pwm_arr, 0, 450/(float)tick, 900/(float)tick},
		{here->rgbww_pwm_arr, here->rgbww_pwm_arr, 0, 150/(float)tick, 150/(float)tick},
	};
	
	if (here == NULL || current_pwm == NULL)
	{
		return;
	}
	
	if (restart == 1 || here->trigger_type == Trigger_Once)
	{
		iam_here_status = 0;
		index = 0;
		here->run_time = 0;
		here->deal_data_update = 0;
		here->trigger_type = Trigger_None;
	}
	if (index < sizeof(element)/sizeof(Effect_Element))
	{
		here->liner.pwm_start = element[index].pwm_start;
		here->liner.pwm_end = element[index].pwm_end;
		here->liner.time_start = element[index].time_start;
		here->liner.time_end = element[index].time_end;
		here->liner.whole_time = element[index].whole_time;
		here->liner.pwm_current = &here->pwm_current;
		here->liner.run_time = &here->run_time;
		here->liner.data_update = &here->deal_data_update;
		res = Effect_Pwm_LinearChange(&here->liner); //逐渐变灭
		memcpy(current_pwm, &here->pwm_current, sizeof(Effect_PulseWightRGBWW));
		if(res) 	//该工作周期结束
		{
			index++;
			here->run_time = 0;
		}
	}
	else
	{
		iam_here_status = 1;
	}
}

void Effect_IAmHere_Process(uint8_t tick, uint8_t restart)
{
	static Effect_PulseWightRGBWW current_pwm = {0};
	
	if(restart)
	{
		memset(&current_pwm, 0, sizeof(Effect_PulseWightRGBWW));
	}
	
	IAmHere_Process(&effect_iamhere, &current_pwm, tick, restart);
	Effect_SetPixel_PulseWight(current_pwm, 0xff);
	Effect_ActivePixel_PulseWight();
}

typedef struct{
	
	Effect_PulseWightRGBWW *rgbww_pwm_arr;
	uint8_t   speed;  			/*<1-10>,*/
	uint8_t   direction;  		/*方向，0：向左，1：向右*/
	uint8_t   proc_direction;  		/*方向，0：向左，1：向右*/
	uint8_t deal_data_update;
	Effect_PulseWightRGBWW init_pwm[EFFECT_PIXEL_NUM];
	float run_time;
	LinearChange_Struct liner;
	uint8_t flash_cnt;
	Light_Effect init_effect;
}Effect_ColorFade;

Effect_ColorFade effect_colorfade = {0};

void ColorFade_Init(void *effect_mode, const Light_Effect* effect, Effect_PulseWightRGBWW *rgbww_pwm, uint8_t restart)
{
	uint8_t data_updated = 0;
	uint16_t index = 0;
	Effect_ColorFade *colorfade = (Effect_ColorFade *)effect_mode;
	
	if (colorfade == NULL || effect == NULL || rgbww_pwm == NULL)
	{
		return;
	}
	
	for (index = 0; index < EFFECT_PIXEL_NUM; index++)
	{
		rgbww_pwm[index] = colorfade->rgbww_pwm_arr[index];
		
		if(restart == 1 || memcmp(colorfade->init_effect.effect_arg.color_fade_arg.color_arg, \
			effect->effect_arg.one_color_chase_arg.color_arg, sizeof(colorfade->init_effect.effect_arg.color_fade_arg.color_arg)) != 0) //重新初始化参数
		{
			data_updated = 1;
			switch(effect->effect_arg.color_fade_arg.color_arg[index].mode)
			{
				case EffectModeCCT:
					Effect_CCT2PulseWight(effect->effect_arg.color_fade_arg.color_arg[index].arg.cct_arg.cct,\
											effect->effect_arg.color_fade_arg.color_arg[index].arg.cct_arg.gm,\
											effect->effect_arg.color_fade_arg.color_arg[index].arg.cct_arg.lightness, &rgbww_pwm[index]);
					break;
				case EffectModeHSI:
					Effect_HSI2PulseWight(effect->effect_arg.color_fade_arg.color_arg[index].arg.hsi_arg.hue, \
											effect->effect_arg.color_fade_arg.color_arg[index].arg.hsi_arg.sat,\
											effect->effect_arg.color_fade_arg.color_arg[index].arg.hsi_arg.lightness, &rgbww_pwm[index], effect->effect_arg.color_fade_arg.color_arg[index].arg.hsi_arg.cct);
					break;
				case EffectModeBlack:
					memset(&rgbww_pwm[index], 0, sizeof(Effect_PulseWightRGBWW));
					break;
				default:
					data_updated = 0;
					break;
			}
		}
	}
	__disable_irq();
	if(data_updated == 1)
	{
		colorfade->deal_data_update = 1;
		colorfade->rgbww_pwm_arr = rgbww_pwm;
	}
	colorfade->speed = (effect->effect_arg.color_fade_arg.speed <= 10) ? (effect->effect_arg.color_fade_arg.speed) : 10;
	colorfade->direction =  effect->effect_arg.color_fade_arg.direction;
	__enable_irq();
}

uint8_t ColorFade_Process(Effect_ColorFade *colorfade, Effect_PulseWightRGBWW *current_pwm, uint8_t tick, uint8_t restart)
{
	uint8_t i;
	
	if (colorfade == NULL || current_pwm == NULL)
	{
		return 0;
	}
	
	if(restart == 1 || colorfade->deal_data_update == 1)
	{
		memcpy(colorfade->init_pwm, colorfade->rgbww_pwm_arr, sizeof(colorfade->init_pwm));
		colorfade->deal_data_update = 0;
		if (restart == 1)
		{
			colorfade->flash_cnt = 0;
			colorfade->run_time = 0;
		}
	}
	if (colorfade->proc_direction != colorfade->direction)
	{
		colorfade->flash_cnt = 0;
		colorfade->run_time = 0;
		colorfade->proc_direction = colorfade->direction;
	}
	
	if (colorfade->direction == 0)
	{
		for (i = 0; i < (EFFECT_PIXEL_NUM - colorfade->flash_cnt % EFFECT_PIXEL_NUM); i++)
		{
			Effect_SetPixel_PulseWight(colorfade->init_pwm[colorfade->flash_cnt/EFFECT_PIXEL_NUM%EFFECT_PIXEL_NUM], i);
		}
		for (; i < EFFECT_PIXEL_NUM; i++)
		{
			Effect_SetPixel_PulseWight(colorfade->init_pwm[(colorfade->flash_cnt/EFFECT_PIXEL_NUM + 1)%EFFECT_PIXEL_NUM], i);
		}
	}
	else
	{
		for (i = 0; i < colorfade->flash_cnt % EFFECT_PIXEL_NUM; i++)
		{
			Effect_SetPixel_PulseWight(colorfade->init_pwm[(colorfade->flash_cnt/EFFECT_PIXEL_NUM + 1)%EFFECT_PIXEL_NUM], i);
		}
		for (; i < EFFECT_PIXEL_NUM; i++)
		{
			Effect_SetPixel_PulseWight(colorfade->init_pwm[colorfade->flash_cnt/EFFECT_PIXEL_NUM%EFFECT_PIXEL_NUM], i);
		}
	}
	
	if (++colorfade->run_time > color_fade_wt[colorfade->speed])
	{
		colorfade->run_time = 0;
		if (++colorfade->flash_cnt >= EFFECT_PIXEL_NUM * EFFECT_PIXEL_NUM)
		{
			colorfade->flash_cnt = 0;
		}
	}
	
	return 1;
}

void Effect_ColorFade_Init(const void* effect, uint8_t restart)
{
	Effect_Init(effect, &effect_colorfade, ColorFade_Init, restart);
}

void Effect_ColorFade_Process(uint8_t tick, uint8_t restart)
{
	static Effect_PulseWightRGBWW current_pwm = {0};
	
	if (ColorFade_Process(&effect_colorfade, &current_pwm, tick, restart) == 1)
	{
		Effect_ActivePixel_PulseWight();
	}
}

typedef struct{
	
	Effect_PulseWightRGBWW *rgbww_pwm_arr;
	uint8_t   speed;  			/*<1-10>,*/
	float work_tick;
	uint8_t deal_data_update[EFFECT_PIXEL_NUM];
	uint8_t data_updated;
	Effect_PulseWightRGBWW init_pwm[EFFECT_PIXEL_NUM];
	Effect_PulseWightRGBWW pwm_current[EFFECT_PIXEL_NUM];
	float run_time[EFFECT_PIXEL_NUM];
	LinearChange_Struct liner[EFFECT_PIXEL_NUM];
	uint8_t run_stage;
	uint8_t flash_cnt;
	Light_Effect init_effect;
}Effect_ColorCycle;

Effect_ColorCycle effect_colorcycle = {0};

void ColorCycle_Init(void *effect_mode, const Light_Effect* effect, Effect_PulseWightRGBWW *rgbww_pwm, uint8_t restart)
{
	uint16_t index = 0;
	uint8_t data_updated = 0;
	Effect_ColorCycle *colorcycle = (Effect_ColorCycle *)effect_mode;

	if (colorcycle == NULL || effect == NULL || rgbww_pwm == NULL)
	{
		return;
	}
	
	for (index = 0; index < EFFECT_PIXEL_NUM; index++)
	{
		rgbww_pwm[index] = colorcycle->rgbww_pwm_arr[index];
		if(restart == 1 || memcmp(colorcycle->init_effect.effect_arg.color_cycle_arg.color_arg, \
			effect->effect_arg.one_color_chase_arg.color_arg, sizeof(colorcycle->init_effect.effect_arg.color_cycle_arg.color_arg)) != 0) //重新初始化参数
		{
			data_updated = 1;
			switch(effect->effect_arg.color_cycle_arg.color_arg[index].mode)
			{
				case EffectModeCCT:
					Effect_CCT2PulseWight(effect->effect_arg.color_cycle_arg.color_arg[index].arg.cct_arg.cct,\
											effect->effect_arg.color_cycle_arg.color_arg[index].arg.cct_arg.gm,\
											effect->effect_arg.color_cycle_arg.color_arg[index].arg.cct_arg.lightness, &rgbww_pwm[index]);
					break;
				case EffectModeHSI:
					Effect_HSI2PulseWight(effect->effect_arg.color_cycle_arg.color_arg[index].arg.hsi_arg.hue,\
											effect->effect_arg.color_cycle_arg.color_arg[index].arg.hsi_arg.sat,\
											effect->effect_arg.color_cycle_arg.color_arg[index].arg.hsi_arg.lightness, &rgbww_pwm[index], effect->effect_arg.color_cycle_arg.color_arg[index].arg.hsi_arg.cct);
					break;
				case EffectModeBlack:
					memset(&rgbww_pwm[index], 0, sizeof(Effect_PulseWightRGBWW));
					break;
				default:
					data_updated = 0;
					break;
			}
		}
	}
	memcpy(colorcycle->init_effect.effect_arg.color_cycle_arg.color_arg, \
			effect->effect_arg.one_color_chase_arg.color_arg, sizeof(colorcycle->init_effect.effect_arg.color_cycle_arg.color_arg));
	if(data_updated == 1)
	{
		colorcycle->data_updated = 1;
		colorcycle->rgbww_pwm_arr = rgbww_pwm;
	}
	colorcycle->speed = (effect->effect_arg.color_cycle_arg.speed <= 10) ? (effect->effect_arg.color_cycle_arg.speed) : 10;
}

uint8_t ColorCycle_Process(Effect_ColorCycle *colorcycle, Effect_PulseWightRGBWW *current_pwm, uint8_t tick, uint8_t restart)
{
	uint8_t i;
	uint8_t res = 0;
	
	if (colorcycle == NULL || current_pwm == NULL)
	{
		return 0;
	}
	
	colorcycle->work_tick = color_cycle_wt[colorcycle->speed];
	if(restart == 1 || colorcycle->data_updated == 1)
	{
		memcpy(colorcycle->init_pwm, colorcycle->rgbww_pwm_arr, sizeof(colorcycle->init_pwm));
		colorcycle->data_updated = 0;
		if (restart == 1)
		{
			colorcycle->flash_cnt = 0;
			memset(colorcycle->run_time, 0, sizeof(colorcycle->run_time));
			memset(colorcycle->pwm_current, 0, sizeof(colorcycle->pwm_current));
		}
	}
	
	for (i = 0; i < EFFECT_PIXEL_NUM; i++)
	{
		colorcycle->liner[i].pwm_start = &colorcycle->init_pwm[(colorcycle->flash_cnt + i) % EFFECT_PIXEL_NUM];
		colorcycle->liner[i].pwm_end = &colorcycle->init_pwm[(colorcycle->flash_cnt + i + 1) % EFFECT_PIXEL_NUM];
		colorcycle->liner[i].time_start = 0;
		colorcycle->liner[i].time_end = colorcycle->work_tick;
		colorcycle->liner[i].whole_time = colorcycle->work_tick;
		colorcycle->liner[i].pwm_current = &colorcycle->pwm_current[i];
		colorcycle->liner[i].run_time = &colorcycle->run_time[i];
		colorcycle->liner[i].data_update = &colorcycle->deal_data_update[i];
		res = Effect_Pwm_LinearChange(&colorcycle->liner[i]);
	}
	if(res)
	{
		if (++colorcycle->flash_cnt >= EFFECT_PIXEL_NUM)
		{
			colorcycle->flash_cnt = 0;
		}
		memset(colorcycle->run_time, 0, sizeof(colorcycle->run_time));
	}
	for (i = 0; i < EFFECT_PIXEL_NUM; i++)
	{
		Effect_SetPixel_PulseWight(colorcycle->pwm_current[i], i);
	}
	return 1;
}

void Effect_ColorCycle_Init(const void* effect, uint8_t restart)
{
	Effect_Init(effect, &effect_colorcycle, ColorCycle_Init, restart);
}

void Effect_ColorCycle_Process(uint8_t tick, uint8_t restart)
{
	static Effect_PulseWightRGBWW current_pwm = {0};
	
	if (ColorCycle_Process(&effect_colorcycle, &current_pwm, tick, restart) == 1)
	{
		Effect_ActivePixel_PulseWight();
	}
}

typedef struct{

	Effect_PulseWightRGBWW *rgbww_pwm_arr;
	uint16_t  min_lightness;  	/*亮度<0-1000>*/
	uint8_t   speed;  			/*<1-10>,*/
	uint8_t   direction;  		/*方向，0：向左，1：向右*/
	uint8_t flash_cnt;
	float work_tick;
	uint8_t deal_data_update[EFFECT_PIXEL_NUM];
	uint8_t data_updated;
	Effect_PulseWightRGBWW init_pwm[EFFECT_PIXEL_NUM];
	Effect_PulseWightRGBWW min_pwm[EFFECT_PIXEL_NUM];
	Effect_PulseWightRGBWW pwm_current[EFFECT_PIXEL_NUM];
	float run_time[EFFECT_PIXEL_NUM];
	LinearChange_Struct liner[EFFECT_PIXEL_NUM];
	uint8_t run_stage;
	Light_Effect init_effect;
}Effect_ColorGradient;

Effect_ColorGradient effect_colorgradient = {0};

void ColorGradient_Init(void *effect_mode, const Light_Effect* effect, Effect_PulseWightRGBWW *rgbww_pwm, uint8_t restart)
{
	uint16_t index = 0;
	uint8_t data_updated = 0;
	Effect_ColorGradient *colorgradient = (Effect_ColorGradient *)effect_mode;

	if (colorgradient == NULL || effect == NULL || rgbww_pwm == NULL)
	{
		return;
	}
	
	for (index = 0; index < EFFECT_PIXEL_NUM; index++)
	{
		rgbww_pwm[index] = colorgradient->rgbww_pwm_arr[index];
		if(restart == 1 || memcmp(colorgradient->init_effect.effect_arg.color_gradient_arg.color_arg, \
			effect->effect_arg.one_color_chase_arg.color_arg, sizeof(colorgradient->init_effect.effect_arg.color_gradient_arg.color_arg)) != 0) //重新初始化参数
		{
			data_updated = 1;
			switch(effect->effect_arg.color_gradient_arg.color_arg[index].mode)
			{
				case EffectModeCCT:
					Effect_CCT2PulseWight(effect->effect_arg.color_gradient_arg.color_arg[index].arg.cct_arg.cct, \
											effect->effect_arg.color_gradient_arg.color_arg[index].arg.cct_arg.gm, \
											effect->effect_arg.color_gradient_arg.color_arg[index].arg.cct_arg.lightness, &rgbww_pwm[index]);
					break;
				case EffectModeHSI:
					Effect_HSI2PulseWight(effect->effect_arg.color_gradient_arg.color_arg[index].arg.hsi_arg.hue, \
											effect->effect_arg.color_gradient_arg.color_arg[index].arg.hsi_arg.sat, \
											effect->effect_arg.color_gradient_arg.color_arg[index].arg.hsi_arg.lightness, &rgbww_pwm[index], effect->effect_arg.color_gradient_arg.color_arg[index].arg.hsi_arg.cct);
					break;
				case EffectModeBlack:
					memset(&rgbww_pwm[index], 0, sizeof(Effect_PulseWightRGBWW));
					break;
				default:
					data_updated = 0;
					break;
			}
		}
	}
	__disable_irq();
	if(data_updated == 1)
	{
		colorgradient->data_updated = 1;
		memset(colorgradient->deal_data_update, 1, sizeof(colorgradient->deal_data_update));
		colorgradient->rgbww_pwm_arr = rgbww_pwm;
	}
	colorgradient->speed = (effect->effect_arg.color_gradient_arg.speed <= 10) ? (effect->effect_arg.color_gradient_arg.speed) : 10;
	colorgradient->min_lightness = effect->effect_arg.color_gradient_arg.lightness_min;
	__enable_irq();
}

uint8_t ColorGradient_Process(Effect_ColorGradient *colorgradient, Effect_PulseWightRGBWW *current_pwm, uint8_t tick, uint8_t restart)
{
	uint8_t i;
	uint8_t j;
	uint8_t res = 0;
	
	if (colorgradient == NULL || current_pwm == NULL)
	{
		return 0;
	}
	colorgradient->work_tick = color_gradient_wt[colorgradient->speed];
	if(restart == 1 || colorgradient->data_updated == 1)
	{
		if (restart == 1)
		{
			colorgradient->flash_cnt = 0;
			memset(colorgradient->run_time, 0, sizeof(colorgradient->run_time));
			memset(colorgradient->pwm_current, 0, sizeof(colorgradient->pwm_current));
			colorgradient->run_stage = 0;
		}
		
		for (i = 0, j = colorgradient->flash_cnt; j < EFFECT_PIXEL_NUM; i++, j++)
		{
			memcpy(&colorgradient->init_pwm[i], &colorgradient->rgbww_pwm_arr[j], sizeof(Effect_PulseWightRGBWW));
		}
		for (j = 0; j < colorgradient->flash_cnt; i++, j++)
		{
			memcpy(&colorgradient->init_pwm[i], &colorgradient->rgbww_pwm_arr[j], sizeof(Effect_PulseWightRGBWW));
		}

		for (i = 0; i < EFFECT_PIXEL_NUM; i++)
		{
			colorgradient->min_pwm[i].r = light_effect.min_pwm.r + (colorgradient->init_pwm[i].r - (int32_t)light_effect.min_pwm.r) * 0.8f * 0.001f * colorgradient->min_lightness;
			colorgradient->min_pwm[i].g = light_effect.min_pwm.g + (colorgradient->init_pwm[i].g - (int32_t)light_effect.min_pwm.g) * 0.8f * 0.001f * colorgradient->min_lightness;
			colorgradient->min_pwm[i].b = light_effect.min_pwm.b + (colorgradient->init_pwm[i].b - (int32_t)light_effect.min_pwm.b) * 0.8f * 0.001f * colorgradient->min_lightness;
			colorgradient->min_pwm[i].ww = light_effect.min_pwm.ww + (colorgradient->init_pwm[i].ww - (int32_t)light_effect.min_pwm.ww) * 0.8f * 0.001f * colorgradient->min_lightness;
			colorgradient->min_pwm[i].cw = light_effect.min_pwm.cw + (colorgradient->init_pwm[i].cw - (int32_t)light_effect.min_pwm.cw) * 0.8f * 0.001f * colorgradient->min_lightness;
		}
		colorgradient->data_updated = 0;
	}
	if(colorgradient->run_stage == 0)  //渐亮
	{
		for (i = 0; i < EFFECT_PIXEL_NUM; i++)
		{
			colorgradient->liner[i].pwm_start = &colorgradient->min_pwm[i];
			colorgradient->liner[i].pwm_end = &colorgradient->init_pwm[i];
			colorgradient->liner[i].time_start = 0;
			colorgradient->liner[i].time_end = colorgradient->work_tick/2;
			colorgradient->liner[i].whole_time = colorgradient->work_tick/2;
			colorgradient->liner[i].pwm_current = &colorgradient->pwm_current[i];
			colorgradient->liner[i].run_time = &colorgradient->run_time[i];
			colorgradient->liner[i].data_update = &colorgradient->deal_data_update[i];
			res = Effect_Pwm_LinearChange(&colorgradient->liner[i]);
		}
		if(res) 	//该工作阶段结束，进入第二阶段
		{
			colorgradient->run_stage = 1;
		}
	}
	else  //渐灭
	{
		for (i = 0; i < EFFECT_PIXEL_NUM; i++)
		{
			colorgradient->liner[i].pwm_start = &colorgradient->init_pwm[i];
			colorgradient->liner[i].pwm_end = &colorgradient->min_pwm[i];
			colorgradient->liner[i].time_start = colorgradient->work_tick/2;
			colorgradient->liner[i].time_end = colorgradient->work_tick;
			colorgradient->liner[i].whole_time = colorgradient->work_tick;
			colorgradient->liner[i].pwm_current = &colorgradient->pwm_current[i];
			colorgradient->liner[i].run_time = &colorgradient->run_time[i];
			colorgradient->liner[i].data_update = &colorgradient->deal_data_update[i];
			res = Effect_Pwm_LinearChange(&colorgradient->liner[i]);
		}
		if(res) 	//该工作阶段结束，进入第一阶段
		{
			colorgradient->run_stage = 0;
			memset(colorgradient->run_time, 0, sizeof(colorgradient->run_time));
			if (++colorgradient->flash_cnt >= EFFECT_PIXEL_NUM)
			{
				colorgradient->flash_cnt = 0;
			}
			for (i = 0, j = colorgradient->flash_cnt; j < EFFECT_PIXEL_NUM; i++, j++)
			{
				memcpy(&colorgradient->init_pwm[i], &colorgradient->rgbww_pwm_arr[j], sizeof(Effect_PulseWightRGBWW));
			}
			for (j = 0; j < colorgradient->flash_cnt; i++, j++)
			{
				memcpy(&colorgradient->init_pwm[i], &colorgradient->rgbww_pwm_arr[j], sizeof(Effect_PulseWightRGBWW));
			}
			for (i = 0; i < EFFECT_PIXEL_NUM; i++)
			{
				colorgradient->min_pwm[i].r = light_effect.min_pwm.r + (colorgradient->init_pwm[i].r - (int32_t)light_effect.min_pwm.r) * 0.8f * 0.001f * colorgradient->min_lightness;
				colorgradient->min_pwm[i].g = light_effect.min_pwm.g + (colorgradient->init_pwm[i].g - (int32_t)light_effect.min_pwm.g) * 0.8f * 0.001f * colorgradient->min_lightness;
				colorgradient->min_pwm[i].b = light_effect.min_pwm.b + (colorgradient->init_pwm[i].b - (int32_t)light_effect.min_pwm.b) * 0.8f * 0.001f * colorgradient->min_lightness;
				colorgradient->min_pwm[i].ww = light_effect.min_pwm.ww + (colorgradient->init_pwm[i].ww - (int32_t)light_effect.min_pwm.ww) * 0.8f * 0.001f * colorgradient->min_lightness;
				colorgradient->min_pwm[i].cw = light_effect.min_pwm.cw + (colorgradient->init_pwm[i].cw - (int32_t)light_effect.min_pwm.cw) * 0.8f * 0.001f * colorgradient->min_lightness;
			}
		}			
	}
	
	for (i = 0; i < EFFECT_PIXEL_NUM; i++)
	{
		Effect_SetPixel_PulseWight(colorgradient->pwm_current[i], i);
	}
	
	return 1;
}

void Effect_ColorGradient_Init(const void* effect, uint8_t restart)
{
	Effect_Init(effect, &effect_colorgradient, ColorGradient_Init, restart);
}
void Effect_ColorGradient_Process(uint8_t tick, uint8_t restart)
{
	static Effect_PulseWightRGBWW current_pwm = {0};
	
	if (ColorGradient_Process(&effect_colorgradient, &current_pwm, tick, restart) == 1)
	{
		Effect_ActivePixel_PulseWight();
	}
}

typedef struct{
	
	Effect_PulseWightRGBWW *rgbww_pwm_arr;
	uint16_t  init_lightness[2];  	/*亮度<0-1000>*/
	uint16_t  min_lightness;  	/*亮度<0-1000>*/
	uint8_t   color_channel[2];
	uint8_t   speed;  			/*<1-10>,*/
	uint8_t   direction;  		/*方向，0：向左，1：向右*/
	uint8_t flash_cnt;
	float work_tick;
	uint8_t deal_data_update[2];
	uint8_t data_updated;
	Effect_PulseWightRGBWW init_pwm[2];
	Effect_PulseWightRGBWW min_pwm[2];
	Effect_PulseWightRGBWW pwm_current[2];
	float run_time[2];
	LinearChange_Struct liner[2];
	uint8_t run_stage;
	Light_Effect init_effect;
}Effect_OneColorChase;
Effect_OneColorChase effect_onecolorchase = {0};

void OneColorChase_Init(void *effect_mode, const Light_Effect* effect, Effect_PulseWightRGBWW *rgbww_pwm, uint8_t restart)
{
	uint16_t index = 0;
	uint8_t data_updated = 0;
	Effect_OneColorChase *onecolorchase = (Effect_OneColorChase *)effect_mode;

	if (onecolorchase == NULL || effect == NULL || rgbww_pwm == NULL)
	{
		return;
	}
	
	for (index = 0; index < 2; index++)
	{
		rgbww_pwm[index] = onecolorchase->rgbww_pwm_arr[index];
		if(restart == 1 || memcmp(onecolorchase->init_effect.effect_arg.one_color_chase_arg.color_arg, \
			effect->effect_arg.one_color_chase_arg.color_arg, sizeof(onecolorchase->init_effect.effect_arg.one_color_chase_arg.color_arg)) != 0)
		{
			data_updated = 1;
			switch(effect->effect_arg.one_color_chase_arg.color_arg[index].mode)
			{
				case EffectModeCCT:
					Effect_CCT2PulseWight(effect->effect_arg.one_color_chase_arg.color_arg[index].arg.cct_arg.cct, 
							effect->effect_arg.one_color_chase_arg.color_arg[index].arg.cct_arg.gm, effect->effect_arg.one_color_chase_arg.color_arg[index].arg.cct_arg.lightness, &rgbww_pwm[index]);
					break;
				case EffectModeHSI:
					Effect_HSI2PulseWight(effect->effect_arg.one_color_chase_arg.color_arg[index].arg.hsi_arg.hue, 
							effect->effect_arg.one_color_chase_arg.color_arg[index].arg.hsi_arg.sat, effect->effect_arg.one_color_chase_arg.color_arg[index].arg.hsi_arg.lightness, &rgbww_pwm[index], effect->effect_arg.one_color_chase_arg.color_arg[index].arg.hsi_arg.cct);
					break;
				case EffectModeBlack:
					memset(&rgbww_pwm[index], 0, sizeof(Effect_PulseWightRGBWW));
					break;
				default:
					data_updated = 0;
					break;
			}
		}
	}
	
	memcpy(onecolorchase->init_effect.effect_arg.one_color_chase_arg.color_arg, \
			effect->effect_arg.one_color_chase_arg.color_arg, sizeof(onecolorchase->init_effect.effect_arg.one_color_chase_arg.color_arg));
	__disable_irq();
	if(data_updated == 1)
	{
		onecolorchase->data_updated = 1;
		memset(onecolorchase->deal_data_update, 1, sizeof(onecolorchase->deal_data_update));
		onecolorchase->rgbww_pwm_arr = rgbww_pwm;
	}
	onecolorchase->speed = (effect->effect_arg.one_color_chase_arg.speed <= 10) ? (effect->effect_arg.one_color_chase_arg.speed) : 10;
	onecolorchase->direction =  effect->effect_arg.one_color_chase_arg.direction;
	__enable_irq();
}

uint8_t OneColorChase_Process(Effect_OneColorChase *onecolorchase, Effect_PulseWightRGBWW *current_pwm, uint8_t tick, uint8_t restart)
{
	uint8_t i;
	uint8_t res = 0;
	
	if (onecolorchase == NULL || current_pwm == NULL)
	{
		return 0;
	}
	onecolorchase->work_tick = one_color_chase_wt[onecolorchase->speed];
	if(restart == 1 || onecolorchase->data_updated == 1)
	{
		if (restart == 1)
		{
			onecolorchase->flash_cnt = 0;
			memset(onecolorchase->run_time, 0, sizeof(onecolorchase->run_time));
			memset(onecolorchase->pwm_current, 0, sizeof(onecolorchase->pwm_current));
		}
		memcpy(&onecolorchase->init_pwm[onecolorchase->flash_cnt], &onecolorchase->rgbww_pwm_arr[0], sizeof(Effect_PulseWightRGBWW));
		memcpy(&onecolorchase->init_pwm[onecolorchase->flash_cnt], &onecolorchase->rgbww_pwm_arr[0], sizeof(Effect_PulseWightRGBWW));
		
		for (i = 0; i < 2; i++)
		{
			memcpy(&onecolorchase->init_pwm[i], &onecolorchase->rgbww_pwm_arr[i], sizeof(Effect_PulseWightRGBWW));
		}
		onecolorchase->data_updated = 0;
	}
	for (i = 0; i < 2; i++)
	{
		onecolorchase->liner[i].pwm_start = &onecolorchase->init_pwm[i];
		onecolorchase->liner[i].pwm_end = &onecolorchase->init_pwm[i];
		onecolorchase->liner[i].time_start = 0;
		onecolorchase->liner[i].time_end = onecolorchase->work_tick;
		onecolorchase->liner[i].whole_time = onecolorchase->work_tick;
		onecolorchase->liner[i].pwm_current = &onecolorchase->pwm_current[i];
		onecolorchase->liner[i].run_time = &onecolorchase->run_time[i];
		onecolorchase->liner[i].data_update = &onecolorchase->deal_data_update[i];
		res = Effect_Pwm_LinearChange(&onecolorchase->liner[i]);
	}
	
	for (i = 0; i < 4; i++)
	{
		if (onecolorchase->direction == 0)
		{
			if((3 - onecolorchase->flash_cnt) == i)
			{
				Effect_SetPixel_PulseWight(onecolorchase->pwm_current[0], i);
			}
			else
			{
				Effect_SetPixel_PulseWight(onecolorchase->pwm_current[1], i);
			}
		}
		else
		{
			if (onecolorchase->flash_cnt == i)
			{
				Effect_SetPixel_PulseWight(onecolorchase->pwm_current[0], i);
			}
			else
			{
				Effect_SetPixel_PulseWight(onecolorchase->pwm_current[1], i);
			}
		}
	}
	if(res) 	//该工作阶段结束，进入第二阶段
	{
		if (++onecolorchase->flash_cnt >= 4)
		{
			onecolorchase->flash_cnt = 0;
		}
		memset(onecolorchase->run_time, 0, sizeof(onecolorchase->run_time));
	}
	return 1;
}

void Effect_OneColorChase_Init(const void* effect, uint8_t restart)
{
	Effect_Init(effect, &effect_onecolorchase, OneColorChase_Init, restart);
}
void Effect_OneColorChase_Process(uint8_t tick, uint8_t restart)
{
	static Effect_PulseWightRGBWW current_pwm = {0};
	
	if (OneColorChase_Process(&effect_onecolorchase, &current_pwm, tick, restart) == 1)
	{
		Effect_ActivePixel_PulseWight();
	}
}

typedef struct{
	
	Effect_PulseWightRGBWW *rgbww_pwm_arr;
	uint8_t   proc_frq;
	float whole_tick;
	float work_tick;
	uint8_t deal_data_update;
	Effect_PulseWightRGBWW init_pwm;
	Effect_PulseWightRGBWW pwm_current;
	float run_time;
	LinearChange_Struct liner;
	Light_Effect init_effect;
}Effect_DMXStrobe;

Effect_DMXStrobe effect_dmx_strobe;
void DMXStrobe_Init(void *effect_mode, const Light_Effect* effect, Effect_PulseWightRGBWW *rgbww_pwm, uint8_t restart)
{
	int change = 1;
	Effect_DMXStrobe *dmx_strobe = (Effect_DMXStrobe *)effect_mode;
	
	if (dmx_strobe == NULL || effect == NULL || rgbww_pwm == NULL)
	{
		return;
	}
	
	if(restart == 1 || effect->effect_arg.dmx_strobe_arg.mode != dmx_strobe->init_effect.effect_arg.dmx_strobe_arg.mode 
				|| memcmp(&effect->effect_arg.dmx_strobe_arg.arg, &dmx_strobe->init_effect.effect_arg.dmx_strobe_arg.arg, sizeof(Effect_Mode_Arg)) != 0)  //重新初始化参数
	{
		switch(effect->effect_arg.dmx_strobe_arg.mode)
		{
			case EffectModeCCT:
				Effect_CCT2PulseWight(effect->effect_arg.dmx_strobe_arg.arg.cct_arg.cct, effect->effect_arg.dmx_strobe_arg.arg.cct_arg.gm, 
											effect->effect_arg.dmx_strobe_arg.arg.cct_arg.lightness, &rgbww_pwm[0]);
				break;
			#if (PRODUCT_FUNCTION == 2 || PRODUCT_FUNCTION == 3) //简单彩色模式和全功能彩色模式有hsi功能
			case EffectModeHSI:
				Effect_HSI2PulseWight(effect->effect_arg.dmx_strobe_arg.arg.hsi_arg.hue, effect->effect_arg.dmx_strobe_arg.arg.hsi_arg.sat, 
										effect->effect_arg.dmx_strobe_arg.arg.hsi_arg.lightness, &rgbww_pwm[0], effect->effect_arg.dmx_strobe_arg.arg.hsi_arg.cct);
				break;
			#endif /*RODUCT_FUNCTION == 2 || PRODUCT_FUNCTION == 3*/
			
			#if (PRODUCT_FUNCTION == 3) //全功能彩色模式有gel功能和xy色坐标功能
			case EffectModeGEL:
				Effect_Gel2PulseWight(effect->effect_arg.dmx_strobe_arg.arg.gel_arg.brand, effect->effect_arg.dmx_strobe_arg.arg.gel_arg.type, 
						effect->effect_arg.dmx_strobe_arg.arg.gel_arg.cct, effect->effect_arg.dmx_strobe_arg.arg.gel_arg.color, effect->effect_arg.dmx_strobe_arg.arg.gel_arg.lightness, &rgbww_pwm[0]);
				break;
			case EffectModeCoord:
				Effect_Coord2PulseWight(effect->effect_arg.dmx_strobe_arg.arg.coord_arg.x, effect->effect_arg.dmx_strobe_arg.arg.coord_arg.y, effect->effect_arg.dmx_strobe_arg.arg.coord_arg.lightness, &rgbww_pwm[0]);
				break;
			case EffectModeSource:
				Effect_Source2PulseWight(effect->effect_arg.dmx_strobe_arg.arg.source_arg.type, effect->effect_arg.dmx_strobe_arg.arg.source_arg.x,\
						effect->effect_arg.dmx_strobe_arg.arg.source_arg.y, effect->effect_arg.dmx_strobe_arg.arg.source_arg.lightness, 1, &rgbww_pwm[0]);
				break;
			#endif /*PRODUCT_FUNCTION == 3*/
			case EffectModeRGB:
				Effect_RGB2PulseWight(effect->effect_arg.dmx_strobe_arg.arg.rgb_arg.lightness, effect->effect_arg.dmx_strobe_arg.arg.rgb_arg.r, \
										effect->effect_arg.dmx_strobe_arg.arg.rgb_arg.g, effect->effect_arg.dmx_strobe_arg.arg.rgb_arg.b, &rgbww_pwm[0]);
				break;
			case EffectModeRGBWW:
				Effect_RGBWW2PulseWight(effect->effect_arg.dmx_strobe_arg.arg.rgbww_arg.lightness, effect->effect_arg.dmx_strobe_arg.arg.rgbww_arg.r, effect->effect_arg.dmx_strobe_arg.arg.rgbww_arg.g,
										effect->effect_arg.dmx_strobe_arg.arg.rgbww_arg.b, effect->effect_arg.dmx_strobe_arg.arg.rgbww_arg.ww, effect->effect_arg.dmx_strobe_arg.arg.rgbww_arg.cw,&rgbww_pwm[0]);
				break;
			case EffectModeColorMixing:
				Effect_ColorMixing2PulseWight((void *)&effect->effect_arg.dmx_strobe_arg.arg.mixing_arg, &rgbww_pwm[0]);
				break;
			default:
				change = 0;
				break;
		}
		
		if (change == 1)
		{
			__disable_irq();
			dmx_strobe->rgbww_pwm_arr = rgbww_pwm; ////更新pwm数据
			__enable_irq();
		}
	}
	
	__disable_irq();
	dmx_strobe->deal_data_update = 0;
	memcpy(&dmx_strobe->init_effect, effect, sizeof(Light_Effect));
	__enable_irq();
	
}

void Effect_DMXStrobe_Init(const void* effect, uint8_t restart)
{
	Effect_Init(effect, &effect_dmx_strobe, DMXStrobe_Init, restart);
}
void DMXStrobe_Process(Effect_DMXStrobe *dmx_strobe, Effect_PulseWightRGBWW *current_pwm, uint8_t tick, uint8_t restart)
{
	uint8_t res = 0;
	
	if (dmx_strobe == NULL || current_pwm == NULL)
	{
		return;
	}

	if(restart == 1 || dmx_strobe->proc_frq != dmx_strobe->init_effect.effect_arg.dmx_strobe_arg.frq)
	{	
		dmx_strobe->deal_data_update = 1;
		if(restart == 1)
		{
			dmx_strobe->run_time = 0;
			dmx_strobe->deal_data_update = 0;
		}
		restart = 0;
		dmx_strobe->proc_frq = dmx_strobe->init_effect.effect_arg.dmx_strobe_arg.frq;
		dmx_strobe->whole_tick = dmx_strobe->proc_frq <= 0 ? 0 : 1000 / dmx_strobe->proc_frq;
		dmx_strobe->work_tick = dmx_strobe->whole_tick / 2;
		//dmx_strobe->init_pwm = dmx_strobe->rgbww_pwm_arr;
		memcpy(&dmx_strobe->init_pwm, dmx_strobe->rgbww_pwm_arr, sizeof(dmx_strobe->init_pwm));
	}
	dmx_strobe->liner.pwm_start = &dmx_strobe->init_pwm;
	dmx_strobe->liner.pwm_end = &dmx_strobe->init_pwm;
	dmx_strobe->liner.time_start = 0;
	dmx_strobe->liner.time_end = dmx_strobe->work_tick;
	dmx_strobe->liner.whole_time = dmx_strobe->whole_tick;
	dmx_strobe->liner.pwm_current = &dmx_strobe->pwm_current;
	dmx_strobe->liner.run_time = &dmx_strobe->run_time;
	dmx_strobe->liner.data_update = &dmx_strobe->deal_data_update;
	res = Effect_Pwm_LinearChange(&dmx_strobe->liner); //保持亮度一定时间
	if(res) //该工作周期结束，重新设置工作时间
	{
		dmx_strobe->run_time = 0;
		memcpy(&dmx_strobe->init_pwm, dmx_strobe->rgbww_pwm_arr, sizeof(dmx_strobe->init_pwm));
	}
	memcpy(current_pwm, &dmx_strobe->pwm_current, sizeof(Effect_PulseWightRGBWW));
}

void Effect_DMXStrobe_Process(uint8_t tick, uint8_t restart)
{
	static Effect_PulseWightRGBWW current_pwm = {0};
	
	if(restart)
	{
		memset(&current_pwm, 0, sizeof(Effect_PulseWightRGBWW));
	}
	
	DMXStrobe_Process(&effect_dmx_strobe, &current_pwm, tick, restart);
	Effect_SetPixel_PulseWight(current_pwm, 0xff);
	Effect_ActivePixel_PulseWight();
}

typedef struct{
	void (*Effect_Init)(const void *, uint8_t);
	void (*Effect_Process)(uint8_t, uint8_t);
}Effect_Sub_Typedef;

Effect_Sub_Typedef effect_sub[] = {
	
	{Effect_ClubLights_Init, Effect_ClubLights_Process},
	{Effect_Paparazzi_Init, Effect_Paparazzi_Process},
	{Effect_Lightning_Init, Effect_Lightning_Process},
	{Effect_TV_Init, Effect_TV_Process},
	{Effect_Candle_Init, Effect_Candle_Process},
	{Effect_Fire_Init, Effect_Fire_Process},
	{Effect_Strobe_Init, Effect_Strobe_Process},
	{Effect_Explosion_Init, Effect_Explosion_Process},
	{Effect_FaultBulb_Init, Effect_FaultBulb_Process},
	{Effect_Pulsing_Init, Effect_Pulsing_Process},
	{Effect_Welding_Init, Effect_Welding_Process},
	{Effect_CopCar_Init, Effect_CopCar_Process},
	{Effect_ColorChase_Init, Effect_ColorChase_Process},
	{Effect_PartyLights_Init, Effect_PartyLights_Process},
	{Effect_Fireworks_Init, Effect_Fireworks_Process},
	{NULL, Effect_Off_Process},
	{Effect_IAmHere_Init, Effect_IAmHere_Process},
    
	{Effect_DMXStrobe_Init, Effect_DMXStrobe_Process},
	{Effect_PaparazziII_Init, Effect_Paparazzi_Process},
	{Effect_LightningII_Init, Effect_Lightning_Process},
	{Effect_TVII_Init, Effect_TV_Process},
	{NULL, NULL},
	{Effect_FireII_Init, Effect_Fire_Process},
	{Effect_StrobeII_Init, Effect_Strobe_Process},
	{Effect_ExplosionII_Init, Effect_Explosion_Process},
	{Effect_FaultBulbII_Init, Effect_FaultBulb_Process},
	{Effect_PulsingII_Init, Effect_Pulsing_Process},
	{Effect_WeldingII_Init, Effect_Welding_Process},
	{Effect_CopCarII_Init, Effect_CopCar_Process},
	{Effect_PartyLightsII_Init, Effect_PartyLights_Process},
	{Effect_FireworksII_Init, Effect_Fireworks_Process},
	{Effect_LightningIII_Init, Effect_Lightning_Process},
	{Effect_TVIII_Init, Effect_TV_Process},
	{Effect_FireIII_Init, Effect_Fire_Process},
	{Effect_FaultBulbIII_Init, Effect_FaultBulb_Process},
	{Effect_PulsingIII_Init, Effect_Pulsing_Process},
	{Effect_CopCarIII_Init, Effect_CopCar_Process},
};

//光效数据初始化
void Effect_Data_Init(const Light_Effect* effect, uint8_t restart)
{
	uint8_t force_restart = 0;
	if (restart || light_effect.type != effect->effect_type)
	{
		force_restart = 1;
	}
	if (effect->effect_type < EffectTypeNULL && effect->effect_type < sizeof(effect_sub)/sizeof(Effect_Sub_Typedef))
	{
		if(effect_sub[effect->effect_type].Effect_Init != NULL)
		{
			effect_sub[effect->effect_type].Effect_Init(effect, force_restart);
		}
		if (force_restart == 1)
		{
			light_effect.restart = 1;
		}
		light_effect.type = effect->effect_type;
		__enable_irq();
	}
}

void Effect_Deal(uint8_t tick, uint8_t restart)		// 处理光效
{
	uint8_t force_restart = 0;
	if (restart == 1 || light_effect.restart == 1)
	{
		force_restart = 1;
		light_effect.restart = 0;
	}
	if(light_effect.type < EffectTypeNULL)
	{
		if(effect_sub[light_effect.type].Effect_Process != NULL)
		{
			effect_sub[light_effect.type].Effect_Process(tick, force_restart);
		}
	}
}

void Light_Effect_Enable(bool en)
{
	light_effect_enable = en;
	if (light_effect_enable == false)
	{
		light_effect.type = EffectTypeNULL;
	}
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

