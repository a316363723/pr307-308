/*********************************************************************************
  *Copyright(C), 2018, Aputure, All rights reserver.
  *FileName:  	dev_bb1735.h
  *Author:    	xuesi.huang
  *Version:   	v1.0
  *Date:      	2018/02/26
  *Description: 蓝牙AT指令
				v1.0：2019/02/26
**********************************************************************************/
#ifndef __DEV_BB1735_H
#define __DEV_BB1735_H

//#include "bsp_ioconfig.h"
#include <stdbool.h>
#include <stdint.h>

#define AT_CMD 				"AT"        	/*测试指令，可用于测试串口通信是否正常。*/
#define DEVIVER_CMD 		"DEVIVER"		/*获取设备固件版本*/
#define DEVIUUID_CMD 		"DEVIUUID"		/*获取设备 UUID*/
#define DEVIMAC_CMD 		"DEVIMAC"		/*获取设备蓝牙地址*/
#define SETUUID_CMD 		"SETUUID"		/*设置设备 UUID*/
#define NODEPROV_CMD 		"NODEPROV"		/*获取节点是否 provision*/
#define NODEADDR_CMD 		"NODEADDR"		/*获取节点地址*/
#define NODELIST_CMD 		"NODELIST"		/*获取节点列表*/
#define NODESUBS_CMD 		"NODESUBS"		/*获取节点订阅地址*/
#define NODEPUBS_CMD 		"NODEPUBS"		/*获取节点发布地址*/
#define NODEMODS_CMD 		"NODEMODS"		/*获取节点 models*/
#define NODEAPPIND_CMD 		"NODEAPPIND"	/*节点 appkey index*/
#define NODETIEMOUT_CMD 	"NODETIEMOUT"	/*节点指令超时时间*/
#define NODEONOFF_CMD 		"NODEONOFF"		/*查询当前节点 ONOFF 状态*/
#define DATA_CMD 			"DATA"			/*向目的节点发送数据*/
#define DATAX_CMD 			"DATAX"			/*向目的节点发送 16 进制数据*/
#define NODERSET_CMD 		"NODERSET"		/*清除节点 provision 信息*/
#define GATT_CMD 		    "GATT"		    /*发送GATT数据*/
#define OTA_CMD 		    "OTA"		    /*发送OTA应答数据数据*/

#define DEVIVER_HEAD_LEN	(9)
#define DEVIUUID_HEAD_LEN	(10)

#define BLE_EN_GPIO_Port 	(GPIO_PORT_G)
#define BLE_EN_Pin 			(GPIO_PIN_07)
#define BLE_RST_GPIO_Port 	(GPIO_PORT_I)
#define BLE_RST_Pin 		(GPIO_PIN_04)
// #define BLE_TX_GPIO_Port 	(GPIO_PORT_B)
// #define BLE_TX_Pin 			(GPIO_PIN_01)
// #define BLE_RX_GPIO_Port 	(GPIO_PORT_B)
// #define BLE_RX_Pin 			(GPIO_PIN_00)


typedef enum {
	
	BB1735_DEVIVER = 0,
	BB1735_DEVIUUID_GET,
	BB1735_DEVIUUID_SET,
	BB1735_DEVIMAC,
	BB1735_NODEPROV,
	BB1735_NODEADDR,
	BB1735_NODELIST,
	BB1735_NODESUBS,
	BB1735_NODEPUBS,
	BB1735_NODEMODS,
	BB1735_NODEAPPIND,
	BB1735_NODETIEMOUT,
	BB1735_DATA,
	BB1735_NODERSET,
	BB1735_OTA,
	BB1735_GATT,
	BB1735_CMD_MAX
}BB1735_Cmd_TypeDef;

#define BB1735_MAX_RCV_LEN 256
typedef struct
{
    uint8_t len; 
    uint8_t data[BB1735_MAX_RCV_LEN];
}BB1735_RX_Package;

extern BB1735_RX_Package BleDMARecieve;
extern BB1735_RX_Package BleRXPack;

bool BB1735_Bluetooth_Module_Init(void);
void BB1735_Bluetooth_Module_disable(void);
void BB1735_Get_DeviVer(char *NodeAddr);
void BB1735_RecData_parse(uint8_t *RecData, uint8_t Cnt);
uint8_t BB1735_Send_Data(char *NodeAddr, BB1735_Cmd_TypeDef Cmd, uint8_t *Data, uint8_t DataLen, uint16_t WaitTime);
void BB1735_Node_Rset(void);
void BB1735_Get_DevMac(char *NodeAddr);
void BB1735_Get_DevUUID(char *NodeAddr);
void BB1735_Set_DevUUID(uint8_t *UUID, uint8_t UUID_Len);
void BB1735_Timer_1MS_Handler(void);

void BB1735_Send_String(char *NodeAddr, uint8_t *Str, uint8_t StrLen);
void BB1735_CMD_Write(uint8_t *pData, uint16_t Cnt);
void ble_usart_rxtx_disable(void);
// extern void BB1735_Bluetooth_Periph_Init(void);
extern void BB1735_Bluetooth_Module_enable(void);
extern void BB1735_Bluetooth_Module_Reset(void);

#endif /*__DEV_BB1735_H*/
