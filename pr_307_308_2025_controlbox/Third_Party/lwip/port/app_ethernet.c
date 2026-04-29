/**
 *******************************************************************************
 * @file  eth/lwip_http_server/source/app_ethernet.c
 * @brief Ethernet DHCP and Connect status module.
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

/*******************************************************************************
 * Include files
 ******************************************************************************/
#include "hc32_ddl.h"
#include "lwip/dhcp.h"
#include "lwip/sys.h"
#include "app_ethernet.h"
#include "ethernetif.h"
#include "FreeRTOS.h"
#include "event_groups.h"

/*******************************************************************************
 * Local type definitions ('typedef')
 ******************************************************************************/

/*******************************************************************************
 * Local pre-processor symbols/macros ('#define')
 ******************************************************************************/
#ifdef USE_DHCP
    #define DHCP_MAX_TRIES                      (4U)
#endif

/*******************************************************************************
 * Global variable definitions (declared in header file with 'extern')
 ******************************************************************************/

/*******************************************************************************
 * Local function prototypes ('static')
 ******************************************************************************/

/*******************************************************************************
 * Local variable definitions ('static')
 ******************************************************************************/
static uint32_t u32LinkTimer = 0UL;

#ifdef USE_DHCP
static uint32_t u32DHCPfineTimer = 0UL;
__IO uint8_t u8DHCPState = DHCP_OFF;
#endif

/*******************************************************************************
 * Function implementation - global ('extern') and local ('static')
 ******************************************************************************/
/**
 * @brief  Notify network connect status of Ethernet.
 * @param  [in] netif                   Pointer to a struct netif structure
 * @retval None
 */
void Ethernet_NotifyConnStatus(struct netif *netif)
{
    if (netif_is_up(netif))
    {
#ifdef USE_DHCP
        u8DHCPState = DHCP_START;
#else
        /* Turn On LED BULE to indicate ETH and LwIP init success*/
        //BSP_LED_On(LED_BLUE);
#endif /* USE_DHCP */
    }
    else
    {
#ifdef USE_DHCP
        u8DHCPState = DHCP_LINK_DOWN;
#endif  /* USE_DHCP */
        /* Turn On LED RED to indicate ETH and LwIP init error */
        //BSP_LED_On(LED_RED);
    }
}

/**
 * @brief  Notify link status change.
 * @param  [in] netif                   Pointer to a struct netif structure
 * @retval None
 */
void EthernetIF_NotifyLinkChange(struct netif *netif)
{
#ifndef USE_DHCP
    ip_addr_t ipaddr;
    ip_addr_t netmask;
    ip_addr_t gw;
#endif

    if (netif_is_link_up(netif))
    {
        //BSP_LED_Off(LED_RED);
        //BSP_LED_On(LED_BLUE);
#ifdef USE_DHCP
        /* Update DHCP state machine */
        u8DHCPState = DHCP_START;
#else
        IP_ADDR4(&ipaddr, IP_ADDR0, IP_ADDR1, IP_ADDR2, IP_ADDR3);
        IP_ADDR4(&netmask, NETMASK_ADDR0, NETMASK_ADDR1, NETMASK_ADDR2, NETMASK_ADDR3);
        IP_ADDR4(&gw, GW_ADDR0, GW_ADDR1, GW_ADDR2, GW_ADDR3);
        netif_set_addr(netif, &ipaddr, &netmask, &gw);
#endif /* USE_DHCP */
        /* When the netif is fully configured this function must be called. */
        netif_set_up(netif);
    }
    else
    {
#ifdef USE_DHCP
        u8DHCPState = DHCP_LINK_DOWN;
#endif /* USE_DHCP */
        /*  When the netif link is down this function must be called. */
        netif_set_down(netif);

        //BSP_LED_Off(LED_BLUE);
        //BSP_LED_On(LED_RED);
    }
}

/**
 * @brief  LwIP periodic handle
 * @param  [in] netif                   Pointer to a struct netif structure
 * @retval None
 */
void LwIP_PeriodicHandle(struct netif *netif)
{
    uint32_t curTick;

    curTick = sys_now();
#ifdef USE_DHCP
    /* Fine DHCP periodic process every 500ms */
    if ((curTick - u32DHCPfineTimer) >= 500)
    {
        u32DHCPfineTimer = curTick;
        LwIP_DhcpProcess(netif);
    }
#endif /* USE_DHCP */

#ifndef ETH_INTERFACE_RMII
    /* Check link status periodically */
    if ((curTick - u32LinkTimer) >= LINK_TIMER_INTERVAL)
    {
        u32LinkTimer = curTick;
        EthernetIF_CheckLink(netif);
    }
#endif /* ETH_INTERFACE_RMII */
}

#ifdef USE_DHCP
/**
 * @brief  Lwip DHCP Process
 * @param  [in] netif                   Pointer to a struct netif structure
 * @retval None
 */
void LwIP_DhcpProcess(struct netif *netif)
{
    ip_addr_t ipaddr;
    ip_addr_t netmask;
    ip_addr_t gw;
    struct dhcp *dhcp;

    switch (u8DHCPState)
    {
        case DHCP_START:
            ip_addr_set_zero_ip4(&netif->ip_addr);
            ip_addr_set_zero_ip4(&netif->netmask);
            ip_addr_set_zero_ip4(&netif->gw);
            u8DHCPState = DHCP_WAIT_ADDRESS;
            (void)dhcp_start(netif);
            break;
        case DHCP_WAIT_ADDRESS:
            if (0U != dhcp_supplied_address(netif))
            {
                u8DHCPState = DHCP_ADDRESS_ASSIGNED;
                extern EventGroupHandle_t network_events;
                xEventGroupSetBits(network_events, 1 << 0);
                //BSP_LED_On(LED_BLUE);
            }
            else
            {
                dhcp = (struct dhcp*)netif_get_client_data(netif, LWIP_NETIF_CLIENT_DATA_INDEX_DHCP);
                /* DHCP timeout */
                if (dhcp->tries > DHCP_MAX_TRIES)
                {
                    u8DHCPState = DHCP_TIMEOUT;
                    dhcp_stop(netif);
                    /* Static address used */
                    IP_ADDR4(&ipaddr, IP_ADDR0, IP_ADDR1, IP_ADDR2, IP_ADDR3);
                    IP_ADDR4(&netmask, NETMASK_ADDR0, NETMASK_ADDR1, NETMASK_ADDR2, NETMASK_ADDR3);
                    IP_ADDR4(&gw, GW_ADDR0, GW_ADDR1, GW_ADDR2, GW_ADDR3);
                    netif_set_addr(netif, &ipaddr, &netmask, &gw);
                    //BSP_LED_On(LED_BLUE);
                }
            }
            break;
        case DHCP_LINK_DOWN:
            /* Stop DHCP */
            dhcp_stop(netif);
            u8DHCPState = DHCP_OFF;
            break;
        default:
            break;
    }
}
#endif

/******************************************************************************
 * EOF (not truncated)
 *****************************************************************************/
