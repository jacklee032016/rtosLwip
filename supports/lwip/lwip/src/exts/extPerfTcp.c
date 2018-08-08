/*
 * Copyright (c) 2007 Xilinx, Inc.  All rights reserved.
 *
 * Xilinx, Inc.
 * XILINX IS PROVIDING THIS DESIGN, CODE, OR INFORMATION "AS IS" AS A
 * COURTESY TO YOU.  BY PROVIDING THIS DESIGN, CODE, OR INFORMATION AS
 * ONE POSSIBLE   IMPLEMENTATION OF THIS FEATURE, APPLICATION OR
 * STANDARD, XILINX IS MAKING NO REPRESENTATION THAT THIS IMPLEMENTATION
 * IS FREE FROM ANY CLAIMS OF INFRINGEMENT, AND YOU ARE RESPONSIBLE
 * FOR OBTAINING ANY RIGHTS YOU MAY REQUIRE FOR YOUR IMPLEMENTATION.
 * XILINX EXPRESSLY DISCLAIMS ANY WARRANTY WHATSOEVER WITH RESPECT TO
 * THE ADEQUACY OF THE IMPLEMENTATION, INCLUDING BUT NOT LIMITED TO
 * ANY WARRANTIES OR REPRESENTATIONS THAT THIS IMPLEMENTATION IS FREE
 * FROM CLAIMS OF INFRINGEMENT, IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS FOR A PARTICULAR PURPOSE.
 *
 */

#include <stdio.h>
#include <string.h>

#include "lwip/err.h"
#include "lwip/tcp.h"
#include "lwip/stats.h"
#include "xil_printf.h"

static unsigned rxperf_port = 5001;	/* iperf default port */
static unsigned rxperf_server_running = 0;

int
transfer_rxperf_data() {
    return 0;
}

static err_t
rxperf_recv_callback(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err)
{
    /* close socket if the peer has sent the FIN packet  */
    if (p == NULL) {
        tcp_close(tpcb);
        return ERR_OK;
    }

    /* all we do is say we've received the packet */
    /* we don't actually make use of it */
    tcp_recved(tpcb, p->tot_len);

    pbuf_free(p);
    return ERR_OK;
}

err_t
rxperf_accept_callback(void *arg, struct tcp_pcb *newpcb, err_t err)
{
    xil_printf("rxperf: Connection Accepted\r\n");
    tcp_recv(newpcb, rxperf_recv_callback);

    return ERR_OK;
}

int
start_rxperf_application()
{
    struct tcp_pcb *pcb;
    err_t err;

    /* create new TCP PCB structure */
    pcb = tcp_new();
    if (!pcb) {
    	xil_printf("rxperf: Error creating PCB. Out of Memory\r\n");
    	return -1;
    }

    /* bind to iperf @port */
    err = tcp_bind(pcb, IP_ADDR_ANY, rxperf_port);
    if (err != ERR_OK) {
    	xil_printf("rxperf: Unable to bind to port %d: err = %d\r\n", rxperf_port, err);
    	return -2;
    }

    /* we do not need any arguments to callback functions :) */
    tcp_arg(pcb, NULL);

    /* listen for connections */
    pcb = tcp_listen(pcb);
    if (!pcb) {
    	xil_printf("rxperf: Out of memory while tcp_listen\r\n");
    	return -3;
    }

    /* specify callback to use for incoming connections */
    tcp_accept(pcb, rxperf_accept_callback);

    rxperf_server_running = 1;

    return 0;
}

void
print_rxperf_app_header()
{
    xil_printf("%20s %6d %s\r\n", "rxperf server",
                        rxperf_port,
                        "$ iperf -c <board ip> -i 5 -t 100");
}


#include <stdio.h>
#include <string.h>

#include "lwip/err.h"
#include "lwip/tcp.h"
#include "lwipopts.h"
#include "lwip/stats.h"
#ifndef __PPC__
#include "xil_printf.h"
#endif

#if (USE_JUMBO_FRAMES==1)
#define SEND_BUFSIZE (9000)
#else
#define SEND_BUFSIZE (1400)
#endif
static struct tcp_pcb *connected_pcb = NULL;
volatile extern int TxPerfConnMonCntr;
static char send_buf[SEND_BUFSIZE];

static unsigned txperf_client_connected = 0;
int
transfer_txperf_data()
{
#if __arm__
	int copy = 3;
#else
	int copy = 0;
#endif
	err_t err;
	struct tcp_pcb *tpcb = connected_pcb;

	if (!connected_pcb)
		return ERR_OK;

	while (tcp_sndbuf(tpcb) > SEND_BUFSIZE) {
		err = tcp_write(tpcb, send_buf, SEND_BUFSIZE, copy);
		if (err != ERR_OK) {
			xil_printf("txperf: Error on tcp_write: %d\r\n", err);
			connected_pcb = NULL;
			return -1;
		}
		err = tcp_output(tpcb);
		if (err != ERR_OK) {
			xil_printf("txperf: Error on tcp_output: %d\r\n",err);
		}
	}

	if (TxPerfConnMonCntr == 20) {
		err = tcp_write(tpcb, send_buf, tcp_sndbuf(tpcb), copy);
		if (err != ERR_OK) {
			xil_printf("txperf: Error on tcp_write: %d\r\n", err);
			connected_pcb = NULL;
		}
		err = tcp_output(tpcb);
		if (err != ERR_OK) {
			xil_printf("txperf: Error on tcp_output: %d\r\n",err);
		}
	}

	return 0;
}

static err_t
txperf_sent_callback(void *arg, struct tcp_pcb *tpcb, u16_t len)
{
#if __arm__
	int copy = 3;
#else
	int copy = 0;
#endif
	err_t err;

	TxPerfConnMonCntr = 0;

	while (tcp_sndbuf(tpcb) > SEND_BUFSIZE) {
		err = tcp_write(tpcb, send_buf, SEND_BUFSIZE, copy);
		if (err != ERR_OK) {
			xil_printf("txperf: Error on tcp_write: %d\r\n", err);
			connected_pcb = NULL;
			return -1;
		}
		err = tcp_output(tpcb);
		if (err != ERR_OK) {
			xil_printf("txperf: Error on tcp_output: %d\r\n",err);
		}
	}

	return ERR_OK;
}

static err_t
txperf_connected_callback(void *arg, struct tcp_pcb *tpcb, err_t err)
{
	xil_printf("txperf: Connected to iperf server\r\n");
    txperf_client_connected = 1;

	/* store state */
	connected_pcb = tpcb;

	/* set callback values & functions */
	tcp_arg(tpcb, NULL);
	tcp_sent(tpcb, txperf_sent_callback);

	/* initiate data transfer */
	return ERR_OK;
}

int
start_txperf_application()
{
	struct tcp_pcb *pcb;
	struct ip_addr ipaddr;
	err_t err;
	u16_t port;
	int i;

	/* create new TCP PCB structure */
	pcb = tcp_new();
	if (!pcb) {
		xil_printf("txperf: Error creating PCB. Out of Memory\r\n");
		return -1;
	}

	/* connect to iperf server */
	IP4_ADDR(&ipaddr,  192, 168, 1, 9);		/* iperf server address */

	port = 5001;					/* iperf default port */
	err = tcp_connect(pcb, &ipaddr, port, txperf_connected_callback);
        txperf_client_connected = 0;

	if (err != ERR_OK) {
		xil_printf("txperf: tcp_connect returned error: %d\r\n", err);
		return err;
	}

	/* initialize data buffer being sent */
	for (i = 0; i < SEND_BUFSIZE; i++)
		send_buf[i] = (i % 10) + '0';

	return 0;
}

void
print_txperf_app_header()
{
        xil_printf("%20s %6s %s\r\n", "txperf client",
                        "N/A",
                        "$ iperf -s -i 5 -w 64k (on host with IP 192.168.1.9)");
}


