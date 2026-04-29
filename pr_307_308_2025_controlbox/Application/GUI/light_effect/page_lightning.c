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

#define GM_BTN_WIDTH            149
#define GM_BTN_HEIGHT           43

#define CCT_BTN_WIDTH           301
#define CCT_BTN_HEIGHT          64

#define SPD_BTN_WIDTH           136
#define SPD_BTN_HEIGHT          35

#define FRQ_BTN_WIDTH           136
#define FRQ_BTN_HEIGHT          35

#define STATE_BTN_WIDTH         77
#define STATE_BTN_HEIGHT        74
    
#define TRIGGER_BTN_WIDTH       81
#define TRIGGER_BTN_HEIGHT      74

/*********************
 *      TPEDEFS
 *********************/
enum {
    TRIGGER_STATE_STOP = 0,
    TRIGGER_STATE_LOOP = 1,
};

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void int_upd_run_cb(void* data);
static void spd_upd_run_cb(void* data);
static void frq_upd_run_cb(void* data);
static void gm_upd_run_cb(void* data);
static void cct_upd_run_cb(void* data);

static bool cct_assign_value_cb(void* data);
static bool int_assign_value_cb(void* data);
static bool spd_assign_value_cb(void* data);
static bool frq_assign_value_cb(void* data);
static bool gm_assign_value_cb(void* data);

static void ctrl_btn_update_cb(void *data);
static void btnlist_update_cb(void *data);
static void btnlist_release_cb(void* data);
static void trigger_btn_release_cb(void);
static void trigger_btn_anim_cb(lv_anim_t* a);

static void page_lightning_construct(void);
static void page_lightning_destruct(void);
static void page_lightning_time_upd_cb(uint32_t ms);
static void page_lightning_event_cb(int event);
/*********************
 *  STATIC VATIABLES
 *********************/
static adj_btn_t btn_list[] = {
    ADJ_BTN_INITIALIZER(0, 1000, 1, 500, "I NT", "%", 9, 59, -6,false, false,  intensity_get_value_str, int_upd_run_cb, int_assign_value_cb, NULL, common_intensity_get_step),        
    ADJ_BTN_INITIALIZER(FREQUENCY_LEVEL_1, FREQUENCY_LEVEL_R, 1, FREQUENCY_LEVEL_1, "FRQ", "", 9, 59, -6,false, false,  frequence_get_value_str, frq_upd_run_cb, frq_assign_value_cb, NULL, NULL),             
    ADJ_BTN_INITIALIZER(SPEED_LEVEL_1, SPEED_LEVEL_R, 1, SPEED_LEVEL_8, "SPD", "", 9, 59, -6,false, false,  speed_get_value_str, spd_upd_run_cb, spd_assign_value_cb, NULL, NULL),             
    ADJ_BTN_INITIALIZER(-100, 100, 1, 6, "G/M", "K", 9, 60, -6, false, false,  gm_get_value_str, gm_upd_run_cb, gm_assign_value_cb, NULL, NULL),             
    ADJ_BTN_INITIALIZER(UI_CCT_MIN, UI_CCT_MAX, 50, 5600,"CCT", "K", 9, 60, -6, false, false,  cct_get_value_str, cct_upd_run_cb, cct_assign_value_cb, NULL, NULL),             
};

static lv_group_t *indev_group;
static struct db_fx_lightning_2 *p_lightning_model;
static lv_obj_t* disp_panel;
static state_btn_t state_btn_dsc;
static my_btnlist_ext_t btnlist_ext;
static trigger_btn_t trigger_btn_dsc;
//static uint8_t hs_trrige;
/*********************
 *  GLOBAL VATIABLES
 *********************/
page_t page_lightning = PAGE_INITIALIZER("Lightning",
                                PAGE_LIGHTNING, 
                                page_lightning_construct, 
                                page_lightning_destruct, 
                                page_lightning_time_upd_cb,
                                page_lightning_event_cb,                                
                                PAGE_LIGHT_EFFECT_MENU);
/*********************
 *  GLOBAL FUNCTIONS
 *********************/
void page_lightning_init(lv_obj_t* parent)
{
	uint8_t  hsmode ;
	data_center_read_config_data(SYS_CONFIG_HIGH_SPEED_MODE, &hsmode);
	if(hsmode)
	{
		g_tUIAllData.lightning2_model.trigger = 0;
		g_tUIAllData.lightning2_model.state = 0;
	}
    p_lightning_model = (struct db_fx_lightning_2*)screen_get_act_page_data();
	page_event_send(EVENT_DATA_PULL);
    indev_group = lv_group_create();    
    my_indev_set_group(indev_group);
    lv_group_set_default(indev_group);

    lv_obj_set_style_bg_color(parent, lv_color_black(), 0);
    
    /* 亮度 */
    lv_obj_t* int_cont = adj_btn_create(parent, LIGHT_EFFECT_COMMON_RADIUS, ADJ_BTN_TYPE_18);
    lv_obj_set_user_data(int_cont, (void *)&btn_list[0]);
    adj_btn_init(int_cont, &btn_list[0], 90);
    lv_obj_add_event_cb(int_cont, adj_btn_event_cb, LV_EVENT_ALL, NULL);    
    lv_obj_set_size(int_cont, INT_BTN_WIDTH, INT_BTN_HEIGHT);
    lv_obj_align(int_cont, LV_ALIGN_TOP_LEFT, 10, 2);
    lv_group_add_obj(indev_group, int_cont);

    static const char* list_str_map[] = {"CCT", "Adv.HSI"};
    btnlist_ext.cur_index = p_lightning_model->mode;
    btnlist_ext.max = 1;
    btnlist_ext.min = 0;
    btnlist_ext.release_cb = btnlist_release_cb;
    btnlist_ext.update_cb = btnlist_update_cb;

    lv_obj_t* btnlist_obj = btnlist_create(parent, 136, 63, &btnlist_ext, list_str_map);
    lv_obj_align_to(btnlist_obj, int_cont, LV_ALIGN_OUT_BOTTOM_MID, 0, 4);

    /* 光模式参数显示面板 */
    disp_panel = lightmode_disp_panel_create(parent, 162, 120);
    lv_obj_t* disp_frame = lv_obj_get_parent(disp_panel);
    lv_obj_align_to(disp_frame, int_cont, LV_ALIGN_OUT_RIGHT_TOP, 3, 0);
    lightmode_disp_panel_refresh(disp_panel, p_lightning_model->mode, &p_lightning_model->mode_arg); 

    btnlist_ext.obj = btnlist_obj;
    btnlist_ext.disp_panel = disp_panel;
    btnlist_ext.setting.param = &p_lightning_model->mode_arg;
    lv_obj_set_user_data(btnlist_obj, &btnlist_ext);
    lv_obj_add_event_cb(btnlist_obj,  btnlist_event_cb, LV_EVENT_ALL, NULL);
    lv_group_add_obj(indev_group, btnlist_obj);

    /* 频率 */
    btn_list[1].name = Lang_GetStringByID(STRING_ID_FRQ);
    lv_obj_t* frq_cont = adj_btn_create(parent, LIGHT_EFFECT_COMMON_RADIUS, ADJ_BTN_TYPE_18);
    lv_obj_set_user_data(frq_cont, (void *)&btn_list[1]);
    adj_btn_init(frq_cont, &btn_list[1], 90);    
    lv_obj_add_event_cb(frq_cont, adj_btn_event_cb, LV_EVENT_ALL, NULL);    
    lv_obj_set_size(frq_cont, FRQ_BTN_WIDTH, FRQ_BTN_HEIGHT);
    lv_obj_align_to(frq_cont, btnlist_obj, LV_ALIGN_OUT_BOTTOM_LEFT, 0, DIVIDE_SPACE);
    lv_group_add_obj(indev_group, frq_cont);

    /* 速度 */
    btn_list[2].name = Lang_GetStringByID(STRING_ID_SPD);
    lv_obj_t* spd_cont = adj_btn_create(parent, LIGHT_EFFECT_COMMON_RADIUS, ADJ_BTN_TYPE_18);
    lv_obj_set_user_data(spd_cont, (void *)&btn_list[2]);
    adj_btn_init(spd_cont, &btn_list[2], 90);
    lv_obj_add_event_cb(spd_cont, adj_btn_event_cb, LV_EVENT_ALL, NULL);    
    lv_obj_set_size(spd_cont, SPD_BTN_WIDTH, SPD_BTN_HEIGHT);
    lv_obj_align_to(spd_cont, frq_cont, LV_ALIGN_OUT_BOTTOM_MID, 0, DIVIDE_SPACE);
    lv_group_add_obj(indev_group, spd_cont);

    LV_IMG_DECLARE(ImgLoopIcon);
    LV_IMG_DECLARE(ImgStopIcon);
    LV_IMG_DECLARE(ImgPulseIcon);

    static const char* str_map[] = {"LOOP", "PULSE", "STOP"};
    static const lv_img_dsc_t* img_map[] = {&ImgLoopIcon, &ImgPulseIcon, &ImgStopIcon};

    str_map[0] = Lang_GetStringByID(STRING_ID_STOP);
    str_map[1] = Lang_GetStringByID(STRING_ID_LOOP);

    img_map[0] = &ImgStopIcon;
    img_map[1] = &ImgLoopIcon;

    state_btn_dsc.cur_index = 0;
    state_btn_dsc.min = 0;
    state_btn_dsc.max = 1;
    state_btn_dsc.img_map = img_map;
    state_btn_dsc.map = str_map;
    if (p_lightning_model->state > state_btn_dsc.max)
    {
        p_lightning_model->state = state_btn_dsc.max;
    }

    state_btn_dsc.cur_index = p_lightning_model->state;
    state_btn_dsc.update_cb = ctrl_btn_update_cb;
    state_btn_dsc.widget_refresh_cb = state_btn_refresh;
    state_btn_dsc.img_to_top = 40;
    state_btn_dsc.img_gap = 3;
    state_btn_dsc.title_to_top = 10;

    /* 状态键 */
    lv_obj_t* state_btn = state_btn_create(parent, &state_btn_dsc, STATE_BTN_WIDTH, STATE_BTN_HEIGHT);
    lv_obj_add_event_cb(state_btn, state_btn_event, LV_EVENT_ALL, NULL);    
    lv_obj_align_to(state_btn, frq_cont, LV_ALIGN_OUT_RIGHT_TOP, DIVIDE_SPACE, 0);
    lv_event_send(state_btn, LV_EVENT_DEFOCUSED, NULL);
    lv_group_add_obj(indev_group, state_btn);

    /* 触发按键 */
    trigger_btn_dsc.img_to_top = 40;
    trigger_btn_dsc.title_to_top = 10;
    trigger_btn_dsc.released_cb = trigger_btn_release_cb;
    lv_obj_t* trigger_btn = trigger_btn_create(parent, &trigger_btn_dsc, TRIGGER_BTN_WIDTH, TRIGGER_BTN_HEIGHT);
    lv_obj_align_to(trigger_btn, state_btn, LV_ALIGN_OUT_RIGHT_TOP, DIVIDE_SPACE, 0);
    lv_group_add_obj(indev_group, trigger_btn);
    lv_group_set_wrap(indev_group, false);
    lv_group_user_focus_obj_by_id(indev_group, screen_get_act_index());
    screen_set_act_index(0);

    adj_btn_refresh_all(btn_list, sizeof(btn_list)/ sizeof(btn_list[0]), true);
	page_event_send(EVENT_DATA_COMPULSION);
	

}
/**********************
 *   STATIC FUNCTIONS
 **********************/
static void page_lightning_construct(void)
{
    lv_obj_set_style_bg_color(lv_scr_act(), lv_color_black(), 0);

    lv_obj_t* t_cont = lv_obj_create(lv_scr_act());
    lv_obj_remove_style_all(t_cont);
    lv_obj_set_size(t_cont, lv_pct(100), lv_pct(12));
    lv_obj_set_style_text_color(t_cont, lv_color_white(), 0);
    lv_obj_set_pos(t_cont, 0, 0);

    light_effect_title_create(t_cont, Lang_GetStringByID(STRING_ID_LIGHTNING));

    lv_obj_t* b_cont = lv_obj_create(lv_scr_act());
    lv_obj_remove_style_all(b_cont);
    lv_obj_set_size(b_cont, lv_pct(100), lv_pct(88));
    lv_obj_align_to(b_cont, t_cont, LV_ALIGN_OUT_BOTTOM_MID, 0, 0);
    page_lightning_init(b_cont);
    
    uint8_t id = PAGE_LIGHTNING;
    uint8_t sub_id = 0;
    data_center_write_config_data_no_event(SYS_CONFIG_LOCAL_UI, &id);
    data_center_write_config_data_no_event(SYS_CONFIG_LOCAL_SUB_UI, &sub_id);
	data_center_write_config_data_no_event(SYS_CONFIG_EFFECTS_MODE, &id);
}

static void page_lightning_destruct(void)
{
    lv_group_focus_freeze(indev_group, true);
    lv_group_remove_all_objs(indev_group);
    lv_group_del(indev_group);

    indev_group = NULL;

    lv_anim_del_all();
    lv_obj_clean(lv_scr_act());
    
    memset(&trigger_btn_dsc, 0, sizeof(trigger_btn_dsc));
}

static void page_lightning_time_upd_cb(uint32_t ms)
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
			btnlist_refresh(btnlist_ext.obj, p_lightning_model->mode);
            force_upd = type == PAGE_INVALID_TYPE_ALL ? true : false;
        }
        break;
    }   
    lv_obj_t* fucosed_obj = lv_group_get_focused(indev_group);
    //如果当前正在处于编辑状态，那么退出编辑状态。
    if (lv_group_get_editing(indev_group))
    {
        lv_event_send(fucosed_obj, LV_EVENT_RELEASED, NULL);
    }

    //更新触发按键
    if (state_btn_dsc.cur_index != p_lightning_model->state || force_upd)
    {
        bool focused = fucosed_obj == state_btn_dsc.btn ? true : false;  
        if (p_lightning_model->state > state_btn_dsc.max)
        {
            p_lightning_model->state = state_btn_dsc.max;
        }
                
        state_btn_dsc.cur_index = p_lightning_model->state;              
        state_btn_dsc.widget_refresh_cb(state_btn_dsc.btn, focused, false);
    }

    if (g_ptUIStateData->trigger_light_anim)
    {
        g_ptUIStateData->trigger_light_anim = 0;
        lv_obj_add_state(trigger_btn_dsc.btn, LV_STATE_FOCUSED);
        lv_event_send(trigger_btn_dsc.btn, LV_EVENT_PRESSED, NULL);

        lv_anim_t a;
        lv_anim_init(&a);
        lv_anim_set_time(&a, 150);
        lv_anim_set_var(&a, &trigger_btn_dsc);
        lv_anim_set_ready_cb(&a, trigger_btn_anim_cb);
        lv_anim_start(&a);        
    }
 
    if (btnlist_ext.cur_index != p_lightning_model->mode || force_upd)
    {
        btnlist_refresh(btnlist_ext.obj, p_lightning_model->mode);
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
static void page_lightning_event_cb(int event)
{
    struct db_fx_lightning_2 lightning;
    uint32_t src_crc32, dest_crc32;
	uint8_t  hsmode ;
	data_center_read_config_data(SYS_CONFIG_HIGH_SPEED_MODE, &hsmode);
    data_center_read_light_data(LIGHT_MODE_FX_LIGHTNING_II, &lightning);   
    dest_crc32 = ui_calc_checksum((uint8_t*)&lightning,  sizeof(struct db_fx_lightning_2));    
    switch (event)
    {
        case EVENT_DATA_WRITE:
        {
            src_crc32 = ui_calc_checksum((uint8_t*)p_lightning_model, sizeof(struct db_fx_lightning_2));            
            if (dest_crc32 != src_crc32)
            {
                if(hsmode == 0)data_center_write_light_data(LIGHT_MODE_FX_LIGHTNING_II, p_lightning_model);
            }
        }
        break;
        case EVENT_DATA_PULL:
        {
            memcpy(p_lightning_model, &lightning, sizeof(struct db_fx_lightning_2));
            page_set_invalid_type(PAGE_INVALID_TYPE_PORTION);
        }
        break;
		case EVENT_DATA_COMPULSION:
            data_center_write_light_data(LIGHT_MODE_FX_LIGHTNING_II, p_lightning_model);
            break;
		case EVENT_DATA_HS_WINDOW:
		{
			bool force_upd = 0;
			memcpy(p_lightning_model, &lightning, sizeof(struct db_fx_lightning_2)); 
			state_btn_dsc.cur_index = 0; 
			state_btn_dsc.widget_refresh_cb(state_btn_dsc.btn, force_upd, false);
			break;
		}
		
    }
}

static void trigger_btn_anim_cb(lv_anim_t* a)
{
    trigger_btn_t* var = a->var;
    if (var != NULL && var->btn != NULL)
    {
        lv_event_send(var->btn, LV_EVENT_RELEASED, NULL);
    }
}

static void int_upd_run_cb(void* data)
{
    adj_btn_t* dsc = (adj_btn_t*)data;
    p_lightning_model->lightness = dsc->value;
}

static void spd_upd_run_cb(void* data)
{
    adj_btn_t* dsc = (adj_btn_t*)data;
    p_lightning_model->spd = (uint8_t)dsc->value;
}

static void frq_upd_run_cb(void* data)
{
    adj_btn_t* dsc = (adj_btn_t*)data;
    p_lightning_model->frq = (uint8_t)dsc->value;
}

static void cct_upd_run_cb(void* data)
{
    adj_btn_t* dsc = (adj_btn_t*)data;
	(void)dsc;
    //p_lightning_model->cct = dsc->value;
}

static void gm_upd_run_cb(void* data)
{
    adj_btn_t* dsc = (adj_btn_t*)data;
	(void)dsc;
    //p_lightning_model->gm = (int8_t)dsc->value;
}

static bool int_assign_value_cb(void* data)
{
    adj_btn_t* dsc = (adj_btn_t*)data;
    ASSIGN_VALUE_TO_BTN(p_lightning_model->lightness);
}

static bool spd_assign_value_cb(void* data)
{
    adj_btn_t* dsc = (adj_btn_t*)data;
    ASSIGN_VALUE_TO_BTN(p_lightning_model->spd);
}

static bool frq_assign_value_cb(void* data)
{
    adj_btn_t* dsc = (adj_btn_t*)data;
    ASSIGN_VALUE_TO_BTN(p_lightning_model->frq);
}

static bool cct_assign_value_cb(void* data)
{
    adj_btn_t* dsc = (adj_btn_t*)data;
	ASSIGN_VALUE_TO_BTN(p_lightning_model->mode_arg.cct.cct);
//    (void)dsc;
	return false;
}

static bool gm_assign_value_cb(void* data)
{
    adj_btn_t* dsc = (adj_btn_t*)data;
	
    (void)dsc;
//	ASSIGN_VALUE_TO_BTN(p_lightning_model->mode_arg.cct.duv);
	
	return false;
}

static void ctrl_btn_update_cb(void *data)
{
    state_btn_t *dsc = (state_btn_t*)data;
    p_lightning_model->state = (uint8_t)dsc->cur_index; 
	set_hs_fx_window();	
}

static void btnlist_update_cb(void *data)
{
    my_btnlist_ext_t* ext = (my_btnlist_ext_t *)data;    
    p_lightning_model->mode = (enum fx_mode)ext->cur_index;
    lightmode_disp_panel_refresh(ext->disp_panel, p_lightning_model->mode, &ext->setting.param[0]);//[p_lightning_model->mode]
}

static void btnlist_release_cb(void* data)
{
    uint8_t page_id = ui_param_setting_get_page(p_lightning_model->mode);
    ui_set_param_setting(p_lightning_model->mode, &p_lightning_model->mode_arg);     
    screen_set_act_index(lv_group_user_get_focused_id(indev_group));
    screen_load_page(page_id, 0, true);
}

static void trigger_btn_release_cb(void)
{

//	hs_trrige = 1;
	UI_PRINTF("SINGLGE_BTN_CLICKED_CB\r\n");    
	//    if (p_lightning_model->state != TRIGGER_STATE_STOP)
	//    {
	p_lightning_model->trigger = 1;
	p_lightning_model->state = 0;
	state_btn_dsc.cur_index = 1;
	data_center_write_light_data(LIGHT_MODE_FX_LIGHTNING_II, p_lightning_model);
	set_hs_fx_window();
	lv_event_send(state_btn_dsc.btn, LV_EVENT_CLICKED, NULL);
//    }        
}
#endif

