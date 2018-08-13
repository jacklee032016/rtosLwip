#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#include "lwipExt.h"
#include "lwip/inet.h"

typedef enum
{
	SHADOW_JSON_INT32,
	SHADOW_JSON_INT16,
	SHADOW_JSON_INT8,
	SHADOW_JSON_UINT32,
	SHADOW_JSON_UINT16,
	SHADOW_JSON_UINT8,
	SHADOW_JSON_FLOAT,
	SHADOW_JSON_DOUBLE,
	SHADOW_JSON_BOOL,
	SHADOW_JSON_STRING,
	SHADOW_JSON_OBJECT
}MuxJsonPrimitiveType;


typedef struct _MuxJsonStruct		MuxJsonStruct;
/**
 * @brief Every JSON name value can have a callback. The callback should follow this signature
 */
typedef void (*MuxJsonStructCallback)(const char *pJsonValueBuffer, uint32_t valueLength, MuxJsonStruct *pJsonStruct_t);

/**
 * @brief This is the struct form of a JSON Key value pair
 */
struct _MuxJsonStruct
{
	const char				*key; ///< JSON key
	void						*data; ///< pointer to the data (JSON value)
	MuxJsonPrimitiveType		type; ///< type of JSON
	MuxJsonStructCallback		cb; ///< callback to be executed on receiving the Key value pair
};


typedef struct
{
	const char				*key;
	void						*pStruct;
	MuxJsonStructCallback		callback;
	char						isFree;
}MuxJsonTokenTable;



const	char *testJSon = 
		"{ \"height\":10, "
		   "\"layers\":[ { \"data\":[6,6], \"height\":10, "
		   "\"name\":\"Calque de Tile 1\", \"opacity\":1, \"type\":\"tilelayer\", "
		   "\"visible\":true, \"width\":10, \"x\":0, \"y\":0 }], "

		   "\"orientation\":\"orthogonal\", \"properties\": { }, \"tileheight\":32, "
		   "\"tilesets\":[ { \"firstgid\":1, \"image\":\"..\\/images\\/tiles.png\", "
		   "\"imageheight\":64, \"imagewidth\":160, \"margin\":0, \"name\":\"Tiles\", "
		   "\"properties\":{}, \"spacing\":0, \"tileheight\":32, \"tilewidth\":32 }], "
		   "\"tilewidth\":32, \"version\":1, \"width\":10 }";


#if 0

/* Function realloc_it() is a wrapper function for standard realloc()
 * with one difference - it frees old memory pointer in case of realloc
 * failure. Thus, DO NOT use old data pointer in anyway after call to
 * realloc_it(). If your code has some kind of fallback algorithm if
 * memory can't be re-allocated - use standard realloc() instead.
 */
static inline void *realloc_it(void *ptrmem, size_t size)
{
	void *p = realloc(ptrmem, size);
	if (!p)
	{
		free (ptrmem);
		fprintf(stderr, "realloc(): errno=%d\n", errno);
	}
	return p;
}

static int dump(const char *js, jsmntok_t *t, size_t count, int indent)
{
	int i, j, k;
	
	if (count == 0)
	{
		return 0;
	}
	
	if (t->type == JSMN_PRIMITIVE)
	{
		printf("%.*s", t->end - t->start, js+t->start);
		return 1;
	}
	else if (t->type == JSMN_STRING)
	{
		printf("'%.*s'", t->end - t->start, js+t->start);
		return 1;
	}
	else if (t->type == JSMN_OBJECT)
	{
		printf("\n");
		j = 0;
		for (i = 0; i < t->size; i++)
		{
			for (k = 0; k < indent; k++)
				printf("  ");
			
			j += dump(js, t+1+j, count-j, indent+1);
			printf(": ");
			j += dump(js, t+1+j, count-j, indent+1);
			printf("\n");
		}
		return j+1;
	}
	else if (t->type == JSMN_ARRAY)
	{
		j = 0;
		printf("\n");
		for (i = 0; i < t->size; i++)
		{
			for (k = 0; k < indent-1; k++)
				printf("  ");
			
			printf("   - ");
			j += dump(js, t+1+j, count-j, indent+1);
			printf("\n");
		}
		
		return j+1;
	}
	return 0;
}
#endif

EXT_JSON_PARSER  testParser;


char *tokenTypeString(jsmntok_t *token)
{
	switch(token->type)
	{
		case JSMN_STRING:
			return "STRING";
			break;
		case JSMN_OBJECT:
			return "OBJECT";
			break;
	 	case JSMN_ARRAY:
			return "ARRAY";
			break;
		case JSMN_PRIMITIVE:
			return "PRIMITIVE";
			break;
		default:	
			return "UNDEFINED";
			break;
	}

	return "UNDEFINED";
}


int tokenPrintHandle(EXT_JSON_PARSER *parser, int index, jsmntok_t *key, void *data)
{
	jsmntok_t *value = key+1;

	printf("No. %d item(%s:%s): '%.*s':\n\r", index, tokenTypeString(key), tokenTypeString(value), JSON_TOKEN_LENGTH(key), parser->currentJSonString+ key->start);

#if 0
	if(value->type==JSMN_STRING)
	{
		printf("'%.*s'(%s)\n\r", 	JSON_TOKEN_LENGTH(value), extParser.currentJSonString+value->start );
	}
	else if(value->type==JSMN_OBJECT)
	{
		printf("OBJECT\n\r");
	}
	else if(value->type==JSMN_ARRAY)
	{
		int i;
		jsmntok_t *item = value+1;
		int arraySize = item->size - 1; /* first item is all string for this array */
		jsmntok_t *_key = item+1;
		jsmntok_t *_value = item+2;
		printf("ARRAY, size %d\n\r", arraySize);
		for(i=0; i<arraySize; i++)
		{
			printf("\tNo. %d item: '%.*s':'%.*s'\n\r", i, JSON_TOKEN_LENGTH(_key), extParser.currentJSonString+ _key->start, 
				JSON_TOKEN_LENGTH(_value), (_value->type==JSMN_STRING)?extParser.currentJSonString+_value->start:"NoString");
			_key +=2;
			_value += 2;
		}
	}
	else if(value->type==JSMN_PRIMITIVE)
	{
		printf("PRIMITIVE\n\r");
	}
	else
	{
		printf("UNDEFINED\n\r");
	}
#endif
	
	return 0; /* iterate to next */
}



int extJsonIterate(EXT_JSON_PARSER *parser, token_callback tokenHandle, void *data)
{
	int i;
	int ret = 0;

	printf("total. %d items\n\r", JSON_TOKEN_COUNT(parser));
	for(i=0; i< JSON_TOKEN_COUNT(parser)/2; i++)
	{
		jsmntok_t *key = &parser->tokens[2*i+1];
		
		ret = (tokenHandle)(parser, i, key, data);
		if( ret == 1)
		{
			return 1;
		}
//		jsmntok_t *value = &extParser.tokens[2*i+2];
//		printf("No. %d item: '%.*s':'%.*s'\n\r", i, JSON_TOKEN_LENGTH(key), extParser.currentJSonString+ key->start, JSON_TOKEN_LENGTH(value), (key->type==JSMN_STRING)?extParser.currentJSonString+value->start:"NoString");
	}

	return 0;	
}


int testJson(EXT_JSON_PARSER  *parser)
{
//	jsmntok_t *key;
	jsmntok_t *value;

//	extJsonParse(parser, (char *)jsonGetParamsCmd);

	if(parser->status == JSON_STATUS_OK)
	{
#if 1	
		if(JSON_TOKEN_COUNT(parser)%2 != 1)
		{
			printf("token number %d is not even number\n\r", JSON_TOKEN_COUNT(parser) );
			return -1;
		}
#endif

#if 0
	int i;
		for(i=0; i< JSON_TOKEN_COUNT()/2; i++)
		{
			jsmntok_t *key = &extParser.tokens[2*i+1];
			jsmntok_t *value = &extParser.tokens[2*i+2];
			printf("No. %d item: '%.*s':'%.*s'\n\r", i, JSON_TOKEN_LENGTH(key), extParser.currentJSonString+ key->start, JSON_TOKEN_LENGTH(value), (key->type==JSMN_STRING)?extParser.currentJSonString+value->start:"NoString");
		}
#endif		
		extJsonIterate(parser, tokenPrintHandle, NULL);
	}
	else
	{
		printf("JSON parsing failed\n\r" );
	}

	if(extJsonFindCommand(parser) )
	{
		printf("JSON command: '%s'\n\r", parser->cmd);
	}

#if 0
	printf("\n\r" );
	extJsonParse(parser, (char *)jsonReplyParams);
	extJsonIterate(parser, tokenPrintHandle, NULL);
#endif

#if 1
	printf("\n\r" );
	extJsonParse(parser, (char *)testJSon, strlen(testJSon));
	extJsonIterate(parser, tokenPrintHandle, NULL);
#endif

	value = extJsonFindKeyToken(parser, "layers");
	if(value == NULL)
	{
		printf("ERROR can't find 'layers' object");
	}
	else
	{
		printf("'layers' object is %s, size is %d\n\r", tokenTypeString(value), value->size );
		value++;
		printf("'layers+1' object is %s, size is %d\n\r", tokenTypeString(value), value->size );
	}

	value = extJsonFindKeyToken(parser, "data");
	if(value == NULL)
	{
		printf("ERROR can't find 'data' object");
	}
	else
	{
		printf("'data' object is %s, size is %d\n\r", tokenTypeString(value), value->size );
	}

	value = extJsonFindKeyToken(parser, "tilesets");
	if(value == NULL)
	{
		printf("ERROR can't find 'tilesets' object");
	}
	else
	{
		printf("'tilesets' object is %s, size is %d\n\r", tokenTypeString(value), value->size );
	}
	
	extJsonPrint(parser, parser->tokens, JSON_TOKEN_COUNT(parser), 2, "Test");
	printf("\n\r" );

	return 0;
}


int main(int argc, char *argv[])
{
	EXT_JSON_PARSER  *parser = &testParser;
	EXT_RUNTIME_CFG	*runCfg= &extRun;

//	testJson(parser);


	extJsonInit(parser, NULL, 0);

	extCfgFromFactory(runCfg);

#if 0
	parser->status = JSON_STATUS_PARSE_ERROR;
	sprintf(parser->msg, "%s", "Parse JSON string from 811 failed");
	extIpCmdResponseReply(parser, runCfg);
	printf("%s\n\r", parser->outBuffer);

	extUuidGenerate( &parser->uuid, runCfg);

	extUuidParse(&parser->uuid, "5da68790-2198-43cb-9321-2951eeb5ee90");

	extJsonResponsePrintConfig(parser, runCfg);
	printf("%s\n\r", parser->outBuffer);
#endif

#if 0
	extJsonInit(parser, NULL);
	if(extJsonRequestParse(jsonRequest, parser, runCfg) < 0)
	{
		parser->status = JSON_STATUS_PARSE_PARAM_ERROR;
		printf("parse parameters error %d:%s\n\r", parser->status, parser->msg );
		return EXIT_FAILURE;
	}

	extJsonDebug(parser, runCfg, "");
#endif

	return EXIT_SUCCESS;
}

