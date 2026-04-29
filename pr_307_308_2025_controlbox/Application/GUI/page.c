/*********************
 *      INCLUDES
 *********************/
#include "ui_common.h"
#include "page_widget.h"
#include "user.h"

/*********************
 *      DEFINES
 *********************/
#define BATT_FRAME_LOW_POWER_COLOR              lv_color_make(255, 0, 0)
#define BATT_FRAME_NORMAL_POWER_COLOR           lv_color_make(0, 255, 0)
#define BATT_CHARGING_COLOR                     lv_color_make(255, 255, 0)
#define BATT_CAPACITY_MAX_HEIGHT                15
#define BATT_CAPACITY_WIDTH                     8
#define ICON_GAP                               -2
#define BATT_CHARGE_ANIMATION_TIME              (2 * 1000)
#define BATT_DISP_MAX_SWITCH_TIME               (3 * 1000)

#define BATT_LOW_TIME_MINUTES                   (10)
/**********************
 *      TYPEDEFS
 **********************/
typedef struct _screen_dsc_t {
    lv_obj_t* l2_screen;
}screen_dsc_t;

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void      batt_charge_anim_cb(void* a, int32_t value);
static lv_obj_t* batt_icon_create(lv_obj_t* parent);
static bool      power_disp_refresh(lv_obj_t* cont, ui_power_dsc_t* p_ui_dsc, bool force_upd);
static void      power_disp_init(lv_obj_t* cont, ui_power_dsc_t* p_dsc);
static void      refresh_charge_cont(lv_obj_t* charge_cont, ui_power_dsc_t* p_dsc, bool force_upd);
static void      l2_scr_del_event_cb(lv_event_t* e);
/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *  GLOBAL VARIABLES
 **********************/
ui_title_t title_model;
screen_dsc_t screen_dsc;
/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void obj_send_event_to_screen(lv_event_code_t code, void *param)
{
    lv_event_send(lv_scr_act(), code, param);
}

void screen_del_l2_panel(void)
{
    uint32_t key = LV_KEY_BACKSPACE;
    obj_send_event_to_screen(LV_EVENT_KEY, (void *)&key);    
}

void screen_exist_l2_panel(void)
{
	if (screen_dsc.l2_screen != NULL)
	{
		lv_obj_del_async(screen_dsc.l2_screen);
		screen_dsc.l2_screen = NULL;
	} 
}

bool l2_screen_is_exist(void)
{
    return screen_dsc.l2_screen == NULL ? false : true;
}

/**
 * @brief 当前屏幕回调函数
 * 
 * @param e 
 */
void screen_event_cb(lv_event_t* e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    
    if (LV_EVENT_KEY == event_code)
    {
        uint32_t key = *(uint32_t *)lv_event_get_param(e);
        if (LV_KEY_HOME == key)
        {
            if (screen_dsc.l2_screen == NULL)
            {
                screen_dsc.l2_screen = drop_down_create(lv_scr_act());
                lv_obj_add_event_cb(screen_dsc.l2_screen, l2_scr_del_event_cb, LV_EVENT_ALL, NULL);
            }
        }
        else if (LV_KEY_END == key)
        {
            if (screen_dsc.l2_screen == NULL)
            {
                screen_dsc.l2_screen = page_preset_task_init(lv_scr_act());
                lv_obj_add_event_cb(screen_dsc.l2_screen, l2_scr_del_event_cb, LV_EVENT_ALL, NULL);
            }
        }
        else if (LV_KEY_BACKSPACE == key)
        {
            if (screen_dsc.l2_screen != NULL)
            {
                lv_obj_del_async(screen_dsc.l2_screen);
                screen_dsc.l2_screen = NULL;
            } 
			else if (screen_dsc.l2_screen == NULL)
            {
                screen_dsc.l2_screen = drop_down_create(lv_scr_act());
                lv_obj_add_event_cb(screen_dsc.l2_screen, l2_scr_del_event_cb, LV_EVENT_ALL, NULL);
            }
        }
    }
	
	//DMX界面跳转
	
    //电子附件界面跳转
//    static uint16_t accessory_type_pre = 0;
////    g_ptUIStateData->ele_accessory.electric_accessory_type = 3;  //testing
//    uint16_t accessory_type_cur = g_ptUIStateData->ele_accessory.electric_accessory_type;

//    if (accessory_type_cur > accessory_type_pre || \
//        (accessory_type_cur != 0 && accessory_type_cur != accessory_type_pre && screen_get_prev_id() == PAGE_ELECT_ACCESSORY && screen_get_act_spid() == SUB_ID_ACCESSORY_LIST))
//    {
//        screen_load_page(PAGE_ELECT_ACCESSORY, 0, false);
//        screen_set_act_spid(0xff);
//        //cur_page_spid_refresh();Temporarily shielded
//    }
//    else if (accessory_type_pre != 0 && accessory_type_cur == 0 )
//    {
//        if(screen_get_prev_id() == PAGE_ELECT_ACCESSORY)
//            screen_load_page(PAGE_LIGHT_MODE_CCT, 0, true);
//        if(screen_get_prev_id() == PAGE_MENU)
//        {
//            screen_set_act_spid(0xff);
//            //cur_page_spid_refresh();Temporarily shielded
//        }
//    }
//    accessory_type_pre = accessory_type_cur;


}

/**
 * @brief 标题栏容器刷新
 * 
 * @param t_cont 
 * @param p_t_model 
 * @param force_upd 
 */
void title_cont_refresh(lv_obj_t* t_cont, ui_title_t* p_t_model, bool force_upd)
{
	lv_obj_t* cont = lv_obj_get_child(t_cont, 1);
    lv_obj_t* img_dmx_teminal_mode = lv_obj_get_child(cont, 0);
    lv_obj_t* img_master_mode = lv_obj_get_child(cont, 1);
    lv_obj_t* img_curve = lv_obj_get_child(cont, 2);
    lv_obj_t* img_fan = lv_obj_get_child(cont, 3);
    lv_obj_t* img_com = lv_obj_get_child(cont, 4);
	lv_obj_t* img_high = lv_obj_get_child(cont, 5);
    lv_obj_t* power_cont = lv_obj_get_child(cont, 6);
	
    if (p_t_model->power_type != ui_get_power_type() || force_upd || p_t_model->indoor_power_type != gui_get_indoor_flag()) {
        p_t_model->power_type = ui_get_power_type();
		p_t_model->indoor_power_type = gui_get_indoor_flag();
        p_t_model->power_dsc.type = p_t_model->power_type;
        power_disp_init(power_cont, &p_t_model->power_dsc);
        force_upd = true;
    }
    force_upd = power_disp_refresh(power_cont, &p_t_model->power_dsc, force_upd);

    sys_config_t *p_sys_menu = &g_tUIAllData.sys_menu_model;

    /* 更新所有的图标 */
    force_upd = true;
    if (p_t_model->com_type != ui_get_comm_type() || \
            p_t_model->fan_mode != ui_get_fan_mode() || \
                p_t_model->curve_type != ui_get_curve_type() || \
                    p_t_model->work_mode != ui_get_master_mode() ||\
                        p_t_model->dmx_temination_on != ui_get_dmx_temination_mode() || \
							p_t_model->high_speed_mode != ui_get_hight_speed_mode() ||force_upd)
    {
        p_t_model->com_type = ui_get_comm_type();
        p_t_model->fan_mode = ui_get_fan_mode();
        p_t_model->curve_type = ui_get_curve_type();
        p_t_model->high_speed_mode = ui_get_hight_speed_mode();
		
        if (!(p_sys_menu->ble_on || p_sys_menu->crmx_on)) 
        {
            if (g_ptUIStateData->lan_state == false)
            {
                lv_obj_add_flag(img_com, LV_OBJ_FLAG_HIDDEN);    
            }            
        } 

        if (g_ptUIStateData->lan_state == true)
        {
            p_t_model->com_type = COM_TYPE_NETWORK;
            lv_obj_clear_flag(img_com, LV_OBJ_FLAG_HIDDEN);    
        }
		else if(p_sys_menu->ble_on || p_sys_menu->crmx_on )
		{
			if(p_sys_menu->crmx_on == 1)
			{
				if(crmx_is_paired() == 0)
				{
					lv_obj_add_flag(img_com, LV_OBJ_FLAG_HIDDEN);
				}
				else
				{
					lv_obj_clear_flag(img_com, LV_OBJ_FLAG_HIDDEN);
				}
			}
			else
			{
				lv_obj_set_style_img_recolor_opa(img_com, LV_OPA_100, 0);
				lv_obj_set_style_img_recolor(img_com, lv_color_make(255, 255, 255), 0);
				lv_obj_clear_flag(img_com, LV_OBJ_FLAG_HIDDEN);
			}
		}			
		
		if(p_t_model->high_speed_mode)
		{
			lv_obj_clear_flag(img_high, LV_OBJ_FLAG_HIDDEN);
		}
		else
		{
			lv_obj_add_flag(img_high, LV_OBJ_FLAG_HIDDEN);
		}
		
        lv_img_set_src(img_com, title_get_comm_img(p_t_model->com_type));                
        lv_img_set_src(img_fan, title_get_fan_img(p_t_model->fan_mode)); 
//		lv_img_set_src(img_master_mode, ui_get_mastermode_img()); 
        lv_img_set_src(img_curve, title_get_small_curve_img(p_t_model->curve_type)); 
		lv_img_set_src(img_high, ui_get_high_speed_img());   
		
        lv_obj_set_style_img_recolor_opa(img_curve, LV_OPA_100, 0);
        lv_obj_set_style_img_recolor(img_curve, lv_color_make(255, 255, 255), 0);
//		lv_obj_set_style_img_recolor_opa(img_master_mode, LV_OPA_100, 0);
//        lv_obj_set_style_img_recolor(img_master_mode, lv_color_make(255, 255, 255), 0);
        p_t_model->work_mode = ui_get_master_mode();
        if (p_t_model->work_mode)
        {
			lv_img_set_src(img_master_mode, ui_get_mastermode_img(0));
//            lv_obj_clear_flag(img_master_mode, LV_OBJ_FLAG_HIDDEN);
        }
        else 
        {
			lv_img_set_src(img_master_mode, ui_get_mastermode_img(1));
//            lv_obj_add_flag(img_master_mode, LV_OBJ_FLAG_HIDDEN);
        }

        p_t_model->dmx_temination_on = ui_get_dmx_temination_mode();
        if (p_t_model->dmx_temination_on)
        {
            lv_obj_clear_flag(img_dmx_teminal_mode, LV_OBJ_FLAG_HIDDEN);
        }
        else
        {
            lv_obj_add_flag(img_dmx_teminal_mode, LV_OBJ_FLAG_HIDDEN);
        }
		
    }

    lv_obj_align(cont, LV_ALIGN_RIGHT_MID,-10,0);
}

/**
 * @brief 标题栏刷新处理
 * 
 * @param t_cont 
 */
void title_update_handler(lv_obj_t* t_cont)
{
    if (t_cont != NULL)
    {
        title_cont_refresh(t_cont, &title_model, false);
    }
}

/**
 * @brief 创建标题栏
 * 
 * @param t_cont 
 * @param title_name 
 */
void title_cont_create(lv_obj_t* t_cont, const char* title_name)
{
    lv_obj_t* label_t = lv_label_create(t_cont);    
    lv_obj_set_style_text_font(label_t, Font_ResouceGet(FONT_22_BOLD), 0);
    lv_label_set_text(label_t, title_name);
    lv_obj_align(label_t, LV_ALIGN_BOTTOM_LEFT, 10, 0);

    lv_obj_t* cont = lv_obj_create(t_cont);
    lv_obj_remove_style_all(cont);
    lv_obj_set_size(cont, LV_SIZE_CONTENT, lv_pct(100));    
    lv_obj_set_flex_flow(cont, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(cont, LV_FLEX_ALIGN_CENTER,LV_FLEX_ALIGN_CENTER,LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_column(cont, 4, 0);

    lv_obj_t* img_dmx_teminal_mode = lv_img_create(cont);
    lv_obj_t* img_master_mode = lv_img_create(cont);
    lv_obj_t* img_curve = lv_img_create(cont);
    lv_obj_t* img_fan = lv_img_create(cont);
    lv_obj_t* img_com = lv_img_create(cont);
	lv_obj_t* img_high_speed = lv_img_create(cont);
    lv_obj_t* power_cont = lv_obj_create(cont);    
	
    lv_obj_remove_style_all(power_cont);
    lv_obj_set_size(power_cont, LV_SIZE_CONTENT, lv_pct(100));
    
    LV_IMG_DECLARE(ImgDMXTeminationIcon_L)
    lv_img_set_src(img_dmx_teminal_mode, &ImgDMXTeminationIcon_L);

    power_disp_init(power_cont, &title_model.power_dsc);
    title_cont_refresh(t_cont, &title_model, true);
}


/**
 * @brief 光效标题栏的创建
 * 
 * @param t_cont 
 * @param title_name 
 */
void light_effect_title_create(lv_obj_t* t_cont, const char* title_name)
{
    char name[30] = {0};
	char name_copy[30] = {0};
    #if PROJECT_TYPE==307
//    if (!(screen_get_act_pid() == PAGE_FAULTYBULB || screen_get_act_pid() == PAGE_WELDING || screen_get_act_pid() == PAGE_COPCAR))
//    {
//        lv_snprintf(name, sizeof(name), "%s II", title_name);
//    }
//    else
//    {
        lv_snprintf(name, sizeof(name), "%s", title_name);
//    }
    #elif PROJECT_TYPE==308
    if (screen_get_act_pid() == PAGE_PULSING || screen_get_act_pid() == PAGE_STROBE || screen_get_act_pid() == PAGE_LIGHTNING
       || screen_get_act_pid() == PAGE_TV || screen_get_act_pid() == PAGE_FAULTYBULB || screen_get_act_pid() == PAGE_FIRE)
    {
        for(uint8_t i = 0; i < 30; i++)
        {
            if(title_name[i] == 'I')
                break;
            name_copy[i] = title_name[i];
        }
        lv_snprintf(name, sizeof(name), "%s", name_copy);
    }
    else
    {
        lv_snprintf(name, sizeof(name), "%s", title_name);
    }
    #endif
    lv_obj_t* title = lv_label_create(t_cont);
    lv_obj_set_style_text_font(title, Font_ResouceGet(FONT_22_BOLD), 0);
    lv_label_set_text(title, name);
    lv_obj_center(title);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * @brief 电池动画回调函数
 * 
 * @param a 
 * @param value 
 */
static void batt_charge_anim_cb(void* a, int32_t value)
{
    lv_obj_set_size(a, BATT_CAPACITY_WIDTH, value);
    lv_obj_align(a, LV_ALIGN_BOTTOM_MID, 0, -2);
}

/**
 * @brief 创建一个电池
 * 
 * @param parent 
 * @return lv_obj_t* 
 */
static lv_obj_t* batt_icon_create(lv_obj_t* parent)
{
    LV_IMG_DECLARE(ImgBatteryFrame)
    lv_obj_t* batt = lv_obj_create(parent);
    lv_obj_clear_flag(batt, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_style(batt, &style_rect_panel, 0);
    lv_obj_set_style_bg_img_src(batt, &ImgBatteryFrame, 0);
    lv_obj_set_style_bg_img_recolor_opa(batt, LV_OPA_100, 0);
    lv_obj_set_style_bg_img_recolor(batt, lv_color_make(255, 0, 0), 0);
    lv_obj_set_size(batt, 12, 22);

    lv_obj_t* capactiy = lv_obj_create(batt);
    lv_obj_clear_flag(capactiy, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_style(capactiy, &style_rect_panel, 0);
    lv_obj_set_style_radius(capactiy, 0, 0);
    lv_obj_set_style_bg_color(capactiy, lv_color_make(255, 255, 0), 0);
    lv_obj_set_size(capactiy, BATT_CAPACITY_WIDTH, 1);
    lv_obj_align(capactiy, LV_ALIGN_BOTTOM_MID, 0, -2);

    LV_IMG_DECLARE(ImgChargeIcon)    
    lv_obj_t* img = lv_img_create(batt);
    lv_img_set_src(img, &ImgChargeIcon);
    lv_obj_set_style_img_recolor(img, BATT_FRAME_NORMAL_POWER_COLOR, 0);
    lv_obj_set_style_img_recolor_opa(img, LV_OPA_100, 0);
    lv_obj_add_flag(img, LV_OBJ_FLAG_HIDDEN);
    lv_obj_center(img);    

    return batt;
}

/**
 * @brief 供电部分显示初始化
 * 
 * @param cont 
 * @param p_dsc 
 */
static void power_disp_init(lv_obj_t* cont,  ui_power_dsc_t* p_dsc)
{
    lv_obj_clean(cont);
    switch (p_dsc->type) 
    {
        case POWER_TYPE_BATTERY:
        {
            lv_obj_t* batt = batt_icon_create(cont);
            lv_obj_align(batt, LV_ALIGN_LEFT_MID, 0, 0);
            lv_obj_t* label = lv_label_create(cont);
            lv_obj_set_style_text_font(label, Font_ResouceGet(FONT_18), 0);
            lv_obj_set_style_text_color(label, lv_color_make(255, 255, 255), 0);
            lv_obj_align_to(label, batt, LV_ALIGN_OUT_RIGHT_MID, 2, 2);
        }
        break;
        case POWER_TYPE_DC:
        {
			lv_obj_t* img = lv_img_create(cont);
			if((gui_get_indoor_flag() >= 1))
			{
				lv_img_set_src(img, title_get_indoor_power_img());   
				lv_obj_align(img, LV_ALIGN_LEFT_MID, 0, 0);  
			}
			else
			{
				LV_IMG_DECLARE(ImgAC);
				lv_obj_set_style_img_recolor_opa(img, LV_OPA_100, 0);
				lv_obj_set_style_img_recolor(img, lv_color_white(), 0);                    
				lv_img_set_src(img, &ImgAC);            
				lv_obj_align(img, LV_ALIGN_LEFT_MID, 0, 0);  
			}
        }
        break;
        case POWER_TYPE_SUPPLY_BOX:
        break;
        case POWER_TYPE_DC_WITH_BATT:
        {
            p_dsc->dsc.dc_with_batt.b_anim_start = false;

            /* w = 12 + 5 + 5 + 2 = 24 */
            lv_obj_t* charge_cont = lv_obj_create(cont);
            lv_obj_remove_style_all(charge_cont);
            lv_obj_set_size(charge_cont, 30, lv_pct(100));
            LV_IMG_DECLARE(ImgBattRightFlag)
            LV_IMG_DECLARE(ImgBattLeftFlag)
            lv_obj_t* batt = batt_icon_create(charge_cont);
            lv_obj_t* img_left = lv_img_create(charge_cont);
            lv_obj_t* img_right = lv_img_create(charge_cont);
            lv_obj_set_style_img_recolor_opa(img_left, LV_OPA_100, 0);
            lv_obj_set_style_img_recolor(img_left, lv_color_white(), 0);
            lv_obj_set_style_img_recolor_opa(img_right, LV_OPA_100, 0);
            lv_obj_set_style_img_recolor(img_right, lv_color_white(), 0);
            lv_img_set_src(img_left, &ImgBattLeftFlag);            
            lv_img_set_src(img_right, &ImgBattRightFlag);
            lv_obj_align(batt, LV_ALIGN_BOTTOM_MID, 0, -3);
            lv_obj_align_to(img_left, batt, LV_ALIGN_OUT_TOP_LEFT, -7, 6);
            lv_obj_align_to(img_right, batt, LV_ALIGN_OUT_TOP_RIGHT, 8, 6);
            lv_obj_t* percent_label = lv_label_create(cont);
            lv_obj_set_style_text_color(percent_label, lv_color_white(), 0);
            lv_obj_set_style_text_font(percent_label, Font_ResouceGet(FONT_22), 0);
            lv_obj_align_to(percent_label, charge_cont, LV_ALIGN_OUT_RIGHT_BOTTOM, 0, 0);
        }
        break;
    }
}

/**
 * @brief 刷新当前显示的电池符号
 * 
 * @param p_dsc 
 */
static void refresh_disp_index(ui_power_dsc_t* p_power_dsc)
{
    static uint32_t last_tick = 0;

    if (p_power_dsc->type != POWER_TYPE_DC_WITH_BATT) {
        last_tick = lv_tick_get();    
        return ;
    }

    /* 更新电池是否存在 */
    ui_dc_batt_dsc_t* p_src_dsc = &g_ptUIStateData->title.power_dsc.dsc.dc_with_batt;
    ui_dc_batt_dsc_t* p_ui_dsc = &p_power_dsc->dsc.dc_with_batt;

    for (uint8_t i = 0; i < 2; i++) 
    {
        p_ui_dsc->batt_sys[i].is_exist = p_src_dsc->batt_sys[i].is_exist;
    }
    
    if (lv_tick_elaps(last_tick) > BATT_DISP_MAX_SWITCH_TIME) {        
        last_tick = lv_tick_get();                
        
        /* 获取下一个待显示的电池 */
        bool b_exist = false;
        uint8_t cnt = 0;
        do {            
            p_power_dsc->dsc.dc_with_batt.disp_index++;
            p_power_dsc->dsc.dc_with_batt.disp_index %= 2;

            cnt += 1;
            b_exist = p_power_dsc->dsc.dc_with_batt.batt_sys[p_power_dsc->dsc.dc_with_batt.disp_index].is_exist;

            if (b_exist || cnt >= 2) 
                break;
        } while (1);
    }
}

/**
 * @brief 刷新电池显示容器
 * 
 * @param charge_cont 
 * @param p_power_dsc 
 * @param force_upd 
 */
static void refresh_charge_cont(lv_obj_t* charge_cont, ui_power_dsc_t* p_power_dsc, bool force_upd)
{
    lv_obj_t* batt = lv_obj_get_child(charge_cont, 0);
    lv_obj_t* batt_capacity = lv_obj_get_child(batt, 0);
    lv_obj_t* img_lightning = lv_obj_get_child(batt, -1);
    lv_obj_t* img_left = lv_obj_get_child(charge_cont, 1);
    lv_obj_t* img_right = lv_obj_get_child(charge_cont, 2);
    batt_dsc_t* p_ui_dsc = &p_power_dsc->dsc.dc_with_batt.batt_sys[p_power_dsc->dsc.dc_with_batt.disp_index];
    lv_obj_t* img_direction;

    if (!force_upd)
        return ;

    if (lv_obj_has_flag(img_lightning, LV_OBJ_FLAG_HIDDEN)) {
        lv_obj_clear_flag(img_lightning, LV_OBJ_FLAG_HIDDEN);        
    }
    
    /* 刷新显示左右电池显示图标. */
    if (p_power_dsc->dsc.dc_with_batt.disp_index == BATT_LEFT) {
        img_direction = img_left;
        lv_obj_add_flag(img_right, LV_OBJ_FLAG_HIDDEN);      
        lv_obj_clear_flag(img_direction, LV_OBJ_FLAG_HIDDEN);  
    }
    else {
        img_direction = img_right;
        lv_obj_add_flag(img_left, LV_OBJ_FLAG_HIDDEN);       
        lv_obj_clear_flag(img_direction, LV_OBJ_FLAG_HIDDEN);         
    }

    /* 刷新电池显示状态 */
    switch (p_ui_dsc->state)
    {
        case BATT_STATE_IDLE:
        {
            if (p_power_dsc->dsc.dc_with_batt.b_anim_start) {
                lv_anim_del(batt_capacity, NULL);
                p_power_dsc->dsc.dc_with_batt.b_anim_start = false;
            }
        }
        break;
        case BATT_STATE_CHARGING:
        {            
            lv_obj_set_style_bg_img_recolor(batt, lv_color_make(0, 255, 0), 0);
            lv_obj_set_style_bg_color(batt_capacity, lv_color_make(0, 255, 0), 0);
            lv_obj_set_style_img_recolor(img_direction, lv_color_make(0, 255, 0), 0);
            lv_obj_set_style_img_recolor(img_lightning, lv_color_white(), 0);                        

            if (!p_power_dsc->dsc.dc_with_batt.b_anim_start) //开始电池动画
            {
                lv_anim_t a;
                lv_anim_init(&a);
                lv_anim_set_var(&a, batt_capacity);
                lv_anim_set_values(&a, 0, BATT_CAPACITY_MAX_HEIGHT);
                lv_anim_set_exec_cb(&a, batt_charge_anim_cb);
                lv_anim_set_time(&a, BATT_CHARGE_ANIMATION_TIME);
                lv_anim_set_repeat_count(&a, LV_ANIM_REPEAT_INFINITE);
                lv_anim_start(&a);

                p_power_dsc->dsc.dc_with_batt.b_anim_start = true;               
            }
        }
        break;
        case BATT_STATE_CHARGE_END:        
        {
            if (p_power_dsc->dsc.dc_with_batt.b_anim_start) {                
                p_power_dsc->dsc.dc_with_batt.b_anim_start = false;
                lv_anim_del(batt_capacity, NULL);
            }

            lv_coord_t height =  lv_map(p_ui_dsc->percent, 0, 100, 1, BATT_CAPACITY_MAX_HEIGHT); //BATT_CAPACITY_MAX_HEIGHT
            lv_obj_set_size(batt_capacity, BATT_CAPACITY_WIDTH, height);
            lv_obj_align(batt_capacity, LV_ALIGN_BOTTOM_MID, 0, -2);   

            lv_obj_set_style_bg_img_recolor(batt, lv_color_white(), 0);
            lv_obj_set_style_bg_color(batt_capacity, lv_color_white(), 0);
            lv_obj_set_style_img_recolor(img_direction, lv_color_white(), 0);
            lv_obj_set_style_img_recolor(img_lightning, lv_color_make(0, 255, 0), 0);              
        }
        break;
    }    
}


/**
 * @brief 供电部分显示刷新
 * 
 * @param cont 
 * @param p_power_dsc 
 * @param force_upd 
 */
static bool power_disp_refresh(lv_obj_t* cont, ui_power_dsc_t* p_power_dsc, bool force_upd)
{
    static uint8_t last_disp = 0xFF;

    switch (p_power_dsc->type) 
    {
        case POWER_TYPE_DC:break;
        case POWER_TYPE_SUPPLY_BOX:
		lv_obj_clean(cont);
		{
			LV_IMG_DECLARE(Imgleftbox);	
			LV_IMG_DECLARE(Imgrightbox);
			switch(g_ptUIStateData->box_type)
			{
				case 0:
				{	
					lv_obj_t* charge_cont = lv_obj_create(cont);
					lv_obj_t* imgright  = lv_img_create(charge_cont);     
					lv_obj_t* imgleft = lv_img_create(charge_cont);
					lv_obj_remove_style_all(charge_cont);
					lv_obj_set_size(charge_cont, 42, lv_pct(100));
					lv_img_set_src(imgleft, &Imgleftbox);
					lv_obj_align(imgleft, LV_ALIGN_LEFT_MID, 0, 0); 
					lv_img_set_src(imgright, &Imgrightbox);
					lv_obj_align(imgright, LV_ALIGN_RIGHT_MID, 0, 0); 
				}
				break;
				case 1:
				{
					lv_obj_t* imgright = lv_img_create(cont);
					lv_img_set_src(imgright, &Imgrightbox);
					lv_obj_align(imgright, LV_ALIGN_CENTER, 0, 0);  
				}
				break;
				case 2:
				{
					lv_obj_t* imgleft = lv_img_create(cont);
					lv_img_set_src(imgleft, &Imgleftbox);
					lv_obj_align(imgleft, LV_ALIGN_CENTER, 0, 0); 
				}
				break;			
			}
		}
		break;
        case POWER_TYPE_DC_WITH_BATT:
        {                
            lv_obj_t* charge_cont = lv_obj_get_child(cont, 0);
            lv_obj_t* percent_label = lv_obj_get_child(cont, -1);

            /* 刷新待显示电池的索引. */ 
            refresh_disp_index(p_power_dsc);

            /* 刷新显示电池内容信息. */
            uint8_t disp_index = p_power_dsc->dsc.dc_with_batt.disp_index;            
            ui_dc_batt_dsc_t* p_src_dsc = &g_ptUIStateData->title.power_dsc.dsc.dc_with_batt;
            ui_dc_batt_dsc_t* p_ui_dsc = &p_power_dsc->dsc.dc_with_batt;
            
            if (p_src_dsc->batt_sys[disp_index].percent != p_ui_dsc->batt_sys[disp_index].percent || \
                    p_src_dsc->batt_sys[disp_index].state != p_ui_dsc->batt_sys[disp_index].state || \
                        last_disp != disp_index || force_upd)                       
            {                
                p_ui_dsc->batt_sys[disp_index].percent = p_src_dsc->batt_sys[disp_index].percent;
                p_ui_dsc->batt_sys[disp_index].state = p_src_dsc->batt_sys[disp_index].state;
                force_upd = true;
                last_disp = disp_index;
            }

            /* 刷新充电显示. */
            refresh_charge_cont(charge_cont, p_power_dsc, force_upd);

            /* 刷新当前电量百分比 */
            if (force_upd) {
                lv_label_set_text_fmt(percent_label, "%d%%", p_ui_dsc->batt_sys[disp_index].percent);
                lv_obj_align_to(percent_label, charge_cont, LV_ALIGN_OUT_RIGHT_BOTTOM, 0, 0);    
            }       
        }
        break;
        case POWER_TYPE_BATTERY:
        {
            lv_obj_t* batt = lv_obj_get_child(cont, 0);
            lv_obj_t* label = lv_obj_get_child(cont, 1);         
            ui_batt_dsc_t* p_src_dsc = &g_ptUIStateData->title.power_dsc.dsc.batt;
            /* 刷新电池 */
            if (p_src_dsc->percent != p_power_dsc->dsc.batt.percent || force_upd) {
                p_power_dsc->dsc.batt.percent = p_src_dsc->percent;
                force_upd = true;
            }

            /* 刷新剩余时间显示 */
            if (p_power_dsc->dsc.batt.b_low_power != p_src_dsc->b_low_power ||
                    p_power_dsc->dsc.batt.remain_time_m != p_src_dsc->remain_time_m || force_upd) {
                
                p_power_dsc->dsc.batt.b_low_power = p_src_dsc->b_low_power;
                p_power_dsc->dsc.batt.remain_time_m = p_src_dsc->remain_time_m;
                
                if (p_power_dsc->dsc.batt.remain_time_m <= BATT_LOW_TIME_MINUTES) {
                    p_power_dsc->dsc.batt.b_low_power = true;
                }

                force_upd = true;
            }

            if (force_upd) {
                lv_obj_t* capacity = lv_obj_get_child(batt, 0);    
                lv_color_t icon_color = p_power_dsc->dsc.batt.b_low_power ? lv_color_make(255, 51, 51) : lv_color_make(255, 255, 255);
                lv_coord_t height =  lv_map(p_power_dsc->dsc.batt.percent, 0, 100, 1, BATT_CAPACITY_MAX_HEIGHT); //BATT_CAPACITY_MAX_HEIGHT

                /* 设置battery frame的颜色 */
                lv_obj_set_style_bg_img_recolor(batt, icon_color, 0);

                /* 设置电池容量 */
                lv_obj_set_style_bg_color(capacity, icon_color, 0);
                lv_obj_set_size(capacity, BATT_CAPACITY_WIDTH, height);
                lv_obj_align(capacity, LV_ALIGN_BOTTOM_MID, 0, -2);     

                /* 设置电池剩余时间 */
                if (p_power_dsc->dsc.batt.remain_time_m > 120)
                    lv_snprintf(sdb, sizeof(sdb), ">2H");
                else
                    lv_snprintf(sdb, sizeof(sdb), "%dH%02dM", p_power_dsc->dsc.batt.remain_time_m / 60, p_power_dsc->dsc.batt.remain_time_m % 60);

                lv_label_set_text(label, sdb);      
                lv_obj_set_style_text_color(label, icon_color, 0);                
            }
        }
        break;
        default:break;
    }

    return force_upd;
}

/**
 * @brief 将l2_screen设置为NULL
 * 
 * @param event 
 */
static void l2_scr_del_event_cb(lv_event_t* event)
{
    lv_event_code_t e = lv_event_get_code(event);
    if (e == LV_EVENT_DELETE)
    {
        screen_dsc.l2_screen = NULL;
    }
}
