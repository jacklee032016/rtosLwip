/*
 * 
 */

#include "lwip/apps/lwiperf.h"

#include <stdio.h>
#include <string.h>

#include "lwip/err.h"
#include "lwip/udp.h"

#include "lwipMux.h"

#if LWIP_EXT_UDP_RX_PERF

static unsigned rxperf_server_running = 0;

#if 0
int transfer_urxperf_data()
{
	return 0;
}
#endif

static void _udpRxPerfRecv(void *arg, struct udp_pcb *tpcb, struct pbuf *p, struct ip_addr *addr, u16_t port)
{
	static int first = 1;
	static int expected_id = 0;
	static int n_dropped = 0;
	int recv_id;

	/* first, see if the datagram is received in order */
	recv_id =  *((int *)(p->payload));

	if (first)
	{
		expected_id = recv_id;
		first = 0;
	}
	else if (expected_id != recv_id)
	{
		n_dropped += (recv_id - expected_id);
		expected_id = recv_id;
	}
	print(".");

	expected_id++;
	
	pbuf_free(p);

	first = 0;
}

char extUdpRxPerfStart(void)
{
	struct udp_pcb *pcb;
	err_t err;

	/* create new TCP PCB structure */
	pcb = udp_new();
	if (!pcb)
	{
		MUX_ERRORF("OOM when creating PCB for UDP RX Perf"MUX_NEW_LINE);
		return EXIT_FAILURE;
	}

	/* bind to iperf @port */
	err = udp_bind(pcb, IP_ADDR_ANY, LWIPERF_TCP_PORT_DEFAULT);
	if (err != ERR_OK)
	{
		MUX_ERRORF(("Unable to bind to UDP Perf port: err = %d"MUX_NEW_LINE, err));
		return EXIT_FAILURE;
	}

	udp_recv(pcb, _udpRxPerfRecv, NULL);

 //     rxperf_server_running = 1;

	return EXIT_SUCCESS;
}

#if 0
void print_urxperf_app_header()
{
    xil_printf("%20s %6d %10s %s\r\n", "rxperf server",
                        rxperf_port,
                        rxperf_server_running ? "RUNNING" : "INACTIVE",
                        "$ iperf -c <board ip> -i 5 -t 100 -u -b <bandwidth>");
}
#endif


#endif

#if LWIP_EXT_UDP_TX_PERF


static unsigned utxperf_port = 5001;
static unsigned txperf_client_connected = 0;

static struct udp_pcb *connected_pcb = NULL;
static struct pbuf *pbuf_to_be_sent = NULL;

#define SEND_BUFSIZE (1450)
static char send_buf[SEND_BUFSIZE];

int
transfer_utxperf_data()
{
	err_t err;
	struct udp_pcb *pcb = connected_pcb;
	static signed int id = 0;
	int *payload;

	pbuf_to_be_sent = pbuf_alloc(PBUF_TRANSPORT, 1450, PBUF_POOL);
	if (!pbuf_to_be_sent) {
		xil_printf("error allocating pbuf to send\r\n");
		return 0;
	}
	else {
		memcpy(pbuf_to_be_sent->payload, send_buf, 1300);
	}

	/* always increment the id */
	payload = (int*)(pbuf_to_be_sent->payload);
	if (id == 0x7FFFFFFE )
		id = 0;
	id++;
	payload[0] = htonl(id);

	err = udp_send(pcb, pbuf_to_be_sent);
	if (err != ERR_OK) {
		xil_printf("Error on udp_send: %d\r\n", err);
		pbuf_free(pbuf_to_be_sent);
		return -1;
	}
	pbuf_free(pbuf_to_be_sent);
	return 0;
}

int
start_utxperf_application()
{
	struct udp_pcb *pcb;
	struct ip_addr ipaddr;
	err_t err;
	u16_t port;
	int i;

	/* create a udp socket */
	pcb = udp_new();
	if (!pcb) {
		xil_printf("Error creating PCB. Out of Memory\r\n");
		return -1;
	}

	/* bind local address */
	if ((err = udp_bind(pcb, IP_ADDR_ANY, 0)) != ERR_OK) {
		xil_printf("error on udp_bind: %x\n\r", err);
	}

	/* connect to iperf server */
	IP4_ADDR(&ipaddr,  192, 168,  1, 9);		/* iperf server address */
	port = utxperf_port;                            /* iperf default port */
	err = udp_connect(pcb, &ipaddr, port);
	if (err != ERR_OK)
		xil_printf("error on udp_connect: %x\n\r", err);

	/* initialize data buffer being sent */
	for (i = 0; i < SEND_BUFSIZE; i++)
		send_buf[i] = (i % 10) + '0';

	connected_pcb = pcb;
	return 0;
}

void print_utxperf_app_header()
{
        xil_printf("%20s %6s %10s %s\r\n", "utxperf client",
                        "N/A",
                        txperf_client_connected ? "CONNECTED" : "CONNECTING",
                        "$ iperf -u -s -i 5 (on host with IP 192.168.1.9)");
}

#endif

