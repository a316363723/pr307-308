#include "delay.h"
#include "hc32_ddl.h"
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
 
/*****************************************************************************************
* Function Name: delay_us
* Description  : 微秒延时函数
* Arguments    : nus:要延时的us数.	
* Return Value : NONE
******************************************************************************************/						   
void delay_us(uint32_t nus)
{		
	sys_delay_us(nus);
}  

 /*****************************************************************************************
* Function Name: delay_ms
* Description  : 毫秒延时函数
* Arguments    : /nms:要延时的ms数
* Return Value : NONE
******************************************************************************************/	
void sys_delay_ms(uint32_t ms)
{	
//	uint32_t i;
//	for(i=0;i<ms;i++) sys_delay_us(1000);
	SysTick_Delay(ms);
}


void delay_ms(uint32_t nms)
{	
   sys_delay_ms(nms);
}
