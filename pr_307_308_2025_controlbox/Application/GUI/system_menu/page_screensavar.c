/*********************
 *      INCLUDES
 *********************/
#include "../ui_common.h"
#include "../page_widget.h"

/*********************
 *      DEFINES
 *********************/
#define SYS_MENU_TITLE_FROM_TOP_PX              38         
/*********************
 *      TPEDEFS
 *********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void page_screensavar_construct(void);
static void page_screensavar_destruct(void);
static void page_screensavar_time_upd_cb(uint32_t ms);
static const char* screensaver_get_str(int8_t index);
static void screensaver_release_cb(lv_event_t* e, uint8_t index);
static void screensaver_common_cb(lv_event_t* e);
/*********************
 *  STATIC VATIABLES
 *********************/
static lv_group_t *indev_group;
static sys_config_t* p_sys_menu_model = NULL;
static my_radiobox_ext_t radiobox_ext;
static int32_t disp_time;
static int32_t life_period;
/*********************
 *  GLOBAL VATIABLES
 *********************/
page_t page_screensavar = PAGE_INITIALIZER("Product Info",
                                PAGE_SCREENSAVER, 
                                page_screensavar_construct, 
                                page_screensavar_destruct, 
                                page_screensavar_time_upd_cb,
                                NULL,                                
                                PAGE_MENU);
/*********************
 *  GLOBAL FUNCTIONS
 *********************/

/**********************
 *   STATIC FUNCTIONS
 **********************/    
static void page_screensavar_construct(void)
{
    indev_group = lv_group_create();
    my_indev_set_group(indev_group);
    lv_group_set_default(indev_group);
    lv_obj_set_style_bg_color(lv_scr_act(), lv_color_black(), 0);

    lv_obj_t* screen = lv_obj_create(lv_scr_act());
    lv_obj_remove_style_all(screen);
    lv_obj_set_size(screen, lv_pct(100), lv_pct(100));
    lv_obj_clear_flag(screen, LV_OBJ_FLAG_SCROLLABLE);

    p_sys_menu_model = screen_get_act_page_data();
    (void)p_sys_menu_model;
    uint8_t spid = screen_get_act_spid();
    
    lv_obj_t* title = lv_label_create(screen);    
    lv_obj_set_style_text_font(title, Font_ResouceGet(FONT_22_BOLD), 0);
    lv_obj_set_style_text_color(title, lv_color_white(), 0);
    lv_label_set_text(title, Lang_GetStringByID(STRING_ID_SCEEENSAVAR));
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, SYS_MENU_TITLE_TO_TOP);    

    life_period = -1;
    disp_time = 0;

    switch (spid)
    {
        case SUB_ID_SCREENSAVER_LIST:
        {
            radiobox_ext.sel = p_sys_menu_model->screensaver_index;
            radiobox_ext.max = SCREENSAVAR_SETTING_MAX;
            radiobox_ext.items_of_page = 4;            
            radiobox_ext.get_name_str = screensaver_get_str;
            radiobox_ext.release_cb = screensaver_release_cb;

            lv_obj_t* radiobox_panel = my_radiobox_list_create(screen, 256, &radiobox_ext, MY_RADIOBOX_TYPE2, LV_ALIGN_LEFT_MID);            
            lv_obj_align_to(radiobox_panel, title, LV_ALIGN_OUT_BOTTOM_MID, 0, 2);
            
            lv_group_add_obj(indev_group, radiobox_panel);
            lv_group_set_wrap(indev_group, false);
            lv_group_set_editing(indev_group, true);        
        }
        break;
        case SUB_ID_SCREENSAVER_WARN:                
        {
            life_period = 3 * 1000; 

            LV_IMG_DECLARE(ImgConfirmFailed)
            lv_obj_t* img = lv_img_create(screen);            
            lv_img_set_src(img, &ImgConfirmFailed);
            lv_obj_align(img, LV_ALIGN_TOP_MID, 0, 64);
            lv_obj_add_event_cb(img, screensaver_common_cb, LV_EVENT_KEY, NULL);
            
            lv_obj_t* label = lv_label_create(screen);
            lv_obj_set_style_text_font(label, Font_ResouceGet(FONT_18), 0);
            lv_obj_set_style_text_color(label, lv_color_white(), 0);
            lv_label_set_text(label, Lang_GetStringByID(STRING_ID_SCREENSAVAR_WARN));
            lv_obj_align_to(label, img, LV_ALIGN_OUT_BOTTOM_MID, 0, 39);  
             
            lv_group_add_obj(indev_group, img);
            lv_group_set_wrap(indev_group, false);
            lv_group_set_editing(indev_group, true);                    
        }
        break;
    }
}

static void page_screensavar_destruct(void)
{
    lv_group_del(indev_group);
    lv_obj_clean(lv_scr_act());    
    indev_group = NULL;
}

static void page_screensavar_time_upd_cb(uint32_t ms)
{
    uint8_t spid = screen_get_act_spid();
    switch (spid)
    {
        case SUB_ID_SCREENSAVER_LIST:
            break; 
        case SUB_ID_SCREENSAVER_WARN:
        {
            if (life_period != -1)
            {
                disp_time += 200;
                if (disp_time >= life_period)
                {   
                    screen_turn_prev_page();
                }
            }
        }        
    }
}


static const char* screensaver_get_str(int8_t index)
{
    const char* str = NULL;
    switch(index)
    {
        case SCREENSAVAR_SETTING_AFTER_30_MINS:  str = Lang_GetStringByID(STRING_ID_SCREENSAVAR_30MINS); break;
        case SCREENSAVAR_SETTING_AFTER_60_MINS:  str = Lang_GetStringByID(STRING_ID_SCREENSAVAR_60MINS); break;
        case SCREENSAVAR_SETTING_AFTER_120_MINS: str = Lang_GetStringByID(STRING_ID_SCREENSAVAR_120MINS); break;
        case SCREENSAVAR_SETTING_NO:             str = Lang_GetStringByID(STRING_ID_SCREENSAVAR_NO); break;
    }
    return str;
}

static void screensaver_release_cb(lv_event_t* e, uint8_t index)
{
    lv_event_code_t event = lv_event_get_code(e);
    lv_obj_t* obj = lv_event_get_target(e);

    if (event == LV_EVENT_RELEASED)
    {
        p_sys_menu_model->screensaver_index = index;
        if (p_sys_menu_model->screensaver_index == SCREENSAVAR_SETTING_NO)
        {
            screen_load_sub_page(SUB_ID_SCREENSAVER_WARN, 0);
        }
        else  
        {
            screen_load_prev_sub_page(0, 0);
        }        
    }
    else if (event == LV_EVENT_KEY)
    {        
        uint32_t key = *(uint32_t *)lv_event_get_param(e);

        if (LV_KEY_BACKSPACE == key)
        {
            screen_load_prev_sub_page(0, 0);
        }   
    }        
}

static void screensaver_common_cb(lv_event_t* e)
{
    lv_event_code_t event = lv_event_get_code(e);
    if (event == LV_EVENT_KEY)        
        screen_turn_prev_page();  
}

