#ifndef APP_OTA_H
#define APP_OTA_H

#include "user_gatt_proto.h"

#define STATUS_NORMAL_LAUNCH    (0xAE860000)
#define STATUS_OTA_SUCCESS      (0xAE861111) 
#define STATUS_OTA_FAILED       (0xAE862222)    
#define STATUS_RECOVERY_SUCCESS (0xAE864444)
#define STATUS_NEED_OTA         (0xAE86AE86)    

#define STATUS_CN_LANGUAGE      (0xC1111111)
#define STATUS_EN_LANGUAGE      (0xE2222222)

#pragma pack(1)
typedef struct{

	char 	 productname[16];	   	// 设备名字
	uint32_t firmwareversion;	 	// 软件版本
	uint32_t hardwareversion;	 	// 硬件版本
	uint32_t firmwaresize;		 	// 固件大小
	uint32_t checkcrc32;		  	// 固件校验值
	uint16_t  firmwaretype;    		// 升级固件的类型   0-app  1-base
}sfirmwarebin;
#pragma pack()

extern uint32_t LaunchStatus;
extern uint32_t LagStatus;

int app_ble_gatt_ota(gatt_packet_ota_t* ota_body);
//uint32_t get_hardware_version(void);
void app_ota_mcu_system_reset(void);
uint8_t FirmwareCorrect(sfirmwarebin* p_arg);
void app_bmp_update_data_erase(void);
void app_ctr_update_data_erase(void);
uint8_t app_ota_get_upgrade_type(void);

#endif  /* APP_OTA_H */

