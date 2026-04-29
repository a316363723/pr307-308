#include "dev_adapter.h"
#include "hal_gpio.h"
#include "hal_uart.h"
#include "hc32_ddl.h"
#include "perf_counter.h"
#include <stdarg.h>
#include <string.h>
#include "cmsis_os2.h"
#include <stdlib.h>


#define AdAPTER_A_Port               HAL_GPIOI
#define AdAPTER_A_PIN                HAL_PIN_1
#define AdAPTER_B_Port               HAL_GPIOC
#define AdAPTER_B_PIN                HAL_PIN_8


#define ADAPTER_START_CODE          ((uint8_t)0x7e)
#define ADAPTER_END_CODE            ((uint8_t)0X0D)
#define ADAPTER_ADDR_CODE           ((uint8_t)0X01)

osMessageQueueId_t os_queue_usart_adapter_rx;
osSemaphoreId_t    os_semaphore_adapter_tx;
#define ADAPTER_MESSAGE_MAX_SIZE    30


typedef struct
{
    int (*write_data)(uint8_t* p_data, uint32_t size, uint32_t timeout);
    int (*read_data)(uint8_t* p_data, uint32_t size, uint32_t timeout);

} dev_adapter_t;


typedef enum
{
	ADAPTER_CMD_READ_VER = 0X01,
    ADAPTER_CMD_INFO = 0x03,
    ADAPTER_CMD_SWITCH = 0x04,
    ADAPTER_CMD_SET_VOLTAGE = 0x06,

} adapter_cmd_enum;
#pragma pack(1)
typedef struct
{
    uint8_t  response;
    uint16_t v_output;
    uint16_t i_output;
    uint16_t i_bat;
    uint16_t temp;
    uint8_t  warn;
    uint8_t  type;
    uint16_t i_input;
    uint16_t v_input;
} adapter_info_t;


typedef struct
{
    uint8_t state;
} adapter_switch_t;


typedef struct{
    uint8_t               head;
    uint8_t               addr;
    uint8_t               length;
    adapter_cmd_enum      cmd_type;
    uint8_t               check_sum;
    uint8_t               end;
}adapter_head_t;


typedef struct
{
    uint8_t               head;
    uint8_t               addr;
    uint8_t               length;
    adapter_cmd_enum      cmd_type;
    uint8_t               state; 
    uint8_t               check_sum;
    uint8_t               end;

}switch_response_body_t;

typedef struct
{
    uint8_t               head;
    uint8_t               addr;
    uint8_t               length;
    adapter_cmd_enum      cmd_type;
    adapter_info_t        info; 
    uint8_t               check_sum;
    uint8_t               end;

}info_response_body_t;


typedef struct
{
    adapter_head_t       head;
    uint8_t              data[0];

} adapter_ack_body_t;


typedef struct
{
    uint8_t length;
    uint8_t data[30];
} adapter_pack_type;

#pragma pack()
dev_adapter_t dev_adapter;

adapter_pack_type adapter_rx_pack = {0};
  
static uint8_t dec_to_bcd(uint8_t dec)  
{  
    uint8_t crc = 0;
    crc = dec % 100;
    return (crc+(crc/10)*6);
} 



static uint8_t bcd_to_dec(uint8_t bcd)
{
    return bcd-(bcd >> 4)*6;
}

float uint16_to_float(uint16_t voltage )
{
	static float adapter_voltage;
	uint8_t *p  = (uint8_t*)&voltage;
	adapter_voltage = (float)bcd_to_dec(*(uint8_t*)(p+1))/100.0f + (float)bcd_to_dec(*(uint8_t*)p);
}

static void adapter_rx_complete_int_cb(void)
{

    hal_uart_receive_dma_channel_stop(HAL_UART_ADAPTER);
    adapter_rx_pack.length = sizeof(adapter_rx_pack.data) - hal_uart_get_receive_dma_cnt(HAL_UART_ADAPTER);
//    memcpy(adapter_rx_pack.data, adapter_rx_pack.data, adapter_rx_pack.length);
    osMessageQueuePut(os_queue_usart_adapter_rx, &adapter_rx_pack, NULL, 0);

    hal_uart_receive_dma_start(HAL_UART_ADAPTER, adapter_rx_pack.data, sizeof(adapter_rx_pack.data));
    hal_uart_receive_dma_channel_start(HAL_UART_ADAPTER);

}


static void adapter_tx_complete_int_cb(void)
{
  
    osSemaphoreRelease(os_semaphore_adapter_tx);
}

void adapter_a_and_b_ctrl(adapter_choose_enum value)
{
    hal_gpio_init(AdAPTER_A_Port, AdAPTER_A_PIN, HAL_PIN_MODE_OUT_PP);
    hal_gpio_init(AdAPTER_B_Port, AdAPTER_B_PIN, HAL_PIN_MODE_OUT_PP);
    if(value == ADAPTER_A_EN)
    {
         hal_gpio_write_pin(AdAPTER_A_Port, AdAPTER_A_PIN,1);
         hal_gpio_write_pin(AdAPTER_B_Port, AdAPTER_B_PIN,0);
        
    }
    else if (value == ADAPTER_B_EN) 
    {
       hal_gpio_write_pin(AdAPTER_A_Port, AdAPTER_A_PIN,0);
       hal_gpio_write_pin(AdAPTER_B_Port, AdAPTER_B_PIN,1);
    }
    else if (value == ADAPTER_A_AND_B_EN) 
    {
       hal_gpio_write_pin(AdAPTER_A_Port, AdAPTER_A_PIN,1);
       hal_gpio_write_pin(AdAPTER_B_Port, AdAPTER_B_PIN,1);
    
    }
     else if (value == ADAPTER_A_AND_B_DIS)
    {
        hal_gpio_write_pin(AdAPTER_A_Port, AdAPTER_A_PIN,0);
        hal_gpio_write_pin(AdAPTER_B_Port, AdAPTER_B_PIN,0);
    
    }
}


int adapter_usart_tx_data(uint8_t* p_data, uint32_t size, uint32_t timeout)
{
//    int32_t sl;
    hal_dmx_write_ctrl_pin(HAL_UART_ADAPTER, 1);
    osSemaphoreAcquire(os_semaphore_adapter_tx, 0);
    hal_uart_transmit_dma_start(HAL_UART_ADAPTER, p_data, size);
    hal_uart_irq_register(HAL_UART_ADAPTER, HAL_UART_IRQ_TX_DMA_FT, adapter_tx_complete_int_cb);
    osSemaphoreAcquire(os_semaphore_adapter_tx, (size)+2 );
    osDelay(3);
//    sl = osKernelLock();
//    delay_us(200);
//    osKernelRestoreLock(sl);
    hal_dmx_write_ctrl_pin(HAL_UART_ADAPTER, 0);
    return size;
}


int adapter_usart_rx_data(uint8_t* p_data, uint32_t size, uint32_t timeout)
{
    adapter_pack_type queue_pack;
  
    if(osOK == osMessageQueueGet(os_queue_usart_adapter_rx, &queue_pack, NULL, timeout))
    {
        memcpy(p_data, queue_pack.data, queue_pack.length);
        return queue_pack.length;
    }

    return 0;
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

uint8_t  init_message_package(adapter_ack_body_t* s_body , uint8_t * s_buff , uint8_t info_led)
{
    uint8_t  i = 0;
    uint8_t  j  = 0;
    uint8_t  a = 0;
    s_buff[i++] = ADAPTER_START_CODE;
    s_buff[i++] = ADAPTER_ADDR_CODE;
    s_buff[i++] = info_led + 1;
    s_buff[i++] = s_body->head.cmd_type;
    memcpy(&s_buff[i], &s_body->data, info_led);
	for(a = i ; a< i+info_led ;a++)
	{
		s_buff[a] = dec_to_bcd (s_buff[a]);
	}
    j = (i + info_led );
    s_buff[j] = dec_to_bcd (check_sum_calc((uint8_t * )&s_buff[1] , j-1));
    j++;
    s_buff[j++] = ADAPTER_END_CODE;
    return j;

}


static int receive_message_check(uint8_t* r_body,adapter_cmd_enum  cmd_type  )
{
    uint8_t  i = 0;
    uint8_t  j  = 0;
    uint8_t len;
    if(r_body[i++] != ADAPTER_START_CODE)
        return -1;
    if(r_body[i++] != ADAPTER_ADDR_CODE)
        return -2;
    len = r_body[i++];
    if(r_body[i] != ( cmd_type | 0x80 ))
        return -3;
     j = i + bcd_to_dec(len) ;
    if(r_body[j] != dec_to_bcd(check_sum_calc((uint8_t*)&r_body[1], j - 1)))
        return -4;

    return 0;
}


int dev_adapter_cmd_write( dev_adapter_t* p_dev, int retry_times, adapter_ack_body_t *p_ack ,uint8_t info_led , uint8_t * pbuff)
{
    int res = -1;
    uint8_t rlen = 0;
    uint8_t wlen = 0 ;
    uint8_t read_buff[ADAPTER_MESSAGE_MAX_SIZE];
    static uint8_t write_buff[ADAPTER_MESSAGE_MAX_SIZE] ;
    wlen = init_message_package(p_ack, write_buff , info_led);
    do
    {
        p_dev->write_data((uint8_t*)&write_buff, wlen, 1);

        rlen = p_dev->read_data(read_buff, 0xFFFF, 100);

        if( (rlen > 0 ) && (0 == receive_message_check(read_buff, p_ack->head.cmd_type)))
        {
            res = 0;
            break;
        }

    }
    while(--retry_times > 0 && 0 != res);

    if( res == 0 )
    {
       memcpy(pbuff, read_buff , rlen);
    }

    return res;

}
//void adapter_switch_ctrl

int adapter_switch_ctrl(uint8_t state)
{
    int res = -1;
    adapter_ack_body_t *body_1;
    switch_response_body_t boby_2;
    body_1 = (adapter_ack_body_t *)malloc(sizeof(adapter_ack_body_t) + 2);
	if(body_1== NULL)
	{
		return res;
	}
    //boby_2 = (switch_response_body_t* )malloc(sizeof(switch_response_body_t));
    body_1->head.cmd_type = ADAPTER_CMD_SWITCH;
    body_1->data[0] = state;
    body_1->data[1] = 0;
    if(0 == dev_adapter_cmd_write( &dev_adapter, 1 , body_1 , 2 , (uint8_t *)&boby_2))
    {
        res = 0;
    }
    free(body_1);
    return res;
}

int adapter_set_voltage_and_current(struct adapter_output_set_t *data)
{
    int res = -1;
    adapter_ack_body_t *body_1;
    switch_response_body_t boby_2;
    body_1 = (adapter_ack_body_t *)malloc(sizeof(adapter_ack_body_t) + 4);
	if(body_1== NULL)
	{
		return res;
	}
    //boby_2 = (switch_response_body_t* )malloc(sizeof(switch_response_body_t));
    body_1->head.cmd_type = ADAPTER_CMD_SET_VOLTAGE;
    body_1->data[0] = data->voltage_high;
    body_1->data[1] = data->voltage_low;
	body_1->data[2] = data->current_high;
    body_1->data[3] = data->current_low;
    if(0 == dev_adapter_cmd_write( &dev_adapter, 1, body_1 , 4 , (uint8_t *)&boby_2))
    {
        res = 0;
    }
    free(body_1);
    return res;
}


int adapter_read_ver(void)
{
    int res = -1;
    adapter_ack_body_t *body_1;
    switch_response_body_t boby_2;
    body_1 = (adapter_ack_body_t *)malloc(sizeof(adapter_ack_body_t) );
	if(body_1== NULL)
	{
		return res;
	}
    //boby_2 = (switch_response_body_t* )malloc(sizeof(switch_response_body_t));
    body_1->head.cmd_type = ADAPTER_CMD_READ_VER;
    if(0 == dev_adapter_cmd_write( &dev_adapter, 1, body_1 , 0 , (uint8_t *)&boby_2))
    {
        res = 0;
    }
    free(body_1);
    return res;
}


int adapter_read_info(struct adapter_user_info_t *user_body)
{
    int res = -1;

    adapter_ack_body_t *body_1;
    info_response_body_t boby_2;
    body_1 = (adapter_ack_body_t *)malloc(sizeof(adapter_ack_body_t) );
	if(body_1== NULL)
	{
		return res;
	}
    body_1->head.cmd_type = ADAPTER_CMD_INFO;
    if(0 == dev_adapter_cmd_write( &dev_adapter, 1, body_1 , 0 ,(uint8_t *)&boby_2 ))
    {
        if(0 != boby_2.info.response)
        {
            res = -2;
			free(body_1);
            return res;
        }
        res = 0;
        user_body->i_bat =  uint16_to_float(boby_2.info.i_bat);
        user_body->i_input =  uint16_to_float(boby_2.info.i_input);
        user_body->i_output =  uint16_to_float(boby_2.info.i_output);
        user_body->temp = uint16_to_float(boby_2.info.temp);
        user_body->type =  boby_2.info.type;
        user_body->v_input =  uint16_to_float(boby_2.info.v_input);
        user_body->v_output =  uint16_to_float(boby_2.info.v_output);
        user_body->warn =  boby_2.info.warn;

    }

	free(body_1);
    return res;
}


void adapter_init_config(void)
{
    os_queue_usart_adapter_rx = osMessageQueueNew(5, sizeof(adapter_pack_type), NULL);
    os_semaphore_adapter_tx =   osSemaphoreNew(1, 0, NULL);

    hal_uart_init(HAL_UART_ADAPTER,9600);
    hal_uart_receive_dma_start(HAL_UART_ADAPTER, adapter_rx_pack.data, sizeof(adapter_rx_pack.data));
    hal_uart_irq_register(HAL_UART_ADAPTER, HAL_UART_IRQ_RX_TIMEOUT, adapter_rx_complete_int_cb);
    dev_adapter.read_data = adapter_usart_rx_data;
    dev_adapter.write_data = adapter_usart_tx_data;


}


