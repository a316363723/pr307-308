#ifndef MENU_ITEM_OBJ_H
#define MENU_ITEM_OBJ_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "lvgl/lvgl.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

typedef struct {
    lv_obj_t obj;
    const lv_img_dsc_t* img;
    lv_coord_t max_width;
    lv_coord_t min_width;    
    uint16_t   min_ratio;
    uint16_t   max_ratio;
}menu_item_obj_t;

extern const lv_obj_class_t menu_item_obj_obj_class;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Create a obj objects
 * @param parent    pointer to an object, it will be the parent of the new obj.
 * @return          pointer to the created obj
 */
lv_obj_t * menu_item_obj_create(lv_obj_t * parent);

void menu_item_obj_set_param(lv_obj_t* obj, const lv_img_dsc_t* img, lv_coord_t min_width, lv_coord_t max_width);

void menu_item_obj_set_ratio(lv_obj_t* obj, uint16_t min_ratio, uint16_t max_ratio);

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif // !MENU_ITEM_OBJ_H
