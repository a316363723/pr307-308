/*********************************************************************************
  *Copyright(C), 2023, Aputure, All rights reserver.
  *FileName:    color_mixing_dev.c
  *Author:      
  *Version:     vv4.0.0
  *Date:        2023/08/28
  *Description: 合光
  *History:  
            -v3.0.0: 2022/04/16,Steven,初始版本
            -v3.0.1: 2022/06/07,Steven,将输出模式、调光曲线、功率限制加到光模式接口中
            -v3.0.2: 2023/03/30,Matthew
                     1.新增wchar32、enum int类型库（编译器设置枚举类型为int类型时使用）
                     2.修复传入亮度递增时输出亮度可能回退的bug
            -v4.0.0: 2023/08/28,Matthew
                     1.加入功率比和照度比曲线多项式系数用于功率计算，功率与照度的曲线需要用excel拟合
                     2.加入照度lux转pwm输出接口，可用于输出结果lux转pwm及输出照度线性缓变过程中lux转pwm
                     3.删除min_lux、rgb_min_lux参数，由新的min_lightness控制合光最小亮度
**********************************************************************************/
#include "color_mixing_dev.h"
#include "define.h"
#include "project_config.h"
#include "bsp_electronic.h"
#include "app_led.h"
#include "flash.h"
struct mixing_dev fivecolor_mixing_dev_t;
struct mixing_dev threecolor_mixing_dev_t;
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


//y = 0.0221x3 + 0.2977x2 + 0.6765x - 0.0043
//y = -0.1922x3 + 0.9844x2 + 0.197x - 0.0028
//y = -0.3143x3 + 0.5964x2 + 0.7116x - 0.0047
//y = -0.0228x3 + 0.1482x2 + 0.8696x - 0.0029
//y = -0.0248x3 + 0.1683x2 + 0.868x - 0.0023
static const float power_curve_coef1[5][4] = 
{
//  a0,a1,a2,a3
    {-0.0043, 0.6765, 0.2977, 0.0221},
    {-0.0028, 0.197, 0.9844,  -0.1922},
    {-0.0047, 0.7116, 0.5964, -0.3143},
    {-0.0029, 0.8696, 0.1482, -0.0228},
    {-0.0023, 0.868, 0.1683, -0.0248},
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
y = 0.3345x3 - 0.1827x2 + 0.833x - 0.0046//ww
y = 0.1412x3 + 0.4901x2 + 0.354x - 0.0018//g
y = 0.225x3 - 0.0758x2 + 0.8459x - 0.0017//cw

*/
//static const float power_curve_coef[3][4] = 
//{
////  a0,a1,a2,a3
//    {-0.0046, 0.833, -0.1827, 0.3345},
//    {-0.0018, 0.354, 0.4901, 0.1412},
//    {-0.0017, 0.8459, -0.0758, 0.225},
//};
static const float power_curve_coef[3][4] = 
{
//  a0,a1,a2,a3
    {0, 1, 0, 0},
    {0, 1, 0, 0},
    {0, 1, 0, 0},
};
/*
y = 0.0016x3 + 0.2939x2 + 0.7376x - 0.0035//WW
y = 0.0181x3 + 0.6395x2 + 0.3303x - 0.0038//G
y = -0.208x3 + 0.5236x2 + 0.693x + 0.0004//CW
*/
static const float power_curve_coef1[3][4] = 
{
//  a0,a1,a2,a3
    {-0.0035, 0.7376, 0.2939, 0.0016},
    {-0.0038, 0.3303, 0.6395, 0.0181},
    {0.0004, 0.693, 0.5236, -0.208},
};
/*
y = 0.372805 x3.000000  + 0.285855 x2.000000  + 0.341855 x - 0.001565 
y = 0.208019 x3.000000  + 0.126209 x2.000000  + 0.664040 x - 0.001564 
y = 0.255669 x3.000000  + 0.035180 x2.000000  + 0.708032 x - 0.002971 

*/
static const float power_analog_curve_coef[3][4] = 
{
//  a0,a1,a2,a3
    {-0.001565, 0.341855, 0.285855, 0.372805},
    {-0.001564, 0.664040, 0.126209, 0.208019},
    {-0.002971, 0.708032, 0.035180, 0.255669},
};
#endif

//static struct mixing_dev s_dev_color;

void color_mixing_init(uint8_t mode)
{
	#ifdef PR_307_L3
	struct mixing_cfg cfg = {
        .led_num = 5,
        .dev_name = "five_color",
        .max_power = (mode == 0) ? (uint16_t)((float)led_base_data.max_total_current * g_power_limit.power_calibration[(light_drive_mode_enum)g_rs485_data.high_speed.mode]) : (uint16_t)((float)analog_led_base_data.max_total_current * g_power_limit.power_calibration[(light_drive_mode_enum)g_rs485_data.high_speed.mode]),
    };
	if(mode == 0)
	{
		if(led_base_data.ver == 0x11)
		{
			uint16_t up_limit_data[5] = {8950,8900,8930,8950,8950};
			led_cfg_init(&cfg.led[0], "Red", led_base_data.data[0]->max_current, power_curve_coef[0], led_up_limit_base_data_info(0,up_limit_data[0]), led_base_data.data[0]->data);
			led_cfg_init(&cfg.led[1], "Gree", led_base_data.data[1]->max_current, power_curve_coef[1], led_up_limit_base_data_info(1,up_limit_data[1]), led_base_data.data[1]->data);
			led_cfg_init(&cfg.led[2], "Blue", led_base_data.data[2]->max_current, power_curve_coef[2], led_up_limit_base_data_info(2,up_limit_data[2]), led_base_data.data[2]->data);
			led_cfg_init(&cfg.led[3], "Warm_White", led_base_data.data[3]->max_current, power_curve_coef[3], led_up_limit_base_data_info(3,up_limit_data[3]), led_base_data.data[3]->data);
			led_cfg_init(&cfg.led[4], "Cool_White", led_base_data.data[4]->max_current, power_curve_coef[4], led_up_limit_base_data_info(4,up_limit_data[4]), led_base_data.data[4]->data);
		}
		else
		{
			led_cfg_init(&cfg.led[0], "Red", led_base_data.data[0]->max_current, power_curve_coef1[0], led_base_data.data[0]->length, led_base_data.data[0]->data);
			led_cfg_init(&cfg.led[1], "Gree", led_base_data.data[1]->max_current, power_curve_coef1[1], led_base_data.data[1]->length, led_base_data.data[1]->data);
			led_cfg_init(&cfg.led[2], "Blue", led_base_data.data[2]->max_current, power_curve_coef1[2], led_base_data.data[2]->length, led_base_data.data[2]->data);
			led_cfg_init(&cfg.led[3], "Warm_White", led_base_data.data[3]->max_current, power_curve_coef1[3], led_base_data.data[3]->length, led_base_data.data[3]->data);
			led_cfg_init(&cfg.led[4], "Cool_White", led_base_data.data[4]->max_current, power_curve_coef1[4], led_base_data.data[4]->length, led_base_data.data[4]->data);
		}
		mixing_dev_init(&fivecolor_mixing_dev_t, &cfg);
		set_constant_max_lux(&fivecolor_mixing_dev_t, fivecolor_mixing_dev_t.led_arg[3].max_lux);
		set_min_lux(&fivecolor_mixing_dev_t, 0.0f);
	}
	else{
		led_cfg_init(&cfg.led[0], "Red", analog_led_base_data.data[0]->max_current, power_analog_curve_coef[0], analog_led_base_data.data[0]->length, analog_led_base_data.data[0]->data);
		led_cfg_init(&cfg.led[1], "Gree", analog_led_base_data.data[1]->max_current, power_analog_curve_coef[1], analog_led_base_data.data[1]->length, analog_led_base_data.data[1]->data);
		led_cfg_init(&cfg.led[2], "Blue", analog_led_base_data.data[2]->max_current, power_analog_curve_coef[2], analog_led_base_data.data[2]->length, analog_led_base_data.data[2]->data);
		led_cfg_init(&cfg.led[3], "Warm_White", analog_led_base_data.data[3]->max_current, power_analog_curve_coef[3], analog_led_base_data.data[3]->length, analog_led_base_data.data[3]->data);
		led_cfg_init(&cfg.led[4], "Cool_White", analog_led_base_data.data[4]->max_current, power_analog_curve_coef[4], analog_led_base_data.data[4]->length, analog_led_base_data.data[4]->data);
		mixing_dev_init(&fivecolor_mixing_dev_t, &cfg);
		set_constant_max_lux(&fivecolor_mixing_dev_t, fivecolor_mixing_dev_t.led_arg[3].max_lux);
		set_min_lux(&fivecolor_mixing_dev_t, 13650.0f);
	}
	#endif
	
	#ifdef PR_308_L2
	
		struct mixing_cfg cfg = {
        .led_num = 3,
        .dev_name = "three_color",
        .max_power = (mode == 0) ? (uint16_t)((float)led_base_data.max_total_current * g_power_limit.power_calibration[(light_drive_mode_enum)g_rs485_data.high_speed.mode]) : (uint16_t)((float)analog_led_base_data.max_total_current * g_power_limit.power_calibration[(light_drive_mode_enum)g_rs485_data.high_speed.mode]),
    };	
	if(mode == 0)
	{
		    // led config
		if(*(uint8_t*)(BASE_DATA_OFFSET + 17) == 0x11)
		{
			uint16_t up_limit_data[3] = {8870,8850,8950};
			led_cfg_init(&cfg.led[0], "Warm_Color", led_base_data.data[0]->max_current, power_curve_coef[0], led_up_limit_base_data_info(0,up_limit_data[0]), led_base_data.data[0]->data);
			led_cfg_init(&cfg.led[1], "Green_Color", led_base_data.data[1]->max_current, power_curve_coef[1], led_up_limit_base_data_info(1,up_limit_data[1]), led_base_data.data[1]->data);
			led_cfg_init(&cfg.led[2], "Cool_Color", led_base_data.data[2]->max_current, power_curve_coef[2], led_up_limit_base_data_info(2,up_limit_data[2]), led_base_data.data[2]->data);
		}
		else
		{
			led_cfg_init(&cfg.led[0], "Warm_Color", led_base_data.data[0]->max_current, power_curve_coef1[0], led_base_data.data[0]->length, led_base_data.data[0]->data);
			led_cfg_init(&cfg.led[1], "Green_Color", led_base_data.data[1]->max_current, power_curve_coef1[1], led_base_data.data[1]->length, led_base_data.data[1]->data);
			led_cfg_init(&cfg.led[2], "Cool_Color", led_base_data.data[2]->max_current, power_curve_coef1[2], led_base_data.data[2]->length, led_base_data.data[2]->data);
		}
		mixing_dev_init(&threecolor_mixing_dev_t, &cfg);
		set_constant_max_lux(&threecolor_mixing_dev_t, threecolor_mixing_dev_t.led_arg[0].max_lux);
		set_min_lux(&threecolor_mixing_dev_t, 0.0f);
	}
	else
	{
		led_cfg_init(&cfg.led[0], "Warm_Color", analog_led_base_data.data[0]->max_current, power_analog_curve_coef[0], analog_led_base_data.data[0]->length, analog_led_base_data.data[0]->data);
		led_cfg_init(&cfg.led[1], "Green_Color", analog_led_base_data.data[1]->max_current, power_analog_curve_coef[1], analog_led_base_data.data[1]->length, analog_led_base_data.data[1]->data);
		led_cfg_init(&cfg.led[2], "Cool_Color", analog_led_base_data.data[2]->max_current, power_analog_curve_coef[2], analog_led_base_data.data[2]->length, analog_led_base_data.data[2]->data);
		mixing_dev_init(&threecolor_mixing_dev_t, &cfg);
		set_constant_max_lux(&threecolor_mixing_dev_t, threecolor_mixing_dev_t.led_arg[0].max_lux);
		set_min_lux(&threecolor_mixing_dev_t, 48489.8f);
	}
//	mixing_cfg_t.dev_name = "three_color";
//	if(mode == 0)
//	{
//		set_min_lux(&threecolor_mixing_dev_t, threecolor_mixing_dev_t.constant_max_lux * 0.0001f);
//	}
//	else
//	{
//		set_min_lux(&threecolor_mixing_dev_t, threecolor_mixing_dev_t.constant_max_lux * 0.0001f);
//	}
	#endif
	fan_mode_set_min_lightness();
}

/**
  * @brief  CCT输出计算
  * @param  lightness 照度等级，范围<0.0-1.0>，0.5为50%，1为100%
  * @param  cct 色温，范围<1600-10000>，单位k
  * @param  duv Duv，需将GM转为Duv
  * @param  ilumination 光输出模式，详见enum ilumination_mode
  * @param  curve 调光曲线，详见enum mixing_curve
  * @param  power_limit 功率限制，范围<0.0-1.0>，1.0表示无限制，0.1表示限制为10%
  * @param  pwm 计算结果PWM，用于直接设置定时器
  * @param  lux 计算结果lux，需调用lux_to_pwm函数得到输出
  * @retval none
  */

#ifndef Reflector_Compensation
#ifdef PR_307_L3
const uint16_t Without_Reflector_Data[] = {
            1892,        1970,        2040,        2092,        2181,        2262,        2317,        2370,        2425,        2477,
        2529,        2578,        2630,        2681,        2730,        2784,        2832,        2886,        2940,        2991,        3042,        3092,        3144,        3203,        3255,
        3307,        3359,        3407,        3460,        3517,        3565,        3615,        3675,        3727,        3779,        3837,        3893,        3947,        3998,        4057,
        4116,        4173,        4237,        4294,        4351,        4408,        4469,        4531,        4593,        4649,        4707,        4761,        4822,        4881,        4937,
        4993,        5054,        5111,        5171,        5231,        5286,        5347,        5405,        5467,        5525,        5577,        5638,        5701,        5760,        5819,
        5873,        5941,        5999,        6057,        6120,        6171,        6235,        6276,        6332,        6377,        6432,        6485,        6541,        6606,        6657,
        6716,        6772,        6854,        6871,        6949,        7010,        7066,        7127,        7183,        7244,        7303,        7349,        7404,        7463,        7524,
        7587,        7652,        7737,        7777,        7819,        7893,        7947,        8007,        8068,        8121,        8185,        8247,        8308,        8358,        8417,
        8479,        8534,        8594,        8654,        8714,        8773,        8832,        8885,        8945,        9002,        9061,        9113,        9170,        9231,        9303,
        9370,        9454,        9513,        9557,        9611,        9671,        9733,        9801,        9877,        9880,        9885,        9974,        10039,        10124,        10200,
        10267,

};

const uint16_t Without_Reflector_Data_Four_Zone[] = {
		2010,	2076,	2119,	2172,	2221,	2270,	2321,	2371,	2420,	2475,	2515,	2550,	2594,	2675,	2715,	2769,	2824,	2867,	2917,	2963,	3015,	3064,	3111,	3163,
		3205,	3230,	3257,	3388,	3413,	3473,	3525,	3582,	3632,	3673,	3721,	3778,	3832,	3890,	3948,	4000,	4054,	4110,	4165,	4219,	4274,	4329,	4385,	4441,
		4495,	4560,	4618,	4677,	4740,	4795,	4852,	4915,	4971,	5030,	5095,	5154,	5211,	5271,	5330,	5382,	5441,	5509,	5570,	5626,	5678,	5724,	5781,	5849,
		5909,	5975,	6005,	6049,	6056,	6102,	6159,	6211,	6258,	6315,	6377,	6436,	6506,	6571,	6642,	6691,	6818,	6829,	6867,	6917,	6969,	7024,	7080,	7132,
		7150,	7235,	7305,	7370,	7436,	7503,	7568,	7644,	7728,	7817,	7821,	7869,	7914,	7945,	7993,	8053,	8109,	8172,	8232,	8293,	8339,	8397,	8458,	8521,
		8580,	8646,	8700,	8747,	8785,	8826,	8871,	8908,	8952,	8995,	9038,	9076,	9118,	9159,	9212,	9240,	9258,	9277,	9335,	9500,	9624,	9691,	9761,	9833,
		9886,	9944,	10020
};
const float Without_Reflector_DUV_Data[] = {0.0022,0.0022,0.0022,0.0025,0.0030,0.0033,0.0040,0.0044,0.0044,0.0046,0.0050,//4000
                                               0.0050,0.0051,0.0051,0.0051,0.0051,0.0051,0.0051,0.0051,0.0053,0.0052,0.0050,//6200
                                               0.0048,0.0045,0.0045,0.0042,0.0038,0.0035,0.0035,0.0035,0.0035,0.0035,0.0035,//8400
                                               0.0034,0.0032,0.0032,0.0032,0.0032,0.0030,0.0030,0.0030};//2000~10000,CCT200K步进
const float Without_Reflector_DUV_Data_Four_Zone[] = {0.0012,0.0012,0.0011,0.0015,0.0021,0.0022,0.0025,0.0028,0.0030,0.0031,0.0035,//4000
                                               0.0035,0.0035,0.0035,0.0035,0.0035,0.0035,0.0035,0.0029,0.0027,0.0027,0.0027,//6200
                                               0.0027,0.0027,0.0027,0.0025,0.0023,0.0022,0.0022,0.0022,0.0022,0.0022,0.0022,//8400
                                               0.0021,0.0019,0.0019,0.0018,0.0018,0.0017,0.0017,0.0017};//2000~10000,CCT200K步进
#endif
#ifdef PR_308_L2
const uint16_t Without_Reflector_Data[] = {
	    1983,        2028,        2078,        2121,        2202,        2262,        2317,        2370,        2425,        2477,
        2529,        2578,        2630,        2681,        2730,        2784,        2832,        2886,        2940,        2991,        3042,        3092,        3144,        3203,        3255,
        3307,        3359,        3407,        3460,        3517,        3565,        3615,        3675,        3727,        3779,        3837,        3893,        3947,        3998,        4057,
        4116,        4173,        4237,        4294,        4351,        4408,        4469,        4531,        4593,        4649,        4707,        4761,        4822,        4881,        4937,
        4993,        5054,        5111,        5171,        5231,        5286,        5347,        5405,        5467,        5525,        5577,        5638,        5701,        5760,        5819,
        5873,        5941,        5999,        6057,        6120,        6171,        6235,        6276,        6332,        6377,        6432,        6485,        6541,        6606,        6657,
        6716,        6772,        6854,        6871,        6949,        7010,        7066,        7127,        7183,        7244,        7303,        7349,        7404,        7463,        7524,
        7587,        7652,        7737,        7729,        7819,        7893,        7947,        8007,        8068,        8121,        8185,        8247,        8308,        8358,        8417,
        8479,        8534,        8594,        8654,        8714,        8773,        8832,        8885,        8945,        9002,        9061,        9113,        9170,        9231,        9303,
        9370,        9454,        9513,        9557,        9611,        9671,        9733,        9801,        9877,        9837,        9885,        9974,        10039,        10124,        10200,
        10267,

};


const float Without_Reflector_DUV_Data[] = {0.0018,0.0020,0.0022,0.0025,0.0030,0.0035,0.0040,0.0044,0.0046,0.0048,0.0050,
                                               0.0050,0.0051,0.0051,0.0051,0.0051,0.0051,0.0051,0.0051,0.0053,0.0052,0.0052,
                                               0.0051,0.0049,0.0047,0.0046,0.0044,0.0044,0.0042,0.0042,0.0040,0.0040,0.0040,
                                               0.0040,0.0040,0.0038,0.0037,0.0035,0.0035,0.0034,0.0033};//2000~10000,CCT200K步进
#endif
float map(float value, float input_min, float input_max, float output_min, float output_max) 
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


void color_cct_calc(float lightness, uint16_t cct, float duv, uint8_t ilumination, uint8_t curve, float power_limit, struct mixing_pwm *pwm, struct mixing_lux *lux)
{
	#ifdef PR_307_L3
	uint16_t Position = 0;
	float input_min = 0;
	float input_max = 0;
	float output_min = 0;
	float output_max = 0;
	if(Electronic_Buf.Bayonet_Status == 0)//是否没有电子附件3E
	{
		if(led_base_data.ver == 0x13)
		{
			if((cct-MIN_CCT)%200)
			{
				if(Without_Reflector_DUV_Data_Four_Zone[((cct-MIN_CCT)/200) + 1] >= Without_Reflector_DUV_Data_Four_Zone[((cct-MIN_CCT)/200)])
				{
					duv += (Without_Reflector_DUV_Data_Four_Zone[((cct-MIN_CCT)/200)] + (Without_Reflector_DUV_Data_Four_Zone[((cct-MIN_CCT)/200) + 1] - Without_Reflector_DUV_Data_Four_Zone[((cct-MIN_CCT)/200)])*((cct-MIN_CCT)%200)/200);
				}
				else
				{
					duv += (Without_Reflector_DUV_Data_Four_Zone[((cct-MIN_CCT)/200)] + (Without_Reflector_DUV_Data_Four_Zone[((cct-MIN_CCT)/200)] - Without_Reflector_DUV_Data_Four_Zone[((cct-MIN_CCT)/200) + 1])*((cct-MIN_CCT)%200)/200);
				}
				duv -= 0.0012f;
			}
			else
			{
				duv += Without_Reflector_DUV_Data_Four_Zone[((cct-MIN_CCT)/200)];
				duv -= 0.0012f;
			}
			if(cct <= MAX_CCT)
			{
				Position = binaryInsertPosition(Without_Reflector_Data_Four_Zone, sizeof(Without_Reflector_Data_Four_Zone)/sizeof(Without_Reflector_Data_Four_Zone[0]), cct);
				if(Position != 0)
				{
					input_min = Without_Reflector_Data_Four_Zone[Position - 1];
					input_max = Without_Reflector_Data_Four_Zone[Position];
					output_min = MIN_CCT + (Position - 1) * 50;
					output_max = MIN_CCT + (Position) * 50;
					cct = (uint16_t)(map((float)cct, input_min, input_max, output_min, output_max));
				}
				else
				{
					cct = MIN_CCT;
				}
			}
		}
		else
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
			if(cct <= MAX_CCT)
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
		}
		
//		else
//		{
//			cct = WITHOUT_REFLECTOR_MAX_CCT;
//		}
	}

#endif
	#ifdef PR_307_L3
	if((light_drive_mode_enum)g_rs485_data.high_speed.mode == 0)
	{
		set_min_lux(&fivecolor_mixing_dev_t, 550);
	}
	else
	{
		set_min_lux(&fivecolor_mixing_dev_t, 13650.0f);
	}
	mixing_dev_cfg(&fivecolor_mixing_dev_t, curve, ilumination, power_limit);
    cct_to_lux(&fivecolor_mixing_dev_t, lightness, cct, duv, ilumination, lux);
    minxing_lux_to_mixing_pwm(&fivecolor_mixing_dev_t, lux, pwm);
	#endif
	
	#ifdef PR_308_L2
	if(Electronic_Buf.Bayonet_Status == 0)//是否没有电子附件3E
	{
		if((*(uint8_t*)(BASE_DATA_OFFSET + 17) == 0x11) || (*(uint8_t*)(BASE_DATA_OFFSET + 17) == 0x12))
		{
			if(cct > 3000 && cct <= 3600)
			{
				duv += 0.0006f;
			}
			else if(cct > 3600)
			{
				duv += 0.0008f;
			}
		}
		else if(*(uint8_t*)(BASE_DATA_OFFSET + 17) == 0x13)
		{
			if(cct < 3000)
			{
				duv -= 0.0023f;
			}
			else if(cct < 3400)
			{
				duv -= 0.0018f;
			}
			else if(cct < 3800)
			{
				duv -= 0.0015f;
			}
			else if(cct <= 6500)
			{
				duv -= 0.0008f;
			}
		}
        else if(*(uint8_t*)(BASE_DATA_OFFSET + 17) == 0x14)
		{
			if(cct < 3000)
			{
				duv -= 0.0016f;
			}
			else if(cct < 3400)
			{
				duv -= 0.0008f;
			}
			else if(cct < 3800)
			{
				duv -= 0.0005f;
			}
			else if(cct <= 6500)
			{
				duv += 0.0004f;
			}
		}
		if(cct >= 4500 && cct < 4900)
		{
			cct -= 60;
		}
		else if(cct >= 4900 && cct <= 6000)
		{
			cct -= 120;
		}
		else if(cct > 6000)
		{
			cct -= 150;
		}
		
	}
	if((light_drive_mode_enum)g_rs485_data.high_speed.mode == 0)
	{
		set_min_lux(&threecolor_mixing_dev_t, 550.0f);
	}
	else
	{
		set_min_lux(&fivecolor_mixing_dev_t, 550.0f);
	}
	mixing_dev_cfg(&threecolor_mixing_dev_t, curve, ilumination, power_limit);
    cct_to_lux(&threecolor_mixing_dev_t, lightness, cct, duv, ilumination, lux);
    minxing_lux_to_mixing_pwm(&threecolor_mixing_dev_t, lux, pwm);
	#endif
}

/**
  * @brief  HSi输出计算
  * @param  lightness 照度等级，范围<0.0-1.0>，0.5为50%，1为100%
  * @param  hue 色环，范围<0.0-360.0>
  * @param  sat 饱和度，范围<0.0-100.0>
  * @param  cct 中心色温，范围<1600-10000>，单位k
  * @param  curve 调光曲线，详见enum mixing_curve
  * @param  power_limit 功率限制，范围<0.0-1.0>，1.0表示无限制，0.1表示限制为10%
  * @param  pwm 计算结果PWM，用于直接设置定时器
  * @param  lux 计算结果lux，需调用lux_to_pwm函数得到输出
  * @retval none
  */
void color_hsi_calc(float lightness, float hue, float sat, uint16_t cct, float duv, uint8_t curve, float power_limit, struct mixing_pwm *pwm, struct mixing_lux *lux)
{
	#ifdef PR_307_L3
	if((light_drive_mode_enum)g_rs485_data.high_speed.mode == 0)
	{
		set_min_lux(&fivecolor_mixing_dev_t, 550.0f);
	}
	else
	{
		set_min_lux(&fivecolor_mixing_dev_t, 2050.0f);
	}
    mixing_dev_cfg(&fivecolor_mixing_dev_t, curve, ILL_MAX, power_limit);
    hsi_to_lux(&fivecolor_mixing_dev_t, lightness, hue, sat, cct, duv, lux);
	analog_mode_lux_less_zero_process(lux);
    minxing_lux_to_mixing_pwm(&fivecolor_mixing_dev_t, lux, pwm);
	#endif
}
/**
  * @brief  色坐标输出计算
  * @param  lightness 照度等级，范围<0.0-1.0>，0.5为50%，1为100%
  * @param  coord_x 色坐标x，范围<0.0-1.0>
  * @param  coord_y 色坐标y，范围<0.0-1.0>
  * @param  curve 调光曲线，详见enum mixing_curve
  * @param  power_limit 功率限制，范围<0.0-1.0>，1.0表示无限制，0.1表示限制为10%
  * @param  pwm 计算结果PWM，用于直接设置定时器
  * @param  lux 计算结果lux，需调用lux_to_pwm函数得到输出
  * @retval none
  */
void color_coordinate_calc(float lightness, float coord_x, float coord_y, uint8_t curve, float power_limit, struct mixing_pwm *pwm, struct mixing_lux *lux)
{
	#ifdef PR_307_L3
	if((light_drive_mode_enum)g_rs485_data.high_speed.mode == 0)
	{
		set_min_lux(&fivecolor_mixing_dev_t, 550.0f);
	}
	else
	{
		set_min_lux(&fivecolor_mixing_dev_t, 2050.0f);
	}
    mixing_dev_cfg(&fivecolor_mixing_dev_t, curve, ILL_MAX, power_limit);
    coordinate_to_lux(&fivecolor_mixing_dev_t, lightness, coord_x, coord_y, lux);
	analog_mode_lux_less_zero_process(lux);
    minxing_lux_to_mixing_pwm(&fivecolor_mixing_dev_t, lux, pwm);
	#endif
	#ifdef PR_308_L2
    mixing_dev_cfg(&threecolor_mixing_dev_t, curve, ILL_MAX, power_limit);
    coordinate_to_lux(&threecolor_mixing_dev_t, lightness, coord_x, coord_y, lux);
    minxing_lux_to_mixing_pwm(&threecolor_mixing_dev_t, lux, pwm);
	#endif
}

/**
  * @brief  gel输出计算
  * @param  lightness 照度等级，范围<0.0-1.0>，0.5为50%，1为100%
  * @param  brand 品牌，0x00：LEE; 0x01：Resco
  * @param  type 类型
  * @param  cct 色温，范围<1600-10000>，单位k
  * @param  color
  * @param  curve 调光曲线，详见enum mixing_curve
  * @param  power_limit 功率限制，范围<0.0-1.0>，1.0表示无限制，0.1表示限制为10%
  * @param  pwm 计算结果PWM，用于直接设置定时器
  * @param  lux 计算结果lux，需调用lux_to_pwm函数得到输出
  * @retval none
  */
void color_gel_calc(float lightness, uint8_t brand, uint8_t type, uint16_t cct, uint16_t color, uint8_t curve, float power_limit, struct mixing_pwm *pwm, struct mixing_lux *lux)
{
	#ifdef PR_307_L3
	if((light_drive_mode_enum)g_rs485_data.high_speed.mode == 0)
	{
		set_min_lux(&fivecolor_mixing_dev_t, 550.0f);
	}
	else
	{
		set_min_lux(&fivecolor_mixing_dev_t, 2050.0f);
	}
    mixing_dev_cfg(&fivecolor_mixing_dev_t, curve, ILL_MAX, power_limit);
    gel_to_lux(&fivecolor_mixing_dev_t, lightness, brand, type, cct, color, lux);
	analog_mode_lux_less_zero_process(lux);
    minxing_lux_to_mixing_pwm(&fivecolor_mixing_dev_t, lux, pwm);
	#endif
	#ifdef PR_308_L2
    mixing_dev_cfg(&threecolor_mixing_dev_t, curve, ILL_MAX, power_limit);
    gel_to_lux(&threecolor_mixing_dev_t, lightness, brand, type, cct, color, lux); 
    minxing_lux_to_mixing_pwm(&threecolor_mixing_dev_t, lux, pwm);
	#endif
}

/**
  * @brief  rgb输出计算
  * @param  dev 合光设备
  * @param  lightness 照度等级，范围<0.0-1.0>，0.5为50%，1为100%
  * @param  r r值，范围<0.0-1.0>
  * @param  g g值，范围<0.0-1.0>
  * @param  b b值，范围<0.0-1.0>
  * @param  curve 调光曲线，详见enum mixing_curve
  * @param  power_limit 功率限制，范围<0.0-1.0>，1.0表示无限制，0.1表示限制为10%
  * @param  pwm 计算结果PWM，用于直接设置定时器
  * @param  lux 计算结果lux，需调用lux_to_pwm函数得到输出
  * @retval none
  */
void color_rgb_calc(float lightness, float r, float g, float b, uint8_t curve, float power_limit, struct mixing_pwm *pwm, struct mixing_lux *lux)
{
	#ifdef PR_307_L3
	if((light_drive_mode_enum)g_rs485_data.high_speed.mode == 0)
	{
		set_min_lux(&fivecolor_mixing_dev_t, 550.0f);
	}
	else
	{
		set_min_lux(&fivecolor_mixing_dev_t, 2050.0f);
	}
    mixing_dev_cfg(&fivecolor_mixing_dev_t, curve, ILL_MAX, power_limit);
    rgb_to_lux(&fivecolor_mixing_dev_t, lightness, r, g, b, lux);
	analog_mode_lux_less_zero_process(lux);
    minxing_lux_to_mixing_pwm(&fivecolor_mixing_dev_t, lux, pwm);
	#endif
	#ifdef PR_308_L2
    mixing_dev_cfg(&threecolor_mixing_dev_t, curve, ILL_MAX, power_limit);
    rgb_to_lux(&threecolor_mixing_dev_t, lightness, r, g, b, lux);
    minxing_lux_to_mixing_pwm(&threecolor_mixing_dev_t, lux, pwm);
	#endif
}

/**
  * @brief  source输出计算
  * @param  lightness 照度等级，范围<0.0-1.0>，0.5为50%，1为100%
  * @param  type 类型,详见enum source_type
  * @param  coord_x source色坐标x，范围<0.0-1.0>
  * @param  coord_y source色坐标y，范围<0.0-1.0>
  * @param  mode 色坐标模式，范围<0-1>，0：根据type查找色坐标，1：使用传参coord_x、coord_y
  * @param  curve 调光曲线，详见enum mixing_curve
  * @param  power_limit 功率限制，范围<0.0-1.0>，1.0表示无限制，0.1表示限制为10%
  * @param  pwm 计算结果PWM，用于直接设置定时器
  * @param  lux 计算结果lux，需调用lux_to_pwm函数得到输出
  * @retval none
  */
void color_source_calc(float lightness, uint8_t type, float coord_x, float coord_y, uint8_t mode, uint8_t curve, float power_limit, struct mixing_pwm *pwm, struct mixing_lux *lux)
{
	#ifdef PR_307_L3
	if((light_drive_mode_enum)g_rs485_data.high_speed.mode == 0)
	{
		set_min_lux(&fivecolor_mixing_dev_t, 550.0f);
	}
	else
	{
		set_min_lux(&fivecolor_mixing_dev_t, 2050.0f);
	}
    mixing_dev_cfg(&fivecolor_mixing_dev_t, curve, ILL_MAX, power_limit);
    source_to_lux(&fivecolor_mixing_dev_t, lightness, type, coord_x, coord_y, mode, lux);
	analog_mode_lux_less_zero_process(lux);
    minxing_lux_to_mixing_pwm(&fivecolor_mixing_dev_t, lux, pwm);
	#endif
	#ifdef PR_308_L2
    mixing_dev_cfg(&threecolor_mixing_dev_t, curve, ILL_MAX, power_limit);
    source_to_lux(&threecolor_mixing_dev_t, lightness, type, coord_x, coord_y, mode, lux);
    minxing_lux_to_mixing_pwm(&threecolor_mixing_dev_t, lux, pwm);
	#endif
}

/**
  * @brief  颜色混合输出计算
  * @param  lightness 照度等级，范围<0.0-1.0>，0.5为50%，1为100%
  * @param  mixing 颜色参数，详见struct color_mixing
  * @param  curve 调光曲线，详见enum mixing_curve
  * @param  power_limit 功率限制，范围<0.0-1.0>，1.0表示无限制，0.1表示限制为10%
  * @param  pwm 计算结果PWM，用于直接设置定时器
  * @param  lux 计算结果lux，需调用lux_to_pwm函数得到输出
  * @retval none
  */
void color_mixing_calc(struct color_mixing *mixing, uint8_t curve, float power_limit, struct mixing_pwm *pwm, struct mixing_lux *lux)
{
	#ifdef PR_307_L3
	if((light_drive_mode_enum)g_rs485_data.high_speed.mode == 0)
	{
		set_min_lux(&fivecolor_mixing_dev_t, 550.0f);
	}
	else
	{
		set_min_lux(&fivecolor_mixing_dev_t, 2050.0f);
	}
    mixing_dev_cfg(&fivecolor_mixing_dev_t, curve, ILL_MAX, power_limit);
    mixing_to_lux(&fivecolor_mixing_dev_t, mixing, lux);
	analog_mode_lux_less_zero_process(lux);
    minxing_lux_to_mixing_pwm(&fivecolor_mixing_dev_t, lux, pwm);
	#endif
	#ifdef PR_308_L2
    mixing_dev_cfg(&threecolor_mixing_dev_t, curve, ILL_MAX, power_limit);
    mixing_to_lux(&threecolor_mixing_dev_t, mixing, lux);
    minxing_lux_to_mixing_pwm(&threecolor_mixing_dev_t, lux, pwm);
	#endif
}

/**
  * @brief  rgbww输出计算
  * @param  r r值，范围<0.0-1.0>
  * @param  g g值，范围<0.0-1.0>
  * @param  b b值，范围<0.0-1.0>
  * @param  ww ww值，范围<0.0-1.0>
  * @param  cw cw值，范围<0.0-1.0>
  * @param  curve 调光曲线，详见enum mixing_curve
  * @param  power_limit 功率限制，范围<0.0-1.0>，1.0表示无限制，0.1表示限制为10%
  * @param  pwm 计算结果PWM，用于直接设置定时器
  * @param  lux 计算结果lux，需调用lux_to_pwm函数得到输出
  * @retval none
  */
void color_rgbww_calc(float r, float g, float b, float ww, float cw, uint8_t curve, float power_limit, struct mixing_pwm *pwm, struct mixing_lux *lux)
{
	#ifdef PR_307_L3
	if((light_drive_mode_enum)g_rs485_data.high_speed.mode == 0)
	{
		set_min_lux(&fivecolor_mixing_dev_t, 550.0f);
	}
	else
	{
		set_min_lux(&fivecolor_mixing_dev_t, 2050.0f);
	}
    mixing_dev_cfg(&fivecolor_mixing_dev_t, curve, ILL_MAX, power_limit);
    rgbww_to_lux(&fivecolor_mixing_dev_t, r, g, b, ww, cw, lux);
	analog_mode_lux_less_zero_process(lux);
    minxing_lux_to_mixing_pwm(&fivecolor_mixing_dev_t, lux, pwm);
	#endif
	#ifdef PR_308_L2
    mixing_dev_cfg(&threecolor_mixing_dev_t, curve, ILL_MAX, power_limit);
    rgbww_to_lux(&threecolor_mixing_dev_t, r, g, b, ww, cw, lux);
    minxing_lux_to_mixing_pwm(&threecolor_mixing_dev_t, lux, pwm);
	#endif
}

/**
  * @brief  获取亮度为0时的最大pwm值
  * @retval PWM数组，数组长度为5
  */
uint16_t *color_get_zerolux_maxpwm(void)
{
	#ifdef PR_307_L3
    return get_zerolux_maxpwm(&fivecolor_mixing_dev_t);
	#endif
	#ifdef PR_308_L2
	return get_zerolux_maxpwm(&threecolor_mixing_dev_t);
	#endif
    
}

/**
  * @brief  设置最大照度
  * @param  lux 照度
  * @retval none
  */
void color_set_constant_lux(float lux)
{
	#ifdef PR_307_L3
    set_constant_max_lux(&fivecolor_mixing_dev_t, lux);
	#endif
	#ifdef PR_308_L2
	set_constant_max_lux(&threecolor_mixing_dev_t, lux);
	#endif
    
}

/**
  * @brief  设置最小亮度值
  * @param  lightness 亮度值
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

/**
  * @brief  照度转pwm输出
  * @param  lux 照度数据
  * @param  pwm 输出数据  
  * @retval none
  */
void color_lux_to_pwm(const struct mixing_lux *lux, struct mixing_pwm *pwm)
{
	#ifdef PR_307_L3
    minxing_lux_to_mixing_pwm(&fivecolor_mixing_dev_t, lux, pwm);
	#endif
	#ifdef PR_308_L2
	minxing_lux_to_mixing_pwm(&threecolor_mixing_dev_t, lux, pwm);
	#endif
}

float gm_to_duv(uint8_t gm)
{
#ifdef PR_307_L3
        float gm_temp = 0.0f;
        
        gm_temp = (int8_t)gm < 100 ? (int8_t)gm : 100;
        
		return ((gm_temp) * 0.0002f);
	
#endif
	
#ifdef PR_308_L2
		float gm_temp = 0.0f;
		
		gm_temp = (int8_t)gm < 100 ? (int8_t)gm : 100;
        
        return ((gm_temp) * 0.00005f);
#endif
}

void set_max_power(float power_radio)
{
#ifdef PR_307_L3
	if(g_rs485_data.high_speed.mode == 0)//pwm模式
	{
		fivecolor_mixing_dev_t.max_power = (uint16_t)((float)led_base_data.max_total_current * g_power_limit.power_calibration[(light_drive_mode_enum)g_rs485_data.high_speed.mode]);
	}
	else
	{
		fivecolor_mixing_dev_t.max_power = (uint16_t)((float)analog_led_base_data.max_total_current * g_power_limit.power_calibration[(light_drive_mode_enum)g_rs485_data.high_speed.mode]);
	}
#endif
#ifdef PR_308_L2
	if(g_rs485_data.high_speed.mode == 0)//pwm模式
	{
		threecolor_mixing_dev_t.max_power = (uint16_t)((float)led_base_data.max_total_current * g_power_limit.power_calibration[(light_drive_mode_enum)g_rs485_data.high_speed.mode]);
	}
	else
	{
		threecolor_mixing_dev_t.max_power = (uint16_t)((float)analog_led_base_data.max_total_current * g_power_limit.power_calibration[(light_drive_mode_enum)g_rs485_data.high_speed.mode]);
	}
#endif
}


void fan_mode_set_min_lightness(void)
{
#ifdef PR_307_L3
//	if(g_rs485_data.high_speed.mode == 0)//pwm
//	{
//		if(g_rs485_data.fan.mode == 4)//静音模式
//		{
//			set_min_lux(&fivecolor_mixing_dev_t, 400.0f);
////			color_set_min_lightness(0.002f);
//		}
//		else
//		{
//			set_min_lux(&fivecolor_mixing_dev_t, 300.0f);
////			color_set_min_lightness(0.002f);
//		}
//	}
//	else
//	{
//		if(g_rs485_data.fan.mode == 4)//静音模式
//		{
//			color_set_min_lightness(0.070f);
//		}
//		else
//		{
//			color_set_min_lightness(0.080f);
//		}
//	}
#endif
#ifdef PR_308_L2
//	if(g_rs485_data.high_speed.mode == 0)//pwm
//	{
//		if(g_rs485_data.fan.mode == 4)//静音模式
//		{
//			set_min_lux(&threecolor_mixing_dev_t, 550.0f);
////			color_set_min_lightness(0.0027f);
//		}
//		else
//		{
//			set_min_lux(&threecolor_mixing_dev_t, 400.0f);
////			color_set_min_lightness(0.002f);
//		}
//	}
//	else
//	{
//		if(g_rs485_data.fan.mode == 4)//静音模式
//		{
//			color_set_min_lightness(0.115f);
//		}
//		else
//		{
//			color_set_min_lightness(0.20f);
//		}
//	}
#endif
}
void analog_mode_lux_less_zero_process(struct mixing_lux *lux)
{
	if((light_drive_mode_enum)g_rs485_data.high_speed.mode != 0)
	{
		for(uint8_t i = 0;i < 5;i++)
		{
			if(lux->lux[i] < 0.05f)
			{
				lux->lux[i] = 0;
			}
		}
	}
}
