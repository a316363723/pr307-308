#ifndef APP_ERROR_CODE_H
#define APP_ERROR_CODE_H
#include "cmsis_os.h"
#include "stdbool.h"
enum
{
	 led_temp_ec_001,				
	 mcu_temp_ec_002 ,				
	 adapter_output_ec_003 	,		
	 battery_voltage_ec_004	,		
	 hard_drive_ec_005,				
	 ctrl_communications_ec_006, 	
	 ble_communications_ec_007 ,		
	 sq_net_ec_008 ,					
	 lumenridio_ec_009 	,			
	 fan_ec_010 ,					
	 flash_ec_011 ,					
	 output_power_ec_012 ,			
	 dc_unbalanced_power_ec_013 ,				
	 dc_interface_voltage_ec_014 ,					
	 error_code_max,
};

typedef void (*FunctionPtr)(bool );
extern FunctionPtr error_functionArray[error_code_max];
extern osThreadId_t app_error_code_id;
extern const osThreadAttr_t g_error_code_cb_thread_attr;
extern void app_error_code_dtection(void *argument);	

#endif
