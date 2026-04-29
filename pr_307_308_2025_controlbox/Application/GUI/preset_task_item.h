#ifndef PRESET_TASK_ITEM_H
#define PRESET_TASK_ITEM_H

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

struct _preset_task_desc_t;

typedef struct {
  lv_obj_t obj;
  struct _preset_task_desc_t* p_task_desc;
}preset_task_item_t;

extern const lv_obj_class_t preset_task_item_obj_class;

struct preset_data;
/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Create a obj objects
 * @param parent    pointer to an object, it will be the parent of the new obj.
 * @return          pointer to the created obj
 */
lv_obj_t * preset_task_item_create(lv_obj_t * parent);

void preset_task_item_set_param(lv_obj_t* obj, void *param);

struct preset_data* preset_task_item_get_param(lv_obj_t* obj);

uint8_t preset_task_item_get_type(lv_obj_t* obj);

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif // !PRESET_TASK_ITEM_H
