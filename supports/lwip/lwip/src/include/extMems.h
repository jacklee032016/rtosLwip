/*
* Memory management for UDP Cmd and HTTPd
* Oct.23, 2018 
*/

#ifndef	__EXT_MEMS_H__
#define	__EXT_MEMS_H__

#if LWIP_EXT_UDP_CMD_TASK
typedef struct _UdpCmd
{
	struct pbuf		*p;
	unsigned	int 		ip;
	unsigned short	port;
}UdpCmd;
#endif

 
typedef struct _HttpEvent
{
	unsigned char			type;
	
	void					*mhc;

	struct tcp_pcb		*pcb;	/* sometimes, mhc is null, only pcb can be used */
	struct pbuf			*pBuf;
}HttpEvent;


#endif
