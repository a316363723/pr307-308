/*********************
 *      INCLUDES
 *********************/
#include "ui_common.h"
#include "page_widget.h"


/*********************
 *      DEFINES
 *********************/
#define INT_CIRCLE_COLOR                         COMMON_EDITED_COLOR 
#define LOADING_ANIM_INTEVAL_TIME                500
#define LOADING_ANIM_TIME                        (1 * 7000)
#define DEFAULT_INTENSITY                        500
#define ARC_INDICATOR_COLOR                      lv_color_make(255, 255, 255)
#define ARC_INDICATOR_BG_COLOR                   lv_color_make(106, 103, 103)
#define LOADING_IMG_COLOR                        lv_color_make(255, 255, 255)
/*********************
 *      TPEDEFS
 *********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void page_customfx_construct(void);
static void page_customfx_destruct(void);
static const char* custom_fx_name_get_str(int8_t x);
static void custom_fx_choose_list_user_event(lv_event_t* e, uint8_t index);
static void custom_fx_list_user_event(lv_event_t* e, uint8_t index);
static void circle_panel_event_cb(lv_event_t* e);
static void refresh_int_arc(lv_obj_t* arc);
static void page_customfx_time_upd_cb(uint32_t ms);
static void page_customfx_event_cb(int event);
static void loading_img_event_cb(lv_event_t* e);  
static void loading_panel_anim_cb(void* a, int32_t value);
static void arc_panel_event_cb(lv_event_t* e);

static void  custom_fx_set_intensity(uint8_t type, uint8_t index, int16_t intensity);
/*********************
 *  STATIC VATIABLES
 *********************/
static lv_group_t *indev_group;
static struct db_local_custom_fx* p_local_cfx_arg = &g_tUIAllData.local_fx;
static struct db_custom_fx* p_local_cfx_ctrl_arg = &g_tUIAllData.cfx_ctrl;
static my_list_ext_t list_ext;
static my_radiobox_ext_t radiobox_ext;
static lv_obj_t* scroll_cont = NULL;
static lv_obj_t* int_arc = NULL;
static int32_t disp_time;
static int32_t life_period;
/*********************
 *  GLOBAL VATIABLES
 *********************/
page_t page_customfx = PAGE_INITIALIZER("Custom FX",
                                PAGE_CUSTOM_FX, 
                                page_customfx_construct, 
                                page_customfx_destruct, 
                                page_customfx_time_upd_cb,
                                page_customfx_event_cb,                                
                                PAGE_MENU);
/*********************
 *  GLOBAL FUNCTIONS
 *********************/
extern uint8_t custom_fx_get_type(void);
extern uint8_t custom_fx_get_index(void);

#ifdef UI_PLATFORM_PC
const char* ex_custom_fx_get_name(uint8_t type, uint8_t index)
{
    return NULL;
}

bool ex_custom_fx_is_empty(uint8_t type, uint8_t index)
{
    return false;
}

#else 
static struct  sys_info_cfx_name cfx_name_info;
static char s_cfx_name[10];

const char* ex_custom_fx_get_name(uint8_t type, uint8_t index)
{
	memset(s_cfx_name, 0, sizeof(s_cfx_name));
    data_center_read_sys_info(SYS_INFO_CFX_NAME, &cfx_name_info);
    memcpy(s_cfx_name, cfx_name_info.cfx_name[type].name[index], sizeof(s_cfx_name));
	
    return s_cfx_name;
}

bool ex_custom_fx_is_empty(uint8_t type, uint8_t index)
{
    data_center_read_sys_info(SYS_INFO_CFX_NAME, &cfx_name_info);
    if(strncmp(cfx_name_info.cfx_name[type].name[index], "NO FX", strlen("NO FX")) == 0)
        return true;
    else
        return false;
}
#endif

uint16_t custom_fx_get_intensity(uint8_t type, uint8_t index)
{
    return p_local_cfx_ctrl_arg->lightness;
}

uint8_t custom_fx_get_index(void)
{
    return p_local_cfx_arg->bank[p_local_cfx_arg->type];
}

uint8_t custom_fx_get_type(void)
{
    return p_local_cfx_arg->type;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/    

//static void local_data_pull(uint8_t spid)
//{
//    (void)spid;
//    data_center_read_light_data(LIGHT_MODE_LOCAL_CFX, p_local_cfx_arg);    
//}

//static void local_data_write(uint8_t spid)
//{
//    (void)spid;
//    data_center_write_light_data(LIGHT_MODE_LOCAL_CFX, p_local_cfx_arg);
//}

static void page_customfx_construct(void)
{
    uint8_t spid = screen_get_act_spid();    
    indev_group = lv_group_create();
    
    life_period = -1;
    disp_time = 0;

    my_indev_set_group(indev_group);
    lv_group_set_default(indev_group);
    lv_obj_set_style_bg_color(lv_scr_act(), lv_color_black(), 0);

    switch(spid)
    {
        case SUB_ID_CUSTOM_FX_LIST:
        {
            lv_obj_t* title = lv_label_create(lv_scr_act());
            
            lv_obj_set_style_text_font(title, Font_ResouceGet(FONT_22_BOLD), 0);
            lv_obj_set_style_text_color(title, lv_color_white(), 0);
            lv_label_set_text(title, Lang_GetStringByID(STRING_ID_CUSTOM_FX));
            lv_obj_align(title, LV_ALIGN_TOP_MID, 0, SYS_MENU_TITLE_TO_TOP); 

			if(p_local_cfx_arg->type == 0)
				list_ext.cur_sel = p_local_cfx_arg->type;  
			else
				list_ext.cur_sel = 1;  
            list_ext.release_cb = custom_fx_list_user_event;
            
            lv_obj_t* list = my_list_create(lv_scr_act(), lv_pct(85), lv_pct(70), &list_ext);            
            lv_obj_set_flex_align(list, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
            lv_obj_align(list, LV_ALIGN_TOP_MID, 0, 38);
         
            my_list_add_btn(list, ui_get_custom_fx_name(CUSTOM_FX_TYPE_PIKER), 10);
            my_list_add_btn(list, ui_get_custom_fx_name(CUSTOM_FX_TYPE_MUSIC), 10);
#if UI_TOUCHBAR_FX            
            my_list_add_btn(list, ui_get_custom_fx_name(CUSTOM_FX_TYPE_TOUCHBAR), 30);
#endif            
            lv_group_add_obj(indev_group, list);
            lv_group_focus_obj(list);      
            lv_group_set_editing(indev_group, true);
        }
        break;
        case SUB_ID_CUSTOM_FX_CHOOSE:
        {
            lv_obj_t* title = lv_label_create(lv_scr_act());
            
            lv_obj_set_style_text_font(title, Font_ResouceGet(FONT_22_BOLD), 0);
            lv_obj_set_style_text_color(title, lv_color_white(), 0);
            lv_label_set_text(title, ui_get_custom_fx_name(p_local_cfx_arg->type));
            lv_obj_align(title, LV_ALIGN_TOP_MID, 0, SYS_MENU_TITLE_TO_TOP);   

            radiobox_ext.sel = p_local_cfx_arg->bank[p_local_cfx_arg->type];            
            radiobox_ext.max = 10;
            radiobox_ext.items_of_page = 3;            
            radiobox_ext.get_name_str = custom_fx_name_get_str;
            radiobox_ext.release_cb = custom_fx_choose_list_user_event;

            lv_obj_t* radiobox_panel = my_long_list_create(lv_scr_act(), 265, &radiobox_ext);            
            lv_obj_align(radiobox_panel, LV_ALIGN_TOP_MID, 0, 50);

            scroll_cont = my_list_scroll_create(lv_scr_act(), 3, 140);                        
            lv_obj_align_to(scroll_cont, radiobox_panel, LV_ALIGN_OUT_RIGHT_TOP, 21, 4);
            
            lv_group_add_obj(indev_group, radiobox_panel);
            lv_group_focus_obj(radiobox_panel);      
            lv_group_set_editing(indev_group, true);                
        }
        break;
        case SUB_ID_CUSTOM_FX_LOADING:
        {            
			p_local_cfx_ctrl_arg->lightness = 500; //默认亮度
            static int8_t vanish_idx = 0;
            disp_time = 0;
            life_period = LOADING_ANIM_TIME*2;        
            vanish_idx = 0;

            lv_obj_t* panel = lv_obj_create(lv_scr_act());
            lv_obj_remove_style_all(panel);
            lv_obj_set_style_bg_color(panel, lv_color_black(), 0);
            lv_obj_set_style_bg_opa(panel, LV_OPA_100, 0);
            lv_obj_set_user_data(panel, &vanish_idx);
            lv_obj_add_event_cb(panel, loading_img_event_cb, LV_EVENT_ALL, NULL);
            lv_obj_center(panel);
            lv_anim_t a;
            
            lv_anim_init(&a);
            lv_anim_set_var(&a, panel);
            lv_anim_set_values(&a, 0, 7);
            lv_anim_set_exec_cb(&a, loading_panel_anim_cb);
            lv_anim_set_time(&a, 500);
            lv_anim_set_repeat_count(&a, LV_ANIM_REPEAT_INFINITE);
            lv_anim_start(&a);

            lv_group_add_obj(indev_group, panel);
            lv_group_focus_obj(panel);      
            lv_group_set_editing(indev_group, true);                 
        }   
        break;     
        case SUB_ID_FX_INTENSITY:
        {   
//			page_event_send(EVENT_DATA_PULL);
            lv_obj_t* title = lv_label_create(lv_scr_act());        
            lv_obj_set_style_text_font(title, Font_ResouceGet(FONT_22_BOLD), 0);
            lv_obj_set_style_text_color(title, lv_color_white(), 0);
            lv_label_set_text(title, custom_fx_name_get_str(p_local_cfx_arg->bank[p_local_cfx_arg->type]));
            lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 10); 

            lv_obj_t* circle_panel = lv_obj_create(lv_scr_act());
            lv_obj_remove_style_all(circle_panel);
            lv_obj_set_style_radius(circle_panel, 5, 0);
            lv_obj_set_style_bg_opa(circle_panel, LV_OPA_100, 0);
            lv_obj_set_style_bg_color(circle_panel, RED_THEME_COLOR, 0);
            lv_obj_set_size(circle_panel, 125, 158);                                    
            lv_obj_align(circle_panel, LV_ALIGN_TOP_MID, 0, 54);

            lv_obj_t* arc_pannel = lv_obj_create(circle_panel);
            lv_obj_remove_style_all(arc_pannel);
            lv_obj_set_style_bg_opa(arc_pannel, LV_OPA_100, 0);
            lv_obj_set_style_bg_color(arc_pannel, RED_THEME_COLOR, 0);
            lv_obj_set_size(arc_pannel, 106, 106);            
            lv_obj_add_event_cb(arc_pannel, arc_panel_event_cb, LV_EVENT_DRAW_POST_END, NULL);
            lv_obj_align(arc_pannel, LV_ALIGN_TOP_MID, 0, 16);

            int_arc = lv_arc_create(arc_pannel);
            lv_obj_set_style_arc_rounded(int_arc, 0, 0);
            lv_obj_set_style_arc_rounded(int_arc, 0, LV_PART_INDICATOR);        
            lv_obj_set_style_arc_width(int_arc, 7, 0);
            lv_obj_set_style_arc_width(int_arc, 7, LV_PART_INDICATOR);
            lv_arc_set_rotation(int_arc, 270);
            lv_arc_set_bg_angles(int_arc,0, 360);
            lv_arc_set_angles(int_arc, 0, 360);
            lv_arc_set_mode(int_arc, LV_ARC_MODE_NORMAL);
            lv_obj_set_size(int_arc, 100, 100);

            lv_obj_set_style_pad_all(int_arc, 0, 0);
            lv_obj_set_style_border_width(int_arc, 0, LV_PART_MAIN);
            lv_obj_set_style_border_width(int_arc, 0, LV_PART_INDICATOR);
            lv_obj_set_style_pad_all(int_arc, 0, LV_PART_KNOB);
            lv_obj_set_style_bg_color(int_arc,  ARC_INDICATOR_COLOR, LV_PART_INDICATOR);
            lv_obj_set_style_arc_color(int_arc, ARC_INDICATOR_COLOR, LV_PART_INDICATOR);
            lv_obj_set_style_bg_color(int_arc, ARC_INDICATOR_BG_COLOR, LV_PART_MAIN);
            lv_obj_set_style_arc_color(int_arc, ARC_INDICATOR_BG_COLOR, LV_PART_MAIN);
            
            lv_obj_set_style_radius(int_arc, 0, LV_PART_MAIN);
            lv_obj_remove_style(int_arc, NULL, LV_PART_KNOB);            
            lv_obj_center(int_arc);
            
            lv_obj_t* label = lv_label_create(int_arc);            
            lv_obj_set_style_text_font(label, Font_ResouceGet(FONT_24_MEDIUM), 0);
            lv_obj_set_style_text_letter_space(label, 0, 0);
            lv_obj_set_style_text_color(label, lv_color_white(), 0);
			
			if (p_local_cfx_ctrl_arg->lightness >= 1000)
			{
				lv_snprintf(sdb, sizeof(sdb), "%d", 100);
			}
			else
			{
				lv_snprintf(sdb, sizeof(sdb), "%d.%d", p_local_cfx_ctrl_arg->lightness / 10, p_local_cfx_ctrl_arg->lightness % 10);
			}
            lv_label_set_text(label, sdb);
            lv_obj_center(label);
            
            lv_obj_t* symbol = lv_label_create(int_arc);            
            lv_obj_set_style_text_font(symbol, Font_ResouceGet(FONT_14), 0);
            lv_obj_set_style_text_color(symbol, lv_color_white(), 0);
            lv_obj_set_style_text_letter_space(symbol, 0, 0);
            lv_label_set_text(symbol, "%");
            lv_obj_align(symbol, LV_ALIGN_CENTER, 25, 4);

            label = lv_label_create(circle_panel);
            lv_obj_set_style_text_font(label, Font_ResouceGet(FONT_18), 0);
            lv_obj_set_style_text_color(label, lv_color_white(), 0);
            lv_label_set_text(label, Lang_GetStringByID(STRING_ID_INTENSITY));
            lv_obj_align(label, LV_ALIGN_BOTTOM_MID, 0, -6);            
            refresh_int_arc(int_arc);

            lv_obj_add_event_cb(circle_panel, circle_panel_event_cb, LV_EVENT_ALL, NULL);
            lv_group_add_obj(indev_group, circle_panel);
            lv_group_focus_obj(circle_panel);      
            lv_group_set_editing(indev_group, true);     

            uint8_t id = PAGE_CUSTOM_FX;
            uint8_t sub_id = SUB_ID_FX_INTENSITY;
            data_center_write_config_data_no_event(SYS_CONFIG_LOCAL_UI, &id);
            data_center_write_config_data_no_event(SYS_CONFIG_LOCAL_SUB_UI, &sub_id);
			page_event_send(EVENT_DATA_WRITE);
        }
        break;
    }
}

static void page_customfx_destruct(void)
{
    lv_group_focus_freeze(indev_group, true);
    lv_group_remove_all_objs(indev_group);
    lv_group_del(indev_group);
    indev_group = NULL;
    lv_obj_clean(lv_scr_act());
}

static void page_customfx_time_upd_cb(uint32_t ms)
{
    uint8_t spid = screen_get_act_spid();
	page_invalid_type_t event = get_hs_custom_fx_event();
    switch (spid)
    {
        case SUB_ID_CUSTOM_FX_LOADING:
        {
            if (life_period != -1 && !l2_screen_is_exist())
            {
                disp_time += 200;
                if (disp_time >= life_period || 1 == ui_get_cfx_read_bank_state())
                {
                    screen_load_sub_page(SUB_ID_FX_INTENSITY, 0);        
                }
            }
        }
        break;
        default: break;
    }
	
//	switch(event)
//	{
//		case EVENT_DATA_HS_WINDOW:
//        {
//            struct sys_info_lamp lamp_info;
//            data_center_read_sys_info(SYS_INFO_LAMP, &lamp_info);
//            
//            if(spid == SUB_ID_CUSTOM_FX_LIST)
//            {
//                if(lamp_info.exist == 1)
//                {
//                    data_center_write_light_data(LIGHT_MODE_LOCAL_CFX, p_local_cfx_arg);
//                }
//                else if(lamp_info.exist == 0)
//                {
//                    screen_load_sub_page_with_stack(SUB_ID_CUSTOM_FX_CHOOSE, p_local_cfx_arg->type); 
//                }
//            }
//            else if(spid == SUB_ID_CUSTOM_FX_CHOOSE)
//            {
//                if(lamp_info.exist == 1 && p_local_cfx_ctrl_arg->ctrl != 0)
//                {
//                    p_local_cfx_ctrl_arg->bank = p_local_cfx_arg->bank[p_local_cfx_arg->type];
//                    p_local_cfx_ctrl_arg->type = p_local_cfx_arg->type;
//                    p_local_cfx_ctrl_arg->loop = 1;
//                    p_local_cfx_ctrl_arg->chaos = 0;
//                    p_local_cfx_ctrl_arg->sequence = 0;
//                    p_local_cfx_ctrl_arg->speed = 1;
//                    p_local_cfx_ctrl_arg->lightness = 500;
//                    data_center_write_light_data(LIGHT_MODE_SIDUS_CFX, p_local_cfx_ctrl_arg);
//                    ui_set_cfx_read_bank_state(0);
//                    screen_load_sub_page_with_stack(SUB_ID_CUSTOM_FX_LOADING, p_local_cfx_ctrl_arg->bank);  
//                }
//                else if(lamp_info.exist == 0 || p_local_cfx_ctrl_arg->ctrl == 0)
//                {
//                    p_local_cfx_ctrl_arg->lightness = 500;
//                    data_center_write_light_data(LIGHT_MODE_SIDUS_CFX, p_local_cfx_ctrl_arg);
//                    screen_load_sub_page_with_stack(SUB_ID_FX_INTENSITY, p_local_cfx_arg->bank[p_local_cfx_arg->type]); 
//                }
//            }
//            else if(spid == SUB_ID_FX_INTENSITY)
//            {
//                if(!ex_custom_fx_is_empty(p_local_cfx_arg->type, p_local_cfx_arg->bank[p_local_cfx_arg->type]))
//                    p_local_cfx_ctrl_arg->ctrl = 2;
//                else
//                    p_local_cfx_ctrl_arg->ctrl = 0;
//                p_local_cfx_ctrl_arg->bank = p_local_cfx_arg->bank[p_local_cfx_arg->type];
//                p_local_cfx_ctrl_arg->type = p_local_cfx_arg->type;
//                p_local_cfx_ctrl_arg->loop = 1;
//                p_local_cfx_ctrl_arg->chaos = 0;
//                p_local_cfx_ctrl_arg->sequence = 0;
//                p_local_cfx_ctrl_arg->speed = 1;
//                data_center_write_light_data(LIGHT_MODE_SIDUS_CFX, p_local_cfx_ctrl_arg);
//            }
//        }
//		life_period = LOADING_ANIM_TIME/10;        
//		set_hs_custom_fx_event(EVENT_DATA_WRITE);
//		break;
//		
//	}
}

static void page_customfx_event_cb(int event)
{
    uint8_t spid = screen_get_act_spid();

    switch (event)
    {
        case EVENT_DATA_PULL:
        {
			data_center_read_light_data(LIGHT_MODE_SIDUS_CFX, p_local_cfx_ctrl_arg);
        }
        break;
        case EVENT_DATA_WRITE:
        {
            struct sys_info_lamp lamp_info;
            data_center_read_sys_info(SYS_INFO_LAMP, &lamp_info);
            
            if(spid == SUB_ID_CUSTOM_FX_LIST)
            {
                if(lamp_info.exist == 1)
                {
                    data_center_write_light_data(LIGHT_MODE_LOCAL_CFX, p_local_cfx_arg);
                }
                else if(lamp_info.exist == 0)
                {
                    screen_load_sub_page_with_stack(SUB_ID_CUSTOM_FX_CHOOSE, p_local_cfx_arg->type); 
                }
            }
            else if(spid == SUB_ID_CUSTOM_FX_CHOOSE)
            {
                if(lamp_info.exist == 1 && p_local_cfx_ctrl_arg->ctrl != 0)
                {
                    p_local_cfx_ctrl_arg->bank = p_local_cfx_arg->bank[p_local_cfx_arg->type];
                    p_local_cfx_ctrl_arg->type = p_local_cfx_arg->type;
                    p_local_cfx_ctrl_arg->loop = 1;
                    p_local_cfx_ctrl_arg->chaos = 0;
                    p_local_cfx_ctrl_arg->sequence = 0;
                    p_local_cfx_ctrl_arg->speed = 1;
                    p_local_cfx_ctrl_arg->lightness = 500;
                    data_center_write_light_data(LIGHT_MODE_SIDUS_CFX, p_local_cfx_ctrl_arg);
                    ui_set_cfx_read_bank_state(0);
                    screen_load_sub_page_with_stack(SUB_ID_CUSTOM_FX_LOADING, p_local_cfx_ctrl_arg->bank);  
                }
                else if(lamp_info.exist == 0 || p_local_cfx_ctrl_arg->ctrl == 0)
                {
                    p_local_cfx_ctrl_arg->lightness = 500;
                    data_center_write_light_data(LIGHT_MODE_SIDUS_CFX, p_local_cfx_ctrl_arg);
                    screen_load_sub_page_with_stack(SUB_ID_FX_INTENSITY, p_local_cfx_arg->bank[p_local_cfx_arg->type]); 
                }
            }
            else if(spid == SUB_ID_FX_INTENSITY)
            {
                if(!ex_custom_fx_is_empty(p_local_cfx_arg->type, p_local_cfx_arg->bank[p_local_cfx_arg->type]))
                    p_local_cfx_ctrl_arg->ctrl = 2;
                else
                    p_local_cfx_ctrl_arg->ctrl = 0;
                p_local_cfx_ctrl_arg->bank = p_local_cfx_arg->bank[p_local_cfx_arg->type];
                p_local_cfx_ctrl_arg->type = p_local_cfx_arg->type;
                p_local_cfx_ctrl_arg->loop = 1;
                p_local_cfx_ctrl_arg->chaos = 0;
                p_local_cfx_ctrl_arg->sequence = 0;
                p_local_cfx_ctrl_arg->speed = 1;
                data_center_write_light_data(LIGHT_MODE_SIDUS_CFX, p_local_cfx_ctrl_arg);
            }
        }
        break;
		case EVENT_DATA_HS_WINDOW:
		{
			p_local_cfx_ctrl_arg->ctrl = 0;
			break;
		}
        default:break;
    }
}




static const char* custom_fx_name_get_str(int8_t x)
{
	struct sys_info_lamp   info_read_body = {0};
			
	data_center_read_sys_info(SYS_INFO_LAMP, &info_read_body);	
    const char* name = ex_custom_fx_get_name(p_local_cfx_arg->type, x);
	if(info_read_body.exist == 1)
        lv_snprintf(sdb, sizeof(sdb), "PP%d: %s", x + 1, name);
	else
		lv_snprintf(sdb, sizeof(sdb), "PP%d: NO FX", x + 1);
    return sdb;
}

static void custom_fx_list_user_event(lv_event_t* e, uint8_t index)
{
    lv_event_code_t event = lv_event_get_code(e);
    lv_obj_t* obj = lv_event_get_target(e); 
	uint8_t spid = screen_get_act_spid();
    shortcut_keys_event_handler(e);

    if (event == LV_EVENT_RELEASED)
    {
		if(index == 0)
			p_local_cfx_arg->type = index;  
		else
			p_local_cfx_arg->type = 2;  
	
			page_event_send(EVENT_DATA_WRITE);
//        screen_load_sub_page_with_stack(SUB_ID_CUSTOM_FX_CHOOSE, index);    
       // 
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

static void loading_panel_anim_cb(void* a, int32_t value)
{
    int8_t* p_idx = lv_obj_get_user_data(a);
    if ((a != NULL) && (p_idx != NULL)) 
    {        
        *p_idx = value;
        lv_obj_invalidate(a);
    }    
}

static void loading_img_event_cb(lv_event_t* e)
{
    lv_event_code_t event = lv_event_get_code(e);
    lv_obj_t* obj = lv_event_get_target(e); 
    int8_t* p_vanish_idx = lv_obj_get_user_data(obj);

//    shortcut_keys_event_handler(e);

    if (event == LV_EVENT_KEY)
    {
        uint32_t key = *(uint32_t *)lv_event_get_param(e);
        
        if (key == LV_KEY_BACKSPACE)
        {
            screen_load_prev_sub_page(0, 0);
        }
		else if (LV_KEY_DEL == key)  //system menu
        {            
            uint8_t index = page_get_index(PAGE_MENU);//获取上一次的字ID
            screen_load_page_and_index(PAGE_MENU, 0, index, true);
        }
    }
    else if (LV_EVENT_DRAW_POST == event)
    {
        lv_point_t poly[4];
        const lv_area_t * clip_area = lv_event_get_param(e);
        lv_draw_rect_dsc_t rect_dsc;
        const int64_t radius = 15;
        lv_point_t center;
        const int64_t w = 8;
        const int64_t h = 23;

        lv_draw_rect_dsc_init(&rect_dsc);        
                       
        center.x = obj->coords.x1 + lv_obj_get_width(obj) / 2;
        center.y = obj->coords.y1 + lv_obj_get_height(obj) / 2;

        lv_point_t p1;
        int16_t angle = 0;
        uint8_t diff = *p_vanish_idx;

        for (uint8_t i = 0; i < 4; i++)
        {
            angle = 0 + i * 45;
               
            p1.x = center.x + (lv_coord_t)(((radius + h) * 1000 * lv_trigo_sin(angle) >> LV_TRIGO_SHIFT) / 1000);
            p1.y = center.y - (lv_coord_t)(((radius + h) * 1000 * lv_trigo_cos(angle) >> LV_TRIGO_SHIFT) / 1000);

            poly[0].x = p1.x - (lv_coord_t)((w * 1000 * lv_trigo_cos(angle) >> LV_TRIGO_SHIFT)  / 1000 / 2);
            poly[0].y = p1.y - (lv_coord_t)((w * 1000 * lv_trigo_sin(angle) >> LV_TRIGO_SHIFT)  / 1000 / 2);
            poly[1].x = p1.x + (lv_coord_t)((w * 1000 * lv_trigo_cos(angle) >> LV_TRIGO_SHIFT)  / 1000 / 2);
            poly[1].y = p1.y + (lv_coord_t)((w * 1000 * lv_trigo_sin(angle) >> LV_TRIGO_SHIFT)  / 1000 / 2);
            
            p1.x = center.x + (lv_coord_t)(((radius) * 1000 * lv_trigo_sin(angle) >> LV_TRIGO_SHIFT) / 1000);
            p1.y = center.y - (lv_coord_t)(((radius) * 1000 * lv_trigo_cos(angle) >> LV_TRIGO_SHIFT) / 1000);

            poly[2].x = p1.x + (lv_coord_t)((w * 1000  * lv_trigo_cos(angle) >> LV_TRIGO_SHIFT) / 1000 / 2);
            poly[2].y = p1.y + (lv_coord_t)((w * 1000  * lv_trigo_sin(angle) >> LV_TRIGO_SHIFT) / 1000 / 2);
            poly[3].x = p1.x - (lv_coord_t)((w * 1000  * lv_trigo_cos(angle) >> LV_TRIGO_SHIFT) / 1000 / 2);
            poly[3].y = p1.y - (lv_coord_t)((w * 1000  * lv_trigo_sin(angle) >> LV_TRIGO_SHIFT) / 1000 / 2);

            uint8_t index = i;
            if (index == diff)
                rect_dsc.bg_color = lv_obj_get_style_bg_color(obj, 0);
            else
                rect_dsc.bg_color = LOADING_IMG_COLOR;

            lv_draw_polygon(poly, 4, clip_area, &rect_dsc);
            
            poly[0].x = (center.x - poly[0].x)  + center.x;
            poly[0].y = (center.y - poly[0].y)  + center.y;
            poly[1].x = (center.x - poly[1].x)  + center.x;
            poly[1].y = (center.y - poly[1].y)  + center.y;
            poly[2].x = (center.x - poly[2].x)  + center.x;
            poly[2].y = (center.y - poly[2].y)  + center.y;
            poly[3].x = (center.x - poly[3].x)  + center.x;
            poly[3].y = (center.y - poly[3].y)  + center.y;

            index = i + 4;
            if (index == diff)
                rect_dsc.bg_color = lv_obj_get_style_bg_color(obj, 0);
            else
                rect_dsc.bg_color = LOADING_IMG_COLOR;

            lv_draw_polygon(poly, 4, clip_area, &rect_dsc);
        }    
    }
}

static void arc_panel_event_cb(lv_event_t* e)
{
    const lv_area_t* clip_area = lv_event_get_clip_area(e);
    lv_draw_arc_dsc_t arc_dsc;
    lv_coord_t center_x, center_y, radius;

    center_x = clip_area->x1 + lv_area_get_width(clip_area) / 2;
    center_y = clip_area->y1 + lv_area_get_height(clip_area) / 2;
    radius = 53;

    lv_draw_arc_dsc_init(&arc_dsc);    
    arc_dsc.color = lv_color_white();
    lv_draw_arc(center_x, center_y, radius, 0, 360, clip_area, &arc_dsc);
}

static void refresh_int_arc(lv_obj_t* arc)
{
    int16_t intensity = custom_fx_get_intensity(p_local_cfx_arg->type, custom_fx_get_index());
    int16_t angle = lv_map(intensity, 0, 1000, 0, 100);
    lv_obj_t* label = lv_obj_get_child(arc, -2);
    lv_obj_t* symbol = lv_obj_get_child(arc, -1);
    lv_obj_t* arc_panel = lv_obj_get_parent(arc);

    lv_arc_set_value(arc, angle);
    
    if (intensity >= 1000)
    {
        lv_snprintf(sdb, sizeof(sdb), "%d", 100);
    }
    else
    {
        lv_snprintf(sdb, sizeof(sdb), "%d.%d", intensity / 10, intensity % 10);
    }

    lv_label_set_text(label, sdb);
    lv_obj_align_to(label, symbol, LV_ALIGN_OUT_LEFT_BOTTOM, -2, 2);
    lv_obj_invalidate(arc_panel);
}

static void custom_fx_set_intensity(uint8_t type, uint8_t index, int16_t intensity)
{
    p_local_cfx_ctrl_arg->lightness = intensity;  
    page_event_send(EVENT_DATA_WRITE);
}

static void circle_panel_event_cb(lv_event_t* e)
{
    lv_obj_t* arc = lv_event_get_target(e);
    lv_event_code_t event = lv_event_get_code(e);

    shortcut_keys_event_handler(e);

   if (event == LV_EVENT_KEY)
    {
        uint32_t key = *(uint32_t *)lv_event_get_param(e);
        if (LV_ENCODER_KEY_CW == key) //up
        {            
            int16_t intensity = custom_fx_get_intensity(p_local_cfx_arg->type, custom_fx_get_index());            

            intensity += 1 *comm_enc_get_step(enc_get_step_index());            
            if (intensity > 1000)
                intensity = 1000;

            custom_fx_set_intensity(p_local_cfx_arg->type, custom_fx_get_index(), intensity);            
            refresh_int_arc(int_arc);                       
        }
        else if (LV_ENCODER_KEY_CC == key) //down
        {   
            int16_t intensity = custom_fx_get_intensity(p_local_cfx_arg->type, custom_fx_get_index());      

            intensity -= 1 * comm_enc_get_step(enc_get_step_index());
            if (intensity < 0)
                intensity = 0;
            
            custom_fx_set_intensity(p_local_cfx_arg->type, custom_fx_get_index(), intensity);
            refresh_int_arc(int_arc);            
        }
        else if (LV_KEY_BACKSPACE == key)
        {
            screen_load_prev_sub_page(SUB_ID_CUSTOM_FX_CHOOSE, 0);
        }
    }
    else if (LV_EVENT_RELEASED == event)
    {        
        const uint16_t intensity[5] = {200, 400, 600, 800, 1000};
        uint8_t index = (custom_fx_get_intensity(p_local_cfx_arg->type, custom_fx_get_index()) / 200) % (sizeof(intensity) / sizeof(intensity[0]));
        custom_fx_set_intensity(p_local_cfx_arg->type, custom_fx_get_index(), intensity[index]);        
        refresh_int_arc(int_arc);
    }
}

static void custom_fx_choose_list_user_event(lv_event_t* e, uint8_t index)
{
    lv_event_code_t event = lv_event_get_code(e);
    lv_obj_t* obj = lv_event_get_target(e);
    my_radiobox_ext_t* ext = lv_obj_get_user_data(obj);
    uint8_t  hsmode ;
    if (event == LV_EVENT_RELEASED)
    {
        p_local_cfx_arg->bank[p_local_cfx_arg->type] = index;    
//		data_center_read_config_data(SYS_CONFIG_HIGH_SPEED_MODE, &hsmode);
//		if(hsmode)
//		{
//			set_hs_custom_fx_window();
//			return;
//		}
        if (!ex_custom_fx_is_empty(p_local_cfx_arg->type, index))
        {            
            p_local_cfx_arg->ctrl = 1; //设置光效初始化.
            p_local_cfx_ctrl_arg->ctrl = 1;
//			data_center_read_config_data(SYS_CONFIG_HIGH_SPEED_MODE, &hsmode);
//			if(hsmode)
//			{
//				set_hs_custom_fx_window();
//			}
//			else
//			{
				page_event_send(EVENT_DATA_WRITE);  
//			}

            //screen_load_sub_page_with_stack(SUB_ID_CUSTOM_FX_LOADING, index);  
        }
        else  
        {
            p_local_cfx_arg->ctrl = 0; //设置光效停止
            p_local_cfx_ctrl_arg->ctrl = 0;	
			page_event_send(EVENT_DATA_WRITE);   
            //screen_load_sub_page_with_stack(SUB_ID_FX_INTENSITY, index);  
        }
        
    }
    else if (event == LV_EVENT_FOCUSED)
    {
        my_list_scroll_refresh(scroll_cont, lv_map(ext->sel, 0, ext->max, 0, 100));
    }
    else if (event == LV_EVENT_KEY)
    {
        uint32_t key = *(uint32_t *)lv_event_get_param(e);
        if ( LV_ENCODER_KEY_CC == key) //up
        {           
            my_list_scroll_refresh(scroll_cont, lv_map(ext->sel, 0, ext->max, 0, 100));
        }
        else if (LV_ENCODER_KEY_CW == key) //down
        {   
            my_list_scroll_refresh(scroll_cont, lv_map(ext->sel, 0, ext->max, 0, 100));
        }
        else if (LV_KEY_BACKSPACE == key)
        {
            screen_load_prev_sub_page(0, 0);
        }
    }
}
