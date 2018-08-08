/*
* Interface between HTTP service and NMOS 
*/

#include "lwipMux.h"

#include "muxHttp.h"
#include "jsmn.h"

const	MUX_CONST_STR	nodeRoots[] =
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

char muxNmosNodeDumpHander(MuxHttpConn  *mhc, void *data)
{
	const ApiAccessPoint	*apiAp = (const ApiAccessPoint *)data;

	snprintf(mhc->uri + strlen(mhc->uri), sizeof(mhc->uri)-strlen(mhc->uri), "(%s)%s", apiAp->name, " will be implemented in future" );
	MUX_DEBUGF(MUX_HTTPD_DEBUG, ("'%s' is not implemented yet", apiAp->name) );
	muxHttpRestError(mhc, WEB_RES_NOT_IMP, (const char *)mhc->uri);
	return EXIT_SUCCESS;
}

char muxNmosRootApHander(MuxHttpConn  *mhc, void *data)
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


static void _muxApiRequestDebug(MuxHttpConn *mhc)
{
	int i;
	char uuidStr[128];
	printf("API Request: v:%d; Count of resources:%d"MUX_NEW_LINE, mhc->apiReq.versionFlags, mhc->apiReq.resourceCount);

	for(i=0; i< mhc->apiReq.resourceCount; i++)	
	{
		printf("resource[%d]: %s;"MUX_NEW_LINE, i, mhc->apiReq.resources[i]);
	}

	i = 0;
	UUID_PRINT(uuidStr, sizeof(uuidStr), i, &mhc->apiReq.uuid );
	printf("UUID: %s;"MUX_NEW_LINE, uuidStr);
	
}


/* UUID can be used in the middle of URI string */
static char _muxParseApiResource(MuxNmosApiReq *apiReq, char *resource)
{
	unsigned int size;
	char	*next = strstr(resource, "/");
	
	printf("parse: %s"MUX_NEW_LINE, resource);
	if(next)
	{
		/* case: UUID in the middle of URI*/
		if(muxUuidParse(&apiReq->uuid, resource) == EXIT_SUCCESS)
		{
			return MUX_UUID_STR_LENGTH+1;	/* +1: ignore the last '/'*/
		}
		
		size = LWIP_MIN(next-resource, NMOS_URI_RESOURCE_LENGTH);
		if(size< ((unsigned int)(next-resource)) )
		{
			MUX_INFOF(("resource buffer is not enough (%d) for '%.*s'", size, next- resource, resource) );
		}
		
		memcpy(apiReq->resources[apiReq->resourceCount++], resource, size);
		return next - resource + 1; /* pass this '/' */
	}

	/* case: the last pat of URI is UUID */
	if(strlen(resource) != MUX_UUID_STR_LENGTH)
	{
		size = LWIP_MIN(strlen(resource), NMOS_URI_RESOURCE_LENGTH);
		if(size< strlen(resource))
		{
			MUX_INFOF(("resource buffer is not enough (%d) for '%.*s'", size, strlen(resource), resource) );
		}
		
		memcpy(apiReq->resources[apiReq->resourceCount++], resource, size);
		return strlen(resource);
	}

	if(muxUuidParse(&apiReq->uuid, resource) == EXIT_SUCCESS)
	{
		return MUX_UUID_STR_LENGTH;
	}

	return 0;
}


static char muxHttpNmosParseApiRequest(MuxHttpConn *mhc, NMOS_API_TYPE type)
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
		MUX_INFOF(("API version '%.*s' not supported", 4, version) );
		return EXIT_FAILURE;
	}
	
	version += strlen(NMOS_API_VERSION_12);
	if(urlLen == version - mhc->uri /* with last '/' */ || urlLen == version+1-mhc->uri /* with last '/' */)
	{
		MUX_INFOF(("API request at root URI") );
//		sprintf(apiReq->resources[apiReq->resourceCount], "%s", "/");
		apiReq->resourceCount = 0;
		return EXIT_SUCCESS;
	}

	version++; /* ignore the root '/' */
	do
	{
		ret = _muxParseApiResource(apiReq, version);
		version += ret;
	}while(version - mhc->uri < urlLen);
		
	_muxApiRequestDebug(mhc);
	
	return EXIT_SUCCESS;
}

#if MUX_HTTPD_DEBUG
static void _muxApiApdebug(const ApiAccessPoint *apRoot)
{
	int i = 0;
	while(apRoot)
	{
		printf("#%d:Type:%d, name:%s"MUX_NEW_LINE, ++i, apRoot->type, apRoot->name);
		apRoot = apRoot->next;
	}
	
}
#endif

static char _muxRestApiHandle(MuxHttpConn *mhc, const ApiAccessPoint *apiAcRoot, NMOS_API_TYPE type)
{
	const ApiAccessPoint	*apiAp = apiAcRoot;
	MuxNmosApiReq	*apiReq = &mhc->apiReq;
	int	index = 0;
	char		ret;

	mhc->reqType = MUX_HTTP_REQ_T_REST;
	

	if( muxHttpNmosParseApiRequest(mhc, type) == EXIT_FAILURE)
	{
		muxHttpRestError(mhc, WEB_RES_BAD_REQUEST, "Failed in parsing resources in URI");
		return EXIT_SUCCESS;
	}
	
	if(apiReq->resourceCount == 0)
	{/* root access point */
		return apiAp->callback(mhc, (void *)apiAp);
	}
	apiAp = apiAp->child;

	while(apiAp)
	{
		MUX_DEBUGF(MUX_HTTPD_DEBUG, ("API:'%s' :: REQ:'%s'", apiAp->name, apiReq->resources[index]) );
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
				if(MUX_DEBUG_IS_ENABLE(MUX_DEBUG_FLAG_CMD))
				{
	//					printf("output RES12 %p, %d bytes: '%s'"LWIP_NEW_LINE, (void *)parser, parser->outIndex, parser->outBuffer);
				}
				MUX_DEBUGF(MUX_HTTPD_DEBUG, ("return value of REST API callback handler:%d", ret) );

				return ret;
			}	

			apiAp = apiAp->child;
		}
		else
		{
			apiAp = apiAp->next;
		}
		
	}

	muxHttpRestError(mhc, WEB_RES_BAD_REQUEST, "Can't find service of URI");

	return EXIT_SUCCESS;

}

char muxHttpHandleRequest(MuxHttpConn *mhc)
{
//	char ret = EXIT_FAILURE;

#if MUX_HTTPD_DEBUG
	_muxApiApdebug(&apNodeRoot);
#endif

	if(HTTP_IS_ERROR(mhc) )
	{
		return EXIT_SUCCESS;
	}

	if(strstr(mhc->uri, NMOS_API_URI_NODE) )
	{
		return _muxRestApiHandle(mhc, &apNodeRoot, NMOS_API_T_NODE);
	}
	else if( strstr(mhc->uri, NMOS_API_URI_CONNECTION) )
	{
		return _muxRestApiHandle(mhc, &apConnRoot, NMOS_API_T_CONNECTION);
	}
	else if(muxHttpWebService(mhc, NULL) == EXIT_SUCCESS)
	{
		return EXIT_SUCCESS;
	}
	else if(muxHttpFileFind(mhc) == ERR_OK)
	{
		mhc->reqType = MUX_HTTP_REQ_T_FILE;
		return EXIT_SUCCESS;
	}
	else
	{
	}

	mhc->reqType = MUX_HTTP_REQ_T_REST;
	muxHttpRestError(mhc, WEB_RES_NOT_FOUND, "URI not exist in server");
	
	return EXIT_SUCCESS;
}


