#include "hal_iic.h"
#include "hc32_ddl.h"

/*
IIC地址分配
IIC2：
WW1：   0001100
WW2:    0001101
B:      0001001 

IIC4：
CW1:    0001100 
CW2:    0001101
R:      0001001 
G:      0001000
*/



#define I2C_TIMEOUT                     (0x000F0U)

struct hc4a0_iic_cfg
{
    M4_I2C_TypeDef* iic_x;
    uint32_t fcg_periph;
    
    uint8_t  scl_port;
    uint16_t scl_pin;
    uint16_t scl_func;
    
    uint8_t  sda_port;
    uint16_t sda_pin;
    uint16_t sda_func;
    
};

struct hc4a0_iic_cfg iic_cfg_arg[HAL_IIC_USE_NUM] = 
{
   
    {
        .iic_x = M4_I2C2,
        .fcg_periph = PWC_FCG1_I2C2,
        
        .scl_port = GPIO_PORT_G,
        .scl_pin = GPIO_PIN_00,
        .scl_func = GPIO_FUNC_51_I2C2_SCL,
        
        .sda_port = GPIO_PORT_G,
        .sda_pin = GPIO_PIN_01,
        .sda_func = GPIO_FUNC_50_I2C2_SDA,   
    },
   
    {
        .iic_x = M4_I2C4,
        .fcg_periph = PWC_FCG1_I2C4,
        .scl_port = GPIO_PORT_H,
        .scl_pin = GPIO_PIN_07,
        .scl_func = GPIO_FUNC_53_I2C4_SCL,
        
        .sda_port = GPIO_PORT_H,
        .sda_pin = GPIO_PIN_08,
        .sda_func = GPIO_FUNC_52_I2C4_SDA,    
    },
};


void hal_iic_init(enum iic_unit_enum iic_type)
{
    /* Initialize I2C port*/
    stc_gpio_init_t stcGpioInit;
    stc_i2c_init_t stcI2cInit;
    stc_irq_signin_config_t stcIrqSignCfg;
    float32_t fErr;
    
    (void)GPIO_StructInit(&stcGpioInit);
    (void)GPIO_Init(iic_cfg_arg[iic_type].scl_port, iic_cfg_arg[iic_type].scl_pin, &stcGpioInit);
    (void)GPIO_Init(iic_cfg_arg[iic_type].sda_port, iic_cfg_arg[iic_type].sda_pin, &stcGpioInit);
    GPIO_SetFunc(iic_cfg_arg[iic_type].scl_port, iic_cfg_arg[iic_type].scl_pin, \
        iic_cfg_arg[iic_type].scl_func, PIN_SUBFUNC_DISABLE);
    GPIO_SetFunc(iic_cfg_arg[iic_type].sda_port, iic_cfg_arg[iic_type].sda_pin,\
        iic_cfg_arg[iic_type].sda_func, PIN_SUBFUNC_DISABLE);  
    /* Enable peripheral clock */
    PWC_Fcg1PeriphClockCmd(iic_cfg_arg[iic_type].fcg_periph, Enable);
    I2C_DeInit(iic_cfg_arg[iic_type].iic_x);
        
    (void)I2C_StructInit(&stcI2cInit);
    stcI2cInit.u32Baudrate = 400000UL;   //IIC 波特率
    stcI2cInit.u32SclTime = 5U;
    stcI2cInit.u32ClkDiv = I2C_CLK_DIV4;
    en_result_t enRet = I2C_Init(iic_cfg_arg[iic_type].iic_x, &stcI2cInit, &fErr);
    if(enRet == Ok)
    {
        /* I2C function command */
        I2C_Cmd(iic_cfg_arg[iic_type].iic_x, Enable);
    }
}

/**
* @brief iic 发送数据
 * 
 * @param[in]value          值
 * @return uint8_t    0--成功   非0--失败
 */
uint8_t hal_iic_send_data(enum iic_unit_enum iic_type, uint8_t addr, uint8_t* pbuf, uint32_t data_len)
{
    uint8_t result = 0;
    uint8_t addr1 = 0x0c;
//    SET_REG32_BIT(iic_cfg_arg[iic_type].iic_x->CR1,I2C_CR1_SWRST);
//	CLEAR_REG32_BIT(iic_cfg_arg[iic_type].iic_x->CR1,I2C_CR1_SWRST);
    result += I2C_Start(iic_cfg_arg[iic_type].iic_x,I2C_TIMEOUT);
    result += I2C_TransAddr(iic_cfg_arg[iic_type].iic_x,addr,I2C_DIR_TX,I2C_TIMEOUT);
    result += I2C_TransData(iic_cfg_arg[iic_type].iic_x, pbuf, data_len,I2C_TIMEOUT);
    result += I2C_Stop(iic_cfg_arg[iic_type].iic_x,I2C_TIMEOUT);
    return result;
}


/**
* @brief iic 接收数据
 * 
 * @param[in]value          值
 * @return uint8_t    0--成功   非0--失败
 */
uint8_t hal_iic_recv_data(enum iic_unit_enum iic_type, uint8_t addr, uint8_t* pbuf, uint32_t data_len)
{
    uint8_t result = 0;

    result += I2C_Start(iic_cfg_arg[iic_type].iic_x,I2C_TIMEOUT);
    if(data_len == 1)
        I2C_AckConfig(iic_cfg_arg[iic_type].iic_x, I2C_NACK);
    result += I2C_TransAddr(iic_cfg_arg[iic_type].iic_x,addr,I2C_DIR_RX,I2C_TIMEOUT);
    result += I2C_MasterReceiveAndStop(iic_cfg_arg[iic_type].iic_x,pbuf, data_len,I2C_TIMEOUT);

    return result;
}




