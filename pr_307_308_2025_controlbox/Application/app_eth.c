/**
 * @file app_eth.c
 * @author Darwin (darwin.xiao@aputure.com)
 * @brief 
 * @version 0.1
 * @date 2022-09-13
 * 
 * @copyright Copyright (c) 2022 爱图仕
 * 
 * @par 修改日志:
 * <table>
 * <tr><th> Date      <th>Version <th>Author  <th>Description
 * <tr><td>2022-09-13     <td>1.0     <td>Darwin.Xiao   <td>初始创建
 * </table>
 */
#include "lwip/timeouts.h"
#include "lwip/tcpip.h"
#include "netif/etharp.h"
#include "lwip/dhcp.h"
#include "lwip/netifapi.h"

#include "hal_eth.h"
#include "app_artnet.h"
#include "user_sacn.h"
#include "app_eth.h"
#include "app_data_center.h"
#include "os_event.h"
#include "base_type.h"

/* Static IP Address */
#define IP_ADDR0                                (192U)
#define IP_ADDR1                                (168U)
#define IP_ADDR2                                (2U)
#define IP_ADDR3                                (16U)

/* Static Netmask */
#define NETMASK_ADDR0                           (255U)
#define NETMASK_ADDR1                           (255U)
#define NETMASK_ADDR2                           (255U)
#define NETMASK_ADDR3                           (0U)

/* Static Gateway Address*/
#define GW_ADDR0                                (192U)
#define GW_ADDR1                                (168U)
#define GW_ADDR2                                (2U)
#define GW_ADDR3                                (1U)

osThreadId_t app_eth_task_id;

const osThreadAttr_t g_eth_thread_attr = { 
    .name = "app eth", 
    .priority = osPriorityLow, 
    .stack_size = 1280 * 4
};

const osThreadAttr_t g_sACN_thread_attr = { 
    .name = "app sacn", 
    .priority = osPriorityLow, 
    .stack_size = 640 * 4
};

struct local_data {
    struct netif netif;
	struct netif dhcp_netif;
    struct sys_info_eth eth;
    uint8_t ipaddr[4];
    uint8_t netmask[4];
    uint8_t gateway[4];
	uint8_t eth_start;
	uint8_t eth_dhcp_start;
    osThreadId_t sacn_thread;
    uint8_t light_net_init : 1;
};

enum{
	DHCP_OFF              = 0U,
	DHCP_START            = 1U,
	DHCP_WAIT_ADDRESS     = 2U,
	DHCP_ADDRESS_ASSIGNED = 3U,
	DHCP_TIMEOUT          = 4U,
	DHCP_LINK_DOWN        = 5U,
};

static void app_eth_init(void);
static void netif_config_network(struct netif *netif, uint8_t start);
static void eth_check_link(struct netif* netif);
static void sAcn_thread_entry(void* arg);
static void lwip_init(void);
static void eth_dhcp_handle(struct netif *netif);
static void rth_500ms_callback (void *argument);

struct local_data s_eth_data;
static uint8_t s_last_eth_start;
static uint8_t s_lwip_count = 0;
static osTimerId_t s_eth_tmr;
// hal_eth主要负责以太网硬件的初始化,
//硬件相关: GPIO复用, 以太网电源使能，以太网复位，接收中断
//软件相关: 
// 接口:
// hal_eth_init()
// err_t ethernetif_init(struct netif *netif)
// hal_eth_restart(void);
// 
//设置mac地址， 底层输入，底层输出，输入处理线程，
// app_eth: artnet, sACN,
// 控制:以太网硬件复位，DHCP处理，

//  双网口->0, 1
//  需要获取网口（0， 1）的状态， 网口索引
//  非DHCP模式
//  定时检测本地网络参数是否有改变，如果有改变, 重新设置网络参数
//  网卡下线 -》设置IP地址、子网掩码、网关 -》网卡上线
//  DHCP模式
//  等待DHCP分配IP, 如果分配失败重启, 

static void update(void)
{
    data_center_write_sys_info(SYS_INFO_ETH, &s_eth_data.eth);   
}

static void eth_config_pull(void)
{
    data_center_read_config_data(SYS_CONFIG_LOCAL_IP, s_eth_data.ipaddr);
    data_center_read_config_data(SYS_CONFIG_NETMASK, s_eth_data.netmask);
    data_center_read_config_data(SYS_CONFIG_GATEWAY, s_eth_data.gateway);
	data_center_read_config_data(SYS_CONFIG_ETH_START, &s_eth_data.eth_start);
}

static void eth_event_callback(uint32_t event_type, uint32_t event_value)
{
    uint16_t universe = 0;
    uint8_t type = 0;
    
    type = os_ev_get_event_data(event_type, event_value);
	if(event_value & EV_DATA_FACTORY_RESET)
	{
		eth_config_pull();
		netif_config_network(&s_eth_data.netif, s_eth_data.eth_start);
	}
	if(event_value & EV_DATA_CONFIG_CHANGE)
	{
		switch(type)
		{
			case SYS_CONFIG_LOCAL_IP:
			case SYS_CONFIG_NETMASK:
			case SYS_CONFIG_GATEWAY:
				eth_config_pull();
				netif_config_network(&s_eth_data.netif, s_eth_data.eth_start);
			break;
			case SYS_CONFIG_ETH_ARTNET_UNIVERSE:
				data_center_read_config_data(SYS_CONFIG_ETH_ARTNET_UNIVERSE, &universe);
				artnet_set_universe_address(universe);
			break;
			case SYS_CONFIG_ETH_SACN_UNIVERSE:
				data_center_read_config_data(SYS_CONFIG_ETH_SACN_UNIVERSE, &universe);
				artnet_set_universe_address(universe);
			break;
			case SYS_CONFIG_ETH_START:
				data_center_read_config_data(SYS_CONFIG_ETH_START, &s_eth_data.eth_start);
			break;
			default:break;
		}
	}
}

uint32_t eth_task_static_size;

void app_eth_entry(void* arg)
{    
	uint16_t universe = 0;
	uint8_t addr[4] = {0};
	uint8_t dhcp_outtime = 0;
	
    eth_config_pull(); 
	
    s_eth_data.light_net_init = 1;
	s_eth_data.eth.pluged = 0;
    lwip_init();
	s_last_eth_start = s_eth_data.eth_start;
    s_eth_data.sacn_thread = osThreadNew(sAcn_thread_entry, NULL, &g_sACN_thread_attr);    
    osThreadSuspend(s_eth_data.sacn_thread);
//	s_eth_tmr = osTimerNew(rth_500ms_callback, osTimerPeriodic,NULL, NULL);                 
//    osTimerStart(s_eth_tmr, 500);
    os_ev_subscribe_event(MAIN_EV_DATA, EV_DATA_CONFIG_CHANGE | EV_DATA_FACTORY_RESET, eth_event_callback);  
    for (;;)
    {     
        os_ev_pull_event(MAIN_EV_DATA);        
        /* 如果网口被插入,  */
        if (s_eth_data.eth.pluged)
        {
            if (s_eth_data.light_net_init == 1)
            {
                s_eth_data.light_net_init = 0;

                /* 重设IP */
				netif_config_network(&s_eth_data.netif, s_eth_data.eth_start);
				/* Artnet初始化 */	
				addr[3] = (s_eth_data.netif.ip_addr.addr & 0xff000000) >> 24;
				addr[2] = (s_eth_data.netif.ip_addr.addr & 0x00ff0000) >> 16;
				addr[1] = (s_eth_data.netif.ip_addr.addr & 0x0000ff00) >> 8;
				addr[0] = (s_eth_data.netif.ip_addr.addr & 0x000000ff);
				artnet_init(addr); 
                
				data_center_read_config_data(SYS_CONFIG_ETH_ARTNET_UNIVERSE, &universe);
				artnet_set_universe_address(universe);
                /* sACN初始化 */
				data_center_read_config_data(SYS_CONFIG_ETH_SACN_UNIVERSE, &universe);
                E131_Init(universe);
				sacn_set_universe(universe);
                /* 启动sACN任务 */            
                osThreadResume(s_eth_data.sacn_thread);
            }
        }
		eth_task_static_size = osThreadGetStackSpace(app_eth_task_id);
		eth_check_link(&s_eth_data.netif);		
        osDelay(100);
    }
}

void netif_config_network(struct netif *netif, uint8_t start)
{
    ip_addr_t ipaddr = {0}, netmask = {0}, gateway = {0};
	
	if(start == ETH_STATIC_START)
	{
		IP_ADDR4(&ipaddr,  s_eth_data.ipaddr[0],  s_eth_data.ipaddr[1],  s_eth_data.ipaddr[2],  s_eth_data.ipaddr[3]);
		IP_ADDR4(&netmask, s_eth_data.netmask[0], s_eth_data.netmask[1], s_eth_data.netmask[2], s_eth_data.netmask[3]);
		IP_ADDR4(&gateway, s_eth_data.gateway[0], s_eth_data.gateway[1], s_eth_data.gateway[2], s_eth_data.gateway[3]);
		netif_set_addr(netif, &ipaddr, &netmask, &gateway);  
		if(start == ETH_STATIC_START)
		{
			artnet_set_ip_addr(ipaddr); 
		}	
	}
	else
	{
		netif_set_addr(netif, &netif->ip_addr, &netif->netmask, &netif->gw); 
		artnet_set_ip_addr(netif->ip_addr); 
	}
}

/**
 * @brief 获取子网掩码
 * 
 * @return uint32_t 
 */
uint32_t netif_get_netmask(void)
{
    return s_eth_data.netif.netmask.addr;
}

/**
 * @brief 获取IP地址
 * 
 * @return uint32_t 
 */
uint32_t netif_get_ip_addr(void)
{
   return s_eth_data.netif.ip_addr.addr;
}

/**
 * @brief 获取DHCP子网掩码
 * 
 * @return uint32_t 
 */
uint32_t netif_get_dhcp_netmask(void)
{
    return s_eth_data.netif.netmask.addr;
}

/**
 * @brief 获取DHCP IP地址
 * 
 * @return uint32_t 
 */
uint32_t netif_get_dhcp_ip_addr(void)
{
   return s_eth_data.netif.ip_addr.addr;
}

/**
 * @brief 获取DHCP 网关
 * 
 * @return uint32_t 
 */
uint32_t netif_get_dhcp_gateway_addr(void)
{
   return s_eth_data.netif.ip_addr.addr;
}

/**
 * @brief 获取广播地址
 * 
 * @return uint32_t 
 */
uint32_t netif_get_brdcast_addr(void)
{
   return (netif_get_ip_addr() & netif_get_netmask()) | (0xFFFFFFFF & (~netif_get_netmask()));
}

/**
 * @brief 获取网卡的mac地址
 * 
 * @param[in]hwaddr         MAC地址
 */
void netif_get_mac_addr(uint8_t* hwaddr)
{
	hal_netif_get_mac_addr(hwaddr);
}

uint8_t app_eth_get_lwip_state(void)
{
	return s_eth_data.eth.pluged;
}

static void sAcn_thread_entry(void* arg)
{
    while (1)
    {
        E131_Recv_Data();
        osDelay(1);
    }
}

static void lwip_init(void)
{    
    tcpip_init(NULL, NULL);        
	app_eth_init();	
}

static void app_eth_init(void)
{
    ip_addr_t ipaddr = {0}, netmask = {0}, gateway = {0};
	
//	if(s_eth_data.eth_start == ETH_STATIC_START)
//	{
		IP_ADDR4(&ipaddr,  s_eth_data.ipaddr[0], s_eth_data.ipaddr[1], s_eth_data.ipaddr[2], s_eth_data.ipaddr[3]);
		IP_ADDR4(&netmask, s_eth_data.netmask[0], s_eth_data.netmask[1], s_eth_data.netmask[2], s_eth_data.netmask[3]);
		IP_ADDR4(&gateway, s_eth_data.gateway[0], s_eth_data.gateway[1], s_eth_data.gateway[2], s_eth_data.gateway[3]);
		
		netif_add(&s_eth_data.netif, &ipaddr, &netmask, &gateway, NULL, &ethernetif_init, &tcpip_input);
		netif_set_default(&s_eth_data.netif);
		
		if (netif_is_link_up(&s_eth_data.netif))
		{
			netif_set_up(&s_eth_data.netif);/* When the netif is fully configured this function must be called */
			update();
		}
		else
		{
			netif_set_down(&s_eth_data.netif);/* When the netif link is down this function must be called */
			update();
		} 
		if(s_eth_data.eth_start == ETH_DHCP_START)
		{
			s_eth_data.eth_dhcp_start = DHCP_START;
			eth_dhcp_handle(&s_eth_data.netif);
		}
//	}
}

static void  eth_check_link(struct netif* netif)
{
    uint8_t link_state = hal_eth_get_linked_state(0);
    
    if (s_eth_data.eth.pluged != link_state)
    {
        if (!s_eth_data.eth.pluged && link_state)
        {
			netif_set_link_up(netif);
			netif_config_network(netif, s_eth_data.eth_start);
			netif_set_up(netif);
			if(s_eth_data.eth_start == ETH_DHCP_START)
			{
				s_eth_data.eth_dhcp_start = DHCP_START;
			}
        }        
        else if (s_eth_data.eth.pluged && !link_state)
        {
			netif_set_link_down(netif);
			netif_set_down(netif);
			if(s_eth_data.eth_start == ETH_DHCP_START)
			{
				s_eth_data.eth_dhcp_start = DHCP_LINK_DOWN;
			}
        }
        s_eth_data.eth.pluged = link_state;
        update();
    }
	if(s_eth_data.eth_start != s_last_eth_start)
	{
		s_last_eth_start = s_eth_data.eth_start;
		if(s_eth_data.eth_start == ETH_DHCP_START)
		{
			netif_set_link_up(netif);
			netif_config_network(netif, s_eth_data.eth_start);
			netif_set_up(netif);
			s_eth_data.eth_dhcp_start = DHCP_START;
		}
		else
		{
//			dhcp_release(netif);
			dhcp_stop(netif);
			s_eth_data.eth_dhcp_start = DHCP_OFF;
			netif_set_link_up(netif);
			netif_config_network(netif, s_eth_data.eth_start);
			netif_set_up(netif);
		}
		update();
	}
	
	eth_dhcp_handle(netif);
}

static void eth_dhcp_handle(struct netif *netif)
{
	ip_addr_t ipaddr;
    ip_addr_t netmask;
    ip_addr_t gw;
    struct dhcp *dhcp;

    switch (s_eth_data.eth_dhcp_start)
    {
        case DHCP_START:
            ip_addr_set_zero_ip4(&netif->ip_addr);
            ip_addr_set_zero_ip4(&netif->netmask);
            ip_addr_set_zero_ip4(&netif->gw);
            s_eth_data.eth_dhcp_start = DHCP_WAIT_ADDRESS;
            (void)dhcp_start(netif);
            break;
        case DHCP_WAIT_ADDRESS:
            if (0U != dhcp_supplied_address(netif))
            {
                s_eth_data.eth_dhcp_start = DHCP_ADDRESS_ASSIGNED;
				netif_config_network(netif, s_eth_data.eth_start);
//                extern EventGroupHandle_t network_events;   
//                xEventGroupSetBits(network_events, 1 << 0);
                //BSP_LED_On(LED_BLUE);
            }
            else
            {
                dhcp = (struct dhcp*)netif_get_client_data(netif, LWIP_NETIF_CLIENT_DATA_INDEX_DHCP);
                /* DHCP timeout */
                if (dhcp->tries > 4)
                {
                    s_eth_data.eth_dhcp_start = DHCP_TIMEOUT;
//					dhcp_release(netif);
					dhcp_stop(netif);
                    /* Static address used */
                    IP_ADDR4(&ipaddr, IP_ADDR0, IP_ADDR1, IP_ADDR2, IP_ADDR3);
                    IP_ADDR4(&netmask, NETMASK_ADDR0, NETMASK_ADDR1, NETMASK_ADDR2, NETMASK_ADDR3);
                    IP_ADDR4(&gw, GW_ADDR0, GW_ADDR1, GW_ADDR2, GW_ADDR3);
                    netif_set_addr(netif, &ipaddr, &netmask, &gw);
                    //BSP_LED_On(LED_BLUE);
                }
            }
            break;
        case DHCP_LINK_DOWN:
            /* Stop DHCP */
//			dhcp_release(netif);
			dhcp_stop(netif);
            s_eth_data.eth_dhcp_start = DHCP_OFF;
            break;
        default:
            break;
    }
}

//static void rth_500ms_callback (void *argument)
//{
//	if(s_eth_data.eth_start == ETH_DHCP_START)
//	{
//		if(s_lwip_count++ > 120)
//		{
//			s_lwip_count = 0;
//			dhcp_coarse_tmr();
//		}
//		dhcp_fine_tmr();
//	}
//}
