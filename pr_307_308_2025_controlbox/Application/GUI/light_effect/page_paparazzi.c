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
#define INT_BTN_WIDTH           152
#define INT_BTN_HEIGHT          43

#define CCT_BTN_WIDTH           152
#define CCT_BTN_HEIGHT          64

#define GM_BTN_WIDTH            149
#define GM_BTN_HEIGHT           64

#define FRQ_BTN_WIDTH           152
#define FRQ_BTN_HEIGHT          69

#define STATE_BTN_WIDTH         149
#define STATE_BTN_HEIGHT        69
/*********************
 *      TPEDEFS
 *********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void int_upd_run_cb(void* data);
static void cct_upd_run_cb(void* data);
static void gm_upd_run_cb(void* data);
static void frq_upd_run_cb(void* data);
static bool int_assign_value_cb(void* data);;
static bool cct_assign_value_cb(void* data);
static bool gm_assign_value_cb(void* data);;
static bool frq_assign_value_cb(void* data);

static void ctrl_btn_update_cb(void* data);

static void page_paparazzi_construct(void);
static void page_paparazzi_destruct(void);
static void page_paparazzi_time_upd_cb(uint32_t ms);
static void page_paparazzi_event_cb(int event);
/*********************
 *  STATIC VATIABLES
 *********************/
static adj_btn_t btn_list[] = {
    ADJ_BTN_INITIALIZER(0, 1000, 1, 500, "I NT", "%", 3, 55, -3, false, false,  intensity_get_value_str, int_upd_run_cb, int_assign_value_cb, NULL, common_intensity_get_step),
    ADJ_BTN_INITIALIZER(UI_CCT_MIN, UI_CCT_MAX, 50, 5600,"CCT", "K", 3, 55, -3, false, false,  cct_get_value_str, cct_upd_run_cb, cct_assign_value_cb, NULL, NULL),             
    ADJ_BTN_INITIALIZER(-100, 100, 1, 6, "G/M", "K", 3, 55, -12, false, false,  gm_get_value_str, gm_upd_run_cb, gm_assign_value_cb, NULL, NULL),             
    ADJ_BTN_INITIALIZER(FREQUENCY_LEVEL_1, FREQUENCY_LEVEL_10, 1, 3, "FRQ", "", 3, 55, -6,false, false,  frequence_get_value_str, frq_upd_run_cb, frq_assign_value_cb, NULL, NULL),                 
};

static lv_group_t *indev_group;
static struct db_fx_paparazzi *p_paparazzi_model;
static state_btn_t state_btn_dsc;
/*********************
 *  GLOBAL VATIABLES
 *********************/
page_t page_paparazzi = PAGE_INITIALIZER("Paparazzi",
                                PAGE_PAPARAZZI, 
                                page_paparazzi_construct, 
                                page_paparazzi_destruct, 
                                page_paparazzi_time_upd_cb,
                                page_paparazzi_event_cb,                                
                                PAGE_LIGHT_EFFECT_MENU);
/*********************
 *  GLOBAL FUNCTIONS
 *********************/
void page_paparazzi_init(lv_obj_t* parent)
{
	uint8_t hsmode;
	data_center_read_config_data(SYS_CONFIG_HIGH_SPEED_MODE, &hsmode);
	if(hsmode)
	{
		g_tUIAllData.paparazzi_model.state = 0;
	}
    p_paparazzi_model = (struct db_fx_paparazzi*)screen_get_act_page_data();
    indev_group = lv_group_create();    
    page_event_send(EVENT_DATA_PULL);
    my_indev_set_group(indev_group);
    lv_group_set_default(indev_group);

    lv_obj_set_style_bg_color(parent, lv_color_black(), 0);
    
    lv_obj_t* int_cont = adj_btn_create(parent, LIGHT_EFFECT_COMMON_RADIUS, ADJ_BTN_TYPE_18);
    lv_obj_set_user_data(int_cont, (void *)&btn_list[0]);
    adj_btn_init(int_cont, &btn_list[0], 90);
    lv_obj_add_event_cb(int_cont, adj_btn_event_cb, LV_EVENT_ALL, NULL);    
    lv_obj_set_size(int_cont, INT_BTN_WIDTH, INT_BTN_HEIGHT);
    lv_obj_align(int_cont, LV_ALIGN_TOP_LEFT, 10, 5);
    lv_group_add_obj(indev_group, int_cont);

    lv_obj_t* cct_cont = adj_btn_create(parent, LIGHT_EFFECT_COMMON_RADIUS, ADJ_BTN_TYPE_18);
    lv_obj_set_user_data(cct_cont, (void *)&btn_list[1]);
    adj_btn_init(cct_cont, &btn_list[1], 90);
    lv_obj_add_event_cb(cct_cont, adj_btn_event_cb, LV_EVENT_ALL, NULL);    
    lv_obj_set_size(cct_cont, CCT_BTN_WIDTH, CCT_BTN_HEIGHT);
    lv_obj_align_to(cct_cont, int_cont, LV_ALIGN_OUT_BOTTOM_LEFT, 0, DIVIDE_SPACE);
    lv_group_add_obj(indev_group, cct_cont);
    
    lv_obj_t* gm_cont = adj_btn_create(parent, LIGHT_EFFECT_COMMON_RADIUS, ADJ_BTN_TYPE_18);
    lv_obj_set_user_data(gm_cont, (void *)&btn_list[2]);
    adj_btn_init(gm_cont, &btn_list[2], 90);
    lv_obj_add_event_cb(gm_cont, adj_btn_event_cb, LV_EVENT_ALL, NULL);    
    lv_obj_set_size(gm_cont, GM_BTN_WIDTH, GM_BTN_HEIGHT);    
    lv_obj_align_to(gm_cont, cct_cont, LV_ALIGN_OUT_RIGHT_MID, DIVIDE_SPACE, 0);
    lv_group_add_obj(indev_group, gm_cont);

    btn_list[3].name = Lang_GetStringByID(STRING_ID_FRQ);
    lv_obj_t* frq_cont = adj_btn_create(parent, LIGHT_EFFECT_COMMON_RADIUS, ADJ_BTN_TYPE_18);
    lv_obj_set_user_data(frq_cont, (void *)&btn_list[3]);
    adj_btn_init(frq_cont, &btn_list[3], 90);
    lv_obj_add_event_cb(frq_cont, adj_btn_event_cb, LV_EVENT_ALL, NULL);    
    lv_obj_set_size(frq_cont, FRQ_BTN_WIDTH, FRQ_BTN_HEIGHT);
    lv_obj_align_to(frq_cont, cct_cont, LV_ALIGN_OUT_BOTTOM_LEFT, 0, DIVIDE_SPACE);
    lv_group_add_obj(indev_group, frq_cont);

    LV_IMG_DECLARE(ImgLoopIcon);
    LV_IMG_DECLARE(ImgStopIcon);

    static const char* str_map[] = {"LOOP", "STOP"};
    static const lv_img_dsc_t* img_map[] = {&ImgLoopIcon, &ImgStopIcon};

    str_map[0] = Lang_GetStringByID(STRING_ID_STOP);
    str_map[1] = Lang_GetStringByID(STRING_ID_LOOP) ;

    img_map[0] = &ImgStopIcon;
    img_map[1] = &ImgLoopIcon;

    state_btn_dsc.cur_index = 0;
    state_btn_dsc.min = 0;
    state_btn_dsc.max = 1;
    state_btn_dsc.img_map = img_map;
    state_btn_dsc.map = str_map;    
    if (p_paparazzi_model->state > state_btn_dsc.max)
    {
        p_paparazzi_model->state = state_btn_dsc.max;
    }

    state_btn_dsc.cur_index = p_paparazzi_model->state;
    state_btn_dsc.update_cb = ctrl_btn_update_cb;
    state_btn_dsc.widget_refresh_cb = state_btn_refresh;
    state_btn_dsc.img_to_top = 40;
    state_btn_dsc.img_gap = 3;
    state_btn_dsc.title_to_top = 10;

    lv_obj_t* state_btn = state_btn_create(parent, &state_btn_dsc, STATE_BTN_WIDTH, STATE_BTN_HEIGHT);
    lv_obj_add_event_cb(state_btn, state_btn_event, LV_EVENT_ALL, NULL);    
    lv_obj_align_to(state_btn, frq_cont, LV_ALIGN_OUT_RIGHT_TOP, DIVIDE_SPACE, 0);
    lv_event_send(state_btn, LV_EVENT_DEFOCUSED, NULL);
    lv_group_add_obj(indev_group, state_btn);    
    lv_group_set_wrap(indev_group, false);
    adj_btn_refresh_all(btn_list, sizeof(btn_list)/ sizeof(btn_list[0]), true);
	page_event_send(EVENT_DATA_COMPULSION);
}
/**********************
 *   STATIC FUNCTIONS
 **********************/
static void page_paparazzi_construct(void)
{
    lv_obj_set_style_bg_color(lv_scr_act(), lv_color_black(), 0);

    lv_obj_t* t_cont = lv_obj_create(lv_scr_act());
    lv_obj_remove_style_all(t_cont);
    lv_obj_set_size(t_cont, lv_pct(100), lv_pct(12));
    lv_obj_set_style_text_color(t_cont, lv_color_white(), 0);
    lv_obj_set_pos(t_cont, 0, 0);

    light_effect_title_create(t_cont, Lang_GetStringByID(STRING_ID_PAPARAZZI));

    lv_obj_t* b_cont = lv_obj_create(lv_scr_act());
    lv_obj_remove_style_all(b_cont);
    lv_obj_set_size(b_cont, lv_pct(100), lv_pct(88));
    lv_obj_align_to(b_cont, t_cont, LV_ALIGN_OUT_BOTTOM_MID, 0, 0);
    page_paparazzi_init(b_cont);
    
    uint8_t id = PAGE_PAPARAZZI;
    uint8_t sub_id = 0;
    data_center_write_config_data_no_event(SYS_CONFIG_LOCAL_UI, &id);
    data_center_write_config_data_no_event(SYS_CONFIG_LOCAL_SUB_UI, &sub_id);
	data_center_write_config_data_no_event(SYS_CONFIG_EFFECTS_MODE, &id);
}

static void page_paparazzi_destruct(void)
{
    lv_group_focus_freeze(indev_group, true);
    lv_group_remove_all_objs(indev_group);
    lv_group_del(indev_group);
    indev_group = NULL;

    lv_obj_clean(lv_scr_act());
}

static void page_paparazzi_time_upd_cb(uint32_t ms)
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

    if (state_btn_dsc.cur_index != p_paparazzi_model->state || force_upd)    //更新触发按键
    {
        bool focused = fucosed_obj == state_btn_dsc.btn ? true : false;      
        if (p_paparazzi_model->state > state_btn_dsc.max)
        {
            p_paparazzi_model->state = state_btn_dsc.max;
        }
                
        state_btn_dsc.cur_index = p_paparazzi_model->state;
        state_btn_dsc.widget_refresh_cb(state_btn_dsc.btn, focused, false);
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
static void page_paparazzi_event_cb(int event)
{
    struct db_fx_paparazzi paparazzi;
    uint32_t src_crc32, dest_crc32;
	uint8_t  hsmode ;
	data_center_read_config_data(SYS_CONFIG_HIGH_SPEED_MODE, &hsmode);
    data_center_read_light_data(LIGHT_MODE_FX_PAPARAZZI, &paparazzi);   
    dest_crc32 = ui_calc_checksum((uint8_t*)&paparazzi,  sizeof(struct db_fx_paparazzi));    
    switch (event)
    {
        case EVENT_DATA_WRITE:
        {
            src_crc32 = ui_calc_checksum((uint8_t*)p_paparazzi_model, sizeof(struct db_fx_paparazzi));            
            if (dest_crc32 != src_crc32)
            {
                if(hsmode == 0)data_center_write_light_data(LIGHT_MODE_FX_PAPARAZZI, p_paparazzi_model);
            }
        }
        break;
        case EVENT_DATA_PULL:
        {
            memcpy(p_paparazzi_model, &paparazzi, sizeof(struct db_fx_paparazzi));
            page_set_invalid_type(PAGE_INVALID_TYPE_PORTION);
        }
        break;
		case EVENT_DATA_COMPULSION:
            data_center_write_light_data(LIGHT_MODE_FX_PAPARAZZI, p_paparazzi_model);
            break;
		case EVENT_DATA_HS_WINDOW:
		{
			bool force_upd = 0;
			memcpy(p_paparazzi_model, &paparazzi, sizeof(struct db_fx_paparazzi)); 
			state_btn_dsc.cur_index = 0; 
			state_btn_dsc.widget_refresh_cb(state_btn_dsc.btn, force_upd, false);
			break;
		}
    }
}



static void int_upd_run_cb(void* data)
{
    adj_btn_t* dsc = (adj_btn_t*)data;
    p_paparazzi_model->lightness = dsc->value;
}

static void cct_upd_run_cb(void* data)
{
    adj_btn_t* dsc = (adj_btn_t*)data;
    p_paparazzi_model->cct = dsc->value;
}

static void gm_upd_run_cb(void* data)
{
    adj_btn_t* dsc = (adj_btn_t*)data;
    p_paparazzi_model->gm = (int8_t)dsc->value;
}

static void frq_upd_run_cb(void* data)
{
    adj_btn_t* dsc = (adj_btn_t*)data;
    p_paparazzi_model->frq = (uint8_t)dsc->value;
}

static bool int_assign_value_cb(void* data)
{
    adj_btn_t* dsc = (adj_btn_t*)data;
    ASSIGN_VALUE_TO_BTN(p_paparazzi_model->lightness);
}

static bool cct_assign_value_cb(void* data)
{
    adj_btn_t* dsc = (adj_btn_t*)data;
    ASSIGN_VALUE_TO_BTN(p_paparazzi_model->cct);
}

static bool gm_assign_value_cb(void* data)
{
    adj_btn_t* dsc = (adj_btn_t*)data;
    ASSIGN_VALUE_TO_BTN(p_paparazzi_model->gm);
}

static bool frq_assign_value_cb(void* data)
{
    adj_btn_t* dsc = (adj_btn_t*)data;
    ASSIGN_VALUE_TO_BTN(p_paparazzi_model->frq);
}

static void ctrl_btn_update_cb(void *data)
{
    state_btn_t *dsc = (state_btn_t*)data;
    p_paparazzi_model->state = (uint8_t)dsc->cur_index;  
	set_hs_fx_window();		
}
#endif
