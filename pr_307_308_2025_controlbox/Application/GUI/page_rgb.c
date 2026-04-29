/*********************
 *      INCLUDES
 *********************/
#include "ui_common.h"
#include "page_widget.h"

/*********************
 *      DEFINES
 *********************/
#define DISP_PANEL_WIDTH  172
#define DISP_PANEL_HEIGHT 189

#define BTN_CONT_WIDTH      122
#define BTN_CONT_HEIGHT     189

#define PALLETE_WIDTH               132
#define PALLETE_HEIGHT              132

#define COMMON_BTN_BG_COLOR                         lv_color_make(40,40,40)
#define COMMON_BTN_BG_FOCUSD_COLOR                  lv_color_make(230,230,230)
#define COMMON_BTN_FOCUSED_COLOR                    lv_color_make(230,230,230)
#define COMMON_BTN_DEFAULT_COLOR                    lv_color_make(0, 0, 0)
#define COMMON_BTN_TEXT_COLOR                       lv_color_make(255,255,255)
#define COMMON_BTN_FOCUSED_TEXT_COLOR               lv_color_make(0,0,0)
/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void disp_panel_init(lv_obj_t* parent);
static void set_pallet_color(lv_obj_t* pallet, uint16_t _r, uint16_t _g, uint16_t _b);

static void int_upd_run_cb(void* data);
static void g_upd_run_cb(void* data);
static void b_upd_run_cb(void* data);
static void r_upd_run_cb(void* data);

static bool int_assign_value_cb(void* data);
static bool g_assign_value_cb(void* data);
static bool b_assign_value_cb(void* data);
static bool r_assign_value_cb(void* data);

static void page_rgb_construct(void);
static void page_rgb_destruct(void);
static void page_rgb_time_upd_cb(uint32_t ms);
static void page_rgb_event_cb(int event);
/**********************
 *  STATIC VARIABLES
 **********************/
static lv_group_t *indev_group;
static lv_obj_t* intensity_bar;
static lv_obj_t* pallet;
static adj_btn_t btn_list[4] =
{
    ADJ_BTN_INITIALIZER(0, 1000, 1, 500, "I NT", "%", 3, 39, -3, false, false,  intensity_get_value_str, int_upd_run_cb, int_assign_value_cb, NULL, common_intensity_get_step), 
    ADJ_BTN_INITIALIZER(UI_RGB_MIN, UI_RGB_MAX, UI_RGB_STEP, UI_RGB_DEF_VAL, "R", "", 13, 39, -3, false, false,  rgb_get_value_str, r_upd_run_cb, r_assign_value_cb, NULL, NULL), 
    ADJ_BTN_INITIALIZER(UI_RGB_MIN, UI_RGB_MAX, UI_RGB_STEP, UI_RGB_DEF_VAL, "G", "", 13, 39, -3, false, false,  rgb_get_value_str, g_upd_run_cb, g_assign_value_cb, NULL, NULL), 
    ADJ_BTN_INITIALIZER(UI_RGB_MIN, UI_RGB_MAX, UI_RGB_STEP, UI_RGB_DEF_VAL, "B", "", 13, 39, -3, false, false,  rgb_get_value_str, b_upd_run_cb, b_assign_value_cb, NULL, NULL), 
};
static struct db_rgb* p_rgb_model;

/*********************
 *  GLOBAL VATIABLES
 *********************/
page_t page_rgb = PAGE_INITIALIZER("RGB",
                                PAGE_LIGHT_MODE_RGB, 
                                page_rgb_construct, 
                                page_rgb_destruct, 
                                page_rgb_time_upd_cb,
                                page_rgb_event_cb,                                
                                PAGE_NONE);

/**********************
 *   GLOBAL FUNCTIONS
 **********************/
void page_rgb_init(lv_obj_t* parent)
{
	
    p_rgb_model = (struct db_rgb*)screen_get_act_page_data();
    indev_group = lv_group_create();
    page_event_send(EVENT_DATA_PULL);

    my_indev_set_group(indev_group);
    lv_group_set_default(indev_group);    
    lv_obj_set_style_bg_color(parent, lv_color_black(), 0);
    
    lv_obj_t* disp_panel = lv_obj_create(parent);
    lv_obj_add_style(disp_panel, &style_rect_panel, 0);    
    lv_obj_set_width(disp_panel, DISP_PANEL_WIDTH);
    lv_obj_set_height(disp_panel, DISP_PANEL_HEIGHT);
    lv_obj_set_size(disp_panel, DISP_PANEL_WIDTH, DISP_PANEL_HEIGHT);    
    lv_obj_set_style_clip_corner(disp_panel, true, 0);     
    lv_obj_align(disp_panel, LV_ALIGN_LEFT_MID, 10, -2);

    disp_panel_init(disp_panel);
      
    lv_obj_t* btn_cont = lv_obj_create(parent);
    lv_obj_remove_style_all(btn_cont);  
    lv_obj_set_flex_flow(btn_cont, LV_FLEX_FLOW_COLUMN);
    lv_obj_add_style(btn_cont, &style_rect_panel, 0);
    lv_obj_set_style_bg_color(btn_cont, lv_color_make(0, 0, 0), 0);
    lv_obj_set_style_pad_gap(btn_cont, 4, 0);
    lv_obj_set_size(btn_cont, BTN_CONT_WIDTH, BTN_CONT_HEIGHT);
    lv_obj_align_to(btn_cont, disp_panel, LV_ALIGN_OUT_RIGHT_TOP, 8, 0);

    for (uint8_t i = 0; i < 4; i++)
    {
        lv_obj_t* obj = adj_btn_create(btn_cont, LIGHT_MODE_COMMON_RADIUS, ADJ_BTN_TYPE_18);        
        lv_obj_set_user_data(obj, (void *)&btn_list[i]);
        adj_btn_init(obj, &btn_list[i], 90);
        lv_obj_add_event_cb(obj, adj_btn_event_cb, LV_EVENT_ALL, NULL);
        lv_obj_set_size(obj, lv_pct(100), 44);
        lv_group_add_obj(indev_group, obj);
    }

    lv_group_focus_obj(btn_list[0].obj);
    lv_group_set_wrap(indev_group, false);

    adj_btn_refresh_all(btn_list, sizeof(btn_list)/ sizeof(btn_list[0]), true);
	
    page_event_send(EVENT_DATA_COMPULSION);
    uint8_t id = PAGE_LIGHT_MODE_RGB;
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
static void page_rgb_construct(void)
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
    page_rgb_init(b_cont);   
	page_event_send(EVENT_DATA_COMPULSION);
    
}

static void page_rgb_destruct(void)
{
    lv_group_focus_freeze(indev_group, true);
    lv_group_remove_all_objs(indev_group);
    lv_group_del(indev_group);
    indev_group = NULL;

    lv_obj_clean(lv_scr_act());
}

static void page_rgb_time_upd_cb(uint32_t ms)
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
    /* 标题栏更新 */
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

    /* 如果当前正在处于编辑状态，那么退出编辑状态。*/
    if (lv_group_get_editing(indev_group))
    {
        lv_obj_t* fucosed_obj = lv_group_get_focused(indev_group);
        lv_event_send(fucosed_obj, LV_EVENT_RELEASED, NULL);
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
static void page_rgb_event_cb(int event)
{
    struct db_rgb rgb;
    uint32_t src_crc32, dest_crc32;

    data_center_read_light_data(LIGHT_MODE_RGB, &rgb);   
    dest_crc32 = ui_calc_checksum((uint8_t*)&rgb,  sizeof(struct db_rgb));    
    switch (event)
    {
        case EVENT_DATA_WRITE:
        {
            src_crc32 = ui_calc_checksum((uint8_t*)p_rgb_model, sizeof(struct db_rgb));            
            if (dest_crc32 != src_crc32)
            {
                data_center_write_light_data(LIGHT_MODE_RGB, p_rgb_model);
            }
        }
        break;
        case EVENT_DATA_PULL:
        {
            memcpy(p_rgb_model, &rgb, sizeof(struct db_rgb));
            page_set_invalid_type(PAGE_INVALID_TYPE_PORTION);
        }
        break;
		case EVENT_DATA_COMPULSION:
            data_center_write_light_data(LIGHT_MODE_RGB, p_rgb_model);
        break;
    }
}


static void disp_panel_init(lv_obj_t* parent)
{    
    lv_obj_t* bar_cont = lv_obj_create(parent);
    lv_obj_clear_flag(bar_cont, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_style(bar_cont, &style_rect_panel, 0);
    lv_obj_set_style_text_font(bar_cont, Font_ResouceGet(FONT_10), 0);
    lv_obj_set_style_text_color(bar_cont, lv_color_make(255, 255, 255), 0);
    lv_obj_set_size(bar_cont, DISP_PANEL_WIDTH, 29);
    lv_obj_align(bar_cont, LV_ALIGN_TOP_MID, -3, 0);

    intensity_bar = intensity_bar_create(bar_cont, 118, 13, 2, 0);
	if(gui_get_hs_mode())
		p_rgb_model->lightness = p_rgb_model->lightness < 200 ? 200 : p_rgb_model->lightness;
    lv_bar_set_value(intensity_bar, p_rgb_model->lightness, LV_ANIM_OFF);

    pallet = lv_obj_create(parent);
    lv_obj_add_style(pallet, &style_rect_panel, 0);
    lv_obj_set_style_bg_color(pallet, lv_color_make(255, 0, 0), 0);
    lv_obj_set_size(pallet, PALLETE_WIDTH, PALLETE_HEIGHT);
    lv_obj_align_to(pallet, bar_cont,  LV_ALIGN_OUT_BOTTOM_MID, 0, 10);    

#if 0
    // HSI色条
    //lv_obj_align_to(pallet, bar_cont,  LV_ALIGN_OUT_BOTTOM_LEFT, 5, 5);    
    // LV_IMG_DECLARE(ImgRGBColorfulBar)
    // lv_obj_t* img = lv_img_create(parent);
    // lv_img_set_src(img, &ImgRGBColorfulBar);
    // lv_obj_align_to(img, pallet,  LV_ALIGN_OUT_RIGHT_MID, 12, 0);    
#endif    
}

static void set_pallet_color(lv_obj_t* pallet, uint16_t _r, uint16_t _g, uint16_t _b)
{
    uint8_t r = lv_map(_r, 0, UI_RGB_MAX, 0, 255);
    uint8_t g = lv_map(_g, 0, UI_RGB_MAX, 0, 255);
    uint8_t b = lv_map(_b, 0, UI_RGB_MAX, 0, 255);

    lv_obj_set_style_bg_color(pallet, lv_color_make(r, g, b), 0);
}

static void int_upd_run_cb(void* data)
{
    adj_btn_t* dsc = (adj_btn_t*)data;
	if(gui_get_hs_mode())
	{
		if(dsc->value == 0 )dsc->value = 0;
		else dsc->value = dsc->value < 200 ? 200 : dsc->value;
	}
    p_rgb_model->lightness = dsc->value;        
    lv_bar_set_value(intensity_bar, (uint16_t) p_rgb_model->lightness, LV_ANIM_OFF);
}

static void g_upd_run_cb(void* data)
{
    adj_btn_t* dsc = (adj_btn_t*)data;
    p_rgb_model->rgb.g = dsc->value;    
    set_pallet_color(pallet, p_rgb_model->rgb.r, p_rgb_model->rgb.g, p_rgb_model->rgb.b);
}

static void b_upd_run_cb(void* data)
{
    adj_btn_t* dsc = (adj_btn_t*)data;  
    p_rgb_model->rgb.b = dsc->value;  
    set_pallet_color(pallet, p_rgb_model->rgb.r, p_rgb_model->rgb.g,   p_rgb_model->rgb.b );
}

static void r_upd_run_cb(void* data)
{
    adj_btn_t* dsc = (adj_btn_t*)data;
    p_rgb_model->rgb.r = dsc->value;
    set_pallet_color(pallet, p_rgb_model->rgb.r, p_rgb_model->rgb.g, p_rgb_model->rgb.b);
}

static bool int_assign_value_cb(void* data)
{
    adj_btn_t* dsc = (adj_btn_t*)data;
    ASSIGN_VALUE_TO_BTN(p_rgb_model->lightness);
}

static bool g_assign_value_cb(void* data)
{
    adj_btn_t* dsc = (adj_btn_t*)data;
    ASSIGN_VALUE_TO_BTN(p_rgb_model->rgb.g);
}

static bool b_assign_value_cb(void* data)
{
    adj_btn_t* dsc = (adj_btn_t*)data;
    ASSIGN_VALUE_TO_BTN(p_rgb_model->rgb.b);
}

static bool r_assign_value_cb(void* data)
{
    adj_btn_t* dsc = (adj_btn_t*)data;
    ASSIGN_VALUE_TO_BTN(p_rgb_model->rgb.r);
}
