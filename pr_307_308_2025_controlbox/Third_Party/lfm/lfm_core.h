#ifndef LFM_CORE_H
#define LFM_CORE_H

#include <stdint.h>
#include <stdbool.h>

#define USE_ASSERT

#ifdef USE_ASSERT
#define assert_param(expr) \
    do                     \
    {                      \
        if (expr)          \
            while (1)      \
                ;          \
    } while (0);
#else
#define assert_param(expr) ((void)0)
#endif

enum lfm_device_type {
    LFM_DEVICE_RS485 = 0,
    LFM_DEVICE_WIRELESS,
    LFM_DEVICE_MAX,
};

enum lfm_mode {
    LFM_FOLLOWER = 0,
    LFM_LEADER,
};

enum lfm_event {
    LFM_EVENT_DATA_SYNC     = (1 << 0),  /* 数据同步事件 */
    LFM_EVENT_BLE_RESET     = (1 << 1),	 /* 蓝牙复位事件 */
    LFM_EVENT_CRMX_UNPAIR   = (1 << 2),	 /* CRMX取消配对事件 */
    LFM_EVENT_FACTORY_RESET = (1 << 3),  /* 恢复出厂设置事件 */
    LFM_EVENT_PAGE_SYNC     = (1 << 4),  /* 页面同步事件 */
    LFM_EVENT_UPGRADE       = (1 << 5),  /* 升级事件 */
	LFM_EVENT_POWER         = (1 << 6),  /* 开关事件 */
	LFM_EVENT_ELECTRIC_MOVE = (1 << 7),  /* 电动移动事件 */
    LFM_EVENT_ALL           = (LFM_EVENT_DATA_SYNC | LFM_EVENT_BLE_RESET | LFM_EVENT_CRMX_UNPAIR | LFM_EVENT_FACTORY_RESET | \
							   LFM_EVENT_PAGE_SYNC | LFM_EVENT_UPGRADE | LFM_EVENT_POWER | LFM_EVENT_ELECTRIC_MOVE),
    LFM_EVENT_NONE          = 0,
};

enum lfm_data_type {
    LFM_DATA_TYPE_HSI = 0,
    LFM_DATA_TYPE_RGB,
    LFM_DATA_TYPE_GEL,
    LFM_DATA_TYPE_XY,
    LFM_DATA_TYPE_SOURCE,
    LFM_DATA_TYPE_CCT,  
	LFM_DATA_TYPE_CLUBLIGHTS,
	LFM_DATA_TYPE_PAPARAZZI,
	LFM_DATA_TYPE_LIGHTNING,
	LFM_DATA_TYPE_TV,
	LFM_DATA_TYPE_CANDLE,
	LFM_DATA_TYPE_FIRE,
	LFM_DATA_TYPE_STROBE, 
	LFM_DATA_TYPE_EXPLOSION,
	LFM_DATA_TYPE_FAULTYBULB,
    LFM_DATA_TYPE_PULSING,
    LFM_DATA_TYPE_WELDING,
    LFM_DATA_TYPE_COPCAR,
    LFM_DATA_TYPE_COLORCHASE,   
    LFM_DATA_TYPE_FIREWORKS,
    LFM_DATA_TYPE_PARTYLIGHT,
	LFM_DATA_TYPE_LIGHTNING_II,
	LFM_DATA_TYPE_TV_II,
	LFM_DATA_TYPE_FIRE_II,
	LFM_DATA_TYPE_STROBE_II,
	LFM_DATA_TYPE_EXPLOSION_II,
	LFM_DATA_TYPE_FAULTYBULB_II,
	LFM_DATA_TYPE_PULSING_II,
	LFM_DATA_TYPE_WELDING_II,
	LFM_DATA_TYPE_COPCAR_II,
	LFM_DATA_TYPE_PARTYLIGHT_II,
	LFM_DATA_TYPE_FIREWORKS_II,
	LFM_DATA_TYPE_CCT_LIMIT,
	LFM_DATA_TYPE_HSI_LIMIT,
	LFM_DATA_TYPE_XY_SET,
	LFM_DATA_TYPE_RGB_SET,
	LFM_DATA_TYPE_HSI_SET,
	LFM_DATA_TYPE_CCT_SET,
	LFM_DATA_TYPE_GEL_SET,
	LFM_DATA_TYPE_SOURCE_SET,
    LFM_DATA_TYPE_NONE = 0xFF,
};

#pragma pack(1)
struct lfm_frame_header {    
    char     uuid[6];       /* 控制盒蓝牙UUID，用来识别机型, 例如000G5 */
    uint8_t  lamp_type;     /* 灯具类型：<0-1000> */
    uint8_t  protocol_ver;  /* 协议版本 */
    uint8_t  soft_ver;      /* 控制盒的软件版本 */
    uint8_t  hard_ver;      /* 控制盒的硬件版本 */
    uint8_t  event;         /* 控制盒事件 */
    uint32_t frame_sn;      /* 当前帧序 */
};

struct lfm_comm_setting {
    uint8_t  fan_mode;          /* 风扇模式 */
    uint8_t  curve;             /* 调光曲线 */
    uint8_t  output_mode;       /* 输出模式 */
    uint8_t  studio_mode;       /* 演播厅模式 */
    uint8_t  language;          /* 语言 */
    uint8_t  ble_status;        /* 蓝牙状态 */
    uint8_t  crmx_status;       /* CRMX状态 */
    uint8_t  crmx_unpair;       /* CRMX取消配对 */
    uint16_t frequency;         /* 频率 */
    uint8_t  dmx_profile_index; /* DMX Profile 索引 */
    uint8_t  dmx_loss_behavior; /* DMX 丢失行为 */
//    uint16_t dmx_address;       /* DMX 地址 */
    uint8_t  lighting_effect;   /* 当前的光效类型 */
    uint8_t  lighting_mode;     /* 当前的光模式 */
    uint8_t  mode;              /* 当前处于光效模式或者光模式 */
	uint8_t  hsmode;            /* 高速摄影模式 */
	uint8_t  eth_state;         /* 以太网状态 */
	uint8_t  power_state;       /* 开关状态 */
	int16_t  yoke_pan;          /* 水平角度 */
	int16_t  yoke_tilt;         /* 俯仰角度 */
	uint16_t fresnel_angle;     /* 电动菲涅尔角度 */
	uint8_t  motor_reset;       /* 电机复位状态 */
    uint8_t  pid;               /* 页面ID */
    uint8_t  spid;              /* 子页面ID */
    uint8_t  data_type;         /* 数据类型 */
    uint16_t data_size;         /* 数据大小 */
	uint8_t  data[0];
};

enum lfm_upgrade_step {
    LFM_UPGRADE_STEP_START,
    LFM_UPGRADE_STEP_DATA,
    LFM_UPGRADE_STEP_END,
};

enum lfm_upgrade_state {
    LFM_UPGRADE_STATE_IDLE = 0,
    LFM_UPGRADE_STATE_START,
    LFM_UPGRADE_STATE_ING,
    LFM_UPGRADE_STATE_ERR,
    LFM_UPGRADE_STATE_TIMROUT,
    LFM_UPGRADE_STATE_END,
};

enum {
    LFM_ERR_NO_LAMP                  = -1,            /* 灯体不存在         */
    LFM_ERR_FW_OVER_SIZE             = -2,            /* 灯体大小不正确     */
    LFM_ERR_FW_TYPE                  = -3,            /* 产品固件不正确     */
    LFM_ERR_FW_NO_MATCH              = -4,            /* 产品固件类型不匹配 */
    LFM_ERR_COMMUNICATE_FAILED       = -5,            /* 通讯错误           */
    LFM_ERR_COMMUNICATE_TIMEOUT      = -6,            /* 通讯超时           */
    LFM_ERR_CRC_FAILED               = -7,            /* CRC32校验失败      */
    LFM_ERR_INTERNEL                 = -8,            /* 内部逻辑错误       */    
    LFM_ERR_OK                       =  0,            /* 没有错误           */
};

struct lfm_firmware_info {
    char      ProductName[16];   /*产品名字*/
    uint32_t  FirmwareVersion;   /*软件版本*/
    uint32_t  HardwareVersion;   /*硬件版本*/
    uint32_t  FirmwareSize;      /*软件固件大小*/
    uint32_t  CheckCRC32;        /*加密前的Firmware crc32校验*/
    uint16_t  FirmwareType;      /*0:app   1:base data  2:app+base_data*/
};

struct lfm_upgrade_header {
    uint8_t  type;              /* 升级数据类型 */
    uint8_t  step;              /* 升级步骤 */
    uint8_t  error;             /* 升级错误 */
    uint16_t data_len;          /* 升级数据长度 */
    uint32_t prog_addr;         /* 编程地址 */
    uint8_t  data[0];           /* 升级数据 */
};
#pragma pack(0)

struct lfm_core;
struct lfm_upgrade_ops {
    int  (*start)( struct lfm_core* core,  struct lfm_upgrade_header* p_header, uint32_t* prog_size);
    int  (*data) ( struct lfm_core* core,  struct lfm_upgrade_header* p_header, uint32_t* crc32);
    int  (*end)  ( struct lfm_core* core,  struct lfm_upgrade_header* p_header);
    int  (*check_firmware)(uint8_t type, struct lfm_firmware_info*);
    void (*time_callback)(const struct lfm_core* core, uint32_t ms);
};

struct lfm_core_ops {
    /* 事件通知 */
//    int (*event_notify_cb)(enum lfm_event, struct lfm_comm_setting*);
	int (*event_notify_cb)(enum lfm_event, uint8_t *data);
	
    int (*refresh_before_send)(enum lfm_event, struct lfm_comm_setting*);

    uint8_t (*get_lfm_mode)(void);

    void (*set_lfm_mode)(uint8_t value);
};

struct lfm_device {
    enum lfm_device_type type;
    int (*write)(uint8_t* data, uint16_t len);
};

struct lfm_core {
    /* 升级结构体 */
    struct {
        uint32_t recv_bytes_cnt;
        volatile uint32_t crc32;
        uint32_t prog_size;
        uint32_t prog_percent;
        uint32_t next_addr;         
        int8_t   err;
        uint8_t  state;
        uint8_t  type : 4;
        uint8_t  end : 1;
        uint8_t  start : 1;                
        struct lfm_upgrade_ops*   ops;
        struct lfm_firmware_info  firmware_bin;
    } upgrade;

    struct lfm_core_ops* core_ops;

    /* 设备列表 */
    struct lfm_device* device[LFM_DEVICE_MAX];    

    /* 正在升级的标识 */
    uint8_t updating_flag;

    /* 当前设备设备类型 */
    enum lfm_device_type cur_dev_type;

    /* 事件列表句柄 */
    void* event_flags;          

    /* 升级FIFO句柄 */
    void* upgrade_msg_queue;

    /* 接收FIFO句柄 */
    void* rx_msg_queue;

    /* 发送处理任务句柄 */
    void* tx_task_handle;

    /* 接收处理任务句柄 */
    void* rx_task_handle;

    void* period_timer;

    void* trans_cmplt_semphr;

    /* 当前的帧序 */
    uint32_t cur_frame_number; 
};

void leader_follwer_mode_init(void);

/**
 * @brief 主从模式初始化，
 *        内部创建需要使用的系统资源
 */
void lfm_core_init(void);

/**
 * @brief 反初始化主从模式lfm_core结构体
 *      
 */
void lfm_core_deinit(void);

/**
 * @brief 注册主从模式的接口设备(目前支持RS485设备、CRMX设备)
 * 
 * @param dev 主从模式设备指针
 */
void lfm_register_device(struct lfm_device* dev);

/**
 * @brief 设备主从模式的设备类型
 * 
 * @param type LFM_DEVICE_RS485  485设备
 *             LFM_DEVICE_WIRELESS CRMX设备
 */
void lfm_core_set_type(enum lfm_device_type type);

/**
 * @brief 设置主从模式核心的操作函数集合
 * 
 * @param ops 核心操作函数集指针
 */
void  lfm_core_set_operations(struct lfm_core_ops* ops);

/**
 * @brief 设置主从模式升级的操作函数集合
 * 
 * @param ops 升级操作函数集指针
 */
void  lfm_upgrade_set_operations(struct lfm_upgrade_ops* ops);

/**
 * @brief 通常是USB升级任务调用，将升级数据进行打包，发送给主从模式tx_task
 * 
 * @param p_header 主从升级数据头(用户需要填写该结构体)
 * @param data     升级数据源指针
 */
void  lfm_transmit_upgrade_packet(struct lfm_upgrade_header* p_header, const uint8_t* data, uint32_t upgrade_uuid);

/**
 * @brief 等待升级数据包发送完成， 一般在调用完lfm_transmit_upgrade_packet
 *        后进行相应的函数调用
 * @param timeout 最长超时时间
 * @return true   没有超时
 * @return false  超时
 */
bool  lfm_upgrade_transfer_wait_finished(uint32_t timeout);

/**
 * @brief 获取485协议帧序帧(这个函数最好可以由485协议提供)
 * 
 * @param data 协议数据指针
 * @return uint32_t 返回帧序
 */
uint32_t  lfm_485_proto_frame_sn_get(uint8_t* data);

/**
 * @brief 获取主从的uuid是否对应(这个函数最好可以由485协议提供)
 * 
 * @param data 协议数据指针
 * @return uint8_t 返回状态
 */
uint8_t lfm_485_proto_frame_uuid_crc(uint8_t* data);

/**
 * @brief 获取接收消息队列的句柄,
 * 
 * @return void* 接收消息队列的句柄
 */
void* lfm_core_get_rx_msg_queue(void);

/**
 * @brief 添加主从事件
 * 
 * @param flag 
 *      LFM_EVENT_DATA_SYNC
 *      LFM_EVENT_BLE_RESET
 *      LFM_EVENT_CRMX_UNPAIR
 *      LFM_EVENT_FACTORY_RESET
 *      LFM_EVENT_PAGE_SYNC
 */
void  lfm_core_add_event_flag(uint32_t flag);

/**
 * @brief 设置发送光效数据标记
 * 
 * @param flag 1--发送完成
 */
void lfm_send_light_data_set_flag(uint8_t flag);

/**
 * @brief 获取发送光效数据标记
 * 
 * @return flag 1--发送完成
 */
uint8_t lfm_send_light_data_get_flag(void);

void lfm_read_data_handle(uint8_t *rx_data);

uint32_t lfm_batch_upgrade_uuid_check(uint32_t uuid_arg);

uint8_t lfm_batch_upgrade_type_check(uint32_t uuid_arg);

uint8_t lfm_upgrade_send_state_get(void);

#endif // !LFM_CORE_H
