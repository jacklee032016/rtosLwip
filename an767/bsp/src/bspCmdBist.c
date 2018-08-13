
#include "compact.h"

typedef struct _EXT_BIST_CMD
{
	const char * const			name;
	const EXT_BIST_HANDLER	handle;
} EXT_BIST_CMD_T;


EXT_BIST_CMD_T bistHandlers[] =
{
	{
		name	: "SPI Flash ID",
		handle: bspBistSpiFlashReadDeviceID,
	},

	{
		name	: "SPI Flash Read/Write",
		handle: bspBistSpiFlashReadWrite,
	},

	{
		name	: "LM95245 Sensor",
		handle: bspBistI2cSensor,
	},
	{
		name	: "EEPROM AT24C04BN",
		handle: bspBistI2cEeprom,
	},
	{
		name	: "Clock Generator",
		handle: bspBistClock,
	},
	{
		name	: "DIP Switch",
		handle: bspBistDipSwitch,
	},
	{
		name	: NULL,
		handle: NULL,
	}

};


/*built-in self-test (BIST)*/
char bspCmdBIST(const struct _EXT_CLI_CMD *cmd, char *outBuffer, size_t bufferLen )
{
	const EXT_BIST_CMD_T *_bist = bistHandlers;
	int i = 0;
	int	index = 0;
	static int cmdIndex = 0;
//	char ret;

	outBuffer[ 0 ] = 0x00;
//	printf("BIST Built-In Self-Test..."EXT_NEW_LINE );

	if(cmdIndex == 0)
	{
		index += snprintf( outBuffer+index, (bufferLen-index), "BIST Built-In Self-Test..."EXT_NEW_LINE);
	}
	
	/* Note:  This function is not re-entrant.  It must not be called from more thank one task. */
	while( _bist->handle )
	{
//		printf("No. %d Test: %s ...:"EXT_NEW_LINE, i, _bist->name);

		if(i== cmdIndex && _bist->handle )
		{
//			printf("test No.%d: %s..."EXT_NEW_LINE, i, _bist->name );
			index += snprintf( outBuffer+index, (bufferLen-index), "\tNo.%d Test: %s ...:"EXT_NEW_LINE, i, _bist->name );
			_bist->handle( outBuffer+index, bufferLen-index);
//			printf("\t\t %s"EXT_NEW_LINE EXT_NEW_LINE, (ret==EXIT_SUCCESS)?"OK":"Failed");
//			index += snprintf( outBuffer+index, (bufferLen-index), "\t\t %s"EXT_NEW_LINE EXT_NEW_LINE, (ret==EXIT_SUCCESS)?"OK":"Failed" );

			_bist++;
			if( _bist->handle)
			{
				cmdIndex++;
				return EXT_TRUE;
			}

			cmdIndex = 0;
			break;
		}

		i++;
		_bist++;
//		printf("prepare No.%d(%d): %s..."EXT_NEW_LINE, i, cmdIndex, _bist->name );
		if(!_bist->handle)
			break;
	}

	cmdIndex = 0;
	return EXT_FALSE;
}

