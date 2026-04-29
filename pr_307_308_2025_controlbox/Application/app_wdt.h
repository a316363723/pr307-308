#ifndef APP_WDT_H
#define APP_WDT_H
#include <stdint.h>
#include <stdbool.h>
#include "cmsis_os.h"

extern const osThreadAttr_t g_app_wdt_thread_attr;

void app_wdt_dtection(void *argument);
void app_wdt_reset_set(void);

#endif

