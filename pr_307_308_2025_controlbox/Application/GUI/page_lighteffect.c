/*********************
 *      INCLUDES
 *********************/
#include "ui_common.h"
#include "page_widget.h"
#include "menu_item_obj.h"

/*********************
 *      DEFINES
 *********************/
#define IMG_COLOR_FOCUSED   lv_color_make(255, 255, 255)
#define IMG_COLOR_DEFAULT   lv_color_make(255, 255, 255)

#define FONT_COLOR          lv_color_make(255, 255, 255)
#define MAX_DISP_OBJS       15

#define ITEM_CONT_WIDTH          lv_obj_get_width(lv_scr_act())
#define ITEM_CONT_HEIGHT         (lv_obj_get_width(lv_scr_act()) - 40)    
#define ITEM_MAX_WIDTH           63

#define ANIMATION_TIME               500
/**********************
 *      TYPEDEFS
 **********************/
LV_IMG_DECLARE(ImgCandle)
LV_IMG_DECLARE(ImgClubLights)
LV_IMG_DECLARE(ImgCopCar)
LV_IMG_DECLARE(ImgColorChase)
LV_IMG_DECLARE(ImgFaultyBulb)
LV_IMG_DECLARE(ImgFireWorks)
LV_IMG_DECLARE(ImgFire)
LV_IMG_DECLARE(ImgLightning)
LV_IMG_DECLARE(ImgPaparazzi)
LV_IMG_DECLARE(ImgExplosion)
LV_IMG_DECLARE(ImgPulsing)
LV_IMG_DECLARE(ImgParty)
LV_IMG_DECLARE(ImgStrobe)
LV_IMG_DECLARE(ImgTV)
LV_IMG_DECLARE(ImgWelding)

#if PROJECT_TYPE==307
typedef struct _item_dsc {
    item_t *item;    
    lv_point_t start_pos;
    lv_point_t end_pos;    
    lv_coord_t start_width;        
    lv_coord_t end_width;
    uint16_t start_coff;
    uint16_t end_coff;
}item_dsc_t;


/**********************
 *  STATIC PROTOTYPES
 **********************/
static void item_cont_event_cb(lv_event_t *e);
static void item_event_handler(item_id_t id);
static void upd_item_cont_circle(void);
static void item_list_print(void);
static void upd_cw_anim_obj(void);
static void animator_op_obj(void);
static void upd_ccw_anim_obj(void);
static void refresh_le_label(void);

static void page_light_effect_construct(void);
static void page_light_effect_destruct(void);
/**********************
 *  STATIC VARIABLES
 **********************/

static const lv_point_t cw_start_pos[] = 
{
    {125, 135},
    {210, 129},
    {266, 103},
    {273, 61},
    {246, 38},
    {216, 27},
    {188, 23},
    {163, 23},
    {137, 23},
    {108, 23},
    {76, 27},
    {42, 37},
    {8, 60},
    {0,103},
    {58, 129},
};

static const lv_point_t cw_end_pos[] = 
{
    {58, 129},
    {125, 135},
    {210, 129},
    {266, 103},
    {273, 61},
    {246, 38},
    {216, 27},
    {188, 23},
    {163, 23},
    {137, 23},
    {108, 23},
    {76, 27},
    {42, 37},
    {8, 60},
    {0,103},
};

static const lv_point_t ccw_start_pos[] = 
{
    {125, 135},
    {210, 129},
    {266, 103},
    {273, 61},
    {246, 38},
    {216, 27},
    {188, 23},
    {163, 23},
    {137, 23},
    {108, 23},
    {76, 27},
    {42, 37},
    {8, 60},
    {0,103},
    {58, 129},
};

static const lv_point_t ccw_end_pos[] = 
{
    {210, 129},
    {266, 103},
    {273, 61},
    {246, 38},
    {216, 27},
    {188, 23},
    {163, 23},
    {137, 23},
    {108, 23},
    {76, 27},
    {42, 37},
    {8, 60},
    {0,103},
    {58, 129},
    {125, 135},
};

static const lv_coord_t cw_start_width[] = 
{
    69,
    56,
    46,
    39,
    33,
    27,
    22,
    17,
    17,
    22,
    27,
    33,
    39,
    46,
    56,
};

static const lv_coord_t cw_end_width[] = 
{
    56,
    69,
    56,
    46,
    39,
    33,
    27,
    22,
    17,
    17,
    22,
    27,
    33,
    39,
    46,
};

static const lv_coord_t ccw_start_width[] = 
{
    69,
    56,
    46,
    39,
    33,
    27,
    22,
    17,
    17,
    22,
    27,
    33,
    39,
    46,
    56,
};

static const lv_coord_t ccw_end_width[] = 
{
    56,
    46,
    39,
    33,
    27,
    22,
    17,
    17,
    22,
    27,
    33,
    39,
    46,
    56,
    69,
};

// static const lv_coord_t img_zoom_width[] = 
// {
//     63,
//     55,
//     47,
//     39,
//     31,
//     23,
//     15,
//     7,
//     7,
//     15,
//     23,
//     31,
//     39,
//     47,
//     55,
// };

static const lv_img_dsc_t *item_imgs[] = 
{
    [ITEM_ID_FIRE] = &ImgFire,
    [ITEM_ID_PAPARAZZI] = &ImgPaparazzi,
    [ITEM_ID_FAULT_BULB] = &ImgFaultyBulb,
    [ITEM_ID_TV] = &ImgTV,
    [ITEM_ID_PULSING] = &ImgPulsing,
    [ITEM_ID_PARTY] = &ImgParty,
    [ITEM_ID_CLUB_LIGHTS] = &ImgClubLights,
    [ITEM_ID_COLOR_CHASE] = &ImgColorChase,
    [ITEM_ID_CANDLE] = &ImgCandle,
    [ITEM_ID_WELDING] = &ImgWelding,
    [ITEM_ID_COP_CAR] = &ImgCopCar,
    [ITEM_ID_STROBE] = &ImgStrobe,
    [ITEM_ID_LIGHTNING] = &ImgLightning,
    [ITEM_ID_FIREWORKS] = &ImgFireWorks,
    [ITEM_ID_EXPLOSIONS] = &ImgExplosion,
};

static item_dsc_t cur_anim_items[MAX_DISP_OBJS];
static item_t* item_list_head = NULL;
static item_t* focused_item = NULL;
static lv_group_t* indev_group;
static item_t item_array[ITEM_ID_MAX];
static lv_obj_t* item_cont;
static lv_obj_t* le_name_label;
static bool anim_is_running = false;
static bool fx_menu_exited = false;
/*********************
 *  GLOBAL VATIABLES
 *********************/
page_t page_light_effect = PAGE_INITIALIZER("light effect",
                                PAGE_LIGHT_EFFECT_MENU, 
                                page_light_effect_construct, 
                                page_light_effect_destruct, 
                                NULL,
                                NULL,                                
                                PAGE_NONE);
/**********************
 *   GLOBAL FUNCTIONS
 **********************/
void light_effect_menu_create(void)
{
    indev_group = lv_group_create();
    my_indev_set_group(indev_group);
    lv_group_set_default(indev_group);

    lv_obj_set_style_bg_color(lv_scr_act(), lv_color_black(), 0);
    lv_obj_clear_flag(lv_scr_act(), LV_OBJ_FLAG_SCROLLABLE);

    item_cont = lv_obj_create(lv_scr_act());
    lv_obj_remove_style_all(item_cont);
    lv_obj_set_size(item_cont, 320, 220);

    LV_IMG_DECLARE(ImgLightEffectBg);
    lv_obj_align(item_cont, LV_ALIGN_TOP_MID, 0, 0);    
    lv_obj_add_event_cb(item_cont, item_cont_event_cb, LV_EVENT_ALL, NULL);
    
    le_name_label =lv_label_create(lv_scr_act());
    lv_obj_set_style_text_font(le_name_label, Font_ResouceGet(FONT_22_BOLD), 0);
    lv_obj_set_style_text_color(le_name_label, lv_color_white(), 0);

    lv_obj_t* bg_img = lv_img_create(item_cont);
    lv_img_set_src(bg_img, &ImgLightEffectBg);
    lv_obj_set_pos(bg_img, 20, 29);

    lv_obj_t* obj;
    uint8_t index;
    /* 获取上一次的索引 */
    index = ui_light_effect_index_get();
	if(g_ptUIStateData->firework_reset == 1)
	{
		g_ptUIStateData->firework_reset = 0;
		index = ITEM_ID_FIREWORKS;
	}
    for (int i = 0; i < ITEM_ID_MAX; i++)
    {
        obj = menu_item_obj_create(item_cont);
        lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);        
        lv_obj_add_style(obj, &style_le_btn, 0);
        lv_obj_set_style_bg_color(obj, lv_color_make(51, 51, 51), LV_STATE_DEFAULT);
        lv_obj_set_style_bg_color(obj, RED_THEME_COLOR, LV_STATE_FOCUSED);
        lv_obj_set_size(obj, cw_start_width[i], cw_start_width[i]);
        
        item_add(&item_list_head, &item_array[i], i, obj, item_imgs[i], lighteffect_get_item_str(i));                
        menu_item_obj_set_param(obj, item_imgs[i], 0, ITEM_MAX_WIDTH);
        menu_item_obj_set_ratio(obj, 0, 256);
        
        if (index == i)
            focused_item = &item_array[i];
    }

    upd_item_cont_circle();
    refresh_le_label();

    lv_group_add_obj(indev_group, item_cont);
    lv_group_focus_obj(item_cont);
    lv_group_set_editing(indev_group, true);

    lv_obj_add_state(focused_item->obj, LV_STATE_FOCUSED);
    lv_event_send(focused_item->obj, LV_EVENT_FOCUSED, NULL);

    item_list_print();
}

const char* lighteffect_get_item_str(uint8_t id)
{
    const char* tmp = NULL;
    switch(id)
    {
        case ITEM_ID_FIRE:       tmp = Lang_GetStringByID(STRING_ID_FIRE); break;
        case ITEM_ID_PAPARAZZI:  tmp = Lang_GetStringByID(STRING_ID_PAPARAZZI); break;
        case ITEM_ID_FAULT_BULB: tmp = Lang_GetStringByID(STRING_ID_FAULT_BULB); break;
        case ITEM_ID_TV:         tmp = Lang_GetStringByID(STRING_ID_TV); break;
        case ITEM_ID_PULSING:    tmp = Lang_GetStringByID(STRING_ID_PULSING); break;
        case ITEM_ID_PARTY:      tmp = Lang_GetStringByID(STRING_ID_PARTY); break;
        case ITEM_ID_CLUB_LIGHTS:tmp = Lang_GetStringByID(STRING_ID_CLUB_LIGHTS); break;
        case ITEM_ID_COLOR_CHASE:tmp = Lang_GetStringByID(STRING_ID_COLOR_CHASE); break;
        case ITEM_ID_CANDLE:     tmp = Lang_GetStringByID(STRING_ID_CANDLE); break;
        case ITEM_ID_WELDING:    tmp = Lang_GetStringByID(STRING_ID_WELDING); break;
        case ITEM_ID_COP_CAR:    tmp = Lang_GetStringByID(STRING_ID_COP_CAR); break;
        case ITEM_ID_STROBE:     tmp = Lang_GetStringByID(STRING_ID_STROBE); break;
        case ITEM_ID_LIGHTNING:  tmp = Lang_GetStringByID(STRING_ID_LIGHTNING); break;
        case ITEM_ID_FIREWORKS:  tmp = Lang_GetStringByID(STRING_ID_FIREWORKS); break;
        case ITEM_ID_EXPLOSIONS: tmp = Lang_GetStringByID(STRING_ID_EXPLOSIONS); break;
        default:break;
    }

    return tmp;
}

/**
 * @brief 获取光效的索引
 * 
 * @param pid 
 * @return uint8_t 
 */
uint8_t light_effect_get_index(uint8_t pid)
{
    uint8_t item_id = 0;

    switch(pid)
    {
        case PAGE_FIRE:          item_id = ITEM_ID_FIRE; break;
        case PAGE_PAPARAZZI:     item_id = ITEM_ID_PAPARAZZI; break;
        case PAGE_FAULTYBULB:    item_id = ITEM_ID_FAULT_BULB; break;
        case PAGE_TV:            item_id = ITEM_ID_TV; break;
        case PAGE_PULSING:       item_id = ITEM_ID_PULSING; break;
        case PAGE_PARTYLIGHT:    item_id = ITEM_ID_PARTY; break;
        case PAGE_CLUBLIGHTS:    item_id = ITEM_ID_CLUB_LIGHTS; break;
        case PAGE_COLORCHASE:    item_id = ITEM_ID_COLOR_CHASE; break;
        case PAGE_CANDLE:        item_id = ITEM_ID_CANDLE; break;
        case PAGE_WELDING:       item_id = ITEM_ID_WELDING; break;
        case PAGE_COPCAR:        item_id = ITEM_ID_COP_CAR; break;
        case PAGE_STROBE:        item_id = ITEM_ID_STROBE; break;
        case PAGE_LIGHTNING:     item_id = ITEM_ID_LIGHTNING; break;
        case PAGE_FIREWORKS:     item_id = ITEM_ID_FIREWORKS; break;
        case PAGE_EXPLOSIOIN:    item_id = ITEM_ID_EXPLOSIONS; break;
        default: break;
    }

    return item_id;    
}

/**
 * @brief 根据光效ID获取ItemID
 * 
 * @param item_id 
 * @return uint8_t 
 */
uint8_t light_effect_get_pid(uint8_t item_id)
{
    uint8_t page_id = PAGE_FIRE;
    switch(item_id)
    {
        case ITEM_ID_FIRE:          page_id = PAGE_FIRE; break;
        case ITEM_ID_PAPARAZZI:     page_id = PAGE_PAPARAZZI; break;
        case ITEM_ID_FAULT_BULB:    page_id = PAGE_FAULTYBULB; break;
        case ITEM_ID_TV:            page_id = PAGE_TV; break;
        case ITEM_ID_PULSING:       page_id = PAGE_PULSING; break;
        case ITEM_ID_PARTY:         page_id = PAGE_PARTYLIGHT; break;
        case ITEM_ID_CLUB_LIGHTS:   page_id = PAGE_CLUBLIGHTS; break;
        case ITEM_ID_COLOR_CHASE:   page_id = PAGE_COLORCHASE; break;
        case ITEM_ID_CANDLE:        page_id = PAGE_CANDLE; break;
        case ITEM_ID_WELDING:       page_id = PAGE_WELDING; break;
        case ITEM_ID_COP_CAR:       page_id = PAGE_COPCAR; break;
        case ITEM_ID_STROBE:        page_id = PAGE_STROBE; break;
        case ITEM_ID_LIGHTNING:     page_id = PAGE_LIGHTNING; break;
        case ITEM_ID_FIREWORKS:     page_id = PAGE_FIREWORKS; break;
        case ITEM_ID_EXPLOSIONS:    page_id = PAGE_EXPLOSIOIN; break;
        default: break;
    }
    return page_id;
}


/**********************
 *  STATIC FUNCTIONS
 **********************/
static void page_light_effect_construct(void)
{
    anim_is_running = false;
    fx_menu_exited = false;
    light_effect_menu_create();
}

static void page_light_effect_destruct(void)
{
    lv_group_focus_freeze(indev_group, true);
    lv_group_remove_all_objs(indev_group);
    lv_group_del(indev_group);
    indev_group = NULL;
    le_name_label = NULL;
    fx_menu_exited = true;

    lv_obj_clean(lv_scr_act());
}
static void anim_pos_x_cb(void* var, int32_t v)
{
    item_dsc_t* pdsc = lv_obj_get_user_data(var);
    lv_obj_t* img = lv_obj_get_child(pdsc->item->obj, 0);
    int32_t cur_distance = LV_ABS(v - pdsc->start_pos.x);
    int32_t distance = LV_ABS(pdsc->start_pos.x - pdsc->end_pos.x);
    int32_t width = LV_ABS(pdsc->end_width - pdsc->start_width);
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

    if (pdsc->start_width > pdsc->end_width)
    {
        target_width = pdsc->start_width - lv_map(cur_distance, 0, distance, 0, width);
    }
    else
    {
        target_width = pdsc->start_width + lv_map(cur_distance, 0, distance, 0, width);
    }

    lv_obj_set_size(var, target_width, target_width);
}

static void item_cont_event_cb(lv_event_t *e)
{
    lv_obj_t* obj = lv_event_get_target(e);
    lv_event_code_t event = lv_event_get_code(e);

    if (!anim_is_running)
    {
        shortcut_keys_event_handler(e);
        backspace_key_event_handler(e);
    }

    if (LV_EVENT_KEY == event)
    {
        uint32_t key = *(uint32_t*)lv_event_get_param(e);

        if (LV_ENCODER_KEY_CW == key && !anim_is_running)
        {                      
            lv_obj_clear_state(focused_item->obj, LV_STATE_FOCUSED);
            lv_event_send(focused_item->obj, LV_EVENT_DEFOCUSED, NULL);

            upd_cw_anim_obj();
            animator_op_obj();              
            
            /* 保存光效索引 */
            ui_light_effect_index_save(focused_item->id);
        }
        else if (LV_ENCODER_KEY_CC == key && !anim_is_running)
        {
            lv_obj_clear_state(focused_item->obj, LV_STATE_FOCUSED);
            lv_event_send(focused_item->obj, LV_EVENT_DEFOCUSED, NULL);
            
            upd_ccw_anim_obj();
            animator_op_obj();            

            /* 保存光效索引 */
            ui_light_effect_index_save(focused_item->id);
        }
    }
    else if (LV_EVENT_RELEASED == event && !anim_is_running)    
    {        
        item_event_handler(focused_item->id);
    }
}

static void item_list_print(void)
{
    item_t* item = item_list_head;
  
    while(item->next != item_list_head)
    {
        UI_PRINTF("id : %d\r\n", item->id);
        item = item->next;
    }  
}

static void upd_cw_anim_obj(void)
{
    item_dsc_t* pdsc = &cur_anim_items[0];
    
    for (int i = 0; i < ITEM_ID_MAX; i++, pdsc++)
    {
        pdsc->item = item_list_get(focused_item, i);
        pdsc->start_pos = cw_start_pos[i];
        pdsc->end_pos = cw_end_pos[i];
        pdsc->start_width = cw_start_width[i];
        pdsc->end_width = cw_end_width[i];
        pdsc->start_coff = lv_map(cw_start_width[i], 0, ITEM_MAX_WIDTH, 0, 256);  
        pdsc->end_coff = lv_map(cw_end_width[i], 0, ITEM_MAX_WIDTH, 0, 256);      
        menu_item_obj_set_param(pdsc->item->obj, NULL, pdsc->start_width, pdsc->end_width);
        menu_item_obj_set_ratio(pdsc->item->obj,  pdsc->start_coff, pdsc->end_coff);              
    }

    focused_item = item_list_get_next(focused_item, 1);
}

static void upd_ccw_anim_obj(void)
{
    item_dsc_t* pdsc = &cur_anim_items[0];
   
    for (int i = 0; i < ITEM_ID_MAX; i++, pdsc++)
    {
        pdsc->item = item_list_get(focused_item, i);
        pdsc->start_pos = ccw_start_pos[i];
        pdsc->end_pos = ccw_end_pos[i];
        pdsc->start_width = ccw_start_width[i];
        pdsc->end_width = ccw_end_width[i];
        pdsc->start_coff = lv_map(ccw_start_width[i], 0, ITEM_MAX_WIDTH, 0, 256);  
        pdsc->end_coff = lv_map(ccw_end_width[i], 0, ITEM_MAX_WIDTH, 0, 256);    
        menu_item_obj_set_param(pdsc->item->obj, NULL, pdsc->start_width, pdsc->end_width);
        menu_item_obj_set_ratio(pdsc->item->obj,  pdsc->start_coff, pdsc->end_coff);
    }

    focused_item = item_list_get_prev(focused_item, 1);
}

static void anim_finish_cb(struct _lv_anim_t* a)
{
    anim_is_running = false;
    if (fx_menu_exited)
        return; 

    refresh_le_label();        
    lv_obj_add_state(focused_item->obj, LV_STATE_FOCUSED);
    lv_event_send(focused_item->obj, LV_EVENT_FOCUSED, NULL);     
}

static void animator_op_obj(void)
{
    item_dsc_t* pdsc = &cur_anim_items[0];
    lv_anim_t a;
    lv_coord_t width;
    
    anim_is_running = true;
    for (int i = 0; i < ITEM_ID_MAX; i++, pdsc++)
    {
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

    lv_anim_init(&a);
    lv_anim_set_var(&a, NULL);
    lv_anim_set_exec_cb(&a, NULL);
    lv_anim_set_ready_cb(&a, anim_finish_cb);
    lv_anim_set_repeat_count(&a, 0);
    lv_anim_set_time(&a, ANIMATION_TIME + 50);
    lv_anim_set_playback_time(&a, 0);
    lv_anim_start(&a);
}

static void upd_item_cont_circle(void)
{
    item_t *tmp;    
    for (int i = 0; i < ITEM_ID_MAX; i++)
    {
        tmp = item_list_get_next(focused_item, i);
        lv_obj_set_pos(tmp->obj, cw_start_pos[i].x, cw_start_pos[i].y);
        lv_obj_set_size(tmp->obj, cw_start_width[i], cw_start_width[i]);
        UI_PRINTF("ID:%d name:%s pos_x:%d pos_y:%d coff:%d\r\n", i,tmp->name, cw_start_pos[i].x, cw_start_pos[i].y, coff);
    }
}

static void refresh_le_label(void)
{   
    if (focused_item == NULL)   return;
    if (le_name_label == NULL)  return;


    const char* name = lighteffect_get_item_str(focused_item->id);
    lv_label_set_text(le_name_label, name);
    lv_obj_align_to(le_name_label, item_cont, LV_ALIGN_OUT_BOTTOM_MID, 0, -10);
}


static void item_event_handler(item_id_t item_id)
{
    uint8_t page_id = light_effect_get_pid(item_id);
    screen_load_page(page_id, 0, true);
}
#elif PROJECT_TYPE==308

//LV_IMG_DECLARE(effects_menu_bg);

//#define EFFECTS_ARC1_X     117
//#define EFFECTS_ARC2_X     45
//#define EFFECTS_ARC3_X     12
//#define EFFECTS_ARC4_X     55
//#define EFFECTS_ARC5_X     112
//#define EFFECTS_ARC6_X     170
//#define EFFECTS_ARC7_X     225
//#define EFFECTS_ARC8_X     264
//#define EFFECTS_ARC9_X     215

//#define EFFECTS_ARC1_Y     115
//#define EFFECTS_ARC2_Y     115
//#define EFFECTS_ARC3_Y     77
//#define EFFECTS_ARC4_Y     38
//#define EFFECTS_ARC5_Y     25
//#define EFFECTS_ARC6_Y     25
//#define EFFECTS_ARC7_Y     38
//#define EFFECTS_ARC8_Y     77
//#define EFFECTS_ARC9_Y     115

//#define     ANIM_TIME   (250) 

//typedef __packed struct 
//{
//        uint8_t  text_id;       /* 主菜单索引 */
//		const uint8_t *str;
//}effects_menu_text_index;

////光效菜单索引
//typedef enum 
//{
//        PAPARAZZI_INDEX = 0,   /* 狗仔队     0*/
//        FIREWORKS_INDEX,       /* 烟花       1*/
//        FAULTY_INDEX,          /* 故障灯泡   2*/
//        LIGHTING_INDEX,        /* 闪电       3*/
//        TV_INDEX,              /* 电视       4*/
//        PULSING_INDEX,         /* 脉搏       5*/
//        STROBZ_INDEX,          /* 闪光灯     6*/
//		FRIE_INDEX,            /* 火光       7*/
//        EXPLOSION_INDEX,       /* 爆炸       8*/
//		EFFECTS_MAX,           /*            9*/
//}effects_menu_index_typedef;

///* 字符对象 */
//static lv_obj_t *effects_menu_mid_str;

//lv_obj_t    *effects_menu_rolling_box[9];

///* 图片对象 */
//static lv_obj_t * img_effects[9];

////圆形大小
//static uint8_t arc_size[EFFECTS_MAX] = {
//	84,
//	59,
//	42,
//	36,
//	34,
//	34,
//	36,
//	42,
//	59,
//};

////圆形x，y位置
//static int16_t arc_pos[EFFECTS_MAX][2] = {
//	{EFFECTS_ARC1_X, EFFECTS_ARC1_Y},
//	{EFFECTS_ARC2_X, EFFECTS_ARC2_Y},
//	{EFFECTS_ARC3_X, EFFECTS_ARC3_Y},
//	{EFFECTS_ARC4_X, EFFECTS_ARC4_Y},
//	{EFFECTS_ARC5_X, EFFECTS_ARC5_Y},
//	{EFFECTS_ARC6_X, EFFECTS_ARC6_Y},
//	{EFFECTS_ARC7_X, EFFECTS_ARC7_Y},
//	{EFFECTS_ARC8_X, EFFECTS_ARC8_Y},
//	{EFFECTS_ARC9_X, EFFECTS_ARC9_Y},
//};

//static int16_t arc_size_offset[EFFECTS_MAX][2] = {
//	{84, 59},
//	{59, 42},
//	{42, 36},
//	{36, 34},
//	{34, 34},
//	{34, 36},
//	{36, 42},
//	{42, 59},
//	{59, 84},
//};

//static int16_t img_size[EFFECTS_MAX] = {
//	320,
//	240,
//	160,
//	120,
//	120,
//	120,
//	120,
//	160,
//	240,
//};

//static int16_t img_size_offset[EFFECTS_MAX][2] = {
//	{320, 240},
//	{240, 160},
//	{160, 120},
//	{120, 120},
//	{120, 120},
//	{120, 120},
//	{120, 160},
//	{160, 240},
//	{240, 320},
//};

//static int16_t arc_cw_x_offset[EFFECTS_MAX][2] = {
//	{EFFECTS_ARC1_X, EFFECTS_ARC2_X},
//	{EFFECTS_ARC2_X, EFFECTS_ARC3_X},
//	{EFFECTS_ARC3_X, EFFECTS_ARC4_X},
//	{EFFECTS_ARC4_X, EFFECTS_ARC5_X},
//	{EFFECTS_ARC5_X, EFFECTS_ARC6_X},
//	{EFFECTS_ARC6_X, EFFECTS_ARC7_X},
//	{EFFECTS_ARC7_X, EFFECTS_ARC8_X},
//	{EFFECTS_ARC8_X, EFFECTS_ARC9_X},
//	{EFFECTS_ARC9_X, EFFECTS_ARC1_X},
//};

//static int16_t arc_y_offset[EFFECTS_MAX][2] = {
//	{EFFECTS_ARC1_Y, EFFECTS_ARC2_Y},
//	{EFFECTS_ARC2_Y, EFFECTS_ARC3_Y},
//	{EFFECTS_ARC3_Y, EFFECTS_ARC4_Y},
//	{EFFECTS_ARC4_Y, EFFECTS_ARC5_Y},
//	{EFFECTS_ARC5_Y, EFFECTS_ARC6_Y},
//	{EFFECTS_ARC6_Y, EFFECTS_ARC7_Y},
//	{EFFECTS_ARC7_Y, EFFECTS_ARC8_Y},
//	{EFFECTS_ARC8_Y, EFFECTS_ARC9_Y},
//	{EFFECTS_ARC9_Y, EFFECTS_ARC1_Y},
//};

//static int16_t arc_ccw_x_offset[EFFECTS_MAX][2] = {
//	{EFFECTS_ARC1_X, EFFECTS_ARC9_X},
//	{EFFECTS_ARC9_X, EFFECTS_ARC8_X},
//	{EFFECTS_ARC8_X, EFFECTS_ARC7_X},
//	{EFFECTS_ARC7_X, EFFECTS_ARC6_X},
//	{EFFECTS_ARC6_X, EFFECTS_ARC5_X},
//	{EFFECTS_ARC5_X, EFFECTS_ARC4_X},
//	{EFFECTS_ARC4_X, EFFECTS_ARC3_X},
//	{EFFECTS_ARC3_X, EFFECTS_ARC2_X},
//	{EFFECTS_ARC2_X, EFFECTS_ARC1_X},
//};

//effects_menu_text_index effects_menu_english_str[EFFECTS_MAX] = {
//		{PAPARAZZI_INDEX, (uint8_t *)"Paparazzi"},
//		{FIREWORKS_INDEX, (uint8_t *)"Fireworks"},
//		{FAULTY_INDEX,    (uint8_t *)"Faulty Bulb"},
//		{LIGHTING_INDEX,  (uint8_t *)"Lightning"},
//		{TV_INDEX, 	      (uint8_t *)"TV"},
//		{PULSING_INDEX,   (uint8_t *)"Pulsing"},
//		{STROBZ_INDEX,    (uint8_t *)"Strobe"},
//		{FRIE_INDEX,      (uint8_t *)"Fire"},
//		{EXPLOSION_INDEX, (uint8_t *)"Explosion"},
//};

//effects_menu_text_index effects_menu_ch_str[EFFECTS_MAX] = {
//		{PAPARAZZI_INDEX, (uint8_t *)"狗仔队"},
//		{FIREWORKS_INDEX, (uint8_t *)"烟花"},
//		{FAULTY_INDEX,    (uint8_t *)"故障灯泡"},
//		{LIGHTING_INDEX,  (uint8_t *)"闪电"},
//		{TV_INDEX, 	      (uint8_t *)"电视"},
//		{PULSING_INDEX,   (uint8_t *)"脉搏"},
//		{STROBZ_INDEX,    (uint8_t *)"闪光灯"},
//		{FRIE_INDEX,      (uint8_t *)"火焰"},
//		{EXPLOSION_INDEX, (uint8_t *)"爆炸"},
//};

//static const lv_img_dsc_t *effects_bmp[EFFECTS_MAX]={
//	[PAPARAZZI_INDEX] = &ImgPaparazzi,
//	[FIREWORKS_INDEX] = &ImgFireWorks,
//	[FAULTY_INDEX   ] = &ImgFaultyBulb,
//	[LIGHTING_INDEX] = &ImgLightning,
//	[TV_INDEX] = &ImgTV,
//	[PULSING_INDEX] = &ImgPulsing,
//	[STROBZ_INDEX] = &ImgStrobe,
//	[FRIE_INDEX] = &ImgFire,
//	[EXPLOSION_INDEX] = &ImgExplosion,
//};

//static lv_group_t* indev_group;
//static uint8_t animt_flag = 0;
//static uint8_t effects_index = 0;

//static void page_light_effect_construct(void);
//static void page_light_effect_destruct(void);
//static void anim_end_cb(struct _lv_anim_t* a);
//static void img_zoom_cb(void * var, int32_t v);
//static void effect_key_callback(lv_event_t* e);

//void menu_img_static_display(lv_obj_t* img_obj, const lv_img_dsc_t *bmp_array, lv_color_t value, uint16_t zoom);

//page_t page_light_effect = PAGE_INITIALIZER("light effect",
//                                PAGE_LIGHT_EFFECT_MENU, 
//                                page_light_effect_construct, 
//                                page_light_effect_destruct, 
//                                NULL,
//                                NULL,                                
//                                PAGE_NONE);
//                                
//void light_effect_menu_create(void)
//{
//    indev_group = lv_group_create();
//    my_indev_set_group(indev_group);
//    lv_group_set_default(indev_group);
//    uint8_t lang_type = 0;
//    
//    lv_obj_set_style_bg_color(lv_scr_act(), lv_color_black(), 0);
//    
//    lv_obj_t * effects_bgimg;
//    lv_color_t img_color;
//    uint16_t img_count;

//    effects_bgimg = lv_img_create(lv_scr_act());     //创建图片对象
//    lv_img_set_src(effects_bgimg, &effects_menu_bg);   //添加图片来源
//    lv_obj_align_to(effects_bgimg, lv_scr_act(), LV_ALIGN_CENTER, 0, -20);                          
//    lv_obj_clear_flag(lv_scr_act(), LV_OBJ_FLAG_SCROLLABLE);
//    
//    effects_menu_mid_str = lv_label_create(lv_scr_act());   //创建文本对象
//    for(uint8_t i = 0; i < 9; i++)
//    {
//        effects_menu_rolling_box[i] = lv_obj_create(lv_scr_act()); 
//        lv_obj_add_style(effects_menu_rolling_box[i], &style_le_btn, 0);
//        lv_obj_set_style_radius(effects_menu_rolling_box[i], LV_RADIUS_CIRCLE, LV_PART_MAIN);   //设置矩形广角为LV_RADIUS_CIRCLE则可以变为园
//        lv_obj_clear_flag(effects_menu_rolling_box[i], LV_OBJ_FLAG_SCROLLABLE);
//        lv_obj_set_size(effects_menu_rolling_box[i], arc_size[i], arc_size[i]);
//        lv_obj_set_pos(effects_menu_rolling_box[i], arc_pos[i][0], arc_pos[i][1]);
//        img_effects[i] = lv_img_create(effects_menu_rolling_box[i]);     //创建图片对象
//        if(i == 0)
//        {
//            img_count = effects_index;
//            img_color = RED_THEME_COLOR;
//        }
//        else
//        {
//            img_count--;
//            if(img_count > EXPLOSION_INDEX)
//                img_count = EXPLOSION_INDEX;
//            img_color = lv_color_make(255, 255, 255);
//        }
//        menu_img_static_display(img_effects[i], effects_bmp[img_count], img_color, img_size[i]);
//            
//    }
//    lv_obj_set_style_text_font(effects_menu_mid_str, Font_ResouceGet(FONT_22_BOLD), 0);
//    lv_obj_set_style_text_color(effects_menu_mid_str, lv_color_white(), 0);
//    data_center_read_config_data(SYS_CONFIG_LANG, &lang_type);
//    if(lang_type == LANGU_EN)
//    {
//            lv_label_set_text(effects_menu_mid_str, (char *)effects_menu_english_str[effects_index].str);
//    }
//    else if(lang_type == LANGU_CHINESE)
//    {
//            lv_label_set_text(effects_menu_mid_str, (char *)effects_menu_ch_str[effects_index].str);
//    }
//    lv_obj_align_to(effects_menu_mid_str, lv_scr_act(), LV_ALIGN_BOTTOM_MID, 0, -10);     //设置bright_arc圆弧基于bg_blo对象中心对齐
//        
//    lv_obj_add_event_cb(effects_bgimg, effect_key_callback, LV_EVENT_ALL, NULL);    //给对象menu_rolling_box添加事件panel_event_cb，事件内容为所有LV_EVENT_ALL
//    lv_group_add_obj(indev_group, effects_bgimg);
//    lv_group_focus_obj(effects_bgimg);
//    lv_group_set_editing(indev_group, true);

//}

//static void page_light_effect_construct(void)
//{
//    light_effect_menu_create();
//}

//static void page_light_effect_destruct(void)
//{
//    lv_group_focus_freeze(indev_group, true);
//    lv_group_remove_all_objs(indev_group);
//    lv_group_del(indev_group);
//    indev_group = NULL;

//    lv_obj_clean(lv_scr_act());
//}

//static void anim_end_cb(struct _lv_anim_t* a)
//{
//    if(animt_flag == true)
//        animt_flag = false;
//}

//static void img_zoom_cb(void * var, int32_t v)
//{
//    lv_img_set_zoom(var, v);
//}

//static void anim_x_cb(void * var, int32_t v)
//{
//    lv_obj_set_x(var, v);
//}
//static void anim_y_cb(void * var, int32_t v)
//{
//    lv_obj_set_y(var, v);
//}
//static void anim_size_cb(void * var, int32_t v)
//{
//    lv_obj_set_size(var, v, v);
//}

///*******************************************************************************************
//函数名称：void menu_cw_rolling(lv_obj_t *obj, uint16_t star_size, uint16_t end_size, int16_t star_x, int16_t end_x, int16_t star_y, int16_t end_y)
//函数作用：光效界面滑动动画显示
//函数参数：obj：滑动的对象
//    star_size：变化的起始大小
//	end_size：变化的结束大小
//	star_x：滑动的起始x坐标
//	end_x：滑动后的x坐标
//	star_y：滑动的起始y坐标
//	end_y：滑动后的y坐标
//	anim_time：动画变化总时间
//返回值  ：NONE
//*******************************************************************************************/
//void menu_rolling(lv_obj_t *obj, uint16_t star_size, uint16_t end_size, int16_t star_x, int16_t end_x, int16_t star_y, int16_t end_y, uint16_t anim_time)
//{
//    lv_anim_t menu_dynamic;
//    
//    lv_anim_init(&menu_dynamic);
//    lv_anim_set_var(&menu_dynamic, obj);   //添加动画效果
//    lv_anim_set_values(&menu_dynamic, star_size, end_size);    //设置大小变化
//    lv_anim_set_time(&menu_dynamic, anim_time);  //设置动画变化总时间
//    lv_anim_set_playback_time(&menu_dynamic, 0);   //参数为0禁止回放效果
//    lv_anim_set_repeat_count(&menu_dynamic, 1);   //动画播放次数1
//    lv_anim_set_exec_cb(&menu_dynamic, anim_size_cb); //添加动画大小回调函数
//    lv_anim_start(&menu_dynamic);   //开始动画大小回调函数
//    lv_anim_set_values(&menu_dynamic, star_x, end_x);   //设置动画x坐标变化大小
//    lv_anim_set_exec_cb(&menu_dynamic, anim_x_cb);  //添加动画x坐标回调函数
//    lv_anim_start(&menu_dynamic);   //开始动画x坐标回调函数
//    lv_anim_set_values(&menu_dynamic, star_y, end_y);
//    lv_anim_set_exec_cb(&menu_dynamic, anim_y_cb);
//    lv_anim_start(&menu_dynamic);
//}

///*******************************************************************************************
//函数名称：void effects_img_rolling_display(lv_obj_t* img_obj, uint16_t img_star_zoom, uint16_t img_end_zoom, lv_color_t img_color)
//函数作用：光效菜单界面显示图片动画效果
//函数参数：
//	img_obj：图片对象
//    up_img：是否需要更换图片
//	img_star_zoom：图片起始缩放大小
//	img_end_zoom：图片结束时的缩放大小
//	img_color：图片颜色
//	img_type：图片ID
//返回值  ：NONE
//*******************************************************************************************/
//static void effects_img_rolling_display(lv_obj_t* img_obj, uint16_t img_star_zoom, uint16_t img_end_zoom, lv_color_t img_color)
//{
//    lv_anim_t img_dynamic;
//    lv_obj_set_style_img_recolor_opa(img_obj, LV_OPA_COVER, LV_STATE_DEFAULT);  //覆盖原来的颜色
//    lv_obj_set_style_img_recolor(img_obj, lv_color_make(255, 255, 255), LV_STATE_DEFAULT);  //重新着色为白色
//    lv_obj_set_style_img_recolor(img_obj, img_color, LV_STATE_DEFAULT);
//    
//    lv_obj_center(img_obj);  //图片显示方式为对齐
//    
//    lv_anim_init(&img_dynamic);
//    lv_anim_set_var(&img_dynamic, img_obj);   //添加动画效果
//    lv_anim_set_values(&img_dynamic, img_star_zoom, img_end_zoom);    //设置大小变化
//    lv_anim_set_time(&img_dynamic, ANIM_TIME);  //设置动画变化总时间
//    lv_anim_set_playback_time(&img_dynamic, 0);   //参数为0禁止回放效果
//    lv_anim_set_repeat_count(&img_dynamic, 1);   //动画播放次数1
//    lv_anim_set_exec_cb(&img_dynamic, img_zoom_cb); //添加动画大小回调函数
//    lv_anim_start(&img_dynamic);
//}
///*******************************************************************************************
//函数名称：void menu_img_static_display(lv_obj_t* img_obj, const lv_img_dsc_t *bmp_array, lv_color_t value, uint16_t zoom)
//函数作用：菜单界面静态显示图片
//函数参数：obj：图片对象的父对象
//    value：图片的新颜色
//	zoom：缩放大小
//	img_type：图片ID
//返回值  ：NONE
//*******************************************************************************************/
//void menu_img_static_display(lv_obj_t* img_obj, const lv_img_dsc_t *bmp_array, lv_color_t value, uint16_t zoom)
//{
//    lv_img_set_src(img_obj, bmp_array);   //添加图片来源
//    lv_obj_set_style_img_recolor_opa(img_obj, LV_OPA_COVER, LV_STATE_DEFAULT);  //覆盖原来的颜色
//    lv_obj_set_style_img_recolor(img_obj, lv_color_make(255, 255, 255), LV_STATE_DEFAULT);  //重新着色为白色
//    lv_obj_set_style_img_recolor(img_obj, value, LV_STATE_DEFAULT);  ////重新着色为value
//    lv_img_set_zoom(img_obj, zoom);   //图片缩放
//    lv_obj_center(img_obj);  //图片显示方式为对齐
//}

///*******************************************************************************************
//函数名称：void effects_menu_encode_cw_rolling_api(void)
//函数作用：菜单顺时针动画效果接口函数
//函数参数：NONE
//返回值  ：NONE
//*******************************************************************************************/
//static void effects_menu_encode_cw_rolling_api(void)
//{
//    lv_anim_t end_idnim;
//    int16_t temp_x;
//    uint8_t lang_type = 0;
//    
//    effects_index++;
//    if(effects_index >= EFFECTS_MAX)
//    {
//        effects_index = PAPARAZZI_INDEX;
//    }
//    
//    for(uint8_t i = 0; i < 9; i++)
//    {
//        temp_x = lv_obj_get_x(effects_menu_rolling_box[i]);
//        switch(temp_x)
//        {
//            case EFFECTS_ARC1_X:
//                menu_rolling(effects_menu_rolling_box[i], arc_size_offset[0][0], arc_size_offset[0][1], arc_cw_x_offset[0][0], arc_cw_x_offset[0][1],
//                            arc_y_offset[0][0], arc_y_offset[0][1], ANIM_TIME);
//                effects_img_rolling_display(img_effects[i], img_size_offset[0][0], img_size_offset[0][1], lv_color_make(255, 255, 255));
//            break;
//            case EFFECTS_ARC2_X:
//                menu_rolling(effects_menu_rolling_box[i], arc_size_offset[1][0], arc_size_offset[1][1], arc_cw_x_offset[1][0], arc_cw_x_offset[1][1],
//                            arc_y_offset[1][0], arc_y_offset[1][1], ANIM_TIME);
//                effects_img_rolling_display(img_effects[i], img_size_offset[1][0], img_size_offset[1][1], lv_color_make(255, 255, 255));
//            break;
//            case EFFECTS_ARC3_X:
//                menu_rolling(effects_menu_rolling_box[i], arc_size_offset[2][0], arc_size_offset[2][1], arc_cw_x_offset[2][0], arc_cw_x_offset[2][1],
//                            arc_y_offset[2][0], arc_y_offset[2][1], ANIM_TIME);
//                effects_img_rolling_display(img_effects[i], img_size_offset[2][0], img_size_offset[2][1], lv_color_make(255, 255, 255));
//            break;
//            case EFFECTS_ARC4_X:
//                menu_rolling(effects_menu_rolling_box[i], arc_size_offset[3][0], arc_size_offset[3][1], arc_cw_x_offset[3][0], arc_cw_x_offset[3][1],
//                            arc_y_offset[3][0], arc_y_offset[3][1], ANIM_TIME);
//                effects_img_rolling_display(img_effects[i], img_size_offset[3][0], img_size_offset[3][1], lv_color_make(255, 255, 255));
//            break;
//            case EFFECTS_ARC5_X:
//                menu_rolling(effects_menu_rolling_box[i], arc_size_offset[4][0], arc_size_offset[4][1], arc_cw_x_offset[4][0], arc_cw_x_offset[4][1],
//                            arc_y_offset[4][0], arc_y_offset[4][1], ANIM_TIME);
//                effects_img_rolling_display(img_effects[i], img_size_offset[4][0], img_size_offset[4][1], lv_color_make(255, 255, 255));
//            break;
//            case EFFECTS_ARC6_X:
//                menu_rolling(effects_menu_rolling_box[i], arc_size_offset[5][0], arc_size_offset[5][1], arc_cw_x_offset[5][0], arc_cw_x_offset[5][1],
//                            arc_y_offset[5][0], arc_y_offset[5][1], ANIM_TIME);
//                effects_img_rolling_display(img_effects[i], img_size_offset[5][0], img_size_offset[5][1], lv_color_make(255, 255, 255));
//            break;
//            case EFFECTS_ARC7_X:
//                menu_rolling(effects_menu_rolling_box[i], arc_size_offset[6][0], arc_size_offset[6][1], arc_cw_x_offset[6][0], arc_cw_x_offset[6][1],
//                            arc_y_offset[6][0], arc_y_offset[6][1], ANIM_TIME);
//                effects_img_rolling_display(img_effects[i], img_size_offset[6][0], img_size_offset[6][1], lv_color_make(255, 255, 255));
//            break;
//            case EFFECTS_ARC8_X:
//                menu_rolling(effects_menu_rolling_box[i], arc_size_offset[7][0], arc_size_offset[7][1], arc_cw_x_offset[7][0], arc_cw_x_offset[7][1],
//                            arc_y_offset[7][0], arc_y_offset[7][1], ANIM_TIME);
//                effects_img_rolling_display(img_effects[i], img_size_offset[7][0], img_size_offset[7][1], lv_color_make(255, 255, 255));
//            break;
//            case EFFECTS_ARC9_X:
//                menu_rolling(effects_menu_rolling_box[i], arc_size_offset[8][0], arc_size_offset[8][1], arc_cw_x_offset[8][0], arc_cw_x_offset[8][1],
//                            arc_y_offset[8][0], arc_y_offset[8][1], ANIM_TIME);
//                effects_img_rolling_display(img_effects[i], img_size_offset[8][0], img_size_offset[8][1], RED_THEME_COLOR);
//            break;
//            default:break;
//        }
//    }
//    data_center_read_config_data(SYS_CONFIG_LANG, &lang_type);
//    if(lang_type == LANGU_EN)
//    {
//        lv_label_set_text(effects_menu_mid_str, (char *)effects_menu_english_str[effects_index].str);
//    }
//    else if(lang_type == LANGU_CHINESE)
//    {
//        lv_label_set_text(effects_menu_mid_str, (char *)effects_menu_ch_str[effects_index].str);
//    }
//    lv_obj_align_to(effects_menu_mid_str, lv_scr_act(), LV_ALIGN_BOTTOM_MID, 0, -10);     //设置bright_arc圆弧基于bg_blo对象中心对齐
//    
//    lv_anim_init(&end_idnim);
//    lv_anim_set_var(&end_idnim, effects_menu_rolling_box[8]);   //添加动画效果的对象
//    lv_anim_set_time(&end_idnim, ANIM_TIME+300);  //设置动画变化总时间
//    lv_anim_set_playback_time(&end_idnim, 0);   //参数为0禁止回放效果
//    lv_anim_set_repeat_count(&end_idnim, 0);   //动画播放次数1
//    lv_anim_set_exec_cb(&end_idnim, NULL); //添加动画大小回调函数
//    lv_anim_set_ready_cb(&end_idnim, anim_end_cb);
//    lv_anim_start(&end_idnim);   //开始动画大小回调函数
//    
//    animt_flag = true;
//}
///*******************************************************************************************
//函数名称：void effects_menu_encode_ccw_rolling_api(void)
//函数作用：菜单逆时针动画效果接口函数
//函数参数：NONE
//返回值  ：NONE
//*******************************************************************************************/
//static void effects_menu_encode_ccw_rolling_api(void)
//{
//    lv_anim_t end_idnim;
//    int16_t temp_x;
//	uint8_t lang_type = 0;
//		
//    effects_index--;
//    if(effects_index >= EFFECTS_MAX)
//    {
//        effects_index = EXPLOSION_INDEX;
//    }
//		
//    for(uint8_t i = 0; i < EFFECTS_MAX; i++)
//    {
//        temp_x = lv_obj_get_x(effects_menu_rolling_box[i]);
//        switch(temp_x)
//        {
//            case EFFECTS_ARC1_X:
//                menu_rolling(effects_menu_rolling_box[i], arc_size_offset[0][0], arc_size_offset[0][1], arc_ccw_x_offset[0][0], arc_ccw_x_offset[0][1],
//                            arc_y_offset[0][0], arc_y_offset[0][1], ANIM_TIME);
//                effects_img_rolling_display(img_effects[i], img_size_offset[0][0], img_size_offset[0][1], lv_color_make(255, 255, 255));
//            break;
//            case EFFECTS_ARC9_X:
//                menu_rolling(effects_menu_rolling_box[i], arc_size_offset[1][0], arc_size_offset[1][1], arc_ccw_x_offset[1][0], arc_ccw_x_offset[1][1],
//                            arc_y_offset[1][0], arc_y_offset[1][1], ANIM_TIME);
//                effects_img_rolling_display(img_effects[i], img_size_offset[1][0], img_size_offset[1][1], lv_color_make(255, 255, 255));
//            break;
//            case EFFECTS_ARC8_X:
//                menu_rolling(effects_menu_rolling_box[i], arc_size_offset[2][0], arc_size_offset[2][1], arc_ccw_x_offset[2][0], arc_ccw_x_offset[2][1],
//                            arc_y_offset[2][0], arc_y_offset[2][1], ANIM_TIME);
//                effects_img_rolling_display(img_effects[i], img_size_offset[2][0], img_size_offset[2][1], lv_color_make(255, 255, 255));
//            break;
//            case EFFECTS_ARC7_X:
//                menu_rolling(effects_menu_rolling_box[i], arc_size_offset[3][0], arc_size_offset[3][1], arc_ccw_x_offset[3][0], arc_ccw_x_offset[3][1],
//                            arc_y_offset[3][0], arc_y_offset[3][1], ANIM_TIME);
//                effects_img_rolling_display(img_effects[i], img_size_offset[3][0], img_size_offset[3][1], lv_color_make(255, 255, 255));
//            break;
//            case EFFECTS_ARC6_X:
//                menu_rolling(effects_menu_rolling_box[i], arc_size_offset[4][0], arc_size_offset[4][1], arc_ccw_x_offset[4][0], arc_ccw_x_offset[4][1],
//                            arc_y_offset[4][0], arc_y_offset[4][1], ANIM_TIME);
//                effects_img_rolling_display(img_effects[i], img_size_offset[4][0], img_size_offset[4][1], lv_color_make(255, 255, 255));
//            break;
//            case EFFECTS_ARC5_X:
//                menu_rolling(effects_menu_rolling_box[i], arc_size_offset[5][0], arc_size_offset[5][1], arc_ccw_x_offset[5][0], arc_ccw_x_offset[5][1],
//                            arc_y_offset[5][0], arc_y_offset[5][1], ANIM_TIME);
//                effects_img_rolling_display(img_effects[i], img_size_offset[5][0], img_size_offset[5][1], lv_color_make(255, 255, 255));
//            break;
//            case EFFECTS_ARC4_X:
//                menu_rolling(effects_menu_rolling_box[i], arc_size_offset[6][0], arc_size_offset[6][1], arc_ccw_x_offset[6][0], arc_ccw_x_offset[6][1],
//                            arc_y_offset[6][0], arc_y_offset[6][1], ANIM_TIME);
//                effects_img_rolling_display(img_effects[i], img_size_offset[6][0], img_size_offset[6][1], lv_color_make(255, 255, 255));
//            break;
//            case EFFECTS_ARC3_X:
//                menu_rolling(effects_menu_rolling_box[i], arc_size_offset[7][0], arc_size_offset[7][1], arc_ccw_x_offset[7][0], arc_ccw_x_offset[7][1],
//                            arc_y_offset[7][0], arc_y_offset[7][1], ANIM_TIME);
//                effects_img_rolling_display(img_effects[i], img_size_offset[7][0], img_size_offset[7][1], lv_color_make(255, 255, 255));
//            break;
//            case EFFECTS_ARC2_X:
//                menu_rolling(effects_menu_rolling_box[i], arc_size_offset[8][0], arc_size_offset[8][1], arc_ccw_x_offset[8][0], arc_ccw_x_offset[8][1],
//                            arc_y_offset[8][0], arc_y_offset[8][1], ANIM_TIME);
//                effects_img_rolling_display(img_effects[i], img_size_offset[8][0], img_size_offset[8][1], RED_THEME_COLOR);
//            break;
//            default:break;
//        }
//    }
//    
//    data_center_read_config_data(SYS_CONFIG_LANG, &lang_type);
//    if(lang_type == LANGU_EN)
//    {
//        lv_label_set_text(effects_menu_mid_str, (char *)effects_menu_english_str[effects_index].str);
//    }
//    else if(lang_type == LANGU_CHINESE)
//    {
//        lv_label_set_text(effects_menu_mid_str, (char *)effects_menu_ch_str[effects_index].str);
//    }
//    
//    lv_obj_align_to(effects_menu_mid_str, lv_scr_act(), LV_ALIGN_BOTTOM_MID, 0, -10);     //设置bright_arc圆弧基于bg_blo对象中心对齐
//    
//    lv_anim_init(&end_idnim);
//    lv_anim_set_var(&end_idnim, effects_menu_rolling_box[8]);   //添加动画效果
//    lv_anim_set_time(&end_idnim, ANIM_TIME+300);  //设置动画变化总时间
//    lv_anim_set_playback_time(&end_idnim, 0);   //参数为0禁止回放效果
//    lv_anim_set_repeat_count(&end_idnim, 0);   //动画播放次数1
//    lv_anim_set_exec_cb(&end_idnim, NULL); //添加动画大小回调函数
//    lv_anim_set_ready_cb(&end_idnim, anim_end_cb);
//    lv_anim_start(&end_idnim);   //开始动画大小回调函数
//    
//    animt_flag = true;
//}

//uint8_t light_effect_get_pid(uint8_t item_id)
//{
//    uint8_t page_id = PAPARAZZI_INDEX;
//    
//    switch(item_id)
//    {
//        case PAPARAZZI_INDEX:   page_id = PAGE_PAPARAZZI; break;
//        case FIREWORKS_INDEX:   page_id = PAGE_FIREWORKS; break;
//        case FAULTY_INDEX:      page_id = PAGE_FAULTYBULB; break;
//        case LIGHTING_INDEX:    page_id = PAGE_LIGHTNING; break;
//        case TV_INDEX:          page_id = PAGE_TV; break;
//        case PULSING_INDEX:     page_id = PAGE_PULSING; break;
//        case STROBZ_INDEX:      page_id = PAGE_STROBE; break;
//        case FRIE_INDEX:        page_id = PAGE_FIRE; break;
//        case EXPLOSION_INDEX:   page_id = PAGE_EXPLOSIOIN; break;
//        default: break;
//    }
//    return page_id;
//}

//uint8_t light_effect_get_index(uint8_t pid)
//{
//    uint8_t item_id = 0;

//    switch(pid)
//    {
//        case PAGE_FIRE:          item_id = FRIE_INDEX; break;
//        case PAGE_PAPARAZZI:     item_id = PAPARAZZI_INDEX; break;
//        case PAGE_FAULTYBULB:    item_id = FAULTY_INDEX; break;
//        case PAGE_TV:            item_id = TV_INDEX; break;
//        case PAGE_PULSING:       item_id = PULSING_INDEX; break;
//        case PAGE_STROBE:        item_id = STROBZ_INDEX; break;
//        case PAGE_LIGHTNING:     item_id = LIGHTING_INDEX; break;
//        case PAGE_FIREWORKS:     item_id = FIREWORKS_INDEX; break;
//        case PAGE_EXPLOSIOIN:    item_id = EXPLOSION_INDEX; break;
//        default: break;
//    }

//    return item_id;    
//}

//const char* lighteffect_get_item_str(uint8_t id)
//{
//    const char* tmp = NULL;
//    switch(id)
//    {
//        case ITEM_ID_FIRE:       tmp = Lang_GetStringByID(STRING_ID_FIRE); break;
//        case ITEM_ID_PAPARAZZI:  tmp = Lang_GetStringByID(STRING_ID_PAPARAZZI); break;
//        case ITEM_ID_FAULT_BULB: tmp = Lang_GetStringByID(STRING_ID_FAULT_BULB); break;
//        case ITEM_ID_TV:         tmp = Lang_GetStringByID(STRING_ID_TV); break;
//        case ITEM_ID_PULSING:    tmp = Lang_GetStringByID(STRING_ID_PULSING); break;
//        case ITEM_ID_PARTY:      tmp = Lang_GetStringByID(STRING_ID_PARTY); break;
//        case ITEM_ID_CLUB_LIGHTS:tmp = Lang_GetStringByID(STRING_ID_CLUB_LIGHTS); break;
//        case ITEM_ID_COLOR_CHASE:tmp = Lang_GetStringByID(STRING_ID_COLOR_CHASE); break;
//        case ITEM_ID_CANDLE:     tmp = Lang_GetStringByID(STRING_ID_CANDLE); break;
//        case ITEM_ID_WELDING:    tmp = Lang_GetStringByID(STRING_ID_WELDING); break;
//        case ITEM_ID_COP_CAR:    tmp = Lang_GetStringByID(STRING_ID_COP_CAR); break;
//        case ITEM_ID_STROBE:     tmp = Lang_GetStringByID(STRING_ID_STROBE); break;
//        case ITEM_ID_LIGHTNING:  tmp = Lang_GetStringByID(STRING_ID_LIGHTNING); break;
//        case ITEM_ID_FIREWORKS:  tmp = Lang_GetStringByID(STRING_ID_FIREWORKS); break;
//        case ITEM_ID_EXPLOSIONS: tmp = Lang_GetStringByID(STRING_ID_EXPLOSIONS); break;
//        default:break;
//    }

//    return tmp;
//}

//static void effect_key_callback(lv_event_t* e)
//{
//    lv_event_code_t event = lv_event_get_code(e);    //获取事件
//    
//    if(LV_EVENT_KEY == event)
//    {
//        uint32_t key = *(uint32_t*)lv_event_get_param(e);     //获取
//        
//        if(animt_flag == true)
//        {
//            return;
//        }
//        
//        shortcut_keys_event_handler(e);
//        backspace_key_event_handler(e);
//        
//        if(key == LV_KEY_RIGHT)
//        {
//            effects_menu_encode_cw_rolling_api();
//            
//            /* 保存光效索引 */
//            ui_light_effect_index_save(effects_index);
//        }
//        else if(key == LV_KEY_LEFT)
//        {
//            effects_menu_encode_ccw_rolling_api();
//            
//            /* 保存光效索引 */
//            ui_light_effect_index_save(effects_index);
//        }
//    }
//    else if(LV_EVENT_PRESSED == event)
//    {
//        if(animt_flag == true)
//        {
//            return;
//        }
//        uint8_t page_id = light_effect_get_pid(effects_index);
//        screen_load_page(page_id, 0, true);
//    }
////    else if(LV_EVENT_LONG_PRESSED == event)
////    {
////        uint32_t key = *(uint32_t*)lv_event_get_param(e);     //获取
////    }
//}

//char* effects_menu_english_str[ITEM_ID_MAX] = {
//        "Explosion",
//		"Paparazzi",
//		"Fireworks",
//		"Faulty Bulb",
//		"Lightning",
//		"TV",
//		"Pulsing",
//		"Strobe",
//		"Fire",
//		
//};

//char* effects_menu_ch_str[ITEM_ID_MAX] = {
//        "爆炸",
//		"狗仔队",
//		"烟花",
//		"故障灯泡",
//		"闪电",
//		"电视",
//		"脉搏",
//		"闪光灯",
//		"火焰",
//};

char* effects_menu_english_str[ITEM_ID_MAX] = {
        "Fire",
	    "Paparazzi",
		"Faulty Bulb",
		"TV",
		"Pulsing",
		"Strobe",
		"Lightning",
		"Fireworks",
		"Explosion",
};

char* effects_menu_ch_str[ITEM_ID_MAX] = {
        "火焰",
	    "狗仔队",
		"故障灯泡",
		"电视",
		"脉搏",
		"闪光灯",
		"闪电",
		"烟花",
		"爆炸",
};

typedef struct _item_dsc {
    item_t *item;    
    lv_point_t start_pos;
    lv_point_t end_pos;    
    lv_coord_t start_width;        
    lv_coord_t end_width;
    uint16_t start_coff;
    uint16_t end_coff;
}item_dsc_t;


/**********************
 *  STATIC PROTOTYPES
 **********************/
static void item_cont_event_cb(lv_event_t *e);
static void item_event_handler(item_id_t id);
static void upd_item_cont_circle(void);
static void item_list_print(void);
static void upd_cw_anim_obj(void);
static void animator_op_obj(void);
static void upd_ccw_anim_obj(void);
static void refresh_le_label(void);

static void page_light_effect_construct(void);
static void page_light_effect_destruct(void);
/**********************
 *  STATIC VARIABLES
 **********************/

static const lv_point_t cw_start_pos[] = 
{
    {117, 120},
    {215, 120},
    {270, 77},
    {225, 28},
    {170, 15},
    {112, 15},
    {55, 28},
    {8, 77},
    {45, 120},
};

static const lv_point_t cw_end_pos[] = 
{
    {45,  120},
    {117, 120},
    {215, 120},
    {270, 77},
    {225, 28},
    {170, 15},
    {112, 15},
    {55,  28},
    {8,  77},
};

static const lv_point_t ccw_start_pos[] = 
{
    {117, 120},
    {215, 120},
    {264, 77},
    {225, 28},
    {170, 15},
    {112, 15},
    {55, 28},
    {8, 77},
    {45, 120},
};

static const lv_point_t ccw_end_pos[] = 
{
    {215, 120},
    {270, 77},
    {225, 28},
    {170, 15},
    {112, 15},
    {55, 28},
    {8, 77},
    {45, 120},
    {117, 120},
};

static const lv_coord_t cw_start_width[] = 
{
    84,
    59,
    42,
    36,
    34,
    34,
    36,
    42,
    59,
};

static const lv_coord_t cw_end_width[] = 
{
    59,
    84,
    59,
    42,
    36,
    34,
    34,
    36,
    42,
};

static const lv_coord_t ccw_start_width[] = 
{
    84,
    59,
    42,
    36,
    34,
    34,
    36,
    42,
    59,
};

static const lv_coord_t ccw_end_width[] = 
{
    59,
    42,
    36,
    34,
    34,
    36,
    42,
    59,
    84,
};
        
static const lv_img_dsc_t *item_imgs[] = 
{
    [ITEM_ID_FIRE] = &ImgFire,
    [ITEM_ID_PAPARAZZI] = &ImgPaparazzi,
    [ITEM_ID_FAULT_BULB] = &ImgFaultyBulb,
    [ITEM_ID_TV] = &ImgTV,
    [ITEM_ID_PULSING] = &ImgPulsing,
    [ITEM_ID_STROBE] = &ImgStrobe,
    [ITEM_ID_LIGHTNING] = &ImgLightning,
    [ITEM_ID_FIREWORKS] = &ImgFireWorks,
    [ITEM_ID_EXPLOSIONS] = &ImgExplosion,
};

static item_dsc_t cur_anim_items[MAX_DISP_OBJS];
static item_t* item_list_head = NULL;
static item_t* focused_item = NULL;
static lv_group_t* indev_group;
static item_t item_array[ITEM_ID_MAX];
static lv_obj_t* item_cont;
static lv_obj_t* le_name_label;
static bool anim_is_running = false;
static bool fx_menu_exited = false;
/*********************
 *  GLOBAL VATIABLES
 *********************/
page_t page_light_effect = PAGE_INITIALIZER("light effect",
                                PAGE_LIGHT_EFFECT_MENU, 
                                page_light_effect_construct, 
                                page_light_effect_destruct, 
                                NULL,
                                NULL,                                
                                PAGE_NONE);
/**********************
 *   GLOBAL FUNCTIONS
 **********************/
void light_effect_menu_create(void)
{
    indev_group = lv_group_create();
    my_indev_set_group(indev_group);
    lv_group_set_default(indev_group);

    lv_obj_set_style_bg_color(lv_scr_act(), lv_color_black(), 0);
    lv_obj_clear_flag(lv_scr_act(), LV_OBJ_FLAG_SCROLLABLE);

    item_cont = lv_obj_create(lv_scr_act());
    lv_obj_remove_style_all(item_cont);
    lv_obj_set_size(item_cont, 320, 220);

    LV_IMG_DECLARE(ImgLightEffectBg);
    lv_obj_align(item_cont, LV_ALIGN_TOP_MID, 0, 0);    
    lv_obj_add_event_cb(item_cont, item_cont_event_cb, LV_EVENT_ALL, NULL);
    
    le_name_label =lv_label_create(lv_scr_act());
    lv_obj_set_style_text_font(le_name_label, Font_ResouceGet(FONT_22_BOLD), 0);
    lv_obj_set_style_text_color(le_name_label, lv_color_white(), 0);

    lv_obj_t* bg_img = lv_img_create(item_cont);
    lv_img_set_src(bg_img, &ImgLightEffectBg);
    lv_obj_set_pos(bg_img, 20, 29);

    lv_obj_t* obj;
    uint8_t index;
    
    /* 获取上一次的索引 */
    index = ui_light_effect_index_get();
	if(g_ptUIStateData->firework_reset == 1)
	{
		g_ptUIStateData->firework_reset = 0;
		index = ITEM_ID_FIREWORKS;
	}
    for (int i = 0; i < ITEM_ID_MAX; i++)
    {
        obj = menu_item_obj_create(item_cont);
        lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);        
        lv_obj_add_style(obj, &style_le_btn, 0);
        lv_obj_set_style_bg_color(obj, lv_color_make(51, 51, 51), LV_STATE_DEFAULT);
        lv_obj_set_style_bg_color(obj, RED_THEME_COLOR, LV_STATE_FOCUSED);
        lv_obj_set_size(obj, cw_start_width[i], cw_start_width[i]);
        
        item_add(&item_list_head, &item_array[i], i, obj, item_imgs[i], lighteffect_get_item_str(i));                
        menu_item_obj_set_param(obj, item_imgs[i], 0, ITEM_MAX_WIDTH);
        menu_item_obj_set_ratio(obj, 0, 256);
        
        if (index == i)
            focused_item = &item_array[i];
    }

    upd_item_cont_circle();
    refresh_le_label();

    lv_group_add_obj(indev_group, item_cont);
    lv_group_focus_obj(item_cont);
    lv_group_set_editing(indev_group, true);

    lv_obj_add_state(focused_item->obj, LV_STATE_FOCUSED);
    lv_event_send(focused_item->obj, LV_EVENT_FOCUSED, NULL);

    item_list_print();
}

const char* lighteffect_get_item_str(uint8_t id)
{
    const char* tmp = NULL;
    uint8_t lang_type = 0;
    data_center_read_config_data(SYS_CONFIG_LANG, &lang_type);
//    switch(id)
//    {
//        case ITEM_ID_FIRE:       
    if(lang_type == LANGU_EN)
    {
            tmp = effects_menu_english_str[id];
    }
    else if(lang_type == LANGU_CHINESE)
    {
            tmp = effects_menu_ch_str[id];
    }
//        break;
//        case ITEM_ID_PAPARAZZI:  tmp = Lang_GetStringByID(STRING_ID_PAPARAZZI); break;
//        case ITEM_ID_FAULT_BULB: tmp = Lang_GetStringByID(STRING_ID_FAULT_BULB); break;
//        case ITEM_ID_TV:         tmp = Lang_GetStringByID(STRING_ID_TV); break;
//        case ITEM_ID_PULSING:    tmp = Lang_GetStringByID(STRING_ID_PULSING); break;
//        case ITEM_ID_STROBE:     tmp = Lang_GetStringByID(STRING_ID_STROBE); break;
//        case ITEM_ID_LIGHTNING:  tmp = Lang_GetStringByID(STRING_ID_LIGHTNING); break;
//        case ITEM_ID_FIREWORKS:  tmp = Lang_GetStringByID(STRING_ID_FIREWORKS); break;
//        case ITEM_ID_EXPLOSIONS: tmp = Lang_GetStringByID(STRING_ID_EXPLOSIONS); break;
//        default:break;
//    }

    return tmp;
}

/**
 * @brief 获取光效的索引
 * 
 * @param pid 
 * @return uint8_t 
 */
uint8_t light_effect_get_index(uint8_t pid)
{
    uint8_t item_id = 0;

    switch(pid)
    {
        case PAGE_FIRE:          item_id = ITEM_ID_FIRE; break;
        case PAGE_PAPARAZZI:     item_id = ITEM_ID_PAPARAZZI; break;
        case PAGE_FAULTYBULB:    item_id = ITEM_ID_FAULT_BULB; break;
        case PAGE_TV:            item_id = ITEM_ID_TV; break;
        case PAGE_PULSING:       item_id = ITEM_ID_PULSING; break;
        case PAGE_STROBE:        item_id = ITEM_ID_STROBE; break;
        case PAGE_LIGHTNING:     item_id = ITEM_ID_LIGHTNING; break;
        case PAGE_FIREWORKS:     item_id = ITEM_ID_FIREWORKS; break;
        case PAGE_EXPLOSIOIN:    item_id = ITEM_ID_EXPLOSIONS; break;
        default: break;
    }

    return item_id;    
}

/**
 * @brief 根据光效ID获取ItemID
 * 
 * @param item_id 
 * @return uint8_t 
 */
uint8_t light_effect_get_pid(uint8_t item_id)
{
    uint8_t page_id = PAGE_FIRE;
    switch(item_id)
    {
        case ITEM_ID_FIRE:          page_id = PAGE_FIRE; break;
        case ITEM_ID_PAPARAZZI:     page_id = PAGE_PAPARAZZI; break;
        case ITEM_ID_FAULT_BULB:    page_id = PAGE_FAULTYBULB; break;
        case ITEM_ID_TV:            page_id = PAGE_TV; break;
        case ITEM_ID_PULSING:       page_id = PAGE_PULSING; break;
        case ITEM_ID_STROBE:        page_id = PAGE_STROBE; break;
        case ITEM_ID_LIGHTNING:     page_id = PAGE_LIGHTNING; break;
        case ITEM_ID_FIREWORKS:     page_id = PAGE_FIREWORKS; break;
        case ITEM_ID_EXPLOSIONS:    page_id = PAGE_EXPLOSIOIN; break;
        default: break;
    }
    return page_id;
}


/**********************
 *  STATIC FUNCTIONS
 **********************/
static void page_light_effect_construct(void)
{
    anim_is_running = false;
    fx_menu_exited = false;
    light_effect_menu_create();
}

static void page_light_effect_destruct(void)
{
    lv_group_focus_freeze(indev_group, true);
    lv_group_remove_all_objs(indev_group);
    lv_group_del(indev_group);
    indev_group = NULL;
    le_name_label = NULL;
    fx_menu_exited = true;

    lv_obj_clean(lv_scr_act());
}
static void anim_pos_x_cb(void* var, int32_t v)
{
    item_dsc_t* pdsc = lv_obj_get_user_data(var);
    lv_obj_t* img = lv_obj_get_child(pdsc->item->obj, 0);
    int32_t cur_distance = LV_ABS(v - pdsc->start_pos.x);
    int32_t distance = LV_ABS(pdsc->start_pos.x - pdsc->end_pos.x);
    int32_t width = LV_ABS(pdsc->end_width - pdsc->start_width);
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

    if (pdsc->start_width > pdsc->end_width)
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

static void item_cont_event_cb(lv_event_t *e)
{
    lv_obj_t* obj = lv_event_get_target(e);
    lv_event_code_t event = lv_event_get_code(e);

    if (!anim_is_running)
    {
        shortcut_keys_event_handler(e);
        backspace_key_event_handler(e);
    }

    if (LV_EVENT_KEY == event)
    {
        uint32_t key = *(uint32_t*)lv_event_get_param(e);

        if (LV_ENCODER_KEY_CW == key && !anim_is_running)
        {                      
            lv_obj_clear_state(focused_item->obj, LV_STATE_FOCUSED);
            lv_event_send(focused_item->obj, LV_EVENT_DEFOCUSED, NULL);

            upd_cw_anim_obj();
            animator_op_obj();              
            
            /* 保存光效索引 */
            ui_light_effect_index_save(focused_item->id);
        }
        else if (LV_ENCODER_KEY_CC == key && !anim_is_running)
        {
            lv_obj_clear_state(focused_item->obj, LV_STATE_FOCUSED);
            lv_event_send(focused_item->obj, LV_EVENT_DEFOCUSED, NULL);
            
            upd_ccw_anim_obj();
            animator_op_obj();            

            /* 保存光效索引 */
            ui_light_effect_index_save(focused_item->id);
        }
    }
    else if (LV_EVENT_RELEASED == event && !anim_is_running)    
    {        
        item_event_handler(focused_item->id);
    }
}

static void item_list_print(void)
{
    item_t* item = item_list_head;
  
    while(item->next != item_list_head)
    {
        UI_PRINTF("id : %d\r\n", item->id);
        item = item->next;
    }  
}

static void upd_cw_anim_obj(void)
{
    item_dsc_t* pdsc = &cur_anim_items[0];
    
    for (int i = 0; i < ITEM_ID_MAX; i++, pdsc++)
    {
        pdsc->item = item_list_get(focused_item, i);
        pdsc->start_pos = cw_start_pos[i];
        pdsc->end_pos = cw_end_pos[i];
        pdsc->start_width = cw_start_width[i];
        pdsc->end_width = cw_end_width[i];
        pdsc->start_coff = lv_map(cw_start_width[i], 0, ITEM_MAX_WIDTH, 0, 256);  
        pdsc->end_coff = lv_map(cw_end_width[i], 0, ITEM_MAX_WIDTH, 0, 256);      
        menu_item_obj_set_param(pdsc->item->obj, NULL, pdsc->start_width, pdsc->end_width);
        menu_item_obj_set_ratio(pdsc->item->obj,  pdsc->start_coff, pdsc->end_coff);              
    }

    focused_item = item_list_get_next(focused_item, 1);
}

static void upd_ccw_anim_obj(void)
{
    item_dsc_t* pdsc = &cur_anim_items[0];
   
    for (int i = 0; i < ITEM_ID_MAX; i++, pdsc++)
    {
        pdsc->item = item_list_get(focused_item, i);
        pdsc->start_pos = ccw_start_pos[i];
        pdsc->end_pos = ccw_end_pos[i];
        pdsc->start_width = ccw_start_width[i];
        pdsc->end_width = ccw_end_width[i];
        pdsc->start_coff = lv_map(ccw_start_width[i], 0, ITEM_MAX_WIDTH, 0, 256);  
        pdsc->end_coff = lv_map(ccw_end_width[i], 0, ITEM_MAX_WIDTH, 0, 256);    
        menu_item_obj_set_param(pdsc->item->obj, NULL, pdsc->start_width, pdsc->end_width);
        menu_item_obj_set_ratio(pdsc->item->obj,  pdsc->start_coff, pdsc->end_coff);
    }

    focused_item = item_list_get_prev(focused_item, 1);
}

static void anim_finish_cb(struct _lv_anim_t* a)
{
    anim_is_running = false;
    if (fx_menu_exited)
        return; 

    refresh_le_label();        
    lv_obj_add_state(focused_item->obj, LV_STATE_FOCUSED);
    lv_event_send(focused_item->obj, LV_EVENT_FOCUSED, NULL);     
}

static void animator_op_obj(void)
{
    item_dsc_t* pdsc = &cur_anim_items[0];
    lv_anim_t a;
    lv_coord_t width;
    
    anim_is_running = true;
    for (int i = 0; i < ITEM_ID_MAX; i++, pdsc++)
    {
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

    lv_anim_init(&a);
    lv_anim_set_var(&a, NULL);
    lv_anim_set_exec_cb(&a, NULL);
    lv_anim_set_ready_cb(&a, anim_finish_cb);
    lv_anim_set_repeat_count(&a, 0);
    lv_anim_set_time(&a, ANIMATION_TIME + 50);
    lv_anim_set_playback_time(&a, 0);
    lv_anim_start(&a);
}

static void upd_item_cont_circle(void)
{
    item_t *tmp;    
    for (int i = 0; i < ITEM_ID_MAX; i++)
    {
        tmp = item_list_get_next(focused_item, i);
        lv_obj_set_pos(tmp->obj, cw_start_pos[i].x, cw_start_pos[i].y);
        lv_obj_set_size(tmp->obj, cw_start_width[i], cw_start_width[i]);
        UI_PRINTF("ID:%d name:%s pos_x:%d pos_y:%d coff:%d\r\n", i,tmp->name, cw_start_pos[i].x, cw_start_pos[i].y, coff);
    }
}

static void refresh_le_label(void)
{   
    if (focused_item == NULL)   return;
    if (le_name_label == NULL)  return;


    const char* name = lighteffect_get_item_str(focused_item->id);
    lv_label_set_text(le_name_label, name);
    lv_obj_align_to(le_name_label, item_cont, LV_ALIGN_OUT_BOTTOM_MID, 0, -10);
}


static void item_event_handler(item_id_t item_id)
{
    uint8_t page_id = light_effect_get_pid(item_id);
    screen_load_page(page_id, 0, true);
}

#endif


