// Contiki-specific includes:
#include "contiki.h"
#include "net/rime/rime.h"     // Establish connections.
#include "net/netstack.h"      // Wireless-stack definitions
#include "dev/leds.h"          // Use LEDs.
#include "core/net/linkaddr.h"
#include "core/net/packetbuf.h"
#include "string.h"
// Standard C includes:
#include <stdio.h>      // For printf.
#include "platform/zoul/dev/button-sensor.h" // User Button
#include "dev/serial-line.h"
#include "routing_table.h"
#include "broadcast_custom.h"
#include "unicast_custom.h"
#include "lib/random.h"


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

//Function declarations
static bool inform_neighbor(const linkaddr_t * address);

//This counter maintain how many types message is broadcasted
int broadcast_count = 0;
int rebroadcast_count = 0;
int gBatteryVoltage=3200;
int gBatteryVoltage_emulated=3200;
bool gBroadcast_ended=false;
states state = neighbour_discovery;

enum PACKET_TYPE {BROADCAST,UNICAST,ASSET_TRACKING};

//----------------------
//Temporary things
const linkaddr_t tmp_nearest_node = {{0x00,0x01}};

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
		printf("%x",packet_ptr[i]);
	printf("\r\n");
}

//This function is called only for gateway
static bool process_packet_gateway(char *packet_ptr,uint16_t packet_len)
{
	printf("%s\r\n",__func__);
	static uint16_t *rssi_ptr,prev_rssi = 0,current_rssi = 0;
	linkaddr_t prev_addr = {0},current_addr = {0} ;
	rssi_ptr = (uint16_t *)packet_ptr + 5;
	current_rssi = *rssi_ptr;
	linkaddr_copy(&current_addr,(linkaddr_t *)(packet_ptr+7));

	memcpy(packet_ptr+packet_len,&linkaddr_node_addr,2);
	packet_len += 2;
	printf("rssi %d asset_addr %x.%x\r\n",current_rssi,current_addr.u8[0],current_addr.u8[1]);

	if(!linkaddr_cmp(&current_addr,&prev_addr))
	{

		if(current_rssi > (prev_rssi + 30) )
		{
			linkaddr_copy(&prev_addr,&current_addr);
			prev_rssi = current_rssi;
			print_packet(packet_ptr,packet_len);
		}
	}
	else
		print_packet(packet_ptr,packet_len);
	return true;
}


#ifndef ASSET_MOTE
static bool forward_data(const linkaddr_t *from,bool is_broadcast)
{
	if(DEBUG)printf("%s\r\n",__func__);
	static uint16_t rssi = 0;
	static char packet_data[128];
	static uint8_t packet_len = 0;
	packet_len = packetbuf_copyto(&packet_data);
	if(is_broadcast)
	{
		rssi = packetbuf_attr(PACKETBUF_ATTR_RSSI);
//		printf("rssi at forward data :%x\r\n",rssi);
		memcpy(packet_data+packet_len,&rssi,2);
		packet_len +=2;
	}
	memcpy(packet_data+packet_len,from,2);
	packet_len += 2;
//	print_packet(packet_data,packet_len);
	packetbuf_copyfrom(packet_data,packet_len);
	packetbuf_set_attr(PACKETBUF_ATTR_PACKET_TYPE,ASSET_TRACKING);
	//Later unicast it to nearby nearest neighbor
	unicast_send(&unicastConn,&nearest_neighbor.addr);
	return true;
}
#endif //ASSET_MOTE


static void
broadcast_recv(struct broadcast_conn *c, const linkaddr_t *from) {
#ifndef ASSET_MOTE
	leds_on(LEDS_GREEN);
//	//printf(" hops %d\r\n",packetbuf_attr(PACKETBUF_ATTR_HOPS));
//	printf(" packet type %d\r\n",packetbuf_attr(PACKETBUF_ATTR_PACKET_TYPE));
//
	 printf("Broadcast_msg_received_from:[%x.%x]:[Cost %d][RSSI: %d]\r\n",
			 from->u8[0], from->u8[1],
			(int16_t)packetbuf_attr(PACKETBUF_ATTR_HOPS), packetbuf_attr(PACKETBUF_ATTR_RSSI));
	 //Here i will add only the neighbor entry to the table
	 //with infinite hop count
	 if(BROADCAST == packetbuf_attr(PACKETBUF_ATTR_PACKET_TYPE))
	 {
		 //Adding neighbors to the list
		 add_routing_entry(from,INFINITE_HOP_COUNT);
		 if(i_am_gateway)
		 {
			 inform_neighbor(from);
		 }
	 } else if(ASSET_TRACKING == packetbuf_attr(PACKETBUF_ATTR_PACKET_TYPE))
	 {
		//here i will forward the data to the next hop.
		printf("asset data packet recvd\r\n");
		if(!i_am_gateway)
			forward_data(from,true);
		else
		{
			printf("printing asset stat for Qt\r\n");
			uint16_t rssi;
			rssi = packetbuf_attr(PACKETBUF_ATTR_RSSI);
			memcpy(packetbuf_dataptr()+packetbuf_datalen(),&rssi,2);

			process_packet_gateway(packetbuf_dataptr(),packetbuf_datalen()+2);
		}
	 }
	leds_off(LEDS_GREEN);
#endif //ASSET_MOTE
}

static void
unicast_recv(struct unicast_conn *c, const linkaddr_t *from) {
#ifndef ASSET_MOTE
	leds_on(LEDS_BLUE);
	 printf("Unicast_msg_received_from:[%x.%x]:[Cost %d][RSSI: %d]\r\n",
			 from->u8[0], from->u8[1],
			(int16_t)packetbuf_attr(PACKETBUF_ATTR_HOPS), packetbuf_attr(PACKETBUF_ATTR_RSSI));
	if(BROADCAST == packetbuf_attr(PACKETBUF_ATTR_PACKET_TYPE))
	{
		printf("wrong msg recvd\r\n");
		return;
	}  else if(UNICAST == packetbuf_attr(PACKETBUF_ATTR_PACKET_TYPE))
	{
//		printf("routing control packet recvd\r\n");
		//Here if the add routing entry means reinitializing the timer
//		if(!i_am_gateway)
		{
			add_routing_entry(from,packetbuf_attr(PACKETBUF_ATTR_HOPS));
		}

		if(!is_gateway_found)
			is_gateway_found = true;

	} else if(ASSET_TRACKING == packetbuf_attr(PACKETBUF_ATTR_PACKET_TYPE))
	{/*
	*store prev rssi
	*store prev addr
	*/
		//here i will forward the data to the next hop.
		printf("asset data packet recvd\r\n");
		if(!i_am_gateway)
			forward_data(from,false);
		else
		{
			printf("printing asset stat for Qt \r\n");
			process_packet_gateway(packetbuf_dataptr(),packetbuf_datalen());
		}
	}
//	print_routing_table();
#endif //ASSET_MOTE
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
PROCESS(main_process, "Main Process");
PROCESS(broadcast_process,"Broadcast Process");
PROCESS(route_share_process, "Routing table sharing");
AUTOSTART_PROCESSES(&main_process,&broadcast_process,&route_share_process);

//------------------------Broadcast process for initial neighbor discovery--
PROCESS_THREAD(broadcast_process, ev, data) {


	PROCESS_EXITHANDLER( printf("****broadcast_process terminated!****\r\n"); )

    PROCESS_BEGIN();
	static int broadcast_counter = 0;
	static struct etimer broadcast_timer;

	etimer_set(&broadcast_timer, CLOCK_SECOND/5);
	printf("Broadcast process started\r\n");
	gBroadcast_ended=false;
	while(1)
	{
        PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&broadcast_timer));
#ifndef ASSET_MOTE
        if(broadcast_counter < 20)
        {
			packetbuf_copyfrom("Wake_up",7);
			packetbuf_set_attr(PACKETBUF_ATTR_PACKET_TYPE,BROADCAST);
			packetbuf_set_attr(PACKETBUF_ATTR_HOPS,INFINITE_HOP_COUNT);
			broadcast_send(&broadcast);
			broadcast_counter++;
        }
//    	if(broadcast_counter > 50)
        else
    	{
    		/* If broadcast period finished*/
    		printf("--------------Stopping the broadcast-------------------- \n\r");
    		etimer_stop(&broadcast_timer);
    		print_routing_table();
    		check_gateway();
    		gBroadcast_ended=true;
    		broadcast_counter=0;
    		PROCESS_EXIT(); // Exit the process.
    	}
    	etimer_reset(&broadcast_timer);
	}
#else
	leds_on(LEDS_RED);
	packetbuf_copyfrom("ASSET",5);
	packetbuf_set_attr(PACKETBUF_ATTR_PACKET_TYPE,ASSET_TRACKING);
	packetbuf_set_attr(PACKETBUF_ATTR_HOPS,INFINITE_HOP_COUNT);
	broadcast_send(&broadcast);
	etimer_set(&broadcast_timer, CLOCK_SECOND*ASSET_SHARE_INTERVAL);
	leds_off(LEDS_RED);
	}
#endif //ASSET_MOTE
	printf("broadcast_process_end_reached\r\n");
    PROCESS_END();
}

//------------------------ PROCESS' THREAD ------------------------
PROCESS_THREAD(main_process, ev, data) {

	PROCESS_EXITHANDLER(broadcast_close(&broadcast);)
	PROCESS_BEGIN();

	/* Configure the user button */
	button_sensor.configure(BUTTON_SENSOR_CONFIG_TYPE_INTERVAL, CLOCK_SECOND);


	static struct etimer et;

	// Configure your team's channel (11 - 26).
	NETSTACK_CONF_RADIO.set_value(RADIO_PARAM_CHANNEL,13);
	NETSTACK_CONF_RADIO.set_value(RADIO_PARAM_TXPOWER, 7);
	print_settings();
	check_for_invalid_addr();

	// Open broadcast connection.
	unicast_open(&unicastConn, 146, &unicast_call);
	channel_set_attributes(146, attributes_unicast);
	// Open broadcast connection.
	broadcast_open(&broadcast, 128, &broadcast_call);
	channel_set_attributes(128, attributes_temp);
	if(linkaddr_cmp(&gateway_node,&linkaddr_node_addr))
	{
		i_am_gateway=true;
	}
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
    	print_routing_table();
    	if(gListEmpty && gBroadcast_ended && !i_am_gateway)
    	{
    		process_start(&broadcast_process, NULL);
    	}
//		printf("Battery_Voltage [Actual]= %dmV [Emulated]=%dmV\r\n", vdd3_sensor.value(CC2538_SENSORS_VALUE_TYPE_CONVERTED),gBatteryVoltage_emulated);
//		gBatteryVoltage=vdd3_sensor.value(CC2538_SENSORS_VALUE_TYPE_CONVERTED);


    	/* If an event from a sensor occurred */
    	if(ev == sensors_event) {
        	/* Event from the User button */
			if(data == &button_sensor) {
				/* Button was pressed */
				if(button_sensor.value(BUTTON_SENSOR_VALUE_TYPE_LEVEL) == BUTTON_SENSOR_PRESSED_LEVEL) {
					printf("Button pressed!\r\n");
					if(gBatteryVoltage_emulated<2000)
					{
						gBatteryVoltage_emulated=gBatteryVoltage_emulated+2000;
					}
					else
					{
						gBatteryVoltage_emulated=gBatteryVoltage_emulated-2000;
					}
			    	leds_on(LEDS_BLUE);
				}
				else {
				}
			}
    	}
//    	if(gBroadcast_ended)
//    	{
//    		gBroadcast_ended=false;
//    		process_start(&route_share_process, NULL);
//    	}
    	check_gateway();
	}
	PROCESS_END();
}
//------------------------ PROCESS' THREAD ------------------------

// ROUTING TABLE  process:

PROCESS_THREAD(route_share_process, ev, data) {
	PROCESS_EXITHANDLER( printf("****route_share_process terminated!***\r\n");)
    PROCESS_BEGIN();

	static struct etimer route_share_etimer;
	etimer_set(&route_share_etimer, CLOCK_SECOND*ROUTE_SHARE_INTERVAL+(random_rand()%5)/5);

	while (1){
		PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&route_share_etimer));
		/* If timer expired, send routing data to all neighbors*/
		//&& !linkaddr_cmp(&gateway_node,&linkaddr_node_addr)
//		if(is_gateway_found && !i_am_gateway && gBroadcast_ended)
		if(is_gateway_found && gBroadcast_ended)
			share_routing_table();
		/* Reset the etimer to trigger again in ROUTE_SHARE_INTERVAL seconds*/
//		etimer_reset(&route_share_etimer);
		etimer_set(&route_share_etimer, CLOCK_SECOND*ROUTE_SHARE_INTERVAL+(random_rand()%5)/5);
	}
	PROCESS_END();
}

#ifndef ASSET_MOTE
static bool inform_neighbor(const linkaddr_t * address)
{
	int i;
//	int counter;
	if(DEBUG)printf("%s\r\n",__func__);
//	printf("Unicast_msg_to : %x%x\r\n", address->u8[0], address->u8[1]);
	for(i=0;i<1;i++)
	{
		packetbuf_copyfrom("unicast",8);
		packetbuf_set_attr(PACKETBUF_ATTR_HOPS,0);
		packetbuf_set_attr(PACKETBUF_ATTR_PACKET_TYPE,UNICAST);
		packetbuf_set_addr(PACKETBUF_ADDR_ESENDER,&gateway_node);
		unicast_send(&unicastConn,address);
//		for(counter = 10000;counter>0;counter--);
	}
	return true;
}
#endif //ASSET_MOTE
