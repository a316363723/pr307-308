/*********************
 *      INCLUDES
 *********************/
#include "ui_common.h"
#include "page_widget.h"
#include "user.h"

/*********************
 *      DEFINES
 *********************/
#define  ANIM_TASK_PERIOD    200
#define  BATT_STATUS_CONT_WIDTH
#define  BATT_STATUS_CONT_HEIGHT
/*********************
 *      TPEDEFS
 *********************/
typedef struct _local_var{
    lv_obj_t* scr;
    lv_obj_t* lightning;
    lv_obj_t* batt_cont[2];    
    lv_obj_t* label[2];
    lv_timer_t* timer;    
    batt_sys_t batt_data;    
}local_var_t;

enum {
    BATT_CONT_NO_BATT = 0,
    BATT_CONT_LOW_POWER,
    BATT_CONT_NORMAL,
};

/**********************
 *  STATIC PROTOTYPES
 **********************/
static lv_obj_t* batt_cont_create(lv_obj_t* panel, batt_dsc_t* p_batt, uint8_t direction);
static void scr_event_cb(lv_event_t* e);
static void batt_monitor_cb(struct _lv_timer_t *t);
static void refresh_batt_percent_cont(lv_obj_t* cont, uint8_t cnt, lv_color_t color);
static void refresh_batt_cont(lv_obj_t* cont, batt_dsc_t* p_batt);
static void batt_cont_init(lv_obj_t* cont, batt_dsc_t* p_batt, uint8_t batt_state, uint8_t direction);
static void refresh_lightning_img(void);
static void refresh_label(lv_obj_t* label, batt_dsc_t* p_batt, uint8_t direction);
static void refresh_screen(bool force_upd);
static uint8_t get_batt_state(batt_dsc_t* p_batt_dsc);
/*********************
 *  STATIC VATIABLES
 *********************/
static batt_sys_t* p_batt_sys;
static bool deleted = false;
lv_group_t* group;
local_var_t local_var;
local_var_t* p_local_var = &local_var;
/*********************
 *  GLOBAL VATIABLES
 *********************/

/*********************
 *  GLOBAL FUNCTIONS
 *********************/
void batt_anim_exec_xcb(void *a, int32_t value)
{
    refresh_batt_percent_cont(a, value, lv_color_white());
}

/**
 * @brief 创建一个显示3s的低电量界面
 * 
 */
void screen_low_power_create(void)
{
    group = lv_group_create();
    p_batt_sys = &g_ptUIStateData->batt_sys;

#ifndef UI_PLATFORM_MCU 
    p_batt_sys->left.is_exist = true;
    p_batt_sys->right.is_exist = true;
    p_batt_sys->left.is_low_power = true;
    p_batt_sys->right.is_low_power = false;
    p_batt_sys->left.state = BATT_STATE_IDLE;
    p_batt_sys->right.state = BATT_STATE_CHARGING;
    p_batt_sys->left.percent = 50;
    p_batt_sys->right.percent = 80;
#endif    

    deleted = false;
    p_local_var->timer = NULL;
    p_local_var->scr = NULL;

    p_local_var->scr = lv_obj_create(lv_scr_act());
    lv_obj_set_style_bg_color(p_local_var->scr, lv_color_black(), 0);
    lv_obj_set_style_border_width(p_local_var->scr, 0, 0);
    lv_obj_set_style_pad_all(p_local_var->scr, 0, 0);    
    lv_obj_clear_flag(p_local_var->scr, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_size(p_local_var->scr, lv_pct(100), lv_pct(100));
    lv_obj_center(p_local_var->scr);
    lv_obj_add_event_cb(p_local_var->scr, scr_event_cb, LV_EVENT_ALL, NULL);
        
    LV_IMG_DECLARE(ImgLowPowerBg)
    
    lv_obj_t* panel = lv_obj_create(p_local_var->scr);
    lv_obj_set_size(panel, 200, 220);
    lv_obj_clear_flag(panel, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_border_width(panel, 0, 0);
    lv_obj_set_style_pad_all(panel, 0, 0);    
    lv_obj_set_style_text_font(panel, Font_ResouceGet(FONT_18), 0);
    lv_obj_set_style_text_color(panel, lv_color_white(), 0);
    lv_obj_set_style_bg_color(panel, lv_color_black(), 0);
    lv_obj_align(panel, LV_ALIGN_TOP_MID, 0, 0);

    lv_obj_t* img = lv_img_create(panel);
    lv_img_set_src(img, &ImgLowPowerBg);
    lv_obj_set_style_img_recolor_opa(img, LV_OPA_100, 0);
    lv_obj_set_style_img_recolor(img, lv_color_white(), 0);
    lv_obj_center(img);

    LV_IMG_DECLARE(ImgChargingLightning)
    p_local_var->lightning  = lv_img_create(panel);
    lv_img_set_src(p_local_var->lightning, &ImgChargingLightning);
    lv_obj_center(p_local_var->lightning);
    
    p_local_var->batt_cont[0] = batt_cont_create(panel, &p_batt_sys->left,  LV_ALIGN_BOTTOM_LEFT);
    p_local_var->label[0] = lv_obj_get_child(panel, -1);
    p_local_var->batt_cont[1] = batt_cont_create(panel, &p_batt_sys->right, LV_ALIGN_BOTTOM_RIGHT);
    p_local_var->label[1] = lv_obj_get_child(panel, -1);

    my_indev_set_group(group);
    lv_group_add_obj(group, p_local_var->scr);
    lv_group_focus_obj(p_local_var->scr);
    lv_group_set_editing(group, true);

    refresh_screen(true);
    g_ptUIStateData->refresh = false;
#if  1
    p_local_var->timer = lv_timer_create(batt_monitor_cb, 2 * 1000, NULL);
    lv_timer_set_repeat_count(p_local_var->timer, LV_ANIM_REPEAT_INFINITE);
#endif    
}

/**
 * @brief 低电量界面退出函数
 * 
 */
void screen_low_power_exit(void)
{
    uint32_t key = LV_KEY_ESC;
    if (p_local_var->scr != NULL)
        lv_event_send(p_local_var->scr, LV_EVENT_KEY, &key);      
}

/**********************
 *   STATIC FUNCTIONS
 **********************/
/**
 * @brief 1s调用1次电池检测函数
 * 
 * @param t 
 */
static void batt_monitor_cb(struct _lv_timer_t *t)
{
    if (deleted)    return;

    refresh_screen(false);
    g_ptUIStateData->refresh = true;
}

/**
 * @brief 屏幕事件回调函数
 * 
 * @param e 
 */
static void scr_event_cb(lv_event_t* e)
{
    lv_event_code_t event = lv_event_get_code(e);
    lv_obj_t* panel = lv_event_get_target(e);

    if (LV_EVENT_KEY == event)
    {                         
        if (!deleted)       
        {
            deleted = true;
            lv_obj_del_async(p_local_var->scr);
            lv_timer_pause(p_local_var->timer);                        
        
            /* 设置默认组 */
            my_indev_set_group(lv_group_get_default());                 
        }        
    }
    else if (LV_EVENT_DELETE == event)
    {
        /* 设置删除标志 */
        deleted = true;

        /* 删除组 */
        lv_group_focus_freeze(group, true);
        lv_group_remove_all_objs(group);
        lv_group_del(group);
        group = NULL;

        /* 释放定时器资源 */
        lv_timer_pause(p_local_var->timer);
        lv_timer_del(p_local_var->timer);        
        memset(p_local_var, 0, sizeof(local_var_t));     

        /* 设置交互状态机 */
        user_exit_low_volt_page();
    }
}

/**
 * @brief 刷新整个屏幕
 * 
 */
static void refresh_screen(bool force_upd)
{
    static uint8_t left_state; 
    static uint8_t right_state;
    uint8_t state;
    
    /* 刷新左边容器 */
    state = get_batt_state(&p_batt_sys->left);
    if (left_state != state || force_upd)
    {
        left_state = state;
        batt_cont_init(p_local_var->batt_cont[0], &p_batt_sys->left, left_state, LV_ALIGN_BOTTOM_LEFT);
    }
    refresh_batt_cont(p_local_var->batt_cont[0], &p_batt_sys->left);

    /* 刷新右边容器 */
    state = get_batt_state(&p_batt_sys->right);
    if (right_state != state || force_upd)
    {
        right_state = state;
        batt_cont_init(p_local_var->batt_cont[1], &p_batt_sys->right, right_state, LV_ALIGN_BOTTOM_RIGHT);
    }

    refresh_batt_cont(p_local_var->batt_cont[1], &p_batt_sys->right);

    /* 刷新百分比 */
    refresh_label(p_local_var->label[0], &p_batt_sys->left, LV_ALIGN_BOTTOM_LEFT);
    refresh_label(p_local_var->label[1], &p_batt_sys->right, LV_ALIGN_BOTTOM_RIGHT);

    /* 刷新闪电图标 */
    refresh_lightning_img();
}

/**
 * @brief 容器的初始化.
 * 
 * @param cont 
 * @param p_batt 
 * @param direction 
 * @return lv_obj_t* 
 */
static void batt_cont_init(lv_obj_t* cont, batt_dsc_t* p_batt, uint8_t batt_state, uint8_t direction)
{
    LV_IMG_DECLARE(ImgLowPowerExclamation);
    LV_IMG_DECLARE(ImgPowerEnergy);

    lv_obj_clean(cont);
    lv_obj_remove_style_all(cont);
    lv_anim_del(cont, NULL);
    /* 显示感叹图标 */
    if (batt_state == BATT_CONT_LOW_POWER)
    {
        lv_obj_set_size(cont, 50, 100);

        lv_obj_t* img = lv_img_create(cont);
        lv_img_set_src(img, &ImgLowPowerExclamation);
        lv_obj_set_style_img_recolor_opa(img, LV_OPA_100, 0);
        lv_obj_set_style_img_recolor(img, lv_color_make(255, 0, 0), 0);
        lv_obj_center(img);   

        /* 设置坐标 */
        if (direction == LV_ALIGN_BOTTOM_LEFT)
        {
            lv_obj_align(cont, direction, 6, -25);
        }
        else if (direction == LV_ALIGN_BOTTOM_RIGHT)
        {
            lv_obj_align(cont, direction, -6, -25);
        }
    }
    /* 正常显示电池 */
    else if (batt_state == BATT_CONT_NORMAL)
    {
        lv_obj_set_size(cont, 50, LV_SIZE_CONTENT);
        lv_obj_set_flex_flow(cont, LV_FLEX_FLOW_COLUMN);
        lv_obj_set_style_pad_gap(cont, 2, 0);
        lv_obj_t* img;

        for (uint8_t i = 0; i < 11; i++)
        {
            img = lv_img_create(cont);
            lv_img_set_src(img, &ImgPowerEnergy);
            lv_obj_set_style_img_recolor_opa(img, LV_OPA_100, 0);
            lv_obj_set_style_img_recolor(img, lv_color_white(), 0);
        }

        /* 设置坐标 */
        if (direction == LV_ALIGN_BOTTOM_LEFT)
        {
            lv_obj_align(cont, direction, 10, -35);            
        }
        else if (direction == LV_ALIGN_BOTTOM_RIGHT)
        {
            lv_obj_align(cont, direction, -6, -35);
        }

        /* 设置充电动画 */
        if (p_batt->state == BATT_STATE_CHARGING)
        {
            lv_anim_t a;
            lv_anim_init(&a);
            lv_anim_set_var(&a, cont);
            lv_anim_set_values(&a, lv_obj_get_child_cnt(cont), 0);
            lv_anim_set_repeat_count(&a, LV_ANIM_REPEAT_INFINITE);
            lv_anim_set_time(&a, 12000);
            lv_anim_set_exec_cb(&a, batt_anim_exec_xcb);
            lv_anim_start(&a);
        }
        else 
        {
            /* 刷新电池容量 */
            refresh_batt_cont(cont, p_batt);
        }
    }
    else if (batt_state == BATT_CONT_NO_BATT)
    {                
        /* 设置容器固定高宽 */
        lv_obj_set_size(cont, 50, 100);     

        /* 显示无电池 */
        lv_obj_t* label = lv_label_create(cont);
        lv_obj_set_style_text_font(label, Font_ResouceGet(FONT_18), 0);
        lv_obj_set_height(label, 20);
        lv_obj_set_width(label, 20);
        lv_label_set_text(label, "-.-");        
        
        /* 设置坐标 */
        if (direction == LV_ALIGN_BOTTOM_LEFT)
        {
            lv_obj_align(label, LV_ALIGN_CENTER, 3,  -5);
            lv_obj_align(cont, direction, 6, -25);
        }
        else if (direction == LV_ALIGN_BOTTOM_RIGHT)
        {
            lv_obj_align(label, LV_ALIGN_CENTER, -3,  -5);
            lv_obj_align(cont, direction, -6, -25);
        }
    }

}

/**
 * @brief 创建电池显示容器并返回对象
 * 
 * @param panel 
 * @param p_batt 
 * @param direction 
 * @return lv_obj_t* 
 */
static lv_obj_t* batt_cont_create(lv_obj_t* panel, batt_dsc_t* p_batt, uint8_t direction)
{
    LV_IMG_DECLARE(ImgLowPowerExclamation);
    LV_IMG_DECLARE(ImgPowerEnergy);

    lv_obj_t* batt_cont = lv_obj_create(panel);
    lv_obj_t* label = lv_label_create(panel);
    
    uint8_t state = get_batt_state(p_batt);

    batt_cont_init(batt_cont, p_batt, state, direction);
    
    if (BATT_CONT_NORMAL == state)
    {
        lv_label_set_text_fmt(label, "%d%%", p_batt->percent);
        lv_obj_align_to(label, batt_cont, LV_ALIGN_OUT_BOTTOM_MID, 0, 6);        
    }
    else if (BATT_CONT_NO_BATT == state)
    {
        //lv_obj_align_to(label, batt_cont, LV_ALIGN_OUT_BOTTOM_MID, 0, 6);
        lv_obj_align(batt_cont, direction, -6, -35);
    }
    else if (BATT_CONT_LOW_POWER == state)
    {
        lv_label_set_text(label, Lang_GetStringByID(STRING_ID_LOW));
        lv_obj_align_to(label, batt_cont, LV_ALIGN_OUT_BOTTOM_MID, 0, -4);        
    }

    return batt_cont;
}

/**
 * @brief 刷新电池显示容器.
 * 
 * @param cont 
 * @param p_batt 
 */
static void refresh_batt_cont(lv_obj_t* cont, batt_dsc_t* p_batt)
{
    uint8_t state = get_batt_state(p_batt);

    if (BATT_CONT_NORMAL == state)
    {
        if (p_batt->state != BATT_STATE_CHARGING)
        {
            uint8_t percent = lv_obj_get_child_cnt(cont) - lv_map(p_batt->percent, 0, 100, 1, lv_obj_get_child_cnt(cont));
            lv_color_t color = lv_color_white();

            lv_anim_del(cont, NULL);
            refresh_batt_percent_cont(cont, percent, color);
        }
        else
        {
            lv_anim_t* cur_anim = lv_anim_get(cont, batt_anim_exec_xcb);
            if (cur_anim == NULL)
            {
                lv_anim_t a;
                lv_anim_init(&a);
                lv_anim_set_var(&a, cont);
                lv_anim_set_values(&a, lv_obj_get_child_cnt(cont), 0);
                lv_anim_set_repeat_count(&a, LV_ANIM_REPEAT_INFINITE);
                lv_anim_set_time(&a, 12000);
                lv_anim_set_exec_cb(&a, batt_anim_exec_xcb);
                lv_anim_start(&a);
            }
        }
    }
}

/**
 * @brief 刷新电池容量百分比
 * 
 * @param cont 
 * @param cnt 
 * @param color 
 */
static void refresh_batt_percent_cont(lv_obj_t* cont, uint8_t cnt, lv_color_t color)
{
    uint8_t sum = lv_obj_get_child_cnt(cont);
    for (uint8_t i = 0; i < sum; i++)
    {
        lv_obj_t* child = lv_obj_get_child(cont, i);
            
        if (child != NULL && i < cnt)
        {
            lv_obj_add_flag(child, LV_OBJ_FLAG_HIDDEN);            
        }
        else
        {
            lv_obj_clear_flag(child, LV_OBJ_FLAG_HIDDEN);
            lv_obj_set_style_img_recolor_opa(child, LV_OPA_100, 0);
            lv_obj_set_style_img_recolor(child, color, 0);
        }                
    }
}

/**
 * @brief 刷新左右显示的标签
 * 
 * @param label 
 * @param p_batt 
 * @param direction 
 */
static void refresh_label(lv_obj_t* label, batt_dsc_t* p_batt, uint8_t direction)
{
    uint8_t state = get_batt_state(p_batt);

    if (BATT_CONT_NORMAL == state)
    {
        lv_label_set_text_fmt(label, "%d%%", p_batt->percent);
        lv_obj_clear_flag(label, LV_OBJ_FLAG_HIDDEN);
    }
    else if (BATT_CONT_NO_BATT == state)
    {
        lv_obj_add_flag(label, LV_OBJ_FLAG_HIDDEN);
    }
    else if (BATT_CONT_LOW_POWER == state)
    {
        lv_label_set_text(label, Lang_GetStringByID(STRING_ID_LOW));
        lv_obj_clear_flag(label, LV_OBJ_FLAG_HIDDEN);        
    }

    if (direction == LV_ALIGN_BOTTOM_RIGHT)
        lv_obj_align(label, LV_ALIGN_BOTTOM_RIGHT, -15, 0); 
    else 
        lv_obj_align(label, LV_ALIGN_BOTTOM_LEFT, 15, 0);
}

/**
 * @brief 刷新闪电标志
 * 
 */
static void refresh_lightning_img(void)
{    
    uint8_t left_state = get_batt_state(&p_batt_sys->left);
    uint8_t right_state = get_batt_state(&p_batt_sys->right);

    if (left_state == BATT_CONT_NORMAL && (p_batt_sys->left.state != BATT_STATE_IDLE))
    {
        lv_obj_clear_flag(p_local_var->lightning, LV_OBJ_FLAG_HIDDEN);
    }
    else if (right_state == BATT_CONT_NORMAL && (p_batt_sys->right.state != BATT_STATE_IDLE))
    {
        lv_obj_clear_flag(p_local_var->lightning, LV_OBJ_FLAG_HIDDEN);
    }
    else  
    {
        lv_obj_add_flag(p_local_var->lightning, LV_OBJ_FLAG_HIDDEN);   
    }
}

/**
 * @brief 获取电池容器显示状态
 * 
 * @param p_batt_dsc 
 * @return uint8_t 
 */
static uint8_t get_batt_state(batt_dsc_t* p_batt_dsc)
{    
    if (p_batt_dsc->is_exist)
    {
        if (p_batt_dsc->is_low_power)
        {
            return BATT_CONT_LOW_POWER;
        }
        else
        {
            return BATT_CONT_NORMAL;
        }
    }
    else
    {
        return BATT_CONT_NO_BATT;
    }
}
