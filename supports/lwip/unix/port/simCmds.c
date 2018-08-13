
#include "extSysParams.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lwipExt.h"


_CODE EXT_CLI_CMD_T extCmds[] =
{
	{
		name	: EXT_CMD_STATS,
		helpString: EXT_NEW_LINE EXT_CMD_STATS":"EXT_NEW_LINE" Displays statistics of LwIP"EXT_NEW_LINE,
		callback: cmnCmdLwipStats,
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
		name	: EXT_CMD_NET_INFO,
		helpString: EXT_NEW_LINE EXT_CMD_NET_INFO" 1|0(DHCP) IP MASK GATEWAY(when DHCP=0) :"EXT_NEW_LINE" Display/Set network information"EXT_NEW_LINE,
		callback: cmnCmdNetInfo,
	},

	{
		name	: EXT_CMD_MAC_INFO,
		helpString: EXT_NEW_LINE EXT_CMD_MAC_INFO" xx:xx:xx:xx:xx:xx :"EXT_NEW_LINE" Display/Set MAC address"EXT_NEW_LINE,
		callback: cmnCmdMacInfo,
	},

	{
		name	: EXT_CMD_LOCAL_INFO,
		helpString: EXT_NEW_LINE EXT_CMD_LOCAL_INFO" xx:xx:xx:xx:xx:xx xxx.xxx.xxx.xxx vPort aPort:"EXT_NEW_LINE" Display/Set configuration of Local on TX/RX"EXT_NEW_LINE,
		callback: cmnCmdLocalInfo,
	},

	{
		name	: EXT_CMD_DEST_INFO,
		helpString: EXT_NEW_LINE EXT_CMD_DEST_INFO" xx:xx:xx:xx:xx:xx xxx.xxx.xxx.xxx vPort aPort:"EXT_NEW_LINE" Display/Set configuration of destination on TX"EXT_NEW_LINE,
		callback: cmnCmdDestInfo,
	},


	{
		name	: EXT_CMD_DEBUG,
		helpString: EXT_NEW_LINE EXT_CMD_DEBUG " "EXT_CMD_DEBUG_ENABLE"|"EXT_CMD_DEBUG_DISABLE" "EXT_NEW_LINE" Enable or Disable debug output"EXT_NEW_LINE,
		callback: cmdCmdDebuggable,
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

char bspCfgSave( EXT_RUNTIME_CFG *cfg, EXT_CFG_TYPE cfgType )
{
	return EXIT_SUCCESS;
}

const char *versionString = EXT_OS_NAME;


