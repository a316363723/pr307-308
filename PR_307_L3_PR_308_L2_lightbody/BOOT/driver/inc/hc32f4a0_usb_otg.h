
#ifndef __HC32F4A0_USB_OTG_H__
#define __HC32F4A0_USB_OTG_H__

/* C binding of definitions if building with C++ compiler */
#ifdef __cplusplus
extern "C"
{
#endif


/**
 * @addtogroup HC32F4A0_DDL_Driver
 * @{
 */

/**
 * @addtogroup DDL_USB_OTG
 * @{
 */

#if (DDL_USBFS_ENABLE == DDL_ON)


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
//void USB_OTG_InitiateSRP(USB_OTG_CORE_HANDLE *pdev , uint8_t state, uint8_t mode);
//void USB_OTG_InitiateHNP(uint8_t state , uint8_t mode);
//void USB_OTG_Switchback (USB_OTG_CORE_DEVICE *pdev);
//uint32_t  USB_OTG_GetCurrentState (USB_OTG_CORE_DEVICE *pdev);


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


#endif /* __HC32F4A0_USB_OTG_H__ */

/*******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/
