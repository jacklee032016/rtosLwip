/*
 * listener.c -- joins a multicast group and echoes all data it receives from the group to its stdout...
 * to compile without warning and work correctly
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

#define	MSGBUFSIZE			256

int main(int argc, char *argv[])
{
	struct sockaddr_in addr;
	int	_socket, nbytes;
	unsigned int	addrlen;
	struct ip_mreq mreq;
	char msgbuf[MSGBUFSIZE];
	int count = 0;

	u_int yes=1;            /*** MODIFICATION TO ORIGINAL */

	/* create what looks like an ordinary UDP socket */
	if (( _socket=socket(AF_INET,SOCK_DGRAM,0)) < 0)
	{
		perror("socket");
		exit(1);
	}


	/**** MODIFICATION TO ORIGINAL */
	/* allow multiple sockets to use the same PORT number */
	if (setsockopt(_socket,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof(yes)) < 0)
	{
		perror("Reusing ADDR failed");
		close(_socket);
		exit(1);
	}
	/*** END OF MODIFICATION TO ORIGINAL */

	/* set up destination address */
	memset(&addr,0,sizeof(addr));
	addr.sin_family=AF_INET;
	addr.sin_addr.s_addr=htonl(INADDR_ANY); /* N.B.: differs from sender */
	addr.sin_port=htons(LWIP_TEST_GROUP_PORT);

	/* bind to receive address */
	if (bind(_socket, (struct sockaddr *) &addr,sizeof(addr)) < 0)
	{
		perror("bind");
		close(_socket);
		exit(1);
	}

	/* use setsockopt() to request that the kernel join a multicast group */
	mreq.imr_multiaddr.s_addr=inet_addr(LWIP_TEST_GROUP_ADDR);
	mreq.imr_interface.s_addr=htonl(INADDR_ANY);
	if (setsockopt(_socket,IPPROTO_IP,IP_ADD_MEMBERSHIP,&mreq,sizeof(mreq)) < 0)
	{
		perror("setsockopt");
		close(_socket);
		exit(1);
	}

	/* now just enter a read-print loop */
	while (1)
	{
		addrlen=sizeof(addr);
		if ((nbytes=recvfrom(_socket, msgbuf, MSGBUFSIZE,0, (struct sockaddr *) &addr, &addrlen)) < 0)
		{
			perror("recvfrom");
			close(_socket);
			exit(1);
		}
		msgbuf[nbytes] = 0;
		printf("Listened No.%d message with length %d :'%s' on %s:%d...\r\n", ++count, nbytes, msgbuf, LWIP_TEST_GROUP_ADDR, LWIP_TEST_GROUP_PORT);

//		puts(msgbuf);
	}

	return 0;
}

