/*********************
 *      INCLUDES
 *********************/
#include "../ui_common.h"
#include "../page_widget.h"

/*********************
 *      DEFINES
 *********************/

/*********************
 *      TPEDEFS
 *********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void upper_upd_run_cb(void* data);
static void cct_upd_run_cb(void* data);
static void sat_upd_run_cb(void* data);
static void lower_upd_run_cb(void* data);

static bool upper_assign_value_cb(void* data);
static bool cct_assign_value_cb(void* data);
static bool sat_assign_value_cb(void* data);
static bool lower_assign_value_cb(void* data);

static void page_hsi_limit_setting_construct(void);
static void page_hsi_limit_setting_destruct(void);
static void page_hsilimit_settiing_storage(void);
/*********************
 *  STATIC VATIABLES
 *********************/
static adj_btn_t btn_list[] = {
    ADJ_BTN_INITIALIZER(1, 360, 1, 360, "Upper Limit", TEMPERATURE_UNIT, 9, 0, -6, true, true,  hue_get_value_str, upper_upd_run_cb, upper_assign_value_cb, adj_btn2_refresh, NULL), 
    ADJ_BTN_INITIALIZER(1, 360, 1, 1, "Lower Limit", TEMPERATURE_UNIT, 9, 0, -6, true, true,  hue_get_value_str, lower_upd_run_cb, lower_assign_value_cb, adj_btn2_refresh, NULL), 
    ADJ_BTN_INITIALIZER(0, 100, 1, 50, "SAT", "%", 6, 45, -6, false, false,  sat_get_value_str, sat_upd_run_cb, sat_assign_value_cb, NULL, NULL), 
    ADJ_BTN_INITIALIZER(UI_CCT_MIN, UI_CCT_MAX, 50, 5600, "CCT", "K", 6, 45, -6, false, false,  cct_get_value_str, cct_upd_run_cb, cct_assign_value_cb, NULL, NULL),   
};

static lv_group_t *indev_group;
static struct db_fx_mode_limit_arg *p_hsi_limit_setting_model;
static void page_hsilimit_time_upd_cb(uint32_t ms);
/*********************
 *  GLOBAL VATIABLES
 *********************/
page_t page_hsi_limit_setting = PAGE_INITIALIZER("CCT Limit Setting",
                                PAGE_HSI_LIMIT_SETTING, 
                                page_hsi_limit_setting_construct, 
                                page_hsi_limit_setting_destruct, 
                                page_hsilimit_time_upd_cb,
                                NULL,                                
                                PAGE_NONE);
/*********************
 *  GLOBAL FUNCTIONS
 *********************/
void page_hsi_limit_setting_init(lv_obj_t* parent)
{
    p_hsi_limit_setting_model = (struct db_fx_mode_limit_arg*)screen_get_act_page_data();

    indev_group = lv_group_create();    
    my_indev_set_group(indev_group);
    lv_group_set_default(indev_group);

    lv_obj_set_style_bg_color(parent, lv_color_black(), 0);

    lv_obj_t* name_cont = lv_obj_create(parent);
    lv_obj_add_style(name_cont, &style_rect_panel, 0);
    lv_obj_set_style_text_color(name_cont, lv_color_white(), 0);
    lv_obj_set_size(name_cont, 104, 125);
    lv_obj_align(name_cont, LV_ALIGN_TOP_LEFT, 22, 24);

    lv_obj_t* name = lv_label_create(name_cont);
    lv_obj_set_style_text_font(name, Font_ResouceGet(FONT_18_BOLD), 0);
    lv_label_set_text_fmt(name, "Adv.HSI");
    lv_obj_center(name);

    btn_list[0].name = Lang_GetStringByID(STRING_ID_UPPER_LIMIT);
    lv_obj_t* up_cont = adj_btn_create(parent, LIGHT_MODE_COMMON_RADIUS, ADJ_BTN_TYPE_18);
    lv_obj_set_user_data(up_cont, (void *)&btn_list[0]);
    adj_btn_init(up_cont, &btn_list[0], 20);
    lv_obj_add_event_cb(up_cont, adj_btn_event_cb, LV_EVENT_ALL, NULL);
    lv_obj_set_size(up_cont,  171, 61);
    lv_obj_align_to(up_cont, name_cont, LV_ALIGN_OUT_RIGHT_TOP, 3, 0);
    lv_group_add_obj(indev_group, up_cont);

    btn_list[1].name = Lang_GetStringByID(STRING_ID_LOWER_LIMIT);
    lv_obj_t* down_cont = adj_btn_create(parent, LIGHT_MODE_COMMON_RADIUS, ADJ_BTN_TYPE_18);
    lv_obj_set_user_data(down_cont, (void *)&btn_list[1]);
    adj_btn_init(down_cont, &btn_list[1], 20);
    lv_obj_add_event_cb(down_cont, adj_btn_event_cb, LV_EVENT_ALL, NULL);    
    lv_obj_set_size(down_cont, 171, 61);
    lv_obj_align_to(down_cont, name_cont, LV_ALIGN_OUT_RIGHT_BOTTOM, 3, 0);   
    lv_group_add_obj(indev_group, down_cont);

    lv_obj_t* sat_cont = adj_btn_create(parent, LIGHT_EFFECT_COMMON_RADIUS, ADJ_BTN_TYPE_18);
    lv_obj_set_user_data(sat_cont, (void *)&btn_list[2]);
    adj_btn_init(sat_cont, &btn_list[2], 20);
    lv_obj_add_event_cb(sat_cont, adj_btn_event_cb, LV_EVENT_ALL, NULL);    
    lv_obj_set_size(sat_cont, 130, 42);
    lv_obj_align_to(sat_cont, name_cont, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 8);
    lv_group_add_obj(indev_group, sat_cont);

    lv_obj_t* cct_cont = adj_btn_create(parent, LIGHT_EFFECT_COMMON_RADIUS, ADJ_BTN_TYPE_18);
    lv_obj_set_user_data(cct_cont, (void *)&btn_list[3]);
    adj_btn_init(cct_cont, &btn_list[3], 20);
    lv_obj_add_event_cb(cct_cont, adj_btn_event_cb, LV_EVENT_ALL, NULL);    
    lv_obj_set_size(cct_cont, 130, 42);
    lv_obj_align_to(cct_cont, down_cont, LV_ALIGN_OUT_BOTTOM_RIGHT, 0, 8);
    
    lv_group_add_obj(indev_group, cct_cont);    
    lv_group_set_wrap(indev_group, false);

    adj_btn_refresh_all(btn_list, sizeof(btn_list)/ sizeof(btn_list[0]), true);
}
/**********************
 *   STATIC FUNCTIONS
 **********************/
static void page_hsi_limit_setting_construct(void)
{
    lv_obj_set_style_bg_color(lv_scr_act(), lv_color_black(), 0);

    lv_obj_t* b_cont = lv_obj_create(lv_scr_act());
    lv_obj_remove_style_all(b_cont);
    lv_obj_set_size(b_cont, lv_pct(100), lv_pct(100));
    lv_obj_align(b_cont, LV_ALIGN_TOP_LEFT, 0, 0);
    page_hsi_limit_setting_init(b_cont);
}

static void page_hsi_limit_setting_destruct(void)
{
    lv_group_focus_freeze(indev_group, true);
    lv_group_remove_all_objs(indev_group);
    lv_group_del(indev_group);
    indev_group = NULL;

    lv_obj_clean(lv_scr_act());
	page_hsilimit_settiing_storage();
}

static void upper_upd_run_cb(void* data)
{
    adj_btn_t* dsc = (adj_btn_t*)data;
    p_hsi_limit_setting_model->hsi.max_hue = dsc->value; 
}

static void cct_upd_run_cb(void* data)
{
    adj_btn_t* dsc = (adj_btn_t*)data;
    p_hsi_limit_setting_model->hsi.cct = dsc->value;  
}

static void sat_upd_run_cb(void* data)
{
    adj_btn_t* dsc = (adj_btn_t*)data;
    p_hsi_limit_setting_model->hsi.sat = dsc->value;      
}

static void lower_upd_run_cb(void* data)
{
    adj_btn_t* dsc = (adj_btn_t*)data;
    p_hsi_limit_setting_model->hsi.min_hue = dsc->value;          
}

static bool upper_assign_value_cb(void* data)
{
    adj_btn_t* dsc = (adj_btn_t*)data;
    ASSIGN_VALUE_TO_BTN(p_hsi_limit_setting_model->hsi.max_hue);
}

static bool cct_assign_value_cb(void* data)
{
    adj_btn_t* dsc = (adj_btn_t*)data;
    ASSIGN_VALUE_TO_BTN(p_hsi_limit_setting_model->hsi.cct);
}

static bool sat_assign_value_cb(void* data)
{
    adj_btn_t* dsc = (adj_btn_t*)data;
    ASSIGN_VALUE_TO_BTN(p_hsi_limit_setting_model->hsi.sat);
}

static bool lower_assign_value_cb(void* data)
{
    adj_btn_t* dsc = (adj_btn_t*)data;
    ASSIGN_VALUE_TO_BTN(p_hsi_limit_setting_model->hsi.min_hue);
}


static void page_hsilimit_settiing_storage(void)
{
	#if PROJECT_TYPE==307
    uint32_t src_crc32, dest_crc32;
	uint8_t mode;
	mode = date_center_get_light_mode();
	switch(mode)
	{
		case LIGHT_MODE_FX_FIRE_II:
		{
			struct db_fx_fire_2 fire;
			struct db_fx_fire_2  p_fire_model;
			data_center_read_light_data(date_center_get_light_mode(), &fire);   
			dest_crc32 = ui_calc_checksum((uint8_t*)&fire,  sizeof(struct db_fx_fire_2)); 
			memcpy(&p_fire_model, &g_tUIAllData.fire2_model, sizeof(struct db_fx_fire_2));
			src_crc32 = ui_calc_checksum((uint8_t*)&g_tUIAllData.fire2_model, sizeof(struct db_fx_fire_2));            
			if (dest_crc32 != src_crc32)
			{
				
				data_center_write_light_data(LIGHT_MODE_FX_FIRE_II, &p_fire_model);
				memcpy(p_hsi_limit_setting_model,&g_tUIAllData.fire2_model.mode_arg,sizeof(g_tUIAllData.fire2_model.mode_arg)); 
				adj_btn_refresh_all(btn_list, sizeof(btn_list)/ sizeof(btn_list[0]), true);
			}
			break;
		}
		case LIGHT_MODE_FX_TV_II:
		{
			
			struct db_fx_tv_2 tv;
			struct db_fx_tv_2  p_tv_model;
			data_center_read_light_data(date_center_get_light_mode(), &tv);   
			dest_crc32 = ui_calc_checksum((uint8_t*)&tv,  sizeof(struct db_fx_tv_2)); 
			memcpy(&p_tv_model, &g_tUIAllData.tv2_model, sizeof(struct db_fx_tv_2));
			src_crc32 = ui_calc_checksum((uint8_t*)&g_tUIAllData.tv2_model, sizeof(struct db_fx_tv_2));            
			if (dest_crc32 != src_crc32)
			{
				
				data_center_write_light_data(LIGHT_MODE_FX_TV_II, &p_tv_model);
				memcpy(p_hsi_limit_setting_model,&g_tUIAllData.tv2_model.mode_arg,sizeof(g_tUIAllData.tv2_model.mode_arg)); 
				adj_btn_refresh_all(btn_list, sizeof(btn_list)/ sizeof(btn_list[0]), true);
			}
			break;
		}
			
	}
   #endif

}

void page_hsilimit_time_upd_cb(uint32_t ms)
{
	page_hsilimit_settiing_storage();
}
