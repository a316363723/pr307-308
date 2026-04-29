#include "hal_adc.h"
#include "hc32_ddl.h"
#include "stdint.h"

#define ADC_STRUCT_DEFINE(_name, channel, ...)      \
    uint8_t _name##_time[] = {__VA_ARGS__};                        \
    const uint32_t _name##_ch = channel;

ADC_STRUCT_DEFINE(adc1, ( ADC_CH4 | ADC_CH5 | ADC_CH8 | ADC_CH9), 100, 100, 100, 100);
ADC_STRUCT_DEFINE(adc2, 0, 100, 100, 100, 100);
ADC_STRUCT_DEFINE(adc3, ( ADC_CH4 | ADC_CH5 | ADC_CH8 | ADC_CH9 | ADC_CH14 | ADC_CH15 | ADC_CH16 | ADC_CH17 | ADC_CH18 | ADC_CH19 ), \
                  200, 200, 200, 200, 200, 200, 200, 200, 200, 200);

#define ADC_GET_TIME_NAME(_name)   (_name##_time)
#define ADC_GET_CH_NAME(_name)   (_name##_ch)
static void (*dmx_rx_btc_int_cb[HAL_ADC_NUM])(void);
static void adc_dma1_IRQHandler(void);
static void adc_dma2_IRQHandler(void);
static void adc_dma3_IRQHandler(void);

struct hc4a0_adc_cfg
{

    M4_ADC_TypeDef *adcx;
    uint32_t fcg_periph;
    uint32_t channel;
    uint8_t *spl_time;
    uint16_t avg_cnt;
    struct hc4a0_dma_cgf
    {
        M4_DMA_TypeDef *dmax;
        uint8_t  channel;
        uint32_t trans_int;
        uint32_t fcg_periph;
    } dma_rx;
    struct hc4a0_dma_irq
    {
        stc_irq_signin_config_t stc_irq_signin_cfg;
        uint32_t Priority;
        en_event_src_t src_en;
    } irq_btc ;
};


static const struct hc4a0_adc_cfg adc_cfg[HAL_ADC_NUM] =
{
    {
        .adcx = M4_ADC1,
        .channel = ADC_GET_CH_NAME(adc1),
        .spl_time = ADC_GET_TIME_NAME(adc1),
        .avg_cnt = ADC_AVG_CNT_8,
        .fcg_periph = PWC_FCG3_ADC1,
        .dma_rx =
        {
            .dmax = M4_DMA2,
            .channel = DMA_CH4,
            .trans_int = DMA_BTC_INT_CH4,
            .fcg_periph = PWC_FCG0_DMA2,
        },
        .irq_btc = {
            .stc_irq_signin_cfg = { INT_DMA2_BTC4, Int024_IRQn, adc_dma1_IRQHandler },
            .Priority = DDL_IRQ_PRIORITY_15,
            .src_en = EVT_ADC1_EOCA,

        },


    },
    {
        .adcx = M4_ADC2,
        .channel = ADC_GET_CH_NAME(adc2),
        .spl_time = ADC_GET_TIME_NAME(adc2),
        .avg_cnt = ADC_AVG_CNT_8,
        .fcg_periph = PWC_FCG3_ADC2,
        .dma_rx =
        {
            .dmax = M4_DMA2,
            .channel = DMA_CH5,
            .trans_int = DMA_BTC_INT_CH5,
            .fcg_periph = PWC_FCG0_DMA2,
        },
        .irq_btc = {
            .stc_irq_signin_cfg = { INT_DMA2_BTC5, Int025_IRQn, adc_dma2_IRQHandler },
            .Priority = DDL_IRQ_PRIORITY_15,
            .src_en = EVT_ADC2_EOCA,

        },
    },
    {
        .adcx = M4_ADC3,
        .channel = ADC_GET_CH_NAME(adc3),
        .spl_time = ADC_GET_TIME_NAME(adc3),
        .avg_cnt = ADC_AVG_CNT_256,
        .fcg_periph = PWC_FCG3_ADC3,
        .dma_rx =
        {
            .dmax = M4_DMA2,
            .channel = DMA_CH6,
            .trans_int = DMA_BTC_INT_CH6,
            .fcg_periph = PWC_FCG0_DMA2,
        },
        .irq_btc = {
            .stc_irq_signin_cfg = { INT_DMA2_BTC6, Int026_IRQn, adc_dma3_IRQHandler },
            .Priority = DDL_IRQ_PRIORITY_15,
            .src_en = EVT_ADC3_EOCA,

        },
    },

};

void AdcSetPinAnalogMode(const M4_ADC_TypeDef *ADCx, uint8_t u8PinNum)
{
    typedef struct
    {
        uint8_t  u8Port;
        uint16_t u16Pin;
    } stc_adc_pin_t;

    stc_gpio_init_t stcGpioInit;
    stc_adc_pin_t astcADC12[ADC1_CH_COUNT] = { \
        {GPIO_PORT_A, GPIO_PIN_00}, {GPIO_PORT_A, GPIO_PIN_01}, \
        {GPIO_PORT_A, GPIO_PIN_02}, {GPIO_PORT_A, GPIO_PIN_03}, \
        {GPIO_PORT_A, GPIO_PIN_04}, {GPIO_PORT_A, GPIO_PIN_05}, \
        {GPIO_PORT_A, GPIO_PIN_06}, {GPIO_PORT_A, GPIO_PIN_07}, \
        {GPIO_PORT_B, GPIO_PIN_00}, {GPIO_PORT_B, GPIO_PIN_01}, \
        {GPIO_PORT_C, GPIO_PIN_00}, {GPIO_PORT_C, GPIO_PIN_01}, \
        {GPIO_PORT_C, GPIO_PIN_02}, {GPIO_PORT_C, GPIO_PIN_03}, \
        {GPIO_PORT_C, GPIO_PIN_04}, {GPIO_PORT_C, GPIO_PIN_05}, \
    };
    stc_adc_pin_t astcADC3[ADC3_CH_COUNT] = { \
        {GPIO_PORT_A, GPIO_PIN_00}, {GPIO_PORT_A, GPIO_PIN_01}, \
        {GPIO_PORT_A, GPIO_PIN_02}, {GPIO_PORT_A, GPIO_PIN_03}, \
        {GPIO_PORT_F, GPIO_PIN_06}, {GPIO_PORT_F, GPIO_PIN_07}, \
        {GPIO_PORT_F, GPIO_PIN_08}, {GPIO_PORT_F, GPIO_PIN_09}, \
        {GPIO_PORT_F, GPIO_PIN_10}, {GPIO_PORT_F, GPIO_PIN_03}, \
        {GPIO_PORT_C, GPIO_PIN_00}, {GPIO_PORT_C, GPIO_PIN_01}, \
        {GPIO_PORT_C, GPIO_PIN_02}, {GPIO_PORT_C, GPIO_PIN_03}, \
        {GPIO_PORT_F, GPIO_PIN_04}, {GPIO_PORT_F, GPIO_PIN_05}, \
        {GPIO_PORT_H, GPIO_PIN_02}, {GPIO_PORT_H, GPIO_PIN_03}, \
        {GPIO_PORT_H, GPIO_PIN_04}, {GPIO_PORT_H, GPIO_PIN_05}, \
    };

    (void)GPIO_StructInit(&stcGpioInit);
    stcGpioInit.u16PinAttr = PIN_ATTR_ANALOG;

    if (ADCx == M4_ADC3)
    {
        (void)GPIO_Init(astcADC3[u8PinNum].u8Port, astcADC3[u8PinNum].u16Pin, &stcGpioInit);
    }
    else
    {
        (void)GPIO_Init(astcADC12[u8PinNum].u8Port, astcADC12[u8PinNum].u16Pin, &stcGpioInit);
    }
}


void AdcSetChannelPinAnalogMode(const M4_ADC_TypeDef *ADCx, uint32_t u32Channel)
{
    uint8_t u8PinNum;

    u8PinNum = 0U;

    while (u32Channel != 0U)
    {
        if ((u32Channel & 0x1UL) != 0UL)
        {
            AdcSetPinAnalogMode(ADCx, u8PinNum);
        }

        u32Channel >>= 1U;
        u8PinNum++;
    }
}


void hal_adc_init(enum hal_adc unit)
{
    stc_adc_init_t stcInit;
    (void)ADC_StructInit(&stcInit);
    PWC_Fcg3PeriphClockCmd(adc_cfg[unit].fcg_periph, Enable);
    (void)ADC_Init(adc_cfg[unit].adcx, &stcInit);

    AdcSetChannelPinAnalogMode(adc_cfg[unit].adcx, adc_cfg[unit].channel);

    (void)ADC_ChannelCmd(adc_cfg[unit].adcx, ADC_SEQ_A, adc_cfg[unit].channel, adc_cfg[unit].spl_time,  Enable);
    
    ADC_SetScanMode(adc_cfg[unit].adcx , ADC_MODE_SA_CONT);
    ADC_AvgChannelConfig(adc_cfg[unit].adcx, adc_cfg[unit].avg_cnt);
    ADC_AvgChannelCmd(adc_cfg[unit].adcx, adc_cfg[unit].channel, Enable);
    
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


void hal_adc_receive_dma_start(enum hal_adc unit, const uint32_t *p_data, uint16_t size)
{
    stc_dma_init_t stcDmaInit;
    stc_dma_rpt_init_t stcDmaRptInit;

    /* Enable DMA peripheral clock and AOS function. */
    PWC_Fcg0PeriphClockCmd( (adc_cfg[unit].dma_rx.fcg_periph | PWC_FCG0_AOS), Enable);

    DMA_SetTriggerSrc(adc_cfg[unit].dma_rx.dmax, adc_cfg[unit].dma_rx.channel, adc_cfg[unit].irq_btc.src_en);

    (void)DMA_StructInit(&stcDmaInit);
    stcDmaInit.u32IntEn     = DMA_INT_ENABLE;
    stcDmaInit.u32BlockSize = (ADC_CH_NUM_19 - ADC_CH_NUM_0) + 1 ;
    stcDmaInit.u32TransCnt  = size;
    stcDmaInit.u32DataWidth = DMA_DATAWIDTH_16BIT;
    stcDmaInit.u32DestAddr  = (uint32_t)(p_data);
    stcDmaInit.u32SrcAddr   = (uint32_t)&adc_cfg[unit].adcx->DR0;
    stcDmaInit.u32SrcInc    = DMA_SRC_ADDR_INC;
    stcDmaInit.u32DestInc   = DMA_DEST_ADDR_INC;
    (void)DMA_Init(adc_cfg[unit].dma_rx.dmax, adc_cfg[unit].dma_rx.channel, &stcDmaInit);

    (void)DMA_RepeatStructInit(&stcDmaRptInit);
    stcDmaRptInit.u32SrcRptEn    = DMA_SRC_RPT_ENABLE;
    stcDmaRptInit.u32SrcRptSize  = (ADC_CH_NUM_19 - ADC_CH_NUM_0) + 1;
    stcDmaRptInit.u32DestRptEn   = DMA_DEST_RPT_ENABLE;
    stcDmaRptInit.u32DestRptSize = (ADC_CH_NUM_19 - ADC_CH_NUM_0) + 1;
    (void)DMA_RepeatInit(adc_cfg[unit].dma_rx.dmax, adc_cfg[unit].dma_rx.channel, &stcDmaRptInit);

    DMA_ClearTransIntStatus(adc_cfg[unit].dma_rx.dmax, adc_cfg[unit].dma_rx.trans_int);

    DMA_Cmd(adc_cfg[unit].dma_rx.dmax, Enable);
    DMA_ChannelCmd(adc_cfg[unit].dma_rx.dmax, adc_cfg[unit].dma_rx.channel, Enable);

}


void hal_adc_irq_register(enum hal_adc unit, enum hal_adc_irq irq_type, void (*irq_cb)(void))
{
    stc_irq_signin_config_t stcIrqSignConfig;

    switch(irq_type)
    {
        case HAL_ADC_IRQ_BTC:
            dmx_rx_btc_int_cb[unit] = irq_cb;
            stcIrqSignConfig.enIntSrc    = adc_cfg[unit].irq_btc.stc_irq_signin_cfg.enIntSrc;
            stcIrqSignConfig.enIRQn      = adc_cfg[unit].irq_btc.stc_irq_signin_cfg.enIRQn;
            stcIrqSignConfig.pfnCallback = adc_cfg[unit].irq_btc.stc_irq_signin_cfg.pfnCallback;
            Irq_Priority_Handler(&stcIrqSignConfig, adc_cfg[unit].irq_btc.Priority);
            break;
		
    }
}

static void adc_dma1_IRQHandler(void)
{
    const enum hal_adc unit = HAL_ADC_1;
    DMA_ClearTransIntStatus(adc_cfg[unit].dma_rx.dmax, adc_cfg[unit].dma_rx.trans_int);
    dmx_rx_btc_int_cb[HAL_ADC_1]();
}


static void adc_dma2_IRQHandler(void)
{
    const enum hal_adc unit = HAL_ADC_2;
    DMA_ClearTransIntStatus(adc_cfg[unit].dma_rx.dmax, adc_cfg[unit].dma_rx.trans_int);
    dmx_rx_btc_int_cb[HAL_ADC_2]();
}


static void adc_dma3_IRQHandler(void)
{
    const enum hal_adc unit = HAL_ADC_3;
    DMA_ClearTransIntStatus(adc_cfg[unit].dma_rx.dmax, adc_cfg[unit].dma_rx.trans_int);
    dmx_rx_btc_int_cb[HAL_ADC_3]();
}



void adc_capture_start_it(enum hal_adc unit )
{
    ADC_Start(adc_cfg[unit].adcx);
}

void adc_capture_stop_it(enum hal_adc unit )
{
    ADC_Stop(adc_cfg[unit].adcx);
}


