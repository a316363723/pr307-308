#ifndef __PAGE_POOL_H
#define __PAGE_POOL_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include <stdint.h>
#include <stdbool.h>

/*********************
 *      DEFINES
 *********************/
#define PAGE_DSC_INITIALIZER(p_dsc, _pid, _spid, _index)       {(p_dsc)->pid = (_pid); \
                                                                (p_dsc)->spid = (_spid);\
                                                                (p_dsc)->index = (_index);\
                                                                (p_dsc)->user_data = NULL;\
                                                                (p_dsc)->size = 0;}


/**********************
 *      TYPEDEFS
 **********************/
//简要的描述页面
typedef struct _page_dsc {
    uint8_t  pid;  //页面ID
    uint8_t  spid; //子页面ID
    uint8_t  index; //索引
    void*    user_data;
    uint16_t size;
}page_dsc_t;

//描述页面池
typedef struct _page_pool {
    page_dsc_t* dsc;
    uint8_t   depth;
    uint8_t   top;
}page_pool_t;

/**
 * @brief 读取页面池的栈顶
 * 
 * @param p_pool 
 * @return uint8_t 
 */
static uint8_t page_pool_get_top(page_pool_t* p_pool)
{
    return p_pool->top;
}

/**
 * @brief 设置页面池栈顶
 * 
 * @param p_pool 
 * @param top 
 */
static void page_pool_set_top(page_pool_t* p_pool, uint8_t top)
{
    p_pool->top = top;
}

void page_dsc_init(page_dsc_t* p_dsc, uint8_t pid, uint8_t spid, uint8_t index);

void page_pool_init(page_pool_t* p_pool, page_dsc_t* p_page_dsc, uint8_t depth);

bool page_pool_is_full(page_pool_t* p_pool);

bool page_pool_is_empty(page_pool_t* p_pool);

void page_pool_clear(page_pool_t* p_pool);

bool page_pool_pop(page_pool_t* p_pool, page_dsc_t* p_page_dsc);

bool page_pool_push(page_pool_t* p_pool, page_dsc_t* p_page_dsc);

page_dsc_t* page_pool_peek(page_pool_t* p_pool, uint8_t index);

void page_dsc_print(page_dsc_t* p_dsc);

void page_pool_print(page_pool_t* p_pool);
#ifdef __cplusplus
}
#endif

#endif
