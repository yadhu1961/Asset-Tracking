/**
 * \file
 *	File defines the interfaces for handling the routing in the application.
 *
 * \authors
 *         Abhishek Sunkum(ga83jok@mytum.de)
 *         Pramod Tikare(ga83baw@mytum.de)
 *         Yadhunandana (ga83jeb@mytum.de)
 */

#ifndef ROUTING_TABLE_H_
#define ROUTING_TABLE_H_

#include "contiki.h"
#include "core/net/linkaddr.h"
#include "stdbool.h"
#include "lib/list.h"
#include "lib/memb.h"
#include "net/rime/rime.h"
#include <stdio.h>
#include <string.h>
#include "project-conf.h"
#include "unicast_custom.h"
#include "broadcast_custom.h"

#define MAX_NEIGHBORS 10
#define NEIGHBOR_DISCV_TIMEOUT 60
#define TIME_TO_LIVE 20
#define INFINITE_HOP_COUNT 0x1F
#define ROUTE_SHARE_INTERVAL (1)
#define ASSET_SHARE_INTERVAL (2)
#define QT_ROUTE_SHARING_INTERVAL (10)
#define DEBUG 1

extern const linkaddr_t gateway_node;
extern bool i_am_gateway;
extern bool gBroadcast_ended;
extern bool gListEmpty;
extern int gBatteryVoltage;
extern int gBatteryVoltage_emulated;

/**
 * struct element which gets added to routing linked list. <br>
 * which has following elements <br>
 * @var neighbors::next
 * pointer to the next element in the list. <br>
 * @var neighbors::nbr_addr
 * neighbor address <br>
 * @var neighbors::hop_count
 * number of hops to gateway via that neighbor.<br>
 * @var neighbors::node_timer <br>
 * Time to live timer for the neighbor.
 *
 */
typedef struct
{
	struct neighbors *next;
	linkaddr_t nbr_addr;
	uint16_t hop_count;
	struct ctimer node_timer;
}neighbors;

enum PACKET_TYPE {BROADCAST,UNICAST,ASSET_TRACKING,NETWORK_STAT};



/**
 * global variable to indicate whether the node is connected to gateway or not.
 */
extern bool is_gateway_found;

/**
 * struct for storing the node information
 * addr stores the address of the neighbor
 */
typedef struct
{
	linkaddr_t addr;
	uint16_t cost;
} nodes;

/**
 * Stores the address and hop count of the nearest neighbor.
 */
extern nodes nearest_neighbor;


/**
 * unused function
 * @return
 */
nodes* get_nearest_neighbor();

/**
 * This is dummy function as of now
 * If necessary some logic can be added here, just after allocating the routing table.
 *
 */
void init_routing_table();


/**
 * Unused function.
 * @param addr
 * @return
 */
bool get_neighbors(linkaddr_t addr);


/**
 *
 * @param address address of the neighbor
 * @param cost cost to the neighbor.
 * @return whether the entry is added to the table or not.
 *
 * This function is responsible for adding the neighbor entries to the routing table.
 * contains the logic to determine whether entry is duplicate or not.
 * initializes the time to live timer/reinitializes if it already exists.
 *
 */
bool add_routing_entry(const linkaddr_t * address,uint8_t cost);

/**
 *
 * This function is used only for debugging purpose.
 * prints the current routing table of the node.
 *
 */
void print_routing_table();

/**
 *
 * @param n pointer to the neighbor entry which needs to be removed.
 *
 * This function removes the neighbor entry from the after TTL.
 *
 */
void remove_nbr(void *n);

/**
 *
 * This is important function responsible for sharing the routing info
 * to the neighbors, This information is used also TTL purposes.
 * When a routing packet is received the TTL also reinitialized for the neighbor.
 *
 */
void share_routing_table();


/**
 *
 * @return returns true always.
 *
 * This function periodically shares the routing table of a node to gateway mote.
 * required only for showing the routing table in QT, Not required in the production code.
 *
 */
bool qt_route_sharing_process(void);


/**
 *
 * Checks whether the packet received is, from gateway or not.
 *
 */
void check_gateway();

#endif /* ROUTING_TABLE_H_ */
