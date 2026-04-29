/*
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * network.c
 * Network code for libartnet
 * Copyright (C) 2004-2007 Simon Newton
 *
 */

#include <errno.h>
#include "lwip/sockets.h"
#include "lwip/opt.h"
#include "lwip/udp.h"
#include "private.h"
#include "app_artnet.h"
#include "app_eth.h"

//typedef int socklen_t;
#define ARTNET_SEND_BUF_LEN  (1024)
#define ARTNET_SEND_BUF_HEAD (42)

struct t_artnet_send_buf
{
	unsigned char head[ARTNET_SEND_BUF_HEAD];
	unsigned char buf[ARTNET_SEND_BUF_LEN];
}__attribute__((packed));
struct t_artnet_send_buf g_artnet_send_buf;


enum { INITIAL_IFACE_COUNT = 10 };
enum { IFACE_COUNT_INC = 5 };
enum { IFNAME_SIZE = 32 }; // 32 sounds a reasonable size

typedef struct iface_s {
  struct sockaddr_in ip_addr;
  struct sockaddr_in bcast_addr;
  int8_t hw_addr[ARTNET_MAC_SIZE];
  char   if_name[IFNAME_SIZE];
  struct iface_s *next;
} iface_t;

unsigned long LOOPBACK_IP = 0x7F000001;


/*
 * Free memory used by the iface's list
 * @param head a pointer to the head of the list
 */
static void free_ifaces(iface_t *head) {
  iface_t *ift, *ift_next;

  for (ift = head; ift != NULL; ift = ift_next) {
    ift_next = ift->next;
    free(ift);
  }
}


/*
 * Add a new interface to an interface list
 * @param head pointer to the head of the list
 * @param tail pointer to the end of the list
 * @return a new iface_t or void
 */
static iface_t *new_iface(iface_t **head, iface_t **tail) {
  iface_t *iface = (iface_t*) calloc(1, sizeof(iface_t));

  if (!iface) {
//    artnet_error("%s: calloc error %s" , __FUNCTION__, strerror(errno));
    return NULL;
  }
  memset(iface, 0, sizeof(iface_t));

  if (!*head) {
    *head = *tail = iface;
  } else {
    (*tail)->next = iface;
    *tail = iface;
  }
  return iface;
}


/*
 * Set if_head to point to a list of iface_t structures which represent the
 * interfaces on this machine
 * @param ift_head the address of the pointer to the head of the list
 */
static int get_ifaces(iface_t **if_head) {
	int ret = ARTNET_ENET;
	iface_t *if_tail, *iface;
	*if_head = if_tail = NULL;
	
	iface = new_iface(if_head, &if_tail);
	if (iface) {
		// set ip address
		iface->ip_addr.sin_addr.s_addr = netif_get_ip_addr();
		// set broadcast address
		iface->bcast_addr.sin_addr.s_addr = netif_get_brdcast_addr();
		// set mac address
    
		netif_get_mac_addr(iface->hw_addr);
		// set interface name
		strncpy(iface->if_name, "eth0", 4);

		ret = ARTNET_EOK;
	}

	return ret;
}


/*
 * Scan for interfaces, and work out which one the user wanted to use.
 */
int artnet_net_init(node n, const char *preferred_ip) {
  iface_t *ift, *ift_head = NULL;
  struct in_addr wanted_ip;

  int found = FALSE;
  int i;
  int ret = ARTNET_EOK;
	
  (void)i;
  if ((ret = get_ifaces(&ift_head)))
    goto e_return;

  if (n->state.verbose) {
    for (ift = ift_head; ift != NULL; ift = ift->next) {
		inet_ntoa(ift->ip_addr.sin_addr);
		inet_ntoa(ift->bcast_addr.sin_addr);
    }
  }

  if (preferred_ip) {
    // search through list of interfaces for one with the correct address
    ret = artnet_net_inet_aton(preferred_ip, &wanted_ip);
    if (ret)
      goto e_cleanup;

    for (ift = ift_head; ift != NULL; ift = ift->next) {
      if (ift->ip_addr.sin_addr.s_addr == wanted_ip.s_addr) {
        found = TRUE;
        n->state.ip_addr = ift->ip_addr.sin_addr;
        n->state.bcast_addr = ift->bcast_addr.sin_addr;
        memcpy(&n->state.hw_addr, &ift->hw_addr, ARTNET_MAC_SIZE);
        break;
      }
    }
    if (!found) {
//      artnet_error("Cannot find ip %s", preferred_ip);
      ret = ARTNET_ENET;
      goto e_cleanup;
    }
  } else {
    if (ift_head) {
      // pick first address
      // copy ip address, bcast address and hardware address
      n->state.ip_addr = ift_head->ip_addr.sin_addr;
      n->state.bcast_addr = ift_head->bcast_addr.sin_addr;
      memcpy(&n->state.hw_addr, &ift_head->hw_addr, ARTNET_MAC_SIZE);
    } else {
//      artnet_error("No interfaces found!");
      ret = ARTNET_ENET;
    }
  }

e_cleanup:
  free_ifaces(ift_head);
e_return :
  return ret;
}


/*
 * Start listening on the socket
 */
int artnet_net_start(node n) {
  struct udp_pcb *Udppcb_svr;
  node tmp;
  int ret = 0;

  (void)ret;
  // only attempt to bind if we are the group master
  if (n->peering.master == TRUE) {

    // create socket
     Udppcb_svr = udp_new();

    if(Udppcb_svr == NULL) {
//      artnet_error("Could not create socket %s", artnet_net_last_error());
      return ARTNET_ENET;
    }

    if (n->state.verbose)
	 ;  
    // bind sockets
    if ((ret = udp_bind(Udppcb_svr, IP_ADDR_ANY, ARTNET_PORT)) != ERR_OK) {
//      artnet_error("Failed to bind to socket %d", ret);
      return ARTNET_ENET;
    }
	
	udp_recv(Udppcb_svr, artnet_receive_callback, NULL);
  
	 n->sd = (int)(Udppcb_svr);
    // Propagate the socket to all our peers
    for (tmp = n->peering.peer; tmp && tmp != n; tmp = tmp->peering.peer)
      tmp->sd = (int)(Udppcb_svr);

	return ARTNET_EOK;
	}
  return ARTNET_ENET;
}


/*
 * Receive a packet.
 */
int artnet_net_recv(node n, artnet_packet p, int delay) {
  return ARTNET_EOK;
}


/*
 * Send a packet.
 */
int artnet_net_send(node n, artnet_packet p) {
    struct udp_pcb *Udppcb_svr = (struct udp_pcb *)n->sd;
	struct pbuf *pack_buf;
	ip_addr_t dAddr;
	static int is_connected = 0;

	if (n->state.mode != ARTNET_ON)
	{
		return ARTNET_EACTION;
	}

	p->from = n->state.ip_addr;

	if (n->state.verbose)
	{
		;
	}

	dAddr.addr = p->to.s_addr;
	//dAddr.addr = p->from.s_addr;

	pack_buf = pbuf_alloc(PBUF_TRANSPORT, p->length, PBUF_RAM);
	memset(&g_artnet_send_buf, 0, sizeof(g_artnet_send_buf));
	memcpy(g_artnet_send_buf.buf, (unsigned char *)(&(p->data)), p->length);
	pack_buf->payload = g_artnet_send_buf.buf;

	if (is_connected == 0)
	{
		if (udp_connect(Udppcb_svr, &dAddr, ARTNET_PORT)!= ERR_OK)
		{
			__nop();
		}					
		else
		{
			is_connected = 1;
		}
	}
	//dAddr.addr = 0XFFFFFFFF;
	udp_sendto(Udppcb_svr, pack_buf, &dAddr, ARTNET_PORT);

	if (is_connected == 1)
	{
		udp_disconnect(Udppcb_svr);
		// bind sockets
	    if (udp_bind(Udppcb_svr, IP_ADDR_ANY, ARTNET_PORT) != ERR_OK)
		{
//	    	artnet_error("Failed to bind to socket");
	    	return ARTNET_ENET;
	    }
	
		udp_recv(Udppcb_svr, artnet_receive_callback, NULL);
	}

	pbuf_free(pack_buf);
	
	if (n->callbacks.send.fh) {
		get_type(p);
		n->callbacks.send.fh(n, p, n->callbacks.send.data);
	}
	
	return ARTNET_EOK;
}


/*
int artnet_net_reprogram(node n) {
  iface_t *ift_head, *ift;
  int i;

  ift_head = get_ifaces(n->sd[0]);

  for (ift = ift_head;ift != NULL; ift = ift->next ) {
    printf("IP: %s\n", inet_ntoa(ift->ip_addr.sin_addr) );
    printf("  bcast: %s\n" , inet_ntoa(ift->bcast_addr.sin_addr) );
    printf("  hwaddr: ");
      for(i = 0; i < 6; i++ ) {
        printf("%hhx:", ift->hw_addr[i] );
      }
    printf("\n");
  }

  free_ifaces(ift_head);

}*/


int artnet_net_set_fdset(node n, fd_set *fdset) {
  return ARTNET_EOK;
}


/*
 * Close a socket
 */
int artnet_net_close(artnet_socket_t sock) {
  return ARTNET_EOK;
}


/*
 * Convert a string to an in_addr
 */
int artnet_net_inet_aton(const char *ip_address, struct in_addr *address) {
  in_addr_t *addr = (in_addr_t*) address;
  if ((*addr = inet_addr(ip_address)) == INADDR_NONE &&
      strcmp(ip_address, "255.255.255.255")) {
//    artnet_error("IP conversion from %s failed", ip_address);
    return ARTNET_EARG;
  }
  return ARTNET_EOK;
}

/*
 *
 */
const char *artnet_net_last_error() {
  return strerror(errno);
}

