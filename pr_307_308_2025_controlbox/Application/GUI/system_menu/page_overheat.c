/*********************
 *      INCLUDES
 *********************/
#include "../ui_common.h"
#include "../page_widget.h"

/*********************
 *      DEFINES
 *********************/
#define OVERHEAT_DISP_TIME      (30 * 1000)
/*********************
 *      TPEDEFS
 *********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void page_overheat_construct(void);
static void page_overheat_destruct(void);

static void scr_overheat_event(lv_event_t* e);
/*********************
 *  STATIC VATIABLES
 *********************/
static lv_group_t *indev_group;
static sys_config_t* p_sys_menu_model = NULL;
static uint32_t disp_time = 0;
/*********************
 *  GLOBAL VATIABLES
 *********************/
page_t page_overheat = PAGE_INITIALIZER("Over Heat",
                                PAGE_OVERHEAT, 
                                page_overheat_construct, 
                                page_overheat_destruct, 
                                NULL,
                                NULL,                                
                                PAGE_MENU);
/*********************
 *  GLOBAL FUNCTIONS
 *********************/

/**********************
 *   STATIC FUNCTIONS
 **********************/    
static void page_overheat_construct(void)
{
    indev_group = lv_group_create();
    my_indev_set_group(indev_group);
    lv_group_set_default(indev_group);
    lv_obj_set_style_bg_color(lv_scr_act(), lv_color_black(), 0);

    p_sys_menu_model = screen_get_act_page_data();
    disp_time = 0;

    (void)disp_time;
    (void)p_sys_menu_model;

    LV_IMG_DECLARE(ImgOverHeat)
    lv_obj_t* img = lv_img_create(lv_scr_act());    
    lv_img_set_src(img, &ImgOverHeat);
    lv_obj_align(img, LV_ALIGN_CENTER, 0, 0);
    
    lv_obj_t* label = lv_label_create(lv_scr_act());
    lv_label_set_long_mode(label, LV_LABEL_LONG_WRAP);    
    lv_obj_set_size(label, lv_pct(85), LV_SIZE_CONTENT);
    lv_obj_set_style_text_color(label, lv_color_white(), 0);
    lv_obj_set_style_text_font(label, Font_ResouceGet(FONT_14), 0);
    lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, 0);
    lv_label_set_text(label, Lang_GetStringByID(STRING_ID_OVERHEAT_INFO));
    lv_obj_align(label, LV_ALIGN_BOTTOM_MID, 0, -25);

    lv_obj_add_event_cb(img, scr_overheat_event, LV_EVENT_ALL, NULL);
    lv_group_add_obj(indev_group, img);
    lv_group_focus_obj(img);      
    lv_group_set_editing(indev_group, true);    
}

static void page_overheat_destruct(void)
{
    lv_group_focus_freeze(indev_group, true);
    lv_group_remove_all_objs(indev_group);
    lv_group_del(indev_group);
    indev_group = NULL;

    lv_obj_clean(lv_scr_act());
}

static void scr_overheat_event(lv_event_t* e)
{
    lv_event_code_t event = lv_event_get_code(e);

    if (event == LV_EVENT_KEY)  
    {
        uint32_t key = *(uint32_t *)lv_event_get_param(e);

        if (key == LV_KEY_BACKSPACE)
        {            
            screen_turn_prev_page();
        }
    }
}
