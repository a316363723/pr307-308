#include "artnet.h"
#include "private.h"
#include "lwip/api.h"
#include "app_dmx.h"
#include "app_data_center.h"

#ifndef TRUE
#define TRUE 1
#define FALSE 0
#endif

#define SYSTEMTICK_PERIOD_MS  10
#define ARTNET_INIT_OK (0xffff)
#define TEST_INTERVAL (0x7fff)

#define ArtNetShortName "ApuTure"
#define ArtNetLongtName "ApuTure ArtNet Device"

static const int MAXCHANNELS = 512;
typedef unsigned char dmx_t ;
dmx_t DmxBuffer[512];
artnet_node g_node;
static  uint8_t s_artnet_data_state;

void artnet_receive_callback(void *arg, struct udp_pcb *upcb, struct pbuf *p, const ip_addr_t *addr, u16_t port)
{
	ip_addr_t dAddr = *addr;
	node n = (node)g_node;
	node tmp;
	artnet_packet_t pac;
	
	memset(&pac.data, 0x0, sizeof(pac.data));
	pac.length = p->len;
	memcpy(&(pac.from), &dAddr, sizeof(struct in_addr));
	memcpy(&(pac.data), p->payload, p->len);

	check_timeouts(n);
	if (pac.length > MIN_PACKET_SIZE && get_type(&pac)) {
		handle(n, &pac);
		for (tmp = n->peering.peer; tmp != NULL && tmp != n; tmp = tmp->peering.peer) {
			handle(tmp, &pac);
		}
	}

	pbuf_free(p);

}

int dmx_handler(artnet_node n, int prt, void *d)
{
	int len = 0;
	uint8_t *data = NULL;
	
	data = artnet_read_dmx(n, prt, &len);
	if(data == NULL)
		return 0;
	memcpy(DmxBuffer, data, len);	
	if(s_artnet_data_state != 0)
		app_dmx_eth_write_data(DmxBuffer, len);
    
	return 0;
}

void artnet_set_ip_addr(ip4_addr_t ip_addr)
{
	char ip_str[16];
	strcpy(ip_str, inet_ntoa(ip_addr));
		
	artnet_net_init(g_node, ip_str);
	artnet_tx_build_art_poll_reply(g_node);
}

int artnet_set_universe_address(uint16_t address)
{
    int i;
    node n = (node)g_node;
    uint8_t net; 
    uint8_t sub_net;
    uint8_t universe;
    int ret;
    
    if(address > 32767)
    {
        return -1;
    }
    
    net = (address & 0x7f00) >> 8;
    sub_net = (address & 0x00f0) >> 4;
    universe = (address & 0x000f);
    
    // program subnet
    if (sub_net == PROGRAM_DEFAULTS) {
        // reset to defaults
        n->state.subnet = n->state.default_subnet;
        n->state.subnet_net_ctl = FALSE;

    } else{
        n->state.subnet = sub_net;
        n->state.subnet_net_ctl = TRUE;
    }
  
    // program netswitch
    if (net == PROGRAM_DEFAULTS) {
        // reset to defaults
        n->state.SubSwitch = n->state.default_net;
        n->state.subnet_net_ctl = FALSE;

    } else {
        n->state.SubSwitch = net;
        n->state.subnet_net_ctl = TRUE;
    }

    // if it does we need to change all port addresses
    for(i=0; i< ARTNET_MAX_PORTS; i++) {
        n->ports.in[i].port_addr = (n->state.SubSwitch << 8) | (n->state.subnet << 4) | (n->ports.in[i].port_addr & 0x0f);//_make_addr(n->state.subnet, n->ports.in[i].port_addr);
        n->ports.out[i].port_addr =(n->state.SubSwitch << 8) | (n->state.subnet << 4) | (n->ports.out[i].port_addr & 0x0f);//_make_addr(n->state.subnet, n->ports.out[i].port_addr);
    }
    
    // program swins
    for (i =0; i < ARTNET_MAX_PORTS; i++) {
      n->ports.in[i].port_addr = n->ports.in[i].port_addr & 0xfff0 | (universe & ~PROGRAM_CHANGE_MASK);//_make_addr(n->state.subnet, p->data.addr.swin[i]);
      n->ports.in[i].port_net_ctl = TRUE;
    }

    // program swouts
    for (i =0; i < ARTNET_MAX_PORTS; i++) {
            n->ports.out[i].port_addr = n->ports.out[i].port_addr & 0xfff0 | (universe & ~PROGRAM_CHANGE_MASK);//_make_addr(n->state.subnet, p->data.addr.swout[i]);
            n->ports.in[i].port_net_ctl = TRUE;
            n->ports.out[i].port_enabled = TRUE;
    }
    
    // reset sequence numbers if the addresses change
  for (i=0; i< ARTNET_MAX_PORTS; i++) {
//    if (addr[i] != n->ports.in[i].port_addr)
      n->ports.in[i].seq = 0;
  }
  
  if ((ret = artnet_tx_build_art_poll_reply(n)))
    return ret;

  return artnet_tx_poll_reply(n, TRUE);
}

void artnet_data_state_set(uint8_t state)
{
	s_artnet_data_state = state;
}

uint8_t artnet_init(uint8_t*  ip_addr)
{
	int subnet_addr = 0;
//	ip4_addr_t ip; 
	
	char ip_str[14];
	
	
//	IP_ADDR4(&ip, ip_addr[0], ip_addr[1], ip_addr[2], ip_addr[3]);
	
    sprintf(ip_str, "%u.%u.%u.%u", ip_addr[0], ip_addr[1], ip_addr[2], ip_addr[3]);
	
	if (g_node == NULL)
	{
		g_node = artnet_new(ip_str, 1); /* set up artnet connection */
	}
	if (g_node == NULL)
	{
		return 0;
	}
	if (ARTNET_EOK != artnet_set_node_type(g_node, ARTNET_NODE))
	{
		return 0;
	}
	if (artnet_set_dmx_handler(g_node, dmx_handler, NULL))
	{
		return 0;
	}
	artnet_set_subnet_addr(g_node, subnet_addr);

	artnet_set_port_type(g_node, 0, ARTNET_ENABLE_OUTPUT, ARTNET_PORT_DMX);
	artnet_set_port_addr(g_node, 0, ARTNET_OUTPUT_PORT, 0);
	artnet_set_port_addr(g_node, 1, ARTNET_OUTPUT_PORT, 1);
	artnet_set_port_addr(g_node, 2, ARTNET_OUTPUT_PORT, 2);
	artnet_set_port_addr(g_node, 3, ARTNET_OUTPUT_PORT, 3);

	artnet_set_port_addr(g_node, 0, ARTNET_INPUT_PORT, 0);
	artnet_set_port_addr(g_node, 1, ARTNET_INPUT_PORT, 1);
	artnet_set_port_addr(g_node, 2, ARTNET_INPUT_PORT, 2);
	artnet_set_port_addr(g_node, 3, ARTNET_INPUT_PORT, 3);
	
	artnet_set_short_name(g_node, ArtNetShortName);
	artnet_set_long_name(g_node, ArtNetLongtName);
	
	artnet_start(g_node);

	return 1;
}
