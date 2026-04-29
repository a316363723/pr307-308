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
    int16_t min;
    int16_t max;
    int16_t step;
}my_custom_bar_t;
/**********************
 *  STATIC PROTOTYPES
 **********************/
static void page_frqsel_construct(void);
static void page_frqsel_destruct(void);
static void page_frqsel_time_cb(uint32_t ms);
static void page_frqsel_event_cb(int event);
static void refresh_freq_label(void);
static void refresh_freq_bar(void);
static void my_bar_event(lv_event_t* e);
/*********************
 *  STATIC VATIABLES
 *********************/
static lv_group_t *indev_group;
static sys_config_t* p_sys_menu_model = NULL;
static lv_obj_t* frq_label = NULL;
static lv_obj_t* unit_label = NULL;
static lv_obj_t* frq_bar_cont = NULL;
static my_custom_bar_t custom_bar_ext;
/*********************
 *  GLOBAL VATIABLES
 *********************/
page_t page_frqsel = PAGE_INITIALIZER("Frequency",
                                PAGE_FREQUENCY_SELECTION, 
                                page_frqsel_construct, 
                                page_frqsel_destruct, 
                                page_frqsel_time_cb,
                                page_frqsel_event_cb,                                
                                PAGE_MENU);

/*********************
 *  GLOBAL FUNCTIONS
 *********************/

/**********************
 *   STATIC FUNCTIONS
 **********************/                            
static void page_frqsel_construct(void)
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
    lv_label_set_text(title_label, Lang_GetStringByID(STRING_ID_FREQUENCY));
    lv_obj_align(title_label, LV_ALIGN_TOP_MID, 0, SYS_MENU_TITLE_TO_TOP);

    /* 固定加号位置  */
    // lv_obj_t* sign_label = lv_label_create(lv_scr_act());
    // lv_obj_set_size(sign_label, LV_SIZE_CONTENT, 48);
    // lv_obj_set_style_text_font(sign_label, Font_ResouceGet(FONT_48), 0);
    // lv_obj_set_style_text_color(sign_label, lv_color_make(255, 255, 255), 0);
    // lv_label_set_text(sign_label, "+");
    // lv_obj_set_pos(sign_label, 54, 98);
    
    frq_label = lv_label_create(lv_scr_act());
    lv_obj_set_size(frq_label, LV_SIZE_CONTENT, 48);
    lv_obj_set_style_text_font(frq_label, Font_ResouceGet(FONT_48), 0);
    lv_obj_set_style_text_color(frq_label, lv_color_make(255, 255, 255), 0);
    
    unit_label = lv_label_create(lv_scr_act());
    lv_obj_set_size(unit_label, LV_SIZE_CONTENT, 18);
        
    lv_obj_set_style_text_font(unit_label, Font_ResouceGet(FONT_18), 0);
    lv_obj_set_style_text_color(unit_label, lv_color_white(), 0);    
    lv_label_set_text(unit_label, "Hz");
    lv_obj_set_pos(unit_label, 227, 120);

    custom_bar_ext.max = 2000;
    custom_bar_ext.min = 0;
    custom_bar_ext.step = 100;

    if (p_sys_menu_model->frequency < custom_bar_ext.min)
    {
        p_sys_menu_model->frequency = custom_bar_ext.min;
    }
    else if(p_sys_menu_model->frequency > custom_bar_ext.max)
    {
        p_sys_menu_model->frequency = custom_bar_ext.max;
    }

    refresh_freq_label();

    frq_bar_cont = lv_obj_create(lv_scr_act());
    lv_obj_remove_style_all(frq_bar_cont);
    lv_obj_set_size(frq_bar_cont, 270, 18);
    lv_obj_align(frq_bar_cont, LV_ALIGN_TOP_MID, 0, 161);

    lv_obj_t* frq_bar =lv_obj_create(frq_bar_cont);
    lv_obj_set_size(frq_bar, 256, 9);
    lv_obj_center(frq_bar);

    LV_IMG_DECLARE(ImgBarDot);
    lv_obj_t* bar_dot = lv_img_create(frq_bar_cont);
    lv_img_set_src(bar_dot, &ImgBarDot);
    
    lv_obj_add_event_cb(frq_bar_cont, my_bar_event, LV_EVENT_ALL, NULL);
    refresh_freq_bar();
    lv_group_add_obj(indev_group, frq_bar_cont);
    lv_group_focus_obj(frq_bar_cont);
    lv_group_set_editing(indev_group, true);
    lv_group_set_wrap(indev_group, false);
}

static void page_frqsel_destruct(void)
{
    lv_group_del(indev_group);
    lv_obj_clean(lv_scr_act());
    indev_group = NULL; 
}

static void page_frqsel_time_cb(uint32_t ms)
{
    uint16_t frequency = p_sys_menu_model->frequency;

    //从数据中心拉取频率.
    data_center_read_config_data(SYS_CONFIG_FRQ, &frequency);

    //如果后台改变频率了， 那么刷新界面.
    if (frequency != p_sys_menu_model->frequency)
    {
        refresh_freq_bar();
        refresh_freq_label();    
    }
}

static void page_frqsel_event_cb(int event)
{
    uint16_t frquency;
    data_center_read_config_data(SYS_CONFIG_FRQ, &frquency);
    switch (event)
    {
        case EVENT_DATA_PULL:
            p_sys_menu_model->frequency = frquency;    
        break;
        case EVENT_DATA_WRITE:
            if (frquency != p_sys_menu_model->frequency)
            {
                data_center_write_config_data(SYS_CONFIG_FRQ, &p_sys_menu_model->frequency);
            }
        break;
    }
}

static void refresh_freq_label(void)
{
    lv_label_set_text_fmt(frq_label, "+%d", p_sys_menu_model->frequency);
    lv_obj_align_to(frq_label, unit_label, LV_ALIGN_OUT_LEFT_BOTTOM, -12, 7);    
}

static void refresh_freq_bar(void)
{
    lv_obj_t* bar_cont = lv_obj_get_child(frq_bar_cont, 0);
    lv_obj_t* bar_dot = lv_obj_get_child(frq_bar_cont, 1);
    lv_coord_t distance = lv_map(p_sys_menu_model->frequency, custom_bar_ext.min, custom_bar_ext.max, 18, 256);
    lv_obj_align_to(bar_dot, bar_cont, LV_ALIGN_OUT_LEFT_MID, distance, 0);
}

static void my_bar_event(lv_event_t* e)
{
    lv_event_code_t event = lv_event_get_code(e);
    lv_obj_t* target = lv_event_get_target(e);

    shortcut_keys_event_handler(e);

    if (LV_EVENT_KEY == event)
    {
        uint32_t key = *(uint32_t*)lv_event_get_param(e);
        if (key == LV_ENCODER_KEY_CW)
        {
            int16_t value = p_sys_menu_model->frequency + custom_bar_ext.step;
            if (value >= custom_bar_ext.max)
            {
                value = custom_bar_ext.max;                                
            }
            p_sys_menu_model->frequency = value;
            // lv_bar_set_value(target, p_sys_menu_model->frequency, LV_ANIM_OFF);
            page_event_send(EVENT_DATA_WRITE);
            refresh_freq_bar();
            refresh_freq_label();
            if(ui_get_master_mode())
			{
				extern void gui_data_sync_event_gen(void);		
				gui_data_sync_event_gen();
			}
        }
        else if (key == LV_ENCODER_KEY_CC)
        {
            int16_t value = p_sys_menu_model->frequency - custom_bar_ext.step;
            if (value <= custom_bar_ext.min)
            {
                value = custom_bar_ext.min;                                
            }
            p_sys_menu_model->frequency = value;
            //lv_bar_set_value(target, p_sys_menu_model->frequency,LV_ANIM_OFF);
            page_event_send(EVENT_DATA_WRITE);
            refresh_freq_bar();
            refresh_freq_label();
			if(ui_get_master_mode())
			{
				extern void gui_data_sync_event_gen(void);		
				gui_data_sync_event_gen();
			}
        }
        else  if (key == LV_KEY_BACKSPACE)
        {
            screen_turn_prev_page();
        }
    }
    else if (LV_EVENT_RELEASED == event)
    {
        UI_PRINTF("OutputMode Release Callback!\r\n");        
        screen_turn_prev_page();    
    }
}
