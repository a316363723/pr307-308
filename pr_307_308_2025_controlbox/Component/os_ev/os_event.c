#include "os_event.h"
#include "cmsis_os2.h"
#include <string.h>
#include <stdlib.h>


struct os_event_core ev_core;

/**
 * @brief 计算前导零
 * 
 * @param[in]value          值
 * @return uint8_t 
 */
static inline uint8_t count_leading_zero(uint32_t value)
{
    return __clz(value);
}

/**
 * @brief 获取元素1的位置
 * 
 * @param[in]value          值
 * @return uint8_t 
 */
uint8_t get_first_bit_pos(uint32_t value)
{    
    assert(value == 0);
    return (31 - count_leading_zero(value));
}


/**
 * @brief 订阅事件
 * 
 * @param[in]event          事件类型
 * @param[in]event_filter   事件过滤值
 * @param[in]call_back      回调函数
 * @return int 0 成功订阅  -1 订阅失败
 */
int os_ev_subscribe_event(uint32_t event_type, uint32_t event_filter, void(*call_back)(uint32_t event_type, uint32_t event_value))
{
	if (event_type == 0)
		return -1;
	
    uint8_t event_idx = get_first_bit_pos(event_type);
    struct elist_item* item = (struct elist_item*)malloc(sizeof(struct elist_item));

    osKernelLock();
    memset(item, 0, sizeof(struct elist_item));
    item->event_handle = osEventFlagsNew(NULL);
    item->event_filter = event_filter;
    item->call_back = call_back;
    item->task_id = (uint32_t)osThreadGetId();  //获取自己线程的ID句柄

    if (ev_core.event_map[event_idx].head == NULL)
    {
        ev_core.event_map[event_idx].head = item;
    }
    else  
    {
        struct elist_item** temp = &ev_core.event_map[event_idx].head;
        while ((*temp)->next != NULL)
        {
            temp = &((*temp)->next);
        }
        (*temp)->next = item;
    }
	
    osKernelUnlock();
    return 0;
}

/**
 * @brief 发布事件
 * 
 * @param[in]event          事件大类
 * @param[in]event_value    事件值
 */
void  os_ev_publish_event(uint32_t event_type, uint32_t event_value, uint32_t data)
{
	if ((event_type == 0) || (event_value == 0))
		return ;
	
    uint8_t event_idx = get_first_bit_pos(event_type);
    uint8_t event_vidx = get_first_bit_pos(event_value);

    struct elist_item* q = ev_core.event_map[event_idx].head;
    ev_core.event_map[event_idx].event_data[event_vidx] = data;
    for (; q != NULL; q = q->next)
    {
        /* 当前任务进程不给自己发送事件. */
        if (q->task_id == (uint32_t)osThreadGetId())
            continue;

        if (q->event_filter & event_value)
        {            
            osEventFlagsSet(q->event_handle, event_value);        
        }        
    }     
}


/**
 * @brief 拉取事件
 * 
 * @param[in]event          事件类型
 */
void os_ev_pull_event(uint32_t event_type)
{
    uint32_t task_id = (uint32_t)osThreadGetId();    
    uint8_t event_idx = 0;
    int event_value = 0;
    struct elist_item* q = NULL;        
     int32_t sl;
    while (event_type > 0)
    {
        sl = osKernelLock();
        event_idx = get_first_bit_pos(event_type);
        
        q = ev_core.event_map[event_idx].head;
        osKernelRestoreLock(sl);
        for (; q != NULL; q = q->next) 
        {
            if (q->task_id == task_id)       
            {
                event_value = osEventFlagsGet(q->event_handle);
                if ((event_value > 0) && (q->call_back != NULL))
                {
                    osEventFlagsClear(q->event_handle, event_value);
                    q->call_back(1 << event_idx, event_value);
                }                                
            }
        }

        event_type &= ~(1 << event_idx);
    }
}

/**
 * @brief 获取事件数据
 * 
 * @param[in]event_type      事件类型
 * @param[in]event_value     事件值
 * @return int 事件数据
 */
int  os_ev_get_event_data(uint32_t event_type, uint32_t event_value)
{	
    uint8_t event_idx = get_first_bit_pos(event_type);
    uint8_t event_vidx = get_first_bit_pos(event_value);

    return ev_core.event_map[event_idx].event_data[event_vidx];
}
