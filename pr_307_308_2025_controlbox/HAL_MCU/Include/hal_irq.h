/**
 * @file hal_irq.h
 * @author Darwin (darwin.xiao@aputure.com)
 * @brief 
 * @version 0.1
 * @date 2022-09-09
 * 
 * @copyright Copyright (c) 2022 爱图仕
 * 
 * @par 修改日志:
 * <table>
 * <tr><th> Date      <th>Version <th>Author  <th>Description
 * <tr><td>2022-09-09     <td>1.0     <td>Darwin   <td>内容
 * </table>
 */
#ifndef HAL_IRQ_H
#define HAL_IRQ_H

#include "irq.h"
#include <stdint.h>
#include <stdbool.h>


void hal_irq_set_enabled(uint32_t irqn, bool enabled);

#endif // !HAL_IRQ_H
