
/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"

/* Standard includes. */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "compact.h"
#include "lwipMux.h"

#include "jsmn.h"

#include "muxOs.h"

static char _cmdTaskStatsCommand(const struct _MUX_CLI_CMD *cmd,  char *outBuffer, size_t bufferLen)
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

	return MUX_FALSE;
}

static char _cmdLwIPStatsCommand(const struct _MUX_CLI_CMD *cmd,  char *outBuffer, size_t bufferLen)
{
	stats_display();

	return MUX_FALSE;
}



#if( configINCLUDE_QUERY_HEAP_COMMAND == 1 )
static char _cmdQueryHeapCommand(const struct _MUX_CLI_CMD *cmd,  char *outBuffer, size_t bufferLen)
{
	configASSERT( outBuffer );

	sprintf( outBuffer, "Current free heap %d bytes, minimum ever free heap %d bytes\r\n", ( int ) xPortGetFreeHeapSize(), ( int ) xPortGetMinimumEverFreeHeapSize() );
	return MUX_FALSE;
}

#endif /* configINCLUDE_QUERY_HEAP */


#if( configGENERATE_RUN_TIME_STATS == 1 )
	
static char _cmdRunTimeStatsCommand(const struct _MUX_CLI_CMD *cmd,  char *outBuffer, size_t bufferLen )
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

	return MUX_FALSE;
}
#endif /* configGENERATE_RUN_TIME_STATS */


static char cmdTestFpga(const struct _MUX_CLI_CMD *cmd, char *outBuffer, size_t bufferLen)
{
	unsigned int index = 0;
	unsigned char val;

//	muxBspFpgaReload();

//	muxFpgaInit();

//	muxFpgaConfig(&muxRun);
	if(!MUX_IS_TX(&muxRun) )
	{
		index += snprintf( outBuffer+index, (bufferLen-index), "'%s' 'enable/disable' not support in RX"MUX_NEW_LINE, cmd->name );
		return MUX_FALSE;
	}

	if(argc < 2)
	{
		index += snprintf( outBuffer+index, (bufferLen-index), "'"MUX_CMD_FPGA"' 'enable/disable' video transferring:%d"MUX_NEW_LINE, argc );
		FPGA_I2C_READ(MUX_FPGA_REG_ENABLE, &val, 1);
		index += snprintf( outBuffer+index, (bufferLen-index), "'"MUX_CMD_FPGA"' status : '%02x'"MUX_NEW_LINE, val);
		index += snprintf( outBuffer+index, (bufferLen-index), "%s"MUX_NEW_LINE, cmd->helpString );
		return MUX_FALSE;
	}

	if(strcasecmp(argv[1], MUX_CMD_DEBUG_ENABLE) == 0 )
	{
		index += snprintf( outBuffer+index, (bufferLen-index), "'"MUX_CMD_FPGA"' 'ENABLE'"MUX_NEW_LINE);
		muxFpgaEnable(1);
	}
	else if(strcasecmp(argv[1], MUX_CMD_DEBUG_DISABLE) == 0 )
	{
		index += snprintf( outBuffer+index, (bufferLen-index), "'"MUX_CMD_FPGA"' 'DISABLE'"MUX_NEW_LINE);
		muxFpgaEnable(0);
	}
	else
	{
		sprintf( outBuffer, "'%s' is not validate command"MUX_NEW_LINE,  argv[1]);
	}

//	index += snprintf(outBuffer+index, bufferLen-index, "%s"MUX_NEW_LINE, "FPGA testing" );

	return MUX_FALSE;
}


static char cmdFpgaRegisterRead(const struct _MUX_CLI_CMD *cmd, char *outBuffer, size_t bufferLen)
{
	unsigned int index = 0;
	unsigned int address = 0, size;
	unsigned char val;
	unsigned	int i;

	if(argc==1)
	{

		for(i=MUX_FPGA_REG_ETHERNET_RESET; i< MUX_FPGA_REG_DEST_PORT_ANC_ST+1; i++)
		{
			FPGA_I2C_READ(i, &val, 1);
			index += snprintf(outBuffer+index, bufferLen-index, "0x%2x: 0x%2x"MUX_NEW_LINE, i, val);
		}

		muxFgpaRegisterDebug();
		index += snprintf(outBuffer+index, bufferLen-index, "'%s [address [size]]'"MUX_NEW_LINE, argv[0] );
	}
	else if(argc >= 2)
	{
		address = (unsigned int)cmnParseGetHexIntValue(argv[1]);
		if(address > 0xFF)
		{
			index += snprintf(outBuffer+index, bufferLen-index, "Invalidate address: %u"MUX_NEW_LINE, address);
			return MUX_FALSE;
		}
		size = 1;

		if(argc >= 3)
		{
			size = (unsigned int)atoi(argv[2]);
			if(size <= 0 )
			{
				index += snprintf(outBuffer+index, bufferLen-index, "Invalidate size: %d"MUX_NEW_LINE, size);
				return MUX_FALSE;
			}
		}

		for(i=address; i< address+size; i++)
		{
			FPGA_I2C_READ(i, &val, 1);
			index += snprintf(outBuffer+index, bufferLen-index, "0x%2x: 0x%2x"MUX_NEW_LINE, i, val);
		}
		
		return MUX_FALSE;
	}
	else
	{
		index += snprintf(outBuffer+index, bufferLen-index, "'%s [address [size]]'"MUX_NEW_LINE, argv[0] );
	}

	index += snprintf(outBuffer+index, bufferLen-index, "%s"MUX_NEW_LINE, "FPGA read register!" );

	return MUX_FALSE;
}

static char cmdFpgaRegisterWrite(const struct _MUX_CLI_CMD *cmd, char *outBuffer, size_t bufferLen)
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
			index += snprintf(outBuffer+index, bufferLen-index, "Invalidate address: %u"MUX_NEW_LINE, address);
			return MUX_FALSE;
		}
		
		temp = (unsigned int)cmnParseGetHexIntValue(argv[2]);
		if(temp > 0xFF)
		{
			index += snprintf(outBuffer+index, bufferLen-index, "Invalidate register value: 0x%u"MUX_NEW_LINE, temp);
			return MUX_FALSE;
		}
		val = (unsigned char)temp;
		FPGA_I2C_WRITE((address), &val, 1);
		
		FPGA_I2C_READ((address), &readout, 1);
		index += snprintf(outBuffer+index, bufferLen-index, "0x%2x: 0x%2x. Write %s"MUX_NEW_LINE, address, readout, (readout==val)?"OK":"Failed");
	}
	else
	{
		index += snprintf(outBuffer+index, bufferLen-index, "'%s address value'"MUX_NEW_LINE, argv[0] );
	}

	index += snprintf(outBuffer+index, bufferLen-index, "%s"MUX_NEW_LINE, "FPGA write register!" );

	return MUX_FALSE;
}

#define	MAC_ADDRESS_OUTPUT(buffer, size, index, mac) \
		{(index) += snprintf((buffer)+(index), (size)-(index), "\"%02x:%02x:%02x:%02x:%02x:%02x\""MUX_NEW_LINE,  \
			(mac)->address[0] , (mac)->address[1] , (mac)->address[2] , (mac)->address[3], (mac)->address[4], (mac)->address[5] );}


static char muxCmdParamsDebug(const struct _MUX_CLI_CMD *cmd, char *outBuffer, size_t bufferLen)
{
	unsigned int index = 0;
	MUX_RUNTIME_CFG *runCfg = &muxRun;
	/*raw json string */

#if 0	
	index += snprintf(outBuffer+index, bufferLen-index, MUX_JSON_KEY_NAME"\t: %s"MUX_NEW_LINE,  runCfg->name);
	index += snprintf(outBuffer+index, bufferLen-index, MUX_JSON_KEY_MODEL"\t: %s"MUX_NEW_LINE,  runCfg->model);
	index += snprintf(outBuffer+index, bufferLen-index, MUX_JSON_KEY_VERSION"\t: %2d.%2d.%2d"MUX_NEW_LINE, runCfg->version.major, runCfg->version.minor, runCfg->version.revision);
	index += snprintf(outBuffer+index, bufferLen-index, "%s/%s"MUX_NEW_LINE, runCfg->user, runCfg->password);

#endif	
	index += snprintf(outBuffer+index, bufferLen-index, "MODE: %s, Multicast:%s"MUX_NEW_LINE, MUX_IS_TX(runCfg)?"TX":"RX", (runCfg->isMCast)?"YES":"NO");

#if 0
	index += snprintf(outBuffer+index, bufferLen-index, MUX_JSON_KEY_MAC"\t: ");
	MAC_ADDRESS_OUTPUT(outBuffer, bufferLen, index, &runCfg->macAddress);
	index += snprintf(outBuffer+index, bufferLen-index, MUX_JSON_KEY_IP"\t: %s"MUX_NEW_LINE,  MUX_LWIP_IPADD_TO_STR(&runCfg->ipAddress));
	index += snprintf(outBuffer+index, bufferLen-index, MUX_JSON_KEY_MASK"\t: %s"MUX_NEW_LINE,  MUX_LWIP_IPADD_TO_STR(&runCfg->ipMask));
	index += snprintf(outBuffer+index, bufferLen-index, MUX_JSON_KEY_GATEWAY"\t: %s"MUX_NEW_LINE,  MUX_LWIP_IPADD_TO_STR(&runCfg->ipGateway));
	index += snprintf(outBuffer+index, bufferLen-index, MUX_JSON_KEY_DHCP"\t: %s"MUX_NEW_LINE, MUX_DHCP_IS_ENABLE(runCfg)?"Yes":"No");
#endif

	index += snprintf(outBuffer+index, bufferLen-index, MUX_JSON_KEY_VIDEO_MAC_LOCAL"\t: ");
	MAC_ADDRESS_OUTPUT(outBuffer, bufferLen, index, &runCfg->local.mac );
	index += snprintf(outBuffer+index, bufferLen-index, MUX_JSON_KEY_VIDEO_IP_LOCAL"\t: %s"MUX_NEW_LINE,  MUX_LWIP_IPADD_TO_STR(&runCfg->local.ip));
	index += snprintf(outBuffer+index, bufferLen-index, MUX_JSON_KEY_VIDEO_PORT_LOCAL"\t: %d"MUX_NEW_LINE, runCfg->local.vport);
	index += snprintf(outBuffer+index, bufferLen-index, MUX_JSON_KEY_AUDIO_PORT_LOCAL"\t: %d"MUX_NEW_LINE, runCfg->local.aport);
	index += snprintf(outBuffer+index, bufferLen-index, MUX_JSON_KEY_ANC_DT_PORT_LOCAL"\t: %d"MUX_NEW_LINE, runCfg->local.dport);
	index += snprintf(outBuffer+index, bufferLen-index, MUX_JSON_KEY_ANC_ST_PORT_LOCAL"\t: %d"MUX_NEW_LINE, runCfg->local.sport);
	
	index += snprintf(outBuffer+index, bufferLen-index, MUX_JSON_KEY_VIDEO_MAC_DEST"\t: ");
	MAC_ADDRESS_OUTPUT(outBuffer, bufferLen, index, &runCfg->dest.mac);
	index += snprintf(outBuffer+index, bufferLen-index, MUX_JSON_KEY_VIDEO_IP_DEST"\t: %s"MUX_NEW_LINE,  MUX_LWIP_IPADD_TO_STR(&runCfg->dest.ip) );
	index += snprintf(outBuffer+index, bufferLen-index, MUX_JSON_KEY_VIDEO_PORT_DEST"\t: %d"MUX_NEW_LINE, runCfg->dest.vport);
	index += snprintf(outBuffer+index, bufferLen-index, MUX_JSON_KEY_AUDIO_PORT_DEST"\t: %d"MUX_NEW_LINE, runCfg->dest.aport);

	index += snprintf(outBuffer+index, bufferLen-index, MUX_JSON_KEY_ANC_DT_PORT_DEST"\t: %d"MUX_NEW_LINE, runCfg->dest.dport);
	index += snprintf(outBuffer+index, bufferLen-index, MUX_JSON_KEY_ANC_ST_PORT_DEST"\t: %d"MUX_NEW_LINE, runCfg->dest.sport);

	if(index>=bufferLen)
	{
//		return MUX_TRUE;
		return MUX_FALSE;
	}

	return MUX_FALSE;
}


static char muxCmdConfigureFpga(const struct _MUX_CLI_CMD *cmd,  char *outBuffer, size_t bufferLen )
{
	int index = 0;
	MUX_ASSERT(("Buffer is null"), outBuffer );

	/* Start with an empty string. */
	outBuffer[ 0 ] = 0x00;

	if(argc < 2)
	{
		index += snprintf( outBuffer+index, (bufferLen-index), "'%s' configuration is '%d'"MUX_NEW_LINE, cmd->name, muxRun.isConfigFpga);
		index += snprintf( outBuffer+index, (bufferLen-index), "%s"MUX_NEW_LINE, cmd->helpString );
		return MUX_FALSE;
	}

	index = (int)cmnParseGetHexIntValue(argv[1]);
	if(( index != 0 && muxRun.isConfigFpga == 0) || (index==0 && muxRun.isConfigFpga != 0) )
	{
		muxRun.isConfigFpga = (index==0)?MUX_FALSE:MUX_TRUE;
		bspCfgSave(&muxRun, MUX_CFG_MAIN);
		sprintf( outBuffer, "'%s' configured as '%s', reboot to make it active"MUX_NEW_LINE, cmd->name, (muxRun.isConfigFpga)?"Configure":"Not Configure" );
	}
	else
	{
		sprintf( outBuffer, "'%s' configuration is not changed"MUX_NEW_LINE, cmd->name );
	}


	
	return MUX_FALSE;
}



static char muxCmdTx(const struct _MUX_CLI_CMD *cmd,  char *outBuffer, size_t bufferLen )
{
	int index = 0;
	MUX_ASSERT(("Buffer is null"), outBuffer );

	/* Start with an empty string. */
	outBuffer[ 0 ] = 0x00;

	if(argc < 2)
	{
		index += snprintf( outBuffer+index, (bufferLen-index), "'%s' configuration is '%s'"MUX_NEW_LINE, cmd->name, MUX_IS_TX(&muxRun)?"TX":"RX");//, muxRun.isTx);
		index += snprintf( outBuffer+index, (bufferLen-index), "%s"MUX_NEW_LINE, cmd->helpString );
		return MUX_FALSE;
	}

	index = (int)cmnParseGetHexIntValue(argv[1]);
	if(( index != 0 && !MUX_IS_TX(&muxRun)) || (index==0 && MUX_IS_TX(&muxRun) ) )
	{
		muxRun.isTx = (index==0)?MUX_FALSE:MUX_TRUE;
		bspCfgSave(&muxRun, MUX_CFG_MAIN);
		sprintf( outBuffer, "'%s' configured as '%s', reboot to make it active"MUX_NEW_LINE, cmd->name, MUX_IS_TX(&muxRun)?"TX":"RX" );
	}
	else
	{
		sprintf( outBuffer, "'%s' configuration is not changed"MUX_NEW_LINE, cmd->name );
	}


	
	return MUX_FALSE;
}

/* added for delay reboot after factory. then console and telnet get infomation */
char muxCmdFactory(const struct _MUX_CLI_CMD *cmd,  char *outBuffer, size_t bufferLen )
{
	bspCmdFactory(cmd, outBuffer, bufferLen);

	muxDelayReboot(1000);

	return MUX_FALSE;
}


static char muxCmdDebuggable(const struct _MUX_CLI_CMD *cmd,  char *outBuffer, size_t bufferLen )
{
	unsigned int debugOption;
	int index = 0;
	MUX_ASSERT(("Buffer is null"), outBuffer );

	/* Start with an empty string. */
	outBuffer[ 0 ] = 0x00;

	debugOption = MUX_DEBUG_FLAG_IP_IN| MUX_DEBUG_FLAG_UDP_IN|MUX_DEBUG_FLAG_IGMP|MUX_DEBUG_FLAG_CMD;
	if(argc < 2)
	{
		index += snprintf( outBuffer+index, (bufferLen-index), "'"MUX_CMD_DEBUG"' configuration is '%08X'"MUX_NEW_LINE, muxRun.debugOptions);
		index += snprintf( outBuffer+index, (bufferLen-index), "%s"MUX_NEW_LINE, cmd->helpString );
		return MUX_FALSE;
	}

	if(strcasecmp(argv[1], MUX_CMD_DEBUG_ENABLE) == 0 )
	{
		MUX_DEBUG_SET_ENABLE(debugOption);
		index += snprintf( outBuffer+index, (bufferLen-index), "'%s' is Enabled"MUX_NEW_LINE,  argv[1]);
	}
	else if(strcasecmp(argv[1], MUX_CMD_DEBUG_DISABLE) == 0 )
	{
		MUX_DEBUG_SET_DISABLE(debugOption);
		index += snprintf( outBuffer+index, (bufferLen-index), "'%s' is Disabled"MUX_NEW_LINE,  argv[1]);
	}
	else
	{
		sprintf( outBuffer, "'%s' is not validate debug command"MUX_NEW_LINE,  argv[1]);
	}
	

	return MUX_FALSE;
}


_CODE MUX_CLI_CMD_T muxCmds[] =
{

	{
		name	: MUX_CMD_TASKS,
		helpString: MUX_NEW_LINE MUX_CMD_TASKS":"MUX_NEW_LINE" Displays a table showing the state of each FreeRTOS task"MUX_NEW_LINE,
		callback: _cmdTaskStatsCommand,
	},
	
	{
		name	: MUX_CMD_STATS,
		helpString: MUX_NEW_LINE MUX_CMD_STATS":"MUX_NEW_LINE" Displays statistics of LwIP"MUX_NEW_LINE,
		callback: _cmdLwIPStatsCommand,
	},

#if( configINCLUDE_QUERY_HEAP_COMMAND == 1 )
	{
		name	: MUX_CMD_HEAP,
		helpString: MUX_NEW_LINE MUX_CMD_HEAP":"MUX_NEW_LINE" Displays the free heap space, and minimum ever free heap space."MUX_NEW_LINE,
		callback: _cmdQueryHeapCommand,
	},
#endif

#if( configGENERATE_RUN_TIME_STATS == 1 )
	{
		name	: "run-time-stats",
		helpString: "\r\nrun-time-stats:"MUX_NEW_LINE" Displays a table showing how much processing time each FreeRTOS task has used"MUX_NEW_LINE,
		callback: _cmdRunTimeStatsCommand,
	},
#endif /* configGENERATE_RUN_TIME_STATS */
	

#if LWIP_EXT_UDP_TX_PERF
	{
		name	: MUX_CMD_UDP_PERF,
		helpString: MUX_NEW_LINE MUX_CMD_UDP_PERF" <ipaddress of UDP Perf server>:"MUX_NEW_LINE" for example, 192.168.168.102"MUX_NEW_LINE,
		callback: muxCmdUdpTxPerf,
	},

#endif
	{
		name	: MUX_CMD_PING,
		helpString: MUX_NEW_LINE MUX_CMD_PING" <ipaddress>:"MUX_NEW_LINE" for example, ping 192.168.168.1"MUX_NEW_LINE,
		callback: muxCmdNetPing,
	},

	{
		name	: MUX_CMD_IGMP,
		helpString: MUX_NEW_LINE MUX_CMD_IGMP" "MUX_CMD_IGMP_JOIN"|"MUX_CMD_IGMP_LEAVE" <Group Address>"MUX_NEW_LINE" "MUX_NEW_LINE,
		callback: muxCmdIgmp,
	},
	{
		name	: MUX_CMD_PARAMS,
		helpString: MUX_NEW_LINE MUX_CMD_PARAMS" :"MUX_NEW_LINE" Display all configuration and running options"MUX_NEW_LINE,
		callback: muxCmdParamsDebug,
	},

	{
		name	: MUX_CMD_NAME,
		helpString: MUX_NEW_LINE MUX_CMD_NAME" :"MUX_NEW_LINE" Modify device's name"MUX_NEW_LINE,
		callback: muxCmdChangeName,
	},

	{
		name	: MUX_CMD_NET_INFO,
		helpString: MUX_NEW_LINE MUX_CMD_NET_INFO" 1|0(DHCP) IP MASK GATEWAY(when DHCP=0) :"MUX_NEW_LINE" Display/Set network information"MUX_NEW_LINE,
		callback: muxCmdNetInfo,
	},

	{
		name	: MUX_CMD_MAC_INFO,
		helpString: MUX_NEW_LINE MUX_CMD_MAC_INFO" xx:xx:xx:xx:xx:xx :"MUX_NEW_LINE" Display/Set MAC address"MUX_NEW_LINE,
		callback: muxCmdMacInfo,
	},

#if 1
	{
		name	: MUX_CMD_LOCAL_INFO,
		helpString: MUX_NEW_LINE MUX_CMD_LOCAL_INFO" xx:xx:xx:xx:xx:xx xxx.xxx.xxx.xxx vPort aPort:"MUX_NEW_LINE" Display/Set configuration of Local on TX/RX"MUX_NEW_LINE,
		callback: muxCmdLocalInfo,
	},
#endif

	{
		name	: MUX_CMD_DEST_INFO,
		helpString: MUX_NEW_LINE MUX_CMD_DEST_INFO" xx:xx:xx:xx:xx:xx xxx.xxx.xxx.xxx vPort aPort:"MUX_NEW_LINE" Display/Set configuration of destination on TX"MUX_NEW_LINE,
		callback: muxCmdDestInfo,
	},


	{
		name	: MUX_CMD_FPGA,
		helpString: MUX_NEW_LINE MUX_CMD_FPGA" :"MUX_NEW_LINE" Enable/Disable media transfer"MUX_NEW_LINE,
		callback: cmdTestFpga,
	},

	{
		name	: MUX_CMD_FPGA_READ,
		helpString: MUX_NEW_LINE MUX_CMD_FPGA_READ" [address [size]]:"MUX_NEW_LINE" read register of FPGA. 'address' is hexadecimal, 'size' is decimal "MUX_NEW_LINE,
		callback: cmdFpgaRegisterRead,
	},

	{
		name	: MUX_CMD_FPGA_WRITE,
		helpString: MUX_NEW_LINE MUX_CMD_FPGA_WRITE" address value:"MUX_NEW_LINE" write register of FPGA. 'address' and 'value' is hexadecimal "MUX_NEW_LINE,
		callback: cmdFpgaRegisterWrite,
	},

	{
		name	: MUX_CMD_DEBUG,
		helpString: MUX_NEW_LINE MUX_CMD_DEBUG " "MUX_CMD_DEBUG_ENABLE"|"MUX_CMD_DEBUG_DISABLE" "MUX_NEW_LINE" Enable or Disable debug output"MUX_NEW_LINE,
		callback: muxCmdDebuggable,
	},

	{
		name	: MUX_CMD_CONFIG_FPGA,
		helpString: MUX_NEW_LINE MUX_CMD_CONFIG_FPGA " 1|0 "MUX_NEW_LINE" Configure FPGA or not when MCU reboot"MUX_NEW_LINE,
		callback: muxCmdConfigureFpga,
	},

	{
		name	: MUX_CMD_TX,
		helpString: MUX_NEW_LINE MUX_CMD_TX " 1|0 "MUX_NEW_LINE" Set as TX or RX mode"MUX_NEW_LINE,
		callback: muxCmdTx,
	},

	{
		name	: MUX_CMD_UPDATE,
		helpString: CMD_HELP_UPDATE,
		callback: bspCmdUpdate,
	},
	
	{
		name	: MUX_CMD_FACTORY,
		helpString: CMD_HELP_FACTORY,
		callback: muxCmdFactory,
	},

	{
		name	: MUX_CMD_REBOOT,
		helpString: CMD_HELP_REBOOT,
		callback: bspCmdReboot,
	},

	{
		name	: MUX_CMD_EFC_FLASH,
		helpString: CMD_HELP_EFC_FLASH,
		callback: bspCmdInternalFlash,
	},

	{
		name	: MUX_CMD_SPI_FLASH_READ,
		helpString: CMD_HELP_SPI_FLASH,
		callback: bspCmdSpiFlashRead,
	},
	{
		name	: MUX_CMD_SPI_FLASH_ERASE,
		helpString: CMD_HELP_SPI_FLASH_ERASE,
		callback: bspCmdSpiFlashErase,
	},

#if 0
	{
		name	: MUX_CMD_LOAD_FPGA_X,
		helpString: CMD_HELP_LOAD_FPGA_X,
		callback: bspCmdSpiFlashXmodemLoad,
	},

	{
		name	: MUX_CMD_LOAD_FPGA_Y,
		helpString: CMD_HELP_LOAD_FPGA_Y,
		callback: bspCmdSpiFlashYmodemLoad,
	},
#endif

	{
		name	: MUX_CMD_BIST,
		helpString: CMD_HELP_BIST,
		callback: bspCmdBIST,
	},
	
	{
		name	: MUX_CMD_VERSION,
		helpString: CMD_HELP_VERSION,
		callback: bspCmdVersion,
	},
	{
		name	: MUX_CMD_DEFAULT,
		helpString: CMD_HELP_HELP,
		callback: bspCmdHelp,
	},

	{
		name	: NULL,
		helpString: NULL,
		callback: NULL,
	}

};

