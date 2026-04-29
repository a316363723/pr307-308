#include "app_data_center.h"
#include "stdbool.h"
#include "string.h"
#include "stdio.h"
#include "os_event.h"
#include "lfs_flash.h"
#include "iot.h"
#include "dev_w25qxx.h"
#include "perf_counter.h"
#include "update_module.h"
#include "cmsis_os2.h"
#include "stddef.h"
#include "user_bluetooth_proto.h"
#include "crc32.h"
#include "project_config.h"
#include "app_ota.h"
#include "app_gui.h"
#include "ui_data.h"
#include "ui_common.h"
#include "dev_ble.h"
#include "version.h"
#include "app_dmx.h"
#include "app_err_detect.h"
#include "user.h"
#include "app_wdt.h"
#include "hal_flash.h"

#define LOCAL_DATA_HEAD         (0xA5)    //本地数据头
#define LOCAL_DATA_LAST_VER     (0x10)    //1.0版
#define LOCAL_DATA_VER          (0x11)    //1.1版

enum data_center_flag{
    LOCAL_RESET = 0,
    LOCAL_SET
};

typedef struct 
{
    enum light_mode mode;
    struct light_data data;
}light_mode_data;

typedef struct preset_file_info
{
    enum {
        FILE_OPERATE_DELETE,
        FILE_OPERATE_IDLE,
    }operate;
    preset_data_t data;
}light_preset_file;

typedef struct _seeprom_data
{
    uint8_t                     Head;
    uint8_t                     Version;
    light_mode_data             s_light_data;
    sys_config_t                s_sys_cfg;
    light_preset_file           s_preset_file[PRESET_DATA_NUMBER];
    uint8_t                     CheckSum;
}local_data;


typedef struct 
{
    uint8_t                     Head;
    uint8_t                     Version;
    light_mode_data             light_data;
	uint32_t                     CheckSum;
}local_light_data;

typedef struct 
{
    uint8_t                     Head;
    uint8_t                     Version;
    sys_config_t                sys_cfg;
	uint32_t                     CheckSum;
}local_sys_data;

typedef struct
{
    uint8_t                     Head;
    uint8_t                     Version;
    light_preset_file           preset_file[PRESET_DATA_NUMBER];
	uint32_t                     CheckSum;
}local_preset_data;


typedef struct _run_time_data{
	uint32_t 	head;
	uint32_t    write_addr;
	uint32_t 	run_time_min;
	uint32_t    crc;
}_run_time_data_type;

struct member_info
{
    uint16_t offset;
    uint16_t size;
};

static void local_iot_ver_init(void);
static void sys_iot_init(void);
static int local_data_save(void);
static void local_iot_data_save(void);
static void iot_error_evnet_pull_cb(uint32_t event_type, uint32_t event_value);
static int preset_file_init(void);
static int preset_file_save(uint8_t *p_cfg);
static int light_data_init(void);
static int light_data_save(uint8_t *p_cfg, uint32_t len);
static int sys_cfg_data_init(void);
static int sys_cfg_data_save(uint8_t *p_cfg, uint32_t len);
static int local_data_read(void);
static void updata_init(void);
static uint32_t checksum_calculate(const uint8_t* p_data, uint32_t length);
static void data_center_local_spcial_handle(sys_config_t *dest_data, sys_config_t *last_data);

static light_mode_data     s_light_data;
static sys_config_t        s_sys_cfg;
static sys_info_t          s_sys_info;
static local_data          s_local_data;
static light_preset_file   s_preset_file[PRESET_DATA_NUMBER];
static local_light_data    s_local_light_data;
static local_sys_data      s_local_sys_data;
static local_preset_data   s_local_preset_data;
static _run_time_data_type s_run_time;
static const sys_config_t s_sys_cfg_dafault = {
    .local_ui =  7,
    .local_sub_ui =  0,
    .output_mode = 1,
    .lang = 0,
    .screensaver_index = 0,
    .curve_type = 0,
    .dmx_locked = 0,
    .dmx_addr = 1,
    .dmx_loss_behavior = 0,
    .dmx_profile_index = 0,
	.dmx_extension_index = 3,
    .dmx_temination_on = 0,
	.dmx_fade_time = 1,
    .fan_mode = 0,
    .light_effect_index = 0,
    .frequency = 0,
    .local_data_size = 0,
    .ble_on = 1,
    .crmx_on = 0,
    .studiomode_on = 0,
    .work_mode = 0,
    .local_ip = {192, 168, 2, 16},
    .netmask = {255, 255, 255, 0},
    .gateway = {192, 168, 2, 1},
    .custom_fx_type = 0,
    .custom_fx_sel = {0, 0, 0},
    .ble_sn_number = "ABCDEF",
    .hs_mode = 0,
    .yoke_pan_limit = {0, 540},
    .yoke_tilt_limit = {-135, 135},
	.yoke_pan = 0,
	.yoke_tilt = 0,
    .fresnel_angle = 18,
	.angle_limit_starte = 1, 
	.access_select = 0,
    .yoke_pan_limit1[0] = {0, 540},
    .yoke_pan_limit1[1] = {0, 540},
    .yoke_pan_limit1[2] = {0, 540},
	.yoke_pan_limit1[3] = {0, 540},
    .yoke_pan_limit1[4] = {0, 540},
    .yoke_pan_limit1[5] = {0, 540},
	.yoke_pan_limit1[6] = {0, 540},
    .yoke_pan_limit1[7] = {0, 540},
    .yoke_pan_limit1[8] = {0, 540},
	.yoke_pan_limit1[9] = {0, 540},
    .yoke_pan_limit1[10] = {0, 540},
    .yoke_tilt_limit1[0] = {-115, 115},
    .yoke_tilt_limit1[1] = {-100, 100},
    .yoke_tilt_limit1[2] = {-105, 105},
	.yoke_tilt_limit1[3] = {-110, 110},
    .yoke_tilt_limit1[4] = {-120, 120},
    .yoke_tilt_limit1[5] = {-125, 125},
	.yoke_tilt_limit1[6] = {-115, 115},
    .yoke_tilt_limit1[7] = {-135, 135},
    .yoke_tilt_limit1[8] = {-135, 135},
	.yoke_tilt_limit1[9] = {-135, 135},
    .yoke_tilt_limit1[10] = {-135, 135},
    .yoke_pan1 = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	.yoke_tilt1 = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    .angle_limit_starte1 = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
	.effects_mode = 7,
	.eth_state_set = 0,
	.eth_artnet_universe = 0,
	.eth_sacn_universe = 1,
	.power_value = 2000,
	.power_limit = 0,
	.demo_state = 0,
	.pan_demo_state = 0,
	.pan_demo_limit = {0, 540},
	.tili_demo_state = 0,
	.tili_demo_limit = {-135, 135},
	.zoom_demo_state = 0,
	.zoom_demo_limit = {18, 50},
	.power_on_state = 0,
};
#define PARAM(param, type)      (param)        
static light_mode_data  s_light_data_dafault = {
    .mode = LIGHT_MODE_CCT,
    .data = {
    .cct_arg.lightness = 500,
    .cct_arg.cct.cct = 5600,
    .cct_arg.cct.duv = 0,
    .hsi_arg.lightness = 500,
    .hsi_arg.hsi.cct = 5600,
    .hsi_arg.hsi.hue = 100,
    .hsi_arg.hsi.sat = 50,
    .gel_arg.lightness = 500,
    .gel_arg.gel.cct = 5600,
    .gel_arg.gel.brand = 0,
		.gel_arg.gel.type[0]=0,          //系列获取: type[ 品牌 ]
		.gel_arg.gel.type[1]=0,
		.gel_arg.gel.color[0][0]=0,
		.gel_arg.gel.color[0][1]=0,
		.gel_arg.gel.color[0][2]=0,
		.gel_arg.gel.color[0][3]=0,
		.gel_arg.gel.color[0][4]=0,
		.gel_arg.gel.color[1][0]=0,
		.gel_arg.gel.color[1][1]=0,
		.gel_arg.gel.color[1][2]=0,
		.gel_arg.gel.color[1][3]=0,
		.gel_arg.gel.color[1][4]=0,
    .rgb_arg.lightness = 500,
    .rgb_arg.rgb.r = 500,
    .rgb_arg.rgb.b = 500,
    .rgb_arg.rgb.g = 500,
    .source_arg.lightness = 500,
    .source_arg.source.type = 0,
    .xy_arg.lightness = 500,
    .xy_arg.xy.x = 3301,
    .xy_arg.xy.y = 3390,
    .club_lights.lightness = 500,
    .club_lights.spd = 5,
    .club_lights.color = 0,
    .club_lights.state = 0,
    .paparazzi.lightness = 500,
    .paparazzi.frq = 5,
    .paparazzi.cct = 5600,
    .paparazzi.gm = 0,
    .paparazzi.state = 0,
    .lightning.lightness = 500,
    .lightning.frq = 5,
    .lightning.cct = 5600,
    .lightning.gm = 0,
    .lightning.trigger = 0,
    .lightning.speed = 5,
    .lightning.state = 0,
    .tv.lightness = 500,
    .tv.frq = 5,
    .tv.cct = 0,
    .tv.state = 0,
    .candle.lightness = 500,
    .candle.cct = 5600,
    .candle.spd = 5,
    .candle.state = 0,
    .fire.lightness = 500,
    .fire.frq = 5,
    .fire.cct = 0,
    .fire.state = 0,
    .strobe.lightness = 500,
    .strobe.frq = 5,
    .strobe.state = 0,
    .strobe.mode = FX_MODE_CCT,
    .strobe.mode_arg.cct.cct = 5600,
    .strobe.mode_arg.cct.duv = 0,
    .explosion.lightness = 500,
    .explosion.frq = 5,
    .explosion.trigger = 0,
    .explosion.state = 0,
    .explosion.mode = FX_MODE_CCT,
    .explosion.mode_arg.cct.cct = 5600,
    .explosion.mode_arg.cct.duv = 0,
    .fault_bulb.lightness = 500,
    .fault_bulb.frq = 5,
    .fault_bulb.speed = 5,
    .fault_bulb.state = 0,
    .fault_bulb.mode = FX_MODE_CCT,
    .fault_bulb.mode_arg.cct.cct = 5600,
    .fault_bulb.mode_arg.cct.duv = 0,
    .pulsing.lightness = 500,
    .pulsing.frq = 5,
    .pulsing.speed = 5,
    .pulsing.state = 0,
    .pulsing.mode = FX_MODE_CCT,
    .pulsing.mode_arg.cct.cct = 5600,
    .pulsing.mode_arg.cct.duv = 0,
    .welding.lightness = 500,
    .welding.frq = 5,
    .welding.trigger = 0,
    .welding.state = 0,
    .welding.min = 50,
    .welding.mode = FX_MODE_CCT,
    .welding.mode_arg.cct.cct = 5600,
    .welding.mode_arg.cct.duv = 0,
    .cop_car.lightness = 500,
    .cop_car.frq = 5,
    .cop_car.color = 0,
    .cop_car.state = 0,
    .color_chase.lightness =  500,
    .color_chase.spd = 5,
    .color_chase.sat = 0,
    .color_chase.state = 0,
    .party_lights.lightness = 500,
    .party_lights.spd = 5,
    .party_lights.sat = 0,
    .party_lights.state = 0,
    .fireworks.lightness = 500,
    .fireworks.frq = 5,
    .fireworks.type = 0,
    .fireworks.state = 0,
    .paparazzi_2.lightness = 500,
    .paparazzi_2.gap_time = 0,
    .paparazzi_2.min_gap_time = 0,
    .paparazzi_2.state = 0,
    .paparazzi_2.mode = FX_MODE_CCT,
    .paparazzi_2.mode_arg.cct.cct = 5600,
    .paparazzi_2.mode_arg.cct.duv = 0,
    .lightning_2.lightness = 500, 
    .lightning_2.frq = 5,
    .lightning_2.spd = 5,
    .lightning_2.state = 0,
    .lightning_2.trigger = 0,
    .lightning_2.mode = FX_MODE_CCT,
    .lightning_2.mode_arg.cct.cct = 5600,
    .lightning_2.mode_arg.cct.duv = 0,
    .tv_2.lightness = 500,
    .tv_2.spd = 5,
    .tv_2.state = 0,
    .tv_2.mode = FX_MODE_CCT,
    .tv_2.mode_arg.cct.max_cct = 5600,
    .tv_2.mode_arg.cct.min_cct = 5600,
    .tv_2.mode_arg.cct.duv = 0, 
    .fire_2.lightness = 500,
    .fire_2.spd = 5,
    .fire_2.state = 0,
    .fire_2.mode = FX_MODE_CCT,
    .fire_2.mode_arg.cct.max_cct = 5600,
    .fire_2.mode_arg.cct.min_cct = 5600,
    .fire_2.mode_arg.cct.duv = 0,
    .strobe_2.lightness = 500,
    .strobe_2.spd = 5,
    .strobe_2.state = 0,
    .strobe_2.mode = FX_MODE_CCT,
    .strobe_2.mode_arg.cct.cct = 5600,
    .strobe_2.mode_arg.cct.duv = 0,
    .explosion_2.lightness = 500,
    .explosion_2.decay = 5,
    .explosion_2.state = 0,
    .explosion_2.mode = FX_MODE_CCT,
    .explosion_2.mode_arg.cct.cct = 5600,
    .explosion_2.mode_arg.cct.duv = 0,
    .fault_bulb_2.lightness = 500,
    .fault_bulb_2.spd = 5,
    .fault_bulb_2.frq = 5,
    .fault_bulb_2.state = 0,
    .fault_bulb_2.mode = FX_MODE_CCT,
    .fault_bulb_2.mode_arg.cct.cct = 5600,
    .fault_bulb_2.mode_arg.cct.duv = 0, 
    .pulsing_2.lightness = 500,
    .pulsing_2.spd = 5,
    .pulsing_2.frq = 5,
    .pulsing_2.state = 0,
    .pulsing_2.mode = FX_MODE_CCT,
    .pulsing_2.mode_arg.cct.cct = 5600,
    .pulsing_2.mode_arg.cct.duv = 0, 
    .welding_2.lightness = 500,
    .welding_2.min_lightness = 50,
    .welding_2.frq = 5,
    .welding_2.state = 0,
    .welding_2.mode = FX_MODE_CCT,
    .welding_2.mode_arg.cct.cct = 5600,
    .welding_2.mode_arg.cct.duv = 0, 
    .cop_car_2.lightness = 500,
    .cop_car_2.frq = 5,
    .cop_car_2.color = 0,  
    .cop_car_2.state = 0,
    .party_lights_2.lightness = 500,
    .party_lights_2.sat = 0,
    .party_lights_2.spd = 5,
    .party_lights_2.state = 0,
    .fireworks_2.lightness = 500,
    .fireworks_2.gap_time = 0,
    .fireworks_2.min_gap_time = 0,
    .fireworks_2.mode = FX_MODE_CCT,
    .fireworks_2.state = 0,
    .lightning_3.lightness = 500,
    .lightning_3.gap_time = 0,
    .lightning_3.min_gap_time = 0,        
    .lightning_3.state = 0,
    .lightning_3.mode = FX_MODE_CCT,
    .lightning_3.mode_arg.cct.cct = 5600,
    .lightning_3.mode_arg.cct.duv = 0,
    .tv_3.lightness = 500,
    .tv_3.gap_time = 0,
    .tv_3.min_gap_time = 0,
    .tv_3.state = 0,
    .tv_3.mode = FX_MODE_CCT,
    .tv_3.mode_arg.cct.max_cct = 5600,
    .tv_3.mode_arg.cct.min_cct = 5600,
    .tv_3.mode_arg.cct.duv = 0,   
    .fire_3.lightness = 500,
    .fire_3.frq = 5,
    .fire_3.state = 0,
    .fire_3.mode = FX_MODE_CCT,
    .fire_3.mode_arg.cct.max_cct = 5600,
    .fire_3.mode_arg.cct.min_cct = 5600,
    .fire_3.mode_arg.cct.duv = 0,
    .fault_bulb_3.lightness = 500,
    .fault_bulb_3.gap_time = 0,
    .fault_bulb_3.min_gap_time = 0,
    .fault_bulb_3.state = 0,
    .fault_bulb_3.mode = FX_MODE_CCT,
    .fault_bulb_3.mode_arg.cct.cct = 5600,
    .fault_bulb_3.mode_arg.cct.duv = 0,
    .pulsing_3.lightness = 500,
    .pulsing_3.frq = 5,
    .pulsing_3.state = 0,
    .pulsing_3.mode = FX_MODE_CCT,
    .pulsing_3.mode_arg.cct.cct = 5600,
    .pulsing_3.mode_arg.cct.duv = 0,
    .cop_car_3.lightness = 500,
    .cop_car_3.frq = 5,
    .cop_car_3.color = 0,
    .cop_car_3.state = 0,
    }
};

static uint8_t                      s_iot_read_data[sizeof(IOT_Parameter_TypeDef)];
static IOT_Parameter_Serial_TypeDef s_local_iot_data = {
    .device_state_serial = DEVICE_CLOSE,            //1号
    .run_time_serial = ADAPTER_SUPPLY_TIME,         //2号
    .mode_time_serial = CCT_MODE_TIME,              //3号
    .int_val = 0,                                   //4号
    .fresnel_val = 361,                             //5号   
    .pitch_angle_val = 361,                         //6号
    .sidus_pro_fx_serial = MANUAL_EFFECTS_TYPE,     //7号
};
static uint8_t s_local_uuid_write_flag;
static struct member_info s_light_struct_member_info[] = 
{
#define DB_MEM_INFO
#define DB_CONTAINER_STRUCT struct light_data
#include "db_light_mode.h"    
};
static struct member_info s_sys_cfg_struct_member_info[] = 
{
#define DB_MEM_INFO
#define DB_CONTAINER_STRUCT sys_config_t
#include "db_sys_config.h"
};
static struct member_info s_sys_info_struct_member_info[] = 
{
#define DB_MEM_INFO
#define DB_CONTAINER_STRUCT sys_info_t
#include "db_sys_info.h"
};

#if DATA_CENTRE_TEST_EN
static void data_center_test(void);

static uint8_t              s_test_write_data = 0;
static light_mode_data_t    s_local_test_write_light_data;
static light_mode_data_t    s_local_test_read_light_data;
static enum light_mode      s_test_light_mode;
static sys_config_t         s_local_test_write_data;
static sys_config_t         s_local_test_read_data;
static enum sys_config_type s_local_test_data_mode;
static preset_data_t        s_local_test_write_preset_data;
static preset_data_t        s_local_test_read_preset_data;
uint8_t buff11[255] = {
0x10, 0x10, 0x10, 0x10, 0x10, 
0x10, 0x10, 0x10, 0x10, 0x10, 
};

uint8_t buff_read[20];

preset_data_t read_data11;
preset_data_t read_data1;
preset_data_t wirte_preset_data;

int test_res;
uint32_t test_len;
const uint32_t *test_read_addr;

uint8_t test_flasgwrite_data[10] = {0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10};
uint32_t addr_data;

#endif
const osThreadAttr_t g_data_center_thread_attr = { 
    .name = "app_data_center", 
    .priority = osPriorityBelowNormal, 
#if PROJECT_TYPE==307
	.stack_size = 256 * 4
#elif PROJECT_TYPE==308
    .stack_size = 384 * 4
#endif
};
osThreadId_t app_data_center_task_id;
extern osSemaphoreId_t thread_sync_sigl;
uint32_t data_task_static_size;

void app_data_center_entry(void *argument)
{
    uint32_t number;
	struct db_dmx dmx = {0};
	
    dev_w25qxx_init();
    delay_ms(100);
    lfs_mount_fail = sys_lfs_mount();
    if(lfs_mount_fail != 0)
    {
        number = (LFS_FLASH_OFFS/1024/64);
        for(uint32_t erase_number = number; erase_number < 64+number; erase_number++)
        {
            dev_w25qxx_erase_64sectors(erase_number);
			app_wdt_reset_set();
        }
        lfs_mount_fail = sys_lfs_mount();
		if(lfs_mount_fail) error_functionArray[flash_ec_011](1);
    }
	else
	{
		error_functionArray[flash_ec_011](0);
	}
#if DATA_CENTRE_TEST_EN
    /* 测试外部flash操作 */
//  dev_w25qxx_erase_sectors(0, 1);
//  dev_w25qxx_write_data(0x00, buff11, sizeof(buff11));
//  dev_w25qxx_read_data(0x00, buff_read, sizeof(buff_read));
//  dev_w25qxx_erase_sectors(0, 1);
//  dev_w25qxx_read_data(0x00, buff_read, sizeof(buff_read));
    
    /* 测试映射地址 */
    dev_w25qxx_erase_sectors(0, 1);
    test_len = dev_w25qxx_get_mapaddr(0, (const void **)&test_read_addr);
    memcpy(test_flasg_data, test_read_addr, 10);
    dev_w25qxx_erase_sectors(UPDATE_START_ADDRESS, 1);
    dev_w25qxx_write_data(UPDATE_START_ADDRESS, test_flasgwrite_data, sizeof(test_flasgwrite_data));
    memcpy(test_flasg_data, test_read_addr, 10);
    dev_w25qxx_read_data(UPDATE_START_ADDRESS, test_flasg_data, sizeof(test_flasg_data));
#endif
    updata_init(); 
	app_ctr_update_data_erase();
	app_bmp_update_data_erase();
    local_data_read();
	//首次上电做的特殊处理
	if(s_light_data.mode == LIGHT_MODE_DMX)
	{
		s_light_data.mode = (uint8_t)ui_get_effectts_mode(s_sys_cfg.effects_mode);
	}
	ui_set_work_mode(s_sys_cfg.work_mode);
	memcpy((void*)((uint32_t)&s_light_data.data + s_light_struct_member_info[LIGHT_MODE_DMX].offset), &dmx, s_light_struct_member_info[LIGHT_MODE_DMX].size);//每次上电清空DMX数据
	
	data_center_run_time_read();
    osSemaphoreRelease(thread_sync_sigl);
    for (;;)
    {
#if DATA_CENTRE_TEST_EN
      if(s_test_write_data == 1)
      {
          data_center_test();
          s_test_write_data = 0;
      }
#endif
		data_task_static_size = osThreadGetStackSpace(app_data_center_task_id);
        osDelay(100);
    }
}

/*
功能：恢复光效常规选择出厂设置
参数：@p_args ：数据类型
返回值：NULL
*/
void data_center_reset_fx_param(struct db_fx_mode_arg* p_args)
{
    PARAM(p_args, page_cct_t)->cct.cct = 5600;
    PARAM(p_args, page_cct_t)->cct.duv = 0;
    PARAM(p_args, page_hsi_t)->hsi.hue = 100;
    PARAM(p_args, page_hsi_t)->hsi.sat = 50;
    PARAM(p_args, page_hsi_t)->hsi.cct = 5600;    
	PARAM(p_args, page_gel_t)->gel.brand = 0;
    PARAM(p_args, page_gel_t)->gel.type[0]=0,          //系列获取: type[ 品牌 ]
	PARAM(p_args, page_gel_t)->gel.type[1]=0,
    PARAM(p_args, page_gel_t)->gel.color[0][0]=0,
	PARAM(p_args, page_gel_t)->gel.color[0][1]=0,
	PARAM(p_args, page_gel_t)->gel.color[0][2]=0,
	PARAM(p_args, page_gel_t)->gel.color[0][3]=0,
	PARAM(p_args, page_gel_t)->gel.color[0][4]=0,
	PARAM(p_args, page_gel_t)->gel.color[1][0]=0,
	PARAM(p_args, page_gel_t)->gel.color[1][1]=0,
	PARAM(p_args, page_gel_t)->gel.color[1][2]=0,
	PARAM(p_args, page_gel_t)->gel.color[1][3]=0,
	PARAM(p_args, page_gel_t)->gel.color[1][4]=0,
    PARAM(p_args, page_gel_t)->gel.cct = 5600; 
	PARAM(p_args, page_xy_t)->xy.x = 3301;
    PARAM(p_args, page_xy_t)->xy.y = 3390;
    PARAM(p_args, page_source_t)->source.type = 0;
    PARAM(p_args, page_source_t)->source.x = 0;
    PARAM(p_args, page_source_t)->source.y = 0;
	PARAM(p_args, page_rgb_t)->rgb.r = 500;
    PARAM(p_args, page_rgb_t)->rgb.g = 500;
    PARAM(p_args, page_rgb_t)->rgb.b = 500;  
	
}
void data_center_reset_fx_limit(struct db_fx_mode_limit_arg* p_args)
{
	PARAM(p_args, page_cct_t)->cct.max_cct = 10000;
	PARAM(p_args, page_cct_t)->cct.min_cct = 2000;
    PARAM(p_args, page_cct_t)->cct.duv = 0;
    PARAM(p_args, page_hsi_t)->hsi.max_hue = 360;
	PARAM(p_args, page_hsi_t)->hsi.min_hue = 1;
    PARAM(p_args, page_hsi_t)->hsi.sat = 50;
    PARAM(p_args, page_hsi_t)->hsi.cct = 5600;  

}

/*
功能：恢复出厂设置
参数：@p_args ：数据类型
返回值：NULL
*/
void data_center_light_data_reset(void)	
{
	memcpy(&s_light_data, &s_light_data_dafault, sizeof(light_mode_data));
	data_center_reset_fx_param(&s_light_data.data.explosion.mode_arg);
	data_center_reset_fx_param(&s_light_data.data.strobe.mode_arg);
	data_center_reset_fx_param(&s_light_data.data.fault_bulb.mode_arg);
	data_center_reset_fx_param(&s_light_data.data.pulsing.mode_arg);
	data_center_reset_fx_param(&s_light_data.data.welding.mode_arg);
	data_center_reset_fx_param(&s_light_data.data.strobe_2.mode_arg);
	data_center_reset_fx_param(&s_light_data.data.explosion_2.mode_arg);
	data_center_reset_fx_param(&s_light_data.data.fault_bulb_2.mode_arg);
	data_center_reset_fx_param(&s_light_data.data.pulsing_2.mode_arg);
	data_center_reset_fx_param(&s_light_data.data.welding_2.mode_arg);
	data_center_reset_fx_param(&s_light_data.data.lightning_2.mode_arg);
	data_center_reset_fx_limit(&s_light_data.data.fire_2.mode_arg);
	data_center_reset_fx_limit(&s_light_data.data.tv_2.mode_arg);
	
}
	

/*
功能：修改本地数据
参数：@type ：数据类型
      @arg：数据值
返回值：0---成功     1---失败
*/
int data_center_write_config_data(enum sys_config_type type, const void* arg)
{
    uint32_t max = sizeof(s_sys_cfg_struct_member_info) / sizeof(s_sys_cfg_struct_member_info[0]);
    if (max < type || NULL == arg)
        return -1;
	
	memcpy((void*)((uint32_t)&s_sys_cfg + s_sys_cfg_struct_member_info[type].offset), arg, s_sys_cfg_struct_member_info[type].size);
	
	os_ev_publish_event(MAIN_EV_DATA, EV_DATA_CONFIG_CHANGE, type);
	
    return 0;
}

/*
功能：修改本地数据,不会产生事件
参数：@type ：数据类型
      @arg：数据值
返回值：0---成功     1---失败
*/
int data_center_write_config_data_no_event(enum sys_config_type type, const void* arg)
{
    uint32_t max = sizeof(s_sys_cfg_struct_member_info) / sizeof(s_sys_cfg_struct_member_info[0]);
    if (max < type || NULL == arg)
        return -1;
	
	memcpy((void*)((uint32_t)&s_sys_cfg + s_sys_cfg_struct_member_info[type].offset), arg, s_sys_cfg_struct_member_info[type].size);
	
    return 0;
}


/*
功能：读取本地数据
参数：@type ：数据类型
      @arg：保存数据值的地址
返回值：0---成功    1---失败
*/
int data_center_read_config_data(enum sys_config_type type, void* arg)
{
    uint32_t max = sizeof(s_sys_cfg_struct_member_info) / sizeof(s_sys_cfg_struct_member_info[0]);
    if (max < type || NULL == arg)
        return -1;
	
	if(SYS_CONFIG_ALL == type)
	{
		memcpy(arg, &s_sys_cfg, sizeof(sys_config_t)); 
	}
	else
	{
		memcpy(arg, (void*)((uint32_t)&s_sys_cfg + s_sys_cfg_struct_member_info[type].offset), s_sys_cfg_struct_member_info[type].size);
	}
	
    return 0;
}

/*
功能：修改状态信息
参数：@type ：数据类型
      @arg：数据值
返回值：0---成功     1---失败
*/
int data_center_write_sys_info(enum sys_info_type type, const void* arg)
{
    uint32_t max = sizeof(s_sys_info_struct_member_info) / sizeof(s_sys_info_struct_member_info[0]);
    if (max < type || NULL == arg)
        return -1;

	memcpy((void*)((uint32_t)&s_sys_info + s_sys_info_struct_member_info[type].offset), arg, s_sys_info_struct_member_info[type].size);
	if(type == SYS_INFO_UUID)
	{
		data_center_set_uuid_write_state(1);
	}
	
    return 0;
}

/*
功能：读取状态信息
参数：@type ：数据类型
      @arg：保存数据值的地址
返回值：0---成功    1---失败
*/
int data_center_read_sys_info(enum sys_info_type type, void* arg)
{
    uint32_t max = sizeof(s_sys_info_struct_member_info) / sizeof(s_sys_info_struct_member_info[0]);
    if (max < type || NULL == arg)
        return -1;

	memcpy(arg, (void*)((uint32_t)&s_sys_info + s_sys_info_struct_member_info[type].offset), s_sys_info_struct_member_info[type].size);
	
    return 0;
}

/*
功能：读取本地光模式数据
参数：@arg：对应光模式参数
返回值：0---成功    -1---失败
*/
int data_center_read_light_data(enum light_mode mode, void* arg)
{
    uint32_t max = sizeof(s_light_struct_member_info) / sizeof(s_light_struct_member_info[0]);
    if (max < mode ||  NULL == arg)
        return -1;
    
    memcpy(arg, (void*)((uint32_t)&s_light_data.data + s_light_struct_member_info[mode].offset), s_light_struct_member_info[mode].size);

    return 0;
}

/*
功能：修改本地光模式数据
参数：@mode ：光模式
      @arg：对应光模式参数
返回值：0---成功     -1---失败
*/
int data_center_write_light_data(enum light_mode mode, const void* arg)
{
    uint32_t max = sizeof(s_light_struct_member_info) / sizeof(s_light_struct_member_info[0]);
    if (max < mode ||  NULL == arg)
        return -1;

    s_light_data.mode = mode;
    memcpy((void*)((uint32_t)&s_light_data.data + s_light_struct_member_info[mode].offset), arg, s_light_struct_member_info[mode].size);

    os_ev_publish_event(MAIN_EV_DATA, EV_DATA_LIGHT_CHANGE, mode);

    return 0;
}


/*
功能：修改本地光模式数据
参数：@mode ：光模式
      @arg：对应光模式参数
返回值：0---成功     -1---失败
*/
int data_center_write_light_data_no_event(enum light_mode mode, const void* arg)
{
    uint32_t max = sizeof(s_light_struct_member_info) / sizeof(s_light_struct_member_info[0]);
    if (max < mode ||  NULL == arg)
        return -1;

    s_light_data.mode = mode;
    memcpy((void*)((uint32_t)&s_light_data.data + s_light_struct_member_info[mode].offset), arg, s_light_struct_member_info[mode].size);
	
    return 0;
}

/*
功能：修改本地光模式亮度
参数：@mode ：光模式
      @arg：对应光模式参数
返回值：0---成功     -1---失败
*/
int data_center_write_brightness(enum light_mode mode, const uint16_t arg)
{
    int32_t sl = osKernelLock();
    s_light_data.mode = mode;
    memcpy((void*)((uint32_t)&s_light_data.data + s_light_struct_member_info[mode].offset), &arg, 2);
    osKernelRestoreLock(sl);

    os_ev_publish_event(MAIN_EV_DATA, EV_DATA_LIGHT_CHANGE, mode);
	  return 0;
}

/*
功能：读取当前光效模式
参数：无
返回值：0---成功     1---失败
*/
enum light_mode date_center_get_light_mode(void)
{
    return s_light_data.mode;
}

/*
功能：设置当前光效模式
参数：@mode：光模式
返回值：0---成功     1---失败
*/
int date_center_set_light_mode(enum light_mode mode)
{
    if (mode > LIGHT_MODE_FACTORY_PWM)
    {
        return 1;
    }
    
    int32_t sl = osKernelLock();
    s_light_data.mode = mode;
    osKernelRestoreLock(sl);
    
    return 0;
}

/*
功能：修改预设光模式数据
参数：@mode ：预设位置
      @arg：对应光模式参数
返回值：0---成功     !0失败
*/
int data_center_write_preset_data(uint8_t preset_num, const void *arg)
{
    if (NULL == arg || preset_num >= PRESET_DATA_NUMBER)
        return -1;
    
    int32_t sl = osKernelLock();
    s_preset_file[preset_num].operate = FILE_OPERATE_IDLE;
    memcpy(&s_preset_file[preset_num].data, arg, sizeof(preset_data_t));
    osKernelRestoreLock(sl);
    
    return 0;
}

/*
功能：读取预设光模式数据
参数：@mode ：预设位置
      @arg：光模式参数
返回值：0---成功     !0---失败(没有预设数据)
*/
int data_center_read_preset_data(uint8_t preset_num, void *arg)
{
    if (NULL == arg || preset_num >= PRESET_DATA_NUMBER)
        return -1;
    
    if(s_preset_file[preset_num].operate == FILE_OPERATE_DELETE)
        return -1;
    
    int32_t sl = osKernelLock();
    memcpy(arg, &s_preset_file[preset_num].data, sizeof(preset_data_t));
    osKernelRestoreLock(sl);
    
    return 0;
}

/*
功能：删除预设光模式数据
参数：@mode ：需要删除的预设序列号
返回值：0---成功     !0---失败
*/
int data_center_delete_preset_data(int preset_num)
{
    if (preset_num >= PRESET_DATA_NUMBER)
        return -1;
    
    s_preset_file[preset_num].operate = FILE_OPERATE_DELETE;
        
    return 0;
}

/*
功能：读取本地IOT数据
参数：@iot_num ：IOT包序列
      @arg：数据地址
返回值：0---成功     1---失败
*/
int data_center_read_run_time(void)
{
    return s_run_time.run_time_min / 2;
}

/*
功能：重置数据
参数：@mode：LOCAL_LIGHT_DATA: 重置光模式数据
             LOCAL_CONFIG_DATA: 重置菜单配置数据
             LOCAL_PRESET_DATA: 重置预设数据
             LOCAL_ALL_DATA: 重置全部数据
返回值：0---成功    1---失败
*/
int data_center_factory_reset(uint32_t data_select)
{
    if(data_select > LOCAL_ALL_DATA)
    {
        return 1;
    }
    
    int32_t sl = osKernelLock();
    if(data_select == LOCAL_ALL_DATA)
    {
        data_center_light_data_reset();
        memcpy(&s_sys_cfg, &s_sys_cfg_dafault, sizeof(sys_config_t));
        for(uint16_t i = 0; i < PRESET_DATA_NUMBER; i++)
        {
            data_center_delete_preset_data(i);
        }
        osKernelRestoreLock(sl);
        return 0;
    }
    if(data_select == LOCAL_LIGHT_DATA)
    {
        data_center_light_data_reset();
    }
    if(data_select == LOCAL_CONFIG_DATA)
    {
        memcpy(&s_sys_cfg, &s_sys_cfg_dafault, sizeof(sys_config_t));
    }
    if(data_select == LOCAL_PRESET_DATA)
    {
        for(uint16_t i = 0; i < PRESET_DATA_NUMBER; i++)
        {
            data_center_delete_preset_data(i);
        }
    }
	
    osKernelRestoreLock(sl);
    
    return 0;
}

void data_center_run_time_write(uint32_t time)
{
	_run_time_data_type *tun_time_temp = &s_run_time;
	uint8_t res = 0;
	uint32_t addr = 0;
	
 	if(tun_time_temp->write_addr < RUN_TIME_FLASH_OFFS1)
	{
		tun_time_temp->write_addr = RUN_TIME_FLASH_OFFS1;
	}
	if(tun_time_temp->write_addr >= RUN_TIME_FLASH_OFFS3)
	{
		tun_time_temp->write_addr = RUN_TIME_FLASH_OFFS1;
		dev_w25qxx_erase_sectors(RUN_TIME_FLASH_OFFS1/(1024*4), 1);
		res = 1;
	}
	addr = tun_time_temp->write_addr;
	tun_time_temp->head = LOCAL_DATA_HEAD;
	tun_time_temp->write_addr = tun_time_temp->write_addr+sizeof(_run_time_data_type);
	tun_time_temp->run_time_min += time;
	tun_time_temp->crc = tun_time_temp->head+tun_time_temp->write_addr+tun_time_temp->run_time_min;
	dev_w25qxx_write_data(addr, (uint8_t *)tun_time_temp, sizeof(_run_time_data_type));
	if(res == 1)    //写完一个数据后再擦除，预防掉电后没有掉电数据
	{
		dev_w25qxx_erase_sectors(RUN_TIME_FLASH_OFFS2/(1024*4), 1);
		dev_w25qxx_erase_sectors(RUN_TIME_FLASH_OFFS3/(1024*4), 1);
	}
}


uint32_t data_center_run_time_read(void)
{
	_run_time_data_type time_temp;
	uint32_t crc = 0;
	uint32_t addr;
	time_temp.write_addr = RUN_TIME_FLASH_OFFS3;
	while(1)
	{
		addr = time_temp.write_addr;
		dev_w25qxx_read_data(time_temp.write_addr, (uint8_t *)&time_temp, sizeof(_run_time_data_type));
		crc = time_temp.head + time_temp.run_time_min + time_temp.write_addr;
		if(time_temp.head == LOCAL_DATA_HEAD && crc == time_temp.crc)
		{
			memcpy(&s_run_time, &time_temp, sizeof(_run_time_data_type));
			break;
		}
		if(addr <= RUN_TIME_FLASH_OFFS1)
		{
			data_center_run_time_clear();
			break;
		}
		if(addr > RUN_TIME_FLASH_OFFS3)
		{
			data_center_run_time_clear();
			break;
		}
		time_temp.write_addr = addr - sizeof(_run_time_data_type); 
	}
}
void data_center_set_uuid_write_state(uint8_t state)
{
	s_local_uuid_write_flag = state;
}

uint8_t data_center_get_uuid_write_state(void)
{
	return s_local_uuid_write_flag;
}

void data_center_iot_handle(void)
{
    if(PAGE_UPDATE != screen_get_act_pid())
    {
		local_iot_data_save();
    }
}

void data_center_local_handle(void)
{
	local_data_save();
}

void data_center_run_time_clear(void)
{
	memset(&s_run_time, 0, sizeof(_run_time_data_type));
	dev_w25qxx_erase_sectors(RUN_TIME_FLASH_OFFS1/(1024*4), 1);
	dev_w25qxx_erase_sectors(RUN_TIME_FLASH_OFFS2/(1024*4), 1);
	dev_w25qxx_erase_sectors(RUN_TIME_FLASH_OFFS3/(1024*4), 1);
}

static void iot_light_int_time_config(const uint8_t mode, uint8_t *int_arg, uint8_t *time_arg)
{
    switch(mode)
    {
        case LIGHT_MODE_CCT:
            *int_arg = s_light_data.data.cct_arg.lightness / 10;
            *time_arg = CCT_MODE_TIME;
        break;
        case LIGHT_MODE_HSI:
            *int_arg = s_light_data.data.hsi_arg.lightness / 10;
            *time_arg = HSI_MODE_TIME;
        break;
        case LIGHT_MODE_GEL:
            *int_arg = s_light_data.data.gel_arg.lightness / 10;
            *time_arg = GEL_MODE_TIME;
        break;
        case LIGHT_MODE_RGB:
            *int_arg = s_light_data.data.rgb_arg.lightness / 10;
            *time_arg = RGB_MODE_TIME;
        break;
        case LIGHT_MODE_XY:
            *int_arg = s_light_data.data.xy_arg.lightness / 10;
            *time_arg = XY_MODE_TIME;
        break;
        case LIGHT_MODE_SOURCE:
            *int_arg = s_light_data.data.source_arg.lightness / 10;
            *time_arg = CCT_MODE_TIME;
        break;
        default:break;
    }
}

static void iot_fx_I_int_time_config(const uint8_t mode, uint8_t *int_arg, uint8_t *time_arg)
{
    *time_arg = SFX_MODE_TIME;
    switch (mode)
    {
        case LIGHT_MODE_FX_CLUBLIGHTS:
            *int_arg = s_light_data.data.club_lights.lightness / 10;
        break;
        case LIGHT_MODE_FX_PAPARAZZI:
            *int_arg = s_light_data.data.paparazzi.lightness / 10;
        break;
        case LIGHT_MODE_FX_LIGHTNING:
            *int_arg = s_light_data.data.lightning.lightness / 10;
        break;
        case LIGHT_MODE_FX_TV:
            *int_arg = s_light_data.data.tv.lightness / 10;
        break;
        case LIGHT_MODE_FX_CANDLE:
            *int_arg = s_light_data.data.candle.lightness / 10;
        break;
        case LIGHT_MODE_FX_FIRE:
            *int_arg = s_light_data.data.fire.lightness / 10;
        break;
        case LIGHT_MODE_FX_STROBE:
            *int_arg = s_light_data.data.strobe.lightness / 10;
        break;
        case LIGHT_MODE_FX_EXPLOSION:
            *int_arg = s_light_data.data.explosion.lightness / 10;
        break;
        case LIGHT_MODE_FX_FAULT_BULB:
            *int_arg = s_light_data.data.fault_bulb.lightness / 10;
        break;
        case LIGHT_MODE_FX_PULSING:
            *int_arg = s_light_data.data.pulsing.lightness / 10;
        break;
        case LIGHT_MODE_FX_WELDING:
            *int_arg = s_light_data.data.welding.lightness / 10;
        break;
        case LIGHT_MODE_FX_COP_CAR:
            *int_arg = s_light_data.data.cop_car.lightness / 10;
        break;
        case LIGHT_MODE_FX_COLOR_CHASE:
            *int_arg = s_light_data.data.color_chase.lightness / 10;
        break;
        case LIGHT_MODE_FX_PARTY_LIGHTS:
            *int_arg = s_light_data.data.party_lights.lightness / 10;
        break;
        case LIGHT_MODE_FX_FIREWORKS:
            *int_arg = s_light_data.data.fireworks.lightness / 10;
        break;
        default:break;
    }
}

static void iot_fx_II_int_config(const uint8_t mode, uint8_t *int_arg, uint8_t *time_arg)
{
    *time_arg = SFX_MODE_TIME;
    switch (mode)
    {
        case LIGHT_MODE_FX_PAPARAZZI_II:
            *int_arg = s_light_data.data.paparazzi_2.lightness / 10;
        break;
        case LIGHT_MODE_FX_LIGHTNING_II:
            *int_arg = s_light_data.data.lightning_2.lightness / 10;
        break;
        case LIGHT_MODE_FX_TV_II:
            *int_arg = s_light_data.data.tv_2.lightness / 10;
        break;
        case LIGHT_MODE_FX_FIRE_II:
            *int_arg = s_light_data.data.fire_2.lightness / 10;
        break;
        case LIGHT_MODE_FX_STROBE_II:
            *int_arg = s_light_data.data.strobe_2.lightness / 10;
        break;
        case LIGHT_MODE_FX_EXPLOSION_II:
            *int_arg = s_light_data.data.explosion_2.lightness / 10;
        break;
        case LIGHT_MODE_FX_FAULT_BULB_II:
            *int_arg = s_light_data.data.fault_bulb_2.lightness / 10;
        break;
        case LIGHT_MODE_FX_PULSING_II:
            *int_arg = s_light_data.data.pulsing_2.lightness / 10;
        break;
        case LIGHT_MODE_FX_WELDING_II:
            *int_arg = s_light_data.data.welding_2.lightness / 10;
        break;
        case LIGHT_MODE_FX_COP_CAR_II:
            *int_arg = s_light_data.data.cop_car_2.lightness / 10;
        break;
        case LIGHT_MODE_FX_PARTY_LIGHTS_II:
            *int_arg = s_light_data.data.party_lights_2.lightness / 10;
        break;
        case LIGHT_MODE_FX_FIREWORKS_II:
            *int_arg = s_light_data.data.fireworks_2.lightness / 10;
        break;
        default:break;
    }
}

static void iot_fx_III_int_config(const uint8_t mode, uint8_t *int_arg, uint8_t *time_arg)
{
    *time_arg = SFX_MODE_TIME;
    switch (mode)
    {
        case LIGHT_MODE_FX_LIGHTNING_III:
            *int_arg = s_light_data.data.lightning_3.lightness / 10;
        break;
        case LIGHT_MODE_FX_TV_III:
            *int_arg = s_light_data.data.tv_3.lightness / 10;
        break;
        case LIGHT_MODE_FX_FIRE_III:
            *int_arg = s_light_data.data.fire_3.lightness / 10;
        break;
        case LIGHT_MODE_FX_FAULTY_BULB_III:
            *int_arg = s_light_data.data.fault_bulb_3.lightness / 10;
        break;
        case LIGHT_MODE_FX_PULSING_III:
            *int_arg = s_light_data.data.pulsing_3.lightness / 10;
        break;
        case LIGHT_MODE_FX_COP_CAR_III:
            *int_arg = s_light_data.data.cop_car_3.lightness / 10;
        break;
        default:break;
    }
}

static int preset_file_save(uint8_t *p_cfg)
{
    int res = -1;
	
	if(lfs_mount_fail == 0)
	{
		lfs_file_open(&lfs, &lfs_file_preset, preset_file_name, LFS_O_RDWR | LFS_O_CREAT);
		uint32_t file_size = sizeof(s_local_preset_data);
		lfs_file_rewind(&lfs, &lfs_file_preset);
		int32_t read_size = lfs_file_write(&lfs, &lfs_file_preset, p_cfg, file_size);
		lfs_file_close(&lfs, &lfs_file_preset);
		
		res = 0;
    }
	
    return res;
}

static int preset_file_init(void)
{
    int res = -1;
	
	if(lfs_mount_fail == 0)
	{
		lfs_file_open(&lfs, &lfs_file_preset, preset_file_name, LFS_O_RDWR | LFS_O_CREAT);
		uint32_t file_size = sizeof(s_local_preset_data);
		uint32_t file_read_size = lfs_file_size(&lfs, &lfs_file_preset);
		uint32_t read_size = lfs_file_read(&lfs, &lfs_file_preset, &s_local_preset_data, file_size);
		lfs_file_close(&lfs, &lfs_file_preset);
		
		if(s_local_preset_data.Head == LOCAL_DATA_HEAD && 
		   s_local_preset_data.Version == LOCAL_DATA_VER && 
		   s_local_preset_data.CheckSum == checksum_calculate((uint8_t *)&s_local_preset_data.preset_file, sizeof(s_preset_file)))
		{
			memcpy(&s_preset_file, &s_local_preset_data.preset_file, sizeof(s_preset_file));
			res = 0;
		}
	}
    return res;
}

static int light_data_init(void)
{
	int res = -1;
	
	if(lfs_mount_fail == 0)
	{
		lfs_file_open(&lfs, &lfs_file_light, light_file_name, LFS_O_RDWR | LFS_O_CREAT);
		uint32_t file_size = sizeof(s_local_light_data);
		uint32_t file_read_size = lfs_file_size(&lfs, &lfs_file_light);
		uint32_t read_size = lfs_file_read(&lfs, &lfs_file_light, &s_local_light_data, file_size);
		lfs_file_close(&lfs, &lfs_file_light);
		
		if(s_local_light_data.Head == LOCAL_DATA_HEAD && 
		   s_local_light_data.Version == LOCAL_DATA_VER && 
		   s_local_light_data.CheckSum == checksum_calculate((uint8_t *)&s_local_light_data.light_data, sizeof(s_light_data)))
		{
			memcpy(&s_light_data, &s_local_light_data.light_data, sizeof(s_light_data));
			res = 0;
		}
	}
    return res;
}

static int light_data_save(uint8_t *p_cfg, uint32_t len)
{
	if(lfs_mount_fail == 0)
	{
		lfs_file_open(&lfs, &lfs_file_light, light_file_name, LFS_O_RDWR | LFS_O_CREAT);
		lfs_file_rewind(&lfs, &lfs_file_light);
		int32_t read_size = lfs_file_write(&lfs, &lfs_file_light, p_cfg, len);
		lfs_file_close(&lfs, &lfs_file_light);
    }
}

static int sys_cfg_data_init(void)
{
	int res = -1;
	if(lfs_mount_fail == 0)
	{
		lfs_file_open(&lfs, &lfs_file_cfg, cfg_file_name, LFS_O_RDWR | LFS_O_CREAT);
		uint32_t file_size = sizeof(s_local_sys_data);
		uint32_t new_data_size = sizeof(s_local_sys_data.sys_cfg);
		uint32_t file_read_size = lfs_file_size(&lfs, &lfs_file_cfg);
		uint32_t read_size = lfs_file_read(&lfs, &lfs_file_cfg, &s_local_sys_data, file_size);
		lfs_file_close(&lfs, &lfs_file_cfg);
		
		if(s_local_sys_data.Head == LOCAL_DATA_HEAD && 
		   s_local_sys_data.Version == LOCAL_DATA_VER && 
		   s_local_sys_data.CheckSum == checksum_calculate((uint8_t *)&s_local_sys_data.sys_cfg, s_local_sys_data.sys_cfg.local_data_size))//拷贝全部
		{
			memcpy(&s_sys_cfg, &s_local_sys_data.sys_cfg, s_local_sys_data.sys_cfg.local_data_size);  
			res = 0;
		}
		else if(s_local_sys_data.Head == LOCAL_DATA_HEAD && 
		        s_local_sys_data.Version != LOCAL_DATA_VER &&   
		        s_local_sys_data.CheckSum == checksum_calculate((uint8_t *)&s_local_sys_data.sys_cfg, s_local_sys_data.sys_cfg.local_data_size))  //读取以前的数据
		{
			data_center_factory_reset(LOCAL_CONFIG_DATA);
			memcpy(&s_sys_cfg, &s_local_sys_data.sys_cfg, s_local_sys_data.sys_cfg.local_data_size); 
			res = 0;
		}
		else    //数据完全错误，恢复出厂设置
		{
			data_center_factory_reset(LOCAL_CONFIG_DATA);
			res = 0;
		}
	}
	
	return res;
}

static int sys_cfg_data_save(uint8_t *p_cfg, uint32_t len)
{
	int res = -1;
	if(lfs_mount_fail == 0)
	{
		lfs_file_open(&lfs, &lfs_file_cfg, cfg_file_name, LFS_O_RDWR | LFS_O_CREAT);
		lfs_file_rewind(&lfs, &lfs_file_cfg);
		int32_t read_size = lfs_file_write(&lfs, &lfs_file_cfg, p_cfg, len);
		lfs_file_close(&lfs, &lfs_file_cfg);
		
		res =0;
	}
	return res;
}

static void preset_file_wtrte_delete_handle(void)
{
//    for(uint16_t i = 0; i < PRESET_DATA_NUMBER; i++)
//    {
//        preset_file_operate(i, s_preset_file[i].operate, s_preset_file[i].buffer, sizeof(preset_data_t));
//        s_preset_file[i].operate = FILE_OPERATE_IDLE;
//    }
}

static void local_iot_ver_init(void)
{
    Device_Ver_TypeDef   local_iot_data_init;
    
    local_iot_data_init.Control_HwVer = 10;
    local_iot_data_init.Control_SwVer = 10;
    local_iot_data_init.Driver_HwVer = 0;
    local_iot_data_init.Driver_SwVer = 0;
    local_iot_data_init.Product_Machine = 0;
    local_iot_data_init.Product_LedType = 5;
    local_iot_data_init.Product_CCTRange = 75;
    local_iot_data_init.Product_CCTRange_L = 25;
    local_iot_data_init.Product_Function = 3;
    local_iot_data_init.CFX_MusicFX_support = 1;
    local_iot_data_init.CFX_PickerFX_support = 1;
    local_iot_data_init.CFX_TouchbarFX_support = 1;
    local_iot_data_init.ManualFX_support = 1;
    local_iot_data_init.ProgramFX_support = 1;
    local_iot_data_init.Protocol_Ver = BLUETOOTH_PROTO_VER;
    set_device_version_msg(&local_iot_data_init);
}

static void sys_iot_init(void)
{
    read_iot_data_from_store(s_iot_read_data, 256,0);
	read_iot_data_from_store(&s_iot_read_data[256], sizeof(IOT_Parameter_TypeDef)-256,1);
	recover_iot_data(s_iot_read_data);
}   

static void local_iot_data_save(void)
{
    static uint8_t iot_1s = 0;
    static uint8_t iot_60s = 0;
    static uint8_t ctr_mode_chage = LOCAL_RESET;
    static uint8_t chance_1s = 0;
    
    if(s_sys_info.ctrl_box.mode != MAX_CTR_WAY)
    {
        s_sys_info.ctrl_box.mode = MAX_CTR_WAY;
        ctr_mode_chage = LOCAL_SET;
        chance_1s = 0;
    }
    if(ctr_mode_chage == LOCAL_SET)
    {
        chance_1s++;
        if(chance_1s >= 99)
        {
            ctr_mode_chage = LOCAL_RESET;
            chance_1s = 0;
            set_ctr_way_msg((IOT_Control_Way_TypeDef)s_sys_info.ctrl_box.mode);
        }
    }
    
    iot_1s++;
    if(iot_1s > 99)
    {
        iot_1s = 0;
        iot_60s++;
        struct sys_info_power power;
        
        if (s_light_data.mode <= LIGHT_MODE_SOURCE)
        {
            s_local_iot_data.sidus_pro_fx_serial = MANUAL_EFFECTS_TYPE;
            
            iot_light_int_time_config(s_light_data.mode, &s_local_iot_data.int_val, (uint8_t*)&s_local_iot_data.mode_time_serial);
        }
        else if (s_light_data.mode >= LIGHT_MODE_FX_CLUBLIGHTS && s_light_data.mode <= LIGHT_MODE_FX_FIREWORKS)
        {
            s_local_iot_data.sidus_pro_fx_serial = MANUAL_EFFECTS_TYPE;
            
            iot_fx_I_int_time_config(s_light_data.mode, &s_local_iot_data.int_val, (uint8_t*)&s_local_iot_data.mode_time_serial);
        }
        else if(s_light_data.mode >= LIGHT_MODE_FX_PAPARAZZI_II && s_light_data.mode <= LIGHT_MODE_FX_FIREWORKS_II)
        {
            s_local_iot_data.sidus_pro_fx_serial = MANUAL_EFFECTS_TYPE;
            
            iot_fx_II_int_config(s_light_data.mode, &s_local_iot_data.int_val, (uint8_t*)&s_local_iot_data.mode_time_serial);
        }
        else if(s_light_data.mode >= LIGHT_MODE_FX_LIGHTNING_III && s_light_data.mode <= LIGHT_MODE_FX_COP_CAR_III)
        {
            s_local_iot_data.sidus_pro_fx_serial = MANUAL_EFFECTS_TYPE;
            
            iot_fx_III_int_config(s_light_data.mode, &s_local_iot_data.int_val, (uint8_t*)&s_local_iot_data.mode_time_serial);
        }
        data_center_read_sys_info(SYS_INFO_POWER, &power);
        if(power.state == 1)
            s_local_iot_data.device_state_serial = DEVICE_OPEN;
        else
            s_local_iot_data.device_state_serial = DEVICE_CLOSE;
        s_local_iot_data.run_time_serial = ADAPTER_SUPPLY_TIME;
        s_local_iot_data.fresnel_val = 361;
        s_local_iot_data.pitch_angle_val = 361;
        dispose_iot_parameter_data(&s_local_iot_data);
    }
    
    if(iot_60s > 59)
    {
        iot_60s = 0;
        memset(s_iot_read_data, 0, sizeof(s_iot_read_data));
        get_iot_data(s_iot_read_data);
		
        write_iot_data_to_store(s_iot_read_data, 256, 0);
        write_iot_data_to_store(s_iot_read_data+256, sizeof(IOT_Parameter_TypeDef)-256, 1);
        
    }
}

static uint32_t checksum_calculate(const uint8_t* p_data, uint32_t length)
{
    uint32_t checksum = 0;
    while (length--)
    {
        checksum += *p_data++;
    }
    return checksum;
}

uint8_t test111[20];

static int local_data_read(void)
{
	int res = 0;
	uint8_t *last_local_ver_addr = NULL;
	
	dev_w25qxx_read_data(SCANNER_G_FLASH_OFFS, s_sys_info.uuid.uuid, 20);
		
    sys_iot_init();
    read_cfg_data_from_store((uint8_t*)&s_local_data, sizeof(s_local_data));	
    if(s_local_data.Head == LOCAL_DATA_HEAD && s_local_data.Version == 0x10)
    {
		res = preset_file_init();
		data_center_factory_reset(LOCAL_PRESET_DATA);
		memcpy(&s_preset_file, &s_local_data.s_preset_file, sizeof(s_local_data.s_preset_file));  //拷贝以前的配置
		res = light_data_init();
		data_center_factory_reset(LOCAL_LIGHT_DATA);
		memcpy(&s_light_data, &s_local_data.s_light_data, sizeof(s_local_data.s_light_data));  //拷贝以前的配置
		res = sys_cfg_data_init();
		data_center_factory_reset(LOCAL_CONFIG_DATA);
		data_center_local_spcial_handle(&s_sys_cfg, &s_local_data.s_sys_cfg);  //拷贝以前的配置  针对软件版本1.1做特殊处理
    }
	else
	{
		preset_file_init();
		light_data_init();
		sys_cfg_data_init();
	}
}

static int local_data_save(void)
{
	//600ms左右时间
	s_local_sys_data.Head = LOCAL_DATA_HEAD;
	s_local_sys_data.Version = LOCAL_DATA_VER;
	s_sys_cfg.local_data_size = sizeof(s_sys_cfg);
	memcpy(&s_local_sys_data.sys_cfg, &s_sys_cfg, sizeof(s_sys_cfg));
	s_local_sys_data.CheckSum = checksum_calculate((uint8_t *)&s_local_sys_data.sys_cfg, sizeof(s_sys_cfg));
	sys_cfg_data_save((uint8_t *)&s_local_sys_data, sizeof(s_local_sys_data));
	s_local_light_data.Head = LOCAL_DATA_HEAD;
	s_local_light_data.Version = LOCAL_DATA_VER;
	memcpy(&s_local_light_data.light_data, &s_light_data, sizeof(s_light_data));
	s_local_light_data.CheckSum = checksum_calculate((uint8_t *)&s_local_light_data.light_data, sizeof(s_light_data));
	light_data_save((uint8_t *)&s_local_light_data, sizeof(s_local_light_data));
	s_local_preset_data.Head = LOCAL_DATA_HEAD;
	s_local_preset_data.Version = LOCAL_DATA_VER;
	memcpy(&s_local_preset_data.preset_file, &s_preset_file, sizeof(s_preset_file));
	s_local_preset_data.CheckSum = checksum_calculate((uint8_t *)&s_local_preset_data.preset_file, sizeof(s_preset_file));
	preset_file_save((uint8_t *)&s_local_preset_data);
    
    return 0;
}

uint8_t isFirmwareCorrect(sfirmwarebin* p_arg)
{
#if PROJECT_TYPE == 307
    if(0 != strcmp(p_arg->productname, "PR_307_L3"))
    {
        return false;
    }
#elif PROJECT_TYPE == 308
    if(0 != strcmp(p_arg->productname, "PR_308_L2"))
    {
        return 0;
    }
#endif
    if(p_arg->hardwareversion != HARDWARE_VERSION)//Base DataһݬөӲݾѦѾ
    {
        return 0;
    }
    
    if(p_arg->firmwaresize > UPDATE_AREA_SIZE)
    {
        return 0;
    }
}

static int ctr_flash_open(const char *name)
{
//    unsigned char i = 0;
    
    if(name == NULL)
    {
        return 1;
    }
    
    return 0;
}

static int ctr_flash_earse(uint32_t addr, uint32_t size)
{
    uint8_t res = 0;
    
    uint8_t number = (UPDATE_START_ADDRESS/1024/64);
    //默认2M大小删除
    for(uint8_t erase_number = number; erase_number < number+32; erase_number++)  //跳过图片+文本区
    {
        res = dev_w25qxx_erase_64sectors(erase_number);
        
        if(res == 0)
        {
            return 1;
        }
    }
    return 0;
}

static int ctr_flash_write(uint32_t addr, const uint8_t *data, uint32_t size)
{
    uint32_t star_addr = 0;
    uint32_t res = 0;
    
    if(data == NULL)   //空指针
    {
        return 1;
    }
    
    star_addr = addr + UPDATE_START_ADDRESS;
    res = dev_w25qxx_write_data(star_addr, data, size);
    
    if(res == 0)  //flash写失败
    {
        return 1;
    }
    
    return 0;
}

static int ctr_flash_colse(void *arg)
{
    uint8_t res = 0;
    const uint8_t *read_addr = 0;
    uint32_t crc = 0;
    uint32_t len = 0;
    
    sfirmwarebin *head_info;
    
    len = dev_w25qxx_get_mapaddr(UPDATE_START_ADDRESS, (const void **)&read_addr);
    if(read_addr == NULL || len == 0)   //映射失败
    {
        return 1;
    }
    head_info = (sfirmwarebin *)read_addr;
    
    res = FirmwareCorrect(head_info);
    if(res == 0)  //头信息校验错误
    {
        return 1;
    }
    
    crc = CRC32_Calculate(crc, (uint8_t *)(read_addr+FIRMWARE_HEAD_SIZE), head_info->firmwaresize);//校验数据
    if(crc != head_info->checkcrc32)
    {
        return 1;
    }
    
    return 0;
}

#if USE_BOOT_UPDATING == 1
static int boot_flash_open(const char *name)
{
//    unsigned char i = 0;
    
    if(name == NULL)
    {
        return 1;
    }
    
    return 0;
}

static int boot_flash_earse(uint32_t addr, uint32_t size)
{
    uint8_t res = 0;
    
	hal_flash_erase(BOOT_START_ADDRESS, size);
	
    return 0;
}

static int boot_flash_write(uint32_t addr, const uint8_t *data, uint32_t size)
{
    uint32_t star_addr = 0;
    uint32_t res = 0;
    
    if(data == NULL)   //空指针
    {
        return 1;
    }
    
    star_addr = addr + UPDATE_START_ADDRESS;
	hal_flash_write(star_addr, size, (uint8_t *)data);
    
    return 0;
}

static int boot_flash_colse(void *arg)
{
    uint8_t res = 0;
    
    return 0;
}
#endif

static void updata_init(void)
{
    struct update_device_ops ops = 
    {
        .open  = ctr_flash_open,
        .erase = ctr_flash_earse,
        .write = ctr_flash_write,
        .close = ctr_flash_colse,                 
    };
#if USE_BOOT_UPDATING == 1
    struct update_device_ops ops1 = 
    {
        .open  = boot_flash_open,
        .erase = boot_flash_earse,
        .write = boot_flash_write,
        .close = boot_flash_colse,                 
    };
	update_device_regsiter(USB_UPDATA_BOOT_DEVICE, &ops1);
#endif
    //注册升级设备
    update_device_regsiter(USB_UPDATA_BOX_DEVICE, &ops);  
}

static void iot_error_evnet_pull_cb(uint32_t event_type, uint32_t event_value)
{
//    uint8_t error_type;
//    
//    if(event_type == MAIN_EV_DATA)
//    {
//        if(event_value == EV_DATA_IOT_ERROR)
//        {
//            error_type = os_ev_get_event_data(event_type, event_value);
//            set_iot_error_data((IOT_Error_Msg_TypeDef)error_type);
//        }
//    }
}

void data_center_all_sys_fx_stop(void)
{
    s_light_data.data.explosion.state = 0;
    s_light_data.data.club_lights.state = 0;
    s_light_data.data.color_chase.state = 0;
    s_light_data.data.color_cycle.status = 0;
    s_light_data.data.color_fade.status = 0;
    s_light_data.data.cop_car.state = 0;
    s_light_data.data.cop_car_2.state = 0;
    s_light_data.data.cop_car_3.state = 0;
    s_light_data.data.explosion_2.state = 0;
    s_light_data.data.fault_bulb.state = 0;
    s_light_data.data.fault_bulb_2.state = 0;
    s_light_data.data.fault_bulb_3.state = 0;
    s_light_data.data.fire.state = 0;
	s_light_data.data.fire_2.state = 0;
    s_light_data.data.fireworks_2.state = 0;
    s_light_data.data.lightning.state = 0;    
    s_light_data.data.paparazzi.state = 0;
    s_light_data.data.pulsing.state = 0;
    s_light_data.data.strobe.state = 0;
    s_light_data.data.tv.state = 0;
	s_light_data.data.tv_2.state = 0;
    s_light_data.data.fireworks.state = 0;
	s_light_data.data.candle.state = 0;
	s_light_data.data.welding.state = 0;
	s_light_data.data.welding_2.state = 0;
	s_light_data.data.party_lights.state = 0;
	s_light_data.data.party_lights_2.state = 0;
	s_light_data.data.cfx.ctrl = 0;
    //TODO
//    s_local_data_save_flag = LOCAL_SET;
    os_ev_publish_event(MAIN_EV_DATA, EV_DATA_LIGHT_CHANGE, date_center_get_light_mode());
}

static void data_center_local_spcial_handle(sys_config_t *dest_data, sys_config_t *last_data)
{
	dest_data->local_ui = last_data->output_mode;
	dest_data->local_sub_ui = last_data->lang;
	dest_data->output_mode = last_data->screensaver_index;
	dest_data->lang = last_data->curve_type;
	dest_data->screensaver_index = last_data->dmx_locked;
	dest_data->curve_type = last_data->dmx_addr;
	dest_data->dmx_locked = last_data->dmx_addr & 0x00FF;
	dest_data->dmx_addr = (last_data->dmx_extension_index & 0xFF) << 8 | last_data->dmx_profile_index;
	dest_data->dmx_loss_behavior = last_data->dmx_temination_on;
	dest_data->dmx_profile_index = (last_data->dmx_fade_time & 0xFF00) >> 8;
	dest_data->dmx_extension_index = last_data->dmx_fade_time & 0x00FF;
	dest_data->dmx_temination_on = (last_data->fan_mode & 0x0FF);
	dest_data->dmx_fade_time = ((last_data->frequency & 0x00FF) << 8) | (last_data->light_effect_index);
	dest_data->fan_mode = (last_data->frequency&0xFF00) >> 8;
	dest_data->light_effect_index = last_data->local_data_size&0x00FF;
	dest_data->frequency = (last_data->local_data_size & 0xFF00) >> 8 | last_data->ble_on;
	dest_data->local_data_size = (last_data->crmx_on << 8) | last_data->studiomode_on;
	dest_data->ble_on = last_data->work_mode;
	dest_data->crmx_on = last_data->local_ip[0];
	dest_data->studiomode_on = last_data->local_ip[1];
	dest_data->work_mode = last_data->local_ip[2];
	dest_data->local_ip[0] = last_data->local_ip[3];
	dest_data->local_ip[1] = last_data->netmask[0];
	dest_data->local_ip[2] = last_data->netmask[1];
	dest_data->local_ip[3] = last_data->netmask[2];
	dest_data->netmask[0] = last_data->netmask[3];
	dest_data->netmask[1] = last_data->gateway[0];
	dest_data->netmask[2] = last_data->gateway[1];
	dest_data->netmask[3] = last_data->gateway[2];
	dest_data->gateway[0] = last_data->gateway[3];
	dest_data->gateway[1] = last_data->custom_fx_type;
	dest_data->gateway[2] = last_data->custom_fx_sel[0];
	dest_data->gateway[3] = last_data->custom_fx_sel[1];
	dest_data->ble_sn_number[0] = last_data->ble_sn_number[3];
	dest_data->ble_sn_number[1] = last_data->ble_sn_number[4];
	dest_data->ble_sn_number[2] = last_data->ble_sn_number[5];
	dest_data->ble_sn_number[3] = last_data->hs_mode;
	dest_data->ble_sn_number[4] = (last_data->yoke_pan_limit[0] & 0xFF00) >> 8;
	dest_data->ble_sn_number[5] = last_data->yoke_pan_limit[0] & 0x00FF;
	dest_data->hs_mode = (last_data->yoke_pan_limit[1] & 0xFF00) >> 8;
	if(dest_data->hs_mode > 1)
		dest_data->hs_mode = 0;
	memcpy(dest_data->yoke_pan_limit, s_sys_cfg_dafault.yoke_pan_limit, sizeof(s_sys_cfg_dafault.yoke_pan_limit));
	memcpy(dest_data->yoke_tilt_limit, s_sys_cfg_dafault.yoke_tilt_limit, sizeof(s_sys_cfg_dafault.yoke_tilt_limit));
	dest_data->angle_limit_starte = last_data->yoke_tilt_limit[0] & 0x00FF;
	dest_data->access_select = (last_data->yoke_tilt_limit[1] & 0xFF00) >> 8;
	memcpy(dest_data->yoke_pan_limit1, s_sys_cfg_dafault.yoke_pan_limit1, sizeof(s_sys_cfg_dafault.yoke_pan_limit1));
	memcpy(dest_data->yoke_tilt_limit1, s_sys_cfg_dafault.yoke_tilt_limit1, sizeof(s_sys_cfg_dafault.yoke_tilt_limit1));
	memcpy(dest_data->pan_demo_limit, s_sys_cfg_dafault.pan_demo_limit, sizeof(s_sys_cfg_dafault.pan_demo_limit));
	memcpy(dest_data->tili_demo_limit, s_sys_cfg_dafault.tili_demo_limit, sizeof(s_sys_cfg_dafault.tili_demo_limit));
	memcpy(dest_data->zoom_demo_limit, s_sys_cfg_dafault.zoom_demo_limit, sizeof(s_sys_cfg_dafault.zoom_demo_limit));
	dest_data->effects_mode = dest_data->local_ui;
	dest_data->eth_state_set = last_data->power_limit;
	dest_data->eth_artnet_universe = last_data->demo_state << 8 | last_data->pan_demo_state;
	dest_data->eth_sacn_universe = last_data->pan_demo_limit[0];
	dest_data->demo_state = 0;
	dest_data->power_value = (last_data->tili_demo_limit[0] & 0x00FF) << 8 | last_data->tili_demo_state;
	dest_data->pan_demo_state = 0;
	dest_data->tili_demo_state = 0;
	dest_data->zoom_demo_state = 0;
	dest_data->power_on_state = 0;
}

#if DATA_CENTRE_TEST_EN
static void data_center_test(void)
{
    data_center_write_light_data(s_test_light_mode, &s_local_test_write_light_data.hsi_arg);
    data_center_read_light_data(s_test_light_mode, &s_local_test_read_light_data.hsi_arg);
    data_center_write_preset_data(0, &s_local_test_write_preset_data);
    data_center_read_preset_data(0, &s_local_test_read_preset_data);
    data_center_delete_preset_data(0);
    data_center_read_preset_data(0, &s_local_test_read_preset_data);
    switch((uint8_t)s_local_test_data_mode)
    {
        case SYS_CONFIG_LOCAL_UI:
            data_center_write_config_data(s_local_test_data_mode, &s_local_test_write_data.local_ui);
            data_center_read_config_data(s_local_test_data_mode, &s_local_test_read_data.local_ui);
        break;
        case SYS_CONFIG_OUTPUT_MODE:
            data_center_write_config_data(s_local_test_data_mode, &s_local_test_write_data.output_mode);
            data_center_read_config_data(s_local_test_data_mode, &s_local_test_read_data.output_mode);
        break;
        case SYS_CONFIG_LANG:
            data_center_write_config_data(s_local_test_data_mode, &s_local_test_write_data.lang);
            data_center_read_config_data(s_local_test_data_mode, &s_local_test_read_data.lang);
        break;
        case SYS_CONFIG_DMX_LOCKED:
            data_center_write_config_data(s_local_test_data_mode, &s_local_test_write_data.dmx_locked);
            data_center_read_config_data(s_local_test_data_mode, &s_local_test_read_data.dmx_locked);
        break;
        case SYS_CONFIG_DMX_ADDR:
            data_center_write_config_data(s_local_test_data_mode, &s_local_test_write_data.dmx_addr);
            data_center_read_config_data(s_local_test_data_mode, &s_local_test_read_data.dmx_addr);
        break;
        case SYS_CONFIG_WORK_MODE:
            data_center_write_config_data(s_local_test_data_mode, &s_local_test_write_data.work_mode);
            data_center_read_config_data(s_local_test_data_mode, &s_local_test_read_data.work_mode);
        break;
        case SYS_CONFIG_LIGHT_EFFECT_IDX:
            data_center_write_config_data(s_local_test_data_mode, &s_local_test_write_data.light_effect_index);
            data_center_read_config_data(s_local_test_data_mode, &s_local_test_read_data.light_effect_index);
        break;
        case SYS_CONFIG_BLE_CTRL:
            data_center_write_config_data(s_local_test_data_mode, &s_local_test_write_data.ble_on);
            data_center_read_config_data(s_local_test_data_mode, &s_local_test_read_data.ble_on);
        break;
        case SYS_CONFIG_CRMX_CTRL:
            data_center_write_config_data(s_local_test_data_mode, &s_local_test_write_data.crmx_on);
            data_center_read_config_data(s_local_test_data_mode, &s_local_test_read_data.crmx_on);
        break;
        case SYS_CONFIG_LOCAL_IP:
            data_center_write_config_data(s_local_test_data_mode, &s_local_test_write_data.local_ip);
            data_center_read_config_data(s_local_test_data_mode, &s_local_test_read_data.local_ip);
        break;
        case SYS_CONFIG_NETMASK:
            data_center_write_config_data(s_local_test_data_mode, &s_local_test_write_data.netmask);
            data_center_read_config_data(s_local_test_data_mode, &s_local_test_read_data.netmask);
        break;
        case SYS_CONFIG_GATEWAY:
            data_center_write_config_data(s_local_test_data_mode, &s_local_test_write_data.gateway);
            data_center_read_config_data(s_local_test_data_mode, &s_local_test_read_data.gateway);
        break;
        case SYS_CONFIG_CUSTOM_FX_TYPE:
            data_center_write_config_data(s_local_test_data_mode, &s_local_test_write_data.custom_fx_type);
            data_center_read_config_data(s_local_test_data_mode, &s_local_test_read_data.custom_fx_type);
        break;
        case SYS_CONFIG_CUSTOM_FX_SEL:
            data_center_write_config_data(s_local_test_data_mode, &s_local_test_write_data.custom_fx_sel);
            data_center_read_config_data(s_local_test_data_mode, &s_local_test_read_data.custom_fx_sel);
        break;
        case SYS_CONFIG_CUSTOM_FX_INTENSITY:
            data_center_write_config_data(s_local_test_data_mode, &s_local_test_write_data.custom_fx_intensity);
            data_center_read_config_data(s_local_test_data_mode, &s_local_test_read_data.custom_fx_intensity);
        break;
        case SYS_CONFIG_UUID:
            data_center_write_config_data(s_local_test_data_mode, &s_local_test_write_data.uuid);
            data_center_read_config_data(s_local_test_data_mode, &s_local_test_read_data.uuid);
        break;
        case SYS_CONFIG_BLE_SN:
            data_center_write_config_data(s_local_test_data_mode, &s_local_test_write_data.ble_sn_number);
            data_center_read_config_data(s_local_test_data_mode, &s_local_test_read_data.ble_sn_number);
        break;

        default:break;
    }
}
#endif
