#include <string.h>
#include "SidusProFX.h"
#include "SidusProFile.h"
#include "SidusPro_Interface.h"
#include "crc32.h"
#include <stdio.h>
/*User Includes Begin*/
/*User Includes End*/

//#define UART_DEBUG 0
#ifdef UART_DEBUG
    #include <stdio.h>
    #define  Sidus_Debug(...)    do{printf("<Sidus>: ");printf(__VA_ARGS__);}while(0)
#else
    #define  Sidus_Debug(...)
#endif

const uint8_t CFX_FileTag[16] = {0x8B, 0x99, 0x92, 0x3E, 0xF4, 0x90, 0x53, 0x80, 0xCA, 0xD7, 0xF9, 0x53, 0x2C, 0xF2, 0x33, 0x8D};
static CFX_Info_Type CFX_Info[3];
static CFX_Event_Body_Type CFX_Event;
static uint8_t ReadBuffer[SIDUSPRO_CFX_BANK_SECTOR_SIZE] = {0};
//static uint16_t Siduscount = 0;//队列标记
//static QNode p[30];
//static uint16_t qCount = 0;

//void EnQueue(uint8_t *data, uint16_t len)
//{
//        memcpy(p[qCount].rec_buff, data, len);
//        p[qCount].len = len;
//        qCount++;
//        if(qCount == 30)
//            qCount = 0;
//}

//void DeQueue(uint8_t *data, uint16_t count)
//{
//        memcpy(data, p[count].rec_buff, p[count].len);
//}

/***********************************************************************************************************************************/

/*****************************************************************************************************************************************/

static void SidusProFile_CFX_Int2Pwm(CFX_Pack_Type* Pack, SidusPro_Pwm_Type* PwmData)
{
    static SidusPro_Pwm_Type pwm_cache = {0};
    static CFX_Pack_Type pre_pack = {0};
    if(0 != memcmp(&pre_pack, Pack, sizeof(CFX_Pack_Type)))
    {
        if(1 == Pack->CCT_Pack.Mode)
        {
            SidusPro_CCT2PWM(Pack->CCT_Pack.CCT * 10, Pack->CCT_Pack.GM, Pack->CCT_Pack.Int, &pwm_cache);
        }
        else if(2 == Pack->CCT_Pack.Mode)
        {
            SidusPro_HSI2PWM(Pack->HSI_Pack.Hue, Pack->HSI_Pack.Sat, Pack->HSI_Pack.Int, &pwm_cache);
        }
        else
        {
            memset(&pwm_cache, 0, sizeof(SidusPro_Pwm_Type));
        }
        memcpy(&pre_pack, Pack, sizeof(CFX_Pack_Type));    
    }   
    memcpy(PwmData, &pwm_cache, sizeof(SidusPro_Pwm_Type));
}

static uint8_t SidusProFile_CFX_IntFileHead_Check(CFX_IntFileHead_Type* Head, uint8_t start_type)
{
    if(0 != memcmp(&Head->Tag[0], CFX_FileTag, sizeof(CFX_FileTag)))
    {
        return 1;
    }
    if(Head->FrameNum > MAX_PACKAGE_NUM)
    {
        return 2;
    }
    if(Head->FxType != start_type)
    {
        return 3;
    }
    return 0;

}
/*
@param SetOrClear: 0:清除对应bit  1:置位对应bit
*/
static void SidusProFile_CFX_BankInfo_Bit_Update(uint8_t Type, uint8_t Bank, uint8_t SetOrClear)
{
    if(Type > 2 || Bank > 9)
        return;

    if(0 == SetOrClear)
        CFX_Info[Type].Bank_Info &= ~(1 << Bank);
    else
        CFX_Info[Type].Bank_Info |= 1 << Bank;
}
//flash pwm 文件快速检查
bool SidusProFile_CFX_PwmFile_FastCheck(uint8_t Type, uint8_t Bank, char* name)
{
    uint32_t Addr = SidusPro_CFX_BankAddr[Type][Bank];
    CFX_PwmFileHead_Type* Head = NULL;
    SidusPro_CFX_Bank_Read(ReadBuffer, Addr, sizeof(CFX_PwmFileHead_Type));
    Head = (CFX_PwmFileHead_Type*)ReadBuffer;
    if(0 != memcmp(&Head->Tag[0], CFX_FileTag, sizeof(CFX_FileTag)) || Head->FrameNum > 7200 || Head->FxType != Type || Head->FxBank != Bank)
    {
        if(NULL != name)
            strcpy(name, "NO CFX");
        return false;
    }
    SidusPro_CFX_Bank_Read(ReadBuffer, Addr + sizeof(CFX_PwmFileHead_Type) + Head->FrameNum * sizeof(SidusPro_Pwm_Type), sizeof(CFX_FileTag));
    if(0 != memcmp(ReadBuffer, CFX_FileTag, sizeof(CFX_FileTag)))
    {
        if(NULL != name)
            strcpy(name, "NO CFX");
        return false;
    }
    if(Head->Name[0] != 0)
    {
        if(NULL != name)
            strcpy(name, Head->Name);
    }
    else
    {
        if(NULL != name)
            strcpy(name, "Untitled");
    }
    return true;
}


#if (0 == SIDUSPRO_CFX_RX_PACK_TYPE)//GATT
/** 
 * @brief   GATT数据解析
 * @param   buffer
 * @param   rx_len
 * @retval  None
 */
static void SidusProFile_CFX_Pack_Deal(uint8_t* buffer, uint8_t rx_len)
{
    static uint32_t RcvBinCrc = 0;
    static uint8_t Flag_Err = 0;
    static uint32_t FlashAddr = 0;
    static CFX_PwmFileHead_Type CFX_PwmFileHead = {0};
    static uint32_t Rcv_SeqNum_Count = 1;
    static uint32_t FrameWriteCnt = 0;//写pwm数据计数
    CFX_IntFileHead_Type* BinHead;
    CFX_Pack_Type* pPackData;
    SidusPro_Pwm_Type PwmData;
    static uint32_t WriteAddr = 0;
    uint32_t Frame2Write = 0;
    uint32_t CRC_RCV = 0;
    uint32_t pwmCrc = 0;
    uint32_t BinSize = 0;
    uint32_t length = 0;
    uint32_t SeqNum = 0;
    static uint8_t Type = 0;
    static uint8_t Bank = 0;
    uint32_t i = 0;
    switch(buffer[0])
    {
        case '1':       //1:xx xx xx xx开始信号
            if(buffer[2] == 0x01 && buffer[3] == 0x00)//file type 0x01 custom effect bin,
            {
                Type = buffer[4];
                Bank = buffer[5];
                if(Type > CFX_Type_Music || Bank > CFX_Bank_10)
                {
                    SidusPro_FileTransfer_Send_Status(CFX_FILE_RX_STATUS_ILLEGAL);
                }
                else
                {
                    RcvBinCrc = 0;   //CRC计算赋初值
                    Rcv_SeqNum_Count = 1;    //数据包计数置1
                    FlashAddr = SidusPro_CFX_BankAddr[Type][Bank];//SPI flash写地址
                    WriteAddr = FlashAddr;
                    FrameWriteCnt = 0;//写入帧数计数
                    SidusPro_FileTransfer_Send_Status(CFX_FILE_RX_STATUS_OK);
                }
            }
            Flag_Err = 0;
            break;
        case '2':       //1:xxxxxxxx  bin文件大小
            BinSize = (buffer[5] << 24) | (buffer[4] << 16) | (buffer[3] << 8) | buffer[2];
            if(BinSize > MAX_RCV_BIN_SIZE)
            {
                SidusPro_FileTransfer_Send_Status(CFX_FILE_RX_STATUS_INVALID_BINSIZE);
            }
            else
            {
                for(i = 0; i < SIDUSPRO_CFX_EACH_BANK_SECTOR_NUM; i++)
                {
                    SidusPro_CFX_Bank_SectorErase(FlashAddr + SIDUSPRO_CFX_BANK_SECTOR_SIZE * i);    
                }                    
                SidusProFile_CFX_BankInfo_Bit_Update(Type, Bank, 0);//清除对应bit
                SidusPro_FileTransfer_Send_Status(CFX_FILE_RX_STATUS_OK);
            }
            break;
        case '3':       //3:xx xxxxxxxx GATT数据包
            length = buffer[2] - 4;
            SeqNum = (buffer[6] << 24) | (buffer[5] << 16) | (buffer[4] << 8) | buffer[3];
            if(SeqNum != Rcv_SeqNum_Count && !Flag_Err)
            {
                Flag_Err = 1;
                SidusPro_FileTransfer_Send_Status(CFX_FILE_RX_STATUS_ILLEGAL);
                break;//break掉不写flash
            }
            Rcv_SeqNum_Count++;
            if(SeqNum == 1)
            {
                BinHead = (CFX_IntFileHead_Type*)&buffer[7];
                if(0 != SidusProFile_CFX_IntFileHead_Check(BinHead, Type))//检查原始bin文件头信息
                {
                    SidusPro_FileTransfer_Send_Status(CFX_FILE_RX_STATUS_INVALID_BIN);
                }
                else//生成本地pwm文件头信息
                {
                    //文件头信息前面部分相同，pwm头信息末尾添加pwm_crc和FxType
                    memcpy(&CFX_PwmFileHead, BinHead, sizeof(CFX_IntFileHead_Type));
                    CFX_PwmFileHead.FxBank = Bank;
                    CFX_PwmFileHead.PwmDataCrc = 0;
                }
                WriteAddr += sizeof(CFX_PwmFileHead_Type);//跳过头信息，最后才写
            }
            else
            {
                pPackData = (CFX_Pack_Type*)&buffer[7];
                Frame2Write = (CFX_PwmFileHead.FrameNum - FrameWriteCnt > 32) ? 32 : (CFX_PwmFileHead.FrameNum - FrameWriteCnt);
                for(i = 0; i < Frame2Write; i++)
                {
                    SidusProFile_CFX_Int2Pwm(pPackData, &PwmData);
                    SidusPro_CFX_Bank_Write((uint8_t*)&PwmData, WriteAddr, sizeof(SidusPro_Pwm_Type));
                    WriteAddr += sizeof(SidusPro_Pwm_Type);
                    CFX_PwmFileHead.PwmDataCrc = CRC32_Calculate(CFX_PwmFileHead.PwmDataCrc, (uint8_t*)&PwmData, sizeof(SidusPro_Pwm_Type));//计算本地pwm数据crc
                    pPackData++;
                }
                FrameWriteCnt += Frame2Write;
            }
            RcvBinCrc = CRC32_Calculate(RcvBinCrc, &buffer[7], length);//计算crc
            break;
        case '4':       //+4:xxxxxxxx结尾CRC校验
            pwmCrc = 0;
            for(i = 0; i < CFX_PwmFileHead.FrameNum; i++)
            {
                SidusPro_CFX_Bank_Read((uint8_t*)&PwmData, FlashAddr + sizeof(CFX_PwmFileHead_Type) + i * sizeof(SidusPro_Pwm_Type), sizeof(SidusPro_Pwm_Type));
                pwmCrc = CRC32_Calculate(pwmCrc, (uint8_t*)&PwmData, sizeof(SidusPro_Pwm_Type));//计算crc
            }
            CRC_RCV = (buffer[5] << 24) | (buffer[4] << 16) | (buffer[3] << 8) | buffer[2];
            if(RcvBinCrc == CRC_RCV && pwmCrc == CFX_PwmFileHead.PwmDataCrc)
            {
                SidusPro_CFX_Bank_Write((uint8_t*)&CFX_FileTag, WriteAddr, sizeof(CFX_FileTag));
                SidusPro_CFX_Bank_Write((uint8_t*)&CFX_PwmFileHead, FlashAddr, sizeof(CFX_PwmFileHead));
                SidusProFile_CFX_BankInfo_Bit_Update(CFX_PwmFileHead.FxType, CFX_PwmFileHead.FxBank, 1);//置位对应bit
                SidusProFile_CFX_PwmFile_FastCheck(CFX_PwmFileHead.FxType, CFX_PwmFileHead.FxBank, CFX_Info[CFX_PwmFileHead.FxType].Name[CFX_PwmFileHead.FxBank]);
                SidusPro_FileTransfer_Send_Status(CFX_FILE_RX_STATUS_OK);
            }
            else
            {
                SidusPro_FileTransfer_Send_Status(CFX_FILE_RX_STATUS_VERIFY_FAIL);
            }
            break;
        default:
            break;
    }
}
#else 
static void SidusProFile_CFX_Pack_Deal(uint8_t* buffer, uint8_t rx_len)
{
    static uint32_t RcvBinCrc = 0;
    static uint8_t Flag_Err = 0;
    static uint32_t FlashAddr = 0;
    static CFX_PwmFileHead_Type CFX_PwmFileHead = {0};
    static uint32_t Rcv_SeqNum_Count = 1;
    static uint32_t FrameWriteCnt = 0;//写pwm数据计数
    CFX_IntFileHead_Type* BinHead;
    CFX_Pack_Type* pPackData;
    SidusPro_Pwm_Type PwmData;
    static uint32_t WriteAddr = 0;
    uint32_t Frame2Write = 0;
    uint32_t CRC_RCV = 0;
    uint32_t pwmCrc = 0;
    uint32_t BinSize = 0;
    uint32_t length = 0;
    uint32_t SeqNum = 0;
    static uint8_t Type = 0;
    static uint8_t Bank = 0;
    uint32_t i = 0;
    switch(buffer[0])
    {
        case 0x01:       
            if(buffer[1] == 0x00)//file type
            {
                Type = buffer[2];
                Bank = buffer[3];
                if(Type > CFX_Type_Music || Bank > CFX_Bank_10)
                {
                    SidusPro_FileTransfer_Send_Status(CFX_FILE_RX_STATUS_ILLEGAL);
                }
                else
                {
                    RcvBinCrc = 0;   //CRC计算赋初值
                    Rcv_SeqNum_Count = 1;    //数据包计数置1
                    FlashAddr = SidusPro_CFX_BankAddr[Type][Bank];//SPI flash写地址
                    WriteAddr = FlashAddr;
                    FrameWriteCnt = 0;//写入帧数计数
                    SidusPro_FileTransfer_Send_Status(CFX_FILE_RX_STATUS_OK);
                }
            }
            Flag_Err = 0;
            break;
        case 0x02:      
            BinSize = (buffer[4] << 24) | (buffer[3] << 16) | (buffer[2] << 8) | buffer[1];
            if(BinSize > MAX_RCV_BIN_SIZE)
            {
                SidusPro_FileTransfer_Send_Status(CFX_FILE_RX_STATUS_INVALID_BINSIZE);
            }
            else
            {
                for(i = 0; i < SIDUSPRO_CFX_EACH_BANK_SECTOR_NUM; i++)
                {
                    SidusPro_CFX_Bank_SectorErase(FlashAddr + SIDUSPRO_CFX_BANK_SECTOR_SIZE * i);    
                }                    
                SidusProFile_CFX_BankInfo_Bit_Update(Type, Bank, 0);//清除对应bit
                SidusPro_FileTransfer_Send_Status(CFX_FILE_RX_STATUS_OK);
            }
            break;
        case 0x03:       
            length = buffer[1] - 4;
            SeqNum = (buffer[5] << 24) | (buffer[4] << 16) | (buffer[3] << 8) | buffer[2];
            if(SeqNum != Rcv_SeqNum_Count && !Flag_Err)
            {
                Flag_Err = 1;
                SidusPro_FileTransfer_Send_Status(CFX_FILE_RX_STATUS_ILLEGAL);
                break;//break掉不写flash
            }
            Rcv_SeqNum_Count++;
            if(SeqNum == 1)
            {
                BinHead = (CFX_IntFileHead_Type*)&buffer[6];
                if(0 != SidusProFile_CFX_IntFileHead_Check(BinHead, Type))//检查原始bin文件头信息
                {
                    SidusPro_FileTransfer_Send_Status(CFX_FILE_RX_STATUS_INVALID_BIN);
                }
                else//生成本地pwm文件头信息
                {
                    //文件头信息前面部分相同，pwm头信息末尾添加pwm_crc和FxType
                    memcpy(&CFX_PwmFileHead, BinHead, sizeof(CFX_IntFileHead_Type));
                    CFX_PwmFileHead.FxBank = Bank;
                    CFX_PwmFileHead.PwmDataCrc = 0;
                }
                WriteAddr += sizeof(CFX_PwmFileHead_Type);//跳过头信息，最后才写
            }
            else
            {
                pPackData = (CFX_Pack_Type*)&buffer[6];
                Frame2Write = (CFX_PwmFileHead.FrameNum - FrameWriteCnt > 32) ? 32 : (CFX_PwmFileHead.FrameNum - FrameWriteCnt);
                for(i = 0; i < Frame2Write; i++)
                {
                    SidusProFile_CFX_Int2Pwm(pPackData, &PwmData);
                    SidusPro_CFX_Bank_Write((uint8_t*)&PwmData, WriteAddr, sizeof(SidusPro_Pwm_Type));
                    WriteAddr += sizeof(SidusPro_Pwm_Type);
                    CFX_PwmFileHead.PwmDataCrc = CRC32_Calculate(CFX_PwmFileHead.PwmDataCrc, (uint8_t*)&PwmData, sizeof(SidusPro_Pwm_Type));//计算本地pwm数据crc
                    pPackData++;
                }
                FrameWriteCnt += Frame2Write;
            }
            RcvBinCrc = CRC32_Calculate(RcvBinCrc, &buffer[6], length);//计算crc
            break;
        case 0x04:       
            pwmCrc = 0;
            for(i = 0; i < CFX_PwmFileHead.FrameNum; i++)
            {
                SidusPro_CFX_Bank_Read((uint8_t*)&PwmData, FlashAddr + sizeof(CFX_PwmFileHead_Type) + i * sizeof(SidusPro_Pwm_Type), sizeof(SidusPro_Pwm_Type));
                pwmCrc = CRC32_Calculate(pwmCrc, (uint8_t*)&PwmData, sizeof(SidusPro_Pwm_Type));//计算crc
            }
            CRC_RCV = (buffer[4] << 24) | (buffer[3] << 16) | (buffer[2] << 8) | buffer[1];
            if(RcvBinCrc == CRC_RCV && pwmCrc == CFX_PwmFileHead.PwmDataCrc)
            {
                SidusPro_CFX_Bank_Write((uint8_t*)&CFX_FileTag, WriteAddr, sizeof(CFX_FileTag));
                SidusPro_CFX_Bank_Write((uint8_t*)&CFX_PwmFileHead, FlashAddr, sizeof(CFX_PwmFileHead));
                SidusProFile_CFX_BankInfo_Bit_Update(CFX_PwmFileHead.FxType, CFX_PwmFileHead.FxBank, 1);//置位对应bit
                SidusProFile_CFX_PwmFile_FastCheck(CFX_PwmFileHead.FxType, CFX_PwmFileHead.FxBank, CFX_Info[CFX_PwmFileHead.FxType].Name[CFX_PwmFileHead.FxBank]);
                SidusPro_FileTransfer_Send_Status(CFX_FILE_RX_STATUS_OK);
            }
            else
            {
                SidusPro_FileTransfer_Send_Status(CFX_FILE_RX_STATUS_VERIFY_FAIL);
            }
            break;
        default:
            break;
    }
}

#endif

/** 
 * @brief   SIDUSPRO_CFX_RX_PACK_TYPE=0时为GATT数据，传入开始为位置为+GATT:后数据，即为&ble_rxbuffer[6]
            SIDUSPRO_CFX_RX_PACK_TYPE=1时为485数据，传入数据为FileTransfer_Body_TypeDef，即为文件传输数据包的CommandBody
 * @param   p_data  数据地址
 * @param   len     数据长度
 * @retval  None
 */
void SidusProFile_FileTransfer_Receive(void* p_data, uint16_t len)
{
    CFX_GATT_Pack_Type Pack = {0};
    len = len > sizeof(Pack.Data) ? sizeof(Pack.Data) : len;
    Pack.length = len;
    memcpy(Pack.Data, p_data, len);
    SidusPro_Queue_Put(SidusPro_Queue_File_Transfer, &Pack);
}

void SidusProFile_Flash_Task(void)
{
    CFX_GATT_Pack_Type Pack = {0};
    CFX_Event_Body_Type Event = {CFX_Event_NULL};
    CFX_PwmFileHead_Type* p_head = NULL;
    static CFX_Init_Sta_Type Init_Sta = CFX_INIT_STA_IDLE;
    bool res = false;
    uint32_t CacheOneTime = 0;
    static uint32_t CacheFrameNum;
    static uint32_t CacheFrameCount;
    static uint32_t InternalAddr = 0;
    static uint32_t Bank_Addr  = 0;
    uint8_t* FileEndTag = NULL;
    CFX_PwmFileHead_Type* FlashFlie = NULL;
    CFX_PwmFileHead_Type* CacheFile = (CFX_PwmFileHead_Type*)(SIDUSPRO_CFX_CACHE_ADDR);
    static uint8_t CacheType = 0;
    static uint8_t CacheBank = 0;
    if(SidusPro_Queue_Get(SidusPro_Queue_File_Transfer, &Pack))
    {
        SidusProFile_CFX_Pack_Deal(Pack.Data, Pack.length);
    }
    if(CFX_Event.Event != CFX_Event_NULL)
    {
        memcpy(&Event, &CFX_Event, sizeof(CFX_Event_Body_Type));
        memset(&CFX_Event, 0, sizeof(CFX_Event_Body_Type));
        if(Event.Event == CFX_Event_Init)
        {            
            Bank_Addr = SidusPro_CFX_BankAddr[Event.Arg.RenameArg.Type][Event.Arg.RenameArg.Bank];
            CacheType = Event.Arg.RenameArg.Type;
            CacheBank = Event.Arg.RenameArg.Bank;
            Init_Sta = CFX_INIT_STA_FAST_CHECK;//go to init
        }
        else if(Event.Event == CFX_Event_Rename)
        {
            SidusPro_CFX_Bank_Read(ReadBuffer, SidusPro_CFX_BankAddr[Event.Arg.RenameArg.Type][Event.Arg.RenameArg.Bank], SIDUSPRO_CFX_BANK_SECTOR_SIZE);
            p_head  = (CFX_PwmFileHead_Type*)&ReadBuffer;
            strcpy(p_head->Name, Event.Arg.RenameArg.Name);
            SidusPro_CFX_Bank_SectorErase(SidusPro_CFX_BankAddr[Event.Arg.RenameArg.Type][Event.Arg.RenameArg.Bank]);
            SidusPro_CFX_Bank_Write(ReadBuffer, SidusPro_CFX_BankAddr[Event.Arg.RenameArg.Type][Event.Arg.RenameArg.Bank], SIDUSPRO_CFX_BANK_SECTOR_SIZE);
            res = SidusProFile_CFX_PwmFile_FastCheck(Event.Arg.RenameArg.Type, Event.Arg.RenameArg.Bank, CFX_Info[Event.Arg.RenameArg.Type].Name[Event.Arg.RenameArg.Bank]);
            SidusPro_CFX_Rename_Callback(res, Event.Arg.RenameArg.Type, Event.Arg.RenameArg.Bank, CFX_Info[Event.Arg.RenameArg.Type].Name[Event.Arg.RenameArg.Bank]);
        }
    }

    switch(Init_Sta)
    {
        case CFX_INIT_STA_FAST_CHECK://检查头信息
            FileEndTag = (uint8_t*)(SIDUSPRO_CFX_CACHE_ADDR + sizeof(CFX_PwmFileHead_Type) + CacheFile->FrameNum * sizeof(SidusPro_Pwm_Type));
            SidusPro_CFX_Bank_Read(ReadBuffer, Bank_Addr, sizeof(CFX_PwmFileHead_Type));
            FlashFlie = (CFX_PwmFileHead_Type*)ReadBuffer;
            if(0 == memcmp(&CacheFile->Tag[0], CFX_FileTag, sizeof(CFX_FileTag)) && CacheFile->FxType == CacheType 
                && CacheFile->FxBank == CacheBank && FlashFlie->PwmDataCrc == CacheFile->PwmDataCrc 
                && 0 == memcmp(CFX_FileTag, FileEndTag, sizeof(CFX_FileTag))
                && CacheFile->PwmDataCrc == CRC32_Calculate(0, (uint8_t*)(SIDUSPRO_CFX_CACHE_ADDR + sizeof(CFX_PwmFileHead_Type)), CacheFile->FrameNum * sizeof(SidusPro_Pwm_Type)))
            {
                SidusPro_CFX_Init_Callback(true ,CacheType, CacheBank);
                Init_Sta = CFX_INIT_STA_IDLE;
            }
            else
            {
                Init_Sta = CFX_INIT_STA_READ_BANK_HEAD;
            }
            break;
        case CFX_INIT_STA_READ_BANK_HEAD://检查文件头信息
            SidusPro_CFX_Bank_Read(ReadBuffer, Bank_Addr, sizeof(CFX_PwmFileHead_Type));
            FlashFlie = (CFX_PwmFileHead_Type*)ReadBuffer;
            if(FlashFlie->FxBank != CacheBank && FlashFlie->FxType != CacheType && 0 != memcmp(FlashFlie->Tag, CFX_FileTag, sizeof(CFX_FileTag)))
            {
                SidusPro_CFX_Init_Callback(false ,CacheType, CacheBank);
                Init_Sta = CFX_INIT_STA_IDLE;
            }
            else
            {
                CacheFrameNum = FlashFlie->FrameNum;
                Init_Sta = CFX_INIT_STA_ERASE_CACHE_AREA;
            }
            break;
        case CFX_INIT_STA_ERASE_CACHE_AREA://擦除缓存区
            SidusPro_CFX_CacheArea_Erase(sizeof(CFX_PwmFileHead_Type) + CacheFrameNum * sizeof(SidusPro_Pwm_Type) + sizeof(CFX_FileTag));
            InternalAddr = SIDUSPRO_CFX_CACHE_ADDR;
            SidusPro_CFX_CacheArea_Write(ReadBuffer, InternalAddr, sizeof(CFX_PwmFileHead_Type));
            InternalAddr += sizeof(CFX_PwmFileHead_Type);
            Bank_Addr += sizeof(CFX_PwmFileHead_Type);
            CacheFrameCount = 0;
            Init_Sta = CFX_INIT_STA_READ_BANK_DATA;
            break;
        case CFX_INIT_STA_READ_BANK_DATA://写缓存区
            if(CacheFrameCount < CacheFrameNum)
            {
                CacheOneTime = (CacheFrameNum - CacheFrameCount > 128) ? 128 : CacheFrameNum - CacheFrameCount;
                CacheFrameCount += CacheOneTime;
                SidusPro_CFX_Bank_Read(ReadBuffer, Bank_Addr, sizeof(SidusPro_Pwm_Type) * CacheOneTime);
                SidusPro_CFX_CacheArea_Write(ReadBuffer, InternalAddr, sizeof(SidusPro_Pwm_Type) * CacheOneTime);
                Bank_Addr += sizeof(SidusPro_Pwm_Type) * CacheOneTime;
                InternalAddr += sizeof(SidusPro_Pwm_Type) * CacheOneTime;
            }
            else
            {
                SidusPro_CFX_Bank_Read(ReadBuffer, Bank_Addr, sizeof(CFX_FileTag));
                SidusPro_CFX_CacheArea_Write(ReadBuffer, InternalAddr, sizeof(CFX_FileTag));
                Init_Sta = CFX_INIT_STA_CHECK_AFTER_WRITE;
            }
            break;
        case CFX_INIT_STA_CHECK_AFTER_WRITE:
            FileEndTag = (uint8_t*)(SIDUSPRO_CFX_CACHE_ADDR + sizeof(CFX_PwmFileHead_Type) + CacheFile->FrameNum * sizeof(SidusPro_Pwm_Type));
            if(0 == memcmp(&CacheFile->Tag[0], CFX_FileTag, sizeof(CFX_FileTag)) && CacheFile->FxType == CacheType 
                && CacheFile->FxBank == CacheBank && 0 == memcmp(CFX_FileTag, FileEndTag, sizeof(CFX_FileTag))
                && CacheFile->PwmDataCrc == CRC32_Calculate(0, (uint8_t*)(SIDUSPRO_CFX_CACHE_ADDR + sizeof(CFX_PwmFileHead_Type)), CacheFile->FrameNum * sizeof(SidusPro_Pwm_Type)))
            {
                SidusPro_CFX_Init_Callback(true ,CacheType, CacheBank);
            }
            else
            {
                SidusPro_CFX_Init_Callback(false ,CacheType, CacheBank);
            }
            Init_Sta = CFX_INIT_STA_IDLE;
            break;
        default:
            break;
    } 
}

void SidusProFile_System_Init(void)
{
    uint8_t i = 0, j = 0;
    for(i = 0; i < CFX_Type_Num; i ++)
    {
        for(j = 0; j < CFX_Bank_Num; j++)
        {
            if(SidusProFile_CFX_PwmFile_FastCheck(i, j, CFX_Info[i].Name[j]))
            {
                SidusProFile_CFX_BankInfo_Bit_Update(i, j, 1);//置位对应bit
            }
        }
    }
    SidusPro_Queue_Create(SidusPro_Queue_File_Transfer, 3, sizeof(CFX_GATT_Pack_Type));
}



bool SidusProFile_Set_Name(uint8_t type, uint8_t bank, const char* name)
{
    uint8_t ret = false;
    if(type < 3 && bank < 10 && NULL != name)
    {
        CFX_Event.Arg.RenameArg.Bank = bank;
        CFX_Event.Arg.RenameArg.Type = type;
        strcpy(CFX_Event.Arg.RenameArg.Name, name);
        CFX_Event.Event = CFX_Event_Rename;
        ret = true;
    }
    return ret;
}

bool SidusProFile_Get_Name(uint8_t type, uint8_t bank, char* name)
{
    uint8_t ret = false;
    if(CFX_Info[type].Bank_Info & 1 << bank && NULL != name)
    {
        strcpy(name, CFX_Info[type].Name[bank]);
        ret = true;
    }
    return ret;
}

char SidusProFile_SidusCode2ASCII(uint8_t sidus_char)
{
    char ascii_char = 0;
    if(sidus_char < 1)
    {
        ascii_char = ' ';
    }
    else if(sidus_char < 11)
    {
        ascii_char = '0' + sidus_char - 1;
    }
    else if(sidus_char < 37)
    {
        ascii_char = 'A' + sidus_char - 11;
    }
    else if(sidus_char < 63)
    {
        ascii_char = 'a' + sidus_char - 37;
    }
    else if(sidus_char < 64)
    {
        ascii_char = '_';
    }
    else
    {
        ascii_char = '\0';
    }
    return ascii_char;
}

uint8_t SidusProFile_ASCII2SidusCode(char ascii_char)
{
    uint8_t  sidus_code = 0;
    if('_' == ascii_char)
    {
        sidus_code = 0x3f;
    }
    else if('0' <= ascii_char && ascii_char <= '9')
    {
        sidus_code = 1 + ascii_char - '0';
    }
    else if('A' <= ascii_char && ascii_char <= 'Z')
    {
        sidus_code = 11 + ascii_char - 'A';
    }
    else if('a' <= ascii_char && ascii_char <= 'z')
    {
        sidus_code = 37 + ascii_char - 'a';
    }
    else
    {
        sidus_code = 0;//其余解析为空格
    }
    return sidus_code;
}

uint16_t SidusProFile_CFX_BankInfo_Get(uint8_t type)
{
    if(type >= CFX_Type_Num)
        return 0;
    return CFX_Info[type].Bank_Info;
}

void SidusProFile_CFX_File_Init(uint8_t type, uint16_t bank)
{
    if(type < 3 && bank < 10)
    {
        CFX_Event.Arg.RenameArg.Bank = bank;
        CFX_Event.Arg.RenameArg.Type = type;
        CFX_Event.Event = CFX_Event_Init;
    }
}
