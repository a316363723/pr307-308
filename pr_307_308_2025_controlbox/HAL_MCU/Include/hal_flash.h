#ifndef HAL_FLASH_H
#define HAL_FLASH_H
#include <stdint.h>


void system_reset (void);
void hal_flash_erase(uint32_t addr, uint32_t size);
void hal_flash_write(uint32_t addr, uint32_t size, uint8_t* data);

#endif

