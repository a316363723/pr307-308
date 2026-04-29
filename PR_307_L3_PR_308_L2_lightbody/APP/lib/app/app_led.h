/****************************************************************************************
**  Filename :  111.h
**  Abstract :  。
**  By       :  何建国
**  Date     :  
**  Changelog:1.First Create
*****************************************************************************************/
#ifndef _APP_LED_H
#define _APP_LED_H
/*****************************************************************************************
									Includes
*****************************************************************************************/
#include "define.h"
#include "light_effect.h"
#include "color_mixing_dev.h"
#include "color_mixing.h"
/*****************************************************************************************
								Macro definitions
*****************************************************************************************/

/*****************************************************************************************
								Typedef definitions
*****************************************************************************************/

//缓变
typedef struct
{
	uint16_t cnt;
	uint16_t pwm_str[7];
}Single_Crc_Cob_Slow_TypeDef;
//缓变
typedef struct
{
	uint16_t cnt;
	struct mixing_pwm pwm_str;
	struct mixing_lux lux_str;
}Cob_Slow_TypeDef;
typedef struct
{
	uint8_t stage;
	uint16_t set_pwm[5];
    uint16_t down_limit_pwm_info[5];
	float start_pwm[5];
	float add_factor[5];
	
	struct mixing_lux set_lux;
    uint32_t down_limit_lux_info[5];
	struct mixing_lux start_lux;
	float add_lux_factor[5];
}Slow_Light_Ctr_TypeDef;

typedef struct
{
	uint8_t stage;
	uint16_t set_pwm[5];
    uint16_t down_limit_pwm_info[5];
	float start_pwm[5];
	float add_factor[5];
}Slow_Light_Single_Crc_Ctr_TypeDef;

//定时器通道索引
typedef __packed struct
{
	volatile uint32_t* p_ch_info[LED_CHANNEL_NUM];
	uint16_t  max_pwm[LED_CHANNEL_NUM];
	uint32_t  p_ch[LED_CHANNEL_NUM];
	M4_TMRA_TypeDef* p_tim[LED_CHANNEL_NUM];
	uint16_t  p_ch_adc[LED_CHANNEL_NUM];
}Led_Pwm_Channel_TypeDef;
//自适应
typedef __packed struct
{
	uint32_t* p_ch_info[LED_CHANNEL_NUM];
	uint32_t* p_h_state_info[LED_CHANNEL_NUM];
	uint32_t* p_l_state_info[LED_CHANNEL_NUM];
	uint16_t  max_val[LED_CHANNEL_NUM];
	uint16_t  min_val[LED_CHANNEL_NUM];
}Led_Adaptive_Ctr_TypeDef;
//自适应
typedef __packed struct
{
	uint32_t* p_ch_info[LED_CHANNEL_NUM];
}Led_En_Channel_TypeDef;

//ADC自适应调节
typedef __packed struct
{
	uint32_t* p_ch_info[LED_CHANNEL_NUM];
	uint32_t* p_volt[LED_CHANNEL_NUM];
	uint8_t   adjust_dir[LED_CHANNEL_NUM];
	uint8_t   adjust_val[LED_CHANNEL_NUM];
	uint8_t   adjust_enalbe[LED_CHANNEL_NUM];
	uint8_t	  wave_times[LED_CHANNEL_NUM];
	uint16_t  max_val[LED_CHANNEL_NUM];
	uint16_t  min_val[LED_CHANNEL_NUM];
}ADC_Adaptive_Ctr_TypeDef;

typedef __packed struct
{
	uint8_t break_en;
	uint8_t enalbe;//使能标志
	uint32_t star_time;//超时时间
	uint8_t state[LED_CHANNEL_NUM];//1:检测到 30% pwm参考电压 ; 2:检测到 低于20% pwm的基准电压
	uint8_t cnt[LED_CHANNEL_NUM];
	float* p_ch_volt[LED_CHANNEL_NUM];//对应通道电压
	float ch_base_volt[LED_CHANNEL_NUM];//基准电压
	float ch_refer_volt[LED_CHANNEL_NUM];//参考电压
}ADC_Pwm_Base_Volt_TypeDef;

/*****************************************************************************************
							Global variables and functions
*****************************************************************************************/
extern Cob_Slow_TypeDef g_str_cob_slow;
extern Single_Crc_Cob_Slow_TypeDef g_str_single_crc_cob_slow;
extern struct mixing_pwm g_str_cob_analog;
extern struct mixing_lux g_str_cob_analog_lux;
extern uint16_t g_str_cob_analog_sta[5];
extern Slow_Light_Ctr_TypeDef g_slow_light_str;
extern Slow_Light_Single_Crc_Ctr_TypeDef g_slow_single_crc_light_str;
extern Led_Pwm_Channel_TypeDef g_led_pwm_channel;
//----------------------------------------------------------------------------------------
extern void led_down_limit_base_data_info(uint8_t mode);
extern uint16_t led_up_limit_base_data_info(uint8_t channel,uint16_t up_limit_value);
extern void Led_Open_Enable_Ctr(uint8_t flag);
extern void Led_Adc_Adaptive_Pare(void);
extern void Led_Adc_Adapt_Test(void);
extern void Cob_Output_Disable(void);
extern void Cob_Output_Enable(void);
extern void Set_Fixed_Cob_Pwm(uint16_t pwm);
extern void Set_Drive_Cob_Pwm_Val(uint16_t* p_pwm);
extern void Set_Drive_Cob_Every_Pwm_Val(uint16_t* p_pwm, uint8_t light_state);
extern ADC_Adaptive_Ctr_TypeDef g_adc_adaptive_para;
void set_drive_pwm_val(uint8_t index, uint16_t p_pwm);
uint8_t led_high_speed_mode_ctr(uint8_t step, uint16_t fade_time, uint16_t switch_time, light_drive_mode_enum mode);
uint8_t led_adc_pwm_volt_state(uint8_t index);
void Cob_Output_Disable(void);
void set_dev_lux(struct mixing_lux *set_lux);
void set_self_adapt_pwm(uint16_t* p_pwm);
#endif
/***********************************END OF FILE*******************************************/
