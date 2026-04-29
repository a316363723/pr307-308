#include "hc32_ddl.h"
#include "lsm6dsv16x_reg.h"
#include "define.h"
#define INT1_PORT                    	(GPIO_PORT_B)
#define INT1_PIN                     	(GPIO_PIN_15)
#define INT1_EXINT_CH 					(EXINT_CH15)
#define INT1_INT_IRQn 					(Int031_IRQn)
#define INT1_INT_PORT_EIR				(INT_PORT_EIRQ15)

#define I2C_TIMEOUT                     (0x2000U)
#define I2C_RET_OK                      (0U)
#define I2C_RET_ERROR                   (1U)

#define GENERATE_START                  (0x00U)
#define GENERATE_RESTART                (0x01U)

#define ADDRESS_W                       (0x00U)
#define ADDRESS_R                       (0x01U)
#define I2C_BAUDRATE                    (300000UL)

#define I2C_SCL1_PORT                    (GPIO_PORT_B)
#define I2C_SCL1_PIN                     (GPIO_PIN_13)
#define I2C_SDA1_PORT                    (GPIO_PORT_B)
#define I2C_SDA1_PIN                     (GPIO_PIN_12)

 static en_result_t Master_Initialize(M4_I2C_TypeDef* I2Cx)
{
    stc_i2c_init_t stcI2cInit;
    float32_t fErr;

    I2C_DeInit(I2Cx);

    (void)I2C_StructInit(&stcI2cInit);
    stcI2cInit.u32Baudrate = I2C_BAUDRATE;
    stcI2cInit.u32SclTime = 5U;
    stcI2cInit.u32ClkDiv = I2C_CLK_DIV2;
    en_result_t enRet = I2C_Init(I2Cx, &stcI2cInit, &fErr);
    I2C_BusWaitCmd(I2Cx, Enable);

    if(enRet == Ok)
    {
        I2C_Cmd(I2Cx, Enable);
    }

    return enRet;
}

void lsm16x_iic_gpio(void)
{
		stc_gpio_init_t stcGpioInit;
		(void)GPIO_StructInit(&stcGpioInit);
//		stcGpioInit.u16PinDir = PIN_DIR_OUT;
//		stcGpioInit.u16PinOType = PIN_OTYPE_NMOS;
//		stcGpioInit.u16PullUp = PIN_PU_ON;

		(void)GPIO_Init(I2C_SCL1_PORT, I2C_SCL1_PIN, &stcGpioInit);
		(void)GPIO_Init(I2C_SDA1_PORT, I2C_SDA1_PIN, &stcGpioInit);
		GPIO_SetFunc(I2C_SCL1_PORT, I2C_SCL1_PIN, GPIO_FUNC_53_I2C3_SCL, PIN_SUBFUNC_DISABLE);
		GPIO_SetFunc(I2C_SDA1_PORT, I2C_SDA1_PIN, GPIO_FUNC_52_I2C3_SDA, PIN_SUBFUNC_DISABLE);
	
    /* Enable peripheral clock */
    PWC_Fcg1PeriphClockCmd(PWC_FCG1_I2C3, Enable);
	
	Master_Initialize(M4_I2C3);
}


void Gyro_Write_value( uint8_t device_address,const uint8_t *value,M4_I2C_TypeDef* I2Cx,uint16_t len,uint8_t reg)
{
	SET_REG32_BIT(I2Cx->CR1,I2C_CR1_SWRST);
	CLEAR_REG32_BIT(I2Cx->CR1,I2C_CR1_SWRST);
	#if NULL
	Master_StartOrRestart(GENERATE_START,I2Cx);

	Master_SendAdr(device_address,I2C_DIR_TX,I2Cx);

	Master_WriteData(&reg,1,I2Cx);
	Master_WriteData(value,len,I2Cx);

	Master_Stop(I2Cx);
	#endif
	(void)I2C_Start(I2Cx, I2C_TIMEOUT);
    (void)I2C_TransAddr(I2Cx, device_address, ADDRESS_W, I2C_TIMEOUT);
    (void)I2C_TransData(I2Cx, &reg, 1UL, I2C_TIMEOUT);
    (void)I2C_TransData(I2Cx, value, len, I2C_TIMEOUT);
    (void)I2C_Stop(I2Cx, I2C_TIMEOUT);

}

en_result_t Gyro_Mem_Read(uint8_t u8DevAddr, uint8_t *pu8Data,M4_I2C_TypeDef* I2Cx, uint32_t u32Size,uint8_t reg)
{
    en_result_t enRet;
//    I2C_Cmd(I2C_UNIT, Enable);

    I2C_SoftwareResetCmd(I2Cx, Enable);
    I2C_SoftwareResetCmd(I2Cx, Disable);
    enRet = I2C_Start(I2Cx,I2C_TIMEOUT);
    if(Ok == enRet)
    {
        enRet = I2C_TransAddr(I2Cx, (uint8_t)u8DevAddr, ADDRESS_W, I2C_TIMEOUT);

        if(Ok == enRet)
        {
            enRet = I2C_TransData(I2Cx, (uint8_t *)&reg, 1, I2C_TIMEOUT);
            if(Ok == enRet)
            {
                enRet = I2C_Restart(I2Cx,I2C_TIMEOUT);
                if(Ok == enRet)
                {
                    if(1ul == u32Size)
                    {
                        I2C_AckConfig(I2Cx, I2C_NACK);
                    }

                    enRet = I2C_TransAddr(I2Cx, (uint8_t)u8DevAddr, ADDRESS_R, I2C_TIMEOUT);
                    if(Ok == enRet)
                    {
                        enRet = I2C_MasterReceiveAndStop(I2Cx, pu8Data, u32Size, I2C_TIMEOUT);
                    }

                    I2C_AckConfig(I2Cx, I2C_ACK);
                }

            }
        }
    }

    if(Ok != enRet)
    {
        I2C_Stop(I2Cx,I2C_TIMEOUT);
    }

//    I2C_Cmd(I2C_UNIT, Disable);
    return enRet;
}


uint8_t lsm16x_state = 0;
void lsm16x_int1_callback(void)
{
	if (Set == EXINT_GetExIntSrc(INT1_EXINT_CH))
    {
		lsm16x_state = 1;
		EXINT_ClrExIntSrc(INT1_EXINT_CH);
	}
}

void lsm16x_int1_config(void)
{
   stc_exint_init_t stcExtiConfig;
    stc_irq_signin_config_t stcIrqRegiConf;
    stc_gpio_init_t stcGpioInit;

	(void)GPIO_StructInit(&stcGpioInit);
    stcGpioInit.u16ExInt = PIN_EXINT_ON;
    stcGpioInit.u16PullUp = PIN_PU_OFF;
    (void)GPIO_Init(INT1_PORT, INT1_PIN, &stcGpioInit);
    
    stcExtiConfig.u32ExIntLvl = EXINT_TRIGGER_RISING;
	stcExtiConfig.u32ExIntFAClk = EXINT_FACLK_HCLK_DIV1;
	stcExtiConfig.u32ExIntFAE = EXINT_FILTER_A_ON;
	stcExtiConfig.u32ExIntFBTime = EXINT_FBTIM_500NS;
	stcExtiConfig.u32ExIntFBE    = EXINT_FILTER_B_OFF;
    stcExtiConfig.u32ExIntCh = INT1_EXINT_CH;
    EXINT_Init(&stcExtiConfig);
    
    stcIrqRegiConf.enIntSrc = INT1_INT_PORT_EIR;
    stcIrqRegiConf.enIRQn = INT1_INT_IRQn;
	stcIrqRegiConf.pfnCallback = &lsm16x_int1_callback;
    INTC_IrqSignIn(&stcIrqRegiConf);
    NVIC_SetPriority(stcIrqRegiConf.enIRQn, DDL_IRQ_PRIORITY_DEFAULT);
    NVIC_EnableIRQ(stcIrqRegiConf.enIRQn);
}

void lsm16x_init(void)
{
	uint8_t res = 0;
	uint8_t i = 0;
	lsm16x_iic_gpio();
	for(i = 0;i < 3;i++)
	{
		res = lsm6dsv16x_fifo_irq();
		if(res == 1)
		{
			g_gyroscope.gyroscope_type = USE_LSM6DSV16X;
			break;
		}
	}
	
	lsm16x_int1_config();
//	if(res == 1)
//	{
//		g_gyroscope.gyroscope_type = USE_LSM6DSV16X;
//	}
//	lsm6dsv16x_fifo();
}

