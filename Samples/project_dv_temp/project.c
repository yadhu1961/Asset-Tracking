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

   LESSON 5: Flooding (broadcast)
*/

// Contiki-specific includes:
#include "contiki.h"
#include "net/rime/rime.h"     // Establish connections.
#include "net/netstack.h"      // Wireless-stack definitions
#include "dev/leds.h"          // Use LEDs.
#include "core/net/linkaddr.h"
// Standard C includes:
#include <stdio.h>      // For printf.
#include "platform/zoul/dev/button-sensor.h" // User Button
#include "dev/serial-line.h"
#include "routing_table.h"
#include "broadcast_custom.h"
#include "unicast_custom.h"


static const struct packetbuf_attrlist attributes_temp[] =
{
	  BROADCAST_CUSTOM_ATTRIBUTES
	  PACKETBUF_ATTR_LAST
};
static const struct packetbuf_attrlist attributes_unicast[] =
{
	  UNICAST_CUSTOM_ATTRIBUTES
	  PACKETBUF_ATTR_LAST
};

typedef enum {
	idle = 0,
	neighbour_discovery,
	asset_tracking,
	number_of_states
} states;

//This counter maintain how many types message is broadcasted
int broadcast_count = 0;
int rebroadcast_count = 0;

states state = neighbour_discovery;

enum PACKET_TYPE {BROADCAST,UNICAST,ASSET_TRACKING};
//------------------------ FUNCTIONS ------------------------
// Creates an instance of a broadcast connection.
static struct broadcast_conn broadcast;
// Creates an instance of a unicast connection.
struct unicast_conn unicastConn;

static void print_packet(char *packet_ptr,uint16_t packet_len)
{
	printf("here is the packet data: ");
	int i;
	for(i = 0 ;i<packet_len;i++)
		printf("%x ",packet_ptr[i]);
	printf("\r\n");
}
//static void print_header(char *packet_ptr,ui)

static void
broadcast_recv(struct broadcast_conn *c, const linkaddr_t *from) {
	leds_on(LEDS_GREEN);
	printf(" hops %d\r\n",packetbuf_attr(PACKETBUF_ATTR_HOPS));
	printf(" packet type %d\r\n",packetbuf_attr(PACKETBUF_ATTR_PACKET_TYPE));

	 printf("Broadcast message received from %x.%x:\r\n",from->u8[0], from->u8[1]);
	add_routing_entry(from,packetbuf_attr(PACKETBUF_ATTR_HOPS));
	leds_off(LEDS_GREEN);
}

static void
unicast_recv(struct unicast_conn *c, const linkaddr_t *from) {
	leds_on(LEDS_BLUE);
	printf("Unicast from: %x.%x, pkt_type %d\r\n",from->u8[0], from->u8[1],packetbuf_attr(PACKETBUF_ATTR_PACKET_TYPE));
	print_packet(packetbuf_dataptr(),packetbuf_datalen());
}

// Defines the functions used as callbacks for a broadcast connection.
static const struct unicast_callbacks unicast_call = {unicast_recv};

// Defines the functions used as callbacks for a broadcast connection.
static const struct broadcast_callbacks broadcast_call = {broadcast_recv};

static void check_for_invalid_addr(void) {
	// Boolean flag to check invalid address.
	static int iAmError = 0;

	// All-zeroes address.
	static linkaddr_t errAddr;
	errAddr.u8[0] = 0;
	errAddr.u8[1] = 0;

	// Check if this mote got an invalid address.
	iAmError = linkaddr_cmp(&errAddr, &linkaddr_node_addr);

	// Turn ON all LEDs if we loaded an invalid address.
	if(iAmError){
		printf("\nLoaded an invalid RIME address (0x%x%x)! "
				"Reset the device.\n\n",
				linkaddr_node_addr.u8[0],
				linkaddr_node_addr.u8[1]);

		// Freezes the app here. Reset needed.
		while (1){
			leds_on(LEDS_RED);
		}
	}
}


// Prints the current settings.
static void print_settings(void){
	radio_value_t channel;

	NETSTACK_CONF_RADIO.get_value(RADIO_PARAM_CHANNEL,&channel);

	printf("\n-------------------------------------\n");
	printf("RIME addr = \t0x%x%x\n",
			linkaddr_node_addr.u8[0],
			linkaddr_node_addr.u8[1]);
	printf("Using radio channel %d\n", channel);
	printf("---------------------------------------\n");
}

//--------------------- PROCESS CONTROL BLOCK ---------------------
PROCESS(main_process, "Always Active process");
PROCESS(route_share_process, "Routing table sharing");
AUTOSTART_PROCESSES(&main_process,&route_share_process);

//------------------------ PROCESS' THREAD ------------------------
PROCESS_THREAD(main_process, ev, data) {

	PROCESS_EXITHANDLER(broadcast_close(&broadcast);)
	PROCESS_BEGIN();

	/* Configure the user button */
	button_sensor.configure(BUTTON_SENSOR_CONFIG_TYPE_INTERVAL, CLOCK_SECOND);

	static uint8_t timer_interval = 5;	// In seconds

	static struct etimer et;
	static int *data_ptr = NULL;

	// Configure your team's channel (11 - 26).
	NETSTACK_CONF_RADIO.set_value(RADIO_PARAM_CHANNEL,13);

	print_settings();
	check_for_invalid_addr();

	// Open broadcast connection.
	unicast_open(&unicastConn, 146, &unicast_call);
	channel_set_attributes(146, attributes_unicast);
	// Open broadcast connection.
	broadcast_open(&broadcast, 128, &broadcast_call);
	channel_set_attributes(128, attributes_temp);


	while(1) {
		etimer_set(&et, CLOCK_SECOND);

    	/* NOTE: Contiki processes cannot start loops that never end.
    	 * In order to stay safe, we need to wait for an event in here.
    	 * When a Contiki process waits for events, it returns control
    	 * to the Contiki kernel.
    	 * The Contiki kernel will service other processes while this
    	 * process is waiting.
    	 *
    	 */
    	PROCESS_WAIT_EVENT();

    	packetbuf_copyfrom("Wake_Up",7);
    	packetbuf_set_attr(PACKETBUF_ATTR_HOPS,4);
    	packetbuf_set_attr(PACKETBUF_ATTR_PACKET_TYPE,1);
    	broadcast_send(&broadcast);
    	//print_routing_table();
	}
	PROCESS_END();
}

const linkaddr_t temp_addr = {{0x8e, 0xf5}};

//------------------------ PROCESS' THREAD ------------------------

// ROUTING TABLE  process:

PROCESS_THREAD(route_share_process, ev, data) {
	PROCESS_EXITHANDLER( printf("route_share_process terminated!\r\n");)
    PROCESS_BEGIN();

	static struct etimer et;
	etimer_set(&et, CLOCK_SECOND*ROUTE_SHARE_INTERVAL);
	while (1){
		PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));
		/* If timer expired, send routing data to all neighbors*/
		share_routing_table();
		/* Reset the etimer to trigger again in ROUTE_SHARE_INTERVAL seconds*/
		etimer_reset(&et);
	}
	PROCESS_END();
}
