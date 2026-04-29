#ifndef SYS_H
#define SYS_H
 
#include "hc32_ddl.h"

#define BITBAND(addr, bitnum) ((addr & 0xF0000000)+0x2000000+((addr &0xFFFFF)<<5)+(bitnum<<2)) 
#define MEM_ADDR(addr)  *((volatile unsigned long  *)(addr)) 
#define BIT_ADDR(addr, bitnum)   MEM_ADDR(BITBAND(addr, bitnum)) 

#define GPIO_A_ADDR_OD ((uint32_t)0x40053800UL+0x04+0x10*GPIO_PORT_A)
#define GPIO_A_ADDR_ID ((uint32_t)0x40053800UL+0x00+0x10*GPIO_PORT_A)

#define GPIO_B_ADDR_OD ((uint32_t)0x40053800UL+0x04+0x10*GPIO_PORT_B)
#define GPIO_B_ADDR_ID ((uint32_t)0x40053800UL+0x00+0x10*GPIO_PORT_B)

#define GPIO_C_ADDR_OD ((uint32_t)0x40053800UL+0x04+0x10*GPIO_PORT_C)
#define GPIO_C_ADDR_ID ((uint32_t)0x40053800UL+0x00+0x10*GPIO_PORT_C)

#define GPIO_D_ADDR_OD ((uint32_t)0x40053800UL+0x04+0x10*GPIO_PORT_D)
#define GPIO_D_ADDR_ID ((uint32_t)0x40053800UL+0x00+0x10*GPIO_PORT_D)

#define GPIO_E_ADDR_OD ((uint32_t)0x40053800UL+0x04+0x10*GPIO_PORT_E)
#define GPIO_E_ADDR_ID ((uint32_t)0x40053800UL+0x00+0x10*GPIO_PORT_E)

#define GPIO_F_ADDR_OD ((uint32_t)0x40053800UL+0x04+0x10*GPIO_PORT_F)
#define GPIO_F_ADDR_ID ((uint32_t)0x40053800UL+0x00+0x10*GPIO_PORT_F)

#define GPIO_G_ADDR_OD ((uint32_t)0x40053800UL+0x04+0x10*GPIO_PORT_G)
#define GPIO_G_ADDR_ID ((uint32_t)0x40053800UL+0x00+0x10*GPIO_PORT_G)

#define GPIO_H_ADDR_OD ((uint32_t)0x40053800UL+0x04+0x10*GPIO_PORT_H)
#define GPIO_H_ADDR_ID ((uint32_t)0x40053800UL+0x00+0x10*GPIO_PORT_H)

#define GPIO_I_ADDR_OD ((uint32_t)0x40053800UL+0x04+0x10*GPIO_PORT_I)
#define GPIO_I_ADDR_ID ((uint32_t)0x40053800UL+0x00+0x10*GPIO_PORT_I)


#define PAout(n)   BIT_ADDR(GPIO_A_ADDR_OD,n)  //输出 
#define PAin(n)    BIT_ADDR(GPIO_A_ADDR_ID,n)  //输入 

#define PBout(n)   BIT_ADDR(GPIO_B_ADDR_OD,n)  //输出 
#define PBin(n)    BIT_ADDR(GPIO_B_ADDR_ID,n)  //输入 

#define PCout(n)   BIT_ADDR(GPIO_C_ADDR_OD,n)  //输出 
#define PCin(n)    BIT_ADDR(GPIO_C_ADDR_ID,n)  //输入 

#define PDout(n)   BIT_ADDR(GPIO_D_ADDR_OD,n)  //输出 
#define PDin(n)    BIT_ADDR(GPIO_D_ADDR_ID,n)  //输入 

#define PEout(n)   BIT_ADDR(GPIO_E_ADDR_OD,n)  //输出 
#define PEin(n)    BIT_ADDR(GPIO_E_ADDR_ID,n)  //输入 

#define PFout(n)   BIT_ADDR(GPIO_F_ADDR_OD,n)  //输出 
#define PFin(n)    BIT_ADDR(GPIO_F_ADDR_ID,n)  //输入 

#define PGout(n)   BIT_ADDR(GPIO_G_ADDR_OD,n)  //输出 
#define PGin(n)    BIT_ADDR(GPIO_G_ADDR_ID,n)  //输入 

#define PHout(n)   BIT_ADDR(GPIO_H_ADDR_OD,n)  //输出 
#define PHin(n)    BIT_ADDR(GPIO_H_ADDR_ID,n)  //输入 

#define PIout(n)   BIT_ADDR(GPIO_I_ADDR_OD,n)  //输出 
#define PIin(n)    BIT_ADDR(GPIO_I_ADDR_ID,n)  //输入 

#define WR_PIN				MEM_ADDR
#define WR_REG				MEM_ADDR

#endif
