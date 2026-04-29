#include "bsp_power.h"
#include "hc32_ddl.h"
#include "define.h"
#include "bsp_debug.h"
#include "bsp_fan.h"
#include "bsp_iic.h"
//#include "bsp_electronic.h"

#define POWER_5V_EN1_PROT				(GPIO_PORT_B) 
#define POWER_5V_EN1_PIN      			(GPIO_PIN_11)
#define POWER_AN_DIM_EN_PR1_PROT		(GPIO_PORT_E) 
#define POWER_AN_DIM_EN_PR1_PIN      	(GPIO_PIN_14)
#define POWER_DIM_MOS_EN_PR1_PROT		(GPIO_PORT_E) 
#define POWER_DIM_MOS_EN_PR1_PIN      	(GPIO_PIN_15)
#define POWER_Surger_EN_T1_PROT			(GPIO_PORT_B) 
#define POWER_Surger_EN_T1_PIN      	(GPIO_PIN_10)

#define POWER_5V_EN2_PROT				(GPIO_PORT_E) 
#define POWER_5V_EN2_PIN      			(GPIO_PIN_13)
#define POWER_AN_DIM_EN_PR2_PROT		(GPIO_PORT_E) 
#define POWER_AN_DIM_EN_PR2_PIN      	(GPIO_PIN_10)
#define POWER_DIM_MOS_EN_PR2_PROT		(GPIO_PORT_E) 
#define POWER_DIM_MOS_EN_PR2_PIN      	(GPIO_PIN_11)
#define POWER_Surger_EN_T2_PROT			(GPIO_PORT_E) 
#define POWER_Surger_EN_T2_PIN      	(GPIO_PIN_12)

#define POWER_EN_12V_PROT				(GPIO_PORT_G) 
#define POWER_EN_12V_PIN      			(GPIO_PIN_12)
#define POWER_EN_24V_PROT				(GPIO_PORT_G) 
#define POWER_EN_24V_PIN      			(GPIO_PIN_14)

#define POWER_PG_48V_PROT				(GPIO_PORT_D) 
#define POWER_PG_48V_PIN      			(GPIO_PIN_06)
#define POWER_PG_12V_PROT				(GPIO_PORT_G) 
#define POWER_PG_12V_PIN      			(GPIO_PIN_09)
#define POWER_PG_24V_PROT				(GPIO_PORT_G) 
#define POWER_PG_24V_PIN      			(GPIO_PIN_10)

static void Power_Check_IO_Init(void)
{
	stc_gpio_init_t stcGpioInit;
	(void)GPIO_StructInit(&stcGpioInit);
	stcGpioInit.u16PinDir = PIN_DIR_IN;  //端口方向配置->输入
	stcGpioInit.u16PullUp = PIN_PU_ON; 
	
	(void)GPIO_Init(POWER_PG_48V_PROT, POWER_PG_48V_PIN, &stcGpioInit);
	(void)GPIO_Init(POWER_PG_12V_PROT, POWER_PG_12V_PIN, &stcGpioInit);
	(void)GPIO_Init(POWER_PG_24V_PROT, POWER_PG_24V_PIN, &stcGpioInit);
}

void Power_EN_IO_Init(void)
{
	stc_gpio_init_t stcGpioInit;
    (void)GPIO_StructInit(&stcGpioInit);
    stcGpioInit.u16PinDir = PIN_DIR_OUT;
	stcGpioInit.u16PullUp = PIN_PU_ON;
    stcGpioInit.u16PinDrv = PIN_DRV_HIGH;
	
    (void)GPIO_Init(POWER_5V_EN1_PROT, POWER_5V_EN1_PIN, &stcGpioInit);
	(void)GPIO_Init(POWER_AN_DIM_EN_PR1_PROT, POWER_AN_DIM_EN_PR1_PIN, &stcGpioInit);
	(void)GPIO_Init(POWER_DIM_MOS_EN_PR1_PROT, POWER_DIM_MOS_EN_PR1_PIN, &stcGpioInit);
	(void)GPIO_Init(POWER_Surger_EN_T1_PROT, POWER_Surger_EN_T1_PIN, &stcGpioInit);
	
	(void)GPIO_Init(POWER_5V_EN2_PROT, POWER_5V_EN2_PIN, &stcGpioInit);
	(void)GPIO_Init(POWER_AN_DIM_EN_PR2_PROT, POWER_AN_DIM_EN_PR2_PIN, &stcGpioInit);
	(void)GPIO_Init(POWER_DIM_MOS_EN_PR2_PROT, POWER_DIM_MOS_EN_PR2_PIN, &stcGpioInit);
	(void)GPIO_Init(POWER_Surger_EN_T2_PROT, POWER_Surger_EN_T2_PIN, &stcGpioInit);
	
	(void)GPIO_Init(POWER_EN_24V_PROT, POWER_EN_24V_PIN, &stcGpioInit);
	(void)GPIO_Init(POWER_EN_12V_PROT, POWER_EN_12V_PIN, &stcGpioInit);
}

void Power_IO_Init(void)
{
	Power_Check_IO_Init();
	Power_EN_IO_Init();
}

static void Set_Gpio_Pin_State(uint8_t u8Port, uint16_t u16Pin, uint8_t state)
{
	if(state)
		GPIO_SetPins(u8Port, u16Pin);
	GPIO_ResetPins(u8Port, u16Pin);
}

uint8_t Fan_Power_Check(void)
{
	return GPIO_ReadInputPins(POWER_PG_12V_PROT, POWER_PG_12V_PIN);
}

uint8_t Water_Power_Check(void)
{
	return GPIO_ReadInputPins(POWER_PG_24V_PROT, POWER_PG_24V_PIN);
}

uint8_t Ctrl_48V_Check(void)
{
	return GPIO_ReadInputPins(POWER_PG_48V_PROT, POWER_PG_48V_PIN);
}

/*****************************************************************************************
* Function Name: Set_Water_Enable
* Description  : 水泵电源使能
* Arguments    : 
* Return Value : 设置成功 1 / 设置失败 0 
******************************************************************************************/
uint8_t Set_Water_Enable(uint8_t state)
{
//	if(Water_Power_Check())
//		return 0;
	if(state)
		
		GPIO_SetPins(POWER_EN_24V_PROT, POWER_EN_24V_PIN);
	else
		GPIO_ResetPins(POWER_EN_24V_PROT, POWER_EN_24V_PIN);
	return 1;
}

/*****************************************************************************************
* Function Name: Set_Fan_Enable
* Description  : 风扇电源使能
* Arguments    : 
* Return Value : 设置成功 1 / 设置失败 0 
******************************************************************************************/
uint8_t Set_Fan_Enable(uint8_t state)
{
//	if(Fan_Power_Check())
//		return 0;
	if(state)
		GPIO_SetPins(POWER_EN_12V_PROT, POWER_EN_12V_PIN);
	else
		GPIO_ResetPins(POWER_EN_12V_PROT, POWER_EN_12V_PIN);
	return 1;
}

uint8_t Get_Water_Enable(void)
{
	return GPIO_ReadOutputPins(POWER_EN_24V_PROT, POWER_EN_24V_PIN);
}

uint8_t Get_Fan_Enable(void)
{
	return GPIO_ReadOutputPins(POWER_EN_12V_PROT, POWER_EN_12V_PIN);
}

uint8_t s_fan_state = 0;
uint8_t get_fan_state(void)
{
	if(s_fan_state == 1)
		return 1;
	return 0;
}


void star_fan_en(void)
{
	static uint8_t s_state;
	static uint32_t s_last_time;
//	static uint8_t s_delay_muc_reset_cnt = 0;
	switch(s_state)
	{
		case 0:
			if(2500 < g_adc_para.V_check_48V/*Ctrl_48V_Check()*/)
			{
				s_state = 1;
				s_fan_state = 0; 
				s_last_time = SysTick_GetTick();
			}
		break;
		case 1:
			if((SysTick_GetTick() - s_last_time) > 5)
			{
				s_state = 2;
			}
		break;
		case 2:
			s_last_time = SysTick_GetTick();
			s_state = 3;
		break;
		case 3:
			if((SysTick_GetTick() - s_last_time) > 10)
			{
				s_fan_state = 1;
				s_state = 4;
			}
		break;
		case 4:
			if(2500 > g_adc_para.V_check_48V/*Ctrl_48V_Check() == 0*/)
			{
//				if(s_delay_muc_reset_cnt < 2)
//				{
//					g_rs485_data.power_switch.state = 0;
//					less_than_48V_electronic_off();
//				}
//				if(++s_delay_muc_reset_cnt > 8)//延时80ms等数据发送完成
//				{
//					s_delay_muc_reset_cnt = 0;
					g_rs485_data.power_switch.state = 0;
					s_fan_state = 0;
					close_cob();
					Set_Fan_Enable(0);
					Set_Water_Enable(0);
					Fan_SetCmpVal(0);
					Water_SetCmpVal(0);
					s_state = 0;
					NVIC_SystemReset();
//				}
			}
			break;
		default:
			break;
	}
}	


/*****************************************************************************************
* Function Name: Set_Adaptive_Or_PWM_Port
* Description  : 风扇电源使能
* Arguments    : PWM 0 / 自适应 1
* Return Value : 设置成功 1 / 设置失败 0 
******************************************************************************************/
uint8_t Set_Adaptive_Or_PWM_Port(uint8_t state)
{
	Set_Gpio_Pin_State(POWER_5V_EN1_PROT, POWER_5V_EN1_PIN, state);
	Set_Gpio_Pin_State(POWER_5V_EN2_PROT, POWER_5V_EN2_PIN, state);
	if(state)
	{
		//PWM
		Set_Gpio_Pin_State(POWER_AN_DIM_EN_PR1_PROT, POWER_AN_DIM_EN_PR1_PIN, 1);
		Set_Gpio_Pin_State(POWER_DIM_MOS_EN_PR1_PROT, POWER_DIM_MOS_EN_PR1_PIN, 0);
		
		Set_Gpio_Pin_State(POWER_AN_DIM_EN_PR2_PROT, POWER_AN_DIM_EN_PR2_PIN, 1);
		Set_Gpio_Pin_State(POWER_DIM_MOS_EN_PR2_PROT, POWER_DIM_MOS_EN_PR2_PIN, 0);
		return 1;
	}
	
	Set_Gpio_Pin_State(POWER_AN_DIM_EN_PR1_PROT, POWER_AN_DIM_EN_PR1_PIN, 0);
	Set_Gpio_Pin_State(POWER_DIM_MOS_EN_PR1_PROT, POWER_DIM_MOS_EN_PR1_PIN, 1);
	
	Set_Gpio_Pin_State(POWER_AN_DIM_EN_PR2_PROT, POWER_AN_DIM_EN_PR2_PIN, 0);
	Set_Gpio_Pin_State(POWER_DIM_MOS_EN_PR2_PROT, POWER_DIM_MOS_EN_PR2_PIN, 1);
	return 1;
}





