/*
 * routing_table.c
 *
 *  Created on: Jun 27, 2017
 *      Author: yadhu
 */

#include"routing_table.h"

nodes nearest_neighbor;

bool is_gateway_found = false;

//const linkaddr_t gateway_node = {{0x1,0x0}};
const linkaddr_t gateway_node = {{0x90,0x3d}};
//I am creating neighbors list here
LIST(nbr_list);
MEMB(neighbor_mem,neighbors, MAX_NEIGHBORS);

bool is_duplicate_entry(const linkaddr_t *address,uint8_t cost);

bool estimate_short_path();
bool i_am_gateway=false;

void init_routing_table()
{
//	//here I will fill dummy addresses and hop counts in the routing entries
//	for(i = 0; i<=10;i++) {
//		routing_table.routing_table_entry[i].neighbor.u8[0] = 0xFF;
//		routing_table.routing_table_entry[i].neighbor.u8[1] = 0xFF;
//		routing_table.routing_table_entry[i].cost_to_gateway = 0xFF;
//	}
}

bool add_routing_entry(const linkaddr_t *address,uint8_t cost)
{
/* below logic is to check and update ttl of the previous hop*/
	const linkaddr_t *prev_hop_addr;
	prev_hop_addr=packetbuf_addr(PACKETBUF_ADDR_ESENDER);
	bool route_through_me=false;
//	printf("prev_hop %x.%x\r\n",prev_hop_addr->u8[0], prev_hop_addr->u8[1]);
	if(linkaddr_cmp(prev_hop_addr,&linkaddr_node_addr))
	{
		route_through_me=true;
		neighbors *e;
		for(e = (neighbors *)list_head(nbr_list);e != NULL; e = (neighbors *)e->next)
		{

			if(linkaddr_cmp(address, &e->nbr_addr))
			{
				ctimer_set(&e->node_timer,CLOCK_SECOND*TIME_TO_LIVE,remove_nbr,e);
//				printf("TTL re-init from next hop \r\n");
				e->hop_count = INFINITE_HOP_COUNT;
				return true;
			}
		}
//		return false;
	}

	bool status = false;
	status = is_duplicate_entry(address,cost);
	if(false == status)
	{
		neighbors *temp = memb_alloc(&neighbor_mem);
		if(temp != NULL)
		{
			linkaddr_copy(&temp->nbr_addr,address);
			temp->hop_count = cost+1;
			if(route_through_me)
			{
				temp->hop_count = INFINITE_HOP_COUNT;
			}
			list_add(nbr_list,temp);
			ctimer_set(&temp->node_timer,CLOCK_SECOND*TIME_TO_LIVE,remove_nbr,temp);
		}
		else
		{
			printf("mem alloc failed \r\n");
			return false;
		}
	}
//	if(cost != INFINITE_HOP_COUNT)
//		estimate_short_path();
	return true;
}

void print_routing_table()
{
	neighbors *e;
//	printf("%s\r\n",__func__);
    for(e = (neighbors *)list_head(nbr_list); e != NULL;e = (neighbors *)e->next) {

    	printf("Address %x.%x: in %d\r\n",e->nbr_addr.u8[0], e->nbr_addr.u8[1], e->hop_count);

//    	if(linkaddr_cmp(&e->nbr_addr,&gateway_node)) {
//    		is_gateway_found = true;
//    		e->hop_count = 1;
//    		linkaddr_copy(&nearest_neighbor.addr,&e->nbr_addr);
//    		nearest_neighbor.cost = e->hop_count;
//    		printf("-------------------gateway found------------------ \r\n");
//    	}
    }
    printf("\r\n");
}

void check_gateway()
{
	neighbors *e;
//	printf("%s\r\n",__func__);
    for(e = (neighbors *)list_head(nbr_list); e != NULL;e = (neighbors *)e->next) {

//    	printf("Address %x.%x: in %d\r\n",e->nbr_addr.u8[0], e->nbr_addr.u8[1], e->hop_count);

    	if(linkaddr_cmp(&e->nbr_addr,&gateway_node)) {
//    		is_gateway_found = true;
    		e->hop_count = 1;
    		linkaddr_copy(&nearest_neighbor.addr,&e->nbr_addr);
    		nearest_neighbor.cost = 1;
//    		printf("GW_Found %x.%x: in %d\r\n",e->nbr_addr.u8[0], e->nbr_addr.u8[1], e->hop_count);
    	}
    }
}

//void update_gate_way_entry()
//{
//
//}
//here this function  will return the nearest neighbor so that
//Asset tracking packets can be broadcasted
bool gListEmpty=true;
bool estimate_short_path()
{
	//This for iteration purpose only
//	if(DEBUG)printf("%s\r\n",__func__);
	static neighbors *tmp_nearest;
	static neighbors temp_nearest_node;

	if(list_length(nbr_list))
	{
		gListEmpty=false;
		tmp_nearest = (neighbors *)list_head(nbr_list);

		temp_nearest_node.hop_count=tmp_nearest->hop_count;
		linkaddr_copy(&temp_nearest_node.nbr_addr,&tmp_nearest->nbr_addr);

		static neighbors *e;
		for(e = (neighbors *)tmp_nearest->next;e != NULL; e = (neighbors *)e->next) {
			if(e->hop_count < temp_nearest_node.hop_count) {
				temp_nearest_node.hop_count = e->hop_count;
				linkaddr_copy(&temp_nearest_node.nbr_addr,&e->nbr_addr);
			}
		}
		linkaddr_copy(&nearest_neighbor.addr,&temp_nearest_node.nbr_addr);

		nearest_neighbor.cost = temp_nearest_node.hop_count;
		if(gBatteryVoltage<2000 || gBatteryVoltage_emulated<2000)
		{
			nearest_neighbor.cost=nearest_neighbor.cost+1;
		}
//		printf(" Nearest_Neigbor:%x.%x, Hop_count:%d\r\n",nearest_neighbor.addr.u8[0],nearest_neighbor.addr.u8[1],nearest_neighbor.cost);
		return true;
	}
	else
	{
		printf("list empty\r\n");
		gListEmpty=true;
		return false;
	}
}

bool is_duplicate_entry(const linkaddr_t *address,uint8_t cost)
{
	//This for iteration purpose only
	neighbors *e;
    for(e = (neighbors *)list_head(nbr_list);e != NULL; e = (neighbors *)e->next) {

        if(linkaddr_cmp(address, &e->nbr_addr)) {
        	e->hop_count = cost+1;
        	ctimer_set(&e->node_timer,CLOCK_SECOND*TIME_TO_LIVE,remove_nbr,e);
//        	printf("Duplicate found return\r\n");
        	return true;
        }
    }
	return false;
}

void remove_nbr(void *n)
{
	neighbors *e = n;
	printf("*neighbor_removed:%x.%x\r\n",e->nbr_addr.u8[0],e->nbr_addr.u8[1]);
	list_remove(nbr_list,e);
	memb_free(&neighbor_mem,e);
}


enum PACKET_TYPE {BROADCAST,UNICAST,ASSET_TRACKING};
void share_routing_table(void)
{
	static neighbors *e=NULL;
	int i;
//	printf("sharing routing table\r\n");
	estimate_short_path();//to update nearest neighbor variable
//	print_routing_table();
//    for(e = (neighbors *)list_head(nbr_list);e != NULL; e = (neighbors *)e->next)
	if(e == NULL)
    {
		e = (neighbors *)list_head(nbr_list);
    	//don't share route info to nearest neighbor but to all other neighbors
//        if(!linkaddr_cmp(&nearest_neighbor.addr, &e->nbr_addr))
    }
	else
	{
    	if(i_am_gateway)
    	{
    		nearest_neighbor.cost=0;
    		nearest_neighbor.addr.u8[0]=0x00;
    		nearest_neighbor.addr.u8[1]=0x00;
    	}
//    	printf("Unicast_msg_to [%x.%x],[cost_thrgh_me: %d]\r\n",e->nbr_addr.u8[0], e->nbr_addr.u8[1],nearest_neighbor.cost);
    	for(i=0;i<1;i++)
    	{
			packetbuf_copyfrom("unicast",8);
			packetbuf_set_attr(PACKETBUF_ATTR_HOPS,nearest_neighbor.cost);
			packetbuf_set_attr(PACKETBUF_ATTR_PACKET_TYPE,UNICAST);
			packetbuf_set_addr(PACKETBUF_ADDR_ESENDER,&nearest_neighbor.addr);
			unicast_send(&unicastConn,&e->nbr_addr);
    	}
		e = (neighbors *)e->next;
	}
}
