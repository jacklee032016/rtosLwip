
/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"

/* Standard includes. */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "compact.h"
#include "lwipExt.h"

#include "jsmn.h"

#include "eos.h"

static char _cmdTaskStatsCommand(const struct _EXT_CLI_CMD *cmd,  char *outBuffer, size_t bufferLen)
{
	const char *const pcHeader = "     State   Priority  Stack    #\r\n************************************************\r\n";
	BaseType_t xSpacePadding;

	configASSERT( outBuffer );

	/* Generate a table of task stats. */
	strcpy( outBuffer, "Task" );
	outBuffer += strlen( outBuffer );

	/* Minus three for the null terminator and half the number of characters in
	"Task" so the column lines up with the centre of the heading. */
	configASSERT( configMAX_TASK_NAME_LEN > 3 );
	for( xSpacePadding = strlen( "Task" ); xSpacePadding < ( configMAX_TASK_NAME_LEN - 3 ); xSpacePadding++ )
	{
		/* Add a space to align columns after the task's name. */
		*outBuffer = ' ';
		outBuffer++;

		/* Ensure always terminated. */
		*outBuffer = 0x00;
	}
	strcpy( outBuffer, pcHeader );
	vTaskList( outBuffer + strlen( pcHeader ) );

	return EXT_FALSE;
}



#if( configINCLUDE_QUERY_HEAP_COMMAND == 1 )
static char _cmdQueryHeapCommand(const struct _EXT_CLI_CMD *cmd,  char *outBuffer, size_t bufferLen)
{
	configASSERT( outBuffer );

	sprintf( outBuffer, "Current free heap %d bytes, minimum ever free heap %d bytes\r\n", ( int ) xPortGetFreeHeapSize(), ( int ) xPortGetMinimumEverFreeHeapSize() );
	return EXT_FALSE;
}

#endif /* configINCLUDE_QUERY_HEAP */


#if( configGENERATE_RUN_TIME_STATS == 1 )
	
static char _cmdRunTimeStatsCommand(const struct _EXT_CLI_CMD *cmd,  char *outBuffer, size_t bufferLen )
{
	const char * const pcHeader = "  Abs Time      % Time\r\n****************************************\r\n";
	BaseType_t xSpacePadding;

	configASSERT( outBuffer );

	/* Generate a table of task stats. */
	strcpy( outBuffer, "Task" );
	outBuffer += strlen( outBuffer );

	/* Pad the string "task" with however many bytes necessary to make it the
	length of a task name.  Minus three for the null terminator and half the
	number of characters in	"Task" so the column lines up with the centre of
	the heading. */
	for( xSpacePadding = strlen( "Task" ); xSpacePadding < ( configMAX_TASK_NAME_LEN - 3 ); xSpacePadding++ )
	{
		/* Add a space to align columns after the task's name. */
		*outBuffer = ' ';
		outBuffer++;

		/* Ensure always terminated. */
		*outBuffer = 0x00;
	}

	strcpy( outBuffer, pcHeader );
	vTaskGetRunTimeStats( outBuffer + strlen( pcHeader ) );

	return EXT_FALSE;
}
#endif /* configGENERATE_RUN_TIME_STATS */


static char cmdTestFpga(const struct _EXT_CLI_CMD *cmd, char *outBuffer, size_t bufferLen)
{
	unsigned int index = 0;
	unsigned char val;

//	extBspFpgaReload();

//	extFpgaInit();

//	extFpgaConfig(&extRun);
	if(!EXT_IS_TX(&extRun) )
	{
		index += snprintf( outBuffer+index, (bufferLen-index), "'%s' 'enable/disable' not support in RX"EXT_NEW_LINE, cmd->name );
		return EXT_FALSE;
	}

	if(argc < 2)
	{
		index += snprintf( outBuffer+index, (bufferLen-index), "'"EXT_CMD_FPGA"' 'enable/disable' video transferring:%d"EXT_NEW_LINE, argc );
		FPGA_I2C_READ(EXT_FPGA_REG_ENABLE, &val, 1);
		index += snprintf( outBuffer+index, (bufferLen-index), "'"EXT_CMD_FPGA"' status : '%02x'"EXT_NEW_LINE, val);
		index += snprintf( outBuffer+index, (bufferLen-index), "%s"EXT_NEW_LINE, cmd->helpString );
		return EXT_FALSE;
	}

	if(strcasecmp(argv[1], EXT_CMD_DEBUG_ENABLE) == 0 )
	{
		index += snprintf( outBuffer+index, (bufferLen-index), "'"EXT_CMD_FPGA"' 'ENABLE'"EXT_NEW_LINE);
		extFpgaEnable(1);
	}
	else if(strcasecmp(argv[1], EXT_CMD_DEBUG_DISABLE) == 0 )
	{
		index += snprintf( outBuffer+index, (bufferLen-index), "'"EXT_CMD_FPGA"' 'DISABLE'"EXT_NEW_LINE);
		extFpgaEnable(0);
	}
	else
	{
		sprintf( outBuffer, "'%s' is not validate command"EXT_NEW_LINE,  argv[1]);
	}

//	index += snprintf(outBuffer+index, bufferLen-index, "%s"EXT_NEW_LINE, "FPGA testing" );

	return EXT_FALSE;
}


static char cmdFpgaRegisterRead(const struct _EXT_CLI_CMD *cmd, char *outBuffer, size_t bufferLen)
{
	unsigned int index = 0;
	unsigned int address = 0, size;
	unsigned char val;
	unsigned	int i;

	if(argc==1)
	{

		for(i=EXT_FPGA_REG_ETHERNET_RESET; i< EXT_FPGA_REG_DEST_PORT_ANC_ST+1; i++)
		{
			FPGA_I2C_READ(i, &val, 1);
			index += snprintf(outBuffer+index, bufferLen-index, "0x%2x: 0x%2x"EXT_NEW_LINE, i, val);
		}

		extFgpaRegisterDebug();
		index += snprintf(outBuffer+index, bufferLen-index, "'%s [address [size]]'"EXT_NEW_LINE, argv[0] );
	}
	else if(argc >= 2)
	{
		address = (unsigned int)cmnParseGetHexIntValue(argv[1]);
		if(address > 0xFF)
		{
			index += snprintf(outBuffer+index, bufferLen-index, "Invalidate address: %u"EXT_NEW_LINE, address);
			return EXT_FALSE;
		}
		size = 1;

		if(argc >= 3)
		{
			size = (unsigned int)atoi(argv[2]);
			if(size <= 0 )
			{
				index += snprintf(outBuffer+index, bufferLen-index, "Invalidate size: %d"EXT_NEW_LINE, size);
				return EXT_FALSE;
			}
		}

		for(i=address; i< address+size; i++)
		{
			FPGA_I2C_READ(i, &val, 1);
			index += snprintf(outBuffer+index, bufferLen-index, "0x%2x: 0x%2x"EXT_NEW_LINE, i, val);
		}
		
		return EXT_FALSE;
	}
	else
	{
		index += snprintf(outBuffer+index, bufferLen-index, "'%s [address [size]]'"EXT_NEW_LINE, argv[0] );
	}

	index += snprintf(outBuffer+index, bufferLen-index, "%s"EXT_NEW_LINE, "FPGA read register!" );

	return EXT_FALSE;
}

static char cmdFpgaRegisterWrite(const struct _EXT_CLI_CMD *cmd, char *outBuffer, size_t bufferLen)
{
	unsigned int index = 0;
	unsigned int address = 0, temp;
	unsigned char val;

	if(argc >= 3)
	{
		unsigned char readout;
		address = (unsigned int)cmnParseGetHexIntValue(argv[1]);
		if(address > 0xFF)
		{
			index += snprintf(outBuffer+index, bufferLen-index, "Invalidate address: %u"EXT_NEW_LINE, address);
			return EXT_FALSE;
		}
		
		temp = (unsigned int)cmnParseGetHexIntValue(argv[2]);
		if(temp > 0xFF)
		{
			index += snprintf(outBuffer+index, bufferLen-index, "Invalidate register value: 0x%u"EXT_NEW_LINE, temp);
			return EXT_FALSE;
		}
		val = (unsigned char)temp;
		FPGA_I2C_WRITE((address), &val, 1);
		
		FPGA_I2C_READ((address), &readout, 1);
		index += snprintf(outBuffer+index, bufferLen-index, "0x%2x: 0x%2x. Write %s"EXT_NEW_LINE, address, readout, (readout==val)?"OK":"Failed");
	}
	else
	{
		index += snprintf(outBuffer+index, bufferLen-index, "'%s address value'"EXT_NEW_LINE, argv[0] );
	}

	index += snprintf(outBuffer+index, bufferLen-index, "%s"EXT_NEW_LINE, "FPGA write register!" );

	return EXT_FALSE;
}


static char extCmdConfigureFpga(const struct _EXT_CLI_CMD *cmd,  char *outBuffer, size_t bufferLen )
{
	int index = 0;
	EXT_ASSERT(("Buffer is null"), outBuffer );

	/* Start with an empty string. */
	outBuffer[ 0 ] = 0x00;

	if(argc < 2)
	{
		index += snprintf( outBuffer+index, (bufferLen-index), "'%s' configuration is '%d'"EXT_NEW_LINE, cmd->name, extRun.isConfigFpga);
		index += snprintf( outBuffer+index, (bufferLen-index), "%s"EXT_NEW_LINE, cmd->helpString );
		return EXT_FALSE;
	}

	index = (int)cmnParseGetHexIntValue(argv[1]);
	if(( index != 0 && extRun.isConfigFpga == 0) || (index==0 && extRun.isConfigFpga != 0) )
	{
		extRun.isConfigFpga = (index==0)?EXT_FALSE:EXT_TRUE;
		bspCfgSave(&extRun, EXT_CFG_MAIN);
		sprintf( outBuffer, "'%s' configured as '%s', reboot to make it active"EXT_NEW_LINE, cmd->name, (extRun.isConfigFpga)?"Configure":"Not Configure" );
	}
	else
	{
		sprintf( outBuffer, "'%s' configuration is not changed"EXT_NEW_LINE, cmd->name );
	}


	
	return EXT_FALSE;
}

/* added for delay reboot after factory. then console and telnet get infomation */
char extCmdFactory(const struct _EXT_CLI_CMD *cmd,  char *outBuffer, size_t bufferLen )
{
	bspCmdFactory(cmd, outBuffer, bufferLen);

	extDelayReboot(1000);

	return EXT_FALSE;
}


_CODE EXT_CLI_CMD_T extCmds[] =
{

	{
		name	: EXT_CMD_TASKS,
		helpString: EXT_NEW_LINE EXT_CMD_TASKS":"EXT_NEW_LINE" Displays a table showing the state of each FreeRTOS task"EXT_NEW_LINE,
		callback: _cmdTaskStatsCommand,
	},
	
	{
		name	: EXT_CMD_STATS,
		helpString: EXT_NEW_LINE EXT_CMD_STATS":"EXT_NEW_LINE" Displays statistics of LwIP"EXT_NEW_LINE,
		callback: cmnCmdLwipStats,
	},

#if( configINCLUDE_QUERY_HEAP_COMMAND == 1 )
	{
		name	: EXT_CMD_HEAP,
		helpString: EXT_NEW_LINE EXT_CMD_HEAP":"EXT_NEW_LINE" Displays the free heap space, and minimum ever free heap space."EXT_NEW_LINE,
		callback: _cmdQueryHeapCommand,
	},
#endif

#if( configGENERATE_RUN_TIME_STATS == 1 )
	{
		name	: "run-time-stats",
		helpString: "\r\nrun-time-stats:"EXT_NEW_LINE" Displays a table showing how much processing time each FreeRTOS task has used"EXT_NEW_LINE,
		callback: _cmdRunTimeStatsCommand,
	},
#endif /* configGENERATE_RUN_TIME_STATS */

	{
		name	: EXT_CMD_TIME,
		helpString: EXT_NEW_LINE EXT_CMD_TIME":"EXT_NEW_LINE" Displays current time"EXT_NEW_LINE,
		callback: cmnCmdTime,
	},
	

#if LWIP_EXT_UDP_TX_PERF
	{
		name	: EXT_CMD_UDP_PERF,
		helpString: EXT_NEW_LINE EXT_CMD_UDP_PERF" <ipaddress of UDP Perf server>:"EXT_NEW_LINE" for example, 192.168.168.102"EXT_NEW_LINE,
		callback: extCmdUdpTxPerf,
	},

#endif
	{
		name	: EXT_CMD_PING,
		helpString: EXT_NEW_LINE EXT_CMD_PING" <ipaddress>:"EXT_NEW_LINE" for example, ping 192.168.168.1"EXT_NEW_LINE,
		callback: cmnCmdLwipPing,
	},

	{
		name	: EXT_CMD_IGMP,
		helpString: EXT_NEW_LINE EXT_CMD_IGMP" "EXT_CMD_IGMP_JOIN"|"EXT_CMD_IGMP_LEAVE" <Group Address>"EXT_NEW_LINE" "EXT_NEW_LINE,
		callback: cmnCmdLwipIgmp,
	},
	{
		name	: EXT_CMD_PARAMS,
		helpString: EXT_NEW_LINE EXT_CMD_PARAMS" :"EXT_NEW_LINE" Display all configuration and running options"EXT_NEW_LINE,
		callback: cmnCmdParams,
	},

	{
		name	: EXT_CMD_NAME,
		helpString: EXT_NEW_LINE EXT_CMD_NAME" :"EXT_NEW_LINE" Modify device's name"EXT_NEW_LINE,
		callback: extCmdChangeName,
	},

	{
		name	: EXT_CMD_NET_INFO,
		helpString: EXT_NEW_LINE EXT_CMD_NET_INFO" 1|0(DHCP) IP MASK GATEWAY(when DHCP=0) :"EXT_NEW_LINE" Display/Set network information"EXT_NEW_LINE,
		callback: cmnCmdNetInfo,
	},

	{
		name	: EXT_CMD_MAC_INFO,
		helpString: EXT_NEW_LINE EXT_CMD_MAC_INFO" xx:xx:xx:xx:xx:xx :"EXT_NEW_LINE" Display/Set MAC address"EXT_NEW_LINE,
		callback: cmnCmdMacInfo,
	},

#if 1
	{
		name	: EXT_CMD_LOCAL_INFO,
		helpString: EXT_NEW_LINE EXT_CMD_LOCAL_INFO" xx:xx:xx:xx:xx:xx xxx.xxx.xxx.xxx vPort aPort:"EXT_NEW_LINE" Display/Set configuration of Local on TX/RX"EXT_NEW_LINE,
		callback: cmnCmdLocalInfo,
	},
#endif

	{
		name	: EXT_CMD_DEST_INFO,
		helpString: EXT_NEW_LINE EXT_CMD_DEST_INFO" xx:xx:xx:xx:xx:xx xxx.xxx.xxx.xxx vPort aPort:"EXT_NEW_LINE" Display/Set configuration of destination on TX"EXT_NEW_LINE,
		callback: cmnCmdDestInfo,
	},


	{
		name	: EXT_CMD_FPGA,
		helpString: EXT_NEW_LINE EXT_CMD_FPGA" :"EXT_NEW_LINE" Enable/Disable media transfer"EXT_NEW_LINE,
		callback: cmdTestFpga,
	},

	{
		name	: EXT_CMD_FPGA_READ,
		helpString: EXT_NEW_LINE EXT_CMD_FPGA_READ" [address [size]]:"EXT_NEW_LINE" read register of FPGA. 'address' is hexadecimal, 'size' is decimal "EXT_NEW_LINE,
		callback: cmdFpgaRegisterRead,
	},

	{
		name	: EXT_CMD_FPGA_WRITE,
		helpString: EXT_NEW_LINE EXT_CMD_FPGA_WRITE" address value:"EXT_NEW_LINE" write register of FPGA. 'address' and 'value' is hexadecimal "EXT_NEW_LINE,
		callback: cmdFpgaRegisterWrite,
	},

	{
		name	: EXT_CMD_DEBUG,
		helpString: EXT_NEW_LINE EXT_CMD_DEBUG " "EXT_CMD_DEBUG_ENABLE"|"EXT_CMD_DEBUG_DISABLE" "EXT_NEW_LINE" Enable or Disable debug output"EXT_NEW_LINE,
		callback: cmdCmdDebuggable,
	},

	{
		name	: EXT_CMD_CONFIG_FPGA,
		helpString: EXT_NEW_LINE EXT_CMD_CONFIG_FPGA " 1|0 "EXT_NEW_LINE" Configure FPGA or not when MCU reboot"EXT_NEW_LINE,
		callback: extCmdConfigureFpga,
	},

	{
		name	: EXT_CMD_TX,
		helpString: EXT_NEW_LINE EXT_CMD_TX " 1|0 "EXT_NEW_LINE" Set as TX or RX mode"EXT_NEW_LINE,
		callback: cmnCmdTx,
	},

	{
		name	: EXT_CMD_UPDATE,
		helpString: CMD_HELP_UPDATE,
		callback: cmnCmdUpdate,
	},
	
	{
		name	: EXT_CMD_FACTORY,
		helpString: CMD_HELP_FACTORY,
		callback: extCmdFactory,
	},

	{
		name	: EXT_CMD_REBOOT,
		helpString: CMD_HELP_REBOOT,
		callback: bspCmdReboot,
	},

	{
		name	: EXT_CMD_EFC_FLASH,
		helpString: CMD_HELP_EFC_FLASH,
		callback: bspCmdInternalFlash,
	},

	{
		name	: EXT_CMD_SPI_FLASH_READ,
		helpString: CMD_HELP_SPI_FLASH,
		callback: bspCmdSpiFlashRead,
	},
	{
		name	: EXT_CMD_SPI_FLASH_ERASE,
		helpString: CMD_HELP_SPI_FLASH_ERASE,
		callback: bspCmdSpiFlashErase,
	},

#if 0
	{
		name	: EXT_CMD_LOAD_FPGA_X,
		helpString: CMD_HELP_LOAD_FPGA_X,
		callback: bspCmdSpiFlashXmodemLoad,
	},

	{
		name	: EXT_CMD_LOAD_FPGA_Y,
		helpString: CMD_HELP_LOAD_FPGA_Y,
		callback: bspCmdSpiFlashYmodemLoad,
	},
#endif

	{
		name	: EXT_CMD_BIST,
		helpString: CMD_HELP_BIST,
		callback: bspCmdBIST,
	},
	
	{
		name	: EXT_CMD_VERSION,
		helpString: CMD_HELP_VERSION,
		callback: cmnCmdVersion,
	},
	{
		name	: EXT_CMD_DEFAULT,
		helpString: CMD_HELP_HELP,
		callback: cmnCmdHelp,
	},

	{
		name	: NULL,
		helpString: NULL,
		callback: NULL,
	}

};

