///*********************
// *      INCLUDES
// *********************/
//#include "../ui_common.h"
//#include "../page_widget.h"

///*********************
// *      DEFINES
// *********************/

///*********************
// *      TPEDEFS
// *********************/

///**********************
// *  STATIC PROTOTYPES
// **********************/
//static void page_movesetting_construct(void);
//static void page_movesetting_destruct(void);
//static void page_movesetting_event_cb(int event);
//static void page_movesetting_timer_cb(uint32_t ms);
//static const char* movesetting_get_str(int8_t sel);
//static void movesetting_release_cb(lv_event_t *e, uint8_t index);
//static void my_radiobox_event_cb(lv_event_t* e);
///*********************
// *  STATIC VATIABLES
// *********************/
//static lv_group_t *indev_group;
//static my_radiobox_ext_t radiobox_ext;
//static sys_config_t* p_sys_menu_model = NULL;
///*********************
// *  GLOBAL VATIABLES
// *********************/
//page_t page_movesetting = PAGE_INITIALIZER("Move Setting",
//                                PAGE_MOVE_SETTING, 
//                                page_movesetting_construct, 
//                                page_movesetting_destruct, 
//                                page_movesetting_timer_cb,
//                                page_movesetting_event_cb,                                
//                                PAGE_MENU);

///*********************
// *  GLOBAL FUNCTIONS
// *********************/

///**********************
// *   STATIC FUNCTIONS
// **********************/     

//static void page_movesetting_construct(void)
//{
//    indev_group = lv_group_create();
//    p_sys_menu_model = (sys_config_t*)screen_get_act_page_data();
//    page_event_send(EVENT_DATA_PULL);

//    my_indev_set_group(indev_group);
//    lv_group_set_default(indev_group);    
//    lv_obj_set_style_bg_color(lv_scr_act(), lv_color_black(), 0);
//    
//    lv_obj_t* title_label = lv_label_create(lv_scr_act());
//    lv_obj_set_style_text_font(title_label, Font_ResouceGet(FONT_22_BOLD), 0);
//    lv_obj_set_style_text_color(title_label, lv_color_white(), 0);
//    lv_label_set_text(title_label, Lang_GetStringByID(STRING_ID_MOVE_SETTING));
//    lv_obj_align(title_label, LV_ALIGN_TOP_MID, 0, SYS_MENU_TITLE_TO_TOP);

//    uint8_t spid = screen_get_act_spid();
//    switch(spid)
//    {
//        
//        case MOVE_MODE_FRESNEL:
//            
//            radiobox_ext.sel = p_sys_menu_model->ms_mode;
//            radiobox_ext.max = OUTPUT_MODE_TYPE_MAX;
//            radiobox_ext.items_of_page = 2;
//            radiobox_ext.get_name_str = movesetting_get_str;
//            radiobox_ext.release_cb = movesetting_release_cb;    
//            radiobox_ext.obj = my_radiobox_list_create(lv_scr_act(), lv_pct(68), &radiobox_ext, MY_RADIOBOX_TYPE2, LV_ALIGN_CENTER);
//            lv_obj_center(radiobox_ext.obj);
//        break;
//        case MOVE_MODE_STENTS:
//            
//        
//        
//            break;
//    
//    
//    
//    }

//    
//    lv_group_add_obj(indev_group, radiobox_ext.obj);
//    lv_group_set_wrap(indev_group, false);
//    lv_group_set_editing(indev_group, true);    
//}

//static void page_movesetting_destruct(void)
//{    
//    lv_group_del(indev_group);    
//    lv_obj_clean(lv_scr_act());
//    indev_group = NULL; 

//    page_event_send(EVENT_DATA_WRITE);
//}

//static void page_movesetting_event_cb(int event)
//{
//    uint8_t msmode;
//    data_center_read_config_data(SYS_CONFIG_OUTPUT_MODE, &msmode);
//    switch (event)
//    {
//        case EVENT_DATA_PULL:
//            p_sys_menu_model->ms_mode = msmode;
//        break;
//        case EVENT_DATA_WRITE:
//            if (msmode != p_sys_menu_model->ms_mode)
//            {
//                data_center_read_config_data(SYS_CONFIG_OUTPUT_MODE, &p_sys_menu_model->ms_mode);
//            }
//        break;
//    }
//}

//static void page_movesetting_timer_cb(uint32_t ms)
//{
//    page_event_send(EVENT_DATA_WRITE);
//    if (radiobox_ext.obj && (radiobox_ext.sel != p_sys_menu_model->ms_mode))
//    {
//        radiobox_ext.sel = p_sys_menu_model->ms_mode;
//        lv_event_send(radiobox_ext.obj, LV_EVENT_FOCUSED, NULL);
//    }
//}

//static void movesetting_release_cb(lv_event_t *e, uint8_t index)
//{
//    lv_event_code_t event = lv_event_get_code(e);

//    shortcut_keys_event_handler(e);

//    if (event == LV_EVENT_RELEASED)
//    {
//        p_sys_menu_model->ms_mode = index;
//        screen_turn_prev_page();
//    }    
//    else if (event == LV_EVENT_KEY)
//    {
//        uint32_t key = *(uint32_t *)lv_event_get_param(e);
//        
//        if (key == LV_KEY_BACKSPACE)
//        {
//            screen_turn_prev_page();
//        }
//    }        
//}   

//static const char* movesetting_get_str(int8_t sel)
//{
//    switch(sel)
//    {
//        case MOVE_MODE_FRESNEL:  lv_snprintf(sdb, sizeof(sdb), "%s", Lang_GetStringByID(STRING_ID_FRESNEL)); break;
//        case MOVE_MODE_STENTS:    lv_snprintf(sdb, sizeof(sdb), "%s", Lang_GetStringByID(STRING_ID_STENTS)); break;
//    }
//    return sdb;
//}
