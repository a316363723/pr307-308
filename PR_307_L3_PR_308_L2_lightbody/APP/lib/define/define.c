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
#include "define.h"

/*****************************************************************************************
							  Variables definitions
*****************************************************************************************/
#ifdef PR_308_L2
const float silence_mode_power_rate[DRIVE_MODE_NUM] = {(float)SILENCE_MODE_POWER / PWM_POWER, 0.160f};
#endif
#ifdef PR_307_L3
const float silence_mode_power_rate[DRIVE_MODE_NUM] = {(float)SILENCE_MODE_POWER / PWM_POWER, (float)SILENCE_MODE_POWER / ANA_POWER};
#endif
Rs485_DMA_Ctr_TypeDef g_str_rs485_dma;

Adc_Para_TypeDef g_adc_para;
Adc_Volt_TypeDef g_volt_para;

Upgrade_TypeDef g_str_ota = {
	.stage = RS485_File_TX_Step_Start,
	.succ_flag = ERROR,
};
Power_Factor_TypeDef g_power_factor = {
    .illumination = 1.0f,
	.high_temp_rate = 1.0f,
	.silence_mode_rate = 1.0f,
	.frq_change = 1.0f,
	.sum_power_rate = 1.0f,
};

Power_Limit_TypeDef g_power_limit = {
	.power_calibration = {1.0f,1.0f},
	.crc_sum = {0, 0},
	.calibration_flag = {0, 0},
};

Single_Power_Limit_TypeDef g_single_power_limit = {
	.single_power_calibration = {9000,9000,9000,9000,9000,9000,9000},
	.crc_sum = {0, 0, 0, 0, 0, 0, 0},
	.single_calibration_flag = {0, 0, 0, 0, 0, 0, 0},
};

Sys_Error_TypeDef g_error_state = {
	.err_msg = 0,
};
Gyroscope_TypeDef g_gyroscope = {
	.gyroscope_type = USE_NULL,
    .yaw_angle = 0,
	.pitch_angle = 0,
    .roll_angle = 0,
};
uint32_t g_runtime = 0;
uint32_t g_clear_run_time = 0;
uint8_t g_self_adapt_version_not_match = 0;
