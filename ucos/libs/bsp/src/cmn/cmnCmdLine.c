
#include "ext.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if 1//TASK_CONSOLE_EN

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

#if 0
char cmnCmdVersion(const struct _EXT_CLI_CMD *cmd, char *outBuffer, unsigned int bufferLen)
{
	strncpy( outBuffer, versionString, bufferLen );
	/* no more data*/
	return EXT_FALSE;
}

void stats_display(char *outBuffer, size_t bufferLen)
{
	int i;
	int index = 0;

#if 0
	struct stats_igmp *igmp = &lwip_stats.igmp;
	struct stats_mem *mem;
#if 1	
	struct stats_syselem *sys;
#endif
	index += snprintf( outBuffer+index, (bufferLen-index), "PROTOCOLS:"EXT_NEW_LINE"\tXmit\tRecv\tFw\tDrop\tChkErr\tLenErr\tMemErr\tRtErr\tProtErr\tOptErr\tErr\tCacheHit"EXT_NEW_LINE);
	STATS_OUT_PROTOCOL((&lwip_stats.link), "LINK");
	STATS_OUT_PROTOCOL((&lwip_stats.etharp), "ETHARP");
	STATS_OUT_PROTOCOL((&lwip_stats.ip_frag), "IP_FRAG");
	STATS_OUT_PROTOCOL((&lwip_stats.ip), "IP");
//	STATS_OUT_PROTOCOL((&lwip_stats.igmp), "IGMP");
	STATS_OUT_PROTOCOL((&lwip_stats.icmp), "ICMP");
	STATS_OUT_PROTOCOL((&lwip_stats.udp), "UDP");
	STATS_OUT_PROTOCOL((&lwip_stats.tcp), "TCP");

	index += snprintf( outBuffer+index, (bufferLen-index),"IGMP:\tXmit:%"STAT_COUNTER_F";\tRecv:%"STAT_COUNTER_F";\t\tDrop:%"\
		STAT_COUNTER_F";\tChkErr:%"STAT_COUNTER_F";\tLenErr:%"STAT_COUNTER_F";\tMemErr:%"STAT_COUNTER_F";\t" \
		"ProtErr:%"STAT_COUNTER_F ";"LWIP_NEW_LINE"\tRxV1:%"STAT_COUNTER_F";\tRxGroup:%"STAT_COUNTER_F";\tRxGeneral:%"\
		STAT_COUNTER_F";\tRxReport:%"STAT_COUNTER_F";\tTxJoin:%"STAT_COUNTER_F";\tTxLeave:%"STAT_COUNTER_F";\tTxReport:%"STAT_COUNTER_F""LWIP_NEW_LINE LWIP_NEW_LINE LWIP_NEW_LINE,
		igmp->xmit, igmp->recv, igmp->drop, igmp->chkerr, igmp->lenerr, igmp->memerr, igmp->proterr, 
		igmp->rx_v1, igmp->rx_group, igmp->rx_general, igmp->rx_report, igmp->tx_join, igmp->tx_leave, igmp->tx_report);


	index += snprintf( outBuffer+index, (bufferLen-index), "MEMORY:"LWIP_NEW_LINE"\t\tAvail \tUsed \tMax \tErr"LWIP_NEW_LINE);

	STATS_OUT_MEM((&lwip_stats.mem), "HEAP");
	for (i = 0; i < MEMP_MAX; i++)
	{
		mem = lwip_stats.memp[i];
		STATS_OUT_MEM(mem, mem->name);
	}
#endif

#ifdef	ARM
	struct MAC_STATS *macStats = &extRun.macStats;
	index += snprintf( outBuffer+index, (bufferLen-index), EXT_NEW_LINE"MAC:"EXT_NEW_LINE"\tIRQ: %ld; Rx IRQ: %ld"EXT_NEW_LINE"\tTX: packets:%ld; Failed:%ld" EXT_NEW_LINE"\tRX: packets:%ld"
		"; Error:frame: %ld; Mem:%ld; Overrun:%ld; Ownership:%ld"EXT_NEW_LINE,
		macStats->isrCount, macStats->isrRecvCount, macStats->txPackets, macStats->txFailed, 
		macStats->rxPackets, macStats->rxErrFrame, macStats->rxErrOOM, macStats->rxErrOverrun, macStats->rxErrOwnership);
#endif

#if 0
#if 1
	index += snprintf( outBuffer+index, (bufferLen-index),EXT_NEW_LINE EXT_NEW_LINE"SYSTEM:"EXT_NEW_LINE"\tused\tmax\terr"EXT_NEW_LINE);
	sys = &lwip_stats.sys.sem;
	STATS_OUT_SYS((sys), "Sem");
	sys = &lwip_stats.sys.mutex;
	STATS_OUT_SYS((sys), "Mutex");
	sys = &lwip_stats.sys.mbox;
	STATS_OUT_SYS((sys), "mbox");
#endif

#ifdef X86
TRACE();
	LINK_STATS_DISPLAY();
	ETHARP_STATS_DISPLAY();
	IPFRAG_STATS_DISPLAY();
	IP6_FRAG_STATS_DISPLAY();
	IP_STATS_DISPLAY();
	ND6_STATS_DISPLAY();
	IP6_STATS_DISPLAY();
	IGMP_STATS_DISPLAY();
	MLD6_STATS_DISPLAY();
	ICMP_STATS_DISPLAY();
	ICMP6_STATS_DISPLAY();
	UDP_STATS_DISPLAY();
	TCP_STATS_DISPLAY();
	
	MEM_STATS_DISPLAY();

	for (i = 0; i < MEMP_MAX; i++)
	{
		MEMP_STATS_DISPLAY(i);
	}
	SYS_STATS_DISPLAY();

TRACE();
#endif
#endif

}
#endif

char cmnCmdNetStats(const struct _EXT_CLI_CMD *cmd,  char *outBuffer, unsigned int bufferLen)
{
	stats_display(outBuffer, bufferLen);

#if 0
	extEtherDebug();
#endif

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

char cmnCmdVersion(const struct _EXT_CLI_CMD *cmd, char *outBuffer, unsigned int bufferLen)
{
	strncpy( outBuffer, versionString, bufferLen );
	/* no more data*/
	return EXT_FALSE;
}


#endif

