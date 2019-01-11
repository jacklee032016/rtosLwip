
#include "eos.h"
#include "gpio.h"

#include "eosFpga.h"

#include "lwipExt.h"
#include "jsmn.h"

struct Tx_Register_Address
{
	uint8_t		srcMac;
	uint8_t		srcIp;

	uint8_t		srcPortV;
	uint8_t		srcPortA;
	uint8_t		srcPortAnc;
	uint8_t		srcPortAuc;

	uint8_t		dstVideoIp;
	uint8_t		dstVideoMac;
	uint8_t		dstVideoPort;

	uint8_t		dstAudioIp;
	uint8_t		dstAudioMac;
	uint8_t		dstAudioPort;

	uint8_t		dstAncIp;
	uint8_t		dstAncMac;
	uint8_t		dstAncPort;

	uint8_t		dstAuxIp;
	uint8_t		dstAuxMac;
	uint8_t		dstAuxPort;
};


const struct Tx_Register_Address FTX_ADDRESS =
{
	srcMac	: 	46,
	srcIp	:	52,

	srcPortV		:	56,
	srcPortA		:	58,
	srcPortAnc	:	60,
	srcPortAuc	:	62,

	dstVideoIp	:	88,
	dstVideoMac	:	64,
	dstVideoPort	:	104,

	dstAudioIp	:	92,
	dstAudioMac	:	70,
	dstAudioPort	:	106,

	dstAncIp		:	96,
	dstAncMac	:	76,
	dstAncPort	:	108,

	dstAuxIp		:	100,
	dstAuxMac	:	82,
	dstAuxPort	:	110
};


struct Rx_Register_Address
{
	uint8_t		localMac;
	uint8_t		localIp;

	uint8_t		mulIpVideo;
	uint8_t		mulIpAudio;
	uint8_t		mulIpAnc;
	uint8_t		mulIpAux;

	uint8_t		portVideo;
	uint8_t		portAudio;
	uint8_t		portAnc;
	uint8_t		portAux;

	uint8_t		srcIp;
	uint8_t		srcMac;

	uint8_t		srcPortVideo;
	uint8_t		srcPortAudio;
	uint8_t		srcPortAnc;
	uint8_t		srcPortAux;

};


const struct Rx_Register_Address FRX_ADDRESS = 
{
	localMac		:	56,
	localIp		:	52,

	mulIpVideo	:	46,
	mulIpAudio	:	49,
	mulIpAnc		:	88,
	mulIpAux		:	91,

	portVideo		:	62,
	portAudio		:	76,
	portAnc		:	80,
	portAux		:	84,

	srcIp		:	64,
	srcMac		:	68,

	srcPortVideo	:	74,
	srcPortAudio	:	78,
	srcPortAnc	:	82,
	srcPortAux	:	86,
};



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

#define	_extFpgaRead3Bytes(address, intVal) \
{	unsigned char *p = (unsigned char *)(intVal); *intVal=0;  _extFpgaRegisterRead((address), p, 3); *intVal = (lwip_ntohl(*intVal)|(239) ) ; }


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
{	unsigned short _value = lwip_ntohs(*((unsigned short *)(shortVal))); _extFpgaRegisterWrite((address), (unsigned char *)&_value, 2); }


#define	_extFpgaWriteShort(address, shortVal) \
{	_extFpgaRegisterWrite((address), shortVal, 2); }


#define	_extFpgaWrite3Bytes(address, intVal) \
{	unsigned int _value = lwip_ntohl(*((unsigned int *)(intVal))); unsigned char *p = (unsigned char *)&_value; _extFpgaRegisterWrite((address), p, 3); }


#define	_extFpgaWriteInteger(address, intVal) \
{	unsigned int _value = lwip_ntohl(*((unsigned int *)(intVal))); _extFpgaRegisterWrite((address), (unsigned char *)&_value, 4); }



static void _changeByteOrderOfMac(EXT_MAC_ADDRESS *mac, unsigned char *address)
{
	int i;
	for(i=0; i<EXT_MAC_ADDRESS_LENGTH; i++ )
	{
		address[i] = mac->address[EXT_MAC_ADDRESS_LENGTH-i-1];
	}
}


static char _frpgaReadParamRegisters(EXT_RUNTIME_CFG *runCfg)
{
	unsigned char _chValue;
	int i;
	
	_extFpgaReadShort(EXT_FPGA_REG_WIDTH, (unsigned char *)&runCfg->runtime.vWidth);
	
	_extFpgaReadShort(EXT_FPGA_REG_HEIGHT, (unsigned char *)&runCfg->runtime.vHeight);
	
	_extFpgaReadByte(EXT_FPGA_REG_FRAMERATE, &runCfg->runtime.vFrameRate);

	_extFpgaReadByte(EXT_FPGA_REG_SAMPLING, &runCfg->runtime.vColorSpace);

	_extFpgaReadByte(EXT_FPGA_REG_DEPTH, &runCfg->runtime.vDepth);

	_extFpgaReadByte(EXT_FPGA_REG_INTLC_SEGM, &runCfg->runtime.vIsInterlaced);

	/* audio */	
	_extFpgaReadByte(EXT_FPGA_REG_AUDIO_CHANNELS, &_chValue);
	runCfg->runtime.aChannels = 0;
	for(i=0; i< 4; i++)
	{
		if( (_chValue &(1<<i)) )
		{
			runCfg->runtime.aChannels += 4;
		}
	}
	
	_extFpgaReadByte(EXT_FPGA_REG_AUDIO_RATE, &runCfg->runtime.aSampleRate);
	
	_extFpgaReadByte(EXT_FPGA_REG_AUDIO_PKT_SIZE, &runCfg->runtime.aPktSize);

	return EXIT_SUCCESS;
}

unsigned int extFgpaRegisterDebug( char *data, unsigned int size)
{
	int index = 0;
//	unsigned char val;
	unsigned short port = 0;
	unsigned int	intValue;
	EXT_MAC_ADDRESS destMac;
	unsigned char		address[EXT_MAC_ADDRESS_LENGTH], _chVal;
	
	EXT_RUNTIME_CFG *rxCfg = &tmpRuntime;
	extSysClearConfig(rxCfg);
	
	_frpgaReadParamRegisters(rxCfg);

	index += snprintf(data+index, size-index, "%s;"EXT_NEW_LINE, extFgpaReadVersion() );
	
	index += snprintf(data+index, size-index, "%s configuration: ", EXT_IS_TX(&extRun)?"TX":"RX");

	if(EXT_IS_TX(&extRun) )
	{
		_extFpgaReadInteger(FTX_ADDRESS.srcIp, (unsigned char *)&intValue);
		index += snprintf(data+index, size-index, EXT_NEW_LINE"Local :"EXT_NEW_LINE"\tIP\t: %s", inet_ntoa(intValue));

		index += snprintf(data+index, size-index, EXT_NEW_LINE"\tMAC\t:" );
		_extFpgaRegisterRead(FTX_ADDRESS.srcMac,  (unsigned char *)&destMac, EXT_MAC_ADDRESS_LENGTH);
		_changeByteOrderOfMac(&destMac, address);
		index += snprintf(data+index, size-index, " %02x:%02x:%02x:%02x:%02x:%02x:", address[0], address[1], address[2], address[3], address[4], address[5]);

		index += snprintf(data+index, size-index, EXT_NEW_LINE"\tPort\t: Video:" );
		_extFpgaReadShort(FTX_ADDRESS.srcPortV,  (unsigned char *)&port);
		index += snprintf(data+index, size-index, "%d;", port);

		index += snprintf(data+index, size-index," Audio:" );
		_extFpgaReadShort(FTX_ADDRESS.srcPortA,  (unsigned char *)&port);
		index += snprintf(data+index, size-index, "%d;", port);

		index += snprintf(data+index, size-index, " ANC:" );
		_extFpgaReadShort(FTX_ADDRESS.srcPortAnc,  (unsigned char *)&port);
		index += snprintf(data+index, size-index, "%d;", port);

		index += snprintf(data+index, size-index, " AUX:" );
		_extFpgaReadShort(FTX_ADDRESS.srcPortAuc,  (unsigned char *)&port);
		index += snprintf(data+index, size-index, "%d;"EXT_NEW_LINE, port);

		/* TX only dest: video */
		_extFpgaReadInteger(FTX_ADDRESS.dstVideoIp, (unsigned char *)&intValue);
		_extFpgaRegisterRead(FTX_ADDRESS.dstVideoMac, (unsigned char *)&destMac, EXT_MAC_ADDRESS_LENGTH);
		_changeByteOrderOfMac(&destMac, address);
		_extFpgaReadShort(FTX_ADDRESS.dstVideoPort,  (unsigned char *)&port);
		index += snprintf(data+index, size-index, EXT_NEW_LINE"Dest :"EXT_NEW_LINE"\tVideo: \tIP:%s;", inet_ntoa(intValue));
		index += snprintf(data+index, size-index, "\tMAC:" );
		index += snprintf(data+index, size-index, " %02x:%02x:%02x:%02x:%02x:%02x;", address[0], address[1], address[2], address[3], address[4], address[5]);
		index += snprintf(data+index, size-index, "\tPort:%d;", port);

		/* audio */
		_extFpgaReadInteger(FTX_ADDRESS.dstAudioIp, (unsigned char *)&intValue);
		_extFpgaRegisterRead(FTX_ADDRESS.dstAudioMac, (unsigned char *)&destMac, EXT_MAC_ADDRESS_LENGTH);
		_changeByteOrderOfMac(&destMac, address);
		_extFpgaReadShort(FTX_ADDRESS.dstAudioPort,  (unsigned char *)&port);
		index += snprintf(data+index, size-index, EXT_NEW_LINE"\tAudio: \tIP:%s;", inet_ntoa(intValue));
		index += snprintf(data+index, size-index, "\tMAC:" );
		index += snprintf(data+index, size-index, " %02x:%02x:%02x:%02x:%02x:%02x;", address[0], address[1], address[2], address[3], address[4], address[5]);
		index += snprintf(data+index, size-index, "\tPort:%d;", port);

		_extFpgaReadInteger(FTX_ADDRESS.dstAncIp, (unsigned char *)&intValue);
		_extFpgaRegisterRead(FTX_ADDRESS.dstAncMac, (unsigned char *)&destMac, EXT_MAC_ADDRESS_LENGTH);
		_changeByteOrderOfMac(&destMac, address);
		_extFpgaReadShort(FTX_ADDRESS.dstAncPort,  (unsigned char *)&port);
		index += snprintf(data+index, size-index, EXT_NEW_LINE"\tANC : \tIP:%s;", inet_ntoa(intValue));
		index += snprintf(data+index, size-index, "\tMAC:" );
		index += snprintf(data+index, size-index, " %02x:%02x:%02x:%02x:%02x:%02x;", address[0], address[1], address[2], address[3], address[4], address[5]);
		index += snprintf(data+index, size-index, "\tPort:%d;", port);

#if EXT_FPGA_AUX_ON	
		_extFpgaReadInteger(FTX_ADDRESS.dstAuxIp, (unsigned char *)&intValue);
		_extFpgaRegisterRead(FTX_ADDRESS.dstAuxMac, (unsigned char *)&destMac, EXT_MAC_ADDRESS_LENGTH);
		_changeByteOrderOfMac(&destMac, address);
		_extFpgaReadShort(FTX_ADDRESS.dstAuxPort,  (unsigned char *)&port);
		index += snprintf(data+index, size-index, EXT_NEW_LINE"\tAUX : \tIP:%s;", inet_ntoa(intValue));
		index += snprintf(data+index, size-index, "\tMAC:" );
		index += snprintf(data+index, size-index, " %02x:%02x:%02x:%02x:%02x:%02x;", address[0], address[1], address[2], address[3], address[4], address[5]);
		index += snprintf(data+index, size-index, "\tPort:%d;", port);
#endif		
		_extFpgaReadByte(EXT_FPGA_REG_SDI_STATUS, &_chVal);
		index += snprintf(data+index, size-index, "LockReg :%02x;"EXT_NEW_LINE, _chVal );
	}
	else
	{
		_extFpgaReadInteger(FRX_ADDRESS.localIp, (unsigned char *)&intValue);
		index += snprintf(data+index, size-index, EXT_NEW_LINE"Local :"EXT_NEW_LINE"\tIP\t: %s", inet_ntoa(intValue));

		index += snprintf(data+index, size-index, EXT_NEW_LINE"\tMAC\t:" );
		_extFpgaRegisterRead(FRX_ADDRESS.localMac,  (unsigned char *)&destMac, EXT_MAC_ADDRESS_LENGTH);
		_changeByteOrderOfMac(&destMac, address);
		index += snprintf(data+index, size-index, " %02x:%02x:%02x:%02x:%02x:%02x:", address[0], address[1], address[2], address[3], address[4], address[5]);

		_extFpgaRead3Bytes(FRX_ADDRESS.mulIpVideo, &intValue);
		_extFpgaReadShort(FRX_ADDRESS.portVideo,  (unsigned char *)&port);
		index += snprintf(data+index, size-index, EXT_NEW_LINE"Dest :"EXT_NEW_LINE"\tVideo :\tIP:%s;\tPort:%d;", inet_ntoa(intValue), port);

		_extFpgaRead3Bytes(FRX_ADDRESS.mulIpAudio, &intValue);
		_extFpgaReadShort(FRX_ADDRESS.portAudio,  (unsigned char *)&port);
		index += snprintf(data+index, size-index, EXT_NEW_LINE"\tAudio :\tIP:%s;\tPort:%d;", inet_ntoa(intValue), port);

		_extFpgaRead3Bytes(FRX_ADDRESS.mulIpAnc, &intValue);
		_extFpgaReadShort(FRX_ADDRESS.portAnc,  (unsigned char *)&port);
		index += snprintf(data+index, size-index, EXT_NEW_LINE"\tANC : \tIP:%s;\tPort:%d;", inet_ntoa(intValue), port);

#if EXT_FPGA_AUX_ON	
		_extFpgaRead3Bytes(FRX_ADDRESS.mulIpAux, &intValue);
		_extFpgaReadShort(FRX_ADDRESS.portAux,  (unsigned char *)&port);
		index += snprintf(data+index, size-index, EXT_NEW_LINE"\tAUX : \tIP:%s;\tPort:%d;", inet_ntoa(intValue), port);
#endif
	}


	index += snprintf(data+index, size-index, EXT_NEW_LINE"Video :\tWxH:%hux%hu; ", rxCfg->runtime.vWidth, rxCfg->runtime.vHeight);
	index += snprintf(data+index, size-index, "FPS:%d; ", CMN_INT_FIND_NAME_V_FPS(rxCfg->runtime.vFrameRate));
	index += snprintf(data+index, size-index, "Depth:%d; ", CMN_INT_FIND_NAME_V_DEPTH(rxCfg->runtime.vDepth) );
	index += snprintf(data+index, size-index, "ColorSpace:%s; ", CMN_FIND_V_COLORSPACE(rxCfg->runtime.vColorSpace));
	index += snprintf(data+index, size-index, "%s;"EXT_NEW_LINE, (rxCfg->runtime.vIsInterlaced == EXT_VIDEO_INTLC_INTERLACED)?"Interlaced":"Progressive");

	index += snprintf(data+index, size-index, "Audio :\tChannels:%d; Sample Rate:%d; Pkt Size: %s"EXT_NEW_LINE, 
		rxCfg->runtime.aChannels, rxCfg->runtime.aSampleRate, CMN_FIND_A_PKTSIZE(rxCfg->runtime.aPktSize) );

	_extFpgaReadByte(EXT_FPGA_REG_ENABLE, &_chVal);
	_extFpgaReadByte(EXT_FPGA_REG_PARAM_STATUS, address);
	index += snprintf(data+index, size-index, "EnableReg :%02x;\tUpdateRgr:%02x"EXT_NEW_LINE, _chVal, address[0] );
	
	return index;
}



char	extFpgaConfig(EXT_RUNTIME_CFG *runCfg )
{
	EXT_MAC_ADDRESS destMac, *mac;
	char ret;
	unsigned char value;
	unsigned char		address[EXT_MAC_ADDRESS_LENGTH];
	EXT_VIDEO_CONFIG *vCfg;
	
	struct netif *_netif = (struct netif *)runCfg->netif;
	const ip4_addr_t *_netIfIpAddr = netif_ip4_addr(_netif);

	unsigned int ip = (_netIfIpAddr->addr == IPADDR_ANY)?runCfg->local.ip:_netIfIpAddr->addr;

	/*configure local address/port, for both RX/TX*/
	
//	_extFpgaRegisterWrite(EXT_FPGA_REG_PORT_AUDIO, (unsigned char *)&runCfg->local.aport, 2);

	if( IS_SECURITY_CHIP_EXIST(runCfg->sc) && bspScCheckMAC(runCfg->sc)== EXIT_FAILURE)
	{
		value = EXT_FPGA_FLAGS_DISABLE_ALL;	
		_extFpgaWriteByte(EXT_FPGA_REG_ENABLE, &value);

		return EXIT_FAILURE;
	}

	/*configure dest, only for TX */
	if(EXT_IS_TX(runCfg) )
	{
		/* local */
		_changeByteOrderOfMac(&runCfg->local.mac, address);
		_extFpgaRegisterWrite(FTX_ADDRESS.srcMac, address, EXT_MAC_ADDRESS_LENGTH);

		_extFpgaWriteInteger(FTX_ADDRESS.srcIp, &ip);

		_extFpgaWriteShort(FTX_ADDRESS.srcPortV, (unsigned char *)&runCfg->local.vport);
		
		_extFpgaWriteShort(FTX_ADDRESS.srcPortA, (unsigned char *)&runCfg->local.aport);
		_extFpgaWriteShort(FTX_ADDRESS.srcPortAnc, (unsigned char *)&runCfg->local.dport);
#if EXT_FPGA_AUX_ON	
		_extFpgaWriteShort(FTX_ADDRESS.srcPortAuc, (unsigned char *)&runCfg->local.sport);
#endif
		/* dest */
		/* video */
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
		_extFpgaRegisterWrite(FTX_ADDRESS.dstVideoMac, address, EXT_MAC_ADDRESS_LENGTH);

		_extFpgaWriteInteger(FTX_ADDRESS.dstVideoIp, &runCfg->dest.ip);
		_extFpgaWriteShort(FTX_ADDRESS.dstVideoPort, (unsigned char *)&runCfg->dest.vport);
		
		/* audio */
		ret = extNetMulticastIP4Mac(&runCfg->dest.audioIp, &destMac);
		if(ret == EXIT_SUCCESS)
		{/* dest MAC is multicast MAC address */
			mac = &destMac;
		}
		else
		{
			mac = &runCfg->dest.mac;
		}
		_changeByteOrderOfMac(mac, address);
		_extFpgaRegisterWrite(FTX_ADDRESS.dstAudioMac, address, EXT_MAC_ADDRESS_LENGTH);

		_extFpgaWriteInteger(FTX_ADDRESS.dstAudioIp, &runCfg->dest.audioIp);
		_extFpgaWriteShort(FTX_ADDRESS.dstAudioPort, (unsigned char *)&runCfg->dest.aport);

		/* ANC */
		ret = extNetMulticastIP4Mac(&runCfg->dest.ancIp, &destMac);
		if(ret == EXIT_SUCCESS)
		{/* dest MAC is multicast MAC address */
			mac = &destMac;
		}
		else
		{
			mac = &runCfg->dest.mac;
		}
		_changeByteOrderOfMac(mac, address);
		_extFpgaRegisterWrite(FTX_ADDRESS.dstAncMac, address, EXT_MAC_ADDRESS_LENGTH);

		_extFpgaWriteInteger(FTX_ADDRESS.dstAncIp, &runCfg->dest.ancIp);
		_extFpgaWriteShort(FTX_ADDRESS.dstAncPort, (unsigned char *)&runCfg->dest.dport);

		/* AUX */
#if EXT_FPGA_AUX_ON	
		ret = extNetMulticastIP4Mac(&runCfg->dest.auxIp, &destMac);
		if(ret == EXIT_SUCCESS)
		{/* dest MAC is multicast MAC address */
			mac = &destMac;
		}
		else
		{
			mac = &runCfg->dest.mac;
		}
		_changeByteOrderOfMac(mac, address);
		_extFpgaRegisterWrite(FTX_ADDRESS.dstAuxMac, address, EXT_MAC_ADDRESS_LENGTH);

		_extFpgaWriteInteger(FTX_ADDRESS.dstAuxIp, &runCfg->dest.auxIp);
#if EXT_FPGA_AUX_ON	
		_extFpgaWriteShort(FTX_ADDRESS.dstAuxPort, (unsigned char *)&runCfg->dest.sport);
#endif

#endif/* AUX */


	//	extFpgaEnable(1);
	}
	else
	{/* RX */
//		const ip4_addr_t *mcIpAddr;
//		unsigned int	intValue;

		vCfg = &runCfg->dest;
#if 0
		if(_netIfIpAddr->addr != IPADDR_ANY)
		{
			return EXIT_SUCCESS;
		}
#endif
		EXT_INFOF(("RX is configuring"));

		/* local IP/MAC */
		_extFpgaWriteInteger(FRX_ADDRESS.localIp, &ip);
//		_changeByteOrderOfMac(&vCfg->mac, address);
		_changeByteOrderOfMac(&runCfg->local.mac, address);
		_extFpgaRegisterWrite(FRX_ADDRESS.localMac, address, EXT_MAC_ADDRESS_LENGTH);

		/* dest multi Video */
		_extFpgaWrite3Bytes(FRX_ADDRESS.mulIpVideo, &vCfg->ip);
		_extFpgaWriteShort(FRX_ADDRESS.portVideo, (unsigned char *)&vCfg->vport );

		/* dest multi Audio */
		_extFpgaWrite3Bytes(FRX_ADDRESS.mulIpAudio, &vCfg->audioIp);
		_extFpgaWriteShort(FRX_ADDRESS.portAudio, (unsigned char *)&vCfg->aport );

		/* dest multi Anx */
		_extFpgaWrite3Bytes(FRX_ADDRESS.mulIpAnc, &vCfg->ancIp);
		_extFpgaWriteShort(FRX_ADDRESS.portAnc, (unsigned char *)&vCfg->dport );

#if EXT_FPGA_AUX_ON	
		/* dest multi Aux */
		_extFpgaWrite3Bytes(FRX_ADDRESS.mulIpAux, &vCfg->auxIp);
		_extFpgaWriteShort(FRX_ADDRESS.portAux, (unsigned char *)&vCfg->sport );
#endif


#if  1
#define	_USING_OR_OP		1
		/* OR_Op must be used to make RTK switch chipset working. 09.21, 2018 */
		/* RX version fromo 09.04, 2018 */
		/* reset */
#if _USING_OR_OP
		_extFpgaReadByte(EXT_FPGA_REG_ETHERNET_RESET, &value);
		EXT_DEBUGF(EXT_DBG_ON, ("RX Read %x from register 0x%x", value, EXT_FPGA_REG_ETHERNET_RESET));
		value = (value | (1<<1));
#else		
		value = 0x02;
#endif
		EXT_DEBUGF(EXT_DBG_ON, ("RX Write %x to register 0x%x: reset FPGA", value, EXT_FPGA_REG_ETHERNET_RESET));
		_extFpgaWriteByte(EXT_FPGA_REG_ETHERNET_RESET, &value);

		/* release reset */
#if _USING_OR_OP
		_extFpgaReadByte(EXT_FPGA_REG_ETHERNET_RESET, &value);
		EXT_DEBUGF(EXT_DBG_ON, ("RX ReRead %x from register 0x%x", value, EXT_FPGA_REG_ETHERNET_RESET));
		value = value & 0xFD;
#else		
		value = 0x00;
#endif
		EXT_DEBUGF(EXT_DBG_ON,  ("TX ReWrite %x to register 0x%x: release reset of FPGA", value, EXT_FPGA_REG_ETHERNET_RESET));
		_extFpgaWriteByte(EXT_FPGA_REG_ETHERNET_RESET, &value);
#endif

#if 0	
		ip4_addr_t *mcIpAddr = (ip4_addr_t *)&vCfg->ip;
		if( ip4_addr_ismulticast(mcIpAddr) )
		{/* join */
			if( (_netIfIpAddr->addr == IPADDR_ANY) )
			{
				EXT_DEBUGF(EXT_DBG_ON,  ("netif is not available, IGMP group can't join now"));
			}
			else
			{
//				printf("Send IGMP JOIN"LWIP_NEW_LINE);
				ret = extLwipGroupMgr(runCfg, runCfg->dest.ip, EXT_TRUE);
			}
		}
#endif

		extFpgaConfigParams(runCfg);


		 extIgmpGroupMgr(runCfg, EXT_TRUE);

//		EXT_DELAY_MS(5000);

		EXT_DEBUGF(EXT_DBG_ON,  ("FPGA Configuration ended!"));

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

#if 0
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
#endif

void extFpgaTimerJob(MuxRunTimeParam  *mediaParams)
{
	unsigned char value;


	_extFpgaReadByte(EXT_FPGA_REG_SDI_STATUS, &value);
	if(value == EXT_FPGA_FLAGS_SDI_CONNECTTED)
	{
		mediaParams->isConnect = EXT_TRUE;
	}
	else
	{
		mediaParams->isConnect = EXT_FALSE;
	}

	_extFpgaReadByte(EXT_FPGA_REG_PARAM_STATUS, &value);
	if(value != EXT_FPGA_FLAGS_PARAM_USABLE)
		return;

	EXT_DEBUGF(EXT_DBG_ON, ("New Media Params is available now!"));
	
//	extFpgaReadParams(mediaParams);
	/* clear register */
	_extFpgaWriteByte(EXT_FPGA_REG_PARAM_STATUS, &value);
	
	return;
}

char extFpgaReadParams( EXT_RUNTIME_CFG *runCfg)
{
	unsigned char _chValue;

	if(! EXT_IS_TX(runCfg))
	{
		return EXIT_FAILURE;
	}

	_extFpgaReadShort(EXT_FPGA_REG_SDI_STATUS, (unsigned char *)&_chValue);
//	EXT_DEBUGF(EXT_DBG_OFF, ("Lock %02x from register 0x%x", _chValue, EXT_FPGA_REG_SDI_STATUS));
	if(!_chValue)
	{
		FIELD_INVALIDATE_U16(runCfg->runtime.vWidth);
		FIELD_INVALIDATE_U16(runCfg->runtime.vHeight);

		FIELD_INVALIDATE_U8(runCfg->runtime.vFrameRate);
		FIELD_INVALIDATE_U8(runCfg->runtime.vColorSpace);
		FIELD_INVALIDATE_U8(runCfg->runtime.vDepth);
		FIELD_INVALIDATE_U8(runCfg->runtime.vIsInterlaced);
		
		FIELD_INVALIDATE_U8(runCfg->runtime.aChannels);
		FIELD_INVALIDATE_U8(runCfg->runtime.aSampleRate);
		FIELD_INVALIDATE_U8(runCfg->runtime.aPktSize);
		
		return EXIT_SUCCESS;
	}
	
	return _frpgaReadParamRegisters(runCfg);
}


char extFpgaConfigParams(EXT_RUNTIME_CFG *runCfg)
{
	unsigned char _chValue;

	if(EXT_IS_TX(runCfg))
	{
		return EXIT_FAILURE;
	}

	if(runCfg->fpgaAuto)
	{
		_chValue = EXT_FPGA_FLAGS_MCU_DISABLE;
		_extFpgaWriteByte(EXT_FPGA_REG_ENABLE, &_chValue);
	}
	else
	{
		/* first, enable manual configuration */
		_chValue = EXT_FPGA_FLAGS_MCU_ENABLE;
		_extFpgaWriteByte(EXT_FPGA_REG_ENABLE, &_chValue);
		
		/* then params */
		_extFpgaWriteShort(EXT_FPGA_REG_WIDTH, (unsigned char *)&runCfg->runtime.vWidth);
		
		_extFpgaWriteShort(EXT_FPGA_REG_HEIGHT, (unsigned char *)&runCfg->runtime.vHeight);
		
		_extFpgaWriteByte(EXT_FPGA_REG_FRAMERATE, &runCfg->runtime.vFrameRate);

		_extFpgaWriteByte(EXT_FPGA_REG_SAMPLING, &runCfg->runtime.vColorSpace);

		_extFpgaWriteByte(EXT_FPGA_REG_DEPTH, &runCfg->runtime.vDepth);

		_extFpgaWriteByte(EXT_FPGA_REG_INTLC_SEGM, &runCfg->runtime.vIsInterlaced);


		_extFpgaWriteByte(EXT_FPGA_REG_AUDIO_CHANNELS, &runCfg->runtime.aChannels);
		_extFpgaWriteByte(EXT_FPGA_REG_AUDIO_RATE, &runCfg->runtime.aSampleRate);
		
		_extFpgaWriteByte(EXT_FPGA_REG_AUDIO_PKT_SIZE, &runCfg->runtime.aPktSize);
		
		/* third, start it */
		_extFpgaWriteByte(EXT_FPGA_REG_PARAM_STATUS, &_chValue);
	}

	return EXIT_SUCCESS;
}

void	extFpgaEnable(char	isEnable)
{
	unsigned char		data = 0xFF;	/* enable all bits for different streams */
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

		_extFpgaReadByte(EXT_FPGA_REG_ETHERNET_RESET, &data);
		data = data||0x01;
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


