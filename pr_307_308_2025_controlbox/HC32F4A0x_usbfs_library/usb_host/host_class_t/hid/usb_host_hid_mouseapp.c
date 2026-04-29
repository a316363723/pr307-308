/**
 *******************************************************************************
 * @file  usb_host_hid_mouseapp.c
 * @brief The application layer for USB Host HID Mouse Handling.
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

/*******************************************************************************
 * Include files
 ******************************************************************************/
#include "usb_host_hid_mouseapp.h"

/*******************************************************************************
 * Local type definitions ('typedef')
 ******************************************************************************/

/*******************************************************************************
 * Local pre-processor symbols/macros ('#define')
 ******************************************************************************/

/*******************************************************************************
 * Local variable definitions ('static')
 ******************************************************************************/
void mouse_init(void);
void mouse_decode(uint8_t *data);

/*******************************************************************************
 * Global variable definitions (declared in header file with 'extern')
 ******************************************************************************/
#ifdef USB_INTERNAL_DMA_ENABLED
 #if defined   (__CC_ARM) /*!< ARM Compiler */
  __align(4)
 #elif defined ( __ICCARM__ ) /*!< IAR Compiler */
  #pragma data_alignment=4
 #elif defined (__GNUC__) /*!< GNU Compiler */
 #pragma pack(4)
 #elif defined  (__TASKING__) /*!< TASKING Compiler */
  __align(4)
 #endif /* __CC_ARM */
#endif

HID_MOUSE_Data_TypeDef HID_MOUSE_Data;
HID_cb_TypeDef HID_MOUSE_cb =
{
    mouse_init,
    mouse_decode,
};

static void  user_mouse_init ( void)
{
    /* Call User Init*/
    //USR_MOUSE_Init();
}

/**
* @brief  USR_MOUSE_ProcessData
*         Process Mouse data
* @param  data      Mouse data to be displayed
* @retval None
*/
static void user_mouse_dataprocess(HID_MOUSE_Data_TypeDef *data)
{

    uint8_t idx = 1U;
    static uint8_t b_state[3U] = { 0U, 0U , 0U};

    if ((data->x != 0U) && (data->y != 0U))
    {
        //HID_MOUSE_UpdatePosition(data->x , data->y);
    }

    for ( idx = 0U ; idx < 3U ; idx ++)
    {

        if(data->button & (0x01U << idx))
        {
            if(b_state[idx] == 0U)
            {
                //HID_MOUSE_ButtonPressed (idx);
                b_state[idx] = 1U;
            }
        }
        else
        {
            if(b_state[idx] == 1U)
            {
               // HID_MOUSE_ButtonReleased (idx);
                b_state[idx] = 0U;
            }
        }
    }
}

/*******************************************************************************
 * Local variable definitions ('static')
 ******************************************************************************/
/**
 *******************************************************************************
 ** \brief  initialize the mouse application of the hid class
 ** \param  none
 ** \retval none
 ******************************************************************************/
void mouse_init(void)
{
    /* Call User Init*/
    user_mouse_init();
}

/**
 *******************************************************************************
 ** \brief  decode the mouse report data
 ** \param  data : buffer of the hid report data
 ** \retval none
 ******************************************************************************/
void mouse_decode(uint8_t *data)
{
    HID_MOUSE_Data.button = data[0];
    HID_MOUSE_Data.x      = data[1];
    HID_MOUSE_Data.y      = data[2];
    user_mouse_dataprocess(&HID_MOUSE_Data);
}

/*******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/
