#include "hal_uart.h"
#include "hc32_ddl.h"
#include "perf_counter.h"

#define CB_USART(cb, ...)  do{ if(NULL != cb) cb(__VA_ARGS__); }while(0)

/*  串口和定时器超时对应的关系
    USART_1：Timer0 Unit1 A 通道
    USART_2：Timer0 Unit1 B 通道
    USART_6：Timer0 Unit2 A 通道
    USART_7：Timer0 Unit2 B 通道
*/
static void (*dmx_rx_err_int_cb[HAL_UART_NUM])(void);
static void (*dmx_rx_timeout_int_cb[HAL_UART_NUM])(void);
static void (*dmx_tx_complete_irq_cb[HAL_UART_NUM])(void);

static void Irq_Priority_Handler(const stc_irq_signin_config_t *pstcConfig, uint32_t Priority);
static void DMX_IRQHandler(void);
static void DMX_DMA_TC_IRQHandler(void);
static void DMX_RxTimeout_IRQHandler(void);
static void CRMX_IRQHandler(void);
static void CRMX_DMA_TC_IRQHandler(void);
static void CRMX_RxTimeout_IRQHandler(void);
static void BLE_IRQHandler(void);
static void BLE_DMA_TC_IRQHandler(void);
static void BLE_RxTimeout_IRQHandler(void);
static void CTl_485_IRQHandler(void);
static void CTl_485_DMA_TC_IRQHandler(void);
static void CTl_485_RxTimeout_IRQHandler(void);
static void ADAPTER_IRQHandler(void);
static void ADAPTER_DMA_TC_IRQHandler(void);
static void DEBUG_IRQHandler(void);
static void DEBUG_DMA_TC_IRQHandler(void);
static void CTl_adapter_RxTimeout_IRQHandler(void);
static void CTl_debug_RxTimeout_IRQHandler(void);


struct hc4a0_uart_cfg
{
    M4_USART_TypeDef *usartx;
    uint32_t fcg3_periph;
    uint32_t u32StopBit;
    uint32_t u32PclkDiv;
    
    struct hc4a0_gpio_cgf
    {
        uint8_t  port;
        uint16_t pin;
        uint16_t func;
    } pin_ctrl, pin_tx, pin_rx;

    struct hc4a0_dma_cgf
    {
        M4_DMA_TypeDef *dmax;
        uint8_t  channel;
        uint32_t trans_int;
    } dma_tx, dma_rx;

    struct hc4a0_dma_irq
    {
        stc_irq_signin_config_t stc_irq_signin_cfg;
        uint32_t Priority;
        en_event_src_t src_en;
    } irq_ei, irq_ti, irq_ri;

    struct hc4a0_tmr_cgf
    {
        union{
            M4_TMR0_TypeDef* tmr0x;
            M4_TMR2_TypeDef* tmr2x;
        };
        uint8_t channel;
        uint32_t fcg2_periph;
        uint16_t rcv_timeout_bits;
    } tmr_cfg;
    
    
};

static struct hc4a0_uart_cfg uart_cfg[HAL_UART_NUM] =
{
    {   // DMX - uart7 (308)
        .usartx = M4_USART7,
        .u32StopBit = USART_STOPBIT_2BIT,
        .fcg3_periph = PWC_FCG3_USART7,
        .u32PclkDiv = USART_PCLK_DIV1,
        .pin_ctrl = {  GPIO_PORT_B, GPIO_PIN_09 },
        .pin_tx = { GPIO_PORT_E, GPIO_PIN_02, GPIO_FUNC_38_USART7_TX },
        .pin_rx = { GPIO_PORT_E, GPIO_PIN_03, GPIO_FUNC_39_USART7_RX },
        .dma_rx = { M4_DMA1, DMA_CH0 },
        .dma_tx = { M4_DMA1, DMA_CH1, DMA_TC_INT_CH1 },
        .tmr_cfg =   { {.tmr0x = M4_TMR0_2}, TMR0_CH_B, PWC_FCG2_TMR0_2 ,140},
        .irq_ei = {
            .stc_irq_signin_cfg = { INT_USART7_EI, Int006_IRQn, DMX_IRQHandler},
            DDL_IRQ_PRIORITY_15
        },

        .irq_ti = {
            .stc_irq_signin_cfg = { INT_DMA1_TC1, Int007_IRQn, DMX_DMA_TC_IRQHandler },
            DDL_IRQ_PRIORITY_15,
            EVT_USART7_TI
        },
        .irq_ri = {
            .stc_irq_signin_cfg = { INT_USART7_RTO, Int008_IRQn, DMX_RxTimeout_IRQHandler },
            DDL_IRQ_PRIORITY_15,
            EVT_USART7_RI
        },

    },

    {   // CRMX - uart3(307)
        .usartx = M4_USART3,
        .u32StopBit = USART_STOPBIT_2BIT,
        .fcg3_periph = PWC_FCG3_USART3,
        .u32PclkDiv = USART_PCLK_DIV1,
        .pin_ctrl = {  0, 0 },
        .pin_tx = { GPIO_PORT_B, GPIO_PIN_03, GPIO_FUNC_36_USART3_TX },
        .pin_rx = { GPIO_PORT_G, GPIO_PIN_15, GPIO_FUNC_37_USART3_RX },
        .dma_rx = { M4_DMA1, DMA_CH2 },
        .dma_tx = { M4_DMA1, DMA_CH3, DMA_TC_INT_CH3 },
        .tmr_cfg =   { {.tmr2x = M4_TMR2_1}, TMR2_CH_A, PWC_FCG2_TMR2_1 ,140},
        .irq_ei = {
            .stc_irq_signin_cfg = { INT_USART3_EI, Int009_IRQn, CRMX_IRQHandler },
            DDL_IRQ_PRIORITY_15
        },
        .irq_ti = {
            .stc_irq_signin_cfg = { INT_DMA1_TC3, Int010_IRQn, CRMX_DMA_TC_IRQHandler },
            DDL_IRQ_PRIORITY_15,
            EVT_USART3_TI
        },
        .irq_ri = {
            .stc_irq_signin_cfg = { INT_TMR2_1_CMPA, Int011_IRQn, CRMX_RxTimeout_IRQHandler },
            DDL_IRQ_PRIORITY_15,
            EVT_USART3_RI
        },
    },
    
    {   // Ble - uart6(307)
        .usartx = M4_USART6,
        .u32StopBit = USART_STOPBIT_1BIT,
        .fcg3_periph = PWC_FCG3_USART6,
        .u32PclkDiv = USART_PCLK_DIV1,
        .pin_ctrl = {  0, 0 },
        .pin_tx = { GPIO_PORT_E, GPIO_PIN_04, GPIO_FUNC_36_USART6_TX   },
        .pin_rx = { GPIO_PORT_E, GPIO_PIN_05, GPIO_FUNC_37_USART6_RX },
        .dma_rx = { M4_DMA1, DMA_CH4 },
        .dma_tx = { M4_DMA1, DMA_CH5, DMA_TC_INT_CH5 },
        .tmr_cfg =   { {.tmr0x = M4_TMR0_2}, TMR0_CH_A, PWC_FCG2_TMR0_2 ,60},
        .irq_ei = {
            .stc_irq_signin_cfg = { INT_USART6_EI, Int012_IRQn, BLE_IRQHandler },
            DDL_IRQ_PRIORITY_15
        },

        .irq_ti = {
            .stc_irq_signin_cfg = { INT_DMA1_TC5, Int013_IRQn, BLE_DMA_TC_IRQHandler },
            DDL_IRQ_PRIORITY_15,
            EVT_USART6_TI
        },
        .irq_ri = {
            .stc_irq_signin_cfg = { INT_USART6_RTO, Int014_IRQn, BLE_RxTimeout_IRQHandler },
            DDL_IRQ_PRIORITY_15,
            EVT_USART6_RI
        },

    },
    
    {   // 485 - uart2(307)
        .usartx = M4_USART2,
        .u32StopBit = USART_STOPBIT_1BIT,
        .fcg3_periph = PWC_FCG3_USART2,
        .u32PclkDiv = USART_PCLK_DIV1,
        .pin_ctrl = {  GPIO_PORT_G, GPIO_PIN_07 },
        .pin_tx = { GPIO_PORT_F, GPIO_PIN_11, GPIO_FUNC_34_USART2_TX },
        .pin_rx = { GPIO_PORT_F, GPIO_PIN_12, GPIO_FUNC_35_USART2_RX },
        .dma_rx = { M4_DMA1, DMA_CH6 },
        .dma_tx = { M4_DMA1, DMA_CH7, DMA_TC_INT_CH7 },
        .tmr_cfg =   { {.tmr0x = M4_TMR0_1}, TMR0_CH_B, PWC_FCG2_TMR0_1 ,140},
        .irq_ei = {
            .stc_irq_signin_cfg = { INT_USART2_EI, Int015_IRQn, CTl_485_IRQHandler },
            DDL_IRQ_PRIORITY_15
        },

        .irq_ti = {
            .stc_irq_signin_cfg = { INT_DMA1_TC7, Int016_IRQn, CTl_485_DMA_TC_IRQHandler },
            DDL_IRQ_PRIORITY_15,
            EVT_USART2_TI
        },
        .irq_ri = {
            .stc_irq_signin_cfg = { INT_USART2_RTO, Int017_IRQn, CTl_485_RxTimeout_IRQHandler },
            DDL_IRQ_PRIORITY_15,
            EVT_USART2_RI
        },

    },
    
    {   // adapter - uart3 (307) 与适配器通信
        .usartx = M4_USART1,
        .u32StopBit = USART_STOPBIT_1BIT,
        .fcg3_periph = PWC_FCG3_USART1,
        .u32PclkDiv = USART_PCLK_DIV16,
        .pin_ctrl = {  GPIO_PORT_G, GPIO_PIN_08},
        .pin_tx = { GPIO_PORT_I, GPIO_PIN_02, GPIO_FUNC_32_USART1_TX },
        .pin_rx = { GPIO_PORT_I, GPIO_PIN_03, GPIO_FUNC_33_USART1_RX },
        .dma_rx = { M4_DMA2, DMA_CH0 },
        .dma_tx = { M4_DMA2, DMA_CH1, DMA_TC_INT_CH1 },
        .tmr_cfg =   {{.tmr0x = M4_TMR0_1}, TMR0_CH_A, PWC_FCG2_TMR0_1, 30},
        .irq_ei = {
            .stc_irq_signin_cfg = {  INT_USART1_EI, Int018_IRQn, ADAPTER_IRQHandler },
            DDL_IRQ_PRIORITY_15
        },

        .irq_ti = {
            .stc_irq_signin_cfg = { INT_DMA2_TC1, Int019_IRQn, ADAPTER_DMA_TC_IRQHandler },
            DDL_IRQ_PRIORITY_15,
            EVT_USART1_TI
        },
        .irq_ri = {
            .stc_irq_signin_cfg = { INT_USART1_RTO, Int020_IRQn, CTl_adapter_RxTimeout_IRQHandler },
            DDL_IRQ_PRIORITY_15,
            EVT_USART1_RI
        },

    },
    
    {   // debug - uart4
        .usartx = M4_USART4,
        .u32StopBit = USART_STOPBIT_1BIT,
        .fcg3_periph = PWC_FCG3_USART4,
        .u32PclkDiv = USART_PCLK_DIV1,
        .pin_ctrl = {  0, 0 },
        .pin_tx = { GPIO_PORT_D, GPIO_PIN_05, GPIO_FUNC_32_USART4_TX },
        .pin_rx = { GPIO_PORT_D, GPIO_PIN_06, GPIO_FUNC_33_USART4_RX },
        .dma_rx = { M4_DMA2, DMA_CH2 },
        .dma_tx = { M4_DMA2, DMA_CH3, DMA_TC_INT_CH3 },
        .tmr_cfg =   { {.tmr2x = M4_TMR2_1}, TMR2_CH_B, PWC_FCG2_TMR2_1 ,60 },
        .irq_ei = {
            .stc_irq_signin_cfg = {  INT_USART4_EI, Int021_IRQn, DEBUG_IRQHandler },
            DDL_IRQ_PRIORITY_15
        },

        .irq_ti = {
            .stc_irq_signin_cfg = { INT_DMA2_TC3, Int022_IRQn, DEBUG_DMA_TC_IRQHandler },
            DDL_IRQ_PRIORITY_15,
            EVT_USART4_TI
        },
        .irq_ri = {
            .stc_irq_signin_cfg = { INT_TMR2_1_CMPB, Int023_IRQn, CTl_debug_RxTimeout_IRQHandler},
            DDL_IRQ_PRIORITY_15,
            0
        },
    },

};

void hal_uart_set_stopbit(enum hal_uart unit, uint32_t stopbit)
{
	uart_cfg[unit].u32StopBit = stopbit;
}

void hal_uart_init(enum hal_uart unit ,uint32_t u32Baudrate)
{
    stc_usart_uart_init_t stc_uart_init =
    {
        .u32Baudrate = u32Baudrate,
        .u32BitDirection = USART_LSB,
        .u32StopBit = uart_cfg[unit].u32StopBit,
        .u32Parity = USART_PARITY_NONE,
        .u32DataWidth = USART_DATA_LENGTH_8BIT,
        .u32ClkMode = USART_INTERNCLK_OUTPUT,
        .u32PclkDiv = uart_cfg[unit].u32PclkDiv ,
        .u32OversamplingBits = USART_OVERSAMPLING_8BIT,
        .u32NoiseFilterState = USART_NOISE_FILTER_ENABLE,
        .u32SbDetectPolarity = USART_SB_DETECT_FALLING,
    };
	
	if(unit == HAL_UART_DMX || unit == HAL_UART_RS485 || unit == HAL_UART_ADAPTER)
	{
		stc_gpio_init_t stc_gpio_init = {0};

		(void)GPIO_StructInit(&stc_gpio_init);
		stc_gpio_init.u16PinDir = PIN_DIR_OUT;
		stc_gpio_init.u16PinState = PIN_STATE_RESET;
		(void)GPIO_Init(uart_cfg[unit].pin_ctrl.port, uart_cfg[unit].pin_ctrl.pin, &stc_gpio_init);
	}
    /* Configure USART RX/TX pin. */
    GPIO_SetFunc(uart_cfg[unit].pin_rx.port, uart_cfg[unit].pin_rx.pin, uart_cfg[unit].pin_rx.func, PIN_SUBFUNC_DISABLE);
    GPIO_SetFunc(uart_cfg[unit].pin_tx.port, uart_cfg[unit].pin_tx.pin, uart_cfg[unit].pin_tx.func, PIN_SUBFUNC_DISABLE);

    /* Enable peripheral clock */
    PWC_Fcg3PeriphClockCmd(uart_cfg[unit].fcg3_periph, Enable);
    // PWC_FCG0_Unlock();
    PWC_Fcg0PeriphClockCmd((PWC_FCG0_DMA1 | PWC_FCG0_DMA2 | PWC_FCG0_AOS), Enable);
    //PWC_FCG0_Lock();

    USART_UartInit(uart_cfg[unit].usartx, &stc_uart_init);
}


void hal_uart_transmit_dma_start(enum hal_uart unit, const uint8_t *p_data, uint32_t size)
{
    en_result_t ret_en;
    stc_dma_init_t stc_dma_init;
    stc_irq_signin_config_t stcIrqSignConfig;
    DMA_StructInit(&stc_dma_init);
    stc_dma_init.u32IntEn = DMA_INT_ENABLE;
    stc_dma_init.u32BlockSize = 1UL;
    stc_dma_init.u32TransCnt = size;
    stc_dma_init.u32DataWidth = DMA_DATAWIDTH_8BIT;
    stc_dma_init.u32DestAddr = (uint32_t)(&uart_cfg[unit].usartx->DR);
    stc_dma_init.u32SrcAddr = ((uint32_t)p_data);
    stc_dma_init.u32SrcInc = DMA_SRC_ADDR_INC; 
    stc_dma_init.u32DestInc = DMA_DEST_ADDR_FIX;
    ret_en = DMA_Init(uart_cfg[unit].dma_tx.dmax, uart_cfg[unit].dma_tx.channel, &stc_dma_init);

    if (Ok == ret_en)
    {
        DMA_Cmd(uart_cfg[unit].dma_tx.dmax, Enable);
        DMA_TransIntCmd(uart_cfg[unit].dma_tx.dmax, uart_cfg[unit].dma_tx.trans_int, Enable);
        DMA_SetTriggerSrc(uart_cfg[unit].dma_tx.dmax, uart_cfg[unit].dma_tx.channel, uart_cfg[unit].irq_ti.src_en);
    }

    GPIO_SetFunc(uart_cfg[unit].pin_tx.port, uart_cfg[unit].pin_tx.pin, uart_cfg[unit].pin_tx.func, PIN_SUBFUNC_DISABLE);
    DMA_ChannelCmd(uart_cfg[unit].dma_tx.dmax,  uart_cfg[unit].dma_tx.channel,  Disable);
    USART_FuncCmd(uart_cfg[unit].usartx, USART_TX, Disable);
    USART_FuncCmd(uart_cfg[unit].usartx, USART_INT_TXE, Disable);

    DMA_ClearTransIntStatus(uart_cfg[unit].dma_tx.dmax,  uart_cfg[unit].dma_tx.trans_int);
    DMA_SetTransCnt(uart_cfg[unit].dma_tx.dmax, uart_cfg[unit].dma_tx.channel, size);
    DMA_SetSrcAddr(uart_cfg[unit].dma_tx.dmax, uart_cfg[unit].dma_tx.channel, (uint32_t)p_data);
    DMA_ChannelCmd(uart_cfg[unit].dma_tx.dmax,  uart_cfg[unit].dma_tx.channel,  Enable);
    USART_FuncCmd(uart_cfg[unit].usartx, USART_TX, Enable);
    USART_FuncCmd(uart_cfg[unit].usartx, USART_INT_TXE, Enable);
}


void hal_uart_receive_dma_start(enum hal_uart unit ,uint8_t *data, uint32_t size)
{
    en_result_t ret_en;
    stc_dma_init_t stc_dma_init;
    DMA_StructInit(&stc_dma_init);

    stc_dma_init.u32IntEn = DMA_INT_ENABLE;
    stc_dma_init.u32BlockSize = 1UL;
    stc_dma_init.u32TransCnt = size;
    stc_dma_init.u32DataWidth = DMA_DATAWIDTH_8BIT;
    stc_dma_init.u32DestAddr = (uint32_t)data;
    stc_dma_init.u32SrcAddr = ((uint32_t)(&uart_cfg[unit].usartx->DR) + 2UL);
    stc_dma_init.u32SrcInc = DMA_SRC_ADDR_FIX;
    stc_dma_init.u32DestInc = DMA_DEST_ADDR_INC;
    ret_en = DMA_Init(uart_cfg[unit].dma_rx.dmax, uart_cfg[unit].dma_rx.channel, &stc_dma_init);

    if (Ok == ret_en)
    {
        DMA_Cmd(uart_cfg[unit].dma_rx.dmax, Enable);
        DMA_ChannelCmd(uart_cfg[unit].dma_rx.dmax, uart_cfg[unit].dma_rx.channel, Enable);
        DMA_SetTriggerSrc(uart_cfg[unit].dma_rx.dmax, uart_cfg[unit].dma_rx.channel, uart_cfg[unit].irq_ri.src_en);
    }
    
    USART_FuncCmd(uart_cfg[unit].usartx, (USART_RX | USART_TX ), Enable);
    USART_ClearStatus(uart_cfg[unit].usartx, (USART_CR1_CFE | USART_CLEAR_FLAG_ORE));
}

static void Irq_Priority_Handler(const stc_irq_signin_config_t *pstcConfig, uint32_t Priority)
{
    if (NULL != pstcConfig)
    {
        (void)INTC_IrqSignIn(pstcConfig);
        NVIC_ClearPendingIRQ(pstcConfig->enIRQn);
        NVIC_SetPriority(pstcConfig->enIRQn, Priority);
        NVIC_EnableIRQ(pstcConfig->enIRQn);
    }
}

void hal_uart_irq_register(enum hal_uart unit,enum hal_uart_irq irq_type, void (*irq_cb)(void))
{
   

    stc_irq_signin_config_t stc_irq_signin_cfg = {0};
    stc_tmr0_init_t stc_tmr0_init = {0};
    stc_tmr2_init_t stcInit;
    uint32_t u32CmpVal;
    
    switch(irq_type)
    {
        case HAL_UART_IRQ_ERR:
            dmx_rx_err_int_cb[unit] = irq_cb;
            
            stc_irq_signin_cfg.enIRQn = uart_cfg[unit].irq_ei.stc_irq_signin_cfg.enIRQn;
            stc_irq_signin_cfg.enIntSrc = uart_cfg[unit].irq_ei.stc_irq_signin_cfg.enIntSrc; /*错误中断*/
            stc_irq_signin_cfg.pfnCallback = uart_cfg[unit].irq_ei.stc_irq_signin_cfg.pfnCallback;
            Irq_Priority_Handler(&stc_irq_signin_cfg, uart_cfg[unit].irq_ei.Priority);
            USART_FuncCmd(uart_cfg[unit].usartx, USART_INT_RX , Enable);
            break;
        case HAL_UART_IRQ_TX_DMA_FT:
            dmx_tx_complete_irq_cb[unit] = irq_cb;
        
            DMA_ClearTransIntStatus(uart_cfg[unit].dma_tx.dmax, uart_cfg[unit].dma_tx.trans_int);

            stc_irq_signin_cfg.enIntSrc = uart_cfg[unit].irq_ti.stc_irq_signin_cfg.enIntSrc; /*发送完成中断*/
            stc_irq_signin_cfg.enIRQn  =  uart_cfg[unit].irq_ti.stc_irq_signin_cfg.enIRQn;
            stc_irq_signin_cfg.pfnCallback = uart_cfg[unit].irq_ti.stc_irq_signin_cfg.pfnCallback;
            Irq_Priority_Handler(&stc_irq_signin_cfg, uart_cfg[unit].irq_ti.Priority);
            USART_FuncCmd(uart_cfg[unit].usartx, USART_INT_TC , Enable);
            break;
        case HAL_UART_IRQ_RX_TIMEOUT:
            dmx_rx_timeout_int_cb[unit] = irq_cb;
        
            /* Register RX timeout IRQ handler && configure NVIC. */
            stc_irq_signin_cfg.enIRQn = uart_cfg[unit].irq_ri.stc_irq_signin_cfg.enIRQn;
            stc_irq_signin_cfg.enIntSrc = uart_cfg[unit].irq_ri.stc_irq_signin_cfg.enIntSrc; /*超时中断类型*/
            stc_irq_signin_cfg.pfnCallback = uart_cfg[unit].irq_ri.stc_irq_signin_cfg.pfnCallback;
            Irq_Priority_Handler(&stc_irq_signin_cfg, uart_cfg[unit].irq_ri.Priority);
        
            PWC_Fcg2PeriphClockCmd(uart_cfg[unit].tmr_cfg.fcg2_periph, Enable);
            TMR0_SetCntVal(uart_cfg[unit].tmr_cfg.tmr0x, uart_cfg[unit].tmr_cfg.channel, 0U);

            TMR0_StructInit(&stc_tmr0_init);
            stc_tmr0_init.u32ClockDivision = TMR0_CLK_DIV1;
            stc_tmr0_init.u32ClockSource = TMR0_CLK_SRC_XTAL32;
            stc_tmr0_init.u32HwTrigFunc = (TMR0_BT_HWTRG_FUNC_START | TMR0_BT_HWTRG_FUNC_CLEAR);
            if (TMR0_CLK_DIV1 == stc_tmr0_init.u32ClockDivision)
            {
                u32CmpVal = (uart_cfg[unit].tmr_cfg.rcv_timeout_bits - 4UL);
            }
            else if (TMR0_CLK_DIV2 == stc_tmr0_init.u32ClockDivision)
            {
                u32CmpVal = (uart_cfg[unit].tmr_cfg.rcv_timeout_bits/2UL - 2UL);
            }
            else
            {
                u32CmpVal = (uart_cfg[unit].tmr_cfg.rcv_timeout_bits / (1UL << (stc_tmr0_init.u32ClockDivision >> TMR0_BCONR_CKDIVA_POS)) - 1UL);
            }
            DDL_ASSERT(u32CmpVal <= 0xFFFFUL);
            stc_tmr0_init.u16CmpValue =  (uint16_t)(u32CmpVal);
           // stc_tmr0_init.u16CmpValue =  (uint16_t)(ffafdfdlf);
            TMR0_Init(uart_cfg[unit].tmr_cfg.tmr0x, uart_cfg[unit].tmr_cfg.channel, &stc_tmr0_init);
            TMR0_ClearStatus(uart_cfg[unit].tmr_cfg.tmr0x, uart_cfg[unit].tmr_cfg.channel);
            USART_ClearStatus(uart_cfg[unit].usartx, USART_CLEAR_FLAG_RTOF);
            USART_ClearStatus(uart_cfg[unit].usartx, USART_CLEAR_FLAG_ORE);
            USART_FuncCmd(uart_cfg[unit].usartx, (USART_RTO | USART_INT_RTO), Enable);
          
            
            break;
        case HAL_UART_IRQ_RX_TIMER:
            
            dmx_rx_timeout_int_cb[unit] = irq_cb;
            stc_irq_signin_cfg.enIRQn = uart_cfg[unit].irq_ri.stc_irq_signin_cfg.enIRQn;
            stc_irq_signin_cfg.enIntSrc = uart_cfg[unit].irq_ri.stc_irq_signin_cfg.enIntSrc; /*超时中断类型*/
            stc_irq_signin_cfg.pfnCallback = uart_cfg[unit].irq_ri.stc_irq_signin_cfg.pfnCallback;
            Irq_Priority_Handler(&stc_irq_signin_cfg, uart_cfg[unit].irq_ri.Priority);
            
            PWC_Fcg2PeriphClockCmd(uart_cfg[unit].tmr_cfg.fcg2_periph, Enable);
        
            (void)TMR2_StructInit(&stcInit);
            stcInit.u32ClkSrc = TMR2_CLK_SYNC_PCLK1;
            stcInit.u32ClkDiv = TMR2_CLK_DIV64;
            stcInit.u32CmpVal = 120000/8;
            (void)TMR2_Init(uart_cfg[unit].tmr_cfg.tmr2x, uart_cfg[unit].tmr_cfg.channel, &stcInit);
        
            TMR2_IntCmd(uart_cfg[unit].tmr_cfg.tmr2x, uart_cfg[unit].tmr_cfg.channel, TMR2_INT_CMP, Enable);
            TMR2_SetCntVal(uart_cfg[unit].tmr_cfg.tmr2x, uart_cfg[unit].tmr_cfg.channel,0);
            TMR2_Start(uart_cfg[unit].tmr_cfg.tmr2x, uart_cfg[unit].tmr_cfg.channel);
          
            break;
        default: 
            break;
    } 
}

uint32_t hal_uart_get_transmit_dma_cnt(enum hal_uart unit)
{
    return DMA_GetTransCnt(uart_cfg[unit].dma_tx.dmax, uart_cfg[unit].dma_tx.channel);
}

uint32_t hal_uart_get_receive_dma_cnt(enum hal_uart unit)
{
    /* DMX 传输剩余数 */
    return DMA_GetTransCnt(uart_cfg[unit].dma_rx.dmax, uart_cfg[unit].dma_rx.channel);
}

void hal_uart_transmit_dma_stop(enum hal_uart unit)
{
    DMA_ChannelCmd(uart_cfg[unit].dma_tx.dmax, uart_cfg[unit].dma_tx.channel, Disable);
}

void hal_uart_receive_dma_channel_stop(enum hal_uart unit)
{
    DMA_ChannelCmd(uart_cfg[unit].dma_rx.dmax, uart_cfg[unit].dma_rx.channel, Disable);
}

void hal_uart_receive_dma_channel_start(enum hal_uart unit)
{
    DMA_ChannelCmd(uart_cfg[unit].dma_rx.dmax, uart_cfg[unit].dma_rx.channel, Enable);
}
/* ----------------------------------------IRQ package----------------------------------------------- */
static void uart_error_irq(enum hal_uart unit)
{
    uint8_t DR;
    
    CB_USART(dmx_rx_err_int_cb[unit]);
    if(Set == USART_GetStatus(uart_cfg[unit].usartx, (USART_FLAG_FE)))
    {
//        DMA_ChannelCmd(uart_cfg[unit].dma_rx.dmax, uart_cfg[unit].dma_rx.channel,  Disable);
    }
    
    USART_ClearStatus(uart_cfg[unit].usartx, (USART_CLEAR_FLAG_FE | USART_CLEAR_FLAG_PE | USART_CLEAR_FLAG_ORE));
    DR = USART_RecData(uart_cfg[unit].usartx);
    (void)DR;
}

static void dma_tx_complete_irq(enum hal_uart unit)
{
    DMA_ClearTransIntStatus(uart_cfg[unit].dma_rx.dmax, uart_cfg[unit].dma_tx.trans_int);
    CB_USART(dmx_tx_complete_irq_cb[unit]);
}

static void uart_rx_timeout_irq(enum hal_uart unit)
{
    uint8_t DR;
    
    CB_USART(dmx_rx_timeout_int_cb[unit]);
    DR = USART_RecData(uart_cfg[unit].usartx);
    (void)DR;
}

/* ----------------------------------------DMX-------------------------------------------------- */
static void DMX_IRQHandler(void)
{
    const enum hal_uart unit = HAL_UART_DMX;
    
    uart_error_irq(unit);
    (void)USART_RecData(uart_cfg[unit].usartx);
    
    USART_ClearStatus(uart_cfg[unit].usartx, (USART_CLEAR_FLAG_PE | \
                                 USART_CLEAR_FLAG_FE | \
                                 USART_CLEAR_FLAG_ORE));
}

static void DMX_DMA_TC_IRQHandler(void)
{
    const enum hal_uart unit = HAL_UART_DMX;
    dma_tx_complete_irq(unit);
}

static void DMX_RxTimeout_IRQHandler(void)
{
    const enum hal_uart unit = HAL_UART_DMX;    
    TMR0_Cmd(uart_cfg[unit].tmr_cfg.tmr0x, uart_cfg[unit].tmr_cfg.channel, Disable);
    USART_ClearStatus(uart_cfg[unit].usartx, USART_CLEAR_FLAG_RTOF);
    uart_rx_timeout_irq(unit);
//    USART_FuncCmd(uart_cfg[unit].usartx, USART_RTO, Disable); 
//    USART_FuncCmd(uart_cfg[unit].usartx, USART_INT_RTO,Disable); 
}

/* ---------------------------------------CRMX--------------------------------------------------- */
static void CRMX_IRQHandler(void)
{ 
  
    const enum hal_uart unit = HAL_UART_CRMX;
    uart_error_irq(unit);
}

static void CRMX_DMA_TC_IRQHandler(void)
{
    const enum hal_uart unit = HAL_UART_CRMX;
    dma_tx_complete_irq(unit);
}

static void CRMX_RxTimeout_IRQHandler(void)
{
    const enum hal_uart unit = HAL_UART_CRMX;
  
    uart_rx_timeout_irq(unit);
}

/* ----------------------------------------BLE-------------------------------------------------- */
static void BLE_IRQHandler(void)
{
    const enum hal_uart unit = HAL_UART_BLE;
    uart_error_irq(unit);
}

static void BLE_DMA_TC_IRQHandler(void)
{
    const enum hal_uart unit = HAL_UART_BLE;
    dma_tx_complete_irq(unit);
}

static void BLE_RxTimeout_IRQHandler(void)
{
    const enum hal_uart unit = HAL_UART_BLE;  
    TMR0_Cmd(uart_cfg[unit].tmr_cfg.tmr0x, uart_cfg[unit].tmr_cfg.channel, Disable);
    USART_ClearStatus(uart_cfg[unit].usartx, USART_CLEAR_FLAG_RTOF);
    USART_ClearStatus(uart_cfg[unit].usartx, USART_CLEAR_FLAG_FE);
    uart_rx_timeout_irq(unit);
}

/* -----------------------------------------485------------------------------------------------- */
static void CTl_485_IRQHandler(void)
{
    const enum hal_uart unit = HAL_UART_RS485;
    uart_error_irq(unit);
}

static void CTl_485_DMA_TC_IRQHandler(void)
{
    const enum hal_uart unit = HAL_UART_RS485;
    dma_tx_complete_irq(unit);
}

static void CTl_485_RxTimeout_IRQHandler(void)
{
   const enum hal_uart unit = HAL_UART_RS485; 
   TMR0_Cmd(uart_cfg[unit].tmr_cfg.tmr0x, uart_cfg[unit].tmr_cfg.channel, Disable);
   USART_ClearStatus(uart_cfg[unit].usartx, USART_CLEAR_FLAG_RTOF);
   USART_ClearStatus(uart_cfg[unit].usartx, USART_CLEAR_FLAG_ORE);
   uart_rx_timeout_irq(unit);
}

/* -----------------------------------------adapter------------------------------------------------- */

static void ADAPTER_IRQHandler(void)
{
    const enum hal_uart unit = HAL_UART_ADAPTER;
    uart_error_irq(unit);
}

static void ADAPTER_DMA_TC_IRQHandler(void)
{
    const enum hal_uart unit = HAL_UART_ADAPTER;
    dma_tx_complete_irq(unit);
}


static void CTl_adapter_RxTimeout_IRQHandler(void)
{
    const enum hal_uart unit = HAL_UART_ADAPTER; 
    TMR0_Cmd(uart_cfg[unit].tmr_cfg.tmr0x, uart_cfg[unit].tmr_cfg.channel, Disable);
    USART_ClearStatus(uart_cfg[unit].usartx, USART_CLEAR_FLAG_RTOF);
    USART_ClearStatus(uart_cfg[unit].usartx, USART_CLEAR_FLAG_ORE);
//  
    uart_rx_timeout_irq(unit);

}

/* -----------------------------------------debug------------------------------------------------- */
static void DEBUG_IRQHandler(void)
{
    const enum hal_uart unit = HAL_UART_DEBUG;
    uart_error_irq(unit);
}

static void DEBUG_DMA_TC_IRQHandler(void)
{
    const enum hal_uart unit = HAL_UART_DEBUG;
    dma_tx_complete_irq(unit);
}

static void CTl_debug_RxTimeout_IRQHandler(void)
{
    const enum hal_uart unit = HAL_UART_DEBUG;   
    TMR2_IntCmd(uart_cfg[unit].tmr_cfg.tmr2x, uart_cfg[unit].tmr_cfg.channel, TMR2_INT_CMP, Disable);
    USART_ClearStatus(uart_cfg[unit].usartx, USART_CLEAR_FLAG_RTOF);
 
    uart_rx_timeout_irq(unit);
}

/* 串口引脚设置为普通IO口 */
static void uart_tx_set_normal_pin(enum hal_uart unit, uint8_t state)
{
    stc_gpio_init_t stc_gpio_init = {0};

    GPIO_StructInit(&stc_gpio_init);
    stc_gpio_init.u16PinDir = PIN_DIR_OUT;
	stc_gpio_init.u16PinState = state;
    GPIO_Init(uart_cfg[unit].pin_tx.port, uart_cfg[unit].pin_tx.pin, &stc_gpio_init);
    GPIO_SetFunc(uart_cfg[unit].pin_tx.port, uart_cfg[unit].pin_tx.pin, GPIO_FUNC_0, PIN_SUBFUNC_DISABLE);
}

/* 串口引脚设置为普通IO口 */
static void uart_rx_set_normal_pin(enum hal_uart unit)
{
    stc_gpio_init_t stc_gpio_init = {0};

    GPIO_StructInit(&stc_gpio_init);
    stc_gpio_init.u16PinDir = PIN_DIR_IN;
    GPIO_Init(uart_cfg[unit].pin_rx.port, uart_cfg[unit].pin_rx.pin, &stc_gpio_init);
    GPIO_SetFunc(uart_cfg[unit].pin_rx.port, uart_cfg[unit].pin_rx.pin, 0, PIN_SUBFUNC_DISABLE);
}

/* 设置为串口 */
static void set_uart_dmx_tx_pin(enum hal_uart unit)
{
    hal_uart_init(unit,250000);
//    GPIO_SetFunc(uart_cfg[unit].pin_tx.port, uart_cfg[unit].pin_tx.pin, uart_cfg[unit].pin_tx.func, PIN_SUBFUNC_ENABLE);
}

static void set_uart_pin(enum hal_uart unit, uint8_t state)
{
    if(state)
    {
        GPIO_SetPins(uart_cfg[unit].pin_tx.port, uart_cfg[unit].pin_tx.pin);
    }
    else
    {
        GPIO_ResetPins(uart_cfg[unit].pin_tx.port, uart_cfg[unit].pin_tx.pin);
    }
}

static void set_rs485_ctr_pin(enum hal_uart unit, uint8_t state)
{
    if(state)
    {
        GPIO_SetPins(uart_cfg[unit].pin_ctrl.port, uart_cfg[unit].pin_ctrl.pin);
    }
    else
    {
        GPIO_ResetPins(uart_cfg[unit].pin_ctrl.port, uart_cfg[unit].pin_ctrl.pin);
    }
}

void hal_dmx_write_ctrl_pin(enum hal_uart unit, uint8_t state)
{
    set_rs485_ctr_pin(unit, state);
}

void hal_dmx_write_usart_tx_pin(enum hal_uart unit, uint8_t state)
{
    uart_tx_set_normal_pin(unit, state);
}

void hal_dmx_usart_enable(enum hal_uart unit)
{
    GPIO_SetFunc(uart_cfg[unit].pin_tx.port, uart_cfg[unit].pin_tx.pin, uart_cfg[unit].pin_tx.func, PIN_SUBFUNC_DISABLE);
}

void hal_ble_uart_set_tx_pin(enum hal_uart unit)
{
    stc_gpio_init_t stc_gpio_init = {0};

    GPIO_StructInit(&stc_gpio_init);
    stc_gpio_init.u16PinDir = PIN_DIR_OUT;
    GPIO_Init(uart_cfg[unit].pin_rx.port, uart_cfg[unit].pin_rx.pin, &stc_gpio_init);
	GPIO_Init(uart_cfg[unit].pin_tx.port, uart_cfg[unit].pin_tx.pin, &stc_gpio_init);
    GPIO_SetFunc(uart_cfg[unit].pin_rx.port, uart_cfg[unit].pin_rx.pin, GPIO_FUNC_0, PIN_SUBFUNC_DISABLE);
	GPIO_SetFunc(uart_cfg[unit].pin_tx.port, uart_cfg[unit].pin_tx.pin, GPIO_FUNC_0, PIN_SUBFUNC_DISABLE);
	 
    GPIO_ResetPins(uart_cfg[unit].pin_rx.port, uart_cfg[unit].pin_rx.pin);
	GPIO_ResetPins(uart_cfg[unit].pin_tx.port, uart_cfg[unit].pin_tx.pin);
}

void hal_ble_uart_set_enale(enum hal_uart unit)
{
    GPIO_SetFunc(uart_cfg[unit].pin_rx.port, uart_cfg[unit].pin_rx.pin, uart_cfg[unit].pin_rx.func, PIN_SUBFUNC_DISABLE);
	GPIO_SetFunc(uart_cfg[unit].pin_tx.port, uart_cfg[unit].pin_tx.pin, uart_cfg[unit].pin_tx.func, PIN_SUBFUNC_DISABLE);
}

/* 生成DMX的break信号 */
void hal_uart_generate_break(enum hal_uart unit, uint32_t us)
{
    uart_tx_set_normal_pin(unit, 0);
    GPIO_SetPins(uart_cfg[unit].pin_ctrl.port, uart_cfg[unit].pin_ctrl.pin);
    
    __disable_irq();
    set_uart_pin(unit, 0);
    delay_us(us);
    set_uart_pin(unit, 1);
    delay_us(20);
    __enable_irq();
    
    set_uart_dmx_tx_pin(unit);
}

uint8_t hal_uart_get_status(enum hal_uart unit, enum hal_uart_flag flag)
{
    uint32_t uart_flag = 0;
    uint8_t res = 0;
    
    switch((uint8_t)flag)
    {
        case HAL_USART_FLAG_PE:   uart_flag = USART_FLAG_PE;    break;
        case HAL_USART_FLAG_FE:   uart_flag = USART_FLAG_FE;    break;
        case HAL_USART_FLAG_ORE:  uart_flag = USART_FLAG_ORE;   break;
        case HAL_USART_FLAG_BE:   uart_flag = USART_FLAG_BE;    break;
        case HAL_USART_FLAG_RXNE: uart_flag = USART_FLAG_RXNE;  break;
        case HAL_USART_FLAG_TC:   uart_flag = USART_FLAG_TC;    break;
        case HAL_USART_FLAG_TXE:  uart_flag = USART_FLAG_TXE;   break;
        case HAL_USART_FLAG_RTOF: uart_flag = USART_FLAG_RTOF;  break;
        case HAL_USART_FLAG_LBD:  uart_flag = USART_FLAG_LBD;   break;
        case HAL_USART_FLAG_WKUP: uart_flag = USART_FLAG_WKUP;  break;
        case HAL_USART_FLAG_MPB:  uart_flag = USART_FLAG_MPB;   break;
        default:break;
    }
    
    res = USART_GetStatus(uart_cfg[unit].usartx, uart_flag);
    
    return res;
}

void hal_uart_funcmd(enum hal_uart unit, enum hal_uart_cmd fun, uint8_t status)
{
    uint32_t uart_fun= 0;
    uint8_t res = 0;
    
    switch((uint8_t)fun)
    {
        case HAL_USART_RX:      uart_fun = USART_RX;       break;
        case HAL_USART_TX:      uart_fun = USART_TX;       break;
        case HAL_USART_RTO:     uart_fun = USART_RTO;      break;
        case HAL_USART_INT_RX:  uart_fun = USART_INT_RX;   break;
        case HAL_USART_INT_TXE: uart_fun = USART_INT_TXE;  break;
        case HAL_USART_INT_TC:  uart_fun = USART_INT_TC;   break;
        case HAL_USART_INT_RTO: uart_fun = USART_INT_RTO;  break;
        default:break;
    }
    
    USART_FuncCmd(uart_cfg[unit].usartx, fun, status);
}

void hal_uart_send_data(enum hal_uart unit ,uint8_t *data, uint32_t size)
{
        __disable_irq();
    for(uint32_t i = 0; i < size;i++)
    {
        while(USART_GetStatus(uart_cfg[unit].usartx, USART_FLAG_TXE) == Reset);
        USART_SendData(uart_cfg[unit].usartx, (uint16_t)data[i]); 
		while(USART_GetStatus(uart_cfg[unit].usartx, USART_FLAG_TXE) == Reset);
	}
	__enable_irq();
}
