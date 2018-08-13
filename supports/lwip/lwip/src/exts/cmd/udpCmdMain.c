/*
 *
 */

#include "lwipExt.h"

#include "jsmn.h"
#include "extUdpCmd.h"


static char	 extCmdPaserHandler(EXT_JSON_PARSER  *parser, void *data, u16_t size, const ip_addr_t *addr, u16_t port)
{
	CMN_IP_COMMAND *ipCmd = (CMN_IP_COMMAND *)data;
	unsigned int crcDecoded, crcReceived;

	ipCmd->length = ntohs(ipCmd->length);
	if(EXT_DEBUG_IS_ENABLE(EXT_DEBUG_FLAG_CMD))
	{
		printf("receive %d (IP CMD->length:%d) bytes from at port %d of '%s' ", size, ipCmd->length, port,  inet_ntoa(*(ip_addr_t *)addr));
		printf("'%.*s' \n\r", ipCmd->length-IPCMD_HEADER_LENGTH, ipCmd->data );
		CONSOLE_DEBUG_MEM(data, size, 0, "Raw IP CMD");
	}

	if (CMD_TAG_REQUEST != ipCmd->tag)
	{
		snprintf(parser->msg, EXT_JSON_MESSAGE_SIZE, "Tag of command is wrong:0x%x", ipCmd->tag );
		EXT_ERRORF(("%s", parser->msg) );
		parser->status = JSON_STATUS_TAG_ERROR;
		goto Failed;
	}


	crcReceived = *(unsigned int *)(ipCmd->data+ipCmd->length-IPCMD_HEADER_LENGTH);	/* the first 4 bytes are header of command, so here is the position of CRC */
	crcDecoded = htonl(cmnMuxCRC32b(ipCmd->data, ipCmd->length-IPCMD_HEADER_LENGTH));
	if (crcReceived != crcDecoded)
	{//Wrong CRC
		//sendto(sockCtrl, msg_reply, n /*+ 8*/, 0, (struct sockaddr *) &addr, addrlen); //  if suceeded the updated data is send back
		snprintf(parser->msg, EXT_JSON_MESSAGE_SIZE, "CRC of command is wrong:received CRC: 0x%x, Decoded CRC:0x%x", crcReceived, crcDecoded );
		EXT_ERRORF(("%s", parser->msg) );
		parser->status = JSON_STATUS_CRC_ERROR;
		goto Failed;
	}
	
	EXT_DEBUGF(EXT_IPCMD_DEBUG, ("received CRC: 0x%x, Decoded CRC:0x%x", crcReceived, crcDecoded));

	memset(parser->msg, 0, sizeof(parser->msg));
	if(extJsonRequestParseCommand((char *)ipCmd->data, size-IPCMD_HEADER_LENGTH*2, parser) == EXIT_FAILURE)
	{
#ifdef	X86
//		extJsonDebug(parser, runCfg, "Parsed JSON");
#endif
		EXT_DEBUGF(EXT_IPCMD_DEBUG, ("parse JSON command failed (%d): %s,because '%s'"LWIP_NEW_LINE, ipCmd->length, ipCmd->data, parser->msg ) );
		goto Failed;
	}
	else
	{
#ifdef	X86
//		extJsonDebug(parser, runCfg, "Parsed JSON");
#endif
		if( extJsonHandle(parser) == EXIT_FAILURE)
		{
			EXT_DEBUGF(EXT_IPCMD_DEBUG, ("JSON command: '%s' handler failed:%s"LWIP_NEW_LINE, parser->cmd, parser->msg ) );
			goto Failed;
		}

#if 0
		if(EXT_DEBUG_IS_ENABLE(EXT_DEBUG_FLAG_CMD))
		{
			printf("output RES2 %p, %d bytes: '%.*s'"LWIP_NEW_LINE, (void *)parser, parser->outIndex, parser->outIndex -IPCMD_HEADER_LENGTH, parser->outBuffer+IPCMD_HEADER_LENGTH);
		}
#endif

	}

	if(EXT_DEBUG_IS_ENABLE(EXT_DEBUG_FLAG_CMD))
	{
		printf("RES status %d, size %d , response: '%.*s'; message '%s'"LWIP_NEW_LINE, 
			parser->status, parser->outIndex,  parser->outIndex -IPCMD_HEADER_LENGTH*2, parser->outBuffer+IPCMD_HEADER_LENGTH, parser->msg );
		EXT_DEBUGF(EXT_IPCMD_DEBUG, ("RES status %d, size %d , response: '%.*s'; message '%s'"LWIP_NEW_LINE, 
			parser->status, parser->outIndex,  parser->outIndex -IPCMD_HEADER_LENGTH*2, parser->outBuffer+IPCMD_HEADER_LENGTH, parser->msg) );
	}
	return EXIT_SUCCESS;

Failed:
	extIpCmdResponseReply(parser);
	return EXIT_FAILURE;
	
}

#if LWIP_NETCONN

#include "lwip/api.h"
#include "lwip/sys.h"

/* UDP_CONN API or RAW API */
//#define	EXT_BC_UDPCONN_API		0

//#if EXT_BC_UDPCONN_API
static void _udpSysCtrlThread(void *arg)
{
//#define	_USE_LOCAL_BOARDCAST

	struct netconn *conn;
	struct netbuf *buf;
	char buffer[1024];
	err_t err;
#ifdef	_USE_LOCAL_BOARDCAST
	ip_addr_t addrAny;
#endif
	EXT_JSON_PARSER  *parser = (EXT_JSON_PARSER *)arg;

//	EXT_RUNTIME_CFG *runCfg = (EXT_RUNTIME_CFG *)arg;

//	printf("Boardcast Thread running.....\n\r");
//	LWIP_DEBUGF(LWIP_DBG_ON, ("Boardcast Thread running2.....\n\r"));

#ifdef	_USE_LOCAL_BOARDCAST
	printf("IP Mask '%s'\n\r", EXT_LWIP_IPADD_TO_STR(&parser->runCfg->ipAddress) );
	addrAny.addr =parser-> runCfg->ipAddress| (~(parser->runCfg->ipMask));
	printf("Listen on '%s'\n\r", EXT_LWIP_IPADD_TO_STR(&addrAny) );
#endif

#if LWIP_IPV6
	conn = netconn_new(NETCONN_UDP_IPV6);
	netconn_bind(conn, IP6_ADDR_ANY, EXT_CTRL_PORT);
#else /* LWIP_IPV6 */
	conn = netconn_new(NETCONN_UDP);
#ifdef	_USE_LOCAL_BOARDCAST
	netconn_bind(conn, &addrAny, EXT_CTRL_PORT);
#else
	netconn_bind(conn, IP_ADDR_ANY, EXT_CTRL_PORT);
#endif
#endif /* LWIP_IPV6 */
	LWIP_ERROR("boardcast: invalid conn", (conn != NULL), return;);

//	ip_set_option(conn->pcb.ip, SOF_BROADCAST);

	while (1)
	{
		err = netconn_recv(conn, &buf);
		if (err == ERR_OK)
		{
		
			/*  no need netconn_connect here, since the netbuf contains the address */
			if(netbuf_copy(buf, buffer, sizeof(buffer)) != buf->p->tot_len)
			{
				LWIP_DEBUGF(LWIP_DBG_ON, ("netbuf_copy failed\n"));
			}
			else
			{
				buffer[buf->p->tot_len] = '\0';
				
//				if(IP_BADCLASS(buf->addr.addr))

//				printf("RX %s (%s) packet ", (IP_BADCLASS(buf->toaddr.addr))?"BOARDCAST":"UNICAST", EXT_LWIP_IPADD_TO_STR(&(buf->toaddr.addr)));
//				printf("from %s : '%s'\n", EXT_LWIP_IPADD_TO_STR(&(buf->addr.addr)), buffer);
#if 0
				if(EXT_DEBUG_IS_ENABLE(EXT_DEBUG_FLAG_CMD))
				{
					printf("RX %s (%s) packet ", (IP_BADCLASS(buf->toaddr.addr))?"BOARDCAST":"UNICAST", inet_ntoa(*(ip_addr_t *)&(buf->toaddr)));
					printf("from %s : '%s'\n", inet_ntoa(*(ip_addr_t *)&(buf->addr)), buffer);
				}
#endif

#if 1				
				extCmdPaserHandler(buffer, buf->p->tot_len, &buf->toaddr, buf->port);
				netbuf_ref(buf, parser->outBuffer, strlen(parser->outBuffer) );
#else				
				char *msg="reply from 767";
				netbuf_ref(buf, msg, strlen(msg) );
#endif
//				err = netconn_sendto(conn, buf, &addrAny, EXT_CTRL_PORT);
				err = netconn_send(conn, buf);
				if(err != ERR_OK)
				{
					LWIP_DEBUGF(LWIP_DBG_ON, ("netconn_send failed: %d"LWIP_NEW_LINE, (int)err));
				}
#if 0				
				else
				{
					LWIP_DEBUGF(LWIP_DBG_ON, ("JSON reply OK: %d bytes"LWIP_NEW_LINE, strlen(parser->outBuffer)));
				}
#endif
				
			}
			
			netbuf_delete(buf);
		}
	}
}
#else


static struct udp_pcb *_ipcmdPcb;

char extIpCmdSendout(EXT_JSON_PARSER  *parser, unsigned int *ip, unsigned short port)
{
	struct pbuf *newBuf = NULL;
	const ip_addr_t *addr =  (ip_addr_t *)ip;
//	const ip4_addr_t *mcIpAddr = (ip4_addr_t *)&runCfg->dest.ip;

	newBuf = pbuf_alloc(PBUF_TRANSPORT, parser->outIndex, PBUF_ROM);
	if (newBuf == NULL)
	{
		EXT_ERRORF(("ERROR : can't allocate new pbuf"));
		return EXIT_FAILURE;
	}

	newBuf->payload = parser->outBuffer;
	newBuf->len = parser->outIndex;
	newBuf->tot_len = parser->outIndex;
	
	EXT_DEBUGF(EXT_IPCMD_DEBUG, ("send out %p:%p, size %d to  port %d of '%s': "LWIP_NEW_LINE"'%.*s'"LWIP_NEW_LINE, 
		newBuf, newBuf->payload, newBuf->len, port,  inet_ntoa(*(ip_addr_t *)addr), parser->outIndex-2*IPCMD_HEADER_LENGTH,  parser->outBuffer+IPCMD_HEADER_LENGTH ) );
	CONSOLE_DEBUG_MEM((unsigned char *)parser->outBuffer, parser->outIndex, 0, "Send out Raw IP CMD");
	udp_sendto(_ipcmdPcb, newBuf, addr, port); //dest port

	pbuf_free(newBuf);

	return EXIT_SUCCESS;
}



static void _rawUdpEchoRecv(void *arg, struct udp_pcb *pcb, struct pbuf *p, const ip_addr_t *addr, u16_t port)
{
//	struct pbuf *newBuf = NULL;
	EXT_JSON_PARSER  *parser = (EXT_JSON_PARSER *)arg;

	unsigned short size;

	EXT_ASSERT(("runCfg is null"), (parser !=NULL && parser->runCfg != NULL) );
	if (p != NULL)
	{
		if(addr == NULL)
		{
			EXT_ERRORF(("ERROR : add is null"LWIP_NEW_LINE));
			return;
		}
		EXT_DEBUGF(EXT_IPCMD_DEBUG, ("receive %d bytes from '%s' port %d"LWIP_NEW_LINE, p->tot_len, inet_ntoa(*(ip_addr_t *)addr), port) );
//		CONSOLE_DEBUG_MEM( p->payload, p->len, 0, "RECV IP Cmd");

		size = (p->tot_len > parser->runCfg->bufLength)? parser->runCfg->bufLength: p->tot_len;
		pbuf_copy_partial(p, parser->runCfg->bufRead, size, 0);
		pbuf_free(p);

		extCmdPaserHandler(parser, parser->runCfg->bufRead, size, addr, port);

		extIpCmdSendout(parser, (unsigned int *)addr, port );
	}
}

static void _udpSysCtrlThread(void *arg)
{
	_ipcmdPcb = udp_new();

//	ip_set_option(((struct ip_pcb *)ptel_pcb), SOF_BROADCAST);
//	ptel_pcb->so_options |= SOF_BROADCAST;

	udp_bind(_ipcmdPcb, IP_ADDR_ANY, EXT_CTRL_PORT);

	udp_recv(_ipcmdPcb, _rawUdpEchoRecv, arg);

#if 0
	struct pbuf *p;

	char msg[]="testing";
	while (1)
	{
		//Allocate packet buffer
		
		p = pbuf_alloc(PBUF_TRANSPORT,sizeof(msg),PBUF_RAM);
		memcpy (p->payload, msg, sizeof(msg));

		
		udp_sendto(ptel_pcb, p, IP_ADDR_BROADCAST, 3600);
		pbuf_free(p); //De-allocate packet buffer

//		vTaskDelay( 200 ); //some delay!
	}       
#endif

}
#endif


void extIpCmdAgentInit(EXT_JSON_PARSER  *parser)
{
#if LWIP_NETCONN
	sys_thread_new(EXT_TASK_SYS_CTRL, _udpSysCtrlThread, parser, 130*15, 2);
#else
	_udpSysCtrlThread(parser);
#endif
}



