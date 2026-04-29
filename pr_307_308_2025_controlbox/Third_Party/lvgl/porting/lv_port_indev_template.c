/**
 * @file lv_port_indev_templ.c
 *
 */

 /*Copy this file as "lv_port_indev.c" and set this value to "1" to enable content*/
#if 1

/*********************
 *      INCLUDES
 *********************/
#include "lv_port_indev_template.h"
#include "hc32_ddl.h"     
#include "UI_Indev.h"
#include "hal_timer.h"
#include "user.h"
//#include "user_io.h"

/*********************
 *      DEFINES
 *********************/
#define UI_KEY_ENTER_PORT       GPIO_PORT_F
#define UI_KEY_ENTER_PIN        GPIO_PIN_15

#define UI_KEY1_PORT            GPIO_PORT_H
#define UI_KEY1_PIN             GPIO_PIN_13    

#define UI_KEY2_PORT            GPIO_PORT_H
#define UI_KEY2_PIN             GPIO_PIN_14

#define UI_KEY3_PORT            GPIO_PORT_I
#define UI_KEY3_PIN             GPIO_PIN_00

#define UI_KEY4_PORT            GPIO_PORT_H
#define UI_KEY4_PIN             GPIO_PIN_15



/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

static void keypad_init(void);
static void keypad_read(lv_indev_drv_t * indev_drv, lv_indev_data_t * data);
static uint32_t keypad_get_key(void);

static void encoder_init(void);
static void encoder_read(lv_indev_drv_t * indev_drv, lv_indev_data_t * data);

/**********************
 *  STATIC VARIABLES
 **********************/
lv_indev_t * indev_keypad;
lv_indev_t * indev_encoder;
uint32_t indev_dev_state = 0;
static uint8_t s_indev_input_state = 0;


uint32_t indev_get_dev_input_state(void)
{
	return s_indev_input_state;
}

void indev_set_dev_input_state(uint8_t state)
{
    s_indev_input_state = state;
}

void lv_port_indev_init(void)
{
    static lv_indev_drv_t indev_drv;
    /*------------------
     * Keypad
     * -----------------*/

    /*Initialize your keypad or keyboard if you have*/
    keypad_init();

    /*Register a keypad input device*/
    lv_indev_drv_init(&indev_drv);
    indev_drv.type = LV_INDEV_TYPE_KEYPAD;
    indev_drv.read_cb = keypad_read;
    indev_keypad = lv_indev_drv_register(&indev_drv);

    /*Later you should create group(s) with `lv_group_t * group = lv_group_create()`,
     *add objects to the group with `lv_group_add_obj(group, obj)`
     *and assign this input device to group to navigate in it:
     *`lv_indev_set_group(indev_keypad, group);`*/

    /*------------------
     * Encoder
     * -----------------*/
    static lv_indev_drv_t enc_indev_drv;
    /*Initialize your encoder if you have*/
    encoder_init();

    /*Register a encoder input device*/
    lv_indev_drv_init(&enc_indev_drv);
    enc_indev_drv.type = LV_INDEV_TYPE_ENCODER;
    enc_indev_drv.read_cb = encoder_read;
    indev_encoder = lv_indev_drv_register(&enc_indev_drv);

}

uint8_t menu_keys_get_value(void)
{
    const uint32_t keys[4][2] = {
        {UI_KEY1_PORT, UI_KEY1_PIN},
        {UI_KEY2_PORT, UI_KEY2_PIN},
        {UI_KEY3_PORT, UI_KEY3_PIN},
        {UI_KEY4_PORT, UI_KEY4_PIN},
    };
    uint8_t keys_value = 0;
    for (uint8_t i = 0; i < 4; i++)
    {
        if (GPIO_ReadInputPins(keys[i][0], keys[i][1]) == Pin_Reset)
        {
            keys_value |= (1 << i);
        }
    }

    return keys_value;
}

uint8_t factory_mode_keys_pressed(void)
{
    uint8_t keys_value = menu_keys_get_value();
    if (keys_value == ((1 << 1) | (1 << 3)))
    {
        return 1;
    }

    return 0;
}

uint8_t calibartion_mode_keys_pressed(void)
{
    uint8_t keys_value = menu_keys_get_value();
    if (keys_value == ((1 << 0) | (1 << 2)))
    {
        return 1;
    }
    return 0;
}

uint8_t restore_factory_setting_keys_pressed(void)
{
    uint8_t keys_value = menu_keys_get_value();
    if (keys_value == (1 << 0))
    {
        return 1;
    }

    return 0;
}

void menu_keys_trigger_test_page(void)
{
	static uint16_t keytime[3] = {0} ;
	
	if(factory_mode_keys_pressed() == 1)
	{
		keytime[0]++;
	}
	else
	{
		keytime[0] = 0;
	}
	if(restore_factory_setting_keys_pressed() == 1)
	{
		keytime[1]++;
	}
	else
	{
		keytime[1] = 0;
	}
	
	if (keytime[0] > 1000)
	{
		if( (screen_get_act_pid() != PAGE_TEST && screen_get_act_pid() != PAGE_CALIBRATION_MODE) \
            && (screen_get_act_pid() != PAGE_UPDATE|| (screen_get_act_pid() == PAGE_UPDATE && screen_get_act_spid() < 3)))
		{
			user_turn_to_page(PAGE_TEST,0,false);
		}
	}
	if(keytime[1] > 300)
	{
		if(screen_get_act_pid() != PAGE_UPDATE || (screen_get_act_pid() == PAGE_UPDATE && screen_get_act_spid() < 3))
		{
			user_turn_to_page(PAGE_FACTORY, 3, false);
		}
	}
    
    if (calibartion_mode_keys_pressed() == 1)
    {
        keytime[2]++;
    }
    else
    {
        keytime[2] = 0;
    }
    
    if (keytime[2] > 600)
    {
        if( (screen_get_act_pid() != PAGE_TEST && screen_get_act_pid() != PAGE_CALIBRATION_MODE) \
            && (screen_get_act_pid() != PAGE_UPDATE|| (screen_get_act_pid() == PAGE_UPDATE && screen_get_act_spid() < 3)))
        {
            user_turn_to_page(PAGE_CALIBRATION_MODE, 0, false);
        }
    }
}


static void keypad_init(void)
{
    /*Your code comes here*/
//key1 138
//key2 139
//key3 140
//key4 141
    /*Your code comes here*/
    stc_gpio_init_t stcGpioInit;

    (void)GPIO_StructInit(&stcGpioInit);
    stcGpioInit.u16PinDir = PIN_DIR_IN;
    (void)GPIO_Init(UI_KEY1_PORT, UI_KEY1_PIN, &stcGpioInit);
    (void)GPIO_Init(UI_KEY2_PORT, UI_KEY2_PIN, &stcGpioInit);
    (void)GPIO_Init(UI_KEY3_PORT, UI_KEY3_PIN, &stcGpioInit);
    (void)GPIO_Init(UI_KEY4_PORT, UI_KEY4_PIN, &stcGpioInit);
}


/*Will be called by the library to read the mouse*/
static void keypad_read(lv_indev_drv_t * indev_drv, lv_indev_data_t * data)
{
    static uint32_t last_key = 0;

    /*Get the current x and y coordinates*/
    //mouse_get_xy(&data->point.x, &data->point.y);

    /*Get whether the a key is pressed and save the pressed key*/
    uint32_t act_key = keypad_get_key();
    if(act_key != 0) {
        data->state = LV_INDEV_STATE_PR;
        /*Translate the keys to LVGL control characters according to your key definitions*/
        switch(act_key) {
        case 1:
            act_key = LV_KEY_DEL;
            break;
        case 2:
		    act_key = LV_KEY_HOME;
            break;			           
        case 3:
			act_key = LV_KEY_END;
            break;
        case 4:
            act_key = LV_KEY_BACKSPACE;
            break;
        case 5:
            act_key = LV_KEY_ENTER;
            break;
        }
		indev_dev_state |= (1<<0);
        last_key = act_key;
        indev_set_dev_input_state(1);
    } else {
        data->state = LV_INDEV_STATE_REL;
		indev_dev_state &= ~(1<<0);
    }

    data->key = last_key;
}

/*Get the currently being pressed key.  0 if no key is pressed*/
static uint32_t keypad_get_key(void)
{
    /*Your code comes here*/    
	if (indev_get_lock())
    {
		return 0;
	}

    static uint8_t is_keys_pressed = 0;
    uint8_t keys_value = menu_keys_get_value();
    uint8_t key = 0;
        
        
    switch (keys_value)
    {
        case 0: key = 0; is_keys_pressed = 0;break;                
        case 1: key = 1; break;
        case 2: key = 2; break;
        case 4: key = 3; break;
        case 8: key = 4; break;
        default: key = 0;   is_keys_pressed = 1; break;     
    }
        
	if (is_keys_pressed)
	{
		key = 0;
	}

    return key;
}

/*------------------
 * Encoder
 * -----------------*/

/*Initialize your keypad*/
static void encoder_init(void)
{
    /*Your code comes here*/
    stc_gpio_init_t stcGpioInit;

    (void)GPIO_StructInit(&stcGpioInit);
    stcGpioInit.u16PinDir = PIN_DIR_IN;
    stcGpioInit.u16PullUp = PIN_PU_ON;
    (void)GPIO_Init(UI_KEY_ENTER_PORT, UI_KEY_ENTER_PIN, &stcGpioInit);
	 hal_timer_encoder_cnt_init();
}
 en_pin_state_t pin_state;
bool encoder_get_key_enter(void)
{
    pin_state = GPIO_ReadInputPins(UI_KEY_ENTER_PORT, UI_KEY_ENTER_PIN);
    if (pin_state == Pin_Reset)
    {
		indev_dev_state |= (1<<1);
		indev_set_dev_input_state(1);
        return true;
    }
	indev_dev_state &= ~(1<<1);
    return false;
}

/*Will be called by the library to read the encoder*/
static void encoder_read(lv_indev_drv_t * indev_drv, lv_indev_data_t * data)
{
	
    //extern int32_t encoder_cnt ;
	
	int32_t cnt = hal_get_encoder_cut() / 2;
	
    enc_read_proc(data, cnt, encoder_get_key_enter());
	if (cnt != 0 )
	{
		indev_dev_state |= (1<<2);
		hal_encoder_tmr_clear();	
        indev_set_dev_input_state(1);
	}
	else
	{
		indev_dev_state &= ~(1<<2);
	}
    //encoder_cnt = 0;
}

#else /*Enable this file at the top*/

/*This dummy typedef exists purely to silence -Wpedantic.*/
typedef int keep_pedantic_happy;
#endif

