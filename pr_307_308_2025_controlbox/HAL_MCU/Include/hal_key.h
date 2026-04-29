#ifndef _KEY_H
#define _KEY_H

#include "hc32_ddl.h"
#include "stdbool.h"


#define POWER_KEY_Port    GPIO_PORT_I
#define POWER_KEY_pin     GPIO_PIN_10

#define  KEY_POWER    ((GPIO_ReadInputPins(POWER_KEY_Port,POWER_KEY_pin) == (0)) ? 1 : 0)   //   暂时默认是电源按键

#define KET_VALUE	  KEY_POWER

//----------------------------------------------------------------------------------------
////键值定义，KeyId
#define KEY_NONE                		0x0000 //无键按下
#define KEY_ON_OFF 					    0x0001 //对应实体IO_KEY0
#define KEY1_1					        0x0002 //对应实体IO_KEY1  //返回按键
#define KEY2_2					        0x0004 //对应实体IO_KEY2  //
#define KEY3_3						    0x0008 //对应实体IO_KEY3
#define KEY4_4						    0x0010 //对应实体IO_KEY4
#define KEY5_5						    0  //暂时没用到
#define KEY6_6						    0  //暂时没用到
#define KEY_EC1_1						0x0020   //确定按键值
#define KEY_EC2_2						0x0100
#define KEY_EC3_3						0x0200
#define KEY1_KEY3_SET					0x000A //组合按键，同时按KEY1和KEY3
#define KEY0_KEY2_ADMIN					0x0005 //组合按键，同时按KEY0和KEY2
#define KEY0_KEY1_KEY2_TEST				0x0007 //组合按键，同时按KEY0,KEY1和KEY2
//----------------------------------------------------------------------------------------
//按键时间定义，如短按，长按，重复键
#define KEY_PRESSED_TMR               	(uint32_t)3  // 50   ms
#define KEY_DOWM_TMR                	(uint32_t)10  // 1000 ms 
#define KEY_UP_TMR                		(uint32_t)6	 // 60  ms
#define KEY_REPEAT_TMR                  (uint32_t)10	 // 100  ms
#define KEY_LONG_TMR                	(uint32_t)100  // 2000 ms 
/*****************************************************************************************
								Typedef definitions
*****************************************************************************************/
//按键状态定义
typedef enum
{
    KEY_STATE_INIT = 0,
    KEY_STATE_WOBBLE,
    KEY_STATE_DWON,
    KEY_STATE_WAITUP,
} TE_KEY_STATE;

//按键事件定义
typedef enum
{
    KEY_NULL = 0x00,
    KEY_PRESSED = 0x01,
    KEY_DOWN = 0x02,
    KEY_UP = 0x04,
    KEY_REPEAT = 0x08,
    KEY_LONG = 0x10
} te_key_event;

//按键属性结构体
typedef struct
{
    uint8_t keyLock ;
    uint8_t KeyState;                      // 按键状态
    uint16_t KeyDwTmr;                     // 按键按下的时间
    uint32_t KeyCur;                       // 记录发生按键事件时的键值
    te_key_event KeyEvent;				   // 按键事件
    te_key_event KeyEventOsCpy;			   // 按键事件 操作系统备份
} TS_KEY_INFO;
/*****************************************************************************************
							Global variables and functions
*****************************************************************************************/
extern void key_poll(void);
extern uint32_t GetKey(void);
extern void set_key_val(te_key_event key_val);
bool get_Key_event(uint32_t KeyId, te_key_event KeyEvent);
void key_gpio_init(void);
void key_power_enabled(void);
void key_power_unenabled(void);

#endif
/***********************************END OF FILE*******************************************/

