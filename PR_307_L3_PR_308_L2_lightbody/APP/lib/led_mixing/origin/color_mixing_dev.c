/*********************************************************************************
  *Copyright(C), 2023, Aputure, All rights reserver.
  *FileName:  	color_mixing_dev.c
  *Author:    	
  *Version:   	v3.0.2
  *Date:      	2023/03/30
  *Description: 合光
  *History:  
			-v3.0.0: 2022/04/16,Steven,初始版本
            -v3.0.1: 2022/06/07,Steven,将输出模式、调光曲线、功率限制加到光模式接口中
            -v3.0.2: 2023/03/30,Matthew
                     1.新增wchar32、enum int类型库（编译器设置枚举类型为int类型时使用）
                     2.修复传入亮度递增时输出亮度可能回退的bug
**********************************************************************************/
#include "color_mixing_dev.h"
#include "define.h"
#include "project_config.h"
#include "bsp_electronic.h"
struct mixing_dev fivecolor_mixing_dev_t;
struct mixing_dev threecolor_mixing_dev_t;
/* 
功率百分比和照度百分比曲线多项式系数
Pn(x) =  a0 + a1*x + a*2x^2 + a3*x^3
功率与照度线性度较好时可认为是线性关系，即为：
a0 = 0, a1 = 1, a2 = 0, a3 = 0,
得：Pn(x) = x, 即为照度为1时功率为1，照度为0.5时功率也为0.5
如模拟调光等线性度不好时，需要采集若干数据到Excel拟合曲线得到

*/ 
#ifdef PR_307_L3
static const float power_curve_coef[5][4] = 
{
//  a0,a1,a2,a3
    {0, 1, 0, 0},
    {0, 1, 0, 0},
    {0, 1, 0, 0},
    {0, 1, 0, 0},
    {0, 1, 0, 0},
};

static const float power_analog_curve_coef[5][4] = 
{
//  a0,a1,a2,a3
    {-0.005435, 0.615234, -0.407291, 0.781436},
    {-0.006541, 0.387933, -0.280473, 0.886476},
    { 0.000371, 0.505745,  0.491551, 0.002640},
    {-0.000641, 0.670255,  0.150787, 0.178286},
    {-0.001038, 0.693834,  0.108372, 0.196770},
};
#endif

#ifdef PR_308_L2
/*
y = 0.372805 x3.000000  + 0.285855 x2.000000  + 0.341855 x - 0.001565 
y = 0.208019 x3.000000  + 0.126209 x2.000000  + 0.664040 x - 0.001564 
y = 0.255669 x3.000000  + 0.035180 x2.000000  + 0.708032 x - 0.002971 

*/
static const float power_curve_coef[3][4] = 
{
//  a0,a1,a2,a3
    {0, 1, 0, 0},
    {0, 1, 0, 0},
    {0, 1, 0, 0},
};

static const float power_analog_curve_coef[3][4] = 
{
//  a0,a1,a2,a3
    {-0.001565, 0.341855, 0.285855, 0.372805},
    {-0.001564, 0.664040, 0.126209, 0.208019},
    {-0.002971, 0.708032, 0.035180, 0.255669},
};
#endif
//290 50 500 0 50 500 
void color_mixing_init(uint8_t mode)
{
	struct mixing_led_cfg led_cfg_array[5];
	struct mixing_cfg mixing_cfg_t;
	//led config
	#ifdef PR_307_L3
	if(mode == 0)
	{
		led_cfg_init(&led_cfg_array[0], "Red", led_base_data.data[0]->max_current, power_curve_coef[0], led_base_data.data[0]->length, led_base_data.data[0]->data);
		led_cfg_init(&led_cfg_array[1], "Gree", led_base_data.data[1]->max_current, power_curve_coef[1], led_base_data.data[1]->length, led_base_data.data[1]->data);
		led_cfg_init(&led_cfg_array[2], "Blue", led_base_data.data[2]->max_current, power_curve_coef[2], led_base_data.data[2]->length, led_base_data.data[2]->data);
		led_cfg_init(&led_cfg_array[3], "Warm_White", led_base_data.data[3]->max_current, power_curve_coef[3], led_base_data.data[3]->length, led_base_data.data[3]->data);
		led_cfg_init(&led_cfg_array[4], "Cool_White", led_base_data.data[4]->max_current, power_curve_coef[4], led_base_data.data[4]->length, led_base_data.data[4]->data);
	}
	else{
		led_cfg_init(&led_cfg_array[0], "Red", analog_led_base_data.data[0]->max_current, power_analog_curve_coef[0], analog_led_base_data.data[0]->length, analog_led_base_data.data[0]->data);
		led_cfg_init(&led_cfg_array[1], "Gree", analog_led_base_data.data[1]->max_current, power_analog_curve_coef[1], analog_led_base_data.data[1]->length, analog_led_base_data.data[1]->data);
		led_cfg_init(&led_cfg_array[2], "Blue", analog_led_base_data.data[2]->max_current, power_analog_curve_coef[2], analog_led_base_data.data[2]->length, analog_led_base_data.data[2]->data);
		led_cfg_init(&led_cfg_array[3], "Warm_White", analog_led_base_data.data[3]->max_current, power_analog_curve_coef[3], analog_led_base_data.data[3]->length, analog_led_base_data.data[3]->data);
		led_cfg_init(&led_cfg_array[4], "Cool_White", analog_led_base_data.data[4]->max_current, power_analog_curve_coef[4], analog_led_base_data.data[4]->length, analog_led_base_data.data[4]->data);
	}
	//five color mixing device init
	mixing_cfg_t.dev_name = "five_color";
	mixing_cfg_t.min_lux = 0;
	mixing_cfg_t.rgb_min_lux = 100;
	mixing_cfg_t.max_power = (mode == 0) ? led_base_data.max_total_current : analog_led_base_data.max_total_current;
	mixing_cfg_t.led_num = 5;
	mixing_cfg_t.led[0] = &led_cfg_array[0];
	mixing_cfg_t.led[1] = &led_cfg_array[1];
	mixing_cfg_t.led[2] = &led_cfg_array[2];
	mixing_cfg_t.led[3] = &led_cfg_array[3];
	mixing_cfg_t.led[4] = &led_cfg_array[4];
	mixing_dev_init(&fivecolor_mixing_dev_t, &mixing_cfg_t);
	color_set_min_lux(0);
	#endif
	
	#ifdef PR_308_L2
	if(mode == 0)
	{
		led_cfg_init(&led_cfg_array[0], "Warm_Color", led_base_data.data[0]->max_current, power_curve_coef[0], led_base_data.data[0]->length, led_base_data.data[0]->data);
		led_cfg_init(&led_cfg_array[1], "Green_Color", led_base_data.data[1]->max_current, power_curve_coef[1], led_base_data.data[1]->length, led_base_data.data[1]->data);
		led_cfg_init(&led_cfg_array[2], "Cool_Color", led_base_data.data[2]->max_current, power_curve_coef[2], led_base_data.data[2]->length, led_base_data.data[2]->data);
		if(led_base_data.data[0]->data[led_base_data.data[0]->length - 1].Illuminance <=  led_base_data.data[2]->data[led_base_data.data[2]->length - 1].Illuminance)
		{
			color_set_max_lux(led_base_data.data[0]->data[led_base_data.data[0]->length - 1].Illuminance);
		}
		else
		{
			color_set_max_lux(led_base_data.data[2]->data[led_base_data.data[2]->length - 1].Illuminance);
		}
	}
	else
	{
		led_cfg_init(&led_cfg_array[0], "Warm_Color", analog_led_base_data.data[0]->max_current, power_analog_curve_coef[0], analog_led_base_data.data[0]->length, analog_led_base_data.data[0]->data);
		led_cfg_init(&led_cfg_array[1], "Green_Color", analog_led_base_data.data[1]->max_current, power_analog_curve_coef[1], analog_led_base_data.data[1]->length, analog_led_base_data.data[1]->data);
		led_cfg_init(&led_cfg_array[2], "Cool_Color", analog_led_base_data.data[2]->max_current, power_analog_curve_coef[2], analog_led_base_data.data[2]->length, analog_led_base_data.data[2]->data);
		if(analog_led_base_data.data[0]->data[analog_led_base_data.data[0]->length - 1].Illuminance <=  analog_led_base_data.data[2]->data[analog_led_base_data.data[2]->length - 1].Illuminance)
		{
			color_set_max_lux(analog_led_base_data.data[0]->data[analog_led_base_data.data[0]->length - 1].Illuminance);
		}
		else
		{
			color_set_max_lux(analog_led_base_data.data[2]->data[analog_led_base_data.data[2]->length - 1].Illuminance);
		}
	}
	mixing_cfg_t.dev_name = "three_color";
	mixing_cfg_t.min_lux = 0;
	mixing_cfg_t.rgb_min_lux = 0;
	mixing_cfg_t.max_power = (mode == 0) ? led_base_data.max_total_current : analog_led_base_data.max_total_current;
	mixing_cfg_t.led_num = 3;
	mixing_cfg_t.led[0] = &led_cfg_array[0];
	mixing_cfg_t.led[1] = &led_cfg_array[1];
	mixing_cfg_t.led[2] = &led_cfg_array[2];
	mixing_cfg_t.led[3] = NULL;
	mixing_cfg_t.led[4] = NULL;
	mixing_dev_init(&threecolor_mixing_dev_t, &mixing_cfg_t);
	color_set_min_lux(0);
	#endif
	silence_mode_set_min_lux(g_rs485_data.fan.mode);
}

//静音模式下使用%0.1照度
void silence_mode_set_min_lux(uint8_t fan_mode)
{
	#ifdef PR_308_L2
	if(fan_mode == 4)//静音模式
	{
		threecolor_mixing_dev_t.min_lux = 120;//120~200Lux
	}
	else
	{
		threecolor_mixing_dev_t.min_lux = 0;
	}
	#endif
	#ifdef PR_307_L3
	if(fan_mode == 4)
	{
		fivecolor_mixing_dev_t.min_lux = 55;//35~150
	}
	else
	{
		fivecolor_mixing_dev_t.min_lux = 0;
	}
	#endif
}


/**
  * @brief  CCT求PWM
  * @param  lightness 照度等级，范围<0.0-1.0>，0.5为50%，1为100%
  * @param  cct 色温，范围<1600-10000>，单位k
  * @param  duv Duv，需将GM转为Duv
  * @param  ilumination_mode 光输出模式，详见enum ilumination_mode
  * @param  curve 调光曲线，详见enum mixing_curve
  * @param  power_limit 功率限制，范围<0.0-1.0>，1.0表示无限制，0.1表示限制为10%
  * @param  pwm 计算结果，PWM，用于设置定时器
  * @retval none
  */
#ifndef Reflector_Compensation
//const uint16_t Without_Reflector_Data[] = {1989,	2073,	2103,	2161,	2223,	2291,	2344,	2396,	2446,	2498,	
//				2548,	2598,	2649,	2700,	2751,	2799,	2851,	2906,	2956,	3010,	3077,	
//				3076,	3160,	3209,	3259,	3313,	3363,	3415,	3469,	3523,	3576,	3630,	
//				3683,	3738,	3796,	3849,	3904,	3957,	4019,	4075,	4127,	4191,	4255,	
//				4315,	4367,	4428,	4486,	4549,	4607,	4665,	4730,	4789,	4850,	4886,	
//				4949,	5007,	5063,	5127,	5185,	5249,	5306,	5367,	5424,	5491,	5548,	
//				5611,	5678,	5734,	5793,	5857,	5921,	5980,	6043,	6107,	6166,	6224,	
//				6287,	6342,	6406,	6472,	6533,	6596,	6658,	6717,	6777,	6828,	6881,	
//				6958,	7016,	7073,	7139,	7192,	7238,	7307,	7371,	7439,	7501,	7569,	
//				7628,	7629,	7699,	7760,	7823,	7882,	7941,	7997,	8058,	8120,	8180,	
//				8244,	8280,	8341,	8403,	8463,	8535,	8590,	8651,	8709,	8770,	8829,	
//				8891,	8952,	9013,	9072,	9132,	9191,	9247,	9309,	9370,	9428,	9491,	
//				9550,	9608,	9666,	9729,	9790,	9855,	9913,	9977};//2000~8900

//const uint16_t Without_Reflector_Data[] = {1906,	2007,	2066,	2134,	2203,	2279,	2333,	2385,	2437,	2487,	
//	2539,	2590,	2641,	2693,	2744,	2792,	2846,	2900,	2950,	3004,	3071,	3065,	3155,	3203,	3254,	
//	3306,	3359,	3410,	3462,	3518,	3571,	3624,	3676,	3727,	3785,	3837,	3891,	3944,	4005,	4060,	
//	4113,	4176,	4236,	4295,	4350,	4409,	4467,	4526,	4585,	4641,	4705,	4764,	4824,	4876,	4940,	
//	4998,	5053,	5117,	5174,	5239,	5296,	5356,	5413,	5478,	5536,	5598,	5663,	5721,	5777,	5840,	
//	5905,	5965,	6025,	6090,	6149,	6205,	6268,	6324,	6387,	6452,	6513,	6575,	6634,	6693,	6753,	
//	6806,	6858,	6931,	6988,	7046,	7110,	7164,	7206,	7277,	7337,	7405,	7471,	7530,	7594,	7603,	
//	7671,	7732,	7791,	7854,	7910,	7965,	8024,	8087,	8147,	8210,	8247,	8305,	8367,	8426,	8495,	
//	8549,	8610,	8667,	8727,	8789,	8852,	8911,	8972,	9033,	9087,	9142,	9201,	9266,	9322,	9381,	
//	9441,	9503,	9559,	9620,	9679,	9740,	9796,	9859,	9924,	9971,};
#ifdef PR_307_L3
const uint16_t Without_Reflector_Data[] = {1983,	2028,	2078,	2138,	2202,	2268,	2320,	2374,	2423,	2476,
	2526,	2576,	2628,	2678,	2729,	2778,	2830,	2884,	2934,	2987,	3015,	3084,	3137,	3189,	3242,
	3294,	3348,	3398,	3453,	3504,	3557,	3613,	3669,	3726,	3782,	3844,	3900,	3959,	4015,	4074,
	4132,	4190,	4249,	4307,	4371,	4429,	4484,	4545,	4608,	4665,	4720,	4786,	4848,	4907,	4966,
	5032,	5096,	5157,	5221,	5283,	5342,	5406,	5471,	5530,	5595,	5664,	5724,	5790,	5851,	5914,
	5980,	6042,	6105,	6169,	6228,	6297,	6360,	6428,	6485,	6556,	6617,	6677,	6749,	6778,	6868,
	6937,	7000,	7059,	7130,	7193,	7255,	7324,	7398,	7452,	7522,	7594,	7651,	7724,	7733,	7806,
	7876,	7930,	7997,	8067,	8132,	8195,	8251,	8324,	8392,	8461,	8503,	8573,	8644,	8710,	8783,
	8845,	8910,	8976,	9045,	9111,	9184,	9251,	9316,	9382,	9449,	9516,	9580,	9647,	9710,	9785,
	9849,	9914,	9981,};


const float Without_Reflector_DUV_Data[] = {0.0012,0.0015,0.0020,0.0025,0.0030,0.0035,0.0040,0.0044,0.0046,0.0048,0.0050,
                                               0.0050,0.0051,0.0051,0.0051,0.0051,0.0051,0.0051,0.0051,0.0053,0.0052,0.0052,
                                               0.0051,0.0049,0.0047,0.0046,0.0044,0.0044,0.0042,0.0042,0.0040,0.0040,0.0040,
                                               0.0040,0.0040,0.0038,0.0037,0.0035,0.0035,0.0034,0.0033};//2000~10000,CCT200K步进
#endif
#ifdef PR_308_L2
const uint16_t Without_Reflector_Data[] = {2678,	2729,	2778,	2830,	2884,	2934,	2987,	3015,	3084,	3137,	3189,	3242,
					3294,	3348,	3398,	3453,	3504,	3557,	3613,	3669,	3726,	3782,	3844,	3900,	3959,	4015,	4074,
					4132,	4190,	4249,	4307,	4371,	4429,	4484,	4545,	4608,	4665,	4720,	4786,	4848,	4907,	4966,
					5032,	5096,	5157,	5221,	5283,	5342,	5406,	5471,	5530,	5595,	5664,	5724,	5790,	5851,	5914,
					5980,	6042,	6105,	6169,	6228,	6297,	6360,	6428,	6485};


const float Without_Reflector_DUV_Data[] = {0.0028,0.0033,0.0038,0.0042,0.0045,0.0047,0.0049,0.0050,
											0.0051,0.0051,0.0051,0.0051,0.0051,0.0051,0.0051,0.0052,0.0053,0.0052,0.0052,
											0.0050};//2700~6500,CCT200K步进
#endif
static float map(float value, float input_min, float input_max, float output_min, float output_max) 
{
    return output_min + ((value - input_min) / (input_max - input_min)) * (output_max - output_min);
}
uint16_t binaryInsertPosition(const uint16_t arr[], uint16_t n, uint16_t target) 
{
    uint16_t start = 0;
    uint16_t end = n - 1;
	uint16_t mid = 0;
    while (start <= end) {
		
			mid = (start + end)/2;
			if (target < arr[mid])
			{
				
				end = mid;
			}
			else if (target > arr[mid])
			{
				start = mid;
			}
			else
			{
				return mid;
			}
			
			if((start + 1 == end)) 
			{
				return start;
			}
    }
    return start;  // 数值不存在于数组中，返回插入位置
}


#endif

void color_cct_to_pwm(float lightness, uint16_t cct, float duv, 
                        uint8_t ilumination, uint8_t curve, float power_limit, struct mixing_pwm *pwm)
{
#ifdef PR_307_L3
	uint16_t Position = 0;
	float input_min = 0;
	float input_max = 0;
	float output_min = 0;
	float output_max = 0;
	if(Electronic_Buf.Bayonet_Status == 0)//是否没有电子附件3E
	{
		if((cct-MIN_CCT)%200)
		{
			if(Without_Reflector_DUV_Data[((cct-MIN_CCT)/200) + 1] >= Without_Reflector_DUV_Data[((cct-MIN_CCT)/200)])
			{
				duv += (Without_Reflector_DUV_Data[((cct-MIN_CCT)/200)] + (Without_Reflector_DUV_Data[((cct-MIN_CCT)/200) + 1] - Without_Reflector_DUV_Data[((cct-MIN_CCT)/200)])*((cct-MIN_CCT)%200)/200);
			}
			else
			{
				duv += (Without_Reflector_DUV_Data[((cct-MIN_CCT)/200)] + (Without_Reflector_DUV_Data[((cct-MIN_CCT)/200)] - Without_Reflector_DUV_Data[((cct-MIN_CCT)/200) + 1])*((cct-MIN_CCT)%200)/200);
			}
			duv -= 0.0012f;
		}
		else
		{
			duv += Without_Reflector_DUV_Data[((cct-MIN_CCT)/200)];
			duv -= 0.0012f;
		}
		if(cct != MAX_CCT)
		{
			Position = binaryInsertPosition(Without_Reflector_Data, sizeof(Without_Reflector_Data)/sizeof(Without_Reflector_Data[0]), cct);
			if(Position != 0)
			{
				input_min = Without_Reflector_Data[Position - 1];
				input_max = Without_Reflector_Data[Position];
				output_min = MIN_CCT + (Position - 1) * 50;
				output_max = MIN_CCT + (Position) * 50;
				cct = (uint16_t)(map((float)cct, input_min, input_max, output_min, output_max));
			}
			else
			{
				cct = MIN_CCT;
			}
		}
		else
		{
			cct = WITHOUT_REFLECTOR_MAX_CCT;
		}
	}

#endif
	#ifdef PR_307_L3
	cct_to_pwm(&fivecolor_mixing_dev_t, lightness, cct, duv, ilumination, curve, power_limit, pwm); //CCT求PWM
	#endif
	
	#ifdef PR_308_L2
	#ifdef COMPENSATE_308
	if(cct > 5000)
	{
		cct -= 70;
		duv += 0.0025f;
	}
	else if(cct > 3600)
	{
		duv += 0.0025f;
	}
	else if(cct > 3000)
	{
		duv += 0.0016f;
	}
	#endif
	cct_to_pwm(&threecolor_mixing_dev_t, lightness, cct, duv, ilumination, curve, power_limit, pwm); //CCT求PWM
	#endif
}
/**
  * @brief  HSi求PWM
  * @param  lightness 照度等级，范围<0.0-1.0>，0.5为50%，1为100%
  * @param  hue 色环，范围<0.0-360.0>
  * @param  sat 饱和度，范围<0.0-100.0>
  * @param  cct 中心色温，范围<1600-10000>，单位k
  * @param  curve 调光曲线，详见enum mixing_curve
  * @param  power_limit 功率限制，范围<0.0-1.0>，1.0表示无限制，0.1表示限制为10%
  * @param  pwm 计算结果，PWM，用于设置定时器
  * @retval none
  */
void color_hsi_to_pwm(float lightness, float hue, float sat, uint16_t cct, uint8_t curve, float power_limit, struct mixing_pwm *pwm)
{
	hsi_to_pwm(&fivecolor_mixing_dev_t, lightness, hue, sat, cct, curve, power_limit, pwm);
}

/**
  * @brief  色坐标求PWM及对应照度
  * @param  lightness 照度等级，范围<0.0-1.0>，0.5为50%，1为100%
  * @param  coord_x 色坐标x，范围<0.0-1.0>
  * @param  coord_y 色坐标y，范围<0.0-1.0>
  * @param  curve 调光曲线，详见enum mixing_curve
  * @param  power_limit 功率限制，范围<0.0-1.0>，1.0表示无限制，0.1表示限制为10%
  * @param  pwm_struct pwm计算结果，PWM，用于设置定时器
  * @param  lux lux计算结果
  * @retval none
  */
void color_coordinate_to_pwm(float lightness, float coord_x, float coord_y, uint8_t curve, float power_limit, struct mixing_pwm *pwm_struct)
{
	struct lux_struct lux;
	coordinate_to_pwm(&fivecolor_mixing_dev_t, lightness, coord_x, coord_y, curve, power_limit, pwm_struct, &lux);
}

/**
  * @brief  gel求PWM
  * @param  lightness 照度等级，范围<0.0-1.0>，0.5为50%，1为100%
  * @param  brand 品牌，0x00：LEE; 0x01：Resco
  * @param  type 类型
  * @param  cct 色温，范围<1600-10000>，单位k
  * @param  color
  * @param  curve 调光曲线，详见enum mixing_curve
  * @param  power_limit 功率限制，范围<0.0-1.0>，1.0表示无限制，0.1表示限制为10%
  * @param  pwm 计算结果，PWM，用于设置定时器
  * @retval none
  */
void color_gel_to_pwm(float lightness, uint8_t brand, uint8_t type, uint16_t cct, uint16_t color, 
                            uint8_t curve, float power_limit, struct mixing_pwm *pwm_struct)
{
	gel_to_pwm(&fivecolor_mixing_dev_t, lightness, brand, type, cct, color, curve, power_limit, pwm_struct); 
}

/**
  * @brief  rgb求PWM
  * @param  dev 合光设备
  * @param  lightness 照度等级，范围<0.0-1.0>，0.5为50%，1为100%
  * @param  r r值，范围<0.0-1.0>
  * @param  g g值，范围<0.0-1.0>
  * @param  b b值，范围<0.0-1.0>
  * @param  curve 调光曲线，详见enum mixing_curve
  * @param  power_limit 功率限制，范围<0.0-1.0>，1.0表示无限制，0.1表示限制为10%
  * @param  pwm_struct 计算结果，PWM，用于设置定时器
  * @retval 0：成功 -1：失败
  */
void color_rgb_to_pwm(float lightness, float r, float g, float b, uint8_t curve, float power_limit, struct mixing_pwm *pwm_struct)
{
	rgb_to_pwm(&fivecolor_mixing_dev_t, lightness, r, g, b, curve, power_limit, pwm_struct);
}

/**
  * @brief  rgbww求PWM
  * @param  r r值，范围<0.0-1.0>
  * @param  g g值，范围<0.0-1.0>
  * @param  b b值，范围<0.0-1.0>
  * @param  ww ww值，范围<0.0-1.0>
  * @param  cw cw值，范围<0.0-1.0>
  * @param  curve 调光曲线，详见enum mixing_curve
  * @param  power_limit 功率限制，范围<0.0-1.0>，1.0表示无限制，0.1表示限制为10%
  * @param  pwm_struct 计算结果，PWM，用于设置定时器
  * @retval none
  */
void color_rgbww_to_pwm(float r, float g, float b, float ww, float cw, uint8_t curve, float power_limit, struct mixing_pwm *pwm_struct)
{
	rgbww_to_pwm(&fivecolor_mixing_dev_t, r, g, b, ww, cw, curve, power_limit, pwm_struct);
}

/**
  * @brief  source求PWM
  * @param  lightness 照度等级，范围<0.0-1.0>，0.5为50%，1为100%
  * @param  type 类型,详见enum source_type
  * @param  coord_x source色坐标x，范围<0.0-1.0>
  * @param  coord_y source色坐标y，范围<0.0-1.0>
  * @param  pwm_struct 计算结果，PWM，用于设置定时器
  * @param  mode 色坐标模式，范围<0-1>，0：根据type查找色坐标，1：使用传参coord_x、coord_y
  * @param  curve 调光曲线，详见enum mixing_curve
  * @param  power_limit 功率限制，范围<0.0-1.0>，1.0表示无限制，0.1表示限制为10%
  * @retval none
  */
void color_source_to_pwm(float lightness, uint8_t type, float coord_x, float coord_y, uint8_t mode, 
                            uint8_t curve, float power_limit, struct mixing_pwm *pwm_struct)
{
	source_to_pwm(&fivecolor_mixing_dev_t, lightness, type, coord_x, coord_y, mode, curve, power_limit, pwm_struct);
}

/**
  * @brief  颜色混合求PWM
  * @param  lightness 照度等级，范围<0.0-1.0>，0.5为50%，1为100%
  * @param  mixing 颜色参数，详见struct color_mixing
  * @param  curve 调光曲线，详见enum mixing_curve
  * @param  power_limit 功率限制，范围<0.0-1.0>，1.0表示无限制，0.1表示限制为10%
  * @param  pwm_struct 计算结果，PWM，用于设置定时器
  * @retval none
  */
void color_mixing_to_pwm(struct color_mixing *mixing, uint8_t curve, float power_limit, struct mixing_pwm *pwm_struct)
{
	mixing_to_pwm(&fivecolor_mixing_dev_t, mixing, curve, power_limit, pwm_struct);
}

/**
  * @brief  获取亮度为0时的最大pwm值
  * @retval PWM数组，数组长度为5
  */
uint16_t *color_get_zerolux_maxpwm(void)
{
	return get_zerolux_maxpwm(&fivecolor_mixing_dev_t);
}

/**
  * @brief  设置最大照度
  * @param  lux 照度
  * @retval none
  */
void color_set_max_lux(float lux)
{
	#ifdef PR_308_L2
	set_max_lux(&threecolor_mixing_dev_t, lux);//Phoebe：会导致恒照度模式不正常
	#endif
	
	#ifdef PR_307_L3
	set_max_lux(&fivecolor_mixing_dev_t, lux);//Phoebe：会导致恒照度模式不正常
	#endif
}

/**
  * @brief  设置非0最小照度
  * @param  lux 照度
  * @retval none
  */
void color_set_min_lux(float lux)
{
	#ifdef PR_307_L3
	set_min_lux(&fivecolor_mixing_dev_t, lux);
	#endif
	
	#ifdef PR_308_L2
	set_min_lux(&threecolor_mixing_dev_t, lux);
	#endif
}

float gm_to_duv(uint8_t gm)
{
#ifdef PR_307_L3
        float gm_temp = 0.0f;
        
        gm_temp = (int8_t)gm < 100 ? (int8_t)gm : 100;
        
		return ((gm_temp - 50) * 0.0004f);
	
#endif
	
#ifdef PR_308_L2
		float gm_temp = 0.0f;
		
		gm_temp = (int8_t)gm < 100 ? (int8_t)gm : 100;
        
        return ((gm_temp - 50) * 0.0001f);
#endif
}
