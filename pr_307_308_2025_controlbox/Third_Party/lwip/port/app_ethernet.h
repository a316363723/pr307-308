/**
 *******************************************************************************
 * @file  eth/lwip_http_server/source/app_ethernet.h
 * @brief Header for app_ethernet.c module.
 @verbatim
   Change Logs:
   Date             Author          Notes
   2020-06-12       Yangjp          First version
 @endverbatim
 *******************************************************************************
 * Copyright (C) 2020, Huada Semiconductor Co., Ltd. All rights reserved.
 *
 * This software component is licensed by HDSC under BSD 3-Clause license
 * (the "License"); You may not use this file except in compliance with the
 * License. You may obtain a copy of the License at:
 *                    opensource.org/licenses/BSD-3-Clause
 *
 *******************************************************************************
 */
#ifndef __APP_ETHERNET_H__
#define __APP_ETHERNET_H__

/* C binding of definitions if building with C++ compiler */
#ifdef __cplusplus
extern "C"
{
#endif

/*******************************************************************************
 * Include files
 ******************************************************************************/
#include "lwip/netif.h"

/*******************************************************************************
 * Global type definitions ('typedef')
 ******************************************************************************/

/*******************************************************************************
 * Global pre-processor symbols/macros ('#define')
 ******************************************************************************/
/* Number of milliseconds when to check for link status from PHY */
#ifndef LINK_TIMER_INTERVAL
    #define LINK_TIMER_INTERVAL                 (1000U)
#endif

/* DHCP process states */
#define DHCP_OFF                                (0U)
#define DHCP_START                              (1U)
#define DHCP_WAIT_ADDRESS                       (2U)
#define DHCP_ADDRESS_ASSIGNED                   (3U)
#define DHCP_TIMEOUT                            (4U)
#define DHCP_LINK_DOWN                          (5U)

/* Enable DHCP, if disabled, Use static address */
//#define USE_DHCP

/* Static IP Address */
#define IP_ADDR0                                (192U)
#define IP_ADDR1                                (168U)
#define IP_ADDR2                                (2U)
#define IP_ADDR3                                (77U)

/* Static Netmask */
#define NETMASK_ADDR0                           (255U)
#define NETMASK_ADDR1                           (255U)
#define NETMASK_ADDR2                           (255U)
#define NETMASK_ADDR3                           (0U)

/* Static Gateway Address*/
#define GW_ADDR0                                (192U)
#define GW_ADDR1                                (168U)
#define GW_ADDR2                                (2U)
#define GW_ADDR3                                (1U)

/*******************************************************************************
 * Global variable definitions ('extern')
 ******************************************************************************/

/*******************************************************************************
 * Global function prototypes (definition in C source)
 ******************************************************************************/
void Ethernet_NotifyConnStatus(struct netif *netif);
void LwIP_PeriodicHandle(struct netif *netif);

#ifdef USE_DHCP
void LwIP_DhcpProcess(struct netif *netif);
#endif

#ifdef __cplusplus
}
#endif

#endif /* __APP_ETHERNET_H__ */

/*******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/
