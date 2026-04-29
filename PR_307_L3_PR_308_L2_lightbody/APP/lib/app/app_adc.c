/****************************************************************************************
**  Filename :  het_led.c
**  Abstract :  LED模块化程序，支持设置LED常亮，常灭，闪烁周期设置，闪烁次数设置等功能。
**  By       :  何建国
**  Date     :  2018-11-21
**  Changelog:1.First Create
*****************************************************************************************/
#define VOLT_PRINTF_EN              0
/*****************************************************************************************
									Includes
*****************************************************************************************/
#include "app_adc.h"
#include "algorithm.h"
#include "data_fill.h"
#include "stdio.h"
#include "bsp_adc.h"
#include "bsp_init.h"
/*****************************************************************************************
							  Variables definitions
*****************************************************************************************/
typedef __packed struct
{
	int8_t	temperature;  //温度，范围-128~127
	float 	resistance;
}Rtc_Resistance;
typedef __packed struct 
{
    uint32_t Source_Data;
    uint32_t Obj_Data;
    uint16_t fill_sel;
    uint32_t fill_buf[ADC_FILL_NUM];
}Adc_Data_Dispose_TypeDef;
static Adc_Data_Dispose_TypeDef s_adc_data_dispose[ADC_CHANNEL_NUMBER]; 

const uint32_t COB_RT_Table[] =
{
    69693,  66329,  63148,  60140,  57293,  54599,  52049,  49633,  47343,  45174,
    43117,  41166,  39315,  37558,  35891,  34307,  32802,  31373,  30014,  28722,
    27493,  26324,  25211,  24152,  23144,  22183,  21268,  20395,  19564,  18771,
    18015,  17293,  16604,  15947,  15319,  14720,  14147,  13600,  13077,  12577,
    12098,  11641,  11203,  10784,  10383,  10000,  9632,   9280,   8942,   8619,
    8309,  8012,  7727,  7454,  7191,  6940,  6698,  6466,  6244,  6030,
    5825,  5627,  5438,  5255,  5080,  4911,  4749,  4593,  4443,  4299,
    4160,  4026,  3898,  3773,  3654,  3539,  3428,  3321,  3218,  3119,
    3023,  2931,  2841,  2755,  2672,  2592,  2515,  2441,  2369,  2299,
    2232,  2167,  2104,  2044,  1985,  1928,  1874,  1821,  1769,  1720,
    1672,  1626,  1581,  1537,  1495,  1455,  1415,  1377,  1340,  1305,
    1270, 1236,  1204, 1172, 1142, 1112,  1083,  1056, 1029,  1002,
    977, 952,  928,  905,  883,  861,  839,   819,   799,  779,
    760,
};

#define RESISTANCE  10   //分压电阻为10k

/*温度对应的电阻值，单位：kΩ，温度范围：-128~127°*/
const Rtc_Resistance _3380_resistance[] = {
	{ -40     ,208.15 },{ -39     ,196.57 },{ -38     ,185.71 },{ -37     ,175.52 },{ -36     ,165.96 },
	{ -35     ,156.97 },{ -34     ,148.53 },{ -33     ,140.60 },{ -32     ,133.14 },{ -31     ,126.12 },
	{ -30     ,119.52 },{ -29     ,113.30 },{ -28     ,107.45 },{ -27     ,101.93 },{ -26     ,96.73  },
	{ -25     ,91.83  },{ -24     ,87.21  },{ -23     ,82.85  },{ -22     ,78.73  },{ -21     ,74.85  },
	{ -20     ,71.18  },{ -19     ,67.71  },{ -18     ,64.43  },{ -17     ,61.33  },{ -16     ,58.40  },
	{ -15     ,55.62  },{ -14     ,53.00  },{ -13     ,50.51  },{ -12     ,48.16  },{ -11     ,45.93  },
	{ -10     ,43.81  },{ -9      ,41.81  },{ -8      ,39.91  },{ -7      ,38.11  },{ -6      ,36.40  },
	{ -5      ,34.77  },{ -4      ,33.23  },{ -3      ,31.77  },{ -2      ,30.38  },{ -1      ,29.05  },
	{ 0       ,27.80  },{ 1       ,26.60  },{ 2       ,25.46  },{ 3       ,24.38  },{ 4       ,23.35  },
	{ 5       ,22.37  },{ 6       ,21.44  },{ 7       ,20.55  },{ 8       ,19.70  },{ 9       ,18.90  },
	{ 10      ,18.13  },{ 11      ,17.39  },{ 12      ,16.69  },{ 13      ,16.02  },{ 14      ,15.39  },
	{ 15      ,14.78  },{ 16      ,14.20  },{ 17      ,13.64  },{ 18      ,13.11  },{ 19      ,12.61  },
	{ 20      ,12.12  },{ 21      ,11.66  },{ 22      ,11.22  },{ 23      ,10.79  },{ 24      ,10.39  },
	{ 25      ,10     },{ 26      ,9.63   },{ 27      ,9.27   },{ 28      ,8.93   },{ 29      ,8.61   },
	{ 30      ,8.30   },{ 31      ,8.00   },{ 32      ,7.71   },{ 33      ,7.43   },{ 34      ,7.17   },
	{ 35      ,6.92   },{ 36      ,6.67   },{ 37      ,6.44   },{ 38      ,6.22   },{ 39      ,6.00   },
	{ 40      ,5.80   },{ 41      ,5.60   },{ 42      ,5.41   },{ 43      ,5.23   },{ 44      ,5.05   },
	{ 45      ,4.88   },{ 46      ,4.72   },{ 47      ,4.57   },{ 48      ,4.42   },{ 49      ,4.27   },
	{ 50      ,4.13   },{ 51      ,4.00   },{ 52      ,3.87   },{ 53      ,3.75   },{ 54      ,3.63   },
	{ 55      ,3.51   },{ 56      ,3.40   },{ 57      ,3.30   },{ 58      ,3.19   },{ 59      ,3.09   },
	{ 60      ,3.00   },{ 61      ,2.91   },{ 62      ,2.82   },{ 63      ,2.73   },{ 64      ,2.65   },
	{ 65      ,2.57   },{ 66      ,2.49   },{ 67      ,2.42   },{ 68      ,2.35   },{ 69      ,2.28   },
	{ 70      ,2.21   },{ 71      ,2.15   },{ 72      ,2.09   },{ 73      ,2.03   },{ 74      ,1.97   },
	{ 75      ,1.91   },{ 76      ,1.86   },{ 77      ,1.80   },{ 78      ,1.75   },{ 79      ,1.70   },
	{ 80      ,1.66   },{ 81      ,1.61   },{ 82      ,1.57   },{ 83      ,1.52   },{ 84      ,1.48   },
	{ 85      ,1.44   },{ 86      ,1.40   },{ 87      ,1.37   },{ 88      ,1.33   },{ 89      ,1.29   },
	{ 90      ,1.26   },{ 91      ,1.23   },{ 92      ,1.19   },{ 93      ,1.16   },{ 94      ,1.13   },
	{ 95      ,1.10   },{ 96      ,1.07   },{ 97      ,1.05   },{ 98      ,1.02   },{ 99      ,0.99   },
	{ 100     ,0.97   },{ 101     ,0.95   },{ 102     ,0.92   },{ 103     ,0.90   },{ 104     ,0.88   },
	{ 105     ,0.85   },{ 106     ,0.83   },{ 107     ,0.81   },{ 108     ,0.79   },{ 109     ,0.77   },
	{ 110     ,0.76   },{ 111     ,0.74   },{ 112     ,0.72   },{ 113     ,0.70   },{ 114     ,0.69   },
	{ 115     ,0.67   },{ 116     ,0.65   },{ 117     ,0.64   },{ 118     ,0.62   },{ 119     ,0.61   },
	{ 120     ,0.60   },{ 121     ,0.58   },{ 122      ,0.57  },{ 123     ,0.56   },{ 124     ,0.54   },
};

const Rtc_Resistance _3450_resistance[] = {
	{ -30     ,123.5 },{ -29     ,116.97 },{ -28     ,110.84 },{ -27     ,105.06 },{ -26     ,99.63 },
	{ -25     ,94.51 },{ -24     ,89.68  },{ -23     ,85.13  },{ -22     ,80.84  },{ -21     ,76.79 },
	{ -20     ,72.79 },{ -19     ,69.36  },{ -18     ,65.96  },{ -17     ,62.74  },{ -16     ,59.70 },
	{ -15     ,56.83 },{ -14     ,54.11  },{ -13     ,51.53  },{ -12     ,49.10  },{ -11     ,46.80 },
	{ -10     ,44.61 },{ -9      ,42.55  },{ -8      ,40.59  },{ -7      ,38.73  },{ -6      ,36.97 },
	{ -5      ,35.30 },{ -4      ,33.71  },{ -3      ,32.21  },{ -2      ,30.78  },{ -1      ,29.42 },
	{ 0       ,28.13 },{ 1       ,26.91  },{ 2       ,25.75  },{ 3       ,24.64  },{ 4       ,23.59 },
	{ 5       ,22.58 },{ 6       ,21.63  },{ 7       ,20.72  },{ 8       ,19.86  },{ 9       ,19.03 },
	{ 10      ,18.25 },{ 11      ,17.50  },{ 12      ,16.79  },{ 13      ,16.11  },{ 14      ,15.46 },
	{ 15      ,14.84 },{ 16      ,14.25  },{ 17      ,13.69  },{ 18      ,13.15  },{ 19      ,12.64 },
	{ 20      ,12.15 },{ 21      ,11.68  },{ 22      ,11.23  },{ 23      ,10.80  },{ 24      ,10.39 },
	{ 25      ,10    },{ 26      ,9.62   },{ 27      ,9.27   },{ 28      ,8.92   },{ 29      ,8.59  },
	{ 30      ,8.28  },{ 31      ,7.98   },{ 32      ,7.69   },{ 33      ,7.41   },{ 34      ,7.15  },
	{ 35      ,6.89  },{ 36      ,6.65   },{ 37      ,6.41   },{ 38      ,6.19   },{ 39      ,5.97  },
	{ 40      ,5.77  },{ 41      ,5.57   },{ 42      ,5.38   },{ 43      ,5.19   },{ 44      ,5.02  },
	{ 45      ,4.85  },{ 46      ,4.68   },{ 47      ,4.53   },{ 48      ,4.38   },{ 49      ,4.23  },
	{ 50      ,4.10  },{ 51      ,3.96   },{ 52      ,3.83   },{ 53      ,3.71   },{ 54      ,3.59  },
	{ 55      ,3.48  },{ 56      ,3.37   },{ 57      ,3.26   },{ 58      ,3.16   },{ 59      ,3.06  },
	{ 60      ,2.96  },{ 61      ,2.87   },{ 62      ,2.78   },{ 63      ,2.70   },{ 64      ,2.62  },
	{ 65      ,2.54  },{ 66      ,2.46   },{ 67      ,2.39   },{ 68      ,2.32   },{ 69      ,2.23  },
	{ 70      ,2.18  },{ 71      ,2.12   },{ 72      ,2.05   },{ 73      ,1.99   },{ 74      ,1.94  },
	{ 75      ,1.88  },{ 76      ,1.83   },{ 77      ,1.78   },{ 78      ,1.73   },{ 79      ,1.68  },
	{ 80      ,1.63  },{ 81      ,1.58   },{ 82      ,1.54   },{ 83      ,1.50   },{ 84      ,1.46  },
	{ 85      ,1.42  },{ 86      ,1.38   },{ 87      ,1.34   },{ 88      ,1.30   },{ 89      ,1.27  },
	{ 90      ,1.24  },{ 91      ,1.20   },{ 92      ,1.17   },{ 93      ,1.14   },{ 94      ,1.11  },
	{ 95      ,1.08  },{ 96      ,1.05   },{ 97      ,1.03   },{ 98      ,1.00   },{ 99      ,0.97  },
	{ 100     ,0.95  },{ 101     ,0.93   },{ 102     ,0.90   },{ 103     ,0.88   },{ 104     ,0.86  },
	{ 105     ,0.84  },{ 106     ,0.82   },{ 107     ,0.80   },{ 108     ,0.78   },{ 109     ,0.76  },
	{ 110     ,0.74  },{ 111     ,0.72   },{ 112     ,0.70   },{ 113     ,0.69   },{ 114     ,0.67  },
	{ 115     ,0.65  },{ 116     ,0.64   },{ 117     ,0.61   },{ 118     ,0.60   },{ 119     ,0.58  },
	{ 120     ,0.52  },
};
/*****************************************************************************************
							  Functions definitions
*****************************************************************************************/
static void Get_Adc_Second_Ave_Val(uint32_t *source_buf, 
								   uint16_t *data_sel, 
								   uint32_t *data, 
								   uint32_t tmp_value);
static float Binary_Search_Temperature(const Rtc_Resistance * data, uint16_t length, float key);
static void Get_Adc_First_Ave_Val(uint16_t *pBuff, uint32_t adc_ch, uint8_t star, uint8_t count);
static void Get_Adc2_Dma_CallBack(uint16_t *pBuff);
static void Get_Adc3_Dma_CallBack(uint16_t *pBuff);
static void adc_to_volt(void);

static uint8_t Get_COB_Temperature(uint32_t R)
{
    uint8_t temp = 0;
    for(temp = 0; temp < 131; temp++)
    {
        if(R > COB_RT_Table[temp])
        {
            break;
        }
    }
    return temp;
}

/*****************************************************************************************
* Function Name: Adc_Driver_Init
* Description  : ADC检测功能初始化
* Arguments    : NONE
* Return Value : NONE
******************************************************************************************/
void Adc_Driver_Init(void)
{
    Adc1_Dma_CallBack = Get_Adc2_Dma_CallBack;
	Adc2_Dma_CallBack = Get_Adc3_Dma_CallBack;
}

/*****************************************************************************************
* Function Name: get_adc_para_val
* Description  : 获取各检测模块检测点的电压值，单位mV(10ms时间片)
* Arguments    : NONE
* Return Value : NONE
******************************************************************************************/
void get_adc_para_val(void)
{
#define BASE_VOLT (3.3f)
#define VDD_CHECK_SET_VALUE (3300)	
	ADC_Enable(); 
	ADC2_Enable(); 

	g_adc_para.V_check_contact1 = (BASE_VOLT * (float)s_adc_data_dispose[ADC_DMA_RANK20_ADDR].Obj_Data) / (float)ADC1_HVGA * 1.0f;
	g_adc_para.V_check_contact2 = (BASE_VOLT * (float)s_adc_data_dispose[ADC_DMA_RANK21_ADDR].Obj_Data) / (float)ADC1_HVGA * 1.0f;
	
	Check_Contact1_En(g_adc_para.V_check_contact2);
	Check_Contact2_En(g_adc_para.V_check_contact1);
		
	g_adc_para.V_red_adapt = s_adc_data_dispose[ADC_DMA_RANK19_ADDR].Obj_Data;
	g_adc_para.V_green_adapt = s_adc_data_dispose[ADC_DMA_RANK16_ADDR].Obj_Data;
	g_adc_para.V_blue_adapt = s_adc_data_dispose[ADC_DMA_RANK15_ADDR].Obj_Data;	
	g_adc_para.V_ww1_adapt = s_adc_data_dispose[ADC_DMA_RANK11_ADDR].Obj_Data;
	g_adc_para.V_cw1_adapt = s_adc_data_dispose[ADC_DMA_RANK9_ADDR].Obj_Data;
	g_adc_para.V_ww2_adapt = s_adc_data_dispose[ADC_DMA_RANK10_ADDR].Obj_Data;
	g_adc_para.V_cw2_adapt = s_adc_data_dispose[ADC_DMA_RANK8_ADDR].Obj_Data;
	g_adc_para.V_YUADC_adapt = s_adc_data_dispose[ADC_DMA_RANK14_ADDR].Obj_Data;
	
	g_adc_para.V_check_48V = s_adc_data_dispose[ADC_DMA_RANK0_ADDR].Obj_Data;//(float)g_adc_para.Mcu_Volt * ((float)s_adc_data_dispose[ADC_DMA_RANK0_ADDR].Obj_Data / (float)ADC1_HVGA) * i;
	float volt;
	volt = s_adc_data_dispose[ADC_DMA_RANK1_ADDR].Obj_Data * 3.3f / 4096;
	g_adc_para.V_check_red = volt / (6.8 / 206.8);
	volt = s_adc_data_dispose[ADC_DMA_RANK2_ADDR].Obj_Data * 3.3f / 4096;
	g_adc_para.V_check_green = volt / (6.8 / 206.8);
	volt = s_adc_data_dispose[ADC_DMA_RANK3_ADDR].Obj_Data * 3.3f / 4096;
	g_adc_para.V_check_blue = volt / (6.8 / 206.8);
	volt = s_adc_data_dispose[ADC_DMA_RANK4_ADDR].Obj_Data * 3.3f / 4096;
	g_adc_para.V_check_cw1 = volt / (6.8 / 206.8);
	volt = s_adc_data_dispose[ADC_DMA_RANK5_ADDR].Obj_Data * 3.3f / 4096;
	g_adc_para.V_check_cw2 = volt / (6.8 / 206.8);
	volt = s_adc_data_dispose[ADC_DMA_RANK6_ADDR].Obj_Data * 3.3f / 4096;
	g_adc_para.V_check_ww1 = volt / (6.8 / 206.8);
	volt = s_adc_data_dispose[ADC_DMA_RANK7_ADDR].Obj_Data * 3.3f / 4096;
	g_adc_para.V_check_ww2 = volt / (6.8 / 206.8);
	
	uint32_t temper_adc_value = 4095.0f * 100 / s_adc_data_dispose[ADC_DMA_RANK12_ADDR].Obj_Data * 100.0f  - 10000;	
	g_adc_para.mcu_temper  =  Get_COB_Temperature(temper_adc_value) - 20;

	g_adc_para.Mcu_Volt = VDD_SET_VALUE;
	float ntc_resistance = (3.0f * ((float)s_adc_data_dispose[ADC_DMA_RANK17_ADDR].Obj_Data)/(float)4095) / (1 - ((float)s_adc_data_dispose[ADC_DMA_RANK17_ADDR].Obj_Data)/(float)4095);
#ifdef PR_307_L3
	g_adc_para.cob_temper = Binary_Search_Temperature(_3380_resistance, sizeof(_3380_resistance) / sizeof(Rtc_Resistance), ntc_resistance) + 4;
#endif
#ifdef PR_308_L2
	g_adc_para.cob_temper = Binary_Search_Temperature(_3380_resistance, sizeof(_3380_resistance) / sizeof(Rtc_Resistance), ntc_resistance) + 3;
#endif
	adc_to_volt();
} 


static void adc_to_volt(void)
{
	g_volt_para.V_red_adapt = ((float)g_adc_para.V_red_adapt / 4096) * BASE_VOLT;
	g_volt_para.V_green_adapt = ((float)g_adc_para.V_green_adapt / 4096) * BASE_VOLT;
	g_volt_para.V_blue_adapt = ((float)g_adc_para.V_blue_adapt / 4096) * BASE_VOLT;
	g_volt_para.V_ww1_adapt = ((float)g_adc_para.V_ww1_adapt / 4096) * BASE_VOLT;
	g_volt_para.V_cw1_adapt = ((float)g_adc_para.V_cw1_adapt / 4096) * BASE_VOLT;
	g_volt_para.V_ww2_adapt = ((float)g_adc_para.V_ww2_adapt / 4096) * BASE_VOLT;
	g_volt_para.V_cw2_adapt = ((float)g_adc_para.V_cw2_adapt / 4096) * BASE_VOLT;
}

static void Get_Adc2_Dma_CallBack(uint16_t *pBuff)
{
	Get_Adc_First_Ave_Val(pBuff, 4, ADC_DMA_RANK0_ADDR, 6);
	Get_Adc_First_Ave_Val(pBuff, 14, ADC_DMA_RANK6_ADDR, 2);
	Get_Adc_First_Ave_Val(pBuff, 10, ADC_DMA_RANK8_ADDR, 4);
}

static void Get_Adc3_Dma_CallBack(uint16_t *pBuff)
{
	Get_Adc_First_Ave_Val(pBuff, 4,  ADC_DMA_RANK12_ADDR, 6);
	Get_Adc_First_Ave_Val(pBuff, 14, ADC_DMA_RANK18_ADDR, 1);
	Get_Adc_First_Ave_Val(pBuff, 16, ADC_DMA_RANK19_ADDR, 1);
	Get_Adc_First_Ave_Val(pBuff, 18, ADC_DMA_RANK20_ADDR, 2);	
}
/*****************************************************************************************
* Function Name: Get_Adc_First_Ave_Val
* Description  : 获得ADC在连续时间连续数次转换的结果；(这个时间段是驱动COBE灯的PWM周期)
* Arguments    : NONE
* Return Value : NONE
******************************************************************************************/
static void Get_Adc_First_Ave_Val(uint16_t *pBuff, uint32_t adc_ch, uint8_t star, uint8_t count)
{
    static uint32_t adc_tem_val[ADC_CHANNEL_NUMBER] = {0};
	static uint16_t s_adc_count[ADC_CHANNEL_NUMBER] = {0};
	uint16_t acc_times = 10;//信号周期 50us, 采样周期4.8us, 50 / 4.8 * 50 = 520
    uint8_t i = 0;
   
    for(i = 0; i < count; i++)
    {
        adc_tem_val[star + i] += pBuff[adc_ch + i];
    }
    s_adc_count[star]++;
    if(s_adc_count[star] >= acc_times)
    {
        s_adc_count[star] = 0;
        for(i = star; i < (star + count); i++)
        {
            s_adc_data_dispose[i].Source_Data = adc_tem_val[i] / acc_times;
            adc_tem_val[i] = 0;
			#if 1
			Get_Adc_Second_Ave_Val((uint32_t*)s_adc_data_dispose[i].fill_buf, 
								   (uint16_t*)&s_adc_data_dispose[i].fill_sel, 
								   (uint32_t*)&s_adc_data_dispose[i].Obj_Data, 
								   s_adc_data_dispose[i].Source_Data);
			#endif
        }
    }
}



/*****************************************************************************************
* Function Name: Get_Adc_Second_Ave_Val
* Description  : 获得平均值；
* Arguments    : NONE
* Return Value : NONE
******************************************************************************************/
static void Get_Adc_Second_Ave_Val(uint32_t *source_buf, 
								   uint16_t *data_sel, 
								   uint32_t *data, 
								   uint32_t tmp_value)
{
    uint32_t des_buf[ADC_FILL_NUM] = {0};
	uint8_t* p_source = (uint8_t*)source_buf;
    AlgorithFillBuf(ADC_FILL_NUM, source_buf, data_sel, tmp_value);//数组填充算法
    memcpy(des_buf, p_source, sizeof(des_buf));
    *data = AlgorithmMidAv((int*)des_buf, ADC_FILL_NUM);//中值平均滤波
}   
/*****************************************************************************************
* Function Name: Binary_Search_Temperature
* Description  : 二分法获取当前温度值；
* Arguments    : 数据表，数据表中数据的排序方式为从大到小;数据表的长度;所查找的值
* Return Value : 所查找的值在数据表中所处的位置;-1，-2：所查找的数据不在数据表的范围
******************************************************************************************/
static float Binary_Search_Temperature(const Rtc_Resistance * data, uint16_t length, float key)
{
	float ret;
	uint16_t start = 0;
	uint16_t end = length - 1;
	uint16_t middle;
	
	if (key > data[start].resistance) //小于最小检测温度，返回最小检测温度
	{
		ret = data[start].temperature;
	}
	else if (key < data[end].resistance) //大于最大检测温度，返回最大检测温度
	{
		ret = data[end].temperature;
	}
	else //开始二分法查表
	{
		while (start <= end)
		{
			middle = (start + end)/2;
			if (key < data[middle].resistance)
			{
				start = middle;
			}
			else if (key > data[middle].resistance)
			{
				end = middle;
			}
			else
			{
				return data[middle].temperature;
			}
			
			if((start + 1 == end)) 
			{
				//在相邻的区按线性求值
				ret = data[start].temperature + 
						(data[end].temperature - data[start].temperature)*
							(data[start].resistance - key)/(data[start].resistance - data[end].resistance);
				return ret;
			}	
		}
		ret = data[start].temperature;
	}
	return ret;
}/*****************************************************************************************
* Function Name: Adc_Printf_Test
* Description  : 
* Arguments    : 
* Return Value : NONE
******************************************************************************************/
void Adc_Para_Printf_Test(void)
{
	#if(1 == GLOBAL_PRINT_ENABLE)
	printf("V3.3 = %d, borad_temper = %d, cob_temper = %d, envir_temper = %d, Vin = %d\n", g_adc_para.Mcu_Volt, g_adc_para.borad_temper, g_adc_para.cob_temper, g_adc_para.envir_temper, g_adc_para.Vin);
	#endif
}
/***********************************END OF FILE*******************************************/
