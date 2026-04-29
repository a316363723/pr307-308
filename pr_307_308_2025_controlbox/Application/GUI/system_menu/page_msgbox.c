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
static void my_msgbox_btn_event_cb(lv_event_t* e);
static void my_msgbox_del_event_cb(lv_event_t* e);
static void msgbox_timecnt_cb(lv_timer_t* t);
static void msgbox_del(my_msgbox_ext_t* ext, lv_obj_t* obj);
static void my_msgbox_del_dmx_cb(lv_event_t* e);
/*********************
 *  STATIC VATIABLES
 *********************/
static my_msgbox_ext_t msgbox_ext;
/*********************
 *  GLOBAL VATIABLES
 *********************/

/*********************
 *  GLOBAL FUNCTIONS
 *********************/

my_msgbox_ext_t* my_msgbox_get_ext(void)
{
    return &msgbox_ext;
}

void my_msgbox_reset(void)
{
    memset(&msgbox_ext, 0, sizeof(msgbox_ext));
}

lv_obj_t* my_msgbox_create(my_msgbox_ext_t* p_ext)
{
    lv_obj_t* msgbox_panel = lv_obj_create(lv_scr_act());
    lv_obj_add_style(msgbox_panel, &style_rect_panel, 0);
    lv_obj_set_style_bg_color(msgbox_panel, lv_color_black(), 0);
    lv_obj_set_style_pad_all(msgbox_panel, 0, 0);
    lv_obj_set_size(msgbox_panel, lv_pct(100), lv_pct(100));
    
    lv_obj_t* title_cont = lv_obj_create(msgbox_panel);
    lv_obj_remove_style_all(title_cont);
    lv_obj_set_style_text_color(title_cont, lv_color_white(), 0);
    lv_obj_set_style_text_font(title_cont, Font_ResouceGet(FONT_22_BOLD), 0);
    lv_obj_set_size(title_cont, lv_pct(100), lv_pct(17));
    lv_obj_align(title_cont, LV_ALIGN_TOP_MID, 0, 0);

    lv_obj_t* title = lv_label_create(title_cont);        
    if (p_ext->title_str)
        lv_label_set_text(title, p_ext->title_str);
    else 
        lv_obj_add_flag(title, LV_OBJ_FLAG_HIDDEN);
    lv_obj_center(title);

    lv_obj_t* img_cont = lv_obj_create(msgbox_panel);
    lv_obj_remove_style_all(img_cont);
    lv_obj_set_style_text_color(img_cont, lv_color_white(), 0);
    lv_obj_set_style_text_font(img_cont, Font_ResouceGet(FONT_22), 0);
    lv_obj_set_size(img_cont, lv_pct(100), lv_pct(50));
    lv_obj_align_to(img_cont, title_cont, LV_ALIGN_OUT_BOTTOM_MID, 0, 0);

    lv_obj_t* img = lv_img_create(img_cont);
    lv_img_set_src(img, p_ext->body_img);
	lv_obj_set_style_img_recolor(img, lv_color_make(0xff, 0xff, 0xff), 0);
    lv_obj_set_style_img_recolor_opa(img, LV_OPA_100, 0);
    lv_obj_align(img, LV_ALIGN_TOP_MID, 0, 5);

    lv_obj_t* img_comment = lv_label_create(img_cont);   
	lv_obj_set_size(img_comment, lv_pct(100), lv_pct(50));
    lv_obj_set_style_text_font(img_comment, Font_ResouceGet(FONT_18), 0);
    if (p_ext->body_comment_str)
    {
        lv_label_set_long_mode(img_comment, LV_LABEL_LONG_WRAP);
        lv_obj_set_style_text_align(img_comment, LV_TEXT_ALIGN_CENTER, 0);
        lv_label_set_text(img_comment, p_ext->body_comment_str);                
        //lv_obj_align(img_comment, LV_ALIGN_BOTTOM_MID, 0, 0);
        lv_obj_align_to(img_comment, img, LV_ALIGN_OUT_BOTTOM_MID, 0, 16);
		lv_label_set_long_mode(img_comment, LV_LABEL_LONG_WRAP); 
    }
    else
    {
        lv_obj_align(img, LV_ALIGN_CENTER, 0, 0);        
        lv_obj_add_flag(img_comment, LV_OBJ_FLAG_HIDDEN);
    }

    lv_obj_t* btn_cont = lv_obj_create(msgbox_panel);
    lv_obj_add_style(btn_cont, &style_rect_panel, 0);
    lv_obj_set_style_bg_color(btn_cont, lv_color_black(), 0);
    lv_obj_set_flex_flow(btn_cont, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(btn_cont, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_gap(btn_cont, 40, 0);
    lv_obj_set_size(btn_cont, lv_pct(100), lv_pct(30));
    lv_obj_align_to(btn_cont, img_cont, LV_ALIGN_OUT_BOTTOM_MID, 0, 0);    
    lv_obj_set_user_data(btn_cont, p_ext);

    lv_obj_t* yes_btn = lv_obj_create(btn_cont);
    lv_obj_clear_flag(yes_btn, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_style(yes_btn, &style_common_btn, 0);
    lv_obj_add_style(yes_btn, &style_common_focued_btn, LV_STATE_FOCUSED);
    lv_obj_set_style_radius(yes_btn, 5, 0);
    lv_obj_set_style_radius(yes_btn, 5, LV_STATE_FOCUSED);        
    lv_obj_set_size(yes_btn, 75, 40);
    
    lv_obj_t* label = lv_label_create(yes_btn);
    lv_obj_set_style_text_font(label, Font_ResouceGet(FONT_18), 0);
    lv_label_set_text(label, Lang_GetStringByID(STRING_ID_YES));
    lv_obj_center(label);

    lv_obj_t* no_btn = lv_obj_create(btn_cont);
    lv_obj_clear_flag(no_btn, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_style(no_btn, &style_common_btn, 0);
    lv_obj_add_style(no_btn, &style_common_focued_btn, LV_STATE_FOCUSED);
    lv_obj_set_style_radius(no_btn, 5, 0);
    lv_obj_set_style_radius(no_btn, 5, LV_STATE_FOCUSED);        
    lv_obj_set_size(no_btn, 75, 40);

    lv_obj_t* label1 = lv_label_create(no_btn);
    lv_obj_set_style_text_font(label1, Font_ResouceGet(FONT_18), 0);
    lv_label_set_text(label1, Lang_GetStringByID(STRING_ID_NO));
    lv_obj_center(label1);

    lv_obj_add_event_cb(btn_cont, my_msgbox_btn_event_cb, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(msgbox_panel, my_msgbox_del_event_cb, LV_EVENT_DELETE, p_ext);
    p_ext->msgbox = msgbox_panel;
    
    lv_group_t* group = lv_group_get_default();
    lv_group_add_obj(group,  btn_cont);
    lv_group_focus_obj(btn_cont);
    lv_group_set_editing(group, true);

    p_ext->msgbox_timer = lv_timer_create(msgbox_timecnt_cb, 1000, btn_cont);

    return msgbox_panel;
}

lv_obj_t* my_msgbox1_create(my_msgbox_ext_t* p_ext)
{
	lv_obj_t* msgbox_panel = lv_obj_create(lv_scr_act());
    lv_obj_add_style(msgbox_panel, &style_rect_panel, 0);
    lv_obj_set_style_bg_color(msgbox_panel, lv_color_black(), 0);
    lv_obj_set_style_pad_all(msgbox_panel, 0, 0);
    lv_obj_set_size(msgbox_panel, lv_pct(100), lv_pct(100));
    
    lv_obj_t* title_cont = lv_obj_create(msgbox_panel);
    lv_obj_remove_style_all(title_cont);
    lv_obj_set_style_text_color(title_cont, lv_color_white(), 0);
    lv_obj_set_style_text_font(title_cont, Font_ResouceGet(FONT_22_BOLD), 0);
    lv_obj_set_size(title_cont, lv_pct(100), lv_pct(17));
    lv_obj_align(title_cont, LV_ALIGN_TOP_MID, 0, 0);

    lv_obj_t* title = lv_label_create(title_cont);        
    if (p_ext->title_str)
        lv_label_set_text(title, p_ext->title_str);
    else 
        lv_obj_add_flag(title, LV_OBJ_FLAG_HIDDEN);
    lv_obj_center(title);

    lv_obj_t* img_cont = lv_obj_create(msgbox_panel);
    lv_obj_remove_style_all(img_cont);
    lv_obj_set_style_text_color(img_cont, lv_color_white(), 0);
    lv_obj_set_style_text_font(img_cont, Font_ResouceGet(FONT_22), 0);
    lv_obj_set_size(img_cont, lv_pct(100), lv_pct(50));
    lv_obj_align_to(img_cont, title_cont, LV_ALIGN_OUT_BOTTOM_MID, 0, 0);

    lv_obj_t* img = lv_img_create(img_cont);
    lv_img_set_src(img, p_ext->body_img);
	lv_obj_set_style_img_recolor(img, lv_color_make(0xff, 0xff, 0xff), 0);
    lv_obj_set_style_img_recolor_opa(img, LV_OPA_100, 0);
    lv_obj_align(img, LV_ALIGN_TOP_MID, 0, 5);

    lv_obj_t* img_comment = lv_label_create(img_cont);   
	lv_obj_set_size(img_comment, lv_pct(100), lv_pct(50));
    lv_obj_set_style_text_font(img_comment, Font_ResouceGet(FONT_18), 0);
    if (p_ext->body_comment_str)
    {
        lv_label_set_long_mode(img_comment, LV_LABEL_LONG_WRAP);
        lv_obj_set_style_text_align(img_comment, LV_TEXT_ALIGN_CENTER, 0);
        lv_label_set_text(img_comment, p_ext->body_comment_str);                
        //lv_obj_align(img_comment, LV_ALIGN_BOTTOM_MID, 0, 0);
        lv_obj_align_to(img_comment, img, LV_ALIGN_OUT_BOTTOM_MID, 0, 16);
		lv_label_set_long_mode(img_comment, LV_LABEL_LONG_WRAP); 
    }
    else
    {
        lv_obj_align(img, LV_ALIGN_CENTER, 0, 0);        
        lv_obj_add_flag(img_comment, LV_OBJ_FLAG_HIDDEN);
    }

    lv_obj_t* btn_cont = lv_obj_create(msgbox_panel);
    lv_obj_add_style(btn_cont, &style_rect_panel, 0);
    lv_obj_set_style_bg_color(btn_cont, lv_color_black(), 0);
    lv_obj_set_flex_flow(btn_cont, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(btn_cont, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_gap(btn_cont, 40, 0);
    lv_obj_set_size(btn_cont, lv_pct(100), lv_pct(30));
    lv_obj_align_to(btn_cont, img_cont, LV_ALIGN_OUT_BOTTOM_MID, 0, 0);    
    lv_obj_set_user_data(btn_cont, p_ext);

    lv_obj_t* yes_btn = lv_obj_create(btn_cont);
    lv_obj_clear_flag(yes_btn, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_style(yes_btn, &style_common_btn, 0);
    lv_obj_add_style(yes_btn, &style_common_focued_btn, LV_STATE_FOCUSED);
    lv_obj_set_style_radius(yes_btn, 15, 0);
    lv_obj_set_style_radius(yes_btn, 15, LV_STATE_FOCUSED);        
    lv_obj_set_size(yes_btn, 75, 40);
    lv_obj_t* label = lv_label_create(yes_btn);
    lv_obj_set_style_text_font(label, Font_ResouceGet(FONT_18), 0);
    lv_label_set_text(label, "Close");
    lv_obj_center(label);
    lv_obj_add_event_cb(btn_cont, my_msgbox_del_dmx_cb, LV_EVENT_ALL, p_ext);
    lv_group_t* group = lv_group_get_default();
    lv_group_add_obj(group,  btn_cont);
    lv_group_set_editing(group, true);

    return msgbox_panel;
}

lv_obj_t* my_msgbox2_create(my_msgbox_ext_t* p_ext)
{
    lv_obj_t* msgbox_panel = lv_obj_create(lv_scr_act());
    lv_obj_add_style(msgbox_panel, &style_rect_panel, 0);
    lv_obj_set_style_bg_color(msgbox_panel, lv_color_black(), 0);
    lv_obj_set_style_pad_all(msgbox_panel, 0, 0);
    lv_obj_set_size(msgbox_panel, lv_pct(100), lv_pct(100));

    lv_obj_t* title_cont = lv_obj_create(msgbox_panel);
    lv_obj_remove_style_all(title_cont);
    lv_obj_set_style_text_color(title_cont, lv_color_white(), 0);
    lv_obj_set_style_text_font(title_cont, Font_ResouceGet(FONT_22_BOLD), 0);
    lv_obj_set_size(title_cont, lv_pct(100), lv_pct(17));
    lv_obj_align(title_cont, LV_ALIGN_TOP_MID, 0, 0);

    lv_obj_t* title = lv_label_create(title_cont);
    if (p_ext->title_str)
        lv_label_set_text(title, p_ext->title_str);
    else
        lv_obj_add_flag(title, LV_OBJ_FLAG_HIDDEN);
    lv_obj_center(title);

    lv_obj_t* img_cont = lv_obj_create(msgbox_panel);
    lv_obj_remove_style_all(img_cont);
    lv_obj_set_style_text_color(img_cont, lv_color_white(), 0);
    lv_obj_set_style_text_font(img_cont, Font_ResouceGet(FONT_22), 0);
    lv_obj_set_size(img_cont, lv_pct(100), lv_pct(37));
    lv_obj_align_to(img_cont, title_cont, LV_ALIGN_OUT_BOTTOM_MID, 0, 0);

    lv_obj_t* img = lv_img_create(img_cont);
    lv_img_set_src(img, p_ext->body_img);
    lv_obj_set_style_img_recolor(img, lv_color_make(0xff, 0xff, 0xff), 0);
    lv_obj_set_style_img_recolor_opa(img, LV_OPA_100, 0);
    lv_obj_align(img, LV_ALIGN_TOP_MID, 0, 5);
    lv_obj_t* btn_cont = lv_obj_create(msgbox_panel);
    lv_obj_add_style(btn_cont, &style_rect_panel, 0);
    lv_obj_set_style_bg_color(btn_cont, lv_color_black(), 0);
    lv_obj_set_flex_flow(btn_cont, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(btn_cont, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_gap(btn_cont, 40, 0);
    lv_obj_set_size(btn_cont, lv_pct(100), lv_pct(20));
    lv_obj_align_to(btn_cont, img_cont, LV_ALIGN_OUT_BOTTOM_MID, 0, 0);
    lv_obj_set_user_data(btn_cont, p_ext);

    lv_obj_t* yes_btn = lv_obj_create(btn_cont);
    lv_obj_clear_flag(yes_btn, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_style(yes_btn, &style_common_btn, 0);
    lv_obj_add_style(yes_btn, &style_common_focued_btn, LV_STATE_FOCUSED);
    lv_obj_set_style_radius(yes_btn, 15, 0);
    lv_obj_set_style_radius(yes_btn, 15, LV_STATE_FOCUSED);
    lv_obj_set_size(yes_btn, 75, 40);

    lv_obj_t* label = lv_label_create(yes_btn);
    lv_obj_set_style_text_font(label, Font_ResouceGet(FONT_18), 0);
    lv_label_set_text(label, Lang_GetStringByID(STRING_ID_YES));
    lv_obj_center(label);

    lv_obj_t* no_btn = lv_obj_create(btn_cont);
    lv_obj_clear_flag(no_btn, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_style(no_btn, &style_common_btn, 0);
    lv_obj_add_style(no_btn, &style_common_focued_btn, LV_STATE_FOCUSED);
    lv_obj_set_style_radius(no_btn, 15, 0);
    lv_obj_set_style_radius(no_btn, 15, LV_STATE_FOCUSED);
    lv_obj_set_size(no_btn, 75, 40);

    lv_obj_t* label1 = lv_label_create(no_btn);
    lv_obj_set_style_text_font(label1, Font_ResouceGet(FONT_18), 0);
    lv_label_set_text(label1, Lang_GetStringByID(STRING_ID_NO));
    lv_obj_center(label1);

     if (p_ext->body_comment_str)
     {
         lv_obj_t* label = lv_label_create(msgbox_panel);
         lv_obj_set_style_text_font(label, Font_ResouceGet(FONT_14), 0);
         lv_obj_set_style_text_color(label, lv_color_white(), 0);
         lv_label_set_long_mode(label, LV_LABEL_LONG_WRAP);     /*Break the long lines*/
         lv_obj_set_width(label, 296);  /*Set smaller width to make the lines wrap*/
         lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, 0);
         lv_label_set_text(label, p_ext->body_comment_str);
         lv_obj_align_to(label, btn_cont, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);
     }
    lv_obj_add_event_cb(btn_cont, my_msgbox_btn_event_cb, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(msgbox_panel, my_msgbox_del_event_cb, LV_EVENT_DELETE, p_ext);
    p_ext->msgbox = msgbox_panel;

    lv_group_t* group = lv_group_get_default();
    lv_group_add_obj(group, btn_cont);
    lv_group_focus_obj(btn_cont);
    lv_group_set_editing(group, true);

    p_ext->msgbox_timer = lv_timer_create(msgbox_timecnt_cb, 1000, btn_cont);

    return msgbox_panel;
}

lv_obj_t* my_msgbox3_create(my_msgbox_ext_t* p_ext)
{
    lv_obj_t* msgbox_panel = lv_obj_create(lv_scr_act());
    lv_obj_add_style(msgbox_panel, &style_rect_panel, 0);
    lv_obj_set_style_bg_color(msgbox_panel, lv_color_make(0xd5, 0xd5, 0xd5), 0);
    lv_obj_set_style_pad_all(msgbox_panel, 0, 0);
    lv_obj_set_size(msgbox_panel, 234, 188);
    lv_obj_align(msgbox_panel, LV_ALIGN_CENTER, 0, 0);

    lv_obj_t* title_cont = lv_obj_create(msgbox_panel);
    lv_obj_remove_style_all(title_cont);
    lv_obj_set_style_text_color(title_cont, lv_color_white(), 0);
    lv_obj_set_style_text_font(title_cont, Font_ResouceGet(FONT_22_BOLD), 0);
    lv_obj_set_size(title_cont, lv_pct(100), lv_pct(17));
    lv_obj_align(title_cont, LV_ALIGN_TOP_MID, 0, 10);

    lv_obj_t* title = lv_label_create(title_cont);
    if (p_ext->title_str)
        lv_label_set_text(title, p_ext->title_str);
    else
        lv_obj_add_flag(title, LV_OBJ_FLAG_HIDDEN);
    lv_obj_center(title);

    lv_obj_t* img_cont = lv_obj_create(msgbox_panel);
    lv_obj_remove_style_all(img_cont);
    lv_obj_set_style_text_color(img_cont, lv_color_white(), 0);
    lv_obj_set_style_text_font(img_cont, Font_ResouceGet(FONT_22), 0);
    lv_obj_set_size(img_cont, lv_pct(100), lv_pct(50));
    lv_obj_align_to(img_cont, title_cont, LV_ALIGN_OUT_BOTTOM_MID, 0, 0);

    lv_obj_t* img = lv_img_create(img_cont);
    lv_obj_align(img, LV_ALIGN_TOP_MID, 0, 5);
    if(p_ext->body_img)
        lv_img_set_src(img, p_ext->body_img);
    else
        lv_obj_add_flag(img, LV_OBJ_FLAG_HIDDEN);
    

    lv_obj_t* img_comment = lv_label_create(img_cont);
    lv_obj_set_style_text_font(img_comment, Font_ResouceGet(FONT_14_Heavy), 0);
    lv_obj_set_style_text_color(img_cont, lv_color_make(0xc4, 0x38, 0x2b), 0);
    if (p_ext->body_comment_str)
    {
        lv_obj_set_width(img_comment, 230);  /*Set smaller width to make the lines wrap*/
        lv_label_set_long_mode(img_comment, LV_LABEL_LONG_WRAP);
        lv_obj_set_style_text_align(img_comment, LV_TEXT_ALIGN_CENTER, 0);
        lv_label_set_text(img_comment, p_ext->body_comment_str);
        //lv_obj_align(img_comment, LV_ALIGN_BOTTOM_MID, 0, 0);
        lv_obj_align_to(img_comment, img, LV_ALIGN_OUT_BOTTOM_MID, 0, 0);
    }
    else
    {
        lv_obj_align(img, LV_ALIGN_CENTER, 0, 0);
        lv_obj_add_flag(img_comment, LV_OBJ_FLAG_HIDDEN);
    }

    lv_obj_t* btn_cont = lv_obj_create(msgbox_panel);
    lv_obj_add_style(btn_cont, &style_rect_panel, 0);
    lv_obj_set_style_bg_color(btn_cont, lv_color_make(0xd5, 0xd5, 0xd5), 0);
    lv_obj_set_flex_flow(btn_cont, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(btn_cont, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_gap(btn_cont, 40, 0);
    lv_obj_set_size(btn_cont, lv_pct(100), lv_pct(30));
    lv_obj_align_to(btn_cont, img_cont, LV_ALIGN_OUT_BOTTOM_MID, 0, 0);
    lv_obj_set_user_data(btn_cont, p_ext);

    lv_obj_t* yes_btn = lv_obj_create(btn_cont);
    lv_obj_clear_flag(yes_btn, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_style(yes_btn, &style_common_btn, 0);
    lv_obj_add_style(yes_btn, &style_common_focued_btn, LV_STATE_FOCUSED);
    lv_obj_set_style_radius(yes_btn, 5, 0);
    lv_obj_set_style_radius(yes_btn, 5, LV_STATE_FOCUSED);
    lv_obj_set_size(yes_btn, 75, 32);

    lv_obj_t* label = lv_label_create(yes_btn);
    lv_obj_set_style_text_font(label, Font_ResouceGet(FONT_18), 0);
    lv_label_set_text(label, Lang_GetStringByID(STRING_ID_YOKE_RESET_YES));
    lv_obj_center(label);

    lv_obj_t* no_btn = lv_obj_create(btn_cont);
    lv_obj_clear_flag(no_btn, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_style(no_btn, &style_common_btn, 0);
    lv_obj_add_style(no_btn, &style_common_focued_btn, LV_STATE_FOCUSED);
    lv_obj_set_style_radius(no_btn, 5, 0);
    lv_obj_set_style_radius(no_btn, 5, LV_STATE_FOCUSED);
    lv_obj_set_size(no_btn, 75, 32);

    lv_obj_t* label1 = lv_label_create(no_btn);
    lv_obj_set_style_text_font(label1, Font_ResouceGet(FONT_18), 0);
    lv_label_set_text(label1, Lang_GetStringByID(STRING_ID_YOKE_RESET_NO));
    lv_obj_center(label1);

    lv_obj_add_event_cb(btn_cont, my_msgbox_btn_event_cb, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(msgbox_panel, my_msgbox_del_event_cb, LV_EVENT_DELETE, p_ext);
    p_ext->msgbox = msgbox_panel;

    lv_group_t* group = lv_group_get_default();
    lv_group_add_obj(group, btn_cont);
    lv_group_focus_obj(btn_cont);
    lv_group_set_editing(group, true);

    p_ext->msgbox_timer = lv_timer_create(msgbox_timecnt_cb, 1000, btn_cont);

    return msgbox_panel;
}

lv_obj_t* my_msgbox4_create(my_msgbox_ext_t* p_ext)
{
	lv_obj_t* msgbox_panel = lv_obj_create(lv_scr_act());
    lv_obj_add_style(msgbox_panel, &style_rect_panel, 0);
    lv_obj_set_style_bg_color(msgbox_panel, lv_color_black(), 0);
    lv_obj_set_style_pad_all(msgbox_panel, 0, 0);
    lv_obj_set_size(msgbox_panel, lv_pct(100), lv_pct(100));
    
    lv_obj_t* title_cont = lv_obj_create(msgbox_panel);
    lv_obj_remove_style_all(title_cont);
    lv_obj_set_style_text_color(title_cont, lv_color_white(), 0);
    lv_obj_set_style_text_font(title_cont, Font_ResouceGet(FONT_22_BOLD), 0);
    lv_obj_set_size(title_cont, lv_pct(100), lv_pct(17));
    lv_obj_align(title_cont, LV_ALIGN_TOP_MID, 0, 0);

    lv_obj_t* title = lv_label_create(title_cont);        
    if (p_ext->title_str)
        lv_label_set_text(title, p_ext->title_str);
    else 
        lv_obj_add_flag(title, LV_OBJ_FLAG_HIDDEN);
    lv_obj_center(title);

    lv_obj_t* img_cont = lv_obj_create(msgbox_panel);
    lv_obj_remove_style_all(img_cont);
    lv_obj_set_style_text_color(img_cont, lv_color_white(), 0);
    lv_obj_set_style_text_font(img_cont, Font_ResouceGet(FONT_22), 0);
    lv_obj_set_size(img_cont, lv_pct(100), lv_pct(50));
    lv_obj_align_to(img_cont, title_cont, LV_ALIGN_OUT_BOTTOM_MID, 0, 0);

    lv_obj_t* img = lv_img_create(img_cont);
    lv_img_set_src(img, p_ext->body_img);
	lv_obj_set_style_img_recolor(img, lv_color_make(0xff, 0xff, 0xff), 0);
    lv_obj_set_style_img_recolor_opa(img, LV_OPA_100, 0);
    lv_obj_align(img, LV_ALIGN_TOP_MID, 0, 0);

    lv_obj_t* img_comment = lv_label_create(img_cont);   
	lv_obj_set_size(img_comment, lv_pct(100), lv_pct(50));
    lv_obj_set_style_text_font(img_comment, Font_ResouceGet(FONT_18), 0);
    if (p_ext->body_comment_str)
    {
        lv_label_set_long_mode(img_comment, LV_LABEL_LONG_WRAP);
        lv_obj_set_style_text_align(img_comment, LV_TEXT_ALIGN_CENTER, 0);
        lv_label_set_text(img_comment, p_ext->body_comment_str);                
        //lv_obj_align(img_comment, LV_ALIGN_BOTTOM_MID, 0, 0);
        lv_obj_align_to(img_comment, img, LV_ALIGN_BOTTOM_MID, 0, 63);
		lv_label_set_long_mode(img_comment, LV_LABEL_LONG_WRAP); 
    }
    else
    {
        lv_obj_align(img, LV_ALIGN_BOTTOM_MID, 0, 0);        
        lv_obj_add_flag(img_comment, LV_OBJ_FLAG_HIDDEN);
    }

    lv_obj_t* btn_cont = lv_obj_create(msgbox_panel);
    lv_obj_add_style(btn_cont, &style_rect_panel, 0);
    lv_obj_set_style_bg_color(btn_cont, lv_color_black(), 0);
    lv_obj_set_flex_flow(btn_cont, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(btn_cont, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_gap(btn_cont, 40, 0);
    lv_obj_set_size(btn_cont, lv_pct(100), lv_pct(30));
    lv_obj_align_to(btn_cont, img_cont, LV_ALIGN_OUT_BOTTOM_MID, 0, 0);    
    lv_obj_set_user_data(btn_cont, p_ext);

    lv_obj_t* yes_btn = lv_obj_create(btn_cont);
    lv_obj_clear_flag(yes_btn, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_style(yes_btn, &style_common_btn, 0);
    lv_obj_add_style(yes_btn, &style_common_focued_btn, LV_STATE_FOCUSED);
    lv_obj_set_style_radius(yes_btn, 15, 0);
    lv_obj_set_style_radius(yes_btn, 15, LV_STATE_FOCUSED);        
    lv_obj_set_size(yes_btn, 75, 40);
    lv_obj_add_state(yes_btn, LV_STATE_FOCUSED);
    lv_obj_t* label = lv_label_create(yes_btn);
    lv_obj_set_style_text_font(label, Font_ResouceGet(FONT_18), 0);
    lv_label_set_text(label, Lang_GetStringByID(STRING_ID_CONFIRM));
    lv_obj_center(label);
    lv_obj_add_event_cb(btn_cont, my_msgbox_del_dmx_cb, LV_EVENT_ALL, p_ext);
    lv_group_t* group = lv_group_get_default();
    lv_group_add_obj(group,  btn_cont);

    lv_group_set_editing(group, true);
    return msgbox_panel;
}
/**********************
 *   STATIC FUNCTIONS
 **********************/

static void my_msgbox_btn_event_cb(lv_event_t* e)
{
    lv_event_code_t event = lv_event_get_code(e);
    lv_obj_t* msgbox = lv_event_get_target(e);
    my_msgbox_ext_t *ext = lv_obj_get_user_data(msgbox);

    shortcut_keys_event_handler(e);

    if (event == LV_EVENT_KEY)
    {
        uint32_t key = *(uint32_t*)lv_event_get_param(e);
        uint8_t childs_sum = lv_obj_get_child_cnt(msgbox);
        int8_t sel = ext->btn_sel;
        bool b_upd = false;

        
        if (LV_ENCODER_KEY_CW == key)
        {
            sel = sel + 1;
            if (sel < childs_sum)
            {                
                b_upd = true;
            }
        }
        else if (LV_ENCODER_KEY_CC == key)
        {
            sel = sel - 1;
            if (sel >= 0)
            {                
                b_upd = true;
            }
        }
        else if (LV_KEY_BACKSPACE == key)
        {
            ext->answer = MSGBOX_ID_NO;            
            msgbox_del(ext, msgbox);                  
        }

        if (b_upd)
        {
            lv_obj_t* obj = lv_obj_get_child(msgbox, ext->btn_sel);
            lv_obj_clear_state(obj, LV_STATE_FOCUSED);
            lv_event_send(obj, LV_EVENT_DEFOCUSED, NULL);
            ext->btn_sel = sel;
            obj = lv_obj_get_child(msgbox, ext->btn_sel);
            lv_obj_add_state(obj, LV_STATE_FOCUSED);
            lv_event_send(obj, LV_EVENT_FOCUSED, NULL);
        }
        
        ext->timecnt = 0;        
    }  
    else if (event == LV_EVENT_FOCUSED)
    {
        lv_obj_t* child = lv_obj_get_child(msgbox, ext->btn_sel);
        if (child != NULL)
        {
            lv_obj_add_state(child, LV_STATE_FOCUSED);
        }
    }
    else if (event == LV_EVENT_RELEASED)
    {             
        ext->answer = ext->btn_sel;
        msgbox_del(ext, msgbox);      
    }
    else if (event == LV_EVENT_DELETE)
    {
        lv_timer_del(ext->msgbox_timer);//当该对象删除的时候，需要将timer定时器删除...
        ext->msgbox_timer = NULL;
        UI_PRINTF("MsgBox has been deleted......\r\n");
    }

}

static void msgbox_timecnt_cb(lv_timer_t* t)
{
    my_msgbox_ext_t* ext = lv_obj_get_user_data(t->user_data);

   if (ext->type == MSGBOX_TYPE_TIMEOUT && !l2_screen_is_exist())
    {
        ext->timecnt += t->period;
        if (ext->timecnt >= ext->timeout)
        {
            ext->answer = MSGBOX_ID_NO;
            msgbox_del(ext, t->user_data);     
        }
    }
}

static void msgbox_del(my_msgbox_ext_t* ext, lv_obj_t* obj)
{
    lv_group_remove_obj(obj);
    lv_obj_del_async(ext->msgbox); 
    
    lv_group_t* group = lv_group_get_default();
    lv_group_set_editing(group, false);    
}

/**
 * @brief 如果用户没有做出选择就执行该删除函数
 * 
 * @param e 
 */
static void my_msgbox_del_event_cb(lv_event_t* e)
{
    lv_event_code_t event = lv_event_get_code(e);

    if (event == LV_EVENT_DELETE)
    {
        //do something.
        my_msgbox_ext_t* ext = lv_event_get_user_data(e);
        if (ext->msgbox_del_cb && ext->answer < MSGBOX_ID_NO_CONFIRM)
        {
            ext->msgbox_del_cb(ext);
        }
    }
}

/**
 * @brief 如果用户没有做出选择就执行该删除函数
 * 
 * @param e 
 */
static void my_msgbox_del_dmx_cb(lv_event_t* e)
{
    lv_event_code_t event = lv_event_get_code(e);

    if (event == LV_EVENT_RELEASED)
    {
        //do something.
        my_msgbox_ext_t* ext = lv_event_get_user_data(e);
        if (ext->msgbox_del_cb)
        {
            ext->msgbox_del_cb(ext);
        }
    }
}
