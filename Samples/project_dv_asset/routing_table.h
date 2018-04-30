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
#include "net/rime/rime.h"
#include <stdio.h>
#include <string.h>
#include "project-conf.h"
#include "unicast_custom.h"
#include "broadcast_custom.h"

//#include "dev/adc-zoul.h"      // ADC
//#include "dev/zoul-sensors.h"  // Sensor functions
#define MAX_NEIGHBORS 10
#define NEIGHBOR_DISCV_TIMEOUT 60
#define TIME_TO_LIVE 20
#define INFINITE_HOP_COUNT 0x1F
#define ROUTE_SHARE_INTERVAL (1)
#define ASSET_SHARE_INTERVAL (2)
#define DEBUG 1

extern const linkaddr_t gateway_node;
extern bool i_am_gateway;
extern bool gBroadcast_ended;
extern bool gListEmpty;
extern int gBatteryVoltage;
extern int gBatteryVoltage_emulated;
typedef struct
{
	struct neighbors *next;
	linkaddr_t nbr_addr;
	uint16_t hop_count;
	struct ctimer node_timer;
}neighbors;


extern bool is_gateway_found;

typedef struct
{
	linkaddr_t addr;
	uint16_t cost;
} nodes;

extern nodes nearest_neighbor;

nodes* get_nearest_neighbor();

void init_routing_table();

bool get_neighbors(linkaddr_t);

bool add_routing_entry(const linkaddr_t *,uint8_t);

void print_routing_table();

void remove_nbr(void *n);

void share_routing_table();
void check_gateway();
#endif /* ROUTING_TABLE_H_ */
