#ifndef PROJECT_CONFIG
#define PROJECT_CONFIG



//#define CONFIG_SOFTWARE_VERSION     (0x10)  /* 软件版本号 */
//#define CONFIG_HARDWARE_VERSION     (0x10)  /* 硬件版本号 */

//升级缓存放在内部falsh时
#if 0
#define BOOT_START_ADDRESS      (0x8000000)
#define APP_START_ADDRESS       (0x8001000)
#define BASE_DATA_START_ADDRESS (0x8040000)
#define UPDATE_START_ADDRESS    (0x8048000)
#define FLASH_END_ADDRESS       (0x8100000 - 1)

//#define BOOT_AREA_SIZE          (APP_START_ADDRESS - BOOT_START_ADDRESS)
#define APP_AREA_SIZE           (BASE_DATA_START_ADDRESS - APP_START_ADDRESS)
#define BASE_DATA_AREA_SIZE     (UPDATE_START_ADDRESS - BASE_DATA_START_ADDRESS)
#define UPDATE_AREA_SIZE        (FLASH_END_ADDRESS - UPDATE_START_ADDRESS + 1)
#define SECURITY_AREA_SIZE		(0x100)   		//256 Byte
#define FIRMWARE_HEAD_SIZE      (128UL)
#define MAX_FIRMWARE_FILE_SIZE	(0x1E0000)
#endif
//升级缓存放在外部falsh时
#if 1
#define BOOT_START_ADDRESS       (0x0000000)
#define APP_START_ADDRESS        (0x0001000)
#define BASE_DATA_START_ADDRESS  (0x0000180)
#define UPDATE_START_ADDRESS     (0x0400000)  //从4M开始存放
#define BMP_UPDATE_START_ADDRESS (0x0200000)  //从2M开始存放
#define FLASH_END_ADDRESS       (0x1E2000)
#define HEAD_INFO_SIZE          (128)

//#define BOOT_AREA_SIZE          (APP_START_ADDRESS - BOOT_START_ADDRESS)
#define APP_AREA_SIZE           (BASE_DATA_START_ADDRESS - APP_START_ADDRESS)
#define BASE_DATA_AREA_SIZE     (UPDATE_START_ADDRESS - BASE_DATA_START_ADDRESS)
#define UPDATE_AREA_SIZE        (0x1E0000)
#define SECURITY_AREA_SIZE		(0x100)   		//256 Byte
#define FIRMWARE_HEAD_SIZE      (128UL) 
#define MAX_FIRMWARE_FILE_SIZE	(0x1E0000)
#define BOOT_AREA_SIZE          (0x16000)

#define USE_BOOT_UPDATING  (0)

#endif


//#define CONFIG_PRODUCT_NAME         "MR232_L3"

#if PROJECT_TYPE==307

#define PROJECT_TYPE    307
#define CONFIG_PRODUCT_MACHINE      (0U)    /*0：无/SFL  1：变焦/VFL  2：变焦+航向+俯仰/TA*/
#define CONFIG_PRODUCT_CCT_MAX      (10000U) /*色温上限*/
#define CONFIG_PRODUCT_CCT_MIN      (2000U) /*色温下限*/
#define CONFIG_PRODUCT_LED_TYPE     (6U)    /*0：D; 1：T; 2：WW；3:WWG; 4：RGB; 5：WRGB; 6：WWRGB; 7:WWRGBCMYP*/
#define CONFIG_PRODUCT_FUNCTION     (3U)    /*0:单色温；1：双色温；2：简单彩色；3：全功能彩色*/
//#define CONFIG_USB_UPDATE_CTR_FILE_NAME "Aputure_PR_307_L3_Ctr_HardwareV1.0_SoftwareV?.?.bin"
//#define CONFIG_USB_UPDATE_LAMP_FILE_NAME "Aputure_PR_307_L3_Lamp_HardwareV1.0_SoftwareV?.?.bin"
#define CONFIG_USB_UPDATE_CTR_FILE_NAME "ES_CS15_Ctr"
#define CONFIG_USB_UPDATE_LAMP_FILE_NAME "ES_CS15_Drv"
#define CONFIG_USB_UPDATE_BMP_FILE_NAME "ES_CS15_Bmp"
#if USE_BOOT_UPDATING == 1
#define CONFIG_USB_UPDATE_BOOT_FILE_NAME "ES_CS15_Boot"
#endif
#define CONFIG_USB_UPDATE_ELE_FRNSEL_FILE_NAME "Motorized_F14_Fresnel"
#define CONFIG_USB_UPDATE_ELE_BRACKET_FILE_NAME "Motorized_Yoke"

#define     EFFECTS_USE_EXPLOSIONS     1
#define     EFFECTS_USE_PAPARAZZI      1
#define     EFFECTS_USE_FIREWORKS      1
#define     EFFECTS_USE_FAULT_BULB     1 
#define     EFFECTS_USE_LIGHTNING      1
#define     EFFECTS_USE_TV             1
#define     EFFECTS_USE_PULSING        1
#define     EFFECTS_USE_STROBE         1
#define     EFFECTS_USE_FIRE           1
#define     EFFECTS_USE_COP_CAR        1
#define     EFFECTS_USE_PARTY          1
#define     EFFECTS_USE_CLUB_LIGHTS    1 
#define     EFFECTS_USE_WELDING        1
#define     EFFECTS_USE_CANDLE         1        
#define     EFFECTS_USE_COLOR_CHASE    1

#define LFM_UPGRADE_BOX_UUID            (0x00000901)
#define LFM_UPGRADE_LAMP_UUID           (0x00000902)
#define LFM_UPGRADE_BMP_UUID            (0x00000903)
#define LFM_UPGRADE_ELE_BRACKET_UUID    (0x00060101)
#define LFM_UPGRADE_ELE_FRNSEL_UUID     (0x00060201)
#elif PROJECT_TYPE==308
#define PROJECT_TYPE    308
#define CONFIG_PRODUCT_MACHINE      (0U)    /*0：无/SFL  1：变焦/VFL  2：变焦+航向+俯仰/TA*/
#define CONFIG_PRODUCT_CCT_MAX      (6500) /*色温上限*/
#define CONFIG_PRODUCT_CCT_MIN      (2700U) /*色温下限*/
#define CONFIG_PRODUCT_LED_TYPE     (3U)    /*0：D; 1：T; 2：WW；3:WWG; 4：RGB; 5：WRGB; 6：WWRGB; 7:WWRGBCMYP*/
#define CONFIG_PRODUCT_FUNCTION     (1U)    /*0:单色温；1：双色温；2：简单彩色；3：全功能彩色*/
#define CONFIG_USB_UPDATE_CTR_FILE_NAME "ES_XT26_Ctr"
#define CONFIG_USB_UPDATE_LAMP_FILE_NAME "ES_XT26_Drv"
#define CONFIG_USB_UPDATE_BMP_FILE_NAME "ES_XT26_Bmp"
#if USE_BOOT_UPDATING == 1
#define CONFIG_USB_UPDATE_BOOT_FILE_NAME "ES_XT26_Boot"
#endif
#define CONFIG_USB_UPDATE_ELE_FRNSEL_FILE_NAME "Motorized_F14_Fresnel"
#define CONFIG_USB_UPDATE_ELE_BRACKET_FILE_NAME "Motorized_Yoke"

#define     EFFECTS_USE_EXPLOSIONS     1
#define     EFFECTS_USE_PAPARAZZI      1
#define     EFFECTS_USE_FIREWORKS      1
#define     EFFECTS_USE_FAULT_BULB     1 
#define     EFFECTS_USE_LIGHTNING      1
#define     EFFECTS_USE_TV             1
#define     EFFECTS_USE_PULSING        1
#define     EFFECTS_USE_STROBE         1
#define     EFFECTS_USE_FIRE           1
#define     EFFECTS_USE_COP_CAR        0
#define     EFFECTS_USE_PARTY          0
#define     EFFECTS_USE_CLUB_LIGHTS    0 
#define     EFFECTS_USE_WELDING        0
#define     EFFECTS_USE_CANDLE         0        
#define     EFFECTS_USE_COLOR_CHASE    0

#define LFM_UPGRADE_BOX_UUID            (0x00001001)
#define LFM_UPGRADE_LAMP_UUID           (0x00001002)
#define LFM_UPGRADE_BMP_UUID            (0x00001003)
#define LFM_UPGRADE_ELE_BRACKET_UUID    (0x00060101)
#define LFM_UPGRADE_ELE_FRNSEL_UUID     (0x00060201)
#endif

#define FRESNEL_ANGLE_MIN 15
#define FRESNEL_ANGLE_MAX  50

#define ANGLE_LIMIT_DEFAULT_MIN  (-135)
#define ANGLE_LIMIT_DEFAULT_MAX  (135)

#define ANGLE_LIMIT_MIN  0
#define ANGLE_LIMIT_MAX  540

#define SCANNER_G_FLASH_OFFS (0x1200000)   //扫码枪外部flash地址

#define USB_UPDATA_BOX_DEVICE        "ctrl_box"
#define USB_UPDATA_LAMP_DEVICE       "ctrl_lamp"
#if USE_BOOT_UPDATING == 1
#define USB_UPDATA_BOOT_DEVICE        "ctrl_box_boot"
#endif

#define WIRELESS_DMX_USE  1   //是否开启CRMX功能
#define ETH_DMX_USE       1   //是否开启ETH功能



#define DIM_FREQ_MIN 0
#define DIM_FREQ_MAX 2000

#define FAN_EXIST    1
#define DIMMING_CURVE_EXIST   1
#define FREQUENCY_SEL_EXIST    0



#endif  /* PROJECT_CONFIG */

