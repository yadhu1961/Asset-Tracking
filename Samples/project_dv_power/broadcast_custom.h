/*
 * broadcast_custom.h
 *
 * \authors
 *         Abhishek Sunkum(ga83jok@mytum.de)
 *         Pramod Tikare(ga83baw@mytum.de)
 *         Yadhunandana (ga83jeb@mytum.de)
 */
#ifndef BROADCAST_CUSTOM_H_
#define BROADCAST_CUSTOM_H_

#include "net/rime/abc.h"
#include "net/linkaddr.h"

/**
 * Defining the custom attributes to the packet
 * Such as HOPS,PACKET_TYPE,E_SENDER
 */
#define BROADCAST_CUSTOM_ATTRIBUTES  {PACKETBUF_ATTR_HOPS, PACKETBUF_ATTR_BIT * 6 }, \
	 {PACKETBUF_ATTR_PACKET_TYPE,PACKETBUF_ATTR_BYTE }, \
	 {PACKETBUF_ADDR_ESENDER,PACKETBUF_ADDRSIZE }, \
	BROADCAST_ATTRIBUTES
#endif /* BROADCAST_CUSTOM_H_ */
