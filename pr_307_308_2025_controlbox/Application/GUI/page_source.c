/*********************
 *      INCLUDES
 *********************/
#include "ui_common.h"
#include "page_widget.h"

/*********************
 *      DEFINES
 *********************/
#define X_BTN_WIDTH                 124
#define X_BTN_HEIGHT                44

#define Y_BTN_WIDTH                 X_BTN_WIDTH
#define Y_BTN_HEIGHT                X_BTN_HEIGHT

#define BAR_CONT_HEIGHT             32
#define INTENSITY_CONT_HEIGHT       32 

#define BTN_TEXT_COLOR              lv_color_make(161, 161, 161)
#define BTN_BG_COLOR                lv_color_make(41, 40, 41)

#define BTN_BG_USER1_COLOR          lv_color_make(255, 255, 255)
#define BTN_BG_FOCUSED_COLOR        RED_THEME_COLOR
#define BTN_TEXT_FOCUSED_COLOR      lv_color_make(255, 255, 255)

#define MAX_OFFSET_VALUE            50
/*********************
 *      TPEDEFS
 *********************/


/**********************
 *  STATIC PROTOTYPES
 **********************/
static void int_upd_run_cb(void* data);
static void x_upd_run_cb(void* data);
static void y_upd_run_cb(void* data);

static bool int_assign_value_cb(void* data);
static bool x_assign_value_cb(void* data);
static bool y_assign_value_cb(void* data);

static void src_btn_style_init(lv_obj_t* obj);
static void src_btn_cont_event_cb(lv_event_t* e);
static void src_btn_event_cb(lv_event_t* e);
static void refresh_xy_btn(uint8_t index, uint16_t x, uint16_t y);
static void src_set_snap_mode(lv_obj_t* cont, uint8_t idx);

static void page_source_construct(void);
static void page_source_destruct(void);
static void page_source_event_cb(int event);
static void page_source_time_upd_cb(uint32_t ms);
/*********************
 *  STATIC VATIABLES
 *********************/
static lv_group_t *indev_group;
static lv_obj_t* intensity_bar;

static adj_btn_t btn_list[] = {
    ADJ_BTN_INITIALIZER(0, 1000, 1, 500, "INT", "%", 3, 40, -3, false, false,  intensity_get_value_str, int_upd_run_cb, int_assign_value_cb, NULL, common_intensity_get_step),     
    ADJ_BTN_INITIALIZER(0, 8000, 1, 4260, "X", "" , 25, 40, -10, false, false,  xy_get_value_str, x_upd_run_cb, x_assign_value_cb, NULL, NULL),
    ADJ_BTN_INITIALIZER(0, 8000, 1, 4005, "Y", "" , 25, 40, -10, false, false,  xy_get_value_str, y_upd_run_cb, y_assign_value_cb, NULL, NULL), 
};
static struct db_source* p_src_model;
static lv_obj_t* x_btn;
static lv_obj_t* y_btn;
static custom_cont_t src_custom_cont;
static src_tab_t src_table[SOURCE_MAX];
/*********************
 *  GLOBAL VATIABLES
 *********************/
page_t page_source = PAGE_INITIALIZER("Source",
                                PAGE_LIGHT_MODE_SOURCE, 
                                page_source_construct, 
                                page_source_destruct, 
                                page_source_time_upd_cb,
                                page_source_event_cb,                                
                                PAGE_NONE);

/**********************
 *   GLOBAL FUNCTIONS
 **********************/
void page_source_init(lv_obj_t* parent)
{
    const src_table_t* p_table;
    p_src_model = (struct db_source*)screen_get_act_page_data();

    for (uint8_t i = 0; i < SOURCE_MAX; i++)
    {
        p_table = source_get_table(i);
        src_table[i] = p_table->value;
    }
    indev_group = lv_group_create();    
    page_event_send(EVENT_DATA_PULL);

    my_indev_set_group(indev_group);
    lv_group_set_default(indev_group);
    lv_obj_set_style_bg_color(parent, lv_color_black(), 0);
    
    lv_obj_t* obj_panel = lv_obj_create(parent);
    lv_obj_remove_style_all(obj_panel);
    lv_obj_set_size(obj_panel, lv_pct(100), lv_pct(100));
    lv_obj_align(obj_panel, LV_ALIGN_TOP_MID, 0, 0);

    lv_obj_t* intensity_cont = lv_obj_create(obj_panel);
    lv_obj_remove_style_all(intensity_cont);
    lv_obj_set_flex_flow(intensity_cont, LV_FLEX_FLOW_ROW);
    lv_obj_set_style_pad_column(intensity_cont, 10, 0);
    lv_obj_set_style_pad_hor(intensity_cont, 10, 0);    
    lv_obj_set_size(intensity_cont, lv_pct(100), INTENSITY_CONT_HEIGHT);
    lv_obj_align(intensity_cont, LV_ALIGN_TOP_MID, 0, 5);
    
    lv_obj_t* bar_cont = lv_obj_create(intensity_cont);
    lv_obj_clear_flag(bar_cont, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_style(bar_cont, &style_rect_panel, 0);
    lv_obj_set_style_text_color(bar_cont, lv_color_make(255, 255, 255), 0);
    lv_obj_set_style_text_font(bar_cont, Font_ResouceGet(FONT_10), 0);
    lv_obj_set_size(bar_cont, 175, lv_pct(100));
    //lv_obj_align(bar_cont, LV_ALIGN_TOP_MID, -10, 0);

    intensity_bar = intensity_bar_create(bar_cont, 123, 13, 2, -5);    
    if(gui_get_hs_mode())
		p_src_model->lightness = p_src_model->lightness < 200 ? 20 : p_src_model->lightness;
    lv_obj_t* int_btn_cont = adj_btn_create(intensity_cont, LIGHT_MODE_COMMON_RADIUS, ADJ_BTN_TYPE_18);
    lv_obj_set_size(int_btn_cont, 122, lv_pct(100));
    adj_btn_init(int_btn_cont, &btn_list[0], 20);
    lv_obj_set_user_data(int_btn_cont, (void *)&btn_list[0]);
    lv_obj_add_event_cb(int_btn_cont, adj_btn_event_cb, LV_EVENT_ALL, NULL);

    src_custom_cont.min = 0;
    src_custom_cont.cur_index = p_src_model->source.type;
    src_custom_cont.max = SOURCE_MAX - 1;

    lv_obj_t* src_btn_cont = lv_obj_create(obj_panel);  
    lv_obj_set_style_pad_all(src_btn_cont, 0, 0);
    lv_obj_set_style_border_width(src_btn_cont, 0, 0);
    lv_obj_set_flex_flow(src_btn_cont, LV_FLEX_FLOW_ROW);
    //lv_obj_set_style_flex_cross_place(src_btn_cont, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_pad_gap(src_btn_cont, 5, 0);
    lv_obj_set_style_pad_bottom(src_btn_cont, 0, 0);
    lv_obj_set_style_pad_top(src_btn_cont, 0, 0);
    lv_obj_set_style_pad_left(src_btn_cont, 10, 0);
    lv_obj_set_style_pad_right(src_btn_cont, 10, 0);
    lv_obj_set_size(src_btn_cont, lv_pct(100), 96 + 15);      
    lv_obj_align_to(src_btn_cont, intensity_cont, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);
    lv_obj_set_user_data(src_btn_cont, &src_custom_cont);
    lv_obj_add_event_cb(src_btn_cont, src_btn_cont_event_cb, LV_EVENT_ALL, NULL);
    src_custom_cont.obj = src_btn_cont;

    lv_obj_set_style_bg_color(src_btn_cont, lv_color_make(0, 0, 0), 0);  
    for (uint8_t i = 0; i <  SOURCE_MAX; i++)
    {        
        lv_obj_t* obj = lv_obj_create(src_btn_cont);
        lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
        lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
        lv_obj_set_style_pad_all(obj, 0, 0);
        src_btn_style_init(obj); 

        lv_obj_set_size(obj, 71, 94);  
        lv_obj_set_style_width(obj, 72, LV_STATE_FOCUSED);
        lv_obj_set_style_height(obj, 96, LV_STATE_FOCUSED);

        p_table = source_get_table(i);
        lv_obj_t* label = lv_label_create(obj);
        lv_obj_set_size(label, lv_pct(100), LV_SIZE_CONTENT);
        lv_label_set_long_mode(label, LV_LABEL_LONG_WRAP);

        lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, 0);
        lv_label_set_text_fmt(label, "%s", p_table->name);        
        lv_obj_center(label);
        lv_obj_add_event_cb(obj, src_btn_event_cb, LV_EVENT_ALL, NULL);
    }
    
    /* 设置容器滚动形式 */
    if (p_src_model->source.type < (src_custom_cont.max - 2) && p_src_model->source.type >= 2)
    {
        lv_obj_set_scroll_snap_x(src_btn_cont, LV_SCROLL_SNAP_CENTER);
    } 

    lv_obj_t* bottom_cont = lv_obj_create(obj_panel);
    lv_obj_remove_style_all(bottom_cont);
    lv_obj_set_size(bottom_cont, lv_pct(100), 60);

    x_btn = adj_btn_create(bottom_cont, LIGHT_MODE_COMMON_RADIUS, ADJ_BTN_TYPE_18);
    lv_obj_set_size(x_btn, X_BTN_WIDTH, X_BTN_HEIGHT);
    lv_obj_align(x_btn, LV_ALIGN_TOP_LEFT, 24, 0);
    lv_obj_set_user_data(x_btn, (void *)&btn_list[1]);
    adj_btn_init(x_btn, &btn_list[1], 20);
    lv_obj_add_event_cb(x_btn, adj_btn_event_cb, LV_EVENT_ALL, NULL);    

    y_btn = adj_btn_create(bottom_cont, LIGHT_MODE_COMMON_RADIUS, ADJ_BTN_TYPE_18);
    lv_obj_set_size(y_btn, X_BTN_WIDTH, X_BTN_HEIGHT);
    lv_obj_align_to(y_btn, x_btn, LV_ALIGN_OUT_RIGHT_MID, 24, 0);
    lv_obj_set_user_data(y_btn, (void *)&btn_list[2]);
    adj_btn_init(y_btn, &btn_list[2], 20);
    lv_obj_add_event_cb(y_btn, adj_btn_event_cb, LV_EVENT_ALL, NULL);

    lv_obj_align_to(bottom_cont, src_btn_cont, LV_ALIGN_OUT_BOTTOM_MID, 0, 0);
    lv_group_add_obj(indev_group, int_btn_cont);
    lv_group_add_obj(indev_group, src_btn_cont);
    lv_group_add_obj(indev_group, x_btn);
    lv_group_add_obj(indev_group, y_btn);

    lv_obj_t* src_obj = lv_obj_get_child(src_btn_cont, p_src_model->source.type);
    lv_obj_scroll_to_view(src_obj, LV_ANIM_OFF);

    lv_obj_add_state(src_obj, LV_STATE_USER_1);  

    p_src_model->source.x = src_table[p_src_model->source.type].coord_x;
    p_src_model->source.y = src_table[p_src_model->source.type].coord_y;
    refresh_xy_btn(p_src_model->source.type, p_src_model->source.x, p_src_model->source.y);

    lv_group_focus_obj(int_btn_cont);
    lv_group_set_editing(indev_group, false);
    lv_group_set_wrap(indev_group, false);

    adj_btn_refresh_all(btn_list, sizeof(btn_list)/ sizeof(btn_list[0]), true);
     page_event_send(EVENT_DATA_COMPULSION);
    uint8_t id = PAGE_LIGHT_MODE_SOURCE;
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

/**********************
 *   STATIC FUNCTIONS
 **********************/
static void page_source_construct(void)
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
    page_source_init(b_cont);
    
}

static void page_source_destruct(void)
{
    lv_group_focus_freeze(indev_group, true);
    lv_group_remove_all_objs(indev_group);
    lv_group_del(indev_group);
    indev_group = NULL;

    lv_obj_clean(lv_scr_act());
}

static void page_source_time_upd_cb(uint32_t ms)
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

    if (lv_group_get_editing(indev_group))
    {
        lv_obj_t* fucosed_obj = lv_group_get_focused(indev_group);
        lv_event_send(fucosed_obj, LV_EVENT_RELEASED, NULL);
    }

    if (src_custom_cont.cur_index != p_src_model->source.type)
    {
        lv_obj_t* cont = lv_group_get_focused(indev_group);
        lv_obj_t* cur_focus_obj = lv_obj_get_child(src_custom_cont.obj, src_custom_cont.cur_index);

        /* 正常样式 */
        lv_obj_set_style_text_color(cur_focus_obj, BTN_TEXT_COLOR, 0);
        lv_obj_set_style_bg_color(cur_focus_obj, BTN_BG_COLOR, 0);
        lv_obj_set_style_bg_opa(cur_focus_obj, LV_OPA_100, 0);
        lv_obj_clear_state(cur_focus_obj, LV_STATE_USER_1);

        src_custom_cont.cur_index = p_src_model->source.type;       
        cur_focus_obj = lv_obj_get_child(src_custom_cont.obj, src_custom_cont.cur_index);

        /* 被聚焦样式 */
        if (cont == src_custom_cont.obj)
        {
            lv_obj_set_style_bg_color(cur_focus_obj, BTN_BG_FOCUSED_COLOR, 0);
            lv_obj_set_style_text_color(cur_focus_obj, lv_color_black(), 0);
        }
        else
        {            
            lv_obj_set_style_text_color(cur_focus_obj, BTN_TEXT_COLOR, 0);
            lv_obj_set_style_bg_opa(cur_focus_obj, LV_OPA_100, 0);
            lv_obj_add_state(cur_focus_obj, LV_STATE_USER_1);
        }

        src_set_snap_mode(src_custom_cont.obj, src_custom_cont.cur_index);
        lv_obj_scroll_to_view(cur_focus_obj, LV_ANIM_OFF);

        refresh_xy_btn((uint8_t)src_custom_cont.cur_index, src_table[src_custom_cont.cur_index].coord_x, src_table[src_custom_cont.cur_index].coord_y);
    }

    adj_btn_refresh_all(btn_list, sizeof(btn_list)/ sizeof(btn_list[0]), force_upd);
}

/**
 * @brief 事件回调函数
 * 
 * @param[in]event          事件
 *       @EVENT_DATA_WRITE   向数据中心写入
 *       @EVENT_DATA_PULL    从数据中心读出
 */
static void page_source_event_cb(int event)
{
    struct db_source source;
    uint32_t src_crc32, dest_crc32;

    data_center_read_light_data(LIGHT_MODE_SOURCE, &source);   
    dest_crc32 = ui_calc_checksum((uint8_t*)&source,  sizeof(struct db_source));    
    switch (event)
    {
        case EVENT_DATA_WRITE:
        {
            src_crc32 = ui_calc_checksum((uint8_t*)p_src_model, sizeof(struct db_source));            
            if (dest_crc32 != src_crc32)
            {
                data_center_write_light_data(LIGHT_MODE_SOURCE, p_src_model);
            }
        }
        break;
        case EVENT_DATA_PULL:
        {
            memcpy(p_src_model, &source, sizeof(struct db_source));
            page_set_invalid_type(PAGE_INVALID_TYPE_PORTION);
        }
        break;
		case EVENT_DATA_COMPULSION:
            data_center_write_light_data(LIGHT_MODE_SOURCE, p_src_model);
        break;
    }
}

static void refresh_xy_btn(uint8_t index, uint16_t x, uint16_t y)
{
    if (x_btn == NULL || y_btn == NULL)     return;

    const src_table_t* p_table = source_get_table(index);
    adj_btn_t* xdsc = lv_obj_get_user_data(x_btn);
    adj_btn_t* ydsc = lv_obj_get_user_data(y_btn);
    
    xdsc->value = x;
    xdsc->max = p_table->value.coord_x + 50;
    xdsc->min = p_table->value.coord_x - 50;    

    ydsc->value = y;
    ydsc->max = p_table->value.coord_y + 50;
    ydsc->min = p_table->value.coord_y - 50;    

    adj_btn_run_update_cb(xdsc);
    adj_btn_refresh(x_btn, xdsc);
    adj_btn_run_update_cb(ydsc);
    adj_btn_refresh(y_btn, ydsc);
}

static void src_btn_cont_event_cb(lv_event_t* e)
{
    lv_obj_t* cont = lv_event_get_target(e);
    lv_event_code_t event = lv_event_get_code(e);
    custom_cont_t *custom_dsc = lv_obj_get_user_data(cont);

    shortcut_keys_event_handler(e);
    backspace_key_event_handler(e);

    if (event == LV_EVENT_FOCUSED)
    {
        lv_group_t* group = lv_obj_get_group(cont);

        if (lv_group_get_editing(group) == false)
        {
            lv_obj_t* cur_focus_obj = lv_obj_get_child(cont, custom_dsc->cur_index);
            lv_obj_set_style_bg_color(cur_focus_obj, BTN_BG_FOCUSED_COLOR, 0);
            lv_obj_set_style_text_color(cur_focus_obj, lv_color_white(), 0);
            lv_obj_scroll_to_view(cur_focus_obj, LV_ANIM_OFF);   
            lv_obj_clear_state(cur_focus_obj, LV_STATE_USER_1);
        }     
    }
    else if (event == LV_EVENT_DEFOCUSED)
    {
        lv_obj_t* cur_focus_obj = lv_obj_get_child(cont, custom_dsc->cur_index);
        lv_obj_set_style_text_color(cur_focus_obj, BTN_TEXT_COLOR, 0);
        lv_obj_set_style_bg_opa(cur_focus_obj, LV_OPA_100, 0);
        lv_obj_scroll_to_view(cur_focus_obj, LV_ANIM_OFF);      
        lv_obj_add_state(cur_focus_obj, LV_STATE_USER_1);  
    }
    else if (event == LV_EVENT_RELEASED)
    {        
        lv_group_t* group = lv_obj_get_group(cont);
        if (lv_group_get_editing(group))
        {            
            lv_group_set_editing(group, false);      

            lv_obj_t* cur_focus_obj = lv_obj_get_child(cont, custom_dsc->cur_index);                                    
            lv_obj_set_style_text_color(cur_focus_obj, BTN_TEXT_COLOR, 0);\
            lv_obj_set_style_bg_opa(cur_focus_obj, LV_OPA_100, 0);
                        
            cur_focus_obj = lv_obj_get_child(cont, custom_dsc->cur_index);              
            lv_obj_clear_state(cur_focus_obj, LV_STATE_FOCUSED);
            lv_event_send(cont, LV_EVENT_FOCUSED, NULL);
        }
        else
        {            
            lv_obj_t* src_obj = lv_obj_get_child(cont, custom_dsc->cur_index);
            lv_group_set_editing(group, true);
            
            lv_obj_set_style_bg_opa(src_obj, LV_OPA_100, 0);          
            lv_obj_set_style_text_color(src_obj, BTN_TEXT_COLOR, 0);  
            lv_obj_set_style_bg_color(src_obj, BTN_BG_COLOR, 0);            
            lv_obj_add_state(src_obj, LV_STATE_FOCUSED);            
        }
    }
    else if (event == LV_EVENT_KEY && lv_group_get_editing(indev_group))
     {
        uint32_t key = *(uint32_t*)lv_event_get_param(e);
        lv_obj_t* cur_obj = lv_obj_get_child(cont, custom_dsc->cur_index);
        bool scroll = true;
        if (lv_obj_is_scrolling(cont))  return;

        if (key == LV_ENCODER_KEY_CW)
        {            
            custom_dsc->cur_index += 1;
            if (custom_dsc->cur_index > custom_dsc->max)
            {
                custom_dsc->cur_index = custom_dsc->max;
                scroll = false;
            }                     
            lv_obj_clear_state(cur_obj, LV_STATE_FOCUSED);

            cur_obj = lv_obj_get_child(cont, custom_dsc->cur_index);
            lv_obj_add_state(cur_obj, LV_STATE_FOCUSED);
            lv_event_send(cur_obj, LV_EVENT_FOCUSED, NULL);
        }        
        else if (key == LV_ENCODER_KEY_CC)    
        {    
            custom_dsc->cur_index -= 1;
            if (custom_dsc->cur_index < custom_dsc->min)
            {
                custom_dsc->cur_index = custom_dsc->min;
                scroll = false;
            }                                        
            lv_obj_clear_state(cur_obj, LV_STATE_FOCUSED);
            
            cur_obj = lv_obj_get_child(cont, custom_dsc->cur_index);
            lv_obj_add_state(cur_obj, LV_STATE_FOCUSED);
            lv_event_send(cur_obj, LV_EVENT_FOCUSED, NULL);
        }

        if ((key == LV_ENCODER_KEY_CW || key == LV_ENCODER_KEY_CC) && scroll)
        {
            p_src_model->source.type = (uint8_t)custom_dsc->cur_index;
            cur_obj = lv_obj_get_child(cont, custom_dsc->cur_index);   

            if (p_src_model->source.type >= (custom_dsc->max - 1))
            {
                lv_obj_set_scroll_snap_x(cont, LV_SCROLL_SNAP_NONE); 
            }
            else if (p_src_model->source.type >= 2)
            {
                lv_obj_set_scroll_snap_x(cont, LV_SCROLL_SNAP_CENTER);
            }
            else if (p_src_model->source.type >= 1)
            {
                lv_obj_set_scroll_snap_x(cont, LV_SCROLL_SNAP_NONE); 
            }   
            else                      
            {
                lv_obj_set_scroll_snap_x(cont, LV_SCROLL_SNAP_START); 
            }

            lv_obj_scroll_to_view(cur_obj, LV_ANIM_ON);                     
        }   
        else if (key == LV_KEY_BACKSPACE) 
        {
            lv_event_send(cont, LV_EVENT_RELEASED, NULL);
        }     
     }
}

static void src_btn_event_cb(lv_event_t* e)
{
    lv_obj_t* obj = lv_event_get_target(e);
    lv_event_code_t event = lv_event_get_code(e);

    if (LV_EVENT_FOCUSED == event)
    {
        uint32_t index = lv_obj_get_child_id(obj);
        refresh_xy_btn(index, src_table[index].coord_x, src_table[index].coord_y);            
    }
}

static void src_set_snap_mode(lv_obj_t* cont, uint8_t idx)
{
    custom_cont_t *custom_dsc = lv_obj_get_user_data(cont);
    if (custom_dsc == NULL)
    {
        return ;
    }
    
    if (idx >= (custom_dsc->max - 2))
    {
        lv_obj_set_scroll_snap_x(cont, LV_SCROLL_SNAP_NONE); 
    }
    else if (idx >= 2)
    {
        lv_obj_set_scroll_snap_x(cont, LV_SCROLL_SNAP_CENTER);
    }
    else if (idx >= 1)
    {
        lv_obj_set_scroll_snap_x(cont, LV_SCROLL_SNAP_NONE); 
    }   
    else                      
    {
        lv_obj_set_scroll_snap_x(cont, LV_SCROLL_SNAP_START); 
    }
}

static void src_btn_style_init(lv_obj_t* obj)
{    
    lv_obj_set_style_bg_color(obj, BTN_BG_COLOR, 0);
    lv_obj_set_style_text_color(obj, BTN_TEXT_COLOR, 0);
    lv_obj_set_style_text_font(obj, Font_ResouceGet(FONT_10_MEDIUM), 0);
    lv_obj_set_style_border_width(obj, 0, 0);    
    lv_obj_set_style_shadow_ofs_y(obj, 0, 0);

    lv_obj_set_style_text_font(obj, Font_ResouceGet(FONT_10_BOLD), LV_STATE_FOCUSED);    
    lv_obj_set_style_bg_color(obj, BTN_BG_FOCUSED_COLOR, LV_STATE_FOCUSED);    
    lv_obj_set_style_text_color(obj, BTN_TEXT_FOCUSED_COLOR, LV_STATE_FOCUSED);      
    
    lv_obj_set_style_bg_color(obj, BTN_BG_USER1_COLOR, LV_STATE_USER_1);
    lv_obj_set_style_text_color(obj, lv_color_make(0, 0, 0), LV_STATE_USER_1);   
}

static void int_upd_run_cb(void* data)
{
    adj_btn_t* dsc = (adj_btn_t*)data;
	if(gui_get_hs_mode())
	{
		if(dsc->value == 0 )dsc->value = 0;
		else dsc->value = dsc->value < 200 ? 200 : dsc->value;
	}
    p_src_model->lightness = dsc->value;
    lv_bar_set_value(intensity_bar, p_src_model->lightness, LV_ANIM_OFF);
}

static void x_upd_run_cb(void* data)
{
    adj_btn_t* dsc = (adj_btn_t*)data;
    p_src_model->source.x = dsc->value;
}

static void y_upd_run_cb(void* data)
{
    adj_btn_t* dsc = (adj_btn_t*)data;
    p_src_model->source.y = dsc->value;
}

static bool int_assign_value_cb(void* data)
{
    adj_btn_t* dsc = (adj_btn_t*)data;
    ASSIGN_VALUE_TO_BTN(p_src_model->lightness);
}

static bool x_assign_value_cb(void* data)
{
    adj_btn_t* dsc = (adj_btn_t*)data;
    ASSIGN_VALUE_TO_BTN(p_src_model->source.x);
}

static bool y_assign_value_cb(void* data)
{
    adj_btn_t* dsc = (adj_btn_t*)data;
    ASSIGN_VALUE_TO_BTN(p_src_model->source.y);
}
