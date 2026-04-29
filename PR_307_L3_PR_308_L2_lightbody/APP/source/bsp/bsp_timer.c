#include "bsp_timer.h"
#include "SidusProFX.h"
#include "light_effect.h"
#include "systick.h"
#include "app_led.h"
#include "fan.h"

#define TMR2_1_UNIT                  	  (M4_TMR2_1)
#define TMR2_1_CH                    	  (TMR2_CH_A)
#define TMR2_1_PERIP_CLK                  (PWC_FCG2_TMR2_1)

/*
 * Clock source for this example.
 * In this example:
 *   1. System clock is configured as 240MHz.
 *   2. PCLK1 is 120MHz.
 *   3. Use timer2 to count 1ms.
 *
 * A simple formula for calculating the compare value is:
 *   Tmr2CompareVal = (Tmr2Period(us) * [Tmr2ClockSource(MHz) / Tmr2ClockDiv]) - 1.
            CMP_VAL = 1000us * (120M / 64) -1
 */
#define TMR2_1_CLK_SRC                    (TMR2_CLK_SYNC_PCLK1)
#define TMR2_1_CLK_DIV                    (TMR2_CLK_DIV64)
#define TMR2_1_CMP_VAL                    (1875UL - 1U)

#define TMR2_1_INT_SRC                	  (INT_TMR2_1_CMPA)
#define TMR2_1_IRQn                   	  (Int014_IRQn)
#define TMR2_1_INT_PRIO                   (DDL_IRQ_PRIORITY_04)
#define TMR2_1_INT_TYPE               	  (TMR2_INT_CMP)  //??????????????

#if NULL
#define DX1001T_EN_PORT                  (GPIO_PORT_E)   
#define DX1001T_EN_PIN                   (GPIO_PIN_10)
#define DX1001T_EN_HIGH()                GPIO_SetPins(DX1001T_EN_PORT, DX1001T_EN_PIN)   //??
#define DX1001T_EN_LOW()                 GPIO_ResetPins(DX1001T_EN_PORT, DX1001T_EN_PIN) 
#endif

#define APP_TMRA_4_UNIT 		(M4_TMRA_4)			   // CW??PWM
#define APP_TMRA_2_UNIT 		(M4_TMRA_2)			   // CW??PWM
#define APP_TMRA_4_PERIP_CLK 	(PWC_FCG2_TMRA_4) /* PCLK(PCLK0 for unit1 ~ uint4. PCLK1=120 for unit5 ~ uint12) */
#define APP_TMRA_2_PERIP_CLK 	(PWC_FCG2_TMRA_2)

#define APP_TMRA_MODE 				(TMRA_MODE_SAWTOOTH)
#define APP_TMRA_DIR 				(TMRA_DIR_UP)
#ifdef PR_308_L2
#define APP_TMRA_PERIOD_CWW_VAL 	(9596 - 1UL) //(240M) 4000=(120M/1)/30K-1;
#endif
#ifdef PR_307_L3
#define APP_TMRA_PERIOD_CWW_VAL 	(12000UL - 1UL) //(240M) 4000=(120M/1)/30K-1;
#endif
// TIM_A_4
#define APP_TMRA_4_PWM_CW_CMP_VAL (1 - 1UL) //(120UL - 1UL)
#define APP_TMRA_4_PWM_CW_CH1 (TMRA_CH_1)
#define APP_TMRA_4_PWM_CW_CH2 (TMRA_CH_2)
#define APP_TMRA_4_PWM_CW_CH3 (TMRA_CH_3)
#define APP_TMRA_4_PWM_CW_CH4 (TMRA_CH_4)

#define APP_TIMA_4_CH1_PORT 		(GPIO_PORT_E)
#define APP_TIMA_4_CH1_PIN 			(GPIO_PIN_03)
#define APP_TMRA_4_CH1_PWM_PIN_FUNC (GPIO_FUNC_5_TIMA4_PWM1)

#define APP_TIMA_4_CH2_PORT 		(GPIO_PORT_E)
#define APP_TIMA_4_CH2_PIN 			(GPIO_PIN_04)
#define APP_TMRA_4_CH2_PWM_PIN_FUNC (GPIO_FUNC_5_TIMA4_PWM2)

#define APP_TIMA_4_CH3_PORT 		(GPIO_PORT_E)
#define APP_TIMA_4_CH3_PIN 			(GPIO_PIN_05)
#define APP_TMRA_4_CH3_PWM_PIN_FUNC (GPIO_FUNC_5_TIMA4_PWM3)

#define APP_TIMA_4_CH4_PORT 		(GPIO_PORT_E)
#define APP_TIMA_4_CH4_PIN 			(GPIO_PIN_06)
#define APP_TMRA_4_CH4_PWM_PIN_FUNC (GPIO_FUNC_5_TIMA4_PWM4)

// TIM_A_2
#define APP_TMRA_2_PWM_CW_CMP_VAL 	(1 - 1UL) // Duty=CmpVal/PeriodVal=0.5
#define APP_TMRA_2_PWM_CW_CH1 (TMRA_CH_1)
#define APP_TMRA_2_PWM_CW_CH2 (TMRA_CH_2)
#define APP_TMRA_2_PWM_CW_CH3 (TMRA_CH_3)
#define APP_TMRA_2_PWM_CW_CH4 (TMRA_CH_4)

#define APP_TIMA_2_CH1_PORT 		(GPIO_PORT_A)
#define APP_TIMA_2_CH1_PIN 			(GPIO_PIN_00)
#define APP_TMRA_2_CH1_PWM_PIN_FUNC (GPIO_FUNC_4_TIMA2_PWM1)

#define APP_TIMA_2_CH2_PORT 		(GPIO_PORT_A)
#define APP_TIMA_2_CH2_PIN 			(GPIO_PIN_01)
#define APP_TMRA_2_CH2_PWM_PIN_FUNC (GPIO_FUNC_4_TIMA2_PWM2)

#define APP_TIMA_2_CH3_PORT 		(GPIO_PORT_A)
#define APP_TIMA_2_CH3_PIN 			(GPIO_PIN_02)
#define APP_TMRA_2_CH3_PWM_PIN_FUNC (GPIO_FUNC_4_TIMA2_PWM3)

#define APP_TIMA_2_CH4_PORT 		(GPIO_PORT_A)
#define APP_TIMA_2_CH4_PIN 			(GPIO_PIN_03)
#define APP_TMRA_2_CH4_PWM_PIN_FUNC (GPIO_FUNC_4_TIMA2_PWM3)

void (*Light_Effect_CallBack)(void) = NULL;
void (*Sidus_Effect_CallBack)(void) = NULL;



/*****************************************************************************************
* Function Name: TIM*RNG_Get_RandomRange
* Description  : 获取随机数，由两个通用定时器和TICK定时器共同生成；
* Arguments    : NONE
* Return Value : NONE
******************************************************************************************/
#if NULL
static uint16_t s_random_val = 0;
uint32_t RNG_Get_RandomRange(uint32_t min,uint32_t max)
{
    uint32_t min_val = min < max ? min : max;
    uint32_t max_val = max > min ? max : min;
    uint32_t value = 0;
    s_random_val = s_random_val + HAL_GetTick();
    value = s_random_val % (max_val + 1 - min_val)+ min_val;
    return value;
}
#endif

#ifdef PR_308_L2
extern bool dmx_strobe_status;
extern uint16_t dmx_strobe_times;
extern Light_Effect g_effect_struct;
extern Cob_Slow_TypeDef g_str_cob_slow;
#endif
static void TMR2_1_Cmp_IrqCallback(void)
{
	static uint16_t FAN_Times = 0;
	if (TMR2_GetStatus(TMR2_1_UNIT, TMR2_1_CH, TMR2_FLAG_CMP) == Set)
  {
		FAN_Times++;
		TMR2_ClrStatus(TMR2_1_UNIT, TMR2_1_CH, TMR2_FLAG_CMP);	
		if( Light_Effect_CallBack != NULL )
				Light_Effect_CallBack();
		if( Sidus_Effect_CallBack != NULL )
		{
			Sidus_Effect_CallBack();
		}

		if( SidusProFX_State() == false && Light_Effect_State() == false )
			Cob_Slow_Ctr_Logic();
//		if(g_rs485_data.light_mode_state.command_1 == RS485_Cmd_Factory_RGBWW_Crc)
//			Cob_Slow_Single_Crc_Ctr_Logic();
		if( FAN_Times == 50 )//10s
		{
			FAN_Times = 0;
			Fan_Ctr_Logic();
		}
  }
}

/* Timer2-1 interrupt configuration. */
static void Tmr2_1_IrqConfig(void)
{
	stc_irq_signin_config_t stcCfg;

	stcCfg.enIntSrc = TMR2_1_INT_SRC;
	stcCfg.enIRQn = TMR2_1_IRQn;
	stcCfg.pfnCallback = &TMR2_1_Cmp_IrqCallback;
	(void)INTC_IrqSignIn(&stcCfg);

	NVIC_ClearPendingIRQ(stcCfg.enIRQn);
	NVIC_SetPriority(stcCfg.enIRQn, TMR2_1_INT_PRIO);
	NVIC_EnableIRQ(stcCfg.enIRQn);

	/* Enable the specified interrupts of Timer2. */
	TMR2_IntCmd(TMR2_1_UNIT, TMR2_1_CH, TMR2_1_INT_TYPE, Enable);
}

void Tmr2Start(void)
{
	TMR2_Start(TMR2_1_UNIT, TMR2_1_CH);
}

/* 定时器2实现 1ms 中断 */
void Timer2_1_Config(void)
{
	stc_tmr2_init_t stcInit;

	/* 1. Enable Timer2 peripheral clock. */
	PWC_Fcg2PeriphClockCmd(TMR2_1_PERIP_CLK, Enable);

	/* 2. Set a default initialization value for stcInit. */
	(void)TMR2_StructInit(&stcInit);
	stcInit.u32ClkSrc = TMR2_1_CLK_SRC;
	stcInit.u32ClkDiv = TMR2_1_CLK_DIV;
	stcInit.u32CmpVal = TMR2_1_CMP_VAL;
	(void)TMR2_Init(TMR2_1_UNIT, TMR2_1_CH, &stcInit);
	/* 3. Configures IRQ. */
	Tmr2_1_IrqConfig();
	/* 4. Start Timer2. */
	Tmr2Start();
	Sidus_Effect_CallBack = SidusProFX_1Ms_Callback;
	Light_Effect_CallBack = Effect_Per1msCallback;
}
/*
PA0	R_PWM	红灯PWM输出TIMA_2_PWM1
PA1	G_PWM	绿灯PWM输出TIMA_2_PWM2
PA2	B_PWM	蓝灯PWM输出TIMA_2_PWM3
PA3	YU_PWM	预留PWM调光TIMA_2_PWM4

PE3	WW1_PWM	TIMA_4_PWM1
PE4	WW2_PWM	TIMA_4_PWM2
PE5	CW1_PWM	TIMA_4_PWM3
PE6	CW2_PWM	TIMA_4_PWM4
*/


void TmrA_4_CW456_PWM_Config(void)
{
	stc_tmra_init_t stcInit;
	stc_tmra_pwm_cfg_t stcCfg;

	/* 1. Enable TimerA peripheral clock. */
	PWC_Fcg2PeriphClockCmd(APP_TMRA_4_PERIP_CLK, Enable);
	/* 2. Set a default initialization value for stcInit. */
	(void)TMRA_StructInit(&stcInit);

	/* 3. Modifies the initialization values depends on the application. */
	stcInit.u32PCLKDiv = TMRA_PCLK_DIV1;
	stcInit.u32CntDir = APP_TMRA_DIR;
	stcInit.u32CntMode = APP_TMRA_MODE;
	stcInit.u32PeriodVal = APP_TMRA_PERIOD_CWW_VAL;
	(void)TMRA_Init(APP_TMRA_4_UNIT, &stcInit);
	(void)TMRA_PWM_StructInit(&stcCfg);
	stcCfg.u32PeriodPolarity = TMRA_PWM_PERIOD_HIGH;
	stcCfg.u32CmpPolarity = TMRA_PWM_CMP_LOW;

	/* 4. Set the comparison reference value. */
	TMRA_SetCmpVal(APP_TMRA_4_UNIT, APP_TMRA_4_PWM_CW_CH1, APP_TMRA_4_PWM_CW_CMP_VAL);
	GPIO_SetFunc(APP_TIMA_4_CH1_PORT, APP_TIMA_4_CH1_PIN, APP_TMRA_4_CH1_PWM_PIN_FUNC, PIN_SUBFUNC_DISABLE);
	(void)TMRA_PWM_Config(APP_TMRA_4_UNIT, APP_TMRA_4_PWM_CW_CH1, &stcCfg);
	TMRA_PWM_Cmd(APP_TMRA_4_UNIT, APP_TMRA_4_PWM_CW_CH1, Enable);

	TMRA_SetCmpVal(APP_TMRA_4_UNIT, APP_TMRA_4_PWM_CW_CH2, APP_TMRA_4_PWM_CW_CMP_VAL);
	GPIO_SetFunc(APP_TIMA_4_CH2_PORT, APP_TIMA_4_CH2_PIN, APP_TMRA_4_CH2_PWM_PIN_FUNC, PIN_SUBFUNC_DISABLE);
	(void)TMRA_PWM_Config(APP_TMRA_4_UNIT, APP_TMRA_4_PWM_CW_CH2, &stcCfg);
	TMRA_PWM_Cmd(APP_TMRA_4_UNIT, APP_TMRA_4_PWM_CW_CH2, Enable);

	TMRA_SetCmpVal(APP_TMRA_4_UNIT, APP_TMRA_4_PWM_CW_CH3, APP_TMRA_4_PWM_CW_CMP_VAL);
	GPIO_SetFunc(APP_TIMA_4_CH3_PORT, APP_TIMA_4_CH3_PIN, APP_TMRA_4_CH3_PWM_PIN_FUNC, PIN_SUBFUNC_DISABLE);
	(void)TMRA_PWM_Config(APP_TMRA_4_UNIT, APP_TMRA_4_PWM_CW_CH3, &stcCfg);
	TMRA_PWM_Cmd(APP_TMRA_4_UNIT, APP_TMRA_4_PWM_CW_CH3, Enable);

	TMRA_SetCmpVal(APP_TMRA_4_UNIT, APP_TMRA_4_PWM_CW_CH4, APP_TMRA_4_PWM_CW_CMP_VAL);
	GPIO_SetFunc(APP_TIMA_4_CH4_PORT, APP_TIMA_4_CH4_PIN, APP_TMRA_4_CH4_PWM_PIN_FUNC, PIN_SUBFUNC_DISABLE);
	(void)TMRA_PWM_Config(APP_TMRA_4_UNIT, APP_TMRA_4_PWM_CW_CH4, &stcCfg);
	TMRA_PWM_Cmd(APP_TMRA_4_UNIT, APP_TMRA_4_PWM_CW_CH4, Enable);
	/* Starts TimerA. */
	TMRA_Start(APP_TMRA_4_UNIT);

	/*
	 * Stop PWM output:
	 *   TMRA_Stop(APP_TMRA_UNIT);
	 *   or
	 *   TMRA_PWM_Cmd(APP_TMRA_UNIT, APP_TMRA_PWM_x_CH, Disable);
	 */
}

void TmrA_4_CW123_PWM_Config(void)
{
	stc_tmra_init_t stcInit;
	stc_tmra_pwm_cfg_t stcCfg;

	/* 1. Enable TimerA peripheral clock. */
	PWC_Fcg2PeriphClockCmd(APP_TMRA_2_PERIP_CLK, Enable);
	/* 2. Set a default initialization value for stcInit. */
	(void)TMRA_StructInit(&stcInit);

	/* 3. Modifies the initialization values depends on the application. */
	stcInit.u32PCLKDiv = TMRA_PCLK_DIV1;
	stcInit.u32CntDir = APP_TMRA_DIR;
	stcInit.u32CntMode = APP_TMRA_MODE;
	stcInit.u32PeriodVal = APP_TMRA_PERIOD_CWW_VAL;
	(void)TMRA_Init(APP_TMRA_2_UNIT, &stcInit);
	(void)TMRA_PWM_StructInit(&stcCfg);
	stcCfg.u32PeriodPolarity = TMRA_PWM_PERIOD_HIGH;
	stcCfg.u32CmpPolarity = TMRA_PWM_CMP_LOW;

	/* 4. Set the comparison reference value. */
	TMRA_SetCmpVal(APP_TMRA_2_UNIT, APP_TMRA_2_PWM_CW_CH1, APP_TMRA_2_PWM_CW_CMP_VAL);
	GPIO_SetFunc(APP_TIMA_2_CH1_PORT, APP_TIMA_2_CH1_PIN, APP_TMRA_2_CH1_PWM_PIN_FUNC, PIN_SUBFUNC_DISABLE);
	(void)TMRA_PWM_Config(APP_TMRA_2_UNIT, APP_TMRA_2_PWM_CW_CH1, &stcCfg);
	TMRA_PWM_Cmd(APP_TMRA_2_UNIT, APP_TMRA_2_PWM_CW_CH1, Enable);

	TMRA_SetCmpVal(APP_TMRA_2_UNIT, APP_TMRA_2_PWM_CW_CH2, APP_TMRA_2_PWM_CW_CMP_VAL);
	GPIO_SetFunc(APP_TIMA_2_CH2_PORT, APP_TIMA_2_CH2_PIN, APP_TMRA_2_CH2_PWM_PIN_FUNC, PIN_SUBFUNC_DISABLE);
	(void)TMRA_PWM_Config(APP_TMRA_2_UNIT, APP_TMRA_2_PWM_CW_CH2, &stcCfg);
	TMRA_PWM_Cmd(APP_TMRA_2_UNIT, APP_TMRA_2_PWM_CW_CH2, Enable);

	TMRA_SetCmpVal(APP_TMRA_2_UNIT, APP_TMRA_2_PWM_CW_CH3, APP_TMRA_2_PWM_CW_CMP_VAL);
	GPIO_SetFunc(APP_TIMA_2_CH3_PORT, APP_TIMA_2_CH3_PIN, APP_TMRA_2_CH3_PWM_PIN_FUNC, PIN_SUBFUNC_DISABLE);
	(void)TMRA_PWM_Config(APP_TMRA_2_UNIT, APP_TMRA_2_PWM_CW_CH3, &stcCfg);
	TMRA_PWM_Cmd(APP_TMRA_2_UNIT, APP_TMRA_2_PWM_CW_CH3, Enable);

	TMRA_SetCmpVal(APP_TMRA_2_UNIT, APP_TMRA_2_PWM_CW_CH4, APP_TMRA_2_PWM_CW_CMP_VAL);
	GPIO_SetFunc(APP_TIMA_2_CH4_PORT, APP_TIMA_2_CH4_PIN, APP_TMRA_2_CH4_PWM_PIN_FUNC, PIN_SUBFUNC_DISABLE);
	(void)TMRA_PWM_Config(APP_TMRA_2_UNIT, APP_TMRA_2_PWM_CW_CH4, &stcCfg);
	TMRA_PWM_Cmd(APP_TMRA_2_UNIT, APP_TMRA_2_PWM_CW_CH4, Enable);

	/* Starts TimerA. */
	TMRA_Start(APP_TMRA_2_UNIT);

	/*
	 * Stop PWM output:
	 *   TMRA_Stop(APP_TMRA_UNIT);
	 *   or
	 *   TMRA_PWM_Cmd(APP_TMRA_UNIT, APP_TMRA_PWM_x_CH, Disable);
	 */
}




/*-------------------------------------------频闪测试----------------------------------------------------------*/
#include "define.h"

/*
Register address order ：        Write buffer order:
OCCRUH					   |	 R   TIM4_2_OUH  PA0  G
OCCRUL                     |     G   TIM4_2_OUL  PA1  W3
OCCRVH                     |     B   TIM4_2_OVH  PA2  C3
OCCRVL                     |     
OCCRWH                     |     C2  TIM4_3_OUH  PC10 C2
OCCRWL                     |     W2  TIM4_3_OUL  PD0  W2
						   |     C1  TIM4_3_OVH  PC11 C1
                           |     W1  TIM4_3_OVL  PD1  W1					   
*/
#define TIM4_PERIOD_VALUE                       (1920UL - 1UL)

#define TIM4_2_UH_R_G_PORT                      (GPIO_PORT_A)
#define TIM4_2_UH_R_G_PIN                       (GPIO_PIN_00)
#define TIM4_2_UL_G_W3_PORT                     (GPIO_PORT_A)
#define TIM4_2_UL_G_W3_PIN                      (GPIO_PIN_01)
#define TIM4_2_VH_B_C3_PORT                     (GPIO_PORT_A)
#define TIM4_2_VH_B_C3_PIN                      (GPIO_PIN_02)

#define TIM4_3_UH_C2_C2_PORT                    (GPIO_PORT_C)
#define TIM4_3_UH_C2_C2_PIN                     (GPIO_PIN_10)
#define TIM4_3_UL_W2_W2_PORT                    (GPIO_PORT_D)
#define TIM4_3_UL_W2_W2_PIN                     (GPIO_PIN_00)
#define TIM4_3_VH_C1_C1_PORT                    (GPIO_PORT_C)
#define TIM4_3_VH_C1_C1_PIN                     (GPIO_PIN_11)
#define TIM4_3_VL_W1_W1_PORT                    (GPIO_PORT_D)
#define TIM4_3_VL_W1_W1_PIN                     (GPIO_PIN_01)

#define TIM4_2_UH_R_G_CH                        (TMR4_OCO_UH)
#define TIM4_2_UL_G_W3_CH                       (TMR4_OCO_UL)
#define TIM4_2_VH_B_C3_CH                       (TMR4_OCO_VH)

#define TIM4_3_UH_C2_C2_CH                      (TMR4_OCO_UH)
#define TIM4_3_UL_W2_W2_CH                      (TMR4_OCO_UL)
#define TIM4_3_VH_C1_C1_CH                      (TMR4_OCO_VH)
#define TIM4_3_VL_W1_W1_CH                      (TMR4_OCO_VL)

#define TIM4_U_GROUP                            (TMR4_PWM_U)
#define TIM4_V_GROUP                            (TMR4_PWM_V)

#define TMR4_2_UNIT                 	        (M4_TMR4_2)
#define TMR4_3_UNIT                             (M4_TMR4_3)

#define TMR4_2_PERIP_CLK                 		(PWC_FCG2_TMR4_2) 
#define TMR4_3_PERIP_CLK 						(PWC_FCG2_TMR4_3)

/*------------------------------DMA-----------------------------*/
#define DMA_TMR4_UINT       					(M4_DMA2)        
#define DMA_TMR4_PERIP_CLK                      (PWC_FCG0_DMA2)

#define DMA_TMR4_2_CH     						(DMA_CH0)
#define DMA_TMR4_3_CH     						(DMA_CH1)

#define DMA_TMR4_2_EVT_SRC                      (EVT_TMR4_2_SCMUH)
#define DMA_TMR4_3_EVT_SRC                      (EVT_TMR4_3_SCMUH)

const uint16_t duty[4][3] = {
    1, 17, 28,  
    1, 16, 28,  
    1, 16, 28,  
    1, 16, 27, 
};
const uint16_t dutyy[4][4] = {
    2999,2999,2999,2999,  
    2998,2998,2998,2998,  
    2997,2997,2997,2997,  
    2996,2996,2996,2996 
};

static void Dma_Timer4_2_init(void)
{
    stc_dma_init_t stcDmaInit;
    stc_dma_rpt_init_t stcDmaRepeatInit;
    stc_dma_nonseq_init_t stcDmaNonSeqInit;

	PWC_Fcg0PeriphClockCmd((DMA_TMR4_PERIP_CLK | PWC_FCG0_AOS), Enable);
    
	(void)DMA_StructInit(&stcDmaInit);
    stcDmaInit.u32IntEn      = DMA_INT_ENABLE;
    stcDmaInit.u32BlockSize  = 3U;//每组3个数据
    stcDmaInit.u32TransCnt   = 0U;//无限循环
    stcDmaInit.u32DataWidth  = DMA_DATAWIDTH_16BIT;
    stcDmaInit.u32DestAddr   = (uint32_t)(&M4_TMR4_2->OCCRUH);
    stcDmaInit.u32SrcAddr    = (uint32_t)(&g_led_pwm_arg.pwm_buffer_one[0][0]);//g_led_pwm_arg.led_channel_buf.pwm_buffer_one
    stcDmaInit.u32SrcInc = DMA_SRC_ADDR_INC;
    stcDmaInit.u32DestInc = DMA_DEST_ADDR_INC;
    (void)DMA_Init(DMA_TMR4_UINT, DMA_TMR4_2_CH, &stcDmaInit);

    (void)DMA_RepeatStructInit(&stcDmaRepeatInit);
    stcDmaRepeatInit.u32DestRptEn = DMA_DEST_RPT_ENABLE;
	stcDmaRepeatInit.u32SrcRptEn = DMA_SRC_RPT_ENABLE;
    stcDmaRepeatInit.u32SrcRptSize = 15U;//4组，共12个数据 以24为单位重载地址传输
    stcDmaRepeatInit.u32DestRptSize = 3U; //定时器3个通道  以6为单位重载地址传输
    (void)DMA_RepeatInit(DMA_TMR4_UINT, DMA_TMR4_2_CH, &stcDmaRepeatInit);
    
    (void)DMA_NonSeqStructInit(&stcDmaNonSeqInit);
    stcDmaNonSeqInit.u32DestNonSeqEn = DMA_DEST_NS_ENABLE;
    stcDmaNonSeqInit.u32DestNonSeqOfs = 2U;
    stcDmaNonSeqInit.u32DestNonSeqCnt  = 1U;
    (void)DMA_NonSeqInit(DMA_TMR4_UINT, DMA_TMR4_2_CH, &stcDmaNonSeqInit);
    
    DMA_SetTriggerSrc(DMA_TMR4_UINT, DMA_TMR4_2_CH, DMA_TMR4_2_EVT_SRC);//作为第一个通道触发开始加载
    DMA_Cmd(DMA_TMR4_UINT,Enable);
    DMA_ChannelCmd(DMA_TMR4_UINT, DMA_TMR4_2_CH, Enable);
    
}

static void Dma_Timer4_3_init(void)
{
    stc_dma_init_t stcDmaInit;
    stc_dma_rpt_init_t stcDmaRepeatInit;
    stc_dma_nonseq_init_t stcDmaNonSeqInit;
    
	PWC_Fcg0PeriphClockCmd((DMA_TMR4_PERIP_CLK | PWC_FCG0_AOS), Enable);

    (void)DMA_StructInit(&stcDmaInit);
    stcDmaInit.u32IntEn      = DMA_INT_ENABLE;
    stcDmaInit.u32BlockSize  = 4U;//每组6个数据
    stcDmaInit.u32TransCnt   = 0U;//无限循环
    stcDmaInit.u32DataWidth  = DMA_DATAWIDTH_16BIT;
    stcDmaInit.u32DestAddr   = (uint32_t)(&M4_TMR4_3->OCCRUH);
    stcDmaInit.u32SrcAddr    = (uint32_t)(&g_led_pwm_arg.pwm_buffer_two[0][0]);
    stcDmaInit.u32SrcInc = DMA_SRC_ADDR_INC;
    stcDmaInit.u32DestInc = DMA_DEST_ADDR_INC;
    (void)DMA_Init(DMA_TMR4_UINT, DMA_TMR4_3_CH, &stcDmaInit);

    (void)DMA_RepeatStructInit(&stcDmaRepeatInit);
    stcDmaRepeatInit.u32DestRptEn = DMA_DEST_RPT_ENABLE;
	stcDmaRepeatInit.u32SrcRptEn = DMA_SRC_RPT_ENABLE; 
    stcDmaRepeatInit.u32SrcRptSize = 20U;//4组，共24个数据 以24为单位重载地址传输
    stcDmaRepeatInit.u32DestRptSize = 4U; //定时器6个通道  以6为单位重载地址传输
    (void)DMA_RepeatInit(DMA_TMR4_UINT, DMA_TMR4_3_CH, &stcDmaRepeatInit);
    
    (void)DMA_NonSeqStructInit(&stcDmaNonSeqInit);
    stcDmaNonSeqInit.u32DestNonSeqEn = DMA_DEST_NS_ENABLE;
    stcDmaNonSeqInit.u32DestNonSeqOfs = 2U;
    stcDmaNonSeqInit.u32DestNonSeqCnt  = 1U;
    (void)DMA_NonSeqInit(DMA_TMR4_UINT, DMA_TMR4_3_CH, &stcDmaNonSeqInit);
    
    DMA_SetTriggerSrc(DMA_TMR4_UINT, DMA_TMR4_3_CH, DMA_TMR4_3_EVT_SRC);//作为第一个通道触发开始加载
    DMA_Cmd(DMA_TMR4_UINT,Enable);
    DMA_ChannelCmd(DMA_TMR4_UINT, DMA_TMR4_3_CH, Enable);
    
}
static void Timer4_2_Config()
{
	stc_tmr4_cnt_init_t stcTmr4Init;
    stc_tmr4_oco_init_t stcTmr4OcInit;
    stc_tmr4_pwm_init_t stcTmr4PwmInit;
    stc_oco_high_ch_compare_mode_t unTmr4OcOcmrh;
    stc_oco_low_ch_compare_mode_t unTmr4OcOcmrl;
    stc_tmr4_sevt_init_t stcTmr4EventInit;
	
	Dma_Timer4_2_init();
	
	PWC_Fcg2PeriphClockCmd(TMR4_2_PERIP_CLK, Enable);
	
	GPIO_SetFunc(TIM4_2_UH_R_G_PORT, TIM4_2_UH_R_G_PIN, GPIO_FUNC_2, PIN_SUBFUNC_DISABLE);
    GPIO_SetFunc(TIM4_2_UL_G_W3_PORT, TIM4_2_UL_G_W3_PIN, GPIO_FUNC_2, PIN_SUBFUNC_DISABLE);
    GPIO_SetFunc(TIM4_2_VH_B_C3_PORT, TIM4_2_VH_B_C3_PIN, GPIO_FUNC_2, PIN_SUBFUNC_DISABLE);
	
    /************************* Configure TMR4 counter *************************/
    /* TMR4 counter: initialize */
	(void)TMR4_CNT_StructInit(&stcTmr4Init);
	stcTmr4Init.u16CntMode = TMR4_CNT_MODE_SAWTOOTH_WAVE;
	stcTmr4Init.u16PclkDiv = TMR4_CNT_PCLK_DIV1;
	stcTmr4Init.u16CycleVal = TIM4_PERIOD_VALUE;
	TMR4_CNT_Init(TMR4_2_UNIT, &stcTmr4Init);
	
	/************************* Configure TMR4 output-compare ******************/
    /* TMR4 OC channel: initialize TMR4 structure */
    (void)TMR4_OCO_StructInit(&stcTmr4OcInit);
//  stcTmr4OcInit.u16CompareVal = 0x0000;
	stcTmr4OcInit.u16OccrLinkTransfer = TMR4_OCO_OCCR_LINK_TRANSFER_ENABLE;
	stcTmr4OcInit.u16OccrBufMode = TMR4_OCO_OCCR_BUF_CNT_PEAK;
	stcTmr4OcInit.u16OcmrBufMode = TMR4_OCO_OCMR_BUF_CNT_PEAK;
	stcTmr4OcInit.u16OcoInvalidOp = TMR4_OCO_INVAILD_OP_LOW;
	stcTmr4OcInit.u16CompareVal = 0x0000;
	
    /* TMR4 OC channel: initialize channel */
    (void)TMR4_OCO_Init(TMR4_2_UNIT, TIM4_2_UH_R_G_CH , &stcTmr4OcInit);
    (void)TMR4_OCO_Init(TMR4_2_UNIT, TIM4_2_UL_G_W3_CH, &stcTmr4OcInit);
    (void)TMR4_OCO_Init(TMR4_2_UNIT, TIM4_2_VH_B_C3_CH, &stcTmr4OcInit);
    
    /* TMR4 OC high channel: compare mode OCMR[15:0] = 0x0FFF = b 0000 1111 1111 1111 */
    unTmr4OcOcmrh.OCMRx_f.OCFDCH = TMR4_OCO_OCF_SET; /* bit[0]     1  */
    unTmr4OcOcmrh.OCMRx_f.OCFPKH = TMR4_OCO_OCF_SET; /* bit[1]     1  */
    unTmr4OcOcmrh.OCMRx_f.OCFUCH = TMR4_OCO_OCF_SET; /* bit[2]     1  */
    unTmr4OcOcmrh.OCMRx_f.OCFZRH = TMR4_OCO_OCF_SET; /* bit[3]     1  */
    unTmr4OcOcmrh.OCMRx_f.OPDCH  = TMR4_OCO_OP_HOLD;    /* Bit[5:4]   11 */
    unTmr4OcOcmrh.OCMRx_f.OPPKH  = TMR4_OCO_OP_HIGH;    /* Bit[7:6]   11 */
    unTmr4OcOcmrh.OCMRx_f.OPUCH  = TMR4_OCO_OP_LOW;    /* Bit[9:8]   11 */
    unTmr4OcOcmrh.OCMRx_f.OPZRH  = TMR4_OCO_OP_LOW;    /* Bit[11:10] 11 */
    unTmr4OcOcmrh.OCMRx_f.OPNPKH = TMR4_OCO_OP_LOW;    /* Bit[13:12] 00 */
    unTmr4OcOcmrh.OCMRx_f.OPNZRH = TMR4_OCO_OP_HIGH;    /* Bit[15:14] 00 */
	unTmr4OcOcmrh.u16ExtendMatch = TMR4_OCO_EXTEND_MATCH_DISABLE;
    TMR4_OCO_SetHighChCompareMode(TMR4_2_UNIT, TIM4_2_UH_R_G_CH, &unTmr4OcOcmrh);
    TMR4_OCO_SetHighChCompareMode(TMR4_2_UNIT, TIM4_2_VH_B_C3_CH, &unTmr4OcOcmrh);

    /* TMR4 OC low channel: compare mode OCMR[31:0] 0x0FF0 0FFF = b 0000 1111 1111 0000   0000 1111 1111 1111 */
    unTmr4OcOcmrl.OCMRx_f.OCFDCL  = TMR4_OCO_OCF_SET; /* bit[0]     1  */
    unTmr4OcOcmrl.OCMRx_f.OCFPKL  = TMR4_OCO_OCF_SET; /* bit[1]     1  */
    unTmr4OcOcmrl.OCMRx_f.OCFUCL  = TMR4_OCO_OCF_SET; /* bit[2]     1  */
    unTmr4OcOcmrl.OCMRx_f.OCFZRL  = TMR4_OCO_OCF_SET; /* bit[3]     1  */
    unTmr4OcOcmrl.OCMRx_f.OPDCL   = TMR4_OCO_OP_HOLD;    /* bit[5:4]   11 */
    unTmr4OcOcmrl.OCMRx_f.OPPKL   = TMR4_OCO_OP_HIGH;    /* bit[7:6]   11 */
    unTmr4OcOcmrl.OCMRx_f.OPUCL   = TMR4_OCO_OP_LOW;    /* bit[9:8]   11 */
    unTmr4OcOcmrl.OCMRx_f.OPZRL   = TMR4_OCO_OP_LOW;    /* bit[11:10] 11 */
    unTmr4OcOcmrl.OCMRx_f.OPNPKL  = TMR4_OCO_OP_LOW;    /* bit[13:12] 00 */
    unTmr4OcOcmrl.OCMRx_f.OPNZRL  = TMR4_OCO_OP_HIGH;    /* bit[15:14] 00 */
    unTmr4OcOcmrl.OCMRx_f.EOPNDCL = TMR4_OCO_OP_HOLD;    /* bit[17:16] 00 */
    unTmr4OcOcmrl.OCMRx_f.EOPNUCL = TMR4_OCO_OP_HOLD;    /* bit[19:18] 00 */
    unTmr4OcOcmrl.OCMRx_f.EOPDCL  = TMR4_OCO_OP_HOLD;    /* bit[21:20] 11 */
    unTmr4OcOcmrl.OCMRx_f.EOPPKL  = TMR4_OCO_OP_HIGH;    /* bit[23:22] 11 */
    unTmr4OcOcmrl.OCMRx_f.EOPUCL  = TMR4_OCO_OP_LOW;    /* bit[25:24] 11 */
    unTmr4OcOcmrl.OCMRx_f.EOPZRL  = TMR4_OCO_OP_LOW;    /* bit[27:26] 11 */
    unTmr4OcOcmrl.OCMRx_f.EOPNPKL = TMR4_OCO_OP_LOW;    /* bit[29:28] 00 */
    unTmr4OcOcmrl.OCMRx_f.EOPNZRL = TMR4_OCO_OP_HIGH;    /* bit[31:30] 00 */
	unTmr4OcOcmrl.u16ExtendMatch = TMR4_OCO_EXTEND_MATCH_DISABLE;
    TMR4_OCO_SetLowChCompareMode(TMR4_2_UNIT, TIM4_2_UL_G_W3_CH, &unTmr4OcOcmrl);
	
	/* TMR4 OC: enable */
    TMR4_OCO_SetOutputCompare(TMR4_2_UNIT, TIM4_2_UH_R_G_CH , TMR4_OCO_DISABLE);
    TMR4_OCO_SetOutputCompare(TMR4_2_UNIT, TIM4_2_UL_G_W3_CH, TMR4_OCO_DISABLE);
    TMR4_OCO_SetOutputCompare(TMR4_2_UNIT, TIM4_2_VH_B_C3_CH, TMR4_OCO_DISABLE);
	
    /************************* Configure TMR4 PWM && TMR4 EVT*****************************/

    /* TMR4 PWM: initialize */
    (void)TMR4_PWM_StructInit(&stcTmr4PwmInit);
    stcTmr4PwmInit.u16Mode = TMR4_PWM_THROUGH_MODE;//直通模式
	stcTmr4PwmInit.u32OxHPortOutMode = TMR4_PWM_PORT_OUTPUT_NORMAL;
	stcTmr4PwmInit.u32OxLPortOutMode = TMR4_PWM_PORT_OUTPUT_NORMAL;//OExy使能输出
	stcTmr4PwmInit.u32EmbOxHPortState = TMR4_PWM_EMB_PORT_OUTPUT_NORMAL;
	stcTmr4PwmInit.u32EmbOxLPortState = TMR4_PWM_EMB_PORT_OUTPUT_NORMAL;
    (void)TMR4_PWM_Init(TMR4_2_UNIT, TIM4_U_GROUP , &stcTmr4PwmInit);
    (void)TMR4_PWM_Init(TMR4_2_UNIT, TIM4_V_GROUP, &stcTmr4PwmInit);  
    
    (void)TMR4_SEVT_StructInit(&stcTmr4EventInit);
	stcTmr4EventInit.u16UpMatchCmd = TMR4_SEVT_UP_ENABLE;
	stcTmr4EventInit.u16DownMatchCmd = TMR4_SEVT_DOWN_ENABLE;
	stcTmr4EventInit.u16PeakMatchCmd = TMR4_SEVT_PEAK_ENABLE;
	stcTmr4EventInit.u16ZeroMatchCmd = TMR4_SEVT_ZERO_ENABLE;
    stcTmr4EventInit.u16CompareVal = TIM4_PERIOD_VALUE;
    (void)TMR4_SEVT_Init(TMR4_2_UNIT, TIM4_2_UH_R_G_CH, &stcTmr4EventInit);
//    /* TMR4 event: set output event signal */
//    TMR4_SEVT_SetPortOutputEventSig(TMR4_2_UNIT, TMR4_SEVT_PORT_OUTPUT_EVENT0_SIGNAL);
//    
//    /* TMR4 PWM: enable main output  */
//    TMR4_PWM_MasterOutputCmd(TMR4_2_UNIT, Enable);//MOE使能输出
    /* Start TMR4 count. */
    TMR4_CNT_Start(TMR4_2_UNIT);
}

static void Timer4_3_Config()
{
	stc_tmr4_cnt_init_t stcTmr4Init;
	stc_tmr4_oco_init_t stcTmr4OcInit;
	stc_tmr4_pwm_init_t stcTmr4PwmInit;
    stc_oco_high_ch_compare_mode_t unTmr4OcOcmrh;
    stc_oco_low_ch_compare_mode_t unTmr4OcOcmrl;
    stc_tmr4_sevt_init_t stcTmr4EventInit;
	
	Dma_Timer4_3_init();
	
	PWC_Fcg2PeriphClockCmd(TMR4_3_PERIP_CLK, Enable);
	
	GPIO_SetFunc(TIM4_3_UH_C2_C2_PORT, TIM4_3_UH_C2_C2_PIN, GPIO_FUNC_2, PIN_SUBFUNC_DISABLE);
    GPIO_SetFunc(TIM4_3_UL_W2_W2_PORT, TIM4_3_UL_W2_W2_PIN, GPIO_FUNC_2, PIN_SUBFUNC_DISABLE);
    GPIO_SetFunc(TIM4_3_VH_C1_C1_PORT, TIM4_3_VH_C1_C1_PIN, GPIO_FUNC_2, PIN_SUBFUNC_DISABLE);
    GPIO_SetFunc(TIM4_3_VL_W1_W1_PORT, TIM4_3_VL_W1_W1_PIN, GPIO_FUNC_2, PIN_SUBFUNC_DISABLE);

    /************************* Configure TMR4 counter *************************/
    /* TMR4 counter: initialize */
	(void)TMR4_CNT_StructInit(&stcTmr4Init);
	stcTmr4Init.u16CntMode = TMR4_CNT_MODE_SAWTOOTH_WAVE;
	stcTmr4Init.u16PclkDiv = TMR4_CNT_PCLK_DIV1;
	stcTmr4Init.u16CycleVal = TIM4_PERIOD_VALUE;
	TMR4_CNT_Init(TMR4_3_UNIT, &stcTmr4Init);

	/************************* Configure TMR4 output-compare ******************/
    /* TMR4 OC channel: initialize TMR4 structure */
    (void)TMR4_OCO_StructInit(&stcTmr4OcInit);
//    stcTmr4OcInit.u16CompareVal = 0x0000;
	stcTmr4OcInit.u16OccrLinkTransfer = TMR4_OCO_OCCR_LINK_TRANSFER_ENABLE;
	stcTmr4OcInit.u16OccrBufMode = TMR4_OCO_OCCR_BUF_CNT_PEAK;
	stcTmr4OcInit.u16OcmrBufMode = TMR4_OCO_OCMR_BUF_CNT_PEAK;
	stcTmr4OcInit.u16OcoInvalidOp = TMR4_OCO_INVAILD_OP_LOW;//比较输出无效时输出低电平，可用于强制输出低电平
	stcTmr4OcInit.u16CompareVal = 0x0000;
    (void)TMR4_OCO_Init(TMR4_3_UNIT, TIM4_3_UH_C2_C2_CH, &stcTmr4OcInit);
    (void)TMR4_OCO_Init(TMR4_3_UNIT, TIM4_3_UL_W2_W2_CH, &stcTmr4OcInit);
    (void)TMR4_OCO_Init(TMR4_3_UNIT, TIM4_3_VH_C1_C1_CH, &stcTmr4OcInit);
	(void)TMR4_OCO_Init(TMR4_3_UNIT, TIM4_3_VL_W1_W1_CH, &stcTmr4OcInit);
	
	/* TMR4 OC high channel: compare mode OCMR[15:0] = 0x0FFF = b 0000 1111 1111 1111 */
    unTmr4OcOcmrh.OCMRx_f.OCFDCH = TMR4_OCO_OCF_SET; /* bit[0]     1  */
    unTmr4OcOcmrh.OCMRx_f.OCFPKH = TMR4_OCO_OCF_SET; /* bit[1]     1  */
    unTmr4OcOcmrh.OCMRx_f.OCFUCH = TMR4_OCO_OCF_SET; /* bit[2]     1  */
    unTmr4OcOcmrh.OCMRx_f.OCFZRH = TMR4_OCO_OCF_SET; /* bit[3]     1  */
    unTmr4OcOcmrh.OCMRx_f.OPDCH  = TMR4_OCO_OP_HOLD;    /* Bit[5:4]   11 */
    unTmr4OcOcmrh.OCMRx_f.OPPKH  = TMR4_OCO_OP_HIGH;    /* Bit[7:6]   11 */
    unTmr4OcOcmrh.OCMRx_f.OPUCH  = TMR4_OCO_OP_LOW;    /* Bit[9:8]   11 */
    unTmr4OcOcmrh.OCMRx_f.OPZRH  = TMR4_OCO_OP_LOW;    /* Bit[11:10] 11 */
    unTmr4OcOcmrh.OCMRx_f.OPNPKH = TMR4_OCO_OP_LOW;    /* Bit[13:12] 00 */
    unTmr4OcOcmrh.OCMRx_f.OPNZRH = TMR4_OCO_OP_HIGH;    /* Bit[15:14] 00 */
	unTmr4OcOcmrh.u16ExtendMatch = TMR4_OCO_EXTEND_MATCH_DISABLE;
    TMR4_OCO_SetHighChCompareMode(TMR4_3_UNIT, TIM4_3_UH_C2_C2_CH, &unTmr4OcOcmrh);
    TMR4_OCO_SetHighChCompareMode(TMR4_3_UNIT, TIM4_3_VH_C1_C1_CH, &unTmr4OcOcmrh);

    /* TMR4 OC low channel: compare mode OCMR[31:0] 0x0FF0 0FFF = b 0000 1111 1111 0000   0000 1111 1111 1111 */
    unTmr4OcOcmrl.OCMRx_f.OCFDCL  = TMR4_OCO_OCF_SET; /* bit[0]     1  */
    unTmr4OcOcmrl.OCMRx_f.OCFPKL  = TMR4_OCO_OCF_SET; /* bit[1]     1  */
    unTmr4OcOcmrl.OCMRx_f.OCFUCL  = TMR4_OCO_OCF_SET; /* bit[2]     1  */
    unTmr4OcOcmrl.OCMRx_f.OCFZRL  = TMR4_OCO_OCF_SET; /* bit[3]     1  */
    unTmr4OcOcmrl.OCMRx_f.OPDCL   = TMR4_OCO_OP_HOLD;    /* bit[5:4]   11 */
    unTmr4OcOcmrl.OCMRx_f.OPPKL   = TMR4_OCO_OP_HIGH;    /* bit[7:6]   11 */
    unTmr4OcOcmrl.OCMRx_f.OPUCL   = TMR4_OCO_OP_LOW;    /* bit[9:8]   11 */
    unTmr4OcOcmrl.OCMRx_f.OPZRL   = TMR4_OCO_OP_LOW;    /* bit[11:10] 11 */
    unTmr4OcOcmrl.OCMRx_f.OPNPKL  = TMR4_OCO_OP_LOW;    /* bit[13:12] 00 */
    unTmr4OcOcmrl.OCMRx_f.OPNZRL  = TMR4_OCO_OP_HIGH;    /* bit[15:14] 00 */
    unTmr4OcOcmrl.OCMRx_f.EOPNDCL = TMR4_OCO_OP_HOLD;    /* bit[17:16] 00 */
    unTmr4OcOcmrl.OCMRx_f.EOPNUCL = TMR4_OCO_OP_HOLD;    /* bit[19:18] 00 */
    unTmr4OcOcmrl.OCMRx_f.EOPDCL  = TMR4_OCO_OP_HOLD;    /* bit[21:20] 11 */
    unTmr4OcOcmrl.OCMRx_f.EOPPKL  = TMR4_OCO_OP_HIGH;    /* bit[23:22] 11 */
    unTmr4OcOcmrl.OCMRx_f.EOPUCL  = TMR4_OCO_OP_LOW;    /* bit[25:24] 11 */
    unTmr4OcOcmrl.OCMRx_f.EOPZRL  = TMR4_OCO_OP_LOW;    /* bit[27:26] 11 */
    unTmr4OcOcmrl.OCMRx_f.EOPNPKL = TMR4_OCO_OP_LOW;    /* bit[29:28] 00 */
    unTmr4OcOcmrl.OCMRx_f.EOPNZRL = TMR4_OCO_OP_HIGH;    /* bit[31:30] 00 */
	unTmr4OcOcmrl.u16ExtendMatch = TMR4_OCO_EXTEND_MATCH_DISABLE;
    TMR4_OCO_SetLowChCompareMode(TMR4_3_UNIT, TIM4_3_UL_W2_W2_CH, &unTmr4OcOcmrl);
	TMR4_OCO_SetLowChCompareMode(TMR4_3_UNIT, TIM4_3_VL_W1_W1_CH, &unTmr4OcOcmrl);
	
//	/* TMR4 OC: enable */
    TMR4_OCO_SetOutputCompare(TMR4_3_UNIT, TIM4_3_UH_C2_C2_CH, TMR4_OCO_DISABLE);
    TMR4_OCO_SetOutputCompare(TMR4_3_UNIT, TIM4_3_UL_W2_W2_CH, TMR4_OCO_DISABLE);
    TMR4_OCO_SetOutputCompare(TMR4_3_UNIT, TIM4_3_VH_C1_C1_CH, TMR4_OCO_DISABLE);
    TMR4_OCO_SetOutputCompare(TMR4_3_UNIT, TIM4_3_VL_W1_W1_CH, TMR4_OCO_DISABLE);
	
	/************************* Configure TMR4 PWM && TMR4 EVT*****************************/

    /* TMR4 PWM: initialize */
    (void)TMR4_PWM_StructInit(&stcTmr4PwmInit);
    stcTmr4PwmInit.u16Mode = TMR4_PWM_THROUGH_MODE;//直通模式
	stcTmr4PwmInit.u32OxHPortOutMode = TMR4_PWM_PORT_OUTPUT_NORMAL;
	stcTmr4PwmInit.u32OxLPortOutMode = TMR4_PWM_PORT_OUTPUT_NORMAL;//OExy使能输出
	stcTmr4PwmInit.u32EmbOxHPortState = TMR4_PWM_EMB_PORT_OUTPUT_NORMAL;
	stcTmr4PwmInit.u32EmbOxLPortState = TMR4_PWM_EMB_PORT_OUTPUT_NORMAL;
    (void)TMR4_PWM_Init(TMR4_3_UNIT, TIM4_U_GROUP, &stcTmr4PwmInit);
    (void)TMR4_PWM_Init(TMR4_3_UNIT, TIM4_V_GROUP, &stcTmr4PwmInit);  
    
    (void)TMR4_SEVT_StructInit(&stcTmr4EventInit);
	stcTmr4EventInit.u16UpMatchCmd = TMR4_SEVT_UP_ENABLE;
	stcTmr4EventInit.u16DownMatchCmd = TMR4_SEVT_DOWN_ENABLE;
	stcTmr4EventInit.u16PeakMatchCmd = TMR4_SEVT_PEAK_ENABLE;
	stcTmr4EventInit.u16ZeroMatchCmd = TMR4_SEVT_ZERO_ENABLE;
    stcTmr4EventInit.u16CompareVal = TIM4_PERIOD_VALUE;
    (void)TMR4_SEVT_Init(TMR4_3_UNIT, TIM4_3_UH_C2_C2_CH, &stcTmr4EventInit);
//    /* TMR4 event: set output event signal */
//    TMR4_SEVT_SetPortOutputEventSig(TMR4_3_UNIT, TMR4_SEVT_PORT_OUTPUT_EVENT0_SIGNAL);
//    
//    /* TMR4 PWM: enable main output  */
//    TMR4_PWM_MasterOutputCmd(TMR4_3_UNIT, Enable);//MOE使能输出
    /* Start TMR4 count. */
    TMR4_CNT_Start(TMR4_3_UNIT);
}

/*****************************************************************************************
* Function Name: Time_Init
* Description  : 所有定时器初始化
* Arguments    : NONE
* Return Value : NONE
******************************************************************************************/
void Time_Init(void)
{
//	TmrA_4_CW456_PWM_Config();
//	TmrA_4_CW123_PWM_Config();
//	TMRA_Gpio_Disable_Init();
	Timer4_2_Config();//频闪测试
	Timer4_3_Config();
	Timer2_1_Config();
//	TMRA_PWM_Cmd(M4_TMRA_2, APP_TMRA_2_PWM_CW_CH4, Disable); //不使用这一路
}
/*****************************************************************************************
* Function Name: Time_Init
* Description  : 所有定时器初始化
* Arguments    : NONE
* Return Value : NONE
******************************************************************************************/
#ifdef PR_307_L3
void Channel_Output_Control(uint8_t channel, en_functional_state_t state)
{
	stc_gpio_init_t stcGpioInit;
    (void)GPIO_StructInit(&stcGpioInit);
    stcGpioInit.u16PinDir = PIN_DIR_OUT;
    stcGpioInit.u16PinDrv = PIN_DRV_HIGH;
		if(channel < LAMP_DRV_ONE_CHANNEL_NUM)
		{
			switch(channel) 
			{
				case 0:
					TMR4_OCO_SetOcoInvalidOp(TMR4_2_UNIT, TIM4_2_UH_R_G_CH,  TMR4_OCO_INVAILD_OP_LOW);
					if(state == Disable)
					{
						GPIO_SetFunc(TIM4_2_UH_R_G_PORT, TIM4_2_UH_R_G_PIN, GPIO_FUNC_0_GPO, PIN_SUBFUNC_DISABLE);
						(void)GPIO_Init(TIM4_2_UH_R_G_PORT, TIM4_2_UH_R_G_PIN, &stcGpioInit);
						GPIO_ResetPins(TIM4_2_UH_R_G_PORT,TIM4_2_UH_R_G_PIN);
//						TMR4_OCO_SetOutputCompare(TMR4_2_UNIT, TIM4_2_UH_R_G_CH, TMR4_OCO_DISABLE);
					}
					else
					{
						GPIO_SetFunc(TIM4_2_UH_R_G_PORT, TIM4_2_UH_R_G_PIN, GPIO_FUNC_2, PIN_SUBFUNC_DISABLE);
						TMR4_OCO_SetOutputCompare(TMR4_2_UNIT, TIM4_2_UH_R_G_CH, TMR4_OCO_ENABLE);
					}
//					state == Disable ? TMR4_OCO_SetOutputCompare(TMR4_2_UNIT, TIM4_2_UH_R_G_CH, TMR4_OCO_DISABLE) : TMR4_OCO_SetOutputCompare(TMR4_2_UNIT, TIM4_2_UH_R_G_CH, TMR4_OCO_ENABLE);
					break;
				case 1:
					TMR4_OCO_SetOcoInvalidOp(TMR4_2_UNIT, TIM4_2_UL_G_W3_CH,  TMR4_OCO_INVAILD_OP_LOW);
					if(state == Disable)
					{
						GPIO_SetFunc(TIM4_2_UL_G_W3_PORT, TIM4_2_UL_G_W3_PIN, GPIO_FUNC_0_GPO, PIN_SUBFUNC_DISABLE);
						(void)GPIO_Init(TIM4_2_UL_G_W3_PORT, TIM4_2_UL_G_W3_PIN, &stcGpioInit);
						GPIO_ResetPins(TIM4_2_UL_G_W3_PORT,TIM4_2_UL_G_W3_PIN);
//						TMR4_OCO_SetOutputCompare(TMR4_2_UNIT, TIM4_2_UH_R_G_CH, TMR4_OCO_DISABLE);
					}
					else
					{
						GPIO_SetFunc(TIM4_2_UL_G_W3_PORT, TIM4_2_UL_G_W3_PIN, GPIO_FUNC_2, PIN_SUBFUNC_DISABLE);
						TMR4_OCO_SetOutputCompare(TMR4_2_UNIT, TIM4_2_UL_G_W3_CH, TMR4_OCO_ENABLE);
					}
//					state == Disable ? TMR4_OCO_SetOutputCompare(TMR4_2_UNIT, TIM4_2_UL_G_W3_CH, TMR4_OCO_DISABLE) : TMR4_OCO_SetOutputCompare(TMR4_2_UNIT, TIM4_2_UL_G_W3_CH, TMR4_OCO_ENABLE);
					break;
				case 2:
//					TMR4_OCO_SetOcoInvalidOp(TMR4_2_UNIT, TIM4_2_VH_B_C3_CH,  TMR4_OCO_INVAILD_OP_LOW);
//					state == Disable ? TMR4_OCO_SetOutputCompare(TMR4_2_UNIT, TIM4_2_VH_B_C3_CH, TMR4_OCO_DISABLE) : TMR4_OCO_SetOutputCompare(TMR4_2_UNIT, TIM4_2_VH_B_C3_CH, TMR4_OCO_ENABLE);
//					break;
				
					TMR4_OCO_SetOcoInvalidOp(TMR4_2_UNIT, TIM4_2_VH_B_C3_CH,  TMR4_OCO_INVAILD_OP_LOW);
					if(state == Disable)
					{
						GPIO_SetFunc(TIM4_2_VH_B_C3_PORT, TIM4_2_VH_B_C3_PIN, GPIO_FUNC_0_GPO, PIN_SUBFUNC_DISABLE);
						(void)GPIO_Init(TIM4_2_VH_B_C3_PORT, TIM4_2_VH_B_C3_PIN, &stcGpioInit);
						GPIO_ResetPins(TIM4_2_VH_B_C3_PORT,TIM4_2_VH_B_C3_PIN);
//						TMR4_OCO_SetOutputCompare(TMR4_2_UNIT, TIM4_2_UH_R_G_CH, TMR4_OCO_DISABLE);
					}
					else
					{
						GPIO_SetFunc(TIM4_2_VH_B_C3_PORT, TIM4_2_VH_B_C3_PIN, GPIO_FUNC_2, PIN_SUBFUNC_DISABLE);
						TMR4_OCO_SetOutputCompare(TMR4_2_UNIT, TIM4_2_VH_B_C3_CH, TMR4_OCO_ENABLE);
					}
//					state == Disable ? TMR4_OCO_SetOutputCompare(TMR4_2_UNIT, TIM4_2_VH_B_C3_CH, TMR4_OCO_DISABLE) : TMR4_OCO_SetOutputCompare(TMR4_2_UNIT, TIM4_2_VH_B_C3_CH, TMR4_OCO_ENABLE);
					break;
					
				default:
					break;
			}
		}			
		else
		{
			switch(channel) 
			{
				case 3:
					TMR4_OCO_SetOcoInvalidOp(TMR4_3_UNIT, TIM4_3_VL_W1_W1_CH,  TMR4_OCO_INVAILD_OP_LOW);
					if(state == Disable)
					{
						GPIO_SetFunc(TIM4_3_VL_W1_W1_PORT, TIM4_3_VL_W1_W1_PIN, GPIO_FUNC_0_GPO, PIN_SUBFUNC_DISABLE);
						(void)GPIO_Init(TIM4_3_VL_W1_W1_PORT, TIM4_3_VL_W1_W1_PIN, &stcGpioInit);
						GPIO_ResetPins(TIM4_3_VL_W1_W1_PORT,TIM4_3_VL_W1_W1_PIN);
//						TMR4_OCO_SetOutputCompare(TMR4_2_UNIT, TIM4_2_UH_R_G_CH, TMR4_OCO_DISABLE);
					}
					else
					{
						GPIO_SetFunc(TIM4_3_VL_W1_W1_PORT, TIM4_3_VL_W1_W1_PIN, GPIO_FUNC_2, PIN_SUBFUNC_DISABLE);
						TMR4_OCO_SetOutputCompare(TMR4_3_UNIT, TIM4_3_VL_W1_W1_CH, TMR4_OCO_ENABLE);
					}
//					state == Disable ? TMR4_OCO_SetOutputCompare(TMR4_3_UNIT, TIM4_3_VL_W1_W1_CH, TMR4_OCO_DISABLE) : TMR4_OCO_SetOutputCompare(TMR4_3_UNIT, TIM4_3_VL_W1_W1_CH, TMR4_OCO_ENABLE);                    
					break;                                                                                         
				case 4:   
					TMR4_OCO_SetOcoInvalidOp(TMR4_3_UNIT, TIM4_3_VH_C1_C1_CH,  TMR4_OCO_INVAILD_OP_LOW);
					if(state == Disable)
					{
						GPIO_SetFunc(TIM4_3_VH_C1_C1_PORT, TIM4_3_VH_C1_C1_PIN, GPIO_FUNC_0_GPO, PIN_SUBFUNC_DISABLE);
						(void)GPIO_Init(TIM4_3_VH_C1_C1_PORT, TIM4_3_VH_C1_C1_PIN, &stcGpioInit);
						GPIO_ResetPins(TIM4_3_VH_C1_C1_PORT,TIM4_3_VH_C1_C1_PIN);
//						TMR4_OCO_SetOutputCompare(TMR4_2_UNIT, TIM4_2_UH_R_G_CH, TMR4_OCO_DISABLE);
					}
					else
					{
						GPIO_SetFunc(TIM4_3_VH_C1_C1_PORT, TIM4_3_VH_C1_C1_PIN, GPIO_FUNC_2, PIN_SUBFUNC_DISABLE);
						TMR4_OCO_SetOutputCompare(TMR4_3_UNIT, TIM4_3_VH_C1_C1_CH, TMR4_OCO_ENABLE);
					}
//					state == Disable ? TMR4_OCO_SetOutputCompare(TMR4_3_UNIT, TIM4_3_VH_C1_C1_CH, TMR4_OCO_DISABLE) : TMR4_OCO_SetOutputCompare(TMR4_3_UNIT, TIM4_3_VH_C1_C1_CH, TMR4_OCO_ENABLE);                   
					break;                                                                                        
				case 5:  
//					TMR4_OCO_SetOcoInvalidOp(TMR4_3_UNIT, TIM4_3_UL_W2_W2_CH,  TMR4_OCO_INVAILD_OP_LOW);
//					state == Disable ? TMR4_OCO_SetOutputCompare(TMR4_3_UNIT, TIM4_3_UL_W2_W2_CH, TMR4_OCO_DISABLE) : TMR4_OCO_SetOutputCompare(TMR4_3_UNIT, TIM4_3_UL_W2_W2_CH, TMR4_OCO_ENABLE);                        
//					break;  
				
					TMR4_OCO_SetOcoInvalidOp(TMR4_3_UNIT, TIM4_3_UL_W2_W2_CH,  TMR4_OCO_INVAILD_OP_LOW);
					if(state == Disable)
					{
						GPIO_SetFunc(TIM4_3_UL_W2_W2_PORT, TIM4_3_UL_W2_W2_PIN, GPIO_FUNC_0_GPO, PIN_SUBFUNC_DISABLE);
						(void)GPIO_Init(TIM4_3_UL_W2_W2_PORT, TIM4_3_UL_W2_W2_PIN, &stcGpioInit);
						GPIO_ResetPins(TIM4_3_UL_W2_W2_PORT,TIM4_3_UL_W2_W2_PIN);
//						TMR4_OCO_SetOutputCompare(TMR4_2_UNIT, TIM4_2_UH_R_G_CH, TMR4_OCO_DISABLE);
					}
					else
					{
						GPIO_SetFunc(TIM4_3_UL_W2_W2_PORT, TIM4_3_UL_W2_W2_PIN, GPIO_FUNC_2, PIN_SUBFUNC_DISABLE);
						TMR4_OCO_SetOutputCompare(TMR4_3_UNIT, TIM4_3_UL_W2_W2_CH, TMR4_OCO_ENABLE);
					}
//					state == Disable ? TMR4_OCO_SetOutputCompare(TMR4_3_UNIT, TIM4_3_UL_W2_W2_CH, TMR4_OCO_DISABLE) : TMR4_OCO_SetOutputCompare(TMR4_3_UNIT, TIM4_3_UL_W2_W2_CH, TMR4_OCO_ENABLE);                        
					break;
				case 6:
					TMR4_OCO_SetOcoInvalidOp(TMR4_3_UNIT, TIM4_3_UH_C2_C2_CH,  TMR4_OCO_INVAILD_OP_LOW);
					if(state == Disable)
					{
						GPIO_SetFunc(TIM4_3_UH_C2_C2_PORT, TIM4_3_UH_C2_C2_PIN, GPIO_FUNC_0_GPO, PIN_SUBFUNC_DISABLE);
						(void)GPIO_Init(TIM4_3_UH_C2_C2_PORT, TIM4_3_UH_C2_C2_PIN, &stcGpioInit);
						GPIO_ResetPins(TIM4_3_UH_C2_C2_PORT,TIM4_3_UH_C2_C2_PIN);
//						TMR4_OCO_SetOutputCompare(TMR4_2_UNIT, TIM4_2_UH_R_G_CH, TMR4_OCO_DISABLE);
					}
					else
					{
						GPIO_SetFunc(TIM4_3_UH_C2_C2_PORT, TIM4_3_UH_C2_C2_PIN, GPIO_FUNC_2, PIN_SUBFUNC_DISABLE);
						TMR4_OCO_SetOutputCompare(TMR4_3_UNIT, TIM4_3_UH_C2_C2_CH, TMR4_OCO_ENABLE);
					}
//					state == Disable ? TMR4_OCO_SetOutputCompare(TMR4_3_UNIT, TIM4_3_UH_C2_C2_CH, TMR4_OCO_DISABLE) : TMR4_OCO_SetOutputCompare(TMR4_3_UNIT, TIM4_3_UH_C2_C2_CH, TMR4_OCO_ENABLE);
					break;
				default:                                                                                                  
					break;                                                                                                
			}
		}
}

#endif

#ifdef PR_308_L2
void Channel_Output_Control(uint8_t channel, en_functional_state_t state)
{
	stc_gpio_init_t stcGpioInit;
    (void)GPIO_StructInit(&stcGpioInit);
    stcGpioInit.u16PinDir = PIN_DIR_OUT;
    stcGpioInit.u16PinDrv = PIN_DRV_HIGH;
		if(channel < LAMP_DRV_ONE_CHANNEL_NUM)
		{
			switch(channel) 
			{
				case 0:
					TMR4_OCO_SetOcoInvalidOp(TMR4_2_UNIT, TIM4_2_UH_R_G_CH,  TMR4_OCO_INVAILD_OP_LOW);
					if(state == Disable)
					{
						GPIO_SetFunc(TIM4_2_UH_R_G_PORT, TIM4_2_UH_R_G_PIN, GPIO_FUNC_0_GPO, PIN_SUBFUNC_DISABLE);
						(void)GPIO_Init(TIM4_2_UH_R_G_PORT, TIM4_2_UH_R_G_PIN, &stcGpioInit);
						GPIO_ResetPins(TIM4_2_UH_R_G_PORT,TIM4_2_UH_R_G_PIN);
//						TMR4_OCO_SetOutputCompare(TMR4_2_UNIT, TIM4_2_UH_R_G_CH, TMR4_OCO_DISABLE);
					}
					else
					{
						GPIO_SetFunc(TIM4_2_UH_R_G_PORT, TIM4_2_UH_R_G_PIN, GPIO_FUNC_2, PIN_SUBFUNC_DISABLE);
						TMR4_OCO_SetOutputCompare(TMR4_2_UNIT, TIM4_2_UH_R_G_CH, TMR4_OCO_ENABLE);
					}
//					state == Disable ? TMR4_OCO_SetOutputCompare(TMR4_2_UNIT, TIM4_2_UH_R_G_CH, TMR4_OCO_DISABLE) : TMR4_OCO_SetOutputCompare(TMR4_2_UNIT, TIM4_2_UH_R_G_CH, TMR4_OCO_ENABLE);
					break;
				case 1:
					TMR4_OCO_SetOcoInvalidOp(TMR4_2_UNIT, TIM4_2_UL_G_W3_CH,  TMR4_OCO_INVAILD_OP_LOW);
					if(state == Disable)
					{
						GPIO_SetFunc(TIM4_2_UL_G_W3_PORT, TIM4_2_UL_G_W3_PIN, GPIO_FUNC_0_GPO, PIN_SUBFUNC_DISABLE);
						(void)GPIO_Init(TIM4_2_UL_G_W3_PORT, TIM4_2_UL_G_W3_PIN, &stcGpioInit);
						GPIO_ResetPins(TIM4_2_UL_G_W3_PORT,TIM4_2_UL_G_W3_PIN);
//						TMR4_OCO_SetOutputCompare(TMR4_2_UNIT, TIM4_2_UH_R_G_CH, TMR4_OCO_DISABLE);
					}
					else
					{
						GPIO_SetFunc(TIM4_2_UL_G_W3_PORT, TIM4_2_UL_G_W3_PIN, GPIO_FUNC_2, PIN_SUBFUNC_DISABLE);
						TMR4_OCO_SetOutputCompare(TMR4_2_UNIT, TIM4_2_UL_G_W3_CH, TMR4_OCO_ENABLE);
					}
//					state == Disable ? TMR4_OCO_SetOutputCompare(TMR4_2_UNIT, TIM4_2_UL_G_W3_CH, TMR4_OCO_DISABLE) : TMR4_OCO_SetOutputCompare(TMR4_2_UNIT, TIM4_2_UL_G_W3_CH, TMR4_OCO_ENABLE);
					break;
				case 2:
//					TMR4_OCO_SetOcoInvalidOp(TMR4_2_UNIT, TIM4_2_VH_B_C3_CH,  TMR4_OCO_INVAILD_OP_LOW);
//					state == Disable ? TMR4_OCO_SetOutputCompare(TMR4_2_UNIT, TIM4_2_VH_B_C3_CH, TMR4_OCO_DISABLE) : TMR4_OCO_SetOutputCompare(TMR4_2_UNIT, TIM4_2_VH_B_C3_CH, TMR4_OCO_ENABLE);
//					break;
				
					TMR4_OCO_SetOcoInvalidOp(TMR4_3_UNIT, TIM4_3_UL_W2_W2_CH,  TMR4_OCO_INVAILD_OP_LOW);
					if(state == Disable)
					{
						GPIO_SetFunc(TIM4_3_UL_W2_W2_PORT, TIM4_3_UL_W2_W2_PIN, GPIO_FUNC_0_GPO, PIN_SUBFUNC_DISABLE);
						(void)GPIO_Init(TIM4_3_UL_W2_W2_PORT, TIM4_3_UL_W2_W2_PIN, &stcGpioInit);
						GPIO_ResetPins(TIM4_3_UL_W2_W2_PORT,TIM4_3_UL_W2_W2_PIN);
//						TMR4_OCO_SetOutputCompare(TMR4_2_UNIT, TIM4_2_UH_R_G_CH, TMR4_OCO_DISABLE);
					}
					else
					{
						GPIO_SetFunc(TIM4_3_UL_W2_W2_PORT, TIM4_3_UL_W2_W2_PIN, GPIO_FUNC_2, PIN_SUBFUNC_DISABLE);
						TMR4_OCO_SetOutputCompare(TMR4_3_UNIT, TIM4_3_UL_W2_W2_CH, TMR4_OCO_ENABLE);
					}
//					state == Disable ? TMR4_OCO_SetOutputCompare(TMR4_3_UNIT, TIM4_3_UL_W2_W2_CH, TMR4_OCO_DISABLE) : TMR4_OCO_SetOutputCompare(TMR4_3_UNIT, TIM4_3_UL_W2_W2_CH, TMR4_OCO_ENABLE);                        
					break;
				default:
					break;
			}
		}			
		else
		{
			switch(channel) 
			{
				case 3:
					TMR4_OCO_SetOcoInvalidOp(TMR4_3_UNIT, TIM4_3_VL_W1_W1_CH,  TMR4_OCO_INVAILD_OP_LOW);
					if(state == Disable)
					{
						GPIO_SetFunc(TIM4_3_VL_W1_W1_PORT, TIM4_3_VL_W1_W1_PIN, GPIO_FUNC_0_GPO, PIN_SUBFUNC_DISABLE);
						(void)GPIO_Init(TIM4_3_VL_W1_W1_PORT, TIM4_3_VL_W1_W1_PIN, &stcGpioInit);
						GPIO_ResetPins(TIM4_3_VL_W1_W1_PORT,TIM4_3_VL_W1_W1_PIN);
//						TMR4_OCO_SetOutputCompare(TMR4_2_UNIT, TIM4_2_UH_R_G_CH, TMR4_OCO_DISABLE);
					}
					else
					{
						GPIO_SetFunc(TIM4_3_VL_W1_W1_PORT, TIM4_3_VL_W1_W1_PIN, GPIO_FUNC_2, PIN_SUBFUNC_DISABLE);
						TMR4_OCO_SetOutputCompare(TMR4_3_UNIT, TIM4_3_VL_W1_W1_CH, TMR4_OCO_ENABLE);
					}
//					state == Disable ? TMR4_OCO_SetOutputCompare(TMR4_3_UNIT, TIM4_3_VL_W1_W1_CH, TMR4_OCO_DISABLE) : TMR4_OCO_SetOutputCompare(TMR4_3_UNIT, TIM4_3_VL_W1_W1_CH, TMR4_OCO_ENABLE);                    
					break;                                                                                         
				case 4:   
					TMR4_OCO_SetOcoInvalidOp(TMR4_3_UNIT, TIM4_3_VH_C1_C1_CH,  TMR4_OCO_INVAILD_OP_LOW);
					if(state == Disable)
					{
						GPIO_SetFunc(TIM4_3_VH_C1_C1_PORT, TIM4_3_VH_C1_C1_PIN, GPIO_FUNC_0_GPO, PIN_SUBFUNC_DISABLE);
						(void)GPIO_Init(TIM4_3_VH_C1_C1_PORT, TIM4_3_VH_C1_C1_PIN, &stcGpioInit);
						GPIO_ResetPins(TIM4_3_VH_C1_C1_PORT,TIM4_3_VH_C1_C1_PIN);
//						TMR4_OCO_SetOutputCompare(TMR4_2_UNIT, TIM4_2_UH_R_G_CH, TMR4_OCO_DISABLE);
					}
					else
					{
						GPIO_SetFunc(TIM4_3_VH_C1_C1_PORT, TIM4_3_VH_C1_C1_PIN, GPIO_FUNC_2, PIN_SUBFUNC_DISABLE);
						TMR4_OCO_SetOutputCompare(TMR4_3_UNIT, TIM4_3_VH_C1_C1_CH, TMR4_OCO_ENABLE);
					}
//					state == Disable ? TMR4_OCO_SetOutputCompare(TMR4_3_UNIT, TIM4_3_VH_C1_C1_CH, TMR4_OCO_DISABLE) : TMR4_OCO_SetOutputCompare(TMR4_3_UNIT, TIM4_3_VH_C1_C1_CH, TMR4_OCO_ENABLE);                   
					break;                                                                                        
				case 5:  
//					TMR4_OCO_SetOcoInvalidOp(TMR4_3_UNIT, TIM4_3_UL_W2_W2_CH,  TMR4_OCO_INVAILD_OP_LOW);
//					state == Disable ? TMR4_OCO_SetOutputCompare(TMR4_3_UNIT, TIM4_3_UL_W2_W2_CH, TMR4_OCO_DISABLE) : TMR4_OCO_SetOutputCompare(TMR4_3_UNIT, TIM4_3_UL_W2_W2_CH, TMR4_OCO_ENABLE);                        
//					break;  
				
					TMR4_OCO_SetOcoInvalidOp(TMR4_2_UNIT, TIM4_2_VH_B_C3_CH,  TMR4_OCO_INVAILD_OP_LOW);
					if(state == Disable)
					{
						GPIO_SetFunc(TIM4_2_VH_B_C3_PORT, TIM4_2_VH_B_C3_PIN, GPIO_FUNC_0_GPO, PIN_SUBFUNC_DISABLE);
						(void)GPIO_Init(TIM4_2_VH_B_C3_PORT, TIM4_2_VH_B_C3_PIN, &stcGpioInit);
						GPIO_ResetPins(TIM4_2_VH_B_C3_PORT,TIM4_2_VH_B_C3_PIN);
//						TMR4_OCO_SetOutputCompare(TMR4_2_UNIT, TIM4_2_UH_R_G_CH, TMR4_OCO_DISABLE);
					}
					else
					{
						GPIO_SetFunc(TIM4_2_VH_B_C3_PORT, TIM4_2_VH_B_C3_PIN, GPIO_FUNC_2, PIN_SUBFUNC_DISABLE);
						TMR4_OCO_SetOutputCompare(TMR4_2_UNIT, TIM4_2_VH_B_C3_CH, TMR4_OCO_ENABLE);
					}
//					state == Disable ? TMR4_OCO_SetOutputCompare(TMR4_2_UNIT, TIM4_2_VH_B_C3_CH, TMR4_OCO_DISABLE) : TMR4_OCO_SetOutputCompare(TMR4_2_UNIT, TIM4_2_VH_B_C3_CH, TMR4_OCO_ENABLE);
					break;
				case 6:
					TMR4_OCO_SetOcoInvalidOp(TMR4_3_UNIT, TIM4_3_UH_C2_C2_CH,  TMR4_OCO_INVAILD_OP_LOW);
					if(state == Disable)
					{
						GPIO_SetFunc(TIM4_3_UH_C2_C2_PORT, TIM4_3_UH_C2_C2_PIN, GPIO_FUNC_0_GPO, PIN_SUBFUNC_DISABLE);
						(void)GPIO_Init(TIM4_3_UH_C2_C2_PORT, TIM4_3_UH_C2_C2_PIN, &stcGpioInit);
						GPIO_ResetPins(TIM4_3_UH_C2_C2_PORT,TIM4_3_UH_C2_C2_PIN);
//						TMR4_OCO_SetOutputCompare(TMR4_2_UNIT, TIM4_2_UH_R_G_CH, TMR4_OCO_DISABLE);
					}
					else
					{
						GPIO_SetFunc(TIM4_3_UH_C2_C2_PORT, TIM4_3_UH_C2_C2_PIN, GPIO_FUNC_2, PIN_SUBFUNC_DISABLE);
						TMR4_OCO_SetOutputCompare(TMR4_3_UNIT, TIM4_3_UH_C2_C2_CH, TMR4_OCO_ENABLE);
					}
//					state == Disable ? TMR4_OCO_SetOutputCompare(TMR4_3_UNIT, TIM4_3_UH_C2_C2_CH, TMR4_OCO_DISABLE) : TMR4_OCO_SetOutputCompare(TMR4_3_UNIT, TIM4_3_UH_C2_C2_CH, TMR4_OCO_ENABLE);
					break;
				default:                                                                                                  
					break;                                                                                                
			}
		}
}
#endif

