/****************************************************************************************
**  Filename :  het_led.c
**  Abstract :  LED模块化程序，支持设置LED常亮，常灭，闪烁周期设置，闪烁次数设置等功能。
**  By       :  何建国
**  Date     :  2018-11-21
**  Changelog:1.First Create
*****************************************************************************************/

/*****************************************************************************************
									Includes
*****************************************************************************************/
#include "app_adc.h" 
#include "bsp_debug.h"
#include "bsp_power.h"
#include "bsp_init.h" 
#include "task.h"
#include "fan.h"
#include "app_task.h"
#include "app_store.h"
#include "stdio.h"
#include "app_led.h"
#include "base_data_rs485.h" 
#include "Rs485_Proto_Slave_Analysis.h"
#include "SidusProFX.h"
#include "SidusProFile.h"
#include "bsp_electronic.h"
#include "lsm6dsv16x_reg.h"
#include "inv_mpu.h"

/*****************************************************************************************
							  Variables definitions
*****************************************************************************************/

/*****************************************************************************************
							  Functions definitions
*****************************************************************************************/

/*****************************************************************************************
* Function Name: Task_Rs485
* Description  : RS485任务(1ms时间片)
* Arguments    : NONE
* Return Value : NONE
******************************************************************************************/
static void Task_Rs485(void)
{
	RS485_Recv_Finish_Ctr();
	RS485_Logic_Ctr();
	RS485_Ack_Ctr();
	Base_Data_Send_Rs485_Data();
	
}

static void tast_rs485_interrupt(void)
{

}
/*****************************************************************************************
* Function Name: Task_Main_Logic
* Description  : 主逻辑处理任务(10ms时间片)
* Arguments    : NONE
* Return Value : NONE
******************************************************************************************/
static void Task_Main_Logic(void)
{
    Polling_Here_Fx_State();
	Main_Logic_Ctr();
	
	get_adc_para_val();
	High_Temp_Ctr_Logic();
    upgrade_overtime_control();
	Upgrade_Flash_Erase();
	Upgrade_Data_Comb();
	
	star_fan_en();	
}
/*****************************************************************************************
* Function Name: Task_Store
* Description  : 存储任务(100ms时间片)
* Arguments    : NONE
* Return Value : NONE
******************************************************************************************/
static void Task_Store(void)
{
	store_data_task();
}
/*****************************************************************************************
* Function Name: Task_Fan
* Description  : 风扇处理任务(100ms时间片)
* Arguments    : NONE
* Return Value : NONE
******************************************************************************************/
static void Led_Adapt(void)
{
	Led_Adc_Adaptive_Pare();
}
/*****************************************************************************************
* Function Name: Print_Test
* Description  : 串口打印测试任务(1000ms时间片)
* Arguments    : NONE
* Return Value : NONE
******************************************************************************************/
//static void Print_Test(void)//note:任务未调用
//{
////	Adc_Printf_Test();
////	Adc_Para_Printf_Test();
//	uint8_t buff[] = "1324654879";
//	static uint32_t s_last_time, s_count;
//	if(SysTick_GetTick() - s_last_time > 1000)
//	{
//		//test_485_send();
//		s_last_time = SysTick_GetTick();		
//		s_count++;
//		//DBG("--------------run count [%d]-------------\r\n", s_count);
//		for(int i = 0;i < 20;i++)
//		{
////		 Get_ADC3_Value(i ) ;
//		//Get_ADC2_Value(i );
//		}
//	}
//	//w25qxx_test();
//}

/*****************************************************************************************
* Function Name: Task_SidusFx
* Description  : sidus fx光效任务(10ms时间片)
* Arguments    : NONE
* Return Value : NONE
******************************************************************************************/
static void Task_SidusFx(void)
{
	SidusProFX_Task();
	SidusProFile_Flash_Task();
}


extern uint8_t lsm16x_state;
static void lsm16x_task(void)
{
	if(g_gyroscope.gyroscope_type == USE_MPU6050 && g_gyroscope.gyroscope_init_state == 0 && g_error_state.gyroscope_eh_011 == 0)
	{
		MPU6050_Dmp_Init();
	}
	if(	lsm16x_state == 1	)
	{
		lsm6dsv16x_fifo_irq_handler();
		lsm16x_state = 0;
	}
	if(g_str_ota.stage <= RS485_File_TX_Step_Start)
	{
		Task_volt_mpu_init();
	}
}

void Task_Electronic_Stall_Crc_Reset(void)
{
	Electronic_Stall_Crc_Reset_Package();
}


void Task_Electronic_Recieve(void)
{
	Electronic_Para_Package();
}

void Task_Electronic_angel(void)
{
	Electronic_angel_Package();
}

void Task_Sent_Electronic_Data(void)
{
	Sent_Electronic_Data();
}

void Task_Build(void)
{
	Task_Init(TASK_STOP, 1, 1, Task_Rs485, tast_rs485_interrupt);
	Task_Init(TASK_STOP, 10, 10, Task_Main_Logic, NULL);
	Task_Init(TASK_STOP, 100, 100, Task_Store, NULL);
    Task_Init(TASK_STOP, 50, 50, Led_Adapt, NULL);  
//    Task_Init(TASK_STOP, 1000, 1000, Print_Test, NULL);   
	Task_Init(TASK_STOP, 2, 2, Task_SidusFx, NULL); //8 8
	Task_Init(TASK_STOP, 2, 2, Task_Electronic_Recieve, NULL); 
	Task_Init(TASK_STOP, 30, 30, lsm16x_task, NULL);
	Task_Init(TASK_STOP, 20, 20, Task_Electronic_angel, NULL); 
	Task_Init(TASK_STOP, 200, 200, Task_Electronic_Stall_Crc_Reset, NULL); 
	Task_Init(TASK_STOP, 20, 20, Task_Sent_Electronic_Data, NULL); 
}
/***********************************END OF FILE*******************************************/
