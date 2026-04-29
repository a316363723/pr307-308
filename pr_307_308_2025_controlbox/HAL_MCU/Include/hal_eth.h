/**
 * @file hal_eth.h
 * @author Darwin (darwin.xiao@aputure.com)
 * @brief 
 * @version 0.1
 * @date 2022-09-13
 * 
 * @copyright Copyright (c) 2022 爱图仕
 * 
 * @par 修改日志:
 * <table>
 * <tr><th> Date      <th>Version     <th>Author        <th>Description
 * <tr><td>2022-09-13     <td>1.0     <td>Darwin.Xiao   <td>初始创建
 * </table>
 */
#ifndef HAL_ETH_H
#define HAL_ETH_H

#include <stdint.h>
#include <stdbool.h>

#include "lwip/err.h"

#define HAL_USE_INTERFACE_RMII     1    


struct pbuf;
struct netif;

int   hal_eth_init(void);
err_t ethernetif_init(struct netif *netif);
bool hal_eth_get_linked_state(uint8_t port);
void netif_set_mac_addr(struct netif* p_netif, uint8_t mac[]);
uint8_t hal_read_phy_reg(uint16_t phy_reg, uint16_t u16Reg, uint16_t *pu16RegVal);
void hal_netif_get_mac_addr(uint8_t *mac);

#endif
