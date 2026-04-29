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
#define BTN_CONT_WIDTH      118
#define BTN_CONT_HEIGHT     189

#define CCT_CIRCLE_CENTER_X                 85
#define CCT_CIRCLE_CENTER_Y                 77

#define CCT_INNER_CIRCLE_RADIUS             38
#define CCT_OUTTER_CIRCLE_RADIUS            70

#define CCT_ARC_MAX_RAD                     (74)
#define CCT_ARC_MIN_RAD                     (-74)

/*********************
 *      TPEDEFS
 *********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void cct_upd_run_cb(void* data);
static void gm_upd_run_cb(void* data);

static bool cct_assign_value_cb(void* data);
static bool gm_assign_value_cb(void* data);

static void refresh_scale_img(lv_obj_t* img, int16_t radius, int16_t value1, int16_t min1, int16_t max1);
static void color_panel_init(lv_obj_t* cont);

static void page_cct_setting_construct(void);
static void page_cct_setting_destruct(void);
static void page_cct_settiing_storage(void);
static void page_cct_time_upd_cb(uint32_t ms);
/*********************
 *  STATIC VATIABLES
 *********************/
static struct db_fx_mode_arg* p_cct_setting_model;
static lv_obj_t* cct_scale_img;
static lv_obj_t* gm_scale_img;
static lv_group_t *indev_group;

static adj_btn_t btn_list[] = {    
    ADJ_BTN_INITIALIZER(UI_CCT_MIN, UI_CCT_MAX, 50, UI_CCT_MIN, "CCT", "K", 3, 41, -2, false, false,  cct_get_value_str, cct_upd_run_cb, cct_assign_value_cb, NULL, NULL),
    ADJ_BTN_INITIALIZER(-100, 100, 1, 0, "G/M", "",  3, 41, -2, false, false,  gm_get_value_str, gm_upd_run_cb, gm_assign_value_cb, NULL, NULL),
};

/*********************
 *  GLOBAL VATIABLES
 *********************/
page_t page_cct_setting = PAGE_INITIALIZER("cct setting",
                                PAGE_CCT_SETTING, 
                                page_cct_setting_construct, 
                                page_cct_setting_destruct, 
                                page_cct_time_upd_cb,
                                NULL,
                                PAGE_NONE);

/*********************
 *  GLOBAL FUNCTIONS
 *********************/
void page_cct_setting_init(lv_obj_t *parent)
{
    p_cct_setting_model = (struct db_fx_mode_arg*)screen_get_act_page_data();

    indev_group = lv_group_create();
    my_indev_set_group(indev_group);
    lv_group_set_default(indev_group);
    
    lv_obj_set_style_bg_color(parent, lv_color_make(0, 0, 0), 0);

    lv_obj_t* color_figure_cont = lv_obj_create(parent);
    lv_obj_add_style(color_figure_cont, &style_rect_panel, 0);
    lv_obj_clear_flag(color_figure_cont, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_size(color_figure_cont, COLOR_FIGURE_WIDTH, COLOR_FIGURE_HEIGHT);    
    lv_obj_align(color_figure_cont, LV_ALIGN_LEFT_MID, 10, 0);

    color_panel_init(color_figure_cont);
    
    lv_obj_t* btn_panel = lv_obj_create(parent);
    lv_obj_remove_style_all(btn_panel);  
    lv_obj_add_style(btn_panel, &style_rect_panel, 0);
    //lv_obj_set_flex_flow(btn_panel, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_pad_row(btn_panel, 8, 0);
    lv_obj_set_style_bg_color(btn_panel, lv_color_make(0, 0, 0), 0);
    lv_obj_set_size(btn_panel, BTN_CONT_WIDTH, BTN_CONT_HEIGHT);
    lv_obj_align_to(btn_panel, color_figure_cont, LV_ALIGN_OUT_RIGHT_TOP, 8, 0);

    lv_obj_t* cct_btn = adj_btn_create(btn_panel, LIGHT_MODE_COMMON_RADIUS, ADJ_BTN_TYPE_18);
    lv_obj_set_size(cct_btn, lv_pct(100), lv_pct(31));
    adj_btn_init(cct_btn, &btn_list[0], 20);
    lv_obj_set_user_data(cct_btn, (void *)&btn_list[0]);
    lv_obj_add_event_cb(cct_btn, adj_btn_event_cb, LV_EVENT_ALL, NULL);    
    lv_group_add_obj(indev_group, cct_btn);
    
    lv_obj_t* gm_btn = adj_btn_create(btn_panel, LIGHT_MODE_COMMON_RADIUS, ADJ_BTN_TYPE_18);
    lv_obj_set_size(gm_btn, lv_pct(100), lv_pct(31));
    adj_btn_init(gm_btn, &btn_list[1], 20);
    lv_obj_set_user_data(gm_btn, (void *)&btn_list[1]);
    lv_obj_add_event_cb(gm_btn, adj_btn_event_cb, LV_EVENT_ALL, NULL);
    lv_group_add_obj(indev_group, gm_btn);
    lv_group_focus_obj(cct_btn);

    lv_obj_align(cct_btn, LV_ALIGN_TOP_MID, 0, 31);
    lv_obj_align_to(gm_btn, cct_btn, LV_ALIGN_OUT_BOTTOM_MID, 0, 8);
    lv_group_set_wrap(indev_group, false);

    adj_btn_refresh_all(btn_list, sizeof(btn_list)/ sizeof(btn_list[0]), true);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/
static void page_cct_setting_construct(void)
{
    lv_obj_set_style_bg_color(lv_scr_act(), lv_color_black(), 0);

    lv_obj_t* b_cont = lv_obj_create(lv_scr_act());
    lv_obj_remove_style_all(b_cont);
    lv_obj_set_size(b_cont, lv_pct(100), lv_pct(100));
    lv_obj_center(b_cont);
    page_cct_setting_init(b_cont);
}

static void page_cct_setting_destruct(void)
{
    lv_group_focus_freeze(indev_group, true);
    lv_group_remove_all_objs(indev_group);
    lv_group_del(indev_group);
    indev_group = NULL;

    lv_obj_clean(lv_scr_act());
	page_cct_settiing_storage();
}

static void color_panel_init(lv_obj_t* cont)
{
    lv_obj_t* img_pannel = lv_obj_create(cont);    
    lv_obj_remove_style_all(img_pannel);
    lv_obj_set_style_text_font(img_pannel, Font_ResouceGet(FONT_10), 0);
    lv_obj_set_style_text_color(img_pannel, lv_color_white(), 0);
    lv_obj_set_size(img_pannel, lv_pct(100), COLOR_FIGURE_HEIGHT - 38 - 29);    
    lv_obj_center(img_pannel);

    LV_IMG_DECLARE(ImgCCT);
    lv_obj_t* img = lv_img_create(img_pannel);
    lv_img_set_src(img, &ImgCCT);    
    lv_obj_align(img, LV_ALIGN_TOP_MID, 0, 0);    

    LV_IMG_DECLARE(ImgCCTSelector)
    cct_scale_img = lv_img_create(img_pannel);
    lv_img_set_src(cct_scale_img, &ImgCCTSelector);

    gm_scale_img = lv_img_create(img_pannel);
    lv_img_set_src(gm_scale_img, &ImgCCTSelector);

    lv_obj_t* cct_label_min = lv_label_create(img_pannel);
    lv_label_set_text_fmt(cct_label_min, "%dK", UI_CCT_MIN);
    lv_obj_t* cct_label_max = lv_label_create(img_pannel);
    lv_label_set_text_fmt(cct_label_max, "%dK", UI_CCT_MAX);
    lv_obj_t* gm_label_min = lv_label_create(img_pannel);
    lv_label_set_text(gm_label_min, "G/M-1");
    lv_obj_t* gm_label_max = lv_label_create(img_pannel);
    lv_label_set_text(gm_label_max, "G/M+1");

    lv_obj_align(cct_label_min, LV_ALIGN_TOP_LEFT, 6, 62);
    lv_obj_align(gm_label_min, LV_ALIGN_TOP_LEFT, 36, 70);
    lv_obj_align(cct_label_max, LV_ALIGN_TOP_RIGHT, -6, 62);
    lv_obj_align(gm_label_max, LV_ALIGN_TOP_RIGHT, -36, 70);
}

static void cct_upd_run_cb(void* data)
{
    adj_btn_t* dsc = (adj_btn_t*)data;
    p_cct_setting_model->cct.cct = dsc->value;       
    refresh_scale_img(cct_scale_img, CCT_OUTTER_CIRCLE_RADIUS, dsc->value, dsc->min, dsc->max);
}

static void gm_upd_run_cb(void* data)
{
    adj_btn_t* dsc = (adj_btn_t*)data;
    p_cct_setting_model->cct.duv = dsc->value;       
    refresh_scale_img(gm_scale_img, CCT_INNER_CIRCLE_RADIUS, dsc->value, dsc->min, dsc->max);
}

static bool cct_assign_value_cb(void* data)
{
    adj_btn_t* dsc = (adj_btn_t*)data;
    ASSIGN_VALUE_TO_BTN(p_cct_setting_model->cct.cct);
}

static bool gm_assign_value_cb(void* data)
{
    adj_btn_t* dsc = (adj_btn_t*)data;
    ASSIGN_VALUE_TO_BTN(p_cct_setting_model->cct.duv);
}

static void refresh_scale_img(lv_obj_t* img, int16_t radius, int16_t value1, int16_t min1, int16_t max1)
{
    if (img == NULL)        return;

    lv_coord_t x, y;
    const lv_img_dsc_t *pdsc = (lv_img_dsc_t*)lv_img_get_src(img);
    // 1. Get the angle.
    int16_t angle = lv_map(value1, min1, max1, CCT_ARC_MIN_RAD, CCT_ARC_MAX_RAD);

    x = CCT_CIRCLE_CENTER_X + (radius * lv_trigo_sin(angle) >> LV_TRIGO_SHIFT);
    y = CCT_CIRCLE_CENTER_Y - (radius * lv_trigo_cos(angle) >> LV_TRIGO_SHIFT);

    x -= pdsc->header.w / 2;
    y -= pdsc->header.h / 2;

    lv_obj_set_pos(img, x, y);

    //当图片rotate 0 - 10°的时候图片会变形.
    if (angle >= -4 && angle <= 4)
    {
        angle = 0;
    }
    else if (angle > 4 && angle < 10)
    {
        angle = 10;
    }
    else if (angle < -4 && angle > -10)
    {
        angle = -10;
    }
    
    lv_img_set_angle(img, angle * 10);
     
    UI_PRINTF("Angle is : %d\r\n", angle);
}


static void page_cct_settiing_storage(void)
{
	#if PROJECT_TYPE==307
    uint32_t src_crc32, dest_crc32;
	uint8_t mode = date_center_get_light_mode();
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
				memcpy(p_cct_setting_model,&g_tUIAllData.lightning2_model.mode_arg,sizeof(g_tUIAllData.lightning2_model.mode_arg)); 
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
				memcpy(p_cct_setting_model,&g_tUIAllData.strobe2_model.mode_arg,sizeof(g_tUIAllData.strobe2_model.mode_arg)); 
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
				memcpy(p_cct_setting_model,&g_tUIAllData.explosion2_model.mode_arg,sizeof(g_tUIAllData.explosion2_model.mode_arg)); 
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
				memcpy(p_cct_setting_model,&g_tUIAllData.faultybulb2_model.mode_arg,sizeof(g_tUIAllData.faultybulb2_model.mode_arg)); 
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
				memcpy(p_cct_setting_model,&g_tUIAllData.pulsing2_model.mode_arg,sizeof(g_tUIAllData.pulsing2_model.mode_arg)); 
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
				memcpy(p_cct_setting_model,&g_tUIAllData.welding2_model.mode_arg,sizeof(g_tUIAllData.welding2_model.mode_arg)); 
				 adj_btn_refresh_all(btn_list, sizeof(btn_list)/ sizeof(btn_list[0]), true);
			}
		}
		break;
		
	}
  #endif
   
}

static void page_cct_time_upd_cb(uint32_t ms)
{
	page_cct_settiing_storage();

}

