
#include "compact.h"
#include "bsp.h"

#include "gpio.h"
#include "pio_handler.h"


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


#define	SC_READ_16BIT(value)	\
	do{	(value) = 0;	 (value) = _bspScReadByte();	\
	(value) = (value) + (_bspScReadByte() << 8);  }while(1)
	


typedef struct
{
	unsigned char		romId[8];	
	unsigned char		secret[8];			
	unsigned char		challenge[32];
	unsigned char		pageData[32];		//memory page data, 32 byte
	unsigned char 	personalityByte[4];//
	unsigned char		readMac[32];		//MAC from chip
	unsigned char 	pageNum;			//page number

	unsigned char		anonFlag;				//anonymous flag. 1--anonymous mode, 0-NOT anonymous mode
}SC_CTRL;

SC_CTRL		chip_ds_msg;

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

#if 0	
	iounmap(s_pio_sionr2);
	iounmap(pio_msk2);
	iounmap(pio_cfgr2);
	iounmap(pio_codr2);
	iounmap(pio_pdsr2);
	
	udelay(60);			//trsth min is 48us
#endif

	delay_us(60);

	return ret;	
}

static void _bspScWriteByte(unsigned char data)
{
	unsigned char i;
	unsigned char temp;

#if 0
	void *s_pio_sionr2;	
	void *pio_msk2;	
	void *pio_cfgr2;		
	void *pio_codr2;		
	void *pio_pdsr2;
	
	s_pio_sionr2 = ioremap(S_PIO_SIONR2, 4);	
	pio_msk2  = ioremap(PIO_MSKR2, 4);	
	pio_cfgr2 = ioremap(PIO_CFGR2, 4);	
	pio_codr2 = ioremap(PIO_CODR2, 4);	
	pio_pdsr2 = ioremap(PIO_PDSR2, 4);
	
	writel_relaxed(0x02000000, s_pio_sionr2);
	writel_relaxed(0x02000000, pio_msk2);
	//avoid glitch
	writel_relaxed(0x02000000, pio_codr2);
	//to protect the ds critical section
	spin_lock_irqsave(&ds_lock, ds_spin_flags);
#endif

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
		
		delay_us(1);      	//tW1L 
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

#if 0	
	spin_unlock_irqrestore(&ds_lock, ds_spin_flags);			  	
	iounmap(s_pio_sionr2);
	iounmap(pio_msk2);
	iounmap(pio_cfgr2);
	iounmap(pio_codr2);
	iounmap(pio_pdsr2);
#endif

}

static unsigned char _bspScReadByte(void)
{
	unsigned char i, data = 0x0;
	unsigned int temp;

#if 0	
	void *s_pio_sionr2;	
	void *pio_msk2;	
	void *pio_cfgr2;	
	void *pio_codr2;		
	void *pio_pdsr2;
	
	s_pio_sionr2 = ioremap(S_PIO_SIONR2, 4);	
	pio_msk2 = ioremap(PIO_MSKR2, 4);	
	pio_cfgr2 = ioremap(PIO_CFGR2, 4);	
	pio_codr2 = ioremap(PIO_CODR2, 4);	
	pio_pdsr2 = ioremap(PIO_PDSR2, 4);
	
	writel_relaxed(0x02000000, s_pio_sionr2);
	writel_relaxed(0x02000000, pio_msk2);
	//avoid glitch
	writel_relaxed(0x02000000, pio_codr2);
	//to protect the ds critical section
	spin_lock_irqsave(&ds_lock, ds_spin_flags);
#endif

	for(i = 0; i < 8; i ++)
	{			
		delay_us(6);	//tREC, recovery time between write, 
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
		
		delay_us(1);
		
//		temp = readl_relaxed(pio_pdsr2) & 0x02000000;	
		temp = SC_INPUT();
		if(temp)	//bit '1'
		{			
			data |= (0x1 << i);
		}
		
		delay_us(10);//tslot minimum is 13us
	}

#if 0	
	spin_unlock_irqrestore(&ds_lock, ds_spin_flags);	
	iounmap(s_pio_sionr2);
	iounmap(pio_msk2);
	iounmap(pio_cfgr2);
	iounmap(pio_codr2);
	iounmap(pio_pdsr2);	
#endif

	return data;
}

//compute rom_id crc8
int calc_rom_id_crc(int dataByte, int crc)
{
	int bit_mask = 0, carry_check = 0, temp_data = 0;
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


char bspScReadRomId(void)
{
	int ret, i;
	int crc_tmp, data_byte;
	unsigned char rom_id[8];
	
	TRACE();
	memset(rom_id, 0, sizeof(rom_id));
	
	ret = _bspScReset();
	if(ret == EXIT_FAILURE)				
	{
		EXT_INFOF(("Secerity Chip reset fail"));
		return ret;
	}
	
	_bspScWriteByte(SC_CMD_READ_ROM);	
	for(i = 0; i < 8; i++)
	{
		rom_id[i] = _bspScReadByte();
	}
	EXT_DEBUGF(EXT_DBG_ON, ("Rom ID: 0x%2x:0x%2x:0x%2x:0x%2x:0x%2x:0x%2x:0x%2x:0x%2x:",
		rom_id[0],rom_id[1],rom_id[2],rom_id[3],rom_id[4],rom_id[5],rom_id[6],rom_id[7] ));
	
	crc_tmp = 0;
	for(i = 0; i < 7; i++)
	{
		data_byte = rom_id[i];	
		crc_tmp = calc_rom_id_crc(data_byte, crc_tmp);
	}
	
	if((crc_tmp == rom_id[7]) && (rom_id[0] == 0x17))//check device family code 0x17 for DS28E15
	{
		return EXIT_SUCCESS;
	}	
	else
	{
		EXT_ERRORF(("Read security chip rom ID error!"));
		return EXIT_FAILURE;
	}
}


static unsigned short CRC16;

//--------------------------------------------------------------------------
// Calculate a new CRC16 from the input data shorteger.  Return the current
// CRC16 and also update the global variable CRC16.
//
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


/*
* Write the 256 bit(32 byte) data to scratchpad with CRC16 verification for DS28E15.  
* 'data'- data to write
* Return: 1 - write to scratch verified; 0 - error writing scratch
*/
static int write_scratch_pad(unsigned char *data)
{
	int i;
	unsigned short read_crc;	

	CRC16 = 0;
	//CMD code
	docrc16(SC_CMD_READ_WRITE_SCRATCHPAD);
	////PB--parameter byte bitmap, BIT3-0:0000 write, 1111 read
	docrc16(0x0);
	
	_bspScReset();
	
	//SKIP_ROM
	_bspScWriteByte(SC_CMD_ROM_SKIP);	
	_bspScWriteByte(SC_CMD_READ_WRITE_SCRATCHPAD);		
	_bspScWriteByte(0x0);	
	
	//first crc	
	SC_READ_16BIT(read_crc);
		
	if( CRC16 != ( (~read_crc) & 0xffff ) )
	{
		EXT_ERRORF(("SC: write_scratch first CRC error!"));
		return EXIT_FAILURE;
	}		
		
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
	
	if( CRC16 != ( (~read_crc) & 0xffff ) )	
	{
		EXT_ERRORF(("SC: write_scratch second CRC error!"));
		return EXIT_FAILURE;
	}
	
	return EXIT_SUCCESS;
}

//----------------------------------------------------------------------
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
//	   seg_num seg number, 0 to 7
//     data - 4 byte buffer containing the data to write, B0 B1 B2 B3
//
//  Returns: TRUE - block written
//           FALSE - Failed to write block (no presence or invalid CRC16)
*/
char write_block(unsigned char page, unsigned char seg_num, unsigned char *data)
{
	int i;
	unsigned short read_crc;
	unsigned char PB;

	if(page >= 2)
		return EXIT_FAILURE;
	if(seg_num > 7)
		return EXIT_FAILURE;
	
	PB = ((seg_num & 0x7) << 5) + (page & 0x1);
	CRC16 = 0;
	//CMD code
	docrc16(SC_CMD_WRITE_MEMORY);
	////PB--parameter byte bitmap, seg 0, page 0
	docrc16(PB);	//get first CRC 
	
	_bspScReset();
	//SKIP_ROM
	_bspScWriteByte(SC_CMD_ROM_SKIP);	
	_bspScWriteByte(SC_CMD_WRITE_MEMORY);		
	_bspScWriteByte(PB);	
	
	//first crc	
	SC_READ_16BIT(read_crc);
		
	if(CRC16 != ((~read_crc) & 0xffff))
	{
		EXT_ERRORF(("SC: write_block first CRC error! page = %d, block = %d", page, seg_num));
		return EXIT_FAILURE;
	}		
		
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
	
	if(CRC16 != ((~read_crc) & 0xffff))
	{
		EXT_ERRORF(("SC: write_block second CRC error! page = %d, block = %d", page, seg_num) );
		return EXIT_FAILURE;
	}
	
	//send release
	_bspScWriteByte(0xAA);
	
	//strong pull up
	enable_ds_pull_up();
	delay_ms(EEPROM_WRITE_DELAY);	
	disable_ds_pull_up();
	
	read_crc = _bspScReadByte();	
	if(read_crc != 0xAA)
	{
		EXT_ERRORF(("SC: write_block CS error!"));
		return EXIT_FAILURE;	
	}

	return EXIT_SUCCESS;
}

//write 32 bytes to one page
char write_page(unsigned char page_num, unsigned char *data)
{
	unsigned char block_data[4];
	int i;
	for(i = 0; i < 8; i++)
	{
		memcpy(block_data, &data[i*4], 4);
		if(write_block(page_num, i, block_data) == EXIT_FAILURE)
		{
			EXT_ERRORF(("SC: Write page %d, block %d error!", page_num, i));
			return EXIT_FAILURE;
		}
	}
	
	return EXIT_SUCCESS;
}

//read one page data, from seg0 to seg7, B0 to B3
int read_page(unsigned char page_num, unsigned char *data)
{
	int i;
	unsigned short read_crc;
	unsigned char PB;
	
	if(page_num > 1)
		return EXIT_FAILURE;

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
		EXT_ERRORF(("read_page second CRC error!"));
		return EXIT_FAILURE;
	}
	
	return EXIT_SUCCESS;
}


static int read_status_personality(unsigned char *status)
{
	int i;
	unsigned short read_crc;
	
	CRC16 = 0;
	//CMD code
	docrc16(SC_CMD_READ_STATUS);
	////PB--parameter byte bitmap, read personality 4 bytes
	docrc16(0xE0);	
	
	_bspScReset();
	//SKIP_ROM
	_bspScWriteByte(SC_CMD_ROM_SKIP);	
	_bspScWriteByte(SC_CMD_READ_STATUS);		
	_bspScWriteByte(0xE0);	//PB seg 0, page 0
	
	//first crc	
	SC_READ_16BIT(read_crc);
		
	if(CRC16 != ((~read_crc) & 0xffff))
	{
		EXT_ERRORF(("CMD_READ_STATUS first CRC error!"));
		return EXIT_FAILURE;
	}		
			
	//read one page data, 32 byte
	for(i = 0; i < 4; i++)
	{
		status[i] = _bspScReadByte();	
	}
	
	//second crc
	SC_READ_16BIT(read_crc);
	
	CRC16 = 0;
	for(i = 0; i < 4; i++)
	{
		docrc16(status[i]);
	}
	if(CRC16 != ((~read_crc) & 0xffff))
	{
		EXT_ERRORF(("SC: CMD_READ_STATUS second CRC error!"));
		return EXIT_FAILURE;
	}
	
	return EXIT_SUCCESS;
}


/*  Do Compute Page MAC command and return MAC. Optionally do
//  anonymous mode (anon != 0). 
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
int compute_read_page_mac(unsigned char page_num, unsigned char *challenge, unsigned char *mac, unsigned char anon)
{
	unsigned char buf[32];
	int i;
	unsigned short read_crc;	
	unsigned char PB;
	
	if(anon == 1)	//anonymous 
		PB = 0xE0;
	else
		PB = 0x00;	//use ROM ID
	
	PB |= (page_num & 0x1);
	
	// write the challenge 32 bytes to the scratchpad
	if(!write_scratch_pad(challenge))
	{
		EXT_ERRORF(("SC: compute_read_page_mac write_scratch_pad fail."));
		return EXIT_FAILURE;
	}
	
	CRC16 = 0;
	//CMD code
	docrc16(SC_CMD_COMPUTE_READ_PAGE_MAC);
	////PB--parameter byte bitmap, BIT3-0:0000 write, 1111 read
	docrc16(PB);
	
	_bspScReset();
	//SKIP_ROM
	_bspScWriteByte(SC_CMD_ROM_SKIP);	
	_bspScWriteByte(SC_CMD_COMPUTE_READ_PAGE_MAC);		
	_bspScWriteByte(PB);	
	
	//first crc	
	SC_READ_16BIT(read_crc);
		
	if( CRC16 != ( (~read_crc) & 0xffff ) )
	{
		EXT_ERRORF(("SC: CMD_COMPUTE_READ_PAGE_MAC first CRC error!"));
		return EXIT_FAILURE;
	}		

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
	
	if(CRC16 != ((~read_crc) & 0xffff ))	
	{
		EXT_ERRORF(("SC: CMD_COMPUTE_READ_PAGE_MAC second CRC error!"));
		return EXIT_FAILURE;
	}
	else
		memcpy(mac, buf, 32);
	
	return EXIT_SUCCESS;	
}

/*
// load and lock the 256 bit(32 byte) secret to chip
// 'secret'- secret to load
// 'lock_flag' - 1: lock , 0: no lock
// 'magic - 0x5A, protect lock_flag
//
// Return: 1 - load&lock secret ok
//         0 - error load&lock secret
*/
static int load_lock_secret(unsigned char *secret, unsigned lock_flag, unsigned char lock_magic)
{
	unsigned short read_crc;	
	unsigned char PB;
	
	if(!write_scratch_pad(secret))
	{
		EXT_ERRORF(("CMD_LOAD_LOCK_SECRET write_scratch_pad error!"));
		return EXIT_FAILURE;
	}	
		
	if((lock_flag == 1)&&(lock_magic == 0x5A))
		PB = 0xE0;
	else
		PB = 0x00;

	CRC16 = 0;
	//CMD code
	docrc16(SC_CMD_LOAD_LOCK_SECRET);
	////PB--parameter byte bitmap, BIT3-0:0000 write, 1111 read
	docrc16(PB);
	
	_bspScReset();
	//SKIP_ROM
	_bspScWriteByte(SC_CMD_ROM_SKIP);	
	_bspScWriteByte(SC_CMD_LOAD_LOCK_SECRET);		
	_bspScWriteByte(PB);
	
	//first crc	
	SC_READ_16BIT(read_crc);
		
	if( CRC16 != ( (~read_crc) & 0xffff ) )
	{
		EXT_ERRORF(("SC: CMD_LOAD_LOCK_SECRET first CRC error!"));
		return EXIT_FAILURE;
	}		
	
	//send release
	_bspScWriteByte(0xAA);
	//strong pull up
	enable_ds_pull_up();				
	//delay tprs
	delay_ms(SECRET_PROGRAM_DELAY);
	
	disable_ds_pull_up();
	
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

void initKey()
{
	unsigned char	rom_id[8];	
	unsigned char	secret[8];			
	unsigned char 	challenge[32];
	unsigned char	page_data[32];		//memory page data, 32 byte
	unsigned char 	personality_byte[4];//
	unsigned char   read_mac[32];		//MAC from chip
	unsigned char 	page_num;			//page number
	unsigned char 	lock_flag;			//1--lock chip secret forever
	unsigned char 	lock_magic;				
	unsigned char   anon;				//anonymous flag. 1--anonymous mode, 0-NOT anonymous mode
	int i;
	int ret;

				ret = copy_from_user(&chip_ds_msg, (struct ds_msg __user *)arg, sizeof(chip_ds_msg));
				if(ret != 0)			
				{
					printk(KERN_ERR"INIT_CHIP_PRODUCTION_KEY copy from user error! ret = %d\n", ret);
					return -EACCES;				
				}				
				ds_read_rom_id(rom_id);
				//ds_reset();
				memset(page_data, 0, sizeof(page_data));				
				//write_page(0, page_data);
				udelay(10);
				//write_page(1, page_data);
				ds_reset();
				memcpy(page_data, &chip_ds_msg.page_0_data[0], 32);				
				if(!write_page(0, page_data))
				{
					printk(KERN_ERR"INIT_CHIP_PRODUCTION_KEY Write page 0 data to chip FAIL!\n");
					return -EACCES;						
				}					
				
				//write page 1 data to chip, page 1 data has MAC address inside, it is written in PRODUCTION_MAC command
				//memcpy(page_data, &chip_ds_msg.page_1_data[0], 32);
				//if(!write_page(1, page_data))
				//{
				//	printk(KERN_ERR"INIT_CHIP_PRODUCTION Write page data to chip FAIL!\n");
				//	return -EACCES;						
				//}	
				
				//load and lock secret		
				lock_flag = chip_ds_msg.lock_flag;
				lock_magic = chip_ds_msg.lock_magic;
				memcpy(secret, &chip_ds_msg.secret[0], 32);				
				if(!load_lock_secret(secret, lock_flag, lock_magic))
				{
					printk(KERN_ERR"INIT_CHIP_PRODUCTION_KEY load lock secret to chip FAIL!\n");
					return -EACCES;						
				}												

}


void initMac()
{
	unsigned char	rom_id[8];	
	unsigned char	secret[8];			
	unsigned char 	challenge[32];
	unsigned char	page_data[32];		//memory page data, 32 byte
	unsigned char 	personality_byte[4];//
	unsigned char   read_mac[32];		//MAC from chip
	unsigned char 	page_num;			//page number
	unsigned char 	lock_flag;			//1--lock chip secret forever
	unsigned char 	lock_magic;				
	unsigned char   anon;				//anonymous flag. 1--anonymous mode, 0-NOT anonymous mode
	int i;
	int ret;

				ret = copy_from_user(&chip_ds_msg, (struct ds_msg __user *)arg, sizeof(chip_ds_msg));
				if(ret != 0)			
				{
					printk(KERN_ERR"INIT_CHIP_PRODUCTION_MAC copy from user error! ret = %d\n", ret);
					return -EACCES;				
				}				
				ds_read_rom_id(rom_id);
				//ds_reset();
				memset(page_data, 0, sizeof(page_data));				
				//write_page(0, page_data);
				udelay(10);
				//write_page(1, page_data);
				ds_reset();							
				//PAGE 1 structure	
				//[0-5] 6 byte MAC address, [10]-- model_id, [20]--production_name_id, [31]--CRC8 of [0]~[30])				
				memcpy(page_data, &chip_ds_msg.page_1_data[0], 32);
				if(!write_page(1, page_data))
				{
					printk(KERN_ERR"INIT_CHIP_PRODUCTION_MAC Write page 1 data to chip FAIL!\n");
					return -EACCES;						
				}						

}

void readMac()
{
	ret = copy_from_user(&chip_ds_msg, (struct ds_msg __user *)arg, sizeof(chip_ds_msg));
	if(ret != 0)			
	{
		printk(KERN_ERR"COMPUTE_READ_MAC copy from user FAIL!\n");
		return -EACCES;				
	}								
	page_num = chip_ds_msg.page_num;
	anon = chip_ds_msg.anon;
	memcpy(challenge, &chip_ds_msg.challenge[0], 32);				
	ret = compute_read_page_mac(page_num, challenge, read_mac, anon);				
	if(ret == 0)
	{
		printk(KERN_ERR"COMPUTE_READ_MAC read_auth_page FAIL!\n");
		return -EACCES;
	}
	memcpy(&chip_ds_msg.mac[0], read_mac, 32);				
	ret = copy_to_user((struct ds_msg __user *)arg, &chip_ds_msg.rom_id[0], sizeof(chip_ds_msg));
	if(ret != 0)			
	{
		printk(KERN_ERR"COMPUTE_READ_MAC copy to user FAIL!\n");
		return -EACCES;				
	}			

}

void readMemo()
{
	unsigned char	rom_id[8];	
	unsigned char	secret[8];			
	unsigned char 	challenge[32];
	unsigned char	page_data[32];		//memory page data, 32 byte
	unsigned char 	personality_byte[4];//
	unsigned char   read_mac[32];		//MAC from chip
	unsigned char 	page_num;			//page number
	unsigned char 	lock_flag;			//1--lock chip secret forever
	unsigned char 	lock_magic;				
	unsigned char   anon;				//anonymous flag. 1--anonymous mode, 0-NOT anonymous mode
	int i;
	int ret;

				ret = copy_from_user(&chip_ds_msg, (struct ds_msg __user *)arg, sizeof(chip_ds_msg));
				if(ret != 0)			
				{
					printk(KERN_ERR"READ_CHIP_MEM copy from user FAIL!\n");
					return -EACCES;				
				}						
				memset(page_data, 0, sizeof(page_data));				
				ds_read_rom_id(rom_id);
				if(!read_page(0, page_data))
				{
					printk(KERN_ERR"READ_CHIP_MEM read page 0 FAIL!\n");
					return -EACCES;				
				}	
				memcpy(&chip_ds_msg.page_0_data[0], page_data, 32);
				if(!read_page(1, page_data))
				{
					printk(KERN_ERR"READ_CHIP_MEM read page 1 FAIL!\n");
					return -EACCES;				
				}	
				memcpy(&chip_ds_msg.page_1_data[0], page_data, 32);
				if(!read_status_personality(personality_byte))
				{
					printk(KERN_ERR"READ_CHIP_MEM read personality FAIL!\n");
					return -EACCES;				
				}
				chip_ds_msg.man_id[0] = personality_byte[2];
				chip_ds_msg.man_id[1] = personality_byte[3];				
				
				ret = copy_to_user((struct ds_msg __user *)arg, &chip_ds_msg.rom_id[0], sizeof(chip_ds_msg));
				if(ret != 0)			
				{
					printk(KERN_ERR"READ_CHIP_MEM copy to user FAIL!\n");
					return -EACCES;				
				}			
}


