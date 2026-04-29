
/*******************************************************************************
 * Include files
 ******************************************************************************/
#include "usb_conf.h"
#include "gen_drv.h"
#include "usbh_msc_core.h"

/**
 * @addtogroup MIDWARE
 * @{
 */

/**
 * @addtogroup USB_HOST_LIB
 * @{
 */

/**
 * @addtogroup USB_HOST_CLASS
 * @{
 */

/** @defgroup USBH_MSC_FATFS USB host MSC FATFS
 * @{
 */

#if (DDL_USBFS_ENABLE == DDL_ON)

/*******************************************************************************
 * Local type definitions ('typedef')
 ******************************************************************************/

/*******************************************************************************
 * Local pre-processor symbols/macros ('#define')
 ******************************************************************************/

/*******************************************************************************
 * Global variable definitions (declared in header file with 'extern')
 ******************************************************************************/

/*******************************************************************************
 * Local function prototypes ('static')
 ******************************************************************************/
DSTATUS MSC_Disk_Initialize (BYTE drv);
DSTATUS MSC_Disk_Status (BYTE drv);
DRESULT MSC_Disk_Read(BYTE drv, BYTE *buff, DWORD sector, UINT count);
DRESULT MSC_Disk_Write(BYTE drv, const BYTE *buff, DWORD sector, UINT count);
DRESULT MSC_Disk_Ioctl(BYTE drv, BYTE ctrl, void *buff);


/*******************************************************************************
 * Local variable definitions ('static')
 ******************************************************************************/
static volatile DSTATUS Stat = STA_NOINIT;  /* Disk status */

const stc_diskio_drv_t gMSCDiskDriver =
{
    &MSC_Disk_Initialize,
    &MSC_Disk_Status,
    &MSC_Disk_Read,
    &MSC_Disk_Write,
    &MSC_Disk_Ioctl,
};

/*******************************************************************************
 * Function implementation - global ('extern') and local ('static')
 ******************************************************************************/
extern USB_OTG_CORE_HANDLE      USB_OTG_Core;
extern USBH_HOST                USB_Host;

/**
 * @defgroup USBH_MSC_FATFS_Global_Functions USBH MSC FATFS Global Functions
 * @{
 */

/**
* @brief  Initialize Disk Drive
* @param  drv Physical drive number (0)
* @retval DSTATUS
*/
DSTATUS MSC_Disk_Initialize (
                         BYTE drv       /* Physical drive number (0) */
                           )
{
    if(HCD_IsDeviceConnected(&USB_OTG_Core))
    {
        Stat &= (DSTATUS)~STA_NOINIT;
    }
    return Stat;
}

/**
* @brief  Get Disk Status
* @param  drv Physical drive number (0)
* @retval DSTATUS
*/
DSTATUS MSC_Disk_Status (
                     BYTE drv       /* Physical drive number (0) */
                       )
{
    DSTATUS status = Stat;
    if (drv)
    {
        status = STA_NOINIT;        /* Supports only single drive */
    }
    return status;
}

/**
* @brief  Read Sector(s)
* @param  drv Physical drive number (0)
* @param  buff Pointer to the data buffer to store read data
* @param  sector Start sector number (LBA)
* @param  count Sector count (1..255)
* @retval DSTATUS
*/
DRESULT MSC_Disk_Read (
                   BYTE drv,            /* Physical drive number (0) */
                   BYTE *buff,          /* Pointer to the data buffer to store read data */
                   DWORD sector,        /* Start sector number (LBA) */
                   UINT count           /* Sector count (1..255) */
                     )
{
    BYTE status = USBH_MSC_OK;

    if (drv || (!count))
    {
        return RES_PARERR;
    }
    if (Stat & STA_NOINIT)
    {
        return RES_NOTRDY;
    }

    if(HCD_IsDeviceConnected(&USB_OTG_Core))
    {

        do
        {
            status = USBH_MSC_Read10(&USB_OTG_Core, buff,sector,512ul * (uint32_t)count);
            USBH_MSC_HandleBOTXfer(&USB_OTG_Core ,&USB_Host);

            if(!HCD_IsDeviceConnected(&USB_OTG_Core))
            {
                return RES_ERROR;
            }
        }
        while(status == USBH_MSC_BUSY );
    }

    if(status == USBH_MSC_OK)
    {
        return RES_OK;
    }
    return RES_ERROR;

}


#if _READONLY == 0
/**
* @brief  Write Sector(s)
* @param  drv Physical drive number (0)
* @param  buff Pointer to the data to be written
* @param  sector Start sector number (LBA)
* @param  count Sector count (1..255)
* @retval DSTATUS
*/
DRESULT MSC_Disk_Write (
                    BYTE drv,           /* Physical drive number (0) */
                    const BYTE *buff,   /* Pointer to the data to be written */
                    DWORD sector,       /* Start sector number (LBA) */
                    UINT count          /* Sector count (1..255) */
                      )
{
    BYTE status = USBH_MSC_OK;

    if (drv || (!count))
    {
        return RES_PARERR;
    }
    if (Stat & STA_NOINIT)
    {
        return RES_NOTRDY;
    }
    if (Stat & STA_PROTECT)
    {
        return RES_WRPRT;
    }
    if(HCD_IsDeviceConnected(&USB_OTG_Core))
    {
        do
        {
            status = USBH_MSC_Write10(&USB_OTG_Core,(BYTE*)buff,sector,512ul * (uint32_t)count);
            USBH_MSC_HandleBOTXfer(&USB_OTG_Core, &USB_Host);

            if(!HCD_IsDeviceConnected(&USB_OTG_Core))
            {
                return RES_ERROR;
            }
        }
        while(status == USBH_MSC_BUSY );
    }

    if(status == USBH_MSC_OK)
    {
        return RES_OK;
    }
    return RES_ERROR;
}
#endif /* _READONLY == 0 */

/**
* @brief  Miscellaneous Functions
* @param  drv Physical drive number (0)
* @param  ctrl Control code
* @param  buff Buffer to send/receive control data
 **
* @retval DSTATUS
*/
DRESULT MSC_Disk_Ioctl (
                    BYTE drv,       /* Physical drive number (0) */
                    BYTE ctrl,      /* Control code */
                    void *buff      /* Buffer to send/receive control data */
                      )
{
    DRESULT res = RES_OK;

    if (drv)
    {
        return RES_PARERR;
    }

    res = RES_ERROR;

    if (Stat & STA_NOINIT)
    {
        return RES_NOTRDY;
    }

    switch (ctrl)
    {
        case CTRL_SYNC :        /* Make sure that no pending write process */
            res = RES_OK;
            break;
        case GET_SECTOR_COUNT : /* Get number of sectors on the disk (DWORD) */
            *(DWORD*)buff = (DWORD) USBH_MSC_Param.MSCapacity;
            res = RES_OK;
            break;
        case GET_SECTOR_SIZE :  /* Get R/W sector size (WORD) */
            *(WORD*)buff = 512u;
            res = RES_OK;
            break;
        case GET_BLOCK_SIZE :   /* Get erase block size in unit of sector (DWORD) */
            *(DWORD*)buff = 512u;
            break;
        default:
            res = RES_PARERR;
            break;
    }
    return res;
}

/**
 * @}
 */

#endif /* DDL_USBFS_ENABLE */

/**
 * @}
 */

/**
 * @}
 */

/**
 * @}
 */

/**
* @}
*/

/*******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/
