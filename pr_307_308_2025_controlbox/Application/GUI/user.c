/*********************
 *      INCLUDES
 *********************/
#include "user.h"
#include "ui_data.h"
#include "page_manager.h"
#include "app_data_center.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/
typedef struct {
    uint8_t pid;        /* 页面主ID */
    uint8_t spid;       /* 页面子ID */    
    bool save;
}interact_switch_page_t;

/**********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *  STATIC VARIABLES
 **********************/
static interact_status_t interact_status = INTERACT_STATUS_IDLE;
static interact_status_t last_interact_status = INTERACT_STATUS_IDLE;
static interact_switch_page_t switch_page;
static bool b_switch_page = false;
static bool locked = false;
static uint8_t fx_version = 0;
/*********************
 *  GLOBAL VATIABLES
 *********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void user_interact_run_exit_cb(interact_status_t status)
{
    switch(status)
    {        
        case INTERACT_STATUS_ON_SCREEN_PROCTECT:        
        {
            screen_proctector_exit();
        }
        break;
        case INTERACT_STATUS_ON_LOW_VOLTAGE:
        {            
            screen_low_power_exit();  
        }
        break;
        case INTERACT_STATUS_ON_WARNING:
        {
            screen_mixplug_exit();
        }   
        break;
        default: break;
    }
}


/**
 * @brief Users use status machine to interact with GUI.
 * 
 * @return true 
 * @return false 
 */
bool user_interact_with_ui(void)
{
    bool enable_time_upd = true;
	
    switch(interact_status)
    {
        case INTERACT_STATUS_IDLE:        break;
        case INTERACT_STATUS_ENTER_SCREEN_PROTECT:
        {
//            screen_del_l2_panel();
			screen_exist_l2_panel();
            screen_proctector_create();
            enable_time_upd = false;
			
            user_set_interact_status(INTERACT_STATUS_ON_SCREEN_PROCTECT);
        }
        break;
        case INTERACT_STATUS_EXIT_SCREEN_PROTECT:
        {
            screen_proctector_exit();
			
			work_screen_proctector_exit(1);
			
            user_set_interact_status(INTERACT_STATUS_IDLE);
            enable_time_upd = false;
        }
        break;
        case INTERACT_STATUS_ENTER_LOW_VOLTAGE:
        {
            uint8_t status = user_get_last_interact_status();            
            screen_del_l2_panel();                      
            user_interact_run_exit_cb((interact_status_t)status);
            screen_low_power_create();            
            user_set_interact_status(INTERACT_STATUS_ON_LOW_VOLTAGE);
            enable_time_upd = false;            
        }
        break;
        case INTERACT_STATUS_EXIT_LOW_VOLTAGE:
        {            
            screen_low_power_exit();
            user_set_interact_status(INTERACT_STATUS_IDLE);         
            enable_time_upd = false;
        }
        break;
        case INTERACT_STATUS_ENTER_WARNING:
        {
            uint8_t status = user_get_last_interact_status();                                    
            screen_del_l2_panel();
            user_interact_run_exit_cb((interact_status_t)status);
            screen_mixplug_create();
            user_set_interact_status(INTERACT_STATUS_ON_WARNING);
        }
        break;
        case INTERACT_STATUS_EXIT_WARNING:
        {
            screen_mixplug_exit();
            user_set_interact_status(INTERACT_STATUS_IDLE);
            enable_time_upd = false;
        }   
        break;
        case INTERACT_STATUS_ON_WARNING:
        case INTERACT_STATUS_ON_LOW_VOLTAGE:
        case INTERACT_STATUS_POWER_OFF:
        case INTERACT_STATUS_ON_SCREEN_PROCTECT:
        {
            enable_time_upd = false;
        }   
        break;
		
		case INTERACT_STATUS_hs_window_PAGE:
			
			page_highspd_window_construct();
			user_set_interact_status(INTERACT_STATUS_IDLE);
			break;
		default:break;
    }

    //更新页面
    if (b_switch_page)
    {
        b_switch_page = false;
        if (!locked)        
        {
            if (!switch_page.save)
            {
                screen_load_page_with_clear(switch_page.pid, switch_page.spid, false);
            }
            else  
            {
                screen_load_page(switch_page.pid, switch_page.spid, switch_page.save);
            }            
        }

        return false;
    }

    return enable_time_upd;
}

/**
 * @brief Make GUI enter into the screen proctetor.
 * 
 */
void user_enter_protector(void)
{
    if (!(user_get_interact_status() >= INTERACT_STATUS_ENTER_SCREEN_PROTECT &&\
                 user_get_interact_status() <= INTERACT_STATUS_ON_SCREEN_PROCTECT ))
    {
        user_set_last_interact_status(interact_status);
        user_set_interact_status(INTERACT_STATUS_ENTER_SCREEN_PROTECT);
    }    
}

/**
 * @brief Exit the screen proctector.
 * 
 */
void user_exit_proctector(void)
{
    if (user_get_interact_status() == INTERACT_STATUS_ON_SCREEN_PROCTECT)
    {
        user_set_last_interact_status(interact_status);
        user_set_interact_status(INTERACT_STATUS_EXIT_SCREEN_PROTECT);        
    }    
}


/**
 * @brief Users make GUI power off.
 * 
 */
void user_enable_power_off(void)
{
    if (user_get_interact_status() != INTERACT_STATUS_POWER_OFF)
    {
        user_set_last_interact_status(interact_status);
        user_set_interact_status(INTERACT_STATUS_POWER_OFF);           
    }
}

/**
 * @brief Users disable GUI power off.
 * 
 */
void user_disable_power_off(void)
{
    if (user_get_interact_status() == INTERACT_STATUS_POWER_OFF)
    {
        user_set_last_interact_status(interact_status);
        user_set_interact_status(INTERACT_STATUS_IDLE);
    }
}

/**
 * @brief Enter into the low voltage screen. 
 * 
 */
void user_enter_low_volt_page(void)
{
    if (user_get_interact_status() < INTERACT_STATUS_ENTER_LOW_VOLTAGE)
    {
        user_set_last_interact_status(interact_status);
        user_set_interact_status(INTERACT_STATUS_ENTER_LOW_VOLTAGE);
    }    
}

/**
 * @brief Exit the low voltage screen. 
 * 
 */
void user_exit_low_volt_page(void)
{
    if (user_get_interact_status() == INTERACT_STATUS_ON_LOW_VOLTAGE)
    {
        user_set_last_interact_status(interact_status);
        user_set_interact_status(INTERACT_STATUS_EXIT_LOW_VOLTAGE);
    }    
}

/**
 * @brief Switch function of user.
 * 
 * @param id 
 * @param subid 
 */
void user_turn_to_page(uint8_t id, uint8_t subid, bool save)
{
    switch_page.pid = id;
    switch_page.spid = subid;
    switch_page.save = save;
    b_switch_page = true;
}

/**
 * @brief 切换到光效页面
 * 
 * @param id 
 * @param fx_ver 
 */
void user_turn_to_lighteffect_page(uint8_t id, uint8_t fx_ver)
{
    switch_page.pid = id;
    switch_page.spid = 0;    
    switch_page.save = false;
    b_switch_page = true;
    fx_version = fx_ver;
}

/**
 * @brief Get the flag of GUI.
 * 
 * @return true 
 * @return false 
 */
bool user_get_power_off_flag(void)
{
    return user_get_interact_status() == INTERACT_STATUS_POWER_OFF ? true : false;
}

/**
 * @brief Get the interactive status.
 * 
 * @return interact_status_t 
 */
interact_status_t user_get_interact_status(void)
{
    return interact_status;
}

/**
 * @brief Get the last interactive status.
 * 
 * @return interact_status_t 
 */
interact_status_t user_get_last_interact_status(void)
{
    return last_interact_status;
}

/**
 * @brief 进入Sidus Pro FX 界面
 * 
 */
void user_enter_siduspro(void)
{
    user_turn_to_page(PAGE_SIDUS_PRO_FX, 0, false);
}

/**
 * @brief 进入过热页面
 * 
 */
void user_enter_overheat(void)
{
    user_turn_to_page(PAGE_OVERHEAT, 0, true);
}

/**
 * @brief 进入DMX页面
 * 
 */
void user_enter_dmx(void)
{
	if(PAGE_UPDATE != screen_get_act_pid() || ((PAGE_UPDATE == screen_get_act_pid() && SUB_ID_UPDATING != screen_get_act_spid())))
		user_turn_to_page(PAGE_DMX_MODE, SUB_ID_DMX_STATUS, false);
}

/**
 * @brief 进入DMX 地址页面
 * 
 */
void user_enter_dmx_addr(void)
{
    user_turn_to_page(PAGE_DMX_MODE, SUB_ID_DMX_ADDRESS, false);
}

/**
 * @brief 进入DMX profile页面
 * 
 */
void user_enter_dmx_profile(void)
{
    user_turn_to_page(PAGE_DMX_MODE, SUB_ID_DMX_PROFILE, false);
}


/**
 * @brief 进入蓝牙复位界面
 * 
 */
void user_enter_ble_resetting(void)
{
    user_turn_to_page(PAGE_CONTROL_SYSTEM,SUB_ID_BLE_RESETING,true);
}

/**
 * @brief 进入CRMX配对页面
 * 
 */
void user_enter_crmx_pairing(void)
{
	if(PAGE_UPDATE != screen_get_act_pid() || ((PAGE_UPDATE == screen_get_act_pid() && SUB_ID_UPDATING != screen_get_act_spid())))
		user_turn_to_page(PAGE_CONTROL_SYSTEM,SUB_ID_LUMENRADIO_PAIRING,true);
}

/**
 * @brief 进入电动附件页面
 * 
 */
void user_enter_ele_access(uint8_t pid, uint8_t spid)
{
//    if(pid != screen_get_act_pid() && spid != screen_get_act_spid())
        user_turn_to_page(pid, spid, true);
}

/**
 * @brief 进入系统更新页面
 * 
 */
void user_enter_updating(void)
{
//    if(pid != screen_get_act_pid() && spid != screen_get_act_spid())
        user_turn_to_page(PAGE_UPDATE, SUB_ID_UPDATING, true);
}

/**
 * @brief 进入批量固件更新页面
 * 
 */
void user_enter_sync_updating(void)
{
//    if(pid != screen_get_act_pid() && spid != screen_get_act_spid())
        user_turn_to_page(PAGE_WORK_MODE, SUB_ID_SYNC_UPDATAING, true);
}

/**
 * @brief 进入错误界面
 * 
 */
void user_enter_error(void)
{
//    if(pid != screen_get_act_pid() && spid != screen_get_act_spid())
        user_turn_to_page(PAGE_PRODUCT_INFO, 7, true);
}


/**
 * @brief 进入混插界面
 * 
 */
void user_enter_mixplug_warn(void)
{
    if (user_get_interact_status() < INTERACT_STATUS_ENTER_WARNING)
    {
        user_set_last_interact_status(interact_status);
        user_set_interact_status(INTERACT_STATUS_ENTER_WARNING);
    }    
}

/**
 * @brief 退出混插界面
 * 
 */
void user_exit_mixplug_warn(void)
{
    if (user_get_interact_status() == INTERACT_STATUS_ON_WARNING)
    {
        user_set_last_interact_status(interact_status);
        user_set_interact_status(INTERACT_STATUS_EXIT_WARNING);
    }    
}

/**
 * @brief 对用户锁定页面调度器
 * 
 */
void user_lock_page_manager(void)
{
    locked = true;
}

/**
 * @brief 解锁界面调度器
 * 
 */
void user_unlock_page_manager(void)
{
    locked = false;
}

/**
 * @brief 获取光效版本
 * 
 * @return uint8_t 
 */
uint8_t user_get_fx_version(void)
{
    return fx_version;
}

/**
 * @brief 获取当前UI
 * 
 * @return uint8_t 
 */
uint8_t user_get_act_pid(void)
{
    return screen_get_act_pid();
}

/**
 * @brief 设置光效版本
 * 
 * @param version 
 */
void user_set_fx_version(uint8_t version)
{
    fx_version = version;
}


void user_enter_hs_window(void)
{
	 if (user_get_interact_status() != INTERACT_STATUS_hs_window_PAGE)
    {
		user_set_interact_status(INTERACT_STATUS_hs_window_PAGE);
		
	}
}

void user_exit_hs_window(void)
{
	 if (user_get_interact_status() != INTERACT_STATUS_hs_window_PAGE)
    {
		user_set_interact_status(INTERACT_STATUS_hs_window_PAGE);
	}
}



uint8_t user_get_effects_ui(uint8_t mode)
{
    uint8_t page_id = PAGE_SIDUS_PRO_FX;
    switch (mode)
    {
        case LIGHT_MODE_CCT:                page_id = PAGE_LIGHT_MODE_CCT;break;	
		#if PROJECT_TYPE==307
        case LIGHT_MODE_HSI:                page_id = PAGE_LIGHT_MODE_HSI;break;
        case LIGHT_MODE_GEL:                page_id = PAGE_LIGHT_MODE_GEL;break;
        case LIGHT_MODE_XY:                 page_id = PAGE_LIGHT_MODE_XY; break;
        case LIGHT_MODE_RGB:                page_id = PAGE_LIGHT_MODE_RGB;break;
        case LIGHT_MODE_SOURCE:             page_id = PAGE_LIGHT_MODE_SOURCE;break;		
        case LIGHT_MODE_FX_PULSING_II:      page_id = PAGE_PULSING; break;
        case LIGHT_MODE_FX_STROBE_II:       page_id = PAGE_STROBE;  break;
        case LIGHT_MODE_FX_EXPLOSION_II:    page_id = PAGE_EXPLOSIOIN; break;
        case LIGHT_MODE_FX_FAULT_BULB_II:   page_id = PAGE_FAULTYBULB; break;
        case LIGHT_MODE_FX_WELDING_II:      page_id = PAGE_WELDING; break;
        case LIGHT_MODE_FX_COP_CAR_II:      page_id = PAGE_COPCAR; break;
        case LIGHT_MODE_FX_CANDLE:          page_id = PAGE_CANDLE; break;
        case LIGHT_MODE_FX_CLUBLIGHTS:      page_id = PAGE_CLUBLIGHTS; break;
        case LIGHT_MODE_FX_COLOR_CHASE:     page_id = PAGE_COLORCHASE; break;
        case LIGHT_MODE_FX_TV_II:           page_id = PAGE_TV; break;
        case LIGHT_MODE_FX_PAPARAZZI:       page_id = PAGE_PAPARAZZI; break;
        case LIGHT_MODE_FX_LIGHTNING_II:    page_id = PAGE_LIGHTNING; break;
        case LIGHT_MODE_FX_FIREWORKS:       page_id = PAGE_FIREWORKS; break;
		case LIGHT_MODE_FX_PARTY_LIGHTS:    page_id = PAGE_SIDUS_PRO_FX; break;
        case LIGHT_MODE_FX_FIRE_II:         page_id = PAGE_FIRE; break;
//        case LIGHT_MODE_FX_STROBE:          page_id = PAGE_STROBE;  break;
//        case LIGHT_MODE_FX_PULSING:         page_id = PAGE_PULSING; break;
//        case LIGHT_MODE_FX_TV:              page_id = PAGE_TV; break;
//        case LIGHT_MODE_FX_LIGHTNING:       page_id = PAGE_LIGHTNING; break;
//        case LIGHT_MODE_FX_FAULT_BULB:      page_id = PAGE_FAULTYBULB; break;
//        case LIGHT_MODE_FX_EXPLOSION:       page_id = PAGE_EXPLOSIOIN; break;
		#endif
		#if PROJECT_TYPE==308
		case LIGHT_MODE_FX_FIRE:            page_id = PAGE_FIRE; break;
	    case LIGHT_MODE_FX_STROBE:          page_id = PAGE_STROBE;  break;
        case LIGHT_MODE_FX_PULSING:         page_id = PAGE_PULSING; break;
        case LIGHT_MODE_FX_TV:              page_id = PAGE_TV; break;
        case LIGHT_MODE_FX_LIGHTNING:       page_id = PAGE_LIGHTNING; break;
        case LIGHT_MODE_FX_FAULT_BULB:      page_id = PAGE_FAULTYBULB; break;
        case LIGHT_MODE_FX_EXPLOSION:       page_id = PAGE_EXPLOSIOIN; break;
		case LIGHT_MODE_FX_FIREWORKS:       page_id = PAGE_FIREWORKS; break;
		case LIGHT_MODE_FX_PAPARAZZI:       page_id = PAGE_PAPARAZZI; break;
		#endif
        case LIGHT_MODE_SIDUS_MFX:                      
        case LIGHT_MODE_SIDUS_PFX:                   
        case LIGHT_MODE_SIDUS_CFX:                  
        case LIGHT_MODE_SIDUS_CFX_PREVIEW:  page_id = PAGE_SIDUS_PRO_FX; break;
        default:break;
    }
    return page_id;
}

void user_switch_page(uint8_t mode)
{
    uint8_t page_id = PAGE_SIDUS_PRO_FX;
    page_id = user_get_effects_ui(mode);
    user_turn_to_page(page_id, 0, false);
}

uint8_t ui_get_effectts_mode(uint8_t page_id)
{
	uint8_t mode = LIGHT_MODE_CCT;
	
    switch (page_id)
    {
        case PAGE_LIGHT_MODE_CCT:                mode = LIGHT_MODE_CCT;break;	
		#if PROJECT_TYPE==307
        case PAGE_LIGHT_MODE_HSI:                mode = LIGHT_MODE_HSI;break;
        case PAGE_LIGHT_MODE_GEL:                mode = LIGHT_MODE_GEL;break;
        case PAGE_LIGHT_MODE_XY:                 mode = LIGHT_MODE_XY; break;
        case PAGE_LIGHT_MODE_RGB:                mode = LIGHT_MODE_RGB;break;
        case PAGE_LIGHT_MODE_SOURCE:             mode = LIGHT_MODE_SOURCE;break;		
        case PAGE_PULSING:      			     mode = LIGHT_MODE_FX_PULSING_II; break;
        case PAGE_STROBE:                        mode = LIGHT_MODE_FX_STROBE_II;  break;
        case PAGE_EXPLOSIOIN:                    mode = LIGHT_MODE_FX_EXPLOSION_II; break;
        case PAGE_FAULTYBULB:                    mode = LIGHT_MODE_FX_FAULT_BULB_II; break;
        case PAGE_WELDING:                       mode = LIGHT_MODE_FX_WELDING_II; break;
        case PAGE_COPCAR:                        mode = LIGHT_MODE_FX_COP_CAR_II; break;
        case PAGE_CANDLE:                        mode = LIGHT_MODE_FX_CANDLE; break;
        case PAGE_CLUBLIGHTS:                    mode = LIGHT_MODE_FX_CLUBLIGHTS; break;
        case PAGE_COLORCHASE:                    mode = LIGHT_MODE_FX_COLOR_CHASE; break;
        case PAGE_TV:                            mode = LIGHT_MODE_FX_TV_II; break;
        case PAGE_PAPARAZZI:                     mode = LIGHT_MODE_FX_PAPARAZZI; break;
        case PAGE_LIGHTNING:                     mode = LIGHT_MODE_FX_LIGHTNING_II; break;
        case PAGE_FIREWORKS:                     mode = LIGHT_MODE_FX_FIREWORKS; break;
		case PAGE_SIDUS_PRO_FX:                  mode = LIGHT_MODE_FX_PARTY_LIGHTS; break;
        case PAGE_FIRE:                          mode = LIGHT_MODE_FX_FIRE_II; break;
//        case LIGHT_MODE_FX_STROBE:          mode = PAGE_STROBE;  break;
//        case LIGHT_MODE_FX_PULSING:         mode = PAGE_PULSING; break;
//        case LIGHT_MODE_FX_TV:              mode = PAGE_TV; break;
//        case LIGHT_MODE_FX_LIGHTNING:       mode = PAGE_LIGHTNING; break;
//        case LIGHT_MODE_FX_FAULT_BULB:      mode = PAGE_FAULTYBULB; break;
//        case LIGHT_MODE_FX_EXPLOSION:       mode = PAGE_EXPLOSIOIN; break;
		#endif
		#if PROJECT_TYPE==308
		case PAGE_FIRE:                     mode = LIGHT_MODE_FX_FIRE; break;
	    case PAGE_STROBE:                   mode = LIGHT_MODE_FX_STROBE;  break;
        case PAGE_PULSING:                  mode = LIGHT_MODE_FX_PULSING; break;
        case PAGE_TV:                       mode = LIGHT_MODE_FX_TV; break;
        case PAGE_LIGHTNING:                mode = LIGHT_MODE_FX_LIGHTNING; break;
        case PAGE_FAULTYBULB:               mode = LIGHT_MODE_FX_FAULT_BULB; break;
        case PAGE_EXPLOSIOIN:               mode = LIGHT_MODE_FX_EXPLOSION; break;
		case PAGE_FIREWORKS:                mode = LIGHT_MODE_FX_FIREWORKS; break;
		case PAGE_PAPARAZZI:                mode = LIGHT_MODE_FX_PAPARAZZI; break;
		#endif
        default:break;
    }
    return mode;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/
/**
 * @brief Set interactive status.
 * 
 * @param status 
 */
static inline  void user_set_interact_status(uint8_t status)
{
    interact_status = (interact_status_t)status;
}

/**
 * @brief Set last interactive status.
 * 
 * @param status 
 */
static inline void user_set_last_interact_status(uint8_t status)
{
    last_interact_status = (interact_status_t)status;
}
