/**
 * @file hal_eth.c
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



#include "cmsis_os2.h"
#include "hc32_ddl.h"
#include "lwip/netif.h"
#include "lwip/etharp.h"

#include "hal_irq.h"
#include "hal_eth.h"



#define ETH_IRQ_PRIPROTY        DDL_IRQ_PRIORITY_07
#define CHIP_ID_ADDRESS	        0x40010450
/**********************
 *  STATIC PROTOTYPES
 **********************/
static void          eth_gpio_init(void);
static void          eth_power_reset(void);
static void          eth_isr_callback(void);
static err_t         eth_low_level_output(struct netif *netif, struct pbuf *p);
static struct pbuf*  eth_low_level_input(struct netif* p_netif);
static void          eth_input_thread(void* arg);
static void          eth_netif_link_cb(struct netif *netif);
static void          low_level_init(struct netif *netif);
/**********************
 *  STATIC VARIABLES
 **********************/
static stc_eth_handle_t EthHandle;                          /* 以太网句柄     */
static stc_eth_dma_desc_t EthDmaTxDscrTab[ETH_TXBUF_NUMBER];/* 发送DMA描述符  */
static stc_eth_dma_desc_t EthDmaRxDscrTab[ETH_RXBUF_NUMBER];/* 接收DMA描述符  */
static uint8_t EthTxBuff[ETH_TXBUF_NUMBER][ETH_TXBUF_SIZE]; /* 发送缓冲区     */
static uint8_t EthRxBuff[ETH_RXBUF_NUMBER][ETH_RXBUF_SIZE]; /* 接收缓冲区     */
static osSemaphoreId_t rx_semphr;                           /* 接收完成信号量 */

static const osThreadAttr_t eth_input_thread_attr = 
{ 
    .name = "app input eth", 
    .priority = osPriorityBelowNormal, 
    .stack_size = 1024 * 4
};

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * @brief 以太网硬件初始化
 * 
 * @return int 0是成功 其他失败
 */
int  hal_eth_init(void)
{
    stc_eth_init_t stcEthInit;
    stc_irq_signin_config_t stcIrqSigninCfg;
	uint8_t chip_id[12] = {0};
    int ret = 0;

    eth_gpio_init();    /* GPIO初始化 */
	eth_power_reset();  /* 电源复位 */

    /* 打开以太网时钟, 复位以太网时钟 */
    PWC_Fcg1PeriphClockCmd(PWC_FCG1_ETHER, Enable);
    (void)ETH_DeInit();
    (void)ETH_CommStructInit(&EthHandle.stcCommInit);
    (void)ETH_StructInit(&stcEthInit);

    /* 设置MAC地址, 芯片ID的后几位 */
     memcpy(chip_id, (const void *)CHIP_ID_ADDRESS, 12);
    EthHandle.stcCommInit.au8MACAddr[0] = chip_id[11];
    EthHandle.stcCommInit.au8MACAddr[1] = chip_id[10];
    EthHandle.stcCommInit.au8MACAddr[2] = chip_id[9];
    EthHandle.stcCommInit.au8MACAddr[3] = chip_id[8];
    EthHandle.stcCommInit.au8MACAddr[4] = chip_id[7];
    EthHandle.stcCommInit.au8MACAddr[5] = chip_id[6];

#if HAL_USE_INTERFACE_RMII    
    EthHandle.stcCommInit.u32MediaInterface  = ETH_MAC_MEDIA_INTERFACE_RMII;
#else
    EthHandle.stcCommInit.u32MediaInterface  = ETH_MAC_MEDIA_INTERFACE_MII;
#endif
    EthHandle.stcCommInit.u32Speed = ETH_MAC_SPEED_100M; 
    EthHandle.stcCommInit.u32DuplexMode = ETH_MAC_MODE_FULLDUPLEX; 
    EthHandle.stcCommInit.u32RxMode = ETH_RX_MODE_INTERRUPT; 
	stcEthInit.stcMacInit.u32MulticastFrameFilter = ETH_MAC_MULTICASTFRAME_FILTER_NONE;
    if (Ok != ETH_Init(&EthHandle, &stcEthInit))
    {
        ret = -1;
    }
    
    /* Initialize Tx Descriptors list: Chain Mode */
    (void)ETH_DMA_TxDescListInit(&EthHandle, EthDmaTxDscrTab, &EthTxBuff[0][0], ETH_TXBUF_NUMBER);
    /* Initialize Rx Descriptors list: Chain Mode  */
    (void)ETH_DMA_RxDescListInit(&EthHandle, EthDmaRxDscrTab, &EthRxBuff[0][0], ETH_RXBUF_NUMBER);

    /* 注册中断函数 */
    stcIrqSigninCfg.enIntSrc = INT_ETH_GLB_INT;
    stcIrqSigninCfg.enIRQn = Int031_IRQn;//(IRQn_Type)ETH_IRQN;
    stcIrqSigninCfg.pfnCallback = eth_isr_callback;
    (void)INTC_IrqSignIn(&stcIrqSigninCfg);
    NVIC_ClearPendingIRQ(stcIrqSigninCfg.enIRQn);
    NVIC_SetPriority(stcIrqSigninCfg.enIRQn, ETH_IRQ_PRIPROTY);
    NVIC_EnableIRQ(stcIrqSigninCfg.enIRQn);
    return ret;
}

/**
 * @brief 启动以太网
 * 
 */
void hal_eth_start(void)
{
    (void)ETH_Start();
}

/**
 * @brief 以太网网卡初始化 
 * 
 * @param[in]netif          网卡指针
 * @return err_t            错误标志
 */
err_t ethernetif_init(struct netif *netif)
{
#if LWIP_NETIF_HOSTNAME
    /* Initialize interface hostname */
    netif->hostname = "lwip";
#endif /* LWIP_NETIF_HOSTNAME */

    netif->name[0] = 'h';
    netif->name[1] = 'd';
    /* We directly use etharp_output() here to save a function call.
     * You can instead declare your own function an call etharp_output()
     * from it if you have to do some checks before sending (e.g. if link
     * is available...) */
    netif->output = &etharp_output;
    netif->linkoutput = &eth_low_level_output;
    /* initialize the hardware */
    low_level_init(netif);
    return (err_t)ERR_OK;
}


bool hal_eth_get_linked_state(uint8_t port)
{
    uint16_t phy0_reg = 0;
    uint16_t phy1_reg = 0;

    (void)port;
    hal_read_phy_reg(0x02, PHY_BSR, &phy0_reg);
    hal_read_phy_reg(0x03, PHY_BSR, &phy1_reg);
    if (phy0_reg != 0U && phy1_reg != 0 && phy0_reg != 0xFFFFU)
    {
        if ((phy0_reg & PHY_LINK_STATUS) || (phy1_reg & PHY_LINK_STATUS))
        {
            return true;
        }
        
        if (!(phy0_reg & PHY_LINK_STATUS) && !(phy1_reg & PHY_LINK_STATUS) )
        {
            return false;
        }
    }

    return false;
}

void netif_set_mac_addr(struct netif* p_netif, uint8_t mac[])
{
    EthHandle.stcCommInit.au8MACAddr[0] = mac[0];
    EthHandle.stcCommInit.au8MACAddr[1] = mac[1];
    EthHandle.stcCommInit.au8MACAddr[2] = mac[2];
    EthHandle.stcCommInit.au8MACAddr[3] = mac[3];
    EthHandle.stcCommInit.au8MACAddr[4] = mac[4];
    EthHandle.stcCommInit.au8MACAddr[5] = mac[5];

    p_netif->hwaddr[0] = (EthHandle.stcCommInit).au8MACAddr[0];
    p_netif->hwaddr[1] = (EthHandle.stcCommInit).au8MACAddr[1];
    p_netif->hwaddr[2] = (EthHandle.stcCommInit).au8MACAddr[2];
    p_netif->hwaddr[3] = (EthHandle.stcCommInit).au8MACAddr[3];
    p_netif->hwaddr[4] = (EthHandle.stcCommInit).au8MACAddr[4];
    p_netif->hwaddr[5] = (EthHandle.stcCommInit).au8MACAddr[5];    

    ETH_MACADDR_SetAddress(ETH_MAC_ADDRESS0, (EthHandle.stcCommInit).au8MACAddr);    
}

void hal_netif_get_mac_addr(uint8_t *mac)
{
	mac[0] = EthHandle.stcCommInit.au8MACAddr[0];
	mac[1] = EthHandle.stcCommInit.au8MACAddr[1];
	mac[2] = EthHandle.stcCommInit.au8MACAddr[2];
	mac[3] = EthHandle.stcCommInit.au8MACAddr[3];
	mac[4] = EthHandle.stcCommInit.au8MACAddr[4];
	mac[5] = EthHandle.stcCommInit.au8MACAddr[5];
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * @brief 以太网底层接收函数
 * 
 * @param[in]p_netif        网卡
 * @return struct pbuf*     数据块指针
 */
static struct pbuf* eth_low_level_input(struct netif* p_netif)
{
    struct pbuf *p = NULL;
    struct pbuf *q;
    uint32_t len;
    uint8_t *rxBuffer;
    __IO stc_eth_dma_desc_t *DmaRxDesc;
    uint32_t byteCnt;
    uint32_t bufferOffset;
    uint32_t payloadOffset;
    uint32_t i;

    /* Get received frame */
    if (Ok != ETH_DMA_GetReceiveFrame(&EthHandle))
    {
        return NULL;
    }

    /* Obtain the size of the packet */
    len = (EthHandle.stcRxFrame).u32Length;
    rxBuffer = (uint8_t *)(EthHandle.stcRxFrame).u32Buffer;
    if (len > 0UL)
    {
        /* Allocate a pbuf chain of pbufs from the Lwip buffer pool */
        p = pbuf_alloc(PBUF_RAW, (uint16_t)len, PBUF_POOL);
    }

    if (p != NULL)
    {
        DmaRxDesc = (EthHandle.stcRxFrame).pstcFSDesc;
        bufferOffset = 0UL;
        for (q = p; q != NULL; q = q->next)
        {
            byteCnt = q->len;
            payloadOffset = 0UL;

            /* Check if the length of bytes to copy in current pbuf is bigger than Rx buffer size */
            while ((byteCnt + bufferOffset) > ETH_RXBUF_SIZE)
            {
                /* Copy data to pbuf */
                (void)memcpy((uint8_t *)&(((uint8_t *)q->payload)[payloadOffset]), (uint8_t *)&(rxBuffer[bufferOffset]), (ETH_RXBUF_SIZE - bufferOffset));
                /* Point to next descriptor */
                DmaRxDesc = (stc_eth_dma_desc_t *)(DmaRxDesc->u32Buffer2NextDescAddr);
                rxBuffer = (uint8_t *)(DmaRxDesc->u32Buffer1Addr);
                byteCnt = byteCnt - (ETH_RXBUF_SIZE - bufferOffset);
                payloadOffset = payloadOffset + (ETH_RXBUF_SIZE - bufferOffset);
                bufferOffset = 0UL;
            }

            /* Copy remaining data in pbuf */
            (void)memcpy((uint8_t *)&(((uint8_t *)q->payload)[payloadOffset]), (uint8_t *)&(rxBuffer[bufferOffset]), byteCnt);
            bufferOffset = bufferOffset + byteCnt;
        }
    }

    /* Release descriptors to DMA */
    /* Point to first descriptor */
    DmaRxDesc = (EthHandle.stcRxFrame).pstcFSDesc;
    for (i = 0UL; i < (EthHandle.stcRxFrame).u32SegCount; i++)
    {
        DmaRxDesc->u32ControlStatus |= ETH_DMARXDESC_OWN;
        DmaRxDesc = (stc_eth_dma_desc_t *)(DmaRxDesc->u32Buffer2NextDescAddr);
    }
    /* Clear Segment_Count */
    (EthHandle.stcRxFrame).u32SegCount = 0UL;

    /* When Rx Buffer unavailable flag is set, clear it and resume reception */
    if (0UL != (READ_REG32_BIT(M4_ETH->DMA_DMASTSR, ETH_DMA_FLAG_RUS)))
    {
        /* Clear DMA RUS flag */
        WRITE_REG32(M4_ETH->DMA_DMASTSR, ETH_DMA_FLAG_RUS);
        /* Resume DMA reception */
        WRITE_REG32(M4_ETH->DMA_RXPOLLR, 0UL);
    }
    
    return p;
}

/**
 * @brief 以太网底层发送函数
 * 
 * @param[in]netif          网卡
 * @param[in]p              数据
 * @return err_t 
 */
static err_t eth_low_level_output(struct netif *netif, struct pbuf *p)
{
    err_t errval;
    struct pbuf *q;
    uint8_t *txBuffer;
    __IO stc_eth_dma_desc_t *DmaTxDesc;
    uint32_t byteCnt;
    uint32_t frameLength = 0UL;
    uint32_t bufferOffset;
    uint32_t payloadOffset;

    DmaTxDesc = EthHandle.stcTxDesc;
    txBuffer = (uint8_t *)((EthHandle.stcTxDesc)->u32Buffer1Addr);
    bufferOffset = 0UL;

    /* Copy frame from pbufs to driver buffers */
    for (q = p; q != NULL; q = q->next)
    {
        /* If this buffer isn't available, goto error */
        if (0UL != (DmaTxDesc->u32ControlStatus & ETH_DMATXDESC_OWN))
        {
            errval = (err_t)ERR_USE;
            goto error;
        }

        /* Get bytes in current lwIP buffer */
        byteCnt = q->len;
        payloadOffset = 0UL;
        /* Check if the length of data to copy is bigger than Tx buffer size */
        while ((byteCnt + bufferOffset) > ETH_TXBUF_SIZE)
        {
            /* Copy data to Tx buffer*/
            (void)memcpy((uint8_t *)&(txBuffer[bufferOffset]), (uint8_t *)&(((uint8_t *)q->payload)[payloadOffset]), (ETH_TXBUF_SIZE - bufferOffset));
            /* Point to next descriptor */
            DmaTxDesc = (stc_eth_dma_desc_t *)(DmaTxDesc->u32Buffer2NextDescAddr);
            /* Check if the buffer is available */
            if (0UL != (DmaTxDesc->u32ControlStatus & ETH_DMATXDESC_OWN))
            {
                errval = (err_t)ERR_USE;
                goto error;
            }

            txBuffer = (uint8_t *)(DmaTxDesc->u32Buffer1Addr);
            byteCnt = byteCnt - (ETH_TXBUF_SIZE - bufferOffset);
            payloadOffset = payloadOffset + (ETH_TXBUF_SIZE - bufferOffset);
            frameLength = frameLength + (ETH_TXBUF_SIZE - bufferOffset);
            bufferOffset = 0UL;
        }

        /* Copy the remaining bytes */
        (void)memcpy((uint8_t *)&(txBuffer[bufferOffset]), (uint8_t *)&(((uint8_t *)q->payload)[payloadOffset]), byteCnt);
        bufferOffset = bufferOffset + byteCnt;
        frameLength = frameLength + byteCnt;
    }
    /* Prepare transmit descriptors to give to DMA */
    (void)ETH_DMA_SetTransmitFrame(&EthHandle, frameLength);
    errval = (err_t)ERR_OK;

error:
    /* When Transmit Underflow flag is set, clear it and issue a Transmit Poll Demand to resume transmission */
    if (0UL != (READ_REG32_BIT(M4_ETH->DMA_DMASTSR, ETH_DMA_FLAG_UNS)))
    {
        /* Clear DMA UNS flag */
        WRITE_REG32(M4_ETH->DMA_DMASTSR, ETH_DMA_FLAG_UNS);
        /* Resume DMA transmission */
        WRITE_REG32(M4_ETH->DMA_TXPOLLR, 0UL);
    }
    
    return errval;
}

/**
 * @brief 以太网复位
 * 
 */
static void eth_power_reset(void)
{
    GPIO_ResetPins(GPIO_PORT_G, GPIO_PIN_11);
    osDelay(50);
    GPIO_SetPins(GPIO_PORT_G, GPIO_PIN_11);
    osDelay(50);  
}

/**
 * @brief GPIO配置
 * 
 */
static void eth_gpio_init(void)
{
    /* ETH_RST */
    stc_gpio_init_t stcGpioInit;
    (void)GPIO_StructInit(&stcGpioInit);
    stcGpioInit.u16PinDrv     = PIN_DRV_HIGH;
    stcGpioInit.u16PullUp     = PIN_PU_ON;
    stcGpioInit.u16PinDir 	  = PIN_DIR_OUT;
    (void)GPIO_Init(GPIO_PORT_G, GPIO_PIN_11, &stcGpioInit);
	
   /* Configure MII/RMII selection IO for ETH */
#if HAL_USE_INTERFACE_RMII

    /* Ethernet RMII pins configuration */
    /*
        ETH_SMI_MDIO ----------------> PA2
        ETH_SMI_MDC -----------------> PC1
        ETH_RMII_TX_EN --------------> PG11   PB11
        ETH_RMII_TXD0 ---------------> PG13
        ETH_RMII_TXD1 ---------------> PG14
        ETH_RMII_REF_CLK ------------> PA1
        ETH_RMII_CRS_DV -------------> PA7
        ETH_RMII_RXD0 ---------------> PC4
        ETH_RMII_RXD1 ---------------> PC5
        ETH_RMII_RX_ER --------------> PI10
    */
    /* Configure PA1, PA2 and PA7 */
    GPIO_SetFunc(GPIO_PORT_A, (GPIO_PIN_01 | GPIO_PIN_02 | GPIO_PIN_07), GPIO_FUNC_11_ETH ,PIN_SUBFUNC_DISABLE);
    /* Configure PC1, PC4 and PC5 */
    GPIO_SetFunc(GPIO_PORT_C, (GPIO_PIN_01 | GPIO_PIN_04 | GPIO_PIN_05), GPIO_FUNC_11_ETH ,PIN_SUBFUNC_DISABLE);
    /* Configure PG11, PG13 and PG14 */
    GPIO_SetFunc(GPIO_PORT_G, (/*GPIO_PIN_11 |*/ GPIO_PIN_13 | GPIO_PIN_14), GPIO_FUNC_11_ETH ,PIN_SUBFUNC_DISABLE);
    
    GPIO_SetFunc(GPIO_PORT_B, (GPIO_PIN_11), GPIO_FUNC_11_ETH ,PIN_SUBFUNC_DISABLE);  //testing
    /* Configure PI10 */
//    GPIO_SetFunc(GPIO_PORT_I, GPIO_PIN_10, GPIO_FUNC_11_ETH ,PIN_SUBFUNC_DISABLE);
#else
    /* Ethernet MII pins configuration */
    /*
        ETH_SMI_MDIO ----------------> PA2
        ETH_SMI_MDC -----------------> PC1
        ETH_MII_TX_CLK --------------> PB6
        ETH_MII_TX_EN ---------------> PG11
        ETH_MII_TXD0 ----------------> PG13
        ETH_MII_TXD1 ----------------> PG14
        ETH_MII_TXD2 ----------------> PB9
        ETH_MII_TXD3 ----------------> PB8
        ETH_MII_RX_CLK --------------> PA1
        ETH_MII_RX_DV ---------------> PA7
        ETH_MII_RXD0 ----------------> PC4
        ETH_MII_RXD1 ----------------> PC5
        ETH_MII_RXD2 ----------------> PB0
        ETH_MII_RXD3 ----------------> PB1
        ETH_MII_RX_ER ---------------> PI10
        ETH_MII_CRS -----------------> PH2
        ETH_MII_COL -----------------> PH3
    */
    /* Configure PA1, PA2 and PA7 */
    GPIO_SetFunc(GPIO_PORT_A, (GPIO_PIN_01 | GPIO_PIN_02 | GPIO_PIN_07), GPIO_FUNC_11_ETH ,PIN_SUBFUNC_DISABLE);
    /* Configure PB0, PB1, PB6, PB8 and PB9 */
    GPIO_SetFunc(GPIO_PORT_B, (GPIO_PIN_00 | GPIO_PIN_01 | GPIO_PIN_06 | GPIO_PIN_08 | GPIO_PIN_09), GPIO_FUNC_11_ETH ,PIN_SUBFUNC_DISABLE);
    /* Configure PC1, PC4 and PC5 */
    GPIO_SetFunc(GPIO_PORT_C, (GPIO_PIN_01 | GPIO_PIN_04 | GPIO_PIN_05), GPIO_FUNC_11_ETH ,PIN_SUBFUNC_DISABLE);
    /* Configure PG11, PG13 and PG14 */
    GPIO_SetFunc(GPIO_PORT_G, (GPIO_PIN_11 | GPIO_PIN_13 | GPIO_PIN_14), GPIO_FUNC_11_ETH ,PIN_SUBFUNC_DISABLE);
//    /* Configure PH2, PH3 */
    GPIO_SetFunc(GPIO_PORT_H, (GPIO_PIN_02 | GPIO_PIN_03), GPIO_FUNC_11_ETH ,PIN_SUBFUNC_DISABLE);
    /* Configure PI10 */
    GPIO_SetFunc(GPIO_PORT_I, GPIO_PIN_10, GPIO_FUNC_11_ETH ,PIN_SUBFUNC_DISABLE);
#endif
}

/**
 * @brief 以太网中断函数
 * 
 */
static void eth_isr_callback(void)
{
    if (Set == ETH_DMA_GetStatus(ETH_DMA_FLAG_RIS))
    {
        ETH_DMA_ClearStatus(ETH_DMA_FLAG_RIS | ETH_DMA_FLAG_NIS);   
        if (rx_semphr != NULL)
        {
            osSemaphoreRelease(rx_semphr);
        }
    }
}

/**
 * @brief 以太网输入处理
 * 
 * @param[in]arg            网卡指针
 */
static void eth_input_thread(void* arg)
{
    struct pbuf* p = NULL;
    struct netif* p_netif = (struct netif*)arg;    

    rx_semphr = osSemaphoreNew(1, 1, NULL);   

    for (;;)
    {
        if (osSemaphoreAcquire(rx_semphr, 100) == osOK)
        {
            do {
                p = eth_low_level_input(p_netif);
                if (p != NULL)
                {
                    if (p_netif->input(p, p_netif) != ERR_OK)
                    {
                        pbuf_free(p);
                    }                
                }
            }while(p != NULL);
        }
    }
}

static void low_level_init(struct netif *netif)
{
    if (hal_eth_init() == 0)
    {
        netif->flags |= NETIF_FLAG_LINK_UP;
    }
        
    netif->mtu       = 1500U;
    netif->flags    |= NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP | NETIF_FLAG_IGMP;
    netif->hwaddr[0] = (EthHandle.stcCommInit).au8MACAddr[0];
    netif->hwaddr[1] = (EthHandle.stcCommInit).au8MACAddr[1];
    netif->hwaddr[2] = (EthHandle.stcCommInit).au8MACAddr[2];
    netif->hwaddr[3] = (EthHandle.stcCommInit).au8MACAddr[3];
    netif->hwaddr[4] = (EthHandle.stcCommInit).au8MACAddr[4];
    netif->hwaddr[5] = (EthHandle.stcCommInit).au8MACAddr[5];
    netif->hwaddr_len = NETIF_MAX_HWADDR_LEN;
//    
    osThreadNew(eth_input_thread, netif, &eth_input_thread_attr);
	netif_set_link_callback(netif, eth_netif_link_cb);
    hal_eth_start();
}


static void eth_netif_link_cb(struct netif *netif)
{
    __IO uint32_t tickStart = 0UL;    
    __IO en_result_t negoResult = Error;
    uint16_t u16RegVal = 0U;

    if (netif_is_link_up(netif))
    {
        /* Restart the auto-negotiation */
        if (ETH_AUTO_NEGOTIATION_DISABLE != (EthHandle.stcCommInit).u16AutoNegotiation)
        {
            /* Enable Auto-Negotiation */
            (void)ETH_PHY_WriteRegister(&EthHandle, PHY_BCR, PHY_AUTONEGOTIATION);

            /* Wait until the auto-negotiation will be completed */
            tickStart = osKernelGetTickCount();
            do
            {
                (void)ETH_PHY_ReadRegister(&EthHandle, PHY_BSR, &u16RegVal);
                if (PHY_AUTONEGO_COMPLETE == (u16RegVal & PHY_AUTONEGO_COMPLETE))
                {
                    break;
                }
                /* Check for the Timeout (1s) */
            } while ((osKernelGetTickCount() - tickStart) <= 1000U);

            if (PHY_AUTONEGO_COMPLETE == (u16RegVal & PHY_AUTONEGO_COMPLETE))
            {
                negoResult = Ok;
                /* Configure ETH duplex mode according to the result of automatic negotiation */
                if (0U != (u16RegVal & (PHY_100BASE_TX_FD | PHY_10BASE_T_FD)))
                {
                    (EthHandle.stcCommInit).u32DuplexMode = ETH_MAC_MODE_FULLDUPLEX;
                }
                else
                {
                    (EthHandle.stcCommInit).u32DuplexMode = ETH_MAC_MODE_HALFDUPLEX;
                }

                /* Configure ETH speed according to the result of automatic negotiation */
                if (0U != (u16RegVal & (PHY_100BASE_TX_FD | PHY_100BASE_TX_HD)))
                {
                    (EthHandle.stcCommInit).u32Speed = ETH_MAC_SPEED_100M;
                }
                else
                {
                    (EthHandle.stcCommInit).u32Speed = ETH_MAC_SPEED_10M;
                }
            }
        }

        /* AutoNegotiation disable or failed*/
        if (Error == negoResult)
        {
            /* Set MAC Speed and Duplex Mode to PHY */
            (void)ETH_PHY_WriteRegister(&EthHandle, PHY_BCR,
                                        ((uint16_t)((EthHandle.stcCommInit).u32DuplexMode >> 3U) |
                                         (uint16_t)((EthHandle.stcCommInit).u32Speed >> 1U)));
        }

        /* ETH MAC Re-Configuration */
        ETH_MAC_SetDuplexSpeed((EthHandle.stcCommInit).u32DuplexMode, (EthHandle.stcCommInit).u32Speed);
        /* Restart MAC interface */
        (void)ETH_Start();
    }
    else
    {
        /* Stop MAC interface */
        (void)ETH_Stop();
    }
}

/* Ethernet MAC_SMIADDR register Mask */
#define ETH_MAC_SMIADDR_CLEAR_MASK                          (0x0000FFC3UL)

uint8_t hal_read_phy_reg(uint16_t phy_reg, uint16_t u16Reg, uint16_t *pu16RegVal)
{
    __IO uint32_t u32Count;
    en_result_t enRet = Ok;

    *pu16RegVal = 0U;
    /* Set the MAC_SMIADDR register */
    /* Keep only the MDC Clock Range SMIC[3:0] bits value */
    MODIFY_REG32(M4_ETH->MAC_SMIADDR, ETH_MAC_SMIADDR_CLEAR_MASK,
                (((uint32_t)(phy_reg) << ETH_MAC_SMIADDR_SMIA_POS) |
                ((uint32_t)u16Reg << ETH_MAC_SMIADDR_SMIR_POS) | ETH_MAC_SMIADDR_SMIB));
    /* Check for the Busy flag */
    u32Count = PHY_READ_TIMEOUT * (HCLK_VALUE / 20000UL);
    while (0UL != READ_REG32_BIT(M4_ETH->MAC_SMIADDR, ETH_MAC_SMIADDR_SMIB))
    {
        if (0UL == u32Count)
        {
            enRet = ErrorTimeout;
            break;
        }
        u32Count--;
    }

    if (ErrorTimeout != enRet)
    {
        /* Get the MAC_SMIDATR value */
        *pu16RegVal = (uint16_t)(READ_REG32(M4_ETH->MAC_SMIDATR));
    }

    return enRet;
}
