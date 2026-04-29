/*********************************************************************************
  *Copyright(C), 2021, Aputure, All rights reserver.
  *FileName:    SidusProFX.c
  *Author:      
  *Version:     v1.3
  *Date:        2021/04/17
  *Description: SidusPro™ FX模块
  *History:  
            -v1.0: 2020/06/18,Matthew,初始版本
            -v1.1: 2020/08/06,Matthew,修复MFX光效部分Bug
            -v1.2: 2020/12/17,Matthew,新增pwm数据队列操作接口,修复部分MFX光效bug
            -v1.3: 2021/04/17,Matthew,用户实现接口新增及修改,CFX文件名功能新增,MFX光效优化

**********************************************************************************/
#include <string.h>
#include <math.h>
#include "SidusProFX.h"
#include "SidusPro_Interface.h"
#include "SidusProFile.h"

#define SFX_PWM_FIFO_LEN    (8)

static uint8_t Flag_SidusProFX_Arg_Init = 0;;
static bool SidusProFX_EN = false;
static SidusProFX_ArgBody_Type SidusPro_Arg[2];
static SidusProFX_ArgBody_Type* pSFX_Arg = &SidusPro_Arg[0];

static uint16_t UnitTimeTab[1000] = {0};//单位0.1S
static uint16_t FreeTimeTab[1000] = {0};
static uint16_t FadeInTimeTab[1000] = {0};
static uint16_t FadeOutTimeTab[1000] = {0};
static uint16_t OverLapWith[1000] = {0};

static uint16_t CFX_Period = 1;//

static uint16_t Curvilinear_Function(uint16_t x, uint8_t Curve)
{
    uint16_t y = 0;
    switch(Curve)
    {
        case 0:
            y = 10 * x;
            break;
        case 1:
            if(x <= 200)
                y = 6.1366e-06f * pow(x, 3) + 0.0015599f * pow(x, 2) + 0.42061f * x + 45.018f;
            else if(x <= 400)
                y = 8.3139e-06f * pow(x, 3) + 0.00025628f * pow(x, 2) + 0.68703f * x + 26.494f;
            else if(x <= 600)
                y = 1.0785e-05f * pow(x, 3) - 0.0027202f * pow(x, 2) + 1.9006f * x - 141.06f;
            else if(x <= 800)
                y = 1.3183e-05f * pow(x, 3) - 0.0070735f * pow(x, 2) + 4.555f * x - 684.42f;
            else
                y = 1.5579e-05f * pow(x, 3) - 0.012882f * pow(x, 2) + 9.2668f * x - 1963.2f;
            break;
        case 2:
            if(x <= 200)
                y = 2.57e-05f * pow(x, 3) - 0.039774f * pow(x, 2) + 31.136f * x + 40.671f;
            else if(x <= 400)
                y = 1.5226e-05f * pow(x, 3) - 0.033649f * pow(x, 2) + 29.889f * x + 128.92f;
            else if(x <= 600)
                y = 8.6863e-06f * pow(x, 3) - 0.025955f * pow(x, 2) + 26.831f * x + 539.77f;
            else if(x <= 800)
                y = 6.9831e-06f * pow(x, 3) - 0.022942f * pow(x, 2) + 25.041f * x + 896.88f;
            else
                y = 1.0127e-05f * pow(x, 3) - 0.030696f * pow(x, 2) + 31.434f * x - 864.22f;
            break;
        case 3:
            if(x <= 200)
                y = 3.1762e-05f * pow(x, 3) + 0.00099877f * pow(x, 2) + 0.66545f * x + 56.574f;
            else if(x <= 400)
                y = 5.9069e-05f * pow(x, 3) - 0.012245f * pow(x, 2) + 1.9995f * x + 101.29f;
            else if(x <= 600)
                y = -0.00016184f * pow(x, 3) + 0.24237f * pow(x, 2) - 96.498f * x + 12895;
            else if(x <= 800)
                y =  6.3445e-05f * pow(x, 3) - 0.17327f * pow(x, 2) + 159.83f * x - 39942;
            else
                y = 2.8906e-05f * pow(x, 3) - 0.089515f * pow(x, 2) + 92.927f * x - 22336;
            break;
        default:
            y = 10 * x;
            break;
    }
    y = (x == 0 ? 0 : (y > 10000 ? 10000 : y));
    return y;
}


static void SidusProFX_Source2Pwm(SFX_Source_Type* Source, SidusPro_Pwm_Type* Pwm)
{
    static SFX_Source_Type Pre_Source = {0};
    static SidusPro_Pwm_Type PwmCache = {0};
    if(0 != memcmp(&Pre_Source, Source, sizeof(SFX_Source_Type)))
    {
        memcpy(&Pre_Source, Source, sizeof(SFX_Source_Type));
        if(Source->CCT.Base == 0)
        {
            SidusPro_CCT2PWM(Source->CCT.CCT, Source->CCT.GM, Source->CCT.Int, &PwmCache);
        }
        else
        {
            SidusPro_HSI2PWM(Source->HSI.Hue, Source->HSI.Sat, Source->HSI.Int, &PwmCache);
        }
    }
    memcpy(Pwm, &PwmCache, sizeof(SidusPro_Pwm_Type));
}
/*
Frq转周期时间
1-9:    0.1Hz-0.9Hz
10-129:    1Hz-120Hz
*/
static uint32_t MFX_Frq2Period(uint8_t Frq)
{
    uint32_t Period = 1;
    if(Frq > 129)
    {
        Period = 1;     //1 Flicker Free频率周期
    }
    if(Frq >= 10)
    {
        Period = 1000 / (Frq - 9);//Frq - 9 得到实际频率
    }
    else
    {
        Period = 1000 * 10 / Frq;
    }
    return Period;
}

static uint32_t MFX_Source_Get(uint32_t mini, uint32_t max, uint32_t step, uint8_t Seq, uint32_t CurTime, uint32_t WholeTime)
{
    uint32_t Result = 0;
    uint32_t temp = 0;
    if(mini > max)
    {
        temp = max;
        max = mini;
        mini = temp;
    }
    switch(Seq)
    {
        case 0:
            Result = mini + step * ((max / step - mini / step) * CurTime / WholeTime);
            break;
        case 1:
            Result = max - step * ((max / step - mini / step) * CurTime / WholeTime);
            break;
        case 2:
            if(CurTime < (WholeTime / 2))
            {
                Result = mini + step * ((max / step - mini / step) * CurTime / (WholeTime / 2));
            }
            else
            {
                Result = max - step * ((max / step - mini / step) * (CurTime - WholeTime / 2) / (WholeTime / 2));
            }
            break;
        case 3:
            Result = step * SidusPro_Get_RandomRange(mini / step, max / step);
            break;
        default:
            break;
    }
    return Result;
}

static uint32_t MFX_CycleTime_Calculate(uint32_t mini, uint32_t max, uint8_t Seq, uint32_t TimesIndex, uint32_t Times)
{
    uint32_t CycleTime = 0;
    if(Times < 2)
        return Seq == 1 ? max : mini;
    switch(Seq)
    {
        case 0:
            CycleTime = mini + TimesIndex * (max - mini) / (Times - 1);
            break;
        case 1:
            CycleTime = mini + (Times - (TimesIndex + 1)) * (max - mini) / (Times - 1);
            break;
        case 2:
            if(Times > 2)//除数不能为0
            {
                if(TimesIndex <= Times / 2)
                    CycleTime = mini + TimesIndex * (max - mini) / (Times - Times / 2 - 1);
                else
                    CycleTime = mini + (Times - (TimesIndex + 1)) * (max - mini) / (Times - Times / 2 - 1);
            }
            else//两次的情况
            {
                CycleTime = (TimesIndex == 0) ? mini : max;
            }
            break;
        case 3:
            CycleTime = SidusPro_Get_RandomRange(mini, max);
            break;
        default:
            break;
    }
    return CycleTime;
}

static uint8_t MFX_UnitFrq_Calculate(uint8_t mini, uint8_t max, uint8_t Seq, uint16_t UnitIndex, uint32_t UnitNum)
{
    uint8_t Frq = 0;
    if(UnitNum < 2)//少于2个单元直接返回
        return Seq == 1 ? max : mini;//倒序返回max
    switch(Seq)
    {
        case 0:
            Frq = mini + UnitIndex * (max - mini) / (UnitNum - 1);
            break;
        case 1:
            Frq = mini + (UnitNum - (UnitIndex + 1)) * (max - mini) / (UnitNum - 1);
            break;
        case 2:
            if(UnitNum > 2)//除数不能为0
            {
                if(UnitIndex < UnitNum / 2)
                    Frq = mini + UnitIndex * (max - mini) / (UnitNum - UnitNum / 2 - 1);
                else
                    Frq = mini + (UnitNum - (UnitIndex + 1)) * (max - mini) / (UnitNum - UnitNum / 2 - 1);
            }
            else
            {
                Frq = (UnitIndex == 0) ? mini : max;
            }
            break;
        case 3://和顺序一样，Unit Index在开始就已经打乱
            Frq = SidusPro_Get_RandomRange(mini, max);
            break;
        default:
            break;
    }
    return Frq;
}

static uint16_t MFX_Unit_Overlap_With_Get(uint8_t Seq, uint16_t UnitIndex, uint32_t UnitNum)
{
    uint16_t OverlapWith = 0;
    if(UnitNum < 2)//少于2个单元直接返回最小值
        return 0;   //一个单元的时候和自己重合
    switch(Seq)
    {
        case 0://012345单元分别和
            if(UnitIndex < UnitNum - 1)
                OverlapWith = UnitIndex + 1;//和后面一个单元重合
            else
                OverlapWith = 0;//最后一个单元和第0个单元重合
            break;
        case 1://012345单元分别和543210单元重合
            OverlapWith = UnitNum - UnitIndex - 1;
            break;
        case 2://012345分别和135420         01234分别和13420
            if(UnitIndex < UnitNum / 2)
                OverlapWith = 1 + UnitIndex * 2;
            else
                OverlapWith = 2 * (UnitNum - 1 - UnitIndex);
            break;
        case 3://和顺序一样，后面需要打乱处理
            if(UnitIndex < UnitNum - 1)
                OverlapWith = UnitIndex + 1;//和后面一个单元重合
            else
                OverlapWith = 0;//最后一个单元和第0个单元重合
            break;
        default:
            break;
    }
    return OverlapWith;
}




static uint32_t MFX_UnitNum_Calculate(uint32_t mini_unit, uint32_t max_unit, uint8_t seq_unit, uint32_t mini_free, uint32_t max_free, uint8_t seq_free, uint32_t cycle_time)
{
    uint32_t UnitNum = 2;
    uint32_t AvgFreeTime = 0;
    uint32_t AvgUnitTime = 0;
    AvgFreeTime = (mini_free + max_free) / 2;
    AvgUnitTime = (mini_unit + max_unit) / 2;

    if(AvgFreeTime + AvgUnitTime > 0)
    {
        UnitNum  = cycle_time / (AvgFreeTime + AvgUnitTime) + (cycle_time % (AvgFreeTime + AvgUnitTime) == 0 ? 0 : 1);
    }
    else//避免Unit Time合Free Time 都为 0
    {
        UnitNum = 2;
    }
    return UnitNum > 2 ? UnitNum : 2;
}


static uint32_t MFX_UnitTime_Calculate(uint32_t mini, uint32_t max, uint8_t Seq, uint16_t UnitIndex, uint32_t UnitNum)
{
    uint32_t UnitTime = 0;
    if(UnitNum < 2)//少于2个单元直接返回
        return Seq == 1 ? max : mini;//倒序返回max
    switch(Seq)
    {
        case 0:
            UnitTime = mini + UnitIndex * (max - mini) / (UnitNum - 1);
            break;
        case 1:
            UnitTime = mini + (UnitNum - (UnitIndex + 1)) * (max - mini) / (UnitNum - 1);
            break;
        case 2:
            if(UnitNum > 2)//除数不能为0
            {
                if(UnitIndex < UnitNum / 2)
                    UnitTime = mini + UnitIndex * (max - mini) / (UnitNum - UnitNum / 2 - 1);
                else
                    UnitTime = mini + (UnitNum - (UnitIndex + 1)) * (max - mini) / (UnitNum - UnitNum / 2 - 1);
            }
            else
            {
                UnitTime = (UnitIndex == 0) ? mini : max;
            }
            break;
        case 3://和顺序一样，需要做打乱处理
            UnitTime = mini + UnitIndex * (max - mini) / (UnitNum - 1);
            break;
        default:
            break;
    }
    return UnitTime;
}




static void MFX_TimeTab_Break(uint16_t* buffer, uint16_t length)
{
    uint16_t RandomIndex = 0;
    uint16_t Index = 0;
    uint16_t Temp = 0;
    if(NULL == buffer || 0 == length)
        return ;

    for(Index = 0; Index < length; Index ++)
    {
        RandomIndex = Index + SidusPro_Get_RandomRange(0, length - Index - 1);

        Temp = buffer[Index];
        buffer[Index] = buffer[RandomIndex];
        buffer[RandomIndex] = Temp;
    }
}
/*
*/
static void MFX_SourceBase_Chase(MFX_Base_Type* pBase, SFX_Source_Type* Source, uint32_t CurTime, uint32_t CycleTime)
{
    static SFX_Source_Type MemSource = {0}; //保存上一次Chase的数据，避免随机的时候没有输出
    if(CurTime < CycleTime)
    {
        switch(pBase->BaseCCT.Base)
        {
            case 0:
                MemSource.CCT.Base = 0;
                if(pBase->BaseCCT.CCT_Seq == 3)//随机
                {
                    if(0 == CurTime % (CycleTime / (pBase->BaseCCT.CCT_Max / 100 - pBase->BaseCCT.CCT_Mini / 100)))
                        MemSource.CCT.CCT = MFX_Source_Get(pBase->BaseCCT.CCT_Mini, pBase->BaseCCT.CCT_Max, 100, pBase->BaseCCT.CCT_Seq, CurTime, CycleTime);
                }
                else
                {
                    MemSource.CCT.CCT = MFX_Source_Get(pBase->BaseCCT.CCT_Mini, pBase->BaseCCT.CCT_Max, 100, pBase->BaseCCT.CCT_Seq, CurTime, CycleTime);
                }
                if(pBase->BaseCCT.Int_Seq == 3)//随机
                {
                    if(0 == CurTime % (CycleTime / (pBase->BaseCCT.Int_Max - pBase->BaseCCT.Int_Mini)))
                        MemSource.CCT.Int = MFX_Source_Get(pBase->BaseCCT.Int_Mini, pBase->BaseCCT.Int_Max, 1, pBase->BaseCCT.Int_Seq, CurTime, CycleTime);
                }
                else
                {
                    MemSource.CCT.Int = MFX_Source_Get(pBase->BaseCCT.Int_Mini, pBase->BaseCCT.Int_Max, 1, pBase->BaseCCT.Int_Seq, CurTime, CycleTime);
                }
                if(pBase->BaseCCT.GM_Seq == 3)//随机
                {
                    if(0 == CurTime % (CycleTime / (pBase->BaseCCT.GM_Max - pBase->BaseCCT.GM_Mini)))
                        MemSource.CCT.GM = MFX_Source_Get(pBase->BaseCCT.GM_Mini, pBase->BaseCCT.GM_Max, 1, pBase->BaseCCT.GM_Seq, CurTime, CycleTime);
                }
                else
                {
                    MemSource.CCT.GM = MFX_Source_Get(pBase->BaseCCT.GM_Mini, pBase->BaseCCT.GM_Max, 1, pBase->BaseCCT.GM_Seq, CurTime, CycleTime);
                }
                break;
            case 1:
                MemSource.CCT.Base = 1;//位置相同
                if(pBase->BaseHSI.Hue_Seq == 3)//随机
                {
                    if(0 == CurTime % (CycleTime / (pBase->BaseHSI.Hue_Max - pBase->BaseHSI.Hue_Mini)))
                        MemSource.HSI.Hue = MFX_Source_Get(pBase->BaseHSI.Hue_Mini, pBase->BaseHSI.Hue_Max, 1, pBase->BaseHSI.Hue_Seq, CurTime, CycleTime);
                }
                else
                {
                    MemSource.HSI.Hue = MFX_Source_Get(pBase->BaseHSI.Hue_Mini, pBase->BaseHSI.Hue_Max, 1, pBase->BaseHSI.Hue_Seq, CurTime, CycleTime);
                }
                if(pBase->BaseHSI.Sat_Seq == 3)//随机
                {
                    if(0 == CurTime % (CycleTime / (pBase->BaseHSI.Sat_Max - pBase->BaseHSI.Sat_Mini)))
                        MemSource.HSI.Sat = MFX_Source_Get(pBase->BaseHSI.Sat_Mini, pBase->BaseHSI.Sat_Max, 1, pBase->BaseHSI.Sat_Seq, CurTime, CycleTime);
                }
                else
                {
                    MemSource.HSI.Sat = MFX_Source_Get(pBase->BaseHSI.Sat_Mini, pBase->BaseHSI.Sat_Max, 1, pBase->BaseHSI.Sat_Seq, CurTime, CycleTime);
                }
                if(pBase->BaseHSI.Int_Seq == 3)//随机
                {
                    if(0 == CurTime % (CycleTime / (pBase->BaseHSI.Int_Max - pBase->BaseHSI.Int_Mini)))
                        MemSource.HSI.Int = MFX_Source_Get(pBase->BaseHSI.Int_Mini, pBase->BaseHSI.Int_Max, 1, pBase->BaseHSI.Int_Seq, CurTime, CycleTime);
                }
                else
                {
                    MemSource.HSI.Int = MFX_Source_Get(pBase->BaseHSI.Int_Mini, pBase->BaseHSI.Int_Max, 1, pBase->BaseHSI.Int_Seq, CurTime, CycleTime);
                }
                break;
            default:
                break;
        }
    }
    else
    {
        MemSource.CCT.Int = 0;//超过Cycle Time的部分为零
    }
    //拷贝输出，随机模式的时候没到随机时间点输出的是上一次随机的值，避免一直随机
    memcpy(Source, &MemSource, sizeof(MemSource));
}


static void MFX_FlashUnit_Handle(uint32_t UnitTicks, uint32_t UnitTime, uint32_t FreeTime, uint32_t Period, SidusPro_Pwm_Type* SourcePwm, SidusPro_Pwm_Type* OutPwm)
{
    //uint32_t Period = MFX_Frq2Period(Frq);
    if(UnitTicks < UnitTime)
    {
        if((UnitTicks % Period) < (Period / 2))//亮灯期间
        {
            memcpy(OutPwm, SourcePwm, sizeof(SidusPro_Pwm_Type));
        }
        else//灭灯期间
        {
            memset(OutPwm, 0, sizeof(SidusPro_Pwm_Type));
        }
    }
    else  //Free Time
    {
        memset(OutPwm, 0, sizeof(SidusPro_Pwm_Type));
    }

}


static float MFX_Continue_Ratio_Get(uint32_t FadeInTime, uint8_t FadeInCurve, uint32_t FadeOutTime, uint8_t FadeOutCurve, uint32_t CycleTime, uint32_t CurTicks)
{
    uint32_t ContinueTime = (CycleTime > FadeInTime + FadeOutTime) ? CycleTime - FadeInTime - FadeOutTime : 0;
    float Ratio = 0.0;
    if(CurTicks < CycleTime)
    {
        if(CurTicks < FadeInTime)
        {
            Ratio = Curvilinear_Function(1000 * CurTicks / FadeInTime, FadeInCurve) / 10000.0f;
        }
        else
        {
            if(CurTicks < FadeInTime + ContinueTime)
            {
                Ratio = 1.0;
            }
            else
            {
                Ratio = Curvilinear_Function(1000 * (CycleTime - CurTicks) / FadeOutTime, FadeOutCurve) / 10000.0f;
            }
        }
    }
    else  //Free Time
    {
        Ratio = 0.0;
    }
    return Ratio;
}

static uint32_t MFX_Overlap_Time_Start_Get(uint16_t UnitIndex, uint16_t* pUnitTimeTab, uint16_t* pFreeTimeTab)
{
    uint32_t TimeStart = 0;
    uint16_t i = 0;
    UnitIndex = UnitIndex > 1000 ? 1000 : UnitIndex;
    for(i = 0; i < UnitIndex; i++)
    {
        TimeStart += *(pUnitTimeTab + i) * 100;
        TimeStart += *(pFreeTimeTab + i) * 100;
    }
    return TimeStart;
}


static void MFX_Running_Task(void)
{
    static SFX_Source_Type SourceLine1 = {0};//时间线1
    static SFX_Source_Type SourceLine2 = {0};//时间线2，用来重合
    static SidusPro_Pwm_Type SourcePwm = {0};
    static uint32_t UnitNum = 1;//CycleTime时间内单元数量
    static uint32_t UnitCnt = 0;//运行过程中单元计数
    static uint8_t LoopTimesSum = 1;//总循环次数，一个CycleTime为一个循环次数
    static uint8_t LoopTimesCnt = 0;//循环次数计数
    static uint32_t UnitTicks = 0;
    static uint8_t FlashFrq = 0;
    static uint32_t CycleTicks = 0;
    static uint32_t CycleTime = 0;
    static uint32_t UnitTime = 0;
    static uint32_t FreeTime = 0;
    static uint32_t FadeInTime = 0;
    static uint32_t FadeOutTime = 0;
    static uint32_t UnitTime_OL = 0;
    static uint32_t FadeInTime_OL = 0;
    static uint32_t FadeOutTime_OL = 0;
    static uint32_t OverlapTime_Start = 0;
    static uint32_t Overlap_Offset = 0;
    static SidusPro_Pwm_Type PwmLine1 = {0};
    static SidusPro_Pwm_Type PwmLine2 = {0};
    static SidusPro_Pwm_Type FinalPwm = {0};
    static uint8_t OLR = 0;
    static uint8_t OLP = 0;
    static uint8_t IsOverLapNeed = 0;
    static uint8_t Flag_Flash_New_Unit = 0;

    static float Ratio = 0.0;
    static float Ratio_OL = 0.0;
    if(Flag_SidusProFX_Arg_Init)
    {
        Flag_SidusProFX_Arg_Init = 0;
        CycleTicks = 0;
        LoopTimesCnt = 0;//当前循环次数
        UnitCnt = 0;
        UnitTicks = 0;
        switch(pSFX_Arg->SFX_Arg.MFX.Mode)
        {
            case MFX_MODE_FLASH:                        //大于100为随机
                LoopTimesSum = (pSFX_Arg->SFX_Arg.MFX.ModeArg.Flash.LoopTimes > 100) ? (SidusPro_Get_RandomRange(1,
                               100)) : pSFX_Arg->SFX_Arg.MFX.ModeArg.Flash.LoopTimes;
                CycleTime = MFX_CycleTime_Calculate(pSFX_Arg->SFX_Arg.MFX.ModeArg.Flash.CycleTime_Mini, pSFX_Arg->SFX_Arg.MFX.ModeArg.Flash.CycleTime_Max,
                                                    pSFX_Arg->SFX_Arg.MFX.ModeArg.Flash.CycleTime_Seq, LoopTimesCnt, LoopTimesSum);
                UnitNum = MFX_UnitNum_Calculate(pSFX_Arg->SFX_Arg.MFX.ModeArg.Flash.UnitTime_Mini, pSFX_Arg->SFX_Arg.MFX.ModeArg.Flash.UnitTime_Max,
                                                pSFX_Arg->SFX_Arg.MFX.ModeArg.Flash.UnitTime_Seq,
                                                pSFX_Arg->SFX_Arg.MFX.ModeArg.Flash.FreeTime_Mini, pSFX_Arg->SFX_Arg.MFX.ModeArg.Flash.FreeTime_Max, pSFX_Arg->SFX_Arg.MFX.ModeArg.Flash.FreeTime_Seq,
                                                CycleTime);
                for(uint16_t i = 0; i < UnitNum; i++)
                {
                    UnitTimeTab[i] = MFX_UnitTime_Calculate(pSFX_Arg->SFX_Arg.MFX.ModeArg.Flash.UnitTime_Mini / 100,
                                                            pSFX_Arg->SFX_Arg.MFX.ModeArg.Flash.UnitTime_Max / 100,
                                                            pSFX_Arg->SFX_Arg.MFX.ModeArg.Flash.UnitTime_Seq, i, UnitNum);
                    FreeTimeTab[i] = MFX_UnitTime_Calculate(pSFX_Arg->SFX_Arg.MFX.ModeArg.Flash.FreeTime_Mini / 100,
                                                            pSFX_Arg->SFX_Arg.MFX.ModeArg.Flash.FreeTime_Max / 100,
                                                            pSFX_Arg->SFX_Arg.MFX.ModeArg.Flash.FreeTime_Seq, i, UnitNum);
                }
                if(pSFX_Arg->SFX_Arg.MFX.ModeArg.Flash.UnitTime_Seq == 3)
                    MFX_TimeTab_Break(UnitTimeTab, UnitNum);//打乱数组
                if(pSFX_Arg->SFX_Arg.MFX.ModeArg.Flash.FreeTime_Seq == 3)
                    MFX_TimeTab_Break(FreeTimeTab, UnitNum);//打乱数组
                UnitTime = 100 * UnitTimeTab[UnitCnt];
                FreeTime = 100 * FreeTimeTab[UnitCnt];
                FlashFrq = MFX_UnitFrq_Calculate(pSFX_Arg->SFX_Arg.MFX.ModeArg.Flash.Frq_Mini, pSFX_Arg->SFX_Arg.MFX.ModeArg.Flash.Frq_Max,
                                                 pSFX_Arg->SFX_Arg.MFX.ModeArg.Flash.Frq_Seq, UnitCnt, UnitNum);
                Flag_Flash_New_Unit = 1;
                break;
            case MFX_MODE_CONTINUE:
                LoopTimesSum = (pSFX_Arg->SFX_Arg.MFX.ModeArg.Continue.LoopTimes > 100) ? (SidusPro_Get_RandomRange(1,
                               100)) : pSFX_Arg->SFX_Arg.MFX.ModeArg.Continue.LoopTimes;
                CycleTime = MFX_CycleTime_Calculate(pSFX_Arg->SFX_Arg.MFX.ModeArg.Continue.CycleTime_Mini, pSFX_Arg->SFX_Arg.MFX.ModeArg.Continue.CycleTime_Max,
                                                    pSFX_Arg->SFX_Arg.MFX.ModeArg.Continue.CycleTime_Seq, LoopTimesCnt, LoopTimesSum);
                FadeInTime = MFX_CycleTime_Calculate(pSFX_Arg->SFX_Arg.MFX.ModeArg.Continue.FadeInTime_Mini, pSFX_Arg->SFX_Arg.MFX.ModeArg.Continue.FadeInTime_Max,
                                                     pSFX_Arg->SFX_Arg.MFX.ModeArg.Continue.FadeInTime_Seq, LoopTimesCnt, LoopTimesSum);
                FadeOutTime = MFX_CycleTime_Calculate(pSFX_Arg->SFX_Arg.MFX.ModeArg.Continue.FadeOutTime_Mini, pSFX_Arg->SFX_Arg.MFX.ModeArg.Continue.FadeOutTime_Max,
                                                      pSFX_Arg->SFX_Arg.MFX.ModeArg.Continue.FadeOutTime_Seq, LoopTimesCnt, LoopTimesSum);
                break;
            case MFX_MODE_PARAGRAPH:
                LoopTimesSum = (pSFX_Arg->SFX_Arg.MFX.ModeArg.Paragraph.LoopTimes > 100) ? (SidusPro_Get_RandomRange(1,
                               100)) : pSFX_Arg->SFX_Arg.MFX.ModeArg.Paragraph.LoopTimes;
                CycleTime = MFX_CycleTime_Calculate(pSFX_Arg->SFX_Arg.MFX.ModeArg.Paragraph.CycleTime_Mini, pSFX_Arg->SFX_Arg.MFX.ModeArg.Paragraph.CycleTime_Max,
                                                    pSFX_Arg->SFX_Arg.MFX.ModeArg.Paragraph.CycleTime_Seq, LoopTimesCnt, LoopTimesSum);
                UnitNum = MFX_UnitNum_Calculate(pSFX_Arg->SFX_Arg.MFX.ModeArg.Paragraph.UnitTime_Mini, pSFX_Arg->SFX_Arg.MFX.ModeArg.Paragraph.UnitTime_Max,
                                                pSFX_Arg->SFX_Arg.MFX.ModeArg.Paragraph.UnitTime_Seq, pSFX_Arg->SFX_Arg.MFX.ModeArg.Paragraph.FreeTime_Mini, 
                                                pSFX_Arg->SFX_Arg.MFX.ModeArg.Paragraph.FreeTime_Max, pSFX_Arg->SFX_Arg.MFX.ModeArg.Paragraph.FreeTime_Seq, CycleTime);
                for(uint16_t i = 0; i < UnitNum; i++)
                {
                    UnitTimeTab[i] = MFX_UnitTime_Calculate(pSFX_Arg->SFX_Arg.MFX.ModeArg.Paragraph.UnitTime_Mini / 100, pSFX_Arg->SFX_Arg.MFX.ModeArg.Paragraph.UnitTime_Max / 100, pSFX_Arg->SFX_Arg.MFX.ModeArg.Paragraph.UnitTime_Seq, i, UnitNum);
                    FreeTimeTab[i] = MFX_UnitTime_Calculate(pSFX_Arg->SFX_Arg.MFX.ModeArg.Paragraph.FreeTime_Mini / 100, pSFX_Arg->SFX_Arg.MFX.ModeArg.Paragraph.FreeTime_Max / 100, pSFX_Arg->SFX_Arg.MFX.ModeArg.Paragraph.FreeTime_Seq, i, UnitNum);
                    FadeInTimeTab[i] = MFX_UnitTime_Calculate(pSFX_Arg->SFX_Arg.MFX.ModeArg.Paragraph.FadeInTime_Mini / 100, pSFX_Arg->SFX_Arg.MFX.ModeArg.Paragraph.FadeInTime_Max / 100, pSFX_Arg->SFX_Arg.MFX.ModeArg.Paragraph.FadeInTime_Seq, i, UnitNum);
                    FadeOutTimeTab[i] = MFX_UnitTime_Calculate(pSFX_Arg->SFX_Arg.MFX.ModeArg.Paragraph.FadeOutTime_Mini / 100, pSFX_Arg->SFX_Arg.MFX.ModeArg.Paragraph.FadeOutTime_Max / 100, pSFX_Arg->SFX_Arg.MFX.ModeArg.Paragraph.FadeOutTime_Seq, i, UnitNum);
                    OverLapWith[i] = MFX_Unit_Overlap_With_Get(pSFX_Arg->SFX_Arg.MFX.ModeArg.Paragraph.Overlap_Seq, i, UnitNum);
                }
                if(pSFX_Arg->SFX_Arg.MFX.ModeArg.Paragraph.UnitTime_Seq == 3)
                    MFX_TimeTab_Break(UnitTimeTab, UnitNum);//打乱数组
                if(pSFX_Arg->SFX_Arg.MFX.ModeArg.Paragraph.FreeTime_Seq == 3)
                    MFX_TimeTab_Break(FreeTimeTab, UnitNum);//打乱数组
                if(pSFX_Arg->SFX_Arg.MFX.ModeArg.Paragraph.FadeInTime_Seq == 3)
                    MFX_TimeTab_Break(FadeInTimeTab, UnitNum);//打乱数组
                if(pSFX_Arg->SFX_Arg.MFX.ModeArg.Paragraph.FadeOutTime_Seq == 3)
                    MFX_TimeTab_Break(FadeOutTimeTab, UnitNum);//打乱数组
                if(pSFX_Arg->SFX_Arg.MFX.ModeArg.Paragraph.Overlap_Seq == 3)
                    MFX_TimeTab_Break(OverLapWith, UnitNum);//打乱数组

                UnitTime = UnitTimeTab[UnitCnt] * 100;
                FreeTime = FreeTimeTab[UnitCnt] * 100;
                FadeInTime = FadeInTimeTab[UnitCnt] * 100;
                FadeOutTime = FadeOutTimeTab[UnitCnt] * 100;
                UnitTime_OL = UnitTimeTab[OverLapWith[UnitCnt]] * 100;
                FadeInTime_OL = FadeInTimeTab[OverLapWith[UnitCnt]] * 100;
                FadeOutTime_OL = FadeOutTimeTab[OverLapWith[UnitCnt]] * 100;
                OverlapTime_Start = MFX_Overlap_Time_Start_Get(OverLapWith[UnitCnt], UnitTimeTab, FreeTimeTab);
                OLR = MFX_UnitFrq_Calculate(pSFX_Arg->SFX_Arg.MFX.ModeArg.Paragraph.OLR_Mini, pSFX_Arg->SFX_Arg.MFX.ModeArg.Paragraph.OLR_Max,
                                            pSFX_Arg->SFX_Arg.MFX.ModeArg.Paragraph.OLR_Seq, UnitCnt, UnitNum);
                OLP = MFX_UnitFrq_Calculate(pSFX_Arg->SFX_Arg.MFX.ModeArg.Paragraph.OLP_Mini, pSFX_Arg->SFX_Arg.MFX.ModeArg.Paragraph.OLP_Max,
                                            pSFX_Arg->SFX_Arg.MFX.ModeArg.Paragraph.OLP_Seq, UnitCnt, UnitNum);
                IsOverLapNeed = OLP < SidusPro_Get_RandomRange(1, 100) ? 0 : 1;
            default:
                break;
        }
    }

    while(!SidusPro_Queue_Is_Full(SidusPro_Queue_PWM))
    {
        if(MFX_CTRL_PAUSE == pSFX_Arg->SFX_Arg.MFX.Ctrl || MFX_CTRL_STOP == pSFX_Arg->SFX_Arg.MFX.Ctrl)
            break;//停止、暂停状态添加FIFO
        switch(pSFX_Arg->SFX_Arg.MFX.Mode)
        {
            case MFX_MODE_FLASH:
                if(LoopTimesCnt < LoopTimesSum)
                {
                    if(UnitCnt < UnitNum)
                    {
                        MFX_SourceBase_Chase(&pSFX_Arg->SFX_Arg.MFX.BaseArg, &SourceLine1, CycleTicks, CycleTime);
                        if(Flag_Flash_New_Unit)//单元时间内颜色相同，新单元切换颜色
                        {
                            Flag_Flash_New_Unit = 0;
                            SidusProFX_Source2Pwm(&SourceLine1, &SourcePwm);
                        }
                        MFX_FlashUnit_Handle(UnitTicks, UnitTime, FreeTime, MFX_Frq2Period(FlashFrq), &SourcePwm, &FinalPwm);
                        if(++UnitTicks >= UnitTime + FreeTime)//单元结束
                        {
                            Flag_Flash_New_Unit = 1;
                            UnitTicks = 0;
                            UnitCnt++;
                            UnitTime = 100 * UnitTimeTab[UnitCnt];
                            FreeTime = 100 * FreeTimeTab[UnitCnt];
                            FlashFrq = MFX_UnitFrq_Calculate(pSFX_Arg->SFX_Arg.MFX.ModeArg.Flash.Frq_Mini, pSFX_Arg->SFX_Arg.MFX.ModeArg.Flash.Frq_Max,
                                                             pSFX_Arg->SFX_Arg.MFX.ModeArg.Flash.Frq_Seq, UnitCnt, UnitNum);
                        }
                    }
                    if(++CycleTicks >= CycleTime)
                    {
                        CycleTicks = 0;
                        UnitCnt = 0;
                        UnitTicks = 0;
                        LoopTimesCnt++;
                        Flag_Flash_New_Unit = 1;
                        if(LoopTimesCnt >= LoopTimesSum)
                        {
                            if(pSFX_Arg->SFX_Arg.MFX.ModeArg.Flash.LoopMode == 1)
                            {
                                LoopTimesCnt = 0;//重新开始循环
                            }
                            else
                            {
                                memset(&FinalPwm, 0, sizeof(FinalPwm));
                            }
                        }
                        CycleTime = MFX_CycleTime_Calculate(pSFX_Arg->SFX_Arg.MFX.ModeArg.Flash.CycleTime_Mini, pSFX_Arg->SFX_Arg.MFX.ModeArg.Flash.CycleTime_Max,
                                                            pSFX_Arg->SFX_Arg.MFX.ModeArg.Flash.CycleTime_Seq, LoopTimesCnt, LoopTimesSum);
                        UnitNum = MFX_UnitNum_Calculate(pSFX_Arg->SFX_Arg.MFX.ModeArg.Flash.UnitTime_Mini, pSFX_Arg->SFX_Arg.MFX.ModeArg.Flash.UnitTime_Max,
                                                        pSFX_Arg->SFX_Arg.MFX.ModeArg.Flash.UnitTime_Seq, pSFX_Arg->SFX_Arg.MFX.ModeArg.Flash.FreeTime_Mini, 
                                                        pSFX_Arg->SFX_Arg.MFX.ModeArg.Flash.FreeTime_Max, pSFX_Arg->SFX_Arg.MFX.ModeArg.Flash.FreeTime_Seq, CycleTime);
                        for(uint16_t i = 0; i < UnitNum; i++)
                        {
                            UnitTimeTab[i] = MFX_UnitTime_Calculate(pSFX_Arg->SFX_Arg.MFX.ModeArg.Flash.UnitTime_Mini / 100,
                                                                    pSFX_Arg->SFX_Arg.MFX.ModeArg.Flash.UnitTime_Max / 100,
                                                                    pSFX_Arg->SFX_Arg.MFX.ModeArg.Flash.UnitTime_Seq, i, UnitNum);
                            FreeTimeTab[i] = MFX_UnitTime_Calculate(pSFX_Arg->SFX_Arg.MFX.ModeArg.Flash.FreeTime_Mini / 100,
                                                                    pSFX_Arg->SFX_Arg.MFX.ModeArg.Flash.FreeTime_Max / 100,
                                                                    pSFX_Arg->SFX_Arg.MFX.ModeArg.Flash.FreeTime_Seq, i, UnitNum);
                        }
                        if(pSFX_Arg->SFX_Arg.MFX.ModeArg.Flash.UnitTime_Seq == 3)
                            MFX_TimeTab_Break(UnitTimeTab, UnitNum);//打乱数组
                        if(pSFX_Arg->SFX_Arg.MFX.ModeArg.Flash.FreeTime_Seq == 3)
                            MFX_TimeTab_Break(FreeTimeTab, UnitNum);//打乱数组
                        UnitTime = 100 * UnitTimeTab[UnitCnt];
                        FreeTime = 100 * FreeTimeTab[UnitCnt];
                        FlashFrq = MFX_UnitFrq_Calculate(pSFX_Arg->SFX_Arg.MFX.ModeArg.Flash.Frq_Mini, pSFX_Arg->SFX_Arg.MFX.ModeArg.Flash.Frq_Max,
                                                         pSFX_Arg->SFX_Arg.MFX.ModeArg.Flash.Frq_Seq, UnitCnt, UnitNum);
                    }
                }
                break;
            case MFX_MODE_CONTINUE:
                if(LoopTimesCnt < LoopTimesSum)
                {
                    Ratio = MFX_Continue_Ratio_Get(FadeInTime, pSFX_Arg->SFX_Arg.MFX.ModeArg.Continue.FadeIn_Curve, FadeOutTime,
                                                   pSFX_Arg->SFX_Arg.MFX.ModeArg.Continue.FadeOut_Curve, CycleTime, CycleTicks);
                    MFX_SourceBase_Chase(&pSFX_Arg->SFX_Arg.MFX.BaseArg, &SourceLine1, CycleTicks, CycleTime);
                    SourceLine1.CCT.Int = SourceLine1.CCT.Int * Ratio;
                    if(0 == CycleTicks % MFX_Frq2Period(pSFX_Arg->SFX_Arg.MFX.ModeArg.Continue.Flicker_Frq))
                    {
                        SidusProFX_Source2Pwm(&SourceLine1, &FinalPwm);
                    }
                    if(++CycleTicks >= CycleTime)
                    {
                        CycleTicks = 0;
                        LoopTimesCnt++;
                        if(LoopTimesCnt >= LoopTimesSum)
                        {
                            if(pSFX_Arg->SFX_Arg.MFX.ModeArg.Continue.LoopMode == 1)
                            {
                                LoopTimesCnt = 0;//重新开始循环
                            }
                            else
                            {
                                memset(&FinalPwm, 0, sizeof(FinalPwm));
                            }
                        }
                        CycleTime = MFX_CycleTime_Calculate(pSFX_Arg->SFX_Arg.MFX.ModeArg.Continue.CycleTime_Mini, pSFX_Arg->SFX_Arg.MFX.ModeArg.Continue.CycleTime_Max,
                                                            pSFX_Arg->SFX_Arg.MFX.ModeArg.Continue.CycleTime_Seq, LoopTimesCnt, LoopTimesSum);
                        FadeInTime = MFX_CycleTime_Calculate(pSFX_Arg->SFX_Arg.MFX.ModeArg.Continue.FadeInTime_Mini, pSFX_Arg->SFX_Arg.MFX.ModeArg.Continue.FadeInTime_Max,
                                                             pSFX_Arg->SFX_Arg.MFX.ModeArg.Continue.FadeInTime_Seq, LoopTimesCnt, LoopTimesSum);
                        FadeOutTime = MFX_CycleTime_Calculate(pSFX_Arg->SFX_Arg.MFX.ModeArg.Continue.FadeOutTime_Mini, pSFX_Arg->SFX_Arg.MFX.ModeArg.Continue.FadeOutTime_Max,
                                                              pSFX_Arg->SFX_Arg.MFX.ModeArg.Continue.FadeOutTime_Seq, LoopTimesCnt, LoopTimesSum);
                    }
                }
                break;
            case MFX_MODE_PARAGRAPH:
                if(LoopTimesCnt < LoopTimesSum)
                {
                    if(UnitCnt < UnitNum)
                    {
                        Ratio = MFX_Continue_Ratio_Get(FadeInTime, pSFX_Arg->SFX_Arg.MFX.ModeArg.Paragraph.FadeIn_Curve, FadeOutTime,
                                                       pSFX_Arg->SFX_Arg.MFX.ModeArg.Paragraph.FadeOut_Curve, UnitTime, UnitTicks);
                        MFX_SourceBase_Chase(&pSFX_Arg->SFX_Arg.MFX.BaseArg, &SourceLine1, CycleTicks, CycleTime);
                        SourceLine1.CCT.Int = SourceLine1.CCT.Int * Ratio;
                        if(IsOverLapNeed && UnitTicks > (100 - OLR) * UnitTime / 100)
                        {
                            Overlap_Offset = UnitTicks - (100 - OLR) * UnitTime / 100;
                            MFX_SourceBase_Chase(&pSFX_Arg->SFX_Arg.MFX.BaseArg, &SourceLine2, OverlapTime_Start + Overlap_Offset, CycleTime);
                            Ratio_OL = MFX_Continue_Ratio_Get(FadeInTime_OL, pSFX_Arg->SFX_Arg.MFX.ModeArg.Paragraph.FadeIn_Curve, FadeOutTime_OL,
                                                              pSFX_Arg->SFX_Arg.MFX.ModeArg.Paragraph.FadeOut_Curve, UnitTime_OL, Overlap_Offset);
                            SourceLine2.CCT.Int = SourceLine2.CCT.Int * Ratio_OL;
                        }
                        else
                        {
                            SourceLine2.CCT.Int = 0;
                        }
                        if(SourceLine1.CCT.Int + SourceLine2.CCT.Int > 1000)
                        {
                            SourceLine1.CCT.Int = 1000 * SourceLine1.CCT.Int / (SourceLine1.CCT.Int + SourceLine2.CCT.Int);
                            SourceLine2.CCT.Int = 1000 * SourceLine2.CCT.Int / (SourceLine1.CCT.Int + SourceLine2.CCT.Int);
                        }
                        if(0 == CycleTicks % MFX_Frq2Period(pSFX_Arg->SFX_Arg.MFX.ModeArg.Paragraph.Flicker_Frq))
                        {
                            SidusProFX_Source2Pwm(&SourceLine1, &PwmLine1);
                            SidusProFX_Source2Pwm(&SourceLine2, &PwmLine2);
                        }
                        for(uint32_t i = 0; i < SIDUSPRO_PWM_CH_NUM; i++)
                        {
                            FinalPwm.pwm[i] = PwmLine1.pwm[i] + PwmLine2.pwm[i];
                        }
                        if(++UnitTicks >= UnitTime + FreeTime)//单元结束
                        {
                            UnitTicks = 0;
                            UnitCnt++;
                            UnitTime = UnitTimeTab[UnitCnt] * 100;
                            FreeTime = FreeTimeTab[UnitCnt] * 100;
                            FadeInTime = FadeInTimeTab[UnitCnt] * 100;
                            FadeOutTime = FadeOutTimeTab[UnitCnt] * 100;
                            UnitTime_OL = UnitTimeTab[OverLapWith[UnitCnt]] * 100;
                            FadeInTime_OL = FadeInTimeTab[OverLapWith[UnitCnt]] * 100;
                            FadeOutTime_OL = FadeOutTimeTab[OverLapWith[UnitCnt]] * 100;
                            OverlapTime_Start = MFX_Overlap_Time_Start_Get(OverLapWith[UnitCnt], UnitTimeTab, FreeTimeTab);
                            OLR = MFX_UnitFrq_Calculate(pSFX_Arg->SFX_Arg.MFX.ModeArg.Paragraph.OLR_Mini, pSFX_Arg->SFX_Arg.MFX.ModeArg.Paragraph.OLR_Max,
                                                        pSFX_Arg->SFX_Arg.MFX.ModeArg.Paragraph.OLR_Seq, UnitCnt, UnitNum);
                            OLP = MFX_UnitFrq_Calculate(pSFX_Arg->SFX_Arg.MFX.ModeArg.Paragraph.OLP_Mini, pSFX_Arg->SFX_Arg.MFX.ModeArg.Paragraph.OLP_Max,
                                                        pSFX_Arg->SFX_Arg.MFX.ModeArg.Paragraph.OLP_Seq, UnitCnt, UnitNum);
                            IsOverLapNeed = OLP < SidusPro_Get_RandomRange(1, 100) ? 0 : 1;
                        }
                    }
                    if(++CycleTicks >= CycleTime)
                    {
                        CycleTicks = 0;
                        UnitCnt = 0;
                        UnitTicks = 0;
                        LoopTimesCnt++;
                        if(LoopTimesCnt >= LoopTimesSum)
                        {
                            if(pSFX_Arg->SFX_Arg.MFX.ModeArg.Paragraph.LoopMode == 1)
                            {
                                LoopTimesCnt = 0;//重新开始循环
                            }
                            else
                            {
                                memset(&FinalPwm, 0, sizeof(FinalPwm));
                            }
                        }
                        CycleTime = MFX_CycleTime_Calculate(pSFX_Arg->SFX_Arg.MFX.ModeArg.Paragraph.CycleTime_Mini, pSFX_Arg->SFX_Arg.MFX.ModeArg.Paragraph.CycleTime_Max,
                                                            pSFX_Arg->SFX_Arg.MFX.ModeArg.Paragraph.CycleTime_Seq, LoopTimesCnt, LoopTimesSum);
                        UnitNum = MFX_UnitNum_Calculate(pSFX_Arg->SFX_Arg.MFX.ModeArg.Paragraph.UnitTime_Mini, pSFX_Arg->SFX_Arg.MFX.ModeArg.Paragraph.UnitTime_Max,
                                                        pSFX_Arg->SFX_Arg.MFX.ModeArg.Paragraph.UnitTime_Seq, pSFX_Arg->SFX_Arg.MFX.ModeArg.Paragraph.FreeTime_Mini, 
                                                        pSFX_Arg->SFX_Arg.MFX.ModeArg.Paragraph.FreeTime_Max, pSFX_Arg->SFX_Arg.MFX.ModeArg.Paragraph.FreeTime_Seq, CycleTime);
                        for(uint16_t i = 0; i < UnitNum; i++)
                        {
                            UnitTimeTab[i] = MFX_UnitTime_Calculate(pSFX_Arg->SFX_Arg.MFX.ModeArg.Paragraph.UnitTime_Mini / 100,
                                                                    pSFX_Arg->SFX_Arg.MFX.ModeArg.Paragraph.UnitTime_Max / 100,
                                                                    pSFX_Arg->SFX_Arg.MFX.ModeArg.Paragraph.UnitTime_Seq, i, UnitNum);
                            FreeTimeTab[i] = MFX_UnitTime_Calculate(pSFX_Arg->SFX_Arg.MFX.ModeArg.Paragraph.FreeTime_Mini / 100,
                                                                    pSFX_Arg->SFX_Arg.MFX.ModeArg.Paragraph.FreeTime_Max / 100,
                                                                    pSFX_Arg->SFX_Arg.MFX.ModeArg.Paragraph.FreeTime_Seq, i, UnitNum);
                            FadeInTimeTab[i] = MFX_UnitTime_Calculate(pSFX_Arg->SFX_Arg.MFX.ModeArg.Paragraph.FadeInTime_Mini / 100,
                                               pSFX_Arg->SFX_Arg.MFX.ModeArg.Paragraph.FadeInTime_Max / 100,
                                               pSFX_Arg->SFX_Arg.MFX.ModeArg.Paragraph.FadeInTime_Seq, i, UnitNum);
                            FadeOutTimeTab[i] = MFX_UnitTime_Calculate(pSFX_Arg->SFX_Arg.MFX.ModeArg.Paragraph.FadeOutTime_Mini / 100,
                                                pSFX_Arg->SFX_Arg.MFX.ModeArg.Paragraph.FadeOutTime_Max / 100,
                                                pSFX_Arg->SFX_Arg.MFX.ModeArg.Paragraph.FadeOutTime_Seq, i, UnitNum);
                        }
                        if(pSFX_Arg->SFX_Arg.MFX.ModeArg.Paragraph.UnitTime_Seq == 3)
                            MFX_TimeTab_Break(UnitTimeTab, UnitNum);//打乱数组
                        if(pSFX_Arg->SFX_Arg.MFX.ModeArg.Paragraph.FreeTime_Seq == 3)
                            MFX_TimeTab_Break(FreeTimeTab, UnitNum);//打乱数组
                        if(pSFX_Arg->SFX_Arg.MFX.ModeArg.Paragraph.FadeInTime_Seq == 3)
                            MFX_TimeTab_Break(FadeInTimeTab, UnitNum);//打乱数组
                        if(pSFX_Arg->SFX_Arg.MFX.ModeArg.Paragraph.FadeOutTime_Seq == 3)
                            MFX_TimeTab_Break(FadeOutTimeTab, UnitNum);//打乱数组

                        UnitTime = UnitTimeTab[UnitCnt] * 100;
                        FreeTime = FreeTimeTab[UnitCnt] * 100;
                        FadeInTime = FadeInTimeTab[UnitCnt] * 100;
                        FadeOutTime = FadeOutTimeTab[UnitCnt] * 100;
                        UnitTime_OL = UnitTimeTab[UnitCnt + 1 >= UnitNum ? 0 : UnitCnt + 1] * 100;
                        FadeInTime_OL = FadeInTimeTab[UnitCnt + 1 >= UnitNum ? 0 : UnitCnt + 1] * 100;
                        FadeOutTime_OL = FadeOutTimeTab[UnitCnt + 1 >= UnitNum ? 0 : UnitCnt + 1] * 100;
                        OLR = MFX_UnitFrq_Calculate(pSFX_Arg->SFX_Arg.MFX.ModeArg.Paragraph.OLR_Mini, pSFX_Arg->SFX_Arg.MFX.ModeArg.Paragraph.OLR_Max,
                                                    pSFX_Arg->SFX_Arg.MFX.ModeArg.Paragraph.OLR_Seq, UnitCnt, UnitNum);
                        OLP = MFX_UnitFrq_Calculate(pSFX_Arg->SFX_Arg.MFX.ModeArg.Paragraph.OLP_Mini, pSFX_Arg->SFX_Arg.MFX.ModeArg.Paragraph.OLP_Max,
                                                    pSFX_Arg->SFX_Arg.MFX.ModeArg.Paragraph.OLP_Seq, UnitCnt, UnitNum);
                        IsOverLapNeed = OLP < SidusPro_Get_RandomRange(1, 100) ? 0 : 1;
                    }
                }
                break;
            default:
                break;
        }

        SidusPro_Queue_Put(SidusPro_Queue_PWM, &FinalPwm);
    }
}


static uint32_t PFX_Flash_Frq2Period(uint8_t Frq)
{
    uint32_t Period = 100;//10Hz 100Ms周期
    if(Frq > 18)
    {
        Period = 100;//10Hz 100Ms周期
    }
    if(Frq >= 9)
    {
        Period = 1000.0f / (2 + Frq - 10);
    }
    else
    {
        Period = 1000.0f * 10.0f / (1.0f + Frq);
    }
    return Period;
}

static void PFX_Running_Task(void)
{
    static SFX_Source_Type Source = {0};
    static SFX_Source_Type PreSource = {0};
    static SidusPro_Pwm_Type SourcePwm = {0};
    static SidusPro_Pwm_Type FinalPwm = {0};
    static uint16_t FlashTimesCnt = 0;
    static uint32_t FlashPeriod = 0;
    static uint32_t PFX_Ticks = 0;
    static uint32_t ContinueTime = 0;
    static uint32_t FadeInTime = 0;
    static uint32_t FadeOutTime = 0;
    static float Ratio = 0;
    static MFX_Base_Type Base = {0};//continue和chase调用MFX的实现方法
    if(Flag_SidusProFX_Arg_Init)
    {
        Flag_SidusProFX_Arg_Init = 0;
        PFX_Ticks = 0;
        memset(&PreSource, 0, sizeof(PreSource));
        switch(pSFX_Arg->SFX_Arg.PFX.Mode)
        {
            case PFX_Mode_Flash:
                FlashTimesCnt = 0;
                FlashPeriod = PFX_Flash_Frq2Period(pSFX_Arg->SFX_Arg.PFX.ModeArg.Flash.BaseCCT.Frq);
                Source.CCT.Base = pSFX_Arg->SFX_Arg.PFX.ModeArg.Flash.BaseCCT.Base;
                Source.CCT.Int = pSFX_Arg->SFX_Arg.PFX.ModeArg.Flash.BaseCCT.Int;//共用体HSI位置相同
                Source.CCT.CCT = pSFX_Arg->SFX_Arg.PFX.ModeArg.Flash.BaseCCT.CCT;
                Source.CCT.GM = pSFX_Arg->SFX_Arg.PFX.ModeArg.Flash.BaseCCT.GM;
                SidusProFX_Source2Pwm(&Source, &SourcePwm);
                break;
            case PFX_Mode_Continue:
                ContinueTime = pSFX_Arg->SFX_Arg.PFX.ModeArg.Continue.BaseCCT.ContinueTime;
                FadeOutTime = pSFX_Arg->SFX_Arg.PFX.ModeArg.Continue.BaseCCT.FadeOutTime;
                FadeInTime = pSFX_Arg->SFX_Arg.PFX.ModeArg.Continue.BaseCCT.FadeInTime;
                
                Base.BaseCCT.Base = pSFX_Arg->SFX_Arg.PFX.ModeArg.Continue.BaseCCT.Base;
                Base.BaseCCT.Int_Max = pSFX_Arg->SFX_Arg.PFX.ModeArg.Continue.BaseCCT.Int;//HSI位置相同
                Base.BaseCCT.Int_Mini = pSFX_Arg->SFX_Arg.PFX.ModeArg.Continue.BaseCCT.Int;
                Base.BaseCCT.Int_Seq = 0;
                Base.BaseCCT.CCT_Max = pSFX_Arg->SFX_Arg.PFX.ModeArg.Continue.BaseCCT.CCT;
                Base.BaseCCT.CCT_Mini = pSFX_Arg->SFX_Arg.PFX.ModeArg.Continue.BaseCCT.CCT;
                Base.BaseCCT.CCT_Seq = 0;
                Base.BaseCCT.GM_Max = pSFX_Arg->SFX_Arg.PFX.ModeArg.Continue.BaseCCT.GM;
                Base.BaseCCT.GM_Mini = pSFX_Arg->SFX_Arg.PFX.ModeArg.Continue.BaseCCT.GM;
                Base.BaseCCT.GM_Seq = 0;
                break;
            case PFX_Mode_Chase:
                ContinueTime = pSFX_Arg->SFX_Arg.PFX.ModeArg.Chase.BaseCCT.Time;
                
                Base.BaseCCT.Base = pSFX_Arg->SFX_Arg.PFX.ModeArg.Chase.BaseCCT.Base;
                Base.BaseCCT.Int_Max = pSFX_Arg->SFX_Arg.PFX.ModeArg.Chase.BaseCCT.Int_Max;//HSI位置相同
                Base.BaseCCT.Int_Mini = pSFX_Arg->SFX_Arg.PFX.ModeArg.Chase.BaseCCT.Int_Mini;
                Base.BaseCCT.Int_Seq = pSFX_Arg->SFX_Arg.PFX.ModeArg.Chase.BaseCCT.Int_Seq;
                Base.BaseCCT.CCT_Max = pSFX_Arg->SFX_Arg.PFX.ModeArg.Chase.BaseCCT.CCT_Max;
                Base.BaseCCT.CCT_Mini = pSFX_Arg->SFX_Arg.PFX.ModeArg.Chase.BaseCCT.CCT_Mini;
                Base.BaseCCT.CCT_Seq = pSFX_Arg->SFX_Arg.PFX.ModeArg.Chase.BaseCCT.CCT_Seq;
                Base.BaseCCT.GM_Max = pSFX_Arg->SFX_Arg.PFX.ModeArg.Chase.BaseCCT.GM_Max;
                Base.BaseCCT.GM_Mini = pSFX_Arg->SFX_Arg.PFX.ModeArg.Chase.BaseCCT.GM_Mini;
                Base.BaseCCT.GM_Seq = pSFX_Arg->SFX_Arg.PFX.ModeArg.Chase.BaseCCT.GM_Seq;
                break;
            default:
                break;
        }
    }
    while(!SidusPro_Queue_Is_Full(SidusPro_Queue_PWM))
    {
        switch(pSFX_Arg->SFX_Arg.PFX.Mode)
        {
            case PFX_Mode_Flash:
                if(PFX_Ticks < FlashPeriod)
                {
                    MFX_FlashUnit_Handle(PFX_Ticks, FlashPeriod, 0, FlashPeriod, &SourcePwm, &FinalPwm);
                    PFX_Ticks++;
                }
                else
                {
                    if(pSFX_Arg->SFX_Arg.PFX.ModeArg.Flash.BaseCCT.Times > 1000)//大于1000为Always
                    {
                        PFX_Ticks = 0;//重新开始Flash
                    }
                    else
                    {
                        if(FlashTimesCnt < pSFX_Arg->SFX_Arg.PFX.ModeArg.Flash.BaseCCT.Times - 1)
                        {
                            PFX_Ticks = 0;//重新开始Flash
                            FlashTimesCnt++;
                        }
                    }
                }
                break;
            case PFX_Mode_Continue:
                Ratio = MFX_Continue_Ratio_Get(FadeInTime, pSFX_Arg->SFX_Arg.PFX.ModeArg.Continue.BaseCCT.FadeInCurve, FadeOutTime,
                                               pSFX_Arg->SFX_Arg.PFX.ModeArg.Continue.BaseCCT.FadeOutCurve,
                                               FadeInTime + ContinueTime + FadeOutTime, PFX_Ticks);
                MFX_SourceBase_Chase(&Base, &Source, PFX_Ticks, FadeInTime + ContinueTime + FadeOutTime);
                Source.CCT.Int = Source.CCT.Int * Ratio;
                SidusProFX_Source2Pwm(&Source, &FinalPwm);
                if(PFX_Ticks < FadeInTime + ContinueTime + FadeOutTime)
                {
                    if(PFX_Ticks < FadeInTime - 1)
                    {
                        PFX_Ticks++;
                    }
                    else
                    {
                        if(ContinueTime < 100 * 1000)
                        {
                            PFX_Ticks++;
                        }
                        else
                        {
                            //always
                        }
                    }
                }
                break;
            case PFX_Mode_Chase:
                MFX_SourceBase_Chase(&Base, &Source, PFX_Ticks, ContinueTime);
                SidusProFX_Source2Pwm(&Source, &FinalPwm);
                if(PFX_Ticks < ContinueTime - 1)
                {
                    PFX_Ticks++;
                }
                else
                {
                    if(pSFX_Arg->SFX_Arg.PFX.ModeArg.Chase.BaseCCT.loop)
                    {
                        PFX_Ticks = 0;
                    }
                    else
                    {
                        PFX_Ticks = ContinueTime;
                    }
                }
                break;
            default:
                break;
        }
        SidusPro_Queue_Put(SidusPro_Queue_PWM, &FinalPwm);
    }
}

static uint16_t CFX_Speed2Preiod(uint8_t Speed)
{
    uint16_t Period = 1;
    Speed = Speed > 5 ? 5 : Speed;
    Period = pow(2, 5 - Speed);
    return Period;
}

static uint8_t CFX_Is_ChaosNeeded(uint8_t ChaosPercent)
{
    return (ChaosPercent >= SidusPro_Get_RandomRange(1, 100)) ? 1 : 0;
}

static void CFX_Running_Task(uint8_t Restart)
{
    static CFX_PwmFileHead_Type* CFX_PwmFile = NULL;
    static SidusPro_Pwm_Type FinalPwm = {0};
    static uint8_t current_dir = 0;//0:正向，1:反向
    static uint16_t FrameNum = 1;
    static uint16_t CurFrame = 1;
    static uint8_t Pre_Ctrl = 0;
    static uint8_t OnceFinish = 0;//单次运行结束标志
    uint16_t ChaosFrame = 1;
    SidusPro_Pwm_Type* pFilePwm = NULL;
    pFilePwm = (SidusPro_Pwm_Type*)(SIDUSPRO_CFX_CACHE_ADDR + sizeof(CFX_PwmFileHead_Type));
    if(Flag_SidusProFX_Arg_Init)
    {
        CFX_PwmFile = (CFX_PwmFileHead_Type*)SIDUSPRO_CFX_CACHE_ADDR;
        Flag_SidusProFX_Arg_Init = 0;
        //pFilePwm = (SidusPro_Pwm_Type*)(SIDUSPRO_CFX_CACHE_ADDR + sizeof(CFX_PwmFileHead_Type));
        CFX_Period = CFX_Speed2Preiod(pSFX_Arg->SFX_Arg.CFX.Speed);
        FrameNum = CFX_PwmFile->FrameNum;
        if(Pre_Ctrl == 0 || OnceFinish || Restart)
        {
            OnceFinish = 0;
            CurFrame = pSFX_Arg->SFX_Arg.CFX.Sequence == 1 ? FrameNum : 1;
            current_dir = pSFX_Arg->SFX_Arg.CFX.Sequence == 1 ? 1 : 0;
        }
        else
        {
            if(pSFX_Arg->SFX_Arg.CFX.Sequence == 1 || pSFX_Arg->SFX_Arg.CFX.Sequence == 0)
            {
                current_dir = pSFX_Arg->SFX_Arg.CFX.Sequence;
            }
        }
        Pre_Ctrl = pSFX_Arg->SFX_Arg.CFX.Ctrl;
    }
    while(!SidusPro_Queue_Is_Full(SidusPro_Queue_PWM) && pSFX_Arg->SFX_Arg.CFX.Ctrl == CFX_CTRL_RUNNING)
    {
        pFilePwm = (SidusPro_Pwm_Type*)(SIDUSPRO_CFX_CACHE_ADDR + sizeof(CFX_PwmFileHead_Type));
        if(0 == current_dir)
        {
            if(CurFrame <= FrameNum)
            {
                if(CFX_Is_ChaosNeeded(pSFX_Arg->SFX_Arg.CFX.Chaos))
                {
                    ChaosFrame = SidusPro_Get_RandomRange(1, CFX_PwmFile->FrameNum);//第一帧到最后一帧
                    pFilePwm += ChaosFrame - 1;
                }
                else
                {
                    pFilePwm += CurFrame - 1;
                }
                for(uint32_t i = 0; i < SIDUSPRO_PWM_CH_NUM; i++)
                {
                    FinalPwm.pwm[i] = pFilePwm->pwm[i] * (pSFX_Arg->SFX_Arg.CFX.Int / 1000.0f);
                }
                CurFrame++;
            }
            else
            {
                if(pSFX_Arg->SFX_Arg.CFX.Sequence == 2)//往复
                {
                    current_dir = 1;
                    CurFrame = FrameNum;
                    pFilePwm = (SidusPro_Pwm_Type*)(SIDUSPRO_CFX_CACHE_ADDR + sizeof(CFX_PwmFileHead_Type)) + FrameNum - 1;
                }
                else//正序
                {
                    if(pSFX_Arg->SFX_Arg.CFX.Loop)
                    {
                        CurFrame = 1;//重头开始循环
                        pFilePwm = (SidusPro_Pwm_Type*)(SIDUSPRO_CFX_CACHE_ADDR + sizeof(CFX_PwmFileHead_Type));
                    }
                    else
                    {
                        OnceFinish = 1;
                        memset(&FinalPwm, 0, sizeof(FinalPwm));
                    }
                }
            }
        }
        else//(1 == current_dir)
        {
            if(CurFrame > 0)
            {
                if(CFX_Is_ChaosNeeded(pSFX_Arg->SFX_Arg.CFX.Chaos))
                {
                    ChaosFrame = SidusPro_Get_RandomRange(1, CFX_PwmFile->FrameNum);//第一帧到最后一帧
                    pFilePwm += ChaosFrame - 1;
                }
                else
                {
                    pFilePwm += CurFrame - 1;
                }
                for(uint32_t i = 0; i < SIDUSPRO_PWM_CH_NUM; i++)
                {
                    FinalPwm.pwm[i] = pFilePwm->pwm[i] * (pSFX_Arg->SFX_Arg.CFX.Int / 1000.0f);
                }
                CurFrame--;
            }
            else
            {
                if(pSFX_Arg->SFX_Arg.CFX.Loop)
                {
                    if(pSFX_Arg->SFX_Arg.CFX.Sequence == 2)//往复
                    {
                        current_dir = 0;//恢复正向
                        CurFrame = 1;
                        pFilePwm = (SidusPro_Pwm_Type*)(SIDUSPRO_CFX_CACHE_ADDR + sizeof(CFX_PwmFileHead_Type));
                    }
                    else//倒序
                    {
                        current_dir = 1;
                        CurFrame = FrameNum;
                        pFilePwm = (SidusPro_Pwm_Type*)(SIDUSPRO_CFX_CACHE_ADDR + sizeof(CFX_PwmFileHead_Type)) + FrameNum - 1;
                    }
                }
                else
                {
                    OnceFinish = 1;
                    memset(&FinalPwm, 0, sizeof(FinalPwm));
                }
            }
        }
        SidusPro_Queue_Put(SidusPro_Queue_PWM, &FinalPwm);
    }
}

static void CFX_Preview_Running_Task(void)
{
    static uint32_t Ticks = 0;
    static SidusPro_Pwm_Type pwm = {0};
    if(Flag_SidusProFX_Arg_Init)
    {
        Flag_SidusProFX_Arg_Init = 0;
        Ticks = 0;
        SidusProFX_Source2Pwm((SFX_Source_Type*)&(pSFX_Arg->SFX_Arg.CFX_Preview.Frame_1), &pwm);
    }
    while(!SidusPro_Queue_Is_Full(SidusPro_Queue_PWM))
    {
        if(Ticks == 100)
        {
            SidusProFX_Source2Pwm((SFX_Source_Type*)&(pSFX_Arg->SFX_Arg.CFX_Preview.Frame_2), &pwm);
        }
        if(Ticks < 200)
        {
            Ticks++;
        }
        SidusPro_Queue_Put(SidusPro_Queue_PWM, &pwm);
    }
}

void SidusProFX_Task(void)
{
    static uint8_t Reset = 0;
    if(SidusProFX_State())
    {
        switch(pSFX_Arg->SFX_Type)
        {
#if (SIDUSPRO_PFX_EN == 1)            
            case SidusPro_PFX:
                PFX_Running_Task();
                break;
#endif 

#if (SIDUSPRO_MFX_EN == 1) 

            case SidusPro_MFX:
                MFX_Running_Task();
                break;
#endif  

#if (SIDUSPRO_CFX_EN == 1)            
            case SidusPro_CFX:
                CFX_Running_Task(Reset);
                break;
            case SidusPro_CFX_Preview:
                CFX_Preview_Running_Task();
                break;
#endif            
            default :
                break;
        }
        Reset = 0;
    }
    else
    {
        if(!Reset)
        {
            Reset = 1;
            SidusPro_Queue_Reset(SidusPro_Queue_PWM);
        }
    }
}

uint32_t SidusProFX_System_Init(void)
{
    return SidusPro_Queue_Create(SidusPro_Queue_PWM, SFX_PWM_FIFO_LEN, sizeof(SidusPro_Pwm_Type)) ? 0 : 1;
}

static uint32_t test_lost = 0;//光效运行过程中数值增加表示丢帧
void SidusProFX_1Ms_Callback(void)
{
    SidusPro_Pwm_Type pwm_cur = {0};
    static uint16_t TicksCFX = 0;
    if(SidusProFX_State())
    {
        switch(pSFX_Arg->SFX_Type)
        {
#if (SIDUSPRO_PFX_EN == 1)            
            case SidusPro_PFX:
                TicksCFX = 0;
                if(true == SidusPro_Queue_Get(SidusPro_Queue_PWM, &pwm_cur))
                {
                    SidusPro_Set_Pwm(&pwm_cur);
                }
                else
                {
                    test_lost++;
                }
                break;
#endif          
#if (SIDUSPRO_MFX_EN == 1)                
            case SidusPro_MFX:
                TicksCFX = 0;
                if(MFX_CTRL_PAUSE != pSFX_Arg->SFX_Arg.MFX.Ctrl && MFX_CTRL_STOP != pSFX_Arg->SFX_Arg.MFX.Ctrl)
                {
                    if(true == SidusPro_Queue_Get(SidusPro_Queue_PWM, &pwm_cur))
                    {
                        SidusPro_Set_Pwm(&pwm_cur);
                    }
                    else
                    {
                        test_lost++;
                    }
                }
                if(pSFX_Arg->SFX_Arg.MFX.Ctrl == MFX_CTRL_STOP)
                {
                    SidusPro_Set_Pwm(&pwm_cur);
                }
                break;
#endif                
#if (SIDUSPRO_CFX_EN == 1)                
            case SidusPro_CFX:
                if(TicksCFX < CFX_Period - 1)
                {
                    TicksCFX++;
                }
                else
                {
                    TicksCFX = 0;
                    if(CFX_CTRL_RUNNING == pSFX_Arg->SFX_Arg.CFX.Ctrl)
                    {
                        if(true == SidusPro_Queue_Get(SidusPro_Queue_PWM, &pwm_cur))
                        {
                            SidusPro_Set_Pwm(&pwm_cur);
                        }
                        else
                        {
                            test_lost++;
                        }
                    }
                    if(CFX_CTRL_STOP == pSFX_Arg->SFX_Arg.CFX.Ctrl)
                    {
                        SidusPro_Set_Pwm(&pwm_cur);
                    }
                }
                break;
            case SidusPro_CFX_Preview:
                if(true == SidusPro_Queue_Get(SidusPro_Queue_PWM, &pwm_cur))
                {
                    SidusPro_Set_Pwm(&pwm_cur);
                }
                else
                {
                    test_lost++;
                }
                break;
#endif                
            default :
                TicksCFX = 0;
                break;
        }
    }
    else
    {
        TicksCFX = 0;
    }
}

void SidusProFX_Enable(bool en)
{
    SidusProFX_EN = en;
}

bool SidusProFX_State(void)
{
    return SidusProFX_EN;
}

void SidusProFX_Arg_Init(SidusProFX_ArgBody_Type* arg)
{
    static uint8_t SFX_Arg_Index = 0;
    SFX_Arg_Index = SFX_Arg_Index == 0 ? 1 : 0;
    memcpy(&SidusPro_Arg[SFX_Arg_Index], arg, sizeof(SidusProFX_ArgBody_Type));
    switch(SidusPro_Arg[SFX_Arg_Index].SFX_Type)
    {
#if (SIDUSPRO_PFX_EN == 1)
        case SidusPro_PFX:
            Flag_SidusProFX_Arg_Init = 1;
            break;
#endif
#if (SIDUSPRO_MFX_EN == 1)
        case SidusPro_MFX:
            switch(SidusPro_Arg[SFX_Arg_Index].SFX_Arg.MFX.Ctrl)
            {
                case MFX_CTRL_STOP:
                    SidusPro_Queue_Reset(SidusPro_Queue_PWM);
                    break;
                case MFX_CTRL_RESTART:
                    Flag_SidusProFX_Arg_Init = 1;
                    break;
                case MFX_CTRL_PAUSE:
                case MFX_CTRL_CONTINUE:
                    break;
                default:
                    break;
            }                
            break;
#endif  
#if (SIDUSPRO_CFX_EN == 1)
        case SidusPro_CFX:
            switch(SidusPro_Arg[SFX_Arg_Index].SFX_Arg.CFX.Ctrl)
            {
                case CFX_CTRL_STOP:
                    Flag_SidusProFX_Arg_Init = 1;
                    SidusPro_Queue_Reset(SidusPro_Queue_PWM);
                    break;
                case CFX_CTRL_RUNNING:
                case CFX_CTRL_PAUSE:
                    Flag_SidusProFX_Arg_Init = 1;
                    break;
                case CFX_CTRL_INIT:
                    SidusProFX_Enable(false);
                    SidusPro_Queue_Reset(SidusPro_Queue_PWM);
                    SidusProFile_CFX_File_Init(SidusPro_Arg[SFX_Arg_Index].SFX_Arg.CFX.Effect_Type, SidusPro_Arg[SFX_Arg_Index].SFX_Arg.CFX.Bank);
                    break;
                default:
                    break;
            }
            break;
        case SidusPro_CFX_Preview:
            Flag_SidusProFX_Arg_Init = 1;
            break;
#endif        
        default:
            break;
    }
    pSFX_Arg = &SidusPro_Arg[SFX_Arg_Index];
}


