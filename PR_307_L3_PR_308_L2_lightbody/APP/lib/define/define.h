/****************************************************************************************
**  Filename :  111.h
**  Abstract :  。
**  By       :  何建国
**  Date     :  
**  Changelog:1.First Create
*****************************************************************************************/
#ifndef _DEFINE_H
#define _DEFINE_H
/*****************************************************************************************
									Includes
*****************************************************************************************/
#include "aes.h"
#include "delay.h"
#include "logic.h"
#include "sys.h"
#include "light_effect.h"
#include "SidusProFX.h"
#include "Rs485_Proto_Slave_Analysis.h"
#include "color_mixing.h"
#include "project_config.h"
/*****************************************************************************************
								Macro definitions
*****************************************************************************************/
#define RS485_RX_BUFF_LEN (1024UL)
#define GLOBAL_PRINT_ENABLE		0

#define COBE_CCT_MODE           0 //1 - 双色温， 0 - 单色温
#define IWDG_EN					0
#define LAMP_OFF				0
#define LAMP_ON					1
#define LAMP_FAN_ERROR_OFF		2 //堵转保护
#define LAMP_TEMP_ERROR_OFF		3 //温度保护

#define RPM_CNT_ERROR           200//判断未堵转的转速

#define CLOSE_MACHINE_TEMP      		95     //关机温度
#define DEC_POWER_TEMP          		90      //降功率温度
#define RESTORE_POWER_TEMP      		80      //恢复功率温度
#define CLOSE_MACHINE_RESTORE_TEMP      75     //关机恢复亮灯温度

#define MCU_CLOSE_LED_TEMP      70      //mcu过温关灯温度
#define MCU_RESTORE_TEMP        60      //mcu过温关灯恢复亮灯温度

#define USE_MPU6050             1
#define USE_LSM6DSV16X          2
#define USE_NULL          		0
//#define GYROSCOPE_TYPE          USE_NULL
#define LIGHTING 				1 

#define ENCODE
/*****************************************************************************************
								Typedef definitions
*****************************************************************************************/
typedef enum {DISABLE = 0, ENABLE = !DISABLE} EventStatus, ControlStatus;
typedef enum {RESET = 0, SET = !RESET} FlagStatus;
typedef enum {ERROR = 0, SUCCESS = !ERROR} ErrStatus;
typedef enum {
    PWM_MODE = 0,
    ANALOG_MODE,
    DRIVE_MODE_NUM,
}light_drive_mode_enum;

//升级头信息
typedef __packed struct{

    char   ProductName[16]; //产品名称
    uint32_t FirmwareVersion; //软件版本
    uint32_t HardwareVersion; //硬件版本
    uint32_t FirmwareSize;  //固件长度
    uint32_t CheckCRC32;  //加密前的Firmware crc32校验
    uint16_t FirmwareType;      //0x00:app  0x01:base_data 0x02:app + base_data
	uint16_t DriveMode;//307&308基础数据专用
}sFirmwareBin;
//upgrade
typedef __packed struct
{
	ControlStatus enable;
	uint32_t start_addr;
	uint16_t sector_num;
}Ota_Erase_TypeDef;
typedef __packed struct
{
	uint32_t len;
	uint32_t acc;
	uint16_t sel;
	uint32_t crc;
}Ota_File_TypeDef;
typedef __packed struct
{
	uint32_t start_addr;
}Ota_Write_TypeDef;
typedef __packed struct
{
	ControlStatus enable;
	rs485_ack_enum type;
}Ota_Ack_TypeDef;
typedef __packed struct
{
	rs485_file_tx_step_enum stage;
	ErrStatus succ_flag;
	Ota_Erase_TypeDef erase;
	Ota_File_TypeDef file;
	Ota_Write_TypeDef write;
	Ota_Ack_TypeDef ack;
}Upgrade_TypeDef;
//ADC
typedef __packed struct
{
    uint16_t Mcu_Volt;
	char borad_temper;
	char cob_temper;
	char envir_temper;
	
	uint32_t Vin;
	uint32_t V_ww_adapt;
	uint32_t V_cw_adapt;

	
	
	uint32_t V_check_48V;
	float V_check_red;
	float V_check_green;
	float V_check_blue;
	float V_check_cw1;
	float V_check_cw2;
	float V_check_ww1;
	float V_check_ww2;

	uint32_t V_ww1_adapt;
	uint32_t V_cw1_adapt;
	uint32_t V_ww2_adapt;
	uint32_t V_cw2_adapt;

	char mcu_temper;
	uint32_t V_check_YU;
	uint32_t V_red_adapt;
	uint32_t V_green_adapt;
	uint32_t V_blue_adapt;
	char cob4_temper;
	char cob3_temper;
	uint32_t V_YUADC_adapt;
	float V_check_contact1;
	float V_check_contact2;
}Adc_Para_TypeDef;
//volt
typedef __packed struct
{
    float Mcu_Volt;
	float borad_temper;
	float cob_temper;
	float envir_temper;
	float Vin;
	
	float V_red_adapt;
	float V_green_adapt;
	float V_blue_adapt;
	float V_ww_adapt;
	float V_cw_adapt;

	float V_check_48V;
	float V_check_red;
	float V_check_green;
	float V_check_blue;
	float V_check_cw1;
	float V_check_cw2;
	float V_check_ww1;
	float V_check_ww2;

	float V_ww1_adapt;
	float V_cw1_adapt;
	float V_ww2_adapt;
	float V_cw2_adapt;

	float mcu_temper;
	float V_check_YU;
	float cob4_temper;
	float cob3_temper;
	float V_YUADC_adapt;
	float V_check_contact1;
	float V_check_contact2;
}Adc_Volt_TypeDef;

//影响灯体输出功率的参数
typedef __packed struct
{
    float illumination; /*主机下发的招录比率*/
	float high_temp_rate;    /*高温降功率的比例*/
	float silence_mode_rate;/*风扇静音模式功率限制比例*/
	float frq_change;   /*初始调光频率与当前频率的比例*/
	float sum_power_rate;   /*总功率比例*/
}Power_Factor_TypeDef;
//功率校准
typedef __packed struct{
	float power_calibration[DRIVE_MODE_NUM];
	uint8_t crc_sum[DRIVE_MODE_NUM];
	uint8_t calibration_flag[DRIVE_MODE_NUM];
}Power_Limit_TypeDef;

//单路功率校准
typedef __packed struct{
	uint16_t single_power_calibration[7];
	uint8_t crc_sum[7];
	uint8_t single_calibration_flag[7];
}Single_Power_Limit_TypeDef;
//RS485 DMA控制
typedef __packed struct
{
	uint8_t rx_buff[RS485_RX_BUFF_LEN];
	uint8_t rx_idle_flag;
	uint16_t rx_len;
}Rs485_DMA_Ctr_TypeDef;

typedef __packed union
{
     uint32_t err_msg;
    __packed struct 
    {
		uint32_t lamp_with_protect_cover_eh_001	: 1;
		uint32_t led_temp_eh_002				: 1;
		uint32_t mcu_temp_eh_003 				: 1;
		uint32_t battery_voltage_eh_004			: 1;
		uint32_t hard_drive_eh_005				: 1;
		uint32_t ctrl_communications_eh_006 	: 1;
		uint32_t ble_communications_eh_007 		: 1;
		uint32_t sq_net_eh_008					: 1;
		uint32_t lumenridio_eh_009 				: 1;
		uint32_t fan_eh_010 					: 1;
		uint32_t gyroscope_eh_011 				: 1;
		uint32_t cool_eh_012 					: 1;
		uint32_t electric_support_eh_013 		: 1;
		uint32_t electric_accessories_eh_014 	: 1;
		uint32_t flash_eh_015 					: 1;
		uint32_t ambient_temp_eh_016 			: 1;
		uint32_t lamp_head_not_match_eh_017   	: 1;
		uint32_t reserve_eh_018   	            : 1;
		uint32_t reserve_eh_019   	            : 1;
		uint32_t reserve_eh_020   	            : 1;
		uint32_t reserve_eh_021   	            : 1;
		uint32_t reserve_eh_022   	            : 1;
		uint32_t reserve_eh_023   	            : 1;
		uint32_t reserve_eh_024   	            : 1;
		uint32_t reserve_eh_025   	            : 1;
		uint32_t reserve_eh_026   	            : 1;
		uint32_t reserve_eh_027   	            : 1;
		uint32_t reserve_eh_028   	            : 1;
		uint32_t reserve_eh_029   	            : 1;
		uint32_t reserve_eh_030   	            : 1;
		uint32_t reserve_eh_031   	            : 1;
		uint32_t reserve_eh_032   	            : 1;
    };
}Sys_Error_TypeDef;

typedef __packed struct
{
  int8_t  gyroscope_type;
  int8_t  gyroscope_init_state;
  int16_t yaw_angle; 
  int16_t pitch_angle;
  int16_t roll_angle;
} Gyroscope_TypeDef;


typedef __packed struct
{
    uint16_t R_G;
	uint16_t G_W3;
	uint16_t B_C3;
	uint16_t C2_C2;
	uint16_t W2_W2;
	uint16_t C1_C1;
	uint16_t W1_W1;
}Led_Channel_TypeDef;              


typedef struct
{
	volatile uint16_t led_pwm[LED_CHANNEL_NUM];
	volatile uint16_t pwm_buffer_one[LAMP_FREQUENCY_DIV][LAMP_DRV_ONE_CHANNEL_NUM];
	volatile uint16_t pwm_buffer_two[LAMP_FREQUENCY_DIV][LAMP_DRV_TWO_CHANNEL_NUM];
} Led_Channel_Arg_TypeDef;
/*****************************************************************************************
							Global variables and functions
*****************************************************************************************/

extern const float silence_mode_power_rate[DRIVE_MODE_NUM];

extern Rs485_DMA_Ctr_TypeDef g_str_rs485_dma;
extern Sys_Error_TypeDef g_error_state;
extern uint8_t g_self_adapt_version_not_match;

extern Upgrade_TypeDef g_str_ota;
extern Adc_Para_TypeDef g_adc_para;
extern Adc_Volt_TypeDef g_volt_para;
extern Power_Factor_TypeDef g_power_factor;
extern Power_Limit_TypeDef g_power_limit;
extern Single_Power_Limit_TypeDef g_single_power_limit;
extern uint8_t g_rs485_rx[1024];

extern rs485_data_ctr_t g_rs485_data;
extern struct color_mixing g_color_mixing_struct;
extern Light_Effect g_effect_struct;
extern uint32_t g_runtime;
extern uint32_t g_clear_run_time;
extern SidusProFX_ArgBody_Type g_sidus_fx_str;
extern Gyroscope_TypeDef g_gyroscope;
extern Led_Channel_Arg_TypeDef g_led_pwm_arg;
//----------------------------------------------------------------------------------------
extern void Set_Device_Run_State(uint8_t state);
extern void Polling_Here_Fx_State(void);
extern void RS485_Logic_Ctr(void);
extern void upgrade_overtime_control(void);
extern void Upgrade_Process_Ctr(rs485_file_transfer_t* p_data, Rs485_Special_Ack_TypeDef* ack);
extern void Upgrade_Flash_Erase(void);
extern rs485_ack_enum Get_Upgrade_Start_State(void);
extern rs485_ack_enum get_cfx_file_state(void);
extern rs485_ack_enum get_cfx_init_state(void);
extern rs485_ack_enum get_cfx_rename_state(void);
extern void Main_Logic_Ctr(void);
extern void Cob_Slow_Ctr_Logic(void);
extern bool Set_Cfx_Name(rs485_cmd_arg_t* p_rs485_decode);
extern void High_Temp_Ctr_Logic(void);
extern uint8_t Get_Vin_Stable_State(void);
extern void Power_Down_State_Detect(void);

extern void RS485_Recv_Finish_Ctr(void);
extern void Rs485_Data_Send_Ctr(uint8_t* p_data, uint8_t len);

extern uint16_t Get_Fan_Curr_Spd(uint8_t sel);
extern ErrStatus Write_Upgrade_Flag(void);
extern void Upgrade_Data_Comb(void);

void update_power_rate(void);
uint8_t get_run_state(void);
#endif
/***********************************END OF FILE*******************************************/
