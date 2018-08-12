/*
 * 
 */

#include "lwip/opt.h"
#include "lwip/apps/lwiperf.h"

#include <stdio.h>
#include <string.h>

#include "lwip/err.h"
#include "lwip/udp.h"
#include "lwip/ip_addr.h"
#include "lwip/timeouts.h"

#include "lwipExt.h"

#if LWIP_EXT_UDP_RX_PERF

#if 0
static unsigned rxperf_server_running = 0;

int transfer_urxperf_data()
{
	return 0;
}
#endif

struct ExtUdpRxPerfStats
{
	unsigned int firstId;
	unsigned int recvId;

	unsigned int recved;
	unsigned int dropped;
	unsigned int traffic;
	
	unsigned int startTime;
};

struct ExtUdpRxPerfStats _stats;


#define UDP_RX_PERF_TIMER_MSECS      100

static void _udpRxTimer(void* arg)
{
	struct ExtUdpRxPerfStats *_st = (struct ExtUdpRxPerfStats *)arg;
	unsigned int endTime = sys_now();

	MUX_DEBUGF(MUX_DBG_ON, ("UdpRxPerf don't receive packets within %u ms", UDP_RX_PERF_TIMER_MSECS));
	MUX_DEBUGF(MUX_DBG_ON, ("RECV: %u, Drop: %u", _st->recved, _st->recvId-_st->firstId-_st->recved));
	MUX_DEBUGF(MUX_DBG_ON, ("Time: %u ms, Traffic: %u", endTime-_st->startTime-UDP_RX_PERF_TIMER_MSECS, _st->traffic));

	memset(_st, 0 , sizeof(struct ExtUdpRxPerfStats));
}


static void _udpRxPerfRecv(void *arg, struct udp_pcb *tpcb, struct pbuf *p, const ip_addr_t *addr, u16_t port)
{
	unsigned int recv_id;

	struct ExtUdpRxPerfStats *_st = (struct ExtUdpRxPerfStats *)arg;
	
	/* first, see if the datagram is received in order */
	recv_id =  lwip_htonl(*(int *)(p->payload));
	MUX_ASSERT(("Pbuf is null"), (p!=NULL));
	
	if (_st->recved == 0)
	{
		_st->firstId = (recv_id);
		_st->recvId = _st->firstId;
		_st->startTime = sys_now();
	}
	else
	{
		sys_untimeout(_udpRxTimer, _st);
		if(recv_id != _st->recvId +1 )
		{
//			MUX_INFOF(("Missed %d datagrams", recv_id-_st->recvId));
			_st->dropped += recv_id-_st->recvId;
		}
		_st->recvId = recv_id;
	}
	
	_st->recved++;
//	_st->recvId = recv_id;
	_st->traffic += p->tot_len;
//	MUX_DEBUGF(MUX_DBG_ON, ("RECV: %u, firstId: %u, recvId: %u, time: %u", _st->recved, _st->firstId, _st->recvId, (unsigned int)(sys_now()-_st->startTime)));
	printf(".");

#if 0//MUX_LWIP_DEBUG
	MUX_LWIP_DEBUG_PBUF(p);
	if(p->next)
	{
		MUX_LWIP_DEBUG_PBUF(p->next);
	}
#endif

	pbuf_free(p);
	sys_timeout(UDP_RX_PERF_TIMER_MSECS, _udpRxTimer, _st);
}

char extUdpRxPerfStart(void)
{
	struct udp_pcb *pcb;
	err_t err;

	/* create new TCP PCB structure */
	pcb = udp_new();
	if (!pcb)
	{
		MUX_ERRORF(("OOM when creating PCB for UDP RX Perf"MUX_NEW_LINE));
		return EXIT_FAILURE;
	}

	/* bind to iperf @port */
	err = udp_bind(pcb, IP_ADDR_ANY, LWIPERF_TCP_PORT_DEFAULT);
	if (err != ERR_OK)
	{
		MUX_ERRORF(("Unable to bind to UDP Perf port: err = %d"MUX_NEW_LINE, err));
		return EXIT_FAILURE;
	}

	memset(&_stats, 0 , sizeof(struct ExtUdpRxPerfStats));
	MUX_DEBUGF(MUX_DBG_ON, ("RECV: %u, Drop: %u", _stats.recved, _stats.recvId-_stats.firstId-_stats.recved));
	udp_recv(pcb, _udpRxPerfRecv, &_stats);

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


#define SEND_BUFSIZE (1450)
static char send_buf[SEND_BUFSIZE];

sys_sem_t	_udpTxSem;

struct ExtUdpTxTask
{
	sys_sem_t			sema;
	struct udp_pcb		*pcb;
	struct pbuf			*sendBuf;

	unsigned int			sendCount;
	unsigned int			oomCount;
	unsigned int			failedCount;

	unsigned char			isStop;
};

static struct ExtUdpTxTask _udpTxTask;


#define UDP_TX_PERF_TIMER_MSECS      10*1000

static void _udpTxTimer(void* arg)
{
	 struct ExtUdpTxTask 	*_txTask = (struct ExtUdpTxTask *)arg;

	_txTask->isStop = 1;

	MUX_DEBUGF(MUX_DBG_ON, ("UdpTxTask timeout"));

}

static void _udpTxPerTask(void *arg)
{
	err_t err;
	struct ExtUdpTxTask *_txTask = &_udpTxTask;
	int count = 0;
	
	static signed int id = 0;
	int *payload;

	while(1)
	{
		sys_arch_sem_wait(&_txTask->sema, 0);
		_txTask->oomCount = 0;
		_txTask->sendCount = 0;
		_txTask->failedCount = 0;
		_txTask->isStop = 0;

		sys_timeout(UDP_TX_PERF_TIMER_MSECS, _udpTxTimer, _txTask);

		MUX_DEBUGF(MUX_DBG_ON, ("UdpTxTask start..."));
		while (1)//count < 1000*10)
		{
			_txTask->sendBuf = pbuf_alloc(PBUF_TRANSPORT, 1450, PBUF_POOL);
			if (!_txTask->sendBuf)
			{
				MUX_ERRORF(("error allocating pbuf to send"MUX_NEW_LINE));
				_txTask->oomCount++;
				break;
			}
			else
			{
				memcpy(_txTask->sendBuf->payload, _txTask->sendBuf, 1300);
			}

			/* always increment the id */
			payload = (int*)(_txTask->sendBuf->payload);
			if (id == 0x7FFFFFFE )
				id = 0;
			id++;
			payload[0] = htonl(id);

			err = udp_send(_txTask->pcb, _txTask->sendBuf);
			if (err != ERR_OK)
			{
//				MUX_ERRORF(("Error on udp_send packet %d: %d"MUX_NEW_LINE, id, err));
				_txTask->failedCount++;
	//			pbuf_free(pbuf_to_be_sent);
	//			return EXIT_FAILURE;
				printf(ERROR_TEXT_BEGIN"*"ERROR_TEXT_END);
			}
			else 
			{
				printf(".");
				_txTask->sendCount++;
			}
			count++;
			
			pbuf_free(_txTask->sendBuf);

			if(_txTask->isStop )
			{
				printf("Ended!"MUX_NEW_LINE);
				break;
			}
		}
		
		printf(MUX_NEW_LINE);

		MUX_DEBUGF(MUX_DBG_ON, ("UdpTxTask Stop, Send %d; failed:%d; OOM: %d", _txTask->sendCount, _txTask->failedCount, _txTask->oomCount));

		udp_disconnect(_txTask->pcb);
		
	}
	
	return;
}


/* called by command line */
char extUdpTxPerfStart(unsigned int svrIpAddress)
{
	ip_addr_t ipaddr;
	err_t err;
	struct ExtUdpTxTask  *_txTask = &_udpTxTask;

	/* connect to iperf server */
	ipaddr.addr = svrIpAddress;

	err = udp_connect(_txTask->pcb, &ipaddr, LWIPERF_TCP_PORT_DEFAULT);
	if (err != ERR_OK)
	{
		MUX_ERRORF(("error on udp_connect for UDP TX Perf: %x"MUX_NEW_LINE, err));
		return EXIT_FAILURE;
	}

	sys_sem_signal(&_txTask->sema);

	return EXIT_SUCCESS;
}


/* start thread */
char extUdpTxPerfTask(void)
{
	err_t err;
	int i;

	/* create a udp socket */
	_udpTxTask.pcb = udp_new();
	if (!_udpTxTask.pcb)
	{
		MUX_ERRORF(("OOM for UDP TX Perf"MUX_NEW_LINE));
		return EXIT_FAILURE;
	}

	/* bind local address */
	if ((err = udp_bind(_udpTxTask.pcb, IP_ADDR_ANY, 0)) != ERR_OK)
	{
		MUX_ERRORF(("error on udp_bind for UDP TX Perf: %x"MUX_NEW_LINE, err));
		return EXIT_FAILURE;
	}
	
	/* initialize data buffer being sent */
	for (i = 0; i < SEND_BUFSIZE; i++)
	{
		send_buf[i] = (i % 10) + '0';
	}

	err = sys_sem_new(&_udpTxTask.sema, 0);
	if (err == ERR_MEM)
	{
		MUX_ERRORF(("Can't create semaphore: %x"MUX_NEW_LINE, err));
		return ERR_MEM;
	}

	sys_thread_new("udpPerf", _udpTxPerTask, NULL, TCPIP_THREAD_STACKSIZE, TCPIP_THREAD_PRIO-1);
	
	return EXIT_SUCCESS;
}

#if 0
void print_utxperf_app_header()
{
        xil_printf("%20s %6s %10s %s\r\n", "utxperf client",
                        "N/A",
                        txperf_client_connected ? "CONNECTED" : "CONNECTING",
                        "$ iperf -u -s -i 5 (on host with IP 192.168.1.9)");
}
#endif

#endif

