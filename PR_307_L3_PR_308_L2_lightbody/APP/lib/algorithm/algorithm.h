/****************************************************************************************
**  Filename :  algorithm.h
**  Abstract :  This file implements PM2.5 function.
**  By       :  HeJianGuo <hejianguo5515@126.com>
**  Date     :  2018-01-23
**  Changelog:1.First Create
*****************************************************************************************/
#ifndef _ALGORITHM_H_
#define _ALGORITHM_H_
/*****************************************************************************************
									Includes
*****************************************************************************************/
#include "stdint.h"
/*****************************************************************************************
								Macro definitions
*****************************************************************************************/
#define TD_ArraryNum 4
//0.2度
#define TemperLimitsValue 2
#define DampnessLimitsValue 2

#define PreheatTemper 0x01
#define PreheatDamp 0x02
#define PreheatVoc 0x04
#define PreheatPM25 0x08

//预热/准备 时间 n*25 n秒
#define PreheatTemperTime 10*25
#define PreheatDampTime 10*25
#define PreheatVocTime 600*25//600*25
#define PreheatPM25Time 20*25
/*****************************************************************************************
								Typedef definitions
*****************************************************************************************/
typedef struct
{
	int ArraryLen;
	float DataArrary[TD_ArraryNum];
}AlgDataStr;

typedef struct
{
	float M_X_Mid;
	float M_X_Last;
	float M_P_Mid;
	float M_P_Last;
	float M_Kg;
	float M_X_Now;
	float M_P_Now;
	float M_Q;
	float M_R;
	
}AlgKalmanDataStr;

typedef struct
{
  char PreheatFlag;
	int PreheatTime;
}PreheatStr;
/*****************************************************************************************
							Global variables and functions
*****************************************************************************************/
extern AlgDataStr TemperAlgDataStr;
extern short TemperLastValue;//25度
extern short DampnessLastValue;//55%
extern AlgKalmanDataStr myAlgPM25DataStr;
extern PreheatStr myPreheatStr;
//----------------------------------------------------------------------------------------
extern short AlgorithmLimits(short NewValue,short* LastValue,short LimitsValue);
extern int AlgorithmMidAv(int DataArrary[],unsigned int ArraryLen);
extern float AlgorithmQueueAv(AlgDataStr* myAlgDataStr,float SamplingValue);
extern float AlgorithmOnceLag(AlgDataStr* myAlgDataStr,float SamplingValue,float Coefficient);
extern float AlgorithmKalman(AlgKalmanDataStr* myAlgKalmanDataStr,float SamplingValue);
#endif
/***********************************END OF FILE*******************************************/

