/**
 *******************************************************************************
 * @file  usb/usb_host_msc/source/usb_host_user.c
 * @brief user application layer.
 @verbatim
   Change Logs:
   Date             Author          Notes
   2022-03-31       CDT             First version
   2022-10-31       CDT             Support LFN print
   2022-10-31       CDT             Fix bug for device disconnect detect
 @endverbatim
 *******************************************************************************
 * Copyright (C) 2022-2023, Xiaohua Semiconductor Co., Ltd. All rights reserved.
 *
 * This software component is licensed by XHSC under BSD 3-Clause license
 * (the "License"); You may not use this file except in compliance with the
 * License. You may obtain a copy of the License at:
 *                    opensource.org/licenses/BSD-3-Clause
 *
 *******************************************************************************
 */

/*******************************************************************************
 * Include files
 ******************************************************************************/
#include <string.h>
#include <stdio.h>
#include "usb_host_user.h"
#include "ff.h"       /* FATFS */
#include "usb_host_msc_class.h"
#include "usb_host_msc_scsi.h"
#include "usb_host_msc_bot.h"
#include "usb_host_driver.h"
#include "hc32_ddl.h"
#include "usb_host_hid_class.h"
#include "app_usb.h"
//#include "ev_hc32f4a0_lqfp176_bsp.h"

/**
 * @addtogroup HC32F4A0_DDL_Applications
 * @{
 */
extern USBH_HOST                    usb_app_host;
/**
 * @addtogroup USB_Host_Msc
 * @{
 */

/*******************************************************************************
 * Local type definitions ('typedef')
 ******************************************************************************/

/*******************************************************************************
 * Local pre-processor symbols/macros ('#define')
 ******************************************************************************/

/*******************************************************************************
 * Global variable definitions (declared in header file with 'extern')
 ******************************************************************************/
/* USBH_USR_Private_Macros */
extern usb_core_instance usb_app_instance;

/*  Points to the DEVICE_PROP structure of current device */
/*  The purpose of this register is to speed up the execution */
usb_host_user_callback_func USR_cb = {
    &host_user_init,
    &host_user_denint,
    &host_user_devattached,
    &host_user_devreset,
    &host_user_devdisconn,
    &host_user_overcurrent,
    &host_user_devspddetected,
    &host_user_devdescavailable,
    &host_user_devaddrdistributed,
    &host_user_cfgdescavailable,
    &host_user_mfcstring,
    &host_user_productstring,
    &host_user_serialnum,
    &host_user_enumcompl,
    &host_user_userinput,
    &host_user_msc_app,
    &host_user_devunsupported,
    &host_user_unrecoverederror

};

/*******************************************************************************
 * Local function prototypes ('static')
 ******************************************************************************/
/*******************************************************************************
 * Local variable definitions ('static')
 ******************************************************************************/
uint8_t USB_HOST_USER_AppState = USH_USR_FS_INIT;

/* USBH_USR_Private_Constants */
const static char *MSG_DEV_ATTACHED     = "> Device Attached\r\n";
const static char *MSG_DEV_DISCONNECTED = "> Device Disconnected\r\n";
const static char *MSG_DEV_ENUMERATED   = "> Enumeration completed\r\n";
const static char *MSG_DEV_FULLSPEED    = "> Full speed device detected\r\n";
const static char *MSG_DEV_LOWSPEED     = "> Low speed device detected\r\n";
const static char *MSG_DEV_ERROR        = "> Device fault\r\n";

const static char *MSG_MSC_CLASS        = "> Mass storage device connected\r\n";
const static char *MSG_HID_CLASS        = "> HID device connected\r\n";
const static char *MSG_UNREC_ERROR      = "> UNRECOVERED ERROR STATE\r\n";
#ifdef USB_MSC_FAT_VALID
const static char *MSG_ROOT_CONT        = "> Exploring disk flash ...\r\n";
const static char *MSG_WR_PROTECT       = "> The disk is write protected\r\n";
#endif

/*******************************************************************************
 * Function implementation - global ('extern') and local ('static')
 ******************************************************************************/
/**
 * @brief  Displays the message on terminal for host lib initialization
 * @param  None
 * @retval None
 */
void host_user_init(void)
{
    static uint8_t startup = 0U;

    if (startup == 0U) {
        startup = 1U;
#if (DDL_PRINT_ENABLE == DDL_ON)
        DDL_Printf("> USB Host library started.\r\n");
        DDL_Printf("     USB Host Library v2.1.0\r\n");
#endif
    }
}

/**
 * @brief  Displays the message on terminal via DDL_Printf
 * @param  None
 * @retval None
 */
void host_user_devattached(void)
{
#if (DDL_PRINT_ENABLE == DDL_ON)
    DDL_Printf(MSG_DEV_ATTACHED);
#endif
}

/**
 * @brief  host_user_unrecoverederror
 * @param  None
 * @retval None
 */
void host_user_unrecoverederror(void)
{
#if (DDL_PRINT_ENABLE == DDL_ON)
    DDL_Printf(MSG_UNREC_ERROR);
#endif
}

/**
 * @brief  Device disconnect event
 * @param  None
 * @retval None
 */
void host_user_devdisconn(void)
{
#if (DDL_PRINT_ENABLE == DDL_ON)
    DDL_Printf(MSG_DEV_DISCONNECTED);
#endif
}

/**
 * @brief  USBH_USR_ResetUSBDevice
 * @param  None
 * @retval None
 */
void host_user_devreset(void)
{
    /* callback for USB-Reset */
}

/**
 * @brief  host_user_devspddetected
 * @param  [in] DeviceSpeed      USB speed
 * @retval None
 */
void host_user_devspddetected(uint8_t DeviceSpeed)
{
#if (DDL_PRINT_ENABLE == DDL_ON)
    if (DeviceSpeed == PRTSPD_FULL_SPEED) {
        DDL_Printf(MSG_DEV_FULLSPEED);
    } else if (DeviceSpeed == PRTSPD_LOW_SPEED) {
        DDL_Printf(MSG_DEV_LOWSPEED);
    } else {
        DDL_Printf(MSG_DEV_ERROR);
    }
#endif
}

/**
 * @brief  host_user_devdescavailable
 * @param  [in] DeviceDesc       device descriptor
 * @retval None
 */
void host_user_devdescavailable(void *DeviceDesc)
{
#if (DDL_PRINT_ENABLE == DDL_ON)
    usb_host_devdesc_typedef *hs;
    hs = DeviceDesc;
    DDL_Printf("VID : %04lXh\r\n", (uint32_t)(*hs).idVendor);
    DDL_Printf("PID : %04lXh\r\n", (uint32_t)(*hs).idProduct);
#endif
}

/**
 * @brief  host_user_devaddrdistributed
 * @param  None
 * @retval None
 */
void host_user_devaddrdistributed(void)
{
}

/**
 * @brief  host_user_cfgdescavailable
 * @param  [in] cfgDesc          Configuration desctriptor
 * @param  [in] itfDesc          Interface desctriptor
 * @param  [in] epDesc           Endpoint desctriptor
 * @retval None
 */
void host_user_cfgdescavailable(usb_host_cfgdesc_typedef *cfgDesc,
                                usb_host_itfdesc_typedef *itfDesc,
                                USB_HOST_EPDesc_TypeDef *epDesc)
{
    usb_host_itfdesc_typedef *id;

    id = itfDesc;
    if ((*id).bInterfaceClass  == 0x08U) {
        usb_app_host.class_callbk = &USBH_MSC_cb;   //chen
    } else if ((*id).bInterfaceClass  == 0x03U) {
        usb_app_host.class_callbk = &USBH_HID_cb;
    } else {
        ;
    }
#if (DDL_PRINT_ENABLE == DDL_ON)
    usb_host_itfdesc_typedef *id;

    id = itfDesc;
    if ((*id).bInterfaceClass  == 0x08U) {
        DDL_Printf(MSG_MSC_CLASS);
    } else if ((*id).bInterfaceClass  == 0x03U) {
        DDL_Printf(MSG_HID_CLASS);
    } else {
        ;
    }
#endif
}

/**
 * @brief  Displays the message on terminal for Manufacturer String
 * @param  [in] ManufacturerString
 * @retval None
 */
void host_user_mfcstring(void *ManufacturerString)
{
#if (DDL_PRINT_ENABLE == DDL_ON)
    DDL_Printf("Manufacturer : %s\r\n", (char *)ManufacturerString);
#endif
}

/**
 * @brief  Displays the message on terminal for product String
 * @param  [in] ProductString
 * @retval None
 */
void host_user_productstring(void *ProductString)
{
#if (DDL_PRINT_ENABLE == DDL_ON)
    DDL_Printf("Product : %s\r\n", (char *)ProductString);
#endif
}

/**
 * @brief  Displays the message on terminal for SerialNum_String
 * @param  [in] SerialNumString
 * @retval None
 */
void host_user_serialnum(void *SerialNumString)
{
#if (DDL_PRINT_ENABLE == DDL_ON)
    DDL_Printf("Serial Number : %s\r\n", (char *)SerialNumString);
#endif
}

/**
 * @brief  User response request is displayed to ask application jump to class
 * @param  None
 * @retval None
 */
void host_user_enumcompl(void)
{
#if (DDL_PRINT_ENABLE == DDL_ON)
    /* Enumeration complete */
    DDL_Printf(MSG_DEV_ENUMERATED);
    DDL_Printf("To see the root content of the disk : \r\n");
    DDL_Printf("Press USER KEY...\r\n");
#endif
}

/**
 * @brief  Device is not supported
 * @param  None
 * @retval None
 */
void host_user_devunsupported(void)
{
#if (DDL_PRINT_ENABLE == DDL_ON)
    DDL_Printf("> Device not supported.\r\n");
#endif
}

/**
 * @brief  User Action for application state entry
 * @param  None
 * @retval HOST_USER_STATUS     User response for key button
 */
HOST_USER_STATUS host_user_userinput(void)
{
    HOST_USER_STATUS usbh_usr_status;

    usbh_usr_status = USER_NONE_RESP;
    /*Key B3 is in polling mode to detect user action */
//    if (GET_BUTTON_KEY() == Pin_Reset) {
//        usbh_usr_status = USER_HAVE_RESP;
//    }

    return usbh_usr_status;
}

/**
 * @brief  Over Current Detected on VBUS
 * @param  None
 * @retval None
 */
void host_user_overcurrent(void)
{
#if (DDL_PRINT_ENABLE == DDL_ON)
    DDL_Printf("Overcurrent detected.\r\n");
#endif
}

/**
 * @brief  Demo application for mass storage
 * @param  None
 * @retval None
 */
int host_user_msc_app(void)
{
	if(app_usb_fatfile_state_get())
        app_usb_update_handle();
	if(app_usb_batch_upgrade_state_get())
		app_usb_batch_upgrade_handle();
}

/**
 * @brief  Deint User state and associated variables
 * @param  None
 * @retval None
 */
void host_user_denint(void)
{
    USB_HOST_USER_AppState = USH_USR_FS_INIT;
}

/**
 * @}
 */

/**
 * @}
 */

/*******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/
