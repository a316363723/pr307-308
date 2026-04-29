/*******************************************************************************
 * Include files
 ******************************************************************************/
#include "hc32_ddl.h"
#include "security_user_implement.h"
#include "bsp_init.h"
#include "bsp_wdt.h"
#include "bsp_debug.h"
#include "task.h"
#include "app_task.h"
#include "define.h"

/**
 * @brief  Main function of template project
 * @param  None
 * @retval int32_t return value, if needed
 */ 
int32_t main(void)
{
	SCB->VTOR = (uint32_t)(0x1A000 & SCB_VTOR_TBLOFF_Msk);//ÖÐ¶ÏÆ«ÒÆ
	__enable_irq();
	Security_Init(); 
	BSP_init();	
	NVIC_SetPriorityGrouping(0x03);
	(void)SysTick_Init(1000U);
	Task_Build();
	
	for(;;)
	{
		#if (1 == IWDG_EN)
		WDT_Feed();
		#endif
        TaskProcess();
	}
}

/*******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/
