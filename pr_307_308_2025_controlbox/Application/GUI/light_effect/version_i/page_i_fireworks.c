/*********************
 *      INCLUDES
 *********************/
#include "ui_common.h"
#include "page_widget.h"
#if (PROJECT_TYPE == 308)
/*********************
 *      DEFINES
 *********************/
#define DIVIDE_SPACE                3

#define INT_BTN_WIDTH           90
#define INT_BTN_HEIGHT          136

//#define FRQ_BTN_WIDTH               149       
//#define FRQ_BTN_HEIGHT              69

//#define LIGHT_MODE_ROLLER_WIDTH     149
//#define LIGHT_MODE_ROLLER_HEIGHT    95

//#define STATE_BTN_WIDTH             149
//#define STATE_BTN_HEIGHT            95

/*********************
 *      TPEDEFS
 *********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void int_upd_run_cb(void* data);
static void cct_upd_run_cb(void* data);
//static void spd_upd_run_cb(void* data);
static bool int_assign_value_cb(void* data);
static bool cct_assign_value_cb(void* data);
//static bool spd_assign_value_cb(void* data);

static void ctrl_btn_update_cb(void *data);
//static void roller_value_changed_cb(uint32_t value);

static void page_fireworks_construct(void);
static void page_fireworks_destruct(void);
static void page_fireworks_time_upd_cb(uint32_t ms);
static void page_fireworks_event_cb(int event);
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
static struct db_fx_fireworks *p_fireworks_model;
//static state_btn_t state_btn_dsc;
//static my_roller_ext_t roller_ext;
static light_mode_list_ext_t list_ext;
static lv_obj_t* state_panel;
static lv_obj_t* cct_panel;
static lv_obj_t* cct_label_panel[3];
static char *cct_label_str[] = {"3200K","5600K","Random"};
/*********************
 *  GLOBAL VATIABLES
 *********************/
page_t page_fireworks = PAGE_INITIALIZER("Fireworks",
                                PAGE_FIREWORKS, 
                                page_fireworks_construct, 
                                page_fireworks_destruct, 
                                page_fireworks_time_upd_cb,
                                page_fireworks_event_cb,
                                PAGE_LIGHT_EFFECT_MENU);
/*********************
 *  GLOBAL FUNCTIONS
 *********************/

void page_fireworks_init(lv_obj_t* parent)
{
	uint8_t hsmode;
	data_center_read_config_data(SYS_CONFIG_HIGH_SPEED_MODE, &hsmode);
	if(hsmode)
	{
		g_tUIAllData.clublights_model.state = 0;
	}
    p_fireworks_model = (struct db_fx_fireworks*)screen_get_act_page_data();
    indev_group = lv_group_create();    

    page_event_send(EVENT_DATA_PULL);
    my_indev_set_group(indev_group);
    lv_group_set_default(indev_group);

    lv_obj_set_style_bg_color(parent, lv_color_black(), 0);

    lv_obj_t* int_panel = adj_btn_create(parent, LIGHT_EFFECT_COMMON_RADIUS, ADJ_BTN_TYPE_14);
    lv_obj_set_size(int_panel, INT_BTN_WIDTH, INT_BTN_HEIGHT);
    lv_obj_align(int_panel, LV_ALIGN_TOP_LEFT, 16, 2);
    adj_int_widget_creat(int_panel, 70, 78, p_fireworks_model->lightness, &btn_list[0]);
    lv_obj_set_user_data(int_panel, (void *)&btn_list[0]);
    lv_obj_add_event_cb(int_panel, adj_btn_event_cb, LV_EVENT_ALL, NULL);       
    lv_group_add_obj(indev_group, int_panel);
    
    cct_panel = adj_btn_create(parent, LIGHT_EFFECT_COMMON_RADIUS, ADJ_BTN_TYPE_14);
    lv_obj_clear_flag(cct_panel, LV_OBJ_FLAG_SCROLLABLE);  //取消滚动效果
    adj_cct_widget_creat(cct_panel, 150, p_fireworks_model->type, &btn_list[1], 0);
    lv_obj_set_size(cct_panel, INT_BTN_WIDTH, INT_BTN_HEIGHT);
    lv_obj_align_to(cct_panel, int_panel,  LV_ALIGN_OUT_RIGHT_MID, 8, 0); 
    lv_obj_set_user_data(cct_panel, (void *)&btn_list[1]);
    lv_obj_add_event_cb(cct_panel, adj_btn_event_cb, LV_EVENT_ALL, NULL); 
    lv_group_add_obj(indev_group, cct_panel);
    lv_obj_t* label_obj = lv_obj_get_child(cct_panel, 2);
    lv_obj_t* img = lv_obj_get_child(cct_panel, 0);
    lv_obj_t* img1 = lv_obj_get_child(cct_panel, 1);
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
        lv_label_set_long_mode(cct_label_panel[i], LV_LABEL_LONG_WRAP);
    }
    lv_obj_set_style_text_color(cct_label_panel[p_fireworks_model->type], lv_color_white(), 0);
    if(p_fireworks_model->type == 2)
    {
        lv_obj_add_flag(img1, LV_OBJ_FLAG_HIDDEN);  //隐藏起来
        effects_refresh_scale_img(cct_panel, 3200, UI_CCT_MIN, UI_CCT_MAX);
    }
    else if(p_fireworks_model->type == 0)
    {
        lv_obj_clear_flag(img1, LV_OBJ_FLAG_HIDDEN);  //取消隐藏
        effects_refresh_scale_img(cct_panel, 3200, UI_CCT_MIN, UI_CCT_MAX);
    }
    else if(p_fireworks_model->type == 1)
    {
        lv_obj_clear_flag(img1, LV_OBJ_FLAG_HIDDEN);  //取消隐藏
        effects_refresh_scale_img(cct_panel, 5600, UI_CCT_MIN, UI_CCT_MAX);
    }
    
    lv_obj_t* frq_panel = adj_btn_create(parent, LIGHT_EFFECT_COMMON_RADIUS, ADJ_BTN_TYPE_24);
    lv_obj_set_size(frq_panel, INT_BTN_WIDTH, INT_BTN_HEIGHT);
    lv_obj_align_to(frq_panel, cct_panel,  LV_ALIGN_OUT_RIGHT_MID, 8, 0); 
    adj_frq_widget_creat(frq_panel, 54, p_fireworks_model->frq, &btn_list[2]);
    lv_obj_set_user_data(frq_panel, (void *)&btn_list[2]);
    lv_obj_add_event_cb(frq_panel, adj_btn_event_cb, LV_EVENT_ALL, NULL);       
    lv_group_add_obj(indev_group, frq_panel);     

    state_panel = adj_state_widget_creat(parent,&p_fireworks_model->state, 288, 50, ctrl_btn_update_cb);
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
static void page_fireworks_construct(void)
{
    lv_obj_set_style_bg_color(lv_scr_act(), lv_color_black(), 0);

    lv_obj_t* t_cont = lv_obj_create(lv_scr_act());
    lv_obj_remove_style_all(t_cont);
    lv_obj_set_size(t_cont, lv_pct(100), lv_pct(12));
    lv_obj_set_style_text_color(t_cont, lv_color_white(), 0);
    lv_obj_set_pos(t_cont, 0, 0);

    light_effect_title_create(t_cont, Lang_GetStringByID(STRING_ID_FIREWORKS));
    
    lv_obj_t* b_cont = lv_obj_create(lv_scr_act());
    lv_obj_remove_style_all(b_cont);
    lv_obj_set_size(b_cont, lv_pct(100), lv_pct(88));
    lv_obj_align_to(b_cont, t_cont, LV_ALIGN_OUT_BOTTOM_MID, 0, 0);
    page_fireworks_init(b_cont);
    
    uint8_t id = PAGE_FIREWORKS;
    uint8_t sub_id = 0;
    data_center_write_config_data_no_event(SYS_CONFIG_LOCAL_UI, &id);
    data_center_write_config_data_no_event(SYS_CONFIG_LOCAL_SUB_UI, &sub_id);
	data_center_write_config_data_no_event(SYS_CONFIG_EFFECTS_MODE, &id);
}

static void page_fireworks_destruct(void)
{
    lv_group_focus_freeze(indev_group, true);
    lv_group_remove_all_objs(indev_group);
    lv_group_del(indev_group);
    indev_group = NULL;

    lv_obj_clean(lv_scr_act());
}

static void page_fireworks_time_upd_cb(uint32_t ms)
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
		p_fireworks_model->state = 1;
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
            struct db_fx_fireworks fireworks;
            
            page_set_invalid_type(PAGE_INVALID_TYPE_IDLE);
            data_center_read_light_data(LIGHT_MODE_FX_FIREWORKS, &fireworks); 
            memcpy(p_fireworks_model, &fireworks, sizeof(fireworks));
            force_upd = type == PAGE_INVALID_TYPE_ALL ? true : false;
        }
        break;
    }   
    lv_obj_t* fucosed_obj = lv_group_get_focused(indev_group);
    if (lv_group_get_editing(indev_group))    //如果当前正在处于编辑状态，那么退出编辑状态。
    {
        lv_event_send(fucosed_obj, LV_EVENT_RELEASED, NULL);
    }

    
    state_btn_t *state_user_data = lv_obj_get_user_data(state_panel);
    if (state_user_data->cur_index != p_fireworks_model->state || force_upd)    //更新触发按键
    {
        bool focused = fucosed_obj == state_user_data->btn ? true : false;
        if (p_fireworks_model->state > state_user_data->max)
        {
            p_fireworks_model->state = state_user_data->max;
        }
                
        state_user_data->cur_index = p_fireworks_model->state;
        state_user_data->widget_refresh_cb(state_user_data->btn, focused, false);          
    }
    
    //state_btn_refresh(state_btn, false, false);
    if (list_ext.list != NULL)    //更新滚动列表
    {
        int8_t selected = (uint8_t)list_ext.selected; 
        if (selected != p_fireworks_model->type || force_upd)
        {            
            list_ext.selected = (int8_t)p_fireworks_model->type;
            lv_event_send(list_ext.list, LV_EVENT_VALUE_CHANGED, NULL);            
        }
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
static void page_fireworks_event_cb(int event)
{
    struct db_fx_fireworks fireworks;
    uint32_t src_crc32, dest_crc32;
	uint8_t type;
	
    data_center_read_light_data(LIGHT_MODE_FX_FIREWORKS, &fireworks);   
    dest_crc32 = ui_calc_checksum((uint8_t*)&fireworks,  sizeof(struct db_fx_fireworks));    
    switch (event)
    {
        case EVENT_DATA_WRITE:
        {
            src_crc32 = ui_calc_checksum((uint8_t*)p_fireworks_model, sizeof(struct db_fx_fireworks));            
            if (dest_crc32 != src_crc32)
            {
                data_center_write_light_data(LIGHT_MODE_FX_FIREWORKS, p_fireworks_model);
            }
        }
        break;
        case EVENT_DATA_PULL:
        {
            data_center_read_light_data(LIGHT_MODE_FX_FIREWORKS, &fireworks);   
			type = page_get_invalid_type();
			if(type == 1)  
				break;
            memcpy(p_fireworks_model, &fireworks, sizeof(struct db_fx_fireworks));
				page_set_invalid_type(PAGE_INVALID_TYPE_PORTION);
        }
        break;
        case EVENT_DATA_COMPULSION:
        {
            data_center_write_light_data(LIGHT_MODE_FX_FIREWORKS, p_fireworks_model);
        }
        break;
		case EVENT_DATA_HS_WINDOW:
		{
		
			break;
		}
    }
}


static void int_upd_run_cb(void* data)
{
    adj_btn_t* dsc = (adj_btn_t*)data;
    p_fireworks_model->lightness = dsc->value;
    page_event_send(EVENT_DATA_WRITE);
}

static void cct_upd_run_cb(void* data)
{
    adj_btn_t* dsc = (adj_btn_t*)data;
	(void)dsc;
    p_fireworks_model->type = dsc->value;
    if(p_fireworks_model->type > dsc->max)
        p_fireworks_model->type = dsc->max;
    else if(p_fireworks_model->type < dsc->min)
        p_fireworks_model->type = dsc->min;
    lv_obj_t* img1 = lv_obj_get_child(cct_panel, 1);
    for(uint8_t i =0 ;i < 3; i++)
    {
        lv_obj_set_style_text_color(cct_label_panel[i], lv_color_make(132,132,132), 0);
    }
    lv_obj_set_style_text_color(cct_label_panel[p_fireworks_model->type], lv_color_white(), 0);
    if(p_fireworks_model->type == 2)
    {
        lv_obj_add_flag(img1, LV_OBJ_FLAG_HIDDEN);  //隐藏起来
        effects_refresh_scale_img(cct_panel, 3200, UI_CCT_MIN, UI_CCT_MAX);
    }
    else if(p_fireworks_model->type == 0)
    {
        lv_obj_clear_flag(img1, LV_OBJ_FLAG_HIDDEN);  //取消隐藏
        effects_refresh_scale_img(cct_panel, 3200, UI_CCT_MIN, UI_CCT_MAX);
    }
    else if(p_fireworks_model->type == 1)
    {
        lv_obj_clear_flag(img1, LV_OBJ_FLAG_HIDDEN);  //取消隐藏
        effects_refresh_scale_img(cct_panel, 5600, UI_CCT_MIN, UI_CCT_MAX);
    }
//	if(p_fireworks_model->type == 0)
//		p_fireworks_model->type = 2;
//	else if(p_fireworks_model->type == 1)
//		p_fireworks_model->type = 0;
//	else if(p_fireworks_model->type == 2)
//		p_fireworks_model->type = 1;
	
    page_event_send(EVENT_DATA_WRITE);
}

//static void spd_upd_run_cb(void* data)
//{
//    adj_btn_t* dsc= (adj_btn_t*)data;
//    p_fireworks_model->frq = (uint8_t)dsc->value;    
//}

static bool int_assign_value_cb(void* data)
{
    adj_btn_t* dsc = (adj_btn_t*)data;
    ASSIGN_VALUE_TO_BTN(p_fireworks_model->lightness);
}

static bool cct_assign_value_cb(void* data)
{
    adj_btn_t* dsc = (adj_btn_t*)data;
	ASSIGN_VALUE_TO_BTN(p_fireworks_model->type);
}

//static bool spd_assign_value_cb(void* data)
//{
//    adj_btn_t* dsc = (adj_btn_t*)data;
//    ASSIGN_VALUE_TO_BTN(p_fireworks_model->frq);
//}

static void ctrl_btn_update_cb(void *data)
{
    state_btn_t *dsc = (state_btn_t*)data;
	uint8_t hsmode;
	
	data_center_read_config_data(SYS_CONFIG_HIGH_SPEED_MODE, &hsmode);
	if(hsmode == 0)
	{
		p_fireworks_model->state = (uint8_t)dsc->cur_index;  
		page_event_send(EVENT_DATA_WRITE);   
	}
	set_hs_fx_window();	
}

//static void roller_value_changed_cb(uint32_t value)
//{
//    p_fireworks_model->type = value;
//}

static void frq_upd_run_cb(void* data)
{
    adj_btn_t* dsc = (adj_btn_t*)data;
    p_fireworks_model->frq = (uint8_t)dsc->value;
    page_event_send(EVENT_DATA_WRITE);
}

static bool frq_assign_value_cb(void* data)
{
    adj_btn_t* dsc = (adj_btn_t*)data;
    ASSIGN_VALUE_TO_BTN(p_fireworks_model->frq);
}
#endif

