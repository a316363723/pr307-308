/*********************
 *      INCLUDES
 *********************/
#include "../ui_common.h"
#include "../page_widget.h"
#include "ui_data.h"

/*********************
 *      DEFINES
 *********************/

/*********************
 *      TPEDEFS
 *********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void page_workmode_construct(void);
static void page_workmode_destruct(void);
static void page_workmode_time_upd_cb(uint32_t ms);

static void work_mode_list_panel_init(void);
static void work_mode_sync_init(void);
static void workmode_sync_updata_seletc_init(void);
static void worknode_sync_updataing_init(void);
static void work_upgrade_confirm_init(void);
static void ctrl_sys_load_sub_page(void);
static void refresh_upd_bar(void);
static void workmode_update_com_event_cb(lv_event_t* e);
static void work_follwo_mode_init(void);
static void follow_mode_even_cb(lv_event_t* e);
static const char* work_update_name_get_str(int8_t x);
static void work_update_choose_list_user_event(lv_event_t* e, uint8_t index);
static void workmode_sync_btn_event(lv_event_t* e);
static void work_upd_config_event(my_msgbox_ext_t* ext);
/*********************
 *  STATIC VATIABLES
 *********************/
static lv_group_t *indev_group;
static sys_config_t* p_sys_menu_model = NULL;
static my_list_ext_t list_ext;
static lv_obj_t* com_setting_list = NULL;
static lv_obj_t* workmode_img;
static int32_t disp_time;
static int32_t life_period;
static uint8_t work_lang_last_type;
static my_radiobox_ext_t s_work_radiobox_ext;
static lv_obj_t* s_work_scroll_cont = NULL;
/*********************
 *  GLOBAL VATIABLES
 *********************/
page_t page_workmode = PAGE_INITIALIZER("Work Mode",
                                PAGE_WORK_MODE, 
                                page_workmode_construct, 
                                page_workmode_destruct, 
                                page_workmode_time_upd_cb,
                                NULL,                                
                                PAGE_MENU);

LV_IMG_DECLARE(ImgWorkMode);

/*********************
 *  GLOBAL FUNCTIONS
 *********************/

/**********************
 *   STATIC FUNCTIONS
 **********************/                            
static void page_workmode_construct(void)
{
    indev_group = lv_group_create();
    my_indev_set_group(indev_group);
    lv_group_set_default(indev_group);
    
    lv_obj_set_style_bg_color(lv_scr_act(), lv_color_black(), 0);

    p_sys_menu_model = (sys_config_t*)screen_get_act_page_data();

    uint8_t spid = screen_get_act_spid();

    switch (spid)
    {
        case SUB_ID_WORK_MODE_LIST:              work_mode_list_panel_init(); break;
        case SUB_ID_SYNC_CONFIRM:                work_mode_sync_init();  break;
		case SUB_ID_SYNC_FOLLWO_MODE:			 work_follwo_mode_init(); break;
		case SUB_ID_SYNC_UPDATA_SELECT:          workmode_sync_updata_seletc_init();break;
		case SUB_ID_SYNC_UPDATE_CONFIRM:	     work_upgrade_confirm_init();break;
		case SUB_ID_SYNC_UPDATAING:              worknode_sync_updataing_init();break;
		case SUB_ID_SYNC_NO_FIRMWARE:
        case SUB_ID_SYNC_UPDATE_FAILED:
        case SUB_ID_SYNC_UPDATA_COMPLETE:
        case SUB_ID_SYNC_NOT_FOUND_UPD_DEV:
        case SUB_ID_SYNC_U_DISK_FORMAT_ERR:
        {     
            lv_obj_t* panel = NULL;                        
            disp_time = 0;

            if (spid == SUB_ID_SYNC_UPDATE_FAILED)
            {
                LV_IMG_DECLARE(ImgConfirmFailed)
                life_period = 2 * 500;
                panel = warning_info_init(lv_scr_act(), &ImgConfirmFailed, 54, 25, Lang_GetStringByID(STRING_ID_UPD_FAILED));
                lv_obj_add_event_cb(panel, workmode_update_com_event_cb, LV_EVENT_ALL, NULL);
            }
            else if (spid == SUB_ID_SYNC_UPDATA_COMPLETE)
            {
                LV_IMG_DECLARE(ImgConfirmOk)                
                life_period = 2 * 500;
				lv_snprintf(sdb, sizeof(sdb), "%s", Lang_GetStringByID(STRING_ID_SYNC_UPDATING));
                panel = warning_info_init(lv_scr_act(), &ImgConfirmOk, 54, 25, sdb);
				lv_obj_clear_flag(lv_scr_act(), LV_OBJ_FLAG_SCROLLABLE);
				lv_obj_t* title_label = lv_label_create(lv_scr_act());
				lv_obj_set_style_text_font(title_label, Font_ResouceGet(FONT_22_BOLD), 0);
				lv_obj_set_style_text_color(title_label, lv_color_white(), 0);
				lv_label_set_text(title_label, Lang_GetStringByID(STRING_ID_SYNC_UPDATE_OK));
				lv_obj_align_to(title_label, lv_scr_act(), LV_ALIGN_TOP_MID, 0, 10);
                lv_obj_add_event_cb(panel, workmode_update_com_event_cb, LV_EVENT_ALL, NULL);
            }
            else if (spid == SUB_ID_SYNC_NOT_FOUND_UPD_DEV)
            {
                LV_IMG_DECLARE(ImgConfirmFailed)
                life_period = 4 * 500;
                panel = warning_info_init(lv_scr_act(), &ImgConfirmFailed, 54, 25, Lang_GetStringByID(STRING_ID_NO_UPD_DEV));                                               
                lv_obj_add_event_cb(panel, workmode_update_com_event_cb, LV_EVENT_ALL, NULL);
            }
            else if (spid == SUB_ID_SYNC_NO_FIRMWARE)
            {
                LV_IMG_DECLARE(ImgConfirmFailed)
                life_period = 2 * 500;
                panel = warning_info_init(lv_scr_act(), &ImgConfirmFailed, 54, 25, Lang_GetStringByID(STRING_ID_NO_FIRMWARE));                
                lv_obj_add_event_cb(panel, workmode_update_com_event_cb, LV_EVENT_ALL, NULL);
            }
            else if (spid == SUB_ID_SYNC_U_DISK_FORMAT_ERR)
            {
                LV_IMG_DECLARE(ImgConfirmFailed)
                life_period = 2 * 500;
                panel = warning_info_init(lv_scr_act(), &ImgConfirmFailed, 54, 25, Lang_GetStringByID(STRING_ID_FORMAT_ERR));                
                lv_obj_add_event_cb(panel, workmode_update_com_event_cb, LV_EVENT_ALL, NULL);
            }

            lv_group_add_obj(indev_group, panel);
            lv_group_focus_obj(panel);
        }
        break;
        default:break;
    }

}

static void page_workmode_destruct(void)
{
    lv_group_focus_freeze(indev_group, true);
    lv_group_remove_all_objs(indev_group);
    lv_group_del(indev_group);
    indev_group = NULL; 

    lv_obj_clean(lv_scr_act());
}

static void workmode_sync_btn_event(lv_event_t* e)
{
    lv_event_code_t event = lv_event_get_code(e);
    lv_obj_t* btn = lv_event_get_target(e);

    shortcut_keys_event_handler(e);

    if (event == LV_EVENT_RELEASED)
    {
        uint8_t index = lv_obj_get_child_id(btn);
		ui_set_work_update_number_flag(1);
    }
    else if (event == LV_EVENT_KEY)
    {
        uint32_t key = *(uint32_t*)lv_event_get_param(e);
        if (key == LV_KEY_BACKSPACE)
        {
            ctrl_sys_load_sub_page();
        }
    }
}

/////////////////////////////
/**
 * @brief 根据当前ID返回上一级
 *
 */
static void ctrl_sys_load_sub_page(void)
{
    uint8_t def_spid = 0, def_index = 0;
    uint8_t subid = screen_get_act_spid();

    switch (subid)
    {
		case SUB_ID_WORK_MODE_LIST:            
			screen_turn_prev_page();
		break;
        case SUB_ID_SYNC_CONFIRM:            
			def_spid = SUB_ID_WORK_MODE_LIST; def_index = 1;
			screen_load_prev_sub_page(def_spid, def_index);  
		break;
        default: break;
    }
    
}

static void sync_hidden_setting_refresh(void)
{
    if (com_setting_list == NULL)   return;

    lv_obj_t* btn1 = lv_obj_get_child(com_setting_list, 0);
    lv_obj_t* btn2 = lv_obj_get_child(com_setting_list, 1);

    if (p_sys_menu_model->work_mode)
    {
        lv_obj_clear_flag(btn2, LV_OBJ_FLAG_HIDDEN);

        lv_obj_set_style_img_recolor(workmode_img, lv_color_make(0xff, 0xff, 0xff), 0);
        lv_obj_set_style_img_recolor_opa(workmode_img, LV_OPA_100, 0);
    }
    else
    {
        lv_obj_add_flag(btn2, LV_OBJ_FLAG_HIDDEN);
        lv_obj_set_style_img_recolor(workmode_img, lv_color_make(0x80, 0x80, 0x80), 0);
        lv_obj_set_style_img_recolor_opa(workmode_img, LV_OPA_100, 0);
    }
}

static void workmode_switch_event_cb(lv_event_t* e)
{
    lv_event_code_t event = lv_event_get_code(e);
    lv_obj_t* btn = lv_event_get_target(e);
//    ui_title_t* p_title = &g_ptUIStateData->title;

    shortcut_keys_event_handler(e);

    if (event == LV_EVENT_RELEASED)
    {
        p_sys_menu_model->work_mode = !p_sys_menu_model->work_mode;
		
		data_center_write_config_data_no_event(SYS_CONFIG_WORK_MODE, &p_sys_menu_model->work_mode);
        list_switch_btn_refresh(btn, p_sys_menu_model->work_mode);
        sync_hidden_setting_refresh();
		if(ui_get_master_mode())
		{
			extern void gui_data_sync_event_gen(void);		
			g_ptUIStateData->work_mode_sync = 1;
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


static void work_mode_list_panel_init(void)
{
    lv_obj_t* title_label = lv_label_create(lv_scr_act());
    lv_obj_set_style_text_font(title_label, Font_ResouceGet(FONT_22_BOLD), 0);
    lv_obj_set_style_text_color(title_label, lv_color_white(), 0);
    lv_label_set_text(title_label, Lang_GetStringByID(STRING_ID_WORK_MODE));
    lv_obj_align(title_label, LV_ALIGN_TOP_MID, 0, SYS_MENU_TITLE_TO_TOP);
	
	data_center_read_config_data(SYS_CONFIG_WORK_MODE, &p_sys_menu_model->work_mode);
    workmode_img = lv_img_create(lv_scr_act());
    lv_img_set_src(workmode_img, &ImgWorkMode);
    lv_obj_align_to(workmode_img, title_label, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);

    com_setting_list = my_list_create(lv_scr_act(), lv_pct(83), lv_pct(38), &list_ext);
    lv_obj_set_flex_align(com_setting_list, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_align(com_setting_list, LV_ALIGN_TOP_MID, 0, 129);

    lv_obj_t* switch_btn = my_list_add_switch_btn(com_setting_list, Lang_GetStringByID(STRING_ID_WORKMODE_STATE), p_sys_menu_model->work_mode);
    lv_obj_add_event_cb(switch_btn, workmode_switch_event_cb, LV_EVENT_ALL, NULL);
    lv_group_add_obj(indev_group, switch_btn);

	lv_obj_t* btn = my_list_add_btn(com_setting_list, Lang_GetStringByID(STRING_ID_WORKMODE_SYNC_UPDATA), 10);
    lv_obj_add_event_cb(btn, workmode_sync_btn_event, LV_EVENT_ALL, NULL);
    lv_group_add_obj(indev_group, btn);

    sync_hidden_setting_refresh();

    bool workmode_state = p_sys_menu_model->work_mode;
    uint8_t act_index = screen_get_act_index();
    lv_obj_t* focused_obj = NULL;

    if (!workmode_state) {
        act_index = 0;
    }

    focused_obj = lv_obj_get_child(com_setting_list, act_index);
    if (focused_obj)
    {
        lv_group_focus_obj(focused_obj);
    }
    else
    {
        lv_group_focus_obj(switch_btn);
    }

    lv_group_set_wrap(indev_group, false);
    lv_group_set_editing(indev_group, false);

}

static void workmode_sync_updata_seletc_init(void)
{
	lv_obj_t* title = lv_label_create(lv_scr_act());
	lv_obj_clear_flag(lv_scr_act(), LV_OBJ_FLAG_SCROLLABLE);
	
	lv_obj_set_style_text_font(title, Font_ResouceGet(FONT_22_BOLD), 0);
	lv_obj_set_style_text_color(title, lv_color_white(), 0);
	lv_label_set_text(title, Lang_GetStringByID(STRING_ID_WORKMODE_SYNC_UPDATA));
	lv_obj_align(title, LV_ALIGN_TOP_MID, 0, SYS_MENU_TITLE_TO_TOP);   

	s_work_radiobox_ext.sel = screen_get_act_index();            
	s_work_radiobox_ext.max = ui_get_work_update_number();
	s_work_radiobox_ext.items_of_page = 3;            
	s_work_radiobox_ext.get_name_str = work_update_name_get_str;
	s_work_radiobox_ext.release_cb = work_update_choose_list_user_event;

	lv_obj_t* radiobox_panel = my_long_list2_create(lv_scr_act(), 265, &s_work_radiobox_ext);            
	lv_obj_align(radiobox_panel, LV_ALIGN_TOP_MID, 0, 50);

	s_work_scroll_cont = my_list_scroll_create(lv_scr_act(), 3, 190);                       
	lv_obj_align_to(s_work_scroll_cont, radiobox_panel, LV_ALIGN_OUT_RIGHT_TOP, 21, 4);
	
	lv_group_add_obj(indev_group, radiobox_panel);
	lv_group_focus_obj(radiobox_panel);      
	lv_group_set_editing(indev_group, true); 
}

static void worknode_sync_updataing_init(void)
{
	lv_obj_t* cont = lv_obj_create(lv_scr_act());
	lv_obj_remove_style_all(cont);
	lv_obj_set_size(cont, lv_pct(100), lv_pct(22));
	lv_obj_set_style_text_color(cont, lv_color_make(255, 255, 255), 0);
	lv_obj_set_style_text_font(cont, Font_ResouceGet(FONT_18), 0);
	lv_obj_center(cont);
	
	lv_obj_t* title_label = lv_label_create(lv_scr_act());
	lv_obj_set_size(title_label, lv_pct(100), lv_pct(22));
	lv_obj_set_style_text_font(title_label, Font_ResouceGet(FONT_22_BOLD), 0);
	lv_obj_set_style_text_color(title_label, lv_color_white(), 0);
	lv_label_set_text(title_label, Lang_GetStringByID(STRING_ID_SYNC_UPDING_FIRMWARE));
	lv_obj_align(title_label, LV_ALIGN_TOP_MID, 0, 10);  
	
	lv_obj_t* ble_reset_bar = lv_bar_create(cont);
	lv_obj_set_style_bg_color(ble_reset_bar, lv_color_make(255, 255, 255), LV_PART_INDICATOR);
	lv_obj_set_style_bg_color(ble_reset_bar, lv_color_make(128, 128, 128), LV_PART_MAIN);
	lv_obj_set_size(ble_reset_bar, lv_pct(63), lv_pct(32));     
	lv_obj_align(ble_reset_bar, LV_ALIGN_TOP_MID, -5, 0);    
	lv_bar_set_range(ble_reset_bar, 0, 100);            

	lv_obj_t* process_label = lv_label_create(cont);
	lv_obj_t* status_label = lv_label_create(lv_scr_act());
	lv_label_set_long_mode(status_label, LV_LABEL_LONG_WRAP);
	lv_obj_set_style_text_align(status_label, LV_TEXT_ALIGN_CENTER, 0);
	lv_obj_set_size(status_label, 236, 80);
	lv_obj_set_style_text_font(status_label, Font_ResouceGet(FONT_14), 0);
	lv_label_set_text(status_label, Lang_GetStringByID(STRING_ID_SYNC_UPDATING));
	lv_obj_set_style_text_color(status_label, lv_color_white(), 0);
	lv_obj_align(status_label, LV_ALIGN_BOTTOM_MID, 0, -20);
	
	lv_obj_t* dot_label = lv_label_create(cont);      
	lv_label_set_text(dot_label, " ");

	ui_enter_critical();
	g_ptUIStateData->upd_status.percent = 0;
	ui_exit_critical();
	struct sys_info_usb usb_status_offer;

	data_center_read_sys_info(SYS_INFO_USB, &usb_status_offer);
	usb_status_offer.percent = 0;            
	data_center_write_sys_info(SYS_INFO_USB, &usb_status_offer); 
	refresh_upd_bar();    
	
	lv_group_add_obj(indev_group,  cont);
	lv_group_focus_obj(cont);
}

static void page_workmode_time_upd_cb(uint32_t ms)
{
    uint8_t spid = screen_get_act_spid();

    switch (spid)
    {
		case SUB_ID_SYNC_FOLLWO_MODE:
		{
			if(work_lang_last_type != g_tUIAllData.sys_menu_model.lang)
			{
				work_lang_last_type = g_tUIAllData.sys_menu_model.lang;
				lv_obj_t* title_label = lv_obj_get_child(lv_scr_act(), 0);
				lv_obj_t* dot_label = lv_obj_get_child(lv_scr_act(), 2);
				lv_label_set_text(title_label, Lang_GetStringByID(STRING_ID_FOLLOW_MODE_NAME));
				lv_label_set_text(dot_label, Lang_GetStringByID(STRING_ID_FOLLOW_MODE_TEXT));
				lv_obj_align_to(dot_label, workmode_img, LV_ALIGN_OUT_BOTTOM_MID, 0, 20);
			}
		}
		break;
		case SUB_ID_SYNC_UPDATA_SELECT:
		case SUB_ID_SYNC_UPDATE_CONFIRM:
			if(ui_upgrade_is_enable() == 0)
			{  
				ui_set_work_update_number_flag(0);
				screen_load_sub_page(SUB_ID_SYNC_NOT_FOUND_UPD_DEV, 0);
				break;          
			}
		break;
        case SUB_ID_SYNC_UPDATAING:    
        {
             if(ui_upgrade_get_status() == FIRMWARE_UPD_STATUS_OK)
             {
		 		ui_upgrade_clear_status();
                screen_load_sub_page(SUB_ID_SYNC_UPDATA_COMPLETE, 0);
             }
             else if(ui_upgrade_get_status() == FIRMWARE_UPD_STATUS_FAILED_NO_FW)
             {
		 		ui_upgrade_clear_status();
                screen_load_sub_page(SUB_ID_SYNC_NO_FIRMWARE, 0);
             }
             else if(ui_upgrade_get_status() == FIRMWARE_UPD_STATUS_FAILED)
             {
		 		ui_upgrade_clear_status();
                screen_load_sub_page(SUB_ID_SYNC_UPDATE_FAILED, 0);
             }
             else if(ui_upgrade_get_status() == FIRMWARE_UPD_STATUS_FAILED_FORMAT_ERR)
             {
		 		ui_upgrade_clear_status();
                screen_load_sub_page(SUB_ID_SYNC_U_DISK_FORMAT_ERR, 0);
             }
             else
             {
                refresh_upd_bar();        
             }
         }
         break;   
         case  SUB_ID_SYNC_UPDATA_COMPLETE:
         case  SUB_ID_SYNC_UPDATE_FAILED:
         case  SUB_ID_SYNC_NO_FIRMWARE:
         case  SUB_ID_SYNC_NOT_FOUND_UPD_DEV:
         case  SUB_ID_SYNC_U_DISK_FORMAT_ERR:
         {
		 	if(spid == SUB_ID_SYNC_NOT_FOUND_UPD_DEV)
		 	{
		 		if (ui_upgrade_is_enable())
		 		{  
					ui_set_work_update_number_flag(1);
		 			screen_load_sub_page(SUB_ID_SYNC_UPDATA_SELECT, 0);
		 			break;          
		 		}
		 	}
             //if page lifetime ended, then go back to the previous page.
             if (life_period != -1)
             {
                 disp_time += 20;
                 if (disp_time >= life_period)
                 {   
                     screen_load_prev_sub_page(SUB_ID_WORK_MODE_LIST, 0);
                 }
             }
         }
         break;
         default:break;
    }
}

static void workmode_sync_del_cb(my_msgbox_ext_t* ext)
{
    if (ext->answer == MSGBOX_ID_YES)
    {
        //执行进入复位页面
        //创建新的页面, 开启一个任务并且  
		extern void gui_data_sync_event_gen(void);		
        g_ptUIStateData->work_mode_sync = 1;
		gui_data_sync_event_gen();
    }
    else if (ext->answer == MSGBOX_ID_NO)    
    {
        //执行返回setting页面
        g_ptUIStateData->work_mode_sync = 0;
    }
//    screen_load_sub_page(SUB_ID_WORK_MODE_LIST, 0);
//    ctrl_sys_load_sub_page();
}

static void workmode_update_com_event_cb(lv_event_t* e)
{
    lv_event_code_t event = lv_event_get_code(e);

	shortcut_keys_event_handler(e);
	
    if (event == LV_EVENT_KEY)
    {
		ui_set_work_update_number_flag(0);
        uint32_t key = *(uint32_t*)lv_event_get_param(e);
        if (key == LV_KEY_BACKSPACE)
        {
            UI_PRINTF("Update_common event callback!  LV_KEY_BACKSPACE.\r\n");
			screen_load_page(PAGE_WORK_MODE, SUB_ID_WORK_MODE_LIST, 0);
        }
    }
}

static void refresh_upd_bar(void)
{
    lv_obj_t* cont = lv_obj_get_child(lv_scr_act(), 0);
	lv_obj_t* tile_label = lv_obj_get_child(lv_scr_act(), 1);
	lv_obj_t* bottom_label = lv_obj_get_child(lv_scr_act(), 2);
    lv_obj_t* ble_reset_bar = lv_obj_get_child(cont, 0);
    lv_obj_t* process_label = lv_obj_get_child(cont, 1);
	
    lv_bar_set_value(ble_reset_bar, ui_upgrade_get_percent(), LV_ANIM_ON);
    lv_label_set_text_fmt(process_label, "%d%%",  ui_upgrade_get_percent());
    lv_obj_align_to(process_label, ble_reset_bar, LV_ALIGN_OUT_RIGHT_MID, 5, 0);
	lv_obj_align_to(bottom_label, lv_scr_act(), LV_ALIGN_BOTTOM_MID, 10, -20);
	if(g_tUIAllData.sys_menu_model.lang == 0)
		lv_obj_align_to(tile_label, lv_scr_act(), LV_ALIGN_TOP_MID, 50, 20);
	else
		lv_obj_align_to(tile_label, lv_scr_act(), LV_ALIGN_TOP_MID, 100, 20);
}

static void work_mode_sync_init(void)
{
    LV_IMG_DECLARE(ImgWorkMode);
    indev_group = lv_group_create();
    my_indev_set_group(indev_group);
    lv_group_set_default(indev_group);

    lv_obj_set_style_bg_color(lv_scr_act(), lv_color_black(), 0);

    p_sys_menu_model = (sys_config_t*)screen_get_act_page_data();

    my_msgbox_reset();
    my_msgbox_ext_t* p_msgbox_ext = my_msgbox_get_ext();
    p_msgbox_ext->body_img = &ImgWorkMode;//&ImgWorkMode;ImgReset
    p_msgbox_ext->timecnt = 0;
    p_msgbox_ext->timeout = 30 * 1000;
    p_msgbox_ext->answer = MSGBOX_ID_NO_CONFIRM;
    p_msgbox_ext->type = MSGBOX_TYPE_TIMEOUT;
    p_msgbox_ext->btn_sel = MSGBOX_ID_NO;
    p_msgbox_ext->body_comment_str = Lang_GetStringByID(STRING_ID_WORKMODE_WARN);
    p_msgbox_ext->title_str = Lang_GetStringByID(STRING_ID_WORKMODE_SYNC);
    p_msgbox_ext->msgbox_del_cb = workmode_sync_del_cb;
    my_msgbox2_create(p_msgbox_ext);
}

static void work_follwo_mode_init(void)
{
    LV_IMG_DECLARE(imgBreak);
    indev_group = lv_group_create();
    my_indev_set_group(indev_group);
    lv_group_set_default(indev_group);

	work_lang_last_type = g_tUIAllData.sys_menu_model.lang;
    lv_obj_set_style_bg_color(lv_scr_act(), lv_color_black(), 0);

	lv_obj_t* title_label = lv_label_create(lv_scr_act());
    lv_obj_set_style_text_font(title_label, Font_ResouceGet(FONT_22_BOLD), 0);
    lv_obj_set_style_text_color(title_label, lv_color_white(), 0);
    lv_label_set_text(title_label, Lang_GetStringByID(STRING_ID_FOLLOW_MODE_NAME));
    lv_obj_align(title_label, LV_ALIGN_TOP_MID, 0, SYS_MENU_TITLE_TO_TOP);
	
    workmode_img = lv_img_create(lv_scr_act());
    lv_img_set_src(workmode_img, &ImgWorkMode);
	lv_obj_set_style_img_recolor(workmode_img, lv_color_make(0xff, 0xff, 0xff), 0);
	lv_obj_set_style_img_recolor_opa(workmode_img, LV_OPA_100, 0);
    lv_obj_align_to(workmode_img, title_label, LV_ALIGN_OUT_BOTTOM_MID, 0, 20);
	
	lv_obj_t* dot_label = lv_label_create(lv_scr_act());      
	lv_obj_set_style_text_font(dot_label, Font_ResouceGet(FONT_14), 0);
    lv_obj_set_style_text_color(dot_label, lv_color_white(), 0);
	lv_label_set_text(dot_label, Lang_GetStringByID(STRING_ID_FOLLOW_MODE_TEXT));
	lv_obj_align_to(dot_label, workmode_img, LV_ALIGN_OUT_BOTTOM_MID, 0, 20);
	
	lv_obj_t* yes_btn = lv_obj_create(lv_scr_act());
    lv_obj_clear_flag(yes_btn, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_style(yes_btn, &style_common_focued_btn, 0);
    lv_obj_set_style_radius(yes_btn, 15, 0);      
    lv_obj_set_size(yes_btn, 75, 40);
	lv_obj_align_to(yes_btn, dot_label, LV_ALIGN_OUT_BOTTOM_MID, 0, 20);
	lv_obj_add_event_cb(yes_btn, follow_mode_even_cb, LV_EVENT_ALL, NULL);
    lv_group_add_obj(indev_group, yes_btn);
	
	lv_obj_t *break_img = lv_img_create(yes_btn);
    lv_img_set_src(break_img, &imgBreak);
	lv_obj_set_style_img_recolor(break_img, lv_color_make(0xff, 0xff, 0xff), 0);
	lv_obj_set_style_img_recolor_opa(break_img, LV_OPA_100, 0);
	lv_obj_align_to(break_img, yes_btn, LV_ALIGN_CENTER, 0, 0);
	
	lv_group_set_wrap(indev_group, false);
    lv_group_set_editing(indev_group, false);


}

static void follow_mode_even_cb(lv_event_t* e)
{
	lv_event_code_t event = lv_event_get_code(e);
    lv_obj_t* btn = lv_event_get_target(e);
//    ui_title_t* p_title = &g_ptUIStateData->title;

//    shortcut_keys_event_handler(e);

    if (event == LV_EVENT_RELEASED)
    {
		screen_load_sub_page(SUB_ID_WORK_MODE_LIST, 0);
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

static const char* work_update_name_get_str(int8_t x)
{
	struct sys_info_update_name update_name_str;
			
	data_center_read_sys_info(SYS_INFO_WORK_UPDATE, &update_name_str);	
	
    lv_snprintf(sdb, sizeof(sdb), "%s", &update_name_str.name[x]);
	
    return sdb;
}

static void work_update_choose_list_user_event(lv_event_t* e, uint8_t index)
{
    lv_event_code_t event = lv_event_get_code(e);
    lv_obj_t* obj = lv_event_get_target(e);
    my_radiobox_ext_t* ext = lv_obj_get_user_data(obj);
    static uint8_t s_last_work_index = 0;
	
	shortcut_keys_event_handler(e);
	
    if (event == LV_EVENT_RELEASED)
    {
		ui_set_batch_update_serial_num(ext->sel);
		screen_load_sub_page(SUB_ID_SYNC_UPDATE_CONFIRM, 0);
    }
    else if (event == LV_EVENT_FOCUSED)
    {
        my_list_scroll_refresh(s_work_scroll_cont, lv_map(ext->sel, 0, ext->max, 0, 100));
    }
    else if (event == LV_EVENT_KEY)
    {
        uint32_t key = *(uint32_t *)lv_event_get_param(e);
		
		if(ext->sel != s_last_work_index)
		{
			for(uint8_t i = 0; i < ext->max; i++)
			{
				lv_obj_t *pre = lv_obj_get_child(obj, i);
				lv_obj_t *label = lv_obj_get_child(pre, 0);
				if(i == ext->sel)
					lv_label_set_long_mode(label, LV_LABEL_LONG_SCROLL);
				else
					lv_label_set_long_mode(label, LV_LABEL_LONG_CLIP);
			}
		}
		s_last_work_index = ext->sel;
		
        if ( LV_ENCODER_KEY_CC == key) //up
        {           
            my_list_scroll_refresh(s_work_scroll_cont, lv_map(ext->sel, 0, ext->max, 0, 100));
        }
        else if (LV_ENCODER_KEY_CW == key) //down
        {   
            my_list_scroll_refresh(s_work_scroll_cont, lv_map(ext->sel, 0, ext->max, 0, 100));
        }
        else if (LV_KEY_BACKSPACE == key)
        {
            screen_load_sub_page(SUB_ID_WORK_MODE_LIST, 0);
        }
    }
}

static void work_upgrade_confirm_init(void)
{
	my_msgbox_reset();
	my_msgbox_ext_t* p_msgbox_ext = my_msgbox_get_ext();
	LV_IMG_DECLARE(ImgWorkMode);
	p_msgbox_ext->body_comment_str = NULL;
	p_msgbox_ext->body_img = &ImgWorkMode;
	p_msgbox_ext->timecnt = 0;
	p_msgbox_ext->timeout = 30 * 1000;
	p_msgbox_ext->answer = MSGBOX_ID_NO_CONFIRM;
	p_msgbox_ext->type = MSGBOX_TYPE_TIMEOUT;
	p_msgbox_ext->btn_sel = MSGBOX_ID_NO;
	p_msgbox_ext->body_comment_str = NULL;
	p_msgbox_ext->title_str = Lang_GetStringByID(STRING_ID_SYNC_UPD_FIRMWARE);
	p_msgbox_ext->msgbox_del_cb = work_upd_config_event;
	my_msgbox_create(p_msgbox_ext);
}

static void work_upd_config_event(my_msgbox_ext_t* ext)
{
    if (ext->answer == MSGBOX_ID_YES)
    {        
        if (ui_upgrade_is_enable())
        {  
            ui_usb_start_batch_upgrade();
            screen_load_sub_page(SUB_ID_SYNC_UPDATAING, 0);          
        }
        else  
        {
            screen_load_sub_page(SUB_ID_NOT_FOUND_UPD_DEV, 0);         
        }
    }
    else if (ext->answer == MSGBOX_ID_NO)    
    {
        screen_load_prev_sub_page(0, 0);
    }
}
