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
static void my_list_event_cb(lv_event_t* e);
static void my_list_btn_event_cb(lv_event_t* e);
static void ip_btn_event_cb(lv_event_t* e);
static void ip_btn_label_add_state(lv_obj_t* btn, uint8_t index);
static void ip_btn_label_clear_state(lv_obj_t* btn, uint8_t index);
static void ip_btn_refresh(lv_obj_t* btn);
/*********************
 *  STATIC VATIABLES
 *********************/

/*********************
 *  GLOBAL VATIABLES
 *********************/

/*********************
 *  GLOBAL FUNCTIONS
 *********************/

lv_obj_t* my_radiobox_list_create(lv_obj_t* parent, lv_coord_t w, my_radiobox_ext_t* p_ext, uint8_t type, lv_align_t align)
{
    lv_obj_t* radiobox_panel = lv_obj_create(parent);
    lv_coord_t height = 0;
    lv_coord_t gap = 5;
    lv_coord_t items_height = 40;

    height = items_height * p_ext->items_of_page + gap * (p_ext->items_of_page - 1) + 2 * gap;
    p_ext->type = type;
    p_ext->default_val = p_ext->sel;

    lv_obj_add_style(radiobox_panel, &style_rect_panel, 0);
    lv_obj_clear_flag(radiobox_panel, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_flag(radiobox_panel, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
    lv_obj_set_style_bg_color(radiobox_panel, lv_color_black(), 0);
    lv_obj_set_flex_flow(radiobox_panel, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_pad_gap(radiobox_panel, gap, 0);
    lv_obj_set_style_pad_ver(radiobox_panel, gap, 0);
    lv_obj_set_size(radiobox_panel, w, height);

    for (int i = 0; i < p_ext->max; i++)
    {
        lv_obj_t* cont = lv_obj_create(radiobox_panel);
        lv_obj_add_style(cont, &style_common_btn, 0);
        lv_obj_add_style(cont, &style_common_focued_btn, LV_STATE_FOCUSED);
        lv_obj_set_style_text_font(cont, Font_ResouceGet(FONT_18), 0);
        lv_obj_set_size(cont, lv_pct(100), items_height);

        const char* str = "sdf";        
        if (p_ext->get_name_str) {
            str = p_ext->get_name_str(i);           
        }

        lv_obj_t* name = lv_label_create(cont);
        lv_obj_set_width(name, LV_SIZE_CONTENT);
        lv_obj_set_height(name, lv_font_get_line_height(Font_ResouceGet(FONT_18)));
        lv_label_set_long_mode(name, LV_LABEL_LONG_CLIP);     

        lv_obj_t* value = lv_label_create(cont);
        lv_obj_set_height(value, lv_font_get_line_height(Font_ResouceGet(FONT_18)));
        lv_label_set_long_mode(value, LV_LABEL_LONG_CLIP);     

        lv_obj_t* img = lv_img_create(cont);
        lv_obj_set_style_img_recolor(img,  lv_color_white(), 0);
        lv_obj_set_style_img_recolor_opa(img, LV_OPA_100, 0);
        lv_obj_set_style_img_recolor(img,  lv_color_white(), LV_STATE_FOCUSED);
        lv_obj_set_style_img_recolor_opa(img, LV_OPA_100, LV_STATE_FOCUSED);
     
        LV_IMG_DECLARE(ImgRadioboxSel)
        LV_IMG_DECLARE(ImgRadioboxNoSel)
        if (p_ext->sel == i)
        {
            lv_img_set_src(img, &ImgRadioboxSel);
        }
        else
        {
            lv_img_set_src(img, &ImgRadioboxNoSel);
        }
        lv_obj_align(img, LV_ALIGN_RIGHT_MID, -3, 0);
        
        // 刷新RadioBox
        if (MY_RADIOBOX_TYPE1 == type)
        {
            lv_label_set_text_fmt(name, "%d.", i + 1);
            lv_obj_align(name, LV_ALIGN_LEFT_MID, 10, 0);
            
            lv_obj_set_width(value, lv_pct(72));
            lv_label_set_text(value, str);
            lv_obj_align_to(value, name, LV_ALIGN_OUT_RIGHT_MID, 5, 0);                    
        }
        else 
        {
            lv_obj_add_flag(name, LV_OBJ_FLAG_HIDDEN);
            lv_obj_set_width(value, lv_pct(85));
            lv_label_set_text(value, str);

            if (align == LV_ALIGN_CENTER)
            {
                lv_obj_set_style_text_align(value, LV_TEXT_ALIGN_CENTER, 0);
                lv_obj_align(value, LV_ALIGN_CENTER, 0, 0);
            }
            else if (align == LV_ALIGN_LEFT_MID)
            {
                lv_obj_align(value, LV_ALIGN_LEFT_MID, 10, 0);                
            } 
        }

        lv_obj_add_event_cb(cont, my_radiobox_event_cb, LV_EVENT_ALL, NULL);
    }

    lv_obj_set_user_data(radiobox_panel, p_ext);
    lv_obj_add_event_cb(radiobox_panel, my_radiobox_list_event_cb, LV_EVENT_ALL, NULL);

    return radiobox_panel;    
}

lv_obj_t* my_radiobox1_list_create(lv_obj_t* parent, lv_coord_t w, my_radiobox_ext_t* p_ext, uint8_t type, lv_align_t align, uint8_t enter_value)
{
    lv_obj_t* radiobox_panel = lv_obj_create(parent);
    lv_coord_t height = 0;
    lv_coord_t gap = 5;
    lv_coord_t items_height = 40;

    height = items_height * p_ext->items_of_page + gap * (p_ext->items_of_page - 1) + 2 * gap;
    p_ext->type = type;
    p_ext->default_val = p_ext->sel;

    lv_obj_add_style(radiobox_panel, &style_rect_panel, 0);
    lv_obj_clear_flag(radiobox_panel, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_flag(radiobox_panel, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
    lv_obj_set_style_bg_color(radiobox_panel, lv_color_black(), 0);
    lv_obj_set_flex_flow(radiobox_panel, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_pad_gap(radiobox_panel, gap, 0);
    lv_obj_set_style_pad_ver(radiobox_panel, gap, 0);
    lv_obj_set_size(radiobox_panel, w, height);
	
	uint8_t value1 = enter_value;
	
    for (int i = 0; i < p_ext->max; i++)
    {
        lv_obj_t* cont = lv_obj_create(radiobox_panel);
        lv_obj_add_style(cont, &style_common_btn, 0);
        lv_obj_add_style(cont, &style_common_focued_btn, LV_STATE_FOCUSED);
        lv_obj_set_style_text_font(cont, Font_ResouceGet(FONT_18), 0);
        lv_obj_set_size(cont, lv_pct(100), items_height);

        const char* str = "sdf";        
        if (p_ext->get_name_str) {
            str = p_ext->get_name_str(i);           
        }

        lv_obj_t* name = lv_label_create(cont);
        lv_obj_set_width(name, LV_SIZE_CONTENT);
        lv_obj_set_height(name, lv_font_get_line_height(Font_ResouceGet(FONT_18)));
        lv_label_set_long_mode(name, LV_LABEL_LONG_CLIP);     

        lv_obj_t* value = lv_label_create(cont);
        lv_obj_set_height(value, lv_font_get_line_height(Font_ResouceGet(FONT_18)));
        lv_label_set_long_mode(value, LV_LABEL_LONG_CLIP);     

        lv_obj_t* img = lv_img_create(cont);
        lv_obj_set_style_img_recolor(img,  lv_color_white(), 0);
        lv_obj_set_style_img_recolor_opa(img, LV_OPA_100, 0);
        lv_obj_set_style_img_recolor(img,  lv_color_white(), LV_STATE_FOCUSED);
        lv_obj_set_style_img_recolor_opa(img, LV_OPA_100, LV_STATE_FOCUSED);
     
        LV_IMG_DECLARE(ImgRadioboxSel)
        LV_IMG_DECLARE(ImgRadioboxNoSel)
        if ((value1 & 0x01) == 1)
        {
            lv_img_set_src(img, &ImgRadioboxSel);
        }
        else
        {
            lv_img_set_src(img, &ImgRadioboxNoSel);
        }
		value1 = value1>>1;
		
        lv_obj_align(img, LV_ALIGN_RIGHT_MID, -3, 0);
        
        // 刷新RadioBox
        if (MY_RADIOBOX_TYPE1 == type)
        {
            lv_label_set_text_fmt(name, "%d.", i + 1);
            lv_obj_align(name, LV_ALIGN_LEFT_MID, 10, 0);
            
            lv_obj_set_width(value, lv_pct(72));
            lv_label_set_text(value, str);
            lv_obj_align_to(value, name, LV_ALIGN_OUT_RIGHT_MID, 5, 0);                    
        }
        else 
        {
            lv_obj_add_flag(name, LV_OBJ_FLAG_HIDDEN);
            lv_obj_set_width(value, lv_pct(85));
            lv_label_set_text(value, str);

            if (align == LV_ALIGN_CENTER)
            {
                lv_obj_set_style_text_align(value, LV_TEXT_ALIGN_CENTER, 0);
                lv_obj_align(value, LV_ALIGN_CENTER, 0, 0);
            }
            else if (align == LV_ALIGN_LEFT_MID)
            {
                lv_obj_align(value, LV_ALIGN_LEFT_MID, 10, 0);                
            }            
        }

        lv_obj_add_event_cb(cont, my_radiobox_event_cb, LV_EVENT_ALL, NULL);
    }

    lv_obj_set_user_data(radiobox_panel, p_ext);
    lv_obj_add_event_cb(radiobox_panel, my_radiobox_list_event_cb, LV_EVENT_ALL, NULL);

    return radiobox_panel;    
}


lv_obj_t* my_smooth_list_create(lv_obj_t* parent, lv_coord_t w, my_radiobox_ext_t* p_ext, uint8_t type, lv_align_t align)
{
    lv_obj_t* radiobox_panel = lv_obj_create(parent);
    lv_coord_t height = 0;
    lv_coord_t gap = 4;
    lv_coord_t items_height = 60;

    height = items_height * p_ext->items_of_page + gap * (p_ext->items_of_page - 1);//+ 2 * gap;
    p_ext->type = type;
    p_ext->default_val = p_ext->sel;

    lv_obj_add_style(radiobox_panel, &style_rect_panel, 0);
    lv_obj_clear_flag(radiobox_panel, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_flag(radiobox_panel, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
    lv_obj_set_style_bg_color(radiobox_panel, lv_color_black(), 0);
    lv_obj_set_flex_flow(radiobox_panel, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_pad_gap(radiobox_panel, gap, 0);
    //lv_obj_set_style_pad_ver(radiobox_panel, gap, 0);
    lv_obj_set_size(radiobox_panel, w, height);

    for (int i = 0; i < p_ext->max; i++)
    {
        lv_obj_t* cont = lv_obj_create(radiobox_panel);
        lv_obj_add_style(cont, &style_common_btn, 0);
        lv_obj_add_style(cont, &style_common_focued_btn, LV_STATE_FOCUSED);
        lv_obj_set_style_text_font(cont, Font_ResouceGet(FONT_18), 0);
        lv_obj_set_size(cont, lv_pct(100), items_height);

        const char* str = "sdf";
        if (p_ext->get_name_str) {
            str = p_ext->get_name_str(i);
        }

        lv_obj_t* name = lv_label_create(cont);
        lv_obj_set_width(name, LV_SIZE_CONTENT);
        lv_obj_set_height(name, lv_font_get_line_height(Font_ResouceGet(FONT_18)));
        lv_label_set_long_mode(name, LV_LABEL_LONG_CLIP);

        lv_obj_t* value = lv_label_create(cont);
        lv_obj_set_height(value, lv_font_get_line_height(Font_ResouceGet(FONT_18)));
        lv_label_set_long_mode(value, LV_LABEL_LONG_CLIP);


        // 刷新RadioBox
        if (MY_RADIOBOX_TYPE1 == type)
        {
            lv_label_set_text_fmt(name, "%d.", i + 1);
            lv_obj_align(name, LV_ALIGN_LEFT_MID, 10, 0);

            lv_obj_set_width(value, lv_pct(72));
            lv_label_set_text(value, str);
            lv_obj_align_to(value, name, LV_ALIGN_OUT_RIGHT_MID, 5, 0);
        }
        else
        {
            lv_obj_add_flag(name, LV_OBJ_FLAG_HIDDEN);
            lv_obj_set_width(value, 122);
            lv_label_set_text(value, str);

            if (align == LV_ALIGN_CENTER)
            {
                lv_obj_set_style_text_align(value, LV_TEXT_ALIGN_CENTER, 0);
                lv_obj_align(value, LV_ALIGN_CENTER, 0, 0);
            }
            else if (align == LV_ALIGN_LEFT_MID)
            {
                lv_obj_align(value, LV_ALIGN_LEFT_MID, 10, 0);
            }
        }

        lv_obj_add_event_cb(cont, my_radiobox_event_cb, LV_EVENT_ALL, NULL);
    }

    lv_obj_set_user_data(radiobox_panel, p_ext);
    lv_obj_add_event_cb(radiobox_panel, my_smooth_list_event_cb, LV_EVENT_ALL, NULL);

    return radiobox_panel;
}


/**
 * @brief 更新radiobox的图片
 * 
 * @param radiobox 
 * @param seleted 
 */
void refresh_radiobox_img(lv_obj_t* radiobox, bool seleted)
{
    LV_IMG_DECLARE(ImgRadioboxSel)
    LV_IMG_DECLARE(ImgRadioboxNoSel)
    lv_obj_t* img = lv_obj_get_child(radiobox, -1);
    
    if (seleted)
    {
        lv_img_set_src(img, &ImgRadioboxSel);
    }
    else
    {
        lv_img_set_src(img, &ImgRadioboxNoSel);
    }
}


// lv_obj_t* my_radiobox_list_create(lv_obj_t* parent, lv_coord_t w, my_radiobox_ext_t* p_ext, uint8_t type)
// {
//     lv_obj_t* radiobox_panel = lv_obj_create(parent);
//     lv_coord_t height = 0;
//     lv_coord_t gap = 5;
//     lv_coord_t items_height = 40;

//     height = items_height * p_ext->items_of_page + gap * (p_ext->items_of_page - 1) + 2 * gap;

//     lv_obj_add_style(radiobox_panel, &style_rect_panel, 0);
//     lv_obj_clear_flag(radiobox_panel, LV_OBJ_FLAG_SCROLLABLE);
//     lv_obj_add_flag(radiobox_panel, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
//     lv_obj_set_style_bg_color(radiobox_panel, lv_color_black(), 0);
//     lv_obj_set_flex_flow(radiobox_panel, LV_FLEX_FLOW_COLUMN);
//     lv_obj_set_style_pad_gap(radiobox_panel, gap, 0);
//     lv_obj_set_style_pad_ver(radiobox_panel, gap, 0);
//     lv_obj_set_size(radiobox_panel, w, height);

//     for (int i = 0; i < p_ext->max; i++)
//     {
//         ;
//         lv_obj_t* cont = lv_obj_create(radiobox_panel);
//         lv_obj_add_style(cont, &style_common_btn, 0);
//         lv_obj_add_style(cont, &style_common_focued_btn, LV_STATE_FOCUSED);
//         lv_obj_set_style_text_font(cont, Font_ResouceGet(FONT_18), 0);
//         lv_obj_set_size(cont, lv_pct(100), items_height);

//         lv_obj_t* name = lv_label_create(cont);
//         lv_obj_set_width(name, lv_pct(85));
//         lv_obj_set_height(name, lv_font_get_line_height(Font_ResouceGet(FONT_18)));
//         lv_label_set_long_mode(name, LV_LABEL_LONG_DOT);
//         const char* str = "sdf";
        
//         if (p_ext->get_name_str) {
//             str = p_ext->get_name_str(i);           
//         }

//         lv_label_set_text(name, str);

//         lv_align_t align = type == 0 ? LV_ALIGN_CENTER : LV_ALIGN_LEFT_MID;
//         lv_coord_t left_to_board = type == 0 ? 0 : 10;

//         if (type == 0)
//         {
//             lv_obj_set_style_text_align(name, LV_TEXT_ALIGN_CENTER, 0);
//         }

//         lv_obj_align(name, align, left_to_board, 0);
        
//         lv_obj_t* img = lv_img_create(cont);
//         lv_obj_set_style_img_recolor(img,  lv_color_white(), 0);
//         lv_obj_set_style_img_recolor_opa(img, LV_OPA_100, 0);
//         lv_obj_set_style_img_recolor(img,  lv_color_black(), LV_STATE_FOCUSED);
//         lv_obj_set_style_img_recolor_opa(img, LV_OPA_100, LV_STATE_FOCUSED);
//         lv_obj_align(img, LV_ALIGN_RIGHT_MID, -3, 0);

//         LV_IMG_DECLARE(ImgRadioboxSel)
//         LV_IMG_DECLARE(ImgRadioboxNoSel)
//         if (p_ext->sel == i)
//         {
//             lv_img_set_src(img, &ImgRadioboxSel);
//         }
//         else
//         {
//             lv_img_set_src(img, &ImgRadioboxNoSel);
//         }
        
//         lv_obj_add_event_cb(cont, my_radiobox_event_cb, LV_EVENT_ALL, NULL);
//     }

//     lv_obj_set_user_data(radiobox_panel, p_ext);
//     lv_obj_add_event_cb(radiobox_panel, my_radiobox_list_event_cb, LV_EVENT_ALL, NULL);

//     return radiobox_panel;    
// }

lv_obj_t* my_long_list_create(lv_obj_t* parent, lv_coord_t w, my_radiobox_ext_t* p_ext)
{
    lv_obj_t* radiobox_panel = lv_obj_create(parent);
    lv_coord_t height = 0;
    lv_coord_t gap = 5;
    lv_coord_t items_height = 40;

    height = items_height * p_ext->items_of_page + gap * (p_ext->items_of_page - 1) + 2 * gap;

    lv_obj_add_style(radiobox_panel, &style_rect_panel, 0);
    lv_obj_clear_flag(radiobox_panel, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_flag(radiobox_panel, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
    lv_obj_set_style_bg_color(radiobox_panel, lv_color_black(), 0);
    lv_obj_set_flex_flow(radiobox_panel, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_pad_gap(radiobox_panel, gap, 0);
    lv_obj_set_style_pad_ver(radiobox_panel, gap, 0);
    lv_obj_set_size(radiobox_panel, w, height);

    for (int i = 0; i < p_ext->max; i++)
    {
        lv_obj_t* cont = lv_obj_create(radiobox_panel);
        lv_obj_add_style(cont, &style_common_btn, 0);
        lv_obj_add_style(cont, &style_common_focued_btn, LV_STATE_FOCUSED);
        lv_obj_set_style_radius(cont, 5, LV_STATE_FOCUSED);
        lv_obj_set_style_text_font(cont, Font_ResouceGet(FONT_18), 0);
        lv_obj_set_size(cont, lv_pct(100), items_height);

        lv_obj_t* name = lv_label_create(cont);
        lv_obj_set_height(name, lv_font_get_line_height(Font_ResouceGet(FONT_18)));
        lv_label_set_long_mode(name, LV_LABEL_LONG_CLIP);
        lv_obj_add_flag(name, LV_OBJ_FLAG_HIDDEN);

        const char* str = "sdf";
        
        if (p_ext->get_name_str) {
            str = p_ext->get_name_str(i);           
        }

        lv_obj_t* value = lv_label_create(cont);
        lv_obj_set_width(value, lv_pct(85));        
        lv_obj_set_height(value, lv_font_get_line_height(Font_ResouceGet(FONT_18)));
        lv_label_set_long_mode(value, LV_LABEL_LONG_CLIP);     
        lv_label_set_text(value, str);
        
        lv_obj_align(value, LV_ALIGN_LEFT_MID, 10, 0);

        LV_IMG_DECLARE(ImgArrowLeft)
        lv_obj_t* img = lv_img_create(cont);
        lv_obj_set_style_img_recolor(img,  lv_color_white(), 0);
        lv_obj_set_style_img_recolor_opa(img, LV_OPA_100, 0);
        lv_obj_set_style_img_recolor(img,  lv_color_white(), LV_STATE_FOCUSED);
        lv_obj_set_style_img_recolor_opa(img, LV_OPA_100, LV_STATE_FOCUSED);    
        lv_img_set_src(img, &ImgArrowLeft);
        lv_img_set_angle(img, 1800);
        lv_img_set_zoom(img, 200);
        lv_obj_align(img, LV_ALIGN_RIGHT_MID, -4, 0);

        lv_obj_add_event_cb(cont, my_radiobox_event_cb, LV_EVENT_ALL, NULL);
    }

    lv_obj_set_user_data(radiobox_panel, p_ext);
    lv_obj_add_event_cb(radiobox_panel, my_radiobox_list_event_cb, LV_EVENT_ALL, NULL);

    return radiobox_panel;        
}

void my_btnmatrix_event_cb(lv_event_t* e)
{
    lv_obj_t* btnmatrix = lv_event_get_target(e);
    lv_event_code_t event = lv_event_get_code(e);
    my_btnmatrix_ext_t* ext = lv_obj_get_user_data(btnmatrix);

    if (ext == NULL)    return;

    if (event == LV_EVENT_FOCUSED)
    {
        lv_obj_t* obj = lv_obj_get_child(btnmatrix, ext->sel);        
        lv_obj_add_state(obj, LV_STATE_FOCUSED);
    }
    else if (event == LV_EVENT_DEFOCUSED)
    {
        lv_obj_t* obj = lv_obj_get_child(btnmatrix, ext->sel);        
        lv_obj_clear_state(obj, LV_STATE_FOCUSED);
    }
    else if (event == LV_EVENT_KEY)
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
            lv_obj_t* prev_obj = lv_obj_get_child(btnmatrix, ext->sel);            
            lv_obj_clear_state(prev_obj, LV_STATE_FOCUSED);      

            ext->sel = index;

            lv_obj_t* next_obj = lv_obj_get_child(btnmatrix, ext->sel);
            lv_obj_add_state(next_obj, LV_STATE_FOCUSED); 
			lv_obj_scroll_to_view(next_obj, LV_ANIM_OFF);			
        }
    }

    if (ext->custom_event_cb)
    {
        ext->custom_event_cb(e, ext->sel);
    }
}


void my_radiobox_list_event_cb(lv_event_t* e)
{
    lv_obj_t* radiobox_cont = lv_event_get_target(e);
    lv_event_code_t event = lv_event_get_code(e);
    const lv_indev_t* indev = lv_indev_get_act();    
    my_radiobox_ext_t* ext = lv_obj_get_user_data(radiobox_cont);
    
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
            return ;

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


void my_radiobox_event_cb(lv_event_t* e)
{
    lv_obj_t* obj = lv_event_get_target(e);
    lv_event_code_t event = lv_event_get_code(e);
    lv_obj_t* list = lv_obj_get_parent(obj);

    if (event == LV_EVENT_FOCUSED)
    {
        uint8_t child_sum = lv_obj_get_child_cnt(obj);
        for (uint8_t cnt = 0; cnt < child_sum; cnt++)
        {
            lv_obj_t* child = lv_obj_get_child(obj, cnt);
            if (cnt == 1)
            {
                lv_label_set_long_mode(child, LV_LABEL_LONG_SCROLL);
            }
            lv_obj_add_state(child, LV_STATE_FOCUSED);
        }
        lv_obj_scroll_to_view(obj, LV_ANIM_OFF);
    }  
    else if (event == LV_EVENT_DEFOCUSED)
    {
        uint8_t child_sum = lv_obj_get_child_cnt(obj);
        for (uint8_t cnt = 0; cnt < child_sum; cnt++)
        {
            lv_obj_t* child = lv_obj_get_child(obj, cnt);
            if (cnt == 1)
            {
                lv_label_set_long_mode(child, LV_LABEL_LONG_CLIP);
            }            
            lv_obj_clear_state(child, LV_STATE_FOCUSED);
        }        
    }
}

lv_obj_t* my_list_create(lv_obj_t* parent, lv_coord_t w, lv_coord_t h, \
                                    my_list_ext_t* p_ext)
{
    lv_obj_t* list = lv_obj_create(parent);        
    lv_obj_add_style(list, &style_rect_panel, 0);
    lv_obj_set_style_bg_color(list, lv_color_black(), 0);
    lv_obj_set_flex_flow(list, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_user_data(list, p_ext);
    lv_obj_add_event_cb(list, my_list_event_cb, LV_EVENT_ALL, NULL);
    lv_obj_set_size(list, w, h);
    p_ext->childs = 0;
    return list;
}


void my_obj_event_create(lv_obj_t* parent)
{    
    lv_obj_add_event_cb(parent, my_list_event_cb, LV_EVENT_ALL, NULL);
}


lv_obj_t* my_list_add_btn(lv_obj_t* list, const char* name, lv_coord_t label_to_left)
{
    lv_obj_t* btn = lv_obj_create(list);
    lv_obj_clear_flag(btn, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_flag(btn, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
    lv_obj_add_style(btn, &style_common_btn, 0);
    lv_obj_set_style_radius(btn, 5, 0);
    lv_obj_add_style(btn, &style_common_focued_btn, LV_STATE_FOCUSED);
    lv_obj_set_style_radius(btn, 5, LV_STATE_FOCUSED);
    lv_obj_set_style_text_font(btn, Font_ResouceGet(FONT_18), 0);
    lv_obj_set_style_text_font(btn, Font_ResouceGet(FONT_18), LV_STATE_FOCUSED);
    lv_obj_set_size(btn, lv_pct(98), 40);
    
    lv_obj_t* label =lv_label_create(btn);
    lv_label_set_long_mode(label, LV_LABEL_LONG_CLIP);
    lv_obj_set_width(label, lv_pct(90));

    if (name != NULL) {
        lv_label_set_text(label, name);
    }    
    lv_obj_align(label, LV_ALIGN_LEFT_MID, label_to_left, 0);
    
    LV_IMG_DECLARE(ImgArrowLeft)
    lv_obj_t* img = lv_img_create(btn);
    lv_obj_set_style_img_recolor(img,  lv_color_white(), 0);
    lv_obj_set_style_img_recolor_opa(img, LV_OPA_100, 0);
	lv_obj_set_style_img_recolor(img,  lv_color_black(), LV_STATE_FOCUSED);
    lv_obj_set_style_img_recolor_opa(img, LV_OPA_100, LV_STATE_FOCUSED);    
    lv_img_set_src(img, &ImgArrowLeft);
    lv_img_set_angle(img, 1800);
    lv_img_set_zoom(img, 200);
    lv_obj_align(img, LV_ALIGN_RIGHT_MID, -4, 0);

    lv_obj_add_event_cb(btn, my_list_btn_event_cb, LV_EVENT_ALL, NULL);
 
    my_list_ext_t* ext = lv_obj_get_user_data(list);
    if (ext != NULL)
    {
        ext->childs += 1;
    }
    
    return btn;
}

lv_obj_t* my_list_add_btn1(lv_obj_t* list, const char* name)
{
    
    lv_obj_t* btn = lv_obj_create(list);
    lv_obj_clear_flag(btn, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_flag(btn, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
    lv_obj_add_style(btn, &style_common_btn, 0);
    lv_obj_set_style_radius(btn, 5, 0);
    lv_obj_add_style(btn, &style_common_focued_btn, LV_STATE_FOCUSED);
    lv_obj_set_style_radius(btn, 5, LV_STATE_FOCUSED);
    lv_obj_set_style_text_font(btn, Font_ResouceGet(FONT_18), 0);
    lv_obj_set_style_text_font(btn, Font_ResouceGet(FONT_18), LV_STATE_FOCUSED);    
    lv_obj_set_size(btn, lv_pct(90), 40);
    
    lv_obj_t* label =lv_label_create(btn);
    lv_label_set_long_mode(label, LV_LABEL_LONG_CLIP);
    lv_obj_set_width(label, lv_pct(80));

    if (name != NULL) {
        lv_label_set_text(label, name);
    }    
    lv_obj_align(label, LV_ALIGN_LEFT_MID, 30, 0);

    lv_obj_add_event_cb(btn, my_list_btn_event_cb, LV_EVENT_ALL, NULL);

    return btn; 
}

lv_obj_t* my_list_add_btn2(lv_obj_t* list, const char* name, const lv_img_dsc_t* p_img_dsc, uint8_t type)
{
    lv_obj_t* btn = lv_obj_create(list);
    lv_obj_clear_flag(btn, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_flag(btn, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
    lv_obj_add_style(btn, &style_common_btn, 0);
    lv_obj_set_style_radius(btn, 5, 0);
    lv_obj_add_style(btn, &style_common_focued_btn, LV_STATE_FOCUSED);
    lv_obj_set_style_radius(btn, 5, LV_STATE_FOCUSED);
    lv_obj_set_style_text_font(btn, Font_ResouceGet(FONT_18), 0);
    lv_obj_set_style_text_font(btn, Font_ResouceGet(FONT_18), LV_STATE_FOCUSED);    
    lv_obj_set_size(btn, lv_pct(90), 42);

    lv_obj_t* label =lv_label_create(btn);
    lv_label_set_long_mode(label, LV_LABEL_LONG_CLIP);
    lv_obj_set_width(label, lv_pct(80));

    if (name != NULL) {
        lv_label_set_text(label, name);
    }    
    lv_obj_align(label, LV_ALIGN_LEFT_MID, 68, 0);

    lv_obj_t* circle = lv_obj_create(btn);
    lv_obj_add_style(circle, &style_rect_panel, 0);
    lv_obj_set_style_bg_color(circle, lv_color_black(), 0);
    lv_obj_set_style_bg_color(circle, lv_color_white(), LV_STATE_FOCUSED);
    lv_obj_set_style_radius(circle, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_size(circle, 37, 37);
    lv_obj_align(circle, LV_ALIGN_LEFT_MID, 15, 0);
    
    lv_obj_t* img = lv_img_create(btn);
    lv_obj_set_style_img_recolor(img,  lv_color_white(), 0);
    lv_obj_set_style_img_recolor_opa(img, LV_OPA_100, 0);
    lv_obj_set_style_img_recolor(img,  RED_THEME_COLOR, LV_STATE_FOCUSED);
    lv_obj_set_style_img_recolor_opa(img, LV_OPA_100, LV_STATE_FOCUSED);    
    lv_img_set_src(img, p_img_dsc);
    if(type == 1)//york   修改兼容DMX
        lv_img_set_zoom(img, 256);
    else
        lv_img_set_zoom(img, 80);

    lv_obj_align_to(img, circle, LV_ALIGN_CENTER, 0, 0);
    //lv_obj_align(img, LV_ALIGN_LEFT_MID, 27, 0);

    LV_IMG_DECLARE(ImgArrowLeft)
    lv_obj_t* img_arrow = lv_img_create(btn);
    lv_obj_set_style_img_recolor(img_arrow,  lv_color_white(), 0);
    lv_obj_set_style_img_recolor_opa(img_arrow, LV_OPA_100, 0);
    lv_obj_set_style_img_recolor(img_arrow,  lv_color_black(), LV_STATE_FOCUSED);
    lv_obj_set_style_img_recolor_opa(img_arrow, LV_OPA_100, LV_STATE_FOCUSED);    
    lv_img_set_src(img_arrow, &ImgArrowLeft);
    lv_img_set_angle(img_arrow, 1800);
    lv_img_set_zoom(img_arrow, 200);
    lv_obj_align(img_arrow, LV_ALIGN_RIGHT_MID, -4, 0);

    my_list_ext_t* ext = lv_obj_get_user_data(list);
    if (ext != NULL)
    {
        ext->childs += 1;
    }
    lv_obj_add_event_cb(btn, my_list_btn_event_cb, LV_EVENT_ALL, NULL);

    return btn;
}

lv_obj_t* my_list_add_btn3(lv_obj_t* list, const char* name, lv_coord_t label_to_left,uint8_t value)
{
    lv_obj_t* btn = lv_obj_create(list);
    lv_obj_clear_flag(btn, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_flag(btn, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
    lv_obj_add_style(btn, &style_common_btn, 0);
    lv_obj_set_style_radius(btn, 5, 0);
    lv_obj_add_style(btn, &style_common_focued_btn, LV_STATE_FOCUSED);
    lv_obj_set_style_radius(btn, 5, LV_STATE_FOCUSED);
    lv_obj_set_style_text_font(btn, Font_ResouceGet(FONT_18), 0);
    lv_obj_set_style_text_font(btn, Font_ResouceGet(FONT_18), LV_STATE_FOCUSED);
    lv_obj_set_size(btn, lv_pct(98), 40);
    
    lv_obj_t* label =lv_label_create(btn);
    lv_label_set_long_mode(label, LV_LABEL_LONG_CLIP);
    lv_obj_set_width(label, lv_pct(90));
	
	lv_obj_t* label_div =lv_label_create(btn);
    lv_label_set_long_mode(label_div, LV_LABEL_LONG_CLIP);
    lv_obj_set_width(label_div, lv_pct(90));
	lv_snprintf(sdb, sizeof(sdb), "%d%", value);
	//format_intensity((char*)sdb, sizeof(sdb), value);
    lv_obj_align(label_div, LV_ALIGN_LEFT_MID, 80, 0);
	
    if (name != NULL) {
        lv_label_set_text(label, name);
		lv_label_set_text(label_div, sdb);
    }    
    lv_obj_align(label, LV_ALIGN_LEFT_MID, label_to_left, 0);
    
    LV_IMG_DECLARE(ImgArrowLeft)
    lv_obj_t* img = lv_img_create(btn);
    lv_obj_set_style_img_recolor(img,  lv_color_white(), 0);
    lv_obj_set_style_img_recolor_opa(img, LV_OPA_100, 0);
    lv_obj_set_style_img_recolor(img,  lv_color_white(), LV_STATE_FOCUSED);
    lv_obj_set_style_img_recolor_opa(img, LV_OPA_100, LV_STATE_FOCUSED);    
    lv_img_set_src(img, &ImgArrowLeft);
    lv_img_set_angle(img, 1800);
    lv_img_set_zoom(img, 200);
    lv_obj_align(img, LV_ALIGN_RIGHT_MID, -4, 0);
	lv_obj_add_event_cb(btn, my_list_btn_event_cb, LV_EVENT_ALL, NULL);
    my_list_ext_t* ext = lv_obj_get_user_data(list);
    if (ext != NULL)
    {
        ext->childs += 1;
    }
    
    return btn;
}

lv_obj_t* my_list_add_btn4(lv_obj_t* list)
{
    lv_obj_t* btn = lv_obj_create(list);
    lv_obj_clear_flag(btn, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_flag(btn, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
    lv_obj_add_style(btn, &style_common_btn, 0);
    lv_obj_set_style_radius(btn, 0, 0);
    lv_obj_add_style(btn, &style_common_focued_btn, LV_STATE_FOCUSED);
    lv_obj_set_style_radius(btn, 0, LV_STATE_FOCUSED);
    lv_obj_set_style_text_font(btn, Font_ResouceGet(FONT_18), 0);
    lv_obj_set_style_text_font(btn, Font_ResouceGet(FONT_18), LV_STATE_FOCUSED);
    lv_obj_set_size(btn, lv_pct(98), 40);
	
    return btn;
}

lv_obj_t* my_list_add_switch_btn(lv_obj_t* list, const char* name, bool state)
{
        
    lv_obj_t* btn = lv_obj_create(list);
    lv_obj_clear_flag(btn, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_flag(btn, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
    lv_obj_add_style(btn, &style_common_btn, 0);
    lv_obj_set_style_radius(btn, 5, 0);
    lv_obj_add_style(btn, &style_common_focued_btn, LV_STATE_FOCUSED);
    lv_obj_set_style_radius(btn, 5, LV_STATE_FOCUSED);
    lv_obj_set_size(btn, lv_pct(98), 42);
    lv_obj_set_style_text_font(btn, Font_ResouceGet(FONT_18), 0);
    lv_obj_set_style_text_font(btn, Font_ResouceGet(FONT_18), LV_STATE_FOCUSED);    

    lv_obj_t* label =lv_label_create(btn);
    lv_label_set_long_mode(label, LV_LABEL_LONG_CLIP);
    lv_obj_set_width(label, lv_pct(80));

    if (name != NULL) {
        lv_label_set_text(label, name);
    }    
    lv_obj_align(label, LV_ALIGN_LEFT_MID, 10, 0);

    lv_obj_t* img = lv_img_create(btn);
    list_switch_btn_refresh(btn, state);

    return btn;
}

void list_switch_btn_refresh(lv_obj_t* obj, bool state)
{
    lv_obj_t* img = lv_obj_get_child(obj, -1);

    LV_IMG_DECLARE(ImgSwitchOff)
    LV_IMG_DECLARE(ImgSwitchOn)
    
    LV_IMG_DECLARE(ImgBtnON);
    LV_IMG_DECLARE(ImgBtnOFF);

    if (state)
    {        
        lv_img_set_src(img, &ImgBtnON);
    }
    else 
    {        
        lv_img_set_src(img, &ImgBtnOFF);   
    }

    lv_obj_align(img, LV_ALIGN_RIGHT_MID, -5, 0);
}

void list_state_btn_refresh(lv_obj_t* obj, bool state)
{
    lv_obj_t* img = lv_obj_get_child(obj, -1);
    
    LV_IMG_DECLARE(imgStart);
    LV_IMG_DECLARE(imgStop);

    if (state)
    {        
        lv_img_set_src(img, &imgStart);
    }
    else 
    {        
        lv_img_set_src(img, &imgStop);   
    }

    lv_obj_align(img, LV_ALIGN_RIGHT_MID, -5, 0);
}

lv_obj_t* ip_btn_create(lv_obj_t* parent, const char* name, ip_btn_ext_t* ext, uint8_t mode)
{
    lv_obj_t* btn = lv_obj_create(parent);    
    lv_obj_add_style(btn, &style_common_btn, 0);
    lv_obj_set_style_radius(btn, 5, 0);
	lv_obj_clear_flag(btn, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_flag(btn, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
	if(mode == 0)   //york 修改
	{
		lv_obj_add_style(btn, &style_common_focued_btn, LV_STATE_FOCUSED);
		lv_obj_set_style_radius(btn, 5, LV_STATE_FOCUSED);
		lv_obj_set_style_text_color(btn, lv_color_black(), LV_STATE_FOCUSED | LV_STATE_EDITED);
	}
	else
	{
		if(ETH_DHCP_START == ui_get_eth_state())
			lv_obj_set_style_bg_color(btn, lv_color_make(38, 38, 38), 0);
		lv_obj_add_style(btn, &style_common_focued_btn, LV_STATE_FOCUSED);
		lv_obj_set_style_radius(btn, 5, LV_STATE_FOCUSED);
		lv_obj_set_style_text_color(btn, lv_color_white(), LV_STATE_FOCUSED | LV_STATE_EDITED);
	}
    lv_obj_set_size(btn, lv_pct(100), 42);
    
    lv_obj_t* label = lv_label_create(btn);    
    lv_label_set_text(label, name);
	if(mode <= 1)   //york 修改
		lv_obj_set_style_text_font(label, Font_ResouceGet(FONT_18), 0);
	else
		lv_obj_set_style_text_font(label, Font_ResouceGet(FONT_18), 0);
    lv_obj_align(label, LV_ALIGN_LEFT_MID, 10, 0);

    lv_obj_t* label_cont = lv_obj_create(btn);
    lv_obj_remove_style_all(label_cont);
    //lv_obj_set_size(label_cont, lv_pct(48), LV_SIZE_CONTENT);
    lv_obj_set_size(label_cont, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_align(label_cont, LV_ALIGN_RIGHT_MID, -10, 0);
    lv_obj_set_flex_flow(label_cont, LV_FLEX_FLOW_ROW);
    lv_obj_set_style_pad_gap(label_cont, 0, 0);  
	
	lv_obj_set_user_data(btn, ext);
    lv_obj_add_event_cb(btn, ip_btn_event_cb, LV_EVENT_ALL, NULL);
	
    lv_obj_t* tmp;
	if(mode == 0) //york 修改
	{
		for (uint8_t i = 0; i < 4; i++)
		{        
			tmp = lv_label_create(label_cont);
			lv_obj_set_style_text_font(tmp, Font_ResouceGet(FONT_16_MEDIUM), 0);    
			lv_obj_set_style_text_color(tmp, lv_color_white(), 0);
			lv_obj_set_style_text_color(tmp, lv_color_black(), LV_STATE_USER_1);
			lv_obj_set_style_bg_opa(tmp, LV_OPA_100, LV_STATE_USER_1);
			lv_obj_set_style_bg_color(tmp, lv_color_white(), LV_STATE_USER_1);
		}
		ip_btn_refresh(btn);
	}
	else if(mode == 1)
	{
		lv_obj_align(label_cont, LV_ALIGN_CENTER, 15, 0);
		tmp = lv_label_create(label_cont);
		lv_obj_set_style_text_font(tmp, Font_ResouceGet(FONT_18_BOLD), 0); 
		lv_obj_set_style_text_color(tmp, lv_color_white(), 0);
		lv_label_set_text(tmp, (const char *)ext->buff);
		
		LV_IMG_DECLARE(ImgArrowLeft)
		lv_obj_t* img_arrow = lv_img_create(btn);
		lv_obj_set_style_img_recolor(img_arrow,  lv_color_white(), 0);
		lv_obj_set_style_img_recolor_opa(img_arrow, LV_OPA_100, 0);
		lv_obj_set_style_img_recolor(img_arrow,  lv_color_black(), LV_STATE_FOCUSED);
		lv_obj_set_style_img_recolor_opa(img_arrow, LV_OPA_100, LV_STATE_FOCUSED);    
		lv_img_set_src(img_arrow, &ImgArrowLeft);
		lv_img_set_angle(img_arrow, 1800);
		lv_img_set_zoom(img_arrow, 200);
		lv_obj_align(img_arrow, LV_ALIGN_RIGHT_MID, -4, 0);
	}
	else if(mode == 2)
	{
		lv_obj_align(label_cont, LV_ALIGN_RIGHT_MID, 0, 0);
		tmp = lv_label_create(label_cont);
		lv_obj_set_style_text_font(tmp, Font_ResouceGet(FONT_18), 0); 
		lv_obj_set_style_text_color(tmp, lv_color_white(), 0);
		lv_label_set_text(tmp, (const char *)ext->buff);
	}
	
    return btn;
}

lv_obj_t* info_btn_create(lv_obj_t* parent, kv_string_t *p_kv)
{    
    lv_obj_t* cont = lv_obj_create(parent);
    lv_obj_add_style(cont, &style_rect_panel, 0);
    lv_obj_set_style_text_color(cont, lv_color_white(), 0);
    lv_obj_set_style_text_font(cont, Font_ResouceGet(FONT_18), 0);
	lv_obj_add_style(cont, &style_common_focued_btn, LV_STATE_FOCUSED);
    lv_obj_set_style_radius(cont, 5, LV_STATE_FOCUSED);
    lv_obj_set_style_text_font(cont, Font_ResouceGet(FONT_18), LV_STATE_FOCUSED);
    lv_obj_set_size(cont, lv_pct(100), 42);
	lv_obj_clear_flag(cont, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_flag(cont, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
	lv_obj_add_event_cb(cont, my_list_btn_event_cb, LV_EVENT_ALL, NULL);
	
    lv_obj_t* label = lv_label_create(cont);
    lv_obj_set_style_text_color(label, lv_color_white(), 0);
    lv_label_set_text(label, p_kv->name);
    lv_obj_align(label, LV_ALIGN_LEFT_MID, 10, 0);

    lv_obj_t* label1 = lv_label_create(cont);
    lv_label_set_text(label1, "Ver. :");
    lv_obj_align(label1, LV_ALIGN_LEFT_MID, 180, 0);

    lv_obj_t* v_label = lv_label_create(cont);
    lv_label_set_text(v_label, p_kv->value);
    lv_obj_align(v_label, LV_ALIGN_RIGHT_MID, -5, 0);

    return cont;
}

lv_obj_t* warning_info_init(lv_obj_t* parent, const lv_img_dsc_t* p_img, lv_coord_t img_to_top, lv_coord_t label_to_img,const char* comment)
{
    lv_obj_t* panel = lv_obj_create(parent);
    lv_obj_remove_style_all(panel);
    lv_obj_clear_flag(panel, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_size(panel, LV_HOR_RES, LV_VER_RES);

    lv_obj_t* img = lv_img_create(panel);
    lv_img_set_src(img, p_img);
    lv_obj_align(img, LV_ALIGN_TOP_MID, 0,  img_to_top);

    lv_obj_t* comment_panel = lv_obj_create(panel);
    lv_obj_remove_style_all(comment_panel);
    lv_obj_clear_flag(comment_panel, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_size(comment_panel, lv_pct(100), LV_VER_RES - img_to_top - p_img->header.h);
    lv_obj_align_to(comment_panel, img, LV_ALIGN_OUT_BOTTOM_MID, 0,  0);
    
    lv_obj_t* label = lv_label_create(comment_panel);
    lv_obj_set_style_text_font(label, Font_ResouceGet(FONT_18_MEDIUM), 0);
    lv_obj_set_style_text_color(label, lv_color_white(), 0);
    lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, 0);
    lv_label_set_long_mode(label, LV_LABEL_LONG_WRAP);    
    lv_obj_set_size(label, lv_pct(98), LV_SIZE_CONTENT);
    lv_label_set_text(label, comment);
    lv_obj_align(label, LV_ALIGN_TOP_MID, 0, label_to_img);
    return panel;
}

lv_obj_t* my_long_list2_create(lv_obj_t* parent, lv_coord_t w, my_radiobox_ext_t* p_ext)
{
    lv_obj_t* radiobox_panel = lv_obj_create(parent);
    lv_coord_t height = 0;
    lv_coord_t gap = 5;

    height = 56;

    lv_obj_add_style(radiobox_panel, &style_rect_panel, 0);
    lv_obj_clear_flag(radiobox_panel, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_flag(radiobox_panel, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
    lv_obj_set_style_bg_color(radiobox_panel, lv_color_black(), 0);
    lv_obj_set_flex_flow(radiobox_panel, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_pad_gap(radiobox_panel, gap, 0);
    lv_obj_set_style_pad_ver(radiobox_panel, gap, 0);
    lv_obj_set_size(radiobox_panel, w, 180);

    for (int i = 0; i < p_ext->max; i++)
    {
        lv_obj_t* cont = lv_obj_create(radiobox_panel);
        lv_obj_add_style(cont, &style_common_btn, 0);
        lv_obj_add_style(cont, &style_common_focued_btn, LV_STATE_FOCUSED);
        lv_obj_set_style_radius(cont, 5, LV_STATE_FOCUSED);
        lv_obj_set_style_text_font(cont, Font_ResouceGet(FONT_18), 0);
        lv_obj_set_size(cont, lv_pct(100), height - 4);

        const char* str = "sdf";
        
        if (p_ext->get_name_str) {
            str = p_ext->get_name_str(i);           
        }
		
        lv_obj_t* value = lv_label_create(cont);
		if(i == p_ext->sel)
			lv_label_set_long_mode(value, LV_LABEL_LONG_SCROLL);
		else
			lv_label_set_long_mode(value, LV_LABEL_LONG_CLIP);
        lv_obj_set_width(value, lv_pct(90));        
        lv_obj_set_height(value, lv_font_get_line_height(Font_ResouceGet(FONT_18)));
        lv_label_set_text(value, str);
        lv_obj_align(value, LV_ALIGN_LEFT_MID, 10, 0);

        lv_obj_add_event_cb(cont, my_radiobox_event_cb, LV_EVENT_ALL, NULL);
    }

    lv_obj_set_user_data(radiobox_panel, p_ext);
    lv_obj_add_event_cb(radiobox_panel, my_radiobox_list_event_cb, LV_EVENT_ALL, NULL);

    return radiobox_panel;        
}

lv_obj_t* my_list_scroll_create(lv_obj_t* parent, lv_coord_t w, lv_coord_t h)
{
    lv_obj_t* scroll_cont = lv_obj_create(parent);
    lv_obj_remove_style_all(scroll_cont);
    lv_obj_set_size(scroll_cont, w, h);

    lv_obj_t* slider = lv_obj_create(scroll_cont);
    lv_obj_remove_style_all(slider);
    lv_obj_set_style_bg_color(slider, lv_color_white(), 0);
    lv_obj_set_style_bg_opa(slider, LV_OPA_100, 0);
    lv_obj_set_style_radius(slider, 1, 0);
    lv_obj_set_size(slider, w, 35);

    return scroll_cont;         
}

void my_list_scroll_refresh(lv_obj_t* scroll_cont, int8_t percent)
{
    if (scroll_cont == NULL)    return;

    lv_obj_t* slider = lv_obj_get_child(scroll_cont, 0);
    lv_coord_t distance = lv_obj_get_height(scroll_cont) - lv_obj_get_height(slider);    
    lv_coord_t y = lv_map(percent, 0, 100,  0, distance);

    lv_obj_set_y(slider, y);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/    

static void ip_btn_label_add_state(lv_obj_t* btn, uint8_t index)
{
    lv_obj_t* label_cont = lv_obj_get_child(btn, -1);
    lv_obj_t* obj = lv_obj_get_child(label_cont, index);
    lv_obj_add_state(obj, LV_STATE_USER_1);
}

static void ip_btn_label_clear_state(lv_obj_t* btn, uint8_t index)
{
    lv_obj_t* label_cont = lv_obj_get_child(btn, -1);
    lv_obj_t* obj = lv_obj_get_child(label_cont, index);
    lv_obj_clear_state(obj, LV_STATE_USER_1);    
}

static void ip_btn_refresh(lv_obj_t* btn)
{
    ip_btn_ext_t* ext = lv_obj_get_user_data(btn);    
    lv_obj_t* label_cont = lv_obj_get_child(btn, -1);
    uint8_t sum = lv_obj_get_child_cnt(label_cont);    
    lv_obj_t* obj;

    for (uint8_t i = 0; i < sum; i++)
    {
        obj = lv_obj_get_child(label_cont, i);
        //UI_PRINTF("%d label : %p\r\n", i, obj);
        if (sum - 1 == i)
        {
            lv_label_set_text_fmt(obj, "%d", ext->buff[i]);
        }
        else
        {
            lv_label_set_text_fmt(obj, "%d.", ext->buff[i]);
        }        
    }
}

static void ip_btn_event_cb(lv_event_t* e)
{
    lv_event_code_t event = lv_event_get_code(e);
    lv_obj_t* target = lv_event_get_target(e);
    lv_group_t* group = lv_obj_get_group(target);
    ip_btn_ext_t* ext = lv_obj_get_user_data(target);    

    shortcut_keys_event_handler(e);

    if (ext->user_event_cb)
    {
        ext->user_event_cb(e);
    }

    if (event == LV_EVENT_RELEASED)
    {
		if(ext->buff[0] == 'M' || ETH_DHCP_START == ui_get_eth_state() || ext->buff[0] == 'A' || 
		   (0 == memcmp(ext->buff, "自动设置IP", sizeof("自动设置IP"))) || 
		   (0 == memcmp(ext->buff, "手动设置IP", sizeof("手动设置IP"))))   //不是调节IP时    york修改
		{
			return;
		}
        if (!lv_group_get_editing(group))
        {
            ext->sel = 0;
            lv_group_set_editing(group, true);
            ip_btn_label_add_state(target, ext->sel);
        }
        else
        {
            ip_btn_label_clear_state(target, ext->sel);
            ext->sel += 1;
            ext->sel %= 4;
            ip_btn_label_add_state(target, ext->sel);
        }
    }
    else if (event == LV_EVENT_VALUE_CHANGED)
    {
        ip_btn_refresh(target);
    }
    else if (event == LV_EVENT_KEY && lv_group_get_editing(group))
    {
        uint32_t key = *(uint32_t *)lv_event_get_param(e);
        
        if (lv_group_get_editing(group))
        {
            if (key == LV_ENCODER_KEY_CW)
            {
                ext->buff[ext->sel]++;
                ip_btn_refresh(target);    
                page_set_invalid_type(PAGE_INVALID_TYPE_PORTION);            
            }
            else if (key == LV_ENCODER_KEY_CC)
            {
                ext->buff[ext->sel]--;
                ip_btn_refresh(target);                
                page_set_invalid_type(PAGE_INVALID_TYPE_PORTION);
            }
            else if (key == LV_KEY_BACKSPACE)
            {
                ip_btn_label_clear_state(target, ext->sel);
                lv_group_set_editing(group, false);
            }    
        }
    }
}

static void my_list_event_cb(lv_event_t* e)
{
    lv_obj_t* list = lv_event_get_target(e);
    lv_event_code_t event = lv_event_get_code(e);
    my_list_ext_t* ext = lv_obj_get_user_data(list);
    
    shortcut_keys_event_handler(e);

    if (event == LV_EVENT_KEY)
    {
        uint32_t key = *(uint32_t *)lv_event_get_param(e);

        if (ext->childs > 0)
        {
            int8_t tmp = ext->cur_sel;
            bool update = false;
            if (key == LV_ENCODER_KEY_CW)
            {
                tmp = tmp + 1;
                if (tmp < (int8_t)ext->childs)
                {
                    update = true;
                }
            }
            else if (key == LV_ENCODER_KEY_CC)
            {
                tmp = tmp - 1;
                if (tmp >= (int8_t)0)
                {   
                    update = true;
                }
            }
            if (update)
            {
                lv_obj_t* obj = lv_obj_get_child(list, ext->cur_sel);
                lv_obj_clear_state(obj, LV_STATE_FOCUSED);
                lv_event_send(obj, LV_EVENT_DEFOCUSED, NULL);
                ext->cur_sel = tmp;
                obj = lv_obj_get_child(list, ext->cur_sel);
                lv_obj_add_state(obj, LV_STATE_FOCUSED);
                lv_event_send(obj, LV_EVENT_FOCUSED, NULL);
            }
        }        
    }
    else if (event == LV_EVENT_FOCUSED)
    {
        if (ext->childs != 0)
        {
            lv_obj_t* obj = lv_obj_get_child(list, ext->cur_sel);
            lv_obj_add_state(obj, LV_STATE_FOCUSED); 
            lv_event_send(obj, LV_EVENT_FOCUSED, NULL);   
        }
    }
    
    if (ext->release_cb)
    {
        ext->release_cb(e, ext->cur_sel);
    }
}

static void my_list_btn_event_cb(lv_event_t* e)
{
    lv_obj_t* obj = lv_event_get_target(e);
    lv_event_code_t event = lv_event_get_code(e);

    if (event == LV_EVENT_FOCUSED)
    {
        uint8_t child_sum = lv_obj_get_child_cnt(obj);
        for (uint8_t cnt = 0; cnt < child_sum; cnt++)
        {
            lv_obj_t* child = lv_obj_get_child(obj, cnt);
            if (cnt == 0)
            {
                lv_label_set_long_mode(child, LV_LABEL_LONG_SCROLL) ;
            }
            lv_obj_add_state(child, LV_STATE_FOCUSED);
        }
    }
    else if (event == LV_EVENT_DEFOCUSED)
    {
        uint8_t child_sum = lv_obj_get_child_cnt(obj);
        for (uint8_t cnt = 0; cnt < child_sum; cnt++)
        {
            lv_obj_t* child = lv_obj_get_child(obj, cnt);
            if (cnt == 0)
            {
                lv_label_set_long_mode(child, LV_LABEL_LONG_CLIP);
            }
            lv_obj_clear_state(child, LV_STATE_FOCUSED);
        }        
    }
}



