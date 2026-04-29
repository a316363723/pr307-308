#ifndef APP_USB_H
#define APP_USB_H
#include "stdint.h"
#include "stdbool.h"
#include "cmsis_os.h"


//#define CONFIG_PRODUCT_NAME         "MR232_L3"
//#define CONFIG_PRODUCT_UUID         "400C5"//F21c
//#define CONFIG_USB_UPDATE_CTR_FILE_NAME "Amaran_F21c_HardwareV1.0_SoftwareV?.?.bin"
//#define CONFIG_USB_UPDATE_LAMP_FILE_NAME "Amaran_F21c_HardwareV1.0_SoftwareV?.?.bin"

//#define BOOT_START_ADDRESS      (0x8000000)
//#define BASE_DATA_START_ADDRESS (0x8001000)
//#define APP_START_ADDRESS       (0x8008000)
//#define UPDATE_START_ADDRESS    (0x8040000)
//#define FLASH_END_ADDRESS       (0x8080000 - 1)

//#define BOOT_AREA_SIZE          (BASE_DATA_START_ADDRESS - BOOT_START_ADDRESS)
//#define BASE_DATA_AREA_SIZE     (APP_START_ADDRESS - BASE_DATA_START_ADDRESS)
//#define APP_AREA_SIZE           (UPDATE_START_ADDRESS - APP_START_ADDRESS)
//#define UPDATE_AREA_SIZE        (FLASH_END_ADDRESS - UPDATE_START_ADDRESS + 1)

//#define UPDATE_FLAG_ADDR        (0x20017E00)
//#define SECURITY_START_ADDR		(0x20017F00)    //Security Encrypt.o 
//#define SECURITY_AREA_SIZE		(0x100)   		//256 Byte

//#define MAX_FIRMWARE_FILE_SIZE	(BASE_DATA_AREA_SIZE + APP_AREA_SIZE + SECURITY_AREA_SIZE)


enum update_step_enum
{
    UPDATE_IDLE,
//	UPDATE_BOOT_FILE_SEARCH,
//    UPDATE_BOOT_FILE_ACQUIRE,
    UPDATE_ELE_FRNSEL_FILE_SEARCH,
    UPDATE_ELE_FRNSEL_FILE_ACQUIRE,
    UPDATE_ELE_BRACKET_FILE_SEARCH,
    UPDATE_ELE_BRACKET_FILE_ACQUIRE,
    UPDATE_LAMP_FILE_SEARCH,
    UPDATE_LAMP_FILE_ACQUIRE,
    UPDATE_BMP_FILE_SEARCH,
    UPDATE_BMP_FILE_ACQUIRE,
    UPDATE_CTR_FILE_SEARCH,
    UPDATE_CTR_FILE_ACQUIRE,
    UPDATE_CTR_REBOOT,
};

enum u_disk_status_enum
{
    U_DISK_PULL_OUT,
    U_DISK_INSERT,
    U_DISK_FORMWARE_ERROR,
    
};

struct usb_status_str
{
    bool fs_mount_state;
    enum update_step_enum update_step;
};


extern const osThreadAttr_t g_usb_cb_thread_attr;
extern osThreadId_t app_usb_task_id;

void usb_msc_disconnnected_handle(void);
void usb_msc_application_handle(void);
void app_usb_entry(void *argument);
void app_usb_update_handle(void);
uint8_t app_usb_fatfile_state_get(void);
uint8_t app_usb_batch_upgrade_file_find(void);
uint8_t app_usb_fatfile_state_get(void);
uint8_t app_usb_batch_upgrade_state_get(void);
void app_usb_batch_upgrade_handle(void);

#endif


