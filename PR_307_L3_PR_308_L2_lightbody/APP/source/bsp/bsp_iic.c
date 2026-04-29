#include "bsp_iic.h"
#include "app_led.h"
#include "math.h"
#include "bsp_timer.h"

#if software_iic
#define SCL_H(x)                (  x/2 ?((x%2)?GPIO_SetPins(GPIO_PORT_H,GPIO_PIN_07):GPIO_SetPins(GPIO_PORT_H,GPIO_PIN_09)):((x%2)?GPIO_SetPins(GPIO_PORT_G,GPIO_PIN_06):GPIO_SetPins(GPIO_PORT_G,GPIO_PIN_04)) )
#define SCL_L(x)		        (  x/2 ?((x%2)?GPIO_ResetPins(GPIO_PORT_H, GPIO_PIN_07):GPIO_ResetPins(GPIO_PORT_H, GPIO_PIN_09)):((x%2)?GPIO_ResetPins(GPIO_PORT_G,GPIO_PIN_06):GPIO_ResetPins(GPIO_PORT_G,GPIO_PIN_04)) )
#define SDA_H(x)		        (  x/2 ?((x%2)?GPIO_SetPins(GPIO_PORT_H,GPIO_PIN_08):GPIO_SetPins(GPIO_PORT_H,GPIO_PIN_10)):((x%2)?GPIO_SetPins(GPIO_PORT_G,GPIO_PIN_05):GPIO_SetPins(GPIO_PORT_G,GPIO_PIN_03)) )							
#define SDA_L(x)		        (  x/2 ?((x%2)?GPIO_ResetPins(GPIO_PORT_H, GPIO_PIN_08):GPIO_ResetPins(GPIO_PORT_H, GPIO_PIN_10)): ((x%2)?GPIO_ResetPins(GPIO_PORT_G, GPIO_PIN_05):GPIO_ResetPins(GPIO_PORT_G, GPIO_PIN_03)) )							
#define SDA_read(x)	        	(  x/2 ?( (x%2)?GPIO_ReadInputPins(GPIO_PORT_H, GPIO_PIN_08):GPIO_ReadInputPins(GPIO_PORT_H, GPIO_PIN_10)): ( (x%2)?GPIO_ReadInputPins(GPIO_PORT_G, GPIO_PIN_05):GPIO_ReadInputPins(GPIO_PORT_G, GPIO_PIN_03) ) )



#pragma push
#pragma O0
extern uint32_t SystemCoreClock;
static void i2c_delay_us(uint32_t us)
{
    volatile uint32_t Delay = us * (SystemCoreClock / 1000000U / 24U);
    do 
    {
        __nop();
    } 
    while (Delay --);    
}
#pragma pop

void DAC_IIC_Init(void)
{
	stc_gpio_init_t iic_gpio;
	GPIO_StructInit(&iic_gpio); 
	iic_gpio.u16PinDir = PIN_DIR_OUT;
	iic_gpio.u16PinOType = PIN_OTYPE_NMOS;
	iic_gpio.u16PullUp = PIN_PU_ON;
	
	GPIO_Init(GPIO_PORT_H ,GPIO_PIN_08 | GPIO_PIN_07 | GPIO_PIN_09 | GPIO_PIN_10 ,&iic_gpio);
	GPIO_SetPins(GPIO_PORT_H, GPIO_PIN_08 | GPIO_PIN_07 | GPIO_PIN_09 | GPIO_PIN_10 );
	
	GPIO_Init(GPIO_PORT_G ,GPIO_PIN_03 | GPIO_PIN_04 | GPIO_PIN_05 | GPIO_PIN_06 ,&iic_gpio);
	GPIO_SetPins(GPIO_PORT_H, GPIO_PIN_03 | GPIO_PIN_04 | GPIO_PIN_05 | GPIO_PIN_06 );
}



uint8_t DAC_I2C_Start(uint8_t x)
{
	SDA_H(x);
	i2c_delay_us(4);
	SCL_H(x);
	i2c_delay_us(4);	
	SDA_L(x);
	i2c_delay_us(4);	
	SCL_L(x);
	return 1;
}

/*******************************************************************************
* Function Name  : I2C_Stop
* Description    : Master Stop Simulation IIC Communication
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void DAC_I2C_Stop(uint8_t x)
{
	SCL_L(x);
	SDA_L(x);
	i2c_delay_us(4);
	SCL_H(x);
	i2c_delay_us(4);
	SDA_H(x);
	i2c_delay_us(4);
} 

/*******************************************************************************
* Function Name  : I2C_Ack
* Description    : Master Send Acknowledge Single
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void DAC_I2C_Ack(uint8_t x)
{	
	SCL_L(x);
	i2c_delay_us(2);
	SDA_L(x);
	i2c_delay_us(2);
	SCL_H(x);
	i2c_delay_us(2);
	SCL_L(x);
	//i2c_delay_us(2);
}   

/*******************************************************************************
* Function Name  : I2C_NoAck
* Description    : Master Send No Acknowledge Single
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void DAC_I2C_NoAck(uint8_t x)
{	
	SCL_L(x);
	i2c_delay_us(2);
	SDA_H(x);
	i2c_delay_us(2);
	SCL_H(x);
	i2c_delay_us(2);
	SCL_L(x);
	//i2c_delay_us(2);
} 

/*******************************************************************************
* Function Name  : I2C_WaitAck
* Description    : Master Reserive Slave Acknowledge Single
* Input          : None
* Output         : None
* Return         : Wheather	 Reserive Slave Acknowledge Single
*******************************************************************************/
uint8_t DAC_I2C_WaitAck(uint8_t x)
{
	uint16_t err_cnt=0;
	
	SDA_H(x);			
	i2c_delay_us(4);
	SCL_H(x);
	i2c_delay_us(4);
	while(SDA_read(x))
	{
		err_cnt ++;
		if(err_cnt > 250)
		{
			SCL_L(x);
			i2c_delay_us(2);
			return 0;
		}
		i2c_delay_us(2);
	}
	SCL_L(x);
	return 1;
}

/*******************************************************************************
* Function Name  : I2C_SendByte
* Description    : Master Send a Byte to Slave
* Input          : Will Send Date
* Output         : None
* Return         : None
*******************************************************************************/
void DAC_I2C_SendByte(uint8_t SendByte,uint8_t x)
{
	uint8_t i=8;
	while(i--)
	{
		SCL_L(x);
		i2c_delay_us(2);
		if(SendByte  &0x80)
			SDA_H(x);  
		else 
			SDA_L(x);   
		SendByte<<=1;
		i2c_delay_us(2);
		SCL_H(x);
		i2c_delay_us(2);
	}
	SCL_L(x);
}  

/*******************************************************************************
* Function Name  : I2C_RadeByte
* Description    : Master Reserive a Byte From Slave
* Input          : None
* Output         : None
* Return         : Date From Slave 
*******************************************************************************/
uint8_t DAC_I2C_RadeByte(uint8_t x)
{ 
	uint8_t i=8;
	uint8_t ReceiveByte=0;

	SDA_H(x);				
	while(i--)
	{
		ReceiveByte<<=1;      
		SCL_L(x);
		i2c_delay_us(2);
		SCL_H(x);
		i2c_delay_us(2);	
		if(SDA_read(x))
		{
			ReceiveByte |= 0x01;
		}
	}
	SCL_L(x);
	return ReceiveByte;
} 

uint8_t Device_Write(uint8_t* data, uint8_t size,uint8_t slave_address,uint8_t x)
{
    uint8_t ret = 0;
    uint8_t i = 0;
    DAC_I2C_Start(x);
    DAC_I2C_SendByte(slave_address,x);
    ret = DAC_I2C_WaitAck(x);

    for(i = 0; i < size; i++)
    {
        DAC_I2C_SendByte(*data,x); 
        ret = DAC_I2C_WaitAck(x);
        data++;
    }
    i2c_delay_us(30);	
    DAC_I2C_Stop(x);   

    return ret;    
}

uint8_t Device_Read(uint8_t* data, uint8_t size, uint8_t slave_address,uint8_t x)
{
    uint8_t i = 0;
	
	DAC_I2C_Start(x);
    DAC_I2C_SendByte(slave_address,x);
    if(!DAC_I2C_WaitAck(x))
	{
		DAC_I2C_Stop(x); 
		return 1;
	}

    for(i = 0; i < size; i++)
    {
        *data = DAC_I2C_RadeByte(x); 
		if( i < size -1 )
          DAC_I2C_Ack(x);
        data++;
    }
	DAC_I2C_NoAck(x);
    i2c_delay_us(30);	
    DAC_I2C_Stop(x);

	return 0;
}
#else

void my_iic_gpio(void)
{
	 stc_gpio_init_t stcGpioInit;
    (void)GPIO_StructInit(&stcGpioInit);
	stcGpioInit.u16PinDir = PIN_DIR_OUT;
	stcGpioInit.u16PinOType = PIN_OTYPE_NMOS;
	stcGpioInit.u16PullUp = PIN_PU_ON;
	
	(void)GPIO_Init(I2C_SCL1_PORT, I2C_SCL1_PIN, &stcGpioInit);
    (void)GPIO_Init(I2C_SDA1_PORT, I2C_SDA1_PIN, &stcGpioInit);
    GPIO_SetFunc(I2C_SCL1_PORT, I2C_SCL1_PIN, GPIO_FUNC_55_I2C5_SCL, PIN_SUBFUNC_DISABLE);
    GPIO_SetFunc(I2C_SDA1_PORT, I2C_SDA1_PIN, GPIO_FUNC_54_I2C5_SDA, PIN_SUBFUNC_DISABLE);
	
	(void)GPIO_Init(I2C_SCL2_PORT, I2C_SCL2_PIN, &stcGpioInit);
    (void)GPIO_Init(I2C_SDA2_PORT, I2C_SDA2_PIN, &stcGpioInit);
    GPIO_SetFunc(I2C_SCL2_PORT, I2C_SCL2_PIN, GPIO_FUNC_53_I2C4_SCL, PIN_SUBFUNC_DISABLE);
    GPIO_SetFunc(I2C_SDA2_PORT, I2C_SDA2_PIN, GPIO_FUNC_52_I2C4_SDA, PIN_SUBFUNC_DISABLE);
	
	
	(void)GPIO_Init(I2C_SCL3_PORT, I2C_SCL3_PIN, &stcGpioInit);
    (void)GPIO_Init(I2C_SDA3_PORT, I2C_SDA3_PIN, &stcGpioInit);
    GPIO_SetFunc(I2C_SCL3_PORT, I2C_SCL3_PIN, GPIO_FUNC_51_I2C2_SCL, PIN_SUBFUNC_DISABLE);
    GPIO_SetFunc(I2C_SDA3_PORT, I2C_SDA3_PIN, GPIO_FUNC_50_I2C2_SDA, PIN_SUBFUNC_DISABLE);
	
    (void)GPIO_Init(I2C_SCL4_PORT, I2C_SCL4_PIN, &stcGpioInit);
    (void)GPIO_Init(I2C_SDA4_PORT, I2C_SDA4_PIN, &stcGpioInit);
    GPIO_SetFunc(I2C_SCL4_PORT, I2C_SCL4_PIN, GPIO_FUNC_49_I2C1_SCL, PIN_SUBFUNC_DISABLE);
    GPIO_SetFunc(I2C_SDA4_PORT, I2C_SDA4_PIN, GPIO_FUNC_48_I2C1_SDA, PIN_SUBFUNC_DISABLE);
	

    /* Enable peripheral clock */
    PWC_Fcg1PeriphClockCmd(PWC_FCG1_I2C1, Enable);
	PWC_Fcg1PeriphClockCmd(PWC_FCG1_I2C2, Enable);
	PWC_Fcg1PeriphClockCmd(PWC_FCG1_I2C4, Enable);
	PWC_Fcg1PeriphClockCmd(PWC_FCG1_I2C5, Enable);
	
	Master_Initialize(M4_I2C1);
	Master_Initialize(M4_I2C5);
	Master_Initialize(M4_I2C4);
	Master_Initialize(M4_I2C2);
}

 en_result_t Master_StartOrRestart(uint8_t u8Start,M4_I2C_TypeDef* I2Cx)
{
    en_result_t enRet;

    /* generate start or restart signal */
    if(GENERATE_START == u8Start)
    {
        enRet = I2C_Start(I2Cx,I2C_TIMEOUT);
    }
    else
    {
        /* Clear start status flag */
        enRet = I2C_Restart(I2Cx,I2C_TIMEOUT);
    }

    return enRet;
}

/**
 * @brief   Send slave address
 * @param   [in] u8Adr  The slave address
 * @param   [in] u8Dir The transfer direction @ref I2C_Transfer_Direction
 * @retval  Process result
 *          - I2C_RET_ERROR  Send failed
 *          - I2C_RET_OK     Send success
 */
 en_result_t Master_SendAdr(uint8_t u8Adr, uint8_t u8Dir,M4_I2C_TypeDef* I2Cx)
{
   return I2C_TransAddr(I2Cx,u8Adr,u8Dir,I2C_TIMEOUT);
}

/**
 * @brief   Send data to slave
 * @param   [in]  pTxData  Pointer to the data buffer
 * @param   [in]  u32Size  Data size
 * @retval  Process result
 *          - I2C_RET_ERROR  Send failed
 *          - I2C_RET_OK     Send success
 */
 en_result_t Master_WriteData(uint8_t const pTxData[], uint32_t u32Size,M4_I2C_TypeDef* I2Cx)
{
    return I2C_TransData(I2Cx, pTxData, u32Size,I2C_TIMEOUT);
}

/**
 * @brief   Write address and receive data from slave
 * @param   [in]  pRxData  Pointer to the data buffer
 * @param   [in]  u32Size  Data size
 * @retval  Process result
 *          - I2C_RET_ERROR  Process failed
 *          - I2C_RET_OK     Process success
 */
 en_result_t Master_ReceiveAndStop(uint8_t pRxData[], uint32_t u32Size,M4_I2C_TypeDef* I2Cx)
{
    return I2C_MasterReceiveAndStop(I2Cx,pRxData, u32Size,I2C_TIMEOUT);
}

/**
 * @brief   General stop condition to slave
 * @param   None
 * @retval  Process result
 *          - I2C_RET_ERROR  Process failed
 *          - I2C_RET_OK     Process success
 */
 en_result_t Master_Stop(M4_I2C_TypeDef* I2Cx)
{
   return I2C_Stop(I2Cx,I2C_TIMEOUT);
}

/**
 * @brief   Initialize the I2C peripheral for master
 * @param   None
 * @retval  Process result
 *          - I2C_RET_ERROR  Process failed
 *          - I2C_RET_OK     Process success
 */
 en_result_t Master_Initialize(M4_I2C_TypeDef* I2Cx)
{
    stc_i2c_init_t stcI2cInit;
    float32_t fErr;

    I2C_DeInit(I2Cx);

    (void)I2C_StructInit(&stcI2cInit);
    stcI2cInit.u32Baudrate = I2C_BAUDRATE;
    stcI2cInit.u32SclTime = 5U;
    stcI2cInit.u32ClkDiv = I2C_CLK_DIV4;
    en_result_t enRet = I2C_Init(I2Cx, &stcI2cInit, &fErr);
    I2C_BusWaitCmd(I2Cx, Enable);

    if(enRet == Ok)
    {
        I2C_Cmd(I2Cx, Enable);
    }

    return enRet;
}

 void JudgeResult(en_result_t enRet)
{
    if(Ok != enRet)
    {
        for(;;)
        {
            DDL_DelayMS(500U);
        }
    }
}

void DAC_Write_value( uint8_t device_address, uint8_t *value, M4_I2C_TypeDef* I2Cx)
{
//	SET_REG32_BIT(I2Cx->CR1,I2C_CR1_SWRST);
//	CLEAR_REG32_BIT(I2Cx->CR1,I2C_CR1_SWRST);
	Master_StartOrRestart(GENERATE_START,I2Cx);

	Master_SendAdr(device_address,I2C_DIR_TX,I2Cx);

	Master_WriteData(value, DAC_VALUE_NUMBER,I2Cx);

	Master_Stop(I2Cx);
}

void DAC_Write_High_Impence( uint8_t device_address, M4_I2C_TypeDef* I2Cx)
{
	uint8_t value[2] = {0x30,0x00};
	SET_REG32_BIT(I2Cx->CR1,I2C_CR1_SWRST);
	CLEAR_REG32_BIT(I2Cx->CR1,I2C_CR1_SWRST);
	Master_StartOrRestart(GENERATE_START,I2Cx);

	Master_SendAdr(device_address,I2C_DIR_TX,I2Cx);

	Master_WriteData(value, DAC_VALUE_NUMBER,I2Cx);

	Master_Stop(I2Cx);
}

void pwm_dac_gpio_init(void)
{
	stc_gpio_init_t pwm_gpio_en;
	GPIO_StructInit(&pwm_gpio_en); 
	pwm_gpio_en.u16PinDir = PIN_DIR_OUT;
	pwm_gpio_en.u16PullUp = PIN_PU_ON;
	
	GPIO_Init(AN_DIM_EN_PR2_PORT ,AN_DIM_EN_PR2_PIN | DIM_MOS_EN_PR2_PIN | V_EN2_PIN | AN_DIM_EN_PR1_PIN | DIM_MOS_EN_PR1_PIN | Surger_EN_T2_PIN ,&pwm_gpio_en);
	GPIO_ResetPins(AN_DIM_EN_PR2_PORT, AN_DIM_EN_PR2_PIN | DIM_MOS_EN_PR2_PIN | V_EN2_PIN | AN_DIM_EN_PR1_PIN | DIM_MOS_EN_PR1_PIN | Surger_EN_T2_PIN );
	
	GPIO_Init(Surger_EN_T1_PORT ,Surger_EN_T1_PIN |  V_EN1_PIN  ,&pwm_gpio_en);	
	GPIO_ResetPins(Surger_EN_T1_PORT,  V_EN1_PIN | Surger_EN_T1_PIN );
	
}

void close_cob(void)
{
	 GPIO_ResetPins(AN_DIM_EN_PR2_PORT, AN_DIM_EN_PR2_PIN | DIM_MOS_EN_PR2_PIN | V_EN2_PIN | AN_DIM_EN_PR1_PIN | DIM_MOS_EN_PR1_PIN | Surger_EN_T2_PIN );
	 GPIO_ResetPins(Surger_EN_T1_PORT,  V_EN1_PIN | Surger_EN_T1_PIN );
}

/*the anlog adjust light by lamp that dose not use anlog en pins*/
void pwm_mode_gpio_en(void)
{
	/*1 the first step that is 5v_en set high*/
	GPIO_SetPins(V_EN2_PORT,V_EN2_PIN);//18
	GPIO_SetPins(V_EN1_PORT,V_EN1_PIN);//
	/*2 the second step that is AN_DIM_EN_PR and DIM_MOS_EN_PR set low*/
	GPIO_ResetPins(AN_DIM_EN_PR1_PORT,AN_DIM_EN_PR1_PIN | DIM_MOS_EN_PR1_PIN);
	GPIO_ResetPins(AN_DIM_EN_PR2_PORT,AN_DIM_EN_PR2_PIN | DIM_MOS_EN_PR2_PIN);
	DDL_DelayMS(2);
	/*3 the third step that is iic set the dac chip on high impence status*/	

//		GPIO_SetPins(GPIO_PORT_F, GPIO_PIN_11 | GPIO_PIN_12);
	DAC_Write_High_Impence(DAC_CHIP_ADDRESS2,M4_I2C1);
	DAC_Write_High_Impence(DAC_CHIP_ADDRESS3,M4_I2C1);
	DAC_Write_High_Impence(DAC_CHIP_ADDRESS0,M4_I2C2);
	DAC_Write_High_Impence(DAC_CHIP_ADDRESS1,M4_I2C2);
		
	DAC_Write_High_Impence(DAC_CHIP_ADDRESS2,M4_I2C4);
	DAC_Write_High_Impence(DAC_CHIP_ADDRESS3,M4_I2C4);
	DAC_Write_High_Impence(DAC_CHIP_ADDRESS0,M4_I2C5);
	DAC_Write_High_Impence(DAC_CHIP_ADDRESS1,M4_I2C5);
//		GPIO_ResetPins(GPIO_PORT_F, GPIO_PIN_11 | GPIO_PIN_12);

//	/*5 the fifth step that is surger_en set high*/
//	GPIO_SetPins(Surger_EN_T1_PORT,Surger_EN_T1_PIN);
//	GPIO_SetPins(Surger_EN_T2_PORT,Surger_EN_T2_PIN);
}

void pwm_mode_Surger_EN(void)
{
	/*5 the fifth step that is surger_en set high*/
	GPIO_SetPins(Surger_EN_T1_PORT,Surger_EN_T1_PIN);
	GPIO_SetPins(Surger_EN_T2_PORT,Surger_EN_T2_PIN);
}

void dac_mode_gpio_en(void)
{
	/*1 the first step that is 5v_en set high*/
	GPIO_SetPins(V_EN2_PORT,V_EN2_PIN);
	GPIO_SetPins(V_EN1_PORT,V_EN1_PIN);
	/*2 the second step that is AN_DIM_EN_PR and DIM_MOS_EN_PR set low*/
	GPIO_ResetPins(AN_DIM_EN_PR1_PORT,AN_DIM_EN_PR1_PIN | DIM_MOS_EN_PR1_PIN);
	GPIO_ResetPins(AN_DIM_EN_PR2_PORT,AN_DIM_EN_PR2_PIN | DIM_MOS_EN_PR2_PIN);
	/*3 the third step that is iic set the dac chip on high impence status*/
	uint8_t dac_data[2] = {0x0f,0xff};
	DDL_DelayMS(2);
	

	DAC_Write_value(DAC_CHIP_ADDRESS2,dac_data,M4_I2C1);
	DAC_Write_value(DAC_CHIP_ADDRESS3,dac_data,M4_I2C1);
	
	DAC_Write_value(DAC_CHIP_ADDRESS0,dac_data,M4_I2C2);
	DAC_Write_value(DAC_CHIP_ADDRESS1,dac_data,M4_I2C2);
	
	DAC_Write_value(DAC_CHIP_ADDRESS2,dac_data,M4_I2C4);
	DAC_Write_value(DAC_CHIP_ADDRESS3,dac_data,M4_I2C4);
	DAC_Write_value(DAC_CHIP_ADDRESS0,dac_data,M4_I2C5);
	DAC_Write_value(DAC_CHIP_ADDRESS1,dac_data,M4_I2C5);
	/*4 the fourth step that is PWM output set low*/
	
	/*5 the fifth step that is surger_en set high*/
	GPIO_SetPins(Surger_EN_T1_PORT,Surger_EN_T1_PIN);
	GPIO_SetPins(Surger_EN_T2_PORT,Surger_EN_T2_PIN);
}


void cob_pwm_toggle_dac(void)
{
	GPIO_SetPins(V_EN2_PORT,V_EN2_PIN);
	GPIO_SetPins(V_EN1_PORT,V_EN1_PIN);
	GPIO_ResetPins(Surger_EN_T1_PORT,Surger_EN_T1_PIN);
	GPIO_ResetPins(Surger_EN_T2_PORT,Surger_EN_T2_PIN);
	GPIO_SetPins(AN_DIM_EN_PR1_PORT,AN_DIM_EN_PR1_PIN | DIM_MOS_EN_PR1_PIN);
	GPIO_SetPins(AN_DIM_EN_PR2_PORT,AN_DIM_EN_PR2_PIN | DIM_MOS_EN_PR2_PIN);
}



void pwm_toggle_dac(void) //º¯ÊýÎ´µ÷ÓÃ
{
	/*1 the first step that sets pwm output low*/
	uint16_t led_pwm = 0;
	Set_Drive_Cob_Pwm_Val(&led_pwm);
	#if NULL
	/*2 the second step that sets iic on normal status*/
	uint8_t dac_data[2] = {0x00,0x00};
	DAC_Write_value(DAC_CHIP_ADDRESS2,dac_data,M4_I2C1);
	DAC_Write_value(DAC_CHIP_ADDRESS3,dac_data,M4_I2C1);
	DAC_Write_value(DAC_CHIP_ADDRESS0,dac_data,M4_I2C2);
	DAC_Write_value(DAC_CHIP_ADDRESS1,dac_data,M4_I2C2);
	
	DAC_Write_value(DAC_CHIP_ADDRESS2,dac_data,M4_I2C4);
	DAC_Write_value(DAC_CHIP_ADDRESS3,dac_data,M4_I2C4);
	DAC_Write_value(DAC_CHIP_ADDRESS0,dac_data,M4_I2C5);
	DAC_Write_value(DAC_CHIP_ADDRESS1,dac_data,M4_I2C5);
	/*3 the third step that sets pwm output high*/
	led_pwm = 11998;
	Set_Drive_Cob_Pwm_Val(&led_pwm);
	#endif
}

void dac_toggle_pwm(void)
{
	/*1 the first step that sets pwm output low*/
	uint16_t led_pwm = 0;
	Set_Drive_Cob_Pwm_Val(&led_pwm);
	/*2 the second step that iic sets on high impence status*/
	DAC_Write_High_Impence(DAC_CHIP_ADDRESS2,M4_I2C1);
	DAC_Write_High_Impence(DAC_CHIP_ADDRESS3,M4_I2C1);
	DAC_Write_High_Impence(DAC_CHIP_ADDRESS0,M4_I2C2);
	DAC_Write_High_Impence(DAC_CHIP_ADDRESS1,M4_I2C2);
	
	DAC_Write_High_Impence(DAC_CHIP_ADDRESS2,M4_I2C4);
	DAC_Write_High_Impence(DAC_CHIP_ADDRESS3,M4_I2C4);
	DAC_Write_High_Impence(DAC_CHIP_ADDRESS0,M4_I2C5);
	DAC_Write_High_Impence(DAC_CHIP_ADDRESS1,M4_I2C5);
}

#define PWM_R_PROT				(GPIO_PORT_A) 
#define PWM_R_PIN      			(GPIO_PIN_00)
#define PWM_G_PROT				(GPIO_PORT_A) 
#define PWM_G_PIN      			(GPIO_PIN_01)
#define PWM_B_PROT				(GPIO_PORT_A) 
#define PWM_B_PIN      			(GPIO_PIN_02)
#define PWM_YU_PROT				(GPIO_PORT_A) 
#define PWM_YU_PIN      		(GPIO_PIN_03)
#define PWM_WW1_PROT				(GPIO_PORT_E) 
#define PWM_WW1_PIN      			(GPIO_PIN_03)
#define PWM_WW2_PROT				(GPIO_PORT_E) 
#define PWM_WW2_PIN      			(GPIO_PIN_04)
#define PWM_CW1_PROT				(GPIO_PORT_E) 
#define PWM_CW1_PIN      			(GPIO_PIN_05)
#define PWM_CW2_PROT				(GPIO_PORT_E) 
#define PWM_CW2_PIN      			(GPIO_PIN_06)

void pwm_io_init(void)
{
	TMRA_DeInit(M4_TMRA_2);
	TMRA_DeInit(M4_TMRA_4);
	stc_gpio_init_t stcGpioInit;
    (void)GPIO_StructInit(&stcGpioInit);
    stcGpioInit.u16PinDir = PIN_DIR_OUT;
	stcGpioInit.u16PullUp = PIN_PU_ON;
    stcGpioInit.u16PinDrv = PIN_DRV_HIGH;
	
	(void)GPIO_Init(PWM_R_PROT, PWM_R_PIN, &stcGpioInit);
	(void)GPIO_Init(PWM_G_PROT, PWM_G_PIN, &stcGpioInit);
	(void)GPIO_Init(PWM_B_PROT, PWM_B_PIN, &stcGpioInit);
	(void)GPIO_Init(PWM_YU_PROT, PWM_YU_PIN, &stcGpioInit);
	(void)GPIO_Init(PWM_WW1_PROT, PWM_WW1_PIN, &stcGpioInit);
	(void)GPIO_Init(PWM_WW2_PROT, PWM_WW2_PIN, &stcGpioInit);
	(void)GPIO_Init(PWM_CW1_PROT, PWM_CW1_PIN, &stcGpioInit);
	(void)GPIO_Init(PWM_CW2_PROT, PWM_CW2_PIN, &stcGpioInit);
}



void set_pwm_io_ctr(uint8_t state)
{
	if(state)
	{
		GPIO_SetPins(PWM_R_PROT, PWM_R_PIN);
		GPIO_SetPins(PWM_G_PROT, PWM_G_PIN);
		GPIO_SetPins(PWM_B_PROT, PWM_B_PIN);
		GPIO_SetPins(PWM_YU_PROT, PWM_YU_PIN);
		GPIO_SetPins(PWM_WW1_PROT, PWM_WW1_PIN);
		GPIO_SetPins(PWM_WW2_PROT, PWM_WW2_PIN);
		GPIO_SetPins(PWM_CW1_PROT, PWM_CW1_PIN);
		GPIO_SetPins(PWM_CW2_PROT, PWM_CW2_PIN);
	}
	else
	{
		GPIO_ResetPins(PWM_R_PROT, PWM_R_PIN);
		GPIO_ResetPins(PWM_G_PROT, PWM_G_PIN);
		GPIO_ResetPins(PWM_B_PROT, PWM_B_PIN);
		GPIO_ResetPins(PWM_YU_PROT, PWM_YU_PIN);
		GPIO_ResetPins(PWM_WW1_PROT, PWM_WW1_PIN);
		GPIO_ResetPins(PWM_WW2_PROT, PWM_WW2_PIN);
		GPIO_ResetPins(PWM_CW1_PROT, PWM_CW1_PIN);
		GPIO_ResetPins(PWM_CW2_PROT, PWM_CW2_PIN);
	}
}

//void body_dac_mode_gpio_en(void)
//{
//	pwm_io_init();
//	set_pwm_io_ctr(0);
//	GPIO_SetPins(V_EN2_PORT,V_EN2_PIN);
//	GPIO_SetPins(V_EN1_PORT,V_EN1_PIN);
//	GPIO_ResetPins(AN_DIM_EN_PR1_PORT,AN_DIM_EN_PR1_PIN | DIM_MOS_EN_PR1_PIN);
//	GPIO_ResetPins(AN_DIM_EN_PR2_PORT,AN_DIM_EN_PR2_PIN | DIM_MOS_EN_PR2_PIN);
//	set_pwm_io_ctr(1);
//	GPIO_SetPins(Surger_EN_T1_PORT,Surger_EN_T1_PIN);
//	GPIO_SetPins(Surger_EN_T2_PORT,Surger_EN_T2_PIN);
//}

//void body_pwm_mode_gpio_en(void)
//{
//	pwm_io_init();
//	set_pwm_io_ctr(0);
//	Time_Init();
//	
//	//text_pin_ctr(1);
//	GPIO_ResetPins(AN_DIM_EN_PR1_PORT,AN_DIM_EN_PR1_PIN);
//	GPIO_ResetPins(AN_DIM_EN_PR2_PORT,AN_DIM_EN_PR2_PIN);
//	GPIO_ResetPins(DIM_MOS_EN_PR1_PORT, DIM_MOS_EN_PR1_PIN);
//	GPIO_ResetPins(DIM_MOS_EN_PR2_PORT, DIM_MOS_EN_PR2_PIN);

//	/*1 the first step that is 5v_en set high*/
//	GPIO_SetPins(V_EN2_PORT,V_EN2_PIN);//18
//	GPIO_SetPins(V_EN1_PORT,V_EN1_PIN);//
//	/*2 the second step that is AN_DIM_EN_PR and DIM_MOS_EN_PR set low*/
//	GPIO_ResetPins(AN_DIM_EN_PR1_PORT,AN_DIM_EN_PR1_PIN | DIM_MOS_EN_PR1_PIN);
//	GPIO_ResetPins(AN_DIM_EN_PR2_PORT,AN_DIM_EN_PR2_PIN | DIM_MOS_EN_PR2_PIN);
//	DDL_DelayMS(1);
//	/*3 the third step that is iic set the dac chip on high impence status*/	
//	DAC_Write_High_Impence(DAC_CHIP_ADDRESS2,M4_I2C1);
//	DAC_Write_High_Impence(DAC_CHIP_ADDRESS3,M4_I2C1);
//	DAC_Write_High_Impence(DAC_CHIP_ADDRESS0,M4_I2C2);
//	DAC_Write_High_Impence(DAC_CHIP_ADDRESS1,M4_I2C2);
//	
//	DAC_Write_High_Impence(DAC_CHIP_ADDRESS2,M4_I2C4);
//	DAC_Write_High_Impence(DAC_CHIP_ADDRESS3,M4_I2C4);
//	DAC_Write_High_Impence(DAC_CHIP_ADDRESS0,M4_I2C5);
//	DAC_Write_High_Impence(DAC_CHIP_ADDRESS1,M4_I2C5);
//	
//	/*4 the fourth step that is PWM output set low*/
//	uint16_t led_pwm[5] = {0};
//	Set_Drive_Cob_Pwm_Val(led_pwm);
//	/*5 the fifth step that is surger_en set high*/
//	GPIO_SetPins(Surger_EN_T1_PORT,Surger_EN_T1_PIN);
//	GPIO_SetPins(Surger_EN_T2_PORT,Surger_EN_T2_PIN);
//}

void set_drive_cob_dac_val(uint16_t* p_adc) 
{
	//DAC_Write_value(DAC_CHIP_ADDRESS2,(uint8_t *)p_adc,M4_I2C1);
//	DAC_Write_value(DAC_CHIP_ADDRESS2,dac_data,M4_I2C1);
//	DAC_Write_value(DAC_CHIP_ADDRESS3,dac_data,M4_I2C1);
//	DAC_Write_value(DAC_CHIP_ADDRESS0,dac_data,M4_I2C2);
//	DAC_Write_value(DAC_CHIP_ADDRESS1,dac_data,M4_I2C2);
//	
//	DAC_Write_value(DAC_CHIP_ADDRESS2,dac_data,M4_I2C4);
//	DAC_Write_value(DAC_CHIP_ADDRESS3,dac_data,M4_I2C4);
//	DAC_Write_value(DAC_CHIP_ADDRESS0,dac_data,M4_I2C5);
//	DAC_Write_value(DAC_CHIP_ADDRESS1,dac_data,M4_I2C5);
}

void write_dac_value(uint16_t intensity)
{
	uint16_t dac_value = 0;
	dac_value = 4095 - pow(2000, 1 - (intensity * g_power_factor.frq_change / 250.0f));
	uint8_t dac_data[2] = {0x00,0x00};
	dac_data[0] |= (dac_value>>8);
	dac_data[1] |= dac_value;
	

	//DAC_Write_value(DAC_CHIP_ADDRESS2,dac_data,M4_I2C1);//xxx
	DAC_Write_value(DAC_CHIP_ADDRESS3,dac_data,M4_I2C1);
	DAC_Write_value(DAC_CHIP_ADDRESS0,dac_data,M4_I2C2);
	DAC_Write_value(DAC_CHIP_ADDRESS1,dac_data,M4_I2C2);
//	
//	DAC_Write_value(DAC_CHIP_ADDRESS2,dac_data,M4_I2C4);//xxx
	DAC_Write_value(DAC_CHIP_ADDRESS3,dac_data,M4_I2C4);
	DAC_Write_value(DAC_CHIP_ADDRESS0,dac_data,M4_I2C5);
	DAC_Write_value(DAC_CHIP_ADDRESS1,dac_data,M4_I2C5);
}

void set_lamp_drive_cob_dac(uint16_t dac_value,uint8_t index)
{
	uint16_t dac_invert = 4095 - dac_value;
	uint8_t dac_data[2] = {0x00,0x00};
	dac_data[0] |= (dac_invert>>8);
	dac_data[1] |= dac_invert;
	switch(index)
	{	
#ifdef PR_308_L2
		case 0:DAC_Write_value(DAC_CHIP_ADDRESS0,dac_data,M4_I2C5);break;//r   308-L2 G
		case 1:DAC_Write_value(DAC_CHIP_ADDRESS1,dac_data,M4_I2C5);break;//g	 308-L2 WW3
		case 5:DAC_Write_value(DAC_CHIP_ADDRESS3,dac_data,M4_I2C4);break;//b	 308-L2 WW2
		case 3:DAC_Write_value(DAC_CHIP_ADDRESS0,dac_data,M4_I2C2);break;//ww1 308-L2 WW1
		case 4:DAC_Write_value(DAC_CHIP_ADDRESS3,dac_data,M4_I2C1);break;//cw1 308-L2 CW2
		case 2:DAC_Write_value(DAC_CHIP_ADDRESS1,dac_data,M4_I2C2);break;//ww2 308-L2 CW3
		case 6:DAC_Write_value(DAC_CHIP_ADDRESS2,dac_data,M4_I2C1);break;//cw2 308-L2 CW1
		case 7:DAC_Write_value(DAC_CHIP_ADDRESS2,dac_data,M4_I2C4);break;//yu
#endif
#ifdef PR_307_L3
		case 0:DAC_Write_value(DAC_CHIP_ADDRESS0,dac_data,M4_I2C5);break;//r   308-L2 G
		case 1:DAC_Write_value(DAC_CHIP_ADDRESS1,dac_data,M4_I2C5);break;//g	 308-L2 WW3
		case 2:DAC_Write_value(DAC_CHIP_ADDRESS3,dac_data,M4_I2C4);break;//b	 308-L2 WW2
		case 3:DAC_Write_value(DAC_CHIP_ADDRESS0,dac_data,M4_I2C2);break;//ww1 308-L2 WW1
		case 4:DAC_Write_value(DAC_CHIP_ADDRESS3,dac_data,M4_I2C1);break;//cw1 308-L2 CW2
		case 5:DAC_Write_value(DAC_CHIP_ADDRESS1,dac_data,M4_I2C2);break;//ww2 308-L2 CW3
		case 6:DAC_Write_value(DAC_CHIP_ADDRESS2,dac_data,M4_I2C1);break;//cw2 308-L2 CW1
		case 7:DAC_Write_value(DAC_CHIP_ADDRESS2,dac_data,M4_I2C4);break;//yu
#endif
	}
}

void lamp_body_dac_init(void)
{
	DAC_Write_High_Impence(DAC_CHIP_ADDRESS2,M4_I2C1);
	DAC_Write_High_Impence(DAC_CHIP_ADDRESS3,M4_I2C1);
	DAC_Write_High_Impence(DAC_CHIP_ADDRESS0,M4_I2C2);
	DAC_Write_High_Impence(DAC_CHIP_ADDRESS1,M4_I2C2);
	
	DAC_Write_High_Impence(DAC_CHIP_ADDRESS2,M4_I2C4);
	DAC_Write_High_Impence(DAC_CHIP_ADDRESS3,M4_I2C4);
	DAC_Write_High_Impence(DAC_CHIP_ADDRESS0,M4_I2C5);
	DAC_Write_High_Impence(DAC_CHIP_ADDRESS1,M4_I2C5);
}

void drive_board_power_en(void)
{
	GPIO_SetPins(V_EN2_PORT,V_EN2_PIN);
	GPIO_SetPins(V_EN1_PORT,V_EN1_PIN);
}
#endif

