/*********************
 *      INCLUDES
 *********************/
#include "preset_task_item.h"
#include "ui_common.h"
#include "ui_data.h"


/*********************
 *      DEFINES
 *********************/
#define MY_CLASS &preset_task_item_obj_class

/**********************
 *      TYPEDEFS
 **********************/

enum {
    CCT_INT = 0,
    CCT_CCT,
    CCT_GM,
    CCT_MAX,
};

enum {
    HSI_INT = 0,
    HSI_HUE,
    HSI_SAT,
    HSI_MAX,
};

enum {
    RGB_R = 0,
    RGB_G,
    RGB_B,
    RGB_MAX,
};

enum {
    GEL_INT = 0,
    GEL_CCT,
    GEL_SERIAS,
    GEL_MAX,
};

enum {
    XY_X = 0,
    XY_Y,   
    XY_MAX 
};

typedef struct _preset_task_desc_t {
    uint8_t index;                                                                  //当前项目索引
    uint8_t item_nums;                                                              //当前项目的总数
    void  (*init_cb)(struct _preset_task_desc_t* pdesc);                            //初始化
    const char* (*get_value_str)(struct _preset_task_desc_t* pdesc, uint8_t id);    //获取值字符串
    const char* (*get_name_str)(uint8_t id);                                        //获取名字字符串
    void (*draw_cb)(lv_obj_t* obj, const lv_area_t* area);                          //绘制函数
    preset_data_t data;
}preset_task_desc_t;

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void preset_task_item_constructor(const lv_obj_class_t * class_p, lv_obj_t* obj);
static void preset_task_item_destructor(const lv_obj_class_t * class_p, lv_obj_t* obj);
static void preset_task_item_event(const lv_obj_class_t * class_p, lv_event_t * e);
static void preset_task_desc_init(preset_task_desc_t* p_task_desc);
static void draw_main(lv_event_t* e);
static void default_content_draw(lv_obj_t* obj, const lv_area_t* clip_area);
static void light_effect_content_draw(lv_obj_t* obj, const lv_area_t* clip_area);
static void light_mode_content_draw(lv_obj_t* obj, const lv_area_t* clip_area);
static void source_content_draw(lv_obj_t* obj, const lv_area_t* clip_area);
static void xy_content_draw(lv_obj_t* obj, const lv_area_t* clip_area);

/**********************
 *   GLOBAL VARIABLES
 **********************/
#if EFFECTS_USE_PULSING
LV_IMG_DECLARE(ImgPresetPulsing)
#endif
#if EFFECTS_USE_STROBE
LV_IMG_DECLARE(ImgPresetStrobe)
#endif
#if EFFECTS_USE_EXPLOSIONS
LV_IMG_DECLARE(ImgPresetExplosion)
#endif 
#if EFFECTS_USE_FAULT_BULB
LV_IMG_DECLARE(ImgPresetFaultyBulb)
#endif   
#if EFFECTS_USE_WELDING 
LV_IMG_DECLARE(ImgPresetWelding)
#endif  
#if EFFECTS_USE_COP_CAR 
LV_IMG_DECLARE(ImgPresetCopcar)
#endif
#if EFFECTS_USE_CANDLE 
LV_IMG_DECLARE(ImgPresetCandle)
#endif
#if EFFECTS_USE_CLUB_LIGHTS 
LV_IMG_DECLARE(ImgPresetClubLights)
#endif
#if EFFECTS_USE_COLOR_CHASE 
LV_IMG_DECLARE(ImgPresetColorChase)
#endif
#if EFFECTS_USE_TV 
LV_IMG_DECLARE(ImgPresetTV)
#endif     
#if EFFECTS_USE_PAPARAZZI 
LV_IMG_DECLARE(ImgPresetPaparazzi)
#endif
#if EFFECTS_USE_LIGHTNING 
LV_IMG_DECLARE(ImgPresetLightning)
#endif
#if EFFECTS_USE_FIREWORKS 
LV_IMG_DECLARE(ImgPresetFireoworks)
#endif
#if EFFECTS_USE_FIRE 
LV_IMG_DECLARE(ImgPresetFire)
#endif      
#if EFFECTS_USE_PARTY 
LV_IMG_DECLARE(ImgPresetPapartyLights)
#endif
LV_IMG_DECLARE(ImgNextArraw)

const lv_obj_class_t preset_task_item_obj_class = {
    .constructor_cb = preset_task_item_constructor,
    .destructor_cb = preset_task_item_destructor,
    .event_cb = preset_task_item_event,
    .editable = LV_OBJ_CLASS_EDITABLE_FALSE,
    .group_def = LV_OBJ_CLASS_GROUP_DEF_FALSE,
    .instance_size = sizeof(preset_task_item_t),
    .base_class = &lv_obj_class    
};

/*********************
 *  STATIC VATIABLES
 *********************/
static const char* str_cct[] = 
{
    "INT",
    "CCT",
    "G/M"
};

static const char* str_hsi[] = 
{
    "INT",
    "HUE",
    "SAT"
};

static const char* str_rgb[] = 
{
    "R",
    "G",
    "B"
};

static const char* str_xy[] = 
{
    "X",
    "Y",
};

static char vsb[50];
/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/
lv_obj_t* preset_task_item_create(lv_obj_t* parent)
{
    LV_LOG_INFO("begin")
    lv_obj_t * obj = lv_obj_class_create_obj(MY_CLASS, parent);
    lv_obj_class_init_obj(obj);
    return obj;
}

void preset_task_item_set_param(lv_obj_t* obj, void *param)
{
    preset_task_item_t* p_item = (preset_task_item_t*)obj;    
    memcpy(&p_item->p_task_desc->data, param, sizeof(preset_data_t));
    preset_task_desc_init(p_item->p_task_desc);
    lv_obj_invalidate(obj);
}

preset_data_t* preset_task_item_get_param(lv_obj_t* obj)
{
    preset_task_item_t* p_item = (preset_task_item_t*)obj; 
    if (p_item != NULL)
    {
        return &p_item->p_task_desc->data;
    }

    return NULL;
}

uint8_t preset_task_item_get_type(lv_obj_t* obj)
{
    preset_task_item_t* p_item = (preset_task_item_t*)obj; 
    if (p_item != NULL)
    {
        return p_item->p_task_desc->data.mode;
    }

    return 0;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/
static const char* cct_get_name_str(uint8_t id)
{
    return str_cct[id];
}

static const char* hsi_get_name_str(uint8_t id)
{
    return str_hsi[id];
}

static const char* rgb_get_name_str(uint8_t id)
{
    return str_rgb[id];
}

static const char* xy_get_name_str(uint8_t id)
{
    return str_xy[id];
}

static const char* cct_get_task_value_str(preset_task_desc_t* pdsc, uint8_t id)
{
    switch (id)
    {
    case CCT_INT:format_intensity(vsb, sizeof(vsb), pdsc->data.value.cct_arg.lightness); break;
    case CCT_CCT:lv_snprintf(vsb, sizeof(vsb), "%dK", pdsc->data.value.cct_arg.cct.cct); break;
    case CCT_GM: format_gm(vsb, sizeof(vsb), pdsc->data.value.cct_arg.cct.duv); break;
    }

    return vsb;
}

static const char* hsi_get_task_value_str(preset_task_desc_t* pdsc,uint8_t id)
{
    switch (id)
    {
    case HSI_INT:format_intensity(vsb, sizeof(vsb), pdsc->data.value.hsi_arg.lightness); break;
    case HSI_HUE:format_adv_hsi_hue(vsb, sizeof(vsb), pdsc->data.value.hsi_arg.hsi.hue);     break;            
    case HSI_SAT:
        format_adv_hsi_sat(vsb, sizeof(vsb), pdsc->data.value.hsi_arg.hsi.sat);
        break;
    }
    return vsb;
}

static const char* gel_get_task_value_str( preset_task_desc_t* pdsc, uint8_t id)
{
    switch (id)
    {
    case GEL_INT:lv_snprintf(vsb, sizeof(vsb), "INT %d.%d%%", pdsc->data.value.hsi_arg.lightness / 10, pdsc->data.value.hsi_arg.lightness % 10); break;
    case GEL_CCT:
    {
        const char* cct_str = pdsc->data.value.gel_arg.gel.cct == 3200 ? "3200K" : "5600K";
        const char* brand_str = pdsc->data.value.gel_arg.gel.brand == LEE ? "L" : "R";

        lv_snprintf(vsb, sizeof(vsb), "%s / %s", cct_str, brand_str);
    }
    break;
    case GEL_SERIAS:
    {
        uint8_t brand = pdsc->data.value.gel_arg.gel.brand;
        uint8_t series = pdsc->data.value.gel_arg.gel.type[brand];
        const gel_dsc_t* p_cur_gel_dsc = gel_get_serial_dsc(brand, series);
        uint8_t index = pdsc->data.value.gel_arg.gel.color[brand][series];

        lv_snprintf(vsb, sizeof(vsb), "No. %d", p_cur_gel_dsc[index].number); break;
    }
    default:break;
    }
    return vsb;
}

static const char* rgb_get_task_value_str( preset_task_desc_t* pdsc, uint8_t id)
{
    switch (id)
    {
    case RGB_R:format_intensity(vsb, sizeof(vsb), pdsc->data.value.rgb_arg.rgb.r); break;
    case RGB_G:format_intensity(vsb, sizeof(vsb), pdsc->data.value.rgb_arg.rgb.g); break;
    case RGB_B:format_intensity(vsb, sizeof(vsb), pdsc->data.value.rgb_arg.rgb.b); break;
    }
    return vsb;
}

static const char* xy_get_task_value_str(preset_task_desc_t* pdsc, uint8_t id)
{
    switch (id)
    {
    case XY_X:lv_snprintf(vsb, sizeof(vsb), "0.%04d", pdsc->data.value.xy_arg.xy.x); break;
    case XY_Y:lv_snprintf(vsb, sizeof(vsb), "0.%04d", pdsc->data.value.xy_arg.xy.y); break;
    }
    return vsb;
}

static void preset_task_item_constructor(const lv_obj_class_t * class_p, lv_obj_t* obj)
{
    LV_UNUSED(class_p);
    LV_TRACE_OBJ_CREATE("begin");

    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);

    preset_task_item_t* p_item = (preset_task_item_t*)obj;
    p_item->p_task_desc = lv_mem_alloc(sizeof(preset_task_desc_t));
    memset(p_item->p_task_desc, 0, sizeof(preset_task_desc_t));
    p_item->p_task_desc->get_name_str = NULL;
    p_item->p_task_desc->get_value_str = NULL;
    p_item->p_task_desc->init_cb = NULL;
    p_item->p_task_desc->item_nums = 0;
    p_item->p_task_desc->index = lv_obj_get_child_id(obj);
    p_item->p_task_desc->data.mode = LIGHT_MODE_FACTORY_PWM;
    preset_task_desc_init(p_item->p_task_desc);
    LV_TRACE_OBJ_CREATE("finished");
}

static void preset_task_item_destructor(const lv_obj_class_t * class_p, lv_obj_t* obj)
{
    LV_UNUSED(class_p);
    preset_task_item_t* p_item = (preset_task_item_t*)obj;
    if (p_item->p_task_desc)        lv_mem_free(p_item->p_task_desc);
}

static void preset_task_desc_init(preset_task_desc_t* p_task_desc)
{
    switch (p_task_desc->data.mode)
    {
        case LIGHT_MODE_CCT:
            p_task_desc->draw_cb = light_mode_content_draw;
            p_task_desc->get_name_str = cct_get_name_str;
            p_task_desc->get_value_str = cct_get_task_value_str;
            break;
        case LIGHT_MODE_HSI:
            p_task_desc->draw_cb = light_mode_content_draw;
            p_task_desc->get_name_str = hsi_get_name_str;
            p_task_desc->get_value_str = hsi_get_task_value_str;
            break;
        case LIGHT_MODE_RGB:
            p_task_desc->draw_cb = light_mode_content_draw;
            p_task_desc->get_name_str =  rgb_get_name_str;
            p_task_desc->get_value_str = rgb_get_task_value_str;
            break;
        case LIGHT_MODE_GEL:
            p_task_desc->draw_cb = light_mode_content_draw;
            p_task_desc->get_name_str = NULL;
            p_task_desc->get_value_str = gel_get_task_value_str;
            break;
        case LIGHT_MODE_XY:
            p_task_desc->draw_cb = xy_content_draw;
            p_task_desc->get_name_str = xy_get_name_str;
            p_task_desc->get_value_str = xy_get_task_value_str;
            break;
        case LIGHT_MODE_SOURCE:
            p_task_desc->draw_cb = source_content_draw;
            p_task_desc->get_name_str = source_get_name;
            p_task_desc->get_value_str = NULL;
            break;
        case LIGHT_MODE_FX_PULSING:
        case LIGHT_MODE_FX_PULSING_II:
        case LIGHT_MODE_FX_STROBE:
        case LIGHT_MODE_FX_STROBE_II:
        case LIGHT_MODE_FX_EXPLOSION:
        case LIGHT_MODE_FX_EXPLOSION_II:
        case LIGHT_MODE_FX_FAULT_BULB:
        case LIGHT_MODE_FX_FAULT_BULB_II:
        case LIGHT_MODE_FX_WELDING_II:
        case LIGHT_MODE_FX_COP_CAR_II:
        case LIGHT_MODE_FX_CANDLE:
        case LIGHT_MODE_FX_CLUBLIGHTS:
        case LIGHT_MODE_FX_COLOR_CHASE:
        case LIGHT_MODE_FX_TV:
        case LIGHT_MODE_FX_TV_II:
        case LIGHT_MODE_FX_PAPARAZZI:
        case LIGHT_MODE_FX_LIGHTNING:
        case LIGHT_MODE_FX_LIGHTNING_II:
        case LIGHT_MODE_FX_FIREWORKS:
        case LIGHT_MODE_FX_FIREWORKS_II:
        case LIGHT_MODE_FX_FIRE:
        case LIGHT_MODE_FX_FIRE_II:
        case LIGHT_MODE_FX_PARTY_LIGHTS:
            p_task_desc->draw_cb =light_effect_content_draw ;
            p_task_desc->get_name_str = NULL;
            p_task_desc->get_value_str = NULL;
            break;
        case LIGHT_MODE_FACTORY_PWM:
        default:        
            p_task_desc->draw_cb = default_content_draw;
            p_task_desc->get_name_str = NULL;
            p_task_desc->get_value_str = NULL;   
            break;        
        
    }
}

static void preset_task_item_event(const lv_obj_class_t * class_p, lv_event_t * e)
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
}

static void get_type_str(uint8_t type, str_dsc_t *pdsc)
{    
    pdsc->font = Font_ResouceGet(FONT_10_MEDIUM);
    pdsc->img  = NULL;
    pdsc->str = " ";
    switch(type)
    {
        case LIGHT_MODE_FACTORY_PWM:       pdsc->str = LV_SYMBOL_PLUS;           break;        
        case LIGHT_MODE_CCT:               pdsc->str = "CCT";                    break;
        case LIGHT_MODE_HSI:               pdsc->str = "Adv.\nHSI";              break;
        case LIGHT_MODE_RGB:               pdsc->str = "RGB";                    break;
        case LIGHT_MODE_GEL:               pdsc->str = "GEL";                    break;
        case LIGHT_MODE_XY:                pdsc->str = "X Y";                    break;
        case LIGHT_MODE_SOURCE:            pdsc->str = "SRC";                    break;  
        #if EFFECTS_USE_PULSING
        case LIGHT_MODE_FX_PULSING:         pdsc->img = &ImgPresetPulsing; break;
        case LIGHT_MODE_FX_PULSING_II:      pdsc->img = &ImgPresetPulsing; break;
        #endif
        #if EFFECTS_USE_STROBE
        case LIGHT_MODE_FX_STROBE:          pdsc->img = &ImgPresetStrobe; break;
        case LIGHT_MODE_FX_STROBE_II:       pdsc->img = &ImgPresetStrobe; break;
        #endif
        #if EFFECTS_USE_EXPLOSIONS
        case LIGHT_MODE_FX_EXPLOSION:       pdsc->img = &ImgPresetExplosion; break;
        case LIGHT_MODE_FX_EXPLOSION_II:    pdsc->img = &ImgPresetExplosion; break;
        #endif 
        #if EFFECTS_USE_FAULT_BULB
        case LIGHT_MODE_FX_FAULT_BULB:      pdsc->img = &ImgPresetFaultyBulb; break;
        case LIGHT_MODE_FX_FAULT_BULB_II:   pdsc->img = &ImgPresetFaultyBulb; break;
        #endif   
        #if EFFECTS_USE_WELDING 
        case LIGHT_MODE_FX_WELDING_II:      pdsc->img = &ImgPresetWelding;  break;
        #endif  
        #if EFFECTS_USE_COP_CAR 
        case LIGHT_MODE_FX_COP_CAR_II:      pdsc->img = &ImgPresetCopcar; break;
        #endif
        #if EFFECTS_USE_CANDLE 
        case LIGHT_MODE_FX_CANDLE:          pdsc->img = &ImgPresetCandle; break;
        #endif
        #if EFFECTS_USE_CLUB_LIGHTS 
        case LIGHT_MODE_FX_CLUBLIGHTS:      pdsc->img = &ImgPresetClubLights; break;
        #endif
        #if EFFECTS_USE_COLOR_CHASE 
        case LIGHT_MODE_FX_COLOR_CHASE:     pdsc->img = &ImgPresetColorChase; break;
        #endif
        #if EFFECTS_USE_TV 
        case LIGHT_MODE_FX_TV:              pdsc->img = &ImgPresetTV;  break;
        case LIGHT_MODE_FX_TV_II:           pdsc->img = &ImgPresetTV;  break;
        #endif     
        #if EFFECTS_USE_PAPARAZZI 
        case LIGHT_MODE_FX_PAPARAZZI:       pdsc->img = &ImgPresetPaparazzi; break;
        #endif
        #if EFFECTS_USE_LIGHTNING 
        case LIGHT_MODE_FX_LIGHTNING:       pdsc->img = &ImgPresetLightning;  break;
        case LIGHT_MODE_FX_LIGHTNING_II:    pdsc->img = &ImgPresetLightning;  break;
        #endif
        #if EFFECTS_USE_FIREWORKS 
        case LIGHT_MODE_FX_FIREWORKS:       pdsc->img = &ImgPresetFireoworks; break;
        case LIGHT_MODE_FX_FIREWORKS_II:    pdsc->img = &ImgPresetFireoworks; break;
        #endif
        #if EFFECTS_USE_FIRE 
        case LIGHT_MODE_FX_FIRE:            pdsc->img = &ImgPresetFire; break;
        case LIGHT_MODE_FX_FIRE_II:         pdsc->img = &ImgPresetFire; break;
        #endif      
        #if EFFECTS_USE_PARTY 
        case LIGHT_MODE_FX_PARTY_LIGHTS:    pdsc->img = &ImgPresetPapartyLights; break;
        #endif
        default:break;        
    }
}

static void default_content_draw(lv_obj_t* obj, const lv_area_t* clip_area)
{
    lv_area_t draw_area;
    char buff[20];   
    lv_draw_label_dsc_t label_dsc;
    lv_draw_label_dsc_init(&label_dsc);  
    lv_area_set(&draw_area, obj->coords.x1 + 45, obj->coords.y1, 
                            obj->coords.x2 - 8, obj->coords.y2);    

    lv_snprintf(buff, sizeof(buff), Lang_GetStringByID(STRING_ID_SAVE));
    
    lv_point_t size;
    label_dsc.font = Font_ResouceGet(FONT_10_MEDIUM);
    label_dsc.color = lv_obj_has_state(obj, LV_STATE_FOCUSED) ? lv_color_make(0, 0, 0) : lv_color_make(255, 255, 255);
    lv_txt_get_size(&size, buff, label_dsc.font, label_dsc.letter_space, label_dsc.line_space, LV_COORD_MAX, label_dsc.flag);
    
    draw_area.x1 = draw_area.x1 + (lv_area_get_width(&draw_area) - size.x) / 2;
    draw_area.y1 = draw_area.y1+  (lv_area_get_height(&draw_area) - size.y) / 2;  
    draw_area.x2 = draw_area.x1 + size.x;
    draw_area.y2 = draw_area.y2 + size.y;
    lv_draw_label(&draw_area, clip_area, &label_dsc, buff, NULL);    
}

static void light_effect_content_draw(lv_obj_t* obj, const lv_area_t* clip_area)
{
    lv_area_t draw_area;
    const char* str = " ";
    lv_draw_label_dsc_t label_dsc;
    preset_task_item_t* p_item_ext = (preset_task_item_t*)obj;
    lv_draw_label_dsc_init(&label_dsc);  
    lv_area_set(&draw_area, obj->coords.x1 + 45, obj->coords.y1, 
                            obj->coords.x2 - 8, obj->coords.y2);                                
    
    switch (p_item_ext->p_task_desc->data.mode)
    {
        #if EFFECTS_USE_PULSING
        case LIGHT_MODE_FX_PULSING:         str = lighteffect_get_item_str(ITEM_ID_PULSING); break;
        case LIGHT_MODE_FX_PULSING_II:      str = lighteffect_get_item_str(ITEM_ID_PULSING); break;
        #endif
        #if EFFECTS_USE_STROBE
        case LIGHT_MODE_FX_STROBE:          str = lighteffect_get_item_str(ITEM_ID_STROBE); break;
        case LIGHT_MODE_FX_STROBE_II:       str = lighteffect_get_item_str(ITEM_ID_STROBE); break;
        #endif
        #if EFFECTS_USE_EXPLOSIONS
        case LIGHT_MODE_FX_EXPLOSION:       str = lighteffect_get_item_str(ITEM_ID_EXPLOSIONS); break;
        case LIGHT_MODE_FX_EXPLOSION_II:    str = lighteffect_get_item_str(ITEM_ID_EXPLOSIONS); break;
        #endif
        #if EFFECTS_USE_FAULT_BULB
        case LIGHT_MODE_FX_FAULT_BULB:      str = lighteffect_get_item_str(ITEM_ID_FAULT_BULB); break;
        case LIGHT_MODE_FX_FAULT_BULB_II:   str = lighteffect_get_item_str(ITEM_ID_FAULT_BULB); break;
        #endif
        #if EFFECTS_USE_WELDING 
        case LIGHT_MODE_FX_WELDING_II:      str = lighteffect_get_item_str(ITEM_ID_WELDING); break;
        #endif
        #if EFFECTS_USE_COP_CAR 
        case LIGHT_MODE_FX_COP_CAR_II:      str = lighteffect_get_item_str(ITEM_ID_COP_CAR); break;
        #endif
        #if EFFECTS_USE_CANDLE 
        case LIGHT_MODE_FX_CANDLE:          str = lighteffect_get_item_str(ITEM_ID_CANDLE); break;
        #endif
        #if EFFECTS_USE_CLUB_LIGHTS 
        case LIGHT_MODE_FX_CLUBLIGHTS:      str = lighteffect_get_item_str(ITEM_ID_CLUB_LIGHTS); break;
        #endif
        #if EFFECTS_USE_COLOR_CHASE 
        case LIGHT_MODE_FX_COLOR_CHASE:     str = lighteffect_get_item_str(ITEM_ID_COLOR_CHASE); break;
        #endif
        #if EFFECTS_USE_TV 
        case LIGHT_MODE_FX_TV:              str = lighteffect_get_item_str(ITEM_ID_TV); break;
        case LIGHT_MODE_FX_TV_II:           str = lighteffect_get_item_str(ITEM_ID_TV); break;
        #endif
        #if EFFECTS_USE_PAPARAZZI 
        case LIGHT_MODE_FX_PAPARAZZI:       str = lighteffect_get_item_str(ITEM_ID_PAPARAZZI); break;
        #endif
        #if EFFECTS_USE_LIGHTNING 
        case LIGHT_MODE_FX_LIGHTNING:       str = lighteffect_get_item_str(ITEM_ID_LIGHTNING); break;
        case LIGHT_MODE_FX_LIGHTNING_II:    str = lighteffect_get_item_str(ITEM_ID_LIGHTNING); break;
        #endif
        #if EFFECTS_USE_FIREWORKS 
        case LIGHT_MODE_FX_FIREWORKS:       str = lighteffect_get_item_str(ITEM_ID_FIREWORKS); break;
        case LIGHT_MODE_FX_FIREWORKS_II:    str = lighteffect_get_item_str(ITEM_ID_FIREWORKS); break;
        #endif
        #if EFFECTS_USE_FIRE 
        case LIGHT_MODE_FX_FIRE:            str = lighteffect_get_item_str(ITEM_ID_FIRE); break;
        case LIGHT_MODE_FX_FIRE_II:         str = lighteffect_get_item_str(ITEM_ID_FIRE); break;
        #endif
        #if EFFECTS_USE_PARTY 
        case LIGHT_MODE_FX_PARTY_LIGHTS:    str = lighteffect_get_item_str(ITEM_ID_PARTY); break;
        #endif
        default: break;
    }

    lv_point_t size;
    label_dsc.font = Font_ResouceGet(FONT_10_MEDIUM);
    label_dsc.color = lv_obj_has_state(obj, LV_STATE_FOCUSED) ? lv_color_make(0, 0, 0) : lv_color_make(255, 255, 255);
    label_dsc.flag = LV_TEXT_FLAG_EXPAND;
    label_dsc.align = LV_TEXT_ALIGN_CENTER;
    lv_txt_get_size(&size, str, label_dsc.font, label_dsc.letter_space, label_dsc.line_space, LV_COORD_MAX, label_dsc.flag);    
    draw_area.x1 = draw_area.x1 + (lv_area_get_width(&draw_area) - size.x) / 2;
    draw_area.y1 = draw_area.y1+  (lv_area_get_height(&draw_area) - size.y) / 2;  
    draw_area.x2 = draw_area.x1 + size.x;
    draw_area.y2 = draw_area.y2 + size.y;
    lv_draw_label(&draw_area, clip_area, &label_dsc, str, NULL);    
}

static void light_mode_content_draw(lv_obj_t* obj, const lv_area_t* clip_area)
{
    lv_area_t draw_area;
    const char* name = NULL,*value = NULL;
    lv_draw_label_dsc_t label_dsc;
    lv_coord_t gap = 2;
    preset_task_item_t* p_item_ext = (preset_task_item_t*)obj;
    lv_draw_label_dsc_init(&label_dsc);  
    label_dsc.font = Font_ResouceGet(FONT_10_MEDIUM);
    for (int8_t i=0; i<3; i++)
    {
        if (p_item_ext->p_task_desc->get_name_str)
        {
            name = p_item_ext->p_task_desc->get_name_str(i);    
        }

        if (p_item_ext->p_task_desc->get_value_str)
        {
            value = p_item_ext->p_task_desc->get_value_str(p_item_ext->p_task_desc, i);
        }

        draw_area.x1 = obj->coords.x1 + 50;
        draw_area.x2 = obj->coords.x2 - 8;
        draw_area.y1 = obj->coords.y1 + 4 + (label_dsc.font->line_height + gap) * i;
        draw_area.y2 = draw_area.y1 + label_dsc.font->line_height;

        if (name != NULL)
        {   
            label_dsc.color = lv_obj_has_state(obj, LV_STATE_FOCUSED) ? lv_color_make(128, 128, 128) : lv_color_make(128, 128, 128);
            label_dsc.flag = LV_TEXT_FLAG_NONE;
            label_dsc.align = LV_TEXT_ALIGN_LEFT;                    
            lv_draw_label(&draw_area, clip_area, &label_dsc, name, NULL);                
        }

        if (value != NULL)
        {
            label_dsc.color = lv_obj_has_state(obj, LV_STATE_FOCUSED) ? lv_color_make(0, 0, 0) : lv_color_make(255, 255, 255);
            label_dsc.flag = LV_TEXT_FLAG_EXPAND;
            label_dsc.align = LV_TEXT_ALIGN_LEFT;
            if (name == NULL)
            {
                draw_area.x1 = obj->coords.x1 + 47 + 6;
            }
            else  
            {
                draw_area.x1 = obj->coords.x1 + 47 + 30;
            }
            lv_draw_label(&draw_area, clip_area, &label_dsc, value, NULL);
        }
    }
}

static void xy_content_draw(lv_obj_t* obj, const lv_area_t* clip_area)
{
    lv_area_t draw_area;
    const char* name = NULL,*value = NULL;
    lv_draw_label_dsc_t label_dsc;
    lv_coord_t gap = 2;
    preset_task_item_t* p_item_ext = (preset_task_item_t*)obj;
    lv_draw_label_dsc_init(&label_dsc);  
    label_dsc.font = Font_ResouceGet(FONT_10_MEDIUM);
    for (int8_t i=0; i<2; i++)
    {
        //name = "X";//p_item_ext->p_task_desc->get_name_str(i);
        //value = "0.9999";//p_item_ext->p_task_desc->get_value_str(p_item_ext->p_task_desc, i);
        if (p_item_ext->p_task_desc->get_name_str)
        {
            name = p_item_ext->p_task_desc->get_name_str(i);    
        }

        if (p_item_ext->p_task_desc->get_value_str)
        {
            value = p_item_ext->p_task_desc->get_value_str(p_item_ext->p_task_desc, i);
        }

        draw_area.x1 = obj->coords.x1 + 50;
        draw_area.x2 = obj->coords.x2 - 8;
        draw_area.y1 = obj->coords.y1 + 11 + (label_dsc.font->line_height + gap) * i;
        draw_area.y2 = draw_area.y1 + label_dsc.font->line_height;

        if (name != NULL)
        {   
            label_dsc.color = lv_obj_has_state(obj, LV_STATE_FOCUSED) ? lv_color_make(128, 128, 128) : lv_color_make(128, 128, 128);
            label_dsc.flag = LV_TEXT_FLAG_EXPAND;
            label_dsc.align = LV_TEXT_ALIGN_LEFT;                    
            lv_draw_label(&draw_area, clip_area, &label_dsc, name, NULL);    
        }

        if (value != NULL)
        {
            label_dsc.color = lv_obj_has_state(obj, LV_STATE_FOCUSED) ? lv_color_make(0, 0, 0) : lv_color_make(255, 255, 255);
            label_dsc.flag = LV_TEXT_FLAG_EXPAND;
            label_dsc.align = LV_TEXT_ALIGN_LEFT;
            draw_area.x1 = obj->coords.x1 + 47 + 30;
            
            lv_draw_label(&draw_area, clip_area, &label_dsc, value, NULL);
        }
    }
}

static void source_content_draw(lv_obj_t* obj, const lv_area_t* clip_area)
{
    lv_area_t draw_area;
    char buff[30];
    const char* name = "Unknown";
    preset_task_item_t* p_item_ext = (preset_task_item_t*)obj;
    lv_draw_label_dsc_t label_dsc;

    lv_draw_label_dsc_init(&label_dsc);  
    lv_area_set(&draw_area, obj->coords.x1 + 45, obj->coords.y1, 
                            obj->coords.x2 - 8, obj->coords.y2);                            

    const src_table_t *src = source_get_table(p_item_ext->p_task_desc->data.value.source_arg.source.type);
    lv_draw_label_hint_t hint;
    hint.line_start = -1;
    hint.coord_y    = 0;
    hint.y          = 0;
    if (src != NULL)
        name = src->name;

    strcpy(buff, name);
    
    lv_point_t size;
    label_dsc.font = Font_ResouceGet(FONT_10_MEDIUM);
    label_dsc.color = lv_obj_has_state(obj, LV_STATE_FOCUSED) ? lv_color_make(0, 0, 0) : lv_color_make(255, 255, 255);
    label_dsc.flag = LV_TEXT_FLAG_FIT;
    label_dsc.align = LV_TEXT_ALIGN_CENTER;
    lv_txt_get_size(&size, buff, label_dsc.font, label_dsc.letter_space, label_dsc.line_space, LV_COORD_MAX, label_dsc.flag);
    
    draw_area.x1 = draw_area.x1;
    draw_area.y1 = draw_area.y1 + (lv_area_get_height(&draw_area) - size.y) / 2;  ;  
    lv_draw_label(&draw_area, clip_area, &label_dsc, buff, &hint);
}

static void draw_main(lv_event_t* e)
{
    lv_area_t* clip_area = lv_event_get_param(e);
    lv_obj_t * obj = lv_event_get_target(e);
    preset_task_item_t* p_item_ext = (preset_task_item_t*)obj;
    lv_color_t font_color;
    lv_color_t bg_color;
    lv_area_t draw_area;
    const lv_font_t* font = lv_obj_get_style_text_font(obj, LV_STATE_DEFAULT);
    lv_draw_rect_dsc_t rect_dsc;
    lv_draw_label_dsc_t label_dsc;
    lv_draw_img_dsc_t img_dsc;
    char buffer[10];
    lv_point_t point;

    lv_draw_img_dsc_init(&img_dsc);
    lv_area_copy(&draw_area, &obj->coords);
    lv_draw_rect_dsc_init(&rect_dsc);
    lv_obj_init_draw_rect_dsc(obj, LV_PART_MAIN, &rect_dsc);   

    //draw background.
    if (lv_obj_has_state(obj, LV_STATE_FOCUSED))
    {
        font_color = lv_obj_get_style_text_color(obj, LV_STATE_FOCUSED);
        bg_color = lv_obj_get_style_bg_color(obj, LV_STATE_FOCUSED);
    }
    else  
    {
        font_color = lv_color_white();
        bg_color = lv_obj_get_style_bg_color(obj, LV_STATE_DEFAULT);
    }

    rect_dsc.bg_color = bg_color;
    rect_dsc.bg_opa = LV_OPA_100;
    lv_draw_rect(&draw_area, clip_area, &rect_dsc);

    //draw label
    lv_draw_label_dsc_init(&label_dsc);    
    label_dsc.font = font;
    label_dsc.color = font_color;
    label_dsc.align = LV_TEXT_ALIGN_CENTER;
    //draw_area.x1 += 2;
    draw_area.x2 = draw_area.x1 + lv_txt_get_width("77", strlen("77"), font, 0, LV_TEXT_FLAG_NONE);
    lv_txt_get_size(&point, LV_SYMBOL_PLUS, label_dsc.font, label_dsc.letter_space, label_dsc.line_space, LV_COORD_MAX, label_dsc.flag);
    draw_area.y1 = draw_area.y1+ lv_area_get_height(&draw_area) / 2 - point.y / 2;       
    lv_snprintf(buffer, sizeof(buffer), "%d", lv_obj_get_child_id(obj) + 1);
    lv_draw_label(&draw_area, clip_area, &label_dsc, buffer, NULL);

    //draw icon background.
    bg_color = lv_obj_has_state(obj, LV_STATE_FOCUSED) ? lv_color_make(196, 56, 43) : lv_color_make(128, 128, 128);
    font_color = lv_obj_has_state(obj, LV_STATE_FOCUSED) ? lv_color_make(255, 255, 255) : lv_color_make(0, 0, 0);
    rect_dsc.bg_color = bg_color;
    rect_dsc.radius = 4;
    lv_area_copy(&draw_area, &obj->coords);
    draw_area.x1 += 16;
    draw_area.y1 = draw_area.y1 + lv_area_get_height(&draw_area) / 2 - 28 / 2;
    draw_area.x2 = draw_area.x1 + 28;
    draw_area.y2 = draw_area.y1 + 28;
    lv_draw_rect(&draw_area, clip_area, &rect_dsc);

    str_dsc_t icon_dsc;
    get_type_str(p_item_ext->p_task_desc->data.mode, &icon_dsc);
    
    if (icon_dsc.img == NULL)
    {
        //draw icon name.    
        lv_draw_label_dsc_init(&label_dsc);    
        label_dsc.color = font_color;
        label_dsc.align = LV_TEXT_ALIGN_CENTER;
        label_dsc.font = icon_dsc.font;
        
        lv_txt_get_size(&point, icon_dsc.str, label_dsc.font, label_dsc.letter_space, label_dsc.line_space, LV_COORD_MAX, label_dsc.flag);
        draw_area.y1 = draw_area.y1+ lv_area_get_height(&draw_area) / 2 - point.y / 2;        
        lv_draw_label(&draw_area, clip_area, &label_dsc, icon_dsc.str, NULL);
    }
    else  
    {
        img_dsc.recolor = lv_obj_has_state(obj, LV_STATE_FOCUSED) ? lv_color_make(255, 255, 255) : lv_color_make(0, 0, 0);
        img_dsc.recolor_opa = LV_OPA_100;
        draw_area.x1 = draw_area.x1 + (lv_area_get_width(&draw_area) - icon_dsc.img->header.w) / 2;
        draw_area.y1 = draw_area.y1 + (lv_area_get_height(&draw_area) - icon_dsc.img->header.h) / 2;
        lv_area_set_width(&draw_area, icon_dsc.img->header.w);
        lv_area_set_height(&draw_area, icon_dsc.img->header.h);
        lv_draw_img(&draw_area, clip_area, icon_dsc.img, &img_dsc);    
    }

    //draw the content.
    if (p_item_ext->p_task_desc->draw_cb)
        p_item_ext->p_task_desc->draw_cb(obj, clip_area);

    //draw the next arrow
    lv_area_copy(&draw_area, &obj->coords); 
    img_dsc.recolor = lv_obj_has_state(obj, LV_STATE_FOCUSED) ? lv_color_make(0, 0, 0) : lv_color_make(255, 255, 255);
    img_dsc.recolor_opa = LV_OPA_100;    
    draw_area.x1 = draw_area.x2 - ImgNextArraw.header.w - 2;
    draw_area.y1 = draw_area.y1 + (lv_area_get_height(&draw_area) - ImgNextArraw.header.h) / 2;     
    draw_area.x2 = draw_area.x1 + ImgNextArraw.header.w - 1;
    draw_area.y2 = draw_area.y1 + ImgNextArraw.header.h - 1;       
    lv_draw_img(&draw_area, clip_area, &ImgNextArraw, &img_dsc);

    //draw divider line.
    lv_area_copy(&draw_area, &obj->coords);    
    draw_area.y1 = draw_area.y2;
    rect_dsc.bg_color = lv_color_black();
    lv_draw_rect(&draw_area, clip_area, &rect_dsc);    

    draw_area.x1 = draw_area.x1 +  lv_area_get_width(&draw_area) / 3;
    draw_area.y1 = draw_area.y2;
    rect_dsc.bg_color = lv_color_make(200, 200, 200);
    lv_draw_rect(&draw_area, clip_area, &rect_dsc);
}
