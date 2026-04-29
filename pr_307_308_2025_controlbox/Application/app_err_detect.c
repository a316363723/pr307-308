#include <string.h>
#include <stdint.h>
#include "dev_adapter.h"
#include "app_err_detect.h"
#include "local_data.h"
#include "ui_data.h"
#include "app_power.h"
#include "dev_power.h"
#include "os_event.h"
#define NUM_SOURCES 3
#define THRESHOLD 3
static osTimerId_t user_timer_id;
static uint32_t m_u32timerCount = 0;
static uint32_t m_no_ignora = 0;
static uint32_t m_check_time = 0; 
static bool m_regognition_error = 0;
typedef void (*FunctionPtr)(bool );
void adapter_test(void);
typedef struct {
    float prev_data; 
    uint32_t repeat_count[2]; 
} DataSource;

FunctionPtr error_functionArray[error_code_max] = {NULL}; 
DataSource data_sources[error_code_max]; 

void registerFunction(FunctionPtr* arrayPtr, FunctionPtr function, int index) {
    arrayPtr[index] = function;
}

static sys_error_type error_code;
osThreadId_t app_error_code_id;
const osThreadAttr_t g_error_code_cb_thread_attr = { 
    .name = "app_error_code_cb", 
    .priority = osPriorityLow, 
#if PROJECT_TYPE==307
	.stack_size = 128 * 4
#elif PROJECT_TYPE==308
    .stack_size = 256 * 4
#endif
};


static void timer_IncTick(void)
{
    m_u32timerCount += 1;
}

/**
 * @brief Provides a tick value in millisecond.
 * @param None
 * @retval Tick value
 */
static  uint32_t timer_getTick(void)
{
    return m_u32timerCount;
}

struct adapter_user_info_t adapter_output;
int is_same_data(uint8_t data1, uint8_t data2) {
    if (data1 == data2) {
        return 1;
    } else {
        return 0;
    }
}


bool get_left_adapter_error(void)
{
	adapter_a_and_b_ctrl(ADAPTER_A_EN);
	osDelay(10);
	adapter_read_info(&adapter_output);
	if(adapter_output.v_output > 40 && adapter_output.v_output < 51)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

bool get_right_adapter_error(void)
{
	adapter_a_and_b_ctrl(ADAPTER_B_EN);
	osDelay(10);
	adapter_read_info(&adapter_output);
	if(adapter_output.v_output > 43 && adapter_output.v_output < 52)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}
uint32_t adapter_err_tic;
void adapter_error_protect(void)
{

	static uint32_t  protective_count = 0;
	if(app_power_get_voltage_value() == 0) return ;
	if(m_check_time++ < 6000) return ;
	m_check_time = 6000;
	if(timer_getTick() - protective_count < 2000) return ;
	protective_count = timer_getTick();
	if(ui_get_power_type() != POWER_TYPE_DC){ m_check_time = 0 ; return;}
	if( get_left_adapter_error() == 0 || get_right_adapter_error() == 0 )
	{
		adapter_err_tic++;
		data_sources[adapter_output_ec_003].repeat_count[0] ++;
	}
	else
	{
		data_sources[adapter_output_ec_003].repeat_count[0] = 0;
	}
	if(data_sources[adapter_output_ec_003].repeat_count[0] > 10)
	{
		data_sources[adapter_output_ec_003].repeat_count[0] = 10;
		error_functionArray[adapter_output_ec_003](1);
	}
	else 
	{ 
		error_functionArray[adapter_output_ec_003](0);
	}
}


void total_power_error_protect(void)
{
	static uint32_t  protective_count = 0;
	if(timer_getTick() - protective_count < 1000) return ;
	protective_count = timer_getTick();
	#if (PROJECT_TYPE == 307) 
	if(get_total_power() > 2050)
	#elif (PROJECT_TYPE == 308)  
	if(get_total_power() >3600)
	#endif  
	{
		data_sources[output_power_ec_012].repeat_count[0] ++;
	}
	else
	{
		data_sources[output_power_ec_012].repeat_count[0] = 0;
	}
	if(data_sources[output_power_ec_012].repeat_count[0] > 10)
	{
		data_sources[output_power_ec_012].repeat_count[0] = 10;
		error_functionArray[output_power_ec_012](1);
	}
	else 
	{
		//error_functionArray[output_power_ec_012](0);
	}

}


void ctrl_mcu_error_set(bool res)
{
	error_code.error_ec.mcu_temp_ec_002 = res;
	m_no_ignora = res;
}

void ctrl_adapter_error_set(bool res)
{
	error_code.error_ec.adapter_output_ec_003 = res;
	m_no_ignora = res;
}

void ctrl_hard_drive_error_set(bool res)
{
	error_code.error_ec.hard_drive_ec_005 = res;
}

void ctrl_ble_error_set(bool res)
{
	error_code.error_ec.ble_communications_ec_007 = res;
}

void ctrl_crmx_error_set(bool res)
{
	error_code.error_ec.lumenridio_ec_009 = res;
}


void ctrl_fan_error_set(bool res)
{
	error_code.error_ec.fan_ec_010 = res;
	m_no_ignora = res;
}

void ctrl_lfs_error_set(bool res)
{
	error_code.error_ec.flash_ec_011 = res;
}

void ctrl_outopt_power_error_set(bool res)
{
	error_code.error_ec.abnormal_power_ec_012 = res;
	m_no_ignora = res;
}


void ctrl_hard_unbalanced_power_error_set(bool res)
{
	error_code.error_ec.dc_unbalanced_power_ec_013 = res;
}


void ctrl_dc_interface_voltage_error_set(bool res)
{
	error_code.error_ec.dc_interface_voltage_ec_014 = res;
}


void error_code_function_registration(void)
{
	registerFunction(error_functionArray, ctrl_mcu_error_set, mcu_temp_ec_002);
	registerFunction(error_functionArray, ctrl_adapter_error_set, adapter_output_ec_003);
	registerFunction(error_functionArray, ctrl_hard_drive_error_set, hard_drive_ec_005);
	registerFunction(error_functionArray, ctrl_ble_error_set, ble_communications_ec_007);
	registerFunction(error_functionArray, ctrl_crmx_error_set, lumenridio_ec_009);
	registerFunction(error_functionArray, ctrl_fan_error_set, fan_ec_010);
	registerFunction(error_functionArray, ctrl_lfs_error_set, flash_ec_011);
	registerFunction(error_functionArray, ctrl_outopt_power_error_set, output_power_ec_012);
	registerFunction(error_functionArray, ctrl_hard_unbalanced_power_error_set, dc_unbalanced_power_ec_013);
	registerFunction(error_functionArray, ctrl_dc_interface_voltage_error_set, dc_interface_voltage_ec_014);
}


static void rs485_error_event_deal(void)
{
	static struct sys_info_lamp   s_info_read_body = {0};
	struct sys_info_power sys_power;
	static uint32_t  protective_count = 0;
	static bool g_analog_dim_atuo_boot = 0;
	uint8_t hs_mode = 0;
	uint8_t state = ui_error_code_event_get() ;
	data_center_read_sys_info(SYS_INFO_POWER, &sys_power);
	if(sys_power.state == 0)  return ;
	data_center_read_config_data(SYS_CONFIG_HIGH_SPEED_MODE, &hs_mode);
	data_center_read_sys_info(SYS_INFO_LAMP, &s_info_read_body);	
	if(s_info_read_body.exist== 0)return ;
	switch(state)
	{
		case 0:	
			if((timer_getTick() - protective_count) > 100 && g_analog_dim_atuo_boot == 1)
			{	

				os_ev_publish_event(MAIN_EV_LIGHT, EV_DATA_LIGHT_SWITCH, 1);
				g_analog_dim_atuo_boot = 0;
				
			}
			break;
		case 1:	
			if(hs_mode)
			{
				os_ev_publish_event(MAIN_EV_LIGHT, EV_DATA_LIGHT_SWITCH, 0);			
				g_analog_dim_atuo_boot = 1;
			}
			else if(m_no_ignora)
			{
				os_ev_publish_event(MAIN_EV_LIGHT, EV_DATA_LIGHT_SWITCH, 0);
				g_analog_dim_atuo_boot = 1;
			}
			protective_count = timer_getTick();
			break;
		default: break;
	}
}

void user_time_ms_tick(void *argument)
{
	timer_IncTick();
}


extern osSemaphoreId_t thread_sync_sigl;
void app_error_code_dtection(void *argument)
{
    m_check_time = 0;
	adapter_init_config();
	static sys_error_type s_error_code = {0};
	user_timer_id = osTimerNew(user_time_ms_tick, osTimerPeriodic, NULL, NULL);
	osTimerStart(user_timer_id, 1);
	error_code_function_registration();
	osSemaphoreRelease(thread_sync_sigl);
	while(1)
    {
		rs485_error_event_deal();
		total_power_error_protect();
//		adapter_error_protect();
		data_center_read_sys_info(SYS_INFO_ERROR_TYPE, &s_error_code);
		s_error_code.error_ec = error_code.error_ec;
		data_center_write_sys_info(SYS_INFO_ERROR_TYPE, &s_error_code);
		osDelay(200);
	}
}


void adapter_test(void)
{
	struct adapter_output_set_t adapter_output_set = {
		.voltage_high = 0x50,
		.voltage_low = 0x55,
		.current_high = 0x5,
		.current_low = 0x55,
	};
	#if 1
	adapter_a_and_b_ctrl(ADAPTER_A_EN);
	osDelay(10);
	adapter_read_info(&adapter_output);
	#endif
	
	#if 1
	adapter_a_and_b_ctrl(ADAPTER_A_EN);
	osDelay(10);
	adapter_switch_ctrl(1);
	#endif
	
	
	#if 1
	adapter_a_and_b_ctrl(ADAPTER_A_EN);
	osDelay(10);
	adapter_switch_ctrl(0);
	#endif
	
	#if 1
	adapter_a_and_b_ctrl(ADAPTER_A_EN);
	osDelay(10);
	adapter_set_voltage_and_current(&adapter_output_set);
	#endif
	
}
