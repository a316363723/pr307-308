#ifndef PAGE_H
#define PAGE_H
#include "ui_config.h"
#ifdef  __cplusplus
extern "C" {
#endif //  __cplusplus

/*********************
 *      INCLUDES
 *********************/
#include <stdint.h>
#include <stdbool.h>
/*********************
 *      DEFINES
 *********************/


/**********************
 *      TYPEDEFS
 **********************/
enum {
    PAGE_DESK,
    PAGE_MENU,
    PAGE_LIGHT_MODE_HSI,
    PAGE_LIGHT_MODE_RGB,
    PAGE_LIGHT_MODE_GEL,
    PAGE_LIGHT_MODE_XY,
    PAGE_LIGHT_MODE_SOURCE,
    PAGE_LIGHT_MODE_CCT,
    PAGE_LIGHT_EFFECT_MENU,
    PAGE_HSI_SETTING,
    PAGE_RGB_SETTING,
    PAGE_GEL_SETTING,
    PAGE_SOURCE_SETTING,
    PAGE_CCT_SETTING,
    PAGE_XY_SETTING,
    PAGE_PULSING,
    PAGE_STROBE,
    PAGE_EXPLOSIOIN,
    PAGE_FAULTYBULB,
    PAGE_WELDING,
    PAGE_COPCAR,  //20
    PAGE_CANDLE,
    PAGE_CLUBLIGHTS,
    PAGE_COLORCHASE,
    PAGE_TV,
    PAGE_PAPARAZZI,
    PAGE_LIGHTNING,
    PAGE_FIREWORKS,
    PAGE_FIRE,     //28
    PAGE_PARTYLIGHT,
    PAGE_CCT_LIMIT_SETTING,
    PAGE_HSI_LIMIT_SETTING,
    PAGE_OUTPUT_MODE,
#if UI_HIGH_SPEED_MODE
    PAGE_HIGH_SPD_MODE,
#endif
    PAGE_DMX_MODE,
    PAGE_DIMMING_CURVE,
    PAGE_FAN_MODE,
    PAGE_STUDIO_MODE,    
    PAGE_FREQUENCY_SELECTION,
    PAGE_CONTROL_SYSTEM,
    PAGE_LANGUAGE,
    PAGE_PRODUCT_INFO,
    PAGE_UPDATE,
    PAGE_FACTORY,  
    PAGE_WORK_MODE,
    PAGE_CUSTOM_FX,
    PAGE_BATTERY_OPTION,
    PAGE_OVERHEAT,
    PAGE_SIDUS_PRO_FX,
    PAGE_SCREENSAVER,
#if UI_ELECT_ACCESSORY
    PAGE_ELECT_ACCESSORY,
#endif
    PAGE_CALIBRATION_MODE,
    PAGE_TEST,
    PAGE_NONE = 0xFF,
};
typedef uint8_t page_id_t;

enum {
    PAGE_INVALID_TYPE_IDLE = 0,
	PAGE_INVALID_TYPE_REFRESH,
    PAGE_INVALID_TYPE_PORTION,
    PAGE_INVALID_TYPE_ALL,
};
typedef uint8_t page_invalid_type_t;

enum {
    EVENT_DATA_WRITE,
    EVENT_DATA_PULL,
    EVENT_DATA_COMPULSION,
	EVENT_DATA_HS_WINDOW,
};


typedef struct _page {
    const char* name;
    uint8_t id;
    uint8_t subid;
    uint8_t next_subid;
    uint8_t index;
    uint8_t prev_def_pid;
    void *user_data;
    void (*init_cb)(void);
    void (*exit_cb)(void);
    void (*time_upd_cb)(uint32_t ms);
    void (*event_cb)(int event);
    page_invalid_type_t invalid_type;
    struct _page *next_page;    
}page_t;

#define PAGE_INITIALIZER(_name, _id, _init_cb, _exit_cb, _time_upd_cb, _event_cb, _last_def_id)\
                        { \
                                .name = _name,\
                                .id = _id,\
                                .subid = 0,\
                                .index = 0,\
                                .prev_def_pid = _last_def_id,\
                                .user_data = NULL,          \
                                .init_cb = _init_cb, \
                                .exit_cb = _exit_cb, \
                                .time_upd_cb = _time_upd_cb, \
                                .event_cb = _event_cb,      \
                                .invalid_type = PAGE_INVALID_TYPE_IDLE,      \
                                .next_page = NULL,          \
                        }
/**********************
 * GLOBAL PROTOTYPES
 **********************/

extern page_t page_desktop;
extern page_t page_sys_menu;
extern page_t page_light_effect;
extern page_t page_xy;
extern page_t page_hsi;
extern page_t page_cct;
extern page_t page_rgb;
extern page_t page_gel;
extern page_t page_source;
extern page_t page_sys_menu;
extern page_t page_light_effect;
extern page_t page_xy_setting;
extern page_t page_hsi_setting;
extern page_t page_cct_setting;
extern page_t page_rgb_setting;
extern page_t page_gel_setting;
extern page_t page_source_setting;
extern page_t page_pulsing;
extern page_t page_strobe;
extern page_t page_explosion;
extern page_t page_faultybulb;
extern page_t page_welding;
extern page_t page_partylight;
extern page_t page_lightning;
extern page_t page_tv;
extern page_t page_fire;
extern page_t page_fireworks;
extern page_t page_faultybulb;
extern page_t page_paparazzi;
extern page_t page_copcar;
extern page_t page_colorchase;
extern page_t page_clublights;
extern page_t page_candle;
extern page_t page_cct_limit_settiing;
extern page_t page_hsi_limit_setting;
extern page_t page_outputmode;
extern page_t page_language;
extern page_t page_dmxmode;
extern page_t page_dimmingcurve;
extern page_t page_fanmode;
extern page_t page_frqsel;
extern page_t page_productinfo;
extern page_t page_ctrl_sys;
extern page_t page_update;
extern page_t page_customfx;
extern page_t page_studiomode;
extern page_t page_factory_reset;
extern page_t page_workmode;
extern page_t page_overheat;
extern page_t page_uitest;
extern page_t page_battery_option;
extern page_t page_siduspro;
extern page_t page_screensavar;
extern page_t page_high_spd_mode;
extern page_t page_elect_accessory;
extern page_t page_calibration_mode;



#ifdef __cplusplus
}
#endif

#endif // !PAGE_H
