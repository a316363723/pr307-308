#ifndef L_CONFIG_H
#define L_CONFIG_H

#define LFM_DEBUG                       1

#if PROJECT_TYPE==307
#define MACHINE_UUID                    "000K5"     /* 机器ID   */
#endif
#if PROJECT_TYPE==308
#define MACHINE_UUID                    "000J5"     /* 机器ID   */
#endif
#define HW_VERSION                      10          /* 硬件版本 */
#define SF_VERSION                      10          /* 软件版本 */
#define PROTOCOL_VERSION                0           /* 协议版本 */
#define LAMP_TYPE                       9           /* 灯体类型 */

#define RX_MSG_QUEUE_CNT                4           /* 接收消息队列长度 */
#define RX_MSG_QUEUE_ITEM_SIZE          512         /* 接收消息队列项大小 */

#define UPGRADE_MSG_QUEUE_CNT           6           /* 升级消息队列长度 */
#define UPGRADE_MSG_QUEUE_ITEM_SIZE     512         /* 升级消息队列项大小 */

#define TX_TASK_STACK_DEPTH             512         /* 发送任务堆栈深度 */
#define TX_TASK_PRIORITY                3           /* 发送任务优先级 */

#define RX_TASK_STACK_DEPTH             512         /* 接收任务堆栈深度 */
#define RX_TASK_PRIORITY                3           /* 接收任务优先级 */

#define MAX_PACKET_LEN                  512         /* 最大一包的长度 */

#define PERIODIC_TIME_MS                20          /* 定时器回调函数的周期, 单位为ms */

#define COMMUNICATE_TIMEOUT_MS          20000       /* 通讯超时的事时间, 单位为ms */

#endif

