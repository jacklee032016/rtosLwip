/**
 * MDNS responder private definitions
 * This file is part of the lwIP TCP/IP stack.
 * Author: Erik Ekman <erik@kryo.se>
 *
 */
#ifndef LWIP_HDR_MDNS_PRIV_H
#define LWIP_HDR_MDNS_PRIV_H

#include "lwip/apps/mdns_opts.h"
#include "lwip/pbuf.h"

#if LWIP_MDNS_RESPONDER

/* Domain struct and methods - visible for unit tests */

#define	MDNS_DOMAIN_MAXLEN		256
#define	MDNS_READNAME_ERROR		0xFFFF

#define	MDNS_PORT					5353
#define	MDNS_TTL					255


struct mdns_domain
{
	/* Encoded domain name */
	u8_t		name[MDNS_DOMAIN_MAXLEN];
	
	/* Total length of domain name, including zero */
	u16_t	length;

	/* Set if compression of this domain is not allowed */
	u8_t skip_compression;
};


err_t mdns_domain_add_label(struct mdns_domain *domain, const char *label, u8_t len);
u16_t mdns_readname(struct pbuf *p, u16_t offset, struct mdns_domain *domain);
int mdns_domain_eq(struct mdns_domain *a, struct mdns_domain *b);
u16_t mdns_compress_domain(struct pbuf *pbuf, u16_t *offset, struct mdns_domain *domain);


/** Information about received packet */
struct mdns_packet
{
	/** Sender IP/port */
	ip_addr_t source_addr;
	u16_t source_port;
	/** If packet was received unicast */
	u16_t recv_unicast;
	/** Netif that received the packet */
	struct netif *netif;
	/** Packet data */
	struct pbuf *pbuf;
	/** Current parsing offset in packet */
	u16_t parse_offset;
	/** Identifier. Used in legacy queries */
	u16_t tx_id;
	/** Number of questions in packet,
	*  read from packet header */
	u16_t questions;
	/** Number of unparsed questions */
	u16_t questions_left;
	/** Number of answers in packet,
	*  (sum of normal, authorative and additional answers)
	*  read from packet header */
	u16_t answers;
	/** Number of unparsed answers */
	u16_t answers_left;
};

/** Domain, type and class.
 *  Shared between questions and answers */
struct mdns_rr_info
{
	  struct mdns_domain domain;
	  u16_t				type;
	  u16_t				klass;
};

struct mdns_question
{
	struct mdns_rr_info	info;
	/** unicast reply requested */
	u16_t				unicast;
};

struct mdns_answer
{
	struct mdns_rr_info info;
	/** cache flush command bit */
	u16_t cache_flush;
	/* Validity time in seconds */
	u32_t ttl;
	/** Length of variable answer */
	u16_t rd_length;
	/** Offset of start of variable answer in packet */
	u16_t rd_offset;
};

err_t mdns_read_answer(struct mdns_packet *pkt, struct mdns_answer *answer);

void mdns_domain_debug_print(struct mdns_domain *domain);



#endif /* LWIP_MDNS_RESPONDER */

#endif /* LWIP_HDR_MDNS_PRIV_H */
