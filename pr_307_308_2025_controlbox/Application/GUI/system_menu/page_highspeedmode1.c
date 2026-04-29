/*********************
 *      INCLUDES
 *********************/
#include "../ui_common.h"
#include "../page_widget.h"

/*********************
 *      DEFINES
 *********************/
#define DEFAULT_TEXT_COLOR      lv_color_make(255, 255, 255)
#define FOCUSED_BG_COLOR        lv_color_make(255, 255, 255)
#define CHECKED_BG_COLOR        lv_color_make(89,  87,  87) 
#define FOCUSED_LABEL_COLOR     lv_color_make(255, 255, 255)
/*********************
 *      TPEDEFS
 *********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void page_fanmode_construct(void);
static void page_fanmode_destruct(void);
static void page_fanmode_time_upd_cb(uint32_t ms);
static void page_fanmode_event_cb(int event);
static void refresh_fan_img(uint8_t mode);
static void refresh_fanmode_desc(uint8_t mode);

static const char* get_fanmode_str(uint8_t fanmode);
static const char* get_fanmode_desc_str(uint8_t fanmode);
static void btn_cont_event(lv_event_t* event, uint8_t index);
static void set_checked_state(lv_obj_t* btn_panel, uint8_t checked, uint8_t prev_checked);
/*********************
 *  STATIC VATIABLES
 *********************/
static lv_group_t *indev_group;
static sys_config_t* p_sys_menu_model = NULL;
static lv_obj_t* fan_img = NULL;
static lv_obj_t* textarea = NULL;
static my_btnmatrix_ext_t btnmatrix_ext;
/*********************
 *  GLOBAL VATIABLES
 *********************/

/*********************
 *  GLOBAL FUNCTIONS
 *********************/
page_t page_fanmode = PAGE_INITIALIZER("Fan Mode",
                                PAGE_FAN_MODE, 
                                page_fanmode_construct, 
                                page_fanmode_destruct, 
                                page_fanmode_time_upd_cb,
                                page_fanmode_event_cb,                                
                                PAGE_MENU);

/**********************
 *   STATIC FUNCTIONS
 **********************/    
static void page_fanmode_construct(void)
{
    indev_group = lv_group_create();
    p_sys_menu_model = screen_get_act_page_data();
    page_event_send(EVENT_DATA_PULL);

    my_indev_set_group(indev_group);
    lv_group_set_default(indev_group);
    lv_obj_set_style_bg_color(lv_scr_act(), lv_color_black(), 0);

    lv_obj_t* title = lv_label_create(lv_scr_act());
    
    lv_obj_set_style_text_font(title, Font_ResouceGet(FONT_22_BOLD), 0);
    lv_obj_set_style_text_color(title, lv_color_white(), 0);            
    lv_label_set_text(title, Lang_GetStringByID(STRING_ID_FAN_MODE));
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, SYS_MENU_TITLE_TO_TOP);

    lv_obj_t* fanimg_cont = lv_obj_create(lv_scr_act());
    lv_obj_clear_flag(fanimg_cont, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_remove_style_all(fanimg_cont);    
    lv_obj_set_size(fanimg_cont, lv_pct(100), 85);
    lv_obj_align(fanimg_cont, LV_ALIGN_TOP_MID, 0, 36); 

    fan_img = lv_img_create(fanimg_cont);
    refresh_fan_img(p_sys_menu_model->fan_mode);

    lv_obj_t* btn_cont = lv_obj_create(lv_scr_act());
    lv_obj_remove_style_all(btn_cont);
    lv_obj_add_style(btn_cont, &style_rect_panel, 0);
    lv_obj_set_flex_flow(btn_cont, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(btn_cont, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_gap(btn_cont, 10, 0);
    lv_obj_set_size(btn_cont, lv_pct(100), 30);
    lv_obj_align(btn_cont, LV_ALIGN_TOP_MID, 0, 125);

    for (uint8_t i = 0;  i < FAN_MODE_MAX; i++)
    {        
        lv_obj_t* obj = lv_obj_create(btn_cont);
        lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
        lv_obj_add_style(obj, &style_common_btn, 0);
        lv_obj_add_style(obj, &style_common_focued_btn, LV_STATE_FOCUSED);
        lv_obj_set_style_text_color(obj, FOCUSED_LABEL_COLOR, LV_STATE_FOCUSED);
        lv_obj_set_style_bg_color(obj, CHECKED_BG_COLOR, LV_STATE_CHECKED);
        lv_obj_set_style_pad_all(obj, 10, 0);
        lv_obj_set_style_pad_all(obj, 10, LV_STATE_FOCUSED);
        lv_obj_set_style_radius(obj, 10, 0);
        lv_obj_set_style_radius(obj, 10, LV_STATE_FOCUSED);                        
       
        const char* str = get_fanmode_str(i);
        lv_obj_t* label = lv_label_create(obj);           
        uint32_t len = lv_txt_get_width(str, strlen(str), Font_ResouceGet(FONT_18_MEDIUM), 0, LV_TEXT_FLAG_EXPAND);
        lv_obj_set_style_text_font(label, Font_ResouceGet(FONT_18_MEDIUM), 0); 
        lv_label_set_text(label, str);
        lv_obj_center(label);

        if (len < 67) 
        {
            lv_obj_set_size(obj, 67, lv_pct(100));
        }
        else   
        {
            lv_obj_set_size(obj, len + 2, lv_pct(100));
        }
    }

    ui_enter_critical();
    p_sys_menu_model->fan_mode %= FAN_MODE_MAX;
    ui_exit_critical();

    btnmatrix_ext.obj = btn_cont;
    btnmatrix_ext.custom_event_cb = btn_cont_event;
    btnmatrix_ext.max = FAN_MODE_MAX;    
    btnmatrix_ext.sel = p_sys_menu_model->fan_mode;

    lv_obj_set_user_data(btn_cont, &btnmatrix_ext);
    lv_obj_add_event_cb(btn_cont, my_btnmatrix_event_cb, LV_EVENT_ALL, 0);

    textarea = lv_obj_create(lv_scr_act());
    lv_obj_remove_style_all(textarea);
    lv_obj_add_style(textarea, &style_rect_panel, 0);
    lv_obj_set_style_text_color(textarea, lv_color_white(), 0);
    lv_obj_set_style_text_line_space(textarea, 0, 0);
    lv_obj_set_style_text_font(textarea, Font_ResouceGet(FONT_14), 0);
    
    lv_obj_set_size(textarea, lv_pct(100),75);
    lv_obj_align_to(textarea, btn_cont, LV_ALIGN_OUT_BOTTOM_MID, 0, 5);
        
    lv_obj_t* label = lv_label_create(textarea);
    lv_obj_set_size(label, lv_pct(90), LV_SIZE_CONTENT);
    lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_LEFT, 0);
    lv_label_set_long_mode(label, LV_LABEL_LONG_WRAP);    
    
    set_checked_state(btn_cont, p_sys_menu_model->fan_mode, p_sys_menu_model->fan_mode);
    refresh_fanmode_desc(p_sys_menu_model->fan_mode);

    lv_group_add_obj(indev_group, btn_cont);
    lv_group_focus_obj(btn_cont);
    lv_group_set_editing(indev_group, true);
}

static void page_fanmode_destruct(void)
{
    lv_group_focus_freeze(indev_group, true);
    lv_group_remove_all_objs(indev_group);
    lv_group_del(indev_group);
    indev_group = NULL;
    textarea = NULL;

    lv_obj_clean(lv_scr_act());
}

static void page_fanmode_time_upd_cb(uint32_t ms)
{
    page_invalid_type_t type = page_get_invalid_type();
    bool force_upd = false;
    
    page_event_send(EVENT_DATA_WRITE);
    
    switch(type)
    {
        case PAGE_INVALID_TYPE_IDLE:    return;
        case PAGE_INVALID_TYPE_PORTION:
        case PAGE_INVALID_TYPE_ALL:
        {
            page_set_invalid_type(PAGE_INVALID_TYPE_IDLE);
            force_upd = type == PAGE_INVALID_TYPE_ALL ? true : false;
        }
        break;
    }

    if ((p_sys_menu_model->fan_mode != btnmatrix_ext.sel && btnmatrix_ext.obj != NULL) || force_upd)
    {            
        lv_event_send(btnmatrix_ext.obj, LV_EVENT_DEFOCUSED, NULL);
        btnmatrix_ext.sel = p_sys_menu_model->fan_mode;        

        lv_event_send(btnmatrix_ext.obj, LV_EVENT_FOCUSED, NULL);      
        refresh_fan_img(btnmatrix_ext.sel);
        refresh_fanmode_desc(btnmatrix_ext.sel);
    }
}

static void page_fanmode_event_cb(int event)
{
    uint8_t fanmode = 0;
    data_center_read_config_data(SYS_CONFIG_FAN_MODE, &fanmode);
    switch (event)
    {
        case EVENT_DATA_PULL:
            p_sys_menu_model->fan_mode = fanmode;
            break;    
        case EVENT_DATA_WRITE:
            if (p_sys_menu_model->fan_mode != fanmode)
            {
                data_center_write_config_data(SYS_CONFIG_FAN_MODE, &p_sys_menu_model->fan_mode);
            }
            break;
    }
}

static void refresh_fan_img(uint8_t mode)
{    
    lv_img_set_src(fan_img,  ui_get_fanmode_img(mode));
    lv_obj_center(fan_img);  
}

static void refresh_fanmode_desc(uint8_t mode)
{
    const char* text = get_fanmode_desc_str(mode);
    lv_obj_t* label = lv_obj_get_child(textarea, 0);
    const lv_font_t* font = Font_ResouceGet(FONT_14);

    if ((p_sys_menu_model->lang == LANG_TYPE_EN) \
            && (mode == FAN_MODE_MEDIUM))
    {
        font = Font_ResouceGet(FONT_12);
    }

    lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_set_style_text_font(label, font, 0);    
    lv_label_set_text(label, text);
    lv_obj_center(label);
}

static void set_checked_state(lv_obj_t* btn_panel, uint8_t checked, uint8_t prev_checked)
{
    lv_obj_t* target = lv_obj_get_child(btn_panel, checked);
    lv_obj_t* prev_target = lv_obj_get_child(btn_panel, prev_checked);

    lv_obj_clear_state(prev_target, LV_STATE_CHECKED);
    lv_obj_add_state(target, LV_STATE_CHECKED);
    lv_obj_invalidate(prev_target);
    lv_obj_invalidate(target);
}

static const char* get_fanmode_str(uint8_t fanmode)
{
    const char* str = NULL;
    switch(fanmode)
    {
        case FAN_MODE_SMART:     str = Lang_GetStringByID(STRING_ID_SMART); break;
        case FAN_MODE_HIGH:     str = Lang_GetStringByID(STRING_ID_HIGH); break;
        case FAN_MODE_MEDIUM:     str = Lang_GetStringByID(STRING_ID_MEDIUM); break;
        case FAN_MODE_SILENT:     str = Lang_GetStringByID(STRING_ID_SILENT); break;
    }
    return str;
}

static const char* get_fanmode_desc_str(uint8_t fanmode)
{
    const char* str = NULL;
    switch(fanmode)
    {
        case FAN_MODE_SMART:      str = Lang_GetStringByID(STRING_ID_SMART_INFO); break;
        case FAN_MODE_HIGH:       str = Lang_GetStringByID(STRING_ID_HIGH_INFO); break;
        case FAN_MODE_MEDIUM:     str =  Lang_GetStringByID(STRING_ID_MEDIUM_INFO); break;
        case FAN_MODE_SILENT:     str = Lang_GetStringByID(STRING_ID_SILENT_INFO); break;
    }
    return str;

}

static void btn_cont_event(lv_event_t* e, uint8_t index)
{
    lv_event_code_t event = lv_event_get_code(e);
    lv_obj_t* target = lv_event_get_target(e);
    shortcut_keys_event_handler(e);

    if (event == LV_EVENT_RELEASED)
    {
        uint8_t checked = index;
        uint8_t prev_checked = p_sys_menu_model->fan_mode;                
        set_checked_state(target, checked, prev_checked); 
        p_sys_menu_model->fan_mode = index;       
    }
    else if (event == LV_EVENT_KEY)
    {
        uint32_t key = *(uint32_t*)lv_event_get_param(e);
        
        if (key == LV_KEY_LEFT || key == LV_KEY_RIGHT)
        {
            refresh_fan_img(index);
            refresh_fanmode_desc(index);
        }
        else if (key == LV_KEY_BACKSPACE)
        {
            screen_turn_prev_page();
        }
    }
}
