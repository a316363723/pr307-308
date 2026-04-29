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

//#define GM_BTN_WIDTH            149
//#define GM_BTN_HEIGHT           43

//#define CCT_BTN_WIDTH           301
//#define CCT_BTN_HEIGHT          64

//#define SPD_BTN_WIDTH           136
//#define SPD_BTN_HEIGHT          35

//#define FRQ_BTN_WIDTH           136
//#define FRQ_BTN_HEIGHT          35

//#define STATE_BTN_WIDTH         77
//#define STATE_BTN_HEIGHT        74

//#define TRIGGER_BTN_WIDTH       81
//#define TRIGGER_BTN_HEIGHT      74

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
//static void spd_upd_run_cb(void* data);
static void cct_upd_run_cb(void* data);
static void frq_upd_run_cb(void* data);
//static void gm_upd_run_cb(void* data);

static bool cct_assign_value_cb(void* data);
static bool int_assign_value_cb(void* data);
//static bool spd_assign_value_cb(void* data);
static bool frq_assign_value_cb(void* data);
//static bool gm_assign_value_cb(void* data);

static void ctrl_btn_update_cb(void *data);
//static void btnlist_update_cb(void *data);
//static void btnlist_release_cb(void* data);
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
    ADJ_BTN_INITIALIZER(0, 1000, 1, 500, "I NT", "%", 3, 55, -3,false, false,  intensity_get_value_str, int_upd_run_cb, int_assign_value_cb, adj_btn4_refresh, common_intensity_get_step), 
    ADJ_BTN_INITIALIZER(UI_CCT_MIN, UI_CCT_MAX, 50, UI_CCT_MIN, "CCT", "K", 3, 41, -3, false, false,  cct_get_value_str, cct_upd_run_cb, cct_assign_value_cb, adj_cct_refresh, NULL),  
    ADJ_BTN_INITIALIZER(FREQUENCY_LEVEL_1, FREQUENCY_LEVEL_R, 1, DECAY_LEVEL_R, "Frq", "", 3, 78, -3,false, false,  frequence_get_value_str, frq_upd_run_cb, frq_assign_value_cb, adj_btn5_refresh, NULL),       
};

static lv_group_t *indev_group;
static struct db_fx_lightning *p_lightning_model;
//static lv_obj_t* disp_panel;
//static state_btn_t state_btn_dsc;
//static my_btnlist_ext_t btnlist_ext;
static trigger_btn_t trigger_btn_dsc;
static lv_obj_t* state_panel;
static lv_obj_t* cct_panel;
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
	uint8_t hsmode;
	data_center_read_config_data(SYS_CONFIG_HIGH_SPEED_MODE, &hsmode);
	if(hsmode)
	{
		g_tUIAllData.clublights_model.state = 0;
	}
    p_lightning_model = (struct db_fx_lightning*)screen_get_act_page_data();
    indev_group = lv_group_create();   
    page_event_send(EVENT_DATA_PULL);    
    my_indev_set_group(indev_group);
    lv_group_set_default(indev_group);

    lv_obj_set_style_bg_color(parent, lv_color_black(), 0);
    
    lv_obj_t* int_panel = adj_btn_create(parent, LIGHT_EFFECT_COMMON_RADIUS, ADJ_BTN_TYPE_14);
    lv_obj_set_size(int_panel, INT_BTN_WIDTH, INT_BTN_HEIGHT);
    lv_obj_align(int_panel, LV_ALIGN_TOP_LEFT, 16, 2);
    adj_int_widget_creat(int_panel, 70, 78, p_lightning_model->lightness, &btn_list[0]);
    lv_obj_set_user_data(int_panel, (void *)&btn_list[0]);
    lv_obj_add_event_cb(int_panel, adj_btn_event_cb, LV_EVENT_ALL, NULL);       
    lv_group_add_obj(indev_group, int_panel);
    
    cct_panel = adj_btn_create(parent, LIGHT_EFFECT_COMMON_RADIUS, ADJ_BTN_TYPE_14);
    lv_obj_clear_flag(cct_panel, LV_OBJ_FLAG_SCROLLABLE);  //取消滚动效果
    adj_cct_widget_creat(cct_panel, 150, p_lightning_model->cct, &btn_list[1], 0);
    lv_obj_set_size(cct_panel, INT_BTN_WIDTH, INT_BTN_HEIGHT);
    lv_obj_align_to(cct_panel, int_panel,  LV_ALIGN_OUT_RIGHT_MID, 8, 0); 
    lv_obj_set_user_data(cct_panel, (void *)&btn_list[1]);
    lv_obj_add_event_cb(cct_panel, adj_btn_event_cb, LV_EVENT_ALL, NULL); 
    lv_group_add_obj(indev_group, cct_panel);
    effects_refresh_scale_img(cct_panel, p_lightning_model->cct, UI_CCT_MIN, UI_CCT_MAX);
    
    lv_obj_t* frq_panel = adj_btn_create(parent, LIGHT_EFFECT_COMMON_RADIUS, ADJ_BTN_TYPE_24);
    lv_obj_set_size(frq_panel, INT_BTN_WIDTH, INT_BTN_HEIGHT);
    lv_obj_align_to(frq_panel, cct_panel,  LV_ALIGN_OUT_RIGHT_MID, 8, 0); 
    adj_frq_widget_creat(frq_panel, 54, p_lightning_model->frq, &btn_list[2]);
    lv_obj_set_user_data(frq_panel, (void *)&btn_list[2]);
    lv_obj_add_event_cb(frq_panel, adj_btn_event_cb, LV_EVENT_ALL, NULL);       
    lv_group_add_obj(indev_group, frq_panel);      


    state_panel = adj_state_widget_creat(parent,&p_lightning_model->state, 140, 50, ctrl_btn_update_cb);
    lv_obj_align_to(state_panel,parent,  LV_ALIGN_BOTTOM_LEFT, 16, -5);       
    lv_group_add_obj(indev_group, state_panel); 
    lv_obj_add_event_cb(state_panel, state_btn_event, LV_EVENT_ALL, NULL);      
    
    
    trigger_btn_dsc.img_to_top = -5;
    trigger_btn_dsc.title_to_top = 5;
    trigger_btn_dsc.released_cb = trigger_btn_release_cb;
    lv_obj_t* trigger_panel = adj_trigger_widget_creat(parent,140, 50, &trigger_btn_dsc);  
    lv_obj_align_to(trigger_panel, state_panel,  LV_ALIGN_OUT_RIGHT_MID, 8, 0);    
    lv_group_add_obj(indev_group, trigger_panel); 
   // lv_obj_add_event_cb(trigger_panel, state_btn_event, LV_EVENT_ALL, NULL);      
    
    lv_group_user_focus_obj_by_id(indev_group, screen_get_act_index());
    screen_set_act_index(0); 
    lv_group_set_wrap(indev_group, false);    
    
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
    
//    page_event_send(EVENT_DATA_WRITE);
	if(get_hs_fx_window_state())
	{
		state_btn_t *dsc = lv_obj_get_user_data(state_panel);
		hs_fx_window_state_set(0);
		if(dsc->cur_index == 1)
		{
			p_lightning_model->state = 1;
			p_lightning_model->trigger = 2;
		}
		else if(dsc->cur_index == 0)
		{
			p_lightning_model->state = 0;
			p_lightning_model->trigger = 0;
		}
		p_lightning_model->state = (uint8_t)dsc->cur_index;    
		page_event_send(EVENT_DATA_WRITE);
		type = PAGE_INVALID_TYPE_REFRESH;
	}
    switch(type)
    {
        case PAGE_INVALID_TYPE_IDLE:    return;
		case PAGE_INVALID_TYPE_REFRESH:
			force_upd = true;
			if(p_lightning_model->trigger == 1)
			{
				ui_set_Lightning_2_state(1);
				page_event_send(EVENT_DATA_WRITE);
			}
			page_set_invalid_type(PAGE_INVALID_TYPE_IDLE);
		break;
        case PAGE_INVALID_TYPE_PORTION:
        case PAGE_INVALID_TYPE_ALL:
        {
            struct db_fx_lightning lightning;
            
            page_set_invalid_type(PAGE_INVALID_TYPE_IDLE);
            data_center_read_light_data(LIGHT_MODE_FX_LIGHTNING, &lightning); 
            memcpy(p_lightning_model, &lightning, sizeof(lightning));
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
    state_btn_t *state_user_data = lv_obj_get_user_data(state_panel);
    if ((state_user_data->cur_index != p_lightning_model->trigger && p_lightning_model->trigger != 1) || force_upd)
    {
		bool focused = fucosed_obj == state_user_data->btn ? true : false;  
		if(p_lightning_model->trigger == 2)
			state_user_data->cur_index = 1; 
		if(p_lightning_model->trigger == 0)	
			state_user_data->cur_index = 0; 	
		state_user_data->widget_refresh_cb(state_user_data->btn, focused, false);
    }

    if (g_ptUIStateData->trigger_light_anim)
    {
        g_ptUIStateData->trigger_light_anim = 0;
        lv_obj_add_state(trigger_btn_dsc.btn, LV_STATE_FOCUSED);
        lv_event_send(trigger_btn_dsc.btn, LV_EVENT_PRESSED, NULL);
		
		state_user_data->cur_index = 0;     
		p_lightning_model->trigger = 0;
		state_user_data->widget_refresh_cb(state_user_data->btn, true, false);
		
        lv_anim_t a;
        lv_anim_init(&a);
        lv_anim_set_time(&a, 150);
        lv_anim_set_var(&a, &trigger_btn_dsc);
        lv_anim_set_ready_cb(&a, trigger_btn_anim_cb);
        lv_anim_start(&a);        
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
    struct db_fx_lightning lightning;
	uint8_t type;
//    uint32_t src_crc32, dest_crc32;

//    data_center_read_light_data(LIGHT_MODE_FX_LIGHTNING, &lightning);   
//    dest_crc32 = ui_calc_checksum((uint8_t*)&lightning,  sizeof(struct db_fx_lightning));    
    switch (event)
    {
        case EVENT_DATA_WRITE:
        {
//            src_crc32 = ui_calc_checksum((uint8_t*)p_lightning_model, sizeof(struct db_fx_lightning));            
//            if (dest_crc32 != src_crc32)
//            {
                data_center_write_light_data(LIGHT_MODE_FX_LIGHTNING, p_lightning_model);
//            }
        }
        break;
        case EVENT_DATA_PULL:
        {
            data_center_read_light_data(LIGHT_MODE_FX_LIGHTNING, &lightning);  
			type = page_get_invalid_type();
			if(type == 1)  
				break;
            memcpy(p_lightning_model, &lightning, sizeof(struct db_fx_lightning));
			page_set_invalid_type(PAGE_INVALID_TYPE_PORTION);
        }
        break;
        case EVENT_DATA_COMPULSION:
        {
            data_center_write_light_data(LIGHT_MODE_FX_LIGHTNING, p_lightning_model);
        }
        break;
		case EVENT_DATA_HS_WINDOW:
		{
		
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
	if(p_lightning_model->trigger == 1)
	{
		p_lightning_model->trigger = 0;
	}
	else  if(p_lightning_model->trigger == 2)
	{
		page_event_send(EVENT_DATA_WRITE);
	}
}

//static void spd_upd_run_cb(void* data)
//{
//    adj_btn_t* dsc = (adj_btn_t*)data;
//    p_lightning_model->spd = (uint8_t)dsc->value;
//}

static void frq_upd_run_cb(void* data)
{
    adj_btn_t* dsc = (adj_btn_t*)data;
    p_lightning_model->frq = (uint8_t)dsc->value;
	if(p_lightning_model->trigger == 1)
	{
		p_lightning_model->trigger = 0;
	}
	else if(p_lightning_model->trigger == 2)
	{
		page_event_send(EVENT_DATA_WRITE);
	}
}

static void cct_upd_run_cb(void* data)
{
    adj_btn_t* dsc = (adj_btn_t*)data;
	(void)dsc;
    p_lightning_model->cct = dsc->value;
    effects_refresh_scale_img(cct_panel, p_lightning_model->cct, UI_CCT_MIN, UI_CCT_MAX);
	if(p_lightning_model->trigger == 1)
	{
		p_lightning_model->trigger = 0;
	}
	else  if(p_lightning_model->trigger == 2)
	{
		page_event_send(EVENT_DATA_WRITE);
	}
}

//static void gm_upd_run_cb(void* data)
//{
//    adj_btn_t* dsc = (adj_btn_t*)data;
//	(void)dsc;
//    //p_lightning_model->gm = (int8_t)dsc->value;
//}

static bool int_assign_value_cb(void* data)
{
	bool res = true;
    adj_btn_t* dsc = (adj_btn_t*)data;
    ASSIGN_VALUE_TO_BTN(p_lightning_model->lightness);
	
	return res;
}

//static bool spd_assign_value_cb(void* data)
//{
//    adj_btn_t* dsc = (adj_btn_t*)data;
//    ASSIGN_VALUE_TO_BTN(p_lightning_model->spd);
//}

static bool frq_assign_value_cb(void* data)
{
    adj_btn_t* dsc = (adj_btn_t*)data;
    ASSIGN_VALUE_TO_BTN(p_lightning_model->frq);
}

static bool cct_assign_value_cb(void* data)
{
    adj_btn_t* dsc = (adj_btn_t*)data;
	ASSIGN_VALUE_TO_BTN(p_lightning_model->cct);
}

//static bool gm_assign_value_cb(void* data)
//{
//    adj_btn_t* dsc = (adj_btn_t*)data;
//    (void)dsc;
////	ASSIGN_VALUE_TO_BTN(p_lightning_model->gm);
//}

static void ctrl_btn_update_cb(void *data)
{
    state_btn_t *dsc = (state_btn_t*)data;
	
	uint8_t hsmode;
	
	data_center_read_config_data(SYS_CONFIG_HIGH_SPEED_MODE, &hsmode);
	if(hsmode == 0)
	{
		if(dsc->cur_index == 1)
		{
			p_lightning_model->state = 1;
			p_lightning_model->trigger = 2;
		}
		else if(dsc->cur_index == 0)
		{
			p_lightning_model->state = 0;
			p_lightning_model->trigger = 0;
		}
		p_lightning_model->state = (uint8_t)dsc->cur_index;    
		page_event_send(EVENT_DATA_WRITE);
	}
    
	set_hs_fx_window();
}

//static void btnlist_update_cb(void *data)
//{
//    my_btnlist_ext_t* ext = (my_btnlist_ext_t *)data;    
//    p_lightning_model->mode = (enum fx_mode)ext->cur_index;
//    lightmode_disp_panel_refresh(ext->disp_panel, p_lightning_model->mode, &ext->setting.param[p_lightning_model->mode]);
//}

//static void btnlist_release_cb(void* data)
//{
//    uint8_t page_id = ui_param_setting_get_page(p_lightning_model->mode);
//    ui_set_param_setting(p_lightning_model->mode, &p_lightning_model->mode_arg);     
//    screen_set_act_index(lv_group_user_get_focused_id(indev_group));
//    screen_load_page(page_id, 0, true);
//}

static void trigger_btn_release_cb(void)
{
    UI_PRINTF("SINGLGE_BTN_CLICKED_CB\r\n");    

	
	p_lightning_model->trigger = 1;
	
	state_btn_t *state_user_data = lv_obj_get_user_data(state_panel);
	state_user_data->cur_index = 0;              
	state_user_data->widget_refresh_cb(state_user_data->btn, true, true);
    if(g_ptUIStateData->trigger_light_anim_state == 1)
		g_ptUIStateData->trigger_light_anim_state = 0;
	else
		page_event_send(EVENT_DATA_WRITE);
	gui_data_sync_event_gen();
	set_hs_fx_window();
}

#endif

