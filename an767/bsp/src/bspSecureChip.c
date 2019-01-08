
#include "compact.h"
#include "bsp.h"

#include "gpio.h"
#include "pio_handler.h"

int ComputeSHA256(unsigned char *message, short length, unsigned short skipconst, unsigned short reverse, unsigned char *digest);

// delay durations
#define EEPROM_WRITE_DELAY       10  	//tprd unit:ms
#define SHA_COMPUTATION_DELAY    3   	//unit:ms
#define SECRET_PROGRAM_DELAY	 100	//tprs unit:ms

#define	SC_CMD_READ_ROM						0x33	//Read rom ID
#define	SC_CMD_READ_WRITE_SCRATCHPAD		0x0F	//Read/write scratchpad 
#define	SC_CMD_ROM_SKIP						0xCC
#define	SC_CMD_WRITE_MEMORY					0x55	//Write to memory
#define	SC_CMD_READ_MEMORY					0xF0	
#define	SC_CMD_READ_STATUS					0xAA
#define	SC_CMD_COMPUTE_READ_PAGE_MAC		0xA5
#define	SC_CMD_LOAD_LOCK_SECRET				0x33 


#define	SC_SET_OUTPUT()	\
	gpio_configure_pin(EXT_PIN_SECURE_CHIP, PIO_OUTPUT_1| PIO_DEFAULT)


#define	SC_SET_INPUT()	\
	gpio_configure_pin(EXT_PIN_SECURE_CHIP, PIO_TYPE_PIO_INPUT )


#define	SC_OUTPUT_LOW()	\
		gpio_set_pin_low(EXT_PIN_SECURE_CHIP)

#define	SC_OUTPUT_HIGH()	\
		gpio_set_pin_high(EXT_PIN_SECURE_CHIP)

#define	SC_INPUT()	\
		pio_get_pin_value(EXT_PIN_SECURE_CHIP)

#define	SC_STRONG_PULL_UP_ENABLE() \
			SC_OUTPUT_HIGH()

#define	SC_STRONG_PULL_UP_DISABLE() \
			SC_SET_INPUT()


//delay_us(2); 
#define	SC_READ_16BIT(value)	\
	do{	(value) = 0;	(value) = _bspScReadByte();	\
	(value) = (value) + (_bspScReadByte() << 8);  (value) = ((~value) & 0xffff); }while(0)
	

static char _bspScReset(void)
{
	unsigned int u32value;
	int i;
	char ret = EXIT_FAILURE;
	
	SC_SET_OUTPUT();
	SC_OUTPUT_LOW();
	//set to low and keep it about 60us
//	writel_relaxed(0x02000000, pio_codr2);		
	delay_us(55);

	//set PC25 as input to read DS28E15 response
	SC_SET_INPUT();
//	writel_relaxed(0x00000000, pio_cfgr2);/* write */	
	delay_us(3);	//tMSP
	
	for(i = 0; i < 200; i++)
	{
		delay_us(1);
		u32value = SC_INPUT();
//		u32value = readl_relaxed(pio_pdsr2) & 0x02000000;			/* read */
		if(u32value == 0)
		{
			ret = EXIT_SUCCESS;							
			break;
		}		
	}

	delay_us(60*5);

	return ret;	
}

static void _bspScWriteByte(unsigned char data)
{
	unsigned char i;
	unsigned char temp;

	for(i = 0; i < 8; i ++)
	{
		delay_us(6);	//tREC, recovery time between write
		
		//set for PC25 as output, non security,	
		//writel_relaxed(0x00000100, pio_cfgr2);
		SC_SET_OUTPUT();
		
		temp = data >> i; 	//shift right		
		temp &= 0x01; 		//start from LSB				
		//all communication begins with the master start from pulling data line low write 0
//		writel_relaxed(0x02000000, pio_codr2);
		SC_OUTPUT_LOW();

		
//		delay_us(1);      	//tW1L 
		delay_ns(20);
	//	ndelay(100);
		if(temp)
		{			
			//set PC25 as input, default pull up resistor set signal to 1
//			writel_relaxed(0x00000000, pio_cfgr2);
			SC_SET_INPUT();
		}
		
		delay_us(10);//keep slot time
		
		//set PC25 as input
//		writel_relaxed(0x00000000, pio_cfgr2);
		SC_SET_INPUT();
    }

}

static unsigned char _bspScReadByte(void)
{
	unsigned char i, data = 0x0;
	unsigned int temp;

	for(i = 0; i < 8; i ++)
	{			
		delay_us(6-1);	//tREC, recovery time between write, 
		//set PC25 as output
		//init registers for PC25 as output, non security,	
//		writel_relaxed(0x00000100, pio_cfgr2);
		SC_SET_OUTPUT();
		
		//all communication begins with the master start from pulling data line low write 0		
//		writel_relaxed(0x02000000, pio_codr2);
		SC_OUTPUT_LOW();
		delay_us(1);      	//tRL     //		
		
		//set PC25 as input		
//		writel_relaxed(0x00000000, pio_cfgr2);
		SC_SET_INPUT();
		
//		delay_us(1);
		delay_ns(10);
		
//		temp = readl_relaxed(pio_pdsr2) & 0x02000000;	
		temp = SC_INPUT();
		if(temp)	//bit '1'
		{			
			data |= (0x1 << i);
		}
		
		delay_us(10-1);//tslot minimum is 13us
	}

	return data;
}

//compute rom_id crc8
static unsigned char __calcCrc8(unsigned char dataByte, unsigned char crc)
{
	int bit_mask = 0;
	unsigned char carry_check = 0, temp_data = 0;
	temp_data = dataByte;
	
	for ( bit_mask = 0; bit_mask <= 7; bit_mask ++)
	{
		dataByte = dataByte ^ crc;
		crc = crc / 2;
		temp_data = temp_data / 2;
		
		carry_check = dataByte & 0x01;
		if (carry_check)
		{
			crc = crc ^ 0x8C;
		}
		
		dataByte = temp_data;
	}
	
	return ( crc );
}


static char _bspScReadRomId(SC_CTRL *sc)
{
	int ret, i;
	unsigned char crc_tmp;
	
	memset(sc->romId, 0, sizeof(sc->romId));
	
	ret = _bspScReset();
	if(ret == EXIT_FAILURE)				
	{
		EXT_INFOF(("Secerity Chip reset fail"));
		return ret;
	}
	
	_bspScWriteByte(SC_CMD_READ_ROM);	
	for(i = 0; i < 8; i++)
	{
		sc->romId[i] = _bspScReadByte();
	}
	
	crc_tmp = 0;
	for(i = 0; i < 7; i++)
	{
		crc_tmp = __calcCrc8(sc->romId[i], crc_tmp);
	}
	
	if((crc_tmp == sc->romId[7]) && (sc->romId[0] == 0x17))//check device family code 0x17 for DS28E15
	{
		return EXIT_SUCCESS;
	}	
	else
	{
		EXT_ERRORF(("Read security chip rom ID error CRC:%02x: "EXT_NEW_LINE"\t'%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x",
			crc_tmp, sc->romId[0],sc->romId[1],sc->romId[2],sc->romId[3],sc->romId[4],sc->romId[5],sc->romId[6],sc->romId[7]) );
		return EXIT_FAILURE;
	}
}


static unsigned short CRC16;

// Calculate a new CRC16 from the input data shorteger.  Return the current
// CRC16 and also update the global variable CRC16.
static short oddparity[16] = { 0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0 };

static unsigned short docrc16(unsigned short data)
{
	data = (data ^ (CRC16 & 0xff)) & 0xff;
	CRC16 >>= 8;

	if (oddparity[data & 0xf] ^ oddparity[data >> 4])
		CRC16 ^= 0xc001;

	data <<= 6;
	CRC16  ^= data;
	data <<= 1;
	CRC16   ^= data;

	return CRC16;
}


/* Write the 256 bit(32 byte) data to scratchpad with CRC16 verification for DS28E15 */
static char __bspWriteScratchPad(unsigned char *data)
{
	int i;
	unsigned short read_crc;
	unsigned short cmd = SC_CMD_READ_WRITE_SCRATCHPAD;

	CRC16 = 0;
	//CMD code
	docrc16(cmd);
	////PB--parameter byte bitmap, BIT3-0:0000 write, 1111 read
	cmd = 0x0;
	docrc16(cmd);
	
	_bspScReset();
	
	//SKIP_ROM
	_bspScWriteByte(SC_CMD_ROM_SKIP);	
	_bspScWriteByte(SC_CMD_READ_WRITE_SCRATCHPAD);		
	_bspScWriteByte(0x0);	
	
	//first crc	
	SC_READ_16BIT(read_crc);
	if( CRC16 != read_crc )
	{
		EXT_ERRORF(("SC: write_scratch first CRC error! %04x != %04x", CRC16,  read_crc) );
		return EXIT_FAILURE;
	}
	EXT_DEBUGF(EXT_DBG_ON, ("WriteScratch CMD CRC: %04x:%04x", CRC16, read_crc));
		
	//data			
	for(i = 0; i < 32; i++ )
	{
		_bspScWriteByte(data[i]);	
	}
	
	SC_READ_16BIT(read_crc);
	
	CRC16 = 0;		
	for(i = 0; i < 32; i++ )
	{
		docrc16(data[i]);		
	}
	
	if( CRC16 != read_crc )	
	{
		EXT_ERRORF(("SC: write_scratch second CRC error : %04x !=%04x !", CRC16, read_crc) );
		return EXIT_FAILURE;
	}
	EXT_DEBUGF(EXT_DBG_ON, ("WriteScratch Data CRC: %04x:%04x", CRC16, read_crc));
	
	return EXIT_SUCCESS;
}

// Read the 256 bit(32 byte) data from scratchpad with CRC16 verification for DS28E15.  
// 'data'- data from scratchpad
//
// Return: 1 - read scratch verified
//         0 - read error 
/*
static int read_scratch_pad(unsigned char *data)
{
	int i;
	unsigned short read_crc;

	CRC16 = 0;
	//CMD code
	docrc16(CMD_READ_WRITE_SCRATCHPAD);
	////PB--parameter byte bitmap, BIT3-0:0000 write, 1111 read
	docrc16(0x0F);
	
	ds_reset();
	//SKIP_ROM
	ds_write_byte(CMD_ROM_SKIP);	
	ds_write_byte(CMD_READ_WRITE_SCRATCHPAD);		
	ds_write_byte(0x0F);	
	
	//first crc	
	read_crc = 0;
	read_crc = ds_read_byte();
	read_crc = read_crc + ( ds_read_byte() << 8);		
		
	if( CRC16 != ( (~read_crc) & 0xffff ) )
	{
		printk(KERN_ERR"read_scratch first CRC error!\n");
		return 0;
	}		
		
	read_crc = 0;
	//data			
	for(i = 0; i < 32; i++ )
		data[i] = ds_read_byte();		
	read_crc = ds_read_byte();
	read_crc = read_crc + ( ds_read_byte() << 8);		
	CRC16 = 0;		
	for(i = 0; i < 32; i++ )		
		docrc16(data[i]);		
	
	if( CRC16 != ( (~read_crc) & 0xffff ) )	
	{
		printk(KERN_ERR"read_scratch second CRC error!\n");
		return 0;
	}				
	return 1;
}
*/


/*
//  Write a 4 byte memory block. The block location is selected by the 
//  page number and offset blcok within the page. Multiple blocks can
//  be programmed without re-selecting the device using the continue flag. 
//  This function does not use the Authenticated Write operation. 
// 
//  Parameters
//     page - page number, 0 or 1
//	  seg_num seg number, 0 to 7
//     data - 4 byte buffer containing the data to write, B0 B1 B2 B3
//
//  Returns: TRUE - block written
//           FALSE - Failed to write block (no presence or invalid CRC16)
*/
static char _writeBlock(unsigned char page, unsigned char seg_num, unsigned char *data)
{
	int i;
	unsigned short read_crc;
	unsigned short cmd = SC_CMD_WRITE_MEMORY;
	unsigned char PB;

	if(page >= 2)
	{
		return EXIT_FAILURE;
	}
	if(seg_num > 7)
	{
		return EXIT_FAILURE;
	}
	
	PB = ((seg_num & 0x7) << 5) + (page & 0x1);
	
	CRC16 = 0;
	//CMD code
	docrc16(cmd);
	////PB--parameter byte bitmap, seg 0, page 0
	cmd = PB;
	docrc16(cmd);	//get first CRC 

	_bspScReset();
	
	//SKIP_ROM
	_bspScWriteByte(SC_CMD_ROM_SKIP);
	_bspScWriteByte(SC_CMD_WRITE_MEMORY);
	_bspScWriteByte(PB);	
	
	//first crc	
	SC_READ_16BIT(read_crc);
	if(CRC16 != read_crc )
	{
		EXT_ERRORF(("SC: write block first CRC error %04x != %04x on page#%d, block#%d", CRC16, read_crc, page, seg_num));
		return EXIT_FAILURE;
	}

	EXT_DEBUGF(EXT_DBG_ON, ("WriteBlock CMD CRC: %04x:%04x on Page#%d, block#%d", CRC16, read_crc, page, seg_num ));

	//write 4 byte data
	for(i = 0; i < 4; i++)
	{
		_bspScWriteByte(data[i]);
	}
	
	//second crc
	SC_READ_16BIT(read_crc);
	
	CRC16 = 0;
	for(i = 0; i < 4; i++)
	{
		docrc16(data[i]);
	}
	
	if(CRC16 != read_crc )
	{
		EXT_ERRORF(("SC: write_block second CRC error %04x != %04x on page#%d, block#%d", CRC16, read_crc, page, seg_num) );
		return EXIT_FAILURE;
	}
	EXT_DEBUGF(EXT_DBG_ON, ("WriteBlock Data CRC: %04x:%04x on Page#%d, block#%d", CRC16, read_crc, page, seg_num ));
	
	//send release
	_bspScWriteByte(0xAA);
	
	//strong pull up
//	enable_ds_pull_up();
	SC_STRONG_PULL_UP_ENABLE();
	delay_ms(EEPROM_WRITE_DELAY);
	SC_STRONG_PULL_UP_DISABLE();
//	disable_ds_pull_up();
	
	PB = _bspScReadByte();	
	if(PB != 0xAA)
	{
//		read_crc = _bspScReadByte();	
//		if(read_crc != 0xAA)
		{
			EXT_ERRORF(("SC: write_block CS error: %02x on page#%d, block#%d", PB, page, seg_num));
			return EXIT_FAILURE;	
		}
	}

	return EXIT_SUCCESS;
}

//write 32 bytes to one page
static char _bspScWritePage(unsigned char page_num, unsigned char *data)
{
	unsigned char block_data[4];
	int i;
/*	
	_bspScReset();
	delay_us(55);
	_bspScReset();
*/	
	for(i = 0; i < 8; i++)
	{
		memcpy(block_data, &data[i*4], 4);
		if(_writeBlock(page_num, i, block_data) == EXIT_FAILURE)
		{
//			EXT_ERRORF(("SC: Write page %d, block %d error!", page_num, i));
			return EXIT_FAILURE;
		}
	}
	
	return EXIT_SUCCESS;
}

#if 0
//read one page data, from seg0 to seg7, B0 to B3
int bspScReadPage(unsigned char page_num, unsigned char *data)
{
	int i;
	unsigned short read_crc;
	unsigned char PB;
	
	if(page_num > 1)
	{
		return EXIT_FAILURE;
	}

	PB = page_num & 0x1;
	
	CRC16 = 0;
	//CMD code
	docrc16(SC_CMD_READ_MEMORY);
	////PB--parameter byte bitmap, seg 0, page 0
	docrc16(PB);	//get first CRC 
	
	_bspScReset();
	//SKIP_ROM
	_bspScWriteByte(SC_CMD_ROM_SKIP);	
	_bspScWriteByte(SC_CMD_READ_MEMORY);		
	_bspScWriteByte(PB);	//PB seg 0, page 0
	
	//first crc	
	SC_READ_16BIT(read_crc);
	if(CRC16 != ((~read_crc) & 0xffff))
	{
		EXT_ERRORF(("SC: read_page first CRC error!"));
		return EXIT_FAILURE;
	}		
			
	//read one page data, 32 byte
	for(i = 0; i < 32; i++)
	{
		data[i] = _bspScReadByte();
	}
	
	//second crc
	SC_READ_16BIT(read_crc);
	CRC16 = 0;
	for(i = 0; i < 32; i++)
	{
		docrc16(data[i]);
	}
	if(CRC16 != ((~read_crc) & 0xffff))
	{
		EXT_ERRORF(("SC: read_page second CRC error!"));
		return EXIT_FAILURE;
	}
	
	return EXIT_SUCCESS;
}
#endif

static char read_status_personality(unsigned char *status)
{
	int i;
	unsigned short read_crc;
	unsigned short cmd = SC_CMD_READ_STATUS;
	
	CRC16 = 0;
	//CMD code
	docrc16(cmd);
	////PB--parameter byte bitmap, read personality 4 bytes
	cmd = 0xE0;
	docrc16(cmd);	
	
	_bspScReset();
	//SKIP_ROM
	_bspScWriteByte(SC_CMD_ROM_SKIP);	
	_bspScWriteByte(SC_CMD_READ_STATUS);		
	_bspScWriteByte(0xE0);	//PB seg 0, page 0
	
	//first crc	
	SC_READ_16BIT(read_crc);
		
	if(CRC16 != read_crc )
	{
		EXT_ERRORF(("CMD_READ_STATUS first CRC error %04x != %04x!", CRC16, read_crc) );
		return EXIT_FAILURE;
	}		
	EXT_DEBUGF(EXT_DBG_ON, ("Read Status CMD CRC: %04x:%04x", CRC16, read_crc));
			
	//read one page data, 32 byte
	for(i = 0; i < 4; i++)
	{
		status[i] = _bspScReadByte();	
	}
	
	SC_READ_16BIT(read_crc);
	
	//second crc
	CRC16 = 0;
	for(i = 0; i < 4; i++)
	{
		docrc16(status[i]);
	}
	
	if(CRC16 !=  read_crc )
	{
		EXT_ERRORF(("SC: CMD_READ_STATUS second CRC error %04x != %04x!", CRC16, read_crc) );
		return EXIT_FAILURE;
	}
	EXT_DEBUGF(EXT_DBG_ON, ("Read Status Data CRC: %04x:%04x", CRC16, read_crc));
	
	return EXIT_SUCCESS;
}


/*  Do Compute Page MAC command and return MAC. Optionally do anonymous mode (anon != 0). 
// 
//  Parameters
//     page_num - page number to read 0 - 16
//     challange - 32 byte buffer containing the challenge
//     mac - 32 byte buffer for page data read
//     anon - Flag to indicate Anonymous mode if (anon != 0)
//
//  Returns: TRUE - page read has correct MAC
//           FALSE - Failed to read page or incorrect MAC
*/
static char compute_read_page_mac(unsigned char page_num, unsigned char *challenge, unsigned char *mac, unsigned char anon)
{
	unsigned char buf[32];
	int i;
	unsigned short read_crc;
	unsigned short cmd = SC_CMD_COMPUTE_READ_PAGE_MAC;
	unsigned char PB;
	
	if(anon == 1)	//anonymous 
		PB = 0xE0;
	else
		PB = 0x00;	//use ROM ID
	
	PB |= (page_num & 0x1);
	
	// write the challenge 32 bytes to the scratchpad
	if(__bspWriteScratchPad(challenge) == EXIT_FAILURE)
	{
		EXT_ERRORF(("SC: compute_read_page_mac fail."));
		return EXIT_FAILURE;
	}
	
	CRC16 = 0;
	//CMD code
	docrc16(cmd);
	////PB--parameter byte bitmap, BIT3-0:0000 write, 1111 read
	cmd = PB;
	docrc16(cmd);
	
	_bspScReset();
	//SKIP_ROM
	_bspScWriteByte(SC_CMD_ROM_SKIP);	
	_bspScWriteByte(SC_CMD_COMPUTE_READ_PAGE_MAC);		
	_bspScWriteByte(PB);	
	
	//first crc	
	SC_READ_16BIT(read_crc);
		
	if( CRC16 !=  read_crc )
	{
		EXT_ERRORF(("SC: CMD_COMPUTE_READ_PAGE_MAC first CRC error %04x != %04x!", CRC16, read_crc) );
		return EXIT_FAILURE;
	}		
	EXT_DEBUGF(EXT_DBG_ON, ("Read MAC CMD CRC: %04x:%04x", CRC16, read_crc));

	//wait for 2*tcsha
	delay_ms(2*SHA_COMPUTATION_DELAY);
	
	//read cs
	read_crc = _bspScReadByte();
	if(read_crc != 0xAA)
	{
		EXT_ERRORF(("read cs error! 0x%x", read_crc));
		return EXIT_FAILURE;
	}
	
	//read 32 byte MAC 
	for(i = 0; i < 32; i++ )
	{
		buf[i] = _bspScReadByte();
	}
	
	SC_READ_16BIT(read_crc);
	
	CRC16 = 0;		
	for(i = 0; i < 32; i++ )
	{
		docrc16(buf[i]);
	}
	
	if(CRC16 != read_crc)
	{
		EXT_ERRORF(("SC: CMD_COMPUTE_READ_PAGE_MAC second CRC error %04x != %04x!", CRC16, read_crc) );
		return EXIT_FAILURE;
	}
	memcpy(mac, buf, 32);

	EXT_DEBUGF(EXT_DBG_ON, ("Read MAC Data CRC: %04x:%04x", CRC16, read_crc));
	
	return EXIT_SUCCESS;	
}

/*
// load and lock the 256 bit(32 byte) secret to chip
// 'secret'- secret to load
// 'lock_flag' - 1: lock , 0: no lock
// 'magic - 0x5A, protect lock_flag
*/
static char load_lock_secret(unsigned char *secret, unsigned char lock_flag, unsigned char lock_magic)
{
	unsigned short read_crc;	
	unsigned short cmd = SC_CMD_LOAD_LOCK_SECRET;	
	unsigned char PB;
	
	if(__bspWriteScratchPad(secret) == EXIT_FAILURE)
	{
		EXT_ERRORF(("SC: CMD_LOAD_LOCK_SECRET error!"));
		return EXIT_FAILURE;
	}	
		
	if((lock_flag == 1)&&(lock_magic == 0x5A))
	{
		PB = 0xE0;
	}
	else
	{
		PB = 0x00;
	}

	CRC16 = 0;
	//CMD code
	docrc16(cmd);
	////PB--parameter byte bitmap, BIT3-0:0000 write, 1111 read
	cmd = PB;
	docrc16(cmd);
	
	_bspScReset();
	//SKIP_ROM
	_bspScWriteByte(SC_CMD_ROM_SKIP);	
	_bspScWriteByte(SC_CMD_LOAD_LOCK_SECRET);		
	_bspScWriteByte(PB);
	
	//first crc	
	SC_READ_16BIT(read_crc);
	if( CRC16 != read_crc)
	{
		EXT_ERRORF(("SC: CMD_LOAD_LOCK_SECRET first CRC error %04x != %04x!", CRC16, read_crc) );
		return EXIT_FAILURE;
	}		
	EXT_DEBUGF(EXT_DBG_ON, ("Read Load Secret CMD CRC: %04x:%04x", CRC16, read_crc));
	
	//send release
	_bspScWriteByte(0xAA);
	//strong pull up
//	enable_ds_pull_up();
	SC_STRONG_PULL_UP_ENABLE();
	//delay tprs
	delay_ms(SECRET_PROGRAM_DELAY);
	SC_STRONG_PULL_UP_DISABLE();
	
//	disable_ds_pull_up();
	
	read_crc = _bspScReadByte();	
	if(read_crc == 0xAA)
	{
		return EXIT_SUCCESS;
	}
	else if(read_crc == 0x55)
	{
		EXT_ERRORF(("SC: CMD_LOAD_LOCK_SECRET secret was locked before."));
		return EXIT_FAILURE;
	}
	else
	{
		EXT_ERRORF(("SC: CMD_LOAD_LOCK_SECRET fail."));
		return EXIT_FAILURE;
	}			
}


static void _bspScCreateSecrect(SC_CTRL *sc)
{
	sc->secret[0] = (sc->romId[0] + 0x2A + 0x2) & 0xFF;
	sc->secret[1] = (sc->romId[1] + 0x4D + 0x2) & 0xFF;
	sc->secret[2] = (sc->romId[2] + 0x41 + 0x2) & 0xFF;
	sc->secret[3] = (sc->romId[3] + 0x52 + 0x2) & 0xFF;

	sc->secret[4] = (sc->romId[4] + 0x42 + 0x2) & 0xFF;
	sc->secret[5] = (sc->romId[5] + 0x4F + 0x2) & 0xFF;
	sc->secret[6] = (sc->romId[6] + 0x48 + 0x2) & 0xFF;
	sc->secret[7] = (sc->romId[7] + 0x4A + 0x2) & 0xFF;

	sc->secret[8]  = (sc->romId[0] + 0x4F + 0x4) & 0xFF;
	sc->secret[9]  = (sc->romId[1] + 0x43 + 0x4) & 0xFF;
	sc->secret[10] = (sc->romId[2] + 0x41 + 0x4) & 0xFF;
	sc->secret[11] = (sc->romId[3] + 0x55 + 0x4) & 0xFF;

	sc->secret[12] = (sc->romId[4] + 0x42 + 0x4) & 0xFF;
	sc->secret[13] = (sc->romId[5] + 0x50 + 0x4) & 0xFF;
	sc->secret[14] = (sc->romId[6] + 0x45 + 0x4) & 0xFF;
	sc->secret[15] = (sc->romId[7] + 0x54 + 0x4) & 0xFF;

	sc->secret[16] = (sc->romId[0] + 0x4A + 0x6) & 0xFF;	
	sc->secret[17] = (sc->romId[1] + 0x49 + 0x6) & 0xFF;
	sc->secret[18] = (sc->romId[2] + 0x41 + 0x6) & 0xFF;
	sc->secret[19] = (sc->romId[3] + 0x52 + 0x6) & 0xFF;

	sc->secret[20] = (sc->romId[4] + 0x4F + 0x6) & 0xFF;
	sc->secret[21] = (sc->romId[5] + 0x44 + 0x6) & 0xFF;
	sc->secret[22] = (sc->romId[6] + 0x46 + 0x6) & 0xFF;
	sc->secret[23] = (sc->romId[7] + 0x41 + 0x6) & 0xFF;

	sc->secret[24] = (sc->romId[0] + 0x4E + 0x8) & 0xFF;	
	sc->secret[25] = (sc->romId[1] + 0x53 + 0x8) & 0xFF;
	sc->secret[26] = (sc->romId[2] + 0x41 + 0x8) & 0xFF;
	sc->secret[27] = (sc->romId[3] + 0x4E + 0x8) & 0xFF;

	sc->secret[28] = (sc->romId[4] + 0x50 + 0x8) & 0xFF;
	sc->secret[29] = (sc->romId[5] + 0x41 + 0x8) & 0xFF;
	sc->secret[30] = (sc->romId[6] + 0x54 + 0x8) & 0xFF;
	sc->secret[31] = (sc->romId[7] + 0x2A + 0x8) & 0xFF;


}

static char _bspScCreatePage(SC_CTRL *sc)
{
	/* page structure */
	memset(sc->pageData, 0, sizeof(sc->pageData) );				
	memcpy(sc->pageData, sc->romId, 8);
	memcpy(sc->pageData + 8, EXT_767_MODEL, 6);
	memcpy(sc->pageData + 14, EXT_767_PRODUCT_NAME, 13);
	sc->pageData[27] = EXT_MAGIC_VALUE_A;
	sc->pageData[28] = EXT_MAGIC_VALUE_B;
	sc->pageData[29] = EXT_MAGIC_VALUE_A;
	sc->pageData[30] = EXT_MAGIC_VALUE_B;
	
	//write_page(0, page_data);
	delay_us(10);
	//write_page(1, page_data);
	_bspScReset();
	
	return _bspScWritePage(sc->pageNum, sc->pageData);
}


static char _bspScComputeMAC(SC_CTRL *sc)
{
	int i,j;
	uint8_t calc_mac[32];
	uint8_t tmp[4]; 
	uint8_t MT[128];
	
	// create buffer to compute and verify mac
	memset(MT,0,128);

	// insert page data
	memcpy(&MT[0], sc->pageData, 32);

	// insert challenge
	memcpy(&MT[32], sc->challenge,32);
   
	// insert secret
	memcpy(&MT[64], sc->secret, 32);
	// insert ROM number or FF
	if (sc->isAnon )
	{
		memset(&MT[96], 0xFF, 8);
	}
	else
	{
		memcpy(&MT[96], sc->romId, 8);
	}

	MT[106] = sc->pageNum;
	MT[105] = sc->manId[0];
	MT[104] = sc->manId[1];

	// change to little endian for A1 devices    
	for (i = 0; i < 108; i+=4)
	{
		for (j = 0; j < 4; j++)
		{
			tmp[3 - j] = MT[i + j];
		}

		for (j = 0; j < 4; j++)
		{
			MT[i + j] = tmp[j];
		}
	}

	ComputeSHA256(MT, 119, EXT_TRUE, EXT_TRUE, calc_mac);
	// Compare calculated mac with one read from device
	for (i = 0; i < 32; i++)
	{
		if (sc->readMac[i] != calc_mac[i])
		{
			return EXIT_FAILURE;
		}
	}
	
	return EXIT_SUCCESS; 
}


static char _bspScPrepare4Check(SC_CTRL *sc)
{
	unsigned char	personality_byte[4];
	
	if(_bspScCreatePage(sc) == EXIT_FAILURE)
	{
		return EXIT_FAILURE;
	}

	/* personality */
	if(read_status_personality(personality_byte) == EXIT_FAILURE)
	{
		return EXIT_FAILURE;
	}
	
	sc->manId[0] = personality_byte[2];
	sc->manId[1] = personality_byte[3];

	EXT_DEBUGF(EXT_DBG_ON, ("personality_byte: %02x %02x %02x %02x", 
		personality_byte[0], personality_byte[1], personality_byte[2], personality_byte[3]) );

	return EXIT_SUCCESS;
}


/* write to page 0 (seed or key) and test secret (rom ID) */
char bspScWriteKey(SC_CTRL *sc, unsigned char *key)	/* key : 32 bytes*/
{
	unsigned char lock_magic = 0x5A;
	unsigned char lock_flag = 0;
	
	if(!IS_SECURITY_CHIP_EXIST(sc))
	{
		return EXIT_FAILURE;
	}
	
	//write_page(0, page_data);
	delay_us(10);
	//write_page(1, page_data);
	_bspScReset();
	
	//load and lock secret		
	return load_lock_secret(key, lock_flag, lock_magic);
}


/* read Message Authen Code from chip, and check */
char bspScCheckMAC(SC_CTRL *sc)
{
	char ret = EXIT_FAILURE;

	if(!IS_SECURITY_CHIP_EXIST(sc))
	{
		return EXIT_FAILURE;
	}
	
	if(_bspScPrepare4Check(sc) == EXIT_FAILURE)
	{
		EXT_ERRORF(("SC: Prepare page for check failed!"));
//		return EXIT_FAILURE;
	}

	ret = compute_read_page_mac(sc->pageNum, sc->challenge, sc->readMac, sc->isAnon);
	if(ret == EXIT_FAILURE)
	{
		EXT_ERRORF(("SC: read MAC from chip failed!"));
		return ret;
	}

	ret = _bspScComputeMAC(sc);
	if(ret == EXIT_FAILURE)			
	{
		EXT_ERRORF(("SC: Security check failed!"));
	}

	return ret;
}

#define	SC_HW_TEST 		1

char	bspScInit(SC_CTRL *sc)
{
	char ret = EXIT_FAILURE;
	int i;
	
	ret = _bspScReadRomId(sc);
	if(ret == EXIT_FAILURE)
	{
		sc->isExist = EXT_FALSE;
		return ret;
	}
	sc->isExist = EXT_TRUE;

#if SC_HW_TEST
#define	_TEST_COUNT	2000

	int errs =0;

	printf(EXT_NEW_LINE"Begin %d times ROM ID tests..."EXT_NEW_LINE, _TEST_COUNT);
	for(i = 0; i < _TEST_COUNT; i++)
	{
		ret = _bspScReadRomId(sc);
		printf("%s", (ret == EXIT_FAILURE)?"'x":".");
		if(ret == EXIT_FAILURE)
		{
			errs++;
		}
	}
	printf(EXT_NEW_LINE"Error %d times in Total %d tests!"EXT_NEW_LINE, errs, _TEST_COUNT);
#endif

	_bspScCreateSecrect(sc);

	/* challenge*/
	bspHwTrngConfig(EXT_TRUE, RNG_MODE_SC_CHALLENGE);

	bspHwTrngWait();


	if(_bspScPrepare4Check(sc) == EXIT_FAILURE)
	{
		EXT_ERRORF(("SC: Prepare page for check failed!"));
		return EXIT_FAILURE;
	}

#if 1
	EXT_DEBUGF(EXT_DBG_ON, ("Rom ID: %02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x",
		sc->romId[0],sc->romId[1],sc->romId[2],sc->romId[3],sc->romId[4],sc->romId[5],sc->romId[6],sc->romId[7] ));

	printf("Secret key is : "EXT_NEW_LINE);
	for(i = 0; i < 32; i++)
	{
		printf("%02x ", sc->secret[i]);
	}
	
	printf(EXT_NEW_LINE"Challenge is : "EXT_NEW_LINE);
	for(i = 0; i < 32; i++)
	{
		printf("%02x ", sc->challenge[i]);
	}

	printf(EXT_NEW_LINE" "EXT_NEW_LINE);

#endif

	return EXIT_SUCCESS;
}

