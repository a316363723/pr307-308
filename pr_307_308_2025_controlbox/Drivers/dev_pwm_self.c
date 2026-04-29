#include "dev_pwm_self.h"
#include <string.h>
#include "hal_timer.h"
#include "cmsis_os2.h"   

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))
#define LED_ADAPTION_NUM  7
void dev_adapt_pwm_set(uint8_t timer_type, uint32_t value);

#if PROJECT_TYPE==308

//const adapt_Coarse_t  s_adapt_Coarse_80k[LED_ADAPTION_NUM][10] = {
//	{{1000,4900},{2000,3950},{3000,2950},{4000,1850},{5000,1850},{6000,1850},{7000,1750},{8000,1750},{9000,1750},{10000,1750}},
//	{{1000,3950},{2000,3200},{3000,2600},{4000,2450},{5000,2450},{6000,2450},{7000,2450},{8000,2450},{9000,2450},{10000,2450}},
//	{{1000,3800},{2000,3200},{3000,2550},{4000,2450},{5000,2450},{6000,2450},{7000,2450},{8000,2450},{9000,2450},{10000,2450}},
//	{{1000,3850},{2000,3300},{3000,2950},{4000,2650},{5000,2650},{6000,2650},{7000,2650},{8000,2650},{9000,2650},{10000,2650}},
//	{{1000,3800},{2000,3300},{3000,2790},{4000,2650},{5000,2650},{6000,2650},{7000,2650},{8000,2650},{9000,2650},{10000,2650}},
//	{{1000,3900},{2000,3300},{3000,2950},{4000,2600},{5000,2600},{6000,2600},{7000,2600},{8000,2600},{9000,2600},{10000,2600}},
//	{{1000,3800},{2000,3300},{3000,2750},{4000,2600},{5000,2600},{6000,2600},{7000,2600},{8000,2600},{9000,2600},{10000,2600}},
//};

const adapt_Coarse_t  s_adapt_Coarse_80k[LED_ADAPTION_NUM][10] = {
	{{1000,4900},{2000,2650},{3000,2650},{4000,1550},{5000,1550},{6000,1550},{7000,1450},{8000,1450},{9000,1450},{10000,1450}},
	{{1000,3950},{2000,3200},{3000,2600},{4000,1984},{5000,1984},{6000,1984},{7000,1984},{8000,1984},{9000,1984},{10000,1984}},
	{{1000,3800},{2000,3200},{3000,2550},{4000,2190},{5000,2190},{6000,2190},{7000,2190},{8000,2190},{9000,2190},{10000,2190}},
	{{1000,3850},{2000,3300},{3000,2950},{4000,2118},{5000,2118},{6000,2118},{7000,2118},{8000,2118},{9000,2118},{10000,2118}},
	{{1000,3800},{2000,3300},{3000,2790},{4000,2156},{5000,2156},{6000,2156},{7000,2156},{8000,2156},{9000,2156},{10000,2156}},
	{{1000,3900},{2000,3300},{3000,2950},{4000,2052},{5000,2052},{6000,2052},{7000,2052},{8000,2052},{9000,2052},{10000,2052}},
	{{1000,3800},{2000,3300},{3000,2750},{4000,2158},{5000,2158},{6000,2158},{7000,2158},{8000,2158},{9000,2158},{10000,2158}},
};


dev_pwm_adapt_t pwm_adapt_80k = {
	.config.timer_period = 5999,
	.config.led_max_volt = 2111,
	.config.led_min_volt = 1613,
	.config.led_base = 1600,
	.config.max_step = 100,
	.config.min_step = 10,
	.config.led_num = LED_ADAPTION_NUM,
	.adapt_fade.fade_time = 350,
	.length = ARRAY_SIZE(s_adapt_Coarse_80k),
	.adapt_fade.pwm_base ={.r= 4900,.g = 3950,.b = 3800,.ww1 = 3850,.cw1 = 3800,.ww2 = 3900,.cw2 = 3800},
	.adapt_fade.pwm_limit = {.r= 1250,.g = 1660,.b = 1800,.ww1 = 1800,.cw1 = 1800,.ww2 = 1800,.cw2 = 1900},
	.adapt_fade.pwm_start ={0},
	.adapt_fade.pwm_current = {0},
	.adapt_fade.pwm_end = {0},
	.data = &s_adapt_Coarse_80k[0],
	.adapt_pwm_ctrl = dev_adapt_pwm_set,
};

const adapt_Coarse_t  s_adapt_Coarse_20k[LED_ADAPTION_NUM][10] = {
	{{1000,1250},{2000,2050},{3000,1200},{4000,1200},{5000,1200},{6000,1200},{7000,1200},{8000,1200},{9000,1200},{10000,1200}},
	{{1000,1660},{2000,2500},{3000,2000},{4000,2000},{5000,2000},{6000,2000},{7000,2000},{8000,2000},{9000,2000},{10000,2000}},
	{{1000,1800},{2000,2550},{3000,2000},{4000,2000},{5000,2000},{6000,2000},{7000,2000},{8000,2000},{9000,2000},{10000,2000}},
	{{1000,1800},{2000,2700},{3000,2000},{4000,2000},{5000,2000},{6000,2000},{7000,2000},{8000,2000},{9000,2000},{10000,2000}},
	{{1000,1800},{2000,2800},{3000,2050},{4000,2050},{5000,2050},{6000,2050},{7000,2050},{8000,2050},{9000,2050},{10000,2050}},
	{{1000,1800},{2000,2750},{3000,2050},{4000,2050},{5000,2050},{6000,2050},{7000,2050},{8000,2050},{9000,2050},{10000,2050}},
	{{1000,1900},{2000,2800},{3000,2000},{4000,2000},{5000,2000},{6000,2000},{7000,2000},{8000,2000},{9000,2000},{10000,2000}},

};

dev_pwm_adapt_t pwm_adapt_20k = {
	.config.timer_period = 5999,
	.config.led_max_volt = 2111,
	.config.led_min_volt = 1613,
	.config.led_base = 2000,
	.config.max_step = 100,
	.config.min_step = 10,
	.config.led_num = LED_ADAPTION_NUM,
	.adapt_fade.fade_time = 350,
	.length = ARRAY_SIZE(s_adapt_Coarse_20k),
	.adapt_fade.pwm_base ={.r= 1250,.g = 1660,.b = 1800,.ww1 = 1800,.cw1 = 1800,.ww2 = 1800,.cw2 = 1900},
	.adapt_fade.pwm_limit = {.r= 1250,.g = 1660,.b = 1800,.ww1 = 1800,.cw1 = 1800,.ww2 = 1800,.cw2 = 1900},
	.adapt_fade.pwm_start ={0},
	.adapt_fade.pwm_current = {0},
	.adapt_fade.pwm_end = {0},
	.data = &s_adapt_Coarse_20k[0],
	.adapt_pwm_ctrl = dev_adapt_pwm_set,
};

#elif PROJECT_TYPE==307
const adapt_Coarse_t  s_adapt_Coarse_80k[LED_ADAPTION_NUM][10] = {
	{{1000,1250},{2000,4500},{3000,4000},{4000,4000},{5000,4000},{6000,4000},{7000,4000},{8000,4000},{9000,4000},{10000,4000}},
	{{1000,1660},{2000,4600},{3000,3700},{4000,3150},{5000,3150},{6000,3150},{7000,3150},{8000,3150},{9000,3150},{10000,3150}},
	{{1000,1800},{2000,4000},{3000,3400},{4000,3200},{5000,3200},{6000,3200},{7000,3200},{8000,3200},{9000,3200},{10000,3200}},
	{{1000,1800},{2000,4500},{3000,4050},{4000,4050},{5000,4050},{6000,4050},{7000,4050},{8000,4050},{9000,4050},{10000,4050}},
	{{1000,1800},{2000,4000},{3000,3400},{4000,3400},{5000,3400},{6000,3400},{7000,3400},{8000,3400},{9000,3400},{10000,3400}},
	{{1000,1800},{2000,4700},{3000,4230},{4000,4230},{5000,4230},{6000,4230},{7000,4230},{8000,4230},{9000,4230},{10000,4230}},
	{{1000,1900},{2000,4100},{3000,3600},{4000,3600},{5000,3600},{6000,3600},{7000,3600},{8000,3600},{9000,3600},{10000,3600}},

};

dev_pwm_adapt_t pwm_adapt_80k = {
	.config.timer_period = 5999,
	.config.led_max_volt = 2111,
	.config.led_min_volt = 1613,
	.config.led_base = 1600,
	.config.max_step = 350,
	.config.min_step = 10,
	.config.led_num = LED_ADAPTION_NUM,
	.adapt_fade.fade_time = 200,
	.length = ARRAY_SIZE(s_adapt_Coarse_80k),
	.adapt_fade.pwm_base ={.r= 5050,.g = 5600,.b = 4950,.ww1 = 5000,.cw1 = 4800,.ww2 = 5200,.cw2 = 4800},
	.adapt_fade.pwm_limit = {.r= 2100,.g = 2250,.b = 2160,.ww1 = 2100,.cw1 = 2060,.ww2 = 2100,.cw2 = 2050},
	.adapt_fade.pwm_start ={0},
	.adapt_fade.pwm_current = {0},
	.adapt_fade.pwm_end = {0},
	.data = &s_adapt_Coarse_80k[0],
	.adapt_pwm_ctrl = dev_adapt_pwm_set,
};


const adapt_Coarse_t  s_adapt_Coarse_20k[LED_ADAPTION_NUM][10] = {
	{{1000,2100},{2000,3500},{3000,3500},{4000,3500},{5000,3500},{6000,3500},{7000,3500},{8000,3500},{9000,3500},{10000,3500}},
	{{1000,1000},{2000,1400},{3000,1400},{4000,1400},{5000,1400},{6000,1400},{7000,1400},{8000,1400},{9000,1400},{10000,1400}},
	{{1000,2160},{2000,2500},{3000,2500},{4000,2500},{5000,2500},{6000,2500},{7000,2500},{8000,2500},{9000,2500},{10000,2500}},
	{{1000,2100},{2000,3800},{3000,3800},{4000,3800},{5000,3800},{6000,3800},{7000,3800},{8000,3800},{9000,3800},{10000,3800}},
	{{1000,2060},{2000,3500},{3000,3500},{4000,3500},{5000,3500},{6000,3500},{7000,3500},{8000,3500},{9000,3500},{10000,3500}},
	{{1000,2100},{2000,4000},{3000,4000},{4000,4000},{5000,4000},{6000,4000},{7000,4000},{8000,4000},{9000,4000},{10000,4000}},
	{{1000,2050},{2000,3500},{3000,3500},{4000,3500},{5000,3500},{6000,3500},{7000,3500},{8000,3500},{9000,3500},{10000,3500}},

};

dev_pwm_adapt_t pwm_adapt_20k = {
	.config.timer_period = 5999,
	.config.led_max_volt = 2111,
	.config.led_min_volt = 1613,
	.config.led_base = 2000,
	.config.max_step = 350,
	.config.min_step = 10,
	.config.led_num = LED_ADAPTION_NUM,
	.adapt_fade.fade_time = 200,
	.length = ARRAY_SIZE(s_adapt_Coarse_20k),
	.adapt_fade.pwm_base ={.r= 2100,.g = 1000,.b = 2160,.ww1 = 2100,.cw1 = 2060,.ww2 = 2100,.cw2 = 2050},
	.adapt_fade.pwm_limit = {.r= 2100,.g = 1000,.b = 2160,.ww1 = 2100,.cw1 = 2060,.ww2 = 2100,.cw2 = 2050},
	.adapt_fade.pwm_start ={0},
	.adapt_fade.pwm_current = {0},
	.adapt_fade.pwm_end = {0},
	.data = &s_adapt_Coarse_20k[0],
	.adapt_pwm_ctrl = dev_adapt_pwm_set,
};

#endif

void dev_adapt_fade_restart(dev_pwm_adapt_t *dev)
{
	dev->adapt_fade.run_time = 0;
	dev->adapt_fade.fade_start = 1;
}

uint16_t dev_adapt_pwm_look(const adapt_Coarse_t* data,uint8_t length,uint16_t key)
{
	uint16_t ret;
	uint16_t start = 0;
	uint16_t end = length - 1;
	uint16_t middle;
	
	if (key < data[start].led_pwm) 
	{
		ret = data[start].p_volt;
	}
	else if (key > data[end].led_pwm) 
	{
		ret = data[end].p_volt;
	}
	else 
	{
		while (start <= end)
		{
			middle = (start + end)/2;
			if (key < data[middle].led_pwm)
			{
				end = middle;
			}
			else if (key > data[middle].led_pwm)
			{
				start = middle;
			}
			else
			{
				return data[middle].p_volt;
			}
			
			if((start + 1 == end)) 
			{
				ret = data[end].p_volt;
	
//				ret = data[start].p_volt + 
//						(data[end].p_volt - data[start].p_volt)*
//							(data[start].led_pwm - key)/(data[start].led_pwm - data[end].led_pwm);
				return ret;
			}	
		}
			ret = data[start].p_volt;
	}
	return ret;
}


void dev_adapt_pwm_regulate(dev_pwm_adapt_t* dev,uint8_t type, uint16_t volt,uint16_t pwm )
{
	uint8_t  dir = 0;
	uint16_t p_volt;
	uint16_t res;	
	enum fan_speed_level
	{
		ADAPT_SPEED_LEVEL_0 = 0,
		ADAPT_SPEED_LEVEL_1,
		ADAPT_SPEED_LEVEL_2,
		ADAPT_SPEED_LEVEL_3,
		ADAPT_SPEED_LEVEL_MAX,
	} level ;
	static uint16_t p_volt_last[LED_ADAPTION_NUM] = {0};
	if(type > dev->config.led_num ) return;
	uint16_t *p_pwm_end = (uint16_t* )&dev->adapt_fade.pwm_end;
	uint16_t *p_pwm_base = (uint16_t* )&dev->adapt_fade.pwm_base;
	uint16_t *p_pwm_limit = (uint16_t* )&dev->adapt_fade.pwm_limit;
	p_pwm_end = p_pwm_end + type;
	p_pwm_limit = p_pwm_limit + type;
	p_pwm_base = p_pwm_base + type;
	
	if(pwm < dev->config.led_base) 
	{	
		*p_pwm_end = *p_pwm_base;
		 p_volt_last[type] = *p_pwm_base;
		return;
	}
	p_volt = dev_adapt_pwm_look(*(dev->data+type),dev->length,(pwm / 1000)*1000);
	if(p_volt != p_volt_last[type])
	{
		*p_pwm_end = p_volt;
		 p_volt_last[type] = p_volt;
		 return;
	}	
	if(volt > (dev->config.led_max_volt + dev->config.led_max_volt))
	{
		dir = 1;
		level = ADAPT_SPEED_LEVEL_MAX;
	}
	else if(volt > (dev->config.led_max_volt + dev->config.led_max_volt / 2))
	{
		dir = 1;
		level = ADAPT_SPEED_LEVEL_3;
	}
	else if(volt > (dev->config.led_max_volt + dev->config.led_max_volt / 4))
	{
		dir = 1;
		level = ADAPT_SPEED_LEVEL_2;
	}
	else if(volt > dev->config.led_max_volt)
	{
		dir = 1;
		level = ADAPT_SPEED_LEVEL_1;
	}	
	else if(volt < (dev->config.led_min_volt - dev->config.led_min_volt / 2))           
	{
		dir = 2;
		level = ADAPT_SPEED_LEVEL_MAX;
	}
	else if(volt < (dev->config.led_min_volt - dev->config.led_min_volt / 3))
	{                                                           
		dir = 2;
		level = ADAPT_SPEED_LEVEL_3;
	}
	else if(volt < (dev->config.led_min_volt - dev->config.led_min_volt / 4))
	{                                                           
		dir = 2;
		level = ADAPT_SPEED_LEVEL_2;
	}
	else if(volt< dev->config.led_min_volt)
	{                                                           
		dir = 2;
		level = ADAPT_SPEED_LEVEL_1;
	}
	res = dev->config.min_step + ((dev->config.max_step - dev->config.min_step) / (ADAPT_SPEED_LEVEL_MAX )) * (level - 1);
	switch (dir)
	{
		case 1:
			*p_pwm_end = *p_pwm_end + res;
			if(*p_pwm_end > dev->config.timer_period)
			{
				*p_pwm_end = dev->config.timer_period;
			}
			else if( *p_pwm_end < *p_pwm_limit)
			{
				*p_pwm_end = *p_pwm_limit;
			}
			break;
		case 2:
			*p_pwm_end = *p_pwm_end - res;
			if(*p_pwm_end > dev->config.timer_period)
			{
			  *p_pwm_end = dev->config.timer_period;
			}
			else if(*p_pwm_end < *p_pwm_limit)
			{
			  *p_pwm_end = *p_pwm_limit;
			}
			break;
		default:
		  break;
	}

}


uint8_t dev_adapt_pwm_linear_deal(adapt_pwm_dim_t adapt_start,  adapt_pwm_dim_t adapt_end,
                          adapt_pwm_dim_t *adapt_current, uint32_t *run_time, float whole_time)
{
	float ratio_r;
	float ratio_g;
	float ratio_b;
	float ratio_ww1;
	float ratio_cw1;
	float ratio_ww2;
	float ratio_cw2;

	ratio_r = (adapt_end.r - adapt_start.r) / whole_time;
	ratio_g = (adapt_end.g - adapt_start.g) / whole_time;
	ratio_b = (adapt_end.b - adapt_start.b) / whole_time;
	ratio_ww1 = (adapt_end.ww1 - adapt_start.ww1) / whole_time;
	ratio_cw1 = (adapt_end.cw1 - adapt_start.cw1) / whole_time;
	ratio_ww2 = (adapt_end.ww2 - adapt_start.ww2) / whole_time;
	ratio_cw2 = (adapt_end.cw2 - adapt_start.cw2) / whole_time;

	if (*run_time >= whole_time)
	{
		*run_time = whole_time;
	}
	else
	(*run_time)++;

	adapt_current->r = adapt_start.r + (ratio_r * (*run_time) + 0.5f);
	adapt_current->g = adapt_start.g + (ratio_g * (*run_time) + 0.5f);
	adapt_current->b = adapt_start.b + (ratio_b * (*run_time) + 0.5f);
	adapt_current->ww1 = adapt_start.ww1 + (ratio_ww1 * (*run_time) + 0.5f);
	adapt_current->cw1 = adapt_start.cw1 + (ratio_cw1 * (*run_time) + 0.5f);
	adapt_current->ww2 = adapt_start.ww2 + (ratio_ww2 * (*run_time) + 0.5f);
	adapt_current->cw2 = adapt_start.cw2 + (ratio_cw2 * (*run_time) + 0.5f);

	if (*run_time == whole_time)
	{
	  return 1;
	}

	return 0;
}

void dev_adapt_pwm_set(uint8_t timer_type, uint32_t value)
{
     hal_timer_duty_cycle_set( timer_type ,value);
}

void dev_adapt_pwm_1ms_back(dev_pwm_adapt_t* dev)
{

	if(dev->adapt_fade.fade_start == 1 && dev->adapt_fade.run_time < dev->adapt_fade.fade_time)
    {
        if(dev->adapt_fade.run_time == 0)
        {
            memcpy(&dev->adapt_fade.pwm_start, &dev->adapt_fade.pwm_current, sizeof(dev->adapt_fade.pwm_current));
        }
        dev_adapt_pwm_linear_deal(dev->adapt_fade.pwm_start, dev->adapt_fade.pwm_end, &dev->adapt_fade.pwm_current, &dev->adapt_fade.run_time, dev->adapt_fade.fade_time );
        
        uint16_t* adapt_dim = &dev->adapt_fade.pwm_current.r;
        for(uint8_t i = HAL_PWM_SELF_ADAPTION_R ; i< (HAL_PWM_SELF_ADAPTION_CW2 + 1) ; i++)
        {
            dev->adapt_pwm_ctrl(i,*adapt_dim);
            adapt_dim++;
        }
    }

}


void dev_adapt_pwm_limit_set(dev_pwm_adapt_t* dev, float whole_time)
{
	uint8_t res;
	dev->adapt_fade.pwm_current.r = dev->config.timer_period;
	dev->adapt_fade.pwm_current.g = dev->config.timer_period;
	dev->adapt_fade.pwm_current.b = dev->config.timer_period;
	dev->adapt_fade.pwm_current.ww1 = dev->config.timer_period;
	dev->adapt_fade.pwm_current.cw1 = dev->config.timer_period;
	dev->adapt_fade.pwm_current.ww2 = dev->config.timer_period;
	dev->adapt_fade.pwm_current.cw2 = dev->config.timer_period;
	memcpy(&dev->adapt_fade.pwm_end, &dev->adapt_fade.pwm_limit, sizeof(dev->adapt_fade.pwm_limit));
	#if PROJECT_TYPE == 307
	dev->adapt_fade.pwm_end.r = 2500;
	dev->adapt_fade.pwm_end.g = 2700;
	dev->adapt_fade.pwm_end.b = 3000;
	dev->adapt_fade.pwm_end.cw1 = 3000;
	dev->adapt_fade.pwm_end.cw2 = 3000;
	dev->adapt_fade.pwm_end.ww1 = 3000;
	dev->adapt_fade.pwm_end.ww2 = 3000;
	#elif PROJECT_TYPE == 308
	dev->adapt_fade.pwm_end.r = 1750;
	dev->adapt_fade.pwm_end.g = 2100;
	dev->adapt_fade.pwm_end.b = 2300;
	dev->adapt_fade.pwm_end.cw1 = 2300;
	dev->adapt_fade.pwm_end.cw2 = 2300;
	dev->adapt_fade.pwm_end.ww1 = 2300;
	dev->adapt_fade.pwm_end.ww2 = 2300;
	#endif
	dev->adapt_fade.run_time = 0;
	do
	{
		if(dev->adapt_fade.run_time == 0)
        {
            memcpy(&dev->adapt_fade.pwm_start, &dev->adapt_fade.pwm_current, sizeof(dev->adapt_fade.pwm_current));
        }
        res = dev_adapt_pwm_linear_deal(dev->adapt_fade.pwm_start, dev->adapt_fade.pwm_end, &dev->adapt_fade.pwm_current, &dev->adapt_fade.run_time, whole_time);
		uint16_t* adapt_dim = &dev->adapt_fade.pwm_current.r;
        for(uint8_t i = HAL_PWM_SELF_ADAPTION_R ; i< (HAL_PWM_SELF_ADAPTION_CW2 + 1) ; i++)
        {
            hal_timer_duty_cycle_set( (enum hal_timera_enum)i ,*adapt_dim);
            adapt_dim++;
        }
		osDelay(1);
			
	}
	while(res != 1);

}