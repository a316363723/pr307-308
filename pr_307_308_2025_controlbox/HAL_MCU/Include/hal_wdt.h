#ifndef HAL_WDT_H
#define HAL_WDT_H
#include <stdint.h>
#include <stdbool.h>

void hal_wdt_init(void);
void hal_wdt_rest(void);
uint16_t hal_get_wdt_cnt(void);

#endif

