
#ifndef __HC32F4A0_USBFS_H__
#define __HC32F4A0_USBFS_H__

/*******************************************************************************
 * Include files
 ******************************************************************************/
#include "hc32_common.h"
#include "ddl_config.h"

//#include "hc32f4a0_usb_core.h"
//#include "hc32f4a0_usb_dcd.h"
//#include "hc32f4a0_usb_hcd.h"
//#include "hc32f4a0_usb_dcd_int.h"
//#include "hc32f4a0_usb_hcd_int.h"
//#include "hc32f4a0_usb_otg.h"

//#include "hc32f4a0_usb_defines.h"
//#include "hc32f4a0_usb_otg_regs.h"

//#include "usbd_conf.h"
//#include "usbd_core.h"
//#include "usbd_def.h"
//#include "usbd_ioreq.h"
//#include "usbd_req.h"

//#include "usbd_hid_custom_core.h"

#if (DDL_USBFS_ENABLE == DDL_ON)

#include "usb_core.h"
#include "usb_dcd.h"
#include "usb_hcd.h"
#include "usb_dcd_int.h"
#include "usb_hcd_int.h"
#include "usb_otg.h"

#include "usb_defines.h"
#include "usb_otg_regs.h"

#include "usbh_msc_bot.h"
#include "usbh_msc_core.h"
#include "usbh_msc_scsi.h"


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

#endif /* DDL_USBFS_ENABLE */
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


#endif /* __HC32F4a0_USBFS_H__ */

/*******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/
