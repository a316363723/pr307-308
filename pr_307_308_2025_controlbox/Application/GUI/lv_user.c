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

/**********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *  STATIC VARIABLES
 **********************/

/*********************
 *  GLOBAL VATIABLES
 *********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * @brief 获取在group中的该对象的ID
 * 
 * @param group 
 * @param obj 
 * @return uint8_t 
 */
uint8_t lv_group_user_get_obj_id(lv_group_t* group, lv_obj_t* obj)
{
    if (group == NULL || obj == NULL)  return 0;
    
    LV_LOG_TRACE("Begin");

    lv_obj_t** obj_i;
    uint8_t index = 0;
    _LV_LL_READ(&group->obj_ll, obj_i) {        
        if ((*obj_i) == obj) {
            LV_LOG_INFO("the object is already added to this group");            
            return index;
        }

        index++;
    }

    return index;
}

/**
 * @brief 获取当前被聚焦对象在组里面的索引
 * 
 * @param group 
 * @return uint8_t 
 */
uint8_t lv_group_user_get_focused_id(lv_group_t* group)
{
    lv_obj_t* obj = lv_group_get_focused(group);
    return lv_group_user_get_obj_id(group, obj);
}

/**
 * @brief 通过ID聚焦这个group的对象
 * 
 * @param group 
 * @param id 
 */
void lv_group_user_focus_obj_by_id(lv_group_t* group, uint8_t id)
{
    if (group == NULL)  return;

    uint8_t objs_sum = lv_group_get_obj_count(group);
    
    if (id >= objs_sum) {
        id = 0;
    }    
    
    lv_obj_t** obj_i;
    _LV_LL_READ(&group->obj_ll, obj_i) {                
        if (id == 0) {
            break;
        }

        id--;
    }

    if (obj_i == NULL)      return;
    lv_group_focus_obj(*obj_i);
}
