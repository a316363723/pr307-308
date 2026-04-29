#include "dmx_profile_analysis.h"
#include "dmx_profile_configuration.h"
#include <stddef.h>
#include "dmx_profile_configuration.h"
#include "project_config.h"

const uint16_t s_source_coord[][2] = {
{4260, 4005,},
{4433, 4077,},
{4355, 4059,},
{5091, 4225,},
{5487, 4139,},
{4891, 4198,},
{5193, 4133,},
{6584, 3128,},
{3735, 4197,},
{4695, 4263,},
{4156, 4090,},
{3963, 3978,},
{3100, 3339,},
{3716, 3860,},
{3854, 3753,},
{3484, 3691,},
{4509, 4258,},
{1962, 1115,},
{3341, 3415,},
{5242, 4285,},
{5410, 4238,},
{3702, 3875,},
{3860, 3951,},
{4097, 3930,},
{3350, 3427,},
{3242, 3359,},
{5210, 4158,},
{5000, 4058,},
{3361, 3505,},
{3198, 3338,},
{2886, 3069,},
{3002, 3166,},
{3325, 3428,},
{1871, 4820,},
{2115, 2482,},
{6329, 3289,},
{5656, 4040,},
{2639, 5563,},
{5097, 4477,},
{6396, 3287,},
{1493, 827,},
{2263, 6655,},
{6920, 3002,},
{4028, 5347,},
{3932, 1886,},
{2753, 1345,},   
};

#if (DMX_FX_PIXEL_CFG == PRODUCT_MT_PRO)
static const uint8_t table_1[] = {2, 3, 4};
static const uint8_t table_2[] = {2, 3, 4, 6, 9};
static const uint8_t table_3[] = {2, 3, 6};
static const uint8_t table_4[] = {2, 3, 4};
#elif (DMX_FX_PIXEL_CFG == PRODUCT_APUTURE_S4C)
static const uint8_t table_1[] = {2, 3, 4};
static const uint8_t table_2[] = {2, 3, 4, 6, 8, 12, 16, 24, 32};
static const uint8_t table_3[] = {2, 3, 4, 6, 8, 12, 16};
static const uint8_t table_4[] = {2, 3, 4, 6, 8, 12};
#elif (DMX_FX_PIXEL_CFG == PRODUCT_APUTURE_S1C)
static const uint8_t table_1[] = {2, 3, 4};
static const uint8_t table_2[] = {2, 3, 4, 6, 8};
static const uint8_t table_3[] = {2, 3, 6};
static const uint8_t table_4[] = {2, 3, 4};
#elif (DMX_FX_PIXEL_CFG == PRODUCT_APUTURE_S2C)
static const uint8_t table_1[] = {2, 3, 4};
static const uint8_t table_2[] = {2, 3, 4, 6, 8, 12, 16};
static const uint8_t table_3[] = {2, 3, 6, 8};
static const uint8_t table_4[] = {2, 3, 6};
#endif

static uint16_t endian_Swap_16bit(const uint8_t* dmx_data);

/********************扩展模块********************************/
enum bit_mode_type
{
    DMX_8BIT_TYPE,
    DMX_16BIT_TYPE,
};

struct dmx_config_data  dmx_sys_config;
struct dmx_config_data  dmx_sys_config_pre;
static uint16_t config_refresh_event = 0;

static uint16_t convert_dmx_to_ctr_dy_gap(uint16_t dmx_val, 
                                    uint16_t dmx_min_val,
                                    uint16_t dmx_max_val, 
                                    uint16_t variate_min_val,
                                    uint16_t variate_max_val, 
                                    uint16_t variate_change_unit_val)
{
    uint16_t variate_curr_val = 0;
    
    if(dmx_val > dmx_max_val)
        return variate_curr_val;
	if(dmx_val <= dmx_min_val)
		return variate_min_val;
	
    dmx_val = (dmx_val < dmx_max_val && dmx_val > 0) ? (dmx_val + 1) : dmx_val;//解决DMX控制台必须在最大值时才能使变量达到最大值
    variate_curr_val = variate_min_val + (variate_max_val - variate_min_val + 1) * (dmx_val - dmx_min_val) / (dmx_max_val - dmx_min_val);
    variate_curr_val = variate_curr_val / variate_change_unit_val * variate_change_unit_val;//取整,以最小单位值为基准
    if(variate_curr_val > variate_max_val)
        variate_curr_val = variate_max_val;
    return variate_curr_val;
}

static uint16_t convert_dmx_to_ctr_dy_gap_2(uint16_t dmx_val, 
                                    uint16_t dmx_max_val, 
                                    uint16_t variate_mid_val,
                                    uint16_t range, 
                                    uint16_t variate_change_unit_val)
{
    uint16_t variate_curr_val = 0;

    if(dmx_val > dmx_max_val * 2)
        return variate_curr_val;    
    if(dmx_val > dmx_max_val - 1)
    {
        variate_curr_val = variate_mid_val - (range - (range + 1) * (255 - dmx_val) / dmx_max_val);
    }
    else
    {
        variate_curr_val = variate_mid_val + (range + 1) * dmx_val / dmx_max_val;
    }
    variate_curr_val = variate_curr_val / variate_change_unit_val * variate_change_unit_val;//取整,以最小单位值为基准
}

uint16_t YOKE_PAN_MIN = 0 ;
uint16_t YOKE_PAN_MAX  ;

uint16_t YOKE_TILT_MIN = 0;
uint16_t YOKE_TILT_MAX = 0;

uint16_t YOKE_FRESNEL_ANGLE_MIN = 0;
uint16_t YOKE_FRESNEL_ANGLE_MAX = 0;

static uint8_t sELECTRIC_CONTROL_SWITCH = 0;
static uint8_t sFUNTION_CONTROL_SWITCH = 0;

void dmx_yoke_tilt_range_set(uint16_t max, uint16_t min)
{	
	YOKE_TILT_MAX = max;
	YOKE_TILT_MIN = min;
}

void dmx_yoke_pan_range_set(uint16_t max, uint16_t min)
{
	YOKE_PAN_MAX = max;	
	YOKE_PAN_MIN = min;
}

void dmx_yoke_fpesnel_angle_range_set(uint16_t max, uint16_t min)
{
	YOKE_FRESNEL_ANGLE_MAX = max;
	YOKE_FRESNEL_ANGLE_MIN = min;
}

void dmx_electric_control_switch_set(uint8_t state)
{
	sELECTRIC_CONTROL_SWITCH = state;
}

void dmx_funtion_control_switch_set(uint8_t state)
{
	sFUNTION_CONTROL_SWITCH = state;
}

	
void dmx_sys_config_event_clear(uint16_t val)
{    
    config_refresh_event &= ~(val);
}

uint8_t dmx_get_fx_extern_len(uint8_t type, uint8_t mode)
{
	uint8_t len = 0;
	uint8_t head_size = 0;
	
	switch(type)
    {
        case 0:
			if(mode == 0)
			{
				len = sizeof(fx_rgb_8bit_club_lights_arg_t)+3;
			}
			else
			{
				len = sizeof(fx_rgb_16bit_club_lights_arg_t)+4;
			}
            break;
        case 1:
			if(mode == 0)
			{
				len = sizeof(fx_rgb_8bit_paparazzi_arg_t)+3;
			}
			else
			{
				len = sizeof(fx_rgb_16bit_paparazzi_arg_t)+4;
			}
            break;
        case 2:
			if(mode == 0)
			{
				len = sizeof(fx_rgb_8bit_lightning_arg_t)+3;
			}
			else
			{
				len = sizeof(fx_rgb_16bit_lightning_arg_t)+4;
			}
            break;
        case 3:
			if(mode == 0)
			{
				len = sizeof(fx_rgb_8bit_tv_arg_t)+3;
			}
			else
			{
				len = sizeof(fx_rgb_16bit_tv_arg_t)+4;
			}
            break;
        case 4:
			if(mode == 0)
			{
				len = sizeof(fx_rgb_8bit_candle_arg_t)+3;
			}
			else
			{
				len = sizeof(fx_rgb_16bit_candle_arg_t)+4;
			}
            break;
        case 5:
			if(mode == 0)
			{
				len = sizeof(fx_rgb_8bit_fire_arg_t)+3;
			}
			else
			{
				len = sizeof(fx_rgb_16bit_fire_arg_t)+4;
			}
            break;
        case 6:
			if(mode == 0)
			{
				len = sizeof(fx_rgb_8bit_strobe_arg_t)+3;
			}
			else
			{
				len = sizeof(fx_rgb_16bit_strobe_arg_t)+4;
			}
            break;
        case 7:
			if(mode == 0)
			{
				len = sizeof(fx_rgb_8bit_explosion_arg_t)+3;
			}
			else
			{
				len = sizeof(fx_rgb_16bit_explosion_arg_t)+4;
			}
            break;
        case 8:
			if(mode == 0)
			{
				len = sizeof(fx_rgb_8bit_fault_bulb_arg_t)+3;
			}
			else
			{
				len = sizeof(fx_rgb_16bit_fault_bulb_arg_t)+4;
			}
            break;
        case 9:
			if(mode == 0)
			{
				len = sizeof(fx_rgb_8bit_pulsing_arg_t)+3;
			}
			else
			{
				len = sizeof(fx_rgb_16bit_pulsing_arg_t)+4;
			}
            break;
        case 10:
			if(mode == 0)
			{
				len = sizeof(fx_rgb_8bit_welding_arg_t)+3;
			}
			else
			{
				len = sizeof(fx_rgb_16bit_welding_arg_t)+4;
			}
            break;
        case 11:
			if(mode == 0)
			{
				len = sizeof(fx_rgb_8bit_cop_car_arg_t)+3;
			}
			else
			{
				len = sizeof(fx_rgb_16bit_cop_car_arg_t)+4;
			}
            break;
        case 12:
			if(mode == 0)
			{
				len = sizeof(fx_rgb_8bit_color_chase_arg_t)+3;
			}
			else
			{
				len = sizeof(fx_rgb_16bit_color_chase_arg_t)+4;
			}
            break;
        case 13:
			if(mode == 0)
			{
				len = sizeof(fx_rgb_8bit_party_lights_arg_t)+3;
			}
			else
			{
				len = sizeof(fx_rgb_16bit_party_lights_arg_t)+4;
			}
            break;
        case 14:
			if(mode == 0)
			{
				len = sizeof(fx_rgb_8bit_fireworks_arg_t)+3;
			}
			else
			{
				len = sizeof(fx_rgb_16bit_fireworks_arg_t)+4;
			}
            break;
        case 15:
			if(mode == 0)
			{
				len = sizeof(fx_rgb_8bit_color_fade_arg_t)+3;
			}
			else
			{
				len = sizeof(fx_rgb_16bit_color_fade_arg_t)+4;
			}
            break;
        case 16:
			if(mode == 0)
			{
				len = sizeof(fx_rgb_8bit_color_cycle_arg_t)+3;
			}
			else
			{
				len = sizeof(fx_rgb_16bit_color_cycle_arg_t)+4;
			}
            break;
        case 17:
			if(mode == 0)
			{
				len = sizeof(fx_rgb_8bit_color_gradient_arg_t)+3;
			}
			else
			{
				len = sizeof(fx_rgb_16bit_color_gradient_arg_t)+4;
			}
            break;
        case 18:
        default:
			if(mode == 0)
			{
				len = sizeof(fx_rgb_8bit_one_color_chase_arg_t)+3;
			}
			else
			{
				len = sizeof(fx_rgb_16bit_one_color_chase_arg_t)+4;
			}
            break;
    }
}

uint16_t dmx_sys_config_refresh_event_get(struct dmx_config_data** config_data)
{
    *config_data = &dmx_sys_config;
    return config_refresh_event;
}

static void zoom_8bit_analysis(uint8_t* profile_data, uint16_t* index)
{
    static uint8_t dmx_data_pre = 0;
	static uint8_t dmx_change_flag = 0;
	
    if(profile_data[(*index)] <= 19)
    {
        dmx_data_pre = 0;
        
        (*index)++;
		if(dmx_change_flag == 1)
		{
			dmx_change_flag = 0;
			dmx_sys_config.zoom = 1800;
			if(dmx_sys_config_pre.zoom != dmx_sys_config.zoom)
			{
				dmx_sys_config_pre.zoom = dmx_sys_config.zoom;
				config_refresh_event |= DMX_ZOOM_REFRESH;
			}
		}
        return;
    }
	dmx_change_flag = 1;
    dmx_data_pre = profile_data[(*index)];
    dmx_sys_config.zoom = convert_dmx_to_ctr_dy_gap(profile_data[(*index)++], 20, 255, YOKE_FRESNEL_ANGLE_MIN, YOKE_FRESNEL_ANGLE_MAX, 1);
	dmx_sys_config.zoom = (uint16_t)dmx_sys_config.zoom * 10;
    if(dmx_sys_config_pre.zoom != dmx_sys_config.zoom)
    {
        dmx_sys_config_pre.zoom = dmx_sys_config.zoom;
        config_refresh_event |= DMX_ZOOM_REFRESH;
    }
}

static void zoom_16bit_analysis(uint8_t* profile_data, uint16_t* index)
{
    static uint8_t dmx_data_pre = 0;
    static uint8_t angle_dec = 0;
	static uint8_t dmx_change_flag1 = 0;
	
    if(profile_data[(*index)] <= 19)
    {
        dmx_data_pre = 0;
        (*index)++;
		angle_dec = convert_dmx_to_ctr_dy_gap(profile_data[(*index)++], 0, 255, 0, 15, 1);
		dmx_sys_config.zoom = angle_dec;
		if(dmx_change_flag1 == 1)
		{
			dmx_change_flag1 = 0;
			dmx_sys_config.zoom = 1800;
			dmx_sys_config_pre.zoom = dmx_sys_config.zoom;
			config_refresh_event |= DMX_ZOOM_REFRESH;
		}
    }
	else
	{
		dmx_change_flag1 = 1;
		dmx_data_pre = profile_data[(*index)];
		dmx_sys_config.zoom = convert_dmx_to_ctr_dy_gap(profile_data[(*index)++], 20, 255, YOKE_FRESNEL_ANGLE_MIN, YOKE_FRESNEL_ANGLE_MAX, 1);
		dmx_sys_config.zoom = (uint16_t)(dmx_sys_config.zoom / 10) * 10;
		angle_dec = convert_dmx_to_ctr_dy_gap(profile_data[(*index)++], 0, 255, 0, 15, 1);
		dmx_sys_config.zoom *= 10;
		dmx_sys_config.zoom += angle_dec;
		if(dmx_sys_config.zoom >= 5000)
			dmx_sys_config.zoom = 5000;
		if(dmx_sys_config_pre.zoom != dmx_sys_config.zoom)
		{
			dmx_sys_config_pre.zoom = dmx_sys_config.zoom;
			config_refresh_event |= DMX_ZOOM_REFRESH;
		}
	}
}

static void motor_8bit_analysis(uint8_t* profile_data, uint16_t* index)
{
	uint16_t angle = 0;
	static uint8_t dmx_change_flag2[2] = {0};
	
    if(profile_data[(*index)] <= 19)
    {
        (*index)++;
		if(dmx_change_flag2[0] == 1)
		{
			dmx_change_flag2[0] = 0;
			dmx_sys_config.pan = YOKE_PAN_MIN;
			dmx_sys_config_pre.pan = dmx_sys_config.pan;
			config_refresh_event |= DMX_PAN_REFRESH;
		}
    }
	else
	{
		dmx_change_flag2[0] = 1;
		angle = convert_dmx_to_ctr_dy_gap(profile_data[(*index)++], 20, 255, YOKE_PAN_MIN, YOKE_PAN_MAX, 1);
//		angle = (angle/10)*10;   //去除细调
		dmx_sys_config.pan = angle * 10;
		if(dmx_sys_config_pre.pan != dmx_sys_config.pan)
		{
			dmx_sys_config_pre.pan = dmx_sys_config.pan;
			config_refresh_event |= DMX_PAN_REFRESH;
		}
	}
	if(profile_data[(*index)] <= 19)
	{
		(*index)++;
		if(dmx_change_flag2[1] == 1)
		{
			dmx_change_flag2[1] = 0;
			dmx_sys_config.tilt = YOKE_TILT_MIN;
			dmx_sys_config_pre.tilt = dmx_sys_config.tilt;
			config_refresh_event |= DMX_PAN_TILT_REFRESH;
		}
	}
	else
	{
		dmx_change_flag2[1] = 1;
		angle = convert_dmx_to_ctr_dy_gap(profile_data[(*index)++], 20, 255, YOKE_TILT_MIN, YOKE_TILT_MAX, 1);
//		angle = (angle/10)*10;   //去除细调
		dmx_sys_config.tilt = angle * 10;
		if(dmx_sys_config_pre.tilt != dmx_sys_config.tilt)
		{
			dmx_sys_config_pre.tilt = dmx_sys_config.tilt;
			config_refresh_event |= DMX_PAN_TILT_REFRESH;
		}
	}
}

static void motor_16bit_analysis(uint8_t* profile_data, uint16_t* index)
{
	uint16_t val = 0;
	uint16_t angle_dec;
	uint16_t angle = 0;
	static uint8_t dmx_change_flag3[2] = {0};
	
    if(profile_data[(*index)] <= 19)
    {
		(*index)+=1;
		angle_dec = convert_dmx_to_ctr_dy_gap(profile_data[(*index)++], 0, 255, 0, 30, 1);
		dmx_sys_config.pan = angle_dec;
		if(dmx_sys_config_pre.pan != dmx_sys_config.pan)
		{
			dmx_sys_config_pre.pan = dmx_sys_config.pan;
			config_refresh_event |= DMX_PAN_REFRESH_FINE;
		}
		if(dmx_change_flag3[0] == 1)
		{
			dmx_change_flag3[0] = 0;
			dmx_sys_config.pan = YOKE_PAN_MIN;
			dmx_sys_config_pre.pan = dmx_sys_config.pan;
			config_refresh_event |= DMX_PAN_REFRESH;
		}
    }
	else
	{
		val = profile_data[(*index)++]; 
		dmx_change_flag3[0] = 1;
		angle = convert_dmx_to_ctr_dy_gap(val, 20, 255, YOKE_PAN_MIN, YOKE_PAN_MAX, 1);
//		angle = (angle/10)*10;   //去除细调
		val = profile_data[(*index)++]; 
		angle_dec = convert_dmx_to_ctr_dy_gap(val, 0, 255, 0, 30, 1);
		angle = angle*10 + angle_dec;
		dmx_sys_config.pan = angle;
		if(dmx_sys_config_pre.pan != dmx_sys_config.pan)
		{
			dmx_sys_config_pre.pan = dmx_sys_config.pan;
			config_refresh_event |= DMX_PAN_REFRESH;
		}
	}
    if(profile_data[(*index)] <= 19)
    {
		(*index)+=1;
		angle_dec = convert_dmx_to_ctr_dy_gap(profile_data[(*index)++], 0, 255, 0, 30, 1);
		dmx_sys_config.tilt = angle_dec;
		if(dmx_sys_config_pre.tilt != dmx_sys_config.tilt)
		{
			dmx_sys_config_pre.tilt = dmx_sys_config.tilt;
			config_refresh_event |= DMX_PAN_TILT_REFRESH_FINE;
		}
		if(dmx_change_flag3[1] == 1)
		{
			dmx_change_flag3[1] = 0;
			dmx_sys_config.tilt = YOKE_TILT_MIN;
			dmx_sys_config_pre.tilt = dmx_sys_config.tilt;
			config_refresh_event |= DMX_PAN_TILT_REFRESH;
		}
    }
    else
	{
		val = profile_data[(*index)++]; 
		dmx_change_flag3[1] = 1;
		angle = convert_dmx_to_ctr_dy_gap(val, 20, 255, YOKE_TILT_MIN, YOKE_TILT_MAX, 1);
		val = profile_data[(*index)++];
		angle_dec = convert_dmx_to_ctr_dy_gap(val, 0, 255, 0, 30, 1);
		angle = angle*10 + angle_dec;
		dmx_sys_config.tilt = angle;
		if(dmx_sys_config_pre.tilt != dmx_sys_config.tilt)
		{
			dmx_sys_config_pre.tilt = dmx_sys_config.tilt;
			config_refresh_event |= DMX_PAN_TILT_REFRESH;
		}
	}
//    val = endian_Swap_16bit((uint8_t*)&profile_data[(*index)]); 
//    dmx_sys_config.pan = convert_dmx_to_ctr_dy_gap(val, 20, 255, YOKE_PAN_MIN, YOKE_PAN_MAX, 1);  
//    (*index)+=2;    
//    val = endian_Swap_16bit((uint8_t*)&profile_data[(*index)]); 
//    dmx_sys_config.tilt = convert_dmx_to_ctr_dy_gap(val, 0, 65535, YOKE_TILT_MIN, YOKE_TILT_MAX, 1);
//    (*index)+=2; 
//    
//    if(dmx_sys_config_pre.pan != dmx_sys_config.pan)
//	{
//		dmx_sys_config_pre.pan = dmx_sys_config.pan;
//        config_refresh_event |= DMX_PAN_REFRESH;
//	}
//	if(dmx_sys_config_pre.tilt != dmx_sys_config.tilt)
//    {
//        dmx_sys_config_pre.tilt = dmx_sys_config.tilt;
//        config_refresh_event |= DMX_PAN_TILT_REFRESH;
//    }
}

static void function_modules_8bit_analysis(uint8_t* profile_data, uint16_t* index)
{
    #if (FAN_EXIST)
    dmx_sys_config.fan_mode = 4 * profile_data[(*index)++] / 256 ;//convert_dmx_to_ctr_dy_gap(profile_data[(*index)++], 0, 255, 0, 3, 1);
    if(dmx_sys_config_pre.fan_mode != dmx_sys_config.fan_mode)
    {
        dmx_sys_config_pre.fan_mode = dmx_sys_config.fan_mode;
        config_refresh_event |= DMX_FAN_REFRESH;
    }
    #endif
    #if (DIMMING_CURVE_EXIST)
    dmx_sys_config.dim_curve = 4 * profile_data[(*index)++] / 256 ;//convert_dmx_to_ctr_dy_gap(profile_data[(*index)++], 0, 255, 0, 3, 1);
    if(dmx_sys_config_pre.dim_curve != dmx_sys_config.dim_curve)
    {
        dmx_sys_config_pre.dim_curve = dmx_sys_config.dim_curve;
        config_refresh_event |= DMX_CURVE_REFRESH;
    }
    #endif
    #if (FREQUENCY_SEL_EXIST)
    if(profile_data[(*index)] > 19)
    {
        dmx_sys_config.dim_freq = convert_dmx_to_ctr_dy_gap(profile_data[(*index)++], 19, 255, DIM_FREQ_MIN, DIM_FREQ_MAX, 100);
        if(dmx_sys_config_pre.dim_freq != dmx_sys_config.dim_freq)
        {
            dmx_sys_config_pre.dim_freq = dmx_sys_config.dim_freq;
            config_refresh_event |= DMX_FRQ_REFRESH;
        }
    }
    else
	{
		dmx_sys_config.dim_freq = 0;
        if(dmx_sys_config_pre.dim_freq != dmx_sys_config.dim_freq)
        {
            dmx_sys_config_pre.dim_freq = dmx_sys_config.dim_freq;
            config_refresh_event |= DMX_FRQ_REFRESH;
        }
        (*index)++;
	}
    #endif
}

static uint16_t dmx_model_extend(uint8_t* profile_data, uint16_t index, enum bit_mode_type bit_mode)
{
    //TODO  
	if(sELECTRIC_CONTROL_SWITCH == 1)
	{
		switch(bit_mode)
		{
			case DMX_8BIT_TYPE:
				zoom_8bit_analysis(profile_data, &index);
				motor_8bit_analysis(profile_data, &index);   
			break;
			case DMX_16BIT_TYPE:
				zoom_16bit_analysis(profile_data, &index);
				motor_16bit_analysis(profile_data, &index);  
			break;
			default:break; 
		}   
	}
	
	if(sFUNTION_CONTROL_SWITCH == 1)
	{
		function_modules_8bit_analysis(profile_data, &index);
    }
    return index;
}

//static uint8_t fan_mode_calculate(uint8_t dmx_value)
//{
//    uint8_t mode;

//    if(dmx_value < 10)
//    {
//        mode = DMX_FAN_MODE_SMART;
//    }
//    else if(dmx_value < 58)
//    {
//        mode = DMX_FAN_MODE_SILENT;
//    }
//    else if(dmx_value < 106)
//    {
//        mode = DMX_FAN_MODE_MEDIUM;

//    }
//    else if(dmx_value < 154)
//    {

//        mode = DMX_FAN_MODE_HIGH;
//    }
//    else
//    {
//        mode = DMX_FAN_MODE_MANUAL;
//    }

//    return  mode;

//}

//static uint8_t fan_rpm_calculate(uint8_t dmx_value)
//{
//    uint8_t mode;

//    if(dmx_value < 11)
//    {
//        mode = 0;
//    }
//    else
//    {
//        mode = 1 + (dmx_value - 11) * (100 - 1) / (255 - 11);
//    }

//    return  mode;

//}

//static void fan_info_analysis(dmx_fan_t* fan_info, const uint8_t profile_fan_mode, const uint8_t profile_fan_rpm)
//{
//    if(NULL == fan_info)
//        return;

//    fan_info->fan_mode = fan_mode_calculate(profile_fan_mode);
//    fan_info->fan_rpm = fan_rpm_calculate(profile_fan_rpm);
//}

static uint16_t cct_8bit_calculate(uint8_t dmx_value, uint16_t cct_max_value,  uint16_t cct_min_value)
{
    uint16_t cct_value;

    if(cct_min_value > cct_max_value)
        return 0;

    cct_value = cct_min_value + dmx_value * (cct_max_value - cct_min_value) / 255;

    return  cct_value;

}

static uint16_t cct_8bit_center_calculate(uint8_t dmx_value, uint16_t cct_max_value,  uint16_t cct_min_value)
{
    uint16_t cct_value;

    if(cct_min_value > cct_max_value)
        return 0;
    if(dmx_value < 10)   return 6500;
	
    cct_value = cct_min_value + (dmx_value-9) * (cct_max_value - cct_min_value) / (255-9);

    return  cct_value;

}


static uint16_t fx_cct_8bit_calculate(uint8_t dmx_value, uint16_t cct_max_value,  uint16_t cct_min_value)
{
    uint16_t cct_value;

    if(cct_min_value > cct_max_value)
        return 0;
    if(dmx_value < 10 )
	{
    	return 6500;
	}
    cct_value = cct_min_value + (dmx_value-9) * (cct_max_value - cct_min_value) / (255 - 9);

    return  cct_value;

}

static uint16_t fx_cct_16bit_calculate(uint16_t dmx_value, uint16_t cct_max_value,  uint16_t cct_min_value)
{
    uint16_t cct_value;

    if(cct_min_value > cct_max_value)
        return 0;
    if(dmx_value > 2295 )
    {
        cct_value = cct_min_value + (dmx_value -2295) * (cct_max_value - cct_min_value) / (65535 - 2295);
    }
    else 
    {
        return 6500;
    }

    return  cct_value;

}

static uint16_t cct_16bit_calculate(uint16_t dmx_value, uint16_t cct_max_value,  uint16_t cct_min_value)
{
    uint16_t cct_value;

    if(cct_min_value > cct_max_value)
        return 0;

    cct_value = cct_min_value + dmx_value * (cct_max_value - cct_min_value) / 65535;

    return  cct_value;

}


static uint16_t cct_16bit_center_calculate(uint16_t dmx_value, uint16_t cct_max_value,  uint16_t cct_min_value)
{
    uint16_t cct_value;

    if(cct_min_value > cct_max_value)
        return 0;
	if(dmx_value <  2296) return 6500;

    cct_value = cct_min_value +  (dmx_value -2295) * (cct_max_value - cct_min_value) / (65535 - 2295);

    return  cct_value;

}



static uint16_t dmx_to_convert(uint16_t dmx_val, uint16_t dmx_max_val, uint16_t variate_max_val, uint16_t variate_min_val, uint16_t gap_val)
{
    uint16_t variate_curr_val = 0;

    if(dmx_val > dmx_max_val)
        return variate_curr_val;

    variate_curr_val = variate_min_val + dmx_val / gap_val;

    if(variate_curr_val > variate_max_val)
        variate_curr_val = variate_max_val;

    return variate_curr_val;
}

static uint16_t dmx_to_linear_calculate(uint16_t dmx_val, uint16_t dmx_max_val, uint16_t variate_max_val, uint16_t variate_min_val, uint16_t variate_change_unit_val)
{
    uint16_t variate_curr_val = 0;

    if(dmx_val > dmx_max_val)
        return variate_curr_val;

    variate_curr_val = variate_min_val + ((variate_max_val - variate_min_val) * dmx_val) / dmx_max_val;
    variate_curr_val = (variate_curr_val / variate_change_unit_val) * variate_change_unit_val;

    if(variate_curr_val > variate_max_val)
        variate_curr_val = variate_max_val;

    return variate_curr_val;
}


static uint8_t dmx_pixel_fx_colors_type_get(uint8_t table_type, uint8_t dmx_value)
{
    uint8_t res;
    uint8_t i;

    switch(table_type)
    {
        case 1:
            i = dmx_value / 10 >= sizeof(table_1) ? (sizeof(table_1) - 1) : (dmx_value / 10);
            res = table_1[i];
            break;
        case 2:
            i = dmx_value / 10 >= sizeof(table_2) ? (sizeof(table_2) - 1) : (dmx_value / 10);
            res = table_2[i];
            break;
        case 3:
            i = dmx_value / 10 >= sizeof(table_3) ? (sizeof(table_3) - 1) : (dmx_value / 10);
            res = table_3[i];
            break;
        case 4:
            i = dmx_value / 10 >= sizeof(table_4) ? (sizeof(table_4) - 1) : (dmx_value / 10);
            res = table_4[i];
            break;
    }

    return res;
}

static float duv_8bit_calculate(uint8_t dmx_value)
{
    float duv = 0.0f;

    if(dmx_value < 11)
    {
        duv = 0.0f;
    }
    else if(dmx_value < 21)
    {
        duv = DMX_DUV_MIN;
    }
    else if(dmx_value >= 21 && dmx_value < 120)
    {
        duv = DMX_DUV_MIN + (float)(DMX_DUV_MAX * (dmx_value - 20) / (99)) ;
    }
    else if(dmx_value < 146)
    {
        duv = 0.0f;
    }
    else if(dmx_value >= 146 && dmx_value < 245)
    {
        duv = (float)(DMX_DUV_MAX  * (dmx_value - 145) / (99));
    }
    else
    {
        duv = DMX_DUV_MAX;
    }

    return duv;
}
static float duv_16bit_calculate(uint16_t dmx_value)
{


    float duv = 0.0f;

    if(dmx_value < 4588)
    {
        duv = 0.0f;
    }
    else if(dmx_value < 9831)
    {
        duv = DMX_DUV_MIN;
    }
    else if(dmx_value >= 9831 && dmx_value < 30146)
    {
        duv = DMX_DUV_MIN + (float)(DMX_DUV_MAX * (dmx_value - 9830)  / (30146 - 9831)) ;
    }
    else if(dmx_value >= 30146 &&  dmx_value < 39977)
    {
        duv = 0.0f;
    }
    else if(dmx_value >= 39977 && dmx_value < 60293)
    {

        duv = (float)(DMX_DUV_MAX * (dmx_value - 39976) / (60293 - 39977)) ;
    }
    else
    {
        duv = DMX_DUV_MAX;
    }

    return duv;
}


static int8_t fx_dmx_gm_16bit_calculate( uint16_t dmx_value)
{

	int8_t duv = 0;

    if(dmx_value < 4588)
    {
        duv = 0;
    }
    else if(dmx_value < 9831)
    {
        duv = DMX_FX_GM_MIN;
    }
    else if(dmx_value >= 9831 && dmx_value < 30146)
    {
        duv = DMX_FX_GM_MIN + (DMX_FX_GM_MAX * (dmx_value - 9830) / (30146 - 9831)) ;
    }
    else if(dmx_value >= 30146 &&  dmx_value < 39977)
    {
        duv = 0;
    }
    else if(dmx_value >= 39977 && dmx_value < 60293)
    {

        duv = DMX_FX_GM_MAX * (dmx_value - 39976) / (60293 - 39977) ;
    }
    else
    {
        duv = DMX_FX_GM_MAX;
    }

    return duv;

}

static int8_t fx_dmx_gm_8bit_calculate( uint8_t dmx_val)
{
	 int8_t duv = 0;

    if(dmx_val < 11)
    {
        duv = 0;
    }
    else if(dmx_val < 21)
    {
        duv = DMX_FX_GM_MIN;
    }
    else if(dmx_val >= 21 && dmx_val < 120)
    {
        duv = DMX_FX_GM_MIN + (DMX_FX_GM_MAX * (dmx_val - 20) / 99) ;
    }
    else if(dmx_val < 146)
    {
        duv = 0;
    }
    else if(dmx_val >= 146 && dmx_val < 245)
    {
        duv = (DMX_FX_GM_MAX * (dmx_val - 145) / 99) ;
    }
    else
    {
        duv = DMX_FX_GM_MAX;
    }

    return duv;

}



static uint8_t strobe_frq_calculate_8bit(uint8_t dmx_value)
{
    uint8_t frq = 0;

    if(dmx_value > 19)
    {
        frq = 1 + 24 * (dmx_value - 20) / (255 - 20);
    }

    return frq;
}

static uint8_t gel_series_calculate_8bit(uint8_t dmx_value, uint8_t brand)
{
    const uint8_t series[2][5] = {{0, 1, 2, 3, 3}, {0, 1, 2, 3, 4}};
    brand = brand == 0 ? 0 : 1;
    dmx_value = dmx_value > 49 ? 49 : dmx_value;
    return series[brand][dmx_value / 10];
}

static uint8_t gel_color_calculate_8bit(uint8_t dmx_value, uint8_t brand, uint8_t series)
{
    const uint8_t color_limit[2][5] = {{32, 32, 9, 45, 0}, {38, 88, 8, 17, 40}};
    uint8_t series_limit = brand == 0 ? 3 : 4;
    brand = brand == 0 ? 0 : 1;
    series = series > series_limit ? series_limit : series;
    return dmx_value / 2 > color_limit[brand][series] ? color_limit[brand][series] : dmx_value / 2;
}


static uint8_t fx_frq_calculate_8bit(uint8_t dmx_value)
{
    return (1 + (dmx_value / 20 > 20 ? 20 : dmx_value / 20));
}

static uint16_t endian_Swap_16bit(const uint8_t* dmx_data)
{

    return  dmx_data[0] << 8 | dmx_data[1];
}


uint8_t profile_0101_cct_w_8bit_analysis(const uint8_t* profile_data, dmx_data_t* dmx_data)
{
    if(NULL == profile_data || NULL == dmx_data)
        return 0;

    const profile_0101_cct_w_8bit_t* p_profile = (profile_0101_cct_w_8bit_t*) &profile_data[0];
    dmx_data->type = DMX_TYPE_CCT;
    dmx_data->lightness = p_profile->intensity  * 1000 / 255.0f;
    dmx_data->arg.cct.cct = 6500;
    dmx_data->arg.cct.duv = 0.0f;
    dmx_model_extend((uint8_t*)&profile_data[0], sizeof(profile_0101_cct_w_8bit_t), DMX_8BIT_TYPE);

    return strobe_frq_calculate_8bit(p_profile->strobe);
}

uint8_t profile_0102_cct_w_16bit_analysis(const uint8_t* profile_data, dmx_data_t* dmx_data)
{
    if(NULL == profile_data || NULL == dmx_data)
        return 0;

    const profile_0102_cct_w_16bit_t* p_profile = (profile_0102_cct_w_16bit_t*)&profile_data[0];
    dmx_data->type = DMX_TYPE_CCT;
    dmx_data->lightness = (profile_data[0] << 8 | profile_data[1])  * 1000 / 65535.0f;
    dmx_data->arg.cct.cct = 6500;
    dmx_data->arg.cct.duv = 0.0f;
    dmx_model_extend((uint8_t*)&profile_data[0], sizeof(profile_0102_cct_w_16bit_t), DMX_16BIT_TYPE);
    return strobe_frq_calculate_8bit(p_profile->strobe);

}

uint8_t profile_0103_cct_lite_8bit_analysis(const uint8_t* profile_data, dmx_data_t* dmx_data)
{
    if(NULL == profile_data || NULL == dmx_data)
        return 0;

    const profile_0103_cct_lite_8bit_t* p_profile = (profile_0103_cct_lite_8bit_t*) &profile_data[0];
    dmx_data->type = DMX_TYPE_CCT;
    dmx_data->lightness = *(uint8_t*)profile_data  * 1000 / 255.0f;
    dmx_data->arg.cct.cct = cct_8bit_calculate(p_profile->cct, DMX_CCT_MAX, DMX_CCT_MIN);
    dmx_data->arg.cct.duv = 0.0f;
    dmx_model_extend((uint8_t*)&profile_data[0], sizeof(profile_0103_cct_lite_8bit_t)+1, DMX_8BIT_TYPE);
    return strobe_frq_calculate_8bit(p_profile->strobe);

}


uint8_t profile_0104_cct_lite_16bit_analysis(const uint8_t* profile_data, dmx_data_t* dmx_data)
{
    if(NULL == profile_data || NULL == dmx_data)
        return 0;

    const profile_0104_cct_lite_16bit_t* p_profile = (profile_0104_cct_lite_16bit_t*) &profile_data[0];
    dmx_data->type = DMX_TYPE_CCT;
    dmx_data->lightness = endian_Swap_16bit(profile_data)  * 1000 / 65535.0f;
    dmx_data->arg.cct.cct = cct_16bit_calculate(endian_Swap_16bit(&profile_data[2]), DMX_CCT_MAX, DMX_CCT_MIN);
    dmx_data->arg.cct.duv = 0.0f;
    dmx_model_extend((uint8_t*)&profile_data[0], sizeof(profile_0104_cct_lite_16bit_t), DMX_16BIT_TYPE);
    return strobe_frq_calculate_8bit(p_profile->strobe);


}
uint8_t profile_0105_cct_gm_8bit_analysis(const uint8_t* profile_data, dmx_data_t* dmx_data)
{
    if(NULL == profile_data || NULL == dmx_data)
        return 0;

    const profile_0105_cct_gm_8bit_t* p_profile = (profile_0105_cct_gm_8bit_t*) &profile_data[0];
    dmx_data->type = DMX_TYPE_CCT;
    dmx_data->lightness = *(uint8_t*)profile_data  * 1000 / 255.0f;
    dmx_data->arg.cct.cct = cct_8bit_calculate(p_profile->cct, DMX_CCT_MAX, DMX_CCT_MIN);
    dmx_data->arg.cct.duv = duv_8bit_calculate(p_profile->gm);
    dmx_model_extend((uint8_t*)&profile_data[0], sizeof(profile_0105_cct_gm_8bit_t), DMX_8BIT_TYPE);
    return strobe_frq_calculate_8bit(p_profile->strobe);
}

//static uint16_t int_16bit_calculate(uint16_t dmx_value)
//{
//	static float cct_value;
//	cct_value =  dmx_value / 1000 * 1000 * 1000 / 65535 +  dmx_value % 1000 / 65535.0f ;
//	return  cct_value;

//}

uint8_t profile_0106_cct_gm_16bit_analysis(const uint8_t* profile_data, dmx_data_t* dmx_data)
{
    if(NULL == profile_data || NULL == dmx_data)
        return 0;

    const profile_0106_cct_gm_16bit_t* p_profile = (profile_0106_cct_gm_16bit_t*) &profile_data[0];
    dmx_data->type = DMX_TYPE_CCT;
    dmx_data->lightness = endian_Swap_16bit(profile_data)* 1000 / 65535.0f;
    dmx_data->arg.cct.cct = cct_16bit_calculate(endian_Swap_16bit((uint8_t*)&p_profile->cct), DMX_CCT_MAX, DMX_CCT_MIN);
    dmx_data->arg.cct.duv = duv_16bit_calculate(endian_Swap_16bit((uint8_t*)&p_profile->gm));
    dmx_model_extend((uint8_t*)&profile_data[0], sizeof(profile_0106_cct_gm_16bit_t), DMX_16BIT_TYPE);
    return strobe_frq_calculate_8bit(p_profile->strobe);

}

uint8_t profile_0201_rgb_8bit_analysis(const uint8_t* profile_data, dmx_data_t* dmx_data)
{
    if(NULL == profile_data || NULL == dmx_data)
        return 0;

    const profile_0201_rgb_8bit_t* p_profile = (profile_0201_rgb_8bit_t*) &profile_data[0];
    dmx_data->type = DMX_TYPE_RGB;
    dmx_data->lightness = *(uint8_t*)profile_data  * 1000 / 255.0f;
    dmx_data->arg.rgb.blue = p_profile->blue / 255.0f;
    dmx_data->arg.rgb.green = p_profile->green / 255.0f;
    dmx_data->arg.rgb.red = p_profile->red / 255.0f;
    dmx_model_extend((uint8_t*)&profile_data[0], sizeof(profile_0201_rgb_8bit_t), DMX_8BIT_TYPE);
    return strobe_frq_calculate_8bit(p_profile->strobe);
}

uint8_t profile_0202_rgb_16bit_analysis(const uint8_t* profile_data, dmx_data_t* dmx_data)
{
    if(NULL == profile_data || NULL == dmx_data)
        return 0;

    const profile_0202_rgb_16bit_t* p_profile = (profile_0202_rgb_16bit_t*) &profile_data[0];
    dmx_data->type = DMX_TYPE_RGB;
    dmx_data->lightness = endian_Swap_16bit(profile_data) * 1000  / 65535.0f;
    dmx_data->arg.rgb.blue = endian_Swap_16bit((uint8_t*)&p_profile->blue) / 65535.0f;
    dmx_data->arg.rgb.green = endian_Swap_16bit((uint8_t*)&p_profile->green) / 65535.0f;
    dmx_data->arg.rgb.red = endian_Swap_16bit((uint8_t*)&p_profile->red) / 65535.0f;
    dmx_model_extend((uint8_t*)&profile_data[0], sizeof(profile_0202_rgb_16bit_t), DMX_16BIT_TYPE);
    return strobe_frq_calculate_8bit(p_profile->strobe);
}


uint8_t profile_0203_rgbw_8bit_analysis(const uint8_t* profile_data, dmx_data_t* dmx_data)
{
    if(NULL == profile_data || NULL == dmx_data)
        return 0;

    const profile_0203_rgbw_8bit_t* p_profile = (profile_0203_rgbw_8bit_t*) &profile_data[0];
    dmx_data->type = DMX_TYPE_RGB;
    dmx_data->lightness = *(uint8_t*)profile_data  * 1000 / 255.0f;
    dmx_data->arg.rgb.blue = p_profile->blue / 255.0f;
    dmx_data->arg.rgb.green = p_profile->green / 255.0f;
    dmx_data->arg.rgb.red = p_profile->red / 255.0f;
    dmx_data->arg.rgb.cw = p_profile->white / 255.0f;
    dmx_data->arg.rgb.ww = p_profile->white / 255.0f;
    dmx_model_extend((uint8_t*)&profile_data[0], sizeof(profile_0203_rgbw_8bit_t), DMX_8BIT_TYPE);
    return strobe_frq_calculate_8bit(p_profile->strobe);
}


uint8_t profile_0204_rgbw_16bit_analysis(const uint8_t* profile_data, dmx_data_t* dmx_data)
{
    if(NULL == profile_data || NULL == dmx_data)
        return 0;

    const profile_0204_rgbw_16bit_t* p_profile = (profile_0204_rgbw_16bit_t*) &profile_data[0];
    dmx_data->type = DMX_TYPE_RGB;
    dmx_data->lightness = endian_Swap_16bit(profile_data)  * 1000 / 65535.0f;
    dmx_data->arg.rgb.blue = endian_Swap_16bit((uint8_t*)&p_profile->blue) / 65535.0f;
    dmx_data->arg.rgb.green = endian_Swap_16bit((uint8_t*)&p_profile->green) / 65535.0f;
    dmx_data->arg.rgb.red = endian_Swap_16bit((uint8_t*)&p_profile->red) / 65535.0f;
    dmx_data->arg.rgb.cw = endian_Swap_16bit((uint8_t*)&p_profile->white) / 65535.0f;
    dmx_data->arg.rgb.ww = endian_Swap_16bit((uint8_t*)&p_profile->white) / 65535.0f;
    dmx_model_extend((uint8_t*)&profile_data[0], sizeof(profile_0204_rgbw_16bit_t), DMX_16BIT_TYPE);
    return strobe_frq_calculate_8bit(p_profile->strobe);
}

uint8_t profile_0205_rgbww_8bit_analysis(const uint8_t* profile_data, dmx_data_t* dmx_data)
{
    if(NULL == profile_data || NULL == dmx_data)
        return 0;

    const profile_0205_rgbww_8bit_t* p_profile = (profile_0205_rgbww_8bit_t*) &profile_data[0];
    dmx_data->type = DMX_TYPE_RGBWW;
    dmx_data->lightness = *(uint8_t*)profile_data  * 1000 / 255.0f;
    dmx_data->arg.rgb.blue = p_profile->blue / 255.0f;;
    dmx_data->arg.rgb.green = p_profile->green / 255.0f;;
    dmx_data->arg.rgb.red = p_profile->red/ 255.0f; ;
    dmx_data->arg.rgb.cw = p_profile->cold_white / 255.0f;;
    dmx_data->arg.rgb.ww = p_profile->warm_white / 255.0f;;
    dmx_model_extend((uint8_t*)&profile_data[0], sizeof(profile_0205_rgbww_8bit_t), DMX_8BIT_TYPE);
    return strobe_frq_calculate_8bit(p_profile->strobe);
}


uint8_t profile_0206_rgbww_16bit_analysis(const uint8_t* profile_data, dmx_data_t* dmx_data)
{
    if(NULL == profile_data || NULL == dmx_data)
        return 0;

    const profile_0206_rgbww_16bit_t* p_profile = (profile_0206_rgbww_16bit_t*) &profile_data[0];
    dmx_data->type = DMX_TYPE_RGBWW;
    dmx_data->lightness = endian_Swap_16bit(profile_data) * 1000  / 65535.0f;
    dmx_data->arg.rgb.blue = endian_Swap_16bit((uint8_t*)&p_profile->blue)/ 65535.0f ;
    dmx_data->arg.rgb.green = endian_Swap_16bit((uint8_t*)&p_profile->green)/ 65535.0f ;
    dmx_data->arg.rgb.red = endian_Swap_16bit((uint8_t*)&p_profile->red)/ 65535.0f ;
    dmx_data->arg.rgb.cw = endian_Swap_16bit((uint8_t*)&p_profile->cold_white) / 65535.0f;
    dmx_data->arg.rgb.ww = endian_Swap_16bit((uint8_t*)&p_profile->warm_white)/ 65535.0f ;
    dmx_model_extend((uint8_t*)&profile_data[0], sizeof(profile_0206_rgbww_16bit_t), DMX_16BIT_TYPE);
    return strobe_frq_calculate_8bit(p_profile->strobe);
}


uint8_t profile_0207_rgbacl_8bit_analysis(const uint8_t* profile_data, dmx_data_t* dmx_data)
{
    if(NULL == profile_data || NULL == dmx_data)
        return 0;

    const profile_0207_rgbacl_8bit_t* p_profile = (profile_0207_rgbacl_8bit_t*) &profile_data[0];
    dmx_data->type = DMX_TYPE_RGB;
    dmx_data->lightness = *(uint8_t*)profile_data * 1000  / 255.0f;
    dmx_data->arg.rgb.blue = p_profile->blue / 255.0f;
    dmx_data->arg.rgb.green = p_profile->green / 255.0f;
    dmx_data->arg.rgb.red = p_profile->red / 255.0f;
    dmx_data->arg.rgb.lime = p_profile->lime / 255.0f;
    dmx_data->arg.rgb.amber = p_profile->amber / 255.0f;
    dmx_data->arg.rgb.cyan = p_profile->cyan / 255.0f;
    dmx_model_extend((uint8_t*)&profile_data[0], sizeof(profile_0207_rgbacl_8bit_t), DMX_8BIT_TYPE);
    return strobe_frq_calculate_8bit(p_profile->strobe);
}


uint8_t profile_0208_rgbacl_16bit_analysis(const uint8_t* profile_data, dmx_data_t* dmx_data)
{
    if(NULL == profile_data || NULL == dmx_data)
        return 0;

    const profile_0208_rgbacl_16bit_t* p_profile = (profile_0208_rgbacl_16bit_t*)&profile_data[0];
    dmx_data->type = DMX_TYPE_RGB;
    dmx_data->lightness = endian_Swap_16bit(profile_data)  * 1000 / 65535.0f;
    dmx_data->arg.rgb.blue = endian_Swap_16bit((uint8_t*)&p_profile->blue) / 65535.0f;
    dmx_data->arg.rgb.green = endian_Swap_16bit((uint8_t*)&p_profile->green) / 65535.0f;
    dmx_data->arg.rgb.red = endian_Swap_16bit((uint8_t*)&p_profile->red) / 65535.0f;
    dmx_data->arg.rgb.amber = endian_Swap_16bit((uint8_t*)&p_profile->amber)  / 65535.0f;
    dmx_data->arg.rgb.lime = endian_Swap_16bit((uint8_t*)&p_profile->lime) / 65535.0f;
    dmx_data->arg.rgb.cyan = endian_Swap_16bit((uint8_t*)&p_profile->cyan) / 65535.0f;
    dmx_model_extend((uint8_t*)&profile_data[0], sizeof(profile_0208_rgbacl_16bit_t), DMX_16BIT_TYPE);
    return strobe_frq_calculate_8bit(p_profile->strobe);
}

uint8_t profile_0301_hsi_8bit_analysis(const uint8_t* profile_data, dmx_data_t* dmx_data)
{
    if(NULL == profile_data || NULL == dmx_data)
        return 0;

    const profile_0301_hsi_8bit_t* p_profile = (profile_0301_hsi_8bit_t*)&profile_data[0];
    dmx_data->type = DMX_TYPE_HSI;
    dmx_data->lightness = *(uint8_t*)profile_data  * 1000 / 255.0f;
    dmx_data->arg.hsi.hue = 1 + (359 * p_profile->hue / 255.0f);
    dmx_data->arg.hsi.sat = p_profile->sat * 100.0f / 255.0f;
    dmx_model_extend((uint8_t*)&profile_data[0], sizeof(profile_0301_hsi_8bit_t), DMX_8BIT_TYPE);
    return strobe_frq_calculate_8bit(p_profile->strobe);
}

uint8_t profile_0302_hsi_16bit_analysis(const uint8_t* profile_data, dmx_data_t* dmx_data)
{
    if(NULL == profile_data || NULL == dmx_data)
        return 0;

    const profile_0302_hsi_16bit_t* p_profile = (profile_0302_hsi_16bit_t*) &profile_data[0];
    dmx_data->type = DMX_TYPE_HSI;
    dmx_data->lightness = endian_Swap_16bit(profile_data) * 1000  / 65535.0f;
    dmx_data->arg.hsi.hue = 1 + (359 * endian_Swap_16bit((uint8_t*)&p_profile->hue) / 65535.0f);
    dmx_data->arg.hsi.sat = endian_Swap_16bit((uint8_t*)&p_profile->sat) * 100.0f / 65535.0f;
    dmx_model_extend((uint8_t*)&profile_data[0], sizeof(profile_0302_hsi_16bit_t), DMX_16BIT_TYPE);
    return strobe_frq_calculate_8bit(p_profile->strobe);
}


uint8_t profile_0303_advhsi_8bit_analysis(const uint8_t* profile_data, dmx_data_t* dmx_data)
{
    if(NULL == profile_data || NULL == dmx_data)
        return 0;

    const profile_0303_advhsi_8bit_t* p_profile = (profile_0303_advhsi_8bit_t*)&profile_data[0];
    dmx_data->type = DMX_TYPE_HSI;
    dmx_data->lightness = *(uint8_t*)profile_data  * 1000 / 255.0f;
    dmx_data->arg.hsi.hue = 1 + (359 * p_profile->hue / 255.0f);
    dmx_data->arg.hsi.sat = p_profile->sat * 100.0f / 255.0f;
    dmx_data->arg.hsi.cct = cct_8bit_center_calculate(p_profile->cct, DMX_CCT_MAX, DMX_CCT_MIN);
    dmx_model_extend((uint8_t*)&profile_data[0], sizeof(profile_0303_advhsi_8bit_t), DMX_8BIT_TYPE);
    return strobe_frq_calculate_8bit(p_profile->strobe);
}


uint8_t profile_0304_advhsi_16bit_analysis(const uint8_t* profile_data, dmx_data_t* dmx_data)
{
    if(NULL == profile_data || NULL == dmx_data)
        return 0;

    const profile_0304_advhsi_16bit_t* p_profile = (profile_0304_advhsi_16bit_t*) &profile_data[0];
    dmx_data->type = DMX_TYPE_HSI;
    dmx_data->lightness = endian_Swap_16bit(profile_data)  * 1000 / 65535.0f;
    dmx_data->arg.hsi.hue = 1 + (359 * endian_Swap_16bit((uint8_t*)&p_profile->hue) / 65535.0f);
    dmx_data->arg.hsi.sat = endian_Swap_16bit((uint8_t*)&p_profile->sat) * 100.f / 65535.0f;
    dmx_data->arg.hsi.cct = cct_16bit_center_calculate(endian_Swap_16bit((uint8_t*)&p_profile->cct), DMX_CCT_MAX, DMX_CCT_MIN);
    dmx_model_extend((uint8_t*)&profile_data[0], sizeof(profile_0304_advhsi_16bit_t), DMX_16BIT_TYPE);
    return strobe_frq_calculate_8bit(p_profile->strobe);
}

uint8_t profile_0401_xy_8bit_analysis(const uint8_t* profile_data, dmx_data_t* dmx_data)
{
    if(NULL == profile_data || NULL == dmx_data)
        return 0;

    const profile_0401_xy_8bit_t* p_profile = (profile_0401_xy_8bit_t*) &profile_data[0];
    dmx_data->type = DMX_TYPE_XY;
    dmx_data->lightness = *(uint8_t*)profile_data  * 1000 / 255.0f;
    dmx_data->arg.xy.x = 0.8f * p_profile->x / 255.0f;
    dmx_data->arg.xy.y = 0.8f * p_profile->y / 255.0f;
    dmx_model_extend((uint8_t*)&profile_data[0], sizeof(profile_0401_xy_8bit_t), DMX_8BIT_TYPE);
    return strobe_frq_calculate_8bit(p_profile->strobe);
}

uint8_t profile_0402_xy_16bit_analysis(const uint8_t* profile_data, dmx_data_t* dmx_data)
{
    if(NULL == profile_data || NULL == dmx_data)
        return 0;

    const profile_0402_xy_16bit_t* p_profile = (profile_0402_xy_16bit_t*) &profile_data[0];
    dmx_data->type = DMX_TYPE_XY;
    dmx_data->lightness = endian_Swap_16bit(profile_data) * 1000  / 65535.0f;
    dmx_data->arg.xy.x = 0.8f * endian_Swap_16bit((uint8_t*)&p_profile->x) / 65535.0f;
    dmx_data->arg.xy.y = 0.8f * endian_Swap_16bit((uint8_t*)&p_profile->y) / 65535.0f;
    dmx_model_extend((uint8_t*)&profile_data[0], sizeof(profile_0402_xy_16bit_t), DMX_16BIT_TYPE);
    return strobe_frq_calculate_8bit(p_profile->strobe);
}


uint8_t profile_0501_gel_8bit_analysis(const uint8_t* profile_data, dmx_data_t* dmx_data)
{
    if(NULL == profile_data || NULL == dmx_data)
        return 0;

    const profile_0501_gel_8bit_t* p_profile = (profile_0501_gel_8bit_t*)&profile_data[0];
    dmx_data->type = DMX_TYPE_GEL;
    dmx_data->lightness = *(uint8_t*)profile_data  * 1000 / 255.0f;
    dmx_data->arg.gel.cct = p_profile->cct < 128 ? 0 : 1;
    dmx_data->arg.gel.brand = p_profile->brand > 29 ? 1 : 0;
    dmx_data->arg.gel.type = gel_series_calculate_8bit(p_profile->type, dmx_data->arg.gel.brand);
    dmx_data->arg.gel.color = gel_color_calculate_8bit(p_profile->color, dmx_data->arg.gel.brand,  dmx_data->arg.gel.type);
    dmx_model_extend((uint8_t*)&profile_data[0], sizeof(profile_0501_gel_8bit_t), DMX_8BIT_TYPE);
    return strobe_frq_calculate_8bit(p_profile->strobe);
}


uint8_t profile_0502_gel_16bit_analysis(const uint8_t* profile_data, dmx_data_t* dmx_data)
{
    if(NULL == profile_data || NULL == dmx_data)
        return 0;

    const profile_0502_gel_16bit_t* p_profile = (profile_0502_gel_16bit_t*) &profile_data[0];
    dmx_data->type = DMX_TYPE_GEL;
    dmx_data->lightness = endian_Swap_16bit(profile_data)  * 1000 / 65535.0f;
    dmx_data->arg.gel.cct = p_profile->cct < 128 ? 0 : 1;
    dmx_data->arg.gel.brand = p_profile->brand > 29 ? 1 : 0;
    dmx_data->arg.gel.type = gel_series_calculate_8bit(p_profile->type, dmx_data->arg.gel.brand);
    dmx_data->arg.gel.color = gel_color_calculate_8bit(p_profile->color, dmx_data->arg.gel.brand,  dmx_data->arg.gel.type);
    dmx_model_extend((uint8_t*)&profile_data[0], sizeof(profile_0502_gel_16bit_t), DMX_16BIT_TYPE);
    return strobe_frq_calculate_8bit(p_profile->strobe);
	
}

uint8_t profile_0601_source_8bit_analysis(const uint8_t* profile_data, dmx_data_t* dmx_data)
{
    if(NULL == profile_data || NULL == dmx_data)
        return 0;

    const profile_0601_source_8bit_t* p_profile = (profile_0601_source_8bit_t*)&profile_data[0];
    dmx_data->type = DMX_TYPE_SOUCE;
    dmx_data->lightness = *(uint8_t*)profile_data  * 1000 / 255.0f;
    dmx_data->arg.souce.type = (uint8_t)dmx_to_convert(*(uint8_t*)&p_profile->type, 255, 45, 0, 3);
    dmx_data->arg.souce.y = convert_dmx_to_ctr_dy_gap_2(p_profile->y, 128, s_source_coord[dmx_data->arg.souce.type][1],  50, 1);  
    dmx_data->arg.souce.x = convert_dmx_to_ctr_dy_gap_2(p_profile->x, 128, s_source_coord[dmx_data->arg.souce.type][0],  50, 1); 

    dmx_model_extend((uint8_t*)&profile_data[0], sizeof(profile_0601_source_8bit_t), DMX_8BIT_TYPE);
	
    return strobe_frq_calculate_8bit(p_profile->strobe);
}

uint8_t profile_0602_source_16bit_analysis(const uint8_t* profile_data, dmx_data_t* dmx_data)
{
    if(NULL == profile_data || NULL == dmx_data)
        return 0;

    const profile_0602_source_16bit_t* p_profile = (profile_0602_source_16bit_t*) &profile_data[0];
    dmx_data->type = DMX_TYPE_SOUCE;
    dmx_data->lightness = endian_Swap_16bit(profile_data)  * 1000 / 65535.0f;
    dmx_data->arg.souce.type = dmx_to_convert(p_profile->type, 255, 45, 0, 3);
    dmx_data->arg.souce.y = convert_dmx_to_ctr_dy_gap_2(p_profile->y, 128, s_source_coord[dmx_data->arg.souce.type][1],  50, 1);
    dmx_data->arg.souce.x = convert_dmx_to_ctr_dy_gap_2(p_profile->x, 128, s_source_coord[dmx_data->arg.souce.type][0],  50, 1); 

    dmx_model_extend((uint8_t*)&profile_data[0], sizeof(profile_0602_source_16bit_t), DMX_16BIT_TYPE);

    return strobe_frq_calculate_8bit(p_profile->strobe);
}


uint8_t profile_0701_fx_w_8bit_analysis(const uint8_t* profile_data, dmx_data_t* dmx_data)
{
    if(NULL == profile_data || NULL == dmx_data)
        return 0;

    const profile_0701_fx_w_8bit_t* p_profile = (profile_0701_fx_w_8bit_t*)&profile_data[0];
    uint8_t fx_state;
    dmx_data->lightness = (*(uint8_t*)profile_data * 1000  / 255 );
    dmx_data->type = DMX_TYPE_FX;

    switch(p_profile->type / 10)
    {
        case 0:
            dmx_data->arg.fx.type = DMX_FX_PAPARAZZI;
            dmx_data->arg.fx.arg.paparazzi.frq = fx_frq_calculate_8bit(p_profile->frq);
            fx_state = p_profile->state < 70 ? DMX_FX_Trigger_Continue : DMX_FX_Trigger_None;
            break;
        case 1:
            dmx_data->arg.fx.type = DMX_FX_FIREWORKS;
            dmx_data->arg.fx.arg.fireworks.frq = fx_frq_calculate_8bit(p_profile->frq);
            fx_state = p_profile->state < 70 ? DMX_FX_Trigger_Continue : DMX_FX_Trigger_None;
            break;
        case 2:
            dmx_data->arg.fx.type = DMX_FX_FAULT_BULB;
            dmx_data->arg.fx.arg.fault_bulb.frq = fx_frq_calculate_8bit(p_profile->frq);
            fx_state = p_profile->state < 70 ? DMX_FX_Trigger_Continue : DMX_FX_Trigger_None;
            break;
        case 3:
            dmx_data->arg.fx.type = DMX_FX_LIGHTNING;
            dmx_data->arg.fx.arg.lightning.frq = fx_frq_calculate_8bit(p_profile->frq);
            fx_state = p_profile->state > 69 ? DMX_FX_Trigger_None : ((p_profile->state > 9) ? DMX_FX_Trigger_Once : DMX_FX_Trigger_Continue);
            break;
        case 4:
            dmx_data->arg.fx.type = DMX_FX_TV;
            dmx_data->arg.fx.arg.tv.frq = fx_frq_calculate_8bit(p_profile->frq);
            fx_state = p_profile->state < 70 ? DMX_FX_Trigger_Continue : DMX_FX_Trigger_None;
            break;
        case 5:
            dmx_data->arg.fx.type = DMX_FX_PULSING;
            dmx_data->arg.fx.arg.pulsing.frq = fx_frq_calculate_8bit(p_profile->frq);
            fx_state = p_profile->state < 70 ? DMX_FX_Trigger_Continue : DMX_FX_Trigger_None;
            break;
        case 6:
            dmx_data->arg.fx.type = DMX_FX_STROBE;
            dmx_data->arg.fx.arg.strobe.spd = fx_frq_calculate_8bit(p_profile->frq);
            fx_state = p_profile->state < 70 ? DMX_FX_Trigger_Continue : DMX_FX_Trigger_None;
            break;
        case 7:
        default:
            dmx_data->arg.fx.type = DMX_FX_EXPLOSION;
            dmx_data->arg.fx.arg.explosion.decay = fx_frq_calculate_8bit(p_profile->frq);
            fx_state = p_profile->state < 70 ? DMX_FX_Trigger_Once : DMX_FX_Trigger_None;
            break;
    }

    dmx_model_extend((uint8_t*)&profile_data[0], sizeof(profile_0701_fx_w_8bit_t), DMX_8BIT_TYPE);

    return fx_state;

}


uint8_t profile_0702_fx_w_16bit_analysis(const uint8_t* profile_data, dmx_data_t* dmx_data)
{

    if(NULL == profile_data || NULL == dmx_data)
        return 0;

    const profile_0702_fx_w_16bit_t* p_profile = (profile_0702_fx_w_16bit_t*)&profile_data[0];
    uint8_t fx_state;
    dmx_data->lightness = endian_Swap_16bit(profile_data) * 1000 / 65535 ;
    dmx_data->type = DMX_TYPE_FX;
    dmx_model_extend((uint8_t*)&profile_data[0], sizeof(profile_0702_fx_w_16bit_t), DMX_16BIT_TYPE);

    switch(p_profile->type / 10)
    {
        case 0:
            dmx_data->arg.fx.type = DMX_FX_PAPARAZZI;
            dmx_data->arg.fx.arg.paparazzi.frq = fx_frq_calculate_8bit(p_profile->frq);
            fx_state = p_profile->state < 70 ? DMX_FX_Trigger_Continue : DMX_FX_Trigger_None;
            break;
        case 1:
            dmx_data->arg.fx.type = DMX_FX_FIREWORKS;
            dmx_data->arg.fx.arg.fireworks.frq = fx_frq_calculate_8bit(p_profile->frq);
            fx_state = p_profile->state < 70 ? DMX_FX_Trigger_Continue : DMX_FX_Trigger_None;
            break;
        case 2:
            dmx_data->arg.fx.type = DMX_FX_FAULT_BULB;
            dmx_data->arg.fx.arg.fault_bulb.frq = fx_frq_calculate_8bit(p_profile->frq);
            fx_state = p_profile->state < 70 ? DMX_FX_Trigger_Continue : DMX_FX_Trigger_None;
            break;
        case 3:
            dmx_data->arg.fx.type = DMX_FX_LIGHTNING;
            dmx_data->arg.fx.arg.lightning.frq = fx_frq_calculate_8bit(p_profile->frq);
            fx_state = p_profile->state > 69 ? DMX_FX_Trigger_None : ((p_profile->state > 9) ? DMX_FX_Trigger_Once : DMX_FX_Trigger_Continue);
            break;
        case 4:
            dmx_data->arg.fx.type = DMX_FX_TV;
            dmx_data->arg.fx.arg.tv.frq = fx_frq_calculate_8bit(p_profile->frq);
            fx_state = p_profile->state < 70 ? DMX_FX_Trigger_Continue : DMX_FX_Trigger_None;
            break;
        case 5:
            dmx_data->arg.fx.type = DMX_FX_PULSING;
            dmx_data->arg.fx.arg.pulsing.spd = fx_frq_calculate_8bit(p_profile->frq);
            fx_state = p_profile->state < 70 ? DMX_FX_Trigger_Continue : DMX_FX_Trigger_None;
            break;
        case 6:
            dmx_data->arg.fx.type = DMX_FX_STROBE;
            dmx_data->arg.fx.arg.strobe.spd = fx_frq_calculate_8bit(p_profile->frq);
            fx_state = p_profile->state < 70 ? DMX_FX_Trigger_Continue : DMX_FX_Trigger_None;
            break;
        case 7:
        default:
            dmx_data->arg.fx.type = DMX_FX_EXPLOSION;
            dmx_data->arg.fx.arg.explosion.decay = fx_frq_calculate_8bit(p_profile->frq);
            fx_state = p_profile->state < 70 ? DMX_FX_Trigger_Once : DMX_FX_Trigger_None;
            break;
    }

    return fx_state;

}

uint8_t profile_0703_fx_lite_8bit_analysis(const uint8_t* profile_data, dmx_data_t* dmx_data)
{
    if(NULL == profile_data || NULL == dmx_data)
        return 0;

    const profile_0703_fx_lite_8bit_t* p_profile = (profile_0703_fx_lite_8bit_t*)&profile_data[0];
    uint8_t fx_state;
    dmx_data->lightness = (*(uint8_t*)profile_data * 1000 / 255 );
    dmx_data->type = DMX_TYPE_FX;
    dmx_model_extend((uint8_t*)&profile_data[0], sizeof(profile_0703_fx_lite_8bit_t), DMX_8BIT_TYPE);
//    dmx_data->arg.fx.cct = cct_8bit_calculate((*(uint8_t*)&p_profile->cct), DMX_CCT_MAX, DMX_CCT_MIN);

    switch(p_profile->type / 10)
    {
        case 0:
            dmx_data->arg.fx.type = DMX_FX_PAPARAZZI;
            dmx_data->arg.fx.arg.paparazzi.frq = fx_frq_calculate_8bit(p_profile->frq);
//			dmx_data->arg.fx.arg.paparazzi.cct = cct_8bit_calculate((*(uint8_t*)&p_profile->cct), DMX_CCT_MAX, DMX_CCT_MIN);
            fx_state = p_profile->state < 70 ? DMX_FX_Trigger_Continue : DMX_FX_Trigger_None;
            break;
        case 1:
            dmx_data->arg.fx.type = DMX_FX_FIREWORKS;
            dmx_data->arg.fx.arg.fireworks.frq = fx_frq_calculate_8bit(p_profile->frq);
//			dmx_data->arg.fx.arg.fireworks.type = dmx_to_convert(p_profile->cct, 255, 2, 0, 10);
            fx_state = p_profile->state < 70 ? DMX_FX_Trigger_Continue : DMX_FX_Trigger_None;
            break;
        case 2:
            dmx_data->arg.fx.type = DMX_FX_FAULT_BULB;
            dmx_data->arg.fx.arg.fault_bulb.frq = fx_frq_calculate_8bit(p_profile->frq);
			dmx_data->arg.fx.arg.fault_bulb.mode = 0;
//			dmx_data->arg.fx.arg.fault_bulb.mode_arg.cct.cct = cct_8bit_calculate((*(uint8_t*)&p_profile->cct), DMX_CCT_MAX, DMX_CCT_MIN);
            fx_state = p_profile->state < 70 ? DMX_FX_Trigger_Continue : DMX_FX_Trigger_None;
            break;
        case 3:
            dmx_data->arg.fx.type = DMX_FX_LIGHTNING;
            dmx_data->arg.fx.arg.lightning.frq = fx_frq_calculate_8bit(p_profile->frq);
//			dmx_data->arg.fx.arg.lightning.cct = cct_8bit_calculate((*(uint8_t*)&p_profile->cct), DMX_CCT_MAX, DMX_CCT_MIN);
            fx_state = p_profile->state > 69 ? DMX_FX_Trigger_None : ((p_profile->state > 9) ? DMX_FX_Trigger_Once : DMX_FX_Trigger_Continue);
            break;
        case 4:
            dmx_data->arg.fx.type = DMX_FX_TV;
            dmx_data->arg.fx.arg.tv.frq = fx_frq_calculate_8bit(p_profile->frq);
//			dmx_data->arg.fx.arg.tv.cct_range = dmx_to_convert(p_profile->cct, 255, 2, 0, 10);
            fx_state = p_profile->state < 70 ? DMX_FX_Trigger_Continue : DMX_FX_Trigger_None;
            break;
        case 5:
            dmx_data->arg.fx.type = DMX_FX_PULSING;
            dmx_data->arg.fx.arg.pulsing.frq = fx_frq_calculate_8bit(p_profile->frq);
			dmx_data->arg.fx.arg.pulsing.mode = 0;
//			dmx_data->arg.fx.arg.pulsing.mode_arg.cct.cct = cct_8bit_calculate((*(uint8_t*)&p_profile->cct), DMX_CCT_MAX, DMX_CCT_MIN);
            fx_state = p_profile->state < 70 ? DMX_FX_Trigger_Continue : DMX_FX_Trigger_None;
            break;
        case 6:
            dmx_data->arg.fx.type = DMX_FX_STROBE;
            dmx_data->arg.fx.arg.strobe.spd = fx_frq_calculate_8bit(p_profile->frq);
			dmx_data->arg.fx.arg.strobe.mode = 0;
//			dmx_data->arg.fx.arg.strobe.mode_arg.cct.cct = cct_8bit_calculate((*(uint8_t*)&p_profile->cct), DMX_CCT_MAX, DMX_CCT_MIN);
            fx_state = p_profile->state < 70 ? DMX_FX_Trigger_Continue : DMX_FX_Trigger_None;
            break;
        case 7:
            dmx_data->arg.fx.type = DMX_FX_EXPLOSION;
            dmx_data->arg.fx.arg.explosion.decay = fx_frq_calculate_8bit(p_profile->frq);
			dmx_data->arg.fx.arg.explosion.mode = 0;
//			dmx_data->arg.fx.arg.explosion.mode_arg.cct.cct = cct_8bit_calculate((*(uint8_t*)&p_profile->cct), DMX_CCT_MAX, DMX_CCT_MIN);
            fx_state = p_profile->state < 70 ? DMX_FX_Trigger_Once : DMX_FX_Trigger_None;
            break;
        case 8:
        default:
            dmx_data->arg.fx.type = DMX_FX_FIRE;
            dmx_data->arg.fx.arg.fire.frq = fx_frq_calculate_8bit(p_profile->frq);
//			dmx_data->arg.fx.arg.fire.cct_range = dmx_to_convert(p_profile->cct, 255, 2, 0, 10);
            fx_state = p_profile->state < 70 ? DMX_FX_Trigger_Continue : DMX_FX_Trigger_None;
            break;
    }

    return fx_state;

}

uint8_t profile_0704_fx_lite_16bit_analysis(const uint8_t* profile_data, dmx_data_t* dmx_data)
{

    if(NULL == profile_data || NULL == dmx_data)
        return 0;

    const profile_0704_fx_lite_16bit_t* p_profile = (profile_0704_fx_lite_16bit_t*)&profile_data[0];
    uint8_t fx_state;
    dmx_data->lightness = endian_Swap_16bit(profile_data) * 1000  / 65535 ;
    dmx_data->type = DMX_TYPE_FX;
    dmx_model_extend((uint8_t*)&profile_data[0], sizeof(profile_0704_fx_lite_16bit_t), DMX_16BIT_TYPE);

    switch(p_profile->type / 10)
    {
        case 0:

            dmx_data->arg.fx.type = DMX_FX_PAPARAZZI;
            dmx_data->arg.fx.arg.paparazzi.frq = fx_frq_calculate_8bit(p_profile->frq);
//			dmx_data->arg.fx.arg.paparazzi.cct = fx_cct_16bit_calculate(p_profile->cct, DMX_CCT_MAX, DMX_CCT_MIN);
            fx_state = p_profile->state < 70 ? DMX_FX_Trigger_Continue : DMX_FX_Trigger_None;
            break;

        case 1:
            dmx_data->arg.fx.type = DMX_FX_FIREWORKS;
            dmx_data->arg.fx.arg.fireworks.frq = fx_frq_calculate_8bit(p_profile->frq);
//			dmx_data->arg.fx.arg.fireworks.type = convert_dmx_to_ctr_dy_gap(p_profile->cct, 0, 65535, 0, 2, 10);
            fx_state = p_profile->state < 70 ? DMX_FX_Trigger_Continue : DMX_FX_Trigger_None;
            break;

        case 2:
            dmx_data->arg.fx.type = DMX_FX_FAULT_BULB;
			dmx_data->arg.fx.arg.fault_bulb.mode = DMX_FX_MODE_CCT;
//			dmx_data->arg.fx.arg.fault_bulb.mode_arg.cct.cct = fx_cct_16bit_calculate(p_profile->cct, DMX_CCT_MAX, DMX_CCT_MIN);
            dmx_data->arg.fx.arg.fault_bulb.frq = fx_frq_calculate_8bit(p_profile->frq);
            fx_state = p_profile->state < 70 ? DMX_FX_Trigger_Continue : DMX_FX_Trigger_None;
            break;
        case 3:
            dmx_data->arg.fx.type = DMX_FX_LIGHTNING;
            dmx_data->arg.fx.arg.lightning.frq = fx_frq_calculate_8bit(p_profile->frq);
//			dmx_data->arg.fx.arg.lightning.cct = fx_cct_16bit_calculate(p_profile->cct, DMX_CCT_MAX, DMX_CCT_MIN);
            fx_state = p_profile->state > 69 ? DMX_FX_Trigger_None : ((p_profile->state > 9) ? DMX_FX_Trigger_Once : DMX_FX_Trigger_Continue);
            break;
        case 4:
            dmx_data->arg.fx.type = DMX_FX_TV;
            dmx_data->arg.fx.arg.tv.frq = fx_frq_calculate_8bit(p_profile->frq);
//			dmx_data->arg.fx.arg.tv.cct_range = convert_dmx_to_ctr_dy_gap(p_profile->cct, 0, 65535, 0, 2, 10);
            fx_state = p_profile->state < 70 ? DMX_FX_Trigger_Continue : DMX_FX_Trigger_None;
            break;
        case 5:
            dmx_data->arg.fx.type = DMX_FX_PULSING;
			dmx_data->arg.fx.arg.pulsing.mode = DMX_FX_MODE_CCT;
//			dmx_data->arg.fx.arg.pulsing.mode_arg.cct.cct = fx_cct_16bit_calculate(p_profile->cct, DMX_CCT_MAX, DMX_CCT_MIN);
            dmx_data->arg.fx.arg.pulsing.frq = fx_frq_calculate_8bit(p_profile->frq);
            fx_state = p_profile->state < 70 ? DMX_FX_Trigger_Continue : DMX_FX_Trigger_None;
            break;
        case 6:
            dmx_data->arg.fx.type = DMX_FX_STROBE;
			dmx_data->arg.fx.arg.strobe.mode = DMX_FX_MODE_CCT;
//			dmx_data->arg.fx.arg.strobe.mode_arg.cct.cct = fx_cct_16bit_calculate(p_profile->cct, DMX_CCT_MAX, DMX_CCT_MIN);
            dmx_data->arg.fx.arg.strobe.spd = fx_frq_calculate_8bit(p_profile->frq);
            fx_state = p_profile->state < 70 ? DMX_FX_Trigger_Continue : DMX_FX_Trigger_None;
            break;
        case 7:
            dmx_data->arg.fx.type = DMX_FX_EXPLOSION;
			dmx_data->arg.fx.arg.explosion.mode = DMX_FX_MODE_CCT;
//			dmx_data->arg.fx.arg.explosion.mode_arg.cct.cct = fx_cct_16bit_calculate(p_profile->cct, DMX_CCT_MAX, DMX_CCT_MIN);
            dmx_data->arg.fx.arg.explosion.decay = fx_frq_calculate_8bit(p_profile->frq);
            fx_state = p_profile->state < 70 ? DMX_FX_Trigger_Once : DMX_FX_Trigger_None;
            break;
        case 8:
        default:
            dmx_data->arg.fx.type = DMX_FX_FIRE;
            dmx_data->arg.fx.arg.fire.frq = fx_frq_calculate_8bit(p_profile->frq);
//			dmx_data->arg.fx.arg.fire.cct_range = convert_dmx_to_ctr_dy_gap(p_profile->cct, 0, 65535, 0, 2, 10);
            fx_state = p_profile->state < 70 ? DMX_FX_Trigger_Continue : DMX_FX_Trigger_None;
            break;
    }

    return fx_state;

}

uint8_t profile_0705_fx_rgb_lite_I_8bit_analysis(const uint8_t* profile_data, dmx_data_t* dmx_data)
{
    if(NULL == profile_data || NULL == dmx_data)
        return 0;

    const profile_0705_fx_rgb_lite_I_8bit_t* p_profile = (profile_0705_fx_rgb_lite_I_8bit_t*)&profile_data[0];
    uint8_t fx_state;
    dmx_data->lightness = (*(uint8_t*)profile_data * 1000 / 255 );
    dmx_data->type = DMX_TYPE_FX;
    dmx_model_extend((uint8_t*)&profile_data[0], sizeof(profile_0705_fx_rgb_lite_I_8bit_t), DMX_8BIT_TYPE);

    switch(p_profile->type / 10)
    {
        case 0:

            dmx_data->arg.fx.type = DMX_FX_PAPARAZZI;
            dmx_data->arg.fx.arg.paparazzi.cct = cct_8bit_calculate(p_profile->arg.paparazzi.cct, DMX_CCT_MAX, DMX_CCT_MIN);
            dmx_data->arg.fx.arg.paparazzi.frq = (fx_frq_calculate_8bit(p_profile->arg.paparazzi.frq) > 10) ? 10 : fx_frq_calculate_8bit(
			p_profile->arg.paparazzi.frq);
            dmx_data->arg.fx.arg.paparazzi.duv = fx_dmx_gm_8bit_calculate(p_profile->arg.paparazzi.gm);
            fx_state = p_profile->state < 70 ? DMX_FX_Trigger_Continue : DMX_FX_Trigger_None;
            break;

        case 1:
            dmx_data->arg.fx.type = DMX_FX_LIGHTNING;
            dmx_data->arg.fx.arg.lightning.cct = cct_8bit_calculate(p_profile->arg.lightning.cct, DMX_CCT_MAX, DMX_CCT_MIN);
            dmx_data->arg.fx.arg.lightning.duv = fx_dmx_gm_8bit_calculate(p_profile->arg.lightning.gm);
            dmx_data->arg.fx.arg.lightning.frq = fx_frq_calculate_8bit(p_profile->arg.lightning.frq);

            fx_state = p_profile->state > 69 ? DMX_FX_Trigger_None : ((p_profile->state > 9) ? DMX_FX_Trigger_Once : DMX_FX_Trigger_Continue);
            break;

        case 2:
            dmx_data->arg.fx.type = DMX_FX_TV;
            dmx_data->arg.fx.arg.tv.cct_range = dmx_to_convert(p_profile->arg.tv.cct_rang, 255, 2, 0, 10);
            dmx_data->arg.fx.arg.tv.frq = fx_frq_calculate_8bit(p_profile->arg.tv.frq);
            fx_state = p_profile->state < 70 ? DMX_FX_Trigger_Continue : DMX_FX_Trigger_None;
            break;

        case 3:
            dmx_data->arg.fx.type = DMX_FX_FIRE;
            dmx_data->arg.fx.arg.fire.cct_range = dmx_to_convert(p_profile->arg.fire.cct_rang, 255, 2, 0, 10);
            dmx_data->arg.fx.arg.fire.frq = fx_frq_calculate_8bit(p_profile->arg.fire.frq);
            fx_state = p_profile->state < 70 ? DMX_FX_Trigger_Continue : DMX_FX_Trigger_None;
            break;

        case 4:
            dmx_data->arg.fx.type = DMX_FX_FAULT_BULB;
            dmx_data->arg.fx.arg.fault_bulb.mode = (enum dmx_fx_mode)dmx_to_convert(p_profile->arg.fault_bulb.mode, 255, 2, 0, 10);

            switch(dmx_data->arg.fx.arg.fault_bulb.mode)
            {
                case DMX_FX_MODE_CCT:
                    dmx_data->arg.fx.arg.fault_bulb.mode_arg.cct.cct = cct_8bit_calculate(p_profile->arg.fault_bulb.arg.cct.cct, DMX_CCT_MAX, DMX_CCT_MIN);
                    dmx_data->arg.fx.arg.fault_bulb.mode_arg.cct.duv = fx_dmx_gm_8bit_calculate(p_profile->arg.fault_bulb.arg.cct.gm);
                    dmx_data->arg.fx.arg.fault_bulb.frq = fx_frq_calculate_8bit(p_profile->arg.fault_bulb.arg.cct.frq);
                    break;

                case DMX_FX_MODE_HSI:
                    dmx_data->arg.fx.arg.fault_bulb.mode_arg.hsi.hue = 1 + (359 * p_profile->arg.fault_bulb.arg.hsi.hue / 255.0f);
                    dmx_data->arg.fx.arg.fault_bulb.mode_arg.hsi.sat = 100 * p_profile->arg.fault_bulb.arg.hsi.sat / 255.0f;
                    dmx_data->arg.fx.arg.fault_bulb.frq  = fx_frq_calculate_8bit(p_profile->arg.fault_bulb.arg.hsi.frq);
                    break;

                case DMX_FX_MODE_GEL:

                    dmx_data->arg.fx.arg.fault_bulb.mode_arg.gel.cct = cct_8bit_calculate(p_profile->arg.fault_bulb.arg.gel.cct, DMX_CCT_MAX, DMX_CCT_MIN);
                    dmx_data->arg.fx.arg.fault_bulb.mode_arg.gel.brand = p_profile->arg.fault_bulb.arg.gel.brand > 29 ? 1 : 0;
                    dmx_data->arg.fx.arg.fault_bulb.mode_arg.gel.type    = gel_series_calculate_8bit(p_profile->arg.fault_bulb.arg.gel.type, dmx_data->arg.fx.arg.fault_bulb.mode_arg.gel.brand);
                    dmx_data->arg.fx.arg.fault_bulb.mode_arg.gel.color  = gel_color_calculate_8bit(p_profile->arg.fault_bulb.arg.gel.color, dmx_data->arg.fx.arg.fault_bulb.mode_arg.gel.brand, dmx_data->arg.fx.arg.fault_bulb.mode_arg.gel.type);
                    dmx_data->arg.fx.arg.fault_bulb.frq = fx_frq_calculate_8bit(p_profile->arg.fault_bulb.arg.gel.frq);
                    break;

                default:
                    break;
            }

            fx_state = p_profile->state < 70 ? DMX_FX_Trigger_Continue : DMX_FX_Trigger_None;
            break;

        case 5:
            dmx_data->arg.fx.type = DMX_FX_PULSING;
            dmx_data->arg.fx.arg.pulsing.mode = (enum dmx_fx_mode)dmx_to_convert(p_profile->arg.pulsing.mode, 255, 2, 0, 10);

            switch(dmx_data->arg.fx.arg.pulsing.mode)
            {
                case DMX_FX_MODE_CCT:
                    dmx_data->arg.fx.arg.pulsing.mode_arg.cct.cct = cct_8bit_calculate(p_profile->arg.pulsing.arg.cct.cct, DMX_CCT_MAX, DMX_CCT_MIN);
                    dmx_data->arg.fx.arg.pulsing.mode_arg.cct.duv = fx_dmx_gm_8bit_calculate(p_profile->arg.pulsing.arg.cct.gm);
                    dmx_data->arg.fx.arg.pulsing.frq = fx_frq_calculate_8bit(p_profile->arg.pulsing.arg.cct.frq);
                    break;

                case DMX_FX_MODE_HSI:
                    dmx_data->arg.fx.arg.pulsing.mode_arg.hsi.hue = 1 + (359 * p_profile->arg.pulsing.arg.hsi.hue / 255.0f);
                    dmx_data->arg.fx.arg.pulsing.mode_arg.hsi.sat = 100 * p_profile->arg.pulsing.arg.hsi.sat / 255.0f;
                    dmx_data->arg.fx.arg.pulsing.frq = fx_frq_calculate_8bit(p_profile->arg.pulsing.arg.hsi.frq);
                    break;

                case DMX_FX_MODE_GEL:

                    dmx_data->arg.fx.arg.pulsing.mode_arg.gel.cct = cct_8bit_calculate(p_profile->arg.pulsing.arg.gel.cct, DMX_CCT_MAX, DMX_CCT_MIN);
                    dmx_data->arg.fx.arg.pulsing.mode_arg.gel.brand = p_profile->arg.pulsing.arg.gel.brand > 29 ? 1 : 0;
                    dmx_data->arg.fx.arg.pulsing.mode_arg.gel.type    = gel_series_calculate_8bit(p_profile->arg.pulsing.arg.gel.type, dmx_data->arg.fx.arg.pulsing.mode_arg.gel.brand);
                    dmx_data->arg.fx.arg.pulsing.mode_arg.gel.color  = gel_color_calculate_8bit(p_profile->arg.pulsing.arg.gel.color, dmx_data->arg.fx.arg.pulsing.mode_arg.gel.brand, dmx_data->arg.fx.arg.pulsing.mode_arg.gel.type);
                    dmx_data->arg.fx.arg.pulsing.frq = fx_frq_calculate_8bit(p_profile->arg.pulsing.arg.gel.frq);
                    break;

                default:
                    break;
            }

            fx_state = p_profile->state < 70 ? DMX_FX_Trigger_Continue : DMX_FX_Trigger_None;
            break;

        case 6:
            dmx_data->arg.fx.type = DMX_FX_COP_CAR;
            dmx_data->arg.fx.arg.copcar.color = dmx_to_convert(p_profile->arg.copcar.color, 255, 4, 0, 10);
            dmx_data->arg.fx.arg.copcar.frq = fx_frq_calculate_8bit(p_profile->arg.copcar.frq);
            fx_state = p_profile->state < 70 ? DMX_FX_Trigger_Continue : DMX_FX_Trigger_None;
            break;

        case 7:
            dmx_data->arg.fx.type = DMX_FX_PARTY_LIGHTS;
            dmx_data->arg.fx.arg.party_lights.sat = p_profile->arg.party_lights.sat * 100 / 255;
            dmx_data->arg.fx.arg.party_lights.frq = fx_frq_calculate_8bit(p_profile->arg.party_lights.frq);
            fx_state = p_profile->state < 70 ? DMX_FX_Trigger_Continue : DMX_FX_Trigger_None;
            break;

        case 8:
        default:
            dmx_data->arg.fx.type = DMX_FX_FIREWORKS;

            dmx_data->arg.fx.arg.fireworks.frq = fx_frq_calculate_8bit(p_profile->arg.fireworks.frq);
            dmx_data->arg.fx.arg.fireworks.type = dmx_to_convert(p_profile->arg.fireworks.color, 255, 2, 0, 10);
            fx_state = p_profile->state < 70 ? DMX_FX_Trigger_Continue : DMX_FX_Trigger_None;
            break;


    }

    return fx_state;
}


uint8_t profile_0706_fx_rgb_lite_III_8bit_analysis(const uint8_t* profile_data, dmx_data_t* dmx_data)
{
    if(NULL == profile_data || NULL == dmx_data)
        return 0;

    const profile_0706_fx_rgb_lite_III_8bit_t* p_profile = (profile_0706_fx_rgb_lite_III_8bit_t*)&profile_data[0];
    uint8_t fx_state;
    dmx_data->lightness = endian_Swap_16bit(profile_data) * 1000 / 65535 ;
    dmx_data->type = DMX_TYPE_FX_II;
    dmx_model_extend((uint8_t*)&profile_data[0], sizeof(profile_0706_fx_rgb_lite_III_8bit_t), DMX_8BIT_TYPE);

    switch(p_profile->type / 10)
    {
        case 0:

            dmx_data->arg.fx2.type = DMX_FX_II_PAPARAZZI;
            dmx_data->arg.fx2.arg.paparazzi2.cct = cct_8bit_calculate(p_profile->arg.paparazzi2.cct, DMX_CCT_MAX, DMX_CCT_MIN);
            dmx_data->arg.fx2.arg.paparazzi2.time_lower_limit = dmx_to_linear_calculate(p_profile->arg.paparazzi2.time_lower_limit, 255, 20, 2, 1);
            dmx_data->arg.fx2.arg.paparazzi2.time_upper_limit = dmx_to_linear_calculate(p_profile->arg.paparazzi2.time_upper_limit, 255, 180, 10, 1);
            dmx_data->arg.fx2.arg.paparazzi2.duv = fx_dmx_gm_8bit_calculate(p_profile->arg.paparazzi2.gm);
            fx_state = p_profile->state < 70 ? DMX_FX_Trigger_Continue : DMX_FX_Trigger_None;
            break;

        case 1:
            dmx_data->arg.fx2.type = DMX_FX_III_LIGHTNING;
            dmx_data->arg.fx2.arg.lightning3.mode = (enum dmx_fx_mode)dmx_to_convert(p_profile->arg.lightning3.mode, 255, 1, 0, 10);

            switch(dmx_data->arg.fx2.arg.lightning3.mode)
            {
                case DMX_FX_MODE_CCT:
                    dmx_data->arg.fx2.arg.lightning3.arg.cct.cct = cct_8bit_calculate(p_profile->arg.lightning3.arg.cct_setting.cct, DMX_CCT_MAX, DMX_CCT_MIN);
                    dmx_data->arg.fx2.arg.lightning3.arg.cct.duv = fx_dmx_gm_8bit_calculate(p_profile->arg.lightning3.arg.cct_setting.gm);
                    dmx_data->arg.fx2.arg.lightning3.time_lower_limit = dmx_to_linear_calculate(p_profile->arg.lightning3.arg.cct_setting.time_lower_limit, 255, 260, 15, 1);
                    dmx_data->arg.fx2.arg.lightning3.time_upper_limit = dmx_to_linear_calculate(p_profile->arg.lightning3.arg.cct_setting.time_upper_limit, 255, 300, 30, 1);
                    break;

                case DMX_FX_MODE_HSI:
                    dmx_data->arg.fx2.arg.lightning3.arg.hsi.hue = 1 + (359 * p_profile->arg.lightning3.arg.hsi_setting.hue / 255.0f);
                    dmx_data->arg.fx2.arg.lightning3.arg.hsi.sat = 100 * p_profile->arg.lightning3.arg.hsi_setting.sat / 255.0f;
                    dmx_data->arg.fx2.arg.lightning3.arg.hsi.cct = fx_cct_8bit_calculate(p_profile->arg.lightning3.arg.hsi_setting.cct, DMX_CCT_MAX, DMX_CCT_MIN);
                    dmx_data->arg.fx2.arg.lightning3.time_lower_limit = dmx_to_linear_calculate(p_profile->arg.lightning3.arg.hsi_setting.time_lower_limit, 255, 260, 15, 1);
                    dmx_data->arg.fx2.arg.lightning3.time_upper_limit = dmx_to_linear_calculate(p_profile->arg.lightning3.arg.hsi_setting.time_upper_limit, 255, 300, 30, 1);
                    break;
                default:
                    break;
            }

            fx_state = p_profile->state > 69 ? DMX_FX_Trigger_None : ((p_profile->state > 9) ? DMX_FX_Trigger_Once : DMX_FX_Trigger_Continue);
            break;

        case 2:
            dmx_data->arg.fx2.type = DMX_FX_III_TV;
            dmx_data->arg.fx2.arg.tv3.mode = (enum dmx_fx_mode)dmx_to_convert(p_profile->arg.tv3.mode, 255, 1, 0, 10);

            switch(dmx_data->arg.fx2.arg.tv3.mode)
            {
                case DMX_FX_MODE_CCT:
                    dmx_data->arg.fx2.arg.tv3.arg.cct.cct_range.c_down_limit = cct_8bit_calculate(p_profile->arg.tv3.arg.cct_setting.cct_lower_limit, DMX_CCT_MAX, DMX_CCT_MIN);
                    dmx_data->arg.fx2.arg.tv3.arg.cct.cct_range.c_up_limit = cct_8bit_calculate(p_profile->arg.tv3.arg.cct_setting.cct_upper_limit, DMX_CCT_MAX, DMX_CCT_MIN);
                    dmx_data->arg.fx2.arg.tv3.arg.cct.duv = fx_dmx_gm_8bit_calculate(p_profile->arg.tv3.arg.cct_setting.gm);
                    dmx_data->arg.fx2.arg.tv3.time_lower_limit = dmx_to_linear_calculate(p_profile->arg.tv3.arg.cct_setting.time_lower_limit, 255, 50, 5, 1);
                    dmx_data->arg.fx2.arg.tv3.time_upper_limit = dmx_to_linear_calculate(p_profile->arg.tv3.arg.cct_setting.time_upper_limit, 255, 150, 25, 1);
                    break;

                case DMX_FX_MODE_HSI:
                    dmx_data->arg.fx2.arg.tv3.arg.hsi.hsi_range.h_down_limit = 1 + (359 * p_profile->arg.tv3.arg.hsi_setting.hsi_lower_limit / 255.0f);
                    dmx_data->arg.fx2.arg.tv3.arg.hsi.hsi_range.h_up_limit = 1 + (359 * p_profile->arg.tv3.arg.hsi_setting.hsi_upper_limit / 255.0f);
                    dmx_data->arg.fx2.arg.tv3.arg.hsi.sat = 100 * p_profile->arg.tv3.arg.hsi_setting.sat / 255.0f;
                    dmx_data->arg.fx2.arg.tv3.arg.hsi.cct = fx_cct_8bit_calculate(p_profile->arg.tv3.arg.hsi_setting.cct, DMX_CCT_MAX, DMX_CCT_MIN);
                    dmx_data->arg.fx2.arg.tv3.time_lower_limit = dmx_to_linear_calculate(p_profile->arg.tv3.arg.hsi_setting.time_lower_limit, 255, 50, 5, 1);
                    dmx_data->arg.fx2.arg.tv3.time_upper_limit = dmx_to_linear_calculate(p_profile->arg.tv3.arg.hsi_setting.time_upper_limit, 255, 150, 25, 1);
                    break;

                default:
                    break;
            }

            fx_state = p_profile->state < 70 ? DMX_FX_Trigger_Continue : DMX_FX_Trigger_None;
            break;

        case 3:
            dmx_data->arg.fx2.type = DMX_FX_III_FIRE;
            dmx_data->arg.fx2.arg.fire3.mode = (enum dmx_fx_mode)dmx_to_convert(p_profile->arg.fire3.mode, 255, 1, 0, 10);

            switch(dmx_data->arg.fx2.arg.fire3.mode)
            {
                case DMX_FX_MODE_CCT:
                    dmx_data->arg.fx2.arg.fire3.arg.cct.cct_range.c_down_limit = cct_8bit_calculate(p_profile->arg.fire3.arg.cct_setting.cct_lower_limit, DMX_CCT_MAX, DMX_CCT_MIN);
                    dmx_data->arg.fx2.arg.fire3.arg.cct.cct_range.c_up_limit = cct_8bit_calculate(p_profile->arg.fire3.arg.cct_setting.cct_upper_limit, DMX_CCT_MAX, DMX_CCT_MIN);
                    dmx_data->arg.fx2.arg.fire3.arg.cct.duv = fx_dmx_gm_8bit_calculate(p_profile->arg.fire3.arg.cct_setting.gm);
                    dmx_data->arg.fx2.arg.fire3.frq = dmx_to_linear_calculate(p_profile->arg.fire3.arg.cct_setting.frq, 255, 180, 18, 1);
                    break;

                case DMX_FX_MODE_HSI:
                    dmx_data->arg.fx2.arg.fire3.arg.hsi.hsi_range.h_down_limit = cct_8bit_calculate(p_profile->arg.fire3.arg.hsi_setting.hsi_lower_limit, DMX_CCT_MAX, DMX_CCT_MIN);
                    dmx_data->arg.fx2.arg.fire3.arg.hsi.hsi_range.h_up_limit = cct_8bit_calculate(p_profile->arg.fire3.arg.hsi_setting.hsi_upper_limit, DMX_CCT_MAX, DMX_CCT_MIN);
                    dmx_data->arg.fx2.arg.fire3.arg.hsi.cct = fx_cct_8bit_calculate(p_profile->arg.fire3.arg.hsi_setting.cct, DMX_CCT_MAX, DMX_CCT_MIN);
                    dmx_data->arg.fx2.arg.fire3.arg.hsi.sat = 100 * p_profile->arg.fire3.arg.hsi_setting.sat / 255.0f;
                    dmx_data->arg.fx2.arg.fire3.frq = dmx_to_linear_calculate(p_profile->arg.fire3.arg.hsi_setting.frq, 255, 180, 18, 1);
                    break;

                default:
                    break;
            }

            fx_state = p_profile->state < 70 ? DMX_FX_Trigger_Continue : DMX_FX_Trigger_None;
            break;

        case 4:
            dmx_data->arg.fx2.type = DMX_FX_III_FAULT_BULB;
            dmx_data->arg.fx2.arg.faulty_bulb3.mode = (enum dmx_fx_mode)dmx_to_convert(p_profile->arg.fault_bulb3.mode, 255, 1, 0, 10);

            switch(dmx_data->arg.fx2.arg.faulty_bulb3.mode)
            {
                case DMX_FX_MODE_CCT:
                    dmx_data->arg.fx2.arg.faulty_bulb3.arg.cct.cct = cct_8bit_calculate(p_profile->arg.fault_bulb3.arg.cct_setting.cct, DMX_CCT_MAX, DMX_CCT_MIN);
                    dmx_data->arg.fx2.arg.faulty_bulb3.arg.cct.duv = fx_dmx_gm_8bit_calculate(p_profile->arg.fault_bulb3.arg.cct_setting.gm);
                    dmx_data->arg.fx2.arg.faulty_bulb3.time_lower_limit = dmx_to_linear_calculate(p_profile->arg.fault_bulb3.arg.cct_setting.time_lower_limit, 255, 18, 3, 1);
                    dmx_data->arg.fx2.arg.faulty_bulb3.time_upper_limit = dmx_to_linear_calculate(p_profile->arg.fault_bulb3.arg.cct_setting.time_upper_limit, 255, 75, 18, 1);
                    break;

                case DMX_FX_MODE_HSI:
                    dmx_data->arg.fx2.arg.faulty_bulb3.arg.hsi.hue = 1 + (359 * p_profile->arg.fault_bulb3.arg.hsi_setting.hue / 255.0f);
                    dmx_data->arg.fx2.arg.faulty_bulb3.arg.hsi.sat = 100 * p_profile->arg.fault_bulb3.arg.hsi_setting.sat / 255.0f;
                    dmx_data->arg.fx2.arg.faulty_bulb3.arg.hsi.cct = fx_cct_8bit_calculate(p_profile->arg.fault_bulb3.arg.hsi_setting.cct, DMX_CCT_MAX, DMX_CCT_MIN);
                    dmx_data->arg.fx2.arg.faulty_bulb3.time_lower_limit = dmx_to_linear_calculate(p_profile->arg.fault_bulb3.arg.hsi_setting.time_lower_limit, 255, 18, 3, 1);
                    dmx_data->arg.fx2.arg.faulty_bulb3.time_upper_limit = dmx_to_linear_calculate(p_profile->arg.fault_bulb3.arg.hsi_setting.time_upper_limit, 255, 75, 18, 1);
                    break;

                default:
                    break;
            }

            fx_state = p_profile->state < 70 ? DMX_FX_Trigger_Continue : DMX_FX_Trigger_None;
            break;

        case 5:
            dmx_data->arg.fx2.type = DMX_FX_III_PULSING;
            dmx_data->arg.fx2.arg.pulsing3.mode = (enum dmx_fx_mode)dmx_to_convert(p_profile->arg.pulsing3.mode, 255, 1, 0, 10);

            switch(dmx_data->arg.fx2.arg.pulsing3.mode)
            {
                case DMX_FX_MODE_CCT:
                    dmx_data->arg.fx2.arg.pulsing3.arg.cct.cct = cct_8bit_calculate(p_profile->arg.pulsing3.arg.cct.cct, DMX_CCT_MAX, DMX_CCT_MIN);
                    dmx_data->arg.fx2.arg.pulsing3.arg.cct.duv = fx_dmx_gm_8bit_calculate(p_profile->arg.pulsing3.arg.cct.gm);
                    dmx_data->arg.fx2.arg.pulsing3.pluse = dmx_to_linear_calculate(p_profile->arg.pulsing3.arg.cct.pluses, 255, 200, 10, 1);
                    break;

                case DMX_FX_MODE_HSI:
                    dmx_data->arg.fx2.arg.pulsing3.arg.hsi.hue = 1 + (359 * p_profile->arg.pulsing3.arg.hsi.hue / 255.0f);
                    dmx_data->arg.fx2.arg.pulsing3.arg.hsi.sat = 100 * p_profile->arg.pulsing3.arg.hsi.sat / 255.0f;
                    dmx_data->arg.fx2.arg.pulsing3.arg.hsi.cct = fx_cct_8bit_calculate(p_profile->arg.pulsing3.arg.hsi.cct, DMX_CCT_MAX, DMX_CCT_MIN);
                    dmx_data->arg.fx2.arg.pulsing3.pluse = dmx_to_linear_calculate(p_profile->arg.pulsing3.arg.hsi.pluses, 255, 200, 10, 1);
                    break;

                default:
                    break;
            }

            fx_state = p_profile->state < 70 ? DMX_FX_Trigger_Continue : DMX_FX_Trigger_None;
            break;

        case 6:
            dmx_data->arg.fx2.type = DMX_FX_III_COP_CAR;
            dmx_data->arg.fx2.arg.cop_car3.color = dmx_to_convert(p_profile->arg.copcar3.color, 255, 4, 0, 10);
            dmx_data->arg.fx2.arg.cop_car3.frq = dmx_to_convert(p_profile->arg.copcar3.frq, 255, 5, 0, 10);
            fx_state = p_profile->state < 70 ? DMX_FX_Trigger_Continue : DMX_FX_Trigger_None;
            break;

        case 7:

            dmx_data->arg.fx2.type = DMX_FX_II_PARTY_LIGHTS;
            dmx_data->arg.fx2.arg.party_lights2.sat = 100 * p_profile->arg.Party_Lights2.sat / 255.0f;
            dmx_data->arg.fx2.arg.party_lights2.spd = dmx_to_convert(p_profile->arg.Party_Lights2.frg, 255, 16, 1, 1);
            fx_state = p_profile->state < 70 ? DMX_FX_Trigger_Continue : DMX_FX_Trigger_None;
            break;

        case 8:
        default:
            dmx_data->arg.fx2.type = DMX_FX_II_FIREWORKS;
            dmx_data->arg.fx2.arg.fireworks2.lower_limit = dmx_to_linear_calculate(p_profile->arg.fireworks2.time_lower_limit, 255, 90, 5, 1);
            dmx_data->arg.fx2.arg.fireworks2.upper_limit = dmx_to_linear_calculate(p_profile->arg.fireworks2.time_upper_limit, 255, 110, 20, 1);
            dmx_data->arg.fx2.arg.fireworks2.color = dmx_to_convert(p_profile->arg.fireworks2.color, 255, 2, 0, 10);
            fx_state = p_profile->state < 70 ? DMX_FX_Trigger_Continue : DMX_FX_Trigger_None;
            break;

    }

    return fx_state;
}

uint8_t profile_0707_fx_rgb_8bit_analysis(const uint8_t* profile_data, dmx_data_t* dmx_data)
{
    if(NULL == profile_data || NULL == dmx_data)
        return 0;

    const profile_0707_fx_rgb_8bit_arg_t* p_profile = (profile_0707_fx_rgb_8bit_arg_t*)&profile_data[0];
    uint8_t fx_state;
    dmx_data->lightness = *(uint8_t*)profile_data * 1000 / 255 ;
    dmx_data->type = DMX_TYPE_FX;
//    dmx_model_extend((uint8_t*)&profile_data[0], sizeof(profile_0707_fx_rgb_8bit_arg_t), DMX_8BIT_TYPE);

    switch(p_profile->type / 10)
    {
        case 0:
			dmx_model_extend((uint8_t*)&profile_data[0], sizeof(fx_rgb_8bit_club_lights_arg_t)+3, DMX_8BIT_TYPE);
            dmx_data->arg.fx.type = DMX_FX_CLUB_LIGHTS;
            dmx_data->arg.fx.arg.clue_lights.color = dmx_to_convert(p_profile->arg.club_lights.color, 255, 7, 0, 10);
            dmx_data->arg.fx.arg.clue_lights.frq = dmx_to_convert(p_profile->arg.club_lights.spd, 255, 10, 1, 10);;
            fx_state = p_profile->state < 70 ? DMX_FX_Trigger_Continue : DMX_FX_Trigger_None;
            break;
        case 1:
			dmx_model_extend((uint8_t*)&profile_data[0], sizeof(fx_rgb_8bit_paparazzi_arg_t)+3, DMX_8BIT_TYPE);
            dmx_data->arg.fx.type = DMX_FX_PAPARAZZI;
            dmx_data->arg.fx.arg.paparazzi.cct = cct_8bit_calculate(p_profile->arg.paparazzi.cct, DMX_CCT_MAX, DMX_CCT_MIN);
            dmx_data->arg.fx.arg.paparazzi.frq = dmx_to_convert(p_profile->arg.paparazzi.frq, 255, 10, 1, 10);
            dmx_data->arg.fx.arg.paparazzi.duv = fx_dmx_gm_8bit_calculate(p_profile->arg.paparazzi.gm);
            fx_state = p_profile->state < 70 ? DMX_FX_Trigger_Continue : DMX_FX_Trigger_None;
            break;

        case 2:
			dmx_model_extend((uint8_t*)&profile_data[0], sizeof(fx_rgb_8bit_lightning_arg_t)+3, DMX_8BIT_TYPE);
            dmx_data->arg.fx.type = DMX_FX_LIGHTNING;
            dmx_data->arg.fx.arg.lightning.cct = cct_8bit_calculate(p_profile->arg.lightning.cct, DMX_CCT_MAX, DMX_CCT_MIN);
            dmx_data->arg.fx.arg.lightning.duv = fx_dmx_gm_8bit_calculate(p_profile->arg.lightning.gm);
            dmx_data->arg.fx.arg.lightning.frq = dmx_to_convert(p_profile->arg.lightning.frq, 255, 11, 1, 10);
            dmx_data->arg.fx.arg.lightning.spd = dmx_to_convert(p_profile->arg.lightning.spd, 255, 11, 1, 10);
            fx_state = p_profile->state > 69 ? DMX_FX_Trigger_None : ((p_profile->state > 9) ? DMX_FX_Trigger_Once : DMX_FX_Trigger_Continue);
            break;

        case 3:
			dmx_model_extend((uint8_t*)&profile_data[0], sizeof(fx_rgb_8bit_tv_arg_t)+3, DMX_8BIT_TYPE);
            dmx_data->arg.fx.type = DMX_FX_TV;
            dmx_data->arg.fx.arg.tv.cct_range = dmx_to_convert(p_profile->arg.tv.cct_rang, 255, 2, 0, 10);
            dmx_data->arg.fx.arg.tv.frq = dmx_to_convert(p_profile->arg.tv.spd, 255, 11, 1, 10);
            fx_state = p_profile->state < 70 ? DMX_FX_Trigger_Continue : DMX_FX_Trigger_None;
            break;

        case 4:
			dmx_model_extend((uint8_t*)&profile_data[0], sizeof(fx_rgb_8bit_candle_arg_t)+3, DMX_8BIT_TYPE);
            dmx_data->arg.fx.type = DMX_FX_CANDLE;
            dmx_data->arg.fx.arg.candle.cct_rang = dmx_to_convert(p_profile->arg.candle.cct_rang, 255, 2, 0, 10);
            dmx_data->arg.fx.arg.candle.frq = dmx_to_convert(p_profile->arg.candle.spd, 255, 10, 1, 10);
            fx_state = p_profile->state < 70 ? DMX_FX_Trigger_Continue : DMX_FX_Trigger_None;
            break;

        case 5:
			dmx_model_extend((uint8_t*)&profile_data[0], sizeof(fx_rgb_8bit_fire_arg_t)+3, DMX_8BIT_TYPE);
            dmx_data->arg.fx.type = DMX_FX_FIRE;
            dmx_data->arg.fx.arg.fire.cct_range = dmx_to_convert(p_profile->arg.fire.cct_rang, 255, 2, 0, 10);
            dmx_data->arg.fx.arg.fire.frq = dmx_to_convert(p_profile->arg.fire.spd, 255, 10, 1, 10);
            fx_state = p_profile->state < 70 ? DMX_FX_Trigger_Continue : DMX_FX_Trigger_None;
            break;

        case 6:
			dmx_model_extend((uint8_t*)&profile_data[0], sizeof(fx_rgb_8bit_strobe_arg_t)+3, DMX_8BIT_TYPE);
            dmx_data->arg.fx.type = DMX_FX_STROBE;
            dmx_data->arg.fx.arg.strobe.mode = (enum dmx_fx_mode)dmx_to_convert(p_profile->arg.strobe.mode, 255, 5, 0, 10);

            switch(dmx_data->arg.fx.arg.strobe.mode)
            {
                case DMX_FX_MODE_CCT:
                    dmx_data->arg.fx.arg.strobe.mode_arg.cct.cct = cct_8bit_calculate(p_profile->arg.strobe.arg.cct.cct, DMX_CCT_MAX, DMX_CCT_MIN);
                    dmx_data->arg.fx.arg.strobe.mode_arg.cct.duv = fx_dmx_gm_8bit_calculate(p_profile->arg.strobe.arg.cct.gm);
                    dmx_data->arg.fx.arg.strobe.spd = dmx_to_convert(p_profile->arg.strobe.arg.cct.spd, 255, 11, 1, 10);
                    break;

                case DMX_FX_MODE_HSI:
                    dmx_data->arg.fx.arg.strobe.mode_arg.hsi.hue = 1 + (359 * p_profile->arg.strobe.arg.hsi.hue / 255.0f);
                    dmx_data->arg.fx.arg.strobe.mode_arg.hsi.sat = 100 * p_profile->arg.strobe.arg.hsi.sat / 255.0f;
                    //dmx_data->arg.fx.arg.strobe.mode_arg.hsi.cct = fx_cct_8bit_calculate(p_profile->arg.strobe.arg.hsi.cct, DMX_CCT_MAX, DMX_CCT_MIN);
                    dmx_data->arg.fx.arg.strobe.spd = dmx_to_convert(p_profile->arg.strobe.arg.hsi.spd, 255, 11, 1, 10);
                    break;

                case DMX_FX_MODE_GEL:

                    dmx_data->arg.fx.arg.strobe.mode_arg.gel.cct   = cct_8bit_calculate(p_profile->arg.strobe.arg.gel.cct, DMX_CCT_MAX, DMX_CCT_MIN);
                    dmx_data->arg.fx.arg.strobe.mode_arg.gel.brand = p_profile->arg.strobe.arg.gel.brand > 29 ? 1 : 0;
                    dmx_data->arg.fx.arg.strobe.mode_arg.gel.type  = gel_series_calculate_8bit(p_profile->arg.strobe.arg.gel.type, dmx_data->arg.fx.arg.strobe.mode_arg.gel.brand);
                    dmx_data->arg.fx.arg.strobe.mode_arg.gel.color = gel_color_calculate_8bit(p_profile->arg.strobe.arg.gel.color, dmx_data->arg.fx.arg.strobe.mode_arg.gel.brand, dmx_data->arg.fx.arg.strobe.mode_arg.gel.type);
                    dmx_data->arg.fx.arg.strobe.spd = dmx_to_convert(p_profile->arg.strobe.arg.gel.spd, 255, 11, 1, 10);
                    break;

                case DMX_FX_MODE_XY:
                    dmx_data->arg.fx.arg.strobe.mode_arg.xy.x = 8000 * p_profile->arg.strobe.arg.xy.x / 255;
                    dmx_data->arg.fx.arg.strobe.mode_arg.xy.y = 8000 * p_profile->arg.strobe.arg.xy.y / 255;
                    dmx_data->arg.fx.arg.strobe.spd = dmx_to_convert(p_profile->arg.strobe.arg.xy.spd, 255, 11, 1, 10);
                    break;

                case DMX_FX_MODE_SOUYRCE:
                    dmx_data->arg.fx.arg.strobe.mode_arg.socue.type = dmx_to_convert(p_profile->arg.strobe.arg.source.type, 255, 45, 0, 3);
                    dmx_data->arg.fx.arg.strobe.spd = dmx_to_convert(p_profile->arg.strobe.arg.source.spd, 255, 11, 1, 10);

                    break;

                case DMX_FX_MODE_RGB:
                    dmx_data->arg.fx.arg.strobe.mode_arg.rgb.r_ratio = 1000 * p_profile->arg.strobe.arg.rgb.r / 255;
                    dmx_data->arg.fx.arg.strobe.mode_arg.rgb.g_ratio = 1000 * p_profile->arg.strobe.arg.rgb.g / 255;
                    dmx_data->arg.fx.arg.strobe.mode_arg.rgb.b_ratio = 1000 * p_profile->arg.strobe.arg.rgb.b / 255;
                    dmx_data->arg.fx.arg.strobe.spd = dmx_to_convert(p_profile->arg.strobe.arg.rgb.spd, 255, 11, 1, 10);
                    break;

                default:
                    break;
            }

            fx_state = p_profile->state < 70 ? DMX_FX_Trigger_Once : DMX_FX_Trigger_None;
            break;

        case 7:
			dmx_model_extend((uint8_t*)&profile_data[0], sizeof(fx_rgb_8bit_explosion_arg_t)+3, DMX_8BIT_TYPE);
            dmx_data->arg.fx.type = DMX_FX_EXPLOSION;
            dmx_data->arg.fx.arg.explosion.mode = (enum dmx_fx_mode)dmx_to_convert(p_profile->arg.explosion.mode, 255, 5, 0, 10);

            switch(dmx_data->arg.fx.arg.explosion.mode)
            {
                case DMX_FX_MODE_CCT:
                    dmx_data->arg.fx.arg.explosion.mode_arg.cct.cct = cct_8bit_calculate(p_profile->arg.explosion.arg.cct.cct, DMX_CCT_MAX, DMX_CCT_MIN);
                    dmx_data->arg.fx.arg.explosion.mode_arg.cct.duv = fx_dmx_gm_8bit_calculate(p_profile->arg.explosion.arg.cct.gm);

                    dmx_data->arg.fx.arg.explosion.decay = dmx_to_convert(p_profile->arg.explosion.arg.cct.decay, 255, 10, 1, 10);
                    break;

                case DMX_FX_MODE_HSI:
                    dmx_data->arg.fx.arg.explosion.mode_arg.hsi.hue = 1 + (359 * p_profile->arg.explosion.arg.hsi.hue / 255.0f);
                    dmx_data->arg.fx.arg.explosion.mode_arg.hsi.sat = 100 * p_profile->arg.explosion.arg.hsi.sat / 255.0f;
                    //dmx_data->arg.fx.arg.explosion.mode_arg.hsi.cct = fx_cct_8bit_calculate(p_profile->arg.explosion.arg.hsi.cct, DMX_CCT_MAX, DMX_CCT_MIN);
                    dmx_data->arg.fx.arg.explosion.decay = dmx_to_convert(p_profile->arg.explosion.arg.hsi.decay, 255, 10, 1, 10);
                    break;

                case DMX_FX_MODE_GEL:

                    dmx_data->arg.fx.arg.explosion.mode_arg.gel.cct   = cct_8bit_calculate(p_profile->arg.explosion.arg.gel.cct, DMX_CCT_MAX, DMX_CCT_MIN);
                    dmx_data->arg.fx.arg.explosion.mode_arg.gel.brand = p_profile->arg.explosion.arg.gel.brand > 29 ? 1 : 0;
                    dmx_data->arg.fx.arg.explosion.mode_arg.gel.type  = gel_series_calculate_8bit(p_profile->arg.explosion.arg.gel.type, dmx_data->arg.fx.arg.explosion.mode_arg.gel.brand);
                    dmx_data->arg.fx.arg.explosion.mode_arg.gel.color = gel_color_calculate_8bit(p_profile->arg.explosion.arg.gel.color, dmx_data->arg.fx.arg.explosion.mode_arg.gel.brand, dmx_data->arg.fx.arg.explosion.mode_arg.gel.type);
                    dmx_data->arg.fx.arg.explosion.decay = dmx_to_convert(p_profile->arg.explosion.arg.gel.decay, 255, 10, 1, 10);
                    break;

                case DMX_FX_MODE_XY:
                    dmx_data->arg.fx.arg.explosion.mode_arg.xy.x = 8000 * p_profile->arg.explosion.arg.xy.x / 255;
                    dmx_data->arg.fx.arg.explosion.mode_arg.xy.y = 8000 * p_profile->arg.explosion.arg.xy.y / 255;
                    dmx_data->arg.fx.arg.explosion.decay = dmx_to_convert(p_profile->arg.explosion.arg.xy.decay, 255, 10, 1, 10);
                    break;

                case DMX_FX_MODE_SOUYRCE:
                    dmx_data->arg.fx.arg.explosion.mode_arg.socue.type = dmx_to_convert(p_profile->arg.explosion.arg.source.type, 255, 45, 0, 3);
                    dmx_data->arg.fx.arg.explosion.decay = dmx_to_convert(p_profile->arg.explosion.arg.source.decay, 255, 10, 1, 10);

                    break;

                case DMX_FX_MODE_RGB:
                    dmx_data->arg.fx.arg.explosion.mode_arg.rgb.r_ratio = 1000 * p_profile->arg.explosion.arg.rgb.r / 255;
                    dmx_data->arg.fx.arg.explosion.mode_arg.rgb.g_ratio = 1000 * p_profile->arg.explosion.arg.rgb.g / 255;
                    dmx_data->arg.fx.arg.explosion.mode_arg.rgb.b_ratio = 1000 * p_profile->arg.explosion.arg.rgb.b / 255;
                    dmx_data->arg.fx.arg.explosion.decay = dmx_to_convert(p_profile->arg.explosion.arg.rgb.decay, 255, 10, 1, 10);
                    break;

                default:
                    break;
            }

            fx_state = p_profile->state < 70 ? DMX_FX_Trigger_Once : DMX_FX_Trigger_None;
            break;

        case 8:
			dmx_model_extend((uint8_t*)&profile_data[0], sizeof(fx_rgb_8bit_fault_bulb_arg_t)+3, DMX_8BIT_TYPE);
            dmx_data->arg.fx.type = DMX_FX_FAULT_BULB;
            dmx_data->arg.fx.arg.fault_bulb.mode = (enum dmx_fx_mode)dmx_to_convert(p_profile->arg.fault_bulb.mode, 255, 5, 0, 10);

            switch(dmx_data->arg.fx.arg.fault_bulb.mode)
            {
                case DMX_FX_MODE_CCT:
                    dmx_data->arg.fx.arg.fault_bulb.mode_arg.cct.cct = cct_8bit_calculate(p_profile->arg.fault_bulb.arg.cct.cct, DMX_CCT_MAX, DMX_CCT_MIN);
                    dmx_data->arg.fx.arg.fault_bulb.mode_arg.cct.duv = fx_dmx_gm_8bit_calculate(p_profile->arg.fault_bulb.arg.cct.gm);
                    dmx_data->arg.fx.arg.fault_bulb.frq = dmx_to_convert(p_profile->arg.fault_bulb.arg.cct.frq, 255, 11, 1, 10);
                    dmx_data->arg.fx.arg.fault_bulb.spd = dmx_to_convert(p_profile->arg.fault_bulb.arg.cct.spd, 255, 10, 1, 10);
                    break;

                case DMX_FX_MODE_HSI:
                    dmx_data->arg.fx.arg.fault_bulb.mode_arg.hsi.hue = 1 + (359 * p_profile->arg.fault_bulb.arg.hsi.hue / 255.0f);
                    dmx_data->arg.fx.arg.fault_bulb.mode_arg.hsi.sat = 100 * p_profile->arg.fault_bulb.arg.hsi.sat / 255.0f;
                    //dmx_data->arg.fx.arg.fault_bulb.mode_arg.hsi.cct = fx_cct_8bit_calculate(p_profile->arg.fault_bulb.arg.hsi.cct, DMX_CCT_MAX, DMX_CCT_MIN);
                    dmx_data->arg.fx.arg.fault_bulb.frq = dmx_to_convert(p_profile->arg.fault_bulb.arg.hsi.frq, 255, 11, 1, 10);
                    dmx_data->arg.fx.arg.fault_bulb.spd = dmx_to_convert(p_profile->arg.fault_bulb.arg.hsi.spd, 255, 10, 1, 10);
                    break;

                case DMX_FX_MODE_GEL:

                    dmx_data->arg.fx.arg.fault_bulb.mode_arg.gel.cct   = cct_8bit_calculate(p_profile->arg.fault_bulb.arg.gel.cct, DMX_CCT_MAX, DMX_CCT_MIN);
                    dmx_data->arg.fx.arg.fault_bulb.mode_arg.gel.brand = p_profile->arg.fault_bulb.arg.gel.brand > 29 ? 1 : 0;
                    dmx_data->arg.fx.arg.fault_bulb.mode_arg.gel.type  = gel_series_calculate_8bit(p_profile->arg.fault_bulb.arg.gel.type, dmx_data->arg.fx.arg.fault_bulb.mode_arg.gel.brand);
                    dmx_data->arg.fx.arg.fault_bulb.mode_arg.gel.color = gel_color_calculate_8bit(p_profile->arg.fault_bulb.arg.gel.color, dmx_data->arg.fx.arg.fault_bulb.mode_arg.gel.brand, dmx_data->arg.fx.arg.fault_bulb.mode_arg.gel.type);
                    dmx_data->arg.fx.arg.fault_bulb.frq = dmx_to_convert(p_profile->arg.fault_bulb.arg.gel.frq, 255, 11, 1, 10);
                    dmx_data->arg.fx.arg.fault_bulb.spd = dmx_to_convert(p_profile->arg.fault_bulb.arg.gel.spd, 255, 10, 1, 10);
                    break;

                case DMX_FX_MODE_XY:
                    dmx_data->arg.fx.arg.fault_bulb.mode_arg.xy.x = 8000 * p_profile->arg.fault_bulb.arg.xy.x / 255;
                    dmx_data->arg.fx.arg.fault_bulb.mode_arg.xy.y = 8000 * p_profile->arg.fault_bulb.arg.xy.y / 255;
                    dmx_data->arg.fx.arg.fault_bulb.frq = dmx_to_convert(p_profile->arg.fault_bulb.arg.xy.frq, 255, 11, 1, 10);
                    dmx_data->arg.fx.arg.fault_bulb.spd = dmx_to_convert(p_profile->arg.fault_bulb.arg.xy.spd, 255, 10, 1, 10);
                    break;

                case DMX_FX_MODE_SOUYRCE:
                    dmx_data->arg.fx.arg.fault_bulb.mode_arg.socue.type = dmx_to_convert(p_profile->arg.fault_bulb.arg.source.type, 255, 45, 0, 3);

                    dmx_data->arg.fx.arg.fault_bulb.frq = dmx_to_convert(p_profile->arg.fault_bulb.arg.source.frq, 255, 11, 1, 10);
                    dmx_data->arg.fx.arg.fault_bulb.spd = dmx_to_convert(p_profile->arg.fault_bulb.arg.source.spd, 255, 10, 1, 10);
                    break;

                case DMX_FX_MODE_RGB:
                    dmx_data->arg.fx.arg.fault_bulb.mode_arg.rgb.r_ratio = 1000 * p_profile->arg.fault_bulb.arg.rgb.r / 255;
                    dmx_data->arg.fx.arg.fault_bulb.mode_arg.rgb.g_ratio = 1000 * p_profile->arg.fault_bulb.arg.rgb.g / 255;
                    dmx_data->arg.fx.arg.fault_bulb.mode_arg.rgb.b_ratio = 1000 * p_profile->arg.fault_bulb.arg.rgb.b / 255;
                    dmx_data->arg.fx.arg.fault_bulb.frq = dmx_to_convert(p_profile->arg.fault_bulb.arg.rgb.frq, 255, 11, 1, 10);
                    dmx_data->arg.fx.arg.fault_bulb.spd = dmx_to_convert(p_profile->arg.fault_bulb.arg.rgb.spd, 255, 10, 1, 10);
                    break;

                default:
                    break;
            }

            fx_state = p_profile->state < 70 ? DMX_FX_Trigger_Continue : DMX_FX_Trigger_None;
            break;

        case 9:
			dmx_model_extend((uint8_t*)&profile_data[0], sizeof(fx_rgb_8bit_pulsing_arg_t)+3, DMX_8BIT_TYPE);
            dmx_data->arg.fx.type = DMX_FX_PULSING;
            dmx_data->arg.fx.arg.pulsing.mode = (enum dmx_fx_mode)dmx_to_convert(p_profile->arg.pulsing.mode, 255, 5, 0, 10);

            switch(dmx_data->arg.fx.arg.pulsing.mode)
            {
                case DMX_FX_MODE_CCT:
                    dmx_data->arg.fx.arg.pulsing.mode_arg.cct.cct = cct_8bit_calculate(p_profile->arg.pulsing.arg.cct.cct, DMX_CCT_MAX, DMX_CCT_MIN);
                    dmx_data->arg.fx.arg.pulsing.mode_arg.cct.duv = fx_dmx_gm_8bit_calculate(p_profile->arg.pulsing.arg.cct.gm);
                    dmx_data->arg.fx.arg.pulsing.frq = dmx_to_convert(p_profile->arg.pulsing.arg.cct.frq, 255, 10, 1, 11);
                    dmx_data->arg.fx.arg.pulsing.spd = dmx_to_convert(p_profile->arg.pulsing.arg.cct.spd, 255, 10, 1, 10);

                    break;

                case DMX_FX_MODE_HSI:
                    dmx_data->arg.fx.arg.pulsing.mode_arg.hsi.hue = 1 + (359 * p_profile->arg.pulsing.arg.hsi.hue / 255.0f);
                    dmx_data->arg.fx.arg.pulsing.mode_arg.hsi.sat = 100 * p_profile->arg.pulsing.arg.hsi.sat / 255.0f;
                    //dmx_data->arg.fx.arg.pulsing.mode_arg.hsi.cct = fx_cct_8bit_calculate(p_profile->arg.pulsing.arg.hsi.cct, DMX_CCT_MAX, DMX_CCT_MIN);
                    dmx_data->arg.fx.arg.pulsing.frq = dmx_to_convert(p_profile->arg.pulsing.arg.hsi.frq, 255, 10, 1, 11);
                    dmx_data->arg.fx.arg.pulsing.spd = dmx_to_convert(p_profile->arg.pulsing.arg.hsi.spd, 255, 10, 1, 10);
                    break;

                case DMX_FX_MODE_GEL:

                    dmx_data->arg.fx.arg.pulsing.mode_arg.gel.cct   = cct_8bit_calculate(p_profile->arg.pulsing.arg.gel.cct, DMX_CCT_MAX, DMX_CCT_MIN);
                    dmx_data->arg.fx.arg.pulsing.mode_arg.gel.brand = p_profile->arg.pulsing.arg.gel.brand > 29 ? 1 : 0;
                    dmx_data->arg.fx.arg.pulsing.mode_arg.gel.type  = gel_series_calculate_8bit(p_profile->arg.pulsing.arg.gel.type, dmx_data->arg.fx.arg.pulsing.mode_arg.gel.brand);
                    dmx_data->arg.fx.arg.pulsing.mode_arg.gel.color = gel_color_calculate_8bit(p_profile->arg.pulsing.arg.gel.color, dmx_data->arg.fx.arg.pulsing.mode_arg.gel.brand, dmx_data->arg.fx.arg.pulsing.mode_arg.gel.type);
                    dmx_data->arg.fx.arg.pulsing.frq = dmx_to_convert(p_profile->arg.pulsing.arg.gel.frq, 255, 10, 1, 11);
                    dmx_data->arg.fx.arg.pulsing.spd = dmx_to_convert(p_profile->arg.pulsing.arg.gel.spd, 255, 10, 1, 10);
                    break;

                case DMX_FX_MODE_XY:
                    dmx_data->arg.fx.arg.pulsing.mode_arg.xy.x = 8000 * p_profile->arg.pulsing.arg.xy.x / 255;
                    dmx_data->arg.fx.arg.pulsing.mode_arg.xy.y = 8000 * p_profile->arg.pulsing.arg.xy.y / 255;
                    dmx_data->arg.fx.arg.pulsing.frq = dmx_to_convert(p_profile->arg.pulsing.arg.xy.frq, 255, 10, 1, 11);
                    dmx_data->arg.fx.arg.pulsing.spd = dmx_to_convert(p_profile->arg.pulsing.arg.xy.spd, 255, 10, 1, 10);
                    break;

                case DMX_FX_MODE_SOUYRCE:
                    dmx_data->arg.fx.arg.pulsing.mode_arg.socue.type = dmx_to_convert(p_profile->arg.pulsing.arg.source.type, 255, 45, 0, 3);
                    dmx_data->arg.fx.arg.pulsing.frq = dmx_to_convert(p_profile->arg.pulsing.arg.source.frq, 255, 10, 1, 11);
                    dmx_data->arg.fx.arg.pulsing.spd = dmx_to_convert(p_profile->arg.pulsing.arg.source.spd, 255, 10, 1, 10);

                    break;

                case DMX_FX_MODE_RGB:
                    dmx_data->arg.fx.arg.pulsing.mode_arg.rgb.r_ratio = 1000 * p_profile->arg.pulsing.arg.rgb.r / 255;
                    dmx_data->arg.fx.arg.pulsing.mode_arg.rgb.g_ratio = 1000 * p_profile->arg.pulsing.arg.rgb.g / 255;
                    dmx_data->arg.fx.arg.pulsing.mode_arg.rgb.b_ratio = 1000 * p_profile->arg.pulsing.arg.rgb.b / 255;
                    dmx_data->arg.fx.arg.pulsing.frq = dmx_to_convert(p_profile->arg.pulsing.arg.rgb.frq, 255, 10, 1, 11);
                    dmx_data->arg.fx.arg.pulsing.spd = dmx_to_convert(p_profile->arg.pulsing.arg.rgb.spd, 255, 10, 1, 10);
                    break;

                default:
                    break;
            }

            fx_state = p_profile->state < 70 ? DMX_FX_Trigger_Continue : DMX_FX_Trigger_None;
            break;

        case 10:
			dmx_model_extend((uint8_t*)&profile_data[0], sizeof(fx_rgb_8bit_welding_arg_t)+3, DMX_8BIT_TYPE);
            dmx_data->arg.fx.type = DMX_FX_WELDING;
            dmx_data->arg.fx.arg.welding.mode = (enum dmx_fx_mode)dmx_to_convert(p_profile->arg.welding.mode, 255, 5, 0, 10);
            dmx_data->arg.fx.arg.welding.min_lightness = p_profile->arg.welding.arg.cct.min_brightness * 1000 / 255;

            switch(dmx_data->arg.fx.arg.welding.mode)
            {
                case DMX_FX_MODE_CCT:
                    dmx_data->arg.fx.arg.welding.mode_arg.cct.cct = cct_8bit_calculate(p_profile->arg.welding.arg.cct.cct, DMX_CCT_MAX, DMX_CCT_MIN);
                    dmx_data->arg.fx.arg.welding.mode_arg.cct.duv = fx_dmx_gm_8bit_calculate(p_profile->arg.welding.arg.cct.gm);
                    dmx_data->arg.fx.arg.welding.frq = dmx_to_convert(p_profile->arg.welding.arg.cct.frq, 255, 10, 1, 11);
                    dmx_data->arg.fx.arg.welding.min_lightness = p_profile->arg.welding.arg.cct.min_brightness * 750 / 255;

                    break;

                case DMX_FX_MODE_HSI:
                    dmx_data->arg.fx.arg.welding.mode_arg.hsi.hue = 1 + (359 * p_profile->arg.welding.arg.hsi.hue / 255.0f);
                    dmx_data->arg.fx.arg.welding.mode_arg.hsi.sat = 100 * p_profile->arg.welding.arg.hsi.sat / 255.0f;
                    //dmx_data->arg.fx.arg.welding.mode_arg.hsi.cct = fx_cct_8bit_calculate(p_profile->arg.welding.arg.hsi.cct, DMX_CCT_MAX, DMX_CCT_MIN);
                    dmx_data->arg.fx.arg.welding.frq = dmx_to_convert(p_profile->arg.welding.arg.hsi.frq, 255, 10, 1, 11);
                    dmx_data->arg.fx.arg.welding.min_lightness = p_profile->arg.welding.arg.hsi.min_brightness * 750 / 255;
                    break;

                case DMX_FX_MODE_GEL:

                    dmx_data->arg.fx.arg.welding.mode_arg.gel.cct   = cct_8bit_calculate(p_profile->arg.welding.arg.gel.cct, DMX_CCT_MAX, DMX_CCT_MIN);
                    dmx_data->arg.fx.arg.welding.mode_arg.gel.brand = p_profile->arg.welding.arg.gel.brand > 29 ? 1 : 0;
                    dmx_data->arg.fx.arg.welding.mode_arg.gel.type  = gel_series_calculate_8bit(p_profile->arg.welding.arg.gel.type, dmx_data->arg.fx.arg.welding.mode_arg.gel.brand);
                    dmx_data->arg.fx.arg.welding.mode_arg.gel.color = gel_color_calculate_8bit(p_profile->arg.welding.arg.gel.color, dmx_data->arg.fx.arg.welding.mode_arg.gel.brand, dmx_data->arg.fx.arg.welding.mode_arg.gel.type);
                    dmx_data->arg.fx.arg.welding.frq = dmx_to_convert(p_profile->arg.welding.arg.gel.frq, 255, 10, 1, 11);
                    dmx_data->arg.fx.arg.welding.min_lightness = p_profile->arg.welding.arg.gel.min_brightness * 750 / 255;
                    break;

                case DMX_FX_MODE_XY:
                    dmx_data->arg.fx.arg.welding.mode_arg.xy.x = 8000 * p_profile->arg.welding.arg.xy.x / 255;
                    dmx_data->arg.fx.arg.welding.mode_arg.xy.y = 8000 * p_profile->arg.welding.arg.xy.y / 255;
                    dmx_data->arg.fx.arg.welding.frq = dmx_to_convert(p_profile->arg.welding.arg.xy.frq, 255, 10, 1, 11);
                    dmx_data->arg.fx.arg.welding.min_lightness = p_profile->arg.welding.arg.cct.min_brightness * 750 / 255;
                    break;

                case DMX_FX_MODE_SOUYRCE:
                    dmx_data->arg.fx.arg.welding.mode_arg.socue.type = dmx_to_convert(p_profile->arg.welding.arg.source.type, 255, 45, 0, 3);
                    dmx_data->arg.fx.arg.welding.frq = dmx_to_convert(p_profile->arg.welding.arg.source.frq, 255, 10, 1, 11);
                    dmx_data->arg.fx.arg.welding.min_lightness = p_profile->arg.welding.arg.source.min_brightness * 750 / 255;

                    break;

                case DMX_FX_MODE_RGB:
                    dmx_data->arg.fx.arg.welding.mode_arg.rgb.r_ratio = 1000 * p_profile->arg.welding.arg.rgb.r / 255;
                    dmx_data->arg.fx.arg.welding.mode_arg.rgb.g_ratio = 1000 * p_profile->arg.welding.arg.rgb.g / 255;
                    dmx_data->arg.fx.arg.welding.mode_arg.rgb.b_ratio = 1000 * p_profile->arg.welding.arg.rgb.b / 255;
                    dmx_data->arg.fx.arg.welding.frq = dmx_to_convert(p_profile->arg.welding.arg.rgb.frq, 255, 10, 1, 11);
                    dmx_data->arg.fx.arg.welding.min_lightness = p_profile->arg.welding.arg.rgb.min_brightness * 750 / 255;
                    break;

                default:
                    break;
            }

            fx_state = p_profile->state < 70 ? DMX_FX_Trigger_Continue : DMX_FX_Trigger_None;
            break;

        case 11:
			dmx_model_extend((uint8_t*)&profile_data[0], sizeof(fx_rgb_8bit_cop_car_arg_t)+3, DMX_8BIT_TYPE);
            dmx_data->arg.fx.type = DMX_FX_COP_CAR;
            dmx_data->arg.fx.arg.copcar.color = dmx_to_convert(p_profile->arg.copcar.color, 255, 4, 0, 10);
            dmx_data->arg.fx.arg.copcar.frq = dmx_to_convert(p_profile->arg.copcar.frq, 255, 5, 0, 10);
            fx_state = p_profile->state < 70 ? DMX_FX_Trigger_Continue : DMX_FX_Trigger_None;
            break;

        case 12:
			dmx_model_extend((uint8_t*)&profile_data[0], sizeof(fx_rgb_8bit_color_chase_arg_t)+3, DMX_8BIT_TYPE);
            dmx_data->arg.fx.type = DMX_FX_COLOR_CHASE;
            dmx_data->arg.fx.arg.color_chase.sat = 100 * p_profile->arg.color_chase.sat / 255.0f;
            dmx_data->arg.fx.arg.color_chase.spd = dmx_to_convert(p_profile->arg.color_chase.spd, 255, 10, 1, 10);
            fx_state = p_profile->state < 70 ? DMX_FX_Trigger_Continue : DMX_FX_Trigger_None;
            break;

        case 13:
			dmx_model_extend((uint8_t*)&profile_data[0], sizeof(fx_rgb_8bit_party_lights_arg_t)+3, DMX_8BIT_TYPE);
            dmx_data->arg.fx.type = DMX_FX_PARTY_LIGHTS;
            dmx_data->arg.fx.arg.party_lights.sat = 100 * p_profile->arg.party_lights.sat / 255.0f;
            dmx_data->arg.fx.arg.party_lights.frq = dmx_to_convert(p_profile->arg.party_lights.spd, 255, 10, 1, 10);
            fx_state = p_profile->state < 70 ? DMX_FX_Trigger_Continue : DMX_FX_Trigger_None;
            break;

        case 14:
			dmx_model_extend((uint8_t*)&profile_data[0], sizeof(fx_rgb_8bit_fireworks_arg_t)+3, DMX_8BIT_TYPE);
            dmx_data->arg.fx.type = DMX_FX_FIREWORKS;

            dmx_data->arg.fx.arg.fireworks.frq = dmx_to_convert(p_profile->arg.fireworks.frq, 255, 10, 1, 10);
            dmx_data->arg.fx.arg.fireworks.type = dmx_to_convert(p_profile->arg.fireworks.color, 255, 2, 0, 10);
            fx_state = p_profile->state < 70 ? DMX_FX_Trigger_Continue : DMX_FX_Trigger_None;
            break;

        case 15:
			dmx_model_extend((uint8_t*)&profile_data[0], sizeof(fx_rgb_8bit_color_fade_arg_t)+3, DMX_8BIT_TYPE);
            dmx_data->arg.fx.type = DMX_FX_COLOR_FADE;
            dmx_data->arg.fx.arg.color_cycle.light_board_a = dmx_to_convert(p_profile->arg.color_fade.light_board_a, 255, 1, 0, 10);
            dmx_data->arg.fx.arg.color_cycle.frq = dmx_to_convert(p_profile->arg.color_fade.frq, 255, 10, 1, 10);
            dmx_data->arg.fx.arg.color_fade.direction = dmx_to_convert(p_profile->arg.color_fade.direction, 255, 1, 0, 10);

            switch(dmx_data->arg.fx.arg.color_fade.light_board_a)
            {
                case DMX_FX_MODE_CCT:
                    dmx_data->arg.fx.arg.color_fade.arg_a.cct.cct = cct_8bit_calculate(p_profile->arg.color_fade.arg_a.cct.cct, DMX_CCT_MAX, DMX_CCT_MIN);
                    dmx_data->arg.fx.arg.color_fade.arg_a.cct.duv = fx_dmx_gm_8bit_calculate(p_profile->arg.color_fade.arg_a.cct.gm);
                    break;

                case DMX_FX_MODE_HSI:
                    dmx_data->arg.fx.arg.color_fade.arg_a.hsi.cct = fx_cct_8bit_calculate(p_profile->arg.color_fade.arg_a.hsi.cct, DMX_CCT_MAX, DMX_CCT_MIN);
                    dmx_data->arg.fx.arg.color_fade.arg_a.hsi.hue = 1 + (359 * p_profile->arg.color_fade.arg_a.hsi.hue / 255.0f);
                    dmx_data->arg.fx.arg.color_fade.arg_a.hsi.sat = 100 * p_profile->arg.color_fade.arg_a.hsi.sat / 255.0f;
                    break;


            }

            dmx_data->arg.fx.arg.color_fade.light_board_b = dmx_to_convert(p_profile->arg.color_fade.light_board_b, 255, 1, 0, 10);

            switch(dmx_data->arg.fx.arg.color_fade.light_board_b)
            {
                case DMX_FX_MODE_CCT:
                    dmx_data->arg.fx.arg.color_fade.arg_b.cct.cct = cct_8bit_calculate(p_profile->arg.color_fade.arg_b.cct.cct, DMX_CCT_MAX, DMX_CCT_MIN);
                    dmx_data->arg.fx.arg.color_fade.arg_b.cct.duv = fx_dmx_gm_8bit_calculate(p_profile->arg.color_fade.arg_b.cct.gm);
                    break;

                case DMX_FX_MODE_HSI:
                    dmx_data->arg.fx.arg.color_fade.arg_b.hsi.cct = fx_cct_8bit_calculate(p_profile->arg.color_fade.arg_b.hsi.cct, DMX_CCT_MAX, DMX_CCT_MIN);
                    dmx_data->arg.fx.arg.color_fade.arg_b.hsi.hue = 1 + (359 * p_profile->arg.color_fade.arg_b.hsi.hue / 255.0f);
                    dmx_data->arg.fx.arg.color_fade.arg_b.hsi.sat = 100 * p_profile->arg.color_fade.arg_b.hsi.sat / 255.0f;
                    break;
            }

            dmx_data->arg.fx.arg.color_fade.light_board_c = dmx_to_convert(p_profile->arg.color_fade.light_board_c, 255, 1, 0, 10);

            switch(dmx_data->arg.fx.arg.color_fade.light_board_c)
            {
                case DMX_FX_MODE_CCT:
                    dmx_data->arg.fx.arg.color_fade.arg_c.cct.cct = cct_8bit_calculate(p_profile->arg.color_fade.arg_c.cct.cct, DMX_CCT_MAX, DMX_CCT_MIN);
                    dmx_data->arg.fx.arg.color_fade.arg_c.cct.duv = fx_dmx_gm_8bit_calculate(p_profile->arg.color_fade.arg_c.cct.gm);
                    break;

                case DMX_FX_MODE_HSI:
                    dmx_data->arg.fx.arg.color_fade.arg_c.hsi.cct = fx_cct_8bit_calculate(p_profile->arg.color_fade.arg_c.hsi.cct, DMX_CCT_MAX, DMX_CCT_MIN);
                    dmx_data->arg.fx.arg.color_fade.arg_c.hsi.hue = 1 + (359 * p_profile->arg.color_fade.arg_c.hsi.hue / 255.0f);
                    dmx_data->arg.fx.arg.color_fade.arg_c.hsi.sat = 100 * p_profile->arg.color_fade.arg_c.hsi.sat / 255.0f;
                    break;
            }

            dmx_data->arg.fx.arg.color_fade.light_board_d = dmx_to_convert(p_profile->arg.color_fade.light_board_d, 255, 1, 0, 10);

            switch(dmx_data->arg.fx.arg.color_fade.light_board_d)
            {
                case DMX_FX_MODE_CCT:
                    dmx_data->arg.fx.arg.color_fade.arg_d.cct.cct = cct_8bit_calculate(p_profile->arg.color_fade.arg_d.cct.cct, DMX_CCT_MAX, DMX_CCT_MIN);
                    dmx_data->arg.fx.arg.color_fade.arg_d.cct.duv = fx_dmx_gm_8bit_calculate(p_profile->arg.color_fade.arg_d.cct.gm);
                    break;

                case DMX_FX_MODE_HSI:
                    dmx_data->arg.fx.arg.color_fade.arg_d.hsi.cct = fx_cct_8bit_calculate(p_profile->arg.color_fade.arg_d.hsi.cct, DMX_CCT_MAX, DMX_CCT_MIN);
                    dmx_data->arg.fx.arg.color_fade.arg_d.hsi.hue = 1 + (359 * p_profile->arg.color_fade.arg_d.hsi.hue / 255.0f);
                    dmx_data->arg.fx.arg.color_fade.arg_d.hsi.sat = 100 * p_profile->arg.color_fade.arg_d.hsi.sat / 255.0f;
                    break;
            }

            fx_state = p_profile->state < 70 ? DMX_FX_Trigger_Continue : DMX_FX_Trigger_None;

            break;

        case 16:
			dmx_model_extend((uint8_t*)&profile_data[0], sizeof(fx_rgb_8bit_color_cycle_arg_t)+3, DMX_8BIT_TYPE);
            dmx_data->arg.fx.type = DMX_FX_COLOR_CYCLE;
            dmx_data->arg.fx.arg.color_cycle.light_board_a = dmx_to_convert(p_profile->arg.color_cycle.light_board_a, 255, 1, 0, 10);
            dmx_data->arg.fx.arg.color_cycle.frq = dmx_to_convert(p_profile->arg.color_cycle.frq, 255, 10, 1, 10);

            switch(dmx_data->arg.fx.arg.color_cycle.light_board_a)
            {
                case DMX_FX_MODE_CCT:
                    dmx_data->arg.fx.arg.color_cycle.arg_a.cct.cct = cct_8bit_calculate(p_profile->arg.color_cycle.arg_a.cct.cct, DMX_CCT_MAX, DMX_CCT_MIN);
                    dmx_data->arg.fx.arg.color_cycle.arg_a.cct.duv = fx_dmx_gm_8bit_calculate(p_profile->arg.color_cycle.arg_a.cct.gm);
                    break;

                case DMX_FX_MODE_HSI:
                    dmx_data->arg.fx.arg.color_cycle.arg_a.hsi.cct = fx_cct_8bit_calculate(p_profile->arg.color_cycle.arg_a.hsi.cct, DMX_CCT_MAX, DMX_CCT_MIN);
                    dmx_data->arg.fx.arg.color_cycle.arg_a.hsi.hue = 1 + (359 * p_profile->arg.color_cycle.arg_a.hsi.hue / 255.0f);
                    dmx_data->arg.fx.arg.color_cycle.arg_a.hsi.sat = 100 * p_profile->arg.color_cycle.arg_a.hsi.sat / 255.0f;
                    break;


            }

            dmx_data->arg.fx.arg.color_cycle.light_board_b = dmx_to_convert(p_profile->arg.color_cycle.light_board_b, 255, 1, 0, 10);

            switch(dmx_data->arg.fx.arg.color_cycle.light_board_b)
            {
                case DMX_FX_MODE_CCT:
                    dmx_data->arg.fx.arg.color_cycle.arg_b.cct.cct = cct_8bit_calculate(p_profile->arg.color_cycle.arg_b.cct.cct, DMX_CCT_MAX, DMX_CCT_MIN);
                    dmx_data->arg.fx.arg.color_cycle.arg_b.cct.duv = fx_dmx_gm_8bit_calculate(p_profile->arg.color_cycle.arg_b.cct.gm);
                    break;

                case DMX_FX_MODE_HSI:
                    dmx_data->arg.fx.arg.color_cycle.arg_b.hsi.cct = fx_cct_8bit_calculate(p_profile->arg.color_cycle.arg_b.hsi.cct, DMX_CCT_MAX, DMX_CCT_MIN);
                    dmx_data->arg.fx.arg.color_cycle.arg_b.hsi.hue = 1 + (359 * p_profile->arg.color_cycle.arg_b.hsi.hue / 255.0f);
                    dmx_data->arg.fx.arg.color_cycle.arg_b.hsi.sat = 100 * p_profile->arg.color_cycle.arg_b.hsi.sat / 255.0f;
                    break;
            }

            dmx_data->arg.fx.arg.color_cycle.light_board_c = dmx_to_convert(p_profile->arg.color_cycle.light_board_c, 255, 1, 0, 10);

            switch(dmx_data->arg.fx.arg.color_cycle.light_board_c)
            {
                case DMX_FX_MODE_CCT:
                    dmx_data->arg.fx.arg.color_cycle.arg_c.cct.cct = cct_8bit_calculate(p_profile->arg.color_cycle.arg_c.cct.cct, DMX_CCT_MAX, DMX_CCT_MIN);
                    dmx_data->arg.fx.arg.color_cycle.arg_c.cct.duv = fx_dmx_gm_8bit_calculate(p_profile->arg.color_cycle.arg_c.cct.gm);
                    break;

                case DMX_FX_MODE_HSI:
                    dmx_data->arg.fx.arg.color_cycle.arg_c.hsi.cct = fx_cct_8bit_calculate(p_profile->arg.color_cycle.arg_c.hsi.cct, DMX_CCT_MAX, DMX_CCT_MIN);
                    dmx_data->arg.fx.arg.color_cycle.arg_c.hsi.hue = 1 + (359 * p_profile->arg.color_cycle.arg_c.hsi.hue / 255.0f);
                    dmx_data->arg.fx.arg.color_cycle.arg_c.hsi.sat = 100 * p_profile->arg.color_cycle.arg_c.hsi.sat / 255.0f;
                    break;
            }

            dmx_data->arg.fx.arg.color_cycle.light_board_d = dmx_to_convert(p_profile->arg.color_cycle.light_board_d, 255, 1, 0, 10);

            switch(dmx_data->arg.fx.arg.color_cycle.light_board_d)
            {
                case DMX_FX_MODE_CCT:
                    dmx_data->arg.fx.arg.color_cycle.arg_d.cct.cct = cct_8bit_calculate(p_profile->arg.color_cycle.arg_d.cct.cct, DMX_CCT_MAX, DMX_CCT_MIN);
                    dmx_data->arg.fx.arg.color_cycle.arg_d.cct.duv = fx_dmx_gm_8bit_calculate(p_profile->arg.color_cycle.arg_d.cct.gm);
                    break;

                case DMX_FX_MODE_HSI:
                    dmx_data->arg.fx.arg.color_cycle.arg_d.hsi.cct = fx_cct_8bit_calculate(p_profile->arg.color_cycle.arg_d.hsi.cct, DMX_CCT_MAX, DMX_CCT_MIN);
                    dmx_data->arg.fx.arg.color_cycle.arg_d.hsi.hue = 1 + (359 * p_profile->arg.color_cycle.arg_d.hsi.hue / 255.0f);
                    dmx_data->arg.fx.arg.color_cycle.arg_d.hsi.sat = 100 * p_profile->arg.color_cycle.arg_d.hsi.sat / 255.0f;
                    break;
            }

            fx_state = p_profile->state < 70 ? DMX_FX_Trigger_Continue : DMX_FX_Trigger_None;

            break;

        case 17:
			dmx_model_extend((uint8_t*)&profile_data[0], sizeof(fx_rgb_8bit_color_gradient_arg_t)+3, DMX_8BIT_TYPE);
            dmx_data->arg.fx.type = DMX_FX_COLOR_GRADIENT;
            dmx_data->arg.fx.arg.color_gradient.light_board_a = dmx_to_convert(p_profile->arg.color_gradient.light_board_a, 255, 1, 0, 10);
            dmx_data->arg.fx.arg.color_gradient.frq = dmx_to_convert(p_profile->arg.color_gradient.frq, 255, 10, 1, 10);
            dmx_data->arg.fx.arg.color_gradient.min_brightness = p_profile->arg.color_gradient.min_brightness * 750 / 255;

            switch(dmx_data->arg.fx.arg.color_gradient.light_board_a)
            {
                case DMX_FX_MODE_CCT:
                    dmx_data->arg.fx.arg.color_gradient.arg_a.cct.cct = cct_8bit_calculate(p_profile->arg.color_gradient.arg_a.cct.cct, DMX_CCT_MAX, DMX_CCT_MIN);
                    dmx_data->arg.fx.arg.color_gradient.arg_a.cct.duv = fx_dmx_gm_8bit_calculate(p_profile->arg.color_gradient.arg_a.cct.gm);
                    break;

                case DMX_FX_MODE_HSI:
                    dmx_data->arg.fx.arg.color_gradient.arg_a.hsi.cct = fx_cct_8bit_calculate(p_profile->arg.color_gradient.arg_a.hsi.cct, DMX_CCT_MAX, DMX_CCT_MIN);
                    dmx_data->arg.fx.arg.color_gradient.arg_a.hsi.hue = 1 + (359 * p_profile->arg.color_gradient.arg_a.hsi.hue / 255.0f);
                    dmx_data->arg.fx.arg.color_gradient.arg_a.hsi.sat = 100 * p_profile->arg.color_gradient.arg_a.hsi.sat / 255.0f;
                    break;


            }

            dmx_data->arg.fx.arg.color_gradient.light_board_b = dmx_to_convert(p_profile->arg.color_gradient.light_board_b, 255, 1, 0, 10);

            switch(dmx_data->arg.fx.arg.color_gradient.light_board_b)
            {
                case DMX_FX_MODE_CCT:
                    dmx_data->arg.fx.arg.color_gradient.arg_b.cct.cct = cct_8bit_calculate(p_profile->arg.color_gradient.arg_b.cct.cct, DMX_CCT_MAX, DMX_CCT_MIN);
                    dmx_data->arg.fx.arg.color_gradient.arg_b.cct.duv = fx_dmx_gm_8bit_calculate(p_profile->arg.color_gradient.arg_b.cct.gm);
                    break;

                case DMX_FX_MODE_HSI:
                    dmx_data->arg.fx.arg.color_gradient.arg_b.hsi.cct = fx_cct_8bit_calculate(p_profile->arg.color_gradient.arg_b.hsi.cct, DMX_CCT_MAX, DMX_CCT_MIN);
                    dmx_data->arg.fx.arg.color_gradient.arg_b.hsi.hue = 1 + (359 * p_profile->arg.color_gradient.arg_b.hsi.hue / 255.0f);
                    dmx_data->arg.fx.arg.color_gradient.arg_b.hsi.sat = 100 * p_profile->arg.color_gradient.arg_b.hsi.sat / 255.0f;
                    break;
            }

            dmx_data->arg.fx.arg.color_gradient.light_board_c = dmx_to_convert(p_profile->arg.color_gradient.light_board_c, 255, 1, 0, 10);

            switch(dmx_data->arg.fx.arg.color_gradient.light_board_c)
            {
                case DMX_FX_MODE_CCT:
                    dmx_data->arg.fx.arg.color_gradient.arg_c.cct.cct = cct_8bit_calculate(p_profile->arg.color_gradient.arg_c.cct.cct, DMX_CCT_MAX, DMX_CCT_MIN);
                    dmx_data->arg.fx.arg.color_gradient.arg_c.cct.duv = fx_dmx_gm_8bit_calculate(p_profile->arg.color_gradient.arg_c.cct.gm);
                    break;

                case DMX_FX_MODE_HSI:
                    dmx_data->arg.fx.arg.color_gradient.arg_c.hsi.cct = fx_cct_8bit_calculate(p_profile->arg.color_gradient.arg_c.hsi.cct, DMX_CCT_MAX, DMX_CCT_MIN);
                    dmx_data->arg.fx.arg.color_gradient.arg_c.hsi.hue = 1 + (359 * p_profile->arg.color_gradient.arg_c.hsi.hue / 255.0f);
                    dmx_data->arg.fx.arg.color_gradient.arg_c.hsi.sat = 100 * p_profile->arg.color_gradient.arg_c.hsi.sat / 255.0f;
                    break;
            }

            dmx_data->arg.fx.arg.color_gradient.light_board_d = dmx_to_convert(p_profile->arg.color_gradient.light_board_d, 255, 1, 0, 10);

            switch(dmx_data->arg.fx.arg.color_gradient.light_board_d)
            {
                case DMX_FX_MODE_CCT:
                    dmx_data->arg.fx.arg.color_gradient.arg_d.cct.cct = cct_8bit_calculate(p_profile->arg.color_gradient.arg_d.cct.cct, DMX_CCT_MAX, DMX_CCT_MIN);
                    dmx_data->arg.fx.arg.color_gradient.arg_d.cct.duv = fx_dmx_gm_8bit_calculate(p_profile->arg.color_gradient.arg_d.cct.gm);
                    break;

                case DMX_FX_MODE_HSI:
                    dmx_data->arg.fx.arg.color_gradient.arg_d.hsi.cct = fx_cct_8bit_calculate(p_profile->arg.color_gradient.arg_d.hsi.cct, DMX_CCT_MAX, DMX_CCT_MIN);
                    dmx_data->arg.fx.arg.color_gradient.arg_d.hsi.hue = 1 + (359 * p_profile->arg.color_gradient.arg_d.hsi.hue / 255.0f);
                    dmx_data->arg.fx.arg.color_gradient.arg_d.hsi.sat = 100 * p_profile->arg.color_gradient.arg_d.hsi.sat / 255.0f;
                    break;
            }

            fx_state = p_profile->state < 70 ? DMX_FX_Trigger_Continue : DMX_FX_Trigger_None;
            break;

        case 18:
        default:
			dmx_model_extend((uint8_t*)&profile_data[0], sizeof(fx_rgb_8bit_one_color_chase_arg_t)+3, DMX_8BIT_TYPE);
            dmx_data->arg.fx.type = DMX_FX_ONE_COLOR_CHASE;
            dmx_data->arg.fx.arg.one_color_chase.light_board_a = dmx_to_convert(p_profile->arg.one_color_chase.light_board_a, 255, 1, 0, 10);
            dmx_data->arg.fx.arg.one_color_chase.frq = dmx_to_convert(p_profile->arg.one_color_chase.frq, 255, 10, 1, 10);
            dmx_data->arg.fx.arg.one_color_chase.direction = dmx_to_convert(p_profile->arg.one_color_chase.direction, 255, 1, 0, 10);;

            switch(dmx_data->arg.fx.arg.one_color_chase.light_board_a)
            {
                case DMX_FX_MODE_CCT:
                    dmx_data->arg.fx.arg.one_color_chase.arg_a.cct.cct = cct_8bit_calculate(p_profile->arg.one_color_chase.arg_a.cct.cct, DMX_CCT_MAX, DMX_CCT_MIN);
                    dmx_data->arg.fx.arg.one_color_chase.arg_a.cct.duv = fx_dmx_gm_8bit_calculate(p_profile->arg.one_color_chase.arg_a.cct.gm);
                    break;

                case DMX_FX_MODE_HSI:
                    dmx_data->arg.fx.arg.one_color_chase.arg_a.hsi.cct = fx_cct_8bit_calculate(p_profile->arg.one_color_chase.arg_a.hsi.cct, DMX_CCT_MAX, DMX_CCT_MIN);
                    dmx_data->arg.fx.arg.one_color_chase.arg_a.hsi.hue = 1 + (359 * p_profile->arg.one_color_chase.arg_a.hsi.hue / 255.0f);
                    dmx_data->arg.fx.arg.one_color_chase.arg_a.hsi.sat = 100 * p_profile->arg.one_color_chase.arg_a.hsi.sat / 255.0f;
                    break;


            }

            dmx_data->arg.fx.arg.one_color_chase.light_board_b = dmx_to_convert(p_profile->arg.color_gradient.light_board_b, 255, 1, 0, 10);

            switch(dmx_data->arg.fx.arg.one_color_chase.light_board_b)
            {
                case DMX_FX_MODE_CCT:
                    dmx_data->arg.fx.arg.one_color_chase.arg_b.cct.cct = cct_8bit_calculate(p_profile->arg.color_gradient.arg_b.cct.cct, DMX_CCT_MAX, DMX_CCT_MIN);
                    dmx_data->arg.fx.arg.one_color_chase.arg_b.cct.duv = fx_dmx_gm_8bit_calculate(p_profile->arg.color_gradient.arg_b.cct.gm);
                    break;

                case DMX_FX_MODE_HSI:
                    dmx_data->arg.fx.arg.one_color_chase.arg_b.hsi.cct = fx_cct_8bit_calculate(p_profile->arg.color_gradient.arg_b.hsi.cct, DMX_CCT_MAX, DMX_CCT_MIN);
                    dmx_data->arg.fx.arg.one_color_chase.arg_b.hsi.hue = 1 + (359 * p_profile->arg.color_gradient.arg_b.hsi.hue / 255.0f);
                    dmx_data->arg.fx.arg.one_color_chase.arg_b.hsi.sat = 100 * p_profile->arg.color_gradient.arg_b.hsi.sat / 255.0f;
                    break;
            }

            fx_state = p_profile->state < 70 ? DMX_FX_Trigger_Continue : DMX_FX_Trigger_None;
            break;

    }

    return fx_state;
}
uint8_t profile_0708_fx_rgb_16bit_analysis(const uint8_t* profile_data, dmx_data_t* dmx_data)
{
    if(NULL == profile_data || NULL == dmx_data)
        return 0;

    const profile_0708_fx_rgb_16bit_arg_t* p_profile = (profile_0708_fx_rgb_16bit_arg_t*)&profile_data[0];
    uint8_t fx_state;
    dmx_data->lightness = endian_Swap_16bit(profile_data) * 1000 / 65535 ;
    dmx_data->type = DMX_TYPE_FX;
//    dmx_model_extend((uint8_t*)&profile_data[0], sizeof(profile_0708_fx_rgb_16bit_arg_t), DMX_16BIT_TYPE);

    switch(p_profile->type / 10)
    {
        case 0:
			dmx_model_extend((uint8_t*)&profile_data[0], sizeof(fx_rgb_16bit_club_lights_arg_t)+4, DMX_16BIT_TYPE);
            dmx_data->arg.fx.type = DMX_FX_CLUB_LIGHTS;
            dmx_data->arg.fx.arg.clue_lights.color = dmx_to_convert(p_profile->arg.club_lights.color, 255, 7, 0, 10);
            dmx_data->arg.fx.arg.clue_lights.frq = dmx_to_convert(p_profile->arg.club_lights.spd, 255, 10, 1, 10);
            fx_state = p_profile->state < 70 ? DMX_FX_Trigger_Continue : DMX_FX_Trigger_None;
            break;

        case 1:
			dmx_model_extend((uint8_t*)&profile_data[0], sizeof(fx_rgb_16bit_paparazzi_arg_t)+4, DMX_16BIT_TYPE);
            dmx_data->arg.fx.type = DMX_FX_PAPARAZZI;
            dmx_data->arg.fx.arg.paparazzi.cct = cct_16bit_calculate(endian_Swap_16bit((uint8_t*)&p_profile->arg.paparazzi.cct), DMX_CCT_MAX, DMX_CCT_MIN);
            dmx_data->arg.fx.arg.paparazzi.frq = dmx_to_convert(p_profile->arg.paparazzi.frq, 255, 10, 1, 10);
            dmx_data->arg.fx.arg.paparazzi.duv = fx_dmx_gm_16bit_calculate(endian_Swap_16bit((uint8_t*)&p_profile->arg.paparazzi.gm));
            fx_state = p_profile->state < 70 ? DMX_FX_Trigger_Continue : DMX_FX_Trigger_None;
            break;

        case 2:
			dmx_model_extend((uint8_t*)&profile_data[0], sizeof(fx_rgb_16bit_lightning_arg_t)+4, DMX_16BIT_TYPE);
            dmx_data->arg.fx.type = DMX_FX_LIGHTNING;
            dmx_data->arg.fx.arg.lightning.cct = cct_16bit_calculate(endian_Swap_16bit((uint8_t*)&p_profile->arg.lightning.cct), DMX_CCT_MAX, DMX_CCT_MIN);
            dmx_data->arg.fx.arg.lightning.duv = fx_dmx_gm_16bit_calculate(endian_Swap_16bit((uint8_t*)&p_profile->arg.lightning.gm));
            dmx_data->arg.fx.arg.lightning.frq = dmx_to_convert(p_profile->arg.lightning.frq, 255, 11, 1, 10);
            dmx_data->arg.fx.arg.lightning.spd = dmx_to_convert(p_profile->arg.lightning.spd, 255, 10, 1, 10);
            fx_state = p_profile->state > 69 ? DMX_FX_Trigger_None : ((p_profile->state > 9) ? DMX_FX_Trigger_Once : DMX_FX_Trigger_Continue);
            break;

        case 3:
			dmx_model_extend((uint8_t*)&profile_data[0], sizeof(fx_rgb_16bit_tv_arg_t)+4, DMX_16BIT_TYPE);
            dmx_data->arg.fx.type = DMX_FX_TV;
            dmx_data->arg.fx.arg.tv.cct_range = dmx_to_convert(p_profile->arg.tv.cct_rang, 255, 2, 0, 10);
            dmx_data->arg.fx.arg.tv.frq = dmx_to_convert(p_profile->arg.tv.spd, 255, 11, 1, 10);
            fx_state = p_profile->state < 70 ? DMX_FX_Trigger_Continue : DMX_FX_Trigger_None;
            break;

        case 4:
			dmx_model_extend((uint8_t*)&profile_data[0], sizeof(fx_rgb_16bit_candle_arg_t)+4, DMX_16BIT_TYPE);
            dmx_data->arg.fx.type = DMX_FX_CANDLE;
            dmx_data->arg.fx.arg.candle.cct_rang = dmx_to_convert(p_profile->arg.candle.cct_range, 255, 2, 0, 10);
            dmx_data->arg.fx.arg.candle.frq = dmx_to_convert(p_profile->arg.candle.spd, 255, 10, 1, 10);
            fx_state = p_profile->state < 70 ? DMX_FX_Trigger_Continue : DMX_FX_Trigger_None;
            break;

        case 5:
			dmx_model_extend((uint8_t*)&profile_data[0], sizeof(fx_rgb_16bit_fire_arg_t)+4, DMX_16BIT_TYPE);
            dmx_data->arg.fx.type = DMX_FX_FIRE;
            dmx_data->arg.fx.arg.fire.cct_range = dmx_to_convert(p_profile->arg.fire.cct_rang, 255, 2, 0, 10);
            dmx_data->arg.fx.arg.fire.frq = dmx_to_convert(p_profile->arg.fire.spd, 255, 10, 1, 10);
            fx_state = p_profile->state < 70 ? DMX_FX_Trigger_Continue : DMX_FX_Trigger_None;
            break;

        case 6:
			dmx_model_extend((uint8_t*)&profile_data[0], sizeof(fx_rgb_16bit_strobe_arg_t)+4, DMX_16BIT_TYPE);
            dmx_data->arg.fx.type = DMX_FX_STROBE;
            dmx_data->arg.fx.arg.strobe.mode = (enum dmx_fx_mode)dmx_to_convert(p_profile->arg.strobe.mode, 255, 5, 0, 10);

            switch(dmx_data->arg.fx.arg.strobe.mode)
            {
                case DMX_FX_MODE_CCT:
                    dmx_data->arg.fx.arg.strobe.mode_arg.cct.cct = cct_16bit_calculate(endian_Swap_16bit((uint8_t*)&p_profile->arg.strobe.arg.cct.cct), DMX_CCT_MAX, DMX_CCT_MIN);
                    dmx_data->arg.fx.arg.strobe.mode_arg.cct.duv = fx_dmx_gm_16bit_calculate(endian_Swap_16bit((uint8_t*)&p_profile->arg.strobe.arg.cct.gm));
                    dmx_data->arg.fx.arg.strobe.spd = dmx_to_convert(p_profile->arg.strobe.arg.cct.spd, 255, 11, 1, 10);
                    break;

                case DMX_FX_MODE_HSI:
                    dmx_data->arg.fx.arg.strobe.mode_arg.hsi.hue = 1 + (359 * endian_Swap_16bit((uint8_t*)&p_profile->arg.strobe.arg.hsi.hue) / 65535.0f);
                    dmx_data->arg.fx.arg.strobe.mode_arg.hsi.sat = 100 * endian_Swap_16bit((uint8_t*)&p_profile->arg.strobe.arg.hsi.sat) / 65535.0f;
                    dmx_data->arg.fx.arg.strobe.mode_arg.hsi.cct = fx_cct_16bit_calculate(endian_Swap_16bit((uint8_t*)&p_profile->arg.strobe.arg.hsi.cct), DMX_CCT_MAX, DMX_CCT_MIN);
                    dmx_data->arg.fx.arg.strobe.spd = dmx_to_convert(p_profile->arg.strobe.arg.hsi.spd, 255, 11, 1, 10);
                    break;

                case DMX_FX_MODE_GEL:

                    dmx_data->arg.fx.arg.strobe.mode_arg.gel.cct   = cct_8bit_calculate(p_profile->arg.strobe.arg.gel.cct, DMX_CCT_MAX, DMX_CCT_MIN);
                    dmx_data->arg.fx.arg.strobe.mode_arg.gel.brand = p_profile->arg.strobe.arg.gel.brand > 29 ? 1 : 0;
                    dmx_data->arg.fx.arg.strobe.mode_arg.gel.type  = gel_series_calculate_8bit(p_profile->arg.strobe.arg.gel.type, dmx_data->arg.fx.arg.strobe.mode_arg.gel.brand);
                    dmx_data->arg.fx.arg.strobe.mode_arg.gel.color = gel_color_calculate_8bit(p_profile->arg.strobe.arg.gel.color, dmx_data->arg.fx.arg.strobe.mode_arg.gel.brand, dmx_data->arg.fx.arg.strobe.mode_arg.gel.type);
                    dmx_data->arg.fx.arg.strobe.spd = dmx_to_convert(p_profile->arg.strobe.arg.gel.spd, 255, 11, 1, 10);
                    break;

                case DMX_FX_MODE_XY:
                    dmx_data->arg.fx.arg.strobe.mode_arg.xy.x = 8000 * endian_Swap_16bit((uint8_t*)&p_profile->arg.strobe.arg.xy.x) / 65535.0f;
                    dmx_data->arg.fx.arg.strobe.mode_arg.xy.y = 8000 * endian_Swap_16bit((uint8_t*)&p_profile->arg.strobe.arg.xy.y) / 65535.0f;
                    dmx_data->arg.fx.arg.strobe.spd = dmx_to_convert(p_profile->arg.strobe.arg.xy.spd, 255, 11, 1, 10);
                    break;

                case DMX_FX_MODE_SOUYRCE:
                    dmx_data->arg.fx.arg.strobe.mode_arg.socue.type = dmx_to_convert(p_profile->arg.strobe.arg.source.type, 255, 45, 0, 3);
                    dmx_data->arg.fx.arg.strobe.spd = dmx_to_convert(p_profile->arg.strobe.arg.source.spd, 255, 11, 1, 10);

                    break;

                case DMX_FX_MODE_RGB:
                    dmx_data->arg.fx.arg.strobe.mode_arg.rgb.r_ratio = 1000 * endian_Swap_16bit((uint8_t*)&p_profile->arg.strobe.arg.rgb.r) / 65535.0f;
                    dmx_data->arg.fx.arg.strobe.mode_arg.rgb.g_ratio = 1000 * endian_Swap_16bit((uint8_t*)&p_profile->arg.strobe.arg.rgb.g) / 65535.0f;
                    dmx_data->arg.fx.arg.strobe.mode_arg.rgb.b_ratio = 1000 * endian_Swap_16bit((uint8_t*)&p_profile->arg.strobe.arg.rgb.b) / 65535.0f;
                    dmx_data->arg.fx.arg.strobe.spd = dmx_to_convert(p_profile->arg.strobe.arg.rgb.spd, 255, 11, 1, 10);
                    break;

                default:
                    break;
            }

            fx_state = p_profile->state < 70 ? DMX_FX_Trigger_Once : DMX_FX_Trigger_None;
            break;

        case 7:
			dmx_model_extend((uint8_t*)&profile_data[0], sizeof(fx_rgb_16bit_explosion_arg_t)+4, DMX_16BIT_TYPE);
            dmx_data->arg.fx.type = DMX_FX_EXPLOSION;
            dmx_data->arg.fx.arg.fault_bulb.mode = (enum dmx_fx_mode)dmx_to_convert(p_profile->arg.explosion.mode, 255, 5, 0, 10);

            switch(dmx_data->arg.fx.arg.explosion.mode)
            {
                case DMX_FX_MODE_CCT:
                    dmx_data->arg.fx.arg.explosion.mode_arg.cct.cct = cct_16bit_calculate(endian_Swap_16bit((uint8_t*)&p_profile->arg.explosion.arg.cct.cct), DMX_CCT_MAX, DMX_CCT_MIN);
                    dmx_data->arg.fx.arg.explosion.mode_arg.cct.duv = fx_dmx_gm_16bit_calculate(endian_Swap_16bit((uint8_t*)&p_profile->arg.explosion.arg.cct.gm));

                    dmx_data->arg.fx.arg.explosion.decay = dmx_to_convert(p_profile->arg.explosion.arg.cct.decay, 255, 10, 1, 10);
                    break;

                case DMX_FX_MODE_HSI:
                    dmx_data->arg.fx.arg.explosion.mode_arg.hsi.hue = 1 + (359 * endian_Swap_16bit((uint8_t*)&p_profile->arg.explosion.arg.hsi.hue) / 65535.0f);
                    dmx_data->arg.fx.arg.explosion.mode_arg.hsi.sat = 100 * endian_Swap_16bit((uint8_t*)&p_profile->arg.explosion.arg.hsi.sat) / 65535.0f;
                    dmx_data->arg.fx.arg.explosion.mode_arg.hsi.cct = fx_cct_16bit_calculate(p_profile->arg.explosion.arg.hsi.cct, DMX_CCT_MAX, DMX_CCT_MIN);
                    dmx_data->arg.fx.arg.explosion.decay = dmx_to_convert(p_profile->arg.explosion.arg.hsi.decay, 255, 10, 1, 10);
                    break;

                case DMX_FX_MODE_GEL:

                    dmx_data->arg.fx.arg.explosion.mode_arg.gel.cct   = cct_8bit_calculate(p_profile->arg.explosion.arg.gel.cct, DMX_CCT_MAX, DMX_CCT_MIN);
                    dmx_data->arg.fx.arg.explosion.mode_arg.gel.brand = p_profile->arg.explosion.arg.gel.brand > 29 ? 1 : 0;
                    dmx_data->arg.fx.arg.explosion.mode_arg.gel.type  = gel_series_calculate_8bit(p_profile->arg.explosion.arg.gel.type, dmx_data->arg.fx.arg.explosion.mode_arg.gel.brand);
                    dmx_data->arg.fx.arg.explosion.mode_arg.gel.color = gel_color_calculate_8bit(p_profile->arg.explosion.arg.gel.color, dmx_data->arg.fx.arg.explosion.mode_arg.gel.brand, dmx_data->arg.fx.arg.explosion.mode_arg.gel.type);
                    dmx_data->arg.fx.arg.explosion.decay = dmx_to_convert(p_profile->arg.explosion.arg.gel.decay, 255, 10, 1, 10);
                    break;

                case DMX_FX_MODE_XY:
                    dmx_data->arg.fx.arg.explosion.mode_arg.xy.x = 8000 * endian_Swap_16bit((uint8_t*)&p_profile->arg.explosion.arg.xy.x) / 65535.0f;
                    dmx_data->arg.fx.arg.explosion.mode_arg.xy.y = 8000 * endian_Swap_16bit((uint8_t*)&p_profile->arg.explosion.arg.xy.y) / 65535.0f;
                    dmx_data->arg.fx.arg.explosion.decay = dmx_to_convert(p_profile->arg.explosion.arg.xy.decay, 255, 10, 1, 10);
                    break;

                case DMX_FX_MODE_SOUYRCE:
                    dmx_data->arg.fx.arg.explosion.mode_arg.socue.type = dmx_to_convert(p_profile->arg.explosion.arg.source.type, 255, 45, 0, 3);
                    dmx_data->arg.fx.arg.explosion.decay = dmx_to_convert(p_profile->arg.explosion.arg.source.decay, 255, 10, 1, 10);

                    break;

                case DMX_FX_MODE_RGB:
                    dmx_data->arg.fx.arg.explosion.mode_arg.rgb.r_ratio = 1000 * endian_Swap_16bit((uint8_t*)&p_profile->arg.explosion.arg.rgb.r) / 65535.0f;
                    dmx_data->arg.fx.arg.explosion.mode_arg.rgb.g_ratio = 1000 * endian_Swap_16bit((uint8_t*)&p_profile->arg.explosion.arg.rgb.g) / 65535.0f;
                    dmx_data->arg.fx.arg.explosion.mode_arg.rgb.b_ratio = 1000 * endian_Swap_16bit((uint8_t*)&p_profile->arg.explosion.arg.rgb.b) / 65535.0f;
                    dmx_data->arg.fx.arg.explosion.decay = dmx_to_convert(p_profile->arg.explosion.arg.rgb.decay, 255, 10, 1, 10);
                    break;

                default:
                    break;
            }

            fx_state = p_profile->state < 70 ? DMX_FX_Trigger_Continue : DMX_FX_Trigger_None;
            break;

        case 8:
			dmx_model_extend((uint8_t*)&profile_data[0], sizeof(fx_rgb_16bit_fault_bulb_arg_t)+4, DMX_16BIT_TYPE);
            dmx_data->arg.fx.type = DMX_FX_FAULT_BULB;
            dmx_data->arg.fx.arg.pulsing.mode = (enum dmx_fx_mode)dmx_to_convert(p_profile->arg.fault_bulb.mode, 255, 5, 0, 10);

            switch(dmx_data->arg.fx.arg.fault_bulb.mode)
            {
                case DMX_FX_MODE_CCT:
                    dmx_data->arg.fx.arg.fault_bulb.mode_arg.cct.cct = cct_16bit_calculate(endian_Swap_16bit((uint8_t*)&p_profile->arg.fault_bulb.arg.cct.cct), DMX_CCT_MAX, DMX_CCT_MIN);
                    dmx_data->arg.fx.arg.fault_bulb.mode_arg.cct.duv = fx_dmx_gm_16bit_calculate(endian_Swap_16bit((uint8_t*)&p_profile->arg.fault_bulb.arg.cct.gm));
                    dmx_data->arg.fx.arg.fault_bulb.frq = dmx_to_convert(p_profile->arg.fault_bulb.arg.cct.frq, 255, 11, 1, 10);
                    dmx_data->arg.fx.arg.fault_bulb.spd = dmx_to_convert(p_profile->arg.fault_bulb.arg.cct.spd, 255, 10, 1, 10);
                    break;

                case DMX_FX_MODE_HSI:
                    dmx_data->arg.fx.arg.fault_bulb.mode_arg.hsi.hue = 1 + (359 * endian_Swap_16bit((uint8_t*)&p_profile->arg.fault_bulb.arg.hsi.hue) / 65535.0f);
                    dmx_data->arg.fx.arg.fault_bulb.mode_arg.hsi.sat = 100 * endian_Swap_16bit((uint8_t*)&p_profile->arg.fault_bulb.arg.hsi.sat) / 65535.0f;
                    dmx_data->arg.fx.arg.fault_bulb.mode_arg.hsi.cct = fx_cct_16bit_calculate(p_profile->arg.fault_bulb.arg.hsi.cct, DMX_CCT_MAX, DMX_CCT_MIN);
                    dmx_data->arg.fx.arg.fault_bulb.frq = dmx_to_convert(p_profile->arg.fault_bulb.arg.hsi.frq, 255, 11, 1, 10);
                    dmx_data->arg.fx.arg.fault_bulb.spd = dmx_to_convert(p_profile->arg.fault_bulb.arg.hsi.spd, 255, 10, 1, 10);
                    break;

                case DMX_FX_MODE_GEL:

                    dmx_data->arg.fx.arg.fault_bulb.mode_arg.gel.cct   = cct_8bit_calculate(p_profile->arg.fault_bulb.arg.gel.cct, DMX_CCT_MAX, DMX_CCT_MIN);
                    dmx_data->arg.fx.arg.fault_bulb.mode_arg.gel.brand = p_profile->arg.fault_bulb.arg.gel.brand > 29 ? 1 : 0;
                    dmx_data->arg.fx.arg.fault_bulb.mode_arg.gel.type  = gel_series_calculate_8bit(p_profile->arg.fault_bulb.arg.gel.type, dmx_data->arg.fx.arg.fault_bulb.mode_arg.gel.brand);
                    dmx_data->arg.fx.arg.fault_bulb.mode_arg.gel.color = gel_color_calculate_8bit(p_profile->arg.fault_bulb.arg.gel.color, dmx_data->arg.fx.arg.fault_bulb.mode_arg.gel.brand, dmx_data->arg.fx.arg.fault_bulb.mode_arg.gel.type);
                    dmx_data->arg.fx.arg.fault_bulb.frq = dmx_to_convert(p_profile->arg.fault_bulb.arg.gel.frq, 255, 11, 1, 10);
                    dmx_data->arg.fx.arg.fault_bulb.spd = dmx_to_convert(p_profile->arg.fault_bulb.arg.gel.spd, 255, 10, 1, 10);
                    break;

                case DMX_FX_MODE_XY:
                    dmx_data->arg.fx.arg.fault_bulb.mode_arg.xy.x = 8000 * endian_Swap_16bit((uint8_t*)&p_profile->arg.fault_bulb.arg.xy.x) / 65535.0f;
                    dmx_data->arg.fx.arg.fault_bulb.mode_arg.xy.y = 8000 * endian_Swap_16bit((uint8_t*)&p_profile->arg.fault_bulb.arg.xy.y) / 65535.0f;
                    dmx_data->arg.fx.arg.fault_bulb.frq = dmx_to_convert(p_profile->arg.fault_bulb.arg.xy.frq, 255, 11, 1, 10);
                    dmx_data->arg.fx.arg.fault_bulb.spd = dmx_to_convert(p_profile->arg.fault_bulb.arg.xy.spd, 255, 10, 1, 10);
                    break;

                case DMX_FX_MODE_SOUYRCE:
                    dmx_data->arg.fx.arg.fault_bulb.mode_arg.socue.type = dmx_to_convert(p_profile->arg.fault_bulb.arg.source.type, 255, 45, 0, 3);

                    dmx_data->arg.fx.arg.fault_bulb.frq = dmx_to_convert(p_profile->arg.fault_bulb.arg.source.frq, 255, 11, 1, 10);
                    dmx_data->arg.fx.arg.fault_bulb.spd = dmx_to_convert(p_profile->arg.fault_bulb.arg.source.spd, 255, 10, 1, 10);
                    break;

                case DMX_FX_MODE_RGB:
                    dmx_data->arg.fx.arg.fault_bulb.mode_arg.rgb.r_ratio = 1000 * endian_Swap_16bit((uint8_t*)&p_profile->arg.fault_bulb.arg.rgb.r) / 65535.0f;
                    dmx_data->arg.fx.arg.fault_bulb.mode_arg.rgb.g_ratio = 1000 * endian_Swap_16bit((uint8_t*)&p_profile->arg.fault_bulb.arg.rgb.g) / 65535.0f;
                    dmx_data->arg.fx.arg.fault_bulb.mode_arg.rgb.b_ratio = 1000 * endian_Swap_16bit((uint8_t*)&p_profile->arg.fault_bulb.arg.rgb.b) / 65535.0f;
                    dmx_data->arg.fx.arg.fault_bulb.frq = dmx_to_convert(p_profile->arg.fault_bulb.arg.rgb.frq, 255, 11, 1, 10);
                    dmx_data->arg.fx.arg.fault_bulb.spd = dmx_to_convert(p_profile->arg.fault_bulb.arg.rgb.spd, 255, 10, 1, 10);
                    break;

                default:
                    break;
            }

            fx_state = p_profile->state < 70 ? DMX_FX_Trigger_Continue : DMX_FX_Trigger_None;
            break;

        case 9:
			dmx_model_extend((uint8_t*)&profile_data[0], sizeof(fx_rgb_16bit_pulsing_arg_t)+4, DMX_16BIT_TYPE);
            dmx_data->arg.fx.type = DMX_FX_PULSING;
            dmx_data->arg.fx.arg.welding.mode = (enum dmx_fx_mode)dmx_to_convert(p_profile->arg.pulsing.mode, 255, 5, 0, 10);

            switch(dmx_data->arg.fx.arg.pulsing.mode)
            {
                case DMX_FX_MODE_CCT:
                    dmx_data->arg.fx.arg.pulsing.mode_arg.cct.cct = cct_16bit_calculate(endian_Swap_16bit((uint8_t*)&p_profile->arg.pulsing.arg.cct.cct), DMX_CCT_MAX, DMX_CCT_MIN);
                    dmx_data->arg.fx.arg.pulsing.mode_arg.cct.duv = fx_dmx_gm_16bit_calculate(endian_Swap_16bit((uint8_t*)&p_profile->arg.pulsing.arg.cct.gm));
                    dmx_data->arg.fx.arg.pulsing.frq = dmx_to_convert(p_profile->arg.pulsing.arg.cct.frq, 255, 10, 1, 11);
                    dmx_data->arg.fx.arg.pulsing.spd = dmx_to_convert(p_profile->arg.pulsing.arg.cct.spd, 255, 10, 1, 10);

                    break;

                case DMX_FX_MODE_HSI:
                    dmx_data->arg.fx.arg.pulsing.mode_arg.hsi.hue = 1 + (359 * endian_Swap_16bit((uint8_t*)&p_profile->arg.pulsing.arg.hsi.hue) / 65535.0f);
                    dmx_data->arg.fx.arg.pulsing.mode_arg.hsi.sat = 100 * endian_Swap_16bit((uint8_t*)&p_profile->arg.pulsing.arg.hsi.sat) / 65535.0f;
                    dmx_data->arg.fx.arg.pulsing.mode_arg.hsi.cct = fx_cct_16bit_calculate(p_profile->arg.pulsing.arg.hsi.cct, DMX_CCT_MAX, DMX_CCT_MIN);
                    dmx_data->arg.fx.arg.pulsing.frq = dmx_to_convert(p_profile->arg.pulsing.arg.hsi.frq, 255, 10, 1, 11);
                    dmx_data->arg.fx.arg.pulsing.spd = dmx_to_convert(p_profile->arg.pulsing.arg.hsi.spd, 255, 10, 1, 10);
                    break;

                case DMX_FX_MODE_GEL:

                    dmx_data->arg.fx.arg.pulsing.mode_arg.gel.cct   = cct_8bit_calculate(p_profile->arg.pulsing.arg.gel.cct, DMX_CCT_MAX, DMX_CCT_MIN);
                    dmx_data->arg.fx.arg.pulsing.mode_arg.gel.brand = p_profile->arg.pulsing.arg.gel.brand > 29 ? 1 : 0;
                    dmx_data->arg.fx.arg.pulsing.mode_arg.gel.type  = gel_series_calculate_8bit(p_profile->arg.pulsing.arg.gel.type, dmx_data->arg.fx.arg.pulsing.mode_arg.gel.brand);
                    dmx_data->arg.fx.arg.pulsing.mode_arg.gel.color = gel_color_calculate_8bit(p_profile->arg.pulsing.arg.gel.color, dmx_data->arg.fx.arg.pulsing.mode_arg.gel.brand, dmx_data->arg.fx.arg.pulsing.mode_arg.gel.type);
                    dmx_data->arg.fx.arg.pulsing.frq = dmx_to_convert(p_profile->arg.pulsing.arg.gel.frq, 255, 10, 1, 11);
                    dmx_data->arg.fx.arg.pulsing.spd = dmx_to_convert(p_profile->arg.pulsing.arg.gel.spd, 255, 10, 1, 10);
                    break;

                case DMX_FX_MODE_XY:
                    dmx_data->arg.fx.arg.pulsing.mode_arg.xy.x = 8000 * endian_Swap_16bit((uint8_t*)&p_profile->arg.pulsing.arg.xy.x) / 65535.0f;
                    dmx_data->arg.fx.arg.pulsing.mode_arg.xy.y = 8000 * endian_Swap_16bit((uint8_t*)&p_profile->arg.pulsing.arg.xy.y) / 65535.0f;
                    dmx_data->arg.fx.arg.pulsing.frq = dmx_to_convert(p_profile->arg.pulsing.arg.xy.frq, 255, 10, 1, 11);
                    dmx_data->arg.fx.arg.pulsing.spd = dmx_to_convert(p_profile->arg.pulsing.arg.xy.spd, 255, 10, 1, 10);
                    break;

                case DMX_FX_MODE_SOUYRCE:
                    dmx_data->arg.fx.arg.pulsing.mode_arg.socue.type = dmx_to_convert(p_profile->arg.pulsing.arg.source.type, 255, 45, 0, 3);
                    dmx_data->arg.fx.arg.pulsing.frq = dmx_to_convert(p_profile->arg.pulsing.arg.source.frq, 255, 10, 1, 11);
                    dmx_data->arg.fx.arg.pulsing.spd = dmx_to_convert(p_profile->arg.pulsing.arg.source.spd, 255, 10, 1, 10);

                    break;

                case DMX_FX_MODE_RGB:
                    dmx_data->arg.fx.arg.pulsing.mode_arg.rgb.r_ratio = 1000 * endian_Swap_16bit((uint8_t*)&p_profile->arg.pulsing.arg.rgb.r) / 65535.0f;
                    dmx_data->arg.fx.arg.pulsing.mode_arg.rgb.g_ratio = 1000 * endian_Swap_16bit((uint8_t*)&p_profile->arg.pulsing.arg.rgb.g) / 65535.0f;
                    dmx_data->arg.fx.arg.pulsing.mode_arg.rgb.b_ratio = 1000 * endian_Swap_16bit((uint8_t*)&p_profile->arg.pulsing.arg.rgb.b) / 65535.0f;
                    dmx_data->arg.fx.arg.pulsing.frq = dmx_to_convert(p_profile->arg.pulsing.arg.rgb.frq, 255, 10, 1, 11);
                    dmx_data->arg.fx.arg.pulsing.spd = dmx_to_convert(p_profile->arg.pulsing.arg.rgb.spd, 255, 10, 1, 10);
                    break;

                default:
                    break;
            }

            fx_state = p_profile->state < 70 ? DMX_FX_Trigger_Continue : DMX_FX_Trigger_None;
            break;

        case 10:
			dmx_model_extend((uint8_t*)&profile_data[0], sizeof(fx_rgb_16bit_welding_arg_t)+4, DMX_16BIT_TYPE);
            dmx_data->arg.fx.type = DMX_FX_WELDING;
            dmx_data->arg.fx.arg.welding.mode = (enum dmx_fx_mode)dmx_to_convert(p_profile->arg.welding.mode, 255, 5, 0, 10);
            dmx_data->arg.fx.arg.welding.min_lightness = p_profile->arg.welding.arg.cct.min_brightness * 1000 / 255;

            switch(dmx_data->arg.fx.arg.welding.mode)
            {
                case DMX_FX_MODE_CCT:
                    dmx_data->arg.fx.arg.welding.mode_arg.cct.cct = cct_16bit_calculate(endian_Swap_16bit((uint8_t*)&p_profile->arg.welding.arg.cct.cct), DMX_CCT_MAX, DMX_CCT_MIN);
                    dmx_data->arg.fx.arg.welding.mode_arg.cct.duv = fx_dmx_gm_16bit_calculate(endian_Swap_16bit((uint8_t*)&p_profile->arg.welding.arg.cct.gm));
                    dmx_data->arg.fx.arg.welding.frq = dmx_to_convert(p_profile->arg.welding.arg.cct.frq, 255, 11, 1, 10);
                    dmx_data->arg.fx.arg.welding.min_lightness = p_profile->arg.welding.arg.cct.min_brightness * 750 / 255;

                    break;

                case DMX_FX_MODE_HSI:
                    dmx_data->arg.fx.arg.welding.mode_arg.hsi.hue = 1 + (359 * endian_Swap_16bit((uint8_t*)&p_profile->arg.welding.arg.hsi.hue) / 65535.0f);
                    dmx_data->arg.fx.arg.welding.mode_arg.hsi.sat = 100 * endian_Swap_16bit((uint8_t*)&p_profile->arg.welding.arg.hsi.sat) / 65535.0f;
                    dmx_data->arg.fx.arg.welding.mode_arg.hsi.cct = fx_cct_16bit_calculate(p_profile->arg.welding.arg.hsi.cct, DMX_CCT_MAX, DMX_CCT_MIN);
                    dmx_data->arg.fx.arg.welding.frq = dmx_to_convert(p_profile->arg.welding.arg.hsi.frq, 255, 11, 1, 10);
                    dmx_data->arg.fx.arg.welding.min_lightness = p_profile->arg.welding.arg.hsi.min_brightness * 750 / 255;
                    break;

                case DMX_FX_MODE_GEL:

                    dmx_data->arg.fx.arg.welding.mode_arg.gel.cct   = cct_8bit_calculate(p_profile->arg.welding.arg.gel.cct, DMX_CCT_MAX, DMX_CCT_MIN);
                    dmx_data->arg.fx.arg.welding.mode_arg.gel.brand = p_profile->arg.welding.arg.gel.brand > 29 ? 1 : 0;
                    dmx_data->arg.fx.arg.welding.mode_arg.gel.type  = gel_series_calculate_8bit(p_profile->arg.welding.arg.gel.type, dmx_data->arg.fx.arg.welding.mode_arg.gel.brand);
                    dmx_data->arg.fx.arg.welding.mode_arg.gel.color = gel_color_calculate_8bit(p_profile->arg.welding.arg.gel.color, dmx_data->arg.fx.arg.welding.mode_arg.gel.brand, dmx_data->arg.fx.arg.welding.mode_arg.gel.type);
                    dmx_data->arg.fx.arg.welding.frq = dmx_to_convert(p_profile->arg.welding.arg.gel.frq, 255, 10, 1, 10);
                    dmx_data->arg.fx.arg.welding.min_lightness = p_profile->arg.welding.arg.gel.min_brightness * 750 / 255;
                    break;

                case DMX_FX_MODE_XY:
                    dmx_data->arg.fx.arg.welding.mode_arg.xy.x = 8000 * endian_Swap_16bit((uint8_t*)&p_profile->arg.welding.arg.xy.x) / 65535.0f;
                    dmx_data->arg.fx.arg.welding.mode_arg.xy.y = 8000 * endian_Swap_16bit((uint8_t*)&p_profile->arg.welding.arg.xy.y) / 65535.0f;
                    dmx_data->arg.fx.arg.welding.frq = dmx_to_convert(p_profile->arg.welding.arg.xy.frq, 255, 11, 1, 10);
                    dmx_data->arg.fx.arg.welding.min_lightness = p_profile->arg.welding.arg.cct.min_brightness * 750 / 255;
                    break;

                case DMX_FX_MODE_SOUYRCE:
                    dmx_data->arg.fx.arg.welding.mode_arg.socue.type = dmx_to_convert(p_profile->arg.welding.arg.source.type, 255, 45, 0, 3);
                    dmx_data->arg.fx.arg.welding.frq = dmx_to_convert(p_profile->arg.welding.arg.source.frq, 255, 11, 1, 10);
                    dmx_data->arg.fx.arg.welding.min_lightness = p_profile->arg.welding.arg.source.min_brightness * 750 / 255;

                    break;

                case DMX_FX_MODE_RGB:
                    dmx_data->arg.fx.arg.welding.mode_arg.rgb.r_ratio = 1000 * endian_Swap_16bit((uint8_t*)&p_profile->arg.welding.arg.rgb.r) / 65535.0f;
                    dmx_data->arg.fx.arg.welding.mode_arg.rgb.g_ratio = 1000 * endian_Swap_16bit((uint8_t*)&p_profile->arg.welding.arg.rgb.g) / 65535.0f;
                    dmx_data->arg.fx.arg.welding.mode_arg.rgb.b_ratio = 1000 * endian_Swap_16bit((uint8_t*)&p_profile->arg.welding.arg.rgb.b) / 65535.0f;
                    dmx_data->arg.fx.arg.welding.frq = dmx_to_convert(p_profile->arg.welding.arg.rgb.frq, 255, 11, 1, 10);
                    dmx_data->arg.fx.arg.welding.min_lightness = p_profile->arg.welding.arg.rgb.min_brightness * 750 / 255;
                    break;

                default:
                    break;
            }

            fx_state = p_profile->state < 70 ? DMX_FX_Trigger_Continue : DMX_FX_Trigger_None;
            break;

        case 11:
			dmx_model_extend((uint8_t*)&profile_data[0], sizeof(fx_rgb_16bit_cop_car_arg_t)+4, DMX_16BIT_TYPE);
            dmx_data->arg.fx.type = DMX_FX_COP_CAR;
            dmx_data->arg.fx.arg.copcar.color = dmx_to_convert(p_profile->arg.copcar.color, 255, 4, 0, 10);
            dmx_data->arg.fx.arg.copcar.frq = dmx_to_convert(p_profile->arg.copcar.frq, 255, 10, 1, 10);
            fx_state = p_profile->state < 70 ? DMX_FX_Trigger_Continue : DMX_FX_Trigger_None;
            break;

        case 12:
			dmx_model_extend((uint8_t*)&profile_data[0], sizeof(fx_rgb_16bit_color_chase_arg_t)+4, DMX_16BIT_TYPE);
            dmx_data->arg.fx.type = DMX_FX_COLOR_CHASE;
            dmx_data->arg.fx.arg.color_chase.sat = 100 * p_profile->arg.color_chase.sat / 255.0f;
            dmx_data->arg.fx.arg.color_chase.spd = dmx_to_convert(p_profile->arg.color_chase.spd, 255, 10, 1, 10);
            fx_state = p_profile->state < 70 ? DMX_FX_Trigger_Continue : DMX_FX_Trigger_None;
            break;

        case 13:
			dmx_model_extend((uint8_t*)&profile_data[0], sizeof(fx_rgb_16bit_party_lights_arg_t)+4, DMX_16BIT_TYPE);
            dmx_data->arg.fx.type = DMX_FX_PARTY_LIGHTS;
            dmx_data->arg.fx.arg.party_lights.sat = 100 * p_profile->arg.party_lights.sat / 255.0f;
            dmx_data->arg.fx.arg.party_lights.frq = dmx_to_convert(p_profile->arg.party_lights.spd, 255, 10, 1, 10);
            fx_state = p_profile->state < 70 ? DMX_FX_Trigger_Continue : DMX_FX_Trigger_None;
            break;

        case 14:
			dmx_model_extend((uint8_t*)&profile_data[0], sizeof(fx_rgb_16bit_fireworks_arg_t)+4, DMX_16BIT_TYPE);
            dmx_data->arg.fx.type = DMX_FX_FIREWORKS;

            dmx_data->arg.fx.arg.fireworks.frq = dmx_to_convert(p_profile->arg.fireworks.frq, 255, 11, 1, 10);
            dmx_data->arg.fx.arg.fireworks.type = dmx_to_convert(p_profile->arg.fireworks.color, 255, 2, 0, 10);
            fx_state = p_profile->state < 70 ? DMX_FX_Trigger_Continue : DMX_FX_Trigger_None;
            break;

        case 15:
			dmx_model_extend((uint8_t*)&profile_data[0], sizeof(fx_rgb_16bit_color_fade_arg_t)+4, DMX_16BIT_TYPE);
            dmx_data->arg.fx.type = DMX_FX_COLOR_FADE;
            dmx_data->arg.fx.arg.color_cycle.light_board_a = dmx_to_convert(p_profile->arg.color_fade.light_board_a, 255, 1, 0, 10);
            dmx_data->arg.fx.arg.color_cycle.frq = dmx_to_convert(p_profile->arg.color_fade.frq, 255, 10, 1, 10);
            dmx_data->arg.fx.arg.color_fade.direction = dmx_to_convert(p_profile->arg.color_fade.direction, 255, 2, 0, 10);

            switch(dmx_data->arg.fx.arg.color_fade.light_board_a)
            {
                case DMX_FX_MODE_CCT:
                    dmx_data->arg.fx.arg.color_fade.arg_a.cct.cct = cct_16bit_calculate(endian_Swap_16bit((uint8_t*)&p_profile->arg.color_fade.arg_a.cct.cct), DMX_CCT_MAX, DMX_CCT_MIN);
                    dmx_data->arg.fx.arg.color_fade.arg_a.cct.duv = fx_dmx_gm_16bit_calculate(endian_Swap_16bit((uint8_t*)&p_profile->arg.color_fade.arg_a.cct.gm));
                    break;

                case DMX_FX_MODE_HSI:
                    dmx_data->arg.fx.arg.color_fade.arg_a.hsi.cct = fx_cct_16bit_calculate(endian_Swap_16bit((uint8_t*)&p_profile->arg.color_fade.arg_a.hsi.cct), DMX_CCT_MAX, DMX_CCT_MIN);
                    dmx_data->arg.fx.arg.color_fade.arg_a.hsi.hue = 1 + (359 * endian_Swap_16bit((uint8_t*)&p_profile->arg.color_fade.arg_a.hsi.hue) / 65535.0f);
                    dmx_data->arg.fx.arg.color_fade.arg_a.hsi.sat = 100 * endian_Swap_16bit((uint8_t*)&p_profile->arg.color_fade.arg_a.hsi.sat) / 65535.0f;
                    break;


            }

            dmx_data->arg.fx.arg.color_fade.light_board_b = dmx_to_convert(p_profile->arg.color_fade.light_board_b, 255, 1, 0, 10);

            switch(dmx_data->arg.fx.arg.color_fade.light_board_b)
            {
                case DMX_FX_MODE_CCT:
                    dmx_data->arg.fx.arg.color_fade.arg_b.cct.cct = cct_16bit_calculate(endian_Swap_16bit((uint8_t*)&p_profile->arg.color_fade.arg_b.cct.cct), DMX_CCT_MAX, DMX_CCT_MIN);
                    dmx_data->arg.fx.arg.color_fade.arg_b.cct.duv = fx_dmx_gm_16bit_calculate(endian_Swap_16bit((uint8_t*)&p_profile->arg.color_fade.arg_b.cct.gm));
                    break;

                case DMX_FX_MODE_HSI:
                    dmx_data->arg.fx.arg.color_fade.arg_b.hsi.cct = fx_cct_16bit_calculate(endian_Swap_16bit((uint8_t*)&p_profile->arg.color_fade.arg_b.hsi.cct), DMX_CCT_MAX, DMX_CCT_MIN);
                    dmx_data->arg.fx.arg.color_fade.arg_b.hsi.hue = 1 + (359 * endian_Swap_16bit((uint8_t*)&p_profile->arg.color_fade.arg_b.hsi.hue) / 65535.0f);
                    dmx_data->arg.fx.arg.color_fade.arg_b.hsi.sat = 100 * endian_Swap_16bit((uint8_t*)&p_profile->arg.color_fade.arg_b.hsi.sat) / 65535.0f;
                    break;
            }

            dmx_data->arg.fx.arg.color_fade.light_board_c = dmx_to_convert(p_profile->arg.color_fade.light_board_c, 255, 1, 0, 10);

            switch(dmx_data->arg.fx.arg.color_fade.light_board_c)
            {
                case DMX_FX_MODE_CCT:
                    dmx_data->arg.fx.arg.color_fade.arg_c.cct.cct = cct_16bit_calculate(endian_Swap_16bit((uint8_t*)&p_profile->arg.color_fade.arg_c.cct.cct), DMX_CCT_MAX, DMX_CCT_MIN);
                    dmx_data->arg.fx.arg.color_fade.arg_c.cct.duv = fx_dmx_gm_16bit_calculate(endian_Swap_16bit((uint8_t*)&p_profile->arg.color_fade.arg_c.cct.gm));
                    break;

                case DMX_FX_MODE_HSI:
                    dmx_data->arg.fx.arg.color_fade.arg_c.hsi.cct = fx_cct_16bit_calculate(endian_Swap_16bit((uint8_t*)&p_profile->arg.color_fade.arg_c.hsi.cct), DMX_CCT_MAX, DMX_CCT_MIN);
                    dmx_data->arg.fx.arg.color_fade.arg_c.hsi.hue = 1 + (359 * endian_Swap_16bit((uint8_t*)&p_profile->arg.color_fade.arg_c.hsi.hue) / 65535.0f);
                    dmx_data->arg.fx.arg.color_fade.arg_c.hsi.sat = 100 * endian_Swap_16bit((uint8_t*)&p_profile->arg.color_fade.arg_c.hsi.sat) / 65535.0f;
                    break;
            }

            dmx_data->arg.fx.arg.color_fade.light_board_d = dmx_to_convert(p_profile->arg.color_fade.light_board_d, 255, 1, 0, 10);

            switch(dmx_data->arg.fx.arg.color_fade.light_board_d)
            {
                case DMX_FX_MODE_CCT:
                    dmx_data->arg.fx.arg.color_fade.arg_d.cct.cct = cct_16bit_calculate(endian_Swap_16bit((uint8_t*)&p_profile->arg.color_fade.arg_d.cct.cct), DMX_CCT_MAX, DMX_CCT_MIN);
                    dmx_data->arg.fx.arg.color_fade.arg_d.cct.duv = fx_dmx_gm_16bit_calculate(endian_Swap_16bit((uint8_t*)&p_profile->arg.color_fade.arg_d.cct.gm));
                    break;

                case DMX_FX_MODE_HSI:
                    dmx_data->arg.fx.arg.color_fade.arg_d.hsi.cct = fx_cct_16bit_calculate(endian_Swap_16bit((uint8_t*)&p_profile->arg.color_fade.arg_d.hsi.cct), DMX_CCT_MAX, DMX_CCT_MIN);
                    dmx_data->arg.fx.arg.color_fade.arg_d.hsi.hue = 1 + (359 * endian_Swap_16bit((uint8_t*)&p_profile->arg.color_fade.arg_d.hsi.hue) / 65535.0f);
                    dmx_data->arg.fx.arg.color_fade.arg_d.hsi.sat = 100 * endian_Swap_16bit((uint8_t*)&p_profile->arg.color_fade.arg_d.hsi.sat) / 65535.0f;
                    break;
            }

            fx_state = p_profile->state < 70 ? DMX_FX_Trigger_Continue : DMX_FX_Trigger_None;

            break;

        case 16:
			dmx_model_extend((uint8_t*)&profile_data[0], sizeof(fx_rgb_16bit_color_cycle_arg_t)+4, DMX_16BIT_TYPE);
            dmx_data->arg.fx.type = DMX_FX_COLOR_CYCLE;
            dmx_data->arg.fx.arg.color_cycle.light_board_a = dmx_to_convert(p_profile->arg.color_cycle.light_board_a, 255, 1, 0, 10);
            dmx_data->arg.fx.arg.color_cycle.frq = dmx_to_convert(p_profile->arg.color_cycle.frq, 255, 10, 1, 10);

            switch(dmx_data->arg.fx.arg.color_cycle.light_board_a)
            {
                case DMX_FX_MODE_CCT:
                    dmx_data->arg.fx.arg.color_cycle.arg_a.cct.cct = cct_16bit_calculate(endian_Swap_16bit((uint8_t*)&p_profile->arg.color_cycle.arg_a.cct.cct), DMX_CCT_MAX, DMX_CCT_MIN);
                    dmx_data->arg.fx.arg.color_cycle.arg_a.cct.duv = fx_dmx_gm_16bit_calculate(endian_Swap_16bit((uint8_t*)&p_profile->arg.color_cycle.arg_a.cct.gm));
                    break;

                case DMX_FX_MODE_HSI:
                    dmx_data->arg.fx.arg.color_cycle.arg_a.hsi.cct = fx_cct_16bit_calculate(endian_Swap_16bit((uint8_t*)&p_profile->arg.color_cycle.arg_a.hsi.cct), DMX_CCT_MAX, DMX_CCT_MIN);
                    dmx_data->arg.fx.arg.color_cycle.arg_a.hsi.hue = 1 + (359 * endian_Swap_16bit((uint8_t*)&p_profile->arg.color_cycle.arg_a.hsi.hue) / 65535.0f);
                    dmx_data->arg.fx.arg.color_cycle.arg_a.hsi.sat = 100 * endian_Swap_16bit((uint8_t*)&p_profile->arg.color_cycle.arg_a.hsi.sat) / 65535.0f;
                    break;


            }

            dmx_data->arg.fx.arg.color_cycle.light_board_b = dmx_to_convert(p_profile->arg.color_cycle.light_board_b, 255, 1, 0, 10);

            switch(dmx_data->arg.fx.arg.color_cycle.light_board_b)
            {
                case DMX_FX_MODE_CCT:
                    dmx_data->arg.fx.arg.color_cycle.arg_b.cct.cct = cct_16bit_calculate(endian_Swap_16bit((uint8_t*)&p_profile->arg.color_cycle.arg_b.cct.cct), DMX_CCT_MAX, DMX_CCT_MIN);
                    dmx_data->arg.fx.arg.color_cycle.arg_b.cct.duv = fx_dmx_gm_16bit_calculate(endian_Swap_16bit((uint8_t*)&p_profile->arg.color_cycle.arg_b.cct.gm));
                    break;

                case DMX_FX_MODE_HSI:
                    dmx_data->arg.fx.arg.color_cycle.arg_b.hsi.cct = fx_cct_16bit_calculate(endian_Swap_16bit((uint8_t*)&p_profile->arg.color_cycle.arg_b.hsi.cct), DMX_CCT_MAX, DMX_CCT_MIN);
                    dmx_data->arg.fx.arg.color_cycle.arg_b.hsi.hue = 1 + (359 * endian_Swap_16bit((uint8_t*)&p_profile->arg.color_cycle.arg_b.hsi.hue) / 65535.0f);
                    dmx_data->arg.fx.arg.color_cycle.arg_b.hsi.sat = 100 * endian_Swap_16bit((uint8_t*)& p_profile->arg.color_cycle.arg_b.hsi.sat) / 65535.0f;
                    break;
            }

            dmx_data->arg.fx.arg.color_cycle.light_board_c = dmx_to_convert(p_profile->arg.color_cycle.light_board_c, 255, 1, 0, 10);

            switch(dmx_data->arg.fx.arg.color_cycle.light_board_c)
            {
                case DMX_FX_MODE_CCT:
                    dmx_data->arg.fx.arg.color_cycle.arg_c.cct.cct = cct_16bit_calculate(endian_Swap_16bit((uint8_t*)&p_profile->arg.color_cycle.arg_c.cct.cct), DMX_CCT_MAX, DMX_CCT_MIN);
                    dmx_data->arg.fx.arg.color_cycle.arg_c.cct.duv = fx_dmx_gm_16bit_calculate(endian_Swap_16bit((uint8_t*)&p_profile->arg.color_cycle.arg_c.cct.gm));
                    break;

                case DMX_FX_MODE_HSI:
                    dmx_data->arg.fx.arg.color_cycle.arg_c.hsi.cct = fx_cct_16bit_calculate(endian_Swap_16bit((uint8_t*)&p_profile->arg.color_cycle.arg_c.hsi.cct), DMX_CCT_MAX, DMX_CCT_MIN);
                    dmx_data->arg.fx.arg.color_cycle.arg_c.hsi.hue = 1 + (359 * endian_Swap_16bit((uint8_t*)&p_profile->arg.color_cycle.arg_c.hsi.hue) / 65535.0f);
                    dmx_data->arg.fx.arg.color_cycle.arg_c.hsi.sat = 100 * endian_Swap_16bit((uint8_t*)&p_profile->arg.color_cycle.arg_c.hsi.sat) / 65535.0f;
                    break;
            }

            dmx_data->arg.fx.arg.color_cycle.light_board_d = dmx_to_convert(p_profile->arg.color_cycle.light_board_d, 255, 1, 0, 10);

            switch(dmx_data->arg.fx.arg.color_cycle.light_board_d)
            {
                case DMX_FX_MODE_CCT:
                    dmx_data->arg.fx.arg.color_cycle.arg_d.cct.cct = cct_16bit_calculate(endian_Swap_16bit((uint8_t*)&p_profile->arg.color_cycle.arg_d.cct.cct), DMX_CCT_MAX, DMX_CCT_MIN);
                    dmx_data->arg.fx.arg.color_cycle.arg_d.cct.duv = fx_dmx_gm_16bit_calculate(endian_Swap_16bit((uint8_t*)&p_profile->arg.color_cycle.arg_d.cct.gm));
                    break;

                case DMX_FX_MODE_HSI:
                    dmx_data->arg.fx.arg.color_cycle.arg_d.hsi.cct = fx_cct_16bit_calculate(endian_Swap_16bit((uint8_t*)&p_profile->arg.color_cycle.arg_d.hsi.cct), DMX_CCT_MAX, DMX_CCT_MIN);
                    dmx_data->arg.fx.arg.color_cycle.arg_d.hsi.hue = 1 + (359 * endian_Swap_16bit((uint8_t*)&p_profile->arg.color_cycle.arg_d.hsi.hue) / 65535.0f);
                    dmx_data->arg.fx.arg.color_cycle.arg_d.hsi.sat = 100 * endian_Swap_16bit((uint8_t*)&p_profile->arg.color_cycle.arg_d.hsi.sat) / 65535.0f;
                    break;
            }

            fx_state = p_profile->state < 70 ? DMX_FX_Trigger_Continue : DMX_FX_Trigger_None;

            break;

        case 17:
			dmx_model_extend((uint8_t*)&profile_data[0], sizeof(fx_rgb_16bit_color_gradient_arg_t)+4, DMX_16BIT_TYPE);
            dmx_data->arg.fx.type = DMX_FX_COLOR_GRADIENT;
            dmx_data->arg.fx.arg.color_gradient.light_board_a = dmx_to_convert(p_profile->arg.color_gradient.light_board_a, 255, 1, 0, 10);
            dmx_data->arg.fx.arg.color_gradient.frq = dmx_to_convert(p_profile->arg.color_gradient.frq, 255, 10, 1, 10);
            dmx_data->arg.fx.arg.color_gradient.min_brightness = p_profile->arg.color_gradient.min_brightness * 750 / 255;

            switch(dmx_data->arg.fx.arg.color_gradient.light_board_a)
            {
                case DMX_FX_MODE_CCT:
                    dmx_data->arg.fx.arg.color_gradient.arg_a.cct.cct = cct_16bit_calculate(endian_Swap_16bit((uint8_t*)&p_profile->arg.color_gradient.arg_a.cct.cct), DMX_CCT_MAX, DMX_CCT_MIN);
                    dmx_data->arg.fx.arg.color_gradient.arg_a.cct.duv = fx_dmx_gm_16bit_calculate(endian_Swap_16bit((uint8_t*)&p_profile->arg.color_gradient.arg_a.cct.gm));
                    break;

                case DMX_FX_MODE_HSI:
                    dmx_data->arg.fx.arg.color_gradient.arg_a.hsi.cct = fx_cct_16bit_calculate(endian_Swap_16bit((uint8_t*)&p_profile->arg.color_gradient.arg_a.hsi.cct), DMX_CCT_MAX, DMX_CCT_MIN);
                    dmx_data->arg.fx.arg.color_gradient.arg_a.hsi.hue = 1 + (359 * endian_Swap_16bit((uint8_t*)&p_profile->arg.color_gradient.arg_a.hsi.hue) / 65535.0f);
                    dmx_data->arg.fx.arg.color_gradient.arg_a.hsi.sat = 100 * endian_Swap_16bit((uint8_t*)&p_profile->arg.color_gradient.arg_a.hsi.sat) / 65535.0f;
                    break;


            }

            dmx_data->arg.fx.arg.color_gradient.light_board_b = dmx_to_convert(p_profile->arg.color_gradient.light_board_b, 255, 1, 0, 10);

            switch(dmx_data->arg.fx.arg.color_gradient.light_board_b)
            {
                case DMX_FX_MODE_CCT:
                    dmx_data->arg.fx.arg.color_gradient.arg_b.cct.cct = cct_16bit_calculate(endian_Swap_16bit((uint8_t*)&p_profile->arg.color_gradient.arg_b.cct.cct), DMX_CCT_MAX, DMX_CCT_MIN);
                    dmx_data->arg.fx.arg.color_gradient.arg_b.cct.duv = fx_dmx_gm_16bit_calculate(endian_Swap_16bit((uint8_t*)&p_profile->arg.color_gradient.arg_b.cct.gm));
                    break;

                case DMX_FX_MODE_HSI:
                    dmx_data->arg.fx.arg.color_gradient.arg_b.hsi.cct = fx_cct_16bit_calculate(p_profile->arg.color_gradient.arg_b.hsi.cct, DMX_CCT_MAX, DMX_CCT_MIN);
                    dmx_data->arg.fx.arg.color_gradient.arg_b.hsi.hue = 1 + (359 * endian_Swap_16bit((uint8_t*)&p_profile->arg.color_gradient.arg_b.hsi.hue) / 65535.0f);
                    dmx_data->arg.fx.arg.color_gradient.arg_b.hsi.sat = 100 * endian_Swap_16bit((uint8_t*)&p_profile->arg.color_gradient.arg_b.hsi.sat) / 65535.0f;
                    break;
            }

            dmx_data->arg.fx.arg.color_gradient.light_board_c = dmx_to_convert(p_profile->arg.color_gradient.light_board_c, 255, 1, 0, 10);

            switch(dmx_data->arg.fx.arg.color_gradient.light_board_c)
            {
                case DMX_FX_MODE_CCT:
                    dmx_data->arg.fx.arg.color_gradient.arg_c.cct.cct = cct_16bit_calculate(endian_Swap_16bit((uint8_t*)&p_profile->arg.color_gradient.arg_c.cct.cct), DMX_CCT_MAX, DMX_CCT_MIN);
                    dmx_data->arg.fx.arg.color_gradient.arg_c.cct.duv = fx_dmx_gm_16bit_calculate(endian_Swap_16bit((uint8_t*)&p_profile->arg.color_gradient.arg_c.cct.gm));
                    break;

                case DMX_FX_MODE_HSI:
                    dmx_data->arg.fx.arg.color_gradient.arg_c.hsi.cct = fx_cct_16bit_calculate(p_profile->arg.color_gradient.arg_c.hsi.cct, DMX_CCT_MAX, DMX_CCT_MIN);
                    dmx_data->arg.fx.arg.color_gradient.arg_c.hsi.hue = 1 + (359 * endian_Swap_16bit((uint8_t*)&p_profile->arg.color_gradient.arg_c.hsi.hue) / 65535.0f);
                    dmx_data->arg.fx.arg.color_gradient.arg_c.hsi.sat = 100 * endian_Swap_16bit((uint8_t*)&p_profile->arg.color_gradient.arg_c.hsi.sat) / 65535.0f;
                    break;
            }

            dmx_data->arg.fx.arg.color_gradient.light_board_d = dmx_to_convert(p_profile->arg.color_gradient.light_board_d, 255, 1, 0, 10);

            switch(dmx_data->arg.fx.arg.color_gradient.light_board_d)
            {
                case DMX_FX_MODE_CCT:
                    dmx_data->arg.fx.arg.color_gradient.arg_d.cct.cct = cct_16bit_calculate(endian_Swap_16bit((uint8_t*)&p_profile->arg.color_gradient.arg_d.cct.cct), DMX_CCT_MAX, DMX_CCT_MIN);
                    dmx_data->arg.fx.arg.color_gradient.arg_d.cct.duv = fx_dmx_gm_16bit_calculate(endian_Swap_16bit((uint8_t*)&p_profile->arg.color_gradient.arg_d.cct.gm));
                    break;

                case DMX_FX_MODE_HSI:
                    dmx_data->arg.fx.arg.color_gradient.arg_d.hsi.cct = fx_cct_16bit_calculate(p_profile->arg.color_gradient.arg_d.hsi.cct, DMX_CCT_MAX, DMX_CCT_MIN);
                    dmx_data->arg.fx.arg.color_gradient.arg_d.hsi.hue = 1 + (359 * endian_Swap_16bit((uint8_t*)&p_profile->arg.color_gradient.arg_d.hsi.hue) / 65535.0f);
                    dmx_data->arg.fx.arg.color_gradient.arg_d.hsi.sat = 100 * endian_Swap_16bit((uint8_t*)&p_profile->arg.color_gradient.arg_d.hsi.sat) / 65535.0f;
                    break;
            }

            fx_state = p_profile->state < 70 ? DMX_FX_Trigger_Continue : DMX_FX_Trigger_None;
            break;

        case 18:
			dmx_model_extend((uint8_t*)&profile_data[0], sizeof(fx_rgb_16bit_one_color_chase_arg_t)+4, DMX_16BIT_TYPE);
            dmx_data->arg.fx.type = DMX_FX_ONE_COLOR_CHASE;
            dmx_data->arg.fx.arg.one_color_chase.light_board_a = dmx_to_convert(p_profile->arg.one_color_chase.light_board_a, 255, 1, 0, 10);
            dmx_data->arg.fx.arg.one_color_chase.frq = dmx_to_convert(p_profile->arg.one_color_chase.frq, 255, 10, 1, 10);
            dmx_data->arg.fx.arg.one_color_chase.direction = dmx_to_convert(p_profile->arg.one_color_chase.direction, 255, 1, 0, 10);

            switch(dmx_data->arg.fx.arg.one_color_chase.light_board_a)
            {
                case DMX_FX_MODE_CCT:
                    dmx_data->arg.fx.arg.one_color_chase.arg_a.cct.cct = cct_16bit_calculate(endian_Swap_16bit((uint8_t*)&p_profile->arg.one_color_chase.arg_a.cct.cct), DMX_CCT_MAX, DMX_CCT_MIN);
                    dmx_data->arg.fx.arg.one_color_chase.arg_a.cct.duv = fx_dmx_gm_16bit_calculate(endian_Swap_16bit((uint8_t*)&p_profile->arg.one_color_chase.arg_a.cct.gm));
                    break;

                case DMX_FX_MODE_HSI:
                    dmx_data->arg.fx.arg.one_color_chase.arg_a.hsi.cct = fx_cct_16bit_calculate(endian_Swap_16bit((uint8_t*)&p_profile->arg.one_color_chase.arg_a.hsi.cct), DMX_CCT_MAX, DMX_CCT_MIN);
                    dmx_data->arg.fx.arg.one_color_chase.arg_a.hsi.hue = 1 + (359 * endian_Swap_16bit((uint8_t*)&p_profile->arg.one_color_chase.arg_a.hsi.hue) / 65535.0f);
                    dmx_data->arg.fx.arg.one_color_chase.arg_a.hsi.sat = 100 * endian_Swap_16bit((uint8_t*)&p_profile->arg.one_color_chase.arg_a.hsi.sat) / 65535.0f;
                    break;


            }

            dmx_data->arg.fx.arg.one_color_chase.light_board_b = dmx_to_convert(p_profile->arg.color_gradient.light_board_b, 255, 1, 0, 10);

            switch(dmx_data->arg.fx.arg.one_color_chase.light_board_b)
            {
                case DMX_FX_MODE_CCT:
                    dmx_data->arg.fx.arg.one_color_chase.arg_b.cct.cct = cct_16bit_calculate(endian_Swap_16bit((uint8_t*)&p_profile->arg.color_gradient.arg_b.cct.cct), DMX_CCT_MAX, DMX_CCT_MIN);
                    dmx_data->arg.fx.arg.one_color_chase.arg_b.cct.duv = fx_dmx_gm_16bit_calculate(endian_Swap_16bit((uint8_t*)&p_profile->arg.color_gradient.arg_b.cct.gm));
                    break;

                case DMX_FX_MODE_HSI:
                    dmx_data->arg.fx.arg.one_color_chase.arg_b.hsi.cct = fx_cct_16bit_calculate(endian_Swap_16bit((uint8_t*)&p_profile->arg.color_gradient.arg_b.hsi.cct), DMX_CCT_MAX, DMX_CCT_MIN);
                    dmx_data->arg.fx.arg.one_color_chase.arg_b.hsi.hue = 1 + (359 * endian_Swap_16bit((uint8_t*)&p_profile->arg.color_gradient.arg_b.hsi.hue) / 65535.0f);
                    dmx_data->arg.fx.arg.one_color_chase.arg_b.hsi.sat = 100 * endian_Swap_16bit((uint8_t*)&p_profile->arg.color_gradient.arg_b.hsi.sat) / 65535.0f;
                    break;
            }

            fx_state = p_profile->state < 70 ? DMX_FX_Trigger_Continue : DMX_FX_Trigger_None;
            break;

    }

    return fx_state;
}


uint8_t profile_0709_pixel_8bit_analysis(const uint8_t* profile_data, dmx_data_t* dmx_data)
{

    if(NULL == profile_data || NULL == dmx_data)
        return 0;

    const profile_0709_pixel_8bit_arg_t* p_profile = (profile_0709_pixel_8bit_arg_t*)&profile_data[0];
    uint8_t fx_state;
    dmx_data->lightness = (*(uint8_t*)profile_data * 1000  / 255 );
    dmx_data->type = DMX_TYPE_FX_PIXEL;
    dmx_model_extend((uint8_t*)&profile_data[0], sizeof(profile_0709_pixel_8bit_arg_t), DMX_8BIT_TYPE);

    switch(p_profile->type / 10)
    {
        case 0:
            dmx_data->arg.pixel_fx.type = DMX_FX_COLOE_FADE;
            dmx_data->arg.pixel_fx.coloe_fade.color_choose = dmx_pixel_fx_colors_type_get(1, p_profile->arg.coloe_fade.color_choose);
            dmx_data->arg.pixel_fx.coloe_fade.spd = dmx_to_linear_calculate(p_profile->arg.coloe_fade.spd, 255, 640, 1, 1);
            dmx_data->arg.pixel_fx.coloe_fade.move = dmx_to_convert(p_profile->arg.coloe_fade.move, 255, 1, 0, 128);
            dmx_data->arg.pixel_fx.coloe_fade.color_1_move = dmx_to_convert(p_profile->arg.coloe_fade.color_1_move, 255, 1, 0, 10);

            switch(dmx_data->arg.pixel_fx.coloe_fade.color_1_move)
            {
                case DMX_FX_MODE_CCT:
                    dmx_data->arg.pixel_fx.coloe_fade.arg1.cct.cct = cct_8bit_calculate(p_profile->arg.coloe_fade.arg1.cct.cct, DMX_CCT_MAX, DMX_CCT_MIN);
                    dmx_data->arg.pixel_fx.coloe_fade.arg1.cct.duv = fx_dmx_gm_8bit_calculate(p_profile->arg.coloe_fade.arg1.cct.gm);
                    break;

                case DMX_FX_MODE_HSI:
                    dmx_data->arg.pixel_fx.coloe_fade.arg1.hsi.cct = fx_cct_8bit_calculate(p_profile->arg.coloe_fade.arg1.hsi.cct, DMX_CCT_MAX, DMX_CCT_MIN);
                    dmx_data->arg.pixel_fx.coloe_fade.arg1.hsi.hue = 1 + (359 * p_profile->arg.coloe_fade.arg1.hsi.hue / 255.0f);
                    dmx_data->arg.pixel_fx.coloe_fade.arg1.hsi.sat = 100 * p_profile->arg.coloe_fade.arg1.hsi.sat / 255.0f;
                    break;

            }

            dmx_data->arg.pixel_fx.coloe_fade.color_2_move = dmx_to_convert(p_profile->arg.coloe_fade.color_2_move, 255, 1, 0, 10);

            switch(dmx_data->arg.pixel_fx.coloe_fade.color_2_move)
            {
                case DMX_FX_MODE_CCT:
                    dmx_data->arg.pixel_fx.coloe_fade.arg2.cct.cct = cct_8bit_calculate(p_profile->arg.coloe_fade.arg2.cct.cct, DMX_CCT_MAX, DMX_CCT_MIN);
                    dmx_data->arg.pixel_fx.coloe_fade.arg2.cct.duv = fx_dmx_gm_8bit_calculate(p_profile->arg.coloe_fade.arg2.cct.gm);
                    break;

                case DMX_FX_MODE_HSI:
                    dmx_data->arg.pixel_fx.coloe_fade.arg2.hsi.cct = fx_cct_8bit_calculate(p_profile->arg.coloe_fade.arg2.hsi.cct, DMX_CCT_MAX, DMX_CCT_MIN);
                    dmx_data->arg.pixel_fx.coloe_fade.arg2.hsi.hue = 1 + (359 * p_profile->arg.coloe_fade.arg2.hsi.hue / 255.0f);
                    dmx_data->arg.pixel_fx.coloe_fade.arg2.hsi.sat = 100 * p_profile->arg.coloe_fade.arg2.hsi.sat / 255.0f;
                    break;


            }

            dmx_data->arg.pixel_fx.coloe_fade.color_3_move = dmx_to_convert(p_profile->arg.coloe_fade.color_3_move, 255, 1, 0, 10);

            switch(dmx_data->arg.pixel_fx.coloe_fade.color_3_move)
            {
                case DMX_FX_MODE_CCT:
                    dmx_data->arg.pixel_fx.coloe_fade.arg3.cct.cct = cct_8bit_calculate(p_profile->arg.coloe_fade.arg3.cct.cct, DMX_CCT_MAX, DMX_CCT_MIN);
                    dmx_data->arg.pixel_fx.coloe_fade.arg3.cct.duv = fx_dmx_gm_8bit_calculate(p_profile->arg.coloe_fade.arg3.cct.gm);
                    break;

                case DMX_FX_MODE_HSI:
                    dmx_data->arg.pixel_fx.coloe_fade.arg3.hsi.cct = fx_cct_8bit_calculate(p_profile->arg.coloe_fade.arg3.hsi.cct, DMX_CCT_MAX, DMX_CCT_MIN);
                    dmx_data->arg.pixel_fx.coloe_fade.arg3.hsi.hue = 1 + (359 * p_profile->arg.coloe_fade.arg3.hsi.hue / 255.0f);
                    dmx_data->arg.pixel_fx.coloe_fade.arg3.hsi.sat = 100 * p_profile->arg.coloe_fade.arg3.hsi.sat / 255.0f;
                    break;


            }

            dmx_data->arg.pixel_fx.coloe_fade.color_4_move = dmx_to_convert(p_profile->arg.coloe_fade.color_4_move, 255, 1, 0, 10);

            switch(dmx_data->arg.pixel_fx.coloe_fade.color_4_move)
            {
                case DMX_FX_MODE_CCT:
                    dmx_data->arg.pixel_fx.coloe_fade.arg4.cct.cct = cct_8bit_calculate(p_profile->arg.coloe_fade.arg4.cct.cct, DMX_CCT_MAX, DMX_CCT_MIN);
                    dmx_data->arg.pixel_fx.coloe_fade.arg4.cct.duv = fx_dmx_gm_8bit_calculate(p_profile->arg.coloe_fade.arg4.cct.gm);
                    break;

                case DMX_FX_MODE_HSI:
                    dmx_data->arg.pixel_fx.coloe_fade.arg4.hsi.cct = fx_cct_8bit_calculate(p_profile->arg.coloe_fade.arg4.hsi.cct, DMX_CCT_MAX, DMX_CCT_MIN);
                    dmx_data->arg.pixel_fx.coloe_fade.arg4.hsi.hue = 1 + (359 * p_profile->arg.coloe_fade.arg4.hsi.hue / 255.0f);
                    dmx_data->arg.pixel_fx.coloe_fade.arg4.hsi.sat = 100 * p_profile->arg.coloe_fade.arg4.hsi.sat / 255.0f;
                    break;


            }

            fx_state = p_profile->state < 10 ? DMX_FX_Trigger_Continue : DMX_FX_Trigger_None;
            break;

        case 1:
            dmx_data->arg.pixel_fx.type = DMX_FX_COLOE_CYCLE;
            dmx_data->arg.pixel_fx.coloe_cycle.color_choose = dmx_pixel_fx_colors_type_get(1, p_profile->arg.coloe_cycle.color_choose);
            dmx_data->arg.pixel_fx.coloe_cycle.spd = dmx_to_linear_calculate(p_profile->arg.coloe_cycle.spd, 255, 100, 1, 1);
            dmx_data->arg.pixel_fx.coloe_cycle.transition = 100 * p_profile->arg.coloe_cycle.transition / 255;
            dmx_data->arg.pixel_fx.coloe_cycle.move = dmx_to_convert(p_profile->arg.coloe_cycle.move, 255, 1, 0, 128);
            dmx_data->arg.pixel_fx.coloe_cycle.color_1_move = dmx_to_convert(p_profile->arg.coloe_cycle.color_1_move, 255, 1, 0, 10);

            switch(dmx_data->arg.pixel_fx.coloe_cycle.color_1_move)
            {
                case DMX_FX_MODE_CCT:
                    dmx_data->arg.pixel_fx.coloe_cycle.arg1.cct.cct = cct_8bit_calculate(p_profile->arg.coloe_cycle.arg1.cct.cct, DMX_CCT_MAX, DMX_CCT_MIN);
                    dmx_data->arg.pixel_fx.coloe_cycle.arg1.cct.duv = fx_dmx_gm_8bit_calculate(p_profile->arg.coloe_cycle.arg1.cct.gm);
                    break;

                case DMX_FX_MODE_HSI:
                    dmx_data->arg.pixel_fx.coloe_cycle.arg1.hsi.cct = fx_cct_8bit_calculate(p_profile->arg.coloe_cycle.arg1.hsi.cct, DMX_CCT_MAX, DMX_CCT_MIN);
                    dmx_data->arg.pixel_fx.coloe_cycle.arg1.hsi.hue = 1 + (359 * p_profile->arg.coloe_cycle.arg1.hsi.hue / 255.0f);
                    dmx_data->arg.pixel_fx.coloe_cycle.arg1.hsi.sat = 100 * p_profile->arg.coloe_cycle.arg1.hsi.sat / 255.0f;
                    break;


            }

            dmx_data->arg.pixel_fx.coloe_cycle.color_2_move = dmx_to_convert(p_profile->arg.coloe_cycle.color_2_move, 255, 1, 0, 10);

            switch(dmx_data->arg.pixel_fx.coloe_cycle.color_2_move)
            {
                case DMX_FX_MODE_CCT:
                    dmx_data->arg.pixel_fx.coloe_cycle.arg2.cct.cct = cct_8bit_calculate(p_profile->arg.coloe_cycle.arg2.cct.cct, DMX_CCT_MAX, DMX_CCT_MIN);
                    dmx_data->arg.pixel_fx.coloe_cycle.arg2.cct.duv = fx_dmx_gm_8bit_calculate(p_profile->arg.coloe_cycle.arg2.cct.gm);
                    break;

                case DMX_FX_MODE_HSI:
                    dmx_data->arg.pixel_fx.coloe_cycle.arg2.hsi.cct = fx_cct_8bit_calculate(p_profile->arg.coloe_cycle.arg2.hsi.cct, DMX_CCT_MAX, DMX_CCT_MIN);
                    dmx_data->arg.pixel_fx.coloe_cycle.arg2.hsi.hue = 1 + (359 * p_profile->arg.coloe_cycle.arg2.hsi.hue / 255.0f);
                    dmx_data->arg.pixel_fx.coloe_cycle.arg2.hsi.sat = 100 * p_profile->arg.coloe_cycle.arg2.hsi.sat / 255.0f;
                    break;


            }

            dmx_data->arg.pixel_fx.coloe_cycle.color_3_move = dmx_to_convert(p_profile->arg.coloe_cycle.color_3_move, 255, 1, 0, 10);

            switch(dmx_data->arg.pixel_fx.coloe_cycle.color_3_move)
            {
                case DMX_FX_MODE_CCT:
                    dmx_data->arg.pixel_fx.coloe_cycle.arg3.cct.cct = cct_8bit_calculate(p_profile->arg.coloe_cycle.arg3.cct.cct, DMX_CCT_MAX, DMX_CCT_MIN);
                    dmx_data->arg.pixel_fx.coloe_cycle.arg3.cct.duv = fx_dmx_gm_8bit_calculate(p_profile->arg.coloe_cycle.arg3.cct.gm);
                    break;

                case DMX_FX_MODE_HSI:
                    dmx_data->arg.pixel_fx.coloe_cycle.arg3.hsi.cct = fx_cct_8bit_calculate(p_profile->arg.coloe_cycle.arg3.hsi.cct, DMX_CCT_MAX, DMX_CCT_MIN);
                    dmx_data->arg.pixel_fx.coloe_cycle.arg3.hsi.hue = 1 + (359 * p_profile->arg.coloe_cycle.arg3.hsi.hue / 255.0f);
                    dmx_data->arg.pixel_fx.coloe_cycle.arg3.hsi.sat = 100 * p_profile->arg.coloe_cycle.arg3.hsi.sat / 255.0f;
                    break;


            }

            dmx_data->arg.pixel_fx.coloe_cycle.color_4_move = dmx_to_convert(p_profile->arg.coloe_cycle.color_4_move, 255, 1, 0, 10);

            switch(dmx_data->arg.pixel_fx.coloe_cycle.color_4_move)
            {
                case DMX_FX_MODE_CCT:
                    dmx_data->arg.pixel_fx.coloe_cycle.arg4.cct.cct = cct_8bit_calculate(p_profile->arg.coloe_cycle.arg4.cct.cct, DMX_CCT_MAX, DMX_CCT_MIN);
                    dmx_data->arg.pixel_fx.coloe_cycle.arg4.cct.duv = fx_dmx_gm_8bit_calculate(p_profile->arg.coloe_cycle.arg4.cct.gm);
                    break;

                case DMX_FX_MODE_HSI:
                    dmx_data->arg.pixel_fx.coloe_cycle.arg4.hsi.cct = fx_cct_8bit_calculate(p_profile->arg.coloe_cycle.arg4.hsi.cct, DMX_CCT_MAX, DMX_CCT_MIN);
                    dmx_data->arg.pixel_fx.coloe_cycle.arg4.hsi.hue = 1 + (359 * p_profile->arg.coloe_cycle.arg4.hsi.hue / 255.0f);
                    dmx_data->arg.pixel_fx.coloe_cycle.arg4.hsi.sat = 100 * p_profile->arg.coloe_cycle.arg4.hsi.sat / 255.0f;
                    break;


            }

            fx_state = p_profile->state < 10 ? DMX_FX_Trigger_Continue : DMX_FX_Trigger_None;
            break;

        case 2:
            dmx_data->arg.pixel_fx.type = DMX_FX_ONE_PIXEL_CHASE;

            dmx_data->arg.pixel_fx.one_pixel_chase.pixel_size = dmx_pixel_fx_colors_type_get(2, p_profile->arg.one_pixel_chase.pixel_size);
            dmx_data->arg.pixel_fx.one_pixel_chase.spd = dmx_to_linear_calculate(p_profile->arg.one_pixel_chase.spd, 255, 100, 1, 1);
            dmx_data->arg.pixel_fx.one_pixel_chase.departure = dmx_to_convert(p_profile->arg.one_pixel_chase.departure, 255, 1, 0, 128);

            switch(dmx_data->arg.pixel_fx.one_pixel_chase.departure)
            {
                case 0:
                    dmx_data->arg.pixel_fx.one_pixel_chase.move_choose.move_1_way = dmx_to_convert(p_profile->arg.one_pixel_chase.move_choose.move_1_way, 255, 2, 0, 60);
                    dmx_data->arg.pixel_fx.one_pixel_chase.way_mode.way1.pixel_1_mode = dmx_to_convert(p_profile->arg.one_pixel_chase.way_mode.way1.pixel_1_mode, 255, 1, 0, 10);

                    switch(dmx_data->arg.pixel_fx.one_pixel_chase.way_mode.way1.pixel_1_mode)
                    {
                        case DMX_FX_MODE_CCT:
                            dmx_data->arg.pixel_fx.one_pixel_chase.way_mode.way1.arg1.cct.cct = cct_8bit_calculate(p_profile->arg.one_pixel_chase.way_mode.way1.arg1.cct.cct, DMX_CCT_MAX, DMX_CCT_MIN);
                            dmx_data->arg.pixel_fx.one_pixel_chase.way_mode.way1.arg1.cct.duv = fx_dmx_gm_8bit_calculate(p_profile->arg.one_pixel_chase.way_mode.way1.arg1.cct.gm);
                            break;

                        case DMX_FX_MODE_HSI:
                            dmx_data->arg.pixel_fx.one_pixel_chase.way_mode.way1.arg1.hsi.cct = fx_cct_8bit_calculate(p_profile->arg.one_pixel_chase.way_mode.way1.arg1.hsi.cct, DMX_CCT_MAX, DMX_CCT_MIN);
                            dmx_data->arg.pixel_fx.one_pixel_chase.way_mode.way1.arg1.hsi.hue = 1 + (359 * p_profile->arg.one_pixel_chase.way_mode.way1.arg1.hsi.hue / 255.0f);
                            dmx_data->arg.pixel_fx.one_pixel_chase.way_mode.way1.arg1.hsi.sat = 100 * p_profile->arg.one_pixel_chase.way_mode.way1.arg1.hsi.sat / 255.0f;
                            break;


                    }

                    dmx_data->arg.pixel_fx.one_pixel_chase.way_mode.way1.background_color_int = p_profile->arg.one_pixel_chase.way_mode.way1.background_color_int * 1000 /  255;
                           

                    dmx_data->arg.pixel_fx.one_pixel_chase.way_mode.way1.background_color_mode = dmx_to_convert(
                                p_profile->arg.one_pixel_chase.way_mode.way1.background_color_mode, 255, 1, 0, 10);

                    switch(dmx_data->arg.pixel_fx.one_pixel_chase.way_mode.way1.background_color_mode)
                    {
                        case DMX_FX_MODE_CCT:
                            dmx_data->arg.pixel_fx.one_pixel_chase.way_mode.way1.arg2.cct.cct = cct_8bit_calculate(p_profile->arg.one_pixel_chase.way_mode.way1.arg2.cct.cct, DMX_CCT_MAX, DMX_CCT_MIN);
                            dmx_data->arg.pixel_fx.one_pixel_chase.way_mode.way1.arg2.cct.duv = fx_dmx_gm_8bit_calculate(p_profile->arg.one_pixel_chase.way_mode.way1.arg2.cct.gm);
                            break;

                        case DMX_FX_MODE_HSI:
                            dmx_data->arg.pixel_fx.one_pixel_chase.way_mode.way1.arg2.hsi.cct = fx_cct_8bit_calculate(p_profile->arg.one_pixel_chase.way_mode.way1.arg2.hsi.cct, DMX_CCT_MAX, DMX_CCT_MIN);
                            dmx_data->arg.pixel_fx.one_pixel_chase.way_mode.way1.arg2.hsi.hue = 1 + (359 * p_profile->arg.one_pixel_chase.way_mode.way1.arg2.hsi.hue / 255.0f);
                            dmx_data->arg.pixel_fx.one_pixel_chase.way_mode.way1.arg2.hsi.sat = 100 * p_profile->arg.one_pixel_chase.way_mode.way1.arg2.hsi.sat / 255.0f;
                            break;


                    }

                    break;

                case 1:
                    dmx_data->arg.pixel_fx.one_pixel_chase.move_choose.move_2_way = dmx_to_convert(p_profile->arg.one_pixel_chase.move_choose.move_2_way, 255, 2, 0, 60);
                    dmx_data->arg.pixel_fx.one_pixel_chase.way_mode.way2.pixel_1_mode = dmx_to_convert(p_profile->arg.one_pixel_chase.way_mode.way2.pixel_1_mode, 255, 1, 0, 10);

                    switch(dmx_data->arg.pixel_fx.one_pixel_chase.way_mode.way2.pixel_1_mode)
                    {
                        case DMX_FX_MODE_CCT:
                            dmx_data->arg.pixel_fx.one_pixel_chase.way_mode.way2.arg1.cct.cct = cct_8bit_calculate(p_profile->arg.one_pixel_chase.way_mode.way2.arg1.cct.cct, DMX_CCT_MAX, DMX_CCT_MIN);
                            dmx_data->arg.pixel_fx.one_pixel_chase.way_mode.way2.arg1.cct.duv = fx_dmx_gm_8bit_calculate(p_profile->arg.one_pixel_chase.way_mode.way2.arg1.cct.gm);
                            break;

                        case DMX_FX_MODE_HSI:
                            dmx_data->arg.pixel_fx.one_pixel_chase.way_mode.way2.arg1.hsi.cct = fx_cct_8bit_calculate(p_profile->arg.one_pixel_chase.way_mode.way2.arg1.hsi.cct, DMX_CCT_MAX, DMX_CCT_MIN);
                            dmx_data->arg.pixel_fx.one_pixel_chase.way_mode.way2.arg1.hsi.hue = 1 + (359 * p_profile->arg.one_pixel_chase.way_mode.way2.arg1.hsi.hue / 255.0f);
                            dmx_data->arg.pixel_fx.one_pixel_chase.way_mode.way2.arg1.hsi.sat = 100 * p_profile->arg.one_pixel_chase.way_mode.way2.arg1.hsi.sat / 255.0f;
                            break;


                    }

                    dmx_data->arg.pixel_fx.one_pixel_chase.way_mode.way2.pixel_2_mode = dmx_to_convert(p_profile->arg.one_pixel_chase.way_mode.way2.pixel_2_mode, 255, 1, 0, 10);

                    switch(dmx_data->arg.pixel_fx.one_pixel_chase.way_mode.way2.pixel_2_mode)
                    {
                        case DMX_FX_MODE_CCT:
                            dmx_data->arg.pixel_fx.one_pixel_chase.way_mode.way2.arg2.cct.cct = cct_8bit_calculate(p_profile->arg.one_pixel_chase.way_mode.way2.arg2.cct.cct, DMX_CCT_MAX, DMX_CCT_MIN);
                            dmx_data->arg.pixel_fx.one_pixel_chase.way_mode.way2.arg2.cct.duv = fx_dmx_gm_8bit_calculate(p_profile->arg.one_pixel_chase.way_mode.way2.arg2.cct.gm);
                            break;

                        case DMX_FX_MODE_HSI:
                            dmx_data->arg.pixel_fx.one_pixel_chase.way_mode.way2.arg2.hsi.cct = fx_cct_8bit_calculate(p_profile->arg.one_pixel_chase.way_mode.way2.arg2.hsi.cct, DMX_CCT_MAX, DMX_CCT_MIN);
                            dmx_data->arg.pixel_fx.one_pixel_chase.way_mode.way2.arg2.hsi.hue = 1 + (359 * p_profile->arg.one_pixel_chase.way_mode.way2.arg2.hsi.hue / 255.0f);
                            dmx_data->arg.pixel_fx.one_pixel_chase.way_mode.way2.arg2.hsi.sat = 100 * p_profile->arg.one_pixel_chase.way_mode.way2.arg2.hsi.sat / 255.0f;
                            break;


                    }


                    dmx_data->arg.pixel_fx.one_pixel_chase.way_mode.way2.background_color_int = p_profile->arg.one_pixel_chase.way_mode.way2.background_color_int *1000 /  255;
                           

                    dmx_data->arg.pixel_fx.one_pixel_chase.way_mode.way2.background_color_mode = dmx_to_convert(
                                p_profile->arg.one_pixel_chase.way_mode.way2.background_color_mode, 255, 1, 0, 10);

                    switch(dmx_data->arg.pixel_fx.one_pixel_chase.way_mode.way2.background_color_mode)
                    {
                        case DMX_FX_MODE_CCT:
                            dmx_data->arg.pixel_fx.one_pixel_chase.way_mode.way2.arg3.cct.cct = fx_cct_8bit_calculate(p_profile->arg.one_pixel_chase.way_mode.way2.arg3.cct.cct, DMX_CCT_MAX, DMX_CCT_MIN);
                            dmx_data->arg.pixel_fx.one_pixel_chase.way_mode.way2.arg3.cct.duv = fx_dmx_gm_8bit_calculate(p_profile->arg.one_pixel_chase.way_mode.way2.arg3.cct.gm);
                            break;

                        case DMX_FX_MODE_HSI:
                            dmx_data->arg.pixel_fx.one_pixel_chase.way_mode.way2.arg3.hsi.cct = cct_8bit_calculate(p_profile->arg.one_pixel_chase.way_mode.way2.arg3.hsi.cct, DMX_CCT_MAX, DMX_CCT_MIN);
                            dmx_data->arg.pixel_fx.one_pixel_chase.way_mode.way2.arg3.hsi.hue = 1 + (359 * p_profile->arg.one_pixel_chase.way_mode.way2.arg3.hsi.hue / 255.0f);
                            dmx_data->arg.pixel_fx.one_pixel_chase.way_mode.way2.arg3.hsi.sat = 100 * p_profile->arg.one_pixel_chase.way_mode.way2.arg3.hsi.sat / 255.0f;
                            break;


                    }

                    break;

            }

            fx_state = p_profile->state < 10 ? DMX_FX_Trigger_Continue : DMX_FX_Trigger_None;
            break;

        case 3:
            dmx_data->arg.pixel_fx.type = DMX_FX_TWO_PIXEL_CHASE;
            dmx_data->arg.pixel_fx.two_pixel_chase.pixel_size = dmx_pixel_fx_colors_type_get(2, p_profile->arg.two_pixel_chase.pixel_size);
            dmx_data->arg.pixel_fx.two_pixel_chase.spd =  dmx_to_linear_calculate(p_profile->arg.one_pixel_chase.spd, 255, 640, 1, 1);
            dmx_data->arg.pixel_fx.two_pixel_chase.departure = dmx_to_convert(p_profile->arg.two_pixel_chase.departure, 255, 1, 0, 128);

            switch(dmx_data->arg.pixel_fx.two_pixel_chase.departure)
            {
                case 0:
                    dmx_data->arg.pixel_fx.two_pixel_chase.move_choose.move_1_way = dmx_to_convert(p_profile->arg.two_pixel_chase.move_choose.move_1_way, 255, 2, 0, 60);
                    dmx_data->arg.pixel_fx.two_pixel_chase.way_mode.way1.pixel_1_mode = dmx_to_convert(p_profile->arg.two_pixel_chase.way_mode.way1.pixel_1_mode, 255, 1, 0, 10);

                    switch(dmx_data->arg.pixel_fx.two_pixel_chase.way_mode.way1.pixel_1_mode)
                    {
                        case DMX_FX_MODE_CCT:
                            dmx_data->arg.pixel_fx.two_pixel_chase.way_mode.way1.arg1.cct.cct = cct_8bit_calculate(p_profile->arg.two_pixel_chase.way_mode.way1.arg1.cct.cct, DMX_CCT_MAX, DMX_CCT_MIN);
                            dmx_data->arg.pixel_fx.two_pixel_chase.way_mode.way1.arg1.cct.duv = fx_dmx_gm_8bit_calculate(p_profile->arg.two_pixel_chase.way_mode.way1.arg1.cct.gm);
                            break;

                        case DMX_FX_MODE_HSI:
                            dmx_data->arg.pixel_fx.two_pixel_chase.way_mode.way1.arg1.hsi.cct = fx_cct_8bit_calculate(p_profile->arg.two_pixel_chase.way_mode.way1.arg1.hsi.cct, DMX_CCT_MAX, DMX_CCT_MIN);
                            dmx_data->arg.pixel_fx.two_pixel_chase.way_mode.way1.arg1.hsi.hue = 1 + (359 * p_profile->arg.two_pixel_chase.way_mode.way1.arg1.hsi.hue / 255.0f);
                            dmx_data->arg.pixel_fx.two_pixel_chase.way_mode.way1.arg1.hsi.sat = 100 * p_profile->arg.two_pixel_chase.way_mode.way1.arg1.hsi.sat / 255.0f;
                            break;


                    }

                    dmx_data->arg.pixel_fx.two_pixel_chase.way_mode.way1.pixel_2_mode = dmx_to_convert(p_profile->arg.two_pixel_chase.way_mode.way1.pixel_2_mode, 255, 1, 0, 10);

                    switch(dmx_data->arg.pixel_fx.two_pixel_chase.way_mode.way1.pixel_2_mode)
                    {
                        case DMX_FX_MODE_CCT:
                            dmx_data->arg.pixel_fx.two_pixel_chase.way_mode.way1.arg2.cct.cct = cct_8bit_calculate(p_profile->arg.two_pixel_chase.way_mode.way1.arg2.cct.cct, DMX_CCT_MAX, DMX_CCT_MIN);
                            dmx_data->arg.pixel_fx.two_pixel_chase.way_mode.way1.arg2.cct.duv = fx_dmx_gm_8bit_calculate(p_profile->arg.two_pixel_chase.way_mode.way1.arg2.cct.gm);
                            break;

                        case DMX_FX_MODE_HSI:
                            dmx_data->arg.pixel_fx.two_pixel_chase.way_mode.way1.arg2.hsi.cct = fx_cct_8bit_calculate(p_profile->arg.two_pixel_chase.way_mode.way1.arg2.hsi.cct, DMX_CCT_MAX, DMX_CCT_MIN);
                            dmx_data->arg.pixel_fx.two_pixel_chase.way_mode.way1.arg2.hsi.hue = 1 + (359 * p_profile->arg.two_pixel_chase.way_mode.way1.arg2.hsi.hue / 255.0f);
                            dmx_data->arg.pixel_fx.two_pixel_chase.way_mode.way1.arg2.hsi.sat = 100 * p_profile->arg.two_pixel_chase.way_mode.way1.arg2.hsi.sat / 255.0f;
                            break;


                    }

                    dmx_data->arg.pixel_fx.two_pixel_chase.way_mode.way1.background_color_int = p_profile->arg.two_pixel_chase.way_mode.way1.background_color_int *1000 / 255;
                           
                    dmx_data->arg.pixel_fx.two_pixel_chase.way_mode.way1.background_color_mode = dmx_to_convert(
                                p_profile->arg.two_pixel_chase.way_mode.way1.background_color_mode, 255, 1, 0, 10);

                    switch(dmx_data->arg.pixel_fx.two_pixel_chase.way_mode.way1.background_color_mode)
                    {
                        case DMX_FX_MODE_CCT:
                            dmx_data->arg.pixel_fx.two_pixel_chase.way_mode.way1.arg3.cct.cct = cct_8bit_calculate(p_profile->arg.two_pixel_chase.way_mode.way1.arg3.cct.cct, DMX_CCT_MAX, DMX_CCT_MIN);
                            dmx_data->arg.pixel_fx.two_pixel_chase.way_mode.way1.arg3.cct.duv = fx_dmx_gm_8bit_calculate(p_profile->arg.two_pixel_chase.way_mode.way1.arg3.cct.gm);
                            break;

                        case DMX_FX_MODE_HSI:
                            dmx_data->arg.pixel_fx.two_pixel_chase.way_mode.way1.arg3.hsi.cct = fx_cct_8bit_calculate(p_profile->arg.two_pixel_chase.way_mode.way1.arg3.hsi.cct, DMX_CCT_MAX, DMX_CCT_MIN);
                            dmx_data->arg.pixel_fx.two_pixel_chase.way_mode.way1.arg3.hsi.hue = 1 + (359 * p_profile->arg.two_pixel_chase.way_mode.way1.arg3.hsi.hue / 255.0f);
                            dmx_data->arg.pixel_fx.two_pixel_chase.way_mode.way1.arg3.hsi.sat = 100 * p_profile->arg.two_pixel_chase.way_mode.way1.arg3.hsi.sat / 255.0f;
                            break;


                    }

                    fx_state = p_profile->state < 10 ? DMX_FX_Trigger_Continue : DMX_FX_Trigger_None;
                    break;

                case 1:
                    dmx_data->arg.pixel_fx.two_pixel_chase.move_choose.move_2_way = dmx_to_convert(p_profile->arg.two_pixel_chase.move_choose.move_2_way, 255, 2, 0, 60);
                    dmx_data->arg.pixel_fx.two_pixel_chase.way_mode.way2.pixel_1_mode = dmx_to_convert(p_profile->arg.two_pixel_chase.way_mode.way2.pixel_1_mode, 255, 1, 0, 10);

                    switch(dmx_data->arg.pixel_fx.two_pixel_chase.way_mode.way2.pixel_1_mode)
                    {
                        case DMX_FX_MODE_CCT:
                            dmx_data->arg.pixel_fx.two_pixel_chase.way_mode.way2.arg1.cct.cct = cct_8bit_calculate(p_profile->arg.two_pixel_chase.way_mode.way2.arg1.cct.cct, DMX_CCT_MAX, DMX_CCT_MIN);
                            dmx_data->arg.pixel_fx.two_pixel_chase.way_mode.way2.arg1.cct.duv = fx_dmx_gm_8bit_calculate(p_profile->arg.two_pixel_chase.way_mode.way2.arg1.cct.gm);
                            break;

                        case DMX_FX_MODE_HSI:
                            dmx_data->arg.pixel_fx.two_pixel_chase.way_mode.way2.arg1.hsi.cct = fx_cct_8bit_calculate(p_profile->arg.two_pixel_chase.way_mode.way2.arg1.hsi.cct, DMX_CCT_MAX, DMX_CCT_MIN);
                            dmx_data->arg.pixel_fx.two_pixel_chase.way_mode.way2.arg1.hsi.hue = 1 + (359 * p_profile->arg.two_pixel_chase.way_mode.way2.arg1.hsi.hue / 255.0f);
                            dmx_data->arg.pixel_fx.two_pixel_chase.way_mode.way2.arg1.hsi.sat = 100 * p_profile->arg.two_pixel_chase.way_mode.way2.arg1.hsi.sat / 255.0f;
                            break;


                    }

                    dmx_data->arg.pixel_fx.two_pixel_chase.way_mode.way2.pixel_2_mode = dmx_to_convert(p_profile->arg.two_pixel_chase.way_mode.way2.pixel_2_mode, 255, 1, 0, 10);

                    switch(dmx_data->arg.pixel_fx.two_pixel_chase.way_mode.way2.pixel_2_mode)
                    {
                        case DMX_FX_MODE_CCT:
                            dmx_data->arg.pixel_fx.two_pixel_chase.way_mode.way2.arg2.cct.cct = cct_8bit_calculate(p_profile->arg.two_pixel_chase.way_mode.way2.arg2.cct.cct, DMX_CCT_MAX, DMX_CCT_MIN);
                            dmx_data->arg.pixel_fx.two_pixel_chase.way_mode.way2.arg2.cct.duv = fx_dmx_gm_8bit_calculate(p_profile->arg.two_pixel_chase.way_mode.way2.arg2.cct.gm);
                            break;

                        case DMX_FX_MODE_HSI:
                            dmx_data->arg.pixel_fx.two_pixel_chase.way_mode.way2.arg2.hsi.cct = fx_cct_8bit_calculate(p_profile->arg.two_pixel_chase.way_mode.way2.arg2.hsi.cct, DMX_CCT_MAX, DMX_CCT_MIN);
                            dmx_data->arg.pixel_fx.two_pixel_chase.way_mode.way2.arg2.hsi.hue = 1 + (359 * p_profile->arg.two_pixel_chase.way_mode.way2.arg2.hsi.hue / 255.0f);
                            dmx_data->arg.pixel_fx.two_pixel_chase.way_mode.way2.arg2.hsi.sat = 100 * p_profile->arg.two_pixel_chase.way_mode.way2.arg2.hsi.sat / 255.0f;
                            break;


                    }

                    dmx_data->arg.pixel_fx.two_pixel_chase.way_mode.way2.pixel_3_mode = dmx_to_convert(p_profile->arg.two_pixel_chase.way_mode.way2.pixel_3_mode, 255, 1, 0, 10);

                    switch(dmx_data->arg.pixel_fx.two_pixel_chase.way_mode.way2.pixel_3_mode)
                    {
                        case DMX_FX_MODE_CCT:
                            dmx_data->arg.pixel_fx.two_pixel_chase.way_mode.way2.arg3.cct.cct = cct_8bit_calculate(p_profile->arg.two_pixel_chase.way_mode.way2.arg3.cct.cct, DMX_CCT_MAX, DMX_CCT_MIN);
                            dmx_data->arg.pixel_fx.two_pixel_chase.way_mode.way2.arg3.cct.duv = fx_dmx_gm_8bit_calculate(p_profile->arg.two_pixel_chase.way_mode.way2.arg3.cct.gm);
                            break;

                        case DMX_FX_MODE_HSI:
                            dmx_data->arg.pixel_fx.two_pixel_chase.way_mode.way2.arg3.hsi.cct = fx_cct_8bit_calculate(p_profile->arg.two_pixel_chase.way_mode.way2.arg3.hsi.cct, DMX_CCT_MAX, DMX_CCT_MIN);
                            dmx_data->arg.pixel_fx.two_pixel_chase.way_mode.way2.arg3.hsi.hue = 1 + (359 * p_profile->arg.two_pixel_chase.way_mode.way2.arg3.hsi.hue / 255.0f);
                            dmx_data->arg.pixel_fx.two_pixel_chase.way_mode.way2.arg3.hsi.sat = 100 * p_profile->arg.two_pixel_chase.way_mode.way2.arg3.hsi.sat / 255.0f;
                            break;


                    }

                    dmx_data->arg.pixel_fx.two_pixel_chase.way_mode.way2.pixel_4_mode = dmx_to_convert(p_profile->arg.two_pixel_chase.way_mode.way2.pixel_4_mode, 255, 1, 0, 10);

                    switch(dmx_data->arg.pixel_fx.two_pixel_chase.way_mode.way2.pixel_4_mode)
                    {
                        case DMX_FX_MODE_CCT:
                            dmx_data->arg.pixel_fx.two_pixel_chase.way_mode.way2.arg4.cct.cct = cct_8bit_calculate(p_profile->arg.two_pixel_chase.way_mode.way2.arg4.cct.cct, DMX_CCT_MAX, DMX_CCT_MIN);
                            dmx_data->arg.pixel_fx.two_pixel_chase.way_mode.way2.arg4.cct.duv = fx_dmx_gm_8bit_calculate(p_profile->arg.two_pixel_chase.way_mode.way2.arg4.cct.gm);
                            break;

                        case DMX_FX_MODE_HSI:
                            dmx_data->arg.pixel_fx.two_pixel_chase.way_mode.way2.arg4.hsi.cct = fx_cct_8bit_calculate(p_profile->arg.two_pixel_chase.way_mode.way2.arg4.hsi.cct, DMX_CCT_MAX, DMX_CCT_MIN);
                            dmx_data->arg.pixel_fx.two_pixel_chase.way_mode.way2.arg4.hsi.hue = 1 + (359 * p_profile->arg.two_pixel_chase.way_mode.way2.arg4.hsi.hue / 255.0f);
                            dmx_data->arg.pixel_fx.two_pixel_chase.way_mode.way2.arg4.hsi.sat = 100 * p_profile->arg.two_pixel_chase.way_mode.way2.arg4.hsi.sat / 255.0f;
                            break;


                    }

                    dmx_data->arg.pixel_fx.two_pixel_chase.way_mode.way2.background_color_int = p_profile->arg.two_pixel_chase.way_mode.way2.background_color_int * 1000 /  255;
                    dmx_data->arg.pixel_fx.two_pixel_chase.way_mode.way2.background_color_mode = dmx_to_convert(
                                p_profile->arg.two_pixel_chase.way_mode.way2.background_color_mode, 255, 1, 0, 10);

                    switch(dmx_data->arg.pixel_fx.two_pixel_chase.way_mode.way2.background_color_mode)
                    {
                        case DMX_FX_MODE_CCT:
                            dmx_data->arg.pixel_fx.two_pixel_chase.way_mode.way2.arg5.cct.cct = cct_8bit_calculate(p_profile->arg.two_pixel_chase.way_mode.way2.arg5.cct.cct, DMX_CCT_MAX, DMX_CCT_MIN);
                            dmx_data->arg.pixel_fx.two_pixel_chase.way_mode.way2.arg5.cct.duv = fx_dmx_gm_8bit_calculate(p_profile->arg.two_pixel_chase.way_mode.way2.arg5.cct.gm);
                            break;

                        case DMX_FX_MODE_HSI:
                            dmx_data->arg.pixel_fx.two_pixel_chase.way_mode.way2.arg5.hsi.cct = fx_cct_8bit_calculate(p_profile->arg.two_pixel_chase.way_mode.way2.arg5.hsi.cct, DMX_CCT_MAX, DMX_CCT_MIN);
                            dmx_data->arg.pixel_fx.two_pixel_chase.way_mode.way2.arg5.hsi.hue = 1 + (359 * p_profile->arg.two_pixel_chase.way_mode.way2.arg5.hsi.hue / 255.0f);
                            dmx_data->arg.pixel_fx.two_pixel_chase.way_mode.way2.arg5.hsi.sat = 100 * p_profile->arg.two_pixel_chase.way_mode.way2.arg5.hsi.sat / 255.0f;
                            break;


                    }

                    break;

            }

            fx_state = p_profile->state < 10 ? DMX_FX_Trigger_Continue : DMX_FX_Trigger_None;
            break;

        case 4:
            dmx_data->arg.pixel_fx.type = DMX_FX_THREE_PIXEL_CHASE;
            dmx_data->arg.pixel_fx.three_pixel_chase.pixel_size = dmx_pixel_fx_colors_type_get(2, p_profile->arg.three_pixel_chase.pixel_size);
            dmx_data->arg.pixel_fx.three_pixel_chase.spd = dmx_to_linear_calculate(p_profile->arg.one_pixel_chase.spd, 255, 640, 1, 1);
            dmx_data->arg.pixel_fx.three_pixel_chase.departure = dmx_to_convert(p_profile->arg.three_pixel_chase.departure, 255, 1, 0, 128);

            switch(dmx_data->arg.pixel_fx.three_pixel_chase.departure)
            {
                case 0:
                    dmx_data->arg.pixel_fx.three_pixel_chase.move_choose.move_1_way = dmx_to_convert(p_profile->arg.three_pixel_chase.move_choose.move_1_way, 255, 2, 0, 60);
                    dmx_data->arg.pixel_fx.three_pixel_chase.way_mode.way1.pixel_1_mode = dmx_to_convert(p_profile->arg.three_pixel_chase.way_mode.way1.pixel_1_mode, 255, 1, 0, 10);

                    switch(dmx_data->arg.pixel_fx.three_pixel_chase.way_mode.way1.pixel_1_mode)
                    {
                        case DMX_FX_MODE_CCT:
                            dmx_data->arg.pixel_fx.three_pixel_chase.way_mode.way1.arg1.cct.cct = cct_8bit_calculate(p_profile->arg.three_pixel_chase.way_mode.way1.arg1.cct.cct, DMX_CCT_MAX, DMX_CCT_MIN);
                            dmx_data->arg.pixel_fx.three_pixel_chase.way_mode.way1.arg1.cct.duv = fx_dmx_gm_8bit_calculate(p_profile->arg.three_pixel_chase.way_mode.way1.arg1.cct.gm);
                            break;

                        case DMX_FX_MODE_HSI:
                            dmx_data->arg.pixel_fx.three_pixel_chase.way_mode.way1.arg1.hsi.cct = fx_cct_8bit_calculate(p_profile->arg.three_pixel_chase.way_mode.way1.arg1.hsi.cct, DMX_CCT_MAX, DMX_CCT_MIN);
                            dmx_data->arg.pixel_fx.three_pixel_chase.way_mode.way1.arg1.hsi.hue = 1 + (359 * p_profile->arg.three_pixel_chase.way_mode.way1.arg1.hsi.hue / 255.0f);
                            dmx_data->arg.pixel_fx.three_pixel_chase.way_mode.way1.arg1.hsi.sat = 100 * p_profile->arg.three_pixel_chase.way_mode.way1.arg1.hsi.sat / 255.0f;
                            break;


                    }

                    dmx_data->arg.pixel_fx.three_pixel_chase.way_mode.way1.pixel_2_mode = dmx_to_convert(p_profile->arg.three_pixel_chase.way_mode.way1.pixel_2_mode, 255, 1, 0, 10);

                    switch(dmx_data->arg.pixel_fx.three_pixel_chase.way_mode.way1.pixel_2_mode)
                    {
                        case DMX_FX_MODE_CCT:
                            dmx_data->arg.pixel_fx.three_pixel_chase.way_mode.way1.arg2.cct.cct = cct_8bit_calculate(p_profile->arg.three_pixel_chase.way_mode.way1.arg2.cct.cct, DMX_CCT_MAX, DMX_CCT_MIN);
                            dmx_data->arg.pixel_fx.three_pixel_chase.way_mode.way1.arg2.cct.duv = fx_dmx_gm_8bit_calculate(p_profile->arg.three_pixel_chase.way_mode.way1.arg2.cct.gm);
                            break;

                        case DMX_FX_MODE_HSI:
                            dmx_data->arg.pixel_fx.three_pixel_chase.way_mode.way1.arg2.hsi.cct = fx_cct_8bit_calculate(p_profile->arg.three_pixel_chase.way_mode.way1.arg2.hsi.cct, DMX_CCT_MAX, DMX_CCT_MIN);
                            dmx_data->arg.pixel_fx.three_pixel_chase.way_mode.way1.arg2.hsi.hue = 1 + (359 * p_profile->arg.three_pixel_chase.way_mode.way1.arg2.hsi.hue / 255.0f);
                            dmx_data->arg.pixel_fx.three_pixel_chase.way_mode.way1.arg2.hsi.sat = 100 * p_profile->arg.three_pixel_chase.way_mode.way1.arg2.hsi.sat / 255.0f;
                            break;


                    }

                    dmx_data->arg.pixel_fx.three_pixel_chase.way_mode.way1.pixel_3_mode = dmx_to_convert(p_profile->arg.three_pixel_chase.way_mode.way1.pixel_3_mode, 255, 1, 0, 10);

                    switch(dmx_data->arg.pixel_fx.three_pixel_chase.way_mode.way1.pixel_3_mode)
                    {
                        case DMX_FX_MODE_CCT:
                            dmx_data->arg.pixel_fx.three_pixel_chase.way_mode.way1.arg3.cct.cct = cct_8bit_calculate(p_profile->arg.three_pixel_chase.way_mode.way1.arg3.cct.cct, DMX_CCT_MAX, DMX_CCT_MIN);
                            dmx_data->arg.pixel_fx.three_pixel_chase.way_mode.way1.arg3.cct.duv = fx_dmx_gm_8bit_calculate(p_profile->arg.three_pixel_chase.way_mode.way1.arg3.cct.gm);
                            break;

                        case DMX_FX_MODE_HSI:
                            dmx_data->arg.pixel_fx.three_pixel_chase.way_mode.way1.arg3.hsi.cct = fx_cct_8bit_calculate(p_profile->arg.three_pixel_chase.way_mode.way1.arg3.hsi.cct, DMX_CCT_MAX, DMX_CCT_MIN);
                            dmx_data->arg.pixel_fx.three_pixel_chase.way_mode.way1.arg3.hsi.hue = 1 + (359 * p_profile->arg.three_pixel_chase.way_mode.way1.arg3.hsi.hue / 255.0f);
                            dmx_data->arg.pixel_fx.three_pixel_chase.way_mode.way1.arg3.hsi.sat = 100 * p_profile->arg.three_pixel_chase.way_mode.way1.arg3.hsi.sat / 255.0f;
                            break;


                    }

                    dmx_data->arg.pixel_fx.three_pixel_chase.way_mode.way1.background_color_int = p_profile->arg.three_pixel_chase.way_mode.way1.background_color_int *1000 /
                            255;

                    dmx_data->arg.pixel_fx.three_pixel_chase.way_mode.way1.background_color_mode = dmx_to_convert(
                                p_profile->arg.three_pixel_chase.way_mode.way1.background_color_mode, 255, 1, 0, 10);

                    switch(dmx_data->arg.pixel_fx.three_pixel_chase.way_mode.way1.background_color_mode)
                    {
                        case DMX_FX_MODE_CCT:
                            dmx_data->arg.pixel_fx.three_pixel_chase.way_mode.way1.arg4.cct.cct = cct_8bit_calculate(p_profile->arg.three_pixel_chase.way_mode.way1.arg4.cct.cct, DMX_CCT_MAX, DMX_CCT_MIN);
                            dmx_data->arg.pixel_fx.three_pixel_chase.way_mode.way1.arg4.cct.duv = fx_dmx_gm_8bit_calculate(p_profile->arg.three_pixel_chase.way_mode.way1.arg4.cct.gm);
                            break;

                        case DMX_FX_MODE_HSI:
                            dmx_data->arg.pixel_fx.three_pixel_chase.way_mode.way1.arg4.hsi.cct = fx_cct_8bit_calculate(p_profile->arg.three_pixel_chase.way_mode.way1.arg4.hsi.cct, DMX_CCT_MAX, DMX_CCT_MIN);
                            dmx_data->arg.pixel_fx.three_pixel_chase.way_mode.way1.arg4.hsi.hue = 1 + (359 * p_profile->arg.three_pixel_chase.way_mode.way1.arg4.hsi.hue / 255.0f);
                            dmx_data->arg.pixel_fx.three_pixel_chase.way_mode.way1.arg4.hsi.sat = 100 * p_profile->arg.three_pixel_chase.way_mode.way1.arg4.hsi.sat / 255.0f;
                            break;


                    }

                    fx_state = p_profile->state < 10 ? DMX_FX_Trigger_Continue : DMX_FX_Trigger_None;
                    break;

                case 1:
                    dmx_data->arg.pixel_fx.three_pixel_chase.move_choose.move_2_way = dmx_to_convert(p_profile->arg.three_pixel_chase.move_choose.move_2_way, 255, 2, 0, 60);
                    dmx_data->arg.pixel_fx.three_pixel_chase.way_mode.way2.pixel_1_mode = dmx_to_convert(p_profile->arg.three_pixel_chase.way_mode.way2.pixel_1_mode, 255, 1, 0, 10);

                    switch(dmx_data->arg.pixel_fx.three_pixel_chase.way_mode.way2.pixel_1_mode)
                    {
                        case DMX_FX_MODE_CCT:
                            dmx_data->arg.pixel_fx.three_pixel_chase.way_mode.way2.arg1.cct.cct = cct_8bit_calculate(p_profile->arg.three_pixel_chase.way_mode.way2.arg1.cct.cct, DMX_CCT_MAX, DMX_CCT_MIN);
                            dmx_data->arg.pixel_fx.three_pixel_chase.way_mode.way2.arg1.cct.duv = fx_dmx_gm_8bit_calculate(p_profile->arg.three_pixel_chase.way_mode.way2.arg1.cct.gm);
                            break;

                        case DMX_FX_MODE_HSI:
                            dmx_data->arg.pixel_fx.three_pixel_chase.way_mode.way2.arg1.hsi.cct = fx_cct_8bit_calculate(p_profile->arg.three_pixel_chase.way_mode.way2.arg1.hsi.cct, DMX_CCT_MAX, DMX_CCT_MIN);
                            dmx_data->arg.pixel_fx.three_pixel_chase.way_mode.way2.arg1.hsi.hue = 1 + (359 * p_profile->arg.three_pixel_chase.way_mode.way2.arg1.hsi.hue / 255.0f);
                            dmx_data->arg.pixel_fx.three_pixel_chase.way_mode.way2.arg1.hsi.sat = 100 * p_profile->arg.three_pixel_chase.way_mode.way2.arg1.hsi.sat / 255.0f;
                            break;


                    }

                    dmx_data->arg.pixel_fx.three_pixel_chase.way_mode.way2.pixel_2_mode = dmx_to_convert(p_profile->arg.three_pixel_chase.way_mode.way2.pixel_2_mode, 255, 1, 0, 10);

                    switch(dmx_data->arg.pixel_fx.three_pixel_chase.way_mode.way2.pixel_2_mode)
                    {
                        case DMX_FX_MODE_CCT:
                            dmx_data->arg.pixel_fx.three_pixel_chase.way_mode.way2.arg2.cct.cct = cct_8bit_calculate(p_profile->arg.three_pixel_chase.way_mode.way2.arg2.cct.cct, DMX_CCT_MAX, DMX_CCT_MIN);
                            dmx_data->arg.pixel_fx.three_pixel_chase.way_mode.way2.arg2.cct.duv = fx_dmx_gm_8bit_calculate(p_profile->arg.three_pixel_chase.way_mode.way2.arg2.cct.gm);
                            break;

                        case DMX_FX_MODE_HSI:
                            dmx_data->arg.pixel_fx.three_pixel_chase.way_mode.way2.arg2.hsi.cct = fx_cct_8bit_calculate(p_profile->arg.three_pixel_chase.way_mode.way2.arg2.hsi.cct, DMX_CCT_MAX, DMX_CCT_MIN);
                            dmx_data->arg.pixel_fx.three_pixel_chase.way_mode.way2.arg2.hsi.hue = 1 + (359 * p_profile->arg.three_pixel_chase.way_mode.way2.arg2.hsi.hue / 255.0f);
                            dmx_data->arg.pixel_fx.three_pixel_chase.way_mode.way2.arg2.hsi.sat = 100 * p_profile->arg.three_pixel_chase.way_mode.way2.arg2.hsi.sat / 255.0f;
                            break;


                    }

                    dmx_data->arg.pixel_fx.three_pixel_chase.way_mode.way2.pixel_3_mode = dmx_to_convert(p_profile->arg.three_pixel_chase.way_mode.way2.pixel_3_mode, 255, 1, 0, 10);

                    switch(dmx_data->arg.pixel_fx.three_pixel_chase.way_mode.way2.pixel_3_mode)
                    {
                        case DMX_FX_MODE_CCT:
                            dmx_data->arg.pixel_fx.three_pixel_chase.way_mode.way2.arg3.cct.cct = cct_8bit_calculate(p_profile->arg.three_pixel_chase.way_mode.way2.arg3.cct.cct, DMX_CCT_MAX, DMX_CCT_MIN);
                            dmx_data->arg.pixel_fx.three_pixel_chase.way_mode.way2.arg3.cct.duv = fx_dmx_gm_8bit_calculate(p_profile->arg.three_pixel_chase.way_mode.way2.arg3.cct.gm);
                            break;

                        case DMX_FX_MODE_HSI:
                            dmx_data->arg.pixel_fx.three_pixel_chase.way_mode.way2.arg3.hsi.cct = fx_cct_8bit_calculate(p_profile->arg.three_pixel_chase.way_mode.way2.arg3.hsi.cct, DMX_CCT_MAX, DMX_CCT_MIN);
                            dmx_data->arg.pixel_fx.three_pixel_chase.way_mode.way2.arg3.hsi.hue = 1 + (359 * p_profile->arg.three_pixel_chase.way_mode.way2.arg3.hsi.hue / 255.0f);
                            dmx_data->arg.pixel_fx.three_pixel_chase.way_mode.way2.arg3.hsi.sat = 100 * p_profile->arg.three_pixel_chase.way_mode.way2.arg3.hsi.sat / 255.0f;
                            break;


                    }

                    dmx_data->arg.pixel_fx.three_pixel_chase.way_mode.way2.pixel_4_mode = dmx_to_convert(p_profile->arg.three_pixel_chase.way_mode.way2.pixel_4_mode, 255, 1, 0, 10);

                    switch(dmx_data->arg.pixel_fx.three_pixel_chase.way_mode.way2.pixel_4_mode)
                    {
                        case DMX_FX_MODE_CCT:
                            dmx_data->arg.pixel_fx.three_pixel_chase.way_mode.way2.arg4.cct.cct = cct_8bit_calculate(p_profile->arg.three_pixel_chase.way_mode.way2.arg4.cct.cct, DMX_CCT_MAX, DMX_CCT_MIN);
                            dmx_data->arg.pixel_fx.three_pixel_chase.way_mode.way2.arg4.cct.duv = fx_dmx_gm_8bit_calculate(p_profile->arg.three_pixel_chase.way_mode.way2.arg4.cct.gm);
                            break;

                        case DMX_FX_MODE_HSI:
                            dmx_data->arg.pixel_fx.three_pixel_chase.way_mode.way2.arg4.hsi.cct = fx_cct_8bit_calculate(p_profile->arg.three_pixel_chase.way_mode.way2.arg4.hsi.cct, DMX_CCT_MAX, DMX_CCT_MIN);
                            dmx_data->arg.pixel_fx.three_pixel_chase.way_mode.way2.arg4.hsi.hue = 1 + (359 * p_profile->arg.three_pixel_chase.way_mode.way2.arg4.hsi.hue / 255.0f);
                            dmx_data->arg.pixel_fx.three_pixel_chase.way_mode.way2.arg4.hsi.sat = 100 * p_profile->arg.three_pixel_chase.way_mode.way2.arg4.hsi.sat / 255.0f;
                            break;


                    }

                    dmx_data->arg.pixel_fx.three_pixel_chase.way_mode.way2.pixel_5_mode = dmx_to_convert(p_profile->arg.three_pixel_chase.way_mode.way2.pixel_5_mode, 255, 1, 0, 10);

                    switch(dmx_data->arg.pixel_fx.three_pixel_chase.way_mode.way2.pixel_5_mode)
                    {
                        case DMX_FX_MODE_CCT:
                            dmx_data->arg.pixel_fx.three_pixel_chase.way_mode.way2.arg5.cct.cct = cct_8bit_calculate(p_profile->arg.three_pixel_chase.way_mode.way2.arg5.cct.cct, DMX_CCT_MAX, DMX_CCT_MIN);
                            dmx_data->arg.pixel_fx.three_pixel_chase.way_mode.way2.arg5.cct.duv = fx_dmx_gm_8bit_calculate(p_profile->arg.three_pixel_chase.way_mode.way2.arg5.cct.gm);
                            break;

                        case DMX_FX_MODE_HSI:
                            dmx_data->arg.pixel_fx.three_pixel_chase.way_mode.way2.arg5.hsi.cct = fx_cct_8bit_calculate(p_profile->arg.three_pixel_chase.way_mode.way2.arg5.hsi.cct, DMX_CCT_MAX, DMX_CCT_MIN);
                            dmx_data->arg.pixel_fx.three_pixel_chase.way_mode.way2.arg5.hsi.hue = 1 + (359 * p_profile->arg.three_pixel_chase.way_mode.way2.arg5.hsi.hue / 255.0f);
                            dmx_data->arg.pixel_fx.three_pixel_chase.way_mode.way2.arg5.hsi.sat = 100 * p_profile->arg.three_pixel_chase.way_mode.way2.arg5.hsi.sat / 255.0f;
                            break;


                    }

                    dmx_data->arg.pixel_fx.three_pixel_chase.way_mode.way2.pixel_6_mode = dmx_to_convert(p_profile->arg.three_pixel_chase.way_mode.way2.pixel_6_mode, 255, 1, 0, 10);

                    switch(dmx_data->arg.pixel_fx.three_pixel_chase.way_mode.way2.pixel_6_mode)
                    {
                        case DMX_FX_MODE_CCT:
                            dmx_data->arg.pixel_fx.three_pixel_chase.way_mode.way2.arg6.cct.cct = cct_8bit_calculate(p_profile->arg.three_pixel_chase.way_mode.way2.arg6.cct.cct, DMX_CCT_MAX, DMX_CCT_MIN);
                            dmx_data->arg.pixel_fx.three_pixel_chase.way_mode.way2.arg6.cct.duv = fx_dmx_gm_8bit_calculate(p_profile->arg.three_pixel_chase.way_mode.way2.arg6.cct.gm);
                            break;

                        case DMX_FX_MODE_HSI:
                            dmx_data->arg.pixel_fx.three_pixel_chase.way_mode.way2.arg6.hsi.cct = fx_cct_8bit_calculate(p_profile->arg.three_pixel_chase.way_mode.way2.arg6.hsi.cct, DMX_CCT_MAX, DMX_CCT_MIN);
                            dmx_data->arg.pixel_fx.three_pixel_chase.way_mode.way2.arg6.hsi.hue = 1 + (359 * p_profile->arg.three_pixel_chase.way_mode.way2.arg6.hsi.hue / 255.0f);
                            dmx_data->arg.pixel_fx.three_pixel_chase.way_mode.way2.arg6.hsi.sat = 100 * p_profile->arg.three_pixel_chase.way_mode.way2.arg6.hsi.sat / 255.0f;
                            break;


                    }

                    dmx_data->arg.pixel_fx.three_pixel_chase.way_mode.way2.background_color_int = p_profile->arg.three_pixel_chase.way_mode.way2.background_color_int *1000 /
                            255;

                    dmx_data->arg.pixel_fx.three_pixel_chase.way_mode.way2.background_color_mode = dmx_to_convert(
                                p_profile->arg.three_pixel_chase.way_mode.way2.background_color_mode, 255, 1, 0, 10);

                    switch(dmx_data->arg.pixel_fx.three_pixel_chase.way_mode.way2.background_color_mode)
                    {
                        case DMX_FX_MODE_CCT:
                            dmx_data->arg.pixel_fx.three_pixel_chase.way_mode.way2.arg7.cct.cct = cct_8bit_calculate(p_profile->arg.three_pixel_chase.way_mode.way2.arg7.cct.cct, DMX_CCT_MAX, DMX_CCT_MIN);
                            dmx_data->arg.pixel_fx.three_pixel_chase.way_mode.way2.arg7.cct.duv = fx_dmx_gm_8bit_calculate(p_profile->arg.three_pixel_chase.way_mode.way2.arg7.cct.gm);
                            break;

                        case DMX_FX_MODE_HSI:
                            dmx_data->arg.pixel_fx.three_pixel_chase.way_mode.way2.arg7.hsi.cct = fx_cct_8bit_calculate(p_profile->arg.three_pixel_chase.way_mode.way2.arg7.hsi.cct, DMX_CCT_MAX, DMX_CCT_MIN);
                            dmx_data->arg.pixel_fx.three_pixel_chase.way_mode.way2.arg7.hsi.hue = 1 + (359 * p_profile->arg.three_pixel_chase.way_mode.way2.arg7.hsi.hue / 255.0f);
                            dmx_data->arg.pixel_fx.three_pixel_chase.way_mode.way2.arg7.hsi.sat = 100 * p_profile->arg.three_pixel_chase.way_mode.way2.arg7.hsi.sat / 255.0f;
                            break;


                    }

                    break;

            }

            fx_state = p_profile->state < 10 ? DMX_FX_Trigger_Continue : DMX_FX_Trigger_None;
            break;

        case 5:
            dmx_data->arg.pixel_fx.type = DMX_FX_RAINBOW;
            dmx_data->arg.pixel_fx.rainbow.move = dmx_to_convert(p_profile->arg.rainbow.move, 255, 1, 0, 128);
            dmx_data->arg.pixel_fx.rainbow.spd = dmx_to_linear_calculate(p_profile->arg.rainbow.spd, 255, 10, 1, 1);
            fx_state = p_profile->state < 10 ? DMX_FX_Trigger_Continue : DMX_FX_Trigger_None;
            break;

        case 6:
        default:
            dmx_data->arg.pixel_fx.type = DMX_FX_PIXEL_FILE;
            dmx_data->arg.pixel_fx.pixel_file.lightness_lower =  dmx_data->lightness;
            dmx_data->arg.pixel_fx.pixel_file.lightness_upper = p_profile->arg.pixel_file.upper_limit_int *1000 / 255;
            dmx_data->arg.pixel_fx.pixel_file.frq =  dmx_to_linear_calculate(p_profile->arg.pixel_file.frq, 255, 10, 1, 1);
            dmx_data->arg.pixel_fx.pixel_file.direction = dmx_to_convert(p_profile->arg.pixel_file.move, 255, 1, 0, 128);
            dmx_data->arg.pixel_fx.pixel_file.fire_arg.mode = (enum dmx_light_mode)dmx_to_convert(p_profile->arg.pixel_file.fire_move, 255, 2, 0, 10);

            switch(dmx_data->arg.pixel_fx.pixel_file.fire_arg.mode)
            {

                case DMX_FX_MODE_CCT:
                    dmx_data->arg.pixel_fx.pixel_file.fire_arg.arg.cct.cct = cct_8bit_calculate(p_profile->arg.pixel_file.arg1.cct.cct, DMX_CCT_MAX, DMX_CCT_MIN);
                    dmx_data->arg.pixel_fx.pixel_file.fire_arg.arg.cct.duv = fx_dmx_gm_8bit_calculate(p_profile->arg.pixel_file.arg1.cct.gm);
                    break;

                case DMX_FX_MODE_HSI:
                    dmx_data->arg.pixel_fx.pixel_file.fire_arg.arg.hsi.cct = fx_cct_8bit_calculate(p_profile->arg.pixel_file.arg1.hsi.cct, DMX_CCT_MAX, DMX_CCT_MIN);
                    dmx_data->arg.pixel_fx.pixel_file.fire_arg.arg.hsi.hue = 1 + (359 * p_profile->arg.pixel_file.arg1.hsi.hue / 255.0f);
                    dmx_data->arg.pixel_fx.pixel_file.fire_arg.arg.hsi.sat = 100 * p_profile->arg.pixel_file.arg1.hsi.sat / 255.0f;
                    break;

                default:

                    break;

            }

            dmx_data->arg.pixel_fx.pixel_file.bg_lightness = p_profile->arg.pixel_file.background_color_int * 1000 / 255;
            dmx_data->arg.pixel_fx.pixel_file.bg_arg.mode = (enum dmx_light_mode)dmx_to_convert(p_profile->arg.pixel_file.background_color_mode, 255, 2, 0, 10);

            switch(dmx_data->arg.pixel_fx.pixel_file.bg_arg.mode)
            {

                case DMX_FX_MODE_CCT:
                    dmx_data->arg.pixel_fx.pixel_file.bg_arg.arg.cct.cct = cct_8bit_calculate(p_profile->arg.pixel_file.arg2.cct.cct, DMX_CCT_MAX, DMX_CCT_MIN);
                    dmx_data->arg.pixel_fx.pixel_file.bg_arg.arg.cct.duv = fx_dmx_gm_8bit_calculate(p_profile->arg.pixel_file.arg2.cct.gm);
                    break;

                case DMX_FX_MODE_HSI:
                    dmx_data->arg.pixel_fx.pixel_file.bg_arg.arg.hsi.cct = fx_cct_8bit_calculate(p_profile->arg.pixel_file.arg2.hsi.cct, DMX_CCT_MAX, DMX_CCT_MIN);
                    dmx_data->arg.pixel_fx.pixel_file.bg_arg.arg.hsi.hue = 1 + (359 * p_profile->arg.pixel_file.arg2.hsi.hue / 255.0f);
                    dmx_data->arg.pixel_fx.pixel_file.bg_arg.arg.hsi.sat = 100 * p_profile->arg.pixel_file.arg2.hsi.sat / 255.0f;
                    break;

                default:

                    break;

            }

            fx_state = p_profile->state < 10 ? DMX_FX_Trigger_Continue : DMX_FX_Trigger_None;
    }

    return fx_state;

}

uint8_t profile_0801_cct_and_rgb_8bit_analysis(const uint8_t* profile_data, dmx_data_t* dmx_data)
{
    if(NULL == profile_data || NULL == dmx_data)
        return 0;

    const profile_0801_cct_and_rgb_8bit_arg_t* p_profile = (profile_0801_cct_and_rgb_8bit_arg_t*)&profile_data[0];
    dmx_data->type = DMX_TYPE_MIXING;
    dmx_data->lightness = *(uint8_t*)profile_data  * 1000 / 255.0f;
    dmx_data->arg.mixing.ratio = p_profile->ratio * 10000 / 255;
    dmx_data->arg.mixing.color1_arg.mode = DMX_ColorMixingMode_CCT;
    dmx_data->arg.mixing.color1_arg.arg.cct.cct = cct_8bit_calculate(p_profile->cct, DMX_CCT_MAX, DMX_CCT_MIN);
    dmx_data->arg.mixing.color1_arg.arg.cct.duv = duv_8bit_calculate(p_profile->gm);
    dmx_data->arg.mixing.color2_arg.arg.rgb.red = p_profile->r / 255.0f;
    dmx_data->arg.mixing.color2_arg.arg.rgb.green = p_profile->g / 255.0f;
    dmx_data->arg.mixing.color2_arg.arg.rgb.blue = p_profile->b / 255.0f;
    dmx_data->arg.mixing.color2_arg.mode = DMX_ColorMixingMode_RGB;
    dmx_model_extend((uint8_t*)&profile_data[0], sizeof(profile_0801_cct_and_rgb_8bit_arg_t), DMX_8BIT_TYPE);
    return strobe_frq_calculate_8bit(p_profile->strobe);
}

uint8_t profile_0802_cct_and_rgb_16bit_analysis(const uint8_t* profile_data, dmx_data_t* dmx_data)
{
    if(NULL == profile_data || NULL == dmx_data)
        return 0;

    const profile_0802_cct_and_rgb_16bit_arg_t* p_profile = (profile_0802_cct_and_rgb_16bit_arg_t*)&profile_data[0];
    dmx_data->type = DMX_TYPE_MIXING;
    dmx_data->lightness = endian_Swap_16bit(profile_data)  * 1000 / 65535.0f;
    dmx_data->arg.mixing.ratio =endian_Swap_16bit((uint8_t*)& p_profile->ratio) * 10000 / 65535.0f;
    dmx_data->arg.mixing.color1_arg.mode = DMX_ColorMixingMode_CCT;
    dmx_data->arg.mixing.color1_arg.arg.cct.cct = cct_16bit_calculate(endian_Swap_16bit((uint8_t*)&p_profile->cct), DMX_CCT_MAX, DMX_CCT_MIN);
    dmx_data->arg.mixing.color1_arg.arg.cct.duv = duv_16bit_calculate(endian_Swap_16bit((uint8_t*)&p_profile->gm));
    dmx_data->arg.mixing.color2_arg.arg.rgb.red = endian_Swap_16bit((uint8_t*)&p_profile->r) / 65535.0f;
    dmx_data->arg.mixing.color2_arg.arg.rgb.green = endian_Swap_16bit((uint8_t*)&p_profile->g)  / 65535.0f;
    dmx_data->arg.mixing.color2_arg.arg.rgb.blue = endian_Swap_16bit((uint8_t*)&p_profile->b) / 65535.0f;
    dmx_data->arg.mixing.color2_arg.mode = DMX_ColorMixingMode_RGB;
    dmx_model_extend((uint8_t*)&profile_data[0], sizeof(profile_0802_cct_and_rgb_16bit_arg_t), DMX_16BIT_TYPE);
    return strobe_frq_calculate_8bit(p_profile->strobe);
}


uint8_t profile_0803_cct_and_rgbw_8bit_analysis(const uint8_t* profile_data, dmx_data_t* dmx_data)
{
    if(NULL == profile_data || NULL == dmx_data)
        return 0;

    const profile_0803_cct_and_rgbw_8bit_arg_t* p_profile = (profile_0803_cct_and_rgbw_8bit_arg_t*) &profile_data[0];
    dmx_data->type = DMX_TYPE_MIXING;
    dmx_data->lightness = *(uint8_t*)profile_data  * 1000 / 255.0f;
    dmx_data->arg.mixing.ratio = p_profile->ratio * 10000 / 255.0f;
    dmx_data->arg.mixing.color1_arg.mode = DMX_ColorMixingMode_CCT;
    dmx_data->arg.mixing.color1_arg.arg.cct.cct = cct_8bit_calculate(p_profile->cct, DMX_CCT_MAX, DMX_CCT_MIN);
    dmx_data->arg.mixing.color1_arg.arg.cct.duv = duv_8bit_calculate(p_profile->gm);
    dmx_data->arg.mixing.color2_arg.arg.rgb.red = p_profile->r / 255.0f;
    dmx_data->arg.mixing.color2_arg.arg.rgb.green = p_profile->g / 255.0f;
    dmx_data->arg.mixing.color2_arg.arg.rgb.blue = p_profile->b / 255.0f;
    dmx_data->arg.mixing.color2_arg.arg.rgb.ww =  p_profile->w / 255.0f;
    dmx_data->arg.mixing.color2_arg.arg.rgb.cw =  p_profile->w / 255.0f;
    dmx_data->arg.mixing.color2_arg.mode = DMX_ColorMixingMode_RGB;
    dmx_model_extend((uint8_t*)&profile_data[0], sizeof(profile_0803_cct_and_rgbw_8bit_arg_t), DMX_8BIT_TYPE);
    return strobe_frq_calculate_8bit(p_profile->strobe);
}


uint8_t profile_0804_cct_and_rgbw_16bit_analysis(const uint8_t* profile_data, dmx_data_t* dmx_data)
{
    if(NULL == profile_data || NULL == dmx_data)
        return 0;

    const profile_0804_cct_and_rgbw_16bit_arg_t* p_profile = (profile_0804_cct_and_rgbw_16bit_arg_t*) &profile_data[0];
    dmx_data->type = DMX_TYPE_MIXING;
    dmx_data->lightness = endian_Swap_16bit(profile_data)  * 1000 / 65535.0f;
    dmx_data->arg.mixing.ratio = endian_Swap_16bit((uint8_t*)&p_profile->ratio )* 10000 / 65535.0f;
    dmx_data->arg.mixing.color1_arg.mode = DMX_ColorMixingMode_CCT;
    dmx_data->arg.mixing.color1_arg.arg.cct.cct = cct_16bit_calculate(endian_Swap_16bit((uint8_t*)&p_profile->cct), DMX_CCT_MAX, DMX_CCT_MIN);
    dmx_data->arg.mixing.color1_arg.arg.cct.duv = duv_16bit_calculate(endian_Swap_16bit((uint8_t*)&p_profile->gm));
    dmx_data->arg.mixing.color2_arg.arg.rgb.red = endian_Swap_16bit((uint8_t*)&p_profile->r) / 65535.0f;
    dmx_data->arg.mixing.color2_arg.arg.rgb.green = endian_Swap_16bit((uint8_t*)&p_profile->g) / 65535.0f;
    dmx_data->arg.mixing.color2_arg.arg.rgb.blue = endian_Swap_16bit((uint8_t*)&p_profile->b) / 65535.0f;
    dmx_data->arg.mixing.color2_arg.arg.rgb.ww =  endian_Swap_16bit((uint8_t*)&p_profile->w) / 65535.0f;
    dmx_data->arg.mixing.color2_arg.arg.rgb.cw =  endian_Swap_16bit((uint8_t*)&p_profile->w) / 65535.0f;
    dmx_data->arg.mixing.color2_arg.mode = DMX_ColorMixingMode_RGB;

    dmx_model_extend((uint8_t*)&profile_data[0], sizeof(profile_0804_cct_and_rgbw_16bit_arg_t), DMX_16BIT_TYPE);
    return strobe_frq_calculate_8bit(p_profile->strobe);
}

uint8_t profile_0805_cct_and_rgbww_8bit_analysis(const uint8_t* profile_data, dmx_data_t* dmx_data)
{
    if(NULL == profile_data || NULL == dmx_data)
        return 0;

    const profile_0805_cct_and_rgbww_8bit_arg_t* p_profile = (profile_0805_cct_and_rgbww_8bit_arg_t*) &profile_data[0];
    dmx_data->type = DMX_TYPE_MIXING;
    dmx_data->lightness = (float)( *(uint8_t*)profile_data * 1000.0f )/ 255.0f;
    dmx_data->arg.mixing.ratio = p_profile->ratio * 10000 / 255.0f;
    dmx_data->arg.mixing.color1_arg.arg.cct.cct = cct_8bit_calculate(p_profile->cct, DMX_CCT_MAX, DMX_CCT_MIN);
    dmx_data->arg.mixing.color1_arg.arg.cct.duv = duv_8bit_calculate(p_profile->gm);
    dmx_data->arg.mixing.color1_arg.mode = DMX_ColorMixingMode_CCT;
    dmx_data->arg.mixing.color2_arg.arg.rgb.red = p_profile->r / 255.0f;
    dmx_data->arg.mixing.color2_arg.arg.rgb.green = p_profile->g / 255.0f;
    dmx_data->arg.mixing.color2_arg.arg.rgb.blue = p_profile->b / 255.0f;
    dmx_data->arg.mixing.color2_arg.arg.rgb.ww =  p_profile->ww / 255.0f;
    dmx_data->arg.mixing.color2_arg.arg.rgb.cw =  p_profile->cw / 255.0f;
    dmx_data->arg.mixing.color2_arg.mode = DMX_ColorMixingMode_RGB;
    dmx_model_extend((uint8_t*)&profile_data[0], sizeof(profile_0805_cct_and_rgbww_8bit_arg_t), DMX_8BIT_TYPE);
    return strobe_frq_calculate_8bit(p_profile->strobe);
}


uint8_t profile_0806_cct_and_rgbww_16bit_analysis(const uint8_t* profile_data, dmx_data_t* dmx_data)
{
    if(NULL == profile_data || NULL == dmx_data)
        return 0;

    const profile_0806_cct_and_rgbww_16bit_arg_t* p_profile = (profile_0806_cct_and_rgbww_16bit_arg_t*) &profile_data[0];
    dmx_data->type = DMX_TYPE_MIXING;
    dmx_data->arg.mixing.color1_arg.mode = DMX_ColorMixingMode_CCT;
    dmx_data->lightness = endian_Swap_16bit(profile_data)  * 1000 / 65535.0f;
    dmx_data->arg.mixing.ratio = endian_Swap_16bit((uint8_t*)&p_profile->ratio )* 10000 / 65535.0f;
    dmx_data->arg.mixing.color1_arg.arg.cct.cct = cct_16bit_calculate(endian_Swap_16bit((uint8_t*)&p_profile->cct), DMX_CCT_MAX, DMX_CCT_MIN);
    dmx_data->arg.mixing.color1_arg.arg.cct.duv = duv_16bit_calculate(endian_Swap_16bit((uint8_t*)&p_profile->gm));
    dmx_data->arg.mixing.color2_arg.arg.rgb.red = endian_Swap_16bit((uint8_t*)&p_profile->r) / 65535.0f;
    dmx_data->arg.mixing.color2_arg.arg.rgb.green = endian_Swap_16bit((uint8_t*)&p_profile->g) / 65535.0f;
    dmx_data->arg.mixing.color2_arg.arg.rgb.blue = endian_Swap_16bit((uint8_t*)&p_profile->b) / 65535.0f;
    dmx_data->arg.mixing.color2_arg.arg.rgb.ww =  p_profile->ww / 65535.0f;
    dmx_data->arg.mixing.color2_arg.arg.rgb.cw =  p_profile->cw / 65535.0f;
    dmx_data->arg.mixing.color2_arg.mode = DMX_ColorMixingMode_RGB;
    dmx_model_extend((uint8_t*)&profile_data[0], sizeof(profile_0806_cct_and_rgbww_16bit_arg_t), DMX_16BIT_TYPE);
    return strobe_frq_calculate_8bit(p_profile->strobe);
}

uint8_t profile_0807_cct_and_rgbacl_8bit_analysis(const uint8_t* profile_data, dmx_data_t* dmx_data)
{
    if(NULL == profile_data || NULL == dmx_data)
        return 0;

    const profile_0807_cct_and_rgbacl_8bit_arg_t* p_profile = (profile_0807_cct_and_rgbacl_8bit_arg_t*) &profile_data[0];
    dmx_data->type = DMX_TYPE_MIXING;
    dmx_data->lightness = *(uint8_t*)profile_data   * 1000 / 255.0f;
    dmx_data->arg.mixing.ratio = p_profile->ratio * 10000 / 255.0f;
    dmx_data->arg.mixing.color1_arg.mode = DMX_ColorMixingMode_CCT;
    dmx_data->arg.mixing.color1_arg.arg.cct.cct =   cct_8bit_calculate(p_profile->cct, DMX_CCT_MAX, DMX_CCT_MIN);
    dmx_data->arg.mixing.color1_arg.arg.cct.duv =   duv_8bit_calculate(p_profile->gm);
    dmx_data->arg.mixing.color2_arg.arg.rgb.red =   p_profile->r / 255.0f;
    dmx_data->arg.mixing.color2_arg.arg.rgb.green = p_profile->g / 255.0f;
    dmx_data->arg.mixing.color2_arg.arg.rgb.blue =  p_profile->b / 255.0f;
    dmx_data->arg.mixing.color2_arg.arg.rgb.amber = p_profile->amber / 255.0f;
    dmx_data->arg.mixing.color2_arg.arg.rgb.cyan =  p_profile->cyan  / 255.0f;
    dmx_data->arg.mixing.color2_arg.mode = DMX_ColorMixingMode_RGB;
    dmx_model_extend((uint8_t*)&profile_data[0], sizeof(profile_0807_cct_and_rgbacl_8bit_arg_t), DMX_8BIT_TYPE);
    return strobe_frq_calculate_8bit(p_profile->strobe);
}

uint8_t profile_0808_cct_and_rgbacl_16bit_analysis(const uint8_t* profile_data, dmx_data_t* dmx_data)
{
    if(NULL == profile_data || NULL == dmx_data)
        return 0;

    const profile_0808_cct_and_rgbacl_16bit_arg_t* p_profile = (profile_0808_cct_and_rgbacl_16bit_arg_t*) &profile_data[0];
    dmx_data->type = DMX_TYPE_MIXING;
    dmx_data->lightness = endian_Swap_16bit(profile_data)  * 1000 / 65535.0f;
    dmx_data->arg.mixing.ratio = endian_Swap_16bit((uint8_t*)&p_profile->ratio )* 10000 / 65535.0f;
    dmx_data->arg.mixing.color1_arg.arg.cct.cct = cct_16bit_calculate(endian_Swap_16bit((uint8_t*)&p_profile->cct), DMX_CCT_MAX, DMX_CCT_MIN);
    dmx_data->arg.mixing.color1_arg.arg.cct.duv = duv_16bit_calculate(endian_Swap_16bit((uint8_t*)&p_profile->gm));
    dmx_data->arg.mixing.color2_arg.arg.rgb.red = endian_Swap_16bit((uint8_t*)&p_profile->r) / 65535.0f;
    dmx_data->arg.mixing.color2_arg.arg.rgb.green = endian_Swap_16bit((uint8_t*)&p_profile->g) / 65535.0f;
    dmx_data->arg.mixing.color2_arg.arg.rgb.blue = endian_Swap_16bit((uint8_t*)&p_profile->b) / 65535.0f;
    dmx_data->arg.mixing.color2_arg.arg.rgb.amber =  endian_Swap_16bit((uint8_t*)&p_profile->amber) / 65535.0f;
    dmx_data->arg.mixing.color2_arg.arg.rgb.cyan =  endian_Swap_16bit((uint8_t*)&p_profile->cyan) / 65535.0f;
    dmx_data->arg.mixing.color2_arg.mode = DMX_ColorMixingMode_RGB;
    dmx_model_extend((uint8_t*)&profile_data[0], sizeof(profile_0808_cct_and_rgbacl_16bit_arg_t), DMX_16BIT_TYPE);
    return strobe_frq_calculate_8bit(p_profile->strobe);
}


uint8_t profile_0901_cct_and_hsi_8bit_analysis(const uint8_t* profile_data, dmx_data_t* dmx_data)
{
    if(NULL == profile_data || NULL == dmx_data)
        return 0;

    const profile_0901_cct_and_hsi_8bit_arg_t* p_profile = (profile_0901_cct_and_hsi_8bit_arg_t*) &profile_data[0];
    dmx_data->type = DMX_TYPE_MIXING;
    dmx_data->lightness = *(uint8_t*)profile_data  * 1000 / 255.0f;
    dmx_data->arg.mixing.color1_arg.mode = DMX_ColorMixingMode_CCT;
    dmx_data->arg.mixing.ratio = p_profile->ratio * 10000 / 255.0f;
    dmx_data->arg.mixing.color1_arg.arg.cct.cct = cct_8bit_calculate(p_profile->cct, DMX_CCT_MAX, DMX_CCT_MIN);
    dmx_data->arg.mixing.color1_arg.arg.cct.duv = duv_8bit_calculate(p_profile->gm);
    dmx_data->arg.mixing.color2_arg.arg.hsi.cct = cct_8bit_calculate(p_profile->hsi_cct, DMX_CCT_MAX, DMX_CCT_MIN);
    dmx_data->arg.mixing.color2_arg.arg.hsi.hue = 1 + (359.0f * p_profile->hue / 255.0f);
    dmx_data->arg.mixing.color2_arg.arg.hsi.sat = p_profile->sat * 100.0f / 255.0f;
    dmx_data->arg.mixing.color2_arg.mode = DMX_ColorMixingMode_HSI;
    dmx_model_extend((uint8_t*)&profile_data[0], sizeof(profile_0901_cct_and_hsi_8bit_arg_t), DMX_8BIT_TYPE);
    return strobe_frq_calculate_8bit(p_profile->strobe);
}


uint8_t profile_0902_cct_and_hsi_16bit_analysis(const uint8_t* profile_data, dmx_data_t* dmx_data)
{
    if(NULL == profile_data || NULL == dmx_data)
        return 0;

    const profile_0902_cct_and_hsi_16bit_arg_t* p_profile = (profile_0902_cct_and_hsi_16bit_arg_t*) &profile_data[0];
    dmx_data->type = DMX_TYPE_MIXING;
    dmx_data->lightness = endian_Swap_16bit(profile_data)  * 1000 / 65535.0f;
    dmx_data->arg.mixing.color1_arg.mode = DMX_ColorMixingMode_CCT;
    dmx_data->arg.mixing.ratio = endian_Swap_16bit((uint8_t*)&p_profile->ratio) * 10000 / 65535.0f;
    dmx_data->arg.mixing.color1_arg.arg.cct.cct = cct_16bit_calculate(endian_Swap_16bit((uint8_t*)&p_profile->cct), DMX_CCT_MAX, DMX_CCT_MIN);
    dmx_data->arg.mixing.color1_arg.arg.cct.duv = duv_16bit_calculate(endian_Swap_16bit((uint8_t*)&p_profile->gm));
    dmx_data->arg.mixing.color2_arg.arg.hsi.cct = cct_16bit_calculate(endian_Swap_16bit((uint8_t*)&p_profile->hsi_cct), DMX_CCT_MAX, DMX_CCT_MIN);
    dmx_data->arg.mixing.color2_arg.arg.hsi.hue = 1 + (359.0f * endian_Swap_16bit((uint8_t*)&p_profile->hue) / 65535.0f);
    dmx_data->arg.mixing.color2_arg.arg.hsi.sat = endian_Swap_16bit((uint8_t*)&p_profile->sat) * 100.0f / 65535.0f;
    dmx_data->arg.mixing.color2_arg.mode = DMX_ColorMixingMode_HSI;
    dmx_model_extend((uint8_t*)&profile_data[0], sizeof(profile_0902_cct_and_hsi_16bit_arg_t), DMX_16BIT_TYPE);
    return strobe_frq_calculate_8bit(p_profile->strobe);
}

uint8_t profile_2001_le_cct_and_rgb_8bit_analysis(const uint8_t* profile_data, dmx_data_t* dmx_data)
{
    if(NULL == profile_data || NULL == dmx_data)
        return 0;

    float total_lightness = 0;
    const profile_2001_le_cct_and_rgb_8bit_arg_t* p_profile = (profile_2001_le_cct_and_rgb_8bit_arg_t*)profile_data;
    dmx_data->type = DMX_TYPE_MIXING;

    for(uint8_t i = 0; i < PROFILE_LIGHT_ENGINE_NUM; i++)
    {

        dmx_data->arg.pixel_ctrl.pixel[i].mode = DMX_LIGHT_MODE_MIXING;

        dmx_data->arg.pixel_ctrl.pixel[i].lightness = p_profile->engine[i].lightness  * 1000 / 255.0f;
        total_lightness += dmx_data->arg.pixel_ctrl.pixel[i].lightness;
        dmx_data->arg.pixel_ctrl.pixel[i].arg.mixing.color1_arg.arg.cct.cct = cct_8bit_calculate(p_profile->engine[i].cct, DMX_CCT_MAX, DMX_CCT_MIN);
        dmx_data->arg.pixel_ctrl.pixel[i].arg.mixing.color1_arg.arg.cct.duv = duv_8bit_calculate(p_profile->engine[i].gm);
        dmx_data->arg.pixel_ctrl.pixel[i].arg.mixing.color1_arg.mode = DMX_ColorMixingMode_CCT;
        dmx_data->arg.pixel_ctrl.pixel[i].arg.mixing.ratio =  p_profile->engine[i].ratio * 10000 / 255.0f;
        dmx_data->arg.pixel_ctrl.pixel[i].arg.mixing.color2_arg.arg.rgb.red = p_profile->engine[i].r / 255.0f;
        dmx_data->arg.pixel_ctrl.pixel[i].arg.mixing.color2_arg.arg.rgb.green = p_profile->engine[i].g / 255.0f;
        dmx_data->arg.pixel_ctrl.pixel[i].arg.mixing.color2_arg.arg.rgb.blue = p_profile->engine[i].b / 255.0f;
        dmx_data->arg.pixel_ctrl.pixel[i].arg.mixing.color2_arg.mode = DMX_ColorMixingMode_RGB;
    }

    dmx_model_extend((uint8_t*)&profile_data[0], sizeof(profile_2001_le_cct_and_rgb_8bit_arg_t), DMX_8BIT_TYPE);
    return 0;
}



uint8_t profile_2002_le_cct_and_rgb_16bit_analysis(const uint8_t* profile_data, dmx_data_t* dmx_data)
{
    if(NULL == profile_data || NULL == dmx_data)
        return 0;

    float total_lightness = 0;
    const profile_2002_le_cct_and_rgb_16bit_arg_t* p_profile = (profile_2002_le_cct_and_rgb_16bit_arg_t*)profile_data;
    dmx_data->type = DMX_TYPE_MIXING;

    for(uint8_t i = 0; i < PROFILE_LIGHT_ENGINE_NUM; i++)
    {

        dmx_data->arg.pixel_ctrl.pixel[i].lightness = endian_Swap_16bit((uint8_t*)&p_profile->engine[i].lightness)  * 1000 / 65535.0f;
        total_lightness += dmx_data->arg.pixel_ctrl.pixel[i].lightness;
        dmx_data->arg.pixel_ctrl.pixel[i].mode = DMX_LIGHT_MODE_MIXING;
        dmx_data->arg.pixel_ctrl.pixel[i].arg.mixing.color1_arg.mode = DMX_ColorMixingMode_CCT;
        dmx_data->arg.pixel_ctrl.pixel[i].arg.mixing.color1_arg.arg.cct.cct  = cct_16bit_calculate(endian_Swap_16bit((uint8_t*)&p_profile->engine[i].cct), DMX_CCT_MAX, DMX_CCT_MIN);
        dmx_data->arg.pixel_ctrl.pixel[i].arg.mixing.color1_arg.arg.cct.duv = duv_16bit_calculate(endian_Swap_16bit((uint8_t*)&p_profile->engine[i].gm));
        dmx_data->arg.pixel_ctrl.pixel[i].arg.mixing.ratio =  endian_Swap_16bit((uint8_t*)&p_profile->engine[i].ratio) * 10000 / 65535.0f;
        dmx_data->arg.pixel_ctrl.pixel[i].arg.mixing.color2_arg.arg.rgb.red  = endian_Swap_16bit((uint8_t*)&p_profile->engine[i].r)  / 65535.0f;
        dmx_data->arg.pixel_ctrl.pixel[i].arg.mixing.color2_arg.arg.rgb.green = endian_Swap_16bit((uint8_t*)&p_profile->engine[i].g)  / 65535.0f;
        dmx_data->arg.pixel_ctrl.pixel[i].arg.mixing.color2_arg.arg.rgb.blue = endian_Swap_16bit((uint8_t*)&p_profile->engine[i].b)  / 65535.0f;
        dmx_data->arg.pixel_ctrl.pixel[i].arg.mixing.color2_arg.mode = DMX_ColorMixingMode_RGB;
    }

    dmx_model_extend((uint8_t*)&profile_data[0], sizeof(profile_2002_le_cct_and_rgb_16bit_arg_t), DMX_16BIT_TYPE);
    return 0;
}

uint8_t profile_2003_le_cct_and_rgbw_8bit_analysis(const uint8_t* profile_data, dmx_data_t* dmx_data)
{
    if(NULL == profile_data || NULL == dmx_data)
        return 0;

    float total_lightness = 0;
    const profile_2003_le_cct_and_rgbw_8bit_arg_t* p_profile = (profile_2003_le_cct_and_rgbw_8bit_arg_t*)profile_data;
    dmx_data->type = DMX_TYPE_PIXEL_CTRL;

    for(uint8_t i = 0; i < PROFILE_LIGHT_ENGINE_NUM; i++)
    {
        dmx_data->arg.pixel_ctrl.pixel[i].mode = DMX_LIGHT_MODE_MIXING;

        dmx_data->arg.pixel_ctrl.pixel[i].lightness = p_profile->engine[i].lightness * 1000  / 255.0f;
        total_lightness += dmx_data->arg.pixel_ctrl.pixel[i].lightness;
        dmx_data->arg.pixel_ctrl.pixel[i].arg.mixing.color1_arg.arg.cct.cct = cct_8bit_calculate(p_profile->engine[i].cct, DMX_CCT_MAX, DMX_CCT_MIN);
        dmx_data->arg.pixel_ctrl.pixel[i].arg.mixing.color1_arg.arg.cct.duv = duv_8bit_calculate(p_profile->engine[i].gm);

        dmx_data->arg.pixel_ctrl.pixel[i].arg.mixing.color1_arg.mode = DMX_ColorMixingMode_CCT;
        dmx_data->arg.pixel_ctrl.pixel[i].arg.mixing.ratio =  p_profile->engine[i].ratio * 10000 / 255.0f;
        dmx_data->arg.pixel_ctrl.pixel[i].arg.mixing.color2_arg.arg.rgb.red = p_profile->engine[i].r / 255.0f;
        dmx_data->arg.pixel_ctrl.pixel[i].arg.mixing.color2_arg.arg.rgb.green = p_profile->engine[i].g / 255.0f;
        dmx_data->arg.pixel_ctrl.pixel[i].arg.mixing.color2_arg.arg.rgb.blue = p_profile->engine[i].b / 255.0f;
        dmx_data->arg.pixel_ctrl.pixel[i].arg.mixing.color2_arg.arg.rgb.ww = p_profile->engine[i].w / 255.0f;
        dmx_data->arg.pixel_ctrl.pixel[i].arg.mixing.color2_arg.arg.rgb.cw = p_profile->engine[i].w / 255.0f;
        dmx_data->arg.pixel_ctrl.pixel[i].arg.mixing.color2_arg.mode = DMX_ColorMixingMode_RGB;
    }

    dmx_model_extend((uint8_t*)&profile_data[0], sizeof(profile_2003_le_cct_and_rgbw_8bit_arg_t), DMX_8BIT_TYPE);
    return 0;
}


uint8_t profile_2004_le_cct_and_rgbw_16bit_analysis(const uint8_t* profile_data, dmx_data_t* dmx_data)
{
    if(NULL == profile_data || NULL == dmx_data)
        return 0;

    float total_lightness = 0;
    const profile_2004_le_cct_and_rgbw_16bit_arg_t* p_profile = (profile_2004_le_cct_and_rgbw_16bit_arg_t*)profile_data;
    dmx_data->type = DMX_TYPE_MIXING;

    for(uint8_t i = 0; i < PROFILE_LIGHT_ENGINE_NUM; i++)
    {
        dmx_data->arg.pixel_ctrl.pixel[i].lightness = endian_Swap_16bit((uint8_t*)&p_profile->engine[i].lightness) * 1000  / 65535.0f;
        total_lightness += dmx_data->arg.pixel_ctrl.pixel[i].lightness;
        dmx_data->arg.pixel_ctrl.pixel[i].mode = DMX_LIGHT_MODE_MIXING;
        dmx_data->arg.pixel_ctrl.pixel[i].arg.mixing.color1_arg.mode = DMX_ColorMixingMode_CCT;
        dmx_data->lightness  = endian_Swap_16bit((uint8_t*)&p_profile->engine[i].lightness) / 65535.0f;
        dmx_data->arg.pixel_ctrl.pixel[i].arg.mixing.color1_arg.arg.cct.cct  = cct_16bit_calculate(endian_Swap_16bit((uint8_t*)&p_profile->engine[i].cct), DMX_CCT_MAX, DMX_CCT_MIN);
        dmx_data->arg.pixel_ctrl.pixel[i].arg.mixing.color1_arg.arg.cct.duv = duv_16bit_calculate(endian_Swap_16bit((uint8_t*)&p_profile->engine[i].gm));

        dmx_data->arg.pixel_ctrl.pixel[i].arg.mixing.ratio =  endian_Swap_16bit((uint8_t*)&p_profile->engine[i].ratio) * 10000 / 65535.0f;
        dmx_data->arg.pixel_ctrl.pixel[i].arg.mixing.color2_arg.arg.rgb.red  = endian_Swap_16bit((uint8_t*)&p_profile->engine[i].r)  / 65535.0f;
        dmx_data->arg.pixel_ctrl.pixel[i].arg.mixing.color2_arg.arg.rgb.green = endian_Swap_16bit((uint8_t*)&p_profile->engine[i].g)  / 65535.0f;
        dmx_data->arg.pixel_ctrl.pixel[i].arg.mixing.color2_arg.arg.rgb.blue = endian_Swap_16bit((uint8_t*)&p_profile->engine[i].b)  / 65535.0f;
        dmx_data->arg.pixel_ctrl.pixel[i].arg.mixing.color2_arg.arg.rgb.ww = endian_Swap_16bit((uint8_t*)&p_profile->engine[i].w)  / 65535.0f;
        dmx_data->arg.pixel_ctrl.pixel[i].arg.mixing.color2_arg.arg.rgb.cw = endian_Swap_16bit((uint8_t*)&p_profile->engine[i].w)  / 65535.0f;
        dmx_data->arg.pixel_ctrl.pixel[i].arg.mixing.color2_arg.mode = DMX_ColorMixingMode_RGB;
    }

    dmx_model_extend((uint8_t*)&profile_data[0], sizeof(profile_2004_le_cct_and_rgbw_16bit_arg_t), DMX_16BIT_TYPE);
    return 0;
}

uint8_t profile_2005_le_cct_and_rgbww_8bit_analysis(const uint8_t* profile_data, dmx_data_t* dmx_data)
{
    if(NULL == profile_data || NULL == dmx_data)
        return 0;

    float total_lightness = 0;
    const profile_2005_le_cct_and_rgbww_8bit_arg_t* p_profile = (profile_2005_le_cct_and_rgbww_8bit_arg_t*)profile_data;
    dmx_data->type = DMX_TYPE_MIXING;

    for(uint8_t i = 0; i < PROFILE_LIGHT_ENGINE_NUM; i++)
    {

        dmx_data->arg.pixel_ctrl.pixel[i].mode = DMX_LIGHT_MODE_MIXING;
        dmx_data->arg.pixel_ctrl.pixel[i].lightness = p_profile->engine[i].lightness  * 1000 / 255.0f;
        total_lightness += dmx_data->arg.pixel_ctrl.pixel[i].lightness;
        dmx_data->arg.pixel_ctrl.pixel[i].arg.mixing.color1_arg.arg.cct.cct = cct_8bit_calculate(p_profile->engine[i].cct, DMX_CCT_MAX, DMX_CCT_MIN);
        dmx_data->arg.pixel_ctrl.pixel[i].arg.mixing.color1_arg.arg.cct.duv = duv_8bit_calculate(p_profile->engine[i].gm);

        dmx_data->arg.pixel_ctrl.pixel[i].arg.mixing.color1_arg.mode = DMX_ColorMixingMode_CCT;
        dmx_data->arg.pixel_ctrl.pixel[i].arg.mixing.ratio =  p_profile->engine[i].ratio * 10000 / 255.0f;
        dmx_data->arg.pixel_ctrl.pixel[i].arg.mixing.color2_arg.arg.rgb.red = p_profile->engine[i].r / 255.0f;
        dmx_data->arg.pixel_ctrl.pixel[i].arg.mixing.color2_arg.arg.rgb.green = p_profile->engine[i].g / 255.0f;
        dmx_data->arg.pixel_ctrl.pixel[i].arg.mixing.color2_arg.arg.rgb.blue = p_profile->engine[i].b / 255.0f;
        dmx_data->arg.pixel_ctrl.pixel[i].arg.mixing.color2_arg.arg.rgb.ww = p_profile->engine[i].ww / 255.0f;
        dmx_data->arg.pixel_ctrl.pixel[i].arg.mixing.color2_arg.arg.rgb.cw = p_profile->engine[i].cw / 255.0f;
        dmx_data->arg.pixel_ctrl.pixel[i].arg.mixing.color2_arg.mode = DMX_ColorMixingMode_RGB;
    }

    dmx_model_extend((uint8_t*)&profile_data[0], sizeof(profile_2005_le_cct_and_rgbww_8bit_arg_t), DMX_8BIT_TYPE);
    return 0;
}



uint8_t profile_2006_le_cct_and_rgbww_16bit_analysis(const uint8_t* profile_data, dmx_data_t* dmx_data)
{
    if(NULL == profile_data || NULL == dmx_data)
        return 0;

    float total_lightness = 0;
    const profile_2006_le_cct_and_rgbww_16bit_arg_t* p_profile = (profile_2006_le_cct_and_rgbww_16bit_arg_t*) &profile_data[0];
    dmx_data->type = DMX_TYPE_MIXING;

    for(uint8_t i = 0; i < PROFILE_LIGHT_ENGINE_NUM; i++)
    {
        dmx_data->arg.pixel_ctrl.pixel[i].mode = DMX_LIGHT_MODE_MIXING;
        dmx_data->arg.pixel_ctrl.pixel[i].arg.mixing.color1_arg.mode = DMX_ColorMixingMode_CCT;
        dmx_data->arg.pixel_ctrl.pixel[i].lightness = endian_Swap_16bit((uint8_t*)&p_profile->engine[i].lightness) * 1000  / 65535.0f;
        total_lightness += dmx_data->arg.pixel_ctrl.pixel[i].lightness;
        dmx_data->arg.pixel_ctrl.pixel[i].arg.mixing.color1_arg.arg.cct.cct  = cct_16bit_calculate(endian_Swap_16bit((uint8_t*)&p_profile->engine[i].cct), DMX_CCT_MAX, DMX_CCT_MIN);
        dmx_data->arg.pixel_ctrl.pixel[i].arg.mixing.color1_arg.arg.cct.duv = duv_16bit_calculate(endian_Swap_16bit((uint8_t*)&p_profile->engine[i].gm));

        dmx_data->arg.pixel_ctrl.pixel[i].arg.mixing.ratio =  endian_Swap_16bit((uint8_t*)&p_profile->engine[i].ratio) * 10000 / 65535.0f;
        dmx_data->arg.pixel_ctrl.pixel[i].arg.mixing.color2_arg.arg.rgb.red  = endian_Swap_16bit((uint8_t*)&p_profile->engine[i].r)  / 65535.0f;
        dmx_data->arg.pixel_ctrl.pixel[i].arg.mixing.color2_arg.arg.rgb.green = endian_Swap_16bit((uint8_t*)&p_profile->engine[i].g)  / 65535.0f;
        dmx_data->arg.pixel_ctrl.pixel[i].arg.mixing.color2_arg.arg.rgb.blue = endian_Swap_16bit((uint8_t*)&p_profile->engine[i].b)  / 65535.0f;
        dmx_data->arg.pixel_ctrl.pixel[i].arg.mixing.color2_arg.arg.rgb.ww = endian_Swap_16bit((uint8_t*)&p_profile->engine[i].ww)  / 65535.0f;
        dmx_data->arg.pixel_ctrl.pixel[i].arg.mixing.color2_arg.arg.rgb.cw = endian_Swap_16bit((uint8_t*)&p_profile->engine[i].cw)  / 65535.0f;
        dmx_data->arg.pixel_ctrl.pixel[i].arg.mixing.color2_arg.mode = DMX_ColorMixingMode_RGB;
    }

    dmx_model_extend((uint8_t*)&profile_data[0], sizeof(profile_2006_le_cct_and_rgbww_16bit_arg_t), DMX_16BIT_TYPE);
    return 0;
}


uint8_t profile_2007_le_cct_and_rgbacl_8bit_analysis(const uint8_t* profile_data, dmx_data_t* dmx_data)
{
    if(NULL == profile_data || NULL == dmx_data)
        return 0;

    float total_lightness = 0;
    const profile_2007_le_cct_and_rgbacl_8bit_arg_t* p_profile = (profile_2007_le_cct_and_rgbacl_8bit_arg_t*)profile_data;
    dmx_data->type = DMX_TYPE_MIXING;

    for(uint8_t i = 0; i < PROFILE_LIGHT_ENGINE_NUM; i++)
    {
        dmx_data->arg.pixel_ctrl.pixel[i].mode = DMX_LIGHT_MODE_MIXING;
        dmx_data->arg.pixel_ctrl.pixel[i].lightness = p_profile->engine[i].lightness  * 1000 / 255.0f;
        total_lightness += dmx_data->arg.pixel_ctrl.pixel[i].lightness;
        dmx_data->arg.pixel_ctrl.pixel[i].arg.mixing.color1_arg.arg.cct.cct = cct_8bit_calculate(p_profile->engine[i].cct, DMX_CCT_MAX, DMX_CCT_MIN);
        dmx_data->arg.pixel_ctrl.pixel[i].arg.mixing.color1_arg.arg.cct.duv = duv_8bit_calculate(p_profile->engine[i].gm);

        dmx_data->arg.pixel_ctrl.pixel[i].arg.mixing.color1_arg.mode = DMX_ColorMixingMode_CCT;
        dmx_data->arg.pixel_ctrl.pixel[i].arg.mixing.ratio =  p_profile->engine[i].ratio * 10000 / 255.0f;
        dmx_data->arg.pixel_ctrl.pixel[i].arg.mixing.color2_arg.arg.rgb.red = p_profile->engine[i].r / 255.0f;
        dmx_data->arg.pixel_ctrl.pixel[i].arg.mixing.color2_arg.arg.rgb.green = p_profile->engine[i].g / 255.0f;
        dmx_data->arg.pixel_ctrl.pixel[i].arg.mixing.color2_arg.arg.rgb.blue = p_profile->engine[i].b / 255.0f;
        dmx_data->arg.pixel_ctrl.pixel[i].arg.mixing.color2_arg.arg.rgb.amber = p_profile->engine[i].amber / 255.0f;
        dmx_data->arg.pixel_ctrl.pixel[i].arg.mixing.color2_arg.arg.rgb.cyan = p_profile->engine[i].cyan / 255.0f;
        dmx_data->arg.pixel_ctrl.pixel[i].arg.mixing.color2_arg.mode = DMX_ColorMixingMode_RGB;
    }

    dmx_model_extend((uint8_t*)&profile_data[0], sizeof(profile_2007_le_cct_and_rgbacl_8bit_arg_t), DMX_8BIT_TYPE);
    return 0;
}



uint8_t profile_2008_le_cct_and_rgbacl_16bit_analysis(const uint8_t* profile_data, dmx_data_t* dmx_data)
{
    if(NULL == profile_data || NULL == dmx_data)
        return 0;

    float total_lightness = 0;
    const profile_2008_le_cct_and_rgbacl_16bit_arg_t* p_profile = (profile_2008_le_cct_and_rgbacl_16bit_arg_t*)profile_data;
    dmx_data->type = DMX_TYPE_MIXING;

    for(uint8_t i = 0; i < PROFILE_LIGHT_ENGINE_NUM; i++)
    {
        dmx_data->arg.pixel_ctrl.pixel[i].mode = DMX_LIGHT_MODE_MIXING;
        dmx_data->arg.pixel_ctrl.pixel[i].arg.mixing.color1_arg.mode = DMX_ColorMixingMode_CCT;
        dmx_data->arg.pixel_ctrl.pixel[i].lightness = endian_Swap_16bit((uint8_t*)&p_profile->engine[i].lightness)  * 1000 / 65535.0f;
        total_lightness += dmx_data->arg.pixel_ctrl.pixel[i].lightness;
        dmx_data->arg.pixel_ctrl.pixel[i].arg.mixing.color1_arg.arg.cct.cct  = cct_16bit_calculate(endian_Swap_16bit((uint8_t*)&p_profile->engine[i].cct), DMX_CCT_MAX, DMX_CCT_MIN);
        dmx_data->arg.pixel_ctrl.pixel[i].arg.mixing.color1_arg.arg.cct.duv = duv_16bit_calculate(endian_Swap_16bit((uint8_t*)&p_profile->engine[i].gm));

        dmx_data->arg.pixel_ctrl.pixel[i].arg.mixing.ratio =  endian_Swap_16bit((uint8_t*)&p_profile->engine[i].ratio) * 10000 / 65535.0f;
        dmx_data->arg.pixel_ctrl.pixel[i].arg.mixing.color2_arg.arg.rgb.red  = endian_Swap_16bit((uint8_t*)&p_profile->engine[i].r)  / 65535.0f;
        dmx_data->arg.pixel_ctrl.pixel[i].arg.mixing.color2_arg.arg.rgb.green = endian_Swap_16bit((uint8_t*)&p_profile->engine[i].g)  / 65535.0f;
        dmx_data->arg.pixel_ctrl.pixel[i].arg.mixing.color2_arg.arg.rgb.blue = endian_Swap_16bit((uint8_t*)&p_profile->engine[i].b)  / 65535.0f;
        dmx_data->arg.pixel_ctrl.pixel[i].arg.mixing.color2_arg.arg.rgb.amber = endian_Swap_16bit((uint8_t*)&p_profile->engine[i].amber)  / 65535.0f;
        dmx_data->arg.pixel_ctrl.pixel[i].arg.mixing.color2_arg.arg.rgb.cyan = endian_Swap_16bit((uint8_t*)&p_profile->engine[i].cyan)  / 65535.0f;
        dmx_data->arg.pixel_ctrl.pixel[i].arg.mixing.color2_arg.mode = DMX_ColorMixingMode_RGB;
    }

    dmx_model_extend((uint8_t*)&profile_data[0], sizeof(profile_2008_le_cct_and_rgbacl_16bit_arg_t), DMX_16BIT_TYPE);
    return 0;
}




uint8_t profile_2101_le_hsi_8bit_analysis(const uint8_t* profile_data, dmx_data_t* dmx_data)
{
    if(NULL == profile_data || NULL == dmx_data)
        return 0;

    float total_lightness = 0;
    const profile_2101_le_hsi_8bit_arg_t* p_profile = (profile_2101_le_hsi_8bit_arg_t*)profile_data;
    dmx_data->type = DMX_TYPE_MIXING;

    for(uint8_t i = 0; i < PROFILE_LIGHT_ENGINE_NUM; i++)
    {

        dmx_data->arg.pixel_ctrl.pixel[i].lightness = p_profile->engine[i].lightness  * 1000 / 255.0f;
        total_lightness += dmx_data->arg.pixel_ctrl.pixel[i].lightness;

        dmx_data->arg.pixel_ctrl.pixel[i].mode = DMX_LIGHT_MODE_HSI;
        dmx_data->arg.pixel_ctrl.pixel[i].arg.hsi.hue = 1 + (359 * p_profile->engine[i].hue) / 255.0f;
        dmx_data->arg.pixel_ctrl.pixel[i].arg.hsi.sat = (100.0f * p_profile->engine[i].sat) / 255.0f;
    }

    dmx_model_extend((uint8_t*)&profile_data[0], sizeof(profile_2101_le_hsi_8bit_arg_t), DMX_8BIT_TYPE);
    return 0;
}

uint8_t profile_2102_le_hsi_16bit_analysis(const uint8_t* profile_data, dmx_data_t* dmx_data)
{
    if(NULL == profile_data || NULL == dmx_data)
        return 0;

    float total_lightness = 0;
    const profile_2102_le_hsi_16bit_arg_t* p_profile = (profile_2102_le_hsi_16bit_arg_t*)profile_data;
    dmx_data->type = DMX_TYPE_MIXING;

    for(uint8_t i = 0; i <  PROFILE_LIGHT_ENGINE_NUM; i++)
    {
        dmx_data->arg.pixel_ctrl.pixel[i].mode = DMX_LIGHT_MODE_HSI;
        dmx_data->arg.pixel_ctrl.pixel[i].lightness = endian_Swap_16bit((uint8_t*)&p_profile->engine[i].lightness)  * 1000 / 65535.0f;
        total_lightness += dmx_data->arg.pixel_ctrl.pixel[i].lightness;
        dmx_data->arg.pixel_ctrl.pixel[i].arg.hsi.hue = 1 + (359.0f * endian_Swap_16bit((uint8_t*)&p_profile->engine[i].hue) / 65535.0f);
        dmx_data->arg.pixel_ctrl.pixel[i].arg.hsi.sat = endian_Swap_16bit((uint8_t*)&p_profile->engine[i].sat) * 100.0f / 65535.0f;

    }

    dmx_data->lightness = total_lightness / PROFILE_LIGHT_ENGINE_NUM;
    dmx_model_extend((uint8_t*)&profile_data[0], sizeof(profile_2102_le_hsi_16bit_arg_t), DMX_16BIT_TYPE);
    return 0;
}

uint8_t profile_2103_le_advhsi_8bit_analysis(const uint8_t* profile_data, dmx_data_t* dmx_data)
{
    if(NULL == profile_data || NULL == dmx_data)
        return 0;

    float total_lightness = 0;
    const profile_2103_le_advhsi_8bit_arg_t* p_profile = (profile_2103_le_advhsi_8bit_arg_t*)profile_data;
    dmx_data->type = DMX_TYPE_MIXING;

    for(uint8_t i = 0; i <  PROFILE_LIGHT_ENGINE_NUM; i++)
    {

        dmx_data->arg.pixel_ctrl.pixel[i].lightness = p_profile->engine[i].lightness * 1000  / 255.0f;
        total_lightness += dmx_data->arg.pixel_ctrl.pixel[i].lightness;
        dmx_data->arg.pixel_ctrl.pixel[i].mode = DMX_LIGHT_MODE_HSI;
        dmx_data->arg.pixel_ctrl.pixel[i].arg.hsi.hue = 1 + (359 * p_profile->engine[i].hue) / 255.0f;
        dmx_data->arg.pixel_ctrl.pixel[i].arg.hsi.sat = (100.0f * p_profile->engine[i].sat) / 255.0f;
        dmx_data->arg.pixel_ctrl.pixel[i].arg.hsi.cct  = cct_8bit_calculate(p_profile->engine[i].cct, DMX_CCT_MAX, DMX_CCT_MIN);
    }

    dmx_model_extend((uint8_t*)&profile_data[0], sizeof(profile_2103_le_advhsi_8bit_arg_t), DMX_8BIT_TYPE);
    return 0;
}

uint8_t profile_2104_le_advhsi_16bit_analysis(const uint8_t* profile_data, dmx_data_t* dmx_data)
{
    if(NULL == profile_data || NULL == dmx_data)
        return 0;

    float total_lightness = 0;
    const profile_2104_le_advhsi_16bit_arg_t* p_profile = (profile_2104_le_advhsi_16bit_arg_t*)profile_data;
    dmx_data->type = DMX_TYPE_MIXING;

    for(uint8_t i = 0; i <  PROFILE_LIGHT_ENGINE_NUM; i++)
    {
        dmx_data->arg.pixel_ctrl.pixel[i].mode = DMX_LIGHT_MODE_HSI;
        dmx_data->arg.pixel_ctrl.pixel[i].lightness = endian_Swap_16bit((uint8_t*)&p_profile->engine[i].lightness)  * 1000 / 65535.0f;
        total_lightness += dmx_data->arg.pixel_ctrl.pixel[i].lightness;
        dmx_data->arg.pixel_ctrl.pixel[i].arg.hsi.hue = 1 + (359 * endian_Swap_16bit((uint8_t*)&p_profile->engine[i].hue) / 65535.0f);
        dmx_data->arg.pixel_ctrl.pixel[i].arg.hsi.sat = endian_Swap_16bit((uint8_t*)&p_profile->engine[i].sat) * 100.f / 65535.0f;
        dmx_data->arg.pixel_ctrl.pixel[i].arg.hsi.cct = cct_16bit_calculate(endian_Swap_16bit((uint8_t*)&p_profile->engine[i].cct), DMX_CCT_MAX, DMX_CCT_MIN);
    }

    dmx_model_extend((uint8_t*)&profile_data[0], sizeof(profile_2104_le_advhsi_16bit_arg_t), DMX_16BIT_TYPE);
    return 0;
}


uint8_t profile_2201_le_xy_8bit_analysis(const uint8_t* profile_data, dmx_data_t* dmx_data)
{
    if(NULL == profile_data || NULL == dmx_data)
        return 0;

    float total_lightness = 0;
    const profile_2201_le_xy_8bit_arg_t* p_profile = (profile_2201_le_xy_8bit_arg_t*)profile_data;
    dmx_data->type = DMX_TYPE_MIXING;

    for(uint8_t i = 0; i < PROFILE_LIGHT_ENGINE_NUM; i++)
    {
        dmx_data->arg.pixel_ctrl.pixel[i].mode = DMX_LIGHT_MODE_XY;
        dmx_data->arg.pixel_ctrl.pixel[i].lightness = p_profile->engine[i].lightness  * 1000 / 255.0f;
        total_lightness += dmx_data->arg.pixel_ctrl.pixel[i].lightness;
        dmx_data->arg.pixel_ctrl.pixel[i].arg.xy.x = p_profile->engine[i].x  / 255.0f * 0.8f;
        dmx_data->arg.pixel_ctrl.pixel[i].arg.xy.y = p_profile->engine[i].y / 255.0f * 0.8f;

    }

    dmx_model_extend((uint8_t*)&profile_data[0], sizeof(profile_2201_le_xy_8bit_arg_t), DMX_8BIT_TYPE);
    return 0;
}


uint8_t profile_2202_le_xy_16bit_analysis(const uint8_t* profile_data, dmx_data_t* dmx_data)
{
    if(NULL == profile_data || NULL == dmx_data)
        return 0;

    float total_lightness = 0;
    const profile_2202_le_xy_16bit_arg_t* p_profile = (profile_2202_le_xy_16bit_arg_t*)profile_data;
    dmx_data->type = DMX_TYPE_MIXING;

    for(uint8_t i = 0; i < PROFILE_LIGHT_ENGINE_NUM; i++)
    {
        dmx_data->arg.pixel_ctrl.pixel[i].mode = DMX_LIGHT_MODE_XY;
        dmx_data->arg.pixel_ctrl.pixel[i].lightness = endian_Swap_16bit((uint8_t*)&p_profile->engine[i].lightness) * 1000  / 65535.0f;
        total_lightness += dmx_data->arg.pixel_ctrl.pixel[i].lightness;
        dmx_data->arg.pixel_ctrl.pixel[i].arg.xy.x = endian_Swap_16bit((uint8_t*)&p_profile->engine[i].x)  / 65535.0f * 0.8f;
        dmx_data->arg.pixel_ctrl.pixel[i].arg.xy.y = endian_Swap_16bit((uint8_t*)&p_profile->engine[i].y) / 65535.0f * 0.8f;

    }

    dmx_model_extend((uint8_t*)&profile_data[0], sizeof(profile_2202_le_xy_16bit_arg_t), DMX_16BIT_TYPE);
    return 0;
}

uint8_t profile_3001_uitimate_w_8bit_analysis(const uint8_t* profile_data, dmx_data_t* dmx_data)
{
    uint8_t fx_state;
    uint8_t uitimate_mode;

    if(NULL == profile_data || NULL == dmx_data)
        return 0;

    dmx_data->lightness = *(uint8_t*)profile_data * 1000  / 255.0f;
    uitimate_mode = dmx_to_convert(*(uint8_t*)&profile_data[1], 255, 1, 0, 128);
    const profile_3001_uitimate_w_8bit_arg_t* p_profile = (profile_3001_uitimate_w_8bit_arg_t*)&profile_data[0];
    dmx_model_extend((uint8_t*)&profile_data[0], sizeof(profile_3001_uitimate_w_8bit_arg_t), DMX_8BIT_TYPE);
    switch(uitimate_mode)
    {
        case 0:
            dmx_data->type  = DMX_TYPE_CCT;
            dmx_data->arg.cct.cct = 6500;
            dmx_data->arg.cct.duv = 0;
        break;
        case 1:
        {
            dmx_data->type  = DMX_TYPE_FX;

            switch(p_profile->effect_type / 10)
            {
                case 0:
                    dmx_data->arg.fx.type = DMX_FX_PAPARAZZI;
                    dmx_data->arg.fx.arg.paparazzi.frq = fx_frq_calculate_8bit(p_profile->frq);
                    fx_state = p_profile->state < 70 ? DMX_FX_Trigger_Continue : DMX_FX_Trigger_None;
                    break;

                case 1:
                    dmx_data->arg.fx.type = DMX_FX_FIREWORKS;
                    dmx_data->arg.fx.arg.fireworks.frq = fx_frq_calculate_8bit(p_profile->frq);
                    fx_state = p_profile->state < 70 ? DMX_FX_Trigger_Continue : DMX_FX_Trigger_None;
                    break;

                case 2:
                    dmx_data->arg.fx.type = DMX_FX_FAULT_BULB;
                    dmx_data->arg.fx.arg.fault_bulb.frq = fx_frq_calculate_8bit(p_profile->frq);
                    fx_state = p_profile->state < 70 ? DMX_FX_Trigger_Continue : DMX_FX_Trigger_None;

                    break;

                case 3:
                    dmx_data->arg.fx.type = DMX_FX_LIGHTNING;
                    dmx_data->arg.fx.arg.lightning.frq = fx_frq_calculate_8bit(p_profile->frq);
                    fx_state = p_profile->state > 69 ? DMX_FX_Trigger_None : ((p_profile->state > 9) ? DMX_FX_Trigger_Once : DMX_FX_Trigger_Continue);
                    break;

                case 4:
                    dmx_data->arg.fx.type = DMX_FX_TV;
                    dmx_data->arg.fx.arg.tv.frq = fx_frq_calculate_8bit(p_profile->frq);
                    fx_state = p_profile->state < 70 ? DMX_FX_Trigger_Continue : DMX_FX_Trigger_None;
                    break;

                case 5:
                    dmx_data->arg.fx.type = DMX_FX_PULSING;
                    dmx_data->arg.fx.arg.pulsing.frq = fx_frq_calculate_8bit(p_profile->frq);
                    fx_state = p_profile->state < 70 ? DMX_FX_Trigger_Continue : DMX_FX_Trigger_None;
                    break;

                case 6:
                    dmx_data->arg.fx.type = DMX_FX_STROBE;
                    dmx_data->arg.fx.arg.strobe.spd = fx_frq_calculate_8bit(p_profile->frq);
                    fx_state = p_profile->state < 70 ? DMX_FX_Trigger_Continue : DMX_FX_Trigger_None;
                    break;

                case 7:
                default:
                    dmx_data->arg.fx.type = DMX_FX_EXPLOSION;
                    dmx_data->arg.fx.arg.explosion.decay = fx_frq_calculate_8bit(p_profile->frq);
                    fx_state = p_profile->state < 70 ? DMX_FX_Trigger_Once : DMX_FX_Trigger_None;
                    break;

            }
            return fx_state;
        }
        break;
      
        default:
            break;
    }

    return 0;
}


uint8_t profile_3002_uitimate_w_16bit_analysis(const uint8_t* profile_data, dmx_data_t* dmx_data)
{
    uint8_t fx_state;
    uint8_t uitimate_mode;

    if(NULL == profile_data || NULL == dmx_data)
        return 0;

    dmx_data->lightness = endian_Swap_16bit(profile_data)  * 1000 / 65535.0f;
    uitimate_mode = dmx_to_convert(*(uint8_t*)&profile_data[2], 255, 1, 0, 128);
    const profile_3002_uitimate_w_16bit_arg_t* p_profile = (profile_3002_uitimate_w_16bit_arg_t*)&profile_data[0];
    dmx_model_extend((uint8_t*)&profile_data[0], sizeof(profile_3002_uitimate_w_16bit_arg_t), DMX_16BIT_TYPE);

    switch(uitimate_mode)
    {
        case 0:
            dmx_data->type  = DMX_TYPE_CCT;
            dmx_data->arg.cct.cct = 6500;
            dmx_data->arg.cct.duv = 0;
        break;

        case 1:
            dmx_data->type  = DMX_TYPE_FX;

            switch(p_profile->effect_type / 10)
            {
                case 0:

                    dmx_data->arg.fx.type = DMX_FX_PAPARAZZI;
                    dmx_data->arg.fx.arg.paparazzi.frq = fx_frq_calculate_8bit(p_profile->frq);
                    fx_state = p_profile->state < 70 ? DMX_FX_Trigger_Continue : DMX_FX_Trigger_None;
                    break;

                case 1:
                    dmx_data->arg.fx.type = DMX_FX_FIREWORKS;
                    dmx_data->arg.fx.arg.fireworks.frq = fx_frq_calculate_8bit(p_profile->frq);
                    fx_state = p_profile->state < 70 ? DMX_FX_Trigger_Continue : DMX_FX_Trigger_None;
                    break;

                case 2:
                    dmx_data->arg.fx.type = DMX_FX_FAULT_BULB;
                    dmx_data->arg.fx.arg.fault_bulb.frq = fx_frq_calculate_8bit(p_profile->frq);
                    fx_state = p_profile->state < 70 ? DMX_FX_Trigger_Continue : DMX_FX_Trigger_None;

                    break;

                case 3:
                    dmx_data->arg.fx.type = DMX_FX_LIGHTNING;
                    dmx_data->arg.fx.arg.lightning.frq = fx_frq_calculate_8bit(p_profile->frq);
                    fx_state = p_profile->state > 69 ? DMX_FX_Trigger_None : ((p_profile->state > 9) ? DMX_FX_Trigger_Once : DMX_FX_Trigger_Continue);
                    break;

                case 4:
                    dmx_data->arg.fx.type = DMX_FX_TV;
                    dmx_data->arg.fx.arg.tv.frq = fx_frq_calculate_8bit(p_profile->frq);
                    fx_state = p_profile->state < 70 ? DMX_FX_Trigger_Continue : DMX_FX_Trigger_None;
                    break;

                case 5:
                    dmx_data->arg.fx.type = DMX_FX_PULSING;
                    dmx_data->arg.fx.arg.pulsing.frq = fx_frq_calculate_8bit(p_profile->frq);
                    fx_state = p_profile->state < 70 ? DMX_FX_Trigger_Continue : DMX_FX_Trigger_None;
                    break;

                case 6:
                    dmx_data->arg.fx.type = DMX_FX_STROBE;
                    dmx_data->arg.fx.arg.strobe.spd = fx_frq_calculate_8bit(p_profile->frq);
                    fx_state = p_profile->state < 70 ? DMX_FX_Trigger_Continue : DMX_FX_Trigger_None;
                    break;

                case 7:
                default:
                    dmx_data->arg.fx.type = DMX_FX_EXPLOSION;
                    dmx_data->arg.fx.arg.explosion.decay = fx_frq_calculate_8bit(p_profile->frq);
                    fx_state = p_profile->state < 70 ? DMX_FX_Trigger_Once : DMX_FX_Trigger_None;
                    break;

            }
            return fx_state;

        default:
            break;
    }

    return 0;
}



uint8_t profile_3003_uitimate_lite_8bit_analysis(const uint8_t* profile_data, dmx_data_t* dmx_data)
{
    uint8_t fx_state;
    uint8_t uitimate_mode;

    if(NULL == profile_data || NULL == dmx_data)
        return 0;

    dmx_data->lightness = *(uint8_t*)profile_data  * 1000 / 255.0f;
    uitimate_mode = dmx_to_convert(*(uint8_t*)&profile_data[2], 255, 1, 0, 128);
    const profile_3003_uitimate_lite_8bit_arg_t* p_profile = (profile_3003_uitimate_lite_8bit_arg_t*)&profile_data[0];  
    dmx_model_extend((uint8_t*)&profile_data[0], sizeof(profile_3003_uitimate_lite_8bit_arg_t), DMX_8BIT_TYPE);
    switch(uitimate_mode)
    {
        case 0:
            dmx_data->type  = DMX_TYPE_CCT;
            dmx_data->arg.cct.cct = cct_8bit_calculate(*(uint8_t*)&profile_data[1], DMX_CCT_MAX, DMX_CCT_MIN);
            dmx_data->arg.cct.duv = 0;
        break;
        case 1:
            dmx_data->type  = DMX_TYPE_FX;
            dmx_data->arg.fx.cct = cct_8bit_calculate(*(uint8_t*)&profile_data[1], DMX_CCT_MAX, DMX_CCT_MIN);

            switch(p_profile->effect_type / 10)
            {
                case 0:
                    dmx_data->arg.fx.type = DMX_FX_PAPARAZZI;
                    dmx_data->arg.fx.arg.paparazzi.frq = fx_frq_calculate_8bit(p_profile->frq);
					dmx_data->arg.fx.arg.paparazzi.cct = cct_8bit_calculate(*(uint8_t*)&profile_data[1], DMX_CCT_MAX, DMX_CCT_MIN);
                    fx_state = p_profile->state < 70 ? DMX_FX_Trigger_Continue : DMX_FX_Trigger_None;
                    break;
                case 1:
                    dmx_data->arg.fx.type = DMX_FX_FIREWORKS;
                    dmx_data->arg.fx.arg.fireworks.frq = fx_frq_calculate_8bit(p_profile->frq);
					dmx_data->arg.fx.arg.fireworks.type = convert_dmx_to_ctr_dy_gap(p_profile->cct, 0, 255, 0, 2, 10);
                    fx_state = p_profile->state < 70 ? DMX_FX_Trigger_Continue : DMX_FX_Trigger_None;
                    break;
                case 2:
                    dmx_data->arg.fx.type = DMX_FX_FAULT_BULB;
					dmx_data->arg.fx.arg.fault_bulb.mode = DMX_FX_MODE_CCT;
					dmx_data->arg.fx.arg.fault_bulb.mode_arg.cct.cct = cct_8bit_calculate(*(uint8_t*)&profile_data[1], DMX_CCT_MAX, DMX_CCT_MIN);
                    dmx_data->arg.fx.arg.fault_bulb.frq = fx_frq_calculate_8bit(p_profile->frq);
                    fx_state = p_profile->state < 70 ? DMX_FX_Trigger_Continue : DMX_FX_Trigger_None;
                    break;
                case 3:
                    dmx_data->arg.fx.type = DMX_FX_LIGHTNING;
					dmx_data->arg.fx.arg.lightning.cct = cct_8bit_calculate(*(uint8_t*)&profile_data[1], DMX_CCT_MAX, DMX_CCT_MIN);
                    dmx_data->arg.fx.arg.lightning.frq = fx_frq_calculate_8bit(p_profile->frq);
                    fx_state = p_profile->state > 69 ? DMX_FX_Trigger_None : ((p_profile->state > 9) ? DMX_FX_Trigger_Once : DMX_FX_Trigger_Continue);
                    break;
                case 4:
                    dmx_data->arg.fx.type = DMX_FX_TV;
					dmx_data->arg.fx.arg.tv.cct_range = convert_dmx_to_ctr_dy_gap(p_profile->cct, 0, 255, 0, 2, 10);
                    dmx_data->arg.fx.arg.tv.frq = fx_frq_calculate_8bit(p_profile->frq);
                    fx_state = p_profile->state < 70 ? DMX_FX_Trigger_Continue : DMX_FX_Trigger_None;
                    break;
                case 5:
                    dmx_data->arg.fx.type = DMX_FX_PULSING;
					dmx_data->arg.fx.arg.pulsing.mode = DMX_FX_MODE_CCT;
					dmx_data->arg.fx.arg.pulsing.mode_arg.cct.cct = cct_8bit_calculate(*(uint8_t*)&profile_data[1], DMX_CCT_MAX, DMX_CCT_MIN);
                    dmx_data->arg.fx.arg.pulsing.frq = fx_frq_calculate_8bit(p_profile->frq);
                    fx_state = p_profile->state < 70 ? DMX_FX_Trigger_Continue : DMX_FX_Trigger_None;
                    break;
                case 6:
                    dmx_data->arg.fx.type = DMX_FX_STROBE;
					dmx_data->arg.fx.arg.strobe.mode = DMX_FX_MODE_CCT;
					dmx_data->arg.fx.arg.strobe.mode_arg.cct.cct = cct_8bit_calculate(*(uint8_t*)&profile_data[1], DMX_CCT_MAX, DMX_CCT_MIN);
                    dmx_data->arg.fx.arg.strobe.spd = fx_frq_calculate_8bit(p_profile->frq);
                    fx_state = p_profile->state < 70 ? DMX_FX_Trigger_Continue : DMX_FX_Trigger_None;
                    break;
                case 7:
					dmx_data->arg.fx.type = DMX_FX_EXPLOSION;
					dmx_data->arg.fx.arg.explosion.mode = DMX_FX_MODE_CCT;
                    dmx_data->arg.fx.arg.explosion.decay = fx_frq_calculate_8bit(p_profile->frq);
					dmx_data->arg.fx.arg.explosion.mode_arg.cct.cct = cct_8bit_calculate(*(uint8_t*)&profile_data[1], DMX_CCT_MAX, DMX_CCT_MIN);
                    fx_state = p_profile->state < 70 ? DMX_FX_Trigger_Once : DMX_FX_Trigger_None;
				case 8:
                default:
					dmx_data->arg.fx.type = DMX_FX_FIRE;
					dmx_data->arg.fx.arg.fire.cct_range = convert_dmx_to_ctr_dy_gap(p_profile->cct, 0, 255, 0, 2, 10);
					dmx_data->arg.fx.arg.fire.frq = fx_frq_calculate_8bit(p_profile->frq);
					fx_state = p_profile->state < 70 ? DMX_FX_Trigger_Continue : DMX_FX_Trigger_None;
				break;

            }
            return fx_state;

        default:
            break;
    }

    return 0;
}

uint8_t profile_3004_uitimate_lite_16bit_analysis(const uint8_t* profile_data, dmx_data_t* dmx_data)
{
    uint8_t fx_state;
    uint8_t uitimate_mode;

    if(NULL == profile_data || NULL == dmx_data)
        return 0;

    dmx_data->lightness = endian_Swap_16bit(profile_data)  * 1000 / 65535.0f;
    uitimate_mode = dmx_to_convert(*(uint8_t*)&profile_data[4], 255, 1, 0, 128);
    const profile_3004_uitimate_lite_16bit_arg_t* p_profile = (profile_3004_uitimate_lite_16bit_arg_t*)&profile_data[0];
    dmx_model_extend((uint8_t*)&profile_data[0], sizeof(profile_3004_uitimate_lite_16bit_arg_t), DMX_16BIT_TYPE);
    switch(uitimate_mode)
    {
        case 0:
            dmx_data->type  = DMX_TYPE_CCT;
            dmx_data->arg.cct.cct = cct_16bit_calculate(endian_Swap_16bit((uint8_t*)&profile_data[2]), DMX_CCT_MAX, DMX_CCT_MIN);
            dmx_data->arg.cct.duv = 0;
        break;

        case 1:
            dmx_data->type  = DMX_TYPE_FX;
            dmx_data->arg.fx.cct = cct_16bit_calculate(endian_Swap_16bit((uint8_t*)&profile_data[2]), DMX_CCT_MAX, DMX_CCT_MIN);

            switch(p_profile->effect_type / 10)
            {
                case 0:
                    dmx_data->arg.fx.type = DMX_FX_PAPARAZZI;
                    dmx_data->arg.fx.arg.paparazzi.frq = fx_frq_calculate_8bit(p_profile->frq);
					dmx_data->arg.fx.arg.paparazzi.cct = fx_cct_16bit_calculate(p_profile->cct, DMX_CCT_MAX, DMX_CCT_MIN);
                    fx_state = p_profile->state < 70 ? DMX_FX_Trigger_Continue : DMX_FX_Trigger_None;
                    break;
                case 1:
                    dmx_data->arg.fx.type = DMX_FX_FIREWORKS;
                    dmx_data->arg.fx.arg.fireworks.frq = fx_frq_calculate_8bit(p_profile->frq);
					dmx_data->arg.fx.arg.fireworks.type = convert_dmx_to_ctr_dy_gap(p_profile->cct, 0, 65535, 0, 2, 10);
                    fx_state = p_profile->state < 70 ? DMX_FX_Trigger_Continue : DMX_FX_Trigger_None;
                    break;
                case 2:
                    dmx_data->arg.fx.type = DMX_FX_FAULT_BULB;
					dmx_data->arg.fx.arg.fault_bulb.mode = DMX_FX_MODE_CCT;
					dmx_data->arg.fx.arg.fault_bulb.mode_arg.cct.cct = fx_cct_16bit_calculate(p_profile->cct, DMX_CCT_MAX, DMX_CCT_MIN);
                    dmx_data->arg.fx.arg.fault_bulb.frq = fx_frq_calculate_8bit(p_profile->frq);
                    fx_state = p_profile->state < 70 ? DMX_FX_Trigger_Continue : DMX_FX_Trigger_None;
                    break;
                case 3:
                    dmx_data->arg.fx.type = DMX_FX_LIGHTNING;
					dmx_data->arg.fx.arg.lightning.cct = fx_cct_16bit_calculate(p_profile->cct, DMX_CCT_MAX, DMX_CCT_MIN);
                    dmx_data->arg.fx.arg.lightning.frq = fx_frq_calculate_8bit(p_profile->frq);
                    fx_state = p_profile->state > 69 ? DMX_FX_Trigger_None : ((p_profile->state > 9) ? DMX_FX_Trigger_Once : DMX_FX_Trigger_Continue);
                    break;
                case 4:
                    dmx_data->arg.fx.type = DMX_FX_TV;
					dmx_data->arg.fx.arg.tv.cct_range = convert_dmx_to_ctr_dy_gap(p_profile->cct, 0, 65535, 0, 2, 10);
                    dmx_data->arg.fx.arg.tv.frq = fx_frq_calculate_8bit(p_profile->frq);
                    fx_state = p_profile->state < 70 ? DMX_FX_Trigger_Continue : DMX_FX_Trigger_None;
                    break;
                case 5:
                    dmx_data->arg.fx.type = DMX_FX_PULSING;
					dmx_data->arg.fx.arg.pulsing.mode = DMX_FX_MODE_CCT;
					dmx_data->arg.fx.arg.pulsing.mode_arg.cct.cct = fx_cct_16bit_calculate(p_profile->cct, DMX_CCT_MAX, DMX_CCT_MIN);
                    dmx_data->arg.fx.arg.pulsing.frq = fx_frq_calculate_8bit(p_profile->frq);
                    fx_state = p_profile->state < 70 ? DMX_FX_Trigger_Continue : DMX_FX_Trigger_None;
                    break;
                case 6:
                    dmx_data->arg.fx.type = DMX_FX_STROBE;
					dmx_data->arg.fx.arg.strobe.mode = DMX_FX_MODE_CCT;
					dmx_data->arg.fx.arg.strobe.mode_arg.cct.cct = fx_cct_16bit_calculate(p_profile->cct, DMX_CCT_MAX, DMX_CCT_MIN);
                    dmx_data->arg.fx.arg.strobe.spd = fx_frq_calculate_8bit(p_profile->frq);
                    fx_state = p_profile->state < 70 ? DMX_FX_Trigger_Continue : DMX_FX_Trigger_None;
                    break;
                case 7:
					dmx_data->arg.fx.type = DMX_FX_EXPLOSION;
					dmx_data->arg.fx.arg.explosion.mode = DMX_FX_MODE_CCT;
                    dmx_data->arg.fx.arg.explosion.decay = fx_frq_calculate_8bit(p_profile->frq);
					dmx_data->arg.fx.arg.explosion.mode_arg.cct.cct = fx_cct_16bit_calculate(p_profile->cct, DMX_CCT_MAX, DMX_CCT_MIN);
                    fx_state = p_profile->state < 70 ? DMX_FX_Trigger_Once : DMX_FX_Trigger_None;
				case 8:
                default:
					dmx_data->arg.fx.type = DMX_FX_FIRE;
					dmx_data->arg.fx.arg.fire.cct_range = convert_dmx_to_ctr_dy_gap(p_profile->cct, 0, 65535, 0, 2, 10);
					dmx_data->arg.fx.arg.fire.frq = fx_frq_calculate_8bit(p_profile->frq);
					fx_state = p_profile->state < 70 ? DMX_FX_Trigger_Continue : DMX_FX_Trigger_None;
				break;

            }
            return fx_state;

        default:
            break;
    }

    return 0;
}

uint8_t profile_3005_uitimate_rgb_8bit_analysis(const uint8_t* profile_data, dmx_data_t* dmx_data)
{
    enum dmx_uitimate_mode
    {
        DMX_UITIMATE_MODE_CCT_AND_RGB,
        DMX_UITIMATE_MODE_CCT,
        DMX_UITIMATE_MODE_CCT_AND_HSI,
        DMX_UITIMATE_MODE_RGB,
        DMX_UITIMATE_MODE_HSI,
        DMX_UITIMATE_MODE_GEL,
        DMX_UITIMATE_MODE_XY,
        DMX_UITIMATE_MODE_SOURCE,
		DMX_UITIMATE_MODE_WW_AND_RGB,
        DMX_UITIMATE_MODE_FX,
    }uitimate_mode;
	
	uint8_t strobe_state;
	
    if(NULL == profile_data || NULL == dmx_data)
        return 0;

    uitimate_mode = (enum dmx_uitimate_mode)dmx_to_convert(*(uint8_t*)&profile_data[1], 255, 8, 0, 10);

    switch(uitimate_mode)
    {
        case DMX_UITIMATE_MODE_CCT_AND_RGB:
#if(DMX_ULTIMATE_8BIT_RGB_PROFILE_CFG_CCT_RGB == 0x0801)
            strobe_state = profile_0801_cct_and_rgb_8bit_analysis(&profile_data[1], dmx_data);
#elif (DMX_ULTIMATE_8BIT_RGB_PROFILE_CFG_CCT_RGB == 0x0803)
           strobe_state =  profile_0803_cct_and_rgbw_8bit_analysis(&profile_data[1], dmx_data);
#elif (DMX_ULTIMATE_8BIT_RGB_PROFILE_CFG_CCT_RGB == 0x0805)
           strobe_state =  profile_0805_cct_and_rgbww_8bit_analysis(&profile_data[1], dmx_data);
#elif (DMX_ULTIMATE_8BIT_RGB_PROFILE_CFG_CCT_RGB == 0x0807)
           strobe_state = profile_0807_cct_and_rgbacl_8bit_analysis(&profile_data[1], dmx_data);
#endif
            break;

        case DMX_UITIMATE_MODE_CCT:
            strobe_state = profile_0105_cct_gm_8bit_analysis(&profile_data[1], dmx_data);
            break;

        case DMX_UITIMATE_MODE_CCT_AND_HSI:
            strobe_state = profile_0901_cct_and_hsi_8bit_analysis(&profile_data[1], dmx_data);
            break;

        case DMX_UITIMATE_MODE_RGB:
            strobe_state = profile_0201_rgb_8bit_analysis(&profile_data[1], dmx_data);
            break;

        case DMX_UITIMATE_MODE_HSI:
#if(DMX_ULTIMATE_8BIT_RGB_PROFILE_CFG_HSI == 0x0301)
            strobe_state = profile_0301_hsi_8bit_analysis(&profile_data[1], dmx_data);
#elif (DMX_ULTIMATE_8BIT_RGB_PROFILE_CFG_HSI == 0x0303)
            strobe_state = profile_0303_advhsi_8bit_analysis(&profile_data[1], dmx_data);
#endif
            break;

        case DMX_UITIMATE_MODE_GEL:
            strobe_state = profile_0501_gel_8bit_analysis(&profile_data[1], dmx_data);
            break;

        case DMX_UITIMATE_MODE_XY:
            strobe_state = profile_0401_xy_8bit_analysis(&profile_data[1], dmx_data);
            break;

        case DMX_UITIMATE_MODE_SOURCE:
            strobe_state = profile_0601_source_8bit_analysis(&profile_data[1], dmx_data);
            break;

        case DMX_UITIMATE_MODE_FX:
#if(DMX_ULTIMATE_8BIT_RGB_PROFILE_CFG_FX == 0x0705)
            strobe_state = profile_0705_fx_rgb_lite_I_8bit_analysis(&profile_data[1], dmx_data);
#elif (DMX_ULTIMATE_8BIT_RGB_PROFILE_CFG_FX == 0x0707)
            strobe_state = profile_0707_fx_rgb_8bit_analysis(&profile_data[1], dmx_data);
#endif
            break;
		case DMX_UITIMATE_MODE_WW_AND_RGB:
#if (DMX_ULTIMATE_8BIT_RGB_PROFILE_CFG_WW_RGB == 0x0203)
            strobe_state = profile_0203_rgbw_8bit_analysis(&profile_data[1], dmx_data);
#elif (DMX_ULTIMATE_8BIT_RGB_PROFILE_CFG_WW_RGB == 0x0205)
            strobe_state = profile_0205_rgbww_8bit_analysis(&profile_data[1], dmx_data);
#elif (DMX_ULTIMATE_8BIT_RGB_PROFILE_CFG_WW_RGB == 0x0207)
            strobe_state = profile_0207_rgbacl_8bit_analysis(&profile_data[1], dmx_data);
#endif
        default:
            break;
    }

    dmx_data->lightness = *(uint8_t*)profile_data  * 1000 / 255.0f;
    return strobe_state;
}


uint8_t profile_3006_uitimate_rgb_16bit_analysis(const uint8_t* profile_data, dmx_data_t* dmx_data)
{
    enum dmx_uitimate_mode
    {
        DMX_UITIMATE_MODE_CCT_AND_RGB,
        DMX_UITIMATE_MODE_CCT,
        DMX_UITIMATE_MODE_CCT_AND_HSI,
        DMX_UITIMATE_MODE_RGB,
        DMX_UITIMATE_MODE_HSI,
        DMX_UITIMATE_MODE_GEL,
        DMX_UITIMATE_MODE_XY,
        DMX_UITIMATE_MODE_SOURCE,
		DMX_UITIMATE_MODE_WW_AND_RGB,
        DMX_UITIMATE_MODE_FX,
    }uitimate_mode;
	
	uint8_t strobe_state;
	
    if(NULL == profile_data || NULL == dmx_data)
        return 0;

    uitimate_mode = (enum dmx_uitimate_mode)dmx_to_convert(*(uint8_t*)&profile_data[2], 255, 8, 0, 10);

    switch(uitimate_mode)
    {
        case DMX_UITIMATE_MODE_CCT_AND_RGB:
#if(DMX_ULTIMATE_16BIT_RGB_PROFILE_CFG_CCT_RGB == 0x0802)
            strobe_state = profile_0802_cct_and_rgb_16bit_analysis(&profile_data[1], dmx_data);
#elif (DMX_ULTIMATE_16BIT_RGB_PROFILE_CFG_CCT_RGB == 0x0804)
            strobe_state = profile_0804_cct_and_rgbw_16bit_analysis(&profile_data[1], dmx_data);
#elif (DMX_ULTIMATE_16BIT_RGB_PROFILE_CFG_CCT_RGB == 0x0806)
            strobe_state = profile_0806_cct_and_rgbww_16bit_analysis(&profile_data[1], dmx_data);
#elif (DMX_ULTIMATE_16BIT_RGB_PROFILE_CFG_CCT_RGB == 0x0808)
            strobe_state = profile_0808_cct_and_rgbacl_16bit_analysis(&profile_data[1], dmx_data);
#endif
            break;

        case DMX_UITIMATE_MODE_CCT:
            strobe_state = profile_0106_cct_gm_16bit_analysis(&profile_data[1], dmx_data);
            break;

        case DMX_UITIMATE_MODE_CCT_AND_HSI:
            strobe_state = profile_0902_cct_and_hsi_16bit_analysis(&profile_data[1], dmx_data);
            break;

        case DMX_UITIMATE_MODE_RGB:
            strobe_state = profile_0202_rgb_16bit_analysis(&profile_data[1], dmx_data);
            break;

        case DMX_UITIMATE_MODE_HSI:
#if(DMX_ULTIMATE_16BIT_RGB_PROFILE_CFG_HSI == 0x0302)
            strobe_state = profile_0302_hsi_16bit_analysis(&profile_data[1], dmx_data);
#elif (DMX_ULTIMATE_16BIT_RGB_PROFILE_CFG_HSI == 0x0304)
            strobe_state = profile_0304_advhsi_16bit_analysis(&profile_data[1], dmx_data);
#endif
            break;

        case DMX_UITIMATE_MODE_GEL:
            strobe_state = profile_0502_gel_16bit_analysis(&profile_data[1], dmx_data);
            break;

        case DMX_UITIMATE_MODE_XY:
            strobe_state = profile_0402_xy_16bit_analysis(&profile_data[1], dmx_data);
            break;

        case DMX_UITIMATE_MODE_SOURCE:
            strobe_state = profile_0602_source_16bit_analysis(&profile_data[1], dmx_data);
            break;

        case DMX_UITIMATE_MODE_FX:
#if(DMX_ULTIMATE_16BIT_RGB_PROFILE_CFG_FX == 0x0706)
            strobe_state = profile_0706_fx_rgb_lite_III_8bit_analysis(&profile_data[1], dmx_data);
#elif(DMX_ULTIMATE_16BIT_RGB_PROFILE_CFG_FX == 0x0708)
            strobe_state = profile_0708_fx_rgb_16bit_analysis(&profile_data[1], dmx_data);
#endif
            break;
		 case DMX_UITIMATE_MODE_WW_AND_RGB:
			#if (DMX_ULTIMATE_16BIT_RGB_PROFILE_CFG_WW_RGB == 0x0204)
					strobe_state = profile_0204_rgbw_16bit_analysis(&profile_data[1], dmx_data);
			#elif (DMX_ULTIMATE_16BIT_RGB_PROFILE_CFG_WW_RGB == 0x0206)
					strobe_state = profile_0206_rgbww_16bit_analysis(&profile_data[1], dmx_data);
			#elif (DMX_ULTIMATE_16BIT_RGB_PROFILE_CFG_WW_RGB == 0x0208)
					strobe_state = profile_0206_rgbacl_16bit_analysis(&profile_data[1], dmx_data);
			#endif
		  break;
        default:
            break;
    }

    dmx_data->lightness = endian_Swap_16bit(profile_data)  * 1000 / 65535.0f;
    return strobe_state;
}

uint8_t profile_3007_uitimate_lite_8bit_analysis(const uint8_t* profile_data, dmx_data_t* dmx_data)
{
    uint8_t fx_state;
    uint8_t uitimate_mode;

    if(NULL == profile_data || NULL == dmx_data)
        return 0;

    dmx_data->lightness = *(uint8_t*)profile_data  * 1000 / 255.0f;
    uitimate_mode = dmx_to_convert(*(uint8_t*)&profile_data[3], 255, 1, 0, 128);
    const profile_3007_uitimate_lite_8bit_arg_t* p_profile = (profile_3007_uitimate_lite_8bit_arg_t*)&profile_data[0];  
    dmx_model_extend((uint8_t*)&profile_data[0], sizeof(profile_3007_uitimate_lite_8bit_arg_t), DMX_8BIT_TYPE);
    switch(uitimate_mode)
    {
        case 0:
            dmx_data->type  = DMX_TYPE_CCT;
            dmx_data->arg.cct.cct = cct_8bit_calculate(*(uint8_t*)&profile_data[1], DMX_CCT_MAX, DMX_CCT_MIN);
            dmx_data->arg.cct.duv = duv_8bit_calculate(p_profile->gm);
        break;
        case 1:
            dmx_data->type  = DMX_TYPE_FX;
            dmx_data->arg.fx.cct = cct_8bit_calculate(*(uint8_t*)&profile_data[1], DMX_CCT_MAX, DMX_CCT_MIN);

            switch(p_profile->effect_type / 10)
            {
                case 0:
                    dmx_data->arg.fx.type = DMX_FX_PAPARAZZI;
                    dmx_data->arg.fx.arg.paparazzi.frq = fx_frq_calculate_8bit(p_profile->frq);
					dmx_data->arg.fx.arg.paparazzi.cct = cct_8bit_calculate(*(uint8_t*)&profile_data[1], DMX_CCT_MAX, DMX_CCT_MIN);
                    fx_state = p_profile->state < 70 ? DMX_FX_Trigger_Continue : DMX_FX_Trigger_None;
                    break;
                case 1:
                    dmx_data->arg.fx.type = DMX_FX_FIREWORKS;
                    dmx_data->arg.fx.arg.fireworks.frq = fx_frq_calculate_8bit(p_profile->frq);
					dmx_data->arg.fx.arg.fireworks.type = convert_dmx_to_ctr_dy_gap(p_profile->cct, 0, 255, 0, 2, 10);
                    fx_state = p_profile->state < 70 ? DMX_FX_Trigger_Continue : DMX_FX_Trigger_None;
                    break;
                case 2:
                    dmx_data->arg.fx.type = DMX_FX_FAULT_BULB;
					dmx_data->arg.fx.arg.fault_bulb.mode = DMX_FX_MODE_CCT;
					dmx_data->arg.fx.arg.fault_bulb.mode_arg.cct.cct = cct_8bit_calculate(*(uint8_t*)&profile_data[1], DMX_CCT_MAX, DMX_CCT_MIN);
                    dmx_data->arg.fx.arg.fault_bulb.frq = fx_frq_calculate_8bit(p_profile->frq);
                    fx_state = p_profile->state < 70 ? DMX_FX_Trigger_Continue : DMX_FX_Trigger_None;
                    break;
                case 3:
                    dmx_data->arg.fx.type = DMX_FX_LIGHTNING;
					dmx_data->arg.fx.arg.lightning.cct = cct_8bit_calculate(*(uint8_t*)&profile_data[1], DMX_CCT_MAX, DMX_CCT_MIN);
                    dmx_data->arg.fx.arg.lightning.frq = fx_frq_calculate_8bit(p_profile->frq);
                    fx_state = p_profile->state > 69 ? DMX_FX_Trigger_None : ((p_profile->state > 9) ? DMX_FX_Trigger_Once : DMX_FX_Trigger_Continue);
                    break;
                case 4:
                    dmx_data->arg.fx.type = DMX_FX_TV;
					dmx_data->arg.fx.arg.tv.cct_range = convert_dmx_to_ctr_dy_gap(p_profile->cct, 0, 255, 0, 2, 10);
                    dmx_data->arg.fx.arg.tv.frq = fx_frq_calculate_8bit(p_profile->frq);
                    fx_state = p_profile->state < 70 ? DMX_FX_Trigger_Continue : DMX_FX_Trigger_None;
                    break;
                case 5:
                    dmx_data->arg.fx.type = DMX_FX_PULSING;
					dmx_data->arg.fx.arg.pulsing.mode = DMX_FX_MODE_CCT;
					dmx_data->arg.fx.arg.pulsing.mode_arg.cct.cct = cct_8bit_calculate(*(uint8_t*)&profile_data[1], DMX_CCT_MAX, DMX_CCT_MIN);
                    dmx_data->arg.fx.arg.pulsing.frq = fx_frq_calculate_8bit(p_profile->frq);
                    fx_state = p_profile->state < 70 ? DMX_FX_Trigger_Continue : DMX_FX_Trigger_None;
                    break;
                case 6:
                    dmx_data->arg.fx.type = DMX_FX_STROBE;
					dmx_data->arg.fx.arg.strobe.mode = DMX_FX_MODE_CCT;
					dmx_data->arg.fx.arg.strobe.mode_arg.cct.cct = cct_8bit_calculate(*(uint8_t*)&profile_data[1], DMX_CCT_MAX, DMX_CCT_MIN);
                    dmx_data->arg.fx.arg.strobe.spd = fx_frq_calculate_8bit(p_profile->frq);
                    fx_state = p_profile->state < 70 ? DMX_FX_Trigger_Continue : DMX_FX_Trigger_None;
                    break;
                case 7:
					dmx_data->arg.fx.type = DMX_FX_EXPLOSION;
					dmx_data->arg.fx.arg.explosion.mode = DMX_FX_MODE_CCT;
                    dmx_data->arg.fx.arg.explosion.decay = fx_frq_calculate_8bit(p_profile->frq);
					dmx_data->arg.fx.arg.explosion.mode_arg.cct.cct = cct_8bit_calculate(*(uint8_t*)&profile_data[1], DMX_CCT_MAX, DMX_CCT_MIN);
                    fx_state = p_profile->state < 70 ? DMX_FX_Trigger_Once : DMX_FX_Trigger_None;
				case 8:
                default:
					dmx_data->arg.fx.type = DMX_FX_FIRE;
					dmx_data->arg.fx.arg.fire.cct_range = convert_dmx_to_ctr_dy_gap(p_profile->cct, 0, 255, 0, 2, 10);
					dmx_data->arg.fx.arg.fire.frq = fx_frq_calculate_8bit(p_profile->frq);
					fx_state = p_profile->state < 70 ? DMX_FX_Trigger_Continue : DMX_FX_Trigger_None;
				break;

            }
            return fx_state;

        default:
            break;
    }

    return 0;
}

uint8_t profile_3008_uitimate_lite_16bit_analysis(const uint8_t* profile_data, dmx_data_t* dmx_data)
{
    uint8_t fx_state;
    uint8_t uitimate_mode;

    if(NULL == profile_data || NULL == dmx_data)
        return 0;

    dmx_data->lightness = endian_Swap_16bit(profile_data)  * 1000 / 65535.0f;
    uitimate_mode = dmx_to_convert(*(uint8_t*)&profile_data[6], 255, 1, 0, 128);
    const profile_3008_uitimate_lite_16bit_arg_t* p_profile = (profile_3008_uitimate_lite_16bit_arg_t*)&profile_data[0];
    dmx_model_extend((uint8_t*)&profile_data[0], sizeof(profile_3008_uitimate_lite_16bit_arg_t), DMX_16BIT_TYPE);
    switch(uitimate_mode)
    {
        case 0:
            dmx_data->type  = DMX_TYPE_CCT;
            dmx_data->arg.cct.cct = cct_16bit_calculate(endian_Swap_16bit((uint8_t*)&profile_data[2]), DMX_CCT_MAX, DMX_CCT_MIN);
            dmx_data->arg.cct.duv = duv_8bit_calculate(p_profile->gm);
        break;

        case 1:
            dmx_data->type  = DMX_TYPE_FX;
            dmx_data->arg.fx.cct = cct_16bit_calculate(endian_Swap_16bit((uint8_t*)&profile_data[2]), DMX_CCT_MAX, DMX_CCT_MIN);

            switch(p_profile->effect_type / 10)
            {
                case 0:
                    dmx_data->arg.fx.type = DMX_FX_PAPARAZZI;
                    dmx_data->arg.fx.arg.paparazzi.frq = fx_frq_calculate_8bit(p_profile->frq);
					dmx_data->arg.fx.arg.paparazzi.cct = fx_cct_16bit_calculate(p_profile->cct, DMX_CCT_MAX, DMX_CCT_MIN);
                    fx_state = p_profile->state < 70 ? DMX_FX_Trigger_Continue : DMX_FX_Trigger_None;
                    break;
                case 1:
                    dmx_data->arg.fx.type = DMX_FX_FIREWORKS;
                    dmx_data->arg.fx.arg.fireworks.frq = fx_frq_calculate_8bit(p_profile->frq);
					dmx_data->arg.fx.arg.fireworks.type = convert_dmx_to_ctr_dy_gap(p_profile->cct, 0, 65535, 0, 2, 10);
                    fx_state = p_profile->state < 70 ? DMX_FX_Trigger_Continue : DMX_FX_Trigger_None;
                    break;
                case 2:
                    dmx_data->arg.fx.type = DMX_FX_FAULT_BULB;
					dmx_data->arg.fx.arg.fault_bulb.mode = DMX_FX_MODE_CCT;
					dmx_data->arg.fx.arg.fault_bulb.mode_arg.cct.cct = fx_cct_16bit_calculate(p_profile->cct, DMX_CCT_MAX, DMX_CCT_MIN);
                    dmx_data->arg.fx.arg.fault_bulb.frq = fx_frq_calculate_8bit(p_profile->frq);
                    fx_state = p_profile->state < 70 ? DMX_FX_Trigger_Continue : DMX_FX_Trigger_None;
                    break;
                case 3:
                    dmx_data->arg.fx.type = DMX_FX_LIGHTNING;
					dmx_data->arg.fx.arg.lightning.cct = fx_cct_16bit_calculate(p_profile->cct, DMX_CCT_MAX, DMX_CCT_MIN);
                    dmx_data->arg.fx.arg.lightning.frq = fx_frq_calculate_8bit(p_profile->frq);
                    fx_state = p_profile->state > 69 ? DMX_FX_Trigger_None : ((p_profile->state > 9) ? DMX_FX_Trigger_Once : DMX_FX_Trigger_Continue);
                    break;
                case 4:
                    dmx_data->arg.fx.type = DMX_FX_TV;
					dmx_data->arg.fx.arg.tv.cct_range = convert_dmx_to_ctr_dy_gap(p_profile->cct, 0, 65535, 0, 2, 10);
                    dmx_data->arg.fx.arg.tv.frq = fx_frq_calculate_8bit(p_profile->frq);
                    fx_state = p_profile->state < 70 ? DMX_FX_Trigger_Continue : DMX_FX_Trigger_None;
                    break;
                case 5:
                    dmx_data->arg.fx.type = DMX_FX_PULSING;
					dmx_data->arg.fx.arg.pulsing.mode = DMX_FX_MODE_CCT;
					dmx_data->arg.fx.arg.pulsing.mode_arg.cct.cct = fx_cct_16bit_calculate(p_profile->cct, DMX_CCT_MAX, DMX_CCT_MIN);
                    dmx_data->arg.fx.arg.pulsing.frq = fx_frq_calculate_8bit(p_profile->frq);
                    fx_state = p_profile->state < 70 ? DMX_FX_Trigger_Continue : DMX_FX_Trigger_None;
                    break;
                case 6:
                    dmx_data->arg.fx.type = DMX_FX_STROBE;
					dmx_data->arg.fx.arg.strobe.mode = DMX_FX_MODE_CCT;
					dmx_data->arg.fx.arg.strobe.mode_arg.cct.cct = fx_cct_16bit_calculate(p_profile->cct, DMX_CCT_MAX, DMX_CCT_MIN);
                    dmx_data->arg.fx.arg.strobe.spd = fx_frq_calculate_8bit(p_profile->frq);
                    fx_state = p_profile->state < 70 ? DMX_FX_Trigger_Continue : DMX_FX_Trigger_None;
                    break;
                case 7:
					dmx_data->arg.fx.type = DMX_FX_EXPLOSION;
					dmx_data->arg.fx.arg.explosion.mode = DMX_FX_MODE_CCT;
                    dmx_data->arg.fx.arg.explosion.decay = fx_frq_calculate_8bit(p_profile->frq);
					dmx_data->arg.fx.arg.explosion.mode_arg.cct.cct = fx_cct_16bit_calculate(p_profile->cct, DMX_CCT_MAX, DMX_CCT_MIN);
                    fx_state = p_profile->state < 70 ? DMX_FX_Trigger_Once : DMX_FX_Trigger_None;
				case 8:
                default:
					dmx_data->arg.fx.type = DMX_FX_FIRE;
					dmx_data->arg.fx.arg.fire.cct_range = convert_dmx_to_ctr_dy_gap(p_profile->cct, 0, 65535, 0, 2, 10);
					dmx_data->arg.fx.arg.fire.frq = fx_frq_calculate_8bit(p_profile->frq);
					fx_state = p_profile->state < 70 ? DMX_FX_Trigger_Continue : DMX_FX_Trigger_None;
				break;

            }
            return fx_state;

        default:
            break;
    }

    return 0;
}

uint8_t (*profile_analysis[DMX_PROFILE_ID_NUM])(const uint8_t*, dmx_data_t*) =
{
#if DMX_PROFILE_0101_EN
    profile_0101_cct_w_8bit_analysis,
#endif
#if DMX_PROFILE_0102_EN
    profile_0102_cct_w_16bit_analysis,
#endif
#if DMX_PROFILE_0103_EN
    profile_0103_cct_lite_8bit_analysis,
#endif

#if DMX_PROFILE_0104_EN
    profile_0104_cct_lite_16bit_analysis,
#endif
#if DMX_PROFILE_0105_EN
    profile_0105_cct_gm_8bit_analysis,
#endif
#if DMX_PROFILE_0106_EN
    profile_0106_cct_gm_16bit_analysis,
#endif

#if DMX_PROFILE_0201_EN
    profile_0201_rgb_8bit_analysis,
#endif

#if DMX_PROFILE_0202_EN
    profile_0202_rgb_16bit_analysis,
#endif

#if DMX_PROFILE_0203_EN
    profile_0203_rgbw_8bit_analysis,
#endif

#if DMX_PROFILE_0204_EN
    profile_0204_rgbw_16bit_analysis,
#endif
#if DMX_PROFILE_0205_EN
    profile_0205_rgbww_8bit_analysis,
#endif

#if DMX_PROFILE_0206_EN
    profile_0206_rgbww_16bit_analysis,
#endif

#if DMX_PROFILE_0207_EN
    profile_0207_rgbacl_8bit_analysis,
#endif
#if DMX_PROFILE_0208_EN
    profile_0208_rgbacl_16bit_analysis,
#endif

#if DMX_PROFILE_0301_EN
    profile_0301_hsi_8bit_analysis,
#endif
#if DMX_PROFILE_0302_EN
    profile_0302_hsi_16bit_analysis,
#endif
#if DMX_PROFILE_0303_EN
    profile_0303_advhsi_8bit_analysis,
#endif
#if DMX_PROFILE_0304_EN
    profile_0304_advhsi_16bit_analysis,
#endif
#if DMX_PROFILE_0401_EN
    profile_0401_xy_8bit_analysis,
#endif
#if DMX_PROFILE_0402_EN
    profile_0402_xy_16bit_analysis,
#endif
#if DMX_PROFILE_0501_EN
    profile_0501_gel_8bit_analysis,
#endif

#if DMX_PROFILE_0502_EN
    profile_0502_gel_16bit_analysis,
#endif
#if DMX_PROFILE_0601_EN
    profile_0601_source_8bit_analysis,
#endif
#if DMX_PROFILE_0602_EN
    profile_0602_source_16bit_analysis,
#endif
#if DMX_PROFILE_0701_EN
    profile_0701_fx_w_8bit_analysis,
#endif
#if DMX_PROFILE_0702_EN
    profile_0702_fx_w_16bit_analysis,
#endif
#if DMX_PROFILE_0703_EN
    profile_0703_fx_lite_8bit_analysis,
#endif
#if DMX_PROFILE_0704_EN
    profile_0704_fx_lite_16bit_analysis,
#endif

#if DMX_PROFILE_0705_EN
    profile_0705_fx_rgb_lite_I_8bit_analysis,
#endif
#if DMX_PROFILE_0706_EN
    profile_0706_fx_rgb_lite_III_8bit_analysis,
#endif
#if DMX_PROFILE_0707_EN
    profile_0707_fx_rgb_8bit_analysis,
#endif
#if DMX_PROFILE_0708_EN
    profile_0708_fx_rgb_16bit_analysis,
#endif
#if DMX_PROFILE_0709_EN
    profile_0709_pixel_8bit_analysis,
#endif
#if DMX_PROFILE_0801_EN
    profile_0801_cct_and_rgb_8bit_analysis,
#endif
#if DMX_PROFILE_0802_EN
    profile_0802_cct_and_rgb_16bit_analysis,
#endif
#if DMX_PROFILE_0803_EN
    profile_0803_cct_and_rgbw_8bit_analysis,
#endif
#if DMX_PROFILE_0804_EN
    profile_0804_cct_and_rgbw_16bit_analysis,
#endif
#if DMX_PROFILE_0805_EN
    profile_0805_cct_and_rgbww_8bit_analysis,
#endif
#if DMX_PROFILE_0806_EN
    profile_0806_cct_and_rgbww_16bit_analysis,
#endif
#if DMX_PROFILE_0807_EN
    profile_0807_cct_and_rgbacl_8bit_analysis,
#endif
#if DMX_PROFILE_0808_EN
    profile_0808_cct_and_rgbacl_16bit_analysis,
#endif

#if DMX_PROFILE_0901_EN
    profile_0901_cct_and_hsi_8bit_analysis,
#endif
#if DMX_PROFILE_0902_EN
    profile_0902_cct_and_hsi_16bit_analysis,
#endif
#if DMX_PROFILE_2001_EN
    profile_2001_le_cct_and_rgb_8bit_analysis,
#endif
#if DMX_PROFILE_2002_EN
    profile_2002_le_cct_and_rgb_16bit_analysis,
#endif
#if DMX_PROFILE_2003_EN
    profile_2003_le_cct_and_rgbw_8bit_analysis,
#endif
#if DMX_PROFILE_2004_EN
    profile_2004_le_cct_and_rgbw_16bit_analysis,
#endif
#if DMX_PROFILE_2005_EN
    profile_2005_le_cct_and_rgbww_8bit_analysis,
#endif

#if DMX_PROFILE_2006_EN
    profile_2006_le_cct_and_rgbww_16bit_analysis,
#endif
#if DMX_PROFILE_2007_EN
    profile_2007_le_cct_and_rgbacl_8bit_analysis,
#endif
#if DMX_PROFILE_2008_EN
    profile_2008_le_cct_and_rgbacl_16bit_analysis,
#endif
#if DMX_PROFILE_2101_EN
    profile_2101_le_hsi_8bit_analysis,
#endif
#if DMX_PROFILE_2102_EN
    profile_2102_le_hsi_16bit_analysis,
#endif
#if DMX_PROFILE_2103_EN
    profile_2103_le_advhsi_8bit_analysis,
#endif
#if DMX_PROFILE_2104_EN
    profile_2104_le_advhsi_16bit_analysis,
#endif

#if DMX_PROFILE_2201_EN
    profile_2201_le_xy_8bit_analysis,
#endif
#if DMX_PROFILE_2202_EN
    profile_2202_le_xy_16bit_analysis,
#endif

#if DMX_PROFILE_3001_EN
    profile_3001_uitimate_w_8bit_analysis,
#endif

#if DMX_PROFILE_3002_EN
    profile_3002_uitimate_w_16bit_analysis,
#endif

#if DMX_PROFILE_3003_EN
    profile_3003_uitimate_lite_8bit_analysis,
#endif
#if DMX_PROFILE_3004_EN
    profile_3004_uitimate_lite_16bit_analysis,
#endif
#if DMX_PROFILE_3005_EN
    profile_3005_uitimate_rgb_8bit_analysis,
#endif
#if DMX_PROFILE_3006_EN
    profile_3006_uitimate_rgb_16bit_analysis,
#endif
#if DMX_PROFILE_3007_EN
    profile_3007_uitimate_lite_8bit_analysis,
#endif
#if DMX_PROFILE_3008_EN
    profile_3008_uitimate_lite_16bit_analysis,
#endif
};




uint8_t profile_user_data_execte(dmx_profile_id id, const uint8_t* profile_data, dmx_data_t* dmx_data)
{
    if(profile_analysis[id] == NULL)
        return 0;

    return profile_analysis[id](profile_data, dmx_data); 
}















