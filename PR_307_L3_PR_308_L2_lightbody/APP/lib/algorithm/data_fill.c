/****************************************************************************************
**  Filename :  data_fill.c
**  Abstract :  数据填充类算法处理。
**  By       :  何建国
**  Date     :  2018-12-10
**  Changelog:1.First Create
*****************************************************************************************/

/*****************************************************************************************
									Includes
*****************************************************************************************/
#include "data_fill.h"
/*****************************************************************************************
							  Variables definitions
*****************************************************************************************/

/*****************************************************************************************
							  Functions definitions
*****************************************************************************************/

/*****************************************************************************************
* Function Name: AlgorithFillBuf（字数据填充）
* Description  : 向长度为data_len的数组填充数据new_data，并且每传入一次数据，*data_sel的值
                 小于data_len的情况下会自加1.
* Arguments    : NONE
* Return Value : NONE
******************************************************************************************/
void AlgorithFillBuf(uint16_t data_len, uint32_t *p_data, uint16_t *data_sel, uint32_t new_data)
{
    p_data[*data_sel] = new_data;
    (*data_sel)++;
    if((*data_sel) >= data_len)
    {
        *data_sel = 0;
    }
}
/*****************************************************************************************
* Function Name: AlgorithHalfWordFillBuf(半字数据填充)
* Description  : 向长度为data_len的数组填充数据new_data，并且每传入一次数据，*data_sel的值
                 小于data_len的情况下会自加1.
* Arguments    : NONE
* Return Value : NONE
******************************************************************************************/
void AlgorithHalfWordFillBuf(uint16_t data_len, uint16_t *p_data, uint16_t *data_sel, uint16_t new_data)
{
    p_data[*data_sel] = new_data;
    (*data_sel)++;
    if((*data_sel) >= data_len)
    {
        *data_sel = 0;
    }
}
/*****************************************************************************************
* Function Name: AlgorithByteFillBuf(字节数据填充)
* Description  : 向长度为data_len的数组填充数据new_data，并且每传入一次数据，*data_sel的值
                 小于data_len的情况下会自加1.
* Arguments    : NONE
* Return Value : NONE
******************************************************************************************/
void AlgorithByteFillBuf(uint16_t data_len, uint8_t *p_data, uint16_t *data_sel, uint8_t new_data)
{
    p_data[*data_sel] = new_data;
    (*data_sel)++;
    if((*data_sel) >= data_len)
    {
        *data_sel = 0;
    }
}
/***********************************END OF FILE*******************************************/
