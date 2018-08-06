
#define _GNU_SOURCE  /* for strcasestr in string.h from nano libc */

#include "lwipMux.h"

#include "muxHttp.h"
#include "base64.h"
#include "netif/ppp/polarssl/sha1.h"

static const char WEBSOCKET_HEADER[] = "Upgrade: websocket"MHTTP_CRLF;
static const char WEBSOCKET_GUID[] = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
static const char WEBSOCKET_KEY[] = "Sec-WebSocket-Key: ";
static const char WEBSOCKET_RESPONSE_HEADER[] = "HTTP/1.1 101 Switching Protocols"MHTTP_CRLF \
                             "Upgrade: websocket"MHTTP_CRLF \
                             "Connection: Upgrade"MHTTP_CRLF \
                             "Sec-WebSocket-Accept: ";

/* base64 encoded key length */
#define WS_BASE64_LEN        29

/* Response buffer length */
#define WS_RSP_LEN           (sizeof(WEBSOCKET_RESPONSE_HEADER) + sizeof(CRLF CRLF) - 2 + WS_BASE64_LEN)

/* WebSocket timeout: X*(HTTPD_POLL_INTERVAL), default is 10*4*500ms = 20s */
#ifndef WS_TIMEOUT
#define WS_TIMEOUT           10
#endif

#if 0
/* Callback functions */
static tWsHandler websocket_cb = NULL;
static tWsOpenHandler websocket_open_cb = NULL;
#endif

/* write reply into mhc->data, and wait to muxHttpSend to send it out */
unsigned short muxHttpWebSocketWrite(MuxHttpConn *mhc, const uint8_t *data, uint16_t len, unsigned char opCode)
{
	unsigned char *buf = mhc->data;
	unsigned short offset = 2, size;
//	err_t err;

	/* write frame header for this transfer */
	buf[0] = MUX_WS_FRAME_FLAG_FIN | opCode;
	if (len > 125)
	{
		offset = 4;
		buf[1] = 126;
		buf[2] = len >> 8;
		buf[3] = len;
	}
	else
	{
		buf[1] = len;
	}

	size = len;
	if( (offset+len) > sizeof(mhc->data) )
	{
		MUX_INFOF(("[WS] data is too long (%d) for buffer with length %d", len, sizeof(mhc->data)-offset) );
		size = sizeof(mhc->data) - offset;
	}

#if 0
	err = muxHttpWrite(mhc, mhc->uri, &offset, TCP_WRITE_FLAG_COPY);
	if(err!= ERR_OK)
	{
		MUX_ERRORF(("[WS] write frame header failed"));
		return err;
	}

	MUX_DEBUGF(MUX_HTTPD_DEBUG, ("[WS] sending frame header :%d", offset) );
	offset = len;
	err = muxHttpWrite(mhc, data, &offset, TCP_WRITE_FLAG_COPY);
	if(err!= ERR_OK)
	{
		MUX_ERRORF(("[WS] write frame data failed"));
		return err;
	}
	MUX_DEBUGF(MUX_HTTPD_DEBUG, ("[WS] sending data :%d", offset) );
#else
	memcpy(mhc->data+offset, data, size);
	mhc->dataSendIndex = offset + size;
#endif

	mhc->httpStatusCode = WEB_RES_REQUEST_OK;

	return size;
}


/**
 * Send status code 1000 (normal closure).
 */
err_t muxHttpWebSocketSendClose(MuxHttpConn *mhc)
{
	const u8_t buf[] = {MUX_WS_FRAME_FLAG_FIN|MUX_WS_CODE_CLOSE, 0x02, 0x03, 0xe8};

	MUX_DEBUGF(MUX_HTTPD_DEBUG, ("[WS] closing connection"));
	return tcp_write(mhc->pcb, buf, (unsigned short )sizeof (buf), TCP_WRITE_FLAG_COPY);
}

/**
 * Parse websocket frame.
 *
 * @return ERR_OK: frame parsed
 *         ERR_CLSD: close request from client
 *         ERR_VAL: invalid frame.
 */
err_t muxHttpWebSocketParseFrame(MuxHttpConn *mhc, struct pbuf *p)
{
	u8_t *data = (u8_t *) p->payload;
	u8_t opcode;

	if (data != NULL && p->tot_len > 1)
	{
		MUX_DEBUGF(MUX_HTTPD_DEBUG, ("[WS] frame received"));
		if ((data[0] & MUX_WS_FRAME_FLAG_FIN) == 0)
		{
			MUX_INFOF(("[WS] Warning: continuation frames not supported") );
			return ERR_OK;
		}
		
		opcode = data[0] & 0x0F;
		
		switch (opcode)
		{
			case MUX_WS_CODE_TEXT:
			case MUX_WS_CODE_BINARY:
				MUX_DEBUGF(MUX_HTTPD_DEBUG, ("[WS] Opcode: 0x%hX, packet length: %d", opcode, p->tot_len));
				
				/* length: 7 bits; 7+16bits; 7+64 bits; only first 2 options are supported */
				if (p->len > 6 )
				{
					int data_offset = 6;
					u8_t *dptr = &data[6];
					u8_t *kptr = &data[2];
					u16_t payloadLen = data[1] & 0x7F;
					unsigned short copied = 0;

					if(! (data[1] & 0x80)  )
					{
						MUX_ERRORF(("[WS] frame from client is not masked"));
					}

					if (payloadLen == 127)
					{/* 7+64 bits mode for length field */
						/* most likely won't happen inside non-fragmented frame */
						MUX_DEBUGF(MUX_HTTPD_DEBUG, ("[WS] Warning: frame is too long"));
						return ERR_OK;
					}
					else if (payloadLen == 126)
					{/* 7+16 bits mode for length field */
						/* extended length */
						dptr += 2;
						kptr += 2;
						data_offset += 2;
						payloadLen = (data[2] << 8) | data[3];
					}

					if (payloadLen > p->tot_len)
					{
						MUX_ERRORF(("[WS] Error: incorrect frame size"));
						return ERR_VAL;
					}

#if 1
					/* unmask */
					for (int i = 0; i < payloadLen; i++)
						*(dptr++) ^= kptr[i % 4];
#endif					

					copied = pbuf_copy_partial(p, mhc->data, payloadLen, data_offset);
					if(copied != payloadLen)
					{
						MUX_INFOF(("[WS] frame: only copied %d bytes from %d bytes", copied, payloadLen));
					}
					
					mhc->data[copied] = 0;
					MUX_DEBUGF(MUX_HTTPD_DEBUG, ("[WS] frame (%d):'%s'", copied, mhc->data) );
					CONSOLE_DEBUG_MEM(mhc->data, copied, 0, "[WS] Frame");

					muxHttpWebSocketWrite(mhc, mhc->data, payloadLen, TCP_WRITE_FLAG_COPY);

					/* user callback */
					// websocket_cb(pcb, &data[data_offset], payloadLen, opcode);
				}
				break;
				
			case MUX_WS_CODE_CLOSE:
				MUX_DEBUGF(MUX_HTTPD_DEBUG, ("[WS] close request"));
				return ERR_CLSD;

			default:
				MUX_DEBUGF(MUX_HTTPD_DEBUG, ("[WS] Unsupported opcode 0x%hX", opcode));
				break;
		}
		
		return ERR_OK;

	}
	return ERR_VAL;
}



/*
* check the websocket header and reply with web socket header when connection is web socket
* SUCESS: this is web socket; FAILURE: this is not web socket, continue to other process 
*/
char muxHttpWebSocketParseHeader(MuxHttpConn *mhc, unsigned char *data, u16_t data_len)
{
	char *keyStart, *keyEnd;
	int	keyLen;
	
	/* Parse WebSocket request */
	if(! strcasestr((char *)data, WEBSOCKET_HEADER) ) 
	{
		return EXIT_FAILURE;
	}
	
	MUX_DEBUGF(MUX_HTTPD_DEBUG, ("[WS] opening handshake"));
	mhc->reqType = MUX_HTTP_REQ_T_WEBSOCKET;
	
	keyStart = strcasestr((char *)data, WEBSOCKET_KEY);
	if(! keyStart)
	{
		MUX_ERRORF(("[WS] malformed packet: can't find Key header"));
		goto badWebSocketReq;
	}
	
	keyStart += sizeof(WEBSOCKET_KEY) - 1;

	keyEnd = strstr(keyStart, MHTTP_CRLF);
	if (! keyEnd)
	{
		MUX_ERRORF( ("[WS] malformed packet: Key header is not correct"));
		goto badWebSocketReq;
	}
	
	keyLen = sizeof(char) * (keyEnd - keyStart);
	
	if ((keyLen + sizeof(WEBSOCKET_GUID) < sizeof(mhc->uri) ) && (keyLen > 0))
	{
		unsigned char sha1sum[MD_OUT_LENGTH_SHA1];
		unsigned int olen = WS_BASE64_LEN;
		int index = 0;

		/* base64(sha1(keyHeader+GUID) ) */

		/* Concatenate key */
		memcpy(mhc->uri, keyStart, (unsigned int)keyLen);
		memcpy(mhc->uri+keyLen, WEBSOCKET_GUID, sizeof(WEBSOCKET_GUID) );

		/* Get SHA1 */
		keyLen += sizeof(WEBSOCKET_GUID) - 1;
		MUX_DEBUGF(MUX_HTTPD_DEBUG, ("[WS] Resulting key(%d): '%s'", keyLen, mhc->uri) );
		sha1((unsigned char *)mhc->uri, keyLen, sha1sum);

		/* Base64 encode */
		index = mbedtls_base64_encode((unsigned char *)mhc->uri, WS_BASE64_LEN, &olen, sha1sum, MD_OUT_LENGTH_SHA1);

//			if (index == 0)
		{
			index = snprintf((char *)mhc->data, sizeof(mhc->data), "%s%s"MHTTP_CRLF MHTTP_CRLF, WEBSOCKET_RESPONSE_HEADER, mhc->uri);

			/* Send response */
			MUX_DEBUGF(MUX_HTTPD_DEBUG, ("[WS] Sending (%d):'%s'", index, mhc->data));
			tcp_write(mhc->pcb, mhc->data, index, TCP_WRITE_FLAG_COPY);
		}

		return EXIT_SUCCESS;
		
	}

	MUX_DEBUGF(MUX_HTTPD_DEBUG, ("[WS] Key overflow"));

badWebSocketReq:

	muxHttpWebSocketSendClose(mhc);
	return EXIT_SUCCESS;
	
}


