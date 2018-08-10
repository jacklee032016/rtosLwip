/*
* mDNS client for NMOS service discovery
* Jack Lee, 05.10, 2018
*/

#include "lwip/opt.h"

#include "lwip/def.h"
#include "lwip/udp.h"
#include "lwip/mem.h"
#include "lwip/memp.h"
#include "lwip/dns.h"
#include "lwip/prot/dns.h"

#include "lwip/apps/mdns.h"
#include "lwip/apps/mdns_priv.h"

#include <string.h>

#ifdef	MUX_LAB
/* for 'rand' from libc of toolchain. J.L. */
#include	<stdlib.h>
#endif

#include "lwipMux.h"

#if LWIP_MDNS_RESPONDER

typedef	struct
{
	u16_t		priority;
	u16_t		weight;
	u16_t		port;
}DNS_RR_SRV;

#define	_MDNS_CLIENT_DEBUG		0

#define	DNS_COMPRESS_FLAG		0xC0

#if _MDNS_CLIENT_DEBUG
static void mdnsClientDebugApiIf(NMOS_API_INTERFACE *apiIf)
{
	MUX_DEBUGF(MUX_MDNS_CLIENT_DEBUG, ("%s API:"LWIP_NEW_LINE"\tName: '%s'; hostname: '%s'; IP: %s, port:%d", (apiIf->type== NMOS_API_T_REGISTRATION)?"Registration":"Query",
		apiIf->name, apiIf->hostname, MUX_LWIP_IPADD_TO_STR(&apiIf->ip), apiIf->port));

	MUX_DEBUGF(MUX_MDNS_CLIENT_DEBUG, ("\tAPI: v1.0:%s;v1.1:%s;v1.2:%s;v20:%s; Priority:%d", 
		NMOS_VERSION_IS_10(apiIf)?"Y":"N", NMOS_VERSION_IS_11(apiIf)?"Y":"N", NMOS_VERSION_IS_12(apiIf)?"Y":"N", NMOS_VERSION_IS_20(apiIf)?"Y":"N",  apiIf->priority)) ;

}


static void mdnsClientDebug(mdns_client_t *client)
{
	mdnsClientDebugApiIf(&client->node.registrationApi);

	mdnsClientDebugApiIf(&client->node.queryApi);
	
}
#endif

/**
 * Send a DNS query packet.
 *
 * @param idx the DNS table entry index for which to send a request
 * @return ERR_OK if packet is sent; an err_t indicating the problem otherwise
 */
static err_t mdnsClientSendQuery( mdns_client_t *client, NMOS_API_TYPE apiType)//,  char *service, u8_t qType)
{
	u16_t				qType = DNS_RRTYPE_PTR;

	err_t err;
	struct dns_hdr hdr;
	struct dns_query qry;
	struct pbuf *p;
	u16_t query_idx, copy_len;
	const char *hostname_part;
	char		*hostOrServiceName;
	u8_t n;

	NMOS_API_INTERFACE *apiIf = NULL;
	
	if(apiType == NMOS_API_T_REGISTRATION)
	{
		apiIf = &client->node.registrationApi;
	}
	else if(apiType == NMOS_API_T_QUERY)
	{
		apiIf = &client->node.queryApi;
	}
	else
	{
		return ERR_VAL;
	}

	hostOrServiceName = client->domainName;
	if(strlen((char *)apiIf->hostname) == 0 )
	{
	}
	else
	{
		qType = DNS_RRTYPE_A;
	}

	/* if here, we have either a new query or a retry on a previous query to process */
	p = pbuf_alloc(PBUF_TRANSPORT, (u16_t)(SIZEOF_DNS_HDR + strlen(hostOrServiceName) + 2 + SIZEOF_DNS_QUERY),  PBUF_RAM);
	if (p == NULL)
	{
		return ERR_MEM;
	}
	
	/* fill dns header */
	memset(&hdr, 0, SIZEOF_DNS_HDR);
	hdr.id = lwip_htons(client->txId++);
	hdr.flags1 = DNS_FLAG1_RD;
	hdr.numquestions = PP_HTONS(1);
	pbuf_take(p, &hdr, SIZEOF_DNS_HDR);

	--hostOrServiceName;

	/* convert hostname into suitable query format. */
	query_idx = SIZEOF_DNS_HDR;
	do
	{
		++hostOrServiceName;
		hostname_part = hostOrServiceName;
		for (n = 0; *hostOrServiceName != '.' && *hostOrServiceName != 0; ++hostOrServiceName)
		{
			++n;
		}
		
		copy_len = (u16_t)(hostOrServiceName - hostname_part);
		pbuf_put_at(p, query_idx, n);	/* length of label */
		pbuf_take_at(p, hostname_part, copy_len, query_idx + 1); /*value of label */
		query_idx += n + 1;
	} while (*hostOrServiceName != 0);
	
	pbuf_put_at(p, query_idx, 0);
	query_idx++;

	qry.type = PP_HTONS(qType);
	qry.cls = PP_HTONS(DNS_RRCLASS_IN);
	
	pbuf_take_at(p, &qry, SIZEOF_DNS_QUERY, query_idx);

	/* send dns packet */
	MUX_DEBUGF(MUX_MDNS_CLIENT_DEBUG, (LWIP_NEW_LINE LWIP_NEW_LINE "MdnsC: sending MDNS request ID %d, type %d for service \"%s\""LWIP_NEW_LINE,  client->txId, qType, client->domainName) );

	err = udp_sendto(client->udpPcb, p, &dns_mquery_v4group, DNS_MQUERY_PORT);

	/* free pbuf */
	pbuf_free(p);

	return err;
}

/* return offset of this lalel */
static unsigned short _mdnsClientGetOneLabel(char *dest, unsigned char *label)
{
	u8_t *src = label;
//	u8_t *dest = client->domainName;
	u8_t i;
	unsigned short length = 0;

//	while (*src)
	{
		u8_t label_len = *src;
		src++;
		length ++;

		for (i = 0; i < label_len; i++)
		{
//			LWIP_DEBUGF(MDNS_DEBUG, ("%c", src[i]));
			*dest++ = *src++;
			length ++;
		}
//		src += label_len;
		//LWIP_DEBUGF(MDNS_DEBUG, ("."));
		*dest = 0;
	}

//	i = src - label;

	return length;
}


/* FQDN labels : end with null byte */
static void _mdnsClientGetFullDomain(char *dest, unsigned char *srcLabel, unsigned short size)
{
	u8_t *src = srcLabel;
	u8_t i;
	unsigned short length = 0;

//	MUX_DEBUGF(MUX_MDNS_CLIENT_DEBUG, ("src label %d", size));
//	CONSOLE_DEBUG_MEM(srcLabel, size, 0, "SRC Label");

//	while(length < size && *src != 0)
	while( *src != 0)
	{
		u8_t label_len = *src;
		src++;

		for (i = 0; i < label_len; i++)
		{
//			LWIP_DEBUGF(MDNS_DEBUG, ("%c", src[i]));
			*dest++ = *src++;
			length ++;
		}
//		src += label_len;
		//LWIP_DEBUGF(MDNS_DEBUG, ("."));
		*dest++= '.';
		length++;
	}
	
	*(dest-1) = 0;	/* remove the last '.' */

}

/* A record */
static char _mdnsClientDnsParseRRA(mdns_client_t *client, struct mdns_answer *answer)
{
//	unsigned short offset = 0;
	NMOS_API_INTERFACE *apiIf = NULL;

	if( IS_STRING_EQUAL(client->domainName, client->node.registrationApi.hostname))
	{
		apiIf = &client->node.registrationApi;
	}
	else if( IS_STRING_EQUAL((char *)client->domainName, client->node.queryApi.hostname))
	{
		apiIf = &client->node.queryApi;
	}
	else 
	{
		return EXIT_FAILURE;
	}
	
	if(!strncmp((const char *)apiIf->hostname, (const char *)client->domainName, strlen(apiIf->hostname)) )
	{
		if(answer->rd_length == 4)
		{
			apiIf->ip = LWIP_MAKEU32(client->rdata[3], client->rdata[2],client->rdata[1],client->rdata[0]);
#if _MDNS_CLIENT_DEBUG
			MUX_INFOF( ("MdnsC: '%s':'%s'"LWIP_NEW_LINE, apiIf->hostname, MUX_LWIP_IPADD_TO_STR(&apiIf->ip)));
			mdnsClientDebug(client);
#endif
		}
		else
		{
			MUX_DEBUGF(MUX_MDNS_CLIENT_DEBUG, ("'MdnsC: RR A Address format error: %d'"LWIP_NEW_LINE, answer->rd_length));
		}
	}
	else
	{
		MUX_DEBUGF(MUX_MDNS_CLIENT_DEBUG, ("'MdnsC: RR A '%s' !='%s'"LWIP_NEW_LINE, apiIf->hostname, client->domainName));
	}

	return EXIT_SUCCESS;
}

/* RDATA of TXT is in length+value, without ending with null bytes */
static char _mdnsClientDnsParseTXT(mdns_client_t *client, NMOS_API_INTERFACE *apiIf, struct mdns_answer *answer)
{
	unsigned short offset = 0;
	unsigned char ret = EXIT_FAILURE;

	if(apiIf==NULL)
	{/* not for me */
		return EXIT_FAILURE;
	}
			MUX_ASSERT(("ApiIf is not initialized"), (apiIf!=NULL) );

//			MUX_DEBUGF(MUX_MDNS_CLIENT_DEBUG, ("MdnsC:  TXT #%d Answer offset:%d"LWIP_NEW_LINE, index, ans.rd_offset));
//			CONSOLE_DEBUG_MEM(client->rdata, (uint32_t)ans.rd_length, 0, "DNS TXT");
//			mdns_domain_debug_print(client->rdata);
			MUX_DEBUGF(MUX_MDNS_CLIENT_DEBUG, ("MdnsC: TXT Answer offset:%d"LWIP_NEW_LINE, answer->rd_offset));
//			_mdnsClientParseRRTxt(client, apiIf, &ans);

//			memcpy(client->domainName, ans.info.domain.name, ans.info.domain.length);
//			LWIP_DEBUGF(MDNS_DEBUG, ("MdnsC:  parse domain '%s' for API '%s' in #%d Answer '"LWIP_NEW_LINE, client->domainName, client->service, index));
//			client->state = MDNS_CLIENT_S_REQUEST_API;
//			client->qType = ans.info.type;

	if( IS_STRING_EQUAL((char *)client->domainName, (char *)apiIf->name) )
	{
		MUX_ERRORF(("RR is not for this API: '%s'!='%s'", client->domainName, apiIf->name ));
		return EXIT_FAILURE;
	}

	offset += _mdnsClientGetOneLabel(client->domainName, client->rdata);
	
	MUX_DEBUGF(MUX_MDNS_CLIENT_DEBUG, ("offset %d(%d):'%s'", offset, answer->rd_offset, client->domainName));
	while( offset <= answer->rd_length)
	{
		MUX_DEBUGF(MUX_MDNS_CLIENT_DEBUG, ("offset %d:'%s'", offset, client->domainName));
		
		if(strstr((char *)client->domainName, NMOS_API_NAME_PROTOCOL) )
		{
			if(!strncmp((char *)client->domainName+10/*"api_proto="*/, NMOS_API_PROTOCOL_HTTP, 4) && strlen((char *)client->domainName) != 15 /**/)
			{
				ret = EXIT_SUCCESS;
				NMOS_PROTOCOL_SET_HTTP(apiIf);
			}
			else
			{
				MUX_ERRORF(("Not support protocol:'%s'", client->domainName));
			}
		}
		else if(strstr((char *)client->domainName, NMOS_API_NAME_VERSION) )
		{
			if(strstr((char *)client->domainName, NMOS_API_VERSION_10) )
			{
				NMOS_VERSION_SET10(apiIf);
				ret = EXIT_SUCCESS;
			}
			if(strstr((char *)client->domainName, NMOS_API_VERSION_11) )
			{
				NMOS_VERSION_SET11(apiIf);
				ret = EXIT_SUCCESS;
			}
			if(strstr((char *)client->domainName, NMOS_API_VERSION_12) )
			{
				NMOS_VERSION_SET12(apiIf);
				ret = EXIT_SUCCESS;
			}
#if 0			
			if(strstr(client->domainName, NMOS_API_VERSION_20) )
			{
				NMOS_VERSION_SET20(apiIf);
				ret = EXIT_SUCCESS;
			}
#endif			
		}
		else if(strstr((char *)client->domainName, NMOS_API_NAME_PRIORITY) )
		{
			apiIf->priority= strtol((char *)(client->domainName+4), NULL, 10);
			if(apiIf->priority < 0)
			{
				MUX_ERRORF(("Not support priority:'%s'", client->domainName));
				apiIf->priority = 0;
			}
			else
			{
				ret = EXIT_SUCCESS;
			}

		}
		else 
		{
			MUX_ERRORF(("Not support label in RR TXT:'%s'", client->domainName));
		}

		offset += _mdnsClientGetOneLabel(client->domainName, client->rdata+offset);

//		return ret;
	};

	return ret;
}

static char _mdnsClientDnsParseSRV(mdns_client_t *client, NMOS_API_INTERFACE *apiIf, struct mdns_answer *answer)
{
	DNS_RR_SRV *srv = (DNS_RR_SRV *)client->rdata;
	err_t res;
	unsigned char firstLableLength = 0;
	unsigned char *target = NULL;
	
	if(apiIf==NULL)
	{/* not for me */
		return EXIT_FAILURE;
	}
	MUX_ASSERT(("ApiIf is not initialized"), (apiIf != NULL) );

	if( IS_STRING_EQUAL((char *)client->domainName, (char *)apiIf->name) )
	{
		MUX_ERRORF(("RR is not for this API: '%s'!='%s'", client->domainName, apiIf->name ));
		return EXIT_FAILURE;
	}

//	MUX_DEBUGF(MUX_MDNS_CLIENT_DEBUG, ("MdnsC: SRV Answer offset:%d"LWIP_NEW_LINE, answer->rd_offset));

	MUX_DEBUGF(MUX_MDNS_CLIENT_DEBUG, (LWIP_NEW_LINE"MdnsC: SRV priority:%d; weight:%d; port:%d"LWIP_NEW_LINE, lwip_htons(srv->priority), lwip_htons(srv->weight), lwip_htons(srv->port)));

	client->state = MDNS_CLIENT_S_REQUEST_API;
	apiIf->port = lwip_htons(srv->port);

	/* RDATA for hostname of SRV RR is FQDN, so it is ended by one null byte */

	firstLableLength = *(client->rdata+sizeof(DNS_RR_SRV));
	
	if((firstLableLength & DNS_COMPRESS_FLAG) == DNS_COMPRESS_FLAG)
	{
		u16_t jumpaddr;
		u16_t copied;
		jumpaddr = (((firstLableLength & 0x3f) << 8) | *(client->rdata+sizeof(DNS_RR_SRV)+1) );
		MUX_DEBUGF(MUX_MDNS_CLIENT_DEBUG, ("MdnsC: compressed target at offset:%d"LWIP_NEW_LINE, jumpaddr) );

		copied = pbuf_copy_partial(client->pkt->pbuf, client->rdata, sizeof(client->rdata), jumpaddr);
		if (copied != sizeof(client->rdata))
		{
			MUX_INFOF(("MdnsC: copied:%d"LWIP_NEW_LINE, copied) );
		}

		target = client->rdata;
	}
	else
	{
		target = client->rdata + (unsigned char)sizeof(DNS_RR_SRV);
	}

	_mdnsClientGetFullDomain(client->domainName, target, answer->rd_length );
	MUX_DEBUGF(MUX_MDNS_CLIENT_DEBUG, (LWIP_NEW_LINE"MdnsC: SRV host:'%s'"LWIP_NEW_LINE, client->domainName ));

	//client->qType = ans.info.type;
	/* if it is not IP address, begin to parse hostname */
	{	
#if _MDNS_CLIENT_DEBUG
		mdnsClientDebugApiIf(apiIf);
#endif
		/* save FQDN (include 'local') to compare later */
		sprintf((char *)apiIf->hostname, "%s.local", client->domainName);
		
		/* begin new QUERY for FQDN */
		sprintf((char *)client->domainName, "%s", apiIf->hostname );
		res = mdnsClientSendQuery(client, apiIf->type);
	
		MUX_ASSERT(("MdnsC: Send MDNS domain QUERY"LWIP_NEW_LINE), (res == ERR_OK) );
		if(res == !ERR_OK)
			return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

static NMOS_API_INTERFACE *_mdnsFindApiIfFromAnswer(mdns_client_t *client,  struct mdns_answer *answer)
{
	NMOS_API_INTERFACE *apiIf = NULL;

//	if( strstr((char *)client->domainName, client->node.registrationApi.service))// "registration") )
	if( strstr((char *)client->domainName, NMOS_MDNS_TYPE_REGISTRATION))// "registration") )
	{
		apiIf = &client->node.registrationApi;
	}
//	else if(strstr((char *)client->domainName, client->node.queryApi.service))//"query"))
	else if(strstr((char *)client->domainName, NMOS_MDNS_TYPE_QUERY))//"query"))
	{
		apiIf = &client->node.queryApi;
	}
	else
	{
		return NULL;
	}
	
	return apiIf;
}

char mdnsClientParseAnswer(mdns_client_t *client, struct mdns_packet *pkt)
{
	char ret = EXIT_FAILURE;
	int index = 0;
	err_t res;
	NMOS_API_INTERFACE *apiIf = NULL;

#if 0
	if(client->state != MDNS_CLIENT_S_INIT)
	{
		return EXIT_SUCCESS;
	}
#endif

	client->pkt = pkt;

	memset(client->domainName, 0 , sizeof(client->domainName));
	while (pkt->answers_left)
	{
		struct mdns_answer ans;
		u16_t copied;

		index++;
//		MUX_DEBUGF(MUX_MDNS_CLIENT_DEBUG, ("MdnsC: read #%d Answer"LWIP_NEW_LINE, index));
		res = mdns_read_answer(pkt, &ans);
		if (res != ERR_OK)
		{
			LWIP_DEBUGF(MUX_MDNS_CLIENT_DEBUG, ("MdnsC: Failed to parse answer, skipping response packet"LWIP_NEW_LINE));
			return ret;
		}
		
#if _MDNS_CLIENT_DEBUG
		_mdnsClientGetFullDomain(client->domainName, ans.info.domain.name, ans.info.domain.length);
		MUX_DEBUGF(MUX_MDNS_CLIENT_DEBUG, ("MdnsC: ANSWER#%d: type:%d; class:%d; rdLength:%d; rdOffset:%d; NAME :'%s'", 
			index, ans.info.type, ans.info.klass, ans.rd_length, ans.rd_offset, client->domainName) );
#endif

		/* read RDATA */
		if(sizeof(client->rdata) < ans.rd_length)
		{
			MUX_ERRORF(("Length of RDATA for Type %d is larger than buffer size: %d>%d", ans.info.type, ans.rd_length, sizeof(client->domainName)));
			return ret;
		}
		
		copied = pbuf_copy_partial(pkt->pbuf, client->rdata, ans.rd_length, ans.rd_offset);
		if (copied != ans.rd_length)
		{
			MUX_DEBUGF(MUX_MDNS_CLIENT_DEBUG, ("MdnsC: copied:%d=%d"LWIP_NEW_LINE, copied, ans.rd_length));
			return ERR_VAL;
		}
		
#if _MDNS_CLIENT_DEBUG
		MUX_DEBUGF(MUX_MDNS_CLIENT_DEBUG, ("MdnsC:#%d RDATA", index) );
		CONSOLE_DEBUG_MEM(client->rdata, (uint32_t)ans.rd_length, 0, "MDNS RDATA");
		MUX_DEBUGF(MUX_MDNS_CLIENT_DEBUG, (LWIP_NEW_LINE));
#endif

		if(ans.info.klass != DNS_RRCLASS_IN)
			return ret;

		if(ans.info.type == DNS_RRTYPE_A)
		{
			ret = _mdnsClientDnsParseRRA(client, &ans);
		}
		else if(ans.info.type == DNS_RRTYPE_PTR)
		{
			apiIf = _mdnsFindApiIfFromAnswer(client, &ans);
			if(apiIf == NULL)
			{
				return EXIT_FAILURE;
			}

			_mdnsClientGetOneLabel(apiIf->name, client->rdata);//, ans.rd_offset -2 /* 0xc0 0c ??? */);
			MUX_INFOF( ("MdnsC: service name '%s' for service '%s'"LWIP_NEW_LINE, apiIf->name, apiIf->service));
			client->state = MDNS_CLIENT_S_PARSE_PTR;
		}
		else if(ans.info.type == DNS_RRTYPE_SRV )
		{
			ret = _mdnsClientDnsParseSRV(client, apiIf, &ans);
		}
		else if(ans.info.type == DNS_RRTYPE_TXT)// && client->state == MDNS_CLIENT_S_PARSE_PTR)
		{
			ret = _mdnsClientDnsParseTXT(client, apiIf, &ans);
		}

	
	}


	return ret;
}

static char	_mdnsClientInitQueryForInterface(mdns_client_t *client, NMOS_API_INTERFACE *apiIf)
{
	err_t res;
	NMOS_API_TYPE type = apiIf->type;
	
	memset(apiIf, 0, sizeof(NMOS_API_INTERFACE) );
	if(type== NMOS_API_T_REGISTRATION)
	{
		apiIf->service = (char *)NMOS_MDNS_TYPE_REGISTRATION;
	}
	else if(type == NMOS_API_T_QUERY)
	{
		apiIf->service = (char *)NMOS_MDNS_TYPE_QUERY;
	}
	else
	{
		return EXIT_FAILURE;
	}

	apiIf->ip = IPADDR_NONE;
	apiIf->type = type;

	sprintf((char *)client->domainName, "%s", apiIf->service );
	
	res = mdnsClientSendQuery(client, apiIf->type);
	MUX_ASSERT(("Send MDNS QUERY"), (res == ERR_OK) );
	if(res == !ERR_OK)
		return EXIT_FAILURE;

	return EXIT_SUCCESS;
}

char mdnsClientInit(mdns_client_t *client, MUX_RUNTIME_CFG *runCfg)
{
	snprintf(client->service, sizeof(client->service), "%s", NMOS_MDNS_TYPE_REGISTRATION);
	client->qType = DNS_RRTYPE_PTR;
	client->txId = (u16_t )rand();
	client->state = MDNS_CLIENT_S_INIT;

	client->node.registrationApi.type = NMOS_API_T_REGISTRATION;
	client->node.queryApi.type = NMOS_API_T_QUERY;

	_mdnsClientInitQueryForInterface(client, &client->node.registrationApi);
	_mdnsClientInitQueryForInterface(client, &client->node.queryApi);
	
	return EXIT_SUCCESS;
}

#endif

