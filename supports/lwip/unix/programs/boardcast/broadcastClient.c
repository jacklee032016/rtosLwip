
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>
#include <string.h>
#include <stdio.h>

#include <unistd.h>

#include "lwipExt.h"

#include "../lwipTestClient.h"


typedef	struct
{
	int						state;
	
	int						port;
	int 						bcSocket;
	
	int						socket;
//	int						peerAddress;
	struct sockaddr_in			peerAddress;
	EXT_UUID_T				uuid;
	EXT_MAC_ADDRESS		macAddress;

	char						buffer[8192];
	int						size;

	EXT_JSON_PARSER  		parser;

	EXT_RUNTIME_CFG		*clientCfg;

}UDP_CLIENT;

const char *jsonGetParamsCmd = 
		"{\""EXT_JSON_KEY_COMMAND"\":\""EXT_JSON_CMD_GET_PARAMS"\", "
		"\""EXT_JSON_KEY_USER"\":\""EXT_USER"\", "
		"\""EXT_JSON_KEY_PASSWORD"\":\""EXT_PASSWORD"\", "
		   "\""EXT_JSON_KEY_ID"\":\"FFFFFFFF-FFFF-FFFF-FFFF-FFFFFFFFFFFF\" }";

const char *jsonGetParamsErrorUser = 
		"{\""EXT_JSON_KEY_COMMAND"\":\""EXT_JSON_CMD_GET_PARAMS"\", "
		"\""EXT_JSON_KEY_USER"\":\"muxLab\", "
		"\""EXT_JSON_KEY_PASSWORD"\":\""EXT_PASSWORD"\", "
		   "\""EXT_JSON_KEY_ID"\":\"FFFFFFFF-FFFF-FFFF-FFFF-FFFFFFFFFFFF\" }";



#if 0
const char *jsonRequestSetParams = "{ \""EXT_JSON_KEY_COMMAND"\":\""EXT_JSON_CMD_SET_PARAMS"\", "
		   "\""EXT_JSON_KEY_IP"\":\"192.168.167.55\", "
		   "\""EXT_JSON_KEY_MASK"\":\"255.255.255.0\", "
		   "\""EXT_JSON_KEY_GATEWAY"\":\"192.168.167.1\", "
		   "\""EXT_JSON_KEY_DHCP"\":0, "
		   "\""EXT_JSON_KEY_TX"\":1, "
		   "\""EXT_JSON_KEY_DEST_IP"\":\"192.168.167.56\", "
		   "\""EXT_JSON_KEY_VIDEO_MAC_LOCAL"\":\"00:A0:E5:00:11:10\","
		   "\""EXT_JSON_KEY_VIDEO_MAC_DEST"\":\"00:A0:E5:00:11:11\", "
		   "\""EXT_JSON_KEY_VIDEO_IP_LOCAL"\":\"172.16.4.211\","
		   "\""EXT_JSON_KEY_VIDEO_IP_DEST"\":\"172.16.4.210\", "
		   "\""EXT_JSON_KEY_VIDEO_PORT_LOCAL"\":40712, "
		   "\""EXT_JSON_KEY_VIDEO_PORT_DEST"\":40696, "
		   "\""EXT_JSON_KEY_AUDIO_PORT_LOCAL"\":40716, "
		   "\""EXT_JSON_KEY_AUDIO_PORT_DEST"\":40700, "
		   "\""EXT_JSON_KEY_MC_IP"\":\"239.0.121.1\","
		   "\""EXT_JSON_KEY_MC_PORT"\":2700, "
		   "\""EXT_JSON_KEY_IS_CONNECT"\":1," 
		   "\""EXT_JSON_KEY_IS_MC"\":1, ";

//		   "\""EXT_JSON_KEY_MAC"\":\"1a:25:23:45:ab:d2\", "

#else
const char *jsonRequestSetParams = "{ \""EXT_JSON_KEY_COMMAND"\":\""EXT_JSON_CMD_SET_PARAMS"\", "
		   "\""EXT_JSON_KEY_ID"\":\"39300000-7e16-dcd3-0000-0004251ca002\","
		   "\""EXT_JSON_KEY_MAC"\":\"1a:25:23:45:ab:d2\", "
#if LWIP_FOR_SIMHOST
		   "\""EXT_JSON_KEY_IP"\":\"192.168.167.55\", "
		   "\""EXT_JSON_KEY_MASK"\":\"255.255.255.0\", "
		   "\""EXT_JSON_KEY_GATEWAY"\":\"192.168.167.1\", "
		   "\""EXT_JSON_KEY_DEST_IP"\":\"192.168.167.56\", "
#else
		   "\""EXT_JSON_KEY_IP"\":\"192.168.168.130\", "
		   "\""EXT_JSON_KEY_MASK"\":\"255.255.255.0\", "
		   "\""EXT_JSON_KEY_GATEWAY"\":\"192.168.168.1\", "
		   "\""EXT_JSON_KEY_DEST_IP"\":\"192.168.168.150\", "
#endif
		   "\""EXT_JSON_KEY_DHCP"\":0, "
		   "\""EXT_JSON_KEY_TX"\":1, "
		   "\""EXT_JSON_KEY_VIDEO_MAC_LOCAL"\":\"00:A0:E5:00:11:10\","
		   "\""EXT_JSON_KEY_VIDEO_MAC_DEST"\":\"00:A0:E5:00:11:11\", "
		   "\""EXT_JSON_KEY_VIDEO_IP_LOCAL"\":\"172.16.4.211\","
		   "\""EXT_JSON_KEY_VIDEO_IP_DEST"\":\"172.16.4.210\", "
		   "\""EXT_JSON_KEY_VIDEO_PORT_LOCAL"\":40712, "
		   "\""EXT_JSON_KEY_VIDEO_PORT_DEST"\":40696, "
		   "\""EXT_JSON_KEY_AUDIO_PORT_LOCAL"\":40716, "
		   "\""EXT_JSON_KEY_AUDIO_PORT_DEST"\":40700, "
		   "\""EXT_JSON_KEY_MC_IP"\":\"239.0.121.1\","
		   "\""EXT_JSON_KEY_MC_PORT"\":2700, "
		   "\""EXT_JSON_KEY_IS_CONNECT"\":1," 
		   "\""EXT_JSON_KEY_IS_MC"\":1 }";
#endif

const char *jsonRequestNoCommand = "{ \""EXT_JSON_KEY_COMMAND"\":\"NoCommand\", "
		   "\""EXT_JSON_KEY_ID"\":\"5da68790-2198-43cb-9321-2951eeb5ee90\","
		   "\""EXT_JSON_KEY_MAC"\":\"1a:25:23:45:ab:d2\", "
		   "\""EXT_JSON_KEY_IP"\":\"192.168.167.55\", "
		   "\""EXT_JSON_KEY_MASK"\":\"255.255.255.0\", "
		   "\""EXT_JSON_KEY_GATEWAY"\":\"192.168.167.1\", "
		   "\""EXT_JSON_KEY_DHCP"\":0, "
		   "\""EXT_JSON_KEY_TX"\":1, "
		   "\""EXT_JSON_KEY_DEST_IP"\":\"192.168.167.56\", "
		   "\""EXT_JSON_KEY_VIDEO_MAC_LOCAL"\":\"00:A0:E5:00:11:10\","
		   "\""EXT_JSON_KEY_VIDEO_MAC_DEST"\":\"00:A0:E5:00:11:11\", "
		   "\""EXT_JSON_KEY_VIDEO_IP_LOCAL"\":\"172.16.4.211\","
		   "\""EXT_JSON_KEY_VIDEO_IP_DEST"\":\"172.16.4.210\", "
		   "\""EXT_JSON_KEY_VIDEO_PORT_LOCAL"\":40712, "
		   "\""EXT_JSON_KEY_VIDEO_PORT_DEST"\":40696, "
		   "\""EXT_JSON_KEY_AUDIO_PORT_LOCAL"\":40716, "
		   "\""EXT_JSON_KEY_AUDIO_PORT_DEST"\":40700, "
		   "\""EXT_JSON_KEY_MC_IP"\":\"239.0.121.1\","
		   "\""EXT_JSON_KEY_MC_PORT"\":2700, "
		   "\""EXT_JSON_KEY_IS_CONNECT"\":1," 
		   "\""EXT_JSON_KEY_IS_MC"\":1 }";


#define	_SEND_MODE_LOCAL_PARAM		0x01
#define	_SEND_MODE_MEDIA_PARAM		0x02

static int getParamsFrom767(UDP_CLIENT *udpClient)
{
	unsigned int ipAddress = 0;

	extJsonParse(&udpClient->parser, udpClient->buffer, strlen(udpClient->buffer));

	/* IP address in network byte order */
	extJsonParseIpAddress(&udpClient->parser, EXT_JSON_KEY_IP, &ipAddress);
	
	ipAddress = ntohl(ipAddress);

	memset(&udpClient->peerAddress, 0, sizeof(struct sockaddr_in));

	udpClient->peerAddress.sin_family = AF_INET;
	udpClient->peerAddress.sin_port = htons(udpClient->port);
	
	udpClient->peerAddress.sin_addr.s_addr = htonl(ipAddress);

	if(extJsonParseUuid(&udpClient->parser, EXT_JSON_KEY_ID, &udpClient->uuid)<0)
	{
		printf("No '"EXT_JSON_KEY_ID"' in JSON data" );
		return EXIT_FAILURE;
	}

	if(extJsonParseMacAddress(&udpClient->parser, EXT_JSON_KEY_MAC, &udpClient->macAddress) < 0)
	{
		printf("No '"EXT_JSON_KEY_MAC"' in JSON data" );
		return EXIT_FAILURE;
	}

//	printf("Parse peer address: '%s(%08x)' \n\r", inet_ntoa(udpClient->peerAddress.sin_addr), ipAddress);

	return EXIT_SUCCESS;
}


static int broadcastGetParams(UDP_CLIENT *udpClient, const char *mess, int i)
{
	struct sockaddr_in s;
	int broadcastEnable=1;
	
	if (( udpClient->bcSocket = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
	{
		perror("socket");
		return EXIT_FAILURE;
	}

	if(setsockopt(udpClient->bcSocket, SOL_SOCKET, SO_BINDTODEVICE, DEVICE_NAME, strlen(DEVICE_NAME)))
	{
		perror("Bind Interface");
		return EXIT_FAILURE;
	}
#if 0
	struct sockaddr_in saSocket;
	memset(&saSocket, 0, sizeof(saSocket));
	saSocket.sin_family      = AF_INET;
	saSocket.sin_addr.s_addr = htonl(INADDR_ANY);
	saSocket.sin_port        = htons(3600);

	if (bind(sock, (struct sockaddr *) &saSocket, sizeof(saSocket)) < 0)
	{
		perror("bind");
		return -1;
	}
#endif

	memset(&s, '\0', sizeof(struct sockaddr_in));
	s.sin_family = AF_INET;
	s.sin_port = (in_port_t)htons(udpClient->port);

#if 1
	int ret=setsockopt(udpClient->bcSocket, SOL_SOCKET, SO_BROADCAST, &broadcastEnable, sizeof(broadcastEnable));
	s.sin_addr.s_addr = htonl(INADDR_BROADCAST);
#else
	s.sin_addr.s_addr = htonl(INADDR_ANY);
#endif

	struct timeval tv;
	tv.tv_sec = BC_CLIETN_TIMEOUT;
	tv.tv_usec = 100000;
	if (setsockopt(udpClient->bcSocket, SOL_SOCKET, SO_RCVTIMEO,&tv,sizeof(tv)) < 0)
	{
		perror("Error");
		return EXIT_FAILURE;
	}

	printf("\tsend %d bytes BOARDCAST '%.*s' on device %s\n\r\n\r", strlen(mess), strlen(mess), mess, DEVICE_NAME);
	if(sendto(udpClient->bcSocket, mess, strlen(mess), 0, (struct sockaddr *)&s, sizeof(struct sockaddr_in)) < 0)
	{
		perror("sendto");
		return EXIT_FAILURE;
	}


	memset(udpClient->buffer,0 , udpClient->size );
	ret = recvfrom(udpClient->bcSocket, udpClient->buffer, udpClient->size, 0, NULL, NULL);
	
	close(udpClient->bcSocket);

	if(ret < 0)
	{
//		printf("error code :%d"LWIP_NEW_LINE, errno);
		if(errno == EAGAIN)
		{
			printf("\ttimeout in boardcast receive..."LWIP_NEW_LINE LWIP_NEW_LINE LWIP_NEW_LINE );
			return BC_CLIENT_ERROR_TIMEOUT;
		}

		perror("recvfrom from boardcast");
		return EXIT_FAILURE;
	}
	else if(ret == 0)
	{
		printf("ERROR: network broken or buffer size is 0\n\r");
		return EXIT_FAILURE;
	}
 
	udpClient->buffer[ret] = '\0';
	printf("\trecieved BOARDCAST reply %d bytes: '%s'\n\r\n\r", ret, udpClient->buffer);

	return EXIT_SUCCESS;
}



static int unicastSetParams(UDP_CLIENT *udpClient, const char *mess, int i)
{
	int ret =0;
	
	if (( udpClient->socket = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
	{
		perror("socket");
		return EXIT_FAILURE;
	}

#if 0
	if(setsockopt(udpClient->socket, SOL_SOCKET, SO_BINDTODEVICE, DEVICE_NAME, strlen(DEVICE_NAME)))
	{
		perror("Bind Interface");
		return -1;
	}
#endif

	struct timeval tv;
	tv.tv_sec = BC_CLIETN_TIMEOUT;
	tv.tv_usec = 100000;
	if (setsockopt(udpClient->socket, SOL_SOCKET, SO_RCVTIMEO,&tv,sizeof(tv)) < 0)
	{
		perror("Error");
		return EXIT_FAILURE;
	}

	printf("\tsend No.%d %d bytes UNICAST to %s: '%.*s' on device %s\n\r\n\r", i, strlen(mess), inet_ntoa(udpClient->peerAddress.sin_addr), strlen(mess), mess, DEVICE_NAME);
	if(sendto(udpClient->socket, mess, strlen(mess), 0, (struct sockaddr *)&udpClient->peerAddress, sizeof(struct sockaddr_in)) < 0)
	{
		perror("sendto");
		return EXIT_FAILURE;
	}

	memset(udpClient->buffer,0 , udpClient->size);
	ret = recvfrom(udpClient->socket, udpClient->buffer, udpClient->size, 0, NULL, NULL);
	close(udpClient->socket);
	if(ret < 0)
	{
		if(errno == EAGAIN)
		{
			printf("\ttimeout in unicast receive..."LWIP_NEW_LINE LWIP_NEW_LINE LWIP_NEW_LINE);
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

	udpClient->buffer[ret] = '\0';
	printf("\trecieved No.%d UNICAST %d bytes: '%s'\n\r\n\r", i, ret, udpClient->buffer);
	
	return EXIT_SUCCESS;
}

int testErrorBoardcastJsonData(UDP_CLIENT *udpClient)
{
	if(broadcastGetParams(udpClient, jsonGetParamsErrorUser, 0) == EXIT_FAILURE )
	{
		printf("ERROR: send boardcast GetParams packet\n\r");
		return EXIT_FAILURE;
	}

	usleep(50000);
	return 0;
}

int testBoardcastGetParams(UDP_CLIENT *udpClient)
{
	if(broadcastGetParams(udpClient, jsonGetParamsCmd, 0) == EXIT_FAILURE )
	{
		printf("ERROR: send boardcast GetParams packet\n\r");
		return EXIT_FAILURE;
	}
	
	return EXIT_SUCCESS;
}

char	testJsonSetLocalParamsCmdPrintConfig(char  *buffer, unsigned int size, EXT_RUNTIME_CFG *cfg, EXT_UUID_T *uuid)
{
	int index = 0;
		
	index += snprintf(buffer+index, size-index, "{\""EXT_JSON_KEY_COMMAND"\":\"%s\",", EXT_JSON_CMD_SET_PARAMS );

	index += snprintf(buffer+index, size-index, "\""EXT_JSON_KEY_NAME"\":\"%s\",", cfg->name );
	index += snprintf(buffer+index, size-index, "\""EXT_JSON_KEY_MODEL"\":\"%s-%s\",", cfg->model, EXT_IS_TX(cfg)?"TX":"RX" );
	index += snprintf(buffer+index, size-index, "\""EXT_JSON_KEY_VERSION"\":\"%02d.%02d.%02d\",", cfg->version.major, cfg->version.minor, cfg->version.revision);

	index += snprintf(buffer+index, size-index, "\""EXT_JSON_KEY_MAC"\":" );
	MAC_ADDRESS_PRINT(buffer, size, index, &(cfg->macAddress));
	
	index += snprintf(buffer+index, size-index, "\""EXT_JSON_KEY_IP"\":\"%s\",", EXT_LWIP_IPADD_TO_STR(&(cfg->ipAddress)) );
	index += snprintf(buffer+index, size-index, "\""EXT_JSON_KEY_MASK"\":\"%s\",", EXT_LWIP_IPADD_TO_STR(&(cfg->ipMask)) );
	index += snprintf(buffer+index, size-index, "\""EXT_JSON_KEY_GATEWAY"\":\"%s\",", EXT_LWIP_IPADD_TO_STR(&(cfg->ipGateway)) );

	index += snprintf(buffer+index, size-index, "\""EXT_JSON_KEY_DHCP"\":%d,", EXT_DHCP_IS_ENABLE(cfg)?1:0);
	index += snprintf(buffer+index, size-index, "\""EXT_JSON_KEY_TX"\":%d}", EXT_IS_TX(cfg)?1:0);

//	parser->outIndex = index;
//	printf("output response %d bytes: '%s'"LWIP_NEW_LINE, parser->outIndex, parser->outBuffer);
	return index;
}


char	testJsonSetMediaParamsCmdPrintConfig(char  *buffer, unsigned int size, EXT_RUNTIME_CFG *cfg, EXT_UUID_T *uuid)
{
	int index = 0;
		
	index += snprintf(buffer+index, size-index, "{\""EXT_JSON_KEY_COMMAND"\":\"%s\",", EXT_JSON_CMD_SET_PARAMS );

	index += snprintf(buffer+index, size-index, "\""EXT_JSON_KEY_NAME"\":\"%s\",", cfg->name );
	index += snprintf(buffer+index, size-index, "\""EXT_JSON_KEY_MODEL"\":\"%s-%s\",", cfg->model, EXT_IS_TX(cfg)?"TX":"RX" );
	index += snprintf(buffer+index, size-index, "\""EXT_JSON_KEY_VERSION"\":\"%02d.%02d.%02d\",", cfg->version.major, cfg->version.minor, cfg->version.revision);

	index += snprintf(buffer+index, size-index, "\""EXT_JSON_KEY_VIDEO_MAC_DEST"\":" );
	MAC_ADDRESS_PRINT(buffer, size, index, &(cfg->dest.mac));

	index += snprintf(buffer+index, size-index, "\""EXT_JSON_KEY_VIDEO_IP_DEST"\":\"%s\",", EXT_LWIP_IPADD_TO_STR(&(cfg->dest.ip)) );

	index += snprintf(buffer+index, size-index, "\""EXT_JSON_KEY_VIDEO_PORT_DEST"\":%d,", cfg->dest.vport);

	index += snprintf(buffer+index, size-index, "\""EXT_JSON_KEY_AUDIO_PORT_DEST"\":%d}", cfg->dest.aport);

//	parser->outIndex = index;
//	printf("output response %d bytes: '%s'"LWIP_NEW_LINE, parser->outIndex, parser->outBuffer);
	return index;
}

int testUnicastSetLocalParams(UDP_CLIENT *udpClient, int i)
{
	memset(udpClient->buffer, 0, udpClient->size);

#if 0
	int index = 0;
	int ret = 0;
	index += snprintf(udpClient->buffer, udpClient->size, "%s",jsonRequestSetParams);
	index += snprintf(udpClient->buffer+index, udpClient->size-index, "\""EXT_JSON_KEY_ID"\":");
	UUID_PRINT(udpClient->buffer, udpClient->size, index, &udpClient->uuid );

	index += snprintf(udpClient->buffer+index, udpClient->size-index, "\""EXT_JSON_KEY_MAC"\":");
	MAC_ADDRESS_PRINT(udpClient->buffer, udpClient->size, index, &udpClient->macAddress);

	index += snprintf(udpClient->buffer+index, udpClient->size-index, "}");
#else
	testJsonSetLocalParamsCmdPrintConfig(udpClient->buffer, udpClient->size, udpClient->clientCfg, &udpClient->uuid);
#endif
	
	return unicastSetParams(udpClient, udpClient->buffer, i);
}

int testUnicastSetMediaParams(UDP_CLIENT *udpClient, int i)
{
	memset(udpClient->buffer, 0, udpClient->size);

#if 0
	int index = 0;
	int ret = 0;
	index += snprintf(udpClient->buffer, udpClient->size, "%s",jsonRequestSetParams);
	index += snprintf(udpClient->buffer+index, udpClient->size-index, "\""EXT_JSON_KEY_ID"\":");
	UUID_PRINT(udpClient->buffer, udpClient->size, index, &udpClient->uuid );

	index += snprintf(udpClient->buffer+index, udpClient->size-index, "\""EXT_JSON_KEY_MAC"\":");
	MAC_ADDRESS_PRINT(udpClient->buffer, udpClient->size, index, &udpClient->macAddress);

	index += snprintf(udpClient->buffer+index, udpClient->size-index, "}");
#else
	testJsonSetMediaParamsCmdPrintConfig(udpClient->buffer, udpClient->size, udpClient->clientCfg, &udpClient->uuid);
#endif
	
	return unicastSetParams(udpClient, udpClient->buffer, i);
}


int testClientParseConfig(UDP_CLIENT *udpClient)
{
//	int index = 0;
	char	ret;
	EXT_JSON_PARSER rawParser;

	printf("parse local params..."LWIP_NEW_LINE);
	ret = extJsonRequestParseCommand((char *)jsonRequestSetParams, strlen(jsonRequestSetParams), &rawParser);
	if(ret == EXIT_FAILURE)
	{
		printf("Parse raw SetParams failed"LWIP_NEW_LINE);
		return EXIT_FAILURE;
	}

	ret = extJsonRequestParse(&rawParser, udpClient->clientCfg);
	if(ret == EXIT_FAILURE)
	{
		printf("Parse raw cfg of SetParams failed"LWIP_NEW_LINE);
		return EXIT_FAILURE;
	}

	sprintf(udpClient->clientCfg->name, "%s", EXT_767_PRODUCT_NAME);
	sprintf(udpClient->clientCfg->model, "%s", EXT_767_MODEL);

	udpClient->clientCfg->version.major = BL_VERSION_MAJOR;
	udpClient->clientCfg->version.minor = BL_VERSION_MINOR;
	udpClient->clientCfg->version.revision = BL_VERSION_REVISION;

	snprintf(udpClient->clientCfg->user, EXT_USER_SIZE, "%s", EXT_USER);
	snprintf(udpClient->clientCfg->password, EXT_PASSWORD_SIZE, "%s", EXT_PASSWORD);

	return EXIT_SUCCESS;
}


int testCompareSetParams(UDP_CLIENT *udpClient, int i, int mode)
{
//	int index = 0;
	char	ret;
	EXT_JSON_PARSER replyParser;
	EXT_RUNTIME_CFG replyCfg;

	memset(&replyCfg, 0, sizeof(EXT_RUNTIME_CFG));

//	printf("\tparse reply params..."LWIP_NEW_LINE);
	ret = extJsonRequestParseCommand(udpClient->buffer, strlen(udpClient->buffer), &replyParser);
	if(ret == EXIT_FAILURE)
	{
		printf("Parse reply SetParams failed"LWIP_NEW_LINE);
		return EXIT_FAILURE;
	}
	ret = extJsonRequestParse(&replyParser, &replyCfg);
	if(ret == EXIT_FAILURE)
	{
		printf("Parse reply cfg of SetParams failed"LWIP_NEW_LINE);
		return EXIT_FAILURE;
	}

//	extJsonDebug( &rawParser, &rawCfg);
//	extJsonDebug( &replyParser, &replyCfg);

	if( (mode &0x01) )
	{
		ret = extJsonCompareLocalCfg(udpClient->clientCfg, &replyCfg);
		if(ret == EXIT_FAILURE)
			return EXIT_FAILURE;
	}	

	if( (mode & 0x02))
	{
		ret = extJsonComparePeerMediaCfg(udpClient->clientCfg, &replyCfg);
		if(ret == EXIT_FAILURE)
			return EXIT_FAILURE;
	}	
	
	return EXIT_SUCCESS;
}


int main(int argc, char *argv[])
{
	EXT_RUNTIME_CFG _clientCfg;

	UDP_CLIENT client;
	UDP_CLIENT *udpClient = &client;
	int i= 1;
	int count = 0;
	int	ret = 0;

	memset(udpClient, 0, sizeof(UDP_CLIENT));
	memset(&_clientCfg, 0 , sizeof(EXT_RUNTIME_CFG));
	udpClient->port = EXT_CTRL_PORT;
	udpClient->size = sizeof(udpClient->buffer);
	udpClient->clientCfg = &_clientCfg;

	if( testClientParseConfig(udpClient)!= EXIT_SUCCESS)
	{
		return EXIT_FAILURE;
	}


	/* test cases of error JSON data */
	printf("Test Case: error request..."LWIP_NEW_LINE);
	if(testErrorBoardcastJsonData(udpClient)== EXIT_FAILURE)
		return EXIT_FAILURE;

	/* test case of configuring flow */
	while(1)
	{
		int getCount = 1;

		
		printf(LWIP_NEW_LINE LWIP_NEW_LINE"***************************************"LWIP_NEW_LINE"Test Case: No.%d Configuring flow..."LWIP_NEW_LINE, ++count);
		do
		{
			if(getCount != 1)
			{
				usleep(BC_CLIETN_DELAY);
			}
			printf(LWIP_NEW_LINE"\t********************"LWIP_NEW_LINE"\tNo.%d broadcast GetParams message"LWIP_NEW_LINE, getCount++);
			ret = broadcastGetParams(udpClient, jsonGetParamsCmd, 0);
		}while(ret == BC_CLIENT_ERROR_TIMEOUT);

		if(ret == EXIT_FAILURE)
		{
			return ret;
		}

		if( testCompareSetParams(udpClient, i, (_SEND_MODE_LOCAL_PARAM+_SEND_MODE_MEDIA_PARAM) ) == EXIT_SUCCESS)
		{
			printf("\t'GetParams' return is same as local configuration, continue to next test step..."LWIP_NEW_LINE);
//			usleep(BC_CLIETN_DELAY);
//			continue;
		}
		else
		{
			printf("\t'GetParams' is not same as local configuration, continue to 'SetParams'..."LWIP_NEW_LINE);

		}

			if( getParamsFrom767( udpClient) == EXIT_FAILURE )
			{
				return EXIT_FAILURE;
			}
			
		int setCount = 1;

		do
		{
			printf(LWIP_NEW_LINE"\t********************"LWIP_NEW_LINE"\tNo.%d unicast 'SetParams(Local)' message"LWIP_NEW_LINE, setCount++);
			ret = testUnicastSetLocalParams(udpClient, i);
		}while(0);
		
		if(ret == BC_CLIENT_ERROR_TIMEOUT)
		{
			continue;
		}
		else if(ret == EXIT_FAILURE)
		{
			return ret;
		}

		if( testCompareSetParams(udpClient, i,  _SEND_MODE_LOCAL_PARAM) == EXIT_SUCCESS)
		{
			printf("\t'SetParams(Local)' is same as configuration, continue to next step..."LWIP_NEW_LINE);
//			usleep(BC_CLIETN_DELAY);
		}
		else
		{
			printf("\t'SetParams(Local) return is not same as configuration, configuration failed!!!"LWIP_NEW_LINE);
			return EXIT_FAILURE;
			continue;
		}


		do
		{
			printf(LWIP_NEW_LINE"\t********************"LWIP_NEW_LINE"\tNo.%d unicast 'SetParams(media only)' message"LWIP_NEW_LINE, setCount);
			ret = testUnicastSetMediaParams(udpClient, i);
		}while(0);
		
		if(ret == BC_CLIENT_ERROR_TIMEOUT)
		{
			continue;
		}
		else if(ret == EXIT_FAILURE)
		{
			return ret;
		}

		if( testCompareSetParams(udpClient, i, _SEND_MODE_MEDIA_PARAM) == EXIT_SUCCESS)
		{
			printf("\t'SetParams(media only)' is same as configuration, continue to next test..."LWIP_NEW_LINE);
			usleep(BC_CLIETN_DELAY);
			continue;
		}
		else
		{
			printf("\t'SetParams(media only) return is not same as configuration, configuration failed!!!"LWIP_NEW_LINE);
			return EXIT_FAILURE;
			continue;
		}

		i++;
	}

	
#if 0
	while(1)
	{
		if(testUnicastSetParams(udpClient, i)== EXIT_FAILURE)
			return EXIT_FAILURE;

		if(i>100)
			break;
		i++;
	}	

	while(1)
	{

		if(unicastSetParams(udpClient, jsonRequestNoCommand, i) < 0 )
		{
			printf("ERROR: send unicast SetParams packet\n\r");
			return -1;
		}

		i++;

		usleep(50000);
		break;
	}
#endif

	return EXIT_SUCCESS;
}

