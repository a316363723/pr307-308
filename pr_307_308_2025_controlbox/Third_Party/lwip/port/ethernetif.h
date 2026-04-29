/**
 *******************************************************************************
 * @file  eth/lwip_http_server/source/ethernetif.h
 * @brief Ethernet interface header file.
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
#ifndef __ETHERNETIF_H__
#define __ETHERNETIF_H__

/* C binding of definitions if building with C++ compiler */
#ifdef __cplusplus
extern "C"
{
#endif

/*******************************************************************************
 * Include files
 ******************************************************************************/
#include "lwip/err.h"
#include "lwip/netif.h"

/*******************************************************************************
 * Global type definitions ('typedef')
 ******************************************************************************/

/*******************************************************************************
 * Global pre-processor symbols/macros ('#define')
 ******************************************************************************/
/* Ethernet PHY interface */
//#define ETH_INTERFACE_RMII

/* Extended PHY Registers */
#define PHY_PSMR                                (0x18U)   /*!< Power Saving Mode Register                         */
#define PHY_IISDR                               (0x1EU)   /*!< Interrupt Indicators and SNR Display Register      */
#define PHY_PSR                                 (0x1FU)   /*!< Page Select Register                               */

#define PHY_P7_RMSR                             (0x10U)   /*!< RMII Mode Setting Register                         */
#define PHY_P7_IWLFR                            (0x13U)   /*!< Interrupt, WOL Enable, and LED Function Registers  */

/* The following parameters will return to default values after a software reset */
#define PHY_EN_PWR_SAVE                         (0x8000U)   /*!< Enable Power Saving Mode               */

#define PHY_FLAG_AUTO_NEGO_ERROR                (0x8000U)   /*!< Auto-Negotiation Error Interrupt Flag  */
#define PHY_FLAG_SPEED_MODE_CHANGE              (0x4000U)   /*!< Speed Mode Change Interrupt Flag       */
#define PHY_FLAG_DUPLEX_MODE_CHANGE             (0x2000U)   /*!< Duplex Mode Change Interrupt Flag      */
#define PHY_FLAG_LINK_STATUS_CHANGE             (0x0800U)   /*!< Link Status Change Interrupt Flag      */

#define PHY_PAGE_ADDR_0                         (0x0000U)   /*!< Page Address 0 (default)               */
#define PHY_PAGE_ADDR_7                         (0x0007U)   /*!< Page Address 7                         */

#define PHY_RMII_CLK_DIR                        (0x1000U)   /*!< TXC direction in RMII Mode             */
#define PHY_RMII_MODE                           (0x0008U)   /*!< RMII Mode or MII Mode                  */
#define PHY_RMII_RXDV_CRSDV                     (0x0004U)   /*!< CRS_DV or RXDV select                  */

#define PHY_INT_LINK_CHANGE                     (0x2000U)   /*!< Link Change Interrupt Mask                         */
#define PHY_INT_DUPLEX_CHANGE                   (0x1000U)   /*!< Duplex Change Interrupt Mask                       */
#define PHY_INT_AUTO_NEGO_ERROR                 (0x0800U)   /*!< Auto-Negotiation Error Interrupt Mask              */
#define PHY_LED_WOL_SELECT                      (0x0400U)   /*!< LED and Wake-On-LAN Function Selection             */
#define PHY_LED_SELECT                          (0x0030U)   /*!< Traditional LED Function Selection.                */
#define PHY_LED_SELECT_00                       (0x0000U)   /*!< LED0: ACT(all)           LED1: LINK(100)           */
#define PHY_LED_SELECT_01                       (0x0010U)   /*!< LED0: LINK(ALL)/ACT(all) LED1: LINK(100)           */
#define PHY_LED_SELECT_10                       (0x0020U)   /*!< LED0: LINK(10)/ACT(all)  LED1: LINK(100)           */
#define PHY_LED_SELECT_11                       (0x0030U)   /*!< LED0: LINK(10)/ACT(10)   LED1: LINK(100)/ACT(100)  */
#define PHY_EN_10M_LED_FUNC                     (0x0001U)   /*!< Enable 10M LPI LED Function                        */

/*******************************************************************************
 * Global variable definitions ('extern')
 ******************************************************************************/

/*******************************************************************************
  Global function prototypes (definition in C source)
 ******************************************************************************/
err_t ethernetif_init(struct netif *netif);
void ethernetif_input(struct netif *netif);

void EthernetIF_CheckLink(struct netif *netif);
void EthernetIF_UpdateLink(struct netif *netif);
void EthernetIF_LinkCallback(struct netif *netif);
void EthernetIF_NotifyLinkChange(struct netif *netif);

#ifdef __cplusplus
}
#endif

#endif /* __ETHERNETIF_H__ */

/*******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/
