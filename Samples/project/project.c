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

#define PACKETBUF_CONF_WITH_PACKET_TYPE

typedef enum {
	gateway_node,
	normal_node
}node_types;

typedef enum {
	idle = 0,
	neighbour_discovery,
	asset_tracking,
	number_of_states
} states;

typedef struct
{
	//This is used for storing the path
	//through which the packet travels
	linkaddr_t path[7];
	uint8_t hops_travelled;
	int16_t RSSI;
}asset_tracking_packets;

static int p;

//Sample asset tracking packet for debugging purpose
//Must be removed later
asset_tracking_packets sample_asset_packet;
int entries_count = 0;

//This counter maintain how many types message is broadcasted
int broadcast_count = 0;
int rebroadcast_count = 0;

const linkaddr_t receiver = {{0x8e, 0xf5}};

typedef struct
{
	uint8_t esender[2];
	uint8_t  hopcount;
	uint8_t packet_type;
	uint8_t packet_id;
}broadcast_packets;

states state = neighbour_discovery;
node_types node_type = gateway_node;	//gateway_node normal_node;
static uint8_t packet_type = 0x00;
static uint8_t packet_id = 0;

static broadcast_packets recv_packet;
static broadcast_packets broadcast_packet;
static asset_tracking_packets asset_tracking_packet;

//------------------------ FUNCTIONS ------------------------
// Creates an instance of a broadcast connection.
static struct broadcast_conn broadcast;
// Creates an instance of a broadcast connection.
static struct unicast_conn unicastConn;

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
//	 printf("Broadcast message received from 0x%x%x:[RSSI %d]\r\n",
//			 from->u8[0], from->u8[1],
//			(int16_t)packetbuf_attr(PACKETBUF_ATTR_RSSI));
	leds_off(LEDS_GREEN);
	packetbuf_copyto(&recv_packet);
	//Function for debugging purpose
//	print_packet(packetbuf_dataptr(),packetbuf_datalen());
//
//	printf("Packet type %x\r\n",packet_type);
//	printf("Number of hops %x\r\n",recv_packet.hopcount);

	packet_type = recv_packet.packet_type;

	printf("packet esender %x %x \r\n",recv_packet.esender[0],recv_packet.esender[1]);
	if(packet_type == 0x0A && node_type == normal_node)
	{

		if(recv_packet.packet_id < packet_id )
		{
			printf("Printf duplicate control packet\r\n");
			return;
		}
		else
		{
			printf("New control packet update the packet id\r\n");
			packet_id = recv_packet.packet_id;
		}

		//If this the case then it is a neighbour discovery packet hence update the routing
		//table
		printf("Adding entry to the routing table\r\n");
		add_routing_entry(from,recv_packet.hopcount);
		entries_count++;
		if(entries_count == 4)
		{
			sample_asset_packet.hops_travelled = 1;
			sample_asset_packet.path[0].u16 = linkaddr_node_addr.u16;
			packetbuf_copyfrom(&sample_asset_packet,sizeof(sample_asset_packet));
			unicast_send(&unicastConn,get_nearest_neighbor());
			//print_routing_table();
		}
		//here I am incrementing the hop and re-broadcasting it so the
		//Other neighbors of mine can receive
		if(rebroadcast_count <= 4) {
			recv_packet.hopcount++;
			packetbuf_copyfrom(&recv_packet,4);
			broadcast_send(&broadcast);
			rebroadcast_count++;
		}
	}
	else if(packet_type == 0x0B && node_type == normal_node)
	{
		//If this is the case then this is a asset tracking packet
		//So unicast it to nearest neighbor.
		//packetbuf_copyto(&asset_tracking_packet);
		asset_tracking_packet.hops_travelled = 0;
		memcpy(&asset_tracking_packet.path[asset_tracking_packet.hops_travelled],from,2);
		asset_tracking_packet.hops_travelled++;
		memcpy(&asset_tracking_packet.path[asset_tracking_packet.hops_travelled],&linkaddr_node_addr.u16,2);
		asset_tracking_packet.hops_travelled++;
		asset_tracking_packet.RSSI = (int16_t)packetbuf_attr(PACKETBUF_ATTR_RSSI);
		packetbuf_copyfrom(&asset_tracking_packet,sizeof(asset_tracking_packet));
		//unicast_send(&unicastConn,get_nearest_neighbor());
		//unicast_send(&unicastConn,&receiver);
	}
	else if(packet_type == 0x0B && node_type == gateway_node)
	{
		//Here i will print the message in the particular format so that
		//Qt can read the info
		//printf("Identfied an asset \r\n");
		packetbuf_copyto(&asset_tracking_packet);
		//printf("Route for the asset with address : %x%x \r\n", asset_tracking_packet.path[0].u8[0],asset_tracking_packet.path[0].u8[1]);
		printf(">>Asset");
		printf(">>%x%x%d%x%x\r\n", asset_tracking_packet.path[0].u8[0],asset_tracking_packet.path[0].u8[1],
				(int16_t)packetbuf_attr(PACKETBUF_ATTR_RSSI),linkaddr_node_addr.u8[0],linkaddr_node_addr.u8[1]);
	}
}

static void
unicast_recv(struct unicast_conn *c, const linkaddr_t *from) {
	leds_on(LEDS_BLUE);
//	printf("Unicast message received from 0x%x%x: [RSSI %d]\r\n",
//			from->u8[0], from->u8[1],
//			(int16_t)packetbuf_attr(PACKETBUF_ATTR_RSSI));
	//print_packet(packetbuf_dataptr(),packetbuf_datalen());
	//packetbuf_copyto(&recv_packet);
	//packet_type = recv_packet.packet_type;
	if(packet_type == 0x0B && node_type == normal_node)
	{
		//If this is the case then this is a asset tracking packet
		//So unicast it to nearest neighbor.
		packetbuf_copyto(&asset_tracking_packet);
		memcpy(&asset_tracking_packet.path[asset_tracking_packet.hops_travelled],from,2);
		asset_tracking_packet.hops_travelled++;
		packetbuf_copyfrom(&asset_tracking_packet,sizeof(asset_tracking_packet));
		unicast_send(&unicastConn,get_nearest_neighbor());

	}
	//else if(packet_type == 0x0B && node_type == gateway_node)
		else if(node_type == gateway_node)
	{
		//Here i will print the message in the particular format so that
		//Qt can read the info
		packetbuf_copyto(&asset_tracking_packet);
		//printf("Route for the asset with address : %x%x \r\n", asset_tracking_packet.path[0].u8[0], asset_tracking_packet.path[0].u8[1]);
		printf("Hops travelled: %x \r\n", asset_tracking_packet.hops_travelled);
		printf(">>Asset");
		printf("%x%x%d", asset_tracking_packet.path[0].u8[0], asset_tracking_packet.path[0].u8[1], asset_tracking_packet.RSSI);
		for(p = 1; p < asset_tracking_packet.hops_travelled;p++)
		{
			printf("%x%x\r\n", asset_tracking_packet.path[p].u8[0], asset_tracking_packet.path[p].u8[1]);

		}
		//printf("Tracking message received for gateway node\r\n");
	}
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
PROCESS(flooding_process, "Lesson 5: Flooding");
AUTOSTART_PROCESSES(&flooding_process);

//------------------------ PROCESS' THREAD ------------------------
PROCESS_THREAD(flooding_process, ev, data) {

	PROCESS_EXITHANDLER(broadcast_close(&broadcast);)
	PROCESS_BEGIN();

	/* Configure the user button */
	button_sensor.configure(BUTTON_SENSOR_CONFIG_TYPE_INTERVAL, CLOCK_SECOND);

	static uint8_t timer_interval = 5;	// In seconds

	static struct etimer et;
	static int *data_ptr = NULL;

	// Configure your team's channel (11 - 26).
	NETSTACK_CONF_RADIO.set_value(RADIO_PARAM_CHANNEL,13);
	NETSTACK_CONF_RADIO.set_value(RADIO_PARAM_TXPOWER, -7);

	print_settings();
	check_for_invalid_addr();

	// Open broadcast connection.
	unicast_open(&unicastConn, 146, &unicast_call);

	// Open broadcast connection.
	broadcast_open(&broadcast, 129, &broadcast_call);

	while(1) {
		etimer_set(&et, CLOCK_SECOND * timer_interval);

    	/* NOTE: Contiki processes cannot start loops that never end.
    	 * In order to stay safe, we need to wait for an event in here.
    	 * When a Contiki process waits for events, it returns control
    	 * to the Contiki kernel.
    	 * The Contiki kernel will service other processes while this
    	 * process is waiting.
    	 *
    	 */
    	PROCESS_WAIT_EVENT();

    	//if(ev == serial_line_event_message) {
        	/* Event from the User button */
        	if(ev == sensors_event) {
            	/* Event from the User button */
    			if(data == &button_sensor) {
    		//data_ptr = (int *)data;
			//if(atoi(data) == 4) {
				/* Button was pressed */
				if(button_sensor.value(BUTTON_SENSOR_VALUE_TYPE_LEVEL) == BUTTON_SENSOR_PRESSED_LEVEL) {
					printf("Button pressed! for two seconds\r\n");
					state = neighbour_discovery;
					node_type = gateway_node;
				}
			}
    	}
    	else if(ev == PROCESS_EVENT_TIMER)
    	{
    		if(state == idle) {
    			printf("state is idle mode\r\n");
    			continue;
    		}
    		else if(state == neighbour_discovery && node_type == gateway_node && broadcast_count < 5)
    		{
    			printf("state is neighbour_discovery broadcasting a message\r\n");
    			leds_on(LEDS_RED);
    			broadcast_packet.esender[0] = linkaddr_node_addr.u8[0];
    			broadcast_packet.esender[1] = linkaddr_node_addr.u8[1];
    			broadcast_packet.hopcount = 0;
    			broadcast_packet.packet_type = 0x0A;
    			packetbuf_copyfrom(&broadcast_packet, 5);
    			//packetbuf_set_attr(PACKETBUF_ATTR_HOPS, random_rand()%7);
    			//here 0x0A means network discovery mode.
    			//packetbuf_set_attr(PACKETBUF_ATTR_PACKET_TYPE,0x0A);
    			//packetbuf_set_addr(PACKETBUF_ADDR_ESENDER, &linkaddr_node_addr);
    			broadcast_send(&broadcast);
    			broadcast_packet.packet_id++;
    			//unicast_send(&unicastConn,&receiver);
    			printf("Broadcast message sent.\r\n");
    			leds_off(LEDS_RED);
    			broadcast_count++;
    		} else if(broadcast_count == 4) {
    			state = asset_tracking;
    		}
    		print_routing_table();
    	}
	}
	PROCESS_END();
}
