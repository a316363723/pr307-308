/***********************************************************************************
                                   Include
***********************************************************************************/
#include "bsp_mpu6050.h"
/***********************************************************************************
                                Define Variate
***********************************************************************************/
#define IIC_CLK(x)      (x?GPIO_SetPins(GPIO_PORT_B,GPIO_PIN_13):GPIO_ResetPins(GPIO_PORT_B,GPIO_PIN_13))
#define IIC_DATA_OUT(x) (x?GPIO_SetPins(GPIO_PORT_B,GPIO_PIN_12):GPIO_ResetPins(GPIO_PORT_B,GPIO_PIN_12))
#define IIC_DATA_INT    (GPIO_ReadInputPins(GPIO_PORT_B, GPIO_PIN_12))
/***********************************************************************************
                                  Function
***********************************************************************************/
#define I2C_BAUDRATE                    (400000UL)

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
/**
 * @brief MCU IIC 设备初始化
 * 
 * @return uint8_t 
 */
void iic_dev_init(void)
{
//        stc_gpio_init_t stcGpioInit;
//        GPIO_StructInit(&stcGpioInit);
//        stcGpioInit.u16PullUp = PIN_PU_ON;
//        stcGpioInit.u16PinDrv = PIN_DRV_LOW;
//        stcGpioInit.u16PinDir = PIN_DIR_OUT;  //上拉输出
////        rcu_periph_clock_enable(RCU_GPIOB);
////    
////        gpio_init(GPIOB, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_6|GPIO_PIN_7);   //SCK RES SDIN
////    
//        (void)GPIO_Init(GPIO_PORT_B, GPIO_PIN_12|GPIO_PIN_13, &stcGpioInit);
//        stcGpioInit.u16PinDir = PIN_DIR_IN;  //上拉输出
//        IIC_CLK(1);
//        IIC_DATA_OUT(1);
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

static void Iic_Data_Out(void)
{
        stc_gpio_init_t stcGpioInit;
        GPIO_StructInit(&stcGpioInit);
        stcGpioInit.u16PullUp = PIN_PU_ON;
        stcGpioInit.u16PinDrv = PIN_DRV_HIGH;
        stcGpioInit.u16PinDir = PIN_DIR_OUT;  //上拉输出
    
        (void)GPIO_Init(GPIO_PORT_B, GPIO_PIN_12, &stcGpioInit);
//        rcu_periph_clock_enable(RCU_GPIOB);
//    
//        gpio_init(GPIOB, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_7);   //SCK RES SDIN
}

static void Iic_Data_Input(void)
{
        stc_gpio_init_t stcGpioInit;
        GPIO_StructInit(&stcGpioInit);
        stcGpioInit.u16PullUp = PIN_PU_ON;
        stcGpioInit.u16PinDrv = PIN_DRV_HIGH;
        stcGpioInit.u16PinDir = PIN_DIR_IN;  //上拉输入
    
        (void)GPIO_Init(GPIO_PORT_B, GPIO_PIN_12, &stcGpioInit);
//        rcu_periph_clock_enable(RCU_GPIOB);
//    
//        gpio_init(GPIOB, GPIO_MODE_IPU, GPIO_OSPEED_50MHZ, GPIO_PIN_7);   //SCK RES SDIN
}

//static void iic_DDL_DelayUS(uint32_t times)
//{
//        uint32_t i = 0;
//        uint32_t j = 0;
//    
//        for(j = 0; j < 120; j++)
//        {
//                for(i = 0; i < times; i++)
//                {
//                        __nop();
//                }
//        }
//}

void Iic_Start(void)
{
        Iic_Data_Out(); //把DIO数据线配置成输出模式，
        IIC_CLK(1);     //CLK时钟线输出高电平
        IIC_DATA_OUT(1);//DIO数据线输出高电平
        DDL_DelayUS(4);    //延时2us
        IIC_DATA_OUT(0);//DIO数据线输出低电平
        DDL_DelayUS(4);    //延时2us
        IIC_CLK(0);     //CLK时钟线输出高电平
} 

void Iic_Stop(void)
{
        Iic_Data_Out(); //把DIO数据线配置成输出模式，
        IIC_CLK(0);     //CLK时钟线输出低电平
        IIC_DATA_OUT(0);//DIO数据线输出低电平
        DDL_DelayUS(4);    //延时2us
        IIC_CLK(1);     //CLK时钟线输出高电平
        IIC_DATA_OUT(1);//DIO数据线输出高电平
        DDL_DelayUS(4);    //延时2us
}

uint8_t IIC_Wait_Ack(void)
{	
        uint16_t ucErrTime=0;
        
        Iic_Data_Input();	   //SDA设置为输入
        IIC_DATA_OUT(1);//DIO数据线输出高电平
        DDL_DelayUS(1);
        IIC_CLK(1);
        DDL_DelayUS(1);
        
        while(IIC_DATA_INT) 
        {		
                ucErrTime++;
                if(ucErrTime>250)//180us		
                {		
                    Iic_Stop();
                    return 1;
                }
        }
        IIC_CLK(0); //时钟输出0 
        return 0;
} 

//产生应答
void Iic_Ack(void)
{
        IIC_CLK(0);         //CLK时钟线输出低电平
        Iic_Data_Out();   //把DIO数据线配置成输入模式，用来读取数据
        IIC_DATA_OUT(0);//DIO数据线输出低电平
        DDL_DelayUS(2);        //延时5us
        IIC_CLK(1);         //CLK时钟线输出高电平
        DDL_DelayUS(2);        //延时2us
        IIC_CLK(0);         //CLK时钟线输出低电平
    
}


//不产生ACK应答		    
void IIC_NAck(void)
{
        IIC_CLK(0);
        Iic_Data_Out();
        IIC_DATA_OUT(1);
        DDL_DelayUS(2);
        IIC_CLK(1);
        DDL_DelayUS(2);
        IIC_CLK(0);
    
}

void Iic_Write_Byte(uint8_t byte)  
{
        uint8_t Num;                    //定义一个用来循环8次接受数据的变量用来计数循环        
    
        Iic_Data_Out();                 //把DIO数据线  
        IIC_CLK(0);           //每次写的时候CLK输出低电平     
        for(Num = 0; Num < 8; Num++)    //循环8次
        { 
                if((byte&0x80)>>7)         //高位在前   判断数据的当前位为1还是0
                {
                        IIC_DATA_OUT(1); //DIO数据线输出高电平   当前数据位为1
                }
                else
                {
                        IIC_DATA_OUT(0); //DIO数据线输出低电平   当前数据位为0
                }
                byte=byte<<1;         //每次发送完一次位数后，数据都左移一位
                DDL_DelayUS(2);          //延时3us
                IIC_CLK(1);           //每次写完时候CLK输出高电平
                DDL_DelayUS(2);          //延时3us
                IIC_CLK(0);           //每次写的时候CLK输出低电平
                DDL_DelayUS(2);          //延时3us
        }
//        IIC_DATA_OUT(0); //DIO数据线输出低电平   当前数据位为0
//        DDL_DelayUS(1);          //延时3us
        
}	    
//读1个字节，ack=1时，发送ACK，ack=0，发送nACK   
uint8_t IIC_Read_Byte(unsigned char ack)
{
        uint8_t i = 0;
        uint8_t receive=0;
    
        Iic_Data_Input();//SDA设置为输入
        for(i = 0; i < 8; i++)
        {
                IIC_CLK(0);  
                DDL_DelayUS(2);
                IIC_CLK(1); 
                receive<<=1;
                if(IIC_DATA_INT)receive++; 
                DDL_DelayUS(1);
        }
        
        if(ack == Set)
                Iic_Ack();   
        else
                IIC_NAck();
        
        return receive;
}

