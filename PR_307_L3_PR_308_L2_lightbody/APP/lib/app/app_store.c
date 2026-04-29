#include "app_store.h"
#include "bsp_w25qxx_q.h"
#include "lfs_flash.h"
#include "iot.h"
#include "define.h"
#include "data_acc.h"
#include "flash.h"

char pdd_file_name[]= "auto_data";	/* 掉电数据 Power-down data */
char single_power_file_name[]= "single_data";	/* 掉电数据 Power-down data */
char iot_file_name[] = "iot_data";  /* 掉电数据 iot data */	

static uint8_t s_power_calibration_flag = 0;/*功率校准标志：0 - 没有功率校准，1 - 有功率校准信号*/
static uint8_t s_single_power_calibration_flag[7] = {0,0,0,0,0,0,0};/*功率校准标志：0 - 没有功率校准，1 - 有功率校准信号*/
static  IOT_Parameter_TypeDef iot_storage_param;
static IOT_Parameter_Serial_TypeDef iot_state_param =
{
    .device_state_serial = DEVICE_OPEN,         //设备的开关机状态
};

static void set_store_power_rate_data(light_drive_mode_enum mode);
static void set_store_single_power_rate_data(void);
static void set_store_iot_data(uint32_t ticks);
static void update_iot_state(IOT_Parameter_Serial_TypeDef* p_iot_param);
static void get_lamp_run_time(void);
static void clear_run_time(void);

void store_data_recover_init(void)
{
	Qspi_Init();
	
	lfs_mount_fail = sys_lfs_mount();
	if(lfs_mount_fail)
	{
		g_error_state.flash_eh_015 = 1;
	}
	
	//功率校准数据恢复
	Read_Data_From_W25qxx((uint8_t *)&g_power_limit, sizeof(Power_Limit_TypeDef), pdd_file_name);
	for(uint8_t i = 0; i < DRIVE_MODE_NUM; i++)
	{
		if( g_power_limit.calibration_flag[i] != 1 )
		{
			g_power_limit.power_calibration[i] = 1;
			g_power_factor.illumination = 1;//不知道这个数值的意义何在
		}
	}
	
	//单路功率校准数据恢复
	Read_Data_From_W25qxx((uint8_t *)&g_single_power_limit, sizeof(Single_Power_Limit_TypeDef), single_power_file_name);
	for(uint8_t i = 0; i < 7; i++)
	{
		if( g_single_power_limit.single_calibration_flag[i] != 1 )
		{
			g_single_power_limit.single_power_calibration[i] = 9000;
		}
	}
	#ifdef PR_307_L3
	if(led_base_data.ver == 0x11 && analog_led_base_data.ver == 0x11)
	{
		for(uint8_t i = 0; i < 7; i++)
		{
			g_single_power_limit.single_power_calibration[i] = 9000;
		}
	}
	#endif
	#ifdef PR_308_L2
	if((*(uint8_t*)(BASE_DATA_OFFSET + 17) == 0x11) && (*(uint8_t*)(ANALOG_DATA_OFFSET + 17) == 0x11))
	{
		for(uint8_t i = 0; i < 7; i++)
		{
			g_single_power_limit.single_power_calibration[i] = 9000;
		}
	}
	#endif
	//iot数据恢复
	Read_Data_From_W25qxx((uint8_t *)&iot_storage_param, sizeof(IOT_Parameter_TypeDef), iot_file_name);
	recover_iot_data((uint8_t*)&iot_storage_param);
	get_lamp_run_time();
}

void store_data_task(void)
{
	set_store_power_rate_data((light_drive_mode_enum)g_rs485_data.high_speed.mode);
	set_store_iot_data(100);
	set_store_single_power_rate_data();
	clear_run_time();
}

void set_power_calibration_state(uint8_t flag)
{
    s_power_calibration_flag = flag;
}

void set_single_power_calibration_state(uint8_t channel,uint8_t flag)
{
    s_single_power_calibration_flag[channel] = flag;
}


static void set_store_power_rate_data(light_drive_mode_enum mode)
{
    uint8_t crc_val = 0;
    static uint8_t s_store_delay[2] = {255,255};
	if(0 == s_power_calibration_flag)
	{
		return;
	}
	crc_val =  ByteSum(sizeof(float), (uint8_t*)(&g_power_limit.power_calibration[mode]));
	if(g_power_limit.crc_sum[mode] != crc_val)
	{
		g_power_limit.crc_sum[mode] = crc_val;
		s_store_delay[mode] = 0;
	}
    if(s_store_delay[mode] >= 10)
        return;
    s_store_delay[mode]++; 
    if(s_store_delay[mode] >= 10)//长时间数据没有发生变化，则确定该功率值
    {
		g_power_limit.calibration_flag[mode] = 1;
		Write_Data_To_W25qxx((uint8_t *)&g_power_limit, sizeof(Power_Limit_TypeDef), pdd_file_name);
		set_power_calibration_state(0);
    }
}

static void set_store_single_power_rate_data(void)
{
    uint8_t crc_val = 0;
	uint8_t need_save_flag = 0;
	uint8_t save_channel = 0;
    static uint8_t s_store_delay[7] = {255,255,255,255,255,255,255};
	for(uint8_t i = 0; i < 7;i++)
	{
		if(s_single_power_calibration_flag[i] == 1)
		{
			save_channel = i;
			need_save_flag++;
			break;
		}
	}
	if(0 == need_save_flag)
	{
		return;
	}
	crc_val =  ByteSum(2, (uint8_t*)(&g_single_power_limit.single_power_calibration[save_channel]));
	if(g_single_power_limit.crc_sum[save_channel] != crc_val)
	{
		g_single_power_limit.crc_sum[save_channel] = crc_val;
		s_store_delay[save_channel] = 0;
	}
    if(s_store_delay[save_channel] >= 10)
        return;
    s_store_delay[save_channel]++; 
    if(s_store_delay[save_channel] >= 10)//长时间数据没有发生变化，则确定该功率值
    {
		g_single_power_limit.single_calibration_flag[save_channel] = 1;
		Write_Data_To_W25qxx((uint8_t *)&g_single_power_limit, sizeof(Single_Power_Limit_TypeDef), single_power_file_name);
		set_single_power_calibration_state(save_channel,0);
    }
}


static void set_store_iot_data(uint32_t ticks)//ticks等于任务轮询的间隔的时间(ms)
{
	static uint32_t tick_cnt = 0;
    static uint32_t ticks_iot  = 0;
    if(++ticks_iot >= 60000 / ticks)
    {
        ticks_iot = 0;
        get_iot_data((uint8_t*)&iot_storage_param);//iot_storage_param用于更新存储 
		Write_Data_To_W25qxx((uint8_t *)&iot_storage_param,sizeof(IOT_Parameter_TypeDef), iot_file_name);
		get_lamp_run_time();
    }
    if(++tick_cnt >= 1000 / ticks)
    {
        tick_cnt = 0;  
        update_iot_state(&iot_state_param);//iot_state_param用于当前时刻状态的记录
		if(g_rs485_data.power_switch.state && ((g_rs485_data.fx_mode_para.intensity > 0) || (g_rs485_data.light_mode_para.intensity > 0)))
		{
			dispose_iot_parameter_data(&iot_state_param);
		}
    }
}

static void update_iot_state(IOT_Parameter_Serial_TypeDef* p_iot_param)
{
	 p_iot_param->device_state_serial = (IOT_Device_State)g_rs485_data.power_switch.state;
}

static void get_lamp_run_time(void)
{
	g_runtime = iot_storage_param.device_state_time[1];
}

static void clear_run_time(void)
{
	if(g_clear_run_time == 1)
	{
		g_clear_run_time = 0;
		iot_storage_param.device_state_time[1] = 0;
		Write_Data_To_W25qxx((uint8_t *)&iot_storage_param,sizeof(IOT_Parameter_TypeDef), iot_file_name);
//		recover_iot_data((uint8_t*)&iot_storage_param);
		recover_run_time();
		g_runtime = iot_storage_param.device_state_time[1];
	}
}
