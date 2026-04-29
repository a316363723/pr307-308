#ifndef  UI_COMMON_H
#define  UI_COMMON_H

#ifdef __cplusplus
extern "C" {
#endif


/*********************
 *      INCLUDES
 *********************/
#include "ui_config.h"
#include "ui_assets.h"
#include "ui_data.h"
#include "ui_indev.h"
#include "ui_hal.h"
#include "page_manager.h"
#include "lv_user.h"
#include "lvgl/lvgl.h"


/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/
typedef uint8_t item_id_t;

/**
 * @brief 用来描述每一项.
 * 
 */
typedef struct _item{
    lv_obj_t* obj;
    item_id_t id;
    const lv_img_dsc_t* img_dsc;        //图标
    const char* name;                   //名字
    struct _item* next;
    struct _item* prev;
}item_t;

typedef struct {
    const char* str;
    const lv_font_t *font;
    const lv_img_dsc_t *img;
}str_dsc_t;

typedef struct {
    lv_obj_t* obj;
    lv_point_t last_pos;
    bool is_init;
}xy_selector_t;

typedef struct _adj_btn {
    lv_obj_t* obj;
    int16_t  min;
    int16_t  max;
    int16_t  step;
    int16_t  value;
    const char* name;
    const char* unit;    
    lv_coord_t  title_to_left;
    lv_coord_t  div_to_left;
    lv_coord_t  val_to_right;
    bool        div_disable;    
    bool        cycle;
    const char* (*get_value_str_cb)(void* data);
    void        (*finished_event_cb)(void* data);
    void        (*cancel_op_cb)(void* data);
    void        (*update_run_cb)(void* data);
    bool        (*assign_value_cb)(void* data);
    void        (*refresh_btn_cb)(lv_obj_t* parent, struct _adj_btn* dsc);    
    int32_t     (*get_step_cb)(uint32_t value);    
}adj_btn_t;

typedef struct _gel_style_t{
    lv_style_t btn_style;
    lv_style_t btn_checked_style;
    lv_style_t btn_focsed_style;
    lv_style_t serials_btn_style;
    lv_style_t color_cont_style;
    lv_style_t color_cont_focus_style;
}gel_style_t;

typedef struct _lighteffect_style_t{
    lv_style_t roller_style;
    lv_style_t roller_focus_style; 
    lv_style_t roller_edit_style;
}lighteffect_style_t;

#define ADJ_BTN_INITIALIZER(_min, _max, _step, _def_val, _name, _unit,  _title_to_left, _div_to_left,  _value_to_right, _div_disable, _cycle, _get_value_str_cb, _upd_run_cb, _assign_cb, _refresh_cb, _get_step_cb)  \
                        { \
                                .obj = NULL,\
                                .min = _min,\
                                .max = _max,\
                                .step = _step,\
                                .value = _def_val,\
                                .name = _name,\
                                .unit = _unit,\
                                .get_value_str_cb = _get_value_str_cb,\
                                .finished_event_cb = NULL,\
                                .cancel_op_cb = NULL,\
                                .update_run_cb = _upd_run_cb,\
                                .assign_value_cb = _assign_cb,\
                                .refresh_btn_cb = _refresh_cb,\
                                .title_to_left = _title_to_left,\
                                .div_to_left = _div_to_left,\
                                .val_to_right = _value_to_right,\
                                .div_disable = _div_disable,     \
                                .cycle = _cycle,\
                                .get_step_cb = _get_step_cb,\
                         }
/**********************
 * GLOBAL PROTOTYPES
 **********************/
extern char sdb[100];
extern lv_style_t style_rect_panel;
extern lv_style_t style_common_btn;
extern lv_style_t style_common_focued_btn;
extern lv_style_t style_menu_item;
extern lv_style_t style_menu_focused_item;
extern lv_style_t style_sys_menu_btn;
extern lv_style_t style_le_btn;
extern lv_style_t style_levelx_btn;
extern lv_style_t style_levelx_win;
extern lv_style_t style_select_panel;;
						 
void item_list_add_tail(item_t** list_head, item_t* item);

void item_add(item_t** list_head, item_t *item, item_id_t id,  lv_obj_t* obj, const void * img_dsc, const char* name);

item_t* item_list_get(item_t* item, int i);

item_t* item_list_get_prev(item_t *item, uint8_t i);

item_t* item_list_get_next(item_t *item, uint8_t i);

void my_indev_set_group(lv_group_t* group);

void common_style_init(void);

void page_hsi_init(lv_obj_t* parent);

void anim_obj_move_x(lv_obj_t* obj, lv_coord_t start_x, lv_coord_t end_x, uint32_t anim_time);

void anim_obj_move_y(lv_obj_t* obj, lv_coord_t start_y, lv_coord_t end_y, uint32_t anim_time);

void anim_obj_size_zoom(lv_obj_t* obj, lv_coord_t start_width, lv_coord_t end_width, uint32_t time);

void anim_obj_img_zoom(lv_obj_t* obj, uint16_t coff_start, uint16_t coff_end, uint32_t time, lv_anim_ready_cb_t ready_cb);

void anim_obj_move(lv_obj_t *obj, lv_coord_t start_x, lv_coord_t start_y, \
                                            lv_coord_t end_x, lv_coord_t end_y, uint32_t time);

void format_intensity(char* sdb, uint16_t size, uint16_t intensity);

void format_gm(char* sdb, uint16_t size, int16_t gm);

void format_adv_hsi_hue(char* sdb, uint16_t size, uint16_t hue);

void format_adv_hsi_sat(char* sdb, uint16_t size, uint16_t sat);

const char* intensity_get_value_str(void* data);

const char* hue_get_value_str(void* data);

const char* adv_hsi_hue_get_value_str(void* data);

const char* sat_get_value_str(void* data);

const char* adv_hsi_sat_get_value_str(void* data);

const char* cct_get_value_str(void* data);

const char* gm_get_value_str(void* data);

const char* xy_get_value_str(void* data);

const char* rgb_get_value_str(void* data);

const char* frequence_get_value_str(void* data);

const char* speed_get_value_str(void* data);

const char* decay_get_value_str(void* data);

const char* light_mode_get_title(uint8_t light_m);

const char* power_value_get_value_str(void* data);

const char* db_value_get_value_str(void* data);
	
const char* power_limit_get_str(void* data);
	
void title_cont_refresh(lv_obj_t* t_cont, ui_title_t* p_t_model, bool force_upd);

void title_cont_create(lv_obj_t* t_cont, const char* name);

void title_update_handler(lv_obj_t* t_cont);

void light_effect_title_create(lv_obj_t* t_cont, const char* title_name);

void screen_event_cb(lv_event_t* e);

void obj_send_event_to_screen(lv_event_code_t code, void *param);

void screen_del_l2_panel(void);

bool l2_screen_is_exist(void);

gel_style_t* gel_style_get(void);

void gel_style_init(gel_style_t* p_style);

void lighteffect_style_init(lighteffect_style_t *pstyle);

lighteffect_style_t* lighteffect_style_get(void);

void my_str_replace(char* ptr, char target, char replace);

void refresh_xy_selector(xy_selector_t* p_selector, uint16_t x, uint16_t y);


//void* page_get_user_data(uint8_t id);

#ifdef __cplusplus
}
#endif


#endif // ! UI_COMMON_H
