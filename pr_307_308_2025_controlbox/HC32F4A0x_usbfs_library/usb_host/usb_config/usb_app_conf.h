/**
 *******************************************************************************
 * @file  usb/usb_host_msc/source/usb_app_conf.h
 * @brief low level driver configuration
 @verbatim
   Change Logs:
   Date             Author          Notes
   2021-05-13       linsq           First version
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
#ifndef __USB_CONF__H__
#define __USB_CONF__H__

/*******************************************************************************
 * Include files
 ******************************************************************************/
/************** choice of FS and HS *******************************************/
/*
USBHS mode(don't define USB_FS_MODE):
1 make the core work in high speed if defien both USB_HS_MODE and USB_HS_EXTERNAL_PHY 
2 make the core work in full speed if just define USB_HS_MODE
USBFS mode(don't define USB_HS_MODE and USB_HS_EXTERNAL_PHY):
make the core work in full speed just define USB_FS_MODE but not define USB_HS_MODE and 
USB_HS_EXTERNAL_PHY
*/

#ifndef USB_HS_MODE
//#define USB_HS_MODE     
#endif

#ifndef USB_FS_MODE
#define USB_FS_MODE     
#endif

#ifndef USB_HS_EXTERNAL_PHY
//#define USB_HS_EXTERNAL_PHY
#endif

/****************** USB FS CONFIGURATION **********************************/
#ifdef USB_FS_MODE
#define RX_FIFO_FS_SIZE         128u
#define TX0_FIFO_FS_SIZE        64u
#define TX1_FIFO_FS_SIZE        128u
#define TX2_FIFO_FS_SIZE        0u
#define TX3_FIFO_FS_SIZE        32u
#define TX4_FIFO_FS_SIZE        32u
#define TX5_FIFO_FS_SIZE        0u
#define TX6_FIFO_FS_SIZE        0u
#define TX7_FIFO_FS_SIZE        0u
#define TX8_FIFO_FS_SIZE        0u
#define TX9_FIFO_FS_SIZE        0u
#define TX10_FIFO_FS_SIZE       0u
#define TX11_FIFO_FS_SIZE       0u
#define TX12_FIFO_FS_SIZE       0u
#define TX13_FIFO_FS_SIZE       0u
#define TX14_FIFO_FS_SIZE       0u
#define TX15_FIFO_FS_SIZE       0u
#endif 
/****************** USB FS CONFIGURATION **********************************/
#ifdef USB_HS_MODE
#define RX_FIFO_HS_SIZE         (512U)
#define TX0_FIFO_HS_SIZE        (64U)
#define TX1_FIFO_HS_SIZE        (64U)
#define TX2_FIFO_HS_SIZE        (0U)
#define TX3_FIFO_HS_SIZE        (64U)
#define TX4_FIFO_HS_SIZE        (0U)
#define TX5_FIFO_HS_SIZE        (0U)
#define TX6_FIFO_HS_SIZE        (0U)
#define TX7_FIFO_HS_SIZE        (0U)
#define TX8_FIFO_HS_SIZE        (0U)
#define TX9_FIFO_HS_SIZE        (0U)
#define TX10_FIFO_HS_SIZE       (0U)
#define TX11_FIFO_HS_SIZE       (0U)
#define TX12_FIFO_HS_SIZE       (0U)
#define TX13_FIFO_HS_SIZE       (0U)
#define TX14_FIFO_HS_SIZE       (0U)
#define TX15_FIFO_HS_SIZE       (0U)

#ifdef USB_HS_EXTERNAL_PHY
#endif

#endif 

/****************** USB OTG MISC CONFIGURATION ********************************/
#define VBUS_SENSING_ENABLED

/****************** USB OTG MODE CONFIGURATION ********************************/
#define USE_HOST_MODE
#define USE_DEVICE_MODE

#ifndef USE_DEVICE_MODE
 #ifndef USE_HOST_MODE
    #error  "USE_DEVICE_MODE or USE_HOST_MODE should be defined"
 #endif
#endif

#define TXH_NP_HS_FIFOSIZ                        128u
#define TXH_P_HS_FIFOSIZ                         256u

#define RX_FIFO_FS_SIZE                           128u
#define TXH_NP_FS_FIFOSIZ                         32u
#define TXH_P_FS_FIFOSIZ                          64u

/****************** C Compilers dependant keywords ****************************/

/* __packed keyword used to decrease the data type alignment to 1-byte */
#if defined (__CC_ARM)         /* ARM Compiler */
  #define __packed    __packed
#elif defined (__ICCARM__)     /* IAR Compiler */
//  #define __packed    __packed
#elif defined   ( __GNUC__ )   /* GNU Compiler */
//  #define __packed    __attribute__ ((__packed__))
#elif defined   (__TASKING__)  /* TASKING Compiler */
  #define __packed    __unaligned
#endif /* __CC_ARM */


#endif //__USB_CONF__H__

/*******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/
