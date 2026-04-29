#ifndef APP_BLE_H
#define APP_BLE_H

#include "cmsis_os.h"
#include "project_def.h"







extern const osThreadAttr_t g_ble_thread_attr;
extern osThreadId_t app_ble_task_id;

void app_ble_thread_entrance(void *argument);
uint32_t get_ble_reset_status(void);




#endif /* APP_BLE_H */


