#include "app_light.h"
#include "local_data.h"
#include <string.h>
#include <stdint.h>
#include "os_event.h"
#include "user.h"
#include "app_data_center.h"
#include <stdint.h>
#include "dev_rs485.h"
#include "update_module.h"
#include "crc32.h"
#include "hal_timer.h"
#include "hal_iic.h"
#include "dev_power.h"
#include "app_power.h"
#include "ui_data.h"
#include "ui_common.h"
#include "app_basic_data.h"
#include "SidusProFX.h"
#include "SidusPro_Interface.h"
#include "SidusProFile.h"
#include "dev_ble.h"
#include "app_ble_port.h"
#include "app_ble_protocol_parse.h"
#include "perf_counter.h"
#include "hal_gpio.h"
#include "hc32f4a0_utility.h"
#include "app_light.h"
#include "api_rs485_proto.h"
#include "app_light_data_analyze.h"
#include "app_dac_self_adapt.h"
#include "dev_pwm_self.h"
#include "dev_pwm_self.h"
#include "app_err_detect.h"
/*------------------------------------*/
/*------Dimming_Curve:0x08------*/
/*------------------------------------*/


#define RS485_FADE_TIME      (350)


#if PROJECT_TYPE==307
#define LAMP_PRODUCT  0X09
#elif PROJECT_TYPE == 308
#define LAMP_PRODUCT  0X10
#endif

void light_get_event_deal(uint32_t event_type, uint32_t event_value);
static void light_unconnet_handle(void);
static void dmx_disconnected_handle(uint32_t event_type, uint32_t event_value);
static void light_flags_handle(void);
static struct sys_info_motor       s_light_motor_info;
static struct sys_info_motor_state  motor_state;
void light_dac_fade_handle(void *argument);
bool light_rs485_trigger(uint8_t mode, uint8_t trigger_state);
void rs485_hs_mode_switch(uint8_t type);
static uint8_t read_buff[200] = {0};
#pragma pack (1)

#define CTRL_DEVICE_TYPE (2)

/* 灯体信息 */
static struct sys_info_lamp   s_info_read_body = {0};
static osTimerId_t light_timer_id;
static osTimerId_t light_timer_fade_id;
static dmx_loss_state s_dmx_loss_state;
static  light_boby_t s_light_arg;
static uint32_t m_u32timerCount = 0;
static uint8_t mdx_loss_start = 0;
osSemaphoreId Rs485_TSemaphore = NULL;
osMessageQueueId_t myQueue_485_RX;
osMessageQueueId_t myQueue_485_TX;
osMutexId_t     MutexID_Rs485;
static struct sys_info_accessories s_access_info;
static uint8_t s_motor_demo_flag;
static uint8_t s_motor_reset_type;

static void timer_IncTick(void)
{
    m_u32timerCount += 1;
}


static  uint32_t timer_getTick(void)
{
    return m_u32timerCount;
}

struct power_adjust_limit_t
{
	uint16_t upper;
	uint16_t lower;
};


struct dev_power_adjust_t
{
	uint16_t adjust_value;
	rs485_cmd_body_t  pwm_body;
	rs485_cmd_body_t  dim_body;
	struct power_adjust_limit_t pwm_limit;
	struct power_adjust_limit_t dim_limit;
};

static osSemaphoreId_t analog_dim_signal;

bool rs485_analog_dac_data_get_init(dev_lamp_t* dev_lamp)
{
    memset(read_buff, 0, sizeof(read_buff));
    rs485_cmd_body_t* p_read_body = (rs485_cmd_body_t*)read_buff;
	uint16_t *dac_init = (uint16_t* )&dac_fade.analog_dac_init;
    uint16_t body_size = 0;
    uint8_t res = 0;
       
    if(osOK != osMutexAcquire(MutexID_Rs485, 0))
    {
        return 0;
    }
    
    rs485_cmd_body_t cmd_body_1 = {.header.rw = 0, .header.cmd_type = RS485_Cmd_Analog_Dim, .cmd_arg.analog_dim_t.type = 0, .cmd_arg.analog_dim_t.led_color_type = HAL_PWM_SELF_ADAPTION_R,};
    rs485_cmd_body_t cmd_body_2 = {.header.rw = 0, .header.cmd_type = RS485_Cmd_Analog_Dim, .cmd_arg.analog_dim_t.type = 0, .cmd_arg.analog_dim_t.led_color_type = HAL_PWM_SELF_ADAPTION_G,};
    rs485_cmd_body_t cmd_body_3 = {.header.rw = 0, .header.cmd_type = RS485_Cmd_Analog_Dim, .cmd_arg.analog_dim_t.type = 0, .cmd_arg.analog_dim_t.led_color_type = HAL_PWM_SELF_ADAPTION_B,};
    rs485_cmd_body_t cmd_body_4 = {.header.rw = 0, .header.cmd_type = RS485_Cmd_Analog_Dim, .cmd_arg.analog_dim_t.type = 0, .cmd_arg.analog_dim_t.led_color_type = HAL_PWM_SELF_ADAPTION_WW1,};
    rs485_cmd_body_t cmd_body_5 = {.header.rw = 0, .header.cmd_type = RS485_Cmd_Analog_Dim, .cmd_arg.analog_dim_t.type = 0, .cmd_arg.analog_dim_t.led_color_type = HAL_PWM_SELF_ADAPTION_CW1,};
    
	if(0 == dev_rs485_cmd_read(&dev_lamp->rs485, 5, 100, true, read_buff, &cmd_body_1, &cmd_body_2, \
            &cmd_body_3, &cmd_body_4,&cmd_body_5, DEV_RS485_VA_ARG_END))
    {
		for(uint8_t i = 0 ; i< 5 ; i++)
        {
			*dac_init = p_read_body->cmd_arg.analog_dim_t.adjust_val;
			 dac_init++;
            //hal_timer_duty_cycle_set( (enum hal_timera_enum)i ,*p_pwm_value) ;
            body_size += sizeof(rs485_cmd_header_t) + p_read_body->header.arg_size;
            p_read_body = (rs485_cmd_body_t*)((uint32_t)read_buff + body_size);
        }
		res = 1;
	}
	else
	{
		res = 0;
	}
    osMutexRelease(MutexID_Rs485);
    return res;
}

bool rs485_analog_dac_data_get(dev_lamp_t* dev_lamp)
{
    sys_config_t config_boby;
    data_center_read_config_data(SYS_CONFIG_HIGH_SPEED_MODE, &config_boby.hs_mode);
	
    if(osSemaphoreAcquire(analog_dim_signal, 0) != osOK ||  config_boby.hs_mode == 0 || dev_lamp->Connected == 0)
        return 0;
	if((PAGE_UPDATE == screen_get_act_pid() && SUB_ID_UPDATING == screen_get_act_spid()) || 
	   (PAGE_WORK_MODE == screen_get_act_pid() && screen_get_act_spid() == SUB_ID_SYNC_UPDATAING))
	{
		return 0;
	}
    if( date_center_get_light_mode() != LIGHT_MODE_CCT && date_center_get_light_mode() != LIGHT_MODE_HSI  &&  \
	    date_center_get_light_mode() != LIGHT_MODE_GEL && date_center_get_light_mode() != LIGHT_MODE_RGB &&  \
		date_center_get_light_mode() != LIGHT_MODE_SOURCE && \
		date_center_get_light_mode() != LIGHT_MODE_XY && screen_get_act_pid() != PAGE_TEST && screen_get_act_pid() != PAGE_CALIBRATION_MODE)
	return 0;	
    memset(read_buff, 0, sizeof(read_buff));
    rs485_cmd_body_t* p_read_body = (rs485_cmd_body_t*)read_buff;
    uint16_t *dac_value = (uint16_t* )&dac_fade.analog_dac_end;
    uint16_t body_size = 0;
    uint8_t res = 0;
       
    if(osOK != osMutexAcquire(MutexID_Rs485, 0))
    {
        return 0;
    }
    
    rs485_cmd_body_t cmd_body_1 = {.header.rw = 0, .header.cmd_type = RS485_Cmd_Analog_Dim, .cmd_arg.analog_dim_t.type = 1, .cmd_arg.analog_dim_t.led_color_type = HAL_PWM_SELF_ADAPTION_R,};
    rs485_cmd_body_t cmd_body_2 = {.header.rw = 0, .header.cmd_type = RS485_Cmd_Analog_Dim, .cmd_arg.analog_dim_t.type = 1, .cmd_arg.analog_dim_t.led_color_type = HAL_PWM_SELF_ADAPTION_G,};
    rs485_cmd_body_t cmd_body_3 = {.header.rw = 0, .header.cmd_type = RS485_Cmd_Analog_Dim, .cmd_arg.analog_dim_t.type = 1, .cmd_arg.analog_dim_t.led_color_type = HAL_PWM_SELF_ADAPTION_B,};
    rs485_cmd_body_t cmd_body_4 = {.header.rw = 0, .header.cmd_type = RS485_Cmd_Analog_Dim, .cmd_arg.analog_dim_t.type = 1, .cmd_arg.analog_dim_t.led_color_type = HAL_PWM_SELF_ADAPTION_WW1,};
    rs485_cmd_body_t cmd_body_5 = {.header.rw = 0, .header.cmd_type = RS485_Cmd_Analog_Dim, .cmd_arg.analog_dim_t.type = 1, .cmd_arg.analog_dim_t.led_color_type = HAL_PWM_SELF_ADAPTION_CW1,};
    
	if(0 == dev_rs485_cmd_read(&dev_lamp->rs485, 5, 100, true, read_buff, &cmd_body_1, &cmd_body_2, \
            &cmd_body_3, &cmd_body_4,&cmd_body_5, DEV_RS485_VA_ARG_END))
    {
		for(uint8_t i = 0 ; i< 5 ; i++)
        {
			*dac_value = p_read_body->cmd_arg.analog_dim_t.adjust_val;
             dac_value++;
            //hal_timer_duty_cycle_set( (enum hal_timera_enum)i ,*p_pwm_value) ;
            body_size += sizeof(rs485_cmd_header_t) + p_read_body->header.arg_size;
            p_read_body = (rs485_cmd_body_t*)((uint32_t)read_buff + body_size);
        }
        analog_dim_fade_restart();  
		res = 1;
	}
	else
	{
		res = 0;
	}
    osMutexRelease(MutexID_Rs485);
    return res;
}

bool rs485_fresnel_Industrial_data_test(void)
{
	if( dev_lamp.Connected == 0 ||  ui_test_Fresnel_Industrial_mode_get() == 0)
    {
        return  0;
    }
	rs485_set_Industrial_fresnel_angle(&dev_lamp, true, 5, 100);	
	ui_test_Fresnel_Industrial_mode_set(0);
}

bool rs485_rgbww_data_test(void)
{
    static rs485_cmd_body_t cmd_body_1;
	rs485_dac_dim_t  dac_body={0};
	static uint16_t rgbww[7];
	static uint16_t rgbww_last[7];
	#if PROJECT_TYPE == 308
	uint8_t iic_buff[2];
	#endif
	uint8_t hs_mode;
    if( dev_lamp.Connected == 0 ||  ui_get_pwm_PwmMode() == 0)
    {
        return  0;
    }
	
	ui_get_pwm_rgbww(rgbww);
	if(memcmp(rgbww,rgbww_last,sizeof(rgbww)) == 0 )  return  0;
	memcpy(rgbww_last,rgbww,sizeof(rgbww));
	data_center_read_config_data(SYS_CONFIG_HIGH_SPEED_MODE, &hs_mode);
	if(hs_mode)
	{
		dac_body.r = (rgbww[0]> 2740) ? 2740 :rgbww[0];
		dac_body.g = (rgbww[1]> 2740) ? 2740 :rgbww[1];
		dac_body.b = (rgbww[2]> 2740) ? 2740 :rgbww[2];
		dac_body.ww = (rgbww[3]> 2740) ? 2740 :rgbww[3];
		dac_body.cw = (rgbww[4]> 2740) ? 2740 :rgbww[4];
		#if PROJECT_TYPE == 307
		if(dac_body.g > dac_fade.analog_dac_init.g )    
		{
			dev_analog_green_ctrl(true);
			
		}
		else
		{
			dev_analog_green_ctrl(false);
		} 
		analog_dim_output_test(&dac_body);
		#endif
		#if PROJECT_TYPE == 308
		iic_buff[0] = GET_16BIT_HIGH8BIT(dac_body.g);
		iic_buff[1] = GET_16BIT_LOW8BIT(dac_body.g);
		hal_iic_send_data(HAL_IIC_ANALOG_CW1_CW2_R_G,(uint8_t)IIC_CW1_ADDRESS,iic_buff,2);
		
		iic_buff[0] = GET_16BIT_HIGH8BIT(dac_body.r);
		iic_buff[1] = GET_16BIT_LOW8BIT(dac_body.r);
		hal_iic_send_data(HAL_IIC_ANALOG_WW1_WW2_B,(uint8_t)IIC_WW1_ADDRESS,iic_buff,2);   
		
		iic_buff[0] = GET_16BIT_HIGH8BIT(dac_body.b);
		iic_buff[1] = GET_16BIT_LOW8BIT(dac_body.b); 
		hal_iic_send_data(HAL_IIC_ANALOG_WW1_WW2_B,(uint8_t)IIC_B_ADDRESS,iic_buff,2); 
		
		iic_buff[0] = GET_16BIT_HIGH8BIT(dac_body.cw);
		iic_buff[1] = GET_16BIT_LOW8BIT(dac_body.cw);
		hal_iic_send_data(HAL_IIC_ANALOG_CW1_CW2_R_G,(uint8_t)IIC_CW2_ADDRESS,iic_buff,2);
		
		iic_buff[0] = GET_16BIT_HIGH8BIT(dac_body.ww);
		iic_buff[1] = GET_16BIT_LOW8BIT(dac_body.ww);
		hal_iic_send_data(HAL_IIC_ANALOG_WW1_WW2_B,(uint8_t)IIC_WW2_ADDRESS,iic_buff,2);
		
		rgbww[5] = (rgbww[5]> 2740) ? 2740 :rgbww[5];
		iic_buff[0] = GET_16BIT_HIGH8BIT(rgbww[5]);
		iic_buff[1] = GET_16BIT_LOW8BIT(rgbww[5]);
		hal_iic_send_data(HAL_IIC_ANALOG_CW1_CW2_R_G,(uint8_t)IIC_R_ADDRESS,iic_buff,2); 
		
		rgbww[6] = (rgbww[6]> 2740) ? 2740 :rgbww[6];
		iic_buff[0] = GET_16BIT_HIGH8BIT(rgbww[6]);
		iic_buff[1] = GET_16BIT_LOW8BIT(rgbww[6]);
		hal_iic_send_data(HAL_IIC_ANALOG_CW1_CW2_R_G,(uint8_t)IIC_G_ADDRESS,iic_buff,2); 
		#endif
	}
	else
	{
		cmd_body_1.header.cmd_type = RS485_Cmd_Factory_RGBWW;
		cmd_body_1.cmd_arg.factory_rgbww.red = rgbww[0]; 
		cmd_body_1.cmd_arg.factory_rgbww.green = rgbww[1];  
		cmd_body_1.cmd_arg.factory_rgbww.blue =rgbww[2];  
		cmd_body_1.cmd_arg.factory_rgbww.ww = rgbww[3];    
		cmd_body_1.cmd_arg.factory_rgbww.cw = rgbww[4];
		cmd_body_1.cmd_arg.factory_rgbww.ww2 = rgbww[5];    
		cmd_body_1.cmd_arg.factory_rgbww.cw2 = rgbww[6];
		rs485_light_control(&dev_lamp, true, 5, 100, &cmd_body_1);
	
	}
    return 0;
}

bool rs485_rgbww_data_send(uint16_t *pwm_data, uint8_t *state)
{
    static rs485_cmd_body_t cmd_body_1;
	
	cmd_body_1.header.cmd_type = RS485_Cmd_Factory_RGBWW_Crc;
	cmd_body_1.cmd_arg.factory_rgbww.red   = pwm_data[0]; 
	cmd_body_1.cmd_arg.factory_rgbww.green = pwm_data[1];  
	cmd_body_1.cmd_arg.factory_rgbww.blue  = pwm_data[2];  
	cmd_body_1.cmd_arg.factory_rgbww.ww    = pwm_data[3];    
	cmd_body_1.cmd_arg.factory_rgbww.cw    = pwm_data[4];
	cmd_body_1.cmd_arg.factory_rgbww.ww2   = pwm_data[5];    
	cmd_body_1.cmd_arg.factory_rgbww.cw2   = pwm_data[6];
	memset(cmd_body_1.cmd_arg.factory_rgbww_crc_t.state, 0, sizeof(cmd_body_1.cmd_arg.factory_rgbww_crc_t.state));
	memcpy(cmd_body_1.cmd_arg.factory_rgbww_crc_t.state, state, sizeof(cmd_body_1.cmd_arg.factory_rgbww_crc_t.state));
	rs485_light_control(&dev_lamp, true, 5, 100, &cmd_body_1);
	
    return 0;
}

bool rs485_pump_data_test(void)
{
    
    static rs485_cmd_body_t cmd_body_1;
    if( dev_lamp.Connected == 0 || ui_test_lamp_pump_mode_get() == 0)
    {
        return  0;
    }
    cmd_body_1.header.cmd_type = RS485_Cmd_Pump;
    cmd_body_1.cmd_arg.pump_t.mode = 0; 
    cmd_body_1.cmd_arg.pump_t.speed = ui_test_lamp_pump_speed_get();  
    rs485_light_control(&dev_lamp, true, 5, 100, &cmd_body_1);
    return 0;
}

bool rs485_power_set_test(void)
{
    static rs485_cmd_body_t cmd_body_1;
	uint8_t output_mode;
	
	if(ui_test_power_mode_get() == 2)
	{
		ui_test_power_mode_set(0);
		data_center_read_config_data(SYS_CONFIG_OUTPUT_MODE, &output_mode);
		cmd_body_1.header.cmd_type = RS485_Cmd_IlluminationMode;
		cmd_body_1.cmd_arg.illumination_mode.mode = output_mode; 
		cmd_body_1.cmd_arg.illumination_mode.ratio = 10000;  
		rs485_light_control(&dev_lamp, true, 5, 100, &cmd_body_1);
	}
    if( dev_lamp.Connected == 0 || ui_test_power_mode_get() == 0)
    {
        return  0;
    }
	ui_test_power_mode_set(0);
	data_center_read_config_data(SYS_CONFIG_OUTPUT_MODE, &output_mode);
    cmd_body_1.header.cmd_type = RS485_Cmd_IlluminationMode;
    cmd_body_1.cmd_arg.illumination_mode.mode = output_mode; 
	if(ui_test_power_value_get() == 1)
		cmd_body_1.cmd_arg.illumination_mode.ratio = 10769;  
	else
 		cmd_body_1.cmd_arg.illumination_mode.ratio = 10000;  
    rs485_light_control(&dev_lamp, true, 5, 100, &cmd_body_1);
    return 0;
}

bool rs485_run_time_set_test(void)
{
    static rs485_cmd_body_t cmd_body_1;
	uint8_t output_mode;
	
    if( dev_lamp.Connected == 0 || ui_drv_clear_time_flag_get() == 0 || PAGE_TEST != screen_get_act_pid())
    {
        return  0;
    }
	ui_drv_clear_time_flag_set(0);
    cmd_body_1.header.cmd_type = RS485_Cmd_Clear_Run_Time;
    cmd_body_1.cmd_arg.clear_run_time_t.clear_run_time_flag = 1;
    rs485_light_control(&dev_lamp, true, 5, 100, &cmd_body_1);
    return 0;
}

void rs485_get_local_info_test(void)
{
	if( dev_lamp.Connected == 0 || PAGE_TEST != screen_get_act_pid() || 11 != screen_get_act_spid())
    {
        return;
    }
	data_center_write_sys_info(SYS_INFO_LAMP, &s_info_read_body);	
	
	uint32_t body_size = 0;
	
    memset(read_buff, 0, sizeof(read_buff));
    rs485_cmd_body_t* p_read_body = (rs485_cmd_body_t*)read_buff;
	rs485_cmd_body_t cmd_body_temp = {.header.cmd_type = RS485_Cmd_Temperature_Msg, .cmd_arg.temperature_msg.type = 6,   }; //电动菲涅尔2温度
	
    if(0 == dev_rs485_cmd_read(&dev_lamp.rs485, 5, 100, true, p_read_body, &cmd_body_temp, DEV_RS485_VA_ARG_END))
    {
        s_info_read_body.fre2_temp = (uint16_t)p_read_body->cmd_arg.temperature_msg.value;
    }
	g_ptUIStateData->test_data.zoom1_Temp = s_info_read_body.fre_temp;
	g_ptUIStateData->test_data.zoom2_Temp = s_info_read_body.fre2_temp;
}

uint8_t rs485_motor_state_test(void)
{
    rs485_cmd_body_t cmd_body_1;
	static uint8_t test_number[3] = {0};
	
    if( dev_lamp.Connected == 0)
    {
        return 0;
    }
	if((PAGE_TEST != screen_get_act_pid()) && (7 != screen_get_act_spid()))
	{
		return 0;
	}
	if(1 == ui_test_motor_test_get())
	{
		ui_test_motor_test_set(0);
		cmd_body_1.header.cmd_type = RS485_Cmd_Motor;
		cmd_body_1.cmd_arg.motor_t.mode_arg.motor_mode = RS485_Motor_Mode_Crc; 
		if(test_number[0] != g_ptUIStateData->test_data.motor_test[0])
		{
			if(g_ptUIStateData->test_data.motor_test[0] == 1)
			{
				cmd_body_1.cmd_arg.motor_t.type = RS485_Pitch_Motor; 
				cmd_body_1.cmd_arg.motor_t.mode_arg.motor_arg.motor_crc_arg.crc_state = 1;
				cmd_body_1.cmd_arg.motor_t.mode_arg.motor_arg.motor_crc_arg.crc_value = 1;
			}
			else
			{
				cmd_body_1.cmd_arg.motor_t.type = RS485_Pitch_Motor; 
				cmd_body_1.cmd_arg.motor_t.mode_arg.motor_arg.motor_crc_arg.crc_state = 1;
				cmd_body_1.cmd_arg.motor_t.mode_arg.motor_arg.motor_crc_arg.crc_value = 2;
			}
			test_number[0] = g_ptUIStateData->test_data.motor_test[0];
			rs485_light_control(&dev_lamp, true, 5, 200, &cmd_body_1);
		}
		
		if(test_number[2] != g_ptUIStateData->test_data.motor_test[2])
		{
			if(g_ptUIStateData->test_data.motor_test[2] == 1)
			{
				cmd_body_1.cmd_arg.motor_t.type = RS485_Fresnel_Motor; 
				cmd_body_1.cmd_arg.motor_t.mode_arg.motor_arg.motor_crc_arg.crc_state = 1;
				cmd_body_1.cmd_arg.motor_t.mode_arg.motor_arg.motor_crc_arg.crc_value = 1;
			}
			else
			{
				cmd_body_1.cmd_arg.motor_t.type = RS485_Fresnel_Motor; 
				cmd_body_1.cmd_arg.motor_t.mode_arg.motor_arg.motor_crc_arg.crc_state = 1;
				cmd_body_1.cmd_arg.motor_t.mode_arg.motor_arg.motor_crc_arg.crc_value = 2;
			}
			test_number[2] = g_ptUIStateData->test_data.motor_test[2];
			rs485_light_control(&dev_lamp, true, 5, 200, &cmd_body_1);
		}
		
		if(test_number[1] != g_ptUIStateData->test_data.motor_test[1])
		{
			if(g_ptUIStateData->test_data.motor_test[1] == 1)
			{
				cmd_body_1.cmd_arg.motor_t.type = RS485_Rotale_Motor; 
				cmd_body_1.cmd_arg.motor_t.mode_arg.motor_arg.motor_crc_arg.crc_state = 1;
				cmd_body_1.cmd_arg.motor_t.mode_arg.motor_arg.motor_crc_arg.crc_value = 1;
			}
			else
			{
				cmd_body_1.cmd_arg.motor_t.type = RS485_Rotale_Motor; 
				cmd_body_1.cmd_arg.motor_t.mode_arg.motor_arg.motor_crc_arg.crc_state = 1;
				cmd_body_1.cmd_arg.motor_t.mode_arg.motor_arg.motor_crc_arg.crc_value = 2;
			}
			test_number[1] = g_ptUIStateData->test_data.motor_test[1];
			rs485_light_control(&dev_lamp, true, 5, 200, &cmd_body_1);
		}
	}
    return 0;
}

uint8_t rs485_motor_demo_test(void)
{
    rs485_cmd_body_t cmd_body_1;
	static uint8_t test_number[3] = {0};
	
    if( dev_lamp.Connected == 0)
    {
        return 0;
    }
	if((PAGE_TEST != screen_get_act_pid()) && (7 != screen_get_act_spid()))
	{
		return 0;
	}
	if(1 == ui_test_motor_test_get())
	{
		ui_test_motor_test_set(0);
		cmd_body_1.header.cmd_type = RS485_Cmd_Motor;
		cmd_body_1.cmd_arg.motor_t.mode_arg.motor_mode = RS485_Motor_Mode_Crc; 
		if(test_number[0] != g_ptUIStateData->test_data.motor_test[0])
		{
			if(g_ptUIStateData->test_data.motor_test[0] == 1)
			{
				cmd_body_1.cmd_arg.motor_t.type = RS485_Pitch_Motor; 
				cmd_body_1.cmd_arg.motor_t.mode_arg.motor_arg.motor_crc_arg.crc_state = 1;
				cmd_body_1.cmd_arg.motor_t.mode_arg.motor_arg.motor_crc_arg.crc_value = 1;
			}
			else
			{
				cmd_body_1.cmd_arg.motor_t.type = RS485_Pitch_Motor; 
				cmd_body_1.cmd_arg.motor_t.mode_arg.motor_arg.motor_crc_arg.crc_state = 1;
				cmd_body_1.cmd_arg.motor_t.mode_arg.motor_arg.motor_crc_arg.crc_value = 2;
			}
			test_number[0] = g_ptUIStateData->test_data.motor_test[0];
			rs485_light_control(&dev_lamp, true, 5, 200, &cmd_body_1);
		}
		
		if(test_number[2] != g_ptUIStateData->test_data.motor_test[2])
		{
			if(g_ptUIStateData->test_data.motor_test[2] == 1)
			{
				cmd_body_1.cmd_arg.motor_t.type = RS485_Fresnel_Motor; 
				cmd_body_1.cmd_arg.motor_t.mode_arg.motor_arg.motor_crc_arg.crc_state = 1;
				cmd_body_1.cmd_arg.motor_t.mode_arg.motor_arg.motor_crc_arg.crc_value = 1;
			}
			else
			{
				cmd_body_1.cmd_arg.motor_t.type = RS485_Fresnel_Motor; 
				cmd_body_1.cmd_arg.motor_t.mode_arg.motor_arg.motor_crc_arg.crc_state = 1;
				cmd_body_1.cmd_arg.motor_t.mode_arg.motor_arg.motor_crc_arg.crc_value = 2;
			}
			test_number[2] = g_ptUIStateData->test_data.motor_test[2];
			rs485_light_control(&dev_lamp, true, 5, 200, &cmd_body_1);
		}
		
		if(test_number[1] != g_ptUIStateData->test_data.motor_test[1])
		{
			if(g_ptUIStateData->test_data.motor_test[1] == 1)
			{
				cmd_body_1.cmd_arg.motor_t.type = RS485_Rotale_Motor; 
				cmd_body_1.cmd_arg.motor_t.mode_arg.motor_arg.motor_crc_arg.crc_state = 1;
				cmd_body_1.cmd_arg.motor_t.mode_arg.motor_arg.motor_crc_arg.crc_value = 1;
			}
			else
			{
				cmd_body_1.cmd_arg.motor_t.type = RS485_Rotale_Motor; 
				cmd_body_1.cmd_arg.motor_t.mode_arg.motor_arg.motor_crc_arg.crc_state = 1;
				cmd_body_1.cmd_arg.motor_t.mode_arg.motor_arg.motor_crc_arg.crc_value = 2;
			}
			test_number[1] = g_ptUIStateData->test_data.motor_test[1];
			rs485_light_control(&dev_lamp, true, 5, 200, &cmd_body_1);
		}
	}
    return 0;
}


bool rs485_base_data_output(void)
{
    bool res = false;
    rs485_dac_dim_t  dac_body;
    sys_config_t cofig_pack;
    static rs485_cmd_body_t cmd_body_1;
    data_center_read_config_data(SYS_CONFIG_HIGH_SPEED_MODE, &cofig_pack.hs_mode);
    if( dev_lamp.Connected == 0)
    {
        return  0;
    }
    if( ui_test_Industrial_mode_get() == 0 )
    {
        return  0;
    }
    if(debug_base_data_collect_cb(&cmd_body_1) == 0)
    {
        return 0;
    }
	if((uint8_t)cmd_body_1.header.cmd_type == RS485_Cmd_user_custom)
	{
		uint8_t hs_mode;
		if(cmd_body_1.cmd_arg.user_custom_t.type)
			hs_mode = 1;
		else 
			hs_mode = 0;
		data_center_write_config_data_no_event(SYS_CONFIG_HIGH_SPEED_MODE, &hs_mode);
		ui_set_hs_mode_state(0);
		rs485_hs_mode_switch(cmd_body_1.cmd_arg.user_custom_t.type);
		base_data_general_ack_pack();
	}
    if(cofig_pack.hs_mode == 1)
    {
        switch((uint8_t)cmd_body_1.header.cmd_type)
        {
            case RS485_Cmd_CCT:
                if(rs485_light_control(&dev_lamp, true, 5, 100, &cmd_body_1))
                {
                    res = 1 ;
                    osSemaphoreRelease(analog_dim_signal); 
                    rs485_analog_dac_data_get(&dev_lamp);
					base_data_general_ack_pack();
                }
                else
                {
                    return 0;
                }
                break;
            case RS485_Cmd_Factory_RGBWW:
//                dac_body.r = cmd_body_1.cmd_arg.factory_rgbww.red; 
//                dac_body.g = cmd_body_1.cmd_arg.factory_rgbww.green;  
//                dac_body.b = cmd_body_1.cmd_arg.factory_rgbww.blue;  
//                dac_body.cw = cmd_body_1.cmd_arg.factory_rgbww.cw;    
//                dac_body.ww = cmd_body_1.cmd_arg.factory_rgbww.ww;
				#if PROJECT_TYPE == 307
				dac_body.r = cmd_body_1.cmd_arg.factory_rgbww.red; 
                dac_body.g = cmd_body_1.cmd_arg.factory_rgbww.green;  
                dac_body.b = cmd_body_1.cmd_arg.factory_rgbww.blue;  
                dac_body.cw = cmd_body_1.cmd_arg.factory_rgbww.cw;    
                dac_body.ww = cmd_body_1.cmd_arg.factory_rgbww.ww;
                if((cmd_body_1.cmd_arg.factory_rgbww.green >= dac_fade.analog_dac_init.g)&& (cmd_body_1.cmd_arg.factory_rgbww.green != 0))    
                {
					
                     dev_analog_green_ctrl(true);
                }
                else
                {
					dev_analog_green_ctrl(false);
                }  	
				#elif PROJECT_TYPE == 308
//				dac_body.r = cmd_body_1.cmd_arg.factory_rgbww.red; 
                dac_body.g = cmd_body_1.cmd_arg.factory_rgbww.green;  
//                dac_body.b = cmd_body_1.cmd_arg.factory_rgbww.blue;  
                dac_body.cw = cmd_body_1.cmd_arg.factory_rgbww.blue;    
                dac_body.ww = cmd_body_1.cmd_arg.factory_rgbww.red;
				#endif	
                analog_dim_output(&dac_body);  
				base_data_general_ack_pack();
                break;
            default :break;
        }
    }
    else
    {
        if(rs485_light_control(&dev_lamp, true, 5, 100, &cmd_body_1))
        {
            res = 1 ;
            base_data_general_ack_pack();
        }
        else
        {
            return 0;
        }
    } 

    return res;
 }
 
void rs485_heart_beat(void)
{
	static uint32_t heart_time;
	static uint8_t beat_step = 0;
	static uint8_t last_unconnet_flag = 0;
	static uint8_t delay_count[2][2] = {0};
	sys_error_type error_code;
	
	switch(beat_step)
	{
		case 0:
			heart_time = timer_getTick();
			beat_step = 1 ;
			break;
		case 1:
			if((timer_getTick() - heart_time ) > 200)
			{
				beat_step = 0;
				if(PAGE_UPDATE != screen_get_act_pid() || ((PAGE_UPDATE == screen_get_act_pid() && screen_get_act_spid() < SUB_ID_UPDATING)))
				{
					if(rs485_board_msg_read(&dev_lamp, true, 5, 100, &s_info_read_body) == 0)
					{
						dev_lamp.Connected = 0;
						s_info_read_body.exist = dev_lamp.Connected; 
						if(last_unconnet_flag != dev_lamp.Connected && dev_lamp.Connected == 0)
							light_unconnet_handle();
					}
					last_unconnet_flag = dev_lamp.Connected;
					if(s_info_read_body.fre_temp > 110 && s_access_info.fresnel_state == 1)
					{
						delay_count[0][1] = 0;
						delay_count[0][0]++;
						if(delay_count[0][0] > 20)
						{
							delay_count[0][0] = 0;
							data_center_read_sys_info(SYS_INFO_ERROR_TYPE, &error_code); 
							error_code.error_eh.motorized_f14_motor_temp_eh_022 = 1;
							data_center_write_sys_info(SYS_INFO_ERROR_TYPE, &error_code); 
						}
					}
					else
					{
						delay_count[0][0] = 0;
						delay_count[0][1]++;
						if(delay_count[0][1] > 20)
						{
							delay_count[0][1] = 0;
							data_center_read_sys_info(SYS_INFO_ERROR_TYPE, &error_code); 
							error_code.error_eh.motorized_f14_motor_temp_eh_022 = 0;
							data_center_write_sys_info(SYS_INFO_ERROR_TYPE, &error_code); 
						}
					}
					if(s_info_read_body.fre2_temp > 110 && s_access_info.fresnel_state == 1)
					{
						delay_count[1][1] = 0;
						delay_count[1][0]++;
						if(delay_count[1][0] > 20)
						{
							delay_count[1][0] = 0;
							data_center_read_sys_info(SYS_INFO_ERROR_TYPE, &error_code); 
							error_code.error_eh.motorized_f14_ic_temp_eh_023 = 1;
							data_center_write_sys_info(SYS_INFO_ERROR_TYPE, &error_code); 
						}
					}
					else
					{
						delay_count[1][0] = 0;
						delay_count[1][1]++;
						if(delay_count[1][1] > 20)
						{
							delay_count[1][1] = 0;
							data_center_read_sys_info(SYS_INFO_ERROR_TYPE, &error_code); 
							error_code.error_eh.motorized_f14_ic_temp_eh_023 = 0;
							data_center_write_sys_info(SYS_INFO_ERROR_TYPE, &error_code); 
						} 
					}
					data_center_write_sys_info(SYS_INFO_LAMP, &s_info_read_body);	
				}
			}
			break;
	}
}
 

void hsi_analysis(rs485_hsi_t* rs485_hsi, struct db_hsi* local_hsi)
{

    rs485_hsi->hue = local_hsi->hsi.hue * 10;
    rs485_hsi->intensity = local_hsi->lightness /1000.0f;
    rs485_hsi->sat = local_hsi->hsi.sat * 10;
    rs485_hsi->cct = local_hsi->hsi.cct;

}

uint8_t *get_light_data_pack(void)
{
    return (uint8_t*)&s_light_arg;
}

void rs485_time_ms_handle(void *argument)
{

	if(s_info_read_body.hard_ver[0] == 0x10)
		dev_adapt_pwm_1ms_back(&pwm_adapt_20k);
	else
	    dev_adapt_pwm_1ms_back(&pwm_adapt_80k);
    if(s_dmx_loss_state.dmx_loss_flag == 1)
    {
        if((screen_get_act_pid() >= PAGE_LIGHT_MODE_HSI && screen_get_act_pid() <= PAGE_HSI_LIMIT_SETTING) || screen_get_act_pid() == PAGE_SIDUS_PRO_FX)   
		{
			return;
		}
        if(s_dmx_loss_state.dmx_loss_mode == DMX_LOSS_BEHAVIOR_BO )  //????
        {
            struct db_dmx dmx;
			
            data_center_read_light_data(LIGHT_MODE_DMX, &dmx);
			if(dmx.dmx_data.type != DMX_TYPE_RGBWW)
			{
				dmx.dmx_data.lightness = 0;
			}
			else
			{
				dmx.dmx_data.lightness = 0;
				dmx.dmx_data.arg.rgb.blue = 0;
				dmx.dmx_data.arg.rgb.cw = 0;
				dmx.dmx_data.arg.rgb.green = 0;
				dmx.dmx_data.arg.rgb.red = 0;
				dmx.dmx_data.arg.rgb.ww = 0;
				dmx.dmx_data.type = DMX_TYPE_RGBWW;
			}
            s_dmx_loss_state.dmx_loss_flag = 0;
			mdx_loss_start = 1;
            data_center_write_light_data(LIGHT_MODE_DMX, &dmx);
        }
        else if(s_dmx_loss_state.dmx_loss_mode == DMX_LOSS_BEHAVIOR_FTB)   //??
        {
            struct db_dmx dmx;
        
            data_center_read_light_data(LIGHT_MODE_DMX, &dmx);
            s_dmx_loss_state.dmx_fade_ms++;
            if(s_dmx_loss_state.dmx_fade_ms < 999)
            {
                return;
            }
            s_dmx_loss_state.dmx_fade_ms = 0;
            s_dmx_loss_state.dmx_fade_s++;
            dmx.dmx_data.lightness -= s_dmx_loss_state.dmx_fade_slope;
            if(s_dmx_loss_state.dmx_fade_s >= s_dmx_loss_state.dmx_fade_time)
            {
                s_dmx_loss_state.dmx_loss_flag = 0;
                dmx.dmx_data.lightness = 0;
            }
            data_center_write_light_data(LIGHT_MODE_DMX, &dmx);
        }
        else if(s_dmx_loss_state.dmx_loss_mode == DMX_LOSS_BEHAVIOR_HFO)
        {
            s_dmx_loss_state.dmx_hold_ms++;
            if(s_dmx_loss_state.dmx_hold_ms < 999)
            {
                return;
            }
			s_dmx_loss_state.dmx_fade_s = 0;
            s_dmx_loss_state.dmx_hold_ms = 0;
            s_dmx_loss_state.dmx_hold_s++;
            if(s_dmx_loss_state.dmx_hold_s > 120)
            {
				s_dmx_loss_state.dmx_hold_s = 0;
                s_dmx_loss_state.dmx_loss_mode = DMX_LOSS_BEHAVIOR_FTB;
            }
        }
    }
	timer_IncTick();
}




void rs485_singel_power_crc_event(void)
{	
	uint8_t ack;
	#if PROJECT_TYPE==307
	uint16_t power_value[7] = {670, 670, 670, 670, 730, 670, 730};
	#elif PROJECT_TYPE==308
	uint16_t power_value[7] = {370, 680, 680, 680, 740, 740, 740};
	#endif
	uint8_t power_error = 5;   //误差
	uint8_t pwm_step = 1;   //pwm步进
	static uint16_t pwm_value[7] = {8000, 0, 0, 0, 0, 0, 0};
	static uint8_t pwm_state[7] = {0, 0, 0, 0, 0, 0, 0};
	uint16_t pwm_value_template[7] = {8000, 0, 0, 0, 0, 0, 0};
	static uint16_t s_count,s_uint;
	static uint8_t s_single_power_index = 0;
	uint8_t hs_mode = 0;
	
	if(dev_lamp.Connected ==0 ) return;
	ack = ui_get_single_power_calibration_state();
	struct sys_info_ctrl_box  ctrl_box ={0};
	data_center_read_sys_info(SYS_INFO_CTRL, &ctrl_box);
	hs_mode = 0;
	switch(ack)
	{
		case CALIBRATE_POWER_IDLE:
			break;
		case CALIBRATE_POWER_PWM_START:
			sys_box_lcd_lock(1);	
			data_center_read_config_data(SYS_CONFIG_HIGH_SPEED_MODE, &hs_mode);
			if(hs_mode)
			{
				hs_mode = 0;
				data_center_write_config_data_no_event(SYS_CONFIG_HIGH_SPEED_MODE, &hs_mode);
				ui_set_hs_mode_state(0);
				rs485_hs_mode_switch(0);
			}
			ui_set_single_power_calibration_state(CALIBRATE_POWER_ING);
			osDelay(1000);
			s_count = 0;
			s_uint = 0;
			memcpy(pwm_value, pwm_value_template, sizeof(pwm_value_template));
			memset(pwm_state, 0, 7);
			s_single_power_index = 0;
			rs485_rgbww_data_send(pwm_value, pwm_state);
			break;
		case CALIBRATE_POWER_ING:
			if(s_count++ < 5) return ;  
			s_count = 0;
			if(ctrl_box.power > (power_value[s_single_power_index] - power_error) && ctrl_box.power < (power_value[s_single_power_index] + power_error))
			{
				if(s_uint++ >  10) 
				{
					s_uint = 0;
					pwm_state[s_single_power_index] = 1;
					s_single_power_index++;
					rs485_rgbww_data_send(pwm_value, pwm_state);
					osDelay(2000);
					if(s_single_power_index == 7)
					{
						s_single_power_index = 0;
						ui_set_single_power_calibration_state(CALIBRATE_POWER_PWM_OK);
						sys_box_lcd_lock(0);
						break;
					}
					pwm_value[s_single_power_index] = 8000;
					pwm_state[s_single_power_index - 1] = 0;
					pwm_value[s_single_power_index - 1] = 0;
					rs485_rgbww_data_send(pwm_value, pwm_state);
				
					osDelay(500);
				}
			}
			else
			{
				s_uint = 0;
			}
			if(ctrl_box.power > power_value[s_single_power_index] && s_single_power_index < 7)
			{
				pwm_value[s_single_power_index] -= pwm_step;
				rs485_rgbww_data_send(pwm_value, pwm_state);
			}
			else if(ctrl_box.power < power_value[s_single_power_index]&& s_single_power_index < 7)
			{
				pwm_value[s_single_power_index] += pwm_step;
				rs485_rgbww_data_send(pwm_value, pwm_state);
			}
	
			if(pwm_value[s_single_power_index] > 9600&& s_single_power_index < 7)
			{
				pwm_value[s_single_power_index] = 9600;
				pwm_state[s_single_power_index] = 0;
				rs485_rgbww_data_send(pwm_value, pwm_state);
			}

			break;
		case CALIBRATE_POWER_PWM_OK:
			break;
		default:break; 	
	}
	osDelay(5);
}

void rs485_power_adjust_event(void)
{	
	uint8_t ack;
	#if PROJECT_TYPE==307
	static struct dev_power_adjust_t power_adjust = {
		.adjust_value = 10000,
		.pwm_body = 
		{
			.header.cmd_type = RS485_Cmd_CCT,
			.cmd_arg.cct.cct = 5800,
			.cmd_arg.cct.intensity = 1.0f,
			.cmd_arg.cct.duv = 0 ,
			.cmd_arg.cct.fade = 350,
		},
		.dim_body = 
		{
			.header.cmd_type = RS485_Cmd_CCT,
			.cmd_arg.cct.cct = 6000,
			.cmd_arg.cct.intensity = 1.0f,
			.cmd_arg.cct.duv = 0 ,
			.cmd_arg.cct.fade = 350,
		},
		.pwm_limit ={
			.upper = 2200,
			.lower = 1920
		},
		.dim_limit ={
			.upper = 2000,
			.lower = 1760
		},		
	};
	#elif PROJECT_TYPE==308
	static struct dev_power_adjust_t power_adjust = {
		.adjust_value = 10000,
		.pwm_body = 
		{
			.header.cmd_type = RS485_Cmd_CCT,
//			.cmd_arg.cct.cct = 3600,
			.cmd_arg.cct.cct = 4050,
			.cmd_arg.cct.intensity = 1.0f,
			.cmd_arg.cct.duv = 0 ,
			.cmd_arg.cct.fade = 350,
		},
		.dim_body = 
		{
			.header.cmd_type = RS485_Cmd_CCT,
			.cmd_arg.cct.cct = 5350,
			.cmd_arg.cct.intensity = 1.0f,
			.cmd_arg.cct.duv = 0 ,
			.cmd_arg.cct.fade = 350,
		},
		.pwm_limit ={
			.upper = 3140,
			.lower = 2900
		},
		.dim_limit ={
			.upper = 3140,
			.lower = 2900
		},		
	};
	#endif
	static uint8_t s_calibration_state = 0;
	static uint16_t s_count,s_uint,upper_power,lower_power;
	if(dev_lamp.Connected ==0 ) return;
	ack = ui_get_power_calibration_state();
	struct sys_info_ctrl_box  ctrl_box ={0};
	data_center_read_sys_info(SYS_INFO_CTRL, &ctrl_box);
	uint8_t hs_mode;
	switch(ack)
	{
		case CALIBRATE_POWER_IDLE:
			break;
		case CALIBRATE_POWER_PWM_START:
			sys_box_lcd_lock(1);		
			hs_mode = 0;
			data_center_write_config_data_no_event(SYS_CONFIG_HIGH_SPEED_MODE, &hs_mode);
			ui_set_hs_mode_state(0);
			rs485_hs_mode_switch(0);
			osDelay(1000);
			dev_lamp.Connected = rs485_light_control(&dev_lamp, true, 5, 100, &power_adjust.pwm_body);
			dev_lamp.Connected = rs485_set_power_mode(&dev_lamp, true, 5, 50, 1,10000 ,1);
			ui_set_power_calibration_state(CALIBRATE_POWER_ING);
			s_calibration_state = CALIBRATE_POWER_PWM_START;
			power_adjust.adjust_value = 10000;
			upper_power = power_adjust.pwm_limit.upper;
			lower_power = power_adjust.pwm_limit.lower;
			s_count = 0;
			s_uint = 0;
			break;
		case CALIBRATE_POWER_ANALOG_START:
			#if PROJECT_TYPE == 307
			if((dac_fade.analog_dac_current.g >= dac_fade.analog_dac_init.g) && dac_fade.analog_dac_current.g != 0)   
			{
				dev_analog_green_ctrl(true);
			}
			else
			{
			   dev_analog_green_ctrl(false); 
			} 
			#endif		
			hs_mode = 1;
			data_center_write_config_data_no_event(SYS_CONFIG_HIGH_SPEED_MODE, &hs_mode);
			ui_set_hs_mode_state(1);		
			rs485_hs_mode_switch(1);
			dev_lamp.Connected = rs485_light_control(&dev_lamp, true, 5, 100, &power_adjust.dim_body);
			dev_lamp.Connected = rs485_set_power_mode(&dev_lamp, true, 5, 50, 1,10000 ,1);
			ui_set_power_calibration_state(CALIBRATE_POWER_ING);
			s_calibration_state = CALIBRATE_POWER_ANALOG_START;
			osSemaphoreRelease(analog_dim_signal);
			power_adjust.adjust_value = 10000;
			upper_power = power_adjust.dim_limit.upper;
			lower_power = power_adjust.dim_limit.lower;
			s_count = 0;
			s_uint = 0;
			break;
		case CALIBRATE_POWER_ING:
			if(s_count++ < 1000) return ;  
			if(ctrl_box.power  > upper_power)
			{
				power_adjust.adjust_value = power_adjust.adjust_value  - 5;
				dev_lamp.Connected = rs485_set_power_mode(&dev_lamp, true, 5, 50, 1,power_adjust.adjust_value , 0 );	
				if(s_calibration_state == CALIBRATE_POWER_ANALOG_START)
				{
					osSemaphoreRelease(analog_dim_signal);
				}
			}
			else if(ctrl_box.power > lower_power )
			{
				s_uint = 0;
				power_adjust.adjust_value = power_adjust.adjust_value  - 1;
				dev_lamp.Connected = rs485_set_power_mode(&dev_lamp, true, 5, 50, 1,power_adjust.adjust_value , 0 );	
				if(s_calibration_state == CALIBRATE_POWER_ANALOG_START)
				{
					osSemaphoreRelease(analog_dim_signal);
				}
			}
			else 
			{
				if(s_uint++ > 10)
				{
					dev_lamp.Connected = rs485_set_power_mode(&dev_lamp, true, 5, 50, 1,power_adjust.adjust_value ,1 );
					osDelay(20);
					dev_lamp.Connected = rs485_set_power_mode(&dev_lamp, true, 5, 50, 1,10000 ,0 );		
					if(s_calibration_state == CALIBRATE_POWER_PWM_START)
					{
						ui_set_power_calibration_state(CALIBRATE_POWER_PWM_OK);
					}
					else
					{
						ui_set_power_calibration_state(CALIBRATE_POWER_ANALOG_OK);
						sys_box_lcd_lock(0);
					}
				}
			}
			break;
		case CALIBRATE_POWER_PWM_OK:
			ui_set_power_calibration_state(CALIBRATE_POWER_ANALOG_START);
			osDelay(1000);
			break;
		case CALIBRATE_POWER_ANALOG_OK:

			break;
		default:break; 	
		}
}



uint16_t get_analog_min_brightness(uint8_t mode, light_boby_t *light_boby)
{
	data_center_read_light_data(mode, light_boby);
	uint16_t brightness;
	switch(mode)
	{
		case LIGHT_MODE_CCT:
			light_boby->cct_arg.lightness = 200;
			brightness = 200;
			light_boby->cct_arg.lightness = 200;
			break;
		case LIGHT_MODE_HSI:
			brightness = 200;
			light_boby->hsi_arg.lightness = 200;
			break;
		case LIGHT_MODE_GEL:
			brightness = 200;
			light_boby->gel_arg.lightness = 200;
			break;
		case LIGHT_MODE_RGB:
			brightness = 200;
			light_boby->rgb_arg.lightness = 200;
			break;
		case LIGHT_MODE_SOURCE:
			brightness = 200;
			light_boby->source_arg.lightness = 200;
			break;
		case LIGHT_MODE_XY:
			brightness = 200;
			 light_boby->xy_arg.lightness = 200;
			break;
		default :
			brightness = 200;
			light_boby->cct_arg.lightness = 200;
		break;
	
	}
	return  brightness;
}

void rs485_hs_mode_switch(uint8_t type)
{
	bool ack = true;
	uint8_t hs_mode;	
	static  light_boby_t light_boby = {0};
	rs485_cmd_body_t cmd_body_1={0};
	uint8_t mode = date_center_get_light_mode();
	
	switch(type)
	{
		case 0:
		{
			if(dev_lamp.hs_state == 1) return ;	
			analog_dim_fade_stop();
			dev_power.power_boost_up(0);
			dev_power.analog_dim_ctrl(0);
			osDelay(100);
			if(s_info_read_body.hard_ver[0] == 0x10)
				dev_adapt_pwm_limit_set(&pwm_adapt_20k,20);
			else
				dev_adapt_pwm_limit_set(&pwm_adapt_80k,20);
			ack = rs485_set_highspeed_mode(&dev_lamp,  true, 5, 100, 0);
			app_power_iic_send_all_data(1);
			/*********************/
			osDelay(150);
			/*********************/
			dev_power.pwm_dim_ctrl(1);	
			dev_power.power_boost_up(1);
			dev_lamp.hs_state = 1;
			osDelay(100);
		}
		break;
		case 1:
		{
			if(dev_lamp.hs_state == 2) return ;
			osTimerStop(light_timer_fade_id);
//			ack =  rs485_set_sleep_mode(&dev_lamp, true, 5, 100, 0);
			light_boby.cct_arg.lightness = 0;
			package_485_light_data(&cmd_body_1, mode, &light_boby);
			cmd_body_1.cmd_arg.cct.fade = 0;
			dev_lamp.Connected = rs485_light_control(&dev_lamp, true, 5, 100, &cmd_body_1);
			dev_power.pwm_dim_ctrl(0);
			app_power_iic_send_all_data(0);
			osDelay(3000);
			ack =  rs485_set_highspeed_mode(&dev_lamp,  true, 5, 100, 1);
			osDelay(100);
			uint16_t brightness = get_analog_min_brightness(mode,&light_boby);
			package_485_light_data(&cmd_body_1, mode, &light_boby);
			dev_lamp.Connected = rs485_light_control(&dev_lamp, true, 5, 100, &cmd_body_1);
			osSemaphoreRelease(analog_dim_signal);
			osMutexRelease(MutexID_Rs485);
			rs485_analog_dac_data_get(&dev_lamp);
			analog_dim_fade_stop();
			data_center_read_light_data(mode, &light_boby);	
			uint16_t * s_int = &light_boby.cct_arg.lightness;
			memcpy(&dac_fade.analog_dac_current, &dac_fade.analog_dac_end, sizeof(dac_fade.analog_dac_end));
			if(*s_int > brightness )
			{
				data_center_read_light_data(mode, &light_boby);
				package_485_light_data(&cmd_body_1, mode, &light_boby);
				dev_lamp.Connected = rs485_light_control(&dev_lamp, true, 5, 100, &cmd_body_1);
				osSemaphoreRelease(analog_dim_signal);
				osMutexRelease(MutexID_Rs485);
				rs485_analog_dac_data_get(&dev_lamp);					
			}
			else
			{
				light_boby.cct_arg.lightness = brightness;
				g_tUIAllData.cct_model.lightness = light_boby.cct_arg.lightness;
				if(screen_get_act_pid() >= PAGE_LIGHT_MODE_HSI && screen_get_act_pid() <= PAGE_LIGHT_MODE_CCT)
					page_set_invalid_type(PAGE_INVALID_TYPE_ALL);
				data_center_write_light_data_no_event(mode, &light_boby);
			}
			analog_dim_fade_stop();
//			ack =  rs485_set_sleep_mode(&dev_lamp, true, 5, 100, 1);
			data_center_read_config_data(SYS_CONFIG_HIGH_SPEED_MODE, &hs_mode);
			if(hs_mode == 1)
			{
				if(ui_error_code_event_get())
				{
					dev_power.analog_dim_ctrl(0);
				}
				else
				{
					dev_power.analog_dim_ctrl(1);
					analog_dim_fade_restart();
				}
				hal_self_adapt_all_pwm_set(2000);
			}	
			dev_lamp.hs_state = 2;
			osTimerStart(light_timer_fade_id, 1);
			osDelay(300);
		}
		break;
		case 2:
		{
			uint16_t brightness = get_analog_min_brightness(mode,&light_boby);
			package_485_light_data(&cmd_body_1, mode, &light_boby);
			dev_lamp.Connected = rs485_light_control(&dev_lamp, true, 5, 100, &cmd_body_1);
			osSemaphoreRelease(analog_dim_signal);
			osMutexRelease(MutexID_Rs485);
			rs485_analog_dac_data_get(&dev_lamp);
			analog_dim_fade_stop();
			data_center_read_light_data(mode, &light_boby);	
			uint16_t * s_int = &light_boby.cct_arg.lightness;
			memcpy(&dac_fade.analog_dac_current, &dac_fade.analog_dac_end, sizeof(dac_fade.analog_dac_end));
			if(* s_int > brightness )
			{
				data_center_read_light_data(mode, &light_boby);
				package_485_light_data(&cmd_body_1, mode, &light_boby);
				dev_lamp.Connected = rs485_light_control(&dev_lamp, true, 5, 100, &cmd_body_1);
				osSemaphoreRelease(analog_dim_signal);
				osMutexRelease(MutexID_Rs485);
				rs485_analog_dac_data_get(&dev_lamp);					
			}
			else
			{
				data_center_write_light_data_no_event(mode, &light_boby);
			}
			analog_dim_fade_stop();
			data_center_read_config_data(SYS_CONFIG_HIGH_SPEED_MODE, &hs_mode);
			if(hs_mode == 1)
			{
				if(ui_error_code_event_get())
				{
					dev_power.analog_dim_ctrl(0);
				}
				else
				{
					dev_power.analog_dim_ctrl(1);
					analog_dim_fade_restart();
			
				}
				hal_self_adapt_all_pwm_set(2000);
			}	
			osTimerStart(light_timer_fade_id, 1);	
			break;
		}
		default :
			dev_lamp.hs_state = 0;
		break;
	}
}

uint8_t light_rs485_self_crtl(void)
{
    static uint8_t reconnected_sta = 0;
	static uint8_t york_delay = 0;
    sys_config_t cofig_pack;
    uint8_t res = 0;
	uint8_t ack = 0;
    if(osOK != osMutexAcquire(MutexID_Rs485, 0))
    {
        return 0;
    }
    
    if(dev_lamp.Connected)
    {
        switch(reconnected_sta)
        {
            case 0:
            if (screen_get_act_pid() == PAGE_TEST || screen_get_act_pid() == PAGE_CALIBRATION_MODE)
            {
                if(rs485_set_fan_mode(&dev_lamp,  true, 5, 100, 0, ui_test_lamp_fan_get()) != 0)   
                {
                    reconnected_sta ++;  
                    goto out;  
                }
           }
           else{
               reconnected_sta ++ ;  
               goto out; 
           }
           break;
           case 1:
                data_center_read_config_data(SYS_CONFIG_HIGH_SPEED_MODE, &cofig_pack.hs_mode);
                if(cofig_pack.hs_mode == 1)
                {
                    reconnected_sta++;
                    goto out;
                }
				else 
				{
					ack = rs485_self_adaption_read(&dev_lamp);
					if(ack == 1)
					{
						reconnected_sta++;
						goto out;
					}
					else if(ack == 2)
					{
						rs485_set_sleep_mode(&dev_lamp, true, 5, 100, 0);
						reconnected_sta++;
						goto out;
					}
					else
					{
						reconnected_sta = 0;
					}
				}
            break;
            case 2:
            {
				int16_t tile11[2] = {0};
				static uint8_t read_ver_flag[2] = {0};
				reconnected_sta++;
				if(york_delay++ < 1)
				{
					goto out;
				}
				york_delay = 0;
                if(0 != rs485_get_access(&dev_lamp,  true, 5, 100, &s_access_info))
                {
					if(s_access_info.fresnel_state == 1 || s_access_info.optical_20_access_state == 1)
					{
						uint8_t tiltm;
						data_center_read_config_data(SYS_CONFIG_ANGLE_LIMIT, &tiltm);
						data_center_read_config_data(SYS_CONFIG_YOKE_TILT_LIMIT, tile11);
						if(tiltm == 1)
						{
							if(tile11[0] <= -120)
								tile11[0] = -120;
							if(tile11[1] >= 120)
								tile11[1] = 120;
						}
						data_center_write_config_data_no_event(SYS_CONFIG_YOKE_TILT_LIMIT, tile11);
					}
					if(s_access_info.yoke_state == 1 && read_ver_flag[0] == 0)
					{
						read_ver_flag[0] = 1;
						for(uint8_t i = 0; i<10; i++)
							rs485_get_device_version2(&dev_lamp, true, 5, 200, &s_info_read_body);
						data_center_write_sys_info(SYS_INFO_LAMP, &s_info_read_body);
						data_center_read_sys_info(SYS_INFO_MOTOR, &s_light_motor_info);
						rs485_get_motor_angle(&dev_lamp, true, 5, 500, RS485_Pitch_Motor, &s_light_motor_info.tilt_angle);
						rs485_get_motor_angle(&dev_lamp, true, 5, 500, RS485_Rotale_Motor, &s_light_motor_info.pan_angle);
						data_center_write_sys_info(SYS_INFO_MOTOR, &s_light_motor_info);
					}
					else if(s_access_info.yoke_state == 0)
					{
						read_ver_flag[0] = 0;
						s_info_read_body.motorized_yoke_soft_ver[0] = 0;
						data_center_write_sys_info(SYS_INFO_LAMP, &s_info_read_body);
					}
					if(s_access_info.fresnel_state == 1 && read_ver_flag[1] == 0)
					{
						read_ver_flag[1] = 1;
						for(uint8_t i = 0; i<10; i++)
							rs485_get_device_version2(&dev_lamp, true, 5, 200, &s_info_read_body);
						data_center_write_sys_info(SYS_INFO_LAMP, &s_info_read_body);
						data_center_read_sys_info(SYS_INFO_MOTOR, &s_light_motor_info);
						rs485_get_motor_angle(&dev_lamp, true, 5, 500, RS485_Fresnel_Motor, &s_light_motor_info.fresnel_angle);
						data_center_write_sys_info(SYS_INFO_MOTOR, &s_light_motor_info);
					}
					else if(s_access_info.fresnel_state == 0)
					{
						read_ver_flag[1] = 0;
						s_info_read_body.motorized_fresnel_soft_ver[0] = 0;
						data_center_write_sys_info(SYS_INFO_LAMP, &s_info_read_body);
					}
					data_center_write_sys_info(SYS_INFO_ACCESS, &s_access_info);
					goto out;
                }
				else
				{
					reconnected_sta = 0;
					memset(&s_access_info,0,sizeof(s_access_info));
					data_center_write_sys_info(SYS_INFO_ACCESS, &s_access_info);
				}
			}
            break;
			case 3:
			{
				uint8_t error[6] = {0};
				sys_error_type err_msg ={0};
				data_center_read_sys_info(SYS_INFO_ERROR_TYPE, &err_msg); 
				if(err_msg.error_eh.motorized_f14_Fresnel_018 == 1)
					error[0] = 1;
				if(err_msg.error_eh.motorized_yoke_tilting_020 == 1)
					error[1] = 1;
				if(err_msg.error_eh.motorized_yoke_panning_019 == 1)
					error[2] = 1;
				if(err_msg.error_eh.motorized_f14_ambient_temp_eh_021 == 1)
					error[3] = 1;
				if(err_msg.error_eh.motorized_f14_motor_temp_eh_022 == 1)
					error[4] = 1;
				if(err_msg.error_eh.motorized_f14_ic_temp_eh_023 == 1)
					error[5] = 1;
				if(0 != rs485_read_err_msg(&dev_lamp,(uint32_t *)&err_msg.error_eh)){
					reconnected_sta++;
					if(error[0] == 1)
						err_msg.error_eh.motorized_f14_Fresnel_018 = 1;
					if(error[1] == 1)
						err_msg.error_eh.motorized_yoke_tilting_020 = 1;
					if(error[2] == 1)
						err_msg.error_eh.motorized_yoke_panning_019 = 1;
					if(error[3] == 1)
						err_msg.error_eh.motorized_f14_ambient_temp_eh_021 = 1;
					if(error[4] == 1)
						err_msg.error_eh.motorized_f14_motor_temp_eh_022 = 1;
					if(error[5] == 1)
						err_msg.error_eh.motorized_f14_ic_temp_eh_023 = 1;
					data_center_write_sys_info(SYS_INFO_ERROR_TYPE, &err_msg); 
					goto out;
				}
                else
                {
                    reconnected_sta = 0;
                }
			}
			break;
            case 4:
			{
				sys_error_type err_msg ={0};
                res = 1; 
				static uint8_t fresnel_count = 0;
				
               	reconnected_sta++;
				data_center_read_sys_info(SYS_INFO_ERROR_TYPE, &err_msg); 
                data_center_read_sys_info(SYS_INFO_MOTOR_STATE, &motor_state);
				if(s_access_info.fresnel_state == 1 || s_access_info.yoke_state == 1)
				{
					rs485_get_motor_stall(&dev_lamp, true, 5, 100, (uint8_t *)&motor_state);//获取堵转状态
					if((s_access_info.fresnel_state == 1) && (0 == ui_fresnel_get_angle_flag_get()))
					{
						if(screen_get_act_pid() == PAGE_ELECT_ACCESSORY )
						{
							if(screen_get_act_spid() != SUB_ID_ELE_YOKE && screen_get_act_spid() != SUB_ID_ELE_APTURE_NO_YOKE && screen_get_act_spid() != SUB_ID_ELE_NO_YOKE) 
							{
								data_center_read_sys_info(SYS_INFO_MOTOR, &s_light_motor_info);
								rs485_get_motor_angle(&dev_lamp, true, 5, 500, RS485_Fresnel_Motor, &s_light_motor_info.fresnel_angle);
								data_center_write_sys_info(SYS_INFO_MOTOR, &s_light_motor_info);
							}
						}
					}
					if(motor_state.fresnel_stall_state == 1)
						err_msg.error_eh.motorized_f14_Fresnel_018 = 1;
					else
						err_msg.error_eh.motorized_f14_Fresnel_018 = 0;
					if(motor_state.tilt_stall_state == 1)
						err_msg.error_eh.motorized_yoke_tilting_020 = 1;
					else
						err_msg.error_eh.motorized_yoke_tilting_020 = 0;
					if(motor_state.pan_stall_state == 1)
						err_msg.error_eh.motorized_yoke_panning_019 = 1;
					else
						err_msg.error_eh.motorized_yoke_panning_019 = 0;
					data_center_write_sys_info(SYS_INFO_ERROR_TYPE, &err_msg);
				}
                data_center_write_sys_info(SYS_INFO_MOTOR_STATE, &motor_state);
                goto out;
            }
            break;
			case 5:
				reconnected_sta = 0;
				if(PAGE_PRODUCT_INFO == screen_get_act_pid())
				{
					if(s_access_info.yoke_state == 1)
					{
						rs485_get_motor_angle(&dev_lamp, true, 5, 200, 3,&s_light_motor_info.roll_angle);
						rs485_get_motor_angle(&dev_lamp, true, 5, 200, RS485_Pitch_Motor, &s_light_motor_info.tilt_angle);
					}
					else
					{
						s_light_motor_info.pan_angle = 0;
						rs485_get_motor_angle(&dev_lamp, true, 5, 500, 3, &s_light_motor_info.roll_angle);
						rs485_get_motor_angle(&dev_lamp, true, 5, 200, 4, &s_light_motor_info.tilt_angle);
					}
					data_center_write_sys_info(SYS_INFO_MOTOR, &s_light_motor_info);
				}
				goto out;
			break;
			default:break;
        }
    }
    
    dev_lamp.Connected = 0;
    osMutexRelease(MutexID_Rs485); 
    return res = 0;
    out :
        osMutexRelease(MutexID_Rs485);
        return res;
    
}

float test_pan_angle[512] = {0.0};

bool light_rs485_write_config(uint8_t mode)
{
    bool ack = true;
    sys_config_t cofig_pack;
    sys_info_t  info_pack;
	struct sys_info_power info_power;
    uint16_t fan_speed = 0;
	float  pan_value = 0;
	float  tilei_value = 0;
	uint8_t  hs_mode = 0;
	struct sys_info_dmx          dmx_link_state;
	
    switch(mode)
    {
        case SYS_CONFIG_FAN_MODE: 
			data_center_read_config_data(SYS_CONFIG_HIGH_SPEED_MODE, &hs_mode);
            data_center_read_config_data(SYS_CONFIG_FAN_MODE, &cofig_pack.fan_mode);
            data_center_read_sys_info(SYS_INFO_CTRL,&info_pack.ctrl_box);
            ack =  rs485_set_fan_mode(&dev_lamp,  true, 5, 200, cofig_pack.fan_mode+1, fan_speed);
			if(hs_mode == 1)osSemaphoreRelease(analog_dim_signal);
            break;
        case SYS_CONFIG_FRQ:
            data_center_read_config_data(SYS_CONFIG_FRQ, &cofig_pack.frequency);
            ack = rs485_set_led_frq(&dev_lamp,  true, 5, 200, cofig_pack.frequency);
            break;
        case SYS_CONFIG_CURVE_TYPE:
			data_center_read_config_data(SYS_CONFIG_HIGH_SPEED_MODE, &hs_mode);
            data_center_read_config_data(SYS_CONFIG_CURVE_TYPE, &cofig_pack.curve_type);
            ack =  rs485_set_dimming_curve(&dev_lamp,  true, 5, 200, cofig_pack.curve_type);
			if(hs_mode == 1)osSemaphoreRelease(analog_dim_signal);
            break;
        case SYS_CONFIG_OUTPUT_MODE:	
			if(PAGE_UPDATE != screen_get_act_pid() && get_base_updata_state() == 0)
			{
				data_center_read_config_data(SYS_CONFIG_HIGH_SPEED_MODE, &hs_mode);
				data_center_read_config_data(SYS_CONFIG_OUTPUT_MODE, &cofig_pack.output_mode);
				data_center_read_sys_info(SYS_INFO_POWER, &info_power);
				ack =  rs485_set_power_mode(&dev_lamp,  true, 5, 200, cofig_pack.output_mode, info_power.ratio, 0);
			}
			if(hs_mode == 1)osSemaphoreRelease(analog_dim_signal);
            break;
        case SYS_CONFIG_HIGH_SPEED_MODE:
            data_center_read_config_data(SYS_CONFIG_HIGH_SPEED_MODE, &hs_mode);
			rs485_hs_mode_switch(hs_mode);
        break;
        case SYS_CONFIG_YOKE_PAN:
        {
			data_center_read_sys_info(SYS_INFO_DMX, &dmx_link_state);
            data_center_read_config_data(SYS_CONFIG_YOKE_PAN, &pan_value);
			if(dmx_link_state.eth_linked != 1 && dmx_link_state.wired_linked != 1 && dmx_link_state.wireless_linked != 1)
			{
				gui_electric_move_event_gen();
			}
			
            ack = rs485_set_motor_angle(&dev_lamp,  true, 5, 100, RS485_Rotale_Motor, pan_value);
        }
        break;
        case SYS_CONFIG_YOKE_TILT:
        {
			static uint16_t pan_count = 0;
			
			data_center_read_sys_info(SYS_INFO_DMX, &dmx_link_state);
            data_center_read_config_data(SYS_CONFIG_YOKE_TILT, &tilei_value);
            if(dmx_link_state.eth_linked != 1 && dmx_link_state.wired_linked != 1 && dmx_link_state.wireless_linked != 1)
			{
				gui_electric_move_event_gen();
			}
            ack = rs485_set_motor_angle(&dev_lamp,  true, 5, 100, RS485_Pitch_Motor, tilei_value);
        }
        break;
        case SYS_CONFIG_FRESNEL_ANGLE:
        {
			data_center_read_sys_info(SYS_INFO_DMX, &dmx_link_state);
            data_center_read_config_data(SYS_CONFIG_FRESNEL_ANGLE, &tilei_value);
			if(dmx_link_state.eth_linked != 1 && dmx_link_state.wired_linked != 1 && dmx_link_state.wireless_linked != 1)
			{
				gui_electric_move_event_gen();
			}
			
            ack = rs485_set_motor_angle(&dev_lamp, true, 5, 100, RS485_Fresnel_Motor, tilei_value);
        }
        break;
		case SYS_CONFIG_DEMO_STATE:
		{
			rs485_set_motor_demo(&dev_lamp, true);
		}
		break;
        default:break;
    }
    return ack;
}

bool light_rs485_write_sysinfo(uint8_t mode)
{
    bool ack = true;
    struct  sys_info_cfx_name cfx_info;
    
    switch(mode)
    {
        case SYS_INFO_CFX_NAME: 
            data_center_read_sys_info(SYS_INFO_CFX_NAME, &cfx_info); 
            
            if(cfx_info.state == 0)
            {
                ack =  rs485_get_cfx_bank_rw(&dev_lamp,  true, 5, 100, cfx_info.cfx_type);
            }
            else if(cfx_info.state == 1)
                ack =  rs485_set_cfx_name(&dev_lamp,  true, 5, 100, cfx_info.cfx_type, cfx_info.cfx_bank, &cfx_info.cfx_name[cfx_info.cfx_type].name[cfx_info.cfx_bank][0]);
            break;
    }
    return ack;
}


bool light_rs485_trigger(uint8_t mode, uint8_t trigger_state)
{
    bool ack = true;
    switch(mode)
    {
        case EV_DATA_LIGHT_TRIGGRT:
            ack = rs485_find_Lamp_mode(&dev_lamp, true, 5, 100, trigger_state);
        break;
        case EV_DATA_LIGHT_SWITCH:
             ack =  rs485_set_sleep_mode(&dev_lamp, true, 5, 100, trigger_state);
        break;
        default:break;
    }
    return ack; 
}

static void light_unconnet_handle(void)
{
	sys_error_type err_msg ={0};
	
	s_info_read_body.motorized_fresnel_soft_ver[0] = 0;
	s_info_read_body.motorized_fresnel_soft_ver[1] = 0;
	s_info_read_body.motorized_yoke_soft_ver[0] = 0;
	s_info_read_body.motorized_yoke_soft_ver[1] = 0;
	s_info_read_body.soft_ver[0] = 0;
	s_info_read_body.hard_ver[0] = 0;
	s_info_read_body.ambient_temp = 0;
	s_info_read_body.cob_temp = 0;
	s_info_read_body.led_run_time = 0;
	s_info_read_body.exist = 0;
	dev_power.pwm_dim_ctrl(0);
	dev_power.analog_dim_ctrl(0);
	app_power_iic_send_all_data(0);
	data_center_read_sys_info(SYS_INFO_ERROR_TYPE, &err_msg); 
	memset(&err_msg.error_eh,0,sizeof(err_msg.error_eh));	
	memset(&s_access_info, 0, sizeof(s_access_info));
	data_center_write_sys_info(SYS_INFO_ACCESS, &s_access_info);
	data_center_write_sys_info(SYS_INFO_ERROR_TYPE, &err_msg); 
	data_center_write_sys_info(SYS_INFO_LAMP, &s_info_read_body);
}

void light_rs485_link_config(void)
{
	sys_error_type err_msg ={0};
    static uint8_t reconnected_sta = 0;
    static rs485_cmd_body_t cmd_body_1;
    static rs485_cmd_body_t cmd_body_2;
    static rs485_cmd_body_t cmd_body_3;
    static rs485_cmd_body_t cmd_body_4;
    static rs485_cmd_body_t cmd_body_5;
    sys_config_t    config_boby;
    light_boby_t light_boby = {0};
	struct sys_info_power info_power;
	
    switch(reconnected_sta)
    {
        case 0:
			data_center_read_sys_info(SYS_INFO_ERROR_TYPE, &err_msg); 
			if(rs485_get_device_version(&dev_lamp, true, 5, 100, &s_info_read_body))
			{
				#if PROJECT_TYPE == 307
				if(s_info_read_body.product_num == 0x09)
				#elif PROJECT_TYPE == 308
				if(s_info_read_body.product_num == 0x10)
				#endif
				{
					s_info_read_body.exist = 1;
					reconnected_sta = 1;
					err_msg.error_eh.lamp_head_not_match_eh_017 = 0;
				}
				else
				{
					if(rs485_get_device_version(&dev_lamp, true, 5, 100, &s_info_read_body))
					{
						#if PROJECT_TYPE == 307
						if(s_info_read_body.product_num != 0x09)
						#elif PROJECT_TYPE == 308
						if(s_info_read_body.product_num != 0x10)
						#endif
						{
							err_msg.error_eh.lamp_head_not_match_eh_017 = 1;
						}
					}
				}
			}
			else
			{
				light_unconnet_handle();
			}
			data_center_write_sys_info(SYS_INFO_ERROR_TYPE, &err_msg); 
            data_center_write_sys_info(SYS_INFO_LAMP, &s_info_read_body);
            break; 
        case 1:
		{
		
			if(dev_lamp.time> 0)
			{
				dev_lamp.time--;
				return;
			}
            user_exit_mixplug_warn();
            data_center_read_config_data(SYS_CONFIG_FRQ, &config_boby.frequency);
            cmd_body_1.header.cmd_type = RS485_Cmd_Dim_Frq;
            cmd_body_1.cmd_arg.dimming_frq.adjust_val = config_boby.frequency;
            data_center_read_config_data(SYS_CONFIG_FAN_MODE, &config_boby.fan_mode);
            cmd_body_2.header.cmd_type =  RS485_Cmd_Fan;
			cmd_body_2.cmd_arg.fan.mode =  config_boby.fan_mode + 1;  //testing  
            data_center_read_config_data(SYS_CONFIG_CURVE_TYPE, &config_boby.curve_type);
            cmd_body_3.header.cmd_type = RS485_Cmd_Dimming_Curve;
            cmd_body_3.cmd_arg.dimming_curve.curve = config_boby.curve_type;
            data_center_read_config_data(SYS_CONFIG_OUTPUT_MODE, &config_boby.output_mode);
			data_center_read_sys_info(SYS_INFO_POWER, &info_power);
            cmd_body_4.header.cmd_type = RS485_Cmd_IlluminationMode;
            cmd_body_4.cmd_arg.illumination_mode.mode = config_boby.output_mode;
			cmd_body_4.cmd_arg.illumination_mode.ratio = info_power.ratio;
            data_center_read_config_data(SYS_CONFIG_HIGH_SPEED_MODE, &config_boby.hs_mode);
            cmd_body_5.header.cmd_type = RS485_Cmd_High_speed;
            cmd_body_5.cmd_arg.high_speed_mode.mode = config_boby.hs_mode;
            if(0 == dev_rs485_cmd_write(&dev_lamp.rs485, 5, 100, true, NULL, &cmd_body_1, &cmd_body_2, &cmd_body_3, &cmd_body_4,&cmd_body_5, DEV_RS485_VA_ARG_END))
            {	
                if(config_boby.hs_mode == 1)
                {
					app_power_iic_send_all_data(0);
				
					if(ui_error_code_event_get())
					{
						dev_power.analog_dim_ctrl(0);
					}
					else
					{
						dev_power.analog_dim_ctrl(1);
					}
					dev_power.pwm_dim_ctrl(0);
                    hal_self_adapt_all_pwm_set(2000);
                }
                else
                {
					dev_power.analog_dim_ctrl(0);
					osDelay(20);
					if(s_info_read_body.hard_ver[0] == 0x10)
						dev_adapt_pwm_limit_set(&pwm_adapt_20k,20);
					else
						dev_adapt_pwm_limit_set(&pwm_adapt_80k,20);
					app_power_iic_send_all_data(1);
					dev_power.pwm_dim_ctrl(1);
                }
                reconnected_sta = 2;  
            }
            else
			{
                goto out;
            }
            break;
		}
		
		case 2:
		{
		
			if(rs485_analog_dac_data_get_init(&dev_lamp))
			{
				 reconnected_sta = 3;  
			}
			else
			{
				goto out; 
			}
			break;
		}
		case 3:
		{
			data_center_read_light_data(date_center_get_light_mode(), &light_boby); 
			package_485_light_data(&cmd_body_1, date_center_get_light_mode(), &light_boby);
			data_center_read_sys_info(SYS_INFO_ACCESS, &s_access_info);
			rs485_get_mount_access(&dev_lamp, true, 5, 100, &s_access_info);
			data_center_write_sys_info(SYS_INFO_ACCESS, &s_access_info);
			if(date_center_get_light_mode() < LIGHT_MODE_SIDUS_MFX || date_center_get_light_mode() > LIGHT_MODE_LOCAL_CFX)
			{
				if( rs485_light_control(&dev_lamp, true, 5, 100, &cmd_body_1) == 0)
				{
					goto out;
				}
			}
			if(rs485_set_sleep_mode(&dev_lamp, true, 5, 300, 1)) 
			{
				data_center_read_config_data(SYS_CONFIG_HIGH_SPEED_MODE, &config_boby.hs_mode); 
				if(config_boby.hs_mode == 1)
				{
					osSemaphoreRelease(analog_dim_signal);
				}
				dev_lamp.Connected = 1;
				dev_lamp.hs_state = 0;
			}
			goto out;
		}
		out:
			reconnected_sta = 0;
			break;
	}
}



static void gui_lfm_data_sync_handle(void)
{
	uint8_t mdoe = 0;
	struct sys_info_dmx          dmx_link_state;
	
	data_center_read_config_data(SYS_CONFIG_WORK_MODE, &mdoe);
	ui_set_work_mode(mdoe);
	data_center_read_sys_info(SYS_INFO_DMX, &dmx_link_state);
	if(dmx_link_state.eth_linked != 1 && dmx_link_state.wired_linked != 1 && dmx_link_state.wireless_linked != 1)
	{
		uint32_t crc32 = CRC32_Calculate(0, (uint8_t*)&g_tUIAllData, sizeof(g_tUIAllData) - 4);
		if (crc32 != g_tUIAllData.checksum) {
			g_tUIAllData.checksum = crc32;
			gui_lfm_seng_data_set_flag(0);
			gui_data_sync_event_gen();
		}
	}
}

static void light_frist_power_get_access(void)
{
	int16_t tile11[2] = {0};
	if(0 != rs485_get_access(&dev_lamp,  true, 5, 100, &s_access_info))
	{
		if(s_access_info.fresnel_state == 1 || s_access_info.optical_20_access_state == 1)
		{
			uint8_t tiltm;
			data_center_read_config_data(SYS_CONFIG_ANGLE_LIMIT, &tiltm);
			data_center_read_config_data(SYS_CONFIG_YOKE_TILT_LIMIT, &tile11);
			if(tiltm == 1)
			{
				if(tile11[0] <= -120)
					tile11[0] = -120;
				if(tile11[1] >= 120)
					tile11[1] = 120;
			}
			data_center_write_config_data_no_event(SYS_CONFIG_YOKE_TILT_LIMIT, &tile11);
		}
	}
}

static void light_motor_reset_state_get(void)
{
	bool res = false;
	bool res2 = false;
	
	if(s_motor_reset_type == 1)
	{
		res = rs485_get_motor_reset(&dev_lamp, true, 5, 100, RS485_Fresnel_Motor);
		if(res == true)
		{
			res = rs485_get_motor_reset(&dev_lamp, true, 5, 100, RS485_Fresnel_Motor);
			if(res == true)
			{
				ui_set_ele_reset_start_flag(BLE_RESET_SUCCEED);
			}
		}
	}
	else if(s_motor_reset_type == 2)
	{
		res = rs485_get_motor_reset(&dev_lamp, true, 5, 100, RS485_Pitch_Motor);
		res2 = rs485_get_motor_reset(&dev_lamp, true, 5, 100, RS485_Rotale_Motor);
		if(res == true && res2 == true)
		{
			res = rs485_get_motor_reset(&dev_lamp, true, 5, 100, RS485_Pitch_Motor);
			res2 = rs485_get_motor_reset(&dev_lamp, true, 5, 100, RS485_Rotale_Motor);
			if(res == true && res2 == true)
			{
				ui_set_ele_reset_start_flag(BLE_RESET_SUCCEED);
			}
		}
	}
}

void light_rs485_link_set(bool link)
{
	if(osOK != osMutexAcquire(MutexID_Rs485, 0))
    {
        return ;
    }
	dev_lamp.Connected = link;
	osMutexRelease(MutexID_Rs485);
}

const osThreadAttr_t g_light_thread_attr =
{
    .name = "app light",
    .priority = osPriorityAboveNormal1,
    .stack_size = 1280 * 4
};
osThreadId_t app_light_task_id;
uint32_t light_task_static_size;


extern osSemaphoreId_t thread_sync_sigl;

void app_light_thread_entrance(void *argument)
{
    MutexID_Rs485 = osMutexNew(NULL);
    uint16_t  scount = 0;
    uint8_t buffer[156] = {0};
	uint8_t state[2] ={0};
	uint16_t delay_s = 0;
    struct sys_info_power sys_power;
    struct update_device_ops lamp_ops = 
    {
        .open = light_update_open,
        .erase = light_update_erase,
        .write = light_update_write,
        .close = light_update_close,
    };
	
    os_ev_subscribe_event(MAIN_EV_DATA, EV_DATA_LIGHT_CHANGE | EV_DATA_CONFIG_CHANGE | EV_DATA_CFX_BANK_RW | EV_DATA_WRITE_CFX_NAME
                          | EV_DATA_READ_MOTOR_ANGLE|EV_DATA_READ_CFX_NAME | EV_DATA_RESET_MOTOR | EV_DATA_FACTORY_RESET | EV_DATA_RESET_FRESENEL | EV_DATA_READ_VER, light_get_event_deal);
    os_ev_subscribe_event(MAIN_EV_LIGHT, EV_DATA_LIGHT_TRIGGRT|EV_DATA_LIGHT_SWITCH|EV_DATA_LIGHT_ZERO, light_get_event_deal);
    os_ev_subscribe_event(MAIN_EV_DMX, EV_DMX_DISCONNECTED, dmx_disconnected_handle);
    update_device_regsiter(USB_UPDATA_LAMP_DEVICE,  &lamp_ops);
    dev_lamp.rs485.write_data = rs485_usart_tx_data;
    dev_lamp.rs485.read_data = rs485_usart_rx_data;
    rs485_init_config();
 	hal_iic_init(HAL_IIC_ANALOG_CW1_CW2_R_G);
	hal_iic_init(HAL_IIC_ANALOG_WW1_WW2_B);
	hal_timer_pwm_init(HAL_PWM_SELF_ADAPTION_R);
	hal_timer_pwm_init(HAL_PWM_SELF_ADAPTION_G);
	hal_timer_pwm_init(HAL_PWM_SELF_ADAPTION_B);
	hal_timer_pwm_init(HAL_PWM_SELF_ADAPTION_WW1);
	hal_timer_pwm_init(HAL_PWM_SELF_ADAPTION_WW2);
	hal_timer_pwm_init(HAL_PWM_SELF_ADAPTION_CW1);
	hal_timer_pwm_init(HAL_PWM_SELF_ADAPTION_CW2);  
    analog_dim_signal = osSemaphoreNew (1, 0, NULL);   
    light_timer_id = osTimerNew(rs485_time_ms_handle, osTimerPeriodic, NULL, NULL);
	light_timer_fade_id = osTimerNew(light_dac_fade_handle, osTimerPeriodic, NULL, NULL);
	dev_lamp.time = 0;
    osTimerStart(light_timer_id, 1); 
	osTimerStart(light_timer_fade_id, 2);
    osSemaphoreRelease(thread_sync_sigl);
	data_center_read_sys_info(SYS_INFO_POWER, &sys_power);
	sys_power.ratio = 10000;
	data_center_write_sys_info(SYS_INFO_POWER, &sys_power);
	light_frist_power_get_access();
	s_motor_demo_flag = 0;
	
    while(1)
    {
		os_ev_pull_event(MAIN_EV_LIGHT|MAIN_EV_DMX|MAIN_EV_DATA);
		data_center_read_sys_info(SYS_INFO_POWER, &sys_power);
		if(sys_power.state == 1)
		{
			if(dev_lamp.Connected == 0)
			{
				if(PAGE_UPDATE != screen_get_act_pid() || 
				  ((PAGE_UPDATE == screen_get_act_pid() && screen_get_act_spid() < SUB_ID_UPDATING) || (PAGE_WORK_MODE == screen_get_act_pid() && screen_get_act_spid() != SUB_ID_SYNC_UPDATAING)))
				{
					light_rs485_link_config();
				}
				gui_lfm_data_sync_handle();
			}
			else{
				dev_lamp.time = 200;
				if((PAGE_UPDATE != screen_get_act_pid() || ((PAGE_UPDATE == screen_get_act_pid() && screen_get_act_spid() < SUB_ID_UPDATING)) || (PAGE_WORK_MODE == screen_get_act_pid() && screen_get_act_spid() != SUB_ID_SYNC_UPDATAING)) \
					&& get_base_updata_state() == 0)   //升级时不发送其他485数据
				{
					if(scount++ > 10) 
					{
						scount = 0;
						light_rs485_self_crtl();
					}
				}
                if(1 == get_ble_cfx_file_state())
                {
                    if(osOK == osMessageQueueGet(os_queue_ble_cfx, buffer, NULL, 0))
                    {
                        api_cfx_file_handle(&dev_lamp, buffer);
                    }
                }
				if(s_motor_demo_flag == 0 && s_access_info.yoke_state == 1)
				{
					if(delay_s++ > 400)
					{
						delay_s = 0;
						s_motor_demo_flag = 1;
						data_center_read_config_data(SYS_CONFIG_DEMO_POWER_ON_LIMIT, &state[0]);
						if(state[0])
						{
							state[1] = 1;
							data_center_write_config_data_no_event(SYS_CONFIG_DEMO_STATE, &state[1]);
							rs485_set_motor_demo(&dev_lamp, true);
						}
					}
				}
				if(ui_get_ele_reset_start_flag() == BLE_RESET_START)
				{
					light_motor_reset_state_get();
				}
				rs485_power_on_trigger_cfx();  // 在cfx自定义光效下关机再次上电自动开机并执行cfx自定义光效
			}
			//写485连接状态到数据中心，方便其他线程读取
			s_info_read_body.exist = dev_lamp.Connected; 
			rs485_analog_dac_data_get(&dev_lamp);        
			rs485_base_data_output();
			rs485_rgbww_data_test();
			rs485_pump_data_test();
			rs485_motor_state_test();
			rs485_power_adjust_event();
			rs485_singel_power_crc_event();
			light_flags_handle();
			rs485_power_set_test();
			rs485_run_time_set_test();
			rs485_get_local_info_test();
			rs485_fresnel_Industrial_data_test();
		}
		rs485_heart_beat();

        osDelay(5);
    }

}

void light_get_event_deal(uint32_t event_type, uint32_t event_value)
{
    light_boby_t light_boby = {0};
    static rs485_cmd_body_t cmd_body_1 = {0};
    uint8_t trigger_state = 0;
    uint8_t  type_1 = 0;
    uint8_t hs_mode;
    uint16_t send_time = 0;
	uint8_t effects_mode = 0;
	
    if(!dev_lamp.Connected)
    {
		if(mdx_loss_start == 1)
			mdx_loss_start = 0;
        return ;
    }
       
    if(osOK == osMutexAcquire(MutexID_Rs485, 0))
    {
        if(event_type == NULL)
        {
            return;
        }
        switch(event_type)
        {
            case MAIN_EV_DATA:  
            {
				if( EV_DATA_LIGHT_CHANGE & event_value)
				{
					effects_mode = date_center_get_light_mode();
					data_center_read_light_data(effects_mode, &light_boby);
					package_485_light_data(&cmd_body_1, effects_mode, &light_boby);
					data_center_read_config_data(SYS_CONFIG_HIGH_SPEED_MODE, &hs_mode);
					if(hs_mode == 1)
					{
						osSemaphoreRelease(analog_dim_signal);
					}
					if(effects_mode == LIGHT_MODE_SIDUS_CFX && PAGE_SIDUS_PRO_FX == screen_get_act_pid())  //响应蓝牙读取光效拾取初始化
					{
						send_ble_cfx_ctrl_pack(light_boby.cfx.type, light_boby.cfx.bank);
					}
					else if(effects_mode == LIGHT_MODE_SIDUS_CFX && PAGE_CUSTOM_FX == screen_get_act_pid())
					{
						uint8_t spid = screen_get_act_spid();
						if(spid == SUB_ID_CUSTOM_FX_CHOOSE && dev_lamp.Connected == 1)
						{
							ui_set_cfx_read_bank_state(1);
						}
					}

					if(effects_mode == LIGHT_MODE_LOCAL_CFX && PAGE_CUSTOM_FX == screen_get_act_pid())  // 发送读取10次cfx名字后退出，不用发送调光指令
					{
						screen_load_sub_page_with_stack(SUB_ID_CUSTOM_FX_CHOOSE, light_boby.local_cfx_arg.type); 
						break;
					}
					
					if(effects_mode == LIGHT_MODE_SIDUS_CFX)
						send_time = 2000;
					else
						send_time = 100;
					
					if(effects_mode != LIGHT_MODE_DMX)
						gui_lfm_data_sync_handle();
					dev_lamp.Connected = rs485_light_control(&dev_lamp, true, 5, send_time, &cmd_body_1);
					if(mdx_loss_start == 1)
					{
						mdx_loss_start = 0;
					}
					#if PROJECT_TYPE==308
					if(effects_mode == LIGHT_MODE_FX_LIGHTNING)
					{
						if(light_boby.lightning.trigger == 1)
							light_boby.lightning.trigger = 0;
						data_center_write_light_data_no_event(effects_mode, &light_boby);
					}
					if(effects_mode == LIGHT_MODE_FX_EXPLOSION)
					{
						if(light_boby.explosion.trigger == 1)
							light_boby.explosion.trigger = 0;
						data_center_write_light_data_no_event(effects_mode, &light_boby);
					}
					#endif
				}
				if( EV_DATA_CONFIG_CHANGE & event_value)
				{
					type_1 = os_ev_get_event_data(event_type, EV_DATA_CONFIG_CHANGE);
					dev_lamp.Connected = light_rs485_write_config(type_1);
				}
				if( EV_DATA_READ_VER & event_value)
				{
					if(s_access_info.yoke_state == 1)
					{
						for(uint8_t i = 0; i<10; i++)
							rs485_get_device_version2(&dev_lamp, true, 5, 200, &s_info_read_body);
						data_center_write_sys_info(SYS_INFO_LAMP, &s_info_read_body);
						data_center_read_sys_info(SYS_INFO_MOTOR, &s_light_motor_info);
						rs485_get_motor_angle(&dev_lamp, true, 5, 500, RS485_Pitch_Motor, &s_light_motor_info.tilt_angle);
						rs485_get_motor_angle(&dev_lamp, true, 5, 500, RS485_Rotale_Motor, &s_light_motor_info.pan_angle);
						data_center_write_sys_info(SYS_INFO_MOTOR, &s_light_motor_info);
					}
					else
					{
						s_info_read_body.motorized_yoke_soft_ver[0] = 0;
						s_info_read_body.motorized_yoke_soft_ver[1] = 0;
						data_center_write_sys_info(SYS_INFO_LAMP, &s_info_read_body);
					}
					if(s_access_info.fresnel_state == 1)
					{
						for(uint8_t i = 0; i<10; i++)
							rs485_get_device_version2(&dev_lamp, true, 5, 200, &s_info_read_body);
						data_center_write_sys_info(SYS_INFO_LAMP, &s_info_read_body);
						data_center_read_sys_info(SYS_INFO_MOTOR, &s_light_motor_info);
						rs485_get_motor_angle(&dev_lamp, true, 5, 500, RS485_Fresnel_Motor, &s_light_motor_info.fresnel_angle);
						data_center_write_sys_info(SYS_INFO_MOTOR, &s_light_motor_info);
					}
					else
					{
						s_info_read_body.motorized_fresnel_soft_ver[0] = 0;
						s_info_read_body.motorized_fresnel_soft_ver[1] = 0;
						data_center_write_sys_info(SYS_INFO_LAMP, &s_info_read_body);
					}
					screen_load_sub_page_with_stack(2, 0);
				}
                if (EV_DATA_CFX_BANK_RW & event_value)
				{
					struct sys_info_cfx_name cfx_info;
					data_center_read_sys_info(SYS_INFO_CFX_NAME, &cfx_info);
					dev_lamp.Connected = rs485_get_cfx_bank_rw(&dev_lamp, true, 5, 1000, cfx_info.cfx_type) ? 1 : 0;
				}
                if (EV_DATA_WRITE_CFX_NAME & event_value)
				{
					struct sys_info_cfx_name cfx_info; 
					data_center_read_sys_info(SYS_INFO_CFX_NAME, &cfx_info);
					dev_lamp.Connected = rs485_set_cfx_name(&dev_lamp, true, 5, 1000, cfx_info.cfx_type, cfx_info.cfx_bank, (char *)cfx_info.read_cfx_name);
					set_read_cfx_name_state(1);
				}
				if( EV_DATA_READ_CFX_NAME & event_value)
				{
					struct sys_info_cfx_name cfx_info;
					data_center_read_sys_info(SYS_INFO_CFX_NAME, &cfx_info);
					if(1 == rs485_get_cfx_name(&dev_lamp, true, 5, 1000, cfx_info.cfx_type, cfx_info.cfx_bank, (char *)cfx_info.read_cfx_name))
					{
						dev_lamp.Connected = 1;
						send_ble_read_cfx_name_pack(cfx_info.read_cfx_name);
					}
					else
					{
						dev_lamp.Connected = 0;
					}
				}
				if (EV_DATA_READ_MOTOR_ANGLE & event_value)
				{
					data_center_read_sys_info(SYS_INFO_MOTOR, &s_light_motor_info);
					type_1 = os_ev_get_event_data(event_type, EV_DATA_READ_MOTOR_ANGLE);
					if(type_1 == 0)
					{
						rs485_get_motor_angle(&dev_lamp, true, 5, 500, RS485_Fresnel_Motor, &s_light_motor_info.fresnel_angle);
						data_center_write_sys_info(SYS_INFO_MOTOR, &s_light_motor_info);
						screen_load_sub_page_with_stack(SUB_ID_ELE_FRESNEL, 0);
					}
					else if(type_1 == 1)
					{
						rs485_get_motor_angle(&dev_lamp, true, 5, 500, RS485_Pitch_Motor, &s_light_motor_info.tilt_angle);
						rs485_get_motor_angle(&dev_lamp, true, 5, 500, RS485_Rotale_Motor, &s_light_motor_info.pan_angle);
						data_center_write_sys_info(SYS_INFO_MOTOR, &s_light_motor_info);
						if(SUB_ID_ELE_YOKE != screen_get_act_spid())
							screen_load_sub_page_with_stack(SUB_ID_ELE_YOKE, 0);
						else
							screen_load_sub_page_with_stack(SUB_ID_ELE_NO_YOKE, 0);
					}
					else if(type_1 == 2)
					{
						rs485_get_motor_angle(&dev_lamp, true, 5, 500, RS485_Pitch_Motor, &s_light_motor_info.tilt_angle);
						rs485_get_motor_angle(&dev_lamp, true, 5, 500, RS485_Rotale_Motor, &s_light_motor_info.pan_angle);
						data_center_write_sys_info(SYS_INFO_MOTOR, &s_light_motor_info);
						screen_load_sub_page_with_stack(SUB_ID_ELE_NO_YOKE, 0);
					}
					else if(type_1 == 3)
					{
						rs485_get_motor_angle(&dev_lamp, true, 5, 500, RS485_Fresnel_Motor, &s_light_motor_info.fresnel_angle);
						rs485_get_motor_angle(&dev_lamp, true, 5, 500, RS485_Pitch_Motor, &s_light_motor_info.tilt_angle);
						rs485_get_motor_angle(&dev_lamp, true, 5, 500, RS485_Rotale_Motor, &s_light_motor_info.pan_angle);
						data_center_write_sys_info(SYS_INFO_MOTOR, &s_light_motor_info);
					}
					else if(type_1 == 4)
					{
						rs485_get_motor_angle(&dev_lamp, true, 5, 500, RS485_Fresnel_Motor, &s_light_motor_info.fresnel_angle);
						rs485_get_motor_angle(&dev_lamp, true, 5, 500, RS485_Pitch_Motor, &s_light_motor_info.tilt_angle);
						rs485_get_motor_angle(&dev_lamp, true, 5, 500, RS485_Rotale_Motor, &s_light_motor_info.pan_angle);
						data_center_write_sys_info(SYS_INFO_MOTOR, &s_light_motor_info);
						screen_load_page(PAGE_ELECT_ACCESSORY, 0, true);
					}  
					else if(type_1 == 5)
					{
						rs485_get_motor_angle(&dev_lamp, true, 5, 500, RS485_Pitch_Motor, &s_light_motor_info.tilt_angle);
						rs485_get_motor_angle(&dev_lamp, true, 5, 500, RS485_Rotale_Motor, &s_light_motor_info.pan_angle);
						data_center_write_sys_info(SYS_INFO_MOTOR, &s_light_motor_info);
						screen_load_sub_page_with_stack(SUB_ID_ELE_APTURE_NO_YOKE, 0);
					}
                }
				if(EV_DATA_RESET_MOTOR & event_value)
				{
					struct sys_info_dmx          dmx_link_state;
					
					data_center_read_sys_info(SYS_INFO_DMX, &dmx_link_state);
					if(dmx_link_state.eth_linked != 1 && dmx_link_state.wired_linked != 1 && dmx_link_state.wireless_linked != 1)
					{
						gui_electric_move_event_gen();
					}
					
					rs485_set_motor_reset(&dev_lamp, true, 5, 500, RS485_Pitch_Motor);
					rs485_set_motor_reset(&dev_lamp, true, 5, 500, RS485_Rotale_Motor);
					
					s_motor_reset_type = 2;
				}
				if(EV_DATA_RESET_FRESENEL & event_value)
				{
					struct sys_info_dmx          dmx_link_state;
					
					data_center_read_sys_info(SYS_INFO_DMX, &dmx_link_state);
					if(dmx_link_state.eth_linked != 1 && dmx_link_state.wired_linked != 1 && dmx_link_state.wireless_linked != 1)
					{
						gui_electric_move_event_gen();
					}
					rs485_set_motor_reset(&dev_lamp, true, 5, 500, RS485_Fresnel_Motor);
					
					s_motor_reset_type = 1;
				}
				if(event_value & EV_DATA_FACTORY_RESET)
				{
					static rs485_cmd_body_t cmd_body_1;
					static rs485_cmd_body_t cmd_body_2;
					static rs485_cmd_body_t cmd_body_3;
					static rs485_cmd_body_t cmd_body_4;
					sys_config_t    config_boby;
					struct sys_info_power info_power;
	
					data_center_read_config_data(SYS_CONFIG_FRQ, &config_boby.frequency);
					cmd_body_1.header.cmd_type = RS485_Cmd_Dim_Frq;
					cmd_body_1.cmd_arg.dimming_frq.adjust_val = config_boby.frequency;
					data_center_read_config_data(SYS_CONFIG_FAN_MODE, &config_boby.fan_mode);
					cmd_body_2.header.cmd_type =  RS485_Cmd_Fan;
					cmd_body_2.cmd_arg.fan.mode =  config_boby.fan_mode + 1;  //testing  
					data_center_read_config_data(SYS_CONFIG_CURVE_TYPE, &config_boby.curve_type);
					cmd_body_3.header.cmd_type = RS485_Cmd_Dimming_Curve;
					cmd_body_3.cmd_arg.dimming_curve.curve = config_boby.curve_type;
					data_center_read_config_data(SYS_CONFIG_OUTPUT_MODE, &config_boby.output_mode);
					data_center_read_sys_info(SYS_INFO_POWER, &info_power);
					cmd_body_4.header.cmd_type = RS485_Cmd_IlluminationMode;
					cmd_body_4.cmd_arg.illumination_mode.mode = config_boby.output_mode;
					cmd_body_4.cmd_arg.illumination_mode.ratio = info_power.ratio;
					data_center_read_config_data(SYS_CONFIG_HIGH_SPEED_MODE, &config_boby.hs_mode);
					dev_rs485_cmd_write(&dev_lamp.rs485, 5, 100, true, NULL, &cmd_body_1, &cmd_body_2, &cmd_body_3, &cmd_body_4, DEV_RS485_VA_ARG_END);
					if(config_boby.hs_mode)
					{
						config_boby.hs_mode = 0;
						data_center_write_config_data_no_event(SYS_CONFIG_HIGH_SPEED_MODE, &config_boby.hs_mode);
						rs485_hs_mode_switch(0);
					}
					if(s_access_info.yoke_state == 1)
						rs485_set_motor_demo(&dev_lamp, true);
				}
            }  
			break;
            case MAIN_EV_LIGHT:
            {
               if( EV_DATA_LIGHT_TRIGGRT & event_value)
                {
					trigger_state = os_ev_get_event_data( event_type,  EV_DATA_LIGHT_TRIGGRT)==0?1:0;
					dev_lamp.Connected = light_rs485_trigger(  EV_DATA_LIGHT_TRIGGRT, trigger_state);
                }
				if(EV_DATA_LIGHT_SWITCH& event_value)
				{
					trigger_state = os_ev_get_event_data( event_type,  EV_DATA_LIGHT_SWITCH);   
					dev_lamp.Connected = light_rs485_trigger( EV_DATA_LIGHT_SWITCH, trigger_state);
					data_center_read_config_data(SYS_CONFIG_HIGH_SPEED_MODE, &hs_mode);
					if(hs_mode)
					{
						if(trigger_state)
						{
							if(ui_error_code_event_get())
							{
								dev_power.analog_dim_ctrl(0);
							}
							else
							{
								rs485_hs_mode_switch(2);
							}
						}
						else
						{					
							dev_power.analog_dim_ctrl(0);
							app_power_iic_send_all_data_no_wait(0);
						}
					}	
				}
			   if(EV_DATA_LIGHT_ZERO& event_value)
			   {	
				   data_center_read_config_data(SYS_CONFIG_HIGH_SPEED_MODE, &hs_mode);
				     /*识别不上附件不亮灯*/ 
				    cmd_body_1.header.cmd_type = RS485_Cmd_CCT;
					cmd_body_1.cmd_arg.cct.cct = 5600;
					cmd_body_1.cmd_arg.cct.intensity = 0;
					cmd_body_1.cmd_arg.cct.duv =  0;
					cmd_body_1.cmd_arg.cct.fade = 0;
					dev_lamp.Connected = rs485_light_control(&dev_lamp, true, 5, 100, &cmd_body_1);
				    if(hs_mode)
					{
						app_power_iic_send_all_data_no_wait(0);
						memset(&dac_fade.analog_dac_current, 0, sizeof(dac_fade.analog_dac_current));
						#if PROJECT_TYPE == 307
							dev_analog_green_ctrl(false);
						#endif
					}
			   }	
			}
            break;
            default:break;
        }

        osMutexRelease(MutexID_Rs485);
    } 
}

static void dmx_disconnected_handle(uint32_t event_type, uint32_t event_value)
{
    uint8_t mode = 0;
	struct sys_info_power sys_power;
	
    if(!dev_lamp.Connected)
    {
        return ;
    }
	data_center_read_sys_info(SYS_INFO_POWER, &sys_power);
	if(sys_power.state != 1)
	{
		return;
	}
    if(event_type != MAIN_EV_DMX && event_value != EV_DMX_DISCONNECTED)
    {
        return;
    }
    if((screen_get_act_pid() >= PAGE_LIGHT_MODE_HSI && screen_get_act_pid() <= PAGE_HSI_LIMIT_SETTING) || screen_get_act_pid() == PAGE_SIDUS_PRO_FX)   
    {
        s_dmx_loss_state.dmx_loss_flag = 0;
        return;
    }
    s_dmx_loss_state.dmx_loss_flag = 1;
    data_center_read_config_data(SYS_CONFIG_DMX_LOSS_BEHAVIOR, &mode);
	if(mode ==  DMX_LOSS_BEHAVIOR_HLS)
	{
		s_dmx_loss_state.dmx_loss_mode = DMX_LOSS_BEHAVIOR_HLS;
	}
    if(mode == DMX_LOSS_BEHAVIOR_BO)
    {
        s_dmx_loss_state.dmx_loss_mode = DMX_LOSS_BEHAVIOR_BO;   //维持最后一次亮度
    }
    else if(mode == DMX_LOSS_BEHAVIOR_FTB)
    {
        struct db_dmx dmx;
        
        data_center_read_light_data(LIGHT_MODE_DMX, &dmx);
        s_dmx_loss_state.dmx_loss_mode = DMX_LOSS_BEHAVIOR_FTB;    //缓变
        s_dmx_loss_state.dmx_fade_ms = 0;
        s_dmx_loss_state.dmx_fade_s = 0;
        dmx.dmx_data.lightness = dmx.dmx_data.lightness > 10?dmx.dmx_data.lightness:0;
        s_dmx_loss_state.dmx_fade_time = (uint8_t)(((float)dmx.dmx_data.lightness) * 0.06f);
        s_dmx_loss_state.dmx_fade_slope = dmx.dmx_data.lightness / s_dmx_loss_state.dmx_fade_time;
        if(dmx.dmx_data.type > DMX_TYPE_MIXING)
        {
            s_dmx_loss_state.dmx_loss_mode = DMX_LOSS_BEHAVIOR_BO; 
        }
    }
    else if(mode == DMX_LOSS_BEHAVIOR_HFO)
    {
        struct db_dmx dmx;
        
        data_center_read_light_data(LIGHT_MODE_DMX, &dmx);
        s_dmx_loss_state.dmx_loss_mode = DMX_LOSS_BEHAVIOR_HFO;
        s_dmx_loss_state.dmx_hold_ms = 0;
        s_dmx_loss_state.dmx_hold_s = 0;
        s_dmx_loss_state.dmx_fade_time = (uint8_t)(((float)dmx.dmx_data.lightness) * 0.06f);
        s_dmx_loss_state.dmx_fade_slope = dmx.dmx_data.lightness / s_dmx_loss_state.dmx_fade_time;
		if(dmx.dmx_data.type > DMX_TYPE_MIXING)
        {
            s_dmx_loss_state.dmx_loss_mode = DMX_LOSS_BEHAVIOR_BO; 
        }
    }
}

void app_light_flags_set(uint32_t flags)
{
	osThreadFlagsSet(app_light_task_id, flags);
}

uint32_t app_light_flags_get(uint32_t flags)
{
	uint32_t enflags;
	
	enflags =  osThreadFlagsWait(flags, osFlagsWaitAny, 0);
	
	return enflags;
}

static void light_flags_handle(void)
{
	static uint8_t delay_time = 0;
	uint32_t flags = 0;
	
	if(delay_time++ < 3)  //20ms进行一次事件解析
	{
		return;
	}
	delay_time = 0;
	
	if(!dev_lamp.Connected)
    {
         return ;
    }
	flags = osThreadFlagsGet();
	if(flags & LIGHT_RS485_OUTPUT_MODE_FLAG)
	{
		osThreadFlagsClear(LIGHT_RS485_OUTPUT_MODE_FLAG);
		light_rs485_write_config(SYS_CONFIG_OUTPUT_MODE);
	}
	if(flags & LIGHT_RS485_CURVE_TYPE_FLAG)
	{
		osThreadFlagsClear(LIGHT_RS485_CURVE_TYPE_FLAG);
		light_rs485_write_config(SYS_CONFIG_CURVE_TYPE);
	}
	if(flags & LIGHT_RS485_FAN_MODE_FLAGE)
	{
		osThreadFlagsClear(LIGHT_RS485_FAN_MODE_FLAGE);
		light_rs485_write_config(SYS_CONFIG_FAN_MODE);
	}
	if(flags & LIGHT_RS485_FRQ_FLAG)
	{
		osThreadFlagsClear(LIGHT_RS485_FRQ_FLAG);
		light_rs485_write_config(SYS_CONFIG_FRQ);
	}
	if(flags & LIGHT_RS485_YOKE_PAN_FLAG)
	{
		osThreadFlagsClear(LIGHT_RS485_YOKE_PAN_FLAG);
		light_rs485_write_config(SYS_CONFIG_YOKE_PAN);
	}
	if(flags & LIGHT_RS485_YOKE_TILT_FLAG)
	{
		osThreadFlagsClear(LIGHT_RS485_YOKE_TILT_FLAG);
		light_rs485_write_config(SYS_CONFIG_YOKE_TILT);
	}
	if(flags & LIGHT_RS485_FRESNEL_ANGLE_FLAG)
	{
		osThreadFlagsClear(LIGHT_RS485_FRESNEL_ANGLE_FLAG);
		light_rs485_write_config(SYS_CONFIG_FRESNEL_ANGLE);
	}
	if(flags & LIGHT_RS485_HIGHT_MODE_FLAG)
	{
		osThreadFlagsClear(LIGHT_RS485_HIGHT_MODE_FLAG);
		light_rs485_write_config(SYS_CONFIG_HIGH_SPEED_MODE);
	}
}


void light_dac_fade_handle(void *argument)
{
	struct sys_info_power sys_power;
	
	data_center_read_sys_info(SYS_INFO_POWER, &sys_power);
	if(sys_power.state)
	{
		rs485_dac_fade_handle();
	}

}
