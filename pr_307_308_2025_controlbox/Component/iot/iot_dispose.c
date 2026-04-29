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
#include "iot.h"
#include  	<string.h>
/*****************************************************************************************
							  Variables definitions
*****************************************************************************************/
static IOT_Parameter_TypeDef s_iot_parameter_str = {0};
static IOT_Control_Way_TypeDef s_control_way = CTR_BOX_WAY;
static uint8_t s_control_change = 0;

IOT_Send_Data_TypeDef g_iot_send_data = {0};
/*****************************************************************************************
							  Functions definitions
*****************************************************************************************/
//-----------------------------------IOT数据记录
/*****************************************************************************************
* Function Name: IOT_Int_Time_TypeDef
* Description  : 根据当前的亮度得到亮度等级制；
* Arguments    : int_val:亮度值
* Return Value : NONE
******************************************************************************************/
static IOT_Int_Time_TypeDef get_int_grade_msg(uint8_t int_val)
{
    if(int_val == 0)
        return INT_TYPE1;
    else if(int_val >= 1 && int_val <= 5)
        return INT_TYPE2;
    else if(int_val >= 6 && int_val <= 10)
        return INT_TYPE3;
    else if(int_val >= 11 && int_val <= 20)
        return INT_TYPE4;
    else if(int_val >= 21 && int_val <= 30)
        return INT_TYPE5;
    else if(int_val >= 31 && int_val <= 40)
        return INT_TYPE6;
    else if(int_val >= 41 && int_val <= 50)
        return INT_TYPE7;
    else if(int_val >= 51 && int_val <= 60)
        return INT_TYPE8;
    else if(int_val >= 61 && int_val <= 70)
        return INT_TYPE9;
    else if(int_val >= 71 && int_val <= 80)
        return INT_TYPE10;
    else if(int_val >= 81 && int_val <= 90)
        return INT_TYPE11;
    else if(int_val >= 91 && int_val <= 95)
        return INT_TYPE12;
    else if(int_val >= 96 && int_val <= 99)
        return INT_TYPE13;
    else
        return INT_TYPE14;
}
/*****************************************************************************************
* Function Name: get_duration_work_time
* Description  : 获得持续工作时间分布情况
* Arguments    : device_state:设备开关机状态
* Return Value : NONE
******************************************************************************************/
static void get_duration_work_time(IOT_Device_State device_state)
{
    static uint32_t s_second_cnt = 0;
    
    #if 0
    static IOT_Device_State s_device_state = DEVICE_CLOSE;
    if(device_state != s_device_state)
    {
        s_device_state = device_state;
        //从开机状态切换到关机状态
        if(DEVICE_CLOSE == device_state)
        {
            if(s_second_cnt >= 30 && s_second_cnt <= 300)
                s_iot_parameter_str.duration_time[DURATION_TIME_TYPE1]++;
            else if(s_second_cnt > 300 && s_second_cnt <= 600)
                s_iot_parameter_str.duration_time[DURATION_TIME_TYPE2]++;
            else if(s_second_cnt > 600 && s_second_cnt <= 1200)
                s_iot_parameter_str.duration_time[DURATION_TIME_TYPE3]++;
            else if(s_second_cnt > 1200 && s_second_cnt <= 1800)
                s_iot_parameter_str.duration_time[DURATION_TIME_TYPE4]++;
            else if(s_second_cnt > 1800 && s_second_cnt <= 2400)
                s_iot_parameter_str.duration_time[DURATION_TIME_TYPE5]++;
            else if(s_second_cnt > 2400 && s_second_cnt <= 3000)
                s_iot_parameter_str.duration_time[DURATION_TIME_TYPE6]++;
            else if(s_second_cnt > 3000 && s_second_cnt <= 3600)
                s_iot_parameter_str.duration_time[DURATION_TIME_TYPE7]++;
            else if(s_second_cnt > 3600 && s_second_cnt <= 4200)
                s_iot_parameter_str.duration_time[DURATION_TIME_TYPE8]++;
            else if(s_second_cnt > 4200 && s_second_cnt <= 4800)
                s_iot_parameter_str.duration_time[DURATION_TIME_TYPE9]++;
            else if(s_second_cnt > 4800 && s_second_cnt <= 5400)
                s_iot_parameter_str.duration_time[DURATION_TIME_TYPE10]++;
            else if(s_second_cnt > 5400 && s_second_cnt <= 6000)
                s_iot_parameter_str.duration_time[DURATION_TIME_TYPE11]++;
            else if(s_second_cnt > 6000 && s_second_cnt <= 6600)
                s_iot_parameter_str.duration_time[DURATION_TIME_TYPE12]++;
            else if(s_second_cnt > 6600 && s_second_cnt <= 7200)
                s_iot_parameter_str.duration_time[DURATION_TIME_TYPE13]++;
            else if(s_second_cnt > 7200 && s_second_cnt <= 7800)
                s_iot_parameter_str.duration_time[DURATION_TIME_TYPE14]++;
            else if(s_second_cnt > 7800 && s_second_cnt <= 8400)
                s_iot_parameter_str.duration_time[DURATION_TIME_TYPE15]++;
            else if(s_second_cnt > 8400 && s_second_cnt <= 9000)
                s_iot_parameter_str.duration_time[DURATION_TIME_TYPE16]++;
            else if(s_second_cnt > 9000 && s_second_cnt <= 9600)
                s_iot_parameter_str.duration_time[DURATION_TIME_TYPE17]++;
            else if(s_second_cnt > 9600 && s_second_cnt <= 10200)
                s_iot_parameter_str.duration_time[DURATION_TIME_TYPE18]++;
            else if(s_second_cnt > 10200 && s_second_cnt <= 10800)
                s_iot_parameter_str.duration_time[DURATION_TIME_TYPE19]++;
            else if(s_second_cnt > 10800 && s_second_cnt <= 11400)
                s_iot_parameter_str.duration_time[DURATION_TIME_TYPE20]++;
            else if(s_second_cnt > 11400 && s_second_cnt <= 12000)
                s_iot_parameter_str.duration_time[DURATION_TIME_TYPE21]++;
            else if(s_second_cnt > 12000 && s_second_cnt <= 12600)
                s_iot_parameter_str.duration_time[DURATION_TIME_TYPE22]++;
            else if(s_second_cnt > 12600 && s_second_cnt <= 13200)
                s_iot_parameter_str.duration_time[DURATION_TIME_TYPE23]++;
            else if(s_second_cnt > 13200 && s_second_cnt <= 13800)
                s_iot_parameter_str.duration_time[DURATION_TIME_TYPE24]++;
            else if(s_second_cnt > 13800 && s_second_cnt <= 14400)
                s_iot_parameter_str.duration_time[DURATION_TIME_TYPE25]++;
            else if(s_second_cnt > 14400 && s_second_cnt <= 18000)
                s_iot_parameter_str.duration_time[DURATION_TIME_TYPE26]++;
            else if(s_second_cnt > 18000 && s_second_cnt <= 21600)
                s_iot_parameter_str.duration_time[DURATION_TIME_TYPE27]++;
            else if(s_second_cnt > 21600 && s_second_cnt <= 25200)
                s_iot_parameter_str.duration_time[DURATION_TIME_TYPE28]++;
            else if(s_second_cnt > 25200 && s_second_cnt <= 28800)
                s_iot_parameter_str.duration_time[DURATION_TIME_TYPE29]++;
            else if(s_second_cnt > 28800 && s_second_cnt <= 32400)
                s_iot_parameter_str.duration_time[DURATION_TIME_TYPE30]++;
            else if(s_second_cnt > 32400 && s_second_cnt < 36000)
                s_iot_parameter_str.duration_time[DURATION_TIME_TYPE31]++;
            else if(s_second_cnt > 36000)
                s_iot_parameter_str.duration_time[DURATION_TIME_TYPE32]++;
        }
        s_second_cnt = 0;
    }
    #endif
    if(DEVICE_CLOSE == device_state)
    {
        s_second_cnt = 0;
        return;
    }
    s_second_cnt++;
    
    if(31 == s_second_cnt)
    {
        s_iot_parameter_str.duration_time[DURATION_TIME_TYPE1]++;
    }
    else if(301 == s_second_cnt)
    {
        s_iot_parameter_str.duration_time[DURATION_TIME_TYPE1]--;
        s_iot_parameter_str.duration_time[DURATION_TIME_TYPE2]++;
    }
    else if(601 == s_second_cnt)
    {
        s_iot_parameter_str.duration_time[DURATION_TIME_TYPE2]--;
        s_iot_parameter_str.duration_time[DURATION_TIME_TYPE3]++;
    }
    else if(1201 == s_second_cnt)
    {
        s_iot_parameter_str.duration_time[DURATION_TIME_TYPE3]--;
        s_iot_parameter_str.duration_time[DURATION_TIME_TYPE4]++;
    }
    else if(1801 == s_second_cnt)
    {
        s_iot_parameter_str.duration_time[DURATION_TIME_TYPE4]--;
        s_iot_parameter_str.duration_time[DURATION_TIME_TYPE5]++;
    }
    else if(2401 == s_second_cnt)
    {
        s_iot_parameter_str.duration_time[DURATION_TIME_TYPE5]--;
        s_iot_parameter_str.duration_time[DURATION_TIME_TYPE6]++;
    }
    else if(3001 == s_second_cnt)
    {
        s_iot_parameter_str.duration_time[DURATION_TIME_TYPE6]--;
        s_iot_parameter_str.duration_time[DURATION_TIME_TYPE7]++;
    }
    else if(3601 == s_second_cnt)
    {
        s_iot_parameter_str.duration_time[DURATION_TIME_TYPE7]--;
        s_iot_parameter_str.duration_time[DURATION_TIME_TYPE8]++;
    }
    else if(4201 == s_second_cnt)
    {
        s_iot_parameter_str.duration_time[DURATION_TIME_TYPE8]--;
        s_iot_parameter_str.duration_time[DURATION_TIME_TYPE9]++;
    }
    else if(4801 == s_second_cnt)
    {
        s_iot_parameter_str.duration_time[DURATION_TIME_TYPE9]--;
        s_iot_parameter_str.duration_time[DURATION_TIME_TYPE10]++;
    }
    else if(5401 == s_second_cnt)
    {
        s_iot_parameter_str.duration_time[DURATION_TIME_TYPE10]--;
        s_iot_parameter_str.duration_time[DURATION_TIME_TYPE11]++;
    }
    else if(6001 == s_second_cnt)
    {
        s_iot_parameter_str.duration_time[DURATION_TIME_TYPE11]--;
        s_iot_parameter_str.duration_time[DURATION_TIME_TYPE12]++;
    }
    else if(6601 == s_second_cnt)
    {
        s_iot_parameter_str.duration_time[DURATION_TIME_TYPE12]--;
        s_iot_parameter_str.duration_time[DURATION_TIME_TYPE13]++;
    }
    else if(7201 == s_second_cnt)
    {
        s_iot_parameter_str.duration_time[DURATION_TIME_TYPE13]--;
        s_iot_parameter_str.duration_time[DURATION_TIME_TYPE14]++;
    }
    else if(7801 == s_second_cnt)
    {
        s_iot_parameter_str.duration_time[DURATION_TIME_TYPE14]--;
        s_iot_parameter_str.duration_time[DURATION_TIME_TYPE15]++;
    }
    else if(8401 == s_second_cnt)
    {
        s_iot_parameter_str.duration_time[DURATION_TIME_TYPE15]--;
        s_iot_parameter_str.duration_time[DURATION_TIME_TYPE16]++;
    }
    else if(9001 == s_second_cnt)
    {
        s_iot_parameter_str.duration_time[DURATION_TIME_TYPE16]--;
        s_iot_parameter_str.duration_time[DURATION_TIME_TYPE17]++;
    }
    else if(9601 == s_second_cnt)
    {
        s_iot_parameter_str.duration_time[DURATION_TIME_TYPE17]--;
        s_iot_parameter_str.duration_time[DURATION_TIME_TYPE18]++;
    }
    else if(10201 == s_second_cnt)
    {
        s_iot_parameter_str.duration_time[DURATION_TIME_TYPE18]--;
        s_iot_parameter_str.duration_time[DURATION_TIME_TYPE19]++;
    }
    else if(10801 == s_second_cnt)
    {
        s_iot_parameter_str.duration_time[DURATION_TIME_TYPE19]--;
        s_iot_parameter_str.duration_time[DURATION_TIME_TYPE20]++;
    }
    else if(11401 == s_second_cnt)
    {
        s_iot_parameter_str.duration_time[DURATION_TIME_TYPE20]--;
        s_iot_parameter_str.duration_time[DURATION_TIME_TYPE21]++;
    }
    else if(12001 == s_second_cnt)
    {
        s_iot_parameter_str.duration_time[DURATION_TIME_TYPE21]--;
        s_iot_parameter_str.duration_time[DURATION_TIME_TYPE22]++;
    }
    else if(12601 == s_second_cnt)
    {
        s_iot_parameter_str.duration_time[DURATION_TIME_TYPE22]--;
        s_iot_parameter_str.duration_time[DURATION_TIME_TYPE23]++;
    }
    else if(13201 == s_second_cnt)
    {
        s_iot_parameter_str.duration_time[DURATION_TIME_TYPE23]--;
        s_iot_parameter_str.duration_time[DURATION_TIME_TYPE24]++;
    }
    else if(13801 == s_second_cnt)
    {
        s_iot_parameter_str.duration_time[DURATION_TIME_TYPE24]--;
        s_iot_parameter_str.duration_time[DURATION_TIME_TYPE25]++;
    }
    else if(14401 == s_second_cnt)
    {
        s_iot_parameter_str.duration_time[DURATION_TIME_TYPE25]--;
        s_iot_parameter_str.duration_time[DURATION_TIME_TYPE26]++;
    }
    else if(18001 == s_second_cnt)
    {
        s_iot_parameter_str.duration_time[DURATION_TIME_TYPE26]--;
        s_iot_parameter_str.duration_time[DURATION_TIME_TYPE27]++;
    }
    else if(21601 == s_second_cnt)
    {
        s_iot_parameter_str.duration_time[DURATION_TIME_TYPE27]--;
        s_iot_parameter_str.duration_time[DURATION_TIME_TYPE28]++;
    }
    else if(25201 == s_second_cnt)
    {
        s_iot_parameter_str.duration_time[DURATION_TIME_TYPE28]--;
        s_iot_parameter_str.duration_time[DURATION_TIME_TYPE29]++;
    }
    else if(28801 == s_second_cnt)
    {
        s_iot_parameter_str.duration_time[DURATION_TIME_TYPE29]--;
        s_iot_parameter_str.duration_time[DURATION_TIME_TYPE30]++;
    }
    else if(32401 == s_second_cnt)
    {
        s_iot_parameter_str.duration_time[DURATION_TIME_TYPE30]--;
        s_iot_parameter_str.duration_time[DURATION_TIME_TYPE31]++;
    }
    else if(36001 == s_second_cnt)
    {
        s_iot_parameter_str.duration_time[DURATION_TIME_TYPE31]--;
        s_iot_parameter_str.duration_time[DURATION_TIME_TYPE32]++;
    }
}
/*****************************************************************************************
* Function Name: IOT_Fresnel_Time_TypeDef
* Description  : 通过自动调焦角度计算出自动调焦等级；
* Arguments    : fresnel_val：自动调焦角度
* Return Value : 返回自动调焦等级；
******************************************************************************************/
static IOT_Fresnel_Time_TypeDef get_fresnel_grade_msg(uint8_t fresnel_val)
{
    if(5 == fresnel_val)
        return FRESNEL_TYPE1;
    else if(fresnel_val >= 6 && fresnel_val <= 10)
        return FRESNEL_TYPE2;
    else if(fresnel_val >= 11 && fresnel_val <= 15)
        return FRESNEL_TYPE3;
    else if(fresnel_val >= 16 && fresnel_val <= 20)
        return FRESNEL_TYPE4;
    else if(fresnel_val >= 21 && fresnel_val <= 25)
        return FRESNEL_TYPE5;
    else if(fresnel_val >= 26 && fresnel_val <= 30)
        return FRESNEL_TYPE6;
    else if(fresnel_val >= 31 && fresnel_val <= 35)
        return FRESNEL_TYPE7;
    else if(fresnel_val >= 36 && fresnel_val <= 40)
        return FRESNEL_TYPE8;
    else if(fresnel_val >= 41 && fresnel_val <= 45)
        return FRESNEL_TYPE9;
    else if(fresnel_val >= 46 && fresnel_val <= 50)
        return FRESNEL_TYPE10;
    else if(fresnel_val >= 51 && fresnel_val <= 54)
        return FRESNEL_TYPE11;
    else if(55 == fresnel_val)
        return FRESNEL_TYPE12;
    else 
        return NO_FRESNEL;
}
/*****************************************************************************************
* Function Name: get_pitch_angle_grade_msg
* Description  : 通过自动俯仰角度计算出自动俯仰等级；
* Arguments    : pitch_angle_val：自动俯仰角度
* Return Value : 返回自动俯仰等级；
******************************************************************************************/
static IOT_Pitch_Angle_Time_TypeDef get_pitch_angle_grade_msg(int8_t pitch_angle_val)
{
    if(-60 == pitch_angle_val)
        return PITCH_ANGLE_TYPE1;
    else if(pitch_angle_val >= -59 && pitch_angle_val <= -50)
        return PITCH_ANGLE_TYPE2;
    else if(pitch_angle_val >= -49 && pitch_angle_val <= -40)
        return PITCH_ANGLE_TYPE3;
    else if(pitch_angle_val >= -39 && pitch_angle_val <= -30)
        return PITCH_ANGLE_TYPE4;
    else if(pitch_angle_val >= -29 && pitch_angle_val <= -10)
        return PITCH_ANGLE_TYPE5;
    else if(pitch_angle_val >= -19 && pitch_angle_val <= -10)
        return PITCH_ANGLE_TYPE6;
    else if(pitch_angle_val >= -9 && pitch_angle_val <= -1)
        return PITCH_ANGLE_TYPE7;
    else if(0 == pitch_angle_val)
        return PITCH_ANGLE_TYPE8;
    else if(pitch_angle_val >= 1 && pitch_angle_val <= 10)
        return PITCH_ANGLE_TYPE9;
    else if(pitch_angle_val >= 11 && pitch_angle_val <= 20)
        return PITCH_ANGLE_TYPE10;
    else if(pitch_angle_val >= 21 && pitch_angle_val <= 30)
        return PITCH_ANGLE_TYPE11;
    else if(pitch_angle_val >= 31 && pitch_angle_val <= 40)
        return PITCH_ANGLE_TYPE12;
    else if(pitch_angle_val >= 41 && pitch_angle_val <= 50)
        return PITCH_ANGLE_TYPE13;
    else if(pitch_angle_val >= 51 && pitch_angle_val <= 60)
        return PITCH_ANGLE_TYPE14;
    else if(pitch_angle_val >= 61 && pitch_angle_val <= 70)
        return PITCH_ANGLE_TYPE15;
    else if(pitch_angle_val >= 71 && pitch_angle_val <= 80)
        return PITCH_ANGLE_TYPE16;
    else if(pitch_angle_val >= 81 && pitch_angle_val <= 89)
        return PITCH_ANGLE_TYPE17;
    else if(90 == pitch_angle_val)
        return PITCH_ANGLE_TYPE18; 
    else 
        return NO_PITCH_ANGLE;
}
/*****************************************************************************************
* Function Name: count_control_way
* Description  : 处理设备控制方式的计数值
* Arguments    : NONE
* Return Value : NONE
******************************************************************************************/
#define CONTROL_WAY_VERIFY_TIM      5
static void count_control_way(void)
{
    static uint8_t tim_cnt = CONTROL_WAY_VERIFY_TIM;
    
    if(1 == s_control_change)
    {
        s_control_change = 0;
        tim_cnt = 0;
    }
    if(tim_cnt < CONTROL_WAY_VERIFY_TIM)
    {
        tim_cnt++;
        if(tim_cnt >= CONTROL_WAY_VERIFY_TIM)
        {
            s_iot_parameter_str.control_way[s_control_way]++;
        }
    }
}
/*****************************************************************************************
* Function Name: dispose_iot_parameter_data
* Description  : 处理IOT数据（1秒轮询）
* Arguments    : p_parameter_serial：参数序号结构体指针
* Return Value : NONE
******************************************************************************************/
void dispose_iot_parameter_data(IOT_Parameter_Serial_TypeDef* p_parameter_serial)
{
    static uint8_t tim_cnt = 0;
    
    tim_cnt++;
    if(tim_cnt > 59)
    {
        tim_cnt = 0;
        s_iot_parameter_str.device_state_time[p_parameter_serial->device_state_serial]++;//设备开关状态
        if(DEVICE_OPEN == p_parameter_serial->device_state_serial)
        {
            s_iot_parameter_str.run_time[p_parameter_serial->run_time_serial]++;//供电运行时间累加
            s_iot_parameter_str.mode_time[p_parameter_serial->mode_time_serial]++;//模式时间累加
            s_iot_parameter_str.int_time[get_int_grade_msg(p_parameter_serial->int_val)]++;//亮度时间累加
            s_iot_parameter_str.fresnel_time[get_fresnel_grade_msg(p_parameter_serial->fresnel_val)]++;//自动调焦时间累加
            s_iot_parameter_str.pitch_angle_time[get_pitch_angle_grade_msg(p_parameter_serial->pitch_angle_val)]++;//自动俯仰角时间累加
            if(SIDUSPRO_FX_MODE_TIME == p_parameter_serial->mode_time_serial)
                s_iot_parameter_str.sidus_pro_fx[p_parameter_serial->sidus_pro_fx_serial]++;//非系统光效时间累加
        }
    }
    get_duration_work_time(p_parameter_serial->device_state_serial);
    count_control_way();
}
/*****************************************************************************************
* Function Name: set_iot_error_data
* Description  : 设置IOT故障数据，对应故障产生一次，调用一次次函数；
* Arguments    : error_msg：故障信息
* Return Value : NONE
******************************************************************************************/
void set_iot_error_data(IOT_Error_Msg_TypeDef error_msg)
{
    s_iot_parameter_str.error_msg[error_msg]++;
}
/*****************************************************************************************
* Function Name: set_ctr_way_msg
* Description  : 设置控制方式变更信息，每次设置结束后调用此函数（结束的标志是结束调节后间隔一段时间）；
* Arguments    : control_way_serial：控制方式
* Return Value : NONE
******************************************************************************************/
void set_ctr_way_msg(IOT_Control_Way_TypeDef control_way_serial)
{
    s_control_way = control_way_serial;
    s_control_change = 1;
}
//-----------------------------------IOT数据存储
/*****************************************************************************************
* Function Name: calculate_byte_xor_sum
* Description  : 计算以buf为起始地址的len个字节与异或因子异或后的累加和，结果存入字节变量sum中.
* Arguments    : 数据长度、数据指针和异或量
* Return Value : NONE
******************************************************************************************/
static uint32_t calculate_byte_xor_sum(uint16_t len, uint8_t *buf, uint8_t xor_factor)
{
    uint32_t sum = 0;
    uint16_t i = 0;
    for(i = 0; i < len; i++)
    {
        sum = sum + (buf[i] ^ xor_factor);
    }
    return sum;
}
/*****************************************************************************************
* Function Name: get_iot_data
* Description  : 需要存储时，获取IOT数据
* Arguments    : 应用层数据首地址
* Return Value : NONE
******************************************************************************************/
void get_iot_data(uint8_t* p_iot_data)
{
    s_iot_parameter_str.check_sum = calculate_byte_xor_sum(sizeof(IOT_Parameter_TypeDef) - 4, 
                                                           (uint8_t*)&s_iot_parameter_str, 0X55);
    memcpy(p_iot_data, &s_iot_parameter_str, sizeof(s_iot_parameter_str));
}
/*****************************************************************************************
* Function Name: recover_iot_data
* Description  : 上电后，恢复IOT数据；
* Arguments    : 数据首地址
* Return Value : NONE
******************************************************************************************/
void recover_iot_data(uint8_t* p_iot_data)
{
    uint32_t check_sum = 0;
    IOT_Parameter_TypeDef* p_iot = (IOT_Parameter_TypeDef*)p_iot_data;
    
    check_sum = p_iot->check_sum;
    if(check_sum == calculate_byte_xor_sum(sizeof(IOT_Parameter_TypeDef) - 4, p_iot_data, 0X55))
    {
        memcpy(&s_iot_parameter_str, p_iot_data, sizeof(s_iot_parameter_str));
    }
}
//-----------------------------------IOT数据转换
/*****************************************************************************************
* Function Name: WordSum
* Description  : 计算以buf为起始地址的len个字的累加和，结果存入字节变量sum中().
* Arguments    : NONE
* Return Value : NONE
******************************************************************************************/
static uint32_t WordSum(uint16_t len, uint32_t *buf)
{
    uint32_t sum = 0;
    uint16_t i = 0;
    for(i = 0; i < len; i++)
    {
        sum += buf[i];
    }
    return sum;
}
uint8_t CheckSum_Get(uint8_t *data, uint8_t num)
{ 
	uint8_t CheckSum = 0;

	while(num--)
	{
		CheckSum += *data++;
	}

	return CheckSum;    
}
/*****************************************************************************************
* Function Name: package_iot_send_data
* Description  : 获得发送数据包；
* Arguments    : NONE
* Return Value : NONE
******************************************************************************************/
static void package_iot_send_data(void)
{
    uint32_t tem_variable1 = 0;
    uint32_t tem_variable2 = 0;
    uint32_t tem_variable3 = 0;
    //------------------------
    tem_variable1 = WordSum(MAX_STATE_SERIAL, (uint32_t*)s_iot_parameter_str.device_state_time);//暂存总运行时间
    tem_variable2 = s_iot_parameter_str.device_state_time[DEVICE_OPEN]; //暂存总开机时间
    
    g_iot_send_data.Run_Tim1.Header.Opera_Type = 0x00;
    g_iot_send_data.Run_Tim1.Header.Command_Type = IOT_COMMAND_TYPE;
    g_iot_send_data.Run_Tim1.IOT_Type = 0x03;
    g_iot_send_data.Run_Tim1.Rum_Tim = tem_variable1;
    
    tem_variable1 = tem_variable1 == 0 ? 1 : tem_variable1;
    tem_variable2 = tem_variable2 == 0 ? 1 : tem_variable2;
    g_iot_send_data.Run_Tim1.Work_Tim_Duty = 1000 * s_iot_parameter_str.device_state_time[DEVICE_OPEN] / tem_variable1;
    g_iot_send_data.Run_Tim1.Off_Tim_Duty = 1000 - g_iot_send_data.Run_Tim1.Work_Tim_Duty;
    g_iot_send_data.Run_Tim1.Use_DC_Duty = 1000 * s_iot_parameter_str.run_time[ADAPTER_SUPPLY_TIME] / tem_variable2;
    g_iot_send_data.Run_Tim1.CheckSum = CheckSum_Get((uint8_t*)(&g_iot_send_data.Run_Tim1.CheckSum) + 1, IOT_PACKET_SIZE - 1);
    
    tem_variable1 = 1000 - g_iot_send_data.Run_Tim1.Use_DC_Duty;    //暂存电池使用千分比
    tem_variable3 = tem_variable2 * tem_variable1 / 1000;//电池总供电时间
    tem_variable3 = tem_variable3 == 0 ? 1 : tem_variable3;
    
    //BAT5 6
    g_iot_send_data.Run_Tim3.Header.Opera_Type = 0x00;
    g_iot_send_data.Run_Tim3.Header.Command_Type = IOT_COMMAND_TYPE;
    g_iot_send_data.Run_Tim3.IOT_Type = 0x05;
    g_iot_send_data.Run_Tim3.BAT5_Duty = 1000 * s_iot_parameter_str.run_time[FIVE_BAT_SUPPLY_TIME] / tem_variable3;
    g_iot_send_data.Run_Tim3.BAT6_Duty = 1000 * s_iot_parameter_str.run_time[SIX_BAT_SUPPLY_TIME] / tem_variable3;
    g_iot_send_data.Run_Tim3.CheckSum = CheckSum_Get((uint8_t*)(&g_iot_send_data.Run_Tim3.CheckSum) + 1, IOT_PACKET_SIZE - 1);
    //BAT 1 - 4
    g_iot_send_data.Run_Tim2.Header.Opera_Type = 0x00;
    g_iot_send_data.Run_Tim2.Header.Command_Type = IOT_COMMAND_TYPE;
    g_iot_send_data.Run_Tim2.IOT_Type = 0x04;
    g_iot_send_data.Run_Tim2.Use_BAT_Duty = tem_variable1;
    g_iot_send_data.Run_Tim2.BAT1_Duty = 1000 * s_iot_parameter_str.run_time[ONE_BAT_SUPPLY_TIME] / tem_variable3;
    g_iot_send_data.Run_Tim2.BAT2_Duty = 1000 * s_iot_parameter_str.run_time[TWO_BAT_SUPPLY_TIME] / tem_variable3;
    g_iot_send_data.Run_Tim2.BAT3_Duty = 1000 * s_iot_parameter_str.run_time[THREE_BAT_SUPPLY_TIME] / tem_variable3;
    g_iot_send_data.Run_Tim2.BAT4_Duty = 1000 * s_iot_parameter_str.run_time[FOUR_BAT_SUPPLY_TIME] / tem_variable3;
    tem_variable3 = g_iot_send_data.Run_Tim2.BAT1_Duty + g_iot_send_data.Run_Tim2.BAT2_Duty + g_iot_send_data.Run_Tim2.BAT3_Duty + \
                    g_iot_send_data.Run_Tim2.BAT4_Duty + g_iot_send_data.Run_Tim3.BAT5_Duty + g_iot_send_data.Run_Tim3.BAT6_Duty;
    if(tem_variable3 < 1000 && tem_variable3 != 0)
        g_iot_send_data.Run_Tim2.BAT1_Duty += (1000 - tem_variable3);
    g_iot_send_data.Run_Tim2.CheckSum = CheckSum_Get((uint8_t*)(&g_iot_send_data.Run_Tim2.CheckSum) + 1, IOT_PACKET_SIZE - 1);
    
    
    
    
    //------------------------
    tem_variable1 = WordSum(MAX_MODE_TIME_TYPE, (uint32_t*)s_iot_parameter_str.mode_time);
    
    g_iot_send_data.Mode_Tim1.Header.Opera_Type = 0x00;
    g_iot_send_data.Mode_Tim1.Header.Command_Type = IOT_COMMAND_TYPE;
    g_iot_send_data.Mode_Tim1.IOT_Type = 0x06;
    g_iot_send_data.Mode_Tim1.Work_Tim = tem_variable1;
    tem_variable1 = tem_variable1 == 0 ? 1 : tem_variable1;
    g_iot_send_data.Mode_Tim1.DMX_Duty = 1000 * s_iot_parameter_str.mode_time[DMX_MODE_TIME] / tem_variable1;
    g_iot_send_data.Mode_Tim1.CCT_Duty = 1000 * s_iot_parameter_str.mode_time[CCT_MODE_TIME] / tem_variable1;
    g_iot_send_data.Mode_Tim1.GEL_Duty = 1000 * s_iot_parameter_str.mode_time[GEL_MODE_TIME] / tem_variable1;
    g_iot_send_data.Mode_Tim1.CheckSum = CheckSum_Get((uint8_t*)(&g_iot_send_data.Mode_Tim1.CheckSum) + 1, IOT_PACKET_SIZE - 1);
    
    g_iot_send_data.Mode_Tim2.Header.Opera_Type = 0x00;
    g_iot_send_data.Mode_Tim2.Header.Command_Type = IOT_COMMAND_TYPE;
    g_iot_send_data.Mode_Tim2.IOT_Type = 0x07;
    g_iot_send_data.Mode_Tim2.HSI_Duty = 1000 * s_iot_parameter_str.mode_time[HSI_MODE_TIME] / tem_variable1;
    g_iot_send_data.Mode_Tim2.XY_Duty = 1000 * s_iot_parameter_str.mode_time[XY_MODE_TIME] / tem_variable1;
    g_iot_send_data.Mode_Tim2.RGB_Duty = 1000 * s_iot_parameter_str.mode_time[RGB_MODE_TIME] / tem_variable1;
    g_iot_send_data.Mode_Tim2.SFX_Duty = 1000 * s_iot_parameter_str.mode_time[SFX_MODE_TIME] / tem_variable1;
    g_iot_send_data.Mode_Tim2.SidusPro_FX_Duty = 1000 * s_iot_parameter_str.mode_time[SIDUSPRO_FX_MODE_TIME] / tem_variable1;
    tem_variable2 = 1000 - g_iot_send_data.Mode_Tim1.DMX_Duty - g_iot_send_data.Mode_Tim1.CCT_Duty - g_iot_send_data.Mode_Tim1.GEL_Duty - \
                    g_iot_send_data.Mode_Tim2.HSI_Duty - g_iot_send_data.Mode_Tim2.XY_Duty - g_iot_send_data.Mode_Tim2.RGB_Duty - \
                    g_iot_send_data.Mode_Tim2.SidusPro_FX_Duty;
    //--h
    if(1000 != tem_variable2 || 0 != g_iot_send_data.Mode_Tim2.SFX_Duty)
        g_iot_send_data.Mode_Tim2.SFX_Duty = tem_variable2;
    g_iot_send_data.Mode_Tim2.CheckSum = CheckSum_Get((uint8_t*)(&g_iot_send_data.Mode_Tim2.CheckSum) + 1, IOT_PACKET_SIZE - 1);
    //------------------------
    tem_variable1 = s_iot_parameter_str.device_state_time[DEVICE_OPEN];
    tem_variable1 = tem_variable1 == 0 ? 1 : tem_variable1;
    g_iot_send_data.INT_Tim1.Header.Opera_Type = 0x00;
    g_iot_send_data.INT_Tim1.Header.Command_Type = IOT_COMMAND_TYPE;
    g_iot_send_data.INT_Tim1.IOT_Type = 0x08;
    g_iot_send_data.INT_Tim1.INT1_Duty = 1000 * s_iot_parameter_str.int_time[INT_TYPE1] / tem_variable1;
    g_iot_send_data.INT_Tim1.INT2_Duty = 1000 * s_iot_parameter_str.int_time[INT_TYPE2] / tem_variable1;
    g_iot_send_data.INT_Tim1.INT3_Duty = 1000 * s_iot_parameter_str.int_time[INT_TYPE3] / tem_variable1;
    g_iot_send_data.INT_Tim1.INT4_Duty = 1000 * s_iot_parameter_str.int_time[INT_TYPE4] / tem_variable1;
    g_iot_send_data.INT_Tim1.INT5_Duty = 1000 * s_iot_parameter_str.int_time[INT_TYPE5] / tem_variable1;
    g_iot_send_data.INT_Tim1.CheckSum = CheckSum_Get((uint8_t*)(&g_iot_send_data.INT_Tim1.CheckSum) + 1, IOT_PACKET_SIZE - 1);
    
    tem_variable2 = 1000 - g_iot_send_data.INT_Tim1.INT1_Duty - g_iot_send_data.INT_Tim1.INT2_Duty - g_iot_send_data.INT_Tim1.INT3_Duty - \
                    g_iot_send_data.INT_Tim1.INT4_Duty - g_iot_send_data.INT_Tim1.INT5_Duty;
    g_iot_send_data.INT_Tim2.Header.Opera_Type = 0x00;
    g_iot_send_data.INT_Tim2.Header.Command_Type = IOT_COMMAND_TYPE;
    g_iot_send_data.INT_Tim2.IOT_Type = 0x09;
    g_iot_send_data.INT_Tim2.INT6_Duty = 1000 * s_iot_parameter_str.int_time[INT_TYPE6] / tem_variable1;
    g_iot_send_data.INT_Tim2.INT7_Duty = 1000 * s_iot_parameter_str.int_time[INT_TYPE7] / tem_variable1;
    g_iot_send_data.INT_Tim2.INT8_Duty = 1000 * s_iot_parameter_str.int_time[INT_TYPE8] / tem_variable1;
    g_iot_send_data.INT_Tim2.INT9_Duty = 1000 * s_iot_parameter_str.int_time[INT_TYPE9] / tem_variable1;
    g_iot_send_data.INT_Tim2.INT10_Duty = 1000 * s_iot_parameter_str.int_time[INT_TYPE10] / tem_variable1;
    g_iot_send_data.INT_Tim2.CheckSum = CheckSum_Get((uint8_t*)(&g_iot_send_data.INT_Tim2.CheckSum) + 1, IOT_PACKET_SIZE - 1);
    
    tem_variable2 = tem_variable2 - g_iot_send_data.INT_Tim2.INT6_Duty - g_iot_send_data.INT_Tim2.INT7_Duty - g_iot_send_data.INT_Tim2.INT8_Duty - \
                    g_iot_send_data.INT_Tim2.INT9_Duty - g_iot_send_data.INT_Tim2.INT10_Duty;
    g_iot_send_data.INT_Tim3.Header.Opera_Type = 0x00;
    g_iot_send_data.INT_Tim3.Header.Command_Type = IOT_COMMAND_TYPE;
    g_iot_send_data.INT_Tim3.IOT_Type = 0x0A;
    g_iot_send_data.INT_Tim3.INT11_Duty = 1000 * s_iot_parameter_str.int_time[INT_TYPE11] / tem_variable1;
    g_iot_send_data.INT_Tim3.INT12_Duty = 1000 * s_iot_parameter_str.int_time[INT_TYPE12] / tem_variable1;
    g_iot_send_data.INT_Tim3.INT13_Duty = 1000 * s_iot_parameter_str.int_time[INT_TYPE13] / tem_variable1;
    g_iot_send_data.INT_Tim3.INT14_Duty = 1000 * s_iot_parameter_str.int_time[INT_TYPE14] / tem_variable1; 
    tem_variable2 = tem_variable2 - g_iot_send_data.INT_Tim3.INT11_Duty - g_iot_send_data.INT_Tim3.INT12_Duty - \
                    g_iot_send_data.INT_Tim3.INT13_Duty;
    //--h                
    if(1000 != tem_variable2 || 0 != g_iot_send_data.INT_Tim3.INT14_Duty)                
        g_iot_send_data.INT_Tim3.INT14_Duty = tem_variable2;
    g_iot_send_data.INT_Tim3.CheckSum = CheckSum_Get((uint8_t*)(&g_iot_send_data.INT_Tim3.CheckSum) + 1, IOT_PACKET_SIZE - 1);
    //------------------------
    tem_variable1 = WordSum(MAX_DURATION_TIME_TYPE, (uint32_t*)s_iot_parameter_str.duration_time);
    
    g_iot_send_data.Duration_Tim1.Header.Opera_Type = 0x00;
    g_iot_send_data.Duration_Tim1.Header.Command_Type = IOT_COMMAND_TYPE;
    g_iot_send_data.Duration_Tim1.IOT_Type = 0x0B;
    g_iot_send_data.Duration_Tim1.Work_Times = tem_variable1;
    tem_variable1 = tem_variable1 == 0 ? 1 : tem_variable1;
    g_iot_send_data.Duration_Tim1.Duration_Time1_Duty = 1000 * s_iot_parameter_str.duration_time[DURATION_TIME_TYPE1] / tem_variable1;
    g_iot_send_data.Duration_Tim1.Duration_Time2_Duty = 1000 * s_iot_parameter_str.duration_time[DURATION_TIME_TYPE2] / tem_variable1;
    g_iot_send_data.Duration_Tim1.Duration_Time3_Duty = 1000 * s_iot_parameter_str.duration_time[DURATION_TIME_TYPE3] / tem_variable1;
    g_iot_send_data.Duration_Tim1.CheckSum = CheckSum_Get((uint8_t*)(&g_iot_send_data.Duration_Tim1.CheckSum) + 1, IOT_PACKET_SIZE - 1);
    
    tem_variable2 = 1000 - g_iot_send_data.Duration_Tim1.Duration_Time1_Duty - g_iot_send_data.Duration_Tim1.Duration_Time2_Duty - g_iot_send_data.Duration_Tim1.Duration_Time3_Duty;
    g_iot_send_data.Duration_Tim2.Header.Opera_Type = 0x00;
    g_iot_send_data.Duration_Tim2.Header.Command_Type = IOT_COMMAND_TYPE;
    g_iot_send_data.Duration_Tim2.IOT_Type = 0x0C;
    g_iot_send_data.Duration_Tim2.Duration_Time4_Duty = 1000 * s_iot_parameter_str.duration_time[DURATION_TIME_TYPE4] / tem_variable1;
    g_iot_send_data.Duration_Tim2.Duration_Time5_Duty = 1000 * s_iot_parameter_str.duration_time[DURATION_TIME_TYPE5] / tem_variable1;
    g_iot_send_data.Duration_Tim2.Duration_Time6_Duty = 1000 * s_iot_parameter_str.duration_time[DURATION_TIME_TYPE6] / tem_variable1;
    g_iot_send_data.Duration_Tim2.Duration_Time7_Duty = 1000 * s_iot_parameter_str.duration_time[DURATION_TIME_TYPE7] / tem_variable1;
    g_iot_send_data.Duration_Tim2.Duration_Time8_Duty = 1000 * s_iot_parameter_str.duration_time[DURATION_TIME_TYPE8] / tem_variable1;
    g_iot_send_data.Duration_Tim2.CheckSum = CheckSum_Get((uint8_t*)(&g_iot_send_data.Duration_Tim2.CheckSum) + 1, IOT_PACKET_SIZE - 1);
    
    tem_variable2 = tem_variable2 - g_iot_send_data.Duration_Tim2.Duration_Time4_Duty - g_iot_send_data.Duration_Tim2.Duration_Time5_Duty - \
                    g_iot_send_data.Duration_Tim2.Duration_Time6_Duty - g_iot_send_data.Duration_Tim2.Duration_Time7_Duty - \
                    g_iot_send_data.Duration_Tim2.Duration_Time8_Duty;
    g_iot_send_data.Duration_Tim3.Header.Opera_Type = 0x00;
    g_iot_send_data.Duration_Tim3.Header.Command_Type = IOT_COMMAND_TYPE;
    g_iot_send_data.Duration_Tim3.IOT_Type = 0x0D;
    g_iot_send_data.Duration_Tim3.Duration_Time9_Duty = 1000 * s_iot_parameter_str.duration_time[DURATION_TIME_TYPE9] / tem_variable1;
    g_iot_send_data.Duration_Tim3.Duration_Time10_Duty = 1000 * s_iot_parameter_str.duration_time[DURATION_TIME_TYPE10] / tem_variable1;
    g_iot_send_data.Duration_Tim3.Duration_Time11_Duty = 1000 * s_iot_parameter_str.duration_time[DURATION_TIME_TYPE11] / tem_variable1;
    g_iot_send_data.Duration_Tim3.Duration_Time12_Duty = 1000 * s_iot_parameter_str.duration_time[DURATION_TIME_TYPE12] / tem_variable1;
    g_iot_send_data.Duration_Tim3.Duration_Time13_Duty = 1000 * s_iot_parameter_str.duration_time[DURATION_TIME_TYPE13] / tem_variable1;
    g_iot_send_data.Duration_Tim3.CheckSum = CheckSum_Get((uint8_t*)(&g_iot_send_data.Duration_Tim3.CheckSum) + 1, IOT_PACKET_SIZE - 1);
    
    tem_variable2 = tem_variable2 - g_iot_send_data.Duration_Tim3.Duration_Time9_Duty - g_iot_send_data.Duration_Tim3.Duration_Time10_Duty - \
                    g_iot_send_data.Duration_Tim3.Duration_Time11_Duty - g_iot_send_data.Duration_Tim3.Duration_Time12_Duty - \
                    g_iot_send_data.Duration_Tim3.Duration_Time13_Duty;
    g_iot_send_data.Duration_Tim4.Header.Opera_Type = 0x00;
    g_iot_send_data.Duration_Tim4.Header.Command_Type = IOT_COMMAND_TYPE;
    g_iot_send_data.Duration_Tim4.IOT_Type = 0x0E;
    g_iot_send_data.Duration_Tim4.Duration_Time14_Duty = 1000 * s_iot_parameter_str.duration_time[DURATION_TIME_TYPE14] / tem_variable1;
    g_iot_send_data.Duration_Tim4.Duration_Time15_Duty = 1000 * s_iot_parameter_str.duration_time[DURATION_TIME_TYPE15] / tem_variable1;
    g_iot_send_data.Duration_Tim4.Duration_Time16_Duty = 1000 * s_iot_parameter_str.duration_time[DURATION_TIME_TYPE16] / tem_variable1;
    g_iot_send_data.Duration_Tim4.Duration_Time17_Duty = 1000 * s_iot_parameter_str.duration_time[DURATION_TIME_TYPE17] / tem_variable1;
    g_iot_send_data.Duration_Tim4.Duration_Time18_Duty = 1000 * s_iot_parameter_str.duration_time[DURATION_TIME_TYPE18] / tem_variable1;
    g_iot_send_data.Duration_Tim4.CheckSum = CheckSum_Get((uint8_t*)(&g_iot_send_data.Duration_Tim4.CheckSum) + 1, IOT_PACKET_SIZE - 1);
    
    tem_variable2 = tem_variable2 - g_iot_send_data.Duration_Tim4.Duration_Time14_Duty - g_iot_send_data.Duration_Tim4.Duration_Time15_Duty - \
                    g_iot_send_data.Duration_Tim4.Duration_Time16_Duty - g_iot_send_data.Duration_Tim4.Duration_Time17_Duty - \
                    g_iot_send_data.Duration_Tim4.Duration_Time18_Duty;
    g_iot_send_data.Duration_Tim5.Header.Opera_Type = 0x00;
    g_iot_send_data.Duration_Tim5.Header.Command_Type = IOT_COMMAND_TYPE;
    g_iot_send_data.Duration_Tim5.IOT_Type = 0x0F;
    g_iot_send_data.Duration_Tim5.Duration_Time19_Duty = 1000 * s_iot_parameter_str.duration_time[DURATION_TIME_TYPE19] / tem_variable1;
    g_iot_send_data.Duration_Tim5.Duration_Time20_Duty = 1000 * s_iot_parameter_str.duration_time[DURATION_TIME_TYPE20] / tem_variable1;
    g_iot_send_data.Duration_Tim5.Duration_Time21_Duty = 1000 * s_iot_parameter_str.duration_time[DURATION_TIME_TYPE21] / tem_variable1;
    g_iot_send_data.Duration_Tim5.Duration_Time22_Duty = 1000 * s_iot_parameter_str.duration_time[DURATION_TIME_TYPE22] / tem_variable1;
    g_iot_send_data.Duration_Tim5.Duration_Time23_Duty = 1000 * s_iot_parameter_str.duration_time[DURATION_TIME_TYPE23] / tem_variable1;
    g_iot_send_data.Duration_Tim5.CheckSum = CheckSum_Get((uint8_t*)(&g_iot_send_data.Duration_Tim5.CheckSum) + 1, IOT_PACKET_SIZE - 1);
    
    tem_variable2 = tem_variable2 - g_iot_send_data.Duration_Tim5.Duration_Time19_Duty - g_iot_send_data.Duration_Tim5.Duration_Time20_Duty - \
                    g_iot_send_data.Duration_Tim5.Duration_Time21_Duty - g_iot_send_data.Duration_Tim5.Duration_Time22_Duty - \
                    g_iot_send_data.Duration_Tim5.Duration_Time23_Duty;
    g_iot_send_data.Duration_Tim6.Header.Opera_Type = 0x00;
    g_iot_send_data.Duration_Tim6.Header.Command_Type = IOT_COMMAND_TYPE;
    g_iot_send_data.Duration_Tim6.IOT_Type = 0x10;
    g_iot_send_data.Duration_Tim6.Duration_Time24_Duty = 1000 * s_iot_parameter_str.duration_time[DURATION_TIME_TYPE24] / tem_variable1;
    g_iot_send_data.Duration_Tim6.Duration_Time25_Duty = 1000 * s_iot_parameter_str.duration_time[DURATION_TIME_TYPE25] / tem_variable1;
    g_iot_send_data.Duration_Tim6.Duration_Time26_Duty = 1000 * s_iot_parameter_str.duration_time[DURATION_TIME_TYPE26] / tem_variable1;
    g_iot_send_data.Duration_Tim6.Duration_Time27_Duty = 1000 * s_iot_parameter_str.duration_time[DURATION_TIME_TYPE27] / tem_variable1;
    g_iot_send_data.Duration_Tim6.Duration_Time28_Duty = 1000 * s_iot_parameter_str.duration_time[DURATION_TIME_TYPE28] / tem_variable1;
    g_iot_send_data.Duration_Tim6.CheckSum = CheckSum_Get((uint8_t*)(&g_iot_send_data.Duration_Tim6.CheckSum) + 1, IOT_PACKET_SIZE - 1);
    
    tem_variable2 = tem_variable2 - g_iot_send_data.Duration_Tim6.Duration_Time24_Duty - g_iot_send_data.Duration_Tim6.Duration_Time25_Duty - \
                    g_iot_send_data.Duration_Tim6.Duration_Time26_Duty - g_iot_send_data.Duration_Tim6.Duration_Time27_Duty - \
                    g_iot_send_data.Duration_Tim6.Duration_Time28_Duty;
    g_iot_send_data.Duration_Tim7.Header.Opera_Type = 0x00;
    g_iot_send_data.Duration_Tim7.Header.Command_Type = IOT_COMMAND_TYPE;
    g_iot_send_data.Duration_Tim7.IOT_Type = 0x11;
    g_iot_send_data.Duration_Tim7.Duration_Time29_Duty = 1000 * s_iot_parameter_str.duration_time[DURATION_TIME_TYPE29] / tem_variable1;
    g_iot_send_data.Duration_Tim7.Duration_Time30_Duty = 1000 * s_iot_parameter_str.duration_time[DURATION_TIME_TYPE30] / tem_variable1;
    g_iot_send_data.Duration_Tim7.Duration_Time31_Duty = 1000 * s_iot_parameter_str.duration_time[DURATION_TIME_TYPE31] / tem_variable1;
    g_iot_send_data.Duration_Tim7.Duration_Time32_Duty = 1000 * s_iot_parameter_str.duration_time[DURATION_TIME_TYPE32] / tem_variable1; 
    tem_variable2 = tem_variable2 - g_iot_send_data.Duration_Tim7.Duration_Time29_Duty - g_iot_send_data.Duration_Tim7.Duration_Time30_Duty - \
                    g_iot_send_data.Duration_Tim7.Duration_Time31_Duty;
    //--h                
    g_iot_send_data.Duration_Tim7.CheckSum = CheckSum_Get((uint8_t*)(&g_iot_send_data.Duration_Tim7.CheckSum) + 1, IOT_PACKET_SIZE - 1);
    //校准值归于第一个参数
    if(tem_variable2 <= DURATION_TIME_TYPE32) 
    {        
        g_iot_send_data.Duration_Tim1.Duration_Time1_Duty += tem_variable2;
        g_iot_send_data.Duration_Tim1.CheckSum = CheckSum_Get((uint8_t*)(&g_iot_send_data.Duration_Tim1.CheckSum) + 1, IOT_PACKET_SIZE - 1);
    }
    //------------------------
    tem_variable1 = WordSum(MAX_CTR_WAY, (uint32_t*)s_iot_parameter_str.control_way);
    
    g_iot_send_data.Control_Times.Header.Opera_Type = 0x00;
    g_iot_send_data.Control_Times.Header.Command_Type = IOT_COMMAND_TYPE;
    g_iot_send_data.Control_Times.IOT_Type = 0x12;
    g_iot_send_data.Control_Times.Control_Times = tem_variable1;
    tem_variable1 = tem_variable1 == 0 ? 1 : tem_variable1;
    g_iot_send_data.Control_Times.Control_Box_Duty = 1000 * s_iot_parameter_str.control_way[CTR_BOX_WAY] / tem_variable1;
    g_iot_send_data.Control_Times.Ble_Ctr_Duty = 1000 * s_iot_parameter_str.control_way[CTR_BLE_WAY] / tem_variable1;
    //--h
    if(g_iot_send_data.Control_Times.Control_Box_Duty != 0 || g_iot_send_data.Control_Times.Ble_Ctr_Duty != 0)
        g_iot_send_data.Control_Times.Ble_Ctr_Duty = 1000 - g_iot_send_data.Control_Times.Control_Box_Duty;
    g_iot_send_data.Control_Times.CheckSum = CheckSum_Get((uint8_t*)(&g_iot_send_data.Control_Times.CheckSum) + 1, IOT_PACKET_SIZE - 1);
    //------------------------
    tem_variable1 = WordSum(MAX_FRESNEL_TYPE, (uint32_t*)s_iot_parameter_str.fresnel_time);
    
    g_iot_send_data.Fresnel_Tim1.Header.Opera_Type = 0x00;
    g_iot_send_data.Fresnel_Tim1.Header.Command_Type = IOT_COMMAND_TYPE;
    g_iot_send_data.Fresnel_Tim1.IOT_Type = 0x13;
    g_iot_send_data.Fresnel_Tim1.Work_Time = tem_variable1;
    tem_variable1 = tem_variable1 == 0 ? 1 : tem_variable1;
    g_iot_send_data.Fresnel_Tim1.No_Fresnel_Duty = 1000 * s_iot_parameter_str.fresnel_time[NO_FRESNEL] / tem_variable1;
    g_iot_send_data.Fresnel_Tim1.Fresnel1_Duty = 1000 * s_iot_parameter_str.fresnel_time[FRESNEL_TYPE1] / tem_variable1;
    g_iot_send_data.Fresnel_Tim1.Fresnel2_Duty = 1000 * s_iot_parameter_str.fresnel_time[FRESNEL_TYPE2] / tem_variable1;
    g_iot_send_data.Fresnel_Tim1.CheckSum = CheckSum_Get((uint8_t*)(&g_iot_send_data.Fresnel_Tim1.CheckSum) + 1, IOT_PACKET_SIZE - 1);
    
    tem_variable2 = 1000 - g_iot_send_data.Fresnel_Tim1.No_Fresnel_Duty - g_iot_send_data.Fresnel_Tim1.Fresnel1_Duty - \
                    g_iot_send_data.Fresnel_Tim1.Fresnel2_Duty;
    g_iot_send_data.Fresnel_Tim2.Header.Opera_Type = 0x00;
    g_iot_send_data.Fresnel_Tim2.Header.Command_Type = IOT_COMMAND_TYPE;
    g_iot_send_data.Fresnel_Tim2.IOT_Type = 0x14;
    g_iot_send_data.Fresnel_Tim2.Fresnel3_Duty = 1000 * s_iot_parameter_str.fresnel_time[FRESNEL_TYPE3] / tem_variable1;
    g_iot_send_data.Fresnel_Tim2.Fresnel4_Duty = 1000 * s_iot_parameter_str.fresnel_time[FRESNEL_TYPE4] / tem_variable1;
    g_iot_send_data.Fresnel_Tim2.Fresnel5_Duty = 1000 * s_iot_parameter_str.fresnel_time[FRESNEL_TYPE5] / tem_variable1;
    g_iot_send_data.Fresnel_Tim2.Fresnel6_Duty = 1000 * s_iot_parameter_str.fresnel_time[FRESNEL_TYPE6] / tem_variable1;
    g_iot_send_data.Fresnel_Tim2.Fresnel7_Duty = 1000 * s_iot_parameter_str.fresnel_time[FRESNEL_TYPE7] / tem_variable1;
    g_iot_send_data.Fresnel_Tim2.CheckSum = CheckSum_Get((uint8_t*)(&g_iot_send_data.Fresnel_Tim2.CheckSum) + 1, IOT_PACKET_SIZE - 1);
    
    tem_variable2 = tem_variable2 - g_iot_send_data.Fresnel_Tim2.Fresnel3_Duty - g_iot_send_data.Fresnel_Tim2.Fresnel4_Duty - \
                    g_iot_send_data.Fresnel_Tim2.Fresnel5_Duty - g_iot_send_data.Fresnel_Tim2.Fresnel6_Duty - \
                    g_iot_send_data.Fresnel_Tim2.Fresnel7_Duty;
    g_iot_send_data.Fresnel_Tim3.Header.Opera_Type = 0x00;
    g_iot_send_data.Fresnel_Tim3.Header.Command_Type = IOT_COMMAND_TYPE;
    g_iot_send_data.Fresnel_Tim3.IOT_Type = 0x15;
    g_iot_send_data.Fresnel_Tim3.Fresnel8_Duty = 1000 * s_iot_parameter_str.fresnel_time[FRESNEL_TYPE8] / tem_variable1;
    g_iot_send_data.Fresnel_Tim3.Fresnel9_Duty = 1000 * s_iot_parameter_str.fresnel_time[FRESNEL_TYPE9] / tem_variable1;
    g_iot_send_data.Fresnel_Tim3.Fresnel10_Duty = 1000 * s_iot_parameter_str.fresnel_time[FRESNEL_TYPE10] / tem_variable1;
    g_iot_send_data.Fresnel_Tim3.Fresnel11_Duty = 1000 * s_iot_parameter_str.fresnel_time[FRESNEL_TYPE11] / tem_variable1;
    g_iot_send_data.Fresnel_Tim3.Fresnel12_Duty = 1000 * s_iot_parameter_str.fresnel_time[FRESNEL_TYPE12] / tem_variable1;
    tem_variable2 = tem_variable2 - g_iot_send_data.Fresnel_Tim3.Fresnel8_Duty - g_iot_send_data.Fresnel_Tim3.Fresnel9_Duty - \
                    g_iot_send_data.Fresnel_Tim3.Fresnel10_Duty - g_iot_send_data.Fresnel_Tim3.Fresnel11_Duty;
    g_iot_send_data.Fresnel_Tim3.CheckSum = CheckSum_Get((uint8_t*)(&g_iot_send_data.Fresnel_Tim3.CheckSum) + 1, IOT_PACKET_SIZE - 1);
    //------------------------
    tem_variable1 = WordSum(MAX_PITCH_ANGLE_TYPE, (uint32_t*)s_iot_parameter_str.pitch_angle_time);
    
    g_iot_send_data.Pitch_Angle_Tim1.Header.Opera_Type = 0x00;
    g_iot_send_data.Pitch_Angle_Tim1.Header.Command_Type = IOT_COMMAND_TYPE;
    g_iot_send_data.Pitch_Angle_Tim1.IOT_Type = 0x16;
    g_iot_send_data.Pitch_Angle_Tim1.Work_Time = tem_variable1;
    tem_variable1 = tem_variable1 == 0 ? 1 : tem_variable1;
    g_iot_send_data.Pitch_Angle_Tim1.No_Pitch_Angle_Duty = 1000 * s_iot_parameter_str.pitch_angle_time[NO_PITCH_ANGLE] / tem_variable1;
    g_iot_send_data.Pitch_Angle_Tim1.Pitch_Angle1_Duty = 1000 * s_iot_parameter_str.pitch_angle_time[PITCH_ANGLE_TYPE1] / tem_variable1;
    g_iot_send_data.Pitch_Angle_Tim1.Pitch_Angle2_Duty = 1000 * s_iot_parameter_str.pitch_angle_time[PITCH_ANGLE_TYPE2] / tem_variable1;
    g_iot_send_data.Pitch_Angle_Tim1.CheckSum = CheckSum_Get((uint8_t*)(&g_iot_send_data.Pitch_Angle_Tim1.CheckSum) + 1, IOT_PACKET_SIZE - 1);
    
    tem_variable2 = 1000 - g_iot_send_data.Pitch_Angle_Tim1.No_Pitch_Angle_Duty - g_iot_send_data.Pitch_Angle_Tim1.Pitch_Angle1_Duty - \
                    g_iot_send_data.Pitch_Angle_Tim1.Pitch_Angle2_Duty;
    g_iot_send_data.Pitch_Angle_Tim2.Header.Opera_Type = 0x00;
    g_iot_send_data.Pitch_Angle_Tim2.Header.Command_Type = IOT_COMMAND_TYPE;
    g_iot_send_data.Pitch_Angle_Tim2.IOT_Type = 0x17;
    g_iot_send_data.Pitch_Angle_Tim2.Pitch_Angle3_Duty = 1000 * s_iot_parameter_str.pitch_angle_time[PITCH_ANGLE_TYPE3] / tem_variable1;
    g_iot_send_data.Pitch_Angle_Tim2.Pitch_Angle4_Duty = 1000 * s_iot_parameter_str.pitch_angle_time[PITCH_ANGLE_TYPE4] / tem_variable1;
    g_iot_send_data.Pitch_Angle_Tim2.Pitch_Angle5_Duty = 1000 * s_iot_parameter_str.pitch_angle_time[PITCH_ANGLE_TYPE5] / tem_variable1;
    g_iot_send_data.Pitch_Angle_Tim2.Pitch_Angle6_Duty = 1000 * s_iot_parameter_str.pitch_angle_time[PITCH_ANGLE_TYPE6] / tem_variable1;
    g_iot_send_data.Pitch_Angle_Tim2.Pitch_Angle7_Duty = 1000 * s_iot_parameter_str.pitch_angle_time[PITCH_ANGLE_TYPE7] / tem_variable1;
    g_iot_send_data.Pitch_Angle_Tim2.CheckSum = CheckSum_Get((uint8_t*)(&g_iot_send_data.Pitch_Angle_Tim2.CheckSum) + 1, IOT_PACKET_SIZE - 1);
    
    tem_variable2 = tem_variable2 - g_iot_send_data.Pitch_Angle_Tim2.Pitch_Angle3_Duty - g_iot_send_data.Pitch_Angle_Tim2.Pitch_Angle4_Duty - \
                    g_iot_send_data.Pitch_Angle_Tim2.Pitch_Angle5_Duty - g_iot_send_data.Pitch_Angle_Tim2.Pitch_Angle6_Duty - \
                    g_iot_send_data.Pitch_Angle_Tim2.Pitch_Angle7_Duty;
    g_iot_send_data.Pitch_Angle_Tim3.Header.Opera_Type = 0x00;
    g_iot_send_data.Pitch_Angle_Tim3.Header.Command_Type = IOT_COMMAND_TYPE;
    g_iot_send_data.Pitch_Angle_Tim3.IOT_Type = 0x18;
    g_iot_send_data.Pitch_Angle_Tim3.Pitch_Angle8_Duty = 1000 * s_iot_parameter_str.pitch_angle_time[PITCH_ANGLE_TYPE8] / tem_variable1;
    g_iot_send_data.Pitch_Angle_Tim3.Pitch_Angle9_Duty = 1000 * s_iot_parameter_str.pitch_angle_time[PITCH_ANGLE_TYPE9] / tem_variable1;
    g_iot_send_data.Pitch_Angle_Tim3.Pitch_Angle10_Duty = 1000 * s_iot_parameter_str.pitch_angle_time[PITCH_ANGLE_TYPE10] / tem_variable1;
    g_iot_send_data.Pitch_Angle_Tim3.Pitch_Angle11_Duty = 1000 * s_iot_parameter_str.pitch_angle_time[PITCH_ANGLE_TYPE11] / tem_variable1;
    g_iot_send_data.Pitch_Angle_Tim3.Pitch_Angle12_Duty = 1000 * s_iot_parameter_str.pitch_angle_time[PITCH_ANGLE_TYPE12] / tem_variable1;
    g_iot_send_data.Pitch_Angle_Tim3.CheckSum = CheckSum_Get((uint8_t*)(&g_iot_send_data.Pitch_Angle_Tim3.CheckSum) + 1, IOT_PACKET_SIZE - 1);
    
    tem_variable2 = tem_variable2 - g_iot_send_data.Pitch_Angle_Tim3.Pitch_Angle8_Duty - g_iot_send_data.Pitch_Angle_Tim3.Pitch_Angle9_Duty - \
                    g_iot_send_data.Pitch_Angle_Tim3.Pitch_Angle10_Duty - g_iot_send_data.Pitch_Angle_Tim3.Pitch_Angle11_Duty - \
                    g_iot_send_data.Pitch_Angle_Tim3.Pitch_Angle12_Duty;
    g_iot_send_data.Pitch_Angle_Tim4.Header.Opera_Type = 0x00;
    g_iot_send_data.Pitch_Angle_Tim4.Header.Command_Type = IOT_COMMAND_TYPE;
    g_iot_send_data.Pitch_Angle_Tim4.IOT_Type = 0x19;
    g_iot_send_data.Pitch_Angle_Tim4.Pitch_Angle13_Duty = 1000 * s_iot_parameter_str.pitch_angle_time[PITCH_ANGLE_TYPE13] / tem_variable1;
    g_iot_send_data.Pitch_Angle_Tim4.Pitch_Angle14_Duty = 1000 * s_iot_parameter_str.pitch_angle_time[PITCH_ANGLE_TYPE14] / tem_variable1;
    g_iot_send_data.Pitch_Angle_Tim4.Pitch_Angle15_Duty = 1000 * s_iot_parameter_str.pitch_angle_time[PITCH_ANGLE_TYPE15] / tem_variable1;
    g_iot_send_data.Pitch_Angle_Tim4.Pitch_Angle16_Duty = 1000 * s_iot_parameter_str.pitch_angle_time[PITCH_ANGLE_TYPE16] / tem_variable1;
    g_iot_send_data.Pitch_Angle_Tim4.Pitch_Angle17_Duty = 1000 * s_iot_parameter_str.pitch_angle_time[PITCH_ANGLE_TYPE17] / tem_variable1;
    g_iot_send_data.Pitch_Angle_Tim4.CheckSum = CheckSum_Get((uint8_t*)(&g_iot_send_data.Pitch_Angle_Tim4.CheckSum) + 1, IOT_PACKET_SIZE - 1);
    
    tem_variable2 = tem_variable2 - g_iot_send_data.Pitch_Angle_Tim4.Pitch_Angle13_Duty - g_iot_send_data.Pitch_Angle_Tim4.Pitch_Angle14_Duty - \
                    g_iot_send_data.Pitch_Angle_Tim4.Pitch_Angle15_Duty - g_iot_send_data.Pitch_Angle_Tim4.Pitch_Angle16_Duty - \
                    g_iot_send_data.Pitch_Angle_Tim4.Pitch_Angle17_Duty;
    g_iot_send_data.Pitch_Angle_Tim5.Header.Opera_Type = 0x00;
    g_iot_send_data.Pitch_Angle_Tim5.Header.Command_Type = IOT_COMMAND_TYPE;
    g_iot_send_data.Pitch_Angle_Tim5.IOT_Type = 0x1A;
    g_iot_send_data.Pitch_Angle_Tim5.Pitch_Angle18_Duty = 1000 * s_iot_parameter_str.pitch_angle_time[PITCH_ANGLE_TYPE18] / tem_variable1;
    g_iot_send_data.Pitch_Angle_Tim5.CheckSum = CheckSum_Get((uint8_t*)(&g_iot_send_data.Pitch_Angle_Tim5.CheckSum) + 1, IOT_PACKET_SIZE - 1);
    //------------------------
    tem_variable1 = s_iot_parameter_str.mode_time[SIDUSPRO_FX_MODE_TIME];
    tem_variable1 = tem_variable1 == 0 ? 1 : tem_variable1;
    g_iot_send_data.SidusPro_FX_Tim.Header.Opera_Type = 0x00;
    g_iot_send_data.SidusPro_FX_Tim.Header.Command_Type = IOT_COMMAND_TYPE;
    g_iot_send_data.SidusPro_FX_Tim.IOT_Type = 0x1B;
    g_iot_send_data.SidusPro_FX_Tim.Manual_Effects_Duty = 1000 * s_iot_parameter_str.sidus_pro_fx[MANUAL_EFFECTS_TYPE] / tem_variable1;
    g_iot_send_data.SidusPro_FX_Tim.Touchbar_Effects_Duty = 1000 * s_iot_parameter_str.sidus_pro_fx[TOUCHBAR_EFFECTS_TYPE] / tem_variable1;
    g_iot_send_data.SidusPro_FX_Tim.MusicEffects_Duty = 1000 * s_iot_parameter_str.sidus_pro_fx[MUSIC_EFFECTS_TYPE] / tem_variable1;
    g_iot_send_data.SidusPro_FX_Tim.PickerEffects_Duty = 1000 * s_iot_parameter_str.sidus_pro_fx[PICKER_EFFECTS_TYPE] / tem_variable1;
    g_iot_send_data.SidusPro_FX_Tim.Magic_Program_Duty = 1000 * s_iot_parameter_str.sidus_pro_fx[MAGIC_PROGRAM_TYPE] / tem_variable1;
    tem_variable2 = 1000 - g_iot_send_data.SidusPro_FX_Tim.Manual_Effects_Duty - g_iot_send_data.SidusPro_FX_Tim.Touchbar_Effects_Duty - \
                    g_iot_send_data.SidusPro_FX_Tim.MusicEffects_Duty - g_iot_send_data.SidusPro_FX_Tim.PickerEffects_Duty;
    //--h                
    if(1000 != tem_variable2 || 0 != g_iot_send_data.SidusPro_FX_Tim.Magic_Program_Duty)
        g_iot_send_data.SidusPro_FX_Tim.Magic_Program_Duty = tem_variable2;
    g_iot_send_data.SidusPro_FX_Tim.CheckSum = CheckSum_Get((uint8_t*)(&g_iot_send_data.SidusPro_FX_Tim.CheckSum) + 1, IOT_PACKET_SIZE - 1);
    //------------------------
    g_iot_send_data.Error_Msg1.Header.Opera_Type = 0x00;
    g_iot_send_data.Error_Msg1.Header.Command_Type = IOT_COMMAND_TYPE;
    g_iot_send_data.Error_Msg1.IOT_Type = 0x1C;
    g_iot_send_data.Error_Msg1.Fan1_Error = s_iot_parameter_str.error_msg[ERROR_FAN1];
    g_iot_send_data.Error_Msg1.Fan2_Error = s_iot_parameter_str.error_msg[ERROR_FAN2];
    g_iot_send_data.Error_Msg1.Fan3_Error = s_iot_parameter_str.error_msg[ERROR_FAN3];
    g_iot_send_data.Error_Msg1.Fan4_Error = s_iot_parameter_str.error_msg[ERROR_FAN4];
    g_iot_send_data.Error_Msg1.Fan5_Error = s_iot_parameter_str.error_msg[ERROR_FAN5];
    g_iot_send_data.Error_Msg1.Fan6_Error = s_iot_parameter_str.error_msg[ERROR_FAN6];
    g_iot_send_data.Error_Msg1.CheckSum = CheckSum_Get((uint8_t*)(&g_iot_send_data.Error_Msg1.CheckSum) + 1, IOT_PACKET_SIZE - 1);
    
    g_iot_send_data.Error_Msg2.Header.Opera_Type = 0x00;
    g_iot_send_data.Error_Msg2.Header.Command_Type = IOT_COMMAND_TYPE;
    g_iot_send_data.Error_Msg2.IOT_Type = 0x1D;
    g_iot_send_data.Error_Msg2.Temp_Sensor1_Error = s_iot_parameter_str.error_msg[ERROR_TEMP_SENSOR1];
    g_iot_send_data.Error_Msg2.Temp_Sensor2_Error = s_iot_parameter_str.error_msg[ERROR_TEMP_SENSOR2];
    g_iot_send_data.Error_Msg2.Temp_Sensor3_Error = s_iot_parameter_str.error_msg[ERROR_TEMP_SENSOR3];
    g_iot_send_data.Error_Msg2.Temp_Sensor4_Error = s_iot_parameter_str.error_msg[ERROR_TEMP_SENSOR4];
    g_iot_send_data.Error_Msg2.Temp_Sensor5_Error = s_iot_parameter_str.error_msg[ERROR_TEMP_SENSOR5];
    g_iot_send_data.Error_Msg2.Temp_Sensor6_Error = s_iot_parameter_str.error_msg[ERROR_TEMP_SENSOR6];
    g_iot_send_data.Error_Msg2.CheckSum = CheckSum_Get((uint8_t*)(&g_iot_send_data.Error_Msg2.CheckSum) + 1, IOT_PACKET_SIZE - 1);
    
    g_iot_send_data.Error_Msg3.Header.Opera_Type = 0x00;
    g_iot_send_data.Error_Msg3.Header.Command_Type = IOT_COMMAND_TYPE;
    g_iot_send_data.Error_Msg3.IOT_Type = 0x1E;
    g_iot_send_data.Error_Msg3.Bat1_Volt_Error = s_iot_parameter_str.error_msg[ERROR_BAT1_VOLT];
    g_iot_send_data.Error_Msg3.Bat2_Volt_Error = s_iot_parameter_str.error_msg[ERROR_BAT2_VOLT];
    g_iot_send_data.Error_Msg3.Bat3_Volt_Error = s_iot_parameter_str.error_msg[ERROR_BAT3_VOLT];
    g_iot_send_data.Error_Msg3.Bat4_Volt_Error = s_iot_parameter_str.error_msg[ERROR_BAT4_VOLT];
    g_iot_send_data.Error_Msg3.Bat5_Volt_Error = s_iot_parameter_str.error_msg[ERROR_BAT5_VOLT];
    g_iot_send_data.Error_Msg3.Bat6_Volt_Error = s_iot_parameter_str.error_msg[ERROR_BAT6_VOLT];
    g_iot_send_data.Error_Msg3.CheckSum = CheckSum_Get((uint8_t*)(&g_iot_send_data.Error_Msg3.CheckSum) + 1, IOT_PACKET_SIZE - 1);
    
    g_iot_send_data.Error_Msg4.Header.Opera_Type = 0x00;
    g_iot_send_data.Error_Msg4.Header.Command_Type = IOT_COMMAND_TYPE;
    g_iot_send_data.Error_Msg4.IOT_Type = 0x1F;
    g_iot_send_data.Error_Msg4.Ctr_Input_Volt_Error = s_iot_parameter_str.error_msg[ERROR_CTR_INPUT_VOLT];
    g_iot_send_data.Error_Msg4.Drv_Input_Volt_Error = s_iot_parameter_str.error_msg[ERROR_DRV_INPUT_VOLT];
    g_iot_send_data.Error_Msg4.Ctr_Input_Curr_Error = s_iot_parameter_str.error_msg[ERROR_CTR_CURR];
    g_iot_send_data.Error_Msg4.Drv_Input_Curr_Error = s_iot_parameter_str.error_msg[ERROR_DRV_CURR];
    g_iot_send_data.Error_Msg4.Ctr_Volt_3d3V_Error = s_iot_parameter_str.error_msg[ERROR_CTR_3D3V];
    g_iot_send_data.Error_Msg4.Ctr_Volt_5V_Error = s_iot_parameter_str.error_msg[ERROR_CTR_5V];
    g_iot_send_data.Error_Msg4.CheckSum = CheckSum_Get((uint8_t*)(&g_iot_send_data.Error_Msg4.CheckSum) + 1, IOT_PACKET_SIZE - 1);
    
    g_iot_send_data.Error_Msg5.Header.Opera_Type = 0x00;
    g_iot_send_data.Error_Msg5.Header.Command_Type = IOT_COMMAND_TYPE;
    g_iot_send_data.Error_Msg5.IOT_Type = 0x20;
    g_iot_send_data.Error_Msg5.Ctr_Volt_9V_Error = s_iot_parameter_str.error_msg[ERROR_CTR_9V];
    g_iot_send_data.Error_Msg5.Ctr_Volt_12V_Error = s_iot_parameter_str.error_msg[ERROR_CTR_12V];
    g_iot_send_data.Error_Msg5.Ctr_Volt_15V_Error = s_iot_parameter_str.error_msg[ERROR_CTR_15V];
    g_iot_send_data.Error_Msg5.Drv_Volt_3d3V_Error = s_iot_parameter_str.error_msg[ERROR_DRV_3D3V];
    g_iot_send_data.Error_Msg5.Drv_Volt_5V_Error = s_iot_parameter_str.error_msg[ERROR_DRV_5V];
    g_iot_send_data.Error_Msg5.Drv_Volt_9V_Error = s_iot_parameter_str.error_msg[ERROR_DRV_9V];
    g_iot_send_data.Error_Msg5.CheckSum = CheckSum_Get((uint8_t*)(&g_iot_send_data.Error_Msg5.CheckSum) + 1, IOT_PACKET_SIZE - 1);
    
    g_iot_send_data.Error_Msg6.Header.Opera_Type = 0x00;
    g_iot_send_data.Error_Msg6.Header.Command_Type = IOT_COMMAND_TYPE;
    g_iot_send_data.Error_Msg6.IOT_Type = 0x21;
    g_iot_send_data.Error_Msg6.Drv_Volt_12V_Error = s_iot_parameter_str.error_msg[ERROR_DRV_12V];
    g_iot_send_data.Error_Msg6.Drv_Volt_15V_Error = s_iot_parameter_str.error_msg[ERROR_DRV_15V];
    g_iot_send_data.Error_Msg6.RS485_Error = s_iot_parameter_str.error_msg[ERROR_RS485];
    g_iot_send_data.Error_Msg6.Stroe_Error = s_iot_parameter_str.error_msg[ERROR_STORE];
    g_iot_send_data.Error_Msg6.PD_Error = s_iot_parameter_str.error_msg[ERROR_PD];
    g_iot_send_data.Error_Msg6.PD_Wireless_Error = s_iot_parameter_str.error_msg[ERROR_WIRELESS_PD];
    g_iot_send_data.Error_Msg6.CheckSum = CheckSum_Get((uint8_t*)(&g_iot_send_data.Error_Msg6.CheckSum) + 1, IOT_PACKET_SIZE - 1);
    
    g_iot_send_data.Error_Msg7.Header.Opera_Type = 0x00;
    g_iot_send_data.Error_Msg7.Header.Command_Type = IOT_COMMAND_TYPE;
    g_iot_send_data.Error_Msg7.IOT_Type = 0x22;
    g_iot_send_data.Error_Msg7.High_Temp_Error = s_iot_parameter_str.error_msg[ERROR_HIGH_TEMP];
    g_iot_send_data.Error_Msg7.USB_Update_Error = s_iot_parameter_str.error_msg[ERROR_USB_UPDATE];
    g_iot_send_data.Error_Msg7.USB_Mount_Error = s_iot_parameter_str.error_msg[ERROR_USB_MOUNT];
    g_iot_send_data.Error_Msg7.Ble_Update_Error = s_iot_parameter_str.error_msg[ERROR_BLE_UPDATE];
    g_iot_send_data.Error_Msg7.CheckSum = CheckSum_Get((uint8_t*)(&g_iot_send_data.Error_Msg7.CheckSum) + 1, IOT_PACKET_SIZE - 1);
}
/*****************************************************************************************
* Function Name: set_device_version_msg
* Description  : 获得设备版本信息和IOT总包数信息,上电调用;
* Arguments    : NONE
* Return Value : NONE;
******************************************************************************************/
void set_device_version_msg(Device_Ver_TypeDef* p_ver)
{
    g_iot_send_data.Msg.Header.Opera_Type = 0x00;
    g_iot_send_data.Msg.Header.Command_Type = IOT_COMMAND_TYPE;
    g_iot_send_data.Msg.IOT_Type = 0x00;
    g_iot_send_data.Msg.Frame_Num = IOT_FRAME_NUM;
    g_iot_send_data.Msg.CheckSum = CheckSum_Get((uint8_t*)(&g_iot_send_data.Msg.CheckSum) + 1, IOT_PACKET_SIZE - 1);
    
    g_iot_send_data.Ver1.Header.Opera_Type = 0x00;
    g_iot_send_data.Ver1.Header.Command_Type = IOT_COMMAND_TYPE;
    g_iot_send_data.Ver1.IOT_Type = 0x01;
    g_iot_send_data.Ver1.Protocol_Ver = p_ver->Protocol_Ver;
    g_iot_send_data.Ver1.Control_HwVer = p_ver->Control_HwVer;
    g_iot_send_data.Ver1.Control_SwVer = p_ver->Control_SwVer;
    g_iot_send_data.Ver1.Driver_HwVer = p_ver->Driver_HwVer;
    g_iot_send_data.Ver1.Driver_SwVer = p_ver->Driver_SwVer;
    g_iot_send_data.Ver1.CheckSum = CheckSum_Get((uint8_t*)(&g_iot_send_data.Ver1.CheckSum) + 1, IOT_PACKET_SIZE - 1);
    
    g_iot_send_data.Ver2.Header.Opera_Type = 0x00;
    g_iot_send_data.Ver2.Header.Command_Type = IOT_COMMAND_TYPE;
    g_iot_send_data.Ver2.IOT_Type = 0x02;
    g_iot_send_data.Ver2.Product_Function = p_ver->Product_Function;
    g_iot_send_data.Ver2.Product_LedType = p_ver->Product_LedType;
    g_iot_send_data.Ver2.Product_CCTRange_L = p_ver->Product_CCTRange_L;
    g_iot_send_data.Ver2.Product_CCTRange = p_ver->Product_CCTRange;
    g_iot_send_data.Ver2.Product_Machine = p_ver->Product_Machine;
    
    g_iot_send_data.Ver2.ManualFX_support = p_ver->ManualFX_support;
    g_iot_send_data.Ver2.ProgramFX_support = p_ver->ProgramFX_support;
    g_iot_send_data.Ver2.CFX_PickerFX_support = p_ver->CFX_PickerFX_support;
    g_iot_send_data.Ver2.CFX_TouchbarFX_support = p_ver->CFX_TouchbarFX_support;
    g_iot_send_data.Ver2.CFX_MusicFX_support = p_ver->CFX_MusicFX_support;
    g_iot_send_data.Ver2.CheckSum = CheckSum_Get((uint8_t*)(&g_iot_send_data.Ver2.CheckSum) + 1, IOT_PACKET_SIZE - 1);
}
/*****************************************************************************************
* Function Name: get_send_data_serial
* Description  : 获得发送数据包序号；
* Arguments    : iot_type：IOT数据类型
* Return Value : 需要被发送数据帧的序号，如果是获取整个IOT数据，则返回0XFF;
******************************************************************************************/
extern void Set_Iot_test_Data(void);
uint8_t get_send_data_serial(uint8_t iot_type)
{
    if(0x7f == iot_type)
    {
//      Set_Iot_test_Data();
        package_iot_send_data();
        return 0xff;
    }
    return iot_type;
}
/*****************************************************************************************
* Function Name: Set_Iot_test_Data
* Description  : 测试IOT数据转换用；
* Arguments    : NONE
* Return Value : NONE
******************************************************************************************/
void Set_Iot_test_Data(void)
{
    uint8_t i = 0;
    uint16_t j = 1;
    
    s_iot_parameter_str.device_state_time[DEVICE_CLOSE] = 10000;//10%
    s_iot_parameter_str.device_state_time[DEVICE_OPEN] = 90000;//90%
    
    s_iot_parameter_str.run_time[ADAPTER_SUPPLY_TIME] = 36000;//40%
    s_iot_parameter_str.run_time[ONE_BAT_SUPPLY_TIME] = 10800;//20%
    s_iot_parameter_str.run_time[TWO_BAT_SUPPLY_TIME] = 27000;//50%
    s_iot_parameter_str.run_time[THREE_BAT_SUPPLY_TIME] = 5400;//10%
    s_iot_parameter_str.run_time[FOUR_BAT_SUPPLY_TIME] = 5400;//10%
    s_iot_parameter_str.run_time[FIVE_BAT_SUPPLY_TIME] = 2700;//5%
    s_iot_parameter_str.run_time[SIX_BAT_SUPPLY_TIME] = 2700;//5%
    
    s_iot_parameter_str.mode_time[DMX_MODE_TIME] = 9000;//10%
    s_iot_parameter_str.mode_time[CCT_MODE_TIME] = 45000;//50%
    s_iot_parameter_str.mode_time[GEL_MODE_TIME] = 9000;//10%
    s_iot_parameter_str.mode_time[HSI_MODE_TIME] = 9000;//10%
    s_iot_parameter_str.mode_time[XY_MODE_TIME] = 900;//1%
    s_iot_parameter_str.mode_time[RGB_MODE_TIME] = 1800;//2%
    s_iot_parameter_str.mode_time[SFX_MODE_TIME] = 4500;//5%
    s_iot_parameter_str.mode_time[SIDUSPRO_FX_MODE_TIME] = 10800;//12%
    
    s_iot_parameter_str.int_time[INT_TYPE1] = 9000;//10%
    s_iot_parameter_str.int_time[INT_TYPE2] = 18000;//20%
    s_iot_parameter_str.int_time[INT_TYPE3] = 9000;//10%
    s_iot_parameter_str.int_time[INT_TYPE4] = 3600;//4%
    s_iot_parameter_str.int_time[INT_TYPE5] = 2700;//3%
    s_iot_parameter_str.int_time[INT_TYPE6] = 1800;//2%
    s_iot_parameter_str.int_time[INT_TYPE7] = 900;//1%
    s_iot_parameter_str.int_time[INT_TYPE8] = 900;//1%
    s_iot_parameter_str.int_time[INT_TYPE9] = 1800;//2%
    s_iot_parameter_str.int_time[INT_TYPE10] = 2700;//3%
    s_iot_parameter_str.int_time[INT_TYPE11] = 3600;//4%
    s_iot_parameter_str.int_time[INT_TYPE12] = 9000;//10%
    s_iot_parameter_str.int_time[INT_TYPE13] = 9000;//10%
    s_iot_parameter_str.int_time[INT_TYPE14] = 18000;//20%
    
    s_iot_parameter_str.duration_time[DURATION_TIME_TYPE1] = 18000;//20%
    s_iot_parameter_str.duration_time[DURATION_TIME_TYPE2] = 9000;//10%
    s_iot_parameter_str.duration_time[DURATION_TIME_TYPE3] = 4500;//5%
    s_iot_parameter_str.duration_time[DURATION_TIME_TYPE4] = 4500;//5%
    s_iot_parameter_str.duration_time[DURATION_TIME_TYPE5] = 4500;//5%
    s_iot_parameter_str.duration_time[DURATION_TIME_TYPE6] = 9000;//10%
    s_iot_parameter_str.duration_time[DURATION_TIME_TYPE7] = 9000;//10%
    s_iot_parameter_str.duration_time[DURATION_TIME_TYPE8] = 9000;//10% 75
    s_iot_parameter_str.duration_time[DURATION_TIME_TYPE9] = 900;//1%
    s_iot_parameter_str.duration_time[DURATION_TIME_TYPE10] = 900;//1%
    s_iot_parameter_str.duration_time[DURATION_TIME_TYPE11] = 900;//1%
    s_iot_parameter_str.duration_time[DURATION_TIME_TYPE12] = 900;//1%
    s_iot_parameter_str.duration_time[DURATION_TIME_TYPE13] = 900;//1%
    s_iot_parameter_str.duration_time[DURATION_TIME_TYPE14] = 900;//1%
    s_iot_parameter_str.duration_time[DURATION_TIME_TYPE15] = 900;//1%
    s_iot_parameter_str.duration_time[DURATION_TIME_TYPE16] = 900;//1%
    s_iot_parameter_str.duration_time[DURATION_TIME_TYPE17] = 900;//1%
    s_iot_parameter_str.duration_time[DURATION_TIME_TYPE18] = 900;//1% 85
    s_iot_parameter_str.duration_time[DURATION_TIME_TYPE19] = 1800;//2%
    s_iot_parameter_str.duration_time[DURATION_TIME_TYPE20] = 900;//1%
    s_iot_parameter_str.duration_time[DURATION_TIME_TYPE21] = 900;//1%
    s_iot_parameter_str.duration_time[DURATION_TIME_TYPE22] = 900;//1%
    s_iot_parameter_str.duration_time[DURATION_TIME_TYPE23] = 900;//1%
    s_iot_parameter_str.duration_time[DURATION_TIME_TYPE24] = 900;//1%
    s_iot_parameter_str.duration_time[DURATION_TIME_TYPE25] = 900;//1%
    s_iot_parameter_str.duration_time[DURATION_TIME_TYPE26] = 900;//1%
    s_iot_parameter_str.duration_time[DURATION_TIME_TYPE27] = 900;//1%
    s_iot_parameter_str.duration_time[DURATION_TIME_TYPE28] = 900;//1%
    s_iot_parameter_str.duration_time[DURATION_TIME_TYPE29] = 900;//1%
    s_iot_parameter_str.duration_time[DURATION_TIME_TYPE30] = 900;//1%
    s_iot_parameter_str.duration_time[DURATION_TIME_TYPE31] = 900;//1%
    s_iot_parameter_str.duration_time[DURATION_TIME_TYPE32] = 900;//1%
    
    s_iot_parameter_str.control_way[CTR_BOX_WAY] = 500;//50%
    s_iot_parameter_str.control_way[CTR_BLE_WAY] = 500;//50%
    
    s_iot_parameter_str.fresnel_time[NO_FRESNEL] = 45000;//50%
    s_iot_parameter_str.fresnel_time[FRESNEL_TYPE1] = 4500;//5%
    s_iot_parameter_str.fresnel_time[FRESNEL_TYPE2] = 4500;//5%
    s_iot_parameter_str.fresnel_time[FRESNEL_TYPE3] = 4500;//5%
    s_iot_parameter_str.fresnel_time[FRESNEL_TYPE4] = 4500;//5%
    s_iot_parameter_str.fresnel_time[FRESNEL_TYPE5] = 9000;//10%
    s_iot_parameter_str.fresnel_time[FRESNEL_TYPE6] = 9000;//10%
    s_iot_parameter_str.fresnel_time[FRESNEL_TYPE7] = 900;//1%
    s_iot_parameter_str.fresnel_time[FRESNEL_TYPE8] = 900;//1%
    s_iot_parameter_str.fresnel_time[FRESNEL_TYPE9] = 900;//1%
    s_iot_parameter_str.fresnel_time[FRESNEL_TYPE10] = 900;//1%
    s_iot_parameter_str.fresnel_time[FRESNEL_TYPE11] = 900;//1%
    s_iot_parameter_str.fresnel_time[FRESNEL_TYPE12] = 4500;//5%
    
    s_iot_parameter_str.pitch_angle_time[NO_PITCH_ANGLE] = 45000;//50%
    s_iot_parameter_str.pitch_angle_time[PITCH_ANGLE_TYPE1] = 4500;//5%
    s_iot_parameter_str.pitch_angle_time[PITCH_ANGLE_TYPE2] = 4500;//5%
    s_iot_parameter_str.pitch_angle_time[PITCH_ANGLE_TYPE3] = 4500;//5%
    s_iot_parameter_str.pitch_angle_time[PITCH_ANGLE_TYPE4] = 4500;//5%
    s_iot_parameter_str.pitch_angle_time[PITCH_ANGLE_TYPE5] = 900;//1%
    s_iot_parameter_str.pitch_angle_time[PITCH_ANGLE_TYPE6] = 900;//1%
    s_iot_parameter_str.pitch_angle_time[PITCH_ANGLE_TYPE7] = 900;//1%
    s_iot_parameter_str.pitch_angle_time[PITCH_ANGLE_TYPE8] = 900;//1%
    s_iot_parameter_str.pitch_angle_time[PITCH_ANGLE_TYPE9] = 900;//1%
    s_iot_parameter_str.pitch_angle_time[PITCH_ANGLE_TYPE10] = 900;//1%
    s_iot_parameter_str.pitch_angle_time[PITCH_ANGLE_TYPE11] = 900;//1%
    s_iot_parameter_str.pitch_angle_time[PITCH_ANGLE_TYPE12] = 900;//1%
    s_iot_parameter_str.pitch_angle_time[PITCH_ANGLE_TYPE13] = 900;//1%
    s_iot_parameter_str.pitch_angle_time[PITCH_ANGLE_TYPE14] = 900;//1%
    s_iot_parameter_str.pitch_angle_time[PITCH_ANGLE_TYPE15] = 4500;//5%
    s_iot_parameter_str.pitch_angle_time[PITCH_ANGLE_TYPE16] = 4500;//5%
    s_iot_parameter_str.pitch_angle_time[PITCH_ANGLE_TYPE17] = 4500;//5%
    s_iot_parameter_str.pitch_angle_time[PITCH_ANGLE_TYPE18] = 4500;//5%
    
    s_iot_parameter_str.sidus_pro_fx[MANUAL_EFFECTS_TYPE] = 5400;//50%
    s_iot_parameter_str.sidus_pro_fx[TOUCHBAR_EFFECTS_TYPE] = 1080;//10%
    s_iot_parameter_str.sidus_pro_fx[MUSIC_EFFECTS_TYPE] = 1080;//10%
    s_iot_parameter_str.sidus_pro_fx[PICKER_EFFECTS_TYPE] = 1080;//10%
    s_iot_parameter_str.sidus_pro_fx[MAGIC_PROGRAM_TYPE] = 1080;//10%
    
    for(i = 0; i < MAX_ERROR_TYPE; i++, j++)
    {
        s_iot_parameter_str.error_msg[i] = j;
    }
}
/***********************************END OF FILE*******************************************/
