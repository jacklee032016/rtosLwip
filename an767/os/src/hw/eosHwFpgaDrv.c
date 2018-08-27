
#include "eos.h"
#include "gpio.h"

#include "eosFpga.h"

#include "lwipExt.h"

#define	_PORT_BYTE_ORDER_CHANGE		0

//#define _FPGA_MUTEX_

#ifdef _FPGA_MUTEX_
volatile xSemaphoreHandle FPGA_Mutex = NULL;

#define LOCK_FPGA()		xSemaphoreTake(FPGA_Mutex, portMAX_DELAY);
#define UNLOCK_FPGA()	xSemaphoreGive( FPGA_Mutex )

#else

//#error "This implementation require Mutex"
#define  	LOCK_FPGA()
#define  	UNLOCK_FPGA()

#endif

static char _fpgaVersion[128];


//#define  FPGA_GET_DONE_SIGNAL()		(gpio_pin_is_high(EXT_PIN_FPGA_PROGRAM ) )
#define  FPGA_GET_DONE_SIGNAL()		(gpio_pin_is_high(EXT_PIN_FPGA_DONE ) )


/* Wait for FPGA to load firmware from SPI memory */
static char  _extBspFpgaWaitDone(unsigned int  seconds)
{
//	char data;
	uint32_t	timeout;
	char  done = EXT_FALSE;
//	wdt_opt_t watchdog_opt;
	
//	watchdog_opt.us_timeout_period = 4000000;	// timeout after 2 second
	LOCK_FPGA();
	
	timeout = seconds;

	while (1)
	{
//		wdt_enable(&watchdog_opt);
		done = FPGA_GET_DONE_SIGNAL();
		if (done== EXT_FALSE)
		{
			printf("L");
		}
		else
		{
			printf("H");
			break;
		}
		
		if (timeout == 0)
		{
			printf("Timeout in waiting FPGA Done"EXT_NEW_LINE);
			break;
		}

		timeout--;
		EXT_DELAY_MS(1000);
	}
	
	UNLOCK_FPGA();
	
	return done;
}

static void _extFpgaRegisterRead(unsigned char address, unsigned char *data, unsigned char size)
{
	unsigned char i;
	unsigned char *index = data;
	
	for(i=0; i< size; i++)
	{
		FPGA_I2C_READ(address+i, index+i, 1);
	}
}

#define	_extFpgaReadByte(address, value)	\
	FPGA_I2C_READ((address), (value), 1)


#define	_extFpgaReadShortChanging(address, shortVal) \
{	_extFpgaRegisterRead((address), (shortVal), 2); \
	 *((unsigned short *)shortVal) = lwip_ntohs(*((unsigned short *)shortVal)); }


#define	_extFpgaReadShort(address, shortVal) \
{	_extFpgaRegisterRead((address), (shortVal), 2); }


#define	_extFpgaReadInteger(address, intVal) \
{	_extFpgaRegisterRead((address), intVal, 4); *((unsigned int *)intVal) = lwip_ntohl(*((unsigned int *)intVal)); }


/* IPAddress from Lwip, in network byte order */
static void	_extFpgaRegisterWrite(unsigned char baseAddr, unsigned char *data, unsigned char size)
{
	unsigned char i;
	unsigned char *index = data;

	for(i=0; i < size; i++)
	{
		FPGA_I2C_WRITE(baseAddr+i, index+i, 1);
	}

//		FPGA_I2C_READ(i, &data, 1);

	return ;//EXIT_SUCCESS;
}

#define	_extFpgaWriteByte(address, value)	\
	FPGA_I2C_WRITE((address), (value), 1)


#define	_extFpgaWriteShortChanging(address, shortVal) \
{	unsigned short value = lwip_ntohs(*((unsigned short *)(shortVal))); _extFpgaRegisterWrite((address), &value, 2); }


#define	_extFpgaWriteShort(address, shortVal) \
{	_extFpgaRegisterWrite((address), shortVal, 2); }


#define	_extFpgaWriteInteger(address, intVal) \
{	unsigned int value = lwip_ntohl(*((unsigned int *)(intVal))); _extFpgaRegisterWrite((address), &value, 4); }



static void _changeByteOrderOfMac(EXT_MAC_ADDRESS *mac, unsigned char *address)
{
	int i;
	for(i=0; i<EXT_MAC_ADDRESS_LENGTH; i++ )
	{
		address[i] = mac->address[EXT_MAC_ADDRESS_LENGTH-i-1];
	}
}


unsigned int extFgpaRegisterDebug( char *data, unsigned int size)
{
	int index = 0;
	unsigned char val;
	unsigned short port = 0;
	unsigned int	intValue;
	EXT_MAC_ADDRESS destMac;
	unsigned char		address[EXT_MAC_ADDRESS_LENGTH];
	
	MuxRunTimeParam *mediaParams = &extRun.runtime;
	extFpgaReadParams(mediaParams);

	index += snprintf(data+index, size-index, "%s;"EXT_NEW_LINE, extFgpaReadVersion() );
	
	index += snprintf(data+index, size-index, "%s configuration: ", EXT_IS_TX(&extRun)?"TX":"RX");
	_extFpgaReadInteger(EXT_FPGA_REG_IP, (unsigned char *)&intValue);
	index += snprintf(data+index, size-index, EXT_NEW_LINE"Local :"EXT_NEW_LINE"\tIP\t: %s", inet_ntoa(intValue));

	index += snprintf(data+index, size-index, EXT_NEW_LINE"\tMAC\t:" );
	_extFpgaRegisterRead(EXT_FPGA_REG_MAC,  (unsigned char *)&destMac, EXT_MAC_ADDRESS_LENGTH);
	_changeByteOrderOfMac(&destMac, address);
	index += snprintf(data+index, size-index, " %02x:%02x:%02x:%02x:%02x:%02x:", address[0], address[1], address[2], address[3], address[4], address[5]);

	index += snprintf(data+index, size-index, EXT_NEW_LINE"\tPort\t: Video:" );
	_extFpgaReadShort(EXT_FPGA_REG_PORT_VIDEO,  (unsigned char *)&port);
	index += snprintf(data+index, size-index, "%d;", port);

	index += snprintf(data+index, size-index," Audio:" );
	_extFpgaReadShort(EXT_FPGA_REG_PORT_AUDIO,  (unsigned char *)&port);
	index += snprintf(data+index, size-index, "%d;", port);

	index += snprintf(data+index, size-index, " DT:" );
	_extFpgaReadShort(EXT_FPGA_REG_PORT_ANC_DT,  (unsigned char *)&port);
	index += snprintf(data+index, size-index, "%d;", port);

	index += snprintf(data+index, size-index, " ST:" );
	_extFpgaReadShort(EXT_FPGA_REG_PORT_ANC_ST,  (unsigned char *)&port);
	index += snprintf(data+index, size-index, "%d;"EXT_NEW_LINE, port);

	if(EXT_IS_TX(&extRun) )
	{
		/* TX only */
		_extFpgaReadInteger(EXT_FPGA_REG_DEST_IP, (unsigned char *)&intValue);
		index += snprintf(data+index, size-index, EXT_NEW_LINE"Dest :"EXT_NEW_LINE"\tIP\t: %s", inet_ntoa(intValue));

		index += snprintf(data+index, size-index, EXT_NEW_LINE"\tMAC\t:" );
		_extFpgaRegisterRead(EXT_FPGA_REG_DEST_MAC, (unsigned char *)&destMac, EXT_MAC_ADDRESS_LENGTH);
		_changeByteOrderOfMac(&destMac, address);
		index += snprintf(data+index, size-index, " %02x:%02x:%02x:%02x:%02x:%02x:", address[0], address[1], address[2], address[3], address[4], address[5]);

		index += snprintf(data+index, size-index, EXT_NEW_LINE"\tPort\t: Video:" );
		_extFpgaReadShort(EXT_FPGA_REG_DEST_PORT_VIDEO,  (unsigned char *)&port);
		index += snprintf(data+index, size-index, "%d;", port);


		index += snprintf(data+index, size-index," Audio:" );
		_extFpgaReadShort(EXT_FPGA_REG_DEST_PORT_AUDIO,  (unsigned char *)&port);
		index += snprintf(data+index, size-index, "%d;", port);

		index += snprintf(data+index, size-index," DT:" );
		_extFpgaReadShort(EXT_FPGA_REG_DEST_PORT_ANC_DT,  (unsigned char *)&port);
		index += snprintf(data+index, size-index, "%d;", port);

		index += snprintf(data+index, size-index," ST:" );
		_extFpgaReadShort(EXT_FPGA_REG_DEST_PORT_ANC_ST,  (unsigned char *)&port);
		index += snprintf(data+index, size-index, "%d;"EXT_NEW_LINE, port);

	}

	index += snprintf(data+index, size-index, EXT_NEW_LINE"Video :"EXT_NEW_LINE"\tWxH:%hux%hu; ", mediaParams->vWidth, mediaParams->vHeight);
	index += snprintf(data+index, size-index, "FPS:%d; ", mediaParams->vFrameRate);
	index += snprintf(data+index, size-index, "Depth:%d; ", mediaParams->vDepth);
	index += snprintf(data+index, size-index, "ColorSpace:%s; ", CMN_FIND_V_COLORSPACE(mediaParams->vColorSpace));
	index += snprintf(data+index, size-index, "%s;"EXT_NEW_LINE, (mediaParams->vIsSegmented)?"Segmented":"Interlaced");
	
	return index;
}


char	extFpgaConfig(EXT_RUNTIME_CFG *runCfg)
{
	EXT_MAC_ADDRESS destMac, *mac;
	char ret;
	unsigned char		address[EXT_MAC_ADDRESS_LENGTH];
	EXT_VIDEO_CONFIG *vCfg;


	/*configure local address/port, for both RX/TX*/
	
//	_extFpgaRegisterWrite(EXT_FPGA_REG_PORT_AUDIO, (unsigned char *)&runCfg->local.aport, 2);

	/*configure dest, only for TX */
	if(EXT_IS_TX(runCfg) )
	{
		/* local */
		_changeByteOrderOfMac(&runCfg->local.mac, address);
		_extFpgaRegisterWrite(EXT_FPGA_REG_MAC, address, EXT_MAC_ADDRESS_LENGTH);

		_extFpgaWriteInteger(EXT_FPGA_REG_IP, (unsigned char *)&runCfg->local.ip);

		_extFpgaWriteShort(EXT_FPGA_REG_PORT_VIDEO, (unsigned char *)&runCfg->local.vport);
		_extFpgaWriteShort(EXT_FPGA_REG_PORT_AUDIO, (unsigned char *)&runCfg->local.aport);
		_extFpgaWriteShort(EXT_FPGA_REG_PORT_ANC_DT, (unsigned char *)&runCfg->local.dport);
		_extFpgaWriteShort(EXT_FPGA_REG_PORT_ANC_ST, (unsigned char *)&runCfg->local.sport);

		/* dest */
		ret = extNetMulticastIP4Mac(&runCfg->dest.ip, &destMac);
		if(ret == EXIT_SUCCESS)
		{/* dest MAC is multicast MAC address */
			mac = &destMac;
		}
		else
		{
			mac = &runCfg->dest.mac;
		}
		_changeByteOrderOfMac(mac, address);
		_extFpgaRegisterWrite(EXT_FPGA_REG_DEST_MAC, address, EXT_MAC_ADDRESS_LENGTH);

		_extFpgaWriteInteger(EXT_FPGA_REG_DEST_IP, (unsigned char *)&runCfg->dest.ip);

		_extFpgaWriteShort(EXT_FPGA_REG_DEST_PORT_VIDEO, (unsigned char *)&runCfg->dest.vport);
		_extFpgaWriteShort(EXT_FPGA_REG_DEST_PORT_AUDIO, (unsigned char *)&runCfg->dest.aport);
		_extFpgaWriteShort(EXT_FPGA_REG_DEST_PORT_ANC_DT, (unsigned char *)&runCfg->dest.dport);
		_extFpgaWriteShort(EXT_FPGA_REG_DEST_PORT_ANC_ST, (unsigned char *)&runCfg->dest.sport);
		
	//	extFpgaEnable(1);
	}
	else
	{/* RX */
//		const ip4_addr_t *mcIpAddr;
		vCfg = &runCfg->dest;

		_changeByteOrderOfMac(&vCfg->mac, address);
		_extFpgaRegisterWrite(EXT_FPGA_REG_MAC, address, EXT_MAC_ADDRESS_LENGTH);

		_extFpgaWriteInteger(EXT_FPGA_REG_IP, (unsigned char *)&vCfg->ip);

		_extFpgaWriteShort(EXT_FPGA_REG_PORT_VIDEO, (unsigned char *)&vCfg->vport);
		_extFpgaWriteShort(EXT_FPGA_REG_PORT_AUDIO, (unsigned char *)&vCfg->aport);
		_extFpgaWriteShort(EXT_FPGA_REG_PORT_ANC_DT, (unsigned char *)&vCfg->dport);
		_extFpgaWriteShort(EXT_FPGA_REG_PORT_ANC_ST, (unsigned char *)&vCfg->sport);

//		printf("FPGA Configuration ended!"LWIP_NEW_LINE);

//		EXT_DELAY_MS(5000);
	
		ip4_addr_t *mcIpAddr = (ip4_addr_t *)&vCfg->ip;
		if( ip4_addr_ismulticast(mcIpAddr) )
		{
//			printf("Send IGMP JOIN"LWIP_NEW_LINE);
			ret = EXT_NET_IGMP_JOIN(runCfg->dest.ip);
		}


	}

	return EXIT_SUCCESS;
}

#define	_extFpgaReadVer( mediaParams) \
{	_extFpgaReadByte(EXT_FPGA_REG_VERSION, &(mediaParams)->version); \
	_extFpgaReadByte(EXT_FPGA_REG_REVISION, &(mediaParams)->revision); \
	_extFpgaReadByte(EXT_FPGA_REG_MONTH, &(mediaParams)->month); \
	_extFpgaReadByte(EXT_FPGA_REG_DAY, &(mediaParams)->day);	\
	_extFpgaReadByte(EXT_FPGA_REG_YEAR, &(mediaParams)->year);		\
	_extFpgaReadByte(EXT_FPGA_REG_HOUR, &(mediaParams)->hour);	\
	_extFpgaReadByte(EXT_FPGA_REG_MINUTE, &(mediaParams)->minute); \
}


char *extFgpaReadVersion(void)
{
	int index = 0;
	MuxRunTimeParam *mediaParams = &extRun.runtime;
	
	_extFpgaReadVer(mediaParams);

	index += snprintf(_fpgaVersion+index, sizeof(_fpgaVersion) - index, " %s, Ver:%02x", 
		((mediaParams->version&EXT_FPGA_TX_FLAGS)==0)?"RX":"TX", (mediaParams->version&(~EXT_FPGA_TX_FLAGS)) );
	index += snprintf(_fpgaVersion+index, sizeof(_fpgaVersion) - index, ".%02x, ", mediaParams->revision);

	index += snprintf(_fpgaVersion+index, sizeof(_fpgaVersion) - index, "(Build %02x ", mediaParams->month);
	index += snprintf(_fpgaVersion+index, sizeof(_fpgaVersion) - index, "%02x ", mediaParams->day);
	index += snprintf(_fpgaVersion+index, sizeof(_fpgaVersion) - index, "%02x ", mediaParams->year);
	index += snprintf(_fpgaVersion+index, sizeof(_fpgaVersion) - index, "%02x:", mediaParams->hour);
	index += snprintf(_fpgaVersion+index, sizeof(_fpgaVersion) - index, "%02x)", mediaParams->minute);

	return _fpgaVersion;
}

typedef	struct _fpgaIntTable
{
	unsigned char		key;
	unsigned char		value;
}FpgaIntTable;


static const FpgaIntTable _frameRateTable[]=
{
	{
		2,
		EXT_V_FRAMERATE_T_23	
	},
	{
		3,
		EXT_V_FRAMERATE_T_24	
	},
	{
		5,
		EXT_V_FRAMERATE_T_25	
	},
	{
		6,
		EXT_V_FRAMERATE_T_29	
	},
	{
		7,
		EXT_V_FRAMERATE_T_30	
	},
	{
		9,
		EXT_V_FRAMERATE_T_50	
	},
	{
		0x0a,
		EXT_V_FRAMERATE_T_59	
	},
	{
		0x0b,
		EXT_V_FRAMERATE_T_60	
	}
};

static const FpgaIntTable _vDepthTable[]=
{
	{
		0,
		EXT_V_DEPTH_8	
	},
	{
		1,
		EXT_V_DEPTH_10	
	},
	{
		2,
		EXT_V_DEPTH_12	
	},
	{
		3,
		EXT_V_DEPTH_16	
	}
};

static unsigned char _translateFrameRate(unsigned char rate, unsigned char isFromFpga)
{
	unsigned int i;
	for(i=0; i< EXT_ARRAYSIZE(_frameRateTable); i++)
	{
		if(isFromFpga )
		{
			if(_frameRateTable[i].key == rate )
				return _frameRateTable[i].value;
		}
		else
		{
			if(_frameRateTable[i].value == rate )
				return _frameRateTable[i].key;
		}
	}

	EXT_ERRORF(("FrameRate %d is invalidate value for %s", rate, (isFromFpga)?"MCU":"FPGA") );
	
	if(isFromFpga)
		return EXT_V_DEPTH_8;
	return 0; /* depth 0 for FPGA */
}

static unsigned char _translateDepth(unsigned char depth, unsigned char isFromFpga)
{
	unsigned int i;
	for(i=0; i< EXT_ARRAYSIZE(_vDepthTable); i++)
	{
		if(isFromFpga )
		{
			if(_vDepthTable[i].key == depth )
				return _vDepthTable[i].value;
		}
		else
		{
			if(_vDepthTable[i].value == depth )
				return _vDepthTable[i].key;
		}
	}

	EXT_ERRORF(("vDepth %d is invalidate value for %s", depth, (isFromFpga)?"MCU":"FPGA") );
	
	if(isFromFpga)
		return EXT_V_FRAMERATE_T_23;
	return 2; /* 23.9 for FPGA */
}


char extFpgaReadParams(MuxRunTimeParam *mediaParams)
{
	_extFpgaReadShortChanging(EXT_FPGA_REG_WIDTH, (unsigned char *)&mediaParams->vWidth);
	
	_extFpgaReadShortChanging(EXT_FPGA_REG_HEIGHT, (unsigned char *)&mediaParams->vHeight);
	
	_extFpgaReadByte(EXT_FPGA_REG_FRAMERATE, &mediaParams->vFrameRate);
	mediaParams->vFrameRate = _translateFrameRate(mediaParams->vFrameRate, EXT_TRUE);

	_extFpgaReadByte(EXT_FPGA_REG_SAMPLING, &mediaParams->vColorSpace);

	_extFpgaReadByte(EXT_FPGA_REG_DEPTH, &mediaParams->vDepth);
	mediaParams->vDepth = _translateDepth(mediaParams->vDepth, EXT_TRUE);

	_extFpgaReadByte(EXT_FPGA_REG_INTLC_SEGM, &mediaParams->vIsSegmented);

	return EXIT_SUCCESS;
}

char extFpgaWriteParams(MuxRunTimeParam *mediaParams)
{
	unsigned char value;
	_extFpgaWriteShortChanging(EXT_FPGA_REG_WIDTH, (unsigned char *)&mediaParams->vWidth);
	
	_extFpgaWriteShortChanging(EXT_FPGA_REG_HEIGHT, (unsigned char *)&mediaParams->vHeight);
	
	value = _translateFrameRate(mediaParams->vFrameRate, EXT_FALSE);
	_extFpgaWriteByte(EXT_FPGA_REG_FRAMERATE, &value);

	_extFpgaWriteByte(EXT_FPGA_REG_SAMPLING, &mediaParams->vColorSpace);

	value = _translateDepth(mediaParams->vDepth, EXT_FALSE);
	_extFpgaReadByte(EXT_FPGA_REG_DEPTH, &value);

	_extFpgaWriteByte(EXT_FPGA_REG_INTLC_SEGM, &mediaParams->vIsSegmented);

	return EXIT_SUCCESS;
}

void	extFpgaEnable(char	isEnable)
{
	unsigned char		data = 0x01;
	if(isEnable == 0)
	{
		data = 0;
	}

	_extFpgaRegisterWrite(EXT_FPGA_REG_ENABLE, &data, 1);
}

void	extFpgaBlinkPowerLED(char	isEnable)
{
	unsigned char		data = 0x01;
	if(isEnable == 0)
	{
		data = 0;
	}

	_extFpgaWriteByte(EXT_FPGA_REG_POWER_LED, &data);
}



/* Reload FPGA from SPI memory */
char  extBspFpgaReload(void)
{
#define	_TIMEOUT_VALUE		20
	char		isOK = EXT_FALSE;
	unsigned char data;
	int timeoutSecond = _TIMEOUT_VALUE;	/* when compare with negative number, it must be int, not char */

	bspHwClockInit();

	 /* active-LOW reset to configure FPGA */
//	gpio_set_pin_high(EXT_PIN_FPGA_PROGRAM);
//	EXT_DELAY_MS(50);
	if(extRun.isConfigFpga )
	{
		EXT_INFOF(("Reconfiguring FPGA....."EXT_NEW_LINE));
		gpio_set_pin_low(EXT_PIN_FPGA_PROGRAM);
		
#if 1	
		EXT_DELAY_MS(10);
#else
		isOK = _extBspFpgaWaitDone(20);
		EXT_DELAY_MS(5000);
#endif	
	}
	else
	{
		EXT_INFOF(("Test version: FPGA is not loaded by MCU"EXT_NEW_LINE));
	}


	gpio_set_pin_high(EXT_PIN_FPGA_PROGRAM);
	
//	isOK = _extBspFpgaWaitDone(20);
	EXT_DELAY_MS(50);

	/* wait for FPGA done pin
	* up to 20 second is the secondary image is loaded 
	*/
	isOK = _extBspFpgaWaitDone(20);
	
	printf("FPGA load image %s"EXT_NEW_LINE, (isOK== EXT_FALSE)?"failed":"sucessed");

	printf("Waiting FPGA ....."EXT_NEW_LINE);
	do
	{
		isOK = FPGA_I2C_READ(EXT_FPGA_REG_ETHERNET_RESET, &data, 1);
		timeoutSecond--;
		
//		EXT_ERRORF(("Timeout %d seconds when waiting FPGA status10"EXT_NEW_LINE, timeoutSecond));
		if(timeoutSecond<0)
		{
//			EXT_ERRORF(("Timeout %d seconds when waiting FPGA status0"EXT_NEW_LINE, _TIMEOUT_VALUE));
			break;
		}
		EXT_DELAY_US(1000*100);
	}while(isOK!= EXIT_SUCCESS);

	if(isOK == EXIT_SUCCESS)
	{
		printf("FPGA Ethernet Reset register %d : %2x"EXT_NEW_LINE, EXT_FPGA_REG_ETHERNET_RESET, data);
		data = 0x01;
	
		_extFpgaWriteByte(EXT_FPGA_REG_ETHERNET_RESET, &data);

#if 0
	/* test I2C when LMH1983 is not working. J.L. April 10, 2018 */
	bspBistI2cSensor(extBspCLIGetOutputBuffer(), EXT_COMMAND_BUFFER_SIZE);
	bspBistI2cEeprom(extBspCLIGetOutputBuffer(), EXT_COMMAND_BUFFER_SIZE);
#endif
	}
	else
	{
		EXT_ERRORF(("Timeout %d seconds when waiting FPGA status"EXT_NEW_LINE, _TIMEOUT_VALUE));
	}
	
	return isOK;
}

#if 0
void extFpgaInit(void)
{
	EXT_FPGA_VER ver;
//	int timeout = 20;
	int i;
	unsigned char data;
	unsigned char val;
	
//	bspHwClockInit();
	for(i=0; i< 8; i++)
	{
		val =(unsigned char)i;
		_extFpgaWriteByte(i, &val);
		_extFpgaReadByte(i, &data);
		printf("FPGA register %d : %s (%d)"EXT_NEW_LINE, i, (val==data)?"OK":"Failed", data);
	}

	for(i=8; i< 16; i++)
	{
		_extFpgaReadByte(i, &data, 1);
		printf("FPGA register %d : 0x%2x"EXT_NEW_LINE, i, data);
	}
	
#if 0	
	while(FPGA_I2C_READ(FPGA_VERSION, (unsigned char *)&ver, 3 ) != EXIT_SUCCESS)
	{
//		wdt_enable(&watchdog_opt);
		EXT_DELAY_MS(1000);
		timeout--;
		if (timeout == 0)
		{
			EXT_ERRORF(("FPGA read version timeout"));
			break;	// too long , exit loop
		}
	}
#endif

	printf("FPGA %s, version:%d, revision:%d, device:%d"EXT_NEW_LINE, (ver.rx_tx== EXT_FALSE)?"RX":"TX", ver.version, ver.revision, ver.fpgaDevice);

}
#endif


