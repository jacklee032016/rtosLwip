/**
 * LwIP configuration for AN767.
 */

#ifndef __LWIPOPTS_H__
#define __LWIPOPTS_H__

//#include "compact.h"


#define LWIP_IPV4					1
#define LWIP_IPV6					0

#define LWIP_COMPAT_SOCKETS		1


#define	NO_SYS						0

#define	LWIP_SOCKET				0 //(NO_SYS==0)
#define	LWIP_NETCONN				0 //(NO_SYS==0)


#define	MUX_WITH_OS				(NO_SYS == 0)

/* options for socket */
#define SO_REUSE					1
#define IP_SOF_BROADCAST_RECV		1
#define IP_SOF_BROADCAST			1
#define SO_REUSE_RXTOALL			1
#define LWIP_SO_RCVTIMEO			(LWIP_SOCKET)


/**
 * LWIP_NETIF_STATUS_CALLBACK==1: Support a callback function whenever an interface
 * changes its up/down status (i.e., due to DHCP IP acquistion)
 */
#define LWIP_NETIF_STATUS_CALLBACK	1


/**
 * SYS_LIGHTWEIGHT_PROT==1: if you want inter-task protection for certain
 * critical regions during buffer allocation, deallocation and memory
 * allocation and deallocation.
 */
#define SYS_LIGHTWEIGHT_PROT			0



/* ---------- Memory options ---------- */
/* MEM_ALIGNMENT: should be set to the alignment of the CPU for which
   lwIP is compiled. 4 byte alignment -> define MEM_ALIGNMENT to 4, 2
   byte alignment -> define MEM_ALIGNMENT to 2. */
#define MEM_ALIGNMENT					4

/* MEM_SIZE: the size of the heap memory. If the application will send
a lot of data that needs to be copied, this should be set high. */
//#define MEM_SIZE                		4 * 1024
#define MEM_SIZE						16 * 1024


/**
 * MEMP_NUM_PBUF: the number of memp struct pbufs (used for PBUF_ROM and PBUF_REF).
 * If the application sends a lot of data out of ROM (or other static memory),
 * this should be set high.
 */
#define MEMP_NUM_PBUF					16		/* 10 */


/* MEMP_NUM_RAW_PCB: the number of UDP protocol control blocks. One
   per active RAW "connection". */
#define MEMP_NUM_RAW_PCB				3

/**
 * MEMP_NUM_UDP_PCB: the number of UDP protocol control blocks. One
 * per active UDP "connection".
 * (requires the LWIP_UDP option)
 */
#define MEMP_NUM_UDP_PCB				12*2 /* 6*/

/**
 * MEMP_NUM_TCP_PCB: the number of simulatenously active TCP connections.
 * (requires the LWIP_TCP option)
 */
#define MEMP_NUM_TCP_PCB				16*4	/* 5 */

/* MEMP_NUM_TCP_PCB_LISTEN: the number of listening TCP connections.  (requires the LWIP_TCP option) */
#define MEMP_NUM_TCP_PCB_LISTEN		8 //8	/* 1 */

/**
 * MEMP_NUM_TCP_SEG: the number of simultaneously queued TCP segments. (requires the LWIP_TCP option)
 */
   
#define MEMP_NUM_TCP_SEG				16 // 16		/* 8 */

/* MEMP_NUM_SYS_TIMEOUT: the number of simulateously active timeouts. */
#define MEMP_NUM_SYS_TIMEOUT			12

/* The following four are used only with the sequential API and can be
   set to 0 if the application only will use the raw API. */
/**
 * MEMP_NUM_NETBUF: the number of struct netbufs.
 * (only needed if you use the sequential API, like api_lib.c)
 */
#define MEMP_NUM_NETBUF         			2 /* 2, unix */


/**
 * MEMP_NUM_NETCONN: the number of struct netconns. (only needed if you use the sequential API, like api_lib.c)
 */
#define MEMP_NUM_NETCONN				10 /* 16 */

/* MEMP_NUM_TCPIP_MSG_*: the number of struct tcpip_msg, which is used
   for sequential API communication and incoming packets. Used in src/api/tcpip.c. */
//#define MEMP_NUM_TCPIP_MSG_API			16
//#define MEMP_NUM_TCPIP_MSG_INPKT			16

#define MEMP_OVERFLOW_CHECK      1

/* ---------- Pbuf options ---------- */
/* PBUF_POOL_SIZE: the number of buffers in the pbuf pool */
#define PBUF_POOL_SIZE				16*4// 200	/* 15 */


/**
 * PBUF_POOL_BUFSIZE: the size of each pbuf in the pbuf pool. The default is
 * designed to accomodate single full size TCP frame in one pbuf, including
 * TCP_MSS, IP header, and link header.
 *
 * NOTE: Added extra word to handle Micrel requirement.
 */
//#define PBUF_POOL_BUFSIZE               GMAC_FRAME_LENTGH_MAX
//#define PBUF_POOL_BUFSIZE               128//1536		/* remove gmac from this file */
#define PBUF_POOL_BUFSIZE               1536 // LWIP_MEM_ALIGN_SIZE(TCP_MSS+40+PBUF_LINK_HLEN+4)


/* PBUF_LINK_HLEN: the number of bytes that should be allocated for a
   link level header. */
#define PBUF_LINK_HLEN          16u 



/**
 * LWIP_NETIF_TX_SINGLE_PBUF: if this is set to 1, lwIP tries to put all data
 * to be sent into one single pbuf. This is for compatibility with DMA-enabled
 * MACs that do not support scatter-gather.
 */
#define LWIP_NETIF_TX_SINGLE_PBUF                     1


/**
 * MEMP_NUM_FRAG_PBUF: the number of IP fragments simultaneously sent
 * (fragments, not whole packets!).
 * This is only used with IP_FRAG_USES_STATIC_BUF==0 and
 * LWIP_NETIF_TX_SINGLE_PBUF==0 and only has to be > 1 with DMA-enabled MACs
 * where the packet is not yet sent when netif->output returns.
 */
#define MEMP_NUM_FRAG_PBUF			2//6




/* TCP options *
 * LWIP_TCP==1: Turn on TCP.
 */
#define LWIP_TCP						1
#define TCP_TTL							255


#define TCP_LISTEN_BACKLOG      			1

/* Controls if TCP should queue segments that arrive out of
   order. Define to 0 if your device is low on memory. */
#define TCP_QUEUE_OOSEQ				0
/**
 * TCP_MSS: The maximum segment size controls the maximum amount of
 * payload bytes per packet. For maximum throughput, set this as
 * high as possible for your network (i.e. 1460 bytes for standard
 * ethernet).
 * For the receive side, this MSS is advertised to the remote side
 * when opening a connection. For the transmit size, this MSS sets
 * an upper limit on the MSS advertised by the remote host.
 */
#define TCP_MSS							1460

/**
 * TCP_WND: The size of a TCP receive window.  This must be at least
 * (2 * TCP_MSS) for things to work well
 */
//#define TCP_WND                 8096
#define TCP_WND						(2 * TCP_MSS)

/**
 * TCP_SND_BUF: TCP sender buffer space (bytes).
 * To achieve good performance, this should be at least 2 * TCP_MSS.
 */
//#define TCP_SND_BUF             2048
#define TCP_SND_BUF					(2 * TCP_MSS)


/* TCP sender buffer space (pbufs). This must be at least = 2 *
   TCP_SND_BUF/TCP_MSS for things to work. */
#define TCP_SND_QUEUELEN			(4 * TCP_SND_BUF/TCP_MSS)

/* TCP writable space (bytes). This must be less than or equal
   to TCP_SND_BUF. It is the amount of space which must be
   available in the tcp snd_buf for select to return writable */
#define TCP_SNDLOWAT				(TCP_SND_BUF/2)


/* Maximum number of retransmissions of data segments. */
#define TCP_MAXRTX					12

/* Maximum number of retransmissions of SYN segments. */
#define TCP_SYNMAXRTX				4

/* ARP */
#define LWIP_ARP					1
#define ARP_TABLE_SIZE				10
#define ARP_QUEUEING				1


#define IP_FORWARD					0


/* IP reassembly and segmentation.These are orthogonal even
 * if they both deal with IP fragments */
#define IP_REASSEMBLY				1
#define IP_REASS_MAX_PBUFS			10
/**
 * MEMP_NUM_REASSDATA: the number of IP packets simultaneously queued for
 * reassembly (whole packets, not fragments!)
 */
#define MEMP_NUM_REASSDATA			2	/* 10 */

#define IP_FRAG							1
#define IPV6_FRAG_COPYHEADER			1
#define LWIP_IPV6_FRAG					1


#define LWIP_IGMP						(LWIP_IPV4)

/* ---------- ICMP options ---------- */
#define ICMP_TTL							255

/* ---------- DHCP options ---------- */
/* Define LWIP_DHCP to 1 if you want DHCP configuration of interfaces. */
#define LWIP_DHCP						1

/* disable DHCP get NTP Server. J.L. */
//#define LWIP_DHCP_GET_NTP_SRV			(LWIP_DHCP)

/* 1 if you want to do an ARP check on the offered address
   (recommended if using DHCP). */
#define DHCP_DOES_ARP_CHECK			(LWIP_DHCP)

/* ---------- AUTOIP options ------- */
#define LWIP_AUTOIP						(LWIP_DHCP)
#define LWIP_DHCP_AUTOIP_COOP			(LWIP_DHCP)
#define LWIP_DHCP_AUTOIP_COOP_TRIES	3


/* ---------- SNMP options ---------- */
#define LWIP_SNMP						0
#define MIB2_STATS						LWIP_SNMP
#define SNMP_USE_NETCONN				LWIP_NETCONN
#define SNMP_USE_RAW					(!LWIP_NETCONN)

/* ---------- DNS options ---------- */
#define LWIP_DNS						1

/* ---------- MDNS options ---------- */
#define LWIP_MDNS_RESPONDER			0
#define LWIP_NUM_NETIF_CLIENT_DATA	(LWIP_MDNS_RESPONDER)

/* ---------- UDP options ---------- */
#define LWIP_UDP						1
#define UDP_TTL							255

#define PPP_SUPPORT					0      /* Set > 0 for PPP */
#define MPPE_SUPPORT				PPP_SUPPORT
#define PPPOE_SUPPORT				PPP_SUPPORT
#define PPPOL2TP_SUPPORT			PPP_SUPPORT
#define PPPOS_SUPPORT				PPP_SUPPORT


/**
 * LWIP_RAW==1: Enable application layer to hook into the IP layer itself.
 * Used to implement custom transport protocol (!= than Raw API).
 */
#define LWIP_RAW					1
#define RAW_TTL						255


/*
   ---------- Thread options ----------
*/

/** The stack sizes allocated to the netif stack: (256 * 4) = 1048 bytes. */
#define netifINTERFACE_TASK_STACK_SIZE		256*4

/** The priority of the netif stack. */
#define netifINTERFACE_TASK_PRIORITY		(tskIDLE_PRIORITY + 4)

/** The stack sizes allocated to the TCPIP stack: (256 * 4) = 1048 bytes. */
#define	TCPIP_THREAD_STACKSIZE				256*8

/** The priority of the TCPIP stack. */
#define	TCPIP_THREAD_PRIO					(tskIDLE_PRIORITY + 3)

/* J.L.*/
/** The mailbox size for the tcpip thread messages */
#define	TCPIP_MBOX_SIZE					16		/* J.L. */
#define	DEFAULT_ACCEPTMBOX_SIZE			16/4
#define	DEFAULT_RAW_RECVMBOX_SIZE		16/4
#define	DEFAULT_TCP_RECVMBOX_SIZE         	16/4

#define	DEFAULT_UDP_RECVMBOX_SIZE		16/4


/*
   ---------- Statistics options ----------
*/

/**
 * LWIP_STATS==1: Enable statistics collection in lwip_stats.
 */
#define LWIP_STATS							0

/**
 * LWIP_STATS_DISPLAY==1: Compile in the statistics output functions.
 */
#define LWIP_STATS_DISPLAY					0

/**
 * LWIP_STATS_LARGE==1: Use 32 bits counter instead of 16.
 */
#define LWIP_STATS_LARGE					1

#if LWIP_STATS
#define	LINK_STATS							1
#define	IP_STATS							1
#define	IPFRAG_STATS						1
#define	ICMP_STATS							1
#define	IGMP_STATS							1
#define	UDP_STATS							1
#define	TCP_STATS							1
#define	MEM_STATS							1
#define	MEMP_STATS							1
#define	SYS_STATS							1
/* Left outside to avoid warning. */
#define	ETHARP_STATS                      			1
#endif

/*
   ---------- Debugging options ----------
*/

/* disable to debug : make it stop. J.L. May.1st, 2018 */
//#define LWIP_NOASSERT

//#define	LWIP_DEBUG							/* debug option from LwIP, defined in command line */
#define	MUXLAB_DEBUG_LWIP				1	/* mux debug options */


#define LWIP_DBG_MIN_LEVEL              LWIP_DBG_LEVEL_ALL
#define LWIP_DBG_TYPES_ON               LWIP_DBG_ON

#if MUXLAB_DEBUG_LWIP
/* enabled debug options  */
#define	ETHARP_DEBUG					LWIP_DBG_OFF

#define	NETIF_DEBUG					LWIP_DBG_OFF
#define	IP_DEBUG						LWIP_DBG_OFF
#define	DHCP_DEBUG					LWIP_DBG_OFF

#define	PBUF_DEBUG					LWIP_DBG_OFF
#define	API_LIB_DEBUG					LWIP_DBG_OFF
#define	API_MSG_DEBUG					LWIP_DBG_OFF

#define	RAW_DEBUG						LWIP_DBG_OFF
#define	MEM_DEBUG						LWIP_DBG_OFF
#define	MEMP_DEBUG					LWIP_DBG_ON
#define	SYS_DEBUG						LWIP_DBG_OFF

#define	UDP_DEBUG						LWIP_DBG_OFF

#define	TCPIP_DEBUG					LWIP_DBG_OFF

#define	IGMP_DEBUG						LWIP_DBG_OFF
#else
#define	ETHARP_DEBUG					LWIP_DBG_OFF

#define	NETIF_DEBUG					LWIP_DBG_OFF
#define	IP_DEBUG						LWIP_DBG_OFF
#define	DHCP_DEBUG					LWIP_DBG_OFF

#define	PBUF_DEBUG					LWIP_DBG_OFF
#define	API_LIB_DEBUG					LWIP_DBG_OFF
#define	API_MSG_DEBUG					LWIP_DBG_OFF

#define	RAW_DEBUG						LWIP_DBG_OFF
#define	MEM_DEBUG						LWIP_DBG_OFF
#define	MEMP_DEBUG					LWIP_DBG_OFF
#define	SYS_DEBUG						LWIP_DBG_OFF

#define	UDP_DEBUG						LWIP_DBG_OFF

#define	TCPIP_DEBUG					LWIP_DBG_OFF

#define	IGMP_DEBUG						LWIP_DBG_OFF
#endif


/* disabled debug options */



#define SOCKETS_DEBUG				LWIP_DBG_OFF
#define ICMP_DEBUG                      LWIP_DBG_OFF
#define INET_DEBUG                      LWIP_DBG_OFF
#define IP_REASS_DEBUG                  LWIP_DBG_OFF

#define TIMERS_DEBUG                    LWIP_DBG_OFF
#define TCP_DEBUG                       LWIP_DBG_OFF
#define TCP_INPUT_DEBUG                 LWIP_DBG_OFF
#define TCP_FR_DEBUG                    LWIP_DBG_OFF
#define TCP_RTO_DEBUG                   LWIP_DBG_OFF
#define TCP_CWND_DEBUG                  LWIP_DBG_OFF
#define TCP_WND_DEBUG                   LWIP_DBG_OFF
#define TCP_OUTPUT_DEBUG                LWIP_DBG_OFF
#define TCP_RST_DEBUG                   LWIP_DBG_OFF
#define TCP_QLEN_DEBUG                  LWIP_DBG_OFF

#define PPP_DEBUG                       LWIP_DBG_OFF
#define SLIP_DEBUG                      LWIP_DBG_OFF
#define AUTOIP_DEBUG                    LWIP_DBG_OFF
#define SNMP_MSG_DEBUG                  LWIP_DBG_OFF
#define SNMP_MIB_DEBUG                  LWIP_DBG_OFF
#define DNS_DEBUG                       LWIP_DBG_OFF

//#define LWIP_TCPIP_CORE_LOCKING         0

//#define	TCPIP_THREAD_STACKSIZE		10240
//#define	TCPIP_THREAD_PRIO				2


#endif

