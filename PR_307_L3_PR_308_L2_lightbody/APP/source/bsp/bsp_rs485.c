#include "bsp_rs485.h"
#include "hc32_ddl.h"
#include "define.h"
#include "string.h"

typedef struct{
	uint8_t rx_buff[RS485_RX_BUFF_LEN];
	uint16_t rx_len;
}RS485_RX;
static RS485_RX g_rs485_sturct;
 /* DMA Unit/Channel/Interrupt definition */
#define DMA_RX_UNIT                        (M4_DMA1)
#define DMA_RX_CH                          (DMA_CH5)
#define DMA_RX_BTC_INT                     (DMA_BTC_INT_CH5)
#define DMA_RX_BTC_INT_SRC                 (INT_DMA1_BTC5)
#define DMA_RX_BTC_INT_IRQn                (Int010_IRQn)
#define DMA_RX_FUNCTION_CLK_GATE           (PWC_FCG0_DMA1)
#define DMA_RX_TRIGGER_SOURCE              (EVT_USART1_RI)

#define DMA_TX_UNIT                        (M4_DMA1)
#define DMA_TX_CH                          (DMA_CH4)
#define DMA_TX_BTC_INT                     (DMA_TC_INT_CH4)
#define DMA_TX_BTC_INT_SRC                 (INT_DMA1_TC4)
#define DMA_TX_BTC_INT_IRQn                (Int011_IRQn)
#define DMA_TX_FUNCTION_CLK_GATE           (PWC_FCG0_DMA1)
#define DMA_TX_TRIGGER_SOURCE              (EVT_USART1_TI)

/* Timer0 unit & channel definition */
#define TMR0_UNIT                       (M4_TMR0_1)
#define TMR0_CH                         (TMR0_CH_A)
#define TMR0_FUNCTION_CLK_GATE          (PWC_FCG2_TMR0_1)

/* UART unit definition */
#define USART_UNIT                      (M4_USART1)
#define USART_BAUDRATE                  (250000UL)
#define USART_FUNCTION_CLK_GATE         (PWC_FCG3_USART1)

/* UART unit interrupt definition */
#define USART_TC_INT_SRC             	(INT_USART1_TCI)
#define USART_TC_INT_IRQn            	(Int012_IRQn)

#define USART_RXTO_INT_SRC              (INT_USART1_RTO)
#define USART_RXTO_INT_IRQn             (Int013_IRQn)

#define USART_RXERR_INT_SRC              (INT_USART1_EI)
#define USART_RXERR_INT_IRQn             (Int001_IRQn)

/* UART RX/TX Port/Pin definition */
#define USART_RX_PORT                   (GPIO_PORT_I) 
#define USART_RX_PIN                    (GPIO_PIN_07)
#define USART_RX_GPIO_FUNC              (GPIO_FUNC_33_USART1_RX)

#define USART_TX_PORT                   (GPIO_PORT_I)   
#define USART_TX_PIN                    (GPIO_PIN_06)
#define USART_TX_GPIO_FUNC              (GPIO_FUNC_32_USART1_TX)

#define RX485_CTRL_PROT                 (GPIO_PORT_I)
#define RX485_CTRL_PIN                  (GPIO_PIN_05)


/*******************************************************************************
 * Local function prototypes ('static')
 ******************************************************************************/
static en_result_t DMA_Rx_Config(void);
static void TMR0_Config(void);
static void USART_RxErr_IrqCallback(void);
static void USART_RxTimeout_IrqCallback(void);
static void dma_tx_tc_irqcallback(void);
static void set_rx485_ctrl_pin(uint8_t state);
/*******************************************************************************
 * Local variable definitions ('static')
 ******************************************************************************/

void (*g_pRs485TxCallback)(void);
void (*g_pRs485RxCallback)(uint8_t *pBuff,uint16_t Length);
void (*g_pRsBaseDataTxCallback)(void);
void (*g_pRsBaseDataRxCallback)(uint8_t *pBuff,uint16_t Length);
/*******************************************************************************
 * Function implementation - global ('extern') and local ('static')
 ******************************************************************************/

/**
 * @brief  Initialize DMA.
 * @param  None
 * @retval en_result_t
 */
static en_result_t DMA_Rx_Config(void)
{
    en_result_t enRet;
    stc_dma_init_t stcDmaInit;

    /* DMA&AOS FCG enable */
    PWC_FCG0_Unlock();
    PWC_Fcg0PeriphClockCmd((DMA_RX_FUNCTION_CLK_GATE | PWC_FCG0_AOS), Enable);
    PWC_FCG0_Lock();

    (void)DMA_StructInit(&stcDmaInit);
    stcDmaInit.u32IntEn = DMA_INT_DISABLE;						//中断												
    stcDmaInit.u32BlockSize = 1UL;								//1block
    stcDmaInit.u32TransCnt = RS485_RX_BUFF_LEN;					//传输长度
    stcDmaInit.u32DataWidth = DMA_DATAWIDTH_8BIT;				//8bit
    stcDmaInit.u32DestAddr = (uint32_t)(g_rs485_sturct.rx_buff);	//目的地址
    stcDmaInit.u32SrcAddr = ((uint32_t)(&USART_UNIT->DR) + 2UL);//源地址 串口DR寄存器
    stcDmaInit.u32SrcInc = DMA_SRC_ADDR_FIX;					//源地址   固定地址
    stcDmaInit.u32DestInc = DMA_DEST_ADDR_INC;					//目的地址 地址自增
    enRet = DMA_Init(DMA_RX_UNIT, DMA_RX_CH, &stcDmaInit);
    if (Ok == enRet)
    {
        /* DMA module enable */
        DMA_Cmd(DMA_RX_UNIT, Enable);

        /* DMA channel enable */
        DMA_ChannelCmd(DMA_RX_UNIT, DMA_RX_CH, Enable);

        /* Set DMA trigger source */
        DMA_SetTriggerSrc(DMA_RX_UNIT, DMA_RX_CH, DMA_RX_TRIGGER_SOURCE);
    }
    return enRet;
}

static en_result_t DMA_Tx_Config(void)
{
    en_result_t enRet;
    stc_dma_init_t stcDmaInit;
    stc_irq_signin_config_t stcIrqSignConfig;

    /* DMA&AOS FCG enable */
    PWC_FCG0_Unlock();
    PWC_Fcg0PeriphClockCmd((DMA_TX_FUNCTION_CLK_GATE | PWC_FCG0_AOS), Enable);
    PWC_FCG0_Lock();

    (void)DMA_StructInit(&stcDmaInit);
    stcDmaInit.u32IntEn = DMA_INT_ENABLE; 		//DMA_INT_ENABLE;中断使能
    stcDmaInit.u32BlockSize = 1UL;				//块大小
    stcDmaInit.u32TransCnt = 128;				//传输128次
    stcDmaInit.u32DataWidth = DMA_DATAWIDTH_8BIT;//数据宽度8bit
    stcDmaInit.u32DestAddr = ((uint32_t)(&USART_UNIT->DR)); //目的地址TX: (&USART_UNIT->DR)
    stcDmaInit.u32SrcAddr = 0;					//源地址 空     RX: (&USART_UNIT->DR +2U) 
    stcDmaInit.u32SrcInc = DMA_SRC_ADDR_INC;	//源地址依次递增
    stcDmaInit.u32DestInc = DMA_DEST_ADDR_FIX; 	//目的地址固定	DMA_DEST_ADDR_FIX;
    enRet = DMA_Init(DMA_TX_UNIT, DMA_TX_CH, &stcDmaInit);	//初始化dmi
    if (Ok == enRet)
    {
		DMA_ClearTransIntStatus(DMA_TX_UNIT, DMA_TX_CH);

        /* Register interrupt */
		 stcIrqSignConfig.enIntSrc = DMA_TX_BTC_INT_SRC;
		 stcIrqSignConfig.enIRQn  = DMA_TX_BTC_INT_IRQn;
		 stcIrqSignConfig.pfnCallback= &dma_tx_tc_irqcallback;
		 (void)INTC_IrqSignIn(&stcIrqSignConfig);
		 NVIC_ClearPendingIRQ(stcIrqSignConfig.enIRQn);
		 NVIC_SetPriority(stcIrqSignConfig.enIRQn,DDL_IRQ_PRIORITY_10);
		 NVIC_EnableIRQ(stcIrqSignConfig.enIRQn);

        /* DMA module enable */
        DMA_Cmd(DMA_TX_UNIT, Enable);

        /* DMA channel interrupt enable */
        DMA_TransIntCmd(DMA_TX_UNIT, DMA_TX_BTC_INT, Enable);	

        /* DMA channel enable */
        DMA_ChannelCmd(DMA_TX_UNIT, DMA_TX_CH, Enable);

        /* Set DMA trigger source */
        DMA_SetTriggerSrc(DMA_TX_UNIT, DMA_TX_CH, DMA_TX_TRIGGER_SOURCE);
    }

	//DMA_SetSrcAddr(DMA_TX_UNIT, DMA_TX_CH, (uint32_t)"pbuf");

    return enRet;
}

/**
 * @brief  Configure TMR0.
 * @param  None
 * @retval None
 */
static void TMR0_Config(void)
{
    uint32_t u32CmpVal;
    stc_tmr0_init_t stcTmr0Init;

    PWC_Fcg2PeriphClockCmd(TMR0_FUNCTION_CLK_GATE, Enable);

    /* Clear CNTAR register for channel A */
    TMR0_SetCntVal(TMR0_UNIT, TMR0_CH, 0U);

    /* TIMER0 basetimer function initialize */
    (void)TMR0_StructInit(&stcTmr0Init);
    stcTmr0Init.u32ClockDivision = TMR0_CLK_DIV8;
    stcTmr0Init.u32ClockSource = TMR0_CLK_SRC_XTAL32;
    stcTmr0Init.u32HwTrigFunc = (TMR0_BT_HWTRG_FUNC_START | TMR0_BT_HWTRG_FUNC_CLEAR);
    if (TMR0_CLK_DIV1 == stcTmr0Init.u32ClockDivision)
    {
        u32CmpVal = (USART_BAUDRATE - 4UL) / 2000;
    }
    else if (TMR0_CLK_DIV2 == stcTmr0Init.u32ClockDivision)
    {
        u32CmpVal = (USART_BAUDRATE/2UL - 2UL) / 2000;
    }
    else
    {
        u32CmpVal = (USART_BAUDRATE / (1UL << (stcTmr0Init.u32ClockDivision >> TMR0_BCONR_CKDIVA_POS)) - 1UL) / 2000;
    }
    DDL_ASSERT(u32CmpVal <= 0xFFFFUL);
    stcTmr0Init.u16CmpValue = (uint16_t)(u32CmpVal);
    (void)TMR0_Init(TMR0_UNIT, TMR0_CH, &stcTmr0Init);

    /* Clear compare flag */
    TMR0_ClearStatus(TMR0_UNIT, TMR0_CH);
}

//DMA发送完成中断
static void dma_tx_tc_irqcallback(void)
{
	DMA_ClearTransIntStatus(DMA_TX_UNIT, DMA_TX_BTC_INT);
	DDL_DelayUS(104);  //等待数据发送完成
    USART_FuncCmd(USART_UNIT, USART_TX, Disable);
	set_rx485_ctrl_pin(0);
}

/**
 * @brief  USART RX timeout IRQ callback.
 * @param  None
 * @retval None
 */
static void USART_RxTimeout_IrqCallback(void)
{
//	set_rx485_ctrl_pin(1);
//	set_rx485_ctrl_pin(0);
    TMR0_Cmd(TMR0_UNIT, TMR0_CH, Disable);
    USART_ClearStatus(USART_UNIT, USART_CLEAR_FLAG_RTOF);
	DMA_ChannelCmd(DMA_RX_UNIT, DMA_RX_CH, Disable);
//    memcpy(&data_arry[0], &uart_buf[0], sizeof(data_arry));
    g_rs485_sturct.rx_len = RS485_RX_BUFF_LEN - DMA_GetTransCnt(DMA_RX_UNIT, DMA_RX_CH);
	g_str_rs485_dma.rx_len = g_rs485_sturct.rx_len;
	g_str_rs485_dma.rx_idle_flag = 1;
	memcpy(g_str_rs485_dma.rx_buff, g_rs485_sturct.rx_buff, RS485_RX_BUFF_LEN);
	memset(g_rs485_sturct.rx_buff, 0, RS485_RX_BUFF_LEN);
	//printf("\r\nble recv %d %s\r\n",BleDMARecieve.len,BleDMARecieve.data);
    DMA_SetDestAddr(DMA_RX_UNIT, DMA_RX_CH, (uint32_t)(&g_rs485_sturct.rx_buff[0]));
    DMA_SetBlockSize(DMA_RX_UNIT, DMA_RX_CH, 1);
    DMA_SetTransCnt(DMA_RX_UNIT, DMA_RX_CH, RS485_RX_BUFF_LEN);
    DMA_ChannelCmd(DMA_RX_UNIT, DMA_RX_CH, Enable);  
}

static void set_rx485_ctrl_pin(uint8_t state)
{
	if(state)
	{
		 GPIO_SetPins(RX485_CTRL_PROT, RX485_CTRL_PIN);
	}
	else
	{
		 GPIO_ResetPins(RX485_CTRL_PROT, RX485_CTRL_PIN);
	}	
}

void rx485_send(uint8_t *pData, uint16_t Cnt)
{
	set_rx485_ctrl_pin(1);
    DMA_ChannelCmd(DMA_TX_UNIT, DMA_TX_CH, Disable);
	USART_FuncCmd(USART_UNIT, USART_TX, Disable);
	USART_FuncCmd(USART_UNIT, USART_INT_TXE, Disable);
    DMA_SetSrcAddr(DMA_TX_UNIT, DMA_TX_CH, (uint32_t)pData);
    DMA_SetBlockSize(DMA_TX_UNIT, DMA_TX_CH, 1);
    DMA_SetTransCnt(DMA_TX_UNIT, DMA_TX_CH, Cnt);
    DMA_ChannelCmd(DMA_TX_UNIT, DMA_TX_CH, Enable);  
	USART_FuncCmd(USART_UNIT, USART_TX, Enable);
	USART_FuncCmd(USART_UNIT, USART_INT_TXE, Enable);  
}

/**
 * @brief  USART error IRQ callback function.
 * @param  None
 * @retval None
 */
static void USART_RxErr_IrqCallback(void)
{
    if (Set == USART_GetStatus(USART_UNIT, (USART_FLAG_PE | USART_FLAG_FE)))
    {
        
    }
		(void)USART_RecData(USART_UNIT);
    USART_ClearStatus(USART_UNIT, (USART_CLEAR_FLAG_PE | \
                                 USART_CLEAR_FLAG_FE | \
                                 USART_CLEAR_FLAG_ORE));
}

/**
 * @brief  Main function of UART DMA project
 * @param  None
 * @retval int32_t return value, if needed
 */
void rs485_slave_init(void)
{
    stc_irq_signin_config_t stcIrqSigninCfg;
    const stc_usart_uart_init_t stcUartInit = {
        .u32Baudrate = USART_BAUDRATE,
        .u32BitDirection = USART_LSB,
        .u32StopBit = USART_STOPBIT_1BIT,
        .u32Parity = USART_PARITY_NONE,
        .u32DataWidth = USART_DATA_LENGTH_8BIT,
        .u32ClkMode = USART_INTERNCLK_OUTPUT,
        .u32PclkDiv = USART_PCLK_DIV4,
        .u32OversamplingBits = USART_OVERSAMPLING_8BIT,
        .u32NoiseFilterState = USART_NOISE_FILTER_DISABLE,
        .u32SbDetectPolarity = USART_SB_DETECT_FALLING,
    };

    /* Initialize DMA. */
    DMA_Rx_Config();
	DMA_Tx_Config();
    /* Initialize TMR0. */
    TMR0_Config();

    /* Configure USART RX/TX pin. */
    GPIO_SetFunc(USART_RX_PORT, USART_RX_PIN, USART_RX_GPIO_FUNC, PIN_SUBFUNC_DISABLE);
    GPIO_SetFunc(USART_TX_PORT, USART_TX_PIN, USART_TX_GPIO_FUNC, PIN_SUBFUNC_DISABLE);

    stc_gpio_init_t stcGpioInit;
    (void)GPIO_StructInit(&stcGpioInit);
    stcGpioInit.u16PinDir = PIN_DIR_OUT;
    stcGpioInit.u16PinDrv = PIN_DRV_HIGH;
    (void)GPIO_Init(RX485_CTRL_PROT, RX485_CTRL_PIN, &stcGpioInit);

    /* Enable peripheral clock */
    PWC_Fcg3PeriphClockCmd(USART_FUNCTION_CLK_GATE, Enable);

    /* Initialize UART function. */
    if (Ok != USART_UartInit(USART_UNIT, &stcUartInit))
    {

    }
	
	/* Register RX error IRQ handler && configure NVIC. */
    stcIrqSigninCfg.enIRQn = USART_RXERR_INT_IRQn;
    stcIrqSigninCfg.enIntSrc = USART_RXERR_INT_SRC;
    stcIrqSigninCfg.pfnCallback = &USART_RxErr_IrqCallback;
    (void)INTC_IrqSignIn(&stcIrqSigninCfg);
    NVIC_ClearPendingIRQ(stcIrqSigninCfg.enIRQn);
    NVIC_SetPriority(stcIrqSigninCfg.enIRQn, DDL_IRQ_PRIORITY_DEFAULT);
    NVIC_EnableIRQ(stcIrqSigninCfg.enIRQn);
	
	
    /* Register RX timeout IRQ handler && configure NVIC. */
    stcIrqSigninCfg.enIRQn = USART_RXTO_INT_IRQn;
    stcIrqSigninCfg.enIntSrc = USART_RXTO_INT_SRC;
    stcIrqSigninCfg.pfnCallback = &USART_RxTimeout_IrqCallback;
    (void)INTC_IrqSignIn(&stcIrqSigninCfg);
    NVIC_ClearPendingIRQ(stcIrqSigninCfg.enIRQn);
    NVIC_SetPriority(stcIrqSigninCfg.enIRQn, DDL_IRQ_PRIORITY_10);
    NVIC_EnableIRQ(stcIrqSigninCfg.enIRQn);

    /* Enable TX && RX && RX interrupt function */
    USART_FuncCmd(USART_UNIT, (USART_RX | USART_INT_RX | USART_TX | \
                               USART_RTO | USART_INT_RTO), Enable);
	//使能接收模式				   
	set_rx485_ctrl_pin(0);	
}

void RS485_Recv_Finish_Ctr(void)
{
   if(g_str_rs485_dma.rx_idle_flag == 1)
   {
	   g_str_rs485_dma.rx_idle_flag = 0;
	   if(NULL != g_pRs485RxCallback)
			g_pRs485RxCallback(g_str_rs485_dma.rx_buff, g_str_rs485_dma.rx_len);
   }
}








