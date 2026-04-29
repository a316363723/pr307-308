#include "bsp_usart.h"
#include "hc32_ddl.h"
#include "define.h"
#include "string.h"
#include "bsp_electronic.h"

#define USART2_USART3_RELAY (1UL)
#define USART4_USART5_RELAY (1UL)
uint8_t RS485_2_transfer_complete = 1;
uint8_t RS485_3_transfer_complete = 1;
typedef struct{
	uint8_t rx_buff[RS485_RX_BUFF_LEN];
	uint16_t rx_len;
}RS485_RX;

#define RELAY23_SELECT_PORT                   (GPIO_PORT_C)   
#define RELAY23_SELECT_PIN                    (GPIO_PIN_08)

#define RELAY45_SELECT_PORT                   (GPIO_PORT_C)   
#define RELAY45_SELECT_PIN                    (GPIO_PIN_06)



/* UART unit definition */
#define RELAY45_UNIT                      (M4_USART6)

/* UART unit interrupt definition */
#define RELAY45_RXERR_INT_SRC              	(INT_USART6_EI)
#define RELAY45_RXERR_INT_IRQn             	(Int024_IRQn)

/* UART unit definition */
#define RELAY23_UNIT                      (M4_USART2)

#define RELAY23_RXERR_INT_SRC              	(INT_USART2_EI)
#define RELAY23_RXERR_INT_IRQn             	(Int029_IRQn)
/* UART RX/TX Port/Pin definition */


/**
 * @brief  USART error IRQ callback function.
 * @param  None
 * @retval None
 */
static void Relay45_RxErr_IrqCallback(void)
{
    if (Set == USART_GetStatus(RELAY45_UNIT, (USART_FLAG_PE | USART_FLAG_FE)))
    {
        (void)USART_RecData(RELAY45_UNIT);
    }

    USART_ClearStatus(RELAY45_UNIT, (USART_CLEAR_FLAG_PE | \
                                 USART_CLEAR_FLAG_FE | \
                                 USART_CLEAR_FLAG_ORE));
}

/**
 * @brief  USART error IRQ callback function.
 * @param  None
 * @retval None
 */
static void Relay23_RxErr_IrqCallback(void)
{
    if (Set == USART_GetStatus(RELAY23_UNIT, (USART_FLAG_PE | USART_FLAG_FE)))
    {
        (void)USART_RecData(RELAY23_UNIT);
    }

    USART_ClearStatus(RELAY23_UNIT, (USART_CLEAR_FLAG_PE | \
                                 USART_CLEAR_FLAG_FE | \
                                 USART_CLEAR_FLAG_ORE));
}


#define OTHER_RS485 1
#if OTHER_RS485

#define CB_USART(cb, ...)  do{ if(NULL != cb) cb(__VA_ARGS__); }while(0)

//static void (*p_rx_complete_callback[HAL_USART_NUM])(hal_usart_rx_pack_t* pack);

static hal_usart_rx_pack_t usart_rx_pack[HAL_USART_NUM];

#define Relay2_3	(1)
#define Relay4_5	(1)

#define RELAY23_SELECT_PORT                   (GPIO_PORT_C)   
#define RELAY23_SELECT_PIN                    (GPIO_PIN_08)

#define RELAY45_SELECT_PORT                   (GPIO_PORT_C)   
#define RELAY45_SELECT_PIN                    (GPIO_PIN_06)

#define RS485_2_UART_RXTO_INT_PRIORITY					 	 (DDL_IRQ_PRIORITY_13)
#define RS485_2_DMA_RX_UNIT                        (M4_DMA1)
#define RS485_2_DMA_RX_CH                          (DMA_CH3)
#define RS485_2_DMA_RX_FUNCTION_CLK_GATE           (PWC_FCG0_DMA1)
#define RS485_2_DMA_RX_TRIGGER_SOURCE              (EVT_USART2_RI)

#define RS485_2_DMA_TX_UNIT                        (M4_DMA1)
#define RS485_2_DMA_TX_CH                          (DMA_CH7)
#define RS485_2_DMA_TX_BTC_INT                     (DMA_TC_INT_CH7)
#define RS485_2_DMA_TX_BTC_INT_SRC                 (INT_DMA1_TC7)
#define RS485_2_DMA_TX_BTC_INT_IRQn                (Int026_IRQn)
#define RS485_2_DMA_TX_FUNCTION_CLK_GATE           (PWC_FCG0_DMA1)
#define RS485_2_DMA_TX_TRIGGER_SOURCE              (EVT_USART2_TI)
#define RS485_2_UART_TXTC_INT_PRIORITY				(DDL_IRQ_PRIORITY_13)
/* Timer0 unit & channel definition */
#define RS485_2_TMR0_UNIT                       	 (M4_TMR0_1)
#define RS485_2_TMR0_CH                         	 (TMR0_CH_B)
#define RS485_2_TMR0_CLK        					 (PWC_FCG2_TMR0_1)
/* UART unit definition */
#define RS485_2_USART_UNIT                      (M4_USART2)
#define RS485_2_USART_BAUDRATE                  (250000UL)
#define RS485_2_USART_CLK        				(PWC_FCG3_USART2)
/* UART unit interrupt definition */
#define RS485_2_USART_RXERR_INT_SRC             (INT_USART2_EI)
#define RS485_2_USART_RXERR_INT_IRQn            (Int029_IRQn)

#define RS485_2_USART_RXTO_INT_SRC              (INT_USART2_RTO)
#define RS485_2_USART_RXTO_INT_IRQn             (Int028_IRQn)

#define RS485_2_USART_RX_INT_SRC              	(INT_USART2_RI)
#define RS485_2_USART_RX_INT_IRQn             	(Int007_IRQn)

#define RS485_2_USART_TX_GPIO_FUNC              (GPIO_FUNC_34_USART2_TX)
#define RS485_2_USART_RX_GPIO_FUNC              (GPIO_FUNC_35_USART2_RX)

#if (Relay2_3)

#define RS485_2_USART_RX_PORT                   (GPIO_PORT_H)   
#define RS485_2_USART_RX_PIN                    (GPIO_PIN_15)

#define RS485_2_USART_TX_PORT                   (GPIO_PORT_H)  
#define RS485_2_USART_TX_PIN                    (GPIO_PIN_14)

#define RS485_2_CTRL_GPIO              					 	 (GPIO_PORT_H)
#define RS485_2_CTRL_Pin              					 	 (GPIO_PIN_13)

#else

#define RS485_2_USART_RX_PORT                   (GPIO_PORT_I)   
#define RS485_2_USART_RX_PIN                    (GPIO_PIN_02)

#define RS485_2_USART_TX_PORT                   (GPIO_PORT_I)  
#define RS485_2_USART_TX_PIN                    (GPIO_PIN_01)

#define RS485_2_CTRL_GPIO              					 	 (GPIO_PORT_I)
#define RS485_2_CTRL_Pin              					 	 (GPIO_PIN_00)

#endif

#define RS485_3_UART_RXTO_INT_PRIORITY			   (DDL_IRQ_PRIORITY_14)
#define RS485_3_DMA_RX_UNIT                        (M4_DMA1)
#define RS485_3_DMA_RX_CH                          (DMA_CH2)
#define RS485_3_DMA_RX_FUNCTION_CLK_GATE           (PWC_FCG0_DMA1)
#define RS485_3_DMA_RX_TRIGGER_SOURCE              (EVT_USART6_RI)

#define RS485_3_DMA_TX_UNIT                        (M4_DMA1)
#define RS485_3_DMA_TX_CH                          (DMA_CH1)
#define RS485_3_DMA_TX_BTC_INT                     (DMA_TC_INT_CH1)
#define RS485_3_DMA_TX_BTC_INT_SRC                 (INT_DMA1_TC1)
#define RS485_3_DMA_TX_BTC_INT_IRQn                (Int021_IRQn)
#define RS485_3_DMA_TX_FUNCTION_CLK_GATE           (PWC_FCG0_DMA1)
#define RS485_3_DMA_TX_TRIGGER_SOURCE              (EVT_USART6_TI)
#define RS485_3_UART_TXTC_INT_PRIORITY				(DDL_IRQ_PRIORITY_14)
/* Timer0 unit & channel definition */
#define RS485_3_TMR0_UNIT                       	 (M4_TMR0_2)
#define RS485_3_TMR0_CH                         	 (TMR0_CH_A)
#define RS485_3_TMR0_CLK        					 (PWC_FCG2_TMR0_2)
/* UART unit definition */
#define RS485_3_USART_UNIT                      (M4_USART6)
#define RS485_3_USART_BAUDRATE                  (250000UL)
#define RS485_3_USART_CLK        				(PWC_FCG3_USART6)
/* UART unit interrupt definition */
#define RS485_3_USART_RXERR_INT_SRC             (INT_USART6_EI)
#define RS485_3_USART_RXERR_INT_IRQn            (Int024_IRQn)

#define RS485_3_USART_RXTO_INT_SRC              (INT_USART6_RTO)
#define RS485_3_USART_RXTO_INT_IRQn             (Int023_IRQn)

#define RS485_3_USART_RX_INT_SRC              	(INT_USART6_RI)
#define RS485_3_USART_RX_INT_IRQn             	(Int009_IRQn)

#define RS485_3_USART_RX_GPIO_FUNC              (GPIO_FUNC_37_USART6_RX)
#define RS485_3_USART_TX_GPIO_FUNC              (GPIO_FUNC_36_USART6_TX)

#if (Relay4_5)

#define RS485_3_USART_RX_PORT                   (GPIO_PORT_A)   
#define RS485_3_USART_RX_PIN                    (GPIO_PIN_12)

#define RS485_3_USART_TX_PORT                   (GPIO_PORT_A)  
#define RS485_3_USART_TX_PIN                    (GPIO_PIN_11)


#define RS485_3_CTRL_GPIO              					 	 (GPIO_PORT_A)
#define RS485_3_CTRL_Pin              					 	 (GPIO_PIN_10)

#else

#define RS485_3_USART_RX_PORT                   (GPIO_PORT_A)   
#define RS485_3_USART_RX_PIN                    (GPIO_PIN_09)


#define RS485_3_USART_TX_PORT                   (GPIO_PORT_A)  
#define RS485_3_USART_TX_PIN                    (GPIO_PIN_08)


#define RS485_3_CTRL_GPIO              					 	 (GPIO_PORT_C)
#define RS485_3_CTRL_Pin              					 	 (GPIO_PIN_09)
#endif

void Error_Init(void)
{
	stc_irq_signin_config_t stcIrqSigninCfg;
	stcIrqSigninCfg.enIRQn = RELAY45_RXERR_INT_IRQn;
	stcIrqSigninCfg.enIntSrc = RELAY45_RXERR_INT_SRC;
	stcIrqSigninCfg.pfnCallback = &Relay45_RxErr_IrqCallback;
	(void)INTC_IrqSignIn(&stcIrqSigninCfg);
	NVIC_ClearPendingIRQ(stcIrqSigninCfg.enIRQn);
	NVIC_SetPriority(stcIrqSigninCfg.enIRQn, DDL_IRQ_PRIORITY_DEFAULT);
	NVIC_EnableIRQ(stcIrqSigninCfg.enIRQn);
	
	stcIrqSigninCfg.enIRQn = RELAY23_RXERR_INT_IRQn;
    stcIrqSigninCfg.enIntSrc = RELAY23_RXERR_INT_SRC;
    stcIrqSigninCfg.pfnCallback = &Relay23_RxErr_IrqCallback;
    (void)INTC_IrqSignIn(&stcIrqSigninCfg);
    NVIC_ClearPendingIRQ(stcIrqSigninCfg.enIRQn);
    NVIC_SetPriority(stcIrqSigninCfg.enIRQn, DDL_IRQ_PRIORITY_DEFAULT);
    NVIC_EnableIRQ(stcIrqSigninCfg.enIRQn);
}

void InstalIrqHandler(const stc_irq_signin_config_t *pstcConfig, uint32_t u32Priority)
{
    if (NULL != pstcConfig)
    {
        (void)INTC_IrqSignIn(pstcConfig);
        NVIC_ClearPendingIRQ(pstcConfig->enIRQn);
        NVIC_SetPriority(pstcConfig->enIRQn, u32Priority);
        NVIC_EnableIRQ(pstcConfig->enIRQn);
    }
}

void RS485_2_RxTimeout_IRQHandler(void)
{
	uint8_t DR;
    TMR0_Cmd(RS485_2_TMR0_UNIT, RS485_2_TMR0_CH, Disable);
	
    USART_ClearStatus(RS485_2_USART_UNIT, USART_CLEAR_FLAG_RTOF);
    usart_rx_pack[HAL_USART_RS485_2].length = sizeof(usart_rx_pack[HAL_USART_RS485_2].data) - DMA_GetTransCnt(RS485_2_DMA_RX_UNIT, RS485_2_DMA_RX_CH);
    DMA_ChannelCmd(RS485_2_DMA_RX_UNIT, RS485_2_DMA_RX_CH, Disable);
	
	memset(Electronic_Buf.R_Bracket_Buf.Electronic_Data,0,sizeof(Electronic_Buf.R_Bracket_Buf.Electronic_Data));
	memcpy(Electronic_Buf.R_Bracket_Buf.Electronic_Data,usart_rx_pack[HAL_USART_RS485_2].data,256);//usart_rx_pack[HAL_USART_RS485_2].length);
	Electronic_Buf.R_Bracket_Buf.Electronic_Size = 256;//usart_rx_pack[HAL_USART_RS485_2].length;
	Electronic_Buf.RW_Bracket_State = Electronic_Recive;
	
    DMA_SetDestAddr(RS485_2_DMA_RX_UNIT, RS485_2_DMA_RX_CH, (uint32_t)&usart_rx_pack[HAL_USART_RS485_2].data);
    DMA_SetBlockSize(RS485_2_DMA_RX_UNIT, RS485_2_DMA_RX_CH, 1);
    DMA_SetTransCnt(RS485_2_DMA_RX_UNIT, RS485_2_DMA_RX_CH, sizeof(usart_rx_pack[HAL_USART_RS485_2].data));
    DMA_ChannelCmd(RS485_2_DMA_RX_UNIT, RS485_2_DMA_RX_CH, Enable); 

	DR = USART_RecData(RS485_2_USART_UNIT);
    (void)DR;
}


void RS485_2_TX_DMA_TC_IRQHandler(void)
{	
		DMA_ClearTransIntStatus(RS485_2_DMA_TX_UNIT, RS485_2_DMA_TX_BTC_INT);
		DDL_DelayUS(104);
		hal_rs485_2_write_ctrl_pin(0);
		USART_FuncCmd(RS485_2_USART_UNIT, USART_INT_TXE | USART_INT_TC, Disable);
		RS485_2_transfer_complete = 1;
}

void RS485_3_RxTimeout_IRQHandler(void)
{
	uint8_t DR;
    TMR0_Cmd(RS485_3_TMR0_UNIT, RS485_3_TMR0_CH, Disable);
	
    USART_ClearStatus(RS485_3_USART_UNIT, USART_CLEAR_FLAG_RTOF);
    usart_rx_pack[HAL_USART_RS485_3].length = sizeof(usart_rx_pack[HAL_USART_RS485_3].data) - DMA_GetTransCnt(RS485_3_DMA_RX_UNIT, RS485_3_DMA_RX_CH);
    DMA_ChannelCmd(RS485_3_DMA_RX_UNIT, RS485_3_DMA_RX_CH, Disable);
	
	memset(Electronic_Buf.R_Bayont_Buf.Electronic_Data,0,sizeof(Electronic_Buf.R_Bayont_Buf.Electronic_Data));
	memcpy(Electronic_Buf.R_Bayont_Buf.Electronic_Data,usart_rx_pack[HAL_USART_RS485_3].data,256);//usart_rx_pack[HAL_USART_RS485_3].length);
	Electronic_Buf.R_Bayont_Buf.Electronic_Size = 256;//usart_rx_pack[HAL_USART_RS485_3].length;
	Electronic_Buf.RW_Bayont_State = Electronic_Recive;
	
    DMA_SetDestAddr(RS485_3_DMA_RX_UNIT, RS485_3_DMA_RX_CH, (uint32_t)&usart_rx_pack[HAL_USART_RS485_3].data);
    DMA_SetBlockSize(RS485_3_DMA_RX_UNIT, RS485_3_DMA_RX_CH, 1);
    DMA_SetTransCnt(RS485_3_DMA_RX_UNIT, RS485_3_DMA_RX_CH, sizeof(usart_rx_pack[HAL_USART_RS485_3].data));
    DMA_ChannelCmd(RS485_3_DMA_RX_UNIT, RS485_3_DMA_RX_CH, Enable); 
//	CB_USART(p_rx_complete_callback[HAL_USART_RS485_3], &usart_rx_pack[HAL_USART_RS485_3]);
	DR = USART_RecData(RS485_3_USART_UNIT);
    (void)DR;
}


void RS485_3_TX_DMA_TC_IRQHandler(void)
{	
		DMA_ClearTransIntStatus(RS485_3_DMA_TX_UNIT, RS485_3_DMA_TX_BTC_INT);
		DDL_DelayUS(104);
		hal_rs485_4_write_ctrl_pin(0);
		USART_FuncCmd(RS485_3_USART_UNIT, USART_INT_TXE | USART_INT_TC, Disable);
		RS485_3_transfer_complete = 1;
}

typedef struct TMR0_Para{
	M4_TMR0_TypeDef* TMR0x;
	uint32_t u32Fcg2Periph;
	uint8_t u8Channel;
}TMR0_AB;

TMR0_AB TMR0_1_B = 
{
	.TMR0x = RS485_2_TMR0_UNIT,
	.u32Fcg2Periph = RS485_2_TMR0_CLK,
	.u8Channel = RS485_2_TMR0_CH
};

TMR0_AB TMR0_2_A = 
{
	.TMR0x = RS485_3_TMR0_UNIT,
	.u32Fcg2Periph = RS485_3_TMR0_CLK,
	.u8Channel = RS485_3_TMR0_CH
};


static void TMR0_AB_Config(TMR0_AB tmrab)
{
    uint32_t u32CmpVal;
    stc_tmr0_init_t stcTmr0Init;

    PWC_Fcg2PeriphClockCmd(tmrab.u32Fcg2Periph, Enable);

    /* Clear CNTAR register for channel A */
    TMR0_SetCntVal(tmrab.TMR0x, tmrab.u8Channel, 0U);

    /* TIMER0 basetimer function initialize */
    (void)TMR0_StructInit(&stcTmr0Init);
    stcTmr0Init.u32ClockDivision = TMR0_CLK_DIV8;
    stcTmr0Init.u32ClockSource = TMR0_CLK_SRC_XTAL32;
    stcTmr0Init.u32HwTrigFunc = (TMR0_BT_HWTRG_FUNC_START | TMR0_BT_HWTRG_FUNC_CLEAR);
    if (TMR0_CLK_DIV1 == stcTmr0Init.u32ClockDivision)
    {
        u32CmpVal = (RS485_2_USART_BAUDRATE - 4UL) / 2000;
    }
    else if (TMR0_CLK_DIV2 == stcTmr0Init.u32ClockDivision)
    {
        u32CmpVal = (RS485_2_USART_BAUDRATE/2UL - 2UL) / 2000;
    }
    else
    {
        u32CmpVal = (RS485_2_USART_BAUDRATE / (1UL << (stcTmr0Init.u32ClockDivision >> TMR0_BCONR_CKDIVA_POS)) - 1UL) / 2000;
    }
    DDL_ASSERT(u32CmpVal <= 0xFFFFUL);
    stcTmr0Init.u16CmpValue =  (uint16_t)(u32CmpVal);
    (void)TMR0_Init(tmrab.TMR0x, tmrab.u8Channel, &stcTmr0Init);

    /* Clear compare flag */
    TMR0_ClearStatus(tmrab.TMR0x, tmrab.u8Channel);
}
void hal_rs485_2_usart_send(uint8_t *pData, uint16_t Cnt)
{
		while(RS485_2_transfer_complete != 1);
		RS485_2_transfer_complete = 0;
		hal_rs485_2_write_ctrl_pin(1);
		DMA_ChannelCmd(RS485_2_DMA_TX_UNIT, RS485_2_DMA_TX_CH, Disable);
		USART_FuncCmd(RS485_2_USART_UNIT, USART_TX, Disable);
		USART_FuncCmd(RS485_2_USART_UNIT, USART_INT_TXE, Disable);
		DMA_SetSrcAddr(RS485_2_DMA_TX_UNIT, RS485_2_DMA_TX_CH, (uint32_t)pData);
		DMA_SetBlockSize(RS485_2_DMA_TX_UNIT, RS485_2_DMA_TX_CH, 1);
		DMA_SetTransCnt(RS485_2_DMA_TX_UNIT, RS485_2_DMA_TX_CH, Cnt);
		DMA_ChannelCmd(RS485_2_DMA_TX_UNIT, RS485_2_DMA_TX_CH, Enable);
		USART_FuncCmd(RS485_2_USART_UNIT, USART_TX, Enable);
		USART_FuncCmd(RS485_2_USART_UNIT, USART_INT_TXE, Enable);    
}

void hal_rs485_3_usart_send(uint8_t *pData, uint16_t Cnt)
{
		while(RS485_3_transfer_complete != 1);
		RS485_3_transfer_complete = 0;
		hal_rs485_4_write_ctrl_pin(1);
		DMA_ChannelCmd(RS485_3_DMA_TX_UNIT, RS485_3_DMA_TX_CH, Disable);
		USART_FuncCmd(RS485_3_USART_UNIT, USART_TX, Disable);
		USART_FuncCmd(RS485_3_USART_UNIT, USART_INT_TXE, Disable);
		DMA_SetSrcAddr(RS485_3_DMA_TX_UNIT, RS485_3_DMA_TX_CH, (uint32_t)pData);
		DMA_SetBlockSize(RS485_3_DMA_TX_UNIT, RS485_3_DMA_TX_CH, 1);
		DMA_SetTransCnt(RS485_3_DMA_TX_UNIT, RS485_3_DMA_TX_CH, Cnt);
		DMA_ChannelCmd(RS485_3_DMA_TX_UNIT, RS485_3_DMA_TX_CH, Enable);
		USART_FuncCmd(RS485_3_USART_UNIT, USART_TX, Enable);
		USART_FuncCmd(RS485_3_USART_UNIT, USART_INT_TXE, Enable);    
}

receive rs485_2_rx = {
	.ch = HAL_USART_RS485_2,
	.USARTx = RS485_2_USART_UNIT,
	.DMAx = RS485_2_DMA_RX_UNIT,
	.DMA_CH = RS485_2_DMA_RX_CH,
	.enintsrc = RS485_2_USART_RXTO_INT_SRC,
	.enirq = RS485_2_USART_RXTO_INT_IRQn,
	.priority = RS485_2_UART_RXTO_INT_PRIORITY,
	.event_src = RS485_2_DMA_RX_TRIGGER_SOURCE,
	.pfncallback = RS485_2_RxTimeout_IRQHandler
};

receive rs485_3_rx = {
	.ch = HAL_USART_RS485_3,
	.USARTx = RS485_3_USART_UNIT,
	.DMAx = RS485_3_DMA_RX_UNIT,
	.DMA_CH = RS485_3_DMA_RX_CH,
	.enintsrc = RS485_3_USART_RXTO_INT_SRC,
	.enirq = RS485_3_USART_RXTO_INT_IRQn,
	.priority = RS485_3_UART_RXTO_INT_PRIORITY,
	.event_src = RS485_3_DMA_RX_TRIGGER_SOURCE,
	.pfncallback = RS485_3_RxTimeout_IRQHandler
};

//DMA init
void hal_rs485_usart_receive_dma_start(receive USART_DMA, TMR0_AB TMRx_AB, void(*rx_complete_irq_callback)(hal_usart_rx_pack_t* pack))
{
			en_result_t enRet;
			stc_dma_init_t stcDmaInit;
			DMA_StructInit(&stcDmaInit);
			stc_irq_signin_config_t stcIrqSigninCfg;
			
//			p_rx_complete_callback[USART_DMA.ch] = rx_complete_irq_callback;
			
			DMA_StructInit(&stcDmaInit);
			stcDmaInit.u32IntEn = DMA_INT_ENABLE;
			stcDmaInit.u32BlockSize = 1UL;
			stcDmaInit.u32TransCnt = 1024;
			stcDmaInit.u32DataWidth = DMA_DATAWIDTH_8BIT;
			stcDmaInit.u32DestAddr = (uint32_t)&usart_rx_pack[USART_DMA.ch].data;
			stcDmaInit.u32SrcAddr = ((uint32_t)(&USART_DMA.USARTx->DR) + 2UL);
			stcDmaInit.u32SrcInc = DMA_SRC_ADDR_FIX;
			stcDmaInit.u32DestInc = DMA_DEST_ADDR_INC;
			enRet = DMA_Init(USART_DMA.DMAx, USART_DMA.DMA_CH, &stcDmaInit);
			if (Ok == enRet)
			{
					/* Register RX timeout IRQ handler && configure NVIC. */
					stcIrqSigninCfg.enIRQn = USART_DMA.enirq; 
					stcIrqSigninCfg.enIntSrc = USART_DMA.enintsrc ; /*超时中断类型*/
					stcIrqSigninCfg.pfnCallback = USART_DMA.pfncallback;
					InstalIrqHandler(&stcIrqSigninCfg, USART_DMA.priority);
					
					DMA_Cmd(USART_DMA.DMAx, Enable);
					DMA_ChannelCmd(USART_DMA.DMAx, USART_DMA.DMA_CH, Enable);
					DMA_SetTriggerSrc(USART_DMA.DMAx, USART_DMA.DMA_CH, USART_DMA.event_src);
			}
	
			TMR0_AB_Config(TMRx_AB);
			USART_FuncCmd(USART_DMA.USARTx, (USART_RX  | USART_RTO | USART_INT_RTO | USART_CR1_RIE ), Enable); /*1*/
}


transmit rs485_2_tx = {
	.ch = HAL_USART_RS485_2,
	.USARTx = RS485_2_USART_UNIT,
	.DMAx = RS485_2_DMA_TX_UNIT,
	.DMA_CH = RS485_2_DMA_TX_CH,
	.u32transInt = RS485_2_DMA_TX_BTC_INT,
	.enintsrc = RS485_2_DMA_TX_BTC_INT_SRC,
	.enirq = RS485_2_DMA_TX_BTC_INT_IRQn,
	.priority = RS485_2_UART_TXTC_INT_PRIORITY,
	.event_src = RS485_2_DMA_TX_TRIGGER_SOURCE,
	.u8port = RS485_2_USART_TX_PORT,
	.u8pin = RS485_2_USART_TX_PIN,
	.u8func = RS485_2_USART_TX_GPIO_FUNC,
	.pfncallback = RS485_2_TX_DMA_TC_IRQHandler
};

transmit rs485_3_tx = {
	.ch = HAL_USART_RS485_3,
	.USARTx = RS485_3_USART_UNIT,
	.DMAx = RS485_3_DMA_TX_UNIT,
	.DMA_CH = RS485_3_DMA_TX_CH,
	.u32transInt = RS485_3_DMA_TX_BTC_INT,
	.enintsrc = RS485_3_DMA_TX_BTC_INT_SRC,
	.enirq = RS485_3_DMA_TX_BTC_INT_IRQn,
	.priority = RS485_3_UART_TXTC_INT_PRIORITY,
	.event_src = RS485_3_DMA_TX_TRIGGER_SOURCE,
	.u8port = RS485_3_USART_TX_PORT,
	.u8pin = RS485_3_USART_TX_PIN,
	.u8func = RS485_3_USART_TX_GPIO_FUNC,
	.pfncallback = RS485_3_TX_DMA_TC_IRQHandler
};

void hal_rs485_usart_transmit_dma_start(transmit USART_DMA)
{
    en_result_t enRet;
    stc_dma_init_t stcDmaInit;
    stc_irq_signin_config_t stcIrqSignConfig;
    DMA_StructInit(&stcDmaInit);
    stcDmaInit.u32IntEn = DMA_INT_ENABLE;
    stcDmaInit.u32BlockSize = 1UL;
    stcDmaInit.u32TransCnt = 512;
    stcDmaInit.u32DataWidth = DMA_DATAWIDTH_8BIT;
    stcDmaInit.u32DestAddr = (uint32_t)(&USART_DMA.USARTx->DR);
    stcDmaInit.u32SrcAddr = 0;
    stcDmaInit.u32SrcInc = DMA_SRC_ADDR_INC; /*地址递增*/
    stcDmaInit.u32DestInc = DMA_DEST_ADDR_FIX;/*地址固定*/
    enRet = DMA_Init(USART_DMA.DMAx, USART_DMA.DMA_CH, &stcDmaInit);
    if (Ok == enRet)
    {
        DMA_ClearTransIntStatus(USART_DMA.DMAx, USART_DMA.u32transInt);

        stcIrqSignConfig.enIntSrc = USART_DMA.enintsrc; /*发送完成中断*/
        stcIrqSignConfig.enIRQn  =  USART_DMA.enirq;
        stcIrqSignConfig.pfnCallback= USART_DMA.pfncallback;

        INTC_IrqSignIn(&stcIrqSignConfig);
        NVIC_ClearPendingIRQ(stcIrqSignConfig.enIRQn);
        NVIC_SetPriority(stcIrqSignConfig.enIRQn,USART_DMA.priority);
        NVIC_EnableIRQ(stcIrqSignConfig.enIRQn);

        DMA_Cmd(USART_DMA.DMAx, Enable);
        DMA_TransIntCmd(USART_DMA.DMAx, USART_DMA.u32transInt, Enable);
        DMA_ChannelCmd(USART_DMA.DMAx, USART_DMA.DMA_CH, Enable);
        DMA_SetTriggerSrc(USART_DMA.DMAx, USART_DMA.DMA_CH, USART_DMA.event_src);
    }
    
    GPIO_SetFunc(USART_DMA.u8port, USART_DMA.u8pin, USART_DMA.u8func, PIN_SUBFUNC_DISABLE);
    DMA_ChannelCmd(USART_DMA.DMAx,  USART_DMA.DMA_CH,  Disable);
    USART_FuncCmd(USART_DMA.USARTx, USART_TX, Disable);
    USART_FuncCmd(USART_DMA.USARTx, USART_INT_TXE, Disable);
    
    DMA_ClearTransIntStatus(USART_DMA.DMAx,  USART_DMA.u32transInt);
	DMA_SetSrcAddr(USART_DMA.DMAx, USART_DMA.DMA_CH, (uint32_t)"pbuf");
    DMA_ChannelCmd(USART_DMA.DMAx,  USART_DMA.DMA_CH,  Enable);
	DMA_SetTransCnt(USART_DMA.DMAx,  USART_DMA.DMA_CH, 0);
}
//Usart init
void hal_rs485_usart_init(uint32_t baudval,uint8_t* u8port, uint16_t* u8pin, uint8_t* u8func,uint32_t u32fcg,M4_USART_TypeDef *USARTx)
{
	const stc_usart_uart_init_t stcUartInit2 = {
		.u32Baudrate = baudval,
		.u32BitDirection = USART_LSB,
		.u32StopBit = USART_STOPBIT_1BIT,
		.u32Parity = USART_PARITY_NONE,
		.u32DataWidth = USART_DATA_LENGTH_8BIT,
		.u32ClkMode = USART_INTERNCLK_OUTPUT,
		.u32PclkDiv = USART_PCLK_DIV4,
		.u32OversamplingBits = USART_OVERSAMPLING_8BIT,
		.u32NoiseFilterState = USART_NOISE_FILTER_ENABLE,
		.u32SbDetectPolarity = USART_SB_DETECT_FALLING,
	};

	/* Configure USART RX/TX pin. */
	GPIO_SetFunc(u8port[0], u8pin[0], u8func[0], PIN_SUBFUNC_DISABLE);  
	GPIO_SetFunc(u8port[1], u8pin[1], u8func[1], PIN_SUBFUNC_DISABLE);

	/* Enable peripheral clock */
	PWC_Fcg3PeriphClockCmd(u32fcg, Enable);
	PWC_FCG0_Unlock();
	PWC_Fcg0PeriphClockCmd((PWC_FCG0_DMA1 | PWC_FCG0_AOS), Enable);
	PWC_FCG0_Lock();

	USART_UartInit(USARTx, &stcUartInit2);
}

uint8_t rs_data[1024];
uint32_t rs_length = 0;
static void rs485_rx_timeout_int_cb(hal_usart_rx_pack_t* pack)
{
	if(	pack->length != 0 )
	{
		memcpy(rs_data,pack->data,pack->length);
		rs_length = pack->length;
	}
}

void Rs485_2_Init(void)
{
	uint8_t u8port[2] = {RS485_2_USART_TX_PORT,RS485_2_USART_RX_PORT};
	uint16_t u8pin[2] = {RS485_2_USART_TX_PIN,RS485_2_USART_RX_PIN};
	uint8_t u8func[2] = {GPIO_FUNC_34_USART2_TX,GPIO_FUNC_35_USART2_RX};
	hal_rs485_usart_init(250000,u8port,u8pin,u8func,RS485_2_USART_CLK,RS485_2_USART_UNIT);
	hal_rs485_usart_transmit_dma_start(rs485_2_tx);
	hal_rs485_usart_receive_dma_start(rs485_2_rx,TMR0_1_B,NULL);

	
	uint8_t u8port1[2] = {RS485_3_USART_TX_PORT,RS485_3_USART_RX_PORT};
	uint16_t u8pin1[2] = {RS485_3_USART_TX_PIN,RS485_3_USART_RX_PIN};
	uint8_t u8func1[2] = {RS485_3_USART_TX_GPIO_FUNC,RS485_3_USART_RX_GPIO_FUNC};
	hal_rs485_usart_init(250000,u8port1,u8pin1,u8func1,RS485_3_USART_CLK,RS485_3_USART_UNIT);
	hal_rs485_usart_transmit_dma_start(rs485_3_tx);
	hal_rs485_usart_receive_dma_start(rs485_3_rx,TMR0_2_A,rs485_rx_timeout_int_cb);
	
	Error_Init();
}

#endif


void hal_rs485_2_write_ctrl_pin(uint8_t state)
{
    if(state)
    {
        GPIO_SetPins(RS485_2_CTRL_GPIO, RS485_2_CTRL_Pin);
    }
    else
    {
        GPIO_ResetPins(RS485_2_CTRL_GPIO, RS485_2_CTRL_Pin);    
    }
}

void hal_rs485_4_write_ctrl_pin(uint8_t state)
{
    if(state)
    {
        GPIO_SetPins(RS485_3_CTRL_GPIO, RS485_3_CTRL_Pin);
    }
    else
    {
        GPIO_ResetPins(RS485_3_CTRL_GPIO, RS485_3_CTRL_Pin);    
    }
}

void rs485_ctrl_pin_init(void)
{
		stc_gpio_init_t stcGpioInit = {0};

		(void)GPIO_StructInit(&stcGpioInit);
		stcGpioInit.u16PinDir = PIN_DIR_OUT;
		(void)GPIO_Init(RS485_2_CTRL_GPIO, RS485_2_CTRL_Pin, &stcGpioInit);
		(void)GPIO_Init(RS485_3_CTRL_GPIO, RS485_3_CTRL_Pin, &stcGpioInit);
		
		hal_rs485_2_write_ctrl_pin(0);
		hal_rs485_4_write_ctrl_pin(0);
		Relay_Usart_Select_Init();
//		if( !Relay4_5 )
//		{
//			 GPIO_SetPins(RELAY45_SELECT_PORT, RELAY45_SELECT_PIN);
//			GPIO_SetPins(RELAY23_SELECT_PORT, RELAY23_SELECT_PIN);
//		}
//		else
//		{
//			 GPIO_ResetPins(RELAY23_SELECT_PORT, RELAY23_SELECT_PIN);
//			GPIO_ResetPins(RELAY45_SELECT_PORT, RELAY45_SELECT_PIN);
//		}
		Rs485_2_Init();
		
}

void Relay_Usart_Select_Init(void)
{
	stc_gpio_init_t stcGpioInit;
    (void)GPIO_StructInit(&stcGpioInit);
    stcGpioInit.u16PinDir = PIN_DIR_OUT;
    stcGpioInit.u16PinDrv = PIN_DRV_HIGH;
	stcGpioInit.u16PullUp = PIN_PU_ON;
	
	//stcGpioInit.u16PinState = !USART2_USART3_RELAY;
    (void)GPIO_Init(RELAY23_SELECT_PORT, RELAY23_SELECT_PIN, &stcGpioInit);
	
	//stcGpioInit.u16PinState = !USART4_USART5_RELAY;
	(void)GPIO_Init(RELAY45_SELECT_PORT, RELAY45_SELECT_PIN, &stcGpioInit);	
	
	if( !Relay4_5 )
	{
		 GPIO_SetPins(RELAY45_SELECT_PORT, RELAY45_SELECT_PIN);
		GPIO_SetPins(RELAY23_SELECT_PORT, RELAY23_SELECT_PIN);
	}
	else
	{
		 GPIO_ResetPins(RELAY23_SELECT_PORT, RELAY23_SELECT_PIN);
		GPIO_ResetPins(RELAY45_SELECT_PORT, RELAY45_SELECT_PIN);
	}	
	
}
