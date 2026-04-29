#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <lwip/err.h>
#include <lwip/inet.h>
#include <string.h>
#include "e131.h"
#include "user_sacn.h"
#include "app_dmx.h"
#include <assert.h>

static int sockfd;
static uint8_t s_sacn_data_state;
static uint16_t last_universe;

uint8_t E131_Init(uint16_t universe)
{
	int8_t temp = 0;
	
    /* create a socket for E1.31 */
    if ((sockfd = e131_socket()) < 0)
	{
		//DEBUG("create Fail\n");
        return 1;
	}
	
	/* bind the socket to the default E1.31 port and join multicast group for universe 1 */
    if (e131_bind(sockfd, E131_DEFAULT_PORT) < 0)
	{
		//DEBUG("bind Fail\n");
        return 2;
	}
	
	/* Join a socket file descriptor to an E1.31 multicast group using a universe */
	if ((temp = e131_multicast_join(sockfd, universe)) < 0)
	{
		//DEBUG("Join Fail=%d\n",temp);
        return 3;
	}
	last_universe = universe;
	/* e1.31 初始化成功 */
	return 0; 
}
void sacn_set_universe(unsigned short universe)
{
    g_sacn_universe = universe;
	int temp = 0;
	
	/* bind the socket to the default E1.31 port and join multicast group for universe 1 */
    e131_bind(sockfd, E131_DEFAULT_PORT);
	el31_clear_join(sockfd, last_universe);
	
	temp = e131_multicast_join(sockfd, universe);
	
	last_universe = universe;
}

void sacn_data_state_set(uint8_t state)
{
	s_sacn_data_state = state;
}

uint8_t sACN_Buff[513] = {0};
/* 放在中断里面或高优先级任务中，循环调用 */
uint8_t E131_Recv_Data(void)
{

	e131_packet_t e131_packet;
	memset(&e131_packet.dmp.prop_val, 0, sizeof(e131_packet.dmp.prop_val));
	e131_packet.dmp.prop_val_cnt = 0;
	e131_error_t error;
	uint8_t last_seq = 0x00;
	
	/* 从socket文件接收一个数据包 */
	if(s_sacn_data_state == 0)
		return 0xF;
	if (e131_recv(sockfd, &e131_packet) < 0)
      return 0xF; 	//假设错误返回值设为15
	
	/* 验证这个数据包是否正确 */
	if ((error = e131_pkt_validate(&e131_packet)) != E131_ERR_NONE) 
	{
	    /* 返回这个错误类型 */
        return error; //fprintf(stderr, "e131_pkt_validate: %s\n", e131_strerror(error));
    }
	
	/* 检查这个数据包是否要丢弃，如果乱序就丢弃 */
//    if (e131_pkt_discard(&e131_packet, last_seq)) 
//	{
//        last_seq = e131_packet.frame.seq_number;
//	    /* 打印这个乱序的序号 */
//        return last_seq;
//    }
	
	/* 参数：标砖错误流；转存一个数据包到error Stream */
    //e131_pkt_dump(stderr, &e131_packet);
	
	/* e131_packet.dmp.prop_val 这个变量是接收到数据的buff */
	memcpy(sACN_Buff, e131_packet.dmp.prop_val + 1, ntohs(e131_packet.dmp.prop_val_cnt) - 1);
	app_dmx_eth_write_data(sACN_Buff, ntohs(e131_packet.dmp.prop_val_cnt) - 1);
	//lwip_dmx_data(sACN_Buff ,ntohs(e131_packet.dmp.prop_val_cnt) - 1);
//	DEBUG("cnt=%d\r\n", ntohs(e131_packet.dmp.prop_val_cnt) - 1);
//	for(uint8_t i=0; i<5; i++)
//	{
//		DEBUG("val1=%d\n", *(e131_packet.dmp.prop_val + 1 + i));
		//DEBUG("val[%d]=%d\n", i, *(sACN_Buff + i));
//	}
    last_seq = e131_packet.frame.seq_number;
	
	return 0;
}


