/*********************
 *      INCLUDES
 *********************/
#include "ui_common.h"
#include "page_widget.h"
#include "math.h"
#include "stdlib.h"
/*********************
 *      DEFINES
 *********************/
#define     ROLLER_RADIUS                           LIGHT_EFFECT_COMMON_RADIUS
#define     ROLLER_DEF_BG_COLOR                     lv_color_make(40, 40, 40) 
#define     ROLLER_DEF_TEXT_COLOR                   lv_color_make(120, 120, 120)

#define     ROLLER_SEL_DEF_BG_COLOR                 lv_color_make(255, 255, 255)
#define     ROLLER_SEL_DEF_TEXT_COLOR               lv_color_make(0, 0, 0)

#define     ROLLER_FOCUS_BG_COLOR                   lv_color_make(224, 224, 224)
#define     ROLLER_FOCUS_TEXT_COLOR                 lv_color_make(120, 120, 120)

#define     ROLLER_SEL_FOCUS_BG_COLOR                 RED_THEME_COLOR
#define     ROLLER_SEL_FOCUS_TEXT_COLOR               lv_color_make(255, 255, 255)

// #define     ROLLER_FOCUS_EDIT_BG_COLOR               lv_color_make(230, 230, 230)
// #define     ROLLER_FOCUS_EDIT_TEXT_COLOR             lv_color_make(0, 255, 250)

#define     ROLLER_SEL_EDIT_BG_COLOR                 RED_THEME_COLOR
//#define     ROLLER_SEL_EDIT_TEXT_COLOR               lv_color_make(108, 198, 255)
#define     ROLLER_SEL_EDIT_TEXT_COLOR               lv_color_make(255, 255, 255)

//143
#define     PARAM_DISP_disp_panel_WIDTH                  162    
#define     PARAM_DISP_disp_panel_HEIGHT                 120
//100

/*********************
 *      TPEDEFS
 *********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void param_setting_cct_refresh(lv_obj_t* disp_panel, struct db_fx_mode_arg* p_arg);
static void param_setting_hsi_refresh(lv_obj_t* disp_panel, struct db_fx_mode_arg* p_arg);
static void param_setting_rgb_refresh(lv_obj_t* disp_panel, struct db_fx_mode_arg* p_arg);
static void param_setting_gel_refresh(lv_obj_t* disp_panel, struct db_fx_mode_arg* p_arg);
static void param_setting_xy_refresh(lv_obj_t* disp_panel,  struct db_fx_mode_arg* p_arg);
static void param_setting_source_refresh(lv_obj_t* disp_panel, struct db_fx_mode_arg* p_arg);

static void limit_setting_cct_init(lv_obj_t* disp_panel,  struct db_fx_mode_limit_arg* p_arg);
static void limit_setting_hsi_init(lv_obj_t* disp_panel,  struct db_fx_mode_limit_arg* p_arg);
//static void lightmode_roller_event_cb(lv_event_t* e);
/*********************
 *  STATIC VATIABLES
 *********************/
static lighteffect_style_t lighteffect_style;
/*********************
 *  GLOBAL VATIABLES
 *********************/

/*********************
 *  GLOBAL FUNCTIONS
 *********************/

lighteffect_style_t* lighteffect_style_get(void)
{
    return &lighteffect_style;
}

void lighteffect_style_init(lighteffect_style_t *pstyle)
{
    lv_style_init(&pstyle->roller_style);
    lv_style_set_radius(&pstyle->roller_style, ROLLER_RADIUS);
    lv_style_set_bg_color(&pstyle->roller_style,  ROLLER_DEF_BG_COLOR);
    lv_style_set_text_color(&pstyle->roller_style, ROLLER_DEF_TEXT_COLOR);
    lv_style_set_pad_all(&pstyle->roller_style, 0);
    lv_style_set_text_font(&pstyle->roller_style, Font_ResouceGet(FONT_18));
    lv_style_set_border_width(&pstyle->roller_style, 0);
    lv_style_set_text_line_space(&pstyle->roller_style, 2);
    
    lv_style_init(&pstyle->roller_focus_style);
    lv_style_set_radius(&pstyle->roller_focus_style, ROLLER_RADIUS);
    lv_style_set_bg_color(&pstyle->roller_focus_style,  ROLLER_FOCUS_BG_COLOR);
    lv_style_set_text_color(&pstyle->roller_focus_style, ROLLER_FOCUS_TEXT_COLOR);    
}


lv_obj_t* lightmode_roller_create(lv_obj_t* parent, lv_coord_t w, lv_coord_t h)
{
    // PARAM_SETTING_CCT = 0,
    // PARAM_SETTING_HSI,
    // PARAM_SETTING_GEL,
    // PARAM_SETTING_SOURCE,
    // PARAM_SETTING_RGB,
    // PARAM_SETTING_XY,
    lighteffect_style_t* p_style = lighteffect_style_get();
    
    const char * opts = "CCT\nAdv.HSI\nGEL\nSOURCE\nRGB\nX Y";    
    lv_obj_t* roller = lv_roller_create(parent);

    //默认状态
    lv_obj_add_style(roller, &p_style->roller_style, LV_PART_MAIN);
    lv_obj_set_style_bg_color(roller, ROLLER_SEL_DEF_BG_COLOR, LV_PART_SELECTED);
    lv_obj_set_style_text_color(roller, ROLLER_SEL_DEF_TEXT_COLOR, LV_PART_SELECTED);
    lv_obj_set_style_radius(roller, 5, LV_PART_SELECTED);

    //聚焦状态
    lv_obj_add_style(roller, &p_style->roller_focus_style, LV_PART_MAIN | LV_STATE_FOCUSED);
    lv_obj_set_style_bg_color(roller, ROLLER_SEL_FOCUS_BG_COLOR, LV_PART_SELECTED | LV_STATE_FOCUSED);
    lv_obj_set_style_text_color(roller, ROLLER_SEL_FOCUS_TEXT_COLOR, LV_PART_SELECTED | LV_STATE_FOCUSED);

    //聚焦状态
    //lv_obj_add_style(roller, &p_style->roller_edit_style, LV_PART_MAIN);
    lv_obj_set_style_bg_color(roller, ROLLER_SEL_EDIT_BG_COLOR, LV_PART_SELECTED | LV_STATE_FOCUSED | LV_STATE_EDITED);
    lv_obj_set_style_text_font(roller, Font_ResouceGet(FONT_18_BOLD), LV_PART_SELECTED | LV_STATE_FOCUSED | LV_STATE_EDITED);
    lv_obj_set_style_text_color(roller, ROLLER_SEL_EDIT_TEXT_COLOR, LV_PART_SELECTED | LV_STATE_FOCUSED | LV_STATE_EDITED);

    lv_roller_set_options(roller, opts, LV_ROLLER_MODE_NORMAL);
    lv_roller_set_visible_row_count(roller, 3);
    lv_obj_set_size(roller, w, h);
    return roller;
}

lv_obj_t* lightmode_disp_panel_create(lv_obj_t* parent, lv_coord_t w, lv_coord_t h)
{
    lv_obj_t* frame = lv_obj_create(parent);
    lv_obj_add_style(frame, &style_rect_panel, 0);
    lv_obj_set_style_radius(frame, ROLLER_RADIUS, 0);
    lv_obj_set_size(frame, w, h);

    lv_obj_t* disp_panel = lv_obj_create(frame);
    lv_obj_set_size(disp_panel, lv_pct(88), lv_pct(83));
    lv_obj_add_style(disp_panel, &style_rect_panel, 0);
    lv_obj_clear_flag(disp_panel, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_text_font(disp_panel, Font_ResouceGet(FONT_14), 0);
    lv_obj_set_style_radius(disp_panel, ROLLER_RADIUS, 0);
    lv_obj_set_style_bg_color(disp_panel, lv_color_black(), 0);
    lv_obj_set_style_text_color(disp_panel, lv_color_white(), 0);

    lv_obj_center(disp_panel);

    lv_obj_t* label;
    for (uint8_t i = 0; i < 4; i++)
    {
        label = lv_label_create(disp_panel);
        lv_obj_add_flag(label, LV_OBJ_FLAG_HIDDEN);
    }

    return disp_panel;
}

void lightmode_roller_event_cb(lv_event_t* e)
{
    lv_obj_t* roller = lv_event_get_target(e);
    lv_event_code_t code = lv_event_get_code(e);
    light_mode_roller_ext_t* roller_ext = lv_obj_get_user_data(roller);
    
    shortcut_keys_event_handler(e);
    backspace_key_event_handler(e);

    if (roller_ext == NULL || roller_ext->setting == NULL)
    {
        return ;
    }

    if (code == LV_EVENT_KEY)
    {
        uint32_t key = *(uint32_t*)lv_event_get_param(e);
        if (key == LV_ENCODER_KEY_CC || key == LV_ENCODER_KEY_CW)
        {
            UI_PRINTF("lightmode_roller_event_cb selected:%d\r\n", lv_roller_get_selected(roller));
            lv_event_send(roller, LV_EVENT_VALUE_CHANGED, NULL);
        }
        else if (key == LV_KEY_BACKSPACE)
        {
            lv_group_t* group = lv_obj_get_group(roller);
            if (lv_group_get_editing(group))
            {
                lv_group_set_editing(group, false);
            }
        }
    }
    else if (code == LV_EVENT_VALUE_CHANGED)
    {
        if (roller_ext->value_changed_cb)
        {
            roller_ext->value_changed_cb(roller_ext);
            if (roller_ext->disp_panel != NULL)
            {
                lightmode_disp_panel_refresh(roller_ext->disp_panel, roller_ext->type , roller_ext->setting);            
            }                    
        }             
    }
    else if (code == LV_EVENT_RELEASED)
    {        
        UI_PRINTF(" if the release operation is ecnoder operation, then turn to the setting page, else cancel the edit mode.\r\n");        
        lv_indev_type_t indev_type = lv_indev_get_type(lv_indev_get_act());
        if (indev_type == LV_INDEV_TYPE_ENCODER)
        {
            uint8_t page_id = ui_param_setting_get_page(roller_ext->type);
            ui_set_param_setting(roller_ext->type, roller_ext->setting);
            screen_set_act_index(lv_group_user_get_focused_id(lv_obj_get_group(roller)));
            screen_load_page(page_id, 0, true);
        }
        else
        {
            lv_group_t* group = lv_obj_get_group(roller);
            if (lv_group_get_editing(group))
            {
                lv_group_set_editing(group, false);
            }
        }
    }
}

void lightmode_disp_panel_refresh(lv_obj_t* disp_panel, param_setting_type_t type, struct db_fx_mode_arg *setting)
{
    switch (type)
    {
    case PARAM_SETTING_CCT:             
        param_setting_cct_refresh(disp_panel, setting);
        break;
    case PARAM_SETTING_HSI:        
        param_setting_hsi_refresh(disp_panel, setting);
        break;
    case PARAM_SETTING_RGB:        
        param_setting_rgb_refresh(disp_panel, setting);
        break;
    case PARAM_SETTING_GEL:        
        param_setting_gel_refresh(disp_panel, setting);
        break;
    case PARAM_SETTING_SOURCE:        
        param_setting_source_refresh(disp_panel, setting);
        break;
    case PARAM_SETTING_XY:        
        param_setting_xy_refresh(disp_panel, setting);
        break;                        
    default:
        break;
    }
}

void lightmode_disp_panel_limit_setting_init(lv_obj_t* disp_panel, param_setting_type_t type, struct db_fx_mode_limit_arg *setting)
{
    lv_obj_clean(disp_panel); 
    switch (type)
    {
    case PARAM_SETTING_CCT:             
        limit_setting_cct_init(disp_panel, setting);
        break;
    case PARAM_SETTING_HSI:        
        limit_setting_hsi_init(disp_panel, setting);
        break;
    }
}

void lightmode_disp_panel_limit_setting_refresh(lv_obj_t* disp_panel, param_setting_type_t type, struct db_fx_mode_limit_arg *setting)
{
    switch (type)
    {
    case PARAM_SETTING_CCT:             
        limit_setting_cct_init(disp_panel, setting);
        break;
    case PARAM_SETTING_HSI:        
        limit_setting_hsi_init(disp_panel, setting);
        break;                    
    default:
        break;
    }
}

lv_obj_t* my_roller_create(lv_obj_t* parent, lv_coord_t w, lv_coord_t h, const char* str_options)
{
    lv_obj_t* roller = lv_roller_create(parent);
    lv_obj_set_style_text_font(roller, Font_ResouceGet(FONT_18_MEDIUM), 0);
    lv_obj_add_style(roller, &style_rect_panel, 0);
    lv_obj_set_style_radius(roller, 5, 0);
    lv_obj_set_style_text_line_space(roller, 2, 0);

    lv_obj_set_style_bg_color(roller, lv_color_make(40, 40, 40),        LV_PART_MAIN);
    lv_obj_set_style_bg_color(roller, lv_color_make(40, 40, 40),        LV_PART_SELECTED);
    lv_obj_set_style_bg_color(roller, RED_THEME_COLOR,     LV_PART_MAIN | LV_STATE_FOCUSED);
    lv_obj_set_style_bg_color(roller, RED_THEME_COLOR,     LV_PART_SELECTED | LV_STATE_FOCUSED);

    lv_obj_set_style_text_font(roller, Font_ResouceGet(FONT_18_MEDIUM),        LV_PART_SELECTED);
    lv_obj_set_style_text_font(roller, Font_ResouceGet(FONT_18_BOLD), LV_PART_SELECTED | LV_STATE_FOCUSED | LV_STATE_EDITED);

    lv_obj_set_style_text_color(roller, lv_color_make(80, 80,80),              LV_PART_MAIN);
    lv_obj_set_style_text_color(roller, lv_color_make(255, 255, 255),           LV_PART_SELECTED);
    lv_obj_set_style_text_color(roller, lv_color_black(),           LV_PART_MAIN | LV_STATE_FOCUSED);
    lv_obj_set_style_text_color(roller, lv_color_white(),              LV_PART_SELECTED | LV_STATE_FOCUSED);
    lv_obj_set_style_text_color(roller, lv_color_white(),                   LV_PART_SELECTED | LV_STATE_FOCUSED | LV_STATE_EDITED);
    
    lv_roller_set_options(roller, str_options, LV_ROLLER_MODE_NORMAL);    
    lv_roller_set_visible_row_count(roller, 3);
    lv_obj_set_size(roller, w, h);
    return roller;
}

void my_roller_event_cb(lv_event_t* e)
{
    lv_event_code_t event = lv_event_get_code(e);
    lv_obj_t* obj = lv_event_get_target(e);
    lv_group_t* group = lv_obj_get_group(obj);

    shortcut_keys_event_handler(e);
    backspace_key_event_handler(e);
    
    if (event == LV_EVENT_FOCUSED)
    {
        lv_obj_t* parent = lv_obj_get_parent(obj);
        lv_obj_add_state(parent, LV_STATE_FOCUSED);
    }
    else if (event == LV_EVENT_DEFOCUSED)
    {
        lv_obj_t* parent = lv_obj_get_parent(obj);
        lv_obj_clear_state(parent, LV_STATE_FOCUSED);
    }
     else if (event == LV_EVENT_RELEASED)
     {
         if (lv_group_get_editing(group))
         {
            lv_group_set_editing(group, false);
         }
     }
    else if (event == LV_EVENT_KEY && lv_group_get_editing(group))
    {
        uint32_t key = *(uint32_t*)lv_event_get_param(e);
        uint8_t selected = (uint8_t)lv_roller_get_selected(obj);
        my_roller_ext_t* ext = lv_obj_get_user_data(obj);
        
        if (key == LV_ENCODER_KEY_CC || key == LV_ENCODER_KEY_CW)
        {
            if (ext && ext->value_changed_cb)
            {
                ext->value_changed_cb(selected);
            }
        }
        else if (key == LV_KEY_BACKSPACE)
        {
            lv_event_send(obj, LV_EVENT_RELEASED, NULL);
        }
    }
}

lv_obj_t* state_btn_create(lv_obj_t *parent, state_btn_t *user_data, lv_coord_t w, lv_coord_t h)
{
    lv_obj_t* state_btn = lv_obj_create(parent);
    lv_obj_add_style(state_btn, &style_common_btn, 0);
    lv_obj_add_style(state_btn, &style_common_focued_btn, LV_STATE_FOCUSED);
    lv_obj_set_style_radius(state_btn, ROLLER_RADIUS, 0);
    lv_obj_set_style_radius(state_btn, ROLLER_RADIUS, LV_STATE_FOCUSED);
    lv_obj_set_size(state_btn, w, h);    

    lv_obj_t* name_label = lv_label_create(state_btn);
    lv_obj_t* imgbtn_cont = lv_obj_create(state_btn);
    lv_coord_t pos_x = 0;
    
    lv_obj_set_style_text_font(name_label, Font_ResouceGet(FONT_18), 0);
    lv_obj_remove_style_all(imgbtn_cont);    
    lv_obj_set_size(imgbtn_cont, LV_SIZE_CONTENT, LV_SIZE_CONTENT);

    user_data->btn = state_btn;
    for (uint8_t i = 0; i <= user_data->max; i++)
    {
        lv_obj_t* img = lv_img_create(imgbtn_cont);
        lv_img_set_src(img, user_data->img_map[i]);     
        lv_obj_set_pos(img, pos_x, 0);   

        pos_x += user_data->img_map[i]->header.w;
        pos_x += user_data->img_gap;
        UI_PRINTF("Img: %d\r\n", i);
    }
    
    lv_obj_set_user_data(state_btn, user_data);
    state_btn_refresh(state_btn, false, false);

    return state_btn;
}

lv_obj_t* trigger_btn_create(lv_obj_t* parent, trigger_btn_t* p_btn_dsc, lv_coord_t w, lv_coord_t h)
{
    
    LV_IMG_DECLARE(ImgPulseIcon)
    
    lv_obj_t* trigger_btn = lv_obj_create(parent);
    /*Add a transition to the the outline*/
    static lv_style_transition_dsc_t trans;
    static lv_style_prop_t props[] = { LV_STYLE_WIDTH, LV_STYLE_HEIGHT, LV_STYLE_PROP_INV };
    lv_style_transition_dsc_init(&trans, props, lv_anim_path_linear, 100, 0, NULL);
    lv_obj_set_style_shadow_ofs_y(trigger_btn, 0, 0);
    lv_obj_add_style(trigger_btn, &style_common_btn, 0);
    lv_obj_add_style(trigger_btn, &style_common_focued_btn, LV_STATE_FOCUSED);

    lv_obj_set_style_transform_width(trigger_btn, 2, LV_STATE_PRESSED);
    lv_obj_set_style_transform_height(trigger_btn, 2, LV_STATE_PRESSED); 
    lv_obj_set_style_radius(trigger_btn, ROLLER_RADIUS, 0);
    lv_obj_set_style_radius(trigger_btn, ROLLER_RADIUS, LV_STATE_FOCUSED);
    lv_obj_set_size(trigger_btn, w, h);    
    lv_obj_set_style_transition(trigger_btn, &trans, LV_STATE_PRESSED);

    lv_obj_t* name_label = lv_label_create(trigger_btn);
    lv_obj_t* img = lv_img_create(trigger_btn);

    lv_obj_set_style_text_font(name_label, Font_ResouceGet(FONT_18), 0);     
    lv_label_set_text(name_label, Lang_GetStringByID(STRING_ID_TRIGGER));
    lv_img_set_src(img, &ImgPulseIcon);
    lv_obj_set_style_img_recolor_opa(img, LV_OPA_100, 0);
    lv_obj_set_style_img_recolor(img, lv_color_make(93, 93, 93), 0);

#if (PROJECT_TYPE == 308)
    lv_obj_align(name_label, LV_ALIGN_LEFT_MID, p_btn_dsc->title_to_top, 0);
    lv_obj_align(img, LV_ALIGN_RIGHT_MID, p_btn_dsc->img_to_top, 0);
#else
    lv_obj_align(name_label, LV_ALIGN_TOP_MID, 0,p_btn_dsc->title_to_top);
    lv_obj_align(img, LV_ALIGN_TOP_MID, 0, p_btn_dsc->img_to_top);
#endif

    lv_obj_set_user_data(trigger_btn, p_btn_dsc);
    lv_obj_add_event_cb(trigger_btn, trigger_btn_event, LV_EVENT_ALL, NULL);

    p_btn_dsc->anim_is_running = false;
    p_btn_dsc->btn = trigger_btn;

    return trigger_btn;
}

void trigger_btn_event(lv_event_t* e)
{
    lv_event_code_t event = lv_event_get_code(e);
    lv_obj_t* target = lv_event_get_target(e);
    trigger_btn_t *p_btn_dsc = lv_obj_get_user_data(target);
    lv_obj_t* img = lv_obj_get_child(target, -1);

    shortcut_keys_event_handler(e);
    backspace_key_event_handler(e);

    if (LV_EVENT_PRESSED == event)
    {
        lv_obj_set_style_img_recolor(img, lv_color_white(), 0);
    }
    else if (LV_EVENT_FOCUSED == event)
    {
        lv_obj_set_style_img_recolor(img, lv_color_black(), 0);
    }
    else if (LV_EVENT_DEFOCUSED == event)
    {
        lv_obj_set_style_img_recolor(img, lv_color_make(93, 93, 93), 0);
    }
    else if (LV_EVENT_RELEASED == event)
    {
        lv_group_t* group = lv_obj_get_group(target);
        lv_obj_set_style_img_recolor(img, lv_color_make(0, 0, 0), 0);
        if (lv_group_get_focused(group) != target)
        {
            lv_obj_clear_state(target, LV_STATE_FOCUSED);
            lv_obj_invalidate(target);
            return;
        }        

        if (p_btn_dsc && p_btn_dsc->released_cb)
        {
            p_btn_dsc->released_cb();
        }
        
        
    }
}

void state_btn_refresh(lv_obj_t* trigger_btn, bool focused, bool editing)
{
    state_btn_t* p_btn_dsc = lv_obj_get_user_data(trigger_btn);
    lv_obj_t*    name_label   = lv_obj_get_child(trigger_btn, 0);
    lv_obj_t*    imgbtn_cont  = lv_obj_get_child(trigger_btn, -1);    
    uint8_t      child_sum    = lv_obj_get_child_cnt(imgbtn_cont);
    lv_obj_t*    obj;
    lv_color_t img_def_color;
    lv_color_t img_focus_color;

    if (focused)
    {
        img_def_color   = lv_color_make(0, 0, 0);
        img_focus_color = lv_color_white();       
    }
    else
    {
        img_def_color   = lv_color_make(87, 87, 87);
        img_focus_color = lv_color_make(255, 255, 255);
    }

    for(uint8_t i = 0; i < child_sum; i++)
    {
        //focus聚焦
        obj = lv_obj_get_child(imgbtn_cont, i);
        if (p_btn_dsc->cur_index == i)
        {
            lv_obj_set_style_img_recolor_opa(obj, LV_OPA_100, 0);
            lv_obj_set_style_img_recolor(obj, img_focus_color, 0);
        }
        else
        {
            lv_obj_set_style_img_recolor_opa(obj, LV_OPA_100, 0);
            lv_obj_set_style_img_recolor(obj, img_def_color, 0);
        }
    }

    lv_label_set_text(name_label, p_btn_dsc->map[p_btn_dsc->cur_index]);
    
#if (PROJECT_TYPE == 308)    
    lv_obj_align(name_label, LV_ALIGN_LEFT_MID, p_btn_dsc->title_to_top, 0);
    lv_obj_align(imgbtn_cont, LV_ALIGN_RIGHT_MID, p_btn_dsc->img_to_top, 0); 
#else 
    lv_obj_align(name_label, LV_ALIGN_TOP_MID, 0, p_btn_dsc->title_to_top);
    lv_obj_align(imgbtn_cont, LV_ALIGN_TOP_MID, 0, p_btn_dsc->img_to_top); 
#endif    
}

void state_btn_event(lv_event_t* e)
{
    lv_obj_t* obj = lv_event_get_target(e);
    lv_event_code_t event = lv_event_get_code(e);
    state_btn_t* p_btn_dsc = lv_obj_get_user_data(obj);
    lv_group_t* group = lv_obj_get_group(obj);
    
    shortcut_keys_event_handler(e);
    backspace_key_event_handler(e);
    
    if (LV_EVENT_FOCUSED == event)
    {
        if (p_btn_dsc->widget_refresh_cb)
        {
            p_btn_dsc->widget_refresh_cb(obj, true, false);
        }                
    }
    else if (LV_EVENT_DEFOCUSED == event)
    {
        if (p_btn_dsc->widget_refresh_cb)
        {
            p_btn_dsc->widget_refresh_cb(obj, false, false);
        }                 
    }
    else if (LV_EVENT_RELEASED == event)
    {
#if 0        
        if (lv_group_get_editing(group) == false)
        {
            lv_group_set_editing(group, true);
            if (p_btn_dsc->widget_refresh_cb)
            {
                p_btn_dsc->widget_refresh_cb(obj, true, true);
            }
        }
        else 
        {
            lv_group_set_editing(group, false);            
            if (p_btn_dsc->widget_refresh_cb)
            {
                p_btn_dsc->widget_refresh_cb(obj, true, false);
            }            
        }
#endif        
    }
    else if (LV_EVENT_CLICKED == event)
    {
        int8_t index = p_btn_dsc->cur_index + 1;
        
        if (index > p_btn_dsc->max)
        {
            index = (uint8_t)p_btn_dsc->min;
        }
        p_btn_dsc->cur_index = index;
        
        if (p_btn_dsc->update_cb)
        {
            p_btn_dsc->update_cb(p_btn_dsc);
        }     

        if (p_btn_dsc->widget_refresh_cb)
        {
            p_btn_dsc->widget_refresh_cb(obj, true, true);
        }        
    }
}

lv_obj_t* btnlist_create(lv_obj_t* parent, lv_coord_t w, lv_coord_t h, my_btnlist_ext_t* ext, const char** list_str_map)
{
    lv_obj_t* btnlist_obj = lv_obj_create(parent);
    lv_obj_add_style(btnlist_obj, &style_common_btn, 0);
    lv_obj_set_style_radius(btnlist_obj, 5, 0);
    lv_obj_add_style(btnlist_obj, &style_common_focued_btn, LV_STATE_FOCUSED);
    lv_obj_set_style_bg_color(btnlist_obj, lv_color_make(255, 255, 255), LV_STATE_FOCUSED);
    lv_obj_set_style_radius(btnlist_obj, 5, LV_STATE_FOCUSED);
    lv_obj_set_flex_flow(btnlist_obj, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_pad_gap(btnlist_obj, 2, 0);
    lv_obj_set_size(btnlist_obj, w, h);
    lv_obj_set_flex_align(btnlist_obj, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    for (uint8_t i = 0; i <=  ext->max; i++)
    {        
        lv_obj_t* btn = lv_obj_create(btnlist_obj);
        lv_obj_set_style_pad_all(btn, 0, 0);
        lv_obj_add_style(btn, &style_common_btn, 0);
        lv_obj_set_style_text_font(btn, Font_ResouceGet(FONT_18), 0);
        lv_obj_set_style_text_font(btn, Font_ResouceGet(FONT_18), LV_STATE_USER_1);
        lv_obj_set_style_text_font(btn, Font_ResouceGet(FONT_18_BOLD),  LV_STATE_USER_1 | LV_STATE_FOCUSED | LV_STATE_EDITED);

        lv_obj_set_style_radius(btn, 5, 0);        
        lv_obj_set_style_bg_color(btn,   lv_color_make(255, 255, 255),    LV_STATE_USER_1);
        lv_obj_set_style_bg_color(btn,   RED_THEME_COLOR,    LV_STATE_USER_1 | LV_STATE_FOCUSED);        
        lv_obj_set_style_bg_color(btn,   lv_color_white(),     LV_STATE_FOCUSED);

        lv_obj_set_style_text_color(btn, lv_color_make(77, 77, 77),  LV_STATE_USER_1);
        lv_obj_set_style_text_color(btn, lv_color_make(255, 255, 255),  LV_STATE_USER_1 | LV_STATE_FOCUSED);
        lv_obj_set_style_text_color(btn, lv_color_make(255, 255, 255),        LV_STATE_USER_1 | LV_STATE_FOCUSED | LV_STATE_EDITED);
        lv_obj_set_style_text_color(btn, lv_color_black(),  LV_STATE_FOCUSED);
        lv_obj_set_size(btn, lv_pct(91), lv_pct(45));

        lv_obj_t* label = lv_label_create(btn);
        lv_obj_set_size(label, lv_pct(100), LV_SIZE_CONTENT);
        lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, 0);
        lv_label_set_text(label, list_str_map[i]);        
        lv_obj_center(label);
        if (i == ext->cur_index)
        {
            lv_obj_add_state(btn, LV_STATE_FOCUSED);
        }
    }

    return btnlist_obj;
}

void btnlist_refresh(lv_obj_t* btnlist, int16_t next_index)
{
    my_btnlist_ext_t* p_ext = lv_obj_get_user_data(btnlist);
    if (p_ext == NULL)        return;

    //get the current obj
    lv_obj_t* sel_obj = lv_obj_get_child(btnlist, p_ext->cur_index);
    lv_obj_t* next_obj = lv_obj_get_child(btnlist, next_index);
    if (sel_obj && next_obj)
    {
        lv_obj_clear_state(sel_obj, LV_STATE_FOCUSED);   
        lv_obj_add_state(next_obj, LV_STATE_FOCUSED);  

        lv_obj_t* label = lv_obj_get_child(sel_obj, 0);
        lv_obj_center(label);
        label = lv_obj_get_child(next_obj, 0);
        lv_obj_center(label);

        p_ext->cur_index = next_index; 
    }

    if (p_ext->update_cb)
    {
        p_ext->update_cb(p_ext);
    }   
}

void btnlist_event_cb(lv_event_t* e)
{
    lv_obj_t* btnlist = lv_event_get_target(e);
    lv_event_code_t event = lv_event_get_code(e);
    lv_group_t* group = lv_obj_get_group(btnlist);
    my_btnlist_ext_t* p_ext_dsc = lv_obj_get_user_data(btnlist);

    shortcut_keys_event_handler(e);
    backspace_key_event_handler(e);

    if (event == LV_EVENT_FOCUSED)
    {
        uint8_t childs = lv_obj_get_child_cnt(btnlist);
        for (uint8_t i=0; i<childs; i++)
        {
            lv_obj_t* obj = lv_obj_get_child(btnlist, i);
            lv_obj_add_state(obj, LV_STATE_USER_1);            
        }
    }
    else if (event == LV_EVENT_DEFOCUSED)
    {
        uint8_t childs = lv_obj_get_child_cnt(btnlist);
        for (uint8_t i=0; i<childs; i++)
        {
            lv_obj_t* obj = lv_obj_get_child(btnlist, i);
            lv_obj_clear_state(obj, LV_STATE_USER_1);   
        }
    }
    else if (event == LV_EVENT_RELEASED)
    {        
        lv_indev_type_t indev_type = lv_indev_get_type(lv_indev_get_act());
        if (lv_group_get_editing(group))
        {
            lv_group_set_editing(group, false);
            uint8_t childs = lv_obj_get_child_cnt(btnlist);
            for (uint8_t i = 0; i < childs; i++)
            {
                lv_obj_t* obj = lv_obj_get_child(btnlist, i);
                lv_obj_clear_state(obj, LV_STATE_EDITED);
            }    

            if (p_ext_dsc->release_cb && indev_type == LV_INDEV_TYPE_ENCODER)
            {
                UI_PRINTF("Encoder Release callback!\r\n");            
                p_ext_dsc->release_cb(p_ext_dsc);
            }         
        }
        else
        {
            lv_group_set_editing(group, true);
            uint8_t childs = lv_obj_get_child_cnt(btnlist);
            for (uint8_t i=0; i<childs; i++)
            {
                lv_obj_t* obj = lv_obj_get_child(btnlist, i);
                lv_obj_add_state(obj, LV_STATE_EDITED);
            }
        }        
    }
    else if (event == LV_EVENT_VALUE_CHANGED)
    {
        UI_PRINTF("BTNLIST VALUE CHANGED!\r\n");
        if (p_ext_dsc->update_cb)
        {
            p_ext_dsc->update_cb(p_ext_dsc);            
        }                
    }
    else if (event == LV_EVENT_KEY)
    {
        uint32_t key = *(uint32_t *)lv_event_get_param(e);
        
        if (key == LV_ENCODER_KEY_CC)
        {
            int8_t index = p_ext_dsc->cur_index - 1;

            if (index < p_ext_dsc->min)
            {
                index = (int8_t)p_ext_dsc->min;
                return;
            }            
                        
            lv_obj_t* sel_obj = lv_obj_get_child(btnlist, p_ext_dsc->cur_index);
            lv_obj_clear_state(sel_obj, LV_STATE_FOCUSED);
            p_ext_dsc->cur_index = index;
            sel_obj = lv_obj_get_child(btnlist, p_ext_dsc->cur_index);
            lv_obj_add_state(sel_obj, LV_STATE_FOCUSED);
            lv_event_send(btnlist, LV_EVENT_VALUE_CHANGED, NULL);
        }
        else if (key == LV_ENCODER_KEY_CW)
        {
            int8_t index = p_ext_dsc->cur_index + 1;

            if (index > p_ext_dsc->max)
            {
                index = (int8_t)p_ext_dsc->max;
                return;
            }

            lv_obj_t* prev_obj = lv_obj_get_child(btnlist, p_ext_dsc->cur_index);
            lv_obj_clear_state(prev_obj, LV_STATE_FOCUSED);
            p_ext_dsc->cur_index = index;
            lv_obj_t* next_obj = lv_obj_get_child(btnlist, p_ext_dsc->cur_index);
            lv_obj_add_state(next_obj, LV_STATE_FOCUSED);
            lv_event_send(btnlist, LV_EVENT_VALUE_CHANGED, NULL);
        }
        else if (key == LV_KEY_BACKSPACE)
        {
            lv_event_send(btnlist, LV_EVENT_RELEASED, NULL);
            return;
        }        
    }
}

void refresh_light_mode_list(lv_obj_t* list, lv_state_t state)
{
    lv_obj_t* label;
    uint8_t child_cnt = lv_obj_get_child_cnt(list);
    uint8_t i;
    lv_color_t selected_color = lv_color_make(255, 255, 255);;
    lv_color_t default_color = lv_color_make(255, 255, 255);
    light_mode_list_ext_t* p_ext = lv_obj_get_user_data(list);
    const lv_font_t* font = Font_ResouceGet(FONT_18_MEDIUM);

    if (state == LV_STATE_DEFAULT) //默认情况下
    {
        selected_color = lv_color_make(255, 255, 255);
        default_color = lv_color_make(80,80, 80);
    }
    else if (state == LV_STATE_FOCUSED)
    {
        selected_color = lv_color_make(255, 255, 255);
        default_color = lv_color_make(0, 0, 0);
    }
    else if (state == (LV_STATE_FOCUSED | LV_STATE_EDITED))
    {
        selected_color = lv_color_white();
        default_color = lv_color_make(0, 0, 0);
        font = Font_ResouceGet(FONT_18_BOLD);
    }

    for (i = 0; i < child_cnt; i++)
    {
        label = lv_obj_get_child(list, i);
        if (i == p_ext->selected)
        {
            lv_obj_set_style_text_color(label, selected_color, 0);        //聚焦是黑色
            lv_obj_set_style_text_font(label, font, 0);
        }
        else  
        {
            lv_obj_set_style_text_color(label, default_color, 0);        //聚焦是黑色
            lv_obj_set_style_text_font(label, Font_ResouceGet(FONT_18_MEDIUM), 0);            
        }        
    }
}

void light_mode_list_event_cb(lv_event_t* e)
{
    lv_event_code_t event = lv_event_get_code(e);
    lv_obj_t* list = lv_event_get_target(e);
    light_mode_list_ext_t* p_ext = lv_obj_get_user_data(list);
    lv_group_t* group = lv_obj_get_group(list);

    shortcut_keys_event_handler(e);
    backspace_key_event_handler(e);

    if (event == LV_EVENT_FOCUSED)
    {
        if (p_ext->bubble) 
        {
            lv_obj_t* parent = lv_obj_get_parent(list);        
            lv_obj_add_state(parent, LV_STATE_FOCUSED);    
        }
        
        refresh_light_mode_list(list, LV_STATE_FOCUSED);
    }
    else if (event == LV_EVENT_DEFOCUSED)
    {
        if (p_ext->bubble)
        {
            lv_obj_t* parent = lv_obj_get_parent(list);
            lv_obj_clear_state(parent, LV_STATE_FOCUSED);
        }
        
        refresh_light_mode_list(list, LV_STATE_DEFAULT);
    }
    else if (event == LV_EVENT_VALUE_CHANGED)
    {
        lv_state_t state = LV_STATE_DEFAULT;

        if (lv_group_get_focused(group) == list) 
        {
            state = LV_STATE_FOCUSED;    
            if (lv_group_get_editing(group))
            {
                state |= LV_STATE_EDITED;
            }            
        }
        
        refresh_light_mode_list(list, state);
    }
    else if (event == LV_EVENT_RELEASED)
    {        
        if (lv_group_get_editing(group))
        {
            lv_group_set_editing(group, false);
            refresh_light_mode_list(list, LV_STATE_FOCUSED);
        }
        else
        {
            lv_group_set_editing(group, true);
            refresh_light_mode_list(list, LV_STATE_EDITED | LV_STATE_FOCUSED);
        }
    }
    else if (event == LV_EVENT_KEY && lv_group_get_editing(group))
    {
        uint32_t key = *(uint32_t *)lv_event_get_param(e);
        if (key == LV_ENCODER_KEY_CW)
        {
            int8_t selected = p_ext->selected + 1;
            if (selected >= p_ext->nums)
                return ;

            p_ext->selected = selected;
            p_ext->value_changed_cb(p_ext->selected);
            refresh_light_mode_list(list, LV_STATE_EDITED | LV_STATE_FOCUSED);
        }
        else if (key == LV_ENCODER_KEY_CC)
        {
            int8_t selected = p_ext->selected - 1;            
            if (selected < 0)
                return ;

            p_ext->selected = selected;          
            p_ext->value_changed_cb(p_ext->selected);  
            refresh_light_mode_list(list, LV_STATE_EDITED | LV_STATE_FOCUSED);
        }
        else if (key == LV_KEY_BACKSPACE)
        {
            lv_event_send(list, LV_EVENT_RELEASED, NULL);
        }
    }
}

lv_obj_t* light_mode_list_add_label(lv_obj_t* list, const char* str)
{
    light_mode_list_ext_t* p_ext = lv_obj_get_user_data(list);
    lv_obj_t* label1 = lv_label_create(list);

    lv_obj_set_width(label1, lv_pct(100));        
    lv_label_set_text(label1, str);
    p_ext->nums += 1;
    return label1;    
}

lv_obj_t* light_mode_list_create(lv_obj_t* parent, light_mode_list_ext_t* p_ext)
{
    lv_obj_t* list = lv_obj_create(parent);    
    lv_obj_add_style(list, &style_rect_panel, 0);    
    lv_obj_add_style(list, &style_common_focued_btn, LV_STATE_FOCUSED);
    lv_obj_set_style_pad_all(list, 0, 0);
    lv_obj_set_style_pad_gap(list, 0, 0);
    lv_obj_clear_flag(list, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_set_flex_flow(list, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_flex_flow(list, LV_FLEX_FLOW_COLUMN, 0);
    lv_obj_set_style_flex_main_place(list, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_text_font(list, Font_ResouceGet(FONT_18), 0);
    lv_obj_set_style_text_align(list, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_set_style_text_color(list, lv_color_white(), 0);

    p_ext->nums = 0;
    lv_obj_set_user_data(list, p_ext);
    lv_obj_add_event_cb(list, light_mode_list_event_cb, LV_EVENT_ALL, NULL);    
    return list;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/
static void param_setting_cct_refresh(lv_obj_t* disp_panel, struct db_fx_mode_arg* p_arg)
{
    char tmp[10];

    lv_obj_t* cct_label = lv_obj_get_child(disp_panel, 0);
    lv_obj_t* gm_label = lv_obj_get_child(disp_panel, 1);
    lv_obj_t* dummy_label1 = lv_obj_get_child(disp_panel, 2);
    lv_obj_t* dummy_label2 = lv_obj_get_child(disp_panel, 3);

    lv_obj_clear_flag(cct_label, LV_OBJ_FLAG_HIDDEN);
    lv_obj_clear_flag(gm_label, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(dummy_label1, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(dummy_label2, LV_OBJ_FLAG_HIDDEN);

    lv_label_set_text_fmt(cct_label, "CCT : %dK", p_arg->cct.cct);
    format_gm(tmp, sizeof(tmp), p_arg->cct.duv);
    lv_label_set_text_fmt(gm_label, "G/M : %s",  tmp);

    lv_obj_align(cct_label, LV_ALIGN_TOP_MID, 0, 20);
    lv_obj_align_to(gm_label, cct_label, LV_ALIGN_OUT_BOTTOM_MID, 0, 30);     
}

static void param_setting_hsi_refresh(lv_obj_t* disp_panel, struct db_fx_mode_arg* p_arg)
{
    lv_obj_t* hue_label = lv_obj_get_child(disp_panel, 0);
    lv_obj_t* sat_label = lv_obj_get_child(disp_panel, 1);    
    lv_obj_t* cct_label = lv_obj_get_child(disp_panel, 2);
    lv_obj_t* dummy_label2 = lv_obj_get_child(disp_panel, 3);

    lv_obj_clear_flag(hue_label, LV_OBJ_FLAG_HIDDEN);
    lv_obj_clear_flag(sat_label, LV_OBJ_FLAG_HIDDEN);
    lv_obj_clear_flag(cct_label, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(dummy_label2, LV_OBJ_FLAG_HIDDEN);

    lv_label_set_text_fmt(hue_label, "HUE : %d%s", p_arg->hsi.hue, TEMPERATURE_UNIT);
    lv_label_set_text_fmt(sat_label, "SAT : %d%%", p_arg->hsi.sat);
    lv_label_set_text_fmt(cct_label, "CCT : %dK",  p_arg->hsi.cct);

    lv_obj_align(hue_label, LV_ALIGN_TOP_MID, 0, 10);
    lv_obj_align_to(sat_label, hue_label, LV_ALIGN_OUT_BOTTOM_MID, 0, 15);  
    lv_obj_align_to(cct_label, sat_label, LV_ALIGN_OUT_BOTTOM_MID, 0, 15);
}

static void param_setting_rgb_refresh(lv_obj_t* disp_panel, struct db_fx_mode_arg* p_arg)
{
    lv_obj_t* r_label = lv_obj_get_child(disp_panel, 0);
    lv_obj_t* g_label = lv_obj_get_child(disp_panel, 1);    
    lv_obj_t* b_label = lv_obj_get_child(disp_panel, 2);
    lv_obj_t* dummy_label1 = lv_obj_get_child(disp_panel, 3);

    lv_obj_clear_flag(r_label, LV_OBJ_FLAG_HIDDEN);
    lv_obj_clear_flag(g_label, LV_OBJ_FLAG_HIDDEN);
    lv_obj_clear_flag(b_label, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(dummy_label1, LV_OBJ_FLAG_HIDDEN);

    format_intensity((char*)sdb, sizeof(sdb), p_arg->rgb.r);
    lv_label_set_text_fmt(r_label, "R: %s", sdb);    
    format_intensity((char*)sdb, sizeof(sdb), p_arg->rgb.g);
    lv_label_set_text_fmt(g_label, "G: %s", sdb);    
    format_intensity((char*)sdb, sizeof(sdb), p_arg->rgb.b);
    lv_label_set_text_fmt(b_label, "B: %s", sdb);    

    lv_obj_align(r_label, LV_ALIGN_TOP_MID, 0, 10);
    lv_obj_align_to(g_label, r_label, LV_ALIGN_OUT_BOTTOM_MID, 0, 15);
    lv_obj_align_to(b_label, g_label, LV_ALIGN_OUT_BOTTOM_MID, 0, 15);    
}

static void param_setting_gel_refresh(lv_obj_t* disp_panel, struct db_fx_mode_arg* p_arg)
{
    lv_obj_t* cct_label = lv_obj_get_child(disp_panel, 0);
    lv_obj_t* brand_label = lv_obj_get_child(disp_panel, 1);
    lv_obj_t* serials_label = lv_obj_get_child(disp_panel, 2);
    lv_obj_t* gel_name_label = lv_obj_get_child(disp_panel, 3);

    const char* cct_str = p_arg->gel.cct == 3200 ? "3200K" : "5600K";
    const char* brand_str = p_arg->gel.brand == LEE ? "L" : "R";
    const gel_dsc_t* p_cur_gel_dsc = gel_get_serial_dsc(p_arg->gel.brand, p_arg->gel.type[p_arg->gel.brand]);
    uint8_t index = p_arg->gel.color[p_arg->gel.brand][p_arg->gel.type[p_arg->gel.brand]];

    lv_obj_clear_flag(cct_label, LV_OBJ_FLAG_HIDDEN);
    lv_obj_clear_flag(brand_label, LV_OBJ_FLAG_HIDDEN);
    lv_obj_clear_flag(serials_label, LV_OBJ_FLAG_HIDDEN);
    lv_obj_clear_flag(gel_name_label, LV_OBJ_FLAG_HIDDEN);

    lv_label_set_text_fmt(cct_label, "%s", cct_str);
    lv_label_set_text_fmt(brand_label, "%s", brand_str);
    lv_label_set_text(serials_label, gel_get_serial_name(p_arg->gel.brand, p_arg->gel.type[p_arg->gel.brand]));

    char tmp_str[40];
    strcpy(tmp_str, p_cur_gel_dsc[index].name);
    my_str_replace(tmp_str, '\n', ' ');

    lv_snprintf(sdb, sizeof(sdb), "%s/%d", tmp_str, p_cur_gel_dsc[index].number); 
    lv_label_set_text(gel_name_label, sdb);

    lv_obj_align(cct_label, LV_ALIGN_TOP_MID, 0, 10);
    lv_obj_align_to(brand_label, cct_label, LV_ALIGN_OUT_BOTTOM_MID, 0, 5);
    lv_obj_align_to(serials_label, brand_label, LV_ALIGN_OUT_BOTTOM_MID, 0, 5);    
    lv_obj_align_to(gel_name_label, serials_label, LV_ALIGN_OUT_BOTTOM_MID, 0, 5);        
}

static void param_setting_xy_refresh(lv_obj_t* disp_panel, struct db_fx_mode_arg* p_arg)
{
    lv_obj_t* x_label = lv_obj_get_child(disp_panel, 0);
    lv_obj_t* y_label = lv_obj_get_child(disp_panel, 1);    
    lv_obj_t* dummy_label1 = lv_obj_get_child(disp_panel, 2);
    lv_obj_t* dummy_label2 = lv_obj_get_child(disp_panel, 3);

    lv_obj_clear_flag(x_label, LV_OBJ_FLAG_HIDDEN);
    lv_obj_clear_flag(y_label, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(dummy_label1, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(dummy_label2, LV_OBJ_FLAG_HIDDEN);

    lv_snprintf(sdb, sizeof(sdb), "X : 0.%04d", p_arg->xy.x);
    lv_label_set_text(x_label, sdb);

    lv_snprintf(sdb, sizeof(sdb), "Y : 0.%04d", p_arg->xy.y);
    lv_label_set_text(y_label, sdb);

    lv_obj_align(x_label, LV_ALIGN_TOP_MID, 0, 20);
    lv_obj_align_to(y_label, x_label, LV_ALIGN_OUT_BOTTOM_MID, 0, 30);      
}

static void param_setting_source_refresh(lv_obj_t* disp_panel, struct db_fx_mode_arg* p_arg)
{
    lv_obj_t* name_label = lv_obj_get_child(disp_panel, 0);
    lv_obj_t* dummy_label1 = lv_obj_get_child(disp_panel, 1);
    lv_obj_t* dummy_label2 = lv_obj_get_child(disp_panel, 2);
    lv_obj_t* dummy_label3 = lv_obj_get_child(disp_panel, 3);

    lv_obj_clear_flag(name_label, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(dummy_label1, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(dummy_label2, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(dummy_label3, LV_OBJ_FLAG_HIDDEN);

    lv_label_set_text(name_label, source_get_name(p_arg->source.type));
    //lv_obj_align(name_label, LV_ALIGN_TOP_MID, 0, 50);
    lv_obj_center(name_label);
}


static void limit_setting_cct_init(lv_obj_t* disp_panel, struct db_fx_mode_limit_arg* p_arg)
{
    lv_obj_t* title = lv_label_create(disp_panel);
    lv_label_set_text(title, "CCT");
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 5);

    lv_obj_t* up_limit_label = lv_label_create(disp_panel);
    lv_label_set_text_fmt(up_limit_label, "%s\n%dK", Lang_GetStringByID(STRING_ID_UPPER_LIMIT),p_arg->cct.max_cct);
    lv_obj_align_to(up_limit_label, title, LV_ALIGN_OUT_BOTTOM_MID, 0, 2);      
    lv_obj_set_style_text_align(up_limit_label, LV_TEXT_ALIGN_CENTER, 0);
    
    lv_obj_t* low_limit_label = lv_label_create(disp_panel);
    lv_obj_set_style_text_align(low_limit_label, LV_TEXT_ALIGN_CENTER, 0);
    lv_label_set_text_fmt(low_limit_label, "%s\n%dK", Lang_GetStringByID(STRING_ID_LOWER_LIMIT), p_arg->cct.min_cct);  
    lv_obj_align_to(low_limit_label, up_limit_label, LV_ALIGN_OUT_BOTTOM_MID, 0, 2);      
}

static void limit_setting_hsi_init(lv_obj_t* disp_panel,struct db_fx_mode_limit_arg* p_arg)
{
    lv_obj_t* title = lv_label_create(disp_panel);
    lv_label_set_text(title, "Adv.HSI");
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 5);    

    lv_obj_t* up_limit_label = lv_label_create(disp_panel);
    lv_obj_set_style_text_align(up_limit_label, LV_TEXT_ALIGN_CENTER, 0);
    lv_label_set_text_fmt(up_limit_label, "%s\n%d"TEMPERATURE_UNIT, Lang_GetStringByID(STRING_ID_UPPER_LIMIT), p_arg->hsi.max_hue);
    lv_obj_align_to(up_limit_label, title, LV_ALIGN_OUT_BOTTOM_MID, 0, 2);      

    lv_obj_t* low_limit_label = lv_label_create(disp_panel);
    lv_obj_set_style_text_align(low_limit_label, LV_TEXT_ALIGN_CENTER, 0);
    lv_label_set_text_fmt(low_limit_label, "%s\n%d"TEMPERATURE_UNIT, Lang_GetStringByID(STRING_ID_LOWER_LIMIT), p_arg->hsi.min_hue);  
    lv_obj_align_to(low_limit_label, up_limit_label, LV_ALIGN_OUT_BOTTOM_MID, 0, 2);
}





//////////////////////////////////////////////////////
#define ARC_INDICATOR_COLOR                      lv_color_make(255, 255, 255)
#define ARC_INDICATOR_BG_COLOR                   lv_color_make(106, 103, 103)

static state_btn_t state_btn_dsc;


lv_obj_t* circle_create(lv_obj_t* parent, uint16_t circle_size)
{
    lv_obj_t* circle = lv_arc_create(parent);
    lv_obj_set_size(circle, circle_size, circle_size);
    lv_arc_set_bg_angles(circle, 0, 360);
    lv_obj_align_to(circle, parent, LV_ALIGN_CENTER, 0, -10);
    lv_obj_set_style_bg_color(circle, lv_color_make(255, 255, 255), LV_PART_INDICATOR);
    lv_obj_set_style_arc_color(circle, lv_color_make(255, 255, 255), LV_PART_INDICATOR);
    lv_obj_set_style_bg_color(circle, lv_color_make(255, 255, 255), LV_PART_MAIN);
    lv_obj_set_style_arc_color(circle, lv_color_make(255, 255, 255), LV_PART_MAIN);
    lv_obj_set_style_arc_width(circle, 1, 0);
    lv_obj_set_style_arc_width(circle, 1, LV_PART_INDICATOR);
    lv_obj_remove_style(circle, NULL, LV_PART_KNOB);
    lv_obj_set_style_arc_rounded(circle, 0, 0);
    lv_obj_set_style_arc_rounded(circle, 0, LV_PART_INDICATOR); 
    return circle;

}

void adj_int_widget_creat(lv_obj_t * parent,uint16_t arc_size, uint16_t circle_size, uint16_t int_val, adj_btn_t* btn)
{   
    lv_obj_t* circle = circle_create(parent, circle_size);
    lv_obj_t* arc = lv_arc_create(circle);
    lv_arc_set_bg_angles(arc, 0, 360);
    lv_arc_set_range(arc, 0, UI_INTENSITY_MAX);
    lv_obj_set_size(arc, arc_size, arc_size);
    lv_obj_set_style_arc_width(arc, 7, 0);
    lv_obj_set_style_arc_width(arc, 7, LV_PART_INDICATOR);
    lv_obj_set_style_bg_color(arc, ARC_INDICATOR_COLOR, LV_PART_INDICATOR);
    lv_obj_set_style_arc_color(arc, ARC_INDICATOR_COLOR, LV_PART_INDICATOR);
    lv_obj_set_style_bg_color(arc, ARC_INDICATOR_BG_COLOR, LV_PART_MAIN);
    lv_obj_set_style_arc_color(arc, ARC_INDICATOR_BG_COLOR, LV_PART_MAIN);
    lv_arc_set_rotation(arc, 270);
    lv_obj_remove_style(arc, NULL, LV_PART_KNOB);
    lv_obj_align_to(arc, circle, LV_ALIGN_CENTER, 0, 0);
    lv_arc_set_value(arc, int_val);
    
    lv_obj_t*  label = lv_label_create(parent);
    lv_obj_set_style_text_font(label, Font_ResouceGet(FONT_14), 0);
    lv_obj_set_style_text_color(label, lv_color_white(), 0);
    lv_label_set_text(label, Lang_GetStringByID(STRING_ID_INTENSITY));
    lv_obj_align_to(label, parent, LV_ALIGN_BOTTOM_MID, 0, -2);

    lv_obj_t* intval_label = lv_label_create(parent);
//    lv_obj_set_style_text_font(intval_label, Font_ResouceGet(FONT_14_BOLD), 0);
//    lv_obj_set_style_text_font(intval_label, Font_ResouceGet(FONT_24_MEDIUM), 0);
//    lv_obj_set_style_text_font(intval_label, Font_ResouceGet(FONT_16_BOLD), LV_STATE_CHECKED);  
    lv_obj_set_style_text_letter_space(intval_label, 0, 0);
    lv_obj_set_style_text_color(intval_label, lv_color_white(), 0);
    
    btn->obj = parent;
    btn->value = int_val;
    const char* int_str =  intensity_get_value_str(btn);
    lv_label_set_text(intval_label, int_str);
    lv_obj_align_to(intval_label, circle, LV_ALIGN_CENTER, 0, 0);
}

void adj_cct_int_widget_creat(lv_obj_t * parent,uint16_t arc_size, uint16_t circle_size, uint16_t int_val, adj_btn_t* btn)
{   
    lv_obj_t* circle = lv_arc_create(parent);
    lv_obj_set_size(circle, circle_size, circle_size);
    lv_arc_set_bg_angles(circle, 0, 360);
    lv_obj_align_to(circle, parent, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_bg_color(circle, lv_color_make(255, 255, 255), LV_PART_INDICATOR);
    lv_obj_set_style_arc_color(circle, lv_color_make(255, 255, 255), LV_PART_INDICATOR);
    lv_obj_set_style_bg_color(circle, lv_color_make(255, 255, 255), LV_PART_MAIN);
    lv_obj_set_style_arc_color(circle, lv_color_make(255, 255, 255), LV_PART_MAIN);
    lv_obj_set_style_arc_width(circle, 1, 0);
    lv_obj_set_style_arc_width(circle, 1, LV_PART_INDICATOR);
    lv_obj_remove_style(circle, NULL, LV_PART_KNOB);
    lv_obj_set_style_arc_rounded(circle, 0, 0);
    lv_obj_set_style_arc_rounded(circle, 0, LV_PART_INDICATOR); 
    
    lv_obj_t* arc = lv_arc_create(circle);
    lv_arc_set_bg_angles(arc, 0, 360);
    lv_arc_set_range(arc, 0, UI_INTENSITY_MAX);
    lv_obj_set_size(arc, arc_size, arc_size);
    lv_obj_set_style_arc_width(arc, 7, 0);
    lv_obj_set_style_arc_width(arc, 7, LV_PART_INDICATOR);
    lv_obj_set_style_bg_color(arc, ARC_INDICATOR_COLOR, LV_PART_INDICATOR);
    lv_obj_set_style_arc_color(arc, ARC_INDICATOR_COLOR, LV_PART_INDICATOR);
    lv_obj_set_style_bg_color(arc, RED_THEME_COLOR, LV_PART_MAIN);
    lv_obj_set_style_arc_color(arc, RED_THEME_COLOR, LV_PART_MAIN);
    lv_arc_set_rotation(arc, 270);
    lv_obj_remove_style(arc, NULL, LV_PART_KNOB);
    lv_obj_align_to(arc, circle, LV_ALIGN_CENTER, 0, 0);
    lv_arc_set_value(arc, int_val);
    
//    lv_obj_t*  label = lv_label_create(parent);
//    lv_obj_set_style_text_font(label, Font_ResouceGet(FONT_18), 0);
//    lv_obj_set_style_text_color(label, lv_color_white(), 0);
//    lv_label_set_text(label, Lang_GetStringByID(STRING_ID_INTENSITY));
//    lv_obj_align_to(label, circle, LV_ALIGN_OUT_BOTTOM_MID, 0, 4);

    lv_obj_t* intval_label = lv_label_create(parent);
//    lv_obj_set_style_text_font(intval_label, Font_ResouceGet(FONT_24_MEDIUM), 0);
//    lv_obj_set_style_text_font(intval_label, Font_ResouceGet(FONT_24_BOLD), LV_STATE_EDITED);  
    lv_obj_set_style_text_letter_space(intval_label, 0, 0);
    lv_obj_set_style_text_color(intval_label, lv_color_white(), 0);
    
    btn->obj = parent;
    btn->value = int_val;
    const char* int_str =  intensity_get_value_str(btn);
    lv_label_set_text(intval_label, int_str);
    lv_obj_set_style_text_font(intval_label, Font_ResouceGet(FONT_24_BOLD), 0);
    lv_obj_align_to(intval_label, circle, LV_ALIGN_CENTER, -3, 0);
}

void adj_cct_widget_creat(lv_obj_t * parent, uint16_t img_zoom, uint16_t cct_val, adj_btn_t* btn, uint8_t type)
{   
    LV_IMG_DECLARE(cct_img);
    lv_obj_t* img = lv_img_create(parent);
    lv_img_set_src(img, &cct_img);    
    lv_obj_align(img, LV_ALIGN_CENTER, 0, -20); 
    lv_img_set_zoom(img, img_zoom);
    
    LV_IMG_DECLARE(ImgCCTSelector)
    lv_obj_t* img1 = lv_img_create(parent);
    lv_img_set_src(img1, &ImgCCTSelector);
    if(type == 1)//cct界面时
        lv_img_set_zoom(img1, 256);
    else
        lv_img_set_zoom(img1, 170);
    
    lv_obj_t* intval_label = lv_label_create(parent);
    lv_obj_set_style_text_letter_space(intval_label, 0, 0);
    lv_obj_set_style_text_color(intval_label, lv_color_white(), 0);
    
    btn->obj = parent;
    btn->value = cct_val;
    const char* int_str =  cct_get_value_str(btn);
    lv_label_set_text(intval_label, int_str);
    if(type == 1)//cct界面时
        lv_obj_set_style_text_font(intval_label, Font_ResouceGet(FONT_22_BOLD), 0);
    if(type == 1)//cct界面时
        lv_obj_align_to(intval_label, parent, LV_ALIGN_CENTER, 0, 30);
    else
	{
//		lv_obj_set_style_text_font(intval_label, Font_ResouceGet(FONT_14_BOLD), 0);
//		lv_obj_set_style_text_font(intval_label, Font_ResouceGet(FONT_14_MEDIUM), LV_STATE_EDITED);
        lv_obj_align_to(intval_label, parent, LV_ALIGN_BOTTOM_MID, -20, 0);
	}
}

void adj_decay_widget_creat(lv_obj_t * parent,uint16_t circle_size, uint16_t val, adj_btn_t* btn )
{   
    lv_obj_t* circle = circle_create(parent, circle_size);

    lv_obj_t*  label = lv_label_create(parent);
    lv_obj_set_style_text_font(label, Font_ResouceGet(FONT_14), 0);
    lv_obj_set_style_text_color(label, lv_color_white(), 0);
    lv_label_set_text(label, Lang_GetStringByID(STRING_ID_DECAY));
    lv_obj_align_to(label, parent, LV_ALIGN_BOTTOM_MID, 0, -2);
	
    lv_obj_t* intval_label = lv_label_create(parent);
    lv_obj_set_style_text_letter_space(intval_label, 0, 0);
    lv_obj_set_style_text_color(intval_label, lv_color_white(), 0);
   
    btn->obj = parent;
    btn->value = val;
    const char* int_str =  frequence_get_value_str(btn);
    lv_label_set_text(intval_label, int_str);
    lv_obj_align_to(intval_label, circle, LV_ALIGN_CENTER, 0, 0);
    
    LV_IMG_DECLARE(ImgFxFrq)
    
    int16_t c_angle = 0;
    int16_t x;
    int16_t y;
    int16_t angle_tmp = 0;
    lv_point_t center;
    center.x = 19;
    center.y = 45;
    for (uint8_t i = 0; i <10; i++)
    {
        lv_obj_t* img = lv_img_create(parent);
        lv_img_set_src(img, &ImgFxFrq);
        lv_img_set_zoom(img, 200);
        angle_tmp = 34 * (i);
        if (angle_tmp <= 90)
        {
            x = center.x - (34 * lv_trigo_cos(36 * (i)) >> LV_TRIGO_SHIFT);
            y = center.y - (34 * lv_trigo_sin(36 * (i)) >> LV_TRIGO_SHIFT);
        }
        else if (angle_tmp > 90 && angle_tmp <= 180)
        {
            x = center.x + (34 * lv_trigo_cos(180 - 36 * (i)) >> LV_TRIGO_SHIFT);
            y = center.y - (34 * lv_trigo_sin(180 - 36 * (i)) >> LV_TRIGO_SHIFT);
        }
        else if (angle_tmp > 180 && angle_tmp <= 270)
        {
            x = center.x + (34 *  lv_trigo_cos(36 * (i) - 180) >> LV_TRIGO_SHIFT);
            y = center.y + (34 * lv_trigo_sin(36 * (i) - 180) >> LV_TRIGO_SHIFT);
        }
        else
        {
            x = center.x - (34 * lv_trigo_cos(360 - 36 * (i)) >> LV_TRIGO_SHIFT);
            y = center.y + (34 * lv_trigo_sin(360 - 36 * (i)) >> LV_TRIGO_SHIFT);
        }
        lv_obj_set_style_img_recolor_opa(img, LV_OPA_100, 0);
        lv_obj_set_style_img_recolor(img, lv_color_make(0x4d, 0x4d, 0x4d), 0);

        x += 19;
        lv_obj_set_pos(img, x, y);
        lv_img_set_angle(img, c_angle + 40);
        c_angle += (360);
    }  

    adj_btn5_refresh(parent, btn);  
}

void adj_frq_widget_creat(lv_obj_t * parent,uint16_t circle_size, uint16_t val, adj_btn_t* btn )
{   
    lv_obj_t* circle = circle_create(parent, circle_size);

    lv_obj_t*  label = lv_label_create(parent);
    lv_obj_set_style_text_font(label, Font_ResouceGet(FONT_14), 0);
    lv_obj_set_style_text_color(label, lv_color_white(), 0);
    lv_label_set_text(label, Lang_GetStringByID(STRING_ID_FREQUENCY_FULL));
    lv_obj_align_to(label, parent, LV_ALIGN_BOTTOM_MID, 0, -2);
	
    lv_obj_t* intval_label = lv_label_create(parent);
    lv_obj_set_style_text_letter_space(intval_label, 0, 0);
    lv_obj_set_style_text_color(intval_label, lv_color_white(), 0);
   
    btn->obj = parent;
    btn->value = val;
    const char* int_str =  frequence_get_value_str(btn);
    lv_label_set_text(intval_label, int_str);
    lv_obj_align_to(intval_label, circle, LV_ALIGN_CENTER, 0, 0);
    
    LV_IMG_DECLARE(ImgFxFrq)
    
    int16_t c_angle = 0;
    int16_t x;
    int16_t y;
    int16_t angle_tmp = 0;
    lv_point_t center;
    center.x = 19;
    center.y = 45;
    for (uint8_t i = 0; i <10; i++)
    {
        lv_obj_t* img = lv_img_create(parent);
        lv_img_set_src(img, &ImgFxFrq);
        lv_img_set_zoom(img, 200);
        angle_tmp = 34 * (i);
        if (angle_tmp <= 90)
        {
            x = center.x - (34 * lv_trigo_cos(36 * (i)) >> LV_TRIGO_SHIFT);
            y = center.y - (34 * lv_trigo_sin(36 * (i)) >> LV_TRIGO_SHIFT);
        }
        else if (angle_tmp > 90 && angle_tmp <= 180)
        {
            x = center.x + (34 * lv_trigo_cos(180 - 36 * (i)) >> LV_TRIGO_SHIFT);
            y = center.y - (34 * lv_trigo_sin(180 - 36 * (i)) >> LV_TRIGO_SHIFT);
        }
        else if (angle_tmp > 180 && angle_tmp <= 270)
        {
            x = center.x + (34 *  lv_trigo_cos(36 * (i) - 180) >> LV_TRIGO_SHIFT);
            y = center.y + (34 * lv_trigo_sin(36 * (i) - 180) >> LV_TRIGO_SHIFT);
        }
        else
        {
            x = center.x - (34 * lv_trigo_cos(360 - 36 * (i)) >> LV_TRIGO_SHIFT);
            y = center.y + (34 * lv_trigo_sin(360 - 36 * (i)) >> LV_TRIGO_SHIFT);
        }
        lv_obj_set_style_img_recolor_opa(img, LV_OPA_100, 0);
        lv_obj_set_style_img_recolor(img, lv_color_make(0x4d, 0x4d, 0x4d), 0);

        x += 19;
        lv_obj_set_pos(img, x, y);
        lv_img_set_angle(img, c_angle + 40);
        c_angle += (360);
    }  

    adj_btn5_refresh(parent, btn);  
}

lv_obj_t* adj_state_widget_creat(lv_obj_t * parent, uint8_t* val, lv_coord_t w, lv_coord_t h, void (*update_cb)(void* data))
{   

    LV_IMG_DECLARE(ImgLoopIcon);
    LV_IMG_DECLARE(ImgStopIcon);

    static const char* str_map[] = {"LOOP", "STOP"};
    static const lv_img_dsc_t* img_map[] = {&ImgLoopIcon, &ImgStopIcon};

    str_map[0] = Lang_GetStringByID(STRING_ID_STOP);
    str_map[1] = Lang_GetStringByID(STRING_ID_LOOP) ;

    img_map[0] = &ImgStopIcon;
    img_map[1] = &ImgLoopIcon;

    state_btn_dsc.cur_index = 0;
    state_btn_dsc.min = 0;
    state_btn_dsc.max = 1;
    state_btn_dsc.img_map = img_map;
    state_btn_dsc.map = str_map;    
    if ((*val) > state_btn_dsc.max)
    {
        *val = state_btn_dsc.max;
    }

    state_btn_dsc.cur_index = *val;
    state_btn_dsc.update_cb = update_cb;
    state_btn_dsc.widget_refresh_cb = state_btn_refresh;
    state_btn_dsc.img_to_top = -5;
    state_btn_dsc.img_gap = 3;
    state_btn_dsc.title_to_top = 5;

    lv_obj_t* state_btn = state_btn_create(parent, &state_btn_dsc, w, h);   

    return state_btn;
}

void effects_refresh_scale_img(lv_obj_t* panel, int16_t value1, int16_t min1, int16_t max1)
{
    arrow_cct_pos_type  arrow_cct_pos = {
    .star_x = 11,
    .star_y = 55,
    .middle_x = 44,
    .middle_y = 25,
    .end_x = 77,
    .end_y = 55,
    };
    
//    static uint8_t crc_value = 2;
    uint8_t cct_count = 0;
    uint16_t cct_angle = 0;
    int16_t cct_x = 0;
    int16_t cct_y = 0;
    
    cct_count = (value1 - min1)/100;
    lv_obj_t* img_panel = lv_obj_get_child(panel, 1);
//	if(value1==6050)
//        cct_count = 41;
//    if(cct_count == 0)
//    {
//        lv_img_set_angle(img_panel, 900);
//        lv_obj_set_pos(img_panel, arrow_cct_pos.star_x, arrow_cct_pos.star_y);
//    }
//    else if(cct_count > 0 && cct_count < 19)
//    {
//        cct_angle = 900+(cct_count*22.5);
//        lv_img_set_angle(img_panel, cct_angle);
//        cct_x = 33.0 * cos((((float)cct_count * 2.25) * 3.14) / 180.0);
//        cct_x = arrow_cct_pos.star_x + (33.0 - abs(cct_x));
//        cct_y = 33.0 * sin((((float)cct_count * 2.25) * 3.14) / 180.0);   //得出对边
////        if(cct_count < 23)
////            cct_y = arrow_cct_pos.star_y - abs(cct_y);   //算出y坐标   +crc_value是为了校准
////        else if(cct_count >= 23)
////            cct_y = arrow_cct_pos.star_y - abs(cct_y)+crc_value;   //算出y坐标   
//        lv_obj_set_pos(img_panel, cct_x, cct_y);
//    }
//    else if(cct_count == 19)
//    {
//        lv_img_set_angle(img_panel, 1800);
//        lv_obj_set_pos(img_panel, arrow_cct_pos.middle_x, arrow_cct_pos.middle_y);
//    }
//    else if(cct_count > 19 && cct_count < 38)
//    {
//            cct_angle = 1800+((cct_count - 19)*47);
//            lv_img_set_angle(img_panel, cct_angle);
//            cct_x = 33.0 * sin((((cct_count - 19) * 4.7) * 3.14) / 180);
////            if(cct_count < 65)
////                cct_x = arrow_cct_pos.middle_x + abs(cct_x);   //算出x坐标   +crc_value是为了校准
////            else if(cct_count >= 65)
////                cct_x = arrow_cct_pos.middle_x + abs(cct_x)+crc_value;   //算出y坐标   +1是为了校准
//            cct_y = 33.0 * cos((((cct_count - 19) * 4.7) * 3.14) / 180);   //得出邻边
//            cct_y = arrow_cct_pos.middle_y + (33.0 - abs(cct_y));   //算出y坐标
//            lv_obj_set_pos(img_panel, cct_x, cct_y);
//    }
//    else if(cct_count >= 38)
//    {
//        lv_img_set_angle(img_panel, 2700);
//        lv_obj_set_pos(img_panel, arrow_cct_pos.end_x, arrow_cct_pos.end_y);
//    }
//    if(value1==6050)
//        cct_count = 41;
    if(cct_count == 0)
    {
        lv_img_set_angle(img_panel, 900);
        lv_obj_set_pos(img_panel, arrow_cct_pos.star_x, arrow_cct_pos.star_y);
    }
    else if(cct_count > 0 && cct_count < 19)
    {
        cct_angle = 900+(cct_count*47.36f);
        lv_img_set_angle(img_panel, cct_angle);
        cct_x = 31.0f * cos((((float)cct_count * 4.73f) * 3.14f) / 180.0f);
        cct_x = arrow_cct_pos.star_x + (31.0f - abs(cct_x));
        cct_y = 31.0f * sin((((float)cct_count * 4.73f) * 3.14f) / 180.0f);   //得出对边
        cct_y = arrow_cct_pos.star_y - abs(cct_y);   //算出y坐标   +crc_value是为了校准  
        lv_obj_set_pos(img_panel, cct_x, cct_y);
    }
    else if(cct_count == 19)
    {
        lv_img_set_angle(img_panel, 1800);
        lv_obj_set_pos(img_panel, arrow_cct_pos.middle_x, arrow_cct_pos.middle_y);
    }
    else if(cct_count > 19 && cct_count < 38)
    {
            cct_angle = 1800+((cct_count - 19)*47.36f);
            lv_img_set_angle(img_panel, cct_angle);
            cct_x = 31.0f * sin((((cct_count - 19) * 4.73f) * 3.14f) / 180.0f);
            cct_x = arrow_cct_pos.middle_x + abs(cct_x);   //算出x坐标   +crc_value是为了校准
            cct_y = 31.0f * cos((((cct_count - 19) * 4.73f) * 3.14f) / 180.0f);   //得出邻边
            cct_y = arrow_cct_pos.middle_y + (31.0f - abs(cct_y));   //算出y坐标
            lv_obj_set_pos(img_panel, cct_x, cct_y);
    }
    else if(cct_count >= 38)
    {
        lv_img_set_angle(img_panel, 2700);
        lv_obj_set_pos(img_panel, arrow_cct_pos.end_x, arrow_cct_pos.end_y);
    }
}

lv_obj_t* adj_trigger_widget_creat(lv_obj_t * parent, lv_coord_t w, lv_coord_t h, trigger_btn_t* trigger_btn_d)
{

    lv_obj_t* trigger_btn = trigger_btn_create(parent, trigger_btn_d, w, h);
    
    return trigger_btn;
}


