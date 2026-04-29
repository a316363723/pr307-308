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


#define PALLETE_WIDTH               132
#define PALLETE_HEIGHT              132

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void g_upd_run_cb(void* data);
static void b_upd_run_cb(void* data);
static void r_upd_run_cb(void* data);

static bool r_assign_value_cb(void* data);
static bool g_assign_value_cb(void* data);
static bool b_assign_value_cb(void* data);

static void page_rgb_setting_construct(void);
static void page_rgb_setting_destruct(void);
static void page_rgb_settiing_storage(void);

static void color_panel_init(lv_obj_t* parent);
/**********************
 *  STATIC VARIABLES
 **********************/
static adj_btn_t btn_list[3] = 
{   
    ADJ_BTN_INITIALIZER(UI_RGB_MIN, UI_RGB_MAX, UI_RGB_STEP, UI_RGB_DEF_VAL, "R", "" ,6, 43, -8, false, false,  rgb_get_value_str, r_upd_run_cb, r_assign_value_cb, NULL, NULL), 
    ADJ_BTN_INITIALIZER(UI_RGB_MIN, UI_RGB_MAX, UI_RGB_STEP, UI_RGB_DEF_VAL, "G", "",6, 43, -8, false, false,  rgb_get_value_str, g_upd_run_cb, g_assign_value_cb, NULL, NULL), 
    ADJ_BTN_INITIALIZER(UI_RGB_MIN, UI_RGB_MAX, UI_RGB_STEP, UI_RGB_DEF_VAL, "B", "", 6, 43, -8, false, false,  rgb_get_value_str, b_upd_run_cb, b_assign_value_cb, NULL, NULL), 
};

static lv_group_t *indev_group;
static lv_obj_t* color_pallet;
static struct db_fx_mode_arg* p_rgb_setting_model;
static void page_rgb_time_upd_cb(uint32_t ms);
/*********************
 *  GLOBAL VATIABLES
 *********************/
page_t page_rgb_setting = PAGE_INITIALIZER("rgb setting",
                                PAGE_RGB_SETTING,  
                                page_rgb_setting_construct, 
                                page_rgb_setting_destruct, 
                                page_rgb_time_upd_cb,
                                NULL,                                
                                PAGE_NONE);

/**********************
 *   GLOBAL FUNCTIONS
 **********************/
void page_rgb_setting_init(lv_obj_t* parent)
{
    p_rgb_setting_model = (struct db_fx_mode_arg*)screen_get_act_page_data();

    indev_group = lv_group_create();
    my_indev_set_group(indev_group);
    lv_group_set_default(indev_group);

    lv_obj_set_style_bg_color(parent, lv_color_black(), 0);

        /* color panel */
    lv_obj_t* color_figure_cont = lv_obj_create(parent);
    lv_obj_add_style(color_figure_cont, &style_rect_panel, 0);
    lv_obj_set_size(color_figure_cont, COLOR_FIGURE_WIDTH, COLOR_FIGURE_HEIGHT);
    lv_obj_align(color_figure_cont, LV_ALIGN_LEFT_MID, 10, 0);

    color_panel_init(color_figure_cont);

    /* rgb button */
    lv_obj_t* btn_panel = lv_obj_create(parent);
    lv_obj_remove_style_all(btn_panel);  
    lv_obj_add_style(btn_panel, &style_rect_panel, 0);
    lv_obj_set_flex_flow(btn_panel, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_pad_row(btn_panel, 8, 0);
    lv_obj_set_style_bg_color(btn_panel, lv_color_make(0, 0, 0), 0);
    lv_obj_set_size(btn_panel, BTN_CONT_WIDTH, BTN_CONT_HEIGHT);
    lv_obj_align_to(btn_panel, color_figure_cont, LV_ALIGN_OUT_RIGHT_TOP, 8, 0);   

    lv_obj_t* r_btn = adj_btn_create(btn_panel, LIGHT_MODE_COMMON_RADIUS, ADJ_BTN_TYPE_18);
    lv_obj_set_size(r_btn, lv_pct(100), lv_pct(31));
    adj_btn_init(r_btn, &btn_list[0], 20);
    lv_obj_set_user_data(r_btn, (void *)&btn_list[0]);
    lv_obj_add_event_cb(r_btn, adj_btn_event_cb, LV_EVENT_ALL, NULL);    
    lv_group_add_obj(indev_group, r_btn);
    
    lv_obj_t* g_btn = adj_btn_create(btn_panel, LIGHT_MODE_COMMON_RADIUS, ADJ_BTN_TYPE_18);
    lv_obj_set_size(g_btn, lv_pct(100), lv_pct(31));
    adj_btn_init(g_btn, &btn_list[1], 20);
    lv_obj_set_user_data(g_btn, (void *)&btn_list[1]);
    lv_obj_add_event_cb(g_btn, adj_btn_event_cb, LV_EVENT_ALL, NULL);
    lv_group_add_obj(indev_group, g_btn);

    lv_obj_t* b_btn = adj_btn_create(btn_panel, LIGHT_MODE_COMMON_RADIUS, ADJ_BTN_TYPE_18);
    lv_obj_set_size(b_btn, lv_pct(100), lv_pct(31));
    adj_btn_init(b_btn, &btn_list[2], 20);
    lv_obj_set_user_data(b_btn, (void *)&btn_list[2]);
    lv_obj_add_event_cb(b_btn, adj_btn_event_cb, LV_EVENT_ALL, NULL);
    lv_group_add_obj(indev_group, b_btn);     
    lv_group_focus_obj(r_btn);
    
    lv_group_set_wrap(indev_group, false);
    adj_btn_refresh_all(btn_list, sizeof(btn_list)/ sizeof(btn_list[0]), true);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/
static void page_rgb_setting_construct(void)
{
    lv_obj_set_style_bg_color(lv_scr_act(), lv_color_black(), 0);

    lv_obj_t* b_cont = lv_obj_create(lv_scr_act());
    lv_obj_remove_style_all(b_cont);
    lv_obj_set_size(b_cont, lv_pct(100), lv_pct(100));
    lv_obj_center(b_cont);

    page_rgb_setting_init(b_cont);    
}

static void page_rgb_setting_destruct(void)
{
    lv_group_focus_freeze(indev_group, true);
    lv_group_remove_all_objs(indev_group);
    lv_group_del(indev_group);
    indev_group = NULL;

    lv_obj_clean(lv_scr_act());
	page_rgb_settiing_storage();
}

static void color_panel_init(lv_obj_t* parent)
{
    color_pallet = lv_obj_create(parent);
    lv_obj_add_style(color_pallet, &style_rect_panel, 0);
    lv_obj_set_style_bg_color(color_pallet, lv_color_make(255, 0, 0), 0);
    lv_obj_set_size(color_pallet, PALLETE_WIDTH, PALLETE_HEIGHT);
    //lv_obj_align(color_pallet, LV_ALIGN_LEFT_MID, 10, 0);
    lv_obj_center(color_pallet);
    // LV_IMG_DECLARE(ImgRGBColorfulBar)
    // lv_obj_t* img = lv_img_create(parent);
    // lv_img_set_src(img, &ImgRGBColorfulBar);
    // lv_obj_align_to(img, color_palette,  LV_ALIGN_OUT_RIGHT_MID, 12, 0);    
}

static void set_pallet_color(lv_obj_t* pallet, uint16_t _r, uint16_t _g, uint16_t _b)
{
    uint8_t r = lv_map(_r, 0, UI_RGB_MAX, 0, 255);
    uint8_t g = lv_map(_g, 0, UI_RGB_MAX, 0, 255);
    uint8_t b = lv_map(_b, 0, UI_RGB_MAX, 0, 255);

    lv_obj_set_style_bg_color(pallet, lv_color_make(r, g, b), 0);
}

static void g_upd_run_cb(void* data)
{
    adj_btn_t* dsc = (adj_btn_t*)data;
    p_rgb_setting_model->rgb.g = dsc->value;
    set_pallet_color(color_pallet, p_rgb_setting_model->rgb.r, p_rgb_setting_model->rgb.g, p_rgb_setting_model->rgb.b);
}

static void b_upd_run_cb(void* data)
{
    adj_btn_t* dsc = (adj_btn_t*)data;   
    p_rgb_setting_model->rgb.b = dsc->value;     
    set_pallet_color(color_pallet, p_rgb_setting_model->rgb.r, p_rgb_setting_model->rgb.g, p_rgb_setting_model->rgb.b);
}

static void r_upd_run_cb(void* data)
{
    adj_btn_t* dsc = (adj_btn_t*)data;
    p_rgb_setting_model->rgb.r = dsc->value;
    set_pallet_color(color_pallet, p_rgb_setting_model->rgb.r, p_rgb_setting_model->rgb.g, p_rgb_setting_model->rgb.b);
}

static bool r_assign_value_cb(void* data)
{
    adj_btn_t* dsc = (adj_btn_t*)data;
    ASSIGN_VALUE_TO_BTN(p_rgb_setting_model->rgb.r);
}

static bool g_assign_value_cb(void* data)
{
    adj_btn_t* dsc = (adj_btn_t*)data;
    ASSIGN_VALUE_TO_BTN(p_rgb_setting_model->rgb.g);
}

static bool b_assign_value_cb(void* data)
{
    adj_btn_t* dsc = (adj_btn_t*)data;
    ASSIGN_VALUE_TO_BTN(p_rgb_setting_model->rgb.b);
}


static void page_rgb_settiing_storage(void)
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
				memcpy(p_rgb_setting_model,&g_tUIAllData.lightning2_model.mode_arg,sizeof(g_tUIAllData.lightning2_model.mode_arg)); 
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
				memcpy(p_rgb_setting_model,&g_tUIAllData.strobe2_model.mode_arg,sizeof(g_tUIAllData.strobe2_model.mode_arg)); 
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
				memcpy(p_rgb_setting_model,&g_tUIAllData.explosion2_model.mode_arg,sizeof(g_tUIAllData.explosion2_model.mode_arg)); 
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
				memcpy(p_rgb_setting_model,&g_tUIAllData.faultybulb2_model.mode_arg,sizeof(g_tUIAllData.faultybulb2_model.mode_arg)); 
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
				memcpy(p_rgb_setting_model,&g_tUIAllData.pulsing2_model.mode_arg,sizeof(g_tUIAllData.pulsing2_model.mode_arg)); 
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
				memcpy(p_rgb_setting_model,&g_tUIAllData.welding2_model.mode_arg,sizeof(g_tUIAllData.welding2_model.mode_arg)); 
				 adj_btn_refresh_all(btn_list, sizeof(btn_list)/ sizeof(btn_list[0]), true);
			}
		}
		break;
		
	}
  
   #endif
}
static void page_rgb_time_upd_cb(uint32_t ms)
{
	page_rgb_settiing_storage();

}
