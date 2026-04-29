#ifndef BSP_INIT_H
#define BSP_INIT_H
#include <stdint.h>

#define TEST (0U)
void BSP_init(void);
void Check_Contact1_En(float volt);
void Check_Contact2_En(float volt1);
uint8_t MPU6050_Dmp_Init(void);
#endif
