

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


static int _apiSendoutSocket(int socket, struct sockaddr_in *peerAddress, void *data, unsigned int size)
{
	unsigned int crc32 =0;
	CMN_IP_COMMAND ipCmd;
	
	memset(&ipCmd, 0, sizeof(CMN_IP_COMMAND));
	
	ipCmd.tag = CMD_TAG_REQUEST;
	ipCmd.length = htons(size + 4);

	memcpy( ipCmd.data, data, size);
	crc32 =  htonl(cmnMuxCRC32b(data, size));
	*((unsigned int *)(ipCmd.data+size)) = crc32;

	printf("\tsend out '%s'"MUX_NEW_LINE, (char *)data);
	if(sendto(socket, &ipCmd, size+8, 0, (struct sockaddr *)peerAddress, sizeof(struct sockaddr_in)) < 0)
	{
		perror("sendto");
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

int apiSendout(API_CLIENT *apiClient, void *data, unsigned int size )
{
	memset(&apiClient->peerAddress, 0, sizeof(struct sockaddr_in));
	
//	ipAddress = ipaddr_addr(apiClient->params->address);
	if (inet_aton(apiClient->params->address, &apiClient->peerAddress.sin_addr) == 0)
//	if(ipAddress == IPADDR_NONE)
	{
		fprintf(stderr, "'%s' -c address: for IP of device\n", apiClient->params->cmd);
		return EXIT_FAILURE;
	}

	apiClient->peerAddress.sin_family = AF_INET;
	apiClient->peerAddress.sin_port = htons(apiClient->port);

	if (( apiClient->socket = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
	{
		perror("socket");
		return EXIT_FAILURE;
	}

	return _apiSendoutSocket(apiClient->socket, (struct sockaddr_in *)&apiClient->peerAddress, data, size);
}


int apiRecvCheck(struct sockaddr_in *peerAddress, void *data, unsigned int size)
{
	unsigned int crcReceived, crcDecoded;
	CMN_IP_COMMAND *ipCmd = (CMN_IP_COMMAND *)data;

	ipCmd->length = ntohs(ipCmd->length);
	printf("\tRECV %d bytes from %s: msg :%d bytes: '%.*s'\r\n", size, inet_ntoa(peerAddress->sin_addr), ipCmd->length-4, ipCmd->length-4,  ipCmd->data  );

	if (CMD_TAG_RESPONSE != ipCmd->tag)
	{
		printf("Tag of command is wrong:0x%x\r\n", ipCmd->tag);
		return EXIT_FAILURE;
	}

	if(ipCmd->length+4 != size )
	{
		printf("length field:%d, received length:%d\r\n", ipCmd->length, size);
		return EXIT_FAILURE;
	}
	
	crcReceived = *((unsigned int *)(ipCmd->data+ ipCmd->length-4));	/* the first 4 bytes are header of command, so here is the position of CRC */
	crcDecoded = htonl(cmnMuxCRC32b(ipCmd->data, ipCmd->length-4));
	if (crcReceived != crcDecoded)
	{//Wrong CRC
		printf("CRC of command is wrong:received CRC: 0x%x, Decoded CRC:0x%x\r\n", crcReceived, crcDecoded);
		return EXIT_FAILURE;
	}

//	printf("received CRC: 0x%x, Decoded CRC:0x%x, msg='%.*s'\r\n", crcReceived, crcDecoded, ipCmd->length-4, ipCmd->data );

	return EXIT_SUCCESS;
}


int apiClientReceive(API_CLIENT *apiClient)
{
	int ret =0;
//	unsigned int ipAddress;

	struct timeval tv;
	tv.tv_sec = BC_CLIETN_TIMEOUT;
	tv.tv_usec = 100000;
	if (setsockopt(apiClient->socket, SOL_SOCKET, SO_RCVTIMEO,&tv,sizeof(tv)) < 0)
	{
		perror("Error");
		return EXIT_FAILURE;
	}
	memset(apiClient->buffer,0 , apiClient->size);
	ret = recvfrom(apiClient->socket, apiClient->buffer, apiClient->size, 0, NULL, NULL);
	close(apiClient->socket);
	if(ret < 0)
	{
		if(errno == EAGAIN)
		{
			printf("\ttimeout in unicast receive..."API_CLIENT_NEW_LINE API_CLIENT_NEW_LINE API_CLIENT_NEW_LINE);
			return BC_CLIENT_ERROR_TIMEOUT;
		}
		
		perror("recvfrom from unicast");
		return EXIT_FAILURE;
	}
	else if(ret == 0)
	{
		printf("ERROR: network broken or buffer size is 0\n\r");
		return EXIT_FAILURE;
	}

	apiRecvCheck(&apiClient->peerAddress, apiClient->buffer, ret);

	apiClient->buffer[ret] = '\0';
	
	return EXIT_SUCCESS;
}



#define	_MAC_ADDRESS_DEBUG(mac) \
		{printf( "%02x:%02x:%02x:%02x:%02x:%02x"API_CLIENT_NEW_LINE,  \
			(mac)->address[0] , (mac)->address[1] , (mac)->address[2] , (mac)->address[3], (mac)->address[4], (mac)->address[5] );}


static const char *_jsonGetParamsCmd = 
		"{\""MUX_IPCMD_KEY_COMMAND"\":\""MUX_IPCMD_CMD_GET_PARAMS"\", "
		"\""MUX_IPCMD_LOGIN_ACK"\":\""MUX_USER"\", "
		"\""MUX_IPCMD_PWD_MSG"\":\""MUX_PASSWORD"\", "
		   "\""MUX_IPCMD_KEY_TARGET"\":\"FF:FF:FF:FF:FF:FF\" }";


char apiClientCmdHeaderPrint(struct API_CLIENT_CMD_HANDLER *handle, API_CLIENT *apiClient)
{
	int index = 0;
	char *data = apiClient->buffer;
	int size = apiClient->size;
		
	index += snprintf(data+index, size-index, "{\""MUX_IPCMD_KEY_TARGET"\":" );
	MAC_ADDRESS_PRINT(data, size, index, &(apiClient->params->target));
	index += snprintf(data+index, size-index, "\""MUX_IPCMD_KEY_COMMAND"\":\"%s\",", handle->ipCmdName );

	index += snprintf(data+index, size-index, "\""MUX_IPCMD_LOGIN_ACK"\":\"%s\",", MUX_USER);
	index += snprintf(data+index, size-index, "\""MUX_IPCMD_PWD_MSG"\":\"%s\"", MUX_PASSWORD );

//	index += snprintf(data+index, size-index, "\""MUX_JSON_KEY_DEBUG"\":\"%s\"}", parser->msg);
	apiClient->bufIndex = index;

	return EXIT_SUCCESS;
}



#if API_CLIENT_DEBUG_OUT
static int apiClientParseAddDevice(API_CLIENT *apiClient)
{
//	int index = 0;
	char	ret;
	MUX_JSON_PARSER replyParser;
	MUX_RUNTIME_CFG replyCfg;
	AN767_DEV_T		*dev = NULL;

	memset(&replyCfg, 0, sizeof(MUX_RUNTIME_CFG));

	replyParser->runCfg = &replyCfg;

	printf("\tparse reply params..."LWIP_NEW_LINE);
	ret = muxJsonRequestParseCommand(apiClient->buffer, strlen(apiClient->buffer), &replyParser);
	if(ret == EXIT_FAILURE)
	{
		printf("Parse reply SetParams failed"API_CLIENT_NEW_LINE);
		return EXIT_FAILURE;
	}
	
	ret = muxJsonRequestParse(&replyParser);
	if(ret == EXIT_FAILURE)
	{
		printf("Parse reply cfg of SetParams failed"API_CLIENT_NEW_LINE);
		return EXIT_FAILURE;
	}

	dev = (AN767_DEV_T *)malloc(sizeof(AN767_DEV_T));
	if(dev==NULL)
	{
		printf("Can't allocate memory for device"API_CLIENT_NEW_LINE);
		return EXIT_FAILURE;
	}
	memset(dev, 0, sizeof(AN767_DEV_T));

	memcpy(&dev->cfg, &replyCfg, sizeof(MUX_RUNTIME_CFG));
	ADD_ELEMENT(apiClient->devs, dev);
	
	return EXIT_SUCCESS;
}

static void	apiClientDebugCfg(MUX_RUNTIME_CFG *cfg, const char *prompt)
{
	printf("%s: "API_CLIENT_NEW_LINE, prompt);

	printf("TX/RX\t: %s\n\r", MUX_IS_TX(cfg)?"TX":"RX");
	printf(MUX_JSON_KEY_MAC"\t: ");
	_MAC_ADDRESS_DEBUG(&cfg->local.mac);
	printf(MUX_JSON_KEY_IP"\t: %s\n\r",  MUX_LWIP_IPADD_TO_STR(&cfg->local.ip));
	printf(MUX_JSON_KEY_MASK"\t: %s\n\r",  MUX_LWIP_IPADD_TO_STR(&cfg->ipMask));
	printf(MUX_JSON_KEY_GATEWAY"\t: %s\n\r",  MUX_LWIP_IPADD_TO_STR(&cfg->ipGateway));
	printf(MUX_JSON_KEY_DHCP"\t: %s\n\r", MUX_DHCP_IS_ENABLE(cfg)?"Enable":"Disable");

	printf(MUX_JSON_KEY_VIDEO_MAC_DEST"\t: ");
	_MAC_ADDRESS_DEBUG(&cfg->dest.mac);
	printf(MUX_JSON_KEY_VIDEO_IP_DEST"\t: %s\n\r",  MUX_LWIP_IPADD_TO_STR(&cfg->dest.ip) );
	printf(MUX_JSON_KEY_VIDEO_PORT_DEST"\t: %d\n\r", cfg->dest.vport);
	printf(MUX_JSON_KEY_AUDIO_PORT_DEST"\t: %d\n\r", cfg->dest.aport);

	if(cfg->local.ip != IPADDR_NONE)
	{
		printf(MUX_JSON_KEY_VIDEO_MAC_LOCAL"\t: ");
		_MAC_ADDRESS_DEBUG(&cfg->local.mac);
		printf(MUX_JSON_KEY_VIDEO_IP_LOCAL"\t: %s\n\r",  MUX_LWIP_IPADD_TO_STR(&cfg->local.ip) );
		printf(MUX_JSON_KEY_VIDEO_PORT_LOCAL"\t: %d\n\r", cfg->local.vport);
		printf(MUX_JSON_KEY_AUDIO_PORT_LOCAL"\t: %d\n\r", cfg->local.aport);
	}

}
#endif

int apiClientFind(struct API_CLIENT_CMD_HANDLER *handle, API_CLIENT *apiClient)
{
	struct sockaddr_in s;
	int broadcastEnable=1;
#if API_CLIENT_DEBUG_OUT
	AN767_DEV_T *dev;
	int index = 0;
#endif
	int count = 0;
	struct sockaddr_in client_address;
	size_t client_address_len = 0;
	char *devName;
	
	if (( apiClient->bcSocket = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
	{
		perror("socket");
		return EXIT_FAILURE;
	}

#if 1
	/* boardcast can not be on all interface in Linux*/
	if(strlen(apiClient->params->name))
	{
		devName = apiClient->params->name;
	}
	else
	{
		devName = DEVICE_NAME;
		fprintf(stderr, "\toption for %s : /" CLIENT_OPTIONS_FIND"/'"API_CLIENT_NEW_LINE, apiClient->params->cmd);
	}
	
	if(setsockopt(apiClient->bcSocket, SOL_SOCKET, SO_BINDTODEVICE, devName, strlen(devName)))
	{
		perror("Bind Interface");
		return EXIT_FAILURE;
	}
#endif

	memset(&s, '\0', sizeof(struct sockaddr_in));
	s.sin_family = AF_INET;
	s.sin_port = (in_port_t)htons(apiClient->port);

#if 1
	int ret=setsockopt(apiClient->bcSocket, SOL_SOCKET, SO_BROADCAST, &broadcastEnable, sizeof(broadcastEnable));
	s.sin_addr.s_addr = htonl(INADDR_BROADCAST);
//	inet_aton("192.168.166.255", &s.sin_addr);
#else
	s.sin_addr.s_addr = htonl(INADDR_ANY);
#endif


	printf("\tsend %d bytes BOARDCAST '%.*s' on device %s\n\r\n\r", strlen(_jsonGetParamsCmd), strlen(_jsonGetParamsCmd), _jsonGetParamsCmd, devName);
//	printf("\tsend %d bytes BOARDCAST '%.*s' on all interfaces\n\r\n\r", strlen(_jsonGetParamsCmd), strlen(_jsonGetParamsCmd), _jsonGetParamsCmd );

	if(_apiSendoutSocket(apiClient->bcSocket, &s, (void *)_jsonGetParamsCmd, strlen(_jsonGetParamsCmd) )== EXIT_FAILURE)
	{
		perror("sendto");
		return EXIT_FAILURE;
	}

	struct timeval tv;
	tv.tv_sec = BC_CLIETN_TIMEOUT;
	tv.tv_usec = 100000;
	if (setsockopt(apiClient->bcSocket, SOL_SOCKET, SO_RCVTIMEO,&tv,sizeof(tv)) < 0)
	{
		perror("Error");
		return EXIT_FAILURE;
	}

	do
	{
		memset(apiClient->buffer,0 , apiClient->size );
		client_address_len = sizeof(struct sockaddr);
		ret = recvfrom(apiClient->bcSocket, apiClient->buffer, apiClient->size, 0, (struct sockaddr *)&client_address, &client_address_len );
		if(ret > 0)
		{
//			printf("\tRECV %d bytes from %s: '%s'\n\r\n\r", ret, inet_ntoa(client_address.sin_addr), apiClient->buffer );
//			printf("\tRECV %d bytes: '%s'\n\r\n\r", ret, apiClient->buffer );
			apiRecvCheck(&client_address, apiClient->buffer, ret);
#if API_CLIENT_DEBUG_OUT
			apiClientParseAddDevice(apiClient);
#endif
			count++;
			client_address_len = 0;
		}	
	}while(ret>0);
	
	close(apiClient->bcSocket);

	if(ret <= 0)
	{
		if(count>0)
		{
			return EXIT_SUCCESS;
		}
		if(errno == 0)
		{/* no error */
			printf("Connection closed\r\n");
		}
		
		printf("recv from boardcast error code :%d"API_CLIENT_NEW_LINE, errno);
		if(errno != EAGAIN)
		{
			perror("failed: recvfrom from boardcast");
			return EXIT_FAILURE;
		}

		return BC_CLIENT_ERROR_TIMEOUT;
	}

#if API_CLIENT_DEBUG_OUT
	/* timeout */	
	dev = apiClient->devs;
	while(dev)
	{
		char		msg[128];
		index++;
		sprintf(msg, API_CLIENT_NEW_LINE API_CLIENT_NEW_LINE"***********************No.%d", index);
		apiClientDebugCfg(&dev->cfg, msg);
		dev = dev->next;
	}
#endif
 
//	apiClient->buffer[ret] = '\0';
//	printf("\trecieved BOARDCAST reply %d bytes: '%s'\n\r\n\r", ret, apiClient->buffer);

	return EXIT_SUCCESS;
}


