/*********************
 *      INCLUDES
 *********************/
#include "../ui_common.h"
#include "../page_widget.h"

/*********************
 *      DEFINES
 *********************/
LV_IMG_DECLARE(imgElliptic);

#define DEFAULT_POWER_VALUE     (800u)

#define POWER_CIRCLE_CENTER_X                 85
#define POWER_CIRCLE_CENTER_Y                 77

#define POWER_INNER_CIRCLE_RADIUS             38
#define POWER_OUTTER_CIRCLE_RADIUS            70

#define POWER_ARC_MAX_RAD                     (73)
#define POWER_ARC_MIN_RAD                     (-74)

#if PROJECT_TYPE==307
#define MAX_POWER_VALUE     (1600u)
#define HIGH_POWER_VALUE    (1600u)
#define LOW_POWER_VALUE     (1000u)
#elif PROJECT_TYPE==308
#define MAX_POWER_VALUE     (2600u)
#define HIGH_POWER_VALUE    (1500u)
#define LOW_POWER_VALUE     (1000u)
#endif

/*********************
 *      TPEDEFS
 *********************/
enum{
	SUB_OUTPUT_MODE_LIST,
	SUB_MAX_OUTPUT_POWER_SET,
	SUB_OUTPUT_MODE_WINDOW,
	SUB_OUTPUT_MODE_REMEN_LIMIT,
	SUB_MAX_NUM,
}Output_Mode_Page_Sub;

typedef struct power_limit_type{
	uint16_t power_value;
	uint8_t  db_value;
}_power_limit_type;

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void page_outputmode_construct(void);
static void page_outputmode_destruct(void);
static void page_outputmode_event_cb(int event);
static void page_outputmode_time_cb(uint32_t ms);
static const char* outputmode_get_str(int8_t sel);
static void outputmode_release_cb(lv_event_t *e, uint8_t index);
static void my_radiobox_event_cb(lv_event_t* e);
static void max_output_btn_event_cb(lv_event_t* e);
static void power_refresh_btn_cb(lv_obj_t* parent, struct _adj_btn* dsc);
static void limit_refresh_btn_cb(lv_obj_t* parent, struct _adj_btn* dsc);

static void power_upd_run_cb(void* data);
static void db_upd_run_cb(void* data);
static void limit_upd_run_cb(void* data);
static bool power_assign_value_cb(void* data);
static bool db_assign_value_cb(void* data);
static bool limit_assign_value_cb(void* data);
static lv_obj_t* body_cont = NULL;
static void power_coor_img_refresh(void);
static void sub_output_mode_window_cb(my_msgbox_ext_t* ext);
static void refresh_scale_img(lv_obj_t* img, int16_t radius, int16_t value1, int16_t min1, int16_t max1);
/*********************
 *  STATIC VATIABLES
 *********************/
static lv_group_t *indev_group;
static my_radiobox_ext_t radiobox_ext;
static uint8_t btn_index;
static sys_config_t* p_sys_menu_model = NULL;
static uint8_t s_max_power_flag = 0;
static lv_obj_t* power_scale_img;
static uint8_t s_power_limit_change;
static adj_btn_t out_btn_list[] = {
    ADJ_BTN_INITIALIZER(550, 2800, 50, 800, "Power", "W", 3, 41, -3, false, false,  power_value_get_value_str, power_upd_run_cb, power_assign_value_cb, power_refresh_btn_cb, NULL),
	ADJ_BTN_INITIALIZER(0, 0, 0, 0, "", "", 3, 41, -3, true, false,  power_limit_get_str, limit_upd_run_cb, limit_assign_value_cb, limit_refresh_btn_cb, NULL),
};
/*********************
 *  GLOBAL VATIABLES
 *********************/
page_t page_outputmode = PAGE_INITIALIZER("Output Mode",
                                PAGE_OUTPUT_MODE, 
                                page_outputmode_construct, 
                                page_outputmode_destruct, 
                                page_outputmode_time_cb,
                                page_outputmode_event_cb,                                
                                PAGE_MENU);

/*********************
 *  GLOBAL FUNCTIONS
 *********************/

/**********************
 *   STATIC FUNCTIONS
 **********************/     

static void page_outputmode_construct(void)
{
	uint8_t spid = screen_get_act_spid();
	
    indev_group = lv_group_create();
    p_sys_menu_model = (sys_config_t*)screen_get_act_page_data();
    page_event_send(EVENT_DATA_PULL);

    my_indev_set_group(indev_group);
    lv_group_set_default(indev_group);    
    lv_obj_set_style_bg_color(lv_scr_act(), lv_color_black(), 0);

	switch(spid)
	{
		case SUB_OUTPUT_MODE_LIST:
		{
			struct sys_info_power info_power;
			
			lv_obj_t* title_label = lv_label_create(lv_scr_act());
			lv_obj_set_style_text_font(title_label, Font_ResouceGet(FONT_22_BOLD), 0);
			lv_obj_set_style_text_color(title_label, lv_color_white(), 0);
			lv_label_set_text(title_label, Lang_GetStringByID(STRING_ID_OUTPUT_MODE));
			lv_obj_align(title_label, LV_ALIGN_TOP_MID, 0, SYS_MENU_TITLE_TO_TOP);
			
			if(p_sys_menu_model->output_mode > OUTPUT_MODE_TYPE_MAX)
				p_sys_menu_model->output_mode = OUTPUT_MODE_TYPE_MAX - 1;
			radiobox_ext.sel = p_sys_menu_model->output_mode;
			data_center_read_sys_info(SYS_INFO_POWER, &info_power);
			#if PROJECT_TYPE == 308
			if(info_power.indoor_powe >= 1)
			{
				radiobox_ext.max = OUTPUT_MODE_TYPE_MAX;
				radiobox_ext.items_of_page = 3;
			}
			else
			{
				radiobox_ext.max = OUTPUT_MODE_TYPE_MAX - 1;
				radiobox_ext.items_of_page = 2;
			}
			#elif  PROJECT_TYPE == 307
			
			if(info_power.indoor_powe == 2)
			{
				radiobox_ext.max = OUTPUT_MODE_TYPE_MAX;
				radiobox_ext.items_of_page = 3;
			}
			else
			{
				radiobox_ext.max = OUTPUT_MODE_TYPE_MAX - 1;
				radiobox_ext.items_of_page = 2;
			}
			#endif
			radiobox_ext.get_name_str = outputmode_get_str;
			radiobox_ext.release_cb = outputmode_release_cb;    
			radiobox_ext.obj = my_radiobox_list_create(lv_scr_act(), lv_pct(80), &radiobox_ext, MY_RADIOBOX_TYPE2, LV_ALIGN_LEFT_MID);
			data_center_read_sys_info(SYS_INFO_POWER, &info_power);
			#if PROJECT_TYPE == 308
			if(info_power.indoor_powe >= 1)
			{
				lv_obj_t *radiobox = lv_obj_get_child(radiobox_ext.obj, 2);
				lv_obj_t *img = lv_obj_get_child(radiobox, 2);
				LV_IMG_DECLARE(ImgArrowLeft)
				lv_img_set_src(img, &ImgArrowLeft);
				lv_img_set_angle(img, 1800);
				lv_img_set_zoom(img, 200);
			}
			#elif  PROJECT_TYPE == 307
			if(info_power.indoor_powe == 2)
			{
				lv_obj_t *radiobox = lv_obj_get_child(radiobox_ext.obj, 2);
				lv_obj_t *img = lv_obj_get_child(radiobox, 2);
				LV_IMG_DECLARE(ImgArrowLeft)
				lv_img_set_src(img, &ImgArrowLeft);
				lv_img_set_angle(img, 1800);
				lv_img_set_zoom(img, 200);
			}
			#endif
			lv_obj_center(radiobox_ext.obj);
			
			lv_group_add_obj(indev_group, radiobox_ext.obj);
		}
		break;
		case SUB_MAX_OUTPUT_POWER_SET:
		{
			uint32_t key = 0;
			struct sys_info_power info_power;
			
			data_center_read_sys_info(SYS_INFO_POWER, &info_power);
			lv_obj_t* title_label = lv_label_create(lv_scr_act());
			lv_obj_set_style_text_font(title_label, Font_ResouceGet(FONT_22_BOLD), 0);
			lv_obj_set_style_text_color(title_label, lv_color_white(), 0);
			lv_label_set_text(title_label, Lang_GetStringByID(STRING_ID_MAX_OUTPUT_SET));
			lv_obj_align(title_label, LV_ALIGN_TOP_MID, 0, SYS_MENU_TITLE_TO_TOP);
			
			lv_obj_t* coor_pannel = lv_obj_create(lv_scr_act());
			lv_obj_add_style(coor_pannel, &style_rect_panel, 0);
			lv_obj_clear_flag(coor_pannel, LV_OBJ_FLAG_SCROLLABLE);
			lv_obj_set_width(coor_pannel, 174);
			lv_obj_set_height(coor_pannel, 178);
			lv_obj_align(coor_pannel, LV_ALIGN_LEFT_MID, 10, 15);
			
			lv_obj_t* temp_title = lv_label_create(coor_pannel);
			lv_obj_clear_flag(temp_title, LV_OBJ_FLAG_SCROLLABLE);
			lv_obj_add_style(temp_title, &style_rect_panel, 0);
			lv_obj_set_style_text_font(temp_title, Font_ResouceGet(FONT_18_BOLD), 0);
			lv_obj_set_style_text_color(temp_title, lv_color_make(102, 102, 102), 0);
			lv_obj_set_size(temp_title, 50, 20);
			lv_label_set_text(temp_title, "45°C");
			lv_obj_align(temp_title, LV_ALIGN_TOP_MID, -3, 20);
			
			lv_obj_t* img_pannel = lv_obj_create(coor_pannel);    
			lv_obj_remove_style_all(img_pannel);
			lv_obj_set_style_text_font(img_pannel, Font_ResouceGet(FONT_10), 0);
			lv_obj_set_style_text_color(img_pannel, lv_color_white(), 0);
			lv_obj_set_size(img_pannel, lv_pct(100), 178 - 38 - 29);    
			lv_obj_align_to(img_pannel, temp_title, LV_ALIGN_OUT_BOTTOM_MID, 0, 23);    

			LV_IMG_DECLARE(imgCoor);
			lv_obj_t* img = lv_img_create(img_pannel);
			lv_img_set_src(img, &imgCoor);    
			lv_obj_align(img, LV_ALIGN_TOP_MID, 0, 0);    

			LV_IMG_DECLARE(ImgCCTSelector)
			power_scale_img = lv_img_create(img_pannel);
			lv_obj_set_style_img_recolor_opa(power_scale_img, LV_OPA_100, 0);
			lv_obj_set_style_img_recolor(power_scale_img, lv_color_make(255, 0, 0), 0);
			lv_img_set_src(power_scale_img, &ImgCCTSelector);
			
			out_btn_list[0].value = gui_get_power_limit();
			if(ui_max_power_flag_get() == 1)
			{
				out_btn_list[0].max = MAX_POWER_VALUE;
			}
			else
			{
				#if PROJECT_TYPE == 308
				if(info_power.indoor_powe == 1)
					out_btn_list[0].max = HIGH_POWER_VALUE;
				else 
					out_btn_list[0].max = LOW_POWER_VALUE;
				#elif PROJECT_TYPE == 307
					out_btn_list[0].max = LOW_POWER_VALUE;
				#endif
			}
			lv_obj_t* cct_label_min = lv_label_create(img_pannel);    
			lv_label_set_text_fmt(cct_label_min, "%dW", out_btn_list[0].min);
			lv_obj_t* cct_label_max = lv_label_create(img_pannel);    
			lv_label_set_text_fmt(cct_label_max, "%dW", out_btn_list[0].max);
			lv_obj_align(cct_label_min, LV_ALIGN_TOP_LEFT, 10, 65);
			lv_obj_align(cct_label_max, LV_ALIGN_TOP_RIGHT, 0, 65);
			
			lv_obj_t* btn_panel = lv_obj_create(lv_scr_act());
			lv_obj_remove_style_all(btn_panel);  
			lv_obj_add_style(btn_panel, &style_rect_panel, 0);
			lv_obj_set_flex_flow(btn_panel, LV_FLEX_FLOW_COLUMN);
			lv_obj_set_style_pad_row(btn_panel, 4, 0);
			lv_obj_set_style_bg_color(btn_panel, lv_color_make(0, 0, 0), 0);
			lv_obj_set_size(btn_panel, 122, 178);
			lv_obj_align_to(btn_panel, coor_pannel, LV_ALIGN_OUT_RIGHT_TOP, 5, 0);
			
			
			lv_obj_t* power_pannel = adj_btn_create(btn_panel, LIGHT_MODE_COMMON_RADIUS, ADJ_BTN_TYPE_18);
			lv_obj_clear_flag(power_pannel, LV_OBJ_FLAG_SCROLLABLE);
			adj2_btn_init(power_pannel, &out_btn_list[0], 20);
			lv_obj_set_user_data(power_pannel, (void *)&out_btn_list[0]);
			lv_obj_set_width(power_pannel, 109);
			lv_obj_set_height(power_pannel, 127);
			lv_obj_add_event_cb(power_pannel, adj_btn2_event_cb, LV_EVENT_ALL, NULL);   
			
			lv_obj_t* power_on_pannel = adj_btn_create(btn_panel, LIGHT_MODE_COMMON_RADIUS, ADJ_BTN_TYPE_14);
			lv_obj_clear_flag(power_on_pannel, LV_OBJ_FLAG_SCROLLABLE);
			adj2_btn_init(power_on_pannel, &out_btn_list[1], 20);
			lv_obj_set_user_data(power_on_pannel, (void *)&out_btn_list[1]);
			lv_obj_set_width(power_on_pannel, 109);
			lv_obj_set_height(power_on_pannel, 45);
			lv_obj_add_event_cb(power_on_pannel, adj_btn2_event_cb, LV_EVENT_ALL, NULL);  
			lv_obj_t* label_title = lv_obj_get_child(power_on_pannel, 0);
			lv_obj_t* label_val = lv_obj_get_child(power_on_pannel, 1);
			lv_obj_add_flag(label_title, LV_OBJ_FLAG_HIDDEN);
			lv_label_set_text(label_val, power_limit_get_str((void *)&out_btn_list[1]));
			lv_obj_set_style_text_color(label_val, lv_color_make(220, 220, 220), LV_STATE_FOCUSED);
			
			if(g_tUIAllData.sys_menu_model.lang == 0)
				lv_obj_align(label_val, LV_ALIGN_CENTER, 0, 15);
			else
				lv_obj_align(label_val, LV_ALIGN_CENTER, 0, 20);
			
			refresh_scale_img(power_scale_img, POWER_OUTTER_CIRCLE_RADIUS, out_btn_list[0].value, out_btn_list[0].min, out_btn_list[0].max);
			lv_obj_add_state(power_pannel, LV_STATE_FOCUSED);
			s_power_limit_change = 1;
		}
		break;
		case SUB_OUTPUT_MODE_WINDOW:
		{
			LV_IMG_DECLARE(Imgoutput);
			my_msgbox_reset();
			my_msgbox_ext_t* p_msgbox_ext = my_msgbox_get_ext();
			p_msgbox_ext->body_img = &Imgoutput;
			p_msgbox_ext->timecnt = 0;
			p_msgbox_ext->timeout = 0XFFFFFFFF;
			p_msgbox_ext->answer = MSGBOX_ID_NO_CONFIRM;
			p_msgbox_ext->type = MSGBOX_TYPE_TIMEOUT;
			p_msgbox_ext->btn_sel = 0;
			p_msgbox_ext->body_comment_str = Lang_GetStringByID(STRING_ID_CONSTANT_SET_WINDOM); 
			p_msgbox_ext->title_str = NULL;
			p_msgbox_ext->msgbox_del_cb = sub_output_mode_window_cb;
			my_msgbox4_create(p_msgbox_ext);
		}
		break;
		case SUB_OUTPUT_MODE_REMEN_LIMIT:
		{
			lv_obj_t* title_label = lv_label_create(lv_scr_act());
			lv_obj_set_style_text_font(title_label, Font_ResouceGet(FONT_22_BOLD), 0);
			lv_obj_set_style_text_color(title_label, lv_color_white(), 0);
			lv_label_set_text(title_label, Lang_GetStringByID(STRING_ID_OUTPUT_SET_OFF_TILT));
			lv_obj_align(title_label, LV_ALIGN_TOP_MID, 0, SYS_MENU_TITLE_TO_TOP);
			
			lv_obj_t* wring_label = lv_label_create(lv_scr_act());
			lv_obj_set_style_text_font(wring_label, Font_ResouceGet(FONT_18_BOLD), 0);
			lv_obj_set_style_text_color(wring_label, lv_color_white(), 0);
			lv_label_set_text(wring_label, Lang_GetStringByID(STRING_ID_OUTPUT_WRING));
			lv_obj_align_to(wring_label, title_label, LV_ALIGN_OUT_BOTTOM_MID, 0, 20);
			
			lv_obj_t* wring_text = lv_label_create(lv_scr_act());   
			lv_obj_set_size(wring_text, lv_pct(100), lv_pct(40));
			lv_obj_set_style_text_color(wring_text, lv_color_white(), 0);
			lv_obj_set_style_text_font(wring_text, Font_ResouceGet(FONT_18), 0);
			lv_label_set_long_mode(wring_text, LV_LABEL_LONG_WRAP);
			lv_obj_set_style_text_align(wring_text, LV_TEXT_ALIGN_CENTER, 0);
			lv_label_set_text(wring_text, Lang_GetStringByID(STRING_ID_OUTPUT_WRING_TEXT));          
			lv_obj_align(wring_text, LV_ALIGN_CENTER, 0, 5);
			
			btn_index = 0;
			lv_obj_t* btn_cont = lv_obj_create(lv_scr_act());
			lv_obj_add_style(btn_cont, &style_rect_panel, 0);
			lv_obj_set_style_bg_color(btn_cont, lv_color_black(), 0);
			lv_obj_set_flex_flow(btn_cont, LV_FLEX_FLOW_ROW);
			lv_obj_set_flex_align(btn_cont, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
			lv_obj_set_style_pad_gap(btn_cont, 40, 0);
			lv_obj_set_size(btn_cont, lv_pct(100), lv_pct(30));
			lv_obj_set_user_data(btn_cont, &btn_index);
			lv_obj_align_to(btn_cont, wring_text, LV_ALIGN_OUT_BOTTOM_MID, 0, -20);  
			
			lv_obj_t* yes_btn = lv_obj_create(btn_cont);
			lv_obj_clear_flag(yes_btn, LV_OBJ_FLAG_SCROLLABLE);
			lv_obj_add_style(yes_btn, &style_common_btn, 0);
			lv_obj_add_style(yes_btn, &style_common_focued_btn, LV_STATE_FOCUSED);
			lv_obj_set_style_radius(yes_btn, 15, 0);
			lv_obj_set_style_radius(yes_btn, 15, LV_STATE_FOCUSED);        
			lv_obj_set_size(yes_btn, 75, 40);
			
			lv_obj_t* label = lv_label_create(yes_btn);
			lv_obj_set_style_text_font(label, Font_ResouceGet(FONT_18), 0);
			lv_label_set_text(label, Lang_GetStringByID(STRING_ID_CONFIRM));
			lv_obj_center(label);

			lv_obj_t* no_btn = lv_obj_create(btn_cont);
			lv_obj_clear_flag(no_btn, LV_OBJ_FLAG_SCROLLABLE);
			lv_obj_add_style(no_btn, &style_common_btn, 0);
			lv_obj_add_style(no_btn, &style_common_focued_btn, LV_STATE_FOCUSED);
			lv_obj_set_style_radius(no_btn, 15, 0);
			lv_obj_set_style_radius(no_btn, 15, LV_STATE_FOCUSED);        
			lv_obj_set_size(no_btn, 75, 40);

			lv_obj_t* label1 = lv_label_create(no_btn);
			lv_obj_set_style_text_font(label1, Font_ResouceGet(FONT_18), 0);
			lv_label_set_text(label1, Lang_GetStringByID(STRING_ID_CANCEL));
			lv_obj_center(label1);
			
			lv_obj_add_event_cb(btn_cont, max_output_btn_event_cb, LV_EVENT_ALL, NULL);
			lv_group_add_obj(indev_group,  btn_cont);
			lv_group_focus_obj(btn_cont);
			lv_group_set_editing(indev_group, true);
		}
		break;
		default:break;
	}
    
    lv_group_set_wrap(indev_group, false);
    lv_group_set_editing(indev_group, true);    
}

static void page_outputmode_destruct(void)
{
    lv_group_del(indev_group);    
    lv_obj_clean(lv_scr_act());
    indev_group = NULL; 
}

static void page_outputmode_event_cb(int event)
{
    uint8_t outputmode;
	uint16_t power_value;
	uint8_t power_limit;
    switch (event)
    {
        case EVENT_DATA_PULL:
			data_center_read_config_data(SYS_CONFIG_OUTPUT_MODE, &outputmode);
            p_sys_menu_model->output_mode = outputmode;
			s_max_power_flag = ui_max_power_flag_get();
        break;
        case EVENT_DATA_WRITE:
		{
			uint8_t spid = screen_get_act_spid();
			
			switch(spid)
			{
				case SUB_OUTPUT_MODE_LIST:
				{
					data_center_read_config_data(SYS_CONFIG_OUTPUT_MODE, &outputmode);
					if (outputmode != p_sys_menu_model->output_mode)
					{
						data_center_write_config_data(SYS_CONFIG_OUTPUT_MODE, &p_sys_menu_model->output_mode);
					}
				}
				break;
				case SUB_MAX_OUTPUT_POWER_SET:
				{
					ui_max_power_flag_set(s_max_power_flag);
				}
				break;
			}
		}
        break;
    }
}

static void page_outputmode_time_cb(uint32_t ms)
{
	uint8_t spid = screen_get_act_spid();
	static uint8_t s_time_delay = 0;
	
	switch(spid)
	{
		case SUB_OUTPUT_MODE_LIST:
		{
			uint8_t output_mode = p_sys_menu_model->output_mode;
			//从数据中心拉取输出模式    
			data_center_read_config_data(SYS_CONFIG_OUTPUT_MODE, &output_mode);

			//如果输出模式在后台被更新, 那么更新界面.
			if (radiobox_ext.obj && (output_mode != p_sys_menu_model->output_mode))
			{
				radiobox_ext.sel = p_sys_menu_model->output_mode;
				lv_event_send(radiobox_ext.obj, LV_EVENT_FOCUSED, NULL);
			}
		}
		break;
		case SUB_MAX_OUTPUT_POWER_SET:
		{
			uint32_t key = 0;
			if(s_power_limit_change == 1)
			{
				if(s_time_delay ++ >= 1)
				{
					s_time_delay = 0;
					s_power_limit_change = 0;
					lv_obj_t* pre_btn = lv_obj_get_child(lv_scr_act(), 2);
					lv_obj_t* power_btn = lv_obj_get_child(pre_btn, 0);
					lv_obj_t* limit_btn = lv_obj_get_child(pre_btn, 1);
				
					lv_group_add_obj(indev_group, power_btn);
					lv_group_add_obj(indev_group, limit_btn);
					lv_group_focus_obj(power_btn);
					lv_group_set_wrap(indev_group, false);
					key = LV_KEY_BACKSPACE;
					lv_event_send(power_btn, LV_EVENT_KEY, &key);
				}
			}
		}
		break;
		default:break;
	}
}

static void outputmode_release_cb(lv_event_t *e, uint8_t index)
{
    lv_event_code_t event = lv_event_get_code(e);

    shortcut_keys_event_handler(e);

    if (event == LV_EVENT_RELEASED)
    {
		if(index <= 1)
		{
			if(index == 1)
			{
				p_sys_menu_model->output_mode = index;
				data_center_write_config_data(SYS_CONFIG_OUTPUT_MODE, &p_sys_menu_model->output_mode);
				if(ui_get_master_mode())
				{
					extern void gui_data_sync_event_gen(void);		
					gui_data_sync_event_gen();
				}
				screen_turn_prev_page();
			}
			else
			{
				#if PROJECT_TYPE==308
				p_sys_menu_model->output_mode = 0;
				data_center_write_config_data(SYS_CONFIG_OUTPUT_MODE, &p_sys_menu_model->output_mode);
				if(ui_get_master_mode())
				{
					extern void gui_data_sync_event_gen(void);		
					gui_data_sync_event_gen();
				}
				screen_turn_prev_page();
				#endif
				#if PROJECT_TYPE==307
				screen_load_page_and_index(PAGE_OUTPUT_MODE, SUB_OUTPUT_MODE_WINDOW, 0,0);
				#endif
			}
		}
		else
		{
			screen_load_sub_page(SUB_MAX_OUTPUT_POWER_SET, 0);
		}
    }    
    else if (event == LV_EVENT_KEY)
    {
        uint32_t key = *(uint32_t *)lv_event_get_param(e);
        
        if (key == LV_KEY_BACKSPACE)
        {
            screen_turn_prev_page();
        }
    }        
}   

static void max_output_btn_event_cb(lv_event_t* e)
{
    lv_event_code_t event = lv_event_get_code(e);
    lv_obj_t* msgbox = lv_event_get_target(e);
	uint8_t *btn_sel = lv_obj_get_user_data(msgbox);
	
    if (event == LV_EVENT_KEY)
    {
        uint32_t key = *(uint32_t*)lv_event_get_param(e);
        uint8_t childs_sum = lv_obj_get_child_cnt(msgbox);
        bool b_upd = false;
		int8_t sel = *btn_sel;
        
        if (LV_ENCODER_KEY_CW == key)
        {
            sel = sel + 1;
            if (sel < childs_sum)
            {                
                b_upd = true;
            }
        }
        else if (LV_ENCODER_KEY_CC == key)
        {
            sel = sel - 1;
            if (sel >= 0)
            {                
                b_upd = true;
            }
        }
        else if (LV_KEY_BACKSPACE == key)
        {
            screen_load_sub_page(SUB_MAX_OUTPUT_POWER_SET, 0);                 
        }

        if (b_upd)
        {
            lv_obj_t* obj = lv_obj_get_child(msgbox, *btn_sel);
            lv_obj_clear_state(obj, LV_STATE_FOCUSED);
            lv_event_send(obj, LV_EVENT_DEFOCUSED, NULL);
            *btn_sel = sel;
            obj = lv_obj_get_child(msgbox, *btn_sel);
            lv_obj_add_state(obj, LV_STATE_FOCUSED);
            lv_event_send(obj, LV_EVENT_FOCUSED, NULL);
        }
    }  
    else if (event == LV_EVENT_FOCUSED)
    {
        lv_obj_t* child = lv_obj_get_child(msgbox, *btn_sel);
        if (child != NULL)
        {
            lv_obj_add_state(child, LV_STATE_FOCUSED);
        }
    }
    else if (event == LV_EVENT_RELEASED)
    {       
		if(*btn_sel == 0)
		{
				ui_max_power_flag_set(1);
		}
		screen_load_sub_page(SUB_MAX_OUTPUT_POWER_SET, 0);
    }
}

static void sub_output_mode_window_cb(my_msgbox_ext_t* ext)
{
	p_sys_menu_model->output_mode = 0;
	data_center_write_config_data(SYS_CONFIG_OUTPUT_MODE, &p_sys_menu_model->output_mode);
	if(ui_get_master_mode())
	{
		extern void gui_data_sync_event_gen(void);		
		gui_data_sync_event_gen();
	}
	screen_turn_prev_page();       

} 


static const char* outputmode_get_str(int8_t sel)
{
    switch(sel)
    {
		case OUTPUT_MODE_CONST_OUTPUT:    lv_snprintf(sdb, sizeof(sdb), "%s", Lang_GetStringByID(STRING_ID_CONSTANT_OUTPUT)); break;
		case OUTPUT_MODE_MAX_OUTPUT:      lv_snprintf(sdb, sizeof(sdb), "%s", Lang_GetStringByID(STRING_ID_MAX_OUTPUT)); break;
		case OUTPUT_MODE_MAX_OUTPUT_SET:  lv_snprintf(sdb, sizeof(sdb), "%s", Lang_GetStringByID(STRING_ID_MAX_OUTPUT_SET)); break;
    } 
    return sdb;
}

static void power_upd_run_cb(void* data)
{
    adj_btn_t* dsc = (adj_btn_t*)data;
	
	refresh_scale_img(power_scale_img, POWER_OUTTER_CIRCLE_RADIUS, dsc->value, dsc->min, dsc->max);
	power_refresh_btn_cb(dsc->obj, dsc);
	gui_set_power_limit(dsc->value);
}

static void db_upd_run_cb(void* data)
{     
    page_event_send(EVENT_DATA_WRITE);
}

static void limit_upd_run_cb(void* data)
{
    adj_btn_t* dsc = (adj_btn_t*)data;
}

static bool power_assign_value_cb(void* data)
{
    adj_btn_t* dsc = (adj_btn_t*)data;
    ASSIGN_VALUE_TO_BTN(dsc->value);
}

static void power_refresh_btn_cb(lv_obj_t* parent, struct _adj_btn* dsc)
{
	lv_obj_t* label_title = lv_obj_get_child(dsc->obj, 0);
    lv_obj_t* label_val = lv_obj_get_child(dsc->obj, 1);
	
	lv_state_t state = lv_obj_get_state(dsc->obj); 

    lv_label_set_text(label_title, dsc->name);
    if (state & LV_STATE_EDITED)
    {
		lv_obj_set_style_text_color(label_title, lv_color_make(255,255,255), LV_STATE_FOCUSED);
    }
    else 
    {
        lv_obj_set_style_text_color(label_title, lv_color_make(220, 220, 220), LV_STATE_FOCUSED);
    }
	
    lv_obj_align(label_title, LV_ALIGN_BOTTOM_MID, 0, -20);
	
	if(dsc->div_disable)
	{
		lv_obj_add_flag(label_title, LV_OBJ_FLAG_HIDDEN);
	}
	
	lv_obj_set_style_text_font(label_val, Font_ResouceGet(FONT_24_BOLD), 0);
	if (dsc->get_value_str_cb)
    {
        const char* str = dsc->get_value_str_cb((void*)dsc);
        lv_label_set_text(label_val, str);
    }    
	if (!dsc->div_disable)
		lv_obj_align(label_val, LV_ALIGN_CENTER, 0, -10);
}

static void limit_refresh_btn_cb(lv_obj_t* parent, struct _adj_btn* dsc)
{
	lv_state_t state = lv_obj_get_state(dsc->obj); 
    if (state & LV_STATE_EDITED)
    {
		screen_load_sub_page(SUB_OUTPUT_MODE_REMEN_LIMIT, 0);
    }
}

static bool db_assign_value_cb(void* data)
{
    adj_btn_t* dsc = (adj_btn_t*)data;
//	uint8_t db_value;
    ASSIGN_VALUE_TO_BTN(dsc->value);
}

static bool limit_assign_value_cb(void* data)
{
    adj_btn_t* dsc = (adj_btn_t*)data;
    ASSIGN_VALUE_TO_BTN(p_sys_menu_model->power_limit);
}

static void power_coor_img_refresh(void)
{
	int8_t power_coor_x = 60;
	int8_t power_coor_y = 108;
	int8_t power_coor_end_x = 0;
	int8_t power_coor_end_y = 0;
	
	power_coor_end_x = ((p_sys_menu_model->power_value - DEFAULT_POWER_VALUE) / 100) * 2 + power_coor_x;
	power_coor_end_y = power_coor_y - ((p_sys_menu_model->power_value - DEFAULT_POWER_VALUE) / 100) * 2;
	
	lv_obj_t *btn = lv_obj_get_child(lv_scr_act(), 1);
	lv_obj_t *img = lv_obj_get_child(btn, 1);
	
	lv_obj_set_pos(img, power_coor_end_x, power_coor_end_y);
}

static void refresh_scale_img(lv_obj_t* img, int16_t radius, int16_t value1, int16_t min1, int16_t max1)
{
    lv_coord_t x, y;
    const lv_img_dsc_t *pdsc;
    int16_t angle;

    if (img == NULL)   
        return ;

    pdsc = (lv_img_dsc_t*)lv_img_get_src(img);

    /* 角度 */
    angle = lv_map(value1, min1, max1, POWER_ARC_MIN_RAD, POWER_ARC_MAX_RAD);

    x = POWER_CIRCLE_CENTER_X + (radius * lv_trigo_sin(angle) >> LV_TRIGO_SHIFT);
    y = POWER_CIRCLE_CENTER_Y - (radius * lv_trigo_cos(angle) >> LV_TRIGO_SHIFT);

    x -= pdsc->header.w / 2;
    y -= pdsc->header.h / 2;

    lv_obj_set_pos(img, x, y);

    /* 当图片rotate 0 - 10°的时候图片会变形 */
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

#if 0
    // static lv_point_t line_points[] = { {85, 77}, {70, 70} };
    // lv_coord_t x =  85 + (CCT_OUTTER_CIRCLE_RADIUS * lv_trigo_sin(74) >> LV_TRIGO_SHIFT);
    // lv_coord_t y = 77 - (CCT_OUTTER_CIRCLE_RADIUS * lv_trigo_cos(74) >> LV_TRIGO_SHIFT);

    // line_points[1].x = x;
    // line_points[1].y = y;

    // lv_obj_t* line = lv_line_create(img_pannel);
    // lv_line_set_points(line, line_points, 2);
    // lv_obj_set_style_line_color(line, lv_color_make(0, 255, 0), 0);
    // lv_obj_set_style_line_width(line, 1, 0);
#endif    
}
