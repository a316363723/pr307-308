
#include "hal_key.h"

TS_KEY_INFO KeyInfo;

void key_gpio_init(void)
{
    stc_gpio_init_t stcGpioInit = {0};
    (void)GPIO_StructInit(&stcGpioInit);
    stcGpioInit.u16PinDir = PIN_DIR_IN;
    (void)GPIO_Init(POWER_KEY_Port, POWER_KEY_pin, &stcGpioInit);

}

/*****************************************************************************************
* Function Name : GetKey
* Description   : 获取键值,返回值每一位代表一个按键按下，支持组合按键
* Arguments     : NONE
* Return Value	: 返回按键值
******************************************************************************************/
uint32_t GetKey(void)
{
    return KET_VALUE;
}
/*****************************************************************************************
* Function Name : set_key_val
* Description   : 设置键值
* Arguments     : NONE
* Return Value	: NONE
******************************************************************************************/
void set_key_val(te_key_event key_val)
{
    KeyInfo.KeyEvent = KEY_NULL;
    KeyInfo.KeyEventOsCpy = KEY_NULL;
    KeyInfo.KeyCur = KEY_NONE;
}
/*****************************************************************************************
* Function Name: KeyModuleProcess
* Description  : 按键检测流程
* Arguments    : 按键结构体指针
* Return Value : 返回TRUE表示有触发(按下或抬起)，FALSE表示无触发
******************************************************************************************/
static uint8_t KeyModuleProcess(TS_KEY_INFO* pKeyInfo)
{
    static uint32_t KeyLast = 0;
    uint32_t KeyCur = 0;

    KeyCur = GetKey();

    switch(pKeyInfo->KeyState)
    {
        case KEY_STATE_INIT:
            KeyLast = 0;
            pKeyInfo->KeyDwTmr = 0;

            if(KeyInfo.KeyEvent != KEY_NULL)
            {
                KeyInfo.KeyEventOsCpy = KeyInfo.KeyEvent;//在操作系统的环境中，用于备份按键事件
            }

            KeyInfo.KeyEvent = KEY_NULL;

            if(KeyCur != KEY_NONE)
            {
                pKeyInfo->KeyState = KEY_STATE_WOBBLE;
            }

            break;

        case KEY_STATE_WOBBLE://消抖
            pKeyInfo->KeyState = KEY_STATE_DWON;
            break;

        case KEY_STATE_DWON:
            pKeyInfo->KeyDwTmr++;

            if(KeyCur == KeyLast)
            {
                if(pKeyInfo->KeyDwTmr >= KEY_LONG_TMR) //长按
                {
                    pKeyInfo->KeyCur = KeyLast;
                    pKeyInfo->KeyEvent |= KEY_LONG;
                    pKeyInfo->KeyState = KEY_STATE_WAITUP;
                }
            }
            else
            {
                if(KeyCur == KEY_NONE)
                {
                    if(pKeyInfo->KeyDwTmr >= KEY_PRESSED_TMR) //点按
                    {
                        pKeyInfo->KeyCur = KeyLast;
                        pKeyInfo->KeyEvent |= KEY_PRESSED;
                        pKeyInfo->KeyState = KEY_STATE_INIT;
                    }
                }
                else
                {
                    pKeyInfo->KeyState = KEY_STATE_INIT;
                }
            }

            break;

        case KEY_STATE_WAITUP:
            if(KeyCur == KeyLast)
            {
                if(++pKeyInfo->KeyDwTmr >= KEY_REPEAT_TMR) //连发
                {
                    pKeyInfo->KeyCur = KeyLast;
                    pKeyInfo->KeyEvent |= KEY_REPEAT;
                    pKeyInfo->KeyDwTmr = 0;
                }
            }
            else
            {
                if(KeyCur == KEY_NONE)
                {
                    pKeyInfo->KeyCur = KeyLast;
                    pKeyInfo->KeyEvent |= KEY_UP;
                    pKeyInfo->KeyState = KEY_STATE_INIT;
                }
            }

            break;

        default:
            break;
    }

    if(KeyLast != KeyCur)
    {
        KeyLast = KeyCur;               //保存上次的按键值
    }

    return(false);
}
/*****************************************************************************************
* Function Name: KeyDrv
* Description  : 按键驱动函数,需放10ms的循环时间片中
* Arguments    : NONE
* Return Value : NONE
******************************************************************************************/
void key_poll(void)
{
    KeyModuleProcess(&KeyInfo);
}
/*****************************************************************************************
* Function Name: IsKey
* Description  : 按键事件判断API
* Arguments    : KeyId:按键ID / KeyEvent:按键事件类型
* Return Value : true:有KeyEvent事件发生 / fasle:无
******************************************************************************************/
bool get_Key_event(uint32_t KeyId, te_key_event KeyEvent)
{

    if(KeyInfo.KeyCur == KeyId)
    {
        if(KeyInfo.KeyEvent & KeyEvent || KeyInfo.KeyEventOsCpy & KeyEvent)
        {
            KeyInfo.KeyEvent = KEY_NULL;
            KeyInfo.KeyEventOsCpy = KEY_NULL;
            KeyInfo.KeyCur = KEY_NONE;
            return true;
        }
    }

    return false;
}


/*电源按键开启*/
void key_power_enabled(void)
{

    KeyInfo.keyLock = 1;

}
/*电源按键关闭*/
void key_power_unenabled(void)
{
    KeyInfo.keyLock = 0;

}




