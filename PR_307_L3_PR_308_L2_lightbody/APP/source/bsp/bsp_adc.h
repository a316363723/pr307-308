#ifndef BSP_ADC_H
#define BSP_ADC_H
#include <stdint.h>

#include "define.h"
/*****************************************************************************************
								Macro definitions
*****************************************************************************************/

/*****************************************************************************************
								Typedef definitions
*****************************************************************************************/
/*
PA4	Check_48V	电压检测ADC12_IN4
PA5	Check_R+	电压检测/ADC12_IN5
PA6	Check_G+	电压检测/ADC12_IN6
PA7	Check_B+	电压检测/ADC12_IN7
PB0	Check_CW1+	调光板电压检测/ADC12_IN8
PB1	Check_CW2+	调光板电压检测/ADC12_IN9
PC4	Check_WW1+	调光板电压检测ADC12_IN14
PC5	Check_WW2+	调光板电压检测ADC12_IN15

PC0	Self_WW1ADC	自适应电压检测ADC123_IN10
PC1	Self_WW2ADC	自适应电压检测ADC123_IN11
PC2	self_CW1ADC	自适应电压检测ADC123_IN12
PC3	self_CW2ADC	自适应电压检测ADC123_IN13


PF6	MCU_NTC	     MCU温度检测ADC3_IN4	GPIO_ADC
PF7	Check_YU+	调光板电压检测ADC3_IN5	GPIO_ADC
PF8	Self_RADC	自适应电压检测ADC3_IN6	GPIO_ADC
PF9	Self_GADC	自适应电压检测ADC3_IN7	GPIO_ADC
PF10Self_BADC	自适应电压检测ADC3_IN8	GPIO_ADC
PF3	COB_NTC4	ADC3_IN9温度检测	GPIO_ADC
PF4	COB_NTC3	ADC3_IN14温度检测	GPIO_ADC
PH2	Self_YUADC	ADC3_IN16	GPIO_ADC
PH4	Check_Contact1	开路3V，短路1.57V，触点接触2.3V ADC3_IN18	GPIO_ADC
PH5	Check_Contact2	开路3V，短路1.57V，触点接触2.3V ADC3_IN19	GPIO_ADC
*/

#define ADC_CH_Check_48V		(ADC_CH4 )
#define ADC_CH_Check_R         	(ADC_CH5 )
#define ADC_CH_Check_G         	(ADC_CH6 )
#define ADC_CH_Check_B         	(ADC_CH7 )
#define ADC_CH_Check_CW1       	(ADC_CH8 )
#define ADC_CH_Check_CW2       	(ADC_CH9 )
#define ADC_CH_Check_WW1       	(ADC_CH14)
#define ADC_CH_Check_WW2       	(ADC_CH15)
#define ADC_CH_Self_WW1ADC      (ADC_CH10)
#define ADC_CH_Self_WW2ADC      (ADC_CH11)
#define ADC_CH_self_CW1ADC      (ADC_CH12)
#define ADC_CH_self_CW2ADC      (ADC_CH13)

#define ADC_CH_MCU_NTC			(ADC_CH4)	  
#define ADC_CH_Check_YU        	(ADC_CH5)
#define ADC_CH_Self_RADC        (ADC_CH6)
#define ADC_CH_Self_GADC        (ADC_CH7)
#define ADC_CH_Self_BADC        (ADC_CH8)
#define ADC_CH_COB_NTC4	        (ADC_CH9)
#define ADC_CH_COB_NTC3	        (ADC_CH14)
#define ADC_CH_Self_YUADC       (ADC_CH16)
#define ADC_CH_Check_Contact1	(ADC_CH18)
#define ADC_CH_Check_Contact2	(ADC_CH19)

//多路ADC在DMA通道中的顺序，和最大通道数
typedef enum
{
	ADC_DMA_RANK0_ADDR = 0, //Check_48V  
	ADC_DMA_RANK1_ADDR,		//Check_R+
	ADC_DMA_RANK2_ADDR,		//Check_G+
	ADC_DMA_RANK3_ADDR,		//Check_B+
	ADC_DMA_RANK4_ADDR,		//Check_CW1+    //Check_WW1+
	ADC_DMA_RANK5_ADDR,		//Check_CW2+    //Check_WW2+
	ADC_DMA_RANK6_ADDR,		//Check_WW1+    //Check_CW1+
	ADC_DMA_RANK7_ADDR,		//Check_WW2+    //Check_CW2+
	ADC_DMA_RANK8_ADDR,		//Self_WW1ADC	//Self_WW2ADC
	ADC_DMA_RANK9_ADDR,		//Self_WW2ADC   //Self_WW1ADC
	                       
	ADC_DMA_RANK10_ADDR,	//self_CW1ADC   //self_CW2ADC
	ADC_DMA_RANK11_ADDR, 	//self_CW2ADC   //self_CW1ADC
	
	ADC_DMA_RANK12_ADDR, 	//MCU_NTC	                        
	ADC_DMA_RANK13_ADDR, 	//Check_YU+
	ADC_DMA_RANK14_ADDR, 	//Self_RADC   //Self_YUADC
	ADC_DMA_RANK15_ADDR, 	//Self_GADC
	ADC_DMA_RANK16_ADDR, 	//Self_BADC
	ADC_DMA_RANK17_ADDR, 	//COB_NTC4
	ADC_DMA_RANK18_ADDR, 	//COB_NTC3
	ADC_DMA_RANK19_ADDR, 	//Self_YUADC  //Self_RADC
	ADC_DMA_RANK20_ADDR, 	//Check_Contact1
	ADC_DMA_RANK21_ADDR, 	//Check_Contact2
	
 	ADC_CHANNEL_NUMBER,		
}Adc_Dma_Data_Dist_TypeDef;

extern void (*Adc1_Dma_CallBack)(uint16_t *pBuff);
extern void (*Adc2_Dma_CallBack)(uint16_t *pBuff);


void Adc_init(void);
void ADC_Enable(void);
void ADC_Disable(void);
void ADC2_Enable(void);
void ADC2_Disable(void);

uint16_t Get_ADC3_Value(uint8_t Channel);
uint16_t Get_ADC2_Value(uint8_t Channel);
#endif
