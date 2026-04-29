/**
 * @file app_eth.h
 * @author Darwin (darwin.xiao@aputure.com)
 * @brief 
 * @version 0.1
 * @date 2022-09-13
 * 
 * @copyright Copyright (c) 2022 爱图仕
 * 
 * @par 修改日志:
 * <table>
 * <tr><th> Date      <th>Version <th>Author  <th>Description
 * <tr><td>2022-09-13     <td>1.0     <td>Darwin.Xiao   <td>初始创建
 * </table>
 */
#ifndef APP_ETH_H
#define APP_ETH_H

#include "cmsis_os.h"
#include "project_def.h"

extern const osThreadAttr_t g_eth_thread_attr;
extern osThreadId_t app_eth_task_id;

void app_eth_entry(void* arg);

uint32_t netif_get_netmask(void);
uint32_t netif_get_ip_addr(void);
uint32_t netif_get_brdcast_addr(void);
void netif_get_mac_addr(uint8_t* hwaddr);
uint32_t netif_get_dhcp_netmask(void);
uint32_t netif_get_dhcp_ip_addr(void);
uint32_t netif_get_dhcp_gateway_addr(void);
uint8_t app_eth_get_lwip_state(void);

#endif
