/*
* Interface between HTTP service and NMOS 
*/

#include "lwipExt.h"

#include "http.h"
#include "jsmn.h"

#if LWIP_EXT_NMOS

const	EXT_CONST_STR	nodeRoots[] =
{
	{
		type	: NMOS_NODE_URL_T_SELF,
		name	: NMOS_NODE_URL_STR_SELF
	},
	{
		type	: NMOS_NODE_URL_T_DEVICES,
		name	: NMOS_NODE_URL_STR_DEVICES
	},
	{
		type	: NMOS_NODE_URL_T_SOURCES,
		name	: NMOS_NODE_URL_STR_SOURCES
	},
	{
		type	: NMOS_NODE_URL_T_FLOWS,
		name	: NMOS_NODE_URL_STR_FLOWS
	},
	{
		type	: NMOS_NODE_URL_T_SENDERS,
		name	: NMOS_NODE_URL_STR_SENDERS
	},
	{
		type	: NMOS_NODE_URL_T_RECEIVERS,
		name	: NMOS_NODE_URL_STR_RECEIVERS
	},
	{
		type	: NMOS_NODE_URL_T_TARGET,
		name	: NMOS_NODE_URL_STR_TARGET
	},
	{
		type	: NMOS_NODE_URL_T_UNKNOWN,
		name	: NULL
	}
};

char extNmosNodeDumpHander(ExtHttpConn  *mhc, void *data)
{
	const ApiAccessPoint	*apiAp = (const ApiAccessPoint *)data;

	snprintf(mhc->uri + strlen(mhc->uri), sizeof(mhc->uri)-strlen(mhc->uri), "(%s)%s", apiAp->name, " will be implemented in future" );
	EXT_DEBUGF(EXT_NMOS_DEBUG, ("'%s' is not implemented yet", apiAp->name) );
	cmnHttpRestError(mhc, WEB_RES_NOT_IMP, (const char *)mhc->uri);
	return EXIT_SUCCESS;
}

char extNmosRootApHander(ExtHttpConn  *mhc, void *data)
{
	int index = 0;
	const ApiAccessPoint	*apiAp = (const ApiAccessPoint *)data;
	int i =0;

	TRACE();
	apiAp = apiAp->child;

	index += snprintf((char *)mhc->data+index, sizeof(mhc->data)-index, "[" );
	while(apiAp)
	{
		if(i != 0)
		{
			index += snprintf((char *)mhc->data+index, sizeof(mhc->data)-index, ",");
		}
		i++;
		index += snprintf((char *)mhc->data+index, sizeof(mhc->data)-index, "\"%s/\"", apiAp->name );
		apiAp = apiAp->next;
	}
	
	index += snprintf((char *)mhc->data+index, sizeof(mhc->data)-index, "]" );

	mhc->contentLength = (unsigned short)index;
	mhc->dataSendIndex = 0;
	
	mhc->httpStatusCode = WEB_RES_REQUEST_OK;
	return EXIT_SUCCESS;
}


static void _extApiRequestDebug(ExtHttpConn *mhc)
{
	int i;
	char uuidStr[128];
	printf("API Request: v:%d; Count of resources:%d"EXT_NEW_LINE, mhc->apiReq.versionFlags, mhc->apiReq.resourceCount);

	for(i=0; i< mhc->apiReq.resourceCount; i++)	
	{
		printf("resource[%d]: %s;"EXT_NEW_LINE, i, mhc->apiReq.resources[i]);
	}

	i = 0;
	UUID_PRINT(uuidStr, sizeof(uuidStr), i, &mhc->apiReq.uuid );
	printf("UUID: %s;"EXT_NEW_LINE, uuidStr);
	
}


/* UUID can be used in the middle of URI string */
static char _extParseApiResource(MuxNmosApiReq *apiReq, char *resource)
{
	unsigned int size;
	char	*next = strstr(resource, "/");
	
	printf("parse: %s"EXT_NEW_LINE, resource);
	if(next)
	{
		/* case: UUID in the middle of URI*/
		if(extUuidParse(&apiReq->uuid, resource) == EXIT_SUCCESS)
		{
			return EXT_UUID_STR_LENGTH+1;	/* +1: ignore the last '/'*/
		}
		
		size = LWIP_MIN(next-resource, NMOS_URI_RESOURCE_LENGTH);
		if(size< ((unsigned int)(next-resource)) )
		{
			EXT_INFOF(("resource buffer is not enough (%d) for '%.*s'", size, next- resource, resource) );
		}
		
		memcpy(apiReq->resources[apiReq->resourceCount++], resource, size);
		return next - resource + 1; /* pass this '/' */
	}

	/* case: the last pat of URI is UUID */
	if(strlen(resource) != EXT_UUID_STR_LENGTH)
	{
		size = LWIP_MIN(strlen(resource), NMOS_URI_RESOURCE_LENGTH);
		if(size< strlen(resource))
		{
			EXT_INFOF(("resource buffer is not enough (%d) for '%.*s'", size, strlen(resource), resource) );
		}
		
		memcpy(apiReq->resources[apiReq->resourceCount++], resource, size);
		return strlen(resource);
	}

	if(extUuidParse(&apiReq->uuid, resource) == EXIT_SUCCESS)
	{
		return EXT_UUID_STR_LENGTH;
	}

	return 0;
}


static char extHttpNmosParseApiRequest(ExtHttpConn *mhc, NMOS_API_TYPE type)
{
	MuxNmosApiReq *apiReq;
	char *version;
	char ret;

	unsigned short urlLen;
	urlLen = (unsigned short)strlen(mhc->uri);

	apiReq = &mhc->apiReq;
	memset(apiReq, 0, sizeof(MuxNmosApiReq));

	if(type == NMOS_API_T_NODE)
	{
		version = mhc->uri+ strlen(NMOS_API_URI_NODE) +1;
	}
	else
	{
		version = mhc->uri+ strlen(NMOS_API_URI_CONNECTION) +1;
	}
	
	if(lwip_strnstr(version, NMOS_API_VERSION_10, strlen(NMOS_API_VERSION_10) ) )
	{
		NMOS_VERSION_SET10(apiReq);
	}
	else if(lwip_strnstr(version, NMOS_API_VERSION_11, strlen(NMOS_API_VERSION_11) ) )
	{
		NMOS_VERSION_SET11(apiReq);
	}
	else if(lwip_strnstr(version, NMOS_API_VERSION_12, strlen(NMOS_API_VERSION_12) ) )
	{
		NMOS_VERSION_SET12(apiReq);
	}
	else 
	{
		EXT_INFOF(("API version '%.*s' not supported", 4, version) );
		return EXIT_FAILURE;
	}
	
	version += strlen(NMOS_API_VERSION_12);
	if(urlLen == version - mhc->uri /* with last '/' */ || urlLen == version+1-mhc->uri /* with last '/' */)
	{
		EXT_INFOF(("API request at root URI") );
//		sprintf(apiReq->resources[apiReq->resourceCount], "%s", "/");
		apiReq->resourceCount = 0;
		return EXIT_SUCCESS;
	}

	version++; /* ignore the root '/' */
	do
	{
		ret = _extParseApiResource(apiReq, version);
		version += ret;
	}while(version - mhc->uri < urlLen);
		
	_extApiRequestDebug(mhc);
	
	return EXIT_SUCCESS;
}

#if EXT_NMOS_DEBUG
static void _extApiApdebug(const ApiAccessPoint *apRoot)
{
	int i = 0;
	while(apRoot)
	{
		printf("#%d:Type:%d, name:%s"EXT_NEW_LINE, ++i, apRoot->type, apRoot->name);
		apRoot = apRoot->next;
	}
	
}
#endif

static char _extRestApiHandle(ExtHttpConn *mhc, const ApiAccessPoint *apiAcRoot, NMOS_API_TYPE type)
{
	const ApiAccessPoint	*apiAp = apiAcRoot;
	MuxNmosApiReq	*apiReq = &mhc->apiReq;
	int	index = 0;
	char		ret;

	mhc->reqType = EXT_HTTP_REQ_T_REST;
	

	if( extHttpNmosParseApiRequest(mhc, type) == EXIT_FAILURE)
	{
		cmnHttpRestError(mhc, WEB_RES_BAD_REQUEST, "Failed in parsing resources in URI");
		return EXIT_SUCCESS;
	}
	
	if(apiReq->resourceCount == 0)
	{/* root access point */
		return apiAp->callback(mhc, (void *)apiAp);
	}
	apiAp = apiAp->child;

	while(apiAp)
	{
		EXT_DEBUGF(EXT_NMOS_DEBUG, ("API:'%s' :: REQ:'%s'", apiAp->name, apiReq->resources[index]) );
		if(strcasecmp(apiAp->name, apiReq->resources[index] ) == 0)
		{
			index++;
			if(index == apiReq->resourceCount)
			{
	TRACE();
				ret = apiAp->callback(mhc, (void *)apiAp);
				if(ret == EXIT_FAILURE)
				{
				}	
				if(EXT_DEBUG_IS_ENABLE(EXT_DEBUG_FLAG_CMD))
				{
	//					printf("output RES12 %p, %d bytes: '%s'"LWIP_NEW_LINE, (void *)parser, parser->outIndex, parser->outBuffer);
				}
				EXT_DEBUGF(EXT_NMOS_DEBUG, ("return value of REST API callback handler:%d", ret) );

				return ret;
			}	

			apiAp = apiAp->child;
		}
		else
		{
			apiAp = apiAp->next;
		}
		
	}

	cmnHttpRestError(mhc, WEB_RES_BAD_REQUEST, "Can't find service of URI");

	return EXIT_SUCCESS;

}

err_t extNmosHandleRequest(ExtHttpConn *mhc)
{
	char ret = EXIT_FAILURE;

#if EXT_NMOS_DEBUG
	_extApiApdebug(&apNodeRoot);
#endif

	if(HTTP_IS_ERROR(mhc) )
	{
		return EXIT_SUCCESS;
	}

	if(strstr(mhc->uri, NMOS_API_URI_NODE) )
	{
		ret = _extRestApiHandle(mhc, &apNodeRoot, NMOS_API_T_NODE);
		goto _return;
	}
	else if( strstr(mhc->uri, NMOS_API_URI_CONNECTION) )
	{
		_extRestApiHandle(mhc, &apConnRoot, NMOS_API_T_CONNECTION);
		goto _return;
	}
 	
	return ERR_INPROGRESS;
	
_return:
 	mhc->reqType = EXT_HTTP_REQ_T_REST;
	
	if(ret == EXIT_FAILURE)
	{
		cmnHttpRestError(mhc, WEB_RES_NOT_FOUND, "URI not exist in server");
		return ERR_VAL;
	}
		
	return ERR_OK;	
}

#endif


