#include "os_event.h"
#include "cmsis_os2.h"
#include <string.h>
#include <stdlib.h>

osThreadId_t gui_thread;
const osThreadAttr_t gui_thread_attributes = {
  .name = "gui_thread",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};

osThreadId_t bt_thread;
const osThreadAttr_t bt_thread_attributes = {
  .name = "bt_thread",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};

osThreadId_t rs485_thread;
const osThreadAttr_t rs485_thread_attributes = {
  .name = "rs485_thread",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};

static void bt_thread_entry(void* arg);
static void gui_thread_entry(void* arg);
static void rs485_thread_entry(void* arg);
static void rs485_data_changed_call_back(uint32_t event_type, uint32_t event_value, uint32_t data);
static void gui_data_changed_call_back(uint32_t event_type, uint32_t event_value, uint32_t data);


uint32_t gui_thread_cnt = 0;
uint32_t rs485_thread_cnt = 0;


void test_demo(void)
{	
	osThreadNew(bt_thread_entry, NULL,   &bt_thread_attributes);
	osThreadNew(gui_thread_entry, NULL,  &gui_thread_attributes);	
	osThreadNew(rs485_thread_entry, NULL, &rs485_thread_attributes);
}

/**
 * @brief GUI 接收发布的数据.
 * 
 * @param[in]arg            My Param doc
 */
static void gui_thread_entry(void* arg)
{

	os_ev_subscribe_event(OS_EVENT_MSG_DATA_CHANGED, 1, gui_data_changed_call_back);
	for(;;)
	{
		os_ev_pull_event(OS_EVENT_MSG_DATA_CHANGED);
		osDelay(10); 
	}
}

/**
 * @brief 485任务接收发布的数据
 * 
 * @param[in]arg            My Param doc
 */
static void rs485_thread_entry(void* arg)
{

	os_ev_subscribe_event(OS_EVENT_MSG_DATA_CHANGED, 1, rs485_data_changed_call_back);
	for(;;)
	{
        os_ev_pull_event(OS_EVENT_MSG_DATA_CHANGED);
		osDelay(10); 
	}
}

/**
 * @brief 蓝牙任务发布数据
 * 
 * @param[in]arg            My Param doc
 */
static void bt_thread_entry(void* arg)
{
	for (;;)
	{		
		os_ev_publish_event(OS_EVENT_MSG_DATA_CHANGED, 1, 0);
		osDelay(10);
	}
}

static void rs485_data_changed_call_back(uint32_t event_type, uint32_t event_value, uint32_t data)
{
	rs485_thread_cnt++;
}

static void gui_data_changed_call_back(uint32_t event_type, uint32_t event_value, uint32_t data)
{
	gui_thread_cnt++;
}
