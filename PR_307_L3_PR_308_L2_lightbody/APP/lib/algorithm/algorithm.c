/****************************************************************************************
**  Filename :  algorithm.c
**  Abstract :  This file implements pm2.5 function.
**  By       :  HeJianGuo <hejianguo5515@126.com>
**  Date     :  2018-01-23
**  Changelog:1.First Create
*****************************************************************************************/

/*****************************************************************************************
									Includes
*****************************************************************************************/
#include "algorithm.h"
/*****************************************************************************************
							  Variables definitions
*****************************************************************************************/

/*****************************************************************************************
							  Functions definitions
*****************************************************************************************/

//算法数据区

//AlgDataStr myAlgTemperDataStr={.ArraryLen=TD_ArraryNum};
//AlgDataStr myAlgDampnessDataStr={.ArraryLen=TD_ArraryNum};

//#define TemperLimitsValue 5
//#define DampnessLimitsValue 10
short TemperLastValue = 250;//25度
short DampnessLastValue = 550;//55%

AlgDataStr TemperAlgDataStr={.ArraryLen=4}; 



//*****************************
//PM25可先做中值平均滤波
//再用卡尔曼滤波
//但这样必须提高采样率
//*****************************
//Q是预测协方差 过小系统收敛慢 过大有可能滤波不干净 *****   R值是测量协方差，影响延迟程度，越大越延迟
//AlgKalmanDataStr myAlgVocDataStr={.M_X_Last=50,.M_P_Last=10,.M_Q=0.005,.M_R=0.2};//Voc是热电阻式的，基本上不用做滤波
//AlgKalmanDataStr myAlgPM25DataStr={.M_X_Last=50,.M_P_Last=10,.M_Q=0.005,.M_R=0.1};//必须初始化的值
AlgKalmanDataStr myAlgPM25DataStr={.M_X_Last=50,.M_P_Last=10,.M_Q=0.01,.M_R=0.07};//必须初始化的值

//unsigned char VocTimeFlag=0;//在定时器中使用，原先采样是1s时间，现已修改，用此变量提高采样率




PreheatStr myPreheatStr={.PreheatFlag=0,.PreheatTime=0};



//算法数据区end

/*****************************************************************************************
* Function Name: AlgorithmLimits
* Description  : 限值滤波,超过限定值时就当做有效的
* Arguments    : (short NewValue,short* LastValue,short LimitsValue)新的采样值，历史值，限制值
* Return Value : 经过滤波处理后的值
******************************************************************************************/
short AlgorithmLimits(short NewValue,short* LastValue,short LimitsValue)
{
	short ret=*LastValue;
	if(NewValue==*LastValue)
	{
		ret = *LastValue;
	}
	else if(NewValue>*LastValue)
	{
		if((NewValue-*LastValue)>=LimitsValue)
		{
			ret = NewValue;
			*LastValue = NewValue;			
		}
	}
	else //if(NewValue<*LastValue)
	{
		if((*LastValue-NewValue)>=LimitsValue)
		{
			ret = NewValue;
			*LastValue = NewValue;			
		}
	}
	return ret;
}
/*****************************************************************************************
* Function Name: AlgorithmMidAv
* Description  : 中值平均滤波
* Arguments    : DataArrary[] 要处理的数组;ArraryLen 要计算的数组的元素个数
* Return Value : 经过滤波处理后的值
******************************************************************************************/
int AlgorithmMidAv(int DataArrary[],unsigned int ArraryLen)  
{
	
	int i;
	int MaxIndex=0,MinIndex=0;
	int OutValue=0;
	int Max=0,Min=0;
	if(ArraryLen<3)  //  长度小于三  算法没意义
		return 0;
	Max = DataArrary[0];
	
	for(i=1;i<ArraryLen;i++)  //找到最大值的下标
	{
		if(Max < DataArrary[i])
		{
			Max = DataArrary[i];
			MaxIndex = i;
		}
	}
	Min = DataArrary[0];
	for(i=1;i<ArraryLen;i++)  //找到最小值的下标
	{
		if(Min > DataArrary[i])
		{
			Min = DataArrary[i];
			MinIndex = i;
		}
	}
	if(Min==Max)  //最大和最小值相等  即所有值相等
	{
		return Min;  // 直接返回，不用再滤波
	}
	DataArrary[MinIndex] = 0;
	DataArrary[MaxIndex] = 0;//把最大最小值去除   做0处理   bug 找不到 MaxIndex 会有可能 MaxIndex 为不定值，将会在这个不定的地址中写0  错误  所以在定义时 应该赋0

	for(i=0;i<ArraryLen;i++)
	{
		OutValue += DataArrary[i];		
	}
	OutValue /= (ArraryLen-2);//去掉最大最小值后的均值
	return OutValue;

}
/*****************************************************************************************
* Function Name: AlgorithmQueueAv
* Description  : 递推平均滤波
* Arguments    : myAlgDataStr 历史值结构体;SamplingValue 新的采样值
* Return Value : 经过滤波处理后的值
******************************************************************************************/
float AlgorithmQueueAv(AlgDataStr* myAlgDataStr,float SamplingValue)
{
	int i;
	float OutValue=0;
	if(myAlgDataStr->ArraryLen>1)
	{
		for(i=myAlgDataStr->ArraryLen-1;i>0;i--)//历史值前一位往后移
		{
			myAlgDataStr->DataArrary[i] = myAlgDataStr->DataArrary[i-1];
		}
		myAlgDataStr->DataArrary[0] = SamplingValue;//新值
		for(i=0;i<myAlgDataStr->ArraryLen;i++)
		{
			OutValue += myAlgDataStr->DataArrary[i];
		}
		OutValue /= myAlgDataStr->ArraryLen;
	}
	else
	{
		myAlgDataStr->DataArrary[0] = OutValue = SamplingValue;
	}

	return OutValue;

}
/*****************************************************************************************
* Function Name: AlgorithmOnceLag
* Description  : 一阶滞后滤波
* Arguments    : myAlgDataStr 历史值结构体;Coefficient 算法系数  0 <= Coefficient < 1  越接近1对历史值的依赖性越大;SamplingValue 新的采样值
* Return Value : 经过滤波处理后的值
******************************************************************************************/
//#define Coefficient 0.2
float AlgorithmOnceLag(AlgDataStr* myAlgDataStr,float SamplingValue,float Coefficient)
{
	float OutValue=0;
	if(0<= Coefficient <1)
	{
		OutValue = Coefficient*myAlgDataStr->DataArrary[0] + (1-Coefficient)*SamplingValue;
		
	}
	else
	{
		OutValue = SamplingValue;
	}
	myAlgDataStr->DataArrary[0] = OutValue;

	return OutValue;
}
/*****************************************************************************************
* Function Name: AlgorithmKalman
* Description  : Kalman滤波
* Arguments    : myAlgKalmanDataStr 要进行滤波计算的对象;SamplingValue 新的采样值
* Return Value : 经过滤波处理后的值
******************************************************************************************/
float AlgorithmKalman(AlgKalmanDataStr* myAlgKalmanDataStr,float SamplingValue)
{
	myAlgKalmanDataStr->M_X_Mid = myAlgKalmanDataStr->M_X_Last;
	myAlgKalmanDataStr->M_P_Mid = myAlgKalmanDataStr->M_P_Last + myAlgKalmanDataStr->M_Q;
	
	myAlgKalmanDataStr->M_Kg = myAlgKalmanDataStr->M_P_Mid/(myAlgKalmanDataStr->M_P_Mid + myAlgKalmanDataStr->M_R);
	myAlgKalmanDataStr->M_X_Now = myAlgKalmanDataStr->M_X_Mid + myAlgKalmanDataStr->M_Kg*(SamplingValue - myAlgKalmanDataStr->M_X_Mid);
	
	myAlgKalmanDataStr->M_P_Now = (1-myAlgKalmanDataStr->M_Kg)*myAlgKalmanDataStr->M_P_Mid;

	myAlgKalmanDataStr->M_X_Last = myAlgKalmanDataStr->M_X_Now;
	myAlgKalmanDataStr->M_P_Last = myAlgKalmanDataStr->M_P_Now;
        
  return myAlgKalmanDataStr->M_X_Now;
}

/*
#define MTestNum 32

float MTest[MTestNum]={100,95,93,105,100,91,99,103,101,97,
											 99,100,106,101,95,98,99,100,103,96,
											 93,94,94,101,100,99,96,99,102,97,
											 102,98
											};

float MTest2[MTestNum];

void KalmanTest(void)
{
	
	int i;
	for(i=0;i<MTestNum;i++)
	{
		MTest2[i]=(short)AlgorithmKalman(&myAlgVocDataStr,MTest[i]);
	}
	i = 0;
}
*/
/***********************************END OF FILE*******************************************/


