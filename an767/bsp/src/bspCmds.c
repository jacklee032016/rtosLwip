
/*
* command line interface and commands used in both bootloader and RTOS
*/

#include "compact.h"

int 		argc = 0;
char		argv[MUX_CMD_MAX_ARGUMENTS][MUX_CMD_MAX_LENGTH];


static int8_t _muxGetOneArgument(const char *start, const char *end)
{
	int length = end - start;

	if(argc<MUX_CMD_MAX_ARGUMENTS && length <= MUX_CMD_MAX_LENGTH-1 )
	{
		memcpy(argv[argc], start, length);
		argv[argc][length] = 0;
		argc++;
	}
	else
	{
		printf("No.%d argument %s (length %d) is ignored"MUX_NEW_LINE, argc, end, length);
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}


static void _cmdParseParameters( const char *cmdStr )
{
	int i;
	const char *start = cmdStr;

	argc = 0;
	for(i=0; i<MUX_CMD_MAX_ARGUMENTS; i++)
	{
		argv[i][0] = 0;
	}
	
	/* Count the number of space delimited words in pcCommandString. */
	while( *cmdStr != 0x00 )
	{
		if( ( *cmdStr ) == ' ' ||(*cmdStr ) == ASCII_KEY_TAB /* TAB */ )
		{
			_muxGetOneArgument(start, cmdStr);

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
		_muxGetOneArgument(start, cmdStr);
	}
}


char bspCmdProcess( const char * const cmdInput, char *outBuffer, size_t bufferSize  )
{
	static const MUX_CLI_CMD_T *currentCmd = NULL;
	char ret = MUX_TRUE;
	
	_cmdParseParameters(cmdInput);
	if(argc == 0)
	{
		printf("No command is defined"MUX_NEW_LINE);
	}

	/* Note:  This function is not re-entrant.  It must not be called from more thank one task. */
	if( currentCmd == NULL )
	{
		currentCmd = muxCmds;

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

		printf( "Total %d argument in '%s':"MUX_NEW_LINE MUX_NEW_LINE, argc, cmdInput);
		for(i=0; i< MUX_CMD_MAX_ARGUMENTS; i++)
		{
			printf( "No.%d argument: '%s' "MUX_NEW_LINE, i, argv[i] );
		}
	}
#endif

	if( currentCmd->name != NULL )
	{/* Call the callback function */
		ret = currentCmd->callback(currentCmd, outBuffer, bufferSize );
		if( ret == MUX_FALSE )
		{
			currentCmd = NULL;
		}
	}
	else
	{/* pxCommand was NULL, the command was not found. */
		snprintf( outBuffer, bufferSize, "Command '%s' not recognised.  Enter 'help' to view a list of available commands."MUX_NEW_LINE MUX_NEW_LINE, cmdInput );
		ret = MUX_FALSE;
		currentCmd = NULL;
	}

	return ret;
}

char	 bspCmdFactory(const struct _MUX_CLI_CMD *cmd, char *outBuffer, size_t bufferLen )
{
	muxCfgFromFactory(&muxRun);
	bspCfgSave(&muxRun, MUX_CFG_MAIN);

	/* this function never return to its caller, so must output directly */
	if(outBuffer && bufferLen> 0 )
	{
		snprintf( outBuffer, (bufferLen),  "Factory configuration has been restored, reboot....."MUX_NEW_LINE MUX_NEW_LINE );
	}
	else
	{
		printf( "Factory configuration has been restored, reboot....."MUX_NEW_LINE MUX_NEW_LINE);
	}

#if 0
	/* CPU is busy for delay, so no output  */
	MUX_DELAY_MS(500); /* make output (serial port or telnet) available */
#else
#endif
	if(muxRun.bootMode == BOOT_MODE_BOOTLOADER)
	{/* for RTOS, it is delayed reboot */
		MUX_REBOOT();
	}
	
	return MUX_FALSE;
}

char bspCmdUpdate(const struct _MUX_CLI_CMD *cmd,  char *outBuffer, size_t bufferLen )
{
	int index = 0;
	
	/* Start with an empty string. */
	outBuffer[ 0 ] = 0x00;

	if(argc < 2)
	{
		index += snprintf( outBuffer+index, (bufferLen-index), "'%s' configuration is '%d'"MUX_NEW_LINE, cmd->name, (muxRun.isUpdate) );
		index += snprintf( outBuffer+index, (bufferLen-index), "%s" MUX_NEW_LINE, cmd->helpString);
		return MUX_FALSE;
	}

	index = ( int)atoi(argv[1]);
	if(( index != 0 && muxRun.isUpdate == MUX_FALSE ) || (index==0 && muxRun.isUpdate== MUX_TRUE) )
	{
		muxRun.isUpdate = !muxRun.isUpdate;
		bspCfgSave(&muxRun, MUX_CFG_MAIN);
		sprintf( outBuffer, "'%s' configured as '%d', reboot to make it active"MUX_NEW_LINE, cmd->name, muxRun.isUpdate );
	}
	else
	{
		sprintf( outBuffer, "'%s' configuration is not changed"MUX_NEW_LINE, cmd->name );
	}
	
	return MUX_FALSE;
}


char	 bspCmdReboot(const struct _MUX_CLI_CMD *cmd, char *outBuffer, size_t bufferLen )
{
#if 0
	MUX_DELAY_MS(1000);

	if(outBuffer)
	{
		strncpy( outBuffer, "rebooting.....", bufferLen );
	}
#else
	printf("rebooting....."MUX_NEW_LINE MUX_NEW_LINE );
#endif

	MUX_REBOOT();
	return MUX_FALSE;
}

char bspCmdVersion(const struct _MUX_CLI_CMD *cmd, char *outBuffer, size_t bufferLen)
{
	strncpy( outBuffer, versionString, bufferLen );
	/* no more data*/
	return MUX_FALSE;
}


char bspCmdHelp(const const struct _MUX_CLI_CMD *_cmd, char *outBuffer, size_t bufferLen)
{
	static const MUX_CLI_CMD_T *cmd = NULL;

	if( cmd == NULL )
	{
		cmd = muxCmds;
	}

	/* Return the next command help string, before moving the pointer on to the next command in the list. */
	strncpy( outBuffer, cmd->helpString, bufferLen );
	cmd++;

	if( cmd->name == NULL )
	{/* no more data*/
		cmd = NULL;
		return MUX_FALSE;
	}

	/* more data needed */
	return MUX_TRUE;
}


