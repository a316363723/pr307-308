#ifndef  _USER_SACN_H_
#define  _USER_SACN_H_

#include <stdint.h>



uint8_t E131_Init(uint16_t universe);
uint8_t E131_Recv_Data(void);
void createMulticastListen(void);
void multicast_send_data(unsigned char * data,unsigned short len);
void sacn_set_universe(unsigned short universe);
void sacn_data_state_set(uint8_t state);

extern uint8_t Mul_Rev_Flag;
extern int sockfd;
int Socket_Config(void);
extern int addr_len;
extern struct sockaddr_in local_addr; 

#endif  /* _USER_SACN_H_ */
