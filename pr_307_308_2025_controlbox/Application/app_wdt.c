#include "app_wdt.h"
#include "hc32_ddl.h"
#include "cmsis_os.h"
#include "hal_wdt.h"

const osThreadAttr_t g_app_wdt_thread_attr =
{
    .name = "app wdt",
    .priority = osPriorityLow,
    .stack_size = 128 * 4
};

void app_wdt_dtection(void *argument)
{
	while(1)
    {
		if(hal_get_wdt_cnt() < 65535/2)
		{
			hal_wdt_rest();
		}
		osDelay(1000);
	}

}

void app_wdt_reset_set(void)
{
	if(hal_get_wdt_cnt() < 65535/2)
	{
		hal_wdt_rest();
		osDelay(100);
	}
}
