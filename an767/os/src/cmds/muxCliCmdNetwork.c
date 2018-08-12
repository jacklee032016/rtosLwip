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

#include "muxOs.h"

#include "lwipExt.h"

#if 0
static char	_resolveOneParam( char *param, unsigned short *port)
{
	MUX_MAC_ADDRESS macAddress;
	unsigned int ip;
	int ret;

	if(muxMacAddressParse(&macAddress, param) == EXIT_SUCCESS)
	{
		printf("parsed MAC: '%s'"MUX_NEW_LINE, param );
		memcpy(&muxRun.videoMacDest, &macAddress, sizeof(MUX_MAC_ADDRESS));
		return EXIT_SUCCESS;
	}

	ip = ipaddr_addr(param);
	if(ip != IPADDR_NONE)
	{
		printf("parsed IP : '%s'"MUX_NEW_LINE, param );
		muxRun.videoIpDest = ip;
		return EXIT_SUCCESS;
	}

	ret = sscanf(param, "%"PRIu16, port);
	if (1 == ret )
	{
		printf("parsed short int: '%d'"MUX_NEW_LINE, *port );
		return EXIT_SUCCESS;
	}
	printf("parsed error short int:%d; return :%d"MUX_NEW_LINE, *port , ret);
	
	return EXIT_FAILURE;
}
#endif


char	muxCmdChangeName(const struct _MUX_CLI_CMD *cmd,  char *outBuffer, size_t bufferLen)
{
	unsigned int index = 0;

	if(argc==1)
	{
		index += snprintf(outBuffer+index, bufferLen-index, "\tName:'%s'; model:'%s'"MUX_NEW_LINE, muxRun.name, muxRun.model );
		return MUX_FALSE;
	}
	
	memset(muxRun.name, 0, MUX_CMD_MAX_LENGTH);
	memcpy(muxRun.name, argv[1], (strlen(argv[1])>MUX_CMD_MAX_LENGTH)? MUX_CMD_MAX_LENGTH: strlen(argv[1]));
	
	bspCfgSave(&muxRun, MUX_CFG_MAIN);

	index += snprintf(outBuffer+index, bufferLen-index, "\tNew Name:'%s'"MUX_NEW_LINE, muxRun.name );

	return MUX_FALSE;
}


