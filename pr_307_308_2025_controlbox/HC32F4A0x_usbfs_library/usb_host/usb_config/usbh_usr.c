/******************************************************************************
 * Copyright (C) 2016, Huada Semiconductor Co.,Ltd. All rights reserved.
 *
 * This software is owned and published by:
 * Huada Semiconductor Co.,Ltd ("HDSC").
 *
 * BY DOWNLOADING, INSTALLING OR USING THIS SOFTWARE, YOU AGREE TO BE BOUND
 * BY ALL THE TERMS AND CONDITIONS OF THIS AGREEMENT.
 *
 * This software contains source code for use with HDSC
 * components. This software is licensed by HDSC to be adapted only
 * for use in systems utilizing HDSC components. HDSC shall not be
 * responsible for misuse or illegal use of this software for devices not
 * supported herein. HDSC is providing this software "AS IS" and will
 * not be responsible for issues arising from incorrect user implementation
 * of the software.
 *
 * Disclaimer:
 * HDSC MAKES NO WARRANTY, EXPRESS OR IMPLIED, ARISING BY LAW OR OTHERWISE,
 * REGARDING THE SOFTWARE (INCLUDING ANY ACCOMPANYING WRITTEN MATERIALS),
 * ITS PERFORMANCE OR SUITABILITY FOR YOUR INTENDED USE, INCLUDING,
 * WITHOUT LIMITATION, THE IMPLIED WARRANTY OF MERCHANTABILITY, THE IMPLIED
 * WARRANTY OF FITNESS FOR A PARTICULAR PURPOSE OR USE, AND THE IMPLIED
 * WARRANTY OF NONINFRINGEMENT.
 * HDSC SHALL HAVE NO LIABILITY (WHETHER IN CONTRACT, WARRANTY, TORT,
 * NEGLIGENCE OR OTHERWISE) FOR ANY DAMAGES WHATSOEVER (INCLUDING, WITHOUT
 * LIMITATION, DAMAGES FOR LOSS OF BUSINESS PROFITS, BUSINESS INTERRUPTION,
 * LOSS OF BUSINESS INFORMATION, OR OTHER PECUNIARY LOSS) ARISING FROM USE OR
 * INABILITY TO USE THE SOFTWARE, INCLUDING, WITHOUT LIMITATION, ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL OR CONSEQUENTIAL DAMAGES OR LOSS OF DATA,
 * SAVINGS OR PROFITS,
 * EVEN IF Disclaimer HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
 * YOU ASSUME ALL RESPONSIBILITIES FOR SELECTION OF THE SOFTWARE TO ACHIEVE YOUR
 * INTENDED RESULTS, AND FOR THE INSTALLATION OF, USE OF, AND RESULTS OBTAINED
 * FROM, THE SOFTWARE.
 *
 * This software may be replicated in part or whole for the licensed use,
 * with the restriction that this Disclaimer and Copyright notice must be
 * included with each copy of this software, whether used in part or whole,
 * at all times.
 */
/******************************************************************************/
/** \file usbh_usr.c
 **
 ** A detailed description is available at
 ** @link
        This file includes the user application layer.
    @endlink
 **
 **   - 2018-05-21  1.0  gouwei First version for USB demo.
 **
 ******************************************************************************/

/*******************************************************************************
 * Include files
 ******************************************************************************/
#include <string.h>
#include <stdio.h>
#include "usbh_usr.h"
#include "ff.h"       /* FATFS */
#include "usbh_msc_core.h"
#include "usbh_msc_scsi.h"
#include "usbh_msc_bot.h"
#include "hc32_ddl.h"
#include "stdbool.h"
#include "app_usb.h"
#include "usbh_hid_core.h"

/*******************************************************************************
 * Local pre-processor symbols/macros ('#define')
 ******************************************************************************/
/* USBH_USR_Private_Macros */
extern USB_OTG_CORE_HANDLE          USB_OTG_Core;


/*******************************************************************************
 * Local variable definitions ('static')
 ******************************************************************************/
uint8_t USBH_USR_ApplicationState = USH_USR_FS_INIT;
//uint8_t Usb_Insert_Flag = 0;



/*  Points to the DEVICE_PROP structure of current device */
/*  The purpose of this register is to speed up the execution */

USBH_Usr_cb_TypeDef USR_cb =
{
    &USBH_USR_Init,
    &USBH_USR_DeInit,
    &USBH_USR_DeviceAttached,
    &USBH_USR_ResetDevice,
    &USBH_USR_DeviceDisconnected,
    &USBH_USR_OverCurrentDetected,
    &USBH_USR_DeviceSpeedDetected,
    &USBH_USR_Device_DescAvailable,
    &USBH_USR_DeviceAddressAssigned,
    &USBH_USR_Configuration_DescAvailable,
    &USBH_USR_Manufacturer_String,
    &USBH_USR_Product_String,
    &USBH_USR_SerialNum_String,
    &USBH_USR_EnumerationDone,
    &USBH_USR_UserInput,
    &USBH_USR_MSC_Application,
    &USBH_USR_DeviceNotSupported,
    &USBH_USR_UnrecoveredError

};

/* USBH_USR_Private_Constants */
const char* MSG_HOST_INIT        = "> Host Library Initialized\n";
const char* MSG_DEV_ATTACHED     = "> Device Attached \n";
const char* MSG_DEV_DISCONNECTED = "> Device Disconnected\n";
const char* MSG_DEV_ENUMERATED   = "> Enumeration completed \n";
const char* MSG_DEV_HIGHSPEED    = "> High speed device detected\n";
const char* MSG_DEV_FULLSPEED    = "> Full speed device detected\n";
const char* MSG_DEV_LOWSPEED     = "> Low speed device detected\n";
const char* MSG_DEV_ERROR        = "> Device fault \n";

const char* MSG_MSC_CLASS        = "> Mass storage device connected\n";
const char* MSG_HID_CLASS        = "> HID device connected\n";
const char* MSG_DISK_SIZE        = "> Size of the disk in MBytes: \n";
const char* MSG_LUN              = "> LUN Available in the device:\n";
const char* MSG_ROOT_CONT        = "> Exploring disk flash ...\n";
const char* MSG_WR_PROTECT       = "> The disk is write protected\n";
const char* MSG_UNREC_ERROR      = "> UNRECOVERED ERROR STATE\n";

/*******************************************************************************
 * Local variable definitions ('static')
 ******************************************************************************/

/**
 *******************************************************************************
 ** \brief  Displays the message on LCD for host lib initialization
 ** \param  None
 ** \retval None
 ******************************************************************************/
void USBH_USR_Init(void)
{
    static uint8_t startup = 0u;

    if(startup == 0u )
    {
        startup = 1u;

#ifdef USE_USB_OTG_HS
        Debug(" USB OTG HS MSC Host\n");
#else
        Debug(" USB OTG FS MSC Host\n");
#endif

    }
}

/**
 *******************************************************************************
 ** \brief  Displays the message on terminal via printf
 ** \param  None
 ** \retval None
 ******************************************************************************/
void USBH_USR_DeviceAttached(void)
{
    Debug((void *)MSG_DEV_ATTACHED);
//    OLED_ShowString(12, 26, 127, 35, 1, FONT_1216_CLASS_SEL, 1, 3, "DEVICE ATTACHED", NULL);
//    OLED_RefreshGram();    
}

/**
 *******************************************************************************
 ** \brief  USBH_USR_UnrecoveredError
 ** \param  None
 ** \retval None
 ******************************************************************************/
void USBH_USR_UnrecoveredError (void)
{

  /* Set default screen color*/
    Debug((void *)MSG_UNREC_ERROR);
}

/**
 *******************************************************************************
 ** \brief  Device disconnect event
 ** \param  None
 ** \retval None
 ******************************************************************************/
void USBH_USR_DeviceDisconnected (void)
{
//    usb_msc_disconnnected_handle();
    USBH_USR_ApplicationState = USH_USR_FS_INIT;
    Debug((void *)MSG_DEV_DISCONNECTED);
}

/**
 *******************************************************************************
 ** \brief  USBH_USR_ResetUSBDevice
 ** \param  None
 ** \retval None
 ******************************************************************************/
void USBH_USR_ResetDevice(void)
{
    /* callback for USB-Reset */
}

/**
 *******************************************************************************
 ** \brief  USBH_USR_DeviceSpeedDetected
 ** \param  DeviceSpeed : USB speed
 ** \retval None
 ******************************************************************************/
void USBH_USR_DeviceSpeedDetected(uint8_t DeviceSpeed)
{
    if(DeviceSpeed == HPRT0_PRTSPD_HIGH_SPEED)
    {
        Debug((void *)MSG_DEV_HIGHSPEED);
    }
    else if(DeviceSpeed == HPRT0_PRTSPD_FULL_SPEED)
    {
        Debug((void *)MSG_DEV_FULLSPEED);
    }
    else if(DeviceSpeed == HPRT0_PRTSPD_LOW_SPEED)
    {
        Debug((void *)MSG_DEV_LOWSPEED);
    }
    else
    {
        Debug((void *)MSG_DEV_ERROR);
    }
}

/**
 *******************************************************************************
 ** \brief  USBH_USR_Device_DescAvailable
 ** \param  DeviceDesc : device descriptor
 ** \retval None
 ******************************************************************************/
void USBH_USR_Device_DescAvailable(void *DeviceDesc)
{
    USBH_DevDesc_TypeDef *hs;
    hs = DeviceDesc;

    Debug("VID : %04lXh\n" , (uint32_t)(*hs).idVendor);
    Debug("PID : %04lXh\n" , (uint32_t)(*hs).idProduct);
}

/**
 *******************************************************************************
 ** \brief  USBH_USR_DeviceAddressAssigned
 ** \param  None
 ** \retval None
 ******************************************************************************/
void USBH_USR_DeviceAddressAssigned(void)
{

}

/**
 *******************************************************************************
 ** \brief  USBH_USR_Configuration_DescAvailable
 ** \param  cfgDesc : Configuration desctriptor
 ** \param  itfDesc : Interface desctriptor
 ** \param  epDesc : Endpoint desctriptor
 ** \retval None
 ******************************************************************************/
void USBH_USR_Configuration_DescAvailable(USBH_CfgDesc_TypeDef * cfgDesc,
                                          USBH_InterfaceDesc_TypeDef *itfDesc,
                                          USBH_EpDesc_TypeDef *epDesc)
{
    USBH_InterfaceDesc_TypeDef *id;

    id = itfDesc;

    if((*id).bInterfaceClass  == 0x08u)
    {
		USB_Host.class_cb = &USBH_MSC_cb;   //chen
		
        Debug((void *)MSG_MSC_CLASS);
    }
    else if((*id).bInterfaceClass  == 0x03u)
    {
		USB_Host.class_cb = &USBH_HID_cb;
        Debug((void *)MSG_HID_CLASS);
    }
    else
    {
        //
    }
}

/**
 *******************************************************************************
 ** \brief  Displays the message on LCD for Manufacturer String
 ** \param  ManufacturerString
 ** \retval None
 ******************************************************************************/
void USBH_USR_Manufacturer_String(void *ManufacturerString)
{
    Debug("Manufacturer : %s\n", (char *)ManufacturerString);
}

/**
 *******************************************************************************
 ** \brief  Displays the message on LCD for product String
 ** \param  ProductString
 ** \retval None
 ******************************************************************************/
void USBH_USR_Product_String(void *ProductString)
{
    Debug("Product : %s\n", (char *)ProductString);
}

/**
 *******************************************************************************
 ** \brief  Displays the message on LCD for SerialNum_String
 ** \param  SerialNumString
 ** \retval None
 ******************************************************************************/
void USBH_USR_SerialNum_String(void *SerialNumString)
{
    Debug( "Serial Number : %s\n", (char *)SerialNumString);
}

/**
 *******************************************************************************
 ** \brief  User response request is displayed to ask application jump to class
 ** \param  None
 ** \retval None
 ******************************************************************************/
void USBH_USR_EnumerationDone(void)
{

    /* Enumeration complete */
    Debug((void *)MSG_DEV_ENUMERATED);
	DDL_DelayMS(30);
    Debug("To see the root content of the disk : \n" );
}

/**
 *******************************************************************************
 ** \brief  Device is not supported
 ** \param  None
 ** \retval None
 ******************************************************************************/
void USBH_USR_DeviceNotSupported(void)
{
    Debug ("> Device not supported.\n");
}

/**
 *******************************************************************************
 ** \brief  User Action for application state entry
 ** \param  None
 ** \retval USBH_USR_Status : User response for key button
 ******************************************************************************/
USBH_USR_Status USBH_USR_UserInput(void)
{
    USBH_USR_Status usbh_usr_status;

    usbh_usr_status = USBH_USR_NO_RESP;

    /*Key B3 is in polling mode to detect user action */
//    if(GET_BUTTON_KEY() == Reset)
//    {
//        usbh_usr_status = USBH_USR_RESP_OK;
//    }

    return usbh_usr_status;
}

/**
 *******************************************************************************
 ** \brief  Over Current Detected on VBUS
 ** \param  None
 ** \retval None
 ******************************************************************************/
void USBH_USR_OverCurrentDetected (void)
{
    Debug ("Overcurrent detected.\n");
}


/**
 * @brief USB应用处理
 * 1、U盘没有升级文件
 * 2、U盘里面只有APP升级文件
 * 3、U盘里面只有LED DRV升级文件
 * 4、U盘里面有APP和LED DRV升级文件
 * 
 * @return int 
 */
int USBH_USR_MSC_Application(void)
{
//    usb_msc_application_handle();
    if(1 == app_usb_fatfile_state_get())
        usb_update_handle();
    return((int)0);
}



/**
 *******************************************************************************
 ** \brief  Toggle leds to shows user input state
 ** \param  None
 ** \retval None
 ******************************************************************************/

/**
 *******************************************************************************
 ** \brief  Deint User state and associated variables
 ** \param  None
 ** \retval None
 ******************************************************************************/
void USBH_USR_DeInit(void)
{
    USBH_USR_ApplicationState = USH_USR_FS_INIT;                        
//    OLED_Fill(12,44,127,64,0);  //chen
//    OLED_RefreshGram();         
}


/*******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/
