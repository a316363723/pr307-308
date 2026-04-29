#ifndef DELAY_H
#define DELAY_H
#include <stdint.h>

void sys_delay_us(uint32_t us);
void sys_delay_ms(uint32_t ms);
extern void delay_us(uint32_t nus);
extern void delay_ms(uint32_t nms);
#endif

