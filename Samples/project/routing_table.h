/*
 * routing_table.h
 *
 *  Created on: Jun 27, 2017
 *      Author: yadhu
 */

#ifndef ROUTING_TABLE_H_
#define ROUTING_TABLE_H_

#include "core/net/linkaddr.h"
#include "stdbool.h"
#include <stdio.h>
#include <string.h>

struct routing_table_entries
{
	linkaddr_t neighbor;
	uint16_t cost_to_gateway;
};

struct routing_tables
{
	struct routing_table_entries routing_table_entry[10];
	uint16_t table_size;
};

extern linkaddr_t nearest_neighbor;

extern struct routing_tables routing_table;

linkaddr_t* get_nearest_neighbor();

bool get_neighbors(linkaddr_t);

bool add_routing_entry(const linkaddr_t *,uint8_t);

void print_routing_table();






#endif /* ROUTING_TABLE_H_ */
