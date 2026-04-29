#ifndef __LCD_SMC_H
#define __LCD_SMC_H

#ifdef __cplusplus
extern "C" {
#endif

/****************
 *   INCLUDE
 ****************/
#include <stdint.h>
#include <stdbool.h>

/******************
 *     DEFINE
 ******************/
/* Use EXMC CS3, A12 as the RS signal */
#define LCD_BASE                        (0x60000000UL | ((1UL << 13U) - 2UL))
/********************* 
 *     TYPEDEF
**********************/
typedef struct
{
    volatile uint16_t REG;
    volatile uint16_t RAM;
} LCD_Controller_Typedef;
	
/********************* 
 *  GLOBAL PROTOTYPES
**********************/
static LCD_Controller_Typedef *LCD = ((LCD_Controller_Typedef *)LCD_BASE);
bool bsp_smc_init(void);

#ifdef __cplusplus
}
#endif

#endif // !__LCD_SMC_H
