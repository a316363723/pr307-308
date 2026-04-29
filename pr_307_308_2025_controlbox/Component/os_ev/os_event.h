#ifndef OS_EVENT_H
#define OS_EVENT_H

#include <stdint.h>
#include <stdbool.h>


#if !defined(assert)
    #define assert(x)		(void)(x)    
#endif


#define BIT(x)           (1 << (x))
#define X(x)                    (x)
#define os_main_event(_name, _wrap)	    \
enum _name##_event {				    \
    _name##_EV_DATA      = _wrap(0),	    \
    _name##_EV_BLE       = _wrap(1),		\
    _name##_EV_LIGHT     = _wrap(2),	    \
    _name##_EV_DMX       = _wrap(3),		\
    _name##_EV_ETH       = _wrap(4),		\
    _name##_EV_USB       = _wrap(5),		\
    _name##_EV_GUI       = _wrap(6),		\
    _name##_EV_POWER     = _wrap(7),	    \
	_name##_EV_CFX     = _wrap(8),	    \
    _name##_EV_MAX       = _wrap(9),	    \
};

#define OS_EV_DEFINE()      \
os_main_event(MAIN, BIT);   \
os_main_event(MAIN_ID, X);  \

OS_EV_DEFINE()

enum dmx_event {
    EV_DMX_CONNECTED = BIT(0),
    EV_DMX_DISCONNECTED = BIT(1),
    EV_CRMX_POWER = BIT(2),
	EV_CRMX_UNLINK = BIT(3),
};

enum os_ble_event {
    OS_EVENT_BLE_POWER = 1 << 0,				// 
	OS_EVENT_BLE_RESET = 1 << 1,				//  
	OS_EVENT_BLE_CTR_LIGHT = 1 << 2,			// ???????????
	OS_EVENT_BLE_POWER_ON_OFF = 1 << 3,			// ?????????
	OS_EVENT_BLE_FACTORY_RESET = 1 << 4,		// ???????????
};

enum data_center_event
{
    EV_DATA_CONFIG_CHANGE          = BIT(0),
    EV_DATA_LIGHT_CHANGE           = BIT(1),
    EV_DATA_FACTORY_RESET          = BIT(2),
    EV_DATA_READ_VER               = BIT(3),
    EV_DATA_CFX_BANK_RW            = BIT(4),
    EV_DATA_READ_CFX_NAME          = BIT(5),
    EV_DATA_WRITE_CFX_NAME         = BIT(6),
    EV_DATA_READ_MOTOR_ANGLE       = BIT(7),
	EV_DATA_RESET_MOTOR            = BIT(8),
	EV_DATA_RESET_FRESENEL         = BIT(9),
};

enum gui_event {
    EV_GUI_EXIT_SCREEN_PROTECTOR = BIT(0),
};

enum os_light_event
{
    EV_DATA_LIGHT_TRIGGRT  = BIT(1),
    EV_DATA_LIGHT_MATCH = BIT(2),
	EV_DATA_LIGHT_SWITCH = BIT(3),
	EV_DATA_LIGHT_DISCONNECT = BIT(4),
	EV_DATA_LIGHT_ZERO = BIT(5),

};

enum os_usb_event
{
    OS_EVENT_USB_UPDATE_START = BIT(0),
	OS_EVENT_USB_BATCH_UPDATE_START = BIT(1),

};

enum os_power_event
{
    EV_LCD_POWER = BIT(0),
    EV_FAN_STOP = BIT(1),
    EV_MCU_TEMP_ALARM = BIT(2),
};


enum os_eth_event
{
    EV_IP_CHANGED = BIT(0),
};

struct elist_item {    
    uint32_t task_id;
    uint32_t event_filter;    
    void*    event_handle;
    void     (*call_back)(uint32_t event_type, uint32_t event_value);
    struct elist_item* next;
};

typedef struct {
    struct elist_item* head;
    int    event_data[32];
}event_list_t;

struct os_event_core {
    event_list_t event_map[MAIN_ID_EV_MAX];
};

int  os_ev_subscribe_event(uint32_t event_type, uint32_t event_filter, void(*call_back)(uint32_t event_type, uint32_t event_value));
void os_ev_publish_event(uint32_t event_type, uint32_t event_value, uint32_t data);
void os_ev_pull_event(uint32_t event_type);
int  os_ev_get_event_data(uint32_t event_type, uint32_t event_value);
uint8_t get_first_bit_pos(uint32_t value);
#endif
