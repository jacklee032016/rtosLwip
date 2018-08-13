

#include "compact.h"

#include <twihs.h>

/** EEPROM Wait Time */
#define WAIT_TIME   10


/* only EEPROM MAC has serial no; this address is the beginning of second half 256, eg.128 byte */
#define	EEPROM_MEM_SERIALNO_ADDR			0x80


static const uint8_t test_data_tx[] =
{
	/** SAM TWI EEPROM EXAMPLE */
	'M', 'u', 'x', 'L', 'a', 'b', '-', 'A',
	'N', '7', '6', '7', 'M', '-','O', 'K',
	'P', 'a', 'g', 'e', '-', 'N', 'o', ',',
	'2'
};

#define TEST_DATA_LENGTH  (sizeof(test_data_tx) / sizeof(uint8_t))

/* startAddress is at page border*/
void extEepromWrite(unsigned char startAddress, unsigned char *value, unsigned int size)
{
	unsigned int i;
	unsigned int pages = (size+EXT_I2C_EEPROM_PAGE_SIZE-1)/EXT_I2C_EEPROM_PAGE_SIZE;
	unsigned char left = EXT_I2C_EEPROM_PAGE_SIZE;
	unsigned char start = 0;
#if 0
	unsigned char *va = value;
	/* write start address to EEPROM */
	
	extI2CWrite(EXT_I2C_EEPROM_CHANNEL, EXT_I2C_EEPROM_ADDRESS, 0, 0, &startAddress, 1);

	EXT_DEBUGF("Write EEPROM start address :\tOK!\r\n");

	uint32_t timeout = TWIHS_TIMEOUT*1000;
	while(timeout>0)
	{
		timeout--;
	}
	extI2CRead(EXT_I2C_EEPROM_CHANNEL, EXT_I2C_EEPROM_ADDRESS, 0, 0, value, size);
#endif

	for(i=0; i< pages; i++)
	{/* the offset address is send together, don't send it independently */
		left = ((size - start)>EXT_I2C_EEPROM_PAGE_SIZE)?EXT_I2C_EEPROM_PAGE_SIZE:(size - start);
		if(i>0)
		{
			EXT_DELAY_US(1500);
		}
#if 0
		printf("EEPROM Write No.%d page at 0x%x, length %d"EXT_NEW_LINE, i, start, left );
#endif
		extI2CWrite(EXT_I2C_EEPROM_CHANNEL, EXT_I2C_EEPROM_ADDRESS, startAddress+start, 1, value+start, left);
		start += EXT_I2C_EEPROM_PAGE_SIZE;
	}
	
#ifdef BSP_BIST_DEBUG
	printf("EEPROM write at 0x%08X:"EXT_NEW_LINE, startAddress);
	CONSOLE_DEBUG_MEM(value, size, startAddress, "EEPROM write");
#endif

}



void extEepromRead(unsigned char startAddress, unsigned char *value, unsigned int size)
{
#if 0
	unsigned int i;
	unsigned char *va = value;
	/* write start address to EEPROM */
	
	extI2CWrite(EXT_I2C_EEPROM_CHANNEL, EXT_I2C_EEPROM_ADDRESS, 0, 0, &startAddress, 1);

	EXT_DEBUGF("Write EEPROM start address :\tOK!\r\n");

	uint32_t timeout = TWIHS_TIMEOUT*1000;
	while(timeout>0)
	{
		timeout--;
	}
	extI2CRead(EXT_I2C_EEPROM_CHANNEL, EXT_I2C_EEPROM_ADDRESS, 0, 0, value, size);
#endif

	extI2CRead(EXT_I2C_EEPROM_CHANNEL, EXT_I2C_EEPROM_ADDRESS, startAddress, 1, value, size);
	
#ifdef BSP_BIST_DEBUG
	printf("EEPROM Read at 0x%08X:"EXT_NEW_LINE, startAddress);
	CONSOLE_DEBUG_MEM(value, size, startAddress, "EEPROM Read");
#endif

}


	
static void _extI2cPca9554Config(uint8_t channel)
{
	twihs_packet_t packet;

	char datatx = channel;
	packet.chip        = EXT_I2C_PCA9554_ADDRESS;
	packet.addr_length = 0;
	packet.buffer      = (void *) &datatx;
	packet.length      = 1;

	/* Send test pattern to EEPROM */
	if (twihs_master_write(TWIHS0, &packet) != TWIHS_SUCCESS)
	{
		EXT_ABORT( "\tTWI master write packet failed" );
	}


#if 0//MUXLAB_VALIDATE_CHIP
/* test invalidate */
	char datarx = 0;
	/* Configure the data packet to be received */
	packet_rx.chip        = packet_tx.chip;
	packet.addr_length = 0;
	packet.buffer      = (void *)&datarx;
	packet.length      = 1;
	
	/* Wait at least 10 ms */
//	mdelay(WAIT_TIME);
	delay_ms(10);
	uint32_t timeout = TWIHS_TIMEOUT*1000;
	while(timeout>0)
	{
		timeout--;
	}

	/* Get memory from EEPROM*/
	if (twihs_master_read(TWIHS0, &packet) != TWIHS_SUCCESS)
	{
		EXT_ABORT("-E-\tTWI master read packet failed");
	}

	if(datarx != datatx)
	{
		EXT_ABORT("-E-\tData validate for PCA9554 failed");
	}
	printf("PCA9554 configure OK!\t:0x%x!\r\n", datarx);
#endif

}

#define I2C_ASSIGN_ADDRESS(packet, address, size)	\
	{if( (size) >= 1){ 	packet.addr[0] = (address&0xFF);} \
		if( (size) >= 2 ){	packet.addr[1] = (((address) >> 8)&0xFF); } \
		if( (size) >= 3) {	packet.addr[2] = (((address) >> 16)&0xFF); }	packet.addr_length = (size); }

#define	__BSP_DEBUG_I2C_NAME		1

#if __BSP_DEBUG_I2C_NAME
struct	extI2Cdevice
{
	const unsigned char		address;
	const char				*name;
};

static _CODE struct extI2Cdevice	_extI2cDevices[] =
{
	{
		address: EXT_I2C_ADDRESS_RTL8035,
		name: "RtkSwitch",				
	},
	{
		address: EXT_I2C_ADDRESS_LM1983,
		name: "ClockGenerator",				
	},
	{
		address: EXT_I2C_ADDRESS_SENSOR,
		name: "Sensor",				
	},
	{
		address: EXT_I2C_ADDRESS_FPGA,
		name: "FPGA",				
	},
	{
		address: EXT_I2C_PCA9554_ADDRESS,
		name: "Multiplexer",				
	},
	{
		address: EXT_I2C_PCA9554_CS_NONE,
		name: "UNKNOWN",				
	}
};

static const char *_extI2cDeviceName(char	address)
{
	_CODE struct extI2Cdevice *dev= _extI2cDevices;

	while(dev->address!= EXT_I2C_PCA9554_CS_NONE)
	{
		if(dev->address == address)
		{
			return dev->name;
		}

		dev++;
	}

	return dev->name; /*now it is "UNKNONW" */
}
#endif

char extI2CRead(unsigned char chanNo, unsigned char deviceAddress, unsigned int regAddress, unsigned char regAddressSize, unsigned char *regVal, unsigned char regSize)
{
	int twiStatus;
	twihs_packet_t	packet;

#if EXTLAB_BOARD
	if(chanNo != EXT_I2C_PCA9554_CS_NONE)
	{
		_extI2cPca9554Config(chanNo);
	}
	else
		_extI2cPca9554Config(0);

#endif

	/* Configure the data packet to be received */
	packet.chip        = deviceAddress;

#if 0
	if(regAddressSize >= 1)
	{
		packet.addr[0] = regAddress;
	}
	if(regAddressSize >= 2 )
	{
		packet.addr[1] = regAddress >> 8;
	}
	if(regAddressSize >= 3)
	{
		packet.addr[2] = regAddress >> 16;
	}	
	packet.addr[0]     = regAddress;
	packet.addr[1]     = regAddress >> 8;
	packet.addr[2]     = regAddress >> 16;
	
	packet.addr_length = regAddressSize;
#else
	I2C_ASSIGN_ADDRESS(packet, regAddress, regAddressSize);
#endif

	packet.buffer = (void *)regVal;
	packet.length = regSize;

	twiStatus= twihs_master_read(TWIHS0, &packet);
	if (twiStatus  != TWIHS_SUCCESS)
	{
//		EXT_ABORT("TWIHS read channel %d, device %x, regAdd:%x, AddSize:%d failed: %d", chanNo, deviceAddress, regAddress, regAddressSize, twiStatus);
#if __BSP_DEBUG_I2C_NAME
		EXT_ERRORF(( "I2C read channel %d, device %s, regAdd:%x, AddSize:%d failed: %d", chanNo, _extI2cDeviceName(deviceAddress), regAddress, regAddressSize, twiStatus));
#else
		EXT_ERRORF(( "I2C read channel %d, device %x, regAdd:%x, AddSize:%d failed: %d", chanNo, deviceAddress, regAddress, regAddressSize, twiStatus));
#endif
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

char extI2CWrite(unsigned char chanNo, unsigned char deviceAddress, unsigned int regAddress, unsigned char regAddressSize,  unsigned char *regVal, unsigned char regSize)
{
	int twiStatus;
	twihs_packet_t packet;

#if EXTLAB_BOARD
	if(chanNo != EXT_I2C_PCA9554_CS_NONE)
	{
		_extI2cPca9554Config(chanNo);
	}
	else
		_extI2cPca9554Config(0);
#endif

	/* Configure the data packet to be transmitted */
	packet.chip        = deviceAddress;

#if 0
	if(regAddressSize >= 1)
	{
		packet.addr[0] = regAddress;
	}
	if(regAddressSize >= 2 )
	{
		packet.addr[1] = regAddress >> 8;
	}
	if(regAddressSize >= 3)
	{
		packet.addr[2] = regAddress >> 16;
	}	
	packet.addr_length = regAddressSize;
#else
	I2C_ASSIGN_ADDRESS(packet, regAddress, regAddressSize);
#endif

	packet.buffer = (void *)regVal;
	packet.length = regSize;

	twiStatus = twihs_master_write(TWIHS0, &packet);
	if ( twiStatus != TWIHS_SUCCESS)
	{
#if __BSP_DEBUG_I2C_NAME
		EXT_ERRORF(("I2C write channel %d, device %s, regAdd:0x%x, regSize:%d, dataSize:%d failed: %d", 
			chanNo, _extI2cDeviceName(deviceAddress), regAddress, regAddressSize, regSize, twiStatus));
#else
		EXT_ERRORF(("I2C write channel %d, device %x, regAdd:0x%x, regSize:%d, dataSize:%d failed: %d", 
			chanNo, deviceAddress, regAddress, regAddressSize, regSize, twiStatus));
#endif
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS; 
}


/*
* temperature in Celsius from LM95245 on I2C bus, byte[1] is degree, byte[0] is k*0.125
*/
short extSensorGetTemperatureCelsius(void)
{/* 11 bits: sign bit + 10 bit value */
	unsigned char regVal;
	short temperature = 0;
    
	extI2CRead(EXT_I2C_PCA9554_CS_SENSOR, EXT_I2C_ADDRESS_SENSOR, EXT_I2C_SENSOR_LOCAL_TEMP_MSB, 1, &regVal, 1);

//	EXT_DEBUGF("LM95245 Temp(MSB): %d", regVal);
	temperature = (regVal<<8);
	extI2CRead(EXT_I2C_PCA9554_CS_SENSOR, EXT_I2C_ADDRESS_SENSOR, EXT_I2C_SENSOR_LOCAL_TEMP_LSB, 1, &regVal, 1);

	/* only 3 bits in LSB register */
	temperature |=  (regVal>>5); //*0.125;// / 256.0;
      
//	EXT_DEBUGF("LM95245 Temp(LSB): %d*0.125;", (regVal>>5) );
	return temperature;
}


/*

*/
char bspHwI2cInit(void)
{
	twihs_options_t opt;

	/* TWIHS0 */
	ioport_set_pin_peripheral_mode(TWIHS0_DATA_GPIO, TWIHS0_DATA_FLAGS);
	ioport_set_pin_peripheral_mode(TWIHS0_CLK_GPIO, TWIHS0_CLK_FLAGS);


	/* Enable the peripheral clock for TWI */
	pmc_enable_periph_clk(ID_TWIHS0);

	/* Configure the options of TWI driver */
	opt.master_clk = sysclk_get_peripheral_hz();
	opt.speed      = BOARD_TWIHS_CLK;

//	printf("Speed:%u"EXT_NEW_LINE, opt.speed);
	if (twihs_master_init(TWIHS0, &opt) != TWIHS_SUCCESS)
	{
		EXT_ERRORF(("TWI master initialization failed"));
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}



#if EXTLAB_BOARD
#else
/* only AT24MAC402 (mainly store MAC/EUI address) has serial no; other 2 types used in 767 has no serial number*/
static char _extE2PromMACSerialNo(void)
{
	int i;
#if 0
	twihs_packet_t packet;

	/* read serial no from EEPROM AT24MAC402, address of serial no is send as data */
	datatx[0] = EEPROM_MEM_SERIALNO_ADDR;
	packet.chip        = AT24MAC402_SERIALNO_ADDRESS;
	packet.addr[0]     = 0;
	packet.addr_length = 0;
	packet.buffer = datatx;
	packet.length = 1;

	if (twihs_master_write(TWIHS0, &packet) != TWIHS_SUCCESS)
	{
		EXT_ABORT("\tTWI master dump write for serial no failed.\r");
	}
#else

	unsigned char	serialNo[16];
	unsigned char regVal = EEPROM_MEM_SERIALNO_ADDR;
	extI2CWrite(EXT_I2C_EEPROM_CHANNEL, EXT_I2C_EEPROM_ADDRESS, 0, 0, &regVal, 1);
#endif

	printf("Write EEPROM serial no. address :\tOK!\r\n");

	uint32_t timeout = TWIHS_TIMEOUT*1000;
	while(timeout>0)
	{
		timeout--;
	}
#if 0
	packet.buffer = datatx;
	packet.length = 16;
	if (twihs_master_read(TWIHS0, &packet) != TWIHS_SUCCESS)
	{
		EXT_ABORT("-E-\tTWI master read serial-no packet failed.\r");
	}
	printf("Read:\tOK! AT24MAC40 SerialNo:\r\n");
	for (i = 0; i < 16; i++)
	{
		printf("%x ", datatx[i]);
	}
	printf("\r\n");
#else	
	extI2CRead(EXT_I2C_EEPROM_CHANNEL, EXT_I2C_EEPROM_ADDRESS, 0, 0, serialNo, 16);
	
	printf("Read:\tOK! EEPROM SerialNo:\r\n");
	for (i = 0; i < 16; i++)
	{
		printf("%c ", serialNo[i]);
	}
	printf("\r\n");

	return EXIT_SUCCESS;
#endif

}
#endif

char bspBistI2cEeprom(char *outBuffer, size_t bufferSzie )
{
	uint8_t datarx[TEST_DATA_LENGTH];
	unsigned char address = 0;
	unsigned int size = TEST_DATA_LENGTH;
	unsigned int i;

#if EXTLAB_BOARD
#else
	if(_extE2PromMACSerialNo() == EXIT_FAILURE)
	{
		printf("ERROR: Read EEPROM serial number"EXT_NEW_LINE);
		return EXIT_FAILURE;
	}
#endif

	//	extEepromRead(0, datarx, TEST_DATA_LENGTH);
	
	/* write and readback 25 bytes, acrossing 2 pages in EEPROM */
	extEepromWrite(address,(unsigned char *) test_data_tx, size);

	extEepromRead(address, datarx, size);
	
//	CONSOLE_DEBUG_MEM((uint8_t *)test_data_tx, size, address, "data tx");
//	CONSOLE_DEBUG_MEM(datarx, size, address, "data rx");
	for(i=0; i< size; i++)
	{
		if(datarx[i] != test_data_tx[i])
		{
			printf("ERROR: No.%d bytes is different, %x!=%x"EXT_NEW_LINE, (int)i, datarx[i],  test_data_tx[i]);
			return EXIT_FAILURE;
		}
	}

	return EXIT_SUCCESS;
}


char  bspBistI2cSensor(char *outBuffer, size_t bufferSize)
{
	unsigned char revisionId = 0;
	unsigned char	manufactureId = 0;
	short temp;
	int index = 0;
	
	outBuffer[ 0 ] = 0x00;

	extI2CRead(EXT_I2C_PCA9554_CS_SENSOR, EXT_I2C_ADDRESS_SENSOR, EXT_I2C_SENSOR_MANUFACTURE_ID, 1, &manufactureId, 1);
	extI2CRead(EXT_I2C_PCA9554_CS_SENSOR, EXT_I2C_ADDRESS_SENSOR, EXT_I2C_SENSOR_REVISION_ID, 1, &revisionId, 1);

	index += snprintf( outBuffer+index, (bufferSize-index), "\t\tSensor: Manu ID:'0x%x'; RevisionID:'0x%x': %s"EXT_NEW_LINE, manufactureId, revisionId, (revisionId== 0xB3 && manufactureId==0x01)?"OK":"Error" );
	if(revisionId!= 0xB3 || manufactureId!=0x01)
	{
		return EXIT_FAILURE;
	}
	
#if MUXLAB_VALIDATE_CHIP
	temp = extSensorGetTemperatureCelsius();
//	EXT_INFOF(("LM95245 OK: ManufactureID:%x;RevisionID:%x; Current temperature:%d+%d*0.125", manufactureId, revisionId, (temp>>8), (temp&0xFF) ));
	index += snprintf( outBuffer+index, (bufferSize-index), "\t\tCurrent temperature:%d+%d*0.125"EXT_NEW_LINE, (temp>>8), (temp&0xFF) );
#endif

	return EXIT_SUCCESS;
}



