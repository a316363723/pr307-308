#ifndef _APP_DATA_CENTER_H
#define _APP_DATA_CENTER_H
#include "local_data.h"
#include "cmsis_os2.h"

/*
预设数量
*/
#define  PRESET_DATA_NUMBER     (10ul)

/*
测试数据替换、掉电存储使能
*/
#define  DATA_CENTRE_TEST_EN    (0ul)

/*
数据恢复出厂设置宏定义
*/
#define LOCAL_LIGHT_DATA        (0x01)
#define LOCAL_CONFIG_DATA       (0x02)
#define LOCAL_PRESET_DATA       (0x03)
#define LOCAL_ALL_DATA          (0x04)

#pragma pack(1)
enum light_mode {
    #define DB_ENUM
    #include "db_light_mode.h"  
};

enum sys_config_type {
    #define DB_ENUM
    #include "db_sys_config.h"  
};

enum sys_info_type {
    #define DB_ENUM
    #include "db_sys_info.h"  
};

struct light_data {
    #define DB_STRUCT
    #include "db_light_mode.h"     
};

typedef struct {
    #define DB_STRUCT
    #include "db_sys_config.h"        
} sys_config_t;

typedef struct 
{
    #define DB_STRUCT
    #include "db_sys_info.h"  
}sys_info_t;

typedef struct preset_data 
{    
    enum light_mode mode;
    union {
        #define DB_STRUCT
        #include "db_light_mode.h"
    } value;
} preset_data_t;
#pragma pack()
/********************************************************************************************
*                                全局变量声明
********************************************************************************************/
extern const osThreadAttr_t g_data_center_thread_attr;
extern osThreadId_t app_data_center_task_id;
/********************************************************************************************
*                                  函数声明
********************************************************************************************/

int data_center_write_config_data(enum sys_config_type type, const void* arg);
int data_center_write_config_data_no_event(enum sys_config_type type, const void* arg);
int data_center_read_config_data(enum sys_config_type type, void* arg);
int data_center_write_sys_info(enum sys_info_type type, const void* arg);
int data_center_read_sys_info(enum sys_info_type type, void* arg);
int data_center_read_light_data(enum light_mode mode, void* arg);
enum light_mode date_center_get_light_mode(void);
int date_center_set_light_mode(enum light_mode mode);
int data_center_write_light_data(enum light_mode mode, const void* arg);
int data_center_write_preset_data(uint8_t preset_num, const void *arg);
int data_center_read_preset_data(uint8_t preset_num,  void *arg);
int data_center_delete_preset_data(int preset_num);
int data_center_read_run_time(void);
int data_center_factory_reset(uint32_t data_select);
void data_center_all_sys_fx_stop(void);
void data_center_set_uuid_write_state(uint8_t state);
uint8_t data_center_get_uuid_write_state(void);
void data_center_run_time_write(uint32_t time);
uint32_t data_center_run_time_read(void);
int data_center_write_brightness(enum light_mode mode, const uint16_t arg);
int data_center_write_light_data_no_event(enum light_mode mode, const void* arg);
void app_data_center_entry(void *argument);
void data_center_iot_handle(void);
void data_center_run_time_clear(void);
void data_center_local_handle(void);

#endif
