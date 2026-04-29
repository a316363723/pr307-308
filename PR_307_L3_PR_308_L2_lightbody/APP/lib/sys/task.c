/****************************************************************************************
**  Filename :  task.c
**  Abstract :  LED模块化程序，支持设置LED常亮，常灭，闪烁周期设置，闪烁次数设置等功能。
**  By       :  何建国
**  Date     :  2018-11-21
**  Changelog:1.First Create
*****************************************************************************************/

/*****************************************************************************************
									Includes
*****************************************************************************************/
#include "task.h" 
/*****************************************************************************************
							  Variables definitions
*****************************************************************************************/
static TASK_COMPONENTS s_TaskList[MAX_TASK_NUMBER];
/*****************************************************************************************
							  Functions definitions
*****************************************************************************************/


/*****************************************************************************************
* Function Name: TaskDeInit
* Description  : 任务结构体初始化
* Arguments    : NONE
* Return Value : NONE
******************************************************************************************/
void TaskDeInit(void)
{
    uint8_t i = 0;
    for(i = 0; i < MAX_TASK_NUMBER; i++)
    {
        s_TaskList[i].Run = 0;
        s_TaskList[i].Timer = 0;
        s_TaskList[i].ItvTime = 0;
        s_TaskList[i].TaskHook = NULL; 
        s_TaskList[i].TaskHook_Interrupt = NULL;
    } 
}
/*****************************************************************************************
* Function Name: Task_Init
* Description  : 任务初始化
* Arguments    : NONE
* Return Value : NONE
******************************************************************************************/
void Task_Init(uint8_t Run, uint16_t Timer, uint16_t ItvTime, void (*TaskFun)(void), void (*TaskIntFun)(void))
{
    static uint8_t task_sel = 0;
    if(task_sel >= MAX_TASK_NUMBER)
    {
		#if(1 == GLOBAL_PRINT_ENABLE)
//        printf("Task stack is overflow!");
		#endif
        return;
    }
    s_TaskList[task_sel].Run = Run;
    s_TaskList[task_sel].Timer = Timer;
    s_TaskList[task_sel].ItvTime = ItvTime;
    s_TaskList[task_sel].TaskHook = TaskFun;
    s_TaskList[task_sel].TaskHook_Interrupt = TaskIntFun;
    task_sel++;
}
/*****************************************************************************************
* Function Name: TaskRemarks
* Description  : 任务服务程序(放入1ms循环时间片段中)
* Arguments    : NONE
* Return Value : NONE
******************************************************************************************/
static void TaskRemarks(void)
{
    uint8_t i = 0;
    for (i = 0; i < MAX_TASK_NUMBER && s_TaskList[i].TaskHook != NULL; i++) // 逐个任务时间处理
    {
        if (s_TaskList[i].Timer) // 时间不为0
        {
            s_TaskList[i].Timer--; // 减去一个节拍
            if (s_TaskList[i].Timer == 0) // 时间减完了
            {
                s_TaskList[i].Timer = s_TaskList[i].ItvTime; // 恢复计时器值，从新下一次
                s_TaskList[i].Run = 1; // 任务可以运行
                if(s_TaskList[i].TaskHook_Interrupt != NULL)
                {
                    s_TaskList[i].TaskHook_Interrupt();
                }
            }
        }
    }
}
/*****************************************************************************************
* Function Name: TaskProcess
* Description  : 任务处理程序
* Arguments    : NONE
* Return Value : NONE
******************************************************************************************/
void TaskProcess(void)
{
    uint8_t i = 0;
    for (i = 0; i < MAX_TASK_NUMBER && s_TaskList[i].TaskHook != NULL; i++) // 逐个任务时间处理
    {
        if (s_TaskList[i].Run) // 时间不为0
        {
            s_TaskList[i].TaskHook(); // 运行任务
            s_TaskList[i].Run = 0; // 标志清0
        }
    }
}
/*****************************************************************************************
* Function Name: HAL_SYSTICK_Callback
* Description  : 滴答时钟回调函数（周期1ms）
* Arguments    : NONE
* Return Value : NONE
******************************************************************************************/
void HAL_SYSTICK_Callback(void)
{
	//TITO:
    TaskRemarks();
}
/***********************************END OF FILE*******************************************/
