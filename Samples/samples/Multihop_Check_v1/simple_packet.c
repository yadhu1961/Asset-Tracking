/*
   Wireless Sensor Networks Laboratory

   Technische Universität München
   Lehrstuhl für Kommunikationsnetze
   http://www.lkn.ei.tum.de

   copyright (c) 2017 Chair of Communication Networks, TUM

   contributors:
   * Thomas Szyrkowiec
   * Mikhail Vilgelm
   * Octavio Rodríguez Cervantes
   * Angel Corona

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, version 2.0 of the License.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.

   LESSON 2: Simple packet
*/

// Contiki-specific includes:
#include "contiki.h"
#include "net/rime/rime.h"	// Establish connections.
#include "net/netstack.h"
#include "lib/random.h"
#include "dev/leds.h"

// Standard C includes:
#include <stdio.h>
#include <stdint.h>

#include "broadcast_custom.h"
/*---------------------------------------------------------------------------*/
/*  MAIN PROCESS DEFINITION  												 */
/*---------------------------------------------------------------------------*/
PROCESS(simple_packet_process, "Custom Broadcast example");
AUTOSTART_PROCESSES(&simple_packet_process);
static int wake_up_received=0;
static const struct packetbuf_attrlist attributes_temp[] =
  {
		  BROADCAST_CUSTOM_ATTRIBUTES
		  PACKETBUF_ATTR_LAST
  };
static void
broadcast_custom_recv(struct broadcast_custom_conn *c, const linkaddr_t *from)
{
	  uint16_t hops;
	  hops= packetbuf_attr(PACKETBUF_ATTR_HOPS);
	  //printf("hops '%d'\r\n", hops);
  //printf("broadcast message received from %d.%d: '%s'\r\n",
    //     from->u8[0], from->u8[1], (char *)packetbuf_dataptr());
	  if(wake_up_received==0){
		  if(!strcmp("Wake_Up",(char *)packetbuf_dataptr())){
			  wake_up_received=1;
			  printf("I can Reach %d.%d: in '%d'\r\n",
					 from->u8[0], from->u8[1], hops);
		  }
	  }
	  else{
		  printf("Wake_Up not recieved\r\n");
	  }

}
static const struct broadcast_custom_callbacks broadcast_custom_call = {broadcast_custom_recv};
static struct broadcast_custom_conn broadcast_custom;

/*** MAIN THREAD ***/
PROCESS_THREAD(simple_packet_process, ev, data) {

	  static struct etimer et;

	  PROCESS_EXITHANDLER(broadcast_custom_close(&broadcast_custom);)

	  PROCESS_BEGIN();
	  NETSTACK_CONF_RADIO.set_value(RADIO_PARAM_CHANNEL, 13);
	  //broadcast_custom_open(&broadcast_custom, 128, &broadcast_custom_call);
	  broadcast_open(&broadcast_custom, 128, &broadcast_custom_call);
	  channel_set_attributes(128, attributes_temp);

	while(1){

	    /* Delay 2-4 seconds */
	    etimer_set(&et, CLOCK_SECOND * 2 + random_rand() % (CLOCK_SECOND * 2));

	    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));

	    packetbuf_copyfrom("Wake_Up", 7);
	    //broadcast_custom_send(&broadcast_custom);
	    packetbuf_set_attr(PACKETBUF_ATTR_HOPS, random_rand()%7);
	    broadcast_send(&broadcast_custom);
	    printf("abc message sent\n");
	}

	PROCESS_END();
}

/*following section is taken from other file broadcast_custom.c, as i couldn't get how to add that file to project*/
#define DEBUG 0
#if DEBUG
#include <stdio.h>
#define PRINTF(...) printf(__VA_ARGS__)
#else
#define PRINTF(...)
#endif

/*---------------------------------------------------------------------------*/
static const struct packetbuf_attrlist attributes[] =
  {
		  BROADCAST_CUSTOM_ATTRIBUTES
		  PACKETBUF_ATTR_LAST
  };
/*---------------------------------------------------------------------------*/
static void
recv_from_abc(struct abc_conn *bc)
{
  linkaddr_t sender;
  struct broadcast_custom_conn *c = (struct broadcast_custom_conn *)bc;

  linkaddr_copy(&sender, packetbuf_addr(PACKETBUF_ADDR_SENDER));
  PRINTF("%d.%d: broadcast: from %d.%d\n",
	 linkaddr_node_addr.u8[0],linkaddr_node_addr.u8[1],
	 sender.u8[0], sender.u8[1]);
  if(c->u->recv) {
    c->u->recv(c, &sender);
  }
}
/*---------------------------------------------------------------------------*/
static void
sent_by_abc(struct abc_conn *bc, int status, int num_tx)
{
  struct broadcast_custom_conn *c = (struct broadcast_custom_conn *)bc;

  PRINTF("%d.%d: sent to %d.%d status %d num_tx %d\n",
	 linkaddr_node_addr.u8[0],linkaddr_node_addr.u8[1],
	 packetbuf_addr(PACKETBUF_ADDR_SENDER)->u8[0],
         packetbuf_addr(PACKETBUF_ADDR_SENDER)->u8[1],
         status, num_tx);
  if(c->u->sent) {
    c->u->sent(c, status, num_tx);
  }
}
/*---------------------------------------------------------------------------*/
static const struct abc_callbacks broadcast_custom_internal = {recv_from_abc, sent_by_abc};
/*---------------------------------------------------------------------------*/
void
broadcast_custom_open(struct broadcast_custom_conn *c, uint16_t channel,
	  const struct broadcast_custom_callbacks *u)
{
  abc_open(&c->c, channel, &broadcast_custom_internal);
  c->u = u;
  channel_set_attributes(channel, attributes);
}
/*---------------------------------------------------------------------------*/
void
broadcast_custom_close(struct broadcast_custom_conn *c)
{
  abc_close(&c->c);
}
/*---------------------------------------------------------------------------*/
int
broadcast_custom_send(struct broadcast_custom_conn *c)
{
	  PRINTF("%d.%d: broadcast_send\n",
		 linkaddr_node_addr.u8[0],linkaddr_node_addr.u8[1]);
  packetbuf_set_addr(PACKETBUF_ADDR_SENDER, &linkaddr_node_addr);
  packetbuf_set_attr(PACKETBUF_ATTR_HOPS, random_rand()%7);
  return abc_send(&c->c);
}

