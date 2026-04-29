/*********************
 *      INCLUDES
 *********************/
#include "../ui_common.h"
#include "../page_widget.h"
#include <stdio.h>
#include "app_power.h"


/*********************
 *      DEFINES
 *********************/

//校准模式界面子页面
enum {
    SUB_ID_CAL_ENTRY = 0,
    SUB_ID_CAL_INDUSTRIAL_MODE,
    SUB_ID_CAL_INDEPENDENT_COLOR_MODE,
    SUB_ID_CAL_POWER_CALIB,
    SUB_ID_CAL_POWER_CRC,
    SUB_ID_CAL_MAX,
};

#define FAN_MAX_SPD                 10000
#define FAN_MIN_SPD                 0
#if PROJECT_TYPE == 308
#define CHL_MAX_INT                 4800
#endif
#if PROJECT_TYPE == 307
#define CHL_MAX_INT                 3000
#endif
#define CHL_MIN_INT                 0
#define TEXT_COLOR                  lv_color_make(11, 244, 122)
#define CHL_DEFAULT_TEXT_COLOR      lv_color_make(255, 255, 255)
#define CHL_FOCUSED_TEXT_COLOR      lv_color_make(0, 0, 0)

/*********************
 *      TPEDEFS
 *********************/
/*******************/

typedef struct {
    my_list_ext_t list;
    union {
        struct {
            lv_obj_t* spd_label[5][2];
        } auto_vol;
        struct {
            lv_obj_t* power_label;
            lv_obj_t* state_label;
        }power_calib;
        struct {
            lv_obj_t* power_label;
            lv_obj_t* state_label;
        }single_power_calib;
    } sub_page;
}calmode_view_t;
/***********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void page_calmode_construct(void);
static void page_calmode_destruct(void);
static void page_calmode_upd_cb(uint32_t ms);
static void calmode_index_entry(lv_obj_t* parent);
static void calmode_industrial_init(lv_obj_t* parent);
static void calmode_independent_color_ctrl_init(lv_obj_t* parent);
static void calmode_power_powercalibrate_init(lv_obj_t* parent);
static void calmode_power_crc_init(lv_obj_t* parent);
static void calmode_refresh_power_calibrate(void);
static void calmode_refresh_power_crc(void);
static void independent_color_event_cb(lv_event_t* e);
static void calmode_list_event_cb(lv_event_t* e, uint8_t index);
static void calmode_common_event(lv_event_t* e);


/*********************
 *  STATIC VATIABLES
 *********************/
static lv_group_t *indev_group;
static UI_State_Data_t* p_model = NULL;
static calmode_view_t view;
static uint8_t fan_mode = 0;
/*********************
 *  GLOBAL VATIABLES
 *********************/
page_t page_calibration_mode = PAGE_INITIALIZER("Calibration Mode",
                                        PAGE_CALIBRATION_MODE,
                                        page_calmode_construct,
                                        page_calmode_destruct,
                                        page_calmode_upd_cb,
                                        NULL,
                                        PAGE_NONE);
/*********************
 *  GLOBAL FUNCTIONS
 *********************/

/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * @brief 校准模式页面构造函数
 * 
 */
static void page_calmode_construct(void)
{
    data_center_read_config_data(SYS_CONFIG_FAN_MODE, &fan_mode);
    
    indev_group = lv_group_create();
    my_indev_set_group(indev_group);
    lv_group_set_default(indev_group);
    lv_obj_set_style_bg_color(lv_scr_act(),  lv_color_make(40,40,40), 0);
    
    p_model = screen_get_act_page_data();
    
    lv_obj_clear_flag(lv_scr_act(), LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_t* scr = lv_obj_create(lv_scr_act());
    lv_obj_add_style(scr, &style_common_btn, 0);
    lv_obj_set_style_bg_color(scr, lv_color_black(), 0);
    lv_obj_set_size(scr, lv_pct(100), lv_pct(100));
    lv_obj_set_style_text_font(scr, Font_ResouceGet(FONT_14), 0);
    lv_obj_clear_flag(scr, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_center(scr);
    
    uint8_t subid = screen_get_act_spid() > SUB_ID_CAL_MAX ? 0 : screen_get_act_spid();
    
    switch (subid)
    {
        case SUB_ID_CAL_ENTRY:
            calmode_index_entry(scr);
            p_model->test_data.PwmMode = INDUSTRIAL_MODE_IDLE;
            break;
        case SUB_ID_CAL_INDUSTRIAL_MODE:            /* 工厂模式 */
            p_model->test_data.IndustrialMode = INDUSTRIAL_MODE_START;
            calmode_industrial_init(scr);
            lv_obj_add_event_cb(scr, calmode_common_event, LV_EVENT_CLICKED, NULL);
            lv_group_add_obj(indev_group, scr);
            lv_group_focus_obj(scr);
            break;
        case SUB_ID_CAL_INDEPENDENT_COLOR_MODE:     /* 单路模式 */
            p_model->test_data.PwmMode = INDUSTRIAL_MODE_START;
            calmode_independent_color_ctrl_init(scr);
            ui_test_rgbpwm_mode_set(1);
            break;
        case SUB_ID_CAL_POWER_CALIB:                /* 功率校准 */
            p_model->test_data.CalibrateState = CALIBRATE_POWER_PWM_START;
            calmode_power_powercalibrate_init(scr);
            lv_obj_add_event_cb(scr, calmode_common_event, LV_EVENT_CLICKED, NULL);
            lv_group_add_obj(indev_group, scr);
            lv_group_focus_obj(scr);
            break;
        case SUB_ID_CAL_POWER_CRC:                  /* 单路校准 */
            p_model->test_data.powercalibrateState = CALIBRATE_POWER_PWM_START;
            calmode_power_crc_init(scr);
            lv_obj_add_event_cb(scr, calmode_common_event, LV_EVENT_CLICKED, NULL);
            lv_group_add_obj(indev_group, scr);
            lv_group_focus_obj(scr);
            break;
        default: break;
    }
}

/**
 * @brief 校准模式页面析构函数
 * 
 */
static void page_calmode_destruct(void)
{
    ui_test_rgbpwm_mode_set(0);
    lv_group_focus_freeze(indev_group, true);
    lv_group_remove_all_objs(indev_group);
    lv_group_del(indev_group);
    indev_group = NULL;
    lv_obj_clean(lv_scr_act());
}

/**
 * @brief 定时页面刷新函数
 * 
 * @param ms 
 */
static void page_calmode_upd_cb(uint32_t ms)
{
    uint8_t spid = screen_get_act_spid();
    
    if (SUB_ID_CAL_POWER_CALIB == spid)
    {
        calmode_refresh_power_calibrate();
    }
    else if (SUB_ID_CAL_POWER_CRC == spid)
    {
        calmode_refresh_power_crc();
    }
}

/**
 * @brief 校准模式页面选项.
 * 
 * @param parent 
 */
static void calmode_index_entry(lv_obj_t* parent)
{
    lv_obj_t* title = lv_label_create(lv_scr_act());
    lv_obj_set_style_text_font(title, Font_ResouceGet(FONT_22), 0);
    lv_obj_set_style_text_color(title, lv_color_white(), 0);
    lv_label_set_text(title, "Calibration Mode");
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 10);
    
    view.list.cur_sel = screen_get_act_index();
    view.list.release_cb = calmode_list_event_cb;
    lv_obj_t* list = my_list_create(parent, lv_pct(79), lv_pct(80), &view.list);
    lv_obj_set_flex_align(list, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_align(list, LV_ALIGN_TOP_MID, 0, 38);
    
    my_list_add_btn(list, "Industrial Mode", 10);
    my_list_add_btn(list, "Independent Color Mode", 10);
    my_list_add_btn(list, "Power Calibrate", 10);
    my_list_add_btn(list, "Single Power Calibrate", 10);
    
    lv_group_add_obj(indev_group, list);
    lv_group_focus_obj(list);
    lv_group_set_editing(indev_group, true);

}

/**
 * @brief 创建工厂模式采光界面
 * 
 * @param parent 
 */
static void calmode_industrial_init(lv_obj_t* parent)
{
    static const char* string[] = {
        "Please start lighting"
    };
    
    lv_obj_t* label;
    label = lv_label_create(parent);
    lv_obj_set_style_text_font(label, Font_ResouceGet(FONT_22), 0);
    lv_label_set_text(label, "Industrial Mode");
    lv_obj_align(label, LV_ALIGN_TOP_MID, 0, 0);

    lv_obj_t* label_cont = lv_obj_create(parent);
    lv_obj_add_style(label_cont, &style_common_btn, 0);
    lv_obj_set_style_bg_color(label_cont, lv_color_black(), 0);
    lv_obj_set_style_text_font(label_cont, Font_ResouceGet(FONT_22), 0);
    lv_obj_set_size(label_cont, lv_pct(100), lv_pct(90));
    lv_obj_set_pos(label_cont, 0, 40);

    label = lv_label_create(label_cont);
    lv_label_set_text(label, string[0]);
    lv_obj_set_pos(label, 60,  80);
}

/**
 * @brief 创建单路模式界面
 * 
 * @param parent 
 */
static void calmode_independent_color_ctrl_init(lv_obj_t* parent)
{
    const char* title[] = {"Ch1 :", "Ch2 :" ,"Ch3 :" ,"Ch4 :" , "Ch5 :", "Ch6 :", "Ch7 :" };
    char str[10] = {0};
    
    lv_obj_t* btn_cont = lv_obj_create(parent);
    lv_obj_remove_style_all(btn_cont);  
    lv_obj_set_flex_flow(btn_cont, LV_FLEX_FLOW_COLUMN);
    lv_obj_add_style(btn_cont, &style_rect_panel, 0);
    lv_obj_set_style_text_font(btn_cont, Font_ResouceGet(FONT_10), 0 );
    lv_obj_set_style_bg_color(btn_cont, lv_color_make(0, 0, 0), 0);
    lv_obj_set_style_pad_gap(btn_cont, 6, 0);
    lv_obj_set_size(btn_cont, 160, 200);
    lv_obj_align(btn_cont, LV_ALIGN_CENTER,  0, 0);
    
    for (uint8_t i = 0; i < 7; i++)
    {
        lv_obj_t* obj = adj_btn_create(btn_cont, LIGHT_MODE_COMMON_RADIUS, ADJ_BTN_TYPE_18);
        lv_obj_set_size(obj, lv_pct(100), 23); 
        lv_obj_set_user_data(obj, &p_model->test_data.rgbww[i]);
        lv_obj_t* label = lv_label_create(obj);
        lv_label_set_long_mode(label, LV_LABEL_LONG_CLIP);
        lv_obj_set_style_text_font(label, Font_ResouceGet(FONT_12), 0 );
        lv_obj_set_width(label, lv_pct(50));
        lv_label_set_text(label, title[i]);
        lv_obj_align(label, LV_ALIGN_LEFT_MID, 10, 0);
        lv_obj_t* text = lv_label_create(obj);
        lv_obj_set_width(text, lv_pct(26));
        lv_obj_align(text, LV_ALIGN_RIGHT_MID, -10, 0);
        lv_obj_set_style_text_font(text, Font_ResouceGet(FONT_12), 0 );
        lv_obj_add_event_cb(obj, independent_color_event_cb, LV_EVENT_ALL, NULL);
        lv_group_add_obj(indev_group, obj);
        sprintf( str ,"%d",p_model->test_data.rgbww[i]);
        lv_label_set_text(text , str);
    }
    /*
    data_center_read_config_data(SYS_CONFIG_HIGH_SPEED_MODE, &config_boby.hs_mode);
    high_speed_mode = config_boby.hs_mode;
    */
}


/**
 * @brief 创建功率校准页面
 * 
 * @param parent 
 */
static void calmode_power_powercalibrate_init(lv_obj_t* parent)
{
    lv_obj_t* label;

    label = lv_label_create(parent);
    lv_obj_set_style_text_font(label, Font_ResouceGet(FONT_22), 0);
    lv_label_set_text(label, "Power Calibrate");
    lv_obj_align(label, LV_ALIGN_TOP_MID, 0, 0);

    label = lv_label_create(parent);
    lv_obj_set_size(label, lv_pct(85), LV_SIZE_CONTENT);
    /* 位置不写死 */
//    lv_obj_set_pos(label, 61, 102);
    lv_obj_set_style_text_font(label, Font_ResouceGet(FONT_18), 0);
    lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, 0);
    view.sub_page.power_calib.state_label = label;

    label = lv_label_create(parent);
    lv_obj_set_style_text_font(label, Font_ResouceGet(FONT_22), 0);
    lv_obj_set_style_text_color(label, TEXT_COLOR, 0);
    lv_obj_set_size(label, 100, 25);
    lv_obj_set_pos(label, 111, 142);
    lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, 0);
    view.sub_page.power_calib.power_label = label;
    
    lv_obj_align_to(view.sub_page.power_calib.state_label, label, LV_ALIGN_OUT_TOP_MID, 0, -15);
    
    calmode_refresh_power_calibrate();
}

/**
 * @brief 创建单路功率校准界面
 * 
 * @param parent 
 */
static void calmode_power_crc_init(lv_obj_t* parent)
{
    lv_obj_t* label;

    label = lv_label_create(parent);
    lv_obj_set_style_text_font(label, Font_ResouceGet(FONT_22), 0);
    lv_label_set_text(label, "Single Power Calibrate");
    lv_obj_align(label, LV_ALIGN_TOP_MID, 0, 0);

    label = lv_label_create(parent);
    lv_obj_set_size(label, lv_pct(85), LV_SIZE_CONTENT);
    /* 位置不写死 */
//    lv_obj_set_pos(label, 61, 102);
    lv_obj_set_style_text_font(label, Font_ResouceGet(FONT_18), 0);
    lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, 0);
    view.sub_page.single_power_calib.state_label = label;

    label = lv_label_create(parent);
    lv_obj_set_style_text_font(label, Font_ResouceGet(FONT_22), 0);
    lv_obj_set_style_text_color(label, TEXT_COLOR, 0);
    lv_obj_set_size(label, 100, 25);
    lv_obj_set_pos(label, 111, 142);
    lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, 0);
    view.sub_page.single_power_calib.power_label = label;
    
    lv_obj_align_to(view.sub_page.single_power_calib.state_label, label, LV_ALIGN_OUT_TOP_MID, 0, -15);
    
    calmode_refresh_power_crc();
}

/**
 * @brief 刷新功率校准页面
 * 
 */
static void calmode_refresh_power_calibrate(void)
{
    const char* state_string[] =  {
        "Calibrate Idle",
        "Calibrate PWM_Begin",
        "Calibrate ANALOG_Begin",
        "Calibrating",
        "Pwm Calibrate OK ！Please continue to wait",
        "Analog Calibrate Ok",
        "Calibrate Failed"
    };

    if (p_model->test_data.CalibrateState > CALIBRATE_POWER_FAIL) {
        p_model->test_data.CalibrateState = CALIBRATE_POWER_PWM_START;
    }
    lv_snprintf(sdb, sizeof(sdb), "%s", state_string[p_model->test_data.CalibrateState]);
    lv_label_set_text(view.sub_page.power_calib.state_label, sdb);

    lv_snprintf(sdb, sizeof(sdb), "%dW", p_model->test_data.CalibratePower);
    lv_label_set_text(view.sub_page.power_calib.power_label, sdb);
    lv_obj_align_to(view.sub_page.power_calib.state_label, view.sub_page.power_calib.power_label, LV_ALIGN_OUT_TOP_MID, 0, -15);
}

/**
 * @brief 刷新单路功率校准页面
 * 
 */
static void calmode_refresh_power_crc(void)
{
    const char* state_string[] =  {
        "Calibrate Idle",
        "Calibrate PWM_Begin",
        "Calibrate ANALOG_Begin",
        "Calibrating",
        "Pwm Calibrate OK",
        "Analog Calibrate Ok",
        "Calibrate Failed"
    };

    if (p_model->test_data.powercalibrateState > CALIBRATE_POWER_FAIL) {
        p_model->test_data.powercalibrateState = CALIBRATE_POWER_PWM_START;
    }
    lv_snprintf(sdb, sizeof(sdb), "%s", state_string[p_model->test_data.powercalibrateState]);
    lv_label_set_text(view.sub_page.single_power_calib.state_label, sdb);

    lv_snprintf(sdb, sizeof(sdb), "%dW", p_model->test_data.powercalibratePower);
    lv_label_set_text(view.sub_page.single_power_calib.power_label, sdb);
    lv_obj_align_to(view.sub_page.single_power_calib.state_label, view.sub_page.single_power_calib.power_label, LV_ALIGN_OUT_TOP_MID, 0, -15);
}

/**
 * @brief 单路模式事件回调函数
 * 
 * @param e 
 */
static void independent_color_event_cb(lv_event_t* e)
{
    char str[10] = {0};
    lv_event_code_t event = lv_event_get_code(e);
    lv_obj_t* target = lv_event_get_target(e);
    if (NULL == target)
    {
        return ;
    }
    lv_group_t* group   = lv_obj_get_group(target);
    lv_obj_t *ch_label  = lv_obj_get_child(target, 0);
    lv_obj_t* slider    = lv_obj_get_child(target, 1);
    int16_t* user_data = (int16_t*)lv_obj_get_user_data(target);
    if (NULL == group || NULL == ch_label || NULL == slider || NULL == user_data)
    {
        return ;
    }
    
    if (event == LV_EVENT_KEY)
    {
        uint32_t key = *(uint32_t *)lv_event_get_param(e);

        if (key == LV_KEY_BACKSPACE)
        {
            if (!lv_group_get_editing(group))
            {
                screen_turn_prev_page();
            }
            else
            {
                lv_group_set_editing(group, false);     // 取消选中
                lv_obj_set_style_text_color(ch_label, CHL_DEFAULT_TEXT_COLOR, 0);
            }
        }
        else if (key == LV_ENCODER_KEY_CW)
        {
            if(*user_data > 100 )
            {
                *user_data = *user_data + 50;
            }
            else
            {
                *user_data = *user_data + 1;
            }
            int16_t value = *user_data ;
           
            if (value >= CHL_MAX_INT || value < CHL_MIN_INT)
            {
                value = CHL_MAX_INT;
            }
            *user_data = value;
            sprintf( str ,"%d",*user_data);
            lv_label_set_text(slider,str );
        }
        else if (key == LV_ENCODER_KEY_CC)
        {
            int16_t value = 0;
            
            if( *user_data < 100)
            {
                 value = *user_data - 1;
            }
            else if(*user_data > 50 & *user_data < 100)
            {
                 value = *user_data - 10;
            }
            else if(*user_data > 100 )
            {
                value = *user_data - 50;
            }
            
            if (value <= CHL_MIN_INT ||  value > CHL_MAX_INT)
            {
                value = CHL_MIN_INT;
            }
            *user_data = value;  
            sprintf( str ,"%d",*user_data);
            lv_label_set_text(slider, str);
        }

    }
    else if (event == LV_EVENT_RELEASED)
    {
        if (!lv_group_get_editing(group))
        {
            lv_group_set_editing(group, true);      // 选中
            lv_obj_set_style_text_color(ch_label, CHL_FOCUSED_TEXT_COLOR, 0);
            
        }
        else
        {
            lv_group_set_editing(group, false);     // 取消选中
            lv_obj_set_style_text_color(ch_label, CHL_DEFAULT_TEXT_COLOR, 0);
        }
    }
}

/**
 * @brief 校准界面列表回调函数
 * 
 * @param e 
 * @param index 
 */
static void calmode_list_event_cb(lv_event_t* e, uint8_t index)
{
    lv_event_code_t event = lv_event_get_code(e);

    if (event == LV_EVENT_RELEASED)
    {
        screen_load_sub_page_with_stack(index+ 1, index);
    }
    else if (event == LV_EVENT_KEY)
    {
        uint32_t key = *(uint32_t *)lv_event_get_param(e);
        
        if (LV_KEY_BACKSPACE == key)
        {
            screen_load_page(PAGE_LIGHT_MODE_CCT, 0, false);
        }
    }
    else if (event == LV_EVENT_DELETE)
    {
        if (PAGE_CALIBRATION_MODE != screen_get_act_pid())
        {
            data_center_write_config_data(SYS_CONFIG_FAN_MODE, &fan_mode);  // 退出校准模式，恢复之前的风扇模式
        }
    }
}

/**
 * @brief 返回上一级界面
 * 
 * @param e 
 */
static void calmode_common_event(lv_event_t* e)
{
    lv_event_code_t event = lv_event_get_code(e);
    if (event == LV_EVENT_CLICKED)
    {
        screen_turn_prev_page();
        p_model->test_data.IndustrialMode = INDUSTRIAL_MODE_IDLE;
    }
}

