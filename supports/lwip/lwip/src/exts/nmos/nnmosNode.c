/*
* basic operation of Node API
*/


#include "lwipExt.h"

#include "http.h"
#include "jsmn.h"

#if 0
/* based on request in MHC, and print response with node info */
static char	muxNmosRestResponse(MuxNmosNode	*node, MuxHttpConn *mhc)
{

	return EXIT_SUCCESS;
}
#endif

static int __nmosPrintResource(char *data, unsigned int size, MuxNmosResource *resourceId)
{
	int index = 0;

	index += snprintf(data+index, size-index, "\""NMOS_LABEL_ID"\":\"%s\",", muxUuidToString(&resourceId->nmosId.uuid));
	index += snprintf(data+index, size-index, "\""NMOS_LABEL_VERSION"\":\"%u:%u\",", resourceId->nmosId.version.seconds, resourceId->nmosId.version.nanoSeconds);
	index += snprintf(data+index, size-index, "\""NMOS_LABEL_LABEL"\":\"%s\",", resourceId->label );
	index += snprintf(data+index, size-index, "\""NMOS_LABEL_DESCRIPTION"\":\"%s\",", resourceId->description );
	index += snprintf(data+index, size-index, "\""NMOS_LABEL_TAGS"\":{%s},", (resourceId->tags==NULL)?"":(char *)resourceId->tags);
	
	return index;
}

static int _nmosReceiverOneHander(char *data, unsigned int size, MuxNmosReceiver *rcv )
{
	int index = 0;
	
	index += snprintf(data+index, size-index, "{");
	index += __nmosPrintResource(data+index, size-index, &rcv->resourceId);

	MUX_ASSERT(("Receiver->device is null"), rcv->device != NULL);

	/* resource core fields */
	index += snprintf(data+index, size-index, "\""NMOS_LABEL_DEVICE_ID"\":\"%s\",", muxUuidToString(&rcv->device->resourceId.nmosId.uuid));
	index += snprintf(data+index, size-index, "\""NMOS_LABEL_TRANSPORT"\":\"%s\",", NMOS_FIND_TRANSPORT(rcv->transport) );
	index += snprintf(data+index, size-index, "\""NMOS_LABEL_INTERFACE_BINDS"\":[\"%s\"],", rcv->interface_binding );
	index += snprintf(data+index, size-index, "\""NMOS_LABEL_FORMAT"\":\"%s\",", NMOS_FIND_SOURCE_FORMAT(rcv->format) );

	MUX_ASSERT(("receiver-cap is not null"), rcv->caps!= NULL);

	index += snprintf(data+index, size-index, "\""NMOS_LABEL_CAPS"\":{\""NMOS_LABEL_MEDIA_TYPES"\":[\"%s\"]},", rcv->caps->type );

	index += snprintf(data+index, size-index, "\""NMOS_LABEL_SUBSCRIPTION"\":{\""NMOS_LABEL_RECEIVER_ID"\":");
	if(rcv->subscriber)
	{
		index += snprintf(data+index, size-index, "\"%s\"", muxUuidToString(&rcv->subscriber->resourceId.nmosId.uuid) );
	}
	else
	{
		index += snprintf(data+index, size-index, "%s", "null" );
	}
	index += snprintf(data+index, size-index, ",\""NMOS_LABEL_ACTIVE"\":%s}", (rcv->enabled)?"true":"false" );

	index += snprintf(data+index, size-index, "}");

	return index;
}

static char _nmosReceiversHander(MuxHttpConn  *mhc, void *data)
{
	int index = 0;
	int srcIndex = 0;
//	const ApiAccessPoint	*apiAp = (const ApiAccessPoint *)data;
	MuxNmosDevice	*dev = mhc->nodeInfo->device;
	MuxNmosReceiver	*rcv = dev->receivers;

	MUX_ASSERT(("node is null"), mhc->nodeInfo!=NULL);
	MUX_ASSERT(("device is null"), dev!=NULL);

	if( MUX_IS_TX(mhc->nodeInfo->runCfg) )
	{
		index += snprintf((char *)mhc->data+index, sizeof(mhc->data)-index, "[]" );
		mhc->contentLength = (unsigned short)index;
		mhc->dataSendIndex = 0;
	
		mhc->httpStatusCode = WEB_RES_REQUEST_OK;
		return EXIT_SUCCESS;
	}
	
	MUX_ASSERT(("receiver is null for TX"), rcv!=NULL);
	MUX_ASSERT(("receiver->device is null for TX"), rcv->device !=NULL);

	if(!UUID_IS_NULL(&mhc->apiReq.uuid) )
	{
		while(rcv)
		{
			if( muxUuidEqual(&mhc->apiReq.uuid, &rcv->resourceId.nmosId.uuid) )
			{
				index += _nmosReceiverOneHander((char *)mhc->data+index, sizeof(mhc->data), rcv);
				break;
			}
			rcv = rcv->next;
		}
		if(!rcv)
		{
			return muxHttpRestError(mhc, WEB_RES_NOT_FOUND, "ID not found for receiver");
		}
	}
	else 
	{

		index += snprintf((char *)mhc->data+index, sizeof(mhc->data)-index, "[" );

		while(rcv)
		{
			if(srcIndex != 0)
			{
				index += snprintf((char *)mhc->data+index, sizeof(mhc->data)-index, "," );
			}
			index += _nmosReceiverOneHander((char *)mhc->data+index, sizeof(mhc->data), rcv );
			
			rcv = rcv->next;
			srcIndex++;
		}
		
		index += snprintf((char *)mhc->data+index, sizeof(mhc->data)-index, "]" );
	}
	
	mhc->contentLength = (unsigned short)index;
	mhc->dataSendIndex = 0;
	
	mhc->httpStatusCode = WEB_RES_REQUEST_OK;
	return EXIT_SUCCESS;
}


static int _nmosSenderOneHander(char *data, unsigned int size, MuxNmosSender *snd )
{
	int index = 0;
	
	index += snprintf(data+index, size-index, "{");
	index += __nmosPrintResource(data+index, size-index, &snd->resourceId);

	MUX_ASSERT(("Sender->device is null"), snd->device != NULL);
	MUX_ASSERT(("Sender->flow is null"), snd->flow != NULL);

	/* resource core fields */
	index += snprintf(data+index, size-index, "\""NMOS_LABEL_DEVICE_ID"\":\"%s\",", muxUuidToString(&snd->device->resourceId.nmosId.uuid));
	index += snprintf(data+index, size-index, "\""NMOS_LABEL_FLOW_ID"\":\"%s\",", muxUuidToString(&snd->flow->resourceId.nmosId.uuid));
	index += snprintf(data+index, size-index, "\""NMOS_LABEL_TRANSPORT"\":\"%s\",", NMOS_FIND_TRANSPORT(snd->transport) );
	index += snprintf(data+index, size-index, "\""NMOS_LABEL_MANIFEST_HREF"\":\"http://%s:%d"NMOS_API_URI_CONNECTION"/v1.0/single/senders/%s/transportfile\",", 
		MUX_LWIP_IPADD_TO_STR(&snd->device->node->runCfg->local.ip), snd->device->node->runCfg->httpPort, muxUuidToString(&snd->resourceId.nmosId.uuid) );

	index += snprintf(data+index, size-index, "\""NMOS_LABEL_INTERFACE_BINDS"\":[\"%s\"],", snd->interface_binding );

	index += snprintf(data+index, size-index, "\""NMOS_LABEL_SUBSCRIPTION"\":{\""NMOS_LABEL_RECEIVER_ID"\":");
	if(snd->subscriber)
	{
		index += snprintf(data+index, size-index, "\"%s\"", muxUuidToString(&snd->subscriber->resourceId.nmosId.uuid) );
	}
	else
	{
		index += snprintf(data+index, size-index, "%s", "null" );
	}
	index += snprintf(data+index, size-index, ",\""NMOS_LABEL_ACTIVE"\":%s}", (snd->enabled)?"true":"false" );

	index += snprintf(data+index, size-index, "}");

	return index;
}

static char _nmosSendersHander(MuxHttpConn  *mhc, void *data)
{
	int index = 0;
	int srcIndex = 0;
//	const ApiAccessPoint	*apiAp = (const ApiAccessPoint *)data;
	MuxNmosDevice	*dev = mhc->nodeInfo->device;
	MuxNmosSender	*snd = dev->senders;

	MUX_ASSERT(("node is null"), mhc->nodeInfo!=NULL);
	MUX_ASSERT(("device is null"), dev!=NULL);

	if(!MUX_IS_TX(mhc->nodeInfo->runCfg) )
	{
		index += snprintf((char *)mhc->data+index, sizeof(mhc->data)-index, "[]" );
		mhc->contentLength = (unsigned short)index;
		mhc->dataSendIndex = 0;
	
		mhc->httpStatusCode = WEB_RES_REQUEST_OK;
		return EXIT_SUCCESS;
	}
	
	MUX_ASSERT(("sender is null for TX"), snd!=NULL);
	MUX_ASSERT(("sender->device is null for TX"), snd->device !=NULL);

	if(!UUID_IS_NULL(&mhc->apiReq.uuid) )
	{
		while(snd)
		{
			if( muxUuidEqual(&mhc->apiReq.uuid, &snd->resourceId.nmosId.uuid) )
			{
				index += _nmosSenderOneHander((char *)mhc->data+index, sizeof(mhc->data), snd );
				break;
			}
			snd = snd->next;
		}
		if(!snd)
		{
			return muxHttpRestError(mhc, WEB_RES_NOT_FOUND, "ID not found for sender");
		}
	}
	else 
	{

		index += snprintf((char *)mhc->data+index, sizeof(mhc->data)-index, "[" );

		while(snd)
		{
			if(srcIndex != 0)
			{
				index += snprintf((char *)mhc->data+index, sizeof(mhc->data)-index, "," );
			}
			index += _nmosSenderOneHander((char *)mhc->data+index, sizeof(mhc->data), snd );
			
			snd = snd->next;
			srcIndex++;
		}
		
		index += snprintf((char *)mhc->data+index, sizeof(mhc->data)-index, "]" );
	}
	
	mhc->contentLength = (unsigned short)index;
	mhc->dataSendIndex = 0;
	
	mhc->httpStatusCode = WEB_RES_REQUEST_OK;
	return EXIT_SUCCESS;
}


static int _nmosFlowOneHander(char *data, unsigned int size, MuxNmosFlow *flow )
{
	int index = 0;
	
	index += snprintf(data+index, size-index, "{");
	index += __nmosPrintResource(data+index, size-index, &flow->resourceId);

	/* resource core fields */
	index += snprintf(data+index, size-index, "\""NMOS_LABEL_DEVICE_ID"\":\"%s\",", muxUuidToString(&flow->source->device->resourceId.nmosId.uuid));
	index += snprintf(data+index, size-index, "\""NMOS_LABEL_SOURCE_ID"\":\"%s\",", muxUuidToString(&flow->source->resourceId.nmosId.uuid));
	index += snprintf(data+index, size-index, "\""NMOS_LABEL_FORMAT"\":\"%s\",", NMOS_FIND_SOURCE_FORMAT(flow->format) );
	index += snprintf(data+index, size-index, "\""NMOS_LABEL_MEDIA_TYPE"\":\"%s\",", flow->mediaType );
	index += snprintf(data+index, size-index, "\""NMOS_LABEL_PARENTS"\":[]" );

	if(flow->format == NMOS_SOURCE_FORMAT_VIDEO)
	{
		int i = 0;
		const NmosVideoComponent *cmp = flow->components;
		
		index += snprintf(data+index, size-index, ",\""NMOS_LABEL_FRAME_HEIGHT"\":%d,", flow->frameHeight);
		index += snprintf(data+index, size-index, "\""NMOS_LABEL_FRAME_WIDTH"\":%d,", flow->frameWidth);

		index += snprintf(data+index, size-index, "\""NMOS_LABEL_INTERLACE_MODE"\":\"%s\",", NMOS_FIND_INTERLACE_MODE(flow->interlaceMode) );
		index += snprintf(data+index, size-index, "\""NMOS_LABEL_COLOR_SPACE"\":\"%s\",", NMOS_FIND_COLOR_SPACE(flow->colorspace) );
		index += snprintf(data+index, size-index, "\""NMOS_LABEL_TRANSFER_CHARC"\":\"%s\"", NMOS_FIND_TRANSFER_CHARC(flow->transferCharacter) );

		if(cmp )
		{
			index += snprintf(data+index, size-index, ",\""NMOS_LABEL_COMPONENTS"\":[" );
		}
		while(cmp)
		{
			index += snprintf(data+index, size-index, "%s{\""NMOS_LABEL_NAME"\":\"%s\",", (i==0)?"":",", NMOS_FIND_VIDEO_COMPONENT(cmp->name));
			index += snprintf(data+index, size-index, "\""NMOS_LABEL_WIDTH"\":%d,", cmp->width);
			index += snprintf(data+index, size-index, "\""NMOS_LABEL_HEIGHT"\":%d,", cmp->height );
			index += snprintf(data+index, size-index, "\""NMOS_LABEL_BIT_DEPTH"\":%d}", cmp->bitDepth );

			cmp = cmp->next;
			i++;
		}
		
		if( i!= 0)
		{
			index += snprintf(data+index, size-index, "]");
		}
	}
	else if(flow->format == NMOS_SOURCE_FORMAT_AUDIO)
	{
		index += snprintf(data+index, size-index, ",\""NMOS_LABEL_BIT_DEPTH"\":%d", flow->bitDepth);
		index += snprintf(data+index, size-index, ",\""NMOS_LABEL_SAMPLE_RATE"\":{\""NMOS_LABEL_NUMERATOR"\":%d}", flow->sampleRate.numberator);
	}
	else if(flow->format == NMOS_SOURCE_FORMAT_DATA)
	{
	}
	else
	{
		MUX_ASSERT(("Invalidate format for FLOW"), 0);
	}
	
	index += snprintf(data+index, size-index, "}");

	return index;
}

static char _nmosFlowsHander(MuxHttpConn  *mhc, void *data)
{
	int index = 0;
	int srcIndex = 0;
//	const ApiAccessPoint	*apiAp = (const ApiAccessPoint *)data;
	MuxNmosDevice	*dev = mhc->nodeInfo->device;
	MuxNmosFlow		*flow = dev->flows;

	MUX_ASSERT(("node is null"), mhc->nodeInfo!=NULL);
	MUX_ASSERT(("device is null"), dev!=NULL);

	if(!MUX_IS_TX(mhc->nodeInfo->runCfg) )
	{
		index += snprintf((char *)mhc->data+index, sizeof(mhc->data)-index, "[]" );
		mhc->contentLength = (unsigned short)index;
		mhc->dataSendIndex = 0;
	
		mhc->httpStatusCode = WEB_RES_REQUEST_OK;
		return EXIT_SUCCESS;
	}
	
	MUX_ASSERT(("flow is null for TX"), flow!=NULL);
	MUX_ASSERT(("flow->device is null for TX"), flow->source->device !=NULL);

	if(!UUID_IS_NULL(&mhc->apiReq.uuid) )
	{
		while(flow)
		{
			if( muxUuidEqual(&mhc->apiReq.uuid, &flow->resourceId.nmosId.uuid) )
			{
				index += _nmosFlowOneHander((char *)mhc->data+index, sizeof(mhc->data), flow );
				break;
			}
			flow = flow->next;
		}
		if(!flow)
		{
			return muxHttpRestError(mhc, WEB_RES_NOT_FOUND, "ID not found for flow");
		}
	}
	else 
	{

		index += snprintf((char *)mhc->data+index, sizeof(mhc->data)-index, "[" );

		while(flow)
		{
			if(srcIndex != 0)
			{
				index += snprintf((char *)mhc->data+index, sizeof(mhc->data)-index, "," );
			}
			index += _nmosFlowOneHander((char *)mhc->data+index, sizeof(mhc->data), flow);
			
			flow = flow->next;
			srcIndex++;
		}
		
		index += snprintf((char *)mhc->data+index, sizeof(mhc->data)-index, "]" );
	}
	printf("source:'%s'"MUX_NEW_LINE, (char *)mhc->data );

	mhc->contentLength = (unsigned short)index;
	mhc->dataSendIndex = 0;
	
	mhc->httpStatusCode = WEB_RES_REQUEST_OK;
	return EXIT_SUCCESS;
}



static int _nmosSourceOneHander(char *data, unsigned int size, MuxNmosSource *src )
{
	int index = 0;
	
	index += snprintf(data+index, size-index, "{");
	index += __nmosPrintResource(data+index, size-index, &src->resourceId);

	/* resource core fields */
	index += snprintf(data+index, size-index, "\""NMOS_LABEL_DEVICE_ID"\":\"%s\",", muxUuidToString(&src->device->resourceId.nmosId.uuid));
	index += snprintf(data+index, size-index, "\""NMOS_LABEL_FORMAT"\":\"%s\",", NMOS_FIND_SOURCE_FORMAT(src->format) );
	index += snprintf(data+index, size-index, "\""NMOS_LABEL_CLOCKS"\":\"%s%d\",", NMOS_LABEL_CLOCK_INDEX, src->clockIndex );
	index += snprintf(data+index, size-index, "\""NMOS_LABEL_PARENTS"\":[]" );

	if(src->format == NMOS_SOURCE_FORMAT_VIDEO)
	{
		index += snprintf(data+index, size-index, ",\""NMOS_LABEL_GRAIN_RATE"\":{\""
			NMOS_LABEL_DENOMINATOR"\":%d,\""NMOS_LABEL_NUMERATOR"\":%d}",
			src->gRate.denominator, src->gRate.numberator);
	}
	else if(src->format == NMOS_SOURCE_FORMAT_AUDIO)
	{
		NmosAudioChannel *audio = src->audioChannels;
		int i = 0;
		index += snprintf(data+index, size-index, ",\""NMOS_LABEL_CHANNELS"\":[");

		while(audio)
		{
			index += snprintf(data+index, size-index, "%s{\""NMOS_LABEL_LABEL"\":\"%s\",", (i==0)?"":",", audio->label );
			index += snprintf(data+index, size-index, "\""NMOS_LABEL_SYMBOL"\":\"%s\"}", NMOS_FIND_CHANNEL_SYMBOL(audio->symbol) );

			audio = audio->next;
			i++;
		}
		index += snprintf(data+index, size-index, "]");
	}
	index += snprintf(data+index, size-index, "}");

	return index;
}

static char _nmosSourcesHander(MuxHttpConn  *mhc, void *data)
{
	int index = 0;
	int srcIndex = 0;
//	const ApiAccessPoint	*apiAp = (const ApiAccessPoint *)data;
	MuxNmosDevice	*dev = mhc->nodeInfo->device;
	MuxNmosSource	*src = dev->sources;

	MUX_ASSERT(("node is null"), mhc->nodeInfo!=NULL);
	MUX_ASSERT(("device is null"), dev!=NULL);

	if(!MUX_IS_TX(mhc->nodeInfo->runCfg) )
	{
		index += snprintf((char *)mhc->data+index, sizeof(mhc->data)-index, "[]" );
		mhc->contentLength = (unsigned short)index;
		mhc->dataSendIndex = 0;
	
		mhc->httpStatusCode = WEB_RES_REQUEST_OK;
		return EXIT_SUCCESS;
	}
	
	MUX_ASSERT(("source is null for TX"), src!=NULL);
	MUX_ASSERT(("source->device is null for TX"), src->device !=NULL);

	if(!UUID_IS_NULL(&mhc->apiReq.uuid) )
	{
		while(src)
		{
			if( muxUuidEqual(&mhc->apiReq.uuid, &src->resourceId.nmosId.uuid) )
			{
				index += _nmosSourceOneHander((char *)mhc->data+index, sizeof(mhc->data), src );
				break;
			}
			src = src->next;
		}
		if(!src)
		{
			return muxHttpRestError(mhc, WEB_RES_NOT_FOUND, "ID not found for source");
		}
	}
	else 
	{
		index += snprintf((char *)mhc->data+index, sizeof(mhc->data)-index, "[" );

		while( src )
		{
			if(srcIndex != 0)
			{
				index += snprintf((char *)mhc->data+index, sizeof(mhc->data)-index, "," );
			}
			index += _nmosSourceOneHander((char *)mhc->data+index, sizeof(mhc->data), src );
			TRACE();
			
			src = src->next;
			srcIndex++;
		}
	
		index += snprintf((char *)mhc->data+index, sizeof(mhc->data)-index, "]" );
	}
	
	printf("source:'%s'"MUX_NEW_LINE, (char *)mhc->data );

	mhc->contentLength = (unsigned short)index;
	mhc->dataSendIndex = 0;
	
	mhc->httpStatusCode = WEB_RES_REQUEST_OK;
	return EXIT_SUCCESS;
}


static char _nmosDeviceHander(MuxHttpConn  *mhc, void *data)
{
	int index = 0;
	int i = 0;
//	const ApiAccessPoint	*apiAp = (const ApiAccessPoint *)data;
	MuxNmosDevice	*dev = mhc->nodeInfo->device;
	MuxNmosReceiver	*rcv = dev->receivers;
	MuxNmosSender	*snd = dev->senders;
	unsigned char isArray = 1;

	MUX_ASSERT(("node is null"), mhc->nodeInfo!=NULL);
	MUX_ASSERT(("device is null"), dev!=NULL);

	if(!UUID_IS_NULL(&mhc->apiReq.uuid) )
	{
		if(!muxUuidEqual(&mhc->apiReq.uuid, &dev->resourceId.nmosId.uuid) )
		{
			return muxHttpRestError(mhc, WEB_RES_NOT_FOUND, "ID not found for device");
		}
		isArray = 0;
	}

	index += snprintf((char *)mhc->data+index, sizeof(mhc->data)-index, "%s{", (isArray)?"[":"" );

	index += __nmosPrintResource((char *)mhc->data+index, sizeof(mhc->data)-index, &dev->resourceId);
	
	index += snprintf((char *)mhc->data+index, sizeof(mhc->data)-index, "\""NMOS_LABEL_NODE_ID"\":\"%s\",", muxUuidToString(&mhc->nodeInfo->resourceId.nmosId.uuid));
	index += snprintf((char *)mhc->data+index, sizeof(mhc->data)-index, "\""NMOS_LABEL_TYPE"\":\"%s\",", NMOS_URN_DEVICE_TYPE_GENERIC );
	
	/* Controls [] */
	index += snprintf((char *)mhc->data+index, sizeof(mhc->data)-index, "\""NMOS_LABEL_CONTROLS"\":[");
	index += snprintf((char *)mhc->data+index, sizeof(mhc->data)-index, "{\""NMOS_LABEL_HREF"\":\"http://%s:%d"NMOS_API_URI_CONNECTION"/"NMOS_API_VERSION_10 "\",", 
		MUX_LWIP_IPADD_TO_STR(&mhc->nodeInfo->runCfg->local.ip), mhc->nodeInfo->runCfg->httpPort );
	index += snprintf((char *)mhc->data+index, sizeof(mhc->data)-index, "\""NMOS_LABEL_TYPE"\":\"%s\"}", NMOS_URN_CONTROL_SRCTRL );
	index += snprintf((char *)mhc->data+index, sizeof(mhc->data)-index, "],");

	if(rcv)
	{
		index += snprintf((char *)mhc->data+index, sizeof(mhc->data)-index, "\"" NMOS_LABEL_RECEIVERS "\":[");
		while(rcv)
		{
			index += snprintf((char *)mhc->data+index, sizeof(mhc->data)-index, "%s\"%s\"", (i==0)?"":",",  muxUuidToString(&rcv->resourceId.nmosId.uuid) );

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
			index += snprintf((char *)mhc->data+index, sizeof(mhc->data)-index, "%s\"%s\"", (i==0)?"":",", muxUuidToString(&snd->resourceId.nmosId.uuid) );

			snd = snd->next;
			i++;
		}
		index += snprintf((char *)mhc->data+index, sizeof(mhc->data)-index, "]");
	}
	else
	{
		index += snprintf((char *)mhc->data+index, sizeof(mhc->data)-index, "\""NMOS_LABEL_SENDERS"\":[]" );
	}

	index += snprintf((char *)mhc->data+index, sizeof(mhc->data)-index, "}%s", (isArray)?"]":"" );

	mhc->contentLength = (unsigned short)index;
	mhc->dataSendIndex = 0;
	
	mhc->httpStatusCode = WEB_RES_REQUEST_OK;
	return EXIT_SUCCESS;
}

static char _nmosSelfHander(MuxHttpConn  *mhc, void *data)
{
	int index = 0;
	const ApiAccessPoint	*apiAp = (const ApiAccessPoint *)data;

	apiAp = apiAp->child;

	index += snprintf((char *)mhc->data+index, sizeof(mhc->data)-index, "{" );

	index += __nmosPrintResource((char *)mhc->data+index, sizeof(mhc->data)-index, &mhc->nodeInfo->resourceId);

	/* api{} */
	index += snprintf((char *)mhc->data+index, sizeof(mhc->data)-index, "\"api\":{");
	index += snprintf((char *)mhc->data+index, sizeof(mhc->data)-index, "\"endpoints\":[{\"host\":\"%s\", \"port\":%d, \"protocol\":\"http\"}],", MUX_LWIP_IPADD_TO_STR(&mhc->nodeInfo->runCfg->local.ip), mhc->nodeInfo->runCfg->httpPort );
	index += snprintf((char *)mhc->data+index, sizeof(mhc->data)-index, "\""NMOS_LABEL_VERSIONS"\":[\"v1.0\", \"v1.1\", \"v1.2\"]" );
	index += snprintf((char *)mhc->data+index, sizeof(mhc->data)-index, "},");

	index += snprintf((char *)mhc->data+index, sizeof(mhc->data)-index, "\""NMOS_LABEL_CAPS"\":{},");/* not defined */
	index += snprintf((char *)mhc->data+index, sizeof(mhc->data)-index, "\""NMOS_LABEL_CLOCKS"\":[{\"name\":\"clk0\",\"ref_type\":\"internal\"}],");
	index += snprintf((char *)mhc->data+index, sizeof(mhc->data)-index, "\"hostname\":\"%s.local\",", mhc->nodeInfo->runCfg->name);
	index += snprintf((char *)mhc->data+index, sizeof(mhc->data)-index, "\""NMOS_LABEL_HREF"\":\"http://%s:%d\",", MUX_LWIP_IPADD_TO_STR(&mhc->nodeInfo->runCfg->local.ip), mhc->nodeInfo->runCfg->httpPort );
	index += snprintf((char *)mhc->data+index, sizeof(mhc->data)-index, "\"interfaces\":[]," );
	index += snprintf((char *)mhc->data+index, sizeof(mhc->data)-index, "\"services\":[]");

	index += snprintf((char *)mhc->data+index, sizeof(mhc->data)-index, "}" );

	mhc->contentLength = (unsigned short)index;
	mhc->dataSendIndex = 0;
	
	mhc->httpStatusCode = WEB_RES_REQUEST_OK;
	return EXIT_SUCCESS;
}



static const ApiAccessPoint	_apNodeReceivers =
{
	type 	: 	NMOS_NODE_URL_T_RECEIVERS,
	name	: 	NMOS_NODE_URL_STR_RECEIVERS,
	callback :	_nmosReceiversHander,
	
	child		:	NULL,
	next		:	NULL
};

static const ApiAccessPoint	_apNodeSenders =
{
	type 	: 	NMOS_NODE_URL_T_SENDERS,
	name	: 	NMOS_NODE_URL_STR_SENDERS,
	callback :	_nmosSendersHander,
	
	child		:	NULL,
	next		:	&_apNodeReceivers
};

static const ApiAccessPoint	_apNodeFlows =
{
	type 	: 	NMOS_NODE_URL_T_FLOWS,
	name	: 	NMOS_NODE_URL_STR_FLOWS,
	callback :	_nmosFlowsHander,
	
	child		:	NULL,
	next		:	&_apNodeSenders
};

static const ApiAccessPoint	_apNodeSources =
{
	type 	: 	NMOS_NODE_URL_T_SOURCES,
	name	: 	NMOS_NODE_URL_STR_SOURCES,
	callback :	_nmosSourcesHander,
	
	child		:	NULL,
	next		:	&_apNodeFlows
};


static const ApiAccessPoint	_apNodeDevices =
{
	type 	: 	NMOS_NODE_URL_T_DEVICES,
	name	: 	NMOS_NODE_URL_STR_DEVICES,
	callback :	_nmosDeviceHander,
	
	child		:	NULL,
	next		:	&_apNodeSources
};

static const ApiAccessPoint	_apNodeSelf =
{
	type 	: 	NMOS_NODE_URL_T_SELF,
	name	: 	NMOS_NODE_URL_STR_SELF,
	callback :	_nmosSelfHander,
	
	child		:	NULL,
	next		:	&_apNodeDevices
};


/* root access point of NODE API */
const ApiAccessPoint	apNodeRoot =
{
	type 	: 	1,
	name	: 	"/",
	callback :	muxNmosRootApHander,
	
	child		:	&_apNodeSelf,
	next		:	NULL
};


