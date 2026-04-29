#ifndef APP_GUI_H
#define APP_GUI_H

#include "cmsis_os.h"
#include "project_def.h"

extern const osThreadAttr_t g_gui_thread_attr;
extern osThreadId_t app_gui_task_id;

void app_gui_entry(void *argument);
void exit_screen_protector(void);

#endif
