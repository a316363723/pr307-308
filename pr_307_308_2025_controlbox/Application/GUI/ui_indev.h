#ifndef UI_INDEV_H
#define UI_INDEV_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include <stdint.h>
#include <stdbool.h>
#include "lvgl/lvgl.h"

/*********************
 *      DEFINES
 *********************/
//
//正转
#define LV_ENCODER_KEY_CW  LV_KEY_RIGHT
//反转
#define LV_ENCODER_KEY_CC  LV_KEY_LEFT

/*********************
 *      TPEDEFS
 *********************/
struct lv_indev_data_t;
/**********************
 * GLOBAL PROTOTYPES
 **********************/
uint32_t enc_get_step_index(void);
int32_t comm_enc_get_step(uint32_t value);
int32_t xy_enc_get_step(uint32_t value);
int32_t adv_hsi_hue_get_step(uint32_t value);
int32_t common_intensity_get_step(uint32_t value);
int32_t universe_enc_get_step(uint32_t value);
int32_t motor_enc_get_step(uint32_t value);

int32_t common_angle_get_step(uint32_t value);

void enc_level_proc(int32_t enc_diff);
void indev_set_lock(bool locked);
bool indev_get_lock(void);
void enc_read_proc(lv_indev_data_t* data, int32_t enc_cnt, bool pressed);

#ifdef __cplusplus
}
#endif

#endif
