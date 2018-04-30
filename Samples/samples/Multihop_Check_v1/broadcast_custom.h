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

/**
 * \brief      Set up an identified best-effort broadcast connection
 * \param c    A pointer to a struct broadcast_conn
 * \param channel The channel on which the connection will operate
 * \param u    A struct broadcast_callbacks with function pointers to functions that will be called when a packet has been received
 *
 *             This function sets up a broadcast connection on the
 *             specified channel. The caller must have allocated the
 *             memory for the struct broadcast_conn, usually by declaring it
 *             as a static variable.
 *
 *             The struct broadcast_callbacks pointer must point to a structure
 *             containing a pointer to a function that will be called
 *             when a packet arrives on the channel.
 *
 */
void broadcast_custom_open(struct broadcast_custom_conn *c, uint16_t channel,
	       const struct broadcast_custom_callbacks *u);

/**
 * \brief      Close a broadcast connection
 * \param c    A pointer to a struct broadcast_conn
 *
 *             This function closes a broadcast connection that has
 *             previously been opened with broadcast_open().
 *
 *             This function typically is called as an exit handler.
 *
 */
void broadcast_custom_close(struct broadcast_custom_conn *c);

/**
 * \brief      Send an identified best-effort broadcast packet
 * \param c    The broadcast connection on which the packet should be sent
 * \retval     Non-zero if the packet could be sent, zero otherwise
 *
 *             This function sends an identified best-effort broadcast
 *             packet. The packet must be present in the packetbuf
 *             before this function is called.
 *
 *             The parameter c must point to a broadcast connection that
 *             must have previously been set up with broadcast_open().
 *
 */
int broadcast_custom_send(struct broadcast_custom_conn *c);

#endif /* BROADCAST_CUSTOM_H_ */
