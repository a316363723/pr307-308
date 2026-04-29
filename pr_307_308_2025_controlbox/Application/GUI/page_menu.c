/*********************
 *      INCLUDES
 *********************/
#include "ui_common.h"
#include "page_widget.h"
#include "menu_item_obj.h"
/**
 * @brief 
 * 该屏幕最大显示目标数
 * 当前显示目标对象
 * obj1 -> obj2 -> obj3
 * 
 * 双向链表
 *          <-
 * 1 -> 2 -> 3 -> 4 -> 5
 * 显示位置: 1(左边不可见), 2, 3, 4, 5(右边不可见)
 * 
 * 更新待移动的对象, 并计算位置
 * 加载动画
 * - 坐标变化, 大小变化, 图片大小变化
 * -  
 */

/*********************
 *      DEFINES
 *********************/
#define IMG_COLOR_FOCUSED       lv_color_make(255, 255, 255)
#define IMG_COLOR_DEFAULT       lv_color_make(230, 230, 230)

#define FONT_COLOR              lv_color_make(255, 255, 255)
#define IMG_COLOR_INDICATOR     lv_color_make(255, 255, 255)

#define MAX_DISP_OBJS                3
#define BTN_NORMAL_WIDTH             65
#define BTN_FOCUSED_WIDTH            104
#define OBJ_GAP_WIDTH                10
#define INVISIBLE_LEFT_POS_X         -BTN_NORMAL_WIDTH
#define INVISIBLE_LEFT_POS_Y         20
#define NORMAL_IMG_COFFICIENT        256

#define DOT_SELECTED_COLOR           lv_color_make(255, 255, 255)
#define DOT_DEFAULT_COLOR            lv_color_make(51, 51, 51)
#define DOT_CONT_WIDTH               140
#define DOT_CONT_HEIGHT              20

#define LABEL_TEXT_COLOR             lv_color_make(255, 255, 255)

#define ANIMATION_TIME               100
/**********************
 *      TYPEDEFS
 **********************/
enum {
#if UI_ELECT_ACCESSORY
    MENU_ITEM_ID_ACCESSORY = 0,
#endif
#if UI_LEAD_FOLLOE_MODE    
    MENU_ITEM_ID_WORK_MODE,
#endif   
#if UI_LEAD_OUT_MODE
    MENU_ITEM_ID_OUTPUT_MODE,
#endif
#if UI_HIGH_SPEED_MODE
    MENU_ITEM_ID_HIGH_SPD_MODE,
#endif
//	MENU_ITEM_ID_FREQUENCY,
	MENU_ITEM_ID_DIMMING_CURVE,
#if UI_DMX_MODE
    MENU_ITEM_ID_DMX_SETTING,
#endif
	MENU_ITEM_ID_CTRL_SYSTEM,
	MENU_ITEM_ID_STUDIO,
    MENU_ITEM_ID_FAN_MODE,
    MENU_ITEM_ID_UPDATE,
	MENU_ITEM_ID_PRODUCT_INFO,
	MENU_ITEM_ID_CUSTOM_FX,
	MENU_ITEM_ID_LANG,
#if UI_SCREEN_SAVER    
    MENU_ITEM_ID_SCREENSAVER,
#endif
    MENU_ITEM_ID_FACTORY,
#if UI_BAT_MODE
    MENU_ITEM_ID_BATTERY_OPTION,
#endif
    MENU_ITEM_ID_MAX,
};

enum {
    TRAVEL_DIR_P = 0,
    TRAVEL_DIR_N,
};

typedef struct _item_dsc {
    item_t *item;
    lv_point_t start_pos;
    lv_point_t end_pos;
    lv_coord_t start_width;
    lv_coord_t width;
    uint8_t bfocus : 1;
}item_dsc_t;

typedef struct _menu {
    lv_coord_t menu_w;
    lv_coord_t menu_h;
    lv_coord_t btn_w;   //正常宽度    
}menu_t;
/**********************
 *  STATIC PROTOTYPES
 **********************/
static void upd_disp_obj_dsc(void);
static void upd_cw_anim_obj(void);
static void upd_ccw_anim_obj(void);
static void refresh_disp_label(void);
static void refresh_dots(lv_obj_t* obj, int sel_num);
static uint8_t menu_get_pid(item_id_t id);
static void menu_create(void);
static void page_menu_construct(void);
static void page_menu_destruct(void);
/**********************
 *  STATIC VARIABLES
 **********************/
static item_t* item_list_head = NULL;
static item_t* focused_item = NULL;
static lv_group_t* indev_group;;
static bool menu_deleted = false;

// static lv_point_t fixed_pos[MAX_DISP_OBJS + 2] = {
//     {-BTN_NORMAL_WIDTH, 20},
//     {0, 20},
//     {78, 0},
//     {196, 20},
//     {224 + BTN_FOCUSED_WIDTH, 20},
// };

LV_IMG_DECLARE(ImgCtrlSystem)
LV_IMG_DECLARE(ImgScreenSaver)
LV_IMG_DECLARE(ImgCustomFX)
LV_IMG_DECLARE(ImgDimCurve)
LV_IMG_DECLARE(ImgFactoryReset)
LV_IMG_DECLARE(ImgFreSelect)
LV_IMG_DECLARE(ImgLanguage)
LV_IMG_DECLARE(ImgProductInfo)
LV_IMG_DECLARE(ImgStudioMode)
LV_IMG_DECLARE(ImgUpdate)
LV_IMG_DECLARE(ImgFanMode)
LV_IMG_DECLARE(ImgHighSpdMode)
LV_IMG_DECLARE(ImgDmxMode)
LV_IMG_DECLARE(ImgOutputMode)
LV_IMG_DECLARE(ImgWorkMode)
LV_IMG_DECLARE(ImgBatteryOption)
LV_IMG_DECLARE(img_Electric)

static const lv_img_dsc_t* item_img[MENU_ITEM_ID_MAX] = {
#if UI_LEAD_OUT_MODE
    [MENU_ITEM_ID_OUTPUT_MODE] = &ImgOutputMode,
#endif
#if UI_HIGH_SPEED_MODE
    [MENU_ITEM_ID_HIGH_SPD_MODE] = &ImgHighSpdMode,
#endif
#if UI_DMX_MODE
    [MENU_ITEM_ID_DMX_SETTING] = &ImgDmxMode,
#endif
    [MENU_ITEM_ID_CUSTOM_FX] = &ImgCustomFX,
#if UI_LEAD_FOLLOE_MODE
    [MENU_ITEM_ID_WORK_MODE] = &ImgWorkMode,
#endif    
    [MENU_ITEM_ID_DIMMING_CURVE] = &ImgDimCurve,
    [MENU_ITEM_ID_FAN_MODE] = &ImgFanMode,
    [MENU_ITEM_ID_STUDIO] = &ImgStudioMode,
    [MENU_ITEM_ID_CTRL_SYSTEM] = &ImgCtrlSystem,
#if UI_SCREEN_SAVER
    [MENU_ITEM_ID_SCREENSAVER] = &ImgScreenSaver,    
#endif
    [MENU_ITEM_ID_LANG] = &ImgLanguage,
//    [MENU_ITEM_ID_FREQUENCY] = &ImgFreSelect,
    [MENU_ITEM_ID_PRODUCT_INFO] = &ImgProductInfo,
    [MENU_ITEM_ID_UPDATE] = &ImgUpdate,
    [MENU_ITEM_ID_FACTORY] = &ImgFactoryReset,
#if UI_BAT_MODE
    [MENU_ITEM_ID_BATTERY_OPTION] = &ImgBatteryOption,
#endif
#if UI_ELECT_ACCESSORY
    [MENU_ITEM_ID_ACCESSORY] = &img_Electric,  //testing
#endif
};

static lv_obj_t* name_disp_label;
static lv_obj_t* menu_panel;
static lv_obj_t* dots_cont;

static item_dsc_t cur_anim_item[MAX_DISP_OBJS + 1];
static lv_coord_t disp_pos_x[MAX_DISP_OBJS];

static item_t items[MENU_ITEM_ID_MAX];

static lv_coord_t panel_h;
static lv_coord_t panel_w;
static lv_coord_t inv_pos_left_x;
static lv_coord_t inv_pos_right_x;
static lv_coord_t com_y;
static lv_coord_t focus_com_y;
//static uint16_t zoom_coff;
static uint8_t disp_nums;
static uint8_t anim_obj_nums;
static bool anim_is_running = false;


/*********************
 *  GLOBAL VATIABLES
 *********************/
page_t page_sys_menu = PAGE_INITIALIZER("sys menu",
                                PAGE_MENU, 
                                page_menu_construct, 
                                page_menu_destruct, 
                                NULL,
                                NULL,                                
                                PAGE_LIGHT_MODE_HSI);

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

uint8_t menu_get_index_by_id(uint8_t pid)
{
    uint8_t item_id = 0;
    switch(pid)
    {
#if UI_LEAD_OUT_MODE
        case PAGE_OUTPUT_MODE:      item_id = MENU_ITEM_ID_OUTPUT_MODE; break;
#endif
#if UI_HIGH_SPEED_MODE
        case PAGE_HIGH_SPD_MODE:    item_id = MENU_ITEM_ID_HIGH_SPD_MODE; break;
#endif
#if UI_DMX_MODE
        case PAGE_DMX_MODE:         item_id = MENU_ITEM_ID_DMX_SETTING; break;
#endif
        case PAGE_CUSTOM_FX:        item_id = MENU_ITEM_ID_CUSTOM_FX; break;
#if UI_LEAD_FOLLOE_MODE   
        case PAGE_WORK_MODE:        item_id = MENU_ITEM_ID_WORK_MODE; break;
#endif        
        case PAGE_DIMMING_CURVE:    item_id =  MENU_ITEM_ID_DIMMING_CURVE;  break;
        case PAGE_FAN_MODE:         item_id = MENU_ITEM_ID_FAN_MODE; break;
        case PAGE_STUDIO_MODE:      item_id = MENU_ITEM_ID_STUDIO ; break;
        case PAGE_CONTROL_SYSTEM:   item_id = MENU_ITEM_ID_CTRL_SYSTEM; break;
#if UI_SCREEN_SAVER         
        case PAGE_SCREENSAVER:      item_id = MENU_ITEM_ID_SCREENSAVER; break;
#endif        
        case PAGE_LANGUAGE:         item_id = MENU_ITEM_ID_LANG; break;
//        case PAGE_FREQUENCY_SELECTION: item_id =  MENU_ITEM_ID_FREQUENCY; break;
        case PAGE_PRODUCT_INFO:     item_id = MENU_ITEM_ID_PRODUCT_INFO; break;
        case PAGE_UPDATE:           item_id = MENU_ITEM_ID_UPDATE; break;
        case PAGE_FACTORY:          item_id = MENU_ITEM_ID_FACTORY; break;
#if UI_BAT_MODE
        case PAGE_BATTERY_OPTION:   item_id = MENU_ITEM_ID_BATTERY_OPTION; break;
#endif
#if UI_ELECT_ACCESSORY
        case PAGE_ELECT_ACCESSORY:  item_id = MENU_ITEM_ID_ACCESSORY; break;
#endif
        default:break;
    }
    return item_id;    
}

/**********************
 *   STATIC FUNCTIONS
 **********************/
static void page_menu_construct(void)
{
    menu_create();
}

static void page_menu_destruct(void)
{
    lv_group_focus_freeze(indev_group, true);
    lv_group_del(indev_group);
    indev_group = NULL;

    lv_obj_clean(lv_scr_act());
}

void item_list_print(void)
{
    item_t* item = item_list_head;
  
    while(item != item_list_head)
    {
        // UI_PRINTF("id : %d\r\n", item->id);
        item = item->next;
    }
}

static void cur_op_item_print(void)
{
    item_dsc_t *pdsc = &cur_anim_item[0];
    for (uint8_t i = 0; i < sizeof(cur_anim_item) / sizeof(cur_anim_item[0]); i++, pdsc++)
    {
        UI_PRINTF("id: %d focus:%d \r\n", pdsc->item->id, pdsc->bfocus);
    }    
}

static const char* get_focused_item_name(void)
{
    return focused_item == NULL ? NULL : focused_item->name;
}

static item_id_t get_focused_item_id(void)
{
    return focused_item == NULL ? 0 : focused_item->id;
}

static const char* item_get_str(item_id_t id)
{
    const char* tmp = NULL;
    switch(id)
    {
#if UI_LEAD_OUT_MODE
        case MENU_ITEM_ID_OUTPUT_MODE:      tmp =  Lang_GetStringByID(STRING_ID_OUTPUT_MODE); break;
#endif
#if UI_HIGH_SPEED_MODE
        case MENU_ITEM_ID_HIGH_SPD_MODE:    tmp = Lang_GetStringByID(STRING_ID_HIGH_SPD_MODE); break;
#endif
#if UI_DMX_MODE
        case MENU_ITEM_ID_DMX_SETTING:      tmp =  Lang_GetStringByID(STRING_ID_DMX_SETTING); break;
#endif
        case MENU_ITEM_ID_CUSTOM_FX:        tmp =  Lang_GetStringByID(STRING_ID_CUSTOM_FX); break;
    #if UI_LEAD_FOLLOE_MODE
        case MENU_ITEM_ID_WORK_MODE:        tmp =  Lang_GetStringByID(STRING_ID_WORK_MODE); break;
    #endif          
        case MENU_ITEM_ID_DIMMING_CURVE:    tmp =  Lang_GetStringByID(STRING_ID_DIMMING_CURVE); break;
        case MENU_ITEM_ID_FAN_MODE:         tmp =  Lang_GetStringByID(STRING_ID_FAN_MODE); break;
        case MENU_ITEM_ID_STUDIO:           tmp =  Lang_GetStringByID(STRING_ID_STUDIO); break;
        case MENU_ITEM_ID_CTRL_SYSTEM:      tmp =  Lang_GetStringByID(STRING_ID_CTRL_SYSTEM); break;
    #if UI_SCREEN_SAVER
        case MENU_ITEM_ID_SCREENSAVER:      tmp =  Lang_GetStringByID(STRING_ID_SCEEENSAVAR); break;
    #endif
        case MENU_ITEM_ID_LANG:             tmp =  Lang_GetStringByID(STRING_ID_LANGUAGE); break;
//        case MENU_ITEM_ID_FREQUENCY:        tmp =  Lang_GetStringByID(STRING_ID_FREQUENCY); break;
        case MENU_ITEM_ID_PRODUCT_INFO:     tmp =  Lang_GetStringByID(STRING_ID_PRODUCT_INFO); break;
        case MENU_ITEM_ID_UPDATE:           tmp =  Lang_GetStringByID(STRING_ID_UPDATE); break;
        case MENU_ITEM_ID_FACTORY:          tmp =  Lang_GetStringByID(STRING_ID_FACTORY); break;
#if UI_BAT_MODE
        case MENU_ITEM_ID_BATTERY_OPTION:   tmp =  Lang_GetStringByID(STRING_ID_BATTERY_OPTION); break;
#endif
#if UI_ELECT_ACCESSORY
        case MENU_ITEM_ID_ACCESSORY:        tmp =  Lang_GetStringByID(STRING_ID_ELE_ACCESSORY); break;
#endif
        default:break;
    }
    return tmp;
}

static uint8_t menu_get_pid(item_id_t id)
{
    uint8_t pid = PAGE_OUTPUT_MODE;
    switch(id)
    {
#if UI_LEAD_OUT_MODE
        case MENU_ITEM_ID_OUTPUT_MODE:    pid = PAGE_OUTPUT_MODE; break;
#endif
#if UI_HIGH_SPEED_MODE
        case MENU_ITEM_ID_HIGH_SPD_MODE:  pid = PAGE_HIGH_SPD_MODE; break;
#endif
#if UI_DMX_MODE
        case MENU_ITEM_ID_DMX_SETTING:    pid = PAGE_DMX_MODE; break;
#endif
        case MENU_ITEM_ID_CUSTOM_FX:      pid = PAGE_CUSTOM_FX; break;
#if UI_LEAD_FOLLOE_MODE   
        case MENU_ITEM_ID_WORK_MODE:      pid = PAGE_WORK_MODE; break;
#endif        
        case MENU_ITEM_ID_DIMMING_CURVE:  pid = PAGE_DIMMING_CURVE; break;
        case MENU_ITEM_ID_FAN_MODE:       pid = PAGE_FAN_MODE; break;
        case MENU_ITEM_ID_STUDIO:         pid = PAGE_STUDIO_MODE; break;
        case MENU_ITEM_ID_CTRL_SYSTEM:    pid = PAGE_CONTROL_SYSTEM; break;
#if UI_SCREEN_SAVER
        case MENU_ITEM_ID_SCREENSAVER:    pid = PAGE_SCREENSAVER; break;
#endif
        case MENU_ITEM_ID_LANG:           pid = PAGE_LANGUAGE; break;
//        case MENU_ITEM_ID_FREQUENCY:      pid = PAGE_FREQUENCY_SELECTION; break;
        case MENU_ITEM_ID_PRODUCT_INFO:   pid = PAGE_PRODUCT_INFO; break;
        case MENU_ITEM_ID_UPDATE:         pid = PAGE_UPDATE; break;
        case MENU_ITEM_ID_FACTORY:        pid = PAGE_FACTORY; break;
#if UI_BAT_MODE
        case MENU_ITEM_ID_BATTERY_OPTION: pid = PAGE_BATTERY_OPTION; break;
#endif
#if UI_ELECT_ACCESSORY
        case MENU_ITEM_ID_ACCESSORY:      pid = PAGE_ELECT_ACCESSORY; break;
#endif
        default:break;
    }
    return pid;
}

static void anim_finish_cb(struct _lv_anim_t* a)
{
    if (menu_deleted)
        return;

    refresh_disp_label();
    refresh_dots(dots_cont, get_focused_item_id());
    anim_is_running = false;
}


static void anim_pos_x_cb(void* var, int32_t v)
{
    item_dsc_t* pdsc = lv_obj_get_user_data(var);
    int32_t cur_distance = LV_ABS(v - pdsc->start_pos.x);
    int32_t distance = LV_ABS(pdsc->start_pos.x - pdsc->end_pos.x);
    int32_t width = LV_ABS(pdsc->width - pdsc->start_width);
    int32_t target_width;
    int32_t y_distance_abs = LV_ABS(pdsc->start_pos.y - pdsc->end_pos.y);
    int32_t target_y;

    lv_obj_set_x(var, v);
  
    if (pdsc->start_pos.y > pdsc->end_pos.y)
    {
        target_y = pdsc->start_pos.y - lv_map(cur_distance, 0, distance, 0, y_distance_abs);
    }
    else
    {
        target_y = pdsc->start_pos.y + lv_map(cur_distance, 0, distance, 0, y_distance_abs);
    }

    lv_obj_set_y(var, target_y);

    if (pdsc->start_width > pdsc->width)
    {
        target_width = pdsc->start_width - lv_map(cur_distance, 0, distance, 0, width);
    }
    else
    {
        target_width = pdsc->start_width + lv_map(cur_distance, 0, distance, 0, width);
    }
  
    lv_obj_set_size(var, target_width, target_width);
    lv_obj_invalidate(lv_obj_get_parent(var));
}

static void animator_op_obj(uint8_t direction)
{
    item_dsc_t* pdsc = &cur_anim_item[0];
    lv_coord_t width;
    lv_obj_t* img = NULL;
    lv_anim_t a;
       
    anim_is_running = true;

    if (direction == TRAVEL_DIR_P)
    {
        pdsc = &cur_anim_item[3];
        for (int8_t i = 3; i >= 0; i--, pdsc--)
        {
            img = lv_obj_get_child(pdsc->item->obj, 0);

            //添加动画 位置a->b 位置b->c  大小更改        
            width = lv_obj_get_style_width(pdsc->item->obj, 0);
            pdsc->start_width = width;
            lv_obj_set_user_data(pdsc->item->obj, pdsc);            

            lv_anim_init(&a);
            lv_anim_set_var(&a, pdsc->item->obj);
            lv_anim_set_values(&a, pdsc->start_pos.x, pdsc->end_pos.x);
            lv_anim_set_exec_cb(&a, anim_pos_x_cb);
            lv_anim_set_repeat_count(&a, 0);
            lv_anim_set_time(&a, ANIMATION_TIME);
            lv_anim_set_playback_time(&a, 0);
            lv_anim_start(&a);
        }
    }
    else
    {
        pdsc = &cur_anim_item[0];
        for (uint8_t i = 0; i <= 3; i++, pdsc++)
        {
            img = lv_obj_get_child(pdsc->item->obj, 0);             
            width = lv_obj_get_style_width(pdsc->item->obj, 0);
            pdsc->start_width = width;            
            lv_obj_set_user_data(pdsc->item->obj, pdsc);

            lv_anim_init(&a);
            lv_anim_set_var(&a, pdsc->item->obj);
            lv_anim_set_values(&a, pdsc->start_pos.x, pdsc->end_pos.x);
            lv_anim_set_exec_cb(&a, anim_pos_x_cb);            
            lv_anim_set_repeat_count(&a, 0);
            lv_anim_set_time(&a, ANIMATION_TIME);
            lv_anim_set_playback_time(&a, 0);
            lv_anim_start(&a);
        }
    }

    lv_obj_add_state(focused_item->obj, LV_STATE_FOCUSED);
    lv_event_send(focused_item->obj, LV_EVENT_FOCUSED, NULL);    
    lv_anim_init(&a);
    lv_anim_set_var(&a, img);
    lv_anim_set_exec_cb(&a, NULL);
    lv_anim_set_ready_cb(&a, anim_finish_cb);
    lv_anim_set_repeat_count(&a, 0);
    lv_anim_set_time(&a, ANIMATION_TIME + 50);
    lv_anim_set_playback_time(&a, 0);
    lv_anim_start(&a);    
}


static void menu_panel_event_cb(lv_event_t* e)
{
    lv_obj_t* obj = lv_event_get_target(e);
    lv_event_code_t event = lv_event_get_code(e);
	struct sys_info_dmx        dmx;
	uint8_t hsmode = 0;
	
    if (!anim_is_running)
    {
        shortcut_keys_event_handler(e);
    }

    if (LV_EVENT_KEY == event)
    {
        uint32_t key = *(uint32_t*)lv_event_get_param(e);

        if (menu_deleted)
            return;

        if (LV_ENCODER_KEY_CW == key && !anim_is_running)
        {                        
            lv_obj_clear_state(focused_item->obj, LV_STATE_FOCUSED);
            lv_event_send(focused_item->obj, LV_EVENT_DEFOCUSED, NULL);

            upd_cw_anim_obj();      
            lv_obj_add_state(focused_item->obj, LV_STATE_FOCUSED);
            lv_event_send(focused_item->obj, LV_EVENT_FOCUSED, NULL);                 
            
            animator_op_obj(TRAVEL_DIR_P);
            cur_op_item_print();     
        }
        else if (LV_ENCODER_KEY_CC == key && !anim_is_running)
        {
            lv_obj_clear_state(focused_item->obj, LV_STATE_FOCUSED);
            lv_event_send(focused_item->obj, LV_EVENT_DEFOCUSED, NULL);

            upd_ccw_anim_obj();
            lv_obj_add_state(focused_item->obj, LV_STATE_FOCUSED);
            lv_event_send(focused_item->obj, LV_EVENT_FOCUSED, NULL);    

            animator_op_obj(TRAVEL_DIR_N);
            cur_op_item_print();
        }
        else if (LV_KEY_BACKSPACE == key && !anim_is_running)
        {
			data_center_read_sys_info(SYS_INFO_DMX, &dmx);
			if(dmx.eth_linked == 1 || dmx.wired_linked == 1 || dmx.wireless_linked == 1)
            {
				menu_deleted = false;
                return;
            }
			uint8_t id = PAGE_LIGHT_MODE_GEL;
			data_center_read_config_data(SYS_CONFIG_EFFECTS_MODE, &id);
            screen_set_act_index(focused_item->id);  
			screen_load_page(id, 0, true);
//            screen_turn_prev_page();
        }
    }
    else if (LV_EVENT_RELEASED == event && !anim_is_running)
    {
        menu_deleted = true;
        lv_anim_del_all();
        screen_set_act_index(focused_item->id);
        //恢复dmx光效
        if(menu_get_pid(focused_item->id) == PAGE_DMX_MODE)
        {
            struct db_dmx dmx_light;
            
            data_center_read_sys_info(SYS_INFO_DMX, &dmx);
            data_center_read_light_data(LIGHT_MODE_DMX, &dmx_light);
            if(dmx.eth_linked == 1 || dmx.wired_linked == 1 || dmx.wireless_linked == 1)
            {
                data_center_write_light_data(LIGHT_MODE_DMX, &dmx_light);
            }
			else
			{
				dmx_light.dmx_data.lightness = 0;
                data_center_write_light_data(LIGHT_MODE_DMX, &dmx_light);
			}
            screen_load_page(menu_get_pid(focused_item->id), 0, true);
        }
        else if(menu_get_pid(focused_item->id) == PAGE_ELECT_ACCESSORY)
        {
			struct sys_info_lamp   info_read_body = {0};
			
//            data_center_read_sys_info(SYS_INFO_DMX, &dmx);
//            if(dmx.eth_linked == 1 || dmx.wired_linked == 1 || dmx.wireless_linked == 1)
//            {
//				menu_deleted = false;
//                return;
//			}
			data_center_write_sys_info(SYS_INFO_LAMP, &info_read_body);	
			if(info_read_body.exist == 1)
				ui_ble_motor_read_angle(4);
			else
				screen_load_page(menu_get_pid(focused_item->id), 0, true);
        }
		else if(menu_get_pid(focused_item->id) == PAGE_CUSTOM_FX)
		{
			data_center_read_sys_info(SYS_INFO_DMX, &dmx);
			uint8_t  hsmode ;
			data_center_read_config_data(SYS_CONFIG_HIGH_SPEED_MODE, &hsmode);
			if(dmx.eth_linked == 1 || dmx.wired_linked == 1 || dmx.wireless_linked == 1 || hsmode == 1)
            {
				menu_deleted = false;
                return;
            }
			else
			{
				screen_load_page(menu_get_pid(focused_item->id), 0, true);
			}
		}
        else
        {
            screen_load_page(menu_get_pid(focused_item->id), 0, true);
        }
    }
    else if (LV_EVENT_DELETE == event)
    {


        page_set_sub_id(PAGE_MENU, focused_item->id);
    }
}

static lv_obj_t* dots_create(lv_obj_t* obj, int num)
{
    LV_IMG_DECLARE(ImgDot);
    lv_obj_t* img;
    lv_obj_t* cont = lv_obj_create(obj);
    lv_obj_remove_style_all(cont);
    lv_obj_clear_flag(cont, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_size(cont, LV_SIZE_CONTENT, DOT_CONT_HEIGHT);
    lv_obj_set_style_pad_column(cont, 4, 0);
    lv_obj_set_flex_flow(cont, LV_FLEX_FLOW_ROW);

    for (int i=0; i<num; i++)
    {
        img = lv_img_create(cont);
        lv_img_set_src(img, &ImgDot);
        lv_obj_set_style_img_recolor_opa(img, LV_OPA_MAX, 0);
        lv_obj_set_style_img_recolor(img, DOT_DEFAULT_COLOR, 0);
        lv_obj_set_style_img_recolor(img, DOT_SELECTED_COLOR, LV_STATE_FOCUSED);        
    }

    return cont;
}

static void refresh_dots(lv_obj_t* obj, int sel_num)
{    
    lv_obj_t* img;
    int child_num =lv_obj_get_child_cnt(obj);
    for (int i = 0; i < child_num; i++)
    {
        img = lv_obj_get_child(obj, i);
        if (i == sel_num)
        {            
            lv_obj_add_state(img, LV_STATE_FOCUSED);
            lv_event_send(img, LV_EVENT_FOCUSED, NULL);
        }
        else
        {
            lv_obj_clear_state(img, LV_STATE_FOCUSED);
            lv_event_send(img, LV_EVENT_DEFOCUSED, NULL);
        }        
    }
}

void menu_create(void)
{
    menu_deleted = false;
    anim_is_running = false;
    indev_group = lv_group_create();

    my_indev_set_group(indev_group);
    lv_group_set_default(indev_group);
    lv_obj_set_style_bg_color(lv_scr_act(), lv_color_black(), 0);
    
    lv_obj_t* title = lv_label_create(lv_scr_act());
    lv_obj_set_style_text_color(title, LABEL_TEXT_COLOR, 0);
    lv_obj_set_style_text_font(title, Font_ResouceGet(FONT_22), 0);    
    lv_label_set_text(title, Lang_GetStringByID(STRING_ID_MENU));
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 20);

    menu_panel = lv_obj_create(lv_scr_act());
    lv_obj_remove_style_all(menu_panel);
    lv_obj_set_size(menu_panel, 260, 104);
    lv_obj_align(menu_panel, LV_ALIGN_CENTER, 0, -10);
    lv_obj_add_event_cb(menu_panel, menu_panel_event_cb, LV_EVENT_ALL, NULL);

    panel_w = lv_obj_get_style_width(menu_panel, 0);
    panel_h = lv_obj_get_style_height(menu_panel, 0);
    disp_nums = (panel_w - BTN_FOCUSED_WIDTH) / (BTN_NORMAL_WIDTH + OBJ_GAP_WIDTH) + 1;    
    focus_com_y = (panel_h - BTN_FOCUSED_WIDTH) / 2;
    com_y = (panel_h - BTN_NORMAL_WIDTH) / 2;
    inv_pos_left_x = 0 - BTN_NORMAL_WIDTH - OBJ_GAP_WIDTH;
    inv_pos_right_x = panel_w;
    anim_obj_nums = disp_nums + 1;
//    zoom_coff = BTN_NORMAL_WIDTH * 100 / BTN_FOCUSED_WIDTH;

    lv_obj_t* obj;
    uint8_t index = screen_get_act_index() < MENU_ITEM_ID_MAX ? screen_get_act_index() : MENU_ITEM_ID_MAX - MENU_ITEM_ID_MAX;

    for (uint8_t id = 0; id < MENU_ITEM_ID_MAX; id++)
    {
//        if (id == MENU_ITEM_ID_ACCESSORY && g_ptUIStateData->ele_accessory.electric_accessory_type == 0)  //chen
//        {
//            index = index == MENU_ITEM_ID_ACCESSORY ? MENU_ITEM_ID_OUTPUT_MODE : index;
//            focused_item = &items[index];
//            continue;
//        }

        obj = menu_item_obj_create(menu_panel);        
        lv_obj_add_style(obj, &style_sys_menu_btn, 0);
        lv_obj_set_style_bg_color(obj, RED_THEME_COLOR, LV_STATE_FOCUSED);
        lv_obj_set_size(obj, BTN_NORMAL_WIDTH, BTN_NORMAL_WIDTH);
        lv_obj_set_style_img_recolor(obj,BLUE_THEME_COLOR,0);
        
        menu_item_obj_set_param(obj, item_img[id], BTN_NORMAL_WIDTH, BTN_FOCUSED_WIDTH);
        lv_obj_set_pos(obj, inv_pos_left_x, com_y);        

        item_add(&item_list_head, &items[id], id, obj, &item_img[id], item_get_str(id));

        if (index == id) 
            focused_item = &items[id];
    }

    upd_disp_obj_dsc();

    LV_IMG_DECLARE(ImgArrowLeft)
    lv_obj_t* img_arrow_l = lv_img_create(lv_scr_act());
    lv_img_set_src(img_arrow_l, &ImgArrowLeft);
    lv_obj_set_style_img_recolor_opa(img_arrow_l, LV_OPA_MAX, 0);
    lv_obj_set_style_img_recolor(img_arrow_l, lv_color_make(255, 255, 255), 0);
    lv_obj_align_to(img_arrow_l, menu_panel, LV_ALIGN_OUT_LEFT_MID, -8, 0);

    lv_obj_t* img_arrow_r = lv_img_create(lv_scr_act());
    lv_img_set_src(img_arrow_r, &ImgArrowLeft);
    lv_img_set_angle(img_arrow_r, 1800);
    lv_obj_set_style_img_recolor_opa(img_arrow_r, LV_OPA_MAX, 0);
    lv_obj_set_style_img_recolor(img_arrow_r, lv_color_make(255, 255, 255), 0);
    lv_obj_align_to(img_arrow_r, menu_panel, LV_ALIGN_OUT_RIGHT_MID, 8, 0);
    
    name_disp_label = lv_label_create(lv_scr_act());
    lv_obj_set_style_text_color(name_disp_label, LABEL_TEXT_COLOR, 0);
    lv_obj_set_style_text_font(name_disp_label, Font_ResouceGet(FONT_22_BOLD), 0);
    refresh_disp_label();

    dots_cont = dots_create(lv_scr_act(), MENU_ITEM_ID_MAX);
    lv_obj_align_to(dots_cont, name_disp_label, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);
    refresh_dots(dots_cont, get_focused_item_id());

    lv_group_add_obj(indev_group, menu_panel);
    lv_group_focus_obj(menu_panel);
    lv_group_set_editing(indev_group, true);

    lv_obj_add_state(focused_item->obj, LV_STATE_FOCUSED);
    lv_event_send(focused_item->obj, LV_EVENT_FOCUSED, NULL);
}

static void refresh_disp_label(void)
{
    lv_label_set_text(name_disp_label, get_focused_item_name());
    lv_obj_align_to(name_disp_label, menu_panel, LV_ALIGN_OUT_BOTTOM_MID, 0, 12);
}

static void upd_disp_obj_dsc(void)
{      
    lv_coord_t start_x = 0;
    int index = 0 - (disp_nums / 2);
    item_t *tmp_item;
    lv_point_t pos;
    lv_coord_t width;

    for (uint8_t i = 0; i < disp_nums; i++, index++)
    {                        
        tmp_item = item_list_get(focused_item, index);

        pos.x = start_x;
        if (tmp_item == focused_item)
        {
            pos.y = focus_com_y;
            width = BTN_FOCUSED_WIDTH;
            start_x += (BTN_FOCUSED_WIDTH + OBJ_GAP_WIDTH);
        }
        else
        {
            pos.y = com_y;
            width = BTN_NORMAL_WIDTH;
            start_x += (BTN_NORMAL_WIDTH + OBJ_GAP_WIDTH);
        }

        lv_obj_get_child(tmp_item->obj, 0);
        lv_obj_set_size(tmp_item->obj, width, width);
        lv_obj_set_pos(tmp_item->obj, pos.x, pos.y);               
        disp_pos_x[i] = pos.x;
    }
}

static void upd_cw_anim_obj(void)
{
    item_dsc_t *p_anim_dsc = &cur_anim_item[0];
    lv_coord_t start_x = 0;
    uint8_t cnt = 0;
    int index = -(disp_nums / 2 + 1);
    int disp_index = -(disp_nums / 2);

    (void)start_x;

    //计算起始位置 顺时针 CW方向 查找当前显示的对象
    for (cnt = 0; cnt < MAX_DISP_OBJS; cnt++)
    {
        if (cnt < disp_nums) 
        {
            p_anim_dsc->item = item_list_get(focused_item, disp_index);
            p_anim_dsc->start_pos.x = disp_pos_x[cnt];
            
            if (p_anim_dsc->item == focused_item)
            {
                p_anim_dsc->start_pos.y = focus_com_y;
            }
            else 
            {
                p_anim_dsc->start_pos.y = com_y;
            }

            lv_obj_clear_flag(p_anim_dsc->item->obj, LV_OBJ_FLAG_HIDDEN);
            p_anim_dsc++; disp_index++;

        }
        else 
        {
            item_t * tmp = item_list_get(focused_item, cnt);
            if (tmp != NULL)
                lv_obj_add_flag(tmp->obj, LV_OBJ_FLAG_HIDDEN);
        }
    }

    // 最右边不可见位置
    p_anim_dsc->start_pos.x = inv_pos_right_x + 4;
    p_anim_dsc->start_pos.y = com_y;

    // 聚焦下一个对象, 计算结束位置
    p_anim_dsc = &cur_anim_item[0];
    focused_item = item_list_get_next(focused_item, 1);
    
    for (cnt = 0; cnt < anim_obj_nums; cnt++, p_anim_dsc++, index++)
    {
        p_anim_dsc->item = item_list_get(focused_item, index);   

        if (p_anim_dsc->item == focused_item)
        {            
            p_anim_dsc->end_pos.x = p_anim_dsc->start_pos.x - BTN_FOCUSED_WIDTH - OBJ_GAP_WIDTH;
            p_anim_dsc->end_pos.y = focus_com_y;
            p_anim_dsc->width = BTN_FOCUSED_WIDTH;
            p_anim_dsc->bfocus = true;
        }
        else 
        {            
            p_anim_dsc->end_pos.x = p_anim_dsc->start_pos.x - BTN_NORMAL_WIDTH - OBJ_GAP_WIDTH;
            p_anim_dsc->end_pos.y = com_y;            
            p_anim_dsc->width = BTN_NORMAL_WIDTH;
            p_anim_dsc->bfocus = false;
        }    
    }

}

static void upd_ccw_anim_obj(void)
{
    item_dsc_t *p_anim_dsc = &cur_anim_item[0];
    uint8_t cnt = 0;
    lv_coord_t start_x = inv_pos_left_x;
    int index = -(disp_nums / 2);
    int disp_index = -(disp_nums / 2);

    p_anim_dsc->start_pos.x = start_x;
    p_anim_dsc->start_pos.y = com_y;
    p_anim_dsc++;

    //逆时针方向 CCW
    for (cnt = 0; cnt < MAX_DISP_OBJS; cnt++)
    {
        if (cnt < disp_nums) 
        {
            p_anim_dsc->item = item_list_get(focused_item, disp_index);
            //p_anim_dsc->start_pos.x = lv_obj_get_style_x(p_anim_dsc->item->obj, 0);
            p_anim_dsc->start_pos.x = disp_pos_x[cnt];
            //p_anim_dsc->start_pos.y = lv_obj_get_style_y(p_anim_dsc->item->obj, 0);
            if (p_anim_dsc->item == focused_item)
            {
                p_anim_dsc->start_pos.y = focus_com_y;
            }
            else 
            {
                p_anim_dsc->start_pos.y = com_y;
            }       
            lv_obj_clear_flag(p_anim_dsc->item->obj, LV_OBJ_FLAG_HIDDEN);
            p_anim_dsc++; disp_index++;
        }
        else  
        {
            item_t * tmp = item_list_get(focused_item, cnt);
            if (tmp != NULL)
                lv_obj_add_flag(tmp->obj, LV_OBJ_FLAG_HIDDEN);            
        }
 
    }

    // 聚焦下一个对象, 计算结束位置
    //last_focused_item = focused_item;
    p_anim_dsc = &cur_anim_item[0];
    focused_item = item_list_get_prev(focused_item, 1);
    
    for (cnt = 0; cnt < anim_obj_nums; cnt++, p_anim_dsc++, index++)
    {
        p_anim_dsc->item = item_list_get(focused_item, index);   

        //UI_PRINTF("width: %d\r\n", lv_obj_get_style_width(p_anim_dsc->item->obj, 0));
        p_anim_dsc->end_pos.x = p_anim_dsc->start_pos.x + lv_obj_get_style_width(p_anim_dsc->item->obj, 0) + OBJ_GAP_WIDTH;
        if (p_anim_dsc->item == focused_item)
        {            
            p_anim_dsc->end_pos.y = focus_com_y;
            p_anim_dsc->width = BTN_FOCUSED_WIDTH;
            p_anim_dsc->bfocus = true;
        }
        else 
        {                        
            p_anim_dsc->end_pos.y = com_y;
            p_anim_dsc->width = BTN_NORMAL_WIDTH;
            p_anim_dsc->bfocus = false;
        }

        // UI_PRINTF("start_x:%d start_y:%d end_x:%d end_y:%d\r\n", p_anim_dsc->start_pos.x, p_anim_dsc->start_pos.y, p_anim_dsc->end_pos.x, p_anim_dsc->end_pos.y);
    }
}
