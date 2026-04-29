#ifndef SIDUSPROFILE_H
#define SIDUSPROFILE_H
#include <stdint.h>
#include <stdbool.h>

#define PACKAGE_PER_SEC             (60UL)
#define MAX_EFFECT_SEC              (120UL)
#define MAX_PACKAGE_NUM             (PACKAGE_PER_SEC * MAX_EFFECT_SEC)
#define MAX_RCV_BIN_SIZE            (128 + (MAX_PACKAGE_NUM * 4) + 16)



typedef enum
{
    CFX_FILE_RX_STATUS_OK,
    CFX_FILE_RX_STATUS_UNSUPPORTED,
    CFX_FILE_RX_STATUS_ILLEGAL,
    CFX_FILE_RX_STATUS_VERIFY_FAIL,
    CFX_FILE_RX_STATUS_INVALID_BIN,
    CFX_FILE_RX_STATUS_INVALID_BINSIZE,
    CFX_FILE_RX_STATUS_MORE_DATA,
    CFX_FILE_RX_STATUS_INVALID_APPID,
    CFX_FILE_RX_STATUS_INVALID_VERSION,
    CFX_FILE_RX_STATUS_CONTINUE
}CFX_File_RX_Status_Type;

typedef __packed struct
{
    uint8_t  Tag[16];       //文件标识符                               ---16Byte
    uint8_t  TxMode;        //传输模式 0:SidusProFX                   ---1Byte
    uint16_t FxType;        //子模式 0:Picker 1:TouchBar 2:Music     ---2Byte
    uint16_t Supported;     //支持光效类型                              ---2Byte
    uint16_t UintTime;      //单位时间/采样率                            ---2Byte
    uint8_t  Bars;          //小节格式                                ---1Byte
    uint32_t FrameNum;      //帧数                                  ---4Byte
    char     Name[10];      //光效名                                 ---10Byte
    uint8_t  Reserve[90];   //预留                                  ---90Byte
} CFX_IntFileHead_Type; //接收bin文件数据头信息                            ---128Byte

typedef struct
{
    uint32_t Int    : 10;
    uint32_t GM     : 10;
    uint32_t CCT    : 10;
    uint32_t Mode   : 2;     //0:空 1:CCT 2:HSI
} CFX_CCT_Pack_Type;

typedef struct
{
    uint32_t Int    : 10;    
    uint32_t Sat    : 10;
    uint32_t Hue    : 10;
    uint32_t Mode   : 2;     //0:空 1:CCT 2:HSI
} CFX_HSI_Pack_Type;

typedef union
{
    CFX_CCT_Pack_Type CCT_Pack;
    CFX_HSI_Pack_Type HSI_Pack;
} CFX_Pack_Type;

typedef struct 
{
    uint16_t Bank_Picker;
    uint16_t Bank_TouchBar;
    uint16_t Bank_Music;
}CFX_FileSystemInfo_Type;

typedef struct 
{
    volatile uint16_t Bank_Info;
    char     Name[10][10];
}CFX_Info_Type;

typedef __packed struct
{
    uint8_t                 length;
    uint8_t                 Data[156];
}CFX_GATT_Pack_Type;

typedef enum 
{
    CFX_Event_NULL,
    CFX_Event_Init,
    CFX_Event_Rename,
}CFX_Event_Type;

typedef enum 
{
    CFX_INIT_STA_IDLE,
    CFX_INIT_STA_FAST_CHECK,
    CFX_INIT_STA_READ_BANK_HEAD,
    CFX_INIT_STA_ERASE_CACHE_AREA,
    CFX_INIT_STA_READ_BANK_DATA,
    CFX_INIT_STA_CHECK_AFTER_WRITE,
}CFX_Init_Sta_Type;


typedef __packed struct
{
    uint8_t Type;
    uint8_t Bank;
}CFX_Event_Init_Arg_Type;

typedef __packed struct
{
    uint8_t Type;
    uint8_t Bank;
    char Name[10];
}CFX_Event_Rename_Arg_Type;


typedef union 
{
    CFX_Event_Init_Arg_Type      InitArg;
    CFX_Event_Rename_Arg_Type    RenameArg;
}CFX_Event_Arg_Type;
typedef struct
{
    CFX_Event_Type      Event;
    CFX_Event_Arg_Type  Arg;
}CFX_Event_Body_Type;

/***********************必须调用函数*********************************/

/*  SIDUSPRO_CFX_RX_PACK_TYPE=0时为GATT数据，传入开始为位置为+GATT:后数据，即为&ble_rxbuffer[6]
    SIDUSPRO_CFX_RX_PACK_TYPE=1时为485数据，传入数据为FileTransfer_Body_TypeDef，即为文件传输数据包的CommandBody */
void SidusProFile_FileTransfer_Receive(void* p_data, uint16_t len);//接收到CFX文件传输数据时调用
void SidusProFile_Flash_Task(void);//Flash任务循环调用
void SidusProFile_System_Init(void);//上电调用

/***********************功能接口*********************************/
bool SidusProFile_Set_Name(uint8_t type, uint8_t bank, const char* name);//重命名光效文件
bool SidusProFile_Get_Name(uint8_t type, uint8_t bank, char* name);//获取光效文件名称
char SidusProFile_SidusCode2ASCII(uint8_t sidus_char);//Sidus Code转Ascii码，App读取时需要转换为Sidus Code回复
uint8_t SidusProFile_ASCII2SidusCode(char ascii_char);//Ascii码转Sidus Code，App设置文件名时需转换为Ascii码后设置
uint16_t SidusProFile_CFX_BankInfo_Get(uint8_t type);//获取光效Bank信息，bit0-9代表Bank1-10有无光效文件

/****************************************分割线**********************************************/
void SidusProFile_CFX_File_Init(uint8_t type, uint16_t bank);//CFX Init函数，SidusProFX.c中会调用
bool SidusProFile_CFX_PwmFile_FastCheck(uint8_t Type, uint8_t Bank, char* name);

#endif

