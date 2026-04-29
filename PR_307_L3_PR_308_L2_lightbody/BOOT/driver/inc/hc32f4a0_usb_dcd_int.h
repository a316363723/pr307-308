
#ifndef HC32F4A0_USB_DCD_INT_H__
#define HC32F4A0_USB_DCD_INT_H__

/* C binding of definitions if building with C++ compiler */
#ifdef __cplusplus
extern "C"
{
#endif

/*******************************************************************************
 * Include files
 ******************************************************************************/
#include "hc32f4a0_usb_dcd.h"

/**
 * @addtogroup HC32F4A0_DDL_Driver
 * @{
 */

/**
 * @addtogroup DDL_USB_DCD_INT
 * @{
 */

#if (DDL_USBFS_ENABLE == DDL_ON)

/*******************************************************************************
 * Global type definitions ('typedef')
 ******************************************************************************/

/**
 * @defgroup USB_DCD_INT_Global_Types USB DCD INT Global Types
 * @{
 */

typedef struct
{
    uint8_t (* DataOutStage) (USB_OTG_CORE_HANDLE *pdev , uint8_t epnum);
    uint8_t (* DataInStage)  (USB_OTG_CORE_HANDLE *pdev , uint8_t epnum);
    uint8_t (* SetupStage) (USB_OTG_CORE_HANDLE *pdev);
    uint8_t (* SOF) (USB_OTG_CORE_HANDLE *pdev);
    uint8_t (* Reset) (USB_OTG_CORE_HANDLE *pdev);
    uint8_t (* Suspend) (USB_OTG_CORE_HANDLE *pdev);
    uint8_t (* Resume) (USB_OTG_CORE_HANDLE *pdev);
    uint8_t (* IsoINIncomplete) (USB_OTG_CORE_HANDLE *pdev);
    uint8_t (* IsoOUTIncomplete) (USB_OTG_CORE_HANDLE *pdev);

    uint8_t (* DevConnected) (USB_OTG_CORE_HANDLE *pdev);
    uint8_t (* DevDisconnected) (USB_OTG_CORE_HANDLE *pdev);
}USBD_DCD_INT_cb_TypeDef;

/**
 * @}
 */

/*******************************************************************************
 * Global pre-processor symbols/macros ('#define')
 ******************************************************************************/

/**
 * @defgroup USB_DCD_INT_Global_Macros USB DCD INT Global Macros
 * @{
 */

/* C-STAT */
#define CLEAR_IN_EP_INTR(epnum,intr) \
    {    \
        *(uint32_t*)&diepint.b = (intr);\
        USB_OTG_WRITE_REG32(&pdev->regs.INEP_REGS[(epnum)]->DIEPINT,*(uint32_t*)&diepint.b); \
    }

/* C-STAT */
#define CLEAR_OUT_EP_INTR(epnum,intr) \
    {   \
        *(uint32_t*)&doepint.b = (intr);\
        USB_OTG_WRITE_REG32(&pdev->regs.OUTEP_REGS[(epnum)]->DOEPINT,*(uint32_t*)&doepint.b); \
    }

/**
 * @}
 */

/*******************************************************************************
 * Global variable definitions ('extern')
 ******************************************************************************/
extern USBD_DCD_INT_cb_TypeDef *USBD_DCD_INT_fops;

/*******************************************************************************
  Global function prototypes (definition in C source)
 ******************************************************************************/
/**
 * @addtogroup USB_DCD_INT_Global_Functions
 * @{
 */

uint32_t USBD_OTG_ISR_Handler (USB_OTG_CORE_HANDLE *pdev);

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

#endif /* HC32F4A0_USB_DCD_INT_H__ */

/*******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/
