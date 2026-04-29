#include "dev_bb1735.h"
#include <string.h>
#include "hc32_ddl.h"
#include "cmsis_os.h"
#define sys_delay_ms(a)		vTaskDelay(a)
#define BB1735_ResetPin_Hight          	GPIO_SetPins(BLE_RST_GPIO_Port, BLE_RST_Pin)
#define BB1735_ResetPin_Low             GPIO_ResetPins(BLE_RST_GPIO_Port, BLE_RST_Pin)

#define READ_BUFFER_SIZE 		128
#define WRITE_BUFFER_SIZE 		128
uint8_t BB1735_Read_Buffer[READ_BUFFER_SIZE] = {0};
uint8_t BB1735_Write_Buffer[WRITE_BUFFER_SIZE] = {'A', 'T', '+'};

uint16_t bb1735_cmd_time = 0;
uint16_t bb1735_cmd_timeout = 0;

BB1735_RX_Package BleDMARecieve;
BB1735_RX_Package BleRXPack;

static uint8_t cmd_rec_len = 0;

void BB1735_Timer_1MS_Handler(void)
{
	if(bb1735_cmd_time < 0xffff)
		bb1735_cmd_time++;
}

inline static bool BB1735_Cmd_Is_Timeout(void)
{
	return ((bb1735_cmd_time < bb1735_cmd_timeout) ? false : true);
}

inline static void BB1735_Cmd_Timeout_Set(uint16_t Timeout)
{
	bb1735_cmd_timeout = Timeout;
	bb1735_cmd_time = 0;
}

void BB1735_CMD_Write(uint8_t *pData, uint16_t Cnt)
{
	//TODO
    // uint16_t i = 0;
    // for(i = 0; i < Cnt; i++)
    // {
    //     while(!usart_flag_get(USART5, USART_FLAG_TBE));
    //     usart_data_transmit(USART5, pData[i]);    
    // }
	extern void usart_send(uint8_t *pData, uint16_t Cnt);
	usart_send(pData, Cnt);
}

//
int32_t BB1735_CMD_Read(uint8_t *RxData)
{
	int32_t res = true;
	//TODO
	return res;
}

//不精确延时函数
static void BB1735_DelayMs(uint16_t ms)
{
	uint16_t i, j;
	
	for(i = 0; i < ms; i++){	
		for(j = 0; j < 8000; j++) ;
	}
}


void BB1735_Reset_Chip(void)
{
	BB1735_ResetPin_Low;
	 BB1735_DelayMs(50);
	//sys_delay_ms(50);
	BB1735_ResetPin_Hight;
}

void BB1735_Read_Pack(uint8_t *Packet, uint16_t *PacketLength, char* NodeAddr, char *Cmd,  uint16_t CmdLen)
{
	*PacketLength = 0;
	Packet[0] = 'A';
	Packet[1] = 'T';
	Packet[2] = '+';
	*PacketLength += 3;
	
	memcpy((char *)(&Packet[*PacketLength]), (const char *)Cmd, CmdLen);
	*PacketLength +=  CmdLen;
	if(NULL == NodeAddr) {
		Packet[*PacketLength] = '?';
		*PacketLength += 1;
	}else {
		Packet[*PacketLength] = '@';
		*PacketLength += 1;
		memcpy((char *)(&Packet[*PacketLength]), (const char *)NodeAddr, 4);
		*PacketLength +=  4;
	}
	Packet[*PacketLength] = '\r';
	*PacketLength += 1;
}

void BB1735_Write_Pack(uint8_t *Packet, uint16_t *PacketLength, char* NodeAddr, char *Cmd, uint8_t CmdLen, uint8_t * Data, uint8_t DataLen)
{
    uint8_t i = 0;
	*PacketLength = 0;
	Packet[0] = 'A';
	Packet[1] = 'T';
	Packet[2] = '+';
	*PacketLength += 3;
	
	memcpy((char *)(&Packet[*PacketLength]), (const char *)Cmd, CmdLen);
	*PacketLength +=  CmdLen;
	if(NULL != NodeAddr) {
		Packet[*PacketLength] = '@';
		*PacketLength += 1;
		memcpy((char *)(&Packet[*PacketLength]), (const char *)NodeAddr, 4);
		*PacketLength +=  4;
	}
	Packet[*PacketLength] = '=';
	*PacketLength += 1;

    for(i = 0; i < DataLen; i++)
    {
        switch(Data[i])
        {
            case 0X0D:
                Packet[*PacketLength] = 0x5C;
                Packet[*PacketLength + 1] = 0X72;
                *PacketLength += 2;
                break;
            case 0X0A:
                Packet[*PacketLength] = 0X5C;
                Packet[*PacketLength + 1] = 0X6E;
                *PacketLength += 2;                            
                break;
            case 0X5C:
                Packet[*PacketLength] = 0x5C;
                Packet[*PacketLength + 1] = 0x5C;
                *PacketLength += 2;
                break;
            default:
                Packet[*PacketLength] = Data[i];
                *PacketLength += 1;
                break;
        }
    }
	Packet[*PacketLength] = '\r';
	*PacketLength += 1;
}

//测试指令,用于测试设备是否正常工作以及 UART 通信是否正常。
bool BB1735_AT_Test(void)
{
	return true;
}

//获取设备固件版本
void BB1735_Get_DeviVer(char *NodeAddr)
{
	uint16_t Length = 0;
	
	BB1735_Read_Pack(BB1735_Write_Buffer, &Length, NodeAddr, DEVIVER_CMD, strlen(DEVIVER_CMD));
	BB1735_CMD_Write(BB1735_Write_Buffer, Length);
}

//获取设备 UUID
void BB1735_Get_DevUUID(char *NodeAddr)
{
	uint16_t Length = 0;
	
	BB1735_Read_Pack(BB1735_Write_Buffer, &Length, NodeAddr, DEVIUUID_CMD, strlen(DEVIUUID_CMD));
	BB1735_CMD_Write(BB1735_Write_Buffer, Length);
}
//设置设备 UUID
void BB1735_Set_DevUUID(uint8_t *UUID, uint8_t UUID_Len)
{
	uint16_t Length = 0;
	
	BB1735_Write_Pack(BB1735_Write_Buffer, &Length, NULL, SETUUID_CMD, strlen(SETUUID_CMD), UUID, UUID_Len);
	BB1735_CMD_Write(BB1735_Write_Buffer, Length);
}

//获取设备蓝牙地址
void BB1735_Get_DevMac(char *NodeAddr)
{
	uint16_t Length = 0;
	
	BB1735_Read_Pack(BB1735_Write_Buffer, &Length, NodeAddr, DEVIMAC_CMD, strlen(DEVIMAC_CMD));
	BB1735_CMD_Write(BB1735_Write_Buffer, Length);
}

//获取节点地址
void BB1735_Get_NodeAddr(char *NodeAddr)
{
	uint16_t Length = 0;
	
	BB1735_Read_Pack(BB1735_Write_Buffer, &Length, NodeAddr, NODEADDR_CMD, strlen(NODEADDR_CMD));
	BB1735_CMD_Write(BB1735_Write_Buffer, Length);
}

//获取节点列表
void BB1735_Get_NodeList(void)
{
	uint16_t Length = 0;
	
	BB1735_Read_Pack(BB1735_Write_Buffer, &Length, NULL, NODELIST_CMD, strlen(NODELIST_CMD));
	BB1735_CMD_Write(BB1735_Write_Buffer, Length);
}

//获取节点订阅地址
void BB1735_Get_NodeSubs(uint8_t *NodeSubs)
{
}

//获取节点发布地址
void BB1735_Get_NodePubs(uint8_t *NodePubs)
{
}

//获取节点 models
void BB1735_Get_NodeMods(uint8_t *NodeMods)
{
}

//设置节点 appkey index
void BB1735_Set_NodeAppIndex(uint8_t *NodeAppIndex, uint8_t NodeAppIndexLen)
{
	uint16_t Length = 0;
	
	BB1735_Write_Pack(BB1735_Write_Buffer, &Length, NULL, NODEAPPIND_CMD, strlen(NODEAPPIND_CMD), NodeAppIndex, NodeAppIndexLen);
	BB1735_CMD_Write(BB1735_Write_Buffer, Length);
}
//获取节点 appkey index
void BB1735_Get_NodeAppIndex(uint8_t *NodeAppIndex)
{
}

//设置节点指令超时时间
void BB1735_Set_NodeTimeout(uint8_t *Timeout)
{
}
//获取节点指令超时时间
void BB1735_Get_NodeTimeout(uint8_t *Timeout)
{
}

//向目的节点发送字符串数据
void BB1735_Send_String(char *NodeAddr, uint8_t *Str, uint8_t StrLen)
{
	uint16_t Length = 0;
	BB1735_Write_Pack(BB1735_Write_Buffer, &Length, NodeAddr, DATA_CMD, strlen(DATA_CMD), Str, StrLen);
	BB1735_CMD_Write(BB1735_Write_Buffer, Length);
    
}

void BB1735_Send_OTA_Data(uint8_t *Data, uint8_t DataLen)
{
	uint16_t Length = 0;
	BB1735_Write_Pack(BB1735_Write_Buffer, &Length, NULL, OTA_CMD, strlen(OTA_CMD), Data, DataLen);
	BB1735_CMD_Write(BB1735_Write_Buffer, Length);  
}

void BB1735_Send_GATT_Data(uint8_t *Data, uint8_t DataLen)
{
	uint16_t Length = 0;
	BB1735_Write_Pack(BB1735_Write_Buffer, &Length, NULL, GATT_CMD, strlen(GATT_CMD), Data, DataLen);
	BB1735_CMD_Write(BB1735_Write_Buffer, Length);  
}



//向目的节点发送 16 进制数据
void BB1735_Send_HexString(char *NodeAddr, uint8_t *Data)
{
}

//获取节点是否 provision
bool BB1735_Is_Provision(void)
{
	bool ret = true;
	
	return ret;
}

//清除节点 provision 信息
void BB1735_Node_Rset(void)
{
	BB1735_CMD_Write("AT+NODERSET=YES\r", strlen("AT+NODERSET=YES\r"));
}

//WaitTime ms
uint8_t BB1735_Send_Data(char* NodeAddr, BB1735_Cmd_TypeDef Cmd, uint8_t *Data, uint8_t DataLen, uint16_t WaitTime)
{
	uint8_t ret = 0;
    (void)WaitTime;
	switch(Cmd)
	{
		case BB1735_DEVIVER:
			BB1735_Get_DeviVer(NodeAddr);
			break;
		case BB1735_DEVIUUID_GET:
			BB1735_Get_DevUUID(NULL);
			break;
		case BB1735_DEVIUUID_SET:
			BB1735_Set_DevUUID(Data, DataLen);
			break;
		case BB1735_DEVIMAC:
			BB1735_Get_DevMac(NULL);
			break;
		case BB1735_NODEPROV:
			break;
		case BB1735_NODEADDR:
			break;
		case BB1735_NODELIST:
			break;
		case BB1735_NODESUBS:
			break;
		case BB1735_NODEPUBS:
			break;
		case BB1735_NODEMODS:
			break;
		case BB1735_NODEAPPIND:
			break;
		case BB1735_NODETIEMOUT:
			break;
		case BB1735_DATA:
			BB1735_Send_String(NodeAddr, Data, DataLen);
			break;
        case BB1735_NODERSET:
            BB1735_Node_Rset();
			break;
        case BB1735_OTA:
            BB1735_Send_OTA_Data(Data, DataLen);
        break;
        case BB1735_GATT:
            BB1735_Send_GATT_Data(Data, DataLen);
        break;    
		default:
			ret = 0;
			break;
	}
	return ret;
}

void BB1735_DATA_Deal(uint8_t *RecData, uint8_t Cnt)
{
	//TODO
	// BT_Rx_Deal(RecData, Cnt);
}


void BB1735_RecData_parse(uint8_t *RecData, uint8_t Cnt)
{
	if(strncmp((const char *)RecData, "+DATA", 5) == 0) {
		BB1735_DATA_Deal(&RecData[11], Cnt - 11 - 1);
	}else if(strncmp((const char *)RecData, "AT+OTA", 6) == 0){
	    
	}else {
		if(Cnt <= READ_BUFFER_SIZE)
			memcpy(BB1735_Read_Buffer, RecData, Cnt);
		cmd_rec_len = Cnt;
	}
}
#if 0
void gd_usart5_init(void)
{
    dma_single_data_parameter_struct dma_init_struct = {0};
    
    // rcu_periph_clock_enable(RCU_USART5);
    // rcu_periph_clock_enable(RCU_GPIOC);
    // gpio_init(GPIOC, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, BLE_TX_Pin);
    // gpio_init(GPIOC, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, BLE_RX_Pin);

    rcu_periph_clock_enable(RCU_GPIOC);
    /* enable USART clock */
    rcu_periph_clock_enable(RCU_USART5);

    /* connect port to USARTx_Tx */
    gpio_af_set(BLE_TX_GPIO_Port, GPIO_AF_8, BLE_TX_Pin);
    /* configure USART Tx as alternate function push-pull */
    gpio_mode_set(BLE_TX_GPIO_Port, GPIO_MODE_AF, GPIO_PUPD_PULLUP, BLE_TX_Pin);
    gpio_output_options_set(BLE_TX_GPIO_Port, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, BLE_TX_Pin);

    /* connect port to USARTx_Rx */
    gpio_af_set(BLE_RX_GPIO_Port, GPIO_AF_8, BLE_RX_Pin);
    /* configure USART Rx as alternate function push-pull */
	gpio_mode_set(BLE_RX_GPIO_Port, GPIO_MODE_INPUT, GPIO_PUPD_NONE, BLE_RX_Pin);
    // gpio_mode_set(BLE_RX_GPIO_Port, GPIO_MODE_AF, GPIO_PUPD_PULLUP, BLE_RX_Pin);
    // gpio_output_options_set(BLE_RX_GPIO_Port, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, BLE_RX_Pin);	


    usart_deinit(USART5);
    usart_baudrate_set(USART5, 115200U);
    usart_word_length_set(USART5, USART_WL_8BIT);
    usart_stop_bit_set(USART5, USART_STB_1BIT);
    usart_parity_config(USART5, USART_PM_NONE);
    usart_hardware_flow_rts_config(USART5, USART_RTS_DISABLE);
    usart_hardware_flow_cts_config(USART5, USART_CTS_DISABLE);
    usart_receive_config(USART5, USART_RECEIVE_ENABLE);
    usart_transmit_config(USART5, USART_TRANSMIT_ENABLE);  
//    usart_interrupt_enable(USART2, USART_INT_IDLE);
//    usart_interrupt_enable(USART2, USART_INT_RBNE);
   	// usart_enable(USART5); 
    nvic_irq_enable(USART5_IRQn, 5, 0);


    /* enable DMA0 */
    rcu_periph_clock_enable(RCU_DMA1);
    dma_deinit(DMA1, DMA_CH1);
    dma_init_struct.direction = DMA_PERIPH_TO_MEMORY;
    dma_init_struct.memory0_addr = 0;
    dma_init_struct.memory_inc = DMA_MEMORY_INCREASE_ENABLE;
    dma_init_struct.periph_memory_width = DMA_PERIPH_WIDTH_8BIT;
    dma_init_struct.number = 0;
    dma_init_struct.periph_addr = (uint32_t)(&USART_DATA(USART5));
    dma_init_struct.periph_inc = DMA_PERIPH_INCREASE_DISABLE;
    dma_init_struct.priority = DMA_PRIORITY_ULTRA_HIGH;
    dma_single_data_mode_init(DMA1, DMA_CH1, &dma_init_struct);
    /* configure DMA mode */
    dma_circulation_disable(DMA1, DMA_CH1);
	/* DMA通道外设选择，gd32f303没有这步 */
	dma_channel_subperipheral_select(DMA1, DMA_CH1, DMA_SUBPERI5);
    nvic_irq_enable(DMA1_Channel1_IRQn, 5, 0);
}

void gd_timer3_init(void)
{
    timer_parameter_struct timer_initpara;
    rcu_periph_clock_enable(RCU_TIMER3);
    timer_deinit(TIMER3);

    /* TIMER3 configuration */
    timer_initpara.prescaler         = 168-1;
    timer_initpara.alignedmode       = TIMER_COUNTER_EDGE;
    timer_initpara.counterdirection  = TIMER_COUNTER_UP;
    timer_initpara.period            = 500;
    timer_initpara.clockdivision     = TIMER_CKDIV_DIV1;
    timer_initpara.repetitioncounter = 0;
    timer_init(TIMER3, &timer_initpara);

    nvic_irq_enable(TIMER3_IRQn, 5, 0);
	timer_interrupt_enable(TIMER3, TIMER_INT_UP);
//    timer_enable(TIMER3);
}

void gd_timer1_init(void)
{
    timer_parameter_struct timer_initpara;
    rcu_periph_clock_enable(RCU_TIMER1);
    timer_deinit(TIMER1);

    /* TIMER3 configuration */
    timer_initpara.prescaler         = 168-1;
    timer_initpara.alignedmode       = TIMER_COUNTER_EDGE;
    timer_initpara.counterdirection  = TIMER_COUNTER_UP;
    timer_initpara.period            = 100;
    timer_initpara.clockdivision     = TIMER_CKDIV_DIV1;
    timer_initpara.repetitioncounter = 0;
    timer_init(TIMER1, &timer_initpara);

    nvic_irq_enable(TIMER1_IRQn, 5, 0);
	timer_interrupt_enable(TIMER1, TIMER_INT_UP);
    timer_enable(TIMER1);
}
/* 模块相关的外设初始化 */
// void BB1735_Bluetooth_Periph_Init(void)
// {
// 	gd_usart5_init();
// 	gd_timer3_init();
// }

void BB1735_Bluetooth_Module_Reset(void)
{
	BB1735_Bluetooth_Module_disable();
	BB1735_Bluetooth_Module_Init();
	BB1735_Bluetooth_Module_enable();
}

bool BB1735_Bluetooth_Module_Init(void)
{	
	// __HAL_UART_ENABLE_IT(&huart6, UART_IT_IDLE);        //使能IDLE中断
    // HAL_UART_Receive_DMA(&huart6, BleDMARecieve.data, BB1735_MAX_RCV_LEN);

	rcu_periph_clock_enable(RCU_GPIOB);
	rcu_periph_clock_enable(RCU_GPIOC);
	/* BLE_EN */
	gpio_bit_write(BLE_EN_GPIO_Port, BLE_EN_Pin, SET);
	
    gpio_mode_set(BLE_EN_GPIO_Port, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, BLE_EN_Pin);
    gpio_output_options_set(BLE_EN_GPIO_Port, GPIO_OTYPE_PP, GPIO_OSPEED_2MHZ, BLE_EN_Pin);
	// gpio_bit_write(BLE_EN_GPIO_Port, BLE_EN_Pin, SET);
	
	/* BLE_RST */
    gpio_mode_set(BLE_RST_GPIO_Port, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, BLE_RST_Pin);
    gpio_output_options_set(BLE_RST_GPIO_Port, GPIO_OTYPE_PP, GPIO_OSPEED_2MHZ, BLE_RST_Pin);	
	gpio_bit_write(BLE_RST_GPIO_Port, BLE_RST_Pin, SET);

	gd_usart5_init();
	// gd_timer3_init();

	return true;
}
#endif
void BB1735_Bluetooth_Module_enable(void)
{
    // dma_memory_address_config(DMA1, DMA_CH1, DMA_MEMORY_0, (uint32_t)&BleDMARecieve.data);
    // dma_transfer_number_config(DMA1, DMA_CH1, BB1735_MAX_RCV_LEN);
    // usart_interrupt_enable(USART5, USART_INT_IDLE);
   	// usart_receiver_timeout_enable(USART5);
   	// usart_receiver_timeout_threshold_config(USART5, 58); //500us，115200/1000/2 = 57.6
	// usart_interrupt_disable(USART5, USART_INT_RT);	
    // dma_interrupt_enable(DMA1, DMA_CH1, DMA_CHXCTL_FTFIE);	
    // /* USART DMA enable for transmission and reception */
    // usart_dma_receive_config(USART5, USART_DENR_ENABLE);	
	// dma_channel_enable(DMA1, DMA_CH1);
	// usart_enable(USART5);

	// // HAL_GPIO_WritePin(BLE_EN_GPIO_Port, BLE_EN_Pin, GPIO_PIN_RESET);
	BB1735_DelayMs(100);
//	// gpio_bit_write(BLE_EN_GPIO_Port, BLE_EN_Pin, RESET);
	GPIO_SetPins(BLE_EN_GPIO_Port, BLE_EN_Pin);
	BB1735_DelayMs(100);
	//硬件复位
	BB1735_Reset_Chip();	
}
#if 0
void BB1735_Bluetooth_Module_disable(void)
{	
	// __HAL_UART_DISABLE_IT(&huart6, UART_IT_IDLE);
	// HAL_TIM_Base_Stop_IT(&htim6);
	// HAL_UART_DMAStop(&huart6);  //停止DMA
	// HAL_GPIO_WritePin(BLE_EN_GPIO_Port, BLE_EN_Pin, GPIO_PIN_SET);

	usart_interrupt_disable(USART5, USART_INT_IDLE);
	timer_disable(TIMER3);
	dma_channel_disable(DMA1, DMA_CH1);
	gpio_bit_write(BLE_EN_GPIO_Port, BLE_EN_Pin, SET);

	ble_usart_rxtx_disable();
}

void ble_usart_rxtx_disable(void)
{
  	// HAL_UART_MspDeInit(&huart6);   
	// GPIO_InitTypeDef GPIO_InitStruct = { 0 };
	// GPIO_InitStruct.Pin = BLE_TX_Pin | BLE_RX_Pin;
	// GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	// GPIO_InitStruct.Pull = GPIO_NOPULL;
	// GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	// HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);	
	// HAL_GPIO_WritePin(BLE_TX_GPIO_Port, BLE_TX_Pin, GPIO_PIN_RESET);
	// HAL_GPIO_WritePin(BLE_RX_GPIO_Port, BLE_RX_Pin, GPIO_PIN_RESET);
	// HAL_GPIO_WritePin(BLE_RST_GPIO_Port, BLE_RST_Pin, GPIO_PIN_RESET);


	usart_disable(USART5);
	rcu_periph_clock_disable(RCU_USART5);
	dma_deinit(DMA1, DMA_CH1);
	nvic_irq_disable(DMA1_Channel1_IRQn);
	nvic_irq_disable(USART5_IRQn);

	gpio_mode_set(BLE_TX_GPIO_Port, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, BLE_TX_Pin);
	gpio_output_options_set(BLE_TX_GPIO_Port, GPIO_OTYPE_PP, GPIO_OSPEED_2MHZ, BLE_TX_Pin);
	gpio_mode_set(BLE_RX_GPIO_Port, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, BLE_RX_Pin);
	gpio_output_options_set(BLE_RX_GPIO_Port, GPIO_OTYPE_PP, GPIO_OSPEED_2MHZ, BLE_RX_Pin);	
	gpio_mode_set(BLE_RST_GPIO_Port, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, BLE_RST_Pin);
	gpio_output_options_set(BLE_RST_GPIO_Port, GPIO_OTYPE_PP, GPIO_OSPEED_2MHZ, BLE_RST_Pin);		

	gpio_bit_write(BLE_TX_GPIO_Port, BLE_TX_Pin, RESET);
	gpio_bit_write(BLE_RX_GPIO_Port, BLE_RX_Pin, RESET);
	gpio_bit_write(BLE_RST_GPIO_Port, BLE_RST_Pin, RESET);
}
#endif
