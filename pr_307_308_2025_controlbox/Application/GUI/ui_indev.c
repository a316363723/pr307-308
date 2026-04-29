/*********************
 *      INCLUDES
 *********************/
#include "ui_config.h"
#include "ui_data.h"
#include "ui_indev.h"
#include "lvgl/lvgl.h"

/*********************
 *      DEFINES
 *********************/
#define ENCODER_OFFSET_LEVEL0_INDEX  0
#define ENCODER_OFFSET_LEVEL1_INDEX  1
#define ENCODER_OFFSET_LEVEL2_INDEX  2
#define ENCODER_OFFSET_LEVEL3_INDEX  3
/*********************
 *  STATIC FUNCTIONS
 *********************/
static void enc_level_proc(int32_t enc_diff);

/*********************
 *  STATIC VARIABLE
 *********************/
static uint32_t enc_step_index = ENCODER_OFFSET_LEVEL0_INDEX;
static bool indev_locked = false;

/*********************
 *  GLOBAL FUNCTIONS
 *********************/
uint32_t enc_get_step_index(void)
{
    return enc_step_index;
}

int32_t comm_enc_get_step(uint32_t value)
{
    const int32_t step_val[] = {ENCODER_OFFSET_LEVEL0_STEP, ENCODER_OFFSET_LEVEL1_STEP, 
                                ENCODER_OFFSET_LEVEL2_STEP, ENCODER_OFFSET_LEVEL3_STEP};
    
    if (value >= GET_ARRAY_SIZE(step_val))
        value = GET_ARRAY_SIZE(step_val) - 1;
    
    return step_val[value];
}

int32_t motor_enc_get_step(uint32_t value)
{
    const int32_t step_val[] = {ENCODER_OFFSET_LEVEL0, ENCODER_OFFSET_LEVEL0, 
                                ENCODER_OFFSET_LEVEL0, ENCODER_OFFSET_LEVEL0};
    
    if (value >= GET_ARRAY_SIZE(step_val))
        value = GET_ARRAY_SIZE(step_val) - 1;
    
    return step_val[value];
}

int32_t xy_enc_get_step(uint32_t value)
{
    const int32_t step_val[] = {1, 10, 100, 400};
    
    if (value >= GET_ARRAY_SIZE(step_val))
        value = GET_ARRAY_SIZE(step_val) - 1;
    
    return step_val[value];
}

int32_t universe_enc_get_step(uint32_t value)
{
    const int32_t step_val[] = {1, 50, 500, 1000};
    
    if (value >= GET_ARRAY_SIZE(step_val))
        value = GET_ARRAY_SIZE(step_val) - 1;
    
    return step_val[value];
}

int32_t adv_hsi_hue_get_step(uint32_t value)
{
//    const int32_t step_val[] = {1, 10, 50, 200};
    const int32_t step_val[] = {ENCODER_OFFSET_LEVEL0_STEP, ENCODER_OFFSET_LEVEL1_STEP, 
                                ENCODER_OFFSET_LEVEL2_STEP, ENCODER_OFFSET_LEVEL3_STEP};
	
    if (value >= GET_ARRAY_SIZE(step_val))
        value = GET_ARRAY_SIZE(step_val) - 1;
    
    return step_val[value];
}

int32_t common_intensity_get_step(uint32_t value)
{
    const int32_t step_val[] = {1, 1, 5, 20};
    
    if (value >= GET_ARRAY_SIZE(step_val))
        value = GET_ARRAY_SIZE(step_val) - 1;
    
    return step_val[value];	
}

int32_t common_angle_get_step(uint32_t value)
{
    const int32_t step_val[] = { 1, 2, 5, 10 };

    if (value >= GET_ARRAY_SIZE(step_val))
        value = GET_ARRAY_SIZE(step_val) - 1;

    return step_val[value];
}

void enc_read_proc(lv_indev_data_t* data, int32_t enc_cnt, bool pressed)
{
    if (indev_get_lock())
    {
        data->state = LV_INDEV_STATE_RELEASED;
        data->enc_diff = 0;
    }
    else 
    {
        data->state = pressed ? LV_INDEV_STATE_PRESSED : LV_INDEV_STATE_RELEASED;
        data->enc_diff = pressed ? 0 : enc_cnt;     
        enc_level_proc(data->enc_diff);
    }       
}

void indev_set_lock(bool locked)
{
    indev_locked = locked;
}

bool indev_get_lock(void)
{
    return indev_locked;
}

/*********************
 *  STATIC FUNCTIONS
 *********************/
static void enc_level_proc(int32_t enc_diff)
{
    uint32_t diff_abs = LV_ABS(enc_diff);
    uint32_t step = ENCODER_OFFSET_LEVEL0_INDEX;

    if (diff_abs >= ENCODER_OFFSET_LEVEL3)
    {
        step = ENCODER_OFFSET_LEVEL3_INDEX;
    }
    else if (diff_abs >= ENCODER_OFFSET_LEVEL2)
    {
        step = ENCODER_OFFSET_LEVEL2_INDEX;
    }
    else if (diff_abs >= ENCODER_OFFSET_LEVEL1)
    {
        step = ENCODER_OFFSET_LEVEL1_INDEX;
    }

    enc_step_index = step;
}
