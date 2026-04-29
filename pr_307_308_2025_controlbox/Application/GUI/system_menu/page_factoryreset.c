/*********************
 *      INCLUDES
 *********************/
#include "../ui_common.h"
#include "../page_widget.h"
#include "user.h"

/*********************
 *      DEFINES
 *********************/
#define FACTORY_RESET_TIMEOUT                               (3000)
#define FACTORY_RESET_RESULT_TIMEOUT                        (1000)
/*********************
 *      TPEDEFS
 *********************/
enum {
    SUB_ID_FACTORY_CONFIRM = 0,
    SUB_ID_FACTORY_RESETTING,
    SUB_ID_FACTORY_RESULT,
	SUB_ID_FACTORY_RESULT_WRING,
};

LV_IMG_DECLARE(img1s)
LV_IMG_DECLARE(img2s)
LV_IMG_DECLARE(img3s)
/**********************
 *  STATIC PROTOTYPES
 **********************/
static void page_factory_reset_construct(void);
static void page_factory_reset_destruct(void);
static void page_factory_reset_time_upd_cb(uint32_t ms);
static void factory_reset_event_cb(my_msgbox_ext_t* ext);
static void factory_reset_confirm_init(void);
static void factory_reseting_init(void);
static void factory_wring_init(void);
static void bar_anim_exec_xcb(void *a, int32_t val);
static void bar_anim_ready_cb(struct _lv_anim_t * a);
static void factory_com_event_cb(lv_event_t* e);
static void factory_wring_event_cb(lv_event_t* e);
static void wring_anim_exec_xcb(void *a, int32_t val);
static void wring_anim_ready_cb(struct _lv_anim_t * a);
/*********************
 *  STATIC VATIABLES
 *********************/
static lv_group_t *indev_group;
static sys_config_t* p_sys_menu_model = NULL;
static int32_t disp_time;
static int32_t life_period;
static uint8_t reset_flag = 0;
static const lv_img_dsc_t* wring_time[3] = {
	[0] = &img3s, 
	[1] = &img2s, 
	[2] = &img1s,
};
/*********************
 *  GLOBAL VATIABLES
 *********************/
page_t page_factory_reset = PAGE_INITIALIZER("Factory Reset",
                                PAGE_FACTORY,  
                                page_factory_reset_construct, 
                                page_factory_reset_destruct, 
                                page_factory_reset_time_upd_cb,
                                NULL,
                                PAGE_MENU);

/*********************
 *  GLOBAL FUNCTIONS
 *********************/

/**********************
 *   STATIC FUNCTIONS
 **********************/                            
static void page_factory_reset_construct(void)
{
    uint8_t spid = 0;

    indev_group = lv_group_create();
    my_indev_set_group(indev_group);
    lv_group_set_default(indev_group);
    
    lv_obj_set_style_bg_color(lv_scr_act(), lv_color_black(), 0);

    p_sys_menu_model = (sys_config_t*)screen_get_act_page_data();
	
    spid = screen_get_act_spid();
    (void)p_sys_menu_model;

    switch (spid)
    {
    case SUB_ID_FACTORY_CONFIRM:    factory_reset_confirm_init();break;
    case SUB_ID_FACTORY_RESETTING:  factory_reseting_init();break;
    case SUB_ID_FACTORY_RESULT:
    {
        LV_IMG_DECLARE(ImgConfirmOk)
        life_period = FACTORY_RESET_RESULT_TIMEOUT;
        disp_time = 0;
        lv_obj_t* panel = warning_info_init(lv_scr_act(), &ImgConfirmOk, 54,  25,Lang_GetStringByID(STRING_ID_FACTORY_FINISHED));
        lv_obj_add_event_cb(panel, factory_com_event_cb, LV_EVENT_ALL, NULL);
        lv_group_add_obj(indev_group, panel);
        lv_group_focus_obj(panel);
    }
    break;
	case SUB_ID_FACTORY_RESULT_WRING:
		factory_wring_init();
	break;
    default:break;
    }
}

static void page_factory_reset_destruct(void)
{
    lv_group_focus_freeze(indev_group, true);
    lv_group_remove_all_objs(indev_group);
    lv_group_del(indev_group);
    indev_group = NULL; 

    lv_obj_clean(lv_scr_act());
}

static void page_factory_reset_time_upd_cb(uint32_t ms)
{
    uint8_t spid = screen_get_act_spid();
	struct sys_info_dmx        dmx;
	
    switch (spid)
    {
        case SUB_ID_FACTORY_RESULT:
        {
            //if page lifetime ended, then go back to the previous page.
            if (life_period != -1)
            {
                disp_time += 200;
                if (disp_time >= life_period)
                {   
					ui_set_light_reset();
					data_center_read_sys_info(SYS_INFO_DMX, &dmx);
					if((dmx.eth_linked != 1) && (dmx.wired_linked != 1) && (dmx.wireless_linked != 1))
					{
						user_turn_to_page(PAGE_LIGHT_MODE_CCT, 0, false);
					}
					else
					{
						screen_turn_prev_page();
					}
                }
            }
        }
        break;
    }
}

static void factory_reset_confirm_init(void)
{
    my_msgbox_reset();
    my_msgbox_ext_t* p_msgbox_ext = my_msgbox_get_ext();
    LV_IMG_DECLARE(ImgReset);
    p_msgbox_ext->body_comment_str = NULL;
    p_msgbox_ext->body_img = &ImgReset;
    p_msgbox_ext->timecnt = 0;
    p_msgbox_ext->timeout = 30 * 1000;
    p_msgbox_ext->answer = MSGBOX_ID_NO_CONFIRM;
    p_msgbox_ext->type = MSGBOX_TYPE_TIMEOUT;
    p_msgbox_ext->btn_sel = MSGBOX_ID_NO;
    p_msgbox_ext->body_comment_str = NULL;
    p_msgbox_ext->title_str = Lang_GetStringByID(STRING_ID_FACTORY);
    p_msgbox_ext->msgbox_del_cb = factory_reset_event_cb;
    my_msgbox_create(p_msgbox_ext);
}

static void factory_reseting_init(void)
{   
    lv_obj_t* cont = lv_obj_create(lv_scr_act());
    lv_obj_remove_style_all(cont);
    lv_obj_set_size(cont, lv_pct(77), lv_pct(22));
    lv_obj_set_style_text_color(cont, lv_color_make(255, 255, 255), 0);
    lv_obj_set_style_text_font(cont, Font_ResouceGet(FONT_18), 0);
    lv_obj_center(cont);

    lv_obj_t* ble_reset_bar = lv_bar_create(cont);
    lv_obj_set_style_bg_color(ble_reset_bar, lv_color_make(255, 255, 255), LV_PART_INDICATOR);
    lv_obj_set_style_bg_color(ble_reset_bar, lv_color_make(128, 128, 128), LV_PART_MAIN);
    lv_obj_set_style_outline_pad(ble_reset_bar, 0, 0);
    lv_obj_set_style_outline_width(ble_reset_bar, 0, 0);
    lv_obj_set_size(ble_reset_bar, lv_pct(84), lv_pct(32));                
    lv_obj_align(ble_reset_bar, LV_ALIGN_TOP_LEFT, 0, 0);    
    lv_bar_set_range(ble_reset_bar, 0, 100);    

    lv_obj_t* process_label = lv_label_create(cont);
    lv_obj_t* status_label = lv_label_create(cont);
    lv_label_set_text(status_label, Lang_GetStringByID(STRING_ID_FACTORY_RESETING));            
    lv_obj_align(status_label, LV_ALIGN_BOTTOM_MID, 0, 0);         
    
    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, ble_reset_bar);
    lv_anim_set_exec_cb(&a, bar_anim_exec_xcb);
    lv_anim_set_time(&a, FACTORY_RESET_TIMEOUT);
    lv_anim_set_ready_cb(&a, bar_anim_ready_cb);
    lv_anim_set_repeat_count(&a, 1);
    lv_anim_set_values(&a, 0, 99);
    lv_anim_start(&a);
    
    lv_group_add_obj(indev_group, ble_reset_bar);
    lv_group_focus_obj(ble_reset_bar);    
    lv_group_set_editing(indev_group, true);
}

static void factory_wring_init(void)
{   
    lv_obj_t* title_label = lv_label_create(lv_scr_act());
    lv_obj_set_style_text_font(title_label, Font_ResouceGet(FONT_22_BOLD), 0);
    lv_obj_set_style_text_color(title_label, lv_color_white(), 0);
    lv_label_set_text(title_label, Lang_GetStringByID(STRING_ID_FACTORY));
    lv_obj_align(title_label, LV_ALIGN_TOP_MID, 0, SYS_MENU_TITLE_TO_TOP);

    lv_obj_t* img = lv_img_create(lv_scr_act());
    lv_obj_set_style_img_recolor(img,  lv_color_white(), 0);
	lv_obj_set_style_img_recolor_opa(img, LV_OPA_100, 0);
	lv_obj_set_style_img_recolor(img,  lv_color_white(), LV_STATE_FOCUSED);
	lv_obj_set_style_img_recolor_opa(img, LV_OPA_100, LV_STATE_FOCUSED);
	lv_obj_align(img, LV_ALIGN_CENTER, 0, 0);
	lv_img_set_src(img, wring_time[0]);
	
    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, img);
    lv_anim_set_exec_cb(&a, wring_anim_exec_xcb);
    lv_anim_set_time(&a, FACTORY_RESET_TIMEOUT);
    lv_anim_set_ready_cb(&a, wring_anim_ready_cb);
    lv_anim_set_repeat_count(&a, 1);
    lv_anim_set_values(&a, 0, 3);
    lv_anim_start(&a);
    
	lv_obj_t* no_btn = lv_obj_create(lv_scr_act());
    lv_obj_clear_flag(no_btn, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_style(no_btn, &style_common_btn, 0);
    lv_obj_add_style(no_btn, &style_common_focued_btn, LV_STATE_FOCUSED);
    lv_obj_set_style_radius(no_btn, 15, 0);
    lv_obj_set_style_radius(no_btn, 15, LV_STATE_FOCUSED);        
    lv_obj_set_size(no_btn, 75, 40);
	lv_obj_align_to(no_btn, img, LV_ALIGN_OUT_BOTTOM_MID, 0, 30);
	lv_obj_add_event_cb(no_btn, factory_wring_event_cb, LV_EVENT_ALL, NULL);
	
    lv_obj_t* label1 = lv_label_create(no_btn);
    lv_obj_set_style_text_font(label1, Font_ResouceGet(FONT_18), 0);
    lv_label_set_text(label1, Lang_GetStringByID(STRING_ID_NO));
    lv_obj_center(label1);
	
    lv_group_add_obj(indev_group, no_btn);
    lv_group_focus_obj(no_btn);    
    lv_group_set_editing(indev_group, true);
	
}

static void factory_reset_event_cb(my_msgbox_ext_t* ext)
{
	struct sys_info_power info_power;
	uint8_t hs_mode = 0;
	
    if (ext->answer == MSGBOX_ID_YES)
    {
		reset_flag = 0;
		if(ui_get_master_mode())
		{
			extern void gui_factory_event_gen(void);		
			gui_factory_event_gen();
		}
        screen_load_sub_page(SUB_ID_FACTORY_RESETTING, 0);
		data_center_read_sys_info(SYS_INFO_POWER, &info_power);
		if(info_power.indoor_powe == 1)
		{
			#if PROJECT_TYPE==308
			gui_set_power_limit(1700);
			#endif
			#if PROJECT_TYPE==307
			gui_set_power_limit(2000);
			#endif
		}
		else if(info_power.indoor_powe == 2)
		{
			#if PROJECT_TYPE==308
			gui_set_power_limit(1200);
			#endif
			#if PROJECT_TYPE==307
			gui_set_power_limit(1200);
			#endif
		}
		ui_max_power_flag_set(0);
		data_center_read_config_data(SYS_CONFIG_HIGH_SPEED_MODE, &hs_mode);
		ui_restore_def_setting();        //执行进入复位页面  
		if(hs_mode)
		{
			ui_set_hs_mode_state(0);
			data_center_write_config_data_no_event(SYS_CONFIG_HIGH_SPEED_MODE, &hs_mode);
		}
		ui_set_motor_reset();
        UI_PRINTF("Factory Reset Begin...........\r\n");   
    }
    else if (ext->answer == MSGBOX_ID_NO)    
    {
        //执行返回setting页面
        UI_PRINTF("Do not do Factory Reset...........\r\n");   
        screen_turn_prev_page();
    }
}

static void factory_wring_event_cb(lv_event_t* e)
{
	lv_event_code_t event = lv_event_get_code(e);
	
	if (event == LV_EVENT_RELEASED)
    {             
        screen_turn_prev_page();     
    }
}

/**
 * @brief 刷新时间图片
 * 
 * @param a 
 * @param val 
 */
static void wring_anim_exec_xcb(void *a, int32_t val)
{
    lv_obj_t* cont = lv_obj_get_parent(a);
    
	lv_img_set_src((lv_obj_t*)a, wring_time[val]);
	
    lv_obj_align(a, LV_ALIGN_CENTER, 0, 0);    
}

/**
 * @brief 动画结束后
 * 
 * @param a 
 */
static void wring_anim_ready_cb(struct _lv_anim_t * a)
{
	struct sys_info_power info_power;

	uint8_t hs_mode;
	reset_flag = 0;
	if(ui_get_master_mode())
	{
		extern void gui_factory_event_gen(void);		
		gui_factory_event_gen();
	}
	data_center_read_sys_info(SYS_INFO_POWER, &info_power);
	if(info_power.indoor_powe == 1)
	{
		#if PROJECT_TYPE==308
		gui_set_power_limit(1700);
		#endif
		#if PROJECT_TYPE==307
		gui_set_power_limit(2000);
		#endif
	}
	else if(info_power.indoor_powe == 2)
	{
		#if PROJECT_TYPE==308
		gui_set_power_limit(1200);
		#endif
		#if PROJECT_TYPE==307
		gui_set_power_limit(1200);
		#endif
	}
	ui_max_power_flag_set(0);
	data_center_read_config_data(SYS_CONFIG_HIGH_SPEED_MODE, &hs_mode);	
    screen_load_sub_page(SUB_ID_FACTORY_RESETTING, 0);
	ui_restore_def_setting();        //执行进入复位页面  
	if(hs_mode)
	{
		ui_set_hs_mode_state(0);
		data_center_write_config_data_no_event(SYS_CONFIG_HIGH_SPEED_MODE, &hs_mode);
	}
	ui_set_motor_reset();
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
    
	ui_set_lumen_name(&reset_flag, val);
	
    lv_bar_set_value(a, val, LV_ANIM_ON);
    lv_label_set_text_fmt(process_label, "%d%%", val);
    lv_obj_align(process_label, LV_ALIGN_TOP_RIGHT, 0, 0);    
}

/**
 * @brief 动画结束后
 * 
 * @param a 
 */
static void bar_anim_ready_cb(struct _lv_anim_t * a)
{
    screen_load_sub_page(SUB_ID_FACTORY_RESULT, 0);
}

/**
 * @brief 工厂设置通用回调函数
 * 
 * @param e 
 */
static void factory_com_event_cb(lv_event_t* e)
{
    lv_event_code_t event = lv_event_get_code(e);

    // shortcut_keys_event_handler(e);

     if (event == LV_EVENT_KEY)
    {
        uint32_t key = *(uint32_t*)lv_event_get_param(e);
        if (key == LV_KEY_BACKSPACE)
        {
            UI_PRINTF("Update_common event callback!  LV_KEY_BACKSPACE.\r\n");
            screen_turn_prev_page();        
        }
    }
}
