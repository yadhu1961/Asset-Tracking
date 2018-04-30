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

struct broadcast_custom_conn;

#define BROADCAST_CUSTOM_ATTRIBUTES  {PACKETBUF_ATTR_HOPS, PACKETBUF_ATTR_BIT * 5 }, \
	 {PACKETBUF_ATTR_PACKET_TYPE,PACKETBUF_ATTR_BYTE }, \
	BROADCAST_ATTRIBUTES

/**
 * \brief     Callback structure for broadcast
 *
 */
struct broadcast_custom_callbacks {
  /** Called when a packet has been received by the broadcast module. */
  void (* recv)(struct broadcast_custom_conn *ptr, const linkaddr_t *sender);
  void (* sent)(struct broadcast_custom_conn *ptr, int status, int num_tx);
};

struct broadcast_custom_conn {
  struct abc_conn c;
  const struct broadcast_custom_callbacks *u;
};

#endif /* BROADCAST_CUSTOM_H_ */
