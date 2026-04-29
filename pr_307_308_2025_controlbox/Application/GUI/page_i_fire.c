/*********************
 *      INCLUDES
 *********************/
#include "ui_common.h"
#include "page_widget.h"
#if (PROJECT_TYPE == 308)
/*********************
 *      DEFINES
 *********************/
#define DIVIDE_SPACE            3

#define INT_BTN_WIDTH           90
#define INT_BTN_HEIGHT          136

//#define CCT_RANGE_BTN_WIDTH     301
//#define CCT_RANGE_BTN_HEIGHT    64

//#define SPD_BTN_WIDTH           136
//#define SPD_BTN_HEIGHT          74

//#define STATE_BTN_WIDTH         162
//#define STATE_BTN_HEIGHT        74
/*********************
 *      TPEDEFS
 *********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void int_upd_run_cb(void* data);
static void cct_upd_run_cb(void* data);
//static void spd_upd_run_cb(void* data);
static bool int_assign_value_cb(void* data);;
//static bool spd_assign_value_cb(void* data);
static bool cct_assign_value_cb(void* data);
static void ctrl_btn_update_cb(void *data);
//static void btnlist_update_cb(void *data);
//static void btnlist_release_cb(void* data);

static void page_fire_construct(void);
static void page_fire_destruct(void);
static void page_fire_time_upd_cb(uint32_t ms);
static void page_fire_event_cb(int event);
static void frq_upd_run_cb(void* data);
static bool frq_assign_value_cb(void* data);
/*********************
 *  STATIC VATIABLES
 *********************/
static adj_btn_t btn_list[] = {
    ADJ_BTN_INITIALIZER(0, 1000, 1, 500, "I NT", "%", 3, 55, -3,false, false,  intensity_get_value_str, int_upd_run_cb, int_assign_value_cb, adj_btn4_refresh, common_intensity_get_step),
    ADJ_BTN_INITIALIZER(0, 2, 1, 0, "CCT", "K", 3, 41, -3, false, false,  cct_get_value_str, cct_upd_run_cb, cct_assign_value_cb, adj_cct_refresh, NULL),         
    ADJ_BTN_INITIALIZER(FREQUENCY_LEVEL_1, FREQUENCY_LEVEL_R, 1, DECAY_LEVEL_R, "Frq", "", 3, 78, -3,false, false,  frequence_get_value_str, frq_upd_run_cb, frq_assign_value_cb, adj_btn5_refresh, NULL),       
};

static lv_group_t *indev_group;
static struct db_fx_fire  *p_fire_model;
static lv_obj_t* state_panel;
static lv_obj_t* cct_panel;
static lv_obj_t* cct_label_panel[3];
static char *cct_label_str[] = {"Warmer", "Natural","Cooler"};
/*********************
 *  GLOBAL VATIABLES
 *********************/
page_t page_fire = PAGE_INITIALIZER("Fire",
                                PAGE_FIRE, 
                                page_fire_construct, 
                                page_fire_destruct, 
                                page_fire_time_upd_cb,
                                page_fire_event_cb,                                
                                PAGE_LIGHT_EFFECT_MENU);
/*********************
 *  GLOBAL FUNCTIONS
 *********************/
void page_fire_init(lv_obj_t* parent)
{
	uint8_t hsmode;
	data_center_read_config_data(SYS_CONFIG_HIGH_SPEED_MODE, &hsmode);
	if(hsmode)
	{
		g_tUIAllData.clublights_model.state = 0;
	}
    p_fire_model = (struct db_fx_fire *)screen_get_act_page_data();
    indev_group = lv_group_create();
    page_event_send(EVENT_DATA_PULL);

    my_indev_set_group(indev_group);
    lv_group_set_default(indev_group);
    lv_obj_set_style_bg_color(parent, lv_color_black(), 0);
    
    lv_obj_t* int_panel = adj_btn_create(parent, LIGHT_EFFECT_COMMON_RADIUS, ADJ_BTN_TYPE_14);
    lv_obj_set_size(int_panel, INT_BTN_WIDTH, INT_BTN_HEIGHT);
    lv_obj_align(int_panel, LV_ALIGN_TOP_LEFT, 16, 2);
    adj_int_widget_creat(int_panel, 70, 78, p_fire_model->lightness, &btn_list[0]);
    lv_obj_set_user_data(int_panel, (void *)&btn_list[0]);
    lv_obj_add_event_cb(int_panel, adj_btn_event_cb, LV_EVENT_ALL, NULL);       
    lv_group_add_obj(indev_group, int_panel);
    
    cct_panel = adj_btn_create(parent, LIGHT_EFFECT_COMMON_RADIUS, ADJ_BTN_TYPE_14);
    lv_obj_clear_flag(cct_panel, LV_OBJ_FLAG_SCROLLABLE);  //取消滚动效果
    adj_cct_widget_creat(cct_panel, 150, p_fire_model->cct, &btn_list[1], 0);
    lv_obj_set_size(cct_panel, INT_BTN_WIDTH, INT_BTN_HEIGHT);
    lv_obj_align_to(cct_panel, int_panel,  LV_ALIGN_OUT_RIGHT_MID, 8, 0); 
    lv_obj_set_user_data(cct_panel, (void *)&btn_list[1]);
    lv_obj_add_event_cb(cct_panel, adj_btn_event_cb, LV_EVENT_ALL, NULL); 
    lv_group_add_obj(indev_group, cct_panel);
    lv_obj_t* label_obj = lv_obj_get_child(cct_panel, 2);
    lv_obj_t* img = lv_obj_get_child(cct_panel, 0);
    lv_obj_add_flag(label_obj, LV_OBJ_FLAG_HIDDEN);  //隐藏起来
    for(uint8_t i =0 ;i < 3; i++)
    {
        cct_label_panel[i] = lv_label_create(cct_panel);
        lv_obj_set_style_text_color(cct_label_panel[i], lv_color_make(132,132,132), 0);
        lv_obj_set_width(cct_label_panel[i], lv_pct(90));
        lv_label_set_text(cct_label_panel[i], cct_label_str[i]);
        if(i==0)
            lv_obj_align_to(cct_label_panel[i], img, LV_ALIGN_OUT_BOTTOM_MID, 15, 0);
        else
            lv_obj_align_to(cct_label_panel[i], cct_label_panel[i-1], LV_ALIGN_OUT_BOTTOM_MID, 0, 1);
    }
    lv_obj_set_style_text_color(cct_label_panel[p_fire_model->cct], lv_color_white(), 0);
    if(p_fire_model->cct == 0)
    {
        effects_refresh_scale_img(cct_panel, 3200, UI_CCT_MIN, UI_CCT_MAX);
    }
    else if(p_fire_model->cct == 1)
    {
        effects_refresh_scale_img(cct_panel, 4600, UI_CCT_MIN, UI_CCT_MAX);
    }
    else if(p_fire_model->cct == 2)
    {
        effects_refresh_scale_img(cct_panel, 6000, UI_CCT_MIN, UI_CCT_MAX);
    }
    
    lv_obj_t* frq_panel = adj_btn_create(parent, LIGHT_EFFECT_COMMON_RADIUS, ADJ_BTN_TYPE_24);
    lv_obj_set_size(frq_panel, INT_BTN_WIDTH, INT_BTN_HEIGHT);
    lv_obj_align_to(frq_panel, cct_panel,  LV_ALIGN_OUT_RIGHT_MID, 8, 0); 
    adj_frq_widget_creat(frq_panel, 54, p_fire_model->frq, &btn_list[2]);
    lv_obj_set_user_data(frq_panel, (void *)&btn_list[2]);
    lv_obj_add_event_cb(frq_panel, adj_btn_event_cb, LV_EVENT_ALL, NULL);       
    lv_group_add_obj(indev_group, frq_panel);      

    state_panel = adj_state_widget_creat(parent,&p_fire_model->state, 288, 50, ctrl_btn_update_cb);
    lv_obj_align(state_panel,  LV_ALIGN_BOTTOM_MID, 0, -9);       
    lv_group_add_obj(indev_group, state_panel); 
    lv_obj_add_event_cb(state_panel, state_btn_event, LV_EVENT_ALL, NULL);       
    
    lv_group_user_focus_obj_by_id(indev_group, screen_get_act_index());
    screen_set_act_index(0); 
    lv_group_set_wrap(indev_group, false);   
    
    page_event_send(EVENT_DATA_COMPULSION);
}
/**********************
 *   STATIC FUNCTIONS
 **********************/
static void page_fire_construct(void)
{
    lv_obj_set_style_bg_color(lv_scr_act(), lv_color_black(), 0);

    lv_obj_t* t_cont = lv_obj_create(lv_scr_act());
    lv_obj_remove_style_all(t_cont);
    lv_obj_set_size(t_cont, lv_pct(100), lv_pct(12));
    lv_obj_set_style_text_color(t_cont, lv_color_white(), 0);
    lv_obj_set_pos(t_cont, 0, 0);

    light_effect_title_create(t_cont, Lang_GetStringByID(STRING_ID_FIRE));

    lv_obj_t* b_cont = lv_obj_create(lv_scr_act());
    lv_obj_remove_style_all(b_cont);
    lv_obj_set_size(b_cont, lv_pct(100), lv_pct(88));
    lv_obj_align_to(b_cont, t_cont, LV_ALIGN_OUT_BOTTOM_MID, 0, 0);
    page_fire_init(b_cont);
    
    uint8_t id = PAGE_FIRE;
    uint8_t sub_id = 0;
    data_center_write_config_data_no_event(SYS_CONFIG_LOCAL_UI, &id);
    data_center_write_config_data_no_event(SYS_CONFIG_LOCAL_SUB_UI, &sub_id);
	data_center_write_config_data_no_event(SYS_CONFIG_EFFECTS_MODE, &id);
}

static void page_fire_destruct(void)
{
    lv_group_focus_freeze(indev_group, true);
    lv_group_remove_all_objs(indev_group);
    lv_group_del(indev_group);
    indev_group = NULL;

    lv_obj_clean(lv_scr_act());
}

static void page_fire_time_upd_cb(uint32_t ms)
{
    page_invalid_type_t type = page_get_invalid_type();
    bool force_upd = false;
    
    //update the title bar.
    lv_obj_t* t_cont = lv_obj_get_child(lv_scr_act(), 0);
    (void)t_cont;
    page_event_send(EVENT_DATA_WRITE);
    
	if(get_hs_fx_window_state())
	{
		hs_fx_window_state_set(0);
		p_fire_model->state = 1;
		type = PAGE_INVALID_TYPE_REFRESH;
	}
    switch(type)
    {
        case PAGE_INVALID_TYPE_IDLE:    return;
		case PAGE_INVALID_TYPE_REFRESH:
			force_upd = true;
			page_set_invalid_type(PAGE_INVALID_TYPE_IDLE);
		break;
        case PAGE_INVALID_TYPE_PORTION:
        case PAGE_INVALID_TYPE_ALL:
        {
            struct db_fx_fire fire;
            
            page_set_invalid_type(PAGE_INVALID_TYPE_IDLE);
            data_center_read_light_data(LIGHT_MODE_FX_FIRE, &fire); 
            memcpy(p_fire_model, &fire, sizeof(fire));
            force_upd = type == PAGE_INVALID_TYPE_ALL ? true : false;
        }
        break;
		
		default:break;
    }   

    lv_obj_t* fucosed_obj = lv_group_get_focused(indev_group);    
    if (lv_group_get_editing(indev_group)) //如果当前正在处于编辑状态，那么退出编辑状态。
    {
        lv_event_send(fucosed_obj, LV_EVENT_RELEASED, NULL);
    }

    state_btn_t *state_user_data = lv_obj_get_user_data(state_panel);
    if (state_user_data->cur_index != p_fire_model->state || force_upd)    //更新触发按键
    {
        bool focused = fucosed_obj == state_user_data->btn ? true : false;      
        if (p_fire_model->state > state_user_data->max)
        {
            p_fire_model->state = state_user_data->max;
        }
        
        state_user_data->cur_index = p_fire_model->state;
        state_user_data->widget_refresh_cb(state_user_data->btn, focused, false);
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
static void page_fire_event_cb(int event)
{
    struct db_fx_fire fire;
    uint32_t src_crc32, dest_crc32;
	uint8_t type;
	
    data_center_read_light_data(LIGHT_MODE_FX_FIRE, &fire);   
    dest_crc32 = ui_calc_checksum((uint8_t*)&fire,  sizeof(struct db_fx_fire));    
    switch (event)
    {
        case EVENT_DATA_WRITE:
        {
            src_crc32 = ui_calc_checksum((uint8_t*)p_fire_model, sizeof(struct db_fx_fire));            
            if (dest_crc32 != src_crc32)
            {
                data_center_write_light_data(LIGHT_MODE_FX_FIRE, p_fire_model);
            }
        }
        break;
        case EVENT_DATA_PULL:
        {
            data_center_read_light_data(LIGHT_MODE_FX_FIRE, &fire); 
			type = page_get_invalid_type();
			if(type == 1)  
				break;
            memcpy(p_fire_model, &fire, sizeof(struct db_fx_fire));
			page_set_invalid_type(PAGE_INVALID_TYPE_PORTION);
        }
        break;
        case EVENT_DATA_COMPULSION:
        {
            data_center_write_light_data(LIGHT_MODE_FX_FIRE, p_fire_model);
        }
        break;
		case EVENT_DATA_HS_WINDOW:
		{
			page_set_invalid_type(PAGE_INVALID_TYPE_REFRESH);
			break;
		}
    }
}

static void int_upd_run_cb(void* data)
{
    adj_btn_t* dsc = (adj_btn_t*)data;
    p_fire_model->lightness = dsc->value;
    page_event_send(EVENT_DATA_WRITE);
}

static void cct_upd_run_cb(void* data)
{
    adj_btn_t* dsc = (adj_btn_t*)data;
	(void)dsc;
    p_fire_model->cct = dsc->value;
    if(p_fire_model->cct > dsc->max)
        p_fire_model->cct = dsc->max;
    else if(p_fire_model->cct < dsc->min)
        p_fire_model->cct = dsc->min;
    for(uint8_t i =0 ;i < 3; i++)
    {
        lv_obj_set_style_text_color(cct_label_panel[i], lv_color_make(132,132,132), 0);
    }
    lv_obj_set_style_text_color(cct_label_panel[p_fire_model->cct], lv_color_white(), 0);
    if(p_fire_model->cct == 0)
    {
        effects_refresh_scale_img(cct_panel, 3200, UI_CCT_MIN, UI_CCT_MAX);
    }
    else if(p_fire_model->cct == 1)
    {
        effects_refresh_scale_img(cct_panel, 4600, UI_CCT_MIN, UI_CCT_MAX);
    }
    else if(p_fire_model->cct == 2)
    {
        effects_refresh_scale_img(cct_panel, 6000, UI_CCT_MIN, UI_CCT_MAX);
    }
    page_event_send(EVENT_DATA_WRITE);
}

static bool int_assign_value_cb(void* data)
{
    adj_btn_t* dsc = (adj_btn_t*)data;
    ASSIGN_VALUE_TO_BTN(p_fire_model->lightness);
}

static bool cct_assign_value_cb(void* data)
{
    adj_btn_t* dsc = (adj_btn_t*)data;
	ASSIGN_VALUE_TO_BTN(p_fire_model->cct);
}

static void ctrl_btn_update_cb(void *data)
{
    state_btn_t *dsc = (state_btn_t*)data;
	uint8_t  hsmode ;
	data_center_read_config_data(SYS_CONFIG_HIGH_SPEED_MODE, &hsmode);
	if(hsmode == 0)
	{
		hs_fx_window_state_set(0);
		p_fire_model->state = (uint8_t)dsc->cur_index;    
		page_event_send(EVENT_DATA_WRITE);
	}
	set_hs_fx_window();
}

static void frq_upd_run_cb(void* data)
{
    adj_btn_t* dsc = (adj_btn_t*)data;
    p_fire_model->frq = (uint8_t)dsc->value;
    page_event_send(EVENT_DATA_WRITE);
}

static bool frq_assign_value_cb(void* data)
{
    adj_btn_t* dsc = (adj_btn_t*)data;
    ASSIGN_VALUE_TO_BTN(p_fire_model->frq);
}
#endif
