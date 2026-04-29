/*file:security.h*/
#ifndef __SECURITY_H
#define	__SECURITY_H
#include <stdint.h>
#include "aes.h"

/*
CHIP_ID_ADDRESS :chip数据地址，用户根据不同的MCU定义
STM32F0:0x1FFFF7AC
STM32F1:0x1FFFF7E8
STM32F2:0x1FFF7A10
STM32F3:0x1FFFF7AC
STM32F4:0x1FFF7A10
STM32F7:0x1FF0F420
STM32L0:0x1FF80050
STM32L1:0x1FF80050
STM32L4:0x1FFF7590
STM32H7:0x1FF0F420
HC32F4A:0x40010450
*/
#define CHIP_ID_ADDRESS	   0x40010450

extern AES_ctx security_ctx;

//函数地址结合chip id进行秘钥拓展
void AES_Key_Create(uint32_t fun_address);
//对加密的函数进行解密，返回函数地址
uint32_t Function_Decrypt(const uint8_t *data);

#endif /*__SECURITY_H*/
