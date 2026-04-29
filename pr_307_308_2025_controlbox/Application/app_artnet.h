#ifndef APP_ARTNET_H
#define APP_ARTNET_H

#include "lwip/sockets.h"
#include "lwip/opt.h"
#include "lwip/udp.h"

uint8_t artnet_init(uint8_t* ip_addr);
void    artnet_receive_callback(void *arg, struct udp_pcb *upcb, struct pbuf *p, const ip_addr_t *addr, u16_t port);
void 	artnet_set_ip_addr(ip4_addr_t ip_addr);
int     artnet_set_universe_address(uint16_t address);
void artnet_data_state_set(uint8_t state);

#endif /*endif APP_ARTNET_H*/
