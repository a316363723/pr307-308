/****************************************************************************************
**  Filename :  het_led.c
**  Abstract :  LED模块化程序，支持设置LED常亮，常灭，闪烁周期设置，闪烁次数设置等功能。
**  By       :  何建国
**  Date     :  2018-11-21
**  Changelog:1.First Create
*****************************************************************************************/

/*****************************************************************************************
									Includes
*****************************************************************************************/
#include "fan.h" 
#include "bsp_timer.h"
#include "systick.h"
#include "bsp_fan.h"
#include "bsp_power.h"
#include "SidusProFile.h"
/*****************************************************************************************
							  Variables definitions
*****************************************************************************************/
static Fan_Ctr_TypeDef s_fan_ctr[FAN_NUM] = {
	{
		.pid.kp = 0.10f,
		.pid.ki = 0.2f,
		.pid.kd = 0.05f,
		.pid.t_cycle = 0.05f,
		.pid.error = 0.0f,
		.pid.last_error = 0.0f,
		.pid.last_last_error = 0.0f,
		.pid.pwm = 0.0f,
		.pid.max_pwm = FAN_TMRA_PERIOD_VAL - 5,
		.pid.min_pwm = 0.0f,
		
		.spd.set_spd = 0,
		.spd.curr_spd = 0,
		.spd.max_spd = 3500,//Limiting for fan mode 0
		.spd.min_spd = 800,
		.spd.node_tim = 0,
		.spd.start_flag = 1,
		.spd.trig_times = 0,
		
		.temp.curr_temp = 25,
		.temp.up_limit_temp = 45,
		.temp.dowm_limit_temp = 35,
		.temp.delay_temp = 2,
		.temp.zero_degree_flag = 0,
		
		.error.cnt = 0,
		.error.flag = 0,
		
		//.p_tim_ch = (uint32_t *)&RW_MEM32((uint32_t)&M4_TMRA_12->CMPAR1 + (uint32_t)TMRA_CH_2 * 4U),
		.Set_PWM_CallBack = Fan_SetCmpVal,
	},
	{
		.pid.kp = 0.10f,
		.pid.ki = 0.2f,
		.pid.kd = 0.05f,
		.pid.t_cycle = 0.05f,
		.pid.error = 0.0f,
		.pid.last_error = 0.0f,
		.pid.last_last_error = 0.0f,
		.pid.pwm = 0.0f,
		.pid.max_pwm = WATER_TMRA_PERIOD_VAL - 5,
		.pid.min_pwm = 0.0f,
		
		.spd.set_spd = 0,
		.spd.curr_spd = 0,
		.spd.max_spd = 8000,
		.spd.min_spd = 1500,
		.spd.node_tim = 0,
		.spd.start_flag = 1,
		.spd.trig_times = 0,
		
		.temp.curr_temp = 25,
		.temp.up_limit_temp = 65,
		.temp.dowm_limit_temp = 40,
		.temp.delay_temp = 2,
		.temp.zero_degree_flag = 0,
		
		.error.cnt = 0,
		.error.flag = 0,
		
		.Set_PWM_CallBack = Water_SetCmpVal,
		//.Set_PWM_CallBack = NULL,//Water_SetCmpVal,
	},
};
static uint8_t s_fan_run_state = 0;/*风扇运行状态 0 - 关闭，1 - 运行*/
static uint8_t s_fan_auto_mode_flag = 0;/*风扇自动模式标志：0 - 非自动模式，1 - 自动模式*/
/*****************************************************************************************
							  Functions definitions
*****************************************************************************************/
static void Get_Fan1_Spd_CallBack(void);
static void Get_Fan_Spd_Cnt(Fan_Ctr_TypeDef* p_fan);
static void Fan_Stop_Judge_Ctr(Fan_Ctr_TypeDef* p_fan);
static void Get_Fan_Error_Msg(Fan_Ctr_TypeDef* p_fan, uint8_t fan_type);
static void Fan_Error_Ctr(Fan_Ctr_TypeDef* p_fan);
static uint16_t Get_Fan_Set_Speed(Fan_Ctr_TypeDef* p_fan);
static void PID_Ctr_Fan(Fan_Ctr_TypeDef* p_fan);
static void Get_Water1_Spd_CallBack(void);
/*****************************************************************************************
* Function Name: Fan_Power_GpioInit
* Description  : 风扇GPIO初始化
* Arguments    : NONE
* Return Value : NONE
******************************************************************************************/
void Fan_Water_Fun_Init(void)
{
//	Set_Fan_Run_State(1);
	Fan_Tim_Ch_Ic_CallBack = Get_Fan1_Spd_CallBack;
	Water_Tim_Ch_Ic_CallBack = Get_Water1_Spd_CallBack;
}

void Fan_Water_Trig_To_Zero(void)
{
	s_fan_ctr[0].spd.node_tim = 0;
	s_fan_ctr[1].spd.node_tim = 0;
}
/*****************************************************************************************
* Function Name: Fan_Ctr_Logic
* Description  : 风扇控制逻辑程序(100ms轮询一次)
* Arguments    : NONE
* Return Value : NONE
******************************************************************************************/
void Fan_Ctr_Logic(void)
{
    if(0 == s_fan_run_state)
	{
		return;
	}
	for(uint8_t i = 0; i < FAN_NUM; i++)
	{
		s_fan_ctr[i].temp.curr_temp = g_adc_para.cob_temper;
        /*自动模式时，需要根据温度计算设置的转速*/
        if((1 == s_fan_auto_mode_flag))
		{
			if(i == 0)
			{
				Get_Fan_Set_Speed(&s_fan_ctr[0]);
			}
			#ifdef PR_308_L2
			if(i == 1)
			{
				if(s_fan_ctr[i].temp.curr_temp > 0 && s_fan_ctr[i].temp.curr_temp <= 50)
				{
					Set_Fan_Run_Speed(4400, 1);
				}
				else if(s_fan_ctr[i].temp.curr_temp > 50)
				{
					Set_Fan_Run_Speed(PUMP_NORMAL_SPEED, 1);
				}
			}
			#endif
        }
		PID_Ctr_Fan(&s_fan_ctr[i]);
		Fan_Stop_Judge_Ctr(&s_fan_ctr[i]);
		Get_Fan_Error_Msg(&s_fan_ctr[i],i);
		Fan_Error_Ctr(&s_fan_ctr[i]);
		if(i == 1)
		{
			if(s_fan_ctr[i].temp.curr_temp > 0 && s_fan_ctr[i].temp.zero_degree_flag == 1)
			{
				s_fan_ctr[i].pid.error = 0.0f,
				s_fan_ctr[i].pid.last_error = 0.0f,
				s_fan_ctr[i].pid.last_last_error = 0.0f,
				s_fan_ctr[i].temp.zero_degree_flag = 0;
				g_rs485_data.new_command.fan = 1;
				Set_Water_Enable(1);
			}
			if(s_fan_ctr[i].temp.curr_temp <= 0 && s_fan_ctr[i].temp.zero_degree_flag == 0)
			{
				s_fan_ctr[i].temp.zero_degree_flag = 1;
				Set_Water_Enable(0);
				s_fan_ctr[i].spd.set_spd = 0;//防止堵转错判
			}
		}
	}
}
/*****************************************************************************************
* Function Name: Clr_Fan_Error_Flag
* Description  : 
* Arguments    : NONE
* Return Value : NONE
******************************************************************************************/
void Clr_Fan_Error_Flag(void)
{
	for(uint8_t i = 0; i < FAN_NUM; i++)
		s_fan_ctr[i].error.flag = 0;
}
/*****************************************************************************************
* Function Name: Set_Fan_Run_Mode
* Description  : 设置风扇运行模式
* Arguments    : mode：0 - 固定转速，1 - 自动
* Return Value : NONE
******************************************************************************************/
void Set_Fan_Run_Mode(uint8_t mode)
{
    s_fan_auto_mode_flag = mode;
}
/*****************************************************************************************
* Function Name: Set_Fan_Run_State
* Description  : 设置风扇运行状态
* Arguments    : flag：0 - 风扇功能关闭，1 - 风扇功能开启
* Return Value : NONE
******************************************************************************************/
void Set_Fan_Run_State(uint8_t flag)
{
    if(0 == flag){
			s_fan_run_state = 0;
			Set_Fan_Enable(0);
			Set_Water_Enable(0);
			s_fan_ctr[0].error.cnt = 0;
			s_fan_ctr[1].error.cnt = 0;
    }
    else{
			s_fan_run_state = 1;
			Set_Fan_Enable(1);
			Set_Water_Enable(1);
    }
}
/*****************************************************************************************
* Function Name: Set_Fan_Max_Speed
* Description  : 
* Arguments    : 风扇转速上限值
* Return Value : NONE
******************************************************************************************/
void Set_Fan_Max_Speed(uint16_t val)
{
	for(uint8_t i = 0; i < FAN_NUM; i++){
		if(val > s_fan_ctr[i].spd.max_spd || val < s_fan_ctr[i].spd.min_spd)
			continue;
		s_fan_ctr[i].spd.max_spd = val;
	}
}
/*****************************************************************************************
* Function Name: Set_Fan_Max_Speed
* Description  : 
* Arguments    : 风扇转速上限值
* Return Value : NONE
******************************************************************************************/
void Set_Fan_Run_Speed(uint16_t val, uint8_t sel)
{
	if(sel == 1)
	{
		s_fan_ctr[sel].temp.curr_temp = g_adc_para.cob_temper;
		if(s_fan_ctr[sel].temp.curr_temp <= 0)
		{
			s_fan_ctr[sel].temp.zero_degree_flag = 1;
			Set_Water_Enable(0);
			s_fan_ctr[sel].spd.set_spd = 0;//防止堵转错判
			return;
		}
//		if(s_fan_ctr[sel].temp.zero_degree_flag == 1 && s_fan_ctr[sel].temp.curr_temp >= 2)
//		{
//			s_fan_ctr[sel].pid.error = 0.0f,
//			s_fan_ctr[sel].pid.last_error = 0.0f,
//			s_fan_ctr[sel].pid.last_last_error = 0.0f,
//			s_fan_ctr[sel].temp.zero_degree_flag = 0;
//			g_rs485_data.new_command.fan = 1;
//			Set_Water_Enable(1);
//		}
	}
	if(s_fan_ctr[sel].spd.min_spd > val)
	{
		s_fan_ctr[sel].spd.set_spd = s_fan_ctr[sel].spd.min_spd;
	}else if(s_fan_ctr[sel].spd.max_spd < val)
	{
		s_fan_ctr[sel].spd.set_spd = s_fan_ctr[sel].spd.max_spd;
	}else
	{
		s_fan_ctr[sel].spd.set_spd = val;
	}
}
/*****************************************************************************************
* Function Name: Get_Fan_Curr_Spd
* Description  : 获取风扇当前转速
* Arguments    : sel:风扇序号，从0开始
* Return Value : NONE
******************************************************************************************/
uint16_t Get_Fan_Curr_Spd(uint8_t sel)
{
	return(s_fan_ctr[sel].spd.curr_spd);
}
/*****************************************************************************************
* Function Name: Get_Fan1_Spd_CallBack
* Description  : 风扇2转速回调函数
* Arguments    : NONE
* Return Value : NONE 
******************************************************************************************/
static void Get_Fan1_Spd_CallBack(void)
{
	Get_Fan_Spd_Cnt(&s_fan_ctr[0]);
}
/*****************************************************************************************
* Function Name: Get_Fan1_Spd_CallBack
* Description  : 风扇2转速回调函数
* Arguments    : NONE
* Return Value : NONE 
******************************************************************************************/
static void Get_Water1_Spd_CallBack(void)
{
	Get_Fan_Spd_Cnt(&s_fan_ctr[1]);
}

/*****************************************************************************************
* Function Name: Get_Fan_Spd_Cnt
* Description  : 风扇2转速回调函数
* Arguments    : NONE
* Return Value : NONE 
******************************************************************************************/
const uint8_t g_fan_phase_number = 2;//分钟转动一圈的脉冲数量
static void Get_Fan_Spd_Cnt(Fan_Ctr_TypeDef* p_fan)
{
	uint16_t tim_cnt = 0;
    p_fan->spd.trig_times++;
    
    if(1 == p_fan->spd.start_flag)
	{
		p_fan->spd.start_flag = 0;
		p_fan->spd.node_tim = HAL_GetTick();
		p_fan->spd.trig_times = 0;
		return;
	}
	if(get_sidus_file_state() != 0)//flash擦写不要读取速度，可能会出现HAL_GetTick不准，导致速度突然很大之后慢慢降速的情况
	{
		p_fan->spd.node_tim = HAL_GetTick();
		p_fan->spd.trig_times = 0;
		return;
	}
	tim_cnt = HAL_GetTick() - p_fan->spd.node_tim;//单位ms
    if(tim_cnt >= 100)
    {
        p_fan->spd.node_tim = HAL_GetTick();
        p_fan->spd.curr_spd = (((float)1000 / (float)tim_cnt) * ((float)p_fan->spd.trig_times / (float)g_fan_phase_number)) * 60;
        p_fan->spd.trig_times = 0;
    }
}
/*****************************************************************************************
* Function Name: Fan_Stop_Judge_Ctr
* Description  : （10ms调用一次）
* Arguments    : NONE
* Return Value : NONE 
******************************************************************************************/
static void Fan_Stop_Judge_Ctr(Fan_Ctr_TypeDef* p_fan)
{
	if(HAL_GetTick() - p_fan->spd.node_tim > 500)
	{
		p_fan->spd.node_tim = HAL_GetTick();
		p_fan->spd.curr_spd = 0;
		p_fan->spd.trig_times = 0;
		p_fan->spd.start_flag = 1;
	}
}

void fan_water_pid_pwm_zero(uint16_t fan_pwm)
{
	s_fan_ctr[0].pid.pwm = fan_pwm;
	s_fan_ctr[1].pid.pwm = fan_pwm;
}
/*****************************************************************************************
* Function Name: judge_fan_fault
* Description  : 风扇故障判断
* Arguments    : NONE
* Return Value : NONE
******************************************************************************************/
static void Get_Fan_Error_Msg(Fan_Ctr_TypeDef* p_fan, uint8_t fan_type)
{
    if((p_fan->spd.set_spd != 0 && (p_fan->spd.curr_spd > RPM_CNT_ERROR)) || 0 == p_fan->spd.set_spd)//转速超过20S仍达不到设置转速判定为堵转
    {
        p_fan->error.cnt = 0;
        return;
    }
    p_fan->error.cnt++;
    if(p_fan->error.cnt >= 200)
    {
        p_fan->error.cnt = 0;//风扇堵转故障
		if(fan_type == 0)//风扇堵转
		{
			g_error_state.fan_eh_010 = 1;
		}
		else if(fan_type == 1)//水泵堵转
		{
			g_error_state.cool_eh_012 = 1;
		}
        p_fan->error.flag = 1; 
    }
}
/*****************************************************************************************
* Function Name: Fan_Error_Ctr
* Description  : 风扇故障控制
* Arguments    : NONE
* Return Value : NONE
******************************************************************************************/
static void Fan_Error_Ctr(Fan_Ctr_TypeDef* p_fan)
{
	if(1 == p_fan->error.flag){
        Set_Device_Run_State(LAMP_FAN_ERROR_OFF);/*设置设备关机*/
        p_fan->error.flag = 0;
    }
}
/*****************************************************************************************
* Function Name: Get_Fan_Set_Speed
* Description  : 通过温度值获取风扇转速值
* Arguments    : 当前温度值
* Return Value : 当前设置的风扇转速值

#define FAN_SMART_SPEED_ONE     2400
#define FAN_SMART_TEMP_ONE      95
#define FAN_SMART_SPEED_TWO     2000
#define FAN_SMART_TEMP_TWO      86
#define FAN_SMART_SPEED_THR     1600
#define FAN_SMART_TEMP_THR      78
#define FAN_SMART_SPEED_FOU     900
#define FAN_SMART_TEMP_FOU      35
******************************************************************************************/
static uint16_t Get_Fan_Set_Speed(Fan_Ctr_TypeDef* p_fan)
{
	uint16_t highest_spd;
	uint16_t lowest_spd;
	
	if( p_fan->temp.curr_temp < FAN_SMART_TEMP_FOU)
	{
		p_fan->temp.dowm_limit_temp = FAN_SMART_TEMP_FOU;
		lowest_spd = FAN_SMART_SPEED_FOU;
		highest_spd = FAN_SMART_SPEED_FOU;
	}
	else if(p_fan->temp.curr_temp >= FAN_SMART_TEMP_FOU && p_fan->temp.curr_temp <= FAN_SMART_TEMP_THR)
	{
		p_fan->temp.dowm_limit_temp = FAN_SMART_TEMP_FOU;
		p_fan->temp.up_limit_temp = FAN_SMART_TEMP_THR;
		lowest_spd = FAN_SMART_SPEED_FOU;
		highest_spd = FAN_SMART_SPEED_THR;
	}
	else if(p_fan->temp.curr_temp > FAN_SMART_TEMP_THR && p_fan->temp.curr_temp <= FAN_SMART_TEMP_TWO)
	{
		p_fan->temp.dowm_limit_temp = FAN_SMART_TEMP_THR;
		p_fan->temp.up_limit_temp = FAN_SMART_TEMP_TWO;
		lowest_spd = FAN_SMART_SPEED_THR;
		highest_spd = FAN_SMART_SPEED_TWO;
	}
	else if(p_fan->temp.curr_temp > FAN_SMART_TEMP_TWO && p_fan->temp.curr_temp <= FAN_SMART_TEMP_ONE)
	{
		p_fan->temp.dowm_limit_temp = FAN_SMART_TEMP_TWO;
		p_fan->temp.up_limit_temp = FAN_SMART_TEMP_ONE;
		lowest_spd = FAN_SMART_SPEED_TWO;
		highest_spd = FAN_SMART_SPEED_ONE;
	}
	else
	{
		p_fan->temp.up_limit_temp = FAN_SMART_TEMP_ONE;
		lowest_spd = FAN_SMART_SPEED_ONE;
		highest_spd = FAN_SMART_SPEED_ONE;
	}
		
    if(p_fan->temp.curr_temp < p_fan->temp.dowm_limit_temp)
    {
//	if(p_fan->temp.dowm_limit_temp - p_fan->temp.curr_temp > p_fan->temp.delay_temp)
//	{
			p_fan->spd.set_spd = lowest_spd;
//        }
    }
    else if(p_fan->temp.curr_temp >= p_fan->temp.up_limit_temp)
    {
        p_fan->spd.set_spd = highest_spd;
    }
    else
    {
        float a = (float)(highest_spd - lowest_spd) / (float)(p_fan->temp.up_limit_temp - p_fan->temp.dowm_limit_temp);
        p_fan->spd.set_spd = lowest_spd + a * (p_fan->temp.curr_temp - p_fan->temp.dowm_limit_temp);
    } 
    return(p_fan->spd.set_spd);
}
/*****************************************************************************************
* Function Name: PID_Ctr_Fan
* Description  : 使用PID方式控制风扇转速（100ms时间片）
* Arguments    : NONE
* Return Value : NONE
******************************************************************************************/
static void PID_Ctr_Fan(Fan_Ctr_TypeDef* p_fan)
{
	float A = 0.0f;
	float B = 0.0f;
	float C = 0.0f;
	
	A = p_fan->pid.kp * (1 + p_fan->pid.t_cycle / p_fan->pid.ki + p_fan->pid.kd / p_fan->pid.t_cycle);
	B = -1 * p_fan->pid.kp * (1 + 2 * p_fan->pid.kd / p_fan->pid.t_cycle);
	C = p_fan->pid.kp * p_fan->pid.kd / p_fan->pid.t_cycle;
	
	p_fan->pid.error = (float)p_fan->spd.set_spd - (float)p_fan->spd.curr_spd;
	p_fan->pid.pwm += A * p_fan->pid.error + B * p_fan->pid.last_error + C * p_fan->pid.last_last_error;
	p_fan->pid.last_last_error = p_fan->pid.last_error;
	p_fan->pid.last_error = p_fan->pid.error;
	
	if(p_fan->pid.pwm > p_fan->pid.max_pwm)
		p_fan->pid.pwm = p_fan->pid.max_pwm;
	else if(p_fan->pid.pwm < p_fan->pid.min_pwm)
		p_fan->pid.pwm = p_fan->pid.min_pwm;
	//WR_REG(p_fan->p_tim_ch) = p_fan->pid.pwm;
	if(p_fan->Set_PWM_CallBack != NULL)
		p_fan->Set_PWM_CallBack(p_fan->pid.pwm);
}
/***********************************END OF FILE*******************************************/
