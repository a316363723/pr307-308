/****************************************************************************************
**  Filename :  111.c
**  Abstract :  。
**  By       :  何建国
**  Date     :  
**  Changelog:1.First Create
*****************************************************************************************/

/*****************************************************************************************
									Includes
*****************************************************************************************/
#include "bsp_timer.h"
#include "app_led.h"
#include "stdio.h"
#include "hc32_ddl.h"
#include "bsp_debug.h"
#include "define.h"
#include <math.h>
#include "bsp_iic.h"
#define DRIVE_LED_PRINT_EN					1
/*****************************************************************************************
							  Variables definitions
*****************************************************************************************/
//Light_Effect g_effect_struct;
Single_Crc_Cob_Slow_TypeDef g_str_single_crc_cob_slow = {0};
Cob_Slow_TypeDef g_str_cob_slow;
struct mixing_pwm g_str_cob_analog;
struct mixing_lux g_str_cob_analog_lux;
uint16_t g_str_cob_analog_sta[5];
Slow_Light_Ctr_TypeDef g_slow_light_str = {0};
Slow_Light_Single_Crc_Ctr_TypeDef g_slow_single_crc_light_str = {0};
struct mixing_lux dev_lux = {0};
/* Suppress warning messages */
#if defined(__CC_ARM)
// Suppress warning message: extended constant initialiser used
#pragma diag_suppress 1296
#elif defined(__ICCARM__)
#elif defined(__GNUC__)
#endif
/*
PA0	R_PWM	红灯PWM输出TIMA_2_PWM1
PA1	G_PWM	绿灯PWM输出TIMA_2_PWM2
PA2	B_PWM	蓝灯PWM输出TIMA_2_PWM3
PA3	YU_PWM	预留PWM调光TIMA_2_PWM4

PE3	WW1_PWM	TIMA_4_PWM1
PE4	WW2_PWM	TIMA_4_PWM2
PE5	CW1_PWM	TIMA_4_PWM3
PE6	CW2_PWM	TIMA_4_PWM4
*/
#ifdef PR_308_L2  //308新的PWM路数有调整
//COB驱动PWM设置
Led_Pwm_Channel_TypeDef g_led_pwm_channel = {
	.p_ch_info[0] = (volatile uint32_t *)((uint32_t)&M4_TMRA_2->CMPAR1 + (uint32_t)TMRA_CH_1 * 4U),//r
	.p_ch_info[1] = (volatile uint32_t *)((uint32_t)&M4_TMRA_2->CMPAR1 + (uint32_t)TMRA_CH_2 * 4U),//g
	.p_ch_info[5] = (volatile uint32_t *)((uint32_t)&M4_TMRA_2->CMPAR1 + (uint32_t)TMRA_CH_3 * 4U),//b
	.p_ch_info[3] = (volatile uint32_t *)((uint32_t)&M4_TMRA_4->CMPAR1 + (uint32_t)TMRA_CH_1 * 4U),//ww1
	.p_ch_info[4] = (volatile uint32_t *)((uint32_t)&M4_TMRA_4->CMPAR1 + (uint32_t)TMRA_CH_3 * 4U),//cw1
	.p_ch_info[2] = (volatile uint32_t *)((uint32_t)&M4_TMRA_4->CMPAR1 + (uint32_t)TMRA_CH_2 * 4U),//ww2
	.p_ch_info[6] = (volatile uint32_t *)((uint32_t)&M4_TMRA_4->CMPAR1 + (uint32_t)TMRA_CH_4 * 4U),//cw2	
	
	
	.max_pwm[0] = 9520,
	.max_pwm[1] = 9520,
	.max_pwm[2] = 9520,
	.max_pwm[3] = 9520,
	.max_pwm[4] = 9520,
	.max_pwm[5] = 9520,
	.max_pwm[6] = 9520,
	
	.p_ch[0] = TMRA_CH_1,
	.p_ch[1] = TMRA_CH_2,
	.p_ch[5] = TMRA_CH_3,
	.p_ch[3] = TMRA_CH_1,
	.p_ch[4] = TMRA_CH_3,
	.p_ch[2] = TMRA_CH_2,
	.p_ch[6] = TMRA_CH_4,
	
	.p_tim[0] = M4_TMRA_2,
	.p_tim[1] = M4_TMRA_2,
	.p_tim[5] = M4_TMRA_2,
	.p_tim[3] = M4_TMRA_4,
	.p_tim[4] = M4_TMRA_4,
	.p_tim[2] = M4_TMRA_4,
	.p_tim[6] = M4_TMRA_4,
	
	.p_ch_adc[0] = 0,
	.p_ch_adc[1] = 0,
	.p_ch_adc[2] = 0,
	.p_ch_adc[3] = 0,
	.p_ch_adc[4] = 0,
	.p_ch_adc[5] = 0,
	.p_ch_adc[6] = 0,
};
#endif

#ifdef PR_307_L3
//COB驱动PWM设置
Led_Pwm_Channel_TypeDef g_led_pwm_channel = {
	.p_ch_info[0] = (volatile uint32_t *)((uint32_t)&M4_TMRA_2->CMPAR1 + (uint32_t)TMRA_CH_1 * 4U),//r
	.p_ch_info[1] = (volatile uint32_t *)((uint32_t)&M4_TMRA_2->CMPAR1 + (uint32_t)TMRA_CH_2 * 4U),//g
	.p_ch_info[2] = (volatile uint32_t *)((uint32_t)&M4_TMRA_2->CMPAR1 + (uint32_t)TMRA_CH_3 * 4U),//b
	.p_ch_info[3] = (volatile uint32_t *)((uint32_t)&M4_TMRA_4->CMPAR1 + (uint32_t)TMRA_CH_1 * 4U),//ww1
	.p_ch_info[4] = (volatile uint32_t *)((uint32_t)&M4_TMRA_4->CMPAR1 + (uint32_t)TMRA_CH_3 * 4U),//cw1
	.p_ch_info[5] = (volatile uint32_t *)((uint32_t)&M4_TMRA_4->CMPAR1 + (uint32_t)TMRA_CH_2 * 4U),//ww2
	.p_ch_info[6] = (volatile uint32_t *)((uint32_t)&M4_TMRA_4->CMPAR1 + (uint32_t)TMRA_CH_4 * 4U),//cw2	
	
	
	.max_pwm[0] = 12000,
	.max_pwm[1] = 12000,
	.max_pwm[2] = 12000,
	.max_pwm[3] = 12000,
	.max_pwm[4] = 12000,
	.max_pwm[5] = 12000,
	.max_pwm[6] = 12000,
	
	.p_ch[0] = TMRA_CH_1,
	.p_ch[1] = TMRA_CH_2,
	.p_ch[2] = TMRA_CH_3,
	.p_ch[3] = TMRA_CH_1,
	.p_ch[4] = TMRA_CH_3,
	.p_ch[5] = TMRA_CH_2,
	.p_ch[6] = TMRA_CH_4,
	
	.p_tim[0] = M4_TMRA_2,
	.p_tim[1] = M4_TMRA_2,
	.p_tim[2] = M4_TMRA_2,
	.p_tim[3] = M4_TMRA_4,
	.p_tim[4] = M4_TMRA_4,
	.p_tim[5] = M4_TMRA_4,
	.p_tim[6] = M4_TMRA_4,
    
    .p_ch_adc[0] = 0,
	.p_ch_adc[1] = 0,
	.p_ch_adc[2] = 0,
	.p_ch_adc[3] = 0,
	.p_ch_adc[4] = 0,
	.p_ch_adc[5] = 0,
	.p_ch_adc[6] = 0,
};
#endif


//ADC检测电压的方式
ADC_Adaptive_Ctr_TypeDef g_adc_adaptive_para = {
	//adc电压值
#ifdef PR_308_L2
	.p_volt[0] = (uint32_t*)&g_adc_para.V_red_adapt,
	.p_volt[1] = (uint32_t*)&g_adc_para.V_green_adapt,
	.p_volt[5] = (uint32_t*)&g_adc_para.V_blue_adapt,
	.p_volt[3] = (uint32_t*)&g_adc_para.V_ww1_adapt,
	.p_volt[4] = (uint32_t*)&g_adc_para.V_cw1_adapt,
	.p_volt[2] = (uint32_t*)&g_adc_para.V_ww2_adapt,
	.p_volt[6] = (uint32_t*)&g_adc_para.V_cw2_adapt,
#endif	
#ifdef PR_307_L3
	.p_volt[0] = (uint32_t*)&g_adc_para.V_red_adapt,
	.p_volt[1] = (uint32_t*)&g_adc_para.V_green_adapt,
	.p_volt[2] = (uint32_t*)&g_adc_para.V_blue_adapt,
	.p_volt[3] = (uint32_t*)&g_adc_para.V_ww1_adapt,
	.p_volt[4] = (uint32_t*)&g_adc_para.V_cw1_adapt,
	.p_volt[5] = (uint32_t*)&g_adc_para.V_ww2_adapt,
	.p_volt[6] = (uint32_t*)&g_adc_para.V_cw2_adapt,
#endif	
	.adjust_dir[0] = 0,//0 - 默认无效值，1 - PWM加，2 - PWM减
	.adjust_dir[1] = 0,
	.adjust_dir[2] = 0,
	.adjust_dir[3] = 0,
	.adjust_dir[4] = 0,
	
	.adjust_val[0] = 0,
	.adjust_val[1] = 0,
	.adjust_val[2] = 0,
	.adjust_val[3] = 0,
	.adjust_val[4] = 0,
	
	.adjust_enalbe[0] = 0,//0 - 禁止调节， 1 - 使能调节
	.adjust_enalbe[1] = 0,
	.adjust_enalbe[2] = 0,
	.adjust_enalbe[3] = 0,
	.adjust_enalbe[4] = 0,
	
	.wave_times[0] = 0,
	.wave_times[1] = 0,
	.wave_times[2] = 0,
	.wave_times[3] = 0,
	.wave_times[4] = 0,
	
	.max_val[0] = 4900,//R 2500
	.max_val[1] = 4900,//G 3300 
	.max_val[2] = 4900,//B 2800
	.max_val[3] = 4900,//WW 1500
	.max_val[4] = 4900,//CW 2500
	
	.min_val[0] = 200,//R
	.min_val[1] = 800,//G
	.min_val[2] = 200,//B
	.min_val[3] = 500,//WW
	.min_val[4] = 1500,//CW
};

ADC_Pwm_Base_Volt_TypeDef g_adc_pwm_base_volt = {
	.p_ch_volt[0] = (float*)&g_adc_para.V_check_red,
	.p_ch_volt[1] = (float*)&g_adc_para.V_check_green,
	.p_ch_volt[2] = (float*)&g_adc_para.V_check_blue,
	.p_ch_volt[3] = (float*)&g_adc_para.V_check_ww1,
	.p_ch_volt[4] = (float*)&g_adc_para.V_check_cw1,
	.p_ch_volt[5] = (float*)&g_adc_para.V_check_ww2,
	.p_ch_volt[6] = (float*)&g_adc_para.V_check_cw2,

	//后续需要写
//	.ch_base_volt[0] = ,
//	.ch_base_volt[1] = ,
//	.ch_base_volt[2] = ,
//	.ch_base_volt[3] = ,
//	.ch_base_volt[4] = ,
//	.ch_base_volt[5] = ,
//	.ch_base_volt[6] = ,
};
Led_Channel_Arg_TypeDef g_led_pwm_arg = {
	.led_pwm[0] = 0,
	.led_pwm[1] = 0,
	.led_pwm[2] = 0,
	.led_pwm[3] = 0,
	.led_pwm[4] = 0,
	.led_pwm[5] = 0,
	.led_pwm[6] = 0,
		.pwm_buffer_one = { 0,0,0,
							0,0,0,
							0,0,0,
							0,0,0,
							0,0,0
		},
		.pwm_buffer_two = {0,0,0,0,
						   0,0,0,0,
						   0,0,0,0,
						   0,0,0,0,
						   0,0,0,0
		},
};
/*****************************************************************************************
							  Functions definitions
*****************************************************************************************/



void led_down_limit_base_data_info(uint8_t mode)
{
#ifdef PR_307_L3
	if(mode == 0)
	{
		for(uint8_t i = 0; i < 5; i++)
		{
			for(uint8_t j = 0; j < led_base_data.data[i]->length; j++)
			{
				if(led_base_data.data[i]->data[j].Illuminance != 0)
				{
					g_slow_light_str.down_limit_pwm_info[i] = led_base_data.data[i]->data[j].pwm;
					g_slow_light_str.down_limit_lux_info[i] = led_base_data.data[i]->data[j].Illuminance;
					break;
				}
			}
		}
	}
	if(mode == 1)
	{
		for(uint8_t i = 0; i < 5; i++)
		{
			for(uint8_t j = 0; j < analog_led_base_data.data[i]->length; j++)
			{
				if(analog_led_base_data.data[i]->data[j].Illuminance != 0)
				{
					if( j== 0)
					{
						g_str_cob_analog_sta[i] = analog_led_base_data.data[i]->data[j].pwm;
					}
					else
					{
						g_str_cob_analog_sta[i] = analog_led_base_data.data[i]->data[j-1].pwm;
					}
					break;
				}
			}
		}
	}
#endif

#ifdef PR_308_L2
	if(mode == 1)
	{
		for(uint8_t i = 0; i < 3; i++)
		{
			for(uint8_t j = 0; j < analog_led_base_data.data[i]->length; j++)
			{
				if(analog_led_base_data.data[i]->data[j].Illuminance != 0)
				{
					if( j== 0)
					{
						g_str_cob_analog_sta[i] = analog_led_base_data.data[i]->data[j].pwm;
					}
					else
					{
						g_str_cob_analog_sta[i] = analog_led_base_data.data[i]->data[j-1].pwm;
					}
					break;
				}
			}
		}
	}
#endif
}
uint16_t led_up_limit_base_data_info(uint8_t channel,uint16_t up_limit_value)
{
	for(uint8_t j = 0; j < led_base_data.data[channel]->length; j++)
	{
		if(led_base_data.data[channel]->data[j].pwm >= up_limit_value)
		{
			return (j + 1);
		}
	}
	return led_base_data.data[channel]->length;
}
/*****************************************************************************************
* Function Name: Led_High_Speed_Mode_Ctr
* Description  : 高速摄影模式端口使能控制 pwm调光需要缓起 模拟调光不需要
* Arguments    : 0 - PWM调光， 1 - 灯控模拟调光; 2 - 灯体模拟调光
* Return Value : NONE
******************************************************************************************/
uint8_t led_high_speed_mode_ctr(uint8_t step, uint16_t fade_time, uint16_t switch_time, light_drive_mode_enum mode)
{
	static uint16_t pwm_surge_fade_time = 0xffff;//fade_time - 1
	static uint8_t deal_step = 0;
	uint8_t ret = 0;
	if(mode == PWM_MODE)
	{
		switch(step)
		{
			case 1:
			{
				pwm_surge_fade_time = fade_time + switch_time;//fade_time + switch_time 550
				deal_step = 0;
			}
			break;
			case 2:
			{
				pwm_surge_fade_time--;
				if(pwm_surge_fade_time == 1340)
				{//520
					deal_step = 1;
				}
				if(pwm_surge_fade_time == fade_time)
				{//520
					deal_step = 2;
				}
				if(pwm_surge_fade_time == 0)
				{
					deal_step = 3;
				}
			}
			break;
			case 0:
			{
				;
			}
			break;
		}
		switch(deal_step)
		{
			case 0:
			{//开始缓起
				drive_board_power_en();
				ret = 0;
			}
			break;
			case 1:
			{//模拟切pwm触发关模拟，缓起一段时间再开pwm，防闪灯
				pwm_mode_gpio_en();
				deal_step = 0;
				ret = 0;
			}
			break;
			case 2:
			{//模拟切pwm触发关模拟，缓起一段时间再开pwm，防闪灯
				pwm_mode_Surger_EN();
				Set_Fixed_Cob_Pwm(0);
				deal_step = 0;
				ret = 0;
			}
			break;
			case 3:
			{//缓起完成
				pwm_surge_fade_time = fade_time - 1;//519

				ret = 1;
			}
			break;

		}
	}
	else if(mode == ANALOG_MODE)
	{
		if( step == 1)
		{
			Set_Fixed_Cob_Pwm(0);
			cob_pwm_toggle_dac();
			ret = 3;
		}
	}
	return ret;
}
/*****************************************************************************************
* Function Name: Led_Open_Enable_Ctr
* Description  : 
* Arguments    : 0 - 关闭， 1 - 开启
* Return Value : NONE
******************************************************************************************/
void Led_Open_Enable_Ctr(uint8_t flag)
{
	if(flag)
	{
		
	}
	else
	{
		close_cob();
	}
}
/*****************************************************************************************
* Function Name: Led_Adc_Adaptive_Pare(100ms)
* Description  : 通过ADC采集自适应通道的电压值，来设置调节参数的值；
* Arguments    : void
* Return Value : NONE
******************************************************************************************/
void Led_Adc_Adaptive_Pare(void)
{

}

/*****************************************************************************************
* Function Name: led_adc_pwm_volt_state
* Description  : 判断是否完成20%PWM基准电压 
* Arguments    : index：通道
* Return Value : 0:未检测完成 ; 1:检测完成
******************************************************************************************/
uint8_t led_adc_pwm_volt_state(uint8_t index)
{
#ifdef PR_308_L2
    if(g_adc_pwm_base_volt.state[index] == 3)
		return 1;
	else
		return 0;
#endif	
#ifdef PR_307_L3
    if(g_adc_pwm_base_volt.state[index] == 3)
		return 0;
	else if(g_adc_pwm_base_volt.state[index] < 3)
		return 1;
	else
		return 2;
#endif

}


/*****************************************************************************************
* Function Name: Cob_Slow_Ctr_Logic   
* Description  : COB缓亮控制(1ms时间片)
* Arguments    : NONE
* Return Value : NONE
******************************************************************************************/
void Cob_Slow_Ctr_Logic(void)
{
	uint8_t i = 0;
	switch(g_slow_light_str.stage)
	{
		case 1://0通道 - 暖白， 1通道 - 冷白
		{
			g_slow_light_str.stage = 2;
#ifdef PR_308_L2
			for(i = 0; i < 5; i++)
			{
				g_slow_light_str.set_lux.lux[i] = (g_str_cob_slow.lux_str.lux[i] >= 0)? g_str_cob_slow.lux_str.lux[i] : 0;
				
#endif	
#ifdef PR_307_L3
			for(i = 0; i < 5; i++)
			{
				g_slow_light_str.set_lux.lux[i] = (g_str_cob_slow.lux_str.lux[i] >= 0)? g_str_cob_slow.lux_str.lux[i] : 0;
#endif
                if(g_power_factor.frq_change == 0.0f)
				{
                    g_slow_light_str.start_lux.lux[i] = dev_lux.lux[i];
                }
                else
				{
                    g_slow_light_str.start_lux.lux[i] = dev_lux.lux[i] / g_power_factor.frq_change; 
                }
				g_slow_light_str.add_lux_factor[i] = (g_slow_light_str.set_lux.lux[i] - g_slow_light_str.start_lux.lux[i]) / (float)g_rs485_data.light_mode_para.fade;
			}
		}
		break;
		case 2:
		{
			struct mixing_pwm led_pwm = {0};
			#ifdef PR_308_L2
			struct mixing_pwm led_pwm_308 = {0};
			#endif
			g_str_cob_slow.cnt++;
			if(g_str_cob_slow.cnt >= g_rs485_data.light_mode_para.fade)
			{
				
				g_str_cob_slow.cnt = 0;
				g_slow_light_str.stage = 0;
				set_dev_lux((struct mixing_lux *)g_slow_light_str.set_lux.lux);
				color_lux_to_pwm(&dev_lux, &led_pwm);
				#ifdef PR_307_L3 
				Set_Drive_Cob_Pwm_Val((uint16_t*)led_pwm.pwm);
				#endif
				#ifdef PR_308_L2
				led_pwm_308.pwm[0] = led_pwm.pwm[1];
				led_pwm_308.pwm[1] = led_pwm.pwm[0];
				led_pwm_308.pwm[4] = led_pwm.pwm[2];
				Set_Drive_Cob_Pwm_Val((uint16_t*)led_pwm_308.pwm);
				#endif
				
			}
			else
			{
				for(i = 0; i < 5; i++)
				{
					g_slow_light_str.start_lux.lux[i] += g_slow_light_str.add_lux_factor[i];
					if(g_slow_light_str.add_lux_factor[i] > 0)
					{
						if(g_slow_light_str.start_lux.lux[i] >= g_slow_light_str.set_lux.lux[i])
						{
							g_slow_light_str.start_lux.lux[i] = g_slow_light_str.set_lux.lux[i];
						}
					}
					else if(g_slow_light_str.add_lux_factor[i] < 0)
					{
						if(g_slow_light_str.start_lux.lux[i] <= g_slow_light_str.set_lux.lux[i])
						{
							g_slow_light_str.start_lux.lux[i] = g_slow_light_str.set_lux.lux[i];
						}
						#ifdef PR_307_L3
						else
						{
							if(g_slow_light_str.set_lux.lux[0] == 0 && g_slow_light_str.set_lux.lux[1] == 0 && g_slow_light_str.set_lux.lux[2] == 0 && g_slow_light_str.set_lux.lux[3] == 0 && g_slow_light_str.set_lux.lux[4] == 0)
							{
								if(g_slow_light_str.start_lux.lux[i] <= g_slow_light_str.down_limit_lux_info[3] || g_slow_light_str.start_lux.lux[i] <= g_slow_light_str.down_limit_lux_info[4])
								{
									g_slow_light_str.start_lux.lux[i] = g_slow_light_str.set_lux.lux[i];
								}
							}
						}
						#endif
					}
					g_slow_light_str.start_lux.lux[i] = g_slow_light_str.start_lux.lux[i] < 0.0f ? 0.0f : g_slow_light_str.start_lux.lux[i];
				}
				set_dev_lux((struct mixing_lux *)g_slow_light_str.start_lux.lux);
				color_lux_to_pwm(&dev_lux, &led_pwm);
				#ifdef PR_307_L3 
				Set_Drive_Cob_Pwm_Val((uint16_t*)led_pwm.pwm);
				#endif
				#ifdef PR_308_L2
				led_pwm_308.pwm[0] = led_pwm.pwm[1];
				led_pwm_308.pwm[1] = led_pwm.pwm[0];
				led_pwm_308.pwm[4] = led_pwm.pwm[2];
				Set_Drive_Cob_Pwm_Val((uint16_t*)led_pwm_308.pwm);
				#endif
			}
		}
		break;
		default:break;
	}
}

void set_drive_pwm_val(uint8_t index, uint16_t p_pwm)
{
	 if(p_pwm * g_power_factor.frq_change)
	{
		TMRA_PWM_Cmd(g_led_pwm_channel.p_tim[index], g_led_pwm_channel.p_ch[index], Enable);
	}
	else
	{
		TMRA_PWM_Cmd(g_led_pwm_channel.p_tim[index], g_led_pwm_channel.p_ch[index], Disable);
	}
	*g_led_pwm_channel.p_ch_info[index] = p_pwm * g_power_factor.frq_change;
}

uint16_t value = 0xfff;
float dac_dist_test_1 = 3000.0;
#ifdef PR_308_L2
uint16_t DAC_WW_Value = 2100;
uint16_t DAC_CW_Value = 2150;
#endif	

void Set_Drive_Cob_Every_Pwm_Val(uint16_t* p_pwm, uint8_t light_state)
{
	if(get_run_state() != LAMP_ON || g_self_adapt_version_not_match == 1)
		return;
	uint8_t i = 0;
    static uint16_t dac_value;
	static uint16_t last_p_pwm[7] = {0};

	#if 1
	if(light_state == 1)
	{
		for(i = 0; i < 7; i++)
		{
			p_pwm[i] = (uint16_t)(((float)g_single_power_limit.single_power_calibration[i] /9000.0f)*(float)p_pwm[i]);
		}
	}
	#endif
	g_led_pwm_arg.led_pwm[0] = p_pwm[0];
	g_led_pwm_arg.led_pwm[1] = p_pwm[1];
	g_led_pwm_arg.led_pwm[2] = p_pwm[2];
	g_led_pwm_arg.led_pwm[3] = p_pwm[3];
	g_led_pwm_arg.led_pwm[4] = p_pwm[4];
	g_led_pwm_arg.led_pwm[5] = p_pwm[5];
	g_led_pwm_arg.led_pwm[6] = p_pwm[6];
    for(i = 0; i < 7; i++)
	{
        dac_value = 4095 - (2000 * (1 - (p_pwm[i]) / dac_dist_test_1));
	 
        //对应灯体模拟调光电压
        if(p_pwm[i] > (uint16_t)dac_dist_test_1)
        {
           dac_value = 4095;
        }
        if(dac_value != g_led_pwm_channel.p_ch_adc[i] || last_p_pwm[i] != p_pwm[i])
        {
			set_lamp_drive_cob_dac(dac_value, i);
            g_led_pwm_channel.p_ch_adc[i] = dac_value;
        }
		
		if((p_pwm[i] >= 1) && g_error_state.ambient_temp_eh_016 == 0 
			&& g_error_state.lamp_with_protect_cover_eh_001 == 0 && g_error_state.mcu_temp_eh_003 == 0 && g_rs485_data.illumination_mode.ratio != 0.0f)
		{
#ifdef PR_308_L2
			if(i == 2)
			{
				for(uint8_t j = 0; j < LAMP_FREQUENCY_DIV; j++)
				{
					if((p_pwm[i] % LAMP_FREQUENCY_DIV) / (j+1) != 0)
					{
						g_led_pwm_arg.pwm_buffer_two[j][1] = p_pwm[i] / LAMP_FREQUENCY_DIV + 1;
					}
					else
					{
						g_led_pwm_arg.pwm_buffer_two[j][1] = p_pwm[i] / LAMP_FREQUENCY_DIV + 0;
					}
				}
			}
			else if(i == 5)
			{
				for(uint8_t j = 0; j < LAMP_FREQUENCY_DIV; j++)
				{
					if((p_pwm[i] % LAMP_FREQUENCY_DIV) / (j+1) != 0)
					{
						g_led_pwm_arg.pwm_buffer_one[j][2] = p_pwm[i] / LAMP_FREQUENCY_DIV + 1;
					}
					else
					{
						g_led_pwm_arg.pwm_buffer_one[j][2] = p_pwm[i] / LAMP_FREQUENCY_DIV + 0;
					}
				}
			}
			else
			{
#endif
				if(i < LAMP_DRV_ONE_CHANNEL_NUM)
				{
					for(uint8_t j = 0; j < LAMP_FREQUENCY_DIV; j++)
					{
						if((p_pwm[i] % LAMP_FREQUENCY_DIV) / (j+1) != 0)
						{
							g_led_pwm_arg.pwm_buffer_one[j][i] = p_pwm[i] / LAMP_FREQUENCY_DIV + 1;
						}
						else
						{
							g_led_pwm_arg.pwm_buffer_one[j][i] = p_pwm[i] / LAMP_FREQUENCY_DIV + 0;
						}
					}
				}
				else
				{
					for(uint8_t j = 0; j < LAMP_FREQUENCY_DIV; j++)
					{
						if((p_pwm[i] % LAMP_FREQUENCY_DIV) / (j+1) != 0)
						{
							g_led_pwm_arg.pwm_buffer_two[j][6-i] = p_pwm[i] / LAMP_FREQUENCY_DIV + 1;
						}
						else
						{
							g_led_pwm_arg.pwm_buffer_two[j][6-i]= p_pwm[i] / LAMP_FREQUENCY_DIV + 0;
						}
					}
				}
#ifdef PR_308_L2
			}
#endif

			Channel_Output_Control( i, Enable);
		}
		else
		{
			Channel_Output_Control( i, Disable);
		}	
		
		last_p_pwm[i] = p_pwm[i];
    }       
}
/*****************************************************************************************
* Function Name : Set_Drive_Cob_Pwm_Val2
* Description   : 
* Arguments     : 
* Return Value	: void
******************************************************************************************/
void Cob_Output_Disable(void)
{
	for(uint8_t j = 0; j < 5; j++)
	{
		g_slow_light_str.start_lux.lux[j] = 0;
	}
	set_dev_lux((struct mixing_lux *)g_slow_light_str.start_lux.lux);
	for(uint8_t i = 0; i < 7; i++)
	{
		Channel_Output_Control( i, Disable);	
	}
}
void Cob_Output_Enable(void)
{
	for(uint8_t i = 0; i < 7; i++)
	{
		Channel_Output_Control( i, Enable);
	}
}
/*****************************************************************************************
* Function Name : Set_Drive_Cob_Pwm_Val2
* Description   : 
* Arguments     : 
* Return Value	: void
******************************************************************************************/
void Set_Fixed_Cob_Pwm(uint16_t pwm)
{
	uint16_t p_pwm[7];
	for(uint8_t i = 0; i < 7; i++)
	{
		p_pwm[i] = pwm;
	}
	for(uint8_t j = 0; j < 5; j++)
	{
		g_slow_light_str.start_lux.lux[j] = 0;
	}
	set_dev_lux((struct mixing_lux *)g_slow_light_str.start_lux.lux);
	Set_Drive_Cob_Every_Pwm_Val(p_pwm,1);
}
/*****************************************************************************************
* Function Name : Set_Drive_Cob_Pwm_Val
* Description   : 
* Arguments     : 
* Return Value	: void
******************************************************************************************/
void Set_Drive_Cob_Pwm_Val(uint16_t* p_pwm)
{
	if(get_run_state() != LAMP_ON){
		return;
	}
	if(0 == g_rs485_data.high_speed.mode)
	{
		#ifdef PR_307_L3
		if(p_pwm[0] == 0 && p_pwm[1] == 0 && p_pwm[2] == 0 && p_pwm[3] == 0 && p_pwm[4] == 0)
		{
		#endif
		#ifdef PR_308_L2
		if(p_pwm[0] == 0 && p_pwm[1] == 0 && p_pwm[4] == 0)
		{
		#endif
			struct mixing_lux temp_dev_lux = {0,0,0,0,0};
			set_dev_lux((struct mixing_lux*)temp_dev_lux.lux);
		}
#ifdef PR_308_L2	
		uint16_t pwm_t[7];
		pwm_t[0] = p_pwm[0];//g
		pwm_t[1] = p_pwm[1];//ww
		pwm_t[2] = p_pwm[1];//ww
		pwm_t[3] = p_pwm[1];//ww
		pwm_t[4] = p_pwm[4];//cw
		pwm_t[5] = p_pwm[4];//cw
		pwm_t[6] = p_pwm[4];//cw
		Set_Drive_Cob_Every_Pwm_Val(pwm_t,1);
#endif	
#ifdef PR_307_L3
		uint16_t pwm_t[7];
		pwm_t[0] = p_pwm[0];
		pwm_t[1] = p_pwm[1];
		pwm_t[2] = p_pwm[2];
		pwm_t[3] = p_pwm[3];
		pwm_t[4] = p_pwm[4];
		pwm_t[5] = p_pwm[3];
		pwm_t[6] = p_pwm[4];
		Set_Drive_Cob_Every_Pwm_Val(pwm_t,1);
		#endif	
	}
}

void set_dev_lux(struct mixing_lux *set_lux)
{
//	memcpy(&dev_lux, &set_lux, sizeof(struct mixing_lux));
	for(uint8_t i = 0;i < 5; i++)
	{
		dev_lux.lux[i] = set_lux->lux[i];
	}
}
void set_self_adapt_pwm(uint16_t* p_pwm)
{
	#ifdef PR_308_L2	
		g_rs485_data.self_adapt_pwm.pwm[0] = (uint16_t)(((float)g_single_power_limit.single_power_calibration[0] /9000.0f)*(float)p_pwm[0]);//g
		g_rs485_data.self_adapt_pwm.pwm[1] = (uint16_t)(((float)g_single_power_limit.single_power_calibration[1] /9000.0f)*(float)p_pwm[1]);//ww
		g_rs485_data.self_adapt_pwm.pwm[2] = (uint16_t)(((float)g_single_power_limit.single_power_calibration[2] /9000.0f)*(float)p_pwm[1]);//ww
		g_rs485_data.self_adapt_pwm.pwm[3] = (uint16_t)(((float)g_single_power_limit.single_power_calibration[3] /9000.0f)*(float)p_pwm[1]);//ww
		g_rs485_data.self_adapt_pwm.pwm[4] = (uint16_t)(((float)g_single_power_limit.single_power_calibration[4] /9000.0f)*(float)p_pwm[4]);//cw
		g_rs485_data.self_adapt_pwm.pwm[5] = (uint16_t)(((float)g_single_power_limit.single_power_calibration[5] /9000.0f)*(float)p_pwm[4]);//cw
		g_rs485_data.self_adapt_pwm.pwm[6] = (uint16_t)(((float)g_single_power_limit.single_power_calibration[6] /9000.0f)*(float)p_pwm[4]);//cw
	#endif	
	#ifdef PR_307_L3
		g_rs485_data.self_adapt_pwm.pwm[0] = (uint16_t)(((float)g_single_power_limit.single_power_calibration[0] /9000.0f)*(float)p_pwm[0]);
		g_rs485_data.self_adapt_pwm.pwm[1] = (uint16_t)(((float)g_single_power_limit.single_power_calibration[1] /9000.0f)*(float)p_pwm[1]);
		g_rs485_data.self_adapt_pwm.pwm[2] = (uint16_t)(((float)g_single_power_limit.single_power_calibration[2] /9000.0f)*(float)p_pwm[2]);
		g_rs485_data.self_adapt_pwm.pwm[3] = (uint16_t)(((float)g_single_power_limit.single_power_calibration[3] /9000.0f)*(float)p_pwm[3]);
		g_rs485_data.self_adapt_pwm.pwm[4] = (uint16_t)(((float)g_single_power_limit.single_power_calibration[4] /9000.0f)*(float)p_pwm[4]);
		g_rs485_data.self_adapt_pwm.pwm[5] = (uint16_t)(((float)g_single_power_limit.single_power_calibration[5] /9000.0f)*(float)p_pwm[3]);
		g_rs485_data.self_adapt_pwm.pwm[6] = (uint16_t)(((float)g_single_power_limit.single_power_calibration[6] /9000.0f)*(float)p_pwm[4]);
	#endif
}
/***********************************END OF FILE*******************************************/
