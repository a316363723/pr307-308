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
static void page_studiomode_construct(void);
static void page_studiomode_destruct(void);
static void page_studiomode_time_cb(uint32_t ms);
static void page_studiomode_event_cb(int event);
static void refresh_studiomode_disp(void);
static void studiomode_event_cb(lv_event_t *e);
/*********************
 *  STATIC VATIABLES
 *********************/
static lv_group_t *indev_group;
static sys_config_t* p_sys_menu_model = NULL;
static lv_obj_t* body_cont = NULL;
static lv_obj_t* img_btn = NULL;
/*********************
 *  GLOBAL VATIABLES
 *********************/
page_t page_studiomode = PAGE_INITIALIZER("Studio Mode",
                                PAGE_STUDIO_MODE, 
                                page_studiomode_construct, 
                                page_studiomode_destruct, 
                                page_studiomode_time_cb,
                                page_studiomode_event_cb,                                
                                PAGE_MENU);

/*********************
 *  GLOBAL FUNCTIONS
 *********************/

/**********************
 *   STATIC FUNCTIONS
 **********************/                            
static void page_studiomode_construct(void)
{
    indev_group = lv_group_create();
    p_sys_menu_model = (sys_config_t*)screen_get_act_page_data();

    page_event_send(EVENT_DATA_PULL);
    my_indev_set_group(indev_group);
    lv_group_set_default(indev_group);
    lv_obj_set_style_bg_color(lv_scr_act(), lv_color_black(), 0);

    lv_obj_t* title_label = lv_label_create(lv_scr_act());
    lv_obj_set_style_text_font(title_label, Font_ResouceGet(FONT_22_BOLD), 0);
    lv_obj_set_style_text_color(title_label, lv_color_white(), 0);
    lv_label_set_text(title_label, Lang_GetStringByID(STRING_ID_STUDIO));
    lv_obj_align(title_label, LV_ALIGN_TOP_MID, 0, SYS_MENU_TITLE_TO_TOP);

    body_cont = lv_obj_create(lv_scr_act());
    lv_obj_remove_style_all(body_cont);
    lv_obj_set_size(body_cont, 98, 87);
    lv_obj_align(body_cont, LV_ALIGN_CENTER, -3, -15);
    //lv_obj_center(body_cont);

    LV_IMG_DECLARE(ImgStudioPerson);
    LV_IMG_DECLARE(ImgStudioLight);
    lv_obj_t* img1 = lv_img_create(body_cont);
    lv_img_set_src(img1, &ImgStudioPerson);
    lv_obj_align(img1, LV_ALIGN_BOTTOM_MID, 0, 0);

    lv_obj_t* img2 = lv_img_create(body_cont);
    lv_img_set_src(img2, &ImgStudioLight);
    lv_obj_align(img2, LV_ALIGN_TOP_LEFT, 0, 0);

    img_btn = lv_img_create(lv_scr_act());
    refresh_studiomode_disp();    

    //lv_obj_align_to(body_cont, img_btn, LV_ALIGN_OUT_TOP, 0, -30);
    lv_obj_add_event_cb(img_btn, studiomode_event_cb, LV_EVENT_ALL, NULL);

    lv_group_add_obj(indev_group, img_btn);
    lv_group_set_wrap(indev_group, false);
    lv_group_set_editing(indev_group, true);
}

static void page_studiomode_destruct(void)
{
    lv_group_del(indev_group);    
    lv_obj_clean(lv_scr_act());
    indev_group = NULL; 
    page_event_send(EVENT_DATA_WRITE);
}

static void page_studiomode_time_cb(uint32_t ms)
{
    uint8_t studio_mode = p_sys_menu_model->studiomode_on;

    //从数据中心中更新演播厅模式
    data_center_read_config_data(SYS_CONFIG_STUDIO_MODE, &studio_mode);

    //更新演播厅模式
    if (studio_mode != p_sys_menu_model->studiomode_on)
    {
        p_sys_menu_model->studiomode_on = studio_mode;
        refresh_studiomode_disp();
    }
}

static void page_studiomode_event_cb(int event)
{
    uint16_t studiomode;
    data_center_read_config_data(SYS_CONFIG_STUDIO_MODE, &studiomode);
    switch (event)
    {
        case EVENT_DATA_PULL:
            p_sys_menu_model->studiomode_on = studiomode;    
            page_set_invalid_type(PAGE_INVALID_TYPE_ALL);
        break;
        case EVENT_DATA_WRITE:
            if (studiomode != p_sys_menu_model->studiomode_on)
            {
                data_center_write_config_data(SYS_CONFIG_STUDIO_MODE, &p_sys_menu_model->studiomode_on);
            }
        break;
    }
}

static void refresh_studiomode_disp(void)
{
    LV_IMG_DECLARE(ImgStudioOn)
    LV_IMG_DECLARE(ImgStudioOff)

    lv_obj_t* img1 = lv_obj_get_child(body_cont, 0);
    lv_obj_t* img2 = lv_obj_get_child(body_cont, 1);

    if (p_sys_menu_model->studiomode_on == STUDIO_MODE_ON)
    {
        lv_obj_set_style_img_recolor(img1, lv_color_white(), 0);
        lv_obj_set_style_img_recolor_opa(img1, LV_OPA_100, 0);
        lv_obj_set_style_img_recolor(img2, lv_color_white(), 0);
        lv_obj_set_style_img_recolor_opa(img2, LV_OPA_100, 0);
        lv_obj_clear_flag(img2, LV_OBJ_FLAG_HIDDEN);

        lv_img_set_src(img_btn, &ImgStudioOn);
        lv_obj_align(img_btn, LV_ALIGN_BOTTOM_MID, 0, -32);      
    }
    else
    {
        lv_obj_set_style_img_recolor(img1, lv_color_make(179, 179, 179), 0);
        lv_obj_set_style_img_recolor_opa(img1, LV_OPA_100, 0);
        lv_obj_add_flag(img2, LV_OBJ_FLAG_HIDDEN);

        lv_img_set_src(img_btn, &ImgStudioOff);                             
        lv_obj_align(img_btn, LV_ALIGN_BOTTOM_MID, 0, -32);
    }
}

static void studiomode_event_cb(lv_event_t *e)
{
    lv_event_code_t event = lv_event_get_code(e);

    shortcut_keys_event_handler(e);

    if (event == LV_EVENT_RELEASED)
    {
        p_sys_menu_model->studiomode_on = !p_sys_menu_model->studiomode_on;
        data_center_write_config_data(SYS_CONFIG_STUDIO_MODE, &p_sys_menu_model->studiomode_on);
        refresh_studiomode_disp();
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
