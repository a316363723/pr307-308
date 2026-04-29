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

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void page_language_construct(void);
static void page_language_destruct(void);
static void page_language_event(int event);
static void page_language_time_cb(uint32_t ms);

static const char* language_get_str(int8_t sel);
static void language_release_cb(lv_event_t *e, uint8_t index);

/*********************
 *  STATIC VATIABLES
 *********************/
static lv_group_t *indev_group;
static my_radiobox_ext_t radiobox_ext;
static sys_config_t* p_sys_menu_model = NULL;
/*********************
 *  GLOBAL VATIABLES
 *********************/
page_t page_language = PAGE_INITIALIZER("Language",
                                PAGE_LANGUAGE,
                                page_language_construct, 
                                page_language_destruct, 
                                page_language_time_cb,
                                page_language_event,                                
                                PAGE_MENU);
/*********************
 *  GLOBAL FUNCTIONS
 *********************/

/**********************
 *   STATIC FUNCTIONS
 **********************/                            
static void page_language_construct(void)
{
    indev_group = lv_group_create();
    page_event_send(EVENT_DATA_PULL);

    my_indev_set_group(indev_group);
    lv_group_set_default(indev_group);
    
    lv_obj_set_style_bg_color(lv_scr_act(), lv_color_black(), 0);

    p_sys_menu_model = (sys_config_t*)screen_get_act_page_data();
    
    lv_obj_t* title_label = lv_label_create(lv_scr_act());
    lv_obj_set_style_text_font(title_label, Font_ResouceGet(FONT_22_BOLD), 0);
    lv_obj_set_style_text_color(title_label, lv_color_white(), 0);
    lv_label_set_text(title_label, Lang_GetStringByID(STRING_ID_LANGUAGE));
    lv_obj_align(title_label, LV_ALIGN_TOP_MID, 0, SYS_MENU_TITLE_TO_TOP);

    radiobox_ext.sel = p_sys_menu_model->lang;
    radiobox_ext.max = LANGU_MAX;
    radiobox_ext.items_of_page = 2;
    radiobox_ext.get_name_str = language_get_str;
    radiobox_ext.release_cb = language_release_cb;    
    radiobox_ext.obj = my_radiobox_list_create(lv_scr_act(), lv_pct(75), &radiobox_ext, MY_RADIOBOX_TYPE2, LV_ALIGN_LEFT_MID);
    lv_obj_center(radiobox_ext.obj);
    
    lv_group_add_obj(indev_group, radiobox_ext.obj);
    lv_group_focus_obj(radiobox_ext.obj);      
    lv_group_set_editing(indev_group, true);
    lv_group_set_wrap(indev_group, false);
}

static void page_language_destruct(void)
{
    lv_group_del(indev_group);   
    lv_obj_clean(lv_scr_act());
    memset(&radiobox_ext, 0, sizeof(radiobox_ext));
    indev_group = NULL; 
}

static void page_language_time_cb(uint32_t ms)
{    
    //保存语言
    uint8_t lang = p_sys_menu_model->lang;    
    //从数据中心拉取语言数据
    data_center_read_config_data(SYS_CONFIG_LANG, &lang);
    //如果不一致，更新它.
    if ((lang != p_sys_menu_model->lang) && radiobox_ext.obj)
    {
        radiobox_ext.sel = p_sys_menu_model->lang;
        lv_event_send(radiobox_ext.obj, LV_EVENT_FOCUSED, NULL);
    }
}

static void page_language_event(int event)
{
    uint8_t lang;
    data_center_read_config_data(SYS_CONFIG_LANG, &lang);
    switch (event)
    {
        case EVENT_DATA_PULL:            
            p_sys_menu_model->lang = lang;
        break;
        case EVENT_DATA_WRITE:
            if (lang != p_sys_menu_model->lang)
            {
                data_center_write_config_data_no_event(SYS_CONFIG_LANG, &(p_sys_menu_model->lang));
            }            
        break;
    }
}

static void language_release_cb(lv_event_t *e, uint8_t index)
{
    lv_event_code_t event = lv_event_get_code(e);

    shortcut_keys_event_handler(e);

    if (event == LV_EVENT_RELEASED)
    {
        p_sys_menu_model->lang = index;
		if(ui_get_master_mode())
		{
			extern void gui_data_sync_event_gen(void);		
			gui_data_sync_event_gen();
		}
        data_center_write_config_data_no_event(SYS_CONFIG_LANG, &p_sys_menu_model->lang);
        screen_turn_prev_page();
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

static const char* language_get_str(int8_t sel)
{
    switch(sel)
    {
        case LANGU_EN:         lv_snprintf(sdb, sizeof(sdb), "%s", Lang_GetStringByID(STRING_ID_EN)); break;
        case LANGU_CHINESE:    lv_snprintf(sdb, sizeof(sdb), "%s", Lang_GetStringByID(STRING_ID_CHINESE_SIMPLE)); break;
    }
    return sdb;
}
