/**
 * @file app_gui.c
 * @author Darwin (darwin.xiao@aputure.com)
 * @brief 
 * @version 0.1
 * @date 2022-09-09
 * 
 * @copyright Copyright (c) 2022 爱图仕
 * 
 * @par 修改日志:
 * <table>
 * <tr><th> Date      	  <th>Version <th>Author  		<th>Description
 * <tr><td>2022-09-09     <td>1.0     <td>Darwin.Xiao   <td>内容
 * </table>
 */


#include "app_gui.h"
#include "port/lcd_bsp.h"
#include "lcd.h"
#include "lvgl/lvgl.h"
#include "porting/lv_port_disp_template.h"
#include "porting/lv_port_indev_template.h"
#include "ui_common.h"
#include "page_manager.h"
#include "os_event.h"
#include "user.h"
#include "dev_w25qxx.h"
#include "ui_data.h"
#include "app_power.h"
#include "crc32.h"
#include "hal_gpio.h"
/*********************
 *      DEFINE
 *********************/
#define POWER_LED1_PORT                 HAL_GPIOE
#define POWER_LED1_Pin                  HAL_PIN_0
#define POWER_LED2_PORT                 HAL_GPIOI
#define POWER_LED2_Pin                  HAL_PIN_9

#define SCREEN_PROTECTOR_MS         1200
//#define LCD_POWER_TEST
/*********************
 *      TYPEDEF
 *********************/
typedef struct  {
    struct sc {
        uint32_t tmr_cnt;   //定时器   
        uint8_t  state : 1;     //状态
    } screen_protector;    

    uint8_t power : 1;
} gui_info_t; 


/*********************
 *  STATIC PROTOTYPE
 *********************/

static void gui_init(void);
static void data_changed_callback(uint32_t event_type, uint32_t event_value);
static void ble_event_callback(uint32_t event_type, uint32_t event_value);
static void dmx_event_callback(uint32_t event_type, uint32_t event_value);
static void power_event_callback(uint32_t event_type, uint32_t event_value);
static void event_check_entry(void *argument);
static void gui_1s_callback (void *argument);
static void gui_go_back_effects_ui(void);
static void gui_lfs_data_write_handle(void);
static void app_gui_upgrade_timeout_handle(void);
/*********************
 *  STATIC VATIABLES
 *********************/
static gui_info_t  s_gui_state;
static lv_timer_t *s_pm_timer;
static osTimerId_t s_gui_tmr;
static uint8_t s_frist_gui_power = 0;
osThreadId_t app_gui_task_id;
uint32_t gui_task_static_size;
uint32_t freertos_task_static_size;
static uint8_t s_gui_lfs_time_flag = 0;
static struct sys_info_dmx          s_gui_dmx_link_state;

/*********************
 *  GLOBAL VARIABLES
 *********************/
const osThreadAttr_t g_gui_thread_attr = { 
    .name = "app gui", 
    .priority = osPriorityLow, 
    .stack_size = 1280 * 8
};

const osThreadAttr_t g_gui_helper_thread_attr = { 
    .name = "app gui", 
    .priority = osPriorityLow, 
    .stack_size = 256*4
};
/*********************
 *  GLOBAL FUNCTIONS
 *********************/
void vApplicationTickHook(void)
{
	lv_tick_inc(1);
}

extern osSemaphoreId_t thread_sync_sigl;

void app_gui_entry(void *argument)
{
	struct sys_info_ctrl_box  ctrl_box ={0};
    struct sys_info_power sys_power;
    uint8_t data_center_ms = 0;
    struct sys_info_uuid ui_uuid;
	
	hal_gpio_init(POWER_LED1_PORT, POWER_LED1_Pin, HAL_PIN_MODE_OUT_PP);
	hal_gpio_init(POWER_LED2_PORT, POWER_LED2_Pin, HAL_PIN_MODE_OUT_PP);
    lcd_init();
    gui_init();  
	ui_test_data_init();
    os_ev_subscribe_event(MAIN_EV_DATA, EV_DATA_LIGHT_CHANGE, data_changed_callback); 
    s_gui_tmr = osTimerNew(gui_1s_callback, osTimerPeriodic,NULL, NULL);    
    osThreadNew(event_check_entry, NULL, &g_gui_helper_thread_attr);                 
    osTimerStart(s_gui_tmr, 1000);
    ui_set_dmx_temination_mode(0);
	data_center_read_config_data(SYS_CONFIG_LANG, &data_center_ms);
	ui_set_lang_type(data_center_ms);
	data_center_read_config_data(SYS_CONFIG_ETH_STATE_SET, &data_center_ms);
	ui_set_eth_universe_state(data_center_ms);
	
	for (;;)
	{
		os_ev_pull_event(MAIN_EV_DATA);
		data_center_read_sys_info(SYS_INFO_DMX, &s_gui_dmx_link_state);
        //防止关机状态下还能触发按键和旋钮的动作
		data_center_read_sys_info(SYS_INFO_CTRL, &ctrl_box);
        data_center_read_sys_info(SYS_INFO_POWER, &sys_power);
        if(sys_power.state == 0 || ctrl_box.lcd_lock)
        {
			if(s_gui_state.screen_protector.state != true)
				indev_set_lock(1);
			else
				indev_set_lock(0);
			if(sys_power.state == 1)
				page_error_info_loop();
        }
        else if(sys_power.state == 1)
        {
            indev_set_lock(0);
			page_error_info_loop();
        }
		lv_task_handler();
		if(data_center_get_uuid_write_state())
		{
			data_center_set_uuid_write_state(0);
			data_center_read_sys_info(SYS_INFO_UUID, &ui_uuid);
			if(dev_w25qxx_erase_sectors(SCANNER_G_FLASH_OFFS/0x1000, 1) != 0)
			{
				dev_w25qxx_write_data(SCANNER_G_FLASH_OFFS, ui_uuid.uuid, 20);
			}
			else
			{
				dev_w25qxx_erase_sectors((SCANNER_G_FLASH_OFFS+1024*4)/0x1000, 1);
				dev_w25qxx_write_data(SCANNER_G_FLASH_OFFS+1024*4, ui_uuid.uuid, 20);
			}
		}
		if(data_center_ms++ >= 1)
		{
			data_center_ms = 0;
			data_center_iot_handle();
		}
		if(s_gui_lfs_time_flag == 1)
		{
			s_gui_lfs_time_flag = 0;
			data_center_run_time_write(2);
		}
		if(1 == ui_ctr_clear_time_flag_get())
		{
			ui_ctr_clear_time_flag_set(0);
			data_center_run_time_clear();
		}
		menu_keys_trigger_test_page();
		gui_task_static_size = osThreadGetStackSpace(app_gui_task_id);
		freertos_task_static_size = xPortGetFreeHeapSize();
		gui_go_back_effects_ui();
        osDelay(5);
	}
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void event_check_entry(void *argument)
{
    os_ev_subscribe_event(MAIN_EV_DMX, EV_DMX_CONNECTED | EV_DMX_DISCONNECTED, dmx_event_callback);  
    os_ev_subscribe_event(MAIN_EV_BLE, OS_EVENT_BLE_CTR_LIGHT, ble_event_callback);  
    os_ev_subscribe_event(MAIN_EV_POWER, EV_LCD_POWER, power_event_callback);
    osSemaphoreRelease(thread_sync_sigl);
    while (1)
    {
		os_ev_pull_event(MAIN_EV_DMX | MAIN_EV_BLE | MAIN_EV_POWER);	        
        osDelay(20);
    }
}


/**
 * @brief DMX事件回调函数
 * 
 * @param[in]event_type     事件类
 * @param[in]event_value    事件值
 */
static void dmx_event_callback(uint32_t event_type, uint32_t event_value)
{
    if (event_value == EV_DMX_CONNECTED)
    {
        user_enter_dmx();
		ui_set_dmx_state(1);
		struct sys_info_power info;
		data_center_read_sys_info(SYS_INFO_POWER, &info);
		if(info.state == 0 && s_frist_gui_power == 0)
		{
			sys_dmx_power_on_set(1);
		}
    }
}

static void gui_lfs_data_write_handle(void)
{
	static uint8_t count_s = 0;
	static uint8_t count_divede = 0;
	struct sys_info_power sys_power;
	
	data_center_read_sys_info(SYS_INFO_POWER, &sys_power);
	if(sys_power.state == 1)
	{
		if(count_s++ >= 59)
		{
			count_s = 0;
			if(count_divede++ >= 1)  //大于2分钟则写入flash
			{
				count_divede = 0;
				s_gui_lfs_time_flag = 1;
			}
		}
	}
	else
	{
		count_s = 0;
		count_divede = 0;
	}
	if(count_s++ >= 10)
		s_frist_gui_power = 1;
}

/**
 * @brief 电源事件回调函数
 * 
 * @param[in]event_type     事件类
 * @param[in]event_value    事件值
 */
static void power_event_callback(uint32_t event_type, uint32_t event_value)
{
    if (event_type != MAIN_EV_POWER)
        return ;

    if (event_value == EV_LCD_POWER)
    {
        uint8_t power_state = os_ev_get_event_data(event_type, event_value);
        if (power_state != s_gui_state.power)
        {
            s_gui_state.power = power_state;
            bsp_lcd_set_bklight(s_gui_state.power);
        }
		if(power_state == 1)
		{
			lcd_display_on();
		}
		else if(power_state == 0)
		{
			lcd_display_off();
		}
    }
}

/**
 * @brief 蓝牙事件回调函数
 * 
 * @param[in]event_type     事件类
 * @param[in]event_value    事件值
 */
static void ble_event_callback(uint32_t event_type, uint32_t event_value)
{
    if (event_value == OS_EVENT_BLE_CTR_LIGHT)
    {
       if (s_gui_state.screen_protector.state)
           exit_screen_protector();            
    }
}

/**
 * @brief 1s 回调函数
 * 
 * @param[in]argument       定时器任务参数
 */
static void gui_1s_callback (void *argument)
{
    uint8_t usb_pluged = 0, eth_inserted = 0;    
	static uint16_t dmx_ui_cont_s = 0;
	static uint8_t  dmx_ui_cont_min = 0;
	
	//如果不在DMX界面，当DMX连接时，超过30s则进入DMX状态界面
	if((s_gui_dmx_link_state.eth_linked == 1 || s_gui_dmx_link_state.wired_linked == 1 || s_gui_dmx_link_state.wireless_linked == 1) && s_gui_state.screen_protector.state == false)
	{
		if(((screen_get_act_pid() <= PAGE_MENU || (screen_get_act_pid() >= PAGE_OUTPUT_MODE && screen_get_act_pid() <= PAGE_HIGH_SPD_MODE) || screen_get_act_pid() >= PAGE_DIMMING_CURVE)) 
		 || (PAGE_UPDATE == screen_get_act_pid() && SUB_ID_UPDATING != screen_get_act_spid()))  
		{
			if(dmx_ui_cont_s++ > 10)
			{
				user_enter_dmx();
			}
		}
		else
		{
			dmx_ui_cont_s = 0;
		}
	}
	gui_lfs_data_write_handle();
	if (indev_get_dev_input_state() || eth_inserted || usb_pluged)
    {
        indev_set_dev_input_state(0);
		dmx_ui_cont_s = 0;
		dmx_ui_cont_min = 0;
		exit_screen_protector();
    }
    else       
    {
        if (!s_gui_state.screen_protector.state)
        {
			app_gui_upgrade_timeout_handle();
			if((screen_get_act_pid() == PAGE_UPDATE && screen_get_act_spid() == SUB_ID_UPDATING) || 
			   (PAGE_WORK_MODE == screen_get_act_pid() && screen_get_act_spid() == SUB_ID_SYNC_UPDATAING))
			{
				s_gui_state.screen_protector.tmr_cnt = 0;
				return;
			}
			s_gui_state.screen_protector.tmr_cnt += 1;
			if (s_gui_state.screen_protector.tmr_cnt >= SCREEN_PROTECTOR_MS)
			{
			   s_gui_state.screen_protector.state = true;
			   user_enter_protector();    
			}
        }
    }
#ifdef LCD_POWER_TEST    
    static uint32_t cnt = 0;
    cnt++;
    if (cnt % 10 == 0)
    {
        os_ev_publish_event(MAIN_EV_POWER, EV_LCD_POWER, !s_gui_state.power);
    }
#endif    
}

static void gui_init(void)
{
    lv_init();
	lv_port_disp_init();
	lv_port_indev_init();	
    common_style_init();
    page_manager_init();
    lv_obj_set_style_bg_color(lv_scr_act(), lv_color_black(), 0);
    lv_obj_add_event_cb(lv_scr_act(), screen_event_cb, LV_EVENT_ALL, NULL);

    s_pm_timer = lv_timer_create((lv_timer_cb_t)page_manager_process, 10, NULL);    
    lv_timer_set_repeat_count(s_pm_timer, -1);        
}

static void data_changed_callback(uint32_t event_type, uint32_t event_value)
{
	if (event_type == MAIN_EV_DATA)
    {
        if (event_value == EV_DATA_LIGHT_CHANGE)
        {
            lv_timer_pause(s_pm_timer);
            page_event_send(EVENT_DATA_PULL);
            lv_timer_resume(s_pm_timer);
        }
    }    
}

static void gui_go_back_effects_ui(void)
{
	static uint8_t save_last_pid;
	static uint8_t gui_save_ui_flag;
	uint8_t pid;
	
	pid = screen_get_act_pid();
	if((((pid <= PAGE_MENU || pid >= PAGE_DIMMING_CURVE)) || (PAGE_LIGHT_EFFECT_MENU == pid || PAGE_HIGH_SPD_MODE == pid)) && gui_save_ui_flag == 1)
	{
		data_center_read_config_data(SYS_CONFIG_EFFECTS_MODE, &save_last_pid);
		uint8_t sub_id = 0;
		data_center_write_config_data_no_event(SYS_CONFIG_LOCAL_UI, &save_last_pid);
		data_center_write_config_data_no_event(SYS_CONFIG_LOCAL_SUB_UI, &sub_id);
		gui_save_ui_flag = 0;
	}
	else if(PAGE_DMX_MODE == screen_get_act_pid() && gui_save_ui_flag == 0)
	{
		gui_save_ui_flag = 1;
	}
}

static void app_gui_upgrade_timeout_handle(void)
{
	if((PAGE_UPDATE == screen_get_act_pid() && screen_get_act_spid() == SUB_ID_UPDATING) || 
	   (PAGE_WORK_MODE == screen_get_act_pid() && screen_get_act_spid() == SUB_ID_SYNC_UPDATAING))
	{
		if(ui_set_upgrade_timeout_get() > UI_UPGRADE_TIMEOUT_MAX)
		{
			ui_set_upgrade_timeout_clear();
			user_turn_to_page(PAGE_UPDATE, SUB_ID_UPDATE_SETTING, true);
			os_ev_publish_event(MAIN_EV_LIGHT, EV_DATA_LIGHT_SWITCH, 1);
		}
		ui_set_upgrade_timeout_inc();
	}
	else
	{
		ui_set_upgrade_timeout_clear();
	}
}

void exit_screen_protector(void)
{
    user_exit_proctector();
    s_gui_state.screen_protector.tmr_cnt = 0;
    s_gui_state.screen_protector.state = false;
}

