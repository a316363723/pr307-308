#ifndef UI_ASSETS_H
#define UI_ASSETS_H

#include "lang.h"
#include "font.h"
#include "unit.h"
#include "lvgl/lvgl.h"

struct lv_img_dsc_t;

const lv_img_dsc_t* title_get_curve_img(uint8_t curve_type);
const lv_img_dsc_t* title_get_small_curve_img(uint8_t type);
const lv_img_dsc_t* title_get_power_img(uint8_t power_type);
const lv_img_dsc_t* title_get_comm_img(uint8_t comm_type);
const lv_img_dsc_t* title_get_fan_img(uint8_t fan_mode);
const lv_img_dsc_t* ui_get_fanmode_img(uint8_t fan_mode);
const lv_img_dsc_t *ui_get_mastermode_img(uint8_t mode);
const lv_img_dsc_t *ui_get_high_speed_img(void);
const lv_img_dsc_t* title_get_indoor_power_img(void);

#endif
