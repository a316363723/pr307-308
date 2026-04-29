/****************************************************************************************
**  Filename :  sort_algorithm.c
**  Abstract :  排序算法模块的处理函数。
**  By       :  何建国
**  Date     :  2019-01-10
**  Changelog:1.First Create
*****************************************************************************************/

/*****************************************************************************************
									Includes
*****************************************************************************************/
#include "sort_algorithm.h"
/*****************************************************************************************
							  Variables definitions
*****************************************************************************************/

/*****************************************************************************************
							  Functions definitions
*****************************************************************************************/

/*****************************************************************************************
* Function Name: Sort_insertion
* Description  : 插入排序算法处理函数
* Arguments    : 数据长度、数据首地址指针
* Return Value : NONE
******************************************************************************************/
void Sort_insertion(uint16_t len, uint32_t *p_data)
{
    uint16_t i = 0;
    uint16_t pre_index = 0;
    uint32_t curr_val = 0;

    for(i = 1; i < len; i++)
    {
        pre_index = i;
        curr_val = p_data[i];
        while(pre_index > 0 && p_data[pre_index - 1] > curr_val)
        {
            pre_index--;
            p_data[pre_index + 1] = p_data[pre_index];//数据后移
        }
        p_data[pre_index] = curr_val;
    }
}
/*****************************************************************************************
* Function Name: Sort_HalfWordinsertion
* Description  : 插入排序算法处理函数(字节数据)
* Arguments    : 数据长度、数据首地址指针
* Return Value : NONE
******************************************************************************************/
void Sort_HalfWordinsertion(uint16_t len, uint16_t *p_data)
{
    uint16_t i = 0;
    uint16_t pre_index = 0;
    uint16_t curr_val = 0;

    for(i = 1; i < len; i++)
    {
        pre_index = i;
        curr_val = p_data[i];
        while(pre_index > 0 && p_data[pre_index - 1] > curr_val)
        {
            pre_index--;
            p_data[pre_index + 1] = p_data[pre_index];//数据后移
        }
        p_data[pre_index] = curr_val;
    }
}
/*****************************************************************************************
* Function Name: Sort_Byteinsertion
* Description  : 插入排序算法处理函数(字节数据)
* Arguments    : 数据长度、数据首地址指针
* Return Value : NONE
******************************************************************************************/
void Sort_Byteinsertion(uint16_t len, uint8_t *p_data)
{
    uint16_t i = 0;
    uint16_t pre_index = 0;
    uint8_t curr_val = 0;

    for(i = 1; i < len; i++)
    {
        pre_index = i;
        curr_val = p_data[i];
        while(pre_index > 0 && p_data[pre_index - 1] > curr_val)
        {
            pre_index--;
            p_data[pre_index + 1] = p_data[pre_index];//数据后移
        }
        p_data[pre_index] = curr_val;
    }
}
/***********************************END OF FILE*******************************************/
