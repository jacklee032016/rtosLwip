
/*
 * sender.c -- multicasts "hello, world!" to a multicast group once a second
 * Antony Courtney,	25/11/94
 */

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>	/* sleep/close() */


#include "lwipMux.h"

#include		"../lwipTestClient.h"

int main(int argc, char *argv[])
{
	struct sockaddr_in addr;
	int _socket;
	int	cnt = 0;
//	struct ip_mreq mreq;
	char *message="Hello, World!";
	int ret = 0;

	/* create what looks like an ordinary UDP socket */
	if ((_socket=socket(AF_INET,SOCK_DGRAM,0)) < 0)
	{
		perror("socket");
		exit(1);
	}

	if(setsockopt(_socket, SOL_SOCKET, SO_BINDTODEVICE, DEVICE_NAME, strlen(DEVICE_NAME)))
	{
		perror("Bind Interface");
		return EXIT_FAILURE;
	}

	/* set up destination address */
	memset(&addr,0,sizeof(addr));
	addr.sin_family=AF_INET;
	addr.sin_addr.s_addr=inet_addr(LWIP_TEST_GROUP_ADDR);
	addr.sin_port=htons(LWIP_TEST_GROUP_PORT);

	/* now just sendto() our destination! */
	while (1)
	{
		
		printf("Send No.%d message of '%s' on %s:%d(%s)...\r\n", ++cnt, message, LWIP_TEST_GROUP_ADDR, LWIP_TEST_GROUP_PORT, DEVICE_NAME);
		ret = sendto(_socket, message, strlen(message),0,(struct sockaddr *) &addr, sizeof(addr));
		if (ret< 0)
		{
			perror("sendto");
			close(_socket);
			exit(1);
		}
		sleep(1);
	}

	close(_socket);

	return 0;
}

