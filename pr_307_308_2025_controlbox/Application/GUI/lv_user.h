#ifndef LV_USER_H
#define LV_USER_H

#ifdef  __cplusplus
extern "C" {
#endif //  __cplusplus

/*********************
 *      INCLUDES
 *********************/
#include <stdint.h>
/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/
struct _lv_group_t;
struct _lv_obj_t;
/**********************
 * GLOBAL PROTOTYPES
 **********************/

uint8_t lv_group_user_get_obj_id(struct _lv_group_t* group, struct _lv_obj_t* obj);

void lv_group_user_focus_obj_by_id(struct _lv_group_t* group, uint8_t id);

uint8_t lv_group_user_get_focused_id(struct _lv_group_t* group);

#ifdef __cplusplus
}
#endif

#endif // !__PAGE
