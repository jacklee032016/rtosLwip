/*
 * @brief Utilities for manipulating JSON
 *
 * json_utils provides JSON parsing utilities for use with the IoT SDK.
 * Underlying JSON parsing relies on the Jasmine JSON parser.
 *
 */

#include "extSysParams.h"

#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>
#include <string.h>

#include "jsmn.h"


IoT_Error_t parseUnsignedInteger32Value(unsigned int *i, const char *jsonString, jsmntok_t *token)
{
	if (token->type != JSMN_PRIMITIVE)
	{
		printf("Token was not an integer");
		return JSON_PARSE_ERROR;
	}

	if (1 != sscanf(jsonString + token->start, "%"PRIu32, i))
	{
		printf("Token was not an integer.");
		return JSON_PARSE_ERROR;
	}

	return NONE_ERROR;
}

IoT_Error_t parseUnsignedInteger16Value(unsigned short *i, const char *jsonString, jsmntok_t *token)
{
	if (token->type != JSMN_PRIMITIVE)
	{
		printf("Token was not an integer");
		return JSON_PARSE_ERROR;
	}

#ifdef	ARM
	if (1 != sscanf(jsonString + token->start, "%"PRIu16, i))
#else
	if (1 != sscanf(jsonString + token->start, "%hu", i))
#endif		
	{
		printf("Token was not an integer.");
		return JSON_PARSE_ERROR;
	}

	return NONE_ERROR;
}

IoT_Error_t parseUnsignedInteger8Value(unsigned char *i, const char *jsonString, jsmntok_t *token)
{
	if (token->type != JSMN_PRIMITIVE)
	{
		printf("Token was not an integer");
		return JSON_PARSE_ERROR;
	}

#ifdef	ARM
	if (1 != sscanf(jsonString + token->start, "%"PRIu8, i))
#else
	if (1 != sscanf(jsonString + token->start, "%hhu", i))
#endif		
	{
		printf("Token was not an integer.");
		return JSON_PARSE_ERROR;
	}

	return NONE_ERROR;
}

IoT_Error_t parseInteger32Value(int *i, const char *jsonString, jsmntok_t *token)
{
	if (token->type != JSMN_PRIMITIVE)
	{
		printf("Token was not an integer");
		return JSON_PARSE_ERROR;
	}

	if (1 != sscanf(jsonString + token->start, "%"PRIi32, i))
	{
		printf("Token was not an integer.");
		return JSON_PARSE_ERROR;
	}

	return NONE_ERROR;
}

IoT_Error_t parseInteger16Value(short *i, const char *jsonString, jsmntok_t *token)
{
	if (token->type != JSMN_PRIMITIVE)
	{
		printf("Token was not an integer");
		return JSON_PARSE_ERROR;
	}

#ifdef	ARM
	if (1 != sscanf(jsonString + token->start, "%"PRIi16, i))
#else		
	if (1 != sscanf(jsonString + token->start, "%hu", i))
#endif		
	{
		printf("Token was not an integer.");
		return JSON_PARSE_ERROR;
	}

	return NONE_ERROR;
}

IoT_Error_t parseInteger8Value(char *i, const char *jsonString, jsmntok_t *token)
{
	if (token->type != JSMN_PRIMITIVE)
	{
		printf("Token was not an integer");
		return JSON_PARSE_ERROR;
	}

#ifdef	ARM
	if (1 != sscanf(jsonString + token->start, "%"PRIi8, i))
#else		
	if (1 != sscanf(jsonString + token->start, "%hhu", i))
#endif		
	{
		printf("Token was not an integer.");
		return JSON_PARSE_ERROR;
	}

	return NONE_ERROR;
}

#if 0
IoT_Error_t parseFloatValue(float *f, const char *jsonString, jsmntok_t *token)
{
	if (token->type != JSMN_PRIMITIVE)
	{
		printf("Token was not a float.");
		return JSON_PARSE_ERROR;
	}

	if (1 != sscanf(jsonString + token->start, "%f", f))
	{
		printf("Token was not a float.");
		return JSON_PARSE_ERROR;
	}

	return NONE_ERROR;
}

IoT_Error_t parseDoubleValue(double *d, const char *jsonString, jsmntok_t *token)
{
	if (token->type != JSMN_PRIMITIVE)
	{
		printf("Token was not a double.");
		return JSON_PARSE_ERROR;
	}

	if (1 != sscanf(jsonString + token->start, "%lf", d))
	{
		printf("Token was not a double.");
		return JSON_PARSE_ERROR;
	}

	return NONE_ERROR;
}
#endif

IoT_Error_t parseBooleanValue(char *b, const char *jsonString, jsmntok_t *token)
{
	if (token->type != JSMN_PRIMITIVE)
	{
		printf("Token was not a primitive.");
		return JSON_PARSE_ERROR;
	}

	if (jsonString[token->start] == 't' && jsonString[token->start + 1] == 'r' && jsonString[token->start + 2] == 'u'
			&& jsonString[token->start + 3] == 'e')
	{
		*b = 1;
	}
	else if (jsonString[token->start] == 'f' && jsonString[token->start + 1] == 'a'
			&& jsonString[token->start + 2] == 'l' && jsonString[token->start + 3] == 's'
			&& jsonString[token->start + 4] == 'e')
	{
		*b = 0;
	}
	else
	{
		printf("Token was not a bool.");
		return JSON_PARSE_ERROR;
	}
	return NONE_ERROR;
}

IoT_Error_t parseStringValue(char *buf, const char *jsonString, jsmntok_t *token)
{
	uint16_t size = 0;
	if (token->type != JSMN_STRING)
	{
		printf("Token was not a string.");
		return JSON_PARSE_ERROR;
	}
	
	size = token->end - token->start;
	memcpy(buf, jsonString + token->start, size);
	buf[size] = '\0';
	return NONE_ERROR;
}


