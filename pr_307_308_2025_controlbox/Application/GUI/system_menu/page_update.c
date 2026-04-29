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
typedef struct {
    lv_obj_t* scroll_cont;
	uint8_t last_sub_pid;
}ctr_view_t;
/**********************
 *  STATIC PROTOTYPES
 **********************/
static void page_update_construct(void);
static void page_update_destruct(void);
static void update_list_event_cb(lv_event_t* e, uint8_t index);
static void info_btn_cont_event_cb(lv_event_t* e, uint8_t index);
static void upd_del_event(my_msgbox_ext_t* ext);
static void refresh_upd_bar(void);
static void page_update_time_upd_cb(uint32_t ms);
static void update_com_event_cb(lv_event_t* e);
static void dot_label_anim_exec_xcb(void *a, int32_t value);
/*********************
 *  STATIC VATIABLES
 *********************/
static lv_group_t *indev_group;
static my_list_ext_t list_ext;
static my_list_ext_t list_ext1;
static sys_config_t* p_sys_menu_model = NULL;
static int32_t disp_time;
static int32_t life_period;
static ctr_view_t s_updata_view;
/*********************
 *  GLOBAL VATIABLES
 *********************/
page_t page_update = PAGE_INITIALIZER("Update",
                                PAGE_UPDATE, 
                                page_update_construct, 
                                page_update_destruct, 
                                page_update_time_upd_cb,
                                NULL,                                
                                PAGE_MENU);
/*********************
 *  GLOBAL FUNCTIONS
 *********************/

/**
 * @brief 字符串转数字
 * 
 * @param str 
 * @return uint32_t 
 */
uint32_t char_to_digit(const char* str, char end_char)
{
    uint32_t sum = 0;
    
    while (*str != '\0')
    {
        if (*str == end_char)    break;        
        sum = sum * 10 + *str++ - '0';        
    }

    return sum;
}

/**
 * @brief 获取系统时间
 * 
 * @param p_time 
 */
void get_sys_time(ui_time_t* p_time)
{
   const char *month_array[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
   const char date[12] = __DATE__;
   const char time[12] = __TIME__;
    /* get the month */
    uint8_t month = 1;
    for (uint8_t index = 0; index < 12; index++) {
        if (memcmp(date, month_array[index], 3) == 0)
        {
            month = index + 1;
            break;
        }
    }
    /* Convert the string to the digit. */
    p_time->month = month;

    if (date[4] == ' ')
        p_time->day = char_to_digit(&date[5], ' ');
    else 
        p_time->day = char_to_digit(&date[4], ' ');
    
    p_time->year = char_to_digit(&date[7], ' ');
    p_time->hour = char_to_digit(&time[0], ':');
    p_time->minute = char_to_digit(&time[3], ':');    
    p_time->second = char_to_digit(&time[6], ':');    
}

/**********************
 *   STATIC FUNCTIONS
 **********************/    
static void page_update_construct(void)
{
    indev_group = lv_group_create();
    my_indev_set_group(indev_group);
    lv_group_set_default(indev_group);
    
    lv_obj_set_style_bg_color(lv_scr_act(), lv_color_black(), 0);

    p_sys_menu_model = screen_get_act_page_data();
    (void)p_sys_menu_model;

    uint8_t spid = screen_get_act_spid();
    life_period = -1;
    switch(spid)
    {
        case SUB_ID_UPDATE_SETTING:
        {
            lv_obj_t* title_label = lv_label_create(lv_scr_act());
            lv_obj_set_style_text_font(title_label, Font_ResouceGet(FONT_22_BOLD), 0);
            lv_obj_set_style_text_color(title_label, lv_color_white(), 0);
            lv_label_set_text(title_label, Lang_GetStringByID(STRING_ID_UPDATE));
            lv_obj_align(title_label, LV_ALIGN_TOP_MID, 0, SYS_MENU_TITLE_TO_TOP);

            list_ext.release_cb = update_list_event_cb;            
            lv_obj_t* list = my_list_create(lv_scr_act(), lv_pct(85), lv_pct(70), &list_ext);  
            lv_obj_set_flex_align(list, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
            lv_obj_align(list, LV_ALIGN_TOP_MID, 0, 53);                      
            
            my_list_add_btn(list, Lang_GetStringByID(STRING_ID_UPD_FIRMWARE), 10);
            my_list_add_btn(list, Lang_GetStringByID(STRING_ID_FIRMWARE_VERSION), 10);
            
            lv_group_add_obj(indev_group, list);
            lv_group_focus_obj(list);
            lv_group_set_editing(indev_group, true);  

            ui_enter_critical();
            g_ptUIStateData->upd_status.status = FIRMWARE_UPD_STATUS_IDLE;       
            ui_exit_critical();
        }
        break;
        case SUB_ID_FIRMWARE_VERSION:
        {
			struct sys_info_lamp   info_read_body = {0};
            ui_time_t* p_time = &g_ptUIStateData->upd_time;
			
			lv_obj_set_scrollbar_mode(lv_scr_act(), LV_SCROLLBAR_MODE_OFF);
            lv_obj_t* title = lv_label_create(lv_scr_act());
            
            lv_obj_set_style_text_font(title, Font_ResouceGet(FONT_22_BOLD), 0);
            lv_obj_set_style_text_color(title, lv_color_white(), 0);
            lv_label_set_text(title, Lang_GetStringByID(STRING_ID_FIRMWARE_VERSION));
            lv_obj_align(title, LV_ALIGN_TOP_MID, 0, SYS_MENU_TITLE_TO_TOP); 
			
			list_ext1.cur_sel = 0;  
            list_ext1.release_cb = info_btn_cont_event_cb;
			lv_obj_t* list = my_list_create(lv_scr_act(), 265, lv_pct(80), &list_ext1);
			list_ext1.childs = 5; 
			lv_obj_add_flag(list, LV_OBJ_FLAG_SCROLLABLE);
			lv_obj_set_scrollbar_mode(list, LV_SCROLLBAR_MODE_OFF);
            lv_obj_set_flex_align(list, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);                       
            lv_obj_align(list, LV_ALIGN_TOP_MID, 0, 38);
			
            kv_string_t kv1;            
            lv_snprintf(sdb, sizeof(sdb), "%d.%d", ui_get_ctrlbox_soft_version() / 10, ui_get_ctrlbox_soft_version() % 10);
            kv1.name = Lang_GetStringByID(STRING_ID_CONTROL_BOX);
            kv1.value = sdb;                        
            info_btn_create(list, &kv1);

			data_center_read_sys_info(SYS_INFO_LAMP, &info_read_body);
			if(info_read_body.exist == 1)
				lv_snprintf(sdb, sizeof(sdb), "%d.%d", ui_get_lamp_soft_version() / 10, ui_get_lamp_soft_version() % 10);    
			else
				lv_snprintf(sdb, sizeof(sdb), "%d.%d", 0, 0);    
            kv1.name = Lang_GetStringByID(STRING_ID_LAMP_HEAD);            
            kv1.value = sdb;
            info_btn_create(list, &kv1);            
			
			if(info_read_body.exist == 1)
				lv_snprintf(sdb, sizeof(sdb), "%d.%d", ui_get_motorized_yoke_soft_version() / 10, ui_get_motorized_yoke_soft_version() % 10);      
			else
				lv_snprintf(sdb, sizeof(sdb), "%d.%d", 0, 0);  
            kv1.name = Lang_GetStringByID(STRING_ID_MOTORIZED_YOKE);            
            kv1.value = sdb;
            info_btn_create(list, &kv1);
			
			if(info_read_body.exist == 1)
				lv_snprintf(sdb, sizeof(sdb), "%d.%d", ui_get_motorized_frenel_soft_version() / 10, ui_get_motorized_frenel_soft_version() % 10);       
			else
				lv_snprintf(sdb, sizeof(sdb), "%d.%d", 0, 0);  
            kv1.name = Lang_GetStringByID(STRING_ID_MOTORIZED_FRESNEL);            
            kv1.value = sdb;
            lv_obj_t *btn = info_btn_create(list, &kv1);
            
            lv_obj_t* upd_info_label = lv_label_create(list);            
            lv_obj_set_style_text_font(upd_info_label, Font_ResouceGet(FONT_14), 0);
            lv_obj_set_style_text_color(upd_info_label, lv_color_white(), 0);
            lv_obj_set_width(upd_info_label, 265);
            lv_label_set_long_mode(upd_info_label, LV_LABEL_LONG_WRAP);        

            get_sys_time(p_time);
            lv_snprintf(sdb, sizeof(sdb), "%s%d.%d.%d%s %d:%02d", Lang_GetStringByID(STRING_ID_LAST_UPD_TIME), p_time->year, p_time->month, p_time->day, 
                                                                                                Lang_GetStringByID(STRING_ID_AT), p_time->hour, p_time->minute);                                                                             
            lv_label_set_text(upd_info_label, sdb);
			lv_obj_clear_flag(upd_info_label, LV_OBJ_FLAG_SCROLLABLE);
			lv_obj_add_flag(upd_info_label, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
            lv_obj_align(upd_info_label, LV_ALIGN_OUT_BOTTOM_MID, 0, -39);
			
			s_updata_view.scroll_cont = my_list_scroll_create(lv_scr_act(), 3, 215);
			lv_obj_align_to(s_updata_view.scroll_cont, list, LV_ALIGN_OUT_RIGHT_TOP, 21, 2); 
			
            lv_group_add_obj(indev_group, list);
            lv_group_focus_obj(list);      
            lv_group_set_editing(indev_group, true);     
        }
        break;
        case SUB_ID_UPDATE_CONFIRM:
        {
            my_msgbox_reset();
            my_msgbox_ext_t* p_msgbox_ext = my_msgbox_get_ext();
            LV_IMG_DECLARE(ImgUpdateArrow);
            p_msgbox_ext->body_comment_str = NULL;
            p_msgbox_ext->body_img = &ImgUpdateArrow;
            p_msgbox_ext->timecnt = 0;
            p_msgbox_ext->timeout = 30 * 1000;
            p_msgbox_ext->answer = MSGBOX_ID_NO_CONFIRM;
            p_msgbox_ext->type = MSGBOX_TYPE_TIMEOUT;
            p_msgbox_ext->btn_sel = MSGBOX_ID_NO;
            p_msgbox_ext->body_comment_str = NULL;
            p_msgbox_ext->title_str = Lang_GetStringByID(STRING_ID_UPD_FIRMWARE);
            p_msgbox_ext->msgbox_del_cb = upd_del_event;
            my_msgbox_create(p_msgbox_ext);
        }
        break;
        case SUB_ID_UPDATING:
        {
            lv_obj_t* cont = lv_obj_create(lv_scr_act());
            lv_obj_remove_style_all(cont);
            lv_obj_set_size(cont, lv_pct(100), lv_pct(22));
            lv_obj_set_style_text_color(cont, lv_color_make(255, 255, 255), 0);
            lv_obj_set_style_text_font(cont, Font_ResouceGet(FONT_18), 0);
            //lv_obj_add_event_cb(cont, update_com_event_cb, LV_EVENT_ALL, NULL);
            lv_obj_center(cont);

            lv_obj_t* ble_reset_bar = lv_bar_create(cont);
            lv_obj_set_style_bg_color(ble_reset_bar, lv_color_make(255, 255, 255), LV_PART_INDICATOR);
            lv_obj_set_style_bg_color(ble_reset_bar, lv_color_make(128, 128, 128), LV_PART_MAIN);
            lv_obj_set_size(ble_reset_bar, lv_pct(63), lv_pct(32));                
            //lv_obj_align(ble_reset_bar, LV_ALIGN_TOP_LEFT, 0, 0);    
            lv_obj_align(ble_reset_bar, LV_ALIGN_TOP_MID, -5, 0);    
            lv_bar_set_range(ble_reset_bar, 0, 100);            

            lv_obj_t* process_label = lv_label_create(cont);
            lv_obj_t* status_label = lv_label_create(cont);
            lv_label_set_text(status_label, Lang_GetStringByID(STRING_ID_UPDATING));
            lv_obj_align(status_label, LV_ALIGN_BOTTOM_MID, 0, 0);   
            
            lv_obj_t* dot_label = lv_label_create(cont);      
            lv_label_set_text(dot_label, " ");

            lv_anim_t a;
            lv_anim_init(&a);
            lv_anim_set_var(&a, dot_label);
            lv_anim_set_values(&a, 0, 3);
            lv_anim_set_time(&a, 3000);
            lv_anim_set_repeat_count(&a, LV_ANIM_REPEAT_INFINITE);            
            lv_anim_set_exec_cb(&a, dot_label_anim_exec_xcb);
            lv_anim_start(&a);

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
            //lv_group_set_editing(indev_group, true);            
        }
        break;
        case SUB_ID_NO_FIRMWARE:
        case SUB_ID_UPDATE_FAILED:
        case SUB_ID_UPDATE_OK:
        case SUB_ID_NOT_FOUND_UPD_DEV:
        case SUB_ID_U_DISK_FORMAT_ERR:
        {
            uint8_t spid = screen_get_act_spid();      
            lv_obj_t* panel = NULL;                        
            disp_time = 0;

            if (spid == SUB_ID_UPDATE_FAILED)
            {
                LV_IMG_DECLARE(ImgConfirmFailed)
                life_period = 2 * 500;
                panel = warning_info_init(lv_scr_act(), &ImgConfirmFailed, 54, 25, Lang_GetStringByID(STRING_ID_UPD_FAILED));
                lv_obj_add_event_cb(panel, update_com_event_cb, LV_EVENT_ALL, NULL);
            }
            else if (spid == SUB_ID_UPDATE_OK)
            {
                LV_IMG_DECLARE(ImgConfirmOk)                
                const char* fw_name = ui_upgrade_get_type() == FIRMWARE_TYPE_CTRL_BOX ? Lang_GetStringByID(STRING_ID_CONTROL_BOX) : Lang_GetStringByID(STRING_ID_LAMP_HEAD);
                life_period = 2 * 500;
//                lv_snprintf(sdb, sizeof(sdb), "%s %s", fw_name, Lang_GetStringByID(STRING_ID_UPDATE_OK));
				lv_snprintf(sdb, sizeof(sdb), "%s", Lang_GetStringByID(STRING_ID_UPDATE_OK));
                panel = warning_info_init(lv_scr_act(), &ImgConfirmOk, 54, 25, sdb);
                lv_obj_add_event_cb(panel, update_com_event_cb, LV_EVENT_ALL, NULL);
            }
            else if (spid == SUB_ID_NOT_FOUND_UPD_DEV)
            {
                LV_IMG_DECLARE(ImgConfirmFailed)
                life_period = 4 * 500;
                panel = warning_info_init(lv_scr_act(), &ImgConfirmFailed, 54, 25, Lang_GetStringByID(STRING_ID_NO_UPD_DEV));                                               
                lv_obj_add_event_cb(panel, update_com_event_cb, LV_EVENT_ALL, NULL);
            }
            else if (spid == SUB_ID_NO_FIRMWARE)
            {
                LV_IMG_DECLARE(ImgConfirmFailed)
                life_period = 2 * 500;
                panel = warning_info_init(lv_scr_act(), &ImgConfirmFailed, 54, 25, Lang_GetStringByID(STRING_ID_NO_FIRMWARE));                
                lv_obj_add_event_cb(panel, update_com_event_cb, LV_EVENT_ALL, NULL);
            }
            else if (spid == SUB_ID_U_DISK_FORMAT_ERR)
            {
                LV_IMG_DECLARE(ImgConfirmFailed)
                life_period = 2 * 500;
                panel = warning_info_init(lv_scr_act(), &ImgConfirmFailed, 54, 25, Lang_GetStringByID(STRING_ID_FORMAT_ERR));                
                lv_obj_add_event_cb(panel, update_com_event_cb, LV_EVENT_ALL, NULL);
            }

            lv_group_add_obj(indev_group, panel);
            lv_group_focus_obj(panel);
        }
        break;
    }
}

static void page_update_destruct(void)
{
    lv_group_focus_freeze(indev_group, true);
    lv_group_remove_all_objs(indev_group);
    lv_group_del(indev_group);
    indev_group = NULL;

    lv_obj_clean(lv_scr_act());
}

static void page_update_time_upd_cb(uint32_t ms)
{
    uint8_t spid = screen_get_act_spid();

    switch (spid)
    {
       case SUB_ID_UPDATING:    
       {
#if UI_DUMMY_DATA_TEST             
            if (g_ptUIStateData->upd_status.percent == 99)
            {
                UI_PRINTF("ID_UPDATE %d\r\n", 99);
                if (g_ptUIStateData->upd_status.status == FIRMWARE_UPD_STATUS_OK)
                {
                    screen_load_sub_page(SUB_ID_UPDATE_OK, 0);
                }                
                else if (g_ptUIStateData->upd_status.status == FIRMWARE_UPD_STATUS_FAILED_NO_FW)
                {
                   screen_load_sub_page(SUB_ID_NO_FIRMWARE, 0);
                }
                else if (g_ptUIStateData->upd_status.status == FIRMWARE_UPD_STATUS_FAILED)
                {
                   screen_load_sub_page(SUB_ID_UPDATE_FAILED, 0);
                }
            }        
            else if (g_ptUIStateData->upd_status.percent  < 100)
            {
                g_ptUIStateData->upd_status.percent += 1;             
                if (g_ptUIStateData->upd_status.percent  >= 99)
                {
                    g_ptUIStateData->upd_status.percent  = 99;
                }
                refresh_upd_bar();
            }
#else 
            if (ui_upgrade_get_status() == FIRMWARE_UPD_STATUS_OK)
            {
				ui_upgrade_clear_status();
                screen_load_sub_page(SUB_ID_UPDATE_OK, 0);
            }                
            else if (ui_upgrade_get_status() == FIRMWARE_UPD_STATUS_FAILED_NO_FW)
            {
				ui_upgrade_clear_status();
                screen_load_sub_page(SUB_ID_NO_FIRMWARE, 0);
            }
            else if (ui_upgrade_get_status() == FIRMWARE_UPD_STATUS_FAILED)
            {
				ui_upgrade_clear_status();
                screen_load_sub_page(SUB_ID_UPDATE_FAILED, 0);
            }
            else if (ui_upgrade_get_status() == FIRMWARE_UPD_STATUS_FAILED_FORMAT_ERR)
            {
				ui_upgrade_clear_status();
                screen_load_sub_page(SUB_ID_U_DISK_FORMAT_ERR, 0);
            }
            else
            {
                refresh_upd_bar();   
				if ((ui_upgrade_is_enable() == 0) && (1 == ui_updata_start_flag_get()))
				{  
					ui_updata_start_flag_set(0);
					screen_load_sub_page(SUB_ID_UPDATE_CONFIRM, 0);
					break;          
				}
            }        
#endif
        }    
        break;   
        case  SUB_ID_UPDATE_OK:
        case  SUB_ID_UPDATE_FAILED:
        case  SUB_ID_NO_FIRMWARE:
        case  SUB_ID_NOT_FOUND_UPD_DEV:
        case  SUB_ID_U_DISK_FORMAT_ERR:
        {
            //wait 10s for the second update...
            if (spid == SUB_ID_UPDATE_OK) {
                if (ui_upgrade_get_status() < FIRMWARE_UPD_STATUS_OK && \
                        ui_upgrade_get_status() > FIRMWARE_UPD_STATUS_IDLE) {
                    screen_load_sub_page(SUB_ID_UPDATING, 0);
                    break;
                }
            }
			if(spid == SUB_ID_NOT_FOUND_UPD_DEV)
			{
				if (ui_upgrade_is_enable())
				{  
					ui_usb_start_upgrade();
					screen_load_sub_page(SUB_ID_UPDATING, 0);
					break;          
				}
			}
            //if page lifetime ended, then go back to the previous page.
            if (life_period != -1)
            {
                disp_time += 20;
                if (disp_time >= life_period)
                {   
                    screen_load_prev_sub_page(0, 0);
                }
            }
        }
        break;
        default:break;
    }
}

static void update_list_event_cb(lv_event_t* e, uint8_t index)
{
    lv_event_code_t event = lv_event_get_code(e);

    if (event == LV_EVENT_RELEASED)
    {
        uint8_t spid = index + 1;
        if (index == 0)
        {
            spid = SUB_ID_UPDATE_CONFIRM;
			screen_load_sub_page_with_stack(spid, index);  
        }
		else
		{
			spid = SUB_ID_FIRMWARE_VERSION;
			struct sys_info_lamp   info_read_body = {0};
			
			data_center_read_sys_info(SYS_INFO_LAMP, &info_read_body);	
			if(info_read_body.exist == 1)
				ui_get_version();
			else
			{
				screen_load_sub_page_with_stack(spid, index);
			}
		}
                      
    }
    else if (event == LV_EVENT_KEY)
    {
        uint32_t key = *(uint32_t *)lv_event_get_param(e);
        
        if (key == LV_KEY_BACKSPACE)
        {
			uint8_t index1 = page_get_index(PAGE_MENU);//获取上一次的字ID
			screen_load_page_and_index(PAGE_MENU, 0, index1, true);
//            screen_turn_prev_page();
        }
    }        
}

static void info_btn_cont_event_cb(lv_event_t* e, uint8_t index)
{
    lv_event_code_t event = lv_event_get_code(e);

    shortcut_keys_event_handler(e);

    if (event == LV_EVENT_KEY)  
    {
        uint32_t key = *(uint32_t *)lv_event_get_param(e);

		if ( LV_ENCODER_KEY_CC == key) //up
		{           
			my_list_scroll_refresh(s_updata_view.scroll_cont, lv_map(index, 0, 5, 0, 100));
		}
		else if (LV_ENCODER_KEY_CW == key) //down
		{   
			my_list_scroll_refresh(s_updata_view.scroll_cont, lv_map(index, 0, 5, 0, 100));
		}
        else if (key == LV_KEY_BACKSPACE)
        {
            screen_load_prev_sub_page(0, 0);
        }
    }
    else if (event == LV_EVENT_RELEASED)
    {
        screen_load_prev_sub_page(0, 0);
    }
}

static void upd_del_event(my_msgbox_ext_t* ext)
{
    if (ext->answer == MSGBOX_ID_YES)
    {        
        if (ui_upgrade_is_enable())
        {  
            ui_usb_start_upgrade();
            screen_load_sub_page(SUB_ID_UPDATING, 0);          
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

static void dot_label_anim_exec_xcb(void *dot_label, int32_t value)
{   
    lv_obj_t* cont = lv_obj_get_parent(dot_label);
    lv_obj_t* status_label = lv_obj_get_child(cont, -2);
    const char *tail[] = {".", ". .", ". . ."};    

    lv_label_set_text(dot_label, tail[value % 3]);
    lv_obj_align_to(dot_label, status_label, LV_ALIGN_OUT_RIGHT_MID, 0, 0);
}

static void refresh_upd_bar(void)
{
    lv_obj_t* cont = lv_obj_get_child(lv_scr_act(), 0);
    lv_obj_t* ble_reset_bar = lv_obj_get_child(cont, 0);
    lv_obj_t* process_label = lv_obj_get_child(cont, 1);

    lv_bar_set_value(ble_reset_bar, ui_upgrade_get_percent(), LV_ANIM_ON);
    lv_label_set_text_fmt(process_label, "%d%%",  ui_upgrade_get_percent());
    lv_obj_align_to(process_label, ble_reset_bar, LV_ALIGN_OUT_RIGHT_MID, 5, 0);
}

static void update_com_event_cb(lv_event_t* e)
{
    lv_event_code_t event = lv_event_get_code(e);

     if (event == LV_EVENT_KEY)
    {
        uint32_t key = *(uint32_t*)lv_event_get_param(e);
        if (key == LV_KEY_BACKSPACE)
        {
            UI_PRINTF("Update_common event callback!  LV_KEY_BACKSPACE.\r\n");
            screen_load_sub_page_with_stack(0, 0);
        }
    }
}
