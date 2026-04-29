/*********************
 *      INCLUDES
 *********************/
#include "menu_item_obj.h"
#include "ui_config.h"
/*********************
 *      DEFINES
 *********************/
#define MY_CLASS &menu_item_obj_obj_class

/**********************
 *      TYPEDEFS
 **********************/
#define IMG_COLOR_FOCUSED       lv_color_make(255, 255, 255)
#define IMG_COLOR_DEFAULT       lv_color_make(230, 230, 230)
/**********************
 *  STATIC PROTOTYPES
 **********************/
static void menu_item_obj_constructor(const lv_obj_class_t * class_p, lv_obj_t* obj);
static void menu_item_obj_event(const lv_obj_class_t * class_p, lv_event_t * e);
static void draw_main(lv_event_t* e);
static void draw_image(lv_event_t* e);
/**********************
 *   GLOBAL VARIABLES
 **********************/
const lv_obj_class_t menu_item_obj_obj_class = {
    .constructor_cb = menu_item_obj_constructor,
    .event_cb = menu_item_obj_event,
    .editable = LV_OBJ_CLASS_EDITABLE_FALSE,
    .group_def = LV_OBJ_CLASS_GROUP_DEF_FALSE,
    .instance_size = sizeof(menu_item_obj_t),
    .base_class = &lv_obj_class    
};

/*********************
 *  STATIC VATIABLES
 *********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_obj_t* menu_item_obj_create(lv_obj_t* parent)
{
    LV_LOG_INFO("begin")
    lv_obj_t * obj = lv_obj_class_create_obj(MY_CLASS, parent);
    lv_obj_class_init_obj(obj);
    return obj;
}

void menu_item_obj_set_param(lv_obj_t* obj, const lv_img_dsc_t* img, lv_coord_t min_width, lv_coord_t max_width)
{
    menu_item_obj_t* p_item = (menu_item_obj_t*)obj;  
    if (img != NULL)
        p_item->img = img;
    p_item->max_width = max_width;
    p_item->min_width = min_width;
}

void menu_item_obj_set_ratio(lv_obj_t* obj, uint16_t min_ratio, uint16_t max_ratio)
{
    menu_item_obj_t* p_item = (menu_item_obj_t*)obj;  
    p_item->min_ratio = min_ratio;
    p_item->max_ratio = max_ratio;
}

/*********************
 *  STATIC PARAMETERS
 *********************/
static void menu_item_obj_constructor(const lv_obj_class_t * class_p, lv_obj_t* obj)
{
    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);

    menu_item_obj_t* p_item = (menu_item_obj_t*)obj;  
    p_item->img = NULL;
    p_item->max_width = 104;
    p_item->min_width = 64;
    p_item->min_ratio = 160;
    p_item->max_ratio = 256;
    lv_obj_remove_style_all(obj);
    lv_obj_invalidate(obj);
}

static void menu_item_obj_event(const lv_obj_class_t * class_p, lv_event_t * e)
{
    LV_UNUSED(class_p);

    lv_res_t res;

    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * obj = lv_event_get_target(e);

    /*Call the ancestor's event handler*/
    res = lv_obj_event_base(MY_CLASS, e);
    if(res != LV_RES_OK) return;

    if (code == LV_EVENT_DRAW_MAIN)
    {
        draw_main(e);
    }
	else if (code == LV_EVENT_DRAW_POST)
	{
		draw_image(e);
	}
}

static void draw_image(lv_event_t* e)
{
	const lv_area_t * clip_area = lv_event_get_param(e);
	lv_obj_t * obj = lv_event_get_target(e);
	menu_item_obj_t* p_item = (menu_item_obj_t*)obj;  
	
    lv_draw_img_dsc_t img_dsc;
    lv_coord_t width, height;
    lv_area_t draw_area;    

    lv_draw_img_dsc_init(&img_dsc);

    lv_area_copy(&draw_area, &obj->coords);
    img_dsc.recolor = lv_obj_has_state(obj, LV_STATE_FOCUSED) ? IMG_COLOR_FOCUSED : IMG_COLOR_DEFAULT;
    img_dsc.recolor_opa = LV_OPA_100;   
    img_dsc.zoom = lv_map(lv_area_get_width(clip_area), p_item->min_width , p_item->max_width, p_item->min_ratio, p_item->max_ratio);

    width = p_item->img->header.w * img_dsc.zoom / 256;
    height = p_item->img->header.h * img_dsc.zoom / 256;
    
    draw_area.x1 = draw_area.x1 + (lv_area_get_width(clip_area) - width) / 2;                        
    draw_area.y1 = draw_area.y1 + (lv_area_get_height(clip_area) - height) / 2;                        
    draw_area.x2 = draw_area.x1 + p_item->img->header.w - 1;
    draw_area.y2 = draw_area.y1 + p_item->img->header.h - 1;

    lv_draw_img(&draw_area, clip_area, p_item->img,&img_dsc);  	
}

static void draw_main(lv_event_t* e)
{
    const lv_area_t* clip_area = lv_event_get_param(e);
    lv_obj_t * obj = lv_event_get_target(e);
    lv_draw_rect_dsc_t rect_dsc;

    lv_draw_rect_dsc_init(&rect_dsc);
    lv_obj_init_draw_rect_dsc(obj, LV_PART_MAIN, &rect_dsc);
    rect_dsc.bg_opa = LV_OPA_100;
    rect_dsc.bg_color = lv_obj_has_state(obj, LV_STATE_FOCUSED) ? RED_THEME_COLOR : lv_color_make(66, 66, 66);
    rect_dsc.bg_grad_color_stop = LV_OPA_0;
    rect_dsc.shadow_ofs_y = 0;
    rect_dsc.shadow_ofs_x = 0;
    rect_dsc.shadow_width = 0;

    lv_coord_t w = lv_obj_get_style_transform_width(obj, LV_PART_MAIN);
    lv_coord_t h = lv_obj_get_style_transform_height(obj, LV_PART_MAIN);
    lv_area_t coords;
    lv_area_copy(&coords, &obj->coords);
    coords.x1 -= w;
    coords.x2 += w;
    coords.y1 -= h;
    coords.y2 += h;

    lv_draw_rect(&coords, clip_area, &rect_dsc);
}
