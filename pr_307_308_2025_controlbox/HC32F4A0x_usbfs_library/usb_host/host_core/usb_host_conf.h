/**
 *******************************************************************************
 * @file  usb_host_conf.h
 * @brief Headfile for USB host config 
 @verbatim
   Change Logs:
   Date             Author          Notes
   2021-03-29       Linsq           First version
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
#ifndef __USBH_CONF__H__
#define __USBH_CONF__H__

/*******************************************************************************
 * Include files
 ******************************************************************************/


/*******************************************************************************
 * Global pre-processor symbols/macros ('#define')
 ******************************************************************************/
#define USBH_MAX_NUM_ENDPOINTS                (2u)
#define USBH_MAX_NUM_INTERFACES               (3u)
#define USBH_MSC_MPS_SIZE                     (0x40u)


#endif //__USBH_CONF__H__

/*******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/
