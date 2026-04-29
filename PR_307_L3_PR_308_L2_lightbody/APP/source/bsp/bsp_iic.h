#ifndef BSP_IIC
#define BSP_IIC

#include <stdint.h>
#include "hc32_ddl.h"

//#define DEVICE_ADDRESS                  (0x09U)

#define I2C_TIMEOUT                     (0x000F0U)
#define I2C_RET_OK                      (0U)
#define I2C_RET_ERROR                   (1U)

#define GENERATE_START                  (0x00U)
#define GENERATE_RESTART                (0x01U)

#define ADDRESS_W                       (0x00U)
#define ADDRESS_R                       (0x01U)

/* Define Write and read data length for the example */
#define TEST_DATA_LEN                   (2U)
/* Define i2c baudrate */
#define I2C_BAUDRATE                    (400000UL)

/*
the second anlog adjust light
*/
#define AN_DIM_EN_PR2_PORT  GPIO_PORT_E
#define AN_DIM_EN_PR2_PIN   GPIO_PIN_10
#define DIM_MOS_EN_PR2_PORT GPIO_PORT_E
#define DIM_MOS_EN_PR2_PIN  GPIO_PIN_11
#define V_EN2_PORT         	GPIO_PORT_E
#define V_EN2_PIN          	GPIO_PIN_13

/*
the first anlog adjust light 
*/
#define AN_DIM_EN_PR1_PORT  GPIO_PORT_E
#define AN_DIM_EN_PR1_PIN   GPIO_PIN_14
#define DIM_MOS_EN_PR1_PORT GPIO_PORT_E
#define DIM_MOS_EN_PR1_PIN  GPIO_PIN_15
#define V_EN1_PORT          GPIO_PORT_B
#define V_EN1_PIN          	GPIO_PIN_11

/*
the second pwm adjust light

*/
#define Surger_EN_T2_PORT GPIO_PORT_E
#define Surger_EN_T2_PIN  GPIO_PIN_12

/*
the first pwm adjust light
*/
#define Surger_EN_T1_PORT GPIO_PORT_B
#define Surger_EN_T1_PIN  GPIO_PIN_10

#define DAC_CHIP_ADDRESS0 0x0c  
#define DAC_CHIP_ADDRESS1 0x0d

#define DAC_CHIP_ADDRESS2 0x09
#define DAC_CHIP_ADDRESS3 0x08

#define DAC_VALUE_NUMBER 2

#define I2C_SCL1_PORT                    (GPIO_PORT_H)
#define I2C_SCL1_PIN                     (GPIO_PIN_07)
#define I2C_SDA1_PORT                    (GPIO_PORT_H)
#define I2C_SDA1_PIN                     (GPIO_PIN_08)

#define I2C_SCL2_PORT                    (GPIO_PORT_H)
#define I2C_SCL2_PIN                     (GPIO_PIN_09)
#define I2C_SDA2_PORT                    (GPIO_PORT_H)
#define I2C_SDA2_PIN                     (GPIO_PIN_10)

#define I2C_SCL3_PORT                    (GPIO_PORT_G)
#define I2C_SCL3_PIN                     (GPIO_PIN_04)
#define I2C_SDA3_PORT                    (GPIO_PORT_G)
#define I2C_SDA3_PIN                     (GPIO_PIN_03)

#define I2C_SCL4_PORT                    (GPIO_PORT_G)
#define I2C_SCL4_PIN                     (GPIO_PIN_06)
#define I2C_SDA4_PORT                    (GPIO_PORT_G)
#define I2C_SDA4_PIN                     (GPIO_PIN_05)


#if NULL
#define		Slave_Addr_Read_Cmd			(0x19)
#define		Slave_Addr_Write_Cmd		(0x18)

#define		Slave2_Addr_Read_Cmd			(0x1B)
#define		Slave2_Addr_Write_Cmd			(0x1A)

#define		Slave3_Addr_Read_Cmd			(0x13)
#define		Slave3_Addr_Write_Cmd			(0x12)

#define		Slave4_Addr_Read_Cmd			(0x11)
#define		Slave4_Addr_Write_Cmd			(0x10)
/* Define port and pin for SDA and SCL */
#define I2C_SCL_PORT                    (GPIO_PORT_G)
#define I2C_SCL_PIN                     (GPIO_PIN_06)
#define I2C_SDA_PORT                    (GPIO_PORT_G)
#define I2C_SDA_PIN                     (GPIO_PIN_05)


void DAC_IIC_Init(void);
uint8_t Device_Write(uint8_t* data, uint8_t size,uint8_t slave_address,uint8_t x);
uint8_t Device_Read(uint8_t* data, uint8_t size, uint8_t slave_address,uint8_t x);
#endif

void my_iic_gpio(void);
en_result_t Master_StartOrRestart(uint8_t u8Start,M4_I2C_TypeDef* I2Cx);
en_result_t Master_SendAdr(uint8_t u8Adr, uint8_t u8Dir,M4_I2C_TypeDef* I2Cx);
en_result_t Master_WriteData(uint8_t const pTxData[], uint32_t u32Size,M4_I2C_TypeDef* I2Cx);
en_result_t Master_ReceiveAndStop(uint8_t pRxData[], uint32_t u32Size,M4_I2C_TypeDef* I2Cx);
en_result_t Master_Stop(M4_I2C_TypeDef* I2Cx);
en_result_t Master_Initialize(M4_I2C_TypeDef* I2Cx);
void JudgeResult(en_result_t enRet);

void pwm_dac_gpio_init(void);
void pwm_mode_gpio_en(void);
void pwm_mode_Surger_EN(void);
void dac_mode_gpio_en(void);
void pwm_toggle_dac(void);
void dac_toggle_pwm(void);
void close_cob(void);
void DAC_Write_value( uint8_t device_address, uint8_t *value,M4_I2C_TypeDef* I2Cx);
void DAC_Write_High_Impence( uint8_t device_address,M4_I2C_TypeDef* I2Cx);
void write_every_line_dac(uint16_t *dac_value);
void write_dac_value(uint16_t intensity);
void set_lamp_drive_cob_dac(uint16_t dac_value,uint8_t index);
void lamp_body_dac_init(void);
void cob_pwm_toggle_dac(void);
void drive_board_power_en(void);
#endif
