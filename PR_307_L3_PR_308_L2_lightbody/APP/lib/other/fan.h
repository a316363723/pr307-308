/****************************************************************************************
**  Filename :  het_led.h
**  Abstract :  LED模块化程序，支持设置LED常亮，常灭，闪烁周期设置，闪烁次数设置等功能。
**  By       :  何建国
**  Date     :  2018-11-21
**  Changelog:1.First Create
*****************************************************************************************/
#ifndef _FAN_H
#define _FAN_H
/*****************************************************************************************
									Includes
*****************************************************************************************/
#include "define.h"
#include "sys.h"
/*****************************************************************************************
								Macro definitions
*****************************************************************************************/
#define FAN_NUM								2
/*****************************************************************************************
								Typedef definitions
*****************************************************************************************/
typedef __packed struct
{
	float kp;
	float ki;
	float kd;
	float t_cycle;
	float error;
	float last_error;
	float last_last_error;
	float pwm;
	float max_pwm;
	float min_pwm;
}Fan_Pid_TypeDef;
typedef __packed struct
{
	uint16_t set_spd;
	uint16_t curr_spd;
	uint16_t max_spd;
	uint16_t min_spd;
	uint32_t node_tim;
	uint8_t  start_flag;
	uint16_t trig_times;
}Fan_Spd_TypeDef;
typedef __packed struct
{
	char curr_temp;
	char up_limit_temp;
	char dowm_limit_temp;
	uint8_t delay_temp;
	uint8_t zero_degree_flag;
}Fan_Temp_TypeDef;
typedef __packed struct
{
	uint16_t cnt;
	uint8_t flag;
}Fan_Error_TypeDef;
typedef __packed struct
{
	Fan_Pid_TypeDef pid;
	Fan_Spd_TypeDef spd;
	Fan_Temp_TypeDef temp;
	Fan_Error_TypeDef error;
	//uint32_t* p_tim_ch;
	void (*Set_PWM_CallBack)(uint32_t);
}Fan_Ctr_TypeDef;
/*****************************************************************************************
							Global variables and functions
*****************************************************************************************/

//----------------------------------------------------------------------------------------
extern void Fan_Power_GpioInit(void);
extern void Fan_Ctr_Logic(void);
extern void Clr_Fan_Error_Flag(void);
extern void Set_Fan_Run_Mode(uint8_t mode);
extern void Set_Fan_Run_State(uint8_t flag);
extern void Set_Fan_Max_Speed(uint16_t val);
extern void Set_Fan_Run_Speed(uint16_t val, uint8_t sel);
extern void Fan_Water_Fun_Init(void);
void Fan_Water_Trig_To_Zero(void);
void fan_water_pid_pwm_zero(uint16_t fan_pwm);
#endif
/***********************************END OF FILE*******************************************/
