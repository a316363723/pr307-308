#ifndef __USER_H
#define __USER_H

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
#define PAGE_TIMER_UPD_PERIOD_MS       200

/**********************
 *      TYPEDEFS
 **********************/

typedef enum {
    INTERACT_STATUS_IDLE = 0,
    INTERACT_STATUS_ENTER_SCREEN_PROTECT,
    INTERACT_STATUS_EXIT_SCREEN_PROTECT,
    INTERACT_STATUS_ON_SCREEN_PROCTECT,
    INTERACT_STATUS_POWER_OFF,
    INTERACT_STATUS_ENTER_LOW_VOLTAGE,
    INTERACT_STATUS_EXIT_LOW_VOLTAGE,
    INTERACT_STATUS_ON_LOW_VOLTAGE,    
    INTERACT_STATUS_ENTER_WARNING,
    INTERACT_STATUS_ON_WARNING,
    INTERACT_STATUS_EXIT_WARNING,
    INTERACT_STATUS_SWITCH_PAGE,
	INTERACT_STATUS_hs_window_PAGE,
	
}interact_status_t;

/**********************
    STATIC PROTOTYPES
 **********************/
static inline void user_set_interact_status(uint8_t status);

static inline void user_set_last_interact_status(uint8_t status);

/**********************
 * GLOBAL PROTOTYPES
 **********************/

void user_switch_page(uint8_t mode);

void user_enter_protector(void);
void user_exit_proctector(void);

void user_enable_power_off(void);
void user_disable_power_off(void);

void user_enter_low_volt_page(void);
void user_exit_low_volt_page(void);

interact_status_t user_get_interact_status(void);

bool user_get_power_off_flag(void);
void user_turn_to_page(uint8_t id, uint8_t subid, bool save);
bool user_interact_with_ui(void);

void user_enter_siduspro(void);
void user_enter_overheat(void);
void user_enter_dmx(void);
void user_enter_dmx_addr(void);
void user_enter_dmx_profile(void);
void user_enter_crmx_pairing(void);
void user_enter_ble_resetting(void);
void user_enter_updating(void);
void user_enter_sync_updating(void);
void user_enter_error(void);

void user_enter_mixplug_warn(void);
void user_exit_mixplug_warn(void);
uint8_t user_get_effects_ui(uint8_t mode);
uint8_t ui_get_effectts_mode(uint8_t page_id);

void user_unlock_page_manager(void);
void user_lock_page_manager(void);
void user_enter_ele_access(uint8_t pid, uint8_t spid);
void crmx_is_paired_set(bool state);
uint8_t user_get_act_pid(void);
interact_status_t user_get_interact_status(void);
interact_status_t user_get_last_interact_status(void);
void user_enter_hs_window(void);

#ifdef __cplusplus
}
#endif

#endif
