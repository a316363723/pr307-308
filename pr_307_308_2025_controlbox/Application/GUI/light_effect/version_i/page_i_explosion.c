

/*********************
 *      INCLUDES
 *********************/
#include "ui_common.h"
#include "page_widget.h"

#if (PROJECT_TYPE == 308)
/*********************
 *      DEFINES
 *********************/

#define DIVIDE_SPACE                  3
#define INT_BTN_WIDTH           90
#define INT_BTN_HEIGHT          136

//#define LIGHT_MODE_ROLLER_WIDTH     136
//#define LIGHT_MODE_ROLLER_HEIGHT    63

//#define LIGHT_MODE_PARAM_WIN_WIDTH  162
//#define LIGHT_MODE_PARAM_WIN_HEIGHT 120

//#define DECAY_BTN_WIDTH             136
//#define DECAY_BTN_HEIGHT            74

//#define TRIGGER_BTN_WIDTH           162
//#define TRIGGER_BTN_HEIGHT          74

//#define ARC_INDICATOR_COLOR                      lv_color_make(255, 255, 255)
//#define ARC_INDICATOR_BG_COLOR                   lv_color_make(106, 103, 103)
//#define LOADING_IMG_COLOR                        lv_color_make(255, 255, 255)
/*********************
 *      TPEDEFS
 *********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void int_upd_run_cb(void* data);
static void cct_upd_run_cb(void* data);
static void frq_upd_run_cb(void* data);
static bool int_assign_value_cb(void* data);
static bool cct_assign_value_cb(void* data);
static bool frq_assign_value_cb(void* data);

//static void ctrl_btn_update_cb(void *data);

static void trigger_btn_release_cb(void);
static void trigger_btn_anim_cb(lv_anim_t* a);

static void page_explosion_construct(void);
static void page_explosion_destruct(void);
static void page_explosion_time_upd_cb(uint32_t ms);
static void page_explosion_event_cb(int event);
/*********************
 *  STATIC VATIABLES
 *********************/
static lv_group_t *indev_group;
static struct db_fx_explosion  *p_explosion_model;
static light_mode_roller_ext_t roller_ext;
static lv_obj_t* cct_panel;

static adj_btn_t btn_list[] = {
    ADJ_BTN_INITIALIZER(0, 1000, 1, 500, "I NT", "%", 3, 55, -3,false, false,  intensity_get_value_str, int_upd_run_cb, int_assign_value_cb, adj_btn4_refresh, common_intensity_get_step),
    ADJ_BTN_INITIALIZER(UI_CCT_MIN, UI_CCT_MAX, 50, UI_CCT_MIN, "CCT", "K", 3, 41, -3, false, false,  cct_get_value_str, cct_upd_run_cb, cct_assign_value_cb, adj_cct_refresh, NULL),         
    ADJ_BTN_INITIALIZER(FREQUENCY_LEVEL_1, FREQUENCY_LEVEL_R, 1, DECAY_LEVEL_R, "Decay", "", 3, 78, -3,false, false,  frequence_get_value_str, frq_upd_run_cb, frq_assign_value_cb, adj_btn5_refresh, NULL),       
};
//static state_btn_t state_btn_dsc;
static trigger_btn_t trigger_btn_dsc;
/*********************
 *  GLOBAL VATIABLES
 *********************/
page_t page_explosion = PAGE_INITIALIZER("Explosion",
                                PAGE_EXPLOSIOIN, 
                                page_explosion_construct, 
                                page_explosion_destruct, 
                                page_explosion_time_upd_cb,
                                page_explosion_event_cb,                                
                                PAGE_LIGHT_EFFECT_MENU);
/*********************
 *  GLOBAL FUNCTIONS
 *********************/
//static void ctrl_btn_update_cb(void *data)
//{
//    state_btn_t *dsc = (state_btn_t*)data;
//    p_explosion_model->state = (uint8_t)dsc->cur_index;    
//}

static void page_explosion_init(lv_obj_t* parent)
{    
	uint8_t hsmode;
	data_center_read_config_data(SYS_CONFIG_HIGH_SPEED_MODE, &hsmode);
	if(hsmode)
	{
		g_tUIAllData.clublights_model.state = 0;
	}
    p_explosion_model = (struct db_fx_explosion *)screen_get_act_page_data();
    indev_group = lv_group_create();  
    page_event_send(EVENT_DATA_PULL);

    my_indev_set_group(indev_group);
    lv_group_set_default(indev_group);
    lv_obj_set_style_bg_color(parent, lv_color_black(), 0);
    
    lv_obj_t* int_panel = adj_btn_create(parent, LIGHT_EFFECT_COMMON_RADIUS, ADJ_BTN_TYPE_14);
    lv_obj_set_size(int_panel, INT_BTN_WIDTH, INT_BTN_HEIGHT);
    lv_obj_align(int_panel, LV_ALIGN_TOP_LEFT, 16, 2);
    adj_int_widget_creat(int_panel, 70, 78, p_explosion_model->lightness, &btn_list[0]);
    lv_obj_set_user_data(int_panel, (void *)&btn_list[0]);
    lv_obj_add_event_cb(int_panel, adj_btn_event_cb, LV_EVENT_ALL, NULL);       
    lv_group_add_obj(indev_group, int_panel);
    
    cct_panel = adj_btn_create(parent, LIGHT_EFFECT_COMMON_RADIUS, ADJ_BTN_TYPE_14);
    lv_obj_clear_flag(cct_panel, LV_OBJ_FLAG_SCROLLABLE);  //取消滚动效果
    adj_cct_widget_creat(cct_panel, 150, p_explosion_model->mode_arg.cct.cct, &btn_list[1], 0);
    lv_obj_set_size(cct_panel, INT_BTN_WIDTH, INT_BTN_HEIGHT);
    lv_obj_align_to(cct_panel, int_panel,  LV_ALIGN_OUT_RIGHT_MID, 8, 0); 
    lv_obj_set_user_data(cct_panel, (void *)&btn_list[1]);
    lv_obj_add_event_cb(cct_panel, adj_btn_event_cb, LV_EVENT_ALL, NULL); 
    lv_group_add_obj(indev_group, cct_panel);
    effects_refresh_scale_img(cct_panel, p_explosion_model->mode_arg.cct.cct, UI_CCT_MIN, UI_CCT_MAX);
    
    lv_obj_t* frq_panel = adj_btn_create(parent, LIGHT_EFFECT_COMMON_RADIUS, ADJ_BTN_TYPE_24);
    lv_obj_set_size(frq_panel, INT_BTN_WIDTH, INT_BTN_HEIGHT);
    lv_obj_align_to(frq_panel, cct_panel,  LV_ALIGN_OUT_RIGHT_MID, 8, 0); 
    adj_decay_widget_creat(frq_panel, 54, p_explosion_model->frq, &btn_list[2]);
    lv_obj_set_user_data(frq_panel, (void *)&btn_list[2]);
    lv_obj_add_event_cb(frq_panel, adj_btn_event_cb, LV_EVENT_ALL, NULL);       
    lv_group_add_obj(indev_group, frq_panel);   

    trigger_btn_dsc.img_to_top = -5;
    trigger_btn_dsc.title_to_top = 5;
    trigger_btn_dsc.released_cb = trigger_btn_release_cb;
    lv_obj_t* trigger_panel = adj_trigger_widget_creat(parent, 288, 50, &trigger_btn_dsc);  
    lv_obj_align(trigger_panel,  LV_ALIGN_BOTTOM_MID, 0, -9);   
    lv_group_add_obj(indev_group, trigger_panel);     
    
    lv_group_user_focus_obj_by_id(indev_group, screen_get_act_index());
    screen_set_act_index(0); 
    lv_group_set_wrap(indev_group, false);
    
    page_event_send(EVENT_DATA_COMPULSION);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/
static void page_explosion_construct(void)
{
    lv_obj_set_style_bg_color(lv_scr_act(), lv_color_black(), 0);

    lv_obj_t* t_cont = lv_obj_create(lv_scr_act());
    lv_obj_remove_style_all(t_cont);
    lv_obj_set_size(t_cont, lv_pct(100), lv_pct(12));
    lv_obj_set_style_text_color(t_cont, lv_color_white(), 0);
    lv_obj_set_pos(t_cont, 0, 0);

    light_effect_title_create(t_cont, Lang_GetStringByID(STRING_ID_EXPLOSIONS_I));

    lv_obj_t* b_cont = lv_obj_create(lv_scr_act());
    lv_obj_remove_style_all(b_cont);
    lv_obj_set_size(b_cont, lv_pct(100), lv_pct(88));
    lv_obj_align_to(b_cont, t_cont, LV_ALIGN_OUT_BOTTOM_MID, 0, 0);
    page_explosion_init(b_cont);
    
    uint8_t id = PAGE_EXPLOSIOIN;
    uint8_t sub_id = 0;
    data_center_write_config_data_no_event(SYS_CONFIG_LOCAL_UI, &id);
    data_center_write_config_data_no_event(SYS_CONFIG_LOCAL_SUB_UI, &sub_id);
	data_center_write_config_data_no_event(SYS_CONFIG_EFFECTS_MODE, &id);
}

static void page_explosion_destruct(void)
{
    lv_group_focus_freeze(indev_group, true);
    lv_group_remove_all_objs(indev_group);
    lv_group_del(indev_group);
    indev_group = NULL;

    lv_anim_del_all();
    lv_obj_clean(lv_scr_act());
    
    memset(&trigger_btn_dsc, 0, sizeof(trigger_btn_dsc));
}

static void page_explosion_time_upd_cb(uint32_t ms)
{
    page_invalid_type_t type = page_get_invalid_type();
    bool force_upd = false;
    
    //update the title bar.
    lv_obj_t* t_cont = lv_obj_get_child(lv_scr_act(), 0);
    (void)t_cont;

//    page_event_send(EVENT_DATA_WRITE);

	if(get_hs_fx_window_state())
	{
		hs_fx_window_state_set(0);
		p_explosion_model->state = 1;
		type = PAGE_INVALID_TYPE_REFRESH;
	}
	
    switch(type)
    {
        case PAGE_INVALID_TYPE_IDLE:    return;
		case PAGE_INVALID_TYPE_REFRESH:
			force_upd = true;
			if(p_explosion_model->state == 1)
			{
				ui_set_explosion_2_anim(1);
				p_explosion_model->state = 1;
				p_explosion_model->trigger = 1;
				p_explosion_model->mode = FX_MODE_CCT;
				page_event_send(EVENT_DATA_WRITE);
			}
			page_set_invalid_type(PAGE_INVALID_TYPE_IDLE);
		break;
        case PAGE_INVALID_TYPE_PORTION:
        case PAGE_INVALID_TYPE_ALL:
        {
            struct db_fx_explosion explosion;
            
            page_set_invalid_type(PAGE_INVALID_TYPE_IDLE);
            data_center_read_light_data(LIGHT_MODE_FX_EXPLOSION, &explosion); 
            memcpy(p_explosion_model, &explosion, sizeof(explosion));
            force_upd = type == PAGE_INVALID_TYPE_ALL ? true : false;
        }
        break;
    }   
    lv_obj_t* fucosed_obj = lv_group_get_focused(indev_group);
    if (lv_group_get_editing(indev_group)) //如果当前正在处于编辑状态，那么退出编辑状态。
    {
        lv_event_send(fucosed_obj, LV_EVENT_RELEASED, NULL);
    }

    if (roller_ext.obj != NULL)
    {
        if (roller_ext.type != p_explosion_model->mode || force_upd)
        {
            roller_ext.type = p_explosion_model->mode;    
            lv_roller_set_selected(roller_ext.obj, roller_ext.type, LV_ANIM_OFF);            
        }
        lv_event_send(roller_ext.obj, LV_EVENT_VALUE_CHANGED, NULL);
    }

    if (g_ptUIStateData->trigger_explosion_anim)
    {
        g_ptUIStateData->trigger_explosion_anim = 0;
        lv_obj_add_state(trigger_btn_dsc.btn, LV_STATE_FOCUSED);
        lv_event_send(trigger_btn_dsc.btn, LV_EVENT_PRESSED, NULL);

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
static void page_explosion_event_cb(int event)
{
    struct db_fx_explosion explosion;
	uint8_t type;
//    uint32_t src_crc32, dest_crc32;

//    data_center_read_light_data(LIGHT_MODE_FX_EXPLOSION, &explosion);   
//    dest_crc32 = ui_calc_checksum((uint8_t*)&explosion,  sizeof(struct db_fx_explosion));    
    switch (event)
    {
        case EVENT_DATA_WRITE:
        {
//            src_crc32 = ui_calc_checksum((uint8_t*)p_explosion_model, sizeof(struct db_fx_explosion));            
//            if (dest_crc32 != src_crc32)
//            {
			
                data_center_write_light_data(LIGHT_MODE_FX_EXPLOSION, p_explosion_model);
//            }
        }
        break;
        case EVENT_DATA_PULL:
        {
			type = page_get_invalid_type();
			if(type == 1)  
				break;
            data_center_read_light_data(LIGHT_MODE_FX_EXPLOSION, &explosion); 
            memcpy(p_explosion_model, &explosion, sizeof(struct db_fx_explosion));
            p_explosion_model->mode = FX_MODE_CCT;
			page_set_invalid_type(PAGE_INVALID_TYPE_PORTION);
        }
        break;
        case EVENT_DATA_COMPULSION:
        {
            data_center_write_light_data(LIGHT_MODE_FX_EXPLOSION, p_explosion_model);
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
    p_explosion_model->mode = FX_MODE_CCT;
}

static void int_upd_run_cb(void* data)
{
    adj_btn_t* dsc = (adj_btn_t*)data;
    p_explosion_model->lightness = dsc->value;
	p_explosion_model->state = 0;
    p_explosion_model->mode = FX_MODE_CCT;
	if(p_explosion_model->trigger == 1)
		page_event_send(EVENT_DATA_WRITE);
}

static void cct_upd_run_cb(void* data)
{
    adj_btn_t* dsc = (adj_btn_t*)data;
    p_explosion_model->mode_arg.cct.cct = dsc->value;
	p_explosion_model->state = 0;
    effects_refresh_scale_img(cct_panel, p_explosion_model->mode_arg.cct.cct, UI_CCT_MIN, UI_CCT_MAX);
    p_explosion_model->mode = FX_MODE_CCT;
	if(p_explosion_model->trigger == 1)
		page_event_send(EVENT_DATA_WRITE);
}

static void frq_upd_run_cb(void* data)
{
    adj_btn_t* dsc = (adj_btn_t*)data;
    p_explosion_model->frq = (uint8_t)dsc->value;
    p_explosion_model->mode = FX_MODE_CCT;
	p_explosion_model->state = 0;
	if(p_explosion_model->trigger == 1)
		page_event_send(EVENT_DATA_WRITE);
}

static bool int_assign_value_cb(void* data)
{
    adj_btn_t* dsc = (adj_btn_t*)data;
    ASSIGN_VALUE_TO_BTN(p_explosion_model->lightness);
}

static bool cct_assign_value_cb(void* data)
{
    adj_btn_t* dsc = (adj_btn_t*)data;
    ASSIGN_VALUE_TO_BTN(p_explosion_model->mode_arg.cct.cct);
}

static bool frq_assign_value_cb(void* data)
{
    adj_btn_t* dsc = (adj_btn_t*)data;
    ASSIGN_VALUE_TO_BTN(p_explosion_model->frq);
}

static void trigger_btn_release_cb(void)
{
	uint8_t hsmode;
	
	data_center_read_config_data(SYS_CONFIG_HIGH_SPEED_MODE, &hsmode);
	if(hsmode == 0)
	{
		p_explosion_model->trigger = 1;
		p_explosion_model->state = 1;
		p_explosion_model->mode = FX_MODE_CCT;
		if(g_ptUIStateData->trigger_explosion_anim_state == 1)
			g_ptUIStateData->trigger_explosion_anim_state = 0;
		else
			page_event_send(EVENT_DATA_WRITE);
		if(ui_get_master_mode())
			gui_data_sync_event_gen();
	}
	set_hs_fx_window();
    UI_PRINTF("SINGLGE_BTN_CLICKED_CB\r\n");
}
#endif

