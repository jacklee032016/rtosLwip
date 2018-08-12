/**
 * Ping sender module
 *
 * This is an example of a "ping" sender (with raw API and socket API).
 * It can be used as a start point to maintain opened a network connection, or
 * like a network "watchdog" for your device.
 *
 */

#include "compact.h"
#include "lwipExt.h"

//#include "muxOs.h"
#include "lwip/timeouts.h"
#include "lwip/inet_chksum.h"

//#include "lwip/ip_addr.h"

/**
 * PING_DEBUG: Enable debugging for PING.
 */
#ifndef PING_DEBUG
#define PING_DEBUG     LWIP_DBG_ON
#endif

/** ping receive timeout - in milliseconds */
#ifndef PING_RCV_TIMEO
#define PING_RCV_TIMEO 1000
#endif

/** ping delay - in milliseconds */
#ifndef PING_DELAY
#define PING_DELAY     1000
#endif

/** ping identifier - must fit on a u16_t */
#ifndef PING_ID
#define PING_ID        0xAFAF
#endif

/** ping additional data size to include in the packet */
#ifndef PING_DATA_SIZE
#define PING_DATA_SIZE 32
#endif

/** ping result action - no default action */
#ifndef PING_RESULT
#define PING_RESULT(ping_ok)
#endif

/* ping variables */
//static const ip_addr_t* ping_target;
static ip_addr_t	_ping_target;
static u16_t ping_seq_num;
static u32_t ping_time;

#if !PING_USE_SOCKETS
static struct raw_pcb *pingPcb = NULL;
#endif /* PING_USE_SOCKETS */

#define	MUX_PING_COUNT		3

static int	_muxPingCount;

/** Prepare a echo ICMP request */
static void ping_prepare_echo( struct icmp_echo_hdr *iecho, u16_t len)
{
	size_t i;
	size_t data_len = len - sizeof(struct icmp_echo_hdr);

	ICMPH_TYPE_SET(iecho, ICMP_ECHO);
	ICMPH_CODE_SET(iecho, 0);
	iecho->chksum = 0;
	iecho->id     = PING_ID;
	iecho->seqno  = lwip_htons(++ping_seq_num);

	/* fill the additional data buffer with some data */
	for(i = 0; i < data_len; i++)
	{
		((char*)iecho)[sizeof(struct icmp_echo_hdr) + i] = (char)i;
	}

	iecho->chksum = inet_chksum(iecho, len);
}

#if PING_USE_SOCKETS

/* Ping using the socket ip */
static err_t ping_send(int s, const ip_addr_t *addr)
{
	int err;
	struct icmp_echo_hdr *iecho;
	struct sockaddr_storage to;
	size_t ping_size = sizeof(struct icmp_echo_hdr) + PING_DATA_SIZE;
	MUX_ASSERT(("ping_size is too big"), ping_size <= 0xffff);

#if LWIP_IPV6
	if(IP_IS_V6(addr) && !ip6_addr_isipv6mappedipv4(ip_2_ip6(addr)))
	{/* todo: support ICMP6 echo */
		return ERR_VAL;
	}
#endif /* LWIP_IPV6 */

	iecho = (struct icmp_echo_hdr *)mem_malloc((mem_size_t)ping_size);
	if (!iecho)
	{
		return ERR_MEM;
	}

	ping_prepare_echo(iecho, (u16_t)ping_size);

#if LWIP_IPV4
	if(IP_IS_V4(addr))
	{
		struct sockaddr_in *to4 = (struct sockaddr_in*)&to;
		to4->sin_len    = sizeof(to4);
		to4->sin_family = AF_INET;
		inet_addr_from_ip4addr(&to4->sin_addr, ip_2_ip4(addr));
	}
#endif /* LWIP_IPV4 */

#if LWIP_IPV6
	if(IP_IS_V6(addr))
	{
		struct sockaddr_in6 *to6 = (struct sockaddr_in6*)&to;
		to6->sin6_len    = sizeof(to6);
		to6->sin6_family = AF_INET6;
		inet6_addr_from_ip6addr(&to6->sin6_addr, ip_2_ip6(addr));
	}
#endif /* LWIP_IPV6 */

	err = lwip_sendto(s, iecho, ping_size, 0, (struct sockaddr*)&to, sizeof(to));

	mem_free(iecho);

	return (err ? ERR_OK : ERR_VAL);
}


static void ping_recv(int s)
{
	char buf[64];
	int len;
	struct sockaddr_storage from;
	int fromlen = sizeof(from);

	while((len = lwip_recvfrom(s, buf, sizeof(buf), 0, (struct sockaddr*)&from, (socklen_t*)&fromlen)) > 0)
	{
		if (len >= (int)(sizeof(struct ip_hdr)+sizeof(struct icmp_echo_hdr)))
		{
			ip_addr_t fromaddr;
			ip_addr_t *addr = &fromaddr;
			memset(&fromaddr, 0, sizeof(fromaddr));

#if LWIP_IPV4
			if(from.ss_family == AF_INET)
			{
				struct sockaddr_in *from4 = (struct sockaddr_in*)&from;
				inet_addr_to_ip4addr(ip_2_ip4(&fromaddr), &from4->sin_addr);
				IP_SET_TYPE(&fromaddr, IPADDR_TYPE_V4);
			}
#endif /* LWIP_IPV4 */

#if LWIP_IPV6
			if(from.ss_family == AF_INET6)
			{
				struct sockaddr_in6 *from6 = (struct sockaddr_in6*)&from;
				inet6_addr_to_ip6addr(ip_2_ip6(&fromaddr), &from6->sin6_addr);
				IP_SET_TYPE(&fromaddr, IPADDR_TYPE_V6);
			}
#endif /* LWIP_IPV6 */

			LWIP_DEBUGF( PING_DEBUG, ("ping: recv "));
			ip_addr_debug_print(PING_DEBUG, addr);
			LWIP_DEBUGF( PING_DEBUG, (" %"U32_F" ms" MUX_NEW_LINE, (sys_now() - ping_time)));

			/* todo: support ICMP6 echo */
#if LWIP_IPV4
			if (IP_IS_V4_VAL(fromaddr))
			{
				struct ip_hdr *iphdr;
				struct icmp_echo_hdr *iecho;

				iphdr = (struct ip_hdr *)buf;
				iecho = (struct icmp_echo_hdr *)(buf + (IPH_HL(iphdr) * 4));
				if ((iecho->id == PING_ID) && (iecho->seqno == lwip_htons(ping_seq_num)))
				{/* do some ping result processing */
					PING_RESULT((ICMPH_TYPE(iecho) == ICMP_ER));
					return;
				}
				else
				{
					LWIP_DEBUGF( PING_DEBUG, ("ping: drop" MUX_NEW_LINE));
				}
			}
#endif /* LWIP_IPV4 */
		}
		fromlen = sizeof(from);

	}

	if (len == 0)
	{
		LWIP_DEBUGF( PING_DEBUG, ("ping: recv - %"U32_F" ms - timeout" MUX_NEW_LINE, (sys_now()-ping_time)));
	}

	/* do some ping result processing */
	PING_RESULT(0);
}

static void ping_thread(void *arg)
{
	int s;
	int ret;

#if LWIP_SO_SNDRCVTIMEO_NONSTANDARD
	int timeout = PING_RCV_TIMEO;
#else
	struct timeval timeout;
	timeout.tv_sec = PING_RCV_TIMEO/1000;
	timeout.tv_usec = (PING_RCV_TIMEO%1000)*1000;
#endif
	LWIP_UNUSED_ARG(arg);

#if LWIP_IPV6
	if(IP_IS_V4(ping_target) || ip6_addr_isipv6mappedipv4(ip_2_ip6(ping_target)))
	{
		s = lwip_socket(AF_INET6, SOCK_RAW, IP_PROTO_ICMP);
	}
	else
	{
		s = lwip_socket(AF_INET6, SOCK_RAW, IP6_NEXTH_ICMP6);
	}
#else
	s = lwip_socket(AF_INET,  SOCK_RAW, IP_PROTO_ICMP);
#endif
	if (s < 0)
	{
		return;
	}

	ret = lwip_setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
	MUX_ASSERT(("setting receive timeout failed"), ret == 0);
	LWIP_UNUSED_ARG(ret);

	while (1)
	{
		if (ping_send(s, ping_target) == ERR_OK)
		{
			LWIP_DEBUGF( PING_DEBUG, ("ping: send "));
			ip_addr_debug_print(PING_DEBUG, ping_target);
			LWIP_DEBUGF( PING_DEBUG, ("\n"));

			ping_time = sys_now();
			ping_recv(s);
		}
		else
		{
			LWIP_DEBUGF( PING_DEBUG, ("ping: send "));
			ip_addr_debug_print(PING_DEBUG, ping_target);
			LWIP_DEBUGF( PING_DEBUG, (" - error\n"));

		}
		sys_msleep(PING_DELAY);
	}
}

#else /* PING_USE_SOCKETS */

/* Ping using the raw ip */
static u8_t pingRawRecv(void *arg, struct raw_pcb *pcb, struct pbuf *p, const ip_addr_t *addr)
{
	struct icmp_echo_hdr *iecho;
	LWIP_UNUSED_ARG(arg);
	LWIP_UNUSED_ARG(pcb);
	LWIP_UNUSED_ARG(addr);
	MUX_ASSERT(("p != NULL"), p != NULL);

	if ((p->tot_len >= (PBUF_IP_HLEN + sizeof(struct icmp_echo_hdr))) && pbuf_header(p, -PBUF_IP_HLEN) == 0)
	{
		iecho = (struct icmp_echo_hdr *)p->payload;

		if ((iecho->id == PING_ID) && (iecho->seqno == lwip_htons(ping_seq_num)))
		{
#if 0		
			MUX_INFOF( ("PING recv "));
			ip_addr_debug_print(LWIP_DBG_ON, addr);
			MUX_INFOF( (" %"U32_F" ms"LWIP_NEW_LINE, (sys_now()-ping_time)));
#else
			printf("PING recv %s %"U32_F" ms"MUX_NEW_LINE, MUX_LWIP_IPADD_TO_STR(addr), (sys_now()-ping_time) );
#endif
			/* do some ping result processing */
			PING_RESULT(1);
			pbuf_free(p);
			return 1; /* eat the packet */

		}

		/* not eaten, restore original packet */
		pbuf_header(p, PBUF_IP_HLEN);
	}

	return 0; /* don't eat the packet */
}

static void pingRawSend(struct raw_pcb *raw, ip_addr_t *addr)
{
	struct pbuf *p;
	struct icmp_echo_hdr *iecho;
	size_t ping_size = sizeof(struct icmp_echo_hdr) + PING_DATA_SIZE;

#if 0
	MUX_INFOF(("PING sent to "));
	ip_addr_debug_print(LWIP_DBG_ON, addr);
	MUX_INFOF((""LWIP_NEW_LINE));
#else
	printf("PING sent to %s"MUX_NEW_LINE, MUX_LWIP_IPADD_TO_STR(addr) );
#endif
	MUX_ASSERT(("ping_size <= 0xffff"), ping_size <= 0xffff);

	p = pbuf_alloc(PBUF_IP, (u16_t)ping_size, PBUF_RAM);
	if (!p)
	{
		return;
	}
	
	if ((p->len == p->tot_len) && (p->next == NULL))
	{
		iecho = (struct icmp_echo_hdr *)p->payload;

		ping_prepare_echo(iecho, (u16_t)ping_size);

		raw_sendto(raw, p, addr);
		ping_time = sys_now();
	}
	
	pbuf_free(p);
}

static void pingRawTimeout(void *arg)
{
	struct raw_pcb *pcb = (struct raw_pcb*)arg;
	ip_addr_t _pingTarget;

	MUX_ASSERT(("ping_timeout: no pcb given!"), pcb != NULL);

	ip_addr_copy_from_ip4(_pingTarget, _ping_target);
	
	if(_muxPingCount++ < MUX_PING_COUNT)
	{
		pingRawSend(pcb, &_pingTarget);
		sys_timeout(PING_DELAY, pingRawTimeout, pcb);
	}
	else
	{
		raw_remove(pingPcb);
		pingPcb = NULL;
	}
}

static void pingRawInit(void)
{
	if(pingPcb == NULL)
	{
		pingPcb = raw_new(IP_PROTO_ICMP);
		MUX_ASSERT(("ping_pcb != NULL"), pingPcb != NULL);

		raw_recv(pingPcb, pingRawRecv, NULL);
		raw_bind(pingPcb, IP_ADDR_ANY);
		sys_timeout(PING_DELAY, pingRawTimeout, pingPcb);
	}	
}

void muxNetPingSendNow(unsigned int destIp)
{
	ip_addr_t _pingTarget;
	
	MUX_ASSERT(("pingPcb != NULL"), pingPcb != NULL);
	_ping_target.addr = destIp;
#if 1	
	ip_addr_copy_from_ip4(_pingTarget, _ping_target );
#else
	pingTarget.addr = destIp;
#endif
	_muxPingCount = 0;
	pingRawSend(pingPcb, &_pingTarget);
}

#endif /* PING_USE_SOCKETS */

void muxNetPingInit(void)
//void muxPingInit(const ip_addr_t* ping_addr)
{
//	ping_target = &_ping_target;
//	ping_target->u_addr.ip4.addr = uIP;
//	ping_target = ping_addr;
#if PING_USE_SOCKETS
//	sys_thread_new("ping_thread", ping_thread, NULL, DEFAULT_THREAD_STACKSIZE, DEFAULT_THREAD_PRIO);
	sys_thread_new("pingd", ping_thread, NULL, DEFAULT_THREAD_STACKSIZE, MUX_TASK_ETHERNET_PRIORITY);
#else /* PING_USE_SOCKETS */
	pingRawInit();
#endif /* PING_USE_SOCKETS */
  
}

