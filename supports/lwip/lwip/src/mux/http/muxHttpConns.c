

#include "lwipMux.h"

#include "muxHttp.h"

/* Initialize a MuxHttpConn */
static void _mhttpStateInit(MuxHttpConn* mhc)
{
	/* Initialize the structure. */
	memset(mhc, 0, sizeof(MuxHttpConn));

	mhc->httpStatusCode = WEB_RES_CONTINUE;
	mhc->nodeInfo = &nmosNode;
}

/** Free a MuxHttpConn.
 * Also frees the file data if dynamic.
 */
static void _mhttpStateEof(MuxHttpConn *mhc)
{
	if(mhc->handle)
	{
#if	MHTTPD_TIMING
		u32_t ms_needed = sys_now() - mhc->time_started;
		u32_t needed = LWIP_MAX(1, (ms_needed/100));
		LWIP_DEBUGF(HTTPD_DEBUG_TIMING, ("httpd: needed %"U32_F" ms to send file of %d bytes -> %"U32_F" bytes/sec\n", ms_needed, mhc->handle->len, ((((u32_t)mhc->handle->len) * 10) / needed)));
#endif
		mfsClose(mhc->handle);
		mhc->handle = NULL;
	}
	
#if	MHTTPD_DYNAMIC_FILE_READ
	if (mhc->buf != NULL)
	{
		mem_free(mhc->buf);
		mhc->buf = NULL;
	}
#endif

#if	MHTTPD_SSI
	if (mhc->ssi)
	{
		mhttp_ssi_state_free(mhc->ssi);
		mhc->ssi = NULL;
	}
#endif

	if (mhc->req)
	{
		pbuf_free(mhc->req);
		mhc->req = NULL;
	}
}


/**
 * The connection shall be actively closed (using RST to close from fault states).
 * Reset the sent- and recv-callbacks.
 *
 * @param pcb the tcp pcb to reset callbacks
 * @param mhc connection state to free
 */
static err_t _mhttpConnCloseOrAbort(MuxHttpConn *mhc, struct tcp_pcb *pcb, u8_t abort_conn)
{
	err_t err = ERR_OK;
	MUX_DEBUGF(MUX_HTTPD_DEBUG, ("Closing connection %p\n", (void*)pcb));
	MUX_RUNTIME_CFG *runCfg = NULL;

	if (mhc != NULL)
	{
		if ((mhc->postDataLeft != 0)
#if	MHTTPD_POST_MANUAL_WND
			|| ((mhc->no_auto_wnd != 0) && (mhc->unrecved_bytes != 0))
#endif
			)
		{
			/* or move to recv function. J.L. */
			if(HTTPREQ_IS_UPLOAD(mhc) )
			{
				CANCEL_UPDATE(mhc->nodeInfo->runCfg);
			}

			/* make sure the post code knows that the connection is closed */
			muxHttpPostDataFinished(mhc);
		}

		if( HTTPREQ_IS_WEBSOCKET(mhc) )
		{
			muxHttpWebSocketSendClose(mhc);
		}

	}
	LWIP_ASSERT(("HTTP Connection is null"), mhc!= NULL);
	runCfg = mhc->nodeInfo->runCfg;


	tcp_arg(pcb, NULL);
	tcp_recv(pcb, NULL);
	tcp_err(pcb, NULL);
	tcp_poll(pcb, NULL, 0);
	tcp_sent(pcb, NULL);

	
	if (mhc != NULL)
	{
		mhttpConnFree(mhc);
	}

	if (abort_conn)
	{
		MUX_INFOF( ("TCP %p is abort"MUX_NEW_LINE, (void*)pcb));
		tcp_abort(pcb);
		return ERR_OK;
	}
	
	runCfg->currentHttpConns --;
	err = tcp_close(pcb);
	if (err != ERR_OK)
	{
		MUX_DEBUGF(MUX_HTTPD_DEBUG, ("Error %d closing %p"MUX_NEW_LINE, err, (void*)pcb));
		MUX_ERRORF( ("Error %d closing %p"MUX_NEW_LINE, err, (void*)pcb));
		/* error closing, try again later in poll */
		tcp_poll(pcb, mhttpPoll,  MHTTPD_POLL_INTERVAL);
	}
	return err;
}

#if	MHTTPD_KILL_OLD_ON_CONNECTIONS_EXCEEDED
/** global list of active HTTP connections, use to kill the oldest when running out of memory */
static MuxHttpConn *_mhttpConns;

static void _addConnection(MuxHttpConn *mhc)
{
	/* add the connection to the list */
	mhc->next = _mhttpConns;
	_mhttpConns = mhc;
}

static void _removeConnection(MuxHttpConn *mhc)
{
	/* take the connection off the list */
	if (_mhttpConns)
	{
		if (_mhttpConns == mhc)
		{
			_mhttpConns = mhc->next;
		}
		else
		{
			MuxHttpConn *last;
			for(last = _mhttpConns; last->next != NULL; last = last->next)
			{
				if (last->next == mhc)
				{
					last->next = mhc->next;
					break;
				}
			}
		}
	}
}

static void _killOldestConnection(u8_t ssi_required)
{
	MuxHttpConn *mhc = _mhttpConns;
	MuxHttpConn *hs_free_next = NULL;

	while(mhc && mhc->next)
	{
#if	MHTTPD_SSI
		if (ssi_required)
		{
			if (mhc->next->ssi != NULL)
			{
				hs_free_next = mhc;
			}
		}
		else
#else
		LWIP_UNUSED_ARG(ssi_required);
#endif
		{
			hs_free_next = mhc;
		}
		LWIP_ASSERT("broken list", mhc != mhc->next);
		mhc = mhc->next;
	}
	
	if (hs_free_next != NULL)
	{
		LWIP_ASSERT("hs_free_next->next != NULL", hs_free_next->next != NULL);
		LWIP_ASSERT("hs_free_next->next->pcb != NULL", hs_free_next->next->pcb != NULL);
		/* send RST when killing a connection because of memory shortage */
		_mhttpConnCloseOrAbort(hs_free_next->next, hs_free_next->next->pcb, 1); /* this also unlinks the mhttp_state from the list */
	}
}
#else /* MHTTPD_KILL_OLD_ON_CONNECTIONS_EXCEEDED */

#define _addConnection(mhc)
#define _removeConnection(mhc)

#endif


/** Allocate a MuxHttpConn. */
MuxHttpConn *mhttpConnAlloc(void)
{
	MuxHttpConn *mhc = HTTP_ALLOC_HTTP_STATE();
#if	MHTTPD_KILL_OLD_ON_CONNECTIONS_EXCEEDED
	if (mhc == NULL)
	{
		_killOldestConnection(0);
		mhc = HTTP_ALLOC_HTTP_STATE();
	}
#endif

	if (mhc != NULL)
	{
		_mhttpStateInit(mhc);
		_addConnection(mhc);
	}
	return mhc;
}

/** Free a MuxHttpConn.
 * Also frees the file data if dynamic.
 */
void mhttpConnFree(MuxHttpConn *mhc)
{
	if (mhc != NULL)
	{
		_mhttpStateEof(mhc);
		_removeConnection(mhc);
		HTTP_FREE_HTTP_STATE(mhc);
	}
}


/**
 * The connection shall be actively closed.
 * Reset the sent- and recv-callbacks.
 *
 * @param pcb the tcp pcb to reset callbacks
 * @param mhc connection state to free
 */
err_t mhttpConnClose(MuxHttpConn *mhc, struct tcp_pcb *pcb)
{
	return _mhttpConnCloseOrAbort(mhc, pcb, 0);
}

/** End of file: either close the connection (Connection: close) or
 * close the file (Connection: keep-alive)
 */
void mhttpConnEof(MuxHttpConn *mhc)
{
  /* HTTP/1.1 persistent connection? (Not supported for SSI) */
#if	MHTTPD_SUPPORT_11_KEEPALIVE
	if (mhc->keepalive)
	{
		_removeConnection(mhc);

		_mhttpStateEof(mhc);
		_mhttpStateInit(mhc);
		/* restore state: */
//		mhc->pcb = pcb;
		mhc->keepalive = 1;
		_addConnection(mhc);
		/* ensure nagle doesn't interfere with sending all data as fast as possible: */
		tcp_nagle_disable(mhc->pcb);
	}
	else
#endif
	{
		mhttpConnClose(mhc, mhc->pcb);
	}
}



/** Call tcp_write() in a loop trying smaller and smaller length
 *
 * @param pcb tcp_pcb to send
 * @param ptr Data to send
 * @param length Length of data to send (in/out: on return, contains the
 *        amount of data sent)
 * @param apiflags directly passed to tcp_write
 * @return the return value of tcp_write
 */
err_t muxHttpWrite(MuxHttpConn *mhc, const void* ptr, u16_t *length, u8_t apiflags)
{
	u16_t len, max_len;
	err_t err;
	LWIP_ASSERT("length != NULL", length != NULL);
	len = *length;
	if (len == 0)
	{
		return ERR_OK;
	}
	/* We cannot send more data than space available in the send buffer. */
	max_len = tcp_sndbuf(mhc->pcb);
	if (max_len < len)
	{
		len = max_len;
	}
#ifdef MHTTPD_MAX_WRITE_LEN
	/* Additional limitation: e.g. don't enqueue more than 2*mss at once */
	max_len = MHTTPD_MAX_WRITE_LEN(mhc->pcb);
	if(len > max_len)
	{
		len = max_len;
	}
#endif

	do
	{
		MUX_DEBUGF(MUX_HTTPD_DEBUG, ("Trying go send %d bytes, flags:0x%x", len, apiflags));
		err = tcp_write(mhc->pcb, ptr, len, apiflags);
		if (err == ERR_MEM)
		{
			if ((tcp_sndbuf(mhc->pcb) == 0) ||(tcp_sndqueuelen(mhc->pcb) >= TCP_SND_QUEUELEN))
			{/* no need to try smaller sizes */
				len = 1;
			}
			else
			{
				len /= 2;
			}

			MUX_DEBUGF(MUX_HTTPD_DEBUG,  ("Send failed, trying less (%d bytes)", len));
		}
	} while ((err == ERR_MEM) && (len > 1));

	if (err == ERR_OK)
	{
		MUX_DEBUGF(MUX_HTTPD_DEBUG, ("Sent %d bytes", len));
		*length = len;
	}
	else
	{
		MUX_DEBUGF(MUX_HTTPD_DEBUG, ("Send failed with err %d (\"%s\")", err, lwip_strerr(err)));
		*length = 0;
	}

#if	MHTTPD_SUPPORT_11_KEEPALIVE
	/* ensure nagle is normally enabled (only disabled for persistent connections
	when all data has been enqueued but the connection stays open for the next
	request */
	tcp_nagle_enable(mhc->pcb);
#endif

	return err;
}


#if	MHTTPD_SSI
/** Allocate as struct mhttp_ssi_state. */
static struct mhttp_ssi_state *http_ssi_state_alloc(void)
{
	struct mhttp_ssi_state *ret = HTTP_ALLOC_SSI_STATE();
	
#if	MHTTPD_KILL_OLD_ON_CONNECTIONS_EXCEEDED
	if (ret == NULL)
	{
		_killOldestConnection(1);
		ret = HTTP_ALLOC_SSI_STATE();
	}
#endif

	if (ret != NULL)
	{
		memset(ret, 0, sizeof(struct mhttp_ssi_state));
	}
	return ret;
}

/** Free a struct mhttp_ssi_state. */
static void http_ssi_state_free(struct mhttp_ssi_state *ssi)
{
	if (ssi != NULL)
	{
		HTTP_FREE_SSI_STATE(ssi);
	}
}

#endif

