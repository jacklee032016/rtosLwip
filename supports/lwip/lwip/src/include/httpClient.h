/*
* 
*/

#ifndef __HTTP_CLIENT_H__
#define __HTTP_CLIENT_H__

#define		EXT_HTTP_CLIENT_MBOX_SIZE					2
#define		EXT_HTTP_CLIENT_PORT							1000

#define		EXT_HTTP_CLIENT_TIMEOUT_NEW_CONN			2000	/* for new TCP connection */
#define		EXT_HTTP_CLIENT_TIMEOUT_2_WAIT				2000	/* from state of DATA/ERROR to state of WAIT*/

#define		HTTPC_MAX_RETRIES								4
#define		HTTPC_POLL_INTERVAL							4	/* in 500 ms*/

#define		HTTP_CLIENT_DEBUG								EXT_DBG_OFF

typedef	enum
{
	HC_EVENT_NEW = EXT_EVENT_NONE +1,		/* new req */
	HC_EVENT_CONNECTED,	/* TCP callback: connected */
	HC_EVENT_RECV,		/* TCP callback: recv */
	HC_EVENT_POLL,			/* TCP callback: POLL */
	HC_EVENT_SENT,			/* TCP callback: SENT */

	HC_EVENT_CLOSE,
	HC_EVENT_TIMEOUT,

	HC_EVENT_ERROR,
	
	HC_EVENT_MAX,
}HC_EVENT_T;


typedef enum
{
	HC_STATE_WAIT = EXT_STATE_CONTINUE +1,
	HC_STATE_INIT,	
	HC_STATE_CONN,
//	HC_STATE_REQ,
	HC_STATE_DATA, 
//	HC_STATE_CLOSE,
	HC_STATE_ERROR,		/* error event from TCP stack implementation  */
	HC_STATE_MAX
}HC_STATE_T;


typedef	struct _HttpClientStatus
{
	HttpClientReq			req;
	
	uint32_t				total;
	uint32_t				httpFails;
	uint32_t				dataErrors;

	char					msg[64];
}HttpClientStatus;


typedef	struct _HttpClient
{
	char					state;
	HcEvent				*evt;

	char					reqType;		/* JSON or SDP */
	
	char					buf[2048];
	int					length;	/* length of response */

	char					*data;	/* data part in HTTP response*/
	int					contentLength;
	
//	uint32_t				destIp;
//	uint16_t				destPort;
	
	HttpClientReq			*reqList;

	HttpClientStatus		requests[3];

	struct tcp_pcb 		*pcb;
	uint8_t				retryCount;		/* poll count */

	/* status and statistics */
	char					statusCode;
	char					msg[256];	/* current detailed status message*/
	
	uint32_t				reqs;
	uint32_t				fails;
	
	EXT_RUNTIME_CFG		*runCfg;
}HttpClient;

extern	sys_mutex_t			_httpClientMutex;
extern	sys_mutex_t			_httpScheduleMutex;

extern	sys_sem_t			_httpClientSema;	/* wait and wake between client and its scheduler */

extern	HttpClient 			_httpClient;


#define	HTTP_CLIENT_WAIT()		\
	sys_sem_wait(&_httpClientSema);


#define	HTTP_CLIENT_SIGNAL()		\
	sys_sem_signal(&_httpClientSema);


/* for http client */
#define	HC_LOCK()		\
		do{sys_mutex_lock(&_httpClientMutex);}while(0)


#define	HC_UNLOCK()	\
		do{sys_mutex_unlock(&_httpClientMutex);}while(0)



#define	HC_EVENT_ALLOC(he) \
		do{(he) = (HcEvent *)memp_malloc(MEMP_EXT_HC_EVENT);}while(0)

#define	HC_EVENT_FREE(he) \
		do{memp_free(MEMP_EXT_HC_EVENT, (he)); (he) = NULL;}while(0)


/* for http client request */
#define	HCQ_LOCK()		\
		do{sys_mutex_lock(&_httpScheduleMutex);}while(0)


#define	HCQ_UNLOCK()	\
		do{sys_mutex_unlock(&_httpScheduleMutex);}while(0)


#define	HCQ_EVENT_ALLOC(req) \
		do{(req) = (HttpClientReq *)memp_malloc(MEMP_EXT_HC_REQ);}while(0)

#define	HCQ_EVENT_FREE(req) \
		do{memp_free(MEMP_EXT_HC_REQ, (req)); (req) = NULL;}while(0)



err_t httpClientConnected(void *arg, struct tcp_pcb *pcb, err_t err);

unsigned char httpClientEventConnected(void *arg);


void extHttpClientClearCurrentRequest(HttpClient *hc);

void	httpClientFsmHandle(HcEvent *hce);


/* check whether request is waiting */
#define	HTTP_CLIENT_IS_NOT_REQ(hc)		\
	((hc)->reqList == NULL ) 

//	((hc)->req.ip == IPADDR_NONE ) 
	//&& (hc)->req.port == -1 )


#define	HTTP_CLIENT_SET_PCB(hc, _pcb) 	\
	do{HC_LOCK(); (hc)->pcb = (_pcb); HC_UNLOCK();}while(0)


extern	sys_timer_t		_hcTimer;		/* timer new TCP connection */


err_t extHttpClientClosePcb(HttpClient *hc, struct tcp_pcb *_pcb);

err_t extHttpSdpParse(HttpClient *hc, EXT_RUNTIME_CFG	*rxCfg, char *data, uint16_t size);


#endif

