/*********************
 *      INCLUDES
 *********************/
#include "ui_common.h"
#include "page_widget.h"

/*********************
 *      DEFINES
 *********************/
#define COMMON_BTN_WIDTH    118
#define COMMON_BTN_HEIGHT   60
#define PANEL_BG_COLOR          lv_color_make(40,40,40)
#define COMMON_BTN_BG_COLOR                         lv_color_make(40,40,40)
#define COMMON_BTN_BG_FOCUSD_COLOR                  RED_THEME_COLOR
#define COMMON_BTN_TEXT_COLOR                       lv_color_make(255,255,255)
#define COMMON_BTN_FOCUSED_TEXT_COLOR               lv_color_make(255,255,255)

#define MENU_ITEM_WIDTH                 55
#define MENU_ITEM_HEIGHT                55
#define MENU_ITEM_COLOR                 lv_color_make(105, 198, 255)
#define MENU_ITEM_FOCUSED_COLOR         lv_color_make(0, 143, 215)
#define MENU_ITEM_TEXT_COLOR            lv_color_make(0, 0, 0)
#define MENU_ITEM_FOCUSED_TEXT_COLOR    lv_color_make(255, 255, 255)
#define LEVEL2_WIN_BG_COLOR             lv_color_make(0, 0, 0)
/**********************
 *  STATIC PROTOTYPES
 **********************/
static void anim_obj_size_cb(void* var, int32_t v);
static void anim_pos_x_cb(void *var, int32_t v);
static void anim_pos_y_cb(void* var, int32_t v);
static void anim_img_size_cb(void* var, int32_t v);

//static void gel_style_init(gel_style_t* p_style);

/**********************
 *   GLOBAL VARIABLES
 **********************/
char sdb[100];

lv_style_t style_select_panel;
lv_style_t style_rect_panel;
lv_style_t style_common_btn;
lv_style_t style_common_focued_btn;
lv_style_t style_menu_item;
lv_style_t style_menu_focused_item;
lv_style_t style_sys_menu_btn;
lv_style_t style_le_btn;
lv_style_t style_levelx_btn;
lv_style_t style_levelx_win;
/****************GEL的样式********************/
/**********************
 *  STATIC PROTOTYPES
 **********************/
static gel_style_t gel_style;


/**********************
 *   GLOBAL FUNCTIONS
 **********************/
void item_list_add_tail(item_t** list_head, item_t* item)
{ 
    if (*list_head == NULL)
    {
        *list_head = item;
        return ;
    }
 
    item_t* next_item = *list_head;

    while(next_item->next != (*list_head))
    {
        next_item = next_item->next;
    }

    next_item->next = item;
    item->prev = next_item;
    item->next = *list_head;                
    (*list_head)->prev = item;        
}

void item_add(item_t** list_head, item_t *item, item_id_t id,  lv_obj_t* obj, const void * img_dsc, \
                            const char* name)
{
    item->obj = obj;
    item->img_dsc = img_dsc;
    item->id = id;
    item->name = name;
    item->next = item;
    item->prev = item;

    item_list_add_tail(list_head, item);
}

item_t* item_list_get_prev(item_t *item, uint8_t i)
{
    item_t* tmp = item;
    
    do {
        if (i == 0)
        {
            break;
        }

        tmp = tmp->prev;

    }while(i--);

    return tmp;
}

item_t* item_list_get_next(item_t *item, uint8_t i)
{
    item_t* tmp = item;
            
    do {
        if (i == 0)
        {
            break;
        }

        tmp = tmp->next;

    }while(i--);

    return tmp;
}

item_t* item_list_get(item_t* item, int i)
{
    item_t* tmp = item;
        
    if (i < 0)
    {        
        tmp = item_list_get_prev(item, LV_ABS(i));
    }
    else 
    {
        tmp = item_list_get_next(item, i);
    }

    return tmp;
}

void my_str_replace(char* ptr, char target, char replace)
{
    while (*ptr != '\0')
    {
        if (*ptr == target)
        {
            *ptr = replace;
        }
        ptr++;
    }
}

void my_indev_set_group(lv_group_t* group)
{
    lv_indev_t* indev = NULL;
    do {
        indev = lv_indev_get_next(indev);
        if (indev == NULL)
        {
            break;
        }
        if (lv_indev_get_type(indev) == LV_INDEV_TYPE_ENCODER || LV_INDEV_TYPE_KEYPAD == lv_indev_get_type(indev))
        {            
            lv_indev_set_group(indev, group);
        }

    }while (1);

}

void menu_item_init(lv_style_t* style, bool focused)
{
    lv_style_init(style);
    lv_style_set_radius(style, 5);
    lv_style_set_outline_width(style, 0);
    lv_style_set_border_width(style, 0);
    lv_style_set_pad_all(style, 20);
    lv_style_set_bg_color(style, !focused ? MENU_ITEM_COLOR : MENU_ITEM_FOCUSED_COLOR);
    lv_style_set_text_color(style, !focused ? MENU_ITEM_TEXT_COLOR : MENU_ITEM_FOCUSED_TEXT_COLOR);
    lv_style_set_height(style, MENU_ITEM_WIDTH);
    lv_style_set_width(style, MENU_ITEM_HEIGHT);
}

void levelx_win_style_init(void)
{
    lv_style_init(&style_levelx_win);
    lv_style_set_pad_all(&style_levelx_win, 5);    
    lv_style_set_border_width(&style_levelx_win, 0);
    lv_style_set_radius(&style_levelx_win, 0);
    lv_style_set_bg_color(&style_levelx_win, LEVEL2_WIN_BG_COLOR);
    lv_style_set_text_font(&style_levelx_win, Font_ResouceGet(FONT_14));

    lv_style_init(&style_levelx_btn);
    lv_style_set_pad_all(&style_levelx_btn, 0);
    lv_style_set_border_width(&style_levelx_btn, 0);
    lv_style_set_radius(&style_levelx_btn, 0);
    lv_style_set_bg_color(&style_levelx_btn, LEVEL2_WIN_BG_COLOR);
    lv_style_set_shadow_ofs_y(&style_levelx_btn, 0);
}

void common_btn_style_init(lv_style_t* style, bool focused)
{
    lv_style_init(style);
    lv_style_set_radius(style, 5);
    lv_style_set_outline_width(style, 0);
    lv_style_set_border_width(style, 0);
    lv_style_set_pad_all(style, 0);
    lv_style_set_bg_color(style, !focused ? COMMON_BTN_BG_COLOR : COMMON_BTN_BG_FOCUSD_COLOR);
    lv_style_set_text_color(style, !focused ? COMMON_BTN_TEXT_COLOR : COMMON_BTN_FOCUSED_TEXT_COLOR);    
}

void select_btn_style_init(lv_style_t* style, bool focused)
{
    lv_style_init(style);
    lv_style_set_radius(style, 5);
	lv_style_set_border_color(style, RED_THEME_COLOR);
	lv_style_set_border_width(style, 2);
	lv_style_set_border_side(style, LV_BORDER_SIDE_FULL); //设置样式边框显示范围
    lv_style_set_outline_width(style, 0);
    lv_style_set_pad_all(style, 0);
    lv_style_set_bg_color(style, COMMON_BTN_BG_COLOR);
}

void style_sys_menu_btn_init(lv_style_t* style)
{
#define ITEM_BG_COLOR           lv_color_make(40, 40, 40)    
    lv_style_init(style);
    lv_style_set_radius(style, LV_RADIUS_CIRCLE);
    lv_style_set_pad_all(style, 0);
    lv_style_set_outline_width(style, 0);
    lv_style_set_border_width(style, 0);
    lv_style_set_bg_color(style, ITEM_BG_COLOR);
}

void style_light_effect_btn_init(lv_style_t* style)
{
#define LIGHT_EFFECT_BTN_BG_COLOR   lv_color_make(128, 128, 128)    
    lv_style_init(style);
    lv_style_set_radius(style, LV_RADIUS_CIRCLE);
    lv_style_set_pad_all(style, 0);
    lv_style_set_outline_width(style, 0);
    lv_style_set_border_width(style, 0);
    lv_style_set_bg_color(style, LIGHT_EFFECT_BTN_BG_COLOR); 
}

gel_style_t* gel_style_get(void)
{
    return &gel_style;
}

void gel_style_init(gel_style_t* p_style)
{
    lv_style_init(&p_style->btn_style);
    lv_style_set_radius(&p_style->btn_style, 0);
    lv_style_set_bg_color(&p_style->btn_style, lv_color_make(38, 38,38));
    lv_style_set_text_color(&p_style->btn_style, lv_color_white());    
    lv_style_set_pad_all(&p_style->btn_style, 0);    

    lv_style_init(&p_style->btn_focsed_style);
    lv_style_set_bg_color(&p_style->btn_focsed_style, RED_THEME_COLOR);
    lv_style_set_text_color(&p_style->btn_focsed_style, lv_color_white());   

    lv_style_init(&p_style->btn_checked_style);
    lv_style_set_bg_color(&p_style->btn_checked_style, lv_color_make(198, 185, 187));
    lv_style_set_text_color(&p_style->btn_checked_style, lv_color_make( 51, 43, 43));   

    lv_style_init(&p_style->serials_btn_style);
    lv_style_set_pad_all(&p_style->serials_btn_style, 0);
    lv_style_set_bg_color(&p_style->serials_btn_style, PANEL_BG_COLOR);
    lv_style_set_text_color(&p_style->serials_btn_style, lv_color_make(255, 255, 255));

    lv_style_init(&p_style->color_cont_style);
    lv_style_set_pad_all(&p_style->color_cont_style, 0);
    lv_style_set_radius(&p_style->color_cont_style, 0);
    lv_style_set_text_color(&p_style->color_cont_style, lv_color_make(93, 93, 93));
    lv_style_set_border_width(&p_style->color_cont_style, 0);
    lv_style_set_radius(&p_style->color_cont_style, 3);

    lv_style_init(&p_style->color_cont_focus_style);    
    lv_style_set_pad_all(&p_style->color_cont_focus_style, 0);
    lv_style_set_radius(&p_style->color_cont_focus_style, 3);
    lv_style_set_border_width(&p_style->color_cont_focus_style, 2);
    lv_style_set_border_color(&p_style->color_cont_focus_style, lv_color_white());
}

void common_style_init(void)
{
    lv_style_init(&style_rect_panel);
    lv_style_set_radius(&style_rect_panel, LIGHT_MODE_COMMON_RADIUS);
    lv_style_set_outline_width(&style_rect_panel, 0);
    lv_style_set_outline_pad(&style_rect_panel, 0);
    lv_style_set_pad_all(&style_rect_panel, 0);
    lv_style_set_border_width(&style_rect_panel, 0);
    lv_style_set_bg_color(&style_rect_panel, PANEL_BG_COLOR);

    common_btn_style_init(&style_common_btn, false);
    common_btn_style_init(&style_common_focued_btn, true);
    menu_item_init(&style_menu_focused_item, true);
    menu_item_init(&style_menu_item, false);
	select_btn_style_init(&style_select_panel, false);
    style_sys_menu_btn_init(&style_sys_menu_btn);
    style_light_effect_btn_init(&style_le_btn);

    gel_style_init(&gel_style);
    lighteffect_style_init(lighteffect_style_get());
    levelx_win_style_init();
}

void anim_obj_move_x(lv_obj_t* obj, lv_coord_t start_x, lv_coord_t end_x, uint32_t anim_time)
{
    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, obj);
    lv_anim_set_values(&a, start_x, end_x);
    lv_anim_set_exec_cb(&a, anim_pos_x_cb);
    lv_anim_set_repeat_count(&a, 0);    
    lv_anim_set_time(&a, anim_time);
    lv_anim_set_playback_time(&a, 0);
    lv_anim_start(&a);
}

void anim_obj_move_y(lv_obj_t* obj, lv_coord_t start_y, lv_coord_t end_y, uint32_t anim_time)
{
    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, obj);
    lv_anim_set_values(&a, start_y, end_y);
    lv_anim_set_exec_cb(&a, anim_pos_y_cb);
    lv_anim_set_repeat_count(&a, 0);    
    lv_anim_set_time(&a, anim_time);
    lv_anim_set_playback_time(&a, 0);
    lv_anim_start(&a);
}

void anim_obj_move(lv_obj_t *obj, lv_coord_t start_x, lv_coord_t start_y, \
                                            lv_coord_t end_x, lv_coord_t end_y, uint32_t time)
{
     lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, obj);
    lv_anim_set_values(&a, start_x, end_x);
    lv_anim_set_exec_cb(&a, anim_pos_x_cb);
    //lv_anim_path_bounce(&a, lv_anim_path_bounce);
    lv_anim_set_repeat_count(&a, 0);    
    lv_anim_set_time(&a, time);
    lv_anim_set_playback_time(&a, 0);
    lv_anim_start(&a);

    lv_anim_set_values(&a, start_y, end_y);
    //lv_anim_set_path_cb(&a, lv_anim_path_bounce);
    lv_anim_set_exec_cb(&a, anim_pos_y_cb);
    lv_anim_start(&a); 
}

void anim_obj_size_zoom(lv_obj_t* obj, lv_coord_t start_width, lv_coord_t end_width, uint32_t time)
{
    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, obj);
    lv_anim_set_values(&a, start_width, end_width);
    lv_anim_set_exec_cb(&a, anim_obj_size_cb);
    lv_anim_set_repeat_count(&a, 0);
    lv_anim_set_time(&a, time);
    lv_anim_set_playback_time(&a, 0);
    lv_anim_start(&a);
}

void anim_obj_img_zoom(lv_obj_t* obj, uint16_t coff_start, uint16_t coff_end, uint32_t time, lv_anim_ready_cb_t ready_cb)
{
    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, obj);
    lv_anim_set_values(&a, coff_start, coff_end);
    lv_anim_set_exec_cb(&a, anim_img_size_cb);

    if (ready_cb != NULL)
    {
        lv_anim_set_ready_cb(&a, ready_cb);
    }
   
    lv_anim_set_repeat_count(&a, 0);
    lv_anim_set_time(&a, time);
    lv_anim_set_playback_time(&a, 0);
    lv_anim_start(&a);
}


/**
 * @brief 格式化亮度
 * 
 * @param sdb 
 * @param size 
 * @param intensity 
 */
void format_intensity(char* sdb, uint16_t size, uint16_t intensity)
{
    if (intensity >= 1000)
    {
        lv_snprintf(sdb, size, "%d%%", 100);
    }
    else
    {
        lv_snprintf(sdb, size, "%d.%d%%", intensity / 10, intensity % 10);
    }
}

/**
 * @brief 格式化GM数值
 * 
 * @param sdb 
 * @param size 
 * @param value 
 */
void format_gm(char* sdb, uint16_t size, int16_t gm)
{   
    const char* sign = gm >= 0 ? "" : "-";
    lv_snprintf((char*)sdb, size, "%s%d.%02d", sign, LV_ABS(gm) / 100,  LV_ABS(gm) % 100);
}

void format_adv_hsi_hue(char* sdb, uint16_t size, uint16_t hue)
{
    if (hue >= 3600)
    {
        lv_snprintf(sdb, size, "%d°", 3600);
    }
    else
    {
        lv_snprintf(sdb, size, "%d.%d°",hue  / 10,hue  % 10);
    }

}

void format_adv_hsi_sat(char* sdb, uint16_t size, uint16_t sat)
{
    if (sat >= 1000)
    {
        lv_snprintf(sdb, size, "%d%%", 100);
    }
    else
    {
        lv_snprintf(sdb, size, "%d.%d%%", sat / 10, sat % 10);
    }
}

/**
 * @brief 字符串处理
 * 
 */
const char* power_value_get_value_str(void* data)
{
    adj_btn_t* dsc = (adj_btn_t*)data;
    lv_snprintf((char*)sdb, sizeof(sdb), "%dW", dsc->value);
    return (const char*)sdb;     
}

const char* db_value_get_value_str(void* data)
{
    adj_btn_t* dsc = (adj_btn_t*)data;
    lv_snprintf((char*)sdb, sizeof(sdb), "%ddB", dsc->value);
    return (const char*)sdb;     
}

const char* power_limit_get_str(void* data)
{
    adj_btn_t* dsc = (adj_btn_t*)data;
//	if(dsc->value == 0)
		return Lang_GetStringByID(STRING_ID_OUTPUT_SET_OFF);  
//	else
//		return Lang_GetStringByID(STRING_ID_OUTPUT_SET_ON);  
}

const char* intensity_get_value_str(void* data)
{
    adj_btn_t* dsc = (adj_btn_t*)data;
	if(ui_regognition_error_get())
	{
		dsc->value = 0;
	}
    format_intensity((char*)sdb, sizeof(sdb), dsc->value);
    return (const char*)sdb;     
}

const char* hue_get_value_str(void* data)
{
	uint8_t regognition_status = 0;
    adj_btn_t* dsc = (adj_btn_t*)data;
//	data_center_read_config_data(SYS_CONFIG_ACC_RECOGNITION, &regognition_status);
    lv_snprintf((char*)sdb, sizeof(sdb), "%d°", dsc->value);
    return (const char*)sdb;  
}

const char* adv_hsi_hue_get_value_str(void* data)
{
    adj_btn_t* dsc = (adj_btn_t*)data;
    if (dsc->value >= 3600)
    {
        lv_snprintf(sdb, sizeof(sdb), "%d°", 360);
    }
    else
    {
        lv_snprintf(sdb, sizeof(sdb), "%d.%d°", dsc->value  / 10, dsc->value  % 10);
    }
    return (const char*)sdb;      
}

const char* sat_get_value_str(void* data)
{
    adj_btn_t* dsc = (adj_btn_t*)data;
    lv_snprintf((char*)sdb, sizeof(sdb), "%d%%", dsc->value);
    return (const char*)sdb;  
}

const char* adv_hsi_sat_get_value_str(void* data)
{
    adj_btn_t* dsc = (adj_btn_t*)data;
    if (dsc->value >= 1000)
    {
        lv_snprintf(sdb, sizeof(sdb), "%d%%", 100);
    }
    else
    {
        lv_snprintf(sdb, sizeof(sdb), "%d.%d%%", dsc->value  / 10, dsc->value  % 10);
    }
    return (const char*)sdb;  
}

const char* cct_get_value_str(void* data)
{
    adj_btn_t* dsc = (adj_btn_t*)data;
    lv_snprintf((char*)sdb, sizeof(sdb), "%dK", dsc->value);
    return (const char*)sdb;  
}

const char* gm_get_value_str(void* data)
{    
    adj_btn_t* dsc = (adj_btn_t*)data;
    const char* sign = dsc->value >= 0 ? "" : "-";
    lv_snprintf((char*)sdb, sizeof(sdb), "%s%d.%02d", sign, LV_ABS(dsc->value) / 100, LV_ABS(dsc->value) % 100);
    return (const char*)sdb;     
}

const char* xy_get_value_str(void* data)
{
    adj_btn_t* dsc = (adj_btn_t*)data;
    lv_snprintf((char*)sdb, sizeof(sdb), "0.%04d", dsc->value);
    return (const char*)sdb;  
}

const char* rgb_get_value_str(void* data)
{
    adj_btn_t* dsc = (adj_btn_t*)data;
    format_intensity((char*)sdb, sizeof(sdb), dsc->value);
    return (const char*)sdb;         
}

const char* light_mode_get_title(uint8_t light_m)
{
    const char* title = NULL;
    switch(light_m)
    {
        case PAGE_LIGHT_MODE_CCT: title = "CCT"; break;
        case PAGE_LIGHT_MODE_GEL: title = "GEL"; break;
        case PAGE_LIGHT_MODE_HSI: title = "HSI"; break;
        case PAGE_LIGHT_MODE_XY: title = "X Y"; break;
        case PAGE_LIGHT_MODE_RGB: title = "RGB"; break;
        case PAGE_LIGHT_MODE_SOURCE: title = "Source"; break;
    }

    return title;
}

const char* frequence_get_value_str(void* data)
{
    adj_btn_t* dsc = (adj_btn_t*)data;

    if (dsc->value >= FREQUENCY_LEVEL_R)
    {
        lv_snprintf((char*)sdb, sizeof(sdb), "R");
    }
    else
    {
        lv_snprintf((char*)sdb, sizeof(sdb), "%d", dsc->value);
    }
    
    return (const char*)sdb;     
}

const char* speed_get_value_str(void* data)
{
    adj_btn_t* dsc = (adj_btn_t*)data;

    if (dsc->value >= FREQUENCY_LEVEL_R)
    {
        lv_snprintf((char*)sdb, sizeof(sdb), "R");
    }
    else
    {
        lv_snprintf((char*)sdb, sizeof(sdb), "%d", dsc->value);
    }
    
    return (const char*)sdb;     
}

const char* decay_get_value_str(void* data)
{
    adj_btn_t* dsc = (adj_btn_t*)data;

    if (dsc->value >= FREQUENCY_LEVEL_R)
    {
        lv_snprintf((char*)sdb, sizeof(sdb), "R");
    }
    else
    {
        lv_snprintf((char*)sdb, sizeof(sdb), "%d", dsc->value);
    }
    
    return (const char*)sdb;  
}

/**********************
 *  STATIC PROTOTYPES
 **********************/

static void anim_pos_x_cb(void *var, int32_t v)
{
    lv_obj_set_x(var, v);
}

static void anim_pos_y_cb(void* var, int32_t v)
{
    lv_obj_set_y(var, v);
}

static void anim_obj_size_cb(void* var, int32_t v)
{
    lv_obj_set_size(var, v, v);
}

static void anim_img_size_cb(void* var, int32_t v)
{
    lv_img_set_zoom(var, v);
}


