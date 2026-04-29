#ifndef HAL_SPI_H
#define HAL_SPI_H
#include "stdint.h"

enum hal_spi_type
{
	HAL_SPI_CRMX,
	HAL_SPI_NUM,
};

void hal_spi_init(enum hal_spi_type unit);
uint8_t hal_spi_send_recv_byte(enum hal_spi_type unit,uint8_t data);
void set_spi_cs_pin(enum hal_spi_type unit, uint8_t state);
void hal_spi_pin_reset(enum hal_spi_type unit);
void hal_spi_pin_enable(enum hal_spi_type unit);

#endif

