/*********************
 *      INCLUDES
 *********************/
#include "page.h"
#include "page_pool.h"
#include "ui_config.h"
#include <string.h>

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *  STATIC VARIABLES
 **********************/

/*********************
 *  GLOBAL VATIABLES
 *********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/
/**
 * @brief 初始化页面池
 * 
 * @param p_pool 
 * @param p_page_dsc 
 * @param depth 
 */
void page_pool_init(page_pool_t* p_pool, page_dsc_t* p_page_dsc, uint8_t depth)
{
    p_pool->dsc = &p_page_dsc[0]; 
    p_pool->depth = depth;
    p_pool->top = 0;
    
    page_pool_clear(p_pool);
}

/**
 * @brief 判断页面池是否满
 * 
 * @param p_pool 
 * @return true 
 * @return false 
 */
bool page_pool_is_full(page_pool_t* p_pool)
{
    if (p_pool->depth == 0 || (p_pool->top == p_pool->depth))
        return true;

    return false;
}

/**
 * @brief 判断页面池是否是空
 * 
 * @param p_pool 
 * @return true 
 * @return false 
 */
bool page_pool_is_empty(page_pool_t* p_pool)
{
    if (p_pool->depth == 0 || p_pool->top == 0)
        return true;

    return false;
}

/**
 * @brief 清理页面池
 * 
 * @param p_pool 
 */
void page_pool_clear(page_pool_t* p_pool)
{
    p_pool->top = 0;
    memset(p_pool->dsc, 0, p_pool->depth * sizeof(page_dsc_t));
}


/**
 * @brief 页面出栈
 * 
 * @param p_pool 
 * @param p_page_dsc 
 * @return true 
 * @return false 
 */
bool page_pool_pop(page_pool_t* p_pool, page_dsc_t* p_page_dsc)
{
    if (p_pool->top > 0)
        p_pool->top--;
    else
        return false;

    *p_page_dsc = p_pool->dsc[p_pool->top];    

    return true; 
}

/**
 * @brief 页面入栈
 * 
 * @param p_pool 
 * @param p_page_dsc 
 * @return true 
 * @return false 
 */
bool page_pool_push(page_pool_t* p_pool, page_dsc_t* p_page_dsc)
{
    if (page_pool_is_full(p_pool))
        return false;

    p_pool->dsc[p_pool->top++] = *p_page_dsc;
    return true;
}

/**
 * @brief 检查页面池里面的元素
 * 
 * @param p_pool 
 * @param index 
 * @return page_dsc_t* 
 */
page_dsc_t* page_pool_peek(page_pool_t* p_pool, uint8_t index)
{
    if (index >= p_pool->top)
        return NULL;

    return &p_pool->dsc[index];
}

/**
 * @brief 打印页面描述
 * 
 * @param p_dsc 
 */
void page_dsc_print(page_dsc_t* p_dsc)
{
    UI_PRINTF("PID: %d SPID: %d INDEX: %d \r\n", p_dsc->pid, p_dsc->spid, p_dsc->index);    
}

/**
 * @brief 打印页面池
 * 
 * @param p_pool 
 */
void page_pool_print(page_pool_t* p_pool)
{
    page_dsc_t* p_page_dsc;

    UI_PRINTF("Pool Top: %d\r\n", p_pool->top);
    for (uint8_t cnt = 0; cnt < p_pool->top; cnt++) {

        p_page_dsc = page_pool_peek(p_pool, cnt);
        if (p_page_dsc != NULL) {
            page_dsc_print(p_page_dsc);
        }
    }    
}
