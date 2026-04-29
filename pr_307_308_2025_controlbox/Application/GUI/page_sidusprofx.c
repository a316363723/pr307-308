/*********************
 *      INCLUDES
 *********************/
#include "ui_common.h"
#include "page_widget.h"
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
static void screen_event_cb(lv_event_t* e);
static void page_siduspro_construct(void);
static void page_siduspro_destruct(void);
/*********************
 *  STATIC VATIABLES
 *********************/
static lv_group_t *indev_group;
/*********************
 *  GLOBAL VATIABLES
 *********************/
page_t page_siduspro = PAGE_INITIALIZER("Source",
                                PAGE_SIDUS_PRO_FX, 
                                page_siduspro_construct, 
                                page_siduspro_destruct, 
                                NULL,
                                NULL,                                
                                PAGE_NONE);
/*********************
 *  GLOBAL FUNCTIONS
 *********************/

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void page_siduspro_construct(void)
{
    indev_group = lv_group_create();  
    my_indev_set_group(indev_group);
    lv_group_set_default(indev_group);

    lv_obj_set_style_bg_color(lv_scr_act(), lv_color_black(), 0);

    lv_obj_t* screen = lv_obj_create(lv_scr_act());
    lv_obj_remove_style_all(screen);
    lv_obj_set_size(screen, lv_pct(100), lv_pct(100));    

    LV_IMG_DECLARE(ImgBLE)
    lv_obj_t* title_img = lv_img_create(screen);
    lv_img_set_src(title_img,  title_get_comm_img(COM_TYPE_BLE));
    lv_obj_set_style_img_recolor_opa(title_img, LV_OPA_100, 0);
    lv_obj_set_style_img_recolor(title_img, lv_color_white(), 0);
    lv_obj_align(title_img, LV_ALIGN_TOP_RIGHT, -10, 5);

    LV_IMG_DECLARE(ImgSidusproFX)
    lv_obj_t* img = lv_img_create(screen);
    lv_img_set_src(img, &ImgSidusproFX);
    lv_obj_center(img);

    lv_obj_add_event_cb(screen, screen_event_cb, LV_EVENT_ALL, NULL);
    lv_group_add_obj(indev_group, screen);
    lv_group_set_editing(indev_group, true);    
}

static void page_siduspro_destruct(void)
{
    lv_group_focus_freeze(indev_group, true);
    lv_group_remove_all_objs(indev_group);
    lv_group_del(indev_group);
    indev_group = NULL;

    lv_obj_clean(lv_scr_act());
}

static void screen_event_cb(lv_event_t* e)
{    
    lv_event_code_t event = lv_event_get_code(e);
    lv_obj_t* panel = lv_event_get_target(e);

    shortcut_keys_event_handler(e);
    backspace_key_event_handler(e);
}
