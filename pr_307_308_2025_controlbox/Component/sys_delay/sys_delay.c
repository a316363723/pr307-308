#include "sys_delay.h"
#include "math.h"
#define USE_HAL_DELAY   (1)



#if defined(__CC_ARM)
    #pragma O0
#elif defined(__ICCARM__)
    #pragma optimize=none
#elif defined(__GNUC__)
    #pragma GCC optimize ("O0")
#endif


extern uint32_t SystemCoreClock;



void sys_delay_us(uint32_t us)
{

//    uint32_t sysDelaytime = us * (SystemCoreClock / 1000000U) / 8;
//    do 
//    {
//        __nop();
//    } 
//    while (sysDelaytime--);  
	
	while(us > 0)
	{
		__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();
		__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();
		__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();
		__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();
		__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();
		__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();
		__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();
		__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();
		__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();
		__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();
		__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();
		__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();
		__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();
		__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();
		__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();
		__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();
		__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();
		__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();
		__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();
		__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();
		__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();
		__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();
		__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();
		--us;
	}

}
 
void sys_delay_ms(uint32_t ms)
{ 
//	 osDelay(ms);
#if (USE_HAL_DELAY == 1)
  
#else
    volatile uint32_t Delay = ms * (SystemCoreClock / 1000U / 8U);
    do 
    {
        __nop();
    } 
    while (Delay --);   
#endif
}

 /*****************************************************************************************
* Function Name: delay_ms
* Description  : 毫秒延时函数
* Arguments    : /nms:要延时的ms数
* Return Value : NONE
******************************************************************************************/	
//void delay_ms(uint32_t ms)
//{	
//	uint32_t i;
//	for(i=0;i<ms;i++) sys_delay_us(1000);
//}
