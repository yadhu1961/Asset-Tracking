

#ifndef UNICAST_CUSTOM_H_
#define UNICAST_CUSTOM_H_

#include "net/rime/abc.h"
#include "net/linkaddr.h"

#define UNICAST_CUSTOM_ATTRIBUTES  {PACKETBUF_ATTR_HOPS, PACKETBUF_ATTR_BIT * 5 }, \
	 {PACKETBUF_ATTR_PACKET_TYPE,PACKETBUF_ATTR_BYTE }, \
	BROADCAST_ATTRIBUTES

// Creates an instance of a unicast connection.
extern struct unicast_conn unicastConn;
#endif /* UNICAST_CUSTOM_H_ */
