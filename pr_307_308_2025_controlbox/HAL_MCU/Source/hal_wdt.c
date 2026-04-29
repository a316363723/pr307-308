#include "hal_wdt.h"
#include "hc32_ddl.h"


void hal_wdt_init(void)
{
	stc_wdt_init_t stcWdtInit;

    /* WDT configure */
    stcWdtInit.u32CountCycle    = WDT_COUNTER_CYCLE_65536;
    stcWdtInit.u32ClockDivision = WDT_CLOCK_DIV8192;
    stcWdtInit.u32RefreshRange  = WDT_RANGE_25TO100PCT;
    stcWdtInit.u32LPModeCountEn = WDT_LPM_COUNT_STOP;
    stcWdtInit.u32TrigType      = WDT_TRIG_EVENT_RESET;
    (void)WDT_Init(&stcWdtInit);
}

void hal_wdt_rest(void)
{
	WDT_Feed();
}

uint16_t hal_get_wdt_cnt(void)
{
	WDT_GetCountValue();
}
