#include "../page_manager.h"
#include "../ui_common.h"
#include "../page_widget.h"
#include "../ui_data.h"

static lv_group_t* indev_group;
static void upd_del_event(my_msgbox_ext_t* ext);
const char*  highspd_mode_type(void);
static bool hs_win_type = 0;
static bool s_costom_fx_state = 0;
static uint8_t fx_event = 0;
static lv_group_t * default_group;
static lv_group_t * default_group_ls;

void page_highspd_window_construct(void)
{
	
	default_group = lv_group_get_default();
	indev_group = lv_group_create();
	default_group_ls = indev_group;
	my_indev_set_group(indev_group);
	
	lv_group_set_default(indev_group);
	my_msgbox_reset();
	my_msgbox_ext_t* p_msgbox_ext = my_msgbox_get_ext();
	//LV_IMG_DECLARE(ImgUpdateArrow);
	//p_msgbox_ext->body_comment_str = Lang_GetStringByID(STRING_ID_HIGH_SPD_WINDOW1);
	p_msgbox_ext->body_img = NULL;
	p_msgbox_ext->timecnt = 0;
	p_msgbox_ext->timeout = 0XFFFFFFFF;
	p_msgbox_ext->answer = MSGBOX_ID_NO_CONFIRM;
	p_msgbox_ext->type = MSGBOX_TYPE_TIMEOUT;
	p_msgbox_ext->btn_sel = MSGBOX_ID_NO;
	p_msgbox_ext->body_comment_str = Lang_GetStringByID(STRING_ID_HIGH_SPD_WINDOW2); 
	p_msgbox_ext->title_str = Lang_GetStringByID(STRING_ID_HIGH_SPD_MODE);
	p_msgbox_ext->msgbox_del_cb = upd_del_event;
	my_msgbox_create(p_msgbox_ext);
	
	lv_obj_t *obj = lv_obj_get_child(p_msgbox_ext->msgbox, 1);
	lv_obj_t *obj1 = lv_obj_get_child(obj, 0);
	lv_obj_t *obj2 = lv_obj_get_child(obj, 1);
	lv_obj_set_size(obj2, lv_pct(90), lv_pct(70));
	lv_obj_set_style_text_align(obj2, LV_TEXT_ALIGN_CENTER, 0);
	lv_obj_align_to(obj2, obj1, LV_ALIGN_OUT_BOTTOM_MID, 5, 16);
	
}

//static bool b_sp_deleted = true;

//static bool b_sp_deleted = true;

static void upd_del_event(my_msgbox_ext_t* ext)
{
	uint8_t  hsmode ;
    if (ext->answer == MSGBOX_ID_YES)
    {    
		 hs_win_type = 1;
		 data_center_read_config_data(SYS_CONFIG_HIGH_SPEED_MODE, &hsmode);
		 if(hsmode)
		 {
			 hsmode = 0;
			 ui_set_hs_mode_state(0);
			 data_center_write_config_data(SYS_CONFIG_HIGH_SPEED_MODE, &hsmode);	 
		 }
		 if(date_center_get_light_mode() == LIGHT_MODE_FX_LIGHTNING_II)
		 {
			 page_event_send(EVENT_DATA_PULL);
			 
		 }
//		if(s_costom_fx_state == 1 && PAGE_CUSTOM_FX == screen_get_act_pid())
//		{
//			set_hs_custom_fx_event(EVENT_DATA_HS_WINDOW);
//		
//		}
    }
    else if (ext->answer == MSGBOX_ID_NO)    
    {
		if(date_center_get_light_mode() == LIGHT_MODE_FX_LIGHTNING)
		{
			struct db_fx_lightning lightning;
			data_center_read_light_data(LIGHT_MODE_FX_LIGHTNING, &lightning);
			lightning.trigger = 0;
			data_center_write_light_data(LIGHT_MODE_FX_LIGHTNING, &lightning);
		}
        data_center_all_sys_fx_stop();

//		if(s_costom_fx_state == 1 && PAGE_CUSTOM_FX == screen_get_act_pid())
//		{
//			my_indev_set_group(default_group);	
//			lv_group_set_default(default_group);
//			lv_obj_t* obj = lv_group_get_focused(default_group);
//			lv_group_add_obj(indev_group, obj);
//			lv_group_focus_obj(obj);    
//			lv_group_set_editing(default_group, true);	
////			screen_load_page(PAGE_CUSTOM_FX, SUB_ID_CUSTOM_FX_CHOOSE, false);
////			default_group_ls = NULL;
////			s_costom_fx_state = 0;
//			return ;
//			
//		}
		page_event_send(EVENT_DATA_HS_WINDOW);
		hs_win_type = 0;
    }
	
	my_indev_set_group(default_group);	
	lv_group_set_default(default_group);  
	lv_group_set_editing(default_group, false);	
	default_group_ls = NULL;
	s_costom_fx_state = 0;
}

//const char*  highspd_mode_type(void)
//{
//	
//	const char* body_comment_str;
//	
//	switch(hs_win_type)
//	{
//		case 0:
//			body_comment_str = Lang_GetStringByID(STRING_ID_HIGH_SPD_WINDOW1);
//			break;
//		case 1:
//			body_comment_str = Lang_GetStringByID(STRING_ID_HIGH_SPD_WINDOW2);
//			break;
//		default : body_comment_str = Lang_GetStringByID(STRING_ID_HIGH_SPD_WINDOW2);
//			break;
//	}
//	return body_comment_str;
//}


void set_hs_fx_window(void)
{
	uint8_t  hsmode ;
	data_center_read_config_data(SYS_CONFIG_HIGH_SPEED_MODE, &hsmode);
	if(hsmode)
	{	
		if(default_group_ls == lv_group_get_default()){return;}
		if((date_center_get_light_mode() >= LIGHT_MODE_FX_CLUBLIGHTS && date_center_get_light_mode() < LIGHT_MODE_DMX ))
		{
			page_highspd_window_construct();
		}
	}

}

void set_hs_custom_fx_window(void)
{
	uint8_t  hsmode ;
	data_center_read_config_data(SYS_CONFIG_HIGH_SPEED_MODE, &hsmode);
	if(hsmode)
	{	
		if(default_group_ls == lv_group_get_default()){return;}
		page_highspd_window_construct();
		s_costom_fx_state = 1;
	}

}

bool get_hs_fx_window_state(void)
{
	return hs_win_type ;
}

void hs_fx_window_state_set(uint8_t state)
{
	hs_win_type = state;
}


void set_hs_custom_fx_event(uint8_t event)
{
	fx_event = event;

}

uint8_t get_hs_custom_fx_event(void)
{
	return fx_event;
	
}