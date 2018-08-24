
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
	
		FPGA_I2C_WRITE(EXT_FPGA_REG_ETHERNET_RESET, &data, 1);

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
		FPGA_I2C_WRITE(i, &val, 1);
		FPGA_I2C_READ(i, &data, 1);
		printf("FPGA register %d : %s (%d)"EXT_NEW_LINE, i, (val==data)?"OK":"Failed", data);
	}

	for(i=8; i< 16; i++)
	{
		FPGA_I2C_READ(i, &data, 1);
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


/* IPAddress from Lwip, in network byte order */
char	extFpgaRegisterWrite(unsigned char baseAddr, unsigned char *data, unsigned char size)
{
	unsigned char i;
	unsigned char *index = data;

	for(i=0; i < size; i++)
	{
		FPGA_I2C_WRITE(baseAddr+i, index+i, 1);
	}

//		FPGA_I2C_READ(i, &data, 1);

	return EXIT_SUCCESS;
}

void extFgpaRegisterDebug(void)
{
	int i;
	unsigned char val;
	unsigned short port = 0;

	printf("%s configuration: ", EXT_IS_TX(&extRun)?"TX":"RX");
	printf(EXT_NEW_LINE"\tLocal :"EXT_NEW_LINE"\t\tIP:\t" );
	for(i=EXT_FPGA_REG_IP; i< EXT_FPGA_REG_IP+4; i++)
	{
		FPGA_I2C_READ(i, &val, 1);
		printf(" %u:%d(0x%2x);", i, val, val);
	}

	printf(EXT_NEW_LINE"\t\tMAC:\t" );
	for(i=EXT_FPGA_REG_MAC; i< EXT_FPGA_REG_MAC+ EXT_MAC_ADDRESS_LENGTH; i++)
	{
		FPGA_I2C_READ(i, &val, 1);
		printf(" %u:0x%2x;", i, val);
	}

	printf(EXT_NEW_LINE"\t\tPort:"EXT_NEW_LINE"\t\t\tVideo:\t" );
	for(i=EXT_FPGA_REG_PORT_VIDEO; i< EXT_FPGA_REG_PORT_VIDEO+2; i++)
	{
		FPGA_I2C_READ(i, &val, 1);
		printf(" %u:0x%2x;", i, val);
		port = (port<<8)|val;	/* byte order changed now */
	}
#if _PORT_BYTE_ORDER_CHANGE	
	printf("(%d)"EXT_NEW_LINE port );
#else
	printf("(%d)"EXT_NEW_LINE, lwip_htons(port));
#endif

	printf("\t\t\tAudio:\t" );
	for(i=EXT_FPGA_REG_PORT_AUDIO; i< EXT_FPGA_REG_PORT_AUDIO+2; i++)
	{
		FPGA_I2C_READ(i, &val, 1);
		printf(" %u:0x%2x;", i, val);
		port = (port<<8)|val;	/* byte order changed now */
	}
#if _PORT_BYTE_ORDER_CHANGE	
	printf("(%d)"EXT_NEW_LINE port );
#else
	printf("(%d)"EXT_NEW_LINE, lwip_htons(port));
#endif

	printf("\t\t\tDT:\t" );
	for(i=EXT_FPGA_REG_PORT_ANC_DT; i< EXT_FPGA_REG_PORT_ANC_DT+2; i++)
	{
		FPGA_I2C_READ(i, &val, 1);
		printf(" %u:0x%2x;", i, val);
		port = (port<<8)|val;	/* byte order changed now */
	}
#if _PORT_BYTE_ORDER_CHANGE	
	printf("(%d)"EXT_NEW_LINE port );
#else
	printf("(%d)"EXT_NEW_LINE, lwip_htons(port));
#endif

	printf("\t\t\tST:\t" );
	for(i=EXT_FPGA_REG_PORT_ANC_ST; i< EXT_FPGA_REG_PORT_ANC_ST+2; i++)
	{
		FPGA_I2C_READ(i, &val, 1);
		printf(" %u:0x%2x;", i, val);
		port = (port<<8)|val;/* byte order changed now */
	}
#if _PORT_BYTE_ORDER_CHANGE	
	printf("(%d)"EXT_NEW_LINE port );
#else
	printf("(%d)"EXT_NEW_LINE, lwip_htons(port));
#endif


	if(EXT_IS_TX(&extRun) )
	{
		/* TX only */
#if _PORT_BYTE_ORDER_CHANGE		
		printf("\tDest"EXT_NEW_LINE"\t\tIP:\t" );
#else
		printf("\tDest"EXT_NEW_LINE"\t\tIP:\t" );
#endif
		for(i=EXT_FPGA_REG_DEST_IP; i< EXT_FPGA_REG_DEST_IP+4; i++)
		{
			FPGA_I2C_READ(i, &val, 1);
			printf(" %u:%d(0x%2x);", i, val, val);
		}

		printf(EXT_NEW_LINE"\t\tMAC:\t" );
		for(i=EXT_FPGA_REG_DEST_MAC; i< EXT_FPGA_REG_DEST_MAC+ EXT_MAC_ADDRESS_LENGTH; i++)
		{
			FPGA_I2C_READ(i, &val, 1);
			printf(" %u:0x%2x;", i, val);
		}

		printf(EXT_NEW_LINE"\t\tPort:"EXT_NEW_LINE"\t\t\tVideo:\t" );
		port = 0;
		for(i=EXT_FPGA_REG_DEST_PORT_VIDEO; i< EXT_FPGA_REG_DEST_PORT_VIDEO+2; i++)
		{
			FPGA_I2C_READ(i, &val, 1);
			printf(" %u:0x%2x;", i, val);
			port = (port<<8)|val;
		}
#if _PORT_BYTE_ORDER_CHANGE	
		printf("(%d)"EXT_NEW_LINE, port );
#else
		printf("(%d)"EXT_NEW_LINE, lwip_htons(port));
#endif

		printf("\t\t\tAudio:\t" );
		port = 0;
		for(i=EXT_FPGA_REG_DEST_PORT_AUDIO; i< EXT_FPGA_REG_DEST_PORT_AUDIO+2; i++)
		{
			FPGA_I2C_READ(i, &val, 1);
			printf(" %u:0x%2x;", i, val);
			port = (port<<8)|val;
		}
#if _PORT_BYTE_ORDER_CHANGE	
		printf("(%d)"EXT_NEW_LINE, port );
#else
		printf("(%d)"EXT_NEW_LINE, lwip_htons(port));
#endif

		printf("\t\t\tDT:\t" );
		port = 0;
		for(i=EXT_FPGA_REG_DEST_PORT_ANC_DT; i< EXT_FPGA_REG_DEST_PORT_ANC_DT+2; i++)
		{
			FPGA_I2C_READ(i, &val, 1);
			printf(" %u:0x%2x;", i, val);
			port = (port<<8)|val;
		}
#if _PORT_BYTE_ORDER_CHANGE	
		printf("(%d)"EXT_NEW_LINE, port );
#else
		printf("(%d)"EXT_NEW_LINE, lwip_htons(port));
#endif

		printf("\t\t\tST:\t" );
		port = 0;
		for(i=EXT_FPGA_REG_DEST_PORT_ANC_ST; i< EXT_FPGA_REG_DEST_PORT_ANC_ST+2; i++)
		{
			FPGA_I2C_READ(i, &val, 1);
			printf(" %u:0x%2x;", i, val);
			port = (port<<8)|val;
		}
#if _PORT_BYTE_ORDER_CHANGE	
		printf("(%d)"EXT_NEW_LINE, port );
#else
		printf("(%d)"EXT_NEW_LINE, lwip_htons(port));
#endif

	}
	
#if _PORT_BYTE_ORDER_CHANGE	
	printf("FPGA !!" EXT_NEW_LINE);
#else
	printf("FPGA !!" EXT_NEW_LINE);
#endif
}


static void _changeByteOrderOfMac(EXT_MAC_ADDRESS *mac, unsigned char *address)
{
	int i;
	for(i=0; i<EXT_MAC_ADDRESS_LENGTH; i++ )
	{
		address[i] = mac->address[EXT_MAC_ADDRESS_LENGTH-i-1];
	}
}

void	extFpgaEnable(char	isEnable)
{
	unsigned char		data = 0x01;
	if(isEnable == 0)
	{
		data = 0;
	}

	extFpgaRegisterWrite(EXT_FPGA_REG_ENABLE, &data, 1);
}

void	extFpgaBlinkPowerLED(char	isEnable)
{
	unsigned char		data = 0x01;
	if(isEnable == 0)
	{
		data = 0;
	}

	extFpgaRegisterWrite(EXT_FPGA_REG_POWER_LED, &data, 1);
}


char	extFpgaConfig(EXT_RUNTIME_CFG *runCfg)
{
	EXT_MAC_ADDRESS destMac, *mac;
	char ret;
	unsigned int ip;
	unsigned char		address[EXT_MAC_ADDRESS_LENGTH];
	unsigned short port = 0;
	EXT_VIDEO_CONFIG *vCfg;


	/*configure local address/port, for both RX/TX*/
	
//	extFpgaRegisterWrite(EXT_FPGA_REG_PORT_AUDIO, (unsigned char *)&runCfg->local.aport, 2);

	/*configure dest, only for TX */
	if(EXT_IS_TX(runCfg) )
	{
		/* local */
		_changeByteOrderOfMac(&runCfg->local.mac, address);
		ip = lwip_htonl(runCfg->local.ip);

		extFpgaRegisterWrite(EXT_FPGA_REG_MAC, address, EXT_MAC_ADDRESS_LENGTH);
		extFpgaRegisterWrite(EXT_FPGA_REG_IP, (unsigned char *)&ip, 4);

		port = runCfg->local.vport;
#if _PORT_BYTE_ORDER_CHANGE	
		port = lwip_htons( runCfg->local.vport);
#endif
		extFpgaRegisterWrite(EXT_FPGA_REG_PORT_VIDEO, (unsigned char *)&port, 2);

		port = runCfg->local.aport;
#if _PORT_BYTE_ORDER_CHANGE	
		port = lwip_htons( runCfg->local.aport);
#endif
		extFpgaRegisterWrite(EXT_FPGA_REG_PORT_AUDIO, (unsigned char *)&port, 2);

		port = runCfg->local.dport;
#if _PORT_BYTE_ORDER_CHANGE	
		port = lwip_htons( runCfg->local.dport);
#endif
		extFpgaRegisterWrite(EXT_FPGA_REG_PORT_ANC_DT, (unsigned char *)&port, 2);

		port = runCfg->local.sport;
#if _PORT_BYTE_ORDER_CHANGE	
		port = lwip_htons( runCfg->local.sport);
#endif
		extFpgaRegisterWrite(EXT_FPGA_REG_PORT_ANC_ST, (unsigned char *)&port, 2);

		/* dest */
		ip = lwip_htonl(runCfg->dest.ip);
		extFpgaRegisterWrite(EXT_FPGA_REG_DEST_IP, (unsigned char *)&ip, 4);

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

		extFpgaRegisterWrite(EXT_FPGA_REG_DEST_MAC, address, EXT_MAC_ADDRESS_LENGTH);

		port = runCfg->dest.vport;
#if _PORT_BYTE_ORDER_CHANGE	
		port = lwip_htons(runCfg->dest.vport);
#endif
		extFpgaRegisterWrite(EXT_FPGA_REG_DEST_PORT_VIDEO, (unsigned char *)&port, 2);

		port = runCfg->dest.aport;
#if _PORT_BYTE_ORDER_CHANGE	
		port = lwip_htons(runCfg->dest.aport);
#endif
		extFpgaRegisterWrite(EXT_FPGA_REG_DEST_PORT_AUDIO, (unsigned char *)&port, 2);

		port = runCfg->dest.dport;
#if _PORT_BYTE_ORDER_CHANGE	
		port = lwip_htons(runCfg->dest.dport);
#endif
		extFpgaRegisterWrite(EXT_FPGA_REG_DEST_PORT_ANC_DT, (unsigned char *)&port, 2);

		port = runCfg->dest.sport;
#if _PORT_BYTE_ORDER_CHANGE	
		port = lwip_htons(runCfg->dest.sport);
#endif
		extFpgaRegisterWrite(EXT_FPGA_REG_DEST_PORT_ANC_ST, (unsigned char *)&port, 2);

//		extFpgaRegisterWrite(EXT_FPGA_REG_DEST_PORT_AUDIO, (unsigned char *)&runCfg->audioPortDest, 2);
		
	//	extFpgaEnable(1);
	}
	else
	{/* RX */
//		const ip4_addr_t *mcIpAddr;

		vCfg = &runCfg->dest;

		_changeByteOrderOfMac(&vCfg->mac, address);
		ip = lwip_htonl(vCfg->ip);

		extFpgaRegisterWrite(EXT_FPGA_REG_MAC, address, EXT_MAC_ADDRESS_LENGTH);

		extFpgaRegisterWrite(EXT_FPGA_REG_IP, (unsigned char *)&ip, 4);

		port = vCfg->vport;
#if _PORT_BYTE_ORDER_CHANGE	
		port = lwip_htons( vCfg->vport);
#endif
		extFpgaRegisterWrite(EXT_FPGA_REG_PORT_VIDEO, (unsigned char *)&port, 2);

		port = vCfg->aport;
#if _PORT_BYTE_ORDER_CHANGE	
		port = lwip_htons( vCfg->aport);
#endif
		extFpgaRegisterWrite(EXT_FPGA_REG_PORT_AUDIO, (unsigned char *)&port, 2);

		port = vCfg->dport;
#if _PORT_BYTE_ORDER_CHANGE	
		port = lwip_htons( vCfg->dport);
#endif
		extFpgaRegisterWrite(EXT_FPGA_REG_PORT_ANC_DT, (unsigned char *)&port, 2);

		port = vCfg->sport;
#if _PORT_BYTE_ORDER_CHANGE	
		port = lwip_htons( vCfg->sport);
#endif
		extFpgaRegisterWrite(EXT_FPGA_REG_PORT_ANC_ST, (unsigned char *)&port, 2);

//		printf("FPGA Configuration ended!"LWIP_NEW_LINE);

//		EXT_DELAY_MS(5000);
	
		ip4_addr_t *mcIpAddr = (ip4_addr_t *)&vCfg->ip;
		if( ip4_addr_ismulticast(mcIpAddr) )
		{
//			printf("Send IGMP JOIN"LWIP_NEW_LINE);
			ret = EXT_NET_IGMP_JOIN(runCfg->dest.ip);
		}


	}

//	extFgpaRegisterDebug();

	return EXIT_SUCCESS;
}


char *extFgpaReadVersion(void)
{
	int index = 0;
	unsigned char val;

	FPGA_I2C_READ(EXT_FPGA_REG_VERSION, &val, 1);
	index += snprintf(_fpgaVersion+index, sizeof(_fpgaVersion) - index, " %s, Ver:%02x", ((val&0x80)==0)?"RX":"TX", val);

	FPGA_I2C_READ(EXT_FPGA_REG_REVISION, &val, 1);
	index += snprintf(_fpgaVersion+index, sizeof(_fpgaVersion) - index, ".%02x, ", val);
	

	FPGA_I2C_READ(EXT_FPGA_REG_MONTH, &val, 1);
	index += snprintf(_fpgaVersion+index, sizeof(_fpgaVersion) - index, "(Build %02x ", val);

	FPGA_I2C_READ(EXT_FPGA_REG_DAY, &val, 1);
	index += snprintf(_fpgaVersion+index, sizeof(_fpgaVersion) - index, "%02x ", val);
	
	FPGA_I2C_READ(EXT_FPGA_REG_YEAR, &val, 1);
	index += snprintf(_fpgaVersion+index, sizeof(_fpgaVersion) - index, "%02x ", val);

	FPGA_I2C_READ(EXT_FPGA_REG_HOUR, &val, 1);
	index += snprintf(_fpgaVersion+index, sizeof(_fpgaVersion) - index, "%02x:", val);

	FPGA_I2C_READ(EXT_FPGA_REG_MINUTE, &val, 1);
	index += snprintf(_fpgaVersion+index, sizeof(_fpgaVersion) - index, "%02x)", val);

	return _fpgaVersion;
}

char extFpgaReadParams(MuxRunTimeParam *mediaParams)
{

	return EXIT_SUCCESS;
}

char extFpgaWriteParams(MuxRunTimeParam *mediaParams)
{

	return EXIT_SUCCESS;
}


