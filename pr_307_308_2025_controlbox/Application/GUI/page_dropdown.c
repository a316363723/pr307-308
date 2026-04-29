/*********************
 *      INCLUDES
 *********************/
#include "ui_common.h"

/*********************
 *      DEFINES
 *********************/
#define CURTAIN_BG_OPA          LV_OPA_50
#define MENU_PANEL_BG_COLOR     lv_color_make(51, 51, 51)
#define MENU_PANEL_WIDTH        320 - 1 
#define MENU_PANEL_HEIGHT       151

#define CURTAIN_BG_COLOR        lv_color_make(51, 51, 51)

#define CURTAIN_HEIGHT          (183 + 5)
#define CURTAIN_TEXT_COLOR      lv_color_make(93, 93, 93)

#define CURTAIN_ANIM_TIME       150
/**********************
 *      TYPEDEFS
 **********************/
enum {
    DP_ITEM_ID_CCT = 0,
#if PROJECT_TYPE==307
    DP_ITEM_ID_HSI,    
    DP_ITEM_ID_RGB,
    DP_ITEM_ID_GEL,    
    DP_ITEM_ID_XY,
    DP_ITEM_ID_SRC, 
#endif    
    DP_ITEM_ID_FX,     
#if UI_TEST
    DP_ITEM_ID_TEST,
#endif      
    DP_ITEM_ID_MAX,
};

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void anim_y_cb(void * var, int32_t v);
static void menu_btn_event(lv_event_t* event);
static void get_type_str(uint8_t id, str_dsc_t* pdsc);
static void curtain_del_event(lv_event_t* event);
static void turn_to_new_page(uint8_t id);
static uint8_t get_focused_item_id(void);
/**********************
 *  STATIC VARIABLES
 **********************/
static lv_group_t* menu_group;

/**********************
 *   GLOBAL FUNCTIONS
 **********************/
lv_obj_t* drop_down_create(lv_obj_t* obj)
{
    menu_group = lv_group_create();
    my_indev_set_group(menu_group);

    lv_obj_t* screen = lv_obj_create(obj);
    lv_obj_remove_style_all(screen);
    //lv_obj_set_size(screen, lv_obj_get_width(lv_scr_act()), lv_obj_get_height(lv_scr_act()));
    lv_obj_set_size(screen, 320, 240);
    lv_obj_set_style_bg_opa(screen, CURTAIN_BG_OPA, 0);
    lv_obj_add_event_cb(screen, curtain_del_event, LV_EVENT_DELETE, NULL);

    lv_obj_t* curtain = lv_obj_create(screen);
    lv_obj_clear_flag(curtain, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_bg_color(curtain, CURTAIN_BG_COLOR, 0);
    lv_obj_set_style_text_color(curtain, CURTAIN_TEXT_COLOR, 0);
    lv_obj_set_style_border_width(curtain, 0, 0);
    lv_obj_set_style_pad_all(curtain, 0, 0);
    lv_obj_set_size(curtain, lv_pct(100), CURTAIN_HEIGHT);
    lv_obj_set_pos(curtain, 0, -10);

    static lv_coord_t col_dsc[] = { 55, 55, 55, 55, LV_GRID_TEMPLATE_LAST};
    static lv_coord_t row_dsc[] = { 55, 55, LV_GRID_TEMPLATE_LAST };

    lv_obj_t* panel = lv_obj_create(curtain);
    lv_obj_remove_style_all(panel);
    lv_obj_set_size(panel, lv_pct(100), lv_pct(97));
    lv_obj_set_grid_dsc_array(panel, col_dsc, row_dsc);  
    
    lv_obj_set_style_pad_all(panel, 0, 0);   
    lv_obj_set_style_pad_top(panel, 10, 0);  
    lv_obj_set_style_pad_row(panel, 17, 0);
    lv_obj_set_style_pad_column(panel, 23, 0);    
    lv_obj_set_style_bg_color(panel, MENU_PANEL_BG_COLOR, 0);

    lv_obj_t* label1;
    lv_obj_t* obj1;
    str_dsc_t str_dsc;
    uint8_t focused_index = get_focused_item_id();
    for (uint32_t i = 0; i < DP_ITEM_ID_MAX; i++)
    {
        uint8_t col = i % 4;
        uint8_t row = i / 4;

        obj1 = lv_obj_create(panel);

        lv_obj_clear_flag(obj1, LV_OBJ_FLAG_SCROLLABLE);

        lv_obj_add_style(obj1, &style_menu_item, LV_STATE_DEFAULT);
        lv_obj_set_style_bg_color(obj1, lv_color_make(128, 128, 128), LV_STATE_DEFAULT);
        lv_obj_add_style(obj1, &style_menu_focused_item, LV_STATE_FOCUSED);             
        lv_obj_set_style_bg_color(obj1, RED_THEME_COLOR, LV_STATE_FOCUSED);
        lv_obj_set_grid_cell(obj1, LV_GRID_ALIGN_STRETCH, col, 1, \
                   LV_GRID_ALIGN_STRETCH, row, 1);
        
        get_type_str(i, &str_dsc);

        label1 = lv_label_create(obj1);
        lv_obj_set_style_text_font(obj1, str_dsc.font, 0);        
        lv_obj_set_style_text_color(obj1, lv_color_white(), 0);        
        lv_obj_set_style_text_align(obj1, LV_TEXT_ALIGN_CENTER, 0);
        lv_obj_set_style_text_line_space(label1, 2, 0);
        
        if (str_dsc.font == Font_ResouceGet(FONT_14))
        {
            lv_obj_set_style_text_font(obj1, Font_ResouceGet(FONT_14_BOLD), LV_STATE_FOCUSED);
        }
        else 
        {
            lv_obj_set_style_text_font(obj1, Font_ResouceGet(FONT_16_BOLD), LV_STATE_FOCUSED);
        }
    
        lv_obj_set_style_text_color(obj1, lv_color_white(), LV_STATE_FOCUSED);
        lv_label_set_text_fmt(label1, "%s", str_dsc.str);
        lv_obj_center(label1);

        lv_obj_add_event_cb(obj1, menu_btn_event, LV_EVENT_ALL, NULL);
        lv_group_add_obj(menu_group, obj1);        
    }

    lv_obj_t* label_title = lv_label_create(curtain);
    lv_label_set_text(label_title, Lang_GetStringByID(STRING_ID_LIGHT_MODE));
    lv_obj_set_style_text_font(label_title, Font_ResouceGet(FONT_18_BOLD), 0);
    lv_obj_set_style_text_color(label_title, lv_color_make(232, 233, 233), 0);
    lv_obj_align(label_title, LV_ALIGN_TOP_LEFT, 15, 15);
    lv_obj_align_to(panel, label_title, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 0);

    lv_obj_t* bottom = lv_obj_create(curtain);
    lv_obj_set_style_pad_all(bottom, 0, 0);
    lv_obj_set_style_outline_width(bottom, 0, 0);
    lv_obj_set_style_border_width(bottom, 0, 0);
    lv_obj_set_style_bg_color(bottom, lv_color_make(182, 182, 182), 0);
    lv_obj_set_size(bottom, lv_pct(15), 2);
    lv_obj_align(bottom, LV_ALIGN_BOTTOM_MID, 0, -3);

    lv_anim_t anim;
    lv_anim_init(&anim);
    lv_anim_set_var(&anim, screen);
    lv_anim_set_values(&anim, -320, 0);
    lv_anim_set_time(&anim, CURTAIN_ANIM_TIME);
    lv_anim_set_exec_cb(&anim, anim_y_cb);
    lv_anim_set_path_cb(&anim, lv_anim_path_bounce);
    lv_anim_start(&anim);
    
    lv_obj_t* focuse_item = lv_obj_get_child(panel, focused_index);
    lv_group_focus_obj(focuse_item);
    lv_group_set_wrap(menu_group, false);

    return screen;
}

void drop_down_del(void)
{
    uint32_t key = LV_KEY_BACKSPACE;
    lv_group_t* default_g = lv_group_get_default();
    obj_send_event_to_screen(LV_EVENT_KEY, (void *)&key);
    my_indev_set_group(default_g);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/
static void anim_y_cb(void * var, int32_t v)
{
    lv_obj_set_y(var, v);
}

static void menu_btn_event(lv_event_t* e)
{
    lv_obj_t* obj = lv_event_get_target(e);
    lv_event_code_t event = lv_event_get_code(e);

    if (LV_EVENT_RELEASED == event)
    {
        uint8_t index = lv_obj_get_child_id(obj);
        turn_to_new_page(index);
    }
    else if (LV_EVENT_KEY == event)
    {
        uint32_t key = *(uint32_t*)lv_event_get_param(e);
        if (LV_KEY_BACKSPACE == key)
        {
            drop_down_del();
        }
        else if (LV_KEY_DEL == key)
        {
            uint8_t index = page_get_index(PAGE_MENU);//获取上一次的字ID
            screen_load_page_and_index(PAGE_MENU, 0, index, true);  
        }
    }
}

static void curtain_del_event(lv_event_t* event)
{
    lv_group_focus_freeze(menu_group, true);
    lv_group_remove_all_objs(menu_group);
    lv_group_del(menu_group);
    menu_group = NULL;
}

static uint8_t get_focused_item_id(void)
{
    uint8_t page_id = screen_get_act_pid();
    uint8_t item_id = DP_ITEM_ID_FX;

    if (page_id < PAGE_LIGHT_MODE_HSI || page_id > PAGE_LIGHT_EFFECT_MENU)
    {
        page_id = page_get_stacktop_id();
    }

    switch(page_id)
    {
        case PAGE_LIGHT_MODE_CCT:    item_id = DP_ITEM_ID_CCT; break;
        #if PROJECT_TYPE==307
        case PAGE_LIGHT_MODE_HSI:    item_id = DP_ITEM_ID_HSI; break;
        case PAGE_LIGHT_MODE_RGB:    item_id = DP_ITEM_ID_RGB; break;
        case PAGE_LIGHT_MODE_GEL:    item_id = DP_ITEM_ID_GEL; break;
        case PAGE_LIGHT_MODE_XY:     item_id = DP_ITEM_ID_XY; break;
        case PAGE_LIGHT_MODE_SOURCE: item_id = DP_ITEM_ID_SRC; break;
        #endif
        case PAGE_LIGHT_EFFECT_MENU: item_id = DP_ITEM_ID_FX; break;
        default: 
        {
            item_id = DP_ITEM_ID_FX;
        }
        break;
    }
    return item_id;
}

static void turn_to_new_page(uint8_t id)
{   
    uint8_t page_id = 0;
    struct sys_info_dmx        dmx;
	
    switch(id)
    {
        case DP_ITEM_ID_CCT: page_id = PAGE_LIGHT_MODE_CCT;break;
        #if PROJECT_TYPE==307
        case DP_ITEM_ID_HSI: page_id = PAGE_LIGHT_MODE_HSI;break;
        case DP_ITEM_ID_RGB: page_id = PAGE_LIGHT_MODE_RGB;break;
        case DP_ITEM_ID_GEL: page_id = PAGE_LIGHT_MODE_GEL;break;
        case DP_ITEM_ID_XY:  page_id = PAGE_LIGHT_MODE_XY;break;
        case DP_ITEM_ID_SRC: page_id = PAGE_LIGHT_MODE_SOURCE;break;
        #endif
        case DP_ITEM_ID_FX:  page_id = PAGE_LIGHT_EFFECT_MENU;break;
#if UI_TEST
        case DP_ITEM_ID_TEST: page_id = PAGE_TEST; break;
#endif
    }
	data_center_read_sys_info(SYS_INFO_DMX, &dmx);
	if((dmx.eth_linked == 1 || dmx.wired_linked == 1 || dmx.wireless_linked == 1))
	{
		return;
	}
    if (page_id > 0)
    {        
        screen_load_page_with_clear(page_id, 0, false);
        if (page_id == screen_get_act_pid())
        {
            drop_down_del();
        }
        else
        {
            //screen_load_page(page_id, 0, false);
            screen_load_page_with_clear(page_id, 0, false);
            my_indev_set_group(NULL);
        }        
    }
}

static void get_type_str(uint8_t type, str_dsc_t *pdsc)
{
    pdsc->font = Font_ResouceGet(FONT_16_MEDIUM);

    switch(type)
    {    
        case DP_ITEM_ID_CCT: 
            pdsc->str = "CCT";   
            break;
        #if PROJECT_TYPE==307
        case DP_ITEM_ID_HSI: 
            pdsc->font = Font_ResouceGet(FONT_16_MEDIUM);
            pdsc->str = "Adv.\nHSI";   
            break;
        case DP_ITEM_ID_RGB: 
            pdsc->str = "RGB";   
            break;
        case DP_ITEM_ID_GEL: 
            pdsc->str = "GEL";   
            break;
        case DP_ITEM_ID_XY: 
            pdsc->str = "X Y";   
            break;
        case DP_ITEM_ID_SRC:
            pdsc->font = Font_ResouceGet(FONT_14);
            pdsc->str = "Source";   
            break;
        #endif
        case DP_ITEM_ID_FX:
            pdsc->str = "F X";
            break;
        #if UI_TEST            
        case DP_ITEM_ID_TEST:
            pdsc->str = "Test";
            break;
        #endif
    }
}
