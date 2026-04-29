/*********************
 *      INCLUDES
 *********************/
#include "ui_common.h"
#include "page_widget.h"

/*********************
 *      DEFINES
 *********************/
#define DISP_PANEL_WIDTH  171
#define DISP_PANEL_HEIGHT 189

#define BTN_CONT_WIDTH      118
#define BTN_CONT_HEIGHT     189
/*********************
 *      TPEDEFS
 *********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void int_upd_run_cb(void* data);
static void param_x_upd_run_cb(void* data);
static void param_y_upd_run_cb(void* data);
static bool int_assign_value_cb(void* data);

static bool param_x_assign_value_cb(void* data);
static bool param_y_assign_value_cb(void* data);
static void disp_panel_init(lv_obj_t* cont);

static void page_xy_construct(void);
static void page_xy_destruct(void);
static void page_xy_time_upd_cb(uint32_t ms);
static void page_xy_event_cb(int event);
/*********************
 *  STATIC VATIABLES
 *********************/
static struct db_xy* p_xy_model;
static xy_selector_t selector;
static lv_group_t* indev_group;
static lv_obj_t* intensity_bar;
static bool root_mode = false;

static adj_btn_t btn_list[] = {
    ADJ_BTN_INITIALIZER(0, 1000, 1, 500, "I NT", "%", 3, 39, -3,false, false,  intensity_get_value_str, int_upd_run_cb, int_assign_value_cb, NULL, common_intensity_get_step),
    ADJ_BTN_INITIALIZER(0, 8000, 1, 3301, "X", "" , 12, 39, -3,false, false,  xy_get_value_str, param_x_upd_run_cb, param_x_assign_value_cb, NULL, xy_enc_get_step),
    ADJ_BTN_INITIALIZER(0, 8000, 1, 3390, "Y", "" , 12, 39, -3,false, false,  xy_get_value_str, param_y_upd_run_cb, param_y_assign_value_cb, NULL, xy_enc_get_step),    
};

/*********************
 *  GLOBAL VATIABLES
 *********************/
page_t page_xy = PAGE_INITIALIZER("X Y",
                                PAGE_LIGHT_MODE_XY, 
                                page_xy_construct, 
                                page_xy_destruct, 
                                page_xy_time_upd_cb,
                                page_xy_event_cb,                                
                                PAGE_NONE);

/*********************
 *  GLOBAL FUNCTIONS
 *********************/
void page_xy_init(lv_obj_t *parent)
{   
    p_xy_model = (struct db_xy*)screen_get_act_page_data();
    indev_group = lv_group_create();
    page_event_send(EVENT_DATA_PULL);
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
    lv_obj_set_style_pad_row(btn_panel, 8, 0);
    lv_obj_set_style_bg_color(btn_panel, lv_color_make(0, 0, 0), 0);
    lv_obj_set_size(btn_panel, BTN_CONT_WIDTH, BTN_CONT_HEIGHT);
    lv_obj_align_to(btn_panel, disp_panel, LV_ALIGN_OUT_RIGHT_TOP, 8, 0);
	if(gui_get_hs_mode())
		p_xy_model->lightness = p_xy_model->lightness < 200 ? 200 : p_xy_model->lightness;
    lv_obj_t* int_btn = adj_btn_create(btn_panel, LIGHT_MODE_COMMON_RADIUS, ADJ_BTN_TYPE_18);
    lv_obj_set_size(int_btn, lv_pct(100), lv_pct(31));
    adj_btn_init(int_btn, &btn_list[0], 20);
    lv_obj_set_user_data(int_btn, (void *)&btn_list[0]);
    lv_obj_add_event_cb(int_btn, adj_btn_event_cb, LV_EVENT_ALL, NULL);
    lv_group_add_obj(indev_group, int_btn);

    lv_obj_t* x_btn = adj_btn_create(btn_panel, LIGHT_MODE_COMMON_RADIUS, ADJ_BTN_TYPE_18);
    lv_obj_set_size(x_btn, lv_pct(100), lv_pct(31));
    adj_btn_init(x_btn, &btn_list[1], 20);
    lv_obj_set_user_data(x_btn, (void *)&btn_list[1]);
    lv_obj_add_event_cb(x_btn, adj_btn_event_cb, LV_EVENT_ALL, NULL);    
    lv_group_add_obj(indev_group, x_btn);
    
    lv_obj_t* y_btn = adj_btn_create(btn_panel, LIGHT_MODE_COMMON_RADIUS, ADJ_BTN_TYPE_18);
    lv_obj_set_size(y_btn, lv_pct(100), lv_pct(31));
    adj_btn_init(y_btn, &btn_list[2], 20);
    lv_obj_set_user_data(y_btn, (void *)&btn_list[2]);
    lv_obj_add_event_cb(y_btn, adj_btn_event_cb, LV_EVENT_ALL, NULL);

    if (!xy_is_in_triangle(&selector, p_xy_model->xy.x, p_xy_model->xy.y)) 
        xy_set_root_mode(true);

    refresh_xy_selector(&selector, p_xy_model->xy.x, p_xy_model->xy.y);
    lv_group_add_obj(indev_group, y_btn);
    lv_group_focus_obj(int_btn);
    lv_group_set_wrap(indev_group, false);
	
    adj_btn_refresh_all(btn_list, sizeof(btn_list)/ sizeof(btn_list[0]), true);
	
    page_event_send(EVENT_DATA_COMPULSION);
	
	uint8_t id = PAGE_LIGHT_MODE_XY;
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

void xy_set_root_mode(bool mode)
{
    root_mode = mode;
}

bool xy_get_root_mode(void)
{
    return root_mode;
}

/**
 * @brief 计算向量
 * 
 * @param ab 
 * @param a 
 * @param b 
 */
void calc_vec(lv_point_t* ab, lv_point_t* a, lv_point_t* b)
{
    ab->x = b->x - a->x;
    ab->y = b->y - a->y;
}

/**
 * @brief 计算向量积
 * 
 * @param ab 
 * @param am 
 * @return int32_t 
 */
int32_t calc_vec_mutiple(lv_point_t* ab, lv_point_t* am)
{
    int32_t result = ab->x * am->y - ab->y * am->x;
    return result;
}

/**
 * @brief 判断两个点是否在一条直线上的
 * 
 * @param p 
 * @param c 
 * @param a 
 * @param b 
 * @return true 
 * @return false 
 */
bool two_point_is_in_side(lv_point_t* p,
    lv_point_t* c,
    lv_point_t* a,
    lv_point_t* b)
{
    lv_point_t vec_ab, vec_ap, vec_ac;
    int32_t ret_p, ret_c;

    calc_vec(&vec_ab, a, b);
    calc_vec(&vec_ac, a, c);
    calc_vec(&vec_ap, a, p);

    ret_p = calc_vec_mutiple(&vec_ab, &vec_ap);
    ret_c = calc_vec_mutiple(&vec_ab, &vec_ac);

    if ((ret_p > 0 && ret_c > 0) || (ret_p < 0 && ret_c < 0)) {
        return true;
    }
    else {
        return false;
    }
}


/**
 * @brief 判断点在一个三角形内.
 *        
 * @param p 
 * @return true 
 * @return false 
 */
bool is_point_in_triangle(lv_point_t* p)
{
    /*  
     * abc => 上左右 - 三个点
     * 判断m,n两个点在一条ab直线的同一侧
     * a^b - a ^ c 
    */
    lv_point_t a = { 54, 36 };
    lv_point_t b = { 50, 137 };
    lv_point_t c = { 133, 99 };

    bool pc = two_point_is_in_side(p, &c, &a, &b);
    bool pa = two_point_is_in_side(p, &a, &b, &c);
    bool pb = two_point_is_in_side(p, &b, &a, &c);

    UI_PRINTF("pc : %d, pa %d pb %d\r\n", pc, pa, pb);

    if (pc && pa && pb)
    {
        return true;
    }
    else {
        return false;
    }
}

bool xy_is_in_triangle(xy_selector_t* p_selector, uint16_t x, uint16_t y)
{
    const lv_point_t coord_origin = { 25, 148 };
    const lv_point_t coord_x_end = { 152, 148 };
    const lv_point_t coord_y_end = { 23, 27 };
    const lv_img_dsc_t* img = lv_img_get_src(p_selector->obj);
    lv_coord_t x_pos = lv_map(x, 0, 8000, coord_origin.x, coord_x_end.x);
    lv_coord_t y_pos = lv_map(y, 0, 8000, 0, 127);

    (void)coord_y_end;

    y_pos = coord_origin.y - y_pos;
    x_pos = x_pos;

    lv_point_t tmp;
    tmp.x = x_pos;
    tmp.y = y_pos;

     if (is_point_in_triangle(&tmp)) {
        return true;
     }
     else {                
        return false;      
     }
}

void refresh_xy_selector_pos(xy_selector_t* p_selector, uint16_t x, uint16_t y, bool in_triangle)
{
    lv_color_t color = in_triangle ? lv_color_make(82, 68, 74) : lv_color_make(255, 255, 255);
    lv_obj_set_style_img_recolor_opa(p_selector->obj, LV_OPA_100, 0);
    lv_obj_set_style_img_recolor(p_selector->obj, color, 0);
    lv_obj_set_pos(p_selector->obj, x, y);
}

/**
* @brief 刷新xy界面的颜色选择器, 
 * 手动不能拧出去\蓝牙调出去了,进入管理模式，用户可以手动调整回来.
 * 
 * @param p_selector 
 * @param x 
 * @param y 
 */
void refresh_xy_selector(xy_selector_t* p_selector, uint16_t x, uint16_t y)
{
    const lv_point_t coord_origin  = {25, 148};
    const lv_point_t coord_x_end   = {152, 148};
    const lv_point_t coord_y_end   = {23, 27};	
    const lv_img_dsc_t* img = lv_img_get_src(p_selector->obj);
	
	lv_coord_t x_pos = lv_map(x, 0, 8000, coord_origin.x, coord_x_end.x);
    lv_coord_t y_pos = lv_map(y, 0, 8000, 0,  127);
		
    (void)coord_y_end;

    y_pos = coord_origin.y - y_pos;
    x_pos = x_pos;

    lv_point_t tmp;
    tmp.x = x_pos;
    tmp.y = y_pos;	

     if (is_point_in_triangle(&tmp))
     {
        root_mode = false;
        refresh_xy_selector_pos(p_selector, x_pos - img->header.w / 2,  y_pos - img->header.h / 2, true);
        UI_PRINTF("The Point is in this triangle. x: %d y: %d\r\n", x_pos, y_pos);
     }
	 else
	 {		     
		 if (root_mode)
		 {
			 refresh_xy_selector_pos(p_selector, x_pos - img->header.w / 2, y_pos - img->header.h / 2, false);
		 }
		 UI_PRINTF("The Point is not in this triangle, x: %d y: %d\r\n", x_pos, y_pos);    		 
	 }
}

/**
 * @brief 计算交点
 * 
 * @param pa 
 * @param x1 
 * @param y1 
 * @param type 
 */
void calc_intesect_node(lv_point_t* pa, lv_point_t x1, lv_point_t y1, int8_t type)
{
	/*
     *  计算k
	 *  y = kx + b
     */
	float k = (float)(y1.y - x1.y) / (float)(y1.x - x1.x);
	float b = x1.y - k * x1.x;

    /* 计算X = x1, 与两点的交点 */
	if (type == 0) 
	{
		pa->y = (lv_coord_t)(k * pa->x + b);
	}
	else
	{
		pa->x = (lv_coord_t)((pa->y - b) / k);
	}
}

/**
 * @brief 计算绝对距离
 * 
 * @param pa 
 * @param pb 
 * @return int16_t 
 */
int16_t calc_abs_distance(lv_point_t* pa, lv_point_t* pb)
{
    int32_t ax = LV_ABS(pa->x - pb->x);
    int32_t ay = LV_ABS(pa->y - pb->y);
    lv_sqrt_res_t res;
    lv_sqrt(ax * ax + ay * ay, &res, 0x8000);
    return res.i;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/
static void page_xy_construct(void)
{
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
    page_xy_init(b_cont);    
    
    uint8_t id = PAGE_LIGHT_MODE_XY;
    uint8_t sub_id = 0;
    data_center_write_config_data_no_event(SYS_CONFIG_LOCAL_UI, &id);
    data_center_write_config_data_no_event(SYS_CONFIG_LOCAL_SUB_UI, &sub_id);
}

static void page_xy_destruct(void)
{
    lv_group_focus_freeze(indev_group, true);
    lv_group_remove_all_objs(indev_group);
    lv_group_del(indev_group);
    indev_group = NULL;

    lv_obj_clean(lv_scr_act());
}

static void page_xy_time_upd_cb(uint32_t ms)
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

    /* 如果当前正在处于编辑状态，那么退出编辑状态。 */
    if (lv_group_get_editing(indev_group))
    {
        lv_obj_t* fucosed_obj = lv_group_get_focused(indev_group);
        lv_event_send(fucosed_obj, LV_EVENT_RELEASED, NULL);
    }

    /* 亮度 */
    adj_btn_refresh_all(btn_list, 1, force_upd);

    /* x 和 y */
    for (uint8_t i = 1; i < 3; i++)
    {        
        bool update = false;
        if (adj_btn_run_assign_value_cb(&btn_list[i]))
        {            
            root_mode = true;
            update = true;            
        }

        if (update || force_upd)
        {
            if (btn_list[i].obj != NULL)
            {
                lv_event_send(btn_list[i].obj, LV_EVENT_VALUE_CHANGED, NULL);
                lv_obj_invalidate(btn_list[i].obj);
            }
            
            adj_btn_run_update_cb(&btn_list[i]);
        }
    }
}

/**
 * @brief 事件回调函数
 * 
 * @param[in]event          事件
 *       @EVENT_DATA_WRITE   向数据中心写入
 *       @EVENT_DATA_PULL    从数据中心读出
 */
static void page_xy_event_cb(int event)
{
    struct db_xy xy;
    uint32_t src_crc32, dest_crc32;

    data_center_read_light_data(LIGHT_MODE_XY, &xy);   
    dest_crc32 = ui_calc_checksum((uint8_t*)&xy,  sizeof(struct db_xy));    
    switch (event)
    {
        case EVENT_DATA_WRITE:
        {
            src_crc32 = ui_calc_checksum((uint8_t*)p_xy_model, sizeof(struct db_xy));            
            if (dest_crc32 != src_crc32)
            {
                data_center_write_light_data(LIGHT_MODE_XY, p_xy_model);
            }
        }
        break;
        case EVENT_DATA_PULL:
        {
            memcpy(p_xy_model, &xy, sizeof(struct db_xy));
            page_set_invalid_type(PAGE_INVALID_TYPE_PORTION);
        }
        break;
		 case EVENT_DATA_COMPULSION:
            data_center_write_light_data(LIGHT_MODE_XY, p_xy_model);
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
    lv_obj_align(bar_cont, LV_ALIGN_TOP_MID, -3, 0);

    intensity_bar = intensity_bar_create(bar_cont, 118, 13, 2, 0);
    lv_bar_set_value(intensity_bar, p_xy_model->lightness, LV_ANIM_OFF);

    lv_obj_t* color_cont = lv_obj_create(cont);
    lv_obj_clear_flag(color_cont, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_style(color_cont, &style_rect_panel, 0);    
    lv_obj_set_size(color_cont, DISP_PANEL_WIDTH, DISP_PANEL_HEIGHT - 29);
    lv_obj_align(color_cont, LV_ALIGN_BOTTOM_MID, 0, 0);
    
    LV_IMG_DECLARE(ImgXY);
    lv_obj_t* img = lv_img_create(color_cont);    
    lv_img_set_src(img, &ImgXY);        
    lv_obj_center(img);

    LV_IMG_DECLARE(ImgHSISelector);
    selector.obj = lv_img_create(color_cont);
    lv_img_set_src(selector.obj, &ImgHSISelector);
    lv_obj_set_size(selector.obj, 9, 9);
    lv_obj_set_pos(selector.obj, 0, 0);
}

static void int_upd_run_cb(void* data)
{
    adj_btn_t* dsc = (adj_btn_t*)data;
	if(gui_get_hs_mode())
	{
		if(dsc->value == 0 )dsc->value = 0;
		else dsc->value = dsc->value < 200 ? 200 : dsc->value;
	}
	
    p_xy_model->lightness = dsc->value;    
    lv_bar_set_value(intensity_bar, p_xy_model->lightness , LV_ANIM_OFF);
}

static void param_x_upd_run_cb(void* data)
{
    adj_btn_t* dsc = (adj_btn_t*)data;

    if (xy_is_in_triangle(&selector, dsc->value, p_xy_model->xy.y) || root_mode)
    {
        p_xy_model->xy.x = dsc->value;  
        refresh_xy_selector(&selector, p_xy_model->xy.x, p_xy_model->xy.y);
    }
    else
    {
        dsc->value = p_xy_model->xy.x;                
    }
}

static void param_y_upd_run_cb(void* data)
{
    adj_btn_t* dsc = (adj_btn_t*)data;

    if (xy_is_in_triangle(&selector, p_xy_model->xy.x, dsc->value) || root_mode)
    {
        p_xy_model->xy.y = dsc->value;  
        refresh_xy_selector(&selector, p_xy_model->xy.x, p_xy_model->xy.y);
    }
    else
    {
        dsc->value = p_xy_model->xy.y;                
    }
}

static bool int_assign_value_cb(void* data)
{
    adj_btn_t* dsc = (adj_btn_t*)data;
    ASSIGN_VALUE_TO_BTN(p_xy_model->lightness);
}

static bool param_x_assign_value_cb(void* data)
{
    adj_btn_t* dsc = (adj_btn_t*)data;
    ASSIGN_VALUE_TO_BTN(p_xy_model->xy.x);
}

static bool param_y_assign_value_cb(void* data)
{
    adj_btn_t* dsc = (adj_btn_t*)data;
    ASSIGN_VALUE_TO_BTN(p_xy_model->xy.y);
}
