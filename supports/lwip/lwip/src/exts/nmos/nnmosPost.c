

#include "lwipExt.h"

#include "http.h"
#include "jsmn.h"


char muxNmosPostDataBegin(void *conn, unsigned char *data, unsigned short len)
{
	MuxHttpConn *mhc = (MuxHttpConn *)conn;

	if(IS_STRING_EQUAL(mhc->uri, NMOS_API_URI_CONNECTION"/single/senders") )
	{
		return EXIT_SUCCESS;
	}

	MUX_ERRORF(("URI '%s' is not validate for POST", mhc->uri) );
	muxHttpRestError(mhc, WEB_RES_NOT_IMP, "POST is not support in this URI");
	return EXIT_FAILURE;
}

char muxNmosPostDataRecv(void *conn, struct pbuf *p)
{
	MuxHttpConn *mhc = (MuxHttpConn *)conn;
	unsigned short len, copied;
	
	len = LWIP_MIN(p->tot_len, sizeof(mhc->data));
	
	copied = pbuf_copy_partial(p, mhc->data+mhc->dataSendIndex, len, 0);
	mhc->dataSendIndex += copied;
	mhc->data[mhc->dataSendIndex] = 0;

	MUX_DEBUGF(MUX_HTTPD_DEBUG, ("packet %d bytes, copied %d (%d)byte  data: '%s'", p->tot_len, copied, mhc->dataSendIndex, mhc->data) );

	if(HTTPREQ_IS_UPLOAD(mhc) )
	{
	}

	pbuf_free(p);
	if(copied != len)
	{
		MUX_INFOF(("Only copied %d bytes from %d byte data", copied, len) );
		return EXIT_FAILURE;
	}

	
	return EXIT_SUCCESS;
}

/* begin to execute on the recevied data of POST request or when conn is closed */
void muxNmosPostDataFinished(void *conn)
{
	MuxHttpConn *mhc = (MuxHttpConn *)conn;

	mhc = mhc;
	MUX_DEBUGF(MUX_HTTPD_DEBUG, ("POST request on '%s', data is '%s'", mhc->uri, mhc->data) );

	snprintf(mhc->uri + strlen(mhc->uri), sizeof(mhc->uri)-strlen(mhc->uri), " %s", "will be implemented in future" );

	muxHttpRestError(mhc, WEB_RES_NOT_IMP, (const char *)mhc->uri);

	TRACE();
}

