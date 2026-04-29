#include "bsp_fan.h"
#include "hc32_ddl.h"

#define APP_TMRA_UNIT                   (M4_TMRA_12)
#define APP_TMRA_PERIP_CLK              (PWC_FCG2_TMRA_12)

/*******************************************************************************
 * Global variable definitions (declared in header file with 'extern')
 ******************************************************************************/

/*******************************************************************************
 * Local function prototypes ('static')
 ******************************************************************************/

static void Fan_TmrAConfig(void);
static void Water_TmrAConfig(void);
static void Fan_TimerA_Capture_Init(void);
volatile uint16_t TimeraOverflowNum = 0;
static void Fan_TmrAIrqConfig(void);
static void Water_TMRA_Cmp_IrqCallback(void);
static void Water_TmrAIrqConfig(void);
static void Water_TimerA_Capture_Init(void);

void (*Fan_Tim_Ch_Ic_CallBack)(void) = NULL;		//风扇 转速检测
void (*Water_Tim_Ch_Ic_CallBack)(void) = NULL;		//水泵 转速检测

/**
 * @brief  TimerA counter compare match interrupt callback function.
 * @param  None
 * @retval None
 */
uint16_t fan_speed;
static void Fan_TMRA_Cmp_IrqCallback(void)
{
    if (TMRA_GetStatus(FAN_TMRA_CAP_UINT, FAN_TMRA_CAP_FLAG) == Set)
    {
        /* A capture occurred */
		//Fan1_Speed_Calculate(&fan_speed);
		if(NULL != Fan_Tim_Ch_Ic_CallBack)
			Fan_Tim_Ch_Ic_CallBack();
        /* Get capture value by calling function TMRA_GetCmpVal. */
        TMRA_ClrStatus(FAN_TMRA_CAP_UINT, FAN_TMRA_CAP_FLAG);
    }
}

//static void Fan_TMRA_Ovf_IrqCallback(void)
//{
//    if (TMRA_GetStatus(FAN_TMRA_CAP_UINT, TMRA_FLAG_OVF) == Set)
//    {
//        /* A capture occurred */
//		//++TimeraOverflowNum;
//        /* Get capture value by calling function TMRA_GetCmpVal. */
//        TMRA_ClrStatus(FAN_TMRA_CAP_UINT, TMRA_FLAG_OVF);
//    }
//}

/**
 * @brief  TimerA counter compare match interrupt callback function.
 * @param  None
 * @retval None
 */
//uint16_t fan_speed;
static void Water_TMRA_Cmp_IrqCallback(void)
{
    if (TMRA_GetStatus(WATER_TMRA_CAP_UINT, WATER_TMRA_CAP_FLAG) == Set)
    {
        /* A capture occurred */
		//Water1_Speed_Calculate(&fan_speed);
		if(NULL != Water_Tim_Ch_Ic_CallBack)
			Water_Tim_Ch_Ic_CallBack();
        /* Get capture value by calling function TMRA_GetCmpVal. */
        TMRA_ClrStatus(WATER_TMRA_CAP_UINT, WATER_TMRA_CAP_FLAG);
    }
}

//static void Water_TMRA_Ovf_IrqCallback(void)
//{
//    if (TMRA_GetStatus(WATER_TMRA_CAP_UINT, TMRA_FLAG_OVF) == Set)
//    {
//        /* A capture occurred */
//		//++TimeraOverflowNum;
//        /* Get capture value by calling function TMRA_GetCmpVal. */
//        TMRA_ClrStatus(WATER_TMRA_CAP_UINT, TMRA_FLAG_OVF);
//    }
//}

/**
 * @brief  TimerA configuration.
 * @param  None
 * @retval None
 */
static void Fan_TmrAConfig(void)
{
    stc_tmra_pwm_cfg_t stcCfg;
	
    /* 4. Set the comparison reference value. */
    TMRA_SetCmpVal(APP_TMRA_UNIT, FAN_TMRA_PWM_A_CH, FAN_TMRA_PWM_A_CMP_VAL);
    GPIO_SetFunc(FAN_TMRA_PWM_A_PORT, FAN_TMRA_PWM_A_PIN, FAN_TMRA_PWM_A_PIN_FUNC, PIN_SUBFUNC_DISABLE);
    (void)TMRA_PWM_StructInit(&stcCfg);
	stcCfg.u32CmpPolarity = TMRA_PWM_CMP_LOW;
    (void)TMRA_PWM_Config(APP_TMRA_UNIT, FAN_TMRA_PWM_A_CH, &stcCfg);
    TMRA_PWM_Cmd(APP_TMRA_UNIT, FAN_TMRA_PWM_A_CH, Enable);
}

static void Water_TmrAConfig(void)
{
    stc_tmra_pwm_cfg_t stcCfg;

    /* 4. Set the comparison reference value. */
    TMRA_SetCmpVal(APP_TMRA_UNIT, WATER_TMRA_PWM_A_CH, WATER_TMRA_PWM_A_CMP_VAL);
    GPIO_SetFunc(WATER_TMRA_PWM_A_PORT, WATER_TMRA_PWM_A_PIN, WATER_TMRA_PWM_A_PIN_FUNC, PIN_SUBFUNC_DISABLE);
    (void)TMRA_PWM_StructInit(&stcCfg);
	stcCfg.u32CmpPolarity = TMRA_PWM_CMP_LOW;
    (void)TMRA_PWM_Config(APP_TMRA_UNIT, WATER_TMRA_PWM_A_CH, &stcCfg);
    TMRA_PWM_Cmd(APP_TMRA_UNIT, WATER_TMRA_PWM_A_CH, Enable);
}

static void Fan_TimerA_Capture_Init(void)
{
    /* 4. Set function mode as capturing mode. */
    TMRA_SetFuncMode(FAN_TMRA_CAP_UINT, FAN_TMRA_CAP_CH, TMRA_FUNC_CAPTURE);
	GPIO_SetFunc(FAN_TMRA_CAPT_PWM_PORT, FAN_TMRA_CAPT_PWM_PIN, FAN_TMRA_CAPT_PWM_PIN_FUNC, PIN_SUBFUNC_DISABLE);
	TMRA_SetCaptCond(FAN_TMRA_CAP_UINT, FAN_TMRA_CAP_CH, FAN_CAPTURE_CONDITION);
	/* 6. Configures IRQ if needed. */
    Fan_TmrAIrqConfig();
}

/**
 * @brief  TimerA interrupt configuration.
 * @param  None
 * @retval None
 */
static void Fan_TmrAIrqConfig(void)
{
    stc_irq_signin_config_t stcCfg;

    stcCfg.enIntSrc    = FAN_TMRA_CAP_INT_SRC;
    stcCfg.enIRQn      = FAN_TMRA_IRQn;
    stcCfg.pfnCallback = &Fan_TMRA_Cmp_IrqCallback;
    (void)INTC_IrqSignIn(&stcCfg);

    NVIC_ClearPendingIRQ(stcCfg.enIRQn);
    NVIC_SetPriority(stcCfg.enIRQn, FAN_TMRA_CAP_INT_PRIO);
    NVIC_EnableIRQ(stcCfg.enIRQn);

    /* Enable the specified interrupts of TimerA. */
    TMRA_IntCmd(FAN_TMRA_CAP_UINT, FAN_TMRA_CAP_INT_TYPE, Enable);
}

static void Water_TimerA_Capture_Init(void)
{
    /* 4. Set function mode as capturing mode. */
    TMRA_SetFuncMode(WATER_TMRA_CAP_UINT, WATER_TMRA_CAP_CH, TMRA_FUNC_CAPTURE);
	GPIO_SetFunc(WATER_TMRA_CAPT_PWM_PORT, WATER_TMRA_CAPT_PWM_PIN, WATER_TMRA_CAPT_PWM_PIN_FUNC, PIN_SUBFUNC_DISABLE);
	TMRA_SetCaptCond(WATER_TMRA_CAP_UINT, WATER_TMRA_CAP_CH, WATER_CAPTURE_CONDITION);
	/* 6. Configures IRQ if needed. */
    Water_TmrAIrqConfig();
}

/**
 * @brief  TimerA interrupt configuration.
 * @param  None
 * @retval None
 */
static void Water_TmrAIrqConfig(void)
{
    stc_irq_signin_config_t stcCfg;

    stcCfg.enIntSrc    = WATER_TMRA_CAP_INT_SRC;
    stcCfg.enIRQn      = WATER_TMRA_IRQn;
    stcCfg.pfnCallback = &Water_TMRA_Cmp_IrqCallback;
    (void)INTC_IrqSignIn(&stcCfg);

    NVIC_ClearPendingIRQ(stcCfg.enIRQn);
    NVIC_SetPriority(stcCfg.enIRQn, WATER_TMRA_CAP_INT_PRIO);
    NVIC_EnableIRQ(stcCfg.enIRQn);

    /* Enable the specified interrupts of TimerA. */
    TMRA_IntCmd(WATER_TMRA_CAP_UINT, WATER_TMRA_CAP_INT_TYPE, Enable);
}

void Fan_SetCmpVal(uint32_t value)
{
	 TMRA_SetCmpVal(APP_TMRA_UNIT, FAN_TMRA_PWM_A_CH, value);
}

void Water_SetCmpVal(uint32_t value)
{
	TMRA_SetCmpVal(APP_TMRA_UNIT, WATER_TMRA_PWM_A_CH, value);
}


static void Bsp_Water_Init(void)
{

	Water_TmrAConfig();
	Water_TimerA_Capture_Init();
}

static void Bsp_Fan_Init(void)
{   
    Fan_TmrAConfig();
	Fan_TimerA_Capture_Init();
}
 
void Bsp_Fan_Water_Init(void)
{
	stc_tmra_init_t stcInit;
	/* 1. Enable TimerA peripheral clock. */
    PWC_Fcg2PeriphClockCmd(APP_TMRA_PERIP_CLK, Enable);

    /* 2. Set a default initialization value for stcInit. */
    (void)TMRA_StructInit(&stcInit);

    /* 3. Modifies the initialization values depends on the application. */
    stcInit.u32CntDir    = FAN_TMRA_DIR;
    stcInit.u32CntMode   = FAN_TMRA_MODE;
    stcInit.u32PeriodVal = FAN_TMRA_PERIOD_VAL;
    (void)TMRA_Init(APP_TMRA_UNIT, &stcInit);

		Bsp_Fan_Init();
		Bsp_Water_Init();
	
    TMRA_Start(APP_TMRA_UNIT);
}

