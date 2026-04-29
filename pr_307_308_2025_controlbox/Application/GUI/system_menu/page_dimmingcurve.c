/*********************
 *      INCLUDES
 *********************/
#include "../ui_common.h"
#include "../page_widget.h"
/*********************
 *      DEFINES
 *********************/
#define CHECKED_BG_COLOR        lv_color_make(89,  87,  87)
#define CHECKED_IMG_COLOR       lv_color_make(179, 179, 179)
#define FOCUSED_BG_COLOR        RED_THEME_COLOR
#define FOCUSED_IMG_COLOR       lv_color_make(255, 255, 255)
#define DEFAULT_IMG_COLOR       lv_color_make(179, 179, 179)
#define DEFAULT_LABEL_COLOR     lv_color_make(179, 179, 179)
#define FOCUSED_LABEL_COLOR     lv_color_make(255, 255, 255)
/*********************
 *      TPEDEFS
 *********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void page_dimmingcurve_construct(void);
static void page_dimmingcurve_destruct(void);
static void page_dimmingcurve_time_upd_cb(uint32_t ms);
static void page_dimmingcurve_event_cb(int event);

static void refre_imgbtn_panel(lv_obj_t* panel);
static void my_imgbtn_init(lv_obj_t* imgbtn_cont, uint8_t curve_type);
static const char* get_dimmingcurve_str(uint8_t fanmode);
static void my_imgbtn_event_cb(lv_event_t* e);
static void set_checked_state(lv_obj_t* btn_panel, uint8_t checked, uint8_t prev_checked);
/*********************
 *  STATIC VATIABLES
 *********************/
static lv_group_t *indev_group;
static sys_config_t* p_sys_menu_model = NULL;
/*********************
 *  GLOBAL VATIABLES
 *********************/

/*********************
 *  GLOBAL FUNCTIONS
 *********************/
page_t page_dimmingcurve = PAGE_INITIALIZER("Dimming Curve",
                                PAGE_DIMMING_CURVE, 
                                page_dimmingcurve_construct, 
                                page_dimmingcurve_destruct, 
                                page_dimmingcurve_time_upd_cb,
                                page_dimmingcurve_event_cb,                                
                                PAGE_MENU);

/**********************
 *   STATIC FUNCTIONS
 **********************/    
static void page_dimmingcurve_construct(void)
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
    lv_label_set_text(title, Lang_GetStringByID(STRING_ID_DIMMING_CURVE));
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, SYS_MENU_TITLE_TO_TOP);

    lv_obj_t* imgbtn_panel = lv_obj_create(lv_scr_act());
    lv_obj_add_style(imgbtn_panel, &style_rect_panel, 0);
    lv_obj_set_style_bg_color(imgbtn_panel, lv_color_black(), 0);
    lv_obj_set_style_pad_gap(imgbtn_panel, 0, 0);
    lv_obj_set_size(imgbtn_panel, lv_pct(77), lv_pct(77));
    lv_obj_clear_flag(imgbtn_panel, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t* focus_obj = NULL;

    for (uint8_t i = 0; i < CURVE_TYPE_MAX; i++)
    {        
        lv_obj_t* obj = lv_obj_create(imgbtn_panel);        
        lv_obj_remove_style_all(obj);
        lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
        lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
        lv_obj_set_style_text_font(obj, Font_ResouceGet(FONT_18_MEDIUM), 0);
        lv_obj_set_size(obj, lv_pct(50), lv_pct(48));        

        my_imgbtn_init(obj, i);
        lv_obj_add_event_cb(obj, my_imgbtn_event_cb, LV_EVENT_ALL, 0);
        lv_event_send(obj, LV_EVENT_DEFOCUSED, NULL);
        
        if (p_sys_menu_model->curve_type == i)
            focus_obj = obj;
    }


    set_checked_state(imgbtn_panel, p_sys_menu_model->curve_type , p_sys_menu_model->curve_type);
    refre_imgbtn_panel(imgbtn_panel);
    lv_obj_align(imgbtn_panel, LV_ALIGN_TOP_MID, 0, 53);

#if 1
    lv_obj_t* btn1 = lv_obj_get_child(imgbtn_panel, CURVE_TYPE_LINEAR);
    lv_obj_t* btn2 = lv_obj_get_child(imgbtn_panel, CURVE_TYPE_S_SHAPE);
    lv_obj_t* btn3 = lv_obj_get_child(imgbtn_panel, CURVE_TYPE_LOG);
    lv_obj_t* btn4 = lv_obj_get_child(imgbtn_panel, CURVE_TYPE_EXP);    
#else
    lv_obj_t* btn1 = lv_obj_get_child(imgbtn_panel, CURVE_TYPE_EXP);
    lv_obj_t* btn2 = lv_obj_get_child(imgbtn_panel, CURVE_TYPE_LOG);
    lv_obj_t* btn3 = lv_obj_get_child(imgbtn_panel, CURVE_TYPE_LINEAR);
    lv_obj_t* btn4 = lv_obj_get_child(imgbtn_panel, CURVE_TYPE_S_SHAPE);    
#endif
    lv_group_add_obj(indev_group, btn1);
    lv_group_add_obj(indev_group, btn2);
    lv_group_add_obj(indev_group, btn3);
    lv_group_add_obj(indev_group, btn4);

    lv_group_focus_obj(focus_obj);
}

static void page_dimmingcurve_destruct(void)
{
    lv_group_del(indev_group);   
    lv_obj_clean(lv_scr_act());

    indev_group = NULL;
}

static void page_dimmingcurve_time_upd_cb(uint32_t ms)
{
    page_invalid_type_t type = page_get_invalid_type();
    bool force_upd = false;
    uint8_t curve = p_sys_menu_model->curve_type;

    (void)force_upd;
    data_center_read_config_data(SYS_CONFIG_CURVE_TYPE, &curve);

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

    if (curve != p_sys_menu_model->curve_type)
    {
        p_sys_menu_model->curve_type = curve;        
        lv_group_user_focus_obj_by_id(indev_group, p_sys_menu_model->curve_type);  
    }
}

static void page_dimmingcurve_event_cb(int event)
{
    uint8_t curve_type = 0;    
    data_center_read_config_data(SYS_CONFIG_CURVE_TYPE, &curve_type);    
    switch(event)
    {
        case EVENT_DATA_PULL:
            p_sys_menu_model->curve_type = curve_type;            
            break;
        case EVENT_DATA_WRITE:            
            if (curve_type != p_sys_menu_model->curve_type)
            {
                data_center_write_config_data(SYS_CONFIG_CURVE_TYPE, &p_sys_menu_model->curve_type);
            }            
            break;
    }
}

static void refre_imgbtn_panel(lv_obj_t* panel)
{
    lv_obj_t* btn1 = lv_obj_get_child(panel, CURVE_TYPE_LINEAR);
    lv_obj_t* btn2 = lv_obj_get_child(panel, CURVE_TYPE_S_SHAPE);
    lv_obj_t* btn3 = lv_obj_get_child(panel, CURVE_TYPE_LOG);
    lv_obj_t* btn4 = lv_obj_get_child(panel, CURVE_TYPE_EXP);
    
    #if 0
    lv_obj_align(btn4, LV_ALIGN_TOP_LEFT, 0, 0);
    lv_obj_align_to(btn3, btn4, LV_ALIGN_OUT_RIGHT_MID, 0, 0);
    lv_obj_align_to(btn1, btn3, LV_ALIGN_OUT_BOTTOM_MID, 0, 2);
    lv_obj_align_to(btn2, btn4, LV_ALIGN_OUT_BOTTOM_MID, 0, 2);
    #else 
    lv_obj_align(btn1, LV_ALIGN_TOP_LEFT, 0, 0);
    lv_obj_align_to(btn2, btn1, LV_ALIGN_OUT_RIGHT_MID, 0, 0);
    lv_obj_align_to(btn3, btn2, LV_ALIGN_OUT_BOTTOM_MID, 0, 2);
    lv_obj_align_to(btn4, btn1, LV_ALIGN_OUT_BOTTOM_MID, 0, 2);
    #endif
}

static void my_imgbtn_init(lv_obj_t* imgbtn_cont, uint8_t curve_type)
{
    lv_obj_t * imgbtn = lv_obj_create(imgbtn_cont);
    lv_obj_add_style(imgbtn, &style_rect_panel, 0);
    lv_obj_set_style_radius(imgbtn, 10, 0);

    lv_obj_set_style_bg_color(imgbtn, FOCUSED_BG_COLOR, LV_STATE_FOCUSED);
    lv_obj_set_style_bg_color(imgbtn, CHECKED_BG_COLOR, LV_STATE_CHECKED);
    lv_obj_set_size(imgbtn, lv_pct(85), lv_pct(74));
    lv_obj_align(imgbtn, LV_ALIGN_TOP_MID, 0, 0);

    lv_obj_t* img = lv_img_create(imgbtn);
    lv_img_set_src(img, title_get_curve_img(curve_type));
    lv_obj_set_style_img_recolor_opa(img, LV_OPA_100, 0);
    lv_obj_set_style_img_recolor_opa(img, LV_OPA_100, LV_STATE_CHECKED);
    lv_obj_set_style_img_recolor_opa(img, LV_OPA_100, LV_STATE_FOCUSED);    
    lv_obj_set_style_img_recolor(img, DEFAULT_IMG_COLOR, 0);
    lv_obj_set_style_img_recolor(img, FOCUSED_IMG_COLOR, LV_STATE_FOCUSED);    
    lv_obj_set_style_img_recolor(img, CHECKED_IMG_COLOR, LV_STATE_CHECKED);
    
    lv_obj_center(img);
    
    lv_obj_t* label = lv_label_create(imgbtn_cont);    
    lv_obj_set_style_text_color(label, FOCUSED_LABEL_COLOR, LV_STATE_FOCUSED);
    lv_obj_set_style_text_color(label, DEFAULT_LABEL_COLOR, 0);
    lv_label_set_text(label, get_dimmingcurve_str(curve_type));    
    lv_obj_align(label, LV_ALIGN_BOTTOM_MID, 0, 0);
}

static const char* get_dimmingcurve_str(uint8_t fanmode)
{
    const char* str = NULL;
    switch(fanmode)
    {
        case CURVE_TYPE_LINEAR:     str = Lang_GetStringByID(STRING_ID_CURVE_LINEAR); break;
        case CURVE_TYPE_EXP:        str = Lang_GetStringByID(STRING_ID_CURVE_EXP); break;
        case CURVE_TYPE_S_SHAPE:    str = Lang_GetStringByID(STRING_ID_CURVE_S_CURVE); break;
        case CURVE_TYPE_LOG:        str = Lang_GetStringByID(STRING_ID_CURVE_LOG); break;
    }
    return str;
}

static void set_checked_state(lv_obj_t* btn_panel, uint8_t checked, uint8_t prev_checked)
{
    lv_obj_t* target = lv_obj_get_child(btn_panel, checked);        
    lv_obj_t* prev_target = lv_obj_get_child(btn_panel, prev_checked);

    lv_obj_t* img_btn = lv_obj_get_child(prev_target, 0);
    lv_obj_t* img = lv_obj_get_child(img_btn, 0);

    lv_obj_clear_state(img_btn, LV_STATE_CHECKED);
    lv_obj_clear_state(img, LV_STATE_CHECKED);
    lv_obj_invalidate(prev_target);

    img_btn = lv_obj_get_child(target, 0);
    img = lv_obj_get_child(img_btn, 0);

    lv_obj_add_state(img_btn, LV_STATE_CHECKED);
    lv_obj_add_state(img, LV_STATE_CHECKED); 
    lv_obj_invalidate(target);
}

static void my_imgbtn_event_cb(lv_event_t* e)
{
    lv_obj_t* imgbtn_cont = lv_event_get_target(e);
    lv_event_code_t event = lv_event_get_code(e);

    shortcut_keys_event_handler(e);

    if (event == LV_EVENT_DEFOCUSED)
    {
        uint8_t child_sum = lv_obj_get_child_cnt(imgbtn_cont);
        for (uint8_t cnt = 0; cnt < child_sum; cnt++)
        {
            lv_obj_t* child = lv_obj_get_child(imgbtn_cont, cnt);
            lv_obj_clear_state(child, LV_STATE_FOCUSED);
        }   

        lv_obj_t* imgbtn = lv_obj_get_child(imgbtn_cont, 0);
        lv_obj_t* img = lv_obj_get_child(imgbtn, 0);

        lv_obj_clear_state(img, LV_STATE_FOCUSED);
        lv_obj_invalidate(img);      
    }
    else if (event == LV_EVENT_FOCUSED)
    {
        uint8_t child_sum = lv_obj_get_child_cnt(imgbtn_cont);
        for (uint8_t cnt = 0; cnt < child_sum; cnt++)
        {
            lv_obj_t* child = lv_obj_get_child(imgbtn_cont, cnt);
            lv_obj_add_state(child, LV_STATE_FOCUSED);
        }

        lv_obj_t* imgbtn = lv_obj_get_child(imgbtn_cont, 0);
        lv_obj_t* img = lv_obj_get_child(imgbtn, 0);

        lv_obj_add_state(img, LV_STATE_FOCUSED);
        lv_obj_invalidate(imgbtn);
    }
    else if (event == LV_EVENT_RELEASED)
    {
        uint8_t checked = lv_obj_get_child_id(imgbtn_cont);
        uint8_t prev_checked = p_sys_menu_model->curve_type;
        
        //修改被选中对象的状态
        set_checked_state(lv_obj_get_parent(imgbtn_cont), checked, prev_checked);
        
        //更新调光曲线值
        p_sys_menu_model->curve_type = lv_obj_get_child_id(imgbtn_cont);
        data_center_write_config_data(SYS_CONFIG_CURVE_TYPE, &p_sys_menu_model->curve_type);
		if(ui_get_master_mode())
		{
			extern void gui_data_sync_event_gen(void);		
			gui_data_sync_event_gen();
		}
    }
    else if (event == LV_EVENT_KEY)  
    {
        uint32_t key = *(uint32_t *)lv_event_get_param(e);

        if (key == LV_KEY_BACKSPACE)
        {
            screen_turn_prev_page();
        }
    }        
}
