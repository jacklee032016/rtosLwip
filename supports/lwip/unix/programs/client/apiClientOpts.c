
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>
#include <string.h>
#include <stdio.h>

#include <unistd.h>


#include "extSysParams.h"
#include "jsmn.h"
#include "extUdpCmd.h"


#include "../lwipTestClient.h"

extern char *optarg;
extern int optind, opterr, optopt;


struct API_CLIENT_OPT_HANDLER;

typedef	struct API_CLIENT_OPT_HANDLER
{
	int		type;
	char		*name;

	char		*prompt;

	int	(*handler)(struct API_CLIENT_OPT_HANDLER *, char *,  struct API_PARAMETERS *);
}API_CLIENT_OPT_HANDLER;

#if 0
/* for the command without any optional parameters*/
static int	_dumpParseSubOptions(struct API_CLIENT_OPT_HANDLER *handler, char *optarg, struct API_PARAMETERS *clientParams)
{
	return EXIT_SUCCESS;
}
#endif

static int	_findCfgOptions(struct API_CLIENT_OPT_HANDLER *handler, char *optarg, struct API_PARAMETERS *clientParams)
{
	char *subopts;
	char *value;
	int ret = EXIT_SUCCESS;

	char *const token[] =
	{
		"dev",
		NULL
	};

	subopts = optarg;
	
	while (*subopts != '\0' && ret != EXIT_FAILURE )
	{
		switch (getsubopt(&subopts, token, &value))
		{
			case 0: /* dev */
//			fprintf(stderr, "4 value of '%s'"API_CLIENT_NEW_LINE, value);
				if (value == NULL)
				{
					fprintf(stderr, "Missing value of '%s'"API_CLIENT_NEW_LINE, token[0]);
					ret = EXIT_FAILURE;
					goto _ret;
				}
				else
				{
					snprintf(clientParams->name, sizeof(clientParams->name), "%s", value);
//					fprintf(stderr, "name: /%d/"API_CLIENT_NEW_LINE, clientParams->name);
				}
				break;

			default:
//			fprintf(stderr, "default: value of '%s'"API_CLIENT_NEW_LINE, value);
				fprintf(stderr, "No match found for token: /%s/"API_CLIENT_NEW_LINE, value);
				ret = EXIT_FAILURE;
				goto _ret;
				break;
		}
	}

_ret:
//	fprintf(stderr, "%s : vport=%d,aport='%d/"LWIP_NEW_LINE, clientParams->cmd, clientParams->vCfg.vport ,clientParams->vCfg.aport );
	if(ret)
	{
		fprintf(stderr, "option for %s : /" CLIENT_OPTIONS_FIND"/'"API_CLIENT_NEW_LINE, clientParams->cmd);
	}

	return ret;
}


static int	_setupRs232CfgOptions(struct API_CLIENT_OPT_HANDLER *handler, char *optarg, struct API_PARAMETERS *clientParams)
{
	char *subopts;
	char *value;
	int ret = EXIT_SUCCESS;
	char res;
	unsigned int intValue = 0;
	char chrValue = 0;

	char *const token[] =
	{
		API_CMD_CLIENT_TARGET,
		"bps",
		"data",
		"stop",
		"parity",
		NULL
	};

	subopts = optarg;
	
	while (*subopts != '\0' && ret != EXIT_FAILURE )
	{
		switch (getsubopt(&subopts, token, &value))
		{
			case 0: /* target */
//			fprintf(stderr, "0 value of '%s'"LWIP_NEW_LINE, value);
				if (value == NULL)
				{
					fprintf(stderr, "Missing value of '%s'"API_CLIENT_NEW_LINE, token[0]);
					ret= EXIT_FAILURE;
					goto _ret;
				}
				else
				{
					res = muxMacAddressParse(&clientParams->target, value);
					if(res == EXIT_FAILURE)
					{
						fprintf(stderr, "'%s'='%s' is not MAC address"API_CLIENT_NEW_LINE, token[0], value );
						ret= EXIT_FAILURE;
						goto _ret;
					}
					snprintf(clientParams->mediaMac, sizeof(clientParams->mediaMac), "%s", value);
//					fprintf(stderr, "MAC: /%s/"LWIP_NEW_LINE, value);

				}
				break;

			case 1: /* bps */
//			fprintf(stderr, "2 value of '%s'"API_CLIENT_NEW_LINE, value);
				if (value == NULL)
				{
					fprintf(stderr, "Missing value of '%s'"API_CLIENT_NEW_LINE, token[1]);
					ret = EXIT_FAILURE;
					goto _ret;
				}
				else
				{
					ret = sscanf(value, "%d", &intValue);
					if(ret != 1)
					{
						fprintf(stderr, "'%s'='%s' is not integer"API_CLIENT_NEW_LINE, token[1], value );
						ret= EXIT_FAILURE;
						goto _ret;
					}
					ret = EXIT_SUCCESS;
					clientParams->rs232bps = intValue;
//					fprintf(stderr, "vport: /%d/"API_CLIENT_NEW_LINE, clientParams->vCfg.vport);
				}
				break;

			case 2: /* Data bit */
//			fprintf(stderr, "3 value of '%s'"API_CLIENT_NEW_LINE, value);
				if (value == NULL)
				{
					fprintf(stderr, "Missing value of '%s'"API_CLIENT_NEW_LINE, token[2]);
					ret = EXIT_FAILURE;
					goto _ret;
				}
				else
				{
					ret = sscanf(value, "%hhd", &chrValue);
					if(ret != 1)
					{
						fprintf(stderr, "'%s'='%s' is not integer"API_CLIENT_NEW_LINE, token[2], value );
						ret= EXIT_FAILURE;
						goto _ret;
					}
					ret = EXIT_SUCCESS;
					clientParams->rs232data = chrValue;
//					fprintf(stderr, "aport: /%d/"LWIP_NEW_LINE, clientParams->vCfg.aport);
				}
				break;

			case 3: /* Stop bit */
//			fprintf(stderr, "3 value of '%s'"API_CLIENT_NEW_LINE, value);
				if (value == NULL)
				{
					fprintf(stderr, "Missing value of '%s'"API_CLIENT_NEW_LINE, token[3]);
					ret = EXIT_FAILURE;
					goto _ret;
				}
				else
				{
					ret = sscanf(value, "%hhd", &chrValue);
					if(ret != 1)
					{
						fprintf(stderr, "'%s'='%s' is not integer"API_CLIENT_NEW_LINE, token[3], value );
						ret= EXIT_FAILURE;
						goto _ret;
					}
					ret = EXIT_SUCCESS;
					clientParams->rs232stop = chrValue;
//					fprintf(stderr, "aport: /%d/"LWIP_NEW_LINE, clientParams->vCfg.aport);
				}
				break;

			case 4: /* parity */
//			fprintf(stderr, "4 value of '%s'"API_CLIENT_NEW_LINE, value);
				if (value == NULL)
				{
					fprintf(stderr, "Missing value of '%s'"API_CLIENT_NEW_LINE, token[4]);
					ret = EXIT_FAILURE;
					goto _ret;
				}
				else
				{
					snprintf(clientParams->rs232Parity, sizeof(clientParams->rs232Parity), "%s", value);
//					fprintf(stderr, "name: /%d/"API_CLIENT_NEW_LINE, clientParams->name);
				}
				break;

			default:
//			fprintf(stderr, "default: value of '%s'"API_CLIENT_NEW_LINE, value);
				fprintf(stderr, "No match found for token: /%s/"API_CLIENT_NEW_LINE, value);
				ret = EXIT_FAILURE;
				goto _ret;
				break;
		}
	}

_ret:
//	fprintf(stderr, "%s : vport=%d,aport='%d/"LWIP_NEW_LINE, clientParams->cmd, clientParams->vCfg.vport ,clientParams->vCfg.aport );
	if(ret)
	{
		fprintf(stderr, "option for %s : /"CLIENT_OPTIONS_SETUP_RS232 "/"API_CLIENT_NEW_LINE, clientParams->cmd);
	}

	return ret;
}


static int	_setupProtocolCfgOptions(struct API_CLIENT_OPT_HANDLER *handler, char *optarg, struct API_PARAMETERS *clientParams)
{
	char *subopts;
	char *value;
	int ret = EXIT_SUCCESS;
	char res;
//	unsigned int ip = 0;
	char isEnable = 0;
	unsigned short portValue;

	char *const token[] =
	{
		API_CMD_CLIENT_TARGET,
		"mcast",
		"ip",
		"vport",
		"aport",
		"dport",
		"sport",
		NULL
	};

	
	subopts = optarg;
	
	while (*subopts != '\0' && ret != EXIT_FAILURE )
	{
		switch (getsubopt(&subopts, token, &value))
		{
			case 0: /* target */
//			fprintf(stderr, "0 value of '%s'"LWIP_NEW_LINE, value);
				if (value == NULL)
				{
					fprintf(stderr, "Missing value of '%s'"API_CLIENT_NEW_LINE, token[0]);
					ret= EXIT_FAILURE;
					goto _ret;
				}
				else
				{
					res = muxMacAddressParse(&clientParams->target, value);
					if(res == EXIT_FAILURE)
					{
						fprintf(stderr, "'%s'='%s' is not MAC address"API_CLIENT_NEW_LINE, token[0], value );
						ret= EXIT_FAILURE;
						goto _ret;
					}
					snprintf(clientParams->mediaMac, sizeof(clientParams->mediaMac), "%s", value);
//					fprintf(stderr, "MAC: /%s/"LWIP_NEW_LINE, value);

				}
				break;

			case 1: /* isMcast */
//			fprintf(stderr, "3 value of '%s'"API_CLIENT_NEW_LINE, value);
				if (value == NULL)
				{
					fprintf(stderr, "Missing value of '%s'"API_CLIENT_NEW_LINE, token[1]);
					ret = EXIT_FAILURE;
					goto _ret;
				}
				else
				{
					ret = sscanf(value, "%hhd", &isEnable);
					if(ret != 1)
					{
						fprintf(stderr, "'%s'='%s' is not integer"API_CLIENT_NEW_LINE, token[1], value );
						ret= EXIT_FAILURE;
						goto _ret;
					}
					ret = EXIT_SUCCESS;
					clientParams->isMcast = isEnable +1;
//					fprintf(stderr, "aport: /%d/"LWIP_NEW_LINE, clientParams->vCfg.aport);
				}
				break;

			case 2: /* IP */
//			fprintf(stderr, "1 value of '%s'"API_CLIENT_NEW_LINE, value);
				if (value == NULL)
				{
					fprintf(stderr, "Missing value of '%s'"API_CLIENT_NEW_LINE, token[2]);
					ret = EXIT_FAILURE;
					goto _ret;
				}
				else
				{
#if 1
					struct in_addr _ip;
					if (inet_aton(value, &_ip) == 0)
#else						
					ip = ipaddr_addr(value);
					if(ip == IPADDR_NONE)
#endif						
					{
						fprintf(stderr, "'%s'='%s' is not IP address"API_CLIENT_NEW_LINE, token[2], value );
						ret= EXIT_FAILURE;
						goto _ret;
					}
#if 0
					clientParams->vCfg.ip = ip;
#else
					clientParams->vCfg.ip = _ip.s_addr;
#endif
//					fprintf(stderr, "IP: /%s/\n", value);
				}
				break;

			case 3: /* vport */
//			fprintf(stderr, "2 value of '%s'"API_CLIENT_NEW_LINE, value);
				if (value == NULL)
				{
					fprintf(stderr, "Missing value of '%s'"API_CLIENT_NEW_LINE, token[3]);
					ret = EXIT_FAILURE;
					goto _ret;
				}
				else
				{
					ret = sscanf(value, "%hd", &portValue);
					if(ret != 1)
					{
						fprintf(stderr, "'%s'='%s' is not integer"API_CLIENT_NEW_LINE, token[3], value );
						ret= EXIT_FAILURE;
						goto _ret;
					}
					ret = EXIT_SUCCESS;
					clientParams->vCfg.vport = portValue;
//					fprintf(stderr, "vport: /%d/"API_CLIENT_NEW_LINE, clientParams->vCfg.vport);
				}
				break;

			case 4: /* audio port */
//			fprintf(stderr, "3 value of '%s'"API_CLIENT_NEW_LINE, value);
				if (value == NULL)
				{
					fprintf(stderr, "Missing value of '%s'"API_CLIENT_NEW_LINE, token[4]);
					ret = EXIT_FAILURE;
					goto _ret;
				}
				else
				{
					ret = sscanf(value, "%hd", &portValue );
					if(ret != 1)
					{
						fprintf(stderr, "'%s'='%s' is not integer"API_CLIENT_NEW_LINE, token[4], value );
						ret= EXIT_FAILURE;
						goto _ret;
					}
					ret = EXIT_SUCCESS;
					clientParams->vCfg.aport = portValue;
//					fprintf(stderr, "aport: /%d/"LWIP_NEW_LINE, clientParams->vCfg.aport);
				}
				break;

			case 5: /* dport */
//			fprintf(stderr, "2 value of '%s'"API_CLIENT_NEW_LINE, value);
				if (value == NULL)
				{
					fprintf(stderr, "Missing value of '%s'"API_CLIENT_NEW_LINE, token[5]);
					ret = EXIT_FAILURE;
					goto _ret;
				}
				else
				{
					ret = sscanf(value, "%hd", &portValue);
					if(ret != 1)
					{
						fprintf(stderr, "'%s'='%s' is not integer"API_CLIENT_NEW_LINE, token[5], value );
						ret= EXIT_FAILURE;
						goto _ret;
					}
					ret = EXIT_SUCCESS;
					clientParams->vCfg.dport = portValue;
//					fprintf(stderr, "vport: /%d/"API_CLIENT_NEW_LINE, clientParams->vCfg.vport);
				}
				break;

			case 6: /* st port */
//			fprintf(stderr, "3 value of '%s'"API_CLIENT_NEW_LINE, value);
				if (value == NULL)
				{
					fprintf(stderr, "Missing value of '%s'"API_CLIENT_NEW_LINE, token[6]);
					ret = EXIT_FAILURE;
					goto _ret;
				}
				else
				{
					ret = sscanf(value, "%hd", &portValue );
					if(ret != 1)
					{
						fprintf(stderr, "'%s'='%s' is not integer"API_CLIENT_NEW_LINE, token[6], value );
						ret= EXIT_FAILURE;
						goto _ret;
					}
					ret = EXIT_SUCCESS;
					clientParams->vCfg.sport = portValue;
//					fprintf(stderr, "aport: /%d/"LWIP_NEW_LINE, clientParams->vCfg.aport);
				}
				break;

			default:
//			fprintf(stderr, "default: value of '%s'"API_CLIENT_NEW_LINE, value);
				fprintf(stderr, "No match found for token: /%s/"API_CLIENT_NEW_LINE, value);
				ret = EXIT_FAILURE;
				goto _ret;
				break;
		}
	}

_ret:
//	fprintf(stderr, "%s : vport=%d,aport='%d/"LWIP_NEW_LINE, clientParams->cmd, clientParams->vCfg.vport ,clientParams->vCfg.aport );
	if(ret)
	{
		fprintf(stderr, "option for %s : /" CLIENT_OPTIONS_SETUP_PROTOCOL"/'"API_CLIENT_NEW_LINE, clientParams->cmd);
	}

	return ret;
}



static int	_setupMediaCfgOptions(struct API_CLIENT_OPT_HANDLER *handler, char *optarg, struct API_PARAMETERS *clientParams)
{
	char *subopts;
	char *value;
	int ret = EXIT_SUCCESS;
	char res;
//	unsigned int ip = 0;
	char chValue = 0;
	unsigned short shortValue;
//	unsigned 	int intValue;

	char *const token[] =
	{
		API_CMD_CLIENT_TARGET,
		"vW",
		"vH",
		"vFps",
		"vDepth",
		"vColorSpace",
		"vInterlaced",
		"vSegment",
		"aFre",
		"aDepth",
		"aCh",
		NULL
	};

	
	subopts = optarg;
	
	while (*subopts != '\0' && ret != EXIT_FAILURE )
	{
		switch (getsubopt(&subopts, token, &value))
		{
			case 0: /* target */
//			fprintf(stderr, "0 value of '%s'"LWIP_NEW_LINE, value);
				if (value == NULL)
				{
					fprintf(stderr, "Missing value of '%s'"API_CLIENT_NEW_LINE, token[0]);
					ret= EXIT_FAILURE;
					goto _ret;
				}
				else
				{
					res = muxMacAddressParse(&clientParams->target, value);
					if(res == EXIT_FAILURE)
					{
						fprintf(stderr, "'%s'='%s' is not MAC address"API_CLIENT_NEW_LINE, token[0], value );
						ret= EXIT_FAILURE;
						goto _ret;
					}
					snprintf(clientParams->mediaMac, sizeof(clientParams->mediaMac), "%s", value);
//					fprintf(stderr, "MAC: /%s/"LWIP_NEW_LINE, value);

				}
				break;

			case 1: /* vW */
//			fprintf(stderr, "3 value of '%s'"API_CLIENT_NEW_LINE, value);
				if (value == NULL)
				{
					fprintf(stderr, "Missing value of '%s'"API_CLIENT_NEW_LINE, token[1]);
					ret = EXIT_FAILURE;
					goto _ret;
				}
				else
				{
					ret = sscanf(value, "%hd", &shortValue);
					if(ret != 1)
					{
						fprintf(stderr, "'%s'='%s' is not integer"API_CLIENT_NEW_LINE, token[1], value );
						ret= EXIT_FAILURE;
						goto _ret;
					}
					ret = EXIT_SUCCESS;
					clientParams->mediaParam.vWidth = shortValue;
//					fprintf(stderr, "aport: /%d/"LWIP_NEW_LINE, clientParams->vCfg.aport);
				}
				break;

			case 2: /* vH */
//			fprintf(stderr, "3 value of '%s'"API_CLIENT_NEW_LINE, value);
				if (value == NULL)
				{
					fprintf(stderr, "Missing value of '%s'"API_CLIENT_NEW_LINE, token[2]);
					ret = EXIT_FAILURE;
					goto _ret;
				}
				else
				{
					ret = sscanf(value, "%hd", &shortValue);
					if(ret != 1)
					{
						fprintf(stderr, "'%s'='%s' is not integer"API_CLIENT_NEW_LINE, token[2], value );
						ret= EXIT_FAILURE;
						goto _ret;
					}
					ret = EXIT_SUCCESS;
					clientParams->mediaParam.vHeight = shortValue;
//					fprintf(stderr, "aport: /%d/"LWIP_NEW_LINE, clientParams->vCfg.aport);
				}
				break;

			case 3: /* vFps */
//			fprintf(stderr, "3 value of '%s'"API_CLIENT_NEW_LINE, value);
				if (value == NULL)
				{
					fprintf(stderr, "Missing value of '%s'"API_CLIENT_NEW_LINE, token[3]);
					ret = EXIT_FAILURE;
					goto _ret;
				}
				else
				{
					ret = sscanf(value, "%hhd", &chValue);
					if(ret != 1)
					{
						fprintf(stderr, "'%s'='%s' is not integer"API_CLIENT_NEW_LINE, token[3], value );
						ret= EXIT_FAILURE;
						goto _ret;
					}
					ret = EXIT_SUCCESS;
					clientParams->mediaParam.vFrameRate = chValue;
//					fprintf(stderr, "aport: /%d/"LWIP_NEW_LINE, clientParams->vCfg.aport);
				}
				break;

			case 4: /* vDepth */
//			fprintf(stderr, "3 value of '%s'"API_CLIENT_NEW_LINE, value);
				if (value == NULL)
				{
					fprintf(stderr, "Missing value of '%s'"API_CLIENT_NEW_LINE, token[4]);
					ret = EXIT_FAILURE;
					goto _ret;
				}
				else
				{
					ret = sscanf(value, "%hhd", &chValue);
					if(ret != 1)
					{
						fprintf(stderr, "'%s'='%s' is not integer"API_CLIENT_NEW_LINE, token[4], value );
						ret= EXIT_FAILURE;
						goto _ret;
					}
					ret = EXIT_SUCCESS;
					clientParams->mediaParam.vDepth = chValue;
//					fprintf(stderr, "aport: /%d/"LWIP_NEW_LINE, clientParams->vCfg.aport);
				}
				break;

			case 5: /* vColorSpace */
				if (value == NULL)
				{
					fprintf(stderr, "Missing value of '%s'"API_CLIENT_NEW_LINE, token[5]);
					ret = EXIT_FAILURE;
					goto _ret;
				}
				else
				{
					snprintf(clientParams->rs232Parity, sizeof(clientParams->rs232Parity), "%s", value);
//					fprintf(stderr, "name: /%d/"API_CLIENT_NEW_LINE, clientParams->name);
				}
				break;

			case 6: /* vInterlaced */
//			fprintf(stderr, "2 value of '%s'"API_CLIENT_NEW_LINE, value);
				if (value == NULL)
				{
					fprintf(stderr, "Missing value of '%s'"API_CLIENT_NEW_LINE, token[6]);
					ret = EXIT_FAILURE;
					goto _ret;
				}
				else
				{
					ret = sscanf(value, "%hhd", &chValue);
					if(ret != 1)
					{
						fprintf(stderr, "'%s'='%s' is not integer"API_CLIENT_NEW_LINE, token[6], value );
						ret= EXIT_FAILURE;
						goto _ret;
					}
					ret = EXIT_SUCCESS;
					clientParams->mediaParam.vIsInterlaced = chValue + 1;
//					fprintf(stderr, "vport: /%d/"API_CLIENT_NEW_LINE, clientParams->vCfg.vport);
				}
				break;

			case 7: /* vSegment */
//			fprintf(stderr, "2 value of '%s'"API_CLIENT_NEW_LINE, value);
				if (value == NULL)
				{
					fprintf(stderr, "Missing value of '%s'"API_CLIENT_NEW_LINE, token[7]);
					ret = EXIT_FAILURE;
					goto _ret;
				}
				else
				{
					ret = sscanf(value, "%hhd", &chValue);
					if(ret != 1)
					{
						fprintf(stderr, "'%s'='%s' is not integer"API_CLIENT_NEW_LINE, token[7], value );
						ret= EXIT_FAILURE;
						goto _ret;
					}
					ret = EXIT_SUCCESS;
					clientParams->mediaParam.vIsSegmented = chValue + 1;
//					fprintf(stderr, "vport: /%d/"API_CLIENT_NEW_LINE, clientParams->vCfg.vport);
				}
				break;

		/* audio */

			case 8: /* aFre */
//			fprintf(stderr, "3 value of '%s'"API_CLIENT_NEW_LINE, value);
				if (value == NULL)
				{
					fprintf(stderr, "Missing value of '%s'"API_CLIENT_NEW_LINE, token[8]);
					ret = EXIT_FAILURE;
					goto _ret;
				}
				else
				{
					ret = sscanf(value, "%hd", &shortValue);
					if(ret != 1)
					{
						fprintf(stderr, "'%s'='%s' is not integer"API_CLIENT_NEW_LINE, token[8], value );
						ret= EXIT_FAILURE;
						goto _ret;
					}
					ret = EXIT_SUCCESS;
					clientParams->mediaParam.aSampleRate = shortValue;
//					fprintf(stderr, "aport: /%d/"LWIP_NEW_LINE, clientParams->vCfg.aport);
				}
				break;

			case 9: /* aDepth */
//			fprintf(stderr, "3 value of '%s'"API_CLIENT_NEW_LINE, value);
				if (value == NULL)
				{
					fprintf(stderr, "Missing value of '%s'"API_CLIENT_NEW_LINE, token[9]);
					ret = EXIT_FAILURE;
					goto _ret;
				}
				else
				{
					ret = sscanf(value, "%hhd", &chValue);
					if(ret != 1)
					{
						fprintf(stderr, "'%s'='%s' is not integer"API_CLIENT_NEW_LINE, token[9], value );
						ret= EXIT_FAILURE;
						goto _ret;
					}
					ret = EXIT_SUCCESS;
					clientParams->mediaParam.aDepth= chValue;
//					fprintf(stderr, "aport: /%d/"LWIP_NEW_LINE, clientParams->vCfg.aport);
				}
				break;

			case 10: /* aChannel */
//			fprintf(stderr, "3 value of '%s'"API_CLIENT_NEW_LINE, value);
				if (value == NULL)
				{
					fprintf(stderr, "Missing value of '%s'"API_CLIENT_NEW_LINE, token[10]);
					ret = EXIT_FAILURE;
					goto _ret;
				}
				else
				{
					ret = sscanf(value, "%hhd", &chValue);
					if(ret != 1)
					{
						fprintf(stderr, "'%s'='%s' is not integer"API_CLIENT_NEW_LINE, token[10], value );
						ret= EXIT_FAILURE;
						goto _ret;
					}
					ret = EXIT_SUCCESS;
					clientParams->mediaParam.aChannels = chValue;
//					fprintf(stderr, "aport: /%d/"LWIP_NEW_LINE, clientParams->vCfg.aport);
				}
				break;


			default:
//			fprintf(stderr, "default: value of '%s'"API_CLIENT_NEW_LINE, value);
				fprintf(stderr, "No match found for token: /%s/"API_CLIENT_NEW_LINE, value);
				ret = EXIT_FAILURE;
				goto _ret;
				break;
		}
	}

_ret:
//	fprintf(stderr, "%s : vport=%d,aport='%d/"LWIP_NEW_LINE, clientParams->cmd, clientParams->vCfg.vport ,clientParams->vCfg.aport );
	if(ret)
	{
		fprintf(stderr, "option for %s : /" CLIENT_OPTIONS_SETUP_MEDIA"/'"API_CLIENT_NEW_LINE, clientParams->cmd);
	}

	return ret;
}

static int	_setupSysCfgOptions(struct API_CLIENT_OPT_HANDLER *handler, char *optarg, struct API_PARAMETERS *clientParams)
{
	char *subopts;
	char *value;
	int ret = EXIT_SUCCESS;
	char res;
//	unsigned int ip = 0;
	char isEnable = 0;

	char *const token[] =
	{
		API_CMD_CLIENT_TARGET,
		"mac",
		"ip",
		"dhcp",
		"dips",
		"name",
		NULL
	};

	
	subopts = optarg;
	
	while (*subopts != '\0' && ret != EXIT_FAILURE )
	{
		switch (getsubopt(&subopts, token, &value))
		{
			case 0: /* target */
//			fprintf(stderr, "0 value of '%s'"LWIP_NEW_LINE, value);
				if (value == NULL)
				{
					fprintf(stderr, "Missing value of '%s'"API_CLIENT_NEW_LINE, token[0]);
					ret= EXIT_FAILURE;
					goto _ret;
				}
				else
				{
					res = muxMacAddressParse(&clientParams->target, value);
					if(res == EXIT_FAILURE)
					{
						fprintf(stderr, "'%s'='%s' is not MAC address"API_CLIENT_NEW_LINE, token[0], value );
						ret= EXIT_FAILURE;
						goto _ret;
					}
					snprintf(clientParams->mediaMac, sizeof(clientParams->mediaMac), "%s", value);
//					fprintf(stderr, "MAC: /%s/"LWIP_NEW_LINE, value);

				}
				break;

			case 1: /* MAC */
//			fprintf(stderr, "0 value of '%s'"LWIP_NEW_LINE, value);
				if (value == NULL)
				{
					fprintf(stderr, "Missing value of '%s'"API_CLIENT_NEW_LINE, token[1]);
					ret= EXIT_FAILURE;
					goto _ret;
				}
				else
				{
					res = muxMacAddressParse(&clientParams->vCfg.mac, value);
					if(res == EXIT_FAILURE)
					{
						fprintf(stderr, "'%s'='%s' is not MAC address"API_CLIENT_NEW_LINE, token[1], value );
						ret= EXIT_FAILURE;
						goto _ret;
					}
					snprintf(clientParams->mediaMac, sizeof(clientParams->mediaMac), "%s", value);
//					fprintf(stderr, "MAC: /%s/"LWIP_NEW_LINE, value);

				}
				break;

			case 2: /* IP */
//			fprintf(stderr, "1 value of '%s'"API_CLIENT_NEW_LINE, value);
				if (value == NULL)
				{
					fprintf(stderr, "Missing value of '%s'"API_CLIENT_NEW_LINE, token[2]);
					ret = EXIT_FAILURE;
					goto _ret;
				}
				else
				{
#if 1
					struct in_addr _ip;
					if (inet_aton(value, &_ip) == 0)
#else						
					ip = ipaddr_addr(value);
					if(ip == IPADDR_NONE)
#endif						
					{
						fprintf(stderr, "'%s'='%s' is not IP address"API_CLIENT_NEW_LINE, token[2], value );
						ret= EXIT_FAILURE;
						goto _ret;
					}
#if 0
					clientParams->vCfg.ip = ip;
#else
					clientParams->vCfg.ip = _ip.s_addr;
#endif
//					fprintf(stderr, "IP: /%s/\n", value);
				}
				break;

			case 3: /* DHCP */
//			fprintf(stderr, "2 value of '%s'"API_CLIENT_NEW_LINE, value);
				if (value == NULL)
				{
					fprintf(stderr, "Missing value of '%s'"API_CLIENT_NEW_LINE, token[3]);
					ret = EXIT_FAILURE;
					goto _ret;
				}
				else
				{
					ret = sscanf(value, "%hhd", &isEnable);
					if(ret != 1)
					{
						fprintf(stderr, "'%s'='%s' is not integer"API_CLIENT_NEW_LINE, token[3], value );
						ret= EXIT_FAILURE;
						goto _ret;
					}
					ret = EXIT_SUCCESS;
					clientParams->isDhcp = isEnable + 1;
//					fprintf(stderr, "vport: /%d/"API_CLIENT_NEW_LINE, clientParams->vCfg.vport);
				}
				break;

			case 4: /* DIP Switch */
//			fprintf(stderr, "3 value of '%s'"API_CLIENT_NEW_LINE, value);
				if (value == NULL)
				{
					fprintf(stderr, "Missing value of '%s'"API_CLIENT_NEW_LINE, token[4]);
					ret = EXIT_FAILURE;
					goto _ret;
				}
				else
				{
					ret = sscanf(value, "%hhd", &isEnable);
					if(ret != 1)
					{
						fprintf(stderr, "'%s'='%s' is not integer"API_CLIENT_NEW_LINE, token[4], value );
						ret= EXIT_FAILURE;
						goto _ret;
					}
					ret = EXIT_SUCCESS;
					clientParams->isDipSwitch = isEnable +1;
//					fprintf(stderr, "aport: /%d/"LWIP_NEW_LINE, clientParams->vCfg.aport);
				}
				break;

			case 5: /* name */
//			fprintf(stderr, "4 value of '%s'"API_CLIENT_NEW_LINE, value);
				if (value == NULL)
				{
					fprintf(stderr, "Missing value of '%s'"API_CLIENT_NEW_LINE, token[5]);
					ret = EXIT_FAILURE;
					goto _ret;
				}
				else
				{
					snprintf(clientParams->name, sizeof(clientParams->name), "%s", value);
//					fprintf(stderr, "name: /%d/"API_CLIENT_NEW_LINE, clientParams->name);
				}
				break;

			default:
//			fprintf(stderr, "default: value of '%s'"API_CLIENT_NEW_LINE, value);
				fprintf(stderr, "No match found for token: /%s/"API_CLIENT_NEW_LINE, value);
				ret = EXIT_FAILURE;
				goto _ret;
				break;
		}
	}

_ret:
//	fprintf(stderr, "%s : vport=%d,aport='%d/"LWIP_NEW_LINE, clientParams->cmd, clientParams->vCfg.vport ,clientParams->vCfg.aport );
	if(ret)
	{
		fprintf(stderr, "option for %s : /" CLIENT_OPTIONS_SETUP_SYS"/'"API_CLIENT_NEW_LINE, clientParams->cmd);
	}

	return ret;
}

static int	_setupActionCfgOptions(struct API_CLIENT_OPT_HANDLER *handler, char *optarg, struct API_PARAMETERS *clientParams)
{
	char *subopts;
	char *value;
	int ret = EXIT_SUCCESS;
	char res;

	char *const token[] =
	{
		API_CMD_CLIENT_TARGET,
		MUX_JSON_KEY_ACTION,
		NULL
	};


	subopts = optarg;
	
	while (*subopts != '\0' && ret != EXIT_FAILURE )
	{
		switch (getsubopt(&subopts, token, &value))
		{
			case 0: /* target */
//			fprintf(stderr, "0 value of '%s'"LWIP_NEW_LINE, value);
				if (value == NULL)
				{
					fprintf(stderr, "Missing value of '%s'"API_CLIENT_NEW_LINE, token[0]);
					ret= EXIT_FAILURE;
					goto _ret;
				}
				else
				{
					res = muxMacAddressParse(&clientParams->target, value);
					if(res == EXIT_FAILURE)
					{
						fprintf(stderr, "'%s'='%s' is not MAC address"API_CLIENT_NEW_LINE, token[0], value );
						ret= EXIT_FAILURE;
						goto _ret;
					}
					snprintf(clientParams->mediaMac, sizeof(clientParams->mediaMac), "%s", value);
//					fprintf(stderr, "MAC: /%s/"LWIP_NEW_LINE, value);

				}
				break;

			case 1:
				if (value == NULL)
				{
					fprintf(stderr, "Missing value of '%s'"API_CLIENT_NEW_LINE, token[1]);
					ret= EXIT_FAILURE;
					goto _ret;
				}
				else
				{
					res = strcmp(value, MUX_JSON_KEY_ACTION_START);
					if(res != 0)
					{
						res = strcmp(value, MUX_JSON_KEY_ACTION_STOP);
						if(res != 0)
						{
							fprintf(stderr, "'%s'='%s' error: %s must be %s or %s"API_CLIENT_NEW_LINE, token[1], value, token[1], MUX_JSON_KEY_ACTION_START, MUX_JSON_KEY_ACTION_STOP );
							ret= EXIT_FAILURE;
							goto _ret;
						}	
					}
					snprintf(clientParams->name, sizeof(clientParams->name), "%s", value);
					fprintf(stderr, "action value of '%s'"API_CLIENT_NEW_LINE, clientParams->name );
				}
				break;


			default:
				fprintf(stderr, "No match found for token: /%s/"API_CLIENT_NEW_LINE, value);
				ret = EXIT_FAILURE;
				goto _ret;
				break;
		}
	}

_ret:
	if(ret)
	{
		fprintf(stderr, "option for %s : /"CLIENT_OPTIONS_SETUP_ACTION"/"API_CLIENT_NEW_LINE, clientParams->cmd);
	}

	return ret;
}



static int	_securityCheckCfgOptions(struct API_CLIENT_OPT_HANDLER *handler, char *optarg, struct API_PARAMETERS *clientParams)
{
	char *subopts;
	char *value;
	int ret = EXIT_SUCCESS;
	char res;

	char *const token[] =
	{
		API_CMD_CLIENT_TARGET,
		"hex",
		"id",
		"status",
		NULL
	};

	subopts = optarg;
	
	clientParams->isGetId = 0;
	clientParams->isGetStatus = 0;
	
	while (*subopts != '\0' && ret != EXIT_FAILURE )
	{
		switch (getsubopt(&subopts, token, &value))
		{
			case 0:
//			fprintf(stderr, "0 value of '%s'"LWIP_NEW_LINE, value);
				if (value == NULL)
				{
					fprintf(stderr, "Missing value of '%s'"API_CLIENT_NEW_LINE, token[0]);
					ret= EXIT_FAILURE;
					goto _ret;
				}
				else
				{
					res = muxMacAddressParse(&clientParams->target, value);
					if(res == EXIT_FAILURE)
					{
						fprintf(stderr, "'%s'='%s' is not MAC address"API_CLIENT_NEW_LINE, token[0], value );
						ret= EXIT_FAILURE;
						goto _ret;
					}
					snprintf(clientParams->mediaMac, sizeof(clientParams->mediaMac), "%s", value);
//					fprintf(stderr, "MAC: /%s/"LWIP_NEW_LINE, value);

				}
				break;
				
			case 1:
//			fprintf(stderr, "0 value of '%s'"LWIP_NEW_LINE, value);
				if (value == NULL)
				{
					fprintf(stderr, "Missing value of '%s'"API_CLIENT_NEW_LINE, token[1]);
					ret= EXIT_FAILURE;
					goto _ret;
				}
				else
				{
					res = muxMacAddressParse(&clientParams->hexData, value);
					if(res == EXIT_FAILURE)
					{
						fprintf(stderr, "'%s'='%s' is not MAC address"API_CLIENT_NEW_LINE, token[1], value );
						ret= EXIT_FAILURE;
						goto _ret;
					}
//					snprintf(clientParams->mediaMac, sizeof(clientParams->mediaMac), "%s", value);
//					fprintf(stderr, "MAC: /%s/"LWIP_NEW_LINE, value);

				}
				break;

			case 2:
				clientParams->isGetId = 1;
				break;

			case 3:
				clientParams->isGetStatus = 1;
				break;

			default:
//			fprintf(stderr, "default: value of '%s'"API_CLIENT_NEW_LINE, value);
				fprintf(stderr, "No match found for token: /%s/"API_CLIENT_NEW_LINE, value);
				ret = EXIT_FAILURE;
				goto _ret;
				break;
		}
	}

_ret:
//	fprintf(stderr, "%s : vport=%d,aport='%d/"LWIP_NEW_LINE, clientParams->cmd, clientParams->vCfg.vport ,clientParams->vCfg.aport );
	if(ret)
	{
		fprintf(stderr, "option for %s : /"CLIENT_OPTIONS_SECURITY"/"API_CLIENT_NEW_LINE, clientParams->cmd);
	}

	return ret;
}


static int	_rs232DataCfgOptions(struct API_CLIENT_OPT_HANDLER *handler, char *optarg, struct API_PARAMETERS *clientParams)
{
	char *subopts;
	char *value;
	int ret = EXIT_SUCCESS;
	char res;
//	unsigned int ip = 0;
	unsigned short port = 0;

	char *const token[] =
	{
		API_CMD_CLIENT_TARGET,
		"hex",
		"feed",
		"wait",
		NULL
	};

	subopts = optarg;
	
	while (*subopts != '\0' && ret != EXIT_FAILURE )
	{
		switch (getsubopt(&subopts, token, &value))
		{
			case 0:
//			fprintf(stderr, "0 value of '%s'"LWIP_NEW_LINE, value);
				if (value == NULL)
				{
					fprintf(stderr, "Missing value of '%s'"API_CLIENT_NEW_LINE, token[0]);
					ret= EXIT_FAILURE;
					goto _ret;
				}
				else
				{
					res = muxMacAddressParse(&clientParams->target, value);
					if(res == EXIT_FAILURE)
					{
						fprintf(stderr, "'%s'='%s' is not MAC address"API_CLIENT_NEW_LINE, token[0], value );
						ret= EXIT_FAILURE;
						goto _ret;
					}
					snprintf(clientParams->mediaMac, sizeof(clientParams->mediaMac), "%s", value);
//					fprintf(stderr, "MAC: /%s/"LWIP_NEW_LINE, value);

				}
				break;
				
			case 1:
//			fprintf(stderr, "0 value of '%s'"LWIP_NEW_LINE, value);
				if (value == NULL)
				{
					fprintf(stderr, "Missing value of '%s'"API_CLIENT_NEW_LINE, token[1]);
					ret= EXIT_FAILURE;
					goto _ret;
				}
				else
				{
					res = muxMacAddressParse(&clientParams->hexData, value);
					if(res == EXIT_FAILURE)
					{
						fprintf(stderr, "'%s'='%s' is not MAC address"API_CLIENT_NEW_LINE, token[1], value );
						ret= EXIT_FAILURE;
						goto _ret;
					}
//					snprintf(clientParams->mediaMac, sizeof(clientParams->mediaMac), "%s", value);
//					fprintf(stderr, "MAC: /%s/"LWIP_NEW_LINE, value);

				}
				break;

			case 2:
//			fprintf(stderr, "2 value of '%s'"API_CLIENT_NEW_LINE, value);
				if (value == NULL)
				{
					fprintf(stderr, "Missing value of '%s'"API_CLIENT_NEW_LINE, token[2]);
					ret = EXIT_FAILURE;
					goto _ret;
				}
				else
				{
					ret = sscanf(value, "%hu", &port);
					if(ret != 1)
					{
						fprintf(stderr, "'%s'='%s' is not integer"API_CLIENT_NEW_LINE, token[2], value );
						ret= EXIT_FAILURE;
						goto _ret;
					}
					ret = EXIT_SUCCESS;
					clientParams->isFeed = port;
//					fprintf(stderr, "vport: /%d/"API_CLIENT_NEW_LINE, clientParams->vCfg.vport);
				}
				break;

			case 3:
//			fprintf(stderr, "3 value of '%s'"API_CLIENT_NEW_LINE, value);
				if (value == NULL)
				{
					fprintf(stderr, "Missing value of '%s'"API_CLIENT_NEW_LINE, token[3]);
					ret = EXIT_FAILURE;
					goto _ret;
				}
				else
				{
					ret = sscanf(value, "%hu", &port);
					if(ret != 1)
					{
						fprintf(stderr, "'%s'='%s' is not integer"API_CLIENT_NEW_LINE, token[3], value );
						ret= EXIT_FAILURE;
						goto _ret;
					}
					ret = EXIT_SUCCESS;
					clientParams->waitMs = port;
//					fprintf(stderr, "aport: /%d/"LWIP_NEW_LINE, clientParams->vCfg.aport);
				}
				break;

			default:
//			fprintf(stderr, "default: value of '%s'"API_CLIENT_NEW_LINE, value);
				fprintf(stderr, "No match found for token: /%s/"API_CLIENT_NEW_LINE, value);
				ret = EXIT_FAILURE;
				goto _ret;
				break;
		}
	}

_ret:
//	fprintf(stderr, "%s : vport=%d,aport='%d/"LWIP_NEW_LINE, clientParams->cmd, clientParams->vCfg.vport ,clientParams->vCfg.aport );
	if(ret)
	{
		fprintf(stderr, "option for %s : /"CLIENT_OPTIONS_RS232"/"API_CLIENT_NEW_LINE, clientParams->cmd);
	}

	return ret;
}

static API_CLIENT_OPT_HANDLER _optionHandlers[] =
{
	{
		0,
		API_CMD_FIND,
		CLIENT_OPTIONS_FIND,
		_findCfgOptions,
	},

	/* setup_media cmds */	
	{
		0,
		API_CMD_SETUP_SYS,
		CLIENT_OPTIONS_SETUP_SYS,
		_setupSysCfgOptions,
	},
	{
		0,
		API_CMD_SETUP_RS232,
		CLIENT_OPTIONS_SETUP_RS232,
		_setupRs232CfgOptions,
	},
	{
		0,
		API_CMD_SETUP_PROTOCOL,
		CLIENT_OPTIONS_SETUP_PROTOCOL,
		_setupProtocolCfgOptions,
	},
	{
		0,
		API_CMD_SETUP_MEDIA,
		CLIENT_OPTIONS_SETUP_MEDIA,
		_setupMediaCfgOptions,
	},
	{
		0,
		API_CMD_SETUP_ACTION,
		CLIENT_OPTIONS_SETUP_ACTION,
		_setupActionCfgOptions,
	},


	{
		0,
		API_CMD_RS232,
		CLIENT_OPTIONS_RS232,
		_rs232DataCfgOptions,
	},

	{
		0,
		API_CMD_SECURITY,
		CLIENT_OPTIONS_SECURITY,
		_securityCheckCfgOptions,
	},
	{
		0,
		NULL,
		"No optional params",
		NULL
	}
};

int	apiClientParseSubOptions(char *optarg, struct API_PARAMETERS *clientParams)
{
	API_CLIENT_OPT_HANDLER *handle = _optionHandlers;

	while(handle->handler != NULL)
	{
		if(!strcasecmp(handle->name, clientParams->cmd) )
		{
//			clientParams->cmdType = handle->type;
			return handle->handler(handle, optarg, clientParams);
		}

		handle++;
	}

	return EXIT_FAILURE;
}


