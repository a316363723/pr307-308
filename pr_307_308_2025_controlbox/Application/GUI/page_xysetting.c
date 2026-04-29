/*********************
 *      INCLUDES
 *********************/
#include "ui_common.h"
#include "page_widget.h"

/*********************
 *      DEFINES
 *********************/
#define COLOR_FIGURE_WIDTH  171
#define COLOR_FIGURE_HEIGHT 189

#define BTN_CONT_WIDTH      118
#define BTN_CONT_HEIGHT     189

/*********************
 *  STATIC VATIABLES
 *********************/
static void param_x_upd_run_cb(void* data);
static void param_y_upd_run_cb(void* data);
static bool param_x_assign_value_cb(void* data);
static bool param_y_assign_value_cb(void* data);

static void page_xy_setting_construct(void);
static void page_xy_setting_destruct(void);

static void color_panel_init(lv_obj_t* cont);
static void page_xy_settiing_storage(void);
static void page_xyseting_time_upd_cb(uint32_t ms);
//static void refresh_color_selector(lv_obj_t* selector, uint16_t x, uint16_t y);

/*********************
 *  STATIC VATIABLES
 *********************/
static struct db_fx_mode_arg* p_xy_setting_model;
static xy_selector_t selector;
static lv_group_t* indev_group;

static adj_btn_t btn_list[] = {
    ADJ_BTN_INITIALIZER(0, 8000, 1, 4260, "X", "" , 12, 39, -3,false, false,  xy_get_value_str, param_x_upd_run_cb, param_x_assign_value_cb, NULL, xy_enc_get_step),
    ADJ_BTN_INITIALIZER(0, 8000, 1, 4005, "Y", "" , 12, 39, -3,false, false,  xy_get_value_str, param_y_upd_run_cb, param_y_assign_value_cb, NULL, xy_enc_get_step),        
};

/*********************
 *  GLOBAL VATIABLES
 *********************/
page_t page_xy_setting = PAGE_INITIALIZER("xy setting",
                                PAGE_XY_SETTING, 
                                page_xy_setting_construct, 
                                page_xy_setting_destruct, 
                                page_xyseting_time_upd_cb,
                                NULL,                                
                                PAGE_NONE);

/*********************
 *  GLOBAL FUNCTIONS
 *********************/
void page_xy_setting_init(lv_obj_t *parent)
{
    indev_group = lv_group_create();
    my_indev_set_group(indev_group);    
    lv_group_set_default(indev_group);

    lv_obj_set_style_bg_color(parent, lv_color_black(), 0);

    lv_obj_t* color_figure_cont = lv_obj_create(parent);
    lv_obj_add_style(color_figure_cont, &style_rect_panel, 0);    
    lv_obj_set_width(color_figure_cont, COLOR_FIGURE_WIDTH);
    lv_obj_set_height(color_figure_cont, COLOR_FIGURE_HEIGHT);
    lv_obj_set_size(color_figure_cont, COLOR_FIGURE_WIDTH, COLOR_FIGURE_HEIGHT);    
    lv_obj_align(color_figure_cont, LV_ALIGN_LEFT_MID, 10, 0);
    
    color_panel_init(color_figure_cont);

    lv_obj_t* btn_panel = lv_obj_create(parent);
    lv_obj_remove_style_all(btn_panel);  
    lv_obj_add_style(btn_panel, &style_rect_panel, 0);
    lv_obj_set_style_pad_row(btn_panel, 8, 0);
    lv_obj_set_style_bg_color(btn_panel, lv_color_make(0, 0, 0), 0);
    lv_obj_set_size(btn_panel, BTN_CONT_WIDTH, BTN_CONT_HEIGHT);
    lv_obj_align_to(btn_panel, color_figure_cont, LV_ALIGN_OUT_RIGHT_TOP, 8, 0);

    lv_obj_t* x_btn = adj_btn_create(btn_panel, LIGHT_MODE_COMMON_RADIUS, ADJ_BTN_TYPE_18);
    lv_obj_set_size(x_btn, lv_pct(100), lv_pct(31));
    adj_btn_init(x_btn, &btn_list[0], 20);
    lv_obj_set_user_data(x_btn, (void *)&btn_list[0]);
    lv_obj_add_event_cb(x_btn, adj_btn_event_cb, LV_EVENT_ALL, NULL);    
    lv_group_add_obj(indev_group, x_btn);
    
    lv_obj_t* y_btn = adj_btn_create(btn_panel, LIGHT_MODE_COMMON_RADIUS, ADJ_BTN_TYPE_18);
    lv_obj_set_size(y_btn, lv_pct(100), lv_pct(31));
    adj_btn_init(y_btn, &btn_list[1], 20);
    lv_obj_set_user_data(y_btn, (void *)&btn_list[1]);
    lv_obj_add_event_cb(y_btn, adj_btn_event_cb, LV_EVENT_ALL, NULL);
    lv_group_add_obj(indev_group, y_btn);
    lv_group_focus_obj(y_btn);
    
    if (!xy_is_in_triangle(&selector, p_xy_setting_model->xy.x, p_xy_setting_model->xy.y)) 
    {
        xy_set_root_mode(true);
    }

    refresh_xy_selector(&selector, p_xy_setting_model->xy.x, p_xy_setting_model->xy.y);

    lv_obj_align(x_btn, LV_ALIGN_TOP_MID, 0, 31);
    lv_obj_align_to(y_btn, x_btn, LV_ALIGN_OUT_BOTTOM_MID, 0, 8);
    adj_btn_refresh_all(btn_list, sizeof(btn_list)/ sizeof(btn_list[0]), true);

    lv_group_set_wrap(indev_group, false);
}


/**********************
 *   STATIC FUNCTIONS
 **********************/
static void page_xy_setting_construct(void)
{
    p_xy_setting_model = (struct db_fx_mode_arg*)screen_get_act_page_data();
    lv_obj_set_style_bg_color(lv_scr_act(), lv_color_black(), 0);


    lv_obj_t* b_cont = lv_obj_create(lv_scr_act());
    lv_obj_remove_style_all(b_cont);
    lv_obj_set_size(b_cont, lv_pct(100), lv_pct(87));
    lv_obj_center(b_cont);

    page_xy_setting_init(b_cont);    
}

static void page_xy_setting_destruct(void)
{
    lv_group_focus_freeze(indev_group, true);
    lv_group_remove_all_objs(indev_group);
    lv_group_del(indev_group);
    indev_group = NULL;

    lv_obj_clean(lv_scr_act());
	page_xy_settiing_storage();
}

static void color_panel_init(lv_obj_t* cont)
{
    lv_obj_t* color_cont = lv_obj_create(cont);
    lv_obj_clear_flag(color_cont, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_style(color_cont, &style_rect_panel, 0);
    lv_obj_clear_flag(color_cont, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_size(color_cont, COLOR_FIGURE_WIDTH, COLOR_FIGURE_HEIGHT - 29);
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

static void param_x_upd_run_cb(void* data)
{
    adj_btn_t* dsc = (adj_btn_t*)data;

    if (xy_is_in_triangle(&selector, dsc->value, p_xy_setting_model->xy.y) || xy_get_root_mode())
    {
        p_xy_setting_model->xy.x = dsc->value;
        refresh_xy_selector(&selector, p_xy_setting_model->xy.x, p_xy_setting_model->xy.y);        
    }    
    else
    {
        dsc->value = p_xy_setting_model->xy.x;         
    }
}

static void param_y_upd_run_cb(void* data)
{
    adj_btn_t* dsc = (adj_btn_t*)data;

    if (xy_is_in_triangle(&selector, p_xy_setting_model->xy.x, dsc->value) || xy_get_root_mode())
    {
        p_xy_setting_model->xy.y = dsc->value;  
        refresh_xy_selector(&selector, p_xy_setting_model->xy.x, p_xy_setting_model->xy.y);
    }    
    else
    {
        dsc->value = p_xy_setting_model->xy.y;     
    }    
}

static bool param_x_assign_value_cb(void* data)
{
    adj_btn_t* dsc = (adj_btn_t*)data;
    ASSIGN_VALUE_TO_BTN(p_xy_setting_model->xy.x); 
}

static bool param_y_assign_value_cb(void* data)
{
    adj_btn_t* dsc = (adj_btn_t*)data;
    ASSIGN_VALUE_TO_BTN(p_xy_setting_model->xy.y);
}

static void page_xy_settiing_storage(void)
{
	#if PROJECT_TYPE==307
    uint32_t src_crc32, dest_crc32;
	uint8_t mode;
	mode = date_center_get_light_mode();
	switch(mode)
	{	
		case LIGHT_MODE_FX_LIGHTNING_II:
		{
			struct db_fx_lightning_2 lightning;
			struct db_fx_lightning_2  p_lightning_model;
			data_center_read_light_data(date_center_get_light_mode(), &lightning);   
			dest_crc32 = ui_calc_checksum((uint8_t*)&lightning,  sizeof(struct db_fx_lightning_2)); 
			memcpy(&p_lightning_model, &g_tUIAllData.lightning2_model, sizeof(struct db_fx_lightning_2));
			src_crc32 = ui_calc_checksum((uint8_t*)&g_tUIAllData.lightning2_model, sizeof(struct db_fx_lightning_2));            
			if (dest_crc32 != src_crc32)
			{
				
				data_center_write_light_data(LIGHT_MODE_FX_LIGHTNING_II, &p_lightning_model);
				memcpy(p_xy_setting_model,&g_tUIAllData.lightning2_model.mode_arg,sizeof(g_tUIAllData.lightning2_model.mode_arg)); 
				adj_btn_refresh_all(btn_list, sizeof(btn_list)/ sizeof(btn_list[0]), true);
			}
		}
		break;
		
		case LIGHT_MODE_FX_STROBE_II:
		{
			struct db_fx_strobe_2 strobe;
			struct db_fx_strobe_2  p_strobe_model;
			data_center_read_light_data(date_center_get_light_mode(), &strobe);   
			dest_crc32 = ui_calc_checksum((uint8_t*)&strobe,  sizeof(struct db_fx_strobe_2)); 
			memcpy(&p_strobe_model, &g_tUIAllData.strobe2_model, sizeof(struct db_fx_strobe_2));
			src_crc32 = ui_calc_checksum((uint8_t*)&g_tUIAllData.strobe2_model, sizeof(struct db_fx_strobe_2));            
			if (dest_crc32 != src_crc32)
			{
				
				data_center_write_light_data(LIGHT_MODE_FX_STROBE_II, &p_strobe_model);
				memcpy(p_xy_setting_model,&g_tUIAllData.strobe2_model.mode_arg,sizeof(g_tUIAllData.strobe2_model.mode_arg)); 
				adj_btn_refresh_all(btn_list, sizeof(btn_list)/ sizeof(btn_list[0]), true);
			}
		}
		break;
		
		case LIGHT_MODE_FX_EXPLOSION_II:
		{
			struct db_fx_explosion_2 explosion;
			struct db_fx_explosion_2  p_explosion_model;
			data_center_read_light_data(date_center_get_light_mode(), &explosion);   
			dest_crc32 = ui_calc_checksum((uint8_t*)&explosion,  sizeof(struct db_fx_explosion_2)); 
			memcpy(&p_explosion_model, &g_tUIAllData.explosion2_model, sizeof(struct db_fx_explosion_2));
			src_crc32 = ui_calc_checksum((uint8_t*)&g_tUIAllData.explosion2_model, sizeof(struct db_fx_explosion_2));            
			if (dest_crc32 != src_crc32)
			{
				
				data_center_write_light_data(LIGHT_MODE_FX_EXPLOSION_II, &p_explosion_model);
				memcpy(p_xy_setting_model,&g_tUIAllData.explosion2_model.mode_arg,sizeof(g_tUIAllData.explosion2_model.mode_arg)); 
				 adj_btn_refresh_all(btn_list, sizeof(btn_list)/ sizeof(btn_list[0]), true);
			}
		}
		break;
		
			case LIGHT_MODE_FX_FAULT_BULB_II:
		{
			struct db_fx_fault_bulb_2 fault_bulb;
			struct db_fx_fault_bulb_2  p_fault_bulb_model;
			data_center_read_light_data(date_center_get_light_mode(), &fault_bulb);   
			dest_crc32 = ui_calc_checksum((uint8_t*)&fault_bulb,  sizeof(struct db_fx_fault_bulb_2)); 
			memcpy(&p_fault_bulb_model, &g_tUIAllData.faultybulb2_model, sizeof(struct db_fx_fault_bulb_2));
			src_crc32 = ui_calc_checksum((uint8_t*)&g_tUIAllData.faultybulb2_model, sizeof(struct db_fx_fault_bulb_2));            
			if (dest_crc32 != src_crc32)
			{
				
				data_center_write_light_data(LIGHT_MODE_FX_FAULT_BULB_II, &p_fault_bulb_model);
				memcpy(p_xy_setting_model,&g_tUIAllData.faultybulb2_model.mode_arg,sizeof(g_tUIAllData.faultybulb2_model.mode_arg)); 
				 adj_btn_refresh_all(btn_list, sizeof(btn_list)/ sizeof(btn_list[0]), true);
			}
		}
		break;
		
			
			case LIGHT_MODE_FX_PULSING_II:
		{
			struct db_fx_pulsing_2 pulsing;
			struct db_fx_pulsing_2  p_pulsing_model;
			data_center_read_light_data(date_center_get_light_mode(), &pulsing);   
			dest_crc32 = ui_calc_checksum((uint8_t*)&pulsing,  sizeof(struct db_fx_pulsing_2)); 
			memcpy(&p_pulsing_model, &g_tUIAllData.pulsing2_model, sizeof(struct db_fx_pulsing_2));
			src_crc32 = ui_calc_checksum((uint8_t*)&g_tUIAllData.pulsing2_model, sizeof(struct db_fx_pulsing_2));            
			if (dest_crc32 != src_crc32)
			{
				
				data_center_write_light_data(LIGHT_MODE_FX_PULSING_II, &p_pulsing_model);
				memcpy(p_xy_setting_model,&g_tUIAllData.pulsing2_model.mode_arg,sizeof(g_tUIAllData.pulsing2_model.mode_arg)); 
				 adj_btn_refresh_all(btn_list, sizeof(btn_list)/ sizeof(btn_list[0]), true);
			}
		}
		break;
		
			case LIGHT_MODE_FX_WELDING_II:
		{
			struct db_fx_welding_2 welding;
			struct db_fx_welding_2  p_welding_model;
			data_center_read_light_data(date_center_get_light_mode(), &welding);   
			dest_crc32 = ui_calc_checksum((uint8_t*)&welding,  sizeof(struct db_fx_welding_2)); 
			memcpy(&p_welding_model, &g_tUIAllData.welding2_model, sizeof(struct db_fx_welding_2));
			src_crc32 = ui_calc_checksum((uint8_t*)&g_tUIAllData.welding2_model, sizeof(struct db_fx_welding_2));            
			if (dest_crc32 != src_crc32)
			{
				
				data_center_write_light_data(LIGHT_MODE_FX_WELDING_II, &p_welding_model);
				memcpy(p_xy_setting_model,&g_tUIAllData.welding2_model.mode_arg,sizeof(g_tUIAllData.welding2_model.mode_arg)); 
				 adj_btn_refresh_all(btn_list, sizeof(btn_list)/ sizeof(btn_list[0]), true);
			}
		}
		break;
	}
  #endif
}

static void page_xyseting_time_upd_cb(uint32_t ms)
{
	page_xy_settiing_storage();

}
