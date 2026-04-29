/**
  ******************************************************************************
  * @file    lwipopts.h
  * @author  MCD Application Team & suozhang
  * @version V2.0.0
  * @date    2019��4��3��15:40:16
  * @brief   lwIP Options Configuration.
  *          This file is based on Utilities\lwip_v2.1.2\src\include\lwip\opt.h 
  *          and contains the lwIP configuration for the STM32H743 demonstration.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2013 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

#ifndef __LWIPOPTS_H__
#define __LWIPOPTS_H__

/**
 * NO_SYS==1: Provides VERY minimal functionality. Otherwise,
 * use lwIP facilities.
 */
#define NO_SYS                  0 /* ʹ�� FreeRTOS ���� lwip */ 

/**
 * SYS_LIGHTWEIGHT_PROT==1: enable inter-task protection (and task-vs-interrupt
 * protection) for certain critical regions during buffer allocation, deallocation
 * and memory allocation and deallocation.
 * ATTENTION: This is required when using lwIP from more than one context! If
 * you disable this, you must be sure what you are doing!
 */
#define SYS_LIGHTWEIGHT_PROT    1 /* ʹ�� rtos ���ٽ������� lwip ���̣߳��Լ��ؼ����� */

/**
 * LWIP_NETCONN==1: Enable Netconn API (require to use api_lib.c)
 */
#define LWIP_NETCONN            1 

/**
 * LWIP_IGMP==1: Turn on IGMP module.
 */
#define LWIP_IGMP               1

/**
 * LWIP_ICMP==1: Enable ICMP module inside the IP stack.
 * Be careful, disable that make your product non-compliant to RFC1122
 */
#define LWIP_ICMP               1 /* Enable ICMP module inside the IP stack.such as ping ... */ 

/**
 * LWIP_HAVE_LOOPIF==1: Support loop interface (127.0.0.1).
 * This is only needed when no real netifs are available. If at least one other
 * netif is available, loopback traffic uses this netif.
 */
#define LWIP_HAVE_LOOPIF        0 /* DisEnable loop interface (127.0.0.1). */ 

/** Define the byte order of the system.
 * Needed for conversion of network data to host byte order.
 * Allowed values: LITTLE_ENDIAN and BIG_ENDIAN
 */
#ifndef BYTE_ORDER
#define BYTE_ORDER  LITTLE_ENDIAN /* С�ֽ��򡢵��ֽ��� */ 
#endif

/* ---------- Debug options ---------- */

#define LWIP_DEBUG                  
#define DNS_SERVER_ADDRESS(ipaddr)  (ip4_addr_set_u32(ipaddr, ipaddr_addr("223.5.5.5"))) /* resolver1.opendns.com */    //208.67.222.222
//#define LWIP_DBG_TYPES_ON           (LWIP_DBG_ON|LWIP_DBG_TRACE|LWIP_DBG_STATE|LWIP_DBG_LEVEL_ALL) // LWIP_DBG_HALT ���������� debug while(1)

#ifdef LWIP_DEBUG

	#define SYS_DEBUG                   LWIP_DBG_OFF
	#define ETHARP_DEBUG                LWIP_DBG_OFF
	#define PPP_DEBUG                   LWIP_DBG_OFF
	#define MEM_DEBUG                   LWIP_DBG_OFF
	#define MEMP_DEBUG                  LWIP_DBG_OFF
	#define PBUF_DEBUG                  LWIP_DBG_OFF
	#define API_LIB_DEBUG               LWIP_DBG_OFF
	#define API_MSG_DEBUG               LWIP_DBG_OFF
	#define TCPIP_DEBUG                 LWIP_DBG_OFF
	#define NETIF_DEBUG                 LWIP_DBG_OFF
	#define SOCKETS_DEBUG               LWIP_DBG_OFF
	#define DNS_DEBUG                   LWIP_DBG_OFF
	#define AUTOIP_DEBUG                LWIP_DBG_OFF
	#define DHCP_DEBUG                  LWIP_DBG_OFF
	#define IP_DEBUG                    LWIP_DBG_OFF
	#define IP_REASS_DEBUG              LWIP_DBG_OFF
	#define ICMP_DEBUG                  LWIP_DBG_OFF
	#define IGMP_DEBUG                  LWIP_DBG_OFF
	#define UDP_DEBUG                   LWIP_DBG_OFF
	#define TCP_DEBUG                   LWIP_DBG_OFF
	#define TCP_INPUT_DEBUG             LWIP_DBG_OFF
	#define TCP_OUTPUT_DEBUG            LWIP_DBG_OFF
	#define TCP_RTO_DEBUG               LWIP_DBG_OFF
	#define TCP_CWND_DEBUG              LWIP_DBG_OFF
	#define TCP_WND_DEBUG               LWIP_DBG_OFF
	#define TCP_FR_DEBUG                LWIP_DBG_OFF
	#define TCP_QLEN_DEBUG              LWIP_DBG_OFF
	#define TCP_RST_DEBUG               LWIP_DBG_OFF

#endif

/* ---------- Memory options ---------- */
/* MEM_ALIGNMENT: should be set to the alignment of the CPU for which
   lwIP is compiled. 4 byte alignment -> define MEM_ALIGNMENT to 4, 2
   byte alignment -> define MEM_ALIGNMENT to 2. */
#define MEM_ALIGNMENT           4  /* �ֽڶ��䣬32λCPU ���� Ϊ4 */

/* if MEMP_OVERFLOW_CHECK is turned on, we reserve some bytes at the beginning
 * and at the end of each element, initialize them as 0xcd and check
 * them later. */
/* If MEMP_OVERFLOW_CHECK is >= 2, on every call to memp_malloc or memp_free,
 * every single element in each pool is checked!
 * This is VERY SLOW but also very helpful. */
#define MEMP_OVERFLOW_CHECK         1 /* ���� ��̬�ڴ��pool ��У���� */

/**
 * Set this to 1 if you want to free PBUF_RAM pbufs (or call mem_free()) from
 * interrupt context (or another context that doesn't allow waiting for a
 * semaphore).
 * If set to 1, mem_malloc will be protected by a semaphore and SYS_ARCH_PROTECT,
 * while mem_free will only use SYS_ARCH_PROTECT. mem_malloc SYS_ARCH_UNPROTECTs
 * with each loop so that mem_free can run.
 *
 * ATTENTION: As you can see from the above description, this leads to dis-/
 * enabling interrupts often, which can be slow! Also, on low memory, mem_malloc
 * can need longer.
 *
 * If you don't want that, at least for NO_SYS=0, you can still use the following
 * functions to enqueue a deallocation call which then runs in the tcpip_thread
 * context:
 * - pbuf_free_callback(p);
 * - mem_free_callback(m);
 */
#define LWIP_ALLOW_MEM_FREE_FROM_OTHER_CONTEXT 1 /* ʹ��RTOS���ź������ٽ��������ڴ�ķ����Լ��ͷŵȵ� */ 

/**
 * MEMP_MEM_MALLOC==1: Use mem_malloc/mem_free instead of the lwip pool allocator.
 * Especially useful with MEM_LIBC_MALLOC but handle with care regarding execution
 * speed (heap alloc can be much slower than pool alloc) and usage from interrupts
 * (especially if your netif driver allocates PBUF_POOL pbufs for received frames
 * from interrupt)!
 * ATTENTION: Currently, this uses the heap for ALL pools (also for private pools,
 * not only for internal pools defined in memp_std.h)!
 */
#define MEMP_MEM_MALLOC             0 /* ʹ�ö�̬�ڴ�� pool �ķ�ʽ �� TCP �Լ� UDP ���ƿ飬�ں��ڳ�ʼ����ʱ��Ϊÿ�����ݽṹ��ʼ������һ����pool */ 

/**
 * suozhang thinking 2018��1��23��11:10:42 �ο� ������ľ Ƕ��ʽ������Щ�� P74
 * MEM_LIBC_MALLOC = 1��ֱ��ʹ��C���е�malloc��free�����䶯̬�ڴ棻����ʹ��LWIP�Դ���mem_malloc��mem_free�Ⱥ�����
 * MEMP_MEM_MALLOC = 1���� memp.c �е�ȫ������Ҳ���ᱻ���룬����̬�ڴ��pool������Բ���ʹ��
 * MEM_USE_POOLS   = 1���� mem.c �� ��ȫ������Ҳ���ᱻ���룬���ַ�ʽ��Ҫ���ڴ�صķ�ʽʵ�֣���Ҫ�û��Լ�ʵ�֣��Ƚ��鷳������û�����������
 * 
 * ����ʹ���� lwip Ĭ�ϵ� ��̬�ڴ�أ�pool���� ��̬�ڴ�� mem �ķ�ʽ��MEM_SIZE ���Ƕ�̬�ڴ�ѵĴ�С
 * LWIP ���ĵ� �ڴ� ���� MEM_SIZE ��̬�ڴ�� + MEMP ��̬�ڴ��pool ���ĵ��ڴ�
 * 
*/

/* MEM_SIZE: the size of the heap memory. If the application will send
a lot of data that needs to be copied, this should be set high. */
#define MEM_SIZE                (48*1024) /* Ӧ�ó��� ���ʹ������� ��Ҫ���Ƶģ����ֵӦ�����ô�һ�� */

/* Relocate the LwIP RAM heap pointer */
#define LWIP_RAM_HEAP_POINTER    (0x20048000)

/* MEMP_NUM_PBUF: the number of memp struct pbufs. If the application
   sends a lot of data out of ROM (or other static memory), this
   should be set high. */
#define MEMP_NUM_PBUF           16 /* Ӧ�ó��� ���ʹ������� �Ǵ���ROM �ģ����� ��ҳ�����ֵӦ�����ô�һ�� */

/**
 * MEMP_NUM_NETCONN: the number of struct netconns.
 * (only needed if you use the sequential API, like api_lib.c)
 */
#define MEMP_NUM_NETCONN        5 /* �ϲ�API ����ʹ�� NETCONN �ĸ��������� UDP ��TCP */

/* MEMP_NUM_UDP_PCB: the number of UDP protocol control blocks. One
   per active UDP "connection". */
#define MEMP_NUM_UDP_PCB        3 /* �ϲ�API ����ʹ�� UDP �ĸ�����UDP ���ӽ϶�ʱ Ӧ�������ֵ */

/* MEMP_NUM_TCP_PCB: the number of simulatenously active TCP
   connections. */
#define MEMP_NUM_TCP_PCB        6 /* �ϲ�API ����ʹ�� TCP �ĸ�����UDP ���ӽ϶�ʱ Ӧ�������ֵ */

/* MEMP_NUM_TCP_PCB_LISTEN: the number of listening TCP
   connections. */
#define MEMP_NUM_TCP_PCB_LISTEN 3 /* �ϲ�API ����ʹ�� TCP �����ĸ�����TCP �������϶�ʱ Ӧ�������ֵ */

/* MEMP_NUM_TCP_SEG: the number of simultaneously queued TCP
   segments. */
#define MEMP_NUM_TCP_SEG        TCP_SND_QUEUELEN /* TCP�ں˵Ļ��屨�Ķθ�������Ӧ�����ݽ϶�ʱӦ�����Ӹ�ֵ */

/**
 * MEMP_NUM_SYS_TIMEOUT: the number of simultaneously active timeouts.
 * The default number of timeouts is calculated here for all enabled modules.
 * The formula expects settings to be either '0' or '1'.
 */
#define MEMP_NUM_SYS_TIMEOUT    10 /* ͬʱ����� ��ʱ���� */


/* ---------- Pbuf options ---------- */

/* ������ struct pbuf ���͵�pbuf�ͻ��õ����ں˲���ʹ������pbuf��Ӧ�û�������������ֲ��ʱ�����ʹ�ã����ܿ��ٷ�������ͷ� */

/* PBUF_POOL_SIZE: the number of buffers in the pbuf pool. */
#define PBUF_POOL_SIZE          10 /* pool ���͵� PUBF �ĸ������������������ʹ�ø����͵�PBUF���ݰ������Ӵ��ֵ  */

/* PBUF_POOL_BUFSIZE: the size of each pbuf in the pbuf pool. */
#define PBUF_POOL_BUFSIZE       LWIP_MEM_ALIGN_SIZE(TCP_MSS+40+PBUF_LINK_ENCAPSULATION_HLEN+PBUF_LINK_HLEN)

/* PBUF_LINK_HLEN: the number of bytes that should be allocated for a
   link level header. */
	 //#define PBUF_LINK_HLEN              16 /* ��ʱ��֪�� Ϊʲô ��ʱ���� 2018��1��23��13:42:14 */

/* LWIP_SUPPORT_CUSTOM_PBUF == 1: to pass directly MAC Rx buffers to the stack 
   no copy is needed */
#define LWIP_SUPPORT_CUSTOM_PBUF      1

/* ---------- TCP options ---------- */

/**
 * LWIP_TCP==1: Turn on TCP.
 */
#define LWIP_TCP                1

/**
 * TCP_TTL: Default Time-To-Live value.
 */
#define TCP_TTL                 IP_DEFAULT_TTL /* IP ���ݰ��е�TTL ��ֵ */

/* Controls if TCP should queue segments that arrive out of
   order. Define to 0 if your device is low on memory. */
#define TCP_QUEUE_OOSEQ         ( LWIP_TCP )  /* TCP�Ƿ񻺳���յ��������Ķ� */

/* TCP Maximum segment size. */
#define TCP_MSS                 (1500 - 40)	  /* TCP_MSS = (Ethernet MTU - IP header size - TCP header size) TCP����Ķδ�С */

/* TCP sender buffer space (bytes). */
#define TCP_SND_BUF             (4*TCP_MSS)   /* TCP ���ͻ�������С�������ֵ��������TCP���� */

/* TCP receive window. */
#define TCP_WND                 (4*TCP_MSS)   /* TCP ���ʹ��ڴ�С�������ֵ��������TCP���� */


/* ---------- DHCP options ---------- */
/* Define LWIP_DHCP to 1 if you want DHCP configuration of
   interfaces. DHCP is not implemented in lwIP 0.5.1, however, so
   turning this on does currently not work. */
#define LWIP_DHCP               1


/* ---------- UDP options ---------- */
#define LWIP_UDP                1
#define UDP_TTL                 255


/* ---------- Statistics options ---------- */
#define LWIP_STATS 0
#define LWIP_PROVIDE_ERRNO 1

/* ---------- link callback options ---------- */
/* LWIP_NETIF_LINK_CALLBACK==1: Support a callback function from an interface
 * whenever the link changes (i.e., link down)
 */
#define LWIP_NETIF_LINK_CALLBACK        1

/* Enable DNS */
#define LWIP_DNS                        1

/* 
The STM32F4x7 allows computing and verifying the IP, UDP, TCP and ICMP checksums by hardware:
 - To use this feature let the following define uncommented.
 - To disable it and process by CPU comment the  the checksum.
*/
#define CHECKSUM_BY_HARDWARE 


#ifdef CHECKSUM_BY_HARDWARE
  /* CHECKSUM_GEN_IP==0: Generate checksums by hardware for outgoing IP packets.*/
  #define CHECKSUM_GEN_IP                 0
  /* CHECKSUM_GEN_UDP==0: Generate checksums by hardware for outgoing UDP packets.*/
  #define CHECKSUM_GEN_UDP                0
  /* CHECKSUM_GEN_TCP==0: Generate checksums by hardware for outgoing TCP packets.*/
  #define CHECKSUM_GEN_TCP                0 
  /* CHECKSUM_CHECK_IP==0: Check checksums by hardware for incoming IP packets.*/
  #define CHECKSUM_CHECK_IP               0
  /* CHECKSUM_CHECK_UDP==0: Check checksums by hardware for incoming UDP packets.*/
  #define CHECKSUM_CHECK_UDP              0
  /* CHECKSUM_CHECK_TCP==0: Check checksums by hardware for incoming TCP packets.*/
  #define CHECKSUM_CHECK_TCP              0
  /* CHECKSUM_CHECK_ICMP==0: Check checksums by hardware for incoming ICMP packets.*/  
  #define CHECKSUM_GEN_ICMP               0
#else
  /* CHECKSUM_GEN_IP==1: Generate checksums in software for outgoing IP packets.*/
  #define CHECKSUM_GEN_IP                 1
  /* CHECKSUM_GEN_UDP==1: Generate checksums in software for outgoing UDP packets.*/
  #define CHECKSUM_GEN_UDP                1
  /* CHECKSUM_GEN_TCP==1: Generate checksums in software for outgoing TCP packets.*/
  #define CHECKSUM_GEN_TCP                1
  /* CHECKSUM_CHECK_IP==1: Check checksums in software for incoming IP packets.*/
  #define CHECKSUM_CHECK_IP               1
  /* CHECKSUM_CHECK_UDP==1: Check checksums in software for incoming UDP packets.*/
  #define CHECKSUM_CHECK_UDP              1
  /* CHECKSUM_CHECK_TCP==1: Check checksums in software for incoming TCP packets.*/
  #define CHECKSUM_CHECK_TCP              1
  /* CHECKSUM_CHECK_ICMP==1: Check checksums by hardware for incoming ICMP packets.*/  
  #define CHECKSUM_GEN_ICMP               1
#endif


#define ETHARP_TRUST_IP_MAC     0
#define IP_REASSEMBLY           0
#define IP_FRAG                 0
#define ARP_QUEUEING            0



/*
   ------------------------------------
   ---------- Socket options ----------
   ------------------------------------
*/
/**
 * LWIP_SOCKET==1: Enable Socket API (require to use sockets.c)
 */
#define LWIP_SOCKET                     1



/**
 * LWIP_TCP_KEEPALIVE==1: Enable TCP_KEEPIDLE, TCP_KEEPINTVL and TCP_KEEPCNT
 * options processing. Note that TCP_KEEPIDLE and TCP_KEEPINTVL have to be set
 * in seconds. (does not require sockets.c, and will affect tcp.c)
 * SuoZhang,2017��4��21��13:36:45��add
 */
#define  TCP_KEEPIDLE_DEFAULT     5000UL  	 // 5��������˫���������ݣ����𱣻�̽�⣨��ֵĬ��Ϊ2Сʱ��
#define  TCP_KEEPINTVL_DEFAULT    1000UL		 // ÿ1�뷢��һ�α���̽�⣨��ֵĬ��Ϊ75S��
//�������������һ������5�α���̽����������5�����Է����޻�Ӧ�����ʾ�����쳣���ں˹ر����ӣ�������err�ص����û�����
#define  TCP_KEEPCNT_DEFAULT      5UL  			 
#define  TCP_MAXIDLE  TCP_KEEPCNT_DEFAULT * TCP_KEEPINTVL_DEFAULT



#define MQTT_DEBUG                  


/*
   ---------------------------------
   ---------- OS options ----------
   ---------------------------------
*/


#define TCPIP_THREAD_NAME              "tcpip_thread"
#define TCPIP_THREAD_STACKSIZE          (5 * 1024)
#define TCPIP_MBOX_SIZE                 12
#define DEFAULT_UDP_RECVMBOX_SIZE       12
#define DEFAULT_TCP_RECVMBOX_SIZE       12
#define DEFAULT_ACCEPTMBOX_SIZE         12
#define DEFAULT_THREAD_STACKSIZE        512
#define TCPIP_THREAD_PRIO               (10)


#endif /* __LWIPOPTS_H__ */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
