/*
*  Schedule task : scheduling request between HTTP SEVER and HTTP CLIENT 
*/


#include "lwipExt.h"

#include "http.h"
#include "httpClient.h"

static sys_mbox_t 		_httpScheduleMailBox;
sys_mutex_t			_httpScheduleMutex;	


char extHttpSchedulePostEvent(char type, uint32_t ip, uint16_t port, char *uri)
{
	HttpClientReq *req = NULL;

	HCQ_LOCK();	
	HCQ_EVENT_ALLOC(req);
	HCQ_UNLOCK();
	
	if (req == NULL)
	{
		EXT_ERRORF((HTTP_CLIENT_NAME"No memory available for HTTP Client Event now"));
		return EXIT_FAILURE;
	}

	memset(req, 0, sizeof(HttpClientReq));
	req->type = type;
	req->destIp = ip;
	req->destPort = port;
	snprintf(req->url, sizeof(req->url), "%s", uri);
	
	EXT_DEBUGF(HTTP_CLIENT_DEBUG, (HTTP_CLIENT_NAME"post event %s", CMN_FIND_HC_EVENT(hce->event)) );

	if (sys_mbox_trypost(&_httpScheduleMailBox, req) != ERR_OK)
	{
		EXT_ERRORF((HTTP_CLIENT_NAME"Post to "EXT_TASK_SCHEDULE" Mailbox failed"));
		HCQ_EVENT_FREE(req);
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}



static void _extHttpScheduleTask(void *arg)
{
	HttpClientReq *req = NULL;

	if (sys_mutex_new(&_httpScheduleMutex) != ERR_OK)
	{
		EXT_ASSERT((HTTP_CLIENT_NAME"failed to create scheduler Mutex"), 0);
	}

	while (1)
	{
		sys_mbox_fetch(&_httpScheduleMailBox, (void **)&req);
		if (req == NULL)
		{
			EXT_ASSERT((HTTP_CLIENT_NAME"task: invalid message"), 0);
			continue;
		}

		HTTP_CLIENT_WAIT();

		extHttpClientNewRequest(req);

		HTTP_CLIENT_SIGNAL();
		
		HCQ_EVENT_FREE(req);
		
	}
	
}

void extHttpScheduleMain(void *data)
{
	HttpClient *hc = &_httpClient;

	EXT_RUNTIME_CFG *runCfg = (EXT_RUNTIME_CFG *)data;
	
	memset(hc, 0, sizeof(HttpClient));
	hc->req.destIp = IPADDR_NONE;
	hc->req.destPort = -1;
	hc->state = HC_STATE_WAIT;
	hc->runCfg = runCfg;


	sys_thread_new(EXT_TASK_SCHEDULE, _extHttpScheduleTask, hc, EXT_NET_IF_TASK_STACK_SIZE, EXT_NET_IF_TASK_PRIORITY -1);
	
}


