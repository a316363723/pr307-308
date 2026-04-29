/*********************
 *      INCLUDES
 *********************/
#include "ui_common.h"
#include "page_widget.h"

/*********************
 *      DEFINES
 *********************/
#define INTENSITY_CONT_HEIGHT       32 
#define GEL_TABLE_CONT_HEIGHT       152
#define BTN_DIVIDER_LINE_COLOR      lv_color_make(255, 0, 0)

/**********************
 *      TYPEDEFS
 **********************/
enum {
    BTN_ID_1 = 0,
    BTN_ID_2,
    BTN_ID_MAX,
};

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void int_upd_run_cb(void* data);
static bool int_assign_value_cb(void* data);
static void cct_btn_ralease_cb(gel_switch_btn_t* btn);
static void brand_btn_release_cb(gel_switch_btn_t* btn);

static void gel_cont_ceate(lv_obj_t* cont);
static void gel_cont_refresh(lv_obj_t* cont, const gel_dsc_t* p_gel_dsc);

static void serials_cont_event_cb(lv_event_t* e);
static void cont_table_event_cb(lv_event_t* e);

static void page_gel_construct(void);
static void page_gel_destruct(void);
static void page_gel_time_upd_cb(uint32_t ms);
static void page_gel_event_cb(int event);

static inline uint8_t get_container_index(const custom_cont_t* p_table_dsc)
{
    return p_table_dsc->cur_index % p_table_dsc->max_disp_nums;
}

/**********************
 *  STATIC VARIABLES
 **********************/
static adj_btn_t int_btn = ADJ_BTN_INITIALIZER(0, 1000, 1, 500, "I NT", "%", 3, 33, -3, false, false,  intensity_get_value_str, int_upd_run_cb, int_assign_value_cb, NULL, common_intensity_get_step);

static lv_group_t *indev_group;
static lv_obj_t* intensity_bar;
static struct db_gel* p_gel_model;

static gel_switch_btn_t cct_switch_btn;
static gel_switch_btn_t brand_switch_btn;
static lv_obj_t* serials_cont;
static lv_obj_t* gel_table_cont;
static custom_cont_t serial_cont_dsc;
static custom_cont_t gel_table_dsc;

/*********************
 *  GLOBAL VATIABLES
 *********************/
page_t page_gel = PAGE_INITIALIZER("GEL",
                                PAGE_LIGHT_MODE_GEL, 
                                page_gel_construct, 
                                page_gel_destruct, 
                                page_gel_time_upd_cb,
                                page_gel_event_cb,                                
                                PAGE_NONE);

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void page_gel_init(lv_obj_t* parent)
{
    p_gel_model = (struct db_gel*)screen_get_act_page_data();    
    indev_group = lv_group_create();  

    page_event_send(EVENT_DATA_PULL);    
    my_indev_set_group(indev_group);
    lv_group_set_default(indev_group);

    lv_obj_set_style_bg_color(parent, lv_color_black(), 0);
     
    lv_obj_t* obj_panel = lv_obj_create(parent);
    lv_obj_remove_style_all(obj_panel);
    lv_obj_set_size(obj_panel, lv_pct(100), lv_pct(95));
    lv_obj_align(obj_panel, LV_ALIGN_CENTER, 0, 0);

    lv_obj_t* bar_cont = lv_obj_create(obj_panel);
    lv_obj_clear_flag(bar_cont, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_style(bar_cont, &style_rect_panel, 0);
    lv_obj_set_style_text_color(bar_cont, lv_color_make(255, 255, 255), 0);
    lv_obj_set_style_text_font(bar_cont, Font_ResouceGet(FONT_10), 0);
    lv_obj_set_size(bar_cont, 208, INTENSITY_CONT_HEIGHT);
    lv_obj_align(bar_cont, LV_ALIGN_TOP_LEFT, 10, 0);

    intensity_bar = intensity_bar_create(bar_cont, 123, 13, 5, 0);    
	if(gui_get_hs_mode())
		p_gel_model->lightness = p_gel_model->lightness < 200 ? 200 : p_gel_model->lightness;
    lv_obj_t* int_btn_cont = adj_btn_create(obj_panel, LIGHT_MODE_COMMON_RADIUS, ADJ_BTN_TYPE_18);
    lv_obj_set_size(int_btn_cont, 88, INTENSITY_CONT_HEIGHT);
    lv_obj_set_style_text_font(int_btn_cont, Font_ResouceGet(FONT_14), 0);    
    lv_obj_set_style_text_font(int_btn_cont, Font_ResouceGet(FONT_14_BOLD), LV_STATE_EDITED);    

    adj_btn_init(int_btn_cont, &int_btn, 20);
    lv_obj_set_user_data(int_btn_cont, (void *)&int_btn);
    lv_obj_add_event_cb(int_btn_cont, adj_btn_event_cb, LV_EVENT_ALL, NULL);
    lv_obj_align_to(int_btn_cont, bar_cont, LV_ALIGN_OUT_RIGHT_MID, 8, 0);    
    lv_group_add_obj(indev_group, int_btn_cont);

    gel_table_cont = lv_obj_create(obj_panel);
    lv_obj_add_style(gel_table_cont, &style_rect_panel, 0);
    lv_obj_set_size(gel_table_cont, 208, GEL_TABLE_CONT_HEIGHT);
    lv_obj_align_to(gel_table_cont, bar_cont, LV_ALIGN_OUT_BOTTOM_MID, 0, 5);
    lv_obj_set_flex_flow(gel_table_cont, LV_FLEX_FLOW_ROW_WRAP);
    lv_obj_set_style_pad_gap(gel_table_cont, 0, 0);    
    lv_obj_set_user_data(gel_table_cont, &gel_table_dsc);

    gel_cont_table_init(gel_table_cont, &(p_gel_model->gel), 76);    
    gel_cont_table_fliping_refresh(gel_table_cont, &(p_gel_model->gel), gel_model_get_index(&p_gel_model->gel));
    lv_obj_add_event_cb(gel_table_cont, cont_table_event_cb, LV_EVENT_ALL, NULL);
    
    cct_switch_btn.btn_id = 0;
    cct_switch_btn.sel_index = p_gel_model->gel.cct == 3200 ? 0 : 1;
    cct_switch_btn.ralease_cb = cct_btn_ralease_cb;
    cct_switch_btn.btn = gel_switch_btn_create(obj_panel, "3200K", "5600K", 88, 20);
    lv_obj_clear_flag(cct_switch_btn.btn, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_align_to(cct_switch_btn.btn, gel_table_cont, LV_ALIGN_OUT_RIGHT_TOP, 9, 0);
    lv_obj_set_user_data(cct_switch_btn.btn, &cct_switch_btn);
    lv_obj_add_event_cb(cct_switch_btn.btn, gel_switch_btn_event, LV_EVENT_ALL, NULL);
    lv_group_add_obj(indev_group, cct_switch_btn.btn);
    lv_event_send(cct_switch_btn.btn, LV_EVENT_DEFOCUSED, NULL);

    brand_switch_btn.btn_id = 1;
    brand_switch_btn.sel_index = p_gel_model->gel.brand;
    brand_switch_btn.ralease_cb = brand_btn_release_cb;
    brand_switch_btn.btn = gel_switch_btn_create(obj_panel, "R", "L", 88, 20);
    lv_obj_clear_flag(brand_switch_btn.btn, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_align_to(brand_switch_btn.btn, cct_switch_btn.btn, LV_ALIGN_OUT_BOTTOM_MID, 0, 3);
    lv_obj_set_user_data(brand_switch_btn.btn, &brand_switch_btn);
    lv_obj_add_event_cb(brand_switch_btn.btn, gel_switch_btn_event, LV_EVENT_ALL, NULL);
    lv_group_add_obj(indev_group, brand_switch_btn.btn);
    lv_event_send(brand_switch_btn.btn, LV_EVENT_DEFOCUSED, NULL);
    
    serials_cont = lv_obj_create(obj_panel);
    serial_cont_dsc.obj = serials_cont;    
    lv_obj_add_style(serials_cont, &style_rect_panel, 0);
    lv_obj_set_scroll_dir(serials_cont, LV_DIR_NONE);
    lv_obj_set_scrollbar_mode(serials_cont, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_size(serials_cont, 88, 104);
    lv_obj_set_flex_flow(serials_cont, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(serials_cont, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER,LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_gap(serials_cont, 2, 0);
    lv_obj_set_style_outline_width(serials_cont, 1, LV_STATE_FOCUSED);
    lv_obj_set_style_outline_color(serials_cont, lv_color_white(), LV_STATE_FOCUSED);
    lv_obj_align_to(serials_cont, brand_switch_btn.btn, LV_ALIGN_OUT_BOTTOM_MID, 0, 5);
    lv_obj_set_user_data(serials_cont, &serial_cont_dsc);
    serials_cont_init(serials_cont, &(p_gel_model->gel));

    lv_obj_add_event_cb(serials_cont, serials_cont_event_cb, LV_EVENT_ALL, NULL);
    lv_event_send(serials_cont, LV_EVENT_DEFOCUSED, NULL);
    lv_group_add_obj(indev_group, serials_cont);
    lv_group_set_wrap(indev_group, false);
    
    adj_btn_refresh_all(&int_btn, sizeof(int_btn)/ sizeof(int_btn), true);
    page_event_send(EVENT_DATA_COMPULSION);
    uint8_t id = PAGE_LIGHT_MODE_GEL;
    uint8_t sub_id = 0;
    data_center_write_config_data_no_event(SYS_CONFIG_LOCAL_UI, &id);
    data_center_write_config_data_no_event(SYS_CONFIG_LOCAL_SUB_UI, &sub_id);
	data_center_write_config_data_no_event(SYS_CONFIG_EFFECTS_MODE, &id);
	
	struct sys_info_dmx          dmx_link_state;
	
	data_center_read_sys_info(SYS_INFO_DMX, &dmx_link_state);
	if(dmx_link_state.eth_linked != 1 && dmx_link_state.wired_linked != 1 && dmx_link_state.wireless_linked != 1)
	{
		gui_lfm_seng_data_set_flag(0);
		gui_data_sync_event_gen();
	}
}

/**
 * @brief 色纸系列容器初始化
 * 
 * @param cont    显示容器
 * @param p_model GEL数据模型
 */
void serials_cont_init(lv_obj_t* cont, struct gel_param* p_model)
{
    gel_style_t *p_gel_style = gel_style_get();
    uint8_t brand = gel_model_get_brand(p_model);
    uint8_t sums = brand == ROSCO ? ROSCO_SUMS : LEE_SUMS;
    uint8_t checked_index = gel_model_get_series(p_model);

    lv_obj_t* obj;
    lv_obj_t* label;
    custom_cont_t *p_cont_dsc = (custom_cont_t*)lv_obj_get_user_data(cont);    
    p_cont_dsc->cur_index = checked_index;
    p_cont_dsc->min = 0;
    p_cont_dsc->max = sums - 1;
    p_cont_dsc->state = 0;

    if (ROSCO == brand) 
        lv_obj_set_style_pad_gap(p_cont_dsc->obj, 4, 0);
    else
        lv_obj_set_style_pad_gap(p_cont_dsc->obj, 2, 0);

    for (uint8_t i = 0; i < sums; i++)
    {
        
        obj = lv_obj_create(cont);
        lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
        lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);       
        lv_obj_add_style(obj, &p_gel_style->serials_btn_style, 0);  
        lv_obj_add_style(obj, &p_gel_style->btn_focsed_style, LV_STATE_FOCUSED);
        lv_obj_add_style(obj, &p_gel_style->btn_checked_style, LV_STATE_CHECKED);
        lv_obj_set_style_text_color(obj, lv_color_white(), LV_STATE_USER_1 | LV_STATE_FOCUSED);        
        lv_obj_set_style_text_font(obj, Font_ResouceGet(FONT_12), 0);
        lv_obj_set_style_border_width(obj, 0, 0);
        lv_obj_set_style_radius(obj, 3, 0);
        lv_obj_set_size(obj, lv_pct(90), 18);

        label = lv_label_create(obj);
        const char* serials_name = gel_get_serial_name(brand, i);
        lv_label_set_text(label, serials_name);
        lv_obj_center(label);
        if (i == checked_index)
            lv_obj_add_state(obj, LV_STATE_CHECKED);
    }
}

/**
 * @brief 色纸显示容器事件回调函数
 * 
 * @param e 
 */
void gel_cont_event_cb(lv_event_t* e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t* cont = lv_event_get_target(e);

    if (code == LV_EVENT_FOCUSED)
    {
        int cnt = lv_obj_get_child_cnt(cont);

        if (lv_obj_get_state(cont) & LV_STATE_USER_1)
        {
            lv_obj_clear_state(cont, LV_STATE_USER_1);
        }

        for (int i = 0; i < cnt; i++)
        {
            lv_obj_t* child = lv_obj_get_child(cont, i);
            lv_obj_add_state(child, LV_STATE_FOCUSED);
#if 0           
            if (i == 0)
            {
                lv_color_t color = lv_obj_get_style_bg_color(child,0);
                uint32_t grayLevel = (uint32_t)(color.ch.red * 0.299 +  color.ch.green * 0.587 + color.ch.blue * 0.114);                
                printf("r: %d, g: %d  b:%d  graylevel : %d\r\n", color.ch.red, color.ch.green, color.ch.blue, grayLevel);
            }
#endif
        }        
        
        //Get color label 
        lv_obj_t* label = lv_obj_get_child(cont, -1);
        lv_label_set_long_mode(label,  LV_LABEL_LONG_SCROLL);
    }
    else if (code == LV_EVENT_DEFOCUSED)
    {
        int cnt = lv_obj_get_child_cnt(cont);
        for (int i = 0; i < cnt; i++)
        {
            lv_obj_t* child = lv_obj_get_child(cont, i);
            lv_obj_clear_state(child, LV_STATE_FOCUSED);
        }        
        
        lv_obj_t* label = lv_obj_get_child(cont, -1);
        lv_label_set_long_mode(label,  LV_LABEL_LONG_CLIP);
    }
}

/**
 * @brief 色纸显示容器初始化
 * 
 * @param cont 
 */
 void gel_cont_ceate(lv_obj_t* cont)
{
    gel_style_t* p_gel_style = gel_style_get();
    
    lv_obj_t* color_cont = lv_obj_create(cont);
    lv_obj_clear_flag(color_cont, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_style(color_cont, &p_gel_style->color_cont_style, 0);
    lv_obj_add_style(color_cont, &p_gel_style->color_cont_focus_style, LV_STATE_FOCUSED);
    lv_obj_set_style_width(color_cont, lv_pct(89), 0);
    lv_obj_set_style_height(color_cont, lv_pct(56), 0);
    lv_obj_set_style_width(color_cont, lv_pct(95), LV_STATE_FOCUSED);
    lv_obj_set_style_height(color_cont, lv_pct(70), LV_STATE_FOCUSED);
    lv_obj_align(color_cont, LV_ALIGN_CENTER, 0, -10);
        
    lv_obj_t* color_label = lv_label_create(color_cont);    
    lv_obj_set_style_text_font(color_label, Font_ResouceGet(FONT_10), 0);      
    lv_obj_set_style_text_align(color_label, LV_TEXT_ALIGN_LEFT, 0);
    lv_obj_align(color_label, LV_ALIGN_BOTTOM_LEFT, 0, 0);
    
    lv_obj_t* label = lv_label_create(cont);    
    lv_obj_set_width(label , lv_pct(100));
    lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, LV_STATE_FOCUSED);
    lv_obj_align(label, LV_ALIGN_BOTTOM_MID, 0, 0);
}

/**
 * @brief 色纸颜色表初始化
 * 
 * @param cont 显示容器
 * @param p_gel_model GEL数据模型
 * @param cont_height 容器高度
 */
void gel_cont_table_init(lv_obj_t* cont, struct gel_param* p_gel_model, lv_coord_t cont_height)
{
    const gel_dsc_t* p_gel_dsc_index = gel_get_serial_dsc(gel_model_get_brand(p_gel_model), gel_model_get_series(p_gel_model));
    uint8_t max_nums = gel_get_max_num(gel_model_get_brand(p_gel_model), gel_model_get_series(p_gel_model));
    custom_cont_t* p_table_dsc = lv_obj_get_user_data(cont);

    p_table_dsc->max_disp_nums = lv_obj_get_style_height(cont, 0) / cont_height * 3;
    p_table_dsc->max = max_nums - 1;
    p_table_dsc->min = 0;
    p_table_dsc->cur_index = gel_model_get_index(p_gel_model);

    for (uint8_t cnt = 0; cnt < p_table_dsc->max_disp_nums; cnt++)
    {                
        lv_obj_t* obj = lv_obj_create(cont);
        lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
        lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);  
        lv_obj_set_size(obj, lv_pct(33), cont_height);
        lv_obj_add_style(obj, &style_rect_panel, 0);
        // lv_obj_set_style_text_color(obj, lv_color_white(), 0);
        // lv_obj_set_style_text_color(obj, COMMON_EDITED_COLOR, LV_STATE_FOCUSED);        
        // lv_obj_set_style_text_color(obj, COMMON_EDITED_COLOR, LV_STATE_USER_1);      
        lv_obj_set_style_text_color(obj, lv_color_make(160, 160, 160), 0);
        lv_obj_set_style_text_color(obj, lv_color_white(), LV_STATE_FOCUSED);      
        lv_obj_set_style_text_color(obj, lv_color_white(), LV_STATE_USER_1); 
        lv_obj_set_style_text_font(obj, Font_ResouceGet(FONT_10_BOLD), LV_STATE_USER_1);
        lv_obj_set_style_text_font(obj, Font_ResouceGet(FONT_10_MEDIUM), 0);        
        lv_obj_set_style_text_font(obj, Font_ResouceGet(FONT_12_BOLD), LV_STATE_FOCUSED);
        lv_obj_add_event_cb(obj, gel_cont_event_cb, LV_EVENT_ALL, NULL);

        gel_cont_ceate(obj);
        gel_cont_refresh(obj, p_gel_dsc_index);
        p_gel_dsc_index++;      
    }
}

/**
 * @brief 开关按键事件函数
 * 
 * @param e 
 */
void gel_switch_btn_event(lv_event_t* e)
{
    lv_event_code_t event = lv_event_get_code(e);
    lv_obj_t* obj = lv_event_get_target(e);

    shortcut_keys_event_handler(e);
    backspace_key_event_handler(e);
    
    if (LV_EVENT_FOCUSED == event)
    {
        gel_switch_btn_t* p_gel_btn = (gel_switch_btn_t*)lv_obj_get_user_data(obj);        
        lv_obj_t* focused_obj = lv_obj_get_child(obj, p_gel_btn->sel_index);        
        if (focused_obj != NULL)
        {
            lv_obj_clear_state(focused_obj, LV_STATE_CHECKED);
            lv_obj_add_state(focused_obj, LV_STATE_FOCUSED);
        }        
    }
    else if (LV_EVENT_DEFOCUSED == event)
    {        
        gel_switch_btn_t* p_gel_btn = (gel_switch_btn_t*)lv_obj_get_user_data(obj);        
        lv_obj_t* focused_obj = lv_obj_get_child(obj, p_gel_btn->sel_index);
        if (focused_obj != NULL)
        {
            lv_obj_add_state(focused_obj, LV_STATE_CHECKED);
            lv_obj_clear_state(focused_obj, LV_STATE_FOCUSED);
        }        
    }
    else if (LV_EVENT_RELEASED == event)
    {
        gel_switch_btn_t* p_gel_btn = (gel_switch_btn_t*)lv_obj_get_user_data(obj);        
        lv_obj_t* prev_obj = lv_obj_get_child(obj, p_gel_btn->sel_index);      
        lv_obj_t* focus_obj;

        p_gel_btn->sel_index++;
        if (p_gel_btn->sel_index >= BTN_ID_MAX) {
            p_gel_btn->sel_index = BTN_ID_1;
        }  

        focus_obj = lv_obj_get_child(obj, p_gel_btn->sel_index);
        lv_obj_clear_state(prev_obj, LV_STATE_FOCUSED);
        lv_obj_add_state(focus_obj, LV_STATE_FOCUSED);  

        if (p_gel_btn->ralease_cb)
        {
            p_gel_btn->ralease_cb(p_gel_btn);
        }                     
    }    
}

/**
 * @brief GEL 色纸容器刷新
 * 
 * @param cont      色纸显示容器
 * @param p_gel_dsc 色纸描述符
 * 
 * [计算方法]
 * $grayLevel = $R * 0.299 + $G * 0.587 + $B * 0.114;
 * if ($grayLevel >= 192) {
 *     // add shadow
 * }
 */
void gel_cont_refresh(lv_obj_t* cont, const gel_dsc_t* p_gel_dsc)
{
    if (p_gel_dsc == NULL)      
        return;

    lv_obj_t* color_cont = lv_obj_get_child(cont, 0);
    lv_obj_t* label = lv_obj_get_child(cont, 1);
    lv_obj_t* color_label = lv_obj_get_child(color_cont, 0);    
    uint32_t grayLevel = (uint32_t)(lv_color_hex(p_gel_dsc->hex_color).ch.red * 0.299 +  lv_color_hex(p_gel_dsc->hex_color).ch.green * 0.587 + lv_color_hex(p_gel_dsc->hex_color).ch.blue * 0.114);
    lv_color_t text_color = lv_color_black();

    lv_obj_set_style_bg_color(color_cont, lv_color_hex(p_gel_dsc->hex_color), 0);        
    lv_snprintf(sdb, sizeof(sdb), "%d\n%s", p_gel_dsc->number, p_gel_dsc->name);        

    if (grayLevel < 15) 
        text_color = lv_color_white();  
        
    lv_obj_set_style_text_color(color_cont, text_color, 0);

    //if (grayLevel >= 40) {
    //    lv_obj_set_style_border_color(color_cont, lv_color_make(164, 162, 164), LV_STATE_FOCUSED);
    //} else {
    lv_obj_set_style_border_color(color_cont, lv_color_white(), LV_STATE_FOCUSED);
    //}

    lv_label_set_text(color_label, sdb);
    lv_obj_align(color_label, LV_ALIGN_BOTTOM_LEFT, 0, 0);
    lv_label_set_text_fmt(label, "DMX%d-%d", p_gel_dsc->dmx_value_min, p_gel_dsc->dmx_value_max);   
    lv_label_set_long_mode(label, LV_LABEL_LONG_SCROLL); 
    lv_obj_align_to(label, color_cont, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);    

    UI_PRINTF("Gray level = %d\r\n", grayLevel);
}

/**
 * @brief GEL系列选择控件回调函数
 * 
 * @param p_cont_dsc 
 */
void gel_serials_cont_refresh(custom_cont_t* p_cont_dsc)
{
    lv_group_t* group = lv_obj_get_group(p_cont_dsc->obj);
    lv_obj_t* focused_obj = lv_group_get_focused(group);     
    uint8_t childs_sum = lv_obj_get_child_cnt(p_cont_dsc->obj);
    lv_obj_t* child = NULL;
    bool focused = focused_obj == p_cont_dsc->obj ? true : false;

    for (uint8_t i = 0; i < childs_sum; i++)
    {
        child = lv_obj_get_child(p_cont_dsc->obj, i);
        if (p_cont_dsc->cur_index == i)
        {   
            if (focused)
            {
                lv_obj_add_state(child, LV_STATE_FOCUSED);                      
            }            
            else
            {   
                lv_obj_add_state(child, LV_STATE_CHECKED);
            }            
        }
        else
        {
            lv_obj_clear_state(child, LV_STATE_CHECKED);
            lv_obj_clear_state(child, LV_STATE_FOCUSED);            
        }
    }    
}

/**
 * @brief GEL系列选择框回调函数
 * 
 * @param e 
 * @param p_gel_model gel界面数据模型
 * @param p_gel_table_cont 
 */
void gel_serials_cont_event_cb(lv_event_t* e, struct gel_param *p_gel_model, lv_obj_t* p_gel_table_cont)
{
    lv_obj_t* cont = lv_event_get_target(e);
    lv_event_code_t event = lv_event_get_code(e);
    custom_cont_t* p_series_dsc = lv_obj_get_user_data(cont);
    lv_group_t* group = lv_obj_get_group(cont);

    if (event == LV_EVENT_FOCUSED)
    {
        if (lv_group_get_editing(group) == false)
        {            
            lv_obj_t* cur_focus_obj = lv_obj_get_child(cont, p_series_dsc->cur_index);                        
            
            if (cur_focus_obj != NULL)
            {
                lv_obj_add_state(cur_focus_obj, LV_STATE_FOCUSED);
                lv_event_send(cur_focus_obj, LV_EVENT_FOCUSED, NULL);            
            }            
        }             
    }
    else if (event == LV_EVENT_DEFOCUSED)
    {
        lv_obj_t* cur_focus_obj = lv_obj_get_child(cont, p_series_dsc->cur_index);
        lv_obj_clear_state(cur_focus_obj, LV_STATE_FOCUSED);  
    }
    else if (event == LV_EVENT_RELEASED)
    {
        uint8_t child_sum = lv_obj_get_child_cnt(cont);
        lv_obj_t* obj = NULL;
        uint8_t i = 0;

        if (lv_group_get_editing(group))
        {
            bool exit = p_series_dsc->state == LV_USER_STATE_KEY_BACKSPACE ? true : false;
            uint8_t state = p_series_dsc->state;
            lv_obj_t* cur_obj;
            uint8_t brand = gel_model_get_brand(p_gel_model);
            uint8_t series = gel_model_get_series(p_gel_model);
            
            (void)brand;            
            lv_obj_set_style_outline_color(cont, lv_color_white(), LV_STATE_FOCUSED);
            lv_group_set_editing(group, false);

            for (i = 0; i < child_sum; i++)
            {
                obj = lv_obj_get_child(cont, i);
                lv_obj_clear_state(obj, LV_STATE_USER_1 | LV_STATE_CHECKED | LV_STATE_FOCUSED);                
            }          
            
            if (LV_USER_STATE_EXIT_ALL == state) {
                exit = true;
            }

            if (!exit) {
                cur_obj = lv_obj_get_child(cont, p_series_dsc->cur_index);                                   
                if (series != p_series_dsc->cur_index) {                    
                    ui_enter_critical();
                    gel_model_set_series(p_gel_model, (uint8_t)p_series_dsc->cur_index);
                    ui_exit_critical();
                    series = (uint8_t)p_series_dsc->cur_index;
                    //gel_cont_table_refresh(p_gel_table_cont, p_gel_model);                    
                    gel_cont_table_fliping_refresh(p_gel_table_cont, p_gel_model, gel_model_get_index(p_gel_model));
                }

                lv_group_add_obj(group, p_gel_table_cont);
                lv_group_focus_obj(p_gel_table_cont);
                lv_group_set_editing(group, true);

                lv_obj_add_state(cur_obj, LV_STATE_CHECKED);
            }
            else {
                 p_series_dsc->cur_index = series;
                 cur_obj = lv_obj_get_child(cont, p_series_dsc->cur_index);                   
                 lv_obj_add_state(cur_obj, LV_STATE_CHECKED);
                 lv_event_send(cont, LV_EVENT_FOCUSED, NULL);
             }
            p_series_dsc->state = LV_USER_STATE_IDLE;
        }
        else 
        {                 
            lv_group_set_editing(group, true);
            lv_obj_set_style_outline_color(cont, lv_color_make(160, 160, 160), LV_STATE_FOCUSED);
            for (i = 0; i < child_sum; i++) {
                obj = lv_obj_get_child(cont, i);
                lv_obj_add_state(obj, LV_STATE_USER_1);
            }                  
        }
    }
    else if (event == LV_EVENT_KEY)
    {
        uint32_t key = *(uint32_t*)lv_event_get_param(e);
        lv_obj_t* prev_obj = lv_obj_get_child(cont, p_series_dsc->cur_index);
        
        if (lv_group_get_editing(group))
        {                        
            if (key == LV_ENCODER_KEY_CW)
            {
                p_series_dsc->cur_index += 1;
                if (p_series_dsc->cur_index > p_series_dsc->max)
                {
                    //p_series_dsc->cur_index = p_series_dsc->min;
                    p_series_dsc->cur_index = p_series_dsc->max;
                }
                
                lv_obj_t* next_obj = lv_obj_get_child(cont, p_series_dsc->cur_index);                
                lv_obj_clear_state(prev_obj, LV_STATE_FOCUSED);
                lv_obj_add_state(next_obj, LV_STATE_FOCUSED);
            }
            else if (key == LV_ENCODER_KEY_CC)
            {
                p_series_dsc->cur_index -= 1;
                if (p_series_dsc->cur_index < p_series_dsc->min)
                {
                    //p_series_dsc->cur_index = p_series_dsc->max;
                    p_series_dsc->cur_index = p_series_dsc->min;
                }
                
                lv_obj_t* next_obj = lv_obj_get_child(cont, p_series_dsc->cur_index);                
                lv_obj_clear_state(prev_obj, LV_STATE_FOCUSED);
                lv_obj_add_state(next_obj, LV_STATE_FOCUSED);
            }
            else if (key == LV_KEY_BACKSPACE)
            {
                p_series_dsc->state = LV_USER_STATE_KEY_BACKSPACE;
                lv_event_send(cont, LV_EVENT_RELEASED, NULL);
                UI_PRINTF("LV_KEY_BACKSPACE\r\n");
            }
        }
    }
}

void gel_cont_table_fliping_refresh(lv_obj_t* parent_cont, struct gel_param* p_gel_model, int16_t cur_index)
{
    uint8_t max_nums = gel_get_max_num(gel_model_get_brand(p_gel_model), gel_model_get_series(p_gel_model));
    const gel_dsc_t* p_gel_dsc_index = gel_get_serial_dsc(gel_model_get_brand(p_gel_model), gel_model_get_series(p_gel_model));
    custom_cont_t* p_table_dsc = lv_obj_get_user_data(parent_cont);
    lv_obj_t* focus_obj = NULL;
    uint8_t cur_disp_pos = cur_index % p_table_dsc->max_disp_nums;
    uint8_t start_disp_index = cur_index - cur_disp_pos;
    uint8_t end_disp_index = start_disp_index + p_table_dsc->max_disp_nums - 1;
    uint8_t disp_nums;

    if (end_disp_index >= max_nums)
        end_disp_index = max_nums - 1;

    disp_nums = end_disp_index - start_disp_index + 1;
    p_table_dsc->max = max_nums - 1;
    p_table_dsc->min = 0;
    p_table_dsc->cur_index = cur_index;

    for (uint8_t i = 0; i < p_table_dsc->max_disp_nums; i++)
    {
        lv_obj_t* cont = lv_obj_get_child(parent_cont, i);
        if (cur_disp_pos == i)
            focus_obj = cont;        

        if (i < disp_nums)
        {
            lv_obj_clear_flag(cont, LV_OBJ_FLAG_HIDDEN);
            lv_obj_clear_state(cont, LV_STATE_USER_1);

            //判断该对象是否被聚焦，如果处于聚焦状态，那么就清除聚焦状态并且发送事件给该对象.
            if (lv_obj_get_state(cont) & LV_STATE_FOCUSED)
            {
                lv_obj_clear_state(cont, LV_STATE_FOCUSED);
                lv_event_send(cont, LV_EVENT_DEFOCUSED, NULL);
            }
            
            gel_cont_refresh(cont, &p_gel_dsc_index[start_disp_index + i]);
        }
        else
        {
            lv_obj_add_flag(cont, LV_OBJ_FLAG_HIDDEN);
        }
    }

    if (focus_obj != NULL)
    {        
        bool b_focused = lv_group_get_focused(lv_obj_get_group(parent_cont)) == parent_cont ? true : false;        
        //判断父对象是否被聚焦.
        if (b_focused)
        {
            lv_obj_add_state(focus_obj, LV_STATE_FOCUSED);
            lv_obj_add_state(focus_obj, LV_STATE_USER_1);
            lv_event_send(focus_obj, LV_EVENT_FOCUSED, NULL);
            //lv_obj_scroll_to_view(focus_obj, LV_ANIM_OFF);
        }
        else
        {
            lv_obj_add_state(focus_obj, LV_STATE_USER_1);
            //lv_obj_scroll_to_view(focus_obj, LV_ANIM_OFF);
        }
    }

    lv_obj_invalidate(parent_cont);
}

void gel_cont_table_fliping_event_cb(lv_event_t* e, struct gel_param* p_gel_model)
{
    lv_obj_t* cont = lv_event_get_target(e);
    lv_event_code_t event = lv_event_get_code(e);
    custom_cont_t* p_table_dsc = lv_obj_get_user_data(cont);

    shortcut_keys_event_handler(e);

    if (event == LV_EVENT_DRAW_POST) 
    {
        const lv_area_t * clip_area = lv_event_get_param(e);
        const lv_coord_t scroll_height = 20;
        lv_area_t ver_area;
        uint8_t pages = p_table_dsc->max / p_table_dsc->max_disp_nums;
        uint8_t cur_page = p_table_dsc->cur_index / p_table_dsc->max_disp_nums;
        lv_draw_rect_dsc_t rect_dsc;
        lv_coord_t max_scroll_height;

        if (lv_area_get_width(clip_area) != lv_obj_get_width(cont) 
                    || lv_area_get_height(clip_area) != lv_obj_get_height(cont))
            return ;

        lv_draw_rect_dsc_init(&rect_dsc);
        if ((p_table_dsc->max % p_table_dsc->max_disp_nums) > 0)
            pages += 1;

        if (pages == 1)     return;

        max_scroll_height = (lv_area_get_height(clip_area) - 10) / pages;
        if (max_scroll_height < scroll_height)
        {
            ver_area.y1 = clip_area->y1 + 5 + cur_page * 100 * ((lv_area_get_height(clip_area) - 10) - scroll_height) / pages / 100;
        }
        else
        {            
            lv_coord_t space = ((lv_area_get_height(clip_area) - 10) - scroll_height * pages) / (pages - 1);
            ver_area.y1 = clip_area->y1 + 5 + cur_page * (space + scroll_height);
        }

        ver_area.y2 = ver_area.y1 + scroll_height;
        ver_area.x1 = clip_area->x2 - 5;
        ver_area.x2 = ver_area.x1 + 2;        

        rect_dsc.bg_color = lv_color_make(255, 255, 255);
        rect_dsc.bg_opa = LV_OPA_50;
        rect_dsc.radius = 1;
        lv_draw_rect(&ver_area, clip_area, &rect_dsc);               
    }
    else if (event == LV_EVENT_FOCUSED)
    {
        lv_group_t* group = lv_obj_get_group(cont);
        if (lv_group_get_editing(group) == false)
        {                                    
            lv_obj_t* cur_focus_obj = lv_obj_get_child(cont, get_container_index(p_table_dsc));
            if (cur_focus_obj != NULL)
            {
                lv_obj_add_state(cur_focus_obj, LV_STATE_FOCUSED);
                lv_event_send(cur_focus_obj, LV_EVENT_FOCUSED, NULL);
            }            
        }
        else 
        {
            lv_obj_invalidate(cont);
        }             
    } 
    else if (event == LV_EVENT_DEFOCUSED)
    {
        lv_obj_t* cur_focus_obj = lv_obj_get_child(cont, get_container_index(p_table_dsc));
        if (cur_focus_obj != NULL)
        {
            lv_obj_clear_state(cur_focus_obj, LV_STATE_FOCUSED);
            lv_obj_add_state(cur_focus_obj, LV_STATE_USER_1);
            lv_event_send(cur_focus_obj, LV_EVENT_DEFOCUSED, NULL);
        }        
    }
    else if (event == LV_EVENT_RELEASED)
    {
        lv_group_t* group = lv_obj_get_group(cont);
        if (lv_group_get_editing(group))
        {
            lv_group_set_editing(group, false);         

             uint8_t child_sum = lv_obj_get_child_cnt(cont);
             lv_obj_t* obj;
             uint8_t i;
             custom_cont_t* p_series_dsc;
            
             for (i = 0; i < child_sum; i++)
             {
                 obj = lv_obj_get_child(cont, i);
                 lv_obj_clear_state(obj, LV_STATE_USER_1);
             }          

             gel_model_set_index(p_gel_model, (uint8_t)p_table_dsc->cur_index);            
             lv_group_focus_prev(group);            
             lv_group_remove_obj(cont);

             obj = lv_group_get_focused(group);
             p_series_dsc = lv_obj_get_user_data(obj);
             if (p_table_dsc->state == LV_USER_STATE_KEY_BACKSPACE)
             {
                 if (p_series_dsc->state != LV_USER_STATE_EXIT_ALL) 
                 {
                     p_series_dsc->state = LV_USER_STATE_ENTER;
                     lv_event_send(obj, LV_EVENT_RELEASED, NULL);
                 }
             }                               

             p_table_dsc->state = LV_USER_STATE_IDLE;
        }
        else 
        {
             //confirm yes            
             lv_group_set_editing(group, true);              
             uint8_t cue_gel_index = gel_model_get_index(p_gel_model);
             if (p_table_dsc->cur_index != cue_gel_index)
             {
                 lv_obj_t* prev_obj = lv_obj_get_child(cont,gel_model_get_series(p_gel_model));
                 lv_obj_t* next_obj = lv_obj_get_child(cont, p_table_dsc->cur_index);                 

                 gel_model_set_index(p_gel_model, (uint8_t)p_table_dsc->cur_index);              
                 lv_obj_clear_state(prev_obj, LV_STATE_CHECKED);
                 lv_obj_add_state(next_obj, LV_STATE_CHECKED);
             }
        }
    }
    else if (event == LV_EVENT_KEY)
    {
        lv_group_t* group = lv_obj_get_group(cont);
        uint32_t key = *(uint32_t*)lv_event_get_param(e);
        lv_obj_t* prev_obj = lv_obj_get_child(cont, get_container_index(p_table_dsc));

        if (lv_group_get_editing(group))
        {
            if (key == LV_ENCODER_KEY_CW)
            {
                p_table_dsc->cur_index += 1;
                if (p_table_dsc->cur_index > p_table_dsc->max)
                {
                    p_table_dsc->cur_index = p_table_dsc->max;
                    return ;
                }                                
                
                lv_obj_t* next_obj = lv_obj_get_child(cont, get_container_index(p_table_dsc));
                lv_obj_clear_state(prev_obj, LV_STATE_USER_1);
                lv_obj_clear_state(prev_obj, LV_STATE_FOCUSED);
                lv_event_send(prev_obj, LV_EVENT_DEFOCUSED, NULL);
                lv_obj_add_state(next_obj, LV_STATE_FOCUSED);
                lv_event_send(next_obj, LV_EVENT_FOCUSED, NULL);                
                
                //flip page.... 
                if (p_table_dsc->cur_index % p_table_dsc->max_disp_nums == 0) 
                    gel_cont_table_fliping_refresh(cont, p_gel_model, p_table_dsc->cur_index);
                lv_obj_invalidate(cont);
                gel_model_set_index(p_gel_model, (uint8_t)p_table_dsc->cur_index);
                UI_PRINTF("Brand: %d, Series: %d, Current Index: %d\r\n", gel_model_get_brand(p_gel_model), gel_model_get_series(p_gel_model), gel_model_get_index(p_gel_model));
            }
            else if (key == LV_ENCODER_KEY_CC)
            {
                bool flip = false;
                if (p_table_dsc->cur_index % p_table_dsc->max_disp_nums == 0
                                                        && p_table_dsc->cur_index != 0)
                {
                    flip = true;
                }
                                 
                p_table_dsc->cur_index -= 1;
                if (p_table_dsc->cur_index < p_table_dsc->min)
                {
                    p_table_dsc->cur_index = p_table_dsc->min;
                    return ;
                }
                    
                lv_obj_t* next_obj = lv_obj_get_child(cont, get_container_index(p_table_dsc));
                lv_obj_clear_state(prev_obj, LV_STATE_USER_1);
                lv_obj_clear_state(prev_obj, LV_STATE_FOCUSED);
                lv_event_send(prev_obj, LV_EVENT_DEFOCUSED, NULL);
                lv_obj_add_state(next_obj, LV_STATE_FOCUSED);
                lv_event_send(next_obj, LV_EVENT_FOCUSED, NULL);
                
                if (flip)
                    gel_cont_table_fliping_refresh(cont, p_gel_model, p_table_dsc->cur_index);     
                lv_obj_invalidate(cont);          
                gel_model_set_index(p_gel_model, (uint8_t)p_table_dsc->cur_index);
                UI_PRINTF("Brand: %d, Series: %d, Current Index: %d\r\n", gel_model_get_brand(p_gel_model), gel_model_get_series(p_gel_model), gel_model_get_index(p_gel_model));
            }
            else if (key == LV_KEY_BACKSPACE)
            {
                p_table_dsc->state = LV_USER_STATE_KEY_BACKSPACE;
                lv_event_send(cont, LV_EVENT_RELEASED, NULL);
            }
        }      
    }
}

/**
 * @brief 色纸界面开关按键
 * 
 * @param obj 显示容器
 * @param string1 字符串1
 * @param string2 字符串2
 * @param w 宽度
 * @param h 高度
 * @return lv_obj_t* 
 */
lv_obj_t* gel_switch_btn_create(lv_obj_t* obj, const char* string1, const char* string2, lv_coord_t w, lv_coord_t h)
{
    gel_style_t* p_gel_style = gel_style_get();        
    lv_obj_t* cct_cont = lv_obj_create(obj);   

    lv_obj_add_style(cct_cont, &style_rect_panel, 0);
    lv_obj_set_style_text_font(cct_cont, Font_ResouceGet(FONT_12), 0);
    lv_obj_set_style_radius(cct_cont, 5, 0);
    lv_obj_set_style_clip_corner(cct_cont, true, 0);
    lv_obj_set_flex_flow(cct_cont, LV_FLEX_FLOW_ROW);
    lv_obj_set_style_pad_gap(cct_cont, 0, 0);
    lv_obj_set_style_outline_color(cct_cont, lv_color_white(), LV_STATE_FOCUSED);
    lv_obj_set_style_outline_width(cct_cont, 2, LV_STATE_FOCUSED);
    lv_obj_set_size(cct_cont, w, h);

    lv_obj_t* btn1 = lv_obj_create(cct_cont);
    lv_obj_clear_flag(btn1, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_flag(btn1, LV_OBJ_FLAG_SCROLL_ON_FOCUS);    
    lv_obj_add_style(btn1, &p_gel_style->btn_style, 0);
    lv_obj_add_style(btn1, &p_gel_style->btn_focsed_style, LV_STATE_FOCUSED);
    lv_obj_add_style(btn1, &p_gel_style->btn_checked_style, LV_STATE_CHECKED);
    lv_obj_set_style_border_width(btn1, 0, 0);
    lv_obj_set_size(btn1, lv_pct(50), h);

    lv_obj_t* label1 = lv_label_create(btn1);
    lv_label_set_text(label1, string1);
    lv_obj_center(label1);

    lv_obj_t* btn2 = lv_obj_create(cct_cont);        
    lv_obj_clear_flag(btn2, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_flag(btn2, LV_OBJ_FLAG_SCROLL_ON_FOCUS);      
    lv_obj_add_style(btn2, &p_gel_style->btn_style, 0);
    lv_obj_add_style(btn2, &p_gel_style->btn_focsed_style, LV_STATE_FOCUSED);
    lv_obj_add_style(btn2, &p_gel_style->btn_checked_style, LV_STATE_CHECKED);
    lv_obj_set_style_border_width(btn2, 0, 0);
    lv_obj_set_size(btn2, lv_pct(50), h);
    
    lv_obj_t* label2 = lv_label_create(btn2);
    lv_label_set_text(label2, string2);    
    lv_obj_center(label2);

    return cct_cont;
}

/**
 * @brief 色纸开关按键刷新
 * 
 * @param p_switch_btn_dsc 
 */
void gel_switch_btn_refresh(gel_switch_btn_t* p_switch_btn_dsc)
{
    lv_group_t* group = lv_obj_get_group(p_switch_btn_dsc->btn);
    lv_obj_t* focused_obj = lv_group_get_focused(group);
    bool focused = focused_obj == p_switch_btn_dsc->btn ? true : false;

    if (lv_group_get_editing(group))    return;

    uint8_t childs_sum = lv_obj_get_child_cnt(p_switch_btn_dsc->btn);
    lv_obj_t* child = NULL;

    for (uint8_t i = 0; i < childs_sum; i++)
    {
        child = lv_obj_get_child(p_switch_btn_dsc->btn, i);
        if (p_switch_btn_dsc->sel_index == i)
        {   
            if (focused)
            {
                lv_obj_add_state(child, LV_STATE_FOCUSED);                      
            }            
            else
            {   
                lv_obj_add_state(child, LV_STATE_CHECKED);
            }            
        }
        else
        {
            lv_obj_clear_state(child, LV_STATE_CHECKED);
            lv_obj_clear_state(child, LV_STATE_FOCUSED);            
        }
    }
}

/**********************
 *   STATIC FUNCTIONS
 **********************/
static void page_gel_construct(void)
{ 
	lv_obj_set_style_bg_color(lv_scr_act(), lv_color_black(), 0);
	uint8_t curre_mode = 0;
    struct sys_info_eth eth;
//    struct sys_info_crmx crmx_sta;
	data_center_read_config_data(SYS_CONFIG_BLE_CTRL, &curre_mode);
    ui_set_ble_state(curre_mode);
    data_center_read_config_data(SYS_CONFIG_CRMX_CTRL, &curre_mode);
    ui_set_crmx_state(curre_mode);
    data_center_read_sys_info(SYS_INFO_ETH, &eth);
    ui_set_lan_state(eth.pluged);
    data_center_read_config_data(SYS_CONFIG_FAN_MODE, &curre_mode);
    ui_set_fan_mode(curre_mode);
    data_center_read_config_data(SYS_CONFIG_CURVE_TYPE, &curre_mode);
    ui_set_curve_type(curre_mode);
    lv_obj_t* t_cont = lv_obj_create(lv_scr_act());
    lv_obj_remove_style_all(t_cont);
    lv_obj_set_size(t_cont, lv_pct(100), lv_pct(13));
    lv_obj_set_style_text_color(t_cont, lv_color_white(), 0);
    lv_obj_set_pos(t_cont, 0, 0);

    title_cont_create(t_cont, screen_get_act_page_name());

    lv_obj_t* b_cont = lv_obj_create(lv_scr_act());
    lv_obj_remove_style_all(b_cont);
    lv_obj_set_size(b_cont, lv_pct(100), lv_pct(87));
    lv_obj_align_to(b_cont, t_cont, LV_ALIGN_OUT_BOTTOM_MID, 0, 0);
    page_gel_init(b_cont);  

    uint8_t id = PAGE_LIGHT_MODE_GEL;
    uint8_t sub_id = 0;
    data_center_write_config_data(SYS_CONFIG_LOCAL_UI, &id);
    data_center_write_config_data(SYS_CONFIG_LOCAL_SUB_UI, &sub_id);
}

static void page_gel_destruct(void)
{
    lv_group_focus_freeze(indev_group, true);
    lv_group_remove_all_objs(indev_group);
    lv_group_del(indev_group);
    indev_group = NULL;

    lv_obj_clean(lv_scr_act());
}

static void page_gel_time_upd_cb(uint32_t ms)
{
    page_invalid_type_t type = page_get_invalid_type();
    bool force_upd = false;
    uint8_t curre_mode = 0;
    struct sys_info_eth eth;
//    struct sys_info_crmx crmx_sta;
	data_center_read_config_data(SYS_CONFIG_BLE_CTRL, &curre_mode);
    ui_set_ble_state(curre_mode);
    data_center_read_config_data(SYS_CONFIG_CRMX_CTRL, &curre_mode);
    ui_set_crmx_state(curre_mode);
    data_center_read_sys_info(SYS_INFO_ETH, &eth);
    ui_set_lan_state(eth.pluged);
    data_center_read_config_data(SYS_CONFIG_FAN_MODE, &curre_mode);
    ui_set_fan_mode(curre_mode);
    data_center_read_config_data(SYS_CONFIG_CURVE_TYPE, &curre_mode);
    ui_set_curve_type(curre_mode);
	data_center_read_config_data(SYS_CONFIG_HIGH_SPEED_MODE, &curre_mode);
	ui_set_hight_speed_mode(curre_mode);
    //update the title bar.
    lv_obj_t* t_cont = lv_obj_get_child(lv_scr_act(), 0);
    title_update_handler(t_cont);
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

    //如果当前正在处于编辑状态，那么退出编辑状态。
    if (lv_group_get_editing(indev_group))
    {
        lv_obj_t* focused_obj = lv_group_get_focused(indev_group);
        if (focused_obj == serials_cont || focused_obj == gel_table_cont) 
            serial_cont_dsc.state = LV_USER_STATE_EXIT_ALL;
        
        lv_event_send(focused_obj, LV_EVENT_RELEASED, NULL);
    }

    //update the CCT Button.
    uint16_t cct_type = p_gel_model->gel.cct == 3200 ? 0 : 1;

    if (cct_switch_btn.sel_index != cct_type|| force_upd)
    {
        cct_switch_btn.sel_index = cct_type;
        gel_switch_btn_refresh(&cct_switch_btn);
    }

    ////update the brand type
    if (brand_switch_btn.sel_index != gel_model_get_brand(&(p_gel_model->gel)) || force_upd) //修改色纸品牌
    {
        brand_switch_btn.sel_index = gel_model_get_brand(&(p_gel_model->gel));
        serial_cont_dsc.cur_index = gel_model_get_series(&(p_gel_model->gel));
        gel_table_dsc.cur_index = gel_model_get_index(&(p_gel_model->gel));
        gel_switch_btn_refresh(&brand_switch_btn);
        
        lv_obj_clean(serials_cont);
        serials_cont_init(serials_cont, &(p_gel_model->gel));
        gel_cont_table_fliping_refresh(gel_table_cont, &(p_gel_model->gel), gel_table_dsc.cur_index);        
    }
    if (serial_cont_dsc.cur_index != gel_model_get_series(&(p_gel_model->gel))) //修改色纸系列
    {
        serial_cont_dsc.cur_index = gel_model_get_series(&(p_gel_model->gel));
        gel_serials_cont_refresh(&serial_cont_dsc);
        gel_cont_table_fliping_refresh(gel_table_cont, &(p_gel_model->gel), gel_table_dsc.cur_index);                
    }
    if (gel_table_dsc.cur_index != gel_model_get_index(&(p_gel_model->gel))) //修改色纸索引
    {
        gel_table_dsc.cur_index = gel_model_get_index(&(p_gel_model->gel));
        gel_cont_table_fliping_refresh(gel_table_cont, &(p_gel_model->gel), gel_table_dsc.cur_index);        
    }

    //刷新所有可调按键
    adj_btn_refresh_all(&int_btn, sizeof(int_btn)/ sizeof(int_btn), force_upd);
}

/**
 * @brief 事件回调函数
 * 
 * @param[in]event          事件
 *       @EVENT_DATA_WRITE   向数据中心写入
 *       @EVENT_DATA_PULL    从数据中心读出
 */
static void page_gel_event_cb(int event)
{
    struct db_gel gel;
    uint32_t src_crc32, dest_crc32;

    data_center_read_light_data(LIGHT_MODE_GEL, &gel);   
    dest_crc32 = ui_calc_checksum((uint8_t*)&gel,  sizeof(struct db_gel));    
    switch (event)
    {
        case EVENT_DATA_WRITE:
        {
            src_crc32 = ui_calc_checksum((uint8_t*)p_gel_model, sizeof(struct db_gel));            
            if (dest_crc32 != src_crc32)
            {
                data_center_write_light_data(LIGHT_MODE_GEL, p_gel_model);
            }
        }
        break;
        case EVENT_DATA_PULL:
        {
            memcpy(p_gel_model, &gel, sizeof(struct db_gel));
            page_set_invalid_type(PAGE_INVALID_TYPE_PORTION);
        }
        break;
		case EVENT_DATA_COMPULSION:
            data_center_write_light_data(LIGHT_MODE_GEL, p_gel_model);
        break;
    }
}

/**
 * @brief 系列显示容器回调函数
 * 
 * @param e 
 */
static void serials_cont_event_cb(lv_event_t* e)
{
    shortcut_keys_event_handler(e);
    backspace_key_event_handler(e);

    gel_serials_cont_event_cb(e, &(p_gel_model->gel), gel_table_cont);
}

/**
 * @brief 色纸颜色表容器控件回调函数
 * 
 * @param e 
 */
static void cont_table_event_cb(lv_event_t* e)
{
   //gel_cont_table_event_cb(e, p_gel_model);
   gel_cont_table_fliping_event_cb(e, &(p_gel_model->gel));
}

static void int_upd_run_cb(void* data)
{
    adj_btn_t* dsc = (adj_btn_t*)data;
	if(gui_get_hs_mode())
	{
		if(dsc->value == 0 )dsc->value = 0;
		else dsc->value = dsc->value < 200 ? 200 : dsc->value;
	}
    p_gel_model->lightness = dsc->value;
    lv_bar_set_value(intensity_bar, dsc->value, LV_ANIM_OFF);
}

static bool int_assign_value_cb(void* data)
{
    adj_btn_t* dsc = (adj_btn_t*)data;
    ASSIGN_VALUE_TO_BTN(p_gel_model->lightness);
}

static void cct_btn_ralease_cb(gel_switch_btn_t* btn)
{
    if (btn->sel_index == 0)
        p_gel_model->gel.cct = 3200;    
    else 
        p_gel_model->gel.cct = 5600;
}

static void brand_btn_release_cb(gel_switch_btn_t* btn)
{
    if (gel_model_get_brand(&(p_gel_model->gel)) != btn->sel_index)
    {
        gel_model_set_brand(&(p_gel_model->gel), btn->sel_index);

        //clean the series container, then begin to initial.
        lv_obj_clean(serials_cont);
        serials_cont_init(serials_cont, &(p_gel_model->gel));
        gel_cont_table_fliping_refresh(gel_table_cont, &(p_gel_model->gel), gel_model_get_index(&(p_gel_model->gel)));        
    }
}
