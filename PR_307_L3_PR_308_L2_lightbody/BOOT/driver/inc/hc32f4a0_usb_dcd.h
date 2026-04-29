
#ifndef __HC32F4A0_USB_DCD_H__
#define __HC32F4A0_USB_DCD_H__

/* C binding of definitions if building with C++ compiler */
#ifdef __cplusplus
extern "C"
{
#endif

/*******************************************************************************
 * Include files
 ******************************************************************************/
#include "hc32f4a0_usb_core.h"

/**
 * @addtogroup HC32F4A0_DDL_Driver
 * @{
 */

/**
 * @addtogroup DDL_USB_DCD
 * @{
 */

#if (DDL_USBFS_ENABLE == DDL_ON)

/*******************************************************************************
 * Global type definitions ('typedef')
 ******************************************************************************/
/**
 * @defgroup USB_DCD_Global_Types USB DCD Global Types
 * @{
 */

typedef struct
{
    uint8_t  bLength;
    uint8_t  bDescriptorType;
    uint8_t  bEndpointAddress;
    uint8_t  bmAttributes;
    uint16_t wMaxPacketSize;
    uint8_t  bInterval;
}EP_DESCRIPTOR , *PEP_DESCRIPTOR;

/**
 * @}
 */

/*******************************************************************************
 * Global pre-processor symbols/macros ('#define')
 ******************************************************************************/
/**
 * @defgroup USB_DCD_Global_Macros USB DCD Global Macros
 * @{
 */

#define USB_OTG_EP_CONTROL                       (0U)
#define USB_OTG_EP_ISOC                          (1U)
#define USB_OTG_EP_BULK                          (2U)
#define USB_OTG_EP_INT                           (3U)
#define USB_OTG_EP_MASK                          (3U)

/*  Device Status */
#define USB_OTG_DEFAULT                          (1U)
#define USB_OTG_ADDRESSED                        (2U)
#define USB_OTG_CONFIGURED                       (3U)
#define USB_OTG_SUSPENDED                        (4U)
/**
 * @}
 */

/*******************************************************************************
 * Global variable definitions ('extern')
 ******************************************************************************/

/*******************************************************************************
  Global function prototypes (definition in C source)
 ******************************************************************************/
/**
 * @addtogroup USB_DCD_Global_Functions
 * @{
 */
void        DCD_Init(USB_OTG_CORE_HANDLE *pdev, USB_OTG_CORE_ID_TypeDef coreID);
void        DCD_DevConnect (USB_OTG_CORE_HANDLE *pdev);
void        DCD_DevDisconnect (USB_OTG_CORE_HANDLE *pdev);
void        DCD_EP_SetAddress (USB_OTG_CORE_HANDLE *pdev, uint8_t address);
uint32_t    DCD_EP_Open(USB_OTG_CORE_HANDLE *pdev ,
                        uint8_t ep_addr,
                        uint16_t ep_mps,
                        uint8_t ep_type);
uint32_t    DCD_EP_Close  (USB_OTG_CORE_HANDLE *pdev, uint8_t ep_addr);
uint32_t    DCD_EP_PrepareRx ( USB_OTG_CORE_HANDLE *pdev,
                                uint8_t   ep_addr,
                                uint8_t *pbuf,
                                uint16_t  buf_len);
uint32_t    DCD_EP_Tx (USB_OTG_CORE_HANDLE *pdev,
                               uint8_t  ep_addr,
                               uint8_t  *pbuf,
                               uint32_t   buf_len);
uint32_t    DCD_EP_Stall (USB_OTG_CORE_HANDLE *pdev, uint8_t epnum);
uint32_t    DCD_EP_ClrStall (USB_OTG_CORE_HANDLE *pdev, uint8_t epnum);
uint32_t    DCD_EP_Flush (USB_OTG_CORE_HANDLE *pdev, uint8_t epnum);
uint32_t    DCD_Handle_ISR(USB_OTG_CORE_HANDLE *pdev);
uint32_t    DCD_GetEPStatus(USB_OTG_CORE_HANDLE *pdev, uint8_t epnum);
void        DCD_SetEPStatus (USB_OTG_CORE_HANDLE *pdev ,
                             uint8_t epnum ,
                             uint32_t Status);
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

#ifdef __cplusplus
}
#endif

#endif /* __HC32F4A0_USB_DCD_H__ */

/*******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/
