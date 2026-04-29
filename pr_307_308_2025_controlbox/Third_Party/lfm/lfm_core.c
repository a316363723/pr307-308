#include "l_config.h"
#include "lfm_core.h"
#include "rs485_protocol.h"
#include "cmsis_os2.h"
#include "ui_data.h"
#include <string.h>
#include "os_event.h"

static void tx_task_entry(void *arg);
static void rx_task_entry(void *arg);
static void period_timer_callback(void* arg);

static int  parse_lfm_data(uint8_t* p_data, uint16_t size);
static void rs485_proto_parse(uint8_t* buffer);
static void init_frame_header(struct lfm_frame_header* p_header, enum lfm_event e);
static void init_frame_upgrade_header(struct lfm_frame_header* p_header, enum lfm_event e, uint32_t uuid);
static int  check_frame_header(struct lfm_frame_header* p_header);
static uint8_t check_sum_calc(const uint8_t* buff, uint16_t size);

static void upgrade_process(uint8_t* data);
static void upgrade_core_init(struct lfm_core* core, struct lfm_firmware_info* firmware_bin);
static void upgrade_packet_wrap(uint8_t* p_des_buff, const struct lfm_upgrade_header* p_upgrade_header, const uint8_t* p_upgrade_data, uint32_t upgrade_uuid);
static uint16_t upgrade_packet_get_length(uint8_t* buff);
static uint32_t event_packet_wrap(uint8_t* p_data, enum lfm_event e);
static void broadcast_data(uint8_t* data, uint16_t len);


#if LFM_DEBUG
struct lfm_core s_lfm_core;
uint32_t dbg_frame_sn[10];
//static uint32_t lfm_rx_task_size;
#else  
static struct lfm_core s_lfm_core;
#endif

static const osThreadAttr_t s_rx_task_attr = {
  .name = "rx_task",
  .priority = (osPriority_t) osPriorityBelowNormal,
  .stack_size = RX_TASK_STACK_DEPTH * 4
};

static const osThreadAttr_t s_tx_task_attr = {
  .name = "tx_task",
  .priority = (osPriority_t) osPriorityBelowNormal,
  .stack_size = TX_TASK_STACK_DEPTH * 4
};

void lfm_core_deinit(void)
{
    memset(&s_lfm_core, 0, sizeof(struct lfm_core));
}

void* lfm_core_get_rx_msg_queue(void)
{
    return s_lfm_core.rx_msg_queue;
}

void lfm_core_init(void)
{    
//    assert(s_lfm_core.core_ops == NULL);
//    assert(s_lfm_core.upgrade.ops == NULL);

//    osKernelLock();
    s_lfm_core.cur_dev_type = LFM_DEVICE_RS485;
    s_lfm_core.cur_frame_number = 0;
    s_lfm_core.event_flags = osEventFlagsNew(NULL);
    s_lfm_core.rx_msg_queue = osMessageQueueNew(RX_MSG_QUEUE_CNT, RX_MSG_QUEUE_ITEM_SIZE, NULL);
    s_lfm_core.upgrade_msg_queue = osMessageQueueNew(UPGRADE_MSG_QUEUE_CNT, UPGRADE_MSG_QUEUE_ITEM_SIZE, NULL); 
    s_lfm_core.period_timer = osTimerNew(period_timer_callback, osTimerPeriodic, NULL, NULL);	
    s_lfm_core.tx_task_handle = osThreadNew(tx_task_entry, NULL, &s_tx_task_attr);
    s_lfm_core.rx_task_handle = osThreadNew(rx_task_entry, NULL, &s_rx_task_attr);
    s_lfm_core.trans_cmplt_semphr = osSemaphoreNew(1, 0, NULL);
//    osKernelUnlock();
	osTimerStart(s_lfm_core.period_timer, PERIODIC_TIME_MS);
}

void  lfm_core_add_event_flag(uint32_t flag)
{
    osEventFlagsSet(s_lfm_core.event_flags, flag);
}

/**
 * @brief 等待升级包发送完成
 * 
 * @param timeout 
 * @return true 
 * @return false 
 */
bool lfm_upgrade_transfer_wait_finished(uint32_t timeout)
{     
	osDelay(timeout);
	return true;
}

/**
 * @brief 设置主从模式的设备类型
 * 
 * @param type 
 */
void  lfm_core_set_type(enum lfm_device_type type)
{
    if (type >= LFM_DEVICE_MAX)
        return ;

    s_lfm_core.cur_dev_type = type;
}

/**
 * @brief 注册设备
 * 
 * @param dev 
 */
void lfm_register_device(struct lfm_device* dev)
{
    if (( dev == NULL ) || ( dev->type >= LFM_DEVICE_MAX ))
        return ;

    s_lfm_core.device[ dev->type ] = dev;
}

/**
 * @brief 设置操作函数集
 * 
 * @param ops 
 */
void lfm_upgrade_set_operations(struct lfm_upgrade_ops* ops)
{
    if ( ops == NULL )
        return ;

    s_lfm_core.upgrade.ops = ops;
}

/**
 * @brief 设置操作字符集
 * 
 * @param ops 
 */
void lfm_core_set_operations(struct lfm_core_ops* ops)
{
    if ( ops == NULL )
        return ;
    
    s_lfm_core.core_ops = ops;
}

/**
 * @brief 向消息队列中发送升级数据
 * 
 * @param p_header 
 * @param data 
 */
void lfm_transmit_upgrade_packet(struct lfm_upgrade_header* p_header, const uint8_t* data, uint32_t upgrade_uuid)
{
    static uint8_t  shared_mem[RX_MSG_QUEUE_ITEM_SIZE]; 
    upgrade_packet_wrap(shared_mem, p_header, data, upgrade_uuid);
    osMessageQueuePut(s_lfm_core.upgrade_msg_queue, shared_mem, 0, osWaitForever);
}

uint32_t lfm_485_proto_frame_sn_get(uint8_t* data)
{
	rs485_proto_header_t* p_recv_header = (rs485_proto_header_t*)data;			
	return p_recv_header->serial_num;
}

uint8_t lfm_485_proto_frame_uuid_crc(uint8_t* data)
{
	uint8_t res = 0;
	uint32_t uuid = 0;
	struct lfm_frame_header* p_recv_header = (struct lfm_frame_header*)data;	
		
	if(strncmp(p_recv_header->uuid, MACHINE_UUID, 5) != 0 )
	{
		res = 1;
		uuid = (p_recv_header->uuid[3] << 24) | (p_recv_header->uuid[2] << 16) | (p_recv_header->uuid[1] << 8) | (p_recv_header->uuid[0]);
		if(lfm_batch_upgrade_type_check(uuid) == 0xff)
		{
			res = 1;
		}
		else if(lfm_batch_upgrade_type_check(uuid) >= 0 && lfm_batch_upgrade_type_check(uuid) <= FIRMWARE_TYPE_ELE_FRNSEL)
		{
			res = 0;
		}
	}

	
	return res;
}

void lfm_read_data_handle(uint8_t *rx_data)
{
	if (osMessageQueueGet(s_lfm_core.rx_msg_queue, rx_data, NULL, 1) == osOK)
	{
		rs485_proto_parse(rx_data);
	}
}

/**
 * @brief 周期定时器
 * 
 * @param arg 
 */
static void period_timer_callback(void* arg)
{
    static uint32_t s_prog_addr = 0;
    static uint32_t s_time_cnt = 0;

    /* 处理升级失败. */
    if ((s_lfm_core.upgrade.err < 0) && \
                ((s_lfm_core.upgrade.state == LFM_UPGRADE_STATE_START) || \
                 (s_lfm_core.upgrade.state == LFM_UPGRADE_STATE_ING)))
    {
		os_ev_publish_event(MAIN_EV_LIGHT, EV_DATA_LIGHT_SWITCH, 1);
		lfm_upgrade_transfer_wait_finished(1000);
		os_ev_publish_event(MAIN_EV_LIGHT, EV_DATA_LIGHT_TRIGGRT, 1);
		lfm_upgrade_transfer_wait_finished(1000);
        s_lfm_core.upgrade.state = LFM_UPGRADE_STATE_ERR;
    }

    /* 周期定时回调函数. */
    if (s_lfm_core.upgrade.ops->time_callback)
    {
        s_lfm_core.upgrade.ops->time_callback(&s_lfm_core, PERIODIC_TIME_MS);
    }

    switch (s_lfm_core.upgrade.state)
    {
        case LFM_UPGRADE_STATE_IDLE:
        {
            s_time_cnt = 0;
            s_prog_addr = 0;
        }
        break;
        case LFM_UPGRADE_STATE_START:        
        {
            s_lfm_core.upgrade.state = LFM_UPGRADE_STATE_ING;
        }
        break;
        case LFM_UPGRADE_STATE_ING:
        {						
            if (s_prog_addr != s_lfm_core.upgrade.next_addr)
            {
                s_prog_addr = s_lfm_core.upgrade.next_addr;
                s_time_cnt = 0;
            }
            else
            {
                s_time_cnt += PERIODIC_TIME_MS;
            }

            if (s_time_cnt >= COMMUNICATE_TIMEOUT_MS)
            {
                s_lfm_core.upgrade.err = LFM_ERR_COMMUNICATE_TIMEOUT;
                s_lfm_core.upgrade.state = LFM_UPGRADE_STATE_TIMROUT;
            }
        }
        break;
        case LFM_UPGRADE_STATE_TIMROUT:
        case LFM_UPGRADE_STATE_ERR:
        case LFM_UPGRADE_STATE_END:
        {       
			os_ev_publish_event(MAIN_EV_LIGHT, EV_DATA_LIGHT_SWITCH, 1);
			lfm_upgrade_transfer_wait_finished(500);
            s_lfm_core.upgrade.state = LFM_UPGRADE_STATE_IDLE;    
            s_lfm_core.upgrade.err = LFM_ERR_OK;        
        }
        break;
        default: break;
    }
}

/**
 * @brief 主从模式发送任务, 处理事件
 * 
 * @param arg 任务参数
 */
static void tx_task_entry(void *arg)
{
    static uint8_t tx_buff[RX_MSG_QUEUE_ITEM_SIZE];
    uint32_t event;
    uint16_t len;
    for (;;)
    {
        event = osEventFlagsWait(s_lfm_core.event_flags, LFM_EVENT_ALL, osFlagsWaitAny | osFlagsNoClear, 0);
        
        if (((int)event > 0) && (!s_lfm_core.updating_flag))
        {
            if (event & LFM_EVENT_DATA_SYNC)
            {
                osEventFlagsClear(s_lfm_core.event_flags, LFM_EVENT_DATA_SYNC);
                if (s_lfm_core.core_ops->get_lfm_mode() == LFM_LEADER)
                {
                    len = event_packet_wrap(tx_buff, LFM_EVENT_DATA_SYNC);
					ui_set_explosion_2_state(0);
					ui_set_lightning_2_trigger(0);
                    broadcast_data(tx_buff, len);			
                }                
            }
            else if (event & LFM_EVENT_FACTORY_RESET)
            {
                osEventFlagsClear(s_lfm_core.event_flags, LFM_EVENT_FACTORY_RESET);
//                if (s_lfm_core.core_ops->get_lfm_mode() == LFM_LEADER)  //york   屏蔽是为因为主机恢复出厂设置后主从模式变为了从模式，这里判断会不通过
//                {
                    len = event_packet_wrap(tx_buff, LFM_EVENT_FACTORY_RESET);
                    broadcast_data(tx_buff, len);
//                }                
            }
            else if (event & LFM_EVENT_BLE_RESET)
            {
                osEventFlagsClear(s_lfm_core.event_flags, LFM_EVENT_BLE_RESET);
                if (s_lfm_core.core_ops->get_lfm_mode() == LFM_LEADER)
                {
                    len = event_packet_wrap(tx_buff, LFM_EVENT_BLE_RESET);
                    broadcast_data(tx_buff, len);
                }
            }
            else if (event & LFM_EVENT_CRMX_UNPAIR)
            {
                osEventFlagsClear(s_lfm_core.event_flags, LFM_EVENT_CRMX_UNPAIR);
                if (s_lfm_core.core_ops->get_lfm_mode() == LFM_LEADER)
                {
                    len = event_packet_wrap(tx_buff, LFM_EVENT_CRMX_UNPAIR);
                    broadcast_data(tx_buff, len);
                }
                
            }
            else if (event & LFM_EVENT_PAGE_SYNC)
            {
                osEventFlagsClear(s_lfm_core.event_flags, LFM_EVENT_PAGE_SYNC);
                if (s_lfm_core.core_ops->get_lfm_mode() == LFM_LEADER)
                {
                    len = event_packet_wrap(tx_buff, LFM_EVENT_PAGE_SYNC);
                    broadcast_data(tx_buff, len);
                }
            }
			else if (event & LFM_EVENT_POWER)
            {
                osEventFlagsClear(s_lfm_core.event_flags, LFM_EVENT_POWER);
                if (s_lfm_core.core_ops->get_lfm_mode() == LFM_LEADER)
                {
                    len = event_packet_wrap(tx_buff, LFM_EVENT_POWER);
                    broadcast_data(tx_buff, len);
                }
            }
			else if (event & LFM_EVENT_ELECTRIC_MOVE)
            {
                osEventFlagsClear(s_lfm_core.event_flags, LFM_EVENT_ELECTRIC_MOVE);
                if (s_lfm_core.core_ops->get_lfm_mode() == LFM_LEADER)
                {
                    len = event_packet_wrap(tx_buff, LFM_EVENT_ELECTRIC_MOVE);
                    broadcast_data(tx_buff, len);
                }
            }
        }

        if (osMessageQueueGet(s_lfm_core.upgrade_msg_queue, tx_buff, NULL, 10) ==  osOK)
        {
            if (s_lfm_core.core_ops->get_lfm_mode() == LFM_LEADER)
            {
                len = upgrade_packet_get_length(tx_buff);
                broadcast_data(tx_buff, len);                
                osSemaphoreRelease(s_lfm_core.trans_cmplt_semphr);
            }            
        }	
    }
}

/**
 * @brief 主从模式接收任务，从消息队列中读取并解析
 * 
 * @param arg 
 */
static void rx_task_entry(void *arg)
{
    static uint8_t rx_buff[RX_MSG_QUEUE_ITEM_SIZE];

    for (;;)
    {
		if (osMessageQueueGet(s_lfm_core.rx_msg_queue, rx_buff, NULL, osWaitForever) == osOK)
		{
			rs485_proto_parse(rx_buff);
		}
    }
}

/**
 * @brief 解析485协议
 * 
 * @param data 
 */
static void rs485_proto_parse(uint8_t* data)
{
	rs485_proto_header_t* p_recv_header = (rs485_proto_header_t*)data;	
	uint8_t checksum = 0;

	if (p_recv_header->start != RS485_START_CODE)
		return ;	
        
	checksum = check_sum_calc(&p_recv_header->header_size, p_recv_header->msg_size - 2);
	if (checksum != p_recv_header->check_sum)
	{
		return ;
	}

	rs485_cmd_header_t* p_cmd_header = (rs485_cmd_header_t*)&data[p_recv_header->header_size];	
    parse_lfm_data(&data[p_recv_header->header_size + sizeof(rs485_cmd_header_t)], p_cmd_header->arg_size);  
}

extern void indev_set_dev_input_state(uint8_t state);

/**
 * @brief 接收解析函数
 * 
 * @param p_data 源数据指针
 * @param size   大小
 * @return int   
 */
static int parse_lfm_data(uint8_t* p_data, uint16_t size)
{
    static uint32_t prev_frame_sn = 0xFFFFFFFF;    
    struct lfm_frame_header* p_header = (struct lfm_frame_header*)p_data;
    int res; 
    
    if ((p_header == NULL) || (res = check_frame_header(p_header)) != 0 )
        return res;

#if LFM_DEBUG
    static uint32_t i = 0;
    dbg_frame_sn[(i++) % 10] = p_header->frame_sn;
#endif    

    if (prev_frame_sn != p_header->frame_sn)
    {
        prev_frame_sn = p_header->frame_sn;        
    }
    else   
    {
        return 0;
    }
	indev_set_dev_input_state(1);
    switch (p_header->event)
    {
        case LFM_EVENT_DATA_SYNC:
        case LFM_EVENT_BLE_RESET:
        case LFM_EVENT_PAGE_SYNC:
        case LFM_EVENT_CRMX_UNPAIR:
        case LFM_EVENT_FACTORY_RESET:
		case LFM_EVENT_POWER:
		case LFM_EVENT_ELECTRIC_MOVE:
        {
            if (s_lfm_core.core_ops->event_notify_cb)
            {
				s_lfm_core.core_ops->event_notify_cb((enum lfm_event)p_header->event, &p_data[sizeof(struct lfm_frame_header)]);
            }
        }
        break;
        case LFM_EVENT_UPGRADE:          
        {
            upgrade_process(p_data + sizeof(struct lfm_frame_header));
        }   
        break;
        default: break;
    }

	return 0;
}


/**
 * @brief 更新处理
 * 
 * @param data 更新数据包
 */
static void upgrade_process(uint8_t* data)
{
    struct lfm_upgrade_header* p_header = (struct lfm_upgrade_header*)data;
    uint32_t cur_prog_size = 0;         
	int res = 0;
	
    if (!s_lfm_core.upgrade.ops)
        return;
    
    switch (p_header->step)
    {
        case LFM_UPGRADE_STEP_START:
        {
			s_lfm_core.upgrade.next_addr = 0;
            struct lfm_firmware_info* firmware_bin = (struct lfm_firmware_info*)p_header->data; 
            int8_t err = LFM_ERR_OK;
            
            /* 校验固件回调函数必须存在，否则返回内部错误. */
            if (!s_lfm_core.upgrade.ops->check_firmware)
            {
                s_lfm_core.upgrade.err = LFM_ERR_INTERNEL;
                break;
            }
			
			/* 初始化升级相关结构体， 置当前升级状态为开始状态 */
            upgrade_core_init(&s_lfm_core, firmware_bin);      
            s_lfm_core.upgrade.type = p_header->type;
            s_lfm_core.upgrade.state = LFM_UPGRADE_STATE_START;
            /* 校验固件版本信息 */
            err = s_lfm_core.upgrade.ops->check_firmware(p_header->type, firmware_bin);
            if (err != LFM_ERR_OK)
            {
                s_lfm_core.upgrade.err = err;
                break;
            }

            /* 执行升级开始回调函数 */
            if (s_lfm_core.upgrade.ops->start)
            {
                res = s_lfm_core.upgrade.ops->start(&s_lfm_core, p_header, &cur_prog_size);     
				if(res != 0)
					s_lfm_core.upgrade.err = LFM_ERR_COMMUNICATE_FAILED;
            }

            s_lfm_core.upgrade.next_addr += cur_prog_size;   
        }
        break;
        case LFM_UPGRADE_STEP_DATA:
        {           
			/* 当前地址和下一个地址不相等, 报错 */   
            if (p_header->prog_addr == s_lfm_core.upgrade.next_addr)
            {
                if (s_lfm_core.upgrade.ops->data)
                {
                    res = s_lfm_core.upgrade.ops->data(&s_lfm_core, p_header, &cur_prog_size);
					if(res != 0)
						s_lfm_core.upgrade.err = LFM_ERR_COMMUNICATE_FAILED;
                }

                /* 计算下一次写入的地址,以及百分比 */
                s_lfm_core.upgrade.next_addr    =  s_lfm_core.upgrade.next_addr + cur_prog_size;      
                s_lfm_core.upgrade.prog_size    =  s_lfm_core.upgrade.prog_size + cur_prog_size;  
				if(s_lfm_core.upgrade.type == 0 || s_lfm_core.upgrade.type == 2)
					s_lfm_core.upgrade.prog_percent =  s_lfm_core.upgrade.prog_size * 100 / s_lfm_core.upgrade.firmware_bin.FirmwareSize; 
				else
					s_lfm_core.upgrade.prog_percent =  (s_lfm_core.upgrade.next_addr * 128) * 100 / s_lfm_core.upgrade.firmware_bin.FirmwareSize; 
			}
            else  
            {
                s_lfm_core.upgrade.err = LFM_ERR_COMMUNICATE_FAILED;
                break;
            }                
        }
        break;
        case LFM_UPGRADE_STEP_END:
        {
            if (p_header->error != 0) 
            {
                s_lfm_core.upgrade.err = LFM_ERR_COMMUNICATE_FAILED;
                break;
            }
            else if (s_lfm_core.upgrade.err != 0)
            {
                s_lfm_core.upgrade.err = LFM_ERR_INTERNEL;
                break;
            }
            
            /* 升级完成回调函数. */
            s_lfm_core.upgrade.state = LFM_UPGRADE_STATE_END;
            if (s_lfm_core.upgrade.ops->end)
            {
                s_lfm_core.upgrade.ops->end(&s_lfm_core, p_header);
            }            
        }
        break;
        default: break;
    }
}

/**
 * @brief 初始化升级结构体
 *  
 * @param core  初始化core
 * @param firmware_bin 升级文件信息头
 */
static void upgrade_core_init(struct lfm_core* core, struct lfm_firmware_info* firmware_bin)
{
    core->upgrade.crc32 = 0;
    core->upgrade.err = LFM_ERR_OK;
    core->upgrade.next_addr = 0;
    core->upgrade.prog_percent = 0;
    core->upgrade.prog_size = 0;
    core->upgrade.state = LFM_UPGRADE_STATE_IDLE;
    memcpy(&core->upgrade.firmware_bin, firmware_bin, sizeof(struct lfm_firmware_info));
}

/**
 * @brief 封装普通数据主从模式包
 * 
 * @param p_data 
 * @param e 
 * @return uint32_t 
 */
static uint32_t event_packet_wrap(uint8_t* p_data, enum lfm_event e)
{
    struct lfm_comm_setting* p_setting = (struct lfm_comm_setting*)(p_data + sizeof(struct lfm_frame_header));    
	
    init_frame_header((struct lfm_frame_header*)p_data, e);

    /* 更新常用设置结构体 */
    if (s_lfm_core.core_ops->refresh_before_send) 
    {
        s_lfm_core.core_ops->refresh_before_send(e, (struct lfm_comm_setting*)p_setting);
    }

    if (e == LFM_EVENT_DATA_SYNC)
    {
        return sizeof(struct lfm_frame_header) + sizeof(struct lfm_comm_setting) + p_setting->data_size;
    }
    else  
    {
        p_setting->data_type = LFM_DATA_TYPE_NONE;
        p_setting->data_size = 0;        
        return sizeof(struct lfm_frame_header) + sizeof(struct lfm_comm_setting);
    }
	
}

/**
 * @brief 封装主从模式升级数据包
 * 
 * @param p_des_buff        存储最终数据的指针
 * @param p_upgrade_header  升级头部指针
 * @param p_upgrade_data    升级数据
 */
static void upgrade_packet_wrap(uint8_t* p_des_buff, const struct lfm_upgrade_header* p_upgrade_header, const uint8_t* p_upgrade_data, uint32_t upgrade_uuid)
{
    uint8_t* p_next_pos = p_des_buff;
    init_frame_upgrade_header((struct lfm_frame_header*)p_next_pos, LFM_EVENT_UPGRADE, upgrade_uuid);

    p_next_pos = p_des_buff + sizeof(struct lfm_frame_header);
    memcpy(p_next_pos, p_upgrade_header, sizeof(struct lfm_upgrade_header));
    
    if (p_upgrade_data != NULL)
    {
        p_next_pos = p_des_buff + sizeof(struct lfm_frame_header) + sizeof(struct lfm_upgrade_header);
        memcpy(p_next_pos, p_upgrade_data, p_upgrade_header->data_len);
    }        
}

/**
 * @brief 升级数据包获取长度
 * 
 * @param buff          数据源
 * @return uint16_t     升级数据包长度
 */
static uint16_t upgrade_packet_get_length(uint8_t* buff)
{
    if (buff == NULL)
        return 0;

    const struct lfm_upgrade_header* p_header = (const struct lfm_upgrade_header*)(buff + sizeof(struct lfm_frame_header));
    uint16_t len = p_header->data_len + sizeof(struct lfm_frame_header) + sizeof(struct lfm_upgrade_header);
    if (len < MAX_PACKET_LEN)
        return len;

    return 0;
}

/**
 * @brief 检查帧头是否正确
 * 
 * @param p_header 帧头
 * @return int     错误码
 */
static int check_frame_header(struct lfm_frame_header* p_header)
{
	uint32_t uuid;
	
    if (p_header->soft_ver != SF_VERSION)
        return -1;

    if (p_header->hard_ver != HW_VERSION)
        return -2;
    
    if (p_header->protocol_ver != PROTOCOL_VERSION)
        return -3;
    
    if (p_header->lamp_type != LAMP_TYPE)
        return -4;
	
	if (0 != strncmp(p_header->uuid, MACHINE_UUID, 5))
	{
		uuid = (p_header->uuid[3] << 24) | (p_header->uuid[2] << 16) | (p_header->uuid[1] << 8) | (p_header->uuid[0]);
		
		if(lfm_batch_upgrade_type_check(uuid) > FIRMWARE_TYPE_ELE_FRNSEL)
		{
			return -5;
		}
	}
	
    return 0;
}

/**
 * @brief 广播主从模式数据包
 * 
 * @param data      主从模式数据指针
 * @param len       主从模式数据长度
 */
static void broadcast_data(uint8_t* data, uint16_t len)
{
    if (s_lfm_core.device[s_lfm_core.cur_dev_type] == NULL)
    {
        return ;
    }

    s_lfm_core.device[s_lfm_core.cur_dev_type]->write(data, len);
    s_lfm_core.cur_frame_number += 1;
}

/**
 * @brief 计算校验和
 * 
 * @param buff 待校验数据指针
 * @param size 大小
 * @return uint8_t 校验值
 */
static uint8_t check_sum_calc(const uint8_t* buff, uint16_t size)
{
    uint8_t check_sum = 0;
    while(size--)
    {
        check_sum += *buff++;
    }
    return check_sum;
}

/**
 * @brief 初始化升级包帧头
 * 
 * @param p_header 帧头指针
 * @param e 事件
 */
static void init_frame_upgrade_header(struct lfm_frame_header* p_header, enum lfm_event e, uint32_t uuid)
{
    p_header->event = e;
    p_header->hard_ver = HW_VERSION;
    p_header->soft_ver = SF_VERSION;
    p_header->protocol_ver = PROTOCOL_VERSION;   
	p_header->lamp_type = LAMP_TYPE;    
    p_header->frame_sn = s_lfm_core.cur_frame_number;
	p_header->uuid[0] = uuid & 0x000000ff;
	p_header->uuid[1] = (uuid & 0x0000ff00) >> 8;
	p_header->uuid[2] = (uuid & 0x00ff0000) >> 16;
	p_header->uuid[3] = (uuid & 0xff000000) >> 24;
	p_header->uuid[4] = 0;
	p_header->uuid[5] = 0;
}

/**
 * @brief 初始化帧头
 * 
 * @param p_header 主从模式帧头指针
 * @param e 事件
 */
static void init_frame_header(struct lfm_frame_header* p_header, enum lfm_event e)
{
    p_header->event = e;
    p_header->hard_ver = HW_VERSION;
    p_header->soft_ver = SF_VERSION;
    p_header->protocol_ver = PROTOCOL_VERSION;
    p_header->lamp_type = LAMP_TYPE;    
    p_header->frame_sn = s_lfm_core.cur_frame_number;

    for (uint8_t i = 0; i < 5; i++)
    {
        p_header->uuid[i] = MACHINE_UUID[i];
    }
    
    p_header->uuid[5] = '\0';
}
