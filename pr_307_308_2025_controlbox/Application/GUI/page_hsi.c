
/*********************
 *      INCLUDES
 *********************/
#include "ui_common.h"
#include "page_widget.h"

/*********************
 *      DEFINES
 *********************/
#define HSI_COLOR_PALETTE_RADIUS                    (134 / 2 - 3)

#define DISP_PANEL_WIDTH                            171
#define DISP_PANEL_HEIGHT                           189

#define BTN_CONT_WIDTH                              122
#define BTN_CONT_HEIGHT                             189

#define COMMON_BTN_BG_COLOR                         lv_color_make(40,40,40)
#define COMMON_BTN_BG_FOCUSD_COLOR                  lv_color_make(230,230,230)
#define COMMON_BTN_FOCUSED_COLOR                    lv_color_make(230,230,230)
#define COMMON_BTN_DEFAULT_COLOR                    lv_color_make(0, 0, 0)
#define COMMON_BTN_TEXT_COLOR                       lv_color_make(255,255,255)
#define COMMON_BTN_FOCUSED_TEXT_COLOR               lv_color_make(0,0,0)
/*********************
 *      TPEDEFS
 *********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

static void disp_panel_init(lv_obj_t* parent);
static void refresh_color_panel(void);

static lv_point_t get_center_selector(lv_point_t center, int16_t radius, int16_t hue, int16_t saturation);

static void int_upd_run_cb(void* data);
static void hue_upd_run_cb(void* data);
static void sat_upd_run_cb(void* data);
static void cct_upd_run_cb(void* data);

static bool int_assign_value_cb(void* data);
static bool hue_assign_value_cb(void* data);
static bool sat_assign_value_cb(void* data);
static bool cct_assign_value_cb(void* data);

static void page_hsi_construct(void);
static void page_hsi_destruct(void);
static void page_hsi_time_upd_cb(uint32_t ms);
static void page_hsi_event_cb(int event);
/*********************
 *  STATIC VATIABLES
 *********************/
static lv_obj_t*   intensity_bar;
static lv_obj_t*   color_selector;
static lv_point_t  img_center_pos;

static lv_group_t *indev_group;

static struct db_hsi* p_hsi_model;
static adj_btn_t btn_list[] = {
    ADJ_BTN_INITIALIZER(0, 1000, 1, 500, "I NT", "%", 3, 41, -2, false, false,  intensity_get_value_str, int_upd_run_cb, int_assign_value_cb, NULL, common_intensity_get_step), 
    ADJ_BTN_INITIALIZER(1, 3600, 1, 100, "HUE", "°", 3, 41, -2, false, true,  adv_hsi_hue_get_value_str, hue_upd_run_cb, hue_assign_value_cb, NULL, adv_hsi_hue_get_step), 
    ADJ_BTN_INITIALIZER(0, 1000, 1, 500, "SAT", "%", 3, 41, -2, false, false, adv_hsi_sat_get_value_str, sat_upd_run_cb, sat_assign_value_cb, NULL, NULL), 
    ADJ_BTN_INITIALIZER(UI_CCT_MIN, UI_CCT_MAX, 50, UI_CCT_MIN, "CCT", "K", 3, 41, -2, false, false,  cct_get_value_str, cct_upd_run_cb, cct_assign_value_cb, NULL, NULL),
};

/*********************
 *  GLOBAL VATIABLES
 *********************/
page_t page_hsi = PAGE_INITIALIZER("Adv.HSI",
                                PAGE_LIGHT_MODE_HSI, 
                                page_hsi_construct, 
                                page_hsi_destruct, 
                                page_hsi_time_upd_cb,
                                page_hsi_event_cb,                                
                                PAGE_NONE);
/*********************
 *  GLOBAL FUNCTIONS
 *********************/
void page_hsi_init(lv_obj_t* parent)
{
    p_hsi_model = (struct db_hsi*)screen_get_act_page_data();
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

    lv_obj_t* btn_panel = lv_obj_create(parent);
    lv_obj_remove_style_all(btn_panel);  
    lv_obj_add_style(btn_panel, &style_rect_panel, 0);
    lv_obj_set_flex_flow(btn_panel, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_pad_row(btn_panel, 4, 0);
    lv_obj_set_style_bg_color(btn_panel, lv_color_make(0, 0, 0), 0);
    lv_obj_set_size(btn_panel, BTN_CONT_WIDTH, BTN_CONT_HEIGHT);
    lv_obj_align_to(btn_panel, disp_panel, LV_ALIGN_OUT_RIGHT_TOP, 8, 0);
    
    lv_obj_t* int_btn = adj_btn_create(btn_panel, LIGHT_MODE_COMMON_RADIUS, ADJ_BTN_TYPE_18);
    lv_obj_set_size(int_btn, lv_pct(100), 44);
    adj_btn_init(int_btn, &btn_list[0], 20);
    lv_obj_set_user_data(int_btn, (void *)&btn_list[0]);
    lv_obj_add_event_cb(int_btn, adj_btn_event_cb, LV_EVENT_ALL, NULL);
    lv_group_add_obj(indev_group, int_btn);

    lv_obj_t* hue_btn = adj_btn_create(btn_panel, LIGHT_MODE_COMMON_RADIUS, ADJ_BTN_TYPE_18);
    lv_obj_set_size(hue_btn, lv_pct(100), 44);
    adj_btn_init(hue_btn, &btn_list[1], 20);
    lv_obj_set_user_data(hue_btn, (void *)&btn_list[1]);
    lv_obj_add_event_cb(hue_btn, adj_btn_event_cb, LV_EVENT_ALL, NULL);    
    lv_group_add_obj(indev_group, hue_btn);
    
    lv_obj_t* sat_btn = adj_btn_create(btn_panel, LIGHT_MODE_COMMON_RADIUS, ADJ_BTN_TYPE_18);
    lv_obj_set_size(sat_btn, lv_pct(100), 44);
    adj_btn_init(sat_btn, &btn_list[2], 20);
    lv_obj_set_user_data(sat_btn, (void *)&btn_list[2]);
    lv_obj_add_event_cb(sat_btn, adj_btn_event_cb, LV_EVENT_ALL, NULL);
    lv_group_add_obj(indev_group, sat_btn);

    lv_obj_t* cct_btn = adj_btn_create(btn_panel, LIGHT_MODE_COMMON_RADIUS, ADJ_BTN_TYPE_18);
    lv_obj_set_size(cct_btn, lv_pct(100), 44);
    adj_btn_init(cct_btn, &btn_list[3], 20);
    lv_obj_set_user_data(cct_btn, (void *)&btn_list[3]);
    lv_obj_add_event_cb(cct_btn, adj_btn_event_cb, LV_EVENT_ALL, NULL);
    lv_group_add_obj(indev_group, cct_btn);

    lv_group_focus_obj(int_btn);
    lv_group_set_wrap(indev_group, false);

    adj_btn_refresh_all(btn_list, sizeof(btn_list)/ sizeof(btn_list[0]), true);
    page_event_send(EVENT_DATA_COMPULSION);
	
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
static void page_hsi_construct(void)
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
    page_hsi_init(b_cont);
    uint8_t id = PAGE_LIGHT_MODE_HSI;
    uint8_t sub_id = 0;
    data_center_write_config_data_no_event(SYS_CONFIG_LOCAL_UI, &id);
    data_center_write_config_data_no_event(SYS_CONFIG_LOCAL_SUB_UI, &sub_id);
	data_center_write_config_data_no_event(SYS_CONFIG_EFFECTS_MODE, &id);
}

static void page_hsi_destruct(void)
{
    lv_group_focus_freeze(indev_group, true);
    lv_group_remove_all_objs(indev_group);
    lv_group_del(indev_group);
    indev_group = NULL;

    lv_obj_clean(lv_scr_act());
}

static void page_hsi_time_upd_cb(uint32_t ms)
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
    /* 标题栏 */
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
    adj_btn_refresh_all(btn_list, sizeof(btn_list)/ sizeof(btn_list[0]), force_upd);
}

/**
 * @brief 事件回调函数
 * 
 * @param[in]event          事件
 *       @EVENT_DATA_WRITE   向数据中心写入
 *       @EVENT_DATA_PULL    从数据中心读出
 */
static void page_hsi_event_cb(int event)
{
    struct db_hsi hsi;
    uint32_t src_crc32, dest_crc32;

    data_center_read_light_data(LIGHT_MODE_HSI, &hsi);   
    dest_crc32 = ui_calc_checksum((uint8_t*)&hsi,  sizeof(struct db_hsi));    
    switch (event)
    {
        case EVENT_DATA_WRITE:
        {
            src_crc32 = ui_calc_checksum((uint8_t*)p_hsi_model, sizeof(struct db_hsi));            
            if (dest_crc32 != src_crc32)
            {
                data_center_write_light_data(LIGHT_MODE_HSI, p_hsi_model);
            }
        }
        break;
        case EVENT_DATA_PULL:
        {
            memcpy(p_hsi_model, &hsi, sizeof(struct db_hsi));
            page_set_invalid_type(PAGE_INVALID_TYPE_PORTION);
        }
        break;
        case EVENT_DATA_COMPULSION:
            data_center_write_light_data(LIGHT_MODE_HSI, p_hsi_model);
        break;
    }
}


static void disp_panel_init(lv_obj_t* cont)
{    
    lv_obj_t* bar_cont = lv_obj_create(cont);
    lv_obj_clear_flag(bar_cont, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_style(bar_cont, &style_rect_panel, 0);
    lv_obj_set_style_text_font(bar_cont, Font_ResouceGet(FONT_10), 0);
    lv_obj_set_style_text_color(bar_cont, lv_color_make(255, 255, 255), 0);
    lv_obj_set_size(bar_cont, DISP_PANEL_WIDTH, 29);
    lv_obj_set_style_radius(bar_cont, 5, 0);
    lv_obj_align(bar_cont, LV_ALIGN_TOP_MID, -3, 0);

	if(gui_get_hs_mode())
		p_hsi_model->lightness = p_hsi_model->lightness < 200 ? 200 : p_hsi_model->lightness;
    intensity_bar = intensity_bar_create(bar_cont, 118, 13, 2, 0);
    lv_bar_set_value(intensity_bar, p_hsi_model->lightness, LV_ANIM_OFF);

    lv_obj_t* color_cont = lv_obj_create(cont);
    lv_obj_clear_flag(color_cont, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_style(color_cont, &style_rect_panel, 0);
    lv_obj_clear_flag(color_cont, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_size(color_cont, DISP_PANEL_WIDTH, DISP_PANEL_HEIGHT - 29);
    lv_obj_align(color_cont, LV_ALIGN_BOTTOM_MID, 0, 0);

    LV_IMG_DECLARE(ImgHSIFigure);
    lv_obj_t* img = lv_img_create(color_cont);
    lv_obj_set_size(img, 147, 147);
    lv_img_set_src(img, &ImgHSIFigure);
    lv_obj_center(img);

    /* 求中心位置 */
    img_center_pos.x = lv_obj_get_style_width(img, 0) / 2;
    img_center_pos.y = lv_obj_get_style_height(img, 0) / 2;

    LV_IMG_DECLARE(ImgHSISelector);
    color_selector = lv_img_create(img);
    lv_img_set_src(color_selector, &ImgHSISelector);
    lv_obj_set_size(color_selector, 9, 9);

    lv_coord_t w = lv_obj_get_style_width(img, 0);
    lv_coord_t h = lv_obj_get_style_height(img, 0);
    lv_point_t center;

    center.x = w / 2 - lv_obj_get_style_width(color_selector, 0) / 2;
    center.y = h / 2 - lv_obj_get_style_width(color_selector, 0) / 2;
    lv_obj_set_pos(color_selector, center.x, center.y);
}

static void int_upd_run_cb(void* data)
{
    adj_btn_t* dsc = (adj_btn_t*)data;
	if(gui_get_hs_mode())
	{
		if(dsc->value == 0 )dsc->value = 0;
		else dsc->value = dsc->value < 200 ? 200 : dsc->value;
	}
    p_hsi_model->lightness = dsc->value;
    lv_bar_set_value(intensity_bar, p_hsi_model->lightness, LV_ANIM_OFF);
}

static void cct_upd_run_cb(void* data)
{
    adj_btn_t* dsc = (adj_btn_t*)data;
    p_hsi_model->hsi.cct = dsc->value;    
}

static void hue_upd_run_cb(void* data)
{
    adj_btn_t* dsc = (adj_btn_t*)data;
    p_hsi_model->hsi.hue = dsc->value;
    refresh_color_panel();
}

static void sat_upd_run_cb(void* data)
{
    adj_btn_t* dsc = (adj_btn_t*)data;
    p_hsi_model->hsi.sat = dsc->value;
    refresh_color_panel();
}

static bool int_assign_value_cb(void* data)
{
    adj_btn_t* dsc = (adj_btn_t*)data;
    ASSIGN_VALUE_TO_BTN(p_hsi_model->lightness);
}

static bool hue_assign_value_cb(void* data)
{
    adj_btn_t* dsc = (adj_btn_t*)data;

    if (p_hsi_model->hsi.hue != dsc->value)
    {
        dsc->value = p_hsi_model->hsi.hue;
        return true;
    }

    return false;
}

static bool sat_assign_value_cb(void* data)
{
    adj_btn_t* dsc = (adj_btn_t*)data;
    ASSIGN_VALUE_TO_BTN(p_hsi_model->hsi.sat);
}

static bool cct_assign_value_cb(void* data)
{
    adj_btn_t* dsc = (adj_btn_t*)data;
    ASSIGN_VALUE_TO_BTN(p_hsi_model->hsi.cct);
}

static void refresh_color_panel(void)
{
    lv_point_t center;
    int16_t radius = HSI_COLOR_PALETTE_RADIUS;

    center = get_center_selector(img_center_pos, radius, p_hsi_model->hsi.hue, p_hsi_model->hsi.sat);  
    UI_PRINTF("Center x:%d Center y: %d\r\n", center.x, center.y);
    center.x -= (lv_obj_get_style_width(color_selector, 0) / 2);
    center.y -= (lv_obj_get_style_height(color_selector, 0)  / 2);
    UI_PRINTF("Real x:%d Real y: %d\r\n", center.x, center.y);    
    lv_obj_set_pos(color_selector, center.x, center.y);  
}

lv_point_t get_center_selector(lv_point_t center, int16_t radius, int16_t hue, int16_t saturation)
{
    lv_point_t pos;
    int16_t inner_radius = radius * saturation / 1000;
    int16_t angle = (3600 - hue + 900); /* 逆时针方向 */

    angle /= 10;
    
    pos.x = center.x + (inner_radius * lv_trigo_sin(angle) >> LV_TRIGO_SHIFT);
    pos.y = center.y - (inner_radius * lv_trigo_cos(angle) >> LV_TRIGO_SHIFT);

    return pos;
}
