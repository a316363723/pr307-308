#include "app_power.h"
#include "os_event.h"
#include "dev_fan.h"
#include "local_data.h"
#include "dev_adapter.h"
#include <string.h>
#include "hal_adc.h"
#include "hal_timer.h"
#include "hal_iic.h"
#include "hal_gpio.h"
#include "dev_power.h"
#include "perf_counter.h"
#include "ui_data.h"
#include "ui_common.h"
#include "hal_key.h"
#include "app_gui.h"
#include "version.h"
#include "lcd.h"
#include "app_ota.h"
#include "hal_flash.h"
#include "user.h"
#include "app_err_detect.h"
#include "app_light.h"
#include "stdlib.h"
#include "app_usb.h"
#include "app_ble.h"

#define ADC_CH_NUM          (20)
#if (PROJECT_TYPE == 308)
	#define fan_speed_super_high 2600
	#define fan_speed_high 2300
	#define fan_speed_medium 1800
	#define fan_speed_silence 900
	static uint16_t fan_speed[4]={fan_speed_super_high,fan_speed_high,fan_speed_medium,fan_speed_silence}; 
#elif (PROJECT_TYPE == 307)  
	#define fan_speed_super_high 1900
	#define fan_speed_high 1700
	#define fan_speed_medium 1500
	#define fan_speed_silence 900
	static uint16_t fan_speed[4]={fan_speed_super_high,fan_speed_high,fan_speed_medium,fan_speed_silence};
#endif  
	
typedef __packed struct{
	int8_t	temperature;  
	float 	resistance;
}Rtc_Resistance;

typedef struct 
{
	struct adapter_user_info_t  user_info;
	struct adapter_output_set_t output_set;
}adapter_manage_t;


struct sys_adc
{
	float mcu_ntc;
	float acin;
	float ismon_ww1;
	float ismon_ww2;
	float dcin1;
	float dcin2;
	float sys_pg;
	float ambient_ntc;
	float ismon_cw1 ;
	float ismon_cw2 ;
	float ismon_r ;
	float ismon_g ;
	float ismon_b ;
	float v_sys;
	float ac_mode;
};

struct sys_fan
{
	uint16_t  speed;
	uint8_t   level;
	uint16_t  speed_set;
	uint16_t  lamp_fan;
	bool  force;
	bool   error;
};

struct sys_ntc
{
	int8_t mcu_temp;
    int8_t ambient_temp;
};

typedef struct
{
    bool key_state;
	bool turn_on; 
	uint8_t start_step;
	uint16_t total_power;
	uint16_t adc_dma_data[3][ADC_CH_NUM];
    struct sys_ntc ntc;
	struct sys_fan fan;
	struct sys_adc adc;
	struct sys_info_power info;
	float  ac_v_check;
}power_manage_t;

static void     app_adc_capture_10ms_callback(void);
static void     app_power_type_set(void);
static uint16_t app_power_fan_speed_get(void);
static void     app_power_fan_per_500ms_handle(void *argument);
//static app_power_get_dc_error(void);
static void     app_power_start_process(void);
static void     app_power_evnet_pull_cb(uint32_t event_type, uint32_t event_value);
static float    app_power_binary_search_temperature(const Rtc_Resistance * data, uint16_t length, float key);
static void     app_power_adc_dma1_bct_back(void);
static void     app_power_adc_dma3_bct_back(void);
static void     app_power_total_power_count(void);
static void     app_power_state_set(uint8_t state);
static uint16_t app_power_fan_speed_value(void);
uint8_t app_power_get_voltage_value(void);

osTimerId_t timer_power_handle;
static osTimerId_t 		  s_fan_tmr;
struct sys_info_ctrl_box  g_ctrl_box;
extern osSemaphoreId_t thread_sync_sigl;
power_manage_t g_power = {
    .key_state = 1,
	.total_power = 0,
	.turn_on = 0,
	.start_step = 0,	
	.fan.speed_set = 1000,
	.fan.error = 1,
};

const Rtc_Resistance _3380_resistance[] = {
	{ -40     ,208.15 },{ -39     ,196.57 },{ -38     ,185.71 },{ -37     ,175.52 },{ -36     ,165.96 },
	{ -35     ,156.97 },{ -34     ,148.53 },{ -33     ,140.60 },{ -32     ,133.14 },{ -31     ,126.12 },
	{ -30     ,119.52 },{ -29     ,113.30 },{ -28     ,107.45 },{ -27     ,101.93 },{ -26     ,96.73  },
	{ -25     ,91.83  },{ -24     ,87.21  },{ -23     ,82.85  },{ -22     ,78.73  },{ -21     ,74.85  },
	{ -20     ,71.18  },{ -19     ,67.71  },{ -18     ,64.43  },{ -17     ,61.33  },{ -16     ,58.40  },
	{ -15     ,55.62  },{ -14     ,53.00  },{ -13     ,50.51  },{ -12     ,48.16  },{ -11     ,45.93  },
	{ -10     ,43.81  },{ -9      ,41.81  },{ -8      ,39.91  },{ -7      ,38.11  },{ -6      ,36.40  },
	{ -5      ,34.77  },{ -4      ,33.23  },{ -3      ,31.77  },{ -2      ,30.38  },{ -1      ,29.05  },
	{ 0       ,27.80  },{ 1       ,26.60  },{ 2       ,25.46  },{ 3       ,24.38  },{ 4       ,23.35  },
	{ 5       ,22.37  },{ 6       ,21.44  },{ 7       ,20.55  },{ 8       ,19.70  },{ 9       ,18.90  },
	{ 10      ,18.13  },{ 11      ,17.39  },{ 12      ,16.69  },{ 13      ,16.02  },{ 14      ,15.39  },
	{ 15      ,14.78  },{ 16      ,14.20  },{ 17      ,13.64  },{ 18      ,13.11  },{ 19      ,12.61  },
	{ 20      ,12.12  },{ 21      ,11.66  },{ 22      ,11.22  },{ 23      ,10.79  },{ 24      ,10.39  },
	{ 25      ,10     },{ 26      ,9.63   },{ 27      ,9.27   },{ 28      ,8.93   },{ 29      ,8.61   },
	{ 30      ,8.30   },{ 31      ,8.00   },{ 32      ,7.71   },{ 33      ,7.43   },{ 34      ,7.17   },
	{ 35      ,6.92   },{ 36      ,6.67   },{ 37      ,6.44   },{ 38      ,6.22   },{ 39      ,6.00   },
	{ 40      ,5.80   },{ 41      ,5.60   },{ 42      ,5.41   },{ 43      ,5.23   },{ 44      ,5.05   },
	{ 45      ,4.88   },{ 46      ,4.72   },{ 47      ,4.57   },{ 48      ,4.42   },{ 49      ,4.27   },
	{ 50      ,4.13   },{ 51      ,4.00   },{ 52      ,3.87   },{ 53      ,3.75   },{ 54      ,3.63   },
	{ 55      ,3.51   },{ 56      ,3.40   },{ 57      ,3.30   },{ 58      ,3.19   },{ 59      ,3.09   },
	{ 60      ,3.00   },{ 61      ,2.91   },{ 62      ,2.82   },{ 63      ,2.73   },{ 64      ,2.65   },
	{ 65      ,2.57   },{ 66      ,2.49   },{ 67      ,2.42   },{ 68      ,2.35   },{ 69      ,2.28   },
	{ 70      ,2.21   },{ 71      ,2.15   },{ 72      ,2.09   },{ 73      ,2.03   },{ 74      ,1.97   },
	{ 75      ,1.91   },{ 76      ,1.86   },{ 77      ,1.80   },{ 78      ,1.75   },{ 79      ,1.70   },
	{ 80      ,1.66   },{ 81      ,1.61   },{ 82      ,1.57   },{ 83      ,1.52   },{ 84      ,1.48   },
	{ 85      ,1.44   },{ 86      ,1.40   },{ 87      ,1.37   },{ 88      ,1.33   },{ 89      ,1.29   },
	{ 90      ,1.26   },{ 91      ,1.23   },{ 92      ,1.19   },{ 93      ,1.16   },{ 94      ,1.13   },
	{ 95      ,1.10   },{ 96      ,1.07   },{ 97      ,1.05   },{ 98      ,1.02   },{ 99      ,0.99   },
	{ 100     ,0.97   },{ 101     ,0.95   },{ 102     ,0.92   },{ 103     ,0.90   },{ 104     ,0.88   },
	{ 105     ,0.85   },{ 106     ,0.83   },{ 107     ,0.81   },{ 108     ,0.79   },{ 109     ,0.77   },
	{ 110     ,0.76   },{ 111     ,0.74   },{ 112     ,0.72   },{ 113     ,0.70   },{ 114     ,0.69   },
	{ 115     ,0.67   },{ 116     ,0.65   },{ 117     ,0.64   },{ 118     ,0.62   },{ 119     ,0.61   },
	{ 120     ,0.60   },{ 121     ,0.58   },{ 122     ,0.57   },{ 123     ,0.56   },{ 124 ,   0.54    },
	{ 125     ,0.53   }
};

const Rtc_Resistance _100k_3950_resistance[] = {
	
	{ -30       ,1787.9797  },{ -29       ,1679.6017  },{ -28       ,1578.5061  },{ -27       ,1484.1584  },{ -26       ,1396.0662  },
	{ -25       ,1313.7754  },{ -24       ,1236.8685  },{ -23       ,1164.9598  },{ -22       ,1097.6941  },{ -21       ,1034.7432  },
	{ -20       ,975.8038   },{ -19       ,920.5962   },{ -18       ,868.8615   },{ -17       ,820.3603   },{ -16       ,774.871    },
	{ -15       ,732.1889   },{ -14       ,692.1238   },{ -13       ,654.4999   },{ -12       ,619.154    },{ -11       ,585.9346   },
	{ -10       ,554.7016   },{ -9        ,525.3245   },{ -8        ,497.6821   },{ -7        ,471.6621   },{ -6        ,447.1599   },
	{ -5        ,424.0781   },{ -4        ,402.3264   },{ -3        ,381.8204   },{ -2        ,362.4818   },{ -1        ,344.2375   },
	{ 0         ,327.0195   },{ 1         ,310.764    },{ 2         ,295.4121   },{ 3         ,280.9084   },{ 4         ,267.2014   },
	{ 5         ,254.2428   },{ 6         ,241.9877   },{ 7         ,230.394    },{ 8         ,219.4224   },{ 9         ,209.0361   },
	{ 10        ,199.2007   },{ 11        ,189.8841   },{ 12        ,181.0559   },{ 13        ,172.6881   },{ 14        ,164.754    },
	{ 15        ,157.229    },{ 16        ,150.0898   },{ 17        ,143.3144   },{ 18        ,136.8825   },{ 19        ,130.7749   },
	{ 20        ,124.9734   },{ 21        ,119.4612   },{ 22        ,114.2223   },{ 23        ,109.2417   },{ 24        ,104.5053   },
	{ 25        ,100        },{ 26        ,95.7132    },{ 27        ,91.6333    },{ 28        ,87.7492    },{ 29        ,84.0505    },
	{ 30        ,80.5274    },{ 31        ,77.1707    },{ 32        ,73.9717    },{ 33        ,70.9222    },{ 34        ,68.0144    },
	{ 35        ,65.2411    },{ 36        ,62.5954    },{ 37        ,60.0707    },{ 38        ,57.661     },{ 39        ,55.3604    },
	{ 40        ,53.1635    },{ 41        ,51.0651    },{ 42        ,49.0602    },{ 43        ,47.1443    },{ 44        ,45.313     },
	{ 45        ,43.5621    },{ 46        ,41.8878    },{ 47        ,40.2862    },{ 48        ,38.7539    },{ 49        ,37.2876    },
	{ 50        ,35.8842    },{ 51        ,34.5405    },{ 52        ,33.2538    },{ 53        ,32.0214    },{ 54        ,30.8408    },
	{ 55        ,29.7096    },{ 56        ,28.6253    },{ 57        ,27.586     },{ 58        ,26.5895    },{ 59        ,25.6338    },
	{ 60        ,24.7171    },{ 61        ,23.8376    },{ 62        ,22.9937    },{ 63        ,22.1836    },{ 64        ,21.4061    },
	{ 65        ,20.6594    },{ 66        ,19.9424    },{ 67        ,19.2537    },{ 68        ,18.592     },{ 69        ,17.9562    },
	{ 70        ,17.3452    },{ 71        ,16.7578    },{ 72        ,16.193     },{ 73        ,15.6499    },{ 74        ,15.1276    },
	{ 75        ,14.6251    },{ 76        ,14.1417    },{ 77        ,13.6764    },{ 78        ,13.2286    },{ 79        ,12.7976    },
	{ 80        ,12.3825    },{ 81        ,11.9828    },{ 82        ,11.5978    },{ 83        ,11.227     },{ 84        ,10.8697    },
	{ 85        ,10.5254    },{ 86        ,10.1935    },{ 87        ,9.8736     },{ 88        ,9.5652     },{ 89        ,9.2678     },
	{ 90        ,8.9809     },{ 91        ,8.7042     },{ 92        ,8.4373     },{ 93        ,8.1797     },{ 94        ,7.9312     },
	{ 95        ,7.6912     },{ 96        ,7.4596     },{ 97        ,7.236      },{ 98        ,7.0201     },{ 99        ,6.8115     },
	{ 100       ,6.6101     },{ 101       ,6.4155     },{ 102       ,6.2274     },{ 103       ,6.0457     },{ 104       ,5.8701     },
	{ 105       ,5.7003     },{ 106       ,5.5362     },{ 107       ,5.3775     },{ 108       ,5.224      },{ 109       ,5.0755     },
	{ 110       ,4.9319     },{ 111       ,4.793      },{ 112       ,4.6586     },{ 113       ,4.5285     },{ 114       ,4.4026     },
	{ 115       ,4.2807     },{ 116       ,4.1627     },{ 117       ,4.0484     },{ 118       ,3.9378     },{ 119       ,3.8306     },
	{ 120       ,3.7268     },{ 121       ,3.6263     },{ 122       ,3.5289     },
};




const osThreadAttr_t g_power_thread_attr =
{
    .name = "app power",
    .priority = osPriorityAboveNormal1,
    .stack_size = 384 * 4
};
osThreadId_t app_power_thread_task_id;
uint32_t power_task_static_size;
static uint8_t s_first_power_limit = 0;

void app_power_thread_entrance(void *argument)
{
	uint8_t hw_driver_error;
	uint8_t adc_star_count = 0;
	uint8_t power_frist_on = 1;
	uint8_t power_delay_on = 0;
    key_gpio_init();
    hal_adc_init(HAL_ADC_1);
    hal_adc_init(HAL_ADC_3);

    hal_adc_irq_register(HAL_ADC_1, HAL_ADC_IRQ_BTC, app_power_adc_dma1_bct_back);
    hal_adc_irq_register(HAL_ADC_3, HAL_ADC_IRQ_BTC, app_power_adc_dma3_bct_back);

    hal_adc_receive_dma_start(HAL_ADC_1, (uint32_t*)&g_power.adc_dma_data[0][0], 0);
    hal_adc_receive_dma_start(HAL_ADC_3, (uint32_t*)&g_power.adc_dma_data[2][0], 0);
    dev_fan_init();
  
    osSemaphoreRelease(thread_sync_sigl);
    adapter_a_and_b_ctrl(ADAPTER_A_AND_B_EN );

    hw_driver_error = dev_power.power_boost_up(1);
    os_ev_subscribe_event(MAIN_EV_BLE, OS_EVENT_BLE_POWER_ON_OFF, app_power_evnet_pull_cb);
    s_fan_tmr = osTimerNew(app_power_fan_per_500ms_handle, osTimerPeriodic, NULL, NULL);                 
    osTimerStart(s_fan_tmr, 500);
	
	ui_max_power_flag_set(0);
    while(1)
    {
		os_ev_pull_event(MAIN_EV_BLE);
        key_poll();
		if(adc_star_count++ >= 2)
		{
			adc_star_count = 0;
			app_adc_capture_10ms_callback();
		}
		if(power_frist_on == 1)
		{
			if(power_delay_on++ > 100)
			{
				power_delay_on = 0;
				power_frist_on = 0;
			}
		}
		else
		{
			app_power_total_power_count();
			app_power_type_set();
			app_power_start_process();
		}
        if(g_power.key_state == 1)//关机状态时不能进入屏保界面
        {
            exit_screen_protector();
        }
		if(hw_driver_error == 0)
		{
			error_functionArray[hard_drive_ec_005](1);
		}
		data_center_write_sys_info(SYS_INFO_CTRL, &g_ctrl_box);
		power_task_static_size = osThreadGetStackSpace(app_power_thread_task_id);
        osDelay(10);
    }
}

void hal_self_adapt_all_pwm_set(uint16_t value)
{
    for(uint8_t i = HAL_PWM_SELF_ADAPTION_R; i< HAL_PWM_SELF_ADAPTION_CW2+1 ;i++)
    {
        hal_timer_duty_cycle_set((enum hal_timera_enum)i,value);
    }
}

void power_init(void)
{
   dev_power.power_manage_init();
}

void app_power_iic_send_all_data(bool state)
{	
	uint8_t i2c_buff[2] = {0};
	if(state)
	{
		i2c_buff[0] = 0xf;
		i2c_buff[1] = 0xff;
	}
    hal_iic_send_data(HAL_IIC_ANALOG_WW1_WW2_B,IIC_WW1_ADDRESS,(uint8_t* )i2c_buff,2);   
	osDelay(20);
    hal_iic_send_data(HAL_IIC_ANALOG_WW1_WW2_B,IIC_WW2_ADDRESS,(uint8_t* )i2c_buff,2); 
	osDelay(20);
    hal_iic_send_data(HAL_IIC_ANALOG_WW1_WW2_B,IIC_B_ADDRESS,(uint8_t* )i2c_buff,2); 
	osDelay(20);
    hal_iic_send_data(HAL_IIC_ANALOG_CW1_CW2_R_G,IIC_CW1_ADDRESS,(uint8_t* )i2c_buff,2); 
	osDelay(20);
    hal_iic_send_data(HAL_IIC_ANALOG_CW1_CW2_R_G,IIC_CW2_ADDRESS,(uint8_t* )i2c_buff,2); 
	osDelay(20);
    hal_iic_send_data(HAL_IIC_ANALOG_CW1_CW2_R_G,IIC_R_ADDRESS,(uint8_t* )i2c_buff,2); 
	osDelay(20);
    hal_iic_send_data(HAL_IIC_ANALOG_CW1_CW2_R_G,IIC_G_ADDRESS,(uint8_t* )i2c_buff,2); 
	osDelay(20);
}

void app_power_iic_send_all_data_no_wait(bool state)
{
	uint8_t i2c_buff[2] = {0};
	
	if(state)
	{
		i2c_buff[0] = 0xf;
		i2c_buff[1] = 0xff;
	}
    hal_iic_send_data(HAL_IIC_ANALOG_WW1_WW2_B,IIC_WW1_ADDRESS,(uint8_t* )i2c_buff,2);   
    hal_iic_send_data(HAL_IIC_ANALOG_WW1_WW2_B,IIC_WW2_ADDRESS,(uint8_t* )i2c_buff,2); 
    hal_iic_send_data(HAL_IIC_ANALOG_WW1_WW2_B,IIC_B_ADDRESS,(uint8_t* )i2c_buff,2); 
    hal_iic_send_data(HAL_IIC_ANALOG_CW1_CW2_R_G,IIC_CW1_ADDRESS,(uint8_t* )i2c_buff,2); 
    hal_iic_send_data(HAL_IIC_ANALOG_CW1_CW2_R_G,IIC_CW2_ADDRESS,(uint8_t* )i2c_buff,2); 
    hal_iic_send_data(HAL_IIC_ANALOG_CW1_CW2_R_G,IIC_R_ADDRESS,(uint8_t* )i2c_buff,2); 
    hal_iic_send_data(HAL_IIC_ANALOG_CW1_CW2_R_G,IIC_G_ADDRESS,(uint8_t* )i2c_buff,2); 
}

void sys_box_lcd_lock(uint8_t value)
{
	g_ctrl_box.lcd_lock = value;
}

void sys_ctrl_mode_set(uint8_t mode)
{
	g_ctrl_box.mode = mode;
}

void sys_hw_ver_set(void)
{
	g_ctrl_box.hw_ver[0] = HARDWARE_VERSION;
}

void sys_soft_ver_set(void)
{
	g_ctrl_box.soft_ver[0] = SOFTWARE_VERSION;
}

void sys_pan_angle_set(uint16_t pan_angle)
{
	g_ctrl_box.pan_angle = pan_angle;
}

void sys_till_angle_set(uint16_t till_angle)
{
	g_ctrl_box.till_angle = till_angle;
}

void sys_scroll_angle_set(uint16_t scroll_angle)
{
	g_ctrl_box.scroll_angle = scroll_angle;
}

void sys_dmx_power_on_set(uint8_t mode)
{
	g_power.turn_on = mode;
}

uint16_t get_total_power(void)
{
	return g_power.total_power ;
}

static void app_adc_capture_10ms_callback(void)
{
    adc_capture_start_it(HAL_ADC_1);
    adc_capture_start_it(HAL_ADC_3);
}

static void app_power_type_set(void)
{
	uint16_t diff;
	
	float acin[4] = { g_power.adc.acin ,g_power.adc.dcin1,g_power.adc.dcin2,g_power.ac_v_check};
	struct sys_info_power info_power;
	sys_config_t cofig_pack;
	static uint16_t last_power_ratio;
	static bool power_chage = 0;
	static uint16_t filterms[6][4] = {0};
	static float power_radio = 0.0;
	uint8_t hs_mode;
	#if PROJECT_TYPE==307
	struct sys_info_accessories access_info;
	uint16_t indoor_power[3] = {10000,10000,5789};    
	uint16_t indoor_power_hs[3] = {10000,10000,5989};
	#elif PROJECT_TYPE==308
	uint16_t indoor_power[3] = {10000,5300,3600};
	uint16_t indoor_power_hs[3] = {10000,5166,3200};
	#endif 

	if(acin[3] > 1.4f)    //室内高压降到1700W
	{
		if(ui_get_power_type() ==  POWER_TYPE_DC) 
		{
			filterms[5][0]++;
			filterms[5][1] = 0;
			filterms[5][2] = 0;
			if(filterms[5][0] > 10)
			{
				
				filterms[5][0] = 0;
				data_center_read_config_data(SYS_CONFIG_HIGH_SPEED_MODE, &hs_mode);
				if(s_first_power_limit == 0)
				{
					s_first_power_limit = 1;
					#if PROJECT_TYPE==308
					gui_set_power_limit(1500);
					#endif
					#if PROJECT_TYPE==307
					//gui_set_power_limit(1600);
					#endif
				}
				power_radio = gui_get_power_limit();
				if(hs_mode == 0)
				{
					#if PROJECT_TYPE==308
					g_power.info.ratio = ((float)power_radio / 2600.0f) * 10000.0;
					#elif PROJECT_TYPE==307
					g_power.info.ratio =  10000.0f;
					#endif
				}
				else if(hs_mode == 1)
				{
					#if PROJECT_TYPE==308
					g_power.info.ratio = ((float)power_radio / 2600.0f) * 10000.0f;
					#elif PROJECT_TYPE==307
					g_power.info.ratio = 10000.0f;
					#endif
				}
				
				data_center_read_sys_info(SYS_INFO_POWER, &info_power);
				info_power.indoor_powe = 1;
				data_center_write_sys_info(SYS_INFO_POWER, &info_power);
			}
		}
	}
	else if(acin[3] > 0.4 && acin[3] < 1.3f)   //室内低压降到1200W
	{
		if(ui_get_power_type() ==  POWER_TYPE_DC) 
		{
			filterms[5][1]++;
			filterms[5][0] = 0;
			filterms[5][2] = 0;
			if(filterms[5][1] > 10)
			{
				filterms[5][1] = 0;
				data_center_read_config_data(SYS_CONFIG_HIGH_SPEED_MODE, &hs_mode);
				
				if(s_first_power_limit == 0)
				{
					s_first_power_limit = 1;
					gui_set_power_limit(1000);
				}
				power_radio = gui_get_power_limit();
				if(hs_mode == 0)
				{
					#if PROJECT_TYPE==308
					g_power.info.ratio = (power_radio / 2600.0f) * 10000;
					#elif PROJECT_TYPE==307
					g_power.info.ratio = (power_radio / 1600.0f) * 10000;
					#endif
				}
				else if(hs_mode == 1)
				{
					#if PROJECT_TYPE==308
					g_power.info.ratio = (power_radio / 2600.0f) * 10000;
					#elif PROJECT_TYPE==307
					g_power.info.ratio = (power_radio / 1600.0f) * 10000;
					#endif
				}
			}
			data_center_read_sys_info(SYS_INFO_POWER, &info_power);
			info_power.indoor_powe = 2;
			data_center_write_sys_info(SYS_INFO_POWER, &info_power);

		}
	}
	else if(acin[3] < 0.3f)   //室外满功率
	{
		if(ui_get_power_type() == POWER_TYPE_DC)
		{			
			filterms[5][2]++;
			filterms[5][1] = 0;
			filterms[5][0] = 0;
			if(filterms[5][2] > 10)
			{
				filterms[5][2] = 0;
				data_center_read_config_data(SYS_CONFIG_HIGH_SPEED_MODE, &hs_mode);
				if(hs_mode == 0)
				{
					g_power.info.ratio = indoor_power[0];
				}
				else if(hs_mode == 1)
				{
					g_power.info.ratio = indoor_power_hs[0];
				}
				#if PROJECT_TYPE==307
				data_center_read_sys_info(SYS_INFO_ACCESS, &access_info);
				if(access_info.fresnel_state == 1 && access_info.yoke_state == 1)
					g_power.info.ratio = 9000;
				else if(access_info.fresnel_state == 1 && access_info.yoke_state == 0)
					g_power.info.ratio = 9500;
				#endif
				data_center_read_sys_info(SYS_INFO_POWER, &info_power);
				info_power.indoor_powe = 0;
				data_center_write_sys_info(SYS_INFO_POWER, &info_power);
			}
		}
	}
	else
	{
		filterms[5][0] = 0;
		filterms[5][1] = 0;
		filterms[5][2] = 0;
	}
	if(acin[0] > 0.5)
	{	
		filterms[0][0]++;
		if(filterms[0][0]> 100)
		{
			filterms[1][0] = 0;
			filterms[2][0] = 0;
			filterms[3][0] = 0;
			filterms[1][1] = 0;
			filterms[4][0] = 0;
			filterms[0][0] = 100;	
			error_functionArray[dc_unbalanced_power_ec_013](0);
			ui_set_power_type(POWER_TYPE_DC);
		    ui_set_box_type(0);
			power_chage = 1;
			goto out;
		}
		return ;
	}
	else if(acin[1] > 0.5 && acin[2] > 0.5)
	{
		
		filterms[1][0]++;
		diff = abs((uint16_t)(acin[1]) - (uint16_t)(acin[2]));
		if(filterms[1][0] > 100)
		{
			filterms[0][0] = 0;
			filterms[2][0] = 0;
			filterms[3][0] = 0;
			filterms[4][0] = 0;
			filterms[1][0] = 100;	
			if(diff >= 2)
			{
				filterms[1][1]++;
				filterms[1][2] = 0;
			}
			else
			{
				filterms[1][1] = 0;
				filterms[1][2] ++ ;
			}
			if(filterms[1][1]> 200)
			{
				power_chage = 1;
				filterms[1][1] = 0;
				g_power.info.ratio = 5000;
				error_functionArray[dc_unbalanced_power_ec_013](1);
				
			}
			if(filterms[1][2] > 20)
			{
				power_chage = 1;
				g_power.info.ratio = 10000;
				filterms[1][2] = 0;
			}
			ui_set_power_type(POWER_TYPE_SUPPLY_BOX);
			ui_set_box_type(0);
		
		
			data_center_read_sys_info(SYS_INFO_POWER, &info_power);
			info_power.indoor_powe = 0;
			data_center_write_sys_info(SYS_INFO_POWER, &info_power);
			
			goto out;
			
		}
		return;
	}
	else if(acin[1] > 0.5 && acin[2] < 0.5)
	{
		
		filterms[2][0]++ ;
		if(filterms[2][0]> 100)
		{
			filterms[3][0] = 0;
			filterms[1][1] = 0;
			filterms[4][0] = 0;
			filterms[0][0] = 0;
			filterms[1][0] = 0;
			filterms[2][0] = 100;
			error_functionArray[dc_unbalanced_power_ec_013](0);
			ui_set_power_type(POWER_TYPE_SUPPLY_BOX);
			ui_set_box_type(1);
			g_power.info.ratio = 5000;
			power_chage = 1;
			data_center_read_sys_info(SYS_INFO_POWER, &info_power);
			info_power.indoor_powe = 0;
			data_center_write_sys_info(SYS_INFO_POWER, &info_power);
			goto out;
		}
		return ;
	}
	else if(acin[1] < 0.5 && acin[2] > 0.5)
	{
	
		filterms[3][0] ++;
	
		if(filterms[3][0]> 100)
		{
			filterms[1][1] = 0;
			filterms[4][0] = 0;	
			filterms[0][0] = 0;
			filterms[1][0] = 0;
			filterms[2][0] = 0;
			filterms[3][0] = 100;
			g_power.info.ratio = 5000;
			error_functionArray[dc_unbalanced_power_ec_013](0);
			ui_set_power_type(POWER_TYPE_SUPPLY_BOX);
			ui_set_box_type(2);
			power_chage = 1;
			data_center_read_sys_info(SYS_INFO_POWER, &info_power);
			info_power.indoor_powe = 0;
			data_center_write_sys_info(SYS_INFO_POWER, &info_power);
			goto out;
		}
		return ;
	}
	else
	{
	
		filterms[4][0] ++;
		if(filterms[4][0]> 10)
		{	
			filterms[0][0] = 0;
			filterms[1][0] = 0;
			filterms[2][0] = 0;
			filterms[3][0] = 0;
			filterms[1][1] = 0;
			filterms[4][0] = 10;
			goto out;
		}
	}
	out:
		if(power_chage == 0)return;
		if(g_power.info.ratio == last_power_ratio) return;
		last_power_ratio = g_power.info.ratio;
		data_center_read_sys_info(SYS_INFO_POWER, &info_power);
		info_power.ratio = last_power_ratio;
		data_center_write_sys_info(SYS_INFO_POWER, &info_power);
		data_center_read_config_data(SYS_CONFIG_OUTPUT_MODE, &cofig_pack.output_mode);
		data_center_write_config_data_no_event(SYS_CONFIG_OUTPUT_MODE, &cofig_pack.output_mode);
		app_light_flags_set(LIGHT_RS485_OUTPUT_MODE_FLAG);
}

static uint16_t app_power_fan_speed_get(void)
{
    uint16_t res;
	sys_config_t cofig_pack;
	
    enum fan_speed_level
    {
        FAN_SPEED_LEVEL_0 = 0,
        FAN_SPEED_LEVEL_1,
        FAN_SPEED_LEVEL_2,
		FAN_SPEED_LEVEL_3,
        FAN_SPEED_LEVEL_MAX,
    } level ;
    data_center_read_config_data(SYS_CONFIG_FAN_MODE, &cofig_pack.fan_mode);
	if(cofig_pack.fan_mode == 0)
	{	
		if(g_ctrl_box.ambient_temp < 25){level = FAN_SPEED_LEVEL_1;}
		else if(g_ctrl_box.ambient_temp <= 35){level = FAN_SPEED_LEVEL_2;}
		else if(g_ctrl_box.ambient_temp <= 45){level = FAN_SPEED_LEVEL_3;}
		else{level = FAN_SPEED_LEVEL_MAX;}
		res = fan_speed[3] + ((fan_speed[0] - fan_speed[3]) / (FAN_SPEED_LEVEL_MAX )) * (level - 1);
		return res;
	}
	else if(cofig_pack.fan_mode == 1)
	{
		res = fan_speed[1];
	}
	else if(cofig_pack.fan_mode == 2)
	{
		res = fan_speed[2];
	}
	else if(cofig_pack.fan_mode == 3)
	{
		res = fan_speed[3];
	}
	else
	{
		res = fan_speed[1];
	}
	
	return res;
}

/**
 * @brief 1s 回调函数
 * 
 * @param[in]argument       定时器任务参数
 */

static void app_power_fan_per_500ms_handle(void *argument)
{
    static uint16_t fan_err_ticks;
	static uint16_t last_fan_irq_count[2] = {0};
	static uint8_t delay_count[2] = {0};
	struct sys_info_accessories access_info;
	sys_error_type error_code;
	
	g_power.ntc.mcu_temp = app_power_binary_search_temperature(_3380_resistance, sizeof(_3380_resistance) / sizeof(Rtc_Resistance), g_power.adc.mcu_ntc);
	g_power.ntc.ambient_temp  =  app_power_binary_search_temperature( _100k_3950_resistance, sizeof(_100k_3950_resistance) / sizeof(Rtc_Resistance),g_power.adc.ambient_ntc ) ;
	g_ctrl_box.ambient_temp = g_power.ntc.ambient_temp ;
	g_ctrl_box.mcu_temp = g_power.ntc.mcu_temp ;
    if(dev_fan_error_get() == 1)
    {
        fan_err_ticks = 0;
		g_power.fan.error = 0;
		error_functionArray[fan_ec_010](0);
    }
	
	if(g_power.ntc.mcu_temp > 75 )
	{
		error_functionArray[mcu_temp_ec_002](1);
		os_ev_publish_event(MAIN_EV_LIGHT, EV_DATA_LIGHT_SWITCH, 0);
	}
	else
	{
		error_functionArray[mcu_temp_ec_002](0);
	}
    if(!g_power.fan.error)
    {
        if(fan_err_ticks++ > 19)
        {
            g_power.fan.error = 1;
            g_power.fan.speed_set = 0;
			os_ev_publish_event(MAIN_EV_LIGHT, EV_DATA_LIGHT_SWITCH, 0);
			error_functionArray[fan_ec_010](1);
        }
    }
    
	if(g_power.key_state== 0)//开机风扇转动，关机风扇停止
	{
		if(g_ctrl_box.ambient_temp < 0 || g_ctrl_box.ambient_temp > 45)
		{
			delay_count[1] = 0;
			delay_count[0]++;
			data_center_read_sys_info(SYS_INFO_ACCESS, &access_info);
			if(delay_count[0] > 10 && access_info.fresnel_state == 1)
			{
				delay_count[0] = 0;
				data_center_read_sys_info(SYS_INFO_ERROR_TYPE, &error_code); 
				error_code.error_eh.motorized_f14_ambient_temp_eh_021 = 1;
				data_center_write_sys_info(SYS_INFO_ERROR_TYPE, &error_code); 
			}
			else if(access_info.fresnel_state == 0)
			{
				delay_count[0] = 0;
				data_center_read_sys_info(SYS_INFO_ERROR_TYPE, &error_code); 
				error_code.error_eh.motorized_f14_ambient_temp_eh_021 = 0;
				data_center_write_sys_info(SYS_INFO_ERROR_TYPE, &error_code); 
			}
		}
		else
		{
			delay_count[1]++;
			if(delay_count[1] > 10)
			{
				delay_count[1] = 0;
				data_center_read_sys_info(SYS_INFO_ERROR_TYPE, &error_code); 
				error_code.error_eh.motorized_f14_ambient_temp_eh_021 = 0;
				data_center_write_sys_info(SYS_INFO_ERROR_TYPE, &error_code); 
			}
			delay_count[0] = 0;
		}
		if(dev_fan_irq_count_get() == last_fan_irq_count[0])
		{
			if((last_fan_irq_count[1]++ > 12) && (!g_power.fan.error))
			{
				last_fan_irq_count[1] = 0;
				g_power.fan.error = 1;
				dev_fan_set_speed(&fan, 0);
				dev_fan_error_set(0);
				os_ev_publish_event(MAIN_EV_LIGHT, EV_DATA_LIGHT_SWITCH, 0);
				error_functionArray[fan_ec_010](1);
			}
		}
		else
		{
			last_fan_irq_count[1] = 0;
		}
		last_fan_irq_count[0] = dev_fan_irq_count_get();
		if(g_power.fan.speed_set == 0 )
		{
			dev_fan_set_speed(&fan, g_power.fan.speed_set);
			g_power.fan.error = 0;
		}
		else
		{
			if (screen_get_act_pid() == PAGE_TEST || screen_get_act_pid() == PAGE_CALIBRATION_MODE)
			{
				g_power.fan.speed_set = ui_test_box_fan_get() > 800 ? ui_test_box_fan_get():800;
				g_ctrl_box.fan_speed = fan.real_fan_speed_read();
				dev_fan_set_speed(&fan, g_power.fan.speed_set);
			}
			else
			{
				g_power.fan.speed_set = app_power_fan_speed_get();
				dev_fan_set_speed(&fan, g_power.fan.speed_set);
			}
		}
		
		dev_fan_polling_fun(&fan, NULL);
	}
	else
	{
		last_fan_irq_count[1] = 0;
		dev_fan_set_speed(&fan, 0);
		g_power.fan.speed_set = 1000;
	}
}

static void app_power_start_process(void)
{
	static uint8_t studio_mode;
	static uint8_t effects_mode;
	static uint16_t adc_off_s1[2] = {0};
	sys_error_type error_code;
	static uint8_t laco_save_flag = 0;
	
	switch(g_power.start_step)
	{
		case 0:
		{
			uint8_t page_id = 0;
			data_center_read_config_data(SYS_CONFIG_LOCAL_UI, &page_id);
			data_center_read_config_data(SYS_CONFIG_STUDIO_MODE, &studio_mode);
			if(LaunchStatus == STATUS_OTA_SUCCESS)
				studio_mode = STUDIO_MODE_ON;
			LaunchStatus = 0;
			if(PAGE_DMX_MODE == page_id || studio_mode == STUDIO_MODE_ON)   
			{
				g_power.key_state = 1;
				g_power.turn_on =1;
			}
			g_power.start_step = 1;
		}
		break;
		case 1:
		{
			if( true == get_Key_event(KEY_ON_OFF, KEY_PRESSED) || g_power.turn_on == 1)
			{
				if((PAGE_UPDATE == screen_get_act_pid() && screen_get_act_spid() == SUB_ID_UPDATING) || 
				   (PAGE_WORK_MODE == screen_get_act_pid() && screen_get_act_spid() == SUB_ID_SYNC_UPDATAING))
				{
					return;
				}
				if(g_power.turn_on == 0)
				{
					if(ui_get_master_mode())
					{
						gui_power_event_gen();
						effects_mode = date_center_get_light_mode();
						if(effects_mode != LIGHT_MODE_DMX)
							gui_data_sync_event_gen();
					}
				}
				sys_box_lcd_lock(0); 
				g_power.turn_on =0;
				data_center_read_config_data(SYS_CONFIG_LANG, &studio_mode);
				ui_set_lang_type(studio_mode);
				app_power_state_set(g_power.key_state);
				data_center_read_sys_info(SYS_INFO_POWER, &g_power.info);
				g_power.info.state = !g_power.key_state;
				data_center_write_sys_info(SYS_INFO_POWER, &g_power.info);  
				data_center_read_sys_info(SYS_INFO_ERROR_TYPE, &error_code);
				error_code.error_data = 0;
				data_center_write_sys_info(SYS_INFO_ERROR_TYPE, &error_code);
			}
			if(g_power.adc.v_sys < 45.0f)   
			{
				if(laco_save_flag == 0)
				{
					laco_save_flag = 1;
					osThreadSuspend(app_usb_task_id);
					osThreadSuspend(app_ble_task_id);
					osThreadSuspend(app_gui_task_id);
					data_center_local_handle();
				}
				if(adc_off_s1[0]++ > 3)
				{
					adc_off_s1[0] = 0;
					if(g_power.adc.v_sys < 45.0f) 
					{
						adc_off_s1[1] = 0;
						app_power_state_set(0);
						g_power.start_step = 2;
					}
				}
			}
			else{
				if(laco_save_flag == 1)
				{
					osThreadResume(app_usb_task_id);
					osThreadResume(app_ble_task_id);
					osThreadResume(app_gui_task_id);
				}
				laco_save_flag = 0;
				adc_off_s1[0] = 0;
			}			
		}
		break;
		case 2:
		if(g_power.adc.v_sys < 46.0f){adc_off_s1[1] = 0; return;}
		if(adc_off_s1[1]++ > 10)
		{
			adc_off_s1[1] = 0;
			g_power.start_step = 0;
			system_reset();
		}
		break;
	}
}

static void app_power_evnet_pull_cb(uint32_t event_type, uint32_t event_value)
{
    if(event_type == MAIN_EV_BLE)
    {
        if(event_value == OS_EVENT_BLE_POWER_ON_OFF)
        {
            g_power.key_state = os_ev_get_event_data(event_type, event_value);
            g_power.turn_on = 1;
        }
    }
}

static float app_power_binary_search_temperature(const Rtc_Resistance * data, uint16_t length, float key)
{
	float ret;
	uint16_t start = 0;
	uint16_t end = length - 1;
	uint16_t middle;
	
	if (key > data[start].resistance) 
	{
		ret = data[start].temperature;
	}
	else if (key < data[end].resistance) 
	{
		ret = data[end].temperature;
	}
	else 
	{
		while (start <= end)
		{
			middle = (start + end)/2;
			if (key < data[middle].resistance)
			{
				start = middle;
			}
			else if (key > data[middle].resistance)
			{
				end = middle;
			}
			else
			{
				return data[middle].temperature;
			}
			
			if((start + 1 == end)) 
			{
				ret = data[start].temperature + 
						(data[end].temperature - data[start].temperature)*
							(data[start].resistance - key)/(data[start].resistance - data[end].resistance);
				return ret;
			}	
		}
		ret = data[start].temperature;
	}
	
	return ret;
}

static void app_power_adc_dma1_bct_back(void)
{
    static float adc_buff[ADC_CH_NUM] = {0};
	float i;
    for(uint8_t j = 0; j < ADC_CH_NUM; j++)
    {
		adc_buff[j] = g_power.adc_dma_data[0][j] ;
    }
	i = 0.00805f;   //i = 10 / 4095.0f * 3.3;
    g_power.adc.ismon_cw1 = adc_buff[9] * i ; //A
    g_power.adc.ismon_cw2 = adc_buff[4] * i ; //A
    g_power.adc.ismon_b = adc_buff[5] * i  ;
    g_power.adc.ismon_g = adc_buff[8] * i ;
    g_power.adc.ismon_r = adc_buff[9] * i  ;
}

static void app_power_adc_dma3_bct_back(void)
{
	static float adc_buff[ADC_CH_NUM] = {0};
	float i;
    for(uint8_t j = 0; j < ADC_CH_NUM; j++)
    {
		adc_buff[j] = g_power.adc_dma_data[2][j];
    }
	g_power.adc.ac_mode = adc_buff[18] * 3.3f / 4095.0f;
//	adc_capture_stop_it(HAL_ADC_3);
	i = 40950.0f; //i = 4095.0f * 10;
    g_power.adc.mcu_ntc =  i / adc_buff[14]  - 10.0f;
    g_power.adc.ambient_ntc =  i / adc_buff[19] - 10.0f;
	i = 0.014774;   //i = (100 + 10) / 4095.0f / 100.0f  * 1000.0f / 60 * 3.3;
    g_power.adc.ismon_ww1 = adc_buff[4] * 10 / 4095.0f; //A
    g_power.adc.ismon_ww2 = adc_buff[5] * 10 / 4095.0f; //A
    g_power.adc.v_sys = adc_buff[17] * 0.01531;  // (180+10 ) /10 / 4095.0f * 3.3;
	#if PROJECT_TYPE==307
	g_power.adc.acin  =   adc_buff[15]* i  ; //245 Current compensation
	g_power.adc.dcin1  =  adc_buff[8] * i / 2.0f;   
    g_power.adc.dcin2  =  adc_buff[16] * i / 2.0f;  
	#endif
	#if PROJECT_TYPE==308
	g_power.adc.acin  =   adc_buff[15]* i * 2 ; //245 Current compensation
	g_power.adc.dcin1  =  adc_buff[8] * i ;   
    g_power.adc.dcin2  =  adc_buff[16] * i ;  
	#endif
}

static void app_power_total_power_count(void)
{
	static uint8_t power_error_count = 0;
	
	#if PROJECT_TYPE==307
	if(ui_get_power_type() == POWER_TYPE_DC)
		g_power.total_power =  (((g_power.adc.acin *0.968) - 0.716 )* g_power.adc.v_sys)*0.1 + g_power.total_power*0.9;
	if(ui_get_power_type() == POWER_TYPE_SUPPLY_BOX)
		g_power.total_power =  ((((g_power.adc.dcin1 + g_power.adc.dcin2) *0.968) - 0.716 )* g_power.adc.v_sys)*0.1 + g_power.total_power*0.9;
	#endif
	#if PROJECT_TYPE==308
	if(ui_get_power_type() == POWER_TYPE_DC)
		g_power.total_power =  (g_power.adc.acin* g_power.adc.v_sys)*0.1 + g_power.total_power*0.9;
	if(ui_get_power_type() == POWER_TYPE_SUPPLY_BOX)
		g_power.total_power =  ((g_power.adc.dcin1 + g_power.adc.dcin2)* g_power.adc.v_sys)*0.1 + g_power.total_power*0.9 ;
	#endif
	g_power.ac_v_check = g_power.adc.ac_mode*0.1 + g_power.ac_v_check*0.9;
	g_ctrl_box.power = g_power.total_power;
	ui_set_power(g_power.total_power);
}

static void app_power_state_set(uint8_t state)//1---开机    0---关机
{
	uint8_t demo_state = 0;
	
	sys_config_t cofig_pack;
	data_center_read_config_data(SYS_CONFIG_HIGH_SPEED_MODE, &cofig_pack.hs_mode);
	ui_crmx_power_changed(state);
	if(state)
	{
		g_power.fan.error = 0;
		os_ev_publish_event(MAIN_EV_POWER, EV_LCD_POWER, 1);
		g_power.key_state = 0;
		dev_power.power_led_set(1);
		lcd_init_reset();
		os_ev_publish_event(MAIN_EV_LIGHT, EV_DATA_LIGHT_SWITCH, 1);
		exit_screen_protector();
	}
	else
	{
		data_center_write_config_data_no_event(SYS_CONFIG_DEMO_STATE, &demo_state);
		os_ev_publish_event(MAIN_EV_POWER, EV_LCD_POWER, 0);
		g_power.key_state = 1;
		g_power.fan.error = 1;
		dev_power.power_led_set(0);
		os_ev_publish_event(MAIN_EV_LIGHT, EV_DATA_LIGHT_SWITCH, 0);
	}
}

static uint16_t app_power_fan_speed_value(void)
{
    return g_power.fan.speed;
}

uint8_t app_power_get_voltage_value(void)
{

    if(g_power.adc.v_sys > 46 && g_power.adc.v_sys < 52)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

void power_led_flicker(void)
{
	dev_power.power_led_set(1);
	osDelay(100);
	dev_power.power_led_set(0);
	osDelay(100);
}