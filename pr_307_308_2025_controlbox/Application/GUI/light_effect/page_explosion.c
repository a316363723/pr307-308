
/*********************
 *      INCLUDES
 *********************/
#include "../ui_common.h"
#include "../page_widget.h"

#if (PROJECT_TYPE == 307)
/*********************
 *      DEFINES
 *********************/

#define DIVIDE_SPACE                  3
#define INT_BTN_WIDTH               136
#define INT_BTN_HEIGHT              53

#define LIGHT_MODE_ROLLER_WIDTH     136
#define LIGHT_MODE_ROLLER_HEIGHT    63

#define LIGHT_MODE_PARAM_WIN_WIDTH  162
#define LIGHT_MODE_PARAM_WIN_HEIGHT 120

#define DECAY_BTN_WIDTH             136
#define DECAY_BTN_HEIGHT            74

#define TRIGGER_BTN_WIDTH           162
#define TRIGGER_BTN_HEIGHT          74
/*********************
 *      TPEDEFS
 *********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void int_upd_run_cb(void* data);
static void decay_upd_run_cb(void* data);
static bool int_assign_value_cb(void* data);;
static bool decay_assign_value_cb(void* data);

//static void ctrl_btn_update_cb(void *data);
static void roller_value_change_cb(struct _light_mode_roller_ext* ext);

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
static struct db_fx_explosion_2  *p_explosion_model;
static light_mode_roller_ext_t roller_ext;
static lv_obj_t* disp_panel;
static adj_btn_t btn_list[] = {
    ADJ_BTN_INITIALIZER(0, 1000, 1, 500, "I NT", "%", 3, 55, -3,false, false,  intensity_get_value_str, int_upd_run_cb, int_assign_value_cb, NULL, common_intensity_get_step),  
    ADJ_BTN_INITIALIZER(DECAY_LEVEL_1, DECAY_LEVEL_R, 1, DECAY_LEVEL_R, "Decay", "", 3, 78, -3,false, false,  frequence_get_value_str, decay_upd_run_cb, decay_assign_value_cb, NULL, NULL),       
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
void page_explosion_init(lv_obj_t* parent)
{
	uint8_t hsmode;
	data_center_read_config_data(SYS_CONFIG_HIGH_SPEED_MODE, &hsmode);
	if(hsmode)
	{
		g_tUIAllData.explosion2_model.state = 0;
	}
    p_explosion_model = (struct db_fx_explosion_2 *)screen_get_act_page_data();
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
    lv_obj_align(int_cont, LV_ALIGN_TOP_LEFT, 10, 2);
    lv_group_add_obj(indev_group, int_cont);

    lv_obj_t* lm_roller = lightmode_roller_create(parent, LIGHT_MODE_ROLLER_WIDTH, LIGHT_MODE_ROLLER_HEIGHT);
    lv_obj_align_to(lm_roller, int_cont, LV_ALIGN_OUT_BOTTOM_MID, 0, DIVIDE_SPACE);
    lv_roller_set_selected(lm_roller, p_explosion_model->mode, LV_ANIM_OFF);
    //lv_group_add_obj(indev_group, lm_roller);

    disp_panel = lightmode_disp_panel_create(parent, LIGHT_MODE_PARAM_WIN_WIDTH, LIGHT_MODE_PARAM_WIN_HEIGHT);
    lv_obj_t* disp_frame = lv_obj_get_parent(disp_panel);
    lv_obj_align_to(disp_frame, int_cont, LV_ALIGN_OUT_RIGHT_TOP, DIVIDE_SPACE, 0);
    lightmode_disp_panel_refresh(disp_panel, p_explosion_model->mode, &p_explosion_model->mode_arg); 

    roller_ext.obj = lm_roller;
    roller_ext.disp_panel = disp_panel;
    roller_ext.type = p_explosion_model->mode;
    roller_ext.setting = &p_explosion_model->mode_arg;
    roller_ext.value_changed_cb = roller_value_change_cb;
    lv_obj_set_user_data(lm_roller, &roller_ext);
    lv_obj_add_event_cb(lm_roller, lightmode_roller_event_cb, LV_EVENT_ALL, NULL);

    btn_list[1].name = Lang_GetStringByID(STRING_ID_DECAY);
    lv_obj_t* decay_cont = adj_btn_create(parent, LIGHT_EFFECT_COMMON_RADIUS, ADJ_BTN_TYPE_18);
    lv_obj_set_user_data(decay_cont, (void *)&btn_list[1]);
    adj_btn_init(decay_cont, &btn_list[1], 90);    
    lv_obj_add_event_cb(decay_cont, adj_btn_event_cb, LV_EVENT_ALL, NULL);    
    lv_obj_set_size(decay_cont, DECAY_BTN_WIDTH, DECAY_BTN_HEIGHT);
    lv_obj_align_to(decay_cont, lm_roller, LV_ALIGN_OUT_BOTTOM_LEFT, 0, DIVIDE_SPACE);
    lv_group_add_obj(indev_group, decay_cont);

    LV_IMG_DECLARE(ImgPulseIcon);
    LV_IMG_DECLARE(ImgStopIcon);
    trigger_btn_dsc.img_to_top = 40;
    trigger_btn_dsc.title_to_top = 7;
    trigger_btn_dsc.released_cb = trigger_btn_release_cb;
    lv_obj_t* trigger_btn = trigger_btn_create(parent, &trigger_btn_dsc, TRIGGER_BTN_WIDTH, TRIGGER_BTN_HEIGHT);
    lv_obj_align_to(trigger_btn, decay_cont, LV_ALIGN_OUT_RIGHT_TOP, DIVIDE_SPACE, 0);
    lv_group_add_obj(indev_group, trigger_btn);
    lv_group_set_wrap(indev_group, false);
    lv_group_user_focus_obj_by_id(indev_group, screen_get_act_index());
    screen_set_act_index(0);
    
    adj_btn_refresh_all(btn_list, sizeof(btn_list) / sizeof(btn_list[0]), true);
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

    light_effect_title_create(t_cont, Lang_GetStringByID(STRING_ID_EXPLOSIONS));

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

    page_event_send(EVENT_DATA_WRITE);

    switch(type)
    {
        case PAGE_INVALID_TYPE_IDLE:    return;
        case PAGE_INVALID_TYPE_PORTION:
        case PAGE_INVALID_TYPE_ALL:
        {
			roller_ext.type = p_explosion_model->mode;    
            lv_roller_set_selected(roller_ext.obj, roller_ext.type, LV_ANIM_OFF); 
			lv_event_send(roller_ext.obj, LV_EVENT_VALUE_CHANGED, NULL);			
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
    struct db_fx_explosion_2 explosion;
    uint32_t src_crc32, dest_crc32;
	uint8_t  hsmode ;
	data_center_read_config_data(SYS_CONFIG_HIGH_SPEED_MODE, &hsmode);
    data_center_read_light_data(LIGHT_MODE_FX_EXPLOSION_II, &explosion);   
    dest_crc32 = ui_calc_checksum((uint8_t*)&explosion,  sizeof(struct db_fx_explosion_2));    
    switch (event)
    {
        case EVENT_DATA_WRITE:
        {
            src_crc32 = ui_calc_checksum((uint8_t*)p_explosion_model, sizeof(struct db_fx_explosion_2));            
            if (dest_crc32 != src_crc32)
            {
				
                 if(hsmode == 0)data_center_write_light_data(LIGHT_MODE_FX_EXPLOSION_II, p_explosion_model);
            }
        }
        break;
        case EVENT_DATA_PULL:
        {
            memcpy(p_explosion_model, &explosion, sizeof(struct db_fx_explosion_2));
            page_set_invalid_type(PAGE_INVALID_TYPE_PORTION);
        }
        break;
		case EVENT_DATA_COMPULSION:
            data_center_write_light_data(LIGHT_MODE_FX_EXPLOSION_II, p_explosion_model);
            break;
		case EVENT_DATA_HS_WINDOW:
		{
//			bool force_upd = 0;
			memcpy(p_explosion_model, &explosion, sizeof(struct db_fx_explosion_2)); 
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
    p_explosion_model->lightness = dsc->value;
}

static void decay_upd_run_cb(void* data)
{
    adj_btn_t* dsc = (adj_btn_t*)data;
    p_explosion_model->decay = (uint8_t)dsc->value;
}

static bool int_assign_value_cb(void* data)
{
    adj_btn_t* dsc = (adj_btn_t*)data;
    ASSIGN_VALUE_TO_BTN(p_explosion_model->lightness);
}

static bool decay_assign_value_cb(void* data)
{
    adj_btn_t* dsc = (adj_btn_t*)data;
    ASSIGN_VALUE_TO_BTN(p_explosion_model->decay);
}

// static void ctrl_btn_update_cb(void *data)
// {
//     custom_cont_t *dsc = (custom_cont_t*)data;
//     p_explosion_model->obj.state = (uint8_t)dsc->cur_index;    
// }

static void roller_value_change_cb(struct _light_mode_roller_ext* ext)
{
    ext->type = (param_setting_type_t)lv_roller_get_selected(ext->obj);
    p_explosion_model->mode = (enum fx_mode)ext->type;
	
}

static void trigger_btn_release_cb(void)
{
    p_explosion_model->state = 1;
	set_hs_fx_window();
	data_center_write_light_data(LIGHT_MODE_FX_EXPLOSION_II, p_explosion_model);
    UI_PRINTF("SINGLGE_BTN_CLICKED_CB\r\n");
}
#endif

