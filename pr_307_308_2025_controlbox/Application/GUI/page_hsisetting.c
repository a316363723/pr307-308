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

#define PARAM_BTN_WIDTH     118
#define PARAM_BTN_HEIGHT    60
#define BTN_CONT_WIDTH      122
#define BTN_CONT_HEIGHT     190

/*********************
 *      TPEDEFS
 *********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void cct_upd_run_cb(void* data);
static void hue_upd_run_cb(void* data);
static void sat_upd_run_cb(void* data);

static bool cct_assign_value_cb(void* data);
static bool hue_assign_value_cb(void* data);
static bool sat_assign_value_cb(void* data);

static void page_hsi_setting_construct(void);
static void page_hsi_setting_destruct(void);
static lv_point_t get_center_selector(lv_point_t center, int16_t radius, int16_t hue, int16_t saturation);

static lv_obj_t* color_panel_init(lv_obj_t* cont);
static void page_hsi_settiing_storage(void);
static void page_hsi_time_upd_cb(uint32_t ms);

/*********************
 *  STATIC VATIABLES
 *********************/
static lv_group_t *indev_group;
static struct db_fx_mode_arg* p_hsi_setting_model;
static lv_obj_t* color_selector;
static lv_point_t img_center_pos;

static adj_btn_t btn_list[] = {   
    ADJ_BTN_INITIALIZER(1, 360, 1, 1, "HUE", "°", 3, 41, -2, false, true,  hue_get_value_str, hue_upd_run_cb, hue_assign_value_cb, NULL, NULL), 
    ADJ_BTN_INITIALIZER(0, 100, 1, 50, "SAT", "%", 3, 41, -2, false, false, sat_get_value_str, sat_upd_run_cb, sat_assign_value_cb, NULL, NULL), 
    ADJ_BTN_INITIALIZER(UI_CCT_MIN, UI_CCT_MAX, 50, UI_CCT_MIN, "CCT", "K", 3, 41, -2, false, false,  cct_get_value_str, cct_upd_run_cb, cct_assign_value_cb, NULL, NULL),
};
/*********************
 *  GLOBAL VATIABLES
 *********************/
page_t page_hsi_setting = PAGE_INITIALIZER("hsi_param_setting",
                                PAGE_HSI_SETTING, 
                                page_hsi_setting_construct, 
                                page_hsi_setting_destruct, 
                                page_hsi_time_upd_cb,
                                NULL,                                
                                PAGE_NONE);

/*********************
 *  GLOBAL FUNCTIONS
 *********************/

void page_hsi_setting_init(lv_obj_t* parent)
{
    p_hsi_setting_model = (struct db_fx_mode_arg *)screen_get_act_page_data();

    indev_group = lv_group_create();
    my_indev_set_group(indev_group);   
    lv_group_set_default(indev_group);
    
    /* color panel */
    lv_obj_t* color_figure_cont = lv_obj_create(parent);
    lv_obj_add_style(color_figure_cont, &style_rect_panel, 0);
    lv_obj_set_size(color_figure_cont, COLOR_FIGURE_WIDTH, COLOR_FIGURE_HEIGHT);
    lv_obj_align(color_figure_cont, LV_ALIGN_LEFT_MID, 10, 0);

    color_panel_init(color_figure_cont);

    /* hsc button */
    lv_obj_t* btn_panel = lv_obj_create(parent);
    lv_obj_remove_style_all(btn_panel);  
    lv_obj_add_style(btn_panel, &style_rect_panel, 0);
    lv_obj_set_style_pad_row(btn_panel, 8, 0);
    lv_obj_set_style_bg_color(btn_panel, lv_color_make(0, 0, 0), 0);
    lv_obj_set_size(btn_panel, BTN_CONT_WIDTH, BTN_CONT_HEIGHT);
    lv_obj_align_to(btn_panel, color_figure_cont, LV_ALIGN_OUT_RIGHT_TOP, 8, 0);

    lv_obj_t* hue_btn = adj_btn_create(btn_panel, LIGHT_MODE_COMMON_RADIUS, ADJ_BTN_TYPE_18);
    lv_obj_set_size(hue_btn, lv_pct(100), 60);
    lv_obj_align(hue_btn, LV_ALIGN_TOP_MID, 0, 0);
    adj_btn_init(hue_btn, &btn_list[0], 20);
    lv_obj_set_user_data(hue_btn, (void *)&btn_list[0]);
    lv_obj_add_event_cb(hue_btn, adj_btn_event_cb, LV_EVENT_ALL, NULL);    
    lv_group_add_obj(indev_group, hue_btn);
    
    lv_obj_t* sat_btn = adj_btn_create(btn_panel, LIGHT_MODE_COMMON_RADIUS, ADJ_BTN_TYPE_18);
    lv_obj_set_size(sat_btn, lv_pct(100), 60);
    adj_btn_init(sat_btn, &btn_list[1], 20);
    lv_obj_align_to(sat_btn, hue_btn, LV_ALIGN_OUT_BOTTOM_MID, 0, 5);
    lv_obj_set_user_data(sat_btn, (void *)&btn_list[1]);
    lv_obj_add_event_cb(sat_btn, adj_btn_event_cb, LV_EVENT_ALL, NULL);
    lv_group_add_obj(indev_group, sat_btn);

    lv_obj_t* cct_btn = adj_btn_create(btn_panel, LIGHT_MODE_COMMON_RADIUS, ADJ_BTN_TYPE_18);
    lv_obj_set_size(cct_btn, lv_pct(100), 60);
    adj_btn_init(cct_btn, &btn_list[2], 20);
    lv_obj_align_to(cct_btn, sat_btn, LV_ALIGN_OUT_BOTTOM_MID, 0, 5);
    lv_obj_set_user_data(cct_btn, (void *)&btn_list[2]);
    lv_obj_add_event_cb(cct_btn, adj_btn_event_cb, LV_EVENT_ALL, NULL);
    lv_group_add_obj(indev_group, cct_btn);
    lv_group_focus_obj(hue_btn);

    lv_group_set_wrap(indev_group, false);
    
    adj_btn_refresh_all(btn_list, sizeof(btn_list)/ sizeof(btn_list[0]), true);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/
static void page_hsi_setting_construct(void)
{
    lv_obj_set_style_bg_color(lv_scr_act(), lv_color_black(), 0);

    lv_obj_t* b_cont = lv_obj_create(lv_scr_act());
    lv_obj_remove_style_all(b_cont);
    lv_obj_set_size(b_cont, lv_pct(100), lv_pct(100));
    lv_obj_center(b_cont);

    page_hsi_setting_init(b_cont);
}

static void page_hsi_setting_destruct(void)
{
    lv_group_focus_freeze(indev_group, true);
    lv_group_remove_all_objs(indev_group);
    lv_group_del(indev_group);
    indev_group = NULL;

    lv_obj_clean(lv_scr_act());
	page_hsi_settiing_storage();
}

static lv_obj_t* color_panel_init(lv_obj_t* cont)
{
    lv_obj_t* color_cont = lv_obj_create(cont);
    lv_obj_clear_flag(color_cont, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_style(color_cont, &style_rect_panel, 0);
    lv_obj_clear_flag(color_cont, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_size(color_cont, COLOR_FIGURE_WIDTH, COLOR_FIGURE_HEIGHT - 29);
    lv_obj_center(color_cont);
    
    LV_IMG_DECLARE(ImgHSIFigure);
    lv_obj_t* img = lv_img_create(color_cont);
    lv_obj_set_size(img, 147, 147);
    lv_img_set_src(img, &ImgHSIFigure);
    lv_obj_center(img);

    //求中心位置
    img_center_pos.x = lv_obj_get_style_width(img, 0) / 2;
    img_center_pos.y = lv_obj_get_style_height(img, 0) / 2;

    LV_IMG_DECLARE(ImgHSISelector);
    color_selector = lv_img_create(img);
    lv_img_set_src(color_selector, &ImgHSISelector);
    lv_obj_set_size(color_selector, 9, 9);

    lv_coord_t w = lv_obj_get_style_width(img, 0);
    lv_coord_t h = lv_obj_get_style_height(img, 0);
    lv_point_t center;
    center.x = w / 2 - lv_obj_get_style_width(color_selector, 0) / 2;
    center.y = h / 2 - lv_obj_get_style_width(color_selector, 0) / 2;
    lv_obj_set_pos(color_selector, center.x, center.y);

    return color_selector;
}

static void cct_upd_run_cb(void* data)
{
    adj_btn_t* dsc = (adj_btn_t*)data;
    p_hsi_setting_model->hsi.cct = dsc->value;    
}

static void hue_upd_run_cb(void* data)
{
    adj_btn_t* dsc = (adj_btn_t*)data;
    lv_obj_t* img = lv_obj_get_parent(color_selector);
    int16_t radius = 134 / 2;
    lv_coord_t w = lv_obj_get_width(img);       
    lv_coord_t h = lv_obj_get_height(img);
    lv_point_t center;

    p_hsi_setting_model->hsi.hue = dsc->value;
    center = get_center_selector(img_center_pos, radius, dsc->value, p_hsi_setting_model->hsi.sat);  
    UI_PRINTF("Center x:%d Center y: %d\r\n", center.x, center.y);
    center.x -= (lv_obj_get_style_width(color_selector, 0) / 2);
    center.y -= (lv_obj_get_style_height(color_selector, 0)  / 2);
    UI_PRINTF("Real x:%d Real y: %d\r\n", center.x, center.y);    
    lv_obj_set_pos(color_selector, center.x, center.y);   
}

static void sat_upd_run_cb(void* data)
{
    adj_btn_t* dsc = (adj_btn_t*)data;
    lv_obj_t* img = lv_obj_get_parent(color_selector);
    int16_t radius = 134 / 2 - 3;
    lv_coord_t w = lv_obj_get_width(img);
    lv_coord_t h = lv_obj_get_height(img);
    lv_coord_t last_x = lv_obj_get_style_x(img, 0);
    lv_point_t center;

    p_hsi_setting_model->hsi.sat = dsc->value;
    center = get_center_selector(img_center_pos, radius, p_hsi_setting_model->hsi.hue, dsc->value);    
    UI_PRINTF("Center x:%d Center y: %d\r\n", center.x, center.y);
    center.x -= (lv_obj_get_style_width(color_selector, 0) / 2);
    center.y -= (lv_obj_get_style_height(color_selector, 0)  / 2);
    UI_PRINTF("Real x:%d Real y: %d\r\n", center.x, center.y);
    lv_obj_set_pos(color_selector, center.x, center.y);
}

static bool cct_assign_value_cb(void* data)
{
    adj_btn_t* dsc = (adj_btn_t*)data;
    ASSIGN_VALUE_TO_BTN(p_hsi_setting_model->hsi.cct);
}

static bool hue_assign_value_cb(void* data)
{
    adj_btn_t* dsc = (adj_btn_t*)data;
    ASSIGN_VALUE_TO_BTN(p_hsi_setting_model->hsi.hue);
}

static bool sat_assign_value_cb(void* data)
{
    adj_btn_t* dsc = (adj_btn_t*)data;
    ASSIGN_VALUE_TO_BTN(p_hsi_setting_model->hsi.sat);
}

static lv_point_t get_center_selector(lv_point_t center, int16_t radius, int16_t hue, int16_t saturation)
{
    lv_point_t pos;
    int16_t inner_radius = radius * saturation / 100;
    int16_t angle = (360 - hue + 90); /* 逆时针方向 */

    pos.x = center.x + (inner_radius * lv_trigo_sin(angle) >> LV_TRIGO_SHIFT);
    pos.y = center.y - (inner_radius * lv_trigo_cos(angle) >> LV_TRIGO_SHIFT);

    return pos;
}

static void page_hsi_settiing_storage(void)
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
				memcpy(p_hsi_setting_model,&g_tUIAllData.lightning2_model.mode_arg,sizeof(g_tUIAllData.lightning2_model.mode_arg)); 
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
				memcpy(p_hsi_setting_model,&g_tUIAllData.strobe2_model.mode_arg,sizeof(g_tUIAllData.strobe2_model.mode_arg)); 
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
				memcpy(p_hsi_setting_model,&g_tUIAllData.explosion2_model.mode_arg,sizeof(g_tUIAllData.explosion2_model.mode_arg)); 
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
				memcpy(p_hsi_setting_model,&g_tUIAllData.faultybulb2_model.mode_arg,sizeof(g_tUIAllData.faultybulb2_model.mode_arg)); 
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
				memcpy(p_hsi_setting_model,&g_tUIAllData.pulsing2_model.mode_arg,sizeof(g_tUIAllData.pulsing2_model.mode_arg)); 
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
				memcpy(p_hsi_setting_model,&g_tUIAllData.welding2_model.mode_arg,sizeof(g_tUIAllData.welding2_model.mode_arg)); 
				adj_btn_refresh_all(btn_list, sizeof(btn_list)/ sizeof(btn_list[0]), true);
			}
		}
		break;
		
	}
  #endif
   
}

static void page_hsi_time_upd_cb(uint32_t ms)
{
	page_hsi_settiing_storage();

}
