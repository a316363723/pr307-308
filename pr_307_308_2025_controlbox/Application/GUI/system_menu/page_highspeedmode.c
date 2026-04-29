/*********************
 *      INCLUDES
 *********************/
#include "../page_manager.h"
#include "../ui_common.h"
#include "../page_widget.h"
#include "../ui_data.h"
#include "os_event.h"
 /*********************
  *      DEFINES
  *********************/
enum {
	HIGH_SPEED_SETTING = 0,
	HIGH_SPEED_WINDOW  = 1,
	HIGH_SPEED_DMX_WINDOW  = 2,
};
  /*********************
   *      TPEDEFS
   *********************/

   /**********************
    *  STATIC PROTOTYPES
    **********************/
static void page_highspd_construct(void);
static void page_highspd_destruct(void);
static void refresh_high_spd_mode_disp(lv_obj_t* parent);
static void high_spd_mode_event_cb(lv_event_t* e);
static void page_hsmode_timer_cb(uint32_t ms);
static void page_hsmode_event_cb(int event);
static void upd_del_event(my_msgbox_ext_t* ext);
static void upd_de2_event(my_msgbox_ext_t* ext);
/*********************
 *  STATIC VATIABLES
 *********************/
static lv_group_t* indev_group;
static sys_config_t* p_sys_menu_model = NULL;
static lv_obj_t* body_cont = NULL;
static uint8_t hs_window = 0;

/*********************
    *  GLOBAL VATIABLES
 *********************/
page_t page_high_spd_mode = PAGE_INITIALIZER("High_Speed Mode",
    PAGE_HIGH_SPD_MODE,
    page_highspd_construct,
    page_highspd_destruct,
    page_hsmode_timer_cb,
    page_hsmode_event_cb,
    PAGE_MENU);

static void page_highspd_construct(void)
{
	uint8_t spid = screen_get_act_spid();

	switch(spid)
	{
		case HIGH_SPEED_SETTING:
		{
			hs_window = 1;
			indev_group = lv_group_create();
			my_indev_set_group(indev_group);
			lv_group_set_default(indev_group);
			lv_obj_set_style_bg_color(lv_scr_act(), lv_color_black(), 0);
			p_sys_menu_model = (sys_config_t*)screen_get_act_page_data();
			page_event_send(EVENT_DATA_PULL);
			
			lv_obj_t* title_label = lv_label_create(lv_scr_act());
			lv_obj_set_style_text_font(title_label, Font_ResouceGet(FONT_22_BOLD), 0);
			lv_obj_set_style_text_color(title_label, lv_color_white(), 0);
			lv_label_set_text(title_label, Lang_GetStringByID(STRING_ID_HIGH_SPD_MODE));
			lv_obj_align(title_label, LV_ALIGN_TOP_MID, 0, SYS_MENU_TITLE_TO_TOP);

			body_cont = lv_obj_create(lv_scr_act());
			lv_obj_remove_style_all(body_cont);
			lv_obj_set_size(body_cont, 320, 200);
			lv_obj_align_to(body_cont, title_label, LV_ALIGN_OUT_BOTTOM_MID, 0, 0);
			lv_obj_add_event_cb(body_cont, high_spd_mode_event_cb, LV_EVENT_ALL, NULL);
			//lv_obj_center(body_cont);

			LV_IMG_DECLARE(ImgHighSpdMode);
			lv_obj_t* img1 = lv_img_create(body_cont);
			lv_img_set_src(img1, &ImgHighSpdMode);
			lv_obj_align(img1, LV_ALIGN_TOP_MID, 0, 10);

			lv_obj_t* img_btn = lv_img_create(body_cont);
//			refresh_high_spd_mode_disp(body_cont);
			
			LV_IMG_DECLARE(ImgStudioOn);
			LV_IMG_DECLARE(ImgStudioOff);
			if (p_sys_menu_model->hs_mode == STUDIO_MODE_ON)
			{
				lv_obj_set_style_img_recolor(img1, lv_color_white(), 0);
				lv_obj_set_style_img_recolor_opa(img1, LV_OPA_100, 0);
				lv_img_set_src(img_btn, &ImgStudioOn);
				lv_obj_align_to(img_btn, img1, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);
			}
			else
			{
				lv_obj_set_style_img_recolor(img1, lv_color_make(179, 179, 179), 0);
				lv_obj_set_style_img_recolor_opa(img1, LV_OPA_100, 0);
				lv_img_set_src(img_btn, &ImgStudioOff);               
				lv_obj_align_to(img_btn, img1, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);
			}
			lv_obj_t* label = lv_label_create(body_cont);
			lv_obj_set_style_text_font(label, Font_ResouceGet(FONT_14), 0);
			lv_obj_set_style_text_color(label, lv_color_white(), 0);
			lv_label_set_long_mode(label, LV_LABEL_LONG_WRAP);     /*Break the long lines*/
			lv_obj_set_width(label, 296);  /*Set smaller width to make the lines wrap*/
			lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, 0);
			lv_label_set_text(label, Lang_GetStringByID(STRING_ID_HIGH_SPD_WARN));
			lv_obj_align_to(label, img_btn, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);
			
			lv_group_add_obj(indev_group, body_cont);
			lv_group_set_wrap(indev_group, false);
			lv_group_set_editing(indev_group, true);
		}
		break;
		case HIGH_SPEED_WINDOW:
		{
			indev_group = lv_group_create();
			my_indev_set_group(indev_group);
			lv_group_set_default(indev_group);
			my_msgbox_reset();
			my_msgbox_ext_t* p_msgbox_ext = my_msgbox_get_ext();
			p_msgbox_ext->body_img = NULL;
			p_msgbox_ext->timecnt = 0;
			p_msgbox_ext->timeout = 0xffffffff;
			p_msgbox_ext->answer = MSGBOX_ID_NO_CONFIRM;
			p_msgbox_ext->type = MSGBOX_TYPE_TIMEOUT;
			p_msgbox_ext->btn_sel = MSGBOX_ID_NO;
			p_msgbox_ext->body_comment_str = Lang_GetStringByID(STRING_ID_HIGH_SPD_WINDOW1);
			p_msgbox_ext->title_str = Lang_GetStringByID(STRING_ID_HIGH_SPD_MODE);
			p_msgbox_ext->msgbox_del_cb = upd_del_event;
			my_msgbox_create(p_msgbox_ext);
			
			lv_obj_t *obj = lv_obj_get_child(p_msgbox_ext->msgbox, 1);
			lv_obj_t *obj1 = lv_obj_get_child(obj, 0);
			lv_obj_t *obj2 = lv_obj_get_child(obj, 1);
			lv_obj_set_size(obj2, lv_pct(90), lv_pct(70));
			lv_obj_set_style_text_align(obj2, LV_TEXT_ALIGN_CENTER, 0);
			lv_obj_align_to(obj2, obj1, LV_ALIGN_OUT_BOTTOM_MID, 5, 16);
			break;
		}
		
		case HIGH_SPEED_DMX_WINDOW:
		{
			indev_group = lv_group_create();
			my_indev_set_group(indev_group);
			lv_group_set_default(indev_group);
			my_msgbox_reset();
			my_msgbox_ext_t* p_msgbox_ext = my_msgbox_get_ext();
			//LV_IMG_DECLARE(ImgUpdateArrow);
			//p_msgbox_ext->body_comment_str = Lang_GetStringByID(STRING_ID_HIGH_SPD_WINDOW1);
			p_msgbox_ext->body_img = NULL;
			p_msgbox_ext->timecnt = 0;
			p_msgbox_ext->timeout = 0xffffffff;
			p_msgbox_ext->answer = MSGBOX_ID_NO_CONFIRM;
			p_msgbox_ext->type = MSGBOX_TYPE_TIMEOUT;
			p_msgbox_ext->btn_sel = 0;
			p_msgbox_ext->body_comment_str = Lang_GetStringByID(STRING_ID_HIGH_SPD_WINDOW3);
			p_msgbox_ext->title_str = Lang_GetStringByID(STRING_ID_HIGH_SPD_MODE);
			p_msgbox_ext->msgbox_del_cb = upd_de2_event;
			my_msgbox1_create(p_msgbox_ext);
			break;
		}
	
	}
    
}


static void upd_del_event(my_msgbox_ext_t* ext)
{
	struct db_cct cct;
    if (ext->answer == MSGBOX_ID_YES)
    {    
		hs_window = 0;
		screen_load_sub_page(HIGH_SPEED_SETTING, 0);  
		p_sys_menu_model->hs_mode = !p_sys_menu_model->hs_mode;
		page_event_send(EVENT_DATA_WRITE);
		
		data_center_read_light_data(LIGHT_MODE_CCT, &cct);
		data_center_write_light_data(LIGHT_MODE_CCT, &cct);
		if(date_center_get_light_mode() == LIGHT_MODE_FX_LIGHTNING)
		{
			struct db_fx_lightning lightning;
			data_center_read_light_data(LIGHT_MODE_FX_LIGHTNING, &lightning);
			lightning.trigger = 0;
			data_center_write_light_data(LIGHT_MODE_FX_LIGHTNING, &lightning);
		}
        data_center_all_sys_fx_stop();
    }
    else if (ext->answer == MSGBOX_ID_NO)    
    {
		screen_load_sub_page(HIGH_SPEED_SETTING, 0);   
    }
}


static void upd_de2_event(my_msgbox_ext_t* ext)
{
	screen_load_sub_page(HIGH_SPEED_SETTING, 0);  
}


static void page_highspd_destruct(void)
{
    lv_group_focus_freeze(indev_group, true);
    lv_group_remove_all_objs(indev_group);
    lv_group_del(indev_group);
    indev_group = NULL;

    lv_obj_clean(lv_scr_act());
}



static void page_hsmode_timer_cb(uint32_t ms)
{
//    page_invalid_type_t type;

//    page_event_send(EVENT_DATA_WRITE);
//    type = page_get_invalid_type();
//    if (type != PAGE_INVALID_TYPE_IDLE)
//    {
//        page_set_invalid_type(PAGE_INVALID_TYPE_IDLE);
//        //refresh_hsmode_disp();
//    }
}

static void page_hsmode_event_cb(int event)
{
    uint8_t hsmode;
    
    data_center_read_config_data(SYS_CONFIG_HIGH_SPEED_MODE, &hsmode);
    switch (event)
    {
        case EVENT_DATA_PULL:
            p_sys_menu_model->hs_mode   = hsmode;
            page_set_invalid_type(PAGE_INVALID_TYPE_ALL);
            break;
        case EVENT_DATA_WRITE:
            if (hsmode != p_sys_menu_model->hs_mode)
            {
                data_center_write_config_data(SYS_CONFIG_HIGH_SPEED_MODE, &p_sys_menu_model->hs_mode);
            }
            break;
        default:
            break;
    }
}

static void refresh_high_spd_mode_disp(lv_obj_t* parent)
{
    LV_IMG_DECLARE(ImgStudioOn)
    LV_IMG_DECLARE(ImgStudioOff)

    lv_obj_t* img1 = lv_obj_get_child(parent, 0);
    lv_obj_t* img2 = lv_obj_get_child(parent, 1);
	
    if (p_sys_menu_model->hs_mode == STUDIO_MODE_ON)
    {
		if(ui_get_high_speed_switch() == 1 && hs_window)
		{
			p_sys_menu_model->hs_mode = !p_sys_menu_model->hs_mode;
			screen_load_sub_page(HIGH_SPEED_WINDOW, 0);  
			return;
		}
		else if(ui_get_high_speed_switch() == 2 && hs_window)
		{
			p_sys_menu_model->hs_mode = !p_sys_menu_model->hs_mode;
			screen_load_sub_page(HIGH_SPEED_DMX_WINDOW, 0);  
			return;
		}
		else 
		{
			lv_obj_set_style_img_recolor(img1, lv_color_white(), 0);
			lv_obj_set_style_img_recolor_opa(img1, LV_OPA_100, 0);
			lv_img_set_src(img2, &ImgStudioOn);
			lv_obj_align_to(img2, img1, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);
		}
    }
    else
    {
        lv_obj_set_style_img_recolor(img1, lv_color_make(179, 179, 179), 0);
		lv_obj_set_style_img_recolor_opa(img1, LV_OPA_100, 0);

        lv_img_set_src(img2, &ImgStudioOff);               
        lv_obj_align_to(img2, img1, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);
    }
	
}

static void high_spd_mode_event_cb(lv_event_t* e)
{
    lv_event_code_t event = lv_event_get_code(e);

    shortcut_keys_event_handler(e);

    if (event == LV_EVENT_RELEASED)
    {
        UI_PRINTF("high speed Release Callback!\r\n");
		p_sys_menu_model->hs_mode = !p_sys_menu_model->hs_mode;
        refresh_high_spd_mode_disp(body_cont);
        page_event_send(EVENT_DATA_WRITE);
		if(ui_get_master_mode())
		{
			extern void gui_data_sync_event_gen(void);		
			gui_data_sync_event_gen();
		}
    }
    else if (event == LV_EVENT_KEY)
    {
        uint32_t key = *(uint32_t*)lv_event_get_param(e);

        if (key == LV_KEY_BACKSPACE)
        {
            screen_turn_prev_page();
        }
    }
}
