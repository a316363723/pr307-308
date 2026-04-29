/*********************
 *      INCLUDES
 *********************/
#include "ui_common.h"
#include "page_widget.h"
#include "stdio.h"

/*********************
 *      DEFINES
 *********************/
#define COM_BTN_EDITIED_COLOR                   COMMON_EDITED_COLOR
#define MENU_ITEM_TEXT_COLOR                    lv_color_make(0, 0, 0)
#define INTENSITY_BAR_BG_COLOR                  lv_color_make(128, 128, 128)
#define INTENSITY_BAR_INDICATOR_COLOR           lv_color_make(255, 255, 255)
/**********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *   GLOBAL VARIABLES
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/
//lv_obj_t* power_cont_create(lv_obj_t* parent, ui_batt_dsc_t* p_dsc);
/**
 * @brief 正常页面的所有控件事件需要继承该函数.
 * 
 * @param e 
 */
void shortcut_keys_event_handler(lv_event_t* e)
{
    lv_obj_t* obj = lv_event_get_target(e);
    lv_event_code_t event = lv_event_get_code(e);
    lv_group_t* group = lv_obj_get_group(obj);
    lv_indev_type_t indev_type = lv_indev_get_type(lv_indev_get_act());

    if (LV_INDEV_TYPE_KEYPAD == indev_type && event == LV_EVENT_KEY)
    {        
        uint32_t key = *(uint32_t *)lv_event_get_param(e);

        if (LV_KEY_HOME == key || LV_KEY_END == key)  //drop down panel
        {
            obj_send_event_to_screen(LV_EVENT_KEY, &key);          
        }
        else if (LV_KEY_DEL == key)  //system menu
        {            
            uint8_t index = page_get_index(PAGE_MENU);//获取上一次的字ID
            screen_load_page_and_index(PAGE_MENU, 0, index, true);
        }
    }
}

void light_mode_special_op_process(void)
{
    uint8_t cur_page_id = screen_get_act_pid();

    if ((cur_page_id >= PAGE_LIGHT_MODE_HSI && cur_page_id <= PAGE_LIGHT_EFFECT_MENU) || \
                            (cur_page_id == PAGE_SIDUS_PRO_FX))
    {        
        uint32_t key = LV_KEY_HOME;
        obj_send_event_to_screen(LV_EVENT_KEY, &key);
    }
}

void backspace_key_event_handler(lv_event_t* e)
{
    lv_obj_t* obj = lv_event_get_target(e);
    lv_event_code_t event = lv_event_get_code(e);
    lv_group_t* group = lv_obj_get_group(obj);
    lv_indev_type_t indev_type = lv_indev_get_type(lv_indev_get_act());

    if (LV_INDEV_TYPE_KEYPAD == indev_type && event == LV_EVENT_KEY)
    {        
        uint32_t key = *(uint32_t *)lv_event_get_param(e);
                
        if (LV_KEY_BACKSPACE == key)
        {
            UI_PRINTF("LV_KEY_BACKSPACE\r\n");   
            bool ret = false;

            if (!lv_group_get_editing(group))
            {

				if (screen_get_act_pid() >= PAGE_PULSING && screen_get_act_pid() <= PAGE_PARTYLIGHT)
				{
					screen_load_page(PAGE_LIGHT_EFFECT_MENU, 0, false);
				}
				else if(screen_get_act_pid() >= PAGE_HSI_SETTING && screen_get_act_pid() <= PAGE_ELECT_ACCESSORY)
				{
					screen_turn_prev_page();
				}
				else
				{
					ret = true;	
				}					
            }
            else if (screen_get_act_pid() == PAGE_LIGHT_EFFECT_MENU)
            {
                ret = true;
            }
            else if (screen_get_act_pid() == PAGE_SIDUS_PRO_FX)
            {
                ret = true;
            }
			if (screen_get_act_pid() >= PAGE_LIGHT_MODE_HSI && screen_get_act_pid() <= PAGE_LIGHT_MODE_CCT)
            {
				if(!lv_group_get_editing(group))
				{
					obj_send_event_to_screen(LV_EVENT_KEY, &key);  
					ret = false;
				}
            }
            if (ret)
                light_mode_special_op_process();    
        } 
    }
}

/**
 * @brief 通用可调按钮的事件回调函数
 * 
 * @param e 
 */
void adj_btn_event_cb(lv_event_t* e)
{
    lv_obj_t* obj = lv_event_get_target(e);
    lv_event_code_t event = lv_event_get_code(e);

    shortcut_keys_event_handler(e);
    backspace_key_event_handler(e);
	
    if (event == LV_EVENT_KEY)
    {
        uint32_t key = *(uint32_t *)lv_event_get_param(e);
        adj_btn_t* user_data = lv_obj_get_user_data(obj);
        lv_group_t* group = lv_obj_get_group(obj);
        lv_indev_type_t indev_type = lv_indev_get_type(lv_indev_get_act());
        int32_t (*p_fn_enc_get_step)(uint32_t value) = user_data->get_step_cb == NULL ? comm_enc_get_step : user_data->get_step_cb;

        if ((indev_type == LV_INDEV_TYPE_ENCODER)&& user_data && lv_group_get_editing(group))
        {
            if (LV_ENCODER_KEY_CC == key)
            {        
				uint16_t step;        
				if(SUB_ID_ELE_FRESNEL == screen_get_act_spid()) 
				{
					struct sys_info_motor_state  motor_state;
	
					data_center_read_sys_info(SYS_INFO_MOTOR_STATE, &motor_state);
					
					step = 1;
					if(motor_state.fresnel_stall_state == 1)
						return;
				}
				else if(PAGE_ELECT_ACCESSORY == screen_get_act_pid())
					step = 1;
				else
					step = user_data->step * p_fn_enc_get_step(enc_get_step_index());
                int16_t value = user_data->value - step;
                
                if (value < user_data->min)
                {
                    if (user_data->cycle) {
                        value = user_data->max;
                    }
                    else {
                        value = user_data->min;
                    }
                }

                if (value != user_data->value)
                {
                    user_data->value = value;
                    if (user_data->update_run_cb)
                    {
                        user_data->update_run_cb(user_data);
                    }
                    lv_event_send(obj, LV_EVENT_VALUE_CHANGED, NULL);
                }            
            }
            else if (LV_ENCODER_KEY_CW == key)
            {             
				uint16_t step;
				
				if(SUB_ID_ELE_FRESNEL == screen_get_act_spid()) 
				{
					struct sys_info_motor_state  motor_state;
	
					data_center_read_sys_info(SYS_INFO_MOTOR_STATE, &motor_state);
					
					step = 1;
					if(motor_state.fresnel_stall_state == 1)
						return;
				}
				else if(PAGE_ELECT_ACCESSORY == screen_get_act_pid())
					step = 1;
				else
					step = user_data->step * p_fn_enc_get_step(enc_get_step_index());                
                int16_t value = user_data->value + step;

                if (value > user_data->max)
                {
                    if (user_data->cycle) {
                        value = user_data->min;
                    }
                    else {
                        value = user_data->max;
                    }
                }

                if (value != user_data->value)
                {
                    user_data->value = value;
                    lv_event_send(obj, LV_EVENT_VALUE_CHANGED, NULL);
                    if (user_data->update_run_cb)
                    {
                        user_data->update_run_cb(user_data);
                    }
                }
            }  
        }
        else if (lv_group_get_editing(group))
        {
            if (LV_KEY_BACKSPACE == key)
            {
                if (lv_group_get_editing(group))
                {
                    lv_group_set_editing(group, false);
                    adj_btn_t* user_data = lv_obj_get_user_data(obj);
                    if (user_data && user_data->cancel_op_cb)
                    {
                        user_data->cancel_op_cb(user_data);
                        user_data->finished_event_cb((void*)user_data);                        
                    }
                    lv_event_send(obj, LV_EVENT_VALUE_CHANGED, NULL);
                }
            }
        }            
    }
    else if (event == LV_EVENT_VALUE_CHANGED)
    {
        adj_btn_t* user_data = lv_obj_get_user_data(obj);
        if (user_data)
        {            
            if (user_data->refresh_btn_cb)
            {
                user_data->refresh_btn_cb(obj, user_data);            
            }
            else
            {
                adj_btn_refresh(obj, user_data);            
            }         
        }
    }
    else if (event == LV_EVENT_RELEASED)
    {
        UI_PRINTF("Released\r\n");
        lv_group_t* group = lv_obj_get_group(obj);
        adj_btn_t* user_data = lv_obj_get_user_data(obj);
        if (lv_group_get_editing(group))
        {
            lv_group_set_editing(group, false);
            if (user_data && user_data->finished_event_cb)
            {
                user_data->finished_event_cb((void*)user_data);
            }            
        }
        else
        {
            lv_group_set_editing(group, true);
        }
        lv_event_send(obj, LV_EVENT_VALUE_CHANGED, NULL);
    }
    else if (event == LV_EVENT_FOCUSED)
    {
        uint8_t child_sum = lv_obj_get_child_cnt(obj);
        for (uint8_t cnt = 0; cnt < child_sum; cnt++)
        {
            lv_obj_t* child = lv_obj_get_child(obj, cnt);
            lv_obj_add_state(child, LV_STATE_FOCUSED);
        }

    }
    else if (event == LV_EVENT_DEFOCUSED)
    {
        uint8_t child_sum = lv_obj_get_child_cnt(obj);
        for (uint8_t cnt = 0; cnt < child_sum; cnt++)
        {
            lv_obj_t* child = lv_obj_get_child(obj, cnt);
            lv_obj_clear_state(child, LV_STATE_FOCUSED);
        }        
    }
}

/**
 * @brief 通用可调按钮的事件回调函数
 * 
 * @param e 
 */
void adj_btn2_event_cb(lv_event_t* e)
{
    lv_obj_t* obj = lv_event_get_target(e);
    lv_event_code_t event = lv_event_get_code(e);

    shortcut_keys_event_handler(e);
    backspace_key_event_handler(e);

    if (event == LV_EVENT_KEY)
    {
        uint32_t key = *(uint32_t *)lv_event_get_param(e);
        adj_btn_t* user_data = lv_obj_get_user_data(obj);
        lv_group_t* group = lv_obj_get_group(obj);
        lv_indev_type_t indev_type = lv_indev_get_type(lv_indev_get_act());
        int32_t (*p_fn_enc_get_step)(uint32_t value) = user_data->get_step_cb == NULL ? comm_enc_get_step : user_data->get_step_cb;

        if ((indev_type == LV_INDEV_TYPE_ENCODER)&& user_data && lv_group_get_editing(group))
        {
            if (LV_ENCODER_KEY_CC == key)
            {        
				uint16_t step;        
				if(SUB_ID_ELE_FRESNEL == screen_get_act_spid() && PAGE_ELECT_ACCESSORY == screen_get_act_pid())
					step = 1;
				else
					step = user_data->step * 1;
                int16_t value = user_data->value - step;
                
                if (value < user_data->min)
                {
                    if (user_data->cycle) {
                        value = user_data->max;
                    }
                    else {
                        value = user_data->min;
                    }
                }

                if (value != user_data->value)
                {
                    user_data->value = value;
                    if (user_data->update_run_cb)
                    {
                        user_data->update_run_cb(user_data);
                    }
                    lv_event_send(obj, LV_EVENT_VALUE_CHANGED, NULL);
                }            
            }
            else if (LV_ENCODER_KEY_CW == key)
            {             
				uint16_t step;
				
				if(SUB_ID_ELE_FRESNEL == screen_get_act_spid() && PAGE_ELECT_ACCESSORY == screen_get_act_pid())
					step = 1;
				else
					step = user_data->step * p_fn_enc_get_step(enc_get_step_index());                
                int16_t value = user_data->value + step;

                if (value > user_data->max)
                {
                    if (user_data->cycle) {
                        value = user_data->min;
                    }
                    else {
                        value = user_data->max;
                    }
                }

                if (value != user_data->value)
                {
                    user_data->value = value;
                    lv_event_send(obj, LV_EVENT_VALUE_CHANGED, NULL);
                    if (user_data->update_run_cb)
                    {
                        user_data->update_run_cb(user_data);
                    }
                }
            }  
        }
        else if (lv_group_get_editing(group))
        {
            if (LV_KEY_BACKSPACE == key)
            {
                if (lv_group_get_editing(group))
                {
                    lv_group_set_editing(group, false);
                    adj_btn_t* user_data = lv_obj_get_user_data(obj);
                    if (user_data && user_data->cancel_op_cb)
                    {
                        user_data->cancel_op_cb(user_data);
                        user_data->finished_event_cb((void*)user_data);                        
                    }
                    lv_event_send(obj, LV_EVENT_VALUE_CHANGED, NULL);
                }
            }
        }            
    }
    else if (event == LV_EVENT_VALUE_CHANGED)
    {
        adj_btn_t* user_data = lv_obj_get_user_data(obj);
        if (user_data)
        {            
            if (user_data->refresh_btn_cb)
            {
                user_data->refresh_btn_cb(obj, user_data);            
            }
            else
            {
                adj2_btn_refresh(obj, user_data);            
            }         
        }
    }
    else if (event == LV_EVENT_RELEASED)
    {
        UI_PRINTF("Released\r\n");
        lv_group_t* group = lv_obj_get_group(obj);
        adj_btn_t* user_data = lv_obj_get_user_data(obj);
        if (lv_group_get_editing(group))
        {
            lv_group_set_editing(group, false);
            if (user_data && user_data->finished_event_cb)
            {
                user_data->finished_event_cb((void*)user_data);
            }           
        }
        else
        {
            lv_group_set_editing(group, true);
        }
        lv_event_send(obj, LV_EVENT_VALUE_CHANGED, NULL);
    }
    else if (event == LV_EVENT_FOCUSED)
    {
        uint8_t child_sum = lv_obj_get_child_cnt(obj);
        for (uint8_t cnt = 0; cnt < child_sum; cnt++)
        {
            lv_obj_t* child = lv_obj_get_child(obj, cnt);
            lv_obj_add_state(child, LV_STATE_FOCUSED);
        }
    }
    else if (event == LV_EVENT_DEFOCUSED)
    {
        uint8_t child_sum = lv_obj_get_child_cnt(obj);
        for (uint8_t cnt = 0; cnt < child_sum; cnt++)
        {
            lv_obj_t* child = lv_obj_get_child(obj, cnt);
            lv_obj_clear_state(child, LV_STATE_FOCUSED);
        }        
    }
}


lv_obj_t* intensity_bar_create(lv_obj_t* parent, lv_coord_t w, lv_coord_t h, lv_coord_t gap , lv_coord_t x_ofs)
{
    lv_obj_t* bar = lv_bar_create(parent);
    lv_obj_set_size(bar, w, h);
    lv_obj_set_style_bg_color(bar, INTENSITY_BAR_BG_COLOR, LV_PART_MAIN);
    lv_obj_set_style_bg_color(bar, INTENSITY_BAR_INDICATOR_COLOR, LV_PART_INDICATOR);
    lv_obj_set_style_radius(bar, 4, LV_PART_MAIN);
    lv_obj_set_style_radius(bar, 4, LV_PART_INDICATOR);
    lv_bar_set_range(bar, 0, UI_INTENSITY_MAX);

    lv_obj_t* label1 = lv_label_create(parent);
    lv_obj_set_style_text_font(label1, Font_ResouceGet(FONT_10), 0);
    lv_obj_t* label2 = lv_label_create(parent);
    lv_obj_set_style_text_font(label2, Font_ResouceGet(FONT_10), 0);
    lv_label_set_text(label1, "0%");
    lv_label_set_text(label2, "100%");

    lv_obj_align(bar, LV_ALIGN_CENTER, x_ofs, 0);
    lv_obj_align_to(label1, bar, LV_ALIGN_OUT_LEFT_MID, -gap, 0);
    lv_obj_align_to(label2, bar, LV_ALIGN_OUT_RIGHT_MID, gap, 0);

    return bar;
}

lv_obj_t* adj_btn_create(lv_obj_t* parent, lv_coord_t value , uint8_t type)
{
    lv_obj_t* obj = lv_obj_create(parent);
    lv_obj_add_style(obj, &style_common_btn, LV_STATE_DEFAULT);
    lv_obj_add_style(obj, &style_common_focued_btn, LV_STATE_FOCUSED);

    if (type == ADJ_BTN_TYPE_14) {
        lv_obj_set_style_text_font(obj, Font_ResouceGet(FONT_14_MEDIUM), 0);
        lv_obj_set_style_text_font(obj, Font_ResouceGet(FONT_14_BOLD), LV_STATE_EDITED);
    }
    else if (type == ADJ_BTN_TYPE_18) {
        lv_obj_set_style_text_font(obj, Font_ResouceGet(FONT_18_MEDIUM), 0);
        lv_obj_set_style_text_font(obj, Font_ResouceGet(FONT_18_BOLD), LV_STATE_EDITED);
    }
    else if (type == ADJ_BTN_TYPE_24) {
        lv_obj_set_style_text_font(obj, Font_ResouceGet(FONT_24_MEDIUM), 0);
        lv_obj_set_style_text_font(obj, Font_ResouceGet(FONT_24_BOLD), LV_STATE_EDITED);
    }   
    else  
    {
        lv_obj_set_style_text_font(obj, Font_ResouceGet(FONT_18_MEDIUM), 0);
        lv_obj_set_style_text_font(obj, Font_ResouceGet(FONT_18_BOLD), LV_STATE_EDITED);
    }

    lv_obj_set_style_text_color(obj, lv_color_make(255, 255, 255), LV_STATE_EDITED);
    lv_obj_set_style_radius(obj, value, 0);
    lv_obj_set_style_radius(obj, value, LV_STATE_FOCUSED);
    return obj;
}

void adj_btn_init(lv_obj_t* parent, adj_btn_t* dsc, uint32_t value)
{      
    lv_obj_t* label_title;
    lv_obj_t* label_div;
    lv_obj_t* label_val;
    const lv_font_t* font = lv_obj_get_style_text_font(parent, LV_STATE_DEFAULT);
    
    dsc->obj = parent;
    label_title = lv_label_create(parent);    
    lv_obj_set_style_text_font(label_title, font, 0);    

    label_div = lv_label_create(parent);    
    lv_obj_set_style_text_font(label_div, font, 0);

    label_val = lv_label_create(parent);        
    (void)label_val;
    /* 限幅 */
    if (dsc->value < dsc->min)
    {
        dsc->value = dsc->min;
    }
    else if (dsc->value > dsc->max)
    {
        dsc->value = dsc->max;
    }

    /* 赋值 */
    if (dsc->assign_value_cb)
    {
        dsc->assign_value_cb((void*)dsc);
    }

    if (dsc->refresh_btn_cb)
    {
        dsc->refresh_btn_cb(parent, dsc);            
    }
    else
    {
        adj_btn_refresh(parent, dsc);         
    }      
}

void adj2_btn_init(lv_obj_t* parent, adj_btn_t* dsc, uint32_t value)
{      
    lv_obj_t* label_title;
//    lv_obj_t* label_div;
    lv_obj_t* label_val;
    const lv_font_t* font = lv_obj_get_style_text_font(parent, LV_STATE_DEFAULT);
    
    dsc->obj = parent;
    label_title = lv_label_create(parent);    
    lv_obj_set_style_text_font(label_title, font, 0);    

    label_val = lv_label_create(parent);
	if(dsc->div_disable)
	{
		lv_obj_set_size(label_val, 109, 56);
		lv_obj_set_style_text_align(label_val, LV_TEXT_ALIGN_CENTER, 0);
		lv_label_set_long_mode(label_val, LV_LABEL_LONG_WRAP); 
	}        
    (void)label_val;
    /* 限幅 */
    if (dsc->value < dsc->min)
    {
        dsc->value = dsc->min;
    }
    else if (dsc->value > dsc->max)
    {
        dsc->value = dsc->max;
    }

    /* 赋值 */
    if (dsc->assign_value_cb)
    {
        dsc->assign_value_cb((void*)dsc);
    }
	
//	label_div = lv_label_create(parent);    
//    lv_obj_set_style_text_font(label_div, font, 0);
	
    if (dsc->refresh_btn_cb)
    {
        dsc->refresh_btn_cb(parent, dsc);            
    }
    else
    {
        adj2_btn_refresh(parent, dsc);         
    }      
}

void adj2_btn_refresh(lv_obj_t* parent, adj_btn_t* dsc)
{
    lv_obj_t* label_title = lv_obj_get_child(parent, 0);
    lv_obj_t* label_val = lv_obj_get_child(parent, 1);
//    lv_obj_t* label_uni = lv_obj_get_child(parent, 2);
//    lv_coord_t ofs = 0;
    lv_state_t state = lv_obj_get_state(parent); 

    lv_label_set_text(label_title, dsc->name);
    if (state & LV_STATE_EDITED)
    {
        lv_obj_set_style_text_color(label_title, lv_color_make(220, 220, 220), LV_STATE_FOCUSED);
    }
    else 
    {
        lv_obj_set_style_text_color(label_title, lv_color_make(255,255,255), LV_STATE_FOCUSED);
    }
	
    lv_obj_align(label_title, LV_ALIGN_TOP_MID, 0, 2);
	
	if(dsc->div_disable)
	{
		lv_obj_add_flag(label_title, LV_OBJ_FLAG_HIDDEN);
	}
	
	if (dsc->get_value_str_cb)
    {
        const char* str = dsc->get_value_str_cb((void*)dsc);
        lv_label_set_text(label_val, str);
    }    
	if (!dsc->div_disable)
		lv_obj_align(label_val, LV_ALIGN_BOTTOM_MID, 0, 3);
	
//    lv_label_set_text(label_uni, dsc->unit);
//    lv_obj_align_to(label_uni, label_val, LV_ALIGN_OUT_RIGHT_MID, 0, 3);   
//	if(dsc->div_disable)
//	{
//		lv_obj_add_flag(label_uni, LV_OBJ_FLAG_HIDDEN);
//	}
}


void adj_btn_refresh(lv_obj_t* parent, adj_btn_t* dsc)
{
    lv_obj_t* label_title = lv_obj_get_child(parent, 0);
    lv_obj_t* label_div = lv_obj_get_child(parent, 1);
    lv_obj_t* label_val = lv_obj_get_child(parent, 2);
    lv_coord_t ofs = 0;
    lv_state_t state = lv_obj_get_state(parent); 

    lv_label_set_text(label_title, dsc->name);
    if (state & LV_STATE_EDITED)
    {
        lv_obj_set_style_text_color(label_title, lv_color_make(220, 220, 220), LV_STATE_FOCUSED);
    }
    else 
    {
        lv_obj_set_style_text_color(label_title, lv_color_make(255,255,255), LV_STATE_FOCUSED);
    }

    lv_obj_align(label_title, LV_ALIGN_LEFT_MID, dsc->title_to_left, ofs);

    if (dsc->div_disable)
    {
        lv_obj_add_flag(label_div, LV_OBJ_FLAG_HIDDEN);
    }
    lv_label_set_text(label_div, ":");
    lv_obj_align(label_div, LV_ALIGN_LEFT_MID, dsc->div_to_left, ofs);
    
    if (dsc->get_value_str_cb)
    {
        const char* str = dsc->get_value_str_cb((void*)dsc);
        lv_label_set_text(label_val, str);
    }    

    if (state & LV_STATE_EDITED) 
    {
        ofs = 1;
    }
    else  
    {
        ofs = 0;
    }

    lv_obj_align(label_val, LV_ALIGN_RIGHT_MID, dsc->val_to_right, ofs);    
}

void adj_btn2_refresh(lv_obj_t* parent, adj_btn_t* dsc)
{
    lv_obj_t* label_title = lv_obj_get_child(parent, 0);
    lv_obj_t* label_div = lv_obj_get_child(parent, 1);
    lv_obj_t* label_val = lv_obj_get_child(parent, 2);
    lv_state_t state = lv_obj_get_state(parent); 

    if (state & LV_STATE_EDITED)
    {
        lv_obj_set_style_text_color(label_title, lv_color_make(220, 220, 220), LV_STATE_FOCUSED);
    }
    else 
    {
        lv_obj_set_style_text_color(label_title, lv_color_make(255,255,255), LV_STATE_FOCUSED);
    }

//    lv_obj_set_style_text_color(label_title, lv_color_make(202, 202, 202), 0);
    lv_label_set_text(label_title, dsc->name);
    lv_obj_align(label_title, LV_ALIGN_TOP_MID, 0, 5);

    if (dsc->div_disable)
    {
        lv_obj_add_flag(label_div, LV_OBJ_FLAG_HIDDEN);
    }
    lv_label_set_text(label_div, ":");
    lv_obj_align(label_div, LV_ALIGN_LEFT_MID, dsc->div_to_left, 0);
    
    if (dsc->get_value_str_cb)
    {
        const char* str = dsc->get_value_str_cb((void*)dsc);        
        lv_label_set_text(label_val, str);
    }    

    lv_obj_align(label_val, LV_ALIGN_BOTTOM_MID, 0, -3);
}

void adj_btn3_refresh(lv_obj_t* parent, adj_btn_t* dsc)
{
    lv_obj_t* label_title = lv_obj_get_child(parent, 0);
    lv_obj_t* label_div = lv_obj_get_child(parent, 1);
    lv_obj_t* label_val = lv_obj_get_child(parent, 2);
    lv_state_t state = lv_obj_get_state(parent);

    lv_label_set_text(label_title, dsc->name);
    if (state & LV_STATE_EDITED)
    {
		lv_obj_set_style_text_color(label_title, lv_color_make(255, 255, 255), LV_STATE_FOCUSED);
    }
    else
    {
        lv_obj_set_style_text_color(label_title, lv_color_make(220, 220, 220), LV_STATE_FOCUSED);
    }

    lv_obj_align(label_title, LV_ALIGN_CENTER, dsc->title_to_left, 20);

    if (dsc->div_disable)
    {
        lv_obj_add_flag(label_div, LV_OBJ_FLAG_HIDDEN);
    }
    lv_label_set_text(label_div, ":");
    lv_obj_align(label_div, LV_ALIGN_LEFT_MID, dsc->div_to_left, 20);

    if (dsc->get_value_str_cb)
    {
		//angle = dsc->value/10.0;
		//sprintf(str, "%.1f°", angle);
//        const char* str = dsc->get_value_str_cb((void*)dsc);
	   // lv_label_set_text(label_val, str);
    }
	lv_obj_set_style_text_font(label_val, Font_ResouceGet(FONT_22_BOLD), 0);
    lv_obj_align_to(label_val, label_title, LV_ALIGN_OUT_TOP_MID, dsc->val_to_right, -10);
}


void adj_btn4_refresh(lv_obj_t* parent, adj_btn_t* dsc)
{
    lv_obj_t* circle = lv_obj_get_child(parent, 0);
    lv_obj_t* arc = lv_obj_get_child(circle, 0);
    uint16_t src_int = 0;
    lv_obj_t* label_val = lv_obj_get_child(parent, 2);
//    lv_coord_t ofs = 0;
    lv_state_t state = lv_obj_get_state(parent); 
    
    if (dsc->get_value_str_cb)
    {
        const char* str = dsc->get_value_str_cb((void*)dsc);
        lv_label_set_text(label_val, str);
            src_int = dsc->value;
        if(src_int != 0 && src_int <= 3)
            src_int = 3;
        else if(src_int != 1000 && src_int >= 913)
            src_int = 913;
        lv_arc_set_value(arc, src_int);
    }    
    lv_obj_align_to(label_val, circle, LV_ALIGN_CENTER, 0, 0);   
}

void adj_cct_refresh(lv_obj_t* parent, adj_btn_t* dsc)
{
    lv_obj_t* label_val = lv_obj_get_child(parent, 2);
   
    if (dsc->get_value_str_cb)
    {
        const char* str = dsc->get_value_str_cb((void*)dsc);
        lv_label_set_text(label_val, str);
    }    
}

void adj_btn5_refresh(lv_obj_t* parent, adj_btn_t* dsc)
{
    lv_obj_t* circle = lv_obj_get_child(parent, 0);
    lv_obj_t* label_val = lv_obj_get_child(parent, 2);
    //lv_obj_t* frq_cont = lv_obj_get_child(parent, 3);
    lv_obj_t* frq_img;
    uint8_t index = 0;
    
//    lv_coord_t ofs = 0;
    lv_state_t state = lv_obj_get_state(parent); 
   
    if (dsc->get_value_str_cb)
    {
        const char* str = dsc->get_value_str_cb((void*)dsc);
        lv_label_set_text(label_val, str);
    }    
    lv_obj_align_to(label_val, circle, LV_ALIGN_CENTER, 0, 0);
    
    if(dsc->value <= 10)
    {
        for(uint8_t i = 0; i < 10; i++)
        {
            index = 3+i <= 9 ? (3+i) : (i-7);            
            frq_img = lv_obj_get_child(parent, index + 3);
            if(i < dsc->value)
                lv_obj_set_style_img_recolor(frq_img, lv_color_white(), 0);
            else
               lv_obj_set_style_img_recolor(frq_img, lv_color_make(0x4d, 0x4d, 0x4d), 0); 
        }
    }
    else if(dsc->value == 11)
    {
        for(uint8_t i = 0; i < 10; i++)
        {
            frq_img = lv_obj_get_child(parent, i + 3);
            if((i % 2) == 0)
                lv_obj_set_style_img_recolor(frq_img, lv_color_white(), 0);
            else
               lv_obj_set_style_img_recolor(frq_img, lv_color_make(0x4d, 0x4d, 0x4d), 0); 
        }    
    }
    
    
//    if (state & LV_STATE_EDITED) 
//    {
//        ofs = 1;
//    }
//    else  
//    {
//        ofs = 0;
//    }

//    lv_obj_align(label_val, LV_ALIGN_RIGHT_MID, dsc->val_to_right, ofs);    
}

bool adj_btn_run_assign_value_cb(adj_btn_t* p_adj_btn)
{
    if (p_adj_btn && p_adj_btn->assign_value_cb)
    {
        return p_adj_btn->assign_value_cb(p_adj_btn);
    }
    return false;
}

void adj_btn_run_update_cb(adj_btn_t* p_adj_btn)
{
    if (p_adj_btn && p_adj_btn->update_run_cb)
    {
        p_adj_btn->update_run_cb(p_adj_btn);
    }    
}

void adj_btn_refresh_all(adj_btn_t* p_btn_dsc, uint8_t num, bool force_upd)
{
    for (uint8_t i = 0; i < num; i++)
    {        
        if (adj_btn_run_assign_value_cb(&p_btn_dsc[i]) || force_upd)
        {
            if (p_btn_dsc[i].obj != NULL) //chen
            {
                lv_event_send(p_btn_dsc[i].obj, LV_EVENT_VALUE_CHANGED, NULL);
            }
            adj_btn_run_update_cb(&p_btn_dsc[i]);
        }
    }
}

// static void mask_event_cb(lv_event_t * e)
// {
//     lv_event_code_t code = lv_event_get_code(e);
//     lv_obj_t * obj = lv_event_get_target(e);

//     static int16_t mask_top_id = -1;
//     static int16_t mask_bottom_id = -1;

//     if (code == LV_EVENT_COVER_CHECK) {
//         lv_event_set_cover_res(e, LV_COVER_RES_MASKED);

//     } else if (code == LV_EVENT_DRAW_MAIN_BEGIN) {
//         /* add mask */
//         const lv_font_t * font = lv_obj_get_style_text_font(obj, LV_PART_MAIN);
//         lv_coord_t line_space = lv_obj_get_style_text_line_space(obj, LV_PART_MAIN);
//         lv_coord_t font_h = lv_font_get_line_height(font);

//         lv_area_t roller_coords;
//         lv_obj_get_coords(obj, &roller_coords);

//         lv_area_t rect_area;
//         rect_area.x1 = roller_coords.x1;
//         rect_area.x2 = roller_coords.x2;
//         rect_area.y1 = roller_coords.y1;
//         rect_area.y2 = roller_coords.y1 + (lv_obj_get_height(obj) - font_h - line_space) / 2;

//         lv_draw_mask_fade_param_t * fade_mask_top = lv_mem_buf_get(sizeof(lv_draw_mask_fade_param_t));
//         lv_draw_mask_fade_init(fade_mask_top, &rect_area, LV_OPA_TRANSP, rect_area.y1, LV_OPA_COVER, rect_area.y2);
//         mask_top_id = lv_draw_mask_add(fade_mask_top, NULL);

//         rect_area.y1 = rect_area.y2 + font_h + line_space - 1;
//         rect_area.y2 = roller_coords.y2;

//         lv_draw_mask_fade_param_t * fade_mask_bottom =lv_mem_buf_get(sizeof(lv_draw_mask_fade_param_t));
//         lv_draw_mask_fade_init(fade_mask_bottom, &rect_area, LV_OPA_COVER, rect_area.y1, LV_OPA_TRANSP, rect_area.y2);
//         mask_bottom_id = lv_draw_mask_add(fade_mask_bottom, NULL);

//     } else if (code == LV_EVENT_DRAW_POST_END) {
//         lv_draw_mask_fade_param_t * fade_mask_top = lv_draw_mask_remove_id(mask_top_id);
//         lv_draw_mask_fade_param_t * fade_mask_bottom = lv_draw_mask_remove_id(mask_bottom_id);
//         lv_mem_buf_release(fade_mask_top);
//         lv_mem_buf_release(fade_mask_bottom);
//         //lv_draw_mask_free_param(&fade_mask_top);
//         //lv_draw_mask_free_param(&fade_mask_bottom);
//         //lv_draw_mask_free_param()
//         mask_top_id = -1;
//         mask_bottom_id = -1;
//     }
// }
// static lv_group_t* local_group;
// static void event_handler(lv_event_t * e)
// {
//     lv_event_code_t code = lv_event_get_code(e);
//     lv_obj_t * obj = lv_event_get_target(e);
//     if(code == LV_EVENT_VALUE_CHANGED) {
//         char buf[32];
//         lv_roller_get_selected_str(obj, buf, sizeof(buf));
//         LV_LOG_USER("Selected value: %s", buf);
//     }
//     else if (code == LV_EVENT_KEY)
//     {
//         uint32_t key = *(uint32_t *)lv_event_get_param(e);
//         UI_PRINTF("Key_value: %d\r\n", key);
//     }
//     else if (code == LV_EVENT_RELEASED)
//     {
//         // lv_group_t* group = lv_obj_get_group(obj);
//         // if (lv_group_get_editing(group) == false)
//         // {
//         //     lv_group_set_editing(group, true);
//         // }
//         // else
//         // {
//         //     lv_group_set_editing(group, false);
//         // }
//     }
// }

// void batt_anim_cb(void* a, int32_t value)
// {
//     lv_obj_set_size(a, 8, value);
//     lv_obj_align(a, LV_ALIGN_BOTTOM_MID, 0, -2);
// }

//ui_power_dsc_t power_dsc;





struct _list_ext {
    lv_obj_t* list;
    int8_t cur_focused_index;
    int8_t sum;
};

static void clear_children_state(lv_obj_t* parent, lv_state_t mask)
{
    int8_t sum = lv_obj_get_child_cnt(parent);
    int8_t i;
    lv_obj_t* child;

    for (i=0; i<sum; i++) {
        child = lv_obj_get_child(parent, i);
        lv_obj_clear_state(child, mask);
    }
}

static void add_children_state(lv_obj_t* parent, lv_state_t mask)
{
    int8_t sum = lv_obj_get_child_cnt(parent);
    int8_t i;
    lv_obj_t* child;

    for (i=0; i<sum; i++) {
        child = lv_obj_get_child(parent, i);
        lv_obj_add_state(child, mask);
    }
}

static void add_state_for_child(lv_obj_t* parent, int8_t index, lv_state_t mask)
{
    int8_t sum = lv_obj_get_child_cnt(parent);
    lv_obj_t* child;

    if (index >= sum) 
        return;

    child = lv_obj_get_child(parent, index);
    lv_obj_add_state(child, mask);
    add_children_state(child, mask);
    lv_obj_invalidate(child);

    if (mask & LV_STATE_FOCUSED)
        lv_obj_scroll_to_view(child, LV_ANIM_ON);
}

static void clear_state_for_child(lv_obj_t* parent, int8_t index, lv_state_t mask)
{
    int8_t sum = lv_obj_get_child_cnt(parent);
    lv_obj_t* child;

    if (index >= sum) 
        return;

    child = lv_obj_get_child(parent, index);
    lv_obj_clear_state(child, mask);
    clear_children_state(child, mask);
    lv_obj_invalidate(child);
}

static void list_event(lv_event_t* e)
{
    lv_event_code_t event = lv_event_get_code(e);
    lv_obj_t* p_list = lv_event_get_target(e);
    struct _list_ext* p_ext = lv_obj_get_user_data(p_list);

    if (LV_EVENT_FOCUSED == event) 
    {        
        clear_children_state(p_list, LV_STATE_FOCUSED);
        add_state_for_child(p_list, p_ext->cur_focused_index, LV_STATE_FOCUSED);        
    } 
    else if (LV_EVENT_KEY == event) 
    {
        uint32_t key = lv_event_get_key(e);
        int8_t index = p_ext->cur_focused_index;

        if (key == LV_KEY_UP) 
        {
            index -= 1;
            if (index < 0) 
            {
                return;
            }         

            clear_state_for_child(p_list, p_ext->cur_focused_index, LV_STATE_FOCUSED);
            p_ext->cur_focused_index = index;
            add_state_for_child(p_list, p_ext->cur_focused_index, LV_STATE_FOCUSED);
        }   
        else if (key == LV_KEY_DOWN)
        {
            index += 1;
            if (index >= p_ext->sum) 
            {                
                return;
            }         

            clear_state_for_child(p_list, p_ext->cur_focused_index, LV_STATE_FOCUSED);
            p_ext->cur_focused_index = index;
            add_state_for_child(p_list, p_ext->cur_focused_index, LV_STATE_FOCUSED);
        }     
    }
}

lv_obj_t* list_create(lv_obj_t* parent, struct _list_ext* p_ext)
{
    p_ext->list = lv_obj_create(parent);
    lv_obj_set_flex_flow(p_ext->list , LV_FLEX_FLOW_COLUMN);
    lv_obj_set_scroll_snap_y(p_ext->list , LV_SCROLL_SNAP_CENTER);   
    lv_obj_add_event_cb(p_ext->list, list_event, LV_EVENT_ALL, NULL);
    lv_obj_set_user_data(p_ext->list, p_ext);
    p_ext->cur_focused_index = 0;
    p_ext->sum = 0;
    return p_ext->list;
}

void list_add_label(struct _list_ext* p_ext, const char* name)
{
    lv_obj_t* cont = lv_obj_create(p_ext->list);
    lv_obj_set_size(cont, lv_pct(100), LV_SIZE_CONTENT);
    lv_obj_set_style_pad_all(cont, 0, 0);
    lv_obj_set_style_height(cont, LV_SIZE_CONTENT, LV_STATE_FOCUSED);
    lv_obj_set_style_bg_color(cont, lv_color_make(255, 0, 0), 0);
    lv_obj_set_style_bg_color(cont, lv_color_make(0, 255, 0), LV_STATE_FOCUSED);
    lv_obj_set_style_text_font(cont, Font_ResouceGet(FONT_14), 0);
    lv_obj_set_style_text_color(cont, lv_color_make(0, 0, 0), 0);
    lv_obj_set_style_text_font(cont, Font_ResouceGet(FONT_32), LV_STATE_FOCUSED);
    lv_obj_set_style_text_color(cont, lv_color_make(255, 255, 255), LV_STATE_FOCUSED);

    lv_obj_t* label = lv_label_create(cont);
    lv_label_set_text(label, "2333");            
    lv_obj_center(label);    
    p_ext->sum++;
}

struct _list_ext my_list_ext;

static void list_test(void)
{
    lv_group_t* group = lv_group_create();    
    my_indev_set_group(group);

    lv_obj_t* list = list_create(lv_scr_act(), &my_list_ext);
    lv_obj_set_size(list, lv_pct(50), lv_pct(50));
    lv_obj_center(list);
    
    char tmp[30];
    for (int i=0; i < 12; i++) {   
        lv_snprintf(tmp, sizeof(tmp), "%d", i);
        list_add_label(&my_list_ext, tmp);        
    }

    lv_group_add_obj(group, list);
    lv_group_focus_obj(list);
    lv_group_set_editing(group, true);
}

/**
 * Add an fade mask to roller.
 */
void roller_test(void)
{
//  static lv_style_t style;
//     lv_style_init(&style);
//     lv_style_set_bg_color(&style, lv_color_black());
//     lv_style_set_text_color(&style, lv_color_white());
//     lv_style_set_border_width(&style, 0);
//     lv_style_set_pad_all(&style, 0);
//     lv_obj_add_style(lv_scr_act(), &style, 0);

//     lv_obj_t *roller1 = lv_roller_create(lv_scr_act());
//     lv_obj_add_style(roller1, &style, 0);
//     lv_obj_set_style_bg_opa(roller1, LV_OPA_TRANSP, LV_PART_SELECTED);

// #if LV_FONT_MONTSERRAT_22
//     lv_obj_set_style_text_font(roller1, &lv_font_montserrat_22, LV_PART_SELECTED);
// #endif

//     lv_roller_set_options(roller1,
//                         "January\n"
//                         "February\n"
//                         "March\n"
//                         "April\n"
//                         "May\n"
//                         "June\n"
//                         "July\n"
//                         "August\n"
//                         "September\n"
//                         "October\n"
//                         "November\n"
//                         "December",
//                         LV_ROLLER_MODE_NORMAL);

//     lv_obj_center(roller1);
//     lv_roller_set_visible_row_count(roller1, 3);
//     lv_obj_add_event_cb(roller1, mask_event_cb, LV_EVENT_ALL, NULL);

////2
///**
// * Roller with various alignments and larger text in the selected area
// */
//     local_group = lv_group_create();

//      /*A style to make the selected option larger*/
//      static lv_style_t style_sel;
//      lv_style_init(&style_sel);
//      lv_style_set_text_font(&style_sel, &lv_font_montserrat_22);

//      const char * opts = "HSI\nCCT\nX Y";
//      lv_obj_t *roller;

//      /*A roller on the left with left aligned text, and custom width*/
//      roller = lv_roller_create(lv_scr_act());
//      lv_roller_set_options(roller, opts, LV_ROLLER_MODE_NORMAL);
//      lv_roller_set_visible_row_count(roller, 2);
//      lv_obj_set_width(roller, 100);
//      lv_obj_add_style(roller, &style_sel, LV_PART_SELECTED);
//      lv_obj_set_style_text_align(roller, LV_TEXT_ALIGN_LEFT, 0);
//      lv_obj_align(roller, LV_ALIGN_LEFT_MID, 10, 0);
//      lv_obj_add_event_cb(roller, event_handler, LV_EVENT_ALL, NULL);
//      lv_roller_set_selected(roller, 2, LV_ANIM_OFF);

//      /*A roller on the middle with center aligned text, and auto (default) width*/
//      roller = lv_roller_create(lv_scr_act());
//      lv_roller_set_options(roller, opts, LV_ROLLER_MODE_NORMAL);
//      lv_roller_set_visible_row_count(roller, 3);
//      lv_obj_add_style(roller, &style_sel, LV_PART_SELECTED);
//      lv_obj_align(roller, LV_ALIGN_CENTER, 0, 0);
//      lv_obj_add_event_cb(roller, event_handler, LV_EVENT_ALL, NULL);
//      lv_roller_set_selected(roller, 5, LV_ANIM_OFF);
//      lv_group_add_obj(local_group, roller);

//      /*A roller on the right with right aligned text, and custom width*/
//      roller = lv_roller_create(lv_scr_act());
//      lv_roller_set_options(roller, opts, LV_ROLLER_MODE_INFINITE);
//      lv_roller_set_visible_row_count(roller, 4);
//      lv_obj_set_width(roller, 80);
//      lv_obj_set_height(roller, 90);
//      lv_obj_add_style(roller, &style_sel, LV_PART_SELECTED);

//      lv_obj_set_style_pad_bottom(roller, 0, 0);
//  #define ROLLER_SEL_PART_FOCUED_COLOR
//  #define ROLLER_SEL_PART_FOCUSED_EDITED_COLOR
//  #define ROLLER_MAIN_PART_FOCUSED_COLOR

//      lv_obj_set_style_text_align(roller, LV_TEXT_ALIGN_CENTER, 0);
//      lv_obj_set_style_bg_color(roller, lv_color_make(244, 23, 55), LV_PART_SELECTED | LV_STATE_FOCUSED);
//      lv_obj_set_style_bg_color(roller, lv_color_make(0, 255, 255), LV_PART_SELECTED | LV_STATE_FOCUSED | LV_STATE_EDITED);
//      lv_obj_set_style_bg_color(roller, lv_color_make(40, 40, 40), LV_PART_MAIN);

//      lv_obj_set_style_text_color(roller, lv_color_make(255, 255, 255), LV_PART_MAIN);
//      lv_obj_set_style_text_color(roller, lv_color_make(0, 0, 0), LV_PART_MAIN | LV_STATE_FOCUSED);
//      lv_obj_set_style_bg_color(roller, lv_color_make(255, 255, 255), LV_PART_MAIN | LV_STATE_FOCUSED);

//      lv_obj_align(roller, LV_ALIGN_RIGHT_MID, -10, 0);
//      lv_obj_add_event_cb(roller, event_handler, LV_EVENT_ALL, NULL);
//      lv_roller_set_selected(roller, 8, LV_ANIM_OFF);

//      lv_group_add_obj(local_group, roller);
//      my_indev_set_group(local_group);
//      lv_group_set_default(local_group);
//      lv_group_set_editing(local_group, true);

    //lv_obj_t* roller = lightmode_roller_create(lv_scr_act(), lv_pct(50), lv_pct(50));

    //lv_obj_center(roller);
    //   lv_group_add_obj(local_group, roller);
    //my_indev_set_group(local_group);
    //lv_group_set_default(local_group);
    
 
    //local_group = lv_group_create();

    //lv_obj_t* obj_panel = lv_obj_create(lv_scr_act());
    //lv_obj_set_size(obj_panel, 100, 200);
    //lv_obj_set_flex_flow(obj_panel, LV_FLEX_FLOW_COLUMN);
    //lv_obj_center(obj_panel);

    //for (int i = 0; i < 3; i++)
    //{
    //    lv_obj_t* obj = lv_obj_create(obj_panel);
    //    lv_obj_set_size(obj, lv_pct(100), lv_pct(33));

    //    lv_obj_t* label = lv_label_create(obj);
    //    lv_label_set_text(label, "sdf");
    //}

    //lv_group_add_obj(local_group, obj_panel);
    //my_indev_set_group(local_group);
    //lv_group_set_default(local_group);
    lv_obj_set_style_bg_color(lv_scr_act(), lv_color_black(), 0);

    // LV_IMG_DECLARE(ImgBatteryFrame)

    // lv_obj_t* rect = lv_obj_create(lv_scr_act());
    // lv_obj_clear_flag(rect, LV_OBJ_FLAG_SCROLLABLE);
    // lv_obj_add_style(rect, &style_rect_panel, 0);    
    // lv_obj_set_style_bg_img_src(rect, &ImgBatteryFrame, 0);
    // lv_obj_set_style_bg_img_recolor_opa(rect, LV_OPA_100, 0);
    // lv_obj_set_style_bg_img_recolor(rect, lv_color_make(255, 0, 0), 0);
    // lv_obj_set_size(rect, 12, 21);
    // lv_obj_center(rect);

    // lv_obj_t* power = lv_obj_create(rect);
    // lv_obj_clear_flag(power, LV_OBJ_FLAG_SCROLLABLE);
    // //lv_obj_remove_style_all(power);    
    // lv_obj_add_style(power, &style_rect_panel, 0);
    // lv_obj_set_style_radius(power, 0, 0);
    // lv_obj_set_style_bg_color(power, lv_color_make(255,255, 0), 0);
    // lv_obj_set_size(power, 8, 10);
    // lv_obj_align(power, LV_ALIGN_BOTTOM_MID, 0, -1);

    // LV_IMG_DECLARE(ImgChargeIcon)
    // lv_obj_t* img = lv_img_create(rect);
    // lv_img_set_src(img, &ImgChargeIcon);
    // lv_obj_set_style_img_recolor(img, BATT_FRAME_NORMAL_POWER_COLOR, 0);
    // lv_obj_set_style_img_recolor_opa(img, LV_OPA_100, 0);
    // lv_obj_center(img);

    // lv_anim_t a;
    // lv_anim_init(&a);
    // lv_anim_set_var(&a, power);
    // lv_anim_set_values(&a, 1, 15);
    // lv_anim_set_exec_cb(&a, batt_anim_cb);
    // lv_anim_set_time(&a, 2000);
    // lv_anim_set_repeat_count(&a, LV_ANIM_REPEAT_INFINITE);
    // lv_anim_start(&a);

    lv_obj_t* title = lv_obj_create(lv_scr_act());
    lv_obj_remove_style_all(title);
    lv_obj_set_size(title, 320, 40);

    //power_cont_create(title, &power_dsc);

    list_test();
}

// static void inf_normalize(lv_obj_t * obj)
// {
//     (void)obj;
// }


uint8_t g8_diff = 0;
#include <math.h>

void custom_roller_event_cb(lv_event_t* e)
{
    lv_obj_t* obj = lv_event_get_target(e);
    lv_event_code_t event = lv_event_get_code(e);


    if (LV_EVENT_KEY == event) {
        uint32_t key_value = *(uint32_t *)lv_event_get_param(e);
        if (key_value == LV_KEY_LEFT) {
            UI_PRINTF("KEY LEFT\r\n");     
        }
        else if (key_value == LV_KEY_RIGHT) {
            UI_PRINTF("KEY RIGHT\r\n");
        }
    }
    else if (LV_EVENT_DRAW_POST == event) {
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
        uint8_t diff = g8_diff;

        for (uint8_t i = 0; i < 4; i++)
        {
            angle = 0 + i * 45;
               
            p1.x = center.x + (lv_coord_t)((radius + h) * 1000 * lv_trigo_sin(angle) >> LV_TRIGO_SHIFT) / 1000;
            p1.y = center.y - (lv_coord_t)((radius + h) * 1000 * lv_trigo_cos(angle) >> LV_TRIGO_SHIFT) / 1000;

            poly[0].x = p1.x - (lv_coord_t)(w * 1000 * lv_trigo_cos(angle) >> LV_TRIGO_SHIFT)  / 1000 / 2;
            poly[0].y = p1.y - (lv_coord_t)(w * 1000 * lv_trigo_sin(angle) >> LV_TRIGO_SHIFT)  / 1000 / 2 ;
            poly[1].x = p1.x + (lv_coord_t)(w * 1000 * lv_trigo_cos(angle) >> LV_TRIGO_SHIFT)  / 1000 / 2;
            poly[1].y = p1.y + (lv_coord_t)(w * 1000 * lv_trigo_sin(angle) >> LV_TRIGO_SHIFT)  / 1000 / 2;
            
            p1.x = center.x + (lv_coord_t)((radius) * 1000 * lv_trigo_sin(angle) >> LV_TRIGO_SHIFT) / 1000;
            p1.y = center.y - (lv_coord_t)((radius) * 1000 * lv_trigo_cos(angle) >> LV_TRIGO_SHIFT) / 1000;


            poly[2].x = p1.x + (lv_coord_t)(w * 1000  * lv_trigo_cos(angle) >> LV_TRIGO_SHIFT) / 1000 / 2;
            poly[2].y = p1.y + (lv_coord_t)(w * 1000  * lv_trigo_sin(angle) >> LV_TRIGO_SHIFT) / 1000 / 2;
            poly[3].x = p1.x - (lv_coord_t)(w * 1000  * lv_trigo_cos(angle) >> LV_TRIGO_SHIFT) / 1000 / 2;
            poly[3].y = p1.y - (lv_coord_t)(w * 1000  * lv_trigo_sin(angle) >> LV_TRIGO_SHIFT) / 1000 / 2;


            uint8_t index = i;
            if (index == diff)
                rect_dsc.bg_color = lv_obj_get_style_bg_color(obj, 0);
            else
                rect_dsc.bg_color = lv_color_make(255, 0, 0);

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
                rect_dsc.bg_color = lv_color_make(255, 0, 0);

            lv_draw_polygon(poly, 4, clip_area, &rect_dsc);
        }    
    }
}


lv_obj_t* roller_add_label(lv_obj_t* parent, const char* text)
{

    lv_obj_t* obj = lv_obj_create(parent);
    lv_obj_set_width(obj, lv_pct(100));
    lv_obj_set_height(obj, 30);
    lv_obj_set_style_bg_color(obj, lv_color_make(0, 255, 0), 0);

    lv_obj_t* label = lv_label_create(obj);
    lv_label_set_text(label, text);
    lv_obj_set_style_text_color(label, lv_color_make(255, 0, 0), 0);
    lv_obj_center(label);
    return label;
}

void wait_anim_exec_xcb(void* a, int32_t value)
{
    g8_diff = value;
    lv_obj_invalidate(a);
}


static lv_group_t* gt_indev_group = NULL;
void my_custom_roller_init(void)
{
    gt_indev_group = lv_group_create();
    my_indev_set_group(gt_indev_group);
    lv_group_set_default(gt_indev_group);

    lv_obj_t* custom_roller = lv_obj_create(lv_scr_act());
    lv_obj_set_size(custom_roller, lv_pct(70), lv_pct(70));
    lv_obj_center(custom_roller);
    lv_obj_add_event_cb(custom_roller, custom_roller_event_cb, LV_EVENT_ALL, NULL);
    
    //roller_add_label(custom_roller, "Hello");

     lv_anim_t a;
     lv_anim_init(&a);
     lv_anim_set_var(&a, custom_roller);
     lv_anim_set_values(&a, 0, 7);
     lv_anim_set_exec_cb(&a, wait_anim_exec_xcb);
     lv_anim_set_time(&a, 500);
     lv_anim_set_repeat_count(&a, LV_ANIM_REPEAT_INFINITE);
     lv_anim_start(&a);

    lv_group_add_obj(gt_indev_group, custom_roller);
    lv_group_focus_obj(custom_roller);
    lv_group_set_editing(gt_indev_group, true);
}
