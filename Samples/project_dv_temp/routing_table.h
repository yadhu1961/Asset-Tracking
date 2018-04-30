/*
 * routing_table.h
 *
 *  Created on: Jun 27, 2017
 *      Author: yadhu
 */

#ifndef ROUTING_TABLE_H_
#define ROUTING_TABLE_H_

#include "contiki.h"
#include "core/net/linkaddr.h"
#include "stdbool.h"
#include "lib/list.h"
#include "lib/memb.h"
#include <stdio.h>
#include <string.h>

#include "unicast_custom.h"

#define MAX_NEIGHBORS 10
#define NEIGHBOR_DISCV_TIMEOUT 60
#define ROUTE_SHARE_INTERVAL (5000)
struct routing_table_entries
{
	linkaddr_t neighbor;
	uint16_t cost_to_gateway;
};

extern const linkaddr_t gateway_node;

struct routing_tables
{
	struct routing_table_entries routing_table_entry[10];
	uint16_t table_size;
};

typedef struct
{
	struct neighbors *next;
	linkaddr_t nbr_addr;
	uint16_t hop_count;
	struct ctimer node_timer;
}neighbors;

//neighbors default_neighbor

//I am creating neighbors list here
LIST(nbr_list);
MEMB(neighbor_mem,neighbors, MAX_NEIGHBORS);

extern linkaddr_t nearest_neighbor;

extern struct routing_tables routing_table;

linkaddr_t* get_nearest_neighbor();

void init_routing_table();

bool get_neighbors(linkaddr_t);

bool add_routing_entry(const linkaddr_t *,uint8_t);

void print_routing_table();

static void remove_nbr(void *n);
void share_routing_table();





#endif /* ROUTING_TABLE_H_ */
