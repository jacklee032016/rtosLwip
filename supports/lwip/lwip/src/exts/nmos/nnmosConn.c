/*
* Interface NMOS Connection API
*/

#include "lwipExt.h"

#include "http.h"
#include "jsmn.h"

#if LWIP_EXT_NMOS

static int _nmosConstraintsHander(char *data, unsigned int size, MuxNmosConstraints *cstrts, unsigned char isSender)
{
	int index = 0;
	
	index += snprintf(data+index, size-index, "{");

	EXT_ASSERT(("Constraints->parent is null"), cstrts->parent != NULL);
	EXT_ASSERT(("cstrts->parent->node is null"), cstrts->parent->node != NULL);

	/* resource core fields */
	index += snprintf(data+index, size-index, "\""NMOS_LABEL_SOURCE_IP"\":{\"enum\":[\"%s\"]},", EXT_LWIP_IPADD_TO_STR(&cstrts->parent->node->runCfg->local.ip) );

	if(isSender)
	{/* dest IP and source port : only for sender */
		index += snprintf(data+index, size-index, "\""NMOS_LABEL_DEST_IP"\":{\"enum\":[\"%s\"]},", EXT_LWIP_IPADD_TO_STR(&cstrts->parent->node->runCfg->dest.ip) );

		index += snprintf(data+index, size-index, "\""NMOS_LABEL_SOURCE_PORT"\":{\""NMOS_LABEL_MINIMUM"\":%d,\""NMOS_LABEL_MAXIMUM"\":%d},", 
			cstrts->sourcePortMini, cstrts->sourcePortMini+cstrts->portRange );
	}
	else
	{
		index += snprintf(data+index, size-index, "\""NMOS_LABEL_MULTICAST_IP"\":{\"enum\":[\"%s\"]},", EXT_LWIP_IPADD_TO_STR(&cstrts->parent->node->runCfg->dest.ip) );
		index += snprintf(data+index, size-index, "\""NMOS_LABEL_INTERFACE_IP"\":{\"enum\":[\"%s\"]},", EXT_LWIP_IPADD_TO_STR(&cstrts->parent->node->runCfg->local.ip) );
	}
	
	index += snprintf(data+index, size-index, "\""NMOS_LABEL_DEST_PORT"\":{\""NMOS_LABEL_MINIMUM"\":%d,\""NMOS_LABEL_MAXIMUM"\":%d},", 
		cstrts->destPortMini, cstrts->destPortMini+cstrts->portRange );

	index += snprintf(data+index, size-index, "\""NMOS_LABEL_FEC_ENABLED"\":%s,", EXT_JSON_BOOL_STR(cstrts->fecEnabled));

	index += snprintf(data+index, size-index, "\""NMOS_LABEL_RTCP_ENABLED"\":%s,", EXT_JSON_BOOL_STR(cstrts->rtcpEnabled));
	index += snprintf(data+index, size-index, "\""NMOS_LABEL_RTP_ENABLED"\":%s", EXT_JSON_BOOL_STR(cstrts->rtpEnabled));

	index += snprintf(data+index, size-index, "}");

	return index;
}


/******** URIs in single/receivers  ************/

static char _connSingleReceiverActiveHander(ExtHttpConn  *mhc, void *data)
{
	int index = 0;
//	int i = 0;
//	const ApiAccessPoint	*apiAp = (const ApiAccessPoint *)data;
	MuxNmosDevice	*dev = mhc->nodeInfo->device;
	MuxNmosReceiver	*rcv = dev->receivers;
	unsigned short srcPort, destPort;

	EXT_ASSERT(("node is null"), mhc->nodeInfo!=NULL);
	EXT_ASSERT(("device is null"), dev!=NULL);


	if(!UUID_IS_NULL(&mhc->apiReq.uuid) )
	{
		while(rcv)
		{
			if( extUuidEqual(&mhc->apiReq.uuid, &rcv->resourceId.nmosId.uuid) )
			{
				break;
			}
			rcv = rcv->next;
		}

		if(rcv == NULL)
		{
			return cmnHttpRestError(mhc, WEB_RES_NOT_FOUND, "ID not found for sender");
		}
	}


	index += snprintf((char *)mhc->data+index, sizeof(mhc->data)-index, "{");

	/* resource core fields */
	index += snprintf((char *)mhc->data+index, sizeof(mhc->data)-index, "\""NMOS_LABEL_SENDER_ID"\":\"%s\",", extUuidToString(&rcv->subscriber->resourceId.nmosId.uuid));
	index += snprintf((char *)mhc->data+index, sizeof(mhc->data)-index, "\""NMOS_LABEL_MASTER_EANBLE"\":%s,", NMOS_LABEL_TRUE);
	/*activation */
	index += snprintf((char *)mhc->data+index, sizeof(mhc->data)-index, "\""NMOS_LABEL_ACTIVATION"\":{");
	index += snprintf((char *)mhc->data+index, sizeof(mhc->data)-index, "\""NMOS_LABEL_MODE"\":\"%s\",", NMOS_FIND_CONN_ACTIVATE(NMOS_CONN_ACTIVATE_T_IMMEDIATE));
	index += snprintf((char *)mhc->data+index, sizeof(mhc->data)-index, "\""NMOS_LABEL_REQUESTED_TIME"\":\"%"FOR_U32":%d\",",  sys_now(), 0 );
	index += snprintf((char *)mhc->data+index, sizeof(mhc->data)-index, "\""NMOS_LABEL_ACTIVATION_TIME"\":\"%"FOR_U32":%d\"},", sys_now(), 0);

	/* transport_params */
	index += snprintf((char *)mhc->data+index, sizeof(mhc->data)-index, "\""NMOS_LABEL_TRANSPORT_PARAMS"\":[{");

	index += snprintf((char *)mhc->data+index, sizeof(mhc->data), "\""NMOS_LABEL_SOURCE_IP"\":{\"enum\":[\"%s\"]},", EXT_LWIP_IPADD_TO_STR(&rcv->device->node->runCfg->local.ip) );
	index += snprintf((char *)mhc->data+index, sizeof(mhc->data), "\""NMOS_LABEL_DEST_IP"\":{\"enum\":[\"%s\"]},", EXT_LWIP_IPADD_TO_STR(&rcv->device->node->runCfg->dest.ip) );

	switch (rcv->format)
	{
		case NMOS_SOURCE_FORMAT_AUDIO:
			srcPort = rcv->device->node->runCfg->local.aport;
			destPort = rcv->device->node->runCfg->dest.aport;
			break;
		case NMOS_SOURCE_FORMAT_VIDEO:
			srcPort = rcv->device->node->runCfg->local.vport;
			destPort = rcv->device->node->runCfg->dest.vport;
			break;
		case NMOS_SOURCE_FORMAT_DATA:
		default:	
			srcPort = rcv->device->node->runCfg->local.vport;
			destPort = rcv->device->node->runCfg->dest.vport;
			break;
	}
	
	index += snprintf((char *)mhc->data+index, sizeof(mhc->data), "\""NMOS_LABEL_SOURCE_PORT"\":%d,", srcPort);
	index += snprintf((char *)mhc->data+index, sizeof(mhc->data), "\""NMOS_LABEL_DEST_PORT"\":%d,", destPort );

	index += snprintf((char *)mhc->data+index, sizeof(mhc->data), "\""NMOS_LABEL_FEC_ENABLED"\":%s,", NMOS_LABEL_FALSE);

	index += snprintf((char *)mhc->data+index, sizeof(mhc->data), "\""NMOS_LABEL_RTCP_ENABLED"\":%s,", NMOS_LABEL_FALSE);
	index += snprintf((char *)mhc->data+index, sizeof(mhc->data), "\""NMOS_LABEL_RTP_ENABLED"\":%s", NMOS_LABEL_TRUE);

	index += snprintf((char *)mhc->data+index, sizeof(mhc->data)-index, "}]" ); /* transport_params */

	index += snprintf((char *)mhc->data+index, sizeof(mhc->data)-index, "}" );

	mhc->contentLength = (unsigned short)index;
	mhc->dataSendIndex = 0;
	
	mhc->httpStatusCode = WEB_RES_REQUEST_OK;
	return EXIT_SUCCESS;
}

static const ApiAccessPoint	__apConnSingleReceiversActive =
{
	type 	: 	2,
	name	: 	NMOS_CONN_URL_STR_ACTIVE,
	callback :	_connSingleReceiverActiveHander,
	
	child		:	NULL,
	next		:	NULL
};


static const ApiAccessPoint	__apConnSingleReceiversStaged =
{
	type 	: 	2,
	name	: 	NMOS_CONN_URL_STR_STAGED,
	callback :	extNmosNodeDumpHander,
	
	child		:	NULL,
	next		:	&__apConnSingleReceiversActive
};


static char _connSingleReceiveConstraintsHander(ExtHttpConn  *mhc, void *data)
{
	int index = 0;
//	const ApiAccessPoint	*apiAp = (const ApiAccessPoint *)data;
	MuxNmosDevice	*dev = mhc->nodeInfo->device;
	MuxNmosReceiver	*rcv = dev->receivers;

	EXT_ASSERT(("node is null"), mhc->nodeInfo!=NULL);
	EXT_ASSERT(("device is null"), dev!=NULL);

	index += snprintf((char *)mhc->data+index, sizeof(mhc->data)-index, "[" );

	if(!UUID_IS_NULL(&mhc->apiReq.uuid) )
	{
		while(rcv)
		{
			if( extUuidEqual(&mhc->apiReq.uuid, &rcv->resourceId.nmosId.uuid) )
			{
				break;
			}
			rcv = rcv->next;
		}

		if(rcv == NULL)
		{
			return cmnHttpRestError(mhc, WEB_RES_NOT_FOUND, "ID not found for receiver");
		}
	}

	if(rcv)
	{
		index += _nmosConstraintsHander((char *)mhc->data+index, sizeof(mhc->data), &rcv->constraints, 0);
	}

	index += snprintf((char *)mhc->data+index, sizeof(mhc->data)-index, "]" );

	mhc->contentLength = (unsigned short)index;
	mhc->dataSendIndex = 0;
	
	mhc->httpStatusCode = WEB_RES_REQUEST_OK;
	return EXIT_SUCCESS;
}

static const ApiAccessPoint	__apConnSingleReceiversConstraints =
{
	type 	: 	2,
	name	: 	NMOS_CONN_URL_STR_CONSTRAINTS,
	callback :	_connSingleReceiveConstraintsHander,
	
	child		:	NULL,
	next		:	&__apConnSingleReceiversStaged
};



static char _connSingleReceiversHander(ExtHttpConn  *mhc, void *data)
{
	int index = 0;
	int i = 0;
//	const ApiAccessPoint	*apiAp = (const ApiAccessPoint *)data;
	MuxNmosDevice	*dev = mhc->nodeInfo->device;
	MuxNmosReceiver	*rcv = dev->receivers;

	EXT_ASSERT(("node is null"), mhc->nodeInfo!=NULL);
	EXT_ASSERT(("device is null"), dev!=NULL);

	if(!UUID_IS_NULL(&mhc->apiReq.uuid) )
	{
		while(rcv)
		{
			if( extUuidEqual(&mhc->apiReq.uuid, &rcv->resourceId.nmosId.uuid) )
			{
				return extNmosRootApHander(mhc, data);
			}
			rcv = rcv->next;
		}
		
		return cmnHttpRestError(mhc, WEB_RES_NOT_FOUND, "ID not found for receiver");
	}

	index += snprintf((char *)mhc->data+index, sizeof(mhc->data)-index, "[" );

	if(rcv)
	{
		while(rcv)
		{
			index += snprintf((char *)mhc->data+index, sizeof(mhc->data)-index, "%s\"%s\"", (i==0)?"":",",  extUuidToString(&rcv->resourceId.nmosId.uuid) );

			rcv = rcv->next;
			i++;
		}
	}

	index += snprintf((char *)mhc->data+index, sizeof(mhc->data)-index, "]" );

	mhc->contentLength = (unsigned short)index;
	mhc->dataSendIndex = 0;
	
	mhc->httpStatusCode = WEB_RES_REQUEST_OK;
	return EXIT_SUCCESS;
}

static const ApiAccessPoint	__apConnSingleReceiver =
{
	type 	: 	2,
	name	: 	NMOS_NODE_URL_STR_RECEIVERS,
	callback :	_connSingleReceiversHander,
	
	child		:	&__apConnSingleReceiversConstraints,
	next		:	NULL
};


/******** URIs in single/senders  ************/
/* content_type of SDP is text/plain?? */
static char _connSingleSenderTransportFileHander(ExtHttpConn  *mhc, void *data)
{
	int index = 0;
//	const ApiAccessPoint	*apiAp = (const ApiAccessPoint *)data;
	MuxNmosDevice	*dev = mhc->nodeInfo->device;
	MuxNmosSender	*snd = dev->senders;

	EXT_ASSERT(("node is null"), mhc->nodeInfo!=NULL);
	EXT_ASSERT(("device is null"), dev!=NULL);


	if(!UUID_IS_NULL(&mhc->apiReq.uuid) )
	{
		while(snd)
		{
			if( extUuidEqual(&mhc->apiReq.uuid, &snd->resourceId.nmosId.uuid) )
			{
				break;
			}
			snd = snd->next;
		}

		if(snd == NULL)
		{
			return cmnHttpRestError(mhc, WEB_RES_NOT_FOUND, "ID not found for sender");
		}
	}

	index += extNmosSdpMediaHander((char *)mhc->data+index, sizeof(mhc->data)-index, snd);

	mhc->contentLength = (unsigned short)index;
	mhc->dataSendIndex = 0;
	
	mhc->httpStatusCode = WEB_RES_REQUEST_OK;
	return EXIT_SUCCESS;
}


static const ApiAccessPoint	__apConnSingleSendersTransportfile =
{
	type 	: 	2,
	name	: 	NMOS_CONN_URL_STR_TRANSPORT_FILE,
	callback :	_connSingleSenderTransportFileHander,
	
	child		:	NULL,
	next		:	NULL
};


static char _connSingleSenderActiveHander(ExtHttpConn  *mhc, void *data)
{
	int index = 0;
//	const ApiAccessPoint	*apiAp = (const ApiAccessPoint *)data;
	MuxNmosDevice	*dev = mhc->nodeInfo->device;
	MuxNmosSender	*snd = dev->senders;
	unsigned short srcPort, destPort;

	EXT_ASSERT(("node is null"), mhc->nodeInfo!=NULL);
	EXT_ASSERT(("device is null"), dev!=NULL);


	if(!UUID_IS_NULL(&mhc->apiReq.uuid) )
	{
		while(snd)
		{
			if( extUuidEqual(&mhc->apiReq.uuid, &snd->resourceId.nmosId.uuid) )
			{
				break;
			}
			snd = snd->next;
		}

		if(snd == NULL)
		{
			return cmnHttpRestError(mhc, WEB_RES_NOT_FOUND, "ID not found for sender");
		}
	}


	index += snprintf((char *)mhc->data+index, sizeof(mhc->data)-index, "{");

	/* resource core fields */
	if(snd->subscriber)
	{
		index += snprintf((char *)mhc->data+index, sizeof(mhc->data)-index, "\""NMOS_LABEL_RECEIVER_ID"\":\"%s\",", 
			extUuidToString(&snd->subscriber->resourceId.nmosId.uuid));
	}
	else
	{
		index += snprintf((char *)mhc->data+index, sizeof(mhc->data)-index, "\""NMOS_LABEL_RECEIVER_ID"\":null," );
	}

	index += snprintf((char *)mhc->data+index, sizeof(mhc->data)-index, "\""NMOS_LABEL_MASTER_EANBLE"\":%s,", NMOS_LABEL_TRUE);
	/*activation */
	index += snprintf((char *)mhc->data+index, sizeof(mhc->data)-index, "\""NMOS_LABEL_ACTIVATION"\":{");
	index += snprintf((char *)mhc->data+index, sizeof(mhc->data)-index, "\""NMOS_LABEL_MODE"\":\"%s\",", NMOS_FIND_CONN_ACTIVATE(NMOS_CONN_ACTIVATE_T_IMMEDIATE));
	index += snprintf((char *)mhc->data+index, sizeof(mhc->data)-index, "\""NMOS_LABEL_REQUESTED_TIME"\":\"%"FOR_U32":%d\",",  sys_now(), 0 );
	index += snprintf((char *)mhc->data+index, sizeof(mhc->data)-index, "\""NMOS_LABEL_ACTIVATION_TIME"\":\"%"FOR_U32":%d\"},", sys_now(), 0);

	/* transport_params */
	index += snprintf((char *)mhc->data+index, sizeof(mhc->data)-index, "\""NMOS_LABEL_TRANSPORT_PARAMS"\":[{");

	index += snprintf((char *)mhc->data+index, sizeof(mhc->data), "\""NMOS_LABEL_SOURCE_IP"\":{\"enum\":[\"%s\"]},", EXT_LWIP_IPADD_TO_STR(&snd->device->node->runCfg->local.ip) );
	index += snprintf((char *)mhc->data+index, sizeof(mhc->data), "\""NMOS_LABEL_DEST_IP"\":{\"enum\":[\"%s\"]},", EXT_LWIP_IPADD_TO_STR(&snd->device->node->runCfg->dest.ip) );

	switch (snd->flow->format)
	{
		case NMOS_SOURCE_FORMAT_AUDIO:
			srcPort = snd->device->node->runCfg->local.aport;
			destPort = snd->device->node->runCfg->dest.aport;
			break;
		case NMOS_SOURCE_FORMAT_VIDEO:
			srcPort = snd->device->node->runCfg->local.vport;
			destPort = snd->device->node->runCfg->dest.vport;
			break;
		case NMOS_SOURCE_FORMAT_DATA:
		default:	
			srcPort = snd->device->node->runCfg->local.vport;
			destPort = snd->device->node->runCfg->dest.vport;
			break;
	}
	
	index += snprintf((char *)mhc->data+index, sizeof(mhc->data), "\""NMOS_LABEL_SOURCE_PORT"\":%d,", srcPort);
	index += snprintf((char *)mhc->data+index, sizeof(mhc->data), "\""NMOS_LABEL_DEST_PORT"\":%d,", destPort );

	index += snprintf((char *)mhc->data+index, sizeof(mhc->data), "\""NMOS_LABEL_FEC_ENABLED"\":%s,", NMOS_LABEL_FALSE);

	index += snprintf((char *)mhc->data+index, sizeof(mhc->data), "\""NMOS_LABEL_RTCP_ENABLED"\":%s,", NMOS_LABEL_FALSE);
	index += snprintf((char *)mhc->data+index, sizeof(mhc->data), "\""NMOS_LABEL_RTP_ENABLED"\":%s", NMOS_LABEL_TRUE);

	index += snprintf((char *)mhc->data+index, sizeof(mhc->data)-index, "}]" ); /* transport_params */

	index += snprintf((char *)mhc->data+index, sizeof(mhc->data)-index, "}" );

	mhc->contentLength = (unsigned short)index;
	mhc->dataSendIndex = 0;
	
	mhc->httpStatusCode = WEB_RES_REQUEST_OK;
	return EXIT_SUCCESS;
}


static const ApiAccessPoint	__apConnSingleSendersActive =
{
	type 	: 	2,
	name	: 	NMOS_CONN_URL_STR_ACTIVE,
	callback :	_connSingleSenderActiveHander,
	
	child		:	NULL,
	next		:	&__apConnSingleSendersTransportfile
};


static char _connSingleSenderStageHander(ExtHttpConn  *mhc, void *data)
{
	int index = 0;
//	const ApiAccessPoint	*apiAp = (const ApiAccessPoint *)data;
	MuxNmosDevice	*dev;// = mhc->nodeInfo->device;
	MuxNmosSender	*snd;// = dev->senders;
	unsigned short srcPort, destPort;
	TRACE();

	EXT_ASSERT(("node is null"), mhc !=NULL && mhc->nodeInfo!=NULL);
	dev=mhc->nodeInfo->device;
	EXT_ASSERT(("device is null"), dev!=NULL);
	snd = dev->senders;


	if(!UUID_IS_NULL(&mhc->apiReq.uuid) )
	{
		while(snd)
		{
			if( extUuidEqual(&mhc->apiReq.uuid, &snd->resourceId.nmosId.uuid) )
			{
				break;
			}
			snd = snd->next;
		}

		if(snd == NULL)
		{
			return cmnHttpRestError(mhc, WEB_RES_NOT_FOUND, "ID not found for sender");
		}
	}


	index += snprintf((char *)mhc->data+index, sizeof(mhc->data)-index, "{");

	/* resource core fields */
	if(snd->subscriber)
	{
		index += snprintf((char *)mhc->data+index, sizeof(mhc->data)-index, "\""NMOS_LABEL_RECEIVER_ID"\":\"%s\",", 
			extUuidToString(&snd->subscriber->resourceId.nmosId.uuid));
	}
	else
	{
		index += snprintf((char *)mhc->data+index, sizeof(mhc->data)-index, "\""NMOS_LABEL_RECEIVER_ID"\":null," );
	}
	index += snprintf((char *)mhc->data+index, sizeof(mhc->data)-index, "\""NMOS_LABEL_MASTER_EANBLE"\":%s,", NMOS_LABEL_TRUE);
	index += snprintf((char *)mhc->data+index, sizeof(mhc->data)-index, "\""NMOS_LABEL_ACTIVATION"\":{\""NMOS_LABEL_MODE"\":%s,\""NMOS_LABEL_REQUESTED_TIME"\":%s,\""NMOS_LABEL_ACTIVATION_TIME"\":%s},", 
		NMOS_LABEL_TRUE, "null", "null");

	/* transport_params */
	index += snprintf((char *)mhc->data+index, sizeof(mhc->data)-index, "\""NMOS_LABEL_TRANSPORT_PARAMS"\":[{");

	index += snprintf((char *)mhc->data+index, sizeof(mhc->data), "\""NMOS_LABEL_SOURCE_IP"\":{\"enum\":[\"%s\"]},", EXT_LWIP_IPADD_TO_STR(&snd->device->node->runCfg->local.ip) );
	index += snprintf((char *)mhc->data+index, sizeof(mhc->data), "\""NMOS_LABEL_DEST_IP"\":{\"enum\":[\"%s\"]},", EXT_LWIP_IPADD_TO_STR(&snd->device->node->runCfg->dest.ip) );

	switch (snd->flow->format)
	{
		case NMOS_SOURCE_FORMAT_AUDIO:
			srcPort = snd->device->node->runCfg->local.aport;
			destPort = snd->device->node->runCfg->dest.aport;
			break;
		case NMOS_SOURCE_FORMAT_VIDEO:
			srcPort = snd->device->node->runCfg->local.vport;
			destPort = snd->device->node->runCfg->dest.vport;
			break;
		case NMOS_SOURCE_FORMAT_DATA:
		default:	
			srcPort = snd->device->node->runCfg->local.vport;
			destPort = snd->device->node->runCfg->dest.vport;
			break;
	}
	
	index += snprintf((char *)mhc->data+index, sizeof(mhc->data), "\""NMOS_LABEL_SOURCE_PORT"\":%d,", srcPort);
	index += snprintf((char *)mhc->data+index, sizeof(mhc->data), "\""NMOS_LABEL_DEST_PORT"\":%d,", destPort );

	index += snprintf((char *)mhc->data+index, sizeof(mhc->data), "\""NMOS_LABEL_FEC_ENABLED"\":%s,", NMOS_LABEL_FALSE);

	index += snprintf((char *)mhc->data+index, sizeof(mhc->data), "\""NMOS_LABEL_RTCP_ENABLED"\":%s,", NMOS_LABEL_FALSE);
	index += snprintf((char *)mhc->data+index, sizeof(mhc->data), "\""NMOS_LABEL_RTP_ENABLED"\":%s", NMOS_LABEL_TRUE);

	index += snprintf((char *)mhc->data+index, sizeof(mhc->data)-index, "}]" ); /* transport_params */

	index += snprintf((char *)mhc->data+index, sizeof(mhc->data)-index, "}" );

	mhc->contentLength = (unsigned short)index;
	mhc->dataSendIndex = 0;
	
	mhc->httpStatusCode = WEB_RES_REQUEST_OK;
	return EXIT_SUCCESS;
}


static const ApiAccessPoint	__apConnSingleSendersStaged =
{
	type 	: 	2,
	name	: 	NMOS_CONN_URL_STR_STAGED,
	callback :	_connSingleSenderStageHander,
	
	child		:	NULL,
	next		:	&__apConnSingleSendersActive
};


static char _connSingleSenderConstraintsHander(ExtHttpConn  *mhc, void *data)
{
	int index = 0;
//	const ApiAccessPoint	*apiAp = (const ApiAccessPoint *)data;
	MuxNmosDevice	*dev = mhc->nodeInfo->device;
	MuxNmosSender	*snd = dev->senders;

	EXT_ASSERT(("node is null"), mhc->nodeInfo!=NULL);
	EXT_ASSERT(("device is null"), dev!=NULL);


	if(!UUID_IS_NULL(&mhc->apiReq.uuid) )
	{
		while(snd)
		{
			if( extUuidEqual(&mhc->apiReq.uuid, &snd->resourceId.nmosId.uuid) )
			{
				break;
			}
			snd = snd->next;
		}

		if(snd == NULL)
		{
			return cmnHttpRestError(mhc, WEB_RES_NOT_FOUND, "ID not found for sender");
		}
	}

	index += snprintf((char *)mhc->data+index, sizeof(mhc->data)-index, "[" );
	if(snd)
	{
		index += _nmosConstraintsHander((char *)mhc->data+index, sizeof(mhc->data), &snd->constraints, 1);
	}

	index += snprintf((char *)mhc->data+index, sizeof(mhc->data)-index, "]" );

	mhc->contentLength = (unsigned short)index;
	mhc->dataSendIndex = 0;
	
	mhc->httpStatusCode = WEB_RES_REQUEST_OK;
	return EXIT_SUCCESS;
}



static const ApiAccessPoint	__apConnSingleSendersConstraints =
{
	type 	: 	2,
	name	: 	NMOS_CONN_URL_STR_CONSTRAINTS,
	callback :	_connSingleSenderConstraintsHander,
	
	child		:	NULL,
	next		:	&__apConnSingleSendersStaged
};



static char _connSingleSendersHander(ExtHttpConn  *mhc, void *data)
{
	int index = 0;
	int i = 0;
//	const ApiAccessPoint	*apiAp = (const ApiAccessPoint *)data;
	MuxNmosDevice	*dev = mhc->nodeInfo->device;
	MuxNmosSender	*snd = dev->senders;

	EXT_ASSERT(("node is null"), mhc->nodeInfo!=NULL);
	EXT_ASSERT(("device is null"), dev!=NULL);

	index += snprintf((char *)mhc->data+index, sizeof(mhc->data)-index, "[" );

	if(!UUID_IS_NULL(&mhc->apiReq.uuid) )
	{
		while(snd)
		{
			if( extUuidEqual(&mhc->apiReq.uuid, &snd->resourceId.nmosId.uuid) )
			{
				return extNmosRootApHander(mhc, data);
			}
			snd = snd->next;
		}
		
		return cmnHttpRestError(mhc, WEB_RES_NOT_FOUND, "ID not found for sender");
	}

	if(snd)
	{
		while(snd)
		{
			index += snprintf((char *)mhc->data+index, sizeof(mhc->data)-index, "%s\"%s\"", (i==0)?"":",", extUuidToString(&snd->resourceId.nmosId.uuid) );

			snd = snd->next;
			i++;
		}
	}

	index += snprintf((char *)mhc->data+index, sizeof(mhc->data)-index, "]" );

	mhc->contentLength = (unsigned short)index;
	mhc->dataSendIndex = 0;
	
	mhc->httpStatusCode = WEB_RES_REQUEST_OK;
	return EXIT_SUCCESS;
}


static const ApiAccessPoint	__apConnSingleSenders =
{
	type 	: 	2,
	name	: 	NMOS_NODE_URL_STR_SENDERS,
	callback :	_connSingleSendersHander,
	
	child		:	&__apConnSingleSendersConstraints,
	next		:	&__apConnSingleReceiver
};


static const ApiAccessPoint	_apConnSingle =
{
	type 	: 	3,
	name	: 	NMOS_CONN_URL_STR_SINGLE,
	callback :	extNmosRootApHander,
	
	child		:	&__apConnSingleSenders,
	next		:	NULL
};

/******** URIs in bulk  ************/


static char _connBulkReceiversHander(ExtHttpConn  *mhc, void *data)
{
#if 0
	int index = 0;
	int i = 0;
//	const ApiAccessPoint	*apiAp = (const ApiAccessPoint *)data;
	MuxNmosDevice	*dev = mhc->nodeInfo->device;
	MuxNmosReceiver	*rcv = dev->receivers;
	MuxNmosSender	*snd = dev->senders;
#endif

	if(mhc->method == HTTP_METHOD_GET )
	{
		cmnHttpRestError(mhc, WEB_RES_METHOD_NA, "GET is not allowed");
		return EXIT_SUCCESS;
	}

#if 0
	EXT_ASSERT(("node is null"), mhc->nodeInfo!=NULL);
	EXT_ASSERT(("device is null"), dev!=NULL);

	index += snprintf((char *)mhc->data+index, sizeof(mhc->data)-index, "[{" );

	index += __nmosPrintResource((char *)mhc->data+index, sizeof(mhc->data)-index, &dev->resourceId);
	
	index += snprintf((char *)mhc->data+index, sizeof(mhc->data)-index, "\""NMOS_LABEL_NODE_ID"\":\"%s\",", extUuidToString(&mhc->nodeInfo->resourceId.nmosId.uuid));
	index += snprintf((char *)mhc->data+index, sizeof(mhc->data)-index, "\""NMOS_LABEL_TYPE"\":\"%s\",", NMOS_URN_DEVICE_TYPE_GENERIC );
	
	/* Controls [] */
	index += snprintf((char *)mhc->data+index, sizeof(mhc->data)-index, "\""NMOS_LABEL_CONTROLS"\":[");
	index += snprintf((char *)mhc->data+index, sizeof(mhc->data)-index, "{\""NMOS_LABEL_HREF"\":\"http://%s:%d"NMOS_API_URI_CONNECTION"/"NMOS_API_VERSION_10 "\",", 
		EXT_LWIP_IPADD_TO_STR(&mhc->nodeInfo->runCfg->local.ip), mhc->nodeInfo->runCfg->httpPort );
	index += snprintf((char *)mhc->data+index, sizeof(mhc->data)-index, "\""NMOS_LABEL_TYPE"\":\"%s\"}", NMOS_URN_CONTROL_SRCTRL );
	index += snprintf((char *)mhc->data+index, sizeof(mhc->data)-index, "],");

	if(rcv)
	{
		index += snprintf((char *)mhc->data+index, sizeof(mhc->data)-index, "\"" NMOS_LABEL_RECEIVERS "\":[");
		while(rcv)
		{
			index += snprintf((char *)mhc->data+index, sizeof(mhc->data)-index, "%s\"%s\"", (i==0)?"":",",  extUuidToString(&rcv->resourceId.nmosId.uuid) );

			rcv = rcv->next;
			i++;
		}
		index += snprintf((char *)mhc->data+index, sizeof(mhc->data)-index, "],");

	}
	else
	{
		index += snprintf((char *)mhc->data+index, sizeof(mhc->data)-index, "\""NMOS_LABEL_RECEIVERS"\":[]," );
	}

	i=0;
	if(snd)
	{
		index += snprintf((char *)mhc->data+index, sizeof(mhc->data)-index, "\"" NMOS_LABEL_SENDERS "\":[");
		while(snd)
		{
			index += snprintf((char *)mhc->data+index, sizeof(mhc->data)-index, "%s\"%s\"", (i==0)?"":",", extUuidToString(&snd->resourceId.nmosId.uuid) );

			snd = snd->next;
			i++;
		}
		index += snprintf((char *)mhc->data+index, sizeof(mhc->data)-index, "]");
	}
	else
	{
		index += snprintf((char *)mhc->data+index, sizeof(mhc->data)-index, "\""NMOS_LABEL_SENDERS"\":[]" );
	}

	index += snprintf((char *)mhc->data+index, sizeof(mhc->data)-index, "}]" );

	mhc->contentLength = (unsigned short)index;
	mhc->dataSendIndex = 0;
	
	mhc->httpStatusCode = WEB_RES_REQUEST_OK;
#else
	extNmosNodeDumpHander(mhc, data);
#endif

	return EXIT_SUCCESS;
}


static char _connBulkSendersHander(ExtHttpConn  *mhc, void *data)
{
#if 0
	int index = 0;
	int i = 0;
//	const ApiAccessPoint	*apiAp = (const ApiAccessPoint *)data;
	MuxNmosDevice	*dev = mhc->nodeInfo->device;
	MuxNmosReceiver	*rcv = dev->receivers;
	MuxNmosSender	*snd = dev->senders;
#endif

	if(mhc->method == HTTP_METHOD_GET )
	{
		cmnHttpRestError(mhc, WEB_RES_METHOD_NA, "GET is not allowed");
		return EXIT_SUCCESS;
	}

	extNmosNodeDumpHander(mhc, data);

	return EXIT_SUCCESS;
}


static const ApiAccessPoint	__apConnBulkReceivers =
{
	type 	: 	2,
	name	: 	NMOS_NODE_URL_STR_RECEIVERS,
	callback :	_connBulkReceiversHander,
	
	child		:	NULL,
	next		:	NULL
};

static const ApiAccessPoint	__apConnBulkSenders =
{
	type 	: 	2,
	name	: 	NMOS_NODE_URL_STR_SENDERS,
	callback :	_connBulkSendersHander,
	
	child		:	NULL,
	next		:	&__apConnBulkReceivers
};



static const ApiAccessPoint	_apConnBulk =
{
	type 	: 	2,
	name	: 	NMOS_CONN_URL_STR_BULK,
	callback :	extNmosRootApHander,
	
	child		:	&__apConnBulkSenders,
	next		:	&_apConnSingle
};


/* root access point of CONN API */
const ApiAccessPoint	apConnRoot =
{
	type 	: 	1,
	name	: 	"/",
	callback :	extNmosRootApHander,
	
	child		:	&_apConnBulk,
	next		:	NULL
};

#endif

