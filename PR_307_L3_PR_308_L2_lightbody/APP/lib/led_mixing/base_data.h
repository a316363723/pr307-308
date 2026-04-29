#ifndef __BASE_DATA_H
#define __BASE_DATA_H

#include <stdint.h>

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))

#pragma pack (1)
typedef struct{

    uint16_t pwm;
    uint32_t Illuminance;
    uint16_t coord_x;
    uint16_t coord_y;
}Base_Data;

typedef struct{

    uint16_t length;
    uint32_t max_current;   //单个LED最大电流
    const Base_Data *data;
}Base_Data1;
#ifdef PR_307_L3
typedef struct{

    uint8_t num;
    uint32_t max_total_current; //产品总最大电流
    const Base_Data1 *data[5];
    uint8_t ver;
    uint8_t time[16];   //基础数据保存时间，例如2020年3月24日15点9分，写成202003241509
    uint8_t spectrometer[32];   //光谱计名称
    uint16_t voltage_config_size;   //电压配置数据空间大小
    const uint8_t *voltage_config;  //电压配置数据
}Base_Data2;
#endif
#ifdef PR_308_L2
typedef struct{

    uint8_t num;
    uint32_t max_total_current; //产品总最大电流
    const Base_Data1 *data[3];//基础数据需要获取版本号，为了防止获取的版本号异常，需与307做区分
    uint8_t ver;
    uint8_t time[16];   //基础数据保存时间，例如2020年3月24日15点9分，写成202003241509
    uint8_t spectrometer[32];   //光谱计名称
    uint16_t voltage_config_size;   //电压配置数据空间大小
    const uint8_t *voltage_config;  //电压配置数据
}Base_Data2;
#endif
#pragma pack ()

extern const Base_Data2 led_base_data;
extern const Base_Data2 analog_led_base_data;

#endif /*__BASE_DATA_H*/
