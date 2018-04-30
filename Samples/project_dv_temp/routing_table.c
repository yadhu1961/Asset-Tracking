/*
 * routing_table.c
 *
 *  Created on: Jun 27, 2017
 *      Author: yadhu
 */

#include"routing_table.h"

linkaddr_t nearest_neighbor;

struct routing_tables routing_table;
const linkaddr_t gateway_node = {{0x90,0x3d}};

static int i;

bool is_duplicate_entry(const linkaddr_t *address,uint8_t cost);

bool estimate_short_path();

void init_routing_table()
{
//	//here I will fill dummy addresses and hop counts in the routing entries
//	for(i = 0; i<=10;i++) {
//		routing_table.routing_table_entry[i].neighbor.u8[0] = 0xFF;
//		routing_table.routing_table_entry[i].neighbor.u8[1] = 0xFF;
//		routing_table.routing_table_entry[i].cost_to_gateway = 0xFF;
//	}
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
		//printf("Duplicate entry don't update routing table\r\n");
		return false;
	}
	neighbors *temp = memb_alloc(&neighbor_mem);
	if(temp != NULL) {
		linkaddr_copy(&temp->nbr_addr,address);
		temp->hop_count = cost;
		list_add(nbr_list,temp);
		ctimer_set(&temp->node_timer,CLOCK_SECOND*300,remove_nbr,temp);
	}
	else
		//printf("mem alloc failed \r\n");

//	estimate_short_path();
	return true;
}

linkaddr_t* get_nearest_neighbor()
{
	//memcpy(&neighbor,&nearest_neighbor,2);
	return &nearest_neighbor;
}

void print_routing_table()
{
	neighbors *e;
	printf("%s\r\n",__func__);
    for(e = list_head(nbr_list); e != NULL;e = e->next) {

    	printf("Address %x.%x: in %d\r\n",e->nbr_addr.u8[0], e->nbr_addr.u8[1], e->hop_count);

    	if(linkaddr_cmp(&e->nbr_addr,&gateway_node)) {
    		printf("gateway found \r\n");
    	}
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
	//This for iteration purpose only
	neighbors *e;
    for(e = list_head(nbr_list);e != NULL; e = e->next) {
        if(linkaddr_cmp(address, &e->nbr_addr)) {
        	//printf("Duplicate found return\r\n");
        	return true;
        }
    }
	return false;
}

static void remove_nbr(void *n)
{
	neighbors *e = n;
	list_remove(nbr_list,e);
	memb_free(&neighbor_mem,e);
}

enum PACKET_TYPE {BROADCAST,UNICAST,ASSET_TRACKING};
void share_routing_table(void)
{
	neighbors *e;
	linkaddr_t nearest_neighbor;
	uint8_t hops;
	hops=1;//need to get this from get_neighbor function
	nearest_neighbor.u8[0] = gateway_node[0];
	nearest_neighbor.u8[1] = gateway_node[1]; //need to take from get_nearest_neighbor()
    for(e = list_head(nbr_list);e != NULL; e = e->next)
    {
    	//don't share route info to nearest neighbor but to all other neighbors
        if(!linkaddr_cmp(&nearest_neighbor, &e->nbr_addr))
        {
        	printf("Uniast_to %x.%x: in %d\r\n",e->nbr_addr.u8[0], e->nbr_addr.u8[1], hops);
        	packetbuf_copyfrom("unicast",7);
			packetbuf_set_attr(PACKETBUF_ATTR_HOPS,hops);
			packetbuf_set_attr(PACKETBUF_ATTR_PACKET_TYPE,UNICAST);
			unicast_send(&unicastConn,&e->nbr_addr);
        }
    }
}
