
#include "lwipExt.h"

#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>

#include "jsmn.h"
#include "extUdpCmd.h"
#include "extFsm.h"



/* check whether this message is a reply for last message sent from TX/RX */
static char _extJsonIsResponse(EXT_JSON_PARSER  *parser)
{
	char name[EXT_USER_SIZE];
	
	if(extJsonParseString(parser, EXT_IPCMD_LOGIN_ACK, name, EXT_USER_SIZE) == EXIT_FAILURE )
	{
		return EXIT_FAILURE;
	}
	
	if(strncmp(name, EXT_IPCMD_STATUS_OK, strlen(EXT_IPCMD_STATUS_OK)) && 
		strncmp(name, EXT_IPCMD_STATUS_FAIL, strlen(EXT_IPCMD_STATUS_FAIL)))
	{
		return EXIT_FAILURE;
	}

	parser->outIndex = 0;

#if EXT_POLL_TASK
	extMediaPostEvent(EXT_MEDIA_EVENT_ACK, NULL);
#endif
	EXT_DEBUGF(EXT_DBG_ON, ("this is a response of IP command"));
	
	return EXIT_SUCCESS;
}

char extIpCmdSetupParams(EXT_JSON_PARSER  *parser)
{
	char ret;
	EXT_RUNTIME_CFG *rxCfg = &tmpRuntime;

	if(_extJsonIsResponse(parser)== EXIT_SUCCESS )
		return EXIT_SUCCESS;

	ret = extIpCmdIsLocal(parser);
	if(ret == EXIT_FAILURE)
		return ret;

	extSysClearConfig(rxCfg);

	if(extJsonRequestParse(parser, rxCfg) == EXIT_FAILURE)
	{
		parser->status = JSON_STATUS_PARSE_PARAM_ERROR;
		return EXIT_FAILURE;
	}



#ifdef	X86
	extDebugCfg(rxCfg, LWIP_NEW_LINE"RECEIVED Cfg:");
	extDebugCfg(parser->runCfg,LWIP_NEW_LINE "Before update Cfg:");
#endif

	extSysCompareParams(parser->runCfg, rxCfg);

#if 0
	/*10.03, 2018 comment to reply this media parameter J.L. */
	/* reply cmd with updated params */
	extIpCmdSendMediaData(parser, EXT_FALSE);
#endif

#ifdef	X86
	extDebugCfg(parser->runCfg, LWIP_NEW_LINE"updated Cfg:");
#endif

	extJsonResponsePrintConfig(parser);

	extSysConfigCtrl(parser->runCfg, rxCfg);
	
	return EXIT_SUCCESS;
}



void extVideoConfigCopy(EXT_VIDEO_CONFIG *dest, EXT_VIDEO_CONFIG *src)
{
	const ip4_addr_t *mcIpAddr;
	
	memcpy(dest->mac.address, src->mac.address, EXT_MAC_ADDRESS_LENGTH);
	if(src->ip != IPADDR_NONE)
	{
		dest->ip = src->ip;
	}

	mcIpAddr = (ip4_addr_t *)&dest->ip;
	if( ip4_addr_ismulticast(mcIpAddr))
	{
		extNetMulticastIP4Mac(&dest->ip, &dest->mac);
	}

	if(src->vport != 0)
	{
		dest->vport = src->vport;
	}
	
	if(src->aport != 0)
	{
		dest->aport = src->aport;
	}
}


/* no matter what current state is, send command to TX or RX.  */
char extCmdConnect(EXT_RUNTIME_CFG  *runCfg, unsigned char isStart)
{
//	char newStatus;
//	char action[32];
//	int index = 0;

//	newStatus = (runCfg->runtime.isConnect==0);

	EXT_INFOF(("%s begin to %s", runCfg->name,  (isStart)?"connect":"stop") );

	if(EXT_IS_TX(runCfg) )
	{
#ifdef	ARM
		extFpgaEnable(isStart);
#endif
	}
	else
	{/* RX leave igmp group */
		char ret = EXIT_SUCCESS;
		if(IP_ADDR_IS_MULTICAST(runCfg->dest.ip))
		{
#ifdef	ARM
			ret = extLwipGroupMgr(runCfg, runCfg->dest.ip, isStart);
#endif
			EXT_DEBUGF(EXT_IPCMD_DEBUG, ("IGMP group %s %s: %s"LWIP_NEW_LINE, (isStart==0)?"leave":"join", EXT_LWIP_IPADD_TO_STR(&runCfg->dest.ip), (ret==EXIT_SUCCESS)?"OK":"Fail" ));
		}
	}

//	runCfg->runtime.isConnect = newStatus;

	return EXIT_SUCCESS;
}

char *extLwipIpAddress(EXT_RUNTIME_CFG *runCfg)
{
	struct netif *_netif = (struct netif *)runCfg->netif;
	return inet_ntoa(*(struct in_addr *)&(_netif->ip_addr));
}


