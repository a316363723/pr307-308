#include "hal_timer.h"
#include "hc32_ddl.h"
#include "hc32f4a0.h"

//?????????????????????????
#define         TMR6_CAPUTURE_UNIT       (M4_TMR6_8)
#define         FCG2PERIPH               (PWC_FCG2_TMR6_8)
#define         TMR6_CAPUTURE_PORT       (GPIO_PORT_C)
#define         TMR6_CAPUTURE_PIN        (GPIO_PIN_02)
#define         TMR6_CAPUTURE_FUNC       (GPIO_FUNC_3_TIM68_PWMA)
#define         TMR6_CAPUTURE_IRQN       (Int027_IRQn)
#define         TMR6_CAPUTURE_INTSRC     (INT_TMR6_8_GCMA)



#define         TMR6_ENCODER_UNIT    (M4_TMR6_1)
#define         TMR6_1_PWMA_PORT     GPIO_PORT_F // (GPIO_PORT_B)
#define         TMR6_1_PWMA_PIN      GPIO_PIN_13// (GPIO_PIN_09)

#define         TMR6_1_PWMB_PORT     GPIO_PORT_F //(GPIO_PORT_B)
#define         TMR6_1_PWMB_PIN      GPIO_PIN_14//(GPIO_PIN_08)
/*
 * Timer2 unit and channel definitions for this example.
 * 'APP_TMR2_UNIT' can be defined as M4_TMR2_<t>(t=1 ~ 4).
 * 'APP_TMR2_CH' can be defined as TMR2_CH_x(x=A, B).
 *
 * NOTE!!! The following definitions are depend on the definitions of 'APP_TMR2_UNIT' and 'APP_TMR2_CH'.
 *   APP_TMR2_ASYNC_CLK_PORT
 *   APP_TMR2_ASYNC_CLK_PIN
 *   APP_TMR2_ASYNC_CLK_PIN_FUNC
 *   APP_TMR2_TRIG_PORT
 *   APP_TMR2_TRIG_PIN
 *   APP_TMR2_TRIG_PIN_FUNC
 *   APP_TMR2_INT_SRC
 */
#define APP_TMR2_UNIT                       (M4_TMR2_1)
#define APP_TMR2_CH                         (TMR2_CH_A)
#define APP_TMR2_PERIP_CLK                  (PWC_FCG2_TMR2_1)
/*
 * Clock source for this example.
 * In this example:
 *   1. System clock is configured as 240MHz.
 *   2. PCLK1 is 120MHz.
 *   3. Use timer2 to count 1ms.
 *
 * A simple formula for calculating the compare value is:
 *   Tmr2CompareVal = (Tmr2Period(us) * [Tmr2ClockSource(MHz) / Tmr2ClockDiv]) - 1.
 */
#define APP_TMR2_CLK                        (TMR2_CLK_SYNC_PCLK1)
#define APP_TMR2_CLK_SRC                    (TMR2_CLK_SYNC_PCLK1)
#define APP_TMR2_CLK_DIV                    (TMR2_CLK_DIV64)
#define APP_TMR2_CMP_VAL                    (1875UL - 1U)
/*
 * Definitions about Timer2 interrupt for the example.
 * Timer2 independent IRQn: [Int000_IRQn, Int031_IRQn], [Int050_IRQn, Int055_IRQn].
 *
 * NOTE:
 *   'APP_TMR2_INT_TYPE' can only be defined as 'TMR2_INT_CMP' for this example.
 */
#define APP_TMR2_INT_TYPE               (TMR2_INT_CMP)
#define APP_TMR2_INT_PRIO               (DDL_IRQ_PRIORITY_06)
#define APP_TMR2_INT_SRC                (INT_TMR2_1_CMPA)
#define APP_TMR2_IRQn                   (Int050_IRQn)

#define USE_NEW_PWM 1

struct hc4a0_timera_cfg
{
    M4_TMRA_TypeDef *timerx;
    uint8_t timer_ch;
    uint32_t fcg_periph; 
    uint32_t period_val;
    uint32_t polarity;   //??
    uint32_t pclk_div;

    uint8_t  port;
    uint16_t pin;
    uint16_t func;
    
};


static struct hc4a0_timera_cfg timera_config[HAL_TIMERA_NUM] = 
{
#if USE_NEW_PWM==1
//第二版自适应
#if PROJECT_TYPE==307
    [HAL_PWM_SELF_ADAPTION_R] = 
    {
        .timerx = M4_TMRA_5,
        .timer_ch = TMRA_CH_2,
        .fcg_periph = PWC_FCG2_TMRA_5,
        .pclk_div = TMRA_PCLK_DIV1,
        
        .period_val = 6000,
        .polarity = TMRA_PWM_CMP_LOW,
        .port = GPIO_PORT_A,
        .pin = GPIO_PIN_03,
        .func = GPIO_FUNC_5_TIMA5_PWM2,
    },
#elif PROJECT_TYPE==308
	[HAL_PWM_SELF_ADAPTION_R] = 
    {
        .timerx = M4_TMRA_4,
        .timer_ch = TMRA_CH_1,
        .fcg_periph = PWC_FCG2_TMRA_4,
        .pclk_div = TMRA_PCLK_DIV1,
        
        .period_val = 6000,
        .polarity = TMRA_PWM_CMP_LOW,
        .port = GPIO_PORT_D,
        .pin = GPIO_PIN_12,
        .func = GPIO_FUNC_4_TIMA4_PWM1,
    },
#endif

#if PROJECT_TYPE==307
    [HAL_PWM_SELF_ADAPTION_G] = 
    {
        .timerx = M4_TMRA_2,
        .timer_ch = TMRA_CH_3,
        .fcg_periph = PWC_FCG2_TMRA_2,
        .pclk_div = TMRA_PCLK_DIV1,
        
        .period_val = 6000,
        .polarity = TMRA_PWM_CMP_LOW,
        .port = GPIO_PORT_H,
        .pin = GPIO_PIN_06,
        .func = GPIO_FUNC_4_TIMA2_PWM3,
    },
#elif PROJECT_TYPE==308
	[HAL_PWM_SELF_ADAPTION_G] = 
    {
        .timerx = M4_TMRA_7,
        .timer_ch = TMRA_CH_3,
        .fcg_periph = PWC_FCG2_TMRA_7,
        .pclk_div = TMRA_PCLK_DIV1,
        
        .period_val = 6000,
        .polarity = TMRA_PWM_CMP_LOW,
        .port = GPIO_PORT_B,
        .pin = GPIO_PIN_15,
        .func = GPIO_FUNC_4_TIMA7_PWM3,
    },  
#endif
#if PROJECT_TYPE==307
    [HAL_PWM_SELF_ADAPTION_B] = 
    {
        .timerx = M4_TMRA_3,
        .timer_ch = TMRA_CH_1,
        .fcg_periph = PWC_FCG2_TMRA_3,
        .pclk_div = TMRA_PCLK_DIV1,
        
        .period_val = 6000,
        .polarity = TMRA_PWM_CMP_LOW, 
        .port = GPIO_PORT_A,
        .pin = GPIO_PIN_06,
        .func = GPIO_FUNC_5_TIMA3_PWM1,
    },
#elif PROJECT_TYPE==308
	[HAL_PWM_SELF_ADAPTION_B] = 
    {
        .timerx = M4_TMRA_10,
        .timer_ch = TMRA_CH_3,
        .fcg_periph = PWC_FCG2_TMRA_10,
        .pclk_div = TMRA_PCLK_DIV1,
        
        .period_val = 6000,
        .polarity = TMRA_PWM_CMP_LOW,
        .port = GPIO_PORT_F,
        .pin = GPIO_PIN_08,
        .func = GPIO_FUNC_4_TIMA10_PWM3,
    },
//	[HAL_PWM_SELF_ADAPTION_B] = 
//    {
//        .timerx = M4_TMRA_3,
//        .timer_ch = TMRA_CH_1,
//        .fcg_periph = PWC_FCG2_TMRA_3,
//        .pclk_div = TMRA_PCLK_DIV1,
//        
//        .period_val = 6000,
//        .polarity = TMRA_PWM_CMP_LOW, 
//        .port = GPIO_PORT_A,
//        .pin = GPIO_PIN_06,
//        .func = GPIO_FUNC_5_TIMA3_PWM1,
//    },
#endif
#if PROJECT_TYPE==307
    [HAL_PWM_SELF_ADAPTION_WW1] = 
    {
        .timerx = M4_TMRA_4,
        .timer_ch = TMRA_CH_1,
        .fcg_periph = PWC_FCG2_TMRA_4,
        .pclk_div = TMRA_PCLK_DIV1,
        
        .period_val = 6000,
        .polarity = TMRA_PWM_CMP_LOW,
        .port = GPIO_PORT_D,
        .pin = GPIO_PIN_12,
        .func = GPIO_FUNC_4_TIMA4_PWM1,
    },
#elif PROJECT_TYPE==308
	[HAL_PWM_SELF_ADAPTION_WW1] = 
    {
        .timerx = M4_TMRA_5,
        .timer_ch = TMRA_CH_2,
        .fcg_periph = PWC_FCG2_TMRA_5,
        .pclk_div = TMRA_PCLK_DIV1,
        
        .period_val = 6000,
        .polarity = TMRA_PWM_CMP_LOW,
        .port = GPIO_PORT_A,
        .pin = GPIO_PIN_03,
        .func = GPIO_FUNC_5_TIMA5_PWM2,
    },
#endif
#if PROJECT_TYPE==307
    [HAL_PWM_SELF_ADAPTION_CW1] = 
    {
        .timerx = M4_TMRA_2,
        .timer_ch = TMRA_CH_4,
        .fcg_periph = PWC_FCG2_TMRA_2,
        .pclk_div = TMRA_PCLK_DIV1,
        
        .period_val = 6000,
        .polarity = TMRA_PWM_CMP_LOW,
        .port = GPIO_PORT_H,
        .pin = GPIO_PIN_09,
        .func = GPIO_FUNC_4_TIMA2_PWM4,
    },
#elif PROJECT_TYPE==308
	[HAL_PWM_SELF_ADAPTION_CW1] = 
    {
        .timerx = M4_TMRA_2,
        .timer_ch = TMRA_CH_3,
        .fcg_periph = PWC_FCG2_TMRA_2,
        .pclk_div = TMRA_PCLK_DIV1,
        
        .period_val = 6000,
        .polarity = TMRA_PWM_CMP_LOW,
        .port = GPIO_PORT_H,
        .pin = GPIO_PIN_06,
        .func = GPIO_FUNC_4_TIMA2_PWM3,
    },
#endif
#if PROJECT_TYPE==307
    [HAL_PWM_SELF_ADAPTION_WW2] = 
    {
        .timerx = M4_TMRA_10,
        .timer_ch = TMRA_CH_3,
        .fcg_periph = PWC_FCG2_TMRA_10,
        .pclk_div = TMRA_PCLK_DIV1,
        
        .period_val = 6000,
        .polarity = TMRA_PWM_CMP_LOW,
        .port = GPIO_PORT_F,
        .pin = GPIO_PIN_08,
        .func = GPIO_FUNC_4_TIMA10_PWM3,
    },
#elif PROJECT_TYPE==308
	[HAL_PWM_SELF_ADAPTION_WW2] = 
    {
        .timerx = M4_TMRA_3,
        .timer_ch = TMRA_CH_1,
        .fcg_periph = PWC_FCG2_TMRA_3,
        .pclk_div = TMRA_PCLK_DIV1,
        
        .period_val = 6000,
        .polarity = TMRA_PWM_CMP_LOW, 
        .port = GPIO_PORT_A,
        .pin = GPIO_PIN_06,
        .func = GPIO_FUNC_5_TIMA3_PWM1,
    },
//	[HAL_PWM_SELF_ADAPTION_WW2] = 
//    {
//        .timerx = M4_TMRA_10,
//        .timer_ch = TMRA_CH_3,
//        .fcg_periph = PWC_FCG2_TMRA_10,
//        .pclk_div = TMRA_PCLK_DIV1,
//        
//        .period_val = 6000,
//        .polarity = TMRA_PWM_CMP_LOW,
//        .port = GPIO_PORT_F,
//        .pin = GPIO_PIN_08,
//        .func = GPIO_FUNC_4_TIMA10_PWM3,
//    },
#endif
#if PROJECT_TYPE==307		
    [HAL_PWM_SELF_ADAPTION_CW2] = 
    {
        .timerx = M4_TMRA_7,
        .timer_ch = TMRA_CH_3,
        .fcg_periph = PWC_FCG2_TMRA_7,
        .pclk_div = TMRA_PCLK_DIV1,
        
        .period_val = 6000,
        .polarity = TMRA_PWM_CMP_LOW,
        .port = GPIO_PORT_B,
        .pin = GPIO_PIN_15,
        .func = GPIO_FUNC_4_TIMA7_PWM3,
    },  
#elif PROJECT_TYPE==308
	[HAL_PWM_SELF_ADAPTION_CW2] = 
    {
        .timerx = M4_TMRA_2,
        .timer_ch = TMRA_CH_4,
        .fcg_periph = PWC_FCG2_TMRA_2,
        .pclk_div = TMRA_PCLK_DIV1,
        
        .period_val = 6000,
        .polarity = TMRA_PWM_CMP_LOW,
        .port = GPIO_PORT_H,
        .pin = GPIO_PIN_09,
        .func = GPIO_FUNC_4_TIMA2_PWM4,
    },
#endif
#endif
//第一版自适应
#if USE_NEW_PWM==0
[HAL_PWM_SELF_ADAPTION_R] = 
    {
        .timerx = M4_TMRA_5,
        .timer_ch = TMRA_CH_2,
        .fcg_periph = PWC_FCG2_TMRA_5,
        .pclk_div = TMRA_PCLK_DIV1,
        
        .period_val = 6000,
        .polarity = TMRA_PWM_CMP_LOW,
        .port = GPIO_PORT_A,
        .pin = GPIO_PIN_03,
        .func = GPIO_FUNC_5_TIMA5_PWM2,
    },
#if PROJECT_TYPE==307
    [HAL_PWM_SELF_ADAPTION_G] = 
    {
        .timerx = M4_TMRA_2,
        .timer_ch = TMRA_CH_3,
        .fcg_periph = PWC_FCG2_TMRA_2,
        .pclk_div = TMRA_PCLK_DIV1,
        
        .period_val = 6000,
        .polarity = TMRA_PWM_CMP_LOW,
        .port = GPIO_PORT_H,
        .pin = GPIO_PIN_06,
        .func = GPIO_FUNC_4_TIMA2_PWM3,
    },
#elif PROJECT_TYPE==308
	[HAL_PWM_SELF_ADAPTION_G] = 
    {
        .timerx = M4_TMRA_7,
        .timer_ch = TMRA_CH_3,
        .fcg_periph = PWC_FCG2_TMRA_7,
        .pclk_div = TMRA_PCLK_DIV1,
        
        .period_val = 6000,
        .polarity = TMRA_PWM_CMP_LOW,
        .port = GPIO_PORT_B,
        .pin = GPIO_PIN_15,
        .func = GPIO_FUNC_4_TIMA7_PWM3,
    },  
#endif

    [HAL_PWM_SELF_ADAPTION_B] = 
    {
        .timerx = M4_TMRA_3,
        .timer_ch = TMRA_CH_1,
        .fcg_periph = PWC_FCG2_TMRA_3,
        .pclk_div = TMRA_PCLK_DIV1,
        
        .period_val = 6000,
        .polarity = TMRA_PWM_CMP_LOW, 
        .port = GPIO_PORT_A,
        .pin = GPIO_PIN_06,
        .func = GPIO_FUNC_5_TIMA3_PWM1,
    },
    
    [HAL_PWM_SELF_ADAPTION_WW1] = 
    {
        .timerx = M4_TMRA_4,
        .timer_ch = TMRA_CH_1,
        .fcg_periph = PWC_FCG2_TMRA_4,
        .pclk_div = TMRA_PCLK_DIV1,
        
        .period_val = 6000,
        .polarity = TMRA_PWM_CMP_LOW,
        .port = GPIO_PORT_D,
        .pin = GPIO_PIN_12,
        .func = GPIO_FUNC_4_TIMA4_PWM1,
    },
	
    [HAL_PWM_SELF_ADAPTION_CW1] = 
    {
        .timerx = M4_TMRA_2,
        .timer_ch = TMRA_CH_4,
        .fcg_periph = PWC_FCG2_TMRA_2,
        .pclk_div = TMRA_PCLK_DIV1,
        
        .period_val = 6000,
        .polarity = TMRA_PWM_CMP_LOW,
        .port = GPIO_PORT_H,
        .pin = GPIO_PIN_09,
        .func = GPIO_FUNC_4_TIMA2_PWM4,
    },
	
    [HAL_PWM_SELF_ADAPTION_WW2] = 
    {
        .timerx = M4_TMRA_10,
        .timer_ch = TMRA_CH_3,
        .fcg_periph = PWC_FCG2_TMRA_10,
        .pclk_div = TMRA_PCLK_DIV1,
        
        .period_val = 6000,
        .polarity = TMRA_PWM_CMP_LOW,
        .port = GPIO_PORT_F,
        .pin = GPIO_PIN_08,
        .func = GPIO_FUNC_4_TIMA10_PWM3,
    },
#if PROJECT_TYPE==307		
    [HAL_PWM_SELF_ADAPTION_CW2] = 
    {
        .timerx = M4_TMRA_7,
        .timer_ch = TMRA_CH_3,
        .fcg_periph = PWC_FCG2_TMRA_7,
        .pclk_div = TMRA_PCLK_DIV1,
        
        .period_val = 6000,
        .polarity = TMRA_PWM_CMP_LOW,
        .port = GPIO_PORT_B,
        .pin = GPIO_PIN_15,
        .func = GPIO_FUNC_4_TIMA7_PWM3,
    },  
#elif PROJECT_TYPE==308
	[HAL_PWM_SELF_ADAPTION_CW2] = 
    {
        .timerx = M4_TMRA_2,
        .timer_ch = TMRA_CH_3,
        .fcg_periph = PWC_FCG2_TMRA_2,
        .pclk_div = TMRA_PCLK_DIV1,
        
        .period_val = 6000,
        .polarity = TMRA_PWM_CMP_LOW,
        .port = GPIO_PORT_H,
        .pin = GPIO_PIN_06,
        .func = GPIO_FUNC_4_TIMA2_PWM3,
    },
#endif
#endif
    [HAL_PWM_FAN] = 
    {
        .timerx = M4_TMRA_4,
        .timer_ch = TMRA_CH_3,
        .fcg_periph = PWC_FCG2_TMRA_4,
        .pclk_div = TMRA_PCLK_DIV128,
        
        .period_val = 6000,
        .polarity = TMRA_PWM_CMP_LOW,
        .port = GPIO_PORT_B,
        .pin = GPIO_PIN_08,
        .func = GPIO_FUNC_4_TIMA4_PWM3,
    },    
};


void hal_timer_duty_cycle_set(enum hal_timera_enum timer_type, uint32_t value)
{
    TMRA_SetCmpVal(timera_config[timer_type].timerx, timera_config[timer_type].timer_ch, value);
}

uint32_t hal_timer_duty_cycle_get(enum hal_timera_enum timer_type)
{
    return TMRA_GetCmpVal(timera_config[timer_type].timerx, timera_config[timer_type].timer_ch);
}

/**
 * @brief  TIMERA PWM
 * @param  None
 * @retval None
 */
void hal_timer_pwm_init(enum hal_timera_enum timer_type)
{
    stc_tmra_init_t stcInit;
    stc_tmra_pwm_cfg_t stcCfg;

    /* 1. Enable TimerA peripheral clock. */
    PWC_Fcg2PeriphClockCmd(timera_config[timer_type].fcg_periph, Enable);
    (void)TMRA_StructInit(&stcInit);

    /* 3. Modifies the initialization values depends on the application. */
    stcInit.u32CntDir    = TMRA_DIR_UP;
    stcInit.u32CntMode   = TMRA_MODE_SAWTOOTH;
	stcInit.u32PeriodVal = timera_config[timer_type].period_val;	
    stcInit.u32PCLKDiv = timera_config[timer_type].pclk_div;
    (void)TMRA_Init(timera_config[timer_type].timerx, &stcInit);

    /*4. Set the comparison reference value. */
    TMRA_SetCmpVal(timera_config[timer_type].timerx, timera_config[timer_type].timer_ch, 0);
    GPIO_SetFunc(timera_config[timer_type].port, timera_config[timer_type].pin, timera_config[timer_type].func, PIN_SUBFUNC_DISABLE);
    
	(void)TMRA_PWM_StructInit(&stcCfg);
	stcCfg.u32CmpPolarity = timera_config[timer_type].polarity;
    (void)TMRA_PWM_Config(timera_config[timer_type].timerx, timera_config[timer_type].timer_ch, &stcCfg);
    TMRA_PWM_Cmd(timera_config[timer_type].timerx, timera_config[timer_type].timer_ch, Enable);  

	/* Starts Timer. */
    TMRA_Start(timera_config[timer_type].timerx);	
	hal_timer_duty_cycle_set( HAL_PWM_SELF_ADAPTION_R ,5900) ;
}

/**********************************Timer 6******************************************/
//??????
void hal_timer_capture_init(void (*irq_caputure_cb)(void))
{
    uint32_t                       u32Period;
    stc_tmr6_basecnt_cfg_t         stcTIM6BaseCntCfg;
    stc_tmr6_port_input_cfg_t      stcTIM6PortInCfg;
    stc_tmr6_buf_func_cfg_t        stcBufCfg;
    stc_irq_signin_config_t        stcIrqRegiConf;
	
	(void)TMR6_BaseCntStructInit(&stcTIM6BaseCntCfg);
	(void)TMR6_PortInputStructInit(&stcTIM6PortInCfg);
	(void)TMR6_BufFuncStructInit(&stcBufCfg);
	
	PWC_Fcg2PeriphClockCmd(FCG2PERIPH, Enable);
	GPIO_SetFunc(TMR6_CAPUTURE_PORT, TMR6_CAPUTURE_PIN, TMR6_CAPUTURE_FUNC, PIN_SUBFUNC_DISABLE);
	
	stcTIM6BaseCntCfg.u32CntMode = TMR6_MODE_SAWTOOTH;
    stcTIM6BaseCntCfg.u32CntDir = TMR6_CNT_INCREASE;
    stcTIM6BaseCntCfg.u32CntClkDiv = TMR6_CLK_PCLK0_DIV1024;
    stcTIM6BaseCntCfg.u32CntStpAftOvf = TMR6_CNT_CONTINUOUS;
    (void)TMR6_Init(TMR6_CAPUTURE_UNIT, &stcTIM6BaseCntCfg);

    /* Period register set */
    u32Period = 0xFFFFu;
    TMR6_SetPeriodReg(TMR6_CAPUTURE_UNIT, TMR6_PERIOD_REG_A, u32Period);

    /* Capture input port configuration */
    stcTIM6PortInCfg.u32PortMode = TMR6_PORT_CAPTURE_INPUT;
    stcTIM6PortInCfg.u32FilterSta = TMR6_PORT_INPUT_FILTER_ON;
    stcTIM6PortInCfg.u32FltClk = TMR6_INPUT_FILTER_PCLK0_DIV64;
    (void)TMR6_PortInputConfig(TMR6_CAPUTURE_UNIT,TMR6_IO_PWMA, &stcTIM6PortInCfg);

    /* Hardware capture: Timer6 PWMA port rise trig*/
    TMR6_HwCaptureChACondCmd(TMR6_CAPUTURE_UNIT, TMR6_HW_CTRL_PWMA_FAILLING, Enable);
    /* HW Clear: Timer6 PWMA port fall trig and function command */
    TMR6_HwClrCondCmd(TMR6_CAPUTURE_UNIT, TMR6_HW_CTRL_PWMA_FAILLING, Enable);
    TMR6_HwClrFuncCmd(TMR6_CAPUTURE_UNIT, Enable);

    /* config interrupt */
    /* Enable M4_TMR6_2 GCMAR interrupt */
    TMR6_IntCmd(TMR6_CAPUTURE_UNIT, TMR6_IRQ_EN_CNT_MATCH_A, Enable);

    stcIrqRegiConf.enIRQn = TMR6_CAPUTURE_IRQN;                    /* Register INT_TMR6_2_GCMA Int to Vect.No.003 */
    stcIrqRegiConf.enIntSrc = TMR6_CAPUTURE_INTSRC;              /* Select Event interrupt of M4_TMR6_2 */
    if(irq_caputure_cb != NULL)
        stcIrqRegiConf.pfnCallback = irq_caputure_cb; /* Callback function */
    (void)INTC_IrqSignIn(&stcIrqRegiConf);                  /* Registration IRQ */

    NVIC_ClearPendingIRQ(stcIrqRegiConf.enIRQn);            /* Clear Pending */
    NVIC_SetPriority(stcIrqRegiConf.enIRQn, DDL_IRQ_PRIORITY_04);/* Set priority */
    NVIC_EnableIRQ(stcIrqRegiConf.enIRQn);                  /* Enable NVIC */

    /* Start timer6 */
    TMR6_SwSyncStart(TMR6_SOFT_SYNC_CTRL_U8);
}


uint32_t hal_fan_capture_val_get(void)
{
    return TMR6_GetGenCmpReg(TMR6_CAPUTURE_UNIT, TMR6_CMP_REG_A);
}

//???
void hal_timer_encoder_cnt_init(void)
{

    stc_tmr6_basecnt_cfg_t         stcTIM6BaseCntCfg;
    stc_tmr6_port_input_cfg_t      stcTIM6PortInCfg;

    (void)TMR6_BaseCntStructInit(&stcTIM6BaseCntCfg);
    (void)TMR6_PortInputStructInit(&stcTIM6PortInCfg);

    PWC_Fcg2PeriphClockCmd(PWC_FCG2_TMR6_1, Enable);

    /* Timer6 PWM port configuration */
    GPIO_SetFunc(TMR6_1_PWMA_PORT, TMR6_1_PWMA_PIN, GPIO_FUNC_3_TIM61_PWMA, PIN_SUBFUNC_DISABLE);
    GPIO_SetFunc(TMR6_1_PWMB_PORT, TMR6_1_PWMB_PIN, GPIO_FUNC_3_TIM61_PWMB, PIN_SUBFUNC_DISABLE);
    TMR6_DeInit(TMR6_ENCODER_UNIT);
    /* Timer6 general count function configuration */
    stcTIM6BaseCntCfg.u32CntMode = TMR6_MODE_SAWTOOTH;
    stcTIM6BaseCntCfg.u32CntDir = TMR6_CNT_INCREASE;
    stcTIM6BaseCntCfg.u32CntClkDiv = TMR6_CLK_PCLK0_DIV1024;
    stcTIM6BaseCntCfg.u32CntStpAftOvf = TMR6_CNT_CONTINUOUS;
    (void)TMR6_Init(TMR6_ENCODER_UNIT, &stcTIM6BaseCntCfg);

    /* Capture input port configuration */
    stcTIM6PortInCfg.u32PortMode = TMR6_PORT_CAPTURE_INPUT;
    stcTIM6PortInCfg.u32FilterSta = TMR6_PORT_INPUT_FILTER_ON;
    stcTIM6PortInCfg.u32FltClk = TMR6_INPUT_FILTER_PCLK0_DIV16;
    (void)TMR6_PortInputConfig(TMR6_ENCODER_UNIT,TMR6_IO_PWMA, &stcTIM6PortInCfg);

    (void)TMR6_PortInputConfig(TMR6_ENCODER_UNIT,TMR6_IO_PWMB, &stcTIM6PortInCfg);
    TMR6_SetUpdateReg(TMR6_ENCODER_UNIT,1000);
    TMR6_HwIncreaseCondClr(TMR6_ENCODER_UNIT);
    TMR6_HwDecreaseCondClr(TMR6_ENCODER_UNIT);
    TMR6_HwIncreaseCondCmd(TMR6_ENCODER_UNIT, TMR6_HW_CNT_PWMAL_PWMBFAILLING, Enable); /* PWMA Rising trigger when PWMB is high level */
    TMR6_HwIncreaseCondCmd(TMR6_ENCODER_UNIT, TMR6_HW_CNT_PWMAH_PWMBRISING, Enable);
    TMR6_HwDecreaseCondCmd(TMR6_ENCODER_UNIT, TMR6_HW_CNT_PWMAL_PWMBRISING,   Enable);
    TMR6_HwDecreaseCondCmd(TMR6_ENCODER_UNIT, TMR6_HW_CNT_PWMAH_PWMBFAILLING, Enable);

    TMR6_SetCntReg(TMR6_ENCODER_UNIT, 1000);
    TMR6_CountCmd(TMR6_ENCODER_UNIT, Enable); 

}

//void hal_encoder_tmr_start(void)
//{ 	 
//	TMR6_CountCmd(TMR6_ENCODER_UNIT, Enable); 
//}
//void hal_encoder_tmr_stop(void)
//{
//	TMR6_CountCmd(TMR6_ENCODER_UNIT, Disable); 
//}

int32_t hal_get_encoder_cut(void)
{
	static int32_t u32Timer6Cnt0;
	u32Timer6Cnt0 =  (uint32_t)TMR6_GetCntReg(TMR6_ENCODER_UNIT) -1000 ;
	return u32Timer6Cnt0;
}

//void hal_set_code_cut(uint32_t u32Timer6Cnt0)
//{
//	TMR6_CountCmd(TMR6_ENCODER_UNIT, Disable); 
//	TMR6_SetCntReg(TMR6_ENCODER_UNIT, u32Timer6Cnt0);
//	TMR6_CountCmd(TMR6_ENCODER_UNIT, Enable); 	
//}

void hal_encoder_tmr_clear(void)
{	
    TMR6_SwSyncUpdate(TMR6_SOFT_SYNC_CTRL_U1);
}

/**********************************Timer 2(1ms?????)******************************************/
static void (*timer_1ms_cbk)(void);

static void tmr_1ms_irqcallback(void);
/**
 * @brief  Timer2 configuration.
 * @param  None
 * @retval None
 */
void hal_tmr_1ms_init(void (*irq_cb)(void))
{
    stc_tmr2_init_t stcInit;
    stc_irq_signin_config_t stcCfg;

    /* 1. Enable Timer2 peripheral clock. */
    PWC_Fcg2PeriphClockCmd(APP_TMR2_PERIP_CLK, Enable);

    /* 2. Set a default initialization value for stcInit. */
    (void)TMR2_StructInit(&stcInit);
    /* 3. Modifies the initialization values depends on the application. */
    stcInit.u32ClkSrc = APP_TMR2_CLK_SRC;
    stcInit.u32ClkDiv = APP_TMR2_CLK_DIV;
    stcInit.u32CmpVal = APP_TMR2_CMP_VAL;
    (void)TMR2_Init(APP_TMR2_UNIT, APP_TMR2_CH, &stcInit);
    
    /*IRQ Handle*/
    stcCfg.enIntSrc    = APP_TMR2_INT_SRC;
    stcCfg.enIRQn      = APP_TMR2_IRQn;
    timer_1ms_cbk = irq_cb;
    stcCfg.pfnCallback = tmr_1ms_irqcallback;
    (void)INTC_IrqSignIn(&stcCfg);

    NVIC_ClearPendingIRQ(stcCfg.enIRQn);
    NVIC_SetPriority(stcCfg.enIRQn, APP_TMR2_INT_PRIO);
    NVIC_EnableIRQ(stcCfg.enIRQn);

    /* Enable the specified interrupts of Timer2. */
    TMR2_IntCmd(APP_TMR2_UNIT, APP_TMR2_CH, APP_TMR2_INT_TYPE, Enable);
	/* Starts Timer2. */
    TMR2_Start(APP_TMR2_UNIT, APP_TMR2_CH);
}

/**
 * @brief  1ms???????
 * @param  None
 * @retval None
 */
static void tmr_1ms_irqcallback(void)
{
    if (TMR2_GetStatus(APP_TMR2_UNIT, APP_TMR2_CH, TMR2_FLAG_CMP) == Set)
    {
        TMR2_ClrStatus(APP_TMR2_UNIT, APP_TMR2_CH, TMR2_FLAG_CMP);
        if(timer_1ms_cbk != NULL)
            timer_1ms_cbk();
    }
}


