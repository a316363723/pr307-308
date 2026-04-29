#ifndef HAL_IIC_H
#define HAL_IIC_H
#include "stdint.h"

#define IIC_WW1_ADDRESS     0x0C
#define IIC_WW2_ADDRESS     0x0D
#define IIC_B_ADDRESS       0x09

#define IIC_CW1_ADDRESS     0x0C
#define IIC_CW2_ADDRESS     0x0D
#define IIC_R_ADDRESS       0x09
#define IIC_G_ADDRESS       0x08

enum iic_unit_enum
{
    HAL_IIC_ANALOG_WW1_WW2_B,
    HAL_IIC_ANALOG_CW1_CW2_R_G,
    HAL_IIC_USE_NUM,
};

void hal_iic_init(enum iic_unit_enum iic_type);
uint8_t hal_iic_send_data(enum iic_unit_enum iic_type, uint8_t addr, uint8_t* pbuf, uint32_t data_len);
uint8_t hal_iic_recv_data(enum iic_unit_enum iic_type, uint8_t addr, uint8_t* pbuf, uint32_t data_len);

#endif

