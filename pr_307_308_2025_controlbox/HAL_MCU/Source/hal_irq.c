/**
 * @file hal_irq.c
 * @author Darwin (darwin.xiao@aputure.com)
 * @brief 
 * @version 0.1
 * @date 2022-09-09
 * 
 * @copyright Copyright (c) 2022 爱图仕
 * 
 * @par 修改日志:
 * <table>
 * <tr><th> Date          <th>Version <th>Author  <th>Description
 * <tr><td>2022-09-09     <td>1.0     <td>Darwin   <td>初始化创建
 * </table>
 */
#include "hc32_ddl.h"
#include "hal_irq.h"


/**
 * @brief 中断使能控制
 * 
 * @param[in]irqn           中断号
 * @param[in]enabled        使能标志
 */
void hal_irq_set_enabled(uint32_t irqn, bool enabled)
{
    if (enabled) 
    {
        NVIC_EnableIRQ((IRQn_Type)irqn);
    }
    else 
    {
        NVIC_DisableIRQ((IRQn_Type)irqn);
    }    
}

