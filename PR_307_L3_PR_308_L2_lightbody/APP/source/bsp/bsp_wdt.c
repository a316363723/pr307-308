#include "bsp_wdt.h"
#include "hc32_ddl.h"
#include "hc32_common.h"
#include "ddl_config.h"

//喂狗函数 ：void WDT_Feed(void)
/**
 * @brief  WDT configuration.
 * @param  None
 * @retval None
 */
void WDT_Config(void)
{
    stc_wdt_init_t stcWdtInit;

    /* WDT configure   看门狗超时时间 = 65536 * 8192 /60000000  = 8.9s */
    stcWdtInit.u32CountCycle    = WDT_COUNTER_CYCLE_65536;  
    stcWdtInit.u32ClockDivision = WDT_CLOCK_DIV8192;  
    stcWdtInit.u32RefreshRange  = WDT_RANGE_0TO100PCT;
    stcWdtInit.u32LPModeCountEn = WDT_LPM_COUNT_STOP;
    stcWdtInit.u32TrigType      = WDT_TRIG_EVENT_RESET;
    (void)WDT_Init(&stcWdtInit);
}
