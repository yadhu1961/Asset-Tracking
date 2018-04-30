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
#include "lib/list.h"
#include "lib/memb.h"
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
struct example_neighbor {
	struct example_neighbor *next;
	linkaddr_t addr;
	struct ctimer ctimer;
};
#define NEIGHBOR_TIMEOUT 60 * CLOCK_SECOND
#define MAX_NEIGHBORS 16
LIST(neighbor_table);
MEMB(neighbor_mem, struct example_neighbor, MAX_NEIGHBORS);
static void
remove_neighbor(void *n)
{
  struct example_neighbor *e = n;

  list_remove(neighbor_table, e);
  memb_free(&neighbor_mem, e);
}
enum modes {NETWORK_DISCOVERY_MODE,ROUTING_DISCOVERY_MODE};
enum modes mode;

static void
broadcast_ndm_recv(struct broadcast_conn *c, const linkaddr_t *from)
{
	uint16_t hops;
	struct example_neighbor *e;
	hops= packetbuf_attr(PACKETBUF_ATTR_HOPS);
	switch(mode){
	case NETWORK_DISCOVERY_MODE:
		printf("NDM_RECV\r\n");
		if(!strcmp("Wake_Up",(char *)packetbuf_dataptr())){
			wake_up_received=1;
			//mode=ROUTING_DISCOVERY_MODE;
			printf("I can Reach %d.%d: in '%d'\r\n",from->u8[0], from->u8[1], hops);
		    printf("Neighours_Now\r\n");
		    for(e = list_head(neighbor_table); e != NULL; e = e->next) {
			    printf("%d.%d\r\n",e->addr.u8[0],e->addr.u8[1]);
		    }
		    /* We received an announcement from a neighbor so we need to update
		     the neighbor list, or add a new entry to the table. */
		    for(e = list_head(neighbor_table); e != NULL; e = e->next) {
		        if(linkaddr_cmp(from, &e->addr)) {
		    	    /* Our neighbor was found, so we update the timeout. */
		            ctimer_set(&e->ctimer, NEIGHBOR_TIMEOUT, remove_neighbor, e);
		            return;
		        }
		    }
			/* The neighbor was not found in the list, so we add a new entry by
			 allocating memory from the neighbor_mem pool, fill in the
			 necessary fields, and add it to the list. */
		    e = memb_alloc(&neighbor_mem);
		    if(e != NULL) {
			  linkaddr_copy(&e->addr, from);
			  list_add(neighbor_table, e);
			  ctimer_set(&e->ctimer, NEIGHBOR_TIMEOUT, remove_neighbor, e);
		    }
		  }
		  else{
		      printf("Wake_Up not recieved\r\n");
		  }
		  break;
	case ROUTING_DISCOVERY_MODE:
		printf("RDM_RECV\r\n");
		break;
	}
}
static const struct broadcast_callbacks broadcast_cbks = {broadcast_ndm_recv};
static struct broadcast_conn broadcast;
/*** MAIN THREAD ***/
PROCESS_THREAD(simple_packet_process, ev, data) {

	  static struct etimer et;

	  PROCESS_EXITHANDLER(broadcast_close(&broadcast);)

	  PROCESS_BEGIN();
	  NETSTACK_CONF_RADIO.set_value(RADIO_PARAM_CHANNEL, 13);
	  broadcast_open(&broadcast, 128, &broadcast_cbks);
	  channel_set_attributes(128, attributes_temp);
	  mode=NETWORK_DISCOVERY_MODE;

	while(1){

	    /* Delay 2-4 seconds */
	    etimer_set(&et, CLOCK_SECOND * 2 + random_rand() % (CLOCK_SECOND * 2));

	    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));
	    switch(mode){
	    case NETWORK_DISCOVERY_MODE:
	    	printf("NDM\r\n");
		    packetbuf_copyfrom("Wake_Up", 7);
		    packetbuf_set_attr(PACKETBUF_ATTR_HOPS, random_rand()%7);
		    broadcast_send(&broadcast);
		    printf("NDM message sent\n");
	    	break;
	    case ROUTING_DISCOVERY_MODE:
	    	printf("RDM\r\n");
		    packetbuf_copyfrom("Routing", 7);
		    packetbuf_set_attr(PACKETBUF_ATTR_HOPS, random_rand()%7);
		    broadcast_send(&broadcast);
		    printf("RDM message sent\n");
	    	break;

	    }
	}

	PROCESS_END();
}

