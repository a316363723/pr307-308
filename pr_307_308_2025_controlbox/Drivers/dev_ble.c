#include "dev_ble.h"
#include <string.h>
#include <stdio.h>
#include "hc32_ddl.h"
#include "perf_counter.h"
#include "hal_gpio.h"
#include "hal_uart.h"
#include "app_ble_port.h"

#define AT_CMD              "AT"            /*测试指令，可用于测试串口通信是否正常。*/
#define DEVIVER_CMD         "DEVIVER"       /*获取设备固件版本*/
#define DEVIUUID_CMD        "DEVIUUID"      /*获取设备 UUID*/
#define DEVIMAC_CMD         "DEVIMAC"       /*获取设备蓝牙地址*/
#define SETUUID_CMD         "SETUUID"       /*设置设备 UUID*/
#define NODEPROV_CMD        "NODEPROV"      /*获取节点是否 provision*/
#define NODEADDR_CMD        "NODEADDR"      /*获取节点地址*/
#define NODELIST_CMD        "NODELIST"      /*获取节点列表*/
#define NODESUBS_CMD        "NODESUBS"      /*获取节点订阅地址*/
#define NODEPUBS_CMD        "NODEPUBS"      /*获取节点发布地址*/
#define NODEMODS_CMD        "NODEMODS"      /*获取节点 models*/
#define NODEAPPIND_CMD      "NODEAPPIND"    /*节点 appkey index*/
#define NODETIEMOUT_CMD     "NODETIEMOUT"   /*节点指令超时时间*/
#define NODEONOFF_CMD       "NODEONOFF"     /*查询当前节点 ONOFF 状态*/
#define DATA_CMD            "DATA"          /*向目的节点发送数据*/
#define DATAX_CMD           "DATAX"         /*向目的节点发送 16 进制数据*/
#define NODERSET_CMD        "NODERSET"      /*清除节点 provision 信息*/
#define GATT_CMD            "GATT"          /*发送GATT数据*/
#define OTA_CMD             "OTA"           /*发送OTA应答数据数据*/

#define BLE_RESET_PIN 			HAL_PIN_6
#define BLE_RESET_GPIO_Port	 	HAL_GPIOE
#define BLE_ENABLE_PIN 			HAL_PIN_8
#define BLE_ENABLE_GPIO_Port	HAL_GPIOI

osMessageQueueId_t os_queue_usart_ble_rx;
osMessageQueueId_t os_queue_usart_ble_tx;
osMessageQueueId_t os_queue_ble_cfx;
osSemaphoreId_t os_semaphore_ble_reset;
osSemaphoreId_t os_mutex_tx_data;
osTimerId_t os_timer_iot;
ble_rx_packet ble_rx_data = {0};



static void ble_package_read_cmd(uint8_t *p_pack, uint16_t *pack_len, const char* node_addr, const char *cmd, uint16_t cmd_len)
{
    *pack_len = 0;
    p_pack[0] = 'A';
    p_pack[1] = 'T';
    p_pack[2] = '+';
    *pack_len += 3;
    
    memcpy((char *)(&p_pack[*pack_len]), cmd, cmd_len);
    *pack_len +=  cmd_len;
    if(NULL == node_addr) {
        p_pack[*pack_len] = '?';
        *pack_len += 1;
    }else {
        p_pack[*pack_len] = '@';
        *pack_len += 1;
        memcpy((char *)(&p_pack[*pack_len]), node_addr, 4);
        *pack_len +=  4;
    }
    p_pack[*pack_len] = '\r';
    *pack_len += 1;
}

void ble_package_write_cmd(uint8_t *p_pack, uint16_t *pack_len, const char* node_addr, const char *cmd, uint8_t cmd_len, const uint8_t* Data, uint8_t DataLen)
{
    uint8_t i = 0;
    *pack_len = 0;
    p_pack[0] = 'A';
    p_pack[1] = 'T';
    p_pack[2] = '+';
    *pack_len += 3;
    
    memcpy((char *)(&p_pack[*pack_len]), cmd, cmd_len);
    *pack_len +=  cmd_len;
    if(NULL != node_addr) {
        p_pack[*pack_len] = '@';
        *pack_len += 1;
        memcpy((char *)(&p_pack[*pack_len]), node_addr, 4);
        *pack_len +=  4;
    }
    p_pack[*pack_len] = '=';
    *pack_len += 1;

    for(i = 0; i < DataLen; i++)
    {
        switch(Data[i])
        {
            case 0X0D:
                p_pack[*pack_len] = 0x5C;
                p_pack[*pack_len + 1] = 0X72;
                *pack_len += 2;
                break;
            case 0X0A:
                p_pack[*pack_len] = 0X5C;
                p_pack[*pack_len + 1] = 0X6E;
                *pack_len += 2;                            
                break;
            case 0X5C:
                p_pack[*pack_len] = 0x5C;
                p_pack[*pack_len + 1] = 0x5C;
                *pack_len += 2;
                break;
            default:
                p_pack[*pack_len] = Data[i];
                *pack_len += 1;
                break;
        }
    }
    p_pack[*pack_len] = '\r';
    *pack_len += 1;
}

static void ble_rx_timeout_int_cb(void)
{
	ble_rx_packet ble_rx_pack = {0};
	
	hal_uart_receive_dma_channel_stop(HAL_UART_BLE);
    ble_rx_pack.length = sizeof(ble_rx_data.data) - hal_uart_get_receive_dma_cnt(HAL_UART_BLE);
    memcpy(ble_rx_pack.data, ble_rx_data.data, ble_rx_pack.length);
	osMessageQueuePut(os_queue_usart_ble_rx, &ble_rx_pack, NULL, 0);
	
	hal_uart_receive_dma_start(HAL_UART_BLE, ble_rx_data.data, sizeof(ble_rx_data.data));
	hal_uart_receive_dma_channel_start(HAL_UART_BLE);
	
    
}

static void ble_usart_config(void)
{
	hal_gpio_init(BLE_ENABLE_GPIO_Port, BLE_ENABLE_PIN, HAL_PIN_MODE_OUT_PP);
	hal_gpio_init(BLE_RESET_GPIO_Port, BLE_RESET_PIN, HAL_PIN_MODE_OUT_PP);
	
	hal_gpio_write_pin(BLE_ENABLE_GPIO_Port, BLE_ENABLE_PIN, true);
	hal_uart_init(HAL_UART_BLE,115200);
	hal_uart_receive_dma_start(HAL_UART_BLE, ble_rx_data.data, sizeof(ble_rx_data.data));
	hal_uart_irq_register(HAL_UART_BLE, HAL_UART_IRQ_RX_TIMEOUT, ble_rx_timeout_int_cb);
}

static void ble_chip_reset(void)
{
    hal_gpio_write_pin(BLE_RESET_GPIO_Port, BLE_RESET_PIN, false);  // low
    delay_ms(10);
	hal_gpio_write_pin(BLE_RESET_GPIO_Port, BLE_RESET_PIN, true);   // high
}

void ble_power_on(void)
{
	ble_usart_config();
	ble_chip_reset();
}

void ble_power_off(void)
{
	hal_gpio_init(BLE_ENABLE_GPIO_Port, BLE_ENABLE_PIN, HAL_PIN_MODE_OUT_PP);
	hal_gpio_init(BLE_RESET_GPIO_Port, BLE_RESET_PIN, HAL_PIN_MODE_OUT_PP);
	
	hal_gpio_write_pin(BLE_ENABLE_GPIO_Port, BLE_ENABLE_PIN, false);
	hal_gpio_write_pin(BLE_RESET_GPIO_Port, BLE_RESET_PIN, false);
	hal_ble_uart_set_tx_pin(HAL_UART_BLE);
}


/* 蓝牙上电复位 */
bool ble_power_on_reset(dev_ble_t *p_dev)
{
	bool ret = false;
	ble_rx_packet rx_packet = {0};
	
	if(p_dev == NULL)
		goto out;
	
	ble_power_on();
	
	while(true == p_dev->usart_rx_data(&rx_packet, 400))
	{
		if(strncmp((const char*)&rx_packet.data[0], "+HSL=0000,0000,0000", strlen("+HSL=0000,0000,0000")) == 0)
        {
            ret = true;
            break;
        }
	}
out:	
	return ret;
}



/* 获取蓝牙设备 uuid */
bool ble_get_device_uuid(dev_ble_t *p_dev, const char *node_addr, uint8_t *uuid)
{
	bool ret = false;
	ble_tx_packet tx_packet = {0};
	ble_rx_packet rx_packet = {0};
	
	if(p_dev->usart_rx_data == NULL || p_dev->usart_tx_data == NULL || p_dev == NULL || uuid == NULL)
		goto out;
	
	ble_package_read_cmd(tx_packet.data, &tx_packet.length, node_addr, DEVIUUID_CMD, strlen(DEVIUUID_CMD));
	p_dev->usart_tx_data(tx_packet.data, tx_packet.length);
	
	while(true == p_dev->usart_rx_data(&rx_packet, 50))
	{
		if(strncmp((const char*)&rx_packet.data[0], "+DEVIUUID:", strlen("+DEVIUUID:")) == 0 &&
           strncmp((const char*)&rx_packet.data[rx_packet.length - 4], "\rOK\r", 4) == 0)
        {
            memcpy(uuid, &rx_packet.data[strlen("+DEVIUUID:")], BLE_UUID_LEN);
            ret = true;
            break;
        }
	}
out:
	return ret;
}


/* 设置蓝牙设备 uuid */
bool ble_set_device_uuid(dev_ble_t *p_dev, const uint8_t *uuid, uint8_t len)
{
	bool ret = false;
	ble_tx_packet tx_packet = {0};
	ble_rx_packet rx_packet = {0};
	
	if(p_dev->usart_rx_data == NULL || p_dev->usart_tx_data == NULL || p_dev == NULL || uuid == NULL)
		goto out;
	
	ble_package_write_cmd(tx_packet.data, &tx_packet.length, NULL, SETUUID_CMD, strlen(SETUUID_CMD), uuid, len);
	p_dev->usart_tx_data(tx_packet.data, tx_packet.length);
	
	while(true == p_dev->usart_rx_data(&rx_packet, 50))
	{
		if(strncmp((const char*)&rx_packet.data[0], "\rOK\r", 4) == 0)
        {
            ret = true;
            break;
        }
	}
out:
	return ret;
}

/* 获取蓝牙MAC地址 */
bool ble_get_device_mac(dev_ble_t *p_dev, const char *node_addr, uint8_t* mac)
{
	bool ret = false;
	ble_tx_packet tx_packet = {0};
	ble_rx_packet rx_packet = {0};
	
	if(p_dev->usart_rx_data == NULL || p_dev->usart_tx_data == NULL || p_dev == NULL|| mac == NULL)
		goto out;
	
	ble_package_read_cmd(tx_packet.data, &tx_packet.length, node_addr, DEVIMAC_CMD, strlen(DEVIMAC_CMD));
	p_dev->usart_tx_data(tx_packet.data, tx_packet.length);

	while(true == p_dev->usart_rx_data(&rx_packet, 100))
	{
		if(strncmp((const char*)&rx_packet.data[0], "+DEVIMAC:", strlen("+DEVIMAC:")) == 0 &&
           strncmp((const char*)&rx_packet.data[rx_packet.length - 4], "\rOK\r", 4) == 0)
        {
            memcpy(mac, &rx_packet.data[strlen("+DEVIMAC:")], BLE_MAC_LEN);
            ret = true;
            break;
        }
	}
out:
	return ret;
}

/* 向目的节点发送字符串数据 */
bool ble_send_string(dev_ble_t* p_dev, const char *node_addr, const uint8_t *p_str, uint8_t len)
{
    bool ret = false;
	ble_tx_packet tx_packet = {0};
	ble_rx_packet rx_packet = {0};
	
	if(p_dev->usart_rx_data == NULL || p_dev->usart_tx_data == NULL || p_dev == NULL || node_addr == NULL || p_str == NULL)
		goto out;
	
    ble_package_write_cmd(tx_packet.data, &tx_packet.length, node_addr, DATA_CMD, strlen(DATA_CMD), p_str, len);
    p_dev->usart_tx_data(tx_packet.data, tx_packet.length);
	
    while(true == p_dev->usart_rx_data(&rx_packet, 5))
	{
        if(strncmp((const char*)&rx_packet.data[0], "\rOK\r", 4) == 0)
        {
            ret = true;
            break;
        }
	}
out:
    return ret;
}

/* gatt协议发送数据 */
bool ble_send_gatt_data(dev_ble_t* p_dev, const uint8_t* p_data, uint8_t len)
{
    bool ret = false;
	ble_tx_packet tx_packet = {0};
	ble_rx_packet rx_packet = {0};
	
	if(p_dev->usart_rx_data == NULL || p_dev->usart_tx_data == NULL || p_dev == NULL || p_data == NULL)
		goto out;
	
    ble_package_write_cmd(tx_packet.data, &tx_packet.length, NULL, GATT_CMD, strlen(GATT_CMD), p_data, len);
    p_dev->usart_tx_data(tx_packet.data, tx_packet.length);
	
    while(true == p_dev->usart_rx_data(&rx_packet, 50))
    {
        if(strncmp((const char*)&rx_packet.data[0], "\rOK\r", 4) == 0)
        {
            ret = true;
            break;
        }
    } 
out:
    return ret; 
}

/* 清除节点 provision 信息 */
bool ble_node_reset(dev_ble_t* p_dev)
{
    bool ret = false;
	ble_rx_packet rx_packet = {0};
	
    if(NULL == p_dev || NULL == p_dev->usart_rx_data)
		goto out;
	
    p_dev->usart_tx_data("AT+NODERSET=YES\r", strlen("AT+NODERSET=YES\r"));
	
    while(true == p_dev->usart_rx_data(&rx_packet, 400))
    {
        if(strncmp((const char*)&rx_packet.data[0], "\rOK\r", 4) == 0)
        {
            ret = true;
            break;
        }
    } 
out:
    return ret; 
}

/* 获取蓝牙版本 */
bool ble_get_version(dev_ble_t* p_dev)
{
    bool ret = false;
	ble_rx_packet rx_packet = {0};
	ble_tx_packet tx_packet = {0};
	
    if(NULL == p_dev || NULL == p_dev->usart_rx_data)
		goto out;
	
    ble_package_read_cmd(tx_packet.data, &tx_packet.length, NULL, DEVIVER_CMD, strlen(DEVIVER_CMD));
	p_dev->usart_tx_data(tx_packet.data, tx_packet.length);
	
    while(true == p_dev->usart_rx_data(&rx_packet, 400))   //有收到就证明ok该函数的作用是为了判断蓝牙通讯异常的
    {
		ret = true;
		break;
    }
out:
    return ret; 
}

/* ASCII的16进制转化成代码符号 */
void ble_product_uuid_generate(const char* product_uuid, uint8_t* mac, uint8_t* generate_uuid)
{
//    uint8_t i = 0;

//    if(NULL == product_uuid || NULL == mac || NULL == generate_uuid)
//        return ;

//    for(i = 0; i < 5; i++)
//    {
//        
//        sprintf((char*)&generate_uuid[i * 2],"%02x", product_uuid[i]);
//    }
//    generate_uuid[10] = 0x32;
//    generate_uuid[11] = 0x64;
//    for (i = 0; i < 6; i++)//MAC 后6位
//    {
//        sprintf((char*)&generate_uuid[12 + i * 2], "%02x", 
//            mac[6 + i] >= 'a' &&  mac[6 + i] <= 'f' ? mac[6 + i] - 0x20 : mac[6 + i]);//转大写
//    }
//    if (strncmp((char*)mac, "dc2c26", 6) == 0)
//    {
//        generate_uuid[24] = '0';
//        generate_uuid[25] = '1';
//    }
//    else
//    {
//        generate_uuid[24] = '0';
//        generate_uuid[25] = '2';
//    }
//    memcpy(&generate_uuid[26], "0f0f0f", 6);
    if(NULL == product_uuid || NULL == mac || NULL == generate_uuid)
        return ;
    // 前10字节，5位UUID如05005
    for(uint8_t i = 0; i < 5; i++)
    {
        sprintf((char*)&generate_uuid[i * 2],"%02x", product_uuid[i]);
    }
    memcpy(&generate_uuid[10], "2d", 2);
    for (uint8_t i = 0; i < 6; i++)//MAC 后6位
    {
        sprintf((char*)&generate_uuid[12 + i * 2], "%02x", 
            mac[6 + i] >= 'a' &&  mac[6 + i] <= 'f' ? mac[6 + i] - 0x20 : mac[6 + i]);//转大写
    }
    // 后8字节
    if (strncmp((char*)mac, "dc2c26", 6) == 0)
    {
        memcpy(&generate_uuid[24], "010f0f0f", 8);
    }
    else if (strncmp((char*)mac, "a4c138", 6) == 0)
    {
        memcpy(&generate_uuid[24], "30306670", 8);
    }
    else
    {
        memcpy(&generate_uuid[24], "020f0f0f", 8);
    }
}



void ble_deal_receive_data(dev_ble_t* p_dev)
{
	ble_rx_packet rx_packet = {0};
	
	if(NULL == p_dev || NULL == p_dev->usart_rx_data)
        return;
	
	if(true == p_dev->usart_rx_data(&rx_packet, 5))
	{
		if(NULL != p_dev->usart_user_data)
        {
            p_dev->usart_user_data(&rx_packet);
        }
	}
	
}


void ble_send_data_queue(ble_send_data_type type, uint8_t *p_data, uint8_t length)
{
	app_ble_tx_packet tx_pack;
	
	tx_pack.type = type;
	tx_pack.length = length > sizeof(tx_pack.data) ? sizeof(tx_pack.data) : length;
	memcpy(tx_pack.data, p_data, tx_pack.length);
	
	osMessageQueuePut(os_queue_usart_ble_tx, &tx_pack, NULL, 0);
}

bool ble_usart_rx_data(ble_rx_packet *rx_packet, uint32_t time_out)
{
	bool ret = false;
	uint8_t count = 0;
	
	do{
		if(osOK == osMessageQueueGet(os_queue_usart_ble_rx, rx_packet, NULL, time_out))
		{
			ret = true;
			break;
		}
	
	}while(++count < 10);
	
	return ret;
}

static void ble_tx_complete_int_cb(void)
{
    //osMutexRelease(os_mutex_tx_data);
}

void ble_usart_tx_data(const void *p_data, uint32_t size)
{
	if(osOK == osMutexAcquire(os_mutex_tx_data, 2000))
	{
		hal_uart_transmit_dma_start(HAL_UART_BLE, p_data, size);
		hal_uart_irq_register(HAL_UART_BLE, HAL_UART_IRQ_TX_DMA_FT, ble_tx_complete_int_cb);
		osMutexRelease(os_mutex_tx_data);
	}
}


void start_ble_reset(void)
{
	set_ble_reset(1);
	osSemaphoreAcquire(os_semaphore_ble_reset, 0);
	set_ble_reset_status(BLE_RESET_STATE_START);
}






