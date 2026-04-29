#ifndef BSP_DEBUG_H
#define BSP_DEBUG_H
#include <stdint.h>

/* Debug printing definition. */
#if (DDL_PRINT_ENABLE == DDL_ON)
#define DBG         (void)printf
#else
#define DBG(...)
#endif

void Debug_Usart_Init(void);

#endif
