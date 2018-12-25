
#include "ext.h"

#include <string.h>

#if TASK_CONSOLE_EN

static char	_cmdBoot(const struct _EXT_CLI_CMD *cmd, char *outBuffer, size_t bufferLen )
{
	strncpy( outBuffer, "startup OS.....", bufferLen );

	EXT_LOAD_BOOLOADER();
	return EXT_FALSE;
}


_CODE  EXT_CLI_CMD_T extCmds[] =
{

	{
		name	: EXT_CMD_BOOT,
		helpString: EXT_NEW_LINE EXT_CMD_BOOT":"EXT_NEW_LINE" Boot RTOS"EXT_NEW_LINE,
		callback: _cmdBoot,
	},
	
	{
		name	: EXT_CMD_STATS,
		helpString: EXT_NEW_LINE EXT_CMD_STATS":"EXT_NEW_LINE" Displays statistics of LwIP"EXT_NEW_LINE,
		callback: cmnCmdNetStats,
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
#endif

