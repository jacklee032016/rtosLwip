
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>

#include <errno.h>

#include "extSysParams.h"
#include "jsmn.h"
#include "extUdpCmd.h"

#include "../lwipTestClient.h"


//static int apiClientUnicastSetParams(API_CLIENT *apiClient, char *_ipaddress, MUX_VIDEO_CONFIG * vCfg)
int apiClientSetupSys(struct API_CLIENT_CMD_HANDLER *handle, API_CLIENT *apiClient)
{
//	unsigned int ipAddress;
	MUX_VIDEO_CONFIG * vCfg = &apiClient->params->vCfg;
	char *data;
	unsigned int index = 0;
	unsigned int size;


	if(apiClientCmdHeaderPrint(handle, apiClient) == EXIT_FAILURE)
	{
		fprintf(stderr, "Error for '%s' \n", apiClient->params->cmd);
		return EXIT_FAILURE;
	}

	data = apiClient->buffer + apiClient->bufIndex;
	size = apiClient->size - apiClient->bufIndex;

	index += snprintf(data+index, size-index, ",\""MUX_IPCMD_DATA_ARRAY"\":[{" );

	if(! MAC_ADDR_IS_NULL(&vCfg->mac) )
	{
		printf("MAC is validate\n" );
		index += snprintf(data+index, size-index, "\""MUX_IPCMD_DATA_MAC"\":");
		MAC_ADDRESS_PRINT(data, size, index, &(vCfg->mac));
	}
	
	if(vCfg->ip != IPADDR_NONE)
	{
		printf("IP invalidate: '%s'\n", inet_ntoa((*(struct in_addr *)&(vCfg->ip))));
		index += snprintf(data+index, size-index, "\""MUX_IPCMD_DATA_IP"\":\"%s\",", inet_ntoa((*(struct in_addr *)&(vCfg->ip)) ));
	}
	
	if(apiClient->params->isDhcp != 0)
	{
		printf("DHCP: '%d'\n", apiClient->params->isDhcp-1);
		index += snprintf(data+index, size-index, "\""MUX_IPCMD_DATA_DHCP"\":%d,", apiClient->params->isDhcp-1 );
	}

	if(apiClient->params->isDipSwitch != 0)
	{
		printf("DIP Switch: '%d'\n", apiClient->params->isDipSwitch -1);
		index += snprintf(data+index, size-index, "\""MUX_IPCMD_DATA_IS_DIP"\":%d,", apiClient->params->isDipSwitch-1 );
	}
	
	if(strlen(apiClient->params->name))
	{
		index += snprintf(data+index, size-index, "\""MUX_IPCMD_DATA_C_NAME"\":\"%s\"", apiClient->params->name);
	}
	
	index += snprintf(data+index, size-index, "}]}" );
	
	if(index <= 0)
	{
		fprintf(stderr, "option for '%s' : -o /"CLIENT_OPTIONS_SETUP_SYS"/(%d)\n", apiClient->params->cmd, index);
		return EXIT_FAILURE;
	}

	if(apiSendout(apiClient, apiClient->buffer, index+apiClient->bufIndex) == EXIT_FAILURE)
	{
		perror("sendto");
		return EXIT_FAILURE;
	}

	apiClientReceive(apiClient);

	return EXIT_SUCCESS;
}


int apiClientSetupRs232(struct API_CLIENT_CMD_HANDLER *handle, API_CLIENT *apiClient)
{
	char *data;
	unsigned int index = 0;
	unsigned int size;

	if(apiClientCmdHeaderPrint(handle, apiClient) == EXIT_FAILURE)
	{
		fprintf(stderr, "Error for '%s' \n", apiClient->params->cmd);
		return EXIT_FAILURE;
	}

	data = apiClient->buffer + apiClient->bufIndex;
	size = apiClient->size - apiClient->bufIndex;

	index += snprintf(data+index, size-index, ",\""MUX_IPCMD_DATA_ARRAY"\":[{" );

	if(apiClient->params->rs232bps != 0 )
	{
		printf("BSP: '%d'\n", apiClient->params->rs232bps);
		index += snprintf(data+index, size-index, "\""MUX_IPCMD_DATA_RS_BAUDRATE"\":%d,", apiClient->params->rs232bps);
	}
	
	if(apiClient->params->rs232data != 0)
	{
		printf("Data: '%d'\n", apiClient->params->rs232data );
		index += snprintf(data+index, size-index, "\""MUX_IPCMD_DATA_RS_DATABITS"\":%d,", apiClient->params->rs232data );
	}

	if(apiClient->params->rs232stop != 0)
	{
		printf("Stop: '%d'\n", apiClient->params->rs232data );
		index += snprintf(data+index, size-index, "\""MUX_IPCMD_DATA_RS_STOPBITS"\":%d,", apiClient->params->rs232stop);
	}
	
	if(strlen(apiClient->params->rs232Parity))
	{
		index += snprintf(data+index, size-index, "\""MUX_IPCMD_DATA_RS_PARITY"\":\"%s\"", apiClient->params->rs232Parity);
	}
	
	index += snprintf(data+index, size-index, "}]}" );
	
	if(index <= 0)
	{
		fprintf(stderr, "option for '%s' : -o /" CLIENT_OPTIONS_SETUP_RS232 "/\n", apiClient->params->cmd);
		return EXIT_FAILURE;
	}

	if(apiSendout(apiClient, apiClient->buffer, index+apiClient->bufIndex) == EXIT_FAILURE)
	{
		perror("sendto");
		return EXIT_FAILURE;
	}

	apiClientReceive(apiClient);

	return EXIT_SUCCESS;
}


int apiClientSetupProtocol(struct API_CLIENT_CMD_HANDLER *handle, API_CLIENT *apiClient)
{
	char *data;
	unsigned int index = 0;
	unsigned int size;


	if(apiClientCmdHeaderPrint(handle, apiClient) == EXIT_FAILURE)
	{
		fprintf(stderr, "Error for '%s' \n", apiClient->params->cmd);
		return EXIT_FAILURE;
	}

	data = apiClient->buffer + apiClient->bufIndex;
	size = apiClient->size - apiClient->bufIndex;

	index += snprintf(data+index, size-index, ",\""MUX_IPCMD_DATA_ARRAY"\":[{" );

	if(apiClient->params->isMcast != 0 )
	{
		printf("IsMCast: '%d'\n", apiClient->params->isMcast );
		index += snprintf(data+index, size-index, "\""MUX_IPCMD_DATA_IS_MCAST"\":%d,", apiClient->params->isMcast -1 );
	}
	
	if(apiClient->params->vCfg.ip !=  IPADDR_NONE)
	{
		printf("MCAST IP: '%s'\n", inet_ntoa((*(struct in_addr *)&(apiClient->params->vCfg.ip))) );
		index += snprintf(data+index, size-index, "\""MUX_IPCMD_DATA_MCAST_IP"\":\"%s\",", inet_ntoa((*(struct in_addr *)&(apiClient->params->vCfg.ip)))  );
	}

	if(apiClient->params->vCfg.vport != 0)
	{
		printf("V.port: '%d'\n", apiClient->params->vCfg.vport);
		index += snprintf(data+index, size-index, "\""MUX_IPCMD_DATA_VIDEO_PORT"\":%d,", apiClient->params->vCfg.vport);
	}

	if(apiClient->params->vCfg.aport != 0)
	{
		printf("A.port: '%d'\n", apiClient->params->vCfg.aport);
		index += snprintf(data+index, size-index, "\""MUX_IPCMD_DATA_AUDIO_PORT"\":%d,", apiClient->params->vCfg.aport);
	}

	if(apiClient->params->vCfg.dport != 0)
	{
		printf("D.port: '%d'\n", apiClient->params->vCfg.dport);
		index += snprintf(data+index, size-index, "\""MUX_IPCMD_DATA_AD_PORT"\":%d,", apiClient->params->vCfg.dport);
	}

	if(apiClient->params->vCfg.sport != 0)
	{
		printf("S.port: '%d'\n", apiClient->params->vCfg.sport);
		index += snprintf(data+index, size-index, "\""MUX_IPCMD_DATA_ST_PORT"\":%d", apiClient->params->vCfg.sport);
	}
	
	
	index += snprintf(data+index, size-index, "}]}" );
	
	if(index <= 0)
	{
		fprintf(stderr, "option for '%s' : -o /"CLIENT_OPTIONS_SETUP_PROTOCOL"/\n", apiClient->params->cmd);
		return EXIT_FAILURE;
	}

	if(apiSendout(apiClient, apiClient->buffer, index+apiClient->bufIndex) == EXIT_FAILURE)
	{
		perror("sendto");
		return EXIT_FAILURE;
	}

	apiClientReceive(apiClient);

	return EXIT_SUCCESS;
}


int apiClientSetupMedia(struct API_CLIENT_CMD_HANDLER *handle, API_CLIENT *apiClient)
{
	char *data;
	unsigned int index = 0;
	unsigned int size;
	MuxRunTimeParam		*mediaParam = &apiClient->params->mediaParam;

	if(apiClientCmdHeaderPrint(handle, apiClient) == EXIT_FAILURE)
	{
		fprintf(stderr, "Error for '%s' \n", apiClient->params->cmd);
		return EXIT_FAILURE;
	}

	data = apiClient->buffer + apiClient->bufIndex;
	size = apiClient->size - apiClient->bufIndex;

	index += snprintf(data+index, size-index, ",\""MUX_IPCMD_DATA_ARRAY"\":[{" );

	if(mediaParam->vWidth!= 0 && mediaParam->vHeight!= 0)
	{
		printf("vWxwH: '%d'x'%d'\n", mediaParam->vWidth, mediaParam->vHeight );
		index += snprintf(data+index, size-index, "\""MUX_IPCMD_DATA_VIDEO_WIDTH"\":%d,", mediaParam->vWidth);
		index += snprintf(data+index, size-index, "\""MUX_IPCMD_DATA_VIDEO_HEIGHT"\":%d,", mediaParam->vHeight);
	}

	if(mediaParam->vFrameRate!= 0 )
	{
		printf("vFps: '%d'\n", mediaParam->vFrameRate );
		index += snprintf(data+index, size-index, "\""MUX_IPCMD_DATA_VIDEO_FRAMERATE"\":%d,", mediaParam->vFrameRate);
	}
	
	if(strlen(apiClient->params->rs232Parity))
	{
		printf("vColorSpace: '%s'\n", apiClient->params->rs232Parity );
		index += snprintf(data+index, size-index, "\""MUX_IPCMD_DATA_VIDEO_COLORSPACE"\":\"%s\",", apiClient->params->rs232Parity);
	}

	if(mediaParam->vDepth != 0 )
	{
		printf("vDepth: '%d'\n", mediaParam->vDepth);
		index += snprintf(data+index, size-index, "\""MUX_IPCMD_DATA_VIDEO_DEPTH"\":%d,", mediaParam->vDepth);
	}
	if(mediaParam->vIsInterlaced!= 0)
	{
		printf("vInterlaced: '%d'\n", mediaParam->vIsInterlaced-1);
		index += snprintf(data+index, size-index, "\""MUX_IPCMD_DATA_VIDEO_INTERLACED"\":%d,", mediaParam->vIsInterlaced-1 );
	}
	if(mediaParam->vIsSegmented != 0)
	{
		printf("vSegment: '%d'\n", mediaParam->vIsSegmented-1);
		index += snprintf(data+index, size-index, "\""MUX_IPCMD_DATA_VIDEO_SEGMENTED"\":%d,", mediaParam->vIsSegmented-1 );
	}

	if(mediaParam->aSampleRate != 0 )
	{
		printf("aFre: '%d'\n", mediaParam->aSampleRate );
		index += snprintf(data+index, size-index, "\""MUX_IPCMD_DATA_AUDIO_SAMPE_RATE"\":%d,", mediaParam->aSampleRate);
	}
	if(mediaParam->aChannels!= 0 )
	{
		printf("aChannel: '%d'\n", mediaParam->aChannels);
		index += snprintf(data+index, size-index, "\""MUX_IPCMD_DATA_AUDIO_CHANNELS"\":%d,", mediaParam->aChannels);
	}

	if(mediaParam->aDepth != 0 )
	{
		printf("aDepth: '%d'\n", mediaParam->aDepth);
		index += snprintf(data+index, size-index, "\""MUX_IPCMD_DATA_AUDIO_DEPTH"\":%d", mediaParam->aDepth);
	}
	
	index += snprintf(data+index, size-index, "}]}" );
	
	if(index <= 0)
	{
		fprintf(stderr, "option for '%s' : -o /"CLIENT_OPTIONS_SETUP_MEDIA"/"API_CLIENT_NEW_LINE, apiClient->params->cmd);
		return EXIT_FAILURE;
	}

	if(apiSendout(apiClient, apiClient->buffer, index+apiClient->bufIndex) == EXIT_FAILURE)
	{
		perror("sendto");
		return EXIT_FAILURE;
	}

	apiClientReceive(apiClient);

	return EXIT_SUCCESS;
}


/* start/stop action */
int apiClientSetupAction(struct API_CLIENT_CMD_HANDLER *handle, API_CLIENT *apiClient)
{
	char *data;
	unsigned int index = 0;
	unsigned int size;

TRACE();
	if(apiClientCmdHeaderPrint(handle, apiClient) == EXIT_FAILURE)
	{
		fprintf(stderr, "Error for '%s' \n", apiClient->params->cmd);
		return EXIT_FAILURE;
	}

	data = apiClient->buffer + apiClient->bufIndex;
	size = apiClient->size - apiClient->bufIndex;

	index += snprintf(data+index, size-index, ",\""MUX_IPCMD_DATA_ARRAY"\":[{" );

	if(!strlen(apiClient->params->name))
	{
		fprintf(stderr, "option for '%s' : -o /"CLIENT_OPTIONS_SETUP_ACTION"/"API_CLIENT_NEW_LINE, apiClient->params->cmd);
		return EXIT_FAILURE;
	}
	
	index += snprintf(data+index, size-index, "\""MUX_IPCMD_DATA_IS_CONNECT"\":\"%d\"", 1);
	
	index += snprintf(data+index, size-index, "}]}" );

	if(apiSendout(apiClient, apiClient->buffer, index+apiClient->bufIndex) == EXIT_FAILURE)
	{
		perror("sendto");
		return EXIT_FAILURE;
	}

	apiClientReceive(apiClient);

	return EXIT_SUCCESS;
}


int apiClientRs232Data(struct API_CLIENT_CMD_HANDLER *handle, API_CLIENT *apiClient)
{
//	unsigned int ipAddress;
	int length;
	char *data = apiClient->buffer;
	int size = apiClient->size;
	int index = 0;

	if(MAC_ADDR_IS_NULL(&apiClient->params->target) || MAC_ADDR_IS_NULL(&apiClient->params->hexData) )
	{
		fprintf(stderr, "option for '%s' : -o /"CLIENT_OPTIONS_RS232"/\n", apiClient->params->cmd);
		return EXIT_FAILURE;
	}

	if(apiClientCmdHeaderPrint(handle, apiClient) == EXIT_FAILURE)
	{
		fprintf(stderr, "Error for '%s' \n", apiClient->params->cmd);
		return EXIT_FAILURE;
	}

	data = apiClient->buffer + apiClient->bufIndex;
	size = apiClient->size - apiClient->bufIndex;

	index += snprintf(data+index, size-index, ",\""MUX_IPCMD_DATA_ARRAY"\":[{" );

	index += snprintf(data+index, size-index, "\""MUX_IPCMD_RS232_DATA_HEX"\":\"%02X%02X%02X%02X%02X%02X\",", 
		apiClient->params->hexData.address[0], apiClient->params->hexData.address[1], apiClient->params->hexData.address[2],
		apiClient->params->hexData.address[3], apiClient->params->hexData.address[4], apiClient->params->hexData.address[5]);
	index += snprintf(data+index, size-index, "\""MUX_IPCMD_RS232_FEEDBACK"\":%d,", (apiClient->params->isFeed==0)?0:1);
	index += snprintf(data+index, size-index, "\""MUX_IPCMD_RS232_WAIT_TIME"\":%d", apiClient->params->waitMs );

	index += snprintf(data+index, size-index, "}]}" );

	length = apiClient->bufIndex + index;

	if(apiSendout(apiClient, apiClient->buffer, length) == EXIT_FAILURE)
	{
		perror("sendto");
		return EXIT_FAILURE;
	}

	apiClientReceive(apiClient);

	return EXIT_SUCCESS;
}



int apiClientSecurityCheck(struct API_CLIENT_CMD_HANDLER *handle, API_CLIENT *apiClient)
{
//	unsigned int ipAddress;
	int length;
	char *data = apiClient->buffer;
	int size = apiClient->size;
	int index = 0;

	if(MAC_ADDR_IS_NULL(&apiClient->params->target) )//|| MAC_ADDR_IS_NULL(&apiClient->params->hexData) )
	{
		fprintf(stderr, "option for '%s' : -o /"CLIENT_OPTIONS_SECURITY"/\n", apiClient->params->cmd);
		return EXIT_FAILURE;
	}

	if(apiClientCmdHeaderPrint(handle, apiClient) == EXIT_FAILURE)
	{
		fprintf(stderr, "Error for '%s' \n", apiClient->params->cmd);
		return EXIT_FAILURE;
	}

	data = apiClient->buffer + apiClient->bufIndex;
	size = apiClient->size - apiClient->bufIndex;

	index += snprintf(data+index, size-index, ",\""MUX_IPCMD_DATA_ARRAY"\":[{" );

	if(! MAC_ADDR_IS_NULL(&apiClient->params->hexData) )
	{
		index += snprintf(data+index, size-index, "\""MUX_IPCMD_SC_SET_KEY"\":\"%02X%02X%02X%02X%02X%02X%02X%02X\"", 
			apiClient->params->hexData.address[0], apiClient->params->hexData.address[1], apiClient->params->hexData.address[2],
			apiClient->params->hexData.address[3], apiClient->params->hexData.address[4], apiClient->params->hexData.address[5], 0xA5, 0x5A);
	}
	else if(apiClient->params->isGetId )
	{
		index += snprintf(data+index, size-index, "\""MUX_IPCMD_SC_GET_ID"\":\"\"" );
	}
	else if(apiClient->params->isGetStatus)
	{
		index += snprintf(data+index, size-index, "\""MUX_IPCMD_SC_GET_STATUS"\":\"\"" );
	}
	else 
	{
		fprintf(stderr, "Error for '%s': no validate sub-command \n", apiClient->params->cmd);
		return EXIT_FAILURE;
	}

	index += snprintf(data+index, size-index, "}]}" );
	
	
	length = apiClient->bufIndex + index;

	if(apiSendout(apiClient, apiClient->buffer, length) == EXIT_FAILURE)
	{
		perror("sendto");
		return EXIT_FAILURE;
	}

	apiClientReceive(apiClient);
	
	return EXIT_SUCCESS;
}




