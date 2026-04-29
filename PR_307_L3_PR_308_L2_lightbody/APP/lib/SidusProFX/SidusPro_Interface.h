#ifndef SIDUSPRO_INTERFACE_H
#define SIDUSPRO_INTERFACE_H
#include <stdint.h>
#include <stdbool.h>

#define SIDUSPRO_MFX_EN     (1)
#define SIDUSPRO_PFX_EN     (1)
#define SIDUSPRO_CFX_EN     (1)

#define SIDUSPRO_CFX_BANK_SECTOR_SIZE       (0x1000)//Flash 扇区大小 = 4k
#define SIDUSPRO_CFX_EACH_BANK_SECTOR_NUM   (18UL)  //单个Bank扇区数量
#define SIDUSPRO_CFX_RX_PACK_TYPE           (1)//CFX光效文件接收数据包类型 0：蓝牙GATT 1：485数据包

//CFX片上ROM或者RAM缓存地址，大小为SIDUSPRO_CFX_BANK_SECTOR_SIZE*SIDUSPRO_CFX_EACH_BANK_SECTOR_NUM
#define SIDUSPRO_CFX_CACHE_ADDR             (0x126000)


#define SIDUSPRO_PWM_CH_NUM         (5)//PWM 通道数量

typedef __packed struct
{
    uint16_t pwm[SIDUSPRO_PWM_CH_NUM];
} SidusPro_Pwm_Type;

typedef enum
{
    SidusPro_Queue_PWM,
    SidusPro_Queue_File_Transfer,
} SidusPro_Queue_Type;

extern const uint32_t SidusPro_CFX_BankAddr[3][10];

uint32_t SidusPro_Get_RandomRange(uint32_t min, uint32_t max);
void SidusPro_Set_Pwm(SidusPro_Pwm_Type* pwm_struct);
void SidusPro_CCT2PWM(uint16_t cct, uint8_t gm, uint16_t lightness, SidusPro_Pwm_Type* rgbww_pwm_struct);
void SidusPro_HSI2PWM(uint16_t hue, uint8_t sat, uint16_t lightness, SidusPro_Pwm_Type* rgbww_pwm_struct);
bool SidusPro_Queue_Create(SidusPro_Queue_Type queue, uint32_t count, uint32_t size);
bool SidusPro_Queue_Put(SidusPro_Queue_Type queue, const void * item);
bool SidusPro_Queue_Get(SidusPro_Queue_Type queue, void * item);
bool SidusPro_Queue_Reset(SidusPro_Queue_Type queue);
bool SidusPro_Queue_Is_Full(SidusPro_Queue_Type queue);
void SidusPro_CFX_Bank_SectorErase(uint32_t addr);
void SidusPro_CFX_Bank_Write(uint8_t* p_buffer, uint32_t addr, uint16_t size);
void SidusPro_CFX_Bank_Read(uint8_t* p_buffer, uint32_t addr, uint16_t size);
void SidusPro_CFX_CacheArea_Erase(uint32_t size);
void SidusPro_CFX_CacheArea_Write(uint8_t* p_buffer, uint32_t addr, uint32_t size);
void SidusPro_FileTransfer_Send_Status(uint8_t status);
void SidusPro_CFX_Init_Callback(bool result, uint8_t type, uint8_t bank);
void SidusPro_CFX_Rename_Callback(bool result, uint8_t type, uint8_t bank, const char* new_name);

extern void Set_Drive_Cob_Pwm_Val(uint16_t* p_pwm);
extern void set_self_adapt_pwm(uint16_t* p_pwm);
#endif

