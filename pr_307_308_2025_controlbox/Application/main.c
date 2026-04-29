#include "hc32_ddl.h"
#include "cmsis_os.h"
#include "board.h"
#include "app_dmx.h"
#include "app_gui.h"
#include "perf_counter.h"
#include "security_user_implement.h"
#include "local_data.h"
#include "app_usb.h"
#include "usb_bsp.h"
#include "app_data_center.h"
#include "app_ble.h"
#include "app_light.h"
#include "app_power.h"
#include "app_eth.h"
#include "app_err_detect.h"
#include "app_wdt.h"
#include "hal_wdt.h"

const osThreadAttr_t g_init_thread_attr =
{
    .name = "app init",
    .priority = osPriorityHigh,
    .stack_size = 256 * 4
};
osSemaphoreId_t thread_sync_sigl;

void app_init_thread_entry(void *argument)
{
    init_cycle_counter(true);

    thread_sync_sigl = osSemaphoreNew (1, 0, NULL);
    osSemaphoreAcquire(thread_sync_sigl, 0);   
	
	app_error_code_id = osThreadNew(app_error_code_dtection, NULL, &g_error_code_cb_thread_attr);  
	osSemaphoreAcquire(thread_sync_sigl, osWaitForever);
	
    app_data_center_task_id = osThreadNew(app_data_center_entry, NULL, &g_data_center_thread_attr);
    osSemaphoreAcquire(thread_sync_sigl, osWaitForever);
	
	app_gui_task_id = osThreadNew(app_gui_entry, NULL, &g_gui_thread_attr);
    osSemaphoreAcquire(thread_sync_sigl, osWaitForever);
	
	app_power_thread_task_id = osThreadNew(app_power_thread_entrance, NULL, &g_power_thread_attr);
	osSemaphoreAcquire(thread_sync_sigl, osWaitForever);

    app_light_task_id = osThreadNew(app_light_thread_entrance, NULL, &g_light_thread_attr);
    osSemaphoreAcquire(thread_sync_sigl, osWaitForever);
     
    app_usb_task_id = osThreadNew(app_usb_entry, NULL, &g_usb_cb_thread_attr);
    app_ble_task_id = osThreadNew(app_ble_thread_entrance, NULL, &g_ble_thread_attr);
    app_eth_task_id = osThreadNew(app_eth_entry, NULL, &g_eth_thread_attr);
	app_dmx_thread_task_id = osThreadNew(app_dmx_thread_entrance, NULL, &g_dmx_thread_attr);  
//	osThreadNew(app_wdt_dtection, NULL, &g_app_wdt_thread_attr);  
	
    osThreadExit();      
}

int main(void)
{
    SCB->VTOR = (uint32_t)(0x18000 & SCB_VTOR_TBLOFF_Msk);
    board_init();
    Security_Init();
    power_init();
//	hal_wdt_init();
//	hal_wdt_rest(); 
	usbfs_init();
    osKernelInitialize();
    
    osThreadNew(app_init_thread_entry, NULL, &g_init_thread_attr);

    osKernelStart();

    while (1)
    {
		__nop();
    }
}
