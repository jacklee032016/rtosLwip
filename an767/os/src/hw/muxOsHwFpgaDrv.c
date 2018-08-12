
#include "muxOs.h"
#include "gpio.h"

#include "muxFpga.h"

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


//#define  FPGA_GET_DONE_SIGNAL()		(gpio_pin_is_high(MUX_PIN_FPGA_PROGRAM ) )
#define  FPGA_GET_DONE_SIGNAL()		(gpio_pin_is_high(MUX_PIN_FPGA_DONE ) )


/* Wait for FPGA to load firmware from SPI memory */
static char  _muxBspFpgaWaitDone(unsigned int  seconds)
{
//	char data;
	uint32_t	timeout;
	char  done = MUX_FALSE;
//	wdt_opt_t watchdog_opt;
	
//	watchdog_opt.us_timeout_period = 4000000;	// timeout after 2 second
	LOCK_FPGA();
	
	timeout = seconds;

	while (1)
	{
//		wdt_enable(&watchdog_opt);
		done = FPGA_GET_DONE_SIGNAL();
		if (done== MUX_FALSE)
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
			printf("Timeout in waiting FPGA Done"MUX_NEW_LINE);
			break;
		}

		timeout--;
		MUX_DELAY_MS(1000);
	}
	
	UNLOCK_FPGA();
	
	return done;
}


/* Reload FPGA from SPI memory */
char  muxBspFpgaReload(void)
{
#define	_TIMEOUT_VALUE		20
	char		isOK = MUX_FALSE;
	unsigned char data;
	int timeoutSecond = _TIMEOUT_VALUE;	/* when compare with negative number, it must be int, not char */

	bspHwClockInit();

	 /* active-LOW reset to configure FPGA */
//	gpio_set_pin_high(MUX_PIN_FPGA_PROGRAM);
//	MUX_DELAY_MS(50);
	if(muxRun.isConfigFpga )
	{
		MUX_INFOF(("Reconfiguring FPGA....."MUX_NEW_LINE));
		gpio_set_pin_low(MUX_PIN_FPGA_PROGRAM);
		
#if 1	
		MUX_DELAY_MS(10);
#else
		isOK = _muxBspFpgaWaitDone(20);
		MUX_DELAY_MS(5000);
#endif	
	}
	else
	{
		MUX_INFOF(("Test version: FPGA is not loaded by MCU"MUX_NEW_LINE));
	}


	gpio_set_pin_high(MUX_PIN_FPGA_PROGRAM);
	
//	isOK = _muxBspFpgaWaitDone(20);
	MUX_DELAY_MS(50);

	/* wait for FPGA done pin
	* up to 20 second is the secondary image is loaded 
	*/
	isOK = _muxBspFpgaWaitDone(20);
	
	printf("FPGA load image %s"MUX_NEW_LINE, (isOK== MUX_FALSE)?"failed":"sucessed");

	printf("Waiting FPGA ....."MUX_NEW_LINE);
	do
	{
		isOK = FPGA_I2C_READ(MUX_FPGA_REG_ETHERNET_RESET, &data, 1);
		timeoutSecond--;
		
//		MUX_ERRORF(("Timeout %d seconds when waiting FPGA status10"MUX_NEW_LINE, timeoutSecond));
		if(timeoutSecond<0)
		{
//			MUX_ERRORF(("Timeout %d seconds when waiting FPGA status0"MUX_NEW_LINE, _TIMEOUT_VALUE));
			break;
		}
		MUX_DELAY_US(1000*100);
	}while(isOK!= EXIT_SUCCESS);

	if(isOK == EXIT_SUCCESS)
	{
		printf("FPGA Ethernet Reset register %d : %2x"MUX_NEW_LINE, MUX_FPGA_REG_ETHERNET_RESET, data);
		data = 0x01;
	
		FPGA_I2C_WRITE(MUX_FPGA_REG_ETHERNET_RESET, &data, 1);

#if 0
	/* test I2C when LMH1983 is not working. J.L. April 10, 2018 */
	bspBistI2cSensor(muxBspCLIGetOutputBuffer(), MUX_COMMAND_BUFFER_SIZE);
	bspBistI2cEeprom(muxBspCLIGetOutputBuffer(), MUX_COMMAND_BUFFER_SIZE);
#endif
	}
	else
	{
		MUX_ERRORF(("Timeout %d seconds when waiting FPGA status"MUX_NEW_LINE, _TIMEOUT_VALUE));
	}
	
	return isOK;
}


void muxFpgaInit(void)
{
	MUX_FPGA_VER ver;
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
		printf("FPGA register %d : %s (%d)"MUX_NEW_LINE, i, (val==data)?"OK":"Failed", data);
	}

	for(i=8; i< 16; i++)
	{
		FPGA_I2C_READ(i, &data, 1);
		printf("FPGA register %d : 0x%2x"MUX_NEW_LINE, i, data);
	}
	
#if 0	
	while(FPGA_I2C_READ(FPGA_VERSION, (unsigned char *)&ver, 3 ) != EXIT_SUCCESS)
	{
//		wdt_enable(&watchdog_opt);
		MUX_DELAY_MS(1000);
		timeout--;
		if (timeout == 0)
		{
			MUX_ERRORF(("FPGA read version timeout"));
			break;	// too long , exit loop
		}
	}
#endif

	printf("FPGA %s, version:%d, revision:%d, device:%d"MUX_NEW_LINE, (ver.rx_tx== MUX_FALSE)?"RX":"TX", ver.version, ver.revision, ver.fpgaDevice);

}


/* IPAddress from Lwip, in network byte order */
char	muxFpgaRegisterWrite(unsigned char baseAddr, unsigned char *data, unsigned char size)
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

void muxFgpaRegisterDebug(void)
{
	int i;
	unsigned char val;
	unsigned short port = 0;

	printf("%s configuration: ", MUX_IS_TX(&muxRun)?"TX":"RX");
	printf(MUX_NEW_LINE"\tLocal :"MUX_NEW_LINE"\t\tIP:\t" );
	for(i=MUX_FPGA_REG_IP; i< MUX_FPGA_REG_IP+4; i++)
	{
		FPGA_I2C_READ(i, &val, 1);
		printf(" %u:%d(0x%2x);", i, val, val);
	}

	printf(MUX_NEW_LINE"\t\tMAC:\t" );
	for(i=MUX_FPGA_REG_MAC; i< MUX_FPGA_REG_MAC+ MUX_MAC_ADDRESS_LENGTH; i++)
	{
		FPGA_I2C_READ(i, &val, 1);
		printf(" %u:0x%2x;", i, val);
	}

	printf(MUX_NEW_LINE"\t\tPort:"MUX_NEW_LINE"\t\t\tVideo:\t" );
	for(i=MUX_FPGA_REG_PORT_VIDEO; i< MUX_FPGA_REG_PORT_VIDEO+2; i++)
	{
		FPGA_I2C_READ(i, &val, 1);
		printf(" %u:0x%2x;", i, val);
		port = (port<<8)|val;	/* byte order changed now */
	}
#if _PORT_BYTE_ORDER_CHANGE	
	printf("(%d)"MUX_NEW_LINE port );
#else
	printf("(%d)"MUX_NEW_LINE, lwip_htons(port));
#endif

	printf("\t\t\tAudio:\t" );
	for(i=MUX_FPGA_REG_PORT_AUDIO; i< MUX_FPGA_REG_PORT_AUDIO+2; i++)
	{
		FPGA_I2C_READ(i, &val, 1);
		printf(" %u:0x%2x;", i, val);
		port = (port<<8)|val;	/* byte order changed now */
	}
#if _PORT_BYTE_ORDER_CHANGE	
	printf("(%d)"MUX_NEW_LINE port );
#else
	printf("(%d)"MUX_NEW_LINE, lwip_htons(port));
#endif

	printf("\t\t\tDT:\t" );
	for(i=MUX_FPGA_REG_PORT_ANC_DT; i< MUX_FPGA_REG_PORT_ANC_DT+2; i++)
	{
		FPGA_I2C_READ(i, &val, 1);
		printf(" %u:0x%2x;", i, val);
		port = (port<<8)|val;	/* byte order changed now */
	}
#if _PORT_BYTE_ORDER_CHANGE	
	printf("(%d)"MUX_NEW_LINE port );
#else
	printf("(%d)"MUX_NEW_LINE, lwip_htons(port));
#endif

	printf("\t\t\tST:\t" );
	for(i=MUX_FPGA_REG_PORT_ANC_ST; i< MUX_FPGA_REG_PORT_ANC_ST+2; i++)
	{
		FPGA_I2C_READ(i, &val, 1);
		printf(" %u:0x%2x;", i, val);
		port = (port<<8)|val;/* byte order changed now */
	}
#if _PORT_BYTE_ORDER_CHANGE	
	printf("(%d)"MUX_NEW_LINE port );
#else
	printf("(%d)"MUX_NEW_LINE, lwip_htons(port));
#endif


	if(MUX_IS_TX(&muxRun) )
	{
		/* TX only */
#if _PORT_BYTE_ORDER_CHANGE		
		printf("\tDest"MUX_NEW_LINE"\t\tIP:\t" );
#else
		printf("\tDest"MUX_NEW_LINE"\t\tIP:\t" );
#endif
		for(i=MUX_FPGA_REG_DEST_IP; i< MUX_FPGA_REG_DEST_IP+4; i++)
		{
			FPGA_I2C_READ(i, &val, 1);
			printf(" %u:%d(0x%2x);", i, val, val);
		}

		printf(MUX_NEW_LINE"\t\tMAC:\t" );
		for(i=MUX_FPGA_REG_DEST_MAC; i< MUX_FPGA_REG_DEST_MAC+ MUX_MAC_ADDRESS_LENGTH; i++)
		{
			FPGA_I2C_READ(i, &val, 1);
			printf(" %u:0x%2x;", i, val);
		}

		printf(MUX_NEW_LINE"\t\tPort:"MUX_NEW_LINE"\t\t\tVideo:\t" );
		port = 0;
		for(i=MUX_FPGA_REG_DEST_PORT_VIDEO; i< MUX_FPGA_REG_DEST_PORT_VIDEO+2; i++)
		{
			FPGA_I2C_READ(i, &val, 1);
			printf(" %u:0x%2x;", i, val);
			port = (port<<8)|val;
		}
#if _PORT_BYTE_ORDER_CHANGE	
		printf("(%d)"MUX_NEW_LINE, port );
#else
		printf("(%d)"MUX_NEW_LINE, lwip_htons(port));
#endif

		printf("\t\t\tAudio:\t" );
		port = 0;
		for(i=MUX_FPGA_REG_DEST_PORT_AUDIO; i< MUX_FPGA_REG_DEST_PORT_AUDIO+2; i++)
		{
			FPGA_I2C_READ(i, &val, 1);
			printf(" %u:0x%2x;", i, val);
			port = (port<<8)|val;
		}
#if _PORT_BYTE_ORDER_CHANGE	
		printf("(%d)"MUX_NEW_LINE, port );
#else
		printf("(%d)"MUX_NEW_LINE, lwip_htons(port));
#endif

		printf("\t\t\tDT:\t" );
		port = 0;
		for(i=MUX_FPGA_REG_DEST_PORT_ANC_DT; i< MUX_FPGA_REG_DEST_PORT_ANC_DT+2; i++)
		{
			FPGA_I2C_READ(i, &val, 1);
			printf(" %u:0x%2x;", i, val);
			port = (port<<8)|val;
		}
#if _PORT_BYTE_ORDER_CHANGE	
		printf("(%d)"MUX_NEW_LINE, port );
#else
		printf("(%d)"MUX_NEW_LINE, lwip_htons(port));
#endif

		printf("\t\t\tST:\t" );
		port = 0;
		for(i=MUX_FPGA_REG_DEST_PORT_ANC_ST; i< MUX_FPGA_REG_DEST_PORT_ANC_ST+2; i++)
		{
			FPGA_I2C_READ(i, &val, 1);
			printf(" %u:0x%2x;", i, val);
			port = (port<<8)|val;
		}
#if _PORT_BYTE_ORDER_CHANGE	
		printf("(%d)"MUX_NEW_LINE, port );
#else
		printf("(%d)"MUX_NEW_LINE, lwip_htons(port));
#endif

	}
	
#if _PORT_BYTE_ORDER_CHANGE	
	printf("FPGA !!" MUX_NEW_LINE);
#else
	printf("FPGA !!" MUX_NEW_LINE);
#endif
}


static void _changeByteOrderOfMac(MUX_MAC_ADDRESS *mac, unsigned char *address)
{
	int i;
	for(i=0; i<MUX_MAC_ADDRESS_LENGTH; i++ )
	{
		address[i] = mac->address[MUX_MAC_ADDRESS_LENGTH-i-1];
	}
}

void	muxFpgaEnable(char	isEnable)
{
	unsigned char		data = 0x01;
	if(isEnable == 0)
	{
		data = 0;
	}

	muxFpgaRegisterWrite(MUX_FPGA_REG_ENABLE, &data, 1);

}

char	muxFpgaConfig(MUX_RUNTIME_CFG *runCfg)
{
	MUX_MAC_ADDRESS destMac, *mac;
	char ret;
	unsigned int ip;
	unsigned char		address[MUX_MAC_ADDRESS_LENGTH];
	unsigned short port = 0;
	MUX_VIDEO_CONFIG *vCfg;


	/*configure local address/port, for both RX/TX*/
	
//	muxFpgaRegisterWrite(MUX_FPGA_REG_PORT_AUDIO, (unsigned char *)&runCfg->local.aport, 2);

	/*configure dest, only for TX */
	if(MUX_IS_TX(runCfg) )
	{
		/* local */
		_changeByteOrderOfMac(&runCfg->local.mac, address);
		ip = lwip_htonl(runCfg->local.ip);

		muxFpgaRegisterWrite(MUX_FPGA_REG_MAC, address, MUX_MAC_ADDRESS_LENGTH);
		muxFpgaRegisterWrite(MUX_FPGA_REG_IP, (unsigned char *)&ip, 4);

		port = runCfg->local.vport;
#if _PORT_BYTE_ORDER_CHANGE	
		port = lwip_htons( runCfg->local.vport);
#endif
		muxFpgaRegisterWrite(MUX_FPGA_REG_PORT_VIDEO, (unsigned char *)&port, 2);

		port = runCfg->local.aport;
#if _PORT_BYTE_ORDER_CHANGE	
		port = lwip_htons( runCfg->local.aport);
#endif
		muxFpgaRegisterWrite(MUX_FPGA_REG_PORT_AUDIO, (unsigned char *)&port, 2);

		port = runCfg->local.dport;
#if _PORT_BYTE_ORDER_CHANGE	
		port = lwip_htons( runCfg->local.dport);
#endif
		muxFpgaRegisterWrite(MUX_FPGA_REG_PORT_ANC_DT, (unsigned char *)&port, 2);

		port = runCfg->local.sport;
#if _PORT_BYTE_ORDER_CHANGE	
		port = lwip_htons( runCfg->local.sport);
#endif
		muxFpgaRegisterWrite(MUX_FPGA_REG_PORT_ANC_ST, (unsigned char *)&port, 2);

		/* dest */
		ip = lwip_htonl(runCfg->dest.ip);
		muxFpgaRegisterWrite(MUX_FPGA_REG_DEST_IP, (unsigned char *)&ip, 4);

		ret = muxNetMulticastIP4Mac(&runCfg->dest.ip, &destMac);
		if(ret == EXIT_SUCCESS)
		{/* dest MAC is multicast MAC address */
			mac = &destMac;
		}
		else
		{
			mac = &runCfg->dest.mac;
		}
		_changeByteOrderOfMac(mac, address);

		muxFpgaRegisterWrite(MUX_FPGA_REG_DEST_MAC, address, MUX_MAC_ADDRESS_LENGTH);

		port = runCfg->dest.vport;
#if _PORT_BYTE_ORDER_CHANGE	
		port = lwip_htons(runCfg->dest.vport);
#endif
		muxFpgaRegisterWrite(MUX_FPGA_REG_DEST_PORT_VIDEO, (unsigned char *)&port, 2);

		port = runCfg->dest.aport;
#if _PORT_BYTE_ORDER_CHANGE	
		port = lwip_htons(runCfg->dest.aport);
#endif
		muxFpgaRegisterWrite(MUX_FPGA_REG_DEST_PORT_AUDIO, (unsigned char *)&port, 2);

		port = runCfg->dest.dport;
#if _PORT_BYTE_ORDER_CHANGE	
		port = lwip_htons(runCfg->dest.dport);
#endif
		muxFpgaRegisterWrite(MUX_FPGA_REG_DEST_PORT_ANC_DT, (unsigned char *)&port, 2);

		port = runCfg->dest.sport;
#if _PORT_BYTE_ORDER_CHANGE	
		port = lwip_htons(runCfg->dest.sport);
#endif
		muxFpgaRegisterWrite(MUX_FPGA_REG_DEST_PORT_ANC_ST, (unsigned char *)&port, 2);

//		muxFpgaRegisterWrite(MUX_FPGA_REG_DEST_PORT_AUDIO, (unsigned char *)&runCfg->audioPortDest, 2);
		
	//	muxFpgaEnable(1);
	}
	else
	{/* RX */
//		const ip4_addr_t *mcIpAddr;

		vCfg = &runCfg->dest;

		_changeByteOrderOfMac(&vCfg->mac, address);
		ip = lwip_htonl(vCfg->ip);

		muxFpgaRegisterWrite(MUX_FPGA_REG_MAC, address, MUX_MAC_ADDRESS_LENGTH);

		muxFpgaRegisterWrite(MUX_FPGA_REG_IP, (unsigned char *)&ip, 4);

		port = vCfg->vport;
#if _PORT_BYTE_ORDER_CHANGE	
		port = lwip_htons( vCfg->vport);
#endif
		muxFpgaRegisterWrite(MUX_FPGA_REG_PORT_VIDEO, (unsigned char *)&port, 2);

		port = vCfg->aport;
#if _PORT_BYTE_ORDER_CHANGE	
		port = lwip_htons( vCfg->aport);
#endif
		muxFpgaRegisterWrite(MUX_FPGA_REG_PORT_AUDIO, (unsigned char *)&port, 2);

		port = vCfg->dport;
#if _PORT_BYTE_ORDER_CHANGE	
		port = lwip_htons( vCfg->dport);
#endif
		muxFpgaRegisterWrite(MUX_FPGA_REG_PORT_ANC_DT, (unsigned char *)&port, 2);

		port = vCfg->sport;
#if _PORT_BYTE_ORDER_CHANGE	
		port = lwip_htons( vCfg->sport);
#endif
		muxFpgaRegisterWrite(MUX_FPGA_REG_PORT_ANC_ST, (unsigned char *)&port, 2);

//		printf("FPGA Configuration ended!"LWIP_NEW_LINE);

//		MUX_DELAY_MS(5000);
	
		ip4_addr_t *mcIpAddr = (ip4_addr_t *)&vCfg->ip;
		if( ip4_addr_ismulticast(mcIpAddr) )
		{
//			printf("Send IGMP JOIN"LWIP_NEW_LINE);
			ret = MUX_NET_IGMP_JOIN(runCfg->dest.ip);
		}


	}

//	muxFgpaRegisterDebug();

	return EXIT_SUCCESS;
}


char *muxFgpaReadVersion(void)
{
	int index = 0;
	unsigned char val;

	FPGA_I2C_READ(MUX_FPGA_REG_VERSION, &val, 1);
	index += snprintf(_fpgaVersion+index, sizeof(_fpgaVersion) - index, " %s, Ver:%02x", ((val&0x80)==0)?"RX":"TX", val);

	FPGA_I2C_READ(MUX_FPGA_REG_REVISION, &val, 1);
	index += snprintf(_fpgaVersion+index, sizeof(_fpgaVersion) - index, ".%02x, ", val);
	

	FPGA_I2C_READ(MUX_FPGA_REG_MONTH, &val, 1);
	index += snprintf(_fpgaVersion+index, sizeof(_fpgaVersion) - index, "(Build %02x ", val);

	FPGA_I2C_READ(MUX_FPGA_REG_DAY, &val, 1);
	index += snprintf(_fpgaVersion+index, sizeof(_fpgaVersion) - index, "%02x ", val);
	
	FPGA_I2C_READ(MUX_FPGA_REG_YEAR, &val, 1);
	index += snprintf(_fpgaVersion+index, sizeof(_fpgaVersion) - index, "%02x ", val);

	FPGA_I2C_READ(MUX_FPGA_REG_HOUR, &val, 1);
	index += snprintf(_fpgaVersion+index, sizeof(_fpgaVersion) - index, "%02x:", val);

	FPGA_I2C_READ(MUX_FPGA_REG_MINUTE, &val, 1);
	index += snprintf(_fpgaVersion+index, sizeof(_fpgaVersion) - index, "%02x)", val);

	return _fpgaVersion;
}

