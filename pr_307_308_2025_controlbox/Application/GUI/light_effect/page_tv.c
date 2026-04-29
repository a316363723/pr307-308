/*********************
 *      INCLUDES
 *********************/
#include "../ui_common.h"
#include "../page_widget.h"
#if (PROJECT_TYPE == 307)
/*********************
 *      DEFINES
 *********************/
#define DIVIDE_SPACE            3

#define INT_BTN_WIDTH           136
#define INT_BTN_HEIGHT          53

#define CCT_RANGE_BTN_WIDTH     301
#define CCT_RANGE_BTN_HEIGHT    64

#define SPD_BTN_WIDTH           136
#define SPD_BTN_HEIGHT          74

#define STATE_BTN_WIDTH         162
#define STATE_BTN_HEIGHT        74
/*********************
 *      TPEDEFS
 *********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void int_upd_run_cb(void* data);
static void spd_upd_run_cb(void* data);
static bool int_assign_value_cb(void* data);;
static bool spd_assign_value_cb(void* data);

static void ctrl_btn_update_cb(void *data);
static void btnlist_update_cb(void *data);
static void btnlist_release_cb(void* data);

static void page_tv_construct(void);
static void page_tv_destruct(void);
static void page_tv_time_upd_cb(uint32_t ms);
static void page_tv_event_cb(int event);
/*********************
 *  STATIC VATIABLES
 *********************/
static adj_btn_t btn_list[] = {
    ADJ_BTN_INITIALIZER(0, 1000, 1, 500, "I NT", "%", 9, 59, -6, false, false,  intensity_get_value_str, int_upd_run_cb, int_assign_value_cb, NULL, common_intensity_get_step),            
    ADJ_BTN_INITIALIZER(SPEED_LEVEL_1, SPEED_LEVEL_R, 1, SPEED_LEVEL_1, "SPD", "", 9, 59, -6, false, false,  speed_get_value_str, spd_upd_run_cb, spd_assign_value_cb, NULL, NULL), 
};

static lv_group_t *indev_group;
static struct db_fx_tv_2  *p_tv_model;
static state_btn_t state_btn_dsc;
static my_btnlist_ext_t btnlist_ext;
static lv_obj_t* disp_panel;
/*********************
 *  GLOBAL VATIABLES
 *********************/
page_t page_tv = PAGE_INITIALIZER("TV",
                                PAGE_TV, 
                                page_tv_construct, 
                                page_tv_destruct, 
                                page_tv_time_upd_cb,
                                page_tv_event_cb,                                
                                PAGE_LIGHT_EFFECT_MENU);
/*********************
 *  GLOBAL FUNCTIONS
 *********************/
void page_tv_init(lv_obj_t* parent)
{
	uint8_t hsmode;
	data_center_read_config_data(SYS_CONFIG_HIGH_SPEED_MODE, &hsmode);
	if(hsmode)
	{
		g_tUIAllData.tv2_model.state = 0;
	}
    p_tv_model = (struct db_fx_tv_2 *)screen_get_act_page_data();
    indev_group = lv_group_create();
    page_event_send(EVENT_DATA_PULL);

    my_indev_set_group(indev_group);
    lv_group_set_default(indev_group);
    lv_obj_set_style_bg_color(parent, lv_color_black(), 0);
    
    //亮度
    lv_obj_t* int_cont = adj_btn_create(parent, LIGHT_EFFECT_COMMON_RADIUS, ADJ_BTN_TYPE_18);
    lv_obj_set_user_data(int_cont, (void *)&btn_list[0]);
    adj_btn_init(int_cont, &btn_list[0], 90);
    lv_obj_add_event_cb(int_cont, adj_btn_event_cb, LV_EVENT_ALL, NULL);    
    lv_obj_set_size(int_cont, INT_BTN_WIDTH, INT_BTN_HEIGHT);
    lv_obj_align(int_cont, LV_ALIGN_TOP_LEFT, 10, 2);
    lv_group_add_obj(indev_group, int_cont);

    static const char* list_str_map[] = {"CCT", "Adv.HSI"};

    btnlist_ext.cur_index = p_tv_model->mode;
    btnlist_ext.max = 1;
    btnlist_ext.min = 0;
    btnlist_ext.release_cb = btnlist_release_cb;
    btnlist_ext.update_cb = btnlist_update_cb;

    lv_obj_t* btnlist_obj = btnlist_create(parent, 136, 63, &btnlist_ext, list_str_map);
    lv_obj_align_to(btnlist_obj, int_cont, LV_ALIGN_OUT_BOTTOM_MID, 0, 4);

    lv_obj_t* disp_frame;
    disp_panel = lightmode_disp_panel_create(parent, 162, 120);
    disp_frame = lv_obj_get_parent(disp_panel);
    lv_obj_align_to(disp_frame, int_cont, LV_ALIGN_OUT_RIGHT_TOP, 3, 0);    
    lightmode_disp_panel_limit_setting_init(disp_panel, p_tv_model->mode, &p_tv_model->mode_arg);    

    btnlist_ext.obj = btnlist_obj;
    btnlist_ext.disp_panel = disp_panel;
    btnlist_ext.setting.limit = &p_tv_model->mode_arg;
    lv_obj_set_user_data(btnlist_obj, &btnlist_ext);
    lv_obj_add_event_cb(btnlist_obj,  btnlist_event_cb, LV_EVENT_ALL, NULL);
    lv_group_add_obj(indev_group, btnlist_obj);

    //速度
    btn_list[1].name = Lang_GetStringByID(STRING_ID_SPD);
    lv_obj_t* spd_cont = adj_btn_create(parent, LIGHT_EFFECT_COMMON_RADIUS, ADJ_BTN_TYPE_18);
    lv_obj_set_user_data(spd_cont, (void *)&btn_list[1]);
    adj_btn_init(spd_cont, &btn_list[1], 90);    
    lv_obj_add_event_cb(spd_cont, adj_btn_event_cb, LV_EVENT_ALL, NULL);    
    lv_obj_set_size(spd_cont, SPD_BTN_WIDTH, SPD_BTN_HEIGHT);
    lv_obj_align_to(spd_cont, btnlist_obj, LV_ALIGN_OUT_BOTTOM_LEFT, 0, DIVIDE_SPACE);
    lv_group_add_obj(indev_group, spd_cont);

    LV_IMG_DECLARE(ImgLoopIcon);
    LV_IMG_DECLARE(ImgStopIcon);

    static const char* str_map[] = {"LOOP", "STOP"};
    static const lv_img_dsc_t* img_map[] = {&ImgLoopIcon, &ImgStopIcon};

    str_map[0] = Lang_GetStringByID(STRING_ID_STOP);
    str_map[1] = Lang_GetStringByID(STRING_ID_LOOP);

    img_map[0] = &ImgStopIcon;
    img_map[1] = &ImgLoopIcon;

    state_btn_dsc.cur_index = 0;
    state_btn_dsc.min = 0;
    state_btn_dsc.max = 1;
    state_btn_dsc.img_map = img_map;
    state_btn_dsc.map = str_map;
    if (p_tv_model->state > state_btn_dsc.max)
    {
        p_tv_model->state = state_btn_dsc.max;
    }

    state_btn_dsc.cur_index = p_tv_model->state;
    state_btn_dsc.update_cb = ctrl_btn_update_cb;
    state_btn_dsc.widget_refresh_cb = state_btn_refresh;
    state_btn_dsc.img_to_top = 38;
    state_btn_dsc.img_gap = 3;
    state_btn_dsc.title_to_top = 7;

    lv_obj_t* state_btn = state_btn_create(parent, &state_btn_dsc, STATE_BTN_WIDTH, STATE_BTN_HEIGHT);
    lv_obj_add_event_cb(state_btn, state_btn_event, LV_EVENT_ALL, NULL);    
    lv_obj_align_to(state_btn, spd_cont, LV_ALIGN_OUT_RIGHT_TOP, DIVIDE_SPACE, 0);
    lv_event_send(state_btn, LV_EVENT_DEFOCUSED, NULL);
    lv_group_add_obj(indev_group, state_btn);
    lv_group_set_wrap(indev_group, false);

    lv_group_user_focus_obj_by_id(indev_group, screen_get_act_index());
    screen_set_act_index(0);

    adj_btn_refresh_all(btn_list, sizeof(btn_list)/ sizeof(btn_list[0]), true);
	page_event_send(EVENT_DATA_COMPULSION);
}
/**********************
 *   STATIC FUNCTIONS
 **********************/
static void page_tv_construct(void)
{
    lv_obj_set_style_bg_color(lv_scr_act(), lv_color_black(), 0);

    lv_obj_t* t_cont = lv_obj_create(lv_scr_act());
    lv_obj_remove_style_all(t_cont);
    lv_obj_set_size(t_cont, lv_pct(100), lv_pct(12));
    lv_obj_set_style_text_color(t_cont, lv_color_white(), 0);
    lv_obj_set_pos(t_cont, 0, 0);

    light_effect_title_create(t_cont, Lang_GetStringByID(STRING_ID_TV));

    lv_obj_t* b_cont = lv_obj_create(lv_scr_act());
    lv_obj_remove_style_all(b_cont);
    lv_obj_set_size(b_cont, lv_pct(100), lv_pct(88));
    lv_obj_align_to(b_cont, t_cont, LV_ALIGN_OUT_BOTTOM_MID, 0, 0);
    page_tv_init(b_cont);
    
    uint8_t id = PAGE_TV;
    uint8_t sub_id = 0;
    data_center_write_config_data(SYS_CONFIG_LOCAL_UI, &id);
    data_center_write_config_data(SYS_CONFIG_LOCAL_SUB_UI, &sub_id);
	data_center_write_config_data_no_event(SYS_CONFIG_EFFECTS_MODE, &id);
}

static void page_tv_destruct(void)
{
    lv_group_focus_freeze(indev_group, true);
    lv_group_remove_all_objs(indev_group);
    lv_group_del(indev_group);
    indev_group = NULL;

    lv_obj_clean(lv_scr_act());
}

static void page_tv_time_upd_cb(uint32_t ms)
{
    page_invalid_type_t type = page_get_invalid_type();
    bool force_upd = false;
    
    //update the title bar.
    lv_obj_t* t_cont = lv_obj_get_child(lv_scr_act(), 0);
    (void)t_cont;
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

    lv_obj_t* fucosed_obj = lv_group_get_focused(indev_group);    
    if (lv_group_get_editing(indev_group)) //如果当前正在处于编辑状态，那么退出编辑状态。
    {
        lv_event_send(fucosed_obj, LV_EVENT_RELEASED, NULL);
    }

    if (state_btn_dsc.cur_index != p_tv_model->state || force_upd)    //更新触发按键
    {
        bool focused = fucosed_obj == state_btn_dsc.btn ? true : false;      
        if (p_tv_model->state > state_btn_dsc.max)
        {
            p_tv_model->state = state_btn_dsc.max;
        }
                
        state_btn_dsc.cur_index = p_tv_model->state;
        state_btn_dsc.widget_refresh_cb(state_btn_dsc.btn, focused, false);
    }

    if (btnlist_ext.cur_index != p_tv_model->mode || force_upd)
    {
        btnlist_refresh(btnlist_ext.obj, p_tv_model->mode);
    }
	lightmode_disp_panel_limit_setting_init(btnlist_ext.disp_panel, p_tv_model->mode,&p_tv_model->mode_arg);
    adj_btn_refresh_all(btn_list, sizeof(btn_list)/ sizeof(btn_list[0]), force_upd); 
}

/**
 * @brief 事件回调函数
 * 
 * @param[in]event          事件
 *       @EVENT_DATA_WRITE   向数据中心写入
 *       @EVENT_DATA_PULL    从数据中心读出
 */
static void page_tv_event_cb(int event)
{
    struct db_fx_tv_2 tv;
    uint32_t src_crc32, dest_crc32;
	uint8_t  hsmode ;
	data_center_read_config_data(SYS_CONFIG_HIGH_SPEED_MODE, &hsmode);
    data_center_read_light_data(LIGHT_MODE_FX_TV_II, &tv);   
    dest_crc32 = ui_calc_checksum((uint8_t*)&tv,  sizeof(struct db_fx_tv_2));    
    switch (event)
    {
        case EVENT_DATA_WRITE:
        {
            src_crc32 = ui_calc_checksum((uint8_t*)p_tv_model, sizeof(struct db_fx_tv_2));            
            if (dest_crc32 != src_crc32)
            {
				if(hsmode == 0)data_center_write_light_data(LIGHT_MODE_FX_TV_II, p_tv_model);
            }
        }
        break;
        case EVENT_DATA_PULL:
        {
            memcpy(p_tv_model, &tv, sizeof(struct db_fx_tv_2));
            page_set_invalid_type(PAGE_INVALID_TYPE_PORTION);
        }
        break;
		case EVENT_DATA_COMPULSION:
            data_center_write_light_data(LIGHT_MODE_FX_TV_II, p_tv_model);
            break;
		case EVENT_DATA_HS_WINDOW:
		{
			bool force_upd = 0;
			memcpy(p_tv_model, &tv, sizeof(struct db_fx_tv_2)); 
			state_btn_dsc.cur_index = 0; 
			state_btn_dsc.widget_refresh_cb(state_btn_dsc.btn, force_upd, false);
			break;
		}
    }
}

static void int_upd_run_cb(void* data)
{
    adj_btn_t* dsc = (adj_btn_t*)data;
    p_tv_model->lightness = dsc->value;
}

static void spd_upd_run_cb(void* data)
{
    adj_btn_t* dsc = (adj_btn_t*)data;
    p_tv_model->spd = (uint8_t)dsc->value;
}

static bool int_assign_value_cb(void* data)
{
    adj_btn_t* dsc = (adj_btn_t*)data;
    ASSIGN_VALUE_TO_BTN(p_tv_model->lightness);
}

static bool spd_assign_value_cb(void* data)
{
    adj_btn_t* dsc = (adj_btn_t*)data;
    ASSIGN_VALUE_TO_BTN(p_tv_model->spd);
}

static void ctrl_btn_update_cb(void *data)
{
    state_btn_t *dsc = (state_btn_t*)data;
    p_tv_model->state = (uint8_t)dsc->cur_index; 
	set_hs_fx_window();
}

static void btnlist_update_cb(void *data)
{
    my_btnlist_ext_t* ext = (my_btnlist_ext_t*)data;    
    p_tv_model->mode = (enum fx_mode)ext->cur_index;        
    lightmode_disp_panel_limit_setting_init(ext->disp_panel, p_tv_model->mode, &p_tv_model->mode_arg);
}

static void btnlist_release_cb(void* data)
{
    my_btnlist_ext_t* ext = (my_btnlist_ext_t*)data;    
    uint8_t page_id = ui_limit_setting_get_page(p_tv_model->mode);
    
    (void)ext;

    ui_set_limit_setting(p_tv_model->mode,  &p_tv_model->mode_arg);     
    screen_set_act_index(lv_group_user_get_focused_id(indev_group));
    screen_load_page(page_id, 0, true);           
}
#endif

