/* 
* network commands in command line
*/

/* Standard includes. */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lwip/api.h"
#include "lwip/inet.h"
#include <lwip/netif.h>
#include <lwip/ip4_addr.h>

#include "eos.h"

#include "lwipExt.h"

#if 0
static char	_resolveOneParam( char *param, unsigned short *port)
{
	EXT_MAC_ADDRESS macAddress;
	unsigned int ip;
	int ret;

	if(extMacAddressParse(&macAddress, param) == EXIT_SUCCESS)
	{
		printf("parsed MAC: '%s'"EXT_NEW_LINE, param );
		memcpy(&extRun.videoMacDest, &macAddress, sizeof(EXT_MAC_ADDRESS));
		return EXIT_SUCCESS;
	}

	ip = ipaddr_addr(param);
	if(ip != IPADDR_NONE)
	{
		printf("parsed IP : '%s'"EXT_NEW_LINE, param );
		extRun.videoIpDest = ip;
		return EXIT_SUCCESS;
	}

	ret = sscanf(param, "%"PRIu16, port);
	if (1 == ret )
	{
		printf("parsed short int: '%d'"EXT_NEW_LINE, *port );
		return EXIT_SUCCESS;
	}
	printf("parsed error short int:%d; return :%d"EXT_NEW_LINE, *port , ret);
	
	return EXIT_FAILURE;
}
#endif


char	extCmdChangeName(const struct _EXT_CLI_CMD *cmd,  char *outBuffer, size_t bufferLen)
{
	unsigned int index = 0;

	if(argc==1)
	{
		index += snprintf(outBuffer+index, bufferLen-index, "\tName:'%s'; model:'%s'"EXT_NEW_LINE, extRun.name, extRun.model );
		return EXT_FALSE;
	}
	
	memset(extRun.name, 0, EXT_CMD_MAX_LENGTH);
	memcpy(extRun.name, argv[1], (strlen(argv[1])>EXT_CMD_MAX_LENGTH)? EXT_CMD_MAX_LENGTH: strlen(argv[1]));
	
	bspCfgSave(&extRun, EXT_CFG_MAIN);

	index += snprintf(outBuffer+index, bufferLen-index, "\tNew Name:'%s'"EXT_NEW_LINE, extRun.name );

	return EXT_FALSE;
}


