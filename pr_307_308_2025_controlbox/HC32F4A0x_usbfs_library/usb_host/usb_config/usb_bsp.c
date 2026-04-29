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
/** \file usb_bsp_template.c
 **
 ** A detailed description is available at
 ** @link
        This file is responsible to offer board support package and is
        configurable by user.
    @endlink
 **
 **   - 2018-05-21  1.0  gouwei First version for USB demo.
 **
 ******************************************************************************/

/*******************************************************************************
 * Include files
 ******************************************************************************/
#include "usb_bsp.h"
#include "hc32_ddl.h"
#include "gen_drv.h"
#include "hc32f4a0_usb_core_driver.h"
#include "usb_host_msc_class.h"
#include "usb_host_user.h"

/* USBF Core*/
#define USB_DP_PORT     (GPIO_PORT_A)
#define USB_DP_PIN      (GPIO_PIN_12)
#define USB_DM_PORT     (GPIO_PORT_A)
#define USB_DM_PIN      (GPIO_PIN_11)
#define USB_VBUS_EN_PORT   	  (GPIO_PORT_A)	//USB电源供电引脚控制
#define USB_VBUS_EN_PIN    	  (GPIO_PIN_10)
#define USB_Power_Enable()    GPIO_SetPins(USB_VBUS_EN_PORT, USB_VBUS_EN_PIN)  //
#define USB_Power_Disable()   GPIO_ResetPins(USB_VBUS_EN_PORT, USB_VBUS_EN_PIN)


/*******************************************************************************
 * Global variable definitions (declared in header file with 'extern')
 ******************************************************************************/
// #ifdef USB_OTG_HS_INTERNAL_DMA_ENABLED
//  #if defined ( __ICCARM__ ) /*!< IAR Compiler */
//    #pragma data_alignment=4
//  #endif
//#endif /* USB_OTG_HS_INTERNAL_DMA_ENABLED */


//#ifdef USB_OTG_HS_INTERNAL_DMA_ENABLED
//  #if defined ( __ICCARM__ ) /*!< IAR Compiler */
//    #pragma data_alignment=4
//  #endif
//#endif /* USB_OTG_HS_INTERNAL_DMA_ENABLED */

//__USB_ALIGN_BEGIN USB_OTG_CORE_HANDLE      USB_OTG_Core __USB_ALIGN_END;
//__USB_ALIGN_BEGIN USBH_HOST                USB_Host __USB_ALIGN_END;
//extern const stc_disk_drv_t gDiskDrv;
extern char USBHPath[4];
extern void hd_usb_prtsusp(usb_core_instance *pdev);
extern  usb_core_instance usb_app_instance;
extern  void hd_usb_host_isr(usb_core_instance *pdev);

#ifdef USB_INTERNAL_DMA_ENABLED
  #if defined ( __ICCARM__ ) /*!< IAR Compiler */
    #pragma data_alignment=4
  #endif
#endif /* USB_INTERNAL_DMA_ENABLED */
__USB_ALIGN_BEGIN usb_core_instance      usb_app_instance __USB_ALIGN_END;

#ifdef USB_INTERNAL_DMA_ENABLED
  #if defined ( __ICCARM__ ) /*!< IAR Compiler */
    #pragma data_alignment=4
  #endif
#endif /* USB_INTERNAL_DMA_ENABLED */
__USB_ALIGN_BEGIN USBH_HOST                usb_app_host __USB_ALIGN_END;


void usbfs_init(void)
{
//    FatFS_LinkDriver(gDiskDrv.drv[0], USBHPath);
	/* Init Host Library */
//    USBH_Init(&USB_OTG_Core,
//#ifdef USE_USB_OTG_FS
//              USB_OTG_FS_CORE_ID,
//#else
//              USB_OTG_HS_CORE_ID,
//#endif
//              &USB_Host,
//              &USBH_MSC_cb,
//              &USR_cb);
	usb_host_init(&usb_app_instance, &usb_app_host, &USBH_MSC_cb, &USR_cb);
}

/**
 *******************************************************************************
 ** \brief  initialize configurations for the BSP
 ** \param  pdev: device instance
 ** \retval none
 ******************************************************************************/
void hd_usb_bsp_init(usb_core_instance *pdev)
{
    stc_gpio_init_t stcGpioCfg;
    /* USB clock source configurate */
    CLK_USB_ClkConfig(CLK_USB_CLK_MCLK_DIV5);
    GPIO_StructInit(&stcGpioCfg);

    stcGpioCfg.u16PinAttr = PIN_ATTR_ANALOG;
    GPIO_Init(USB_DM_PORT, USB_DM_PIN, &stcGpioCfg);
    GPIO_Init(USB_DP_PORT, USB_DP_PIN, &stcGpioCfg);
//    GPIO_SetFunc(USB_VBUS_EN_PORT, USB_VBUS_EN_PORT, GPIO_FUNC_10_USBF_VBUS, PIN_SUBFUNC_DISABLE); //VBUS
    PWC_Fcg1PeriphClockCmd(PWC_FCG1_USBFS, Enable);
	
	(void)GPIO_StructInit(&stcGpioCfg);
	stcGpioCfg.u16PinDir = PIN_DIR_OUT;
	(void)GPIO_Init(USB_VBUS_EN_PORT, USB_VBUS_EN_PIN, &stcGpioCfg);
	GPIO_OE(USB_VBUS_EN_PORT, USB_VBUS_EN_PIN, Enable);
	USB_Power_Enable();   
}

/**
 *******************************************************************************
 ** \brief  handle the USB interrupt
 ** \param  none
 ** \retval none
 ******************************************************************************/
static void USB_IRQ_Handler(void)
{
    hd_usb_host_isr(&usb_app_instance);
}

/**
 *******************************************************************************
 ** \brief  configure the NVIC of USB
 ** \param  none
 ** \retval none
 ******************************************************************************/
void usb_bsp_nvic(void)
{
    stc_irq_signin_config_t stcIrqRegiConf;
    /* Register INT_USBFS_GLB Int to Vect.No.030 */
    stcIrqRegiConf.enIRQn = Int030_IRQn;
    /* Select interrupt function */
#ifdef USB_FS_MODE
    stcIrqRegiConf.enIntSrc = INT_USBFS_GLB;
#else
    stcIrqRegiConf.enIntSrc = INT_USBHS_GLB;
#endif
    /* Callback function */
    stcIrqRegiConf.pfnCallback = &USB_IRQ_Handler;
    /* Registration IRQ */
    INTC_IrqSignIn(&stcIrqRegiConf);
    /* Clear Pending */
    NVIC_ClearPendingIRQ(stcIrqRegiConf.enIRQn);
    /* Set priority */
    NVIC_SetPriority(stcIrqRegiConf.enIRQn, DDL_IRQ_PRIORITY_15);
    /* Enable NVIC */
    NVIC_EnableIRQ(stcIrqRegiConf.enIRQn);
}

/**
 *******************************************************************************
 ** \brief  Configures the IO for the Vbus and OverCurrent
 ** \param  speed : Full, Low
 ** \retval None
 ******************************************************************************/
void  hd_usb_bsp_cfgvbus(usb_core_instance *pdev)
{
}

/**
 *******************************************************************************
 ** \brief  Drive vbus
 ** \param  pdev : device instance
 ** \param  state : the vbus state it would be.
 ** \retval None
 ******************************************************************************/
void hd_usb_bsp_drivevbus(usb_core_instance *pdev, uint8_t state)
{
}

/**
 *******************************************************************************
 ** \brief  This function provides delay time in micro sec
 ** \param  usec : Value of delay required in micro sec
 ** \retval None
 ******************************************************************************/
void hd_usb_udelay(const uint32_t usec)
{
//    volatile uint32_t    i;
//    uint32_t    j;
//    j = HCLK_VALUE / 1000000UL * usec;
//    for(i = 0UL; i < j; i++)
//    {
//        ;
//    }
    unsigned long   i;
	for(i=0; i<usec; i++)
	{
		DDL_DelayUS(1);   //240M系统时讚->6讛应10us,609讛应1ms(示舀欠英藬)
	}
}

/**
 *******************************************************************************
 ** \brief  This function provides delay time in milli sec
 ** \param  msec : Value of delay required in milli sec
 ** \retval None
 ******************************************************************************/
void hd_usb_mdelay(const uint32_t msec)
{
    unsigned long   i;
	for(i=0; i<msec; i++)
	{
		DDL_DelayUS(609);   //240M系统时讚->6讛应10us,609讛应1ms(示舀欠英藬)
	}
}

//void USB_IRQ_Handler(void)
//{
//    //Debug("usb isr\n");
//    USBH_OTG_ISR_Handler(&USB_OTG_Core);
//}

///* USB电源使能脚 */
//static void USB_Power_GPIO_Config(void)
//{
//	stc_gpio_init_t stcGpioInit;
//	(void)GPIO_StructInit(&stcGpioInit);
//	stcGpioInit.u16PinDir = PIN_DIR_OUT;
//	(void)GPIO_Init(USB_VBUS_EN_PORT, USB_VBUS_EN_PIN, &stcGpioInit);
//	GPIO_OE(USB_VBUS_EN_PORT, USB_VBUS_EN_PIN, Enable);
//	USB_Power_Enable();   
//}

///**
// *******************************************************************************
// ** \brief  Initilizes BSP configurations
// ** \param  None
// ** \retval None
// ******************************************************************************/
//void USB_OTG_BSP_Init(USB_OTG_CORE_HANDLE *pdev)
//{
//    stc_gpio_init_t stcGpioCfg;
//	
//	//Debug("USBFS start !!\n");
//	/* USB clock source configurate */
//    CLK_USB_ClkConfig(CLK_USB_CLK_MCLK_DIV5); //  240/5 = 48Mhz
//	
//	GPIO_StructInit(&stcGpioCfg);
//	/* Disable digital function for DM DP */
//    stcGpioCfg.u16PinAttr = PIN_ATTR_ANALOG;
//    GPIO_Init(USB_DM_PORT, USB_DM_PIN, &stcGpioCfg);
//    GPIO_Init(USB_DP_PORT, USB_DP_PIN, &stcGpioCfg);
////	GPIO_SetFunc(USB_VBUS_EN_PORT, USB_VBUS_EN_PIN, GPIO_FUNC_10_USBF_DRVVBUS, PIN_SUBFUNC_DISABLE); //VBUS，用PC9才可以，不能换成普通引脚
//	PWC_Fcg1PeriphClockCmd(PWC_FCG1_USBFS, Enable);
//	USB_Power_GPIO_Config();
//}

///**
// *******************************************************************************
// ** \brief  Enabele USB Global interrupt
// ** \param  None
// ** \retval None
// ******************************************************************************/
//void USB_OTG_BSP_EnableInterrupt(void)
//{
//    stc_irq_signin_config_t stcIrqRegiConf;
//    /* Register INT_USBFS_GLB Int to Vect.No.030 */
//    stcIrqRegiConf.enIRQn = Int030_IRQn;
//    /* Select INT_USBFS_GLB interrupt function */
//    stcIrqRegiConf.enIntSrc = INT_USBFS_GLB;
//    /* Callback function */
//    stcIrqRegiConf.pfnCallback = &USB_IRQ_Handler;
//    /* Registration IRQ */
//    INTC_IrqSignIn(&stcIrqRegiConf);
//    /* Clear Pending */
//    NVIC_ClearPendingIRQ(stcIrqRegiConf.enIRQn);
//    /* Set priority */
//    NVIC_SetPriority(stcIrqRegiConf.enIRQn, DDL_IRQ_PRIORITY_15);
//    /* Enable NVIC */
//    NVIC_EnableIRQ(stcIrqRegiConf.enIRQn);
//}

///**
// *******************************************************************************
// ** \brief  Drives the Vbus signal through IO
// ** \param  speed : Full, Low
// ** \param  state : VBUS states
// ** \retval None
// ******************************************************************************/
//void USB_OTG_BSP_DriveVBUS(USB_OTG_CORE_HANDLE *pdev,uint8_t state)
//{
//  USB_OTG_HPRT0_TypeDef  hprt0;

////  hprt0.d32 = USB_OTG_ReadHPRT0(pdev);
////  hprt0.b.prtlnsts = state;
////  USB_OTG_WRITE_REG32(pdev->regs.HPRT0, hprt0.d32);
//}


///**
// *******************************************************************************
// ** \brief  Configures the IO for the Vbus and OverCurrent
// ** \param  speed : Full, Low
// ** \retval None
// ******************************************************************************/
//void  USB_OTG_BSP_ConfigVBUS(USB_OTG_CORE_HANDLE *pdev)
//{
//}

///**
// *******************************************************************************
// ** \brief  Initialises delay unit Systick timer /Timer2
// ** \param  None
// ** \retval None
// ******************************************************************************/
//void USB_OTG_BSP_TimeInit ( void )
//{

//}

///**
// *******************************************************************************
// ** \brief  This function provides delay time in micro sec
// ** \param  usec : Value of delay required in micro sec
// ** \retval None
// ******************************************************************************/

//void USB_OTG_BSP_uDelay (const uint32_t t)
//{
//    uint32_t    i;
//	for(i=0; i<t; i++)
//	{
//		DDL_DelayUS(1);		//240M系统时钟->6对应10us,1是2us(示波器测试)
//	}
//}


///**
// *******************************************************************************
// ** \brief  This function provides delay time in milli sec
// ** \param  msec : Value of delay required in milli sec
// ** \retval None
// ******************************************************************************/
//void USB_OTG_BSP_mDelay (const uint32_t msec)
//{
//    unsigned long   i;
//	for(i=0; i<msec; i++)
//	{
//		DDL_DelayUS(609);   //240M系统时钟->6对应10us,609对应1ms(示波器测试)
//	}
//}

///**
// *******************************************************************************
// ** \brief  Time base IRQ
// ** \param  None
// ** \retval None
// ******************************************************************************/
//void USB_OTG_BSP_TimerIRQ (void)
//{

//}

/*******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/
