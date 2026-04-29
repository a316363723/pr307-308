/*********************
 *      INCLUDES
 *********************/
#include "../ui_common.h"
#include "../page_widget.h"

/*********************
 *      DEFINES
 *********************/
#define PROGRESS_BAR_ANIM_TIME          (20 * 1000)
/*********************
 *      TPEDEFS
 *********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void page_battery_option_construct(void);
static void page_battery_option_destruct(void);
static void power_option_event_cb(lv_event_t* e, uint8_t index);
static void batt_activate_event_cb(lv_event_t* e, uint8_t index);
static void batt_tip_event_cb(lv_event_t* e, uint8_t index);
static void confirm_del_event(my_msgbox_ext_t* ext);
static void refresh_upd_bar(void);
static void page_battery_option_time_upd_cb(uint32_t ms);
static void batt_option_com_event_cb(lv_event_t* e);
static lv_obj_t* confirm_cont_add_btn(lv_obj_t* cont, lv_coord_t w, lv_coord_t h, const char* name);

static void batt_option_list_init(void);
static void batt_option_act_confirm(void);
static void batt_option_activating_init(void);

static void full_power_tip_init(void);
static void full_power_confirm(void);
/*********************
 *  STATIC VATIABLES
 *********************/
static lv_group_t *indev_group;
static my_list_ext_t list_ext;
static sys_config_t* p_sys_menu_model = NULL;
static int32_t disp_time;
static int32_t life_period;


/*********************
 *  GLOBAL VATIABLES
 *********************/
page_t page_battery_option = PAGE_INITIALIZER("Battery Option",
                                PAGE_BATTERY_OPTION, 
                                page_battery_option_construct, 
                                page_battery_option_destruct, 
                                page_battery_option_time_upd_cb,
                                NULL,                                
                                PAGE_MENU);
/*********************
 *  GLOBAL FUNCTIONS
 *********************/

/**********************
 *   STATIC FUNCTIONS
 **********************/    

/**
 * @brief 电池扩展选项的构造函数
 * 
 */
static void page_battery_option_construct(void)
{
    batt_option_t* p_batt_opt = &g_ptUIStateData->batt_option;
    (void)p_batt_opt;

    indev_group = lv_group_create();
    my_indev_set_group(indev_group);
    lv_group_set_default(indev_group);
    
    lv_obj_set_style_bg_color(lv_scr_act(), lv_color_black(), 0);

    p_sys_menu_model = screen_get_act_page_data();
    (void)p_sys_menu_model;

    uint8_t spid = screen_get_act_spid();
    life_period = -1;
    disp_time = 0;

    switch(spid)
    {
        case SUB_ID_BATT_OPTION_LIST:       batt_option_list_init();        break;
        case SUB_ID_ACTIVATE_BATT_CONFIRM:  batt_option_act_confirm();      break;
        case SUB_ID_ACTIVATING:             batt_option_activating_init();  break;        
        case SUB_ID_FULL_POWER_TIP:         full_power_tip_init();          break;
        case SUB_ID_FULL_POWER_CONFIRM:     full_power_confirm();           break;
        case SUB_ID_BATTERY_NOT_SUPPORT:
        case SUB_ID_ACTIVATE_SUCCESS:
        case SUB_ID_ACTIVATE_FAILED:
        {
            uint8_t spid = screen_get_act_spid();      
            lv_obj_t* panel = NULL;                        
            disp_time = 0;

            if (spid == SUB_ID_ACTIVATE_FAILED)
            {
                LV_IMG_DECLARE(ImgConfirmFailed)
                life_period = 3 * 1000;
                panel = warning_info_init(lv_scr_act(), &ImgConfirmFailed, 54, 25,Lang_GetStringByID(STRING_ID_ACTIVATE_FAILED));
            }
            else if (spid == SUB_ID_ACTIVATE_SUCCESS)
            {
                LV_IMG_DECLARE(ImgConfirmOk)                                
                life_period = 3 * 1000;
                lv_snprintf(sdb, sizeof(sdb), "%s",  Lang_GetStringByID(STRING_ID_ACTIVATE_SUCCESS));
                panel = warning_info_init(lv_scr_act(), &ImgConfirmOk, 54, 25, sdb);
            }
            else if (spid == SUB_ID_BATTERY_NOT_SUPPORT)
            {
                LV_IMG_DECLARE(ImgConfirmFailed)
                life_period = 3 * 1000;
                panel = warning_info_init(lv_scr_act(), &ImgConfirmFailed, 20, 7, Lang_GetStringByID(STRING_ID_BATTERY_NOT_SUPPORT));                
            }

            lv_obj_add_event_cb(panel, batt_option_com_event_cb, LV_EVENT_ALL, NULL);
            lv_group_add_obj(indev_group, panel);
            lv_group_focus_obj(panel);
            lv_group_set_editing(indev_group, true);
        }
        break;
    }
}

/**
 * @brief 电池扩展选项界面析构函数
 * 
 */
static void page_battery_option_destruct(void)
{
    lv_group_focus_freeze(indev_group, true);
    lv_group_remove_all_objs(indev_group);
    lv_group_del(indev_group);
    indev_group = NULL;

    lv_obj_clean(lv_scr_act());
}

/**
 * @brief 电池扩展选项界面 定时刷新回调函数
 * 
 * @param ms 
 */
static void page_battery_option_time_upd_cb(uint32_t ms)
{
    uint8_t spid = screen_get_act_spid();
    batt_option_t* p_batt_opt = &g_ptUIStateData->batt_option;

    switch (spid)
    {    
        case SUB_ID_ACTIVATING:    
       {
            if (p_batt_opt->state == BATT_ACTIVATE_SUCCESS)
            {
                screen_load_sub_page(SUB_ID_ACTIVATE_SUCCESS, 0);
            }                
            else if (p_batt_opt->state == BATT_ACTIVATE_FAILED)
            {
                screen_load_sub_page(SUB_ID_ACTIVATE_FAILED, 0);
            }

            if (p_batt_opt->percent < 100)
            {
                g_ptUIStateData->batt_option.percent += 1;
                if (g_ptUIStateData->batt_option.percent >= 99)
                {
                    g_ptUIStateData->batt_option.percent = 99;
                    p_batt_opt->state = BATT_ACTIVATE_FAILED;
                }
            }
        }    
        break;            
        case SUB_ID_ACTIVATE_SUCCESS:
        case SUB_ID_ACTIVATE_FAILED:    
        case SUB_ID_ACTIVATE_BATT_CONFIRM:
        case SUB_ID_FULL_POWER_TIP:
        case SUB_ID_BATTERY_NOT_SUPPORT:
        {
            //wait 10s for the second update...
            //if page lifetime ended, then go back to the previous page.
            if (life_period != -1 && !l2_screen_is_exist())
            {
                disp_time += 200;
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

/**
 * @brief 电池扩展选项列表
 * 
 */
static void batt_option_list_init(void)
{
    
    lv_obj_t* title_label = lv_label_create(lv_scr_act());
    lv_obj_set_style_text_font(title_label, Font_ResouceGet(FONT_22_BOLD), 0);
    lv_obj_set_style_text_color(title_label, lv_color_white(), 0);
    lv_label_set_text(title_label, Lang_GetStringByID(STRING_ID_BATTERY_OPTION));
    lv_obj_align(title_label, LV_ALIGN_TOP_MID, 0, SYS_MENU_TITLE_TO_TOP);

    list_ext.release_cb = power_option_event_cb;
    list_ext.cur_sel = screen_get_act_index();
    lv_obj_t* list = my_list_create(lv_scr_act(), lv_pct(75), lv_pct(70), &list_ext);  
    lv_obj_set_flex_align(list, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_align(list, LV_ALIGN_TOP_MID, 0, 53);                      
    
    my_list_add_btn(list, Lang_GetStringByID(STRING_ID_ACTIVATE_BATTERY), 30);
    my_list_add_btn(list, Lang_GetStringByID(STRING_ID_FULL_BATTERY_POWER), 30);
    
    lv_group_add_obj(indev_group, list);
    lv_group_focus_obj(list);
    lv_group_set_editing(indev_group, true); 
}

/**
 * @brief 电池扩展选项激活确认窗口
 * 
 */
static void batt_option_act_confirm(void)
{
    //life_period = 10 * 1000;

    lv_obj_t* screen = lv_obj_create(lv_scr_act());
    lv_obj_remove_style_all(screen);
    lv_obj_set_size(screen, lv_pct(100), lv_pct(100));
    lv_obj_center(screen);

    lv_obj_t* title_label = lv_label_create(screen);
    lv_obj_set_style_text_font(title_label, Font_ResouceGet(FONT_22_BOLD), 0);
    lv_obj_set_style_text_color(title_label, lv_color_white(), 0);
    lv_label_set_text(title_label, Lang_GetStringByID(STRING_ID_ACTIVATE_BATTERY));
    lv_obj_align(title_label, LV_ALIGN_TOP_MID, 0, SYS_MENU_TITLE_TO_TOP);

    list_ext.release_cb = batt_activate_event_cb;
    lv_obj_t* list = my_list_create(screen, 160, lv_pct(70), &list_ext);  
    lv_obj_set_flex_align(list, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_align(list, LV_ALIGN_CENTER, 0, 53);
    lv_obj_set_style_pad_gap(list, 16, 0);

    confirm_cont_add_btn(list, lv_pct(100), 42, Lang_GetStringByID(STRING_ID_ACTIVATE));
    confirm_cont_add_btn(list, lv_pct(100), 42, Lang_GetStringByID(STRING_ID_CANCEL));
    
    lv_group_add_obj(indev_group, list);
    lv_group_focus_obj(list);
    lv_group_set_editing(indev_group, true);   
}

/**
 * @brief 刷新进度条函数
 * 
 * @param a 
 * @param val 
 */
static void bar_anim_exec_xcb(void *a, int32_t val)
{
    lv_obj_t* cont = lv_obj_get_parent(a);
    lv_obj_t* process_label = lv_obj_get_child(cont, 1);
    
    lv_bar_set_value(a, val, LV_ANIM_OFF);
    lv_label_set_text_fmt(process_label, "%d%%", val);
    lv_obj_align(process_label, LV_ALIGN_TOP_RIGHT, 0, 0);    
}


/**
 * @brief 电池扩展激活中界面
 * 
 */
static void batt_option_activating_init(void)
{
    batt_option_t* p_batt_opt = &g_ptUIStateData->batt_option;   
 
    
    lv_obj_t* cont = lv_obj_create(lv_scr_act());
    lv_obj_remove_style_all(cont);
    lv_obj_set_size(cont, lv_pct(77), lv_pct(22));
    lv_obj_set_style_text_color(cont, lv_color_make(255, 255, 255), 0);
    lv_obj_set_style_text_font(cont, Font_ResouceGet(FONT_18), 0);            
    lv_obj_add_event_cb(cont, batt_option_com_event_cb, LV_EVENT_ALL, NULL);
    lv_obj_center(cont);

    lv_obj_t* batt_act_bar = lv_bar_create(cont);
    lv_obj_set_style_bg_color(batt_act_bar, lv_color_make(255, 255, 255), LV_PART_INDICATOR);
    lv_obj_set_style_bg_color(batt_act_bar, lv_color_make(128, 128, 128), LV_PART_MAIN);
    lv_obj_set_style_outline_pad(batt_act_bar, 0, 0);
    lv_obj_set_style_outline_width(batt_act_bar, 0, 0);    
    lv_obj_set_size(batt_act_bar, lv_pct(84), lv_pct(32));        
    lv_obj_align(batt_act_bar, LV_ALIGN_TOP_LEFT, 0, 0);    
    lv_bar_set_range(batt_act_bar, 0, 100);            

    lv_obj_t* process_label = lv_label_create(cont);
    lv_obj_t* status_label = lv_label_create(cont);
    lv_label_set_text(status_label,  Lang_GetStringByID(STRING_ID_ACTIVATING));            
    lv_obj_align(status_label, LV_ALIGN_BOTTOM_MID, 0, 0);         

    ui_enter_critical();
    p_batt_opt->percent = 0;            
    ui_exit_critical();
    
    refresh_upd_bar();    
    
    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, batt_act_bar);
    lv_anim_set_exec_cb(&a, bar_anim_exec_xcb);
    lv_anim_set_time(&a, PROGRESS_BAR_ANIM_TIME);
    lv_anim_set_repeat_count(&a, 1);
    lv_anim_set_values(&a, 0, 99);
    lv_anim_start(&a);

    lv_group_add_obj(indev_group, cont);
    lv_group_focus_obj(cont);    
    lv_group_set_editing(indev_group, true);
}

/**
 * @brief 满功率提示界面
 * 
 */
static void full_power_tip_init(void)
{
    //life_period = 10 * 1000;

    lv_obj_t* panel = lv_obj_create(lv_scr_act());
    lv_obj_remove_style_all(panel);
    lv_obj_set_size(panel, lv_pct(100), lv_pct(100));
    lv_obj_set_pos(panel, 0, 0);

    LV_IMG_DECLARE(ImgConfirmFailed)
    lv_obj_t* img = lv_img_create(panel);
    lv_img_set_src(img, &ImgConfirmFailed);
    lv_img_set_zoom(img, 167);    
    lv_obj_align(img, LV_ALIGN_TOP_MID, 0, 0);

    lv_obj_t* comment_body = lv_obj_create(panel);
    lv_obj_remove_style_all(comment_body);
    lv_obj_set_size(comment_body, lv_pct(94), 88);
    lv_obj_align_to(comment_body, img, LV_ALIGN_OUT_BOTTOM_MID, 0, 0);
        
    lv_obj_t* comment = lv_label_create(comment_body);
    lv_obj_set_width(comment, lv_pct(100));
    lv_obj_set_style_text_font(comment, Font_ResouceGet(FONT_12), 0);
    lv_obj_set_style_text_color(comment, lv_color_white(), 0);
    lv_obj_set_style_text_align(comment, LV_TEXT_ALIGN_LEFT, 0);
    
    lv_label_set_long_mode(comment, LV_LABEL_LONG_WRAP);
    lv_label_set_text(comment,  Lang_GetStringByID(STRING_ID_FULL_POWER_TIPS));
    lv_obj_align(comment, LV_ALIGN_CENTER, 0, 0);

    list_ext.release_cb = batt_tip_event_cb;
    lv_obj_t* list = my_list_create(panel, lv_pct(56), 70, &list_ext);  
    lv_obj_set_flex_align(list, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_align(list, LV_ALIGN_CENTER, 0, 53);    
    lv_obj_set_style_pad_gap(list, 5, 0);

    confirm_cont_add_btn(list, lv_pct(100), 30, Lang_GetStringByID(STRING_ID_I_UNDERSTAND));
    confirm_cont_add_btn(list, lv_pct(100), 30, Lang_GetStringByID(STRING_ID_GIVE_UP));
    
    lv_obj_align_to(list, comment_body, LV_ALIGN_OUT_BOTTOM_MID, 0, 0);

    lv_group_add_obj(indev_group, list);
    lv_group_focus_obj(list);
    lv_group_set_editing(indev_group, true);       
}

/**
 * @brief 满功率确认界面
 * 
 */
static void full_power_confirm(void)
{
    my_msgbox_reset();
    my_msgbox_ext_t* p_msgbox_ext = my_msgbox_get_ext();
    LV_IMG_DECLARE(ImgConfirmFailed);
    p_msgbox_ext->body_img = &ImgConfirmFailed;
    p_msgbox_ext->timecnt = 0;
    p_msgbox_ext->timeout = 30 * 1000;
    p_msgbox_ext->answer = MSGBOX_ID_NO_CONFIRM;
    p_msgbox_ext->type = MSGBOX_TYPE_TIMEOUT;
    p_msgbox_ext->btn_sel = MSGBOX_ID_NO;
    p_msgbox_ext->body_comment_str = Lang_GetStringByID(STRING_ID_CONFIRM);
    p_msgbox_ext->title_str = NULL;
    p_msgbox_ext->msgbox_del_cb = confirm_del_event;
    my_msgbox_create(p_msgbox_ext);
}

static lv_obj_t* confirm_cont_add_btn(lv_obj_t* cont, lv_coord_t w, lv_coord_t h, const char* name)
{        
    lv_obj_t* btn = lv_obj_create(cont);
    lv_obj_clear_flag(btn, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_style(btn, &style_common_btn, 0);
    lv_obj_add_style(btn, &style_common_focued_btn, LV_STATE_FOCUSED);
    lv_obj_set_style_radius(btn, 5, 0);
    lv_obj_set_style_radius(btn, 5, LV_STATE_FOCUSED);    
    lv_obj_set_style_text_font(btn, Font_ResouceGet(FONT_18), 0);
    lv_obj_set_style_text_font(btn, Font_ResouceGet(FONT_18), LV_STATE_FOCUSED);     
    lv_obj_set_size(btn, w, h);

    lv_obj_t* label =lv_label_create(btn);
    lv_obj_set_width(label, lv_pct(100));
    lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, 0);
    if (name != NULL) {
        lv_label_set_text(label, name);
    }        
    lv_obj_center(label);
     
    my_list_ext_t* ext = lv_obj_get_user_data(cont);
    if (ext != NULL)
    {
        ext->childs += 1;
    }
    
    return btn;    
}

/**
 * @brief 电池满功率提示
 * 
 * 
 * @param e 
 * @param index 
 */
static void batt_tip_event_cb(lv_event_t* e, uint8_t index)
{
    lv_event_code_t event = lv_event_get_code(e);

    if (event == LV_EVENT_RELEASED)
    {
        uint8_t spid = index + 1;

        switch (index)
        {
            case 0: spid = SUB_ID_FULL_POWER_CONFIRM; 
                screen_load_sub_page(spid, 0);   
                break;
            case 1: 
                screen_load_prev_sub_page(0, 0); 
                break;
        }              
    }
    else if (event == LV_EVENT_KEY)
    {
        uint32_t key = *(uint32_t *)lv_event_get_param(e);
        
        if (key == LV_KEY_BACKSPACE)
        {
            screen_load_prev_sub_page(0, 0); 
        }
    }   
}

/**
 * @brief 电池扩展选项列表事件回调函数
 * 
 * @param e 
 * @param index 
 */
static void power_option_event_cb(lv_event_t* e, uint8_t index)
{
    lv_event_code_t event = lv_event_get_code(e);

    if (event == LV_EVENT_RELEASED)
    {
        uint8_t spid = index + 1;

        switch (index)
        {
            case 0: spid = SUB_ID_ACTIVATE_BATT_CONFIRM; break;
            case 1: spid = SUB_ID_FULL_POWER_TIP; break;
        }

        screen_load_sub_page_with_stack(spid, index);                
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

/**
 * @brief 电池激活确认界面事件函数
 * 
 * @param e 
 * @param index 
 */
static void batt_activate_event_cb(lv_event_t* e, uint8_t index)
{
    lv_event_code_t event = lv_event_get_code(e);

    if (event == LV_EVENT_RELEASED)
    {
        uint8_t spid = index + 1;

        switch (index)
        {
            case 0: 
                spid = SUB_ID_ACTIVATING; 
                g_ptUIStateData->batt_option.state = BATT_ACTIVATE_START;
                ui_battery_start_activating();
                screen_load_sub_page(spid, 0);              
                break;
            case 1: 
                screen_load_prev_sub_page(0, 0); 
                break;
        }                
    }
    else if (event == LV_EVENT_KEY)
    {
        uint32_t key = *(uint32_t *)lv_event_get_param(e);
        
        if (key == LV_KEY_BACKSPACE)
        {
            screen_load_prev_sub_page(0, 0);
        }
    }        
}

static void confirm_del_event(my_msgbox_ext_t* ext)
{
    batt_option_t* p_batt_option = &g_ptUIStateData->batt_option;
    if (ext->answer == MSGBOX_ID_YES)
    {     
        if (p_batt_option->is_support_fullpower) 
        {
            p_batt_option->power_switch = 1;             
            screen_load_prev_sub_page(0, 0);        
        }
        else  
        {
            screen_load_sub_page(SUB_ID_BATTERY_NOT_SUPPORT, 0);
        }
    }
    else if (ext->answer == MSGBOX_ID_NO)    
    {        
        screen_load_prev_sub_page(0, 0);
    }
}

static void refresh_upd_bar(void)
{
    lv_obj_t* cont = lv_obj_get_child(lv_scr_act(), 0);
    lv_obj_t* batt_act_bar = lv_obj_get_child(cont, 0);
    lv_obj_t* process_label = lv_obj_get_child(cont, 1);

    
    lv_bar_set_value(batt_act_bar, g_ptUIStateData->batt_option.percent, LV_ANIM_ON);
    lv_label_set_text_fmt(process_label, "%d%%",  g_ptUIStateData->batt_option.percent);
    lv_obj_align(process_label, LV_ALIGN_TOP_RIGHT, 0, 0);
}

static void batt_option_com_event_cb(lv_event_t* e)
{
    lv_event_code_t event = lv_event_get_code(e);

    //shortcut_keys_event_handler(e);

     if (event == LV_EVENT_KEY)
    {
        uint32_t key = *(uint32_t*)lv_event_get_param(e);
        if (key == LV_KEY_BACKSPACE)
        {
            UI_PRINTF("Update_common event callback!  LV_KEY_BACKSPACE.\r\n");
            screen_load_prev_sub_page(0, 0);
        }
    }
}
