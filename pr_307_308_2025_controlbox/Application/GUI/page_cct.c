/*********************
 *      INCLUDES
 *********************/
#include "ui_common.h"
#include "page_widget.h"
#include "math.h"
#include "stdlib.h"
/*********************
 *      DEFINES
 *********************/

#define DISP_PANEL_HEIGHT 189

#define BTN_CONT_WIDTH      122
#define BTN_CONT_HEIGHT     189

#define COMMON_BTN_BG_COLOR                         lv_color_make(40,40,40)
#define COMMON_BTN_BG_FOCUSD_COLOR                  lv_color_make(230,230,230)
#define COMMON_BTN_FOCUSED_COLOR                    lv_color_make(230,230,230)
#define COMMON_BTN_DEFAULT_COLOR                    lv_color_make(0, 0, 0)
#define COMMON_BTN_TEXT_COLOR                       lv_color_make(255,255,255)
#define COMMON_BTN_FOCUSED_TEXT_COLOR               lv_color_make(0,0,0)

#define CCT_CIRCLE_CENTER_X                 85
#define CCT_CIRCLE_CENTER_Y                 77

#define CCT_INNER_CIRCLE_RADIUS             38
#define CCT_OUTTER_CIRCLE_RADIUS            70

#define CCT_ARC_MAX_RAD                     (73)
#define CCT_ARC_MIN_RAD                     (-74)

/*********************
 *      TPEDEFS
 *********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/


#define DISP_PANEL_WIDTH  172
static void int_upd_run_cb(void* data);
static void cct_upd_run_cb(void* data);
static void gm_upd_run_cb(void* data);

static bool int_assign_value_cb(void* data);
static bool cct_assign_value_cb(void* data);
static bool gm_assign_value_cb(void* data);

static void refresh_scale_img(lv_obj_t* img, int16_t radius, int16_t value1, int16_t min1, int16_t max1);
static void disp_pannel_init(lv_obj_t* cont);

static void page_cct_construct(void);
static void page_cct_destruct(void);
static void page_cct_time_upd_cb(uint32_t ms);
static void page_cct_event_cb(int event);

/*********************
 *  STATIC VATIABLES
 *********************/
static lv_obj_t* label11111;
static struct db_cct* p_cct_model;
static lv_group_t *indev_group;
static lv_obj_t* cct_scale_img;
static lv_obj_t* gm_scale_img;
static lv_obj_t* intensity_bar;
static lv_obj_t *label11111;

static adj_btn_t btn_list[] = {
    ADJ_BTN_INITIALIZER(0, 1000, 1, 500, "I NT", "%", 3, 41, -3, false, false,  intensity_get_value_str, int_upd_run_cb, int_assign_value_cb, NULL, common_intensity_get_step),
    ADJ_BTN_INITIALIZER(UI_CCT_MIN, UI_CCT_MAX, 50, UI_CCT_MIN, "CCT", "K", 3, 41, -3, false, false,  cct_get_value_str, cct_upd_run_cb, cct_assign_value_cb, NULL, NULL),
    ADJ_BTN_INITIALIZER(UI_GM_MIN, UI_GM_MAX, 1, 6, "G/M", "", 3, 41, -3, false, false,  gm_get_value_str, gm_upd_run_cb, gm_assign_value_cb, NULL, NULL),
		
};

/*********************
 *  GLOBAL VATIABLES
 *********************/
page_t page_cct = PAGE_INITIALIZER("CCT",
                                PAGE_LIGHT_MODE_CCT, 
                                page_cct_construct, 
                                page_cct_destruct, 
                                page_cct_time_upd_cb,
                                page_cct_event_cb,
                                PAGE_NONE);
/*********************
 *  GLOBAL FUNCTIONS
 *********************/
void page_cct_init(lv_obj_t *parent)
{
    p_cct_model = (struct db_cct*)screen_get_act_page_data();
    indev_group = lv_group_create();

    page_event_send(EVENT_DATA_PULL);

    my_indev_set_group(indev_group);
    lv_group_set_default(indev_group);
    
    lv_obj_set_style_bg_color(parent, lv_color_make(0, 0, 0), 0);

    lv_obj_t* disp_pannel = lv_obj_create(parent);
    lv_obj_add_style(disp_pannel, &style_rect_panel, 0);
    lv_obj_clear_flag(disp_pannel, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_width(disp_pannel, DISP_PANEL_WIDTH);
    lv_obj_set_height(disp_pannel, DISP_PANEL_HEIGHT);
    lv_obj_set_size(disp_pannel, DISP_PANEL_WIDTH, DISP_PANEL_HEIGHT);    
    lv_obj_align(disp_pannel, LV_ALIGN_LEFT_MID, 10, -2);

    disp_pannel_init(disp_pannel);
    
    lv_obj_t* btn_panel = lv_obj_create(parent);
    lv_obj_remove_style_all(btn_panel);  
    lv_obj_add_style(btn_panel, &style_rect_panel, 0);
    lv_obj_set_flex_flow(btn_panel, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_pad_row(btn_panel, 4, 0);
    lv_obj_set_style_bg_color(btn_panel, lv_color_make(0, 0, 0), 0);
    lv_obj_set_size(btn_panel, BTN_CONT_WIDTH, BTN_CONT_HEIGHT);
    lv_obj_align_to(btn_panel, disp_pannel, LV_ALIGN_OUT_RIGHT_TOP, 8, 0);

    lv_obj_t* int_btn = adj_btn_create(btn_panel, LIGHT_MODE_COMMON_RADIUS, ADJ_BTN_TYPE_18);
    lv_obj_set_size(int_btn, 122, 60);
    adj_btn_init(int_btn, &btn_list[0], 20);
    lv_obj_set_user_data(int_btn, (void *)&btn_list[0]);
    lv_obj_add_event_cb(int_btn, adj_btn_event_cb, LV_EVENT_ALL, NULL);
    lv_group_add_obj(indev_group, int_btn);

    lv_obj_t* cct_btn = adj_btn_create(btn_panel, LIGHT_MODE_COMMON_RADIUS, ADJ_BTN_TYPE_18);
    lv_obj_set_size(cct_btn, 122, 60);
    adj_btn_init(cct_btn, &btn_list[1], 20);
    lv_obj_set_user_data(cct_btn, (void *)&btn_list[1]);
    lv_obj_add_event_cb(cct_btn, adj_btn_event_cb, LV_EVENT_ALL, NULL);    
    lv_group_add_obj(indev_group, cct_btn);
    
    lv_obj_t* gm_btn = adj_btn_create(btn_panel, LIGHT_MODE_COMMON_RADIUS, ADJ_BTN_TYPE_18);
    lv_obj_set_size(gm_btn, 122, 60);
    adj_btn_init(gm_btn, &btn_list[2], 20);
    lv_obj_set_user_data(gm_btn, (void *)&btn_list[2]);
    lv_obj_add_event_cb(gm_btn, adj_btn_event_cb, LV_EVENT_ALL, NULL);
    lv_group_add_obj(indev_group, gm_btn);

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
static void page_cct_construct(void)
{
	uint8_t curre_mode = 0;
    struct sys_info_eth eth;
//    struct sys_info_crmx crmx_sta;
    /* 标题栏 */
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
    lv_obj_set_style_bg_color(lv_scr_act(), lv_color_black(), 0);

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
    page_cct_init(b_cont);
    
    uint8_t id = PAGE_LIGHT_MODE_CCT;
    uint8_t sub_id = 0;
    data_center_write_config_data_no_event(SYS_CONFIG_LOCAL_UI, &id);
    data_center_write_config_data_no_event(SYS_CONFIG_LOCAL_SUB_UI, &sub_id);
	data_center_write_config_data_no_event(SYS_CONFIG_EFFECTS_MODE, &id);
	
//	label11111 = lv_label_create(lv_scr_act());
//	lv_obj_align(label11111, LV_ALIGN_TOP_LEFT, 50, 10);
//	lv_obj_set_style_text_font(label11111, Font_ResouceGet(FONT_18), 0);
//	lv_label_set_text(label11111, "Industrial mode");
//	 lv_obj_set_style_text_color(label11111, lv_color_make(255, 0, 0), 0);

//	lv_snprintf(sdb, sizeof(sdb), "%dW", (long)g_ptUIStateData->test_data.SupplyPower);
//	lv_label_set_text(label11111, sdb);
}

static void page_cct_destruct(void)
{    
    lv_group_focus_freeze(indev_group, true);
    lv_group_remove_all_objs(indev_group);
    lv_group_del(indev_group);

    intensity_bar = NULL;
    indev_group = NULL;

    lv_obj_clean(lv_scr_act());
}

static void page_cct_time_upd_cb(uint32_t ms)
{
    page_invalid_type_t type = page_get_invalid_type();
    bool force_upd = false;
    uint8_t curre_mode = 0;
    struct sys_info_eth eth;
    /* 标题栏 */
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
//	lv_snprintf(sdb, sizeof(sdb), "%dW", (long)g_ptUIStateData->test_data.SupplyPower);
//	lv_label_set_text(label11111, sdb);
    switch(type)
    {
        case PAGE_INVALID_TYPE_IDLE:    return;
		case PAGE_INVALID_TYPE_REFRESH:
			page_set_invalid_type(PAGE_INVALID_TYPE_ALL);
			return;
		break;
        case PAGE_INVALID_TYPE_PORTION:
        case PAGE_INVALID_TYPE_ALL:
        {
			page_set_invalid_type(PAGE_INVALID_TYPE_IDLE);
            force_upd = type == PAGE_INVALID_TYPE_ALL ? true : false;
        }
        break;
    } 
    
    /* 如果当前正在处于编辑状态，那么退出编辑状态. */
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
static void page_cct_event_cb(int event)
{
    struct db_cct cct;
    uint32_t src_crc32, dest_crc32;
	uint8_t type;
	
    data_center_read_light_data(LIGHT_MODE_CCT, &cct);   
    dest_crc32 = ui_calc_checksum((uint8_t*)&cct,  sizeof(struct db_cct));    
    switch (event)
    {
        case EVENT_DATA_WRITE:
        {
            src_crc32 = ui_calc_checksum((uint8_t*)p_cct_model, sizeof(struct db_cct));            
            if (dest_crc32 != src_crc32)
            {
                data_center_write_light_data(LIGHT_MODE_CCT, p_cct_model);
            }
        }
        break;
        case EVENT_DATA_PULL:
        {
			type = page_get_invalid_type();
			if(type == 1)  
				break;
            memcpy(p_cct_model, &cct, sizeof(struct db_cct));
			page_set_invalid_type(PAGE_INVALID_TYPE_PORTION);
        }
        break;
        case EVENT_DATA_COMPULSION:
            data_center_write_light_data(LIGHT_MODE_CCT, p_cct_model);
            break;
    }
}

static void disp_pannel_init(lv_obj_t* cont)
{
    lv_obj_t* bar_cont = lv_obj_create(cont);
    lv_obj_clear_flag(bar_cont, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_style(bar_cont, &style_rect_panel, 0);
    lv_obj_set_style_text_font(bar_cont, Font_ResouceGet(FONT_10), 0);
    lv_obj_set_style_text_color(bar_cont, lv_color_make(255, 255, 255), 0);
    lv_obj_set_size(bar_cont, DISP_PANEL_WIDTH, 15);
    lv_obj_align(bar_cont, LV_ALIGN_TOP_MID, -3, 37);

    intensity_bar = intensity_bar_create(bar_cont, 118, 13, 2, 0);
	if(gui_get_hs_mode())
		p_cct_model->lightness = p_cct_model->lightness < 200 ? 200 : p_cct_model->lightness;

    lv_bar_set_value(intensity_bar, p_cct_model->lightness, LV_ANIM_OFF);

    lv_obj_t* img_pannel = lv_obj_create(cont);    
    lv_obj_remove_style_all(img_pannel);
    lv_obj_set_style_text_font(img_pannel, Font_ResouceGet(FONT_10), 0);
    lv_obj_set_style_text_color(img_pannel, lv_color_white(), 0);
    lv_obj_set_size(img_pannel, lv_pct(100), DISP_PANEL_HEIGHT - 38 - 29);    
    lv_obj_align_to(img_pannel, bar_cont, LV_ALIGN_OUT_BOTTOM_MID, 0, 23);    

    LV_IMG_DECLARE(ImgCCT);
    lv_obj_t* img = lv_img_create(img_pannel);
    lv_img_set_src(img, &ImgCCT);    
    lv_obj_align(img, LV_ALIGN_TOP_MID, 0, 0);    

    LV_IMG_DECLARE(ImgCCTSelector)
    cct_scale_img = lv_img_create(img_pannel);
    lv_img_set_src(cct_scale_img, &ImgCCTSelector);

    gm_scale_img = lv_img_create(img_pannel);
    lv_img_set_src(gm_scale_img, &ImgCCTSelector);

    lv_obj_t* cct_label_min = lv_label_create(img_pannel);    
    lv_label_set_text_fmt(cct_label_min, "%dK", UI_CCT_MIN);
    lv_obj_t* cct_label_max = lv_label_create(img_pannel);    
    lv_label_set_text_fmt(cct_label_max, "%dK", UI_CCT_MAX);
    lv_obj_t* gm_label_min = lv_label_create(img_pannel);
	#if PROJECT_TYPE==307
	lv_label_set_text(gm_label_min, "G/M-1.0");
    lv_obj_t* gm_label_max = lv_label_create(img_pannel);
    lv_label_set_text(gm_label_max, "G/M+1.0");
	#elif PROJECT_TYPE==308
	lv_label_set_text(gm_label_min, "G/M-0.5");
    lv_obj_t* gm_label_max = lv_label_create(img_pannel);
    lv_label_set_text(gm_label_max, "G/M+0.5");
	#endif


    lv_obj_align(cct_label_min, LV_ALIGN_TOP_LEFT, 2, 62);
    lv_obj_align(gm_label_min, LV_ALIGN_TOP_LEFT, 36, 70);
    lv_obj_align(cct_label_max, LV_ALIGN_TOP_RIGHT, 0, 62);
    lv_obj_align(gm_label_max, LV_ALIGN_TOP_RIGHT, -36, 70);

#if 0
    // 测试代码
    // lv_img_set_pivot(img1, 2, 10);
    // static lv_point_t line_points[] = { {85, 77}, {70, 70} };
    // lv_coord_t x =  85 + (CCT_OUTTER_CIRCLE_RADIUS * lv_trigo_sin(74) >> LV_TRIGO_SHIFT);
    // lv_coord_t y = 77 - (CCT_OUTTER_CIRCLE_RADIUS * lv_trigo_cos(74) >> LV_TRIGO_SHIFT);

    // line_points[1].x = x;
    // line_points[1].y = y;

    // lv_obj_t* line = lv_line_create(img_pannel);
    // lv_line_set_points(line, line_points, 2);
    // lv_obj_set_style_line_color(line, lv_color_make(0, 255, 0), 0);
    // lv_obj_set_style_line_width(line, 1, 0);

    // x -= 3/2;
    // y -= 20/2;
    
    // lv_obj_set_pos(img1, x, y);
    // lv_img_set_angle(img1, 740);
    // lv_anim_t a;
    // lv_anim_init(&a);
    // lv_anim_set_var(&a, img1);
    // lv_anim_set_exec_cb(&a, set_angle);
    // lv_anim_set_values(&a, 0, 3600);
    // lv_anim_set_time(&a, 5000);
    // lv_anim_set_repeat_count(&a, LV_ANIM_REPEAT_INFINITE);
    // lv_anim_start(&a);
#endif
}

static void int_upd_run_cb(void* data)
{
    adj_btn_t* dsc = (adj_btn_t*)data;
	if(gui_get_hs_mode())
	{
		if(dsc->value == 0 )dsc->value = 0;
		else dsc->value = dsc->value < 200 ? 200 : dsc->value;
	}
    p_cct_model->lightness = dsc->value;
    if (intensity_bar != NULL)
    {
        lv_bar_set_value(intensity_bar, dsc->value, LV_ANIM_OFF);
    }
//    page_event_send(EVENT_DATA_WRITE);
}

static void cct_upd_run_cb(void* data)
{
    adj_btn_t* dsc = (adj_btn_t*)data;
    p_cct_model->cct.cct = dsc->value;       
    refresh_scale_img(cct_scale_img, CCT_OUTTER_CIRCLE_RADIUS, dsc->value, dsc->min, dsc->max);
//    page_event_send(EVENT_DATA_WRITE);
}

static void gm_upd_run_cb(void* data)
{
    adj_btn_t* dsc = (adj_btn_t*)data;
    p_cct_model->cct.duv = dsc->value;       
    refresh_scale_img(gm_scale_img, CCT_INNER_CIRCLE_RADIUS, dsc->value, dsc->min, dsc->max);
//	page_event_send(EVENT_DATA_WRITE);
}

static bool int_assign_value_cb(void* data)
{
    adj_btn_t* dsc = (adj_btn_t*)data;
    ASSIGN_VALUE_TO_BTN(p_cct_model->lightness);
}

static bool cct_assign_value_cb(void* data)
{
    adj_btn_t* dsc = (adj_btn_t*)data;
    ASSIGN_VALUE_TO_BTN(p_cct_model->cct.cct);
}

static bool gm_assign_value_cb(void* data)
{
    adj_btn_t* dsc = (adj_btn_t*)data;
    ASSIGN_VALUE_TO_BTN(p_cct_model->cct.duv);
}

static void refresh_scale_img(lv_obj_t* img, int16_t radius, int16_t value1, int16_t min1, int16_t max1)
{
    lv_coord_t x, y;
    const lv_img_dsc_t *pdsc;
    int16_t angle;

    if (img == NULL)   
        return ;

    pdsc = (lv_img_dsc_t*)lv_img_get_src(img);

    /* 角度 */
    angle = lv_map(value1, min1, max1, CCT_ARC_MIN_RAD, CCT_ARC_MAX_RAD);

    x = CCT_CIRCLE_CENTER_X + (radius * lv_trigo_sin(angle) >> LV_TRIGO_SHIFT);
    y = CCT_CIRCLE_CENTER_Y - (radius * lv_trigo_cos(angle) >> LV_TRIGO_SHIFT);

    x -= pdsc->header.w / 2;
    y -= pdsc->header.h / 2;

    lv_obj_set_pos(img, x, y);

    /* 当图片rotate 0 - 10°的时候图片会变形 */
    if (angle >= -4 && angle <= 4)
    {
        angle = 0;
    }
    else if (angle > 4 && angle < 10)
    {
        angle = 10;
    }
    else if (angle < -4 && angle > -10)
    {
        angle = -10;
    }
    
    lv_img_set_angle(img, angle * 10);     
    UI_PRINTF("Angle is : %d\r\n", angle);

#if 0
    // static lv_point_t line_points[] = { {85, 77}, {70, 70} };
    // lv_coord_t x =  85 + (CCT_OUTTER_CIRCLE_RADIUS * lv_trigo_sin(74) >> LV_TRIGO_SHIFT);
    // lv_coord_t y = 77 - (CCT_OUTTER_CIRCLE_RADIUS * lv_trigo_cos(74) >> LV_TRIGO_SHIFT);

    // line_points[1].x = x;
    // line_points[1].y = y;

    // lv_obj_t* line = lv_line_create(img_pannel);
    // lv_line_set_points(line, line_points, 2);
    // lv_obj_set_style_line_color(line, lv_color_make(0, 255, 0), 0);
    // lv_obj_set_style_line_width(line, 1, 0);
#endif    
}


//#if PROJECT_TYPE==307
//#define DISP_PANEL_WIDTH  140
//LV_IMG_DECLARE(img_Dcb)
//LV_IMG_DECLARE(img_Dca)
////LV_IMG_DECLARE(ImgCCTSelector)

//static void int_upd_run_cb(void* data);
//static bool int_assign_value_cb(void* data);
//static const char* cct_get_dimmingcurve_str(uint8_t fanmode);
//static void cct_upd_run_cb(void* data);
//static bool cct_assign_value_cb(void* data);
//static void key_cali_cb(lv_event_t* e);
//static uint8_t* int_sn_str_handle(uint8_t *str);

//static void page_cct_construct(void);
//static void page_cct_destruct(void);
//static void page_cct_time_upd_cb(uint32_t ms);
//static void page_cct_event_cb(int event);
//static void refresh_scale_img(lv_obj_t* panel, int16_t value1, int16_t min1, int16_t max1);
//static void int_cct_panel_chage(void);

//static struct db_cct* p_cct_model;
//static lv_group_t *indev_group;
//static lv_obj_t* cct_panel;
//static lv_obj_t* btn_panel;
//static lv_obj_t* int_panel;
//static lv_obj_t* cct_scale_img;
//static arrow_cct_pos_type  s_arrow_cct_pos = {
//    .star_x = 12,
//    .star_y = 92,
//    .middle_x = 68,
//    .middle_y = 40,
//    .end_x = 124,
//    .end_y = 92,
//};
//static bool s_int_cct_chage_flag = false;

//static adj_btn_t btn_list[] = {
//    ADJ_BTN_INITIALIZER(0, 1000, 1, 500, "I NT", "%", 3, 55, -3,false, false,  intensity_get_value_str, int_upd_run_cb, int_assign_value_cb, NULL, common_intensity_get_step),   
//    ADJ_BTN_INITIALIZER(UI_CCT_MIN, UI_CCT_MAX, 50, UI_CCT_MIN, "CCT", "K", 3, 41, -3, false, false,  cct_get_value_str, cct_upd_run_cb, cct_assign_value_cb, NULL, NULL),        
//};
///*********************
// *  GLOBAL VATIABLES
// *********************/
//page_t page_cct = PAGE_INITIALIZER("CCT",
//                                PAGE_LIGHT_MODE_CCT, 
//                                page_cct_construct, 
//                                page_cct_destruct, 
//                                page_cct_time_upd_cb,
//                                page_cct_event_cb,
//                                PAGE_NONE);
///*********************
// *  GLOBAL FUNCTIONS
// *********************/
//void page_cct_init(lv_obj_t *parent)
//{
//    s_int_cct_chage_flag = false;
////    uint8_t sn[7] = {0};
////    uint8_t new_sn[11] = "SN:";
//    uint8_t power_type = 4;
//    uint8_t curre_mode = 0;
//    
//    p_cct_model = (struct db_cct*)screen_get_act_page_data();
//    indev_group = lv_group_create();
//    
//    page_event_send(EVENT_DATA_PULL);

//    my_indev_set_group(indev_group);
//    lv_group_set_default(indev_group);
//    
//    lv_obj_set_style_bg_color(parent, lv_color_make(0, 0, 0), 0);

//    int_panel = lv_obj_create(parent);
//    lv_obj_add_style(int_panel, &style_rect_panel, 0);
//    lv_obj_clear_flag(int_panel, LV_OBJ_FLAG_SCROLLABLE);
//    lv_obj_set_width(int_panel, DISP_PANEL_WIDTH);
//    lv_obj_set_height(int_panel, DISP_PANEL_HEIGHT);
//    lv_obj_set_size(int_panel, DISP_PANEL_WIDTH, DISP_PANEL_HEIGHT);    
//    lv_obj_align(int_panel, LV_ALIGN_LEFT_MID, 10, -2);

//    adj_cct_int_widget_creat(int_panel, 98, 106, p_cct_model->lightness, &btn_list[0]);
//    lv_obj_set_user_data(int_panel, (void *)&btn_list[0]);
//    lv_obj_add_event_cb(int_panel, key_cali_cb, LV_EVENT_ALL, NULL);       
//    lv_group_add_obj(indev_group, int_panel);
//    
//    cct_panel = lv_obj_create(parent);
//    lv_obj_add_style(cct_panel, &style_rect_panel, 0);
//    lv_obj_clear_flag(cct_panel, LV_OBJ_FLAG_SCROLLABLE);
//    lv_obj_set_width(cct_panel, DISP_PANEL_WIDTH);
//    lv_obj_set_height(cct_panel, DISP_PANEL_HEIGHT);
//    lv_obj_set_size(cct_panel, DISP_PANEL_WIDTH, DISP_PANEL_HEIGHT);    
//    lv_obj_align(cct_panel, LV_ALIGN_RIGHT_MID, -10, -2);
//    
//    adj_cct_widget_creat(cct_panel, 256, p_cct_model->cct.cct, &btn_list[1], 1);
//    lv_obj_set_user_data(cct_panel, (void *)&btn_list[1]);
//    refresh_scale_img(cct_panel, p_cct_model->cct.cct, UI_CCT_MIN, UI_CCT_MAX);
//    
//    int_cct_panel_chage();

//    lv_group_focus_obj(int_panel);
//    lv_group_set_wrap(indev_group, false);
//    lv_group_set_editing(indev_group, true);
//    page_event_send(EVENT_DATA_COMPULSION);
//}

///**********************
// *   STATIC FUNCTIONS
// **********************/
//static void page_cct_construct(void)
//{
//    uint8_t curre_mode = 0;
//    
//    struct sys_info_eth eth;
//    
//    data_center_read_config_data(SYS_CONFIG_BLE_CTRL, &curre_mode);
//    ui_set_ble_state(curre_mode);
//    data_center_read_config_data(SYS_CONFIG_CRMX_CTRL, &curre_mode);
//    ui_set_crmx_state(curre_mode);
//    data_center_read_sys_info(SYS_INFO_ETH, &eth);
//    ui_set_lan_state(eth.pluged);
//    data_center_read_config_data(SYS_CONFIG_FAN_MODE, &curre_mode);
//    ui_set_fan_mode(curre_mode);
//    data_center_read_config_data(SYS_CONFIG_CURVE_TYPE, &curre_mode);
//    ui_set_curve_type(curre_mode);
//    
//    lv_obj_set_style_bg_color(lv_scr_act(), lv_color_black(), 0);

//    lv_obj_t* t_cont = lv_obj_create(lv_scr_act());
//    lv_obj_remove_style_all(t_cont);
//    lv_obj_set_size(t_cont, lv_pct(100), lv_pct(13));
//    lv_obj_set_style_text_color(t_cont, lv_color_white(), 0);
//    lv_obj_set_pos(t_cont, 0, 0);

//    title_cont_create(t_cont, screen_get_act_page_name());

//    lv_obj_t* b_cont = lv_obj_create(lv_scr_act());
//    lv_obj_remove_style_all(b_cont);
//    lv_obj_set_size(b_cont, lv_pct(100), lv_pct(87));
//    lv_obj_align_to(b_cont, t_cont, LV_ALIGN_OUT_BOTTOM_MID, 0, 0);
//    page_cct_init(b_cont);
//    
//    uint8_t id = PAGE_LIGHT_MODE_CCT;
//    uint8_t sub_id = 0;
//    data_center_write_config_data_no_event(SYS_CONFIG_LOCAL_UI, &id);
//    data_center_write_config_data_no_event(SYS_CONFIG_LOCAL_SUB_UI, &sub_id);
//}

//static void page_cct_destruct(void)
//{    
//    lv_group_focus_freeze(indev_group, true);
//    lv_group_remove_all_objs(indev_group);
//    lv_group_del(indev_group);

//    indev_group = NULL;

//    lv_obj_clean(lv_scr_act());
//}

//static void page_cct_time_upd_cb(uint32_t ms)
//{
//    page_invalid_type_t type = page_get_invalid_type();
//    bool force_upd = false;
//    uint8_t curre_mode = 0;
//    struct sys_info_eth eth;
//    struct sys_info_crmx crmx_sta;
//    /* 标题栏 */
//    data_center_read_config_data(SYS_CONFIG_BLE_CTRL, &curre_mode);
//    ui_set_ble_state(curre_mode);
//    data_center_read_config_data(SYS_CONFIG_CRMX_CTRL, &curre_mode);
//    ui_set_crmx_state(curre_mode);
//    data_center_read_sys_info(SYS_INFO_ETH, &eth);
//    ui_set_lan_state(eth.pluged);
//    data_center_read_config_data(SYS_CONFIG_FAN_MODE, &curre_mode);
//    ui_set_fan_mode(curre_mode);
//    data_center_read_config_data(SYS_CONFIG_CURVE_TYPE, &curre_mode);
//    ui_set_curve_type(curre_mode);
//    lv_obj_t* t_cont = lv_obj_get_child(lv_scr_act(), 0);
//    title_update_handler(t_cont);
////    page_event_send(EVENT_DATA_WRITE);
//    
//    if(type == PAGE_INVALID_TYPE_ALL)
//    {
//            struct db_cct cct;
////        case PAGE_INVALID_TYPE_IDLE:    return;
////        case PAGE_INVALID_TYPE_PORTION:
////        case PAGE_INVALID_TYPE_ALL:
////        {
////            page_set_invalid_type(PAGE_INVALID_TYPE_IDLE);
//            force_upd = (type == PAGE_INVALID_TYPE_ALL) ? true : false;
//            data_center_read_light_data(LIGHT_MODE_CCT, &cct); 
//            memcpy(p_cct_model, &cct, sizeof(cct));
//            page_set_invalid_type(PAGE_INVALID_TYPE_IDLE);
//            page_event_send(EVENT_DATA_WRITE);
////        }
////        break;
//    }   
//    adj_btn_refresh_all(btn_list, sizeof(btn_list)/ sizeof(btn_list[0]), force_upd);
//}


///**
// * @brief 事件回调函数
// * 
// * @param[in]event          事件
// *       @EVENT_DATA_WRITE   向数据中心写入
// *       @EVENT_DATA_PULL    从数据中心读出
// */
//static void page_cct_event_cb(int event)
//{
//    struct db_cct cct;
//    uint32_t src_crc32, dest_crc32;
//    
////    data_center_read_light_data(LIGHT_MODE_CCT, &cct);   
////    dest_crc32 = ui_calc_checksum((uint8_t*)&cct,  sizeof(struct db_cct));  
//    
//    switch (event)
//    {
//        case EVENT_DATA_WRITE:
//        {
////            src_crc32 = ui_calc_checksum((uint8_t*)p_cct_model, sizeof(struct db_cct));            
////            if (dest_crc32 != src_crc32)
////            {
//                data_center_write_light_data(LIGHT_MODE_CCT, p_cct_model);
////            }
//        }
//        break;
//        case EVENT_DATA_PULL:
//        {
//            data_center_read_light_data(LIGHT_MODE_CCT, &cct);  
//            memcpy(p_cct_model, &cct, sizeof(struct db_cct));
//        }
//        break;
//        case EVENT_DATA_COMPULSION:
//            data_center_write_light_data(LIGHT_MODE_CCT, p_cct_model);
//        break;
//    }
//}

//static void int_upd_run_cb(void* data)
//{
//    const char *int_str = NULL;
//    uint16_t src_int = 0;
//    
//    adj_btn_t* dsc = (adj_btn_t*)data;
//    p_cct_model->lightness = dsc->value;
//    src_int = dsc->value;
//    if(src_int != 0 && src_int <= 3)
//        src_int = 3;
//    else if(src_int != 1000 && src_int >= 985)
//        src_int = 985;
//    int_str = dsc->get_value_str_cb(dsc);
//    lv_obj_t* arc_panel = lv_obj_get_child(int_panel, 0);
//    lv_obj_t* arc = lv_obj_get_child(arc_panel, 0);
//    lv_arc_set_value(arc, src_int);
//    lv_obj_t* label = lv_obj_get_child(int_panel, 1);
//    lv_label_set_text(label, int_str);
//    page_event_send(EVENT_DATA_WRITE);
//}

//static bool int_assign_value_cb(void* data)
//{
//    adj_btn_t* dsc = (adj_btn_t*)data;
//    ASSIGN_VALUE_TO_BTN(p_cct_model->lightness);
//}

//static uint8_t* int_sn_str_handle(uint8_t *str)
//{
//    while(*str != '\0')
//    {
//        if(*str >= 'a' && *str <= 'z')
//        {
//            *str -= 32;
//        }
//        str++;
//    }
//    return str;
//}

//static void cct_upd_run_cb(void* data)
//{
//    adj_btn_t* dsc = (adj_btn_t*)data;
//    p_cct_model->cct.cct = dsc->value;       
//    lv_obj_t* cct_label = lv_obj_get_child(cct_panel, 2);
//    const char* int_str =  cct_get_value_str(data);
//    lv_label_set_text(cct_label, int_str);
//    
//    refresh_scale_img(cct_scale_img, dsc->value, dsc->min, dsc->max);
//    page_event_send(EVENT_DATA_WRITE);
//}

//static bool cct_assign_value_cb(void* data)
//{
//    adj_btn_t* dsc = (adj_btn_t*)data;
//    ASSIGN_VALUE_TO_BTN(p_cct_model->cct.cct);
//}

//static const char* cct_get_dimmingcurve_str(uint8_t fanmode)
//{
//    const char* str = NULL;
//    switch(fanmode)
//    {
//        case CURVE_TYPE_LINEAR:     str = Lang_GetStringByID(STRING_ID_CURVE_LINEAR); break;
//        case CURVE_TYPE_EXP:        str = Lang_GetStringByID(STRING_ID_CURVE_EXP); break;
//        case CURVE_TYPE_S_SHAPE:    str = Lang_GetStringByID(STRING_ID_CURVE_S_CURVE); break;
//        case CURVE_TYPE_LOG:        str = Lang_GetStringByID(STRING_ID_CURVE_LOG); break;
//    }
//    return str;
//}

//static void int_cct_panel_chage(void)
//{
//    if(s_int_cct_chage_flag == false)
//    {
//        lv_obj_t* arc_panel = lv_obj_get_child(int_panel, 0);
//        lv_obj_t* arc = lv_obj_get_child(arc_panel, 0);
//        lv_obj_set_style_bg_color(arc, CCT_RED_THEME_COLOR, LV_PART_MAIN);
//        lv_obj_set_style_arc_color(arc, CCT_RED_THEME_COLOR, LV_PART_MAIN);
//        
//        lv_obj_set_style_bg_color(int_panel, RED_THEME_COLOR, LV_PART_MAIN);
//        
//        lv_obj_set_style_bg_color(cct_panel, lv_color_make(40,40,40), LV_PART_MAIN);
//    }
//    else if(s_int_cct_chage_flag == true)
//    {
//        lv_obj_t* arc_panel = lv_obj_get_child(int_panel, 0);
//        lv_obj_t* arc = lv_obj_get_child(arc_panel, 0);
//        lv_obj_set_style_bg_color(arc, lv_color_make(0, 0, 0), LV_PART_MAIN);
//        lv_obj_set_style_arc_color(arc, lv_color_make(0, 0, 0), LV_PART_MAIN);
//        
//        lv_obj_set_style_bg_color(int_panel, lv_color_make(40,40,40), LV_PART_MAIN);
//        
//        lv_obj_set_style_bg_color(cct_panel, RED_THEME_COLOR, LV_PART_MAIN);
//    }
//}

//static void refresh_scale_img(lv_obj_t* panel, int16_t value1, int16_t min1, int16_t max1)
//{
//    static uint8_t crc_value = 2;
//    uint8_t cct_count = 0;
//    uint16_t cct_angle = 0;
//    int16_t cct_x = 0;
//    int16_t cct_y = 0;
//    
//    cct_count = (value1 - min1)/100;
//    lv_obj_t* img_panel = lv_obj_get_child(cct_panel, 1);
//    if(value1==6050)
//        cct_count = 41;
//    if(cct_count == 0)
//    {
//        lv_img_set_angle(img_panel, 900);
//        lv_obj_set_pos(img_panel, s_arrow_cct_pos.star_x, s_arrow_cct_pos.star_y);
//    }
//    else if(cct_count > 0 && cct_count < 40)
//    {
//        cct_angle = 900+(cct_count*22.5);
//        lv_img_set_angle(img_panel, cct_angle);
//        cct_x = 53.0 * cos((((float)cct_count * 2.25) * 3.14) / 180.0);
//        cct_x = 12 + (53.0 - abs(cct_x));
//        cct_y = 53.0 * sin((((float)cct_count * 2.25) * 3.14) / 180.0);   //得出对边
//        if(cct_count < 23)
//            cct_y = 92 - abs(cct_y);   //算出y坐标   +crc_value是为了校准
//        else if(cct_count >= 23)
//            cct_y = 92 - abs(cct_y)+crc_value;   //算出y坐标   +1是为了校准
//        lv_obj_set_pos(img_panel, cct_x, cct_y);
//    }
//    else if(cct_count == 40)
//    {
//        lv_img_set_angle(img_panel, 1800);
//        lv_obj_set_pos(img_panel, s_arrow_cct_pos.middle_x, s_arrow_cct_pos.middle_y);
//    }
//    else if(cct_count > 40 && cct_count < 80)
//    {
//            cct_angle = 1800+((cct_count - 40)*22.5);
//            lv_img_set_angle(img_panel, cct_angle);
//            cct_x = 55.0 * sin((((cct_count - 40) * 2.25) * 3.14) / 180);
//            if(cct_count < 65)
//                cct_x = 68 + abs(cct_x);   //算出x坐标   
//            else if(cct_count >= 65)
//                cct_x = 68 + abs(cct_x)+crc_value;   //   +crc_value是为了校准
//            cct_y = 55.0 * cos((((cct_count - 40) * 2.25) * 3.14) / 180);   //得出邻边
//            cct_y = 40 + (55.0 - abs(cct_y));   //算出y坐标
//            lv_obj_set_pos(img_panel, cct_x, cct_y);
//    }
//    else if(cct_count == 80)
//    {
//        lv_img_set_angle(img_panel, 2700);
//        lv_obj_set_pos(img_panel, s_arrow_cct_pos.end_x, s_arrow_cct_pos.end_y);
//    }
//}

//static void key_cali_cb(lv_event_t* e)
//{
//    lv_obj_t* obj = lv_event_get_target(e);
//    lv_event_code_t event = lv_event_get_code(e);

//    shortcut_keys_event_handler(e);
//    backspace_key_event_handler(e);
//    
//    if (event == LV_EVENT_KEY)
//    {
//        adj_btn_t* user_data;
//        lv_indev_type_t indev_type = lv_indev_get_type(lv_indev_get_act());
//        uint32_t key = *(uint32_t *)lv_event_get_param(e);
//        if(s_int_cct_chage_flag == false)
//            user_data = lv_obj_get_user_data(int_panel);
//        else 
//            user_data = lv_obj_get_user_data(cct_panel);
//         int32_t (*p_fn_enc_get_step)(uint32_t value) = user_data->get_step_cb == NULL ? comm_enc_get_step : user_data->get_step_cb;
//        
//        if (indev_type == LV_INDEV_TYPE_ENCODER)
//        {
//            if (LV_ENCODER_KEY_CC == key)
//            {                
//                uint16_t step = user_data->step * p_fn_enc_get_step(enc_get_step_index());
//                int16_t value = user_data->value - step;
//                
//                if (value < user_data->min)
//                {
//                    if (user_data->cycle) {
//                        value = user_data->max;
//                    }
//                    else {
//                        value = user_data->min;
//                    }
//                }

//                if (value != user_data->value)
//                {
//                    user_data->value = value;
//                    if (user_data->update_run_cb)
//                    {
//                        user_data->update_run_cb(user_data); 
//                        user_data->assign_value_cb(user_data);
//                    }
//                }            
//            }
//            else if (LV_ENCODER_KEY_CW == key)
//            {             
//                uint16_t step = user_data->step * p_fn_enc_get_step(enc_get_step_index());                 
//                int16_t value = user_data->value + step;

//                if (value > user_data->max)
//                {
//                    if (user_data->cycle) {
//                        value = user_data->min;
//                    }
//                    else {
//                        value = user_data->max;
//                    }
//                }

//                if (value != user_data->value)
//                {
//                    user_data->value = value;
//                    if (user_data->update_run_cb)
//                    {
//                        user_data->update_run_cb(user_data); 
//                        user_data->assign_value_cb(user_data);
//                    }
//                }
//            }
//        }
//    }
//    else if(event == LV_EVENT_PRESSED)
//    {
//        s_int_cct_chage_flag = !s_int_cct_chage_flag;
//        int_cct_panel_chage();
//    }
//}

//#endif
