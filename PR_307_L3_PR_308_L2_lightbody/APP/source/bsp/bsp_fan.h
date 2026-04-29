#ifndef BSP_FAN_H
#define BSP_FAN_H
#include <stdint.h>

#define FAN_TMRA_PWM_A_CH               (TMRA_CH_2)
#define FAN_TMRA_PWM_A_PORT             (GPIO_PORT_B)
#define FAN_TMRA_PWM_A_PIN              (GPIO_PIN_04)
#define FAN_TMRA_PWM_A_PIN_FUNC         (GPIO_FUNC_5_TIMA12_PWM2)

#define FAN_TMRA_MODE                   (TMRA_MODE_SAWTOOTH)
#define FAN_TMRA_DIR                    (TMRA_DIR_UP)
#define FAN_TMRA_PERIOD_VAL             (6000UL - 1UL)
#define FAN_TMRA_PWM_A_CMP_VAL          (2000UL - 1UL)
	
#define FAN_TMRA_CAPT_PWM_PORT          		(GPIO_PORT_B)
#define FAN_TMRA_CAPT_PWM_PIN           		(GPIO_PIN_06)
#define FAN_TMRA_CAPT_PWM_PIN_FUNC      		(GPIO_FUNC_5_TIMA12_PWM4)

#define FAN_TMRA_CAP_UINT                       (M4_TMRA_12)
#define FAN_TMRA_CAP_CH                         (TMRA_CH_4)
#define FAN_TMRA_CAP_PERIP_CLK                  (PWC_FCG2_TMRA_12)
/* The divider of the clock source. @ref TMRA_PCLK_Divider */
#define FAN_TMRA_CLK                        	(TMRA_CLK_PCLK)
#define FAN_TMRA_PCLK_DIV                   	(TMRA_PCLK_DIV32)
#define FAN_CAPTURE_CONDITION               	(TMRA_CAPT_COND_PWMR)//捕获触发源为PWM的上升沿

#define FAN_TMRA_IRQn                       	(Int015_IRQn)
#define FAN_TMRA_OVF_IRQn						(Int016_IRQn)

#define FAN_TMRA_CAP_INT_SRC                    (INT_TMRA_12_CMP)
#define FAN_TMRA_OVF_INT_SRC					(INT_TMRA_12_OVF)
#define FAN_TMRA_CAP_INT_PRIO                   (DDL_IRQ_PRIORITY_03) //中断优先级
#define FAN_TMRA_CAP_INT_TYPE                   (TMRA_INT_CMP_CH4)
#define FAN_TMRA_CAP_FLAG                       (TMRA_FLAG_CMP_CH4)
#define FAN_TMRA_CAP_PERIOD  (6000UL - 1UL)


#define WATER_TMRA_PWM_A_CH             (TMRA_CH_1)
#define WATER_TMRA_PWM_A_PORT           (GPIO_PORT_B)
#define WATER_TMRA_PWM_A_PIN            (GPIO_PIN_03)
#define WATER_TMRA_PWM_A_PIN_FUNC       (GPIO_FUNC_5_TIMA12_PWM1)
	
#define WATER_TMRA_MODE                 (TMRA_MODE_SAWTOOTH)
#define WATER_TMRA_DIR                  (TMRA_DIR_UP)
#define WATER_TMRA_PERIOD_VAL           (6000UL - 1UL)
#define WATER_TMRA_PWM_A_CMP_VAL 		(1000UL - 1UL)	
	
#define WATER_TMRA_CAPT_PWM_PORT          		(GPIO_PORT_B)
#define WATER_TMRA_CAPT_PWM_PIN           		(GPIO_PIN_05)
#define WATER_TMRA_CAPT_PWM_PIN_FUNC      		(GPIO_FUNC_5_TIMA12_PWM3)

#define WATER_TMRA_CAP_UINT                       (M4_TMRA_12)
#define WATER_TMRA_CAP_CH                         (TMRA_CH_3)
#define WATER_TMRA_CAP_PERIP_CLK                  (PWC_FCG2_TMRA_12)
/* The divider of the clock source. @ref TMRA_PCLK_Divider */
#define WATER_TMRA_CLK                        	(TMRA_CLK_PCLK)
#define WATER_TMRA_PCLK_DIV                   	(TMRA_PCLK_DIV32)
#define WATER_CAPTURE_CONDITION               	(TMRA_CAPT_COND_PWMR)//捕获触发源为PWM的上升沿

#define WATER_TMRA_IRQn                       	(Int017_IRQn)
#define WATER_TMRA_OVF_IRQn						(Int018_IRQn)

#define WATER_TMRA_CAP_INT_SRC                    (INT_TMRA_12_CMP)
#define WATER_TMRA_OVF_INT_SRC					(INT_TMRA_12_OVF)
#define WATER_TMRA_CAP_INT_PRIO                   (DDL_IRQ_PRIORITY_03) //中断优先级
#define WATER_TMRA_CAP_INT_TYPE                   (TMRA_INT_CMP_CH3)
#define WATER_TMRA_CAP_FLAG                       (TMRA_FLAG_CMP_CH3)
#define WATER_TMRA_CAP_PERIOD  (6000UL - 1UL)

extern void (*Fan_Tim_Ch_Ic_CallBack)(void);		
extern void (*Water_Tim_Ch_Ic_CallBack)(void);		
extern void Bsp_Fan_Water_Init(void);                                                
extern void Fan_SetCmpVal(uint32_t value);
extern void Water_SetCmpVal(uint32_t value);
#endif
