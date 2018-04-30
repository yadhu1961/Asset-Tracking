/*
 * broadcast_custom.h
 *
 *  Created on: Jun 28, 2017
 *      Author: ga83jok
 */

#ifndef BROADCAST_CUSTOM_H_
#define BROADCAST_CUSTOM_H_

#include "net/rime/abc.h"
#include "net/linkaddr.h"


#define BROADCAST_CUSTOM_ATTRIBUTES  {PACKETBUF_ATTR_HOPS, PACKETBUF_ATTR_BIT * 6 }, \
	 {PACKETBUF_ATTR_PACKET_TYPE,PACKETBUF_ATTR_BYTE }, \
	 {PACKETBUF_ADDR_ESENDER,PACKETBUF_ADDRSIZE }, \
	BROADCAST_ATTRIBUTES
#endif /* BROADCAST_CUSTOM_H_ */
