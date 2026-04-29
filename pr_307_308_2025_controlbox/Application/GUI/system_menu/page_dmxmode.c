/*********************
 *      INCLUDES
 *********************/
#include "../ui_common.h"
#include "../page_widget.h"
#include "user.h"
/*********************
 *      DEFINES
 *********************/

#define DISP_PANEL_WIDTH  172
#define DISP_PANEL_HEIGHT 189

#define BTN_CONT_WIDTH      122
#define BTN_CONT_HEIGHT     189
/*********************
 *      TPEDEFS
 *********************/
typedef struct {
    lv_obj_t* dmx_addr_value_label;
    lv_obj_t* dmx_lock_img;
    lv_obj_t* scroll_cont;
    lv_obj_t* imgbtn;
    lv_obj_t* img_cont;
    lv_obj_t* dmx_protector;
    lv_obj_t* prev_focus_obj;
	uint8_t last_sub_pid;
}view_t;

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void page_dmxmode_construct(void);
static void page_dmxmode_destruct(void);
static void page_dmxmode_time_upd(uint32_t time);
static void page_dmxmode_event_cb(int event);

static const char* dmx_loss_behavior_get_str(int8_t x);
static void dmx_addr_event_cb(lv_event_t* e);
static lv_obj_t* dmx_addr_cont_create(lv_obj_t* parent, lv_coord_t w, lv_coord_t h);
static void refresh_dmx_addr_img(lv_obj_t* img);
static void refresh_dmx_addr_value_label(void);
static const char* dmx_extension_get_str(int8_t index);

static void dmx_loss_behavior_release_cb(lv_event_t* e, uint8_t index);
static void dmx_extension_release_cb(lv_event_t* e, uint8_t index);
static void dmx_profile_release_cb(lv_event_t* e, uint8_t index);
static void dmx_list_event_cb(lv_event_t* e, uint8_t index);
static void dmx_temination_event_cb(lv_event_t* e);
static void refresh_dmx_temination_img(void);
static void dmx_status_refresh_hide(void);

static lv_obj_t* title_comm_icon_cont_create(void);
static void refresh_title_comm_icon(void);
static void dmx_protector_event_cb(lv_event_t* e);
static void dmx_mode_protector(void);
static void refresh_dmx_protector(void);
static const char* dmx_smooth_get_str(int8_t index);
static void dmx_smooth_release_cb(lv_event_t* e, uint8_t index);
static void dmx_status_refresh(void);
/*********************
 *  STATIC VATIABLES
 *********************/
static lv_group_t *indev_group;
static my_list_ext_t list_ext;
static my_radiobox_ext_t radiobox_ext;
static my_radiobox_ext_t smooth_ext;
static sys_config_t* p_sys_menu_model = NULL;
static view_t local_view;
static uint32_t dmx_idle_tick_cnt = 0;
static uint32_t local_time_cnt = 0;
static lv_obj_t* img_sun;
static uint8_t s_dmx_extern_index = 0;
/*********************
 *  GLOBAL VATIABLES
 *********************/
page_t page_dmxmode = PAGE_INITIALIZER("DMX Mode",
                                PAGE_DMX_MODE, 
                                page_dmxmode_construct, 
                                page_dmxmode_destruct, 
                                page_dmxmode_time_upd,
                                page_dmxmode_event_cb,                                
                                PAGE_MENU);
/*********************
 *  GLOBAL FUNCTIONS
 *********************/
#ifdef UI_PLATFORM_PC
bool ui_dmx_temination_disp(void)
{
    return false;
}

extern bool ui_dmx_signal_is_exist(void)
{
    return true;
}
#else

bool ui_dmx_temination_disp(void)
{
    return false;
}

extern bool ui_dmx_signal_is_exist(void)
{
    return true;
}
#endif
/**********************
 *   STATIC FUNCTIONS
 **********************/

static void page_dmxmode_construct(void)
{
    indev_group = lv_group_create();
    my_indev_set_group(indev_group);
    lv_group_set_default(indev_group);
    
    lv_obj_set_style_bg_color(lv_scr_act(), lv_color_black(), 0);
    p_sys_menu_model = screen_get_act_page_data();

    uint8_t spid = screen_get_act_spid();

    dmx_idle_tick_cnt = 0;
    local_time_cnt = 0;    
    page_event_send(EVENT_DATA_PULL);
    
	ui_set_ctr_spid_index(0);
    switch(spid)
    {
       case SUB_ID_DMX_ENTRY:  
       {         
			lv_obj_clear_flag(lv_scr_act(), LV_OBJ_FLAG_SCROLLABLE);
            lv_obj_t* title = lv_label_create(lv_scr_act());            
            lv_obj_set_style_text_font(title, Font_ResouceGet(FONT_22_BOLD), 0);
            lv_obj_set_style_text_color(title, lv_color_white(), 0);
            lv_label_set_text(title, Lang_GetStringByID(STRING_ID_DMX_SETTING));
            lv_obj_align(title, LV_ALIGN_TOP_MID, 0, SYS_MENU_TITLE_TO_TOP);   
		   
            list_ext.cur_sel = screen_get_act_index();  
            list_ext.release_cb = dmx_list_event_cb;
			list_ext.max = 7;
            lv_obj_t* list = my_list_create(lv_scr_act(), lv_pct(85), lv_pct(80), &list_ext); 
			lv_obj_clear_flag(list, LV_OBJ_FLAG_SCROLLABLE);
            lv_obj_set_flex_align(list, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);                       
            lv_obj_align(list, LV_ALIGN_TOP_MID, 0, 38);
            
		    my_list_add_btn(list, Lang_GetStringByID(STRING_ID_DMX_STATUS), 10);
            my_list_add_btn(list, Lang_GetStringByID(STRING_ID_DMX_ADDRESS), 10);
            my_list_add_btn(list, Lang_GetStringByID(STRING_ID_DMX_SMOOTH), 10);
            my_list_add_btn(list, Lang_GetStringByID(STRING_ID_LOSS_BEHAVIOR), 10);
            my_list_add_btn(list, Lang_GetStringByID(STRING_ID_CONFIG), 10);
		    my_list_add_btn(list, Lang_GetStringByID(STRING_ID_DMX_EXTENSION), 10);
			my_list_add_btn(list, Lang_GetStringByID(STRING_ID_DMX_TEMINATION), 10);
			
			local_view.scroll_cont = my_list_scroll_create(lv_scr_act(), 3, 215);
            lv_obj_align_to(local_view.scroll_cont, list, LV_ALIGN_OUT_RIGHT_TOP, 21, 2);   
			
			lv_group_add_obj(indev_group, list);
			lv_group_focus_obj(list);
			lv_group_set_editing(indev_group, true);   
        }
        break;
        case SUB_ID_DMX_ADDRESS:
        {            
            if (p_sys_menu_model->dmx_addr == 0)
                p_sys_menu_model->dmx_addr = 1;

            lv_obj_t* cont = dmx_addr_cont_create(lv_scr_act(), lv_pct(90), lv_pct(39));
            lv_obj_add_event_cb(cont, dmx_addr_event_cb, LV_EVENT_ALL, NULL);
            lv_obj_center(cont);

			lv_obj_t* title = lv_label_create(lv_scr_act());            
            lv_obj_set_style_text_font(title, Font_ResouceGet(FONT_14_MEDIUM), 0);
            lv_obj_set_style_text_color(title, lv_color_white(), 0);
			lv_obj_set_width(title, 230);  /*Set smaller width to make the lines wrap*/
            lv_label_set_long_mode(title, LV_LABEL_LONG_WRAP);
            lv_obj_set_style_text_align(title, LV_TEXT_ALIGN_CENTER, 0);
            lv_label_set_text(title, Lang_GetStringByID(STRING_ID_DMX_LOCK));
            lv_obj_align(title, LV_ALIGN_BOTTOM_MID, 0, -10);  
			
            lv_group_add_obj(indev_group, cont);
            lv_group_focus_obj(cont);
            lv_group_set_editing(indev_group, true);
        }
        break;
        case SUB_ID_DMX_SMOOTH:
        {
            lv_obj_t* title = lv_label_create(lv_scr_act());
            lv_obj_set_style_text_font(title, Font_ResouceGet(FONT_22_BOLD), 0);
            lv_obj_set_style_text_color(title, lv_color_white(), 0);
            lv_label_set_text(title, Lang_GetStringByID(STRING_ID_DMX_SMOOTH));
            lv_obj_align(title, LV_ALIGN_TOP_MID, 0, SYS_MENU_TITLE_TO_TOP);

            lv_obj_t* disp_pannel = lv_obj_create(lv_scr_act());
            lv_obj_add_style(disp_pannel, &style_rect_panel, 0);
            lv_obj_clear_flag(disp_pannel, LV_OBJ_FLAG_SCROLLABLE);
            lv_obj_set_width(disp_pannel, DISP_PANEL_WIDTH);
            lv_obj_set_height(disp_pannel, DISP_PANEL_HEIGHT);
            lv_obj_align(disp_pannel, LV_ALIGN_LEFT_MID, 10, 15);

            LV_IMG_DECLARE(ImgDmxSmooth);
            img_sun = lv_img_create(disp_pannel);
            lv_img_set_src(img_sun, &ImgDmxSmooth);
            lv_obj_align(img_sun, LV_ALIGN_CENTER, 0, 0);

            lv_obj_t* btn_panel = lv_obj_create(lv_scr_act());
            lv_obj_remove_style_all(btn_panel);
            lv_obj_add_style(btn_panel, &style_rect_panel, 0);
            lv_obj_set_flex_flow(btn_panel, LV_FLEX_FLOW_COLUMN);
            lv_obj_set_style_pad_row(btn_panel, 4, 0);
            lv_obj_set_style_bg_color(btn_panel, lv_color_make(0, 0, 0), 0);
            lv_obj_set_size(btn_panel, BTN_CONT_WIDTH, BTN_CONT_HEIGHT);
            lv_obj_align_to(btn_panel, disp_pannel, LV_ALIGN_OUT_RIGHT_TOP, 8, 0);

            smooth_ext.sel = p_sys_menu_model->dmx_fade_time;
            smooth_ext.max = DMX_FADE_SEL_MAX;
            smooth_ext.items_of_page = 3;
            smooth_ext.get_name_str = dmx_smooth_get_str;
            smooth_ext.release_cb = dmx_smooth_release_cb;

            lv_obj_t* radiobox_panel = my_smooth_list_create(btn_panel, BTN_CONT_WIDTH, &smooth_ext, MY_RADIOBOX_TYPE2, LV_ALIGN_CENTER);
            lv_obj_align(radiobox_panel, LV_ALIGN_TOP_MID, 0, 25);

            lv_group_add_obj(indev_group, radiobox_panel);
            lv_group_set_wrap(indev_group, false);
            lv_group_set_editing(indev_group, true);
        }
        break;
        case SUB_ID_DMX_LOSS_BEHAVIOR:
        {
            lv_obj_t* title = lv_label_create(lv_scr_act());            
            lv_obj_set_style_text_font(title, Font_ResouceGet(FONT_22_BOLD), 0);
            lv_obj_set_style_text_color(title, lv_color_white(), 0);
            lv_label_set_text(title, Lang_GetStringByID(STRING_ID_LOSS_BEHAVIOR));
            lv_obj_align(title, LV_ALIGN_TOP_LEFT, 12, SYS_MENU_TITLE_TO_TOP);

            radiobox_ext.sel = p_sys_menu_model->dmx_loss_behavior;
            radiobox_ext.max = DMX_LOSS_BEHAVIOR_MAX;
            radiobox_ext.items_of_page = 4;            
            radiobox_ext.get_name_str = dmx_loss_behavior_get_str;
            radiobox_ext.release_cb = dmx_loss_behavior_release_cb;

            lv_obj_t* radiobox_panel = my_radiobox_list_create(lv_scr_act(), 265, &radiobox_ext, MY_RADIOBOX_TYPE2, LV_ALIGN_LEFT_MID);            
            lv_obj_align(radiobox_panel, LV_ALIGN_TOP_MID, 0, 37);
            
            lv_group_add_obj(indev_group, radiobox_panel);
            lv_group_set_wrap(indev_group, false);
            lv_group_set_editing(indev_group, true);
        }
        break;
        case SUB_ID_DMX_PROFILE:
        {
            lv_obj_t* title = lv_label_create(lv_scr_act());
            
            lv_obj_set_style_text_font(title, Font_ResouceGet(FONT_22_BOLD), 0);
            lv_obj_set_style_text_color(title, lv_color_white(), 0);            
            lv_label_set_text(title, Lang_GetStringByID(STRING_ID_CONFIG));
            lv_obj_align(title, LV_ALIGN_TOP_LEFT, 12, SYS_MENU_TITLE_TO_TOP);

            radiobox_ext.sel = p_sys_menu_model->dmx_profile_index;
            #if PROJECT_TYPE==307
            radiobox_ext.max = DMX_PROFILE_NUMS;
            #elif PROJECT_TYPE==308
            radiobox_ext.max = 6;
            #endif
            radiobox_ext.items_of_page = 4;
            radiobox_ext.get_name_str = dmx_profile_get_name;
            radiobox_ext.release_cb = dmx_profile_release_cb;
            
            lv_obj_t* radiobox_panel = my_radiobox_list_create(lv_scr_act(), 265, &radiobox_ext, MY_RADIOBOX_TYPE1, LV_ALIGN_LEFT_MID);
            //lv_obj_align_to(radiobox_panel, title, LV_ALIGN_OUT_BOTTOM_MID, 0, 2);
            lv_obj_align(radiobox_panel, LV_ALIGN_TOP_MID, 0, 37);
            lv_group_add_obj(indev_group, radiobox_panel);

            local_view.scroll_cont = my_list_scroll_create(lv_scr_act(), 3, 200);
            lv_obj_align_to(local_view.scroll_cont, radiobox_panel, LV_ALIGN_OUT_RIGHT_TOP, 21, 2);        
            lv_group_focus_obj(radiobox_panel);
            lv_group_set_editing(indev_group, true);
            lv_group_set_wrap(indev_group, false);            
        }
        break;
		case SUB_ID_DMX_EXTENSION:
		{
			lv_obj_t* title = lv_label_create(lv_scr_act());            
            lv_obj_set_style_text_font(title, Font_ResouceGet(FONT_22_BOLD), 0);
            lv_obj_set_style_text_color(title, lv_color_white(), 0);
            lv_label_set_text(title, Lang_GetStringByID(STRING_ID_DMX_EXTENSION));
            lv_obj_align(title, LV_ALIGN_TOP_MID, 0, SYS_MENU_TITLE_TO_TOP);

			if(s_dmx_extern_index > 1)
				s_dmx_extern_index = 0;
            radiobox_ext.sel = s_dmx_extern_index;
            radiobox_ext.max = 2;
            radiobox_ext.items_of_page = 2;            
            radiobox_ext.get_name_str = dmx_extension_get_str;
            radiobox_ext.release_cb = dmx_extension_release_cb;

            lv_obj_t* radiobox_panel = my_radiobox1_list_create(lv_scr_act(), 265, &radiobox_ext, MY_RADIOBOX_TYPE2, LV_ALIGN_LEFT_MID, p_sys_menu_model->dmx_extension_index);            
            lv_obj_align(radiobox_panel, LV_ALIGN_CENTER, 0, 0);
            
			lv_obj_t* title_low = lv_label_create(lv_scr_act());            
            lv_obj_set_style_text_font(title_low, Font_ResouceGet(FONT_14_MEDIUM), 0);
            lv_obj_set_style_text_color(title_low, lv_color_white(), 0);
			lv_obj_set_width(title_low, 300);  /*Set smaller width to make the lines wrap*/
            lv_label_set_long_mode(title_low, LV_LABEL_LONG_WRAP);
            lv_obj_set_style_text_align(title_low, LV_TEXT_ALIGN_CENTER, 0);
            lv_label_set_text(title_low, Lang_GetStringByID(STRING_ID_DMX_EXTENSION_TEXT));
            lv_obj_align(title_low, LV_ALIGN_BOTTOM_MID, 0, -10);  
			
            lv_group_add_obj(indev_group, radiobox_panel);
            lv_group_set_wrap(indev_group, false);
            lv_group_set_editing(indev_group, true);
		}
		break;
        case SUB_ID_DMX_TEMINATION:
        {
            //标题栏
            lv_obj_t* title = lv_label_create(lv_scr_act());
            
            lv_obj_set_style_text_font(title, Font_ResouceGet(FONT_22_BOLD), 0);
            lv_obj_set_style_text_color(title, lv_color_white(), 0);            
            lv_label_set_text(title, Lang_GetStringByID(STRING_ID_DMX_TEMINATION));
            lv_obj_align(title, LV_ALIGN_TOP_MID, 0, SYS_MENU_TITLE_TO_TOP);      

            //display img.
            LV_IMG_DECLARE(ImgDMXTeminationIcon_B)            
            lv_obj_t* img2 = lv_img_create(lv_scr_act());
            lv_img_set_src(img2, &ImgDMXTeminationIcon_B);
            lv_obj_align(img2, LV_ALIGN_CENTER, 0, -15);

            local_view.imgbtn = lv_img_create(lv_scr_act());
            lv_obj_add_event_cb(local_view.imgbtn, dmx_temination_event_cb, LV_EVENT_ALL, NULL);
            refresh_dmx_temination_img();

            lv_group_add_obj(indev_group, local_view.imgbtn);
            lv_group_set_wrap(indev_group, false);
            lv_group_set_editing(indev_group, true);            
        }
        break;
		case SUB_ID_DMX_STATUS:
			ui_set_dmx_state(0);
			dmx_mode_protector();
			refresh_dmx_protector();
			lv_group_add_obj(indev_group, local_view.dmx_protector);
            lv_group_set_wrap(indev_group, false);
            lv_group_set_editing(indev_group, true); 
		break;
		default:break;
    }
	
    uint8_t id = PAGE_DMX_MODE;
    uint8_t sub_id = spid;
    data_center_write_config_data_no_event(SYS_CONFIG_LOCAL_UI, &id);
    data_center_write_config_data_no_event(SYS_CONFIG_LOCAL_SUB_UI, &sub_id);
}

static void page_dmxmode_destruct(void)
{
	uint8_t sub_pid = 0;
	
	dmx_idle_tick_cnt = 0;
    lv_group_del(indev_group);    
    lv_obj_clean(lv_scr_act());

    indev_group = NULL;
	sub_pid = local_view.last_sub_pid;
    memset(&local_view, 0, sizeof(local_view));
	local_view.last_sub_pid = sub_pid;
}

static void page_dmxmode_time_upd(uint32_t time)
{
    uint8_t spid = screen_get_act_spid();
    local_time_cnt += time;
    if (local_time_cnt % 1000 == 0) {
		
        if (ui_dmx_signal_is_exist())        
        {
			if(dmx_idle_tick_cnt < 20)
            dmx_idle_tick_cnt += 1;
        }
        else  
        {
            dmx_idle_tick_cnt = 0;
        }
    }
	
	if(SUB_ID_DMX_STATUS == spid)
	{
		refresh_title_comm_icon();
		dmx_status_refresh();
	}
	else
		dmx_status_refresh_hide();
	
    switch (spid)
    {
        case SUB_ID_DMX_ADDRESS:
        {
            if (local_time_cnt % 500 == 0)
            {
                uint16_t dmx_addr = p_sys_menu_model->dmx_addr;
                
                data_center_read_config_data(SYS_CONFIG_DMX_ADDR, &dmx_addr);
                if (dmx_addr != p_sys_menu_model->dmx_addr)
                {
                    p_sys_menu_model->dmx_addr = dmx_addr;
                    refresh_dmx_addr_value_label();       

                    dmx_idle_tick_cnt = 0; 
                }
            }
        }
        break;
        case SUB_ID_DMX_PROFILE:
        {
            lv_obj_t* radiobox_panel = lv_group_get_focused(indev_group);
            my_radiobox_ext_t* ext = lv_obj_get_user_data(radiobox_panel);
            uint8_t dmx_profile_index;

            data_center_read_config_data(SYS_CONFIG_DMX_PROFILE, &dmx_profile_index);

            /* 如果当前Profile索引更新，那么就更新profile的默认值 */
            if (p_sys_menu_model->dmx_profile_index != dmx_profile_index)
            {
			
                p_sys_menu_model->dmx_profile_index = dmx_profile_index;   
				page_event_send(EVENT_DATA_WRITE);
                if (ext)             
                {
                    lv_event_send(radiobox_panel, LV_EVENT_REFRESH, &p_sys_menu_model->dmx_profile_index);                  
                }                
                dmx_idle_tick_cnt = 0;
            }
        }
        break;
        default:break;
    }

	if (dmx_idle_tick_cnt >= 10)
    {
		dmx_idle_tick_cnt = 0;
		if(SUB_ID_DMX_STATUS != screen_get_act_spid())
		{
			local_view.last_sub_pid = screen_get_act_spid();
			screen_load_sub_page(SUB_ID_DMX_STATUS, 0);
		}
    }
}

static void page_dmxmode_event_cb(int event)
{
    switch(event)
    {
        case EVENT_DATA_PULL:
        {
			if(p_sys_menu_model == NULL)
			{
				return;
			}
            data_center_read_config_data(SYS_CONFIG_DMX_ADDR, &p_sys_menu_model->dmx_addr);
            data_center_read_config_data(SYS_CONFIG_DMX_LOCKED, &p_sys_menu_model->dmx_locked);
            data_center_read_config_data(SYS_CONFIG_DMX_FADETIME, &p_sys_menu_model->dmx_fade_time);
            data_center_read_config_data(SYS_CONFIG_DMX_LOSS_BEHAVIOR, &p_sys_menu_model->dmx_loss_behavior);
            data_center_read_config_data(SYS_CONFIG_DMX_PROFILE, &p_sys_menu_model->dmx_profile_index);
			data_center_read_config_data(SYS_CONFIG_DMX_EXTENSION, &p_sys_menu_model->dmx_extension_index);
			data_center_read_config_data(SYS_CONFIG_DMX_TEMINATION, &p_sys_menu_model->dmx_temination_on);
        }
        break;
        case EVENT_DATA_WRITE:
        {
			if(p_sys_menu_model == NULL)
			{
				return;
			}
            data_center_write_config_data_no_event(SYS_CONFIG_DMX_ADDR, &p_sys_menu_model->dmx_addr);
            data_center_write_config_data_no_event(SYS_CONFIG_DMX_LOCKED, &p_sys_menu_model->dmx_locked);
            data_center_write_config_data_no_event(SYS_CONFIG_DMX_FADETIME, &p_sys_menu_model->dmx_fade_time);
            data_center_write_config_data_no_event(SYS_CONFIG_DMX_LOSS_BEHAVIOR, &p_sys_menu_model->dmx_loss_behavior);
            data_center_write_config_data_no_event(SYS_CONFIG_DMX_PROFILE, &p_sys_menu_model->dmx_profile_index);
			data_center_write_config_data_no_event(SYS_CONFIG_DMX_EXTENSION, &p_sys_menu_model->dmx_extension_index);
			data_center_write_config_data_no_event(SYS_CONFIG_DMX_TEMINATION, &p_sys_menu_model->dmx_temination_on);
        }
        break;
    }
}

static void anim_pos_x_cb(void* var, int32_t v)
{
    if (v > 255)
    {
        v = 510 - v;
        lv_obj_set_style_img_recolor_opa(var, v, 0);
    }
    else
    {
        lv_obj_set_style_img_recolor_opa(var, v, 0);
    }
        
}

void my_smooth_list_event_cb(lv_event_t* e)
{
    lv_obj_t* radiobox_cont = lv_event_get_target(e);
    lv_event_code_t event = lv_event_get_code(e);
    const lv_indev_t* indev = lv_indev_get_act();
    my_radiobox_ext_t* ext = lv_obj_get_user_data(radiobox_cont);
    lv_anim_t a;

    shortcut_keys_event_handler(e);

    if (event == LV_EVENT_FOCUSED)  /* 首次被聚焦 */
    {
        lv_obj_t* obj = lv_obj_get_child(radiobox_cont, ext->sel);
        lv_obj_add_state(obj, LV_STATE_FOCUSED);
        lv_event_send(obj, LV_EVENT_FOCUSED, NULL);
        lv_obj_scroll_to_view(obj, LV_ANIM_OFF);
    }
    else if (event == LV_EVENT_KEY) /* 用户按键操作 */
    {
        uint32_t key = *(uint32_t*)lv_event_get_param(e);
        int8_t index = ext->sel;
        bool b_upd = false;

        if (key == LV_ENCODER_KEY_CW)
        {
            index = ext->sel + 1;
            if (index < ext->max)
            {
                b_upd = true;
            }
        }
        else if (key == LV_ENCODER_KEY_CC)
        {
            index = ext->sel - 1;
            if (index >= 0)
            {
                b_upd = true;
            }
        }

        if (b_upd)
        {
            lv_obj_t* prev_obj = lv_obj_get_child(radiobox_cont, ext->sel);
            lv_obj_clear_state(prev_obj, LV_STATE_FOCUSED);
            lv_event_send(prev_obj, LV_EVENT_DEFOCUSED, NULL);

            ext->sel = index;

            lv_obj_t* next_obj = lv_obj_get_child(radiobox_cont, ext->sel);
            lv_obj_add_state(next_obj, LV_STATE_FOCUSED);
            lv_event_send(next_obj, LV_EVENT_FOCUSED, NULL);
            lv_obj_scroll_to_view(next_obj, LV_ANIM_OFF);

            if (index == DMX_FADE_CLOSE)
            {
                lv_anim_del_all();
                lv_obj_set_style_img_recolor_opa(img_sun, LV_OPA_100, 0);
                lv_obj_set_style_img_recolor(img_sun, lv_color_make(0x4d, 0x4d, 0x4d), 0);
            }
            else if (index == DMX_FADE_SMOOTH)
            {
                lv_anim_del_all();
                lv_obj_set_style_img_recolor(img_sun, lv_color_make(0x00, 0x00, 0x00), 0);
                lv_anim_init(&a);
                lv_anim_set_var(&a, img_sun);
                lv_anim_set_values(&a, 0, 510);
                lv_anim_set_exec_cb(&a, anim_pos_x_cb);
                lv_anim_set_repeat_count(&a, LV_ANIM_REPEAT_INFINITE);
                lv_anim_set_time(&a, UI_DMX_SMOOTH_FADE_TIME * 2);
                lv_anim_set_playback_time(&a, 0);
                lv_anim_start(&a);
            }
            else if (index == DMX_FADE_SUPER_SMOOTH)
            {
                lv_anim_del_all();
                lv_obj_set_style_img_recolor(img_sun, lv_color_make(0x00, 0x00, 0x00), 0);
                lv_anim_init(&a);
                lv_anim_set_var(&a, img_sun);
                lv_anim_set_values(&a, 0, 510);
                lv_anim_set_exec_cb(&a, anim_pos_x_cb);
                lv_anim_set_repeat_count(&a, LV_ANIM_REPEAT_INFINITE);
                lv_anim_set_time(&a, UI_DMX_SUPER_SMOOTH_FADE_TIME * 2);
                lv_anim_set_playback_time(&a, 0);
                lv_anim_start(&a);

            }
			if(ui_get_master_mode())
			{
				extern void gui_data_sync_event_gen(void);		
				gui_data_sync_event_gen();
			}
        }
    }
    else if (event == LV_EVENT_REFRESH) /* 用户主动刷新 */
    {
        void* param = lv_event_get_param(e);
        uint8_t index = 0;

        if (param == NULL)
            return;

        index = *(uint8_t*)lv_event_get_param(e);
        if (index >= ext->max)
            return;

        /* 获取上一次默认控件值 */
        lv_obj_t* obj = lv_obj_get_child(radiobox_cont, ext->default_val);
        refresh_radiobox_img(obj, false);

        /* 刷新图标 */
        obj = lv_obj_get_child(radiobox_cont, index);
        refresh_radiobox_img(obj, true);

        /* 更新控件默认值 */
        ext->default_val = index;
    }

    if (ext->release_cb)    /* 用户事件函数 */
    {
        ext->release_cb(e, ext->sel);
    }
}

static void dmx_protector_event_cb(lv_event_t* e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t* obj = lv_event_get_target(e);

	if (code == LV_EVENT_KEY || code == LV_EVENT_RELEASED)
    {
		uint32_t key = *(uint32_t*)lv_event_get_param(e);

		if(PAGE_DMX_MODE != screen_get_act_pid())
		{
			user_turn_to_page(PAGE_DMX_MODE, 0, false);
		}
		else
		{
			user_turn_to_page(PAGE_DMX_MODE, local_view.last_sub_pid, false);
		}
        dmx_idle_tick_cnt = 0;
    }
}

static void dmx_mode_protector(void)
{
    local_view.dmx_protector = lv_obj_create(lv_scr_act());
    lv_obj_remove_style_all(local_view.dmx_protector);
    lv_obj_set_style_bg_color(local_view.dmx_protector, lv_color_black(), 0);
    lv_obj_set_style_bg_opa(local_view.dmx_protector, LV_OPA_100, 0);
    lv_obj_set_size(local_view.dmx_protector, lv_pct(100), lv_pct(100));
    lv_obj_add_event_cb(local_view.dmx_protector, dmx_protector_event_cb, LV_EVENT_ALL, NULL);

    lv_obj_t* dmx_addr_panel = lv_obj_create(local_view.dmx_protector);
    lv_obj_remove_style_all(dmx_addr_panel);    
    lv_obj_add_style(dmx_addr_panel, &style_rect_panel, 0);
    lv_obj_set_style_bg_opa(dmx_addr_panel, LV_OPA_100, 0);
    lv_obj_set_size(dmx_addr_panel, 256, 60);
    lv_obj_set_pos(dmx_addr_panel, 28, 28);
    
    lv_obj_t* label = lv_label_create(dmx_addr_panel);
    lv_obj_set_style_text_color(label, lv_color_make(255, 255, 255), 0);
    lv_obj_set_style_text_font(label, Font_ResouceGet(FONT_22_BOLD), 0);
    lv_obj_set_pos(label, 12, 17);

    lv_obj_t* dmx_profile_panel = lv_obj_create(local_view.dmx_protector);
    lv_obj_remove_style_all(dmx_profile_panel);
    lv_obj_add_style(dmx_profile_panel, &style_rect_panel, 0);
    lv_obj_set_style_bg_opa(dmx_profile_panel, LV_OPA_100, 0);
    lv_obj_set_size(dmx_profile_panel, 256, 120);
    lv_obj_align_to(dmx_profile_panel, dmx_addr_panel, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 5);

    lv_obj_t* title = lv_label_create(dmx_profile_panel);
    lv_obj_set_style_text_color(title, lv_color_make(255, 255, 255), 0);
    lv_obj_set_style_text_font(title, Font_ResouceGet(FONT_22_BOLD), 0);
    lv_obj_set_pos(label, 12, 19);

    lv_obj_t* content = lv_label_create(dmx_profile_panel);
    lv_obj_set_style_text_color(content, lv_color_make(255, 255, 255), 0);
    lv_obj_set_style_text_font(content, Font_ResouceGet(FONT_22), 0);
    lv_obj_set_size(content, 240, LV_SIZE_CONTENT);
    lv_obj_align_to(content, title, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 5);
	
	lv_obj_t* channel = lv_label_create(dmx_profile_panel);
    lv_obj_set_style_text_color(channel, lv_color_make(255, 255, 255), 0);
    lv_obj_set_style_text_font(channel, Font_ResouceGet(FONT_22_BOLD), 0);
    lv_obj_set_size(channel, 80, LV_SIZE_CONTENT);
    lv_obj_align_to(channel, title, LV_ALIGN_OUT_RIGHT_MID, 5, 0);
	
	lv_obj_t* next_addr = lv_label_create(dmx_profile_panel);
    lv_obj_set_style_text_color(content, lv_color_make(255, 255, 255), 0);
    lv_obj_set_style_text_font(content, Font_ResouceGet(FONT_22), 0);
    lv_obj_set_size(content, 240, LV_SIZE_CONTENT);
    lv_obj_align_to(content, title, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 5);
	
	lv_obj_t* next_label = lv_label_create(dmx_addr_panel);
    lv_obj_set_style_text_color(next_label, lv_color_make(255, 255, 255), 0);
    lv_obj_set_style_text_font(next_label, Font_ResouceGet(FONT_13_BOLD), 0);
    lv_obj_align_to(next_label, dmx_addr_panel, LV_ALIGN_RIGHT_MID, 0, 0);
}

static void dmx_status_refresh(void)
{
	uint16_t next_addr = 0;
	
	if(local_view.dmx_protector == NULL)
		return;
	lv_obj_t* dmx_addr_panel = lv_obj_get_child(local_view.dmx_protector, 0);
    lv_obj_t* label = lv_obj_get_child(dmx_addr_panel, 0);
	lv_obj_t* next_label = lv_obj_get_child(dmx_addr_panel, 1);
    lv_obj_t* dmx_profile_panel = lv_obj_get_child(local_view.dmx_protector, 1);
    lv_obj_t* title = lv_obj_get_child(dmx_profile_panel, 0);
    lv_obj_t* content = lv_obj_get_child(dmx_profile_panel, 1);
    lv_obj_t* channel = lv_obj_get_child(dmx_profile_panel, 2);
	
    /* 刷新地址 */
    data_center_read_config_data(SYS_CONFIG_DMX_ADDR, &p_sys_menu_model->dmx_addr);
    lv_label_set_text_fmt(label, "DMX : %03d", p_sys_menu_model->dmx_addr);
    lv_obj_align(label, LV_ALIGN_LEFT_MID, 12, 0);

	/* 刷新下一个地址 */
	next_addr = p_sys_menu_model->dmx_addr+gui_get_dmx_profile_addr();
	if(next_addr > 512)
	{
		if(g_tUIAllData.sys_menu_model.lang == 0)
			lv_label_set_text(next_label, "Next Add : Full");
		else 
			lv_label_set_text(next_label, "下一个地址 ：占满");
	}
	else
	{
		if(g_tUIAllData.sys_menu_model.lang == 0)
			lv_label_set_text_fmt(next_label, "Next Add : %03d", next_addr);
		else 
			lv_label_set_text_fmt(next_label, "下一个地址 : %03d", next_addr);
	}
	lv_obj_align(next_label, LV_ALIGN_RIGHT_MID, -10, 3);
	
    /* 刷新DMX Profile */
    data_center_read_config_data(SYS_CONFIG_DMX_PROFILE, &p_sys_menu_model->dmx_profile_index);
    lv_label_set_text_fmt(title, "Profile %d:", p_sys_menu_model->dmx_profile_index + 1);
    lv_obj_align(title, LV_ALIGN_TOP_LEFT, 12, 19);
    lv_label_set_text(content, dmx_profile_get_name(p_sys_menu_model->dmx_profile_index));
    lv_obj_align_to(content, title, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 5);
	
    lv_label_set_text_fmt(channel, "%dCH", gui_get_dmx_profile_addr());
    lv_obj_align_to(channel, title, LV_ALIGN_OUT_RIGHT_MID, 5, 0);
}

static void refresh_dmx_protector(void)
{
	uint16_t next_addr = 0;
	
    if (local_view.dmx_protector == NULL)
        return;
	
	local_view.img_cont = title_comm_icon_cont_create();
	lv_obj_align(local_view.img_cont, LV_ALIGN_TOP_RIGHT, -12, SYS_MENU_TITLE_TO_TOP);
	refresh_title_comm_icon();
	
    lv_obj_t* dmx_addr_panel = lv_obj_get_child(local_view.dmx_protector, 0);
    lv_obj_t* label = lv_obj_get_child(dmx_addr_panel, 0);
	lv_obj_t* next_label = lv_obj_get_child(dmx_addr_panel, 1);
    lv_obj_t* dmx_profile_panel = lv_obj_get_child(local_view.dmx_protector, 1);
    lv_obj_t* title = lv_obj_get_child(dmx_profile_panel, 0);
    lv_obj_t* content = lv_obj_get_child(dmx_profile_panel, 1);
    lv_obj_t* channel = lv_obj_get_child(dmx_profile_panel, 2);
	
    /* 刷新地址 */
    data_center_read_config_data(SYS_CONFIG_DMX_ADDR, &p_sys_menu_model->dmx_addr);
    lv_label_set_text_fmt(label, "DMX : %03d", p_sys_menu_model->dmx_addr);
    lv_obj_align(label, LV_ALIGN_LEFT_MID, 12, 0);

	/* 刷新下一个地址 */
	next_addr = p_sys_menu_model->dmx_addr+gui_get_dmx_profile_addr();
	if(next_addr > 512)
	{
		if(g_tUIAllData.sys_menu_model.lang == 0)
			lv_label_set_text(next_label, "Next Add : Full");
		else 
			lv_label_set_text(next_label, "下一个地址 ：占满");
	}
	else
	{
		if(g_tUIAllData.sys_menu_model.lang == 0)
			lv_label_set_text_fmt(next_label, "Next Add : %03d", next_addr);
		else 
			lv_label_set_text_fmt(next_label, "下一个地址 : %03d", next_addr);
	}
	lv_obj_align(next_label, LV_ALIGN_RIGHT_MID, -10, 3);
	
    /* 刷新DMX Profile */
    data_center_read_config_data(SYS_CONFIG_DMX_PROFILE, &p_sys_menu_model->dmx_profile_index);
    lv_label_set_text_fmt(title, "Profile %d:", p_sys_menu_model->dmx_profile_index + 1);
    lv_obj_align(title, LV_ALIGN_TOP_LEFT, 12, 19);
    lv_label_set_text(content, dmx_profile_get_name(p_sys_menu_model->dmx_profile_index));
    lv_obj_align_to(content, title, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 5);
	
    lv_label_set_text_fmt(channel, "%dCH", gui_get_dmx_profile_addr());
    lv_obj_align_to(channel, title, LV_ALIGN_OUT_RIGHT_MID, 5, 0);
}

static lv_obj_t* dmx_addr_cont_create(lv_obj_t* parent, lv_coord_t w, lv_coord_t h)
{
    
    lv_obj_t* dmx_addr_cont = lv_obj_create(lv_scr_act());
    lv_obj_add_style(dmx_addr_cont, &style_common_btn, 0);
    lv_obj_set_style_text_font(dmx_addr_cont, Font_ResouceGet(FONT_48), 0);
    lv_obj_set_style_radius(dmx_addr_cont, 5, 0);
    lv_obj_set_style_clip_corner(dmx_addr_cont, true, 0);
    lv_obj_set_size(dmx_addr_cont, w, h);

    lv_obj_t* dmx_label = lv_label_create(dmx_addr_cont);
    lv_label_set_text(dmx_label, "DMX:");
    lv_obj_align(dmx_label, LV_ALIGN_LEFT_MID, 3, 0);

    local_view.dmx_addr_value_label = lv_label_create(dmx_addr_cont);
    lv_obj_set_style_text_color(local_view.dmx_addr_value_label, RED_THEME_COLOR, 0);
    refresh_dmx_addr_value_label();

    lv_obj_t* img_cont = lv_obj_create(dmx_addr_cont);
    lv_obj_add_style(img_cont, &style_rect_panel, 0);
    lv_obj_set_style_bg_color(img_cont, lv_color_make(77, 77, 77), 0);
    lv_obj_set_size(img_cont, lv_pct(18), lv_pct(100));
    lv_obj_align(img_cont, LV_ALIGN_RIGHT_MID, 0, 0);

    lv_obj_t* img = lv_img_create(img_cont);
    lv_obj_set_style_img_recolor(img, lv_color_white(), 0);
    lv_obj_set_style_img_recolor_opa(img, LV_OPA_100, 0);

    local_view.dmx_lock_img = img;
    refresh_dmx_addr_img(img);

    return dmx_addr_cont;
}

static const char* dmx_loss_behavior_get_str(int8_t index)
{
    const char* str = NULL;
    switch(index)
    {
        case DMX_LOSS_BEHAVIOR_BO:  str = Lang_GetStringByID(STRING_ID_BLACK_OUT); break;
        case DMX_LOSS_BEHAVIOR_FTB: str = Lang_GetStringByID(STRING_ID_FADE_TO_BLACK); break;
        case DMX_LOSS_BEHAVIOR_HLS: str = Lang_GetStringByID(STRING_ID_HOLD_LAST_SETTING); break;
        case DMX_LOSS_BEHAVIOR_HFO: str = Lang_GetStringByID(STRING_ID_HOLD_2MIN_FADE_OUT); break;
    }
    return str;
}

static const char* dmx_extension_get_str(int8_t index)
{
    const char* str = NULL;
    switch(index)
    {
        case DMX_EXTENSION_ACC:      str = Lang_GetStringByID(STRING_ID_DMX_MOTORIZED_ACC); break;
        case DMX_EXTENSION_FUNCTION: str = Lang_GetStringByID(STRING_ID_DMX_FUNCTION); break;
    }
    return str;
}

static const char* dmx_smooth_get_str(int8_t index)
{
    const char* str = NULL;
    switch (index)
    {
    case DMX_FADE_CLOSE:  str = Lang_GetStringByID(STRING_ID_DMX_SMOOTH_CLOSE); break;
    case DMX_FADE_SMOOTH: str = Lang_GetStringByID(STRING_ID_DMX_SMOOTH_SMOOTH); break;
    case DMX_FADE_SUPER_SMOOTH: str = Lang_GetStringByID(STRING_ID_DMX_SMOOTH_SUPER_SMOOTH); break;
    default:break;
    }
    return str;
}

static void refresh_dmx_addr_img(lv_obj_t* img)
{
    LV_IMG_DECLARE(ImgLock)
    LV_IMG_DECLARE(ImgUnlock)
    if (p_sys_menu_model->dmx_locked)
    {
        lv_img_set_src(img, &ImgLock);
    }
    else
    {
        lv_img_set_src(img, &ImgUnlock);
    }
    lv_obj_center(img);
}

static void refresh_dmx_addr_value_label(void)
{
    char tmp[10];
    lv_snprintf(tmp, sizeof(tmp), "%03d", p_sys_menu_model->dmx_addr);
    lv_label_set_text(local_view.dmx_addr_value_label, tmp);
    lv_obj_align(local_view.dmx_addr_value_label, LV_ALIGN_RIGHT_MID, -53, 0);
}

static void dmx_addr_event_cb(lv_event_t* e)
{
    lv_event_code_t event = lv_event_get_code(e);

    if (!p_sys_menu_model->dmx_locked)
        shortcut_keys_event_handler(e);

    if (event == LV_EVENT_KEY)
    {
        uint32_t key = *(uint32_t*)lv_event_get_param(e);

        if (key == LV_KEY_RIGHT &&  (!p_sys_menu_model->dmx_locked))
        {
            int16_t tmp = p_sys_menu_model->dmx_addr + 1 * common_intensity_get_step(enc_get_step_index());
            
            if (tmp > 512)
            {
                tmp = 1;
            }

            p_sys_menu_model->dmx_addr = tmp;      
            data_center_write_config_data(SYS_CONFIG_DMX_ADDR, &p_sys_menu_model->dmx_addr);
            refresh_dmx_addr_value_label();
        }
        else if (key == LV_KEY_LEFT &&  (!p_sys_menu_model->dmx_locked))
        {
            int16_t tmp = p_sys_menu_model->dmx_addr - 1 * comm_enc_get_step(enc_get_step_index());
            
            if (tmp < 1)
            {
                tmp = 512;
            }

            p_sys_menu_model->dmx_addr = tmp;            
            data_center_write_config_data(SYS_CONFIG_DMX_ADDR, &p_sys_menu_model->dmx_addr);
            refresh_dmx_addr_value_label();
        }
        else if (key == LV_KEY_BACKSPACE && (!p_sys_menu_model->dmx_locked))
        {            
            if (screen_load_prev_sub_page(0, 0))
            {
                screen_load_sub_page(SUB_ID_DMX_ENTRY, 1);
            }
        }
		if(ui_get_master_mode())
		{
			extern void gui_data_sync_event_gen(void);		
			gui_data_sync_event_gen();
		}
        dmx_idle_tick_cnt = 0;
    }
    else if (event == LV_EVENT_LONG_PRESSED)
    {
        p_sys_menu_model->dmx_locked = !p_sys_menu_model->dmx_locked;
        data_center_write_config_data_no_event(SYS_CONFIG_DMX_LOCKED, &p_sys_menu_model->dmx_locked);
        if (local_view.dmx_lock_img != NULL)
        {
            refresh_dmx_addr_img(local_view.dmx_lock_img);
        }        
    }
}

static void dmx_profile_release_cb(lv_event_t* e, uint8_t index)
{
    lv_event_code_t event = lv_event_get_code(e);
    lv_obj_t* obj = lv_event_get_target(e);
    my_radiobox_ext_t* ext = lv_obj_get_user_data(obj);
    
    if (event == LV_EVENT_RELEASED)
    {
        p_sys_menu_model->dmx_profile_index = index;
		if(ui_get_master_mode())
		{
			extern void gui_data_sync_event_gen(void);		
			gui_data_sync_event_gen();
		}
        data_center_write_config_data(SYS_CONFIG_DMX_PROFILE, &p_sys_menu_model->dmx_profile_index);
        screen_load_prev_sub_page(0, 0);
    }
    else if (event == LV_EVENT_FOCUSED)
    {
        my_list_scroll_refresh(local_view.scroll_cont, lv_map(ext->sel, 0, ext->max, 0, 100));
    }
    else if (event == LV_EVENT_KEY)
    {
        uint32_t key = *(uint32_t *)lv_event_get_param(e);
        if (LV_ENCODER_KEY_CC == key) //up
        {            
            my_list_scroll_refresh(local_view.scroll_cont,lv_map(ext->sel, 0, ext->max, 0, 100));
        }
        else if (LV_ENCODER_KEY_CW == key) //down
        {   
            my_list_scroll_refresh(local_view.scroll_cont, lv_map(ext->sel, 0, ext->max, 0, 100));
        }
        else if (key == LV_KEY_BACKSPACE)
        {
             screen_load_prev_sub_page(0, 0);           
        }        
        dmx_idle_tick_cnt = 0;
    }
}

static void dmx_loss_behavior_release_cb(lv_event_t* e, uint8_t index)
{
    lv_event_code_t event = lv_event_get_code(e);
    lv_obj_t* obj = lv_event_get_target(e);

    if (event == LV_EVENT_RELEASED)
    {
        p_sys_menu_model->dmx_loss_behavior = index;
		if(ui_get_master_mode())
		{
			extern void gui_data_sync_event_gen(void);		
			gui_data_sync_event_gen();
		}
        data_center_write_config_data(SYS_CONFIG_DMX_LOSS_BEHAVIOR, &p_sys_menu_model->dmx_loss_behavior);
        screen_load_prev_sub_page(0, 0);
    }
    else if (event == LV_EVENT_KEY)
    {        
        uint32_t key = *(uint32_t *)lv_event_get_param(e);

        if (LV_KEY_BACKSPACE == key)
        {
            screen_load_prev_sub_page(0, 0);
        }   
        dmx_idle_tick_cnt = 0;
    }        
}

static void dmx_extension_release_cb(lv_event_t* e, uint8_t index)
{
    lv_event_code_t event = lv_event_get_code(e);
    lv_obj_t* obj = lv_event_get_target(e);

    if (event == LV_EVENT_RELEASED)
    {
		switch(index)
		{
			case 0:
				if((p_sys_menu_model->dmx_extension_index&0x01) == 1)
				{
					p_sys_menu_model->dmx_extension_index &= ~(1<<0);
				}
				else
				{
					p_sys_menu_model->dmx_extension_index |= (1<<0);
				}
			break;
			case 1:
				if((p_sys_menu_model->dmx_extension_index&0x02) == 0x02)
				{
					p_sys_menu_model->dmx_extension_index &= ~(1<<1);
				}
				else
				{
					p_sys_menu_model->dmx_extension_index |= (1<<1);
				}
			break;
			default:break;
		}
		if(ui_get_master_mode())
		{
			extern void gui_data_sync_event_gen(void);		
			gui_data_sync_event_gen();
		}
        data_center_write_config_data(SYS_CONFIG_DMX_EXTENSION, &p_sys_menu_model->dmx_extension_index);
        screen_load_prev_sub_page(0, 0);
    }
    else if (event == LV_EVENT_KEY)
    {        
        uint32_t key = *(uint32_t *)lv_event_get_param(e);

		s_dmx_extern_index = index;
        if (LV_KEY_BACKSPACE == key)
        {
            screen_load_prev_sub_page(0, 0);
        }   
        dmx_idle_tick_cnt = 0;
    }        
}

static void dmx_smooth_release_cb(lv_event_t* e, uint8_t index)
{
    lv_event_code_t event = lv_event_get_code(e);
    lv_obj_t* obj = lv_event_get_target(e);

    if (event == LV_EVENT_RELEASED)
    {
        p_sys_menu_model->dmx_fade_time = index;
        //screen_load_sub_page(SUB_ID_DMX_ENTRY, 0);
        screen_load_prev_sub_page(0, 0);
        page_event_send(EVENT_DATA_WRITE);
    }
    else if (event == LV_EVENT_KEY)
    {
        uint32_t key = *(uint32_t*)lv_event_get_param(e);

        if (LV_KEY_BACKSPACE == key)
        {
            screen_load_prev_sub_page(0, 0);
        }
        dmx_idle_tick_cnt = 0;
    }
}

static void dmx_list_event_cb(lv_event_t* e, uint8_t index)
{
    lv_event_code_t event = lv_event_get_code(e);
	lv_obj_t* obj = lv_event_get_target(e);
	my_list_ext_t* ext = lv_obj_get_user_data(obj);
	
    shortcut_keys_event_handler(e);

    if (event == LV_EVENT_RELEASED)
    {
        screen_load_sub_page_with_stack(index+1, index);
		if(index == 0)
			local_view.last_sub_pid = index;
    }
	else if (event == LV_EVENT_FOCUSED)
    {
        my_list_scroll_refresh(local_view.scroll_cont, lv_map(ext->cur_sel, 0, ext->max, 0, 100));
    }
    else if (event == LV_EVENT_KEY)
    {        
        uint32_t key = *(uint32_t *)lv_event_get_param(e);

		if (LV_ENCODER_KEY_CC == key) //up
        {            
            my_list_scroll_refresh(local_view.scroll_cont,lv_map(ext->cur_sel, 0, ext->max, 0, 100));
        }
        else if (LV_ENCODER_KEY_CW == key) //down
        {   
            my_list_scroll_refresh(local_view.scroll_cont, lv_map(ext->cur_sel, 0, ext->max, 0, 100));
        }
        if (LV_KEY_BACKSPACE == key)
        {  
			if(dmx_idle_tick_cnt < 20)
			{
				user_turn_to_page(PAGE_CONTROL_SYSTEM, 0, false);
			}
			else
			{
				extern void user_enter_dmx(void);
				
				user_enter_dmx();
			}
//            screen_turn_prev_page();
        }   
        dmx_idle_tick_cnt = 0;
    }    
}

static lv_obj_t* title_comm_icon_cont_create(void)
{
    LV_IMG_DECLARE(ImgLan)
    LV_IMG_DECLARE(ImgBle)
    LV_IMG_DECLARE(ImgWifi2)
    
	
    lv_obj_t* img_cont = lv_obj_create(lv_scr_act());
    lv_obj_remove_style_all(img_cont);
    lv_obj_set_size(img_cont, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_set_flex_flow(img_cont, LV_FLEX_FLOW_ROW);
    lv_obj_set_style_pad_gap(img_cont, 7, 0);
    lv_obj_set_flex_align(img_cont, LV_FLEX_ALIGN_CENTER,LV_FLEX_ALIGN_CENTER,LV_FLEX_ALIGN_CENTER);
    
	lv_obj_t* status = lv_label_create(img_cont);   
	lv_obj_set_style_text_font(status, Font_ResouceGet(FONT_14), 0);
	lv_obj_set_style_text_color(status, lv_color_white(), 0);
	lv_label_set_text(status, Lang_GetStringByID(STRING_ID_CRMX_NO_CONNECT));
	
    lv_obj_t* crmx = lv_img_create(img_cont);   
    lv_img_set_src(crmx, &ImgWifi2);
	
	lv_obj_t* lan = lv_img_create(img_cont);
    lv_obj_set_style_img_recolor_opa(lan, LV_OPA_100, 0);
    lv_obj_set_style_img_recolor(lan, lv_color_white(), 0);
    lv_img_set_src(lan, &ImgLan);

    lv_obj_t* ble = lv_img_create(img_cont);
    lv_obj_set_style_img_recolor_opa(ble, LV_OPA_100, 0);
    lv_obj_set_style_img_recolor(ble, lv_color_white(), 0);    
    lv_img_set_src(ble, &ImgBle);
	
	lv_obj_t* power_cont = lv_obj_create(img_cont);  
	lv_obj_remove_style_all(power_cont);
    lv_obj_set_size(power_cont, LV_SIZE_CONTENT, lv_pct(100));
	
    lv_obj_add_flag(lan, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(ble, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(crmx, LV_OBJ_FLAG_HIDDEN);
	lv_obj_add_flag(status, LV_OBJ_FLAG_HIDDEN);
	lv_obj_clear_flag(power_cont, LV_OBJ_FLAG_HIDDEN);
	
    return img_cont;
}

static void dmx_power_disp_refresh(lv_obj_t* cont, uint8_t mode)
{
//    static uint8_t last_disp = 0xFF;

    switch (mode) 
    {
        case POWER_TYPE_DC:
		{
			lv_obj_clean(cont);
            lv_obj_t* img = lv_img_create(cont);
			if((gui_get_indoor_flag() >= 1))
			{
				lv_img_set_src(img, title_get_indoor_power_img());   
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
        break;
        case POWER_TYPE_BATTERY:
        break;
        default:break;
    }

}

extern ui_title_t title_model;

static void refresh_title_comm_icon(void)
{
	static uint8_t dmx_icon_count;
	
	uint8_t curre_mode = 0;
    struct sys_info_eth eth;
	
    /* 标题栏 */
    data_center_read_config_data(SYS_CONFIG_BLE_CTRL, &curre_mode);
    ui_set_ble_state(curre_mode);
    data_center_read_config_data(SYS_CONFIG_CRMX_CTRL, &curre_mode);
    ui_set_crmx_state(curre_mode);
    data_center_read_sys_info(SYS_INFO_ETH, &eth);
    ui_set_lan_state(eth.pluged);
	
    lv_obj_t* lan = lv_obj_get_child(local_view.img_cont, 2);
    lv_obj_t* ble = lv_obj_get_child(local_view.img_cont, 3);
    lv_obj_t* label = lv_obj_get_child(local_view.img_cont, 0);
	lv_obj_t* crmx = lv_obj_get_child(local_view.img_cont, 1);
	lv_obj_t* power_cont = lv_obj_get_child(local_view.img_cont, 4);
	
	if ((lan == NULL) || (ble == NULL) || (crmx == NULL) || (label== NULL) || power_cont == NULL)
        return;
	
	dmx_power_disp_refresh(power_cont, ui_get_power_type());
	
    if (g_ptUIStateData->lan_state)
	{
        lv_obj_clear_flag(lan, LV_OBJ_FLAG_HIDDEN);
	}
    else  
	{
        lv_obj_add_flag(lan, LV_OBJ_FLAG_HIDDEN);
	}

    if (p_sys_menu_model->ble_on)
	{
        lv_obj_clear_flag(ble, LV_OBJ_FLAG_HIDDEN);
	}
    else  
	{
        lv_obj_add_flag(ble, LV_OBJ_FLAG_HIDDEN);
	}
	
    if (p_sys_menu_model->crmx_on)
    {
        lv_img_set_src(crmx, title_get_comm_img(COM_TYPE_WIFI));
        if(crmx_is_paired() == 0)
		{
			lv_obj_add_flag(crmx, LV_OBJ_FLAG_HIDDEN);
			lv_obj_add_flag(label, LV_OBJ_FLAG_HIDDEN);
		}
		else if(crmx_is_paired() == 1)
		{
			lv_obj_clear_flag(crmx, LV_OBJ_FLAG_HIDDEN);
			
			if(dmx_icon_count == 1)
			{
				dmx_icon_count = 0;
				lv_obj_clear_flag(label, LV_OBJ_FLAG_HIDDEN);
				lv_label_set_text(label, Lang_GetStringByID(STRING_ID_CRMX_NO_CONNECT));
			}
			else
			{
				dmx_icon_count = 1;
				lv_obj_add_flag(label, LV_OBJ_FLAG_HIDDEN);
			}
		}
		else if(crmx_is_paired() == 2)
		{
			lv_obj_clear_flag(crmx, LV_OBJ_FLAG_HIDDEN);
			if(dmx_icon_count++ >= 1)
			{
				dmx_icon_count = 0;
				lv_obj_clear_flag(label, LV_OBJ_FLAG_HIDDEN);
				lv_label_set_text(label, Lang_GetStringByID(STRING_ID_CRMX_NO_DATA));
			}
			else
			{
				lv_obj_add_flag(label, LV_OBJ_FLAG_HIDDEN);
			}
		}
		else if(crmx_is_paired() == 3)
		{
			lv_obj_clear_flag(crmx, LV_OBJ_FLAG_HIDDEN);
			lv_obj_add_flag(label, LV_OBJ_FLAG_HIDDEN);
		}
    }
    else  
    {
        lv_obj_add_flag(crmx, LV_OBJ_FLAG_HIDDEN);
		lv_obj_add_flag(label, LV_OBJ_FLAG_HIDDEN);
    }
	
	if(p_sys_menu_model->crmx_on == 0 && p_sys_menu_model->ble_on == 0)
	{
		lv_obj_clear_flag(ble, LV_OBJ_FLAG_HIDDEN);
		lv_obj_set_style_img_recolor_opa(ble, LV_OPA_100, 0);
		lv_obj_set_style_img_recolor(ble, lv_color_black(), 0);  
	}
	else 
	{
		lv_obj_set_style_img_recolor_opa(ble, LV_OPA_100, 0);
		lv_obj_set_style_img_recolor(ble, lv_color_white(), 0); 
	}
}

static void dmx_status_refresh_hide(void)
{
	if(local_view.img_cont == NULL)
		return;
	
	lv_obj_t* lan = lv_obj_get_child(local_view.img_cont, 2);
    lv_obj_t* ble = lv_obj_get_child(local_view.img_cont, 3);
    lv_obj_t* label = lv_obj_get_child(local_view.img_cont, 0);
	lv_obj_t* crmx = lv_obj_get_child(local_view.img_cont, 1);
	
	if(lan == NULL || ble == NULL || crmx == NULL || label == NULL)
		return;
		
	lv_obj_add_flag(lan, LV_OBJ_FLAG_HIDDEN);
	lv_obj_add_flag(ble, LV_OBJ_FLAG_HIDDEN);
	lv_obj_add_flag(crmx, LV_OBJ_FLAG_HIDDEN);
	lv_obj_add_flag(label, LV_OBJ_FLAG_HIDDEN);
}

static void refresh_dmx_temination_img(void)
{
    LV_IMG_DECLARE(ImgStudioOn)
    LV_IMG_DECLARE(ImgStudioOff)
    if (p_sys_menu_model->dmx_temination_on == DMX_TEMINATION_OFF)
    {
        lv_img_set_src(local_view.imgbtn, &ImgStudioOff);                 
    }
    else
    {
        lv_img_set_src(local_view.imgbtn, &ImgStudioOn);
    }
	if(ui_get_master_mode())
	{
		extern void gui_data_sync_event_gen(void);		
		gui_data_sync_event_gen();
	}
    lv_obj_align(local_view.imgbtn, LV_ALIGN_BOTTOM_MID, 0, -32);  
}

static void dmx_temination_event_cb(lv_event_t* e)
{
    lv_event_code_t event = lv_event_get_code(e);

    shortcut_keys_event_handler(e);

    if (event == LV_EVENT_RELEASED)
    {
        p_sys_menu_model->dmx_temination_on = !p_sys_menu_model->dmx_temination_on;
        data_center_write_config_data(SYS_CONFIG_DMX_TEMINATION, &p_sys_menu_model->dmx_temination_on);
        refresh_dmx_temination_img();        
    }    
    else if (event == LV_EVENT_KEY)  
    {
        uint32_t key = *(uint32_t *)lv_event_get_param(e);

        dmx_idle_tick_cnt = 0;
        if (key == LV_KEY_BACKSPACE)
        {
            screen_load_prev_sub_page(0, 0);
        }

    }    
}
