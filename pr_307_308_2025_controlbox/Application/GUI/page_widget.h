#ifndef PAGE_WIDGET_H
#define PAGE_WIDGET_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "lvgl/lvgl.h"
#include "ui_common.h"
/*********************
 *      DEFINES
 *********************/
#define ASSIGN_VALUE_TO_BTN(x)                      \
    if (dsc->value != (x))                          \
    {                                               \
        if (((x) >= dsc->min) && ((x) <= dsc->max)) \
        {                                           \
            if ((x) != dsc->value)                  \
            {                                       \
                dsc->value = (x);                   \
                return true;                        \
            }                                       \
        }                                           \
        else                                        \
        {                                           \
            (x) = dsc->value;                       \
        }                                           \
    }                                               \
    return false;


/**********************
 *      TYPEDEFS
 **********************/

enum {
    LV_USER_EXIT_STATE_EDIT = 0,    
    LV_USER_EXIT_STATE_EXIT = 1,
};

enum {
    LV_USER_STATE_IDLE = 0,
    LV_USER_STATE_ENTER,
    LV_USER_STATE_KEY_BACKSPACE,
    LV_USER_STATE_EXIT_ALL,
};

enum  {
    MY_RADIOBOX_TYPE1 = 0,
    MY_RADIOBOX_TYPE2 = 1,
};

enum {
    ADJ_BTN_TYPE_14,
    ADJ_BTN_TYPE_18,
    ADJ_BTN_TYPE_24,
};

typedef struct {
    lv_obj_t* obj;
    int16_t cur_index;    
    int16_t min;
    int16_t max;
    uint8_t state;
    uint8_t max_disp_nums;
}custom_cont_t;

typedef struct {
    lv_obj_t* btn;
    int16_t cur_index;    
    int16_t min;
    int16_t max;
    lv_coord_t title_to_top;
    lv_coord_t img_to_top;
    lv_coord_t img_gap;
    const char** map; 
    const lv_img_dsc_t** img_map;
    void (*update_cb)(void *data);
    void (*event_handler_cb)(void *data);    
    void (*widget_refresh_cb)(lv_obj_t* obj, bool focused, bool edited);    
}state_btn_t;

typedef struct {
    lv_obj_t* btn;
    bool anim_is_running;    
    lv_coord_t img_to_top;
    lv_coord_t title_to_top;
    void (*released_cb)(void); 
}trigger_btn_t;

typedef struct _light_mode_roller_ext{
    lv_obj_t* obj;
    lv_obj_t* disp_panel;
    param_setting_type_t type;
    struct db_fx_mode_arg *setting;
    void (*value_changed_cb)(struct _light_mode_roller_ext* ext);
}light_mode_roller_ext_t;

typedef struct {
    lv_obj_t* obj;
    void (*value_changed_cb)(uint32_t value);
}my_roller_ext_t;

typedef struct {
    int16_t min;
    int16_t max;
    int16_t cur_index;    

    lv_obj_t* obj;
    lv_obj_t* disp_panel;        
    param_setting_type_t* type;    
    union {
        struct db_fx_mode_arg  *param;
        struct db_fx_mode_limit_arg   *limit;
    } setting;

    void (*update_cb)(void* data);
    void (*release_cb)(void* data);
}my_btnlist_ext_t;

typedef struct {
    uint8_t type;
    uint8_t sel;            /* 控件当前选中值 */
    uint8_t max;
    uint8_t items_of_page;
    uint8_t default_val;    /* 控件默认选中值 */
    lv_obj_t* obj;          /* 对象 */
    const char* (*get_name_str)(int8_t id);
    void (*release_cb)(lv_event_t* e, uint8_t index);
}my_radiobox_ext_t;

typedef struct {
    lv_obj_t* obj;
    uint8_t sel;
    uint8_t max;
    void (*custom_event_cb)(lv_event_t* e, uint8_t index);    
}my_btnmatrix_ext_t;

typedef struct {   
    uint8_t cur_sel; 
    uint8_t childs;
	uint8_t max;
    void (*release_cb)(lv_event_t* e, uint8_t index);
}my_list_ext_t;

//my msgbox 
enum {
    MSGBOX_ID_YES = 0,
    MSGBOX_ID_NO = 1,
    MSGBOX_ID_NO_CONFIRM,
};

enum {
    MSGBOX_TYPE_TIMEOUT = 0,
    MSGBOX_TYPE_FOREVER,
};

typedef struct _my_msgbox_ext{
    const char* title_str;
    const lv_img_dsc_t* body_img;
    const char* body_comment_str;
    int8_t btn_sel;
    uint8_t answer;
    uint8_t type;
    uint32_t timeout;
    uint32_t timecnt;
    lv_obj_t* msgbox;   
    lv_timer_t* msgbox_timer; 
    void (*msgbox_del_cb)(struct _my_msgbox_ext* ext);
}my_msgbox_ext_t;

typedef struct _ip_btn {
    uint8_t sel;
    uint8_t* buff;
    lv_obj_t* obj;
    void (*user_event_cb)(lv_event_t* e);
}ip_btn_ext_t;

typedef struct {
    const char* name;
    const char* value;
}kv_string_t;

typedef struct _gel_switch_btn_t{
    uint8_t btn_id;
    uint8_t sel_index;
    lv_obj_t* btn;
    void (*ralease_cb)(struct _gel_switch_btn_t* btn);
}gel_switch_btn_t;

typedef struct {
    lv_obj_t* list;    
    int8_t selected;
    int8_t nums;
    uint8_t bubble : 1;
    void (*value_changed_cb)(uint32_t value);
}light_mode_list_ext_t;

typedef struct arrow_cct_pos{
    int16_t star_x;
    int16_t star_y;
    int16_t middle_x;
    int16_t middle_y;
    int16_t end_x;
    int16_t end_y;
}arrow_cct_pos_type;

/**********************
 * GLOBAL PROTOTYPES
 **********************/
lv_obj_t* drop_down_create(lv_obj_t* obj);

lv_obj_t* page_preset_task_init(lv_obj_t* parent);

lv_obj_t* adj_btn_create(lv_obj_t* parent, lv_coord_t radius, uint8_t type);

lv_obj_t* intensity_bar_create(lv_obj_t* parent, lv_coord_t w, lv_coord_t h, lv_coord_t gap, lv_coord_t x_ofs);

void adj_btn_init(lv_obj_t* parent, adj_btn_t* dsc, uint32_t value);

void adj2_btn_init(lv_obj_t* parent, adj_btn_t* dsc, uint32_t value);

void adj_btn_refresh(lv_obj_t* parent, adj_btn_t* dsc);

void adj_btn2_refresh(lv_obj_t* parent, adj_btn_t* dsc);

void adj_btn3_refresh(lv_obj_t* parent, adj_btn_t* dsc);

void adj_btn4_refresh(lv_obj_t* parent, adj_btn_t* dsc);

void adj_btn5_refresh(lv_obj_t* parent, adj_btn_t* dsc);

void adj2_btn_refresh(lv_obj_t* parent, adj_btn_t* dsc);

void adj_btn_event_cb(lv_event_t* e);

bool adj_btn_run_assign_value_cb(adj_btn_t* p_adj_btn);

void adj_btn_run_update_cb(adj_btn_t* p_adj_btn);

void adj_btn_refresh_all(adj_btn_t* p_btn_dsc, uint8_t num, bool force_upd);

void serials_cont_init(lv_obj_t* cont, struct gel_param* p_model);

void gel_cont_event_cb(lv_event_t* e);

void gel_cont_table_init(lv_obj_t* cont, struct gel_param* p_gel_model, lv_coord_t cont_height);

void gel_cont_table_fliping_refresh(lv_obj_t* parent_cont, struct gel_param *p_gel_model, int16_t cur_index);

void gel_cont_table_fliping_event_cb(lv_event_t* e, struct gel_param *p_gel_model);

lv_obj_t* gel_switch_btn_create(lv_obj_t* obj, const char* string1, const char* string2, lv_coord_t w, lv_coord_t h);

void gel_switch_btn_event(lv_event_t* e);

void gel_serials_cont_event_cb(lv_event_t* e, struct gel_param *p_gel_model, lv_obj_t* p_gel_table_cont);

void gel_serials_cont_refresh(custom_cont_t* p_cont_dsc);

void gel_cont_refresh(lv_obj_t* cont, const gel_dsc_t* p_gel_dsc);

void gel_switch_btn_refresh(gel_switch_btn_t* p_switch_btn_dsc);

lv_obj_t* lightmode_roller_create(lv_obj_t* parent, lv_coord_t w, lv_coord_t h);

lv_obj_t* lightmode_disp_panel_create(lv_obj_t* parent, lv_coord_t w, lv_coord_t h);

void lightmode_disp_panel_limit_setting_init(lv_obj_t* disp_panel, param_setting_type_t type, struct db_fx_mode_limit_arg *setting);

void lightmode_disp_panel_limit_setting_refresh(lv_obj_t* disp_panel, param_setting_type_t type, struct db_fx_mode_limit_arg *setting);

void lightmode_disp_panel_refresh(lv_obj_t* disp_panel, param_setting_type_t type,struct db_fx_mode_arg *setting);

void lightmode_roller_event_cb(lv_event_t* e);

lv_obj_t* my_roller_create(lv_obj_t* parent, lv_coord_t w, lv_coord_t h, const char* str_options);

void my_roller_event_cb(lv_event_t* e);

void state_btn_refresh(lv_obj_t* trigger_btn, bool focused, bool editing);

void state_btn_event(lv_event_t* e);

lv_obj_t* state_btn_create(lv_obj_t *parent, state_btn_t *user_data, lv_coord_t w, lv_coord_t h);

lv_obj_t* btnlist_create(lv_obj_t* parent, lv_coord_t w, lv_coord_t h, my_btnlist_ext_t* ext, const char** list_str_map);

void btnlist_event_cb(lv_event_t* e);

void btnlist_refresh(lv_obj_t* btnlist, int16_t next_index);

void shortcut_keys_event_handler(lv_event_t* e);

void backspace_key_event_handler(lv_event_t* e);

//lv_obj_t* my_radiobox_list_create(lv_obj_t* parent, lv_coord_t w, my_radiobox_ext_t* p_ext, uint8_t type);

lv_obj_t* my_radiobox_list_create(lv_obj_t* parent, lv_coord_t w, my_radiobox_ext_t* p_ext, uint8_t type, lv_align_t align);

lv_obj_t* my_radiobox1_list_create(lv_obj_t* parent, lv_coord_t w, my_radiobox_ext_t* p_ext, uint8_t type, lv_align_t align, uint8_t enter_value);

void my_radiobox_list_event_cb(lv_event_t* e);

void my_radiobox_event_cb(lv_event_t* e);

void my_btnmatrix_event_cb(lv_event_t* e);

lv_obj_t* my_list_create(lv_obj_t* parent, lv_coord_t w, lv_coord_t h, \
                                    my_list_ext_t* p_ext);

lv_obj_t* my_list_add_btn(lv_obj_t* list, const char* name, lv_coord_t label_to_left);

lv_obj_t* my_list_add_btn3(lv_obj_t* list, const char* name, lv_coord_t label_to_left,uint8_t value);

lv_obj_t* my_list_add_btn1(lv_obj_t* list, const char* name);

lv_obj_t* my_list_add_btn2(lv_obj_t* list, const char* name, const lv_img_dsc_t* p_img_dsc, uint8_t type);

lv_obj_t* my_list_add_btn4(lv_obj_t* list);

lv_obj_t* my_list_add_switch_btn(lv_obj_t* list, const char* name, bool state);

void list_switch_btn_refresh(lv_obj_t* obj, bool state);

void list_state_btn_refresh(lv_obj_t* obj, bool state);

my_msgbox_ext_t* my_msgbox_get_ext(void);

void my_msgbox_reset(void);

lv_obj_t* my_msgbox_create(my_msgbox_ext_t* p_ext);

lv_obj_t* my_msgbox1_create(my_msgbox_ext_t* p_ext);

lv_obj_t* my_msgbox2_create(my_msgbox_ext_t* p_ext);  //testing

lv_obj_t* my_msgbox3_create(my_msgbox_ext_t* p_ext);

lv_obj_t* my_msgbox4_create(my_msgbox_ext_t* p_ext);

lv_obj_t* ip_btn_create(lv_obj_t* parent, const char* name, ip_btn_ext_t* ext, uint8_t mode);

lv_obj_t* info_btn_create(lv_obj_t* parent, kv_string_t *p_kv);

lv_obj_t* warning_info_init(lv_obj_t* parent, const lv_img_dsc_t* p_img, lv_coord_t img_to_top, lv_coord_t label_to_img,const char* comment);

lv_obj_t* my_long_list_create(lv_obj_t* parent, lv_coord_t w, my_radiobox_ext_t* p_ext);

lv_obj_t* my_list_scroll_create(lv_obj_t* parent, lv_coord_t w, lv_coord_t h);

void my_list_scroll_refresh(lv_obj_t* scroll_cont, int8_t percent);

void trigger_btn_event(lv_event_t* e);

lv_obj_t* trigger_btn_create(lv_obj_t* parent, trigger_btn_t* p_btn_dsc, lv_coord_t w, lv_coord_t h);

void state_btn_refresh(lv_obj_t* trigger_btn, bool focused, bool editing);

bool xy_is_in_triangle(xy_selector_t* p_selector, uint16_t x, uint16_t y);

void xy_set_root_mode(bool mode);

bool xy_get_root_mode(void);

void light_mode_list_event_cb(lv_event_t* e);

void refresh_light_mode_list(lv_obj_t* list, lv_state_t state);

lv_obj_t* light_mode_list_create(lv_obj_t* parent, light_mode_list_ext_t* p_ext);

lv_obj_t* light_mode_list_add_label(lv_obj_t* list, const char* str);

lv_obj_t* my_smooth_list_create(lv_obj_t* parent, lv_coord_t w, my_radiobox_ext_t* p_ext, uint8_t type, lv_align_t align);

void my_smooth_list_event_cb(lv_event_t* e);

lv_obj_t* my_msgbox2_create(my_msgbox_ext_t* p_ext);  //testing

void my_obj_event_create(lv_obj_t* parent);

lv_obj_t* circle_create(lv_obj_t* parent, uint16_t circle_size);

void adj_int_widget_creat(lv_obj_t * parent,uint16_t arc_size, uint16_t circle_size, uint16_t int_val, adj_btn_t* btn);

void adj_frq_widget_creat(lv_obj_t * parent,uint16_t circle_size, uint16_t val, adj_btn_t* btn );

lv_obj_t* adj_state_widget_creat(lv_obj_t * parent, uint8_t* val, lv_coord_t w, lv_coord_t h, void (*update_cb)(void* data));

lv_obj_t* adj_trigger_widget_creat(lv_obj_t * parent, lv_coord_t w, lv_coord_t h,trigger_btn_t* trigger_btn_d);

void adj_cct_int_widget_creat(lv_obj_t * parent,uint16_t arc_size, uint16_t circle_size, uint16_t int_val, adj_btn_t* btn);

void adj_cct_widget_creat(lv_obj_t * parent, uint16_t img_zoom, uint16_t cct_val, adj_btn_t* btn, uint8_t type);

void effects_refresh_scale_img(lv_obj_t* panel, int16_t value1, int16_t min1, int16_t max1);

void adj_cct_refresh(lv_obj_t* parent, adj_btn_t* dsc);

void adj_btn2_event_cb(lv_event_t* e);

void refresh_radiobox_img(lv_obj_t* radiobox, bool seleted);

void adj_decay_widget_creat(lv_obj_t * parent,uint16_t circle_size, uint16_t val, adj_btn_t* btn );

lv_obj_t* my_long_list2_create(lv_obj_t* parent, lv_coord_t w, my_radiobox_ext_t* p_ext);

#ifdef __cplusplus
}
#endif

#endif // ! PAGE_WIDGET_H
