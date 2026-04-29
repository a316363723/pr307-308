/****************************************************************************************
**  Filename :  111.c
**  Abstract :  。
**  By       :  何建国
**  Date     :  
**  Changelog:1.First Create
*****************************************************************************************/

/*****************************************************************************************
									Includes
*****************************************************************************************/
#include "data_convert.h"
/*****************************************************************************************
							  Variables definitions
*****************************************************************************************/

/*****************************************************************************************
							  Functions definitions
*****************************************************************************************/
/*****************************************************************************************
* Function Name: convert_two_version_number
* Description  : 2数字长度版本号转换
* Arguments    : 存放版本号首地址、版本号高部分数据、版本号低部分数据
* Return Value : NONE
******************************************************************************************/
void convert_two_version_number(char* p_name, uint8_t high_part, uint8_t low_part)
{
    uint8_t weight_val = 10;
    uint8_t nonzero_flag = 0;//1-此前发现过非0值
    uint8_t i = 0;
    
    while(weight_val)
    {
        //第一次发现非0值
        if(high_part / weight_val > 0 && 0 == nonzero_flag)
        {
            nonzero_flag = 1;
        }
        if(1 == nonzero_flag)
        {
            p_name[i] = high_part / weight_val + '0';
            i++;
        }
        weight_val /= 10;
        //到最后都没有非0值
        if(0 == nonzero_flag && 0 == weight_val)
        {
            p_name[i] = '0';
            i++;
        }
    }
    p_name[i++] = '.';
    weight_val = 10;
    nonzero_flag = 0;
    while(weight_val)
    {
        //第一次发现非0值
        if(low_part / weight_val > 0 && 0 == nonzero_flag)
        {
            nonzero_flag = 1;
        }
        if(1 == nonzero_flag)
        {
            p_name[i] = low_part / weight_val + '0';
            i++;
        }
        weight_val /= 10;
        //到最后都没有非0值
        if(0 == nonzero_flag && 0 == weight_val)
        {
            p_name[i] = '0';
            i++;
        }
    }
    p_name[i] = '\0';
}
/***********************************END OF FILE*******************************************/
