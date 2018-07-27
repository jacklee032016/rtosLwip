

#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>
#include <string.h>
#include <stdio.h>

#include <unistd.h>

#include "muxSysParams.h"
#include "jsmn.h"
#include "muxIpCmd.h"

#include "../lwipTestClient.h"

extern char *optarg;
extern int optind, opterr, optopt;

/* add dump function for calling library of lwip */
unsigned int sys_now(void)
{
	return 1000000L;
}


static void usage(char* base, struct API_PARAMETERS *params)
{
	printf("%s: "MUX_NEW_LINE "\tCommand line interface for JSON API." MUX_NEW_LINE 
		"\t%s -a ipaddress/fqdn -c command -o options" MUX_NEW_LINE 
		"\tcmds: " API_CMD_FIND ", " API_CMD_SETUP_SYS ", "API_CMD_SETUP_RS232 ", "API_CMD_SETUP_PROTOCOL", " API_CMD_SETUP_MEDIA", "API_CMD_SETUP_ACTION ", "  MUX_NEW_LINE 
		"\t      " API_CMD_RS232", " API_CMD_SECURITY " "MUX_NEW_LINE, 
		  base, base);

	exit(-1);
}


API_CLIENT_CMD_HANDLER apiClientCmdHandlers[]=
{
	/* PLAY commands */
	{
		.name = API_CMD_FIND,
		.ipCmdName = MUX_IPCMD_CMD_GET_PARAMS,	
		.validate = NULL,
		.execute = apiClientFind
	},

	{
		.name = API_CMD_SETUP_SYS,
		.ipCmdName = MUX_IPCMD_CMD_SET_PARAMS,	
		.validate = NULL,
		.execute = apiClientSetupSys
	},

	{
		.name = API_CMD_SETUP_RS232,
		.ipCmdName = MUX_IPCMD_CMD_SET_PARAMS,	
		.validate = NULL,
		.execute = apiClientSetupRs232
	},

	{
		.name = API_CMD_SETUP_PROTOCOL,
		.ipCmdName = MUX_IPCMD_CMD_SET_PARAMS,	
		.validate = NULL,
		.execute = apiClientSetupProtocol
	},

	{
		.name = API_CMD_SETUP_MEDIA,
		.ipCmdName = MUX_IPCMD_CMD_SET_PARAMS,	
		.validate = NULL,
		.execute = apiClientSetupMedia
	},
	
	{
		.name = API_CMD_SETUP_ACTION,
		.ipCmdName = MUX_IPCMD_CMD_SET_PARAMS,	
		.validate = NULL,
		.execute = apiClientSetupAction
	},
	
	{
		.name = API_CMD_RS232,
		.ipCmdName = MUX_IPCMD_CMD_SEND_RS232,
		.validate = NULL,
		.execute = apiClientRs232Data
	},

	{
		.name = API_CMD_SECURITY,
		.ipCmdName = MUX_IPCMD_CMD_SECURITY_CHECK,
		.validate = NULL,
		.execute = apiClientSecurityCheck
	},


	{
		.name = NULL,
		.validate = NULL,
		.execute = NULL
	}
};

static int	_apiFindCmd(API_CLIENT *apiClient, char *programName)
{
	API_CLIENT_CMD_HANDLER *handle = apiClientCmdHandlers;
	struct API_PARAMETERS *params = apiClient->params;
	
	while(handle->execute )
	{
		if(IS_STRING_EQUAL(handle->name, params->cmd) )
		{
			return EXIT_SUCCESS;
		}
		
		handle++;
	}
	return EXIT_FAILURE;
}



static int	_apiHandleCmd(API_CLIENT *apiClient, char *programName)
{
	int ret = EXIT_SUCCESS;

	API_CLIENT_CMD_HANDLER *handle = apiClientCmdHandlers;
	struct API_PARAMETERS *params = apiClient->params;

	while(handle->execute )
	{
		if(IS_STRING_EQUAL(handle->name, params->cmd) )
		{
			return (handle->execute)(handle, apiClient);
		}
		
		handle++;
	}

	printf("WARNS: '%s' command is not found\n\n", params->cmd);
	usage(programName, params);
	return ret;
}

int main(int argc, char *argv[])
{
	int opt;
	MUX_RUNTIME_CFG _clientCfg;
	struct	API_PARAMETERS params;


	API_CLIENT client;
	API_CLIENT *apiClient = &client;
	int	ret = 0;

	memset(&params, 0, sizeof(struct API_PARAMETERS));
	memset(apiClient, 0, sizeof(API_CLIENT));
	memset(&_clientCfg, 0 , sizeof(MUX_RUNTIME_CFG));
	apiClient->port = MUX_CTRL_PORT;
	apiClient->size = sizeof(apiClient->buffer);
	apiClient->clientCfg = &_clientCfg;
	apiClient->params = &params;


	while ((opt = getopt (argc, argv, "a:c:o:")) != -1)
	{
		switch (opt)
		{
			case 'a':
				snprintf(params.address, sizeof(params.address), "%s",optarg);
				break;

			case 'c':
				snprintf(params.cmd, sizeof(params.cmd), "%s", optarg);
				break;

			case 'o':
				if(IS_STRING_NULL(params.cmd))
				{
					fprintf(stderr, "No command defined before options for the command, use -c CMD before -o OPTIONS\n");
					exit(1);
				}

				ret = _apiFindCmd(apiClient, argv[0]);
				if(ret ==  EXIT_FAILURE)
				{
					printf("Unknow command '%s' \n", params.cmd);
					usage(argv[0], &params);
				}

				ret = apiClientParseSubOptions(optarg, &params);
				if(ret )
				{
					exit(1);
				}
				break;

			default:
				usage(argv[0], &params);
		}

	}

	if( IS_STRING_NULL(params.cmd) )
	{
		printf("Command is not defined\n");
		usage(argv[0], &params);
	}
	

	ret = _apiHandleCmd(apiClient, argv[0]);
	if(ret ==  EXIT_FAILURE)
	{
		printf("command '%s' failed\n", params.cmd);
		usage(argv[0], &params);
	}

	return ret;
}


