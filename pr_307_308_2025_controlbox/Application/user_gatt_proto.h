/*********************************************************************************
  *Copyright(C), 2021, Aputure, All rights reserver.
  *FileName:    user_gatt_proto.h
  *Author:         
  *Version:     v.1.0
  *Date:        2021/09/29
  *Description: APP与MCU通信的蓝牙GATT数据包格式
                v1.0：2021/09/29,Matthew
**********************************************************************************/
#ifndef USER_GATT_PROTO_H
#define USER_GATT_PROTO_H
#include <stdint.h>

#define YOKE_PRODUCT_UUID   "EM005"
#define FRESNEL_PRODUCT_UUID   "EM015"

#define BLE_MOTORIZED_UPGRADE_MAX       (2)

#pragma pack (1)

/*---命令类型---*/
typedef enum{     
    GATT_CMD_CFX = 0,
    GATT_CMD_OTA,
    GATT_CMD_NULL,
    GATT_CMD_MAX_VLAUE = 0xFF,
}gatt_cmd_enum;

typedef struct 
{
    uint8_t check_sum;
    uint8_t length;
    uint8_t sequence;
    uint8_t proto_ver;
    uint8_t cmd;
    uint8_t rw;
}gatt_head_t;

typedef struct 
{
    uint8_t drv_hw_ver;
	uint8_t drv_sw_ver;
	uint8_t ctr_hw_ver;
	uint8_t ctr_sw_ver;
}gatt_ver_t;

typedef struct 
{
    uint8_t uuid[5];
	uint8_t ctr_hw_ver;
	uint8_t ctr_sw_ver;
}gatt_elever_t;

typedef struct 
{
    uint8_t step;
    union 
    {
        uint8_t  device;
        uint32_t length;
        struct 
        {
            uint8_t  length;
            uint32_t sequence;
            uint8_t  data[];    //结构体的柔性数组定义
        }        data_body;
        uint32_t crc32;
        uint8_t  ack;
		struct 
		{
			uint8_t  type;
			uint8_t drv_hw_ver;
			uint8_t drv_sw_ver;
			uint8_t ctr_hw_ver;
			uint8_t ctr_sw_ver;
		}        gatt_ver_t;
		struct 
		{
			uint8_t  type;
			gatt_elever_t ver2[BLE_MOTORIZED_UPGRADE_MAX];
		}        gatt_ver2_t;
    }step_body;
}gatt_packet_ota_t;

typedef union
{
    gatt_packet_ota_t ota_body;
    uint8_t body_buffer[0xff - sizeof(gatt_head_t)];
}gatt_body_t;

typedef struct 
{
    gatt_head_t head;
    gatt_body_t body;
}gatt_packet_t;

#pragma pack ()


#endif

