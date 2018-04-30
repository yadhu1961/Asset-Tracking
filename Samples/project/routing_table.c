/*
 * routing_table.c
 *
 *  Created on: Jun 27, 2017
 *      Author: yadhu
 */

#include"routing_table.h"

linkaddr_t nearest_neighbor;

struct routing_tables routing_table;

static int i;

bool is_duplicate_entry(const linkaddr_t *address,uint8_t cost);

bool estimate_short_path();

void init_routing_table()
{
	//here I will fill dummy addresses and hop counts in the routing entries
	for(i = 0; i<=10;i++) {
		routing_table.routing_table_entry[i].neighbor.u8[0] = 0xFF;
		routing_table.routing_table_entry[i].neighbor.u8[1] = 0xFF;
		routing_table.routing_table_entry[i].cost_to_gateway = 0xFF;
	}
}

int get_routing_table_size()
{
	return routing_table.table_size;
}

bool add_routing_entry(const linkaddr_t *address,uint8_t cost)
{
	//Check the validity of the routing entry
	if(is_duplicate_entry(address,cost))
	{
		printf("Duplicate entry don't update routing table");
		return false;
	}
	memcpy(&(routing_table.routing_table_entry[routing_table.table_size].neighbor),address,2);
	routing_table.routing_table_entry[routing_table.table_size].cost_to_gateway = cost;
	routing_table.table_size++;
	//After every entry shortest path needs to be estimated
	estimate_short_path();
	return true;
}

linkaddr_t* get_nearest_neighbor()
{
	//memcpy(&neighbor,&nearest_neighbor,2);
	return &nearest_neighbor;
}

void print_routing_table()
{
	printf("size of routing table %d\r\n",sizeof(linkaddr_t));
	printf("printing routing table\r\n");
	for(i = 0; i<routing_table.table_size;i++) {
		printf("Neighbor %x %x  cost %d \r\n",routing_table.routing_table_entry[i].neighbor.u8[0], \
				routing_table.routing_table_entry[i].neighbor.u8[1], \
				routing_table.routing_table_entry[i].cost_to_gateway);
	}
}

//here this function  will return the nearest neighbor so that
//Asset tracking packets can be broadcasted
bool estimate_short_path()
{
	static int lowest_cost = 0xFF;
	lowest_cost = routing_table.routing_table_entry[0].cost_to_gateway;
	nearest_neighbor.u16 = routing_table.routing_table_entry[0].neighbor.u16;
	for(i = 1; i<routing_table.table_size;i++) {
		if(routing_table.routing_table_entry[i].cost_to_gateway < lowest_cost)
			nearest_neighbor.u16 = routing_table.routing_table_entry[i].neighbor.u16;
	}
	return true;
}

bool is_duplicate_entry(const linkaddr_t *address,uint8_t cost)
{
	for(i = 0; i<routing_table.table_size;i++) {
		if(address->u16 == routing_table.routing_table_entry[i].neighbor.u16)
			return true;
	}
	return false;
}
