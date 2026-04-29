/*********************
 *      INCLUDES
 *********************/
#include "ui_common.h"
#include "page_widget.h"

/*********************
 *      DEFINES
 *********************/
#define X_BTN_WIDTH     123
#define X_BTN_HEIGHT    44

#define Y_BTN_WIDTH     X_BTN_WIDTH
#define Y_BTN_HEIGHT    X_BTN_HEIGHT

#define BAR_CONT_HEIGHT 32

#define INTENSITY_CONT_HEIGHT      32 

#define BTN_TEXT_COLOR          lv_color_make(161, 161, 161)
#define BTN_BG_COLOR            lv_color_make(41, 40, 41)

#define BTN_BG_USER1_COLOR      lv_color_make(255, 255, 255)
#define BTN_BG_FOCUSED_COLOR    RED_THEME_COLOR
#define BTN_TEXT_FOCUSED_COLOR  lv_color_make(255, 255, 255)

#define MAX_OFFSET_VALUE        50

/*********************
 *      TPEDEFS
 *********************/


/**********************
 *  STATIC PROTOTYPES
 **********************/
static void x_upd_run_cb(void* data);
static void y_upd_run_cb(void* data);

static bool x_assign_value_cb(void* data);
static bool y_assign_value_cb(void* data);

static void src_btn_cont_event_cb(lv_event_t* e);
static void src_btn_event_cb(lv_event_t* e);
static void refresh_xy_btn(uint8_t index, uint16_t x, uint16_t y);

static void src_btn_style_init(lv_obj_t* obj);
static void page_source_setting_construct(void);
static void page_source_setting_destruct(void);
static void page_souce_settiing_storage(void);
/*********************
 *  STATIC VATIABLES
 *********************/
static lv_group_t *indev_group;
static adj_btn_t btn_list[] = {
    ADJ_BTN_INITIALIZER(0, 8000, 1, 4260, "X", "" , 25, 40, -10, false, false,  xy_get_value_str, x_upd_run_cb, x_assign_value_cb, NULL, NULL),
    ADJ_BTN_INITIALIZER(0, 8000, 1, 4005, "Y", "" , 25, 40, -10, false, false,  xy_get_value_str, y_upd_run_cb, y_assign_value_cb, NULL, NULL), 
};

static struct db_fx_mode_arg* p_src_setting_model;
static lv_obj_t* x_btn;
static lv_obj_t* y_btn;
static custom_cont_t src_custom_cont;
static src_tab_t src_table[SOURCE_MAX];
static void page_source_time_upd_cb(uint32_t ms);
	
/*********************
 *  GLOBAL VATIABLES
 *********************/
page_t page_source_setting = PAGE_INITIALIZER("source setting",
                                PAGE_SOURCE_SETTING, 
                                page_source_setting_construct, 
                                page_source_setting_destruct, 
                                page_source_time_upd_cb,
                                NULL,                                
                                PAGE_NONE);

/**********************
 *   GLOBAL FUNCTIONS
 **********************/
void page_source_setting_init(lv_obj_t* parent)
{
    p_src_setting_model = (struct db_fx_mode_arg*)screen_get_act_page_data();

    for (uint8_t i = 0; i < SOURCE_MAX; i++)
    {
        const src_table_t* p_table = source_get_table(i);
        src_table[i] = p_table->value;
    }

    indev_group = lv_group_create();    
    my_indev_set_group(indev_group);
    lv_group_set_default(indev_group);

    lv_obj_set_style_bg_color(parent, lv_color_black(), 0);
    
    lv_obj_t* obj_panel = lv_obj_create(parent);
    lv_obj_remove_style_all(obj_panel);
    lv_obj_set_size(obj_panel, lv_pct(100), lv_pct(95));
    lv_obj_center(obj_panel);

    src_custom_cont.min = 0;
    src_custom_cont.cur_index = p_src_setting_model->source.type;
    src_custom_cont.max = SOURCE_MAX - 1;

    lv_obj_t* src_btn_cont = lv_obj_create(obj_panel);
    //lv_obj_remove_style_all(src_btn_cont);
    lv_obj_add_style(src_btn_cont, &style_rect_panel, 0);
    lv_obj_set_flex_flow(src_btn_cont, LV_FLEX_FLOW_ROW);
    lv_obj_set_style_pad_all(src_btn_cont, 0, 0);
    lv_obj_set_style_pad_gap(src_btn_cont, 6, 0);
    lv_obj_set_style_pad_top(src_btn_cont, 21, 0);
    lv_obj_set_style_pad_left(src_btn_cont, 10, 0);

    lv_obj_set_style_pad_right(src_btn_cont, 10, 0);
    lv_obj_set_size(src_btn_cont, lv_pct(100), 132);    
    //lv_obj_set_scroll_snap_x(src_btn_cont, LV_SCROLL_SNAP_CENTER);
    lv_obj_align(src_btn_cont, LV_ALIGN_TOP_MID, 0, 10);
    lv_obj_set_user_data(src_btn_cont, &src_custom_cont);
    lv_obj_add_event_cb(src_btn_cont, src_btn_cont_event_cb, LV_EVENT_ALL, NULL);
    src_custom_cont.obj = src_btn_cont;

    lv_obj_set_style_bg_color(src_btn_cont, lv_color_make(0, 0, 0), 0);  
    for (uint8_t i = 0; i <  SOURCE_MAX; i++)
    {
        
        lv_obj_t* obj = lv_obj_create(src_btn_cont);
        lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
        lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
        lv_obj_set_style_pad_all(obj, 0, 0);
        
        src_btn_style_init(obj);        
        lv_obj_set_size(obj, 71, 94);  
        lv_obj_set_style_width(obj, 72, LV_STATE_FOCUSED);
        lv_obj_set_style_height(obj, 96, LV_STATE_FOCUSED);
    
        const src_table_t* p_table = source_get_table(i);
        lv_obj_t* label = lv_label_create(obj);
        lv_obj_set_size(label, lv_pct(100), LV_SIZE_CONTENT);
        lv_label_set_long_mode(label, LV_LABEL_LONG_WRAP);
        lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, 0);
        lv_label_set_text_fmt(label, "%s", p_table->name);        
        lv_obj_center(label);
        lv_obj_add_event_cb(obj, src_btn_event_cb, LV_EVENT_ALL, NULL);
    }
    
    //设置容器滚动形式
    if (p_src_setting_model->source.type < (src_custom_cont.max - 2) && p_src_setting_model->source.type >= 2)
    {
        lv_obj_set_scroll_snap_x(src_btn_cont, LV_SCROLL_SNAP_CENTER);
    } 

    lv_obj_t* bottom_cont = lv_obj_create(obj_panel);
    lv_obj_remove_style_all(bottom_cont);
    lv_obj_set_size(bottom_cont, lv_pct(100), 60);

    x_btn = adj_btn_create(bottom_cont, LIGHT_MODE_COMMON_RADIUS, ADJ_BTN_TYPE_18);
    lv_obj_set_size(x_btn, X_BTN_WIDTH, X_BTN_HEIGHT);
    lv_obj_align(x_btn, LV_ALIGN_LEFT_MID, 24, 0);
    lv_obj_set_user_data(x_btn, (void *)&btn_list[0]);
    lv_obj_add_event_cb(x_btn, adj_btn_event_cb, LV_EVENT_ALL, NULL);
    adj_btn_init(x_btn, &btn_list[0], 20);

    y_btn = adj_btn_create(bottom_cont, LIGHT_MODE_COMMON_RADIUS, ADJ_BTN_TYPE_18);
    lv_obj_set_size(y_btn, X_BTN_WIDTH, X_BTN_HEIGHT);
    lv_obj_align_to(y_btn, x_btn, LV_ALIGN_OUT_RIGHT_MID, 24, 0);
    lv_obj_set_user_data(y_btn, (void *)&btn_list[1]);
    adj_btn_init(y_btn, &btn_list[1], 20);
    lv_obj_add_event_cb(y_btn, adj_btn_event_cb, LV_EVENT_ALL, NULL);

    lv_obj_align_to(bottom_cont, src_btn_cont, LV_ALIGN_OUT_BOTTOM_MID, 0, 0);
    lv_group_add_obj(indev_group, src_btn_cont);
    lv_group_add_obj(indev_group, x_btn);
    lv_group_add_obj(indev_group, y_btn);

    lv_obj_t* src_obj = lv_obj_get_child(src_btn_cont, p_src_setting_model->source.type);
    lv_obj_scroll_to_view(src_obj, LV_ANIM_OFF);     
    lv_obj_add_state(src_obj, LV_STATE_USER_1);  

    p_src_setting_model->source.x = src_table[p_src_setting_model->source.type].coord_x;
    p_src_setting_model->source.y = src_table[p_src_setting_model->source.type].coord_y;
    refresh_xy_btn(p_src_setting_model->source.type, p_src_setting_model->source.x, p_src_setting_model->source.y);

    lv_group_focus_obj(src_btn_cont);
    lv_group_set_editing(indev_group, false);
    lv_group_set_wrap(indev_group, false);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/
static void page_source_setting_construct(void)
{
    lv_obj_set_style_bg_color(lv_scr_act(), lv_color_black(), 0);

    lv_obj_t* t_cont = lv_obj_create(lv_scr_act());
    lv_obj_remove_style_all(t_cont);
    lv_obj_set_size(t_cont, lv_pct(100), lv_pct(13));
    lv_obj_set_pos(t_cont, 0, 0);

    lv_obj_t* b_cont = lv_obj_create(lv_scr_act());
    lv_obj_remove_style_all(b_cont);
    lv_obj_set_size(b_cont, lv_pct(100), lv_pct(87));
    lv_obj_align_to(b_cont, t_cont, LV_ALIGN_OUT_BOTTOM_MID, 0, 0);
    page_source_setting_init(b_cont);
}

static void page_source_setting_destruct(void)
{
    lv_group_focus_freeze(indev_group, true);
    lv_group_remove_all_objs(indev_group);
    lv_group_del(indev_group);
    indev_group = NULL;

    lv_obj_clean(lv_scr_act());
	page_souce_settiing_storage();
}

static void refresh_xy_btn(uint8_t index, uint16_t x, uint16_t y)
{
    if (x_btn == NULL || y_btn == NULL)     return;

    const src_table_t* p_table = source_get_table(index);
    adj_btn_t* xdsc = lv_obj_get_user_data(x_btn);
    adj_btn_t* ydsc = lv_obj_get_user_data(y_btn);
    
    xdsc->value = x;
    xdsc->max = p_table->value.coord_x + 50;
    xdsc->min = p_table->value.coord_x - 50;    

    ydsc->value = y;
    ydsc->max = p_table->value.coord_y + 50;
    ydsc->min = p_table->value.coord_y - 50;    
    
    adj_btn_run_update_cb(xdsc);
    adj_btn_refresh(x_btn, xdsc);
    adj_btn_run_update_cb(ydsc);
    adj_btn_refresh(y_btn, ydsc);
}

static void src_btn_cont_event_cb(lv_event_t* e)
{
    lv_obj_t* cont = lv_event_get_target(e);
    lv_event_code_t event = lv_event_get_code(e);

    shortcut_keys_event_handler(e);
    backspace_key_event_handler(e);

    if (event == LV_EVENT_FOCUSED)
    {
        lv_group_t* group = lv_obj_get_group(cont);
        
        if (lv_group_get_editing(group) == false)
        {
            lv_obj_t* cur_focus_obj = lv_obj_get_child(cont, p_src_setting_model->source.type);
            lv_obj_set_style_bg_color(cur_focus_obj, BTN_BG_FOCUSED_COLOR, 0);
            lv_obj_set_style_text_color(cur_focus_obj, lv_color_white(), 0);
            lv_obj_scroll_to_view(cur_focus_obj, LV_ANIM_OFF);   
            lv_obj_clear_state(cur_focus_obj, LV_STATE_USER_1);
        }     
    }
    else if (event == LV_EVENT_DEFOCUSED)
    {
        lv_obj_t* cur_focus_obj = lv_obj_get_child(cont, p_src_setting_model->source.type);
        lv_obj_set_style_text_color(cur_focus_obj, BTN_TEXT_COLOR, 0);
        lv_obj_set_style_bg_opa(cur_focus_obj, LV_OPA_100, 0);
        lv_obj_scroll_to_view(cur_focus_obj, LV_ANIM_OFF);      
        lv_obj_add_state(cur_focus_obj, LV_STATE_USER_1);  
    }
    else if (event == LV_EVENT_RELEASED)
    {
        //UI_PRINTF("Released\r\n");
        lv_group_t* group = lv_obj_get_group(cont);
        if (lv_group_get_editing(group))
        {            
            custom_cont_t *custom_dsc = lv_obj_get_user_data(cont);
            lv_obj_t* cur_focus_obj = lv_obj_get_child(cont, p_src_setting_model->source.type);
            
            lv_group_set_editing(group, false);      
            
            lv_obj_set_style_text_color(cur_focus_obj, BTN_TEXT_COLOR, 0);
            lv_obj_set_style_bg_opa(cur_focus_obj, LV_OPA_100, 0);
            
            p_src_setting_model->source.type = (uint8_t)custom_dsc->cur_index;
            cur_focus_obj = lv_obj_get_child(cont, custom_dsc->cur_index);  
            
            lv_obj_clear_state(cur_focus_obj, LV_STATE_FOCUSED);
            //接触container的edit状态
            lv_event_send(cont, LV_EVENT_FOCUSED, NULL);
        }
        else
        {            
            lv_obj_t* src_obj = lv_obj_get_child(cont, p_src_setting_model->source.type);
            lv_group_set_editing(group, true);
            
            lv_obj_set_style_bg_opa(src_obj, LV_OPA_100, 0);          
            lv_obj_set_style_text_color(src_obj, BTN_TEXT_COLOR, 0);  
            lv_obj_set_style_bg_color(src_obj, BTN_BG_COLOR, 0);      
            lv_obj_add_state(src_obj, LV_STATE_FOCUSED);            
        }
    }
     else if (event == LV_EVENT_KEY && lv_group_get_editing(indev_group))
     {
        uint32_t key = *(uint32_t*)lv_event_get_param(e);
        custom_cont_t *custom_dsc = lv_obj_get_user_data(cont);
        lv_obj_t* cur_obj = lv_obj_get_child(cont, custom_dsc->cur_index);
        bool scroll = true;
		if (lv_obj_is_scrolling(cont))  return;

        //go next
        if (key == LV_ENCODER_KEY_CW)
        {            
            custom_dsc->cur_index += 1;
            if (custom_dsc->cur_index > custom_dsc->max)
            {
                custom_dsc->cur_index = custom_dsc->max;
                scroll = false;
            }
            
            lv_obj_clear_state(cur_obj, LV_STATE_FOCUSED);

            cur_obj = lv_obj_get_child(cont, custom_dsc->cur_index);
            lv_obj_add_state(cur_obj, LV_STATE_FOCUSED);
            lv_event_send(cur_obj, LV_EVENT_FOCUSED, NULL);
        }
        //go prev
        else if (key == LV_ENCODER_KEY_CC)
        {    
            custom_dsc->cur_index -= 1;
            if (custom_dsc->cur_index < custom_dsc->min)
            {
                custom_dsc->cur_index = custom_dsc->min;
                scroll = false;
            }    

            lv_obj_clear_state(cur_obj, LV_STATE_FOCUSED);
            
            cur_obj = lv_obj_get_child(cont, custom_dsc->cur_index);
            lv_obj_add_state(cur_obj, LV_STATE_FOCUSED);
            lv_event_send(cur_obj, LV_EVENT_FOCUSED, NULL);
        }

        if ((key == LV_ENCODER_KEY_CW || key == LV_ENCODER_KEY_CC) && scroll)
        {
            p_src_setting_model->source.type = (uint8_t)custom_dsc->cur_index;
            cur_obj = lv_obj_get_child(cont, custom_dsc->cur_index);   

            if (p_src_setting_model->source.type >= (custom_dsc->max - 2))
            {
                lv_obj_set_scroll_snap_x(cont, LV_SCROLL_SNAP_NONE); 
            }
            else if (p_src_setting_model->source.type >= 2)
            {
                lv_obj_set_scroll_snap_x(cont, LV_SCROLL_SNAP_CENTER);
            }
            else if (p_src_setting_model->source.type >= 1)
            {
                lv_obj_set_scroll_snap_x(cont, LV_SCROLL_SNAP_NONE); 
            }   
            else                      
            {
                lv_obj_set_scroll_snap_x(cont, LV_SCROLL_SNAP_START); 
            }

            lv_obj_scroll_to_view(cur_obj, LV_ANIM_ON);  
        }   
        else if (key == LV_KEY_BACKSPACE) 
        {
            lv_event_send(cont, LV_EVENT_RELEASED, NULL);
        }     
     }
}

static void src_btn_event_cb(lv_event_t* e)
{
    lv_obj_t* obj = lv_event_get_target(e);
    lv_event_code_t event = lv_event_get_code(e);

    if (LV_EVENT_FOCUSED == event)
    {
		
        uint32_t index = lv_obj_get_child_id(obj);
        refresh_xy_btn(index, src_table[index].coord_x, src_table[index].coord_y);            
    }
}

static void src_btn_style_init(lv_obj_t* obj)
{    
    lv_obj_set_style_bg_color(obj, BTN_BG_COLOR, 0);
    lv_obj_set_style_text_color(obj, BTN_TEXT_COLOR, 0);
    lv_obj_set_style_text_font(obj, Font_ResouceGet(FONT_10_MEDIUM), 0);
    lv_obj_set_style_border_width(obj, 0, 0);    
    lv_obj_set_style_shadow_ofs_y(obj, 0, 0);

    lv_obj_set_style_text_font(obj, Font_ResouceGet(FONT_10_BOLD), LV_STATE_FOCUSED);    
    lv_obj_set_style_bg_color(obj, BTN_BG_FOCUSED_COLOR, LV_STATE_FOCUSED);    
    lv_obj_set_style_text_color(obj, BTN_TEXT_FOCUSED_COLOR, LV_STATE_FOCUSED);      
    
    lv_obj_set_style_bg_color(obj, BTN_BG_USER1_COLOR, LV_STATE_USER_1);
    lv_obj_set_style_text_color(obj, lv_color_make(0, 0, 0), LV_STATE_USER_1);   
}

static void x_upd_run_cb(void* data)
{
    adj_btn_t* dsc = (adj_btn_t*)data;
    p_src_setting_model->xy.x = dsc->value;    
}

static void y_upd_run_cb(void* data)
{
    adj_btn_t* dsc = (adj_btn_t*)data;
    p_src_setting_model->xy.y = dsc->value;
}

static bool x_assign_value_cb(void* data)
{
    adj_btn_t* dsc = (adj_btn_t*)data;
    ASSIGN_VALUE_TO_BTN(p_src_setting_model->xy.x);
}

static bool y_assign_value_cb(void* data)
{
    adj_btn_t* dsc = (adj_btn_t*)data;
    ASSIGN_VALUE_TO_BTN(p_src_setting_model->xy.y);
}

static void page_souce_settiing_storage(void)
{
	#if PROJECT_TYPE==307
    uint32_t src_crc32, dest_crc32;
	uint8_t mode;
	mode = date_center_get_light_mode();
	switch(mode)
	{	
		case LIGHT_MODE_FX_LIGHTNING_II:
		{
			struct db_fx_lightning_2 lightning;
			struct db_fx_lightning_2  p_lightning_model;
			data_center_read_light_data(date_center_get_light_mode(), &lightning);   
			dest_crc32 = ui_calc_checksum((uint8_t*)&lightning,  sizeof(struct db_fx_lightning_2)); 
			memcpy(&p_lightning_model, &g_tUIAllData.lightning2_model, sizeof(struct db_fx_lightning_2));
			src_crc32 = ui_calc_checksum((uint8_t*)&g_tUIAllData.lightning2_model, sizeof(struct db_fx_lightning_2));            
			if (dest_crc32 != src_crc32)
			{
				
				data_center_write_light_data(LIGHT_MODE_FX_LIGHTNING_II, &p_lightning_model);
				memcpy(p_src_setting_model,&g_tUIAllData.lightning2_model.mode_arg,sizeof(g_tUIAllData.lightning2_model.mode_arg)); 
				adj_btn_refresh_all(btn_list, sizeof(btn_list)/ sizeof(btn_list[0]), true);
			}
		}
		break;
		
		case LIGHT_MODE_FX_STROBE_II:
		{
			struct db_fx_strobe_2 strobe;
			struct db_fx_strobe_2  p_strobe_model;
			data_center_read_light_data(date_center_get_light_mode(), &strobe);   
			dest_crc32 = ui_calc_checksum((uint8_t*)&strobe,  sizeof(struct db_fx_strobe_2)); 
			memcpy(&p_strobe_model, &g_tUIAllData.strobe2_model, sizeof(struct db_fx_strobe_2));
			src_crc32 = ui_calc_checksum((uint8_t*)&g_tUIAllData.strobe2_model, sizeof(struct db_fx_strobe_2));            
			if (dest_crc32 != src_crc32)
			{
				
				data_center_write_light_data(LIGHT_MODE_FX_STROBE_II, &p_strobe_model);
				memcpy(p_src_setting_model,&g_tUIAllData.strobe2_model.mode_arg,sizeof(g_tUIAllData.strobe2_model.mode_arg)); 
				adj_btn_refresh_all(btn_list, sizeof(btn_list)/ sizeof(btn_list[0]), true);
			}
		}
		break;
		
		case LIGHT_MODE_FX_EXPLOSION_II:
		{
			struct db_fx_explosion_2 explosion;
			struct db_fx_explosion_2  p_explosion_model;
			data_center_read_light_data(date_center_get_light_mode(), &explosion);   
			dest_crc32 = ui_calc_checksum((uint8_t*)&explosion,  sizeof(struct db_fx_explosion_2)); 
			memcpy(&p_explosion_model, &g_tUIAllData.explosion2_model, sizeof(struct db_fx_explosion_2));
			src_crc32 = ui_calc_checksum((uint8_t*)&g_tUIAllData.explosion2_model, sizeof(struct db_fx_explosion_2));            
			if (dest_crc32 != src_crc32)
			{
				
				data_center_write_light_data(LIGHT_MODE_FX_EXPLOSION_II, &p_explosion_model);
				memcpy(p_src_setting_model,&g_tUIAllData.explosion2_model.mode_arg,sizeof(g_tUIAllData.explosion2_model.mode_arg)); 
				 adj_btn_refresh_all(btn_list, sizeof(btn_list)/ sizeof(btn_list[0]), true);
			}
		}
		break;
		
			case LIGHT_MODE_FX_FAULT_BULB_II:
		{
			struct db_fx_fault_bulb_2 fault_bulb;
			struct db_fx_fault_bulb_2  p_fault_bulb_model;
			data_center_read_light_data(date_center_get_light_mode(), &fault_bulb);   
			dest_crc32 = ui_calc_checksum((uint8_t*)&fault_bulb,  sizeof(struct db_fx_fault_bulb_2)); 
			memcpy(&p_fault_bulb_model, &g_tUIAllData.faultybulb2_model, sizeof(struct db_fx_fault_bulb_2));
			src_crc32 = ui_calc_checksum((uint8_t*)&g_tUIAllData.faultybulb2_model, sizeof(struct db_fx_fault_bulb_2));            
			if (dest_crc32 != src_crc32)
			{
				
				data_center_write_light_data(LIGHT_MODE_FX_FAULT_BULB_II, &p_fault_bulb_model);
				memcpy(p_src_setting_model,&g_tUIAllData.faultybulb2_model.mode_arg,sizeof(g_tUIAllData.faultybulb2_model.mode_arg)); 
				 adj_btn_refresh_all(btn_list, sizeof(btn_list)/ sizeof(btn_list[0]), true);
			}
		}
		break;
		
			
			case LIGHT_MODE_FX_PULSING_II:
		{
			struct db_fx_pulsing_2 pulsing;
			struct db_fx_pulsing_2  p_pulsing_model;
			data_center_read_light_data(date_center_get_light_mode(), &pulsing);   
			dest_crc32 = ui_calc_checksum((uint8_t*)&pulsing,  sizeof(struct db_fx_pulsing_2)); 
			memcpy(&p_pulsing_model, &g_tUIAllData.pulsing2_model, sizeof(struct db_fx_pulsing_2));
			src_crc32 = ui_calc_checksum((uint8_t*)&g_tUIAllData.pulsing2_model, sizeof(struct db_fx_pulsing_2));            
			if (dest_crc32 != src_crc32)
			{
				
				data_center_write_light_data(LIGHT_MODE_FX_PULSING_II, &p_pulsing_model);
				memcpy(p_src_setting_model,&g_tUIAllData.pulsing2_model.mode_arg,sizeof(g_tUIAllData.pulsing2_model.mode_arg)); 
				 adj_btn_refresh_all(btn_list, sizeof(btn_list)/ sizeof(btn_list[0]), true);
			}
		}
		break;
		
			case LIGHT_MODE_FX_WELDING_II:
		{
			struct db_fx_welding_2 welding;
			struct db_fx_welding_2  p_welding_model;
			data_center_read_light_data(date_center_get_light_mode(), &welding);   
			dest_crc32 = ui_calc_checksum((uint8_t*)&welding,  sizeof(struct db_fx_welding_2)); 
			memcpy(&p_welding_model, &g_tUIAllData.welding2_model, sizeof(struct db_fx_welding_2));
			src_crc32 = ui_calc_checksum((uint8_t*)&g_tUIAllData.welding2_model, sizeof(struct db_fx_welding_2));            
			if (dest_crc32 != src_crc32)
			{
				
				data_center_write_light_data(LIGHT_MODE_FX_WELDING_II, &p_welding_model);
				memcpy(p_src_setting_model,&g_tUIAllData.welding2_model.mode_arg,sizeof(g_tUIAllData.welding2_model.mode_arg)); 
				 adj_btn_refresh_all(btn_list, sizeof(btn_list)/ sizeof(btn_list[0]), true);
			}
		}
		break;
		
	}
  #endif
   
}

static void page_source_time_upd_cb(uint32_t ms)
{
	page_souce_settiing_storage();

}
