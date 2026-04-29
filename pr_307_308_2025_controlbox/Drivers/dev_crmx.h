#ifndef DEV_CRMX_H
#define DEV_CRMX_H
#include "stdint.h"
#include "dev_lumenradio_api.h"

extern LumenRadioDevice_t g_lumenradio_device;

void dev_crmx_init(void);
void dev_crmx_power_ctrl(uint8_t state);
uint8_t dev_crmx_disconnect(void);
uint8_t dev_crmx_connect_detect(void);
uint8_t dev_crmx_irq_read(void);
uint8_t dev_crmx_send_read_data(uint8_t data);
void dev_crmx_cs_ctrl(uint8_t state);
void dev_crmx_testing(void);
#endif

