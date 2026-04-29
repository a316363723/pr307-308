#ifndef UI_DATA_H
#define UI_DATA_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include <stdint.h>
#include <stdbool.h>
#include "base_type.h"
#include "app_data_center.h"
#include "ui_config.h"
#include "project_config.h"

/*********************
 *      DEFINES
 *********************/
#define PRESET_TASK_MAXIUM      10
#define UI_DUMMY_DATA_TEST      0

#define GET_ARRAY_SIZE(x)       ( sizeof((x)) / sizeof((x)[0]) )

#define SYS_MENU_TITLE_TO_TOP   10

#define UI_UPGRADE_TIMEOUT_MAX   23   //升级超时时间   单位s

#define UI_DATA_VERSION         0x10
#define UI_DATA_MAGIC_NUMBER    ((0xAA55CC00) | UI_DATA_VERSION)

#define gel_model_get_index(p_model)   ((p_model)->color[(p_model)->brand][(p_model)->type[(p_model)->brand]])
#define gel_model_get_series(p_model)  ((p_model)->type[(p_model)->brand])
#define gel_model_get_brand(p_model)   ((p_model)->brand)

#define gel_model_set_series(p_model, _s)   ((p_model)->type[(p_model)->brand] = (_s))
#define gel_model_set_brand(p_model, _b)    ((p_model)->brand = (_b))
#define gel_model_set_index(p_model, _i)    ((p_model)->color[(p_model)->brand][(p_model)->type[(p_model)->brand]] = (_i))

/**********************
 *      TYPEDEFS
 **********************/
typedef struct {
    float lower_limit_angle;
    float upper_limit_angle;
}aputure_limit_angle_type;

typedef struct {
    int16_t x;
    int16_t y;
} point_t;

//描述色纸结构体
typedef struct _gel_dsc_t {
    const char* name;
    uint32_t hex_color;    
    uint16_t number;
    uint16_t dmx_value_min;
    uint16_t dmx_value_max;
    uint16_t dmx_channel;
    uint8_t series;
}gel_dsc_t;

typedef struct {
	uint16_t coord_x;
	uint16_t coord_y;
}src_tab_t;

typedef struct {
    const char* name;
    src_tab_t value;
}src_table_t;

// typedef struct {
//     uint8_t output_mode;
//     uint8_t lang_index;
//     uint8_t locked : 1;
//     uint8_t dmx_loss_behavior;    
//     uint8_t screensaver_index;
//     uint16_t dmx_addr;
//     uint8_t dmx_profile_index;
//     uint8_t curve_type;
//     uint8_t fan_mode;
//     uint8_t light_effect_index;
//     uint16_t frequency;
//     uint8_t ble_on : 1;
//     uint8_t lumenradio_on : 1;
//     uint8_t studiomode_on : 1;
//     uint8_t dmx_temination_on : 1;
//     uint8_t work_mode : 1;
//     uint8_t local_ip[4];
//     uint8_t remote_ip[4];
//     uint8_t gateway_ip[4];

//     uint8_t start_fx : 1;
//     uint8_t custom_fx_type : 3;
//     uint8_t custom_fx_sel[CUSTOM_FX_TYPE_MAX];
//     int16_t custom_fx_int[CUSTOM_FX_TYPE_MAX][10];
//     uint8_t ble_sn_number[7];
//     uint8_t uuid[20];
// }page_sys_menu_t;

typedef struct {
    uint8_t top_id;     //顶层的ID.
    uint8_t page_id;    //当前需记录的page_id.
    uint8_t page_spid;  //当前需要记录页面的子ID.
    uint8_t last_id;    //最后一次的ID
}pm_data_t;

typedef struct _ui_data{
    uint32_t                     magic_number;
    struct db_rgb    rgb_model;
    struct db_hsi    hsi_model;    
    struct db_xy     xy_model;
    struct db_gel    gel_model;
    struct db_source source_model;
    struct db_cct    cct_model;  
   
#if (PROJECT_TYPE == 308)  
    struct db_fx_explosion          explosion_model;
    struct db_fx_fault_bulb         faultybulb_model;
    struct db_fx_pulsing            pulsing_model; 
    struct db_fx_strobe             strobe_model;
    struct db_fx_lightning          lightning_model;
    struct db_fx_tv                 tv_model;
    struct db_fx_fireworks          fireworks_model;
    struct db_fx_fire               fire_model;
#else    
    struct db_fx_explosion_2        explosion2_model;
    struct db_fx_fault_bulb_2       faultybulb2_model;
    struct db_fx_strobe_2           strobe2_model;
    struct db_fx_lightning_2        lightning2_model;
    struct db_fx_fireworks          fireworks_model;  //need change
    struct db_fx_fire_2             fire2_model;
	struct db_fx_strobe             strobe_model;
	struct db_fx_explosion          explosion_model;
	struct db_fx_fault_bulb         faultybulb_model;
#endif
    struct db_fx_tv_2               tv2_model;
    struct db_fx_pulsing_2          pulsing2_model;
    struct db_fx_welding_2          welding2_model;    
    struct db_fx_cop_car_2          copcar2_model;
    struct db_fx_candle             candle_model;
    struct db_fx_club_lights        clublights_model;
    struct db_fx_color_chase        colorchase_model;
    struct db_fx_paparazzi          paparazzi_model;
    struct db_fx_party_lights       partylight_model;
    struct db_local_custom_fx       local_fx;    
    struct db_custom_fx             cfx_ctrl;
    sys_config_t                    sys_menu_model;
    pm_data_t                       pm_data_model;
	uint32_t                        checksum;
#ifdef UI_PLATFORM_MCU
//}UI_Data_t __attribute__ ((aligned (1)));;
}UI_Data_t;
#else
}UI_Data_t;
#endif

typedef struct _batt_dsc {
    uint8_t   is_exist : 1;
    uint8_t   state : 2;         // Battery state
    uint8_t   is_low_power : 1;  // 显示低电量
    uint16_t  percent;       // 百分百
    uint32_t  remain_time_m; // 剩余时间  
}batt_dsc_t;

typedef struct _batt_sys {
    batt_dsc_t left;
    batt_dsc_t right;
}batt_sys_t;

typedef struct _ui_batt_dsc { 
    bool      b_low_power;
    uint8_t   state;         // Battery state
    uint16_t  percent;       // 百分百
    uint32_t  remain_time_m; // 剩余时间      
}ui_batt_dsc_t;

typedef struct _ui_dc_batt_dsc {
    uint8_t disp_index : 2;
    bool b_anim_start;
    batt_dsc_t batt_sys[2];                
}ui_dc_batt_dsc_t;

typedef struct _ui_power_dsc {
    uint8_t type;
    union {
        ui_batt_dsc_t batt;
        ui_dc_batt_dsc_t dc_with_batt;
    } dsc;
}ui_power_dsc_t;

typedef struct _title_t{
    uint8_t page_id;
    uint8_t fan_mode;
    uint8_t curve_type;
    uint8_t power_type;
	uint8_t indoor_power_type;
    uint8_t com_type;
    uint8_t work_mode : 1;
    uint8_t dmx_temination_on : 1;
	uint8_t high_speed_mode : 1;
    ui_power_dsc_t power_dsc;
}ui_title_t;

enum {
    BLE_RESET_IDLE = 0,
    BLE_RESET_START,
    BLE_RESETING,
    BLE_RESET_SUCCEED,
    BLE_RESET_FAILED,    
};

typedef struct _ble_status{
    uint8_t  status;
    uint8_t  percent;
    uint32_t timecnt;
}ble_status_t;

typedef struct _lumenradio_status{
    uint8_t status;
    uint8_t unpair_status;
    uint8_t force_upd : 1;
}lumenradio_status_t;

typedef struct _firmware_upd{
    uint8_t status;
    uint8_t percent;
    uint8_t upd_enable : 1;
    uint8_t fw_type : 1;
}firmware_upd_t;

typedef struct _version {
    uint8_t hw_ver;
    uint8_t soft_ver;
}version_t;

typedef struct _batt_option {
    uint8_t state;
    uint8_t percent;
    uint8_t power_switch : 1;
    uint8_t is_support_fullpower : 1;
}batt_option_t;

typedef struct _electric_accessory
{
    uint8_t             electric_accessory_type;  //电动支架类型
    float            ele_fresnel_angle;
    uint8_t             ele_no_access_sel;
    uint8_t             ele_yoke_angle_limit_on;
    int16_t            ele_yoke_tilt[2];  //电动支架光学附件俯仰  最小最大值
    int16_t            ele_yoke_pan[2];  //电动支架光学附件水平
    uint8_t             ele_yoke_angle_limit_on1[12];
    int16_t            ele_yoke_tilt1[12][2];  //电动支架非光学附件俯仰  最小最大值
    int16_t            ele_yoke_pan1[12][2];  //电动支架非光学附件水平
    float               ele_yoke_tilt_val;
    float               ele_yoke_pan_val;
    float               ele_yoke_tilt_val1[12];
    float               ele_yoke_pan_val1[12];
    uint8_t             ele_yoke_tilt_lock;
    uint8_t             ele_yoke_pan_lock;
    uint8_t             ele_yoke_tilt_lock1[12];
    uint8_t             ele_yoke_pan_lock1[12];
    uint8_t             block_calib;
    uint8_t             rst_origin;
	uint8_t             demo_state;	
	uint8_t             pan_demo_state;	
	int16_t             pan_demo_limit[2];	
	uint8_t             tili_demo_state;	
	int16_t             tili_demo_limit[2];
	uint8_t             zoom_demo_state;	
	int16_t             zoom_demo_limit[2];
	uint8_t             power_on_state;

}electric_accessory_t;

typedef enum {
    CALIBRATE_POWER_IDLE,
	CALIBRATE_POWER_PWM_START,
	CALIBRATE_POWER_ANALOG_START,
	CALIBRATE_POWER_ING,
	CALIBRATE_POWER_PWM_OK,
	CALIBRATE_POWER_ANALOG_OK,
	CALIBRATE_POWER_FAIL,
}calibrate_power_t;

typedef enum {
    INDUSTRIAL_MODE_IDLE,
	INDUSTRIAL_MODE_START,
	INDUSTRIAL_MODE_ING,
	INDUSTRIAL_MODE_OK,
	INDUSTRIAL_MODE_FAIL,
}industrial_mode_t;
typedef struct _ui_motor_t {
    uint16_t  state1; 
    uint16_t  state2;
    uint16_t lateral_angle;
    uint16_t pitch_angle;
    uint16_t zoom_angle;
    uint16_t lateral_max;
    uint16_t lateral_min;
    uint16_t pitch_max;
    uint16_t pitch_min;
    uint16_t zoom_max;
    uint16_t zoom_min;
}ui_motor_test_t;


typedef struct _ui_test {
    int32_t Bat1Vout_mV;
    int32_t Bat1DischargeCurrent_mA;
    int32_t Bat1ChargeCurrent_mA;
    int32_t Bat2Vout_mV;
    int32_t Bat2DischargeCurrent_mA;
    int32_t Bat2ChargeCurrent_mA;
    int32_t COBVin_mV;
    int32_t COBIin_mA;
    int32_t AdapterVout_mV;
    int32_t AdapterIout_mA;
    int32_t FanRealRPM;
    int32_t FanAdjRPM;
    int32_t LampFanAdjRPM;
    int32_t LampFanRealRPM;
	int32_t SupplyPower;     
    uint32_t RemainTime;
    int8_t  NTCTemp;
    int8_t  LampNTCTemp;
    int32_t DC48_Vout_mV;
    int32_t DC48_Iout_mA;
	int32_t CalibratePower;
	uint8_t CalibrateState;
	uint8_t CalibrateErrFlag;     
    uint8_t IndustrialMode;
    uint8_t PwmMode;
    uint8_t PumpMode;
	uint8_t power_set_mode;
	uint8_t power_set_value;
	uint8_t motor_test_mode;
	uint8_t clear_run_time_flag;
	uint8_t clear_davice_run_time_flag;
	int32_t powercalibratePower;
	uint8_t powercalibrateState;
    uint16_t rgbww[7];
    uint16_t pump_speed;
    uint16_t motor_test[4];
	uint8_t    zoom1_Temp;
	uint8_t    zoom2_Temp;
	uint8_t Fresnel_Industrial;
}ui_test_data_t;


typedef struct _ui_time_t {
    uint32_t year; //从2000年开始
    uint8_t month;
    uint8_t day;
    uint8_t hour;
    uint8_t minute;
    uint8_t second;
}ui_time_t;

typedef struct _ui_state_data {
    ble_status_t        ble_status;
    lumenradio_status_t lumen_status;
    version_t           ctrlbox;
    version_t           lamp;
    firmware_upd_t      upd_status;
    ui_title_t          title;        
    ui_test_data_t      test_data;
    batt_sys_t          batt_sys;
    batt_option_t       batt_option;
    uint8_t             mac_address[6];
    ui_time_t           upd_time;
    uint8_t             lan_state : 1;
    uint8_t             dmx_wired : 1;
    uint8_t             refresh : 1; 
    uint8_t             light_up : 1;
	uint8_t             trigger_explosion_state;
    uint8_t             trigger_explosion_anim : 1;
	uint8_t             trigger_explosion_anim_state : 1;
    uint8_t             trigger_light_anim : 1;
	uint8_t             trigger_light_anim_state : 1;
    uint8_t             work_mode_sync : 1;
    electric_accessory_t ele_accessory;
	uint8_t         	box_type;
	uint8_t             firework_reset : 1;
	uint8_t         	eth_state;
	uint8_t         	eth_universe_state;
	uint16_t         	eth_artnet_universe;
	uint16_t         	eth_sacn_universe;      
	bool                error_code_event;
	bool                high_speed_switch;
	bool				regognition_status;
	bool                regognition_error;
    ///uint8_t             electric_accessory_type;  //电动支架类型
}UI_State_Data_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/
extern UI_Data_t g_tUIAllData;
extern UI_State_Data_t* g_ptUIStateData;
extern aputure_limit_angle_type  aputure_limit_angle_value[7];

const gel_dsc_t* gel_get_serial_dsc(uint8_t brand, uint8_t serials);
const char* gel_get_serial_name(uint8_t brand, uint8_t serials);
uint8_t gel_get_max_num(uint8_t brand, uint8_t serials);
const src_table_t* source_get_table(uint8_t src_index);
const char* source_get_name(uint8_t src_index);

/* 预设任务相关API */
void ui_preset_task_del(uint8_t index);
void ui_preset_task_save(uint8_t index, preset_data_t* task);
void ui_preset_task_get(uint8_t index, preset_data_t* param);
uint8_t ui_preset_task_apply(uint8_t index, preset_data_t *task);

void ui_set_param_setting(param_setting_type_t type, struct db_fx_mode_arg* setting); 
uint8_t ui_param_setting_get_page(param_setting_type_t type);
uint8_t ui_limit_setting_get_page(param_setting_type_t type);
void ui_set_limit_setting(param_setting_type_t type, struct db_fx_mode_limit_arg* setting);

const char* dmx_profile_get_name(int8_t type);

const char* ui_get_custom_fx_name(uint8_t type);

/* GEL相关API */
void ui_set_gel_brand(struct db_gel* p_gel_model, uint8_t brand);
uint8_t ui_get_gel_brand(struct db_gel* p_gel_model);
void ui_set_gel_series(struct db_gel* p_gel_model, uint8_t brand, uint8_t series);
void ui_set_gel_index(struct db_gel* p_gel_model, uint8_t brand, uint8_t series, uint8_t index);
uint8_t ui_get_gel_index(struct db_gel* p_gel_model, uint8_t brand, uint8_t series);

pm_data_t* ui_get_pm_data(void);
void ui_set_hs_mode_state(uint8_t state);
bool ui_get_master_mode(void);
void ui_set_master_mode(uint8_t type);
bool ui_get_dmx_temination_mode(void);
void ui_set_dmx_temination_mode(uint8_t state);
uint8_t ui_get_fan_mode(void);
void ui_set_fan_mode(uint8_t type);
uint8_t ui_get_curve_type(void);
void ui_set_curve_type(uint8_t type);
void ui_set_comm_type(uint8_t type);
uint8_t ui_get_comm_type(void);
void ui_set_lan_state(uint8_t state);
void ui_set_ble_state(uint8_t state);
void ui_set_crmx_state(uint8_t state);
uint8_t ui_get_power_type(void);
uint8_t ui_get_lang_type(void);
uint8_t *ui_get_ble_sn(void);
uint8_t ui_get_cfx_read_bank_state(void);
void ui_set_cfx_read_bank_state(uint8_t state);
void ui_set_work_mode(uint8_t type);
void ui_set_lang_type(uint8_t lang);
uint8_t ui_get_hight_speed_mode(void);
void ui_set_hight_speed_mode(uint8_t type);

void ui_restore_def_setting(void);
void ui_reset_ble_status(void);
void ui_reset_lumen_status(void);
void ui_set_lan_type(bool state);
uint8_t ui_light_effect_index_get(void);
void ui_light_effect_index_save(uint8_t index);
void crmx_is_paired_set( bool state );
uint8_t crmx_is_paired(void);
uint8_t crmx_get_link_streng(void);
const char* lighteffect_get_item_str(uint8_t id);
bool ui_data_version_check(UI_Data_t* p_data);
void ui_set_crmx_circle_state(uint8_t state);
void ui_set_power_calibration_state(uint8_t state);
uint8_t ui_get_power_calibration_state(void);
void ui_set_power(uint16_t power);
uint16_t ui_test_lamp_fan_get(void);
uint16_t ui_test_lamp_pump_speed_get(void);
uint8_t ui_test_lamp_pump_mode_get(void);
uint8_t ui_test_Industrial_mode_get(void);
void ui_set_Lightning_2_state(uint8_t state);
void ui_set_explosion_2_anim(uint8_t state);
void ui_test_rgbpwm_mode_set(uint8_t mode);
uint8_t ui_get_pwm_PwmMode(void);
void ui_get_pwm_rgbww(uint16_t data[]);
uint16_t ui_test_box_fan_get(void);
void ui_test_data_init(void);
void gui_ble_reset_event_gen(void);
void gui_factory_event_gen(void);
void gui_data_sync_event_gen(void);
void gui_page_sync_event_gen(void);
void gui_crmx_unpair_event_gen(void);
uint8_t ui_get_dmx_state(void);
void ui_set_dmx_state(uint8_t state);
void ui_set_lumen_name(uint8_t *reset_flag, uint16_t time);
void ui_get_mac_addr(void);
/* 临界代码段API */
void ui_error_code_event_set(bool state);
bool ui_error_code_event_get(void);
void ui_enter_critical(void);
void ui_exit_critical(void);
void ui_test_data_set(void);
void ui_set_box_type(uint8_t state);
void page_error_info_loop(void);
uint16_t ui_test_motor_state_get(void);
uint16_t ui_test_motor_count_get(void);
void ui_set_power_type(uint8_t state);
void ui_set_explosion_2_state(uint8_t state);
uint8_t ui_set_explosion_2_state_get(void);
void ui_set_lightning_2_trigger(uint8_t trigger);
uint8_t ui_get_eth_universe_state(void);
void ui_set_eth_universe_state(uint8_t state);
uint16_t ui_get_eth_artnet_universe(void);
void ui_set_eth_artnet_universe(uint16_t universe);
uint16_t ui_get_eth_sacn_universe(void);
void ui_set_eth_sacn_universe(uint16_t universe);
void ui_set_ctr_spid_index(uint8_t index);
uint8_t ui_get_ctr_spid_index(void);
uint8_t ui_get_eth_state(void);
void ui_set_eth_state(uint8_t state);
void ui_get_dhcp_ip_addr(uint8_t *addr);
void ui_get_dhcp_netmask_addr(uint8_t *addr);
void ui_get_dhcp_gateway_addr(uint8_t *addr);
uint64_t gui_get_64type_convert(uint8_t value);
uint8_t gui_get_indoor_flag(void);
uint8_t ui_accessory_pan_lock_get(uint8_t mode);
uint8_t ui_accessory_tilt_lock_get(uint8_t mode);
void ui_accessory_pan_lock_set(uint8_t mode, uint8_t value);
void ui_accessory_tilt_lock_set(uint8_t mode, uint8_t value);
uint8_t ui_get_hs_mode_state(void);
uint8_t ui_get_trigger_cfx_state(void);
void ui_set_trigger_cfx_state(uint8_t state);
uint8_t ui_get_high_speed_switch(void);
void ui_set_high_speed_switch(bool state);
void ui_test_power_mode_set(uint8_t mode);
uint8_t ui_test_power_mode_get(void);
uint8_t ui_test_power_value_get(void);
void gui_lfm_seng_data_set_flag(uint8_t flag);
uint8_t  gui_lfm_seng_data_get_flag(void);
void ui_test_motor_test_set(uint8_t mode);
uint8_t ui_test_motor_test_get(void);
void gui_power_event_gen(void);
void gui_electric_move_event_gen(void);
uint8_t gui_get_hs_mode(void);
uint8_t gui_get_dmx_profile_addr(void);
void ui_fresnel_get_angle_flag_set(uint8_t mode);
uint8_t ui_fresnel_get_angle_flag_get(void);
uint8_t get_lfm_port_mode(void);
void ui_updata_start_flag_set(uint8_t mode);
uint8_t ui_updata_start_flag_get(void);
void ui_set_single_power_calibration_state(uint8_t state);
uint8_t ui_get_single_power_calibration_state(void);
void ui_max_power_flag_set(uint8_t mode);
uint8_t ui_max_power_flag_get(void);
void ui_ctr_clear_time_flag_set(uint8_t state);
uint8_t ui_ctr_clear_time_flag_get(void);
void ui_drv_clear_time_flag_set(uint8_t state);
uint8_t ui_drv_clear_time_flag_get(void);
float ui_accessory_get_angle_limit(float angle, uint8_t mode);	
void ui_motor_reset_status_set(uint8_t state);
uint8_t ui_motor_reset_status_get(void);
uint16_t gui_get_power_limit(void);
uint16_t gui_set_power_limit(uint16_t power);
void set_hs_custom_fx_window(void);
uint8_t get_hs_custom_fx_event(void);
void set_hs_custom_fx_event(uint8_t event);
void ui_fresnel_get_angle_count_set(uint8_t count);
uint8_t ui_fresnel_get_angle_count_get(void);
uint8_t ui_get_work_update_number_flag(void);
void ui_set_work_update_number_flag(uint8_t state);
uint8_t ui_get_batch_update_serial_num(void);
void ui_set_batch_update_serial_num(uint8_t num);
uint8_t ui_get_work_update_number(void);
void ui_set_upgrade_timeout_clear(void);
uint16_t ui_set_upgrade_timeout_get(void);
void ui_set_upgrade_timeout_inc(void);
void ui_set_ele_reset_start_flag(uint8_t state);
uint16_t ui_get_ele_reset_start_flag(void);
uint8_t ui_test_Fresnel_Industrial_mode_get(void);
void ui_test_Fresnel_Industrial_mode_set( uint8_t angle );
void ui_regognition_error_set(bool state);
bool ui_regognition_error_get(void);

#ifdef __cplusplus
}
#endif

#endif
