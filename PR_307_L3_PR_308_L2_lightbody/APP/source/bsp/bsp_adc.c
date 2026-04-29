#include "bsp_adc.h"
#include "hc32_ddl.h"
#include "app_adc.h"
/*
ADC2
PA4        Check_48V        电压检测ADC12_IN4
PA5        Check_R+        电压检测/ADC12_IN5
PA6        Check_G+        电压检测/ADC12_IN6
PA7        Check_B+        电压检测/ADC12_IN7
PB0        Check_CW1+        调光板电压检测/ADC12_IN8
PB1        Check_CW2+        调光板电压检测/ADC12_IN9
PC4        Check_WW1+        调光板电压检测ADC12_IN14
PC5        Check_WW2+        调光板电压检测ADC12_IN15

PC0        Self_WW1ADC        自适应电压检测ADC123_IN10
PC1        Self_WW2ADC        自适应电压检测ADC123_IN11
PC2        self_CW1ADC        自适应电压检测ADC123_IN12
PC3        self_CW2ADC        自适应电压检测ADC123_IN13

ADC3
PF6        MCU_NTC             MCU温度检测ADC3_IN4        GPIO_ADC
PF7        Check_YU+        调光板电压检测ADC3_IN5        GPIO_ADC
PF8        Self_RADC        自适应电压检测ADC3_IN6        GPIO_ADC
PF9        Self_GADC        自适应电压检测ADC3_IN7        GPIO_ADC
PF10		Self_BADC        自适应电压检测ADC3_IN8        GPIO_ADC
PF3        COB_NTC4        ADC3_IN9温度检测        GPIO_ADC
PF4        COB_NTC3        ADC3_IN14温度检测        GPIO_ADC
PH2        Self_YUADC        ADC3_IN16        GPIO_ADC
PH4        Check_Contact1        开路3V，短路1.57V，触点接触2.3V ADC3_IN18        GPIO_ADC
PH5        Check_Contact2        开路3V，短路1.57V，触点接触2.3V ADC3_IN19        GPIO_ADC
*/

#define APP_ADC_VREF                        (3.245f)
#define APP_ADC_ACCURACY                    (1UL << 12U)
#define APP_CAL_VOL(adcVal)                 ((float)(adcVal) * APP_ADC_VREF) / ((float)APP_ADC_ACCURACY)

#define ADC3_CHANNEL_NUM					20
#define ADC2_CHANNEL_NUM					16

#define APP_ADC_CLK                    	  (CLK_PERI_CLK_PCLK)

#define APP_ADC3_UNIT                       M4_ADC3
#define APP_ADC2_UNIT                       M4_ADC2

#define APP_ADC3_PERIP_CLK                  PWC_FCG3_ADC3
#define APP_ADC2_PERIP_CLK                  PWC_FCG3_ADC2

#define APP_ADC2_SA_SAMPLE_TIME             {35,35,35,35,35,35,35,35,35,35,35,35}
#define APP_ADC3_SA_SAMPLE_TIME             {35,35,35,35,35,35,35,35,35,35}

/* Specify the ADC channels according to the application. */
#define  APP_ADC2_SA_CH                     	( ADC_CH_Check_48V | ADC_CH_Check_R | ADC_CH_Check_G | ADC_CH_Check_B | ADC_CH_Check_CW1 | ADC_CH_Check_CW2 | \
												ADC_CH_Check_WW1   | ADC_CH_Check_WW2   | ADC_CH_Self_WW1ADC | \
												ADC_CH_Self_WW2ADC | ADC_CH_self_CW1ADC | ADC_CH_self_CW2ADC)

#define APP_ADC3_SA_CH                      ( ADC_CH_MCU_NTC | ADC_CH_Check_YU | ADC_CH_Self_RADC | ADC_CH_Self_GADC | ADC_CH_Self_BADC | ADC_CH_COB_NTC4 | \
												 ADC_CH_COB_NTC3 | ADC_CH_Self_YUADC | ADC_CH_Check_Contact1 | ADC_CH_Check_Contact2)
//#define APP_ADC3_SA_CH                     	 ( ADC_CH4 | ADC_CH5 | ADC_CH6 | ADC_CH7 | ADC_CH8 | ADC_CH9 | ADC_CH10 | ADC_CH11 | ADC_CH12  | ADC_CH14 )
//#define APP_ADC2_SA_CH                      (ADC_CH4 | ADC_CH5 | ADC_CH6 | ADC_CH7 | ADC_CH8 | ADC_CH9 | ADC_CH14)

#define APP_DMA_UNIT                        (M4_DMA1)
#define APP_DMA_CH                          (DMA_CH6)
#define APP_DMA_PERIP_CLK                   (PWC_FCG0_DMA1)
#define APP_DMA_BLOCK_SIZE                  (20U)					
#define APP_DMA_TRANS_COUNT                 (1U)					
#define APP_DMA_DATA_WIDTH                  (DMA_DATAWIDTH_16BIT)
#define APP_DMA_TRIG_SRC                    (EVT_ADC3_EOCA)
#define APP_DMA_SRC_ADDR                    (&M4_ADC3->DR0)		 

/*----------------------------????DMA IRQ----------------------------------*/
#define APP_DMA_IRQ_SRC                     (INT_DMA1_BTC6)			
#define APP_DMA_IRQn                        (Int005_IRQn)
#define APP_DMA_INT_PRIO                    (DDL_IRQ_PRIORITY_04)
#define APP_DMA_TRANS_CPLT_FLAG             (DMA_BTC_INT_CH6)		


#define APP2_DMA_UNIT                        (M4_DMA1)
#define APP2_DMA_CH                          (DMA_CH0)
#define APP2_DMA_PERIP_CLK                   (PWC_FCG0_DMA1)
#define APP2_DMA_BLOCK_SIZE                  (16U)					//块大小，所有通道作为一个块
#define APP2_DMA_TRANS_COUNT                 (1U)					//传输次数（0表示无限次传输）
#define APP2_DMA_DATA_WIDTH                  (DMA_DATAWIDTH_16BIT)
#define APP2_DMA_TRIG_SRC                    (EVT_ADC2_EOCA)
#define APP2_DMA_SRC_ADDR                    (&M4_ADC2->DR0)		    //是从通道0开始

/*----------------------------添加DMA IRQ----------------------------------*/
#define APP2_DMA_IRQ_SRC                     (INT_DMA1_BTC0)			//DMA块传输完成中断(也是所有的通道为一块)
#define APP2_DMA_IRQn                        (Int006_IRQn)
#define APP2_DMA_INT_PRIO                    (DDL_IRQ_PRIORITY_04)//(DDL_IRQ_PRIORITY_10)
#define APP2_DMA_TRANS_CPLT_FLAG             (DMA_BTC_INT_CH0)		//DMA中断通道

uint16_t m_au16AdcSaVal[ADC3_CHANNEL_NUM] = {0};
uint16_t m_au16AdcSaVal2[ADC2_CHANNEL_NUM] = {0};

uint8_t DMA_Finish_Flag = 0U;
uint8_t DMA_Finish_Flag2 = 0U;
void (*Adc1_Dma_CallBack)(uint16_t *pBuff) = NULL;
void (*Adc2_Dma_CallBack)(uint16_t *pBuff) = NULL;

static void AdcInitConfig(void)
{
    stc_adc_init_t stcInit;

    /* Set a default value. */
    (void)ADC_StructInit(&stcInit);

    /* 1. Modify the default value depends on the application. It is not needed in this example. */
	stcInit.u16AutoClrCmd = ADC_AUTO_CLR_ENABLE;    //自动清除，指被DMA读取完转换数据（ADC_DRx）后，ADC_DRx将被自动清除
	stcInit.u16DataAlign = ADC_DATA_ALIGN_RIGHT;
	stcInit.u16Resolution = ADC_RESOLUTION_12BIT;
	stcInit.u16ScanMode = ADC_MODE_SA_CONT;			//序列A连续扫描（即连续循环采集）

    /* 2. Enable ADC peripheral clock. */

    PWC_Fcg3PeriphClockCmd(APP_ADC3_PERIP_CLK, Enable);

	PWC_Fcg3PeriphClockCmd(APP_ADC2_PERIP_CLK, Enable);
	
    /* 3. Initializes ADC. */

    (void)ADC_Init(APP_ADC3_UNIT, &stcInit);

	(void)ADC_Init(APP_ADC2_UNIT, &stcInit);
}

static void AdcChannelConfig(void)
{
    uint8_t au8AdcSASplTime[] = APP_ADC3_SA_SAMPLE_TIME;
	uint8_t au8AdcSASplTime2[] = APP_ADC2_SA_SAMPLE_TIME;
	/*ADC3*/

//    AdcSetChannelPinAnalogMode(APP_ADC3_UNIT, APP_ADC3_SA_CH);

    /* 2. Enable the ADC channels. */
    (void)ADC_ChannelCmd(APP_ADC3_UNIT, ADC_SEQ_A, \
                         APP_ADC3_SA_CH, au8AdcSASplTime, \
                         Enable);
	/*ADC2*/
//	AdcSetChannelPinAnalogMode(APP_ADC2_UNIT, APP_ADC2_SA_CH);
	(void)ADC_ChannelCmd(APP_ADC2_UNIT, ADC_SEQ_A, \
                         APP_ADC2_SA_CH, au8AdcSASplTime2, \
                         Enable);
    /* 3. Set the number of averaging sampling and enable the channel, if needed. */
#if (defined APP_ADC_AVG_CH) && (APP_ADC_AVG_CH != 0U)
    ADC_AvgChannelConfig(APP_ADC_UNIT, APP_ADC_AVG_CNT);
    ADC_AvgChannelCmd(APP_ADC_UNIT, APP_ADC_AVG_CH, Enable);
#endif
}
uint16_t last_au16AdcSaVal[ADC3_CHANNEL_NUM] = {0};
uint16_t last_au16AdcSaVal2[ADC2_CHANNEL_NUM] = {0};
static void DMA_Btc1_IrqCallback(void)
{
    DMA_ClearTransIntStatus(APP_DMA_UNIT, APP_DMA_TRANS_CPLT_FLAG);
	ADC_Disable();
    DMA_Finish_Flag = 1;
	memcpy(last_au16AdcSaVal, m_au16AdcSaVal, sizeof(last_au16AdcSaVal));
	if(NULL != Adc2_Dma_CallBack)
		Adc2_Dma_CallBack(last_au16AdcSaVal);
}


static void DMA_Btc0_IrqCallback(void)
{
	//adc2
    DMA_ClearTransIntStatus(APP2_DMA_UNIT, APP2_DMA_TRANS_CPLT_FLAG);
	ADC2_Disable();
    DMA_Finish_Flag2 = 1;
	memcpy(last_au16AdcSaVal2, m_au16AdcSaVal2, sizeof(last_au16AdcSaVal2));
	if(NULL != Adc1_Dma_CallBack)
		Adc1_Dma_CallBack(last_au16AdcSaVal2);
}

/**
 * @brief  Interrupt configuration.
 * @param  None
 * @retval None
 */
static void AdcDmaIrqConfig(void)
{
    stc_irq_signin_config_t stcIrqSignConfig;

    stcIrqSignConfig.enIntSrc    = APP_DMA_IRQ_SRC;
    stcIrqSignConfig.enIRQn      = APP_DMA_IRQn;
    stcIrqSignConfig.pfnCallback = &DMA_Btc1_IrqCallback;

    (void)INTC_IrqSignIn(&stcIrqSignConfig);
    DMA_ClearTransIntStatus(APP_DMA_UNIT, APP_DMA_TRANS_CPLT_FLAG);

    /* NVIC setting */
    NVIC_ClearPendingIRQ(APP_DMA_IRQn);
    NVIC_SetPriority(APP_DMA_IRQn, APP_DMA_INT_PRIO);
    NVIC_EnableIRQ(APP_DMA_IRQn);
}

static void AdcDmaIrqConfig2(void)
{
    stc_irq_signin_config_t stcIrqSignConfig;

    stcIrqSignConfig.enIntSrc    = APP2_DMA_IRQ_SRC;
    stcIrqSignConfig.enIRQn      = APP2_DMA_IRQn;
    stcIrqSignConfig.pfnCallback = &DMA_Btc0_IrqCallback;

    (void)INTC_IrqSignIn(&stcIrqSignConfig);
    DMA_ClearTransIntStatus(APP2_DMA_UNIT, APP2_DMA_TRANS_CPLT_FLAG);

    /* NVIC setting */
    NVIC_ClearPendingIRQ(APP2_DMA_IRQn);
    NVIC_SetPriority(APP2_DMA_IRQn, APP2_DMA_INT_PRIO);
    NVIC_EnableIRQ(APP2_DMA_IRQn);
}

static void AdcDmaConfig(void)
{
    stc_dma_init_t stcDmaInit;
    stc_dma_rpt_init_t stcDmaRptInit;

    /* Enable DMA peripheral clock and AOS function. */
	PWC_FCG0_Unlock();	
    PWC_Fcg0PeriphClockCmd((APP_DMA_PERIP_CLK | PWC_FCG0_AOS), Enable);
	PWC_Fcg0PeriphClockCmd((APP2_DMA_PERIP_CLK | PWC_FCG0_AOS), Enable);
	PWC_FCG0_Lock();
	/* 选择DMA的触发源 */
    DMA_SetTriggerSrc(APP_DMA_UNIT, APP_DMA_CH, APP_DMA_TRIG_SRC);

    (void)DMA_StructInit(&stcDmaInit);
    stcDmaInit.u32IntEn     = DMA_INT_ENABLE;		//DMA中断（使能）
    stcDmaInit.u32BlockSize = APP_DMA_BLOCK_SIZE; 
    stcDmaInit.u32TransCnt  = APP_DMA_TRANS_COUNT;  //传输次数（0表示无限次传输）
    stcDmaInit.u32DataWidth = APP_DMA_DATA_WIDTH;
    stcDmaInit.u32DestAddr  = (uint32_t)(&m_au16AdcSaVal[0U]); //DMA 的寄存器只支持 32bit 读写，8/16bit 读写操作无效。
    stcDmaInit.u32SrcAddr   = (uint32_t)APP_DMA_SRC_ADDR;
    stcDmaInit.u32SrcInc    = DMA_SRC_ADDR_INC;		//源和目标地址都要增加
    stcDmaInit.u32DestInc   = DMA_DEST_ADDR_INC;
    (void)DMA_Init(APP_DMA_UNIT, APP_DMA_CH, &stcDmaInit);

    (void)DMA_RepeatStructInit(&stcDmaRptInit);		//DMA重复触发
    stcDmaRptInit.u32SrcRptEn    = DMA_SRC_RPT_ENABLE;
    stcDmaRptInit.u32SrcRptSize  = APP_DMA_BLOCK_SIZE;
    stcDmaRptInit.u32DestRptEn   = DMA_DEST_RPT_ENABLE;
    stcDmaRptInit.u32DestRptSize = APP_DMA_BLOCK_SIZE;
    (void)DMA_RepeatInit(APP_DMA_UNIT, APP_DMA_CH, &stcDmaRptInit);

    AdcDmaIrqConfig();

    DMA_Cmd(APP_DMA_UNIT, Enable);
//    DMA_ChannelCmd(APP_DMA_UNIT, APP_DMA_CH, Enable);
//	ADC_Start(APP_ADC_UNIT);
}

static void AdcDmaConfig2(void)
{
    stc_dma_init_t stcDmaInit;
    stc_dma_rpt_init_t stcDmaRptInit;

    /* Enable DMA peripheral clock and AOS function. */
	PWC_FCG0_Unlock();	
    PWC_Fcg0PeriphClockCmd((APP2_DMA_PERIP_CLK | PWC_FCG0_AOS), Enable);
	PWC_FCG0_Lock();
	/* 选择DMA的触发源 */
    DMA_SetTriggerSrc(APP2_DMA_UNIT, APP2_DMA_CH, APP2_DMA_TRIG_SRC);

    (void)DMA_StructInit(&stcDmaInit);
    stcDmaInit.u32IntEn     = DMA_INT_ENABLE;		//DMA中断（使能）
    stcDmaInit.u32BlockSize = APP2_DMA_BLOCK_SIZE; 
    stcDmaInit.u32TransCnt  = APP2_DMA_TRANS_COUNT;  //传输次数（0表示无限次传输）
    stcDmaInit.u32DataWidth = APP2_DMA_DATA_WIDTH;
    stcDmaInit.u32DestAddr  = (uint32_t)(&m_au16AdcSaVal2[0U]); //DMA 的寄存器只支持 32bit 读写，8/16bit 读写操作无效。
    stcDmaInit.u32SrcAddr   = (uint32_t)APP2_DMA_SRC_ADDR;
    stcDmaInit.u32SrcInc    = DMA_SRC_ADDR_INC;		//源和目标地址都要增加
    stcDmaInit.u32DestInc   = DMA_DEST_ADDR_INC;
    (void)DMA_Init(APP2_DMA_UNIT, APP2_DMA_CH, &stcDmaInit);

    (void)DMA_RepeatStructInit(&stcDmaRptInit);		//DMA重复触发
    stcDmaRptInit.u32SrcRptEn    = DMA_SRC_RPT_ENABLE;
    stcDmaRptInit.u32SrcRptSize  = APP2_DMA_BLOCK_SIZE;
    stcDmaRptInit.u32DestRptEn   = DMA_DEST_RPT_ENABLE;
    stcDmaRptInit.u32DestRptSize = APP2_DMA_BLOCK_SIZE;
    (void)DMA_RepeatInit(APP2_DMA_UNIT, APP2_DMA_CH, &stcDmaRptInit);

    AdcDmaIrqConfig2();

    DMA_Cmd(APP2_DMA_UNIT, Enable);
//    DMA_ChannelCmd(APP_DMA_UNIT, APP_DMA_CH, Enable);
//	ADC_Start(APP_ADC_UNIT);
}

void ADC_Enable(void)
{
//	DMA_Cmd(APP_DMA_UNIT, Enable);  //影响别的DMA工作
	DMA_ChannelCmd(APP_DMA_UNIT, APP_DMA_CH, Enable);
	ADC_Start(APP_ADC3_UNIT);
}

void ADC2_Enable(void)
{
//	DMA_Cmd(APP_DMA_UNIT, Enable);  //影响别的DMA工作
	DMA_ChannelCmd(APP2_DMA_UNIT, APP2_DMA_CH, Enable);
	ADC_Start(APP_ADC2_UNIT);
}

void ADC_Disable(void)
{
//	DMA_Cmd(APP_DMA_UNIT, Disable);
	DMA_ChannelCmd(APP_DMA_UNIT, APP_DMA_CH, Disable);
	ADC_Stop(APP_ADC3_UNIT);
}

void ADC2_Disable(void)
{
//	DMA_Cmd(APP_DMA_UNIT, Disable);
	DMA_ChannelCmd(APP2_DMA_UNIT, APP2_DMA_CH, Disable);
	ADC_Stop(APP_ADC2_UNIT);
}

static void get_adc_value(void)
{
	for(uint8_t i = 0; i <= 100; i ++)
	{
		get_adc_para_val();
		delay_us(200);
	}
}

void Adc_init(void)
{
    AdcInitConfig();
    AdcChannelConfig();
	AdcDmaConfig();	
	AdcDmaConfig2();
	get_adc_value();
}

/* 求10个数的平均值 */
static uint16_t Data_Average(uint16_t *data, uint8_t num)     
{
    uint8_t i;
    uint16_t minnum = 0;
    uint16_t maxnum = 0;
    uint16_t temp = 0;
    uint16_t DataBuf = 0;

    for(i=0; i<num; i++)     //找出最大与最小值
    {
        temp = data[i];
        DataBuf += temp;
        
        if(i==0)
        {
            maxnum = temp;
            minnum = temp;
        }
        else
        {
            if(temp>maxnum)
                maxnum = temp;
            else
            {
                if(temp<minnum)
                    minnum = temp;
            }
        }
    }

    DataBuf = DataBuf-(maxnum+minnum);      //总数减去最大值与最小值
    return DataBuf/8;
}


/*
函数功能: 获取ADC3通道1电压值 单位：mV
*/
uint16_t Get_ADC3_Value(uint8_t Channel)
{
	uint8_t i;
	uint16_t Adc_Val = 0;
	uint16_t Adc_Sum[10] = {0};

	for(i=0; i<10; i++)
	{
		ADC_Enable();   		  //开始转化
		while (!DMA_Finish_Flag)  //等待转化完成
		{	
			DDL_DelayUS(1);//这里可能需要加一个延时
		}    	
		Adc_Sum[i] += m_au16AdcSaVal[Channel];
		DMA_Finish_Flag = 0;
	}
	Adc_Val = Data_Average(Adc_Sum, 10);
	return Adc_Val;
}

uint16_t Get_ADC2_Value(uint8_t Channel)
{
	uint8_t i;
	uint16_t Adc_Val = 0;
	uint16_t Adc_Sum[10] = {0};

	for(i=0; i<10; i++)
	{
		ADC2_Enable();   		  //开始转化
		while (!DMA_Finish_Flag2)  //等待转化完成
		{	
			DDL_DelayUS(1);//这里可能需要加一个延时
		}    	
		Adc_Sum[i] += m_au16AdcSaVal2[Channel];
		DMA_Finish_Flag2 = 0;
	}
	Adc_Val = Data_Average(Adc_Sum, 10);
	return Adc_Val;
}

