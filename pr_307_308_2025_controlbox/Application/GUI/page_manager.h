#ifndef PAGE_MANAGER_H
#define PAGE_MANAGER_H

#ifdef  __cplusplus
extern "C" {
#endif //  __cplusplus

/*********************
 *      INCLUDES
 *********************/
#include "page.h"

/*********************
 *      DEFINES
 *********************/
#define PAGE_TIMER_UPD_PERIOD_MS       200

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/
void page_manager_init(void);

void page_maneger_register(page_t* page, void* user_data);

void page_maneger_delete(page_t* page); //testing

void screen_load_page(uint8_t id, uint8_t subid, bool b_record);

void screen_load_page_and_index(uint8_t id, uint8_t subid, uint8_t index, bool b_record);

void screen_load_page_with_clear(uint8_t id, uint8_t subid, bool b_record);

void screen_load_sub_page(uint8_t subid, uint8_t index);

bool screen_turn_prev_page(void);

void screen_load_sub_page_with_stack(uint8_t sid, uint8_t index);

bool screen_load_prev_sub_page(uint8_t def_spid, uint8_t index);

uint8_t screen_get_act_pid(void);

uint8_t screen_get_act_index(void);

void screen_set_act_index(uint8_t index);

page_invalid_type_t page_get_invalid_type(void);

void page_set_invalid_type(page_invalid_type_t type);

void* screen_get_act_page_data(void);

void page_set_user_data(uint8_t id, void *user_data);

uint8_t page_get_sub_id(uint8_t id);

void page_set_sub_id(uint8_t id,  uint8_t subid);

uint8_t page_get_index(uint8_t pid);

void page_event_send(int event);

const char* screen_get_act_page_name(void);

void screen_set_act_spid(uint8_t spid);

uint8_t screen_get_act_spid(void);

uint8_t screen_get_top_id(void);

uint8_t screen_get_prev_id(void);

uint8_t screen_get_last_page_id(void);

uint8_t page_get_stacktop_id(void);

void screen_proctector_create(void);

void screen_proctector_exit(void);

void work_screen_proctector_exit(uint8_t state);

void screen_del_l2_panel(void);

void screen_exist_l2_panel(void);

void screen_low_power_exit(void);

void screen_low_power_create(void);

uint8_t menu_get_index_by_id(uint8_t pid);

void page_manager_process(void* t);

bool screen_auto_power_on(void);

void screen_mixplug_create(void);

void screen_mixplug_exit(void);

uint8_t light_effect_get_index(uint8_t pid);

void foce_turn_to_next_page(uint8_t id, uint8_t subid, bool b_record);

void power_screen_load_page_with_clear(uint8_t id, uint8_t subid, bool b_record);

void page_highspd_window_construct(void);

void set_hs_fx_window(void);

bool get_hs_fx_window_state(void);

void hs_fx_window_state_set(uint8_t state);

#ifdef __cplusplus
}
#endif

#endif
