#ifndef __ADC_H
#define __ADC_H	

#include "stdint.h"
#include "hc32_ddl.h"

enum hal_adc
{
	HAL_ADC_1,
	HAL_ADC_2,
    HAL_ADC_3,
	HAL_ADC_NUM,
};

enum hal_adc_irq
{
	HAL_ADC_IRQ_BTC,

};

void adc_capture_start_it(enum hal_adc unit );
void adc_capture_stop_it(enum hal_adc unit );
void hal_adc_init(enum hal_adc unit);
void hal_adc_irq_register(enum hal_adc unit,enum hal_adc_irq irq_type, void (*irq_cb)(void));
void hal_adc_receive_dma_start(enum hal_adc unit,const uint32_t *p_data, uint16_t size);



#endif


