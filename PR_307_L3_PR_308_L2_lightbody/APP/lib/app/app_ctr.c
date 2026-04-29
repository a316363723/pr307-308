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
#include "fan.h"
#include "light_effect_interface.h"
#include "define.h"
#include "flash.h"
#include "data_cmp.h"
#include "bsp_timer.h"
#include "bsp_power.h"
#include "app_led.h"
#include "SidusProFX.h"
#include "SidusProFile.h"
#include "Rs485_Proto_Slave_Analysis.h"
#include "stdio.h"
#include "color_mixing.h"
#include "lfs_flash.h"
#include "app_store.h"
#include "bsp_iic.h"
#include "hc32f4a0_gpio.h"
#include "color_mixing_dev.h"
#include "bsp_electronic.h"
/*****************************************************************************************
							  Variables definitions
*****************************************************************************************/
static uint8_t s_device_run_state = 0;/*设备运行状态 0 - 关闭，1 - 开启*/
static uint8_t s_cob_temperature_flag = 0;/*cob温度状态：0 - 没有出现降功率的温度，1 - 出现降功率温度， 2 - 关机温度*/
static uint8_t s_here_fx_flag = 0;/*定位光效运行标志，0 - 没有运行，1 - 运行中*/

rs485_data_ctr_t g_rs485_data = {0};
struct color_mixing g_color_mixing_struct;
Light_Effect g_effect_struct;
SidusProFX_ArgBody_Type g_sidus_fx_str;
/*****************************************************************************************
							  Functions definitions
*****************************************************************************************/
static void Cob_Slow_Bright_Recognize(void);
static uint16_t sys_fx_power_cut(uint16_t lightness, float illumination_rate);

/*****************************************************************************************
* Function Name: Set_Device_Run_State
* Description  : 设置设备运行状态
* Arguments    : state：0 - 关机状态，1 - 开机状态
* Return Value : NONE
******************************************************************************************/
void Set_Device_Run_State(uint8_t state)
{
    s_device_run_state = state;
}

uint8_t get_run_state(void)
{
	return  s_device_run_state;
}
/*****************************************************************************************
* Function Name: Polling_Here_Fx_State
* Description  : 轮询定位光效状态
* Arguments    : none
* Return Value : NONE
******************************************************************************************/
void Polling_Here_Fx_State(void)
{
    if(1 == s_here_fx_flag)
	{
        if(1 == Get_EffectTypeIAmHere_Status())
		{/*定位光效运行结束*/
            s_here_fx_flag = 0;
            Light_Effect_Enable(false);
            g_rs485_data.new_command.light_mode = 1;/*重新启动光模式*/
            g_rs485_data.fx_restart = 1;/*光效重新启动*/
            Clear_EffectTypeIAmHere_Status();
        }
    }
}

/*****************************************************************************************
* Function Name: RS485_Logic_Ctr
* Description  : 485逻辑(1ms时间片)
* Arguments    : NONE
* Return Value : NONE
******************************************************************************************/
static uint8_t set_sys_fx1_light_para(Effect_Mode_Arg *p_obj, Effect_Mode *run_mode, const fx_mode_para_t *p_source);
#ifdef PR_307_L3
static uint8_t set_sys_fx2_light_para(Effect_Mode_Arg *p_obj, Effect_Mode *run_mode, const fx_mode_para_t *p_source);
static uint8_t set_sys_fx2_range_light_para(Effect_Mode_Arg *p_lower_obj, Effect_Mode_Arg *p_upper_obj, 
                                            Effect_Mode *lower_run_mode, Effect_Mode *upper_run_mode, 
                                            const fx_mode_para_t *p_source);
#endif
void RS485_Logic_Ctr(void)
{
	update_power_rate();
	led_high_speed_mode_ctr(2, 1000, 460, (light_drive_mode_enum)g_rs485_data.high_speed.mode);
    if(1 == g_rs485_data.new_command.on_off)
	{//开关机标志位处理
        g_rs485_data.new_command.on_off = 0;
		Electronic_Buf.Bracket_Times = 0;
		Electronic_Buf.Bayonet_Times = 0;
        if(0 == g_rs485_data.power_switch.state)
		{
            Set_Device_Run_State(LAMP_OFF);
        }
        else
		{
			s_here_fx_flag = 0;
			g_rs485_data.new_command.light_mode = 1;/*重新启动光模式*/
			g_rs485_data.fx_restart = 1;/*光效重新启动*/
			Set_Device_Run_State(LAMP_ON);
			fan_water_pid_pwm_zero(0);
            /*风扇模块使能*/
            Set_Fan_Run_State(1);		
            /*COB驱动模块使能*/
            Led_Open_Enable_Ctr(1);
            /*高温标志清零*/
            s_cob_temperature_flag = 0;
            g_power_factor.high_temp_rate = 1.0f;
            update_power_rate(); //修改了
            #if(1 == GLOBAL_PRINT_ENABLE)
            printf("open.\n");
            #endif
        }
    }
    if(1 == g_rs485_data.new_command.here_fx)
	{
        g_rs485_data.new_command.here_fx = 0;
        if(0 == g_rs485_data.i_am_here.state)
		{
            Light_Effect_Enable(true);
            SidusProFX_Enable(false);
            g_effect_struct.effect_type = EffectTypeIAmHere;
            g_effect_struct.effect_arg.i_am_here.run = g_rs485_data.i_am_here.state;//Sean:寻灯
            Effect_Data_Init(&g_effect_struct, 1/*g_rs485_data.fx_restart*/);
            s_here_fx_flag = 1;
            Clear_EffectTypeIAmHere_Status();
            Led_Open_Enable_Ctr(1);
        }
        else
		{
            Light_Effect_Enable(false);
            g_rs485_data.new_command.light_mode = 1;/*重新启动光模式*/
            g_rs485_data.fx_restart = 1;/*光效重新启动*/
            s_here_fx_flag = 0;
            Clear_EffectTypeIAmHere_Status();
        }
    }
    if(1 == g_rs485_data.new_command.Illumination_Mode)
	{//功率限制
        g_rs485_data.new_command.Illumination_Mode = 0;
        g_rs485_data.new_command.light_mode = 1;/*重新启动光模式*/
        g_rs485_data.fx_restart = 1;/*光效重新启动*/
        g_power_factor.illumination = (float)g_rs485_data.illumination_mode.ratio / 10000.0f;
		if(g_power_factor.illumination == 0)
			g_power_factor.illumination = 1;
        if(1 == g_rs485_data.illumination_mode.save_ctr)
        {
            g_rs485_data.illumination_mode.save_ctr = 0;
            set_power_calibration_state(1);
			g_power_limit.power_calibration[(light_drive_mode_enum)g_rs485_data.high_speed.mode] = g_power_factor.illumination;
			set_max_power(g_power_limit.power_calibration[(light_drive_mode_enum)g_rs485_data.high_speed.mode]);
            g_power_factor.illumination = 1.0f;
        }
        update_power_rate();
    }
    if(1 == g_rs485_data.new_command.frq)
	{
        g_rs485_data.new_command.frq = 0;
    }
    if(1 == g_rs485_data.new_command.dim)
	{
        g_rs485_data.new_command.dim = 0;
        g_rs485_data.new_command.light_mode = 1;/*重新启动光模式*/
        g_rs485_data.fx_restart = 0;/*光效正常运行*/
    }
	if(1 == g_rs485_data.new_command.adjust_hight_mode)
	{
		//需要重新初始化合光模块
		if(get_fan_state())
		{
			g_rs485_data.new_command.adjust_hight_mode = 0;
			color_mixing_init(g_rs485_data.high_speed.mode);
			led_down_limit_base_data_info(g_rs485_data.high_speed.mode);

			if(get_run_state() != LAMP_FAN_ERROR_OFF && g_error_state.ambient_temp_eh_016 == 0)//get_run_state() != LAMP_TEMP_ERROR_OFF)
			{
				led_high_speed_mode_ctr(1, 1000, 460, (light_drive_mode_enum)g_rs485_data.high_speed.mode);//开始缓起
			}

			if(g_rs485_data.fan.mode == 4)
			{
				g_power_factor.silence_mode_rate = silence_mode_power_rate[g_rs485_data.high_speed.mode];
				update_power_rate();
			}
		}
	}	
    if(1 == g_rs485_data.new_command.fan)
	{
		static uint8_t last_fan_mode = 0;
        g_rs485_data.new_command.fan = 0;
		if((last_fan_mode == 4 && g_rs485_data.fan.mode !=4) || (last_fan_mode != 4 && g_rs485_data.fan.mode ==4))
		{
			g_rs485_data.new_command.light_mode = 1;/*重新启动光模式*/
			g_rs485_data.fx_restart = 1;/*光效重新启动*/
		}
		fan_mode_set_min_lightness();		
        switch(g_rs485_data.fan.mode)
		{
            case 0:
			{
                Set_Fan_Run_Speed(g_rs485_data.fan.speed, 0);
                Set_Fan_Run_State(1);
                Set_Fan_Run_Mode(0);
				Set_Fan_Run_Speed(PUMP_NORMAL_SPEED, 1);
				g_power_factor.silence_mode_rate = 1.0f;
				update_power_rate();
            }
            break;
            case 1://智能模式
			{
                Set_Fan_Run_State(1);
                Set_Fan_Run_Mode(1);
				#ifdef PR_307_L3
				Set_Fan_Run_Speed(PUMP_NORMAL_SPEED, 1);
				#endif
				g_power_factor.silence_mode_rate = 1.0f;
				update_power_rate();
            }
            break;
            case 2://高速模式
			{
				Set_Fan_Run_State(1);
				Set_Fan_Run_Speed(FAN_HIGH_SPEED, 0);
				Set_Fan_Run_Speed(PUMP_NORMAL_SPEED, 1);
				Set_Fan_Run_Mode(0);
				g_power_factor.silence_mode_rate = 1.0f;
				update_power_rate();
            }
            break;
            case 3://中速模式
			{
				Set_Fan_Run_State(1);
				Set_Fan_Run_Speed(FAN_MIDDLE_SPEED, 0);
				Set_Fan_Run_Speed(PUMP_NORMAL_SPEED, 1);
				Set_Fan_Run_Mode(0);
				g_power_factor.silence_mode_rate = 1.0f;
				update_power_rate();
            }
            break;
            case 4://静音模式
			{
                Set_Fan_Run_State(1);
				Set_Fan_Run_Speed(FAN_SILENT_SPEED, 0);
				Set_Fan_Run_Speed(PUMP_SILENT_SPEED, 1);
				Set_Fan_Run_Mode(0);
				g_power_factor.silence_mode_rate = silence_mode_power_rate[g_rs485_data.high_speed.mode];
				update_power_rate();
            }
            break;
            default:break;
        }
		last_fan_mode = g_rs485_data.fan.mode;
    }
	if(1 == g_rs485_data.new_command.pump_t)
	{
        g_rs485_data.new_command.pump_t = 0;
        switch(g_rs485_data.pump_t.mode)
		{
            case 0:
			{
                Set_Fan_Run_Speed(g_rs485_data.pump_t.speed, 1);
                Set_Fan_Run_State(1);
            }
            break;
            default:break;
        }
    }
    if(1 == g_rs485_data.new_command.upgrade)
	{
        g_rs485_data.new_command.upgrade = 0;
        
    }
    if(1 == g_rs485_data.new_command.light_mode)
	{
		if( led_high_speed_mode_ctr(0, 10000, 460, (light_drive_mode_enum)g_rs485_data.high_speed.mode) )
		{
			g_rs485_data.new_command.light_mode = 0;
			switch(g_rs485_data.light_mode_state.command_1)
			{
				case RS485_Cmd_CCT:
				{
					Light_Effect_Enable(false);
					SidusProFX_Enable(false);
					color_cct_calc(g_rs485_data.light_mode_para.intensity, g_rs485_data.light_mode_para.cct, 
					g_rs485_data.light_mode_para.duv, (enum ilumination_mode)g_rs485_data.illumination_mode.mode, (enum mixing_curve)g_rs485_data.dimming_curve.curve, 
					g_power_factor.sum_power_rate, &g_str_cob_slow.pwm_str,&g_str_cob_slow.lux_str);
#ifdef PR_308_L2
					if( g_rs485_data.light_mode_para.intensity == 0 )
					{
						g_str_cob_slow.pwm_str.pwm[0] = 0;
//					Set_Fixed_Cob_Pwm(0);
					}
#endif
					Cob_Slow_Bright_Recognize();
				}
				break;
				case RS485_Cmd_Factory_RGBWW:
				{
					static uint16_t led_duty[LED_CHANNEL_NUM] = {0};
					Light_Effect_Enable(false);
					SidusProFX_Enable(false);
#ifdef PR_308_L2
					if((g_rs485_data.light_mode_para.ww2_ratio == 12000) && (g_rs485_data.light_mode_para.cw2_ratio == 12000))
					{
//						led_duty[0] = g_rs485_data.light_mode_para.green_ratio;
//						led_duty[1] = g_rs485_data.light_mode_para.ww_ratio; 
//						led_duty[4] = g_rs485_data.light_mode_para.cw_ratio;
						led_duty[0] = g_rs485_data.light_mode_para.green_ratio;
						led_duty[1] = g_rs485_data.light_mode_para.red_ratio;
						led_duty[2] = g_rs485_data.light_mode_para.red_ratio;
						led_duty[3] = g_rs485_data.light_mode_para.red_ratio;					
						led_duty[4] = g_rs485_data.light_mode_para.blue_ratio;
						led_duty[5] = g_rs485_data.light_mode_para.blue_ratio;
						led_duty[6] = g_rs485_data.light_mode_para.blue_ratio;
//						Set_Drive_Cob_Every_Pwm_Val(led_duty,0);
					}
					else
					{
						led_duty[0] = g_rs485_data.light_mode_para.red_ratio;
						led_duty[1] = g_rs485_data.light_mode_para.green_ratio;
						led_duty[2] = g_rs485_data.light_mode_para.blue_ratio; 
						led_duty[3] = g_rs485_data.light_mode_para.ww_ratio; 
						led_duty[4] = g_rs485_data.light_mode_para.cw_ratio;
						led_duty[5] = g_rs485_data.light_mode_para.ww2_ratio; 
						led_duty[6] = g_rs485_data.light_mode_para.cw2_ratio; 		
//						Set_Drive_Cob_Every_Pwm_Val(led_duty,0);
					}
#endif
	
#ifdef PR_307_L3						
					led_duty[0] = g_rs485_data.light_mode_para.red_ratio;
					led_duty[1] = g_rs485_data.light_mode_para.green_ratio;
					led_duty[2] = g_rs485_data.light_mode_para.blue_ratio; 
					led_duty[3] = g_rs485_data.light_mode_para.ww_ratio; 
					led_duty[4] = g_rs485_data.light_mode_para.cw_ratio;
					led_duty[5] = g_rs485_data.light_mode_para.ww2_ratio; 
					led_duty[6] = g_rs485_data.light_mode_para.cw2_ratio; 		
//					Set_Drive_Cob_Every_Pwm_Val(led_duty,0); 
#endif
					for(uint8_t i = 0; i < 7;i++)
					{
						g_rs485_data.self_adapt_pwm.pwm[i] = led_duty[i];
					}
					Set_Drive_Cob_Every_Pwm_Val(led_duty,1); 
				}
				break;
				case RS485_Cmd_Factory_RGBWW_Crc:
				{
					uint8_t save_cnt = 0;
					Light_Effect_Enable(false);
					SidusProFX_Enable(false);
					g_str_single_crc_cob_slow.pwm_str[0] = g_rs485_data.factory_rgbww_crc_t.red;
					g_str_single_crc_cob_slow.pwm_str[1] = g_rs485_data.factory_rgbww_crc_t.green;
					g_str_single_crc_cob_slow.pwm_str[2] = g_rs485_data.factory_rgbww_crc_t.blue;
					g_str_single_crc_cob_slow.pwm_str[3] = g_rs485_data.factory_rgbww_crc_t.ww;
					g_str_single_crc_cob_slow.pwm_str[4] = g_rs485_data.factory_rgbww_crc_t.cw;
					g_str_single_crc_cob_slow.pwm_str[5] = g_rs485_data.factory_rgbww_crc_t.ww2;
					g_str_single_crc_cob_slow.pwm_str[6] = g_rs485_data.factory_rgbww_crc_t.cw2;
					for(uint8_t i = 0; i < 7; i++)
					{
						if(g_rs485_data.factory_rgbww_crc_t.state[i] == 1 && g_str_single_crc_cob_slow.pwm_str[i] > 0)
						{
							set_single_power_calibration_state(i,1);
							g_single_power_limit.single_power_calibration[i] = g_str_single_crc_cob_slow.pwm_str[i];
							save_cnt++;
						}
					}
					if(save_cnt == 0)
					{
						Set_Drive_Cob_Every_Pwm_Val(g_str_single_crc_cob_slow.pwm_str,0);
						for(uint8_t i = 0; i < 7;i++)
						{
							g_rs485_data.self_adapt_pwm.pwm[i] = g_str_single_crc_cob_slow.pwm_str[i];
						}
					}
				}
				break;
				case RS485_Cmd_DMX_Strobe:{
					Light_Effect_Enable(true);
					SidusProFX_Enable(false);
					g_effect_struct.effect_type = EffectTypeDMXStrobe;
					Effect_Data_Init(&g_effect_struct, g_rs485_data.fx_restart);
				}
				break;
#ifdef PR_307_L3
				case RS485_Cmd_HSI:
				{
					Light_Effect_Enable(false);
					SidusProFX_Enable(false);
					color_hsi_calc(g_rs485_data.light_mode_para.intensity, (float)(g_rs485_data.light_mode_para.hue / 100.0f), (float)(g_rs485_data.light_mode_para.sat / 100.0f), //Phoebe:数据类型
					g_rs485_data.light_mode_para.cct, 0.0f,(enum mixing_curve)g_rs485_data.dimming_curve.curve, g_power_factor.sum_power_rate, &g_str_cob_slow.pwm_str, &g_str_cob_slow.lux_str);
					Cob_Slow_Bright_Recognize();
				}
				break;
				case RS485_Cmd_GEL:
				{
					Light_Effect_Enable(false);
					SidusProFX_Enable(false);
					color_gel_calc(g_rs485_data.light_mode_para.intensity, g_rs485_data.light_mode_para.origin, 
								g_rs485_data.light_mode_para.series, g_rs485_data.light_mode_para.cct, g_rs485_data.light_mode_para.color, \
							(enum mixing_curve)g_rs485_data.dimming_curve.curve, g_power_factor.sum_power_rate, &g_str_cob_slow.pwm_str, &g_str_cob_slow.lux_str);
					Cob_Slow_Bright_Recognize();
				}
				break;
				case RS485_Cmd_RGB:
				{
					Light_Effect_Enable(false);
					SidusProFX_Enable(false);
					color_rgb_calc(g_rs485_data.light_mode_para.intensity, (float)g_rs485_data.light_mode_para.red_ratio / 1000, (float)g_rs485_data.light_mode_para.green_ratio / 1000,
						(float)g_rs485_data.light_mode_para.blue_ratio / 1000,(enum mixing_curve)g_rs485_data.dimming_curve.curve, g_power_factor.sum_power_rate, &g_str_cob_slow.pwm_str, &g_str_cob_slow.lux_str);
					Cob_Slow_Bright_Recognize();
				}
				break;
				case RS485_Cmd_XY_Coordinate:
				{
					Light_Effect_Enable(false);
					SidusProFX_Enable(false);
					 color_coordinate_calc(g_rs485_data.light_mode_para.intensity, (float)g_rs485_data.light_mode_para.x / 10000.f,
						(float)g_rs485_data.light_mode_para.y / 10000.f, (enum mixing_curve)g_rs485_data.dimming_curve.curve, g_power_factor.sum_power_rate, &g_str_cob_slow.pwm_str, &g_str_cob_slow.lux_str);		
					Cob_Slow_Bright_Recognize();
				}
				break;
				case RS485_Cmd_Source:
				{
					Light_Effect_Enable(false);
					SidusProFX_Enable(false);
					color_source_calc(g_rs485_data.light_mode_para.intensity, g_rs485_data.light_mode_para.type, (float)g_rs485_data.light_mode_para.x / 10000.f, (float)g_rs485_data.light_mode_para.y / 10000.f, g_rs485_data.illumination_mode.mode, 
							   (enum mixing_curve)g_rs485_data.dimming_curve.curve, g_power_factor.sum_power_rate, &g_str_cob_slow.pwm_str, &g_str_cob_slow.lux_str);
					Cob_Slow_Bright_Recognize();
				}
				break;
#endif
				case RS485_Cmd_Sys_FX:
				{
					Light_Effect_Enable(true);
					SidusProFX_Enable(false);	
					switch(g_rs485_data.light_mode_state.command_2)
					{
#ifdef PR_307_L3
						case RS485_FX_ClubLights:
						{
							g_effect_struct.effect_type = EffectTypeClubLights;
							g_effect_struct.effect_arg.club_lights_arg.lightness = sys_fx_power_cut(g_rs485_data.fx_mode_para.intensity, g_power_factor.illumination);
							g_effect_struct.effect_arg.club_lights_arg.frq = g_rs485_data.fx_mode_para.frq;
							g_effect_struct.effect_arg.club_lights_arg.color = g_rs485_data.fx_mode_para.color_num;
							Effect_Data_Init(&g_effect_struct, g_rs485_data.fx_restart);
						}
						break;
						case RS485_FX_Candle:
						{
							g_effect_struct.effect_type = EffectTypeCandle;
							g_effect_struct.effect_arg.candle_arg.lightness = sys_fx_power_cut(g_rs485_data.fx_mode_para.intensity, g_power_factor.illumination);
							g_effect_struct.effect_arg.candle_arg.frq = g_rs485_data.fx_mode_para.frq;
							g_effect_struct.effect_arg.candle_arg.cct = g_rs485_data.fx_mode_para.cct;
							Effect_Data_Init(&g_effect_struct, g_rs485_data.fx_restart);
						}
						break;
						case RS485_FX_Welding:
						{
							g_effect_struct.effect_type = EffectTypeWelding;
							g_effect_struct.effect_arg.welding_arg.frq = g_rs485_data.fx_mode_para.frq;
							g_effect_struct.effect_arg.welding_arg.min = g_rs485_data.fx_mode_para.min_int;
							if(0 == set_sys_fx1_light_para(&g_effect_struct.effect_arg.welding_arg.arg, 
														   &g_effect_struct.effect_arg.welding_arg.mode, 
														   &g_rs485_data.fx_mode_para))
							{
								Effect_Data_Init(&g_effect_struct, g_rs485_data.fx_restart);
							}
						}
						break;
						case RS485_FX_CopCar:
						{
							g_effect_struct.effect_type = EffectTypeCopCar;
							g_effect_struct.effect_arg.cop_car_arg.lightness = sys_fx_power_cut(g_rs485_data.fx_mode_para.intensity, g_power_factor.illumination);
							g_effect_struct.effect_arg.cop_car_arg.frq = g_rs485_data.fx_mode_para.frq;
							g_effect_struct.effect_arg.cop_car_arg.color = g_rs485_data.fx_mode_para.color_num;
							Effect_Data_Init(&g_effect_struct, g_rs485_data.fx_restart);
						}
						break;
						case RS485_FX_ColorChase:
						{
							g_effect_struct.effect_type = EffectTypeColorChase;
							g_effect_struct.effect_arg.color_chase_arg.lightness = sys_fx_power_cut(g_rs485_data.fx_mode_para.intensity, g_power_factor.illumination);
							g_effect_struct.effect_arg.color_chase_arg.frq = g_rs485_data.fx_mode_para.frq;
							g_effect_struct.effect_arg.color_chase_arg.sat = g_rs485_data.fx_mode_para.sat;
							Effect_Data_Init(&g_effect_struct, g_rs485_data.fx_restart);
						}
						break;
						case RS485_FX_PartyLights:
						{
							g_effect_struct.effect_type = EffectTypePartyLights;
							g_effect_struct.effect_arg.party_lights_arg.lightness = sys_fx_power_cut(g_rs485_data.fx_mode_para.intensity, g_power_factor.illumination);
							g_effect_struct.effect_arg.party_lights_arg.frq = g_rs485_data.fx_mode_para.frq;
							g_effect_struct.effect_arg.party_lights_arg.sat = g_rs485_data.fx_mode_para.sat;
							Effect_Data_Init(&g_effect_struct, g_rs485_data.fx_restart);
						}
						break;
#endif						
						case RS485_FX_Paparazzi:
						{
							g_effect_struct.effect_type = EffectTypePaparazzi;
							g_effect_struct.effect_arg.paparazzi_arg.mode = EffectModeCCT;
							g_effect_struct.effect_arg.paparazzi_arg.arg.cct_arg.lightness = sys_fx_power_cut(g_rs485_data.fx_mode_para.intensity, g_power_factor.illumination);
							g_effect_struct.effect_arg.paparazzi_arg.arg.cct_arg.cct = g_rs485_data.fx_mode_para.cct;
							g_effect_struct.effect_arg.paparazzi_arg.arg.cct_arg.gm = g_rs485_data.fx_mode_para.gm;
							g_effect_struct.effect_arg.paparazzi_arg.frq = g_rs485_data.fx_mode_para.frq;
							Effect_Data_Init(&g_effect_struct, g_rs485_data.fx_restart);
						}
						break;
						case RS485_FX_Lightning:
						{
							g_effect_struct.effect_type = EffectTypeLightning;
							g_effect_struct.effect_arg.lightning_arg.mode = EffectModeCCT;
							g_effect_struct.effect_arg.lightning_arg.arg.cct_arg.lightness = sys_fx_power_cut(g_rs485_data.fx_mode_para.intensity, g_power_factor.illumination);
							g_effect_struct.effect_arg.lightning_arg.arg.cct_arg.cct = g_rs485_data.fx_mode_para.cct;
							g_effect_struct.effect_arg.lightning_arg.arg.cct_arg.gm = g_rs485_data.fx_mode_para.gm;
							g_effect_struct.effect_arg.lightning_arg.trigger = (Effect_TriggerType)(g_rs485_data.fx_mode_para.trigger);
							g_effect_struct.effect_arg.lightning_arg.frq = g_rs485_data.fx_mode_para.frq;
							g_effect_struct.effect_arg.lightning_arg.speed = g_rs485_data.fx_mode_para.spd;
							Effect_Data_Init(&g_effect_struct, g_rs485_data.fx_restart);
						}
						break;
						case RS485_FX_TV:
						{
							uint16_t cct_range[3][2] = {{TV_CCT_RANGE1_LOW, TV_CCT_RANGE1_UP}, {TV_CCT_RANGE2_LOW, TV_CCT_RANGE2_UP},{TV_CCT_RANGE3_LOW, TV_CCT_RANGE3_UP}};
							
							g_effect_struct.effect_type = EffectTypeTV;
							g_effect_struct.effect_arg.tv_arg.lightness = sys_fx_power_cut(g_rs485_data.fx_mode_para.intensity, g_power_factor.illumination);
							g_effect_struct.effect_arg.tv_arg.frq = g_rs485_data.fx_mode_para.frq;
							g_effect_struct.effect_arg.tv_arg.cct = g_rs485_data.fx_mode_para.cct < 2 ? g_rs485_data.fx_mode_para.cct : 2;
							
							g_effect_struct.effect_arg.tv_arg.range1_mode = EffectModeCCT;
							g_effect_struct.effect_arg.tv_arg.range1_arg.cct_arg.cct = cct_range[g_effect_struct.effect_arg.tv_arg.cct][0];
							g_effect_struct.effect_arg.tv_arg.range1_arg.cct_arg.gm = 10;
							g_effect_struct.effect_arg.tv_arg.range1_arg.cct_arg.lightness = g_effect_struct.effect_arg.tv_arg.lightness;
							
							g_effect_struct.effect_arg.tv_arg.range2_mode = EffectModeCCT;
							g_effect_struct.effect_arg.tv_arg.range2_arg.cct_arg.cct = cct_range[g_effect_struct.effect_arg.tv_arg.cct][1];
							g_effect_struct.effect_arg.tv_arg.range2_arg.cct_arg.gm = 10;
							g_effect_struct.effect_arg.tv_arg.range2_arg.cct_arg.lightness = g_effect_struct.effect_arg.tv_arg.lightness;
							
							Effect_Data_Init(&g_effect_struct, g_rs485_data.fx_restart);
						}
						break;
						case RS485_FX_Fire:
						{
							uint16_t cct_range[3][2] = {{FIRE_CCT_RANGE1_LOW, FIRE_CCT_RANGE1_UP}, {FIRE_CCT_RANGE2_LOW, FIRE_CCT_RANGE2_UP}, {FIRE_CCT_RANGE3_LOW, FIRE_CCT_RANGE3_UP}};
							g_effect_struct.effect_type = EffectTypeFire;
							g_effect_struct.effect_arg.fire_arg.lightness = sys_fx_power_cut(g_rs485_data.fx_mode_para.intensity, g_power_factor.illumination);
							g_effect_struct.effect_arg.fire_arg.frq = g_rs485_data.fx_mode_para.frq;
							g_effect_struct.effect_arg.fire_arg.cct = g_rs485_data.fx_mode_para.cct < 2 ? g_rs485_data.fx_mode_para.cct : 2;
							g_effect_struct.effect_arg.fire_arg.range1_mode = EffectModeCCT;
							g_effect_struct.effect_arg.fire_arg.range1_arg.cct_arg.cct = cct_range[g_effect_struct.effect_arg.fire_arg.cct][0];
							g_effect_struct.effect_arg.fire_arg.range1_arg.cct_arg.gm = 10;
							g_effect_struct.effect_arg.fire_arg.range1_arg.cct_arg.lightness = g_effect_struct.effect_arg.fire_arg.lightness;
							g_effect_struct.effect_arg.fire_arg.range2_mode = EffectModeCCT;
							g_effect_struct.effect_arg.fire_arg.range2_arg.cct_arg.cct = cct_range[g_effect_struct.effect_arg.fire_arg.cct][1];
							g_effect_struct.effect_arg.fire_arg.range2_arg.cct_arg.gm = 10;
							g_effect_struct.effect_arg.fire_arg.range2_arg.cct_arg.lightness = g_effect_struct.effect_arg.fire_arg.lightness;
							Effect_Data_Init(&g_effect_struct, g_rs485_data.fx_restart);
						}
						break;                        
						case RS485_FX_Strobe:
						{
							g_effect_struct.effect_type = EffectTypeStrobe;
							g_effect_struct.effect_arg.strobe_arg.frq = g_rs485_data.fx_mode_para.frq;
							if(0 == set_sys_fx1_light_para(&g_effect_struct.effect_arg.strobe_arg.arg, 
														   &g_effect_struct.effect_arg.strobe_arg.mode, 
														   &g_rs485_data.fx_mode_para))
							{
								Effect_Data_Init(&g_effect_struct, g_rs485_data.fx_restart);
							}
						}
						break;
						case RS485_FX_Explosion:
						{
							g_effect_struct.effect_type = EffectTypeExplosion;
							g_effect_struct.effect_arg.explosion_arg.frq = g_rs485_data.fx_mode_para.frq;
							g_effect_struct.effect_arg.explosion_arg.trigger = (Effect_TriggerType)(g_rs485_data.fx_mode_para.trigger);
							if(0 == set_sys_fx1_light_para(&g_effect_struct.effect_arg.explosion_arg.arg, 
														   &g_effect_struct.effect_arg.explosion_arg.mode, 
														   &g_rs485_data.fx_mode_para))
							{
								Effect_Data_Init(&g_effect_struct, g_rs485_data.fx_restart);
							}
						}
						break;
						case RS485_FX_FaultBulb:
						{
							g_effect_struct.effect_type = EffectTypeFaultBulb;
							g_effect_struct.effect_arg.faultBulb_arg.frq = g_rs485_data.fx_mode_para.frq;
							g_effect_struct.effect_arg.faultBulb_arg.speed = g_rs485_data.fx_mode_para.spd;
							if(0 == set_sys_fx1_light_para(&g_effect_struct.effect_arg.faultBulb_arg.arg, 
														   &g_effect_struct.effect_arg.faultBulb_arg.mode, 
														   &g_rs485_data.fx_mode_para)){
								Effect_Data_Init(&g_effect_struct, g_rs485_data.fx_restart);
							}
						}
						break;
						case RS485_FX_Pulsing:
						{
							g_effect_struct.effect_type = EffectTypePulsing;
							g_effect_struct.effect_arg.pulsing_arg.frq = g_rs485_data.fx_mode_para.frq;
							g_effect_struct.effect_arg.pulsing_arg.speed = g_rs485_data.fx_mode_para.spd;
							if(0 == set_sys_fx1_light_para(&g_effect_struct.effect_arg.pulsing_arg.arg, 
														   &g_effect_struct.effect_arg.pulsing_arg.mode, 
														   &g_rs485_data.fx_mode_para))
							{
								Effect_Data_Init(&g_effect_struct, g_rs485_data.fx_restart);
							}
						}
						break;                             
						case RS485_FX_Fireworks:
						{
							g_effect_struct.effect_type = EffectTypeFireworks;
							g_effect_struct.effect_arg.fireworks_arg.lightness = sys_fx_power_cut(g_rs485_data.fx_mode_para.intensity, g_power_factor.illumination);
							g_effect_struct.effect_arg.fireworks_arg.frq = g_rs485_data.fx_mode_para.frq;
							g_effect_struct.effect_arg.fireworks_arg.type = g_rs485_data.fx_mode_para.type;
							Effect_Data_Init(&g_effect_struct, g_rs485_data.fx_restart);
						}
						break;
						case RS485_FX_Effect_Off:
						{
							g_effect_struct.effect_type = EffectTypeEffectOff;
							Effect_Data_Init(&g_effect_struct, g_rs485_data.fx_restart);
						}
						break;
						case RS485_FX_Null:
						{
							g_effect_struct.effect_type = EffectTypeNULL;
							Effect_Data_Init(&g_effect_struct, g_rs485_data.fx_restart);
						}
						break;
						default:break;
					}
				}
				break;
#ifdef PR_307_L3
				case RS485_Cmd_Sys_FX_II:
				{
					Light_Effect_Enable(true);
					SidusProFX_Enable(false);
					switch(g_rs485_data.light_mode_state.command_2)
					{
						case RS485_FX_II_Paparazzi_2:
						{
							if(0 == g_rs485_data.fx_mode_para.state)
							{
								g_effect_struct.effect_type = EffectTypeEffectOff;
								Effect_Data_Init(&g_effect_struct, g_rs485_data.fx_restart);
							}
							else
							{
								g_effect_struct.effect_type = EffectTypePaparazziII;
								g_effect_struct.effect_arg.paparazzi2_arg.max_interval = g_rs485_data.fx_mode_para.gap_time * 100;
								g_effect_struct.effect_arg.paparazzi2_arg.min_interval = g_rs485_data.fx_mode_para.min_gap_time * 100;
								if(0 == set_sys_fx2_light_para(&g_effect_struct.effect_arg.paparazzi2_arg.arg, 
															   &g_effect_struct.effect_arg.paparazzi2_arg.mode,  
															   &g_rs485_data.fx_mode_para))
								{
									Effect_Data_Init(&g_effect_struct, g_rs485_data.fx_restart);
								}
							}
						}
						break;
						case RS485_FX_II_Lightning_2:
						{
							g_effect_struct.effect_type = EffectTypeLightningII;
							g_effect_struct.effect_arg.lightning2_arg.trigger = (Effect_TriggerType)g_rs485_data.fx_mode_para.trigger;
							g_effect_struct.effect_arg.lightning2_arg.frq = g_rs485_data.fx_mode_para.frq;
							g_effect_struct.effect_arg.lightning2_arg.speed = g_rs485_data.fx_mode_para.spd;
							if(0 == set_sys_fx2_light_para(&g_effect_struct.effect_arg.lightning2_arg.arg, 
														   &g_effect_struct.effect_arg.lightning2_arg.mode,  
														   &g_rs485_data.fx_mode_para))
							{
								Effect_Data_Init(&g_effect_struct, g_rs485_data.fx_restart);
							}
						}
						break;
						case RS485_FX_II_TV_2:
						{
							if(0 == g_rs485_data.fx_mode_para.state)
							{
								g_effect_struct.effect_type = EffectTypeEffectOff;
								Effect_Data_Init(&g_effect_struct, g_rs485_data.fx_restart);
							}
							else
							{
								g_effect_struct.effect_type = EffectTypeTVII;
								g_effect_struct.effect_arg.tv2_arg.frq = g_rs485_data.fx_mode_para.spd;
								if(0 == set_sys_fx2_range_light_para(&g_effect_struct.effect_arg.tv2_arg.range1_arg, &g_effect_struct.effect_arg.tv2_arg.range2_arg, 
												&g_effect_struct.effect_arg.tv2_arg.range1_mode, &g_effect_struct.effect_arg.tv2_arg.range2_mode, 
												&g_rs485_data.fx_mode_para))
								{
									Effect_Data_Init(&g_effect_struct, g_rs485_data.fx_restart);
								}
							}
						}
						break;
						case RS485_FX_II_Fire_2:
						{
							if(0 == g_rs485_data.fx_mode_para.state)
							{
								g_effect_struct.effect_type = EffectTypeEffectOff;
								Effect_Data_Init(&g_effect_struct, g_rs485_data.fx_restart);
							}
							else
							{
								g_effect_struct.effect_type = EffectTypeFireII;
								g_effect_struct.effect_arg.fire2_arg.frq = g_rs485_data.fx_mode_para.spd;
								if(0 == set_sys_fx2_range_light_para(&g_effect_struct.effect_arg.fire2_arg.range1_arg, &g_effect_struct.effect_arg.fire2_arg.range2_arg, 
												&g_effect_struct.effect_arg.fire2_arg.range1_mode, &g_effect_struct.effect_arg.fire2_arg.range2_mode, 
												&g_rs485_data.fx_mode_para))
								{
									Effect_Data_Init(&g_effect_struct, g_rs485_data.fx_restart);
								}
							}
						}
						break;
						case RS485_FX_II_Strobe_2:
						{
							if(0 == g_rs485_data.fx_mode_para.state)
							{
								g_effect_struct.effect_type = EffectTypeEffectOff;
								Effect_Data_Init(&g_effect_struct, g_rs485_data.fx_restart);
							}
							else
							{
								g_effect_struct.effect_type = EffectTypeStrobeII;
								g_effect_struct.effect_arg.strobe2_arg.frq = g_rs485_data.fx_mode_para.spd;
								if(0 == set_sys_fx2_light_para(&g_effect_struct.effect_arg.strobe2_arg.arg, 
															   &g_effect_struct.effect_arg.strobe2_arg.mode,  
															   &g_rs485_data.fx_mode_para))
								{
									Effect_Data_Init(&g_effect_struct, g_rs485_data.fx_restart);
								}
							} 
						}
						break;
						case RS485_FX_II_Explosion_2:
						{
							g_effect_struct.effect_type = EffectTypeExplosionII;
							g_effect_struct.effect_arg.explosion2_arg.trigger = (Effect_TriggerType)g_rs485_data.fx_mode_para.trigger;
							g_effect_struct.effect_arg.explosion2_arg.frq = g_rs485_data.fx_mode_para.decay;
							if(0 == set_sys_fx2_light_para(&g_effect_struct.effect_arg.explosion2_arg.arg, 
														   &g_effect_struct.effect_arg.explosion2_arg.mode,  
														   &g_rs485_data.fx_mode_para))
							{
								Effect_Data_Init(&g_effect_struct, g_rs485_data.fx_restart);
							}
						}
						break;
						case RS485_FX_II_Fault_Bulb_2:
						{

							if(0 == g_rs485_data.fx_mode_para.state){
								g_effect_struct.effect_type = EffectTypeEffectOff;
								Effect_Data_Init(&g_effect_struct, g_rs485_data.fx_restart);
							}
							else{
								g_effect_struct.effect_type = EffectTypeFaultBulbII;
								g_effect_struct.effect_arg.faultBulb2_arg.frq = g_rs485_data.fx_mode_para.frq;
								g_effect_struct.effect_arg.faultBulb2_arg.speed = g_rs485_data.fx_mode_para.spd;
								if(0 == set_sys_fx2_light_para(&g_effect_struct.effect_arg.faultBulb2_arg.arg, 
															   &g_effect_struct.effect_arg.faultBulb2_arg.mode,  
															   &g_rs485_data.fx_mode_para))
								{
									Effect_Data_Init(&g_effect_struct, g_rs485_data.fx_restart);
								}
							}
						}
						break;
						case RS485_FX_II_Pulsing_2:
						{
							if(0 == g_rs485_data.fx_mode_para.state)
							{
								g_effect_struct.effect_type = EffectTypeEffectOff;
								Effect_Data_Init(&g_effect_struct, g_rs485_data.fx_restart);
							}
							else
							{
								g_effect_struct.effect_type = EffectTypePulsingII;
								g_effect_struct.effect_arg.pulsing2_arg.frq = g_rs485_data.fx_mode_para.frq;
								g_effect_struct.effect_arg.pulsing2_arg.speed = g_rs485_data.fx_mode_para.spd;
								if(0 == set_sys_fx2_light_para(&g_effect_struct.effect_arg.pulsing2_arg.arg, 
															   &g_effect_struct.effect_arg.pulsing2_arg.mode,  
															   &g_rs485_data.fx_mode_para))
								{
									Effect_Data_Init(&g_effect_struct, g_rs485_data.fx_restart);
								}
							}
						}
						break;
						case RS485_FX_II_Welding_2:
						{
							if(0 == g_rs485_data.fx_mode_para.state)
							{
								g_effect_struct.effect_type = EffectTypeEffectOff;
								Effect_Data_Init(&g_effect_struct, g_rs485_data.fx_restart);
							}
							else
							{
								g_effect_struct.effect_type = EffectTypeWeldingII;
								g_effect_struct.effect_arg.welding2_arg.min = sys_fx_power_cut(g_rs485_data.fx_mode_para.min_int, g_power_factor.illumination);
								g_effect_struct.effect_arg.welding2_arg.frq = g_rs485_data.fx_mode_para.frq;
								if(0 == set_sys_fx2_light_para(&g_effect_struct.effect_arg.welding2_arg.arg, 
															   &g_effect_struct.effect_arg.welding2_arg.mode,  
															   &g_rs485_data.fx_mode_para))
								{
									Effect_Data_Init(&g_effect_struct, g_rs485_data.fx_restart);
								}
							}   
						}
						break;
						case RS485_FX_II_Cop_Car_2:
						{
							if(0 == g_rs485_data.fx_mode_para.state)
							{
								g_effect_struct.effect_type = EffectTypeEffectOff;
								Effect_Data_Init(&g_effect_struct, g_rs485_data.fx_restart);
							}
							else
							{
								g_effect_struct.effect_type = EffectTypeCopCarII;
								g_effect_struct.effect_arg.cop_car2_arg.lightness = sys_fx_power_cut(g_rs485_data.fx_mode_para.intensity, g_power_factor.illumination);
								g_effect_struct.effect_arg.cop_car2_arg.frq = g_rs485_data.fx_mode_para.frq;
								g_effect_struct.effect_arg.cop_car2_arg.color = g_rs485_data.fx_mode_para.color;
								Effect_Data_Init(&g_effect_struct, g_rs485_data.fx_restart);
							}
						}
						break;
						case RS485_FX_II_Party_Lights_2:
						{
							if(0 == g_rs485_data.fx_mode_para.state)
							{
								g_effect_struct.effect_type = EffectTypeEffectOff;
								Effect_Data_Init(&g_effect_struct, g_rs485_data.fx_restart);
							}
							else
							{
								g_effect_struct.effect_type = EffectTypePartyLightsII;
								g_effect_struct.effect_arg.party_lights2_arg.lightness = sys_fx_power_cut(g_rs485_data.fx_mode_para.intensity, g_power_factor.illumination);
								g_effect_struct.effect_arg.party_lights2_arg.sat = g_rs485_data.fx_mode_para.sat;
								g_effect_struct.effect_arg.party_lights2_arg.frq = g_rs485_data.fx_mode_para.spd;
								Effect_Data_Init(&g_effect_struct, g_rs485_data.fx_restart);
							}
						}
						break;
						case RS485_FX_II_Fireworks_2:
						{
							if(0 == g_rs485_data.fx_mode_para.state)
							{
								g_effect_struct.effect_type = EffectTypeEffectOff;
								Effect_Data_Init(&g_effect_struct, g_rs485_data.fx_restart);
							}
							else
							{
								g_effect_struct.effect_type = EffectTypeFireworksII;
								g_effect_struct.effect_arg.fireworks2_arg.lightness = sys_fx_power_cut(g_rs485_data.fx_mode_para.intensity, g_power_factor.illumination);
								g_effect_struct.effect_arg.fireworks2_arg.max_interval = g_rs485_data.fx_mode_para.gap_time;
								g_effect_struct.effect_arg.fireworks2_arg.min_interval = g_rs485_data.fx_mode_para.min_gap_time;
								g_effect_struct.effect_arg.fireworks2_arg.type = g_rs485_data.fx_mode_para.mode;
								Effect_Data_Init(&g_effect_struct, g_rs485_data.fx_restart);
							}
						}
						break;
						case RS485_FX_II_Lightning_3:
						{
							g_effect_struct.effect_type = EffectTypeLightningIII;
							g_effect_struct.effect_arg.lightning3_arg.trigger = (Effect_TriggerType)g_rs485_data.fx_mode_para.trigger;
							g_effect_struct.effect_arg.lightning3_arg.max_interval = g_rs485_data.fx_mode_para.gap_time;
								g_effect_struct.effect_arg.lightning3_arg.min_interval = g_rs485_data.fx_mode_para.min_gap_time;
							if(0 == set_sys_fx2_light_para(&g_effect_struct.effect_arg.lightning3_arg.arg, 
														   &g_effect_struct.effect_arg.lightning3_arg.mode,  
														   &g_rs485_data.fx_mode_para))
							{
								Effect_Data_Init(&g_effect_struct, g_rs485_data.fx_restart);
							}
						}
						break;
						case RS485_FX_II_TV_3:
						{
							if(0 == g_rs485_data.fx_mode_para.state)
							{
								g_effect_struct.effect_type = EffectTypeEffectOff;
								Effect_Data_Init(&g_effect_struct, g_rs485_data.fx_restart);
							}
							else
							{
								g_effect_struct.effect_type = EffectTypeTVIII;
								g_effect_struct.effect_arg.tv3_arg.frq = g_rs485_data.fx_mode_para.spd;
								if(0 == set_sys_fx2_range_light_para(&g_effect_struct.effect_arg.tv3_arg.range1_arg, &g_effect_struct.effect_arg.tv3_arg.range2_arg, 
												&g_effect_struct.effect_arg.tv3_arg.range1_mode, &g_effect_struct.effect_arg.tv3_arg.range2_mode, 
												&g_rs485_data.fx_mode_para))
								{
									Effect_Data_Init(&g_effect_struct, g_rs485_data.fx_restart);
								}
							}
						}
						break;
						case RS485_FX_II_Fire_3:
						{

							if(0 == g_rs485_data.fx_mode_para.state)
							{
								g_effect_struct.effect_type = EffectTypeEffectOff;
								Effect_Data_Init(&g_effect_struct, g_rs485_data.fx_restart);
							}
							else
							{
								g_effect_struct.effect_type = EffectTypeFireIII;
								g_effect_struct.effect_arg.fire3_arg.frq = g_rs485_data.fx_mode_para.spd;
								if(0 == set_sys_fx2_range_light_para(&g_effect_struct.effect_arg.fire3_arg.range1_arg, &g_effect_struct.effect_arg.fire3_arg.range2_arg, 
												&g_effect_struct.effect_arg.fire3_arg.range1_mode, &g_effect_struct.effect_arg.fire3_arg.range2_mode, 
												&g_rs485_data.fx_mode_para))
								{
									Effect_Data_Init(&g_effect_struct, g_rs485_data.fx_restart);
								}
							}
						}
						break;
						case RS485_FX_II_Faulty_Bulb_3:
						{
							if(0 == g_rs485_data.fx_mode_para.state)
							{
								g_effect_struct.effect_type = EffectTypeEffectOff;
								Effect_Data_Init(&g_effect_struct, g_rs485_data.fx_restart);
							}
							else
							{
								g_effect_struct.effect_type = EffectTypeFaultBulbIII;
								g_effect_struct.effect_arg.faultBulb3_arg.max_interval = g_rs485_data.fx_mode_para.gap_time;
								g_effect_struct.effect_arg.faultBulb3_arg.min_interval = g_rs485_data.fx_mode_para.min_gap_time;
								if(0 == set_sys_fx2_light_para(&g_effect_struct.effect_arg.faultBulb3_arg.arg, 
															   &g_effect_struct.effect_arg.faultBulb3_arg.mode,  
															   &g_rs485_data.fx_mode_para))
								{
									Effect_Data_Init(&g_effect_struct, g_rs485_data.fx_restart);
								}
							}
						}
						break;
						case RS485_FX_II_Pulsing_3:
						{
							if(0 == g_rs485_data.fx_mode_para.state)
							{
								g_effect_struct.effect_type = EffectTypeEffectOff;
								Effect_Data_Init(&g_effect_struct, g_rs485_data.fx_restart);
							}
							else
							{
								g_effect_struct.effect_type = EffectTypePulsingIII;
								g_effect_struct.effect_arg.pulsing3_arg.frq = g_rs485_data.fx_mode_para.frq;
								if(0 == set_sys_fx2_light_para(&g_effect_struct.effect_arg.pulsing3_arg.arg, 
															   &g_effect_struct.effect_arg.pulsing3_arg.mode,  
															   &g_rs485_data.fx_mode_para))
								{
									Effect_Data_Init(&g_effect_struct, g_rs485_data.fx_restart);
								}
							}
						}
						break;
						case RS485_FX_II_Cop_Car_3:
						{
							if(0 == g_rs485_data.fx_mode_para.state)
							{
								g_effect_struct.effect_type = EffectTypeEffectOff;
								Effect_Data_Init(&g_effect_struct, g_rs485_data.fx_restart);
							}
							else
							{
								g_effect_struct.effect_type = EffectTypeCopCarIII;
								g_effect_struct.effect_arg.cop_car3_arg.lightness = g_rs485_data.fx_mode_para.intensity;
								g_effect_struct.effect_arg.cop_car3_arg.frq = g_rs485_data.fx_mode_para.frq;
								g_effect_struct.effect_arg.cop_car3_arg.color = g_rs485_data.fx_mode_para.mode;
								Effect_Data_Init(&g_effect_struct, g_rs485_data.fx_restart);
							}
						}
						break;
						default:break;
					}
				}
				break; 
				case RS485_Cmd_Color_Mixing:
				{
					Light_Effect_Enable(false);
					SidusProFX_Enable(false);	
					color_mixing_calc(&g_color_mixing_struct, (enum mixing_curve)g_rs485_data.dimming_curve.curve, g_power_factor.sum_power_rate, &g_str_cob_slow.pwm_str, &g_str_cob_slow.lux_str);
					Cob_Slow_Bright_Recognize();
				}
				break;	
				case RS485_Cmd_RGBWW:
				{
					static float light_duty[LED_LIGHT_NUM] = {0};
					Light_Effect_Enable(false);
					SidusProFX_Enable(false);
					light_duty[0] = (float)g_rs485_data.light_mode_para.red_ratio / 1000.0f;
					light_duty[1] = (float)g_rs485_data.light_mode_para.green_ratio / 1000.0f;
					light_duty[2] = (float)g_rs485_data.light_mode_para.blue_ratio / 1000.0f;
					light_duty[3] = (float)g_rs485_data.light_mode_para.ww_ratio / 1000.0f;
					light_duty[4] = (float)g_rs485_data.light_mode_para.cw_ratio / 1000.0f;
					color_rgbww_calc( light_duty[0], light_duty[1], light_duty[2], light_duty[3], light_duty[4], (enum mixing_curve)g_rs485_data.dimming_curve.curve,  g_power_factor.sum_power_rate, &g_str_cob_slow.pwm_str, &g_str_cob_slow.lux_str);
					Cob_Slow_Bright_Recognize();
				}
				break;	
#endif			
				case RS485_Cmd_PFX_Ctrl:
				{
					Light_Effect_Enable(false);
					SidusProFX_Arg_Init(&g_sidus_fx_str);
					SidusProFX_Enable(true);
				}
				break;
				case RS485_Cmd_CFX_Ctrl:
				{
					Light_Effect_Enable(false);	
					SidusProFX_Arg_Init(&g_sidus_fx_str);
					SidusProFX_Enable(true);
					#if(1 == GLOBAL_PRINT_ENABLE)
	//                printf("cfx state = %d\n", g_sidus_fx_str.SFX_Arg.CFX.Ctrl);
					#endif
				}
				break;
				case RS485_Cmd_CFX_Preview:
				{
					Light_Effect_Enable(false);	
					SidusProFX_Arg_Init(&g_sidus_fx_str);
					SidusProFX_Enable(true);
				}
				break;
				case RS485_Cmd_MFX_Ctrl:
				{
					Light_Effect_Enable(false);			
					SidusProFX_Arg_Init(&g_sidus_fx_str);
					SidusProFX_Enable(true);
				}
				break;
				case RS485_Cmd_PixelEffect:
				{
					switch(g_rs485_data.light_mode_state.command_2)
					{
						case RS485_PixelFX_Color_Cut:
						{
							
						}
						break;
						case RS485_PixelFX_Color_Rotate:
						{
							
						}
						break;
						case RS485_PixelFX_One_Color_Move:
						{
							
						}
						break;
						case RS485_PixelFX_Two_Color_Move:
						{
							
						}
						break;
						case RS485_PixelFX_Three_Color_Move:
						{
							
						}
						break;
						case RS485_PixelFX_Pixel_Fire:
						{
							
						}
						break;
						case RS485_PixelFX_Many_Color_Move:
						{
							
						}
						break;
						case RS485_PixelFX_On_Off_Effect:
						{
							
						}
						break;
						case RS485_PixelFX_Belt_Effect:
						{
							switch(g_rs485_data.light_mode_state.command_2)
							{
								case RS485_BeltFX_Fade:
								{
									
								}
								break;
								case RS485_BeltFX_Holoday:
								{
									
								}
								break;
								case RS485_BeltFX_One_Color_Marquee:
								{
									
								}
								break;
								case RS485_BeltFX_Full_Color_Marquee:
								{
									
								}
								break;
								case RS485_BeltFX_Starry_Sky:
								{
									
								}
								break;
								case RS485_BeltFX_Aurora:
								{
									
								}
								break;
								case RS485_BeltFX_Racing:
								{
									
								}
								break;
								case RS485_BeltFX_Train:
								{
									
								}
								break;
								case RS485_BeltFX_Snake:
								{
									
								}
								break;
								case RS485_BeltFX_Forest:
								{
									
								}
								break;
								case RS485_BeltFX_Bonfire:
								{
									
								}
								break;
								case RS485_BeltFX_Brook:
								{
									
								}
								break;
								case RS485_BeltFX_Game:
								{
									
								}
								break;
								case RS485_BeltFX_Timewait:
								{
									
								}
								break;
								case RS485_BeltFX_Party:
								{
									
								}
								break;
								case RS485_BeltFX_Belt_Fireworks:
								{
									
								}
								break;
								case RS485_BeltFX_Marbles:
								{
									
								}
								break;
								case RS485_BeltFX_Pendulum:
								{
									
								}
								break;
								case RS485_BeltFX_Flash_Point:
								{
									
								}
								break;
								default:break;
							}
						}
						break;
						case RS485_PixelFX_Music:
						{
							
						}
						break;
						case RS485_PixelFX_Rainbow:
						{
							
						}
						break;
						default:break;
					}
				}
				break;
				case RS485_Cmd_Partition_Color:
				{
					
				}
				break;
				case RS485_Cmd_Partition_Effect:
				{
					
				}
				break;

				default:break;
			}
		}
	}
}
/*****************************************************************************************
* Function Name: Main_Logic_Ctr
* Description  : 主逻辑(10ms时间片)
* Arguments    : NONE
* Return Value : NONE
******************************************************************************************/
void Main_Logic_Ctr(void)
{
	static uint8_t pwm_wait_recover = 0;
    /*当前状态为关机状态，且非定位光效执行状态*/
    if(LAMP_OFF == s_device_run_state && 0 == s_here_fx_flag)
	{
        /*关闭常规光效*/
        Light_Effect_Enable(false);	
        /*关闭自定义光效*/
        SidusProFX_Enable(false);
        /*关闭COB驱动的相关功能，将输出PWM值设置为0*/
        Cob_Output_Disable();
        /*关闭风扇驱动的相关功能*/
		Set_Fan_Run_State(0);
        /*将光模式触发变量的值设置为0*/
        g_rs485_data.new_command.light_mode = 0;
		Clr_Fan_Error_Flag();
		g_error_state.mcu_temp_eh_003 = 0;
		g_error_state.ambient_temp_eh_016 = 0;
		g_error_state.fan_eh_010 = 0;
		g_error_state.cool_eh_012 = 0;	
    }
	if(LAMP_FAN_ERROR_OFF == s_device_run_state)
	{
        /*关闭常规光效*/
        Light_Effect_Enable(false);
        /*关闭自定义光效*/
        SidusProFX_Enable(false);
        /*关闭COB驱动的相关功能，将输出PWM值设置为0*/
//		close_cob();
//        Led_Open_Enable_Ctr(0);
        Cob_Output_Disable();
        /*关闭风扇驱动的相关功能*/
        Set_Fan_Run_State(0);
        /*将光模式触发变量的值设置为0*/
        g_rs485_data.new_command.light_mode = 0;
    }
	if(g_error_state.ambient_temp_eh_016 == 1 || g_error_state.lamp_with_protect_cover_eh_001 == 1
		|| g_error_state.mcu_temp_eh_003 == 1 || g_self_adapt_version_not_match == 1)//cob过温或者带保护罩或者mcu过温或者自适应版本不匹配
	{
		if(g_rs485_data.high_speed.mode == 0)//pwm模式
		{
			/*关闭常规光效*/
			Light_Effect_Enable(false);
			/*关闭自定义光效*/
			SidusProFX_Enable(false);
			/*关闭COB驱动的相关功能，将输出PWM值设置为0*/
	//		close_cob();
	//        Led_Open_Enable_Ctr(0);
			Cob_Output_Disable();
			/*关闭风扇驱动的相关功能*/
	//        Set_Fan_Run_State(0);
			/*将光模式触发变量的值设置为0*/
			g_rs485_data.new_command.light_mode = 0;
			pwm_wait_recover = 1;
		}
		else
		{
			pwm_wait_recover = 0;
		}
    }
	else
	{
		if(pwm_wait_recover == 1)
		{
			pwm_wait_recover = 0;
			Set_Fixed_Cob_Pwm(0);//缓亮
			g_rs485_data.new_command.light_mode = 1;
		}
	}
	if(SUCCESS == g_str_ota.succ_flag)
	{
		static uint16_t s_cnt = 0;
		s_cnt++;
		if(20 == s_cnt)
		{
			if(ERROR == Write_Upgrade_Flag())
			{
				s_cnt = 0;
				return;
			}
		}
		else if(s_cnt >= 100)
		{
			g_str_ota.succ_flag = ERROR;
			NVIC_SystemReset();
		}
	}
}
/*****************************************************************************************
* Function Name : Set_Cfx_Name
* Description   : 设置自定义光效的名称
* Arguments     : p_rs485_decode：接收的485数据指针
* Return Value	: void
******************************************************************************************/
bool Set_Cfx_Name(rs485_cmd_arg_t* p_rs485_decode)
{
    return(SidusProFile_Set_Name(p_rs485_decode->cfx_name.effect_type, p_rs485_decode->cfx_name.bank, (char*)p_rs485_decode->cfx_name.name));
}
/*****************************************************************************************
* Function Name : High_Temp_Ctr_Logic
* Description   : 高温处理(10ms时间片)
* Arguments     : none
* Return Value	: void
******************************************************************************************/
void High_Temp_Ctr_Logic(void)
{
	if(g_adc_para.cob_temper > 0)
	{
		switch(s_cob_temperature_flag)
		{
			case 0:
			{
				if(g_adc_para.cob_temper > DEC_POWER_TEMP)
				{
					g_error_state.led_temp_eh_002 = 1;
					s_cob_temperature_flag = 1;
					g_power_factor.high_temp_rate = 0.9f;
					update_power_rate();
					g_rs485_data.new_command.light_mode = 1;
					g_rs485_data.fx_restart = 0;/*光效正常运行*/
				}
				else if((g_error_state.led_temp_eh_002 == 1) && (g_adc_para.cob_temper < RESTORE_POWER_TEMP))
				{
					s_cob_temperature_flag = 0;
					g_power_factor.high_temp_rate = 1.0f;
					update_power_rate();
					g_rs485_data.new_command.light_mode = 1;
					g_rs485_data.fx_restart = 1;/*光效重新运行*/
					g_error_state.led_temp_eh_002 = 0;
				}
			}
			break;
			case 1:
			{
				if(g_adc_para.cob_temper < RESTORE_POWER_TEMP)/*从降低功率的温度恢复至正常温度*/
				{
					s_cob_temperature_flag = 0;
					g_power_factor.high_temp_rate = 1.0f;
					update_power_rate();
					g_rs485_data.new_command.light_mode = 1;
					g_rs485_data.fx_restart = 1;/*光效重新运行*/
					g_error_state.led_temp_eh_002 = 0;
				}
				if(g_adc_para.cob_temper > CLOSE_MACHINE_TEMP)
				{
					g_error_state.ambient_temp_eh_016 = 1;
	//				Set_Device_Run_State(LAMP_TEMP_ERROR_OFF);
					s_cob_temperature_flag = 2;
				}
			}
			case 2:
			{
				if(g_adc_para.cob_temper < CLOSE_MACHINE_RESTORE_TEMP)/*从降低功率的温度恢复至降功率温度*/
				{
					s_cob_temperature_flag = 0;
					g_power_factor.high_temp_rate = 1.0f;
					update_power_rate();
					g_rs485_data.fx_restart = 1;/*光效重新运行*/
					g_error_state.led_temp_eh_002 = 0;
					g_error_state.ambient_temp_eh_016 = 0;
				}
			}
			break;
			default:break;
		}
	}
	else
	{
		if(g_adc_para.cob_temper <= -26 && g_error_state.ambient_temp_eh_016 == 0)
		{
			g_error_state.ambient_temp_eh_016 = 1;
		}
		else if(g_adc_para.cob_temper > -26 && g_error_state.ambient_temp_eh_016 == 1)
		{
			g_error_state.ambient_temp_eh_016 = 0;
		}
	}
	/*mcu over temperature process*/
	if(g_adc_para.mcu_temper > MCU_CLOSE_LED_TEMP)//MCU过温
	{
		if(g_error_state.mcu_temp_eh_003 == 0)
		{
			g_error_state.mcu_temp_eh_003 = 1;
		}
	}
	else if(g_adc_para.mcu_temper < MCU_RESTORE_TEMP)//MCU温度恢复
	{
		if(g_error_state.mcu_temp_eh_003 == 1)
		{
			g_error_state.mcu_temp_eh_003 = 0;
		}
	}
}

/*****************************************************************************************
* Function Name : Cob_Slow_Bright_Recognize
* Description   : 
* Arguments     : void
* Return Value	: void
******************************************************************************************/
static void Cob_Slow_Bright_Recognize(void)
{
	uint8_t i = 0;
	#ifdef PR_308_L2
	uint16_t led_duty[5] = {0};
	led_duty[0] = g_str_cob_slow.pwm_str.pwm[1];
	led_duty[1] = g_str_cob_slow.pwm_str.pwm[0];
	led_duty[4] = g_str_cob_slow.pwm_str.pwm[2];
	set_self_adapt_pwm(led_duty);
	#endif	
	#ifdef PR_307_L3
	set_self_adapt_pwm((uint16_t*)g_str_cob_slow.pwm_str.pwm);
	#endif
    for(i = 0; i < LED_CHANNEL_NUM; i++)
    {
        if(0 != g_str_cob_slow.pwm_str.pwm[i])
            break;
    }
    if(LED_CHANNEL_NUM == i)
    {
        if(0 == g_rs485_data.light_mode_para.fade)
		{
            g_slow_light_str.stage = 0;
#ifdef PR_308_L2
			uint16_t led_duty[5] = {0};
			led_duty[0] = g_str_cob_slow.pwm_str.pwm[1];
			led_duty[1] = g_str_cob_slow.pwm_str.pwm[0];
			led_duty[4] = g_str_cob_slow.pwm_str.pwm[2];
			Set_Drive_Cob_Pwm_Val(led_duty);
#endif	
#ifdef PR_307_L3
			Set_Drive_Cob_Pwm_Val((uint16_t*)g_str_cob_slow.pwm_str.pwm);
#endif	
        }
        else
		{
            g_slow_light_str.stage = 1;
            g_str_cob_slow.cnt = 0;
        }
    }
    else
    {
        if(0 == g_rs485_data.light_mode_para.fade)
		{
            g_slow_light_str.stage = 0;
#ifdef PR_308_L2
			uint16_t led_duty[5] = {0};
			led_duty[0] = g_str_cob_slow.pwm_str.pwm[1];
			led_duty[1] = g_str_cob_slow.pwm_str.pwm[0];
			led_duty[4] = g_str_cob_slow.pwm_str.pwm[2];
            Set_Drive_Cob_Pwm_Val(led_duty);

#endif	
#ifdef PR_307_L3
			Set_Drive_Cob_Pwm_Val((uint16_t*)g_str_cob_slow.pwm_str.pwm);
#endif	
        }
        else
		{
            g_slow_light_str.stage = 1;
            g_str_cob_slow.cnt = 0;
        }
    }
}
/*****************************************************************************************
* Function Name : update_power_rate
* Description   : 更新功率因数
* Arguments     : void
* Return Value	: void
******************************************************************************************/
void update_power_rate(void)
{
	if(g_rs485_data.fan.mode != 4)//if current fan is not silence mode,need to multiply by g_power_factor.illumination.
	{
			g_power_factor.sum_power_rate = g_power_factor.illumination * g_power_factor.high_temp_rate * g_power_factor.silence_mode_rate;
	}
	else
	{
			#ifdef PR_308_L2
			if(g_power_factor.illumination < 0.134f)
			#endif	
			#ifdef PR_307_L3
			if(g_power_factor.illumination < 0.256f)
			#endif	
			{
				g_power_factor.sum_power_rate = g_power_factor.illumination * g_power_factor.high_temp_rate;
			}
			else
			{
				g_power_factor.sum_power_rate = g_power_factor.high_temp_rate * g_power_factor.silence_mode_rate;
			}
	}
	if(g_rs485_data.illumination_mode.ratio == 0.0f)
	{
		Set_Fixed_Cob_Pwm(0);
	}
}
/*****************************************************************************************
* Function Name : sys_fx_power_cut
* Description   : 系统光效输出功率限制
* Arguments     : 亮度（0-1000），照度比例（0.0-1.0）
* Return Value	: 限制后的亮度值
******************************************************************************************/
static uint16_t sys_fx_power_cut(uint16_t lightness, float illumination_rate)
{
    if(0 == lightness)
        return 0;
    if(lightness < 100)
        return 100;
    /*风扇静音模式，此比例在0.1左右*/
    if(illumination_rate < 0.4f)
        return lightness;
    if(lightness > 700)
        return 700;
    return lightness;
}
/*****************************************************************************************
* Function Name : set_sys_fx21_light_para
* Description   : 设置1代系统光效的光参参数
* Arguments     : p_obj：获取数据端
                  run_mode：光效运行的模式
                  proto_mode：协议模式
                  p_source：提供数据端
* Return Value	: 0 - 正常， 1 - 异常
******************************************************************************************/
static uint8_t set_sys_fx1_light_para(Effect_Mode_Arg *p_obj, Effect_Mode *run_mode, 
                                      const fx_mode_para_t *p_source)
{
    uint8_t ret_val = 0;
    switch (p_source->mode)
    {
        case RS485_FX_Mode_CCT:
        {
            *run_mode = EffectModeCCT;
            p_obj->cct_arg.lightness = sys_fx_power_cut(p_source->intensity, g_power_factor.illumination);
            p_obj->cct_arg.cct = p_source->cct;
            p_obj->cct_arg.gm = p_source->gm;
        }
        break;
        case RS485_FX_Mode_HSI:
        {
            *run_mode = EffectModeHSI;
            p_obj->hsi_arg.lightness = sys_fx_power_cut(p_source->intensity, g_power_factor.illumination);
            p_obj->hsi_arg.hue = p_source->hue;
            p_obj->hsi_arg.sat = p_source->sat;
            p_obj->hsi_arg.cct = 6500;
        }
        break;
        case RS485_FX_Mode_GEL:
        {
            *run_mode = EffectModeGEL;
            p_obj->gel_arg.lightness = sys_fx_power_cut(p_source->intensity, g_power_factor.illumination);
            p_obj->gel_arg.cct = p_source->cct;
            p_obj->gel_arg.brand = p_source->origin;
            p_obj->gel_arg.type = p_source->series;
            p_obj->gel_arg.color = p_source->color;
        }
        break;
        case RS485_FX_Mode_XY:
        {
            *run_mode = EffectModeCoord;
            p_obj->coord_arg.lightness = sys_fx_power_cut(p_source->intensity, g_power_factor.illumination);
            p_obj->coord_arg.x = p_source->x;
            p_obj->coord_arg.y = p_source->y;
        }
        break;
        case RS485_FX_Mode_Source:
        {
            *run_mode = EffectModeSource;
            p_obj->source_arg.lightness = sys_fx_power_cut(p_source->intensity, g_power_factor.illumination);
            p_obj->source_arg.type = p_source->type;
        }
        break;
		case RS485_FX_Mode_RGB:
		{
			*run_mode = EffectModeRGB;
			p_obj->rgb_arg.lightness = sys_fx_power_cut(p_source->intensity, g_power_factor.illumination);
            p_obj->rgb_arg.r = p_source->red_ratio;
			p_obj->rgb_arg.g = p_source->green_ratio;
			p_obj->rgb_arg.b = p_source->blue_ratio;
		}
		break;
        default:
		{
            ret_val = 1;
        }
        break;
    }
    return ret_val;
}
/*****************************************************************************************
* Function Name : set_sys_fx2_light_para
* Description   : 设置2代系统光效的光参参数
* Arguments     : p_obj：获取数据端
                  run_mode：光效运行的模式
                  proto_mode：协议模式
                  p_source：提供数据端
* Return Value	: 0 - 正常， 1 - 异常
******************************************************************************************/
#ifdef PR_307_L3
static uint8_t set_sys_fx2_light_para(Effect_Mode_Arg *p_obj, Effect_Mode *run_mode, 
                                      const fx_mode_para_t *p_source)
{
    uint8_t ret_val = 0;
    switch (p_source->mode)
	{
        case RS485_FX_II_Mode_CCT:
		{
            *run_mode = EffectModeCCT;
            p_obj->cct_arg.lightness = sys_fx_power_cut(p_source->intensity, g_power_factor.illumination);
            p_obj->cct_arg.cct = p_source->cct;
            p_obj->cct_arg.gm = p_source->duv;
        }
        break;
        case RS485_FX_II_Mode_HSI:
		{
            *run_mode = EffectModeHSI;
            p_obj->hsi_arg.lightness = sys_fx_power_cut(p_source->intensity, g_power_factor.illumination);
            p_obj->hsi_arg.hue = p_source->hue;
            p_obj->hsi_arg.sat = p_source->sat;
            p_obj->hsi_arg.cct = p_source->cct;
        }
        break;
        case RS485_FX_II_Mode_GEL:
		{
            *run_mode = EffectModeGEL;
            p_obj->gel_arg.lightness = sys_fx_power_cut(p_source->intensity, g_power_factor.illumination);
            p_obj->gel_arg.cct = p_source->cct;
            p_obj->gel_arg.brand = p_source->origin;
            p_obj->gel_arg.type = p_source->series;
            p_obj->gel_arg.color = p_source->color;
        }
        break;
        case RS485_FX_II_Mode_XY:
		{
            *run_mode = EffectModeCoord;
            p_obj->coord_arg.lightness = sys_fx_power_cut(p_source->intensity, g_power_factor.illumination);
            p_obj->coord_arg.x = p_source->x;
            p_obj->coord_arg.y = p_source->y;
        }
        break;
        case RS485_FX_II_Mode_SOUYRCE:
		{
            *run_mode = EffectModeSource;
            p_obj->source_arg.lightness = sys_fx_power_cut(p_source->intensity, g_power_factor.illumination);
            p_obj->source_arg.type = p_source->type;
            p_obj->source_arg.x = p_source->x;
            p_obj->source_arg.y = p_source->y;
        }
        break;
        case RS485_FX_II_Mode_RGB:
		{
            *run_mode = EffectModeRGB;
            p_obj->rgb_arg.lightness = sys_fx_power_cut(p_source->intensity, g_power_factor.illumination);
            p_obj->rgb_arg.r = p_source->red_ratio;
            p_obj->rgb_arg.g = p_source->green_ratio;
            p_obj->rgb_arg.b = p_source->blue_ratio;
        }
        break;
        default:
		{
            ret_val = 1;
        }
        break;
    }
    return ret_val;
}
/*****************************************************************************************
* Function Name : set_sys_fx2_range_light_para
* Description   : 设置2代系统光效中光参中有参数是范围的光效运行参数；
* Arguments     : p_lower_obj：下限点获取数据端
                  p_upper_obj：上限点获取数据端
                  lower_run_mode：下限点光效运行的模式
                  upper_run_mode：上限点光效运行模式
                  p_source：提供数据端
* Return Value	: 0 - 正常， 1 - 异常
******************************************************************************************/
static uint8_t set_sys_fx2_range_light_para(Effect_Mode_Arg *p_lower_obj, Effect_Mode_Arg *p_upper_obj, 
                                            Effect_Mode *lower_run_mode, Effect_Mode *upper_run_mode, 
                                            const fx_mode_para_t *p_source)
{
    uint8_t ret_val = 0;
    switch(p_source->mode)
	{
        case RS485_FX_II_Mode_CCT_Range:
		{
            *lower_run_mode = EffectModeCCT;
            p_lower_obj->cct_arg.lightness = sys_fx_power_cut(p_source->intensity, g_power_factor.illumination);
            p_lower_obj->cct_arg.cct = p_source->cct_lower;
            p_lower_obj->cct_arg.gm = p_source->duv;
            
            *upper_run_mode = EffectModeCCT;
            p_upper_obj->cct_arg.lightness = sys_fx_power_cut(p_source->intensity, g_power_factor.illumination);
            p_upper_obj->cct_arg.cct = p_source->cct_upper;
            p_upper_obj->cct_arg.gm = p_source->duv;
        }
        break;
        case RS485_FX_II_Mode_HSI_Range:
		{
            *lower_run_mode = EffectModeHSI;
            p_lower_obj->hsi_arg.lightness = sys_fx_power_cut(p_source->intensity, g_power_factor.illumination);
            p_lower_obj->hsi_arg.hue = p_source->hue_lower;
            p_lower_obj->hsi_arg.sat = p_source->sat;
            p_lower_obj->hsi_arg.cct = p_source->cct;
            
            *upper_run_mode = EffectModeHSI;
            p_upper_obj->hsi_arg.lightness = sys_fx_power_cut(p_source->intensity, g_power_factor.illumination);
            p_upper_obj->hsi_arg.hue = p_source->hue_upper;
            p_upper_obj->hsi_arg.sat = p_source->sat;
            p_upper_obj->hsi_arg.cct = p_source->cct;
        }
        break;
        default:
		{
            
        }
        break;
    }
    return ret_val;
}
#endif
/***********************************END OF FILE*******************************************/

