/********************* 
 *      INCLUDES   电动附件
 *********************/
#include "page_manager.h"
#include "ui_common.h"
#include "page_widget.h"
#include "ui_data.h"
#include "stdio.h"
/*********************
*      DEFINES
*********************/
#define CANVAS_WIDTH   172
#define CANVAS_HEIGHT  170

#define TRIANGLE_X0    CANVAS_WIDTH / 2
#define TRIANGLE_Y0    CANVAS_HEIGHT

#define PAGE_DEMO_BTN_NUMBER (8u)

#define ELE_RESET_TIMEOUT    (30)

/*********************
*      TPEDEFS
*********************/

LV_IMG_DECLARE(ImgRadioboxSel)
LV_IMG_DECLARE(ImgRadioboxNoSel)

typedef struct {
    //uint8_t cur_sel;
    uint8_t cur_sel_sub;
   // uint8_t childs;
    int16_t* buff;
   // void (*release_cb)(lv_event_t* e, uint8_t index);
}btn_ext_t;


/**********************
*  STATIC PROTOTYPES
**********************/
static void page_elect_accessory_construct(void);
static void page_elect_accessory_destruct(void);
static void page_elect_accessory_time_upd_cb(uint32_t ms);
static void angle_upd_run_cb(void* data);
static bool angle_assign_value_cb(void* data);
static void disp_pannel_init(lv_obj_t* cont);
static void triangle_refresh(lv_obj_t *cont, uint16_t angle);
static void triangle_run_upd(int16_t value, uint8_t mode);
static void motor_demo_text_init(lv_obj_t *parent, int16_t *limit_value, const char *name);
static void motor_limit_state_refresh(lv_obj_t *obj, uint8_t cnt);
static void motor_limit_text_refresh(lv_obj_t *obj, int16_t angle, uint8_t cnt);
static uint8_t motor_reset_state_get(void);

static void angle_onoff_event_cb(lv_event_t* e);
static void limit_btn_event_cb(lv_event_t* e);
static lv_obj_t* limit_btn_creat(lv_obj_t* parent, const char* name, uint16_t label_x, uint16_t label_y, btn_ext_t* btn_use_data);
static void refresh_yoke_disp(void);
static void limit_res_event_cb(lv_event_t* e);

static void angle_reset_event_cb(my_msgbox_ext_t* ext);
static void page_elect_accessory_event_cb(int event);
    
static lv_obj_t* btn_panel_grid_creat(lv_coord_t w, lv_coord_t h, lv_coord_t* col, lv_coord_t* row, lv_coord_t left_offset);
static void get_yoke_btn_str(uint8_t type, str_dsc_t* pdsc);
static void refresh_lock_img(lv_obj_t* img, uint8_t lock);
static void yoke_pan_event_cb(lv_event_t* e);
static void block_cal_event_cb(my_msgbox_ext_t* ext);
static void origin_rst_event_cb(my_msgbox_ext_t* ext);
static void yoke_btn_event_cb(lv_event_t* e);
static void refresh_adj_value_label(lv_obj_t* label, float val);
static void yoke_info_creat(lv_obj_t* parent ,int16_t min_val,int16_t max_val,const void* img_src , lv_coord_t x_ofs ,char * string);
static void access_btn_eent_cb(lv_event_t* e);
static void limit_confirm_event_cb(lv_event_t* e);
static const char* select_acc_name_get_str(int8_t x);
static void select_an_acc_choose_list_user_event(lv_event_t* e, uint8_t index);
static void access_wing_yes_event_cb(lv_event_t* e);
static void access_wing_no_event_cb(lv_event_t* e);
static const char* select_acc_name_get_str1(int8_t x);
static void fre_reset_btn_event_cb(lv_event_t* e);
static void switch_btn_event_cb(lv_event_t* e);
static void demo_limit_btn_event_cb(lv_event_t* e);
static void angle_upd_run_time(void);
static void bar_anim_exec_xcb(void *a, int32_t val);
/*********************
 *  STATIC VATIABLES
 *********************/
static lv_obj_t* angle_label;

static lv_group_t* indev_group;
static UI_State_Data_t* p_sys_menu_model = NULL;

static my_list_ext_t list_ext;
static uint8_t list_to_sub[ELE_ACCESSORY_NUM];
static lv_obj_t* img_onoff;
static void warning_event_cb(lv_event_t* e);
static btn_ext_t btn_ext[2];
static lv_obj_t* motor_demo_list = NULL;
static my_list_ext_t list_ext1;
static my_list_ext_t s_list_ext[3];
static lv_obj_t* life_line[79];
static lv_obj_t* right_line[79];
static lv_obj_t* life_anim_line[20];
static lv_obj_t* right_anim_line[20];
static lv_obj_t* hiden_btn;
static lv_obj_t* demo_obj_num[PAGE_DEMO_BTN_NUMBER] = {NULL};
static lv_obj_t* access_scroll_cont = NULL;
static lv_obj_t* demo_scroll_cont = NULL;
static int16_t   trigger_angle_value;
static uint8_t dec_value;
static struct sys_info_accessories  s_access_info;
static struct sys_info_accessories  s_access_info_last;
static uint8_t s_pan_tilt_flag = 0;
static uint8_t s_access_flag = 0;
static my_radiobox_ext_t access_radiobox_ext;
static my_radiobox_ext_t access_radiobox_ext1;
static int16_t pan_value[2] = {ANGLE_LIMIT_MIN, ANGLE_LIMIT_MAX};
static int16_t liti_value[2] = {ANGLE_LIMIT_DEFAULT_MIN, ANGLE_LIMIT_DEFAULT_MAX};
static uint8_t  limit_on_value;
static uint8_t triangle_mode = 0;
static float ele_fresnel_angle = 0;
static float ele_reset_flag = 0;
static char s_select_an_acc_name_str[][25] = {
"light Dome II", 
"Light Dome 150",
"Light Dome 120",
"Lantern 90",
"Lantern",
"Light Dome III",
"Spotlight Max",
"Use of non-Aputure acc.", 
"Non-Aputure acc.NO.1",
"Non-Aputure acc.NO.2",
"Non-Aputure acc.NO.3",
"NO accessories",
};
static char s_select_an_acc_name_ch_str[][25] = {
"light Dome II", 
"Light Dome 150",
"Light Dome 120",
"Lantern 90",
"Lantern",
"Light Dome III",
"Spotlight Max",
"使用非爱图仕附件", 
"自定义附件01",
"自定义附件02",
"自定义附件03",
"没有使用附件",
};

static char s_select_an_acc_name_str1[][25] = { 
"Non-Aputure acc.NO.1",
"Non-Aputure acc.NO.2",
"Non-Aputure acc.NO.3",
};
static char s_select_an_acc_name_ch_str1[][25] = {
"自定义附件01",
"自定义附件02",
"自定义附件03",
};

static struct sys_info_motor       motor_info;
static struct sys_info_motor_state  s_motor_state;
static uint8_t access_spid =  0;
static uint8_t motor_reset_flag = 0;
static uint8_t s_ui_fresnel_angle_count = 0;
static uint8_t wring_flag = 0;
static lv_point_t life_line_point[79][2] = {
                                           { {46, 45 + 2},       {46 + 40, 48 + 150} },
                                           { {46, 45 + 2 * 2},   {46 + 40, 48 + 150} },
                                           { {46, 45 + 2 * 2},   {46 + 40, 48 + 150} },
                                           { {46, 45 + 2 * 3},   {46 + 40, 48 + 150} },
                                           { {46, 45 + 2 * 4},   {46 + 40, 48 + 150} },
                                           { {46, 45 + 2 * 5},   {46 + 40, 48 + 150} },
                                           { {46, 45 + 2 * 6},   {46 + 40, 48 + 150} },
                                           { {46, 45 + 2 * 7},   {46 + 40, 48 + 150} },
                                           { {46, 45 + 2 * 8},   {46 + 40, 48 + 150} },
                                           { {46, 45 + 2 * 9},   {46 + 40, 48 + 150} },
                                           { {46, 45 + 2 * 10},  {46 + 40, 48 + 150} },
                                           { {46, 45 + 2 * 11},  {46 + 40, 48 + 150} },
                                           { {46, 45 + 2 * 12},  {46 + 40, 48 + 150} },
                                           { {46, 45 + 2 * 13},  {46 + 40, 48 + 150} },
                                           { {46, 45 + 2 * 14},  {46 + 40, 48 + 150} },
                                           { {46, 45 + 2 * 15},  {46 + 40, 48 + 150} },
                                           { {46, 45 + 2 * 16},  {46 + 40, 48 + 150} },
                                           { {46, 45 + 2 * 17},  {46 + 40, 48 + 150} },
                                           { {46, 45 + 2 * 18},  {46 + 40, 48 + 150} },
                                           { {46, 45 + 2 * 19},  {46 + 40, 48 + 150} },
                                           { {46, 45 + 2 * 20},  {46 + 40, 48 + 150} },
                                           { {46, 45 + 2 * 21},  {46 + 40, 48 + 150} },
                                           { {46, 45 + 2 * 22},  {46 + 40, 48 + 150} },
                                           { {46, 45 + 2 * 23},  {46 + 40, 48 + 150} },
                                           { {46, 45 + 2 * 24},  {46 + 40, 48 + 150} },
                                           { {46, 45 + 2 * 25},  {46 + 40, 48 + 150} },
                                           { {46, 45 + 2 * 26},  {46 + 40, 48 + 150} },
                                           { {46, 45 + 2 * 27},  {46 + 40, 48 + 150} },
                                           { {46, 45 + 2 * 28},  {46 + 40, 48 + 150} },
                                           { {46, 45 + 2 * 29},  {46 + 40, 48 + 150} },
                                           { {46, 45 + 2 * 30},  {46 + 40, 48 + 150} },
                                           { {46, 45 + 2 * 31},  {46 + 40, 48 + 150} },
                                           { {46, 45 + 2 * 32},  {46 + 40, 48 + 150} },
                                           { {46, 45 + 2 * 33},  {46 + 40, 48 + 150} },
                                           { {46, 45 + 2 * 34},  {46 + 40, 48 + 150} },
                                           { {46, 45 + 2 * 35},  {46 + 40, 48 + 150} },
                                           { {46, 45 + 2 * 36},  {46 + 40, 48 + 150} },
                                           { {46, 45 + 2 * 37},  {46 + 40, 48 + 150} },
                                           { {46, 45 + 2 * 38},  {46 + 40, 48 + 150} },
                                           { {46, 45 + 2 * 39},  {46 + 40, 48 + 150} },
                                           { {46, 45 + 2 * 40},  {46 + 40, 48 + 150} },
                                           { {46, 45 + 2 * 41},  {46 + 40, 48 + 150} },
                                           { {46, 45 + 2 * 42},  {46 + 40, 48 + 150} },
                                           { {46, 45 + 2 * 43},  {46 + 40, 48 + 150} },
                                           { {46, 45 + 2 * 44},  {46 + 40, 48 + 150} },
                                           { {46, 45 + 2 * 45},  {46 + 40, 48 + 150} },
                                           { {46, 45 + 2 * 46},  {46 + 40, 48 + 150} },
                                           { {46, 45 + 2 * 47},  {46 + 40, 48 + 150} },
                                           { {46, 45 + 2 * 48},  {46 + 40, 48 + 150} },
                                           { {46, 45 + 2 * 49},  {46 + 40, 48 + 150} },
                                           { {46, 45 + 2 * 50},  {46 + 40, 48 + 150} },
                                           { {46, 45 + 2 * 51},  {46 + 40, 48 + 150} },
                                           { {46, 45 + 2 * 52},  {46 + 40, 48 + 150} },
                                           { {46, 45 + 2 * 53},  {46 + 40, 48 + 150} },
                                           { {46, 45 + 2 * 54},  {46 + 40, 48 + 150} },
                                           { {46, 45 + 2 * 55},  {46 + 40, 48 + 150} },
                                           { {46, 45 + 2 * 56},  {46 + 40, 48 + 150} },
                                           { {46, 45 + 2 * 57},  {46 + 40, 48 + 150} },
                                           { {46, 45 + 2 * 58},  {46 + 40, 48 + 150} },
                                           { {46, 45 + 2 * 59},  {46 + 40, 48 + 150} },
                                           { {46, 45 + 2 * 60},  {46 + 40, 48 + 150} },
                                           { {46, 45 + 2 * 61},  {46 + 40, 48 + 150} },
                                           { {46, 45 + 2 * 62},  {46 + 40, 48 + 150} },
                                           { {46, 45 + 2 * 63},  {46 + 40, 48 + 150} },
                                           { {46, 45 + 2 * 64},  {46 + 40, 48 + 150} },
                                           { {46, 45 + 2 * 65},  {46 + 40, 48 + 150} },
                                           { {46, 45 + 2 * 66},  {46 + 40, 48 + 150} },
                                           { {46, 45 + 2 * 67},  {46 + 40, 48 + 150} },
                                           { {46, 45 + 2 * 68},  {46 + 40, 48 + 150} },
                                           { {46, 45 + 2 * 69},  {46 + 40, 48 + 150} },
                                           { {46, 45 + 2 * 70},  {46 + 40, 48 + 150} },
                                           { {46, 45 + 2 * 71},  {46 + 40, 48 + 150} },
                                           { {46, 45 + 2 * 72},  {46 + 40, 48 + 150} },
                                           { {46, 45 + 2 * 73},  {46 + 40, 48 + 150} },
                                           { {46, 45 + 2 * 74},  {46 + 40, 48 + 150} },
                                           { {46, 45 + 2 * 75},  {46 + 40, 48 + 150} },
                                           { {46, 45 + 2 * 76},  {46 + 40, 48 + 150} },
                                           { {46, 45 + 2 * 77},  {46 + 40, 48 + 150} },
                                           { {46, 45 + 2 * 78},  {46 + 40, 48 + 150} },
};

static lv_point_t right_line_point[79][2] = {
                                            { {46 + 80, 45 + 2},       {46 + 40, 48 + 150} },
                                            { {46 + 80, 45 + 2 * 2},   {46 + 40, 48 + 150} },
                                            { {46 + 80, 45 + 2 * 2},   {46 + 40, 48 + 150} },
                                            { {46 + 80, 45 + 2 * 3},   {46 + 40, 48 + 150} },
                                            { {46 + 80, 45 + 2 * 4},   {46 + 40, 48 + 150} },
                                            { {46 + 80, 45 + 2 * 5},   {46 + 40, 48 + 150} },
                                            { {46 + 80, 45 + 2 * 6},   {46 + 40, 48 + 150} },
                                            { {46 + 80, 45 + 2 * 7},   {46 + 40, 48 + 150} },
                                            { {46 + 80, 45 + 2 * 8},   {46 + 40, 48 + 150} },
                                            { {46 + 80, 45 + 2 * 9},   {46 + 40, 48 + 150} },
                                            { {46 + 80, 45 + 2 * 10},  {46 + 40, 48 + 150} },
                                            { {46 + 80, 45 + 2 * 11},  {46 + 40, 48 + 150} },
                                            { {46 + 80, 45 + 2 * 12},  {46 + 40, 48 + 150} },
                                            { {46 + 80, 45 + 2 * 13},  {46 + 40, 48 + 150} },
                                            { {46 + 80, 45 + 2 * 14},  {46 + 40, 48 + 150} },
                                            { {46 + 80, 45 + 2 * 15},  {46 + 40, 48 + 150} },
                                            { {46 + 80, 45 + 2 * 16},  {46 + 40, 48 + 150} },
                                            { {46 + 80, 45 + 2 * 17},  {46 + 40, 48 + 150} },
                                            { {46 + 80, 45 + 2 * 18},  {46 + 40, 48 + 150} },
                                            { {46 + 80, 45 + 2 * 19},  {46 + 40, 48 + 150} },
                                            { {46 + 80, 45 + 2 * 20},  {46 + 40, 48 + 150} },
                                            { {46 + 80, 45 + 2 * 21},  {46 + 40, 48 + 150} },
                                            { {46 + 80, 45 + 2 * 22},  {46 + 40, 48 + 150} },
                                            { {46 + 80, 45 + 2 * 23},  {46 + 40, 48 + 150} },
                                            { {46 + 80, 45 + 2 * 24},  {46 + 40, 48 + 150} },
                                            { {46 + 80, 45 + 2 * 25},  {46 + 40, 48 + 150} },
                                            { {46 + 80, 45 + 2 * 26},  {46 + 40, 48 + 150} },
                                            { {46 + 80, 45 + 2 * 27},  {46 + 40, 48 + 150} },
                                            { {46 + 80, 45 + 2 * 28},  {46 + 40, 48 + 150} },
                                            { {46 + 80, 45 + 2 * 29},  {46 + 40, 48 + 150} },
                                            { {46 + 80, 45 + 2 * 30},  {46 + 40, 48 + 150} },
                                            { {46 + 80, 45 + 2 * 31},  {46 + 40, 48 + 150} },
                                            { {46 + 80, 45 + 2 * 32},  {46 + 40, 48 + 150} },
                                            { {46 + 80, 45 + 2 * 33},  {46 + 40, 48 + 150} },
                                            { {46 + 80, 45 + 2 * 34},  {46 + 40, 48 + 150} },
                                            { {46 + 80, 45 + 2 * 35},  {46 + 40, 48 + 150} },
                                            { {46 + 80, 45 + 2 * 36},  {46 + 40, 48 + 150} },
                                            { {46 + 80, 45 + 2 * 37},  {46 + 40, 48 + 150} },
                                            { {46 + 80, 45 + 2 * 38},  {46 + 40, 48 + 150} },
                                            { {46 + 80, 45 + 2 * 39},  {46 + 40, 48 + 150} },
                                            { {46 + 80, 45 + 2 * 40},  {46 + 40, 48 + 150} },
                                            { {46 + 80, 45 + 2 * 41},  {46 + 40, 48 + 150} },
                                            { {46 + 80, 45 + 2 * 42},  {46 + 40, 48 + 150} },
                                            { {46 + 80, 45 + 2 * 43},  {46 + 40, 48 + 150} },
                                            { {46 + 80, 45 + 2 * 44},  {46 + 40, 48 + 150} },
                                            { {46 + 80, 45 + 2 * 45},  {46 + 40, 48 + 150} },
                                            { {46 + 80, 45 + 2 * 46},  {46 + 40, 48 + 150} },
                                            { {46 + 80, 45 + 2 * 47},  {46 + 40, 48 + 150} },
                                            { {46 + 80, 45 + 2 * 48},  {46 + 40, 48 + 150} },
                                            { {46 + 80, 45 + 2 * 49},  {46 + 40, 48 + 150} },
                                            { {46 + 80, 45 + 2 * 50},  {46 + 40, 48 + 150} },
                                            { {46 + 80, 45 + 2 * 51},  {46 + 40, 48 + 150} },
                                            { {46 + 80, 45 + 2 * 52},  {46 + 40, 48 + 150} },
                                            { {46 + 80, 45 + 2 * 53},  {46 + 40, 48 + 150} },
                                            { {46 + 80, 45 + 2 * 54},  {46 + 40, 48 + 150} },
                                            { {46 + 80, 45 + 2 * 55},  {46 + 40, 48 + 150} },
                                            { {46 + 80, 45 + 2 * 56},  {46 + 40, 48 + 150} },
                                            { {46 + 80, 45 + 2 * 57},  {46 + 40, 48 + 150} },
                                            { {46 + 80, 45 + 2 * 58},  {46 + 40, 48 + 150} },
                                            { {46 + 80, 45 + 2 * 59},  {46 + 40, 48 + 150} },
                                            { {46 + 80, 45 + 2 * 60},  {46 + 40, 48 + 150} },
                                            { {46 + 80, 45 + 2 * 61},  {46 + 40, 48 + 150} },
                                            { {46 + 80, 45 + 2 * 62},  {46 + 40, 48 + 150} },
                                            { {46 + 80, 45 + 2 * 63},  {46 + 40, 48 + 150} },
                                            { {46 + 80, 45 + 2 * 64},  {46 + 40, 48 + 150} },
                                            { {46 + 80, 45 + 2 * 65},  {46 + 40, 48 + 150} },
                                            { {46 + 80, 45 + 2 * 66},  {46 + 40, 48 + 150} },
                                            { {46 + 80, 45 + 2 * 67},  {46 + 40, 48 + 150} },
                                            { {46 + 80, 45 + 2 * 68},  {46 + 40, 48 + 150} },
                                            { {46 + 80, 45 + 2 * 69},  {46 + 40, 48 + 150} },
                                            { {46 + 80, 45 + 2 * 70},  {46 + 40, 48 + 150} },
                                            { {46 + 80, 45 + 2 * 71},  {46 + 40, 48 + 150} },
                                            { {46 + 80, 45 + 2 * 72},  {46 + 40, 48 + 150} },
                                            { {46 + 80, 45 + 2 * 73},  {46 + 40, 48 + 150} },
                                            { {46 + 80, 45 + 2 * 74},  {46 + 40, 48 + 150} },
                                            { {46 + 80, 45 + 2 * 75},  {46 + 40, 48 + 150} },
                                            { {46 + 80, 45 + 2 * 76},  {46 + 40, 48 + 150} },
                                            { {46 + 80, 45 + 2 * 77},  {46 + 40, 48 + 150} },
                                            { {46 + 80, 45 + 2 * 78},  {46 + 40, 48 + 150} },
};

static lv_point_t life_line_anim_point[17][2] = {
                                           { {46    ,  45},  {46 + 40, 48 + 150} },
                                           { {46 + 2,  45},  {46 + 40, 48 + 150} },
                                           { {46 + 3,  45},  {46 + 40, 48 + 150} },
                                           { {46 + 4,  45},  {46 + 40, 48 + 150} },
                                           { {46 + 5,  45},  {46 + 40, 48 + 150} },
                                           { {46 + 6,  45},  {46 + 40, 48 + 150} },
                                           { {46 + 7,  45},  {46 + 40, 48 + 150} },
                                           { {46 + 8,  45},  {46 + 40, 48 + 150} },
                                           { {46 + 9,  45},  {46 + 40, 48 + 150} },
                                           { {46 + 10, 45},  {46 + 40, 48 + 150} },
                                           { {46 + 11, 45},  {46 + 40, 48 + 150} },
                                           { {46 + 12, 45},  {46 + 40, 48 + 150} },
                                           { {46 + 13, 45},  {46 + 40, 48 + 150} },
                                           { {46 + 14, 45},  {46 + 40, 48 + 150} },
                                           { {46 + 15, 45},  {46 + 40, 48 + 150} },
                                           { {46 + 16, 45},  {46 + 40, 48 + 150} },
                                           { {46 + 17, 45},  {46 + 40, 48 + 150} },
};

static lv_point_t right_line_anim_point[17][2] = {
                                           { {46 + 80    ,  45},  {46 + 40, 48 + 150} },
                                           { {46 + 80 - 2,  45},  {46 + 40, 48 + 150} },
                                           { {46 + 80 - 3,  45},  {46 + 40, 48 + 150} },
                                           { {46 + 80 - 4,  45},  {46 + 40, 48 + 150} },
                                           { {46 + 80 - 5,  45},  {46 + 40, 48 + 150} },
                                           { {46 + 80 - 6,  45},  {46 + 40, 48 + 150} },
                                           { {46 + 80 - 7,  45},  {46 + 40, 48 + 150} },
                                           { {46 + 80 - 8,  45},  {46 + 40, 48 + 150} },
                                           { {46 + 80 - 9,  45},  {46 + 40, 48 + 150} },
                                           { {46 + 80 - 10, 45},  {46 + 40, 48 + 150} },
                                           { {46 + 80 - 11, 45},  {46 + 40, 48 + 150} },
                                           { {46 + 80 - 12, 45},  {46 + 40, 48 + 150} },
                                           { {46 + 80 - 13, 45},  {46 + 40, 48 + 150} },
                                           { {46 + 80 - 14, 45},  {46 + 40, 48 + 150} },
                                           { {46 + 80 - 15, 45},  {46 + 40, 48 + 150} },
                                           { {46 + 80 - 16, 45},  {46 + 40, 48 + 150} },
                                           { {46 + 80 - 17, 45},  {46 + 40, 48 + 150} },
};
static adj_btn_t btn_list[] = {
    ADJ_BTN_INITIALIZER(150, 500, 2, 150, "NULL", "°", 0, 0, 0, true, false,  hue_get_value_str, angle_upd_run_cb, angle_assign_value_cb, adj_btn3_refresh, common_angle_get_step),
    ADJ_BTN_INITIALIZER(0, 0, 0, 0, "NULL", "°", 0, 0, 0, true, false,  NULL, NULL, angle_assign_value_cb, NULL, NULL),
    ADJ_BTN_INITIALIZER(ANGLE_LIMIT_MIN, ANGLE_LIMIT_MAX, 1, 0, "NULL", "°", 0, 0, 0, true, false,  NULL, NULL, angle_assign_value_cb, NULL, NULL),
};
/*********************
 *  GLOBAL VATIABLES
 *********************/
page_t page_elect_accessory = PAGE_INITIALIZER("Accessories",
    PAGE_ELECT_ACCESSORY,
    page_elect_accessory_construct,
    page_elect_accessory_destruct,
    page_elect_accessory_time_upd_cb,
    page_elect_accessory_event_cb,
    PAGE_MENU);

/*********************
 *  GLOBAL FUNCTIONS
 *********************/

 /**********************
  *   STATIC FUNCTIONS
  **********************/

static void accessory_list_user_event(lv_event_t* e, uint8_t index)
{
    lv_event_code_t event = lv_event_get_code(e);
    lv_obj_t* obj = lv_event_get_target(e);

    shortcut_keys_event_handler(e);

    if(event == LV_EVENT_RELEASED)
    {
        struct sys_info_lamp   info_read_body = {0};
        uint8_t spid = list_to_sub[index];
        data_center_read_sys_info(SYS_INFO_LAMP, &info_read_body);		
//		if (spid == SUB_ID_ACCESSORY_REGOGNITION)
//		{
//			p_sys_menu_model->regognition_status = p_sys_menu_model->regognition_status ? false : true;	// test
//			data_center_write_config_data(SYS_CONFIG_ACC_RECOGNITION, &p_sys_menu_model->regognition_status);
//		}
		
        if(info_read_body.exist == 1)
        {
            if(spid == SUB_ID_ELE_FRESNEL)
            {
                ui_ble_motor_read_angle(0);
            }
            else if(spid == SUB_ID_ELE_YOKE)
            {
                ui_ble_motor_read_angle(1);
            }
            else
            {
                screen_load_sub_page_with_stack(spid, index);
            }
        }
        else
            screen_load_sub_page_with_stack(spid, index);
    }
    else if (event == LV_EVENT_KEY)
    {
        uint32_t key = *(uint32_t*)lv_event_get_param(e);

        if (key == LV_KEY_BACKSPACE)
        {
			extern void user_turn_to_page(uint8_t id, uint8_t subid, bool save);
			
            user_turn_to_page(PAGE_MENU, 0, true);
        }
		else if ( LV_ENCODER_KEY_CC == key) //up
        {           
            my_list_scroll_refresh(access_scroll_cont, lv_map(index, 0, 5, 0, 100));
        }
        else if (LV_ENCODER_KEY_CW == key) //down
        {   
            my_list_scroll_refresh(access_scroll_cont, lv_map(index, 0, 5, 0, 100));
        }
    }
}

static void page_elect_accessory_construct(void)
{
    indev_group = lv_group_create();
    my_indev_set_group(indev_group);
    lv_group_set_default(indev_group);
 
    p_sys_menu_model = screen_get_act_page_data();
    uint8_t spid = screen_get_act_spid();
    page_event_send(EVENT_DATA_PULL);
    
	data_center_read_sys_info(SYS_INFO_ACCESS, &s_access_info);
    switch (spid)
    {
        case SUB_ID_ACCESSORY_LIST:
        {
			lv_obj_clear_flag(lv_scr_act(), LV_OBJ_FLAG_SCROLLABLE);
            lv_obj_set_style_bg_color(lv_scr_act(), lv_color_black(), 0);
            lv_obj_t* title = lv_label_create(lv_scr_act());

            lv_obj_set_style_text_font(title, Font_ResouceGet(FONT_22_BOLD), 0);
            lv_obj_set_style_text_color(title, lv_color_white(), 0);
            lv_label_set_text(title, Lang_GetStringByID(STRING_ID_ELE_ACCESSORY));
            lv_obj_align(title, LV_ALIGN_TOP_MID, 0, SYS_MENU_TITLE_TO_TOP);
			
			list_ext.cur_sel = access_spid; 
            list_ext.release_cb = accessory_list_user_event;
			list_ext.max = 5;
			
            lv_obj_t* list = my_list_create(lv_scr_act(), lv_pct(85), lv_pct(78), &list_ext);
            lv_obj_set_flex_align(list, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
            lv_obj_align(list, LV_ALIGN_TOP_MID, 0, 40);
			lv_obj_clear_flag(list, LV_OBJ_FLAG_SCROLLABLE);
			
            list_to_sub[list_ext.childs] = SUB_ID_ELE_FRESNEL;
            my_list_add_btn(list, Lang_GetStringByID(STRING_ID_ELE_FRESNEL), 10);
            list_to_sub[list_ext.childs] = SUB_ID_ELE_YOKE;
            my_list_add_btn(list, Lang_GetStringByID(STRING_ID_ELE_YOKE), 10);
			
            list_to_sub[list_ext.childs] = SUB_ID_ELE_OPTICAL_ACCESS;
            my_list_add_btn(list, Lang_GetStringByID(STRING_ID_OPTICAL_ACCESSORIES), 10);
        
			
//			data_center_read_config_data(SYS_CONFIG_ACC_RECOGNITION, &p_sys_menu_model->regognition_status);
//			list_to_sub[list_ext.childs] = SUB_ID_ACCESSORY_REGOGNITION;  // test
//            my_list_add_switch_btn(list, Lang_GetStringByID(STRING_ID_ACC_RECOGNING), p_sys_menu_model->regognition_status);
			
//			list_ext.childs += 1;
		    list_to_sub[list_ext.childs] = SUB_ID_ELE_DEMO;
            my_list_add_btn(list, Lang_GetStringByID(STRING_ID_DEMO), 10);
				
			access_scroll_cont = my_list_scroll_create(lv_scr_act(), 3, 215);
            lv_obj_align_to(access_scroll_cont, list, LV_ALIGN_OUT_RIGHT_TOP, 21, 2); 	
				
            lv_group_add_obj(indev_group, list);
            lv_group_focus_obj(list);
            lv_group_set_editing(indev_group, true);
        }
        break;
        case SUB_ID_ELE_FRESNEL:
        {
			motor_reset_flag = 1;
			char str[10] = {0};
            memcpy(&s_access_info_last, &s_access_info, sizeof(struct sys_info_accessories));
			
			access_spid = 0;
			
            lv_obj_set_style_bg_color(lv_scr_act(), lv_color_black(), 0);
            if(s_access_info.fresnel_state == 0)
            {
                uint8_t lang_type = 0;
                
                lv_obj_t* label1_panel = lv_label_create(lv_scr_act());
                lv_obj_set_style_text_font(label1_panel, Font_ResouceGet(FONT_22_BOLD), 0);
                lv_obj_set_style_text_color(label1_panel, lv_color_white(), 0);
				lv_label_set_text(label1_panel, Lang_GetStringByID(STRING_ID_ELE_FRESNEL));
                lv_obj_align(label1_panel, LV_ALIGN_TOP_MID, 0, 5);
                
                lv_obj_t* btn_plate = lv_obj_create(lv_scr_act());
                lv_obj_add_style(btn_plate, &style_common_btn, 0);
                lv_obj_set_style_radius(btn_plate, 0, 0);
                lv_obj_set_size(btn_plate, 265, 159);
                lv_obj_align(btn_plate, LV_ALIGN_CENTER, 0, 10);
                data_center_read_config_data(SYS_CONFIG_LANG, &lang_type);
                lv_obj_t* label_panel = lv_label_create(btn_plate);
                lv_obj_set_style_text_font(label_panel, Font_ResouceGet(FONT_18_BOLD), 0);
                lv_obj_set_style_text_color(label_panel, lv_color_white(), 0);
				lv_label_set_text(label_panel, Lang_GetStringByID(STRING_ID_NO_FRESNEL));
                lv_obj_align(label_panel, LV_ALIGN_CENTER, 0, 5);
                
                lv_obj_add_event_cb(label_panel, access_btn_eent_cb, LV_EVENT_ALL, NULL);
                lv_group_add_obj(indev_group, label_panel);

                lv_group_focus_obj(label_panel);
                lv_group_set_wrap(indev_group, false);
            }
            else
            {
				
				data_center_read_sys_info(SYS_INFO_MOTOR, &motor_info);
                btn_list[0].name = Lang_GetStringByID(STRING_ID_ANGLE_ADJUST);

                lv_obj_t* disp_pannel = lv_obj_create(lv_scr_act());
                lv_obj_add_style(disp_pannel, &style_rect_panel, 0);
                lv_obj_clear_flag(disp_pannel, LV_OBJ_FLAG_SCROLLABLE);
                lv_obj_set_width(disp_pannel, 172);
                lv_obj_set_height(disp_pannel, 203);
                lv_obj_align(disp_pannel, LV_ALIGN_LEFT_MID, 10, -2);
                
                disp_pannel_init(disp_pannel);

                lv_obj_t* btn_panel = lv_obj_create(lv_scr_act());
                lv_obj_remove_style_all(btn_panel);
                lv_obj_add_style(btn_panel, &style_rect_panel, 0);
                lv_obj_set_flex_flow(btn_panel, LV_FLEX_FLOW_COLUMN);
                lv_obj_set_style_pad_row(btn_panel, 20, 0);
                lv_obj_set_style_bg_color(btn_panel, lv_color_make(0, 0, 0), 0);
                lv_obj_set_size(btn_panel, 122, 154);
                lv_obj_align_to(btn_panel, disp_pannel, LV_ALIGN_OUT_RIGHT_TOP, 8, 0);

                lv_obj_t* angle_btn = adj_btn_create(btn_panel, LIGHT_MODE_COMMON_RADIUS, ADJ_BTN_TYPE_18);
                lv_obj_set_size(angle_btn, 122, 154);
                adj_btn_init(angle_btn, &btn_list[0], 20);
                lv_obj_set_user_data(angle_btn, (void*)&btn_list[0]);
                lv_obj_add_event_cb(angle_btn, adj_btn_event_cb, LV_EVENT_ALL, NULL);
                lv_group_add_obj(indev_group, angle_btn);

				lv_obj_t* reset_btn_panel = lv_obj_create(lv_scr_act());
                lv_obj_remove_style_all(reset_btn_panel);
                lv_obj_add_style(reset_btn_panel, &style_rect_panel, 0);
                lv_obj_set_flex_flow(reset_btn_panel, LV_FLEX_FLOW_COLUMN);
                lv_obj_set_style_pad_row(reset_btn_panel, 20, 0);
                lv_obj_set_style_bg_color(reset_btn_panel, lv_color_make(0, 0, 0), 0);
                lv_obj_set_size(reset_btn_panel, 122, 42);
                lv_obj_align_to(reset_btn_panel, disp_pannel, LV_ALIGN_OUT_RIGHT_BOTTOM, 8, 0);
				
				lv_obj_t* reset_btn = adj_btn_create(reset_btn_panel, LIGHT_MODE_COMMON_RADIUS, ADJ_BTN_TYPE_14);
                lv_obj_set_size(reset_btn, 122, 42);
                lv_obj_add_event_cb(reset_btn, fre_reset_btn_event_cb, LV_EVENT_ALL, NULL);
                lv_group_add_obj(indev_group, reset_btn);
				
				lv_obj_t*  ret_label = lv_label_create(reset_btn);
				lv_obj_set_style_text_font(ret_label, Font_ResouceGet(FONT_14), 0);
				lv_obj_set_style_text_color(ret_label, lv_color_white(), 0);
				lv_label_set_text(ret_label, Lang_GetStringByID(STRING_ID_YOKE_RESET_ORIGIN));
				lv_obj_align(ret_label, LV_ALIGN_CENTER, 0, 5);
				
                lv_group_focus_obj(angle_btn);
                lv_group_set_wrap(indev_group, false);
				lv_obj_t* label_val = lv_obj_get_child(btn_list[0].obj, 2);
				sprintf(str, "%.1f°", motor_info.fresnel_angle);
                adj_btn3_refresh(btn_list[0].obj, &btn_list[0]);  
				lv_label_set_text(angle_label, str);
				lv_label_set_text(label_val, str);
				data_center_read_sys_info(SYS_INFO_MOTOR, &motor_info);
			
            }
        }
        break;
        case SUB_ID_ELE_YOKE:
        {
			access_spid = 1;
            memcpy(&s_access_info_last, &s_access_info, sizeof(struct sys_info_accessories));
			
            lv_obj_set_style_bg_color(lv_scr_act(), lv_color_black(), 0);
			lv_obj_clear_flag(lv_scr_act(), LV_OBJ_FLAG_SCROLLABLE);
            if(s_access_info.yoke_state == 0)
            {
                uint8_t lang_type = 0;
                
                lv_obj_t* label1_panel = lv_label_create(lv_scr_act());
                lv_obj_set_style_text_font(label1_panel, Font_ResouceGet(FONT_22_BOLD), 0);
                lv_obj_set_style_text_color(label1_panel, lv_color_white(), 0);
				lv_label_set_text(label1_panel, Lang_GetStringByID(STRING_ID_ELE_YOKE));
                lv_obj_align(label1_panel, LV_ALIGN_TOP_MID, 0, 5);
                
                lv_obj_t* btn_plate = lv_obj_create(lv_scr_act());
                lv_obj_add_style(btn_plate, &style_common_btn, 0);
                lv_obj_set_style_radius(btn_plate, 0, 0);
                lv_obj_set_size(btn_plate, 265, 159);
                lv_obj_align(btn_plate, LV_ALIGN_CENTER, 0, 10);
                data_center_read_config_data(SYS_CONFIG_LANG, &lang_type);
                lv_obj_t* label_panel = lv_label_create(btn_plate);
                lv_obj_set_style_text_font(label_panel, Font_ResouceGet(FONT_18_BOLD), 0);
                lv_obj_set_style_text_color(label_panel, lv_color_white(), 0);
				lv_label_set_text(label_panel, Lang_GetStringByID(STRING_ID_NO_YOKE));
                lv_obj_align(label_panel, LV_ALIGN_CENTER, 0, 5);
                
                lv_obj_add_event_cb(label_panel, access_btn_eent_cb, LV_EVENT_ALL, NULL);
                lv_group_add_obj(indev_group, label_panel);

                lv_group_focus_obj(label_panel);
                lv_group_set_wrap(indev_group, false);
            }
            else if(s_access_info.yoke_state == 1 && (s_access_info.fresnel_state == 1 || s_access_info.optical_20_access_state == 1 ||s_access_info.optical_30_access_state == 1 ||
                    s_access_info.optical_50_access_state == 1))
            {
				motor_reset_flag = 2;
                s_access_flag = 0;
                lv_obj_t* info_panel = lv_obj_create(lv_scr_act());
				lv_obj_remove_style_all(info_panel);
				lv_obj_set_size(info_panel, 320, 110);
				lv_obj_set_style_bg_color(info_panel, lv_color_make(0, 0, 0), 0);
				lv_obj_set_style_text_font(info_panel, Font_ResouceGet(FONT_14), 0);
				lv_obj_set_style_text_color(info_panel, lv_color_white(), 0);
				lv_obj_align(info_panel, LV_ALIGN_TOP_MID, 0, 0);


                LV_IMG_DECLARE(ImgPan)
                LV_IMG_DECLARE(ImgTilt)
				
				uint8_t tiltm;
				data_center_read_config_data(SYS_CONFIG_ANGLE_LIMIT, &tiltm);
				if(s_access_info.fresnel_state == 1 || s_access_info.optical_20_access_state == 1)
				{
					if(p_sys_menu_model->ele_accessory.ele_yoke_tilt[0] <= -120)
						p_sys_menu_model->ele_accessory.ele_yoke_tilt[0] = -120;
					if(p_sys_menu_model->ele_accessory.ele_yoke_tilt[0] >= 120)
						p_sys_menu_model->ele_accessory.ele_yoke_tilt[0] = 120;
					if(p_sys_menu_model->ele_accessory.ele_yoke_tilt[1] >= 120)
						p_sys_menu_model->ele_accessory.ele_yoke_tilt[1] = 120;
					if(p_sys_menu_model->ele_accessory.ele_yoke_tilt[1] <= -120)
						p_sys_menu_model->ele_accessory.ele_yoke_tilt[1] = -120;
				}
                yoke_info_creat(info_panel, p_sys_menu_model->ele_accessory.ele_yoke_pan[0], p_sys_menu_model->ele_accessory.ele_yoke_pan[1], &ImgPan, 69, (char *)Lang_GetStringByID(STRING_ID_PAN_LIMIT));
                yoke_info_creat(info_panel, p_sys_menu_model->ele_accessory.ele_yoke_tilt[0], p_sys_menu_model->ele_accessory.ele_yoke_tilt[1], &ImgTilt, 220, (char *)Lang_GetStringByID(STRING_ID_TILT_LIMIT));
      
                static lv_coord_t col_dsc[] = { 150, 150, LV_GRID_TEMPLATE_LAST };
                static lv_coord_t row_dsc[] = { 43, 43, LV_GRID_TEMPLATE_LAST };
                static lv_coord_t col2_dsc[] = { 265, LV_GRID_TEMPLATE_LAST };
                static lv_coord_t row2_dsc[] = { 43, LV_GRID_TEMPLATE_LAST };

                lv_obj_t* btn_panel = btn_panel_grid_creat(320, 116, col_dsc, row_dsc, 10);
                lv_obj_align_to(btn_panel, info_panel, LV_ALIGN_OUT_BOTTOM_MID, 0, 0);
                lv_obj_t* btn2_panel = btn_panel_grid_creat(320, 67, col2_dsc, row2_dsc, 28);
                lv_obj_align_to(btn2_panel, btn_panel, LV_ALIGN_OUT_BOTTOM_MID, 0, 0);

                uint8_t col;
                uint8_t row;
                lv_obj_t* label1;
                lv_obj_t* obj1;
				
				for (uint8_t i = 0; i < 4; i++)
				{
					col = i % 2;
					row = i / 2;

					obj1 = lv_obj_create(btn_panel);
					lv_obj_set_grid_cell(obj1, LV_GRID_ALIGN_STRETCH, col, 1, LV_GRID_ALIGN_STRETCH, row, 1);
					
					lv_obj_clear_flag(obj1, LV_OBJ_FLAG_SCROLLABLE);
					lv_obj_add_style(obj1, &style_menu_item, LV_STATE_DEFAULT);
					lv_obj_set_style_bg_color(obj1, lv_color_make(0x23, 0x23, 0x23), LV_STATE_DEFAULT);
					lv_obj_add_style(obj1, &style_menu_focused_item, LV_STATE_FOCUSED);
					lv_obj_set_style_bg_color(obj1, RED_THEME_COLOR, LV_STATE_FOCUSED);
					if(i <= 1)
						lv_obj_set_style_text_font(obj1, Font_ResouceGet(FONT_18_BOLD), LV_STATE_USER_1);
					else
						lv_obj_set_style_text_font(obj1, Font_ResouceGet(FONT_14_MEDIUM), LV_STATE_USER_1);
				
					str_dsc_t str_dsc;
					get_yoke_btn_str(i, &str_dsc);
					label1 = lv_label_create(obj1);
					if(i <= 1)
						lv_obj_set_style_text_font(label1, Font_ResouceGet(FONT_18), LV_STATE_DEFAULT);
					else
						lv_obj_set_style_text_font(label1, Font_ResouceGet(FONT_14_MEDIUM), LV_STATE_DEFAULT);
					lv_obj_set_style_text_color(obj1, lv_color_white(), 0);
					lv_obj_set_style_text_align(obj1, LV_TEXT_ALIGN_CENTER, 0);
					if (i <= 1)
					{
						data_center_read_sys_info(SYS_INFO_MOTOR, &motor_info);
						
						lv_obj_align(label1, LV_ALIGN_LEFT_MID, -10, 0);

						lv_obj_t* label2 = lv_label_create(obj1);
						lv_obj_align(label2, LV_ALIGN_CENTER, 0, 0);

						lv_obj_set_style_text_font(label2, Font_ResouceGet(FONT_16_BOLD), LV_STATE_USER_1);

						lv_obj_t* img_lock = lv_img_create(obj1);
						lv_obj_set_style_img_recolor(img_lock, lv_color_white(), 0);
						lv_obj_set_style_img_recolor_opa(img_lock, LV_OPA_100, 0);
						lv_obj_align(img_lock, LV_ALIGN_RIGHT_MID, 10, 0);
						
						lv_obj_add_event_cb(obj1, yoke_pan_event_cb, LV_EVENT_ALL, NULL);
						if (i == 0)
						{
							refresh_adj_value_label(label2, p_sys_menu_model->ele_accessory.ele_yoke_pan_val);
							refresh_lock_img(img_lock, p_sys_menu_model->ele_accessory.ele_yoke_pan_lock);  
						}
						else
						{
							refresh_adj_value_label(label2, p_sys_menu_model->ele_accessory.ele_yoke_tilt_val);
							refresh_lock_img(img_lock, p_sys_menu_model->ele_accessory.ele_yoke_tilt_lock); 
						}
					}
					else
					{
						lv_obj_center(label1); 
						lv_obj_add_event_cb(obj1, yoke_btn_event_cb, LV_EVENT_ALL, NULL);
					}
					lv_obj_set_style_text_line_space(label1, 2, 0);
					lv_obj_set_style_text_color(obj1, lv_color_white(), LV_STATE_FOCUSED);
					lv_label_set_text(label1, str_dsc.str);
					
					if(i == 0)
						lv_group_focus_obj(obj1);
					lv_group_add_obj(indev_group, obj1);
				}
                lv_group_set_wrap(indev_group, false);
                lv_group_set_editing(indev_group, false);
            }
            else 
            {
                uint8_t lang_type = 0;
                s_access_flag = 1;
                data_center_read_config_data(SYS_CONFIG_LANG, &lang_type);
                
                lv_obj_clear_flag(lv_scr_act(), LV_OBJ_FLAG_SCROLLABLE);
                lv_obj_t* label1_panel = lv_label_create(lv_scr_act());
                lv_obj_set_style_text_font(label1_panel, Font_ResouceGet(FONT_22_BOLD), 0);
				lv_label_set_text(label1_panel, Lang_GetStringByID(STRING_ID_PLEASE_SELECT_ACC_TYPE));
                lv_obj_set_style_text_color(label1_panel, lv_color_white(), 0);
                lv_obj_align(label1_panel, LV_ALIGN_TOP_MID, 0, 10);
                
                access_radiobox_ext.sel = screen_get_act_index();            
                access_radiobox_ext.max = 11;
                access_radiobox_ext.items_of_page = 4;            
                access_radiobox_ext.get_name_str = select_acc_name_get_str;
                access_radiobox_ext.release_cb = select_an_acc_choose_list_user_event;

                lv_obj_t* radiobox_panel = my_long_list_create(lv_scr_act(), 265, &access_radiobox_ext);   
                lv_obj_align(radiobox_panel, LV_ALIGN_TOP_MID, 0, 50);
                
                lv_obj_t *child1 = lv_obj_get_child(lv_scr_act(), 1);
                for(uint8_t i = 0; i < access_radiobox_ext.max; i++)
                {
                    lv_obj_t *child2 = lv_obj_get_child(child1, i);
                    lv_obj_t *img = lv_obj_get_child(child2, 2);
					if(i != 7)
					{
						if(p_sys_menu_model->ele_accessory.ele_no_access_sel == i)
							lv_img_set_src(img, &ImgRadioboxSel);
						else
							lv_img_set_src(img, &ImgRadioboxNoSel); 
					}					
                }
                
                access_scroll_cont = my_list_scroll_create(lv_scr_act(), 3, 190);                        
                lv_obj_align_to(access_scroll_cont, radiobox_panel, LV_ALIGN_OUT_RIGHT_TOP, 21, 4);
                
                lv_group_add_obj(indev_group, radiobox_panel);
                lv_group_focus_obj(radiobox_panel);      
                lv_group_set_editing(indev_group, true);
            }
        }
        break;
        case SUB_ID_ELE_YOKE_BLOCK_CONFIRM:
        {
            my_msgbox_reset();
            my_msgbox_ext_t* p_msgbox_ext = my_msgbox_get_ext();
            p_msgbox_ext->body_comment_str = Lang_GetStringByID(STRING_ID_YOKE_BLOCK_CONFIRM);
            p_msgbox_ext->body_img = NULL;
            p_msgbox_ext->timecnt = 0;
            p_msgbox_ext->timeout = 30 * 1000;
            p_msgbox_ext->answer = MSGBOX_ID_NO_CONFIRM;
            p_msgbox_ext->type = MSGBOX_TYPE_TIMEOUT;
            p_msgbox_ext->btn_sel = MSGBOX_ID_NO;
            p_msgbox_ext->title_str = NULL;
            p_msgbox_ext->msgbox_del_cb = block_cal_event_cb;
            lv_obj_t* msgbox_panel = my_msgbox3_create(p_msgbox_ext);

            lv_obj_t* comment = lv_obj_get_child(msgbox_panel, 1);
            lv_obj_align(comment, LV_ALIGN_TOP_MID, 0, 10);
            lv_obj_t* btn_cont = lv_obj_get_child(msgbox_panel, 2);
            lv_obj_align_to(btn_cont, comment, LV_ALIGN_OUT_BOTTOM_MID, 0, -30);

            lv_obj_t* commentII = lv_label_create(msgbox_panel);
            lv_obj_set_style_text_font(commentII, Font_ResouceGet(FONT_14_Heavy), 0);
            lv_obj_set_style_text_color(commentII, lv_color_make(0xc4, 0x38, 0x2b), 0);
            lv_obj_set_width(commentII, 230);  /*Set smaller width to make the lines wrap*/
            lv_label_set_long_mode(commentII, LV_LABEL_LONG_WRAP);
            lv_obj_set_style_text_align(commentII, LV_TEXT_ALIGN_CENTER, 0);
            lv_label_set_text(commentII, Lang_GetStringByID(STRING_ID_YOKE_BLOCK_NOTE_CONFIRM));
            lv_obj_align_to(commentII, btn_cont, LV_ALIGN_OUT_BOTTOM_MID, 0, 0);
        }
        break;
        case SUB_ID_ELE_YOKE_RESET_CONFIRM:
        {
            my_msgbox_reset();
            my_msgbox_ext_t* p_msgbox_ext = my_msgbox_get_ext();
            p_msgbox_ext->body_comment_str = Lang_GetStringByID(STRING_ID_YOKE_ORIGIN_RST_CONFIRM);;
            p_msgbox_ext->body_img = NULL;
            p_msgbox_ext->timecnt = 0;
            p_msgbox_ext->timeout = 30 * 1000;
            p_msgbox_ext->answer = MSGBOX_ID_NO_CONFIRM;
            p_msgbox_ext->type = MSGBOX_TYPE_TIMEOUT;
            p_msgbox_ext->btn_sel = MSGBOX_ID_NO;
            p_msgbox_ext->title_str = NULL;
            p_msgbox_ext->msgbox_del_cb = origin_rst_event_cb;
            lv_obj_t* msgbox_panel = my_msgbox3_create(p_msgbox_ext);
			
			lv_obj_t* comment = lv_obj_get_child(msgbox_panel, 1);
            lv_obj_align(comment, LV_ALIGN_TOP_MID, 0, 10);
            lv_obj_t* btn_cont = lv_obj_get_child(msgbox_panel, 2);
            lv_obj_align_to(btn_cont, comment, LV_ALIGN_OUT_BOTTOM_MID, 0, -30);

            lv_obj_t* commentII = lv_label_create(msgbox_panel);
            lv_obj_set_style_text_font(commentII, Font_ResouceGet(FONT_14_Heavy), 0);
            lv_obj_set_style_text_color(commentII, lv_color_make(0xc4, 0x38, 0x2b), 0);
            lv_obj_set_width(commentII, 230);  /*Set smaller width to make the lines wrap*/
            lv_label_set_long_mode(commentII, LV_LABEL_LONG_WRAP);
            lv_obj_set_style_text_align(commentII, LV_TEXT_ALIGN_CENTER, 0);
            lv_label_set_text(commentII, Lang_GetStringByID(STRING_ID_YOKE_BLOCK_NOTE_CONFIRM));
            lv_obj_align_to(commentII, btn_cont, LV_ALIGN_OUT_BOTTOM_MID, 0, 0);
        }
        break;
        case SUB_ID_ELE_YOKE_ANGLE_LIMIT:
        {
			limit_on_value = p_sys_menu_model->ele_accessory.ele_yoke_angle_limit_on1[p_sys_menu_model->ele_accessory.ele_no_access_sel];
			
            lv_obj_set_style_bg_color(lv_scr_act(), lv_color_black(), 0);
            lv_obj_t* btn_plate = lv_obj_create(lv_scr_act());
            lv_obj_remove_style_all(btn_plate);
            lv_obj_add_style(btn_plate, &style_rect_panel, 0);
            lv_obj_set_flex_flow(btn_plate, LV_FLEX_FLOW_COLUMN);
            lv_obj_set_style_pad_row(btn_plate, 9, 0);
            lv_obj_set_style_bg_color(btn_plate, lv_color_make(0, 0, 0), 0);
            lv_obj_set_size(btn_plate, 190, 210);
            lv_obj_align(btn_plate, LV_ALIGN_TOP_RIGHT, -16, 13);

            lv_obj_t* label1_panel = lv_label_create(lv_scr_act());
            lv_obj_set_style_text_font(label1_panel, Font_ResouceGet(FONT_14), 0);
            lv_obj_set_style_text_color(label1_panel, lv_color_white(), 0);
            lv_label_set_text(label1_panel, Lang_GetStringByID(STRING_ID_MOTION_LIMIT));
            lv_obj_align(label1_panel, LV_ALIGN_TOP_LEFT, 15, 26);

            lv_obj_t* btn_panel = lv_obj_create(btn_plate);
            lv_obj_add_style(btn_panel, &style_rect_panel, 0);
            lv_obj_add_style(btn_panel, &style_common_focued_btn, LV_STATE_FOCUSED);
            lv_obj_set_size(btn_panel, 190, 50);
            lv_obj_add_event_cb(btn_panel, angle_onoff_event_cb, LV_EVENT_ALL, NULL);
            lv_group_add_obj(indev_group, btn_panel);

            img_onoff = lv_img_create(btn_panel);
            lv_obj_align(img_onoff, LV_ALIGN_CENTER, 0, 0);
            refresh_yoke_disp();
			
			if(s_access_info.yoke_state == 1 && (s_access_info.fresnel_state != 1 && s_access_info.optical_20_access_state != 1 && s_access_info.optical_30_access_state != 1 &&
                    s_access_info.optical_50_access_state != 1))
			{
				btn_ext[0].buff = &liti_value;
				btn_ext[1].buff = &pan_value;
			}
			else
			{
				btn_ext[0].buff = &p_sys_menu_model->ele_accessory.ele_yoke_tilt[0];
				btn_ext[1].buff = &p_sys_menu_model->ele_accessory.ele_yoke_pan[0];
			}
            lv_obj_t* btn1 = limit_btn_creat(btn_plate, Lang_GetStringByID(STRING_ID_TILT_LIMIT), 15, 85, &btn_ext[0]);
            lv_group_add_obj(indev_group, btn1);

            lv_obj_t* btn2 = limit_btn_creat(btn_plate, Lang_GetStringByID(STRING_ID_PAN_LIMIT), 15, 144, &btn_ext[1]);
            lv_group_add_obj(indev_group, btn2);

            lv_obj_t* con_btn = lv_obj_create(lv_scr_act());
            lv_obj_add_style(con_btn, &style_rect_panel, 0);
            lv_obj_add_style(con_btn, &style_common_focued_btn, LV_STATE_FOCUSED);
            lv_obj_set_size(con_btn, 95, 34);
            lv_obj_align_to(con_btn, lv_scr_act(), LV_ALIGN_BOTTOM_LEFT, 50, -5);
            lv_obj_add_event_cb(con_btn, limit_confirm_event_cb, LV_EVENT_ALL, NULL);
            lv_group_add_obj(indev_group, con_btn);
    
            lv_obj_t* con_label = lv_label_create(con_btn);
            lv_label_set_text(con_label, Lang_GetStringByID(STRING_ID_YOKE_RESET_YES));
            lv_obj_set_style_text_font(con_label, Font_ResouceGet(FONT_18), 0);
            lv_obj_set_style_text_color(con_label, lv_color_white(), 0);
            lv_obj_center(con_label);
            
            lv_obj_t* res_btn = lv_obj_create(lv_scr_act());
            lv_obj_add_style(res_btn, &style_rect_panel, 0);
            lv_obj_add_style(res_btn, &style_common_focued_btn, LV_STATE_FOCUSED);
            lv_obj_set_size(res_btn, 95, 34);
            lv_obj_align_to(res_btn, lv_scr_act(), LV_ALIGN_BOTTOM_RIGHT, -50, -5);
            lv_obj_add_event_cb(res_btn, limit_res_event_cb, LV_EVENT_ALL, NULL);
            lv_group_add_obj(indev_group, res_btn);

            lv_obj_t* res_label = lv_label_create(res_btn);
            lv_label_set_text(res_label, Lang_GetStringByID(STRING_ID_YOKE_RESET));
            lv_obj_set_style_text_font(res_label, Font_ResouceGet(FONT_18), 0);
            lv_obj_set_style_text_color(res_label, lv_color_white(), 0);
            lv_obj_center(res_label);
            
            lv_group_focus_obj(btn_panel);
            lv_group_set_wrap(indev_group, false);
            lv_group_set_editing(indev_group, false);
        }
        break;
        case SUN_ID_ELE_YOKE_LIMIT_CONFIRM:
        {
            my_msgbox_reset();
            my_msgbox_ext_t* p_msgbox_ext = my_msgbox_get_ext();
            p_msgbox_ext->body_comment_str = Lang_GetStringByID(STRING_ID_YOKE_RESET_CONFIRM);;
            p_msgbox_ext->body_img = NULL;
            p_msgbox_ext->timecnt = 0;
            p_msgbox_ext->timeout = 30 * 1000;
            p_msgbox_ext->answer = MSGBOX_ID_NO_CONFIRM;
            p_msgbox_ext->type = MSGBOX_TYPE_TIMEOUT;
            p_msgbox_ext->btn_sel = MSGBOX_ID_NO;
            p_msgbox_ext->title_str = NULL;
            p_msgbox_ext->msgbox_del_cb = angle_reset_event_cb;
            my_msgbox3_create(p_msgbox_ext);
        }
        break;
        case SUB_ID_ELE_OPTICAL_ACCESS:
        {
			access_spid = 2;
			
            uint8_t lang_type = 0;
			memcpy(&s_access_info_last, &s_access_info, sizeof(struct sys_info_accessories));
			
            lv_obj_set_style_bg_color(lv_scr_act(), lv_color_black(), 0);
        
            lv_obj_t* label1_panel = lv_label_create(lv_scr_act());
            lv_obj_set_style_text_font(label1_panel, Font_ResouceGet(FONT_22_BOLD), 0);
            lv_obj_set_style_text_color(label1_panel, lv_color_white(), 0);
            lv_label_set_text(label1_panel, Lang_GetStringByID(STRING_ID_OPTICAL_ACCESSORIES));
            lv_obj_align(label1_panel, LV_ALIGN_TOP_MID, 0, 5);
            
            lv_obj_t* btn_plate = lv_obj_create(lv_scr_act());
            lv_obj_add_style(btn_plate, &style_common_btn, 0);
            lv_obj_set_style_radius(btn_plate, 0, 0);
            lv_obj_set_size(btn_plate, 265, 159);
            lv_obj_align(btn_plate, LV_ALIGN_CENTER, 0, 10);
            
            data_center_read_config_data(SYS_CONFIG_LANG, &lang_type);
            if(s_access_info.optical_20_access_state == 0 && s_access_info.optical_30_access_state == 0 && 
			   s_access_info.optical_50_access_state == 0 && s_access_info.adapter_ring == 0)
            {
                lv_obj_t* label_panel = lv_label_create(btn_plate);
                lv_obj_set_style_text_font(label_panel, Font_ResouceGet(FONT_16_BOLD), 0);
                lv_obj_set_style_text_color(label_panel, lv_color_white(), 0);
				lv_label_set_text(label_panel, Lang_GetStringByID(STRING_ID_NO_REFLECTOR));
                lv_obj_align(label_panel, LV_ALIGN_CENTER, 0, 5);
            }
            else if(s_access_info.optical_20_access_state == 1)
            {
				LV_IMG_DECLARE(Img20ReflectorName);
				lv_obj_t* img = lv_img_create(btn_plate);
				lv_img_set_src(img, &Img20ReflectorName); 
				lv_obj_align(img, LV_ALIGN_TOP_MID, 0, 10);
				
                LV_IMG_DECLARE(Img20Reflector2);
				LV_IMG_DECLARE(img20Reflector);
                lv_obj_t* img1 = lv_img_create(btn_plate);
				lv_img_set_src(img1, &img20Reflector);  
                lv_img_set_src(img1, &Img20Reflector2);  
                lv_obj_align(img1, LV_ALIGN_BOTTOM_MID, 0, -15);  
            }
            else if(s_access_info.optical_30_access_state == 1)
            {
				LV_IMG_DECLARE(Img30ReflectorName);
				lv_obj_t* img = lv_img_create(btn_plate);
				lv_img_set_src(img, &Img30ReflectorName); 
				lv_obj_align(img, LV_ALIGN_TOP_MID, 0, 10);
				
                LV_IMG_DECLARE(Img30Reflector2);
				LV_IMG_DECLARE(img30Reflector);
                lv_obj_t* img1 = lv_img_create(btn_plate);
				lv_img_set_src(img1, &img30Reflector);
                lv_img_set_src(img1, &Img30Reflector2);    
                lv_obj_align(img1, LV_ALIGN_BOTTOM_MID, 0, -15);  
            }
            else if(s_access_info.optical_50_access_state == 1)
            {
				LV_IMG_DECLARE(Img50ReflectorName);
				lv_obj_t* img = lv_img_create(btn_plate);
				lv_img_set_src(img, &Img50ReflectorName); 
				lv_obj_align(img, LV_ALIGN_TOP_MID, 0, 10);
				
                LV_IMG_DECLARE(Img50Reflector2);
				LV_IMG_DECLARE(img50Reflector);
                lv_obj_t* img1 = lv_img_create(btn_plate);
				lv_img_set_src(img1, &img50Reflector); 
                lv_img_set_src(img1, &Img50Reflector2);   
				
                lv_obj_align(img1, LV_ALIGN_BOTTOM_MID, 0, -15);  
            }
			else if(s_access_info.adapter_ring == 1)
            {
				lv_obj_t* label_panel = lv_label_create(btn_plate);
                lv_obj_set_style_text_font(label_panel, Font_ResouceGet(FONT_18_BOLD), 0);
                lv_obj_set_style_text_color(label_panel, lv_color_white(), 0);
				lv_label_set_text(label_panel, Lang_GetStringByID(STRING_ID_ADAPTORRING));
				lv_obj_align(label_panel, LV_ALIGN_TOP_MID, 0, 10);
				
				LV_IMG_DECLARE(imgAdaptorRing);
                lv_obj_t* img1 = lv_img_create(btn_plate);
				lv_img_set_src(img1, &imgAdaptorRing);   
				
                lv_obj_align(img1, LV_ALIGN_BOTTOM_MID, 0, -15);  
            }
            lv_obj_add_event_cb(btn_plate, access_btn_eent_cb, LV_EVENT_ALL, NULL);
            lv_group_add_obj(indev_group, btn_plate);
        
            lv_group_focus_obj(btn_plate);
            lv_group_set_wrap(indev_group, false);
            lv_group_set_editing(indev_group, false);
        }
        break;
        case SUB_ID_ELE_NO_ACCESS_WRING:
        {
            uint8_t lang_type = 0;
                
            lv_obj_set_style_bg_color(lv_scr_act(), lv_color_black(), 0);
            data_center_read_config_data(SYS_CONFIG_LANG, &lang_type);
            lv_obj_t* label_panel = lv_label_create(lv_scr_act());
            lv_obj_set_style_text_font(label_panel, Font_ResouceGet(FONT_22_BOLD), 0);
            lv_obj_set_style_text_color(label_panel, lv_color_white(), 0);
			lv_label_set_text(label_panel, Lang_GetStringByID(STRING_ID_USE_NO_APUTURE_ACC));
            lv_obj_align(label_panel, LV_ALIGN_TOP_MID, 0, 5);
            
            lv_obj_t* label_pane2 = lv_label_create(lv_scr_act());
            lv_obj_set_style_text_font(label_pane2, Font_ResouceGet(FONT_14), 0);
            lv_obj_set_style_text_color(label_pane2, lv_color_white(), 0);
			lv_label_set_text(label_pane2, Lang_GetStringByID(STRING_ID_USE_NO_APUTRUE_ACC_WRING));
            lv_obj_align(label_pane2, LV_ALIGN_CENTER, 0, -10);
            
            lv_obj_t* con_btn = lv_obj_create(lv_scr_act());
            lv_obj_add_style(con_btn, &style_rect_panel, 0);
            lv_obj_add_style(con_btn, &style_common_focued_btn, LV_STATE_FOCUSED);
            lv_obj_set_size(con_btn, 75, 40);
            lv_obj_align_to(con_btn, lv_scr_act(), LV_ALIGN_BOTTOM_LEFT, 60, -15);
            lv_obj_add_event_cb(con_btn, access_wing_yes_event_cb, LV_EVENT_ALL, NULL);
            lv_group_add_obj(indev_group, con_btn);
            
            lv_obj_t* con_label = lv_label_create(con_btn);
			lv_obj_set_style_text_font(con_label, Font_ResouceGet(FONT_18), 0);
			lv_label_set_text(con_label, Lang_GetStringByID(STRING_ID_YES));
            lv_obj_set_style_text_color(con_label, lv_color_white(), 0);
            lv_obj_center(con_label);
            
            lv_obj_t* res_btn = lv_obj_create(lv_scr_act());
            lv_obj_add_style(res_btn, &style_rect_panel, 0);
            lv_obj_add_style(res_btn, &style_common_focued_btn, LV_STATE_FOCUSED);
            lv_obj_set_size(res_btn, 75, 40);
            lv_obj_align_to(res_btn, lv_scr_act(), LV_ALIGN_BOTTOM_RIGHT, -60, -15);
            lv_obj_add_event_cb(res_btn, access_wing_no_event_cb, LV_EVENT_ALL, NULL);
            lv_group_add_obj(indev_group, res_btn);

            lv_obj_t* res_label = lv_label_create(res_btn);
			lv_label_set_text(res_label, Lang_GetStringByID(STRING_ID_NO));
            lv_obj_set_style_text_font(res_label, Font_ResouceGet(FONT_18), 0);
            lv_obj_set_style_text_color(res_label, lv_color_white(), 0);
            lv_obj_center(res_label);
        }
        break;
        case SUB_ID_ELE_NO_ACCESS_WRING1:
        {
            lv_obj_set_style_bg_color(lv_scr_act(), lv_color_black(), 0);
            lv_obj_t* label_panel = lv_label_create(lv_scr_act());
            lv_obj_set_style_text_font(label_panel, Font_ResouceGet(FONT_22_BOLD), 0);
			lv_label_set_text(label_panel, Lang_GetStringByID(STRING_ID_USE_NO_APUTURE_ACC));
            lv_obj_set_style_text_color(label_panel, lv_color_white(), 0);
            lv_obj_align(label_panel, LV_ALIGN_TOP_MID, 0, 5);
            
			lv_obj_t* textarea = lv_obj_create(lv_scr_act());
			lv_obj_remove_style_all(textarea);
			lv_obj_set_style_text_color(textarea, lv_color_white(), 0);
			lv_obj_set_style_text_line_space(textarea, 0, 0);
			lv_obj_set_style_text_font(textarea, Font_ResouceGet(FONT_14), 0);
			lv_obj_set_size(textarea, 280,97);
			lv_obj_align(textarea, LV_ALIGN_CENTER, 0, 0);
			
            lv_obj_t* label_pane2 = lv_label_create(textarea);
			lv_obj_set_size(label_pane2, 260, LV_SIZE_CONTENT);
			lv_obj_set_style_text_align(label_pane2, LV_TEXT_ALIGN_CENTER, 0);
			lv_label_set_long_mode(label_pane2, LV_LABEL_LONG_WRAP); 
			lv_label_set_text(label_pane2, Lang_GetStringByID(STRING_ID_USE_NO_APUTRUE_ACC_ANGLE_TILT));
            lv_obj_align(label_pane2, LV_ALIGN_CENTER, 0, 0);
			
            lv_obj_t* con_btn = lv_obj_create(lv_scr_act());
            lv_obj_add_style(con_btn, &style_rect_panel, 0);
            lv_obj_add_style(con_btn, &style_common_focued_btn, LV_STATE_FOCUSED);
            lv_obj_set_size(con_btn, 75, 40);
            lv_obj_align_to(con_btn, lv_scr_act(), LV_ALIGN_BOTTOM_MID, 0, -15);
            lv_obj_add_event_cb(con_btn, access_wing_yes_event_cb, LV_EVENT_ALL, NULL);
            lv_group_add_obj(indev_group, con_btn);
            
            lv_obj_t* con_label = lv_label_create(con_btn);
			lv_obj_set_style_text_font(con_label, Font_ResouceGet(FONT_18), 0);
			lv_label_set_text(con_label, Lang_GetStringByID(STRING_ID_YES));
            
            lv_obj_set_style_text_color(con_label, lv_color_white(), 0);
            lv_obj_center(con_label);
            
        }
        break;
        case SUB_ID_ELE_NO_YOKE_ANGLE_LIMIT:
        {
			limit_on_value = 1;
            lv_obj_set_style_bg_color(lv_scr_act(), lv_color_black(), 0);
            lv_obj_t* btn_plate = lv_obj_create(lv_scr_act());
            lv_obj_remove_style_all(btn_plate);
            lv_obj_add_style(btn_plate, &style_rect_panel, 0);
            lv_obj_set_flex_flow(btn_plate, LV_FLEX_FLOW_COLUMN);
            lv_obj_set_style_pad_row(btn_plate, 9, 0);
            lv_obj_set_style_bg_color(btn_plate, lv_color_make(0, 0, 0), 0);
            lv_obj_set_size(btn_plate, 190, 210);
            lv_obj_align(btn_plate, LV_ALIGN_TOP_RIGHT, -16, 13);

            lv_obj_t* label1_panel = lv_label_create(lv_scr_act());
            lv_obj_set_style_text_font(label1_panel, Font_ResouceGet(FONT_14), 0);
            lv_obj_set_style_text_color(label1_panel, lv_color_white(), 0);
            lv_label_set_text(label1_panel, Lang_GetStringByID(STRING_ID_MOTION_LIMIT));
            lv_obj_align(label1_panel, LV_ALIGN_TOP_LEFT, 15, 26);

            lv_obj_t* btn_panel = lv_obj_create(btn_plate);
            lv_obj_add_style(btn_panel, &style_rect_panel, 0);
            lv_obj_add_style(btn_panel, &style_common_focued_btn, LV_STATE_FOCUSED);
            lv_obj_set_size(btn_panel, 190, 50);
            lv_obj_add_event_cb(btn_panel, angle_onoff_event_cb, LV_EVENT_ALL, NULL);
            lv_group_add_obj(indev_group, btn_panel);

            img_onoff = lv_img_create(btn_panel);
            lv_obj_align(img_onoff, LV_ALIGN_CENTER, 0, 0);
            refresh_yoke_disp();
            btn_ext[0].buff = &liti_value;
            btn_ext[1].buff = &pan_value;
            lv_obj_t* btn1 = limit_btn_creat(btn_plate, Lang_GetStringByID(STRING_ID_TILT_LIMIT), 15, 85, &btn_ext[0]);
            lv_group_add_obj(indev_group, btn1);

            lv_obj_t* btn2 = limit_btn_creat(btn_plate, Lang_GetStringByID(STRING_ID_PAN_LIMIT), 15, 144, &btn_ext[1]);
            lv_group_add_obj(indev_group, btn2);

            lv_obj_t* con_btn = lv_obj_create(lv_scr_act());
            lv_obj_add_style(con_btn, &style_rect_panel, 0);
            lv_obj_add_style(con_btn, &style_common_focued_btn, LV_STATE_FOCUSED);
            lv_obj_set_size(con_btn, 95, 34);
            lv_obj_align_to(con_btn, lv_scr_act(), LV_ALIGN_BOTTOM_MID, 20, 0);
            lv_obj_add_event_cb(con_btn, limit_confirm_event_cb, LV_EVENT_ALL, NULL);
            lv_group_add_obj(indev_group, con_btn);
    
            lv_obj_t* con_label = lv_label_create(con_btn);
            lv_label_set_text(con_label, Lang_GetStringByID(STRING_ID_YOKE_RESET_YES));
            lv_obj_set_style_text_font(con_label, Font_ResouceGet(FONT_18), 0);
            lv_obj_set_style_text_color(con_label, lv_color_white(), 0);
            lv_obj_center(con_label);
            
            lv_group_focus_obj(btn_panel);
            lv_group_set_wrap(indev_group, false);
            lv_group_set_editing(indev_group, false);
        }
        break;
        case SUB_ID_ELE_NO_YOKE_ANGLE_LIMIT_ENTER:
        {
            lv_obj_set_style_bg_color(lv_scr_act(), lv_color_black(), 0);
            lv_obj_t* label_panel = lv_label_create(lv_scr_act());
            lv_obj_set_style_text_font(label_panel, Font_ResouceGet(FONT_22_BOLD), 0);
            lv_obj_set_style_text_color(label_panel, lv_color_white(), 0);
			lv_label_set_text(label_panel, Lang_GetStringByID(STRING_ID_PLEASE_SAVE_PATH));
            lv_obj_align(label_panel, LV_ALIGN_TOP_MID, 0, 5);
            
            access_radiobox_ext1.sel = 0;
            access_radiobox_ext1.max = 3;
            access_radiobox_ext1.items_of_page = 3;
            access_radiobox_ext1.get_name_str = select_acc_name_get_str1;
            access_radiobox_ext1.release_cb = select_an_acc_choose_list_user_event;    
            
            lv_obj_t* radiobox_panel = my_long_list_create(lv_scr_act(), 265, &access_radiobox_ext1);   
            lv_obj_align(radiobox_panel, LV_ALIGN_TOP_MID, 0, 50);
            
            lv_obj_t *child1 = lv_obj_get_child(lv_scr_act(), 1);
            for(uint8_t i = 0; i < access_radiobox_ext1.max; i++)
            {
                lv_obj_t *child2 = lv_obj_get_child(child1, i);
                lv_obj_t *img = lv_obj_get_child(child2, 2);
                if(0 == i)
                    lv_img_set_src(img, &ImgRadioboxSel);
                else
                    lv_img_set_src(img, &ImgRadioboxNoSel);   
            }
            lv_group_add_obj(indev_group, radiobox_panel);
            lv_group_focus_obj(radiobox_panel);      
            lv_group_set_editing(indev_group, true);
            lv_group_set_wrap(indev_group, false);
        }
        break;
        case SUB_ID_ELE_NO_YOKE:
        {
            int16_t pan_upper = ANGLE_LIMIT_MAX;
            int16_t pan_lower = ANGLE_LIMIT_MIN;
            int16_t tilit_upper = ANGLE_LIMIT_DEFAULT_MAX;
            int16_t tilit_lower = ANGLE_LIMIT_DEFAULT_MIN;
			
			memcpy(&s_access_info_last, &s_access_info, sizeof(struct sys_info_accessories));
            lv_obj_set_style_bg_color(lv_scr_act(), lv_color_black(), 0);
            lv_obj_t* info_panel = lv_obj_create(lv_scr_act());
			lv_obj_remove_style_all(info_panel);
			lv_obj_set_size(info_panel, 320, 110);
			lv_obj_set_style_bg_color(info_panel, lv_color_make(0, 0, 0), 0);
			lv_obj_set_style_text_font(info_panel, Font_ResouceGet(FONT_14), 0);
			lv_obj_set_style_text_color(info_panel, lv_color_white(), 0);
			lv_obj_align(info_panel, LV_ALIGN_TOP_MID, 0, 0);

            LV_IMG_DECLARE(ImgPan)
            LV_IMG_DECLARE(ImgTilt)
            
			uint8_t tiltm[12];
			data_center_read_config_data(SYS_CONFIG_NO_ANGLE_LIMIT, tiltm);
			if(tiltm[p_sys_menu_model->ele_accessory.ele_no_access_sel] != 1)   //判断角度限制是否开启
			{
				p_sys_menu_model->ele_accessory.ele_yoke_pan1[p_sys_menu_model->ele_accessory.ele_no_access_sel][1] = ANGLE_LIMIT_MAX;
				p_sys_menu_model->ele_accessory.ele_yoke_pan1[p_sys_menu_model->ele_accessory.ele_no_access_sel][0] = ANGLE_LIMIT_MIN;
				p_sys_menu_model->ele_accessory.ele_yoke_tilt1[p_sys_menu_model->ele_accessory.ele_no_access_sel][1] = ANGLE_LIMIT_DEFAULT_MAX;
				p_sys_menu_model->ele_accessory.ele_yoke_tilt1[p_sys_menu_model->ele_accessory.ele_no_access_sel][0] = ANGLE_LIMIT_DEFAULT_MIN;
			}
			
			pan_upper = p_sys_menu_model->ele_accessory.ele_yoke_pan1[p_sys_menu_model->ele_accessory.ele_no_access_sel][1];
            pan_lower = p_sys_menu_model->ele_accessory.ele_yoke_pan1[p_sys_menu_model->ele_accessory.ele_no_access_sel][0];
            tilit_upper = p_sys_menu_model->ele_accessory.ele_yoke_tilt1[p_sys_menu_model->ele_accessory.ele_no_access_sel][1];
            tilit_lower = p_sys_menu_model->ele_accessory.ele_yoke_tilt1[p_sys_menu_model->ele_accessory.ele_no_access_sel][0];
			
			if(tiltm[p_sys_menu_model->ele_accessory.ele_no_access_sel] == 1)
			{
				if(p_sys_menu_model->ele_accessory.ele_yoke_pan_val < pan_lower)
					p_sys_menu_model->ele_accessory.ele_yoke_pan_val = pan_lower;
				if(p_sys_menu_model->ele_accessory.ele_yoke_pan_val > pan_upper)
					p_sys_menu_model->ele_accessory.ele_yoke_pan_val = pan_upper;
				if(p_sys_menu_model->ele_accessory.ele_yoke_tilt_val < tilit_lower)
				{
					p_sys_menu_model->ele_accessory.ele_yoke_tilt_val = tilit_lower;
					data_center_read_sys_info(SYS_INFO_MOTOR, &motor_info);
					motor_info.tilt_angle = p_sys_menu_model->ele_accessory.ele_yoke_tilt_val;
                    data_center_write_sys_info(SYS_INFO_MOTOR, &motor_info);
				}
				if(p_sys_menu_model->ele_accessory.ele_yoke_tilt_val > tilit_upper)
				{
					p_sys_menu_model->ele_accessory.ele_yoke_tilt_val = tilit_upper;
					data_center_read_sys_info(SYS_INFO_MOTOR, &motor_info);
					motor_info.tilt_angle = p_sys_menu_model->ele_accessory.ele_yoke_tilt_val;
                    data_center_write_sys_info(SYS_INFO_MOTOR, &motor_info);
				}
			}
			
            yoke_info_creat(info_panel, pan_lower, pan_upper, &ImgPan, 69, (char *)Lang_GetStringByID(STRING_ID_PAN_LIMIT));
            yoke_info_creat(info_panel, tilit_lower, tilit_upper, &ImgTilt, 220, (char *)Lang_GetStringByID(STRING_ID_TILT_LIMIT));
  
            static lv_coord_t col_dsc[] = { 150, 150, LV_GRID_TEMPLATE_LAST };
            static lv_coord_t row_dsc[] = { 43, 43, LV_GRID_TEMPLATE_LAST };

            lv_obj_t* btn_panel = btn_panel_grid_creat(320, 116, col_dsc, row_dsc, 10);
            lv_obj_align_to(btn_panel, info_panel, LV_ALIGN_OUT_BOTTOM_MID, 0, 0);

            uint8_t col;
            uint8_t row;
            lv_obj_t* label1;
            lv_obj_t* obj1;

            for (uint8_t i = 0; i < 2; i++)
            {
                col = i % 2;
                row = i / 2;

                obj1 = lv_obj_create(btn_panel);
                lv_obj_set_grid_cell(obj1, LV_GRID_ALIGN_STRETCH, col, 1, LV_GRID_ALIGN_STRETCH, row, 1);

                lv_obj_clear_flag(obj1, LV_OBJ_FLAG_SCROLLABLE);
                lv_obj_add_style(obj1, &style_menu_item, LV_STATE_DEFAULT);
                lv_obj_set_style_bg_color(obj1, lv_color_make(0x23, 0x23, 0x23), LV_STATE_DEFAULT);
                lv_obj_add_style(obj1, &style_menu_focused_item, LV_STATE_FOCUSED);
                lv_obj_set_style_bg_color(obj1, RED_THEME_COLOR, LV_STATE_FOCUSED);
				lv_obj_set_style_text_font(obj1, Font_ResouceGet(FONT_18_BOLD), LV_STATE_USER_1);

                str_dsc_t str_dsc;
                get_yoke_btn_str(i, &str_dsc);
                label1 = lv_label_create(obj1);
                lv_obj_set_style_text_color(obj1, lv_color_white(), 0);
                lv_obj_set_style_text_align(obj1, LV_TEXT_ALIGN_CENTER, 0);
                if (i <= 1)
                {
                    data_center_read_sys_info(SYS_INFO_MOTOR, &motor_info);
                    
                    lv_obj_align(label1, LV_ALIGN_LEFT_MID, -10, 0);

                    lv_obj_t* label2 = lv_label_create(obj1);
                    lv_obj_align(label2, LV_ALIGN_CENTER, 0, 0);

                    lv_obj_set_style_text_font(label2, Font_ResouceGet(FONT_16_BOLD), LV_STATE_USER_1);

                    lv_obj_t* img_lock = lv_img_create(obj1);
                    lv_obj_set_style_img_recolor(img_lock, lv_color_white(), 0);
                    lv_obj_set_style_img_recolor_opa(img_lock, LV_OPA_100, 0);
                    lv_obj_align(img_lock, LV_ALIGN_RIGHT_MID, 10, 0);
                    
                    lv_obj_add_event_cb(obj1, yoke_pan_event_cb, LV_EVENT_ALL, NULL);
                    if (i == 0)
                    {
                        refresh_adj_value_label(label2, p_sys_menu_model->ele_accessory.ele_yoke_pan_val);
                        refresh_lock_img(img_lock, p_sys_menu_model->ele_accessory.ele_yoke_pan_lock);  
                    }
                    else
                    {
                        refresh_adj_value_label(label2, p_sys_menu_model->ele_accessory.ele_yoke_tilt_val);
                        refresh_lock_img(img_lock, p_sys_menu_model->ele_accessory.ele_yoke_tilt_lock); 
                    }
                }
                else
                {
                    lv_obj_center(label1); 
                    lv_obj_add_event_cb(obj1, yoke_btn_event_cb, LV_EVENT_ALL, NULL);
                }
                lv_obj_set_style_text_line_space(label1, 2, 0);
                lv_obj_set_style_text_color(obj1, lv_color_white(), LV_STATE_FOCUSED);
				lv_label_set_text(label1, str_dsc.str);
                
                if(i == 0)
                    lv_group_focus_obj(obj1);
                lv_group_add_obj(indev_group, obj1);
            }
			
            lv_group_set_wrap(indev_group, false);
            lv_group_set_editing(indev_group, false);
        }
        break;
		case SUB_ID_ELE_APTURE_NO_YOKE:
		{
			motor_reset_flag = 2;
			int16_t pan_upper = ANGLE_LIMIT_MAX;
            int16_t pan_lower = ANGLE_LIMIT_MIN;
            int16_t tilit_upper = ANGLE_LIMIT_DEFAULT_MAX;
            int16_t tilit_lower = ANGLE_LIMIT_DEFAULT_MIN;
			
            memcpy(&s_access_info_last, &s_access_info, sizeof(struct sys_info_accessories));
			
            lv_obj_set_style_bg_color(lv_scr_act(), lv_color_black(), 0);
		
			lv_obj_t* info_panel = lv_obj_create(lv_scr_act());
			lv_obj_remove_style_all(info_panel);
			lv_obj_set_size(info_panel, 320, 110);
			lv_obj_set_style_bg_color(info_panel, lv_color_make(0, 0, 0), 0);
			lv_obj_set_style_text_font(info_panel, Font_ResouceGet(FONT_14), 0);
			lv_obj_set_style_text_color(info_panel, lv_color_white(), 0);
			lv_obj_align(info_panel, LV_ALIGN_TOP_MID, 0, 0);

			LV_IMG_DECLARE(ImgPan)
			LV_IMG_DECLARE(ImgTilt)
			
			uint8_t tiltm[12];
			data_center_read_config_data(SYS_CONFIG_NO_ANGLE_LIMIT, tiltm);
			if(tiltm[p_sys_menu_model->ele_accessory.ele_no_access_sel] != 1)   //判断角度限制是否开启
			{
				p_sys_menu_model->ele_accessory.ele_yoke_pan1[p_sys_menu_model->ele_accessory.ele_no_access_sel][1] = ANGLE_LIMIT_MAX;
				p_sys_menu_model->ele_accessory.ele_yoke_pan1[p_sys_menu_model->ele_accessory.ele_no_access_sel][0] = ANGLE_LIMIT_MIN;
				p_sys_menu_model->ele_accessory.ele_yoke_tilt1[p_sys_menu_model->ele_accessory.ele_no_access_sel][1] = ANGLE_LIMIT_DEFAULT_MAX;
				p_sys_menu_model->ele_accessory.ele_yoke_tilt1[p_sys_menu_model->ele_accessory.ele_no_access_sel][0] = ANGLE_LIMIT_DEFAULT_MIN;
			}
		
			pan_upper = p_sys_menu_model->ele_accessory.ele_yoke_pan1[p_sys_menu_model->ele_accessory.ele_no_access_sel][1];
            pan_lower = p_sys_menu_model->ele_accessory.ele_yoke_pan1[p_sys_menu_model->ele_accessory.ele_no_access_sel][0];
            tilit_upper = p_sys_menu_model->ele_accessory.ele_yoke_tilt1[p_sys_menu_model->ele_accessory.ele_no_access_sel][1];
            tilit_lower = p_sys_menu_model->ele_accessory.ele_yoke_tilt1[p_sys_menu_model->ele_accessory.ele_no_access_sel][0];
			
			if(tiltm[p_sys_menu_model->ele_accessory.ele_no_access_sel] == 1)
			{
				if(p_sys_menu_model->ele_accessory.ele_no_access_sel <= 6)
				{
					if(tilit_lower < aputure_limit_angle_value[p_sys_menu_model->ele_accessory.ele_no_access_sel].lower_limit_angle)
					{
						tilit_lower = aputure_limit_angle_value[p_sys_menu_model->ele_accessory.ele_no_access_sel].lower_limit_angle;
						p_sys_menu_model->ele_accessory.ele_yoke_tilt1[p_sys_menu_model->ele_accessory.ele_no_access_sel][0] = tilit_lower;
					}
					if(tilit_lower > aputure_limit_angle_value[p_sys_menu_model->ele_accessory.ele_no_access_sel].upper_limit_angle)
					{
						tilit_lower = aputure_limit_angle_value[p_sys_menu_model->ele_accessory.ele_no_access_sel].upper_limit_angle;
						p_sys_menu_model->ele_accessory.ele_yoke_tilt1[p_sys_menu_model->ele_accessory.ele_no_access_sel][0] = tilit_lower;
					}
					if(tilit_upper > aputure_limit_angle_value[p_sys_menu_model->ele_accessory.ele_no_access_sel].upper_limit_angle)
					{
						tilit_upper = aputure_limit_angle_value[p_sys_menu_model->ele_accessory.ele_no_access_sel].upper_limit_angle;
						p_sys_menu_model->ele_accessory.ele_yoke_tilt1[p_sys_menu_model->ele_accessory.ele_no_access_sel][1] = tilit_upper;
					}
					if(tilit_upper < aputure_limit_angle_value[p_sys_menu_model->ele_accessory.ele_no_access_sel].lower_limit_angle)
					{
						tilit_upper = aputure_limit_angle_value[p_sys_menu_model->ele_accessory.ele_no_access_sel].lower_limit_angle;
						p_sys_menu_model->ele_accessory.ele_yoke_tilt1[p_sys_menu_model->ele_accessory.ele_no_access_sel][1] = tilit_upper;
					}
				}
				else
				{
					if(tilit_lower < ANGLE_LIMIT_DEFAULT_MIN)
					{
						tilit_lower = ANGLE_LIMIT_DEFAULT_MIN;
						p_sys_menu_model->ele_accessory.ele_yoke_tilt1[p_sys_menu_model->ele_accessory.ele_no_access_sel][0] = tilit_lower;
					}
					if(tilit_lower > ANGLE_LIMIT_DEFAULT_MAX)
					{
						tilit_lower = ANGLE_LIMIT_DEFAULT_MAX;
						p_sys_menu_model->ele_accessory.ele_yoke_tilt1[p_sys_menu_model->ele_accessory.ele_no_access_sel][0] = tilit_lower;
					}
					if(tilit_upper > ANGLE_LIMIT_DEFAULT_MAX)
					{
						tilit_upper = ANGLE_LIMIT_DEFAULT_MAX;
						p_sys_menu_model->ele_accessory.ele_yoke_tilt1[p_sys_menu_model->ele_accessory.ele_no_access_sel][1] = tilit_upper;
					}
					if(tilit_upper < ANGLE_LIMIT_DEFAULT_MIN)
					{
						tilit_upper = ANGLE_LIMIT_DEFAULT_MIN;
						p_sys_menu_model->ele_accessory.ele_yoke_tilt1[p_sys_menu_model->ele_accessory.ele_no_access_sel][1] = tilit_upper;
					}
				}
				if(p_sys_menu_model->ele_accessory.ele_yoke_pan_val < pan_lower)
					p_sys_menu_model->ele_accessory.ele_yoke_pan_val = pan_lower;
				if(p_sys_menu_model->ele_accessory.ele_yoke_pan_val > pan_upper)
					p_sys_menu_model->ele_accessory.ele_yoke_pan_val = pan_upper;
				if(p_sys_menu_model->ele_accessory.ele_yoke_tilt_val < tilit_lower)
				{
					p_sys_menu_model->ele_accessory.ele_yoke_tilt_val = tilit_lower;
					data_center_read_sys_info(SYS_INFO_MOTOR, &motor_info);
					motor_info.tilt_angle = p_sys_menu_model->ele_accessory.ele_yoke_tilt_val;
                    data_center_write_sys_info(SYS_INFO_MOTOR, &motor_info);
				}
				if(p_sys_menu_model->ele_accessory.ele_yoke_tilt_val > tilit_upper)
				{
					p_sys_menu_model->ele_accessory.ele_yoke_tilt_val = tilit_upper;
					data_center_read_sys_info(SYS_INFO_MOTOR, &motor_info);
					motor_info.tilt_angle = p_sys_menu_model->ele_accessory.ele_yoke_tilt_val;
                    data_center_write_sys_info(SYS_INFO_MOTOR, &motor_info);
				}
			}
			
			yoke_info_creat(info_panel, pan_lower, pan_upper, &ImgPan, 69, (char *)Lang_GetStringByID(STRING_ID_PAN_LIMIT));
            yoke_info_creat(info_panel, tilit_lower, tilit_upper, &ImgTilt, 220, (char *)Lang_GetStringByID(STRING_ID_TILT_LIMIT));
			
			static lv_coord_t col_dsc[] = { 150, 150, LV_GRID_TEMPLATE_LAST };
			static lv_coord_t row_dsc[] = { 43, 43, LV_GRID_TEMPLATE_LAST };
			static lv_coord_t col2_dsc[] = { 265, LV_GRID_TEMPLATE_LAST };
			static lv_coord_t row2_dsc[] = { 43, LV_GRID_TEMPLATE_LAST };

			lv_obj_t* btn_panel = btn_panel_grid_creat(320, 116, col_dsc, row_dsc, 10);
			lv_obj_align_to(btn_panel, info_panel, LV_ALIGN_OUT_BOTTOM_MID, 0, 0);
			lv_obj_t* btn2_panel = btn_panel_grid_creat(320, 67, col2_dsc, row2_dsc, 28);
			lv_obj_align_to(btn2_panel, btn_panel, LV_ALIGN_OUT_BOTTOM_MID, 0, 0);

			uint8_t col;
			uint8_t row;
			lv_obj_t* label1;
			lv_obj_t* obj1;

			for (uint8_t i = 0; i < 4; i++)
			{
				col = i % 2;
				row = i / 2;

				obj1 = lv_obj_create(btn_panel);
				lv_obj_set_grid_cell(obj1, LV_GRID_ALIGN_STRETCH, col, 1, LV_GRID_ALIGN_STRETCH, row, 1);
				
				lv_obj_clear_flag(obj1, LV_OBJ_FLAG_SCROLLABLE);
				lv_obj_add_style(obj1, &style_menu_item, LV_STATE_DEFAULT);
				lv_obj_set_style_bg_color(obj1, lv_color_make(0x23, 0x23, 0x23), LV_STATE_DEFAULT);
				lv_obj_add_style(obj1, &style_menu_focused_item, LV_STATE_FOCUSED);
				lv_obj_set_style_bg_color(obj1, RED_THEME_COLOR, LV_STATE_FOCUSED);
				if(i <= 1)
					lv_obj_set_style_text_font(obj1, Font_ResouceGet(FONT_18_BOLD), LV_STATE_USER_1);
				else
					lv_obj_set_style_text_font(obj1, Font_ResouceGet(FONT_14_MEDIUM), LV_STATE_USER_1);
			
				str_dsc_t str_dsc;
				get_yoke_btn_str(i, &str_dsc);
				label1 = lv_label_create(obj1);
				if(i <= 1)
					lv_obj_set_style_text_font(label1, Font_ResouceGet(FONT_18), LV_STATE_DEFAULT);
				else
					lv_obj_set_style_text_font(label1, Font_ResouceGet(FONT_14_MEDIUM), LV_STATE_DEFAULT);
				lv_obj_set_style_text_color(obj1, lv_color_white(), 0);
				lv_obj_set_style_text_align(obj1, LV_TEXT_ALIGN_CENTER, 0);
				if (i <= 1)
				{
					data_center_read_sys_info(SYS_INFO_MOTOR, &motor_info);
					
					lv_obj_align(label1, LV_ALIGN_LEFT_MID, -10, 0);

					lv_obj_t* label2 = lv_label_create(obj1);
					lv_obj_align(label2, LV_ALIGN_CENTER, 0, 0);

					lv_obj_set_style_text_font(label2, Font_ResouceGet(FONT_16_BOLD), LV_STATE_USER_1);

					lv_obj_t* img_lock = lv_img_create(obj1);
					lv_obj_set_style_img_recolor(img_lock, lv_color_white(), 0);
					lv_obj_set_style_img_recolor_opa(img_lock, LV_OPA_100, 0);
					lv_obj_align(img_lock, LV_ALIGN_RIGHT_MID, 10, 0);
					
					lv_obj_add_event_cb(obj1, yoke_pan_event_cb, LV_EVENT_ALL, NULL);
					if (i == 0)
					{
						refresh_adj_value_label(label2, p_sys_menu_model->ele_accessory.ele_yoke_pan_val);
						refresh_lock_img(img_lock, p_sys_menu_model->ele_accessory.ele_yoke_pan_lock);  
					}
					else
					{
						refresh_adj_value_label(label2, p_sys_menu_model->ele_accessory.ele_yoke_tilt_val);
						refresh_lock_img(img_lock, p_sys_menu_model->ele_accessory.ele_yoke_tilt_lock); 
					}
				}
				else
				{
					lv_obj_center(label1); 
					lv_obj_add_event_cb(obj1, yoke_btn_event_cb, LV_EVENT_ALL, NULL);
				}
				lv_obj_set_style_text_line_space(label1, 2, 0);
				lv_obj_set_style_text_color(obj1, lv_color_white(), LV_STATE_FOCUSED);
				lv_label_set_text(label1, str_dsc.str);
				
				if(i == 0)
					lv_group_focus_obj(obj1);
				lv_group_add_obj(indev_group, obj1);
			}
			lv_group_set_wrap(indev_group, false);
			lv_group_set_editing(indev_group, false);
		}
		break;
		case SUB_ID_ELE_DEMO:
			access_spid = 3;
			lv_obj_t* label;
		
			memcpy(&s_access_info_last, &s_access_info, sizeof(struct sys_info_accessories));
			lv_obj_set_style_bg_color(lv_scr_act(), lv_color_black(), 0);
            lv_obj_t* title = lv_label_create(lv_scr_act());
			lv_obj_clear_flag(lv_scr_act(), LV_OBJ_FLAG_SCROLLABLE);
		
            lv_obj_set_style_text_font(title, Font_ResouceGet(FONT_22_BOLD), 0);
            lv_obj_set_style_text_color(title, lv_color_white(), 0);
            lv_label_set_text(title, Lang_GetStringByID(STRING_ID_DEMO));
            lv_obj_align(title, LV_ALIGN_TOP_MID, 0, SYS_MENU_TITLE_TO_TOP);
			
			if(s_access_info.yoke_state == 0)
            {
                uint8_t lang_type = 0;
                
                lv_obj_t* btn_plate = lv_obj_create(lv_scr_act());
                lv_obj_add_style(btn_plate, &style_common_btn, 0);
                lv_obj_set_style_radius(btn_plate, 0, 0);
                lv_obj_set_size(btn_plate, 265, 159);
                lv_obj_align(btn_plate, LV_ALIGN_CENTER, 0, 10);
                data_center_read_config_data(SYS_CONFIG_LANG, &lang_type);
                lv_obj_t* label_panel = lv_label_create(btn_plate);
                lv_obj_set_style_text_font(label_panel, Font_ResouceGet(FONT_18_BOLD), 0);
                lv_obj_set_style_text_color(label_panel, lv_color_white(), 0);
				lv_label_set_text(label_panel, Lang_GetStringByID(STRING_ID_NO_YOKE));
                lv_obj_align(label_panel, LV_ALIGN_CENTER, 0, 5);
                
                lv_obj_add_event_cb(label_panel, access_btn_eent_cb, LV_EVENT_ALL, NULL);
                lv_group_add_obj(indev_group, label_panel);

                lv_group_focus_obj(label_panel);
                lv_group_set_wrap(indev_group, false);
				lv_group_set_editing(indev_group, false);
            }
            else if(s_access_info.yoke_state == 1)
            {
				list_ext1.max = PAGE_DEMO_BTN_NUMBER;
				list_ext1.release_cb = NULL;
				motor_demo_list = lv_obj_create(lv_scr_act());    
				lv_obj_add_style(motor_demo_list, &style_rect_panel, 0);
				lv_obj_set_style_bg_color(motor_demo_list, lv_color_black(), 0);
				lv_obj_clear_flag(motor_demo_list, LV_OBJ_FLAG_SCROLLABLE);
				lv_obj_set_flex_flow(motor_demo_list, LV_FLEX_FLOW_COLUMN);
				lv_obj_set_user_data(motor_demo_list, &list_ext1);
				lv_obj_set_size(motor_demo_list, lv_pct(90), lv_pct(75));
				list_ext1.childs = 0;
				lv_obj_align(motor_demo_list, LV_ALIGN_CENTER, 0, -20);
				lv_obj_set_flex_align(motor_demo_list, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);  
				lv_obj_align(motor_demo_list, LV_ALIGN_TOP_MID, 0, 53);
				
				lv_obj_t* demo_btn = my_list_add_switch_btn(motor_demo_list, Lang_GetStringByID(STRING_ID_DEMO), p_sys_menu_model->ele_accessory.demo_state);
				list_state_btn_refresh(demo_btn, p_sys_menu_model->ele_accessory.demo_state);
				lv_obj_add_event_cb(demo_btn, switch_btn_event_cb, LV_EVENT_ALL, NULL);
				lv_group_add_obj(indev_group, demo_btn);
				demo_obj_num[list_ext1.childs] = demo_btn;
				list_ext1.childs+=1;
				
				lv_obj_t* pan_btn = my_list_add_switch_btn(motor_demo_list, Lang_GetStringByID(STRING_ID_PAN), p_sys_menu_model->ele_accessory.pan_demo_state);
				lv_obj_add_event_cb(pan_btn, switch_btn_event_cb, LV_EVENT_ALL, NULL);
				lv_group_add_obj(indev_group, pan_btn);
				demo_obj_num[list_ext1.childs] = pan_btn;
				list_ext1.childs+=1;
				
				lv_obj_t* pan_limit_btn = my_list_add_btn4(motor_demo_list);
				demo_obj_num[list_ext1.childs] = pan_limit_btn;
				s_list_ext[0].max = 3;
				s_list_ext[0].cur_sel = 0;
				lv_obj_set_user_data(pan_limit_btn, &s_list_ext[0]);
				lv_obj_add_event_cb(pan_limit_btn, demo_limit_btn_event_cb, LV_EVENT_ALL, NULL);
				lv_group_add_obj(indev_group, pan_limit_btn);  
				if(p_sys_menu_model->ele_accessory.pan_demo_limit[0] <= ANGLE_LIMIT_MIN)
					p_sys_menu_model->ele_accessory.pan_demo_limit[0] = ANGLE_LIMIT_MIN;
				if(p_sys_menu_model->ele_accessory.pan_demo_limit[1] >= ANGLE_LIMIT_MAX)
					p_sys_menu_model->ele_accessory.pan_demo_limit[1] = ANGLE_LIMIT_MAX;
				motor_demo_text_init(pan_limit_btn, p_sys_menu_model->ele_accessory.pan_demo_limit, Lang_GetStringByID(STRING_ID_PAN_LIMIT));
				list_ext1.childs+=1;
				
				lv_obj_t* tili_btn = my_list_add_switch_btn(motor_demo_list, Lang_GetStringByID(STRING_ID_TILT), p_sys_menu_model->ele_accessory.tili_demo_state);
				lv_obj_add_event_cb(tili_btn, switch_btn_event_cb, LV_EVENT_ALL, NULL);
				lv_group_add_obj(indev_group, tili_btn);
				demo_obj_num[list_ext1.childs] = tili_btn;
				list_ext1.childs+=1;
				
				lv_obj_t* tili_limit_btn = my_list_add_btn4(motor_demo_list);
				s_list_ext[1].max = 3;
				s_list_ext[1].cur_sel = 0;
				lv_obj_set_user_data(tili_limit_btn, &s_list_ext[1]);
				lv_obj_add_event_cb(tili_limit_btn, demo_limit_btn_event_cb, LV_EVENT_ALL, NULL);
				lv_group_add_obj(indev_group, tili_limit_btn);  
				if(s_access_info.yoke_state == 1 && (s_access_info.fresnel_state == 1 || s_access_info.optical_20_access_state == 1))
				{
					if(p_sys_menu_model->ele_accessory.tili_demo_limit[0] <= -120)
						p_sys_menu_model->ele_accessory.tili_demo_limit[0] = -120;
					if(p_sys_menu_model->ele_accessory.tili_demo_limit[0] >= 120)
						p_sys_menu_model->ele_accessory.tili_demo_limit[0] = 120;
					if(p_sys_menu_model->ele_accessory.tili_demo_limit[1] >= 120)
						p_sys_menu_model->ele_accessory.tili_demo_limit[1] = 120;
					if(p_sys_menu_model->ele_accessory.tili_demo_limit[1] <= -120)
						p_sys_menu_model->ele_accessory.tili_demo_limit[1] = -120;
				}
				else
				{
					if(p_sys_menu_model->ele_accessory.tili_demo_limit[0] <= ANGLE_LIMIT_DEFAULT_MIN)
						p_sys_menu_model->ele_accessory.tili_demo_limit[0] = ANGLE_LIMIT_DEFAULT_MIN;
					if(p_sys_menu_model->ele_accessory.tili_demo_limit[1] >= ANGLE_LIMIT_DEFAULT_MAX)
						p_sys_menu_model->ele_accessory.tili_demo_limit[1] = ANGLE_LIMIT_DEFAULT_MAX;
				}
				data_center_write_config_data_no_event(SYS_CONFIG_TILI_DEMO_LIMIT, p_sys_menu_model->ele_accessory.tili_demo_limit);
				data_center_write_config_data_no_event(SYS_CONFIG_PAN_DEMO_LIMIT, p_sys_menu_model->ele_accessory.pan_demo_limit);
				motor_demo_text_init(tili_limit_btn, p_sys_menu_model->ele_accessory.tili_demo_limit, Lang_GetStringByID(STRING_ID_TILT_LIMIT));
				demo_obj_num[list_ext1.childs] = tili_limit_btn;
				list_ext1.childs+=1;
				
				lv_obj_t* zoom_btn = my_list_add_switch_btn(motor_demo_list, Lang_GetStringByID(STRING_ID_ELE_DEMO_FRESNEL), p_sys_menu_model->ele_accessory.zoom_demo_state);
				lv_obj_add_event_cb(zoom_btn, switch_btn_event_cb, LV_EVENT_ALL, NULL);
				lv_group_add_obj(indev_group, zoom_btn);
				demo_obj_num[list_ext1.childs] = zoom_btn;
				list_ext1.childs+=1;
				
				lv_obj_t* zoom_limit_btn = my_list_add_btn4(motor_demo_list);
				s_list_ext[2].max = 3;
				s_list_ext[2].cur_sel = 0;
				lv_obj_set_user_data(zoom_limit_btn, &s_list_ext[2]);
				lv_obj_add_event_cb(zoom_limit_btn, demo_limit_btn_event_cb, LV_EVENT_ALL, NULL);
				lv_group_add_obj(indev_group, zoom_limit_btn);  
				if(p_sys_menu_model->ele_accessory.zoom_demo_limit[0] <= 18)
					p_sys_menu_model->ele_accessory.zoom_demo_limit[0] = 18;
				if(p_sys_menu_model->ele_accessory.zoom_demo_limit[1] >= 50)
					p_sys_menu_model->ele_accessory.zoom_demo_limit[1] = 50;
				motor_demo_text_init(zoom_limit_btn, p_sys_menu_model->ele_accessory.zoom_demo_limit, Lang_GetStringByID(STRING_ID_FRESNEL_LIMIT));
				demo_obj_num[list_ext1.childs] = zoom_limit_btn;
				list_ext1.childs+=1;
				
				lv_obj_t* power_btn = my_list_add_switch_btn(motor_demo_list, Lang_GetStringByID(STRING_ID_DEMO_POWER_ON), p_sys_menu_model->ele_accessory.power_on_state);
				list_state_btn_refresh(power_btn, p_sys_menu_model->ele_accessory.power_on_state);
				lv_obj_add_event_cb(power_btn, switch_btn_event_cb, LV_EVENT_ALL, NULL);
				lv_group_add_obj(indev_group, power_btn);
				demo_obj_num[list_ext1.childs] = power_btn;
				list_ext1.childs+=1;
				
				switch(list_ext1.cur_sel)
				{
					case 0: lv_group_focus_obj(demo_btn);break;
					case 1: lv_group_focus_obj(pan_btn);break;
					case 2: lv_group_focus_obj(pan_limit_btn);break;
					case 3: lv_group_focus_obj(tili_btn);break;
					case 4: lv_group_focus_obj(tili_limit_btn);break;
					case 5: lv_group_focus_obj(zoom_btn);break;
					case 6: lv_group_focus_obj(zoom_limit_btn);break;
					case 7: lv_group_focus_obj(power_btn);break;
					default:break;
				}
				
				demo_scroll_cont = my_list_scroll_create(lv_scr_act(), 3, 190);                        
                lv_obj_align_to(demo_scroll_cont, motor_demo_list, LV_ALIGN_OUT_RIGHT_TOP, 10, 2);
				
				lv_group_set_wrap(indev_group, false);
				lv_group_set_editing(indev_group, false);
			}
		break;
		case SUB_ID_ELE_RESET:
			ui_set_ele_reset_start_flag(BLE_RESET_START);
			
			lv_obj_set_style_bg_color(lv_scr_act(), lv_color_black(), 0);
		
			lv_obj_t* reset_bar = lv_bar_create(lv_scr_act());
			lv_obj_set_style_bg_color(reset_bar, lv_color_make(255, 255, 255), LV_PART_INDICATOR);
			lv_obj_set_style_bg_color(reset_bar, lv_color_make(128, 128, 128), LV_PART_MAIN);
			lv_obj_set_style_outline_pad(reset_bar, 0, 0);
			lv_obj_set_style_outline_width(reset_bar, 0, 0);
			lv_obj_set_size(reset_bar, 250, 21);                
			lv_obj_align(reset_bar, LV_ALIGN_CENTER, 0, 0);    
			lv_bar_set_range(reset_bar, 0, 100);
			
			lv_obj_t* process_label = lv_label_create(lv_scr_act());
			lv_obj_set_style_text_font(process_label, Font_ResouceGet(FONT_18_BOLD), 0);
			lv_obj_set_style_text_color(process_label, lv_color_white(), LV_PART_MAIN);
			lv_label_set_text(process_label, "0%");
			lv_obj_align_to(process_label, reset_bar, LV_ALIGN_OUT_TOP_MID, 0, -10);    
			
			lv_obj_t* status_label = lv_label_create(lv_scr_act());
			lv_obj_set_style_text_font(status_label, Font_ResouceGet(FONT_22_BOLD), 0);
			lv_obj_set_style_text_color(status_label, lv_color_white(), LV_PART_MAIN);
			if(motor_reset_state_get() == 1)
				lv_label_set_text(status_label, Lang_GetStringByID(STRING_ID_FRESNEL_RESETING));   
			else if(motor_reset_state_get() == 2)
				lv_label_set_text(status_label, Lang_GetStringByID(STRING_ID_TOKE_RESETING));   
			lv_obj_align_to(status_label, reset_bar, LV_ALIGN_OUT_BOTTOM_MID, 0, 20); 
			
		break;
		case SUB_ID_ELE_RESET_STATE:
		{
			lv_obj_t *panel;
			
			lv_obj_set_style_bg_color(lv_scr_act(), lv_color_black(), 0);
			if(wring_flag == 1)
			{
				LV_IMG_DECLARE(ImgConfirmFailed)
				panel = warning_info_init(lv_scr_act(), &ImgConfirmFailed, 54, 25,Lang_GetStringByID(STRING_ID_RESET_FAILED)); 
			}
			else if(wring_flag == 2)
			{
				LV_IMG_DECLARE(ImgConfirmOk)
				panel = warning_info_init(lv_scr_act(), &ImgConfirmOk, 54, 25,Lang_GetStringByID(STRING_ID_RESET_OK));  
			}
			lv_obj_add_event_cb(panel, NULL, LV_EVENT_ALL, NULL);
			lv_group_add_obj(indev_group, panel);
		}
		break;
		
		case SUB_ID_ACCESSORY_REGOGNITION:
		{
			#if 0
			access_spid = 3;
			lv_obj_clear_flag(lv_scr_act(), LV_OBJ_FLAG_SCROLLABLE);
            lv_obj_set_style_bg_color(lv_scr_act(), lv_color_black(), 0);
            lv_obj_t* title = lv_label_create(lv_scr_act());

            lv_obj_set_style_text_font(title, Font_ResouceGet(FONT_22_BOLD), 0);
            lv_obj_set_style_text_color(title, lv_color_white(), 0);
            lv_label_set_text(title, Lang_GetStringByID(STRING_ID_OUTPUT_WRING));
            lv_obj_align(title, LV_ALIGN_TOP_MID, 0, SYS_MENU_TITLE_TO_TOP);
			
			lv_obj_t *label = lv_label_create(lv_scr_act());
			lv_label_set_long_mode(label, LV_LABEL_LONG_WRAP);
			lv_obj_set_size(label, lv_pct(100), LV_SIZE_CONTENT);  // 使其文字标签的高度随文字长度变化
			lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, 0);  // 可以使得换行后的标签居中显示
			lv_obj_set_style_text_font(label, Font_ResouceGet(FONT_18), 0);
            lv_obj_set_style_text_color(label, lv_color_white(), 0);
			if (!p_sys_menu_model->regognition_status)
			{
				lv_label_set_text(label, Lang_GetStringByID(STRING_ID_ACC_RECON_ON));
			}
			else
			{
				lv_label_set_text(label,Lang_GetStringByID(STRING_ID_ACC_RECON_OFF));
			}
            lv_obj_align(label, LV_ALIGN_TOP_MID, 0, 38);
			
			lv_obj_t *btn = lv_obj_create(lv_scr_act());
			lv_obj_remove_style_all(btn);
//			lv_obj_clear_flag(btn, LV_OBJ_FLAG_SCROLLABLE);
			lv_obj_set_style_bg_opa(btn, LV_OPA_100, 0);
			lv_obj_set_style_opa(btn, LV_OPA_100, 0);
			lv_obj_set_style_bg_color(btn, lv_color_make(196, 56, 43), 0);
			lv_obj_set_size(btn, 85, 34);
			lv_obj_set_style_radius(btn, 5, 0);
			lv_obj_set_style_radius(btn, 5, LV_STATE_FOCUSED);
			lv_obj_align(btn, LV_ALIGN_BOTTOM_MID, 0, -13);
			lv_obj_add_event_cb(btn, warning_event_cb, LV_EVENT_ALL, NULL);
			
			lv_obj_t *label2 = lv_label_create(btn);
			lv_obj_set_style_text_font(label2, Font_ResouceGet(FONT_18), 0);
			lv_obj_set_style_text_color(label2, lv_color_white(), 0);
			
			lv_label_set_text(label2, Lang_GetStringByID(STRING_ID_CONFIRM));
			lv_obj_align(label2, LV_ALIGN_CENTER, 0, 0);
			
			lv_group_add_obj(indev_group, btn);
            lv_group_focus_obj(btn);
            lv_group_set_wrap(indev_group, false);
			lv_group_set_editing(indev_group, false);
			#endif
		}
		break;
        default:break;
    }
}

static void page_elect_accessory_time_upd_cb(uint32_t ms)
{
	page_invalid_type_t type = page_get_invalid_type();
    uint8_t spid = screen_get_act_spid();
	data_center_read_sys_info(SYS_INFO_ACCESS, &s_access_info);
	bool res = false;
	
	switch(type)
    {
		case PAGE_INVALID_TYPE_REFRESH:
			page_set_invalid_type(PAGE_INVALID_TYPE_IDLE);
			res = true;
		break;
        default:break;
    }
	
    switch(spid)
    {
        case SUB_ID_ELE_FRESNEL:
		{
			char str[10] = {0};
			lv_obj_t* label_val = lv_obj_get_child(btn_list[0].obj, 2);
			if(s_access_info_last.fresnel_state != s_access_info.fresnel_state)
			{
				screen_load_prev_sub_page(SUB_ID_ACCESSORY_LIST, 0);
				break;
			}
            if(s_access_info.fresnel_state == 1)
            {
                data_center_read_sys_info(SYS_INFO_MOTOR, &motor_info);
				if(1 == ui_fresnel_get_angle_flag_get())
				{
					if(s_ui_fresnel_angle_count++ > 4)
					{
						s_ui_fresnel_angle_count = 0;
						ui_fresnel_get_angle_flag_set(0);
					}
				}
				if((motor_info.fresnel_angle!= ele_fresnel_angle) && (0 == ui_fresnel_get_angle_flag_get()))
                {
					ele_fresnel_angle = motor_info.fresnel_angle;
                    btn_list[0].value = ele_fresnel_angle*10;
					angle_upd_run_time();
					sprintf(str, "%.1f°", motor_info.fresnel_angle);
					lv_label_set_text(angle_label, str);
					lv_label_set_text(label_val, str);
                    adj_btn3_refresh(btn_list[0].obj, &btn_list[0]);
                }
            }
		}
        break;
        case SUB_ID_ELE_YOKE:
            if(s_access_info.fresnel_state == 1 && s_access_info.yoke_state == 1)
            {
                lv_obj_t *obj = lv_obj_get_child(lv_scr_act(), 1);
                data_center_read_sys_info(SYS_INFO_MOTOR, &motor_info);
                if(motor_info.pan_angle != p_sys_menu_model->ele_accessory.ele_yoke_pan_val)
                {
                    p_sys_menu_model->ele_accessory.ele_yoke_pan_val = motor_info.pan_angle;
                    lv_obj_t *obj1 = lv_obj_get_child(obj, 0);
                    lv_obj_t *obj2 = lv_obj_get_child(obj1, 1);
                    refresh_adj_value_label(obj2, p_sys_menu_model->ele_accessory.ele_yoke_pan_val);
                    data_center_write_sys_info(SYS_INFO_MOTOR, &motor_info);
                }
                if(motor_info.tilt_angle != p_sys_menu_model->ele_accessory.ele_yoke_tilt_val)
                {
                    p_sys_menu_model->ele_accessory.ele_yoke_tilt_val = motor_info.tilt_angle;
                    lv_obj_t *obj1 = lv_obj_get_child(obj, 1);
                    lv_obj_t *obj2 = lv_obj_get_child(obj1, 1);
                    refresh_adj_value_label(obj2, p_sys_menu_model->ele_accessory.ele_yoke_tilt_val);
                    data_center_write_sys_info(SYS_INFO_MOTOR, &motor_info);
                }
            }
			if(res == true)
			{
				if(s_access_info.fresnel_state == 1 || s_access_info.optical_20_access_state == 1 || s_access_info.optical_30_access_state != 1 || s_access_info.optical_50_access_state != 1)
				{
					lv_obj_t *pre_obj = lv_obj_get_child(lv_scr_act(), 1);
					lv_obj_t *btn_obj = lv_obj_get_child(pre_obj, 0);
					lv_obj_t *pan_lock_obj = lv_obj_get_child(btn_obj, 2);
					refresh_lock_img(pan_lock_obj, p_sys_menu_model->ele_accessory.ele_yoke_pan_lock);
					lv_obj_t *tilt_btn_obj = lv_obj_get_child(btn_obj, 3);
					lv_obj_t *tilt_lock_obj = lv_obj_get_child(tilt_btn_obj, 2);
					refresh_lock_img(tilt_lock_obj, p_sys_menu_model->ele_accessory.ele_yoke_tilt_lock);
				}
			}
			if(s_access_info_last.yoke_state != s_access_info.yoke_state || 
			   s_access_info_last.fresnel_state != s_access_info.fresnel_state ||
			   s_access_info_last.optical_20_access_state != s_access_info.optical_20_access_state || 
			   s_access_info_last.optical_30_access_state != s_access_info.optical_30_access_state ||
			   s_access_info_last.optical_50_access_state != s_access_info.optical_50_access_state)
			{
				screen_load_prev_sub_page(SUB_ID_ACCESSORY_LIST, 1);
			}
        break;
        case SUB_ID_ELE_NO_YOKE:
            if(s_access_info.yoke_state == 1)
            {
                lv_obj_t *obj = lv_obj_get_child(lv_scr_act(), 1);
                data_center_read_sys_info(SYS_INFO_MOTOR, &motor_info);
                if(motor_info.pan_angle != p_sys_menu_model->ele_accessory.ele_yoke_pan_val1[p_sys_menu_model->ele_accessory.ele_no_access_sel])
                {
                    p_sys_menu_model->ele_accessory.ele_yoke_pan_val1[p_sys_menu_model->ele_accessory.ele_no_access_sel] = motor_info.pan_angle;
                    lv_obj_t *obj1 = lv_obj_get_child(obj, 0);
                    lv_obj_t *obj2 = lv_obj_get_child(obj1, 1);
                    refresh_adj_value_label(obj2, motor_info.pan_angle);
                    data_center_write_sys_info(SYS_INFO_MOTOR, &motor_info);
                }
				if(res == true)
				{
					lv_obj_t *pre_obj = lv_obj_get_child(lv_scr_act(), 1);
					lv_obj_t *btn_obj = lv_obj_get_child(pre_obj, 0);
					lv_obj_t *pan_lock_obj = lv_obj_get_child(btn_obj, 2);
					refresh_lock_img(pan_lock_obj, p_sys_menu_model->ele_accessory.ele_yoke_pan_lock1[p_sys_menu_model->ele_accessory.ele_no_access_sel]);
					lv_obj_t *tilt_btn_obj = lv_obj_get_child(btn_obj, 3);
					lv_obj_t *tilt_lock_obj = lv_obj_get_child(tilt_btn_obj, 2);
					refresh_lock_img(tilt_lock_obj, p_sys_menu_model->ele_accessory.ele_yoke_tilt_lock1[p_sys_menu_model->ele_accessory.ele_no_access_sel]);
				}
                if(motor_info.tilt_angle != p_sys_menu_model->ele_accessory.ele_yoke_tilt_val1[p_sys_menu_model->ele_accessory.ele_no_access_sel])
                {
                    p_sys_menu_model->ele_accessory.ele_yoke_tilt_val1[p_sys_menu_model->ele_accessory.ele_no_access_sel] = motor_info.tilt_angle;
                    lv_obj_t *obj1 = lv_obj_get_child(obj, 1);
                    lv_obj_t *obj2 = lv_obj_get_child(obj1, 1);
                    refresh_adj_value_label(obj2, motor_info.tilt_angle);
                    data_center_write_sys_info(SYS_INFO_MOTOR, &motor_info);
                }
            }
			if(s_access_info_last.yoke_state != s_access_info.yoke_state || 
			   s_access_info_last.fresnel_state != s_access_info.fresnel_state ||
			   s_access_info_last.optical_20_access_state != s_access_info.optical_20_access_state || 
			   s_access_info_last.optical_30_access_state != s_access_info.optical_30_access_state ||
			   s_access_info_last.optical_50_access_state != s_access_info.optical_50_access_state)
			{
				screen_load_prev_sub_page(SUB_ID_ACCESSORY_LIST, 1);
			}
        break;
		case SUB_ID_ELE_APTURE_NO_YOKE:
			if(s_access_info.yoke_state == 1)
            {
                lv_obj_t *obj = lv_obj_get_child(lv_scr_act(), 1);
                data_center_read_sys_info(SYS_INFO_MOTOR, &motor_info);
                if(motor_info.pan_angle != p_sys_menu_model->ele_accessory.ele_yoke_pan_val1[p_sys_menu_model->ele_accessory.ele_no_access_sel])
                {
                    p_sys_menu_model->ele_accessory.ele_yoke_pan_val1[p_sys_menu_model->ele_accessory.ele_no_access_sel] = motor_info.pan_angle;
                    lv_obj_t *obj1 = lv_obj_get_child(obj, 0);
                    lv_obj_t *obj2 = lv_obj_get_child(obj1, 1);
                    refresh_adj_value_label(obj2, motor_info.pan_angle);
                    data_center_write_sys_info(SYS_INFO_MOTOR, &motor_info);
                }
                if(motor_info.tilt_angle != p_sys_menu_model->ele_accessory.ele_yoke_tilt_val1[p_sys_menu_model->ele_accessory.ele_no_access_sel])
                {
                    p_sys_menu_model->ele_accessory.ele_yoke_tilt_val1[p_sys_menu_model->ele_accessory.ele_no_access_sel] = motor_info.tilt_angle;
                    lv_obj_t *obj1 = lv_obj_get_child(obj, 1);
                    lv_obj_t *obj2 = lv_obj_get_child(obj1, 1);
                    refresh_adj_value_label(obj2, motor_info.tilt_angle);
                    data_center_write_sys_info(SYS_INFO_MOTOR, &motor_info);
                }
				if(res == true)
				{
					lv_obj_t *pre_obj = lv_obj_get_child(lv_scr_act(), 1);
					lv_obj_t *btn_obj = lv_obj_get_child(pre_obj, 0);
					lv_obj_t *pan_lock_obj = lv_obj_get_child(btn_obj, 2);
					refresh_lock_img(pan_lock_obj, p_sys_menu_model->ele_accessory.ele_yoke_pan_lock1[p_sys_menu_model->ele_accessory.ele_no_access_sel]);
					lv_obj_t *tilt_btn_obj = lv_obj_get_child(pre_obj, 1);
					lv_obj_t *tilt_lock_obj = lv_obj_get_child(tilt_btn_obj, 2);
					refresh_lock_img(tilt_lock_obj, p_sys_menu_model->ele_accessory.ele_yoke_tilt_lock1[p_sys_menu_model->ele_accessory.ele_no_access_sel]);
				}
            }
			if(s_access_info_last.yoke_state != s_access_info.yoke_state || 
			   s_access_info_last.fresnel_state != s_access_info.fresnel_state ||
			   s_access_info_last.optical_20_access_state != s_access_info.optical_20_access_state || 
			   s_access_info_last.optical_30_access_state != s_access_info.optical_30_access_state ||
			   s_access_info_last.optical_50_access_state != s_access_info.optical_50_access_state)
			{
				screen_load_prev_sub_page(SUB_ID_ACCESSORY_LIST, 1);
			}
		break;
		case SUB_ID_ELE_OPTICAL_ACCESS:
			if(s_access_info_last.optical_20_access_state != s_access_info.optical_20_access_state || 
			   s_access_info_last.optical_30_access_state != s_access_info.optical_30_access_state ||
			   s_access_info_last.optical_50_access_state != s_access_info.optical_50_access_state ||
			   s_access_info_last.adapter_ring != s_access_info.adapter_ring)
			{
				screen_load_prev_sub_page(SUB_ID_ACCESSORY_LIST, 2);
			}
        break;
		case SUB_ID_ELE_DEMO:
			if(s_access_info_last.yoke_state != s_access_info.yoke_state)
			{
				screen_load_prev_sub_page(SUB_ID_ACCESSORY_LIST, 3);
			}
		break;
		case SUB_ID_ELE_RESET:
		{
			static uint8_t percent = 0;
			static uint8_t reset_timems = 0;
			static uint8_t reset_times = 0;
			lv_obj_t *bar;
			
			reset_timems++;
			if(reset_timems >= 3)
			{
				reset_timems = 0;
				reset_times++;
				if(reset_times > (ELE_RESET_TIMEOUT-1))
				{
					reset_times = 0;
					percent = 100;
					bar = lv_obj_get_child(lv_scr_act(), 0);
					bar_anim_exec_xcb(bar, percent);
				}
				if(percent < 100 && ui_get_ele_reset_start_flag() == BLE_RESET_START)
				{
					percent += 2;
					bar = lv_obj_get_child(lv_scr_act(), 0);
					bar_anim_exec_xcb(bar, percent);
				}
				else if(percent < 100 && ui_get_ele_reset_start_flag() == BLE_RESET_SUCCEED)
				{
					bar = lv_obj_get_child(lv_scr_act(), 0);
					percent = 100;
					bar_anim_exec_xcb(bar, percent);
				}
				if(percent >= 100)
				{
					percent = 0;
					reset_times = 0;
					if(ui_get_ele_reset_start_flag() == BLE_RESET_SUCCEED)
					{
						wring_flag = 2;
					}
					else
					{
						wring_flag = 1;
					}
					screen_load_sub_page_with_stack(SUB_ID_ELE_RESET_STATE, 0);
				}
			}
		}
		break;
		case SUB_ID_ELE_RESET_STATE:
		{
			static uint8_t reset_state_time = 0;
			
			if(reset_state_time++ >= 2)
			{
				reset_state_time = 0;
				if(motor_reset_flag == 1)
				{
					screen_load_sub_page(SUB_ID_ELE_FRESNEL, 0);
				}
				else if(motor_reset_flag == 2)
				{
					if((s_access_info.yoke_state == 1) && 
				      (s_access_info.fresnel_state == 1 || s_access_info.optical_20_access_state == 1 || s_access_info.optical_30_access_state == 1 || s_access_info.optical_50_access_state == 1))
					{
						screen_load_sub_page(SUB_ID_ELE_YOKE, 0);
					}
					else
					{
						screen_load_sub_page(SUB_ID_ELE_APTURE_NO_YOKE, 0);
					}
				}
				
			}
		}
		break;
        default:break;
    }
}

static void page_elect_accessory_destruct(void)
{
    lv_group_focus_freeze(indev_group, true);
    lv_group_remove_all_objs(indev_group);
    lv_group_del(indev_group);
    indev_group = NULL;

    lv_obj_clean(lv_scr_act());
	memset(&demo_scroll_cont, 0, sizeof(demo_scroll_cont));
}

static void accecc_local_write_data(void)
{
    uint8_t spid = 0;
    
    spid = screen_get_act_spid();
    data_center_read_sys_info(SYS_INFO_MOTOR, &motor_info);
    switch(spid)
    {
        case SUB_ID_ELE_FRESNEL:
            data_center_write_config_data(SYS_CONFIG_FRESNEL_ANGLE, &p_sys_menu_model->ele_accessory.ele_fresnel_angle);
        break;
        case SUB_ID_ELE_YOKE:
            
            if(s_access_flag == 0)   //电动支架和光学附件搭配
            {
                if(s_pan_tilt_flag == 0)
                {
                    data_center_write_config_data(SYS_CONFIG_YOKE_PAN, &p_sys_menu_model->ele_accessory.ele_yoke_pan_val);
                }
                else if(s_pan_tilt_flag == 1)
                {
                    data_center_write_config_data(SYS_CONFIG_YOKE_TILT, &p_sys_menu_model->ele_accessory.ele_yoke_tilt_val);
                }
                motor_info.pan_angle = p_sys_menu_model->ele_accessory.ele_yoke_pan_val;
                motor_info.tilt_angle = p_sys_menu_model->ele_accessory.ele_yoke_tilt_val;
            }
            else if(s_access_flag == 1)   //电动支架和非光学附件搭配
            {
                uint8_t sel = 0;
                sel = access_radiobox_ext.sel;
                data_center_write_config_data_no_event(SYS_CONFIG_NOT_ACCESS_SELECT, &sel);
            }
            data_center_write_sys_info(SYS_INFO_MOTOR, &motor_info);
        break;
        case SUB_ID_ELE_YOKE_ANGLE_LIMIT:
        case SUB_ID_ELE_NO_YOKE_ANGLE_LIMIT:
        break;
        case SUB_ID_ELE_NO_YOKE_ANGLE_LIMIT_ENTER:
        {
            struct sys_info_lamp   info_read_body = {0};
            data_center_read_sys_info(SYS_INFO_LAMP, &info_read_body);		
        
            data_center_write_config_data_no_event(SYS_CONFIG_NOT_ACCESS_SELECT,  &p_sys_menu_model->ele_accessory.ele_no_access_sel);
            data_center_write_config_data_no_event(SYS_CONFIG_YOKE_NO_TILT_LIMIT, &p_sys_menu_model->ele_accessory.ele_yoke_tilt1);
            data_center_write_config_data_no_event(SYS_CONFIG_YOKE_NO_PAN_LIMIT,  &p_sys_menu_model->ele_accessory.ele_yoke_pan1);
            data_center_write_config_data_no_event(SYS_CONFIG_NO_ANGLE_LIMIT,        &p_sys_menu_model->ele_accessory.ele_yoke_angle_limit_on1);
            screen_load_prev_sub_page(SUB_ID_ELE_YOKE, 0);
        }
        break;
        case SUB_ID_ELE_NO_YOKE:
            if(s_pan_tilt_flag == 0)
            {
                data_center_write_config_data(SYS_CONFIG_YOKE_PAN, &p_sys_menu_model->ele_accessory.ele_yoke_pan_val1[p_sys_menu_model->ele_accessory.ele_no_access_sel]);
            }
            else if(s_pan_tilt_flag == 1)
            {
                data_center_write_config_data(SYS_CONFIG_YOKE_TILT, &p_sys_menu_model->ele_accessory.ele_yoke_tilt_val1[p_sys_menu_model->ele_accessory.ele_no_access_sel]);
            }
            data_center_write_config_data_no_event(SYS_CONFIG_NOT_ACCESS_SELECT, &p_sys_menu_model->ele_accessory.ele_no_access_sel);
            motor_info.pan_angle = p_sys_menu_model->ele_accessory.ele_yoke_pan_val1[p_sys_menu_model->ele_accessory.ele_no_access_sel];
            motor_info.tilt_angle = p_sys_menu_model->ele_accessory.ele_yoke_tilt_val1[p_sys_menu_model->ele_accessory.ele_no_access_sel];
            data_center_write_sys_info(SYS_INFO_MOTOR, &motor_info);
        break;
		case SUB_ID_ELE_APTURE_NO_YOKE:
            if(s_pan_tilt_flag == 0)
            {
                data_center_write_config_data(SYS_CONFIG_YOKE_PAN, &p_sys_menu_model->ele_accessory.ele_yoke_pan_val1[p_sys_menu_model->ele_accessory.ele_no_access_sel]);
            }
            else if(s_pan_tilt_flag == 1)
            {
                data_center_write_config_data(SYS_CONFIG_YOKE_TILT, &p_sys_menu_model->ele_accessory.ele_yoke_tilt_val1[p_sys_menu_model->ele_accessory.ele_no_access_sel]);
            }
            data_center_write_config_data_no_event(SYS_CONFIG_NOT_ACCESS_SELECT, &p_sys_menu_model->ele_accessory.ele_no_access_sel);
            motor_info.pan_angle = p_sys_menu_model->ele_accessory.ele_yoke_pan_val1[p_sys_menu_model->ele_accessory.ele_no_access_sel];
            motor_info.tilt_angle = p_sys_menu_model->ele_accessory.ele_yoke_tilt_val1[p_sys_menu_model->ele_accessory.ele_no_access_sel];
            data_center_write_sys_info(SYS_INFO_MOTOR, &motor_info);
        break;
		case SUB_ID_ELE_DEMO:
			switch(list_ext1.cur_sel)
			{
				case 0: data_center_write_config_data(SYS_CONFIG_DEMO_STATE, &p_sys_menu_model->ele_accessory.demo_state);break;
				case 1: data_center_write_config_data_no_event(SYS_CONFIG_PAN_DEMO_STATE, &p_sys_menu_model->ele_accessory.pan_demo_state);break;
				case 2: data_center_write_config_data_no_event(SYS_CONFIG_PAN_DEMO_LIMIT, &p_sys_menu_model->ele_accessory.pan_demo_limit);break;
				case 3: data_center_write_config_data_no_event(SYS_CONFIG_TILI_DEMO_STATE, &p_sys_menu_model->ele_accessory.tili_demo_state);break;
				case 4: data_center_write_config_data_no_event(SYS_CONFIG_TILI_DEMO_LIMIT, &p_sys_menu_model->ele_accessory.tili_demo_limit);break;
				case 5: data_center_write_config_data_no_event(SYS_CONFIG_ZOOM_DEMO_STATE, &p_sys_menu_model->ele_accessory.zoom_demo_state);break;
				case 6: data_center_write_config_data_no_event(SYS_CONFIG_ZOOM_DEMO_LIMIT, &p_sys_menu_model->ele_accessory.zoom_demo_limit);break;
				case 7: data_center_write_config_data_no_event(SYS_CONFIG_DEMO_POWER_ON_LIMIT, &p_sys_menu_model->ele_accessory.power_on_state);break;
				default:break;
			}
			if(p_sys_menu_model->ele_accessory.demo_state == 1 && list_ext1.cur_sel != 0)
				data_center_write_config_data(SYS_CONFIG_DEMO_STATE, &p_sys_menu_model->ele_accessory.demo_state);
		break;
        default:break;
    }
}

static const char* select_acc_name_get_str(int8_t x)
{
	uint8_t lang_type = 0;
    const char* name = NULL;
	
	data_center_read_config_data(SYS_CONFIG_LANG, &lang_type);
	if(lang_type > 1)
		lang_type = 1;
	
	if(lang_type == 0)
	{
		name = s_select_an_acc_name_str[x];
	}
	else
	{
		name = s_select_an_acc_name_ch_str[x];
	}

	lv_snprintf(sdb, sizeof(sdb), "%s", name);

    return sdb;
}

static const char* select_acc_name_get_str1(int8_t x)
{
	uint8_t lang_type = 0;
    const char* name = NULL;
	
	data_center_read_config_data(SYS_CONFIG_LANG, &lang_type);
	if(lang_type > 1)
		lang_type = 1;
	
	if(lang_type == 0)
	{
		name = s_select_an_acc_name_str1[x];
	}
	else
	{
		name = s_select_an_acc_name_ch_str1[x];
	}
   lv_snprintf(sdb, sizeof(sdb), "%s", name);

    return sdb;
}

static void select_an_acc_choose_list_user_event(lv_event_t* e, uint8_t index)
{
    lv_event_code_t event = lv_event_get_code(e);
    lv_obj_t* obj = lv_event_get_target(e);
    my_radiobox_ext_t* ext = lv_obj_get_user_data(obj);
    
    if (event == LV_EVENT_RELEASED)
    {
        if(SUB_ID_ELE_YOKE == screen_get_act_spid())   //电动支架和光学附件搭配
        {
            switch(ext->sel)
            {
                case 0:   
				case 1:
				case 2:
				case 3:
				case 4:
				case 5:
				case 6:
					ui_ble_motor_read_angle(5); p_sys_menu_model->ele_accessory.ele_no_access_sel = ext->sel; break;
				case 7:  screen_load_sub_page_with_stack(SUB_ID_ELE_NO_ACCESS_WRING, 0); break;
                case 8:  ui_ble_motor_read_angle(1);p_sys_menu_model->ele_accessory.ele_no_access_sel = ext->sel; break;
                case 9:  ui_ble_motor_read_angle(1);p_sys_menu_model->ele_accessory.ele_no_access_sel = ext->sel; break;
                case 10: ui_ble_motor_read_angle(1);p_sys_menu_model->ele_accessory.ele_no_access_sel = ext->sel; break;
                default:break;
            }
			data_center_write_config_data_no_event(SYS_CONFIG_NOT_ACCESS_SELECT, &p_sys_menu_model->ele_accessory.ele_no_access_sel);
        }  
        else if(SUB_ID_ELE_NO_YOKE_ANGLE_LIMIT_ENTER == screen_get_act_spid()) 
        {      
            p_sys_menu_model->ele_accessory.ele_yoke_pan1[ext->sel+8][0] = pan_value[0];
            p_sys_menu_model->ele_accessory.ele_yoke_pan1[ext->sel+8][1] = pan_value[1];
            p_sys_menu_model->ele_accessory.ele_yoke_tilt1[ext->sel+8][0] = liti_value[0];
            p_sys_menu_model->ele_accessory.ele_yoke_tilt1[ext->sel+8][1] = liti_value[1];
            p_sys_menu_model->ele_accessory.ele_no_access_sel = ext->sel+8;
			p_sys_menu_model->ele_accessory.ele_yoke_angle_limit_on1[ext->sel] = limit_on_value;
            page_event_send(EVENT_DATA_WRITE);
        }
    }
    if(SUB_ID_ELE_YOKE == screen_get_act_spid())   //电动支架和光学附件搭配
    {
        if (event == LV_EVENT_FOCUSED)
        {
            my_list_scroll_refresh(access_scroll_cont, lv_map(ext->sel, 0, ext->max, 0, 100));
        }
        if (event == LV_EVENT_KEY)
        {
            uint32_t key = *(uint32_t *)lv_event_get_param(e);
            if ( LV_ENCODER_KEY_CC == key) //up
            {           
                my_list_scroll_refresh(access_scroll_cont, lv_map(ext->sel, 0, ext->max, 0, 100));
            }
            else if (LV_ENCODER_KEY_CW == key) //down
            {   
                my_list_scroll_refresh(access_scroll_cont, lv_map(ext->sel, 0, ext->max, 0, 100));
            }
            else if (LV_KEY_BACKSPACE == key)
            {
                screen_load_prev_sub_page(SUB_ID_ACCESSORY_LIST, 0);
            }
        }
    }
    else if(SUB_ID_ELE_NO_YOKE_ANGLE_LIMIT_ENTER == screen_get_act_spid()) 
    {
        if (event == LV_EVENT_KEY)
        {
            uint32_t key = *(uint32_t *)lv_event_get_param(e);
            
            if (key == LV_KEY_BACKSPACE)
            {
                screen_turn_prev_page();
            }
            for(uint8_t i = 0; i < access_radiobox_ext1.max; i++)
            {
                lv_obj_t *child2 = lv_obj_get_child(obj, i);
                lv_obj_t *img = lv_obj_get_child(child2, 2);
                if(access_radiobox_ext1.sel == i)
                    lv_img_set_src(img, &ImgRadioboxSel);
                else
                    lv_img_set_src(img, &ImgRadioboxNoSel);   
            }
        }
    }
}

static void page_elect_accessory_event_cb(int event)
{
    switch (event)
    {
        case EVENT_DATA_WRITE:
        {
            accecc_local_write_data();
        }
        break;
        case EVENT_DATA_PULL:
        {
            data_center_read_sys_info(SYS_INFO_MOTOR, &motor_info);
            data_center_read_config_data(SYS_CONFIG_FRESNEL_ANGLE, &p_sys_menu_model->ele_accessory.ele_fresnel_angle);
            data_center_read_config_data(SYS_CONFIG_YOKE_PAN, &p_sys_menu_model->ele_accessory.ele_yoke_pan_val);
            data_center_read_config_data(SYS_CONFIG_YOKE_TILT, &p_sys_menu_model->ele_accessory.ele_yoke_tilt_val);
            data_center_read_config_data(SYS_CONFIG_YOKE_TILT_LIMIT, &p_sys_menu_model->ele_accessory.ele_yoke_tilt);
            data_center_read_config_data(SYS_CONFIG_YOKE_PAN_LIMIT, &p_sys_menu_model->ele_accessory.ele_yoke_pan);
            data_center_read_config_data(SYS_CONFIG_NOT_ACCESS_SELECT, &p_sys_menu_model->ele_accessory.ele_no_access_sel);
			if(p_sys_menu_model->ele_accessory.ele_no_access_sel > 10)
				p_sys_menu_model->ele_accessory.ele_no_access_sel = 10;
            data_center_read_config_data(SYS_CONFIG_ANGLE_LIMIT, &p_sys_menu_model->ele_accessory.ele_yoke_angle_limit_on);
            data_center_read_config_data(SYS_CONFIG_YOKE_NO_TILT_LIMIT, &p_sys_menu_model->ele_accessory.ele_yoke_tilt1);
            data_center_read_config_data(SYS_CONFIG_YOKE_NO_PAN_LIMIT, &p_sys_menu_model->ele_accessory.ele_yoke_pan1);
            data_center_read_config_data(SYS_CONFIG_NO_ANGLE_LIMIT, &p_sys_menu_model->ele_accessory.ele_yoke_angle_limit_on1);
            data_center_read_config_data(SYS_CONFIG_DEMO_STATE, &p_sys_menu_model->ele_accessory.demo_state);
			data_center_read_config_data(SYS_CONFIG_PAN_DEMO_STATE, &p_sys_menu_model->ele_accessory.pan_demo_state);
			data_center_read_config_data(SYS_CONFIG_PAN_DEMO_LIMIT, &p_sys_menu_model->ele_accessory.pan_demo_limit);
			data_center_read_config_data(SYS_CONFIG_TILI_DEMO_STATE, &p_sys_menu_model->ele_accessory.tili_demo_state);
			data_center_read_config_data(SYS_CONFIG_TILI_DEMO_LIMIT, &p_sys_menu_model->ele_accessory.tili_demo_limit);
			data_center_read_config_data(SYS_CONFIG_ZOOM_DEMO_STATE, &p_sys_menu_model->ele_accessory.zoom_demo_state);
			data_center_read_config_data(SYS_CONFIG_ZOOM_DEMO_LIMIT, &p_sys_menu_model->ele_accessory.zoom_demo_limit);
			data_center_read_config_data(SYS_CONFIG_DEMO_POWER_ON_LIMIT, &p_sys_menu_model->ele_accessory.power_on_state);
			
			g_ptUIStateData->ele_accessory.ele_yoke_tilt_lock = 0;
			g_ptUIStateData->ele_accessory.ele_yoke_pan_lock = 0;
			memset(g_ptUIStateData->ele_accessory.ele_yoke_pan_lock1, 0, sizeof(g_ptUIStateData->ele_accessory.ele_yoke_pan_lock1));
			memset(g_ptUIStateData->ele_accessory.ele_yoke_tilt_lock1, 0, sizeof(g_ptUIStateData->ele_accessory.ele_yoke_tilt_lock1));
            p_sys_menu_model->ele_accessory.ele_fresnel_angle = motor_info.fresnel_angle;
            p_sys_menu_model->ele_accessory.ele_yoke_pan_val = motor_info.pan_angle;
            p_sys_menu_model->ele_accessory.ele_yoke_tilt_val = motor_info.tilt_angle;
			ele_fresnel_angle = p_sys_menu_model->ele_accessory.ele_fresnel_angle;
			
        }
        break;
        default:break;
    }
}

static void disp_pannel_init(lv_obj_t* cont)
{
    const char* angle_str;
    adj_btn_t dsc;
	
    angle_label = lv_label_create(cont);
    lv_obj_set_style_text_font(angle_label, Font_ResouceGet(FONT_18_BOLD), 0);
    lv_obj_set_style_text_color(angle_label, lv_color_white(), 0);
    lv_label_set_text(angle_label, Lang_GetStringByID(STRING_ID_INTENSITY));
	lv_obj_set_size(angle_label, 60, 32);
    lv_obj_align(angle_label, LV_ALIGN_TOP_MID, 10, 5);
    if(p_sys_menu_model->ele_accessory.ele_fresnel_angle < 18)
        p_sys_menu_model->ele_accessory.ele_fresnel_angle = 18;
    if(p_sys_menu_model->ele_accessory.ele_fresnel_angle > 50)
        p_sys_menu_model->ele_accessory.ele_fresnel_angle = 50;
    dsc.value = p_sys_menu_model->ele_accessory.ele_fresnel_angle*10;
	btn_list[0].value = dsc.value;
    motor_info.fresnel_angle = p_sys_menu_model->ele_accessory.ele_fresnel_angle;
    data_center_write_sys_info(SYS_INFO_MOTOR, &motor_info);
    angle_str = btn_list[0].get_value_str_cb(&dsc);
    lv_label_set_text(angle_label, angle_str);
    trigger_angle_value = dsc.value/10;
    dec_value = dsc.value/10;
    
    triangle_refresh(cont, dsc.value/10);
    
}

//创建三角形
static void triangle_refresh(lv_obj_t *cont, uint16_t angle)
{
    lv_obj_t* yellow_btn;

    yellow_btn = lv_obj_create(cont);
    lv_obj_set_size(yellow_btn, 80, 150);
    lv_obj_set_style_radius(yellow_btn, 0, 0);
	lv_obj_set_style_border_color(yellow_btn, lv_color_make(254, 255, 241), 0);
    lv_obj_set_style_bg_color(yellow_btn, lv_color_make(254, 255, 241), 0);
    lv_obj_set_style_outline_color(yellow_btn, lv_color_make(254, 255, 241), 0);
    lv_obj_align(yellow_btn, LV_ALIGN_BOTTOM_MID, 0, -5);
    
    for (uint8_t i = 0; i < 79; i++)
    {
        life_line[i] = lv_line_create(cont);
        lv_line_set_points(life_line[i], life_line_point[i], 2);
        lv_obj_clear_flag(life_line[i], LV_OBJ_FLAG_HIDDEN);
        lv_obj_set_style_line_width(life_line[i], 3, 0);
        lv_obj_set_style_line_color(life_line[i], lv_color_make(40, 40, 40), 0);
        lv_obj_set_style_line_rounded(life_line[i], true, 0);
    }

    for (uint8_t i = 0; i < 79; i++)
    {
        right_line[i] = lv_line_create(cont);
        lv_line_set_points(right_line[i], right_line_point[i], 2);
        lv_obj_clear_flag(right_line[i], LV_OBJ_FLAG_HIDDEN);
        lv_obj_set_style_line_width(right_line[i], 3, 0);
        lv_obj_set_style_line_color(right_line[i], lv_color_make(40, 40, 40), 0);
        lv_obj_set_style_line_rounded(right_line[i], true, 0);
    }
    
    for (uint8_t i = 0; i < 17; i++)
    {
        life_anim_line[i] = lv_line_create(cont);
        lv_line_set_points(life_anim_line[i], life_line_anim_point[i], 2);
        lv_obj_clear_flag(life_anim_line[i], LV_OBJ_FLAG_HIDDEN);
        lv_obj_set_style_line_width(life_anim_line[i], 2, 0);
        lv_obj_set_style_line_color(life_anim_line[i], lv_color_make(40, 40, 40), 0);
        lv_obj_set_style_line_rounded(life_anim_line[i], true, 0);
    }

    for (uint8_t i = 0; i < 17; i++)
    {
        right_anim_line[i] = lv_line_create(cont);
        lv_line_set_points(right_anim_line[i], right_line_anim_point[i], 2);
        lv_obj_clear_flag(right_anim_line[i], LV_OBJ_FLAG_HIDDEN);
        lv_obj_set_style_line_width(right_anim_line[i], 2, 0);
        lv_obj_set_style_line_color(right_anim_line[i], lv_color_make(40, 40, 40), 0);
        lv_obj_set_style_line_rounded(right_anim_line[i], true, 0);
    }
    
    triangle_run_upd(angle, 0);
}

//更新三角形
static void triangle_run_upd(int16_t value, uint8_t mode)
{
    uint8_t hiden_btn_hight = value-18;
    uint8_t cont;
    
	if(value >= 20 && value < 49)
		hiden_btn_hight = hiden_btn_hight / 2;
	if(value == 49)
		hiden_btn_hight = 16;
	if(value == 50)
		hiden_btn_hight = 17;
    if(trigger_angle_value < value || trigger_angle_value == value)
    {
        for (uint8_t i = 0; i < hiden_btn_hight; i++)
        {
            lv_obj_add_flag(life_anim_line[16-i], LV_OBJ_FLAG_HIDDEN);
            lv_obj_add_flag(right_anim_line[16-i], LV_OBJ_FLAG_HIDDEN);
        }
    }
    else if(trigger_angle_value > value)
    {
		cont = dec_value - hiden_btn_hight;
		for (uint8_t i = 0; i < 16 - hiden_btn_hight; i++)
        {
            lv_obj_clear_flag(life_anim_line[i], LV_OBJ_FLAG_HIDDEN);
            lv_obj_clear_flag(right_anim_line[i], LV_OBJ_FLAG_HIDDEN);
        }  
    }
    if(mode == 1 && triangle_mode == 0)
        page_event_send(EVENT_DATA_WRITE);
    dec_value = hiden_btn_hight;
    trigger_angle_value = value;
}

static void angle_upd_run_time(void)
{
	char str[10] = {0};
	
    data_center_read_sys_info(SYS_INFO_MOTOR_STATE, &s_motor_state);
    if(s_motor_state.fresnel_stall_state == 1)
    {
        btn_list[0].value = p_sys_menu_model->ele_accessory.ele_fresnel_angle*10;
        return;
    }
    if(btn_list[0].value > 500)
    {
        btn_list[0].value = 500;
    }
    else if(btn_list[0].value < 180)
    {
        btn_list[0].value = 180;
    }
    p_sys_menu_model->ele_accessory.ele_fresnel_angle = (float)btn_list[0].value/10.0f;

    if (angle_label != NULL)
    {
		s_ui_fresnel_angle_count = 0;
		ui_fresnel_get_angle_flag_set(1);
		lv_obj_t* label_val = lv_obj_get_child(btn_list[0].obj, 2);
		data_center_read_sys_info(SYS_INFO_MOTOR, &motor_info);
		motor_info.fresnel_angle = p_sys_menu_model->ele_accessory.ele_fresnel_angle;
		ele_fresnel_angle = motor_info.fresnel_angle;
		triangle_mode = 1;
		sprintf(str, "%.1f°", motor_info.fresnel_angle);
		lv_label_set_text(angle_label, str);
		lv_label_set_text(label_val, str);
		adj_btn3_refresh(btn_list[0].obj, &btn_list[0]);
		data_center_write_sys_info(SYS_INFO_MOTOR, &motor_info);
        triangle_run_upd(btn_list[0].value/10, 1);
    }
}

static void angle_upd_run_cb(void* data)
{
    adj_btn_t* dsc = (adj_btn_t*)data;
	char str[10] = {0};
	
    data_center_read_sys_info(SYS_INFO_MOTOR_STATE, &s_motor_state);
    if(s_motor_state.fresnel_stall_state == 1)
    {
        dsc->value = p_sys_menu_model->ele_accessory.ele_fresnel_angle*10;
        return;
    }
    if(dsc->value > 500)
    {
        dsc->value = 500;
    }
    else if(dsc->value < 180)
    {
        dsc->value = 180;
    }
    p_sys_menu_model->ele_accessory.ele_fresnel_angle = (float)dsc->value/10.0f;

    if (angle_label != NULL)
    {
		s_ui_fresnel_angle_count = 0;
		ui_fresnel_get_angle_flag_set(1);
		lv_obj_t* label_val = lv_obj_get_child(btn_list[0].obj, 2);
		data_center_read_sys_info(SYS_INFO_MOTOR, &motor_info);
		motor_info.fresnel_angle = p_sys_menu_model->ele_accessory.ele_fresnel_angle;
		ele_fresnel_angle = motor_info.fresnel_angle;
		triangle_mode = 0;
		sprintf(str, "%.1f°", motor_info.fresnel_angle);
		lv_label_set_text(angle_label, str);
		lv_label_set_text(label_val, str);
		adj_btn3_refresh(btn_list[0].obj, &btn_list[0]);
		data_center_write_sys_info(SYS_INFO_MOTOR, &motor_info);
        triangle_run_upd(dsc->value/10, 1);
    }
}

static bool angle_assign_value_cb(void* data)
{
    adj_btn_t* dsc = (adj_btn_t*)data;
    
    data_center_read_sys_info(SYS_INFO_MOTOR_STATE, &s_motor_state);
    if(s_motor_state.fresnel_stall_state == 1)
    {
        return false;
    }
    ASSIGN_VALUE_TO_BTN(p_sys_menu_model->ele_accessory.ele_fresnel_angle);
}



//////////*******************电动支架********************//////////////////

static void refresh_yoke_disp(void)
{
    uint8_t value = 0;
    
    LV_IMG_DECLARE(ImgStudioOn)
    LV_IMG_DECLARE(ImgStudioOff)
    if(s_access_flag == 0)
    {
        value = p_sys_menu_model->ele_accessory.ele_yoke_angle_limit_on;
    }
    else if(s_access_flag == 1)
    {
        value = limit_on_value;
    }
    if (value == ELE_YOKE_ANGLE_LIMIT_ON)
    {
        lv_img_set_src(img_onoff, &ImgStudioOn); 
    }
    else
    {
        lv_img_set_src(img_onoff, &ImgStudioOff);                   
    }
}

static lv_obj_t* limit_btn_creat(lv_obj_t* parent, const char* name, uint16_t label_x, uint16_t label_y, btn_ext_t* btn_use_data)
{

    lv_obj_t* label2_panel = lv_label_create(lv_scr_act());
    lv_obj_set_style_text_font(label2_panel, Font_ResouceGet(FONT_14), 0);
    lv_obj_set_style_text_color(label2_panel, lv_color_white(), 0);
    lv_label_set_text(label2_panel, name);
    lv_obj_align(label2_panel, LV_ALIGN_TOP_LEFT, label_x, label_y);

    lv_obj_t* btn2_panel = lv_obj_create(parent);
    lv_obj_add_style(btn2_panel, &style_rect_panel, 0);
    lv_obj_add_style(btn2_panel, &style_common_focued_btn, LV_STATE_FOCUSED);
    lv_obj_set_size(btn2_panel, 190, 50);
    lv_obj_set_user_data(btn2_panel, btn_use_data);
    lv_obj_add_event_cb(btn2_panel, limit_btn_event_cb, LV_EVENT_ALL, NULL);


    lv_obj_t* t_limit1_panel = lv_obj_create(btn2_panel);
    lv_obj_remove_style_all(t_limit1_panel);
    lv_obj_set_size(t_limit1_panel, 57, 32);
    lv_obj_set_style_bg_color(t_limit1_panel, lv_color_white(), 0);
    lv_obj_set_style_bg_opa(t_limit1_panel, LV_OPA_100, 0);
    lv_obj_align(t_limit1_panel, LV_ALIGN_LEFT_MID, 13, 0);


    lv_obj_t* t_limith_panel = lv_obj_create(btn2_panel);
    lv_obj_remove_style_all(t_limith_panel);
    lv_obj_set_size(t_limith_panel, 57, 32);
    lv_obj_set_style_bg_color(t_limith_panel, lv_color_white(), 0);
    lv_obj_set_style_bg_opa(t_limith_panel, LV_OPA_100, 0);
    lv_obj_align(t_limith_panel, LV_ALIGN_RIGHT_MID, -25, 0);

    lv_obj_t* t_limit_l = lv_label_create(t_limit1_panel);
    lv_obj_set_style_text_font(t_limit_l, Font_ResouceGet(FONT_18), 0);
    lv_obj_set_style_text_color(t_limit_l, lv_color_black(), 0);
    lv_obj_set_style_text_color(t_limit_l, RED_THEME_COLOR, LV_STATE_USER_1);
    lv_obj_set_style_bg_opa(t_limit_l, LV_OPA_100, LV_STATE_USER_1);

    lv_label_set_text_fmt(t_limit_l, "%d", btn_use_data->buff[0]);
    lv_obj_center(t_limit_l);

    lv_obj_t* t_limit_h = lv_label_create(t_limith_panel);
    lv_obj_set_style_text_font(t_limit_h, Font_ResouceGet(FONT_18), 0);
    lv_obj_set_style_text_color(t_limit_h, lv_color_black(), 0);
    lv_obj_set_style_text_color(t_limit_h, RED_THEME_COLOR, LV_STATE_USER_1);
    lv_obj_set_style_bg_opa(t_limit_h, LV_OPA_100, LV_STATE_USER_1);
    lv_label_set_text_fmt(t_limit_h, "%d", btn_use_data->buff[1]);
    lv_obj_center(t_limit_h);

    lv_obj_t* symbol_lab = lv_label_create(btn2_panel);
    lv_obj_set_style_text_font(symbol_lab, Font_ResouceGet(FONT_18), 0);
    lv_obj_set_style_text_color(symbol_lab, lv_color_white(), 0);
    lv_label_set_text(symbol_lab, "°");
    lv_obj_align_to(symbol_lab, t_limit1_panel, LV_ALIGN_OUT_RIGHT_TOP, 2, 0);

    lv_obj_t* symbol2_lab = lv_label_create(btn2_panel);
    lv_obj_set_style_text_font(symbol2_lab, Font_ResouceGet(FONT_18), 0);
    lv_obj_set_style_text_color(symbol2_lab, lv_color_white(), 0);
    lv_label_set_text(symbol2_lab, "°");
    lv_obj_align_to(symbol2_lab, t_limith_panel, LV_ALIGN_OUT_RIGHT_TOP, 2, 0);

    lv_obj_t* symbol3_lab = lv_label_create(btn2_panel);
    lv_obj_set_style_text_font(symbol3_lab, Font_ResouceGet(FONT_24), 0);
    lv_obj_set_style_text_color(symbol3_lab, lv_color_white(), 0);
    lv_label_set_text(symbol3_lab, "~");
    lv_obj_align_to(symbol3_lab, t_limit1_panel, LV_ALIGN_OUT_RIGHT_MID, 13, 0);


    return btn2_panel;
}

static void plate_event_cb(lv_event_t* e)
{
    lv_event_code_t event = lv_event_get_code(e);

    lv_obj_t* btn_plate = lv_event_get_target(e);
    lv_group_t* group = lv_obj_get_group(btn_plate);
    shortcut_keys_event_handler(e);

    if (event == LV_EVENT_KEY && lv_group_get_editing(group) == false)
    {
        uint32_t key = *(uint32_t*)lv_event_get_param(e);
        if (LV_KEY_BACKSPACE == key)
        {
            screen_turn_prev_page();
        }
    }
}

static void limit_btn_label_add_state(lv_obj_t* btn, uint8_t index)
{
    lv_obj_t* panel = lv_obj_get_child(btn, index);
    lv_obj_t* label = lv_obj_get_child(panel, -1);
    lv_obj_add_state(label, LV_STATE_USER_1);
}

static void limit_btn_label_clear_state(lv_obj_t* btn, uint8_t index)
{
    lv_obj_t* panel = lv_obj_get_child(btn, index);
    lv_obj_t* label = lv_obj_get_child(panel, -1);
    lv_obj_clear_state(label, LV_STATE_USER_1);
}


static void limit_label_refresh(lv_obj_t* btn, uint8_t index)
{
    lv_obj_t* panel = lv_obj_get_child(btn, index);
    lv_obj_t* label = lv_obj_get_child(panel, -1);
    btn_ext_t* user_data = lv_obj_get_user_data(btn);

    data_center_read_sys_info(SYS_INFO_MOTOR_STATE, &s_motor_state);
    if(user_data->cur_sel_sub == 1)
    {
        if(s_motor_state.tilt_stall_state == 1)
        {
            return;
        }
    }
    else
    {
        if(s_motor_state.pan_stall_state == 1)
        {
            return;
        }
    }
    lv_label_set_text_fmt(label, "%d", user_data->buff[index]);
    if(s_access_flag == 0)
    {
		
    }
    else if(s_access_flag == 1)
    {
        if(user_data->buff == pan_value)
            pan_value[index] = user_data->buff[index];
        else
            liti_value[index] = user_data->buff[index];
    }
    
}


static void limit_btn_event_cb(lv_event_t* e)
{
    lv_obj_t* target = lv_event_get_target(e);
    lv_event_code_t event = lv_event_get_code(e);
    btn_ext_t* user_data = lv_obj_get_user_data(target);
    lv_group_t* group = lv_obj_get_group(target);

    plate_event_cb(e);

    if (event == LV_EVENT_RELEASED)
    {
        if (!lv_group_get_editing(group))
        {
            user_data->cur_sel_sub = 0;
            lv_group_set_editing(group, true);
            limit_btn_label_add_state(target, user_data->cur_sel_sub);
        }
        else
        {
            limit_btn_label_clear_state(target, user_data->cur_sel_sub);
            user_data->cur_sel_sub += 1;
            user_data->cur_sel_sub %= 2;
			if(user_data->cur_sel_sub == 0)
			{
				lv_group_set_editing(group, false);
				return;
			}
            limit_btn_label_add_state(target, user_data->cur_sel_sub);

        }
    }
    else if (event == LV_EVENT_VALUE_CHANGED)
    {
        limit_label_refresh(target, user_data->cur_sel_sub);
    }
    else if (event == LV_EVENT_KEY && lv_group_get_editing(group))
    {
        uint32_t key = *(uint32_t*)lv_event_get_param(e);

        if (key == LV_ENCODER_KEY_CW)
        {
			int16_t max_val;
			if(user_data == &btn_ext[0])
				max_val = user_data->cur_sel_sub == 0 ? user_data->buff[1] : ANGLE_LIMIT_DEFAULT_MAX;
			else
				max_val = user_data->cur_sel_sub == 0 ? user_data->buff[1] : ANGLE_LIMIT_MAX;
            if (user_data->buff[user_data->cur_sel_sub] < max_val)
                user_data->buff[user_data->cur_sel_sub]++;
            limit_label_refresh(target, user_data->cur_sel_sub);
        }
        else if (key == LV_ENCODER_KEY_CC)
        {
			int16_t min_val;
			if(user_data == &btn_ext[0])
				min_val = user_data->cur_sel_sub == 1 ? user_data->buff[0] : ANGLE_LIMIT_DEFAULT_MIN;
			else
				min_val = user_data->cur_sel_sub == 1 ? user_data->buff[0] : ANGLE_LIMIT_MIN;
			
            if (user_data->buff[user_data->cur_sel_sub] > min_val)
                user_data->buff[user_data->cur_sel_sub]--;
            limit_label_refresh(target, user_data->cur_sel_sub);
            //page_set_invalid_type(PAGE_INVALID_TYPE_PORTION);
        }
        else if (key == LV_KEY_BACKSPACE)
        {
            limit_btn_label_clear_state(target, user_data->cur_sel_sub);
            lv_group_set_editing(group, false);
        }
    }
}

static void angle_onoff_event_cb(lv_event_t* e)
{
    lv_event_code_t event = lv_event_get_code(e);

    plate_event_cb(e);

    if (event == LV_EVENT_RELEASED)
    {
        if(s_access_flag == 0)
        {
            p_sys_menu_model->ele_accessory.ele_yoke_angle_limit_on = !p_sys_menu_model->ele_accessory.ele_yoke_angle_limit_on;
        }
        else if(s_access_flag == 1)
        {
            limit_on_value = !limit_on_value;
        }
        refresh_yoke_disp();
    }
}

static void limit_res_event_cb(lv_event_t* e)
{
    plate_event_cb(e);

    lv_event_code_t event = lv_event_get_code(e);

    if (event == LV_EVENT_RELEASED)
    {
		screen_load_sub_page_with_stack(SUN_ID_ELE_YOKE_LIMIT_CONFIRM, 0);
    }
}

static void limit_confirm_event_cb(lv_event_t* e)
{
    plate_event_cb(e);

    lv_event_code_t event = lv_event_get_code(e);

    if (event == LV_EVENT_RELEASED)
    {
		if(SUB_ID_ELE_YOKE_ANGLE_LIMIT == screen_get_act_spid())
		{
			if(s_access_flag == 0)//电动附件
			{
				data_center_write_config_data_no_event(SYS_CONFIG_YOKE_TILT_LIMIT, &p_sys_menu_model->ele_accessory.ele_yoke_tilt);
				data_center_write_config_data_no_event(SYS_CONFIG_YOKE_PAN_LIMIT,  &p_sys_menu_model->ele_accessory.ele_yoke_pan);
				data_center_write_config_data_no_event(SYS_CONFIG_ANGLE_LIMIT,        &p_sys_menu_model->ele_accessory.ele_yoke_angle_limit_on);
				screen_load_prev_sub_page(SUB_ID_ELE_YOKE, 0);
			}
			else   //非电动附件
			{
				uint8_t sel = 0;
				
				sel = p_sys_menu_model->ele_accessory.ele_no_access_sel;
				if(pan_value[0] < 0)
					pan_value[0] = 0;
				p_sys_menu_model->ele_accessory.ele_yoke_pan1[sel][0] = pan_value[0];
				p_sys_menu_model->ele_accessory.ele_yoke_pan1[sel][1] = pan_value[1];
				p_sys_menu_model->ele_accessory.ele_yoke_tilt1[sel][0] = liti_value[0];
				p_sys_menu_model->ele_accessory.ele_yoke_tilt1[sel][1] = liti_value[1];
				p_sys_menu_model->ele_accessory.ele_yoke_angle_limit_on1[sel] = limit_on_value;
				if(p_sys_menu_model->ele_accessory.ele_yoke_angle_limit_on1[sel] == 1)
				{
					if(p_sys_menu_model->ele_accessory.ele_yoke_tilt1[sel][0] < aputure_limit_angle_value[sel].lower_limit_angle)
						p_sys_menu_model->ele_accessory.ele_yoke_tilt1[sel][0] = aputure_limit_angle_value[sel].lower_limit_angle;
					if(p_sys_menu_model->ele_accessory.ele_yoke_tilt1[sel][0] > aputure_limit_angle_value[sel].upper_limit_angle)
						p_sys_menu_model->ele_accessory.ele_yoke_tilt1[sel][0] = aputure_limit_angle_value[sel].upper_limit_angle;
					if(p_sys_menu_model->ele_accessory.ele_yoke_tilt1[sel][1] < aputure_limit_angle_value[sel].lower_limit_angle)
						p_sys_menu_model->ele_accessory.ele_yoke_tilt1[sel][1] = aputure_limit_angle_value[sel].lower_limit_angle;
					if(p_sys_menu_model->ele_accessory.ele_yoke_tilt1[sel][1] > aputure_limit_angle_value[sel].upper_limit_angle)
						p_sys_menu_model->ele_accessory.ele_yoke_tilt1[sel][1] = aputure_limit_angle_value[sel].upper_limit_angle;
				}
				data_center_write_config_data_no_event(SYS_CONFIG_YOKE_NO_TILT_LIMIT, &p_sys_menu_model->ele_accessory.ele_yoke_tilt1);
				data_center_write_config_data_no_event(SYS_CONFIG_YOKE_NO_PAN_LIMIT,  &p_sys_menu_model->ele_accessory.ele_yoke_pan1);
				data_center_write_config_data_no_event(SYS_CONFIG_NO_ANGLE_LIMIT,     &p_sys_menu_model->ele_accessory.ele_yoke_angle_limit_on1);
				screen_load_prev_sub_page(SUB_ID_ELE_APTURE_NO_YOKE, 0);
			}
		}
		else
			screen_load_sub_page_with_stack(SUB_ID_ELE_NO_YOKE_ANGLE_LIMIT_ENTER, 0);
    }
}

static void access_wing_yes_event_cb(lv_event_t* e)
{
    lv_event_code_t event = lv_event_get_code(e);

    if (event == LV_EVENT_RELEASED)
    {
        if(SUB_ID_ELE_NO_ACCESS_WRING == screen_get_act_spid())   
            screen_load_sub_page_with_stack(SUB_ID_ELE_NO_ACCESS_WRING1, 0);
        else if(SUB_ID_ELE_NO_ACCESS_WRING1 == screen_get_act_spid())
            screen_load_sub_page_with_stack(SUB_ID_ELE_NO_YOKE_ANGLE_LIMIT, 0);
    }
	if (event == LV_EVENT_KEY)
    {
        uint32_t key = *(uint32_t*)lv_event_get_param(e);
        if (LV_KEY_BACKSPACE == key)
        {
            screen_load_sub_page(SUB_ID_ELE_YOKE, access_radiobox_ext.sel);
        }
    }
}

static void access_wing_no_event_cb(lv_event_t* e)
{
//    plate_event_cb(e);

    lv_event_code_t event = lv_event_get_code(e);

    if (event == LV_EVENT_RELEASED)
    {
        screen_load_sub_page(SUB_ID_ELE_YOKE, 0);
    }
	if (event == LV_EVENT_KEY)
    {
        uint32_t key = *(uint32_t*)lv_event_get_param(e);
        if (LV_KEY_BACKSPACE == key)
        {
            screen_load_sub_page(SUB_ID_ELE_YOKE, access_radiobox_ext.sel);
        }
    }
}



/*****************重置确定界面******************************/
static void angle_reset_event_cb(my_msgbox_ext_t* ext)
{
    if (ext->answer == MSGBOX_ID_YES)
    {
		if(s_access_info.fresnel_state == 1|| s_access_info.optical_20_access_state == 1)
		{
			p_sys_menu_model->ele_accessory.ele_yoke_tilt[0] = -120;
			p_sys_menu_model->ele_accessory.ele_yoke_tilt[1] = 120;
			p_sys_menu_model->ele_accessory.ele_yoke_pan[0] = ANGLE_LIMIT_MIN;
			p_sys_menu_model->ele_accessory.ele_yoke_pan[1] = ANGLE_LIMIT_MAX;
			liti_value[0] = -120;
			liti_value[1] = 120;
			pan_value[0] = ANGLE_LIMIT_MIN;
			pan_value[1] = ANGLE_LIMIT_MAX;
			data_center_write_config_data(SYS_CONFIG_YOKE_TILT_LIMIT, &p_sys_menu_model->ele_accessory.ele_yoke_tilt);
            data_center_write_config_data(SYS_CONFIG_YOKE_PAN_LIMIT, &p_sys_menu_model->ele_accessory.ele_yoke_pan);
		}
		else if(s_access_info.optical_30_access_state == 1|| s_access_info.optical_50_access_state == 1 || s_access_info.adapter_ring == 1)
		{
			p_sys_menu_model->ele_accessory.ele_yoke_tilt[0] = ANGLE_LIMIT_DEFAULT_MIN;
			p_sys_menu_model->ele_accessory.ele_yoke_tilt[1] = ANGLE_LIMIT_DEFAULT_MAX;
			p_sys_menu_model->ele_accessory.ele_yoke_pan[0] = ANGLE_LIMIT_MIN;
			p_sys_menu_model->ele_accessory.ele_yoke_pan[1] = ANGLE_LIMIT_MAX;
			liti_value[0] = ANGLE_LIMIT_DEFAULT_MIN;
			liti_value[1] = ANGLE_LIMIT_DEFAULT_MAX;
			pan_value[0] = ANGLE_LIMIT_MIN;
			pan_value[1] = ANGLE_LIMIT_MAX;
			data_center_write_config_data(SYS_CONFIG_YOKE_TILT_LIMIT, &p_sys_menu_model->ele_accessory.ele_yoke_tilt);
            data_center_write_config_data(SYS_CONFIG_YOKE_PAN_LIMIT, &p_sys_menu_model->ele_accessory.ele_yoke_pan);
		}
		else
		{
			data_center_read_config_data(SYS_CONFIG_NOT_ACCESS_SELECT, &p_sys_menu_model->ele_accessory.ele_no_access_sel);
			p_sys_menu_model->ele_accessory.ele_yoke_tilt1[p_sys_menu_model->ele_accessory.ele_no_access_sel][0] = ANGLE_LIMIT_DEFAULT_MIN;
			p_sys_menu_model->ele_accessory.ele_yoke_tilt1[p_sys_menu_model->ele_accessory.ele_no_access_sel][1] = ANGLE_LIMIT_DEFAULT_MAX;
			if(p_sys_menu_model->ele_accessory.ele_no_access_sel <= 6)
			{
				if(p_sys_menu_model->ele_accessory.ele_yoke_tilt1[p_sys_menu_model->ele_accessory.ele_no_access_sel][0] < aputure_limit_angle_value[p_sys_menu_model->ele_accessory.ele_no_access_sel].lower_limit_angle)
				{
					p_sys_menu_model->ele_accessory.ele_yoke_tilt1[p_sys_menu_model->ele_accessory.ele_no_access_sel][0] = aputure_limit_angle_value[p_sys_menu_model->ele_accessory.ele_no_access_sel].lower_limit_angle;
				}
				if(p_sys_menu_model->ele_accessory.ele_yoke_tilt1[p_sys_menu_model->ele_accessory.ele_no_access_sel][0] > aputure_limit_angle_value[p_sys_menu_model->ele_accessory.ele_no_access_sel].upper_limit_angle)
				{
					p_sys_menu_model->ele_accessory.ele_yoke_tilt1[p_sys_menu_model->ele_accessory.ele_no_access_sel][0] = aputure_limit_angle_value[p_sys_menu_model->ele_accessory.ele_no_access_sel].upper_limit_angle;
				}
				if(p_sys_menu_model->ele_accessory.ele_yoke_tilt1[p_sys_menu_model->ele_accessory.ele_no_access_sel][1] > aputure_limit_angle_value[p_sys_menu_model->ele_accessory.ele_no_access_sel].upper_limit_angle)
				{
					p_sys_menu_model->ele_accessory.ele_yoke_tilt1[p_sys_menu_model->ele_accessory.ele_no_access_sel][1] = aputure_limit_angle_value[p_sys_menu_model->ele_accessory.ele_no_access_sel].upper_limit_angle;
				}
				if(p_sys_menu_model->ele_accessory.ele_yoke_tilt1[p_sys_menu_model->ele_accessory.ele_no_access_sel][1] < aputure_limit_angle_value[p_sys_menu_model->ele_accessory.ele_no_access_sel].lower_limit_angle)
				{
					p_sys_menu_model->ele_accessory.ele_yoke_tilt1[p_sys_menu_model->ele_accessory.ele_no_access_sel][1] = aputure_limit_angle_value[p_sys_menu_model->ele_accessory.ele_no_access_sel].lower_limit_angle;
				}
			}
			p_sys_menu_model->ele_accessory.ele_yoke_pan1[p_sys_menu_model->ele_accessory.ele_no_access_sel][0] = ANGLE_LIMIT_MIN;
			p_sys_menu_model->ele_accessory.ele_yoke_pan1[p_sys_menu_model->ele_accessory.ele_no_access_sel][1] = ANGLE_LIMIT_MAX;
			liti_value[0] = p_sys_menu_model->ele_accessory.ele_yoke_tilt1[p_sys_menu_model->ele_accessory.ele_no_access_sel][0];
			liti_value[1] = p_sys_menu_model->ele_accessory.ele_yoke_tilt1[p_sys_menu_model->ele_accessory.ele_no_access_sel][1];
			pan_value[0] = ANGLE_LIMIT_MIN;
			pan_value[1] = ANGLE_LIMIT_MAX;
			data_center_write_config_data_no_event(SYS_CONFIG_YOKE_NO_TILT_LIMIT, &p_sys_menu_model->ele_accessory.ele_yoke_tilt1);
			data_center_write_config_data_no_event(SYS_CONFIG_YOKE_NO_PAN_LIMIT, &p_sys_menu_model->ele_accessory.ele_yoke_pan1);
		}
        screen_load_sub_page(SUB_ID_ELE_YOKE_ANGLE_LIMIT, 0);
        UI_PRINTF("Angle Reset Begin...........\r\n");
    }
    else if (ext->answer == MSGBOX_ID_NO)
    {
        //执行返回setting页面
        UI_PRINTF("Do not do Angle Reset...........\r\n");
        screen_turn_prev_page();
    }
}


/********************电动支架界面********************/
static lv_obj_t* btn_panel_grid_creat(lv_coord_t w, lv_coord_t h, lv_coord_t* col, lv_coord_t* row , lv_coord_t left_offset)
{

    lv_obj_t* btn_panel = lv_obj_create(lv_scr_act());
    lv_obj_remove_style_all(btn_panel);
    lv_obj_set_size(btn_panel, w, h);
    lv_obj_set_style_bg_color(btn_panel, lv_color_make(0, 0, 0), 0);
    lv_obj_set_style_text_font(btn_panel, Font_ResouceGet(FONT_18), 0);
    lv_obj_set_style_text_color(btn_panel, lv_color_white(), 0);
    lv_obj_set_style_border_width(btn_panel, 0, 0);

    lv_obj_set_style_pad_all(btn_panel, 0, 0);
    lv_obj_set_style_pad_left(btn_panel, left_offset, 0);
    lv_obj_set_style_pad_top(btn_panel, 10, 0);
    lv_obj_set_style_pad_row(btn_panel, 15, 0);
    lv_obj_set_style_pad_column(btn_panel, 7, 0);
    lv_obj_set_grid_dsc_array(btn_panel, col, row);

    return btn_panel;
}

static void get_yoke_btn_str(uint8_t type, str_dsc_t* pdsc)
{
    pdsc->font = Font_ResouceGet(FONT_18_BOLD);
    pdsc->img = NULL;
    pdsc->str = " ";
	switch (type)
    {
     case 0:
         pdsc->str = Lang_GetStringByID(STRING_ID_YOKE_HORIZON);
        break;
    case 1:
        pdsc->str = Lang_GetStringByID(STRING_ID_YOKE_PITCH);
        break;
    case 2:
        pdsc->str  = Lang_GetStringByID(STRING_ID_ORIGIN_RESET);
        break;
    case 3:
        pdsc->str  = Lang_GetStringByID(STRING_ID_MOTION_LIMIT);
        break;
    default:
        break;
 
    }
}

static void refresh_lock_img(lv_obj_t* img , uint8_t lock)
{
    LV_IMG_DECLARE(ImgLock)
     LV_IMG_DECLARE(ImgUnlock)

    lv_img_set_zoom(img, 150);
    if (lock)
    {
        lv_img_set_src(img, &ImgLock);
    }
    else
    {
        lv_img_set_src(img, &ImgUnlock);
    }
}

static void refresh_adj_value_label(lv_obj_t* label , float val)
{
    char tmp[10] = {0};
	sprintf(tmp, "%.1f", val);
    lv_label_set_text(label, tmp);
    lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);
}

static void adj_btn_label_add_state(lv_obj_t* btn)
{
    lv_obj_t* label = lv_obj_get_child(btn, 1);
    lv_obj_add_state(label, LV_STATE_USER_1);
}

static void adj_btn_label_clear_state(lv_obj_t* btn)
{
    lv_obj_t* label = lv_obj_get_child(btn, 1);
    lv_obj_clear_state(label, LV_STATE_USER_1);
}

static void yoke_pan_event_cb(lv_event_t* e)
{
    uint8_t* lock = NULL ;
    float* label_val = NULL;
    int16_t label_val_max;
    int16_t label_val_min;
    float tmp = 0;
    uint8_t spid = screen_get_act_spid();
	struct sys_info_motor_state  motor_state;
	
    data_center_read_sys_info(SYS_INFO_MOTOR_STATE, &motor_state);
    lv_event_code_t event = lv_event_get_code(e);
    lv_obj_t* target = lv_event_get_target(e);
    lv_group_t* group = lv_obj_get_group(target);
    uint8_t index = lv_obj_get_child_id(target);

    shortcut_keys_event_handler(e);
//    backspace_key_event_handler(e);

    if (index == 0)
    {
        s_pan_tilt_flag = 0;
        if(spid == SUB_ID_ELE_YOKE)
        {
            lock = &p_sys_menu_model->ele_accessory.ele_yoke_pan_lock;
            label_val = &p_sys_menu_model->ele_accessory.ele_yoke_pan_val;
            label_val_min = p_sys_menu_model->ele_accessory.ele_yoke_pan[0];
            label_val_max = p_sys_menu_model->ele_accessory.ele_yoke_pan[1];
        }
        else if(spid == SUB_ID_ELE_NO_YOKE || spid == SUB_ID_ELE_APTURE_NO_YOKE)
        {
            lock = &p_sys_menu_model->ele_accessory.ele_yoke_pan_lock1[p_sys_menu_model->ele_accessory.ele_no_access_sel];
            label_val = &p_sys_menu_model->ele_accessory.ele_yoke_pan_val1[p_sys_menu_model->ele_accessory.ele_no_access_sel];
            label_val_min = p_sys_menu_model->ele_accessory.ele_yoke_pan1[p_sys_menu_model->ele_accessory.ele_no_access_sel][0];
            label_val_max = p_sys_menu_model->ele_accessory.ele_yoke_pan1[p_sys_menu_model->ele_accessory.ele_no_access_sel][1];
        }
    }
    else if (index == 1)
    {
        s_pan_tilt_flag = 1;
        if(spid == SUB_ID_ELE_YOKE)
        {
            lock = &p_sys_menu_model->ele_accessory.ele_yoke_tilt_lock;
            label_val = &p_sys_menu_model->ele_accessory.ele_yoke_tilt_val;
            label_val_min = p_sys_menu_model->ele_accessory.ele_yoke_tilt[0];
            label_val_max = p_sys_menu_model->ele_accessory.ele_yoke_tilt[1];
        }
        else if(spid == SUB_ID_ELE_NO_YOKE || spid == SUB_ID_ELE_APTURE_NO_YOKE)
        {
            lock = &p_sys_menu_model->ele_accessory.ele_yoke_tilt_lock1[p_sys_menu_model->ele_accessory.ele_no_access_sel];
            label_val = &p_sys_menu_model->ele_accessory.ele_yoke_tilt_val1[p_sys_menu_model->ele_accessory.ele_no_access_sel];
            label_val_min = p_sys_menu_model->ele_accessory.ele_yoke_tilt1[p_sys_menu_model->ele_accessory.ele_no_access_sel][0];
            label_val_max = p_sys_menu_model->ele_accessory.ele_yoke_tilt1[p_sys_menu_model->ele_accessory.ele_no_access_sel][1];
        }
    }

    if (event == LV_EVENT_RELEASED && !(*lock))
    {
        if (!lv_group_get_editing(group))
        {
            lv_group_set_editing(group, true);
            adj_btn_label_add_state(target);
        }
        else
        {
            adj_btn_label_clear_state(target);
            lv_group_set_editing(group, false);
        }
    }
    else if (event == LV_EVENT_KEY)
    {
        uint32_t key = *(uint32_t*)lv_event_get_param(e);
        lv_obj_t* label = lv_obj_get_child(target, 1);
		
							
        if (key == LV_ENCODER_KEY_CW && (!(*lock)) && lv_group_get_editing(group))
        {
			if(motor_state.tilt_stall_state == 1 || motor_state.pan_stall_state == 1)
				return;
            tmp = *label_val;
            tmp += 0.5 * motor_enc_get_step(enc_get_step_index());
            if (tmp > label_val_max)
                tmp = label_val_max;
			if (tmp < label_val_min)
                tmp = label_val_min;
            *label_val = tmp;
            refresh_adj_value_label(label, *label_val);
            page_event_send(EVENT_DATA_WRITE);
        }
        else if (key == LV_ENCODER_KEY_CC && (!(*lock)) && lv_group_get_editing(group))
        {
			if(motor_state.tilt_stall_state == 1 || motor_state.pan_stall_state == 1)
				return;
            tmp = *label_val;
            tmp -= 0.5 * motor_enc_get_step(enc_get_step_index());
            if (tmp < label_val_min)
                tmp = label_val_min;
			if (tmp > label_val_max)
                tmp = label_val_max;
            *label_val = tmp;
            refresh_adj_value_label(label, *label_val);
            page_event_send(EVENT_DATA_WRITE);
        }
        else if (key == LV_KEY_BACKSPACE && lv_group_get_editing(group))
        {
            adj_btn_label_clear_state(target);
            lv_group_set_editing(group, false);
        }
        else if (key == LV_KEY_BACKSPACE)
        {
            if(SUB_ID_ELE_NO_YOKE == screen_get_act_spid())
                screen_load_sub_page(SUB_ID_ELE_YOKE, p_sys_menu_model->ele_accessory.ele_no_access_sel);
            else if(SUB_ID_ELE_YOKE == screen_get_act_spid())
                screen_load_prev_sub_page(SUB_ID_ACCESSORY_LIST, 0);
			else if(SUB_ID_ELE_APTURE_NO_YOKE == screen_get_act_spid())
				screen_load_sub_page(SUB_ID_ELE_YOKE, p_sys_menu_model->ele_accessory.ele_no_access_sel);
        }
    }
    else if (event == LV_EVENT_LONG_PRESSED)
    {
        (*lock) = !(*lock);
        lv_obj_t* img = lv_obj_get_child(target, 2);
        refresh_lock_img(img, (*lock));
        adj_btn_label_clear_state(target);
        lv_group_set_editing(group, false);
    }
}

static void fre_reset_btn_event_cb(lv_event_t* e)
{
    lv_event_code_t event = lv_event_get_code(e);
    lv_obj_t* target = lv_event_get_target(e);
    uint8_t index = lv_obj_get_child_id(target);

    shortcut_keys_event_handler(e);

    if (event == LV_EVENT_RELEASED)
    {
        screen_load_sub_page_with_stack(SUB_ID_ELE_YOKE_RESET_CONFIRM, 0);
    }
    else if (event == LV_EVENT_KEY)
    {
        uint32_t key = *(uint32_t*)lv_event_get_param(e);
        if (key == LV_KEY_BACKSPACE)
        {
            screen_load_prev_sub_page(SUB_ID_ACCESSORY_LIST, 0);
        }
    }
}

static void yoke_btn_event_cb(lv_event_t* e)
{
    lv_event_code_t event = lv_event_get_code(e);
    lv_obj_t* target = lv_event_get_target(e);
    uint8_t index = lv_obj_get_child_id(target);

    shortcut_keys_event_handler(e);
    backspace_key_event_handler(e);

    if (event == LV_EVENT_RELEASED)
    {
        index = index == 0 ? 4 : index;
        screen_load_sub_page_with_stack(SUB_ID_ELE_YOKE_RESET_CONFIRM + index - 2, 0);
    }
    else if (event == LV_EVENT_KEY)
    {
        uint32_t key = *(uint32_t*)lv_event_get_param(e);
        if (key == LV_KEY_BACKSPACE)
        {
            screen_load_prev_sub_page(SUB_ID_ELE_YOKE, 0);
        }
    }
}

static void access_btn_eent_cb(lv_event_t* e)
{
    lv_event_code_t event = lv_event_get_code(e);
    lv_obj_t* target = lv_event_get_target(e);
    uint8_t index = lv_obj_get_child_id(target);

    shortcut_keys_event_handler(e);
	if( event == LV_EVENT_KEY)
    {
        uint32_t key = *(uint32_t *)lv_event_get_param(e);
                
        if (LV_KEY_BACKSPACE == key)
        {
			if(SUB_ID_ELE_YOKE == screen_get_act_spid() || SUB_ID_ELE_OPTICAL_ACCESS == screen_get_act_spid() || SUB_ID_ELE_FRESNEL == screen_get_act_spid()
			   || SUB_ID_ELE_DEMO == screen_get_act_spid())
				screen_load_prev_sub_page(SUB_ID_ACCESSORY_LIST, 0);
		}
	}
    if (event == LV_EVENT_RELEASED)
    {
        screen_load_prev_sub_page(SUB_ID_ACCESSORY_LIST, 0);
    }
}

static void block_cal_event_cb(my_msgbox_ext_t* ext)
{
    if (ext->answer == MSGBOX_ID_YES)
    {
        p_sys_menu_model->ele_accessory.block_calib = 1;
		screen_turn_prev_page();
        UI_PRINTF("Angle Reset Begin...........\r\n");
    }
    else if (ext->answer == MSGBOX_ID_NO)
    {
        //执行返回setting页面
        UI_PRINTF("Do not do Angle Reset...........\r\n");
        screen_turn_prev_page();
    }
}

static void origin_rst_event_cb(my_msgbox_ext_t* ext)
{
    if (ext->answer == MSGBOX_ID_YES)
    {
        p_sys_menu_model->ele_accessory.rst_origin = 1;
		if(motor_reset_flag == 1)
		{
			ele_reset_flag = 1;
			ui_motor_reset_status_set(2);
			ui_set_frsenel_reset();
			data_center_read_sys_info(SYS_INFO_MOTOR, &motor_info);
			motor_info.fresnel_angle = 50;
			data_center_write_sys_info(SYS_INFO_MOTOR, &motor_info);
			p_sys_menu_model->ele_accessory.ele_fresnel_angle = 50;
			data_center_write_config_data_no_event(SYS_CONFIG_FRESNEL_ANGLE, &p_sys_menu_model->ele_accessory.ele_fresnel_angle);
		}
		else
		{
			ele_reset_flag = 2;
			ui_motor_reset_status_set(1);
			ui_set_motor_reset();
			if(s_access_info.fresnel_state == 1|| s_access_info.optical_20_access_state == 1)
			{
				p_sys_menu_model->ele_accessory.ele_yoke_tilt[0] = -120;
				p_sys_menu_model->ele_accessory.ele_yoke_tilt[1] = 120;
				p_sys_menu_model->ele_accessory.ele_yoke_pan[0] = ANGLE_LIMIT_MIN;
				p_sys_menu_model->ele_accessory.ele_yoke_pan[1] = ANGLE_LIMIT_MAX;
				p_sys_menu_model->ele_accessory.ele_yoke_pan_val = 0.0;
				p_sys_menu_model->ele_accessory.ele_yoke_tilt_val = 90.0;
				data_center_write_config_data_no_event(SYS_CONFIG_YOKE_PAN, &p_sys_menu_model->ele_accessory.ele_yoke_pan_val);
				data_center_write_config_data_no_event(SYS_CONFIG_YOKE_TILT, &p_sys_menu_model->ele_accessory.ele_yoke_tilt_val);
			}
			else if(s_access_info.optical_30_access_state == 1|| s_access_info.optical_50_access_state == 1 || s_access_info.adapter_ring == 1)
			{
				p_sys_menu_model->ele_accessory.ele_yoke_tilt[0] = ANGLE_LIMIT_DEFAULT_MIN;
				p_sys_menu_model->ele_accessory.ele_yoke_tilt[1] = ANGLE_LIMIT_DEFAULT_MAX;
				p_sys_menu_model->ele_accessory.ele_yoke_pan[0] = ANGLE_LIMIT_MIN;
				p_sys_menu_model->ele_accessory.ele_yoke_pan[1] = ANGLE_LIMIT_MAX;
				p_sys_menu_model->ele_accessory.ele_yoke_pan_val = 0.0;
				p_sys_menu_model->ele_accessory.ele_yoke_tilt_val = 90.0;
				data_center_write_config_data_no_event(SYS_CONFIG_YOKE_PAN, &p_sys_menu_model->ele_accessory.ele_yoke_pan_val);
				data_center_write_config_data_no_event(SYS_CONFIG_YOKE_TILT, &p_sys_menu_model->ele_accessory.ele_yoke_tilt_val);
			}
			else
			{
				data_center_read_config_data(SYS_CONFIG_NOT_ACCESS_SELECT, &p_sys_menu_model->ele_accessory.ele_no_access_sel);
				p_sys_menu_model->ele_accessory.ele_yoke_tilt1[p_sys_menu_model->ele_accessory.ele_no_access_sel][0] = ANGLE_LIMIT_DEFAULT_MIN;
				p_sys_menu_model->ele_accessory.ele_yoke_tilt1[p_sys_menu_model->ele_accessory.ele_no_access_sel][1] = ANGLE_LIMIT_DEFAULT_MAX;
				p_sys_menu_model->ele_accessory.ele_yoke_pan1[p_sys_menu_model->ele_accessory.ele_no_access_sel][0] = ANGLE_LIMIT_MIN;
				p_sys_menu_model->ele_accessory.ele_yoke_pan1[p_sys_menu_model->ele_accessory.ele_no_access_sel][1] = ANGLE_LIMIT_MAX;
				p_sys_menu_model->ele_accessory.ele_yoke_pan_val1[p_sys_menu_model->ele_accessory.ele_no_access_sel] = 0.0;
				p_sys_menu_model->ele_accessory.ele_yoke_tilt_val1[p_sys_menu_model->ele_accessory.ele_no_access_sel] = 90.0;
				p_sys_menu_model->ele_accessory.ele_yoke_pan_val = 0.0;
				p_sys_menu_model->ele_accessory.ele_yoke_tilt_val = 90.0;
				data_center_write_config_data_no_event(SYS_CONFIG_YOKE_PAN, &p_sys_menu_model->ele_accessory.ele_yoke_pan_val);
				data_center_write_config_data_no_event(SYS_CONFIG_YOKE_TILT, &p_sys_menu_model->ele_accessory.ele_yoke_tilt_val);
				data_center_write_config_data_no_event(SYS_CONFIG_YOKE_NO_TILT_LIMIT, &p_sys_menu_model->ele_accessory.ele_yoke_tilt1);
				data_center_write_config_data_no_event(SYS_CONFIG_YOKE_NO_PAN_LIMIT, &p_sys_menu_model->ele_accessory.ele_yoke_pan1);
				data_center_write_config_data_no_event(SYS_CONFIG_YOKE_NO_PAN, &p_sys_menu_model->ele_accessory.ele_yoke_pan_val1);
				data_center_write_config_data_no_event(SYS_CONFIG_YOKE_NO_TILT, &p_sys_menu_model->ele_accessory.ele_yoke_tilt_val1);
			}
			data_center_read_sys_info(SYS_INFO_MOTOR, &motor_info);
			motor_info.pan_angle = 0;
			motor_info.tilt_angle = 90.0;
			data_center_write_sys_info(SYS_INFO_MOTOR, &motor_info);
		}
		screen_load_sub_page_with_stack(SUB_ID_ELE_RESET, 0);
        UI_PRINTF("Angle Reset Begin...........\r\n");
    }
    else if (ext->answer == MSGBOX_ID_NO)
    {
        //执行返回setting页面
        UI_PRINTF("Do not do Angle Reset...........\r\n");
        screen_turn_prev_page();
    }
}

static void yoke_info_creat(lv_obj_t* parent ,int16_t min_val,int16_t max_val,const void* img_src , lv_coord_t x_ofs ,char * string)
{
    lv_obj_t* img = lv_img_create(parent);
    lv_obj_set_style_img_recolor(img, lv_color_white(), 0);
    lv_obj_set_style_img_recolor_opa(img, LV_OPA_100, 0);
    lv_obj_align(img, LV_ALIGN_BOTTOM_LEFT, x_ofs, -50);
	lv_img_set_zoom(img, 640);
    lv_img_set_src(img, img_src);
	
    lv_obj_t* label_1p = lv_label_create(parent);
    lv_obj_align_to(label_1p, img, LV_ALIGN_OUT_BOTTOM_LEFT, -45, 30);
    lv_obj_set_style_text_font(label_1p, Font_ResouceGet(FONT_14), 0);
    lv_label_set_text(label_1p, string);

    lv_obj_t* label_2p = lv_label_create(parent);
    lv_obj_set_style_text_font(label_2p, Font_ResouceGet(FONT_14), 0);
    lv_obj_align_to(label_2p, label_1p, LV_ALIGN_OUT_RIGHT_MID, 0, 0);

    char tmp[20];
    lv_snprintf(tmp, sizeof(tmp), ": %d%°~%d%°", min_val, max_val);
    lv_label_set_text(label_2p, tmp);
}

static void switch_btn_event_cb(lv_event_t* e)
{
    lv_event_code_t event = lv_event_get_code(e);
    lv_obj_t* btn = lv_event_get_target(e);
	uint8_t cur_sel =0;
	
    shortcut_keys_event_handler(e);

	lv_obj_t *demo_btn = NULL;
				
	demo_btn = lv_group_get_focused(indev_group);
	for(uint8_t i = 0; i < list_ext1.max; i++)
	{
		if(demo_btn == demo_obj_num[i])
			my_list_scroll_refresh(demo_scroll_cont, lv_map(i, 0, list_ext1.max, 0, 100));
	}

    if (event == LV_EVENT_RELEASED)
    {
		uint8_t state;
		lv_obj_t *paren = lv_obj_get_parent(btn);
		for(uint8_t i = 0; i < list_ext1.max; i++)
		{
			lv_obj_t *chlid = lv_obj_get_child(paren, i);
			if(chlid == btn)
			{
				cur_sel = i;
				break;
			}
		}
		list_ext1.cur_sel = cur_sel;
		switch(cur_sel)
		{
			case 0: 
				state = !p_sys_menu_model->ele_accessory.demo_state;
				p_sys_menu_model->ele_accessory.demo_state = state;
			break;
			case 1: 
				state = !p_sys_menu_model->ele_accessory.pan_demo_state;
				p_sys_menu_model->ele_accessory.pan_demo_state = state;
			break;
			case 3: 
				state = !p_sys_menu_model->ele_accessory.tili_demo_state;
				p_sys_menu_model->ele_accessory.tili_demo_state = state;
			break;
			case 5: 
				state = !p_sys_menu_model->ele_accessory.zoom_demo_state;
				p_sys_menu_model->ele_accessory.zoom_demo_state = state;
			break;
			case 7: 
				state = !p_sys_menu_model->ele_accessory.power_on_state;
				p_sys_menu_model->ele_accessory.power_on_state = state;
			break;
			default:break;
		}
		page_event_send(EVENT_DATA_WRITE);
		if(cur_sel == 0 || cur_sel == 7)
			list_state_btn_refresh(btn, state);
		else
			list_switch_btn_refresh(btn, state);     
    }
    else if (event == LV_EVENT_KEY)
    {
        uint32_t key = *(uint32_t*)lv_event_get_param(e);
        if (key == LV_KEY_BACKSPACE)
        {
            screen_turn_prev_page();
        }
    }
}

static void demo_limit_btn_event_cb(lv_event_t* e)
{
    lv_event_code_t event = lv_event_get_code(e);
    lv_obj_t* btn = lv_event_get_target(e);
	lv_group_t *group = lv_obj_get_group(btn);
	my_list_ext_t* user_data = lv_obj_get_user_data(btn);
	
    shortcut_keys_event_handler(e);

    if (event == LV_EVENT_RELEASED)
    {
		if(true == lv_group_get_editing(group))
		{
			user_data->cur_sel++;
			if(user_data->cur_sel >= 3)
			{
				user_data->cur_sel = 0;
				lv_group_set_editing(group, false);
			}
		}
		else
		{
			lv_group_set_editing(group, true);
			user_data->cur_sel = 1;
			
		}
		motor_limit_state_refresh(btn, user_data->cur_sel);
    }
    else if (event == LV_EVENT_KEY)
    {
        uint32_t key = *(uint32_t*)lv_event_get_param(e);
		uint8_t cur_sel = 0;
		
		if(true == lv_group_get_editing(group))
		{
			lv_obj_t *paren = lv_obj_get_parent(btn);
			for(uint8_t i = 0; i < list_ext1.max; i++)
			{
				lv_obj_t *chlid = lv_obj_get_child(paren, i);
				if(chlid == btn)
				{
					cur_sel = i;
					break;
				}
			}
			list_ext1.cur_sel = cur_sel;
			switch(cur_sel)
			{
				case 2: 
					if (key == LV_KEY_RIGHT)
					{
						p_sys_menu_model->ele_accessory.pan_demo_limit[user_data->cur_sel-1]+=1;
						if(p_sys_menu_model->ele_accessory.pan_demo_limit[user_data->cur_sel-1] >= ANGLE_LIMIT_MAX)
							p_sys_menu_model->ele_accessory.pan_demo_limit[user_data->cur_sel-1] = ANGLE_LIMIT_MAX;
					}
					else if(key == LV_KEY_LEFT)
					{
						p_sys_menu_model->ele_accessory.pan_demo_limit[user_data->cur_sel-1]-=1;
						if(p_sys_menu_model->ele_accessory.pan_demo_limit[user_data->cur_sel-1] < ANGLE_LIMIT_MIN)
							p_sys_menu_model->ele_accessory.pan_demo_limit[user_data->cur_sel-1] = ANGLE_LIMIT_MIN;
					}
					motor_limit_text_refresh(btn, p_sys_menu_model->ele_accessory.pan_demo_limit[user_data->cur_sel-1], user_data->cur_sel);
					page_event_send(EVENT_DATA_WRITE);
				break;
				case 4: 
					if (key == LV_KEY_RIGHT)
					{
						p_sys_menu_model->ele_accessory.tili_demo_limit[user_data->cur_sel-1]+=1;
						if(p_sys_menu_model->ele_accessory.tili_demo_limit[user_data->cur_sel-1] >= ANGLE_LIMIT_DEFAULT_MAX)
							p_sys_menu_model->ele_accessory.tili_demo_limit[user_data->cur_sel-1] = ANGLE_LIMIT_DEFAULT_MAX;
					}
					else if(key == LV_KEY_LEFT)
					{
						p_sys_menu_model->ele_accessory.tili_demo_limit[user_data->cur_sel-1]-=1;
						if(p_sys_menu_model->ele_accessory.tili_demo_limit[user_data->cur_sel-1] < ANGLE_LIMIT_DEFAULT_MIN)
							p_sys_menu_model->ele_accessory.tili_demo_limit[user_data->cur_sel-1] = ANGLE_LIMIT_DEFAULT_MIN;
					}
					if(s_access_info.yoke_state == 1 && (s_access_info.fresnel_state == 1 || s_access_info.optical_20_access_state == 1))
					{
						if(p_sys_menu_model->ele_accessory.tili_demo_limit[0] <= -120)
							p_sys_menu_model->ele_accessory.tili_demo_limit[0] = -120;
						if(p_sys_menu_model->ele_accessory.tili_demo_limit[1] >= 120)
							p_sys_menu_model->ele_accessory.tili_demo_limit[1] = 120;
					}
					motor_limit_text_refresh(btn, p_sys_menu_model->ele_accessory.tili_demo_limit[user_data->cur_sel-1], user_data->cur_sel);
					page_event_send(EVENT_DATA_WRITE);
				break;
				case 6: 
					if (key == LV_KEY_RIGHT)
					{
						p_sys_menu_model->ele_accessory.zoom_demo_limit[user_data->cur_sel-1]+=1;
						if(p_sys_menu_model->ele_accessory.zoom_demo_limit[user_data->cur_sel-1] >= 50)
							p_sys_menu_model->ele_accessory.zoom_demo_limit[user_data->cur_sel-1] = 50;
					}
					else if(key == LV_KEY_LEFT)
					{
						p_sys_menu_model->ele_accessory.zoom_demo_limit[user_data->cur_sel-1]-=1;
						if(p_sys_menu_model->ele_accessory.zoom_demo_limit[user_data->cur_sel-1] < 18)
							p_sys_menu_model->ele_accessory.zoom_demo_limit[user_data->cur_sel-1] = 18;
					}
					motor_limit_text_refresh(btn, p_sys_menu_model->ele_accessory.zoom_demo_limit[user_data->cur_sel-1], user_data->cur_sel);
					page_event_send(EVENT_DATA_WRITE);
				break;
				default:break;
			}
			if(key == LV_KEY_BACKSPACE)
			{
				user_data->cur_sel = 0;
				lv_group_set_editing(group, false);
				motor_limit_state_refresh(btn, user_data->cur_sel);
			}
		}
        else
		{
			if (key == LV_KEY_BACKSPACE)
			{
				screen_turn_prev_page();
			}
		}
    }
}

static void motor_demo_text_init(lv_obj_t *parent, int16_t *limit_value, const char *name)
{
	uint8_t str[3] = {0};
	
	lv_obj_t *name_label = lv_label_create(parent);
	lv_obj_set_style_text_font(name_label, Font_ResouceGet(FONT_18_MEDIUM), 0);         
	lv_label_set_text(name_label , (const char*)name);
	lv_obj_set_style_text_color(name_label, lv_color_white(), 0);
	lv_obj_align(name_label, LV_ALIGN_LEFT_MID, 10, 0);
	
	lv_obj_t *min_btn = lv_obj_create(parent);
	lv_obj_set_style_text_font(min_btn, Font_ResouceGet(FONT_18_MEDIUM), 0);
	lv_obj_set_size(min_btn, 57, 32);
	lv_obj_set_style_radius(min_btn, 0, 0);
	lv_obj_clear_flag(min_btn, LV_OBJ_FLAG_SCROLLABLE);
	lv_obj_set_style_text_color(min_btn, lv_color_black(), 0);
	lv_obj_set_style_text_color(min_btn, lv_color_make(255, 0, 0), LV_STATE_USER_1);
	lv_obj_set_style_bg_color(min_btn, lv_color_white(), 0);
	lv_obj_align_to(min_btn, name_label, LV_ALIGN_OUT_RIGHT_MID, 10, 0);
	
	lv_obj_t *min_label = lv_label_create(min_btn);
	lv_obj_set_style_text_font(min_label, Font_ResouceGet(FONT_18_MEDIUM), 0);
	sprintf((char *)str ,"%d", limit_value[0]);             
	lv_label_set_text(min_label , (const char*)str);
	lv_obj_set_style_text_color(min_label, lv_color_black(), 0);
	lv_obj_set_style_text_color(min_label, lv_color_make(255, 0, 0), LV_STATE_USER_1);
	lv_obj_align(min_label, LV_ALIGN_CENTER, 0, 0);
	
	lv_obj_t* symbol_lab = lv_label_create(parent);
    lv_obj_set_style_text_font(symbol_lab, Font_ResouceGet(FONT_18), 0);
    lv_obj_set_style_text_color(symbol_lab, lv_color_white(), 0);
    lv_label_set_text(symbol_lab, "°");
    lv_obj_align_to(symbol_lab, min_btn, LV_ALIGN_OUT_RIGHT_TOP, 2, 0);
	
	lv_obj_t* symbol3_lab = lv_label_create(parent);
    lv_obj_set_style_text_font(symbol3_lab, Font_ResouceGet(FONT_24), 0);
    lv_obj_set_style_text_color(symbol3_lab, lv_color_white(), 0);
    lv_label_set_text(symbol3_lab, "~");
    lv_obj_align_to(symbol3_lab, min_btn, LV_ALIGN_OUT_RIGHT_MID, 18, 0);
	
    lv_obj_t *max_btn = lv_obj_create(parent);
	lv_obj_set_size(max_btn, 57, 32);
	lv_obj_set_style_radius(max_btn, 0, 0);
	lv_obj_clear_flag(max_btn, LV_OBJ_FLAG_SCROLLABLE);
	lv_obj_set_style_text_font(max_btn, Font_ResouceGet(FONT_18_MEDIUM), 0);
	lv_obj_set_style_text_color(max_btn, lv_color_black(), 0);
	lv_obj_set_style_text_color(max_btn, lv_color_make(255, 0, 0), LV_STATE_USER_1);
	lv_obj_set_style_bg_color(max_btn, lv_color_white(), 0);
	lv_obj_align_to(max_btn, symbol3_lab, LV_ALIGN_OUT_RIGHT_MID, 8, 0);
	
	memset(str, 0, 3);
	lv_obj_t *max_label = lv_label_create(max_btn);
	lv_obj_set_style_text_font(max_label, Font_ResouceGet(FONT_18_MEDIUM), 0);
	sprintf((char *)str ,"%d", limit_value[1]);             
	lv_label_set_text(max_label , (const char*)str);
	lv_obj_set_style_text_color(max_label, lv_color_black(), 0);
	lv_obj_set_style_text_color(max_label, lv_color_make(255, 0, 0), LV_STATE_USER_1);
	lv_obj_align(max_label, LV_ALIGN_CENTER, 0, 0);
	
	lv_obj_t* symbol2_lab = lv_label_create(parent);
    lv_obj_set_style_text_font(symbol2_lab, Font_ResouceGet(FONT_18), 0);
    lv_obj_set_style_text_color(symbol2_lab, lv_color_white(), 0);
    lv_label_set_text(symbol2_lab, "°");
    lv_obj_align_to(symbol2_lab, max_btn, LV_ALIGN_OUT_RIGHT_TOP, 2, 0);
}

static void motor_limit_state_refresh(lv_obj_t *obj, uint8_t cnt)
{
	lv_obj_t *min_btn = lv_obj_get_child(obj, 1);
	lv_obj_t *label = lv_obj_get_child(min_btn, 0);
	
	lv_obj_t *max_btn = lv_obj_get_child(obj, 4);
	lv_obj_t *label2 = lv_obj_get_child(max_btn, 0);
	if(cnt == 1)
	{
		lv_obj_add_state(label, LV_STATE_USER_1);
		lv_obj_clear_state(label2, LV_STATE_USER_1);
		lv_obj_add_state(label2, LV_STATE_DEFAULT);
	}
	else if(cnt == 2)
	{
		lv_obj_clear_state(label, LV_STATE_USER_1);
		lv_obj_add_state(label, LV_STATE_DEFAULT);
		lv_obj_add_state(label2, LV_STATE_USER_1);
	}
	else if(cnt == 0)
	{
		lv_obj_clear_state(label, LV_STATE_USER_1);
		lv_obj_clear_state(label2, LV_STATE_USER_1);
		lv_obj_add_state(label, LV_STATE_DEFAULT);
		lv_obj_add_state(label2, LV_STATE_DEFAULT);
	}
}

static void motor_limit_text_refresh(lv_obj_t *obj, int16_t angle, uint8_t cnt)
{
	uint8_t str[3] = {0};
	lv_obj_t *min_btn = lv_obj_get_child(obj, 1);
	lv_obj_t *label = lv_obj_get_child(min_btn, 0);
	
	lv_obj_t *max_btn = lv_obj_get_child(obj, 4);
	lv_obj_t *label2 = lv_obj_get_child(max_btn, 0);
	
	if(cnt == 1)
	{
		sprintf((char *)str ,"%d", angle);             
		lv_label_set_text(label , (const char*)str);
	}
	else if(cnt == 2)
	{
		sprintf((char *)str ,"%d", angle);             
		lv_label_set_text(label2 , (const char*)str);
	}
}

static uint8_t motor_reset_state_get(void)
{
	return ele_reset_flag;
}

/**
 * @brief 刷新进度条函数
 * 
 * @param a 
 * @param val 
 */
static void bar_anim_exec_xcb(void *a, int32_t val)
{
    lv_obj_t* cont = lv_obj_get_parent(a);
    lv_obj_t* process_label = lv_obj_get_child(cont, 1);
    
    lv_bar_set_value(a, val, LV_ANIM_ON);
    lv_label_set_text_fmt(process_label, "%d%%", val);
    lv_obj_align_to(process_label, a, LV_ALIGN_OUT_TOP_MID, 0, -10);    
}

static void warning_event_cb(lv_event_t* e)
{
	lv_event_code_t event = lv_event_get_code(e);
//    lv_obj_t* obj = lv_event_get_target(e);	
	if (event == LV_EVENT_RELEASED)
	{
		p_sys_menu_model->regognition_status = p_sys_menu_model->regognition_status ? false : true;	// 点击确定后才可以更改状态
		data_center_write_config_data(SYS_CONFIG_ACC_RECOGNITION, &p_sys_menu_model->regognition_status);
		screen_load_sub_page_with_stack(SUB_ID_ACCESSORY_LIST, 0);
	}
	else if (event == LV_EVENT_KEY)
	{
		uint32_t key = *(uint32_t *)lv_event_get_param(e);
		if (key == LV_KEY_BACKSPACE)
			screen_load_sub_page_with_stack(SUB_ID_ACCESSORY_LIST, 0);
	}
}