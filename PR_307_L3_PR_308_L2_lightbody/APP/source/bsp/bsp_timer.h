#ifndef BSP_TIMER_H
#define BSP_TIMER_H


#include "hc32_ddl.h"

//#define LIGHT_FRQ_DEFAULT   20.0f
//#define LIGHT_FRQ_MIN 		20.0f
//#define LIGHT_FRQ_MAX 		22.1f

//#define LIGHT_TIMA_7_FRQ_MAX				120000UL					//	定时器的最大频率


void Tmr2Start(void);
void Time_Init(void);
void Channel_Output_Control(uint8_t channel, en_functional_state_t state);
extern void (*Light_Effect_CallBack)(void);
extern void (*Sidus_Effect_CallBack)(void);
#if NULL
extern uint32_t RNG_Get_RandomRange(uint32_t min, uint32_t max);
#endif

#endif  /* _BSP_TIMER_H_ */
