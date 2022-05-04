

#include "extSysParams.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int 		argc = 0;
char		argv[EXT_CMD_MAX_ARGUMENTS][EXT_CMD_MAX_LENGTH];


static char _extGetOneArgument(const char *start, const char *end)
{
	int length = end - start;

	if(argc<EXT_CMD_MAX_ARGUMENTS && length <= EXT_CMD_MAX_LENGTH-1 )
	{
		memcpy(argv[argc], start, length);
		argv[argc][length] = 0;
		argc++;
	}
	else
	{
		printf("No.%d argument %s (length %d) is ignored"EXT_NEW_LINE, argc, end, length);
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}


static void _cmdParseParameters( const char *cmdStr )
{
	int i;
	const char *start = cmdStr;

	argc = 0;
	for(i=0; i<EXT_CMD_MAX_ARGUMENTS; i++)
	{
		argv[i][0] = 0;
	}
	
	/* Count the number of space delimited words in pcCommandString. */
	while( *cmdStr != 0x00 )
	{
		if( ( *cmdStr ) == ' ' ||(*cmdStr ) == ASCII_KEY_TAB /* TAB */ )
		{
			_extGetOneArgument(start, cmdStr);

			while(( *cmdStr ) == ' '|| (*cmdStr ) == ASCII_KEY_TAB /* TAB */)
			{/* ignore other space character */
				cmdStr++;
			}
			
			start = cmdStr;
		}
		else
		{
			cmdStr++;
		}

	}

	if(start< cmdStr)
	{
		_extGetOneArgument(start, cmdStr);
	}
}


char cmnCmdLineProcess( const char * const cmdInput, char *outBuffer, unsigned int bufferSize  )
{
	static const EXT_CLI_CMD_T *currentCmd = NULL;
	char ret = EXT_TRUE;
	
	_cmdParseParameters(cmdInput);
	if(argc == 0)
	{
		printf("No command is defined"EXT_NEW_LINE);
	}

	/* Note:  This function is not re-entrant.  It must not be called from more thank one task. */
	if( currentCmd == NULL )
	{
		currentCmd = extCmds;

		/* Search for the command string in the list of registered commands. */
		while( currentCmd->name != NULL)
		{
			if( strncmp( argv[0], currentCmd->name,  strlen(argv[0]) ) == 0 )
			{
				break;
			}

			currentCmd++;
		}
	}

#if 0
	{
		int i;

		printf( "Total %d argument in '%s':"EXT_NEW_LINE EXT_NEW_LINE, argc, cmdInput);
		for(i=0; i< EXT_CMD_MAX_ARGUMENTS; i++)
		{
			printf( "No.%d argument: '%s' "EXT_NEW_LINE, i, argv[i] );
		}
	}
#endif

	if( currentCmd->name != NULL )
	{/* Call the callback function */
		ret = currentCmd->callback(currentCmd, outBuffer, bufferSize );
		if( ret == EXT_FALSE )
		{
			currentCmd = NULL;
		}
	}
	else
	{/* pxCommand was NULL, the command was not found. */
		snprintf( outBuffer, bufferSize, "Command '%s' not recognised.  Enter 'help' to view a list of available commands."EXT_NEW_LINE EXT_NEW_LINE, cmdInput );
		ret = EXT_FALSE;
		currentCmd = NULL;
	}

	return ret;
}


char cmnCmdVersion(const struct _EXT_CLI_CMD *cmd, char *outBuffer, unsigned int bufferLen)
{
	strncpy( outBuffer, versionString, bufferLen );
	/* no more data*/
	return EXT_FALSE;
}


char cmnCmdHelp(const const struct _EXT_CLI_CMD *_cmd, char *outBuffer, unsigned int bufferLen)
{
	static const EXT_CLI_CMD_T *cmd = NULL;

	if( cmd == NULL )
	{
		cmd = extCmds;
	}

	/* Return the next command help string, before moving the pointer on to the next command in the list. */
	strncpy( outBuffer, cmd->helpString, bufferLen );
	cmd++;

	if( cmd->name == NULL )
	{/* no more data*/
		cmd = NULL;
		return EXT_FALSE;
	}

	/* more data needed */
	return EXT_TRUE;
}



