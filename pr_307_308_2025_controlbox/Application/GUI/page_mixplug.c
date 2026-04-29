/*********************
 *      INCLUDES
 *********************/
#include "ui_common.h"
#include "page_widget.h"
#include "user.h"

/*********************
 *      DEFINES
 *********************/

/*********************
 *      TPEDEFS
 *********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void screen_mixplug_event_cb(lv_event_t* e);
//static void screen_anim_exec_xcb(lv_anim_t * a);
/*********************
 *  STATIC VATIABLES
 *********************/
static lv_group_t* indev_group;
static bool deleted = true;
/*********************
 *  GLOBAL VATIABLES
 *********************/

/*********************
 *  GLOBAL FUNCTIONS
 *********************/
void screen_mixplug_exit(void)
{
    if (deleted)       
        return;

    lv_obj_t* obj = lv_group_get_focused(indev_group);
	uint8_t key = LV_KEY_ESC;
	lv_event_send(obj, LV_EVENT_KEY, &key);
}

void screen_mixplug_create(void)
{
    LV_IMG_DECLARE(ImgConfirmFailed)

    if (!deleted)       
        return;

    deleted = false;
    indev_group = lv_group_create();
    my_indev_set_group(indev_group);

    lv_obj_t* screen_panel = lv_obj_create(lv_scr_act());
    lv_obj_clear_flag(screen_panel, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_radius(screen_panel, 0, 0);
    lv_obj_set_style_pad_all(screen_panel, 0, 0);
    lv_obj_set_style_border_width(screen_panel, 0, 0);
    lv_obj_set_style_outline_width(screen_panel, 0, 0);    
    lv_obj_set_style_bg_color(screen_panel, lv_color_black(), 0);
    lv_obj_set_size(screen_panel, lv_pct(100), lv_pct(100));
    warning_info_init(screen_panel, &ImgConfirmFailed, 54, 25, Lang_GetStringByID(STRING_ID_MIX_PLUG));
    
    lv_obj_add_event_cb(screen_panel, screen_mixplug_event_cb, LV_EVENT_ALL, NULL);
    lv_obj_center(screen_panel);

    lv_group_add_obj(indev_group, screen_panel);
    lv_group_set_editing(indev_group, true);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

// static void screen_anim_exec_xcb(lv_anim_t * a)
// {
//     screen_mixplug_exit();
// }

static void screen_mixplug_event_cb(lv_event_t* e)
{
    lv_event_code_t event = lv_event_get_code(e);
    lv_obj_t* panel = lv_event_get_target(e);

    if (LV_EVENT_KEY == event)
    {           
        if (!deleted)
        {
            deleted = true;
            lv_obj_del_async(panel);
            my_indev_set_group(lv_group_get_default());
        }             
    }
    else if (LV_EVENT_DELETE == event)
    {
        deleted = true;

        /* 释放组资源 */
        lv_group_focus_freeze(indev_group, true);
        lv_group_remove_all_objs(indev_group);
        lv_group_del(indev_group);        
        indev_group = NULL;

        /* 手动退出状态机 */
        user_exit_mixplug_warn();
    }
}
