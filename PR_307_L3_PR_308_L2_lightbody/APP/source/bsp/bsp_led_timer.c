#include "bsp_led_timer.h"
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
#define TIM4_PERIOD_VALUE                       (3000UL - 1UL)

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

static void Dma_Timer4_2_init(void)
{
    stc_dma_init_t stcDmaInit;
    stc_dma_rpt_init_t stcDmaRepeatInit;
    stc_dma_nonseq_init_t stcDmaNonSeqInit;

	PWC_Fcg0PeriphClockCmd((DMA_TMR4_PERIP_CLK | PWC_FCG0_AOS), Enable);
    
	(void)DMA_StructInit(&stcDmaInit);
    stcDmaInit.u32IntEn      = DMA_INT_ENABLE;
    stcDmaInit.u32BlockSize  = 3U;//每组6个数据
    stcDmaInit.u32TransCnt   = 0U;//无限循环
    stcDmaInit.u32DataWidth  = DMA_DATAWIDTH_16BIT;
    stcDmaInit.u32DestAddr   = (uint32_t)(&M4_TMR4_2->OCCRUH);
    stcDmaInit.u32SrcAddr    = (uint32_t)(g_led_pwm_arg.pwm_buffer_one);
    stcDmaInit.u32SrcInc = DMA_SRC_ADDR_INC;
    stcDmaInit.u32DestInc = DMA_DEST_ADDR_INC;
    (void)DMA_Init(DMA_TMR4_UINT, DMA_TMR4_2_CH, &stcDmaInit);

    (void)DMA_RepeatStructInit(&stcDmaRepeatInit);
    stcDmaRepeatInit.u32DestRptEn = DMA_SRC_RPT_ENABLE;
	stcDmaRepeatInit.u32SrcRptEn = DMA_DEST_RPT_ENABLE;
    stcDmaRepeatInit.u32SrcRptSize = 12U;//4组，共24个数据 以24为单位重载地址传输
    stcDmaRepeatInit.u32DestRptSize = 3U; //定时器6个通道  以6为单位重载地址传输
    (void)DMA_RepeatInit(DMA_TMR4_UINT, DMA_TMR4_2_CH, &stcDmaRepeatInit);
    
    (void)DMA_NonSeqStructInit(&stcDmaNonSeqInit);
    stcDmaNonSeqInit.u32DestNonSeqEn = DMA_DEST_NS_ENABLE;
    stcDmaNonSeqInit.u32DestNonSeqOfs = 2U;
    stcDmaNonSeqInit.u32SrcNonSeqCnt  = 1U;
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
    stcDmaInit.u32SrcAddr    = (uint32_t)(g_led_pwm_arg.pwm_buffer_two);
    stcDmaInit.u32SrcInc = DMA_SRC_ADDR_INC;
    stcDmaInit.u32DestInc = DMA_DEST_ADDR_INC;
    (void)DMA_Init(DMA_TMR4_UINT, DMA_TMR4_3_CH, &stcDmaInit);

    (void)DMA_RepeatStructInit(&stcDmaRepeatInit);
    stcDmaRepeatInit.u32DestRptEn = DMA_SRC_RPT_ENABLE;
	stcDmaRepeatInit.u32SrcRptEn = DMA_DEST_RPT_ENABLE;
    stcDmaRepeatInit.u32SrcRptSize = 16U;//4组，共24个数据 以24为单位重载地址传输
    stcDmaRepeatInit.u32DestRptSize = 4U; //定时器6个通道  以6为单位重载地址传输
    (void)DMA_RepeatInit(DMA_TMR4_UINT, DMA_TMR4_3_CH, &stcDmaRepeatInit);
    
    (void)DMA_NonSeqStructInit(&stcDmaNonSeqInit);
    stcDmaNonSeqInit.u32DestNonSeqEn = DMA_DEST_NS_ENABLE;
    stcDmaNonSeqInit.u32DestNonSeqOfs = 2U;
    stcDmaNonSeqInit.u32SrcNonSeqCnt  = 1U;
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
    stcTmr4OcInit.u16CompareVal = 0x0000;
	stcTmr4OcInit.u16OccrLinkTransfer = TMR4_OCO_OCCR_LINK_TRANSFER_ENABLE;
	stcTmr4OcInit.u16OccrBufMode = TMR4_OCO_OCCR_BUF_CNT_PEAK;
	
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
    unTmr4OcOcmrh.OCMRx_f.OPZRH  = TMR4_OCO_OP_HOLD;    /* Bit[11:10] 11 */
    unTmr4OcOcmrh.OCMRx_f.OPNPKH = TMR4_OCO_OP_HIGH;    /* Bit[13:12] 00 */
    unTmr4OcOcmrh.OCMRx_f.OPNZRH = TMR4_OCO_OP_HIGH;    /* Bit[15:14] 00 */
	unTmr4OcOcmrh.
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
    unTmr4OcOcmrl.OCMRx_f.OPZRL   = TMR4_OCO_OP_HOLD;    /* bit[11:10] 11 */
    unTmr4OcOcmrl.OCMRx_f.OPNPKL  = TMR4_OCO_OP_HIGH;    /* bit[13:12] 00 */
    unTmr4OcOcmrl.OCMRx_f.OPNZRL  = TMR4_OCO_OP_HOLD;    /* bit[15:14] 00 */
    unTmr4OcOcmrl.OCMRx_f.EOPNDCL = TMR4_OCO_OP_HOLD;    /* bit[17:16] 00 */
    unTmr4OcOcmrl.OCMRx_f.EOPNUCL = TMR4_OCO_OP_HOLD;    /* bit[19:18] 00 */
    unTmr4OcOcmrl.OCMRx_f.EOPDCL  = TMR4_OCO_OP_HOLD;    /* bit[21:20] 11 */
    unTmr4OcOcmrl.OCMRx_f.EOPPKL  = TMR4_OCO_OP_HIGH;    /* bit[23:22] 11 */
    unTmr4OcOcmrl.OCMRx_f.EOPUCL  = TMR4_OCO_OP_LOW;    /* bit[25:24] 11 */
    unTmr4OcOcmrl.OCMRx_f.EOPZRL  = TMR4_OCO_OP_HOLD;    /* bit[27:26] 11 */
    unTmr4OcOcmrl.OCMRx_f.EOPNPKL = TMR4_OCO_OP_HOLD;    /* bit[29:28] 00 */
    unTmr4OcOcmrl.OCMRx_f.EOPNZRL = TMR4_OCO_OP_HOLD;    /* bit[31:30] 00 */
    TMR4_OCO_SetLowChCompareMode(TMR4_2_UNIT, TIM4_2_UL_G_W3_CH, &unTmr4OcOcmrl);
	
	/* TMR4 OC: enable */
    TMR4_OCO_SetOutputCompare(TMR4_2_UNIT, TIM4_2_UH_R_G_CH , TMR4_OCO_ENABLE);
    TMR4_OCO_SetOutputCompare(TMR4_2_UNIT, TIM4_2_UL_G_W3_CH, TMR4_OCO_ENABLE);
    TMR4_OCO_SetOutputCompare(TMR4_2_UNIT, TIM4_2_VH_B_C3_CH, TMR4_OCO_ENABLE);
	
    /************************* Configure TMR4 PWM && TMR4 EVT*****************************/

    /* TMR4 PWM: initialize */
    (void)TMR4_PWM_StructInit(&stcTmr4PwmInit);
    stcTmr4PwmInit.u16Mode = TMR4_PWM_THROUGH_MODE;//直通模式
	stcTmr4PwmInit.u32OxHPortOutMode = TMR4_PWM_PORT_OUTPUT_NORMAL;
	stcTmr4PwmInit.u32OxLPortOutMode = TMR4_PWM_PORT_OUTPUT_NORMAL;//OExy使能输出
    (void)TMR4_PWM_Init(TMR4_2_UNIT, TIM4_2_UH_R_G_CH , &stcTmr4PwmInit);
    (void)TMR4_PWM_Init(TMR4_2_UNIT, TIM4_2_UL_G_W3_CH, &stcTmr4PwmInit);
    (void)TMR4_PWM_Init(TMR4_2_UNIT, TIM4_2_VH_B_C3_CH, &stcTmr4PwmInit);
    
    
    (void)TMR4_SEVT_StructInit(&stcTmr4EventInit);
	stcTmr4EventInit.u16UpMatchCmd = TMR4_SEVT_UP_ENABLE;
	stcTmr4EventInit.u16DownMatchCmd = TMR4_SEVT_DOWN_ENABLE;
	stcTmr4EventInit.u16PeakMatchCmd = TMR4_SEVT_PEAK_ENABLE;
	stcTmr4EventInit.u16ZeroMatchCmd = TMR4_SEVT_ZERO_ENABLE;
    stcTmr4EventInit.u16CompareVal = TIM4_PERIOD_VALUE;
    (void)TMR4_SEVT_Init(TMR4_2_UNIT, TIM4_2_UH_R_G_CH, &stcTmr4EventInit);
    /* TMR4 event: set output event signal */
    TMR4_SEVT_SetPortOutputEventSig(TMR4_2_UNIT, TMR4_SEVT_PORT_OUTPUT_EVENT0_SIGNAL);
    
    /* TMR4 PWM: enable main output  */
    TMR4_PWM_MasterOutputCmd(TMR4_2_UNIT, Enable);//MOE使能输出
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
    stcTmr4OcInit.u16CompareVal = 0x0000;
	stcTmr4OcInit.u16OccrLinkTransfer = TMR4_OCO_OCCR_LINK_TRANSFER_ENABLE;
	stcTmr4OcInit.u16OccrBufMode = TMR4_OCO_OCCR_BUF_CNT_PEAK;
	
    /* TMR4 OC channel: initialize channel */
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
    unTmr4OcOcmrh.OCMRx_f.OPZRH  = TMR4_OCO_OP_HOLD;    /* Bit[11:10] 11 */
    unTmr4OcOcmrh.OCMRx_f.OPNPKH = TMR4_OCO_OP_HIGH;    /* Bit[13:12] 00 */
    unTmr4OcOcmrh.OCMRx_f.OPNZRH = TMR4_OCO_OP_HIGH;    /* Bit[15:14] 00 */
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
    unTmr4OcOcmrl.OCMRx_f.OPZRL   = TMR4_OCO_OP_HOLD;    /* bit[11:10] 11 */
    unTmr4OcOcmrl.OCMRx_f.OPNPKL  = TMR4_OCO_OP_HIGH;    /* bit[13:12] 00 */
    unTmr4OcOcmrl.OCMRx_f.OPNZRL  = TMR4_OCO_OP_HOLD;    /* bit[15:14] 00 */
    unTmr4OcOcmrl.OCMRx_f.EOPNDCL = TMR4_OCO_OP_HOLD;    /* bit[17:16] 00 */
    unTmr4OcOcmrl.OCMRx_f.EOPNUCL = TMR4_OCO_OP_HOLD;    /* bit[19:18] 00 */
    unTmr4OcOcmrl.OCMRx_f.EOPDCL  = TMR4_OCO_OP_HOLD;    /* bit[21:20] 11 */
    unTmr4OcOcmrl.OCMRx_f.EOPPKL  = TMR4_OCO_OP_HIGH;    /* bit[23:22] 11 */
    unTmr4OcOcmrl.OCMRx_f.EOPUCL  = TMR4_OCO_OP_LOW;    /* bit[25:24] 11 */
    unTmr4OcOcmrl.OCMRx_f.EOPZRL  = TMR4_OCO_OP_HOLD;    /* bit[27:26] 11 */
    unTmr4OcOcmrl.OCMRx_f.EOPNPKL = TMR4_OCO_OP_HOLD;    /* bit[29:28] 00 */
    unTmr4OcOcmrl.OCMRx_f.EOPNZRL = TMR4_OCO_OP_HOLD;    /* bit[31:30] 00 */
    TMR4_OCO_SetLowChCompareMode(TMR4_3_UNIT, TIM4_3_UL_W2_W2_CH, &unTmr4OcOcmrl);
	TMR4_OCO_SetLowChCompareMode(TMR4_3_UNIT, TIM4_3_VL_W1_W1_CH, &unTmr4OcOcmrl);
	
	/* TMR4 OC: enable */
    TMR4_OCO_SetOutputCompare(TMR4_3_UNIT, TIM4_3_UH_C2_C2_CH, TMR4_OCO_ENABLE);
    TMR4_OCO_SetOutputCompare(TMR4_3_UNIT, TIM4_3_UL_W2_W2_CH, TMR4_OCO_ENABLE);
    TMR4_OCO_SetOutputCompare(TMR4_3_UNIT, TIM4_3_VH_C1_C1_CH, TMR4_OCO_ENABLE);
    TMR4_OCO_SetOutputCompare(TMR4_3_UNIT, TIM4_3_VL_W1_W1_CH, TMR4_OCO_ENABLE);
	
	/************************* Configure TMR4 PWM && TMR4 EVT*****************************/

    /* TMR4 PWM: initialize */
    (void)TMR4_PWM_StructInit(&stcTmr4PwmInit);
    stcTmr4PwmInit.u16Mode = TMR4_PWM_THROUGH_MODE;//直通模式
	stcTmr4PwmInit.u32OxHPortOutMode = TMR4_PWM_PORT_OUTPUT_NORMAL;
	stcTmr4PwmInit.u32OxLPortOutMode = TMR4_PWM_PORT_OUTPUT_NORMAL;//OExy使能输出
    (void)TMR4_PWM_Init(TMR4_3_UNIT, TIM4_3_UH_C2_C2_CH, &stcTmr4PwmInit);
    (void)TMR4_PWM_Init(TMR4_3_UNIT, TIM4_3_UL_W2_W2_CH, &stcTmr4PwmInit);
    (void)TMR4_PWM_Init(TMR4_3_UNIT, TIM4_3_VH_C1_C1_CH, &stcTmr4PwmInit);
    (void)TMR4_PWM_Init(TMR4_3_UNIT, TIM4_3_VL_W1_W1_CH, &stcTmr4PwmInit);   
    
    (void)TMR4_SEVT_StructInit(&stcTmr4EventInit);
	stcTmr4EventInit.u16UpMatchCmd = TMR4_SEVT_UP_ENABLE;
	stcTmr4EventInit.u16DownMatchCmd = TMR4_SEVT_DOWN_ENABLE;
	stcTmr4EventInit.u16PeakMatchCmd = TMR4_SEVT_PEAK_ENABLE;
	stcTmr4EventInit.u16ZeroMatchCmd = TMR4_SEVT_ZERO_ENABLE;
    stcTmr4EventInit.u16CompareVal = TIM4_PERIOD_VALUE;
    (void)TMR4_SEVT_Init(TMR4_3_UNIT, TIM4_3_UH_C2_C2_CH, &stcTmr4EventInit);
    /* TMR4 event: set output event signal */
    TMR4_SEVT_SetPortOutputEventSig(TMR4_3_UNIT, TMR4_SEVT_PORT_OUTPUT_EVENT0_SIGNAL);
    
    /* TMR4 PWM: enable main output  */
    TMR4_PWM_MasterOutputCmd(TMR4_3_UNIT, Enable);//MOE使能输出
    /* Start TMR4 count. */
    TMR4_CNT_Start(TMR4_3_UNIT);
}
