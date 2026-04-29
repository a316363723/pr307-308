///*****************************************************************************
// * Copyright (C) 2016, Huada Semiconductor Co.,Ltd All rights reserved.
// *
// * This software is owned and published by:
// * Huada Semiconductor Co.,Ltd ("HDSC").
// *
// * BY DOWNLOADING, INSTALLING OR USING THIS SOFTWARE, YOU AGREE TO BE BOUND
// * BY ALL THE TERMS AND CONDITIONS OF THIS AGREEMENT.
// *
// * This software contains source code for use with HDSC
// * components. This software is licensed by HDSC to be adapted only
// * for use in systems utilizing HDSC components. HDSC shall not be
// * responsible for misuse or illegal use of this software for devices not
// * supported herein. HDSC is providing this software "AS IS" and will
// * not be responsible for issues arising from incorrect user implementation
// * of the software.
// *
// * Disclaimer:
// * HDSC MAKES NO WARRANTY, EXPRESS OR IMPLIED, ARISING BY LAW OR OTHERWISE,
// * REGARDING THE SOFTWARE (INCLUDING ANY ACCOMPANYING WRITTEN MATERIALS),
// * ITS PERFORMANCE OR SUITABILITY FOR YOUR INTENDED USE, INCLUDING,
// * WITHOUT LIMITATION, THE IMPLIED WARRANTY OF MERCHANTABILITY, THE IMPLIED
// * WARRANTY OF FITNESS FOR A PARTICULAR PURPOSE OR USE, AND THE IMPLIED
// * WARRANTY OF NONINFRINGEMENT.
// * HDSC SHALL HAVE NO LIABILITY (WHETHER IN CONTRACT, WARRANTY, TORT,
// * NEGLIGENCE OR OTHERWISE) FOR ANY DAMAGES WHATSOEVER (INCLUDING, WITHOUT
// * LIMITATION, DAMAGES FOR LOSS OF BUSINESS PROFITS, BUSINESS INTERRUPTION,
// * LOSS OF BUSINESS INFORMATION, OR OTHER PECUNIARY LOSS) ARISING FROM USE OR
// * INABILITY TO USE THE SOFTWARE, INCLUDING, WITHOUT LIMITATION, ANY DIRECT,
// * INDIRECT, INCIDENTAL, SPECIAL OR CONSEQUENTIAL DAMAGES OR LOSS OF DATA,
// * SAVINGS OR PROFITS,
// * EVEN IF Disclaimer HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
// * YOU ASSUME ALL RESPONSIBILITIES FOR SELECTION OF THE SOFTWARE TO ACHIEVE YOUR
// * INTENDED RESULTS, AND FOR THE INSTALLATION OF, USE OF, AND RESULTS OBTAINED
// * FROM, THE SOFTWARE.
// *
// * This software may be replicated in part or whole for the licensed use,
// * with the restriction that this Disclaimer and Copyright notice must be
// * included with each copy of this software, whether used in part or whole,
// * at all times.
// */
///******************************************************************************/
///** \file usb_bsp.h
// **
// ** A detailed description is available at
// ** @link Specific api's relative to the used hardware platform @endlink
// **
// **   - 2018-12-26  1.0  wangmin First version for USB demo.
// **
// ******************************************************************************/
//#ifndef __USB_BSP__H__
//#define __USB_BSP__H__

///*******************************************************************************
// * Include files
// ******************************************************************************/
//#include <stdint.h>
//#include "usb_core.h"
//#include "usb_conf.h"
//#include "usbh_usr.h"
//#include "usbh_msc_core.h"

//extern USB_OTG_CORE_HANDLE          USB_OTG_Core;
//extern USBH_HOST                    USB_Host;
// 
//void BSP_Init(void);
//void USB_OTG_BSP_Init (USB_OTG_CORE_HANDLE *pdev);
//void USB_OTG_BSP_uDelay (const uint32_t usec);
//void USB_OTG_BSP_mDelay (const uint32_t msec);
//void USB_OTG_BSP_EnableInterrupt (void);
//#ifdef USE_HOST_MODE
//void USB_OTG_BSP_ConfigVBUS(USB_OTG_CORE_HANDLE *pdev);
//void USB_OTG_BSP_DriveVBUS(USB_OTG_CORE_HANDLE *pdev,uint8_t state);
//#endif

//void usbfs_init(void);
//void dev_usb_core_task(void);

//#endif /* __USB_BSP__H__ */

///*******************************************************************************
// * EOF (not truncated)
// ******************************************************************************/

/**
 *******************************************************************************
 * @file  usb/usb_host_msc/source/usb_bsp.h
 * @brief Head file for usb_bsp.c
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
#ifndef __USB_BSP_H__
#define __USB_BSP_H__

/* C binding of definitions if building with C++ compiler */
#ifdef __cplusplus
extern "C"
{
#endif

/*******************************************************************************
 * Include files
 ******************************************************************************/
#include "hc32f4a0_usb_core_driver.h"
#include "usb_app_conf.h"
#include "hc32_ddl.h"

/**
 * @addtogroup HC32F4A0_DDL_Examples
 * @{
 */

/**
 * @addtogroup USBD_HID_CUSTOM
 * @{
 */

/*******************************************************************************
 * Global type definitions ('typedef')
 ******************************************************************************/

/*******************************************************************************
 * Global pre-processor symbols/macros ('#define')
 ******************************************************************************/


/*******************************************************************************
 * Global variable definitions ('extern')
 ******************************************************************************/

/*******************************************************************************
  Global function prototypes (definition in C source)
 ******************************************************************************/
extern void hd_usb_bsp_init(usb_core_instance *pdev);
extern void hd_usb_udelay(const uint32_t usec);
extern void hd_usb_mdelay(const uint32_t msec);
extern void usb_bsp_nvic(void);
extern void usbfs_init(void);
extern void dev_usb_core_task(void);
#ifdef USE_HOST_MODE
extern void hd_usb_bsp_cfgvbus(usb_core_instance *pdev);
extern void hd_usb_bsp_drivevbus(usb_core_instance *pdev, uint8_t state);
#endif

/**
 * @}
 */

/**
 * @}
 */


#ifdef __cplusplus
}
#endif

#endif /* __USB_BSP__H__ */

/*******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/

