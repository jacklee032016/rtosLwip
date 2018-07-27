#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include "jsmn.h"


#define	MUX_JSON_MESSAGE_SIZE		32
#define	MUX_JSON_TOKEN_SIZE			50
#define	MUX_JSON_OUT_BUF_SIZE		512

#define	JSON_TOKEN_COUNT()	\
		(muxParser.parser.toknext)



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

/*
 * An example of reading JSON from stdin and printing its content to stdout.
 * The output looks like YAML, but I'm not sure if it's really compatible.
 */

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


jsmn_parser	jsonParser;
jsmntok_t 	jsonTokens[MUX_JSON_TOKEN_SIZE];
char			jsonOutBuffer[MUX_JSON_OUT_BUF_SIZE];

size_t tokcount = MUX_JSON_TOKEN_SIZE;

MUX_JSON_PARSER  muxParser;

int _muxJsonPrint(jsmntok_t *t, size_t count)
{
	int i, j, k;
	int indent = 2;
	
	if(count == 0)
	{
		return 0;
	}
	
	if (t->type == JSMN_PRIMITIVE)
	{
		printf("%.*s", t->end - t->start, muxParser.currentJSonString +t->start);
		return 1;
	}
	else if (t->type == JSMN_STRING)
	{
		printf("'%.*s'", t->end - t->start, muxParser.currentJSonString +t->start);
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

int	_muxJsonParse( const char *jStr, size_t len )
{
	int ret;

	muxParser.currentJSonString = jStr;
	muxParser.len = len;
	
	ret = jsmn_parse(&muxParser.parser, jStr, len, muxParser.tokens, muxParser.tokenCount);
	if (ret < 0)
	{
		if (ret == JSMN_ERROR_NOMEM)
		{
			printf("ERROR: JSMN_ERROR_NOMEM\n\r");
		}
		else if(ret == JSMN_ERROR_INVAL)
		{
			printf("ERROR: JSMN_ERROR_INVAL\n\r");
		}
		else
		{
			printf("ERROR: JSMN_ERROR_PART\n\r");
		}

		return ret;
	}
}

void _muxJsonInit(void)
{
	/* Prepare parser */
	jsmn_init(&muxParser.parser);
	
	muxParser.currentJSonString = NULL;
	muxParser.length = 0;
	
	muxParser.tokenCount = MUX_JSON_TOKEN_SIZE;

	muxParser.outSize = MUX_JSON_OUT_BUF_SIZE;
	muxParser.outIndex = 0;
	
}

int main(int argc, char *argv[])
{
	int r;
	int eof_expected = 0;
	char buf[BUFSIZ];

//	char *js = NULL;
	char *js = 
		"{ \"height\":10, \"layers\":[ { \"data\":[6,6], \"height\":10, "
		   "\"name\":\"Calque de Tile 1\", \"opacity\":1, \"type\":\"tilelayer\", "
		   "\"visible\":true, \"width\":10, \"x\":0, \"y\":0 }], "
		   "\"orientation\":\"orthogonal\", \"properties\": { }, \"tileheight\":32, "
		   "\"tilesets\":[ { \"firstgid\":1, \"image\":\"..\\/images\\/tiles.png\", "
		   "\"imageheight\":64, \"imagewidth\":160, \"margin\":0, \"name\":\"Tiles\", "
		   "\"properties\":{}, \"spacing\":0, \"tileheight\":32, \"tilewidth\":32 }], "
		   "\"tilewidth\":32, \"version\":1, \"width\":10 }";
	size_t jslen;;

	jsmntok_t *tok;
	size_t tokcount = 200;


	jslen = strlen(js);
	
	/* Allocate some tokens as a start */
	tok = malloc(sizeof(*tok) * tokcount);
	if (tok == NULL)
	{
		fprintf(stderr, "malloc(): errno=%d\n", errno);
		return 3;
	}

	for (;;)
	{
#if 0	
		/* Read another chunk */
		r = fread(buf, 1, sizeof(buf), stdin);
		if (r < 0)
		{
			fprintf(stderr, "fread(): %d, errno=%d\n", r, errno);
			return 1;
		}
		
		if (r == 0)
		{
			if (eof_expected != 0)
			{
				return 0;
			}
			else
			{
				fprintf(stderr, "fread(): unexpected EOF\n");
				return 2;
			}
		}

		js = realloc_it(js, jslen + r + 1);
		if (js == NULL)
		{
			return 3;
		}
		strncpy(js + jslen, buf, r);
		jslen = jslen + r;

again:
#endif	
		r = jsmn_parse(&p, js, jslen, tok, tokcount);
		if (r < 0)
		{
			if (r == JSMN_ERROR_NOMEM)
			{
				tokcount = tokcount * 2;
				tok = realloc_it(tok, sizeof(*tok) * tokcount);
				if (tok == NULL)
				{
					return 3;
				}
//				goto again;
			}
		}
		else
		{
			dump(js, tok, p.toknext, 0);
			eof_expected = 1;
		}

		break;
	}

	return EXIT_SUCCESS;
}

