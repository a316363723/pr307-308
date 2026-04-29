/*********************
 *      INCLUDES
 *********************/
#include "ui_common.h"
#include "page_widget.h"
#include "user.h"

/*********************
 *      DEFINES
 *********************/
#define ANIM_TASK_PERIOD    200
/*********************
 *      TPEDEFS
 *********************/
typedef enum {
    ANIM_STATUS_START,
    ANIM_STATUS_TRANSLATE_X,
    ANIM_STATUS_RANDOM_CRASH,
    ANIM_STATUS_BUSY,
} anim_status_t;
/**********************
 *  STATIC PROTOTYPES
 **********************/
static void img_zoom_cb(void* img, int32_t v);
static void screen_protector_task_cb(struct _lv_timer_t *t);
static void screen_protector_event_cb(lv_event_t* e);
static void set_next_status(anim_status_t status);
static void anim_zoom_ready_cb(struct _lv_anim_t *t);
static void anim_translate_x_ready_cb(struct _lv_anim_t* t);
static void img_set_translate_x(void* img, int32_t v);
static void anim_random_crash(void* img, int32_t v);
/*********************
 *  STATIC VATIABLES
 *********************/
static lv_obj_t* img_logo1, *img_logo2;
static lv_timer_t* sp_timer = NULL;
static anim_status_t sp_status = ANIM_STATUS_START;
static anim_status_t sp_next_status = ANIM_STATUS_START;
static lv_group_t* indev_group;
static bool b_sp_deleted = true;
/*********************
 *  GLOBAL VATIABLES
 *********************/

/*********************
 *  GLOBAL FUNCTIONS
 *********************/

void screen_proctector_exit(void)
{
//    lv_obj_t* obj = lv_group_get_focused(indev_group);
//	uint8_t key = LV_KEY_ESC;
//	lv_event_send(obj, LV_EVENT_KEY, &key);
}

void work_screen_proctector_exit(uint8_t state)
{
	if(screen_get_act_pid() == PAGE_WORK_MODE && screen_get_act_spid() == SUB_ID_SYNC_FOLLWO_MODE)
	{
		lv_obj_t* obj = lv_group_get_focused(indev_group);
		uint8_t key = LV_KEY_ESC;
		lv_event_send(obj, LV_EVENT_KEY, &key);
	}
	else if(state == 1)
	{
		lv_obj_t* obj = lv_group_get_focused(indev_group);
		uint8_t key = LV_KEY_ESC;
		lv_event_send(obj, LV_EVENT_KEY, &key);
	}
}

void screen_proctector_create(void)
{
    if (!b_sp_deleted)     
        return;
    
    b_sp_deleted = false;
    indev_group = lv_group_create();
    sp_status = ANIM_STATUS_START;
    my_indev_set_group(indev_group);
	
    lv_obj_t* screen_panel = lv_obj_create(lv_scr_act());
    //lv_obj_remove_style_all(screen_panel);
    lv_obj_clear_flag(screen_panel, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_radius(screen_panel, 0, 0);
    lv_obj_set_style_pad_all(screen_panel, 0, 0);
	lv_obj_set_style_border_width(screen_panel, 0, 0);
	lv_obj_set_style_outline_width(screen_panel, 0, 0);    
    lv_obj_set_style_bg_color(screen_panel, lv_color_black(), 0);
    lv_obj_set_size(screen_panel, lv_pct(100), lv_pct(100));
    lv_obj_add_event_cb(screen_panel, screen_protector_event_cb, LV_EVENT_ALL, NULL);
    lv_obj_center(screen_panel);

    LV_IMG_DECLARE(ImgScreenProctector)
    img_logo1 = lv_img_create(screen_panel);
    lv_img_set_src(img_logo1, &ImgScreenProctector);
    lv_obj_add_flag(img_logo1, LV_OBJ_FLAG_HIDDEN);

    img_logo2 = lv_img_create(screen_panel);
    lv_img_set_src(img_logo2, &ImgScreenProctector);
    lv_obj_add_flag(img_logo2, LV_OBJ_FLAG_HIDDEN);
    
    sp_timer = lv_timer_create(screen_protector_task_cb, ANIM_TASK_PERIOD, NULL);

    lv_group_add_obj(indev_group, screen_panel);
    lv_group_set_editing(indev_group, true);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void screen_protector_task_cb(struct _lv_timer_t *t)
{
    if (b_sp_deleted)
        return;

    switch(sp_status)
    {
        case ANIM_STATUS_START:
        {
            lv_obj_clear_flag(img_logo1, LV_OBJ_FLAG_HIDDEN);
            lv_obj_center(img_logo1);
            //出场放大
            lv_anim_t a;
            lv_anim_init(&a);
            lv_anim_set_var(&a, img_logo1);
            lv_anim_set_ready_cb(&a, anim_zoom_ready_cb);
            lv_anim_set_exec_cb(&a, img_zoom_cb);
            lv_anim_set_values(&a, 0, 256);
            lv_anim_set_time(&a, 3000);
            lv_anim_set_playback_time(&a, 0);
            //lv_anim_set_playback_time(&a, 500);
            lv_anim_set_repeat_count(&a, 0);
            lv_anim_start(&a);

            sp_status = ANIM_STATUS_BUSY;
            sp_next_status = ANIM_STATUS_BUSY;
        }
        break;
        case ANIM_STATUS_TRANSLATE_X:
        {
            const lv_img_dsc_t* p_img_dsc = lv_img_get_src(img_logo1);
            lv_coord_t img_w = p_img_dsc->header.w;
            lv_coord_t img_h = p_img_dsc->header.h;
            
            (void)img_h;
            lv_obj_set_pos(img_logo2, -img_w, lv_obj_get_y(img_logo1));            
            lv_obj_clear_flag(img_logo2, LV_OBJ_FLAG_HIDDEN);

            //平移
            lv_anim_t a;
            lv_anim_init(&a);
            lv_anim_set_var(&a, img_logo1);
            lv_anim_set_exec_cb(&a, img_set_translate_x);
            UI_PRINTF("start:%d end:%d\r\n", lv_obj_get_x(img_logo1), 320 + lv_obj_get_x(img_logo1) + img_w);
            //lv_anim_set_values(&a, lv_obj_get_x(img_logo1), 320- lv_obj_get_x(img_logo1) + lv_obj_get_x(img_logo1) + img_w);
            lv_anim_set_values(&a, 0, 320);
            lv_anim_set_time(&a, 5000);
            lv_anim_set_ready_cb(&a, anim_translate_x_ready_cb);            
            lv_anim_set_repeat_count(&a, 1);
            lv_anim_start(&a);

            sp_status = ANIM_STATUS_BUSY;
            sp_next_status = ANIM_STATUS_BUSY;
        }
        break;
        case ANIM_STATUS_RANDOM_CRASH:
        {
            lv_anim_t a;
            lv_anim_init(&a);
            lv_anim_set_var(&a, img_logo2);
            lv_anim_set_exec_cb(&a, anim_random_crash);
            lv_anim_set_values(&a, 0, 1000);
            lv_anim_set_time(&a, 3000);            
            lv_anim_set_repeat_count(&a, LV_ANIM_REPEAT_INFINITE);
            lv_anim_start(&a);
            sp_status = ANIM_STATUS_BUSY;
            sp_next_status = ANIM_STATUS_BUSY;
        }
        break;
        case ANIM_STATUS_BUSY:
        {
            if (sp_status != sp_next_status)
            {
                sp_status = sp_next_status;
            }
        }
        break;
        default:break;
    }
}

static void set_next_status(anim_status_t status)
{
    sp_next_status = status;
}

static void img_zoom_cb(void* img, int32_t v)
{
    lv_img_set_zoom(img, v);
}

static void img_set_translate_x(void* img, int32_t v)
{
    const lv_img_dsc_t* pdsc = lv_img_get_src(img);

    lv_obj_set_x(img, v);
    if (lv_obj_get_x2(img) >= 320)
    {
        lv_coord_t img2_start = 0 - (lv_coord_t)pdsc->header.w + LV_ABS(lv_obj_get_x2(img) - 320);
        lv_obj_set_x(img_logo2, img2_start);
    }
}

static void anim_zoom_ready_cb(struct _lv_anim_t *t)
{
    set_next_status(ANIM_STATUS_TRANSLATE_X);
}

static void anim_translate_x_ready_cb(struct _lv_anim_t* t)
{
    set_next_status(ANIM_STATUS_RANDOM_CRASH);
}

static void anim_random_crash(void* img, int32_t v)
{
#define VELOCITY_X      1
#define VELOCITY_Y      1
    static int8_t v_x = VELOCITY_X, v_y = VELOCITY_Y;
    lv_point_t pos;
    (void)v;

    if (lv_obj_get_x(img) <= 0 || lv_obj_get_x2(img) >= 320)
    {
        v_x = -v_x;
    }

    if (lv_obj_get_y(img) <= 0 || lv_obj_get_y2(img) >= 240)
    {
        v_y = -v_y;
    }
    
    pos.x = lv_obj_get_x(img) + v_x;
    pos.y = lv_obj_get_y(img) + v_y;
    lv_obj_set_pos(img, pos.x, pos.y);
    UI_PRINTF("pos.x : %d pos.y : %d, img y2 %d img y1 %d \r\n", pos.x, pos.y, lv_obj_get_y(img), lv_obj_get_y2(img));
}

static void screen_protector_event_cb(lv_event_t* e)
{
    lv_event_code_t event = lv_event_get_code(e);
    lv_obj_t* panel = lv_event_get_target(e);

    if (LV_EVENT_KEY == event)
    {
        if (!b_sp_deleted)
        {
            b_sp_deleted = !b_sp_deleted;
            lv_obj_del_async(panel);
            my_indev_set_group(lv_group_get_default());                        
        }
    }
    else if (LV_EVENT_DELETE == event)
    {
        b_sp_deleted = true;

        /* 释放定时器资源 */
        lv_timer_pause(sp_timer);
        lv_timer_del(sp_timer);
        sp_timer = NULL;

        /* 释放group资源 */
        lv_group_focus_freeze(indev_group, true);
        lv_group_remove_all_objs(indev_group);
        lv_group_del(indev_group);
        indev_group = NULL;
        
        /* 手动设置状态机 */
        user_exit_proctector();
    }
}
