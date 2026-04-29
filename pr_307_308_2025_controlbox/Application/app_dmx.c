#include "app_dmx.h"
#include "perf_counter.h"
#include <string.h>
#include <stdlib.h>
#include "dmx_profile_analysis.h"
#include "app_data_center.h"
#include "os_event.h"
#include "project_config.h"
#include "dev_crmx.h"
#include "page.h"
#include "page_manager.h"
#include "hal_gpio.h"
#include "app_rdm.h"
#include "app_gui.h"
#include "user.h"
#include "app_basic_data.h"
#include "ui_data.h"
#include "l_config.h"
#include "lfm_core.h"
#include "app_light.h"
#include "app_ble_port.h"
#include "app_err_detect.h"
#include "app_eth.h"

#define DMX_UNIT_TOTAL		3

#define DMX_RES_P1_Port   HAL_GPIOI
#define DMX_RES_P1_PIN    HAL_PIN_5
#define DMX_RES_P2_Port   HAL_GPIOI
#define DMX_RES_P2_PIN    HAL_PIN_4
#define DMX_RES_P3_Port   HAL_GPIOI
#define DMX_RES_P3_PIN    HAL_PIN_12
#define DMX_RES_P4_Port   HAL_GPIOE
#define DMX_RES_P4_PIN    HAL_PIN_1

const osThreadAttr_t g_dmx_thread_attr = { 
    .name = "app dmx", 
    .priority = osPriorityAboveNormal2, 
    .stack_size = 1024 * 4
};

enum dmx_res_type
{
    DMX_RES_120_TYPE ,
    DMX_RES_60_TYPE ,
    DMX_RES_40_TYPE,
    DMX_RES_30_TYPE ,
};

enum dmx_sta {
    DMX_STA_DISCONNECTED = 0,
    DMX_STA_CONNECTING,
    DMX_STA_CONNECTED,
};

enum dmx_event_type {
    EVENT_WIRED_RDM_UPDATE = 1 << 0,
    EVENT_WIRED_DMX_UPDATE  = 1 << 1,
    EVENT_WIRED_DMX_CONNECTED  = 1 << 2,
    EVENT_WIRED_DMX_DISCONNECTED  = 1 << 3,
    EVENT_WIRED_DMX_TRANSMIT  = 1 << 4,
    EVENT_WIRELESS_RDM_UPDATE = 1 << 5,
    EVENT_WIRELESS_DMX_UPDATE  = 1 << 6,
    EVENT_WIRELESS_DMX_CONNECTED  = 1 << 7,
    EVENT_WIRELESS_DMX_DISCONNECTED  = 1 << 8,
    EVENT_ETH_RDM_UPDATE = 1 << 9,
    EVENT_ETH_DMX_UPDATE  = 1 << 10,
    EVENT_ETH_DMX_CONNECTED  = 1 << 11,
    EVENT_ETH_DMX_DISCONNECTED  = 1 << 12,
};

//User needs to modify
enum dmx_profile_type
{
#if (PROJECT_TYPE == 308)    
    DMX_PROFILE_8BIT_CCT = 0,
    DMX_PROFILE_8BIT_FX,
    DMX_PROFILE_8BIT_LIGHTING_AND_FX,
    DMX_PROFILE_16BIT_CCT,
    DMX_PROFILE_16BIT_FX,
    DMX_PROFILE_16BIT_LIGHTING_AND_FX,    
    DMX_PROFILE_MAX_NUM,
#elif (PROJECT_TYPE == 307)    
	DMX_PROFILE_8BIT_CCT_RGB = 0,
    DMX_PROFILE_8BIT_CCT,
    DMX_PROFILE_8BIT_CCT_HSI,
    DMX_PROFILE_8BIT_RGB,
    DMX_PROFILE_8BIT_HSI, 
	DMX_PROFILE_8BIT_GEL,
    DMX_PROFILE_8BIT_XY,
    DMX_PROFILE_8BIT_SOURCE,
    DMX_PROFILE_8BIT_FX,
	DMX_PROFILE_8BIT_RGBWW,
    DMX_PROFILE_8BIT_ULTIMATE,
	DMX_PROFILE_16BIT_CCT_RGB,
    DMX_PROFILE_16BIT_CCT,
    DMX_PROFILE_16BIT_CCT_HSI,
    DMX_PROFILE_16BIT_RGB,
    DMX_PROFILE_16BIT_HSI,
	DMX_PROFILE_16BIT_GEL,
    DMX_PROFILE_16BIT_XY,
    DMX_PROFILE_16BIT_SOURCE,
    DMX_PROFILE_16BIT_FX,
	 DMX_PROFILE_16BIT_RGBWW,
    DMX_PROFILE_16BIT_ULTIMATE,
    
    DMX_PROFILE_MAX_NUM,  
#endif   
};

struct dev_dmx {
    enum dmx_sta sta;
    uint16_t console_length;
    uint32_t lost_tick;
    uint8_t  pre_rx_cnt;
    uint16_t pre_rx_size[10];
    uint8_t* profile_buf;
    uint16_t rdm_buffer_size;
    uint8_t  rdm_buff[257];
};

struct dmx_config
{
    uint16_t dmx_addr;
    uint16_t profile_size;
    uint16_t transmit_size;
    uint8_t  profile_index;
    uint8_t  dmx_loss_behav;
    uint8_t  temination_on;
    uint32_t fade_time;  //DMX缓变时间
    uint8_t  transmit_buff[514];
};

struct dmx_usart_rec 
{
    uint16_t length;
    uint8_t data[640];
};

static void dmx_lost_timer_cb(void *argument);
static void app_dmx_config_data_update(uint32_t event_type, uint32_t event_value);
static void dmx_data_protocol(uint8_t* buff);
static void dmx_termin_res_init(void);
static void dmx_termin_res_sel(enum dmx_res_type val, uint8_t pull_need);
static void app_dmx_local_data_recover(struct dmx_config*  config_data);
static void dmx_extend_func_ctrl(void);
static void dmx_uart_config(void);
static uint8_t check_sum_calc(const uint8_t* buff, uint16_t size);
static void dmx_uast_reset(void);
static void dmx_event_user_handle(enum dmx_event_type event, uint8_t* buff, uint16_t size);
static LumenRadioPinState_t crms_irq_read_cb(uint8_t id);
static void crmx_delay_cb(uint8_t id, uint8_t count);
static void crmx_nss_write_cb(uint8_t id, LumenRadioPinState_t status);
static uint8_t crmx_data_read_write_cb(uint8_t id, uint8_t data);
static void app_crmx_init(void);
static void app_crmx_power_cb(uint32_t event_type, uint32_t event_value);
static void crmx_sttatus_handle(uint8_t state);
static void app_crmx_stutas_handle(void);
static void app_crmx_set_name_funtion(void);
static void app_rdm_identy_set(uint8_t flag);
static void app_rdm_set_dmxaddr(uint16_t addr);
static uint16_t app_rdm_get_dmxaddr(void);
static void app_rdm_set_dmx_profile(uint16_t profile);
static uint16_t app_rdm_get_dmx_profile(void);
static void crmx_uart_config(void);
static void rdm_task_entry(void *arg);
static void dmx_rx_timeout_int_cb(enum hal_uart unit, uint8_t *p_data, uint32_t size);
static void dmx_rx_err_int_cb(enum hal_uart unit, uint8_t *p_data, uint32_t size);
static uint8_t dmx_profile_data_cmp(uint8_t* profile, uint8_t* dmx_data, uint16_t profile_size, uint16_t console_length , uint16_t dmx_addr);
static void dmx_profile_data_copy(uint8_t* profile, uint8_t* dmx_data, uint16_t profile_size, uint16_t console_length , uint16_t dmx_addr);

static void wired_dmx_tx_complete_irq_cb(void);
static void wired_dmx_rx_err_int_cb(void);
static void wired_dmx_rx_timeout_int_cb(void);
#if WIRELESS_DMX_USE 
static void wireless_dmx_rx_err_int_cb(void);
static void wireless_dmx_rx_timeout_int_cb(void);
static void wireless_dmx_tx_complete_irq_cb(void);
#endif

static enum dmx_event_type event_group[DMX_UNIT_TOTAL][4] = {
    {EVENT_WIRED_RDM_UPDATE, EVENT_WIRED_DMX_UPDATE, EVENT_WIRED_DMX_CONNECTED, EVENT_WIRED_DMX_DISCONNECTED},
#if WIRELESS_DMX_USE  
    {EVENT_WIRELESS_RDM_UPDATE, EVENT_WIRELESS_DMX_UPDATE, EVENT_WIRELESS_DMX_CONNECTED, EVENT_WIRELESS_DMX_DISCONNECTED},    
#endif   
#if ETH_DMX_USE  
    {NULL, EVENT_ETH_DMX_UPDATE, EVENT_ETH_DMX_CONNECTED, EVENT_ETH_DMX_DISCONNECTED},    
#endif      
}; 
static struct dev_dmx dmx[DMX_UNIT_TOTAL];
static struct sys_info_dmx s_dmx_link_state;
static struct dmx_config config;
static osTimerId_t dmx_timer_id;
static osSemaphoreId_t tx_semaphore_id[DMX_UNIT_TOTAL];
static osEventFlagsId_t dmx_event_id; 
uint32_t event_cnt[9];
static struct dmx_usart_rec dmx_rx_pack[DMX_UNIT_TOTAL] = {0};
static struct sys_info_crmx crmx_sta;
static uint8_t crmx_on = 0;
osThreadId_t app_dmx_thread_task_id;
static uint8_t s_lumenradio_name_flag;
static struct db_dmx dmx_protocol;
static struct db_dmx s_last_dmx_protocol;
static osMessageQueueId_t s_rdm_queuid;
static const osThreadAttr_t s_rdm_task_attr = {
  .name = "rdm_task",
  .priority = (osPriority_t) osPriorityAboveNormal,
  .stack_size = 256 * 4
};

uint32_t dmx_task_static_size;

void app_dmx_thread_entrance(void *argument)
{
    uint16_t delay_cnt = 0;
	uint8_t crmx_flag = 0;
	uint8_t  hsmode ;
	uint8_t dmx_send_count = 0;
	
    init_RDM();
    app_dmx_local_data_recover(&config);
    dmx_termin_res_init();
    dmx_uart_config();
    tx_semaphore_id[HAL_UART_DMX] = osSemaphoreNew (1, 0, NULL);
#if WIRELESS_DMX_USE 
	crmx_uart_config();
    tx_semaphore_id[HAL_UART_CRMX] = osSemaphoreNew (1, 0, NULL);
    app_crmx_init();
#endif 
    dmx_event_id = osEventFlagsNew(NULL);
	s_rdm_queuid = osMessageQueueNew(2, 512, NULL);
    dmx_timer_id = osTimerNew(dmx_lost_timer_cb, osTimerPeriodic, NULL, NULL);
    osTimerStart(dmx_timer_id, 1);
    /* 根据实际情况设置 */
    app_dmx_set_profile_size(514); //testing
    os_ev_subscribe_event(MAIN_EV_DATA, EV_DATA_CONFIG_CHANGE | EV_DATA_FACTORY_RESET, app_dmx_config_data_update);  
    os_ev_subscribe_event(MAIN_EV_DMX, EV_CRMX_POWER|EV_CRMX_UNLINK, app_crmx_power_cb);  
    
    g_pRdmIdentifyCallback = app_rdm_identy_set;
    g_pRdmSetStartAddrCallback = app_rdm_set_dmxaddr;
    g_pRdmGetStartAddrCallback = app_rdm_get_dmxaddr;
    
    g_pRdmSetProfileCallback = app_rdm_set_dmx_profile;
    g_pRdmGetProfileCallback = app_rdm_get_dmx_profile;
	
	leader_follwer_mode_init();
	osThreadNew(rdm_task_entry, NULL, &s_rdm_task_attr);
	
    for(;;)
    {
		dmx_uast_reset();
	
		data_center_read_config_data(SYS_CONFIG_HIGH_SPEED_MODE, &hsmode);
		if(hsmode)
		{
			if(ui_get_high_speed_switch() == 2)
			{
				hsmode = 0;
				ui_set_hs_mode_state(0);
				data_center_write_config_data(SYS_CONFIG_HIGH_SPEED_MODE, &hsmode);	
			}
		}
        if(delay_cnt++ > 50)
        {
			delay_cnt = 0;
			app_crmx_stutas_handle();
            dmx_extend_func_ctrl();   //DMX扩展通道控制
        }
		
		if(2 == app_dmx_lumenradio_name_flag_get())
		{
			app_crmx_set_name_funtion();
		}
        os_ev_pull_event(MAIN_EV_DATA | MAIN_EV_DMX);
        uint32_t get_event = osEventFlagsGet(dmx_event_id);
        if(get_event & EVENT_WIRED_RDM_UPDATE)
        {
            osEventFlagsClear(dmx_event_id, EVENT_WIRED_RDM_UPDATE);
            dmx_event_user_handle(EVENT_WIRED_RDM_UPDATE, dmx[HAL_UART_DMX].rdm_buff, dmx[HAL_UART_DMX].rdm_buffer_size);
        }
        if(get_event & EVENT_WIRED_DMX_UPDATE)
        {
            osEventFlagsClear(dmx_event_id, EVENT_WIRED_DMX_UPDATE);
            if(dmx[2].lost_tick == 0)  //以太网没有DMX数据再处理有线DMX数据
                dmx_event_user_handle(EVENT_WIRED_DMX_UPDATE, dmx[HAL_UART_DMX].profile_buf, config.profile_size);
        }
        if(get_event & EVENT_WIRED_DMX_CONNECTED)
        {
            osEventFlagsClear(dmx_event_id, EVENT_WIRED_DMX_CONNECTED);
            if(dmx[2].lost_tick == 0)  //以太网没有DMX数据再处理有线DMX数据
                dmx_event_user_handle(EVENT_WIRED_DMX_CONNECTED, dmx[HAL_UART_DMX].profile_buf, config.profile_size);
        }
        if(get_event & EVENT_WIRED_DMX_DISCONNECTED)
        {
			hal_dmx_write_ctrl_pin(HAL_UART_DMX, 0);
            osEventFlagsClear(dmx_event_id, EVENT_WIRED_DMX_DISCONNECTED);
            if(dmx[2].lost_tick == 0)  //以太网没有DMX数据再处理有线DMX数据
                dmx_event_user_handle(EVENT_WIRED_DMX_DISCONNECTED, dmx[HAL_UART_DMX].profile_buf, config.profile_size);
        }
        if(get_event & EVENT_WIRED_DMX_TRANSMIT)
        {
            osEventFlagsClear(dmx_event_id, EVENT_WIRED_DMX_TRANSMIT);
            dmx_event_user_handle(EVENT_WIRED_DMX_TRANSMIT, config.transmit_buff, config.transmit_size);
        }
#if WIRELESS_DMX_USE  
		if(crmx_on == 1)
		{
			if(get_event & EVENT_WIRELESS_RDM_UPDATE)
			{
				osEventFlagsClear(dmx_event_id, EVENT_WIRELESS_RDM_UPDATE);
				dmx_event_user_handle(EVENT_WIRELESS_RDM_UPDATE, dmx[HAL_UART_CRMX].rdm_buff, dmx[HAL_UART_CRMX].rdm_buffer_size);
			}
			if(get_event & EVENT_WIRELESS_DMX_UPDATE)
			{
				crmx_flag = 1;
				osEventFlagsClear(dmx_event_id, EVENT_WIRELESS_DMX_UPDATE);
				if(dmx[0].lost_tick == 0 && dmx[2].lost_tick == 0)  //如果当前没有有线DMX、以太网数据则转发DMX数据 //有线和以太网没有DMX数据再处理无线DMX数据
					dmx_event_user_handle(EVENT_WIRELESS_DMX_UPDATE, dmx[HAL_UART_CRMX].profile_buf, config.profile_size);
			}
			if(get_event & EVENT_WIRELESS_DMX_CONNECTED)
			{
				crmx_flag = 1;
				osEventFlagsClear(dmx_event_id, EVENT_WIRELESS_DMX_CONNECTED);
				if(dmx[0].lost_tick == 0 && dmx[2].lost_tick == 0)  //如果当前没有有线DMX、以太网数据则转发DMX数据 //有线和以太网没有DMX数据再处理无线DMX数据
					dmx_event_user_handle(EVENT_WIRELESS_DMX_CONNECTED, dmx[HAL_UART_CRMX].profile_buf, config.profile_size);
			}
			if(get_event & EVENT_WIRELESS_DMX_DISCONNECTED)
			{
				hal_dmx_write_ctrl_pin(HAL_UART_DMX, 0);
				osEventFlagsClear(dmx_event_id, EVENT_WIRELESS_DMX_DISCONNECTED);
				if(dmx[0].lost_tick == 0 && dmx[2].lost_tick == 0)  //如果当前没有有线DMX、以太网数据则转发DMX数据 //有线和以太网没有DMX数据再处理无线DMX数据
					dmx_event_user_handle(EVENT_WIRELESS_DMX_DISCONNECTED, dmx[HAL_UART_CRMX].profile_buf, config.profile_size);
			}
		}
		else
		{
			error_functionArray[lumenridio_ec_009](0);
			if(crmx_flag == 1)
			{
				crmx_flag = 0;
				s_dmx_link_state.wireless_linked = 0;
				crmx_sta.pair_state = 0;
				data_center_write_sys_info(SYS_INFO_CRMX, &crmx_sta);
				data_center_write_sys_info(SYS_INFO_DMX, &s_dmx_link_state); 
				memset(dmx[HAL_UART_CRMX].profile_buf, 0, 512);
				if(dmx[0].lost_tick == 0 && dmx[2].lost_tick == 0)
				{
					os_ev_publish_event(MAIN_EV_DMX, EV_DMX_DISCONNECTED, 0);
					ui_set_high_speed_switch(0)	;				
				}
			}		
		}
#endif 
#if ETH_DMX_USE
        if(get_event & EVENT_ETH_DMX_UPDATE)
        {
            osEventFlagsClear(dmx_event_id, EVENT_ETH_DMX_UPDATE);
            dmx_event_user_handle(EVENT_ETH_DMX_UPDATE, dmx[DMX_UNIT_TOTAL-1].profile_buf, config.profile_size);
        }
        if(get_event & EVENT_ETH_DMX_CONNECTED)
        {
            osEventFlagsClear(dmx_event_id, EVENT_ETH_DMX_CONNECTED);
            dmx_event_user_handle(EVENT_ETH_DMX_CONNECTED, dmx[DMX_UNIT_TOTAL-1].profile_buf, config.profile_size);
        }
        if(get_event & EVENT_ETH_DMX_DISCONNECTED)
        {
			hal_dmx_write_ctrl_pin(HAL_UART_DMX, 0);
            osEventFlagsClear(dmx_event_id, EVENT_ETH_DMX_DISCONNECTED);
            dmx_event_user_handle(EVENT_ETH_DMX_DISCONNECTED, dmx[DMX_UNIT_TOTAL-1].profile_buf, config.profile_size);
        }
#endif      
		
		if(1 == app_eth_get_lwip_state())
		{
			if(dmx[2].lost_tick != 0 && dmx[0].lost_tick == 0 && dmx[1].lost_tick == 0 && dmx_send_count++ >= 2)
			{
				dmx_send_count = 0;
				app_dmx_eth_set_transmit();
			}
		}
		dmx_task_static_size = osThreadGetStackSpace(app_dmx_thread_task_id);
		
        osDelay(2);
    }
}

void app_dmx_set_profile_size(uint16_t size)
{
    osKernelLock();
    config.profile_size = size;
    dmx[HAL_UART_DMX].profile_buf = (uint8_t *)realloc(dmx[HAL_UART_DMX].profile_buf, size);
#if WIRELESS_DMX_USE
    dmx[HAL_UART_CRMX].profile_buf = (uint8_t *)realloc(dmx[HAL_UART_CRMX].profile_buf, size);
#endif 
#if ETH_DMX_USE
    dmx[DMX_UNIT_TOTAL - 1].profile_buf = (uint8_t *)realloc(dmx[DMX_UNIT_TOTAL - 1].profile_buf, size);
#endif     
    osKernelUnlock();
}

void app_dmx_eth_disconnet_handle(void)
{
	dmx[2].lost_tick = 0;
	dmx[2].sta = DMX_STA_DISCONNECTED;
	s_dmx_link_state.eth_linked = 0;
	data_center_write_sys_info(SYS_INFO_DMX, &s_dmx_link_state);
	memset(dmx_rx_pack[2].data, 0, sizeof(dmx_rx_pack[2].data));
	osEventFlagsSet(dmx_event_id, event_group[2][3]);
	memset(dmx_rx_pack[HAL_UART_DMX].data, 0, sizeof(dmx_rx_pack[HAL_UART_DMX].data));
	hal_uart_receive_dma_channel_stop(HAL_UART_DMX);
	hal_uart_receive_dma_start(HAL_UART_DMX, dmx_rx_pack[HAL_UART_DMX].data, sizeof(dmx_rx_pack[HAL_UART_DMX].data));
	hal_uart_receive_dma_channel_start(HAL_UART_DMX); 
}

void app_dmx_eth_set_transmit(void)
{
	if(s_dmx_link_state.wired_linked != 1)  //如果当前没有有线DMX数据则转发DMX数据
	{
		wired_dmx_generate_transmit_event(dmx_rx_pack[DMX_UNIT_TOTAL - 1].data, dmx_rx_pack[DMX_UNIT_TOTAL - 1].length);
	}
}

void wired_dmx_transmit_with_break(uint8_t* data, uint16_t size)
{
	osSemaphoreAcquire(tx_semaphore_id[HAL_UART_DMX], 0);
    //临界区代码 时序重要不能被打断
	int32_t sl = osKernelLock();
	hal_dmx_write_ctrl_pin(HAL_UART_DMX, 1);
    hal_dmx_write_usart_tx_pin(HAL_UART_DMX, 0);
    delay_us(166);
    int32_t su = osKernelUnlock();
    
    //DMA发送并等待完成信号量 可被打断
	hal_dmx_usart_enable(HAL_UART_DMX);
	hal_uart_transmit_dma_start(HAL_UART_DMX, data, size);
    osSemaphoreAcquire(tx_semaphore_id[HAL_UART_DMX], (size / 20) + 2);
    //临界区代码 时序重要不能被打断
	osKernelRestoreLock(su);
    delay_us(100);
    hal_dmx_write_ctrl_pin(HAL_UART_DMX, 0);
    osKernelRestoreLock(sl);
}

void wired_dmx_transmit_with_mab(uint8_t* data, uint16_t size)
{
	osSemaphoreAcquire(tx_semaphore_id[HAL_UART_DMX], 0);
    //临界区代码 时序重要不能被打断
    int32_t sl = osKernelLock();
    hal_dmx_write_ctrl_pin(HAL_UART_DMX, 1);
    delay_us(30);
    int32_t su = osKernelUnlock();
    
    //DMA发送并等待完成信号量 可被打断
    hal_uart_transmit_dma_start(HAL_UART_DMX, data, size);
    osSemaphoreAcquire(tx_semaphore_id[HAL_UART_DMX], (size / 20) + 2);
    //临界区代码 时序重要不能被打断
    osKernelRestoreLock(su);
	delay_us(200);
    hal_dmx_write_ctrl_pin(HAL_UART_DMX, 0);
    osKernelRestoreLock(sl);
}

bool app_dmx_get_connection_state(enum hal_uart unit)
{
    osKernelLock();
    bool res = dmx[unit].sta == DMX_STA_DISCONNECTED ? false : true;
    osKernelUnlock();
    return res;
}

void wired_dmx_generate_transmit_event(uint8_t* p_data, uint16_t size)
{
    size = size > 514 ? 514 : size;
    memcpy(config.transmit_buff, p_data, size);
    config.transmit_size = size;
    osEventFlagsSet(dmx_event_id, EVENT_WIRED_DMX_TRANSMIT);
}

void wired_dmx_transmit_data(uint8_t* data, uint16_t size)
{
	osSemaphoreAcquire(tx_semaphore_id[HAL_UART_DMX], 0);
     hal_dmx_write_ctrl_pin(HAL_UART_DMX, 1);
    //DMA发送并等待完成信号量 可被打断
    hal_uart_transmit_dma_start(HAL_UART_DMX, data, size);
    osSemaphoreAcquire(tx_semaphore_id[HAL_UART_DMX], (size / 20) + 2);
    
    //临界区代码 时序重要不能被打断
    int32_t sl = osKernelLock();
    delay_us(100);
    hal_dmx_write_ctrl_pin(HAL_UART_DMX, 0);
    int32_t su = osKernelUnlock();
}

#if WIRELESS_DMX_USE 
void wireless_dmx_transmit_with_break(uint8_t* data, uint16_t size)
{
	osSemaphoreAcquire(tx_semaphore_id[HAL_UART_CRMX], 0);
    //临界区代码 时序重要不能被打断
    int32_t sl = osKernelLock();
    hal_dmx_write_ctrl_pin(HAL_UART_CRMX, 1);
    hal_dmx_write_usart_tx_pin(HAL_UART_CRMX, 0);
    delay_us(176);
    int32_t su = osKernelUnlock();
    
    //DMA发送并等待完成信号量 可被打断
	hal_dmx_usart_enable(HAL_UART_CRMX);
    hal_uart_transmit_dma_start(HAL_UART_CRMX, data, size);
    osSemaphoreAcquire(tx_semaphore_id[HAL_UART_CRMX], (size / 20) + 2);
	
    //临界区代码 时序重要不能被打断
    osKernelRestoreLock(su);
    delay_us(150);
    hal_dmx_write_ctrl_pin(HAL_UART_CRMX, 0);
    osKernelRestoreLock(sl);
}
#endif
#if ETH_DMX_USE  
void app_dmx_eth_write_data(uint8_t *p_data, uint32_t size)
{
    static uint16_t dmx_addr_pre = 0;
    static uint8_t  dmx_profile_index_pre = 0;
    
    dmx[DMX_UNIT_TOTAL - 1].lost_tick = 5500;
    if(size < 7 || p_data == NULL)
    {
        return ;
    }
	
	if(s_dmx_link_state.wired_linked != 1)  //如果当前没有有线DMX数据则转发DMX数据
	{
		memset(dmx_rx_pack[DMX_UNIT_TOTAL - 1].data, 0, sizeof(dmx_rx_pack[DMX_UNIT_TOTAL - 1].data));
		memcpy(&(dmx_rx_pack[DMX_UNIT_TOTAL - 1].data[1]), p_data, size);
		dmx_rx_pack[DMX_UNIT_TOTAL - 1].length = size+1;
	}
    switch(dmx[DMX_UNIT_TOTAL - 1].sta)
    {
        case DMX_STA_DISCONNECTED:
            //断连状态偶尔干扰信号不进入连接流程
            if(dmx[DMX_UNIT_TOTAL - 1].pre_rx_cnt++ > 0)
            {
                dmx[DMX_UNIT_TOTAL - 1].pre_rx_cnt = 0;
                dmx[DMX_UNIT_TOTAL - 1].sta = DMX_STA_CONNECTING;
            }
        break;
        case DMX_STA_CONNECTING:
            dmx[DMX_UNIT_TOTAL - 1].pre_rx_size[dmx[DMX_UNIT_TOTAL - 1].pre_rx_cnt++] = size;
            //连续两包数据长度不一样，清零重新判断        
            if(dmx[DMX_UNIT_TOTAL - 1].pre_rx_cnt >= 2 && dmx[DMX_UNIT_TOTAL - 1].pre_rx_size[dmx[DMX_UNIT_TOTAL - 1].pre_rx_cnt - 2] != dmx[DMX_UNIT_TOTAL - 1].pre_rx_size[dmx[DMX_UNIT_TOTAL - 1].pre_rx_cnt - 1])
            {
                dmx[DMX_UNIT_TOTAL - 1].pre_rx_cnt = 0;
            }
            if(dmx[DMX_UNIT_TOTAL - 1].pre_rx_cnt == 3)
            {
                dmx[DMX_UNIT_TOTAL - 1].pre_rx_cnt = 0;
                dmx[DMX_UNIT_TOTAL - 1].sta = DMX_STA_CONNECTED;
                dmx[DMX_UNIT_TOTAL - 1].console_length = dmx[DMX_UNIT_TOTAL - 1].pre_rx_size[0] - 1;
                dmx_profile_data_copy(dmx[DMX_UNIT_TOTAL - 1].profile_buf, p_data, config.profile_size, dmx[DMX_UNIT_TOTAL - 1].console_length, config.dmx_addr-1);
                osEventFlagsSet(dmx_event_id, event_group[DMX_UNIT_TOTAL - 1][2]); // DMX连接事件
                
            }
        break;
        case DMX_STA_CONNECTED:
            if(size - 1 == dmx[DMX_UNIT_TOTAL - 1].console_length)
            {
                if(config.dmx_addr != dmx_addr_pre || config.profile_index != dmx_profile_index_pre || \
                    0 != dmx_profile_data_cmp(dmx[DMX_UNIT_TOTAL - 1].profile_buf, p_data, config.profile_size, dmx[DMX_UNIT_TOTAL - 1].console_length, config.dmx_addr-1))
                {
                    dmx_addr_pre = config.dmx_addr;
                    dmx_profile_index_pre = config.profile_index;
                    
                    dmx_profile_data_copy(dmx[DMX_UNIT_TOTAL - 1].profile_buf, p_data, config.profile_size, dmx[DMX_UNIT_TOTAL - 1].console_length, config.dmx_addr-1);
                    osEventFlagsSet(dmx_event_id, event_group[DMX_UNIT_TOTAL - 1][1]); // DMX更新事件
                }
            }
            else
            {
                //超过3包数据长度不对重新识别数据长度
                if(++dmx[DMX_UNIT_TOTAL - 1].pre_rx_cnt > 2)
                {
                    dmx[DMX_UNIT_TOTAL - 1].pre_rx_cnt = 0;
                    dmx[DMX_UNIT_TOTAL - 1].sta = DMX_STA_CONNECTING;
                }
            }
        break;
        default:break;
    }
}
#endif

void app_dmx_lumenradio_name_flag_set(uint8_t flag)
{
	s_lumenradio_name_flag = flag;
}	

uint8_t app_dmx_lumenradio_name_flag_get(void)
{
	return s_lumenradio_name_flag;
}

void app_dmx_last_data_clear(void)
{
	memset(&s_last_dmx_protocol, 0, sizeof(s_last_dmx_protocol));
}

static void rdm_task_entry(void *arg)
{
	static uint8_t rx_buff[512];
	
	for(;;)
	{
		if(osMessageQueueGet(s_rdm_queuid, rx_buff, NULL, osWaitForever) == osOK)
		{	
			Dispose_Recv_Rdm_Data(rx_buff);
		}	
	}
}

static void app_dmx_config_data_update(uint32_t event_type, uint32_t event_value)
{
    uint8_t type = 0;
    
    type = os_ev_get_event_data(event_type, event_value);
	if(event_value & EV_DATA_FACTORY_RESET)
	{
		data_center_read_config_data(SYS_CONFIG_DMX_PROFILE, &config.profile_index);
		data_center_read_config_data(SYS_CONFIG_DMX_LOSS_BEHAVIOR, &config.dmx_loss_behav);
		data_center_read_config_data(SYS_CONFIG_DMX_ADDR, &config.dmx_addr);
	}
	else if(event_value & EV_DATA_CONFIG_CHANGE)
	{
		switch(type)
		{
			case SYS_CONFIG_DMX_ADDR:
				data_center_read_config_data(SYS_CONFIG_DMX_ADDR, &config.dmx_addr);
			break;
			case SYS_CONFIG_DMX_LOSS_BEHAVIOR:
				data_center_read_config_data(SYS_CONFIG_DMX_LOSS_BEHAVIOR, &config.dmx_loss_behav);
			break;
			case SYS_CONFIG_DMX_PROFILE:
				data_center_read_config_data(SYS_CONFIG_DMX_PROFILE, &config.profile_index);
				//app_dmx_set_profile_size  可以在这里设置profilebuf
			break;
			case SYS_CONFIG_DMX_TEMINATION:
				data_center_read_config_data(SYS_CONFIG_DMX_TEMINATION, &config.temination_on);
				if(config.temination_on == 1)
				{
					dmx_termin_res_sel(DMX_RES_30_TYPE, 1); 
				}
				else
				{
					dmx_termin_res_sel(DMX_RES_30_TYPE, 0); 
				}
				//TODO
			break;
			default:break;
		}
	}
}

static void app_dmx_local_data_recover(struct dmx_config*  config_data)
{
    data_center_read_config_data(SYS_CONFIG_DMX_ADDR, &config_data->dmx_addr);
    data_center_read_config_data(SYS_CONFIG_DMX_LOSS_BEHAVIOR, &config_data->dmx_loss_behav);
    data_center_read_config_data(SYS_CONFIG_DMX_PROFILE, &config_data->profile_index);
    data_center_read_config_data(SYS_CONFIG_DMX_TEMINATION, &config_data->temination_on);
}

static uint8_t dmx_profile_data_cmp(uint8_t* profile, uint8_t* dmx_data, uint16_t profile_size, uint16_t console_length , uint16_t dmx_addr)
{
    uint8_t res = 0;
    
    if(dmx_addr + profile_size - 1 > console_length)
    {
        if(dmx_addr <= console_length)
        {
            res = memcmp(profile, &dmx_data[dmx_addr], console_length - dmx_addr + 1);
        }
    }
    else
    {
        res = memcmp(profile, &dmx_data[dmx_addr], profile_size);
    }
    
    return res;
}

static void dmx_profile_data_copy(uint8_t* profile, uint8_t* dmx_data, uint16_t profile_size, uint16_t console_length , uint16_t dmx_addr)
{
    
    if(dmx_addr + profile_size - 1 > console_length)
    {
        memset(profile, 0, profile_size);
        if(dmx_addr <= console_length)
        {
            memcpy(profile, &dmx_data[dmx_addr], console_length - dmx_addr + 1);
        }
    }
    else
    {
        memcpy(profile, &dmx_data[dmx_addr], profile_size);
    }
}

static void dmx_rx_err_int_cb(enum hal_uart unit, uint8_t *p_data, uint32_t size)
{
    static uint16_t dmx_addr_pre = 0;
    static uint8_t dmx_profile_index_pre = 0;
    static uint8_t refresh_ui;
	
	if(size < 30)
	{
		return;
	}
	dmx[unit].lost_tick = 500;
    if(p_data[0] != 0x00)
    {
        return ;
    }
    
    switch(dmx[unit].sta)
    {
        case DMX_STA_DISCONNECTED:
            //断连状态偶尔干扰信号不进入连接流程
            if(dmx[unit].pre_rx_cnt++ > 0)
            {
                dmx[unit].pre_rx_cnt = 0;
                dmx[unit].sta = DMX_STA_CONNECTING;
            }
            break;
        case DMX_STA_CONNECTING:
            dmx[unit].pre_rx_size[dmx[unit].pre_rx_cnt++] = size;
              
            if(dmx[unit].pre_rx_cnt >= 10)
            {
				for(uint8_t i = 0; i < 9; i++)
				{
					if(dmx[unit].pre_rx_size[i] != dmx[unit].pre_rx_size[i+1])//数据长度连续不一样，清零重新判断  
					{
						dmx[unit].pre_rx_cnt = 0;
						return;
					}
				}
				dmx[unit].sta = DMX_STA_CONNECTED;
                dmx[unit].console_length = dmx[unit].pre_rx_size[0] - 1;
                dmx_profile_data_copy(dmx[unit].profile_buf, p_data, config.profile_size, dmx[unit].console_length, config.dmx_addr);
                osEventFlagsSet(dmx_event_id, event_group[unit][2]); // DMX连接事件
            }
            break;
        case DMX_STA_CONNECTED:
            if(size - 1 == dmx[unit].console_length)
            {
				dmx[unit].pre_rx_cnt = 0;
				refresh_ui = 0;
                if(config.dmx_addr != dmx_addr_pre || config.profile_index != dmx_profile_index_pre || \
                    0 != dmx_profile_data_cmp(dmx[unit].profile_buf, p_data, config.profile_size, dmx[unit].console_length, config.dmx_addr))
                {
                    dmx_addr_pre = config.dmx_addr;
                    dmx_profile_index_pre = config.profile_index;
                    
                    dmx_profile_data_copy(dmx[unit].profile_buf, p_data, config.profile_size, dmx[unit].console_length, config.dmx_addr);
                    osEventFlagsSet(dmx_event_id, event_group[unit][1]); // DMX更新事件
                }
            }
            else
            {
                //超过5包数据长度不对重新识别数据长度
                if(++dmx[unit].pre_rx_cnt > 5)
                {
                    dmx[unit].pre_rx_cnt = 0;
                    dmx[unit].sta = DMX_STA_CONNECTING;
                }
            }
            break;
        default:break;
    }
}

static void dmx_rx_timeout_int_cb(enum hal_uart unit, uint8_t *p_data, uint32_t size)
{
    memcpy(dmx[unit].rdm_buff, p_data, p_data[2]);
	osMessageQueuePut(s_rdm_queuid, dmx[unit].rdm_buff, NULL, 0); 
}

static void wired_dmx_rx_err_int_cb(void)
{
    if (Set == hal_uart_get_status(HAL_UART_DMX, (HAL_USART_FLAG_FE)))
    {
		dmx_rx_pack[HAL_UART_DMX].length = sizeof(dmx_rx_pack[HAL_UART_DMX].data) - hal_uart_get_receive_dma_cnt(HAL_UART_DMX); 
        if(dmx_rx_pack[HAL_UART_DMX].data[0] == 0x00)
        {
			if(dmx_rx_pack[HAL_UART_DMX].length > 10)
				dmx_rx_err_int_cb(HAL_UART_DMX, dmx_rx_pack[HAL_UART_DMX].data, dmx_rx_pack[HAL_UART_DMX].length);
        }
		hal_uart_receive_dma_channel_stop(HAL_UART_DMX);
		hal_uart_receive_dma_start(HAL_UART_DMX, dmx_rx_pack[HAL_UART_DMX].data, sizeof(dmx_rx_pack[HAL_UART_DMX].data));
		hal_uart_receive_dma_channel_start(HAL_UART_DMX); 
    }
}
                
static void wired_dmx_rx_timeout_int_cb(void)
{
	uint8_t mode = 0;
	static uint32_t prev_frame_sn = 0xFFFFFFFF;
	
	dmx_rx_pack[HAL_UART_DMX].length = sizeof(dmx_rx_pack[HAL_UART_DMX].data) - hal_uart_get_receive_dma_cnt(HAL_UART_DMX); 
	if(dmx_rx_pack[HAL_UART_DMX].data[0] == 0xcc)
    {
        dmx_rx_timeout_int_cb(HAL_UART_DMX, dmx_rx_pack[HAL_UART_DMX].data, dmx_rx_pack[HAL_UART_DMX].length);
    }
    if(dmx_rx_pack[HAL_UART_DMX].data[0] == 0x55) {  //主从模式
		//发送给主从接收    
		//检查485协议帧的帧序是否和上一次的帧序不一样.
		if (lfm_485_proto_frame_sn_get(dmx_rx_pack[HAL_UART_DMX].data) != prev_frame_sn)
		{
			rs485_proto_header_t* p_receive_header = (rs485_proto_header_t*)dmx_rx_pack[HAL_UART_DMX].data;	
			if(p_receive_header->start != RS485_START_CODE)
				return;
			if(p_receive_header->msg_size > RS485_MESSAGE_MAX_SIZE)
				return;
			if(p_receive_header->check_sum != check_sum_calc((uint8_t*)&p_receive_header->header_size, (p_receive_header->msg_size-2)))
				return;
			if(0 != lfm_485_proto_frame_uuid_crc(&dmx_rx_pack[HAL_UART_DMX].data[15]))
				return;
			if((PAGE_WORK_MODE == screen_get_act_pid() && screen_get_act_spid() == SUB_ID_SYNC_UPDATAING))
				return;
			data_center_write_config_data_no_event(SYS_CONFIG_WORK_MODE, &mode);
			ui_set_work_mode(mode);
			prev_frame_sn = lfm_485_proto_frame_sn_get(dmx_rx_pack[HAL_UART_DMX].data);	
			osMessageQueuePut(lfm_core_get_rx_msg_queue(), dmx_rx_pack[HAL_UART_DMX].data, NULL, 0);        
		}
	}
	if(dmx_rx_pack[HAL_UART_DMX].data[0] != 0xcc && dmx_rx_pack[HAL_UART_DMX].data[0] != 0x00)
	{
		hal_uart_receive_dma_channel_stop(HAL_UART_DMX);
		hal_uart_receive_dma_start(HAL_UART_DMX, dmx_rx_pack[HAL_UART_DMX].data, sizeof(dmx_rx_pack[HAL_UART_DMX].data));
		hal_uart_receive_dma_channel_start(HAL_UART_DMX);
	}
}

static void wired_dmx_tx_complete_irq_cb(void)
{
    osSemaphoreRelease(tx_semaphore_id[HAL_UART_DMX]);
}

#if WIRELESS_DMX_USE 
static void wireless_dmx_rx_err_int_cb(void)
{
	uint8_t mode = 0;
	static uint32_t prev_frame_sn1 = 0xFFFFFFFF;
	if (Set == hal_uart_get_status(HAL_UART_CRMX, (HAL_USART_FLAG_FE | HAL_USART_FLAG_PE)))
    {
		dmx_rx_pack[HAL_UART_CRMX].length = sizeof(dmx_rx_pack[HAL_UART_CRMX].data) - hal_uart_get_receive_dma_cnt(HAL_UART_CRMX);
		if(dmx_rx_pack[HAL_UART_CRMX].data[0] == 0x00 && dmx_rx_pack[HAL_UART_CRMX].length > 10 && crmx_on != 0)
        {
			dmx_rx_err_int_cb(HAL_UART_CRMX, dmx_rx_pack[HAL_UART_CRMX].data, dmx_rx_pack[HAL_UART_CRMX].length);
			if(dmx_rx_pack[HAL_UART_CRMX].data[0] == 0 && dmx[0].lost_tick == 0 && dmx[2].lost_tick == 0 && dmx[1].sta == DMX_STA_CONNECTED)  //如果当前没有有线DMX、以太网数据则转发DMX数据
				wired_dmx_generate_transmit_event(dmx_rx_pack[HAL_UART_CRMX].data, dmx_rx_pack[HAL_UART_CRMX].length);
		}
		if(dmx_rx_pack[HAL_UART_CRMX].data[0] == 0x55 && crmx_on != 0 && dmx[2].lost_tick == 0 && dmx[0].lost_tick == 0) {  //主从模式
			//发送给主从接收    
			//检查485协议帧的帧序是否和上一次的帧序不一样.
			if (lfm_485_proto_frame_sn_get(dmx_rx_pack[HAL_UART_CRMX].data) != prev_frame_sn1)
			{
				rs485_proto_header_t* p_receive_header = (rs485_proto_header_t*)dmx_rx_pack[HAL_UART_CRMX].data;	
				if(p_receive_header->start != RS485_START_CODE)
					return;
				if(p_receive_header->msg_size > RS485_MESSAGE_MAX_SIZE)
					return;
				if(p_receive_header->check_sum != check_sum_calc((uint8_t*)&p_receive_header->header_size, (p_receive_header->msg_size-2)))
					return;
				if(0 != lfm_485_proto_frame_uuid_crc(&dmx_rx_pack[HAL_UART_CRMX].data[15]))
					return;
				if(PAGE_WORK_MODE == screen_get_act_pid() && screen_get_act_spid() == SUB_ID_SYNC_UPDATAING)
					return;
				data_center_write_config_data_no_event(SYS_CONFIG_WORK_MODE, &mode);
				ui_set_work_mode(mode);
				prev_frame_sn1 = lfm_485_proto_frame_sn_get(dmx_rx_pack[HAL_UART_CRMX].data);	
				osMessageQueuePut(lfm_core_get_rx_msg_queue(), dmx_rx_pack[HAL_UART_CRMX].data, NULL, 0);        
			}
		}
	}
	hal_uart_receive_dma_channel_stop(HAL_UART_CRMX);
	hal_uart_receive_dma_start(HAL_UART_CRMX, dmx_rx_pack[HAL_UART_CRMX].data, sizeof(dmx_rx_pack[HAL_UART_CRMX].data));
	hal_uart_receive_dma_channel_start(HAL_UART_CRMX); 
}

static void wireless_dmx_rx_timeout_int_cb(void)
{
	static uint32_t prev_frame_sn = 0xFFFFFFFF;
	uint8_t mode = 0;
	
    dmx_rx_pack[HAL_UART_CRMX].length = sizeof(dmx_rx_pack[HAL_UART_CRMX].data) - hal_uart_get_receive_dma_cnt(HAL_UART_CRMX);
	if(dmx_rx_pack[HAL_UART_CRMX].data[0] == 0x55) {  //主从模式
		//发送给主从接收    
		//检查485协议帧的帧序是否和上一次的帧序不一样.
		hal_uart_receive_dma_channel_stop(HAL_UART_DMX);
		hal_uart_receive_dma_start(HAL_UART_DMX, dmx_rx_pack[HAL_UART_DMX].data, sizeof(dmx_rx_pack[HAL_UART_DMX].data));
        hal_uart_receive_dma_channel_start(HAL_UART_DMX); 
		if (lfm_485_proto_frame_sn_get(dmx_rx_pack[HAL_UART_DMX].data) != prev_frame_sn)
		{
			rs485_proto_header_t* p_receive_header = (rs485_proto_header_t*)dmx_rx_pack[HAL_UART_DMX].data;	
			if(p_receive_header->start != RS485_START_CODE)
				return;
			if(p_receive_header->msg_size > RS485_MESSAGE_MAX_SIZE)
				return;
			if(p_receive_header->check_sum != check_sum_calc((uint8_t*)&p_receive_header->header_size, (p_receive_header->msg_size-2)))
				return;
			data_center_write_config_data_no_event(SYS_CONFIG_WORK_MODE, &mode);
			ui_set_work_mode(mode);
			prev_frame_sn = lfm_485_proto_frame_sn_get(dmx_rx_pack[HAL_UART_DMX].data);	
			osMessageQueuePut(lfm_core_get_rx_msg_queue(), dmx_rx_pack[HAL_UART_DMX].data, NULL, 0);        
		}
	}
	if(dmx_rx_pack[HAL_UART_DMX].data[0] != 0xcc && dmx_rx_pack[HAL_UART_DMX].data[0] != 0x00)
	{
		hal_uart_receive_dma_channel_stop(HAL_UART_DMX);
		hal_uart_receive_dma_start(HAL_UART_DMX, dmx_rx_pack[HAL_UART_DMX].data, sizeof(dmx_rx_pack[HAL_UART_DMX].data));
		hal_uart_receive_dma_channel_start(HAL_UART_DMX);
	}
//	if(crmx_on != 1)
//	{
//		return;
//	}
//	
//    dmx_rx_timeout_int_cb(HAL_UART_CRMX, dmx_rx_pack[HAL_UART_CRMX].data, dmx_rx_pack[HAL_UART_CRMX].length);
}

static void wireless_dmx_tx_complete_irq_cb(void)
{
    osSemaphoreRelease(tx_semaphore_id[HAL_UART_CRMX]);
}
#endif

static void dmx_lost_timer_cb(void *argument)
{
	static uint8_t eth_dmx_flag = 0;
	
    for(uint8_t unit = 0; unit < DMX_UNIT_TOTAL; unit++)
    {
        if(dmx[unit].lost_tick > 0)
        {
            dmx[unit].lost_tick--;
        }
        else
        {
            dmx[unit].pre_rx_cnt = 0;
			if(unit == 0 && dmx[0].sta != DMX_STA_DISCONNECTED)
			{
				dmx[0].sta = DMX_STA_DISCONNECTED;
				s_dmx_link_state.wired_linked = 0;
				memset(dmx_rx_pack[HAL_UART_DMX].data, 0, sizeof(dmx_rx_pack[HAL_UART_DMX].data));
				hal_uart_receive_dma_channel_stop(HAL_UART_DMX);
				hal_uart_receive_dma_start(HAL_UART_DMX, dmx_rx_pack[HAL_UART_DMX].data, sizeof(dmx_rx_pack[HAL_UART_DMX].data));
				hal_uart_receive_dma_channel_start(HAL_UART_DMX); 
				data_center_write_sys_info(SYS_INFO_DMX, &s_dmx_link_state);
				osEventFlagsSet(dmx_event_id, event_group[0][3]);
			}
			if(unit == 1 && dmx[1].sta != DMX_STA_DISCONNECTED)
			{
				dmx[1].sta = DMX_STA_DISCONNECTED;
				s_dmx_link_state.wireless_linked = 0;
				memset(dmx_rx_pack[HAL_UART_DMX].data, 0, sizeof(dmx_rx_pack[HAL_UART_DMX].data));
				hal_uart_receive_dma_channel_stop(HAL_UART_DMX);
				hal_uart_receive_dma_start(HAL_UART_DMX, dmx_rx_pack[HAL_UART_DMX].data, sizeof(dmx_rx_pack[HAL_UART_DMX].data));
				hal_uart_receive_dma_channel_start(HAL_UART_DMX); 
				memset(dmx_rx_pack[HAL_UART_CRMX].data, 0, sizeof(dmx_rx_pack[HAL_UART_CRMX].data));
				data_center_write_sys_info(SYS_INFO_DMX, &s_dmx_link_state);
				osEventFlagsSet(dmx_event_id, event_group[1][3]);
			}
        }
		if(dmx[2].lost_tick != 0)
			eth_dmx_flag = 1;
    }
	if(0 == app_eth_get_lwip_state())
	{
		if(eth_dmx_flag == 1)
		{
			eth_dmx_flag = 0;
			app_dmx_eth_disconnet_handle();
		}
	}
	else if(1 == app_eth_get_lwip_state())
	{
		if(dmx[2].lost_tick == 0 && dmx[0].lost_tick == 0 && dmx[1].lost_tick == 0 && eth_dmx_flag == 1)
		{
			eth_dmx_flag = 0;
			app_dmx_eth_disconnet_handle();
		}
	}
}

/**DMX 终端电阻**/
static void dmx_termin_res_init(void)
{
    hal_gpio_init(DMX_RES_P1_Port, DMX_RES_P1_PIN, HAL_PIN_MODE_OUT_PP);
    hal_gpio_init(DMX_RES_P2_Port, DMX_RES_P2_PIN, HAL_PIN_MODE_OUT_PP);
    hal_gpio_init(DMX_RES_P3_Port, DMX_RES_P3_PIN, HAL_PIN_MODE_OUT_PP);
    hal_gpio_init(DMX_RES_P4_Port, DMX_RES_P4_PIN, HAL_PIN_MODE_OUT_PP);
    
    hal_gpio_write_pin(DMX_RES_P1_Port, DMX_RES_P1_PIN, false);    
    hal_gpio_write_pin(DMX_RES_P2_Port, DMX_RES_P2_PIN, false); 
    hal_gpio_write_pin(DMX_RES_P3_Port, DMX_RES_P3_PIN, false); 
    hal_gpio_write_pin(DMX_RES_P4_Port, DMX_RES_P4_PIN, false); 
}


static void dmx_termin_res_sel(enum dmx_res_type val, uint8_t pull_need)
{
    if(pull_need == 1 && val == DMX_RES_30_TYPE)
        hal_gpio_write_pin(DMX_RES_P1_Port, DMX_RES_P1_PIN, true);
    else if(pull_need == 0 && val == DMX_RES_30_TYPE)
        hal_gpio_write_pin(DMX_RES_P1_Port, DMX_RES_P1_PIN, false); 
}

static void dmx_extend_func_ctrl(void)
{
    uint16_t event_val = 0;
    struct dmx_config_data* dmx_config = NULL;
    uint8_t dmx_extension = 0;
	struct sys_info_motor       motor_dmx_info;
	struct sys_info_accessories dmx_access_info;
	uint8_t limi = 0;
	int16_t data[2];
	int16_t data1[12][2];
	uint8_t limi1 = 0;
	uint8_t dmx_extension1[12] = {1};
	
    event_val = dmx_sys_config_refresh_event_get(&dmx_config);
    if(event_val != 0 && dmx_config != NULL)
    {
		data_center_read_config_data(SYS_CONFIG_DMX_EXTENSION, &dmx_extension);
		if(dmx_extension == 1 || dmx_extension == 3)
		{
			data_center_read_sys_info(SYS_INFO_MOTOR, &motor_dmx_info);
			if(event_val & DMX_ZOOM_REFRESH)
			{
				dmx_sys_config_event_clear(DMX_ZOOM_REFRESH);
				motor_dmx_info.fresnel_angle = dmx_config->zoom/100.0f;
				data_center_write_config_data_no_event(SYS_CONFIG_FRESNEL_ANGLE, &motor_dmx_info.fresnel_angle);
				data_center_write_sys_info(SYS_INFO_MOTOR, &motor_dmx_info);
				app_light_flags_set(LIGHT_RS485_FRESNEL_ANGLE_FLAG);
			}
			if(event_val & DMX_PAN_TILT_REFRESH)
			{
				dmx_sys_config_event_clear(DMX_PAN_TILT_REFRESH);
				data_center_read_sys_info(SYS_INFO_ACCESS, &dmx_access_info);
				if(dmx_access_info.fresnel_state == 1 || dmx_access_info.optical_20_access_state == 1 || 
				   dmx_access_info.optical_30_access_state == 1 || dmx_access_info.optical_50_access_state == 1)  // 装配我司电动附件
				{
					data_center_read_config_data(SYS_CONFIG_ANGLE_LIMIT, &limi);  //是否打开角度限制
					if(limi == 1)
					{
						data_center_read_config_data(SYS_CONFIG_YOKE_TILT_LIMIT, data);
						if(dmx_access_info.fresnel_state == 1 || dmx_access_info.optical_20_access_state == 1)
						{
							if(data[1] > 120)
								data[1] = 120;
							if(data[0] < -120)
								data[0] = -120;
						}
						else
						{
							if(data[1] > ANGLE_LIMIT_DEFAULT_MAX)
								data[1] = ANGLE_LIMIT_DEFAULT_MAX;
							if(data[0] < ANGLE_LIMIT_DEFAULT_MIN)
								data[0] = ANGLE_LIMIT_DEFAULT_MIN;
						}
						if(data[0] < ANGLE_LIMIT_MIN)
						{
							data[0] = (-(data[0]));
						}
						dmx_config->tilt = dmx_config->tilt-(data[1]*10);
					}
					else
					{
						if(dmx_access_info.fresnel_state == 1 || dmx_access_info.optical_20_access_state == 1)
						{
							dmx_config->tilt = dmx_config->tilt-1200;
						}
						else
							dmx_config->tilt = dmx_config->tilt-1350;
					}
				}
				else
				{
					data_center_read_config_data(SYS_CONFIG_NO_ANGLE_LIMIT, &dmx_extension1);  //是否打开角度限制
					data_center_read_config_data(SYS_CONFIG_NOT_ACCESS_SELECT, &limi1);
					if(limi1 > 10)
						limi1 = 10;
					if(dmx_extension1[limi1] == 1)
					{
						data_center_read_config_data(SYS_CONFIG_YOKE_NO_TILT_LIMIT, data1);
						dmx_config->tilt = dmx_config->tilt-((-data1[limi1][0])*10);
					}
					else
					{
						if(limi >= 8)
						{
							data_center_read_config_data(SYS_CONFIG_YOKE_NO_TILT_LIMIT, data1);
							dmx_config->tilt = dmx_config->tilt-((-data1[limi1][0])*10);
						}
						else
						{
							dmx_config->tilt = dmx_config->tilt-1350;
						}
					}
				}
				dmx_config->tilt /= 10.0f;
				data_center_write_config_data_no_event(SYS_CONFIG_YOKE_TILT, &dmx_config->tilt);
				motor_dmx_info.tilt_angle = dmx_config->tilt;
				data_center_write_sys_info(SYS_INFO_MOTOR, &motor_dmx_info);
				app_light_flags_set(LIGHT_RS485_YOKE_TILT_FLAG);
			}
			if(event_val & DMX_PAN_REFRESH)
			{
				dmx_sys_config_event_clear(DMX_PAN_REFRESH);
				dmx_config->pan /= 10.0f;
				data_center_write_config_data_no_event(SYS_CONFIG_YOKE_PAN, &dmx_config->pan);
				motor_dmx_info.pan_angle = dmx_config->pan;
				data_center_write_sys_info(SYS_INFO_MOTOR, &motor_dmx_info);
				app_light_flags_set(LIGHT_RS485_YOKE_PAN_FLAG);
			}
		}
		if(dmx_extension == 2 || dmx_extension == 3)
		{
			if(event_val & DMX_FAN_REFRESH)
			{
				dmx_sys_config_event_clear(DMX_FAN_REFRESH);
				data_center_write_config_data_no_event(SYS_CONFIG_FAN_MODE, &dmx_config->fan_mode);
				app_light_flags_set(LIGHT_RS485_FAN_MODE_FLAGE);
			}
			if(event_val & DMX_CURVE_REFRESH)
			{
				dmx_sys_config_event_clear(DMX_CURVE_REFRESH);
				if(dmx_config->dim_curve == 1)  dmx_config->dim_curve = 3;
				else if(dmx_config->dim_curve == 2)  dmx_config->dim_curve = 1;
				else if(dmx_config->dim_curve == 3)  dmx_config->dim_curve = 2;
				data_center_write_config_data_no_event(SYS_CONFIG_CURVE_TYPE, &dmx_config->dim_curve);
				app_light_flags_set(LIGHT_RS485_CURVE_TYPE_FLAG);
			}
		}
    }
}

static uint8_t check_sum_calc(const uint8_t* buff, uint16_t size)
{
    uint8_t check_sum = 0;

    while(size--)
    {
        check_sum += *buff++;
    }

    return check_sum;
}

static void dmx_uart_config(void)
{
	hal_uart_set_stopbit(HAL_UART_DMX, USART_STOPBIT_2BIT);
    hal_uart_init(HAL_UART_DMX,250000);
    hal_dmx_write_ctrl_pin(HAL_UART_DMX, 0);
    hal_uart_receive_dma_start(HAL_UART_DMX, dmx_rx_pack[HAL_UART_DMX].data, sizeof(dmx_rx_pack[HAL_UART_DMX].data));
    hal_uart_irq_register(HAL_UART_DMX, HAL_UART_IRQ_ERR, wired_dmx_rx_err_int_cb);
    hal_uart_irq_register(HAL_UART_DMX, HAL_UART_IRQ_RX_TIMEOUT, wired_dmx_rx_timeout_int_cb);	
    hal_uart_irq_register(HAL_UART_DMX, HAL_UART_IRQ_TX_DMA_FT, wired_dmx_tx_complete_irq_cb);	
}


static void crmx_uart_config(void)
{
    hal_uart_init(HAL_UART_CRMX,250000);
    hal_uart_receive_dma_start(HAL_UART_CRMX, dmx_rx_pack[HAL_UART_CRMX].data, sizeof(dmx_rx_pack[HAL_UART_CRMX].data));
    hal_uart_irq_register(HAL_UART_CRMX, HAL_UART_IRQ_ERR, wireless_dmx_rx_err_int_cb);
    hal_uart_irq_register(HAL_UART_CRMX, HAL_UART_IRQ_TX_DMA_FT, wireless_dmx_tx_complete_irq_cb);	
}

static void dmx_uast_reset(void)
{
	static uint8_t asta = 0;
	static uint8_t bsta = 0;
	switch(asta)
	{
		case 0: 		
			dmx_uart_config();
			bsta = 0;
			asta = 2;
		break;
		case 1:
			base_data_collect_init();
			bsta = 1;
			asta = 2;
		break;
		case 2 :
            if ( (screen_get_act_pid() == PAGE_TEST || screen_get_act_pid() == PAGE_CALIBRATION_MODE) && bsta == 0)
			{
				asta = 1;
			}
			else if (screen_get_act_pid() != PAGE_TEST && screen_get_act_pid() != PAGE_CALIBRATION_MODE && bsta == 1)
			{
				asta = 0;
			}
			else
			{
				asta = 2;
			}
			break;
	}
}

static void dmx_event_user_handle(enum dmx_event_type event, uint8_t* buff, uint16_t size)
{
    static uint8_t refresh_ui;
    switch(event)
    {
            /* 处理RDM数据 */
        case EVENT_WIRED_RDM_UPDATE:
            event_cnt[0]++;
//            Dispose_Recv_Rdm_Data();
        break;
            /* 处理DMX数据更新事件，DMX连接后DMX addr 、DMX profile 或者DMX数据有变化才会产生该事件*/
        case EVENT_WIRED_DMX_UPDATE:
            event_cnt[1]++;
            if((screen_get_act_pid() <= PAGE_MENU || screen_get_act_pid() >= PAGE_OUTPUT_MODE)) 
            {
				if(screen_get_act_pid() == PAGE_UPDATE && screen_get_act_spid() > SUB_ID_FIRMWARE_VERSION)
				{
					return;
				}
                dmx_data_protocol(buff);
                s_dmx_link_state.wired_linked = 1;
                data_center_write_sys_info(SYS_INFO_DMX, &s_dmx_link_state);
				exit_screen_protector();
            }
        break;
            /* 处理DMX连接需求, 如跳转界面，响应DMX数据 */
        case EVENT_WIRED_DMX_CONNECTED:
            event_cnt[2]++;
            s_dmx_link_state.wired_linked = 1;
			ui_set_high_speed_switch(1);
            data_center_write_sys_info(SYS_INFO_DMX, &s_dmx_link_state);
            if((screen_get_act_pid() != PAGE_DMX_MODE) && (refresh_ui == 0) && (PAGE_UPDATE != screen_get_act_pid() || ((PAGE_UPDATE == screen_get_act_pid() && SUB_ID_UPDATING != screen_get_act_spid()))))  //如果当前bu在DMX界面
            {
                refresh_ui = 1;
				if(PAGE_WORK_MODE == screen_get_act_pid() && screen_get_act_spid() == SUB_ID_SYNC_UPDATAING)
					break;
                os_ev_publish_event(MAIN_EV_DMX, EV_DMX_CONNECTED, 0);
            }
            dmx_data_protocol(buff);
			exit_screen_protector();
        break;
            /* 处理DMX断连需求 */
        case EVENT_WIRED_DMX_DISCONNECTED:
            event_cnt[3]++;
            refresh_ui = 0;
            s_dmx_link_state.wired_linked = 0;
            data_center_write_sys_info(SYS_INFO_DMX, &s_dmx_link_state); 
//			if(screen_get_act_pid() == PAGE_DMX_MODE)  //如果当前在DMX界面
			os_ev_publish_event(MAIN_EV_DMX, EV_DMX_DISCONNECTED, 0);  
			ui_set_high_speed_switch(0);		
			memset(buff, 0, 512);
			app_dmx_last_data_clear();
        break;
            /* 处理DMX转发需求，如转发CRMX或者ArtNet数据 */
        case EVENT_WIRED_DMX_TRANSMIT:
            event_cnt[4]++;
            wired_dmx_transmit_with_break(buff, size);
        break;
#if WIRELESS_DMX_USE          
            /* 处理RDM数据 */
        case EVENT_WIRELESS_RDM_UPDATE:
            event_cnt[5]++;
        break;
            /* 处理DMX数据更新事件，DMX连接后DMX addr 、DMX profile 或者DMX数据有变化才会产生该事件*/
        case EVENT_WIRELESS_DMX_UPDATE:
            event_cnt[6]++;
		    if((screen_get_act_pid() <= PAGE_MENU || screen_get_act_pid() >= PAGE_OUTPUT_MODE)) 
            {
				if(screen_get_act_pid() == PAGE_UPDATE && screen_get_act_spid() > SUB_ID_FIRMWARE_VERSION)
				{
					return;
				}
				dmx_data_protocol(buff);
				exit_screen_protector();
			}
        break;
            /* 处理DMX连接需求, 如跳转界面，响应DMX数据 */
        case EVENT_WIRELESS_DMX_CONNECTED:
            event_cnt[7]++;
            s_dmx_link_state.wireless_linked = 1;
			ui_set_high_speed_switch(1);
            data_center_write_sys_info(SYS_INFO_DMX, &s_dmx_link_state);  
            if((screen_get_act_pid() != PAGE_DMX_MODE) && (refresh_ui == 0) && (PAGE_UPDATE != screen_get_act_pid() || ((PAGE_UPDATE == screen_get_act_pid() && SUB_ID_UPDATING != screen_get_act_spid()))))  //如果当前不在DMX界面
            {
				if(PAGE_WORK_MODE == screen_get_act_pid() && screen_get_act_spid() == SUB_ID_SYNC_UPDATAING)
					break;
                refresh_ui = 1;
                os_ev_publish_event(MAIN_EV_DMX, EV_DMX_CONNECTED, 0);
            }
            dmx_data_protocol(buff);
			exit_screen_protector();
        break;
            /* 处理DMX断连需求 */
        case EVENT_WIRELESS_DMX_DISCONNECTED:
            event_cnt[8]++;
            s_dmx_link_state.wireless_linked = 0;
            refresh_ui = 0;
            data_center_write_sys_info(SYS_INFO_DMX, &s_dmx_link_state); 
//			if(screen_get_act_pid() == PAGE_DMX_MODE)  //如果当前在DMX界面
			if(dmx[0].lost_tick == 0)
			{
				os_ev_publish_event(MAIN_EV_DMX, EV_DMX_DISCONNECTED, 0);  
				ui_set_high_speed_switch(0);
			}				
			app_dmx_last_data_clear();
			memset(buff, 0, 512);
        break;  
#endif       
#if ETH_DMX_USE          
            /* 处理RDM数据 */
        case EVENT_ETH_RDM_UPDATE:
            
        break;
            /* 处理DMX数据更新事件，DMX连接后DMX addr 、DMX profile 或者DMX数据有变化才会产生该事件*/
        case EVENT_ETH_DMX_UPDATE:
			s_dmx_link_state.eth_linked = 1;
            if((screen_get_act_pid() <= PAGE_MENU || screen_get_act_pid() >= PAGE_OUTPUT_MODE)) 
            {
				if(screen_get_act_pid() == PAGE_UPDATE && screen_get_act_spid() > SUB_ID_FIRMWARE_VERSION)
				{
					return;
				}
                dmx_data_protocol(buff);
				exit_screen_protector();
			}
        break;
            /* 处理DMX连接需求, 如跳转界面，响应DMX数据 */
        case EVENT_ETH_DMX_CONNECTED:
            s_dmx_link_state.eth_linked = 1;
			ui_set_high_speed_switch(1);
            data_center_write_sys_info(SYS_INFO_DMX, &s_dmx_link_state);  
            if((screen_get_act_pid() != PAGE_DMX_MODE) && (refresh_ui == 0) && (PAGE_UPDATE != screen_get_act_pid() || ((PAGE_UPDATE == screen_get_act_pid() && SUB_ID_UPDATING != screen_get_act_spid()))))  //如果当前不在DMX界面
            {
				refresh_ui = 1;
				if(PAGE_WORK_MODE == screen_get_act_pid() && screen_get_act_spid() == SUB_ID_SYNC_UPDATAING)
					break;
                os_ev_publish_event(MAIN_EV_DMX, EV_DMX_CONNECTED, 0);
            }
            dmx_data_protocol(buff);
			exit_screen_protector();
        break;
            /* 处理DMX断连需求 */
        case EVENT_ETH_DMX_DISCONNECTED:
            s_dmx_link_state.eth_linked = 0;
            refresh_ui = 0;
            data_center_write_sys_info(SYS_INFO_DMX, &s_dmx_link_state); 
			if(dmx[0].lost_tick == 0 && dmx[1].lost_tick == 0)
			{
				os_ev_publish_event(MAIN_EV_DMX, EV_DMX_DISCONNECTED, 0); 
				ui_set_high_speed_switch(0);
			}				
			app_dmx_last_data_clear();
			memset(buff, 0, 512);
        break;  
#endif           
        default:break;
    }
}

static LumenRadioPinState_t crms_irq_read_cb(uint8_t id)
{
	LumenRadioPinState_t status = 0;
	
	status = (LumenRadioPinState_t)dev_crmx_irq_read();
	
	return status;
}

static void crmx_delay_cb(uint8_t id, uint8_t count)
{
	osDelay(count);
}

static void crmx_nss_write_cb(uint8_t id, LumenRadioPinState_t status)
{
	dev_crmx_cs_ctrl(status);
}

static uint8_t crmx_data_read_write_cb(uint8_t id, uint8_t data)
{
	uint8_t read_data = 0;
	
	read_data = dev_crmx_send_read_data(data);
	
	return read_data;
}

static void app_crmx_init(void)
{
    dev_crmx_init();
    data_center_read_config_data(SYS_CONFIG_CRMX_CTRL, &crmx_on);
    dev_crmx_power_ctrl(crmx_on); 
	g_lumenradio_device.m_deviceId = 1;
	g_lumenradio_device.m_spiIrqState = LUMENRADIO_SPI_IRQ_STATE_IDLE;
	g_lumenradio_device.m_readIrqPin = crms_irq_read_cb;
	g_lumenradio_device.m_delayus = crmx_delay_cb;
	g_lumenradio_device.m_writeNssPin = crmx_nss_write_cb;
	g_lumenradio_device.m_spiTxRxByte = crmx_data_read_write_cb;
}

static void app_crmx_power_cb(uint32_t event_type, uint32_t event_value)
{
	bool res;
	
    if(event_type != MAIN_EV_DMX)
        return;
    
    switch(event_value)
    {
        case EV_CRMX_POWER:
            dev_crmx_power_ctrl(os_ev_get_event_data(event_type, event_value)); 
        break;
		case EV_CRMX_UNLINK:
            res = lumenradio_unlink(&g_lumenradio_device);
			if(res == false)
			{
				lumenradio_unlink(&g_lumenradio_device);
			} 
        break;
        default:break;
    }
}

static void crmx_sttatus_handle(uint8_t state)
{
	uint8_t crmx_state[4] = {0};
	bool res = false;
	static uint8_t crmx_link_anim = 1;
	uint8_t crmx_link_strength = 0;
	bool link_status = 0;
	bool rf_link_status = 0;
	bool dmx_status_link = 0;
	LumenRadioDmxWindowReg_t dmx_window;
	
	if(state != 1)
	{
		return;
	}
	
	res = lumenradio_get_status(&g_lumenradio_device, LUMENRADIO_FLAG_LINKED , LUMENRADIO_FLAGS_RELA_AND, &link_status );
	res = lumenradio_get_status(&g_lumenradio_device, LUMENRADIO_FLAG_RF_LINK, LUMENRADIO_FLAGS_RELA_AND, &rf_link_status);
	res = lumenradio_get_status(&g_lumenradio_device, LUMENRADIO_FLAG_DMX, LUMENRADIO_FLAGS_RELA_AND, &dmx_status_link);
	lumenradio_read_register(&g_lumenradio_device,LUMENRADIO_REG_DMX_WINDOW,&dmx_window);
	
	if(dmx_window.m_windowSize == 0)
	{
		error_functionArray[lumenridio_ec_009](1);
	}
	
	if(link_status == false)//没有连接tx two
	{
		for(uint8_t i = 0; i < 4; i++)
		{
			lumenradio_get_status(&g_lumenradio_device, LUMENRADIO_FLAG_LINKED, LUMENRADIO_FLAGS_RELA_OR, &link_status);
			if(link_status == false)
				crmx_state[i] = 0;
			else
				crmx_state[i] = 1;
		}
		for(uint8_t i = 0; i < 4; i++)
		{
			if(crmx_state[i] == 1)
			{
				break;
			}
			if(i == 3)
			{
				crmx_sta.pair_state = 0;
				crmx_link_anim = 0;
			}
		}
	}
	else if(link_status == true && rf_link_status == false)//连接tx two但是距离太远
	{
		for(uint8_t i = 0; i < 4; i++)
		{
			lumenradio_get_status(&g_lumenradio_device, LUMENRADIO_FLAG_RF_LINK, LUMENRADIO_FLAGS_RELA_OR, &rf_link_status);
			if(rf_link_status == false)
				crmx_state[i] = 0;
			else
				crmx_state[i] = 1;
		}
		for(uint8_t i = 0; i < 4; i++)
		{
			if(crmx_state[i] == 1)
			{
				break;
			}
			if(i == 3)
			{
				crmx_sta.pair_state = 1;
			}
		}
	}
	else if(link_status == true && rf_link_status == true && dmx_status_link == false)//连接tx two但是没有dmx数据
	{
		for(uint8_t i = 0; i < 4; i++)
		{
			lumenradio_get_status(&g_lumenradio_device, LUMENRADIO_FLAG_DMX, LUMENRADIO_FLAGS_RELA_OR, &dmx_status_link);
			if(dmx_status_link == false)
				crmx_state[i] = 0;
			else
				crmx_state[i] = 1;
		}
		for(uint8_t i = 0; i < 4; i++)
		{
			if(crmx_state[i] == 1)
			{
				break;
			}
			if(i == 3)
			{
				crmx_sta.pair_state = 2;
			}
		}
	}
	else if(link_status == true && rf_link_status == true && dmx_status_link == true)//连接tx two也有dmx数据
	{
		crmx_sta.pair_state = 3;
	}
	
	if(ui_get_dmx_state())
	{
		if(rf_link_status == true && screen_get_act_pid() != PAGE_DMX_MODE && screen_get_act_spid() != SUB_ID_DMX_STATUS)
		{
			if(((PAGE_UPDATE != screen_get_act_pid() || (((PAGE_UPDATE == screen_get_act_pid() && screen_get_act_spid() < SUB_ID_UPDATING)) || \
				PAGE_WORK_MODE == screen_get_act_pid())) && screen_get_act_spid() != SUB_ID_SYNC_UPDATAING) \
				&& get_base_updata_state() == 0)   //升级时不跳转
				user_enter_dmx();
			ui_set_dmx_state(0);
		}
	}
	if(link_status == true)//获取信号强度
	{
		if(crmx_link_anim == 0)   //第一次连接进入连接界面
		{
			crmx_link_anim = 1;
			if(((PAGE_UPDATE != screen_get_act_pid() || (((PAGE_UPDATE == screen_get_act_pid() && screen_get_act_spid() < SUB_ID_UPDATING)) || \
				PAGE_WORK_MODE == screen_get_act_pid())) && screen_get_act_spid() != SUB_ID_SYNC_UPDATAING) \
				&& get_base_updata_state() == 0)   //升级时不跳转
				user_enter_crmx_pairing();
		}
		res = lumenradio_get_link_quality(&g_lumenradio_device, &crmx_link_strength);
		if(res == true)
		{
			if(crmx_link_strength < 20)
				crmx_sta.crmx_link_streng = 0;
			else if(crmx_link_strength < 40)
				crmx_sta.crmx_link_streng = 1;
			else if(crmx_link_strength < 70)
				crmx_sta.crmx_link_streng = 2;
			else if(crmx_link_strength < 100)
				crmx_sta.crmx_link_streng = 3;
		}
	}
	
	data_center_write_sys_info(SYS_INFO_CRMX, &crmx_sta);
}

static void app_crmx_stutas_handle(void)
{
	struct sys_info_power info;
	static uint8_t frist_power_crmx_status = 0;
	static uint8_t frist_power_crmx_count = 0;
	
	data_center_read_sys_info(SYS_INFO_POWER, &info);
	data_center_read_config_data(SYS_CONFIG_CRMX_CTRL, &crmx_on);
	if(info.state == 1 && crmx_on == 1)
	{
		switch(frist_power_crmx_status)
		{
			case 0:
				if(frist_power_crmx_count++ > 19)
				{
					frist_power_crmx_count = 0;
					frist_power_crmx_status = 1;
				}
			break;
			case 1:
			{
				crmx_sttatus_handle(crmx_on);
			}
			break;
		}
	}
	else
	{
		frist_power_crmx_count = 0;
		frist_power_crmx_status = 0;
	}
}

static void app_crmx_set_name_funtion(void)
{
	char name[21] = {0};
	char ble_name[6] ={0};
	bool res;
	int8_t i = 0;
	static uint16_t crmx_name_count = 0;
	
	crmx_name_count++;
	if(crmx_name_count == 1)
	{
		dev_crmx_power_ctrl(1);
	}
	if(crmx_name_count >= 500)
	{
		crmx_name_count = 0;
		data_center_read_config_data(SYS_CONFIG_BLE_SN, ble_name);
		i = 6;
		while(i >= 0)
		{
			if(ble_name[i] >= 'a' && ble_name[i] <= 'z')
			{
				ble_name[i] = ble_name[i] - 32;
			}
			i--;
		}
		memcpy(&name[13], ble_name, 6);
		name[19]='\0';
		
		res = lumenradio_set_work_mode(&g_lumenradio_device, LUMENRADIO_WORK_MODE_RECEIVER);
		if(res != true)
		{
			lumenradio_set_work_mode(&g_lumenradio_device, LUMENRADIO_WORK_MODE_RECEIVER);
		}
		res = lumenradio_set_device_name(&g_lumenradio_device, name);
		if(res != true)
		{
			lumenradio_set_device_name(&g_lumenradio_device, name);
		}
		res = lumenradio_unlink(&g_lumenradio_device);
		if(res == false)
		{
			lumenradio_unlink(&g_lumenradio_device);
		}
		dev_crmx_power_ctrl(0);
		set_ble_power(1);
		set_ble_power_status(1);
		os_ev_publish_event(MAIN_EV_BLE, OS_EVENT_BLE_RESET, 0);
		app_dmx_lumenradio_name_flag_set(0);
	}
}

static void app_rdm_identy_set(uint8_t flag)
{
    if(flag == 0)
        os_ev_publish_event(MAIN_EV_LIGHT, EV_DATA_LIGHT_TRIGGRT, 0);
    else if(flag == 1)
        os_ev_publish_event(MAIN_EV_LIGHT, EV_DATA_LIGHT_TRIGGRT, 1);
}

static void app_rdm_set_dmxaddr(uint16_t addr)
{
    data_center_write_config_data_no_event(SYS_CONFIG_DMX_ADDR, &addr);
	config.dmx_addr = addr;
	user_enter_dmx_addr(); 
}

static uint16_t app_rdm_get_dmxaddr(void)
{
    uint16_t addr;
    
    data_center_read_config_data(SYS_CONFIG_DMX_ADDR, &addr);
    
    return addr;
}

//DMX模型
static void app_rdm_set_dmx_profile(uint16_t profile)
{
	uint8_t pro = profile;
	
    data_center_write_config_data_no_event(SYS_CONFIG_DMX_PROFILE, &pro);
	config.profile_index = pro;
	user_enter_dmx_profile(); 
}

static uint16_t app_rdm_get_dmx_profile(void)
{
    uint8_t profile;
    
    data_center_read_config_data(SYS_CONFIG_DMX_PROFILE, &profile);
    
    return (uint16_t)profile;
}

extern struct dmx_config_data  dmx_sys_config;

//DMX 数据解析   User needs to modify
#if (PROJECT_TYPE == 308)  
static void dmx_data_protocol(uint8_t* buff)
{
	struct sys_info_accessories dmx_access_info;
	uint8_t hs_mode;
	uint8_t dmx_extension = 0;
	uint8_t dmx_extension1[12] = {1};
	int16_t data[2];
	int16_t data1[12][2];
	struct sys_info_power info;
	
	data_center_read_sys_info(SYS_INFO_POWER, &info);
    dmx_protocol.fade = config.fade_time;
	dmx_yoke_fpesnel_angle_range_set(500, 180);
	data_center_read_sys_info(SYS_INFO_ACCESS, &dmx_access_info);
	data_center_read_config_data(SYS_CONFIG_HIGH_SPEED_MODE, &hs_mode);
	data_center_read_config_data(SYS_CONFIG_DMX_EXTENSION, &dmx_extension);
	switch(dmx_extension)
	{
		case 0:
			dmx_electric_control_switch_set(0);
			dmx_funtion_control_switch_set(0);
		break;
		case 1:
			dmx_electric_control_switch_set(1);
			dmx_funtion_control_switch_set(0);
		break;
		case 2:
			dmx_electric_control_switch_set(0);
			dmx_funtion_control_switch_set(1);
		break;
		case 3:
			dmx_electric_control_switch_set(1);
			dmx_funtion_control_switch_set(1);
		break;
		default:break;
	}
	
	if(dmx_access_info.fresnel_state == 1 || dmx_access_info.optical_20_access_state == 1 || 
	   dmx_access_info.optical_30_access_state == 1 || dmx_access_info.optical_50_access_state == 1)  // 装配我司电动附件
	{
		data_center_read_config_data(SYS_CONFIG_ANGLE_LIMIT, &dmx_extension);  //是否打开角度限制
		if(dmx_extension == 1)
		{
			data_center_read_config_data(SYS_CONFIG_YOKE_PAN_LIMIT, data);
			if(data[1] > ANGLE_LIMIT_MAX)
				data[1] = ANGLE_LIMIT_MAX;
			if(data[0] < ANGLE_LIMIT_MIN)
				data[0] = ANGLE_LIMIT_MIN;
			dmx_yoke_pan_range_set(data[1], data[0]);
			data_center_read_config_data(SYS_CONFIG_YOKE_TILT_LIMIT, data);
			if(dmx_access_info.fresnel_state == 1 || dmx_access_info.optical_20_access_state)
			{
				if(data[1] > 120)
					data[1] = 120;
				if(data[0] < -120)
					data[0] = -120;
			}
			else
			{
				if(data[1] > ANGLE_LIMIT_DEFAULT_MAX)
					data[1] = ANGLE_LIMIT_DEFAULT_MAX;
				if(data[0] < ANGLE_LIMIT_DEFAULT_MIN)
					data[0] = ANGLE_LIMIT_DEFAULT_MIN;
			}
			if(data[0] < ANGLE_LIMIT_MIN)
			{
				data[1] = data[1] + (-(data[0]));
				data[0] = ANGLE_LIMIT_MIN;
			}
			dmx_yoke_tilt_range_set(data[1], data[0]);
		}
		else
		{
			dmx_yoke_pan_range_set(ANGLE_LIMIT_MAX, ANGLE_LIMIT_MIN);
			if(dmx_access_info.fresnel_state == 1 || dmx_access_info.optical_20_access_state)
				dmx_yoke_tilt_range_set(240, 0);
			else
				dmx_yoke_tilt_range_set(270, 0);
		}
	}
	else
	{
		data_center_read_config_data(SYS_CONFIG_NO_ANGLE_LIMIT, &dmx_extension1);  //是否打开角度限制
		data_center_read_config_data(SYS_CONFIG_NOT_ACCESS_SELECT, &dmx_extension);
		if(dmx_extension > 10)
			dmx_extension = 10;
		if(dmx_extension1[dmx_extension] == 1)
		{
			data_center_read_config_data(SYS_CONFIG_YOKE_NO_PAN_LIMIT, data1);
			if(data1[dmx_extension][1] > ANGLE_LIMIT_MAX)
				data1[dmx_extension][1] = ANGLE_LIMIT_MAX;
			if(data1[dmx_extension][0] < ANGLE_LIMIT_MIN)
				data1[dmx_extension][0] = ANGLE_LIMIT_MIN;
			dmx_yoke_pan_range_set(data1[dmx_extension][1], data1[dmx_extension][0]);
			data_center_read_config_data(SYS_CONFIG_YOKE_NO_TILT_LIMIT, data1);
			if(data1[dmx_extension][1] > ANGLE_LIMIT_DEFAULT_MAX)
				data1[dmx_extension][1] = ANGLE_LIMIT_DEFAULT_MAX;
			if(data1[dmx_extension][0] < ANGLE_LIMIT_DEFAULT_MIN)
				data1[dmx_extension][0] = ANGLE_LIMIT_DEFAULT_MIN;
			if(data1[dmx_extension][0] < ANGLE_LIMIT_MIN)
			{
				data1[dmx_extension][1] = data1[dmx_extension][1] + (-(data1[dmx_extension][0]));
				data1[dmx_extension][0] = ANGLE_LIMIT_MIN;
			}
			dmx_yoke_tilt_range_set(data1[dmx_extension][1], data1[dmx_extension][0]);
		}
		else
		{
			if(dmx_extension >= 8)
			{
				data_center_read_config_data(SYS_CONFIG_YOKE_NO_PAN_LIMIT, data1);
				if(data1[dmx_extension][1] > ANGLE_LIMIT_MAX)
					data1[dmx_extension][1] = ANGLE_LIMIT_MAX;
				if(data1[dmx_extension][0] < ANGLE_LIMIT_MIN)
					data1[dmx_extension][0] = ANGLE_LIMIT_MIN;
				dmx_yoke_pan_range_set(data1[dmx_extension][1], data1[dmx_extension][0]);
				data_center_read_config_data(SYS_CONFIG_YOKE_NO_TILT_LIMIT, data1);
				if(data1[dmx_extension][1] > ANGLE_LIMIT_DEFAULT_MAX)
					data1[dmx_extension][1] = ANGLE_LIMIT_DEFAULT_MAX;
				if(data1[dmx_extension][0] < ANGLE_LIMIT_DEFAULT_MIN)
					data1[dmx_extension][0] = ANGLE_LIMIT_DEFAULT_MIN;
				if(data1[dmx_extension][0] < ANGLE_LIMIT_MIN)
				{
					data1[dmx_extension][1] = data1[dmx_extension][1] + (-(data1[dmx_extension][0]));
					data1[dmx_extension][0] = ANGLE_LIMIT_MIN;
				}
				dmx_yoke_tilt_range_set(data1[dmx_extension][1], data1[dmx_extension][0]);
			}
			else
			{
				dmx_yoke_pan_range_set(ANGLE_LIMIT_MAX, ANGLE_LIMIT_MIN);
				dmx_yoke_tilt_range_set(270, 0);
			}
		}
	}
	
	switch(config.profile_index)
	{
		case DMX_PROFILE_8BIT_CCT:
			dmx_protocol.strobe_or_fx_state = profile_user_data_execte(DMX_PROFILE_ID_0105,buff,&dmx_protocol.dmx_data); 
		break;
		case DMX_PROFILE_8BIT_FX:
			dmx_protocol.strobe_or_fx_state = profile_user_data_execte(DMX_PROFILE_ID_0703,buff,&dmx_protocol.dmx_data); 
		break;
		case DMX_PROFILE_8BIT_LIGHTING_AND_FX:
			dmx_protocol.strobe_or_fx_state = profile_user_data_execte(DMX_PROFILE_ID_3007,buff,&dmx_protocol.dmx_data); 
		break;        
		case DMX_PROFILE_16BIT_CCT:
			dmx_protocol.strobe_or_fx_state = profile_user_data_execte(DMX_PROFILE_ID_0106,buff,&dmx_protocol.dmx_data); 
		break;
		case DMX_PROFILE_16BIT_FX:
			dmx_protocol.strobe_or_fx_state = profile_user_data_execte(DMX_PROFILE_ID_0704,buff,&dmx_protocol.dmx_data);             
		break;
		case DMX_PROFILE_16BIT_LIGHTING_AND_FX:
			dmx_protocol.strobe_or_fx_state = profile_user_data_execte(DMX_PROFILE_ID_3008,buff,&dmx_protocol.dmx_data); 
		break;        
		default:break;
	}
	if((0 != memcmp(&dmx_protocol, &s_last_dmx_protocol, sizeof(s_last_dmx_protocol))))
	{
		data_center_write_light_data(LIGHT_MODE_DMX, &dmx_protocol); 	
		memcpy(&s_last_dmx_protocol, &dmx_protocol, sizeof(s_last_dmx_protocol));
	}
}
#elif (PROJECT_TYPE == 307)
static void dmx_data_protocol(uint8_t* buff)
{
	struct sys_info_accessories dmx_access_info;
	int16_t data[2];
	int16_t data1[12][2];
	uint8_t dmx_extension = 0;
	uint8_t dmx_extension1[12] = {1};
	uint8_t hs_mode;
	struct sys_info_power info;
	
	data_center_read_sys_info(SYS_INFO_POWER, &info);
    dmx_protocol.fade = config.fade_time;
	data_center_read_config_data(SYS_CONFIG_HIGH_SPEED_MODE, &hs_mode);
	data_center_read_config_data(SYS_CONFIG_DMX_EXTENSION, &dmx_extension);
	switch(dmx_extension)
	{
		case 0:
			dmx_electric_control_switch_set(0);
			dmx_funtion_control_switch_set(0);
		break;
		case 1:
			dmx_electric_control_switch_set(1);
			dmx_funtion_control_switch_set(0);
		break;
		case 2:
			dmx_electric_control_switch_set(0);
			dmx_funtion_control_switch_set(1);
		break;
		case 3:
			dmx_electric_control_switch_set(1);
			dmx_funtion_control_switch_set(1);
		break;
		default:break;
	}
	
	dmx_yoke_fpesnel_angle_range_set(500, 180);
	data_center_read_sys_info(SYS_INFO_ACCESS, &dmx_access_info);
	
	if(dmx_access_info.fresnel_state == 1 || dmx_access_info.optical_20_access_state == 1 || 
	   dmx_access_info.optical_30_access_state == 1 || dmx_access_info.optical_50_access_state == 1)  // 装配我司电动附件
	{
		data_center_read_config_data(SYS_CONFIG_ANGLE_LIMIT, &dmx_extension);  //是否打开角度限制
		if(dmx_extension == 1)
		{
			data_center_read_config_data(SYS_CONFIG_YOKE_PAN_LIMIT, data);
			if(data[1] > ANGLE_LIMIT_MAX)
				data[1] = ANGLE_LIMIT_MAX;
			if(data[0] < ANGLE_LIMIT_MIN)
				data[0] = ANGLE_LIMIT_MIN;
			dmx_yoke_pan_range_set(data[1], data[0]);
			data_center_read_config_data(SYS_CONFIG_YOKE_TILT_LIMIT, data);
			if(dmx_access_info.fresnel_state == 1 || dmx_access_info.optical_20_access_state)
			{
				if(data[1] > 120)
					data[1] = 120;
				if(data[0] < -120)
					data[0] = -120;
			}
			else
			{
				if(data[1] > ANGLE_LIMIT_DEFAULT_MAX)
					data[1] = ANGLE_LIMIT_DEFAULT_MAX;
				if(data[0] < ANGLE_LIMIT_DEFAULT_MIN)
					data[0] = ANGLE_LIMIT_DEFAULT_MIN;
			}
			if(data[0] < ANGLE_LIMIT_MIN)
			{
				data[1] = data[1] + (-(data[0]));
				data[0] = ANGLE_LIMIT_MIN;
			}
			dmx_yoke_tilt_range_set(data[1], data[0]);
		}
		else
		{
			dmx_yoke_pan_range_set(ANGLE_LIMIT_MAX, ANGLE_LIMIT_MIN);
			if(dmx_access_info.fresnel_state == 1 || dmx_access_info.optical_20_access_state)
				dmx_yoke_tilt_range_set(240, 0);
			else
				dmx_yoke_tilt_range_set(270, 0);
		}
	}
	else
	{
		data_center_read_config_data(SYS_CONFIG_NO_ANGLE_LIMIT, &dmx_extension1);  //是否打开角度限制
		data_center_read_config_data(SYS_CONFIG_NOT_ACCESS_SELECT, &dmx_extension);
		if(dmx_extension > 10)
			dmx_extension = 10;
		if(dmx_extension1[dmx_extension] == 1)
		{
			data_center_read_config_data(SYS_CONFIG_YOKE_NO_PAN_LIMIT, data1);
			if(data1[dmx_extension][1] > ANGLE_LIMIT_MAX)
				data1[dmx_extension][1] = ANGLE_LIMIT_MAX;
			if(data1[dmx_extension][0] < ANGLE_LIMIT_MIN)
				data1[dmx_extension][0] = ANGLE_LIMIT_MIN;
			dmx_yoke_pan_range_set(data1[dmx_extension][1], data1[dmx_extension][0]);
			data_center_read_config_data(SYS_CONFIG_YOKE_NO_TILT_LIMIT, data1);
			if(data1[dmx_extension][1] > ANGLE_LIMIT_DEFAULT_MAX)
				data1[dmx_extension][1] = ANGLE_LIMIT_DEFAULT_MAX;
			if(data1[dmx_extension][0] < ANGLE_LIMIT_DEFAULT_MIN)
				data1[dmx_extension][0] = ANGLE_LIMIT_DEFAULT_MIN;
			if(data1[dmx_extension][0] < ANGLE_LIMIT_MIN)
			{
				data1[dmx_extension][1] = data1[dmx_extension][1] + (-(data1[dmx_extension][0]));
				data1[dmx_extension][0] = ANGLE_LIMIT_MIN;
			}
			dmx_yoke_tilt_range_set(data1[dmx_extension][1], data1[dmx_extension][0]);
		}
		else
		{
			if(dmx_extension >= 8)
			{
				data_center_read_config_data(SYS_CONFIG_YOKE_NO_PAN_LIMIT, data1);
				if(data1[dmx_extension][1] > ANGLE_LIMIT_MAX)
					data1[dmx_extension][1] = ANGLE_LIMIT_MAX;
				if(data1[dmx_extension][0] < ANGLE_LIMIT_MIN)
					data1[dmx_extension][0] = ANGLE_LIMIT_MIN;
				dmx_yoke_pan_range_set(data1[dmx_extension][1], data1[dmx_extension][0]);
				data_center_read_config_data(SYS_CONFIG_YOKE_NO_TILT_LIMIT, data1);
				if(data1[dmx_extension][1] > ANGLE_LIMIT_DEFAULT_MAX)
					data1[dmx_extension][1] = ANGLE_LIMIT_DEFAULT_MAX;
				if(data1[dmx_extension][0] < ANGLE_LIMIT_DEFAULT_MIN)
					data1[dmx_extension][0] = ANGLE_LIMIT_DEFAULT_MIN;
				if(data1[dmx_extension][0] < ANGLE_LIMIT_MIN)
				{
					data1[dmx_extension][1] = data1[dmx_extension][1] + (-(data1[dmx_extension][0]));
					data1[dmx_extension][0] = ANGLE_LIMIT_MIN;
				}
				dmx_yoke_tilt_range_set(data1[dmx_extension][1], data1[dmx_extension][0]);
			}
			else
			{
				dmx_yoke_pan_range_set(ANGLE_LIMIT_MAX, ANGLE_LIMIT_MIN);
				dmx_yoke_tilt_range_set(270, 0);
			}
		}
	}
	
//	if(hs_mode == 0)
//	{
	switch(config.profile_index)
	{
		case DMX_PROFILE_8BIT_CCT_RGB:
			dmx_protocol.strobe_or_fx_state = profile_user_data_execte(DMX_PROFILE_ID_0801,buff,&dmx_protocol.dmx_data); 
		break;
		case DMX_PROFILE_8BIT_CCT:
			dmx_protocol.strobe_or_fx_state = profile_user_data_execte(DMX_PROFILE_ID_0105,buff,&dmx_protocol.dmx_data); 
		break;
		case DMX_PROFILE_8BIT_CCT_HSI:
			dmx_protocol.strobe_or_fx_state = profile_user_data_execte(DMX_PROFILE_ID_0901,buff,&dmx_protocol.dmx_data); 
		break;        
		case DMX_PROFILE_8BIT_RGB:
			dmx_protocol.strobe_or_fx_state = profile_user_data_execte(DMX_PROFILE_ID_0201,buff,&dmx_protocol.dmx_data); 
		break;
		case DMX_PROFILE_8BIT_HSI:
			dmx_protocol.strobe_or_fx_state = profile_user_data_execte(DMX_PROFILE_ID_0303,buff,&dmx_protocol.dmx_data);             
		break;		
		case DMX_PROFILE_8BIT_GEL:
			dmx_protocol.strobe_or_fx_state = profile_user_data_execte(DMX_PROFILE_ID_0501,buff,&dmx_protocol.dmx_data); 
		break;
		case DMX_PROFILE_8BIT_XY:
			dmx_protocol.strobe_or_fx_state = profile_user_data_execte(DMX_PROFILE_ID_0401,buff,&dmx_protocol.dmx_data); 
		break;      
		case DMX_PROFILE_8BIT_SOURCE:
			dmx_protocol.strobe_or_fx_state = profile_user_data_execte(DMX_PROFILE_ID_0601,buff,&dmx_protocol.dmx_data); 
		break;
		case DMX_PROFILE_8BIT_FX:
			dmx_protocol.strobe_or_fx_state = profile_user_data_execte(DMX_PROFILE_ID_0707,buff,&dmx_protocol.dmx_data); 
		break;    
		case DMX_PROFILE_8BIT_RGBWW:
			dmx_protocol.strobe_or_fx_state = profile_user_data_execte(DMX_PROFILE_ID_0205,buff,&dmx_protocol.dmx_data); 
		break;  		
		case DMX_PROFILE_8BIT_ULTIMATE:
			dmx_protocol.strobe_or_fx_state = profile_user_data_execte(DMX_PROFILE_ID_3005,buff,&dmx_protocol.dmx_data); 
		break;
		case DMX_PROFILE_16BIT_CCT_RGB:
			dmx_protocol.strobe_or_fx_state = profile_user_data_execte(DMX_PROFILE_ID_0802,buff,&dmx_protocol.dmx_data); 
		break;   
		case DMX_PROFILE_16BIT_CCT:
			dmx_protocol.strobe_or_fx_state = profile_user_data_execte(DMX_PROFILE_ID_0106,buff,&dmx_protocol.dmx_data); 
		break;
		case DMX_PROFILE_16BIT_CCT_HSI:
			dmx_protocol.strobe_or_fx_state = profile_user_data_execte(DMX_PROFILE_ID_0902,buff,&dmx_protocol.dmx_data); 
		break;        
		case DMX_PROFILE_16BIT_RGB:
			dmx_protocol.strobe_or_fx_state = profile_user_data_execte(DMX_PROFILE_ID_0202,buff,&dmx_protocol.dmx_data); 
		break;
		case DMX_PROFILE_16BIT_HSI:
			dmx_protocol.strobe_or_fx_state = profile_user_data_execte(DMX_PROFILE_ID_0304,buff,&dmx_protocol.dmx_data);             
		break;
		case DMX_PROFILE_16BIT_GEL:
			dmx_protocol.strobe_or_fx_state = profile_user_data_execte(DMX_PROFILE_ID_0502,buff,&dmx_protocol.dmx_data); 
		break;
		case DMX_PROFILE_16BIT_XY:
			dmx_protocol.strobe_or_fx_state = profile_user_data_execte(DMX_PROFILE_ID_0402,buff,&dmx_protocol.dmx_data); 
		break;      
		case DMX_PROFILE_16BIT_SOURCE:
			dmx_protocol.strobe_or_fx_state = profile_user_data_execte(DMX_PROFILE_ID_0602,buff,&dmx_protocol.dmx_data); 
		break;
		case DMX_PROFILE_16BIT_FX:
			dmx_protocol.strobe_or_fx_state = profile_user_data_execte(DMX_PROFILE_ID_0708,buff,&dmx_protocol.dmx_data); 
		break;  
		case DMX_PROFILE_16BIT_RGBWW:
			dmx_protocol.strobe_or_fx_state = profile_user_data_execte(DMX_PROFILE_ID_0206,buff,&dmx_protocol.dmx_data); 
		break;		
		case DMX_PROFILE_16BIT_ULTIMATE:
			dmx_protocol.strobe_or_fx_state = profile_user_data_execte(DMX_PROFILE_ID_3006,buff,&dmx_protocol.dmx_data); 
		break;         
		default:
			break;
	}
	if((0 != memcmp(&dmx_protocol, &s_last_dmx_protocol, sizeof(s_last_dmx_protocol))) && info.state == 1)
	{
		data_center_write_light_data(LIGHT_MODE_DMX, &dmx_protocol); 	
		memcpy(&s_last_dmx_protocol, &dmx_protocol, sizeof(s_last_dmx_protocol));
	}
//	}
//	else
//	{
//		switch(config.profile_index)
//		{
//			case DMX_PROFILE_8BIT_CCT:
//				dmx_protocol.strobe_or_fx_state = profile_user_data_execte(DMX_PROFILE_ID_0105,buff,&dmx_protocol.dmx_data); 
//				data_center_write_light_data(LIGHT_MODE_DMX, &dmx_protocol); 
//			break;
//			case DMX_PROFILE_16BIT_CCT:
//				dmx_protocol.strobe_or_fx_state = profile_user_data_execte(DMX_PROFILE_ID_0106,buff,&dmx_protocol.dmx_data); 
//				data_center_write_light_data(LIGHT_MODE_DMX, &dmx_protocol); 
//			break;
//			default:
//			break;
//		}
//	}
}

#endif
