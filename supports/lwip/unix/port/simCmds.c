
#include "extSysParams.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lwipExt.h"


_CODE MUX_CLI_CMD_T muxCmds[] =
{
	{
		name	: MUX_CMD_STATS,
		helpString: MUX_NEW_LINE MUX_CMD_STATS":"MUX_NEW_LINE" Displays statistics of LwIP"MUX_NEW_LINE,
		callback: cmnCmdLwipStats,
	},


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
		callback: cmnCmdLwipPing,
	},

	{
		name	: MUX_CMD_IGMP,
		helpString: MUX_NEW_LINE MUX_CMD_IGMP" "MUX_CMD_IGMP_JOIN"|"MUX_CMD_IGMP_LEAVE" <Group Address>"MUX_NEW_LINE" "MUX_NEW_LINE,
		callback: cmnCmdLwipIgmp,
	},
	{
		name	: MUX_CMD_PARAMS,
		helpString: MUX_NEW_LINE MUX_CMD_PARAMS" :"MUX_NEW_LINE" Display all configuration and running options"MUX_NEW_LINE,
		callback: cmnCmdParams,
	},

	{
		name	: MUX_CMD_NET_INFO,
		helpString: MUX_NEW_LINE MUX_CMD_NET_INFO" 1|0(DHCP) IP MASK GATEWAY(when DHCP=0) :"MUX_NEW_LINE" Display/Set network information"MUX_NEW_LINE,
		callback: cmnCmdNetInfo,
	},

	{
		name	: MUX_CMD_MAC_INFO,
		helpString: MUX_NEW_LINE MUX_CMD_MAC_INFO" xx:xx:xx:xx:xx:xx :"MUX_NEW_LINE" Display/Set MAC address"MUX_NEW_LINE,
		callback: cmnCmdMacInfo,
	},

	{
		name	: MUX_CMD_LOCAL_INFO,
		helpString: MUX_NEW_LINE MUX_CMD_LOCAL_INFO" xx:xx:xx:xx:xx:xx xxx.xxx.xxx.xxx vPort aPort:"MUX_NEW_LINE" Display/Set configuration of Local on TX/RX"MUX_NEW_LINE,
		callback: cmnCmdLocalInfo,
	},

	{
		name	: MUX_CMD_DEST_INFO,
		helpString: MUX_NEW_LINE MUX_CMD_DEST_INFO" xx:xx:xx:xx:xx:xx xxx.xxx.xxx.xxx vPort aPort:"MUX_NEW_LINE" Display/Set configuration of destination on TX"MUX_NEW_LINE,
		callback: cmnCmdDestInfo,
	},


	{
		name	: MUX_CMD_DEBUG,
		helpString: MUX_NEW_LINE MUX_CMD_DEBUG " "MUX_CMD_DEBUG_ENABLE"|"MUX_CMD_DEBUG_DISABLE" "MUX_NEW_LINE" Enable or Disable debug output"MUX_NEW_LINE,
		callback: cmdCmdDebuggable,
	},

	{
		name	: MUX_CMD_TX,
		helpString: MUX_NEW_LINE MUX_CMD_TX " 1|0 "MUX_NEW_LINE" Set as TX or RX mode"MUX_NEW_LINE,
		callback: cmnCmdTx,
	},

	{
		name	: MUX_CMD_UPDATE,
		helpString: CMD_HELP_UPDATE,
		callback: cmnCmdUpdate,
	},
	
	{
		name	: MUX_CMD_VERSION,
		helpString: CMD_HELP_VERSION,
		callback: cmnCmdVersion,
	},
	{
		name	: MUX_CMD_DEFAULT,
		helpString: CMD_HELP_HELP,
		callback: cmnCmdHelp,
	},

	{
		name	: NULL,
		helpString: NULL,
		callback: NULL,
	}

};

char bspCfgSave( MUX_RUNTIME_CFG *cfg, MUX_CFG_TYPE cfgType )
{
	return EXIT_SUCCESS;
}

const char *versionString = MUX_OS_NAME;


