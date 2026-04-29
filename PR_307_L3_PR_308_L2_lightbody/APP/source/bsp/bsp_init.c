#include "bsp_init.h"
#include "hc32_ddl.h"
#include "bsp_init.h"
#include "bsp_rs485.h"
#include "bsp_wdt.h"
#include "bsp_debug.h"
#include "bsp_fan.h"
#include "task.h"
#include "fan.h"
#include "bsp_usart.h"
#include "bsp_timer.h"
//#include "bsp_w25q256.h"
#include "lfs_flash.h"
#include "rs485.h" 
#include "SidusProFX.h"
#include "SidusProFile.h"
#include "base_data_rs485.h" 
#include "light_effect.h"
#include "light_effect_interface.h"
#include "app_led.h"
#include "app_store.h"
#include "color_mixing_dev.h"
#include "app_adc.h"
#include "bsp_flash.h"
#include "bsp_iic.h"
#include "bsp_adc.h"
#include "bsp_power.h"
#include "bsp_mpu6050.h"
#include "mpu6050.h"
#include "inv_mpu.h"
#include "bsp_electronic.h"
#include "lsm6dsv16x_reg.h"

//#define VERSION_1_GPIO_PORT    (GPIO_PORT_F)
//#define VERSION_1_PIN          (GPIO_PIN_15)

//#define VERSION_2_GPIO_PORT    (GPIO_PORT_G)
//#define VERSION_2_PIN          (GPIO_PIN_00)

//#define VERSION_3_GPIO_PORT    (GPIO_PORT_G)
//#define VERSION_3_PIN          (GPIO_PIN_01)

#define LED_PROT				(GPIO_PORT_E) 
#define LED_PIN      			(GPIO_PIN_02)

#define Ctrol1_48_PROT					(GPIO_PORT_D) 
#define Ctrol1_48_PIN      				(GPIO_PIN_15)
#define Ctrol2_48_PROT					(GPIO_PORT_D) 
#define Ctrol2_48_PIN      				(GPIO_PIN_14)

#define HARDWARE_RECISION1_PROT				(GPIO_PORT_F) 
#define HARDWARE_RECISION1_PIN      		(GPIO_PIN_05)
#define HARDWARE_RECISION2_PROT				(GPIO_PORT_G) 
#define HARDWARE_RECISION2_PIN      		(GPIO_PIN_00)
#define HARDWARE_RECISION3_PROT				(GPIO_PORT_G) 
#define HARDWARE_RECISION3_PIN      		(GPIO_PIN_01)

#define BAYONET  1 //附件
#define BRACKET  2 //支架

stc_clk_freq_t Clkdata;

static void PG_Gpio_Init(void)
{
   	stc_gpio_init_t stcGpioInit;
    (void)GPIO_StructInit(&stcGpioInit);
    stcGpioInit.u16PinDir = PIN_DIR_OUT;
    stcGpioInit.u16PinDrv = PIN_DRV_HIGH;
    (void)GPIO_Init(Ctrol1_48_PROT, Ctrol1_48_PIN, &stcGpioInit);
	(void)GPIO_Init(Ctrol2_48_PROT, Ctrol2_48_PIN, &stcGpioInit);
	GPIO_ResetPins(Ctrol1_48_PROT,Ctrol1_48_PIN);
}	

//电子卡口电压控制
static uint8_t Check_Contact1_Volt(float svolt , uint8_t Electronic_Type)
{
	 //开路3V，短路1.8V，触点接触2.4V
	//float volt = g_adc_para.V_check_contact2;
	 float volt = svolt;
	static uint8_t bracket_ret = 0;
	static uint8_t bayonet_ret = 3;
//	static uint16_t recover_cnt = 0;
	static uint16_t bracket_cnt = 0;
	static uint16_t bayonet_cnt = 0;
	static uint8_t pre_bracket_state = 0;//判断是否第一次进入对应电压范围
	static uint8_t pre_bayonet_state = 3;
	if(Electronic_Type == BRACKET)//电动支架
	{
		 if(2.8f < volt)//开路
		 {
			 if(pre_bracket_state != 3)
			 {
				 bracket_cnt = 0;
				 pre_bracket_state = 3;
			 }
			 if(bracket_ret != 3)
			 {
				 bracket_cnt++;
				 if(bracket_cnt >= 1000)
				 {
					 bracket_cnt = 0;
					 bracket_ret = 3;
				 }
			 }
			return bracket_ret;
		 }else if((2.2f < volt) && (2.6f > volt))//电子支架
		 {
			 if(pre_bracket_state != 1)
			 {
				 bracket_cnt = 0;
				 pre_bracket_state = 1;
			 }
			 if(bracket_ret != 1)
			 {
				 bracket_cnt++;
				 if(bracket_cnt >= 200)
				 {
					 bracket_cnt = 0;
					 bracket_ret = 1;
				 }
			 }
			return bracket_ret;
		 }else if((1.6f < volt) && (2.0f > volt))//短路
		 {
			 if(pre_bracket_state != 2)
			 {
				 bracket_cnt = 0;
				 pre_bracket_state = 2;
			 }
			 if(bracket_ret != 2)
			 {
				 bracket_cnt++;
				 if(bracket_cnt >= 1000)
				 {
					 bracket_cnt = 0;
					 bracket_ret = 2;
//					 g_error_state.electric_support_eh_013 = 1;
				 }
			 }
			return bracket_ret;
		 }
		 else
		 {
			 if(pre_bracket_state != 0)
			 {
				 bracket_cnt = 0;
				 pre_bracket_state = 0;
			 }
			 if(bracket_ret != 0)
			 {
				 bracket_cnt++;
				 if(bracket_cnt >= 1000)
				 {
					 bracket_cnt = 0;
					 bracket_ret = 0;
				 }
			 }
			 return bracket_ret;
		 }
	}
	 
	 //开路3V，短路1.8V，触点接触2.4V
	else if(Electronic_Type == BAYONET)//电子附件
	{
		 if(2.8f < volt)//开路
		 {
			 if(pre_bayonet_state != 3)
			 {
				 bayonet_cnt = 0;
				 pre_bayonet_state = 3;
			 }
			 if(bayonet_ret != 3)
			 {
				 bayonet_cnt++;
				 if(bayonet_cnt >= 150)
				 {
					 bayonet_cnt = 0;
					 bayonet_ret = 3;
					 if(g_error_state.lamp_with_protect_cover_eh_001 == 1)
					 {
						 g_error_state.lamp_with_protect_cover_eh_001 = 0;
					 }
				 }
			 }
			return bayonet_ret;
		 }
		 else if((1.8f < volt) && (2.2f > volt))//判断电子附件
		 {
			 if(pre_bayonet_state != 1)
			 {
				 bayonet_cnt = 0;
				 pre_bayonet_state = 1;
			 }
			 if(bayonet_ret != 1)
			 {
				 bayonet_cnt++;
				 if(bayonet_cnt >= 200 && bayonet_ret == 3)//判断上次是否属于开路
				 {
					 bayonet_cnt = 0;
					 bayonet_ret = 1;
				 }
			 }
			return bayonet_ret;
		 }
		 else if((1.2f < volt) && (1.6f > volt))//短路与保护罩使用同样的范围
		 {
			 if(pre_bayonet_state != 2)
			 {
				 bayonet_cnt = 0;
				 pre_bayonet_state = 2;
			 }
			 if(bayonet_ret != 2)
			 {
				 bayonet_cnt++;
				 if(bayonet_cnt >= 200)
				 {
					 bayonet_cnt = 0;
					 bayonet_ret = 2;
					 g_error_state.lamp_with_protect_cover_eh_001 = 1;
//					 g_error_state.electric_accessories_eh_014 = 1;
				 }
			 }
			return bayonet_ret; 
		 }
		 else
		 {
			 if(pre_bayonet_state != 0)
			 {
				 bayonet_cnt = 0;
				 pre_bayonet_state = 0;
			 }
			 if(bayonet_ret != 0)
			 {
				 bayonet_cnt++;
				 if(bayonet_cnt >= 200)//判断上次是否属于开路
				 {
					 bayonet_cnt = 0;
					 bayonet_ret = 0;
				 }
			 }
			 return bayonet_ret;
		 }
	}
	return 0;
}
void Check_Contact2_En(float volt1)
{
	static uint8_t Electronic_Bracket_Delay = 0;
	static uint8_t pin_state = 0;
	uint8_t volt_state = Check_Contact1_Volt(volt1,BRACKET);
//	if( GPIO_ReadInputPins(Ctrol1_48_PROT,Ctrol1_48_PIN) == Pin_Reset )
//	{
//		pin_state = 0;
//		Electronic_Buf.Bracket_Times = 0;
//		Electronic_Buf.Electronic_Connect &= 0xfffe;
//	}
	if(1 == volt_state && 0 == pin_state && get_fan_state() == 1 && g_rs485_data.power_switch.state == 1)
	{
		Electronic_Bracket_Delay++;
		if( Electronic_Bracket_Delay == 250 )
		{
			pin_state = 1;
			GPIO_SetPins(Ctrol1_48_PROT, Ctrol1_48_PIN);
			Electronic_Buf.Bracket_Status = 1;
			Electronic_Bracket_Delay = 0;
		}
	}
	else if(1 != volt_state && 1 == pin_state)
	{
		pin_state = 0;
		GPIO_ResetPins(Ctrol1_48_PROT, Ctrol1_48_PIN);
		Electronic_Buf.Bracket_Status = 0;
		Electronic_Bracket_Delay = 0;
		Electronic_Buf.Bracket_Times = 0;
		Electronic_Buf.Electronic_Connect &= 0xfffe;
		Electronic_Buf.Bracket_Send_State = Electronic_Idle;
		if(g_error_state.electric_support_eh_013 == 1 && volt_state == 3)//不短路情况下电子附件通讯异常后拿掉电子附件应该恢复该错误代码
		{
			g_error_state.electric_support_eh_013 = 0;
		}
	}	
}

void Check_Contact1_En(float volt1)
{
	static uint8_t Electronic_Bayonet_Delay = 0;
	static uint8_t pin_state = 0;
	static uint8_t last_bayonet_status= 0;
//	static uint16_t bayonet_error_cnt= 0;
	static uint16_t bayonet_error_reset_flag= 0;
	uint8_t volt_state = Check_Contact1_Volt(volt1,BAYONET);
//	if( GPIO_ReadInputPins(Ctrol2_48_PROT,Ctrol2_48_PIN) == Pin_Reset )
//	{
//		pin_state = 0;
//		Electronic_Buf.Bayonet_Times = 0;
//		Electronic_Buf.Electronic_Connect &= 0x01;
//	}
	if(1 == volt_state && 0 == pin_state && get_fan_state() == 1 && g_rs485_data.power_switch.state == 1)
	{
		Electronic_Bayonet_Delay++;
		if( Electronic_Bayonet_Delay == 250 )
		{
			pin_state = 1;
			Electronic_Buf.Bayonet_Times = 0;
			g_error_state.electric_accessories_eh_014 = 0;
			Electronic_Buf.Bayonet_Communicate_Error = 0;
			GPIO_SetPins(Ctrol2_48_PROT, Ctrol2_48_PIN);
			Electronic_Buf.Bayonet_Status = 1;
			Electronic_Bayonet_Delay = 0;
			bayonet_error_reset_flag = 0;
//			bayonet_error_cnt = 0;
		}
	}
	else if(1 != volt_state && 1 == pin_state)
	{
		pin_state = 0;
		GPIO_ResetPins(Ctrol2_48_PROT, Ctrol2_48_PIN);
		Electronic_Buf.Bayonet_Status = 0;
		Electronic_Bayonet_Delay = 0;
		Electronic_Buf.Bayonet_Times = 0;
		Electronic_Buf.Electronic_Connect &= 0x01;
		Electronic_Buf.Bayont_Send_State = Electronic_Idle;
		if(g_error_state.electric_accessories_eh_014 == 1 && volt_state == 3)//不短路情况下电子支架通讯异常后拿掉电子支架应该恢复该错误代码
		{
			g_error_state.electric_accessories_eh_014 = 0;
			Electronic_Buf.Bayonet_Error_Reset_Cnt = 0;
			Electronic_Buf.Bayonet_Communicate_Error= 0;
		}
	}
	 if(Electronic_Buf.Bayonet_Status != last_bayonet_status)
	 {
		g_rs485_data.new_command.light_mode = 1;/*重新启动光模式*/
		g_rs485_data.fx_restart = 1;/*光效重新启动*/
		last_bayonet_status = Electronic_Buf.Bayonet_Status;
	 }
	 if(Electronic_Buf.Bayonet_Communicate_Error == 1 && Electronic_Buf.Bayonet_Status == 1)
	 {
		 if(bayonet_error_reset_flag == 0)
		 {
			 if(Electronic_Buf.Bayonet_Error_Reset_Cnt < 3)
			 {
	//			 bayonet_error_cnt++;
	//			 if(bayonet_error_cnt >= 500)
	//			 {
	//				bayonet_error_cnt = 0;
					GPIO_ResetPins(Ctrol2_48_PROT, Ctrol2_48_PIN);
					pin_state = 0;
					Electronic_Buf.Bayonet_Error_Reset_Cnt++;
					bayonet_error_reset_flag = 1;
	//			 }
			 }
			 else
			 {
				g_error_state.electric_accessories_eh_014 = 1; 
			 }
		 }

	 }
	 if(g_error_state.electric_accessories_eh_014 == 1 && volt_state == 3)//不短路情况下电子支架通讯异常后拿掉电子支架应该恢复该错误代码
	{
		g_error_state.electric_accessories_eh_014 = 0;
		Electronic_Buf.Bayonet_Error_Reset_Cnt = 0;
		Electronic_Buf.Bayonet_Communicate_Error= 0;
	}
}


void Electronic_En(void)
{
	GPIO_SetPins(Ctrol1_48_PROT, Ctrol1_48_PIN);
	GPIO_SetPins(Ctrol2_48_PROT, Ctrol2_48_PIN);
}

/**
 * @brief  MCU Peripheral registers write unprotected.
 * @param  None
 * @retval None
 * @note Comment/uncomment each API depending on APP requires.
 */
static void Peripheral_WE(void)
{
		/* Unlock GPIO register: PSPCR, PCCR, PINAER, PCRxy, PFSRxy */
		GPIO_Unlock();
		/* Unlock PWC register: FCG0 */
		PWC_FCG0_Unlock();
		/* Unlock PWC, CLK, PVD registers, @ref PWC_REG_Write_Unlock_Code for details */
		PWC_Unlock(PWC_UNLOCK_CODE_0| PWC_UNLOCK_CODE_1 | PWC_UNLOCK_CODE_2);
		/* Unlock SRAM register: WTCR */
		SRAM_WTCR_Unlock();
		/* Unlock SRAM register: CKCR */
		// SRAM_CKCR_Unlock();
		/* Unlock all EFM registers */
		EFM_Unlock();
		/* Unlock EFM register: FWMC */
		//EFM_FWMC_Unlock();
		/* Unlock EFM OTP write protect registers */
//		EFM_OTP_WP_Unlock();
		/* Unlock all MPU registers */
		// MPU_Unlock();
}

static void bsp_clk_init(void)
{
	stc_clk_pllh_init_t stcPLLHInit;

    /* PCLK0, HCLK  Max 240MHz */
    /* PCLK1, PCLK4 Max 120MHz */
    /* PCLK2, PCLK3 Max 60MHz  */
    /* EX BUS Max 120MHz */
    CLK_ClkDiv(CLK_CATE_ALL,                                                   \
               (CLK_PCLK0_DIV1 | CLK_PCLK1_DIV2 | CLK_PCLK2_DIV4 |             \
                CLK_PCLK3_DIV4 | CLK_PCLK4_DIV2 | CLK_EXCLK_DIV2 |             \
                CLK_HCLK_DIV1));

    (void)CLK_PLLHStructInit(&stcPLLHInit);
    /* VCO = (8/1)*120 = 960MHz*/
    stcPLLHInit.u8PLLState = CLK_PLLH_ON;
    stcPLLHInit.PLLCFGR = 0UL;
    stcPLLHInit.PLLCFGR_f.PLLM = 2UL - 1UL;
    stcPLLHInit.PLLCFGR_f.PLLN = 120UL - 1UL;
    stcPLLHInit.PLLCFGR_f.PLLP = 4UL - 1UL;
    stcPLLHInit.PLLCFGR_f.PLLQ = 4UL - 1UL;
    stcPLLHInit.PLLCFGR_f.PLLR = 4UL - 1UL;
    stcPLLHInit.PLLCFGR_f.PLLSRC = CLK_PLLSRC_XTAL;
    (void)CLK_PLLHInit(&stcPLLHInit);

    /* Highspeed SRAM set to 1 Read/Write wait cycle */
    SRAM_SetWaitCycle(SRAM_SRAMH, SRAM_WAIT_CYCLE_1, SRAM_WAIT_CYCLE_1);

    /* SRAM1_2_3_4_backup set to 2 Read/Write wait cycle */
    SRAM_SetWaitCycle((SRAM_SRAM123 | SRAM_SRAM4 | SRAM_SRAMB), SRAM_WAIT_CYCLE_2, SRAM_WAIT_CYCLE_2);

    /* 0-wait @ 40MHz */
    EFM_SetWaitCycle(EFM_WAIT_CYCLE_5);

    /* 4 cycles for 200 ~ 250MHz */
    GPIO_SetReadWaitCycle(GPIO_READ_WAIT_4);

    CLK_SetSysClkSrc(CLK_SYSCLKSOURCE_PLLH);
}

static void Hardware_Revision_Init(void)
{
	stc_gpio_init_t stcGpioInit;
	(void)GPIO_StructInit(&stcGpioInit);
	stcGpioInit.u16PinDir = PIN_DIR_IN;  //端口方向配置->输入
	stcGpioInit.u16PullUp = PIN_PU_ON; 
	
	(void)GPIO_Init(HARDWARE_RECISION1_PROT, HARDWARE_RECISION1_PIN, &stcGpioInit);
	(void)GPIO_Init(HARDWARE_RECISION2_PROT, HARDWARE_RECISION2_PIN, &stcGpioInit);
	(void)GPIO_Init(HARDWARE_RECISION3_PROT, HARDWARE_RECISION3_PIN, &stcGpioInit);
}

static void LED_Init(void)
{
	stc_gpio_init_t stcGpioInit;
    (void)GPIO_StructInit(&stcGpioInit);
    stcGpioInit.u16PinDir = PIN_DIR_OUT;
    stcGpioInit.u16PinDrv = PIN_DRV_HIGH;
    (void)GPIO_Init(LED_PROT, LED_PIN, &stcGpioInit);
}

//static void Hardware_Gpio_Init(void)
//{
//	stc_gpio_init_t stc_gpio;
//	GPIO_StructInit(&stc_gpio);
//	stc_gpio.u16PullUp = PIN_PU_ON;
//	(void)GPIO_Init(VERSION_1_GPIO_PORT,VERSION_1_PIN,&stc_gpio);
//	(void)GPIO_Init(VERSION_2_GPIO_PORT,VERSION_2_PIN,&stc_gpio);
//	(void)GPIO_Init(VERSION_3_GPIO_PORT,VERSION_3_PIN,&stc_gpio);
//}

//static uint32_t HardwareVersion_Read(void)
//{
//    uint32_t version = 0x10;

//    version += GPIO_ReadInputPins(VERSION_1_GPIO_PORT, VERSION_1_PIN) ? 0x01 : 0x00;
//    version += GPIO_ReadInputPins(VERSION_2_GPIO_PORT, VERSION_2_PIN) ? 0x02 : 0x00;
//    version += GPIO_ReadInputPins(VERSION_3_GPIO_PORT, VERSION_3_PIN) ? 0x04 : 0x00;
//    return version;
//}

void BSP_init(void)
{
	Peripheral_WE();
	GPIO_SetDebugPort(GPIO_PIN_TRST,Disable);
	GPIO_SetDebugPort(GPIO_PIN_TRST,Disable);
	GPIO_SetDebugPort(GPIO_PIN_SWO,Disable);

	/* SysTick configuration */
	bsp_clk_init();
	CLK_GetClockFreq(&Clkdata);
	
	my_iic_gpio();
	pwm_dac_gpio_init();
	
	uint8_t res = 1;
	res = MPU_Init();
	if(res == 0)
	{
		mpu_MPU6050_Dmp_Init();
		g_gyroscope.gyroscope_type = USE_MPU6050;
	}
	if(g_gyroscope.gyroscope_type == USE_NULL)
	{
		lsm16x_init();
	}
	if(g_gyroscope.gyroscope_type == USE_NULL)
	{
//		g_error_state.gyroscope_eh_011 = 1;
	}
	
	Time_Init();
	
	Flash_Init();
	Adc_Driver_Init();
	Adc_init();
	
	Rs485_Msg_Init();
	rs485_slave_init();
	rs485_ctrl_pin_init();

#if (DDL_PRINT_ENABLE == DDL_ON)
	/* Initializes UART for debug printing. Baudrate is 115200. */
	Debug_Usart_Init();
	DBG("run star\r\n");
#endif /* #if (DDL_PRINT_ENABLE == DDL_ON) */
	Hardware_Revision_Init();
	LED_Init();
	PG_Gpio_Init();
	#ifdef Force_Electronic
	Electronic_En();
	#endif

#if(1 == IWDG_EN)	
	WDT_Config();
	WDT_Feed();
#endif 

	Bsp_Fan_Water_Init();
	Fan_Water_Fun_Init();
	Power_IO_Init();
	
	store_data_recover_init();

	Light_Effect_Init();
	
	/*sidus光效*/
	SidusProFX_System_Init();
	SidusProFile_System_Init();
	led_down_limit_base_data_info(1);
	Electronic_Buf.Fresnel_Temperature1 = 25;
	Electronic_Buf.Fresnel_Temperature2 = 25;


	TaskDeInit();
}

uint8_t MPU6050_Dmp_Init(void)
{
	uint8_t res = 1;
	if(g_gyroscope.gyroscope_init_state == 0 && g_error_state.gyroscope_eh_011 == 0)
	{
		res = Dmp_Load_Firmware();
		if(res) 
		{
//			g_error_state.gyroscope_eh_011 = 1;
			return 1;
		}
	}
	return 0;
//	gyro_int1_config();
}

void SysTick_IrqHandler(void)
{
    SysTick_IncTick();
	HAL_SYSTICK_Callback();
}                                 
